/* *****************************************************************************
 * File:   hmrec.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 01 02 19:56
 * 
 * Description: Hex Monitor Recorder
 * 
 **************************************************************************** */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "hmdrv.h"
//#include "hmprotocol.h"
#include "hmrec.h"

/* *****************************************************************************
 * Check if Not Used
 **************************************************************************** */
#if (HMDRV_USE_RECORDER) && (!(HMDRV_DISABLE))

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Variables
 **************************************************************************** */
#if HMDRV_RECORDER_HANDLE_COUNT > 0
HMREC_sHandle_t sRecHandle[HMDRV_RECORDER_HANDLE_COUNT];
#endif

#if HMDRV_RECORDER_BUFINT_COUNT > 0
HMDRV_U8 au8RecBuffer0[HMDRV_RECORDER_BUFFER0_SIZE / HMDRV_RECBUFF_BYTE_SIZE];
#endif


#if HMDRV_RECORDER_BUFINT_COUNT > 1
HMDRV_U8 au8RecBuffer1[HMDRV_RECORDER_BUFFER1_SIZE / HMDRV_RECBUFF_BYTE_SIZE];
#endif

HMDRV_U16 HMREC_u16FailSnapshotWrite;

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
static HMDRV_BOOL HMREC_Compare8S(HMREC_sHandle_t* sHandle);
static HMDRV_BOOL HMREC_Compare8U(HMREC_sHandle_t* sHandle);
static HMDRV_BOOL HMREC_Compare16S(HMREC_sHandle_t* sHandle);
static HMDRV_BOOL HMREC_Compare16U(HMREC_sHandle_t* sHandle);
static HMDRV_BOOL HMREC_Compare32S(HMREC_sHandle_t* sHandle);
static HMDRV_BOOL HMREC_Compare32U(HMREC_sHandle_t* sHandle);






    



/* *****************************************************************************
 * HMREC_vInit
 * 
 * Description: Module Initialization
 **************************************************************************** */
void HMREC_vInit(HMREC_sHandle_t* sHandle)
{
    /* initialize Recorder flags*/
    sHandle->uFlags.nAll = 0U;
    
    /* reset buffer address and size until initialized */
    sHandle->pBufferAddr = NULL;
    sHandle->nBufferSize = 0U;
    HMREC_u16FailSnapshotWrite = 0;
}


/* *****************************************************************************
 * HMREC_vProcess
 * 
 * Description: Process Recorder
 **************************************************************************** */
void HMREC_vProcess(HMREC_sHandle_t* sHandle) 
{
    HMDRV_U8 sz;
    #if HMDRV_USE_EXT_MEM
    HMDRV_U8 size;
    #endif
    HMDRV_BOOL cmp;
    HMDRV_U8 i;
    //HMDRV_U16 memoryQueueID;    //??? to to if needed

	/* running ? */
    if(!sHandle->uFlags.sFlg.bIsRunning)
    {
		/* skip if not running */
    }
    else
    {
        /* skip this call ? */
        if(sHandle->u16RecTimeDivCtr)
        {
            /* maybe next time... */
            sHandle->u16RecTimeDivCtr--;
        }
        else
        {

            /* re-initialize divider */
            sHandle->u16RecTimeDivCtr = sHandle->u16RecTimeDiv;

            /* take snapshot of variable values */
            #if HMDRV_USE_EXT_MEM
            if (HMDRV_bIsExternalAddress(sHandle->pWritePtr))
            {
                HMDRV_ADDR pSnapshotPtr;	
                HMDRV_U16 u16GlobalQueueID;
               
                pSnapshotPtr = HMDRV_pSnapshotAddressGet(sHandle->pWritePtr, sHandle->u8RecSampleSize, &u16GlobalQueueID);
                //pSnapshotPtr = HMDRV_pGetInternalMemoryAddressWrite(sHandle->pWritePtr, sHandle->u8RecSampleSize, &memoryQueueID);
                
                if (pSnapshotPtr)
                {
                    size = 0;
                    for (i=0U; i<sHandle->u8RecVarCount; i++)
                    {
                        sz = sHandle->u8RecVarSize[i];

                        HMDRV_CopyMemory(pSnapshotPtr, sHandle->pu8RecVarAddr[i], sz);

                        #if HMDRV_CFG_BUS_WIDTH > 1
                        sz /= HMDRV_CFG_BUS_WIDTH;
                        #endif
                        size += sz;
                        sHandle->pWritePtr += sz;
                        pSnapshotPtr += sz;		
                    }	
                    
                    HMDRV_CopySnapshotToMemory(sHandle->pWritePtr, size, &u16GlobalQueueID);
                    //ProcessExternalMemory(pSnapshotPtr, sHandle->pWritePtr, size, &memoryQueueID);
                }
                else
                {
                    //cannot copy snapshot because copy buffer busy
                    HMREC_u16FailSnapshotWrite++;
                }
                
            }
            else
            #endif
            {
                for (i=0U; i<sHandle->u8RecVarCount; i++)
                {
                    sz = sHandle->u8RecVarSize[i];

                    HMDRV_CopyMemory(sHandle->pWritePtr, sHandle->pu8RecVarAddr[i], sz);

                    #if HMDRV_CFG_BUS_WIDTH > 1
                    if(sz < HMDRV_CFG_BUS_WIDTH)
                    {
                        sz = HMDRV_CFG_BUS_WIDTH;
                    }
                    sz /= HMDRV_CFG_BUS_WIDTH;
                    #endif

                    sHandle->pWritePtr += sz;		
                }	
                
            }


            /* another sample taken (startIx "points" after sample just taken) */
            /* i.e. it points to the oldest sample */
            sHandle->u16RecBuffStartIx++;

            /* wrap around (circular buffer) ? */
            if(sHandle->pWritePtr >= sHandle->pEndBuffPtr)
            {   
                sHandle->pWritePtr = sHandle->pBufferAddr;
                sHandle->uFlags.sFlg.bInvirginCycle = 0U;
                sHandle->u16RecBuffStartIx = 0U;
            }

            
            if(sHandle->uFlags.sFlg.bInvirginCycle)
            {   
                /* no trigger testing in virgin cycle */
            }
            else
            {
                /* test trigger condition if still running */
                if(!sHandle->uFlags.sFlg.bIsStopping && sHandle->pfCompareFunc != NULL)
                {
                    /* compare trigger threshold */
                    cmp = ((PCOMPAREFUNC)(sHandle->pfCompareFunc))(sHandle);

                    /* negated logic (falling-edge) ? */
                    if(sHandle->u8RecTriggerMode == 2U)
                    {
                        cmp = (HMDRV_BOOL) !cmp;
                    }

                    /* above threshold ? */
                    if(cmp)
                    {
                        /* were we at least once below threshold ? */
                        if(sHandle->uFlags.sFlg.bTrgCrossActive)
                        {
                            /* EDGE TRIGGER ! */
                            HMREC_TriggerRec(sHandle);
                        }
                    }
                    else
                    {
                        /* we got bellow threshold, now wait for being above threshold */
                        sHandle->uFlags.sFlg.bTrgCrossActive = 1U;
                    }
                }

                /* in stopping mode ? (note that this bit might have been set just above!) */
                if(sHandle->uFlags.sFlg.bIsStopping)
                {
                    /* count down post-trigger samples expired ? */
                    if(!sHandle->u16StoprecCountDown)
                    {
                        /* STOP RECORDER */
                        sHandle->uFlags.sFlg.bIsRunning = 0U;
                    }
                    else
                    {
                        /* perhaps next time */
                        sHandle->u16StoprecCountDown--; 
                    }
                }
            }
        }
	}
}

/* *****************************************************************************
 * HMREC_TriggerRec  - optimize this function call in process for faster execution
 * 
 * Description: Starts the post-trigger stop countdown
 **************************************************************************** */
void HMREC_TriggerRec(HMREC_sHandle_t* sHandle)
{
    if(!sHandle->uFlags.sFlg.bIsStopping)
    {
        sHandle->uFlags.sFlg.bIsStopping = 1U;
        sHandle->u16StoprecCountDown = sHandle->u16RecPostTrigger;
    }
}

/* *****************************************************************************
 * HMREC_StartRec 
 * 
 * Description: This function starts recording (initializes internal recording variables 
 * and flags)
 **************************************************************************** */
HMPROTOCOL_eResponseCode_t HMREC_StartRec(HMREC_sHandle_t* sHandle)
{
    HMPROTOCOL_eResponseCode_t eResponseCode;
     
    /* must be configured */
    if(!sHandle->uFlags.sFlg.bIsConfigured)
    {
        eResponseCode = HMPROTOCOL_STC_NOTINIT;
    }
    else    
    /* already running ? */
    if(sHandle->uFlags.sFlg.bIsRunning)
    {
        eResponseCode = HMPROTOCOL_STC_RECRUN;
    }
    else
    {
        /* initialize write pointer */
        sHandle->pWritePtr = sHandle->pBufferAddr;

        /* current (first) sample index */
        sHandle->u16RecBuffStartIx = 0U;

        /* initialize time divisor */
        sHandle->u16RecTimeDivCtr = 0U;

        /* initiate virgin cycle */
        sHandle->uFlags.sFlg.bIsStopping = 0U;          /* no trigger active */
        sHandle->uFlags.sFlg.bTrgCrossActive = 0U;      /* waiting for threshold crossing */
        sHandle->uFlags.sFlg.bInvirginCycle = 1U;       /* initial cycle */

        /* run now */
        sHandle->uFlags.sFlg.bIsRunning = 1U;           /* is running now! */

        eResponseCode = HMPROTOCOL_STS_OK;
    }
    return eResponseCode;
}



/* *****************************************************************************
 * HMREC_StopRec 
 * 
 * Description: Stop Recording (Manual trigger)
 **************************************************************************** */
HMPROTOCOL_eResponseCode_t HMREC_StopRec(HMREC_sHandle_t* sHandle)
{
    HMPROTOCOL_eResponseCode_t eResponseCode;
    
    if(!sHandle->uFlags.sFlg.bIsConfigured)
    {
        /* must be configured */
        eResponseCode = HMPROTOCOL_STC_NOTINIT;   
    }
    else    
    if(!sHandle->uFlags.sFlg.bIsRunning)
    {
        /* already stopped */
        eResponseCode = HMPROTOCOL_STS_RECDONE;
    }
    else
    {
        /* simulate trigger */
        HMREC_TriggerRec(sHandle);
        eResponseCode = HMPROTOCOL_STS_OK;
    }
    
    return eResponseCode;
}

/**************************************************************************//*!
*
* @brief    Compare macro used in trigger detection
*
* @param    v - original command 
* @param    t - response buffer
*
* @return   zero when value is lower than threshold.
* @return   non-zero when value is greater than or equal as threshold
*
******************************************************************************/
#define CMP(v,t) ((HMDRV_BOOL)(((v) < (t)) ? 0 : 1))

static HMDRV_BOOL HMREC_Compare8S(HMREC_sHandle_t* sHandle)
{
    return CMP(HMDRV_GetS8(sHandle->pu8TrgVarAddr), sHandle->uTrgThreshold.s8);
}

static HMDRV_BOOL HMREC_Compare8U(HMREC_sHandle_t* sHandle)
{
    return CMP(HMDRV_GetU8(sHandle->pu8TrgVarAddr), sHandle->uTrgThreshold.u8);
}

static HMDRV_BOOL HMREC_Compare16S(HMREC_sHandle_t* sHandle)
{
    return CMP(HMDRV_GetS16(sHandle->pu8TrgVarAddr), sHandle->uTrgThreshold.s16);
}

static HMDRV_BOOL HMREC_Compare16U(HMREC_sHandle_t* sHandle)
{
    return CMP(HMDRV_GetU16(sHandle->pu8TrgVarAddr), sHandle->uTrgThreshold.u16);
}

static HMDRV_BOOL HMREC_Compare32S(HMREC_sHandle_t* sHandle)
{
    return CMP(HMDRV_GetS32(sHandle->pu8TrgVarAddr), sHandle->uTrgThreshold.s32);
}

static HMDRV_BOOL HMREC_Compare32U(HMREC_sHandle_t* sHandle)
{
    return CMP(HMDRV_GetU32(sHandle->pu8TrgVarAddr), sHandle->uTrgThreshold.u32);
}
#if HMREC_FLOAT_TRIG
static HMDRV_BOOL HMREC_Comparefloat(HMREC_sHandle_t* sHandle)
{
    return CMP(HMDRV_GetFloat(sHandle->pu8TrgVarAddr), sHandle->uTrgThreshold.fp);
}
#endif



/* *****************************************************************************
 * HMREC_IsInRecBuffer
 * 
 * Description: Check whether given memory region is inside the recorder buffer.
 * This function returns non-zero if user space is in recorder buffer.
 **************************************************************************** */
HMDRV_BOOL HMREC_IsInRecBuffer(HMREC_sHandle_t* sHandle, HMDRV_ADDR pAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_BOOL bRet = 0U;
    
    if(pAddr >= sHandle->pBufferAddr)
    {
        bRet = (HMDRV_BOOL)((pAddr + nSize) <= (sHandle->pBufferAddr + HMREC_GetRecBuffSize(sHandle)) ? HMDRV_TRUE : HMDRV_FALSE);    
    }
    
    return bRet;
}

/* *****************************************************************************
 * HMREC_GetRecBuffCheckCode
 * 
 * Description: This function returns recorder buffer information - 
 * buffer address and start index to pMessageIO .
 **************************************************************************** */
HMPROTOCOL_eResponseCode_t HMREC_GetRecBuffCheckCode(HMREC_sHandle_t* sHandle)
//HMPROTOCOL_eResponseCode_t HMREC_GetRecBuff(HMREC_sHandle_t* sHandle, HMDRV_BPTR pMessageIO)
{   
    //rework to return pResponse in all functions

    HMPROTOCOL_eResponseCode_t eResponseCode;
    //HMDRV_BPTR pResponse;
    if(!sHandle->uFlags.sFlg.bIsConfigured)
    {
        /* must be configured */
        eResponseCode = HMPROTOCOL_STC_NOTINIT;
    }
    else
    if(sHandle->uFlags.sFlg.bIsRunning)
    {
        /* must be stopped */
        eResponseCode = HMPROTOCOL_STC_SERVBUSY;
    }
    else
    {
        /* fill the return info - done in protocol module */
        //pResponse = pMessageIO;
        //pResponse = HMDRV_AddressToBuffer(pResponse, sHandle->pBufferAddr);
        //HMDRV_ValueToBuffer16(pResponse, sHandle->u16RecBuffStartIx);
        
        eResponseCode = HMPROTOCOL_STS_OK;
    }
    
    return eResponseCode;
}

/* *****************************************************************************
 * HMREC_GetRecBuffFreemaster
 * 
 * Description: This function returns recorder buffer information - 
 * buffer address and start index to pMessageIO .
 **************************************************************************** */
HMDRV_BPTR HMREC_GetRecBuffFreemaster(HMREC_sHandle_t* sHandle, HMDRV_BPTR pMessageIO)
{   
    HMDRV_BPTR pResponse;
    
    if(!sHandle->uFlags.sFlg.bIsConfigured)
    {
        /* must be configured */
        pResponse = HMDRV_ConstToBuffer8(pMessageIO, HMPROTOCOL_STC_NOTINIT);
    }
    else
    if(sHandle->uFlags.sFlg.bIsRunning)
    {
        /* must be stopped */
        pResponse = HMDRV_ConstToBuffer8(pMessageIO, HMPROTOCOL_STC_SERVBUSY);
    }
    else
    {
        /* fill the return info - done in protocol module */
        pResponse = HMDRV_ConstToBuffer8(pMessageIO, HMPROTOCOL_STS_OK);
        pResponse = HMDRV_AddressToBuffer(pResponse, sHandle->pBufferAddr);
        pResponse = HMDRV_ValueToBuffer16(pResponse, sHandle->u16RecBuffStartIx);
    }
    
    return pResponse;
}

/* *****************************************************************************
 * HMREC_GetRecBuffIndex
 * 
 * Description: This function returns recorder buffer index 
 * (recorder information when PC application knows the buffer address)
 **************************************************************************** */
HMDRV_U16 HMREC_GetRecBuffIndex(HMREC_sHandle_t* sHandle)
{      
    return sHandle->u16RecBuffStartIx;
}



/* *****************************************************************************
 * HMREC_GetRecBuffSize - Get recorder memory size
 * 
 * Description: Recorder memory size in native size of units 
 * (=bytes on most platforms)
 **************************************************************************** */
HMDRV_SIZE HMREC_GetRecBuffSize(HMREC_sHandle_t* sHandle)
{
    return sHandle->nBufferSize;
}

/* *****************************************************************************
 * HMREC_GetRecBuffAddress - Get recorder buffer start address
 * 
 * Description: Recorder buffer start address
 **************************************************************************** */
HMDRV_ADDR HMREC_GetRecBuffAddress(HMREC_sHandle_t* sHandle)
{
    return sHandle->pBufferAddr;
}

/* *****************************************************************************
 * HMREC_GetRecSampleSize - Get recorder sample size
 * 
 * Description: Recorder one sample size in native size of units 
 * (=bytes on most platforms)
 **************************************************************************** */
HMDRV_U8 HMREC_GetRecSampleSize(HMREC_sHandle_t* sHandle)
{
    return sHandle->u8RecSampleSize;
}


/* *****************************************************************************
 * HMREC_AbortRec
 * 
 * Description: Abort and de-initialize recorder
 **************************************************************************** */
void HMREC_AbortRec(HMREC_sHandle_t* sHandle)
{
    /* clear flags */
    sHandle->uFlags.nAll = 0U;
}


/* *****************************************************************************
 * HMREC_SetUpRecBuff
 * 
 * Description: Replacing the recorder buffer with the user's one
 * Use the HMDRV_SetUpBuff32 to pass the forced 32bit address in SDM
 **************************************************************************** */
void HMREC_SetUpRecBuff(HMREC_sHandle_t* sHandle, HMDRV_ADDR pAddr, HMDRV_SIZE nSize)
{    
    /* convert Buffer Address */
    
    HMDRV_ARR2ADDR(sHandle->pBufferAddr, pAddr);
    
    /* size in native size of units (=bytes on most platforms) */
    HMDRV_ARR2SIZE(sHandle->nBufferSize, nSize);
}

/* *****************************************************************************
 * HMREC_SetUpRec
 * 
 * Description: Handling SETUPREC and SETUPREC_EX commands
 *          As all command handlers, the return value should be the buffer 
 *           pointer where the response output finished (except checksum)
 **************************************************************************** */
HMPROTOCOL_eResponseCode_t HMREC_SetUpRec(HMREC_sHandle_t* sHandle, HMDRV_BPTR pMessageIO)
{
    HMPROTOCOL_eResponseCode_t eResponseCode = HMPROTOCOL_STS_OK;
    HMDRV_SIZE8 nRecVarsetSize;
    HMDRV_SIZE blen;
    HMDRV_U8 i, sz;
    
    /* de-initialize first   */
    HMREC_AbortRec(sHandle);

    /* check if buffer valid */
    if(!sHandle->pBufferAddr || !sHandle->nBufferSize)
    {
        eResponseCode = HMPROTOCOL_STE_INVBUFF;
    }
    else
    {
    
        /* seek the setup data */
        pMessageIO = HMDRV_SkipInBuffer(pMessageIO, 2U);
        pMessageIO = HMDRV_ValueFromBuffer8(&sHandle->u8RecTriggerMode, pMessageIO);
        pMessageIO = HMDRV_ValueFromBuffer16(&sHandle->u16RecTotalSmps, pMessageIO);
        pMessageIO = HMDRV_ValueFromBuffer16(&sHandle->u16RecPostTrigger, pMessageIO);
        pMessageIO = HMDRV_ValueFromBuffer16(&sHandle->u16RecTimeDiv, pMessageIO);

        /* address & size of trigger variable */
        pMessageIO = HMDRV_AddressFromBuffer(&sHandle->pu8TrgVarAddr, pMessageIO);
        pMessageIO = HMDRV_ValueFromBuffer8(&sHandle->u8TrgVarSize, pMessageIO);

        /* trigger compare mode  */
        pMessageIO = HMDRV_ValueFromBuffer8(&sHandle->u8TrgVarSigned, pMessageIO);

        /* threshold value  */
        pMessageIO = HMDRV_ValueFromBuffer32(&sHandle->uTrgThreshold.u32, pMessageIO);

        /* recorder variable count */
        pMessageIO = HMDRV_ValueFromBuffer8(&sHandle->u8RecVarCount, pMessageIO);

        /* rec variable information must fit into our buffers */
        if(!sHandle->u8RecVarCount || sHandle->u8RecVarCount > (HMDRV_U8)HMREC_MAX_VARS)
        {
            eResponseCode = HMPROTOCOL_STE_INVBUFF;
        }
        else
        {


            /* calculate sum of sizes of all variables */
            nRecVarsetSize = 0U;

            /* get all addresses and sizes */
            for(i=0U; i<sHandle->u8RecVarCount; i++)
            {
                /* variable size */
                pMessageIO = HMDRV_ValueFromBuffer8(&sz, pMessageIO);

                sHandle->u8RecVarSize[i] = sz;
                nRecVarsetSize += sz;

                /* variable address */
                pMessageIO = HMDRV_AddressFromBuffer(&sHandle->pu8RecVarAddr[i], pMessageIO);

                /* valid numeric variable sizes only */
                if(sz == 0U || sz > 8U)
                {
                    eResponseCode = HMPROTOCOL_STE_INVSIZE;
                    i = sHandle->u8RecVarCount;
                }

                #if HMDRV_CFG_BUS_WIDTH > 1U
                /* even sizes only */
                if(sz & 0x1)
                {
                    eResponseCode = HMPROTOCOL_STE_INVSIZE;
                    i = sHandle->u8RecVarCount;
                }
                #endif /* HMDRV_CFG_BUS_WIDTH > 1U */

            }
            sHandle->u8RecSampleSize = nRecVarsetSize;

            if (eResponseCode == HMPROTOCOL_STS_OK)
            {

                /* any trigger? */
                sHandle->pfCompareFunc = NULL;
                if(sHandle->u8RecTriggerMode)
                {
                    /* access to trigger variable? */
                    /* get compare function */

                    if(sHandle->u8TrgVarSigned & HMREC_FLOAT_TRIG_MASK)
                    {
                        #if HMREC_FLOAT_TRIG
                        sHandle->pfCompareFunc = (PCOMPAREFUNC_Dummy)HMREC_Comparefloat;
                        #else
                        eResponseCode = HMPROTOCOL_STE_FLOATDISABLED;
                        #endif
                    }
                    else
                    {
                        switch(sHandle->u8TrgVarSize)
                        {
                        #if HMDRV_CFG_BUS_WIDTH == 1U
                        case 1: 
                            sHandle->pfCompareFunc = sHandle->u8TrgVarSigned 
                                    ? (PCOMPAREFUNC_Dummy)HMREC_Compare8S
                                    : (PCOMPAREFUNC_Dummy)HMREC_Compare8U;
                            break;
                        #endif
                        case 2: 
                            sHandle->pfCompareFunc = sHandle->u8TrgVarSigned 
                                    ? (PCOMPAREFUNC_Dummy)HMREC_Compare16S
                                    : (PCOMPAREFUNC_Dummy)HMREC_Compare16U;
                            break;

                        case 4: 
                            sHandle->pfCompareFunc = sHandle->u8TrgVarSigned 
                                    ? (PCOMPAREFUNC_Dummy)HMREC_Compare32S
                                    : (PCOMPAREFUNC_Dummy)HMREC_Compare32U;
                            break;
                        /* invalid trigger variable size  */
                        default:
                            eResponseCode = HMPROTOCOL_STE_INVSIZE;
                            break;
                        }
                    }
                }

                if (eResponseCode == HMPROTOCOL_STS_OK)
                {
                    /* total recorder buffer length in native sizeof units (=bytes on most platforms) */
                    blen = (HMDRV_SIZE) (sHandle->u16RecTotalSmps * nRecVarsetSize / HMDRV_CFG_BUS_WIDTH);

                    /* recorder memory available? */
                    if(blen > HMREC_GetRecBuffSize(sHandle))
                    {
                        eResponseCode = HMPROTOCOL_STE_INVSIZE;
                    }
                    else
                    {
                        /* remember the effective end of circular buffer */
                        sHandle->pEndBuffPtr = sHandle->pBufferAddr + blen;

                        /* everything is okay    */
                        sHandle->uFlags.sFlg.bIsConfigured = 1U;

                        /* eResponseCode = HMPROTOCOL_STS_OK; - not needed - made above */ 
                    }
                }
            }
        }    
    }
    return eResponseCode;
}

/* *****************************************************************************
 * HMREC_GetRecStatus
 * 
 * Description: Handling GETRECSTS command.
 * As all command handlers, the return value should be the buffer 
 *           pointer where the response output finished (except checksum)
 *
 * This function returns current recorder status
 **************************************************************************** */
HMPROTOCOL_eResponseCode_t HMREC_GetRecStatus(HMREC_sHandle_t* sHandle)
{
    HMPROTOCOL_eResponseCode_t eResponseCode;
    
    eResponseCode = 
            sHandle->uFlags.sFlg.bIsRunning ? 
                HMPROTOCOL_STS_RECRUN : 
                HMPROTOCOL_STS_RECDONE;
    
    /* must be configured */
    if(!sHandle->uFlags.sFlg.bIsConfigured)
    {
        eResponseCode = HMPROTOCOL_STC_NOTINIT;
    }
        
    /* get run/stop status */
    return eResponseCode;
}
#endif
