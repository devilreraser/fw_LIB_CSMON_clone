/* *****************************************************************************
 * File:   fpga_pwm.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 08 14 12:51
 * 
 * Description: FPGA AP PWM (Analog PWM board PWM fpga)
 * 
 **************************************************************************** */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Guard condition file contents not included more than once */  
/* #pragma once */
#ifndef FPGA_PWM_H
#define	FPGA_PWM_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdbool.h>
#include <stdint.h>
 
#include "boardcfg.h"    
    
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
typedef struct
{
    uint16_t bErrBot  : 1;    
    uint16_t bErrTop  : 1;    
    uint16_t bErrDet  : 1;    
    uint16_t bErrCh1    : 1;
    uint16_t bErrCh2    : 1;
    uint16_t bErrCh3    : 1;
    uint16_t bReserved      : 1;
    uint16_t bGlobalFault   : 1;
    uint16_t u8Reserved     : 8;
}sFREGPWM_t;




typedef union
{
    uint16_t u16Register;
    sFREGPWM_t sFREGPWM;
}uFREGPWM_t;


/* driver detection */
typedef struct
{
    uint8_t ErrDET1 : 1;    /* Latched */
    uint8_t ErrDET2 : 1;    /* Latched */
    uint8_t ErrDET3 : 1;    /* Latched */
    uint8_t DET1 : 1;
    uint8_t DET2 : 1;
    uint8_t DET3 : 1;
}sDET_t;            

typedef union
{
    uint8_t u8Register;
    sDET_t sDET;
}uDET_t;


/* gate driver errors */
typedef struct
{
    uint8_t ErrBot1 : 1;    /* Latched */
    uint8_t ErrBot2 : 1;    /* Latched */
    uint8_t ErrBot3 : 1;    /* Latched */
    uint8_t ErrTop1 : 1;    /* Latched */
    uint8_t ErrTop2 : 1;    /* Latched */
    uint8_t ErrTop3 : 1;    /* Latched */
}sGDE_t;

typedef union
{
    uint8_t u8Register;
    sGDE_t sGDE;
}uGDE_t;


#if BOARDCFG_USE_FPGA_PWM

/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 
extern uDET_t  uStatDET;
extern uGDE_t  uStatGDE;

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
bool FPGA_PWM_vGetPWMError(void);
void FPGA_PWM_vGetPWMStat(void);
void FPGA_PWM_vSetDeadTimeAndMinPulse(void);
void FPGA_PWM_vGetDeadTimeAndMinPulse(void);
void FPGA_PWM_vRunTimeSetupProcess(void);
void FPGA_PWM_vEnable(void);
void FPGA_PWM_vDisable(void);
bool FPGA_PWM_vGetDetectDriverPhase1(void);
bool FPGA_PWM_vGetDetectDriverPhase2(void);
bool FPGA_PWM_vGetDetectDriverPhase3(void);
void FPGA_PWM_vClearErrorStatus(void);

#else

#define FPGA_PWM_vGetPWMError() false
#define FPGA_PWM_vGetPWMStat()
#define FPGA_PWM_vSetDeadTimeAndMinPulse()
#define FPGA_PWM_vGetDeadTimeAndMinPulse()
#define FPGA_PWM_vRunTimeSetupProcess()
#define FPGA_PWM_vEnable()
#define FPGA_PWM_vDisable()
#define FPGA_PWM_vGetDetectDriverPhase1() true
#define FPGA_PWM_vGetDetectDriverPhase2() true
#define FPGA_PWM_vGetDetectDriverPhase3() true
#define FPGA_PWM_vClearErrorStatus()

#endif




#endif	/* FPGA_PWM_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
