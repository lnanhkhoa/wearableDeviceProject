// middleCommunication.h

#ifndef MIDDLECOMMUNICATION_H_
#define MIDDLECOMMUNICATION_H_


#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

#include <stdint.h>
/*********************************************************************
 * CONSTANTS
 */
   
/*********************************************************************
 * MACROS
 */
  
/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task creation function for the Heart Rate.
 */

void functionCreateRandomMeasurement(void);

void functionCreateRandomValueMeasurement(void);

void functionCreateRandomPedometerMeasurement(void);

uint8_t* getHeartRate(void);

uint8_t* getSpO2(void);

uint8_t* createFrame(void);

uint8_t* getHeartRateRawMeasurement(void);

uint8_t* getHeartRateValueMeasurement(void);

uint8_t* getPedometerValueMeasurement(void);


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
