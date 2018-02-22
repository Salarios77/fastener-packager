/*
 * File:   componentTests.h
 * Author: Salar Hosseini
 *
 * Created on February 19, 2018, 4:34 PM
 */

#ifndef COMPONENT_TESTS_H
#define COMPONENT_TESTS_H

/********************************** Includes **********************************/
#include "configBits.h"
#include "prebuilt/lcd.h"

/****************************** Public Interfaces *****************************/
void solenoidInterruptTest (void);
void microswitchCountTest (void);
unsigned short readADC(char channel);
void ldrTest ();
void dcMotorTest();

#endif /* COMPONENT_TESTS_H */
