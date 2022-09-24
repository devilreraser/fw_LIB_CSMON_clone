/* *****************************************************************************
 * File:   hmscopeparam.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 11 03 12:54
 * 
 * Description: Hex Monitor Scope Parameters
 * 
 **************************************************************************** */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "hmdrv.h"
//#include "hmprotocol.h"
#include "hmscopeparam.h"

#if HMDRV_USE_CSMON
#include "csmon_t.h"
#endif



/* *****************************************************************************
 * Check if Not Used
 **************************************************************************** */
#if (HMDRV_USE_SCOPE_PARAMETERS) && (!(HMDRV_DISABLE))



/* *****************************************************************************
 * Types
 **************************************************************************** */



/* *****************************************************************************
 * Variables
 **************************************************************************** */

#if HMDRV_HMSCOPEPRM_BUFFER_SIZE > 0
#if HMDRV_SCOPE_PARAM_HANDLE_COUNT > 0
HMSCOPEPRM_sHandle_t sScopeHandlePrm[HMDRV_SCOPE_PARAM_HANDLE_COUNT];
HMDRV_U8 HMSCOPEPRM_u8Buffer[HMDRV_SCOPE_PARAM_HANDLE_COUNT][HMDRV_HMSCOPEPRM_BUFFER_SIZE];
HMDRV_BPTR pBufferScope[HMDRV_SCOPE_PARAM_HANDLE_COUNT] = 
{
    NULL, 
    #if HMDRV_SCOPE_PARAM_HANDLE_COUNT >= 2
    NULL,
    #endif
    #if HMDRV_SCOPE_PARAM_HANDLE_COUNT >= 3
    NULL,
    #endif
};
HMDRV_U16 nSizeBytesScopeBuffer[HMDRV_SCOPE_PARAM_HANDLE_COUNT] = 
{
    0,
    #if HMDRV_SCOPE_PARAM_HANDLE_COUNT >= 2
    0,
    #endif
    #if HMDRV_SCOPE_PARAM_HANDLE_COUNT >= 3
    0,
    #endif
};
#if HMDRV_VALID_SCOPE_DATA_ONLY
HMDRV_BOOL DataChangedParamFlag; 
#endif
#endif
#endif

HMSCOPEPRM_sStatusModeFullSize_t  sScopeStatusParam[HMDRV_SCOPE_PARAM_HANDLE_COUNT];

HMDRV_U8 nScopeIndexFreemaster = HMDRV_FREEMASTER_SCOPE_PARAM_INDEX;    

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
HMSCOPEPRM_eResponseCode_t HMSCOPE_ReadScopeParam(HMDRV_U8 scopeIndex);
void HMSCOPEPRM_vInitBufferParam(HMDRV_U8 nScopeIndex, HMDRV_BPTR pBuffer, HMDRV_U8 nSizeBytes);

/* *****************************************************************************
 * HMSCOPEPRM_vInit
 * 
 * Description: Module Initialization
 **************************************************************************** */
void HMSCOPEPRM_vInit(void)
{
    HMDRV_U8 u8Index;
    #if HMDRV_HMSCOPEPRM_BUFFER_SIZE > 0
    #if HMDRV_SCOPE_PARAM_HANDLE_COUNT > 0
    for (u8Index = 0; u8Index < HMDRV_SCOPE_PARAM_HANDLE_COUNT; u8Index++)
    {
        pBufferScope[u8Index] = HMSCOPEPRM_u8Buffer[u8Index];
        nSizeBytesScopeBuffer[u8Index] = HMDRV_HMSCOPEPRM_BUFFER_SIZE;
        sScopeHandlePrm[u8Index].u16ScopeVarCount = 0;
    }
    #endif
    #endif
}

/* *****************************************************************************
 * HMSCOPEPRM_vInitBufferParam
 * 
 * Description: Buffer pointer Initialization
 **************************************************************************** */
void HMSCOPEPRM_vInitBufferParam(HMDRV_U8 nScopeIndex, HMDRV_BPTR pBuffer, HMDRV_U8 nSizeBytes)
{
    pBufferScope[nScopeIndex] = pBuffer;
    nSizeBytesScopeBuffer[nScopeIndex] = nSizeBytes;
}


/* *****************************************************************************
 * HMSCOPEPRM_vSetDataChangeFlag
 * 
 * Description: Data Changed Possible
 **************************************************************************** */
void HMSCOPEPRM_vSetDataChangeFlag(void)
{
    DataChangedParamFlag = true;
}


/* *****************************************************************************
 * HMSCOPEPRM_ReadScope
 * 
 * Description: Process Read Scope Command
 **************************************************************************** */
HMDRV_BPTR HMSCOPEPRM_ReadScope(HMDRV_U8 nScopeIndex, HMDRV_BPTR pMessageIO)
{
    HMDRV_U8 i;
    
    if(!sScopeHandlePrm[nScopeIndex].u16ScopeVarCount)
    {
        pMessageIO = HMDRV_ConstToBuffer8(pMessageIO, HMPROTOCOL_STC_NOTINIT);
    }
    else
    {
        /* success */
        pMessageIO = HMDRV_ConstToBuffer8(pMessageIO, HMPROTOCOL_STS_OK);

        for (i=0U; i<sScopeHandlePrm[nScopeIndex].u16ScopeVarCount; i++)
        {
            #if HMDRV_VALID_SCOPE_DATA_ONLY
            do
            {
                DataChangedParamFlag = false;
                HMDRV_CopyToBuffer(pMessageIO, sScopeHandlePrm[nScopeIndex].pScopeVarAddr[i], sScopeHandlePrm[nScopeIndex].nScopeVarSize[i].u8Register);
            }while(DataChangedParamFlag);
            pMessageIO += sScopeHandlePrm[nScopeIndex].nScopeVarSize[i].u8Register;
            #else
            pMessageIO = HMDRV_CopyToBuffer(pMessageIO, sScopeHandlePrm[nScopeIndex].pScopeVarAddr[i], sScopeHandlePrm[nScopeIndex].nScopeVarSize[i]);
            #endif
        } 
    }
        
    /* return end position */
    return pMessageIO;  
}

/* *****************************************************************************
 * HMSCOPE_ReadScope
 * 
 * Description: Process Read Scope Command
 **************************************************************************** */
HMSCOPEPRM_eResponseCode_t HMSCOPE_ReadScopeParam(HMDRV_U8 scopeIndex)
{
    volatile HMDRV_BPTR pBuffer = pBufferScope[scopeIndex];
    HMSCOPEPRM_eResponseCode_t eResponseCode;
    HMDRV_U8 i, sz;
    HMDRV_U16 sizeTotal = 0;
    
    if(!sScopeHandlePrm[scopeIndex].u16ScopeVarCount)
    {
        eResponseCode = HMSCOPEPRM_CODE_NOTINIT;
    }
    else
    if(pBuffer == NULL)
    {
        eResponseCode = HMSCOPEPRM_CODE_INVBUFF;
    }
    else
    {
        /* success */
        eResponseCode = HMSCOPEPRM_CODE_OK;

        for (i=0U; i<sScopeHandlePrm[scopeIndex].u16ScopeVarCount; i++)
        {
            sz = sScopeHandlePrm[scopeIndex].nScopeVarSize[i].u8Register;
            if (((HMDRV_uVariableSizeOffset_t)sz).sOffset.bBitField)
            {
                sz = 1;
            }
            
            sizeTotal += sz;
            
            if (sizeTotal <= nSizeBytesScopeBuffer[scopeIndex])
            {
                #if HMDRV_VALID_SCOPE_DATA_ONLY
                do
                {
                    DataChangedParamFlag = false;
                    HMDRV_CopyToBuffer(pBuffer, sScopeHandlePrm[scopeIndex].pScopeVarAddr[i], sz);
                } while(DataChangedParamFlag == true);
                pBuffer += sz;
                #else
                pBuffer = HMDRV_CopyToBuffer(pBuffer, sScopeHandlePrm[scopeIndex].pScopeVarAddr[i], sz);
                #endif
            }
        } 
    }
        
    /* return error code */
    return eResponseCode;  
}



HMSCOPEPRM_sStatusModeFullSize_t* HMSCOPEPRM_GetStatusScopeParamAddress(HMDRV_U8 scopeIndex)
{
    return &sScopeStatusParam[scopeIndex];
}



/* *****************************************************************************
 * HMSCOPEPRM_SetUpScope
 * 
 * Description: Handling SETUPSCOPE and SETUPSCOPE_EX command
 *          As all command handlers, the return value should be the buffer 
 *           pointer where the response output finished (except checksum)
 **************************************************************************** */
HMPROTOCOL_eResponseCode_t HMSCOPEPRM_SetUpScope(HMDRV_U8 scopeIndex, HMDRV_BPTR pMessageIO)
{

    HMDRV_U8 i, sz, nVarCnt;
    HMPROTOCOL_eResponseCode_t eResponseCode = HMPROTOCOL_STS_OK;



    /* uninitialize scope */
    sScopeHandlePrm[scopeIndex].u16ScopeVarCount = 0U;

    /* seek the setup data */
    pMessageIO = HMDRV_SkipInBuffer(pMessageIO, 2U);
    
    /* scope variable count  */
    pMessageIO = HMDRV_ValueFromBuffer8(&nVarCnt, pMessageIO);

    /* scope variable information must fit into our buffers */
    if(!nVarCnt || nVarCnt > (HMDRV_U8)HMSCOPEPRM_MAX_VARS)
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
            sScopeHandlePrm[scopeIndex].nScopeVarSize[i].u8Register = sz;

            /* variable address */
            pMessageIO = HMDRV_AddressFromBuffer(&sScopeHandlePrm[scopeIndex].pScopeVarAddr[i], pMessageIO);

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
        sScopeHandlePrm[scopeIndex].u16ScopeVarCount = nVarCnt;
    }
    /* return just a status */
    return eResponseCode;
 }




HMDRV_ADDR HMSCOPEPRM_GetBufferScopeParamAddress(HMDRV_U8 scopeIndex)
{
    return (HMDRV_ADDR)(HMDRV_ADDR_SIZE)pBufferScope[scopeIndex];
}





#endif
