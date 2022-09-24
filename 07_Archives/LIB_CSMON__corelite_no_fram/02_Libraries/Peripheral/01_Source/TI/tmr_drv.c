/* *****************************************************************************
 * File:   tmr_drv.c
 * Author: Dimitar Lilov
 *
 * Created on 2020 01 04 01:00
 * 
 * Description: ...
 * 
 **************************************************************************** */
#ifdef _WIN32
#pragma once
#endif /* _WIN32 */

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "tmr_drv.h"
#include <stdint.h>
//#include "stdextend.h"
#include "boardcfg.h"

#include "driverlib.h"
#include "device.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
//
// Globals
//
uint16_t cpuTimer0IntCount;
uint16_t cpuTimer1IntCount;
uint16_t cpuTimer2IntCount;


volatile uint16_t u16TimerMainPeriod = TIMER_MAIN_USEC;

volatile uint16_t u16SecondPeriod;
volatile uint16_t u16SecondCounter;

volatile uint16_t u16TimerMainISRCounter;


#if BOARDCFG_USE_TIMER_AND_UART_FUNCTIONALITY
/* UART RX Timing and TX Delay */
volatile uint16_t u16TimerMainFunctionDelay[UART_COUNT];
volatile uint16_t u16TimerMainFunctionDelayCompare[UART_COUNT];
volatile uint16_t u16TimerMainFunctionDelayCounter[UART_COUNT];
volatile uint16_t u16TimerMainDelayedData[UART_COUNT];
volatile pfVoid_u8_t pfDelayedFunction_u8[UART_COUNT] = {NULL, NULL, NULL, NULL};
volatile pfVoid_void_t pfDelayedFunction[UART_COUNT] = {NULL, NULL, NULL, NULL};
#endif

volatile pfVoid_void_t pfCSMONFunction = NULL;

volatile uint16_t TMR_u16FreeRunTimerPrescaller = 0;


/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */
void TMR_vInitCPUTimers(void);
void configCPUTimer(uint32_t cpuTimer, float freq, float period);

/* *****************************************************************************
 * Functions
 **************************************************************************** */
void TMR_vInit(void)
{
    //
    // Initializes the Device Peripheral. For this example, only initialize the
    // Cpu Timers.
    //
    TMR_vInitCPUTimers();


    // Reserved for RTOS/ Simulink
    //configCPUTimer(CPUTIMER0_BASE, DEVICE_SYSCLK_FREQ, 1000000);

    // leave with default configuration CPU-Timer 1 (32 Bit Free Rolling on SysClk Timer)
    //configCPUTimer(CPUTIMER1_BASE, DEVICE_SYSCLK_FREQ, 1000000);

    // 100 us default Period (in uSeconds)
    configCPUTimer(CPUTIMER2_BASE, DEVICE_SYSCLK_FREQ, u16TimerMainPeriod);

    //
    // To ensure precise timing, use write-only instructions to write to the
    // entire register. Therefore, if any of the configuration bits are changed
    // in configCPUTimer and initCPUTimers, the below settings must also
    // be updated.
    //
    //CPUTimer_enableInterrupt(CPUTIMER0_BASE);
#if BOARDCFG_FREE_RUN_TIMER_INTERNAL
    CPUTimer_enableInterrupt(CPUTIMER1_BASE);
#endif
    CPUTimer_enableInterrupt(CPUTIMER2_BASE);

    //
    // Starts CPU-Timer 0, CPU-Timer 1, and CPU-Timer 2.
    //
    //CPUTimer_startTimer(CPUTIMER0_BASE);
#if BOARDCFG_FREE_RUN_TIMER_INTERNAL
    CPUTimer_startTimer(CPUTIMER1_BASE);
#endif
    CPUTimer_startTimer(CPUTIMER2_BASE);


}



uint16_t TMR_u16GetFreeRunTimerPrescaller(void)
{
    return TMR_u16FreeRunTimerPrescaller;
}

//
// TMR_vInitCPUTimers - This function initializes all three CPU timers
// to a known state.
//
void TMR_vInitCPUTimers(void)
{
    //
    // Initialize timer period to maximum
    //
    //CPUTimer_setPeriod(CPUTIMER0_BASE, 0xFFFFFFFF);
#if BOARDCFG_FREE_RUN_TIMER_INTERNAL
    CPUTimer_setPeriod(CPUTIMER1_BASE, 0xFFFFFFFF);
#endif
    CPUTimer_setPeriod(CPUTIMER2_BASE, 0xFFFFFFFF);

    //
    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT)
    //
    //CPUTimer_setPreScaler(CPUTIMER0_BASE, 0);
#if BOARDCFG_FREE_RUN_TIMER_INTERNAL
    CPUTimer_setPreScaler(CPUTIMER1_BASE, 0);
#else
    TMR_u16FreeRunTimerPrescaller = (HWREGH(CPUTIMER1_BASE + CPUTIMER_O_TPRH) << 8U) + (HWREGH(CPUTIMER1_BASE + CPUTIMER_O_TPR) & CPUTIMER_TPR_TDDR_M);
#endif
    CPUTimer_setPreScaler(CPUTIMER2_BASE, 0);

    //
    // Make sure timer is stopped
    //
    //CPUTimer_stopTimer(CPUTIMER0_BASE);
#if BOARDCFG_FREE_RUN_TIMER_INTERNAL
    CPUTimer_stopTimer(CPUTIMER1_BASE);
#endif
    CPUTimer_stopTimer(CPUTIMER2_BASE);

    //
    // Reload all counter register with period value
    //
    //CPUTimer_reloadTimerCounter(CPUTIMER0_BASE);
#if BOARDCFG_FREE_RUN_TIMER_INTERNAL
    CPUTimer_reloadTimerCounter(CPUTIMER1_BASE);
#endif
    CPUTimer_reloadTimerCounter(CPUTIMER2_BASE);

    //
    // Reset interrupt counter
    //
    cpuTimer0IntCount = 0;
    cpuTimer1IntCount = 0;
    cpuTimer2IntCount = 0;
}

//
// configCPUTimer - This function initializes the selected timer to the
// period specified by the "freq" and "period" parameters. The "freq" is
// entered as Hz and the period in uSeconds. The timer is held in the stopped
// state after configuration.
//
void configCPUTimer(uint32_t cpuTimer, float freq, float period)
{
    uint32_t temp;

    //
    // Initialize timer period:
    //
    temp = (uint32_t)(freq / 1000000 * period);
    CPUTimer_setPeriod(cpuTimer, temp);

    //
    // Set pre-scale counter to divide by 1 (SYSCLKOUT):
    //
    CPUTimer_setPreScaler(cpuTimer, 0);

    //
    // Initializes timer control register. The timer is stopped, reloaded,
    // free run disabled, and interrupt enabled.
    // Additionally, the free and soft bits are set
    //
    CPUTimer_stopTimer(cpuTimer);
    CPUTimer_reloadTimerCounter(cpuTimer);
    CPUTimer_setEmulationMode(cpuTimer,
                              CPUTIMER_EMULATIONMODE_RUNFREE
                              //CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT
                              );
    CPUTimer_enableInterrupt(cpuTimer);

    //
    // Resets interrupt counters for the three cpuTimers
    //
    if (cpuTimer == CPUTIMER0_BASE)
    {
        cpuTimer0IntCount = 0;
    }
    else if(cpuTimer == CPUTIMER1_BASE)
    {
        cpuTimer1IntCount = 0;
    }
    else if(cpuTimer == CPUTIMER2_BASE)
    {
        cpuTimer2IntCount = 0;
    }
}

#if BOARDCFG_FREE_RUN_TIMER_INTERNAL
//
// TMR1_ISR - Counter for CpuTimer1
//
__interrupt void TMR1_ISR(void)
{
    cpuTimer0IntCount++;

    //
    // Acknowledge this interrupt to receive more interrupts from group 1
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
#endif

volatile uint8_t    hours;
volatile uint8_t    minutes;
volatile uint8_t    seconds;
volatile uint8_t    second_Passed;

//
// TMR2_ISR - Counter for CpuTimer2
//
__interrupt void TMR2_ISR(void)
{
    //
    // The CPU acknowledges the interrupt.
    //
    cpuTimer1IntCount++;


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


#if BOARDCFG_USE_TIMER_AND_UART_FUNCTIONALITY
    /* Uart Functionality */
    uint16_t index;

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
                    UART_PutChar((UART_eModule_t)index,u16TimerMainDelayedData[index]);
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
                    UART_PutChar((UART_eModule_t)index,u16TimerMainDelayedData[index]);
                }
            }
        }
    }
#endif

    if (pfCSMONFunction != NULL)
    {
        pfCSMONFunction();
    }


    /* Toggle LED0 at 1 Hz rate */
    #if USE_DEBUG_LED1
    DEBUG_LED1_LAT ^= 1;
    #endif


}

#if 0
//
// TMR2_ISR - Counter for CpuTimer2
//
__interrupt void TMR2_ISR(void)
{
    //
    // The CPU acknowledges the interrupt.
    //
    cpuTimer2IntCount++;
}
#endif





uint16_t TIMER_vGetTimerMainRollingCounter(void)
{
    return u16TimerMainISRCounter;
}

#if BOARDCFG_USE_TIMER_AND_UART_FUNCTIONALITY
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
#endif

void TIMER_vSetupTimerMainCSMONFunction(pfVoid_void_t pfInput)
{
    pfCSMONFunction = pfInput;
}

void TIMER_u16ResetTimerMainPeriod (uint16_t usec)
{
    u16TimerMainPeriod = usec;
    TMR_vInit();
}


//
// End of File
//
