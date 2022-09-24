/* *****************************************************************************
 * File:   testproc.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 08 20 17:11
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
#ifndef TESTPROC_H
#define	TESTPROC_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdbool.h>
#include <stdint.h>
//#include "stdextend.h"

#include "boardcfg.h"    
#include "sys_drv.h"
    
    
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
typedef enum
{
    TEST_STA_IDLE,
    TEST_STA_INIT,
    TEST_STA_CALIB,
    TEST_STA_READY,
    TEST_STA_DISCH_RELEASE, 
    TEST_STA_PRECHARGE,
    TEST_STA_PRECHARGE_DELAY,
    TEST_STA_MC_CLOSE, 
    TEST_STA_PRECHARGE_OPEN,
    TEST_STA_WAIT,
    TEST_STA_TEST,
    TEST_STA_STOP,
    TEST_STA_ERROR,
    TEST_STA_IO_MANUAL,
    TEST_STA_COUNT
}TESTPROC_eState_t;


typedef enum
{
    TEST_ERR_NO_ERROR,
    TEST_ERR_DISCH_RELEASE,
    TEST_ERR_TIMEOUT_PRECHARGE,
    TEST_ERR_TIMEOUT_MAIN_CONTACT,
    TEST_ERR_TIMEOUT_DC_LEVEL,
    TEST_ERR_CONTROL,
    TEST_ERR_DETECTION,
    TEST_ERR_COUNT
}TESTPROC_eErrorState_t;

typedef enum
{
    REQUEST_STOP,
    REQUEST_RUN,
    REQUEST_IDLE,
    REQUEST_COUNT,
}TESTPROC_eRunRequest_t;


#if BOARDCFG_USE_TESTPROC
/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 
extern bool bVoltageApplied;
extern bool bVoltageDischarging;
extern uint16_t sh_u_dcl_set;

extern TESTPROC_eErrorState_t eTestProcErrorState;
extern bool bStopTest;
extern bool bStartTest;
extern bool bStartTestReserved;
extern bool bStartTestCalibrationOnlyNextStartTest;
extern bool bStartTestCalibrationIfNeeded;
extern bool bTestProcessErrorDetection;
extern bool bTestCalibrationCompleted;
extern TESTPROC_eRunRequest_t eTestRunRequest;
extern bool bTestRunStatus;
extern bool bTestRunStatusFbk;
extern bool bClearFaultRequest;
extern bool bClearFaultStatus;
extern bool bTestErrorStatus;
extern bool bTestDummyRequest;
extern bool bTestDummyStatus;
extern uint8_t u8LastStopTestID;

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
TESTPROC_eState_t TESTPROC_eGetState(void);
void TESTPROC_vProcess(void);
void TESTPROC_vStackUsageMaskingProcess(void);
void TESTPROC_vClearFaultRequestChangeProcess(void);
void TESTPROC_vTestRunRequestChangeProcess(void);
void TESTPROC_vTestDummyRequestChangeProcess(void);
void TESTPROC_vProcessVoltageRegulaion(void);

#else

#define TESTPROC_vProcess()
#define TESTPROC_vProcessVoltageRegulaion()

#endif  /* #if BOARDCFG_USE_TESTPROC */



#endif	/* TESTPROC_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
