/* *****************************************************************************
 * File:   incap.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 04 30 16:31
 * 
 * Description: Input Capture Initialization
 * 
 **************************************************************************** */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <xc.h>
#include <stdbool.h>
#include "boardcfg.h"

#if USE_INPUT_CAPTURE

#include "incap.h"
#include "clk_drv.h"
#include "uart_drv.h"


/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define USE_DMA     1   /* DMA Data Reading works */
#define USE_TIMER_COUNTER_FOR_BUFFER_POSITION   1

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
typedef enum
{
    INCAP_IDLE,
    INCAP_START,
    INCAP_COLLECT,
    INCAP_DONE,
    INCAP_STOP,
     
    INCAP_COUNT
}INCAP_eState_t;





/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
//Define Buffer in DMA RAM as global variable:
#define DMA_BUFFER_LENGTH   128
__eds__ unsigned int BufferA[DMA_BUFFER_LENGTH*2] __attribute__((space(eds)));
__eds__ unsigned int BufferB[DMA_BUFFER_LENGTH*2] __attribute__((space(eds)));
uint16_t u16CounterIC1;
uint16_t u16CounterIC2;


#define INCAP_ON_START  INCAP_IDLE  // INCAP_START // 

INCAP_eState_t INCAP_eState = INCAP_IDLE;
INCAP_eState_t INCAP_eStateReq = INCAP_IDLE;

uint16_t u16CounterI2CStartProcess = 0;

bool bJustStarted;


/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */
bool INCAP_vReset(void);
void INCAP_vStop(void);


/* *****************************************************************************
 * Functions
 **************************************************************************** */

/* *****************************************************************************
 * INCAP_vInit
 * 
 * Description: Initialize Input Capture Module
 **************************************************************************** */
void INCAP_vInit(UART_eModule_t eUartIndex)
{
	__builtin_write_OSCCONL(OSCCON & (~(1<<6))); // clear bit 6 
    //RPOR13bits.RP113R = 1;    //RP113/RG0 as U1TX
    //RPINR18bits.U1RXR = 31; //RPI31/RA15 as U1RX
    if (eUartIndex == UART_1)
    {
        RPINR7bits.IC1R = RPINR18bits.U1RXR;    /* IC1 same as U1RX */
        RPINR7bits.IC2R = RPINR18bits.U1RXR;    /* IC2 same as U1RX */    
        #if USE_TIMER_COUNTER_FOR_BUFFER_POSITION
        RPINR5bits.T7CKR = RPINR18bits.U1RXR;    /* T7CK same as U1RX */
        #endif
    }
    else if (eUartIndex == UART_2)
    {
        RPINR7bits.IC1R = RPINR19bits.U2RXR;    /* IC1 same as U2RX */
        RPINR7bits.IC2R = RPINR19bits.U2RXR;    /* IC2 same as U2RX */    
        #if USE_TIMER_COUNTER_FOR_BUFFER_POSITION
        RPINR5bits.T7CKR = RPINR19bits.U2RXR;    /* T7CK same as U2RX */
        #endif
    }
    else if (eUartIndex == UART_3)
    {
        RPINR7bits.IC1R = RPINR27bits.U3RXR;    /* IC1 same as U3RX */
        RPINR7bits.IC2R = RPINR27bits.U3RXR;    /* IC2 same as U3RX */    
        #if USE_TIMER_COUNTER_FOR_BUFFER_POSITION
        RPINR5bits.T7CKR = RPINR27bits.U3RXR;    /* T7CK same as U3RX */
        #endif
    }
    else if (eUartIndex == UART_4)
    {
        RPINR7bits.IC1R = RPINR28bits.U4RXR;    /* IC1 same as U4RX */
        RPINR7bits.IC2R = RPINR28bits.U4RXR;    /* IC2 same as U4RX */    
        #if USE_TIMER_COUNTER_FOR_BUFFER_POSITION
        RPINR5bits.T7CKR = RPINR28bits.U4RXR;    /* T7CK same as U4RX */
        #endif
    }
    
    
	__builtin_write_OSCCONL(OSCCON | (1<<6)); 	 // Set bit 6 	
    
    (void)INCAP_vReset();
    
    

}
/* *****************************************************************************
 * INCAP_vReset
 * 
 * Description: Reset Input Capture Module
 **************************************************************************** */
bool INCAP_vReset(void)
{
    bool initOK = 0;
    uint16_t countFail = 0;
    
    IC1CON1bits.ICM = 0b000; // Disable Input Capture 1 module
    IC2CON1bits.ICM = 0b000; // Disable Input Capture 2 module



    IC2CON2bits.ICTRIG = 1; // trigger mode
    IC2CON1bits.ICI = 0b00; // Interrupt on every capture event
    IC2CON1bits.ICTSEL = 7; // Select system clock as the IC2 Time base
    IC2CON2bits.SYNCSEL = 0b10000; // IC1 is the trigger or sync
    IC2CON2bits.IC32 = 1; // 32 bit mode
    
    IC1CON1bits.ICTSEL = 7; // Select system clock as the IC1 Time base
    IC1CON1bits.ICI = 0b00; // Interrupt on every capture event
    IC1CON2bits.IC32 = 1; // 32 bit mode
    IC1CON2bits.SYNCSEL = 0; // no sync select for this capture - main capture


    
#if USE_DMA
    //Set up DMA for Input Capture:
    DMA4CONbits.AMODE = 0b00; // Register indirect with post increment
    DMA4CONbits.MODE = 0b10; // Continuous, Ping-Pong modes are enabled
    DMA4CONbits.DIR = 0; // Peripheral to RAM
    DMA4PAD = (int)&IC1BUF; // Address of the capture buffer register
    DMA4REQ = 1; // Select IC1 module as DMA request source
    DMA4CNT = DMA_BUFFER_LENGTH-1; // Number of words to buffer
    DMA4STAH = __builtin_dmapage (&BufferA);
    DMA4STAL = __builtin_dmaoffset (&BufferA);
    DMA4STBH = __builtin_dmapage (&BufferA[DMA_BUFFER_LENGTH]);
    DMA4STBL = __builtin_dmaoffset (&BufferA[DMA_BUFFER_LENGTH]);
    
    //Set up DMA for Input Capture:
    DMA5CONbits.AMODE = 0b00; // Register indirect with post increment
    DMA5CONbits.MODE = 0b10; // Continuous, Ping-Pong modes are enabled
    DMA5CONbits.DIR = 0; // Peripheral to RAM
    DMA5PAD = (int)&IC2BUF; // Address of the capture buffer register
    DMA5REQ = 5; // Select IC2 module as DMA request source
    DMA5CNT = DMA_BUFFER_LENGTH-1; // Number of words to buffer
    DMA5STAH = __builtin_dmapage (&BufferB);
    DMA5STAL = __builtin_dmaoffset (&BufferB);
    DMA5STBH = __builtin_dmapage (&BufferB[DMA_BUFFER_LENGTH]);
    DMA5STBL = __builtin_dmaoffset (&BufferB[DMA_BUFFER_LENGTH]);
    
#if 1
    //Set up Timer Counter to get buffer fill position:
    T7CONbits.TON = 0; // Disable Timer
    T7CONbits.TCS = 1; // Select external clock source
    //T7CONbits.TSYNC = 1; // Enable Synchronization always sync
    T7CONbits.TCKPS = 0b00; // Select 1:1 Prescaler
    PR7 = (sizeof(BufferA)/4)-1; // Load the period value
    IPC12bits.T7IP = ISR_PRIORITY_TMR7; // Set Timer 7 Interrupt Priority Level
    TMR7 = 0x00; // Clear timer register Type C Timer start counting on the first rising edge
    IFS3bits.T7IF = 0; // Clear Timer 7 Interrupt Flag
    IEC3bits.T7IE = 0; // Enable Timer7 interrupt
    
#else   /* not implemented or tested */    
    //Set up DMA to get buffer fill position:
    DMA6CONbits.AMODE = 0b00; // Register indirect with post increment
    DMA6CONbits.MODE = 0b10; // Continuous, Ping-Pong modes are enabled
    DMA6CONbits.DIR = 0; // Peripheral to RAM
    DMA6PAD = (int)&IC2BUF; // Address of the capture buffer register
    DMA6REQ = 5; // Select IC2 module as DMA request source
    DMA6CNT = DMA_BUFFER_LENGTH-1; // Number of words to buffer
    DMA6STAH = __builtin_dmapage (&BufferB);
    DMA6STAL = __builtin_dmaoffset (&BufferB);
    DMA6STBH = __builtin_dmapage (&BufferB[DMA_BUFFER_LENGTH]);
    DMA6STBL = __builtin_dmaoffset (&BufferB[DMA_BUFFER_LENGTH]);
#endif
    
    
    IFS2bits.DMA4IF = 0; // Clear the DMA interrupt flag bit
    IEC2bits.DMA4IE = 0; // Enable DMA interrupt enable bit
    IFS3bits.DMA5IF = 0; // Clear the DMA interrupt flag bit
    IEC3bits.DMA5IE = 0; // Enable DMA interrupt enable bit
    DMA4CONbits.CHEN = 1;
    DMA5CONbits.CHEN = 1;
#endif
    
#if USE_TIMER_COUNTER_FOR_BUFFER_POSITION == 0
    u16CounterIC1 = 0;
    u16CounterIC2 = 0;
#endif
    IFS0bits.IC1IF = 0; // Clear the IC1 interrupt status flag
    IEC0bits.IC1IE = 0; // Enable IC1 interrupts
    IPC0bits.IC1IP = ISR_PRIORITY_INCAP1; // Set module interrupt priority
    
    IFS0bits.IC2IF = 0; // Clear the IC2 interrupt status flag
    IEC0bits.IC2IE = 0; // Enable IC2 interrupts
    IPC1bits.IC2IP = ISR_PRIORITY_INCAP1; // Set module interrupt priority
    

#define ASSURE_START_WITH_HIGH_LEVEL 1
    
#if ASSURE_START_WITH_HIGH_LEVEL 
    /* assure that will start with High Level */
    do
    {
        IC1CON1bits.ICM = 0b000; // Disable Input Capture 1 module
        IC2CON1bits.ICM = 0b000; // Disable Input Capture 2 module
        DMA4CONbits.CHEN = 0;
        DMA5CONbits.CHEN = 0;
        
#if USE_TIMER_COUNTER_FOR_BUFFER_POSITION
        T7CONbits.TON = 0; // Disable Timer
        TMR7 = 0x00; // Clear timer register Type C Timer start counting on the first rising edge
        IFS3bits.T7IF = 0; // Clear Timer 1 Interrupt Flag
        //IEC3bits.T7IE = 0; // Enable Timer1 interrupt
#endif
        if (UART1_RX_PIN == 1)
        {
            #if USE_TIMER_COUNTER_FOR_BUFFER_POSITION
            T7CONbits.TON = 1; // Enable Timer
            #endif
            DMA4CONbits.CHEN = 1;
            DMA5CONbits.CHEN = 1;
            IC2CON1bits.ICM = 0b001; // Generate capture event on every edge
            IC1CON1bits.ICM = 0b001; // Generate capture event on every edge
            
            if (UART1_RX_PIN == 1)
            {
                initOK = 1;
            }
        }
        countFail++;
    }while ((initOK == 0)&&(countFail < 100));     /* assure small delay */
#else
            #if USE_TIMER_COUNTER_FOR_BUFFER_POSITION
            T7CONbits.TON = 1; // Enable Timer
            #endif
            IC2CON1bits.ICM = 0b001; // Generate capture event on every edge
            IC1CON1bits.ICM = 0b001; // Generate capture event on every edge
            initOK = 1;
#endif
    return initOK;
}
/* *****************************************************************************
 * INCAP_vStop
 * 
 * Description: Stop Input Capture Module
 **************************************************************************** */
void INCAP_vStop(void)
{
    IC1CON1bits.ICM = 0b000; // Disable Input Capture 1 module
    IC2CON1bits.ICM = 0b000; // Disable Input Capture 2 module    
}




/* *****************************************************************************
 * _T7Interrupt
 * 
 * Description: Set up Timer7 Interrupt Handler
 **************************************************************************** */
void __attribute__((__interrupt__, auto_psv)) _T7Interrupt(void)
{
    /* Interrupt Service Routine code goes here */
    IFS3bits.T7IF = 0; //Clear Timer7 interrupt flag
}




/* *****************************************************************************
 * _IC1Interrupt
 * 
 * Description: Set up IC1 Interrupt Handler
 **************************************************************************** */
void __attribute__ ((__interrupt__, auto_psv)) _IC1Interrupt(void)
{
#if USE_DMA == 0
    BufferA[u16CounterIC1] = IC1BUF;    
#endif
    
#if USE_TIMER_COUNTER_FOR_BUFFER_POSITION == 0
    u16CounterIC1++;
    if (u16CounterIC1 >= (sizeof(BufferA)/2))
    {
        u16CounterIC1 = 0;
    }
#endif
    IFS0bits.IC1IF = 0; // Reset respective interrupt flag
}
/* *****************************************************************************
 * _IC1Interrupt
 * 
 * Description: Set up IC1 Interrupt Handler
 **************************************************************************** */
void __attribute__ ((__interrupt__, auto_psv)) _IC2Interrupt(void)
{
#if USE_DMA == 0
    BufferB[u16CounterIC2] = IC2BUF;    
#endif

#if USE_TIMER_COUNTER_FOR_BUFFER_POSITION == 0
    u16CounterIC2++;
    if (u16CounterIC2 >= (sizeof(BufferB)/2))
    {
        u16CounterIC2 = 0;
    }
#endif
    IFS0bits.IC2IF = 0; // Reset respective interrupt flag
}
/* *****************************************************************************
 * _DMA4Interrupt
 * 
 * Description: Set up DMA Interrupt Handler
 **************************************************************************** */
void __attribute__((__interrupt__, auto_psv)) _DMA4Interrupt(void)
{
    // Process the captured values
    IFS2bits.DMA4IF = 0; // Clear the DMA4 Interrupt Flag
}


/* *****************************************************************************
 * _DMA5Interrupt
 * 
 * Description: Set up DMA Interrupt Handler
 **************************************************************************** */
void __attribute__((__interrupt__, auto_psv)) _DMA5Interrupt(void)
{
    // Process the captured values
    IFS3bits.DMA5IF = 0; // Clear the DMA5 Interrupt Flag
}



uint32_t u32TimeDelay[DMA_BUFFER_LENGTH*2];
uint32_t u32TimeDelayLast = 0;
uint16_t u16TimeDelayProcessedCounter;


bool bDetected9 = 0;
bool bDetected8 = 0;
bool bDetected10 = 0;
uint32_t u32DetectedBaud9 = 0;
uint32_t u32DetectedBaud8 = 0;
uint32_t u32DetectedBaud10 = 0;

uint16_t u16DetectedBaudTicks9 = 0;
uint16_t u16DetectedBaudTicks8 = 0;
uint16_t u16DetectedBaudTicks10 = 0;
uint16_t u16DetectedBaudRegister9 = 0;
uint16_t u16DetectedBaudRegister8 = 0;
uint16_t u16DetectedBaudRegister10 = 0;

    uint16_t u16IndexStart;
    uint16_t u16IndexEnd;
    volatile uint16_t u16IndexLoop;
    uint16_t u16CountLoop;
    
    uint32_t u32Delay8;   /* 8 bits (+ 1 start + 1 stop) */
    uint32_t u32Delay9;   /* 9 bits */
    uint32_t u32Delay10;   /* 10 bits */
    uint32_t u32DelayError8;   
    uint32_t u32DelayError9;   
    uint32_t u32DelayError10;   
    uint32_t u32DataCurrent;
    uint32_t u32DataDelay;
    uint32_t u32DelayDiff;
        uint32_t u32MinCurrent;
        uint32_t u32MaxCurrent;
    
     
    
void INCAP_vProcessToggleData(
        uint16_t u16IndexFoundStart,
        uint16_t u16Count,
        uint32_t u32Min,
        uint32_t u32Max
        )
{
   if (u32Min > (uint32_t)(0xFFFFFFFF/10))
    {
        return;
    }
   
   u32MinCurrent = u32Min;
   u32MaxCurrent = u32Max;

    u32Delay8 = u32MinCurrent * 10; /* 8N1 -> 10 baud */
    u32Delay9 = u32MinCurrent * 11; /* 8P1 -> 11 baud  8N2(2 stop bits no parity)  */
    u32Delay10 = u32MinCurrent * 12; /* 8P2 -> 12 baud  8P2(2 stop bits with parity)  */
    
    u32DelayError8 = u32Delay8 >> 6;    /* assure less than 1.56% error */
    if (u32DelayError8 == 0)
    {
        u32DelayError8 = 1;  /* at least one tick error consider to be available */
    }
    
    u32DelayError9 = u32Delay9 >> 6;    /* assure less than 1.56% error */
    if (u32DelayError9 == 0)
    {
        u32DelayError9 = 1;  /* at least one tick error consider to be available */
    }
    
    u32DelayError10 = u32Delay10 >> 6;    /* assure less than 1.56% error */
    if (u32DelayError10 == 0)
    {
        u32DelayError10 = 1;  /* at least one tick error consider to be available */
    }
    
    
    
    
    u16IndexStart = u16IndexFoundStart;
    u16IndexEnd = u16IndexStart + u16Count;
    if (u16IndexEnd >= (sizeof(BufferA)/2))
    {
        u16IndexEnd -= (sizeof(BufferA)/2);
    }
    
    while (u16IndexStart != u16IndexEnd)
    {
        u16IndexLoop = u16IndexStart;
        u16CountLoop = 0;
        u32DataDelay = 0;
        
        while (u16IndexLoop != u16IndexEnd)
        {
            u32DataCurrent = u32TimeDelay[u16IndexLoop];
            u32DataDelay += u32DataCurrent;
            u16CountLoop++;
            if ((u16CountLoop & 1) == 0)    /* even count pieces time detection of concatenated bytes (start one byte till start another byte) note: stop bits matters */
            //if ((u16CountLoop & 1) == 1)    /* odd count time pieces needed for even count edges time delay */
            {
                /* even toggles - possible byte */
                if (u32Delay8 >= u32DataDelay)
                {
                    u32DelayDiff = u32Delay8 - u32DataDelay;
                }
                else
                {
                    u32DelayDiff = u32DataDelay - u32Delay8;
                }
                
                if (u32DelayDiff <= u32DelayError8)
                {
                    //found byte 8
                    bDetected8 = 1;
                    u16DetectedBaudTicks8 = (u32DataDelay*2 + (10))/(10*2);     /* trunk after add 0.5 */
                    u16DetectedBaudRegister8 = ((u16DetectedBaudTicks8 + 2)/4) - 1;
                    u32DetectedBaud8 = CLK_DRV_u32GetFcy()/((u16DetectedBaudRegister8+1)*4);
                    u16CounterI2CStartProcess = u16IndexLoop+1;  
                    if (u16CounterI2CStartProcess >= (sizeof(BufferA)/2))
                    {
                        u16CounterI2CStartProcess = 0;
                    }
                }
                //else consider use or not
                {
                    if (u32Delay9 >= u32DataDelay)
                    {
                        u32DelayDiff = u32Delay9 - u32DataDelay;
                    }
                    else
                    {
                        u32DelayDiff = u32DataDelay - u32Delay9;
                    }
                
                    if (u32DelayDiff <= u32DelayError9)
                    {
                        //found byte 9
                        bDetected9 = 1;
                        u16DetectedBaudTicks9 = (u32DataDelay*2 + (11))/(11*2);   /* trunk after add 0.5 */
                        u16DetectedBaudRegister9 = ((u16DetectedBaudTicks9 + 2)/4) - 1;
                        u32DetectedBaud9 = CLK_DRV_u32GetFcy()/((u16DetectedBaudRegister9+1)*4);
                        u16CounterI2CStartProcess = u16IndexLoop+1; 
                        if (u16CounterI2CStartProcess >= (sizeof(BufferA)/2))
                        {
                            u16CounterI2CStartProcess = 0;
                        }
                    }
                    //else consider use or not
                    {
                        if (u32Delay10 >= u32DataDelay)
                        {
                            u32DelayDiff = u32Delay10 - u32DataDelay;
                        }
                        else
                        {
                            u32DelayDiff = u32DataDelay - u32Delay10;
                        }

                        if (u32DelayDiff <= u32DelayError10)
                        {
                            //found byte 10
                            bDetected10 = 1;
                            u16DetectedBaudTicks10 = (u32DataDelay*2 + (12))/(12*2);   /* trunk after add 0.5 */
                            u16DetectedBaudRegister10 = ((u16DetectedBaudTicks10 + 2)/4) - 1;
                            u32DetectedBaud10 = CLK_DRV_u32GetFcy()/((u16DetectedBaudRegister10+1)*4);
                            u16CounterI2CStartProcess = u16IndexLoop+1; 
                            if (u16CounterI2CStartProcess >= (sizeof(BufferA)/2))
                            {
                                u16CounterI2CStartProcess = 0;
                            }
                        }

                    }
                
                }


            }
            u16IndexLoop++;
            if (u16IndexLoop >= (sizeof(BufferA)/2))
            {
                u16IndexLoop = 0;
            }
        }
        u16IndexStart++;
        if (u16IndexStart >= (sizeof(BufferA)/2))
        {
            u16IndexStart = 0;
        }
    }
    
    
    
    
    
}




/* *****************************************************************************
 * INCAP_vProcess
 * 
 * Description: Analyze Captured Data
 **************************************************************************** */
void INCAP_vProcess(void)
{
    uint16_t u16Index;
    uint16_t u16CounterLast;
    uint16_t u16CountTogglesFound;
    //uint16_t u16CountBytesFound;
    uint32_t u32TimeDelayLastNew;
    
    
    
    if (INCAP_eState == INCAP_COLLECT)
    {
        /* get last fully filled value position */
#if USE_TIMER_COUNTER_FOR_BUFFER_POSITION == 0
        u16CounterLast = u16CounterIC1;
        if ((int16_t)(u16CounterIC1 - u16CounterIC2) > (int16_t)(u16CounterIC2 - u16CounterIC1))
        {
            u16CounterLast = u16CounterIC2;
        }
#else
        u16CounterLast = TMR7*2;
        //code below not needed - made in PR1
        //if (u16CounterLast >= (sizeof(BufferA)/2))
        //{
        //    u16CounterLast -= (sizeof(BufferA)/2);
        //}
#endif
        
        /* Fill Time Delay Buffer */
        while (u16TimeDelayProcessedCounter != u16CounterLast)
        {
            u32TimeDelay[u16TimeDelayProcessedCounter] = ((uint32_t)BufferB[u16TimeDelayProcessedCounter] << 16);
            u32TimeDelay[u16TimeDelayProcessedCounter] += BufferA[u16TimeDelayProcessedCounter];
            u32TimeDelayLastNew = u32TimeDelay[u16TimeDelayProcessedCounter];
            u32TimeDelay[u16TimeDelayProcessedCounter] -= u32TimeDelayLast;
                    
            u32TimeDelayLast = u32TimeDelayLastNew;
            u16TimeDelayProcessedCounter++;
            if (u16TimeDelayProcessedCounter >= (sizeof(BufferA)/2))
            {
                u16TimeDelayProcessedCounter = 0;
            }
            
            bJustStarted = 0;
        }
        
        
        u16CountTogglesFound = u16CounterLast - u16CounterI2CStartProcess;
        
        if (u16CountTogglesFound >= 8)   /* max 4 bytes (0) */
        {
            uint32_t u32Min = 0xFFFFFFFF;
            uint32_t u32Max = 0;
            uint32_t u32Data = 0;
            uint32_t u32ByteDelay = 0;
            uint32_t u32DataPrev = 0;
            bool bFoundStart = 0;
            uint16_t u16IndexFoundStart = 0;
            uint16_t u16TogglesFromFoundStart = 0;
            
            u16Index = u16CounterI2CStartProcess;
            while (u16Index != u16CounterLast)
            {
                u32Data = u32TimeDelay[u16Index];
                
                /* find end message (byte and log timeout after it) */
                if (u32Min <= (uint32_t)(0xFFFFFFFF/12))
                {
                    if (u32Data > (u32Data*12))
                    {
                        if (bFoundStart)
                        {
                            INCAP_vProcessToggleData(u16IndexFoundStart, u16TogglesFromFoundStart,u32Min,u32Max);
                        }
                    }
                }
                
                
                
                /* find start bit -> 12 = 8data+parity+start+2stop */
                //if (bFoundStart == 0)
                {
                    if (u32Data <= (uint32_t)(0xFFFFFFFF/12))
                    {
                        if (u32DataPrev > (u32Data*12))
                        {
                            if (bFoundStart)
                            {
                                //already found start - process data till here
                                INCAP_vProcessToggleData(u16IndexFoundStart, u16TogglesFromFoundStart-1,u32Min,u32Max);
                            }
                            
                            bFoundStart = 1;
                            u32Min = u32Data;
                            u32Max = u32Data;
                            //u32ByteDelay = u32Data;
                            u32ByteDelay = 0;
                            //u32DataPrev = u32Data;
                            u16IndexFoundStart = u16Index;
                            //u16TogglesFromFoundStart = 1;
                            u16TogglesFromFoundStart = 0;
                        }
                    }
                }
                //else
                {      
                    if (u32Min > u32Data)
                    {
                        u32Min = u32Data;
                    }
                    if (u32Max < u32Data)
                    {
                        u32Max = u32Data;
                    }
                    u32ByteDelay += u32Data;
                    
                    
                    u16TogglesFromFoundStart++;
                    
                    if ((u16TogglesFromFoundStart & 1) == 0)
                    {
                        /* even toggles - possible byte */
                        
                      
                        
                    }
                }
                
                u32DataPrev = u32Data;
                u16Index++;
                if (u16Index >= (sizeof(BufferA)/2))
                {
                    u16Index = 0;
                }

            }
 
            if (bFoundStart)
            {
                INCAP_vProcessToggleData(u16IndexFoundStart, u16TogglesFromFoundStart,u32Min,u32Max);
            }

            
            
            
            
        }
        
        
        
        
        if (INCAP_eStateReq == INCAP_STOP)
        {
            INCAP_vStop();
            INCAP_eState = INCAP_IDLE;
        }
    }
    else
    {
        if (INCAP_eStateReq == INCAP_START)
        {
            if (INCAP_vReset())
            {
                INCAP_eState = INCAP_COLLECT;
                bJustStarted = 1;
                u32TimeDelayLast = 0;
                u16TimeDelayProcessedCounter = 0;
                u16CounterI2CStartProcess = 0;

                bDetected8 = 0;
                bDetected9 = 0;
                bDetected10 = 0;
                u32DetectedBaud8 = 0;
                u32DetectedBaud9 = 0;
                u32DetectedBaud10 = 0;

            }
        }
    }
}

#endif /* USE_INPUT_CAPTURE */
