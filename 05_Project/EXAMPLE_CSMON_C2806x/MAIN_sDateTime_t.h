#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Guard condition file contents not included more than once */
/* #pragma once */
#ifndef MAIN_SDATETIMET_H
#define MAIN_SDATETIMET_H

#include <stdint.h>

typedef struct _MAIN_sDateTime_t_
{
    uint_least8_t u8Seconds;
    uint_least8_t u8Minutes;
    uint_least8_t u8Hours;
    uint_least8_t u8Weekdays;
    uint_least8_t u8Day;
    uint_least8_t u8Month;
    uint_least8_t u8Year;
    uint_least8_t u8Reserved;
} MAIN_sDateTime_t;


#endif  /* MAIN_SDATETIMET_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
