/******************************************************************************

 @file  simple_peripheral.h

 @brief This file contains the Simple BLE Peripheral sample application
        definitions and prototypes.

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

#ifndef SIMPLEBLEPERIPHERAL_H
#define SIMPLEBLEPERIPHERAL_H

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

// Internal Events for RTOS application
#define SBP_STATE_CHANGE_EVT                (uint16_t)(1 << 0)
#define SBP_CHAR_CHANGE_EVT                 (uint16_t)(1 << 1)
#define SBP_PERIODIC_EVT                    (uint16_t)(1 << 2)
#define SBP_CONN_EVT_END_EVT                (uint16_t)(1 << 3)
#define EVENT_UPDATE_OLED                   (uint16_t)(1 << 4)
#define EVENT_BIGTIME_CHECKSTATE            (uint16_t)(1 << 5)
#define EVENT_SLEEPMODE_COMPLETE            (uint16_t)(1 << 6)
#define EVENT_TURNON_DISPLAY                (uint16_t)(1 << 7)
#define EVENT_MEASUREMENT                	  (uint16_t)(1 << 8)



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
   
#define NUM_SUM_GREEN 20
#define NUM_AVERAGE_GREEN 30
#define NUM_TIME 20
#define NUM_BLE_ARRAY 20
#define SPEED_UPDATE_NEW_VALUE 3  // 4 speed level 1 - 2 - 3 - 4


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task creation function for the Simple BLE Peripheral.
 */
extern void SimpleBLEPeripheral_createTask(void);


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SIMPLEBLEPERIPHERAL_H */
