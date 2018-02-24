/*
 * File:   componentTests.c
 * Author: Salar Hosseini
 *
 * Created on February 19, 2018, 4:34 PM
 */

/***** Includes *****/
#include "componentTests.h"

/***** Constants *****/
const unsigned char keys[] = "123A456B789C*0#D";

//Interrupt Service Routine for testing solenoids
void solenoidInterruptTest (){
    
    unsigned char keypress = (PORTB & 0xF0) >> 4;
    switch (keys[keypress]){
        case '1':
            LATAbits.LA4 = ~LATAbits.LA4;
            __delay_ms(150);
            LATAbits.LA4 = ~LATAbits.LA4;
            break;
        case '2':
            LATAbits.LA5 = ~LATAbits.LA5; 
            __delay_ms(150);
            LATAbits.LA5 = ~LATAbits.LA5; 
            break;
        case '3':
            LATAbits.LA6 = ~LATAbits.LA6; 
            __delay_ms(150);
            LATAbits.LA6 = ~LATAbits.LA6; 
            break;
        case '4':
            LATAbits.LA7 = ~LATAbits.LA7; 
            __delay_ms(150);
            LATAbits.LA7 = ~LATAbits.LA7; 
            break;
        default:
            break;
    }
    
    /*
    LATAbits.LA4 = ~LATAbits.LA4;
    __delay_ms(150);
    LATAbits.LA4 = ~LATAbits.LA4;
    */
}

//Test microswitches and try counting number of presses
void microswitchCountTest (){
    boolean pressed = false;
    int counter = 0;
    initLCD();
    
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
}

void eepromTest(){
    unsigned char timeStart [7], timeEnd [7];
    unsigned char inputs [6] = {'A','0','1','2','3','4'};
    unsigned short int numRemaining [4] = {24,5,3,15}; //# remaining of each fastener type
    unsigned short int operationTime = 125;
    
    initRTC();
    
    getDateTime(timeEnd);
    saveResults (inputs, numRemaining, operationTime, timeEnd);
    
    inputs[0] = 'B';
    saveResults (inputs, numRemaining, operationTime, timeEnd);
}

/*
 * Provided by A2C sample code.
 */
unsigned short readADC(char channel){
    /* Reads the analog input from the specified analog channel.
     *
     * Arguments: channel, the byte corresponding to the channel to read
     *
     * Returns: the 10-bit value corresponding to the voltage read from
     *          the specified channel
     */
    
    ADCON0 = (channel & 0x0F) << 2; // Select ADC channel (i.e. pin)
    ADON = 1; // Enable module
    ADCON0bits.GO = 1; // Initiate sampling
    while(ADCON0bits.GO_NOT_DONE){  continue;   } // Poll for acquisition completion
    return (ADRESH << 8) | ADRESL; // Return result as a 16-bit value
}

/*
 * Provided by A2C sample code
 */
void ldrTest(){
    /* Initialize character LCD. */
    initLCD();

    /* Main loop. */
    while(1) {
        __lcd_clear();
        __lcd_home();
        
       /* Print results to character LCD. Since ADC results are 10 bits, they
        * are split up into two registers ADRESH and ADRESL. The results of the
        * ADC are displayed in hexadecimal so that the values in each of these
        * registers will be easily visible; ADRESL corresponds to the two least
        * significant digits, while ADRESH corresponds to the most significant 
        * bit. */
       printf("RA0: %.3x", readADC(0));
       __lcd_newline();
       printf("RA1: %.3x", readADC(1));
       
       __delay_ms(100);
    }
}

void dcMotorTest(){
    LATBbits.LB3 = ~LATBbits.LB3;
    //LATBbits.LB2 = ~LATBbits.LB2;
    //__delay_ms(5000);
    //LATBbits.LB3 = ~LATBbits.LB3;
    //LATBbits.LB2 = ~LATBbits.LB2;
}