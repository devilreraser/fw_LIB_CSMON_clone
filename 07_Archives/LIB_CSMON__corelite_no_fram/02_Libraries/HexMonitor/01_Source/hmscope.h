/* *****************************************************************************
 * File:   hmscope.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 01 12 10:56
 * 
 * Description: Hex Monitor Scope
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef HMSCOPE_H
#define	HMSCOPE_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "hmprotocol.h"

/*****************************************************************************
* Scope support
******************************************************************************/
#ifndef HMSCOPE_MAX_VARS
#ifdef  HMDRV_MAX_SCOPE_VARS
#define HMSCOPE_MAX_VARS     HMDRV_MAX_SCOPE_VARS
#else
#define HMSCOPE_MAX_VARS     8  /* max. number of scope variables */
#endif
#endif


/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* Scope Handle */
typedef struct
{
    HMDRV_U8    u8ScopeVarCount;                 /* number of active scope variables */
    HMDRV_ADDR  pScopeVarAddr[HMSCOPE_MAX_VARS]; /* addresses of scope variables */
    HMDRV_SIZE8 nScopeVarSize[HMSCOPE_MAX_VARS]; /* sizes of scope variables */
}HMSCOPE_sHandle_t;


/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
#if HMDRV_USE_SCOPE
void HMSCOPE_vInit(void);
HMDRV_BPTR HMSCOPE_ReadScope(HMDRV_BPTR pMessageIO);
HMPROTOCOL_eResponseCode_t HMSCOPE_SetUpScope(HMDRV_BPTR pMessageIO);
void HMSCOPE_vSetDataChangeFlag(void);
#else
#define HMSCOPE_vInit()
#define HMSCOPE_ReadScope(x) x 
#define HMSCOPE_SetUpScope(x) 0
#define HMSCOPE_vSetDataChangeFlag()
#endif

#endif	/* HMSCOPE_H */
