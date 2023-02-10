#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _01_SOURCE_CSMON_LIB_SUPPORT_PARAMS_INTERNAL_CSMON_LIB_SUPPORT_PARAMETER_LIST_H_
#define _01_SOURCE_CSMON_LIB_SUPPORT_PARAMS_INTERNAL_CSMON_LIB_SUPPORT_PARAMETER_LIST_H_


#include "MotorParam/Generated/csmon_config.h"


#pragma diag_push
#pragma diag_error 48
#define CSMON_LIB_SUPPORT_eSetParameterList CSMON_LIB_SUPPORT_eSetParameterListInternal
#pragma diag_pop


extern void CSMON_LIB_SUPPORT_eSetParameterListInternal(
        const volatile MAIN_sParameterList_t* const pParameterList,
        const size_t maxParameterCount
        );

#endif /* _01_SOURCE_CSMON_LIB_SUPPORT_PARAMS_INTERNAL_CSMON_LIB_SUPPORT_PARAMETER_LIST_H_ */

#ifdef __cplusplus
}
#endif /* __cplusplus */
