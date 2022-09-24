/* *****************************************************************************
 * File:   hmscopeparam.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 11 14 11:54
 * 
 * Description: Hex Monitor Scope Parameters
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef HMSCOPEPRM_T_H
#define	HMSCOPEPRM_T_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */


/*****************************************************************************
* Scope support
******************************************************************************/
#ifndef HMSCOPEPRM_MAX_VARS
#ifdef  HMDRV_MAX_SCOPE_PARAM_VARS
#define HMSCOPEPRM_MAX_VARS     HMDRV_MAX_SCOPE_PARAM_VARS
#else
#define HMSCOPEPRM_MAX_VARS     20  /* max. number of scope variables */
#endif
#endif




/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

typedef enum 
{
    HMSCOPEPRM_CODE_OK,
    HMSCOPEPRM_CODE_NOTINIT,
    //HMSCOPEPRM_CODE_RECRUN,
    //HMSCOPEPRM_CODE_RECDONE,
    //HMSCOPEPRM_CODE_SERVBUSY,
    HMSCOPEPRM_CODE_INVBUFF,
    HMSCOPEPRM_CODE_INVSIZE,
    //HMSCOPEPRM_CODE_FLOATDISABLED,
            
            
}HMSCOPEPRM_eResponseCode_t;



/* Scope Handle */
typedef struct
{
    HMDRV_U16   u16ScopeVarCount;                   /* number of active scope variables */
    HMDRV_U16   u16Reserved;                        /*  */
    HMDRV_ADDR  pScopeVarAddr[HMSCOPEPRM_MAX_VARS]; /* addresses of scope variables */
    HMDRV_uVariableSizeOffset_t nScopeVarSize[HMSCOPEPRM_MAX_VARS]; /* sizes of scope variables */
}HMSCOPEPRM_sHandle_t;

typedef struct
{
    uint16_t                    Dummy;
}HMSCOPEPRM_sStatusModeFullSize_t;

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
HMSCOPEPRM_sStatusModeFullSize_t* HMSCOPEPRM_GetStatusScopeParamAddress(HMDRV_U8 scopeIndex);
HMDRV_ADDR HMSCOPEPRM_GetBufferScopeParamAddress(HMDRV_U8 scopeIndex);

#endif	/* HMSCOPEPRM_T_H */
