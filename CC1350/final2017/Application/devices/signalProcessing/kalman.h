//////////////////////////////////////////////////////////
//        Kalman filter Example - Nguyen Anh Hoang
//                      22/12/2017
//////////////////////////////////////////////////////////

#ifndef KALMAN_H_
#define KALMAN_H_

#ifdef __cplusplus
extern "C"
{
#endif

void SimpleKalmanFilter(float mea_e, float est_e, float q);
void updateEstimate(float mea, float *update_estimate);
void setMeasurementError(float mea_e);
void setEstimateError(float est_e);
void setProcessNoise(float q);
float getKalmanGain();


#ifdef __cplusplus
}
#endif

#endif
