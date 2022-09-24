/* *****************************************************************************
 * File:   dio.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 08 14 12:51
 * 
 * Description: DIO (Digital Inputs Outputs) TSC
 * 
 **************************************************************************** */
#ifdef _WIN32
#pragma once
#endif // _WIN32


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <xc.h>
#include <stdint.h>
#include "dio.h"
#include "boardcfg.h"

#include "sys_drv.h"

#if BOARDCFG_USE_DIO

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
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */

bool bDO_nDIS = 1;

uControlIO_t uDOInternal;
uControlIO_t uDIInternal;
uControlIO_t uDOInternalFbk;
uControlIO_t uDIInternalFbk;

bool IO_bUseIOFbkEmulation = 0;

uint16_t u16DIInternalCounterToggle[8];
uint16_t u16DIInternalCounterHigh[8];
uint16_t u16DIInternalCounterLow[8];

uEnableOutput_t uEnableRequestMask = {0xFF};     /* EN1..EN4 Pin Request Mask */
uEnableOutput_t uEnableRequest = {0};            /* EN1..EN4 Pin Request */
uFaultInput_t uFaultStatus = {0};              /* FLT1..FLT4 Pin Status */
uFunctionIO_t uFunctionStatus = {0};         /* F0..F14 Pin Status */
uFunctionIO_t uFunctionOutput = {0};         /* F0..F14 Pin Output */
uFunctionIO_t uFunctionOutputUsed = {0};     /* F0..F14 Pin Output */
uFunctionIO_t uFunctionOutputInit = {0};     /* F0..F14 Pin Output */
uFunctionIO_t uFunctionInputUsed = {0};      /* F0..F14 Pin Output */


/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */

void DIO_vIOInit (void)
{
    uDOInternal.u16Register = 0;
    
    if (uDOInternal.sControlIO.bD1){ DO1_SetHigh(); } else { DO1_SetLow(); }
    if (uDOInternal.sControlIO.bD2){ DO2_SetHigh(); } else { DO2_SetLow(); }
    if (uDOInternal.sControlIO.bD3){ DO3_SetHigh(); } else { DO3_SetLow(); }
    if (uDOInternal.sControlIO.bD4){ DO4_SetHigh(); } else { DO4_SetLow(); }
    if (uDOInternal.sControlIO.bD5){ DO5_SetHigh(); } else { DO5_SetLow(); }
    if (uDOInternal.sControlIO.bD6){ DO6_SetHigh(); } else { DO6_SetLow(); }
    if (uDOInternal.sControlIO.bD7){ DO7_SetHigh(); } else { DO7_SetLow(); }
    if (uDOInternal.sControlIO.bD8){ DO8_SetHigh(); } else { DO8_SetLow(); }
    
    
    if (bDO_nDIS > 0)
    {
        DO_EN_SetHigh();
    }
    else
    {
        DO_EN_SetLow();
    }
    
    #if BOARDCFG_USE_FUNCTION_PINS

    #if USE_TSC_F0 == FUNC_OUTPUT
    uFunctionOutputUsed.sFunctionIO.bF0 = 1;
    #endif
    
    #if USE_TSC_F1 == FUNC_OUTPUT
    uFunctionOutputUsed.sFunctionIO.bF1 = 1;
    #endif

    #if USE_TSC_F2 == FUNC_OUTPUT
    uFunctionOutputUsed.sFunctionIO.bF2 = 1;
    #endif

    #if USE_TSC_F3 == FUNC_OUTPUT
    uFunctionOutputUsed.sFunctionIO.bF3 = 1;
    #endif

    #if USE_TSC_F4 == FUNC_OUTPUT
    uFunctionOutputUsed.sFunctionIO.bF4 = 1;
    #endif

    #if USE_TSC_F5 == FUNC_OUTPUT
    uFunctionOutputUsed.sFunctionIO.bF5 = 1;
    #endif

    #if USE_TSC_F6 == FUNC_OUTPUT
    uFunctionOutputUsed.sFunctionIO.bF6 = 1;
    #endif

    #if USE_TSC_F7 == FUNC_OUTPUT
    uFunctionOutputUsed.sFunctionIO.bF7 = 1;
    #endif

    #if USE_TSC_F8 == FUNC_OUTPUT
    uFunctionOutputUsed.sFunctionIO.bF8 = 1;
    #endif

    #if USE_TSC_F9 == FUNC_OUTPUT
    uFunctionOutputUsed.sFunctionIO.bF9 = 1;
    #endif

    #if USE_TSC_F10 == FUNC_OUTPUT
    uFunctionOutputUsed.sFunctionIO.bF10 = 1;
    #endif

    #if USE_TSC_F11 == FUNC_OUTPUT
    uFunctionOutputUsed.sFunctionIO.bF11 = 1;
    #endif

    #if USE_TSC_F12 == FUNC_OUTPUT
    uFunctionOutputUsed.sFunctionIO.bF12 = 1;
    #endif

    #if USE_TSC_F13 == FUNC_OUTPUT
    uFunctionOutputUsed.sFunctionIO.bF13 = 1;
    #endif

    #if USE_TSC_F14 == FUNC_OUTPUT
    uFunctionOutputUsed.sFunctionIO.bF14 = 1;
    #endif

    #if USE_TSC_F15 == FUNC_OUTPUT
    uFunctionOutputUsed.sFunctionIO.bF15 = 1;
    #endif





    
    #if USE_TSC_F0 == FUNC_INPUT
    uFunctionInputUsed.sFunctionIO.bF0 = 1;
    #endif

    #if USE_TSC_F1 == FUNC_INPUT
    uFunctionInputUsed.sFunctionIO.bF1 = 1;
    #endif

    #if USE_TSC_F2 == FUNC_INPUT
    uFunctionInputUsed.sFunctionIO.bF2 = 1;
    #endif

    #if USE_TSC_F3 == FUNC_INPUT
    uFunctionInputUsed.sFunctionIO.bF3 = 1;
    #endif

    #if USE_TSC_F4 == FUNC_INPUT
    uFunctionInputUsed.sFunctionIO.bF4 = 1;
    #endif

    #if USE_TSC_F5 == FUNC_INPUT
    uFunctionInputUsed.sFunctionIO.bF5 = 1;
    #endif

    #if USE_TSC_F6 == FUNC_INPUT
    uFunctionInputUsed.sFunctionIO.bF6 = 1;
    #endif

    #if USE_TSC_F7 == FUNC_INPUT
    uFunctionInputUsed.sFunctionIO.bF7 = 1;
    #endif

    #if USE_TSC_F8 == FUNC_INPUT
    uFunctionInputUsed.sFunctionIO.bF8 = 1;
    #endif

    #if USE_TSC_F9 == FUNC_INPUT
    uFunctionInputUsed.sFunctionIO.bF9 = 1;
    #endif

    #if USE_TSC_F10 == FUNC_INPUT
    uFunctionInputUsed.sFunctionIO.bF10 = 1;
    #endif

    #if USE_TSC_F11 == FUNC_INPUT
    uFunctionInputUsed.sFunctionIO.bF11 = 1;
    #endif

    #if USE_TSC_F12 == FUNC_INPUT
    uFunctionInputUsed.sFunctionIO.bF12 = 1;
    #endif

    #if USE_TSC_F13 == FUNC_INPUT
    uFunctionInputUsed.sFunctionIO.bF13 = 1;
    #endif

    #if USE_TSC_F14 == FUNC_INPUT
    uFunctionInputUsed.sFunctionIO.bF14 = 1;
    #endif
    
    #if USE_TSC_F15 == FUNC_INPUT
    uFunctionInputUsed.sFunctionIO.bF15 = 1;
    #endif
    
    #endif /* #if BOARDCFG_USE_FUNCTION_PINS */
    
    
}


void DIO_vIOProcess (void)
{
    uint16_t index;
    uint16_t dataNow, dataPrev;
      
    uDIInternalFbk.sControlIO.bD1 = DI1_GetValue();
    uDIInternalFbk.sControlIO.bD2 = DI2_GetValue();
    uDIInternalFbk.sControlIO.bD3 = DI3_GetValue();
    uDIInternalFbk.sControlIO.bD4 = DI4_GetValue();
    uDIInternalFbk.sControlIO.bD5 = DI5_GetValue();
    uDIInternalFbk.sControlIO.bD6 = DI6_GetValue();
    uDIInternalFbk.sControlIO.bD7 = DI7_GetValue();
    uDIInternalFbk.sControlIO.bD8 = DI8_GetValue();

    dataNow = uDOInternalFbk.u16Register;
    dataPrev = uDIInternal.u16Register;
    for (index = 0; index < 8; index++)
    {
        if (dataNow & 1)
        {
            u16DIInternalCounterHigh[index]++;
            if (dataPrev & 1)
            {
                
            }
            else
            {
                u16DIInternalCounterToggle[index]++;
            }
        }
        else
        {
            u16DIInternalCounterLow[index]++;
            if (dataPrev & 1)
            {
                u16DIInternalCounterToggle[index]++;
            }
            else
            {
                
            }
            
        }
        dataNow >>= 1;
        dataPrev >>= 1;
    }

    if (uDOInternal.sControlIO.bD1){ DO1_SetHigh(); } else { DO1_SetLow(); }
    if (uDOInternal.sControlIO.bD2){ DO2_SetHigh(); } else { DO2_SetLow(); }
    if (uDOInternal.sControlIO.bD3){ DO3_SetHigh(); } else { DO3_SetLow(); }
    if (uDOInternal.sControlIO.bD4){ DO4_SetHigh(); } else { DO4_SetLow(); }
    if (uDOInternal.sControlIO.bD5){ DO5_SetHigh(); } else { DO5_SetLow(); }
    if (uDOInternal.sControlIO.bD6){ DO6_SetHigh(); } else { DO6_SetLow(); }
    if (uDOInternal.sControlIO.bD7){ DO7_SetHigh(); } else { DO7_SetLow(); }
    if (uDOInternal.sControlIO.bD8){ DO8_SetHigh(); } else { DO8_SetLow(); }

    if (bDO_nDIS > 0)
    {
        DO_EN_SetHigh();
    }
    else
    {
        DO_EN_SetLow();
    }
    
    uDOInternalFbk.sControlIO.bD1 = DO1_GetValue();
    uDOInternalFbk.sControlIO.bD2 = DO2_GetValue();
    uDOInternalFbk.sControlIO.bD3 = DO3_GetValue();
    uDOInternalFbk.sControlIO.bD4 = DO4_GetValue();
    uDOInternalFbk.sControlIO.bD5 = DO5_GetValue();
    uDOInternalFbk.sControlIO.bD6 = DO6_GetValue();
    uDOInternalFbk.sControlIO.bD7 = DO7_GetValue();
    uDOInternalFbk.sControlIO.bD8 = DO8_GetValue();

    uDIInternal.u16Register = uDIInternalFbk.u16Register;  
    
}

/* Prevent pin from update in main loop */
void DIO_vEnablePinRequestDeny(DIO_eEnablePin ePin) 
{
    if (ePin < DIO_PIN_EN_COUNT)
    {
        uEnableRequestMask.u8Register &= (~((uint8_t)1 << ePin));
    }
}

/* Allow pin to update in main loop */
void DIO_vEnablePinRequestAllow(DIO_eEnablePin ePin) 
{
    if (ePin < DIO_PIN_EN_COUNT)
    {
        uEnableRequestMask.u8Register |= ((uint8_t)1 << ePin);
    }
}


void DIO_vEnableRequestProcess(void)
{
    if (uEnableRequestMask.sEnableOutput.bEN1) { if(uEnableRequest.sEnableOutput.bEN1) {EN1_SetHigh();} else {EN1_SetLow();} }
    if (uEnableRequestMask.sEnableOutput.bEN2) { if(uEnableRequest.sEnableOutput.bEN1) {EN2_SetHigh();} else {EN2_SetLow();} }
    if (uEnableRequestMask.sEnableOutput.bEN3) { if(uEnableRequest.sEnableOutput.bEN1) {EN3_SetHigh();} else {EN3_SetLow();} }
    if (uEnableRequestMask.sEnableOutput.bEN4) { if(uEnableRequest.sEnableOutput.bEN1) {EN4_SetHigh();} else {EN4_SetLow();} }
}
void DIO_vFaultStatusProcess(void)
{
    uFaultStatus.sFaultInput.bFLT1 = FLT1_GetValue();
    uFaultStatus.sFaultInput.bFLT2 = FLT2_GetValue();
    uFaultStatus.sFaultInput.bFLT3 = FLT3_GetValue();
    uFaultStatus.sFaultInput.bFLT4 = FLT4_GetValue();
}


#if BOARDCFG_USE_FUNCTION_PINS
void DIO_vFunctionPinsStatusProcess(void)
{
    uFunctionStatus.sFunctionIO.bF0  =  F0_GetValue();
    uFunctionStatus.sFunctionIO.bF1  =  F1_GetValue();
    uFunctionStatus.sFunctionIO.bF2  =  F2_GetValue();
    uFunctionStatus.sFunctionIO.bF3  =  F3_GetValue();
    uFunctionStatus.sFunctionIO.bF4  =  F4_GetValue();
    uFunctionStatus.sFunctionIO.bF5  =  F5_GetValue();
    uFunctionStatus.sFunctionIO.bF6  =  F6_GetValue();
    uFunctionStatus.sFunctionIO.bF7  =  F7_GetValue();
    uFunctionStatus.sFunctionIO.bF8  =  F8_GetValue();
    uFunctionStatus.sFunctionIO.bF9  =  F9_GetValue();
    uFunctionStatus.sFunctionIO.bF10 = F10_GetValue();
    uFunctionStatus.sFunctionIO.bF11 = F11_GetValue();
    uFunctionStatus.sFunctionIO.bF12 = F12_GetValue();
    uFunctionStatus.sFunctionIO.bF13 = F13_GetValue();
    uFunctionStatus.sFunctionIO.bF14 = F14_GetValue();
    uFunctionStatus.sFunctionIO.bF15 = F15_GetValue();
}



void DIO_vFunctionPinsOutputProcess(void)
{
    if (uFunctionOutputUsed.sFunctionIO.bF0 ) { if(uFunctionOutput.sFunctionIO.bF0 ) { F0_SetHigh();} else { F0_SetLow();}  F0_SetDigitalOutput();} else {if (uFunctionInputUsed.sFunctionIO.bF0  > 0) { F0_SetDigitalInput();} }                        
    if (uFunctionOutputUsed.sFunctionIO.bF1 ) { if(uFunctionOutput.sFunctionIO.bF1 ) { F1_SetHigh();} else { F1_SetLow();}  F1_SetDigitalOutput();} else {if (uFunctionInputUsed.sFunctionIO.bF1  > 0) { F1_SetDigitalInput();} }
    if (uFunctionOutputUsed.sFunctionIO.bF2 ) { if(uFunctionOutput.sFunctionIO.bF2 ) { F2_SetHigh();} else { F2_SetLow();}  F2_SetDigitalOutput();} else {if (uFunctionInputUsed.sFunctionIO.bF2  > 0) { F2_SetDigitalInput();} }
    if (uFunctionOutputUsed.sFunctionIO.bF3 ) { if(uFunctionOutput.sFunctionIO.bF3 ) { F3_SetHigh();} else { F3_SetLow();}  F3_SetDigitalOutput();} else {if (uFunctionInputUsed.sFunctionIO.bF3  > 0) { F3_SetDigitalInput();} }
    if (uFunctionOutputUsed.sFunctionIO.bF4 ) { if(uFunctionOutput.sFunctionIO.bF4 ) { F4_SetHigh();} else { F4_SetLow();}  F4_SetDigitalOutput();} else {if (uFunctionInputUsed.sFunctionIO.bF4  > 0) { F4_SetDigitalInput();} }
    if (uFunctionOutputUsed.sFunctionIO.bF5 ) { if(uFunctionOutput.sFunctionIO.bF5 ) { F5_SetHigh();} else { F5_SetLow();}  F5_SetDigitalOutput();} else {if (uFunctionInputUsed.sFunctionIO.bF5  > 0) { F5_SetDigitalInput();} }
    if (uFunctionOutputUsed.sFunctionIO.bF6 ) { if(uFunctionOutput.sFunctionIO.bF6 ) { F6_SetHigh();} else { F6_SetLow();}  F6_SetDigitalOutput();} else {if (uFunctionInputUsed.sFunctionIO.bF6  > 0) { F6_SetDigitalInput();} }
    if (uFunctionOutputUsed.sFunctionIO.bF7 ) { if(uFunctionOutput.sFunctionIO.bF7 ) { F7_SetHigh();} else { F7_SetLow();}  F7_SetDigitalOutput();} else {if (uFunctionInputUsed.sFunctionIO.bF7  > 0) { F7_SetDigitalInput();} }
    if (uFunctionOutputUsed.sFunctionIO.bF8 ) { if(uFunctionOutput.sFunctionIO.bF8 ) { F8_SetHigh();} else { F8_SetLow();}  F8_SetDigitalOutput();} else {if (uFunctionInputUsed.sFunctionIO.bF8  > 0) { F8_SetDigitalInput();} }
    if (uFunctionOutputUsed.sFunctionIO.bF9 ) { if(uFunctionOutput.sFunctionIO.bF9 ) { F9_SetHigh();} else { F9_SetLow();}  F9_SetDigitalOutput();} else {if (uFunctionInputUsed.sFunctionIO.bF9  > 0) { F9_SetDigitalInput();} }
    if (uFunctionOutputUsed.sFunctionIO.bF10) { if(uFunctionOutput.sFunctionIO.bF10) {F10_SetHigh();} else {F10_SetLow();} F10_SetDigitalOutput();} else {if (uFunctionInputUsed.sFunctionIO.bF10 > 0) {F10_SetDigitalInput();} }
    if (uFunctionOutputUsed.sFunctionIO.bF11) { if(uFunctionOutput.sFunctionIO.bF11) {F11_SetHigh();} else {F11_SetLow();} F11_SetDigitalOutput();} else {if (uFunctionInputUsed.sFunctionIO.bF11 > 0) {F11_SetDigitalInput();} }
    if (uFunctionOutputUsed.sFunctionIO.bF12) { if(uFunctionOutput.sFunctionIO.bF12) {F12_SetHigh();} else {F12_SetLow();} F12_SetDigitalOutput();} else {if (uFunctionInputUsed.sFunctionIO.bF12 > 0) {F12_SetDigitalInput();} }
    if (uFunctionOutputUsed.sFunctionIO.bF13) { if(uFunctionOutput.sFunctionIO.bF13) {F13_SetHigh();} else {F13_SetLow();} F13_SetDigitalOutput();} else {if (uFunctionInputUsed.sFunctionIO.bF13 > 0) {F13_SetDigitalInput();} }
    if (uFunctionOutputUsed.sFunctionIO.bF14) { if(uFunctionOutput.sFunctionIO.bF14) {F14_SetHigh();} else {F14_SetLow();} F14_SetDigitalOutput();} else {if (uFunctionInputUsed.sFunctionIO.bF14 > 0) {F14_SetDigitalInput();} }
    if (uFunctionOutputUsed.sFunctionIO.bF15) { if(uFunctionOutput.sFunctionIO.bF15) {F15_SetHigh();} else {F15_SetLow();} F15_SetDigitalOutput();} else {if (uFunctionInputUsed.sFunctionIO.bF15 > 0) {F15_SetDigitalInput();} } 
}

#endif /* #if BOARDCFG_USE_FUNCTION_PINS */



#endif /* #if BOARDCFG_USE_DIO */
