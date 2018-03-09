/*
 * WDsDisplay.cpp
 *
 *  Created on: Sep 5, 2017
 *      Author: AppsTI
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "libDisplay/bluetooth_enabled.h"
#include "libDisplay/battery.h"
#include "libDisplay/heart.h"
#include "libDisplay/stepcount.h"

#include "Adafruit_SSD1306.h"
#include "WDsDisplay.h"

/* BLE icon's location*/

bool colon_status, footsteps_status;
uint8_t heart_status;

const unsigned char *arrayBatteryStatus[11] = {
        battery_warning, 
        battery_10p, 
        battery_20p, 
        battery_30p, 
        battery_40p, 
        battery_50p,
        battery_60p, 
        battery_70p, 
        battery_80p, 
        battery_90p, 
        battery_full 
    };
const unsigned char *arrayCardiogram[4] = {cardiogram, cardiogram1, cardiogram2, cardiogram3};



void WDsDisplay__init(int8_t rst)
{
    AdafruitSSD1306_init(rst);
    colon_status = footsteps_status = false;
    heart_status = 0;
}

void WDsDisplay__begin(int8_t vccstate, int8_t i2caddr){
    AdafruitSSD1306_begin(vccstate, i2caddr, true);
}

void WDsDisplay__Bluetooth_icon(bool enable)
{
    WDsDisplay__clear_16x16(BLEicon_x, BLEicon_y, 0);
    if(enable)
        AdafruitSSD1306_drawBitmap(BLEicon_x, BLEicon_y, bluetooth_enabled , BLEicon_width, BLEicon_height, 1, 0);
}

void WDsDisplay__Battery_charging(void)
{
    WDsDisplay__clear_16x16(BATicon_x, BATicon_y, 0);
    AdafruitSSD1306_drawBitmap(BATicon_x, BATicon_y, battery_charging, BATicon_width, BATicon_height, 1, 0);
}

void WDsDisplay__Battery_set(uint8_t percent, bool charge)
{
    WDsDisplay__clear_16x16(BATicon_x, BATicon_y, 0);
    if(percent>100) percent = 100;
    uint8_t per= percent/10;
    AdafruitSSD1306_drawBitmap(BATicon_x, BATicon_y, arrayBatteryStatus[per], BATicon_width, BATicon_height, 1, 0);
}

extern uint8_t hour, minute;
void WDsDisplay__Clock_set(){
    uint8_t buffer[6];
    AdafruitSSD1306_setTextSize(clock_textSize);
    AdafruitSSD1306_setTextColor(clock_textColor);
    AdafruitSSD1306_setCursor(clock_cursorx, clock_cursory);
    if(hour<24 && minute<60){
        if(hour<10){
            buffer[0] = '0';
            buffer[1] = '0' + hour;

        }else{
            buffer[0] = '0' + (hour/10);
            buffer[1] = '0' + (hour%10);
        }

        if(colon_status)
             buffer[2] = ':';
        else buffer[2] = ' ';

        if(minute<10){
            buffer[3] = '0';
            buffer[4] = '0' + minute;
        }
        else{
            buffer[3] = '0' + (minute/10);
            buffer[4] = '0' + (minute%10);
        }
        buffer[5] = 0;

        AdafruitSSD1306_print((uint8_t*)buffer);
    }
}


void WDsDisplay__Colon_toogle(void){
    colon_status = !colon_status;
    AdafruitSSD1306_setTextSize(clock_textSize);
    AdafruitSSD1306_setTextColor(clock_textColor);
    AdafruitSSD1306_setCursor(clock_cursorx + 6*clock_textSize*2, clock_cursory);
    WDsDisplay__clear_18x24(clock_cursorx +6*clock_textSize*2, clock_cursory, 0);
    if(colon_status) AdafruitSSD1306_printc(':');
    else AdafruitSSD1306_printc(' ');
}

void WDsDisplay__Heartrate_status(void){
    WDsDisplay__clear_heart(heart_x, heart_y, 0);
    AdafruitSSD1306_drawBitmap(heart_x, heart_y, arrayCardiogram[heart_status] , sizeBigHeart, sizeBigHeart, 1, 0);
    // switch(heart_status){
    //     case 0: AdafruitSSD1306_drawBitmap(heart_x, heart_y, cardiogram , sizeBigHeart, sizeBigHeart, 1, 0); break;
    //     case 1: AdafruitSSD1306_drawBitmap(heart_x, heart_y, cardiogram1, sizeBigHeart, sizeBigHeart, 1, 0); break;
    //     case 2: AdafruitSSD1306_drawBitmap(heart_x, heart_y, cardiogram2, sizeBigHeart, sizeBigHeart, 1, 0); break;
    //     case 3: AdafruitSSD1306_drawBitmap(heart_x, heart_y, cardiogram3, sizeBigHeart, sizeBigHeart, 1, 0); break;
    //     default: heart_status=0; break;
    // }
    heart_status++;
    heart_status = heart_status%4;
}

void WDsDisplay__Heartrate_number(uint16_t number){ // -1 is wait
    char buffer[4] = "";
    uint16_t temp = number;
    uint8_t padding = 10;
    WDsDisplay__clear_character(heart_x + sizeBigHeart + padding, heart_y + padding, 4, 3, 0);
    AdafruitSSD1306_setTextSize(clock_textSize);
    AdafruitSSD1306_setTextColor(clock_textColor);

    if (number >1000){
        buffer[0] = buffer[1] = buffer[2] = '_';
        AdafruitSSD1306_setCursor(heart_x + sizeBigHeart + padding, heart_y + padding);
        AdafruitSSD1306_print((uint8_t*)buffer);
        return;
    }
    uint8_t stringlen = 0;
    if(number<10){
        buffer[0] = '0' +temp;
        stringlen = 1;
    }else if(number<100){
        buffer[1] = '0' +(temp%10);
        buffer[0] = '0' +(temp/10);
        stringlen = 2;
    }else if(number <1000){
        buffer[2] = '0' +(temp%10);
        temp = temp/10;
        buffer[1] = '0' + temp%10;
        buffer[0] = '0' + temp/10;
        stringlen = 3;
    }

    buffer[stringlen] = 0; 
    switch(stringlen){
    case 1: AdafruitSSD1306_setCursor(heart_x + sizeBigHeart + padding + (3*font_width)*1, heart_y + padding); break;
    case 2: AdafruitSSD1306_setCursor(heart_x + sizeBigHeart + padding + (3*font_width)*1, heart_y + padding); break;
    case 3: AdafruitSSD1306_setCursor(heart_x + sizeBigHeart + padding                 , heart_y + padding); break;
    default: break;
    }
    AdafruitSSD1306_print((uint8_t*)buffer);
    return;
}

void WDsDisplay__Footsteps_status(void){
    WDsDisplay__clear_heart(footsteps_x, footsteps_y, 0);
    if(footsteps_status)
        AdafruitSSD1306_drawBitmap(footsteps_x, footsteps_y, footsteps , sizeFootsteps, sizeFootsteps, 1, 0);
    else
        AdafruitSSD1306_drawBitmap(footsteps_x, footsteps_y, footsteps1, sizeFootsteps, sizeFootsteps, 1, 0);
    footsteps_status = !footsteps_status; // boolean
}


void WDsDisplay__Footsteps_number(uint16_t number){
    char buffer[6] = "";
    uint16_t temp = number;
    uint8_t padding = 10;
    uint8_t padding_y = 20;
    WDsDisplay__clear_character(heart_x + sizeBigHeart + padding, heart_y + padding_y, 4, 3, 0);
    AdafruitSSD1306_setTextSize(2);
    AdafruitSSD1306_setTextColor(clock_textColor);

    // sprintf(buffer, "%d", number);
    uint8_t stringlen = 0;
    if(number<10){
        buffer[0] = '0' +temp;
        stringlen = 1;
    }else if(number<100){
        buffer[1] = '0' +(temp%10);
        buffer[0] = '0' +(temp/10);
        stringlen = 2;
    }else if(number <1000){
        buffer[2] = '0' +(temp%10);
        temp = temp/10;
        buffer[1] = '0' + temp%10;
        buffer[0] = '0' + temp/10;
        stringlen = 3;
    }else if(number <10000){
        buffer[3] = '0' +(temp%10);
        temp = temp/10;
        buffer[2] = '0' + temp%10;
        temp = temp/10;
        buffer[1] = '0' + temp%10;
        buffer[0] = '0' + temp/10;
        stringlen = 4;
    }else if(number <100000){
        buffer[4] = '0' +(temp%10);
        temp = temp/10;
        buffer[3] = '0' + temp%10;
        temp = temp/10;
        buffer[2] = '0' + temp%10;
        temp = temp/10;
        buffer[1] = '0' + temp%10;
        buffer[0] = '0' + temp/10;
        stringlen = 5;
    }

    buffer[stringlen] = 0; // end

    // int8_t stringlen = strlen(buffer);
    switch(stringlen){
        case 1: AdafruitSSD1306_setCursor(heart_x + sizeBigHeart + padding + (2*font_width)*2, heart_y + padding_y); break;
        case 2: AdafruitSSD1306_setCursor(heart_x + sizeBigHeart + padding + (2*font_width)*2, heart_y + padding_y); break;
        case 3: AdafruitSSD1306_setCursor(heart_x + sizeBigHeart + padding + (2*font_width)*1, heart_y + padding_y); break;
        case 4: AdafruitSSD1306_setCursor(heart_x + sizeBigHeart + padding + (2*font_width)*1, heart_y + padding_y); break;
        case 5: AdafruitSSD1306_setCursor(heart_x + sizeBigHeart + padding, heart_y + padding_y); break;
        default: break;
    }
    AdafruitSSD1306_print((uint8_t*)buffer);
}

void WDsDisplay__Display_head(void){
    AdafruitSSD1306_displayPage(0, 1);
}

void WDsDisplay__Display_body(void){
    AdafruitSSD1306_displayPage(2, 7);
}

void WDsDisplay__display(){
    AdafruitSSD1306_display();
}

void WDsDisplay__Clear_head(void){
    uint8_t* pointer = AdafruitSSD1306_getbuffer();
    memset(pointer, 0, SSD1306_LCDWIDTH*2);
}

void WDsDisplay__Clear_body(void){
    uint8_t *pointer = AdafruitSSD1306_getbuffer();
    memset(pointer+SSD1306_LCDWIDTH*2, 0, (SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8) - SSD1306_LCDWIDTH*2);
}

void WDsDisplay__Fillfull(void){
    uint8_t *pointer = AdafruitSSD1306_getbuffer();
    memset(pointer, 0xff, (SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8));   
}

/* Private function */

void WDsDisplay__clear_16x16(int16_t x, int16_t y, uint16_t color){
    uint8_t *pointer = AdafruitSSD1306_getbuffer();
    AdafruitSSD1306_fillRect(x, y, 16, 16, color);
}

void WDsDisplay__clear_18x24(int16_t x, int16_t y, uint16_t color){
    AdafruitSSD1306_fillRect(x, y, 18, 24, color);
}

void WDsDisplay__clear_heart(int16_t x, int16_t y, uint16_t color){
    uint8_t* pointer = AdafruitSSD1306_getbuffer();
    uint8_t i = 0;
    for ( i = 0; i < 6; ++i) // 48/8
    {
        memset(pointer +(y/8+i)*SSD1306_LCDWIDTH +x, 0, 48);
    }

    AdafruitSSD1306_fillRect(x, y, 48, 48, color);
}

void WDsDisplay__clear_character(int16_t x, int16_t y, uint16_t fontsize, uint16_t length, uint16_t color){
    uint8_t fontSize[2] = {6, 8};
    AdafruitSSD1306_fillRect(x, y, fontSize[0]*fontsize*length, fontSize[1]*fontsize, color);
}

void WDsDisplay__clearDisplay(){
    AdafruitSSD1306_clearDisplay();
}

void WDsDisplay__onoff(bool mode){
    AdafruitSSD1306_onoff(mode);
}      
void WDsDisplay__print(uint8_t *str){
    AdafruitSSD1306_print(str);
}

void WDsDisplay__setCursor(int16_t x, int16_t y){
    AdafruitSSD1306_setCursor(x,y);
}
void WDsDisplay__setTextColor(uint16_t c){
    AdafruitSSD1306_setTextColor(c);
}
void WDsDisplay__setTextSize(uint8_t s){
    AdafruitSSD1306_setTextSize(s);
}

uint8_t *WDsDisplay__getBuffer(void){
  return AdafruitSSD1306_getbuffer();
}
