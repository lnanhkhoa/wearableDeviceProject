/*
 *    ======== wearableDevice.cpp ========
 */

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <cstring>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/hal/Seconds.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Event.h>

/* Driver Header files */
#include "Board.h"
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>

/* Board Header files */
#include "bigtime.h"
#include "devices/dvMMA9553.h"
#include "OledDisplay/WDsDisplay.h"

/**Include API**
*/
// #include "heart_rate.h"
/*************************
*/
#include "wearableDevice.h"


/***** Variable declarations *****/
Task_Struct mainTaskStruct;
Char mainTaskStack[TASKSTACKSIZE];
Task_Handle mainTaskHandle;

Clock_Struct countSleepClockStruct;
static Clock_Handle countSleepClockHandle;
Clock_Struct mainClockStruct;
static Clock_Handle mainClockHandle;
Clock_Struct checkStateClockStruct;
static Clock_Handle checkStateClockHandle;

Event_Struct operationEvent;
static Event_Handle operationEventHandle;
Event_Struct operationEvent1;
static Event_Handle operationEventHandle1;

/* Semaphore used to gate for shutdown */
Semaphore_Struct sem0Struct;
Semaphore_Handle sem0Handle;
Semaphore_Struct sem1Struct;
Semaphore_Handle sem1Handle;

struct eventchange{
  bool head;
  bool body;
}eventChange;

struct elementhead{
  bool bleIcon;
  char text[20];
  uint8_t batteryLevel;
}elementHead;

uint8_t modeMain = 0;
bool onoff = true;
uint32_t timestamp = 0;
uint16_t heartRateNumber = 0;
bool  taskExitIsSet = false, 
      enableSleep = false, 
      stateDisplay = Display_ON ;

I2C_Handle i2c;
I2C_Params i2cParams;
I2C_Transaction i2cTransaction;

static void gpioButtonFxn0(uint_least8_t index){
  /* Clear the GPIO interrupt and toggle an LED */
  GPIO_toggle(Board_GPIO_LED0);
  enableSleep = false;
  Clock_stop(countSleepClockHandle);
  
  // Turn off OLED
  if(stateDisplay == Display_OFF){
    modeMain=3;
    Event_post(operationEventHandle, EVENT_TURNON_DISPLAY | EVENT_UPDATE_OLED);
  }

  //Increate ModeMain
  modeMain++;
  modeMain%=4;
  if(modeMain==1){
    heartRateNumber = 0;
    taskExitIsSet = false;
    // Semaphore_post(sem0Handle);
  }
  else{
    taskExitIsSet = true;
  }
}

static void gpioButtonFxn1(uint_least8_t index){
  GPIO_toggle(Board_GPIO_LED1);
}

static void clock_main(){
  static UInt32 seconds = 10; // different to Second_get() in initial running
  if(seconds != Seconds_get()){
    uint8_t hour, min, sec;
    seconds = Seconds_get(); 
    sec = Clock_getSecond();
    hour = Clock_getHour();
    min = Clock_getMinute();
    WDsDisplay__Clear_body();
    WDsDisplay__Clock_set(hour, min);
    WDsDisplay__Colon_toogle();
    eventChange.body = true;
    if(!enableSleep){
      // Semaphore_post(sem1Handle);
      enableSleep = true;
      Clock_start(countSleepClockHandle);
    }
  }
}

static void heart_show(uint8_t rate){
  static uint8_t _rate;
  WDsDisplay__Clear_body();
  WDsDisplay__Heartrate_status();
  if(_rate != rate){
    _rate = rate;
  }
  WDsDisplay__Heartrate_number(_rate);
  eventChange.body = true;
}

static int16_t heart_measurement(uint16_t _next){
  uint16_t _heartRate = _next;
  uint32_t count = 2000000;
  while(count--);
  _heartRate = 50;
  return _heartRate;
}

static void heart_main(){
  //Start measure heart rate
  heart_show(heartRateNumber);
}

static void pedometer_show(uint16_t stepCount) {
  WDsDisplay__Clear_body();
  WDsDisplay__Footsteps_status();
  WDsDisplay__Footsteps_number(stepCount);
  eventChange.body = true;
}

static uint16_t pedometer_measurement(void){
  // uint8_t Buf[20], countdown = 255;
  // uint16_t StepCount = 50;
  // pedometer_cmd_readstatus();
  // while(countdown--){
  //   pedometer_Read(Buf, 2);
  //   delay_mma9553();
  //   if (Buf[1] == 0x80)
  //   {
  //     pedometer_Read(Buf, 16);
  //     StepCount = (Buf[6] <<8) | Buf[7];
  //     break;
  //   }
  // }
  return 50;
}

static void pedometer_main(void){
  uint16_t steps = 0;
  
  // if(modeMain == 3){
    // steps = batt_testing();
  // }
  // steps = pedometer_measurement();

  pedometer_show(steps);
  if(!enableSleep){
    enableSleep = true;
    Clock_start(countSleepClockHandle);
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
  WDsDisplay__print((uint8_t*)elementHead.text);
  WDsDisplay__Bluetooth_icon(elementHead.bleIcon);
  WDsDisplay__Battery_set(elementHead.batteryLevel, false);
  eventChange.head = true;
}

static void HienThi_showHead(void){
  display_head();
  if(eventChange.head){
    WDsDisplay__Display_head();
    eventChange.head = false;
  }
}

static bool testing = true;
static void toogle_Testing(void){
  if(testing){
    WDsDisplay__clearDisplay();
  }
  else{
    WDsDisplay__Fillfull();
  }
  testing = !testing;
}

static void testing_main(void){
  if(!eventChange.body){
    toogle_Testing();
    GPIO_toggle(Board_GPIO_LED1);
    eventChange.body = true;
  }
}

static void HienThi_showBody(uint8_t mode){
  switch(mode){
    case 0:  clock_main(); break;
    case 1:  heart_main(); break;
     case 2: pedometer_main(); break;
    case 3:  testing_main(); break;
    default: break;
  }
  if(eventChange.body){
    WDsDisplay__Display_body();
    eventChange.body = false;
  }
}

static void HienThi_TurnONOFFDisplay(bool turn){
  WDsDisplay__onoff(turn);
  onoff = turn;
}

/*
 *  ======== mainTaskStructFunction ========
 */
static void mainTaskStructFunction(UArg arg0, UArg arg1){
  /* Open GPIO for usage */
  GPIO_init();
  GPIO_PinConfig pinConfig = Board_GPIO_BUTTON0 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING;
  GPIO_setConfig(Board_GPIO_BUTTON0, pinConfig);
  GPIO_setCallback(Board_GPIO_BUTTON0, gpioButtonFxn0);
  GPIO_setCallback(Board_GPIO_BUTTON1, gpioButtonFxn1);
  GPIO_enableInt(Board_GPIO_BUTTON0);
  GPIO_enableInt(Board_GPIO_BUTTON1);

  I2C_init();
  I2C_Params_init(&i2cParams);
  i2cParams.bitRate = I2C_400kHz;
  i2cParams.transferMode = I2C_MODE_BLOCKING;
  i2c = I2C_open(Board_I2C_TMP, &i2cParams);
  if (i2c == NULL){
    while (1);
  }

  pedometer_init();
  // Clock_init(1);
  // WDsDisplay__init(OLED_RESET);
  // HienThi_init();
  // elementHead.bleIcon = false;
  // strcpy( elementHead.text, "MEMSTECH");
  // elementHead.batteryLevel = get_BatteryPercent();
  // elementHead.batteryLevel = 30;
  uint32_t events;
  while(1){
    /* Wait for event */
    events = Event_pend(operationEventHandle, 0, EVENT_ALL, BIOS_WAIT_FOREVER);

   if(events & EVENT_UPDATE_OLED && (stateDisplay == Display_ON)){
      // HienThi_showHead();
      // HienThi_showBody(modeMain);
    }
  }
}

static void countSleepClockStructFunction(UArg arg0, UArg arg1){
  Event_post(operationEventHandle, EVENT_SLEEPMODE_COMPLETE);
}

static void checkStateClockFunction(){
  // if(get_BLEState())
  //   elementHead.bleIcon = true;
  // else
  //   elementHead.bleIcon = false;
  elementHead.batteryLevel = 30;

}
static uint32_t _time;
static void mainClockTaskStructFunction(UArg arg0, UArg arg1){
  _time = Seconds_get();
  if(timestamp != _time){
    timestamp = _time;
    Clock_setSecond();
  }

  Event_post(operationEventHandle, EVENT_UPDATE_OLED);
}

static void primaryTaskStructFunction(UArg arg0, UArg arg1){
  uint16_t _temp = 50;
  while(true){
    if (Semaphore_pend(sem0Handle, BIOS_WAIT_FOREVER))
    {
      if (taskExitIsSet) continue;
      // do something in 1sec
      _temp = heart_measurement(_temp);
      if (taskExitIsSet) continue;
      // do something in 1sec
      _temp = heart_measurement(_temp);
      if (taskExitIsSet) continue;
      // do something in 1sec
      _temp = heart_measurement(_temp);
      if (taskExitIsSet) continue;
      // do something in 1sec
      _temp = heart_measurement(_temp);
      if (taskExitIsSet) continue;
      // do something in 1sec
      _temp = heart_measurement(_temp);
      if (taskExitIsSet) continue;
      // do something in 1sec
      heartRateNumber = heart_measurement(_temp);
    }
    if(!enableSleep){
      Clock_start(countSleepClockHandle);
      enableSleep = true;
    }
  }
}

static void sleepModeTaskStructFunction(UArg arg0, UArg arg1){

}


void wearableDevice_init(){

  /* Create event used internally for state changes */
  Event_Params eventParam;
  Event_Params_init(&eventParam);
  Event_construct(&operationEvent, &eventParam);
  operationEventHandle = Event_handle(&operationEvent);

  Event_Params_init(&eventParam);
  Event_construct(&operationEvent1, &eventParam);
  operationEventHandle1 = Event_handle(&operationEvent1);
  

  /* Create Task */
  Task_Params mainTaskParams;
  Task_Params_init(&mainTaskParams);
  mainTaskParams.stackSize = TASKSTACKSIZE;
  mainTaskParams.priority = MAINTASK_PRIORITY;
  mainTaskParams.stack = &mainTaskStack;
  Task_construct(&mainTaskStruct, mainTaskStructFunction, &mainTaskParams, NULL);
  

  /* Create Clock */
  Clock_Params clkParams;
  Clock_Params_init(&clkParams);
  clkParams.period = 20000;
  clkParams.startFlag = true;
  Clock_construct(&mainClockStruct, (Clock_FuncPtr)mainClockTaskStructFunction, 1, &clkParams);
  mainClockHandle = Clock_handle(&mainClockStruct);

  clkParams.period = 1000000;
  clkParams.startFlag = true;
  Clock_construct(&countSleepClockStruct, (Clock_FuncPtr)countSleepClockStructFunction, 500000, &clkParams);
  countSleepClockHandle = Clock_handle(&countSleepClockStruct);

  clkParams.period = 200000;
  Clock_construct(&checkStateClockStruct, (Clock_FuncPtr)checkStateClockFunction, 4, &clkParams);
  checkStateClockHandle = Clock_handle(&checkStateClockStruct);

   /* Configure shutdown semaphore. */
  Semaphore_Params semParams;
  Semaphore_Params_init(&semParams);
  Semaphore_construct(&sem0Struct, 0, &semParams);
  sem0Handle = Semaphore_handle(&sem0Struct);

  Semaphore_construct(&sem1Struct, 0, &semParams);
  sem1Handle = Semaphore_handle(&sem1Struct);
}




/**********
  API
 **********/


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

// void updateTime(uint8_t *newdatetime){
//     if(checkCorrectTime(newdatetime))
//         Clock_updateTime(newdatetime);
// }

// void updateConfigProfile(uint8_t *newconfigprofile){
//     uint8_t gender = *(newconfigprofile);
//     newconfigprofile++;
//     pedometer_write_config(newconfigprofile);
//     strcpy( elementHead.text, "OK");
// }

