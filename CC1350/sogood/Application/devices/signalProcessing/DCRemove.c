//////////////////////////////////////////////////////////
//        DC Remove Example - Nguyen Anh Hoang
//                      22/12/2017
//////////////////////////////////////////////////////////

#include "DCRemove.h"

float wOld = 0;
float wOld1 = 0;
float wOld2 = 0;

float dcRemoval(float signalIn, float alpha) {
  float signalOut ;
  float wNew = signalIn + alpha * wOld;
  signalOut = wNew - wOld;
  wOld = wNew;
  return signalOut;
}

float dcRemoval1(float signalIn, float alpha) {
  float signalOut ;
  float wNew = signalIn + alpha * wOld1;
  signalOut = wNew - wOld1;
  wOld1 = wNew;
  return signalOut;
}

float dcRemoval2(float signalIn, float alpha) {
  float signalOut ;
  float wNew = signalIn + alpha * wOld2;
  signalOut = wNew - wOld2;
  wOld2 = wNew;
  return signalOut;
}
