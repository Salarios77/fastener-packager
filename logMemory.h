/*
 * File:   logMemory.h
 * Author: Salar Hosseini
 *
 * Created on February 3, 2018, 1:13 PM
 */

#ifndef LOG_MEMORY_H
#define LOG_MEMORY_H

/********************************** Includes **********************************/
#include <xc.h>
#include "configBits.h"

/****************************** Public Interfaces *****************************/
void saveResults (unsigned char * inputs, unsigned short int * numRemaining, unsigned short int operationTime, unsigned char timeEnd);
void retrieveResults (unsigned char * inputs, unsigned short int * numRemaining, unsigned short int * operationTime, unsigned char * timeEnd, unsigned char logOption);

#endif /* LOG_MEMORY_H */

