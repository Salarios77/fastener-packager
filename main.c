/*
 * File:   main.c
 * Author: Salar Hosseini
 *
 * Created on February 1, 2018, 8:25 AM
 */

/***** Includes *****/
#include <xc.h>
#include "configBits.h"
#include "standbyInterface.h"
#include "logMemory.h"
#include "lcd.h"

/*
 * For a 4-step assembly, every other compartment must be filled in, starting from C1.
 * For a 5-step assembly, compartments 1, 2, 4, 5, and 7 must be filled in.
 * For a 6-step assembly, compartments 1, 2, 3, 5, 6, and 7 must be filled in.
 * For a 7 or 8-step assembly, compartments must be filled in consecutively, starting from C1.
 */
void initOperation(unsigned char * inputs){
    //OPERATION
    __delay_ms(3000);
}

void main(void) {
    unsigned char timeStart [7], timeEnd [7];
    unsigned char inputs [6] = {'0','0','0','0','0','0'};
    unsigned short int numRemaining [4] = {0,0,0,0}; //# remaining of each fastener type
    unsigned short int operationTime;
    
    // <editor-fold defaultstate="collapsed" desc="Machine Configuration">
    /********************************* PIN I/O ********************************/
    /* Latches are being cleared to ensure a controlled start-up state. */  
    LATA = 0x00;
    LATB = 0x00; 
    LATC = 0x00;
    LATD = 0x00;
    LATE = 0x00;

    /*After the states of LATx are known, the data direction registers, TRISx
     * are configured. Default is  1. */
    TRISA = 0x00; 
    TRISB = 0xFF; // All inputs
    TRISC = 0xE0; //RC5:7 - Microswitch
    TRISD = 0x00; // All output mode on port D for the LCD
    TRISE = 0x00;
    
    /************************** A/D Converter Module **************************/
    ADCON0 = 0x00;  // Disable ADC
    ADCON1 = 0b00001111; // Set all A/D ports to digital (pg. 222)
    // </editor-fold>
    
    
    while(1){
        initStandby(inputs); //Initiate Standby Mode & get inputs
        getDateTime(timeStart);
        initOperation(inputs);
        getDateTime(timeEnd);
        operationTime = calcOperationTime (timeStart, timeEnd);
        showResults(inputs, numRemaining, operationTime);
        //saveResults(inputs, numRemaining, operationTime, timeEnd);
    }
    

    /*
    boolean pressed = false;
    int counter = 0;
    initLCD();
    
    //Enable Interrupts
    INTCON3bits.INT1IE = 1; //enable INT1 external interrupt //enable INT1 external interrupt
    ei(); //global interrupt enable - INTCONbits.GIE = 1;
    
    //Microswitch
    while(1){
        if (!pressed && PORTCbits.RC5 == 0){
            pressed = true;
            LATAbits.LA1 = ~LATAbits.LA1;
            counter ++;
            __lcd_clear();
            printf ("counter: %d", counter);
        }
        else if (PORTCbits.RC5 == 1){
            pressed = false;
        }
    }
    */
}

//GLOBAL VARIABLES MODIFIED IN ANY ISR should be declared volatile 
void interrupt interruptHandler(void) {
    //check both the interrupt enable and interrupt flag for INT1 interrupt
    if (INT1IE && INT1IF){ 
        LATAbits.LA0 = ~LATAbits.LA0; //toggle led
        __delay_ms(150);
        LATAbits.LA0 = ~LATAbits.LA0; //toggle led
        INT1IF = 0; //clear flag
    }
    
    //for timer interrupts
    //if (TMR0IE && TMR0IF){
    //}
} 
