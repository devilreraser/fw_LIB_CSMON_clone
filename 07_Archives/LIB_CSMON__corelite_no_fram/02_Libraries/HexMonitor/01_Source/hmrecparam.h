/* *****************************************************************************
 * File:   hmrecparam.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 11 03 11:46
 * 
 * Description: Hex Monitor Recorder Parameters
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef HMRECPARAM_H
#define	HMRECPARAM_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
//#include "hmprotocol.h"  do not use protocol - use modbus instead
#include "hmrecparam_t.h"
#include "hmmodbus.h"









/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */


/* *****************************************************************************
 * External Usage Variables
 **************************************************************************** */
#if (HMDRV_USE_RECORDER_PARAMETERS) && (!(HMDRV_DISABLE))
#if HMDRV_RECORDER_PARAM_HANDLE_COUNT > 0
extern HMRECPRM_sHandle_t sRecHandleParam[HMDRV_RECORDER_PARAM_HANDLE_COUNT];
extern HMRECPRM_sStatus_t sRecStatusParam[HMDRV_RECORDER_PARAM_HANDLE_COUNT];
#endif

#if HMDRV_RECORDER_PARAM_BUFINT_COUNT > 0
extern HMDRV_U8 au8RecParamBuffer0[HMDRV_RECORDER_PARAM_BUFFER0_SIZE / HMDRV_RECBUFF_BYTE_SIZE];
#endif

#if HMDRV_RECORDER_PARAM_BUFINT_COUNT > 1
extern HMDRV_U8 au8RecParamBuffer1[HMDRV_RECORDER_PARAM_BUFFER1_SIZE / HMDRV_RECBUFF_BYTE_SIZE];
#endif

#if HMDRV_RECORDER_PARAM_BUFINT_COUNT > 2
extern HMDRV_U8 au8RecParamBuffer2[HMDRV_RECORDER_PARAM_BUFFER2_SIZE / HMDRV_RECBUFF_BYTE_SIZE];
#endif

#endif
/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
#if HMDRV_USE_RECORDER_PARAMETERS

void HMRECPRM_vInit(HMRECPRM_sHandle_t* sHandle);
void HMRECPRM_vProcess(HMDRV_U16 u16RecorderIndex);
void HMRECPRM_TriggerRec(HMRECPRM_sHandle_t* sHandle);
void HMRECPRM_AbortRec(HMRECPRM_sHandle_t* sHandle);
void HMRECPRM_SetUpRecBuff(HMRECPRM_sHandle_t* sHandle, HMDRV_ADDR pAddr, HMDRV_SIZE_FLEX nSize);
HMDRV_BOOL HMRECPRM_IsInRecBuffer(HMRECPRM_sHandle_t* sHandle, HMDRV_ADDR pAddr, HMDRV_SIZE8 nSize);
HMPROTOCOL_eResponseCode_t HMRECPRM_StartRec(HMRECPRM_sHandle_t* sHandle);
HMPROTOCOL_eResponseCode_t HMRECPRM_StopRec(HMRECPRM_sHandle_t* sHandle);
HMPROTOCOL_eResponseCode_t HMRECPRM_GetRecStatus(HMRECPRM_sHandle_t* sHandle);
HMPROTOCOL_eResponseCode_t HMRECPRM_GetRecBuffCheckCode(HMRECPRM_sHandle_t* sHandle);
HMDRV_BPTR HMRECPRM_GetRecBuffFreemaster(HMRECPRM_sHandle_t* sHandle, HMDRV_BPTR pMessageIO);
HMPROTOCOL_eResponseCode_t HMRECPRM_SetUpRec(HMRECPRM_sHandle_t* sHandle, HMDRV_BPTR pMessageIO);
HMDRV_U16 HMRECPRM_GetRecBuffIndex(HMRECPRM_sHandle_t* sHandle);
HMDRV_SIZE_FLEX HMRECPRM_GetRecBuffSize(HMRECPRM_sHandle_t* sHandle);
HMDRV_U8 HMRECPRM_GetRecSampleSize(HMRECPRM_sHandle_t* sHandle);
HMDRV_U16 HMRECPRM_GetTimeBase(HMRECPRM_sHandle_t* sHandle);
void HMRECPRM_SetTimeBase(HMRECPRM_sHandle_t* sHandle, HMDRV_U16 u16TimeBase);
HMDRV_ADDR HMRECPRM_GetRecBuffAddress(HMRECPRM_sHandle_t* sHandle);
void HMRECPRM_vTimeSync(void);

/* *****************************************************************************
 * HMRECPRM_vGetDateTime - external interface Get RTC Value
 **************************************************************************** */
extern void HMRECPRM_vGetDateTime (
        uint_least8_t* pu8BCDSeconds,
        uint_least8_t* pu8BCDMinutes,
        uint_least8_t* pu8BCDHours,
        uint_least8_t* pu8BCDWeekdays,
        uint_least8_t* pu8BCDDay,
        uint_least8_t* pu8BCDMonth,
        uint_least8_t* pu8BCDYear);





#else

#define HMRECPRM_vInit(x) 
#define HMRECPRM_vProcess(x) 
#define HMRECPRM_TriggerRec(x) 
#define HMRECPRM_AbortRec(x) 
#define HMRECPRM_SetUpRecBuff(x,y,z) 
#define HMRECPRM_IsInRecBuffer(x,y,z) 0
#define HMRECPRM_StartRec(x) 0
#define HMRECPRM_StopRec(x) 0
#define HMRECPRM_GetRecStatus(x) 0
#define HMRECPRM_GetRecBuffCheckCode(x) 0
#define HMRECPRM_GetRecBuffFreemaster(x,y) 0
#define HMRECPRM_SetUpRec(x,y) 0
#define HMRECPRM_GetRecBuffIndex(x) 0
#define HMRECPRM_GetRecBuffSize(x) 0
#define HMRECPRM_GetRecSampleSize(x) 0 
#define HMRECPRM_GetTimeBase(x) 0
#define HMRECPRM_SetTimeBase(x,y) 
#define HMRECPRM_GetRecBuffAddress(x) 0 
#define HMRECPRM_vTimeSync()
#endif

#endif	/* HMRECPARAM_H */
