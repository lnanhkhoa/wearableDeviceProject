//////////////////////////////////////////////////////////
//        DC Remove Example - Nguyen Anh Hoang
//                      22/12/2017
//////////////////////////////////////////////////////////

#include"DCRemove.h"

float dcRemoval(float signalIn, float alpha) {
  float signalOut ;
  static float wOld;
  float wNew = signalIn + alpha * wOld;
  signalOut = wNew - wOld;
  wOld = wNew;
  return signalOut;
}
