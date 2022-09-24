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
#include "fpga_common.h"
#include "boardcfg.h"
#include "sys_drv.h"
#include "driverlib.h"
#include "device.h"


#if USE_CONTROL
#include "control_common.h"
#else
typedef struct
{
     uint16_t bStack1 : 1;
     uint16_t bStack2 : 1;
     uint16_t bStack3 : 1;

}CONTROL_sStackUsageMask_t;

typedef union
{
    uint16_t u16Register;
    CONTROL_sStackUsageMask_t sStackUsageMask;
}CONTROL_uStackUsageMask_t;

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
    uint16_t bErrorSCI      : 1;
    uint16_t bErrorIO       : 1;
}FPGA_COMMON_sErrorStop_t;

typedef union
{
    uint16_t u16Register;
    FPGA_COMMON_sErrorStop_t sErrorStop;
}FPGA_COMMON_uErrorStop_t;



/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
bool FPGA_COMMON_bUseCommunicationTrips = 1;

bool FPGA_COMMON_bUseFPGANoErrorEmulation = 0;


bool FPGA_COMMON_bUseRiseErrorFromCommunication = BOARDCFG_USE_ERROR_RISE_FPGA_SCI;
bool FPGA_COMMON_bUseRiseErrorFromDigitalInputs = BOARDCFG_USE_ERROR_RISE_FPGA_FLT;

CONTROL_uStackUsageMask_t FPGA_COMMON_uStackMask =
{
     0x7
};

FPGA_COMMON_uErrorStop_t FPGA_COMMON_uErrorStop;
uFaultInput_t uFaultInputError = {0};
uFaultInput_t uFaultInputDenyError = {0};
uFaultInput_t uFaultInputDenyInv = {0};
uFaultInput_t uFaultInputTest = {0};


/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */

void FPGA_COMMON_vResetEnable(void)
{
    uEnableOutput.sEnableOutputSCC.bClearFaults = CLR_FLT_RST;
#ifdef CLR_FLT_PIN
    GPIO_writePin(CLR_FLT_PIN, uEnableOutput.sEnableOutputSCC.bClearFaults);
#endif
    /* Clear Auxiliary errors */
    FPGA_SCI_vClearTrippedErrors();
    FPGA_COMMON_uErrorStop.u16Register = 0;
    IO_vClearTrippedErrors();
}



void FPGA_COMMON_vResetRelease(void)
{
    uEnableOutput.sEnableOutputSCC.bClearFaults = CLR_FLT_IDL;
#ifdef CLR_FLT_PIN
    GPIO_writePin(CLR_FLT_PIN, uEnableOutput.sEnableOutputSCC.bClearFaults);
#endif
}

void FPGA_COMMON_vResetTripStatus(void)
{
    /* Clear Auxiliary errors */
    FPGA_SCI_vClearTrippedErrors();
    FPGA_COMMON_uErrorStop.u16Register = 0;
    IO_vClearTrippedErrors();
}


void FPGA_COMMON_vEnablePWM(void)
{

#if BOARDCFG_USE_FPGA_SCI
    if (FPGA_COMMON_uStackMask.sStackUsageMask.bStack1)
    {
        uEnableOutput.sEnableOutputSCC.bEnablePWM = ENA_PWM_RUN;
    }
    else
    {
        uEnableOutput.sEnableOutputSCC.bEnablePWM = ENA_PWM_OFF;
    }
    if (FPGA_COMMON_uStackMask.sStackUsageMask.bStack2)
    {
        uEnableOutput.sEnableOutputSCC.bEnablePWM2 = ENA_PWM_RUN;
    }
    else
    {
        uEnableOutput.sEnableOutputSCC.bEnablePWM2 = ENA_PWM_OFF;
    }
    if (FPGA_COMMON_uStackMask.sStackUsageMask.bStack3)
    {
        uEnableOutput.sEnableOutputSCC.bEnablePWM3 = ENA_PWM_RUN;
    }
    else
    {
        uEnableOutput.sEnableOutputSCC.bEnablePWM3 = ENA_PWM_OFF;
    }
#ifdef ENA_PWM1_PIN
    GPIO_writePin(ENA_PWM1_PIN, uEnableOutput.sEnableOutputSCC.bEnablePWM);
#endif
#ifdef ENA_PWM2_PIN
    GPIO_writePin(ENA_PWM2_PIN, uEnableOutput.sEnableOutputSCC.bEnablePWM2);
#endif
#ifdef ENA_PWM3_PIN
    GPIO_writePin(ENA_PWM3_PIN, uEnableOutput.sEnableOutputSCC.bEnablePWM3);
#endif
#else
    uEnableOutput.sEnableOutputSCC.bEnablePWM = ENA_PWM_RUN;
#ifdef ENA_PWM1_PIN
    GPIO_writePin(ENA_PWM1_PIN, uEnableOutput.sEnableOutputSCC.bEnablePWM);
#endif
#endif


}


void FPGA_COMMON_vDisablePWM(void)
{
#ifdef ENA_PWM1_PIN
#if BOARDCFG_USE_FPGA_SCI
    uEnableOutput.sEnableOutputSCC.bEnablePWM = ENA_PWM_OFF;
    uEnableOutput.sEnableOutputSCC.bEnablePWM2 = ENA_PWM_OFF;
    uEnableOutput.sEnableOutputSCC.bEnablePWM3 = ENA_PWM_OFF;

    GPIO_writePin(ENA_PWM1_PIN, uEnableOutput.sEnableOutputSCC.bEnablePWM);
     GPIO_writePin(ENA_PWM2_PIN, uEnableOutput.sEnableOutputSCC.bEnablePWM2);
     GPIO_writePin(ENA_PWM3_PIN, uEnableOutput.sEnableOutputSCC.bEnablePWM3);
#else
    uEnableOutput.sEnableOutputSCC.bEnablePWM = ENA_PWM_OFF;
    GPIO_writePin(ENA_PWM1_PIN, uEnableOutput.sEnableOutputSCC.bEnablePWM);
#endif
#endif
}



bool FPGA_COMMON_bIsError(void)
{
    bool bResult = false;
    uFaultInput_t uFaultInputDenyInv_loc = {0};
    uFaultInput_t uFaultInputTest_loc = {0};

    if (FPGA_COMMON_bUseCommunicationTrips)
    {
        if (FPGA_SCI_bIsError())
        {
            if (FPGA_COMMON_bUseRiseErrorFromCommunication)
            {
                FPGA_COMMON_uErrorStop.sErrorStop.bErrorSCI = 1;
                bResult = true;
            }
        }
    }

    uFaultInputDenyInv_loc.u16Register = ~(uFaultInputDeny.u16Register);
    uFaultInputTest_loc.u16Register = uFaultInput.u16Register;

    uFaultInputDenyInv.u16Register = uFaultInputDenyInv_loc.u16Register;
    uFaultInputTest.u16Register = uFaultInputTest_loc.u16Register;

    if ((uFaultInputTest_loc.u16Register & uFaultInputDenyInv_loc.u16Register) > 0)
    {

        uFaultInputDenyError.u16Register = uFaultInputDenyInv_loc.u16Register;
        uFaultInputError.u16Register = uFaultInputTest_loc.u16Register;
        if (FPGA_COMMON_bUseRiseErrorFromDigitalInputs)
        {
            FPGA_COMMON_uErrorStop.sErrorStop.bErrorIO = 1;
            bResult = true;
        }
    }

    if(FPGA_COMMON_bUseFPGANoErrorEmulation)
    {
        bResult = false;
    }

    return bResult;
}

FPGA_uErrorCode_t FPGA_COMMON_uErrorCode = {0};

FPGA_uErrorCode_t FPGA_COMMON_uGetError(void)
{
    FPGA_uErrorCode_t uErrorCode = {0};

    if (FPGA_COMMON_bUseCommunicationTrips)
    {

        if (FPGA_SCI_bIsErrorTemp())
        {
            uErrorCode.sErrorCode.bTempError = 1;
            FPGA_COMMON_uErrorCode.sErrorCode.bTempError = 1;
        }
        if (FPGA_SCI_bIsErrorDet())
        {
            uErrorCode.sErrorCode.bDETError = 1;
            FPGA_COMMON_uErrorCode.sErrorCode.bDETError = 1;
        }
        if (FPGA_SCI_bIsErrorDrv())
        {
            uErrorCode.sErrorCode.bDRVError = 1;
            FPGA_COMMON_uErrorCode.sErrorCode.bDRVError = 1;
        }
        if (FPGA_SCI_bIsErrorOvt())
        {
            uErrorCode.sErrorCode.bTempError = 1;
            FPGA_COMMON_uErrorCode.sErrorCode.bTempError = 1;
        }
        if (FPGA_SCI_bIsErrorOcn())
        {
            uErrorCode.sErrorCode.bOCError = 1;
            FPGA_COMMON_uErrorCode.sErrorCode.bOCError = 1;
        }
        if (FPGA_SCI_bIsErrorOcp())
        {
            uErrorCode.sErrorCode.bOCError = 1;
            FPGA_COMMON_uErrorCode.sErrorCode.bOCError = 1;
        }
        if (FPGA_SCI_bIsErrorVdc())
        {
            uErrorCode.sErrorCode.bOVError = 1;
            FPGA_COMMON_uErrorCode.sErrorCode.bOVError = 1;
        }

        if (FPGA_SCI_bIsErrorGlb())
        {
            uErrorCode.sErrorCode.bGLBError = 1;
            FPGA_COMMON_uErrorCode.sErrorCode.bGLBError = 1;
        }

    }


    if ((uFaultInput.sFaultInput.bFLT1 != 0)      && (uFaultInputDeny.sFaultInput.bFLT1 != 0))
    {
        uErrorCode.sErrorCode.bStackError = 1;
        FPGA_COMMON_uErrorCode.sErrorCode.bStackError = 1;
    }
    if ((uFaultInput.sFaultInput.bFLT2 != 0)      && (uFaultInputDeny.sFaultInput.bFLT2 != 0))
    {
        uErrorCode.sErrorCode.bStackError = 1;
        FPGA_COMMON_uErrorCode.sErrorCode.bStackError = 1;
    }
    if ((uFaultInput.sFaultInput.bFLT3 != 0)      && (uFaultInputDeny.sFaultInput.bFLT3 != 0))
    {
        uErrorCode.sErrorCode.bStackError = 1;
        FPGA_COMMON_uErrorCode.sErrorCode.bStackError = 1;
    }

#if 0
    if ((uFaultInput.sFaultInputSCC.bERR != 0)      && (uFaultInputDeny.sFaultInputSCC.bERR != 0))
    {
        uErrorCode.sErrorCode.bDRVError = 1;
        FPGA_COMMON_uErrorCode.sErrorCode.bDRVError = 1;
    }
    if ((uFaultInput.sFaultInputSCC.bEDET != 0)     && (uFaultInputDeny.sFaultInputSCC.bEDET != 0))
    {
        uErrorCode.sErrorCode.bDETError = 1;
        FPGA_COMMON_uErrorCode.sErrorCode.bDETError = 1;
    }
    if ((uFaultInput.sFaultInputSCC.bOVT != 0)      && (uFaultInputDeny.sFaultInputSCC.bOVT != 0))
    {
        uErrorCode.sErrorCode.bTempError = 1;
        FPGA_COMMON_uErrorCode.sErrorCode.bTempError = 1;
    }
    if ((uFaultInput.sFaultInputSCC.bOCPIph != 0)   && (uFaultInputDeny.sFaultInputSCC.bOCPIph != 0))
    {
        uErrorCode.sErrorCode.bOCError = 1;
        FPGA_COMMON_uErrorCode.sErrorCode.bOCError = 1;
    }
    if ((uFaultInput.sFaultInputSCC.bOCNIph != 0)   && (uFaultInputDeny.sFaultInputSCC.bOCNIph != 0))
    {
        uErrorCode.sErrorCode.bOCError = 1;
        FPGA_COMMON_uErrorCode.sErrorCode.bOCError = 1;
    }
    if ((uFaultInput.sFaultInputSCC.bOVDCL != 0)    && (uFaultInputDeny.sFaultInputSCC.bOVDCL != 0))
    {
        uErrorCode.sErrorCode.bOVError = 1;
        FPGA_COMMON_uErrorCode.sErrorCode.bOVError = 1;
    }
#endif

    return uErrorCode;
}



void FPGA_COMMON_vSetStackErrorMask(uint16_t nStack)
{
    FPGA_SCI_vSetStackUsed(nStack);

    FPGA_COMMON_uStackMask.u16Register |= (1 << nStack);
    IO_vProcessEnable2346(FPGA_COMMON_uStackMask.u16Register);

}

void FPGA_COMMON_vClrStackErrorMask(uint16_t nStack)
{
    FPGA_SCI_vSetStackDeny(nStack);
    FPGA_COMMON_uStackMask.u16Register &= (~(1 << nStack));
    IO_vProcessEnable2346(FPGA_COMMON_uStackMask.u16Register);
}



