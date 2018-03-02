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
#include <stdint.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Event.h>

// Hardware Interrupt 
#include "Board.h"
// #include <ti/drivers/GPIO.h>
// #include <ti/drivers/gpio/GPIOCC26XX.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CCC26XX.h>

#include "wearableDevice.h"
#include "measurementModule.h"
#include "simpleApplication.h"

/*********************************************************************
 * CONSTANTS
 */

// How often to perform periodic event (in msec)
#define SBP_PERIODIC_EVT_PERIOD               5000
#define SBP_MEASUREMENT_EVT_PERIOD            10
#define WEAR_UPDATEOLED_EVT_PERIOD            200
#define WEAR_SLEEPMODE_EVT_PERIOD             5000
#define WEAR_BIGTIME_EVT_PERIOD               1000

// Task configuration
#define MAINTASK_PRIORITY                     1


#ifndef MAINTASKS_TACKSIZE
#define MAINTASKS_TACKSIZE                   1024
#endif


// Task configuration
#define MEAS_PRIORITY                     2


#ifndef MEAS_TACKSIZE
#define MEAS_TACKSIZE                   1024
#endif

// Internal Events for RTOS application
#define EVENT_ALL                             0xffff
#define SBP_STATE_CHANGE_EVT                  0x0001
#define SBP_CHAR_CHANGE_EVT                   0x0002
#define SBP_PERIODIC_EVT                      0x0004
#define SBP_CONN_EVT_END_EVT                  0x0008
#define SBP_MEASUREMENT_EVT                   0x0010
#define WEAR_UPDATEOLED_EVT                   0x0020
#define WEAR_SLEEPMODE_EVT                    0x0040
#define WEAR_BIGTIME_EVT                      0x0080



/***** Variable declarations *****/
Task_Struct mainTaskStruct;
Task_Handle mainTaskHandle;
Char mainTaskStack[MAINTASKS_TACKSIZE];

Task_Struct measTaskStruct;
Task_Handle measTaskHandle;
Char measTaskStack[MEAS_TACKSIZE];

Clock_Struct countSleepClockStruct;
Clock_Handle countSleepClockHandle;
Clock_Struct mainClockStruct;
Clock_Handle mainClockHandle;
Clock_Struct checkStateClockStruct;
Clock_Handle checkStateClockHandle;
Clock_Struct bigtimeClockStruct;
Clock_Handle bigtimeClockHandle;
Clock_Struct measureClockStruct;
static Clock_Handle measureClockHandle;

Event_Struct operationEvent;
Event_Handle operationEventHandle;
Event_Struct operationEvent1;
Event_Handle operationEventHandle1;

extern Semaphore_Handle txMeasSem;

static PIN_Handle buttonPinHandle;
static PIN_State buttonPinState;

PIN_Config buttonPinTable[] = {
    Board_BUTTON0  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    PIN_TERMINATE
};

I2C_Handle i2c;
I2C_Params i2cParams;
I2C_Transaction i2cTransaction;

extern uint8_t modeMain;
extern bool stateDisplay;
extern bool enableMeasurement;

static void SimpleBLEPeripheral_GPIOInterrupt(void){
  /* Debounce logic, only toggle if the button is still pushed (low) */
  CPUdelay(8000*50);

  gpioButtonFxn0();
  
  if(!wearableDevice_ResetSleepMode()){
    Clock_start(countSleepClockHandle);
    wearableDevice_SetupSleepMode();
  }
  
  if(stateDisplay == Display_OFF){
    Clock_start(mainClockHandle);
  }

  if (enableMeasurement == true)
  {
    Clock_start(measureClockHandle);
  }else{
    Clock_stop(measureClockHandle);
  }
}

void SimpleBLEPeripheral_createTask(void){
  
  // GPIO_init();
  // GPIO_PinConfig pinConfig = Board_GPIO_BUTTON0 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING;
  // GPIO_setConfig(Board_GPIO_BUTTON0, pinConfig);
  // GPIO_setCallback(Board_GPIO_BUTTON0, SimpleBLEPeripheral_GPIOInterrupt);
  // GPIO_enableInt(Board_GPIO_BUTTON0);
  // 
  
  wearableDevice_init();
  HR_measurementInit();


  while(true){
    /* Wait for event */
    uint32_t events = Event_pend(operationEventHandle, 0, EVENT_ALL, BIOS_WAIT_FOREVER);
    
    if(events & WEAR_UPDATEOLED_EVT){
      wearableDevice_UpdateOLED();

      if(!wearableDevice_ResetSleepMode()){
        Clock_start(countSleepClockHandle);
      }
    }

    if (events & WEAR_SLEEPMODE_EVT){
      Clock_stop(countSleepClockHandle);
      Clock_stop(mainClockHandle);
      wearableDevice_SleepMode();

    }

    if (events & WEAR_BIGTIME_EVT)
    {
      // Perform periodic application task
      wearableDevice_BigTime();

      // xin o nho,ke function o day
      HR_TurnONOFF_HRModule(enableMeasurement);
    }

    if(events & SBP_MEASUREMENT_EVT)
    {
      HR_measurementTask();
    }
  }

}


// void Measurement_createTask(void){
//   while(1){
//     Semaphore_pend(txMeasSem, BIOS_WAIT_FOREVER);
//     HR_measurementTask();
//   }
// }



void mainClockTaskStructFunction(UArg arg0, UArg arg1){
  Event_post(operationEventHandle, WEAR_UPDATEOLED_EVT);
}

void countSleepClockStructFunction(UArg arg0, UArg arg1){
  Event_post(operationEventHandle, WEAR_SLEEPMODE_EVT);
}

void bigtimeClockStructFunction(UArg arg0, UArg arg1){
  Event_post(operationEventHandle, WEAR_BIGTIME_EVT);
}

void measureClockTaskStructFunction(UArg arg0, UArg arg1){
  // Semaphore_post(txMeasSem);
  Event_post(operationEventHandle, SBP_MEASUREMENT_EVT);
}


void simpleApplication_init(){

  buttonPinHandle = PIN_open(&buttonPinState, buttonPinTable);
  if(!buttonPinHandle) {
      /* Error initializing button pins */
      while(1);
  }

  /* Setup callback for button pins */
    if (PIN_registerIntCb(buttonPinHandle, &SimpleBLEPeripheral_GPIOInterrupt) != 0) {
        /* Error registering button callback function */
        while(1);
    }




  /* Create event used internally for state changes */
  Event_Params eventParam;
  Event_Params_init(&eventParam);
  Event_construct(&operationEvent, &eventParam);
  operationEventHandle = Event_handle(&operationEvent);
  
  /* Create Task */
  Task_Params mainTaskParams;
  Task_Params_init(&mainTaskParams);
  mainTaskParams.stackSize = MAINTASKS_TACKSIZE;
  mainTaskParams.priority = MAINTASK_PRIORITY;
  mainTaskParams.stack = &mainTaskStack;
  Task_construct(&mainTaskStruct, SimpleBLEPeripheral_createTask, &mainTaskParams, NULL);
  
  // /* Create Task */
  // mainTaskParams.stackSize = MEAS_TACKSIZE;
  // mainTaskParams.priority = MEAS_PRIORITY;
  // mainTaskParams.stack = &measTaskStack;
  // Task_construct(&measTaskStruct, Measurement_createTask, &mainTaskParams, NULL);

  /* Create Clock */
  Clock_Params clkParams;
  Clock_Params_init(&clkParams);

  
  clkParams.period = 20000;
  clkParams.startFlag = true;
  Clock_construct(&mainClockStruct, (Clock_FuncPtr)mainClockTaskStructFunction, 1, &clkParams);
  mainClockHandle = Clock_handle(&mainClockStruct);

  clkParams.period = 900000;
  clkParams.startFlag = true;
  Clock_construct(&countSleepClockStruct, (Clock_FuncPtr)countSleepClockStructFunction, 500000, &clkParams);
  countSleepClockHandle = Clock_handle(&countSleepClockStruct);

  clkParams.period = 100000;
  Clock_construct(&bigtimeClockStruct, (Clock_FuncPtr)bigtimeClockStructFunction, 1, &clkParams);
  bigtimeClockHandle = Clock_handle(&bigtimeClockStruct);

  clkParams.period = 100;
  clkParams.startFlag = false;
  Clock_construct(&measureClockStruct, (Clock_FuncPtr)measureClockTaskStructFunction, 1, &clkParams);
  measureClockHandle = Clock_handle(&measureClockStruct);
}
