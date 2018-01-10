/******************************************************************************

 @file  heart_rate.c

 @brief This file contains the Heart Rate sample application for use with the
        CC26xx Bluetooth Low Energy Protocol Stack.

 Group: WCS, BTS
 Target Device: CC2650, CC2640, CC1350

 ******************************************************************************
 
 Copyright (c) 2011-2016, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 Release Name: ble_sdk_2_02_01_18
 Release Date: 2016-10-26 15:20:04
 *****************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include <string.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>

#include "bcomdef.h"
#include "linkdb.h"
#include "gatt.h"
#include "gapgattserver.h"
#include "gattservapp.h"
#include "peripheral.h"
#include "gapbondmgr.h"
#include "gatt_profile_uuid.h"
#include "../PROFILES/devinfoservice.h"
#include "../PROFILES/heartrateservice.h"
#include "../PROFILES/battservice.h"
#include "../PROFILES/Apedometer.h"
#include "../PROFILES/Asimple_gatt_profile.h"
#include "osal_snv.h"
#include "icall_apimsg.h"

#include "util.h"
#include "board_key.h"
#include "board.h"
#include "middleComunication.h"
// #include "wearableDevice.h"
#include "heart_rate.h"

/*********************************************************************
 * MACROS
 */

// Convert BPM to RR-Interval for data simulation purposes.
#define HEARTRATE_BPM2RR(bpm)            ((uint16) 60 * 1024 / (uint16) (bpm))

/*********************************************************************
 * CONSTANTS
 */

// Fast advertising interval in 625us units.
#define DEFAULT_FAST_ADV_INTERVAL                       32

// Duration of fast advertising duration in ms.
#define DEFAULT_FAST_ADV_DURATION                       30000

// Slow advertising interval in 625us units.
#define DEFAULT_SLOW_ADV_INTERVAL                       1600

// Slow advertising duration in ms (set to 0 for continuous advertising).
#define DEFAULT_SLOW_ADV_DURATION                       0

// How often to perform heart rate periodic event.
#define DEFAULT_HEARTRATE_PERIOD                        2000

// Whether to enable automatic parameter update request when a connection is
// formed.
#define DEFAULT_ENABLE_UPDATE_REQUEST                   GAPROLE_LINK_PARAM_UPDATE_WAIT_BOTH_PARAMS

// Minimum connection interval (units of 1.25ms) if automatic parameter update
// request is enabled.
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL               200

// Maximum connection interval (units of 1.25ms) if automatic parameter update
// request is enabled.
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL               1600

// Slave latency to use if automatic parameter update request is enabled.
#define DEFAULT_DESIRED_SLAVE_LATENCY                   1

// Supervision timeout value (units of 10ms) if automatic parameter update
// request is enabled.
#define DEFAULT_DESIRED_CONN_TIMEOUT                    1000

// Battery level is critical when it is less than this %
#define DEFAULT_BATT_CRITICAL_LEVEL                     6

// Battery measurement period in ms
#define DEFAULT_BATT_PERIOD                             1000

// How often to perform periodic event (in msec)
#define SBP_PERIODIC_EVT_PERIOD                         1000

// How often to perform periodic event (in msec)
#define PEDOMETER_PERIODIC_EVT_PERIOD                   1000


// Arbitrary values used to simulate measurements.
#define HEARTRATE_BPM_DEFAULT                           73
#define HEARTRATE_BPM_MAX                               80
#define HEARTRATE_ENERGY_INCREMENT                      10
#define HEARTRATE_FLAGS_IDX_MAX                         7

// Task configuration
#define HEARTRATE_TASK_PRIORITY                         1
#define HEARTRATE_TASK_STACK_SIZE                       1024

// Internal events for RTOS application.
// Peripheral State Change Event.
#define HEARTRATE_STATE_CHANGE_EVT                      (uint16_t)(1 << 0)

// Heart Rate events.
#define HEARTRATE_MEAS_EVT                              (uint16_t)(1 << 1)
#define HEARTRATE_MEAS_PERIODIC_EVT                     (uint16_t)(1 << 2)

// Battery Event.
#define HEARTRATE_BATT_EVT                              (uint16_t)(1 << 3)
#define HEARTRATE_BATT_PERIODIC_EVT                     (uint16_t)(1 << 4)

// Pedometer events 
#define PEDO_CHAR_CHANGE_EVT                            (uint16_t)(1 << 5)

// Simple peripheral events.
#define SBP_CHAR_CHANGE_EVT                             (uint16_t)(1 << 6)
#define SBP_PERIODIC_EVT                                (uint16_t)(1 << 7)

// Key Press events.
#define HEARTRATE_KEY_CHANGE_EVT                        (uint16_t)(1 << 8)

#define HEARTRATE_MEAS_LEN                              9
#define HEARTRATE_RAWMEAS_LEN                           9

/*********************************************************************
 * TYPEDEFS
 */

// App event passed from profiles.
typedef struct
{
  appEvtHdr_t hdr;  // Event header
} heartRateEvt_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// Entity ID globally used to check for source and/or destination of messages.
static ICall_EntityID selfEntity;

// HeartRate_semaphore globally used to post events to the application thread.
static ICall_Semaphore sem;

// Clock instances for internal periodic events.
static Clock_Struct measPerClock;
static Clock_Struct rawMeasPerClock;
static Clock_Struct battPerClock;
static Clock_Struct periodicClock;
static Clock_Struct pedometerClock;

// Queue object used for app messages
static Queue_Struct appMsg;
static Queue_Handle appMsgQueue;

// events flag for internal application events.
static uint16_t events;

// Task configuration
Task_Struct HeartRate_task;
Char HeartRate_taskStack[HEARTRATE_TASK_STACK_SIZE];

// Profile state parameter.
gaprole_States_t gapProfileState = GAPROLE_INIT;

// GAP Profile - Name attribute for SCAN RSP data (max size = 31 bytes).
static uint8_t scanData[] =
{
  // Complete name.
  0x12,   // length of this data
  GAP_ADTYPE_LOCAL_NAME_COMPLETE,
  'H',
  'E',
  'A',
  'R',
  'T',
  ' ',
  'R',
  'a',
  't',
  'e',
  ' ',
  'S',
  'e',
  'n',
  's',
  'o',
  'R'
};

//GAP - Advertisement data (max size = 31 bytes)
static uint8_t advertData[] =
{
  // Flags
  0x02,
  GAP_ADTYPE_FLAGS,
  GAP_ADTYPE_FLAGS_GENERAL | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
  // Service UUIDs
  0x05,
  GAP_ADTYPE_16BIT_MORE,
  LO_UINT16(HEARTRATE_SERV_UUID),
  HI_UINT16(HEARTRATE_SERV_UUID),
  LO_UINT16(BATT_SERV_UUID),
  HI_UINT16(BATT_SERV_UUID)
};

// Device name attribute value.
static uint8_t attDeviceName[GAP_DEVICE_NAME_LEN] = "HEART Rate SensoR";

// GAP connection handle.
static uint16_t gapConnHandle;

// Components of heart rate measurement structure.
uint8_t heartRateBpm = HEARTRATE_BPM_DEFAULT;
static uint16_t heartRateEnergyLvl = 0;
static uint16_t heartRateRrInterval = HEARTRATE_BPM2RR(HEARTRATE_BPM_DEFAULT);
static uint16_t heartRateRrInterval2 = HEARTRATE_BPM2RR(HEARTRATE_BPM_DEFAULT);

// Advertising user-cancelled state.
static bool advCancelled = FALSE;

// Index for array below.
static uint8_t flagsIdx = 2;

// Flags for simulated measurements.
static const uint8_t heartRateflags[HEARTRATE_FLAGS_IDX_MAX] =
{
  HEARTRATE_FLAGS_CONTACT_NOT_SUP,
  HEARTRATE_FLAGS_CONTACT_NOT_DET,
  HEARTRATE_FLAGS_CONTACT_DET | HEARTRATE_FLAGS_ENERGY_EXP,
  HEARTRATE_FLAGS_CONTACT_DET | HEARTRATE_FLAGS_RR,
  HEARTRATE_FLAGS_CONTACT_DET | HEARTRATE_FLAGS_ENERGY_EXP | HEARTRATE_FLAGS_RR,
  (HEARTRATE_FLAGS_FORMAT_UINT16 | HEARTRATE_FLAGS_CONTACT_DET |
  HEARTRATE_FLAGS_ENERGY_EXP | HEARTRATE_FLAGS_RR),
  0x00
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */

//********************************************************************

// Task functions and message processing.
static void HeartRate_init(void);
static void HeartRate_taskFxn(UArg a0, UArg a1);
static void HeartRate_processStackMsg(ICall_Hdr *pMsg);
static void HeartRate_processGattMsg(gattMsgEvent_t *pMsg);
static void HeartRate_processAppMsg(heartRateEvt_t *pMsg);
static uint8_t HeartRate_enqueueMsg(uint8_t event, uint8_t state);
static void HeartRate_clockHandler(UArg arg);
static void HeartRate_measPerTask(void);
static void HeartRate_battPerTask(void);
static void HeartRate_measNotify(void);
static bool HeartRate_toggleAdvertising(void);


// Events and callbacks for profiles and keys.
static void HeartRate_battCB(uint8_t event);
static void HeartRate_battEvt(uint8_t event);
static void HeartRate_serviceCB(uint8_t event);
static void HeartRate_heartRateEvt(uint8_t event);
static void HeartRate_keyPressHandler(uint8_t keys);
static void HeartRate_handleKeys(uint8_t shift, uint8_t keys);
static void HeartRate_stateChangeCB(gaprole_States_t newState);
static void HeartRate_stateChangeEvt(gaprole_States_t pEvent);


//********************************************************************

static void SimpleBLEPeripheral_processCharValueChangeEvt(uint8_t paramID);
static void SimpleBLEPeripheral_performPeriodicTask(void);
static void Pedometer_performPeriodicTask(void);
static void SimpleBLEPeripheral_charValueChangeCB(uint8_t paramID);
static void Pedometer_pedometerCB(uint8_t paramID);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// GAP Role Callbacks.
static gapRolesCBs_t heartRatePeripheralCB =
{
  HeartRate_stateChangeCB  // Profile State Change Callbacks.
};

// Bond Manager Callbacks.
static const gapBondCBs_t heartRateBondCB =
{
  NULL,                    // Passcode callback.
  NULL                     // Pairing state callback.
};

static simpleProfileCBs_t simpleProfile_ProfileCBs =
{
  SimpleBLEPeripheral_charValueChangeCB // Characteristic value change callback
};

static pedometerCBs_t Pedometer_pedometerCBs =
{
 Pedometer_pedometerCB // Characteristic value change callback
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      HeartRate_createTask
 *
 * @brief   Task creation function for the Heart Rate.
 *
 * @param   none
 *
 * @return  none
 */
void HeartRate_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = HeartRate_taskStack;
  taskParams.stackSize = HEARTRATE_TASK_STACK_SIZE;
  taskParams.priority = HEARTRATE_TASK_PRIORITY;

  Task_construct(&HeartRate_task, HeartRate_taskFxn, &taskParams, NULL);
}

/*********************************************************************
 * @fn      HeartRate_init
 *
 * @brief   Initialization function for the Heart Rate application thread.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notification ...).
 *
 * @param   none
 *
 * @return  none
 */
void HeartRate_init(void)
{
  // ******************************************************************
  // N0 STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
  // ******************************************************************
  // Register the current thread as an ICall dispatcher application
  // so that the application can send and receive messages.
  ICall_registerApp(&selfEntity, &sem);

  // Hard code the DB Address till CC2650 board gets its own IEEE address.
  //uint8_t bdAddress[B_ADDR_LEN] = { 0x22, 0x22, 0x22, 0x22, 0x22, 0x22 };
  //HCI_EXT_SetBDADDRCmd(bdAddress);

  // Set device's Sleep Clock Accuracy
  //HCI_EXT_SetSCACmd(40);

  // Create an RTOS queue for message from profile to be sent to app.
  appMsgQueue = Util_constructQueue(&appMsg);

  // Create one-shot clocks for internal periodic events.
  Util_constructClock(&measPerClock, HeartRate_clockHandler,
                      DEFAULT_HEARTRATE_PERIOD, 0, false,
                      HEARTRATE_MEAS_PERIODIC_EVT);

  Util_constructClock(&rawMeasPerClock, HeartRate_clockHandler,
                      DEFAULT_HEARTRATE_PERIOD, 0, false,
                      HEARTRATE_MEAS_PERIODIC_EVT);

  Util_constructClock(&battPerClock, HeartRate_clockHandler,
                      DEFAULT_BATT_PERIOD, 0, false,
                      HEARTRATE_BATT_PERIODIC_EVT);

  Util_constructClock(&periodicClock, HeartRate_clockHandler,
                      SBP_PERIODIC_EVT_PERIOD, 0, false, 
                      SBP_PERIODIC_EVT);

  Util_constructClock(&pedometerClock, HeartRate_clockHandler,
                      PEDOMETER_PERIODIC_EVT_PERIOD, 0, false, 
                      PEDO_CHAR_CHANGE_EVT);

  // Setup the GAP Peripheral Role Profile.
  {
    uint8_t initial_advertising_enable = TRUE;

    // By setting this to zero, the device will go into the waiting state after
    // being discoverable for 30.72 second, and will not being advertising again
    // until the enabler is set back to TRUE.
    uint16_t gapRole_AdvertOffTime = 0;

    uint8_t enable_update_request  = DEFAULT_ENABLE_UPDATE_REQUEST;
    uint16_t desired_min_interval  = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
    uint16_t desired_max_interval  = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
    uint16_t desired_slave_latency = DEFAULT_DESIRED_SLAVE_LATENCY;
    uint16_t desired_conn_timeout  = DEFAULT_DESIRED_CONN_TIMEOUT;

    // Set the GAP Role Parameters.
    GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
                          &initial_advertising_enable);
    GAPRole_SetParameter(GAPROLE_ADVERT_OFF_TIME, sizeof(uint16_t),
                          &gapRole_AdvertOffTime);

    GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof (scanData),
                          scanData);
    GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData),
                          advertData);

    GAPRole_SetParameter(GAPROLE_PARAM_UPDATE_ENABLE, sizeof(uint8_t),
                          &enable_update_request);
    GAPRole_SetParameter(GAPROLE_MIN_CONN_INTERVAL, sizeof(uint16_t),
                          &desired_min_interval);
    GAPRole_SetParameter(GAPROLE_MAX_CONN_INTERVAL, sizeof(uint16_t),
                          &desired_max_interval);
    GAPRole_SetParameter(GAPROLE_SLAVE_LATENCY, sizeof(uint16_t),
                          &desired_slave_latency);
    GAPRole_SetParameter(GAPROLE_TIMEOUT_MULTIPLIER, sizeof(uint16_t),
                          &desired_conn_timeout);
  }

  // Set the GAP Characteristics.
  GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName);

  // Setup the GAP Bond Manager.
  {
    uint32 passkey = 0; // passkey "000000"
    uint8_t pairMode = GAPBOND_PAIRING_MODE_WAIT_FOR_REQ;
    uint8_t mitm = FALSE;
    uint8_t ioCap = GAPBOND_IO_CAP_DISPLAY_ONLY;
    uint8_t bonding = TRUE;
    GAPBondMgr_SetParameter(GAPBOND_DEFAULT_PASSCODE, sizeof (uint32),
                             &passkey);
    GAPBondMgr_SetParameter(GAPBOND_PAIRING_MODE, sizeof (uint8_t),
                             &pairMode);
    GAPBondMgr_SetParameter(GAPBOND_MITM_PROTECTION, sizeof (uint8_t), &mitm);
    GAPBondMgr_SetParameter(GAPBOND_IO_CAPABILITIES, sizeof (uint8_t),
                             &ioCap);
    GAPBondMgr_SetParameter(GAPBOND_BONDING_ENABLED, sizeof (uint8_t),
                             &bonding);
  }

  // Initialize GATT attributes.
  GGS_AddService(GATT_ALL_SERVICES);         // GAP
  GATTServApp_AddService(GATT_ALL_SERVICES); // GATT attributes

  // Add device info service.
  DevInfo_AddService();

  {
    uint8_t devInfoMfrName[DEVINFO_STR_ATTR_LEN+1] = "MEMSTECH";
    DevInfo_SetParameter(DEVINFO_MANUFACTURER_NAME, DEVINFO_STR_ATTR_LEN, devInfoMfrName);
  }
  // Add heart rate service.
  HeartRate_AddService(GATT_ALL_SERVICES);
  // Add battery service.
  Batt_AddService();
  // Add Simple Profile service.
  SimpleProfile_AddService(GATT_ALL_SERVICES); // Simple GATT Profile

  // Add Pedometer service.
   Pedometer_AddService(GATT_ALL_SERVICES);

  // Setup the Heart Rate Characteristic Values.
  {
    uint8_t sensLoc = HEARTRATE_SENS_LOC_WRIST;
    HeartRate_SetParameter(HEARTRATE_SENS_LOC, sizeof (uint8_t), &sensLoc);
  }

  // Setup Battery Characteristic Values.
  {
    uint8_t critical = DEFAULT_BATT_CRITICAL_LEVEL;
    Batt_SetParameter(BATT_PARAM_CRITICAL_LEVEL, sizeof(uint8_t), &critical);
  }
  
  // Setup Simple Profile Characteristic Values.
  {
    uint8_t charValue3 = 3;
    uint8_t charValue4[5] = {0x01, 0x02, 0x03, 0x04, 0x05};
    uint8_t charValue5[SIMPLEPROFILE_CHAR5_LEN] = { 1, 2, 3, 4, 5 };
    uint8_t charValue6 = 6;

    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR3, sizeof(uint8_t),
                               &charValue3);
    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR4, sizeof(charValue4),
                               &charValue4);
    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR5, SIMPLEPROFILE_CHAR5_LEN,
                               charValue5);
    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR6, sizeof(uint8_t),
                               &charValue6);
  }

  // Setup Pedometer Characteristic Values.
  {
    // uint16_t charValue = 60;
    //  Pedometer_SetParameter(PEDOMETER_CHAR1, sizeof (uint16_t), &charValue);
  }

  // Register for Heart Rate service callback.
  HeartRate_Register(&HeartRate_serviceCB);

  // Register for Battery service callback.
  Batt_Register (&HeartRate_battCB);
  {
    // uint8_t percent = 0;
    // Batt_MeasLevel();
    // Batt_GetParameter(BATT_PARAM_LEVEL, &percent);
    // // updateBatteryInDevice(percent);
  }
  // Util_startClock(&battPerClock);

  // Register for Simple Profile service callback.
  SimpleProfile_RegisterAppCBs(&simpleProfile_ProfileCBs);

  // Register for Simple Profile service callback.
  Pedometer_RegisterAppCBs(&Pedometer_pedometerCBs);

  // Start the Device.
  GAPRole_StartDevice(&heartRatePeripheralCB);

  // Start the Bond Manager.
  GAPBondMgr_Register((gapBondCBs_t *)&heartRateBondCB);
}

/*********************************************************************
 * @fn      HeartRate_taskFxn
 *
 * @brief   Application task entry point for the Heart Rate.
 *
 * @param   none
 *
 * @return  none
 */
static void HeartRate_taskFxn(UArg a0, UArg a1)
{
  // Initialize the application.
  HeartRate_init();

  // Application main loop.
  for(;;)
  {
    // Waits for a signal to the semaphore associated with the calling thread.
    // Note that the semaphore associated with a thread is signaled when a
    // message is queued to the message receive queue of the thread or when the
    // ICall_signal() function is called on the thread's respective semaphore.
    ICall_Errno errno = ICall_wait(ICALL_TIMEOUT_FOREVER);

    if (errno == ICALL_ERRNO_SUCCESS)
    {
      ICall_EntityID dest;
      ICall_ServiceEnum src;
      ICall_HciExtEvt *pMsg = NULL;

      if (ICall_fetchServiceMsg(&src, &dest, (void **)&pMsg) == ICALL_ERRNO_SUCCESS)
      {
        uint8 safeToDealloc = TRUE;
        if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity))
        {
          // Process inter-task message.
          HeartRate_processStackMsg((ICall_Hdr *)pMsg);
        }

        if (pMsg)
        {
          ICall_freeMsg(pMsg);
        }
      }
    }

    // If RTOS queue is not empty, process app message.
    while (!Queue_empty(appMsgQueue))
    {
      heartRateEvt_t *pMsg1 = (heartRateEvt_t*)Util_dequeueMsg(appMsgQueue); // diff between pMsg1 and above pMsg (bug)
      if (pMsg1)
      {
        // Process message.
        HeartRate_processAppMsg(pMsg1);

        // Free the space from the message.
        ICall_free(pMsg1);
      }
    }

    // Heart rate service periodic task.
    if (events & HEARTRATE_MEAS_PERIODIC_EVT)
    {
      events &= ~HEARTRATE_MEAS_PERIODIC_EVT;

      HeartRate_measPerTask();
    }

    // Battery service periodic task.
    if (events & HEARTRATE_BATT_PERIODIC_EVT)
    {
      events &= ~HEARTRATE_BATT_PERIODIC_EVT;

      HeartRate_battPerTask();
    }

    if (events & SBP_PERIODIC_EVT)
    {
      events &= ~SBP_PERIODIC_EVT;

      Util_startClock(&periodicClock);

      // Perform periodic application task
      SimpleBLEPeripheral_performPeriodicTask();
    }

    if (events & PEDO_CHAR_CHANGE_EVT)
    {
      events &= ~PEDO_CHAR_CHANGE_EVT;

      Util_startClock(&pedometerClock);
      // Perform periodic application task
      Pedometer_performPeriodicTask();
    }

  }
}

/*********************************************************************
 * @fn      HeartRate_processStackMsg
 *
 * @brief   Process an incoming stack message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void HeartRate_processStackMsg(ICall_Hdr *pMsg)
{
  switch (pMsg->event)
  {
    case GATT_MSG_EVENT:
      HeartRate_processGattMsg((gattMsgEvent_t *)pMsg);
      break;

    default:
      // Do nothing.
      break;
  }
}

/*********************************************************************
 * @fn      HeartRate_processGattMsg
 *
 * @brief   Process GATT messages.
 *
 * @param   pMsg - pointer the the GATT message.
 *
 * @return  none
 */
static void HeartRate_processGattMsg(gattMsgEvent_t *pMsg)
{
  GATT_bm_free(&pMsg->msg, pMsg->method);
}

/*********************************************************************
 * @fn      HeartRate_processAppMsg
 *
 * @brief   Process an incoming callback from a profile.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void HeartRate_processAppMsg(heartRateEvt_t *pMsg)
{
  switch (pMsg->hdr.event)
  {
    case HEARTRATE_STATE_CHANGE_EVT:
      HeartRate_stateChangeEvt((gaprole_States_t)pMsg->hdr.state);
      break;

    case HEARTRATE_KEY_CHANGE_EVT:
      HeartRate_handleKeys(0, pMsg->hdr.state);
      break;

    case HEARTRATE_MEAS_EVT:
      HeartRate_heartRateEvt(pMsg->hdr.state);
      break;

    case HEARTRATE_BATT_EVT:
      HeartRate_battEvt(pMsg->hdr.state);

    case SBP_CHAR_CHANGE_EVT:
      SimpleBLEPeripheral_processCharValueChangeEvt(pMsg->hdr.state);
      break;

    case PEDO_CHAR_CHANGE_EVT:
      SimpleBLEPeripheral_processCharValueChangeEvt(pMsg->hdr.state);
      break;

    default:
      // Do nothing.
      break;
  }
}

/*********************************************************************
 * @fn      HeartRate_keyPressHandler
 *
 * @brief   Key event handler function.
 *
 * @param   a0 - ignored
 *
 * @return  none
 */
static void HeartRate_keyPressHandler(uint8_t keys)
{
  // Enqueue the event.
  HeartRate_enqueueMsg(HEARTRATE_KEY_CHANGE_EVT, keys);
}

/*********************************************************************
 * @fn      HeartRate_handleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
static void HeartRate_handleKeys(uint8_t shift, uint8_t keys)
{
  // Up key.
  if (keys & KEY_UP)
  {
    // Set simulated measurement flag index.
    if (++flagsIdx == HEARTRATE_FLAGS_IDX_MAX)
    {
      flagsIdx = 0;
    }
  }

  // Left key.
  if (keys & KEY_LEFT)
  {
    // If not in a connection, toggle advertising on and off.
    if(gapProfileState != GAPROLE_CONNECTED)
    {
      // Set fast advertising interval for user-initiated connections.
      GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MIN, DEFAULT_FAST_ADV_INTERVAL);
      GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MAX, DEFAULT_FAST_ADV_INTERVAL);
      GAP_SetParamValue(TGAP_GEN_DISC_ADV_MIN, DEFAULT_FAST_ADV_DURATION);

      // Toggle GAP advertisement status.
      // Set flag if advertising was cancelled.
     // if (HeartRate_toggleAdvertising() == FALSE)
     // {
     //   advCancelled = TRUE;
     // }
    }
  }
}

/*********************************************************************
 * @fn      HeartRate_toggleAdvertising
 *
 * @brief   Toggle advertising state.
 *
 * @param   none
 *
 * @return  status - TRUE if advertising, FALSE otherwise.
 */
static bool HeartRate_toggleAdvertising(void)
{
  uint8_t advState;

  // Find the current GAP advertisement status.
  GAPRole_GetParameter(GAPROLE_ADVERT_ENABLED, &advState);

  // Get the opposite state.
  advState = !advState;

  // Change the GAP advertisement status to opposite of current status.
  GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &advState);

  return advState;
}

/*********************************************************************
 * @fn      HeartRate_measNotify
 *
 * @brief   Prepare and send a heart rate measurement notification.
 *
 * @return  none
 */
static void HeartRate_measNotify(void)
{
  // Change heart rate measurement
  uint8_t *value = getHeartRateRawMeasurement();
  HeartRate_SetParameter(HEARTRATE_MEASURE, 13, value);

  // Change heart rate value 
  uint8_t *value1 = getHeartRateValueMeasurement();
  HeartRate_SetParameter(HEARTRATE_VALUE, 3, value1);

}

/*********************************************************************
 * @fn      HeartRate_stateChangeCB
 *
 * @brief   Callback from GAP Role indicating a role state change.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void HeartRate_stateChangeCB(gaprole_States_t newState)
{
  // Enqueue the event.
  HeartRate_enqueueMsg(HEARTRATE_STATE_CHANGE_EVT, newState);
}

/*********************************************************************
 * @fn      HeartRate_stateChangeEvt
 *
 * @brief   Notification from the profile of a state change.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void HeartRate_stateChangeEvt(gaprole_States_t newState)
{
  // If no change to the GAP Role state has occurred
  if (gapProfileState == newState)
  {
    return;
  }

  // If connected
  if (newState == GAPROLE_CONNECTED)
  {
    Util_startClock(&periodicClock);
    Util_startClock(&pedometerClock);
    // Get connection handle.
    GAPRole_GetParameter(GAPROLE_CONNHANDLE, &gapConnHandle);
  }
  // If disconnected
  else if (gapProfileState == GAPROLE_CONNECTED && newState != GAPROLE_CONNECTED)
  {
    // Stop periodic measurement of heart rate.
    Util_stopClock(&measPerClock);
    Util_stopClock(&rawMeasPerClock);
    Util_stopClock(&periodicClock);
    Util_stopClock(&pedometerClock);

    if (newState == GAPROLE_WAITING_AFTER_TIMEOUT)
    {
      // Link loss timeout-- use fast advertising
      GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MIN, DEFAULT_FAST_ADV_INTERVAL);
      GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MAX, DEFAULT_FAST_ADV_INTERVAL);
      GAP_SetParamValue(TGAP_GEN_DISC_ADV_MIN, DEFAULT_FAST_ADV_DURATION);
    }
    else
    {
      // Else use slow advertising
      GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MIN, DEFAULT_SLOW_ADV_INTERVAL);
      GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MAX, DEFAULT_SLOW_ADV_INTERVAL);
      GAP_SetParamValue(TGAP_GEN_DISC_ADV_MIN, DEFAULT_SLOW_ADV_DURATION);
    }

    // Enable advertising.
   // HeartRate_toggleAdvertising();
  }
  // If advertising stopped
  else if (gapProfileState == GAPROLE_ADVERTISING &&
            newState == GAPROLE_WAITING)
  {
    // If advertising stopped by user
    if (advCancelled)
    {
      // Disable advertising.
      advCancelled = FALSE;
    }
    // Else if fast advertising switch to slow
    else if (GAP_GetParamValue(TGAP_GEN_DISC_ADV_INT_MIN) == DEFAULT_FAST_ADV_INTERVAL)
    {
      GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MIN, DEFAULT_SLOW_ADV_INTERVAL);
      GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MAX, DEFAULT_SLOW_ADV_INTERVAL);
      GAP_SetParamValue(TGAP_GEN_DISC_ADV_MIN, DEFAULT_SLOW_ADV_DURATION);

      // Enable advertising.
      // HeartRate_toggleAdvertising();
    }
  #if AUTO_ADV
    else
    {
      // Test mode: continue advertising.
      // HeartRate_toggleAdvertising();
    }
  #endif //AUTO_ADV
  }
  #if AUTO_ADV
  else if (newState == GAPROLE_WAITING_AFTER_TIMEOUT)
  {
    // Test mode: continue advertising.
    // HeartRate_toggleAdvertising();
  }
  #endif //AUTO_ADV
  // If started
  else if (newState == GAPROLE_STARTED)
  {
    // Set the system ID from the bd addr.
    uint8_t systemId[DEVINFO_SYSTEM_ID_LEN];
    GAPRole_GetParameter(GAPROLE_BD_ADDR, systemId);

    // Shift three bytes up.
    systemId[7] = systemId[5];
    systemId[6] = systemId[4];
    systemId[5] = systemId[3];

    // Set middle bytes to zero.
    systemId[4] = 0;
    systemId[3] = 0;

    // Pass systemId to the Device Info service.
    DevInfo_SetParameter(DEVINFO_SYSTEM_ID, DEVINFO_SYSTEM_ID_LEN, systemId);
  }

  // Update GAP profile state.
  gapProfileState = newState;
}

/*********************************************************************
 * @fn      HeartRate_serviceCB
 *
 * @brief   Callback function for heart rate service.
 *
 * @param   event - service event
 *
 * @return  none
 */
static void HeartRate_serviceCB(uint8_t event)
{
  // Enqueue the message.
  HeartRate_enqueueMsg(HEARTRATE_MEAS_EVT, event);
}

/*********************************************************************
 * @fn      HeartRate_heartRateEvt
 *
 * @brief   event handler for heart rate service callbacks.
 *
 * @param   event - service event
 *
 * @return  none
 */
static void HeartRate_heartRateEvt(uint8_t event)
{
  if (event == HEARTRATE_MEAS_NOTI_ENABLED)
  {
    // If connected start periodic measurement.
    if (gapProfileState == GAPROLE_CONNECTED)
    {
      Util_startClock(&measPerClock);
      Util_startClock(&rawMeasPerClock);
    }
  }
  else if (event == HEARTRATE_MEAS_NOTI_DISABLED)
  {
    // Stop periodic measurement.
    Util_stopClock(&measPerClock);
    Util_stopClock(&rawMeasPerClock);
  }
  else if (event == HEARTRATE_COMMAND_SET)
  {
    // Reset energy expended.
    heartRateEnergyLvl = 0;
  }
}

/*********************************************************************
 * @fn      HeartRate_battCB
 *
 * @brief   Callback function for battery service.
 *
 * @param   event - service event
 *
 * @return  none
 */
static void HeartRate_battCB(uint8_t event)
{
  // Enqueue the message.
  HeartRate_enqueueMsg(HEARTRATE_BATT_EVT, event);
}

/*********************************************************************
 * @fn      HeartRate_battEvt
 *
 * @brief   Event handler for battery service callbacks.
 *
 * @param   event - service event
 *
 * @return  none
 */
static void HeartRate_battEvt(uint8_t event)
{
  if (event == BATT_LEVEL_NOTI_ENABLED)
  {
    // If connected start periodic measurement.
    if (gapProfileState == GAPROLE_CONNECTED)
    {
      Util_startClock(&battPerClock);
    }
  }
  else if (event == BATT_LEVEL_NOTI_DISABLED)
  {
    // Stop periodic measurement.
    Util_stopClock(&battPerClock);
  }
}


/*********************************************************************
 * @fn      HeartRate_clockHandler
 *
 * @brief   Handler function for clock timeouts.
 *
 * @param   none
 *
 * @return  none
 */
static void HeartRate_clockHandler(UArg arg)
{
  // Store the event.
  events |= arg;

  // Wake up the application.
  Semaphore_post(sem);
}

/*********************************************************************
 * @fn      HeartRate_perTask
 *
 * @brief   Perform a periodic heart rate measurement.
 *
 * @param   none
 *
 * @return  none
 */
static void HeartRate_measPerTask(void)
{
  if (gapProfileState == GAPROLE_CONNECTED)
  {
    // Send heart rate measurement notification.
    HeartRate_measNotify();

    // Restart timer.
    Util_startClock(&measPerClock);
    Util_startClock(&rawMeasPerClock);
  }
}

/*********************************************************************
 * @fn      HeartRate_battPerTask
 *
 * @brief   Perform a periodic task for battery measurement.
 *
 * @param   none
 *
 * @return  none
 */
static void HeartRate_battPerTask(void)
{
  // Perform battery level check
  uint8_t percent;
  Batt_MeasLevel();
  Batt_GetParameter(BATT_PARAM_LEVEL, &percent);
  // updateBatteryInDevice(percent);

  if (gapProfileState == GAPROLE_CONNECTED){
      // Send a notification
//      battNotifyLevel();
  }
  // Restart timer.
  Util_startClock(&battPerClock);
}




/*********************************************************************
 * @fn      SimpleBLEPeripheral_charValueChangeCB
 *
 * @brief   Callback from Simple Profile indicating a characteristic
 *          value change.
 *
 * @param   paramID - parameter ID of the value that was changed.
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_charValueChangeCB(uint8_t paramID)
{
  HeartRate_enqueueMsg(SBP_CHAR_CHANGE_EVT, paramID);
}


/*********************************************************************
 * @fn      Pedometer_pedometerCB
 *
 * @brief   Callback from Simple Profile indicating a characteristic
 *          value change.
 *
 * @param   paramID - parameter ID of the value that was changed.
 *
 * @return  None.
 */
static void Pedometer_pedometerCB(uint8_t paramID)
{
  HeartRate_enqueueMsg(PEDO_CHAR_CHANGE_EVT, paramID);
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_processCharValueChangeEvt
 *
 * @brief   Process a pending Simple Profile characteristic value change
 *          event.
 *
 * @param   paramID - parameter ID of the value that was changed.
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_processCharValueChangeEvt(uint8_t paramID)
{
  uint8_t newValue;
  switch(paramID)
  {
    case SIMPLEPROFILE_CHAR1:
      // uint8_t newDateTime[7] = {}; // ss,mm,hh,dd,mm,yy,cc
      // SimpleProfile_GetParameter(SIMPLEPROFILE_CHAR1, newDateTime);
      // updateTime(newDateTime);
      break;
    case SIMPLEPROFILE_CHAR2:
      // uint8_t newConfigProfile[3] = {};
      // SimpleProfile_GetParameter(SIMPLEPROFILE_CHAR2, newConfigProfile);
      // testing(50);
      // updateConfigProfile(newConfigProfile);
      break;
    case SIMPLEPROFILE_CHAR3:
      SimpleProfile_GetParameter(SIMPLEPROFILE_CHAR3, &newValue);
      break;
    case SIMPLEPROFILE_CHAR4:
      SimpleProfile_GetParameter(SIMPLEPROFILE_CHAR4, &newValue);
      break;

    default:
    // should not reach here!
    break;
  }
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_performPeriodicTask
 *
 * @brief   Perform a periodic application task. This function gets called
 *          every five seconds (SBP_PERIODIC_EVT_PERIOD). In this example,
 *          the value of the third characteristic in the SimpleGATTProfile
 *          service is retrieved from the profile, and then copied into the
 *          value of the the fourth characteristic.
 *
 * @param   None.
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_performPeriodicTask(void)
{
  uint8_t valueToCopy;

  // // Call to retrieve the value of the third characteristic in the profile
  if (SimpleProfile_GetParameter(SIMPLEPROFILE_CHAR3, &valueToCopy) == SUCCESS)
  {
  //   // Call to set that value of the fourth characteristic in the profile.
  //   // Note that if notifications of the fourth characteristic have been
  //   // enabled by a GATT client device, then a notification will be sent
  //   // every time this function is called.
    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR4, sizeof(valueToCopy), &valueToCopy);
  }
}


/*********************************************************************
 * @fn      Pedometer_performPeriodicTask
 *
 * @brief   
 *
 * @param   None.
 *
 * @return  None.
 */
static void Pedometer_performPeriodicTask(void)
{
  uint8_t *value = getPedometerValueMeasurement();
  Pedometer_SetParameter(PEDOMETER_CHAR1, 3, value);
}


/*********************************************************************
 * @fn      HeartRate_enqueueMsg
 *
 * @brief   Creates a message and puts the message in RTOS queue.
 *
 * @param   event - message event.
 * @param   state - message state.
 *
 * @return  TRUE or FALSE
 */
static uint8_t HeartRate_enqueueMsg(uint8_t event, uint8_t state)
{
  heartRateEvt_t *pMsg;

  // Create dynamic pointer to message.
  if (pMsg = ICall_malloc(sizeof(heartRateEvt_t)))
  {
    pMsg->hdr.event = event;
    pMsg->hdr.state = state;

    // Enqueue the message.
    return Util_enqueueMsg(appMsgQueue, sem, (uint8_t *)pMsg);
  }

  return FALSE;
}

/*********************************************************************
                    API for any Header files need
*********************************************************************/

bool get_BLEState(void){
  if(gapProfileState == GAPROLE_CONNECTED)
    return true;
  else
    return false;
}



/*********************************************************************
*********************************************************************/
