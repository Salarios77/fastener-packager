/*
 * File:   componentTests.c
 * Author: Salar Hosseini
 *
 * Created on February 19, 2018, 4:34 PM
 */

/***** Includes *****/
#include "componentTests.h"

/***** Typedefs *****/
typedef enum {false, true} boolean;

/***** Constants *****/
const unsigned char keys[] = "123A456B789C*0#D";

//Interrupt Service Routine for testing solenoids
void solenoidInterruptTest (){
    unsigned char keypress = (PORTB & 0xF0) >> 4;
    switch (keys[keypress]){
        case '1':
            LATAbits.LA0 = ~LATAbits.LA0;
            __delay_ms(150);
            LATAbits.LA0 = ~LATAbits.LA0;
            break;
        case '2':
            LATAbits.LA1 = ~LATAbits.LA1; 
            __delay_ms(150);
            LATAbits.LA1 = ~LATAbits.LA1; 
            break;
        case '3':
            LATAbits.LA2 = ~LATAbits.LA2; 
            __delay_ms(150);
            LATAbits.LA2 = ~LATAbits.LA2; 
            break;
        case '4':
            LATAbits.LA3 = ~LATAbits.LA3; 
            __delay_ms(150);
            LATAbits.LA3 = ~LATAbits.LA3; 
            break;
        default:
            break;
    }
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