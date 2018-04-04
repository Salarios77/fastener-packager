/*
 * File:   main.c
 * Author: Salar Hosseini
 *
 * Created on February 1, 2018, 7:33 PM
 */

#ifndef STANDBY_INTERFACE_H
#define STANDBY_INTERFACE_H

/********************************** Includes **********************************/
#include "configBits.h"
#include "prebuilt/lcd.h"
#include "RTC.h"
#include "logMemory.h"
#include <string.h>

/****************************** Public Interfaces *****************************/
unsigned short int calcOperationTime (unsigned char * timeStart, unsigned char * timeEnd);
void initStandby(unsigned char * quantityInputs, unsigned char setInputs [8][4]);
char getKeyPressed (boolean shouldDispTime);
void showLogs(void);
void errScreen(void);
boolean checkValid (unsigned short int currSelection, char *);
void getInputs (unsigned char * quantityInputs, unsigned char setInputs [8][4]);
void dispRealTime (void);
void doneScreen(void);
void showResults(unsigned char * quantityInputs, unsigned char setInputs [8][4], unsigned short int numRemaining [4], unsigned short int operationTime);

#endif /* STANDBY_INTERFACE_H */