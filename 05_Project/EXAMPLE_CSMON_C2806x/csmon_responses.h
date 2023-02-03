#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#ifndef _CSMON_RESPONSES_H_
#define _CSMON_RESPONSES_H_


#include "csmon.h"

typedef struct _CSMON_Responses_t_
{
    CSMON_eResponseCode_t eInit;
    CSMON_eResponseCode_t eProcess;
    CSMON_eResponseCode_t eSetServerOnStatus;
    CSMON_eResponseCode_t eSetRecorder;
    CSMON_eResponseCode_t eSetFlagProcessPassed;
} CSMON_Responses_t;


typedef CSMON_Responses_t* CSMON_Responses_Handle;
typedef const CSMON_Responses_t* CSMON_Responses_CHandle;


static inline void CSMON_Responses_init(const CSMON_Responses_Handle handle)
{
    CSMON_Responses_t* const obj = (CSMON_Responses_t*)handle;

    obj->eInit = CSMON_RESPONSE_CODE_OK;
    obj->eProcess = CSMON_RESPONSE_CODE_OK;
    obj->eSetServerOnStatus = CSMON_RESPONSE_CODE_OK;
    obj->eSetRecorder = CSMON_RESPONSE_CODE_OK;
    obj->eSetFlagProcessPassed = CSMON_RESPONSE_CODE_OK;
}

#endif /* _CSMON_LIB_SUPPORT_H_ */

#ifdef __cplusplus
}
#endif /* __cplusplus */
