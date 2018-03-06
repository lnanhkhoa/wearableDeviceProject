

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "board.h"

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
// #include <ti/drivers/GPIO.h>
// #include <ti/drivers/gpio/GPIOCC26XX.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CCC26XX.h>
// #include <ti/drivers/UART.h>

// #include <ti/devices/DeviceFamily.h>
// #include DeviceFamily_constructPath(driverlib/aon_batmon.h)
// #include DeviceFamily_constructPath(driverlib/trng.h)
#include <driverlib/aon_batmon.h>
#include <driverlib/trng.h>


#include "bigtime/bigtime.h"
#include "devices/dvMMA9553.h"
#include "OledDisplay/WDsDisplay.h"

#include "wearableDevice.h"

static uint8_t battMeasure(void);
static void Batt_MeasLevel(void);
static void clock_main();
static void heart_show(uint16_t rate);
static void heart_main();
static void pedometer_show(uint16_t stepCount);
static uint16_t pedometer_measurement(void);
static void pedometer_main(void);
static void HienThi_init(void);
static void display_head(void);
static void HienThi_showHead(void);
static void HienThi_showBody(uint8_t mode);
static void HienThi_TurnONOFFDisplay(bool turn);
static void SpO2_Reset();
static void checkStateClockFunction(void);

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

elementhead elementHead;
eventchange eventChange;

uint8_t modeMain = 1;
bool  enableMeasurement = true;
bool      enableSleep = false, 
      stateDisplay = Display_ON,
      updateClock = true;
uint8_t BufferForPedometer[20];
uint16_t stepsCountNumber;

extern uint16_t heartRateNumber;
extern PIN_Handle pinHandle;
extern PIN_State pinState;


extern I2C_Handle i2c;
extern I2C_Params i2cParams;
extern I2C_Transaction i2cTransaction;

extern Semaphore_Handle txSendSem;
// typedef unsigned char   uint8;
// extern uint8 simpleProfileChar3;
// extern uint8 simpleProfileChar4[];

void gpioButtonFxn0(void){
  enableSleep = false;
  if(stateDisplay == Display_OFF){
    modeMain=2;
  }

  //Increate ModeMain
  modeMain++;
  modeMain%=3;
  enableMeasurement = (modeMain == 1?true:false);
}

static uint8_t battMeasure(void)
{
  static uint32_t percent;
  // Read the battery voltage (V), only the first 12 bits
  percent = AONBatMonBatteryVoltageGet();
  // Convert to from V to mV to avoid fractions.
  // Fractional part is in the lower 8 bits thus converting is done as follows:
  // (1/256)/(1/1000) = 1000/256 = 125/32
  // This is done most effectively by multiplying by 125 and then shifting
  // 5 bits to the right.
  percent = (percent * 125) >> 5;
  // Convert to percentage of maximum voltage.
  percent = (percent-BATT_MIN_VOLTAGE)* 100 / BATT_SCOPE_VOLTAGE;
  return percent;
}

static void Batt_MeasLevel(void)
{
  uint16_t level;
  level = battMeasure();
  if(level >=0 && level <=100){
    // Update level
    elementHead.batteryLevel = (uint8_t)(level & 0x00FF);
    // simpleProfileChar3 = elementHead.batteryLevel;
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
    eventChange.body = true;
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
  eventChange.body = true;
}

static void heart_main(){
  //Start measure heart rate
  heart_show(heartRateNumber);
}

static void pedometer_show(uint16_t stepCount){
  WDsDisplay__Clear_body();
  WDsDisplay__Footsteps_status();
  WDsDisplay__Footsteps_number(stepCount);
  eventChange.body = true;
}

static uint16_t pedometer_measurement(void){
  uint8_t countdown = 255;
  static uint16_t StepCount = 11;
  pedometer_cmd_readstatus();
  while(countdown--){
    pedometer_Read(BufferForPedometer, 2);
    delay_mma9553();
    if (BufferForPedometer[1] == 0x80)
    {
      pedometer_Read(BufferForPedometer, 16);
      StepCount = (BufferForPedometer[6] <<8) | BufferForPedometer[7];
      // memcpy(simpleProfileChar4, (BufferForPedometer+6), 4);
      // memcpy(simpleProfileChar4+2, (BufferForPedometer+12), 2);
      break;
    }
  }
  return StepCount;
}

static void pedometer_main(void){
  uint16_t _steps = pedometer_measurement();
  if(stepsCountNumber != _steps){
    stepsCountNumber = _steps;
    enableSleep = false;
  }
  pedometer_show(stepsCountNumber);
}

static void HienThi_init(void){
  WDsDisplay__begin( SSD1306_SWITCHCAPVCC, 0x3C);
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
static void HienThi_showBody(uint8_t mode){
  switch(mode){
    case 0:  clock_main();      break;
    case 1:  heart_main();      break;
    case 2:  pedometer_main();  break;
    default: break;
  }
  if(eventChange.body){
    WDsDisplay__Display_body();
    eventChange.body = false;
  }
}

static void HienThi_TurnONOFFDisplay(bool turn){
  WDsDisplay__onoff(turn);
  if(turn){
    stateDisplay = Display_ON;
  //   Clock_start(mainClockHandle);
  }
  else {
    stateDisplay = Display_OFF;
  //   Clock_stop(mainClockHandle);
  //   Clock_stop(countSleepClockHandle);
  }
}

static void checkStateClockFunction(void){
  updateClock = true;
    Clock_setSecond();
    Batt_MeasLevel();
    stepsCountNumber = pedometer_measurement();
}

static void updateTime(uint8_t *newdatetime);
static void updateConfigProfile(uint8_t *newconfigprofile);


void wearableDevice_init(void){

  // GPIO_write(Board_GPIO_LED0, 0); // reset Pedometer
  // delay_mma9553();
  // GPIO_write(Board_GPIO_LED0, 1); // reset Pedometer


  PIN_setOutputValue(pinHandle, Board_LED0, 0);
  delay_mma9553();
  PIN_setOutputValue(pinHandle, Board_LED0, 1);
  I2C_init();
  I2C_Params_init(&i2cParams);
  i2cParams.bitRate = I2C_400kHz;
  i2cParams.transferMode = I2C_MODE_BLOCKING;
  i2c = I2C_open(Board_I2C_TMP, &i2cParams);
  if (i2c == NULL){
    while (1);
  }

  Clock_init(1);
  WDsDisplay__init(OLED_RESET);
  HienThi_init();
  elementHead.bleIcon = false;
  strcpy( elementHead.text, "MEMSTECH");
  Batt_MeasLevel();
  
  pedometer_init();
}


void wearableDevice_UpdateOLED(void){
  if(stateDisplay == Display_OFF) HienThi_TurnONOFFDisplay(true);
  HienThi_showHead();
  HienThi_showBody(modeMain);
}


void wearableDevice_SleepMode(void){
  HienThi_TurnONOFFDisplay(false);
}

bool wearableDevice_ResetSleepMode(){
  if(modeMain == 1) enableSleep = false;
  return enableSleep;
}

void wearableDevice_SetupSleepMode(void){
  enableSleep = true;
}

void wearableDevice_BigTime(void){
  checkStateClockFunction();
}

void wearableDevice_stateBLE(bool stateBLE){
  elementHead.bleIcon = stateBLE;
}

void wearableDevice_updateTime(uint8_t *pointerDateTime){
  Clock_updateTime(pointerDateTime);
}


void updateConfigProfile(uint8_t *newconfigprofile);
