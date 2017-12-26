/*********************************************************************
This is a library for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

These displays use SPI to communicate, 4 or 5 pins are required to
interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen below must be included in any redistribution
*********************************************************************/

//#ifdef __AVR__
//  #include <avr/pgmspace.h>
//#elif defined(ESP8266) || defined(ESP32)
// #include <pgmspace.h>
//#else
 #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
//#endif

//#if !defined(__ARM_ARCH) && !defined(ENERGIA) && !defined(ESP8266) && !defined(ESP32) && !defined(__arc__)
// #include <util/sleep.h>
//#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Adafruit_GFX.h"
#include "../devices/i2ctransfer/i2ctransfer.h"
#include "Adafruit_SSD1306.h"


extern int16_t WIDTH, HEIGHT;   // This is the 'raw' display w/h - never changes
extern uint8_t rotation;

// the memory buffer for the LCD

static uint8_t buffer[SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#define ssd1306_swap(a, b) { int16_t t = a; a = b; b = t; }

int8_t _i2caddr, _vccstate, sid, sclk, dc, rst, cs;
boolean hwSPI;

#ifdef HAVE_PORTREG
    PortReg *mosiport, *clkport, *csport, *dcport;
    PortMask mosipinmask, clkpinmask, cspinmask, dcpinmask;
#endif



extern void (*P_drawPixel)(int16_t , int16_t , uint16_t );

extern void 
    (*P_startWrite)(void),
    (*P_writePixel)(int16_t, int16_t , uint16_t ),
    (*P_writeFillRect)(int16_t , int16_t , int16_t , int16_t , uint16_t ),
    (*P_writeFastVLine)(int16_t , int16_t , int16_t , uint16_t ),
    (*P_writeFastHLine)(int16_t , int16_t , int16_t , uint16_t ),
    (*P_writeLine)(int16_t , int16_t , int16_t , int16_t , uint16_t ),
    (*P_endWrite)(void);

extern void (*PsetRotation)(uint8_t);

extern void
    (*P_drawFastVLine)(int16_t , int16_t , int16_t , uint16_t ),
    (*P_drawFastHLine)(int16_t , int16_t , int16_t , uint16_t ),
    (*P_fillRect)(int16_t , int16_t , int16_t , int16_t , uint16_t ),
    (*P_fillScreen)(uint16_t ),
    (*P_drawLine)(int16_t , int16_t , int16_t , int16_t , uint16_t ),
    (*P_drawRect)(int16_t , int16_t , int16_t , int16_t , uint16_t );

extern void (*P_write)(uint8_t);

extern void (*P_setCursor)(int16_t , int16_t ),
            (*P_setTextColor)(uint16_t ),
            (*P_setTextSize)(uint8_t );







// the most basic function, set a single pixel
void AdafruitSSD1306_drawPixel(int16_t x, int16_t y, uint16_t color) {
  
  if ((x < 0) || (x >= AdafruitSSD1306_width()) || (y < 0) || (y >= AdafruitSSD1306_height()))
    return;

  // check rotation, move pixel around if necessary
  switch (AdafruitGFX_getRotation()) {
  case 1:
    ssd1306_swap(x, y);
    x = WIDTH - x - 1;
    break;
  case 2:
    x = WIDTH - x - 1;
    y = HEIGHT - y - 1;
    break;
  case 3:
    ssd1306_swap(x, y);
    y = HEIGHT - y - 1;
    break;
  }

  // x is which column
    switch (color)
    {
      case WHITE:   buffer[x+ (y/8)*SSD1306_LCDWIDTH] |=  (1 << (y&7)); break;
      case BLACK:   buffer[x+ (y/8)*SSD1306_LCDWIDTH] &= ~(1 << (y&7)); break;
      case INVERSE: buffer[x+ (y/8)*SSD1306_LCDWIDTH] ^=  (1 << (y&7)); break;
    }

}

// void AdafruitSSD1306_init(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS){
//   AdafruitGFX_init(SSD1306_LCDWIDTH, SSD1306_LCDHEIGHT);
//   cs = CS;
//   rst = RST;
//   dc = DC;
//   sclk = SCLK;
//   sid = SID;
//   hwSPI = false;
// }

// // constructor for hardware SPI - we indicate DataCommand, ChipSelect, Reset
// void AdafruitSSD1306_init(int8_t DC, int8_t RST, int8_t CS){
//   AdafruitGFX_init(SSD1306_LCDWIDTH, SSD1306_LCDHEIGHT);
//   dc = DC;
//   rst = RST;
//   cs = CS;
//   hwSPI = true;
// }

// initializer for I2C - we only indicate the reset pin!
void AdafruitSSD1306_init(int8_t reset){
  AdafruitGFX_init(SSD1306_LCDWIDTH, SSD1306_LCDHEIGHT);
  sclk = dc = cs = sid = -1;
  rst = reset;

  P_drawPixel = AdafruitSSD1306_drawPixel;
  P_drawFastHLine = AdafruitSSD1306_drawFastHLine;
  P_drawFastVLine = AdafruitSSD1306_drawFastVLine;

}


void AdafruitSSD1306_begin(int8_t vccstate, int8_t i2caddr, bool reset) {

  _vccstate = vccstate;
  _i2caddr = i2caddr;

  // set pin directions
  //  if (sid != -1){
  //    pinMode(dc, OUTPUT);
  //    pinMode(cs, OUTPUT);
  // #ifdef HAVE_PORTREG
  //    csport      = portOutputRegister(digitalPinToPort(cs));
  //    cspinmask   = digitalPinToBitMask(cs);
  //    dcport      = portOutputRegister(digitalPinToPort(dc));
  //    dcpinmask   = digitalPinToBitMask(dc);
  // #endif
  //    if (!hwSPI){
  //      // set pins for software-SPI
  //      pinMode(sid, OUTPUT);
  //      pinMode(sclk, OUTPUT);
  // #ifdef HAVE_PORTREG
  //      clkport     = portOutputRegister(digitalPinToPort(sclk));
  //      clkpinmask  = digitalPinToBitMask(sclk);
  //      mosiport    = portOutputRegister(digitalPinToPort(sid));
  //      mosipinmask = digitalPinToBitMask(sid);
  // #endif
  //  }


  // if (hwSPI){
  //  SPI.begin();
  // #ifdef SPI_HAS_TRANSACTION
  //      SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  // #else
  //      SPI.setClockDivider (4);
  // #endif
  //    }
  //  }
  //  else
  //  {
  //  }
  if ((reset) && (rst >= 0)) {
    // Setup reset pin direction (used by both SPI and I2C)
  //    pinMode(rst, OUTPUT);
  //    digitalWrite(rst, HIGH);
  //    // VDD (3.3V) goes high at start, lets just chill for a ms
  //    sleep(1);
  //    // bring reset low
  //    digitalWrite(rst, LOW);
  //    // wait 10ms
  //    sleep(10);
  //    // bring out of reset
  //    digitalWrite(rst, HIGH);
    // turn on VCC (9V?)
  }

  // Init sequence
  ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
  ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
  ssd1306_command(0x80);                                  // the suggested ratio 0x80

  ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
  ssd1306_command(SSD1306_LCDHEIGHT - 1);

  ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
  ssd1306_command(0x0);                                   // no offset
  ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
  ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
  if (_vccstate == SSD1306_EXTERNALVCC)
    { ssd1306_command(0x10); }
  else
    { ssd1306_command(0x14); }
  ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
  ssd1306_command(0x00);                                  // 0x0 act like ks0108
  ssd1306_command(SSD1306_SEGREMAP | 0x1);
  ssd1306_command(SSD1306_COMSCANDEC);

  #if defined SSD1306_128_32
  ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
  ssd1306_command(0x02);
  ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
  ssd1306_command(0x8F);

  #elif defined SSD1306_128_64
  ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
  ssd1306_command(0x12);
  ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
  if (_vccstate == SSD1306_EXTERNALVCC)
    { ssd1306_command(0x9F); }
  else
    { ssd1306_command(0xCF); }

  #elif defined SSD1306_96_16
  ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
  ssd1306_command(0x2);   //ada x12
  ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
  if (vccstate == SSD1306_EXTERNALVCC)
    { ssd1306_command(0x10); }
  else
    { ssd1306_command(0xAF); }

  #endif

  ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
  if (_vccstate == SSD1306_EXTERNALVCC)
    { ssd1306_command(0x22); }
  else
    { ssd1306_command(0xF1); }
  ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
  ssd1306_command(0x40);
  ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
  ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6

  ssd1306_command(SSD1306_DEACTIVATE_SCROLL);

  ssd1306_command(SSD1306_DISPLAYON);//--turn on oled panel
}


void AdafruitSSD1306_invertDisplay(uint8_t i) {
  if (i) {
    ssd1306_command(SSD1306_INVERTDISPLAY);
  } else {
    ssd1306_command(SSD1306_NORMALDISPLAY);
  }
}

void ssd1306_command(uint8_t c) {
  if (sid != -1)
  {
    // SPI
  #ifdef HAVE_PORTREG
      *csport |= cspinmask;
      *dcport &= ~dcpinmask;
      *csport &= ~cspinmask;
  #else
  //    digitalWrite(cs, HIGH);
  //    digitalWrite(dc, LOW);
  //    digitalWrite(cs, LOW);
  #endif
    AdafruitSSD1306_fastSPIwrite(c);
  #ifdef HAVE_PORTREG
      *csport |= cspinmask;
  #else
  //    digitalWrite(cs, HIGH);
  #endif
  }
  else
  {
    // I2C
    int8_t reg = 0x00;   // Co = 0, D/C = 0

    I2c_WriteByte(_i2caddr, reg, c);
  }
}

// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void AdafruitSSD1306_startscrollright(uint8_t start, uint8_t stop){
  ssd1306_command(SSD1306_RIGHT_HORIZONTAL_SCROLL);
  ssd1306_command(0X00);
  ssd1306_command(start);
  ssd1306_command(0X00);
  ssd1306_command(stop);
  ssd1306_command(0X00);
  ssd1306_command(0XFF);
  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

// startscrollleft
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void AdafruitSSD1306_startscrollleft(uint8_t start, uint8_t stop){
  ssd1306_command(SSD1306_LEFT_HORIZONTAL_SCROLL);
  ssd1306_command(0X00);
  ssd1306_command(start);
  ssd1306_command(0X00);
  ssd1306_command(stop);
  ssd1306_command(0X00);
  ssd1306_command(0XFF);
  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

// startscrolldiagright
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void AdafruitSSD1306_startscrolldiagright(uint8_t start, uint8_t stop){
  ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
  ssd1306_command(0X00);
  ssd1306_command(SSD1306_LCDHEIGHT);
  ssd1306_command(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
  ssd1306_command(0X00);
  ssd1306_command(start);
  ssd1306_command(0X00);
  ssd1306_command(stop);
  ssd1306_command(0X01);
  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

// startscrolldiagleft
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void AdafruitSSD1306_startscrolldiagleft(uint8_t start, uint8_t stop){
  ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
  ssd1306_command(0X00);
  ssd1306_command(SSD1306_LCDHEIGHT);
  ssd1306_command(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
  ssd1306_command(0X00);
  ssd1306_command(start);
  ssd1306_command(0X00);
  ssd1306_command(stop);
  ssd1306_command(0X01);
  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

void AdafruitSSD1306_stopscroll(void){
  ssd1306_command(SSD1306_DEACTIVATE_SCROLL);
}

// Dim the display
// dim = true: display is dimmed
// dim = false: display is normal
void AdafruitSSD1306_dim(boolean dim) {
  uint8_t contrast;
  if (dim) {
    contrast = 0; // Dimmed display
  } else {
    if (_vccstate == SSD1306_EXTERNALVCC) {
      contrast = 0x9F;
    } else {
      contrast = 0xCF;
    }
  }
  // the range of contrast to too small to be really useful
  // it is useful to dim the display
  ssd1306_command(SSD1306_SETCONTRAST);
  ssd1306_command(contrast);
}

void AdafruitSSD1306_onoff(bool mode){
    if(mode)
        ssd1306_command(SSD1306_DISPLAYON);
    else
        ssd1306_command(SSD1306_DISPLAYOFF);
}


void AdafruitSSD1306_display(void) {
  uint32_t i;
  ssd1306_command(SSD1306_COLUMNADDR);
  ssd1306_command(0);   // Column start address (0 = reset)
  ssd1306_command(SSD1306_LCDWIDTH-1); // Column end address (127 = reset)

  ssd1306_command(SSD1306_PAGEADDR);
  ssd1306_command(0); // Page start address (0 = reset)
  #if SSD1306_LCDHEIGHT == 64
    ssd1306_command(7); // Page end address
  #endif
  #if SSD1306_LCDHEIGHT == 32
    ssd1306_command(3); // Page end address
  #endif
  #if SSD1306_LCDHEIGHT == 16
    ssd1306_command(1); // Page end address
  #endif

  // if (sid != -1)
  // {
  //   // SPI
  // #ifdef HAVE_PORTREG
  //     *csport |= cspinmask;
  //     *dcport |= dcpinmask;
  //     *csport &= ~cspinmask;
  // #else
  //  // digitalWrite(cs, HIGH);
  //  // digitalWrite(dc, HIGH);
  //  // digitalWrite(cs, LOW);
  //   ;
  // #endif
  //   for (i=0; i<(SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8); i++) {
  //     AdafruitSSD1306_fastSPIwrite(buffer[i]);
  //   }
  // #ifdef HAVE_PORTREG
  //     *csport |= cspinmask;
  // #else
  // //    digitalWrite(cs, HIGH);
  // #endif
  // }
  // else
  // {
    // save I2C bitrate
  #ifdef TWBR
      uint8_t twbrbackup = TWBR;
      TWBR = 12; // upgrade to 400KHz!
  #endif

    // I2C
     for (i=0; i<(SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8); i++) {
      // send a bunch of data in one xmission

      I2c_WriteRegister(_i2caddr, 0x40, &buffer[i], 16);
     // I2c_WriteRegister(_i2caddr, 0x40, &buffer[0], (SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8));
      i+=15;
     }
  #ifdef TWBR
      TWBR = twbrbackup;
  #endif
  // }
}


void AdafruitSSD1306_displayPage(uint8_t startPage, uint8_t endPage) {
  uint32_t i;
  ssd1306_command(SSD1306_COLUMNADDR);
  ssd1306_command(0);   // Column start address (0 = reset)
  ssd1306_command(SSD1306_LCDWIDTH-1); // Column end address (127 = reset)
  ssd1306_command(SSD1306_PAGEADDR);
  ssd1306_command(startPage); // Page start address (0 = reset)

 #if SSD1306_LCDHEIGHT == 64
   if(endPage>7) endPage = 7;
   ssd1306_command(7); // Page end address
 #endif
 #if SSD1306_LCDHEIGHT == 32
   if(endPage>3) endPage = 3;
   ssd1306_command(3); // Page end address
 #endif
 #if SSD1306_LCDHEIGHT == 16
   if(endPage>1) endPage = 1;
   ssd1306_command(1); // Page end address
 #endif

  // if (sid != -1)
  // {
  //   // SPI
  // #ifdef HAVE_PORTREG
  //     *csport |= cspinmask;
  //     *dcport |= dcpinmask;
  //     *csport &= ~cspinmask;
  // #else
  //  // digitalWrite(cs, HIGH);
  //  // digitalWrite(dc, HIGH);
  //  // digitalWrite(cs, LOW);
  //     ;
  // #endif
  //   // for (i=startPage*127; i<(endPage+1)*127; i++) {
  //   //   AdafruitSSD1306_fastSPIwrite(buffer[i]);
  //   // }
  // #ifdef HAVE_PORTREG
  //     *csport |= cspinmask;
  // #else
  // //    digitalWrite(cs, HIGH);
  // #endif
  // }
  // else
  // {
    // save I2C bitrate
  #ifdef TWBR
      uint8_t twbrbackup = TWBR;
      TWBR = 12; // upgrade to 400KHz!
  #endif
    // I2C
    for (i=startPage*127; i<(endPage+1)*127; i++) {
      I2c_WriteRegister(_i2caddr, 0x40, &buffer[i], 16);
      i+=15;
    }
  #ifdef TWBR
      TWBR = twbrbackup;
  #endif
  // }
}


// clear everything in buffer
void AdafruitSSD1306_clearDisplay(void) {
  memset(buffer, 0, (SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8));
}


void AdafruitSSD1306_fastSPIwrite(uint8_t d) {

  //  if(hwSPI) {
  //    (void)SPI.transfer(d);
  //  } else {
  //    for(uint8_t bit = 0x80; bit; bit >>= 1) {
  //#ifdef HAVE_PORTREG
  //      *clkport &= ~clkpinmask;
  //      if(d & bit) *mosiport |=  mosipinmask;
  //      else        *mosiport &= ~mosipinmask;
  //      *clkport |=  clkpinmask;
  //#else
  //      digitalWrite(sclk, LOW);
  //      if(d & bit) digitalWrite(sid, HIGH);
  //      else        digitalWrite(sid, LOW);
  //      digitalWrite(sclk, HIGH);
  //#endif
  //    }
  //  }
}

void AdafruitSSD1306_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  boolean bSwap = false;
  switch(rotation) {
    case 0:
      // 0 degree rotation, do nothing
      break;
    case 1:
      // 90 degree rotation, swap x & y for rotation, then invert x
      bSwap = true;
      ssd1306_swap(x, y);
      x = WIDTH - x - 1;
      break;
    case 2:
      // 180 degree rotation, invert x and y - then shift y around for AdafruitSSD1306_height.
      x = WIDTH - x - 1;
      y = HEIGHT - y - 1;
      x -= (w-1);
      break;
    case 3:
      // 270 degree rotation, swap x & y for rotation, then invert y  and adjust y for w (not to become h)
      bSwap = true;
      ssd1306_swap(x, y);
      y = HEIGHT - y - 1;
      y -= (w-1);
      break;
  }

  if(bSwap) {
    AdafruitSSD1306_drawFastVLineInternal(x, y, w, color);
  } else {
    AdafruitSSD1306_drawFastHLineInternal(x, y, w, color);
  }
}

void AdafruitSSD1306_drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color) {
  // Do bounds/limit checks
  if(y < 0 || y >= HEIGHT) { return; }

  // make sure we don't try to draw below 0
  if(x < 0) {
    w += x;
    x = 0;
  }

  // make sure we don't go off the edge of the display
  if( (x + w) > WIDTH) {
    w = (WIDTH - x);
  }

  // if our AdafruitSSD1306_width is now negative, punt
  if(w <= 0) { return; }

  // set up the pointer for  movement through the buffer
  register uint8_t *pBuf = buffer;
  // adjust the buffer pointer for the current row
  pBuf += ((y/8) * SSD1306_LCDWIDTH);
  // and offset x columns in
  pBuf += x;

  register uint8_t mask = 1 << (y&7);

  switch (color)
  {
  case WHITE:         while(w--) { *pBuf++ |= mask; }; break;
    case BLACK: mask = ~mask;   while(w--) { *pBuf++ &= mask; }; break;
  case INVERSE:         while(w--) { *pBuf++ ^= mask; }; break;
  }
}


void AdafruitSSD1306_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  bool bSwap = false;
  switch(rotation) {
    case 0:
      break;
    case 1:
      // 90 degree rotation, swap x & y for rotation, then invert x and adjust x for h (now to become w)
      bSwap = true;
      ssd1306_swap(x, y);
      x = WIDTH - x - 1;
      x -= (h-1);
      break;
    case 2:
      // 180 degree rotation, invert x and y - then shift y around for AdafruitSSD1306_height.
      x = WIDTH - x - 1;
      y = HEIGHT - y - 1;
      y -= (h-1);
      break;
    case 3:
      // 270 degree rotation, swap x & y for rotation, then invert y
      bSwap = true;
      ssd1306_swap(x, y);
      y = HEIGHT - y - 1;
      break;
  }

  if(bSwap) {
    AdafruitSSD1306_drawFastHLineInternal(x, y, h, color);
  } else {
    AdafruitSSD1306_drawFastVLineInternal(x, y, h, color);
  }
}


void AdafruitSSD1306_drawFastVLineInternal(int16_t x, int16_t __y, int16_t __h, uint16_t color) {

  // do nothing if we're off the left or right side of the screen
  if(x < 0 || x >= WIDTH) { return; }

  // make sure we don't try to draw below 0
  if(__y < 0) {
    // __y is negative, this will subtract enough from __h to account for __y being 0
    __h += __y;
    __y = 0;

  }

  // make sure we don't go past the AdafruitSSD1306_height of the display
  if( (__y + __h) > HEIGHT) {
    __h = (HEIGHT - __y);
  }

  // if our AdafruitSSD1306_height is now negative, punt
  if(__h <= 0) {
    return;
  }

  // this display doesn't need ints for coordinates, use local byte registers for faster juggling
  register uint8_t y = __y;
  register uint8_t h = __h;


  // set up the pointer for fast movement through the buffer
  register uint8_t *pBuf = buffer;
  // adjust the buffer pointer for the current row
  pBuf += ((y/8) * SSD1306_LCDWIDTH);
  // and offset x columns in
  pBuf += x;

  // do the first partial byte, if necessary - this requires some masking
  register uint8_t mod = (y&7);
  if(mod) {
    // mask off the high n bits we want to set
    mod = 8-mod;

    // note - lookup table results in a nearly 10% performance improvement in fill* functions
    // register uint8_t mask = ~(0xFF >> (mod));
    static uint8_t premask[8] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
    register uint8_t mask = premask[mod];

    // adjust the mask if we're not going to reach the end of this byte
    if( h < mod) {
      mask &= (0XFF >> (mod-h));
    }

  switch (color)
    {
    case WHITE:   *pBuf |=  mask;  break;
    case BLACK:   *pBuf &= ~mask;  break;
    case INVERSE: *pBuf ^=  mask;  break;
    }

    // fast exit if we're done here!
    if(h<mod) { return; }

    h -= mod;

    pBuf += SSD1306_LCDWIDTH;
  }


  // write solid bytes while we can - effectively doing 8 rows at a time
  if(h >= 8) {
    if (color == INVERSE)  {          // separate copy of the code so we don't impact performance of the black/white write version with an extra comparison per loop
      do  {
      *pBuf=~(*pBuf);

        // adjust the buffer forward 8 rows worth of data
        pBuf += SSD1306_LCDWIDTH;

        // adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
        h -= 8;
      } while(h >= 8);
      }
    else {
      // store a local value to work with
      register uint8_t val = (color == WHITE) ? 255 : 0;

      do  {
        // write our value in
      *pBuf = val;

        // adjust the buffer forward 8 rows worth of data
        pBuf += SSD1306_LCDWIDTH;

        // adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
        h -= 8;
      } while(h >= 8);
      }
    }

  // now do the final partial byte, if necessary
  if(h) {
    mod = h & 7;
    // this time we want to mask the low bits of the byte, vs the high bits we did above
    // register uint8_t mask = (1 << mod) - 1;
    // note - lookup table results in a nearly 10% performance improvement in fill* functions
    static uint8_t postmask[8] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };
    register uint8_t mask = postmask[mod];
    switch (color)
    {
      case WHITE:   *pBuf |=  mask;  break;
      case BLACK:   *pBuf &= ~mask;  break;
      case INVERSE: *pBuf ^=  mask;  break;
    }
  }
}


uint8_t *AdafruitSSD1306_getbuffer(void){
  return buffer;
}





void AdafruitSSD1306_drawBitmap(int16_t x, int16_t y, const unsigned char *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg){
  AdafruitGFX_drawBitmap(x, y, bitmap, w, h, color, bg);
}

void AdafruitSSD1306_setCursor(int16_t x, int16_t y){
  P_setCursor(x, y);
}

void AdafruitSSD1306_setTextColor(uint16_t c){
  P_setTextColor(c);
}

void AdafruitSSD1306_setTextSize(uint8_t s){
  P_setTextSize(s);
}

void AdafruitSSD1306_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color){
  P_fillRect(x, y, w, h, color);
} 

void AdafruitSSD1306_print(uint8_t *str){
  AdafruitGFX_print(str);
}

void AdafruitSSD1306_printc(char c){
  AdafruitGFX_printc(c);
}

int16_t AdafruitSSD1306_height(void){
  return AdafruitGFX_height();
}

int16_t AdafruitSSD1306_width(void){
  return AdafruitGFX_width();
}
