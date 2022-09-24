/* *****************************************************************************
 * File:   hmrec.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 01 02 19:56
 * 
 * Description: Hex Monitor Recorder
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef HMREC_H
#define	HMREC_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "hmprotocol.h"

/*****************************************************************************
* Recorder support
******************************************************************************/
#ifndef HMREC_MAX_VARS
#ifdef  HMDRV_MAX_REC_VARS
#define HMREC_MAX_VARS     HMDRV_MAX_REC_VARS
#else
#define HMREC_MAX_VARS     8  /* max. number of recorder variables */
#endif
#endif


#define HMREC_FLOAT_TRIG_MASK      0x02

#ifndef HMREC_EXECUTE_MICROSEC
#define HMREC_EXECUTE_MICROSEC  100
#endif


/* recorder time base declaration helpers */
#define HMREC_BASE_SECONDS(x)  ((x) & 0x3fff)
#define HMREC_BASE_MILLISEC(x) (((x) & 0x3fff) | 0x4000)
#define HMREC_BASE_MICROSEC(x) (((x) & 0x3fff) | 0x8000)
#define HMREC_BASE_NANOSEC(x)  (((x) & 0x3fff) | 0xc000)


/* recorder time base, specifies how often the recorder is called in the user app. */
//#define HMREC_TIMEBASE     HMREC_BASE_NANOSEC((unsigned short)(HMREC_EXECUTE_NANOSEC)) /* 0 = "unknown" */
#define HMREC_TIMEBASE     HMREC_BASE_MICROSEC((unsigned short)(HMREC_EXECUTE_MICROSEC)) /* 0 = "unknown" */

#define HMREC_FLOAT_TRIG   HMDRV_FLOAT_TRIG_ENABLE    /* enable/disable floating point triggering */


/***********************************
*  global recorder types
***********************************/


/* trigger threshold level (1,2 or 4 bytes) */
typedef union 
{
    HMDRV_U8  u8; 
    HMDRV_S8  s8;
    HMDRV_U16 u16;
    HMDRV_S16 s16;
    HMDRV_U32 u32;
    HMDRV_S32 s32;
    HMDRV_FLOAT fp;
} HMREC_uTrgThreshold_t;                        

/* compare functions prototype */
typedef HMDRV_BOOL (*PCOMPAREFUNC_Dummy)(void);


/* recorder runtime flags */
typedef struct
{
        unsigned bIsConfigured : 1;     /* recorder is configured */
        unsigned bIsRunning : 1;        /* recorder is running */
        unsigned bIsStopping : 1;       /* trigger activated, sample countdown */
        unsigned bInvirginCycle : 1;    /* virgin cycle of the circular buffer in-progress */
        unsigned bTrgCrossActive : 1;   /* trigger threshold was crossed */
        unsigned bUseOwnBuffer : 1;     /* use external specified from the user not dedicated for the module buffer */
    
}HMREC_sFlags_t;

typedef union 
{
    HMDRV_FLAGS     nAll;
    HMREC_sFlags_t    sFlg;   
} HMREC_uFlags_t;


/* Recorder Handle */
typedef struct
{
	HMREC_uFlags_t  uFlags;                 /* recorder flags */
	HMDRV_ADDR      pBufferAddr;            /* recorder buffer address */
    HMDRV_SIZE      nBufferSize;            /* recorder buffer size */

    /* Recorder buffer pointers */
    HMDRV_ADDR      pWritePtr;              /* write pointer in buffer */
    HMDRV_ADDR      pEndBuffPtr;            /* pointer to end of buffer */

    /* one sample are all records processed at one pass */
    HMDRV_U16       u16RecBuffStartIx;      /* first sample index */
    
    /* pointer to active compare function */
    PCOMPAREFUNC_Dummy    pfCompareFunc;

    /* configuration variables trigger */
    HMDRV_ADDR pu8TrgVarAddr;               /* trigger variable address */
    HMDRV_U8   u8TrgVarSize;                /* trigger variable threshold size */
    HMDRV_U8   u8TrgVarSigned;              /* trigger compare mode (0 = unsigned, 1 = signed) ??? consider boolean */
    HMREC_uTrgThreshold_t uTrgThreshold;

    /* configuration variables */
    HMDRV_U16  u16RecTotalSmps;        /* number of samples to measure */

    HMDRV_U16  u16RecPostTrigger;      /* number of post-trigger samples to keep */

    HMDRV_U16  u16RecTimeDiv;          /* divisor of recorder "clock" */

    HMDRV_U8   u8RecTriggerMode;      /* trigger mode (0 = disabled, 1 = _/, 2 = \_) ??? consider both */

    HMDRV_U8    u8RecVarCount;        /* number of active recorder variables */
    HMDRV_U8    u8RecSampleSize;        /* Size of one sample of active recorder variables - not used and not needed -> but check Modbus uses it and malloc if queues used */
    HMDRV_ADDR  pu8RecVarAddr[HMREC_MAX_VARS]; /* addresses of recorded variables */
    HMDRV_SIZE8 u8RecVarSize[HMREC_MAX_VARS]; /* sizes of recorded variables */

    /* runtime variables  */
    HMDRV_U16  u16RecTimeDivCtr;       /* recorder "clock" divisor counter */

    HMDRV_U16  u16StoprecCountDown;    /* post-trigger countdown counter */


}HMREC_sHandle_t;

/* compare functions prototype */
typedef HMDRV_BOOL (*PCOMPAREFUNC)(HMREC_sHandle_t*);

/* *****************************************************************************
 * External Usage Variables
 **************************************************************************** */
#if (HMDRV_USE_RECORDER) && (!(HMDRV_DISABLE))
#if HMDRV_RECORDER_HANDLE_COUNT > 0
extern HMREC_sHandle_t sRecHandle[HMDRV_RECORDER_HANDLE_COUNT];
#endif

#if HMDRV_RECORDER_BUFINT_COUNT > 0
extern HMDRV_U8 au8RecBuffer0[HMDRV_RECORDER_BUFFER0_SIZE / HMDRV_RECBUFF_BYTE_SIZE];
#endif


#if HMDRV_RECORDER_BUFINT_COUNT > 1
extern HMDRV_U8 au8RecBuffer1[HMDRV_RECORDER_BUFFER1_SIZE / HMDRV_RECBUFF_BYTE_SIZE];
#endif
#endif
/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
#if HMDRV_USE_RECORDER
void HMREC_vInit(HMREC_sHandle_t* sHandle);
void HMREC_vProcess(HMREC_sHandle_t* sHandle);
void HMREC_TriggerRec(HMREC_sHandle_t* sHandle);
void HMREC_AbortRec(HMREC_sHandle_t* sHandle);
void HMREC_SetUpRecBuff(HMREC_sHandle_t* sHandle, HMDRV_ADDR pAddr, HMDRV_SIZE nSize);
HMDRV_BOOL HMREC_IsInRecBuffer(HMREC_sHandle_t* sHandle, HMDRV_ADDR pAddr, HMDRV_SIZE8 nSize);
HMPROTOCOL_eResponseCode_t HMREC_StartRec(HMREC_sHandle_t* sHandle);
HMPROTOCOL_eResponseCode_t HMREC_StopRec(HMREC_sHandle_t* sHandle);
HMPROTOCOL_eResponseCode_t HMREC_GetRecStatus(HMREC_sHandle_t* sHandle);
HMPROTOCOL_eResponseCode_t HMREC_GetRecBuffCheckCode(HMREC_sHandle_t* sHandle);
HMDRV_BPTR HMREC_GetRecBuffFreemaster(HMREC_sHandle_t* sHandle, HMDRV_BPTR pMessageIO);
HMPROTOCOL_eResponseCode_t HMREC_SetUpRec(HMREC_sHandle_t* sHandle, HMDRV_BPTR pMessageIO);
HMDRV_U16 HMREC_GetRecBuffIndex(HMREC_sHandle_t* sHandle);
HMDRV_SIZE HMREC_GetRecBuffSize(HMREC_sHandle_t* sHandle);
HMDRV_U8 HMREC_GetRecSampleSize(HMREC_sHandle_t* sHandle);
HMDRV_ADDR HMREC_GetRecBuffAddress(HMREC_sHandle_t* sHandle);
#else
#define HMREC_vInit(x)
#define HMREC_vProcess(x)
#define HMREC_TriggerRec(x)
#define HMREC_AbortRec(x)
#define HMREC_SetUpRecBuff(x,y,z)
#define HMREC_IsInRecBuffer(x,y,z) 0
#define HMREC_StartRec(x) 0
#define HMREC_StopRec(x) 0
#define HMREC_GetRecStatus(x) 0
#define HMREC_GetRecBuffCheckCode(x) 0
#define HMREC_GetRecBuffFreemaster(x,y) 0
#define HMREC_SetUpRec(x,y) 0
#define HMREC_GetRecBuffIndex(x) 0
#define HMREC_GetRecBuffSize(x) 0
#define HMREC_GetRecSampleSize(x) 0
#define HMREC_GetRecBuffAddress(x) 0
#endif

#endif	/* HMREC_H */
