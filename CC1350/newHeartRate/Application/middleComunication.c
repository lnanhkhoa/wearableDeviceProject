// middleCommunication.c


/*********************************************************************
 * INCLUDES
 */
#include <time.h>
#include <stdlib.h>
#include <stdint.h>

#include "middleComunication.h"

/*********************************************************************
 * CONSTANTS
 */
uint8_t heartRate[10];
uint8_t spO2[10];
uint8_t frame[20];
uint8_t heartRateValue[3];
uint8_t pedometerValue[3];
   
/*********************************************************************
 * MACROS
 */
  
/*********************************************************************
 * FUNCTIONS
 */

void functionCreateRandomMeasurement(){
	uint8_t i;
	for( i=0; i<10; i++){
		heartRate[i] =50 + rand()%50;
		spO2[i] = 90 + rand()%10;
	}
}

void functionCreateRandomValueMeasurement(){
	heartRateValue[0]=0x02;
	uint8_t i;
	for( i=1; i<3; i++){
		heartRateValue[i] =50 + rand()%50;
	}
}

void functionCreateRandomPedometerMeasurement(){
	uint8_t i;
	for( i=0; i<3; i++){
		pedometerValue[i] =50 + rand()%50;
	}
}

uint8_t* getHeartRate(void){
	return heartRate;
}

uint8_t* getSpO2(void){
	return spO2;
}

uint8_t* createFrame(void){
	// how many
	// length of heart Rate
	// length of SpO2
	frame[0] = 2;
	frame[1] = frame[2] = 5;
	uint8_t length1 = frame[1], length2 = frame[2];
	uint8_t i;
	for(i = 0; i<length1; i++){
		frame[3 + i] = *(getHeartRate()+i);
	}
	
	for (i = 0; i < length2; ++i)
	{
		frame[3 + length1 + i] = *(getSpO2()+i);
	}
	return frame;
}

uint8_t *getHeartRateRawMeasurement(void){
	srand(time(NULL));
	functionCreateRandomMeasurement();
	uint8_t *p = createFrame();
	return p;
}

uint8_t *getHeartRateValueMeasurement(void){
	srand(time(NULL));
	functionCreateRandomValueMeasurement();
	return heartRateValue;
}

uint8_t *getPedometerValueMeasurement(void){
	srand(time(NULL));
	functionCreateRandomPedometerMeasurement();
	return pedometerValue;

}
/*
 * Task creation function for the Heart Rate.
 */


/*********************************************************************
*********************************************************************/
