/* 
 * File:   GLCD_PIC.h
 * Author: Tyler Gamvrelis
 *
 * Created on August 13, 2017, 6:00 PM
 */

#ifndef GLCD_PIC_H
#define	GLCD_PIC_H

/********************************** Includes **********************************/
#include <xc.h>
#include "../configBits.h"

/*********************************** Macros ***********************************/
/* Display write commands (one-liners and 2-liners only). */
#define __NOP()     glcdTransfer(INST_NOP, CMD)      // Empty processor cycle
#define __SWRESET(){\
    /* All registers to default state. */   \
    glcdTransfer(INST_SWRESET, CMD); /* Issue command. */   \
    __delay_ms(130); /* Delay specified on pg. 83 of datasheet. */  \
}
#define __SLPIN(){\
    /* Enter sleep mode. */ \
    glcdTransfer(INST_SLPIN, CMD); /* Issue command. */ \
    __delay_ms(130); /* Delay specified on pg. 93 of datasheet to stabilize
                        power circuits. */  \
}
#define __SLPOUT(){\
    /* Exit sleep mode. */ \
    glcdTransfer(INST_SLPOUT, CMD); /* Issue command. */ \
    __delay_ms(130); /* Delay specified on pg. 94 of datasheet to stabilize
                        timing for supply voltages and clock circuits. */  \
}
#define __SETMADCTL(){\
    /* Set the mirror/exchange parameters. */ \
    glcdTransfer(INST_MADCTL, CMD);\
    glcdTransfer(MADCTLbits.reg, MEMWRITE);\
}
#define __PTLON()   glcdTransfer(INST_PTLON, CMD)    // Partial mode on
#define __NORON()   glcdTransfer(INST_NORON, CMD)    // Partial mode off (normal)      
#define __INVOFF()  glcdTransfer(INST_INVOFF, CMD)   // Display inversion off   
#define __INVON()   glcdTransfer(INST_INVON, CMD)    // Display inversion on      
#define __DISPOFF() glcdTransfer(INST_DISPOFF, CMD)  // Turn off display
#define __DISPON()  glcdTransfer(INST_DISPON, CMD)   // Turn on display
#define __RAMWR()   glcdTransfer(INST_RAMWR, CMD)    // Enable display data RAM writing
#define __TEOFF()   glcdTransfer(INST_TEOFF, CMD);   // Tearing effect off
#define __IDMOFF()  glcdTransfer(INST_IDMOFF, CMD)   // Idle mode off
#define __IDMON()   glcdTransfer(INST_IDMON, CMD)    // Idle mode on

/*********************************** Defines **********************************/
/* RD1 is RS, which is DCX (data/command flag) in the datasheet.
 * 
 * RS = 0 --> command to display controller
 * RS = 1 --> data for display data RAM
 */
#define RS_GLCD          LATDbits.LATD1
#define TRIS_RS_GLCD     TRISDbits.TRISD1

/* RD0 is CS, which is CSX (chip enable) in the datasheet. 
 * 
 * CS = 0 --> display selected for control via SPI
 * CS = 1 --> display not selected for control
 */
#define CS_GLCD          LATDbits.LATD0
#define TRIS_CS_GLCD     TRISDbits.TRISD0

/* Arguments for low-level driver, glcdTransfer. */
#define CMD             1
#define MEMWRITE        0

/* Origin locations (relative to the display when mounted on the DevBugger). */
#define ORIGIN_TOP_LEFT        0
#define ORIGIN_TOP_RIGHT       1
#define ORIGIN_BOTTOM_RIGHT    2
#define ORIGIN_BOTTOM_LEFT     3

/********************************** Constants *********************************/
/* Display dimensions addressable in ST7735 controller display data RAM. */
const unsigned char GLCD_ADDRESSABLE_SIZE_HORZ = 128; // 128 pixels
const unsigned char GLCD_ADDRESSABLE_SIZE_VERT = 160; // 160 pixel

/* Display dimensions as seen in the real world. */
const unsigned char GLCD_SIZE_HORZ = 128;    // 0 to 127 --> 128 pixels
const unsigned char GLCD_SIZE_VERT = 128;    // 0 to 127 --> 128 pixels

/* 18-bit color depth information here.
 * 
 * Each color is encoded by 3 bytes. For each byte, the 6 most significant bits
 * contain useful information and the two lest significant bits are don't cares.
 * Color = B2B1B0 = RGB.
 * 
 * Thus, for each color component (red, green and blue or RGB for short), there 
 * are 2^6 possible values. Since there are three different colors, there are
 * thus (2^6)^3 = 2^18 = 262,144 colors.
 * 
 * The RGB color model is additive, that is, an arbitrary color in the RGB color
 * space is formed by superimposing the intensities of the separate components.
 * Thus, black is zero intensity for all components, and white is full intensity
 * for all components.
 */
const unsigned long BLACK = 0x000000;
const unsigned long GREY = 0x808080;
const unsigned long WHITE = 0xFFFFFF;
const unsigned long RED = 0xFF0000;
const unsigned long ORANGE = 0xFF8C00;
const unsigned long YELLOW = 0xFFFF00;
const unsigned long GREEN = 0x00FF00;
const unsigned long BLUE = 0x0000FF;
const unsigned long INDIGO = 0x4B0082;
const unsigned long VIOLET = 0x9400D3;

/************************************ Types ***********************************/
/* Define union for MADCTL for easy bit addressing. This will be useful for 
 * determining if rotations are in effect. A union is basically used to provide
 * several different ways of using the same space in memory. In this case, we 
 * are using the same space in memory to define a struct with individual bits we
 * can address, and an unsigned char (byte) that encompasses these bits. This
 * way, we can easily set and test the individual bits while still being able to
 * pass the entire byte into a function without sacrificing efficiency.
 * 
 *  Bits:       MY, mirror y-axis (0 --> top to bottom)
 *              MX, mirror x-axis (0 --> left to right)
 *              MV, row/column exchange (0 --> normal)
 *              ML, scan direction parameter (0 --> refresh top to bottom)
 *              RGB, red, green, and blue pixel position change (0 --> RGB)
 *              MH, horizontal refresh order (0 --> refresh left to right)
 * Unsigned char (byte):
 *              reg, encompasses all the bytes in the struct
 */
typedef union{
    struct{
        unsigned        :1; // LSb
        unsigned        :1;
        unsigned MH     :1;
        unsigned RGB    :1;
        unsigned ML     :1;
        unsigned MV     :1;
        unsigned MX     :1;
        unsigned MY     :1; // MSb 
    };
    unsigned char reg; // Alternate way of accessing the above 8 bits, as a byte
}MADCTLbits_t;

/**************************** External variables ******************************/
extern MADCTLbits_t MADCTLbits;

/****************************** Public Interfaces *****************************/
/* Fundamental communication interface. */
void glcdTransfer(unsigned char byte, unsigned char cmd);

/* Fundamental drawing interfaces. */
void glcdDrawRectangle(unsigned char XS, unsigned char XE,\
                        unsigned char YS, unsigned char YE,\
                        unsigned long color);
void glcdDrawPixel(unsigned char XS, unsigned char YS, unsigned long color);

/* The following two functions may be used by students who:
 *  1. Want to maximize write efficiency to the GLCD by minimizing color depth
 *  2. Want to change the location of the origin (i.e. rotate the display). */
void glcdSetCOLMOD(unsigned char numBitsPerPixel);
void glcdSetOrigin(unsigned char corner);

/* Initialization sequence. */
void initGLCD(void);

void draw();

#endif	/* GLCD_PIC_H */
