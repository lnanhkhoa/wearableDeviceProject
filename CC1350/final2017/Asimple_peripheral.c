/******************************************************************************

 @file  simple_peripheral.c

 @brief This file contains the Simple BLE Peripheral sample application for use
        with the CC2650 Bluetooth Low Energy Protocol Stack.

 Group: WCS, BTS
 Target Device: CC1350

 ******************************************************************************
 
 Copyright (c) 2013-2017, Texas Instruments Incorporated
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
 Release Name: ti-ble-2.3.1-stack-sdk_1_60_xx
 Release Date: 2017-12-16 12:03:51
 *****************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <string.h>
#include <math.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>

#include "hci_tl.h"
#include "gatt.h"
#include "linkdb.h"
#include "gapgattserver.h"
#include "gattservapp.h"
// #include "devinfoservice.h"
#include "PROFILES/Asimple_gatt_profile.h"

#if defined(FEATURE_OAD) || defined(IMAGE_INVALIDATE)
#include "oad_target.h"
#include "oad.h"
#endif //FEATURE_OAD || IMAGE_INVALIDATE

#include "peripheral.h"
#include "gapbondmgr.h"

#include "osal_snv.h"
#include "icall_apimsg.h"

#include "util.h"

#ifdef USE_RCOSC
#include "rcosc_calibration.h"
#endif //USE_RCOSC

#include "board.h"
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CCC26XX.h>
// #include <ti/drivers/UART.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/aon_batmon.h)
#include DeviceFamily_constructPath(driverlib/trng.h)

#include "bigtime.h"
#include "devices/dvMMA9553.h"
#include "devices/heart_rate_3click/AFE4404.h"
#include "OledDisplay/WDsDisplay.h"

#include "Asimple_peripheral.h"

/*********************************************************************
 * CONSTANTS
 */

// Advertising interval when device is discoverable (units of 625us, 160=100ms)
#define DEFAULT_ADVERTISING_INTERVAL          160

// Limited discoverable mode advertises for 30.72s, and then stops
// General discoverable mode advertises indefinitely
#define DEFAULT_DISCOVERABLE_MODE             GAP_ADTYPE_FLAGS_GENERAL

#ifndef FEATURE_OAD
// Minimum connection interval (units of 1.25ms, 80=100ms) if automatic
// parameter update request is enabled
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     80

// Maximum connection interval (units of 1.25ms, 800=1000ms) if automatic
// parameter update request is enabled
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     800
#else //!FEATURE_OAD
// Minimum connection interval (units of 1.25ms, 8=10ms) if automatic
// parameter update request is enabled
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     8

// Maximum connection interval (units of 1.25ms, 8=10ms) if automatic
// parameter update request is enabled
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     8
#endif // FEATURE_OAD

// Slave latency to use if automatic parameter update request is enabled
#define DEFAULT_DESIRED_SLAVE_LATENCY         0

// Supervision timeout value (units of 10ms, 1000=10s) if automatic parameter
// update request is enabled
#define DEFAULT_DESIRED_CONN_TIMEOUT          1000

// Whether to enable automatic parameter update request when a connection is
// formed
#define DEFAULT_ENABLE_UPDATE_REQUEST         GAPROLE_LINK_PARAM_UPDATE_INITIATE_BOTH_PARAMS

// Connection Pause Peripheral time value (in seconds)
#define DEFAULT_CONN_PAUSE_PERIPHERAL         6

// How often to perform periodic event (in msec)
#define SBP_PERIODIC_EVT_PERIOD               1000

#ifdef FEATURE_OAD
// The size of an OAD packet.
#define OAD_PACKET_SIZE                       ((OAD_BLOCK_SIZE) + 2)
#endif // FEATURE_OAD

// Task configuration
#define SBP_TASK_PRIORITY                     1
#define SBP_TASK_STACK_SIZE                   1024

/*********************************************************************
 * TYPEDEFS
 */

// App event passed from profiles.
typedef struct
{
  appEvtHdr_t hdr;  // event header.
} sbpEvt_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Display Interface


/*********************************************************************
 * LOCAL VARIABLES
 */

// Entity ID globally used to check for source and/or destination of messages
static ICall_EntityID selfEntity;

// Semaphore globally used to post events to the application thread
static ICall_Semaphore sem;

// Clock instances for internal periodic events.
static Clock_Struct periodicClock;
static Clock_Struct countSleepClockStruct;
static Clock_Struct mainClockStruct;
static Clock_Struct checkStateClockStruct;
static Clock_Struct measureClockStruct;

// Queue object used for app messages
static Queue_Struct appMsg;
static Queue_Handle appMsgQueue;

// events flag for internal application events.
static uint16_t events;

// Task configuration
Task_Struct sbpTask;
Char sbpTaskStack[SBP_TASK_STACK_SIZE];

// Profile state and parameters
//static gaprole_States_t gapProfileState = GAPROLE_INIT;

// GAP - SCAN RSP data (max size = 31 bytes)
static uint8_t scanRspData[] =
{
  // complete name
  0x14,   // length of this data
  GAP_ADTYPE_LOCAL_NAME_COMPLETE,
  'S',
  'i',
  'm',
  'p',
  'l',
  'e',
  'B',
  'L',
  'E',
  'P',
  'e',
  'r',
  'i',
  'p',
  'h',
  'e',
  'r',
  'a',
  'l',

  // connection interval range
  0x05,   // length of this data
  GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE,
  LO_UINT16(DEFAULT_DESIRED_MIN_CONN_INTERVAL),   // 100ms
  HI_UINT16(DEFAULT_DESIRED_MIN_CONN_INTERVAL),
  LO_UINT16(DEFAULT_DESIRED_MAX_CONN_INTERVAL),   // 1s
  HI_UINT16(DEFAULT_DESIRED_MAX_CONN_INTERVAL),

  // Tx power level
  0x02,   // length of this data
  GAP_ADTYPE_POWER_LEVEL,
  0       // 0dBm
};

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertisting)
static uint8_t advertData[] =
{
  // Flags; this sets the device to use limited discoverable
  // mode (advertises for 30 seconds at a time) instead of general
  // discoverable mode (advertises indefinitely)
  0x02,   // length of this data
  GAP_ADTYPE_FLAGS,
  DEFAULT_DISCOVERABLE_MODE | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

  // service UUID, to notify central devices what services are included
  // in this peripheral
#if !defined(FEATURE_OAD) || defined(FEATURE_OAD_ONCHIP)
  0x03,   // length of this data
#else //OAD for external flash
  0x05,  // lenght of this data
#endif //FEATURE_OAD
  GAP_ADTYPE_16BIT_MORE,      // some of the UUID's, but not all
#ifdef FEATURE_OAD
  LO_UINT16(OAD_SERVICE_UUID),
  HI_UINT16(OAD_SERVICE_UUID),
#endif //FEATURE_OAD
#ifndef FEATURE_OAD_ONCHIP
  LO_UINT16(SIMPLEPROFILE_SERV_UUID),
  HI_UINT16(SIMPLEPROFILE_SERV_UUID)
#endif //FEATURE_OAD_ONCHIP
};

// GAP GATT Attributes
static uint8_t attDeviceName[GAP_DEVICE_NAME_LEN] = "Simple BLE Peripheral";

// Globals used for ATT Response retransmission
static gattMsgEvent_t *pAttRsp = NULL;
static uint8_t rspTxRetry = 0;

bool eventChange_head;
bool eventChange_body;
bool elementHead_bleIcon;
char elementHead_text[20];
uint8_t elementHead_batteryLevel;
uint8_t modeMain = 0;
bool  stateMeasurement = false, 
enableSleep = false, 
stateDisplay = Display_ON;
uint8_t BufferForPedometer[20];
bool updateClock = true;
uint8_t count = 0;

uint16_t heartRateNumber = 960;
uint8_t spO2Number = 0;
uint16_t StepCount = 10;

I2C_Handle i2c;
I2C_Params i2cParams;
I2C_Transaction i2cTransaction;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void SimpleBLEPeripheral_init( void );
static void SimpleBLEPeripheral_taskFxn(UArg a0, UArg a1);

static uint8_t SimpleBLEPeripheral_processStackMsg(ICall_Hdr *pMsg);
static uint8_t SimpleBLEPeripheral_processGATTMsg(gattMsgEvent_t *pMsg);
static void SimpleBLEPeripheral_processAppMsg(sbpEvt_t *pMsg);
static void SimpleBLEPeripheral_processStateChangeEvt(gaprole_States_t newState);
static void SimpleBLEPeripheral_processCharValueChangeEvt(uint8_t paramID);
static void SimpleBLEPeripheral_performPeriodicTask(void);
static void SimpleBLEPeripheral_clockHandler(UArg arg);

static void SimpleBLEPeripheral_sendAttRsp(void);
static void SimpleBLEPeripheral_freeAttRsp(uint8_t status);

static void SimpleBLEPeripheral_stateChangeCB(gaprole_States_t newState);
#ifndef FEATURE_OAD_ONCHIP
static void SimpleBLEPeripheral_charValueChangeCB(uint8_t paramID);
#endif //!FEATURE_OAD_ONCHIP
static void SimpleBLEPeripheral_enqueueMsg(uint8_t event, uint8_t state);

#ifdef FEATURE_OAD
void SimpleBLEPeripheral_processOadWriteCB(uint8_t event, uint16_t connHandle,
 uint8_t *pData);
#endif //FEATURE_OAD

static void gpioButtonFxn1(uint_least8_t index);
static void gpioButtonFxn0(uint_least8_t index);

static void mainTaskInit(void);
static void mainToggle1(void);

static uint8_t battMeasure(void);
static void Batt_MeasLevel(void);
static void clock_main();
static void heart_show(uint16_t rate);
static void heart_main();
static void pedometer_show(uint16_t stepCount);
static void heart_measurement(bool ONOFF);
static void pedometer_main(void);
static void HienThi_init(void);
static void display_head(void);
static void HienThi_showHead(void);
static void HienThi_showBody(uint8_t mode);
static void HienThi_TurnONOFFDisplay(bool turn);
static void SpO2_Reset();
static void checkStateClockFunction(void);
static void updateTime(uint8_t *newdatetime);
static void updateConfigProfile(uint8_t *newconfigprofile);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// GAP Role Callbacks
static gapRolesCBs_t SimpleBLEPeripheral_gapRoleCBs =
{
  SimpleBLEPeripheral_stateChangeCB     // Profile State Change Callbacks
};

// GAP Bond Manager Callbacks
static gapBondCBs_t simpleBLEPeripheral_BondMgrCBs =
{
  NULL, // Passcode callback (not used by application)
  NULL  // Pairing / Bonding state Callback (not used by application)
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SimpleBLEPeripheral_createTask
 *
 * @brief   Task creation function for the Simple BLE Peripheral.
 *
 * @param   None.
 *
 * @return  None.
 */
void SimpleBLEPeripheral_createTask(void)
{
  // Display_init();
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = sbpTaskStack;
  taskParams.stackSize = SBP_TASK_STACK_SIZE;
  taskParams.priority = SBP_TASK_PRIORITY;

  Task_construct(&sbpTask, SimpleBLEPeripheral_taskFxn, &taskParams, NULL);
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_init
 *
 * @brief   Called during initialization and contains application
 *          specific initialization (ie. hardware initialization/setup,
 *          table initialization, power up notification, etc), and
 *          profile initialization/setup.
 *
 * @param   None.
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_init(void)
{
  // ******************************************************************
  // N0 STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
  // ******************************************************************
  // Register the current thread as an ICall dispatcher application
  // so that the application can send and receive messages.
  ICall_registerApp(&selfEntity, &sem);

#ifdef USE_RCOSC
  RCOSC_enableCalibration();
#endif // USE_RCOSC

#if defined( USE_FPGA )
  // configure RF Core SMI Data Link
  IOCPortConfigureSet(IOID_12, IOC_PORT_RFC_GPO0, IOC_STD_OUTPUT);
  IOCPortConfigureSet(IOID_11, IOC_PORT_RFC_GPI0, IOC_STD_INPUT);

  // configure RF Core SMI Command Link
  IOCPortConfigureSet(IOID_10, IOC_IOCFG0_PORT_ID_RFC_SMI_CL_OUT, IOC_STD_OUTPUT);
  IOCPortConfigureSet(IOID_9, IOC_IOCFG0_PORT_ID_RFC_SMI_CL_IN, IOC_STD_INPUT);

  // configure RF Core tracer IO
  IOCPortConfigureSet(IOID_8, IOC_PORT_RFC_TRC, IOC_STD_OUTPUT);
#else // !USE_FPGA
  #if defined( DEBUG_SW_TRACE )
    // configure RF Core tracer IO
  IOCPortConfigureSet(IOID_8, IOC_PORT_RFC_TRC, IOC_STD_OUTPUT | IOC_CURRENT_4MA | IOC_SLEW_ENABLE);
  #endif // DEBUG_SW_TRACE
#endif // USE_FPGA




  mainTaskInit();    


  // Create an RTOS queue for message from profile to be sent to app.
  appMsgQueue = Util_constructQueue(&appMsg);

  // Create one-shot clocks for internal periodic events.
  // Util_constructClock(&periodicClock, SimpleBLEPeripheral_clockHandler, SBP_PERIODIC_EVT_PERIOD, 0, false, SBP_PERIODIC_EVT);
  Util_constructClock(&mainClockStruct, SimpleBLEPeripheral_clockHandler, 200, 0, true, EVENT_UPDATE_OLED);
  Util_constructClock(&checkStateClockStruct, SimpleBLEPeripheral_clockHandler, 1000, 0, true, EVENT_BIGTIME_CHECKSTATE);
  Util_constructClock(&countSleepClockStruct, SimpleBLEPeripheral_clockHandler, 5000, 0, false, EVENT_SLEEPMODE_COMPLETE);
  Util_constructClock(&measureClockStruct, SimpleBLEPeripheral_clockHandler, 10, 0, true, EVENT_MEASUREMENT);

  // Setup the GAP
  GAP_SetParamValue(TGAP_CONN_PAUSE_PERIPHERAL, DEFAULT_CONN_PAUSE_PERIPHERAL);

  // Setup the GAP Peripheral Role Profile
  {
    // For all hardware platforms, device starts advertising upon initialization
    uint8_t initialAdvertEnable = TRUE;

    // By setting this to zero, the device will go into the waiting state after
    // being discoverable for 30.72 second, and will not being advertising again
    // until the enabler is set back to TRUE
    uint16_t advertOffTime = 0;

    uint8_t enableUpdateRequest = DEFAULT_ENABLE_UPDATE_REQUEST;
    uint16_t desiredMinInterval = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
    uint16_t desiredMaxInterval = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
    uint16_t desiredSlaveLatency = DEFAULT_DESIRED_SLAVE_LATENCY;
    uint16_t desiredConnTimeout = DEFAULT_DESIRED_CONN_TIMEOUT;

    // Set the GAP Role Parameters
    GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
     &initialAdvertEnable);
    GAPRole_SetParameter(GAPROLE_ADVERT_OFF_TIME, sizeof(uint16_t),
     &advertOffTime);

    GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof(scanRspData),
     scanRspData);
    GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);

    GAPRole_SetParameter(GAPROLE_PARAM_UPDATE_ENABLE, sizeof(uint8_t),
     &enableUpdateRequest);
    GAPRole_SetParameter(GAPROLE_MIN_CONN_INTERVAL, sizeof(uint16_t),
     &desiredMinInterval);
    GAPRole_SetParameter(GAPROLE_MAX_CONN_INTERVAL, sizeof(uint16_t),
     &desiredMaxInterval);
    GAPRole_SetParameter(GAPROLE_SLAVE_LATENCY, sizeof(uint16_t),
     &desiredSlaveLatency);
    GAPRole_SetParameter(GAPROLE_TIMEOUT_MULTIPLIER, sizeof(uint16_t),
     &desiredConnTimeout);
  }

  // Set the GAP Characteristics
  GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName);

  // Set advertising interval
  {
    uint16_t advInt = DEFAULT_ADVERTISING_INTERVAL;

    GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MIN, advInt);
    GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MAX, advInt);
    GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MIN, advInt);
    GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MAX, advInt);
  }

  // Setup the GAP Bond Manager
  {
    uint32_t passkey = 0; // passkey "000000"
    uint8_t pairMode = GAPBOND_PAIRING_MODE_WAIT_FOR_REQ;
    uint8_t mitm = TRUE;
    uint8_t ioCap = GAPBOND_IO_CAP_DISPLAY_ONLY;
    uint8_t bonding = TRUE;

    GAPBondMgr_SetParameter(GAPBOND_DEFAULT_PASSCODE, sizeof(uint32_t),
      &passkey);
    GAPBondMgr_SetParameter(GAPBOND_PAIRING_MODE, sizeof(uint8_t), &pairMode);
    GAPBondMgr_SetParameter(GAPBOND_MITM_PROTECTION, sizeof(uint8_t), &mitm);
    GAPBondMgr_SetParameter(GAPBOND_IO_CAPABILITIES, sizeof(uint8_t), &ioCap);
    GAPBondMgr_SetParameter(GAPBOND_BONDING_ENABLED, sizeof(uint8_t), &bonding);
  }

   // Initialize GATT attributes
  GGS_AddService(GATT_ALL_SERVICES);           // GAP
  GATTServApp_AddService(GATT_ALL_SERVICES);   // GATT attributes
  // DevInfo_AddService();                        // Device Information Service


#ifdef IMAGE_INVALIDATE
  Reset_addService();
#endif //IMAGE_INVALIDATE


#ifndef FEATURE_OAD_ONCHIP

  // Register callback with SimpleGATTprofile
  SimpleProfile_RegisterAppCBs(&SimpleBLEPeripheral_simpleProfileCBs);
#endif //!FEATURE_OAD_ONCHIP

  // Start the Device
  VOID GAPRole_StartDevice(&SimpleBLEPeripheral_gapRoleCBs);

  // Start Bond Manager
  VOID GAPBondMgr_Register(&simpleBLEPeripheral_BondMgrCBs);

  // Register with GAP for HCI/Host messages
  GAP_RegisterForMsgs(selfEntity);

  // Register for GATT local events and ATT Responses pending for transmission
  GATT_RegisterForMsgs(selfEntity);

  HCI_LE_ReadMaxDataLenCmd();

#if defined FEATURE_OAD
#if defined (HAL_IMAGE_A)
  // Display_print0(dispHandle, 0, 0, "BLE Peripheral A");
#else
  // Display_print0(dispHandle, 0, 0, "BLE Peripheral B");
#endif // HAL_IMAGE_A
#else
  // Display_print0(dispHandle, 0, 0, "BLE Peripheral");
#endif // FEATURE_OAD
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_taskFxn
 *
 * @brief   Application task entry point for the Simple BLE Peripheral.
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */

static void SimpleBLEPeripheral_taskFxn(UArg a0, UArg a1)
{
  // Initialize application
  SimpleBLEPeripheral_init();

  // Application main loop
  for (;;)
  {
    // Waits for a signal to the semaphore associated with the calling thread.
    // Note that the semaphore associated with a thread is signaled when a
    // message is queued to the message receive queue of the thread or when
    // ICall_signal() function is called onto the semaphore.
    ICall_Errno errno = ICall_wait(ICALL_TIMEOUT_FOREVER);

    if (errno == ICALL_ERRNO_SUCCESS)
    {
      ICall_EntityID dest;
      ICall_ServiceEnum src;
      ICall_HciExtEvt *pMsg = NULL;

      if (ICall_fetchServiceMsg(&src, &dest,
        (void **)&pMsg) == ICALL_ERRNO_SUCCESS)
      {
        uint8 safeToDealloc = TRUE;

        if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity))
        {
          ICall_Stack_Event *pEvt = (ICall_Stack_Event *)pMsg;

          // Check for BLE stack events first
          if (pEvt->signature == 0xffff)
          {
            if (pEvt->event_flag & SBP_CONN_EVT_END_EVT)
            {
              // Try to retransmit pending ATT Response (if any)
              SimpleBLEPeripheral_sendAttRsp();
            }
          }
          else
          {
            // Process inter-task message
            safeToDealloc = SimpleBLEPeripheral_processStackMsg((ICall_Hdr *)pMsg);
          }
        }

        if (pMsg && safeToDealloc)
        {
          ICall_freeMsg(pMsg);
        }
      }

      // If RTOS queue is not empty, process app message.
      while (!Queue_empty(appMsgQueue))
      {
        sbpEvt_t *pMsg = (sbpEvt_t *)Util_dequeueMsg(appMsgQueue);
        if (pMsg)
        {
          // Process message.
          SimpleBLEPeripheral_processAppMsg(pMsg);

          // Free the space from the message.
          ICall_free(pMsg);
        }
      }
    }

    if (events & EVENT_UPDATE_OLED)
    {
      events &= ~EVENT_UPDATE_OLED;
      Util_stopClock(&mainClockStruct);
      if(stateDisplay == Display_OFF){
        HienThi_TurnONOFFDisplay(true);
      }
      // Function
      Util_startClock(&mainClockStruct);
    }

    if (events & EVENT_BIGTIME_CHECKSTATE)
    {
      events &= ~EVENT_BIGTIME_CHECKSTATE;
      Util_stopClock(&checkStateClockStruct);
      // checkStateClockFunction();
      Util_startClock(&checkStateClockStruct);
    }

    if (events & EVENT_SLEEPMODE_COMPLETE)
    {
      events &= ~EVENT_SLEEPMODE_COMPLETE;
      // Turn off the screen
      if(stateDisplay == Display_ON){
        Util_stopClock(&countSleepClockStruct);
        HienThi_TurnONOFFDisplay(false);
      }
    }

    if (events & SBP_PERIODIC_EVT)
    {
      events &= ~SBP_PERIODIC_EVT;

      Util_stopClock(&periodicClock);
      // Perform periodic application task
      
      Util_startClock(&periodicClock);
    }

    if (events & EVENT_MEASUREMENT)
    {
      events &= ~EVENT_MEASUREMENT;
      // Perform periodic application task
      Util_stopClock(&measureClockStruct);
      //
      Util_startClock(&measureClockStruct);
    }
  }
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_processStackMsg
 *
 * @brief   Process an incoming stack message.
 *
 * @param   pMsg - message to process
 *
 * @return  TRUE if safe to deallocate incoming message, FALSE otherwise.
 */
static uint8_t SimpleBLEPeripheral_processStackMsg(ICall_Hdr *pMsg)
{
  uint8_t safeToDealloc = TRUE;

  switch (pMsg->event)
  {
    case GATT_MSG_EVENT:
      // Process GATT message
    safeToDealloc = SimpleBLEPeripheral_processGATTMsg((gattMsgEvent_t *)pMsg);
    break;

    case HCI_GAP_EVENT_EVENT:
    {
        // Process HCI message
      switch(pMsg->status)
      {
        case HCI_COMMAND_COMPLETE_EVENT_CODE:
            // Process HCI Command Complete Event
        break;

        default:
        break;
      }
    }
    break;

    default:
      // do nothing
    break;
  }

  return (safeToDealloc);
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_processGATTMsg
 *
 * @brief   Process GATT messages and events.
 *
 * @return  TRUE if safe to deallocate incoming message, FALSE otherwise.
 */
static uint8_t SimpleBLEPeripheral_processGATTMsg(gattMsgEvent_t *pMsg)
{
  // See if GATT server was unable to transmit an ATT response
  if (pMsg->hdr.status == blePending)
  {
    // No HCI buffer was available. Let's try to retransmit the response
    // on the next connection event.
    if (HCI_EXT_ConnEventNoticeCmd(pMsg->connHandle, selfEntity,
     SBP_CONN_EVT_END_EVT) == SUCCESS)
    {
      // First free any pending response
      SimpleBLEPeripheral_freeAttRsp(FAILURE);

      // Hold on to the response message for retransmission
      pAttRsp = pMsg;

      // Don't free the response message yet
      return (FALSE);
    }
  }
  else if (pMsg->method == ATT_FLOW_CTRL_VIOLATED_EVENT)
  {
    // ATT request-response or indication-confirmation flow control is
    // violated. All subsequent ATT requests or indications will be dropped.
    // The app is informed in case it wants to drop the connection.

    // Display the opcode of the message that caused the violation.
    // Display_print1(dispHandle, 5, 0, "FC Violated: %d", pMsg->msg.flowCtrlEvt.opcode);
  }
  else if (pMsg->method == ATT_MTU_UPDATED_EVENT)
  {
    // MTU size updated
    // Display_print1(dispHandle, 5, 0, "MTU Size: $d", pMsg->msg.mtuEvt.MTU);
  }

  // Free message payload. Needed only for ATT Protocol messages
  GATT_bm_free(&pMsg->msg, pMsg->method);

  // It's safe to free the incoming message
  return (TRUE);
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_sendAttRsp
 *
 * @brief   Send a pending ATT response message.
 *
 * @param   none
 *
 * @return  none
 */
static void SimpleBLEPeripheral_sendAttRsp(void)
{
  // See if there's a pending ATT Response to be transmitted
  if (pAttRsp != NULL)
  {
    uint8_t status;

    // Increment retransmission count
    rspTxRetry++;

    // Try to retransmit ATT response till either we're successful or
    // the ATT Client times out (after 30s) and drops the connection.
    status = GATT_SendRsp(pAttRsp->connHandle, pAttRsp->method, &(pAttRsp->msg));
    if ((status != blePending) && (status != MSG_BUFFER_NOT_AVAIL))
    {
      // Disable connection event end notice
      HCI_EXT_ConnEventNoticeCmd(pAttRsp->connHandle, selfEntity, 0);

      // We're done with the response message
      SimpleBLEPeripheral_freeAttRsp(status);
    }
    else
    {
      // Continue retrying
      // Display_print1(dispHandle, 5, 0, "Rsp send retry: %d", rspTxRetry);
    }
  }
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_freeAttRsp
 *
 * @brief   Free ATT response message.
 *
 * @param   status - response transmit status
 *
 * @return  none
 */
static void SimpleBLEPeripheral_freeAttRsp(uint8_t status)
{
  // See if there's a pending ATT response message
  if (pAttRsp != NULL)
  {
    // See if the response was sent out successfully
    if (status == SUCCESS)
    {
      // Display_print1(dispHandle, 5, 0, "Rsp sent retry: %d", rspTxRetry);
    }
    else
    {
      // Free response payload
      GATT_bm_free(&pAttRsp->msg, pAttRsp->method);

      // Display_print1(dispHandle, 5, 0, "Rsp retry failed: %d", rspTxRetry);
    }

    // Free response message
    ICall_freeMsg(pAttRsp);

    // Reset our globals
    pAttRsp = NULL;
    rspTxRetry = 0;
  }
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_processAppMsg
 *
 * @brief   Process an incoming callback from a profile.
 *
 * @param   pMsg - message to process
 *
 * @return  None.
 */

static void SimpleBLEPeripheral_processAppMsg(sbpEvt_t *pMsg)
{
  switch (pMsg->hdr.event)
  {
    case SBP_STATE_CHANGE_EVT:
    SimpleBLEPeripheral_processStateChangeEvt((gaprole_States_t)pMsg->
      hdr.state);
    break;

    case SBP_CHAR_CHANGE_EVT:
    SimpleBLEPeripheral_processCharValueChangeEvt(pMsg->hdr.state);
    break;

    default:
      // Do nothing.
    break;
  }
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_stateChangeCB
 *
 * @brief   Callback from GAP Role indicating a role state change.
 *
 * @param   newState - new state
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_stateChangeCB(gaprole_States_t newState)
{
  SimpleBLEPeripheral_enqueueMsg(SBP_STATE_CHANGE_EVT, newState);
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_processStateChangeEvt
 *
 * @brief   Process a pending GAP Role state change event.
 *
 * @param   newState - new state
 *
 * @return  None.
 */
static void SimpleBLEPeripheral_processStateChangeEvt(gaprole_States_t newState)
{
#ifdef PLUS_BROADCASTER
  static bool firstConnFlag = false;
#endif // PLUS_BROADCASTER
  switch ( newState )
  {
    case GAPROLE_STARTED:
    {
       uint8_t ownAddress[B_ADDR_LEN];
       uint8_t systemId[DEVINFO_SYSTEM_ID_LEN];

       GAPRole_GetParameter(GAPROLE_BD_ADDR, ownAddress);

       // use 6 bytes of device address for 8 bytes of system ID value
       systemId[0] = ownAddress[0];
       systemId[1] = ownAddress[1];
       systemId[2] = ownAddress[2];

       // set middle bytes to zero
       systemId[4] = 0x00;
       systemId[3] = 0x00;

       // shift three bytes up
       systemId[7] = ownAddress[5];
       systemId[6] = ownAddress[4];
       systemId[5] = ownAddress[3];

        DevInfo_SetParameter(DEVINFO_SYSTEM_ID, DEVINFO_SYSTEM_ID_LEN, systemId);

        // Display device address
        // Display_print0(dispHandle, 1, 0, Util_convertBdAddr2Str(ownAddress));
        // Display_print0(dispHandle, 2, 0, "Initialized");
    }
    break;

    case GAPROLE_ADVERTISING:
      // Display_print0(dispHandle, 2, 0, "Advertising");
    break;

    case GAPROLE_CONNECTED:
    {
      linkDBInfo_t linkInfo;
      uint8_t numActive = 0;

      Util_startClock(&periodicClock);

      numActive = linkDB_NumActive();

        // Use numActive to determine the connection handle of the last
        // connection
      if ( linkDB_GetInfo( numActive - 1, &linkInfo ) == SUCCESS )
      {
          // Display_print1(dispHandle, 2, 0, "Num Conns: %d", (uint16_t)numActive);
          // Display_print0(dispHandle, 3, 0, Util_convertBdAddr2Str(linkInfo.addr));
      }
      else
      {
        uint8_t peerAddress[B_ADDR_LEN];

        GAPRole_GetParameter(GAPROLE_CONN_BD_ADDR, peerAddress);

          // Display_print0(dispHandle, 2, 0, "Connected");
          // Display_print0(dispHandle, 3, 0, Util_convertBdAddr2Str(peerAddress));
      }

        #ifdef PLUS_BROADCASTER
          // Only turn advertising on for this state when we first connect
          // otherwise, when we go from connected_advertising back to this state
          // we will be turning advertising back on.
      if (firstConnFlag == false)
      {
            uint8_t advertEnabled = FALSE; // Turn on Advertising

            // Disable connectable advertising.
            GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
             &advertEnabled);

            // Set to true for non-connectabel advertising.
            advertEnabled = TRUE;

            // Enable non-connectable advertising.
            GAPRole_SetParameter(GAPROLE_ADV_NONCONN_ENABLED, sizeof(uint8_t),
             &advertEnabled);
            firstConnFlag = true;
          }
        #endif // PLUS_BROADCASTER
        }
        break;

        case GAPROLE_CONNECTED_ADV:
      // Display_print0(dispHandle, 2, 0, "Connected Advertising");
        break;

        case GAPROLE_WAITING:
        Util_stopClock(&periodicClock);
        if(stateDisplay == Display_OFF){
          heart_measurement(false);
          stateMeasurement = false;
        }
        SimpleBLEPeripheral_freeAttRsp(bleNotConnected);



      // Clear remaining lines
        // Display_clearLines(dispHandle, 3, 5);
        break;

        case GAPROLE_WAITING_AFTER_TIMEOUT:
        SimpleBLEPeripheral_freeAttRsp(bleNotConnected);

      // Display_print0(dispHandle, 2, 0, "Timed Out");

      // Clear remaining lines
        //Display_clearLines(dispHandle, 3, 5);

      #ifdef PLUS_BROADCASTER
        // Reset flag for next connection.
        firstConnFlag = false;
      #endif //#ifdef (PLUS_BROADCASTER)
        break;

        case GAPROLE_ERROR:
      // Display_print0(dispHandle, 2, 0, "Error");
        break;

        default:
        // Display_clearLine(dispHandle, 2);
        break;
      }

  // Update the state
  //gapProfileState = newState;
    }

#ifndef FEATURE_OAD_ONCHIP
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
      SimpleBLEPeripheral_enqueueMsg(SBP_CHAR_CHANGE_EVT, paramID);
    }
#endif //!FEATURE_OAD_ONCHIP

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
        {
          uint8_t newDateTime[7] = {0x00, 0x00, 0x00, 0x00, 0x03, 0x02, 0x01 };
          SimpleProfile_GetParameter(SIMPLEPROFILE_CHAR1, newDateTime);
          updateTime(newDateTime);
        }
        break;

        case SIMPLEPROFILE_CHAR2:
        {
          uint8_t newConfigProfile[3] = {0x00, 0x00, 0x00};
          SimpleProfile_GetParameter(SIMPLEPROFILE_CHAR2, newConfigProfile);
          updateConfigProfile(newConfigProfile);
        }
        break;

        case SIMPLEPROFILE_CHAR7:
        {
          SimpleProfile_GetParameter(SIMPLEPROFILE_CHAR7, &newValue);
          if (!stateMeasurement && newValue == '1'){
            heart_measurement(true);
            stateMeasurement = true;
          }

          if(stateMeasurement && newValue == '0'){
            heart_measurement(false);
            stateMeasurement = false;
          }
        }
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
    static uint8_t valuetest = 0x00;
    static void SimpleBLEPeripheral_performPeriodicTask(void)
    {
      valuetest++;
      SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR6, sizeof(uint8_t),
       &valuetest);
    }


#ifdef FEATURE_OAD
/*********************************************************************
 * @fn      SimpleBLEPeripheral_processOadWriteCB
 *
 * @brief   Process a write request to the OAD profile.
 *
 * @param   event      - event type:
 *                       OAD_WRITE_IDENTIFY_REQ
 *                       OAD_WRITE_BLOCK_REQ
 * @param   connHandle - the connection Handle this request is from.
 * @param   pData      - pointer to data for processing and/or storing.
 *
 * @return  None.
 */
    void SimpleBLEPeripheral_processOadWriteCB(uint8_t event, uint16_t connHandle,
     uint8_t *pData)
    {
      oadTargetWrite_t *oadWriteEvt = ICall_malloc( sizeof(oadTargetWrite_t) + \
       sizeof(uint8_t) * OAD_PACKET_SIZE);

      if ( oadWriteEvt != NULL )
      {
        oadWriteEvt->event = event;
        oadWriteEvt->connHandle = connHandle;

        oadWriteEvt->pData = (uint8_t *)(&oadWriteEvt->pData + 1);
        memcpy(oadWriteEvt->pData, pData, OAD_PACKET_SIZE);

        Queue_put(hOadQ, (Queue_Elem *)oadWriteEvt);

    // Post the application's semaphore.
        Semaphore_post(sem);
      }
      else
      {
    // Fail silently.
      }
    }
#endif //FEATURE_OAD

/*********************************************************************
 * @fn      SimpleBLEPeripheral_clockHandler
 *
 * @brief   Handler function for clock timeouts.
 *
 * @param   arg - event type
 *
 * @return  None.
 */
    static void SimpleBLEPeripheral_clockHandler(UArg arg)
    {
  // Store the event.
      events |= arg;

  // Wake up the application.
      Semaphore_post(sem);
    }

/*********************************************************************
 * @fn      SimpleBLEPeripheral_enqueueMsg
 *
 * @brief   Creates a message and puts the message in RTOS queue.
 *
 * @param   event - message event.
 * @param   state - message state.
 *
 * @return  None.
 */
    static void SimpleBLEPeripheral_enqueueMsg(uint8_t event, uint8_t state)
    {
      sbpEvt_t *pMsg;

  // Create dynamic pointer to message.
      if ((pMsg = ICall_malloc(sizeof(sbpEvt_t))))
      {
        pMsg->hdr.event = event;
        pMsg->hdr.state = state;

    // Enqueue the message.
        Util_enqueueMsg(appMsgQueue, sem, (uint8*)pMsg);
      }
    }

/*********************************************************************
*********************************************************************/



// Hop nhat 2 file


    static void gpioButtonFxn0(uint_least8_t index){
  /* Clear the GPIO interrupt and toggle an LED */
      enableSleep = false;
      Util_stopClock(&countSleepClockStruct);
  //Increate ModeMain
      modeMain++;
      modeMain%=3;

  // Turn on OLED
      if(stateDisplay == Display_OFF){
        modeMain=0;
        Util_startClock(&mainClockStruct);
        if(stateMeasurement){
          modeMain = 1;
        } 
      }

      if(modeMain == 1){
        heartRateNumber = 0;
        stateMeasurement = true;
      }else{
        stateMeasurement = false;  
        // Util_stopClock(&measureClockStruct);
      }
    }

    static void gpioButtonFxn1(uint_least8_t index){
      // GPIO_toggle(Board_GPIO_LED1);
      // GPIO_toggle(Board_DIO12);
    }




    static void mainTaskInit(void){
      {

        // /* Initialize display and try to open both UART and LCD types of display. */
        // const char  echoPrompt[] = "Echoing characters:\r\n";
        // /* Create a UART with data processing off. */
        // UART_init();
        // UART_Params_init(&uartParams);
        // uartParams.writeDataMode = UART_DATA_BINARY;
        // uartParams.readDataMode = UART_DATA_BINARY;
        // uartParams.readReturnMode = UART_RETURN_FULL;
        // uartParams.readEcho = UART_ECHO_OFF;
        // uartParams.baudRate = 115200;

        // uart = UART_open(Board_UART0, &uartParams);

        // if (uart == NULL) {
        //     /* UART_open() failed */
        //     while (1);
        // }
        // UART_write(uart, echoPrompt, sizeof(echoPrompt));

        GPIO_init();
        GPIO_PinConfig pinConfig = Board_GPIO_BUTTON0 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING;
        GPIO_PinConfig pinConfig1 = Board_GPIO_BUTTON1 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING;

        GPIO_setConfig(Board_GPIO_LED0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
        GPIO_setConfig(Board_GPIO_LED1, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
        GPIO_setConfig(Board_GPIO_BUTTON0, pinConfig);
        GPIO_setConfig(Board_GPIO_BUTTON1, pinConfig1);

        GPIO_setCallback(Board_GPIO_BUTTON0, gpioButtonFxn0);
        GPIO_setCallback(Board_GPIO_BUTTON1, gpioButtonFxn1);
        GPIO_enableInt(Board_GPIO_BUTTON0);
        GPIO_enableInt(Board_GPIO_BUTTON1);
      }
      {
        I2C_init();
        I2C_Params_init(&i2cParams);
        i2cParams.bitRate = I2C_400kHz;
        i2cParams.transferMode = I2C_MODE_BLOCKING;
        i2c = I2C_open(Board_I2C_TMP, &i2cParams);
        if (i2c == NULL){
          while (1);
        }
      }
      {
        Clock_init(1);
        WDsDisplay__init(OLED_RESET);
        HienThi_init();
        elementHead_bleIcon = false;
        strcpy( elementHead_text, "MEMSTECH");
        Batt_MeasLevel();
      }
      {
        GPIO_write(Board_GPIO_LED0, 1);
        pedometer_init();
      }
      {
       AFE_Init();
    // Setup kalman param
       SimpleKalmanFilter(2, 2, 0.0008);
       // Util_stopClock(&measureClockStruct);
     }
   }

   static void mainToggle1(void){
    HienThi_showHead();
    HienThi_showBody(modeMain);

  }



  static uint8_t battMeasure(void)
  {
    static uint32_t percent;
    uint16_t battMinLevel = BATT_MIN_VOLTAGE;
    uint16_t range = BATT_MAX_VOLTAGE - BATT_MIN_VOLTAGE;
  // Read the battery voltage (V), only the first 12 bits
    percent = AONBatMonBatteryVoltageGet();
  // Convert to from V to mV to avoid fractions.
  // Fractional part is in the lower 8 bits thus converting is done as follows:
  // (1/256)/(1/1000) = 1000/256 = 125/32
  // This is done most effectively by multiplying by 125 and then shifting
  // 5 bits to the right.
    percent = (percent * 125) >> 5;
  // Convert to percentage of maximum voltage.
    percent = (percent - battMinLevel)*100/ range;
    return percent;
  }

  static void Batt_MeasLevel(void)
  {
    uint16_t level;
    level = battMeasure();
    if(level >=0 && level <=100){
    // Update level
      elementHead_batteryLevel = (uint8_t)(level & 0x00FF);
    }
  }

  static void clock_main(){
    int hour, min, sec;
    if(updateClock){
      updateClock = false;
      sec = Clock_getSecond();
      hour = Clock_getHour();
      min = Clock_getMinute();
      WDsDisplay__clearDisplay();
      WDsDisplay__Clock_set(hour, min);
      WDsDisplay__Colon_toogle();
      eventChange_body = true;
      if(!enableSleep){
        enableSleep = true;
        Util_startClock(&countSleepClockStruct);
      }
    }
  }

  static void heart_show(uint16_t rate){
    static uint8_t _rate;
    WDsDisplay__Clear_body();
    WDsDisplay__Heartrate_status();
    if(_rate != rate){
      _rate = rate;
    }
    WDsDisplay__Heartrate_number(_rate);
    eventChange_body = true;
  }

  static void heart_measurement(bool ONOFF){
    uint8_t value;
    SimpleProfile_GetParameter(SIMPLEPROFILE_CHAR7, &value);
    if(ONOFF){
      if(value == '1') value = '0';
      TurnONOFF_HRModule(true);
      // GPIO_write(Board_GPIO_LED1, 1);
      // Util_startClock(&measureClockStruct);
    }else{
      if(value == '0') value = '1';
      TurnONOFF_HRModule(false);
      // GPIO_write(Board_GPIO_LED1, 0);
      // Util_stopClock(&measureClockStruct);
    }
    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR7, 1, &value);
  }

  static void heart_main(){
  //Start measure heart rate
    heart_show(heartRateNumber);
  }

  static void pedometer_show(uint16_t stepCount) {
    WDsDisplay__Clear_body();
    WDsDisplay__Footsteps_status();
    WDsDisplay__Footsteps_number(stepCount);
    eventChange_body = true;
  }

  static uint16_t pedometer_measurement(void){
    uint8_t countdown = 255;
    pedometer_cmd_readstatus();
    while(countdown--){
      pedometer_Read(BufferForPedometer, 2);
      delay_mma9553();
      if (BufferForPedometer[1] == 0x80)
      {
        pedometer_Read(BufferForPedometer, 16);
        StepCount = (BufferForPedometer[6] <<8) | BufferForPedometer[7];
        break;
      }

    }
    return StepCount;
  }

  static void pedometer_main(void){
    static uint16_t steps = 100;
    uint16_t _steps = pedometer_measurement();
    if(steps != _steps){
      steps = _steps;
      Util_stopClock(&countSleepClockStruct);
      enableSleep = false;
    }
    pedometer_show(steps);
    if(!enableSleep){
      enableSleep = true;
      Util_startClock(&countSleepClockStruct);
    }  
  }

  static void HienThi_init(void){
    WDsDisplay__begin(SSD1306_SWITCHCAPVCC, 0x3C);
    WDsDisplay__clearDisplay();
    WDsDisplay__display();
    delay_mma9553();
  }

  static void display_head(void){
    WDsDisplay__Clear_head();
    WDsDisplay__setTextSize(1);
    WDsDisplay__setTextColor(WHITE);
    WDsDisplay__setCursor(40, 8);
    WDsDisplay__print((uint8_t*)elementHead_text);
    WDsDisplay__Bluetooth_icon(elementHead_bleIcon);
    WDsDisplay__Battery_set(elementHead_batteryLevel, false);
    eventChange_head = true;
  }

  static void HienThi_showHead(void){
    display_head();
    if(eventChange_head){
      WDsDisplay__Display_head();
      eventChange_head = false;
    }
  }

  typedef void (*funcVoid)();
  funcVoid arrayMain[3] = {clock_main, heart_main, pedometer_main};
  static void HienThi_showBody(uint8_t mode){
  arrayMain[mode](); // instead of switch-case

  // switch(mode){
  //   case 0: clock_main();break;
  //   case 1: heart_main();break;
  //   case 2: pedometer_main();break;
  //   default: break;
  // }

  if(stateMeasurement){
    heart_measurement(true);
  }else{
    heart_measurement(false);
  }
  if(eventChange_body){
    WDsDisplay__Display_body();
    eventChange_body = false;
  }
}

static void HienThi_TurnONOFFDisplay(bool turn){
  WDsDisplay__onoff(turn);
  if(turn){
    stateDisplay = Display_ON;
    Util_startClock(&mainClockStruct);
  }
  else {
    stateDisplay = Display_OFF;
    Util_stopClock(&mainClockStruct);
    Util_stopClock(&countSleepClockStruct);
  }
}

// /////////////////Reset value
static void SpO2_Reset()
{
  redACValueSqSum = 0;
  irACValueSqSum = 0;
  spO2Number = 0;
  samplesRecorded = 0;
}

uint32_t timeTicks;
uint8_t arrayNumPulseToUpdate[4] = {10, 6, 3, 1};
float acSqRatio = 0;
uint8_t index111 = 0;
static void measureProcessFunction(void){
  {
    countBleElement ++;
    timeSum = 0;
    timeSavedCount = 0;
    timeValidCount = 0;
    dataLedGreen = get_GR_data();
    dataLedRed = get_RED_data();
    dataLedIR = get_IR_data();
    greenSumXValue = 0;
    greenSumXSum = 0;
    dataSum = dataLedGreen + dataLedRed + dataLedIR;
  }

  //Filter
  dataLedGreenDCRemove = dcRemoval(dataLedGreen, 0.9, 0);
  dataLedGreenLPF = LowPass_filter(dataLedGreenDCRemove,dataLedGreenLPF);
  dataLedGreenKalman = updateEstimate(dataLedGreenDCRemove);
  //Calculator Sum X Green Value
  for(count = 0 ; count < NUM_SUM_GREEN - 1 ; count++){
    signalGreenArray[count] = signalGreenArray[count+1];
    greenSumXValue += signalGreenArray[count];
  }
  signalGreenArray[NUM_SUM_GREEN - 1] = dataLedGreenKalman;
  greenSumXValue += signalGreenArray[NUM_SUM_GREEN - 1];
  //////////////////////////////////////////////

  //Calculator Average X Green Value
  for(count = 0 ; count < NUM_AVERAGE_GREEN - 1 ; count++){
    signalSumGreenArray[count] = signalSumGreenArray[count+1];
    greenSumXSum += signalSumGreenArray[count];
  }
  signalSumGreenArray[NUM_AVERAGE_GREEN - 1] = greenSumXValue;
  greenSumXSum += signalSumGreenArray[NUM_AVERAGE_GREEN - 1];
  averageGreenValue = (greenSumXSum + signalSumGreenArray[NUM_AVERAGE_GREEN - 1] * 5)/(NUM_AVERAGE_GREEN+4);
  //////////////////////////////////////////////

  //Save to BLE array
  if(countBleElement >= 20){
    countBleElement = 0;

      //Here  ------------------------------>  Send Data to smartphone

      /////////////////////////End///////////////////////////////////

    for(count = 0 ; count < NUM_BLE_ARRAY ; count++){
      arrayForBLETransmitter[count] = 0;
    }
  }
  arrayForBLETransmitter[countBleElement] = averageGreenValue>>8;
  //////////////////////////////////////////////

  greenSumXValue = dcRemoval(greenSumXValue, 0.9, 1);
  averageGreenValue = dcRemoval(averageGreenValue, 0.9, 2);

  // Processing HR and SpO2
  if(dataSum> 30000 && dataSum < 300000 && dataLedGreen > 10000 && dataLedGreen < 100000){
    {
      irACValueSqSum += dataLedRed * dataLedRed;
      redACValueSqSum += dataLedIR * dataLedIR;
      samplesRecorded++;
    }
    if(greenSumXValue > averageGreenValue && pulseDw){
      {
        pulseUp = true;
        pulseDw = false;
        pulseCount++;
        timeTicks = 10;
      }
      for(count = 0 ; count < NUM_TIME - 1 ; count++){
        if(timeArray[count+1] > timeArray[count] && timeArray[count] != 0){
          timeSum += timeArray[count+1] - timeArray[count];
          timeSavedCount++;
        }
        timeArray[count] = timeArray[count+1];
      }
      timeArray[NUM_TIME-1] = timeTicks;
      // if(timeSavedCount>0)
      //   averageTime = (float)timeSum/timeSavedCount;
      uint8_t cout = 0;
      for(cout = 0 ; cout < (NUM_TIME - 1) ; cout++){
        if(timeArray[cout+1] > timeArray[cout] && timeArray[cout] != 0){
          timeDiff = timeArray[cout+1] - timeArray[cout];
          if(timeDiff > averageTime*0.7 && timeDiff < averageTime*1.3){
            timeValidCount++;
          }else{
            timeValidCount = 0;
          }
        }
      }

      numPulseToUpdate =  arrayNumPulseToUpdate[SPEED_UPDATE_NEW_VALUE];

      if(pulseCount > numPulseToUpdate){
        if(timeValidCount > NUM_TIME / 2){
          if(averageTime>0)
            heartRateNumber = 6000000/averageTime;
          float acSqRatio = 100.0 * log(redACValueSqSum/samplesRecorded) / log(irACValueSqSum/samplesRecorded);

          if (acSqRatio > 66) {
            index111 = (uint8_t)acSqRatio - 56;
          } else if (acSqRatio > 50) {
            index111 = (uint8_t)acSqRatio - 40;
          }
                // SpO2_Reset();
          spO2Number = spO2LUT[index111];
          if(index111 > 42){
            spO2Number = 99;
          }
        }
        pulseCount = 0;
      }
    }
    if(greenSumXValue < averageGreenValue && pulseUp){
      pulseUp = false;
      pulseDw = true;
    }
  }
}


static void checkStateClockFunction(void){
  updateClock = true;
  Clock_setSecond();
  Batt_MeasLevel();
  SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR3, 1, &elementHead_batteryLevel);
  SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR4, 2, &StepCount);
  SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR5, 2, &heartRateNumber);

}


static bool checkCorrectTime(uint8_t *newdatetime){
  if( *(newdatetime) <=60){
    if( *(newdatetime +1) <=60){
      if( *(newdatetime +2) <=24){
        if( *(newdatetime +3) <=31){
          if( *(newdatetime +4) <=11){
            if( *(newdatetime +5) <=99){
              if( *(newdatetime +6) <=30){
                return true;
              }
            }
          }
        }
      }
    }
  }
  return false;
}


static void updateTime(uint8_t *newdatetime){
  if(checkCorrectTime(newdatetime)){
    Clock_updateTime(newdatetime);
    // GPIO_toggle(Board_GPIO_LED1);
  }

}

static void updateConfigProfile(uint8_t *newconfigprofile){
  uint8_t gender = *(newconfigprofile);
  newconfigprofile++;
  pedometer_write_config(newconfigprofile);
  strcpy( elementHead_text, "OK");
}


void AppAssertHandler(uint8 assertCause, uint8 assertSubcause)
{
//  Board_openDisplay(BOARD_DISPLAY_TYPE_LCD);
//  Board_writeString(">>>APP ASSERT", 0);
  // check the assert cause
//  switch (assertCause)
//  {
//    case HAL_ASSERT_CAUSE_ARITHMETIC_ERROR:
//      if (assertSubcause == HAL_ASSERT_CAUSE_DIV_BY_ZERO)
//      {
//  //      Board_writeString("***ERROR***", 1);
//  //      Board_writeString(">> DIV_BY_ZERO ERROR!", 2);
//      }
//      else
//      {
//  //      Board_writeString("***ERROR***", 1);
//  //      Board_writeString(">> ARITHMETIC ERROR!", 2);
//      }
//    break;
//    default:
////    Board_writeString("***ERROR***", 1);
////    Board_writeString(">> DEFAULT SPINLOCK!", 2);
//    HAL_ASSERT_SPINLOCK;
//  }
  return;
}
