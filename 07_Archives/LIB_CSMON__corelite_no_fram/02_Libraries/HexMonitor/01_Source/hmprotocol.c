/* *****************************************************************************
 * File:   hmprotocol.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 01 03 11:35
 * 
 * Description: Hex Monitor Library Protocol
 * 
 **************************************************************************** */





/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "hmdrv.h"


#if HMDRV_USE_FREEMASTER



#include "hmprotocol.h"
#include "hmrec.h"
#include "hmrecparam.h"
#include "hmscope.h"
#include "hmscopeparam.h"
#include "hmserial.h"



#if HMDRV_USE_WITH_FREEMASTER_DECODER

#include "freemaster.h"
#include "freemaster_private.h"

#endif

HMDRV_U8 nRecorderFreemasterIndex = HMDRV_RECORDER_USED_FREEMASTER;
HMDRV_U8 nScopePrmFreemasterIndex = HMDRV_FREEMASTER_SCOPE_PARAM_INDEX;

HMDRV_U8 bRecorderParameterUsage = 0;
HMDRV_U8 bScopeParameterUsage = 0;

HMDRV_U16 u16TimeBase = HMREC_TIMEBASE;

void HMPROTOCOL_vRecorderSetTimeBase(HMDRV_U16 u16Inputusec)
{
    u16TimeBase = HMREC_BASE_MICROSEC((unsigned short)(u16Inputusec));
}


/* *****************************************************************************
 * HMPROTOCOL_vInit
 * 
 * Description: Module Initialization
 **************************************************************************** */
void HMPROTOCOL_vInit(void)
{
    //nRecorderFreemasterIndex = HMDRV_RECORDER_USED_FREEMASTER;
    //nScopePrmFreemasterIndex = HMDRV_FREEMASTER_SCOPE_PARAM_INDEX;
}


/**************************************************************************//*!
*
* @brief    Handling READMEM and READMEM_EX commands
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
******************************************************************************/

HMDRV_BPTR HMPROTOCOL_ReadMem(HMDRV_BPTR pMessageIO)
{
    //HMDRV_BPTR pResponseBackup = pMessageIO;
    volatile HMDRV_BPTR pResponse = pMessageIO;
    HMDRV_ADDR nAddr;
    HMDRV_U8 nSize;

    pMessageIO = HMDRV_SkipInBuffer(pMessageIO, 2U);
    pMessageIO = HMDRV_ValueFromBuffer8(&nSize, pMessageIO);
    pMessageIO = HMDRV_AddressFromBuffer(&nAddr, pMessageIO);


    /* check the response will safely fit into comm buffer */
    if(nSize > (HMDRV_U8)HMDRV_COMM_BUFFER_SIZE)
    {
        return HMDRV_ConstToBuffer8(pResponse, HMPROTOCOL_STC_RSPBUFFOVF);
    }
    
    
    
    /* success  */
    pResponse = HMDRV_ConstToBuffer8(pResponse, HMPROTOCOL_STS_OK);
    
    
    pResponse = HMDRV_CopyToBuffer(pResponse, nAddr, (HMDRV_SIZE8) nSize);
    
    /* Data Access Error - not used - returns zero data
    if(pResponse == NULL)
    {
        return HMDRV_ConstToBuffer8(pResponseBackup, HMPROTOCOL_STC_EACCESS);
    }
     */
    
    
    return pResponse;
}

/**************************************************************************//*!
*
* @brief    Handling WRITEMEM and WRITEMEM_EX commands
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
******************************************************************************/

HMDRV_BPTR HMPROTOCOL_WriteMem(HMDRV_BPTR pMessageIO)
{
    HMDRV_BPTR pResponse = pMessageIO;
    HMDRV_ADDR nAddr;
    HMDRV_U8 nSize,nResponseCode;

    pMessageIO = HMDRV_SkipInBuffer(pMessageIO, 2U);
    pMessageIO = HMDRV_ValueFromBuffer8(&nSize, pMessageIO);
    pMessageIO = HMDRV_AddressFromBuffer(&nAddr, pMessageIO);

    /* check here if need to protect some memory area */

    /*lint -e{534} ignoring function return value */
    (void)HMDRV_CopyFromBuffer(nAddr, pMessageIO, (HMDRV_SIZE8) nSize);
    nResponseCode = HMPROTOCOL_STS_OK;

    return HMDRV_ConstToBuffer8(pResponse, nResponseCode);
}

/**************************************************************************//*!
*
* @brief    Handling WRITEMEMMASK and WRITEMEMMASK_EX commands
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
******************************************************************************/

HMDRV_BPTR HMPROTOCOL_WriteMemMask(HMDRV_BPTR pMessageIO)
{
    HMDRV_BPTR pResponse = pMessageIO;
    HMDRV_ADDR nAddr;
    HMDRV_U8 nSize,nResponseCode;

    pMessageIO = HMDRV_SkipInBuffer(pMessageIO, 2U);
    pMessageIO = HMDRV_ValueFromBuffer8(&nSize, pMessageIO);
    pMessageIO = HMDRV_AddressFromBuffer(&nAddr, pMessageIO);

    /* check here if need to protect some memory area */

    #if HMDRV_CFG_BUS_WIDTH > 1U
    /* size must be divisible by bus width (mask must not begin in half of memory word) */
    if(nSize % HMDRV_CFG_BUS_WIDTH)
    {
        nResponseCode = HMPROTOCOL_STC_INVSIZE;

    }
    else
    #endif
    {
    /* put the data */
    HMDRV_CopyFromBufferWithMask(nAddr, pMessageIO, (HMDRV_SIZE8)nSize);
    nResponseCode = HMPROTOCOL_STS_OK;
    }
      
    return HMDRV_ConstToBuffer8(pResponse, nResponseCode);
}

/**************************************************************************//*!
*
* @brief    Handling GETINFO or GETINFO_BRIEF
*
* @param    pMessageIO - original command (in) and response buffer (out) 
*
* @return   As all command handlers, the return value should be the buffer 
*           pointer where the response output finished (except checksum)
*
******************************************************************************/

HMDRV_BPTR HMPROTOCOL_GetBoardInfo(HMDRV_BPTR pMessageIO)
{
    HMDRV_BPTR pResponse = pMessageIO;
    HMDRV_U16 wTmp;
    HMDRV_U8 *pStr;

    pResponse = HMDRV_ConstToBuffer8(pResponse, HMPROTOCOL_STS_OK);              
    pResponse = HMDRV_ConstToBuffer8(pResponse, (HMDRV_U8)(HMDRV_PROT_VER));            /* protVer */
    pResponse = HMDRV_ConstToBuffer8(pResponse, (HMDRV_U8)(HMDRV_CFGFLAG));             /* cfgFlags */
    pResponse = HMDRV_ConstToBuffer8(pResponse, (HMDRV_U8)(HMDRV_CFG_BUS_WIDTH));       /* dataBusWdt */
    pResponse = HMDRV_ConstToBuffer8(pResponse, (HMDRV_U8)(HMDRV_GLOB_VERSION_MAJOR));  /* globVerMajor */
    pResponse = HMDRV_ConstToBuffer8(pResponse, (HMDRV_U8)(HMDRV_GLOB_VERSION_MINOR));  /* globVerMinor */
    pResponse = HMDRV_ConstToBuffer8(pResponse, (HMDRV_U8)(HMDRV_COMM_BUFFER_SIZE));    /* cmdBuffSize  */
    /* that is all for brief info */
    
    #if HMDRV_USE_RECORDER || HMDRV_USE_RECORDER_PARAMETERS
    if (bRecorderParameterUsage == 0)
    {
        if (nRecorderFreemasterIndex >= HMDRV_RECORDER_HANDLE_COUNT)
        {
            nRecorderFreemasterIndex = HMDRV_RECORDER_HANDLE_COUNT - 1;
        }
        /* recorder buffer size is always measured in bytes */
        wTmp = HMREC_GetRecBuffSize(&sRecHandle[nRecorderFreemasterIndex]);
        wTmp *= HMDRV_CFG_BUS_WIDTH;
        /* send size and timebase    */
        pResponse = HMDRV_ValueToBuffer16(pResponse, wTmp);
        pResponse = HMDRV_ValueToBuffer16(pResponse, u16TimeBase);
    }
    else
    {
        if (nRecorderFreemasterIndex >= HMDRV_RECORDER_PARAM_HANDLE_COUNT)
        {
            nRecorderFreemasterIndex = HMDRV_RECORDER_PARAM_HANDLE_COUNT - 1;
        }
        /* recorder buffer size is always measured in bytes */
        wTmp = HMRECPRM_GetRecBuffSize(&sRecHandleParam[nRecorderFreemasterIndex]);
        wTmp *= HMDRV_CFG_BUS_WIDTH;
        /* send size and timebase    */
        pResponse = HMDRV_ValueToBuffer16(pResponse, wTmp);
        pResponse = HMDRV_ValueToBuffer16(pResponse, (HMDRV_U16) HMRECPRM_GetTimeBase(&sRecHandleParam[nRecorderFreemasterIndex]));
    }

    #else /* HMDRV_USE_RECORDER */
    HMDRV_UNUSED(wTmp);
    /* recorder info zeroed */
    pResponse = HMDRV_ConstToBuffer16(pResponse, 0);
    pResponse = HMDRV_ConstToBuffer16(pResponse, 0);
    #endif /* HMDRV_USE_RECORDER */

    /* description string */
    pStr = (HMDRV_U8*)  HMDRV_IDT_STRING;
    for(wTmp = 0U; wTmp < (HMDRV_U8)(HMPROTOCOL_DESCR_SIZE); wTmp++)
    {
        pResponse = HMDRV_ValueToBuffer8(pResponse, *pStr);

        /* terminating zero used to clear the remainder of the buffer */        
        if(*pStr)
        {
            pStr ++;
        }
    }


    return pResponse;   
}




volatile HMDRV_U8 breakData;

/* *****************************************************************************
 * HMPROTOCOL_bDecoder
 * 
 * Description: Protocol Decoder
 **************************************************************************** */
HMDRV_BOOL HMPROTOCOL_bDecoder(HMDRV_BPTR pMessageIO)
{
    #define HMPROTOCOL_USE_CHECK_CODE_RESPONSE  0   /* for Recorder Get Buffer */
    
	HMDRV_BOOL bResult = HMDRV_FALSE;
    HMDRV_BPTR pResponseEnd;
    HMDRV_U8 nCmd;
    #if (HMDRV_USE_RECORDER && (HMDRV_RECORDER_HANDLE_COUNT > 1) ) || (HMDRV_USE_RECORDER_PARAMETERS && (HMDRV_RECORDER_PARAM_HANDLE_COUNT > 1))
    HMDRV_U8 nRecorderIndex = nRecorderFreemasterIndex;
    #endif
    #if HMDRV_USE_RECORDER || HMDRV_USE_RECORDER_PARAMETERS
    HMREC_sHandle_t* psHandle = NULL; 
    HMRECPRM_sHandle_t* psHandlePrm = NULL; 
    #endif
    
    #if HMPROTOCOL_USE_CHECK_CODE_RESPONSE
    HMPROTOCOL_eResponseCode_t eResponseCode;
    #endif
    
    /* no EX access by default */
    HMDRV_SetExAddr(HMDRV_FALSE);
    
    /* command code comes first in the message */
    /*lint -e{534} return value is not used */
    (void)HMDRV_ValueFromBuffer8(&nCmd, pMessageIO);

    //if recorder command needs recorder index handling (nRecorderIndex)
    pResponseEnd = pMessageIO;
    #if HMDRV_USE_RECORDER || HMDRV_USE_RECORDER_PARAMETERS
    if (bRecorderParameterUsage == 0)
    {
        #if HMDRV_USE_RECORDER
        #if HMDRV_RECORDER_HANDLE_COUNT == 1
        psHandle = HMDRV_RECINDEX2PTR(0);
        #else
        psHandle = HMDRV_RECINDEX2PTR(nRecorderIndex);
        #endif
        #endif
    }
    else
    {
        #if HMDRV_USE_RECORDER_PARAMETERS
        #if HMDRV_RECORDER_PARAM_HANDLE_COUNT == 1
        psHandlePrm = HMDRV_RECPRMINDEX2PTR(0);
        #else
        psHandlePrm = HMDRV_RECPRMINDEX2PTR(nRecorderIndex);
        #endif
        #endif
    }

    
    #endif

    if (nCmd == HMPROTOCOL_CMD_WRITEMEMMASK_EX)
    {
        breakData = HMPROTOCOL_CMD_WRITEMEMMASK_EX;
    }
    
    if (nCmd == HMPROTOCOL_CMD_WRITEMEMMASK)
    {
        breakData = HMPROTOCOL_CMD_WRITEMEMMASK;
    }
    
    /* process command   */
    switch (nCmd)
    {
        #if HMDRV_USE_RECORDER || HMDRV_USE_RECORDER_PARAMETERS
        /* get recorder status */
        case HMPROTOCOL_CMD_GETRECSTS:
            if (bRecorderParameterUsage == 0)
            {
                pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd,HMREC_GetRecStatus(psHandle));
            }
            else
            {
                pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd,HMRECPRM_GetRecStatus(psHandlePrm));
            }
            break;

        /* start recorder */
        case HMPROTOCOL_CMD_STARTREC:
            if (bRecorderParameterUsage == 0)
            {
                pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd,HMREC_StartRec(psHandle));
            }
            else
            {
                pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd,HMRECPRM_StartRec(psHandlePrm));
            }
            break;

        /* stop recorder */
        case HMPROTOCOL_CMD_STOPREC:
            if (bRecorderParameterUsage == 0)
            {
                pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd,HMREC_StopRec(psHandle));
            }
            else
            {
                pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd,HMRECPRM_StopRec(psHandlePrm));
            }
            break;

        /* setup recorder */    
        #if HMDRV_USE_EX_CMDS
        case HMPROTOCOL_CMD_SETUPREC_EX:                 
           HMDRV_SetExAddr(HMDRV_TRUE);
        #endif
        /*lint -fall through */
        case HMPROTOCOL_CMD_SETUPREC:   
            if (bRecorderParameterUsage == 0)
            {
                pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd,HMREC_SetUpRec(psHandle, pMessageIO));
            }
            else
            {
                HMLIB_vInitRecParamBuff(true);
                pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd,HMRECPRM_SetUpRec(psHandlePrm, pMessageIO));
            }
            break;
            
        /* get recorder buffer information (force EX instead of non-EX) */  
        #if HMDRV_USE_EX_CMDS
        case HMPROTOCOL_CMD_GETRECBUFF_EX:
            HMDRV_SetExAddr(HMDRV_TRUE);
            if (bRecorderParameterUsage == 0)
            {
                pResponseEnd = HMREC_GetRecBuffFreemaster(psHandle, pMessageIO);
            }
            else
            {
                pResponseEnd = HMRECPRM_GetRecBuffFreemaster(psHandlePrm, pMessageIO);
            }
            break;
        #endif
        /*lint -fall through */
        case HMPROTOCOL_CMD_GETRECBUFF:  
            if (bRecorderParameterUsage == 0)
            {
                if ((HMDRV_U32)psHandle->pEndBuffPtr > 0xFFFF) 
                {
                    /* address bigger than 16 bits - send invalid command to freemaster to force use extended address */
                    pResponseEnd = HMDRV_ConstToBuffer8(pMessageIO, HMPROTOCOL_STC_INVCMD);
                }
                else
                {
                #if (!HMPROTOCOL_USE_CHECK_CODE_RESPONSE)
                pResponseEnd = HMREC_GetRecBuffFreemaster(psHandle, pMessageIO);
                #else
                eResponseCode = HMREC_GetRecBuffCheckCode(psHandle);
                pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd,eResponseCode);
                if (eResponseCode == HMPROTOCOL_STS_OK)
                {
                    pResponseEnd = HMDRV_AddressToBuffer(pResponseEnd, psHandle->pBufferAddr);
                    pResponseEnd = HMDRV_ValueToBuffer16(pResponseEnd, psHandle->u16RecBuffStartIx);               
                }
                #endif
                }
            }
            else
            {
                if ((HMDRV_U32)psHandlePrm->pEndBuffPtr > 0xFFFF) 
                {
                    /* address bigger than 16 bits - send invalid command to freemaster to force use extended address */
                    pResponseEnd = HMDRV_ConstToBuffer8(pMessageIO, HMPROTOCOL_STC_INVCMD);
                }
                else
                {
                #if (!HMPROTOCOL_USE_CHECK_CODE_RESPONSE)
                pResponseEnd = HMRECPRM_GetRecBuffFreemaster(psHandlePrm, pMessageIO);
                #else
                eResponseCode = HMRECPRM_GetRecBuffCheckCode(psHandlePrm);
                pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd,eResponseCode);
                if (eResponseCode == HMPROTOCOL_STS_OK)
                {
                    pResponseEnd = HMDRV_AddressToBuffer(pResponseEnd, psHandlePrm->pBufferAddr);
                    pResponseEnd = HMDRV_ValueToBuffer16(pResponseEnd, psHandlePrm->u16RecBuffStartIx);               
                }
                #endif
                }
                
            }
            break; 
        #endif /* HMDRV_USE_RECORDER */            
            
        #if HMDRV_USE_SCOPE || HMDRV_USE_SCOPE_PARAMETERS 
        /* prepare scope variables */   
        #if HMDRV_USE_EX_CMDS
        case HMPROTOCOL_CMD_SETUPSCOPE_EX:
            HMDRV_SetExAddr(HMDRV_TRUE);
        #endif
        /*lint -fallthrough */
        case HMPROTOCOL_CMD_SETUPSCOPE:
            if (bScopeParameterUsage == 0)
            {
                pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd,HMSCOPE_SetUpScope(pMessageIO));
            }
            else
            {
                pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd,HMSCOPEPRM_SetUpScope(nScopePrmFreemasterIndex, pMessageIO));
            }
            break;  
            
        case HMPROTOCOL_CMD_READSCOPE:
            if (bScopeParameterUsage == 0)
            {
                pResponseEnd = HMSCOPE_ReadScope(pMessageIO);
            }
            else
            {
                pResponseEnd = HMSCOPEPRM_ReadScope(nScopePrmFreemasterIndex, pMessageIO);
            }
            
            break;
        #endif /* HMDRV_USE_SCOPE */
            
            
        #if HMDRV_USE_READMEM
        /* read a block of memory */
        #if HMDRV_USE_EX_CMDS
        case HMPROTOCOL_CMD_READMEM_EX:
            HMDRV_SetExAddr(HMDRV_TRUE);
        #endif
        /*lint -fallthrough */
        case HMPROTOCOL_CMD_READMEM:
            pResponseEnd = HMPROTOCOL_ReadMem(pMessageIO);
            break;            
        #endif /* HMDRV_USE_READMEM */
            
            
        #if HMDRV_USE_WRITEMEM
        /* write a block of memory */
        #if HMDRV_USE_EX_CMDS
        case HMPROTOCOL_CMD_WRITEMEM_EX:
            HMDRV_SetExAddr(HMDRV_TRUE);
        #endif
        /*lint -fallthrough */
        case HMPROTOCOL_CMD_WRITEMEM:
            pResponseEnd = HMPROTOCOL_WriteMem(pMessageIO);
            break;
        #endif /* HMDRV_USE_WRITEMEM */

            
        #if HMDRV_USE_WRITEMEMMASK
        /* write block of memory with a bit mask */
        #if HMDRV_USE_EX_CMDS
        case HMPROTOCOL_CMD_WRITEMEMMASK_EX:
            HMDRV_SetExAddr(HMDRV_TRUE);
        #endif
        /*lint -fallthrough */
        case HMPROTOCOL_CMD_WRITEMEMMASK:
            pResponseEnd = HMPROTOCOL_WriteMemMask(pMessageIO);
            break;
        #endif /* HMDRV_USE_WRITEMEMMASK */
            
            
            
            
        #if HMDRV_USE_BRIEFINFO
        /* retrieve a subset of board information structure */
        case HMPROTOCOL_CMD_GETINFOBRIEF:
        #endif
        /* retrieve board information structure */
        case HMPROTOCOL_CMD_GETINFO: 
            pResponseEnd = HMPROTOCOL_GetBoardInfo(pMessageIO);
            break;

            
        default:
            pResponseEnd = HMDRV_ConstToBuffer8(pMessageIO, HMPROTOCOL_STC_INVCMD);
            break;    
    }    
    
    /* anything to send back? */
    if(pResponseEnd != pMessageIO)
    {
        /*lint -e{946,960} subtracting pointers is appropriate here */
        HMDRV_SIZE8 nSize = (HMDRV_SIZE8)(pResponseEnd - pMessageIO);
        
        HMSERIAL_vSendResponse(pMessageIO, nSize);
        bResult = HMDRV_TRUE;
    }
    else
    {
        /* nothing sent out */
        //bResult = HMDRV_FALSE;
    }
    
    return bResult;
}

#endif /* HMDRV_USE_FREEMASTER */

