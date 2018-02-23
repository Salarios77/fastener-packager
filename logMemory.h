/*
 * File:   logMemory.h
 * Author: Salar Hosseini
 *
 * Created on February 3, 2018, 1:13 PM
 */

#ifndef LOG_MEMORY_H
#define LOG_MEMORY_H

/********************************** Includes **********************************/
#include "configBits.h"

/****************************** Public Interfaces *****************************/
char ReadEEByte (char address);
unsigned char WriteEEByte (char address, char data);
void writeAndInc (char * address, char data);
char readAndInc (char * address);
void writeEntry (unsigned char * inputs, unsigned short int * numRemaining, unsigned short int operationTime, unsigned char * timeEnd, char numStored);
void saveResults (unsigned char * inputs, unsigned short int * numRemaining, unsigned short int operationTime, unsigned char * timeEnd);
boolean retrieveResults (unsigned char * inputs, unsigned short int * numRemaining, unsigned short int * operationTime, unsigned char * timeEnd, unsigned char logOption);

#endif /* LOG_MEMORY_H */

