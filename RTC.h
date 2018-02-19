/*
 * File:   RTC.h
 * Author: Salar Hosseini
 *
 * Created on February 2, 2018, 10:51 PM
 */

#ifndef RTC_H
#define RTC_H

/********************************** Includes **********************************/
#include "configBits.h"
#include "prebuilt/I2C.h"

/*********************************** Macros ***********************************/
#define __bcd_to_num(num) (num & 0x0F) + ((num & 0xF0)>>4)*10

/****************************** Public Interfaces *****************************/
void initRTC (void);
//void RTC_setTime(void);
void getDateTime (unsigned char time [7]);

#endif /* RTC_H */