




#ifndef __BIGTIME_H__
#define __BIGTIME_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* data */

    extern void Clock_init(int newId);  // Constructor
    extern void Clock_updateTime(uint8_t *newDateTime);
    extern int Clock_getSecond();
    extern int Clock_getMinute();
    extern int Clock_getHour();
    extern int Clock_getDay();
    extern int Clock_getMonth();
    extern int Clock_getYear();
    extern int Clock_getCentury();
    extern int Clock_getMillenium();
    
    extern void Clock_setSecond();

#ifdef __cplusplus
}
#endif

#endif
