/* *****************************************************************************
 * File:   rtc_drv.h
 * Author: Dimitar Lilov
 *
 * Created on 2020 03 08 17:03
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
#ifndef RTC_DRV_H
#define	RTC_DRV_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdint.h>
#ifndef __XC16__
#include "sys_drv.h"
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
    
#if 1
typedef struct
{
    uint_least8_t u8Seconds;
    uint_least8_t u8Minutes;
    uint_least8_t u8Hours;
    uint_least8_t u8Weekdays;
    uint_least8_t u8Day;
    uint_least8_t u8Month;
    uint_least8_t u8Year;
    uint_least8_t u8Reserved;
}RTC_DRV_sDateTime_t;
#else
typedef struct
{
    uint8_t u8Seconds;
    uint8_t u8Minutes;
    uint8_t u8Hours;
    uint8_t u8Weekdays;
    uint8_t u8Day;
    uint8_t u8Month;
    uint8_t u8Year;
    uint8_t u8Reserved;
}RTC_DRV_sDateTime_t;
#endif

    
#ifndef FPGA_SCI_H
typedef struct
{
    uint16_t u8Day : 8;
    uint16_t u8Month : 8;
    uint16_t u8Year : 8;

    uint16_t b2TimeSyncSampleMode          : 2;    /* HMRECPRM_eTimeSyncSampleMode_t */
    uint16_t b5Reserved                    : 5;
    uint16_t bMicroSec0BuffOvf1Mode        : 1;    /* HMRECPRM_eMicroSec0BuffOvf1Mode_t */
}RTC_DRV_sTimeSyncModeYYMMDD_t;

typedef struct
{
    uint16_t u8Second : 8;
    uint16_t u8Minute : 8;
    uint16_t u8Hour : 8;
    uint16_t u8WeekDays : 8;
}RTC_DRV_sTimeSyncWWHHMMSS_t;


#else
#define RTC_DRV_sTimeSyncModeYYMMDD_t FPGA_SCI_sTimeSyncModeYYMMDD_t
#define RTC_DRV_sTimeSyncWWHHMMSS_t     FPGA_SCI_sTimeSyncWWHHMMSS_t
#endif

/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void RTC_DRV_vInit(void);

#ifndef FPGA_SCI_H
RTC_DRV_sTimeSyncModeYYMMDD_t   RTC_DRV_GetDate(void);
RTC_DRV_sTimeSyncWWHHMMSS_t     RTC_DRV_GetTime(void);
#else
#define RTC_DRV_GetDate()  FPGA_SCI_GetDate()  
#define RTC_DRV_GetTime()  FPGA_SCI_GetTime()  
#endif


/* *****************************************************************************
 * RTC_DRV_vSetDateTime - Set RTC Value - called from CSMON library compatible
 **************************************************************************** */
void RTC_DRV_vSetDateTime (
        uint_least8_t u8BCDSeconds,
        uint_least8_t u8BCDMinutes,
        uint_least8_t u8BCDHours,
        uint_least8_t u8BCDWeekdays,
        uint_least8_t u8BCDDay,
        uint_least8_t u8BCDMonth,
        uint_least8_t u8BCDYear);

/* *****************************************************************************
 * RTC_DRV_vGetDateTime - Get RTC Value - called from CSMON library compatible
 **************************************************************************** */
void RTC_DRV_vGetDateTime (
        uint_least8_t* pu8BCDSeconds,
        uint_least8_t* pu8BCDMinutes,
        uint_least8_t* pu8BCDHours,
        uint_least8_t* pu8BCDWeekdays,
        uint_least8_t* pu8BCDDay,
        uint_least8_t* pu8BCDMonth,
        uint_least8_t* pu8BCDYear);

/* *****************************************************************************
 * RTC_DRV_vProcess - Read Write To Real Driver Interface (I2C, SPI...)
 **************************************************************************** */
void RTC_DRV_vProcess(void);


#endif	/* RTC_DRV_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
