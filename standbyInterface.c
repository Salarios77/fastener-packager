/*
 * File:   StandbyInterface.c
 * Author: Salar Hosseini
 *
 * Created on February 1, 2018, 1:02 PM
 */

/***** Includes *****/
#include "standbyInterface.h"

/***** Constants *****/
const unsigned char keys[] = "123A456B789C*0#D"; 

//Time arrays in form: {YY,MM,DD,WEEKDAY,HH,MM,SS}
unsigned short int calcOperationTime (unsigned char * timeStart, unsigned char * timeEnd){
    unsigned short int hourStart, minStart, secStart, hourEnd, minEnd, secEnd; 
    hourStart = (unsigned) __bcd_to_num((unsigned)timeStart[2]);
    minStart = (unsigned)__bcd_to_num((unsigned)timeStart[1]);
    secStart = (unsigned)__bcd_to_num((unsigned)timeStart[0]);
    hourEnd = (unsigned)__bcd_to_num((unsigned)timeEnd[2]);
    minEnd = (unsigned)__bcd_to_num((unsigned)timeEnd[1]);
    secEnd = (unsigned)__bcd_to_num((unsigned)timeEnd[0]);
    
    return (hourEnd*3600 + minEnd*60 + secEnd) - (hourStart*3600 + minStart*60 + secStart);
}

unsigned char getKeyPressed (boolean shouldDispTime){
    /* RB1 is the interrupt pin, so if there is no key pressed, RB1 will be
     * 0. The PIC will wait and do nothing until a key press is signaled. */
    
    unsigned char lastTimeRefresh [7], currTimeTemp [7];
    int i;
    
    if (shouldDispTime){
        dispRealTime();
        getDateTime(lastTimeRefresh);
        lcd_set_cursor(15,2);
    }
    
    while(PORTBbits.RB1 == 0){
        if (shouldDispTime){
            getDateTime(currTimeTemp);
            //Refresh RTC display time every 0.5s
            if (calcOperationTime(lastTimeRefresh, currTimeTemp) >=0.5){
                dispRealTime();
                lcd_set_cursor(15,2);
                for (i = 0; i<7; i++){
                    lastTimeRefresh[i] = currTimeTemp[i];
                }
            }
        }
        continue;   
    }
        
    /* Read the 4-bit character code. */
    unsigned char keypress = (PORTB & 0xF0) >> 4;
        
    /* Wait until the key has been released. */
    while(PORTBbits.RB1 == 1){  
        if (shouldDispTime){
            getDateTime(currTimeTemp);
            if (calcOperationTime(lastTimeRefresh, currTimeTemp) >=0.5){
                dispRealTime();
                for (i = 0; i<7; i++){
                    lastTimeRefresh[i] = currTimeTemp[i];
                }
            }
        }
        continue;   
    }
        
    Nop();  // Apply breakpoint here to prevent compiler optimizations
        
    return keys[keypress];
}

void waitA(){
    while(1){
        if (getKeyPressed(false) == 'A')
            break;
    }
}

void dispRealTime (){
    unsigned char time [7];
    getDateTime(time);
    
    // Print received data to LCD.
    __lcd_home();
    printf("%02x/%02x/20%02x %02x:%02x", time[4],time[5],time[6],time[2],time[1]); // Print date&time in DD/MM/YYYY HH:MM
    __delay_ms(115);
}

void errScreen(){
    __lcd_clear();
    printf ("INVALID INPUT!");
    __lcd_newline();
    printf ("HIT A");
    waitA();
}

/*
 * Fastener set: B, N, S, W, BN, BS, BW, BBN, BBS, BBW, BNW, BSW, BWW, BNWW,
 * BSWW, BBSW, BBNW, BNNW, BNNN, BWWW
 * Number of fastener sets per step (compartment): 1, 2, 3, 4
 * Number of assembly steps: 4, 5, 6, 7, 8
 * 
 * Must comply with the following rules:
 * - The maximum number of each fastener to be dispensed in each compartment is: 2 for B, 3 for N,
 *   2 for S, and 4 for W.
 * - The maximum number of total fasteners to be dispensed in each compartment is 4.
 * 
 * @param inputScreenPos: the current input screen {1,2,3}
 * @param inputs: the the input array with inputs from all three input screens (size 6)
*/
boolean checkValid (unsigned short int inputScreenPos, unsigned char * inputs){    
    
    unsigned char validFastenerSets [20][5] = {"B000", "N000", "S000", "W000", 
                                 "BN00", "BS00", "BW00", 
                                 "BBN0", "BBS0", "BBW0", "BNW0", "BSW0", "BWW0", 
                                 "BNWW", "BSWW", "BBSW", "BBNW", "BNNW", "BNNN", "BWWW"};
    unsigned char fastenerSet [5];
    unsigned short int i, numSetsPerStep;
    unsigned short int numFasteners [4] = {0,0,0,0};
    const unsigned int MAX_FASTENERS [4] = {2,3,2,4};
    
    switch (inputScreenPos) {
        case 1: //Fastener set selection
            for (i = 0; i<4; i++){ //make copy of fastener set inputs
                fastenerSet[i] = inputs[i];
            }
            fastenerSet[4] = '\0';
            for (i = 0; i<20; i++){
                if (strcmp(fastenerSet, validFastenerSets[i]) == 0)
                    return true;
            }
            return false;
        case 2: //Sets per step selection
            //Count the fasteners in each compartment
            numSetsPerStep = (unsigned int)inputs[4]-48;
            for (i = 0; i<4; i++){
                switch (inputs[i]){
                    case '0':
                        break;
                    case 'B':
                        numFasteners[0] += numSetsPerStep;
                        if (numFasteners[0] > MAX_FASTENERS[0])
                            return false;
                        break;
                    case 'N':
                        numFasteners[1] += numSetsPerStep;
                        if (numFasteners[1] > MAX_FASTENERS[1])
                            return false;
                        break;
                    case 'S':
                        numFasteners[2] += numSetsPerStep;
                        if (numFasteners[2] > MAX_FASTENERS[2])
                            return false;
                        break;
                    case 'W':
                        numFasteners[3] += numSetsPerStep;
                        if (numFasteners[3] > MAX_FASTENERS[3])
                            return false;
                        break;
                    default:
                        return false;
                }
            }
            if (numFasteners[0] + numFasteners[1] + numFasteners[2] + numFasteners[3] <= 4)
                return true;
            return false;
        case 3: //Number of steps selection
            switch (inputs[5]){
                case '4': case '5': case '6': case '7': case '8':
                    return true;
                default:
                    return false;
            }
        default:
            return false;
    }
}

/*
 * @param inputs: char array with 6 indices
 */
void getInputs (unsigned char * inputs){
    const unsigned char setKeys[] = "BNSW";
    unsigned char keyPressed;
    unsigned short int keyCount, screenPos = 1;
    boolean userTyping;
    unsigned int i; //loop variable

    while (screenPos < 4){
        userTyping = true;
        __lcd_clear();
        keyCount = 0;
        switch (screenPos){
            case 1:
                inputs[0] = '0';
                inputs[1] = '0';
                inputs[2] = '0';
                inputs[3] = '0';
                printf ("SET? HIT 1,2,3,4");
                __lcd_newline();
                printf ("FOR BNSW: ");
                //printf ("SET? 1234={BNSW}");
                break;
            case 2:
                printf ("NUMBER OF SETS");
                __lcd_newline();
                printf ("PER STEP? ");
                break;
            case 3:
                printf ("NUMBER OF STEPS");
                __lcd_newline();
                printf ("IN ASSEMBLY? ");
                break;
            default:
                break;
        }
        //__lcd_newline();
        //printf ("#-CONFIRM ");
        while(userTyping){
            keyPressed = getKeyPressed(false);
            switch(keyPressed){
                case '#':
                    if (keyCount > 0)
                        userTyping = false;
                    continue;
                case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case '0':
                    if ((keyPressed == '1' || keyPressed == '2' || keyPressed == '3' || keyPressed == '4') && screenPos == 1 && keyCount < 4){
                        putch (setKeys[(unsigned int)keyPressed-49]);
                        inputs[keyCount] = setKeys[(unsigned int)keyPressed-49];
                        keyCount++;
                    }
                    else if ((screenPos == 2 || screenPos == 3) && keyCount < 1){
                        putch (keyPressed);
                        inputs[screenPos+2] = keyPressed;
                        keyCount++;
                    }
                    continue;
                case 'D':
                    if (keyCount > 0){
                        __lcd_clear();
                        keyCount--;
                        switch (screenPos){
                            case 1:
                                printf ("SET? HIT 1,2,3,4");
                                __lcd_newline();
                                printf ("FOR BNSW: ");
                                for (i = 0; i<keyCount; i++){
                                    putch (inputs[i]);
                                }
                                break;
                            case 2:
                                printf ("NUMBER OF SETS");
                                __lcd_newline();
                                printf ("PER STEP? ");
                                break;
                            case 3:
                                printf ("NUMBER OF STEPS");
                                __lcd_newline();
                                printf ("IN ASSEMBLY? ");
                                break;
                            default:
                                break;
                        }
                    }
                    break;
                default:
                    continue;  
            }
        }
        if (checkValid (screenPos, inputs)) //if no bad input
            screenPos ++;
        else
            errScreen();
    }
}

/*
 * @param inputs: the 6 inputted characters to be determined, with 0s as terminations for the fastener set
 */
void initStandby(unsigned char * inputs){
    unsigned char keyPressed;
    boolean opNotStarted = true, onOptionScreen;
    
    /* Initialize LCD. */
    initLCD();
    
    /* Initialize I2C Master. */
    initRTC();

    while(opNotStarted){
        /***** Screen 1 *****/
        __lcd_clear();
        __lcd_newline(); //remove later and add real time instead
        printf ("CONTINUE? HIT A");
        while(1){
            if (getKeyPressed(true) == 'A')
                break;
        }

        /***** Screen 2 *****/
        onOptionScreen = true;
        __lcd_clear();
        printf ("CHOOSE A-START,");
        __lcd_newline();
        printf ("B-LOGS, C-BACK");
        while(opNotStarted && onOptionScreen){
            keyPressed = getKeyPressed(false);
            switch(keyPressed){
                case 'A': 
                    opNotStarted = false;
                    onOptionScreen = false;
                    break;
                case 'B':
                    showLogs();
                    onOptionScreen = false;
                    break;
                case 'C':
                    onOptionScreen = false;
                    continue;
                default:
                    continue;
            }
        }
    }
    
    /***** Screens 3,4,5,6 *****/
    getInputs(inputs);
    __lcd_clear();
    printf ("HIT A TO START");
    __lcd_newline();
    printf ("OPERATION!");
    waitA();
    
    /***** Screen 7 - Operation *****/
    __lcd_clear();
    printf ("OPERATING...");
    __lcd_newline();
}

void doneScreen(){
    /***** Screen 8 *****/
    __lcd_clear();
    printf ("OPERATION DONE!");
    __lcd_newline();
    printf ("HIT A");
    waitA();
}

/*
 * @param inputs - char array of user inputs (size 6)
 * @param numRemaining - int array containing # of remaining fasteners of each type
 * @param operationTime - int which stores the total operation time
 */
void showResults(unsigned char * inputs, unsigned short int * numRemaining, unsigned short int operationTime){
    int i; //loop variable
    
    /***** Screen 9 *****/
    __lcd_clear();
    printf ("DONE:%c[", inputs[5]);
    for (i = 0; i<4; i++){
        if (inputs[i] == '0')
            break;
        putch (inputs[i]);
    }
    printf ("x%c]", inputs[4]);
    __lcd_newline();
    printf("TIME:%dS. HIT A", operationTime);
    waitA();
    
    /***** Screen 10 *****/
    __lcd_clear();
    printf ("LEFT: %dB,%dN,", numRemaining[0], numRemaining[1]);
    __lcd_newline();
    printf ("%dS,%dW. HIT A", numRemaining[2], numRemaining[3]);
    waitA();
}

void showLogs(){
    boolean onOptionScreen, haveEntry;
    unsigned char keyPressed;
    //To be retrieved from memory:
    unsigned char timeEnd [7];
    unsigned char inputs [6];
    unsigned short int numRemaining [4]; //# remaining of each fastener type
    unsigned short int operationTime;
    
    onOptionScreen = true;
    __lcd_clear();
    printf ("LOGS NEWEST TO");
    __lcd_newline();
    printf ("OLDEST: A,B,C,D");
    while(onOptionScreen){
        keyPressed = getKeyPressed(false);
        switch (keyPressed){
            case 'A': case 'B': case 'C': case 'D':
                onOptionScreen = false;
                continue;
            default:
                continue;
        }
    }
    
    haveEntry = retrieveResults (inputs, numRemaining, &operationTime, timeEnd, keyPressed-64);
    if (haveEntry)
        showResults (inputs, numRemaining, operationTime); //CHANGE LATER TO INCLUDE REAL TIME END
    else {
        __lcd_clear();
        printf ("NO ENTRY. HIT A");
        waitA();
    }
}