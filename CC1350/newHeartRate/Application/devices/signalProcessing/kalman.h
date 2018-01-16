//////////////////////////////////////////////////////////
//        Kalman filter Example - Nguyen Anh Hoang
//                      22/12/2017
//////////////////////////////////////////////////////////

#ifndef KALMAN_H_
#define KALMAN_H_

void SimpleKalmanFilter(float mea_e, float est_e, float q);
float updateEstimate(float mea);
void setMeasurementError(float mea_e);
void setEstimateError(float est_e);
void setProcessNoise(float q);
float getKalmanGain();

#endif
