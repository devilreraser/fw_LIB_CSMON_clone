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
#ifndef FPGA_SCI_H
RTC_DRV_sTimeSyncModeYYMMDD_t   RTC_DRV_GetDate(void);
RTC_DRV_sTimeSyncWWHHMMSS_t     RTC_DRV_GetTime(void);
#else
#define RTC_DRV_GetDate()  FPGA_SCI_GetDate()  
#define RTC_DRV_GetTime()  FPGA_SCI_GetTime()  
#endif
#endif	/* RTC_DRV_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
