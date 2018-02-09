

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

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
#include "OledDisplay/WDsDisplay.h"






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








void wearableDevice_init();