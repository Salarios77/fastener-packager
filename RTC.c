/*
 * File:   RTC.c
 * Author: Salar Hosseini
 *
 * Created on February 2, 2018, 10:51 PM
 */

#include "RTC.h"

/***** Constants *****/
const char firstTimeSet[7] = {  0x00, // 30 Seconds 
                                0x53, // 51 Minutes
                                0x15, // 24 hour mode, set to 15:00
                                0x01, // Monday
                                0x05, // 5th
                                0x02, // February
                                0x18  // 2018
};

void initRTC (){
    I2C_Master_Init(100000); //Initialize I2C Master with 100 kHz clock
    
    /* Set the time in the RTC.
     * 
     * To see the RTC keep time, comment this line out after programming the PIC
     * directly before with this line included. */
    //RTC_setTime();
}

/* Uncomment and call to set the RTC time for the first time
 * Writes the firstTimeSet array to the RTC memory.
 * Arguments: none
 * Returns: none
void RTC_setTime(){
    I2C_Master_Start(); // Start condition
    I2C_Master_Write(0b11010000); //7 bit RTC address + Write
    I2C_Master_Write(0x00); // Set memory pointer to seconds
    
    //Write array. 
    for(char i=0; i<7; i++){
        I2C_Master_Write(firstTimeSet[i]);
    }
    
    I2C_Master_Stop(); //Stop condition
}
*/

/*
 * @param time: the time to be determined - {YY,MM,DD,WEEKDAY,HH,MM,SS}
 */
void getDateTime (unsigned char time [7]){
    unsigned char i; // Loop counter

    /* Reset RTC memory pointer. */
    I2C_Master_Start(); // Start condition
    I2C_Master_Write(0b11010000); // 7 bit RTC address + Write
    I2C_Master_Write(0x00); // Set memory pointer to seconds
    I2C_Master_Stop(); // Stop condition

    /* Read current time. */
    I2C_Master_Start(); // Start condition
    I2C_Master_Write(0b11010001); // 7 bit RTC address + Read
    for(i = 0; i < 6; i++){
        time[i] = I2C_Master_Read(ACK); // Read with ACK to continue reading
    }
    time[6] = I2C_Master_Read(NACK); // Final Read with NACK
    I2C_Master_Stop(); // Stop condition
}