/* *****************************************************************************
 * File:   hmrecparam.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 11 03 11:46
 * 
 * Description: Hex Monitor Recorder Parameters
 * 
 **************************************************************************** */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "hmdrv.h"
//#include "hmprotocol.h"
#include "hmrecparam.h"

/* *****************************************************************************
 * Check if Not Used
 **************************************************************************** */
#if (HMDRV_USE_RECORDER_PARAMETERS) && (!(HMDRV_DISABLE))

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

typedef struct
{
    uint_least8_t u8Reserved;
    uint_least8_t u8Year;
    uint_least8_t u8Month;
    uint_least8_t u8Day;
    uint_least8_t u8WeekDays;
    uint_least8_t u8Hours;
    uint_least8_t u8Minutes;
    uint_least8_t u8Seconds;
}HMRECPRM_sDateTime_t;





/* *****************************************************************************
 * Variables
 **************************************************************************** */
#if HMDRV_RECORDER_PARAM_HANDLE_COUNT > 0
HMRECPRM_sHandle_t              sRecHandleParam[HMDRV_RECORDER_PARAM_HANDLE_COUNT];
HMRECPRM_sStatus_t      sRecStatusParam[HMDRV_RECORDER_PARAM_HANDLE_COUNT] =
{
 /* Day   Month Year  Flags    Weekday Hour   Min   Sec     usec        reserved */
 { {0x01, 0x01, 0x01, 0,0,0,0}, {0x01, 0x00, 0x00, 0x00}, 0x00000000, 0x00000000 },
#if HMDRV_RECORDER_PARAM_HANDLE_COUNT > 1
 { {0x01, 0x01, 0x01, 0,0,0,0}, {0x01, 0x00, 0x00, 0x00}, 0x00000000, 0x00000000 },
#endif
#if HMDRV_RECORDER_PARAM_HANDLE_COUNT > 2
 { {0x01, 0x01, 0x01, 0,0,0,0}, {0x01, 0x00, 0x00, 0x00}, 0x00000000, 0x00000000 },
#endif
};
#endif






#if HMDRV_RECORDER_PARAM_BUFINT_COUNT > 0
HMDRV_U8 au8RecParamBuffer0[HMDRV_RECORDER_PARAM_BUFFER0_SIZE / HMDRV_RECBUFF_BYTE_SIZE];
#endif


#if HMDRV_RECORDER_PARAM_BUFINT_COUNT > 1
HMDRV_U8 au8RecParamBuffer1[HMDRV_RECORDER_PARAM_BUFFER1_SIZE / HMDRV_RECBUFF_BYTE_SIZE];
#endif

#if HMDRV_RECORDER_PARAM_BUFINT_COUNT > 2
HMDRV_U8 au8RecParamBuffer2[HMDRV_RECORDER_PARAM_BUFFER2_SIZE / HMDRV_RECBUFF_BYTE_SIZE];
#endif

HMRECPRM_uTimeBaseValue_t sRecTimeBase[HMDRV_RECORDER_TIME_BASE_COUNT] = 
{
    {HMRECPRM_EXECUTE_MICROSEC_BASE_0},
    
    #if HMDRV_RECORDER_TIME_BASE_COUNT > 1
    {HMRECPRM_EXECUTE_MICROSEC_BASE_1},
    #endif

    #if HMDRV_RECORDER_TIME_BASE_COUNT > 2
    {HMRECPRM_EXECUTE_MICROSEC_BASE_2},
    #endif
    
    #if HMDRV_RECORDER_TIME_BASE_COUNT > 3
    {HMRECPRM_EXECUTE_MICROSEC_BASE_4},
    #endif
};







/* *****************************************************************************
 * Variables - Time Synchronization
 **************************************************************************** */
#if BOARDCFG_USE_ANDREAS_INTERFACE_RTC == 0
RTC_DRV_sTimeSyncWWHHMMSS_t     MAIN_sTimeI2C;
RTC_DRV_sTimeSyncModeYYMMDD_t   MAIN_sDateI2C;
#else
SYS_RTC_sTime_t                 MAIN_sTimeI2C;
SYS_RTC_sDate_t                 MAIN_sDateI2C;
#endif

HMRECPRM_sTimeSyncWWHHMMSS_t    MAIN_sTime;
HMRECPRM_sTimeSyncModeYYMMDD_t  MAIN_sDate;
uint32_t MAIN_u32FreeCounter;
uint32_t u32RecorderMicroSec[HMDRV_RECORDER_PARAM_HANDLE_COUNT];
bool abRecorderJustStopped[HMDRV_RECORDER_PARAM_HANDLE_COUNT];
bool abRecorderRunning[HMDRV_RECORDER_PARAM_HANDLE_COUNT];
uint32_t u32RecorderLastFreeCounter[HMDRV_RECORDER_PARAM_HANDLE_COUNT];



HMRECPRM_sDateTime_t HMRECPRM_sDateTime =
{
     0x00,     /* uint_least8_t u8Reserved; */
     0x01,     /* uint_least8_t u8Year;     */
     0x01,     /* uint_least8_t u8Month;    */
     0x01,     /* uint_least8_t u8Day;      */
     0x01,     /* uint_least8_t u8WeekDays; */
     0x00,     /* uint_least8_t u8Hours;    */
     0x00,     /* uint_least8_t u8Minutes;  */
     0x00,     /* uint_least8_t u8Seconds;  */
};





/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void HMRECPRM_vResetAllStopDetections(void);

/* *****************************************************************************
 * HMRECPRM_vInit
 * 
 * Description: Module Initialization
 **************************************************************************** */
void HMRECPRM_vInit(HMRECPRM_sHandle_t* sHandle)
{
    /* initialize Recorder flags*/
    sHandle->uFlags.nAll = 0U;
    
    /* reset buffer address and size until initialized */
    sHandle->pBufferAddr = NULL;
    sHandle->nBufferSize = 0U;
    
    sHandle->u16RecTimeBase = HMRECPRM_TIMEBASE;
}


/* *****************************************************************************
 * HMRECPRM_vInitParam
 * 
 * Description: Module Initialization
 **************************************************************************** */
void HMRECPRM_vInitParam(HMRECPRM_sHandle_t* sHandle, HMDRV_U16 u16ConfigModeIndex)
{
    /* initialize Recorder flags*/
    sHandle->uFlags.nAll = 0U;
    
    /* reset buffer address and size until initialized */
    sHandle->pBufferAddr = NULL;
    sHandle->nBufferSize = 0U;
    if (u16ConfigModeIndex < HMDRV_RECORDER_TIME_BASE_COUNT)
        sHandle->u16RecTimeBase = (uint16_t)sRecTimeBase[u16ConfigModeIndex].u16Register;
    else
        sHandle->u16RecTimeBase = HMRECPRM_TIMEBASE;
}


/* *****************************************************************************
 * HMRECPRM_vProcess
 * 
 * Description: Process Recorder
 **************************************************************************** */
void HMRECPRM_vProcess(HMDRV_U16 u16RecorderIndex)
{
    HMRECPRM_sHandle_t*                 sHandle = &sRecHandleParam[u16RecorderIndex];
    HMRECPRM_sStatus_t*         sStatus = &sRecStatusParam[u16RecorderIndex];

    HMDRV_U8 sz;
    #if HMDRV_USE_EXT_MEM
    HMDRV_U8 size;
    #endif
    HMDRV_BOOL cmp;
    HMDRV_U8 i;

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
                
                if (pSnapshotPtr)
                {
                    size = 0;
                    for (i=0U; i<sHandle->u8RecVarCount; i++)
                    {

                        sz = sHandle->uRecVarSize[i].u8Register;
                        if (((HMDRV_uVariableSizeOffset_t)sz).sOffset.bBitField)
                        {
                            sz = 1;
                        }

                        HMDRV_CopyMemory(pSnapshotPtr, sHandle->pu8RecVarAddr[i], sz);

                        #if HMDRV_CFG_BUS_WIDTH > 1
                        sz /= HMDRV_CFG_BUS_WIDTH;
                        #endif
                        size += sz;
                        sHandle->pWritePtr += sz;
                        pSnapshotPtr += sz;		
                    }	
                    HMDRV_CopySnapshotToMemory(sHandle->pWritePtr, size, &u16GlobalQueueID);
                }
                else
                {
                    //cannot copy snapshot because copy buffer busy
                }
                
            }
            else
            #endif
            {
                for (i=0U; i<sHandle->u8RecVarCount; i++)
                {
                    sz = sHandle->uRecVarSize[i].u8Register;
                    if (((HMDRV_uVariableSizeOffset_t)sz).sOffset.bBitField)
                    {
                        sz = 1;
                    }

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

            sHandle->uRecSamplesAfterStopDetect++;

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
                    cmp = ((HMRECPRM_PCOMPAREFUNC)(sHandle->pfCompareFunc))(sHandle);

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
                            HMRECPRM_TriggerRec(sHandle);
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
                    if(!sHandle->nStoprecCountDown)
                    {
                        /* STOP RECORDER */
                        sHandle->uFlags.sFlg.bIsRunning = 0U;
                        if (sHandle->uFlags.sFlg.bStopDetectEnable)
                        {
                            sHandle->uFlags.sFlg.bStopDetected = 1U;
                        }
                        sStatus->sTimeSyncModeYYMMDD.b2RecorderStatus = HMRECPRM_REC_STA_DONE;

                    }
                    else
                    {
                        /* perhaps next time */
                        sHandle->nStoprecCountDown--; 
                    }
                }
            }
        }
	}
}

HMDRV_BOOL HMRECPRM_IsJustStopped(HMDRV_U16 u16RecorderIndex)
{
    HMDRV_BOOL bResult;

    bResult = sRecHandleParam[u16RecorderIndex].uFlags.sFlg.bStopDetected;
    sRecHandleParam[u16RecorderIndex].uFlags.sFlg.bStopDetected = 0;

    return bResult;
}

HMDRV_BOOL HMRECPRM_IsRunning(HMDRV_U16 u16RecorderIndex)
{
    HMDRV_BOOL bResult;

    bResult = sRecHandleParam[u16RecorderIndex].uFlags.sFlg.bIsRunning;

    return bResult;
}


/* *****************************************************************************
 * HMRECPRM_TriggerRec  - optimize this function call in process for faster execution
 * 
 * Description: Starts the post-trigger stop countdown
 **************************************************************************** */
void HMRECPRM_TriggerRec(HMRECPRM_sHandle_t* sHandle)
{
    if(!sHandle->uFlags.sFlg.bIsStopping)
    {
        sHandle->uFlags.sFlg.bIsStopping = 1U;
        sHandle->nStoprecCountDown = (HMDRV_SIZE_FLEX)((HMRECPRM_u32Union_t)(sHandle->uRecPostTrigger)).u32Register;
        //sHandle->nStoprecCountDown = (HMDRV_SIZE_FLEX)(sHandle->uRecPostTrigger.u32Register);
    }
}

void HMRECPRM_vResetAllStopDetections(void)
{
    HMDRV_U16 index;
    for (index = 0; index < HMDRV_RECORDER_PARAM_HANDLE_COUNT; index++)
    {
        sRecHandleParam[index].uFlags.sFlg.bStopDetected = 0;
        sRecHandleParam[index].uFlags.sFlg.bStopDetectEnable = 1;
    }
}

void HMRECPRM_vDisableAllStopDetections(void)
{
    HMDRV_U16 index;
    for (index = 0; index < HMDRV_RECORDER_PARAM_HANDLE_COUNT; index++)
    {
        sRecHandleParam[index].uFlags.sFlg.bStopDetected = 0;
        sRecHandleParam[index].uFlags.sFlg.bStopDetectEnable = 0;
    }
}


/* *****************************************************************************
 * HMRECPRM_StartRec 
 * 
 * Description: This function starts recording (initializes internal recording variables 
 * and flags)
 **************************************************************************** */
HMPROTOCOL_eResponseCode_t HMRECPRM_StartRec(HMRECPRM_sHandle_t* sHandle)
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

        HMRECPRM_vResetAllStopDetections();

        eResponseCode = HMPROTOCOL_STS_OK;
    }
    return eResponseCode;
}



/* *****************************************************************************
 * HMRECPRM_StartRecParam
 * 
 * Description: This function starts recording (initializes internal recording variables 
 * and flags)
 **************************************************************************** */
HMRECPRM_eResponseCode_t HMRECPRM_StartRecParam(HMDRV_U16 u16RecorderIndex)
{
    HMRECPRM_sHandle_t*                 sHandle = &sRecHandleParam[u16RecorderIndex];
    HMRECPRM_sStatus_t*         sStatus = &sRecStatusParam[u16RecorderIndex];
    HMRECPRM_eResponseCode_t eResponseCode;
     
    /* must be configured */
    if(!sHandle->uFlags.sFlg.bIsConfigured)
    {
        eResponseCode = HMRECPRM_CODE_NOTINIT;
    }
    else    
    /* already running ? */
    if(sHandle->uFlags.sFlg.bIsRunning)
    {
        eResponseCode = HMRECPRM_CODE_RECRUN;
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
        sStatus->sTimeSyncModeYYMMDD.b2RecorderStatus = HMRECPRM_REC_STA_RUNNING;

        HMRECPRM_vResetAllStopDetections();

        eResponseCode = HMRECPRM_CODE_OK;
    }
    return eResponseCode;
}



/* *****************************************************************************
 * HMRECPRM_StopRec 
 * 
 * Description: Stop Recording (Manual trigger)
 **************************************************************************** */
HMPROTOCOL_eResponseCode_t HMRECPRM_StopRec(HMRECPRM_sHandle_t* sHandle)
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
        HMRECPRM_TriggerRec(sHandle);
        eResponseCode = HMPROTOCOL_STS_OK;
    }
    
    return eResponseCode;
}

/* *****************************************************************************
 * HMRECPRM_StopRecParam
 * 
 * Description: Stop Recording (Manual trigger)
 **************************************************************************** */
HMRECPRM_eResponseCode_t HMRECPRM_StopRecParam(HMDRV_U16 u16RecorderIndex)
{
    HMRECPRM_sHandle_t*                 sHandle = &sRecHandleParam[u16RecorderIndex];
    //HMRECPRM_sStatus_t*         sStatus = &sRecStatusParam[u16RecorderIndex];
    HMRECPRM_eResponseCode_t eResponseCode;
    
    if(!sHandle->uFlags.sFlg.bIsConfigured)
    {
        /* must be configured */
        eResponseCode = HMRECPRM_CODE_NOTINIT;   
    }
    else    
    if(!sHandle->uFlags.sFlg.bIsRunning)
    {
        /* already stopped */
        eResponseCode = HMRECPRM_CODE_RECDONE;
    }
    else
    {
        /* simulate trigger */
        HMRECPRM_TriggerRec(sHandle);
        eResponseCode = HMRECPRM_CODE_OK;
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

HMDRV_BOOL HMRECPRM_CompareBOOL(HMRECPRM_sHandle_t* sHandle)
{
    return CMP(HMDRV_GetU8(sHandle->pu8TrgVarAddr) >> sHandle->uTrgVarSize.sOffset.u7ValueOffset, sHandle->uTrgThreshold.u8);
}

HMDRV_BOOL HMRECPRM_Compare8S(HMRECPRM_sHandle_t* sHandle)
{
    return CMP(HMDRV_GetS8(sHandle->pu8TrgVarAddr), sHandle->uTrgThreshold.s8);
}

HMDRV_BOOL HMRECPRM_Compare8U(HMRECPRM_sHandle_t* sHandle)
{
    return CMP(HMDRV_GetU8(sHandle->pu8TrgVarAddr), sHandle->uTrgThreshold.u8);
}

HMDRV_BOOL HMRECPRM_Compare16S(HMRECPRM_sHandle_t* sHandle)
{
    return CMP(HMDRV_GetS16(sHandle->pu8TrgVarAddr), sHandle->uTrgThreshold.s16);
}

HMDRV_BOOL HMRECPRM_Compare16U(HMRECPRM_sHandle_t* sHandle)
{
    return CMP(HMDRV_GetU16(sHandle->pu8TrgVarAddr), sHandle->uTrgThreshold.u16);
}

HMDRV_BOOL HMRECPRM_Compare32S(HMRECPRM_sHandle_t* sHandle)
{
    return CMP(HMDRV_GetS32(sHandle->pu8TrgVarAddr), sHandle->uTrgThreshold.s32);
}

HMDRV_BOOL HMRECPRM_Compare32U(HMRECPRM_sHandle_t* sHandle)
{
    return CMP(HMDRV_GetU32(sHandle->pu8TrgVarAddr), sHandle->uTrgThreshold.u32);
}

#if HMRECPRM_FLOAT_TRIG
HMDRV_BOOL HMRECPRM_Comparefloat(HMRECPRM_sHandle_t* sHandle)
{
    return CMP(HMDRV_GetFloat(sHandle->pu8TrgVarAddr), sHandle->uTrgThreshold.fp);
}
#endif



/* *****************************************************************************
 * HMRECPRM_IsInRecBuffer
 * 
 * Description: Check whether given memory region is inside the recorder buffer.
 * This function returns non-zero if user space is in recorder buffer.
 **************************************************************************** */
HMDRV_BOOL HMRECPRM_IsInRecBuffer(HMRECPRM_sHandle_t* sHandle, HMDRV_ADDR pAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_BOOL bRet = 0U;
    
    if(pAddr >= sHandle->pBufferAddr)
    {
        bRet = (HMDRV_BOOL)((pAddr + nSize) <= (sHandle->pBufferAddr + HMRECPRM_GetRecBuffSize(sHandle)) ? HMDRV_TRUE : HMDRV_FALSE);    
    }
    
    return bRet;
}

/* *****************************************************************************
 * HMRECPRM_GetRecBuffCheckCode
 * 
 * Description: This function returns recorder buffer information - 
 * buffer address and start index to pMessageIO .
 **************************************************************************** */
HMPROTOCOL_eResponseCode_t HMRECPRM_GetRecBuffCheckCode(HMRECPRM_sHandle_t* sHandle)
//HMPROTOCOL_eResponseCode_t HMRECPRM_GetRecBuff(HMRECPRM_sHandle_t* sHandle, HMDRV_BPTR pMessageIO)
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
 * HMRECPRM_GetRecBuffCheckCode
 * 
 * Description: This function returns recorder buffer information - 
 * buffer address and start index to pMessageIO .
 **************************************************************************** */
HMRECPRM_eResponseCode_t HMRECPRM_GetRecBuffCheckCodeParam(HMRECPRM_sHandle_t* sHandle)
//HMPROTOCOL_eResponseCode_t HMRECPRM_GetRecBuff(HMRECPRM_sHandle_t* sHandle, HMDRV_BPTR pMessageIO)
{   
    //rework to return pResponse in all functions

    HMRECPRM_eResponseCode_t eResponseCode;
    //HMDRV_BPTR pResponse;
    if(!sHandle->uFlags.sFlg.bIsConfigured)
    {
        /* must be configured */
        eResponseCode = HMRECPRM_CODE_NOTINIT;
    }
    else
    if(sHandle->uFlags.sFlg.bIsRunning)
    {
        /* must be stopped */
        eResponseCode = HMRECPRM_CODE_SERVBUSY;
    }
    else
    {
        /* fill the return info - done in protocol module */
        //pResponse = pMessageIO;
        //pResponse = HMDRV_AddressToBuffer(pResponse, sHandle->pBufferAddr);
        //HMDRV_ValueToBuffer16(pResponse, sHandle->u16RecBuffStartIx);
        
        eResponseCode = HMRECPRM_CODE_OK;
    }
    
    return eResponseCode;
}

/* *****************************************************************************
 * HMRECPRM_GetRecBuffFreemaster
 * 
 * Description: This function returns recorder buffer information - 
 * buffer address and start index to pMessageIO .
 **************************************************************************** */
HMDRV_BPTR HMRECPRM_GetRecBuffFreemaster(HMRECPRM_sHandle_t* sHandle, HMDRV_BPTR pMessageIO)
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
 * HMRECPRM_GetRecBuffFreemaster
 * 
 * Description: This function returns recorder buffer information - 
 * buffer address and start index to pMessageIO .
 **************************************************************************** */
HMDRV_BPTR HMRECPRM_GetRecBuffFreemasterParam(HMRECPRM_sHandle_t* sHandle, HMDRV_BPTR pMessageIO)
{   
    HMDRV_BPTR pResponse;
    
    if(!sHandle->uFlags.sFlg.bIsConfigured)
    {
        /* must be configured */
        pResponse = HMDRV_ConstToBuffer8(pMessageIO, HMRECPRM_CODE_NOTINIT);
    }
    else
    if(sHandle->uFlags.sFlg.bIsRunning)
    {
        /* must be stopped */
        pResponse = HMDRV_ConstToBuffer8(pMessageIO, HMRECPRM_CODE_SERVBUSY);
    }
    else
    {
        /* fill the return info - done in protocol module */
        pResponse = HMDRV_ConstToBuffer8(pMessageIO, HMRECPRM_CODE_OK);
        pResponse = HMDRV_AddressToBuffer(pResponse, sHandle->pBufferAddr);
        pResponse = HMDRV_ValueToBuffer16(pResponse, sHandle->u16RecBuffStartIx);
    }
    
    return pResponse;
}

/* *****************************************************************************
 * HMRECPRM_GetRecBuffIndex
 * 
 * Description: This function returns recorder buffer index 
 * (recorder information when PC application knows the buffer address)
 **************************************************************************** */
HMDRV_U16 HMRECPRM_GetRecBuffIndex(HMRECPRM_sHandle_t* sHandle)
{      
    return sHandle->u16RecBuffStartIx;
}



/* *****************************************************************************
 * HMRECPRM_GetRecBuffSize - Get recorder memory size
 * 
 * Description: Recorder memory size in native size of units 
 * (=bytes on most platforms)
 **************************************************************************** */
HMDRV_SIZE_FLEX HMRECPRM_GetRecBuffSize(HMRECPRM_sHandle_t* sHandle)
{
    return sHandle->nBufferSize;
}


/* *****************************************************************************
 * HMRECPRM_GetRecBuffAddress - Get recorder buffer start address
 * 
 * Description: Recorder buffer start address
 **************************************************************************** */
HMDRV_ADDR HMRECPRM_GetRecBuffAddress(HMRECPRM_sHandle_t* sHandle)
{
    return sHandle->pBufferAddr;
}

/* *****************************************************************************
 * HMRECPRM_GetRecSampleSize - Get recorder sample size
 * 
 * Description: Recorder one sample size in native size of units 
 * (=bytes on most platforms)
 **************************************************************************** */
HMDRV_U8 HMRECPRM_GetRecSampleSize(HMRECPRM_sHandle_t* sHandle)
{
    return sHandle->u8RecSampleSize;
}

/* *****************************************************************************
 * HMRECPRM_GetTimeBase - Get recorder time base
 * 
 * Description: 
 **************************************************************************** */
HMDRV_U16 HMRECPRM_GetTimeBase(HMRECPRM_sHandle_t* sHandle)
{
    return sHandle->u16RecTimeBase;
}

/* *****************************************************************************
 * HMRECPRM_SetTimeBase - Set recorder time base
 * 
 * Description: 
 **************************************************************************** */
void HMRECPRM_SetTimeBase(HMRECPRM_sHandle_t* sHandle, HMDRV_U16 u16TimeBase)
{
    sHandle->u16RecTimeBase = u16TimeBase;
}


/* *****************************************************************************
 * HMRECPRM_AbortRec
 * 
 * Description: Abort and de-initialize recorder
 **************************************************************************** */
void HMRECPRM_AbortRec(HMRECPRM_sHandle_t* sHandle)
{
    /* clear flags */
    sHandle->uFlags.nAll = 0U;
}


/* *****************************************************************************
 * HMRECPRM_SetUpRecBuff
 * 
 * Description: Replacing the recorder buffer with the user's one
 * Use the HMDRV_SetUpBuff32 to pass the forced 32bit address in SDM
 **************************************************************************** */
void HMRECPRM_SetUpRecBuff(HMRECPRM_sHandle_t* sHandle, HMDRV_ADDR pAddr, HMDRV_SIZE_FLEX nSize)
{    
    /* convert Buffer Address */
    
    HMDRV_ARR2ADDR(sHandle->pBufferAddr, pAddr);
    
    /* size in native size of units (=bytes on most platforms) */
    HMDRV_ARR2SIZEFLEX(sHandle->nBufferSize, nSize);
}

/* *****************************************************************************
 * HMRECPRM_SetUpRec
 * 
 * Description: Handling SETUPREC and SETUPREC_EX commands
 *          As all command handlers, the return value should be the buffer 
 *           pointer where the response output finished (except checksum)
 **************************************************************************** */
HMPROTOCOL_eResponseCode_t HMRECPRM_SetUpRec(HMRECPRM_sHandle_t* sHandle, HMDRV_BPTR pMessageIO)
{
    HMPROTOCOL_eResponseCode_t eResponseCode = HMPROTOCOL_STS_OK;
    HMDRV_SIZE8 nRecVarsetSize;
    HMDRV_U32 blen;
    HMDRV_U8 i, sz;
    
    /* de-initialize first   */
    HMRECPRM_AbortRec(sHandle);

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
        
        //pMessageIO = HMDRV_ValueFromBuffer16(   (HMDRV_U8*)(&sHandle->uRecTotalSmps.au16Word[0]),    pMessageIO);     
        //pMessageIO = HMDRV_ValueFromBuffer16(   (HMDRV_U8*)(&sHandle->uRecPostTrigger.au16Word[0]),  pMessageIO);
        pMessageIO = HMDRV_ValueFromBuffer16(   (HMDRV_U8*)&(((HMRECPRM_u32Union_t*)(&sHandle->uRecTotalSmps))->au16Word[0]),    pMessageIO);     
        pMessageIO = HMDRV_ValueFromBuffer16(   (HMDRV_U8*)&(((HMRECPRM_u32Union_t*)(&sHandle->uRecPostTrigger))->au16Word[0]),  pMessageIO);
        
        
        
        pMessageIO = HMDRV_ValueFromBuffer16(&sHandle->u16RecTimeDiv, pMessageIO);
        

        /* address & size of trigger variable */
        pMessageIO = HMDRV_AddressFromBuffer(&sHandle->pu8TrgVarAddr, pMessageIO);
        pMessageIO = HMDRV_ValueFromBuffer8(&sHandle->uTrgVarSize.u8Register, pMessageIO);

        /* trigger compare mode  */
        pMessageIO = HMDRV_ValueFromBuffer8(&sHandle->u8TrgVarSigned, pMessageIO);

        /* threshold value  */
        pMessageIO = HMDRV_ValueFromBuffer32(&sHandle->uTrgThreshold.u32, pMessageIO);

        /* recorder variable count */
        pMessageIO = HMDRV_ValueFromBuffer8(&sHandle->u8RecVarCount, pMessageIO);

        /* rec variable information must fit into our buffers */
        if(!sHandle->u8RecVarCount || sHandle->u8RecVarCount > (HMDRV_U8)HMRECPRM_MAX_VARS)
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

                sHandle->uRecVarSize[i].u8Register = sz;
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

                    if(sHandle->u8TrgVarSigned & HMRECPRM_FLOAT_TRIG_MASK)
                    {
                        #if HMRECPRM_FLOAT_TRIG
                        sHandle->pfCompareFunc = (HMRECPRM_PCOMPAREFUNC_Dummy)HMRECPRM_Comparefloat;
                        #else
                        eResponseCode = HMPROTOCOL_STE_FLOATDISABLED;
                        #endif
                    }
                    else
                    {
                        switch(sHandle->uTrgVarSize.u8Register)
                        {
                        #if HMDRV_CFG_BUS_WIDTH == 1U
                        case 1: 
                            sHandle->pfCompareFunc = sHandle->u8TrgVarSigned 
                                    ? (HMRECPRM_PCOMPAREFUNC_Dummy)HMRECPRM_Compare8S
                                    : (HMRECPRM_PCOMPAREFUNC_Dummy)HMRECPRM_Compare8U;
                            break;
                        #endif
                        case 2: 
                            sHandle->pfCompareFunc = sHandle->u8TrgVarSigned 
                                    ? (HMRECPRM_PCOMPAREFUNC_Dummy)HMRECPRM_Compare16S
                                    : (HMRECPRM_PCOMPAREFUNC_Dummy)HMRECPRM_Compare16U;
                            break;

                        case 4: 
                            sHandle->pfCompareFunc = sHandle->u8TrgVarSigned 
                                    ? (HMRECPRM_PCOMPAREFUNC_Dummy)HMRECPRM_Compare32S
                                    : (HMRECPRM_PCOMPAREFUNC_Dummy)HMRECPRM_Compare32U;
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
                    blen = (HMDRV_SIZE_FLEX) (sHandle->uRecTotalSmps * nRecVarsetSize / HMDRV_CFG_BUS_WIDTH);

                    /* recorder memory available? */
                    if(blen > HMRECPRM_GetRecBuffSize(sHandle))
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





HMRECPRM_sStatus_t* HMRECPRM_GetStatusRecParamAddress(HMDRV_U8 recorderIndex)
{
    return &sRecStatusParam[recorderIndex];
}

HMDRV_ADDR HMRECPRM_GetBufferRecParamAddress(HMDRV_U8 recorderIndex)
{
    return sRecHandleParam[recorderIndex].pBufferAddr;
}

HMDRV_ADDR HMRECPRM_GetBufferRecParamPtrAddress(HMDRV_U8 recorderIndex)
{
    return HMDRV_PTRFROMADDR(sRecHandleParam[recorderIndex].pBufferAddr);
}


/* *****************************************************************************
 * HMRECPRM_SetUpRecParamStatusDateTime
 *
 * Description:
 **************************************************************************** */
void HMRECPRM_SetUpRecParamStatusDateTime(HMDRV_U16 u16RecorderIndex, HMRECPRM_sTimeSyncWWHHMMSS_t* sTime, HMRECPRM_sTimeSyncModeYYMMDD_t* sDate, HMDRV_U32 u32MicroSec)
{
    HMRECPRM_sStatus_t* sStatus;

    sStatus = &sRecStatusParam[u16RecorderIndex];
    sStatus->sTimeSyncModeYYMMDD.u8Year = sDate->u8Year;
    sStatus->sTimeSyncModeYYMMDD.u8Month = sDate->u8Month;
    sStatus->sTimeSyncModeYYMMDD.u8Day = sDate->u8Day;
    sStatus->sTimeSyncModeYYMMDD.bMicroSec0BuffOvf1Mode = HMRECPRM_TIME_MICRO_SECONDS;
    sStatus->sTimeSyncWWHHMMSS_t.u8Second = sTime->u8Second;
    sStatus->sTimeSyncWWHHMMSS_t.u8Minute = sTime->u8Minute;
    sStatus->sTimeSyncWWHHMMSS_t.u8Hour = sTime->u8Hour;
    sStatus->sTimeSyncWWHHMMSS_t.u8WeekDays = sTime->u8WeekDays;
    sStatus->u32MicroSec0BuffOvf1 = u32MicroSec;
}

void HMRECPRM_ClrLastStopDetectionToRecorderStopSamplesCounter(HMDRV_U16 u16RecorderIndex)
{
    sRecHandleParam[u16RecorderIndex].uRecSamplesAfterStopDetect = 0;
}

/* *****************************************************************************
 * HMRECPRM_GetRecStatus
 * 
 * Description: Handling GETRECSTS command.
 * As all command handlers, the return value should be the buffer 
 *           pointer where the response output finished (except checksum)
 *
 * This function returns current recorder status
 **************************************************************************** */
HMPROTOCOL_eResponseCode_t HMRECPRM_GetRecStatus(HMRECPRM_sHandle_t* sHandle)
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

/* *****************************************************************************
 * HMRECPRM_GetRecStatus
 * 
 * Description: Handling GETRECSTS command.
 * As all command handlers, the return value should be the buffer 
 *           pointer where the response output finished (except checksum)
 *
 * This function returns current recorder status
 **************************************************************************** */
HMRECPRM_eResponseCode_t HMRECPRM_GetRecStatusParam(HMRECPRM_sHandle_t* sHandle)
{
    HMRECPRM_eResponseCode_t eResponseCode;
    
    eResponseCode = 
            sHandle->uFlags.sFlg.bIsRunning ? 
                HMRECPRM_CODE_RECRUN : 
                HMRECPRM_CODE_RECDONE;
    
    /* must be configured */
    if(!sHandle->uFlags.sFlg.bIsConfigured)
    {
        eResponseCode = HMRECPRM_CODE_NOTINIT;
    }
        
    /* get run/stop status */
    return eResponseCode;
}




HMRECPRM_sTimeSyncModeYYMMDD_t RecorderDateFromLocalDateTime(HMRECPRM_sDateTime_t dateTime)
{
    HMRECPRM_sTimeSyncModeYYMMDD_t result;
    result.u8Day =  dateTime.u8Day;
    result.u8Month =  dateTime.u8Month;
    result.u8Year =  dateTime.u8Year;
    return result;
}

HMRECPRM_sTimeSyncWWHHMMSS_t RecorderTimeFromLocalDateTime(HMRECPRM_sDateTime_t dateTime)
{
    HMRECPRM_sTimeSyncWWHHMMSS_t result;
    result.u8Second =  dateTime.u8Seconds;
    result.u8Minute =  dateTime.u8Minutes;
    result.u8Hour =  dateTime.u8Hours;
    result.u8WeekDays =  dateTime.u8WeekDays;
    return result;
}


#if BOARDCFG_USE_ANDREAS_INTERFACE_RTC == 0

HMRECPRM_sTimeSyncModeYYMMDD_t RecorderDateFromRTCDate(RTC_DRV_sTimeSyncModeYYMMDD_t date)
{
    HMRECPRM_sTimeSyncModeYYMMDD_t result;
    result.u8Day =  date.u8Day;
    result.u8Month =  date.u8Month;
    result.u8Year =  date.u8Year;
    result.b3Reserved =  date.b5Reserved;
    result.bMicroSec0BuffOvf1Mode =  date.bMicroSec0BuffOvf1Mode;
    result.b2TimeSyncSampleMode =  date.b2TimeSyncSampleMode;
    return result;
}

HMRECPRM_sTimeSyncWWHHMMSS_t RecorderTimeFromRTCTime(RTC_DRV_sTimeSyncWWHHMMSS_t time)
{
    HMRECPRM_sTimeSyncWWHHMMSS_t result;
    result.u8Second =  time.u8Second;
    result.u8Minute =  time.u8Minute;
    result.u8Hour =  time.u8Hour;
    result.u8WeekDays =  time.u8WeekDays;
    return result;
}
#else
HMRECPRM_sTimeSyncModeYYMMDD_t RecorderDateFromFPGADate(SYS_RTC_sDate_t date)
{
    HMRECPRM_sTimeSyncModeYYMMDD_t result;
    result.u8Day =  date.u8Day;
    result.u8Month =  date.u8Month;
    result.u8Year =  date.u8Year;
    result.b3Reserved =  date.b3Reserved;
    result.bMicroSec0BuffOvf1Mode =  date.bMicroSec0BuffOvf1Mode;
    result.b2TimeSyncSampleMode =  date.b2TimeSyncSampleMode;
    return result;
}

HMRECPRM_sTimeSyncWWHHMMSS_t RecorderTimeFromFPGATime(SYS_RTC_sTime_t time)
{
    HMRECPRM_sTimeSyncWWHHMMSS_t result;
    result.u8Second =  time.u8Second;
    result.u8Minute =  time.u8Minute;
    result.u8Hour =  time.u8Hour;
    result.u8WeekDays =  time.u8WeekDays;
    return result;
}

#endif

void FillTimeData (void)
{
    /* Get Time Data From FPGA_SCI and free - rolling counter */

#if 0
#if BOARDCFG_USE_ANDREAS_INTERFACE_RTC == 0
    MAIN_sDateI2C = RTC_DRV_GetDate();
    MAIN_sTimeI2C = RTC_DRV_GetTime();
#else
    MAIN_sDateI2C = SYS_RTC_GetDate();
    MAIN_sTimeI2C = SYS_RTC_GetTime();
#endif
    MAIN_sDate = RecorderDateFromRTCDate(MAIN_sDateI2C);
    MAIN_sTime = RecorderTimeFromRTCTime(MAIN_sTimeI2C);

#else

    HMRECPRM_vGetDateTime
    (
        &HMRECPRM_sDateTime.u8Seconds,
        &HMRECPRM_sDateTime.u8Minutes,
        &HMRECPRM_sDateTime.u8Hours,
        &HMRECPRM_sDateTime.u8WeekDays,
        &HMRECPRM_sDateTime.u8Day,
        &HMRECPRM_sDateTime.u8Month,
        &HMRECPRM_sDateTime.u8Year
    );
    MAIN_sDate = RecorderDateFromLocalDateTime(HMRECPRM_sDateTime);
    MAIN_sTime = RecorderTimeFromLocalDateTime(HMRECPRM_sDateTime);
#endif

    MAIN_u32FreeCounter = TIMER_vGetTimerMainRollingCounter();
}



/* *****************************************************************************
 * HMRECPRM_vTimeSync
 *
 * Description: Synchronize All Recorders Sample Trigger Time
 **************************************************************************** */
void HMRECPRM_vTimeSync (void)
{
    uint16_t u16Index;
    
    bool bIsJustStopped;
    
    /* Enter Critical Here If Needed */
    
    for (u16Index = 0; u16Index < HMDRV_RECORDER_PARAM_HANDLE_COUNT; u16Index++)
    {
        abRecorderJustStopped[u16Index] = HMRECPRM_IsJustStopped(u16Index);
        abRecorderRunning[u16Index] = HMRECPRM_IsRunning(u16Index);
    }
    FillTimeData();
    
    /* Save Free Running Counter For The next Stop Detection Sample - not really needed when on same ISR */
    for (u16Index = 0; u16Index < HMDRV_RECORDER_PARAM_HANDLE_COUNT; u16Index++)
    {
        u32RecorderLastFreeCounter[u16Index] = MAIN_u32FreeCounter;
    }
    
    bIsJustStopped = false;
    for (u16Index = 0; u16Index < HMDRV_RECORDER_PARAM_HANDLE_COUNT; u16Index++)
    {
        if (abRecorderJustStopped[u16Index])
        {
            bIsJustStopped = true;
        }
    }

    if (bIsJustStopped)
    {
        HMRECPRM_vDisableAllStopDetections();

        for (u16Index = 0; u16Index < HMDRV_RECORDER_PARAM_HANDLE_COUNT; u16Index++)
        {
            if (abRecorderJustStopped[u16Index])
            {
                u32RecorderMicroSec[u16Index] = 0;
                HMRECPRM_SetUpRecParamStatusDateTime(u16Index, &MAIN_sTime, &MAIN_sDate, u32RecorderMicroSec[u16Index]);
                HMRECPRM_ClrLastStopDetectionToRecorderStopSamplesCounter(u16Index);
            }
            else
            {
                if (abRecorderRunning[u16Index])
                {
                    //calc usec
                    u32RecorderMicroSec[u16Index] = (MAIN_u32FreeCounter - u32RecorderLastFreeCounter[u16Index]) * TIMER_MAIN_USEC;
                    HMRECPRM_SetUpRecParamStatusDateTime(u16Index, &MAIN_sTime, &MAIN_sDate, u32RecorderMicroSec[u16Index]); //To do - in NV memory
                    //LastStopDetectionToRecorderStopSamplesCounter - must be in NV memory
                    HMRECPRM_ClrLastStopDetectionToRecorderStopSamplesCounter(u16Index);
                }
            }
        }
    }

    /* Exit Critical Here If Needed */


}










#endif
