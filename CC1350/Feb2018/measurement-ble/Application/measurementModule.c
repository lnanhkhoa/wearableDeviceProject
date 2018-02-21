

#include "stdint.h"
#include "stdbool.h"
#include "measurementModule.h"

void measurementInit();
void measurementTask();

void getRawData();
void filterModule();
void calc();
void getRealData(uint16_t *heartRateNumber);
void TurnONOFF_HRModule(bool turn);
