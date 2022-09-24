/* *****************************************************************************
 * File:   fpga_common.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 08 14 12:51
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
#include "boardcfg.h"

#if BOARDCFG_USE_FPGA

#include "sys_drv.h"
#include "fpga_common.h"
#include "fpga_pwm.h"
#include "fpga_trip.h"


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
bool FPGA_COMMON_bUseFPGANoErrorEmulation = 0;

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */
void FPGA_COMMON_vResetEnable(void)
{
    BOARDCFG_FPGA_RESET();
    #if BOARDCFG_USE_FPGA_TRIP    
    FPGA_TRIP_vClearErrorStatus();
    #endif
    #if BOARDCFG_USE_FPGA_PWM    
    FPGA_PWM_vClearErrorStatus();
    #endif
}

void FPGA_COMMON_vResetRelease(void)
{
    BOARDCFG_FPGA_RESET_RELEASE();
}

void FPGA_COMMON_vEnablePWM(void)
{
    #if BOARDCFG_USE_FPGA_PWM    
    FPGA_PWM_vEnable();
    #endif    
}

void FPGA_COMMON_vDisablePWM(void)
{
    #if BOARDCFG_USE_FPGA_PWM    
    FPGA_PWM_vDisable();
    #endif    
   
}



bool FPGA_COMMON_bIsError(void)
{
    bool bResult = false;

    #if BOARDCFG_USE_FPGA_PWM    
    if (FPGA_PWM_vGetPWMError())
    {
        bResult = true;
    }
    #endif    
    #if BOARDCFG_USE_FPGA_TRIP    
    if (FPGA_TRIP_vGetTripError())
    {
        bResult = true;
    }
    #endif    
    if(FPGA_COMMON_bUseFPGANoErrorEmulation)
    {
        bResult = false;
    }
    return bResult;
}

FPGA_uErrorCode_t FPGA_COMMON_uGetError(void)
{
    FPGA_uErrorCode_t uErrorCode = {0};

    #if BOARDCFG_USE_FPGA_TRIP    
    if (FPGA_TRIP_vGetTripError())
    {
        uErrorCode.sErrorCode.bTripError = 1;
    }
    #endif    
    #if BOARDCFG_USE_FPGA_PWM    
    if (FPGA_PWM_vGetPWMError())
    {
        uErrorCode.sErrorCode.bPWMError = 1;
    }
    #endif    

    return uErrorCode;
}

void FPGA_COMMON_vSetStackErrorMask(uint16_t nStack)
{
}

void FPGA_COMMON_vClrStackErrorMask(uint16_t nStack)
{
}

void FPGA_COMMON_vResetTripStatus(void)
{
    /* Clear Auxiliary errors */
}

#endif /* BOARDCFG_USE_FPGA */
