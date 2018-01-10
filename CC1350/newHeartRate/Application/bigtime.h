




#ifndef __BIGTIME_H__
#define __BIGTIME_H__

/* data */

    void Clock_init(int newId);  // Constructor

    void Clock_TimerDim(uint8_t count);
    int Clock_getId();
    int Clock_getMicrosecond();
    int Clock_getMillisecond();
    int Clock_getSecond();
    int Clock_getMinute();
    int Clock_getHour();
    int Clock_getDay();
    int Clock_getMonth();
    int Clock_getYear();
    int Clock_getCentury();
    int Clock_getMillenium();

    void Clock_setMicrosecond();
    void Clock_setMillisecond();
    void Clock_setMillisecond_inputnMil(int nMilliseconds);
    void Clock_setSecond();
    void Clock_setMinute();
    void Clock_setHour();
    void Clock_setDay();
    void Clock_setMonth();
    void Clock_setYear();
    void Clock_setCentury();
    void Clock_setMillenium();

#endif
