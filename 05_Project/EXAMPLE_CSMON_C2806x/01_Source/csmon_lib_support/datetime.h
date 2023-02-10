/* *****************************************************************************
 * File:   template.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 08 14 12:51
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
#ifndef DATETIME_H
#define	DATETIME_H

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

// Example of __DATE__ string: "Jul 27 2012"
//                              01234567890
    
    
    

#define BUILD_YEAR_CH0 (uint32_t)(__DATE__[ 7])
#define BUILD_YEAR_CH1 (uint32_t)(__DATE__[ 8])
#define BUILD_YEAR_CH2 (uint32_t)(__DATE__[ 9])
#define BUILD_YEAR_CH3 (uint32_t)(__DATE__[10])


#define BUILD_MONTH_IS_JAN (__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n')
#define BUILD_MONTH_IS_FEB (__DATE__[0] == 'F')
#define BUILD_MONTH_IS_MAR (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r')
#define BUILD_MONTH_IS_APR (__DATE__[0] == 'A' && __DATE__[1] == 'p')
#define BUILD_MONTH_IS_MAY (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y')
#define BUILD_MONTH_IS_JUN (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n')
#define BUILD_MONTH_IS_JUL (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l')
#define BUILD_MONTH_IS_AUG (__DATE__[0] == 'A' && __DATE__[1] == 'u')
#define BUILD_MONTH_IS_SEP (__DATE__[0] == 'S')
#define BUILD_MONTH_IS_OCT (__DATE__[0] == 'O')
#define BUILD_MONTH_IS_NOV (__DATE__[0] == 'N')
#define BUILD_MONTH_IS_DEC (__DATE__[0] == 'D')


#define BUILD_MONTH_CH0 \
        (uint32_t)((BUILD_MONTH_IS_OCT || BUILD_MONTH_IS_NOV || BUILD_MONTH_IS_DEC) ? '1' : '0')

#define BUILD_MONTH_CH1 \
        (uint32_t)( \
        (BUILD_MONTH_IS_JAN) ? '1' : \
        (BUILD_MONTH_IS_FEB) ? '2' : \
        (BUILD_MONTH_IS_MAR) ? '3' : \
        (BUILD_MONTH_IS_APR) ? '4' : \
        (BUILD_MONTH_IS_MAY) ? '5' : \
        (BUILD_MONTH_IS_JUN) ? '6' : \
        (BUILD_MONTH_IS_JUL) ? '7' : \
        (BUILD_MONTH_IS_AUG) ? '8' : \
        (BUILD_MONTH_IS_SEP) ? '9' : \
        (BUILD_MONTH_IS_OCT) ? '0' : \
        (BUILD_MONTH_IS_NOV) ? '1' : \
        (BUILD_MONTH_IS_DEC) ? '2' : \
        /* error default */    '?' \
    )

#define BUILD_DAY_CH0 (uint32_t)((__DATE__[4] >= '0') ? (__DATE__[4]) : '0')
#define BUILD_DAY_CH1 (uint32_t)(__DATE__[ 5])


// Example of __TIME__ string: "21:06:19"
//                              01234567

#define BUILD_HOUR_CH0  (uint32_t)(__TIME__[0])
#define BUILD_HOUR_CH1  (uint32_t)(__TIME__[1])

#define BUILD_MIN_CH0   (uint32_t)(__TIME__[3])
#define BUILD_MIN_CH1   (uint32_t)(__TIME__[4])

#define BUILD_SEC_CH0   (uint32_t)(__TIME__[6])
#define BUILD_SEC_CH1   (uint32_t)(__TIME__[7])


#define BUILD_SEC_NM0   ((uint32_t)BUILD_SEC_CH0 - 0x30)
#define BUILD_SEC_NM1   ((uint32_t)BUILD_SEC_CH1 - 0x30)

#define BUILD_MIN_NM0   ((uint32_t)BUILD_MIN_CH0 - 0x30)
#define BUILD_MIN_NM1   ((uint32_t)BUILD_MIN_CH1 - 0x30)

#define BUILD_HOUR_NM0  ((uint32_t)BUILD_HOUR_CH0 - 0x30)
#define BUILD_HOUR_NM1  ((uint32_t)BUILD_HOUR_CH1 - 0x30)

#define BUILD_DAY_NM0   ((uint32_t)BUILD_DAY_CH0 - 0x30)
#define BUILD_DAY_NM1   ((uint32_t)BUILD_DAY_CH1 - 0x30)

#define BUILD_MONTH_NM0 ((uint32_t)BUILD_MONTH_CH0 - 0x30)
#define BUILD_MONTH_NM1 ((uint32_t)BUILD_MONTH_CH1 - 0x30)

#define BUILD_YEAR_NM0  ((uint32_t)BUILD_YEAR_CH0 - 0x30)
#define BUILD_YEAR_NM1  ((uint32_t)BUILD_YEAR_CH1 - 0x30)
#define BUILD_YEAR_NM2  ((uint32_t)BUILD_YEAR_CH2 - 0x30)
#define BUILD_YEAR_NM3  ((uint32_t)BUILD_YEAR_CH3 - 0x30)

#define BUILD_RTC_SEC   (uint_least8_t)( (BUILD_SEC_NM0 * 16)  + (BUILD_SEC_NM1) )
#define BUILD_RTC_MIN   (uint_least8_t)( (BUILD_MIN_NM0 * 16)  + (BUILD_MIN_NM1) )
#define BUILD_RTC_HOUR  (uint_least8_t)( (BUILD_HOUR_NM0 * 16) + (BUILD_HOUR_NM1) )

#define BUILD_RTC_DAY   ( (BUILD_DAY_NM0 * 16)  + (BUILD_DAY_NM1) )
#define BUILD_RTC_MONTH ( (BUILD_MONTH_NM0 * 16)  + (BUILD_MONTH_NM1) )
#define BUILD_RTC_YEAR  ( (BUILD_YEAR_NM2 * 16) + (BUILD_YEAR_NM3) )

#define BUILD_SEC       ( (BUILD_SEC_NM0 * 10)  + (BUILD_SEC_NM1) )
#define BUILD_MIN       ( (BUILD_MIN_NM0 * 10)  + (BUILD_MIN_NM1) )
#define BUILD_HOUR      ( (BUILD_HOUR_NM0 * 10) + (BUILD_HOUR_NM1) )

#define BUILD_DAY       ( (BUILD_DAY_NM0 * 10)  + (BUILD_DAY_NM1) )
#define BUILD_MONTH     ( (BUILD_MONTH_NM0 * 10)  + (BUILD_MONTH_NM1) )
#define BUILD_YEAR      ( (BUILD_YEAR_NM0 * 1000) + (BUILD_YEAR_NM1 * 100) + (BUILD_YEAR_NM2 * 10) + (BUILD_YEAR_NM3) )

#define BUILD_TIME_SEC  ( (BUILD_HOUR * 3600) +   (BUILD_MIN * 60) + (BUILD_SEC) )

#define BUILD_DATE_DAY  ( (BUILD_YEAR * 366)  + (BUILD_MONTH * 31) + (BUILD_DAY) )


#define DATETIME_BUILD  (uint32_t)( (BUILD_DATE_DAY * 3600 * 24) + BUILD_TIME_SEC )


/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
 




#endif	/* DATETIME_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
