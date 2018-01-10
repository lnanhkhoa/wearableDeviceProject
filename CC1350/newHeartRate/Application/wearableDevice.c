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
#include <ti/display/Display.h>
/* Board Header files */

/* Library Header files */
#include "bigtime.h"
 #include "devices/dvMMA9553.h"
#include "OledDisplay/WDsDisplay.h"

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

uint8_t modeMain = 3;
bool onoff = true;
volatile UInt32 timestamp = 0;
volatile uint16_t heartRateNumber = 0;
uint16_t percent = 60;
bool  taskExitIsSet = false, 
      enableSleep = false, 
      stateDisplay = Display_ON ;

I2C_Handle i2c;
I2C_Params i2cParams;
I2C_Transaction i2cTransaction;
Display_Handle dispHandle;

void gpioButtonFxn0(uint_least8_t index){
  /* Clear the GPIO interrupt and toggle an LED */
  GPIO_toggle(Board_GPIO_LED0);
  enableSleep = false;
  // Clock_stop(countSleepClockHandle);
  
  if(stateDisplay == Display_OFF){
    modeMain=3;
    Event_post(operationEventHandle, EVENT_TURNON_DISPLAY | EVENT_UPDATE_OLED);
  }
  modeMain++;
  modeMain%=4;

  if(modeMain==1){
    heartRateNumber = 0;
    taskExitIsSet = false;
    Semaphore_post(sem0Handle);
  }
  else{
    taskExitIsSet = true;
  }
}

void gpioButtonFxn1(uint_least8_t index){
  GPIO_toggle(Board_GPIO_LED1);
}

void display_head(void){
  WDsDisplay__Clear_head();
  WDsDisplay__setTextSize(1);
  WDsDisplay__setTextColor(WHITE);
  WDsDisplay__setCursor(40, 8);
  WDsDisplay__print((uint8_t*)elementHead.text);
  WDsDisplay__Bluetooth_icon(elementHead.bleIcon);
  WDsDisplay__Battery_set(percent, false);
  eventChange.head = true;
}

void clock_main(){
  static UInt32 seconds = 10; // different to Second_get() in initial running
  if(seconds != Clock_getSecond()){
    uint8_t hour, min;
    seconds = Clock_getSecond();
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

void heart_show(uint8_t rate){
  static uint8_t _rate;
  WDsDisplay__Clear_body();
  WDsDisplay__Heartrate_status();
  if(_rate != rate){
    _rate = rate;
  }
  WDsDisplay__Heartrate_number(_rate);
  eventChange.body = true;
}

int16_t heart_measurement(uint16_t _next){
  uint16_t _heartRate = _next;
  uint32_t count = 2000000;
  while(count--);
  _heartRate = 50;
  return _heartRate;
}

void heart_main(){
  //Start measure heart rate
  heart_show(heartRateNumber);
}

void pedometer_show(int16_t stepCount) {
  static uint8_t _stepCount = 0;
  WDsDisplay__Clear_body();
  WDsDisplay__Footsteps_status();
  _stepCount++;
  WDsDisplay__Footsteps_number(_stepCount);
  eventChange.body = true;
}

int16_t pedometer_measurement(void){
  // uint8_t Buf[20];
  static int16_t StepCount;// countdown = 300;
  // pedometer_cmd_readstatus();
  // while(countdown--){
    // pedometer_Read(Buf, 2);
    // delay_mma9553();
    // if (Buf[1] == 0x80)
    // {
      // pedometer_Read(Buf, 16);
      // StepCount = (Buf[6] <<8) | Buf[7];
      StepCount = 50;
      // break;
    // }
  // }
  return StepCount;
}

void pedometer_main(void){
  static int16_t steps = 0;
  // steps = pedometer_measurement();
  pedometer_show(steps);
  if(!enableSleep){
    enableSleep = true;
    Clock_start(countSleepClockHandle);
  }
}

void HienThi_init(void){
  WDsDisplay__begin(SSD1306_SWITCHCAPVCC, 0x3C);
  WDsDisplay__clearDisplay();
  WDsDisplay__display();
}

void HienThi_showHead(void){
  display_head();
  if(eventChange.head){
    WDsDisplay__Display_head();
    eventChange.head = false;
  }
}

bool testing = true;
void toogle_Testing(void){
  if(testing){
    WDsDisplay__Clear_head();
    WDsDisplay__Clear_body();
  }
  else{
    WDsDisplay__Fillfull();
  }
  testing = !testing;
}

void testing_main(void){
  if(!eventChange.body){
    toogle_Testing();
    eventChange.body = true;
  }
}

void HienThi_showBody(uint8_t mode){
  switch(mode){
    case 0:  clock_main(); break;
    case 1:  heart_main(); break;
    case 2:  pedometer_main(); break;
    case 3:  testing_main(); break;
    default: break;
  }
  if(eventChange.body){
    WDsDisplay__Display_body();
    eventChange.body = false;
  }
}

void HienThi_TurnONOFFDisplay(bool turn){
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
  i2c = I2C_open(Board_I2C_TMP, &i2cParams);
  if (i2c == NULL){
    while (1);
  }

  Clock_init(1);
  WDsDisplay__init(OLED_RESET);
  HienThi_init();
  pedometer_init();
  elementHead.bleIcon = false;
  strcpy( elementHead.text, "MEMSTECH");
  elementHead.batteryLevel = 90;
  uint32_t events;
  while(1){
    /* Wait for event */
    events = Event_pend(operationEventHandle, 0, EVENT_ALL, BIOS_WAIT_FOREVER);

//    if(events & EVENT_UPDATE_OLED && (stateDisplay == Display_ON)){
    if(true){
      HienThi_showHead();
      // HienThi_showBody(modeMain);
    }
  }
}

void countSleepClockStructFunction(UArg arg0, UArg arg1){
  Event_post(operationEventHandle, EVENT_SLEEPMODE_COMPLETE);
}

void checkStateClockFunction(UArg arg0, UArg arg1){
  // if(gapProfileState == GAPROLE_CONNECTED)
    // elementHead.bleIcon = true;
  // else
    // elementHead.bleIcon = false;
}
static UInt32 _time;
void mainClockTaskStructFunction(UArg arg0, UArg arg1){
  _time = Seconds_get();
  if(timestamp != _time){
    timestamp = _time;
    Clock_setSecond();
  }

  Event_post(operationEventHandle, EVENT_UPDATE_OLED);
}

void primaryTaskStructFunction(UArg arg0, UArg arg1){
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

void sleepModeTaskStructFunction(UArg arg0, UArg arg1){

}


void wearableDevice_init(){

  /* Create event used internally for state changes */
  Event_Params eventParam;
  Event_Params_init(&eventParam);
  Event_construct(&operationEvent, &eventParam);
  operationEventHandle = Event_handle(&operationEvent);

  Event_Params eventParam1;
  Event_Params_init(&eventParam1);
  Event_construct(&operationEvent1, &eventParam1);
  operationEventHandle1 = Event_handle(&operationEvent1);
  
  Task_Params mainTaskParams;
  Task_Params_init(&mainTaskParams);
  mainTaskParams.stackSize = TASKSTACKSIZE;
  mainTaskParams.priority = MAINTASK_PRIORITY;
  mainTaskParams.stack = &mainTaskStack;
  Task_construct(&mainTaskStruct, mainTaskStructFunction, &mainTaskParams, NULL);
  
  Clock_Params clkParams;
  Clock_Params_init(&clkParams);
  clkParams.period = 20000;
   // clkParams.arg = (UArg)&HienThi;
  clkParams.startFlag = true;
  Clock_construct(&mainClockStruct, (Clock_FuncPtr)mainClockTaskStructFunction, 1, &clkParams);
  mainClockHandle = Clock_handle(&mainClockStruct);

   /* Configure shutdown semaphore. */
  Semaphore_Params semParams;
  Semaphore_Params_init(&semParams);
  Semaphore_construct(&sem0Struct, 0, &semParams);
  sem0Handle = Semaphore_handle(&sem0Struct);

  Semaphore_construct(&sem1Struct, 0, &semParams);
  sem1Handle = Semaphore_handle(&sem1Struct);
}

