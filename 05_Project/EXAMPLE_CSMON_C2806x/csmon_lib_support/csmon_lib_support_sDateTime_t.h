#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Guard condition file contents not included more than once */
/* #pragma once */
#ifndef _CSMON_LIB_SUPPORT_SDATETIMET_H_
#define _CSMON_LIB_SUPPORT_SDATETIMET_H_

#include <stdint.h>

typedef struct _CSMON_LIB_SUPPORT_sDateTime_t_
{
    uint_least8_t u8Seconds;
    uint_least8_t u8Minutes;
    uint_least8_t u8Hours;
    uint_least8_t u8Weekdays;
    uint_least8_t u8Day;
    uint_least8_t u8Month;
    uint_least8_t u8Year;
    uint_least8_t u8Reserved;
} CSMON_LIB_SUPPORT_sDateTime_t;


typedef CSMON_LIB_SUPPORT_sDateTime_t* CSMON_LIB_SUPPORT_sDateTime_Handle;
typedef const CSMON_LIB_SUPPORT_sDateTime_t* CSMON_LIB_SUPPORT_sDateTime_CHandle;


static inline void CSMON_LIB_SUPPORT_DateTime_set (
        const CSMON_LIB_SUPPORT_sDateTime_Handle handle,
        const uint_least8_t u8BCDSeconds,
        const uint_least8_t u8BCDMinutes,
        const uint_least8_t u8BCDHours,
        const uint_least8_t u8BCDWeekdays,
        const uint_least8_t u8BCDDay,
        const uint_least8_t u8BCDMonth,
        const uint_least8_t u8BCDYear)
{
    CSMON_LIB_SUPPORT_sDateTime_t* const obj = (CSMON_LIB_SUPPORT_sDateTime_t*)handle;

    obj->u8Seconds    = u8BCDSeconds;
    obj->u8Minutes    = u8BCDMinutes;
    obj->u8Hours      = u8BCDHours;
    obj->u8Weekdays   = u8BCDWeekdays;
    obj->u8Day        = u8BCDDay;
    obj->u8Month      = u8BCDMonth;
    obj->u8Year       = u8BCDYear;
 //   MAIN_bDateTimeSet = true;
}


static inline void CSMON_LIB_SUPPORT_DateTime_get (
        const CSMON_LIB_SUPPORT_sDateTime_CHandle handle,
        uint_least8_t* const pu8BCDSeconds,
        uint_least8_t* const pu8BCDMinutes,
        uint_least8_t* const pu8BCDHours,
        uint_least8_t* const pu8BCDWeekdays,
        uint_least8_t* const pu8BCDDay,
        uint_least8_t* const pu8BCDMonth,
        uint_least8_t* const pu8BCDYear)
{
    const CSMON_LIB_SUPPORT_sDateTime_t* const obj = (const CSMON_LIB_SUPPORT_sDateTime_t*)handle;

    *pu8BCDSeconds  = obj->u8Seconds;
    *pu8BCDMinutes  = obj->u8Minutes;
    *pu8BCDHours    = obj->u8Hours;
    *pu8BCDWeekdays = obj->u8Weekdays;
    *pu8BCDDay      = obj->u8Day;
    *pu8BCDMonth    = obj->u8Month;
    *pu8BCDYear     = obj->u8Year;
}


#endif  /* _CSMON_LIB_SUPPORT_SDATETIMET_H_ */


#ifdef __cplusplus
}
#endif /* __cplusplus */
