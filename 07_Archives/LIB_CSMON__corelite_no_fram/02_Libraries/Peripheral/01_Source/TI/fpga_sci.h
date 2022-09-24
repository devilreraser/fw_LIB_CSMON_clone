/* *****************************************************************************
 * File:   fpga_sci.h
 * Author: Dimitar Lilov
 *
 * Created on 2020 01 17 12:36
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
#ifndef FPGA_SCI_H
#define	FPGA_SCI_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdbool.h>
#include <stdint.h>
#include "stdextend.h"
#include "sci_drv.h"


/* *****************************************************************************
 * Configuration Definitions
 * ************************************************************************** */
#define RX_TOUT u32uSecToTimerTicks(50000)        //was 100 // 100*500us = 50ms

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */
#define ADR_DSP     0x01
#define ADR_FPGA    0x02

#define CMD_RTC     0x00
#define CMD_ERR     0x01
#define CMD_MSK     0x02
#define CMD_TMP     0x03
#define CMD_CNT     0x04

/* Error */
#define NO_ERROR    1
#define ERROR       0

/* Detection */
#define DETECTED    1
#define MISSING     0

/* Mask */
#define FLT_ACTIVE  1
#define FLT_MASKED  0

#define MASK_ERRORS_BITS 9
#define MASK_ERRORS ((1 << MASK_ERRORS_BITS)-1)
#define MASK_GLOBAL_BITS 10
#define MASK_GLOBAL ((1 << MASK_GLOBAL_BITS)-1)

#define FPGA_SCI_USE_TEMP_TRIP_DRIVER_GLOBAL    1

#define FPGA_SCI_USE_TEMP_TRIP_DRIVER1  FPGA_SCI_USE_TEMP_TRIP_DRIVER_GLOBAL
#define FPGA_SCI_USE_TEMP_TRIP_DRIVER2  FPGA_SCI_USE_TEMP_TRIP_DRIVER_GLOBAL
#define FPGA_SCI_USE_TEMP_TRIP_DRIVER3  FPGA_SCI_USE_TEMP_TRIP_DRIVER_GLOBAL
#define FPGA_SCI_USE_TEMP_TRIP_DRIVER4  FPGA_SCI_USE_TEMP_TRIP_DRIVER_GLOBAL
#define FPGA_SCI_USE_TEMP_TRIP_DRIVER5  FPGA_SCI_USE_TEMP_TRIP_DRIVER_GLOBAL
#define FPGA_SCI_USE_TEMP_TRIP_DRIVER6  FPGA_SCI_USE_TEMP_TRIP_DRIVER_GLOBAL
#define FPGA_SCI_USE_TEMP_TRIP_DRIVER7  FPGA_SCI_USE_TEMP_TRIP_DRIVER_GLOBAL
#define FPGA_SCI_USE_TEMP_TRIP_DRIVER8  FPGA_SCI_USE_TEMP_TRIP_DRIVER_GLOBAL
#define FPGA_SCI_USE_TEMP_TRIP_DRIVER9  FPGA_SCI_USE_TEMP_TRIP_DRIVER_GLOBAL

#define FPGA_SCI_USE_TEMP_TRIP_BOARD    0
#define FPGA_SCI_USE_TEMP_TRIP_HUMIDITY 0


/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
typedef struct
{
    uint16_t DrvError1 : 1;
    uint16_t DrvError2 : 1;
    uint16_t DrvError3 : 1;
    uint16_t DrvError4 : 1;
    uint16_t DrvError5 : 1;
    uint16_t DrvError6 : 1;
    uint16_t DrvError7 : 1;
    uint16_t DrvError8 : 1;
    uint16_t DrvError9 : 1;
    uint16_t OVError1  : 1;
} FPGA_SCI_sDrvError;

typedef union
{
    uint16_t u16Register;
    FPGA_SCI_sDrvError sDrvError;
} FPGA_SCI_uDrvError;



typedef struct
{
    uint16_t DrvError1 : 1;
    uint16_t DrvError2 : 1;
    uint16_t DrvError3 : 1;
    uint16_t DrvError4 : 1;
    uint16_t DrvError5 : 1;
    uint16_t DrvError6 : 1;
    uint16_t DrvError7 : 1;
    uint16_t DrvError8 : 1;
    uint16_t DrvError9 : 1;
    uint16_t OvtBoard  : 1;
    uint16_t Humidity  : 1;
} FPGA_SCI_sTmpError;

typedef union
{
    uint16_t u16Register;
    FPGA_SCI_sTmpError sTmpError;
} FPGA_SCI_uTmpError;

typedef struct
{
    uint16_t u8Day : 8;
    uint16_t u8Month : 8;
    uint16_t u8Year : 8;

    uint16_t b2TimeSyncSampleMode          : 2;    /* HMRECPRM_eTimeSyncSampleMode_t */
    uint16_t b5Reserved                    : 5;
    uint16_t bMicroSec0BuffOvf1Mode        : 1;    /* HMRECPRM_eMicroSec0BuffOvf1Mode_t */
}FPGA_SCI_sTimeSyncModeYYMMDD_t;

typedef struct
{
    uint16_t u8Second : 8;
    uint16_t u8Minute : 8;
    uint16_t u8Hour : 8;
    uint16_t u8WeekDays : 8;
}FPGA_SCI_sTimeSyncWWHHMMSS_t;


/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */

bool FPGA_SCI_bGetRTCGet(void);
void FPGA_SCI_vErrorMasksSetup(void);
bool FPGA_SCI_vClockSetup(void);
void FPGA_SCI_vProcess(void);
void FPGA_SCI_vUartOverflowDetection(void);
void FPGA_SCI_vProcessUartTx(uint8_t u8Index);
void FPGA_SCI_vProcessUartRx(void);
void FPGA_SCI_vProcessFastTD (uint8_t u8Index);
void FPGA_SCI_vProcessTest(SCI_eModule_t eUartFPGARW);
void FPGA_SCI_vClearTrippedErrors (void);
bool FPGA_SCI_bIsError (void);
bool FPGA_SCI_bIsErrorTemp (void);
bool FPGA_SCI_bIsErrorDet (void);
bool FPGA_SCI_bIsErrorDrv (void);
bool FPGA_SCI_bIsErrorOvt (void);
bool FPGA_SCI_bIsErrorOcn (void);
bool FPGA_SCI_bIsErrorOcp (void);
bool FPGA_SCI_bIsErrorVdc (void);
bool FPGA_SCI_bIsErrorGlb (void);

void FPGA_SCI_vSetStackUsed(uint16_t nStack);
void FPGA_SCI_vSetStackDeny(uint16_t nStack);

FPGA_SCI_sTimeSyncModeYYMMDD_t FPGA_SCI_GetDate (void);
FPGA_SCI_sTimeSyncWWHHMMSS_t FPGA_SCI_GetTime (void);



void FPGA_SCI_vSetDateTime (
        uint_least8_t u8BCDSeconds,
        uint_least8_t u8BCDMinutes,
        uint_least8_t u8BCDHours,
        uint_least8_t u8BCDWeekdays,
        uint_least8_t u8BCDDay,
        uint_least8_t u8BCDMonth,
        uint_least8_t u8BCDYear);

void FPGA_SCI_vGetDateTime (
        uint_least8_t* pu8BCDSeconds,
        uint_least8_t* pu8BCDMinutes,
        uint_least8_t* pu8BCDHours,
        uint_least8_t* pu8BCDWeekdays,
        uint_least8_t* pu8BCDDay,
        uint_least8_t* pu8BCDMonth,
        uint_least8_t* pu8BCDYear);

#endif	/* FPGA_SCI_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
