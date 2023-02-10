#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#ifndef _CSMON_LIB_SUPPORT_CONDITIONALS_H_
#define _CSMON_LIB_SUPPORT_CONDITIONALS_H_

#include <csmon.h>


/* *****************************************************************************
 * Functions
 **************************************************************************** */

#if CSMON_REALADR_16BIT
#   define CSMON_LIB_SUPPORT_eSetParameterListRealAddress CSMON_LIB_SUPPORT_eSetParameterListRealAddress16
#else
#   define CSMON_LIB_SUPPORT_eSetParameterListRealAddress CSMON_LIB_SUPPORT_eSetParameterListRealAddress32
#endif


#endif /* _CSMON_LIB_SUPPORT_CONDITIONALS_H_ */

#ifdef __cplusplus
}
#endif /* __cplusplus */
