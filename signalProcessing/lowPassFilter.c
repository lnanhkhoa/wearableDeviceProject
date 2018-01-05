//////////////////////////////////////////////////////////
//        Low pass filter Example - Nguyen Anh Hoang
//                      22/12/2017
//        -fc = 2Hz
//        -Sample Rate = 1000Hz(0.001s)
//        -Transfer function:
//                 12.57
//               ---------
//               s + 12.57
//        - Transfer function in discret mode:
//                 0.01249
//              ------------
//               z - 0.9875
//        - Equation implement
//               y(n+1) = 0.9875*y(n)+0.01249
//////////////////////////////////////////////////////////

float LowPass_filter(float signalIn,float signalOut){
    signalOut = 0.9875*signalOut + 0.01249*signalIn;
    return signalOut;
}
