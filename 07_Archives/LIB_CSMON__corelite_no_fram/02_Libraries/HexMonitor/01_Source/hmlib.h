/* *****************************************************************************
 * File:   hmlib.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 01 02 19:56
 * 
 * Description: Hex Monitor Library
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef HMLIB_H
#define	HMLIB_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "hmdrv.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */



/* *****************************************************************************
 * External Variables
 **************************************************************************** */

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void HMLIB_vInit(void);

void HMLIB_vProcess(void);
void HMLIB_vProcessSamePort(void);
void HMLIB_vProcessDifferentPorts(void);

void HMLIB_vProcessRecorders(void);

void HMLIB_vRecorder(HMDRV_U8 u8Index);
void HMLIB_vRecorder0(void);
void HMLIB_vRecorder1(void);
void HMLIB_vRecorder2(void);

void HMLIB_vRecorderParam(HMDRV_U8 u8Index);
void HMLIB_vRecorderParam0(void);
void HMLIB_vRecorderParam1(void);
void HMLIB_vRecorderParam2(void);
void HMLIB_vRecorderParamTimeSync(void);

void HMLIB_vProcessTick(void);

void HMLIB_vScopeDataUpdate(void);
void HMLIB_vScopeParamDataUpdate(void);

HMDRV_ADDR HMLIB_GetRecBuffAddress(HMDRV_U8 u8Index);
HMDRV_U32  HMLIB_GetRecBufferSize(HMDRV_U8 u8Index);
HMDRV_U16  HMLIB_GetRecSampleSize(HMDRV_U8 u8Index);

HMDRV_ADDR HMLIB_GetRecParamBuffAddress(HMDRV_U8 u8Index);
HMDRV_U32  HMLIB_GetRecParamBufferSize(HMDRV_U8 u8Index);
HMDRV_U16  HMLIB_GetRecParamSampleSize(HMDRV_U8 u8Index);


void HMLIB_vProcessSCITXISR (void);
void HMLIB_vProcessSCITXISRWithIndex(HMDRV_U8 u8Index);

void HMLIB_vSetPortIndexHexMon(HMDRV_U8 u8Index);
void HMLIB_vSetPortIndexModBus(HMDRV_U8 u8Index);

void HMLIB_vProcessFastTD(HMDRV_U8 u8Index);

void HMLIB_vDebugSend (void);
void HMLIB_vInitRecParamBuff(bool bUseFreemaster);

void HMLIB_vRecorderSetTimeBase (uint16_t u16MicroSeconds);

#endif	/* HMLIB_H */
