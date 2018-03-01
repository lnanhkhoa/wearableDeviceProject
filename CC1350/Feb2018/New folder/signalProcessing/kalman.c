//////////////////////////////////////////////////////////
//        Kalman filter Example - Nguyen Anh Hoang
//                      22/12/2017
//////////////////////////////////////////////////////////

#include "kalman.h"
#include <math.h>

float err_measure;
float err_estimate;
float Q;
float current_estimate;
float last_estimate;
float kalman_gain;

void SimpleKalmanFilter(float mea_e, float est_e, float q)
{
  err_measure=mea_e;
  err_estimate=est_e;
  Q = q;
}

float updateEstimate(float mea)
{
  kalman_gain = err_estimate/(err_estimate + err_measure);
  current_estimate = last_estimate + kalman_gain * (mea - last_estimate);
  err_estimate =  (1.0 - kalman_gain)*err_estimate + fabs(last_estimate-current_estimate)*Q;
  last_estimate=current_estimate;

  return current_estimate;
}

void setMeasurementError(float mea_e)
{
  err_measure=mea_e;
}

void setEstimateError(float est_e)
{
  err_estimate=est_e;
}

void setProcessNoise(float q)
{
  Q=q;
}

float getKalmanGain() {
  return kalman_gain;
}

