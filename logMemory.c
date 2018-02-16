/*
 * File:   logMemory.c
 * Author: Salar Hosseini
 *
 * Created on February 3, 2018, 1:13 PM
 */

/***** Includes *****/
#include "logMemory.h"

/* Save to EEPROM
 * @param inputs - size 6 char array
 * @param numRemaining - size 4 int array
 * @param operationTime - int (3 digits)
 * @param timeEnd - size 7 char array
 */
void saveResults (unsigned char * inputs, unsigned short int * numRemaining, unsigned short int operationTime, unsigned char timeEnd){
    /* Allocation:
     * 6 bytes - inputs (6 chars)
     * 8 bytes - numRemaining (4*(2 digits))
     * 3 bytes - operationTime (3 digits)
     * 4 bytes - timeEnd (MM,DD,HH,MM) (don't save YY & SS)
     */
    
    //void eeprom_write(unsigned char address, unsigned char value);
}

/* READ FROM EEPROM
 * @param inputs - size 6 char array
 * @param numRemaining - size 4 int array
 * @param operationTime - int (3 digits)
 * @param timeEnd - size 7 char array
 */
void retrieveResults (unsigned char * inputs, unsigned short int * numRemaining, unsigned short int * operationTime, unsigned char * timeEnd, unsigned char logOption){
    
    //unsigned char eeprom_read(unsigned char address);
}
