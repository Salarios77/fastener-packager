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
#define VIB_TIMER_COUNT 5
#define WHITE_THRESHOLD 0x200
#define TAPE_LDR 0
#define DEGREE_LDR 1
#define TIME_OPEN_FLAP 2000

/***** Global Variables *****/
unsigned volatile char timerCounter = 0; //Timer overflow counter
unsigned volatile char currFastener = 0; //0-B, 1-W, 2-S, 3-N

/*
 * f_out = f_clk / (4 * prescaler * (MAX_COUNT - TMR0_loadval))
 * 
 * f_clk = FOSC1 = 32MHz (internal oscillator with PLL)
 * MAX_COUNT = 2^16 for 16-bit mode
 * TMR0_loadval = 0
 * Prescale value = 2^7 = 128 (see pg 123 of PIC18F4620 data sheet for assignment)
 *  
 * ==> f_out = 0.95 Hz ==> T_out = 1.05 s
 */
void initVibTimer(){
    T0CONbits.T08BIT = 0;   // 16-bit mode selected
    T0CONbits.T0CS = 0;     // Internal clock selected (timer mode ON)
    T0CONbits.PSA = 0;      // Prescaler assigned
    T0CONbits.T0PS0 = 0;    // Prescaler values
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
    unsigned short int i, numSetsPerStep = (unsigned short int)inputs[4]-48;
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
        compartments[i] = CMPARTS[(unsigned int)inputs[5]-48-4][i];
    }
}

boolean microswitchInput(unsigned short int currFastener){
    unsigned int DETECT_TIME = 150;
    unsigned int i;
    for (i = 0; i < DETECT_TIME; i++){
        switch (currFastener){
            case 0:
                if (PORTCbits.RC5 == 0)
                    return true;
                break;
            case 1:
                if (PORTCbits.RC6 == 0)
                    return true;
                break;
            case 2:
                if (PORTCbits.RC7 == 0)
                    return true;
                break;
            case 3:
                if (PORTDbits.RD1 == 0)
                    return true;
                break;
            default:
                break;
        }
        __delay_ms(1);
    }
    return false;
}

boolean dispense (unsigned short int currFastener){
    boolean pressed = false;
    switch (currFastener){
        case 0:
            LATAbits.LA4 = ~LATAbits.LA4;
            pressed = microswitchInput(currFastener);
            LATAbits.LA4 = ~LATAbits.LA4;
            break;
        case 1:
            LATAbits.LA5 = ~LATAbits.LA5;
            pressed = microswitchInput(currFastener);
            LATAbits.LA5 = ~LATAbits.LA5;
            break;
        case 2:
            LATAbits.LA6 = ~LATAbits.LA6;
            pressed = microswitchInput(currFastener);
            LATAbits.LA6 = ~LATAbits.LA6;
            break;
        case 3:
            LATAbits.LA7 = ~LATAbits.LA7;
            pressed = microswitchInput(currFastener);
            LATAbits.LA7 = ~LATAbits.LA7;
            break;
        default:
            break;
    }
    __delay_ms(2000);
    return pressed;
}

/*
 * @param inputs - char arr of inputs (size 6)
 * @param numRemaining - int array of fasteners remaining (size 4)
 */
void initOperation(unsigned char * inputs, unsigned short int * numRemaining){
    unsigned short int fasteners [4] = {0,0,0,0}; //0-B, 1-W, 2-S, 3-N 
    boolean compartments [8] = {0, 0, 0, 0, 0, 0, 0, 0};
    unsigned short int i, j, k; //loop variables
    boolean detectedFastener;
    
    /* Rotate Box CW until white tape found */
    LATBbits.LB3 = 1;
    while (readADC(TAPE_LDR) > WHITE_THRESHOLD){ continue; }
    /* Rotate another 360 degrees to ensure compartment open*/
    __delay_ms(500);
    while (readADC(TAPE_LDR) > WHITE_THRESHOLD) { continue; }
    LATBbits.LB3 = 0;
    
    /* Enable Timer Interrupt */
    INTCONbits.TMR0IE = 1;
    //INTCONbits.INT0IE = 1; //default falling edge
    ei (); //INTCONbits.GIE = 1 (global interrupt enable)
    /* Set up vibration motor timer */
    initVibTimer();
    
    ////////////////////////////////////////////////////////////////////////////
    setupAssemblyArrays(inputs, fasteners, compartments);
    
    // <editor-fold defaultstate="collapsed" desc="For Debugging">
    /*
    __lcd_clear();
    for (i = 0; i < 6; i++){
        putch (inputs[i]);
    }
    putch (' ');
    for (i = 0; i < 4; i++){
        putch (fasteners[i]+48);
    }
    __lcd_newline();
    for (i = 0; i < 8; i++){
        putch (compartments[i]+48);
    }    
    while(1);
    */
    // </editor-fold>
        
    for (i = 0; i<8; i++){
        if (i != 0){
            //rotate box 45 degrees CW
            LATBbits.LB3 = 1;
            while (readADC(DEGREE_LDR) > WHITE_THRESHOLD){ continue; }
            LATBbits.LB3 = 0;
        }
        //Check if current compartment needs to be dispensed to
        if (!compartments[i])
            continue;
        //Cycle through every fastener type
        for (j = 0; j < 4; j++){
            currFastener = j;
            for (k = 0; k < fasteners[j]; k++){
                dispense(j);
            }
        }  
    }
    
    ////////////////////////////////////////////////////////////////////////////
    /* Open extra's flap */
    LATEbits.LE0 = 1;
    __delay_ms(TIME_OPEN_FLAP);
    LATEbits.LE0 = 0;
    
    /* dispense extras */
    detectedFastener = true;
    for (j = 0; j<4; j++){
        currFastener = j;
        while (detectedFastener){
            detectedFastener = dispense(j);
            if (detectedFastener){
                //numRemaining was designed for BNSW but operation was designed for BWSN
                if (j == 1)
                    numRemaining[3]++;
                else if (j == 3)
                    numRemaining[1]++;
                else
                    numRemaining[j]++;
            }
        }
    }
    
    /* Close extra's flap */
    LATEbits.LE1 = 1;
    __delay_ms(TIME_OPEN_FLAP);
    LATEbits.LE1 = 0;
    ////////////////////////////////////////////////////////////////////////////
    
    /* Rotate box 405 degrees CCW */
    LATBbits.LB2 = 1;
    __delay_ms(500);
    while (readADC(TAPE_LDR) > WHITE_THRESHOLD){ continue; }
    /* Rotate another 45 degrees to ensure compartment closed*/
    __delay_ms(500);
    while (readADC(DEGREE_LDR) > WHITE_THRESHOLD) { continue; }
    LATBbits.LB2 = 0;
    
    /* Disable Timer Interrupt */
    INTCONbits.TMR0IE = 0;
    //Turn off any vibration motors that could be on
    LATCbits.LC0 = 0;
    LATCbits.LC1 = 0;
    LATCbits.LC2 = 0;
    LATDbits.LD0 = 0;
    di();
}

void main(void) {
    unsigned char timeStart [7], timeEnd [7];
    unsigned char inputs [6] = {'0','0','0','0','0','0'};
    unsigned short int numRemaining [4] = {0,0,0,0}; //# remaining of each fastener type
    unsigned short int operationTime;
    
    // <editor-fold defaultstate="collapsed" desc="Machine Configuration">
    
    /******************************* OSCILLATOR *******************************/
    /* Use 8 MHz internal oscillator block with PLL enabled --> 32 MHz */
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
    OSCTUNEbits.PLLEN = 1; //PLL enabled for INTOSC
    
    /********************************* PIN I/O ********************************/
    /* Latches are being cleared to ensure a controlled start-up state. */  
    LATA = 0x00;
    LATB = 0x00; 
    LATC = 0x00;
    LATD = 0x00;
    LATE = 0x00;

    /* After the states of LATx are known, the data direction registers, TRISx
     * are configured. Default is  1. */
    TRISA = 0x03; 
    TRISB = 0xF3; 
    TRISC = 0xE0; 
    TRISD = 0x02; 
    TRISE = 0x00;
    
    /************************** A/D Converter Module **************************/
    ADCON0 = 0x00;  // Disable ADC
    ADCON1 = 0x0D; // (only A0, A1 analog - see pg. 7-158)
    ADCON2bits.ADFM = 1; // Right justify A/D result
    
    // </editor-fold>
    
    // <editor-fold defaultstate="collapsed" desc="Testing Block">
    
    //INTCON3bits.INT1IE = 1; //enable INT1 external interrupt 
    //ei (); //INTCONbits.GIE = 1
    
    //eepromTest();

    /* Test LDR */
    //ldrTest();
    
    /* Test DC Motor */
    //dcMotorTest();
    //while(1);

    /* Test microswitch */
    //microswitchCountTest();
    // </editor-fold>
    
    ////////////////////////////////////////////////////////////////////////////
    
    /* Initialize GLCD. */
    initGLCD();
    glcdDrawRectangle(0, GLCD_SIZE_HORZ, 0, GLCD_SIZE_VERT, WHITE);
    draw();
    
    /* Main Operation */
    while(1){
        initStandby(inputs); //Initiate Standby Mode & get inputs
        getDateTime(timeStart);
        //initOperation(inputs, numRemaining);
        __delay_ms(3000);
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
        timerCounter++;
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
        TMR0IF = 0; //clear flag
    }
}