#include "csmon_lib_support.h"

#include "datetime.h"


extern bool MAIN_bDateTimeSet = false;

extern MAIN_sDateTime_t MAIN_sDateTimeGet =
{
    0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00 /* 2001-01-01-Mon-00:00:00 */
};


extern MAIN_sDateTime_t MAIN_sDateTimeSet =
{
    BUILD_RTC_SEC,
    BUILD_RTC_MIN,
    BUILD_RTC_HOUR,
    0x00,
    BUILD_RTC_DAY,
    BUILD_RTC_MONTH,
    BUILD_RTC_YEAR,
    0x00,
    /* 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00 2001-01-01-Mon-00:00:00 */
};


/* *****************************************************************************
 * CSMON_vSetDateTime - Set RTC Value - called from CSMON library
 **************************************************************************** */
extern void CSMON_vSetDateTime (
        const uint_least8_t u8BCDSeconds,
        const uint_least8_t u8BCDMinutes,
        const uint_least8_t u8BCDHours,
        const uint_least8_t u8BCDWeekdays,
        const uint_least8_t u8BCDDay,
        const uint_least8_t u8BCDMonth,
        const uint_least8_t u8BCDYear)
{
    MAIN_DateTime_set(&MAIN_sDateTimeSet, u8BCDSeconds, u8BCDMinutes, u8BCDHours, u8BCDWeekdays, u8BCDDay, u8BCDMonth, u8BCDYear);
    MAIN_bDateTimeSet = true;
}


/* *****************************************************************************
 * CSMON_vGetDateTime - Get RTC Value - called from CSMON library
 **************************************************************************** */
extern void CSMON_vGetDateTime (
        uint_least8_t* const pu8BCDSeconds,
        uint_least8_t* const pu8BCDMinutes,
        uint_least8_t* const pu8BCDHours,
        uint_least8_t* const pu8BCDWeekdays,
        uint_least8_t* const pu8BCDDay,
        uint_least8_t* const pu8BCDMonth,
        uint_least8_t* const pu8BCDYear)
{
    *pu8BCDSeconds  = MAIN_sDateTimeGet.u8Seconds;
    *pu8BCDMinutes  = MAIN_sDateTimeGet.u8Minutes;
    *pu8BCDHours    = MAIN_sDateTimeGet.u8Hours;
    *pu8BCDWeekdays = MAIN_sDateTimeGet.u8Weekdays;
    *pu8BCDDay      = MAIN_sDateTimeGet.u8Day;
    *pu8BCDMonth    = MAIN_sDateTimeGet.u8Month;
    *pu8BCDYear     = MAIN_sDateTimeGet.u8Year;
}
