#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#ifndef _01_SOURCE_CSMON_CONFIG_TYPES_H_
#define _01_SOURCE_CSMON_CONFIG_TYPES_H_

#include <stdint.h>


typedef struct Currents_t_
{
    int16_t PhA;
    int16_t PhB;
    int16_t PhC;
} Currents_t;


#endif /* _01_SOURCE_CSMON_CONFIG_TYPES_H_ */

#ifdef __cplusplus
}
#endif /* __cplusplus */
