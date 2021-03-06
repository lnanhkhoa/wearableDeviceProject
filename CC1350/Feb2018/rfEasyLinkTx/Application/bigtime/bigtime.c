/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== bigtime.c ========
 */

#include <stdint.h>
#include <stdbool.h>

 #include "bigtime.h"

const char *months[12] = { "January", "February", "March",
                           "April",   "May",      "June",
                           "July",    "August",   "September",
                           "October", "November", "December" };

uint8_t id;
int microsecond;
int millisecond;
uint8_t second;
uint8_t minute;
uint8_t hour;
uint8_t day;
uint8_t month;
uint8_t year;
uint8_t century;
uint8_t millenium;

/*
 * Clock methods
 */
void Clock_init(uint8_t newId)
{
    id = newId;
    microsecond = 0;
    millisecond = 0;
    second = 10;
    minute = 00;
    hour = 14;
    day = 13;
    month = 10;
    year = 17;
    century = 20;
    millenium = 0;

}

void Clock_updateTime(uint8_t *newDateTime){
    // ss, mm, hh, dd, mm, yy,cc
    microsecond = 0;
    millisecond = 0;
    second      = *(newDateTime);
    minute      = *(newDateTime +1);
    hour        = *(newDateTime +2);
    day         = *(newDateTime +3);
    month       = *(newDateTime +4);
    year        = *(newDateTime +5);
    century     = *(newDateTime +6);
    millenium = 0;
}


void Clock_setMicrosecond()
{
    if (microsecond >= 999) {
        Clock_setMillisecond();
        microsecond = 0;
    }
    else {
        microsecond++;
    }

    return;
}

void Clock_setMillisecond()
{
    if (millisecond >= 999) {
        Clock_setSecond();
        millisecond = 0;
    }
    else {
        millisecond++;
    }

    return;
}

void Clock_setnMillisecond(uint8_t nMilliseconds)
{
    uint8_t secs;

    millisecond += nMilliseconds;
    secs = millisecond / 1000;
    millisecond %= 1000;

    while (secs--) {
        Clock_setSecond();
    }

    return;
}

void Clock_setSecond()
{
    if (second == 59) {
        Clock_setMinute();
        second = 0;
    }
    else {
        second++;
    }

    return;
}

void Clock_setMinute()
{
    if (minute == 59) {
        Clock_setHour();
        minute = 0;
    }
    else {
        minute++;
    }

    return;
}

void Clock_setHour()
{
    if (hour == 23) {
        Clock_setDay();
        hour = 0;
    }
    else {
        hour++;
    }

    return;
}

void Clock_setDay()
{
    bool thirtydays = false;
    bool feb = false;
    bool leap = false;

    if (month == 4 || month == 6 || month == 9 || month == 11) {
        // April, June, September, November.
        thirtydays = true;
    }

    if (month == 2) {  // Test for February
        feb = true;
    }

    /*
     * A year is a leap year if it is divisible by 4, but not by 100.
     *
     * If a year is divisible by 4 and by 100, it is a leap year only
     * if it is also divisible by 400.
     */
    if ((year%4 == 0 && year%100 != 0) ||
            (year%4 == 0 && year%100 == 0 && year%400 == 0)) {
        leap = true;
    }

    if ((day == 28) && (feb) && (!leap)) {
        Clock_setMonth();
        day = 1;
    }
    else if ((day == 29) && (feb) && (leap)) {
        Clock_setMonth();
        day = 1;
    }
    else if ((day == 30) && (thirtydays == true)) {
        Clock_setMonth();
        day = 1;
    }
    else if ((day == 31) && (thirtydays == false)) {
        Clock_setMonth();
        day = 1;
    }
    else {
        day++;
    }

    return;
}

void Clock_setMonth()
{
    if (month >= 12) {
        Clock_setYear();
        month = 1;
    }
    else {
        month++;
    }

    return;
}

void Clock_setYear()
{
    year++;
    if ((year%100) == 0) {
        Clock_setCentury();
    }

    return;
}

void Clock_setCentury()
{
    century++;
    if ((century%10) == 0) {
        Clock_setMillenium();
    }

    return;
}

void Clock_setMillenium()
{
    millenium++;

    return;
}

uint8_t Clock_getId()
{
    return id;
}

int Clock_getMicrosecond()
{
    return microsecond;
}

int Clock_getMillisecond()
{
    return millisecond;
}

uint8_t Clock_getSecond()
{
    return second;
}

uint8_t Clock_getMinute()
{
    return minute;
}

uint8_t Clock_getHour()
{
    return hour;
}

uint8_t Clock_getDay()
{
    return day;
}

uint8_t Clock_getMonth()
{
    return month;
}

uint8_t Clock_getYear()
{
    return year;
}

uint8_t Clock_getCentury()
{
    return century;
}

uint8_t Clock_getMillenium()
{
    return millenium;
}
