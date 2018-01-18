/*
 *    ======== wearableDevice.cpp ========
 */

#include <xdc/std.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
// #include <time.h>
/* Driver Header files */
#include "Board.h"
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>

#include <ti/sysbios/BIOS.h>
//#include <ti/sysbios/hal/Seconds.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>

/* Board Header files */
#include "bigtime.h"
#include "OledDisplay/WDsDisplay.h"
#include "devices/dvMMA9553.h"

#include "devices/heart_rate_3click/AFE4404.h"
// //Library for signal processing
#include "devices/signalProcessing/DCRemove.h"
#include "devices/signalProcessing/lowPassFilter.h"
#include "devices/signalProcessing/kalman.h"

/**Include API**
*/
// #include "heart_rate.h"
/*************************
*/
#include "wearableDevice.h"


/***** Variable declarations *****/
Task_Struct mainTaskStruct;
Task_Handle mainTaskHandle;
Char mainTaskStack[MAINTASKS_TACKSIZE];

Clock_Struct countSleepClockStruct;
Clock_Handle countSleepClockHandle;
Clock_Struct mainClockStruct;
Clock_Handle mainClockHandle;
Clock_Struct checkStateClockStruct;
Clock_Handle checkStateClockHandle;
 Clock_Struct measureClockStruct;
 static Clock_Handle measureClockHandle;

Event_Struct operationEvent;
Event_Handle operationEventHandle;
Event_Struct operationEvent1;
Event_Handle operationEventHandle1;

typedef struct
{
  bool head;
  bool body;
} eventchange;

typedef struct {
  bool bleIcon;
  char text[20];
  uint8_t batteryLevel;
}elementhead;


static eventchange eventChange;
static elementhead elementHead;

uint8_t modeMain = 0;
bool  taskExitIsSet = true, 
      enableSleep = false, 
      stateDisplay = Display_ON ;
uint8_t BufferForPedometer[20];
bool updateClock = true;


uint8_t count =0;
uint8_t countBleElement;
long averageGreenValue = 0;
long greenSumXValue = 0;
long greenSumXSum = 0;
long dataLedGreen,dataLedGreenDCRemove,dataLedGreenLPF,dataLedGreenKalman;
long dataLedRed;
long dataLedIR;
long dataSum;
long signalGreenArray[NUM_SUM_GREEN], signalSumGreenArray[NUM_AVERAGE_GREEN];

// variable for heart rate processing
uint32_t timeArray[NUM_TIME];
uint32_t timeSum = 0;
uint8_t timeCount = 0, pulseCount, numPulseToUpdate;
float averageTime =0;
bool pulseUp = false, pulseDw = true;
/////////////////////////////////////////
int8_t arrayForBLETransmitter[NUM_BLE_ARRAY];

/////Variable for HR and spO2Number
const uint8_t spO2LUT[43] = { 100,100,100,100,99,99,99,99,99,99,98,98,98,98,
                              98,97,97,97,97,97,97,96,96,96,96,96,96,95,95,
                              95,95,95,95,94,94,94,94,94,93,93,93,93,93
};
uint16_t heartRateNumber = 0;
uint8_t spO2Number = 0;
float redACValueSqSum = 0;
float irACValueSqSum = 0;
uint32_t samplesRecorded;
int doDai = 0;

I2C_Handle i2c;
I2C_Params i2cParams;
I2C_Transaction i2cTransaction;

void gpioButtonFxn0(uint_least8_t index){
  /* Clear the GPIO interrupt and toggle an LED */
  GPIO_toggle(Board_GPIO_LED0);
  enableSleep = false;
  Clock_stop(countSleepClockHandle);
  // Turn on OLED
  if(stateDisplay == Display_OFF){
    modeMain=2;
    Event_post(operationEventHandle, EVENT_UPDATE_OLED);
    Clock_start(mainClockHandle);
  }

  //Increate ModeMain
  modeMain++;
  modeMain%=3;

  if(modeMain==1){
    heartRateNumber = 0;
    taskExitIsSet = false;
    Clock_start(measureClockHandle);
  } 
  else{
    taskExitIsSet = true;
    Clock_stop(measureClockHandle);
  }
}

void gpioButtonFxn1(uint_least8_t index){
  GPIO_toggle(Board_GPIO_LED1);
}

void clock_main(){
  int hour, min, sec;
  if(updateClock){
    updateClock = false;
    sec = Clock_getSecond();
    hour = Clock_getHour();
    min = Clock_getMinute();
    WDsDisplay__clearDisplay();
    WDsDisplay__Clock_set(hour, min);
    WDsDisplay__Colon_toogle();
    eventChange.body = true;
    if(!enableSleep){
      enableSleep = true;
      Clock_start(countSleepClockHandle);
    }
  }
}

void heart_show(uint16_t rate){
  static uint8_t _rate;
  WDsDisplay__Clear_body();
  WDsDisplay__Heartrate_status();
  if(_rate != rate){
    _rate = rate;
  }
  WDsDisplay__Heartrate_number(600);
  eventChange.body = true;
}

// static int16_t heart_measurement(uint16_t _next){
//   uint16_t _heartRate = _next;
//   uint32_t count = 2000000;
//   while(count--);
//   _heartRate = 50;
//   return _heartRate;
// }

void heart_main(){
  //Start measure heart rate
  heart_show(heartRateNumber);
}

void pedometer_show(uint16_t stepCount) {
  WDsDisplay__Clear_body();
  WDsDisplay__Footsteps_status();
  WDsDisplay__Footsteps_number(stepCount);
  eventChange.body = true;
}

uint16_t pedometer_measurement(void){
  uint8_t countdown = 255;
  static uint16_t StepCount = 10;
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

void pedometer_main(void){
  // static uint16_t steps = 100;
  uint16_t _steps = pedometer_measurement();
  pedometer_show(_steps);

  // if(steps != _steps){
  //   steps = _steps;
  //   pedometer_show(_steps);
  //   Clock_stop(countSleepClockHandle);
  // }else{
  //   pedometer_show(steps);
    if(!enableSleep){
      enableSleep = true;
      Clock_start(countSleepClockHandle);
    }
  // }
}

void HienThi_init(void){
  WDsDisplay__begin(SSD1306_SWITCHCAPVCC, 0x3C);
  WDsDisplay__clearDisplay();
  WDsDisplay__display();
  delay_mma9553();
}


void display_head(void){
  WDsDisplay__Clear_head();
  WDsDisplay__setTextSize(1);
  WDsDisplay__setTextColor(WHITE);
  WDsDisplay__setCursor(40, 8);
  WDsDisplay__print((uint8_t*)elementHead.text);
  WDsDisplay__Bluetooth_icon(elementHead.bleIcon);
  WDsDisplay__Battery_set(elementHead.batteryLevel, false);
  eventChange.head = true;
}

void HienThi_showHead(void){
  display_head();
  if(eventChange.head){
    WDsDisplay__Display_head();
    eventChange.head = false;
  }
}

void HienThi_showBody(uint8_t mode){  
  switch(mode){
    case 0:  clock_main(); break;
    case 1:  heart_main(); break;
    case 2:  pedometer_main(); break;
    default: break;
  }
  if(taskExitIsSet){
    TurnONOFF(false);
  }else{
    TurnONOFF(true);
  }
  if(eventChange.body){
    WDsDisplay__Display_body();
    eventChange.body = false;
  }
}

void HienThi_TurnONOFFDisplay(bool turn){
  WDsDisplay__onoff(turn);
  if(turn){
    stateDisplay = Display_ON;
    Clock_start(mainClockHandle);
  }
  else {
    stateDisplay = Display_OFF;
    Clock_stop(mainClockHandle);
    Clock_stop(countSleepClockHandle);
  }
}

// /////////////////Reset value
void SpO2_Reset()
{
    redACValueSqSum = 0;
    irACValueSqSum = 0;
    spO2Number = 0;
    samplesRecorded = 0;
}
uint32_t timeTicks;
void measureProcessFunction(){
  {
    countBleElement++;
    timeSum = 0;
    timeCount = 0;
    dataLedGreen = get_GR_data();
    dataLedRed = get_RED_data();
    dataLedIR = get_IR_data();
    greenSumXValue = 0;
    greenSumXSum = 0;
    dataSum = dataLedGreen + dataLedRed + dataLedIR;
    pulseCount = 0;
  }

  //Filter
  dataLedGreenDCRemove = dcRemoval(dataLedGreen, 0.9);
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
  }
  arrayForBLETransmitter[countBleElement] = averageGreenValue>>8;
  //////////////////////////////////////////////

  greenSumXValue = dcRemoval1(greenSumXValue, 0.9);
  averageGreenValue = dcRemoval2(averageGreenValue, 0.9);

  // Processing HR and spO2Number
  if(dataSum> 90000 && dataSum < 180000 && dataLedGreen > 30000 && dataLedGreen < 60000){
      irACValueSqSum += dataLedRed * dataLedRed;
      redACValueSqSum += dataLedIR * dataLedIR;
      samplesRecorded++;
      if(greenSumXValue > averageGreenValue && pulseDw){
          pulseUp = true;
          pulseDw = false;
          pulseCount++;
          timeTicks = Clock_getTicks();
          for(count = 0 ; count < NUM_TIME - 1 ; count++){
              if(timeArray[count+1] > timeArray[count] && timeArray[count] != 0){
                  timeSum += timeArray[count+1] - timeArray[count];
                  timeCount++;
              }
              timeArray[count] = timeArray[count+1];
          }
          timeArray[NUM_TIME-1] = timeTicks;
          uint8_t x = SPEED_UPDATE_NEW_VALUE;
          switch (x) {
            case 1:
                numPulseToUpdate = 10;
                break;
            case 2:
                numPulseToUpdate = 6;
                break;
            case 3:
                numPulseToUpdate = 3;
                break;
            default:
                numPulseToUpdate = 1;
                break;
          }
          if(pulseCount > numPulseToUpdate){
            if(timeCount > NUM_TIME / 2){
                averageTime = timeSum/timeCount;
                heartRateNumber = 6000000/averageTime;

                float acSqRatio = 100.0 * log(redACValueSqSum/samplesRecorded) / log(irACValueSqSum/samplesRecorded);
                static uint8_t index = 0;

                if (acSqRatio > 66) {
                    index = (uint8_t)acSqRatio - 66;
                } else if (acSqRatio > 50) {
                    index = (uint8_t)acSqRatio - 50;
                }
                SpO2_Reset();
                spO2Number = spO2LUT[index];
            }
            pulseCount = 0;
          }
      }
      if(greenSumXValue < averageGreenValue && pulseUp){
          pulseUp = false;
          pulseDw = true;
      }
  }
  if(dataSum< 110000 || dataSum > 250000 || dataLedGreen < 40000 || dataLedGreen > 90000){
      for(count = 0 ; count < NUM_TIME; count++){
          timeArray[count] = 0;
      }
  }
}


/*
 *  ======== mainTaskStructFunction ========
 */
void mainTaskStructFunction(UArg arg0, UArg arg1){

  /* Open GPIO for usage */
  {
    GPIO_init();
    GPIO_PinConfig pinConfig = Board_GPIO_BUTTON0 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING;
    GPIO_PinConfig pinConfig1 = Board_GPIO_BUTTON1 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING;
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
    elementHead.bleIcon = false;
    strcpy( elementHead.text, "MEMSTECH");
    elementHead.batteryLevel = 90; // get_BatteryPercent();
  }
  {
    pedometer_init();
  }
  {
   AFE_Init();
    // Setup kalman param
   SimpleKalmanFilter(2, 2, 0.0008);
   Clock_stop(measureClockHandle);
  }

  uint8_t countdown = 10;
  while(true){
    /* Wait for event */
    uint32_t events = Event_pend(operationEventHandle, 0, EVENT_ALL, BIOS_WAIT_FOREVER);
    
    if(events & EVENT_UPDATE_OLED){
      // events &= ~EVENT_UPDATE_OLED;
      GPIO_toggle(Board_GPIO_LED1);
      if(stateDisplay == Display_OFF)
        HienThi_TurnONOFFDisplay(true);
      HienThi_showHead();
      HienThi_showBody(modeMain);
    }

    if(events & EVENT_MEASUREMENT){
      // events &= ~EVENT_MEASUREMENT;
      GPIO_toggle(Board_GPIO_LED0);
      measureProcessFunction();
    }
    if(events & EVENT_SLEEPMODE_COMPLETE && stateDisplay == Display_ON){
      // events &= ~EVENT_SLEEPMODE_COMPLETE;
      HienThi_TurnONOFFDisplay(false);
    }
  }
}

void countSleepClockStructFunction(UArg arg0, UArg arg1){
  Event_post(operationEventHandle, EVENT_SLEEPMODE_COMPLETE);
}

void checkStateClockFunction(){
    updateClock = true;
    Clock_setSecond();
}
void mainClockTaskStructFunction(UArg arg0, UArg arg1){
  Event_post(operationEventHandle, EVENT_UPDATE_OLED);
}

void measureClockTaskStructFunction(){
  Event_post(operationEventHandle, EVENT_MEASUREMENT);
}


void primaryTaskStructFunction(UArg arg0, UArg arg1){
  uint16_t _temp = 50;
  // while(true){
  //   if (Semaphore_pend(sem0Handle, BIOS_WAIT_FOREVER))
    {
     // if (taskExitIsSet) continue;
     // // do something in 1sec
     // _temp = heart_measurement(_temp);
     // if (taskExitIsSet) continue;
     // // do something in 1sec
     // _temp = heart_measurement(_temp);
     // if (taskExitIsSet) continue;
     // // do something in 1sec
     // _temp = heart_measurement(_temp);
     // if (taskExitIsSet) continue;
     // // do something in 1sec
     // _temp = heart_measurement(_temp);
     // if (taskExitIsSet) continue;
     // // do something in 1sec
     // _temp = heart_measurement(_temp);
     // if (taskExitIsSet) continue;
      // do something in 1sec
      // heartRateNumber = heart_measurement(_temp);
    }
    // if(!enableSleep){
    //   Clock_start(countSleepClockHandle);
    //   enableSleep = true;
    // }
  // }
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
  mainTaskParams.stackSize = MAINTASKS_TACKSIZE;
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

  clkParams.period = 100;
  clkParams.startFlag = true;
  Clock_construct(&measureClockStruct, (Clock_FuncPtr)measureClockTaskStructFunction, 10000, &clkParams);
  measureClockHandle = Clock_handle(&measureClockStruct);

  clkParams.period = 900000;
  clkParams.startFlag = true;
  Clock_construct(&countSleepClockStruct, (Clock_FuncPtr)countSleepClockStructFunction, 500000, &clkParams);
  countSleepClockHandle = Clock_handle(&countSleepClockStruct);

  clkParams.period = 100000;
  Clock_construct(&checkStateClockStruct, (Clock_FuncPtr)checkStateClockFunction, 4, &clkParams);
  checkStateClockHandle = Clock_handle(&checkStateClockStruct);
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

