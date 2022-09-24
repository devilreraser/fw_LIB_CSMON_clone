/* *****************************************************************************
 * File:   adcpwm_drv.c
 * Author: Dimitar Lilov
 *
 * Created on 2020 04 10 22:50
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
#include "adcpwm_drv.h"
#include "sys_drv.h"
#include "system.h"


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

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */
void ADCPWM_DRV_vInit(void)
{
    #if !defined(BOARDCFG_USE_MCC_ADC)

    ADC_vInit();        /* Configure ADC Before PWM */
    
    #else

    ADC1_Initialize();
    ADC2_Initialize();  
    #if 0
    ADC_RegisterFunctionsInISR();
    #endif

    #endif
    
    #ifndef BOARDCFG_USE_MCC_PWM
    PWM_vInit();
    #else
    PWM_Initialize();
    PWM_RegisterFunctionsInISR();
    #endif

    #if !defined(BOARDCFG_USE_MCC_ADC) && (BOARDCFG_USE_OLD_NON_MCC_ADC)
    ADCPWM_vStackModeInit(CLK_DRV_u32GetFcy()); 
    bStack1FreqReset = 0;
    bStack2FreqReset = 0;
    #endif

}


#if !defined(BOARDCFG_USE_MCC_ADC) && (BOARDCFG_USE_OLD_NON_MCC_ADC)
bool ADCPWM_DRV_vConfigurationChangeRequest(void)
{
    return ( (bStack1FreqReset) ||(bStack2FreqReset) );
}
#endif
