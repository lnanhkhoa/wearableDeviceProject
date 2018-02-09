//////////////////////////////////////////////////////////
//        DC Remove Example - Nguyen Anh Hoang
//                      22/12/2017
//////////////////////////////////////////////////////////

#include "DCRemove.h"

float arrayWOld[3]={0, 0, 0};

void dcRemoval(float signalIn, float alpha, int index, float* signalOut) {
  float wOldindex  = arrayWOld[index];

  float wNew = signalIn + alpha * wOldindex;
  *signalOut = wNew - wOldindex;
  wOldindex = wNew;
  arrayWOld[index] = wOldindex;
}
