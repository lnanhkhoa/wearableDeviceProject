/******************************************************************************

 @file  heart_rate.h

 @brief This file contains the Heart Rate sample application definitions and
        prototypes.

 Group: WCS, BTS
 Target Device: CC2650, CC2640, CC1350

 ******************************************************************************
 
 Copyright (c) 2014-2016, Texas Instruments Incorporated
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

#ifndef HEARTRATE_H
#define HEARTRATE_H

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
   

/*********************************************************************
 * MACROS
 */

// Convert BPM to RR-Interval for data simulation purposes.
#define HEARTRATE_BPM2RR(bpm)            ((uint16) 60 * 1024 / (uint16) (bpm))

/*********************************************************************
 * CONSTANTS
 */

// Fast advertising interval in 625us units.
#define DEFAULT_FAST_ADV_INTERVAL                       32

// Duration of fast advertising duration in ms.
#define DEFAULT_FAST_ADV_DURATION                       30000

// Slow advertising interval in 625us units.
#define DEFAULT_SLOW_ADV_INTERVAL                       1600

// Slow advertising duration in ms (set to 0 for continuous advertising).
#define DEFAULT_SLOW_ADV_DURATION                       0

// How often to perform heart rate periodic event.
#define DEFAULT_HEARTRATE_PERIOD                        2000

// Whether to enable automatic parameter update request when a connection is
// formed.
#define DEFAULT_ENABLE_UPDATE_REQUEST                   GAPROLE_LINK_PARAM_UPDATE_WAIT_BOTH_PARAMS

// Minimum connection interval (units of 1.25ms) if automatic parameter update
// request is enabled.
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL               200

// Maximum connection interval (units of 1.25ms) if automatic parameter update
// request is enabled.
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL               1600

// Slave latency to use if automatic parameter update request is enabled.
#define DEFAULT_DESIRED_SLAVE_LATENCY                   1

// Supervision timeout value (units of 10ms) if automatic parameter update
// request is enabled.
#define DEFAULT_DESIRED_CONN_TIMEOUT                    1000

// Battery level is critical when it is less than this %
#define DEFAULT_BATT_CRITICAL_LEVEL                     30

// Battery measurement period in ms
#define DEFAULT_BATT_PERIOD                             15000

// How often to perform periodic event (in msec)
#define SBP_PERIODIC_EVT_PERIOD                         1000

// How often to perform periodic event (in msec)
#define PEDOMETER_PERIODIC_EVT_PERIOD                   1000


// Arbitrary values used to simulate measurements.
#define HEARTRATE_BPM_DEFAULT                           73
#define HEARTRATE_BPM_MAX                               80
#define HEARTRATE_ENERGY_INCREMENT                      10
#define HEARTRATE_FLAGS_IDX_MAX                         7

// Task configuration
#define HEARTRATE_TASK_PRIORITY                         1
#define HEARTRATE_TASK_STACK_SIZE                       1024

// Internal events for RTOS application.
// Peripheral State Change Event.
#define HEARTRATE_STATE_CHANGE_EVT                      (uint16_t)(1 << 0)

// Heart Rate events.
#define HEARTRATE_MEAS_EVT                              (uint16_t)(1 << 1)
#define HEARTRATE_MEAS_PERIODIC_EVT                     (uint16_t)(1 << 2)

// Battery Event.
#define HEARTRATE_BATT_EVT                              (uint16_t)(1 << 3)
#define HEARTRATE_BATT_PERIODIC_EVT                     (uint16_t)(1 << 4)

// Pedometer events 
#define PEDO_CHAR_CHANGE_EVT                            (uint16_t)(1 << 5)

// Simple peripheral events.
#define SBP_CHAR_CHANGE_EVT                             (uint16_t)(1 << 6)
#define SBP_PERIODIC_EVT                                (uint16_t)(1 << 7)

// Key Press events.
#define HEARTRATE_KEY_CHANGE_EVT                        (uint16_t)(1 << 8)

#define HEARTRATE_MEAS_LEN                              9
#define HEARTRATE_RAWMEAS_LEN                           9



/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task creation function for the Heart Rate.
 */
extern void HeartRate_createTask(void);


/* API for any header files need */
extern bool get_BLEState(void);

extern uint8_t get_BatteryPercent(void);

// extern uint16_t batt_testing(void);
/*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* HEARTRATE_H */
