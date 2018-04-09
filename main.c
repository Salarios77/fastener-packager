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
#define N_VIB_ON_TIME  3 // 0.75s
#define N_VIB_OFF_TIME 17 // 4s 

#define S_VIB_ON_TIME  4 // 1s
#define S_VIB_OFF_TIME 17 // 4s 

#define W_VIB_ON_TIME  16 // 2s
#define W_VIB_OFF_TIME 17 // 4s 

#define B_VIB_ON_TIME  3 // 0.75s
#define B_VIB_OFF_TIME 17 // 4s 

#define WHITE_THRESHOLD_DEG 12500
#define WHITE_THRESHOLD_TAPE 6500
#define TAPE_LDR 0
#define DEGREE_LDR 1
#define TIME_OPEN_FLAP 50

/***** Global Variables *****/
unsigned volatile char timerCounter = 0; //Timer overflow counter
unsigned volatile char currFastener = 0; //0-B, 1-W, 2-S, 3-N
volatile boolean vibOn = true;

volatile boolean flapTurning = false;

volatile int counter = 0;

/*
 * f_out = f_clk / (4 * prescaler * (MAX_COUNT - TMR0_loadval))
 * 
 * f_clk = FOSC1 = 32MHz (internal oscillator with PLL)
 * MAX_COUNT = 2^16 for 16-bit mode
 * TMR0_loadval = 0
 * Prescale value = 2^5 = 32 (see pg 123 of PIC18F4620 data sheet for assignment)
 *  
 * ==> 1/f_out = T_out = 0.23 s
 */
void initVibTimer(){
    T0CONbits.T08BIT = 0;   // 16-bit mode selected
    T0CONbits.T0CS = 0;     // Internal clock selected (timer mode ON)
    T0CONbits.PSA = 0;      // Prescaler assigned
    T0CONbits.T0PS0 = 0;    // Prescaler values
    T0CONbits.T0PS1 = 0;    // Prescaler values
    T0CONbits.T0PS2 = 1;    // Prescaler values
    
    T0CONbits.TMR0ON = 1;   // Turn ON the timer
}

void rotateTillTape(boolean isCW){
    // <editor-fold defaultstate="collapsed" desc="Old rotate till tape">
    /*
    //rotate box CW
    LATAbits.LA2 = 1; //enable
    LATBbits.LB3 = 1;
    __delay_ms(750);
    boolean found = false;
    while (!found){
                    
        __lcd_clear();
        while (readADC(TAPE_LDR) > WHITE_THRESHOLD_TAPE + 20){ continue; }
        __delay_ms(1);
                    
        if (readADC(TAPE_LDR) < WHITE_THRESHOLD_TAPE + 20){
            found = true;
            __lcd_clear();
            printf ("found");
        }
    }
    LATBbits.LB3 = 0;
    __delay_ms (150);
    LATAbits.LA2 = 0; //disable
    */
    // </editor-fold>
    
    LATAbits.LA2 = 1; //enable
    if (isCW)
        LATBbits.LB3 = 1;
    else
        LATEbits.LE2 = 1;
    boolean found = true;
    __delay_ms(800);
    while (1){
        if (readADC(TAPE_LDR) < WHITE_THRESHOLD_TAPE) //WHITE
        {
            if (!found){
                __lcd_clear();
                printf("found tape");
                break;
            }
            else
                found = true;
        }
        else if (readADC(TAPE_LDR < WHITE_THRESHOLD_TAPE)){ //BLACK
            found = false;
        }
    }
    if (isCW)
        LATBbits.LB3 = 0;
    else
        LATEbits.LE2 = 0;
    __delay_ms (150);
    LATAbits.LA2 = 0; //disable
}

void rotate45Fancy(){
    /*
    int i;
    boolean passedCheck = true;
    //rotate box 45 degrees CW
    LATAbits.LA2 = 1; //enable
    LATBbits.LB3 = 1;
    //__delay_ms(1100);
    boolean found = true;
    __lcd_clear();
    while (1){
        i = 0;
        passedCheck = true;
        if (readADC(DEGREE_LDR) < WHITE_THRESHOLD_DEG) //WHITE
        {
            //check if stays
            while (i < 2){
                if (readADC(DEGREE_LDR) > WHITE_THRESHOLD_DEG){
                    passedCheck = false;
                    break;
                }
                __delay_ms(1);
                i++;
            }
            if (passedCheck){
                if (!found){
                    __lcd_clear();
                    printf("found");
                    break;
                }
                else
                    found = true;
            }
        }
        else if (readADC(DEGREE_LDR < WHITE_THRESHOLD_DEG)){ //BLACK
            //check if stays
            while (i < 15){
                if (readADC(DEGREE_LDR) > WHITE_THRESHOLD_DEG){
                    passedCheck = false;
                    break;
                }
                __delay_ms(1);
                i++;
            }
            if (passedCheck)
                found = false;
        }
    }
    LATBbits.LB3 = 0;
    __delay_ms (150);
    LATAbits.LA2 = 0; //disable
    */
}

//rotate box 45 degrees CW
void rotate45(){
    LATAbits.LA2 = 1; //enable
    LATBbits.LB3 = 1;
    boolean found = true;
    __delay_ms(500);
    while (1){
        if (readADC(DEGREE_LDR) < WHITE_THRESHOLD_DEG) //WHITE
        {
            if (!found){
                __lcd_clear();
                printf("found deg");
                break;
            }
            else
                found = true;
        }
        else if (readADC(DEGREE_LDR < WHITE_THRESHOLD_DEG)){ //BLACK
            found = false;
        }
    }
    LATBbits.LB3 = 0;
    __delay_ms (150);
    LATAbits.LA2 = 0; //disable
    
}

//rotate box 45 degrees CW
void rotate45CCW(){
    LATAbits.LA2 = 1; //enable
    LATEbits.LE2 = 1;
    boolean found = true;
    __delay_ms(500);
    while (1){
        if (readADC(DEGREE_LDR) < WHITE_THRESHOLD_DEG) //WHITE
        {
            if (!found){
                __lcd_clear();
                printf("found deg");
                break;
            }
            else
                found = true;
        }
        else if (readADC(DEGREE_LDR < WHITE_THRESHOLD_DEG)){ //BLACK
            found = false;
        }
    }
    LATEbits.LE2 = 0;
    __delay_ms (150);
    LATAbits.LA2 = 0; //disable
}

void rotatehalf45CCW(){
    LATAbits.LA2 = 1; //enable
    LATEbits.LE2 = 1;
    __delay_ms(1400);
    LATEbits.LE2 = 0;
    __delay_ms (150);
    LATAbits.LA2 = 0; //disable
}

//rotate box (find white tape), position flap down, initialize vibration
void setupDispensing (){
    __lcd_newline();
    printf ("Operation Test");
    
    rotateTillTape(true);
    rotateTillTape(true); //2nd time to ensure compartment open
    
    calibrateFlapStart(); //flap down
    
    // Enable Timer Interrupt
    vibOn = true;
    LATCbits.LC0 = 1;
    INTCONbits.TMR0IE = 1;
    // Set up vibration motor timer 
    initVibTimerTest();
    ei();
    
    LATAbits.LA3 = 0;
    LATEbits.LE0 = 0;
    LATEbits.LE1 = 0;
    __delay_ms(100);
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
void setupAssemblyArrays (unsigned char * inputs, unsigned short int * fasteners){
    unsigned short int i, numSetsPerStep = (unsigned short int)inputs[4]-48;                          
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
            case 'N':
                fasteners[2] += numSetsPerStep;
                break;
            case 'S':
                fasteners[3] += numSetsPerStep;
                break;
            default:
                break;
        }
    }
}

void setupCompartmentsArray (unsigned char * inputs, boolean * compartments){
    unsigned short int i;
    const boolean CMPARTS [5][8] = {{1, 0, 1, 0, 1, 0, 1, 0},
                                    {1, 1, 0, 1, 1, 0, 1, 0},
                                    {1, 1, 1, 0, 1, 1, 1, 0},
                                    {1, 1, 1, 1, 1, 1, 1, 0},
                                    {1, 1, 1, 1, 1, 1, 1, 1}};      
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
                if (PORTCbits.RC5 == 0){
                    __delay_ms(1);
                    if (PORTCbits.RC5 == 0){
                        return true;
                    }
                }
                break;
            case 1:
                if (PORTCbits.RC6 == 0){
                    __delay_ms(1);
                    if (PORTCbits.RC6 == 0){
                        return true;
                    }
                }
                break;
            case 2:
                if (PORTCbits.RC7 == 0){
                    return true;
                    //__delay_ms(1);
                    //if (PORTCbits.RC7 == 0){
                    //    return true;
                    //}
                }
                __delay_ms(1);
                break;
            case 3:
                if (PORTDbits.RD1 == 0){
                    __delay_ms(1);
                    if (PORTDbits.RD1 == 0){
                        return true;
                    }
                }
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
            //__delay_ms(500);
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
 * @param quantity_inputs: 1x9 array of # assembly steps, and 8 x (number of sets/compartment)
 * @param set_inputs: 8x4 2d array of the fasteners sets in the 8 compartments, with 0s as terminations
 * @param numRemaining - int array of fasteners remaining (size 4)
 */
void initOperation(unsigned char * quantityInputs, unsigned char setInputs [8][4], unsigned short int * numRemaining){
    unsigned short int fasteners [4] = {0,0,0,0}; //0-B, 1-W, 2-S, 3-N 
    boolean compartments [8] = {0, 0, 0, 0, 0, 0, 0, 0};
    unsigned short int i, j, k, setNum = 0; //loop variables
    boolean detectedFastener;
    unsigned char tempInputs [6];
    
    // <editor-fold defaultstate="collapsed" desc="Old Dispensing Setup">
    // Rotate Box CW until white tape found 
    /*
    LATAbits.LA2 = 1; //enable
    LATBbits.LB3 = 1;
    while (readADC(TAPE_LDR) > WHITE_THRESHOLD_TAPE){ continue; }
    // Rotate another 360 degrees to ensure compartment open
    __delay_ms(250);
    while (readADC(TAPE_LDR) > WHITE_THRESHOLD_TAPE) { continue; }
    LATBbits.LB3 = 0;
    __delay_ms(150);
    LATAbits.LA2 = 0; //disable
    
    /* Enable Timer Interrupt */
    /*
    INTCONbits.TMR0IE = 1;
    //INTCONbits.INT0IE = 1; //default falling edge
    ei (); //INTCONbits.GIE = 1 (global interrupt enable)
    // Set up vibration motor timer 
    initVibTimer();
    */
    // </editor-fold>
    setupDispensing();
    
    ////////////////////////////////////////////////////////////////////////////
    tempInputs[5] = quantityInputs[0];
    setupCompartmentsArray (tempInputs, compartments);
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
        
    //Cycle through every fastener type
    for (i = 7; i >= 0; i--){
        if (i != 7)
            rotate45(); //open next compartment
        if (!compartments[i])
            continue;
        
        //Setup assembly arrays
        tempInputs[4] = quantityInputs[setNum+1];
        for (j = 0; j < 4; j++){
            tempInputs[j] = setInputs[setNum][j];
        }
        setupAssemblyArrays(tempInputs, fasteners);
        setNum++;
        
        rotate45CCW(); //go to flap
        rotatehalf45CCW();
        LATBbits.LB0 = 1; //enable flap vibration
        for (j = 0; j < 4; j++){
            if (fasteners[j] > 0){
                LATCbits.LC0 = 0;
                LATCbits.LC1 = 0;
                LATCbits.LC2 = 0;
                LATDbits.LD0 = 0;
                currFastener = j;
                //__delay_ms (3000);
            }
            for (k = 0; k < fasteners[j]; k++){
                dispense(j);
            }
        }
        __delay_ms(3000);
        LATBbits.LB0 = 0; //disable flap vibration
        rotateTillTape(true);
    }
    
    rotateTest2(); //flap up
    
    // <editor-fold defaultstate="collapsed" desc="Old Dispensing">
    for (i = 0; i<8; i++){
        if (i != 0){
            //rotate box 45 degrees CW
            rotate45();
        }
        //Check if current compartment needs to be dispensed to
        if (!compartments[i])
            continue;
        
        //Setup assembly arrays
        tempInputs[4] = quantityInputs[setNum+1];
        for (j = 0; j < 4; j++){
            tempInputs[j] = setInputs[setNum][j];
        }
        setupAssemblyArrays(tempInputs, fasteners);
        setNum++;
        
        //Cycle through every fastener type
        for (j = 0; j < 4; j++){
            if (fasteners[j] > 0){
                LATCbits.LC0 = 0;
                LATCbits.LC1 = 0;
                LATCbits.LC2 = 0;
                LATDbits.LD0 = 0;
                currFastener = j;
                __delay_ms (3000);
            }
            for (k = 0; k < fasteners[j]; k++){
                dispense(j);
            }
        }  
    }
    // </editor-fold>
    ////////////////////////////////////////////////////////////////////////////
    
    // <editor-fold defaultstate="collapsed" desc="Old Flap Up">
    /*
    // Open extra's flap 
    LATAbits.LA3 = 1; //enable
    LATEbits.LE0 = 1;
    __delay_ms(TIME_OPEN_FLAP);
    LATEbits.LE0 = 0;
    __delay_ms(150);
    LATAbits.LA3 = 0; //disable
    */
    // </editor-fold>
    
    //Dispense extras
    detectedFastener = true;
    for (j = 0; j<4; j++){
        currFastener = j;
        while (detectedFastener){
            detectedFastener = dispense(j);
            if (detectedFastener){
                //numRemaining was designed for BNSW but operation was designed for BWNS
                switch (j){
                    case 0:
                        numRemaining[0]++;
                        break;
                    case 1:
                        numRemaining[3]++;
                        break;
                    case 2:
                        numRemaining[1]++;
                        break;
                    case 3:
                        numRemaining[2]++;
                        break;
                    default:
                        break;
                }
            }
        }
    }
    
    ////////////////////////////////////////////////////////////////////////////
    
    rotateTillTape(false);
    rotate45CCW();
    // <editor-fold defaultstate="collapsed" desc="Old Close Box">
    /*
    // Rotate box 405 degrees CCW 
    LATAbits.LA2 = 1; //enable
    LATEbits.LE2 = 1;
    __delay_ms(250);
    while (readADC(TAPE_LDR) > WHITE_THRESHOLD_TAPE){ continue; }
    // Rotate another 45 degrees to ensure compartment closed
    __delay_ms(250);
    while (readADC(DEGREE_LDR) > WHITE_THRESHOLD_DEG) { continue; }
    LATEbits.LE2 = 0;
    __delay_ms(150);
    LATAbits.LA2 = 0; //disable
    */
    // </editor-fold>
    
    /* Disable Timer Interrupt */
    INTCONbits.TMR0IE = 0;
    //Turn off any vibration motors that could be on
    LATCbits.LC0 = 0;
    LATCbits.LC1 = 0;
    LATCbits.LC2 = 0;
    LATDbits.LD0 = 0;
    di();
}

void pinConfig (){
    /* Latches are being cleared to ensure a controlled start-up state. */  
    LATA = 0x00;
    LATB = 0x00; 
    LATC = 0x00;
    LATD = 0x00;
    LATE = 0x00;

    /* After the states of LATx are known, the data direction registers, TRISx
     * are configured. Default is  1. */
    TRISA = 0x03; 
    TRISB = 0xF2; 
    TRISC = 0xE0; 
    TRISD = 0x02; 
    TRISE = 0x00;
}

void main(void) {
    unsigned char timeStart [7], timeEnd [7];
    unsigned char quantityInputs [9] = {'0','0','0','0','0','0','0','0','0'};
    unsigned char setInputs[8][4] = {'0', '0', '0', '0', 
                                      '0', '0', '0', '0', 
                                      '0', '0', '0', '0', 
                                      '0', '0', '0', '0', 
                                      '0', '0', '0', '0', 
                                      '0', '0', '0', '0', 
                                      '0', '0', '0', '0', 
                                      '0', '0', '0', '0'}; 
    unsigned short int numRemaining [4] = {0,0,0,0}; //# remaining of each fastener type
    unsigned short int operationTime;
    
    // <editor-fold defaultstate="collapsed" desc="Machine Configuration">
    
    /******************************* OSCILLATOR *******************************/
    //OSCCON = 0xF2;
    
    /* Use 8 MHz internal oscillator block with PLL enabled --> 32 MHz */
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
    OSCTUNEbits.PLLEN = 1; //PLL enabled for INTOSC
    
    /********************************* PIN I/O ********************************/
    pinConfig();
    
    /************************** A/D Converter Module **************************/
    ADCON0 = 0x00;  // Disable ADC
    ADCON1 = 0x0D; // (only A0, A1 analog - see pg. 7-158)
    ADCON2 = 0b100001010;
    //ADCON2bits.ADFM = 1; // Right justify A/D result
    
    // </editor-fold>
    
    // <editor-fold defaultstate="collapsed" desc="Testing Block">
    
    //INTCON3bits.INT1IE = 1; //enable INT1 external interrupt 
    //ei (); //INTCONbits.GIE = 1
    di();
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
    pinConfig(); //fix pins after using GLCD
    
    
    INTCON3bits.INT1IE = 1; //enable INT1 external interrupt 
    ei (); //INTCONbits.GIE = 1
    initLCD();
    printf ("Ready for test");
    while(1);
    
    
    /* Main Operation */
    
    /*
    while(1){
        initStandby(quantityInputs, setInputs); //Initiate Standby Mode & get inputs
        //getDateTime(timeStart);
        //initOperation(quantityInputs, setInputs, numRemaining);
        __delay_ms(3000);
        //getDateTime(timeEnd);
        //operationTime = calcOperationTime (timeStart, timeEnd);
        doneScreen();
        showResults(quantityInputs, setInputs, numRemaining, operationTime);
        saveResults(quantityInputs, setInputs, numRemaining, operationTime, timeEnd);
    }
    *
}

void vibrate (){
    __lcd_newline();
    printf ("timer");
    /* Enable Timer Interrupt */
    INTCONbits.TMR0IE = 1;
    /* Set up vibration motor timer */
    initVibTimer();
}

void calibrateDegreeIR (){
    __delay_ms(1000);
    while(PORTBbits.RB1 == 0){
        __lcd_home();
        printf("DEGREE IR: %d", readADC(DEGREE_LDR));
        LATAbits.LA2 = 1; //enable
        LATBbits.LB2 = 0;
        LATBbits.LB3 = 1;
        __delay_ms(3);
        LATBbits.LB3 = 0;
        __delay_ms(3);
        LATAbits.LA2 = 0; //disable
        __delay_ms(25);
    }
    LATBbits.LB3 = 0;
    __delay_ms(3);
    LATAbits.LA2 = 0; //disable
}

void operationTest(){
    boolean compartments [8] = {0, 0, 0, 0, 0, 0, 1, 1};
    int i, j, k;
    int fasteners [] = {2, 2, 2, 2};
           
    setupDispensing();
    
    //Cycle through every fastener type
    for (i = 7; i >= 0; i--){
        if (i != 7)
            rotate45(); //open next compartment
        if (!compartments[i])
            continue;
        rotate45CCW(); //go to flap
        rotatehalf45CCW();
        LATBbits.LB0 = 1; //enable flap vibration
        for (j = 0; j < 4; j++){
            if (fasteners[j] > 0){
                LATCbits.LC0 = 0;
                LATCbits.LC1 = 0;
                LATCbits.LC2 = 0;
                LATDbits.LD0 = 0;
                currFastener = j;
                //__delay_ms (3000);
            }
            for (k = 0; k < fasteners[j]; k++){
                dispense(j);
            }
        }
        __delay_ms(3000);
        LATBbits.LB0 = 0; //disable flap vibration
        rotateTillTape(true);
    }
    
    rotateTest2(); //flap up
    
    // <editor-fold defaultstate="collapsed" desc="Old">
    /*
    
                
                //SPACERS
                currFastener = 2;
                __delay_ms (5000);

                //push
                for (i = 0; i<3; i++) {
                    dispense(3);
                    __delay_ms(1000);
                }

                //WASHERS
                currFastener = 1;
                __delay_ms (5000);

                //push
                for (i = 0; i<3; i++) {
                    dispense(1);
                    __delay_ms(1000);
                }
                rotate45();
                
                //reset
                LATA = 0x00;
                LATB = 0x00; 
                LATC = 0x00;
                LATD = 0x00;
                LATE = 0x00;
    */
    // </editor-fold>
}

// <editor-fold defaultstate="collapsed" desc="Interrupt Handler">

//GLOBAL VARIABLES MODIFIED IN ANY ISR should be declared volatile 
void interrupt interruptHandler(void) {
    //check both the interrupt enable and interrupt flag for INT1 interrupt
    
    if (INT1IE && INT1IF){ 
        // Solenoid test 
        //solenoidInterruptTest();
        
        const unsigned char keys[] = "123A456B789C*0#D";
        unsigned char keypress = (PORTB & 0xF0) >> 4;
        int i;
        boolean pressed;
        switch (keys[keypress]){
            case '1':
                rotateTest2();
                break;
            case '6':  
                //calibrateFlapStart();
                operationTest();
                break;
            case '0':
                rotate45CCW();
                break;
            case '2':
                rotate45();
            case '3':
                rotateTillTape(true);
                rotateTillTape(true);
                rotate45CCW();
                rotate45CCW();
                //rotatehalf45CCW();
                __delay_ms(2000);
                rotateTillTape(true);
                rotate45();
                rotate45CCW();
                rotate45CCW();
                //rotatehalf45CCW();
                break;
            case 'A':
                pressed = dispense(0);
                __lcd_clear();
                if (pressed)
                    counter ++;
                printf ("Pressed A: %d", counter);
                break;
            case 'B':
                pressed = dispense(1);
                __lcd_clear();
                if (pressed)
                    counter ++;
                printf ("Pressed B: %d", counter);
                break;
            case 'C':
                pressed = dispense(2);
                __lcd_clear();
                if (pressed)
                    counter ++;
                printf ("Pressed C: %d", counter);
                break;
            case 'D':
                pressed = dispense(3);
                __lcd_clear();
                if (pressed)
                    counter ++;
                printf ("Pressed D: %d", counter);
                break;
            /*
            case '1': 
                rotateTest2(); //up
                //calibrateFlapStart();
                break;
            case '*':
                rotateTillTape(true);
                break;
            */
            case '#': 
                //rotateTest3(); 
                calibrateFlapStart(); //down
                break;
            /*
            case '2':
                //calibrateDegreeIR();
                rotate45();
                break;
            
            
            case '7':
                currFastener = 0;
                if (!TMR0IE)
                    vibrate();
                break;
            case '8':
                currFastener = 1;
                if (!TMR0IE)
                    vibrate();
                break;
            case '9':
                currFastener = 2;
                if (!TMR0IE)
                    vibrate();
                break;
            case '0':
                currFastener = 3;
                if (!TMR0IE)
                    vibrate();
                break;
            default:
                break;
            */
        }
        
        //week8Test();
        //rotateTest();
        INT1IF = 0; //clear flag
    }
    
    if (TMR0IE && TMR0IF){ //for timer interrupts
        timerCounter++;
        switch (currFastener){
            case 0:
                if (timerCounter >= B_VIB_ON_TIME && vibOn || timerCounter >= B_VIB_OFF_TIME && !vibOn){
                    vibOn = ~vibOn;
                    LATCbits.LC0 = ~LATCbits.LC0;
                    timerCounter = 0;
                }
                break;
            case 1:
                if (timerCounter >= W_VIB_ON_TIME && vibOn || timerCounter >= W_VIB_OFF_TIME && !vibOn){
                    vibOn = ~vibOn;
                    //LATCbits.LC1 = ~LATCbits.LC1;
                    LATDbits.LD0 = ~LATDbits.LD0;
                    timerCounter = 0;
                }
                break;
            case 2:
                if (timerCounter >= S_VIB_ON_TIME && vibOn || timerCounter >= S_VIB_OFF_TIME && !vibOn){
                    vibOn = ~vibOn;
                    LATCbits.LC1 = ~LATCbits.LC1;
                    timerCounter = 0;
                }
                break;
            case 3:
                if (timerCounter >= N_VIB_ON_TIME && vibOn || timerCounter >= N_VIB_OFF_TIME && !vibOn){
                    vibOn = ~vibOn;
                    LATCbits.LC2 = ~LATCbits.LC2;
                    timerCounter = 0;
                }
                break;
            default:
                break;
        }
        TMR0IF = 0; //clear flag
    }
}

// </editor-fold>