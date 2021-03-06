/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!

Copyright (c) 2013 Adafruit Industries.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include "glcdfont.h"
#include "Adafruit_GFX.h"

int16_t WIDTH, HEIGHT;   // This is the 'raw' display w/h - never changes
int16_t _width, _height, // Display w/h as modified by current rotation
        cursor_x, cursor_y;
uint16_t textcolor, textbgcolor;
uint8_t textsize, rotation;
boolean wrap,   // If set, 'wrap' text at right edge of display
      _cp437; // If set, use correct CP437 charset (default is off)
GFXfont *gfxFont;

/* Pointer to function*/

void (*P_drawPixel)(int16_t , int16_t , uint16_t );

void (*P_startWrite)(void);
void (*P_writePixel)(int16_t, int16_t , uint16_t );
void (*P_writeFillRect)(int16_t , int16_t , int16_t , int16_t , uint16_t );
void (*P_writeFastVLine)(int16_t , int16_t , int16_t , uint16_t );
void (*P_writeFastHLine)(int16_t , int16_t , int16_t , uint16_t );
void (*P_writeLine)(int16_t , int16_t , int16_t , int16_t , uint16_t );
void (*P_endWrite)(void);


void (*PsetRotation)(uint8_t);


void
    (*P_drawFastVLine)(int16_t , int16_t , int16_t , uint16_t ),
    (*P_drawFastHLine)(int16_t , int16_t , int16_t , uint16_t ),
    (*P_fillRect)(int16_t , int16_t , int16_t , int16_t , uint16_t ),
    (*P_fillScreen)(uint16_t ),
    (*P_drawLine)(int16_t , int16_t , int16_t , int16_t , uint16_t ),
    (*P_drawRect)(int16_t , int16_t , int16_t , int16_t , uint16_t ),

    (*P_setCursor)(int16_t , int16_t ),
    (*P_setTextColor)(uint16_t ),
    (*P_setTextSize)(uint8_t );

void (*P_write)(uint8_t);



// Bresenham's algorithm - thx wikpedia
void AdafruitGFX_writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    int16_t steep = fabs(y1 - y0) > fabs(x1 - x0);
    if (steep) {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }

    if (x0 > x1) {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = fabs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (steep) {
            AdafruitGFX_writePixel(y0, x0, color);
        } else {
            AdafruitGFX_writePixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

void AdafruitGFX_drawPixel(int16_t x, int16_t y, uint16_t color){

}

void AdafruitGFX_startwrite(){
    // OverAdafruitGFX_write in subclasses if desired!
}

void AdafruitGFX_writePixel(int16_t x, int16_t y, uint16_t color){
    // OverAdafruitGFX_write in subclasses if AdafruitGFX_startwrite is defined!
    P_drawPixel(x, y, color);
}

// (x,y) is topmost point; if unsure, calling function
// should sort endpoints or call AdafruitGFX_writeLine() instead
void AdafruitGFX_writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    // OverAdafruitGFX_write in subclasses if AdafruitGFX_startwrite is defined!
    // Can be just AdafruitGFX_writeLine(x, y, x, y+h-1, color);
    // or AdafruitGFX_writeFillRect(x, y, 1, h, color);
    P_drawFastVLine(x, y, h, color);
}

// (x,y) is leftmost point; if unsure, calling function
// should sort endpoints or call AdafruitGFX_writeLine() instead
void AdafruitGFX_writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    // OverAdafruitGFX_write in subclasses if AdafruitGFX_startwrite is defined!
    // Example: AdafruitGFX_writeLine(x, y, x+w-1, y, color);
    // or AdafruitGFX_writeFillRect(x, y, w, 1, color);
    P_drawFastHLine(x, y, w, color);
}

void AdafruitGFX_writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    // OverAdafruitGFX_write in subclasses if desired!
    AdafruitGFX_fillRect(x,y,w,h,color);
}

void AdafruitGFX_endwrite(){
    // OverAdafruitGFX_write in subclasses if AdafruitGFX_startwrite is defined!
}

// (x,y) is topmost point; if unsure, calling function
// should sort endpoints or call AdafruitGFX_drawLine() instead
void AdafruitGFX_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    // Update in subclasses if desired!
    AdafruitGFX_startwrite();
    AdafruitGFX_writeLine(x, y, x, y+h-1, color);
    AdafruitGFX_endwrite();
}

// (x,y) is leftmost point; if unsure, calling function
// should sort endpoints or call AdafruitGFX_drawLine() instead
void AdafruitGFX_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    // Update in subclasses if desired!
    AdafruitGFX_startwrite();
    AdafruitGFX_writeLine(x, y, x+w-1, y, color);
    AdafruitGFX_endwrite();
}

void AdafruitGFX_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    // Update in subclasses if desired!
    AdafruitGFX_startwrite();
    int16_t i;
    for (i=x; i<x+w; i++) {
        AdafruitGFX_writeFastVLine(i, y, h, color);
    }
    AdafruitGFX_endwrite();
}

void AdafruitGFX_fillScreen(uint16_t color) {
    // Update in subclasses if desired!
    AdafruitGFX_fillRect(0, 0, _width, _height, color);
}

void AdafruitGFX_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    // Update in subclasses if desired!
    if(x0 == x1){
        if(y0 > y1) _swap_int16_t(y0, y1);
        P_drawFastVLine(x0, y0, y1 - y0 + 1, color);
    } else if(y0 == y1){
        if(x0 > x1) _swap_int16_t(x0, x1);
        P_drawFastHLine(x0, y0, x1 - x0 + 1, color);
    } else {
        AdafruitGFX_startwrite();
        AdafruitGFX_writeLine(x0, y0, x1, y1, color);
        AdafruitGFX_endwrite();
    }
}

// Draw a circle outline
void AdafruitGFX_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    AdafruitGFX_startwrite();
    AdafruitGFX_writePixel(x0  , y0+r, color);
    AdafruitGFX_writePixel(x0  , y0-r, color);
    AdafruitGFX_writePixel(x0+r, y0  , color);
    AdafruitGFX_writePixel(x0-r, y0  , color);

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        AdafruitGFX_writePixel(x0 + x, y0 + y, color);
        AdafruitGFX_writePixel(x0 - x, y0 + y, color);
        AdafruitGFX_writePixel(x0 + x, y0 - y, color);
        AdafruitGFX_writePixel(x0 - x, y0 - y, color);
        AdafruitGFX_writePixel(x0 + y, y0 + x, color);
        AdafruitGFX_writePixel(x0 - y, y0 + x, color);
        AdafruitGFX_writePixel(x0 + y, y0 - x, color);
        AdafruitGFX_writePixel(x0 - y, y0 - x, color);
    }
    AdafruitGFX_endwrite();
}

void AdafruitGFX_drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) {
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;
        if (cornername & 0x4) {
            AdafruitGFX_writePixel(x0 + x, y0 + y, color);
            AdafruitGFX_writePixel(x0 + y, y0 + x, color);
        }
        if (cornername & 0x2) {
            AdafruitGFX_writePixel(x0 + x, y0 - y, color);
            AdafruitGFX_writePixel(x0 + y, y0 - x, color);
        }
        if (cornername & 0x8) {
            AdafruitGFX_writePixel(x0 - y, y0 + x, color);
            AdafruitGFX_writePixel(x0 - x, y0 + y, color);
        }
        if (cornername & 0x1) {
            AdafruitGFX_writePixel(x0 - y, y0 - x, color);
            AdafruitGFX_writePixel(x0 - x, y0 - y, color);
        }
    }
}

void AdafruitGFX_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    AdafruitGFX_startwrite();
    AdafruitGFX_writeFastVLine(x0, y0-r, 2*r+1, color);
    AdafruitGFX_fillCircleHelper(x0, y0, r, 3, 0, color);
    AdafruitGFX_endwrite();
}

// Used to do circles and roundrects
void AdafruitGFX_fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color) {

    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;

        if (cornername & 0x1) {
            AdafruitGFX_writeFastVLine(x0+x, y0-y, 2*y+1+delta, color);
            AdafruitGFX_writeFastVLine(x0+y, y0-x, 2*x+1+delta, color);
        }
        if (cornername & 0x2) {
            AdafruitGFX_writeFastVLine(x0-x, y0-y, 2*y+1+delta, color);
            AdafruitGFX_writeFastVLine(x0-y, y0-x, 2*x+1+delta, color);
        }
    }
}

// Draw a rectangle
void AdafruitGFX_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    AdafruitGFX_startwrite();
    AdafruitGFX_writeFastHLine(x, y, w, color);
    AdafruitGFX_writeFastHLine(x, y+h-1, w, color);
    AdafruitGFX_writeFastVLine(x, y, h, color);
    AdafruitGFX_writeFastVLine(x+w-1, y, h, color);
    AdafruitGFX_endwrite();
}

// Draw a rounded rectangle
void AdafruitGFX_drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
    // smarter version
    AdafruitGFX_startwrite();
    AdafruitGFX_writeFastHLine(x+r  , y    , w-2*r, color); // Top
    AdafruitGFX_writeFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
    AdafruitGFX_writeFastVLine(x    , y+r  , h-2*r, color); // Left
    AdafruitGFX_writeFastVLine(x+w-1, y+r  , h-2*r, color); // Right
    // draw four corners
    AdafruitGFX_drawCircleHelper(x+r    , y+r    , r, 1, color);
    AdafruitGFX_drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
    AdafruitGFX_drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
    AdafruitGFX_drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
    AdafruitGFX_endwrite();
}

// Fill a rounded rectangle
void AdafruitGFX_fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
    // smarter version
    AdafruitGFX_startwrite();
    AdafruitGFX_writeFillRect(x+r, y, w-2*r, h, color);

    // draw four corners
    AdafruitGFX_fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
    AdafruitGFX_fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
    AdafruitGFX_endwrite();
}

// Draw a triangle
void AdafruitGFX_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    AdafruitGFX_drawLine(x0, y0, x1, y1, color);
    AdafruitGFX_drawLine(x1, y1, x2, y2, color);
    AdafruitGFX_drawLine(x2, y2, x0, y0, color);
}

// Fill a triangle
void AdafruitGFX_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {

    int16_t a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
        _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
    }
    if (y1 > y2) {
        _swap_int16_t(y2, y1); _swap_int16_t(x2, x1);
    }
    if (y0 > y1) {
        _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
    }

    AdafruitGFX_startwrite();
    if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if(x1 < a)      a = x1;
        else if(x1 > b) b = x1;
        if(x2 < a)      a = x2;
        else if(x2 > b) b = x2;
        AdafruitGFX_writeFastHLine(a, y0, b-a+1, color);
        AdafruitGFX_endwrite();
        return;
    }

    int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1;
    int32_t
    sa   = 0,
    sb   = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if(y1 == y2) last = y1;   // Include y1 scanline
    else         last = y1-1; // Skip it

    for(y=y0; y<=last; y++) {
        a   = x0 + sa / dy01;
        b   = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
        a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if(a > b) _swap_int16_t(a,b);
        AdafruitGFX_writeFastHLine(a, y, b-a+1, color);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for(; y<=y2; y++) {
        a   = x1 + sa / dy12;
        b   = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        /* longhand:
        a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if(a > b) _swap_int16_t(a,b);
        AdafruitGFX_writeFastHLine(a, y, b-a+1, color);
    }
    AdafruitGFX_endwrite();
}
/*

// BITMAP / XBITMAP / GRAYSCALE / RGB BITMAP FUNCTIONS ---------------------

// Draw a PROGMEM-resident 1-bit image at the specified (x,y) position,
// using the specified foreground color (unset bits are transparent).
void AdafruitGFX_drawBitmap(int16_t x, int16_t y, const unsigned char bitmap[], int16_t w, int16_t h, uint16_t color){

    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;
    int16_t i,j;
    AdafruitGFX_startwrite();
    for(j=0; j<h; j++, y++) {
        for(i=0; i<w; i++) {
            if(i & 7) byte <<= 1;
            else      byte   = pgm_read_byte(&bitmap[j * byteWidth + i / 8]);
            if(byte & 0x80) AdafruitGFX_writePixel(x+i, y, color);
        }
    }
    AdafruitGFX_endwrite();
}

*/
// Draw a PROGMEM-resident 1-bit image at the specified (x,y) position,
// using the specified foreground (for set bits) and background (unset
// bits) colors.
// void AdafruitGFX_drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color, uint16_t bg) {

//     int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
//     uint8_t byte = 0;
//     int16_t i, j;
//     AdafruitGFX_startwrite();
//     for(j=0; j<h; j++, y++) {
//         for(i=0; i<w; i++ ) {
//             if(i & 7) byte <<= 1;
//             else      byte   = pgm_read_byte(&bitmap[j * byteWidth + i / 8]);
//             AdafruitGFX_writePixel(x+i, y, (byte & 0x80) ? color : bg);
//         }
//     }
//     AdafruitGFX_endwrite();
// }

/*
// Draw a RAM-resident 1-bit image at the specified (x,y) position,
// using the specified foreground color (unset bits are transparent).
void AdafruitGFX_drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {
    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    AdafruitGFX_startwrite();
    for(int16_t j=0; j<h; j++, y++) {
        for(int16_t i=0; i<w; i++ ) {
            if(i & 7) byte <<= 1;
            else      byte   = bitmap[j * byteWidth + i / 8];
            if(byte & 0x80) AdafruitGFX_writePixel(x+i, y, color);
        }
    }
    AdafruitGFX_endwrite();
}
*/

// Draw a RAM-resident 1-bit image at the specified (x,y) position,
// using the specified foreground (for set bits) and background (unset
// bits) colors.
void AdafruitGFX_drawBitmap(int16_t x, int16_t y, const unsigned char *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg) {

    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
    unsigned char byte = 0;
    int16_t i, j;

    AdafruitGFX_startwrite();
    for(j=0; j<h; j++, y++) {
        for(i=0; i<w; i++ ) {
            if(i & 7) byte <<= 1;
            else      byte   = bitmap[j * byteWidth + i / 8];
            AdafruitGFX_writePixel(x+i, y, (byte & 0x80) ? color : bg);
        }
    }
    AdafruitGFX_endwrite();
}
/*
// Draw PROGMEM-resident XBitMap Files (*.xbm), exported from GIMP,
// Usage: Export from GIMP to *.xbm, rename *.xbm to *.c and open in editor.
// C Array can be directly used with this function.
// There is no RAM-resident version of this function; if generating bitmaps
// in RAM, use the format defined by drawBitmap() and call that instead.
void AdafruitGFX_drawXBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color) {

    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    AdafruitGFX_startwrite();
    for(int16_t j=0; j<h; j++, y++) {
        for(int16_t i=0; i<w; i++ ) {
            if(i & 7) byte >>= 1;
            else      byte   = pgm_read_byte(&bitmap[j * byteWidth + i / 8]);
            // Nearly identical to drawBitmap(), only the bit order
            // is reversed here (left-to-right = LSB to MSB):
            if(byte & 0x01) AdafruitGFX_writePixel(x+i, y, color);
        }
    }
    AdafruitGFX_endwrite();
}
*/
// Draw a PROGMEM-resident 8-bit image (grayscale) at the specified (x,y)
// pos.  Specifically for 8-bit display devices such as IS31FL3731;
// no color reduction/expansion is performed.
void AdafruitGFX_drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h) {
    int16_t i, j;
    AdafruitGFX_startwrite();
    for(j=0; j<h; j++, y++) {
        for(i=0; i<w; i++ ) {
            AdafruitGFX_writePixel(x+i, y, (uint8_t)pgm_read_byte(&bitmap[j * w + i]));
        }
    }
    AdafruitGFX_endwrite();
}


/*
// Draw a RAM-resident 8-bit image (grayscale) at the specified (x,y)
// pos.  Specifically for 8-bit display devices such as IS31FL3731;
// no color reduction/expansion is performed.
void AdafruitGFX_drawGrayscaleBitmap(int16_t x, int16_t y,
  uint8_t *bitmap, int16_t w, int16_t h) {
    AdafruitGFX_startwrite();
    for(int16_t j=0; j<h; j++, y++) {
        for(int16_t i=0; i<w; i++ ) {
            AdafruitGFX_writePixel(x+i, y, bitmap[j * w + i]);
        }
    }
    AdafruitGFX_endwrite();
}

// Draw a PROGMEM-resident 8-bit image (grayscale) with a 1-bit mask
// (set bits = opaque, unset bits = clear) at the specified (x,y) position.
// BOTH buffers (grayscale and mask) must be PROGMEM-resident.
// Specifically for 8-bit display devices such as IS31FL3731;
// no color reduction/expansion is performed.
void AdafruitGFX_drawGrayscaleBitmap(int16_t x, int16_t y,
  const uint8_t bitmap[], const uint8_t mask[],
  int16_t w, int16_t h) {
    int16_t bw   = (w + 7) / 8; // Bitmask scanline pad = whole byte
    uint8_t byte = 0;
    AdafruitGFX_startwrite();
    for(int16_t j=0; j<h; j++, y++) {
        for(int16_t i=0; i<w; i++ ) {
            if(i & 7) byte <<= 1;
            else      byte   = pgm_read_byte(&mask[j * bw + i / 8]);
            if(byte & 0x80) {
                AdafruitGFX_writePixel(x+i, y, (uint8_t)pgm_read_byte(&bitmap[j * w + i]));
            }
        }
    }
    AdafruitGFX_endwrite();
}

// Draw a RAM-resident 8-bit image (grayscale) with a 1-bit mask
// (set bits = opaque, unset bits = clear) at the specified (x,y) pos.
// BOTH buffers (grayscale and mask) must be RAM-resident, no mix-and-
// match.  Specifically for 8-bit display devices such as IS31FL3731;
// no color reduction/expansion is performed.
void AdafruitGFX_drawGrayscaleBitmap(int16_t x, int16_t y,
  uint8_t *bitmap, uint8_t *mask, int16_t w, int16_t h) {
    int16_t bw   = (w + 7) / 8; // Bitmask scanline pad = whole byte
    uint8_t byte = 0;
    AdafruitGFX_startwrite();
    for(int16_t j=0; j<h; j++, y++) {
        for(int16_t i=0; i<w; i++ ) {
            if(i & 7) byte <<= 1;
            else      byte   = mask[j * bw + i / 8];
            if(byte & 0x80) {
                AdafruitGFX_writePixel(x+i, y, bitmap[j * w + i]);
            }
        }
    }
    AdafruitGFX_endwrite();
}
*/

// Draw a PROGMEM-resident 16-bit image (RGB 5/6/5) at the specified (x,y)
// position.  For 16-bit display devices; no color reduction performed.
void AdafruitGFX_drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h) {
    int16_t i,j;
    AdafruitGFX_startwrite();
    for(j=0; j<h; j++, y++) {
        for(i=0; i<w; i++ ) {
            AdafruitGFX_writePixel(x+i, y, pgm_read_word(&bitmap[j * w + i]));
        }
    }
    AdafruitGFX_endwrite();
}
/*
// Draw a RAM-resident 16-bit image (RGB 5/6/5) at the specified (x,y)
// position.  For 16-bit display devices; no color reduction performed.
void AdafruitGFX_drawRGBBitmap(int16_t x, int16_t y,
  uint16_t *bitmap, int16_t w, int16_t h) {
    AdafruitGFX_startwrite();
    for(int16_t j=0; j<h; j++, y++) {
        for(int16_t i=0; i<w; i++ ) {
            AdafruitGFX_writePixel(x+i, y, bitmap[j * w + i]);
        }
    }
    AdafruitGFX_endwrite();
}

// Draw a PROGMEM-resident 16-bit image (RGB 5/6/5) with a 1-bit mask
// (set bits = opaque, unset bits = clear) at the specified (x,y) position.
// BOTH buffers (color and mask) must be PROGMEM-resident.
// For 16-bit display devices; no color reduction performed.
void AdafruitGFX_drawRGBBitmap(int16_t x, int16_t y,
  const uint16_t bitmap[], const uint8_t mask[],
  int16_t w, int16_t h) {
    int16_t bw   = (w + 7) / 8; // Bitmask scanline pad = whole byte
    uint8_t byte = 0;
    AdafruitGFX_startwrite();
    for(int16_t j=0; j<h; j++, y++) {
        for(int16_t i=0; i<w; i++ ) {
            if(i & 7) byte <<= 1;
            else      byte   = pgm_read_byte(&mask[j * bw + i / 8]);
            if(byte & 0x80) {
                AdafruitGFX_writePixel(x+i, y, pgm_read_word(&bitmap[j * w + i]));
            }
        }
    }
    AdafruitGFX_endwrite();
}

// Draw a RAM-resident 16-bit image (RGB 5/6/5) with a 1-bit mask
// (set bits = opaque, unset bits = clear) at the specified (x,y) pos.
// BOTH buffers (color and mask) must be RAM-resident, no mix-and-match.
// For 16-bit display devices; no color reduction performed.
void AdafruitGFX_drawRGBBitmap(int16_t x, int16_t y,
  uint16_t *bitmap, uint8_t *mask, int16_t w, int16_t h) {
    int16_t bw   = (w + 7) / 8; // Bitmask scanline pad = whole byte
    uint8_t byte = 0;
    AdafruitGFX_startwrite();
    for(int16_t j=0; j<h; j++, y++) {
        for(int16_t i=0; i<w; i++ ) {
            if(i & 7) byte <<= 1;
            else      byte   = mask[j * bw + i / 8];
            if(byte & 0x80) {
                AdafruitGFX_writePixel(x+i, y, bitmap[j * w + i]);
            }
        }
    }
    AdafruitGFX_endwrite();
}
*/

// TEXT- AND CHARACTER-HANDLING FUNCTIONS ----------------------------------

// Draw a character
void AdafruitGFX_drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) {
    int8_t i,j;
    if(!gfxFont) { // 'Classic' built-in font

        if((x >= _width)            || // Clip right
           (y >= _height)           || // Clip bottom
           ((x + 6 * size - 1) < 0) || // Clip left
           ((y + 8 * size - 1) < 0))   // Clip top
            return;

        if(!_cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior

        AdafruitGFX_startwrite();
        for(i=0; i<5; i++ ) { // Char bitmap = 5 columns
            uint8_t line = pgm_read_byte(&font[c * 5 + i]);
            for(j=0; j<8; j++, line >>= 1) {
                if(line & 1) {
                    if(size == 1)
                        AdafruitGFX_writePixel(x+i, y+j, color);
                    else
                        AdafruitGFX_writeFillRect(x+i*size, y+j*size, size, size, color);
                } else if(bg != color) {
                    if(size == 1)
                        AdafruitGFX_writePixel(x+i, y+j, bg);
                    else
                        AdafruitGFX_writeFillRect(x+i*size, y+j*size, size, size, bg);
                }
            }
        }
        if(bg != color) { // If opaque, draw vertical line for last column
            if(size == 1) AdafruitGFX_writeFastVLine(x+5, y, 8, bg);
            else          AdafruitGFX_writeFillRect(x+5*size, y, size, 8*size, bg);
        }
        AdafruitGFX_endwrite();

    } else { // Custom font

        // Character is assumed previously filtered by AdafruitGFX_write() to eliminate
        // newlines, returns, non-printable characters, etc.  Calling
        // AdafruitGFX_drawChar() directly with 'bad' characters of font may cause mayhem!

        c -= (uint8_t)pgm_read_byte(&gfxFont->first);
        GFXglyph *glyph  = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c]);
        uint8_t  *bitmap = (uint8_t *)pgm_read_pointer(&gfxFont->bitmap);

        uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
        uint8_t  w  = pgm_read_byte(&glyph->width),
                 h  = pgm_read_byte(&glyph->height);
        int8_t   xo = pgm_read_byte(&glyph->xOffset),
                 yo = pgm_read_byte(&glyph->yOffset);
        uint8_t  xx, yy, bits = 0, bit = 0;
        int16_t  xo16 = 0, yo16 = 0;

        if(size > 1) {
            xo16 = xo;
            yo16 = yo;
        }

        /*
         *  NOTE: THERE IS NO 'BACKGROUND' COLOR OPTION ON CUSTOM FONTS.
         *  THIS IS ON PURPOSE AND BY DESIGN.  The background color feature
         *  has typically been used with the 'classic' font to overAdafruitGFX_write old
         *  screen contents with new data.  This ONLY works because the
         *  characters are a uniform size; it's not a sensible thing to do with
         proportionally-spaced fonts with glyphs of varying sizes (and that
         may overlap).  To replace previously-drawn text when using a custom
         font, use the getTextBounds() function to determine the smallest
         rectangle encompassing a string, erase the area with AdafruitGFX_fillRect(),
         then draw new text.  This WILL infortunately 'blink' the text, but
         is unavoidable.  Drawing 'background' pixels will NOT fix this,
         only creates a new set of problems.  Have an idea to work around
         this (a canvas object type for MCUs that can afford the RAM and
         displays supporting setAddrWindow() and pushColors()), but haven't
         implemented this yet.
         */


        AdafruitGFX_startwrite();
        for(yy=0; yy<h; yy++) {
            for(xx=0; xx<w; xx++) {
                if(!(bit++ & 7)) {
                    bits = pgm_read_byte(&bitmap[bo++]);
                }
                if(bits & 0x80) {
                    if(size == 1) {
                        AdafruitGFX_writePixel(x+xo+xx, y+yo+yy, color);
                    } else {
                        AdafruitGFX_writeFillRect(x+(xo16+xx)*size, y+(yo16+yy)*size,
                          size, size, color);
                    }
                }
                bits <<= 1;
            }
        }
        AdafruitGFX_endwrite();

    } // End classic vs custom font
}

void AdafruitGFX_write(uint8_t c) {
    uint8_t first, w, h;

    if(!gfxFont) { // 'Classic' built-in font

        if(c == '\n') {                        // Newline?
            cursor_x  = 0;                     // Reset x to zero,
            cursor_y += textsize * 8;          // advance y one line
        } else if(c != '\r') {                 // Ignore carriage returns
            if(wrap && ((cursor_x + textsize * 6) > _width)) { // Off right?
                cursor_x  = 0;                 // Reset x to zero,
                cursor_y += textsize * 8;      // advance y one line
            }
            AdafruitGFX_drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
            cursor_x += textsize * 6;          // Advance x one char
        }

    } else { // Custom font

        if(c == '\n') {
            cursor_x  = 0;
            cursor_y += (int16_t)textsize * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
        } else if(c != '\r') {
            first = pgm_read_byte(&gfxFont->first);
            if((c >= first) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last))) {
                GFXglyph *glyph = &(((GFXglyph *)pgm_read_pointer( &gfxFont->glyph))[c - first]);
                w     = pgm_read_byte(&glyph->width);
                h     = pgm_read_byte(&glyph->height);
                if((w > 0) && (h > 0)) { // Is there an associated bitmap?
                    int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset); // sic
                    if(wrap && ((cursor_x + textsize * (xo + w)) > _width)) {
                        cursor_x  = 0;
                        cursor_y += (int16_t)textsize * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
                    }
                    AdafruitGFX_drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
                }
                cursor_x += (uint8_t)pgm_read_byte(&glyph->xAdvance) * (int16_t)textsize;
            }
        }

    }
}

void AdafruitGFX_print(uint8_t *str) {
    while (1){
        unsigned char c = pgm_read_byte(str++);
        if (c == 0) break;
        AdafruitGFX_write(c);
    }
}

void AdafruitGFX_printc(char c)
{
  AdafruitGFX_write(c);
}

void AdafruitGFX_setCursor(int16_t x, int16_t y) {
    cursor_x = x;
    cursor_y = y;
}

int16_t AdafruitGFX_getCursorX(void) {
    return cursor_x;
}

int16_t AdafruitGFX_getCursorY(void) {
    return cursor_y;
}

void AdafruitGFX_setTextSize(uint8_t s) {
    textsize = (s > 0) ? s : 1;
}

void AdafruitGFX_setTextColor(uint16_t c) {
    // For 'transparent' background, we'll set the bg
    // to the same as fg instead of using a flag
    textcolor = textbgcolor = c;
}

void AdafruitGFX_setTextWrap(boolean w) {
    wrap = w;
}

uint8_t AdafruitGFX_getRotation(void) {
    return rotation;
}

void AdafruitGFX_setRotation(uint8_t x) {
    rotation = (x & 3);
    switch(rotation) {
        case 0:
        case 2:
            _width  = WIDTH;
            _height = HEIGHT;
            break;
        case 1:
        case 3:
            _width  = HEIGHT;
            _height = WIDTH;
            break;
    }
}

// Enable (or disable) Code Page 437-compatible charset.
// There was an error in glcdfont.c for the longest time -- one character
// (#176, the 'light shade' block) was missing -- this threw off the index
// of every character that followed it.  But a TON of code has been written
// with the erroneous character indices.  By default, the library uses the
// original 'wrong' behavior and old sketches will still work.  Pass 'true'
// to this function to use correct CP437 character values in your code.
void AdafruitGFX_cp437(boolean x) {
    _cp437 = x;
}

void AdafruitGFX_setFont(const GFXfont *f) {
    if(f) {            // Font struct pointer passed in?
        if(!gfxFont) { // And no current font struct?
            // Switching from classic to new font behavior.
            // Move cursor pos down 6 pixels so it's on baseline.
            cursor_y += 6;
        }
    } else if(gfxFont) { // NULL passed.  Current font struct defined?
        // Switching from new to classic font behavior.
        // Move cursor pos up 6 pixels so it's at top-left of char.
        cursor_y -= 6;
    }
    gfxFont = (GFXfont *)f;
}

// Broke this out as it's used by both the PROGMEM- and RAM-resident
// getTextBounds() functions.
void AdafruitGFX_charBounds(char c, int16_t *x, int16_t *y,
  int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy) {

    if(gfxFont) {

        if(c == '\n') { // Newline?
            *x  = 0;    // Reset x to zero, advance y by one line
            *y += textsize * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
        } else if(c != '\r') { // Not a carriage return; is normal char
            uint8_t first = pgm_read_byte(&gfxFont->first),
                    last  = pgm_read_byte(&gfxFont->last);
            if((c >= first) && (c <= last)) { // Char present in this font?
                GFXglyph *glyph = &(((GFXglyph *)pgm_read_pointer(
                  &gfxFont->glyph))[c - first]);
                uint8_t gw = pgm_read_byte(&glyph->width),
                        gh = pgm_read_byte(&glyph->height),
                        xa = pgm_read_byte(&glyph->xAdvance);
                int8_t  xo = pgm_read_byte(&glyph->xOffset),
                        yo = pgm_read_byte(&glyph->yOffset);
                if(wrap && ((*x+(((int16_t)xo+gw)*textsize)) > _width)) {
                    *x  = 0; // Reset x to zero, advance y by one line
                    *y += textsize * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
                }
                int16_t ts = (int16_t)textsize,
                        x1 = *x + xo * ts,
                        y1 = *y + yo * ts,
                        x2 = x1 + gw * ts - 1,
                        y2 = y1 + gh * ts - 1;
                if(x1 < *minx) *minx = x1;
                if(y1 < *miny) *miny = y1;
                if(x2 > *maxx) *maxx = x2;
                if(y2 > *maxy) *maxy = y2;
                *x += xa * ts;
            }
        }

    } else { // Default font

        if(c == '\n') {                     // Newline?
            *x  = 0;                        // Reset x to zero,
            *y += textsize * 8;             // advance y one line
            // min/max x/y unchaged -- that waits for next 'normal' character
        } else if(c != '\r') {  // Normal char; ignore carriage returns
            if(wrap && ((*x + textsize * 6) > _width)) { // Off right?
                *x  = 0;                    // Reset x to zero,
                *y += textsize * 8;         // advance y one line
            }
            int x2 = *x + textsize * 6 - 1, // Lower-right pixel of char
                y2 = *y + textsize * 8 - 1;
            if(x2 > *maxx) *maxx = x2;      // Track max x, y
            if(y2 > *maxy) *maxy = y2;
            if(*x < *minx) *minx = *x;      // Track min x, y
            if(*y < *miny) *miny = *y;
            *x += textsize * 6;             // Advance x one char
        }
    }
}

// Pass string and a cursor position, returns UL corner and W,H.
void AdafruitGFX_getTextBounds(char *str, int16_t x, int16_t y,
        int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {
    uint8_t c; // Current character

    *x1 = x;
    *y1 = y;
    *w  = *h = 0;

    int16_t minx = _width, miny = _height, maxx = -1, maxy = -1;

    while((c = *str++))
        AdafruitGFX_charBounds(c, &x, &y, &minx, &miny, &maxx, &maxy);

    if(maxx >= minx) {
        *x1 = minx;
        *w  = maxx - minx + 1;
    }
    if(maxy >= miny) {
        *y1 = miny;
        *h  = maxy - miny + 1;
    }
}

// Return the size of the display (per current rotation)
int16_t AdafruitGFX_width(void) {
    return _width;
}

int16_t AdafruitGFX_height(void) {
    return _height;
}

//void AdafruitGFX_invertDisplay(boolean i) {
//    // Do nothing, must be subclassed if supported by hardware
//}




void AdafruitGFX_init(int16_t w, int16_t h){
    WIDTH = w;
    HEIGHT = h;
    _width    = WIDTH;
    _height   = HEIGHT;
    rotation  = 0;
    cursor_y  = cursor_x    = 0;
    textsize  = 1;
    textcolor = textbgcolor = 0xFFFF;
    wrap      = true;
    _cp437    = false;
    gfxFont   = NULL;

    P_drawPixel         = AdafruitGFX_drawPixel;

    P_startWrite        = AdafruitGFX_startwrite;
    P_writePixel        = AdafruitGFX_writePixel;
    P_writeFillRect     = AdafruitGFX_writeFillRect;
    P_writeFastVLine    = AdafruitGFX_writeFastVLine;
    P_writeFastHLine    = AdafruitGFX_writeFastHLine;
    P_writeLine         = AdafruitGFX_writeLine;
    P_endWrite          = AdafruitGFX_endwrite;

    PsetRotation        = AdafruitGFX_setRotation;

    P_drawFastVLine     = AdafruitGFX_drawFastVLine;
    P_drawFastHLine     = AdafruitGFX_drawFastHLine;
    P_fillRect          = AdafruitGFX_fillRect;
    P_fillScreen        = AdafruitGFX_fillScreen;
    P_drawLine          = AdafruitGFX_drawLine;
    P_drawRect          = AdafruitGFX_drawRect;

    P_write             = AdafruitGFX_write;

    P_setCursor         = AdafruitGFX_setCursor;
    P_setTextColor      = AdafruitGFX_setTextColor;
    P_setTextSize       = AdafruitGFX_setTextSize;

}
