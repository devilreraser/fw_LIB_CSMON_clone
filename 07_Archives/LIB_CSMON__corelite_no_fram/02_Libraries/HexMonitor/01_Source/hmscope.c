/* *****************************************************************************
 * File:   hmscope.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 01 12 10:56
 * 
 * Description: Hex Monitor Scope
 * 
 **************************************************************************** */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "hmdrv.h"
//#include "hmprotocol.h"
#include "hmscope.h"




/* *****************************************************************************
 * Check if Not Used
 **************************************************************************** */
#if (HMDRV_USE_SCOPE) && (!(HMDRV_DISABLE))

/* *****************************************************************************
 * Variables
 **************************************************************************** */
static HMSCOPE_sHandle_t sScopeHandle;

#if HMDRV_VALID_SCOPE_DATA_ONLY
HMDRV_BOOL DataChangedFlag; 
#endif

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */


/* *****************************************************************************
 * HMSCOPE_vInit
 * 
 * Description: Module Initialization
 **************************************************************************** */
void HMSCOPE_vInit(void)
{
}


/* *****************************************************************************
 * HMSCOPE_vSetDataChangeFlag
 * 
 * Description: Data Changed Possible
 **************************************************************************** */
void HMSCOPE_vSetDataChangeFlag(void)
{
    DataChangedFlag = true;
}


/* *****************************************************************************
 * HMSCOPE_ReadScope
 * 
 * Description: Process Read Scope Command
 **************************************************************************** */
HMDRV_BPTR HMSCOPE_ReadScope(HMDRV_BPTR pMessageIO)
{
    HMDRV_U8 i;
    
    if(!sScopeHandle.u8ScopeVarCount)
    {
        pMessageIO = HMDRV_ConstToBuffer8(pMessageIO, HMPROTOCOL_STC_NOTINIT);
    }
    else
    {
        /* success */
        pMessageIO = HMDRV_ConstToBuffer8(pMessageIO, HMPROTOCOL_STS_OK);

        for (i=0U; i<sScopeHandle.u8ScopeVarCount; i++)
        {
            #if HMDRV_VALID_SCOPE_DATA_ONLY
            do
            {
                DataChangedFlag = false;
                HMDRV_CopyToBuffer(pMessageIO, sScopeHandle.pScopeVarAddr[i], sScopeHandle.nScopeVarSize[i]);
            }while(DataChangedFlag);
            pMessageIO += sScopeHandle.nScopeVarSize[i];
            #else
            pMessageIO = HMDRV_CopyToBuffer(pMessageIO, sScopeHandle.pScopeVarAddr[i], sScopeHandle.nScopeVarSize[i]);
            #endif
        } 
    }
        
    /* return end position */
    return pMessageIO;  
}

/* *****************************************************************************
 * HMSCOPE_SetUpScope
 * 
 * Description: Handling SETUPSCOPE and SETUPSCOPE_EX command
 *          As all command handlers, the return value should be the buffer 
 *           pointer where the response output finished (except checksum)
 **************************************************************************** */
HMPROTOCOL_eResponseCode_t HMSCOPE_SetUpScope(HMDRV_BPTR pMessageIO)
{

    HMDRV_U8 i, sz, nVarCnt;
    HMPROTOCOL_eResponseCode_t eResponseCode = HMPROTOCOL_STS_OK;



    /* uninitialize scope */
    sScopeHandle.u8ScopeVarCount = 0U;

    /* seek the setup data */
    pMessageIO = HMDRV_SkipInBuffer(pMessageIO, 2U);
    
    /* scope variable count  */
    pMessageIO = HMDRV_ValueFromBuffer8(&nVarCnt, pMessageIO);

    /* scope variable information must fit into our buffers */
    if(!nVarCnt || nVarCnt > (HMDRV_U8)HMSCOPE_MAX_VARS)
    {
        eResponseCode = HMPROTOCOL_STE_INVBUFF;
    }
    else
    {
        /* get all addresses and sizes */
        for(i=0U; i<nVarCnt; i++)
        {
            /* variable size */
            pMessageIO = HMDRV_ValueFromBuffer8(&sz, pMessageIO);
            sScopeHandle.nScopeVarSize[i] = sz;

            /* variable address */
            pMessageIO = HMDRV_AddressFromBuffer(&sScopeHandle.pScopeVarAddr[i], pMessageIO);

            /* valid numeric variable sizes only */
            if(sz == 0U || sz > 8U)
            {
                eResponseCode = HMPROTOCOL_STE_INVSIZE;
            }

            #if HMDRV_CFG_BUS_WIDTH > 1U
            /* even sizes only */
            if(sz & 0x1)
            {
                eResponseCode = HMPROTOCOL_STE_INVSIZE;
            }
            #endif
        }

        /* activate scope */
        sScopeHandle.u8ScopeVarCount = nVarCnt;
    }
    /* return just a status */
    return eResponseCode;
 }
#endif
