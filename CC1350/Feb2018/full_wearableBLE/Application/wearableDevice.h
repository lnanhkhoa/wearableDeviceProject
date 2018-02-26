

#ifndef WEARABLEDEVICE_H
#define WEARABLEDEVICE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
*  EXTERNAL VARIABLES
*/

/*********************************************************************
 * CONSTANTS
 */


#define OLED_RESET 4
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2
#define Display_ON  true
#define Display_OFF false
#define BLACK 0
#define WHITE 1
#define INVERSE 2

// Max voltage (mV)
#define BATT_MAX_VOLTAGE            3800
#define BATT_MIN_VOLTAGE            2500
#define BATT_SCOPE_VOLTAGE					(BATT_MAX_VOLTAGE - BATT_MIN_VOLTAGE)


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task creation function for the Simple BLE Peripheral.
 */

extern void gpioButtonFxn0(void);

extern void wearableDevice_init(void);

extern void wearableDevice_UpdateOLED(void);

extern void wearableDevice_SleepMode(void);

extern bool wearableDevice_ResetSleepMode(void);

extern void wearableDevice_BigTime(void);

extern void wearableDevice_stateBLE(bool stateBLE);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif 
