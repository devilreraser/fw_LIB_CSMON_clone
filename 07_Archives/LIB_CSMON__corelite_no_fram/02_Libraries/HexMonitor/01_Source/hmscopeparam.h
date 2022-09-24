/* *****************************************************************************
 * File:   hmscopeparam.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 11 03 12:54
 * 
 * Description: Hex Monitor Scope Parameters
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef HMSCOPEPRM_H
#define	HMSCOPEPRM_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "hmprotocol.h"
#include "hmscopeparam_t.h"

/*****************************************************************************
* Scope support
******************************************************************************/


/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Variable External Usage Definitions
 **************************************************************************** */
#if HMDRV_SCOPE_PARAM_HANDLE_COUNT > 0
extern HMSCOPEPRM_sHandle_t sScopeHandlePrm[HMDRV_SCOPE_PARAM_HANDLE_COUNT];
#endif

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
#if HMDRV_USE_SCOPE_PARAMETERS
void HMSCOPEPRM_vInit(void);
HMDRV_BPTR HMSCOPEPRM_ReadScope(HMDRV_U8 nScopeIndex, HMDRV_BPTR pMessageIO);
HMPROTOCOL_eResponseCode_t HMSCOPEPRM_SetUpScope(HMDRV_U8 scopeIndex, HMDRV_BPTR pMessageIO);
void HMSCOPEPRM_vSetDataChangeFlag(void);


#else
#define HMSCOPEPRM_vInit()
#define HMSCOPEPRM_ReadScope(i,x) x
#define HMSCOPEPRM_SetUpScope(i,x) 0
#define HMSCOPEPRM_vSetDataChangeFlag()
#endif

#endif	/* HMSCOPEPRM_H */
