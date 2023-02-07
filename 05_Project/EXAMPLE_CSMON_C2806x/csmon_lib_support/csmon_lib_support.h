
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#ifndef _CSMON_LIB_SUPPORT_H_
#define _CSMON_LIB_SUPPORT_H_

#include "csmon_lib_support_sDateTime_t.h"
#include "csmon_lib_support_responses.h"

#include <stdbool.h>


/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

extern CSMON_LIB_SUPPORT_Responses_t csMonResponses;

extern bool CSMON_LIB_SUPPORT_bDateTimeSet;

extern CSMON_LIB_SUPPORT_sDateTime_t CSMON_LIB_SUPPORT_sDateTimeGet;

extern CSMON_LIB_SUPPORT_sDateTime_t CSMON_LIB_SUPPORT_sDateTimeSet;


/* *****************************************************************************
 * Prototypes of functions definitions
 **************************************************************************** */

/* CSMON_LIB_SUPPORT_init */
extern void CSMON_LIB_SUPPORT_init(void);

/* CsMonExternalParametersInitialization()
 * Application specific implementation
 * */
extern void CsMonExternalParametersInitialization();

/* CsMonRecordersInitialization()
 * Application specific implementation */
extern void CsMonRecordersInitialization();


#endif /* _CSMON_LIB_SUPPORT_H_ */

#ifdef __cplusplus
}
#endif /* __cplusplus */
