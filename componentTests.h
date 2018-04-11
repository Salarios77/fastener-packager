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
#include "RTC.h"
#include "logMemory.h"

/****************************** Public Interfaces *****************************/
void solenoidInterruptTest (void);
void microswitchCountTest (void);
void eepromTest(void);
unsigned short readADC(char channel);
void ldrTest (void);
void dcMotorTest(void);
void rotateTest(void);
void rotateTest2(void);
void rotateTest3(void);
void week8Test (void);
void calibrateFlapStart();
void flapUpLittle();
void flapDownLittle();

#endif /* COMPONENT_TESTS_H */
