/* *****************************************************************************
 * File:   io_drv.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 11 25 01:11
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
#include "io_drv.h"
#include "driverlib.h"
#include "device.h"
#include "sys_drv.h"


#if defined CPU1
//#include "main_cpu1.h"
#elif  defined CPU2
//#include "main_cpu2.h"
#else
//#include "main.h"
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

typedef struct
{
    uint16_t bErrorSTOP             : 1;
    uint16_t bErrorOVT_CHOKES       : 1;
    uint16_t bErrorCONTACTOR1       : 1;
    uint16_t bErrorCONTACTOR2       : 1;
    uint16_t bErrorCONTACTOR3       : 1;
}IO_sErrorStop_t;

typedef union
{
    uint16_t u16Register;
    IO_sErrorStop_t sErrorStop;
}IO_uErrorStop_t;



/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
bool IO_bUseIOFbkEmulation = BOARDCFG_IO_FBK_EMUALTION;

uint32_t u32DigitalInputs;
uint16_t u16DigitalOutputs;

uint32_t u32DigitalOutputPortMask1;
uint32_t u32DigitalOutputPortMask2;
uint32_t u32DigitalOutputPortMask;
uint32_t u32DigitalInputPortMask;

uint32_t u32OutputPortDataRead;
uint32_t u32OutputPortDataWrite;
uint32_t u32InputPortDataRead;
uint32_t u32OutputPortDataNew;
uint32_t u32OutputPortDataPreserve;
uint32_t u32InputPortDataReadActiveLow;

uControlIO_t uDOExternal = {0};
uControlIO_t uDIExternal;
uControlIO_t uDOExternalFbk;
uControlIO_t uDIExternalFbk;

uFaultInput_t uFaultInput = {0};


uFaultInput_t uFaultInputDeny =
{
    MASK_DENY_FAULT_INPUT1 << 0 |
    MASK_DENY_FAULT_INPUT2 << 1 |
    MASK_DENY_FAULT_INPUT3 << 2 |
    MASK_DENY_FAULT_INPUT4 << 3 |
    MASK_DENY_FAULT_INPUT5 << 4 |
    MASK_DENY_FAULT_INPUT6 << 5
};

uEnableOutput_t uEnableOutput = {0};

uEnableOutput_t uEnableOutputUse =
{
    USE_ENABLE_OUTPUT1 << 0 |
    USE_ENABLE_OUTPUT2 << 1 |
    USE_ENABLE_OUTPUT3 << 2 |
    USE_ENABLE_OUTPUT4 << 3 |
    USE_ENABLE_OUTPUT5 << 4 |
    USE_ENABLE_OUTPUT6 << 5
};

bool bUseFPGACommandOnEnOut2346 = !BOARDCFG_USE_FPGA_SCI;
uEnableOutput_t uEnableOutput2346 = {0};

IO_uErrorStop_t IO_uErrorStop = {0};

uint16_t IO_nStackMask = 0x7;

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */
void IO_vInit(void)
{
    //
    // Inputs
    //

#ifdef DI1_PIN
    GPIO_setPinConfigInput(DI1_PIN);
    GPIO_setPinConfigInput(DI2_PIN);
    GPIO_setPinConfigInput(DI3_PIN);
    GPIO_setPinConfigInput(DI4_PIN);
    GPIO_setPinConfigInput(DI5_PIN);
    GPIO_setPinConfigInput(DI6_PIN);
    GPIO_setPinConfigInput(DI7_PIN);
    GPIO_setPinConfigInput(DI8_PIN);
    GPIO_setPinConfigInput(DI9_PIN);
    GPIO_setPinConfigInput(DI10_PIN);
    GPIO_setPinConfigInput(DI11_PIN);
    GPIO_setPinConfigInput(DI12_PIN);
    GPIO_setPinConfigInput(DI13_PIN);
    GPIO_setPinConfigInput(DI14_PIN);
    GPIO_setPinConfigInput(DI15_PIN);
    GPIO_setPinConfigInput(DI16_PIN);
    GPIO_setPinConfigInput(DI17_PIN);
    GPIO_setPinConfigInput(DI18_PIN);
    GPIO_setPinConfigInput(DI19_PIN);
    GPIO_setPinConfigInput(DI20_PIN);
    GPIO_setPinConfigInput(DI21_PIN);
    GPIO_setPinConfigInput(DI22_PIN);
    GPIO_setPinConfigInput(DI23_PIN);
    GPIO_setPinConfigInput(DI24_PIN);
#endif

    //
    // Outputs
    //

#ifdef DO1_PIN
    GPIO_setPinConfigOutput(DO1_PIN);
    GPIO_setPinConfigOutput(DO2_PIN);
    GPIO_setPinConfigOutput(DO3_PIN);
    GPIO_setPinConfigOutput(DO4_PIN);
    GPIO_setPinConfigOutput(DO5_PIN);
    GPIO_setPinConfigOutput(DO6_PIN);
    GPIO_setPinConfigOutput(DO7_PIN);
    GPIO_setPinConfigOutput(DO8_PIN);
    GPIO_setPinConfigOutput(DO9_PIN);
    GPIO_setPinConfigOutput(DO10_PIN);
    GPIO_setPinConfigOutput(DO11_PIN);
    GPIO_setPinConfigOutput(DO12_PIN);
    GPIO_setPinConfigOutput(DO13_PIN);
    GPIO_setPinConfigOutput(DO14_PIN);
    GPIO_setPinConfigOutput(DO15_PIN);
    GPIO_setPinConfigOutput(DO16_PIN);
#endif

#ifdef DO1_PIN
    u32DigitalOutputPortMask1  = ((uint32_t)0x000000FF << ((uint16_t)DO1_PIN & (uint16_t)(32-1)));
    u32DigitalOutputPortMask2  = ((uint32_t)0x000000FF << ((uint16_t)DO9_PIN & (uint16_t)(32-1)));
    u32DigitalOutputPortMask  = u32DigitalOutputPortMask1 | u32DigitalOutputPortMask2;
#endif
#ifdef DI1_PIN
    u32DigitalInputPortMask   = ((uint32_t)0x00FFFFFF << ((uint16_t)DI1_PIN & (uint16_t)(32-1)));
#endif



}
void IO_vProcessEnable2346(uint16_t nStack)
{

    IO_nStackMask = nStack;

    switch (nStack)
    {
    case 1:
        uEnableOutput2346.u16Register = 10; /* stack1 */
        break;

    case 2:
        uEnableOutput2346.u16Register = 11; /* stack2 */
        break;

    case 4:
        uEnableOutput2346.u16Register = 12; /* stack3 */
        break;

    case 3:
        uEnableOutput2346.u16Register = 13; /* stack1+2 */
        break;

    case 6:
        uEnableOutput2346.u16Register = 14; /* stack2+3 */
        break;

    case 5:
        uEnableOutput2346.u16Register = 15; /* stack1+3 */
        break;

    case 7:
        uEnableOutput2346.u16Register = 0;  /* stack1+2+3 */
        break;

    default:
        break;
    }
}

void IO_vProcess(void)
{
#ifdef DO1_PIN
    u32OutputPortDataRead = GPIO_readPortData((GPIO_Port)(DO1_PIN / 32));
    u32OutputPortDataPreserve = u32OutputPortDataRead & (~(u32DigitalOutputPortMask));

    u32OutputPortDataNew = ((uint32_t)(u16DigitalOutputs & 0x00FF) << DO1_PIN) | (((uint32_t)((uint16_t)u16DigitalOutputs>>8) & 0x00FF) << DO9_PIN);
    u32OutputPortDataNew &= u32DigitalOutputPortMask;
    u32OutputPortDataWrite = u32OutputPortDataPreserve | u32OutputPortDataNew;

    GPIO_writePortData(GPIO_PORT_A, u32OutputPortDataWrite);
#endif
#ifdef DI1_PIN
    u32InputPortDataReadActiveLow = GPIO_readPortData((GPIO_Port)(DI1_PIN / 32));
    u32InputPortDataRead = ~u32InputPortDataReadActiveLow;
    u32DigitalInputs = u32InputPortDataRead & u32DigitalInputPortMask;

    uDIExternal.u16Register = (uint16_t)u32DigitalInputs;
    uDOExternalFbk.u16Register = u16DigitalOutputs;
#endif
#ifdef FLT1_PIN
    uFaultInput.sFaultInput.bFLT1 = GPIO_readPin(FLT1_PIN);
    uFaultInput.sFaultInput.bFLT2 = GPIO_readPin(FLT2_PIN);
    uFaultInput.sFaultInput.bFLT3 = GPIO_readPin(FLT3_PIN);
    uFaultInput.sFaultInput.bFLT4 = GPIO_readPin(FLT4_PIN);
    uFaultInput.sFaultInput.bFLT5 = GPIO_readPin(FLT5_PIN);
    uFaultInput.sFaultInput.bFLT6 = GPIO_readPin(FLT6_PIN);
#endif
    if  (bUseFPGACommandOnEnOut2346)
    {
        uEnableOutput.sEnableOutput.bEN2 = uEnableOutput2346.sEnableOutput.bEN1;
        uEnableOutput.sEnableOutput.bEN3 = uEnableOutput2346.sEnableOutput.bEN2;
        uEnableOutput.sEnableOutput.bEN4 = uEnableOutput2346.sEnableOutput.bEN3;
        uEnableOutput.sEnableOutput.bEN6 = uEnableOutput2346.sEnableOutput.bEN4;
    }
#ifdef OE_PWM_BUF1

    if (uEnableOutputUse.sEnableOutput.bEN1)
    {
        GPIO_writePin(OE_PWM_BUF1, uEnableOutput.sEnableOutput.bEN1);
    }
    if (uEnableOutputUse.sEnableOutput.bEN2)
    {
        GPIO_writePin(OE_PWM_BUF2, uEnableOutput.sEnableOutput.bEN2);
    }
    if (uEnableOutputUse.sEnableOutput.bEN3)
    {
        GPIO_writePin(OE_PWM_BUF3, uEnableOutput.sEnableOutput.bEN3);
    }
    if (uEnableOutputUse.sEnableOutput.bEN4)
    {
        GPIO_writePin(OE_PWM_BUF4, uEnableOutput.sEnableOutput.bEN4);
    }
    if (uEnableOutputUse.sEnableOutput.bEN5)
    {
        GPIO_writePin(OE_PWM_BUF5, uEnableOutput.sEnableOutput.bEN5);
    }
    if (uEnableOutputUse.sEnableOutput.bEN6)
    {
        GPIO_writePin(OE_PWM_BUF6, uEnableOutput.sEnableOutput.bEN6);
    }
#endif
}

void IO_vClearTrippedErrors (void)
{
    IO_uErrorStop.u16Register = 0;
}

bool IO_COMMON_bIsError(void)
{
    bool bResult = false;

    /* DI_ESTOP */
    if ( (BOARDCFG_vGetIO(DI_SIGNAL_ESTOP) == 0) && (IO_bUseIOFbkEmulation == 0) )
    {
        IO_uErrorStop.sErrorStop.bErrorSTOP = 1;
        bResult = true;
    }

    /* DI_AC_CHOCKES_OVERTEMPERATURE */
    if ( (BOARDCFG_vGetIO(DI_SIGNAL_AC_CHOCKES_OVERTEMPERATURE) == 0) && (IO_bUseIOFbkEmulation == 0) )
    {
        IO_uErrorStop.sErrorStop.bErrorOVT_CHOKES = 1;
        bResult = true;
    }

    /* AC_CONTACTOR_FEEDBACK - rise slowly current or this must be faster */
    if ( ((BOARDCFG_vGetIO(DI_SIGNAL_AC_CONTACTOR1_FEEDBACK) == 0) && (IO_bUseIOFbkEmulation == 0)) ||
         ((BOARDCFG_vGetOutput(DO_SIGNAL_AC1_CONTACTOR_CONTROL) == 0) && (IO_bUseIOFbkEmulation == 1)) )
    {
        if ((IO_nStackMask & (1 << 0)) > 0)
        {
            IO_uErrorStop.sErrorStop.bErrorCONTACTOR1 = 1;
            bResult = true;
        }
    }
    if ( ((BOARDCFG_vGetIO(DI_SIGNAL_AC_CONTACTOR2_FEEDBACK) == 0) && (IO_bUseIOFbkEmulation == 0)) ||
         ((BOARDCFG_vGetOutput(DO_SIGNAL_AC2_CONTACTOR_CONTROL) == 0) && (IO_bUseIOFbkEmulation == 1)) )
    {
        if ((IO_nStackMask & (1 << 1)) > 0)
        {
            IO_uErrorStop.sErrorStop.bErrorCONTACTOR2 = 1;
            bResult = true;
        }
    }
    if ( ((BOARDCFG_vGetIO(DI_SIGNAL_AC_CONTACTOR3_FEEDBACK) == 0) && (IO_bUseIOFbkEmulation == 0)) ||
         ((BOARDCFG_vGetOutput(DO_SIGNAL_AC3_CONTACTOR_CONTROL) == 0) && (IO_bUseIOFbkEmulation == 1)) )
    {
        if ((IO_nStackMask & (1 << 2)) > 0)
        {
            IO_uErrorStop.sErrorStop.bErrorCONTACTOR3 = 1;
            bResult = true;
        }
    }

    return bResult;
}




