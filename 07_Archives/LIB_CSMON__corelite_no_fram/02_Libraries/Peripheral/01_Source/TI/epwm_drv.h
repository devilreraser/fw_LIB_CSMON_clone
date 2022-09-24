/* *****************************************************************************
 * File:   epwm_drv.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 11 24 02:34
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
#ifndef EPWM_DRV_H
#define	EPWM_DRV_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdint.h>

#include "mclib.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define EPWM_ON_AT_STARTUP  0
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
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void EPWM_vInit (void);
void EPWM_vSetFunctionEPWM1DataProcess (pfVoid_t pFunc);
void EPWM_vSetFunctionEPWM6DataProcess (pfVoid_t pFunc);
void EPWM_vSetDeadTime(void);
void EPWM_vSetPWMFrequencyGlobal(uint16_t u16PWMFreq_Hz);

__interrupt void EPWM1_ISR(void);
__interrupt void EPWM2_ISR(void);
__interrupt void EPWM3_ISR(void);
__interrupt void EPWM4_ISR(void);
__interrupt void EPWM5_ISR(void);
__interrupt void EPWM6_ISR(void);
__interrupt void EPWM7_ISR(void);
__interrupt void EPWM8_ISR(void);
__interrupt void EPWM9_ISR(void);
__interrupt void EPWM10_ISR(void);
__interrupt void EPWM11_ISR(void);
__interrupt void EPWM12_ISR(void);


#endif	/* EPWM_DRV_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
