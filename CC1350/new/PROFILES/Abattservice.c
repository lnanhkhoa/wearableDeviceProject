/******************************************************************************

 @file  battservice.c

 @brief This file contains the Battery service.

 Group: WCS, BTS
 Target Device: CC2650, CC2640, CC1350

 ******************************************************************************
 
 Copyright (c) 2012-2016, Texas Instruments Incorporated
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
#include <xdc/std.h>
#include <stdbool.h>
#include <driverlib/aon_batmon.h>

#include "bcomdef.h"

#include "Abattservice.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

#define BATT_LEVEL_VALUE_IDX        2 // Position of battery level in attribute array
#define BATT_LEVEL_VALUE_CCCD_IDX   3 // Position of battery level CCCD in attribute array

#define BATT_LEVEL_VALUE_LEN        1

/**
 * GATT Characteristic Descriptions
 */
#define GATT_DESC_LENGTH_UUID            0x3111 // Used with Unit percent

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

uint16_t battMaxLevel = BATT_MAX_VOLTAGE;
static uint8_t battLevel = 0;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static uint8_t battMeasure(void);


/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      Batt_Setup
 *
 * @brief   Set up which ADC source is to be used. Defaults to VDD/3.
 *
 * @param   adc_ch - ADC Channel, e.g. HAL_ADC_CHN_AIN6
 * @param   minVal - max battery level
 * @param   maxVal - min battery level
 * @param   sCB - HW setup callback
 * @param   tCB - HW tear down callback
 * @param   cCB - percentage calculation callback
 *
 * @return  none.
 */
void Batt_Setup(uint16_t maxVal)
{
  battMaxLevel = maxVal;
}

/*********************************************************************
 * @fn          Batt_MeasLevel
 *
 * @brief       Measure the battery level and update the battery
 *              level value in the service characteristics.  If
 *              the battery level-state characteristic is configured
 *              for notification and the battery level has changed
 *              since the last measurement, then a notification
 *              will be sent.
 *
 * @return      Success
 */
bStatus_t Batt_MeasLevel(void)
{
  uint16_t level;

  level = battMeasure();
  if(level >=0 && level <=100){
    // Update level
    battLevel = (uint8_t)(level & 0x00FF);
  }
  return SUCCESS;
}

/*********************************************************************
 * @fn      battMeasure
 *
 * @brief   Measure the battery level with the ADC and return
 *          it as a percentage 0-100%.
 *
 * @return  Battery level.
 */
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
  percent = ((percent* 100) / battMaxLevel);
  
  return percent;
}


// uint16_t Batt_testing(void){
//   // return AONBatMonBatteryVoltageGet();
//   return 80;
// }

/*********************************************************************
*********************************************************************/
