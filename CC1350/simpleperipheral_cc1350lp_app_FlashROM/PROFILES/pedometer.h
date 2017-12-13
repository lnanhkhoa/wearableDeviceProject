/******************************************************************************

 @file  simple_gatt_profile.h

 @brief This file contains the Simple GATT profile definitions and prototypes
        prototypes.

 Group: WCS, BTS
 Target Device: CC2650, CC2640, CC1350

 ******************************************************************************
 
 Copyright (c) 2010-2016, Texas Instruments Incorporated
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
 Release Name: ble_sdk_2_02_00_31
 Release Date: 2016-06-16 18:57:29
 *****************************************************************************/

#ifndef SIMPLEGATTPROFILE_H
#define SIMPLEGATTPROFILE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */

// Profile Parameters
#define Pedometer_CHAR1                   0  // RW uint8 - Profile Characteristic 1 value 
#define Pedometer_CHAR2                   1  // RW uint8 - Profile Characteristic 2 value
#define Pedometer_CHAR3                   2  // RW uint8 - Profile Characteristic 3 value
#define Pedometer_CHAR4                   3  // RW uint8 - Profile Characteristic 4 value
#define Pedometer_CHAR5                   4  // RW uint8 - Profile Characteristic 4 value
  
// Simple Profile Service UUID
#define Pedometer_SERV_UUID               0xFFF0
    
// Key Pressed UUID
#define Pedometer_CHAR1_UUID            0xFFF1
#define Pedometer_CHAR2_UUID            0xFFF2
#define Pedometer_CHAR3_UUID            0xFFF3
#define Pedometer_CHAR4_UUID            0xFFF4
#define Pedometer_CHAR5_UUID            0xFFF5
  
  
// Profile Range Values
#define ACCEL_RANGE_2G                20
#define ACCEL_RANGE_4G                40    
#define ACCEL_RANGE_8G                80

// Simple Keys Profile Services bit fields
#define Pedometer_SERVICE               0x00000002

// Length of Characteristic 5 in bytes
#define Pedometer_CHAR5_LEN           5  

/*********************************************************************
 * TYPEDEFS
 */

  
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * Profile Callbacks
 */

// Callback when a characteristic value has changed
typedef void (*pedometerChange_t)( uint8 paramID );

typedef struct
{
  pedometerChange_t        pfnPedometerChange;  // Called when characteristic value changes
} pedometerCBs_t;

    

/*********************************************************************
 * API FUNCTIONS 
 */


/*
 * Pedometer_AddService- Initializes the Simple GATT Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 */

extern bStatus_t Pedometer_AddService( uint32 services );

/*
 * Pedometer_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t Pedometer_RegisterAppCBs( pedometerCBs_t *appCallbacks );

/*
 * Pedometer_SetParameter - Set a Simple GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 */
extern bStatus_t Pedometer_SetParameter( uint8 param, uint8 len, void *value );
  
/*
 * Pedometer_GetParameter - Get a Simple GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 */
extern bStatus_t Pedometer_GetParameter( uint8 param, void *value );


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SIMPLEGATTPROFILE_H */
