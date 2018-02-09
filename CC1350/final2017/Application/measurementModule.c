// measurementModule.c
// 
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "devices/heart_rate_3click/AFE4404.h"
#include "devices/signalProcessing/DCRemove.h"
#include "devices/signalProcessing/lowPassFilter.h"
#include "devices/signalProcessing/kalman.h"

#include "measurementModule.h"


int8_t arrayForBLETransmitter[NUM_ARRAY_RAWDATA_HEARTRATE];
uint8_t timeSavedCount = 0, pulseCount, numPulseToUpdate, timeValidCount;
uint32_t timeDiff = 0;
uint32_t timeSum = 0;
uint8_t countBleElement;
uint32_t averageGreenValue = 0;
uint32_t dataLedGreen;
float dataLedGreenDCRemove, dataLedGreenLPF, dataLedGreenKalman;
uint32_t dataLedRed;
uint32_t dataLedIR;
uint32_t dataSum;
uint32_t signalGreenArray[NUM_SUM_GREEN] = {0x00};
uint32_t signalSumGreenArray[NUM_AVERAGE_GREEN] = {0x00};
uint32_t greenSumXValue;
uint32_t greenSumXSum;

void measurement_init(){
	AFE_Init();
	SimpleKalmanFilter(2, 2, 0.0008);
	AFE_TurnLED(false);
}

void getRawData(){
  countBleElement++;
  timeSum = 0;
  timeSavedCount = 0;
  timeValidCount = 0;
  dataLedGreen = get_GR_data();
  dataLedRed = get_RED_data();
  dataLedIR = get_IR_data();
  greenSumXValue = 0;
  greenSumXSum = 0;
  dataSum = dataLedGreen + dataLedRed + dataLedIR;
}

void filterModule(){	
	//Filter
  dcRemoval(dataLedGreen, 0.9, 0, &dataLedGreenDCRemove);
  LowPass_filter(dataLedGreenDCRemove, &dataLedGreenLPF);
  updateEstimate(dataLedGreenDCRemove, &dataLedGreenKalman);
}


void calc(){
  uint32_t count = 0;
  //Calculator Sum X Green Value
  // for(count = 0 ; count < NUM_SUM_GREEN ; count++){
  //   *(signalGreenArray + count) = *(signalGreenArray+count+1);
  // }
  // *(signalGreenArray + NUM_SUM_GREEN) = dataLedGreenKalman;

  // for(count = 0 ; count < NUM_SUM_GREEN ; count++){
  //   greenSumXValue += *(signalGreenArray + count);
  //   }
  
  // //////////////////////////////////////////////
  // // Calculator Average X Green Value
  // for(count = 0 ; count < NUM_AVERAGE_GREEN ; count++){
  //   *(signalSumGreenArray+count) = *(signalSumGreenArray+count+1);
  // }
  // *(signalSumGreenArray + NUM_AVERAGE_GREEN) = greenSumXValue;

  // for(count = 0 ; count < NUM_AVERAGE_GREEN ; count++){
  //   greenSumXSum += *(signalSumGreenArray + count);
  // }
  // averageGreenValue = (greenSumXSum + *(signalSumGreenArray +NUM_AVERAGE_GREEN - 1) * 5)/(NUM_AVERAGE_GREEN+4);
  // // // //////////////////////////////////////////////
  // // // //Save to BLE array
  // if(countBleElement > 19){
  //   countBleElement = 0;

  //     //Here  ------------------------------>  Send Data to smartphone

  //     /////////////////////////End///////////////////////////////////

  //   for(count = 0 ; count < NUM_ARRAY_RAWDATA_HEARTRATE ; count++){
  //     arrayForBLETransmitter[count] = 0;
  //   }
  //  }
  //  arrayForBLETransmitter[countBleElement] = averageGreenValue>>8;
  //  //////////////////////////////////////////////

  //  dcRemoval(greenSumXValue, 0.9, 1, &greenSumXValue);
  //  dcRemoval(averageGreenValue, 0.9, 2, &averageGreenValue);


}
void getRealData(uint16_t *heartRateNumber){
	*heartRateNumber = countBleElement;
}

void TurnONOFF_HRModule(bool turn){
	AFE_TurnLED(turn);
}
