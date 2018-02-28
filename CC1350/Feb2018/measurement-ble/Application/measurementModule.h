// measurementModule.h
// 
// 
#ifndef MEASUREMENTMODULE_H_
#define MEASUREMENTMODULE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define NUM_SUM_GREEN 20
#define NUM_AVERAGE_GREEN 20
#define NUM_TIME 20
#define NUM_ARRAY_RAWDATA_HEARTRATE 20
#define SPEED_UPDATE_NEW_VALUE 3  // 4 speed level 1 - 2 - 3 - 4


void HR_measurementInit();
void HR_measurementTask();
void HR_getRawData();
void HR_filterModule();
void HR_calc();

uint8_t HR_getRealData();
void HR_TurnONOFF_HRModule(bool turn);


#ifdef __cplusplus
}
#endif

#endif
