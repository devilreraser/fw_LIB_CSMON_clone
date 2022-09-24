/* *****************************************************************************
 * File:   testproc.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 08 20 17:11
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

#if BOARDCFG_USE_TESTPROC

#include "testproc.h"
#include "control.h"
//#include "timer.h"

#include "hmlib.h"
#include <stdbool.h>
#include <stdint.h>

#if BOARDCFG_USE_DIO
#include "dio.h"
#endif

#if BOARDCFG_USE_FPGA_DIO   
#include "fpga_dio.h"
#endif

#if BOARDCFG_USE_FPGA_PWM
#include "fpga_pwm.h"
#endif

#if BOARDCFG_USE_FPGA
#include "fpga_common.h"
#endif

#if USE_CSMON
#include "csmon.h"
#endif


/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define CBANK_MAX_DISCHARGE_TIMEOUT u32uSecToTimerTicks(10000000);

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

//#define STRING_CONCAT_BASE(_a_, _b_) _a_##_b_
//#define STRING_CONCAT(_a_, _b_) STRING_CONCAT_BASE(_a_, _b_)
//#define GPIO_PIN_MODE(_pin_, _mode_) STRING_CONCAT(GPIO_, STRING_CONCAT(_pin_, STRING_CONCAT(_, _mode_)))
//#define GPIO_PIN_MODE_GPIO(_pin_) STRING_CONCAT(GPIO_, STRING_CONCAT(_pin_, STRING_CONCAT(_GPIO, _pin_)))

 


#define OutputSet(output, level)    {output = level;}
#define OutputSetWithDummy(output1,output2, level)    {output1 = level;output2 = level;}
#define OutputFun(functn, level)    functn(level)

#define Input_Get(input)            input
#define Input_Fun(functn)           functn()

/* Outputs */

#define CMD_PWM_CLEAR_FAULT         sControlAccess.bClearFaults
#define CMD_PWM_CLEAR_FAULT_FPGA    sControlAccess.bErrorClearForced

#define CMD_PWM_DISABLE             sControlAccess.bStart
#define CMD_PWM_ENABLE              sControlAccess.bStartNoCalib
#define CMD_CALIB_OFFSET            sControlAccess.bStartCalibOnly

#define CMD_PRECHARGE_DUMMY         bDummyCommandPrecharge  /* DO1 Dido SW */   /* GREEN */ /* Normal operation or Precharge */
#define CMD_DIS_RELEASE_DUMMY       bDummyCommandDischargeRelease/* BLUE */
#define CMD_MC_CLOSE_DUMMY          bDummyCommandMainContact    /* RED */


#if BOARDCFG_USE_TSC_SMALL_RELAY_IO_CONFIGURATION

#define CMD_PRECHARGE               uDOExternal.sControlIO.bD1  /* DO1 Dido SW */   /* GREEN */ /* Normal operation or Precharge */
#define CMD_DIS_RELEASE             uDOExternal.sControlIO.bD2   /* BLUE */
#define CMD_MC_CLOSE                uDOExternal.sControlIO.bD3  /* RED */
#define CMD_FAN                     uDOExternal.sControlIO.bD4
#define CMD_SPARE2                  uDOExternal.sControlIO.bD5
#define CMD_SPARE3                  uDOExternal.sControlIO.bD6
//#define CMD_SSR                     uDOExternal.sControlIO.bD7  /*  */
#define CMD_SSR                     uFunctionOutput.sFunctionIO.bF1  /* Solid State Relay Is Commanded By F1 function that drives DO7 through the FPGA */
#define CMD_SPARE5                  uDOExternal.sControlIO.bD8  /* DO8 Dido SW */

#elif BOARDCFG_USE_TSC_EXT_IO_CONFIGURATION

#define CMD_PRECHARGE               uDOInternal.sControlIO.bD1  /* DO1 Dido SW */   /* GREEN */ /* Normal operation or Precharge */
#define CMD_DIS_RELEASE             uDOInternal.sControlIO.bD2   /* BLUE */
#define CMD_MC_CLOSE                uDOInternal.sControlIO.bD3  /* RED */
#define CMD_FAN                     uDOInternal.sControlIO.bD4
#define CMD_SPARE2                  uDOInternal.sControlIO.bD5
#define CMD_SPARE3                  uDOInternal.sControlIO.bD6
#define CMD_SSR                     uDOInternal.sControlIO.bD7  /*  */
//#define CMD_SSR                     uFunctionOutput.sFunctionIO.bF1  /* Solid State Relay Is Commanded By F1 function that drives DO7 through the FPGA */
#define CMD_SPARE5                  uDOInternal.sControlIO.bD8  /* DO8 Dido SW */

#else

#define CMD_PRECHARGE               bRealCommandPrecharge//uDOExternal.sControlIO.bD1  /* DO1 Dido SW */   /* GREEN */ /* Normal operation or Precharge */
#define CMD_DIS_RELEASE             bRealCommandDischargeRelease //uDOExternal.sControlIO.bD2  /* BLUE */
#define CMD_MC_CLOSE                bRealCommandMainContact //uDOExternal.sControlIO.bD3  /* RED */
#define CMD_FAN                     bDummyCommand//uDOExternal.sControlIO.bD4
#define CMD_SPARE2                  bDummyCommand//uDOExternal.sControlIO.bD5
#define CMD_SPARE3                  bDummyCommand//uDOExternal.sControlIO.bD6
//#define CMD_SSR                     uDOExternal.sControlIO.bD7  /*  */
#define CMD_SSR                     bDummyCommand//uFunctionOutput.sFunctionIO.bF1  /* Solid State Relay Is Commanded By F1 function that drives DO7 through the FPGA */
#define CMD_SPARE5                  bDummyCommand//uDOExternal.sControlIO.bD8  /* DO8 Dido SW */

#endif


#define CMD_ON              1 
#define CMD_OFF             0 

/* Inputs */
#define FBK_PWM_ENABLE_REQ  sControlAccess.bStartNoCalib
#define FBK_PWM_ENABLE      sControlAccess.bStart
#define FBK_PWM_ERROR       sControlAccess.bFault
#define FBK_PWM_ERROR_FPGA  FPGA_COMMON_bIsError()

#if BOARDCFG_USE_TSC_SMALL_RELAY_IO_CONFIGURATION
#define FBK_MCB_CLOSED      uDIExternal.sControlIO.bD1  /* DI0 Dido SW */
#define FBK_MCB_TRIP        uDIExternal.sControlIO.bD2
#define FBK_MC_CLOSED       uDIExternal.sControlIO.bD3  /* RED */
#define FBK_PRECH           uDIExternal.sControlIO.bD4  /* GREEN */
#define FBK_PRECHDEL        uDIExternal.sControlIO.bD5     /* GREEN connect to FBK_PRECH */
#define FBK_DIS_RELEASE     uDIExternal.sControlIO.bD6  /* BLUE */
#define FBK_OVERTEMP        uDIExternal.sControlIO.bD7
#define FBK_SAFETY          uDIExternal.sControlIO.bD8  /* DI7 Dido SW */
#elif BOARDCFG_USE_TSC_EXT_IO_CONFIGURATION
#define FBK_MCB_CLOSED      uDIInternal.sControlIO.bD1  /* DI0 Dido SW */
#define FBK_MCB_TRIP        uDIInternal.sControlIO.bD2
#define FBK_MC_CLOSED       uDIInternal.sControlIO.bD3  /* RED */
#define FBK_PRECH           uDIInternal.sControlIO.bD4  /* GREEN */
#define FBK_PRECHDEL        uDIInternal.sControlIO.bD5     /* GREEN connect to FBK_PRECH */
#define FBK_DIS_RELEASE     uDIInternal.sControlIO.bD6  /* BLUE */
#define FBK_OVERTEMP        uDIInternal.sControlIO.bD7
#define FBK_SAFETY          uDIInternal.sControlIO.bD8  /* DI7 Dido SW */
#else
#define FBK_MCB_CLOSED      0//uDIExternal.sControlIO.bD1  /* DI0 Dido SW */
#define FBK_MCB_TRIP        0//uDIExternal.sControlIO.bD2
#define FBK_MC_CLOSED       0//uDIExternal.sControlIO.bD3  /* RED */
#define FBK_PRECH           0//uDIExternal.sControlIO.bD4  /* GREEN */
#define FBK_PRECHDEL        0//uDIExternal.sControlIO.bD5     /* GREEN connect to FBK_PRECH */
#define FBK_DIS_RELEASE     0//uDIExternal.sControlIO.bD6  /* BLUE */
#define FBK_OVERTEMP        0//uDIExternal.sControlIO.bD7
#define FBK_SAFETY          0//uDIExternal.sControlIO.bD8  /* DI7 Dido SW */
#endif
#define FBK_ON              1 
#define FBK_OFF             0 

#define sh_u_dcl_fbk        VoltageStack1






/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

CONTROL_uStackUsageMask_t TESTPROC_uStackMask =
{
     0x7
};

CONTROL_uStackUsageMask_t TESTPROC_uStackMaskLast =
{
     0x0
};

/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */


/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */

bool bUseTestprocIOFbkEmulation = BOARDCFG_TESTPROC_IO_FBK_EMULATION;

bool bDummyCommandPrecharge = 0;
bool bDummyCommandDischargeRelease = 0;
bool bDummyCommandMainContact = 0;

bool bRealCommandPrecharge = 0;
bool bRealCommandDischargeRelease = 0;
bool bRealCommandMainContact = 0;

bool bDummyCommand;

bool bVoltageApplied = 0;
bool bVoltageDischarging = 0;

uint32_t u32VoltageDischargeTimeout = 0;

uint16_t u16TimeLast = 0;
uint16_t u16TimePass = 0;
uint16_t u16TimeCurr = 0;
uint32_t u32TestStateTimePass = 0;
uint32_t u32TestErrTimeout = 0;
uint32_t u32TestCmdTimeout = 0;
uint32_t u32TestFanTimeout = 0;

TESTPROC_eState_t eTestProcState = TEST_STA_IDLE;
TESTPROC_eState_t eTestProcStateLast = TEST_STA_IDLE;
TESTPROC_eErrorState_t eTestProcErrorState = TEST_ERR_NO_ERROR;
bool bTestProcessErrorDetection = 0;

bool bStartTestReserved = 0;

bool bTestWithoutCalibration = true;
bool bStartTest = false;
bool bStartTestCalibrationOnlyNextStartTest = BOARDCFG_START_WITH_CALIBRATION_FIRST_AFTER_RESET;
bool bStartTestCalibrationIfNeeded = false;
bool bTestCalibrationCompleted = false;
bool bStopTest = false;
bool bTestOutStatus = false;

TESTPROC_eRunRequest_t eTestRunRequest = REQUEST_IDLE;
bool bTestRunStatus = false;
bool bTestRunStatusFbk = false;


bool bClearFaultRequest = false;
bool bClearFaultStatus = false;

bool bTestDummyRequest;
bool bTestDummyStatus;

bool bTestErrorStatus = false;;
bool bTestStopStatus = false;   //not used

bool bVoltageRegulation = false;
bool bVoltageRegulationFromPWMISR = true;

uint16_t sh_u_dcl_set = BOARDCFG_UDCL_DEF_REQUEST;
uint16_t sh_u_dcl_now;
 
 uint16_t u_dcl_min_precharge = BOARDCFG_UDCL_MIN_PRECHARGE;
 uint16_t u_dcl_max_precharge = BOARDCFG_UDCL_MAX_PRECHARGE;

uint8_t u8LastStopTestID = 0;
 
 
//CSMON_eRecorderParameterMask_t TESTPROC_u8MaskSetupRecorderRunModeEnter = CSMON_MASK_RECORDER_NONE;    /* do not setup recorder by default */
//CSMON_eRecorderParameterMask_t TESTPROC_u8MaskSetupRecorderRunModeEnter = CSMON_MASK_RECORDER_1;    /* recorder 1 */
CSMON_eRecorderParameterMask_t TESTPROC_u8MaskSetupRecorderRunModeEnter = CSMON_MASK_RECORDERS_012;    /* use recorders 0 1 2 by default */

 
//CSMON_eRecorderParameterMask_t TESTPROC_u8MaskStartRecorderRunModeEnter = CSMON_MASK_RECORDER_NONE;    /* do not start recorder by default */
//CSMON_eRecorderParameterMask_t TESTPROC_u8MaskStartRecorderRunModeEnter = CSMON_MASK_RECORDER_1;    /* recorder 1 */
CSMON_eRecorderParameterMask_t TESTPROC_u8MaskStartRecorderRunModeEnter = CSMON_MASK_RECORDERS_012;    /* use recorders 0 1 2 by default */

//CSMON_eRecorderParameterMask_t TESTPROC_u8MaskStopRecorderErrModeEnter = CSMON_MASK_RECORDER_NONE;    /* do not stop recorder */
//CSMON_eRecorderParameterMask_t TESTPROC_u8MaskStopRecorderErrModeEnter = CSMON_MASK_RECORDER_1;    /* recorder 1 */
CSMON_eRecorderParameterMask_t TESTPROC_u8MaskStopRecorderErrModeEnter = CSMON_MASK_RECORDERS_012;    /* use recorders 0 1 2 by default */

//CSMON_eRecorderParameterMask_t TESTPROC_u8MaskStopRecorderStopModeEnter = CSMON_MASK_RECORDER_1;    /* recorder 1 */
CSMON_eRecorderParameterMask_t TESTPROC_u8MaskStopRecorderStopModeEnter = CSMON_MASK_RECORDERS_012;    /* use recorders 0 1 2 by default */

uint8_t TESTPROC_u8MaskSetupRecorderRunModeEnterCount = 0;
uint8_t TESTPROC_u8MaskStartRecorderRunModeEnterCount = 0;
uint8_t TESTPROC_u8MaskStopRecorderErrModeEnterCount = 0;
uint8_t TESTPROC_u8MaskStopRecorderStopModeEnterCount = 0;

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */




/* *****************************************************************************
 * Functions
 **************************************************************************** */


TESTPROC_eState_t TESTPROC_eGetState(void)
{
    return eTestProcState;
}





bool bCheckError(void)
{
    bool bResult = false;
    
    
    
    
    if ((FBK_PWM_ERROR == true)
     || (eTestProcErrorState != TEST_ERR_NO_ERROR))
    {
        if (eTestProcErrorState == TEST_ERR_NO_ERROR)
        {
            eTestProcErrorState = TEST_ERR_CONTROL;
        }
        u32TestErrTimeout = u32uSecToTimerTicks(500000);
        eTestProcState = TEST_STA_ERROR;
        bClearFaultRequest = false;
        bResult = true;
        
        CSMON_vSetStopRecorderParameterMask(TESTPROC_u8MaskStopRecorderErrModeEnter);
        TESTPROC_u8MaskStopRecorderErrModeEnterCount++;
            
    }
    return bResult;
}

void vStartTest(void)
{
    
    if(bTestRunStatus == false)
    {
        if( ((bStartTestCalibrationOnlyNextStartTest == true) || (bCheckError() == false)) && (bTestOutStatus == false)) 
        {
            if (FBK_PWM_ERROR_FPGA == true)
            {
                OutputSet(CMD_PWM_CLEAR_FAULT_FPGA      ,CMD_ON);
            }
            bTestRunStatus = true;            
            OutputSet(CMD_PWM_CLEAR_FAULT      ,CMD_ON);

        }
        eTestProcState = TEST_STA_INIT;
        eTestProcStateLast = eTestProcState;
    }
    
}


void vStartTestFbk(void)
{
    
    if(bTestRunStatus == false)
    {
        if( ((bStartTestCalibrationOnlyNextStartTest == true) || (bCheckError() == false)) && (bTestOutStatus == false)) 
        {
            bTestRunStatusFbk = eTestRunRequest;
        }
        //eTestProcState = TEST_STA_INIT;
        //eTestProcStateLast = eTestProcState;
        
        

    }
    
}

void TESTPROC_vStackUsageMaskingProcess(void)
{
    if (TESTPROC_uStackMask.u16Register != TESTPROC_uStackMaskLast.u16Register)
    {
        TESTPROC_uStackMaskLast.u16Register = TESTPROC_uStackMask.u16Register;

        if (TESTPROC_uStackMask.sStackUsageMask.bStack1)
        {
            FPGA_COMMON_vSetStackErrorMask(FPGA_COMMON_STACK1);
        }
        else
        {
            FPGA_COMMON_vClrStackErrorMask(FPGA_COMMON_STACK1);
        }
        if (TESTPROC_uStackMask.sStackUsageMask.bStack2)
        {
            FPGA_COMMON_vSetStackErrorMask(FPGA_COMMON_STACK2);
        }
        else
        {
            FPGA_COMMON_vClrStackErrorMask(FPGA_COMMON_STACK2);
        }
        if (TESTPROC_uStackMask.sStackUsageMask.bStack3)
        {
            FPGA_COMMON_vSetStackErrorMask(FPGA_COMMON_STACK3);
        }
        else
        {
            FPGA_COMMON_vClrStackErrorMask(FPGA_COMMON_STACK3);
        }

        CONTROL_vSetStackMask(TESTPROC_uStackMask.u16Register);
    }


}


void TESTPROC_vClearFaultRequestChangeProcess(void)
{
    if (bClearFaultRequest == 1)
    {    
        if ((bTestRunStatus) || (eTestProcState != TEST_STA_ERROR))    //??? to be checked
        {
            bClearFaultStatus = 0;    /* Test Must not run and be in error state */  
        }
        else
        {
            bClearFaultStatus = 1;
        }
    }
    else
    {
        bClearFaultStatus = 0;
    }
}
void TESTPROC_vTestRunRequestChangeProcess(void)
{
    if (eTestRunRequest == REQUEST_RUN)
    {
        vStartTestFbk();
        
        //eTestRunRequest = REQUEST_IDLE;
    }
    else
    {
        //bStopTest = true;
        bTestRunStatusFbk = false;
    }
}





void TESTPROC_vTestDummyRequestChangeProcess(void)
{
    if (bTestDummyStatus != bTestDummyRequest)
    {

        bTestDummyStatus = bTestDummyRequest;
        
        if (bTestDummyStatus)
        {
            FPGA_COMMON_bUseFPGANoErrorEmulation = 1;
            #if BOARDCFG_USE_DIO
            IO_bSetUseIOFbkEmulation();
            #endif
            sControlAccess.bEmulateCurrentFbck = 1;
            sControlAccess.bEmulateVoltageFbck = 1;
            bUseTestprocIOFbkEmulation = 1;
            bUseControlTemperatureTrip = 0;
            
            /* Clear Errors On Entering Emulation Mode */
            OutputSet(CMD_PWM_CLEAR_FAULT_FPGA      ,CMD_ON);
            OutputSet(CMD_PWM_CLEAR_FAULT      ,CMD_ON);
            
        }
        else
        {
            FPGA_COMMON_bUseFPGANoErrorEmulation = 0;
            #if BOARDCFG_USE_DIO
            IO_bClrUseIOFbkEmulation();
            #endif
            sControlAccess.bEmulateCurrentFbck = 0;
            sControlAccess.bEmulateVoltageFbck = 0;
            bUseTestprocIOFbkEmulation = BOARDCFG_TESTPROC_IO_FBK_EMULATION;            /* Normal Operation depending on board configuration */
            bUseControlTemperatureTrip = BOARDCFG_USE_TEMPERATURE_MEASUREMENT_TRIP;     /* Normal Operation depending on board configuration */
        }
    }
}



void TESTPROC_vProcessVoltageRegulaion(void)
{
    if (bVoltageRegulation == 1)
    {
        if (bVoltageRegulationFromPWMISR)
        {
            if (sh_u_dcl_now >= sh_u_dcl_set)
            {
                OutputSet(CMD_SSR, CMD_OFF );
            }
            else
            {
                OutputSet(CMD_SSR, CMD_ON );
            }
        }
    }
}


void TESTPROC_vProcess(void)
{


    TESTPROC_vTestDummyRequestChangeProcess();




    /* Calculate TimePassed From Previous Call */
    u16TimeCurr = TIMER_vGetTimerMainRollingCounter();
    u16TimePass = u16TimeCurr - u16TimeLast;
    u16TimeLast = u16TimeCurr;
    
    /* Generate u32TestStateTimePass */
    if ( u32TestStateTimePass < (uint32_t)( (int32_t)0 - (int32_t)u16TimePass ) )
    {
        u32TestStateTimePass += u16TimePass;
    }
    else
    {
        u32TestStateTimePass = 0xFFFFFFFF;
    }
    
    if (u32VoltageDischargeTimeout > u16TimePass)
    {
        u32VoltageDischargeTimeout -= u16TimePass;
    }
    else
    {
        u32VoltageDischargeTimeout = 0;
        bVoltageDischarging = 0;
    }
   
    
    
    if (u32TestCmdTimeout > u16TimePass)
    {
        u32TestCmdTimeout -= u16TimePass;
    }
    else
    {
        u32TestCmdTimeout = 0;
    }
    
    
    
    if (u32TestErrTimeout > u16TimePass)
    {
        u32TestErrTimeout -= u16TimePass;
    }
    else
    {
        u32TestErrTimeout = 0;
    }
    
    if (u32TestFanTimeout > u16TimePass)
    {
        u32TestFanTimeout -= u16TimePass;
    }
    else
    {
        u32TestFanTimeout = 0;
        
    }
    
    sh_u_dcl_now = sh_u_dcl_fbk;
    

    
    /* Modbus Start Stop Clear Errors */
//    if (eTestRunRequest > REQUEST_IDLE)
//    {
//        eTestRunRequest = REQUEST_IDLE;
//        bStartTest = true;
//    }
//    else
    if (eTestRunRequest == REQUEST_RUN)
    {
        eTestRunRequest = REQUEST_IDLE;
        bStartTest = true;
    }
    else if (eTestRunRequest == REQUEST_STOP)
    {
        eTestRunRequest = REQUEST_IDLE;
        u8LastStopTestID = 1;
        bStopTest = true;
    }

    
    
    if(bClearFaultRequest == true)
    {
        bClearFaultRequest = false;
        u8LastStopTestID = 2;
        bStopTest = true;
    }

    
    
    
    
    if(bStartTest)
    {
        bStartTest = false;
        
        CSMON_vSetSetupRecorderParameterMask(TESTPROC_u8MaskSetupRecorderRunModeEnter);
        TESTPROC_u8MaskSetupRecorderRunModeEnterCount++;
        CSMON_vSetStartRecorderParameterMask(TESTPROC_u8MaskStartRecorderRunModeEnter);
        TESTPROC_u8MaskStartRecorderRunModeEnterCount++;
        
        vStartTest();
    }
    if(bStopTest)
    {
        bStopTest = false;
        
        
        if(u8LastStopTestID != 3)   /* stop test recorder if not stop enter mode after calibration only request */
        {
            CSMON_vSetStopRecorderParameterMask(TESTPROC_u8MaskStopRecorderStopModeEnter);
            TESTPROC_u8MaskStopRecorderStopModeEnterCount++;
        }
        u8LastStopTestID = 0;
            
        
        
        if(bTestRunStatus)
        {
            OutputSet(CMD_PWM_ENABLE    ,CMD_OFF);
            OutputSet(CMD_PWM_DISABLE    ,CMD_OFF);
            OutputSetWithDummy(CMD_MC_CLOSE,  CMD_MC_CLOSE_DUMMY  ,CMD_OFF);
            OutputSetWithDummy(CMD_PRECHARGE ,CMD_PRECHARGE_DUMMY ,CMD_OFF);
            OutputSet(CMD_SSR           ,CMD_OFF);
            bVoltageApplied = 0;
            //OutputSetWithDummy(CMD_DIS_RELEASE ,CMD_DIS_RELEASE_DUMMY   ,CMD_OFF);
            eTestProcState = TEST_STA_STOP;
            eTestProcStateLast = eTestProcState;
            u32TestCmdTimeout = u32uSecToTimerTicks(500000);
        }
        else
        {
            eTestProcState = TEST_STA_IDLE;
            eTestProcStateLast = TEST_STA_IDLE;
            eTestProcErrorState = TEST_ERR_NO_ERROR;
            bClearFaultStatus = true;
            bTestErrorStatus = false;
            if (FBK_PWM_ERROR_FPGA == true)
            {
                OutputSet(CMD_PWM_CLEAR_FAULT_FPGA      ,CMD_ON);
            }
            
            if (FBK_PWM_ERROR == true)
            {
                OutputSet(CMD_PWM_CLEAR_FAULT           ,CMD_ON);
            }
            FPGA_COMMON_vResetTripStatus();
        }
    }
    
    
    if (eTestProcState != TEST_STA_IO_MANUAL)
    {
    //if (Input_Get(CMD_FAN) == CMD_ON) - not needed
    {
        if (u32TestFanTimeout == 0)
        {
            OutputSet(CMD_FAN    ,CMD_OFF);
        }
    }
    }
    
    if (FBK_PWM_ERROR_FPGA == true)
    {
        bTestProcessErrorDetection = 1;
    }
    else
    {
        bTestProcessErrorDetection = 0;
    }
    

    
    switch (eTestProcState)
    {
        case TEST_STA_IDLE:
            CONTROL_vSetStartUpEmulationVoltage();
            
            if (bStartTestCalibrationOnlyNextStartTest) 
            {
                bTestCalibrationCompleted = 0;
                
            }
            break;
            
        case TEST_STA_INIT:
            OutputSet(CMD_PWM_ENABLE    ,CMD_OFF);
            OutputSet(CMD_PWM_DISABLE    ,CMD_OFF);
            OutputSetWithDummy(CMD_MC_CLOSE,  CMD_MC_CLOSE_DUMMY  ,CMD_OFF);
            OutputSetWithDummy(CMD_PRECHARGE ,CMD_PRECHARGE_DUMMY ,CMD_OFF);
            OutputSetWithDummy(CMD_DIS_RELEASE,  CMD_DIS_RELEASE_DUMMY  ,CMD_OFF);
            OutputSet(CMD_SSR           ,CMD_OFF);

            bVoltageApplied = 0;
            
            if((bStartTestCalibrationOnlyNextStartTest == true) || (bCheckError() == false))
            {
                if ((bTestWithoutCalibration == false) || (bStartTestCalibrationOnlyNextStartTest))
                {
                    u32TestCmdTimeout = u32uSecToTimerTicks(500000);        /* this time must be enough for the calibration to complete */
                    eTestProcState = TEST_STA_CALIB;
                    eTestProcStateLast = eTestProcState;

                    OutputSet(CMD_CALIB_OFFSET  ,CMD_ON );      //request perform offset calibration
                }
                else
                {
                    eTestProcState = TEST_STA_READY;
                    eTestProcStateLast = eTestProcState;
                }
            }
            break;
            
        case TEST_STA_CALIB:
            
            
            
            if((bStartTestCalibrationOnlyNextStartTest == true) || (bCheckError() == false))
            {
                if (FBK_PWM_ENABLE)
                {
                    u32TestCmdTimeout = u32uSecToTimerTicks(500000);        /* reset timeout if calibration not completed */
                }
                
                if(u32TestCmdTimeout == 0)
                { 
                    if (bStartTestCalibrationOnlyNextStartTest)
                    {
                        bStartTestCalibrationOnlyNextStartTest = 0;
                        
                        
                        bTestCalibrationCompleted = 1;
                        
                        if (bStartTestCalibrationIfNeeded)     
                        {
                            bStartTestCalibrationIfNeeded = 0;
                            eTestProcState = TEST_STA_READY;
                            eTestProcStateLast = eTestProcState;
                        }
                        else
                        {
                            eTestProcState = TEST_STA_IDLE;
                            eTestProcStateLast = eTestProcState;
                            bStopTest = true;
                            u8LastStopTestID = 3;
                        }
                    }
                    else
                    {
                        eTestProcState = TEST_STA_READY;
                        eTestProcStateLast = eTestProcState;
                        bStartTestCalibrationIfNeeded = 0;
                    }
                    
                }
            }

            break;
            
        case TEST_STA_READY:
            
            //OutputSet(CMD_CALIB_OFFSET  ,CMD_OFF);
            OutputSet(CMD_PWM_ENABLE    ,CMD_OFF);
            OutputSet(CMD_PWM_DISABLE    ,CMD_OFF);
            OutputSet(CMD_SSR           ,CMD_OFF);
            OutputSetWithDummy(CMD_MC_CLOSE,  CMD_MC_CLOSE_DUMMY  ,CMD_OFF);
            OutputSetWithDummy(CMD_PRECHARGE ,CMD_PRECHARGE_DUMMY ,CMD_OFF);
            OutputSetWithDummy(CMD_DIS_RELEASE,  CMD_DIS_RELEASE_DUMMY  ,CMD_OFF);
            bVoltageApplied = 0;            
            if(bCheckError() == false) 
            {
                if (FBK_PWM_ERROR_FPGA == true)
                {
                    eTestProcState = TEST_STA_ERROR;
                    eTestProcErrorState = TEST_ERR_DETECTION;
                    eTestProcStateLast = eTestProcState;
                    
                    CSMON_vSetStopRecorderParameterMask(TESTPROC_u8MaskStopRecorderErrModeEnter);
                    TESTPROC_u8MaskStopRecorderErrModeEnterCount++;
            
                   
                }
                else
                {
                    eTestProcState = TEST_STA_DISCH_RELEASE;
                    eTestProcStateLast = eTestProcState;
                    u32TestCmdTimeout = u32uSecToTimerTicks(500000);
                }
            }
            break;
            
        case TEST_STA_DISCH_RELEASE:

            OutputSet(CMD_PWM_ENABLE    ,CMD_OFF);
            OutputSet(CMD_PWM_DISABLE    ,CMD_OFF);
            OutputSetWithDummy(CMD_MC_CLOSE ,CMD_MC_CLOSE_DUMMY      ,CMD_OFF);
            OutputSet(CMD_SSR           ,CMD_OFF);
            OutputSetWithDummy(CMD_DIS_RELEASE ,CMD_DIS_RELEASE_DUMMY   ,CMD_ON );
            bVoltageApplied = 0;            
            if((bCheckError() == false) && (FBK_PWM_ERROR_FPGA == false))
            {
                if((Input_Get(FBK_DIS_RELEASE) == FBK_ON) || ((bUseTestprocIOFbkEmulation == 1) && (bDummyCommandDischargeRelease == FBK_ON)))
                {
                    u32TestCmdTimeout = u32uSecToTimerTicks(3000000);
                    eTestProcState = TEST_STA_PRECHARGE;
                    eTestProcStateLast = eTestProcState;
                    bVoltageRegulation = 0;
                } 
                else    
                {
                    if(u32TestCmdTimeout == 0)
                    { 
                        eTestProcErrorState = TEST_ERR_DISCH_RELEASE;
                        bVoltageRegulation = 0;
                    }
                }
            }

            break;
            
        case TEST_STA_PRECHARGE:
            //OutputSet(CMD_PWM_ENABLE    ,CMD_OFF);
            //OutputSet(CMD_PWM_DISABLE    ,CMD_OFF);
            OutputSetWithDummy(CMD_MC_CLOSE,  CMD_MC_CLOSE_DUMMY  ,CMD_OFF);
            OutputSetWithDummy(CMD_PRECHARGE ,CMD_PRECHARGE_DUMMY ,CMD_ON);
            OutputSetWithDummy(CMD_DIS_RELEASE,  CMD_DIS_RELEASE_DUMMY  ,CMD_ON);
            bVoltageApplied = 1;          
            
            if((bCheckError() == false) && (FBK_PWM_ERROR_FPGA == false))
            { // no check for error because of mech vibrations
                
                
                

                CONTROL_vSetRunningEmulationVoltage();
                
                
                if((sh_u_dcl_now > u_dcl_min_precharge) &&
                        ((Input_Get(FBK_PRECH)== FBK_ON) || ((bUseTestprocIOFbkEmulation == 1) && (bDummyCommandPrecharge == FBK_ON))))
                {
                    if (sh_u_dcl_set >= u_dcl_max_precharge )
                    {
                        OutputSet(CMD_SSR, CMD_ON );
                        bVoltageRegulation = 0;
                        //OutputSet(CMD_FAN, CMD_OFF );
                        u32TestCmdTimeout = u32uSecToTimerTicks(10000000); // 10s delay for precharge before close contactor
                        eTestProcState = TEST_STA_PRECHARGE_DELAY;
                        eTestProcStateLast = eTestProcState;
                    }
                    else
                    {
                        u32TestCmdTimeout = u32uSecToTimerTicks(10000000); // 10s delay reset forever
                        OutputSet(CMD_FAN, CMD_ON );
                        u32TestFanTimeout = u32uSecToTimerTicks(60000000);
                        if (sh_u_dcl_now >= sh_u_dcl_set)
                        {
                            if ((FBK_PWM_ENABLE_REQ == 0) && (FBK_PWM_ENABLE == 0))
                            {
                                OutputSet(CMD_PWM_ENABLE    ,CMD_ON );
                                
                            }
                        }
                        
                        if (bVoltageRegulationFromPWMISR == 0)
                        {
                            if (sh_u_dcl_now >= sh_u_dcl_set)
                            {
                                OutputSet(CMD_SSR, CMD_OFF );
                            }
                            else
                            {
                                OutputSet(CMD_SSR, CMD_ON );

                            }
                        }
                        bVoltageRegulation = 1;
                    }
 
                } 
                else
                {
                    if (sh_u_dcl_set >= u_dcl_max_precharge )
                    {
                        OutputSet(CMD_SSR, CMD_ON );
                        bVoltageRegulation = 0;
                    }
                    else
                    {
                        if (bVoltageRegulationFromPWMISR == 0)
                        {
                            if (sh_u_dcl_now > sh_u_dcl_set)
                            {
                                OutputSet(CMD_SSR, CMD_OFF );
                            }
                            else if (sh_u_dcl_now < sh_u_dcl_set)
                            {
                                OutputSet(CMD_SSR, CMD_ON );
                            }
                        }
                        bVoltageRegulation = 1;
                    }
                    
                    if(u32TestCmdTimeout == 0)
                    { 
                        eTestProcErrorState = TEST_ERR_TIMEOUT_PRECHARGE;
                    }
                }
            }
            
            break;
            
        case TEST_STA_PRECHARGE_DELAY:
            OutputSet(CMD_PWM_ENABLE    ,CMD_OFF);
            OutputSet(CMD_PWM_DISABLE    ,CMD_OFF);
            OutputSetWithDummy(CMD_MC_CLOSE ,CMD_MC_CLOSE_DUMMY      ,CMD_OFF);
            OutputSet(CMD_SSR           ,CMD_ON);
            OutputSetWithDummy(CMD_PRECHARGE ,CMD_PRECHARGE_DUMMY     ,CMD_ON );
            OutputSetWithDummy(CMD_DIS_RELEASE ,CMD_DIS_RELEASE_DUMMY   ,CMD_ON );
            bVoltageApplied = 1;          

            if((bCheckError() == false) && (FBK_PWM_ERROR_FPGA == false))   /* !!! Not tested */
            { // no check for error because of mech vibrations


                if((u32TestCmdTimeout == 0) &&
                        ((Input_Get(FBK_PRECHDEL) == FBK_ON) || ((bUseTestprocIOFbkEmulation == 1) && (bDummyCommandPrecharge == FBK_ON))))
                {
                    // 10s min passsed and external time relay is on
                    OutputSetWithDummy(CMD_MC_CLOSE ,CMD_MC_CLOSE_DUMMY      ,CMD_ON );
                    u32TestCmdTimeout = u32uSecToTimerTicks(2000000); // time out for damping distortions
                    eTestProcState = TEST_STA_MC_CLOSE;
                    eTestProcStateLast = eTestProcState;
                }
            }
            
            break;
            
        case TEST_STA_MC_CLOSE:
            OutputSet(CMD_PWM_ENABLE    ,CMD_OFF);
            OutputSet(CMD_PWM_DISABLE    ,CMD_OFF);
            OutputSetWithDummy(CMD_PRECHARGE ,CMD_PRECHARGE_DUMMY     ,CMD_ON );
            OutputSetWithDummy(CMD_DIS_RELEASE ,CMD_DIS_RELEASE_DUMMY   ,CMD_ON );
            OutputSet(CMD_SSR           ,CMD_ON);
            OutputSetWithDummy(CMD_MC_CLOSE ,CMD_MC_CLOSE_DUMMY      ,CMD_ON );
            bVoltageApplied = 1;          

            if((bCheckError() == false) && (FBK_PWM_ERROR_FPGA == false))
            {
                if((Input_Get(FBK_MC_CLOSED) == FBK_ON) || ((bUseTestprocIOFbkEmulation == 1) && (bDummyCommandMainContact == FBK_ON)))
                {
                    if(u32TestCmdTimeout == 0)
                    {
                        eTestProcState = TEST_STA_WAIT;
                        eTestProcStateLast = eTestProcState;
                        u32TestCmdTimeout = u32uSecToTimerTicks(2000000); // time delay at least 1s
                    }
                } 
                else
                {
                    if(u32TestCmdTimeout == 0)
                    { 
                        eTestProcErrorState = TEST_ERR_TIMEOUT_MAIN_CONTACT;
                    }
                    
                }   
            }
            break;
            
        case TEST_STA_WAIT:
            OutputSet(CMD_PWM_ENABLE    ,CMD_OFF);
            OutputSet(CMD_PWM_DISABLE    ,CMD_OFF);
            OutputSetWithDummy(CMD_PRECHARGE ,CMD_PRECHARGE_DUMMY     ,CMD_ON );
            OutputSetWithDummy(CMD_DIS_RELEASE ,CMD_DIS_RELEASE_DUMMY   ,CMD_ON );
            OutputSet(CMD_SSR           ,CMD_ON);
            OutputSetWithDummy(CMD_MC_CLOSE ,CMD_MC_CLOSE_DUMMY      ,CMD_ON );
            bVoltageApplied = 1;          

            if((bCheckError() == false) && (FBK_PWM_ERROR_FPGA == false))
            {
                if((sh_u_dcl_now >= sh_u_dcl_set) || (sh_u_dcl_set >= u_dcl_max_precharge))
                {
                    u32TestCmdTimeout = u32uSecToTimerTicks(100000); // 100ms delay and start test
                    eTestProcState = TEST_STA_TEST;
                    eTestProcStateLast = eTestProcState;
                } 
                else
                {
                    if(u32TestCmdTimeout == 0)
                    { 
                        eTestProcErrorState = TEST_ERR_TIMEOUT_DC_LEVEL;
                    }
                    
                }
            }
            
            break;
            
        case TEST_STA_TEST:
            
            OutputSetWithDummy(CMD_MC_CLOSE ,CMD_MC_CLOSE_DUMMY      ,CMD_ON );
            OutputSetWithDummy(CMD_PRECHARGE ,CMD_PRECHARGE_DUMMY     ,CMD_ON );
            OutputSet(CMD_SSR           ,CMD_ON );
            OutputSetWithDummy(CMD_DIS_RELEASE ,CMD_DIS_RELEASE_DUMMY   ,CMD_ON );
            bVoltageApplied = 1;          

            if((bCheckError() == false) && (FBK_PWM_ERROR_FPGA == false))
            {
                if ((FBK_PWM_ENABLE_REQ == 0) && (FBK_PWM_ENABLE == 0))
                {
                    OutputSet(CMD_PWM_ENABLE    ,CMD_ON );
                }
            }            
            break;
            
        case TEST_STA_STOP:
            OutputSet(CMD_PWM_ENABLE    ,CMD_OFF);
            OutputSet(CMD_PWM_DISABLE    ,CMD_OFF);
            OutputSetWithDummy(CMD_MC_CLOSE ,CMD_MC_CLOSE_DUMMY      ,CMD_OFF);
            OutputSetWithDummy(CMD_PRECHARGE ,CMD_PRECHARGE_DUMMY     ,CMD_OFF);
            OutputSet(CMD_SSR           ,CMD_OFF);
            OutputSetWithDummy(CMD_DIS_RELEASE ,CMD_DIS_RELEASE_DUMMY   ,CMD_ON );
            
            if( ((Input_Get(FBK_MC_CLOSED) == false) || ((bUseTestprocIOFbkEmulation == 1) && (bDummyCommandMainContact == FBK_ON)))
                    && (u32TestErrTimeout == 0))
            {
                OutputSetWithDummy(CMD_DIS_RELEASE ,CMD_DIS_RELEASE_DUMMY   ,CMD_OFF);
                
                
                
                u32VoltageDischargeTimeout = CBANK_MAX_DISCHARGE_TIMEOUT;
                bVoltageDischarging = 1;          
                bVoltageApplied = 0;          
                bTestRunStatus = false;
                bTestRunStatusFbk = false;
                eTestProcState = TEST_STA_IDLE;
                eTestProcStateLast = eTestProcState;
            }
                
            break;
            
        case TEST_STA_ERROR:
            OutputSet(CMD_PWM_ENABLE    ,CMD_OFF);
            OutputSet(CMD_PWM_DISABLE    ,CMD_OFF);
            OutputSetWithDummy(CMD_MC_CLOSE ,CMD_MC_CLOSE_DUMMY      ,CMD_OFF);
            OutputSetWithDummy(CMD_PRECHARGE ,CMD_PRECHARGE_DUMMY     ,CMD_OFF);
            OutputSet(CMD_SSR           ,CMD_OFF);
            OutputSetWithDummy(CMD_DIS_RELEASE ,CMD_DIS_RELEASE_DUMMY   ,CMD_ON );
            
            

            if( ((Input_Get(FBK_MC_CLOSED) == false) || ((bUseTestprocIOFbkEmulation == 1) && (bDummyCommandMainContact == FBK_ON)))
                    && (u32TestErrTimeout == 0))
            {
                OutputSetWithDummy(CMD_DIS_RELEASE ,CMD_DIS_RELEASE_DUMMY   ,CMD_OFF);

                u32VoltageDischargeTimeout = CBANK_MAX_DISCHARGE_TIMEOUT;
                bVoltageDischarging = 1; 
                bVoltageApplied = 0;          
                bTestRunStatus = false;
                bTestRunStatusFbk = false;
                bClearFaultStatus = false;
                bTestErrorStatus = true;
                //eTestProcState = TEST_STA_IDLE;
                //eTestProcStateLast = eTestProcState;
            }
                
                
            
            break;
        
        case TEST_STA_IO_MANUAL:
            
            break;
            
        default :
            break;
    }
    
}    

#endif /* #if BOARDCFG_USE_TESTPROC */
