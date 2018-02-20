/*
 * File:   logMemory.c
 * Author: Salar Hosseini
 *
 * Created on February 3, 2018, 1:13 PM
 */

/***** Includes *****/
#include "logMemory.h"

char readByteEE (char address){
    while (EECON1bits.WR == 1);
    EEADR = address; // load address of EEPROM to read
    EECON1bits.EEPGD = 0; // access EEPROM data memory
    EECON1bits.CFGS = 0; // do not access configuration registers
    EECON1bits.RD = 1; // initiate read 
    while (EECON1bits.RD == 1);
    return EEDATA; // return EEPROM byte
}

unsigned char writeByteEE (char address, char data){
    EECON1bits.WREN = 1; // allow EEPROM writes
    EEADR = address; // load address of write to EEPROM
    EEDATA = data; // load data to write to EEPROM
    EECON1bits.EEPGD = 0; // access EEPROM data memory
    EECON1bits.CFGS = 0; // do not access configuration registers
    INTCONbits.GIE = 0; // disable interrupts for critical EEPROM write sequence
    
    /* Critical Sequence */
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;
    
    while (EECON1bits.WR == 1); // wait for write to complete
    INTCONbits.GIE = 1; // enable interrupts, critical sequence complete
    EECON1bits.WREN = 0; // do not allow EEPROM writes

    //Verify write operation
    if (readByteEE(address) == data) // read the byte we just wrote to EEPROM
        return 1; // write was successful
    return 0; // write failed
}

void writeAndInc (char * address, char data){
    writeByteEE (*address, data);
    *address += 1;
}

char readAndInc (char * address){
    *address += 1;
    return readByteEE (*address - 1);
}

/*
 * @param numStored - 0,1,2,3,4
 */
void writeEntry (unsigned char * inputs, unsigned short int * numRemaining, unsigned short int operationTime, unsigned char * timeEnd, char numStored){
    int i; //loop variable
    char writeAddr = 0x01 + numStored*16; //15 is number of bytes per entry
    
    for (i = 0; i < 6; i++){
        writeAndInc (&writeAddr, inputs[i]);    
    }
    for (i = 0; i < 4; i++){
        writeAndInc (&writeAddr, (char)numRemaining[i]);    
    }
    writeAndInc (&writeAddr, (char)operationTime);
    //Time arrays in form: {YY,MM,DD,WEEKDAY,HH,MM,SS}
    writeAndInc (&writeAddr, timeEnd[5]);
    writeAndInc (&writeAddr, timeEnd[4]);
    writeAndInc (&writeAddr, timeEnd[2]);
    writeAndInc (&writeAddr, timeEnd[1]);
}

/* Save to EEPROM
 * @param inputs - size 6 char array
 * @param numRemaining - size 4 int array
 * @param operationTime - int
 * @param timeEnd - size 7 char array
 * 
 * Allocation:
 * Starting at 0x00,
 * 1 byte  - number of logs stored
 * 6 bytes - inputs (6 chars)
 * 4 bytes - numRemaining (4 numbers)
 * 1 byte  - operationTime (1 numbers)
 * 4 bytes - timeEnd (MM,DD,HH,MM) (don't save YY & SS)
 */
void saveResults (unsigned char * inputs, unsigned short int * numRemaining, unsigned short int operationTime, unsigned char * timeEnd){
    char numStored, writeAddr = 0x00;
    int i; //loop variable
    //variables for shifting
    unsigned char timeEndTemp [7];
    unsigned char inputsTemp [6];
    unsigned short int numRemainingTemp [4]; //# remaining of each fastener type
    unsigned short int operationTimeTemp;
    
    numStored = readByteEE (0x00); //this byte indicates how many logs have been stored
    switch (numStored){
        case 0xFF: case 0x01: case 0x02: case 0x03: case 0x04:
            if (numStored == 0xFF)
                numStored = 0x00;
            
            //shift elements back
            for (i = numStored; i>0; i--){
                if (i != 4){
                    retrieveResults (inputsTemp, numRemainingTemp, &operationTimeTemp, timeEndTemp, i);
                    writeEntry (inputsTemp, numRemainingTemp, operationTimeTemp, timeEndTemp, i);
                }
            }
            writeEntry (inputs, numRemaining, operationTime, timeEnd, 0x00);
            
            if (numStored < 4)
                numStored ++;
            writeByteEE (0x00, numStored); //update value indicating how many logs have been stored
            break;
        default:
            break;
    }
}

/* READ FROM EEPROM
 * @param inputs - size 6 char array
 * @param numRemaining - size 4 int array
 * @param operationTime - int
 * @param timeEnd - size 7 char array
 * @param logOption - 1,2,3,4
 */
void retrieveResults (unsigned char * inputs, unsigned short int * numRemaining, unsigned short int * operationTime, unsigned char * timeEnd, unsigned char numStored){
    int i; //loop variable
    char readAddr = 0x01 + (numStored-1)*16; //15 is number of bytes per entry

    for (i = 0; i < 6; i++){
        inputs [i] = (unsigned char)readAndInc (&readAddr);
    }
    for (i = 0; i < 4; i++){
        numRemaining[i] = (unsigned short int)readAndInc (&readAddr);
    }
    *operationTime = (unsigned short int)readAndInc (&readAddr);
    timeEnd[5] = (unsigned char)readAndInc(&readAddr);
    timeEnd[4] = (unsigned char)readAndInc(&readAddr);
    timeEnd[2] = (unsigned char)readAndInc(&readAddr);
    timeEnd[1] = (unsigned char)readAndInc(&readAddr);
}