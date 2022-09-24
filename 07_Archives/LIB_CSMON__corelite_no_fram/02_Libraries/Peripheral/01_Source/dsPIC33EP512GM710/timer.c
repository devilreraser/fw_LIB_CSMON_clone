/* *****************************************************************************
 * File:   timer.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 04 04 16:37
 * 
 * Description: Timers Initialization
 * 
 **************************************************************************** */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <xc.h>
#include <stdbool.h>
#include <stddef.h>
#include "boardcfg.h"
#include "sys_drv.h"
#include "timer.h"
#include "uart_drv.h"


/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
volatile uint16_t u16TimerMainPeriod = TIMER_MAIN_USEC;

volatile uint16_t u16SecondPeriod;
volatile uint16_t u16SecondCounter;

volatile uint16_t u16TimerMainISRCounter;



/* UART RX Timing and TX Delay */
volatile uint16_t u16TimerMainFunctionDelay[UART_COUNT];
volatile uint16_t u16TimerMainFunctionDelayCompare[UART_COUNT];
volatile uint16_t u16TimerMainFunctionDelayCounter[UART_COUNT];
volatile uint16_t u16TimerMainDelayedData[UART_COUNT];
volatile pfVoid_u8_t pfDelayedFunction_u8[UART_COUNT] = {NULL, NULL, NULL, NULL};
volatile pfVoid_void_t pfDelayedFunction[UART_COUNT] = {NULL, NULL, NULL, NULL};

volatile uint8_t    hours;
volatile uint8_t    minutes;
volatile uint8_t    seconds;
volatile uint8_t    second_Passed;




/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */


/* *****************************************************************************
 * Functions
 **************************************************************************** */

void TIMER_vInit(void)
{
    T9CONbits.TON = 0; // Disable Timer
    T8CONbits.TON = 0; // Disable Timer
    
    T8CONbits.TSIDL = 1; // Stop Timer in Debug Break
    T8CONbits.T32 = 1; // Enable 32-bit Timer mode
    T8CONbits.TCS = 0; // Select internal instruction cycle clock
    T8CONbits.TGATE = 0; // Disable Gated Timer mode
    T8CONbits.TCKPS = 0b00; // Select 1:1 Prescaler
    TMR9 = 0x00; // Clear 32-bit Timer (msw)
    TMR8 = 0x00; // Clear 32-bit Timer (lsw)
    PR9 = 0xFFFF; // Load 32-bit period value (msw)
    PR8 = 0xFFFF; // Load 32-bit period value (lsw)
    IPC13bits.T9IP = ISR_PRIORITY_TMR9; // Set Timer9 Interrupt Priority Level
    IFS3bits.T9IF = 0; // Clear Timer9 Interrupt Flag
    IEC3bits.T9IE = 0; // Enable/Disable Timer9 interrupt
    T8CONbits.TON = 1; // Start 32-bit Timer
}

/* Example code for Timer9 ISR*/
void __attribute__((__interrupt__, auto_psv)) _T9Interrupt(void)
{
    /* Interrupt Service Routine code goes here */
    IFS3bits.T9IF = 0; //Clear Timer9 interrupt flag
}


/******************************************************************************
 * Function:        TIMER1_vInit()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is used to configure Timer1.
 *****************************************************************************/
void TIMER1_vInit( uint32_t u32Fcy  )
{
    uint64_t ticks;
    
    T1CON = 0;          // Timer reset
    IFS0bits.T1IF = 0;  // Reset Timer1 interrupt flag
    IPC0bits.T1IP = ISR_PRIORITY_TMR1;  // Timer1 Interrupt priority level=4
    IEC0bits.T1IE = 1;  // Enable Timer1 interrupt
    
    ticks = (u32Fcy * u16TimerMainPeriod + 500000)/1000000 - 1;
    
    u16SecondPeriod = (u32Fcy + ((uint16_t)ticks >> 1))/ticks;
    
    PR1 = (uint16_t)ticks;       // Timer1 period register 
    T1CONbits.TCS = 0; // Select internal instruction cycle clock
    //EnSecOsc();         // no Enable Secondary Osc
    T1CONbits.TON = 1;  // Enable Timer1 and start the counter
}



void TMR1_CallBack(void)
{
    uint16_t index;
    
    DEBUG_SET_TMR_1();


    u16SecondCounter++;
    if (u16SecondCounter >= u16SecondPeriod)
    {
        u16SecondCounter = 0;
        
        if( seconds < 59 )          // is cummulative seconds < 59?
        {
            seconds++;              // yes, so increment seconds
        }
        else                        // else seconds => 59
        {
            seconds = 0x00;         // reset seconds
            if( minutes < 59 )      // is cummulative minutes < 59?
            {
                minutes++;          // yes, so updates minutes
            }
            else                    // else minutes => 59
            {
                minutes = 0x00;     // reset minutes
                if( hours < 23 )    // is cummulative hours < 23
                {
                    hours++;        // yes, so update hours
                }
                else
                {
                    hours = 0x00;   // reset time
                }
            }
        }

        /* set flag to update each second */
        second_Passed = 1;
    }
    
    u16TimerMainISRCounter++;
    
    
    
    /* Uart Functionality */
    
    for (index = 0; index < UART_COUNT; index++)
    {
        if (u16TimerMainFunctionDelay[index] > 0)
        {
            u16TimerMainFunctionDelay[index]--;
            if (u16TimerMainFunctionDelay[index] == 0)
            {
                if (pfDelayedFunction_u8[index] != NULL)
                {
                    pfDelayedFunction_u8[index](u16TimerMainDelayedData[index]);
                }
                else
                {
                    if (pfDelayedFunction[index] != NULL)
                    {
                        pfDelayedFunction[index]();
                    }
                    //do {U1TXREG = u16TimerMainDelayedData[index]; } while(0);
                    UART_PutChar(index,u16TimerMainDelayedData[index]);
                }
            }
        }

        u16TimerMainFunctionDelayCounter[index]++;
        if (u16TimerMainFunctionDelayCompare[index] > 0)
        {
            if (u16TimerMainFunctionDelayCounter[index] >= u16TimerMainFunctionDelayCompare[index])
            {
                u16TimerMainFunctionDelayCompare[index] = 0;
                if (pfDelayedFunction_u8[index] != NULL)
                {
                    pfDelayedFunction_u8[index](u16TimerMainDelayedData[index]);
                }
                else
                {
                    if (pfDelayedFunction[index] != NULL)
                    {
                        pfDelayedFunction[index]();
                    }
                    //do {U1TXREG = u16TimerMainDelayedData[index]; } while(0);
                    UART_PutChar(index,u16TimerMainDelayedData[index]);
                }
            }
        }
    }

    DEBUG_CLR_TMR_1();
}

#ifndef BOARDCFG_USE_MCC_TMR
/******************************************************************************
 * Function:        _T1Interrupt
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Timer1 Interrupt Handler.
 *****************************************************************************/
void __attribute__ ( (interrupt, auto_psv) ) _T1Interrupt( void )
{
    TMR1_CallBack();
    /* reset Timer 1 interrupt flag */
    IFS0bits.T1IF = 0;
}
#endif


uint16_t TIMER_vGetTimerMainRollingCounter(void)
{
    return u16TimerMainISRCounter;
}

void TIMER_vSetupTimerMainFunctionDelay(UART_eModule_t index, uint16_t usec)
{
    u16TimerMainFunctionDelay[index] = (usec + (u16TimerMainPeriod>>1))/u16TimerMainPeriod;
}

void TIMER_vSetupTimerMainDelayedFunctionSendData(UART_eModule_t index, pfVoid_u8_t pfInput)
{
    pfDelayedFunction_u8[index] = pfInput;
}


void TIMER_vSetupTimerMainDelayedFunction(UART_eModule_t index, pfVoid_void_t pfInput)
{
    pfDelayedFunction[index] = pfInput;
}

void TIMER_vResetTimerMainFunctionDelayCounter(UART_eModule_t index)
{
    if (u16TimerMainFunctionDelayCompare[index] == 0)
    {
        u16TimerMainFunctionDelayCounter[index] = 0;
    }
}
void TIMER_vSetupTimerMainFunctionDelayCompare(UART_eModule_t index, uint16_t usec)
{
    u16TimerMainFunctionDelayCompare[index] = (usec + (u16TimerMainPeriod>>1))/u16TimerMainPeriod;
}

void TIMER_vSetupTimerMainDelayedData(UART_eModule_t index, uint16_t data)
{
    u16TimerMainDelayedData[index] = data;
}

void TIMER_u16ResetTimerMainPeriod (uint16_t usec)
{
    u16TimerMainPeriod = usec;
    TIMER1_vInit(CLK_DRV_u32GetFcy());      //to do initialization of the timer period for csmon test application in texas as made for delfino and enerdrive not fully checked
}
