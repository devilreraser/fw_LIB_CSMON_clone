/* *****************************************************************************
 * File:   timer.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 04 04 16:37
 * 
 * Description: Timers Initialization
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef TIMER_H
#define	TIMER_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "boardcfg.h"
#include "uart_drv.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Constants Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

typedef void (*pfVoid_void_t)(void);

typedef void (*pfVoid_u8_t)(uint8_t);


/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */
#define u32uSecToTimerTicks(uSec)  (uint32_t)(uSec / TIMER_MAIN_USEC)
#define u16uSecToTimerTicks(uSec)  (uint16_t)(uSec / TIMER_MAIN_USEC)

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 


/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void TIMER_vInit(void);
void TIMER1_vInit( uint32_t u32Fcy  );

void TIMER_vSetupTimerMainFunctionDelay(UART_eModule_t index, uint16_t usec);

void TIMER_vSetupTimerMainDelayedFunction(UART_eModule_t index, pfVoid_void_t pfInput);
void TIMER_vSetupTimerMainDelayedFunctionSendData(UART_eModule_t index, pfVoid_u8_t pfInput);

/* below use reset on rx complete and set compare for delayed transmit */
void TIMER_vResetTimerMainFunctionDelayCounter(UART_eModule_t index);
void TIMER_vSetupTimerMainFunctionDelayCompare(UART_eModule_t index, uint16_t usec);

void TIMER_vSetupTimerMainDelayedData(UART_eModule_t index, uint16_t data);

uint16_t TIMER_vGetTimerMainRollingCounter(void);

void TIMER_u16ResetTimerMainPeriod (uint16_t usec);
    

        
#endif	/* TIMER_H */
