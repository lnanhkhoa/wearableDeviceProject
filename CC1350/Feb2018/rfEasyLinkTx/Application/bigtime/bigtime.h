




#ifndef __BIGTIME_H__
#define __BIGTIME_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* data */

    void Clock_init(uint8_t newId);  // Constructor
    void Clock_updateTime(uint8_t *newDateTime);
    void Clock_TimerDim(uint8_t count);
    uint8_t Clock_getId();
    int Clock_getMicrosecond();
    int Clock_getMillisecond();
    uint8_t Clock_getSecond();
    uint8_t Clock_getMinute();
    uint8_t Clock_getHour();
    uint8_t Clock_getDay();
    uint8_t Clock_getMonth();
    uint8_t Clock_getYear();
    uint8_t Clock_getCentury();
    uint8_t Clock_getMillenium();

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

#ifdef __cplusplus
}
#endif

#endif
