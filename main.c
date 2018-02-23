/*
 * File:   main.c
 * Author: Salar Hosseini
 *
 * Created on February 1, 2018, 8:25 AM
 */

/***** Includes *****/
#include "standbyInterface.h"
#include "componentTests.h"
#include "prebuilt/glcd_pic.h"

/***** Defines *****/
#define VIB_TIMER_COUNT 3

/***** Global Variables *****/
unsigned volatile char timerCounter = 0; //Timer overflow counter
unsigned char currFastener = 0; //0-B, 1-W, 2-S, 3-N

/*
 * f_out = f_clk / (prescaler * (MAX_COUNT - TMR0_loadval))
 * 
 * f_clk = FOSC1 = 10MHz (external oscillator)
 * MAX_COUNT = 2^16 for 16-bit mode
 * TMR0_loadval = 0
 * Prescale value = 2^8 = 256 (see pg 123 of PIC18F4620 data sheet for assignment)
 *  
 * ==> f_out = 0.60 Hz ==> T_out = 1.68 s
 */
void initVibTimer(){
    T0CONbits.T08BIT = 0;   // 16-bit mode selected
    T0CONbits.T0CS = 0;     // Internal clock selected (timer mode ON)
    T0CONbits.PSA = 0;      // Prescaler assigned
    T0CONbits.T0PS0 = 1;    // Prescaler values
    T0CONbits.T0PS1 = 1;    // Prescaler values
    T0CONbits.T0PS2 = 1;    // Prescaler values
    
    T0CONbits.TMR0ON = 1;   // Turn ON the timer
}

/*
 * @param inputs - char array of inputs (size 6)
 * @param fasteners - int array of fastener counts (size 4) 0-B, 1-W, 2-S, 3-N
 * 
 * For a 4-step assembly, every other compartment must be filled in, starting from C1.
 * For a 5-step assembly, compartments 1, 2, 4, 5, and 7 must be filled in.
 * For a 6-step assembly, compartments 1, 2, 3, 5, 6, and 7 must be filled in.
 * For a 7 or 8-step assembly, compartments must be filled in consecutively, starting from C1.
 */
void setupAssemblyArrays (unsigned char * inputs, unsigned short int * fasteners, boolean * compartments){
    unsigned short int i, numSetsPerStep = (unsigned int)inputs[4]-48;
    const boolean CMPARTS [5][8] = {{1, 0, 1, 0, 1, 0, 1, 0},
                                    {1, 1, 0, 1, 1, 0, 1, 0},
                                    {1, 1, 1, 0, 1, 1, 1, 0},
                                    {1, 1, 1, 1, 1, 1, 1, 0},
                                    {1, 1, 1, 1, 1, 1, 1, 1}};
                                   
    for (i = 0; i<4; i++){
        switch (inputs[i]) {
            case '0':
                break;
            case 'B':
                fasteners[0] += numSetsPerStep;
                break;
            case 'W':
                fasteners[1] += numSetsPerStep;
                break;
            case 'S':
                fasteners[2] += numSetsPerStep;
                break;
            case 'N':
                fasteners[3] += numSetsPerStep;
                break;
            default:
                break;
        }
    }
    for (i = 0; i<7; i++){
        compartments[i] = CMPARTS[(unsigned int)inputs[5]-48-1][i];
    }
}

void initOperation(unsigned char * inputs){
    unsigned const short WHITE_THRESHOLD = 0x200;
    unsigned const char TAPE_LDR = 0, DEGREE_LDR = 1;
    unsigned short int fasteners [4] = {0,0,0,0}; //0-B, 1-W, 2-S, 3-N 
    boolean compartments [8] = {0, 0, 0, 0, 0, 0, 0, 0};
    unsigned short int i;
    
    /* Rotate Box CW until white tape found */
    LATBbits.LB3 = 1;
    while (readADC(TAPE_LDR) > WHITE_THRESHOLD){
        continue;
    }
    /* Rotate another 360 degrees to ensure compartment open*/
    __delay_ms(500);
    while (readADC(TAPE_LDR) > WHITE_THRESHOLD){
        continue;
    }
    
    /* Enable Timer Interrupt */
    INTCONbits.TMR0IE = 1;
    //INTCONbits.INT0IE = 1; //default falling edge
    ei (); //INTCONbits.GIE = 1 (global interrupt enable)
    /* Set up vibration motor timer */
    initVibTimer();
    
    ////////////////////////////////////////////////////////////////////////////
    setupAssemblyArrays(inputs, fasteners, compartments);
    
    
    ////////////////////////////////////////////////////////////////////////////
    
    /* Disable Timer Interrupt */
    INTCONbits.TMR0IE = 0;
    LATCbits.LC0 = 0;
    LATCbits.LC1 = 0;
    LATCbits.LC2 = 0;
    LATDbits.LD0 = 0;
    di();
    
    //OPERATION
    __delay_ms(3000);
}

void eepromTest (){
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
    TRISA = 0x0F; 
    TRISB = 0xF3; // All inputs
    TRISC = 0xE0; //RC5:7 - Microswitch
    TRISD = 0x02; // All output mode on port D for the LCD
    TRISE = 0x00;
    
    /************************** A/D Converter Module **************************/
    ADCON0 = 0x00;  // Disable ADC
    ADCON1 = 0x0D; // Set all A/D ports to digital (pg. 7-158)
    ADCON2bits.ADFM = 1; // Right justify A/D result
    
    // </editor-fold>
    
    //INTCON3bits.INT1IE = 1; //enable INT1 external interrupt 
    //ei (); //INTCONbits.GIE = 1
    
    //eepromTest();

    /* Test LDR */
    //ldrTest();
    
    /* Test DC Motor */
    //dcMotorTest();

    /* Test microswitch */
    //microswitchCountTest();
    
    ////////////////////////////////////////////////////////////////////////////
    
    /* Initialize GLCD. */
    initGLCD();
    glcdDrawRectangle(0, GLCD_SIZE_HORZ, 0, GLCD_SIZE_VERT, WHITE);
    draw();
    
    /* Main Operation */
    while(1){
        initStandby(inputs); //Initiate Standby Mode & get inputs
        getDateTime(timeStart);
        //CHANGE RB1 to DC motor control pin
        initOperation(inputs);
        getDateTime(timeEnd);
        operationTime = calcOperationTime (timeStart, timeEnd);
        doneScreen();
        showResults(inputs, numRemaining, operationTime);
        saveResults(inputs, numRemaining, operationTime, timeEnd);
    }
}

//GLOBAL VARIABLES MODIFIED IN ANY ISR should be declared volatile 
void interrupt interruptHandler(void) {
    //check both the interrupt enable and interrupt flag for INT1 interrupt
    if (INT1IE && INT1IF){ 
        /* Solenoid test */
        solenoidInterruptTest();
        INT1IF = 0; //clear flag
    }
    else if (TMR0IE && TMR0IF){ //for timer interrupts
        if (timerCounter >= VIB_TIMER_COUNT){
            switch (currFastener){
                case 0:
                    LATCbits.LC0 = ~LATCbits.LC0;
                    break;
                case 1:
                    LATCbits.LC1 = ~LATCbits.LC1;
                    break;
                case 2:
                    LATCbits.LC2 = ~LATCbits.LC2;
                    break;
                case 3:
                    LATDbits.LD0 = ~LATDbits.LD0;
                    break;
                default:
                    break;
            }
            timerCounter = 0;
        }
        else
            timerCounter++;
        TMR0IF = 0; //clear flag
    }
}