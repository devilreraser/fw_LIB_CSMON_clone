/* *****************************************************************************
 * File:   rtc_drv.c
 * Author: Dimitar Lilov
 *
 * Created on 2020 03 08 17:03
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
#include "rtc_drv.h"
#include "i2cmem.h"

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

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */
   uint8_t u8RTCSEC;
    uint8_t u8RTCMIN;
    uint8_t u8RTCHOUR;
    uint8_t u8RTCWKDAY;
    uint8_t u8RTCDATE;
    uint8_t u8RTCMTH;
    uint8_t u8RTCYEAR;
    uint8_t u8RTCCONTROL;
    uint8_t u8RTCOSCTRIM;        /* unsigned trim value */
    uint8_t u8EEUNLOCK;

#ifndef FPGA_SCI_H
RTC_DRV_sTimeSyncModeYYMMDD_t RTC_DRV_GetDate(void)
{
    RTC_DRV_sTimeSyncModeYYMMDD_t sDate;
    
    sDate.u8Day     = RTCC.uTimekeeping.sTimekeepReg.u8RTCDATE;
    sDate.u8Month   = RTCC.uTimekeeping.sTimekeepReg.u8RTCMTH;
    sDate.u8Year    = RTCC.uTimekeeping.sTimekeepReg.u8RTCYEAR;
    
    return sDate;
}

RTC_DRV_sTimeSyncWWHHMMSS_t RTC_DRV_GetTime(void)
{
    RTC_DRV_sTimeSyncWWHHMMSS_t sTime;
    
    sTime.u8Second  = RTCC.uTimekeeping.sTimekeepReg.u8RTCSEC;
    sTime.u8Minute  = RTCC.uTimekeeping.sTimekeepReg.u8RTCMIN;
    sTime.u8Hour    = RTCC.uTimekeeping.sTimekeepReg.u8RTCHOUR;
    sTime.u8WeekDays= RTCC.uTimekeeping.sTimekeepReg.u8RTCWKDAY; /* 2001 01 01 Monday */
    
    return sTime;
}
#endif