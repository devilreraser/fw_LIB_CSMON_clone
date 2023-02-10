
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#ifndef _CSMON_LIB_SUPPORT_H_
#define _CSMON_LIB_SUPPORT_H_

#include "csmon_lib_support_conditionals.h"
#include "csmon_lib_support_sDateTime_t.h"
#include "csmon_lib_support_responses.h"

#include "csmon_lib_support_parameter_list.h"

#include <stdbool.h>


/* *****************************************************************************
 * Types
 **************************************************************************** */

typedef uint16_t recorderIndex_t;
typedef uint16_t parameterID_t;
typedef uint16_t parameterPos_t;



extern CSMON_LIB_SUPPORT_Responses_t csMonResponses;

extern bool CSMON_LIB_SUPPORT_bDateTimeSet;

extern CSMON_LIB_SUPPORT_sDateTime_t CSMON_LIB_SUPPORT_sDateTimeGet;

extern CSMON_LIB_SUPPORT_sDateTime_t CSMON_LIB_SUPPORT_sDateTimeSet;


/* *****************************************************************************
 * Prototypes of functions definitions
 **************************************************************************** */

/* CSMON_LIB_SUPPORT_init */
extern void CSMON_LIB_SUPPORT_init(void);

/* CsMonRecordersInitialization()
 * Application specific implementation */
extern void CsMonRecordersInitialization();


extern void CSMON_LIB_SUPPORT_eSetParameterListCommon(
        const volatile MAIN_sParameterList_t* const pParameterList,
        const size_t maxParameterCount
        );


/* *****************************************************************************
 * Functions definitions
 **************************************************************************** */

static inline CSMON_eResponseCode_t CSMON_LIB_SUPPORT_eSetParameterListRealAddress16(
        const volatile uint16_t* const pRealAddress16,
        const size_t offset,
        const uint16_t u16MaxParameterCount
        )
{
    uint16_t* const pRealAddress = (uint16_t*)pRealAddress16;
    const uint16_t u16Offset = (uint16_t)offset;
    return CSMON_eSetParameterListRealAddress(pRealAddress, u16Offset, u16MaxParameterCount);
}


static inline CSMON_eResponseCode_t CSMON_LIB_SUPPORT_eSetParameterListRealAddress32(
        const volatile uint32_t* const pRealAddress32,
        const size_t offset,
        const uint16_t u16MaxParameterCount
        )
{
    uint16_t* const pRealAddress = (uint16_t*)pRealAddress32;
    const uint16_t u16Offset = (uint16_t)offset;
    return CSMON_eSetParameterListRealAddress(pRealAddress, u16Offset, u16MaxParameterCount);
}

#endif /* _CSMON_LIB_SUPPORT_H_ */

#ifdef __cplusplus
}
#endif /* __cplusplus */
