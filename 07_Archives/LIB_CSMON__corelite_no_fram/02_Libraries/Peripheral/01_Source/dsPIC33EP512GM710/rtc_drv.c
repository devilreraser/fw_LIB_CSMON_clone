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
#include <stdint.h>
#include <stdbool.h>
#include "rtc_drv.h"
#include "boardcfg.h"

#if BOARDCFG_USE_I2C_RTC
#include "i2cmem.h"
#endif

#include "datetime.h"
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
/* Example of __DATE__ string: "Jul 27 2012" */
/*                              01234567890 */
const char RTC_DRV_as8Date[11] = __DATE__;

/* Example of __TIME__ string: "21:06:19" */
/*                              01234567 */
const char RTC_DRV_as8Time[8] = __TIME__;

RTC_DRV_sDateTime_t RTC_DRV_sDateTimeGet =
{
    0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00 /* 2001-01-01-Mon-00:00:00 */
};
RTC_DRV_sDateTime_t RTC_DRV_sDateTimeSet =
{
#ifndef __XC16__    /* TI should work */
    BUILD_RTC_SEC,
    BUILD_RTC_MIN,
    BUILD_RTC_HOUR,
    0x00,
    BUILD_RTC_DAY,
    BUILD_RTC_MONTH,
    BUILD_RTC_YEAR,
    0x00,
#else
    0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00 /* 2001-01-01-Mon-00:00:00 */
#endif
};
bool RTC_DRV_bDateTimeSet = false;
bool RTC_DRV_bDateTimeGet = false;
bool RTC_DRV_bDateTimeSetupAtStart = false;

bool RTC_DRV_bDateTimeGetRequest = false;


/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */

bool RTC_DRV_bDateTimeSetLaterThanDateTimeGet(void)
{
    bool bResult = false;
    
    if (RTC_DRV_sDateTimeSet.u8Year > RTC_DRV_sDateTimeGet.u8Year)
    {
        bResult = true;
    }
    else if (RTC_DRV_sDateTimeSet.u8Year == RTC_DRV_sDateTimeGet.u8Year)
    {
        if (RTC_DRV_sDateTimeSet.u8Month > RTC_DRV_sDateTimeGet.u8Month)
        {
            bResult = true;
        }
        else if (RTC_DRV_sDateTimeSet.u8Month == RTC_DRV_sDateTimeGet.u8Month)
        {
            if (RTC_DRV_sDateTimeSet.u8Day > RTC_DRV_sDateTimeGet.u8Day)
            {
                bResult = true;
            }
            else if (RTC_DRV_sDateTimeSet.u8Day == RTC_DRV_sDateTimeGet.u8Day)
            {
                if (RTC_DRV_sDateTimeSet.u8Hours > RTC_DRV_sDateTimeGet.u8Hours)
                {
                    bResult = true;
                }
                else if (RTC_DRV_sDateTimeSet.u8Hours == RTC_DRV_sDateTimeGet.u8Hours)
                {
                    if (RTC_DRV_sDateTimeSet.u8Minutes > RTC_DRV_sDateTimeGet.u8Minutes)
                    {
                        bResult = true;
                    }
                    else if (RTC_DRV_sDateTimeSet.u8Minutes == RTC_DRV_sDateTimeGet.u8Minutes)
                    {
                        if (RTC_DRV_sDateTimeSet.u8Seconds > RTC_DRV_sDateTimeGet.u8Seconds)
                        {
                            bResult = true;
                        }
                        else if (RTC_DRV_sDateTimeSet.u8Seconds == RTC_DRV_sDateTimeGet.u8Seconds)
                        {

                        }

                    }

                }

            }

        }
        
    }
    return bResult;
}


uint_least8_t RTC_DRV_u8DayOfWeek_2000_2099(uint_least8_t year, uint_least8_t month, uint_least8_t day)
{

    uint32_t days;

    //static const uint_least8_t month_offset_table[] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5}; // Typical table.

    // Added 1 to Jan, Feb. Subtracted 1 from each instead of adding 6 in calc below.

    static const uint_least8_t month_offset_table[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

    // Year is 0 - 99, representing years 2000 - 2099

    // Month starts at 0.
    month--;

    // Day starts at 1.

    // Subtract 1 in calc for Jan, Feb, only in leap years.
    // Subtracting 1 from year has the effect of subtracting 2 in leap years, subtracting 1 otherwise.
    // Adding 1 for Jan, Feb in Month Table so calc ends up subtracting 1 for Jan, Feb, only in leap years.
    // All of this complication to avoid the check if it is a leap year.
    if (month < 2) {
        year--;
    }

    // Century constant is 6. Subtract 1 from Month Table, so difference is 7.

    // Sunday (0), Monday (1) ...
    days = (day + month_offset_table[month] + year + (year >> 2));

    return days % 7;

} /* end rtc_DayOfWeek() */


void RTC_DRV_vInit(void)
{
    uint_least8_t u8Weekday;
    uint_least8_t u8Day;
    uint_least8_t u8Month;
    uint_least8_t u8Year;
    
#ifdef __XC16__
    RTC_DRV_sDateTimeSet.u8Seconds = ((RTC_DRV_as8Time[6] - 0x30) << 4) + RTC_DRV_as8Time[7] - 0x30;
    RTC_DRV_sDateTimeSet.u8Minutes = ((RTC_DRV_as8Time[3] - 0x30) << 4) + RTC_DRV_as8Time[4] - 0x30;
    RTC_DRV_sDateTimeSet.u8Hours   = ((RTC_DRV_as8Time[0] - 0x30) << 4) + RTC_DRV_as8Time[1] - 0x30;
    
    RTC_DRV_sDateTimeSet.u8Day     = ((RTC_DRV_as8Date[4] - 0x30) << 4) + RTC_DRV_as8Date[5] - 0x30;
    u8Day = ((RTC_DRV_as8Date[4] - 0x30) * 10) + RTC_DRV_as8Date[5] - 0x30;
    
    RTC_DRV_sDateTimeSet.u8Year    = ((RTC_DRV_as8Date[9] - 0x30) << 4) + RTC_DRV_as8Date[10]- 0x30;
    u8Year    = ((RTC_DRV_as8Date[9] - 0x30) * 10) + RTC_DRV_as8Date[10]- 0x30;
    
    if ( (RTC_DRV_as8Date[0] == 'J') && (RTC_DRV_as8Date[1] == 'a') )
    {
        u8Month = 0;
        RTC_DRV_sDateTimeSet.u8Month    = 0x01;
    }
    else if (RTC_DRV_as8Date[0] == 'F')
    {
        u8Month = 1;
        RTC_DRV_sDateTimeSet.u8Month    = 0x02;
    }
    else if ( (RTC_DRV_as8Date[0] == 'M') && (RTC_DRV_as8Date[2] == 'r') )
    {
        u8Month = 2;
        RTC_DRV_sDateTimeSet.u8Month    = 0x03;
    }
    else if ( (RTC_DRV_as8Date[0] == 'A') && (RTC_DRV_as8Date[1] == 'p') )
    {
        u8Month = 3;
        RTC_DRV_sDateTimeSet.u8Month    = 0x04;
    }
    else if (RTC_DRV_as8Date[0] == 'M')
    {
        u8Month = 4;
        RTC_DRV_sDateTimeSet.u8Month    = 0x05;
    }
    else if ( (RTC_DRV_as8Date[0] == 'J') && (RTC_DRV_as8Date[2] == 'n') )
    {
        u8Month = 5;
        RTC_DRV_sDateTimeSet.u8Month    = 0x06;
    }
    else if (RTC_DRV_as8Date[0] == 'J')
    {
        u8Month = 6;
        RTC_DRV_sDateTimeSet.u8Month    = 0x07;
    }
    else if (RTC_DRV_as8Date[0] == 'A')
    {
        u8Month = 7;
        RTC_DRV_sDateTimeSet.u8Month    = 0x08;
    }
    else if (RTC_DRV_as8Date[0] == 'S')
    {
        u8Month = 8;
        RTC_DRV_sDateTimeSet.u8Month    = 0x09;
    }
    else if (RTC_DRV_as8Date[0] == 'O')
    {
        u8Month = 9;
        RTC_DRV_sDateTimeSet.u8Month    = 0x10;
    }
    else if (RTC_DRV_as8Date[0] == 'N')
    {
        u8Month = 10;
        RTC_DRV_sDateTimeSet.u8Month    = 0x11;
    }
    else if (RTC_DRV_as8Date[0] == 'D')
    {
        u8Month = 11;
        RTC_DRV_sDateTimeSet.u8Month    = 0x12;
    }
    
    u8Weekday = RTC_DRV_u8DayOfWeek_2000_2099(u8Year, u8Month, u8Day);
    RTC_DRV_sDateTimeSet.u8Weekdays= u8Weekday;
#else
    /* RTC_DRV_sDateTimeSet elements are Binary Coded Decimals (BCD) */
    u8Day = RTC_DRV_sDateTimeSet.u8Day & 0xF;
    u8Day += (RTC_DRV_sDateTimeSet.u8Day >> 4) * 10;

    u8Month = RTC_DRV_sDateTimeSet.u8Month & 0xF;
    u8Month += (RTC_DRV_sDateTimeSet.u8Month >> 4) * 10;

    u8Year = RTC_DRV_sDateTimeSet.u8Year & 0xF;
    u8Year += (RTC_DRV_sDateTimeSet.u8Year >> 4) * 10;


    u8Weekday = RTC_DRV_u8DayOfWeek_2000_2099(u8Year, u8Month, u8Day);
    RTC_DRV_sDateTimeSet.u8Weekdays= u8Weekday;
#endif
    
}

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
        uint_least8_t u8BCDYear)
{
    RTC_DRV_sDateTimeSet.u8Seconds    = u8BCDSeconds;
    RTC_DRV_sDateTimeSet.u8Minutes    = u8BCDMinutes;
    RTC_DRV_sDateTimeSet.u8Hours      = u8BCDHours;
    RTC_DRV_sDateTimeSet.u8Weekdays   = u8BCDWeekdays;
    RTC_DRV_sDateTimeSet.u8Day        = u8BCDDay;
    RTC_DRV_sDateTimeSet.u8Month      = u8BCDMonth;
    RTC_DRV_sDateTimeSet.u8Year       = u8BCDYear;
    RTC_DRV_bDateTimeSet = true;
}

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
        uint_least8_t* pu8BCDYear)
{
    *pu8BCDSeconds  = RTC_DRV_sDateTimeGet.u8Seconds;
    *pu8BCDMinutes  = RTC_DRV_sDateTimeGet.u8Minutes;
    *pu8BCDHours    = RTC_DRV_sDateTimeGet.u8Hours;
    *pu8BCDWeekdays = RTC_DRV_sDateTimeGet.u8Weekdays;
    *pu8BCDDay      = RTC_DRV_sDateTimeGet.u8Day;
    *pu8BCDMonth    = RTC_DRV_sDateTimeGet.u8Month;
    *pu8BCDYear     = RTC_DRV_sDateTimeGet.u8Year;
    RTC_DRV_bDateTimeGet = true;
}


/* *****************************************************************************
 * RTC_DRV_vProcess - Read Write To Real Driver Interface (I2C, SPI...)
 **************************************************************************** */
void RTC_DRV_vProcess(void)
{
    #ifdef FPGA_SCI_H
    FPGA_SCI_vGetDateTime
    (
            &RTC_DRV_sDateTimeGet.u8Seconds,
            &RTC_DRV_sDateTimeGet.u8Minutes,
            &RTC_DRV_sDateTimeGet.u8Hours,
            &RTC_DRV_sDateTimeGet.u8Weekdays,
            &RTC_DRV_sDateTimeGet.u8Day,
            &RTC_DRV_sDateTimeGet.u8Month,
            &RTC_DRV_sDateTimeGet.u8Year
    );
    #elif BOARDCFG_USE_I2C_RTC
    RTC_DRV_sDateTimeGet.u8Seconds  = RTCC.uTimekeeping.sTimekeepReg.u8RTCSEC  ;
    RTC_DRV_sDateTimeGet.u8Minutes  = RTCC.uTimekeeping.sTimekeepReg.u8RTCMIN  ;
    RTC_DRV_sDateTimeGet.u8Hours    = RTCC.uTimekeeping.sTimekeepReg.u8RTCHOUR ;
    RTC_DRV_sDateTimeGet.u8Weekdays = RTCC.uTimekeeping.sTimekeepReg.u8RTCWKDAY;/* 2001 01 01 Monday */
    RTC_DRV_sDateTimeGet.u8Day      = RTCC.uTimekeeping.sTimekeepReg.u8RTCDATE ;
    RTC_DRV_sDateTimeGet.u8Month    = RTCC.uTimekeeping.sTimekeepReg.u8RTCMTH  ;
    RTC_DRV_sDateTimeGet.u8Year     = RTCC.uTimekeeping.sTimekeepReg.u8RTCYEAR ;
    #endif
    
    #if BOARDCFG_USE_I2C_RTC
    if (I2CMEM_bGetMemStateIdle())
    #else
    #endif
    {
        if (RTC_DRV_bDateTimeSetupAtStart == false)
        {
            #ifdef FPGA_SCI_H
            if ( FPGA_SCI_bGetRTCGet())
            #else
            if (RTC_DRV_bDateTimeGetRequest)
            #endif
            {
                RTC_DRV_bDateTimeSetupAtStart = true;
                
                if (RTC_DRV_bDateTimeSetLaterThanDateTimeGet())
                {
                    RTC_DRV_bDateTimeSet = true;
                }
                
            }
        }
        if (RTC_DRV_bDateTimeSet)
        {
            #ifdef FPGA_SCI_H
            FPGA_SCI_vSetDateTime
            (
                    RTC_DRV_sDateTimeSet.u8Seconds,
                    RTC_DRV_sDateTimeSet.u8Minutes,
                    RTC_DRV_sDateTimeSet.u8Hours,
                    RTC_DRV_sDateTimeSet.u8Weekdays,
                    RTC_DRV_sDateTimeSet.u8Day,
                    RTC_DRV_sDateTimeSet.u8Month,
                    RTC_DRV_sDateTimeSet.u8Year
            );
            if (FPGA_SCI_vClockSetup())
            {
                RTC_DRV_bDateTimeSet = false;
            }


            #elif BOARDCFG_USE_I2C_RTC
            RTCC.uTimekeeping.sTimekeepReg.u8RTCSEC   = RTC_DRV_sDateTimeSet.u8Seconds ;
            RTCC.uTimekeeping.sTimekeepReg.u8RTCMIN   = RTC_DRV_sDateTimeSet.u8Minutes ; 
            RTCC.uTimekeeping.sTimekeepReg.u8RTCHOUR  = RTC_DRV_sDateTimeSet.u8Hours   ;
            RTCC.uTimekeeping.sTimekeepReg.u8RTCWKDAY = RTC_DRV_sDateTimeSet.u8Weekdays;/* 2001 01 01 Monday */
            RTCC.uTimekeeping.sTimekeepReg.u8RTCDATE  = RTC_DRV_sDateTimeSet.u8Day     ;
            RTCC.uTimekeeping.sTimekeepReg.u8RTCMTH   = RTC_DRV_sDateTimeSet.u8Month   ;
            RTCC.uTimekeeping.sTimekeepReg.u8RTCYEAR  = RTC_DRV_sDateTimeSet.u8Year    ;

            eI2CCmd = I2C_CMD_RTCC_WR;
            RTC_DRV_bDateTimeSet = false;
            #else
            /* not implemented - do nothing */
            RTC_DRV_bDateTimeSet = false;
            #endif
        }
        else
        {
            #ifdef FPGA_SCI_H

            #elif BOARDCFG_USE_I2C_RTC
            eI2CCmd = I2C_CMD_RTCC_RD;  
            #else
            /* not implemented - do nothing */
            #endif
            RTC_DRV_bDateTimeGetRequest = true;
        } 
    }
}
