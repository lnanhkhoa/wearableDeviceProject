/*
 * WDsDisplay.h
 *
 *  Created on: Sep 5, 2017
 *      Author: AppsTI
 */

#ifndef _WDSDISPLAY_H_
#define _WDSDISPLAY_H_


/* BLE icon's location */
#define BLEicon_x 0
#define BLEicon_y 0
#define BLEicon_width 16
#define BLEicon_height 16

/* Battery icon's location */
#define BATicon_x 112
#define BATicon_y 0
#define BATicon_width 16
#define BATicon_height 16

/* Font */
#define font_width  6
#define font_height 8


/* Clock */
#define clock_textSize   3
#define clock_textColor  WHITE
#define clock_cursorx    19
#define clock_cursory    28

/* Heart Rate */
#define heart_x 10
#define heart_y 16
#define sizeBigHeart 48
#define sizeSmallHeart 32

/* Foot Steps */
#define footsteps_x 10
#define footsteps_y 16
#define sizeFootsteps 48

    extern void WDsDisplay__init(int8_t rst);
    extern void WDsDisplay__begin(int8_t vccstate, int8_t i2caddr);
    extern void WDsDisplay__Bluetooth_icon(bool enable);
    extern void WDsDisplay__Battery_charging(void);
    extern void WDsDisplay__Battery_set(uint8_t percent, bool charge);
    extern void WDsDisplay__Clock_set();
    extern void WDsDisplay__Colon_toogle(void);
    extern void WDsDisplay__Heartrate_status(void);
    extern void WDsDisplay__Heartrate_number(uint16_t number); // -1 is wait
    extern void WDsDisplay__Footsteps_status(void);
    extern void WDsDisplay__Footsteps_number(uint16_t number);

    void WDsDisplay__Display_head(void);
    void WDsDisplay__Display_body(void);
    void WDsDisplay__Clear_head(void);
    void WDsDisplay__Clear_body(void);
    void WDsDisplay__Fill_body(void);
    void WDsDisplay__Fillfull(void);
    
    void 
        WDsDisplay__clear_16x16(int16_t x, int16_t y, uint16_t color),
        WDsDisplay__clear_18x24(int16_t x, int16_t y, uint16_t color),
        WDsDisplay__clear_heart(int16_t x, int16_t y, uint16_t color),
        WDsDisplay__clear_character(int16_t x, int16_t y, uint16_t fontsize, uint16_t length, uint16_t color);

    void WDsDisplay__clearDisplay();
    void WDsDisplay__display();
    void WDsDisplay__onoff(bool mode);
    
    void WDsDisplay__print(uint8_t *str);
    void WDsDisplay__setCursor(int16_t x, int16_t y);
    void WDsDisplay__setTextColor(uint16_t c);
    void WDsDisplay__setTextSize(uint8_t s);
    uint8_t *WDsDisplay__getBuffer(void);

#endif /* OLEDDISPLAY_H_ */
