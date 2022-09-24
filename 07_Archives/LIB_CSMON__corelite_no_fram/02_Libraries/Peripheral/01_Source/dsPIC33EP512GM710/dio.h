/* *****************************************************************************
 * File:   io.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 08 14 12:51
 * 
 * Description: IO on TSC
 * 
 **************************************************************************** */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Guard condition file contents not included more than once */  
/* #pragma once */
#ifndef IO_H
#define	IO_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdint.h>
#include <stdbool.h>
    
#include "boardcfg.h"
    
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */
typedef enum
{
    DIO_PIN_EN_1,
    DIO_PIN_EN_2,
    DIO_PIN_EN_3,
    DIO_PIN_EN_4,
    DIO_PIN_EN_COUNT,
}DIO_eEnablePin;
    
    
/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
typedef struct
{
    uint16_t bFLT1 : 1;
    uint16_t bFLT2 : 1;
    uint16_t bFLT3 : 1;
    uint16_t bFLT4 : 1;
    uint16_t bFLT5 : 1;
    uint16_t bFLT6 : 1;
    uint16_t bFLT7 : 1;
    uint16_t bFLT8 : 1;
}sFaultInput_t;

typedef struct
{
    uint16_t bEN1 : 1;
    uint16_t bEN2 : 1;
    uint16_t bEN3 : 1;
    uint16_t bEN4 : 1;
    uint16_t bEN5 : 1;
    uint16_t bEN6 : 1;
    uint16_t bEN7 : 1;
    uint16_t bEN8 : 1;
}sEnableOutput_t;

typedef struct
{
    uint16_t bF0 : 1;
    uint16_t bF1 : 1;
    uint16_t bF2 : 1;
    uint16_t bF3 : 1;
    uint16_t bF4 : 1;
    uint16_t bF5 : 1;
    uint16_t bF6 : 1;
    uint16_t bF7 : 1;

    uint16_t bF8 : 1;  
    uint16_t bF9  : 1;
    uint16_t bF10 : 1;
    uint16_t bF11 : 1;
    uint16_t bF12 : 1;
    uint16_t bF13 : 1;
    uint16_t bF14 : 1;
    uint16_t bF15 : 1;

    
}sFunctionIO_t;

typedef struct
{
    uint16_t bD1 : 1;
    uint16_t bD2 : 1;
    uint16_t bD3 : 1;
    uint16_t bD4 : 1;
    uint16_t bD5 : 1;
    uint16_t bD6 : 1;
    uint16_t bD7 : 1;
    uint16_t bD8 : 1;
    
    uint16_t bD9  : 1;
    uint16_t bD10 : 1;
    uint16_t bD11 : 1;
    uint16_t bD12 : 1;
    uint16_t bD13 : 1;
    uint16_t bD14 : 1;
    uint16_t bD15 : 1;
    uint16_t bD16 : 1;
    
}sControlIO_t;

typedef union
{
    uint16_t u16Register;
    sControlIO_t sControlIO;
}uControlIO_t;

typedef union
{
    uint16_t u16Register;
    sFunctionIO_t sFunctionIO;
}uFunctionIO_t;

typedef union
{
    uint8_t u8Register;
    sFaultInput_t sFaultInput;
}uFaultInput_t;

typedef union
{
    uint8_t u8Register;
    sEnableOutput_t sEnableOutput;
}uEnableOutput_t;



#if BOARDCFG_USE_DIO

/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 
extern uFunctionIO_t uFunctionOutput;         /* F0..F14 Pin Output */
extern uFaultInput_t uFaultStatus;
extern bool IO_bUseIOFbkEmulation;
extern uControlIO_t uDOInternal;
extern uControlIO_t uDIInternal;

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */ 
#define IO_bSetUseIOFbkEmulation() {IO_bUseIOFbkEmulation = 1;}
#define IO_bClrUseIOFbkEmulation() {IO_bUseIOFbkEmulation = 0;}
#define IO_COMMON_bIsError() 0

void DIO_vIOInit (void);
void DIO_vIOProcess (void);
void DIO_vEnableRequestProcess(void);
void DIO_vFaultStatusProcess(void);
#if BOARDCFG_USE_FUNCTION_PINS
void DIO_vFunctionPinsStatusProcess(void);
void DIO_vFunctionPinsOutputProcess(void);
#else
#define DIO_vFunctionPinsStatusProcess()
#define DIO_vFunctionPinsOutputProcess()
#endif
void DIO_vEnablePinRequestDeny(DIO_eEnablePin ePin); 
void DIO_vEnablePinRequestAllow(DIO_eEnablePin ePin);
   
#else

#define IO_bSetUseIOFbkEmulation()
#define IO_bClrUseIOFbkEmulation()
#define IO_COMMON_bIsError() 0

#define DIO_vIOInit()
#define DIO_vIOProcess()
#define DIO_vEnableRequestProcess()
#define DIO_vFaultStatusProcess()
#define DIO_vFunctionPinsStatusProcess()
#define DIO_vFunctionPinsOutputProcess()
#define DIO_vEnablePinRequestDeny(x)
#define DIO_vEnablePinRequestAllow(x)

#endif




#endif	/* IO_H */

#ifdef __cplusplus
}
#endif /* __cplusplus */
