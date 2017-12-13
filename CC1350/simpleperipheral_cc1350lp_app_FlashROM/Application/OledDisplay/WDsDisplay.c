/*
 * WDsDisplay.cpp
 *
 *  Created on: Sep 5, 2017
 *      Author: AppsTI
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "libDisplay/bluetooth_enabled.h"
#include "libDisplay/battery.h"
#include "libDisplay/heart.h"
#include "libDisplay/stepcount.h"

#include <stdio.h>
#include <string.h>
#include "WDsDisplay.h"

/* BLE icon's location*/

bool colon_status, footsteps_status;
uint8_t heart_status;


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
    unsigned int per= percent/10;
    switch(per)
    {
    case 10: ;
    case 9: AdafruitSSD1306_drawBitmap(BATicon_x, BATicon_y, battery_full, BATicon_width, BATicon_height, 1, 0); break;
    case 8: AdafruitSSD1306_drawBitmap(BATicon_x, BATicon_y, battery_90p, BATicon_width, BATicon_height, 1, 0); break;
    case 7: AdafruitSSD1306_drawBitmap(BATicon_x, BATicon_y, battery_80p, BATicon_width, BATicon_height, 1, 0); break;
    case 6: AdafruitSSD1306_drawBitmap(BATicon_x, BATicon_y, battery_70p, BATicon_width, BATicon_height, 1, 0); break;
    case 5:
        if((unsigned int)(percent%10) >5){
            AdafruitSSD1306_drawBitmap(BATicon_x, BATicon_y, battery_60p, BATicon_width, BATicon_height, 1, 0); break;
        }else{
            AdafruitSSD1306_drawBitmap(BATicon_x, BATicon_y, battery_50p, BATicon_width, BATicon_height, 1, 0); break;
        }

    case 4: AdafruitSSD1306_drawBitmap(BATicon_x, BATicon_y, battery_40p, BATicon_width, BATicon_height, 1, 0); break;
    case 3: AdafruitSSD1306_drawBitmap(BATicon_x, BATicon_y, battery_30p, BATicon_width, BATicon_height, 1, 0); break;
    case 2: AdafruitSSD1306_drawBitmap(BATicon_x, BATicon_y, battery_20p, BATicon_width, BATicon_height, 1, 0); break;
    case 1: AdafruitSSD1306_drawBitmap(BATicon_x, BATicon_y, battery_10p, BATicon_width, BATicon_height, 1, 0); break;
    default:
        if(charge){
            AdafruitSSD1306_drawBitmap(BATicon_x, BATicon_y, battery_low, BATicon_width, BATicon_height, 1, 0); break;
        }else{
            AdafruitSSD1306_drawBitmap(BATicon_x, BATicon_y, battery_warning, BATicon_width, BATicon_height, 1, 0); break;
        }
    }

}


void WDsDisplay__Clock_set(uint8_t hh, uint8_t mm){
//    colon_status = true;
    char buffer[5];
    AdafruitSSD1306_setTextSize(clock_textSize);
    AdafruitSSD1306_setTextColor(clock_textColor);
    AdafruitSSD1306_setCursor(clock_cursorx, clock_cursory);
    if(hh>24 || hh<0 || mm>60 || mm<0) return;

    if(hh<10)   sprintf(buffer, "0%d", hh);
    else        sprintf(buffer, "%d", hh);

    if(colon_status)
         buffer[2] = ':';
    else buffer[2] = ' ';

    if(mm<10)   sprintf(&buffer[3], "0%d", mm);
    else        sprintf(&buffer[3], "%d", mm);

    AdafruitSSD1306_print((uint8_t*)buffer);
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

    switch(heart_status){
    case 0: AdafruitSSD1306_drawBitmap(heart_x, heart_y, cardiogram , sizeBigHeart, sizeBigHeart, 1, 0); break;
    case 1: AdafruitSSD1306_drawBitmap(heart_x, heart_y, cardiogram1, sizeBigHeart, sizeBigHeart, 1, 0); break;
    case 2: AdafruitSSD1306_drawBitmap(heart_x, heart_y, cardiogram2, sizeBigHeart, sizeBigHeart, 1, 0); break;
    case 3: AdafruitSSD1306_drawBitmap(heart_x, heart_y, cardiogram3, sizeBigHeart, sizeBigHeart, 1, 0); break;
    default: heart_status=0; break;
    }
    heart_status++;
    heart_status = heart_status%4;
}


void WDsDisplay__Heartrate_number(int8_t number){ // -1 is wait
    char buffer[3] = "";
    uint8_t padding = 10;
    WDsDisplay__clear_character(heart_x + sizeBigHeart + padding, heart_y + padding, 4, 3, 0);
    AdafruitSSD1306_setTextSize(clock_textSize);
    AdafruitSSD1306_setTextColor(clock_textColor);

    if (number<0){
        buffer[0] = buffer[1] = buffer[2] = '_';
        AdafruitSSD1306_setCursor(heart_x + sizeBigHeart + padding, heart_y + padding);
        AdafruitSSD1306_print((uint8_t*)buffer);
        return;
    }
    sprintf(buffer, "%d", number);
    int8_t stringlen = strlen(buffer);
    switch(stringlen){
    case 1: AdafruitSSD1306_setCursor(heart_x + sizeBigHeart + padding + (3*font_width)*1, heart_y + padding); break;
    case 2: AdafruitSSD1306_setCursor(heart_x + sizeBigHeart + padding + (3*font_width)*1, heart_y + padding); break;
    case 3: AdafruitSSD1306_setCursor(heart_x + sizeBigHeart + padding, heart_y + padding); break;
    default: break;
    }
    AdafruitSSD1306_print((uint8_t*)buffer);
    return;
}

void WDsDisplay__Footsteps_status(void){
    footsteps_status = !footsteps_status;
    WDsDisplay__clear_heart(footsteps_x, footsteps_y, 0);
    if(footsteps_status)
        AdafruitSSD1306_drawBitmap(footsteps_x, footsteps_y, footsteps , sizeFootsteps, sizeFootsteps, 1, 0);
    else
        AdafruitSSD1306_drawBitmap(footsteps_x, footsteps_y, footsteps1, sizeFootsteps, sizeFootsteps, 1, 0);
}


void WDsDisplay__Footsteps_number(uint32_t number){
    char buffer[6] = "";
    sprintf(buffer, "%d", number);
    uint8_t padding = 10;
    uint8_t padding_y = 20;
    WDsDisplay__clear_character(heart_x + sizeBigHeart + padding, heart_y + padding_y, 4, 3, 0);
    AdafruitSSD1306_setTextSize(2);
    AdafruitSSD1306_setTextColor(clock_textColor);

    int8_t stringlen = strlen(buffer);
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


void WDsDisplay__Clear_head(void){
    uint8_t* pointer =  AdafruitSSD1306_getbuffer();
    memset(pointer, 0, SSD1306_LCDWIDTH*2);
}


void WDsDisplay__Clear_body(void){
    uint8_t* pointer =  AdafruitSSD1306_getbuffer();
    memset(pointer+SSD1306_LCDWIDTH*2, 0, (SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8) - SSD1306_LCDWIDTH*2);
}




/* Private class */

void WDsDisplay__clear_16x16(int16_t x, int16_t y, uint16_t color){
    AdafruitSSD1306_fillRect(x, y, 16, 16, color);
}

void WDsDisplay__clear_18x24(int16_t x, int16_t y, uint16_t color){
    AdafruitSSD1306_fillRect(x, y, 18, 24, color);
}

void WDsDisplay__clear_heart(int16_t x, int16_t y, uint16_t color){
    AdafruitSSD1306_fillRect(x, y, 48, 48, color);
}

void WDsDisplay__clear_character(int16_t x, int16_t y, uint16_t fontsize, uint16_t length, uint16_t color){
    uint8_t fontSize[2] = {6, 8};
    AdafruitSSD1306_fillRect(x, y, fontSize[0]*fontsize*length, fontSize[1]*fontsize, color);

}


void WDsDisplay__clearDisplay(){
    AdafruitSSD1306_clearDisplay();
}

void WDsDisplay__display(){
    AdafruitSSD1306_display();
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
