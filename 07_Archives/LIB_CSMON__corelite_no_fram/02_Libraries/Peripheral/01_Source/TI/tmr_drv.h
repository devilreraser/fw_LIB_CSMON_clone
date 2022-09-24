/* *****************************************************************************
 * File:   tmr_drv.h
 * Author: Dimitar Lilov
 *
 * Created on 2020 01 04 01:00
 * 
 * Description: ...
 * 
 **************************************************************************** */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Guard condition file contents not included more than once */  
/* #pragma once */
#ifndef TMR_DRV_H
#define	TMR_DRV_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdint.h>
#include "stdextend.h"

#include "boardcfg.h"
#if BOARDCFG_USE_TIMER_AND_UART_FUNCTIONALITY
#include "uart_drv.h"
#endif


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


void TMR_vInit(void);

uint16_t TMR_u16GetFreeRunTimerPrescaller(void);

//__interrupt void TMR0_ISR(void);
#if BOARDCFG_FREE_RUN_TIMER_INTERNAL
__interrupt void TMR1_ISR(void);
#endif
__interrupt void TMR2_ISR(void);

#if BOARDCFG_USE_TIMER_AND_UART_FUNCTIONALITY
void TIMER_vSetupTimerMainFunctionDelay(UART_eModule_t index, uint16_t usec);

void TIMER_vSetupTimerMainDelayedFunction(UART_eModule_t index, pfVoid_void_t pfInput);
void TIMER_vSetupTimerMainDelayedFunctionSendData(UART_eModule_t index, pfVoid_u8_t pfInput);

/* below use reset on rx complete and set compare for delayed transmit */
void TIMER_vResetTimerMainFunctionDelayCounter(UART_eModule_t index);
void TIMER_vSetupTimerMainFunctionDelayCompare(UART_eModule_t index, uint16_t usec);

void TIMER_vSetupTimerMainDelayedData(UART_eModule_t index, uint16_t data);
#endif

uint16_t TIMER_vGetTimerMainRollingCounter(void);

void TIMER_vSetupTimerMainCSMONFunction(pfVoid_void_t pfInput);
void TIMER_u16ResetTimerMainPeriod (uint16_t usec);

#endif	/* TMR_DRV_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
