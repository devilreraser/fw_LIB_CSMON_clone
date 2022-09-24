/* *****************************************************************************
 * File:   hmlib.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 01 02 19:56
 * 
 * Description: Hex Monitor Library 
 * 
 **************************************************************************** */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "hmlib.h"
//#include "hmprotocol.h"
#include "hmrec.h"
#include "hmrecparam.h"
#include "hmscope.h"
#include "hmscopeparam.h"
#include "hmmodbus.h"
#include "hmserial.h"

static volatile HMDRV_SIZE8 u8SCIHexMonIndex = HMLIB_HEXMON_UART_MODULE;
static volatile HMDRV_SIZE8 u8SCIModBusIndex = HMLIB_MODBUS_UART_MODULE;



void HMLIB_vInitRecParamBuff(bool bUseFreemaster)
{
    if (bUseFreemaster)
    {
        HMRECPRM_SetUpRecBuff(&sRecHandleParam[1], HMDRV_RECORDER_PARAM_BUFFER1_ADDR, HMDRV_RECORDER_PARAM_BUFFER1_SIZE_FREEMASTER); 
    }
    else
    {
        HMRECPRM_SetUpRecBuff(&sRecHandleParam[1], HMDRV_RECORDER_PARAM_BUFFER1_ADDR, HMDRV_RECORDER_PARAM_BUFFER1_SIZE);
    }
}


/* *****************************************************************************
 * HMLIB_vInit
 * 
 * Description: Module Initialization
 **************************************************************************** */
void HMLIB_vInit(void)
{
    #if ( ( (HMDRV_USE_RECORDER)            && (!(HMDRV_DISABLE)) && (HMDRV_RECORDER_HANDLE_COUNT       > 1) ) \
       || ( (HMDRV_USE_RECORDER_PARAMETERS) && (!(HMDRV_DISABLE)) && (HMDRV_RECORDER_PARAM_HANDLE_COUNT > 1) )  ) 
    HMDRV_U8 index;
    #endif
    
    HMPROTOCOL_vInit();

    #if (HMDRV_USE_RECORDER) && (!(HMDRV_DISABLE))
    #if HMDRV_RECORDER_HANDLE_COUNT == 1
    HMREC_vInit(&sRecHandle[0]);
    HMREC_SetUpRecBuff(&sRecHandle[0], HMDRV_RECORDER_BUFFER0_ADDR, HMDRV_RECORDER_BUFFER0_SIZE);
    #else
    index = 0;
    while(index < HMDRV_RECORDER_HANDLE_COUNT)
    {
        HMREC_vInit(&sRecHandle[index]);
        index++;
    }
    HMREC_SetUpRecBuff(&sRecHandle[0], HMDRV_RECORDER_BUFFER0_ADDR, HMDRV_RECORDER_BUFFER0_SIZE);
    #if HMDRV_RECORDER_BUFFER_COUNT > 1
    HMREC_SetUpRecBuff(&sRecHandle[1], HMDRV_RECORDER_BUFFER1_ADDR, HMDRV_RECORDER_BUFFER1_SIZE);
    #endif
    #if HMDRV_RECORDER_BUFFER_COUNT > 2
    HMREC_SetUpRecBuff(&sRecHandle[2], HMDRV_RECORDER_BUFFER2_ADDR, HMDRV_RECORDER_BUFFER2_SIZE);
    #endif
    #endif
    #endif

    #if (HMDRV_USE_RECORDER_PARAMETERS) && (!(HMDRV_DISABLE))
    #if HMDRV_RECORDER_PARAM_HANDLE_COUNT == 1
    HMRECPRM_vInit(&sRecHandleParam[0]);
    HMRECPRM_SetUpRecBuff(&sRecHandleParam[0], HMDRV_RECORDER_PARAM_BUFFER0_ADDR, HMDRV_RECORDER_PARAM_BUFFER0_SIZE);
    #else
    index = 0;
    while(index < HMDRV_RECORDER_PARAM_HANDLE_COUNT)
    {
        HMRECPRM_vInit(&sRecHandleParam[index]);
        index++;
    }
    HMRECPRM_SetUpRecBuff(&sRecHandleParam[0], HMDRV_RECORDER_PARAM_BUFFER0_ADDR, HMDRV_RECORDER_PARAM_BUFFER0_SIZE);
    #if HMDRV_RECORDER_PARAM_BUFFER_COUNT > 1
    HMRECPRM_SetUpRecBuff(&sRecHandleParam[1], HMDRV_RECORDER_PARAM_BUFFER1_ADDR, HMDRV_RECORDER_PARAM_BUFFER1_SIZE);
    #endif
    #if HMDRV_RECORDER_PARAM_BUFFER_COUNT > 2
    HMRECPRM_SetUpRecBuff(&sRecHandleParam[2], HMDRV_RECORDER_PARAM_BUFFER2_ADDR, HMDRV_RECORDER_PARAM_BUFFER2_SIZE);
    #endif
    #endif
    #endif

    #if HMDRV_USE_UART_INIT
	HMDRV_vInit();
    #endif

    #if HMDRV_USE_MODBUS
    HMMODBUS_vInit();
    #endif

    HMSERIAL_vInit();
}


void HMLIB_vProcessRecorders(void)
{
    HMLIB_vRecorder0();
    HMLIB_vRecorder1();
    HMLIB_vRecorder2();

    HMLIB_vRecorderParam0();
    HMLIB_vRecorderParam1();
    HMLIB_vRecorderParam2();
    HMLIB_vRecorderParamTimeSync();
}


void HMLIB_vRecorder(HMDRV_U8 u8Index)
{
    #if (HMDRV_USE_RECORDER) && (!(HMDRV_DISABLE))
    #if HMDRV_RECORDER_HANDLE_COUNT > 0
    if (u8Index < HMDRV_RECORDER_HANDLE_COUNT)
    {
        HMREC_vProcess(&sRecHandle[u8Index]);
    }
    #endif
    #endif
}

void HMLIB_vRecorder0(void)
{
    #if (HMDRV_USE_RECORDER) && (!(HMDRV_DISABLE))
    #if HMDRV_RECORDER_HANDLE_COUNT > 0
    HMREC_vProcess(&sRecHandle[0]);
    #endif
    #endif
}

void HMLIB_vRecorder1(void)
{
    #if (HMDRV_USE_RECORDER) && (!(HMDRV_DISABLE))
    #if HMDRV_RECORDER_HANDLE_COUNT > 1 
    HMREC_vProcess(&sRecHandle[1]);
    #endif
    #endif
}

void HMLIB_vRecorder2(void)
{
    #if (HMDRV_USE_RECORDER) && (!(HMDRV_DISABLE))
    #if HMDRV_RECORDER_HANDLE_COUNT > 2
    HMREC_vProcess(&sRecHandle[2]);
    #endif
    #endif
}

void HMLIB_vRecorderParam(HMDRV_U8 u8Index)
{
    #if (HMDRV_USE_RECORDER_PARAMETERS) && (!(HMDRV_DISABLE))
    #if HMDRV_RECORDER_PARAM_HANDLE_COUNT > 0
    if (u8Index < HMDRV_RECORDER_PARAM_HANDLE_COUNT)
    {
        HMRECPRM_vProcess(u8Index);
    }
    #endif
    #endif
}

void HMLIB_vRecorderParam0(void)
{
    #if (HMDRV_USE_RECORDER_PARAMETERS) && (!(HMDRV_DISABLE))
    #if HMDRV_RECORDER_PARAM_HANDLE_COUNT > 0
    HMRECPRM_vProcess(0);
    #endif
    #endif
}

void HMLIB_vRecorderParam1(void)
{
    #if (HMDRV_USE_RECORDER_PARAMETERS) && (!(HMDRV_DISABLE))
    #if HMDRV_RECORDER_PARAM_HANDLE_COUNT > 1 
    HMRECPRM_vProcess(1);
    #endif
    #endif
}

void HMLIB_vRecorderParam2(void)
{
    #if (HMDRV_USE_RECORDER_PARAMETERS) && (!(HMDRV_DISABLE))
    #if HMDRV_RECORDER_PARAM_HANDLE_COUNT > 2
    HMRECPRM_vProcess(2);
    #endif
    #endif
}

void HMLIB_vRecorderParamTimeSync(void)
{
    #if (HMDRV_USE_RECORDER_PARAMETERS) && (!(HMDRV_DISABLE))
    HMRECPRM_vTimeSync();
    #endif
}





void HMLIB_vDebugSend (void)
{
    HMDRV_vDebugSend();
}



void HMLIB_vProcess(void)
{ 
    if (u8SCIHexMonIndex == u8SCIModBusIndex)
    {
        HMLIB_vProcessSamePort();
    }
    else
    {
        HMLIB_vProcessDifferentPorts();
    }
}

void HMLIB_vProcessSamePort(void)
{
    #if HMDRV_USE_UART_INIT
    HMDRV_vProcess();   /* error handling */
    #endif
    /* polled SCI mode */
    HMSERIAL_vProcessSCI(); 
    
    
}

void HMLIB_vProcessDifferentPorts(void)
{ 
    #if HMDRV_USE_UART_INIT
    HMDRV_vProcess();   /* error handling */
    #endif
    /* polled SCI mode */
    HMSERIAL_vProcessSCIFreemasterOnly(); 
    #if HMDRV_USE_MODBUS
    HMMODBUS_vProcessSCI();
    #endif

}

void HMLIB_vSetPortIndexHexMon(HMDRV_U8 u8Index)
{ 
    u8SCIHexMonIndex = u8Index;
}

void HMLIB_vSetPortIndexModBus(HMDRV_U8 u8Index)
{ 
    u8SCIModBusIndex = u8Index;
}


void HMLIB_vProcessFastTD(HMDRV_U8 u8Index)
{
#if HMDRV_TE_MIN_TIME
    if (u8SCIHexMonIndex == u8Index)
    {
        HMSERIAL_vProcessFastTD(); 
    }
    if (u8SCIModBusIndex == u8Index)
    {
        HMMODBUS_vProcessFastTD();
    }
#endif
    
}

#if HMDRV_SCI_TX_ISR
void HMLIB_vProcessSCITXISR (void)
{
    HMSERIAL_vProcessSCITXISR();
}



void HMLIB_vProcessSCITXISRWithIndex(HMDRV_U8 u8Index)
{
    if (u8SCIHexMonIndex == u8Index)
    {
        if (u8SCIModBusIndex == u8Index)
        {
            HMSERIAL_vProcessSCITXISR(); 
        }
        else
        {
             HMSERIAL_vProcessSCITXISRFreemasterOnly(); 
        }
    }
    else if (u8SCIModBusIndex == u8Index)
    {
        HMMODBUS_vProcessSCITXISR();
    }
    
    
}
#endif

void HMLIB_vScopeDataUpdate(void)
{
    #if HMDRV_VALID_SCOPE_DATA_ONLY
    HMSCOPE_vSetDataChangeFlag();
    #endif
}

void HMLIB_vScopeParamDataUpdate(void)
{
    #if HMDRV_VALID_SCOPE_DATA_ONLY
    HMSCOPEPRM_vSetDataChangeFlag();
    #endif
}

HMDRV_ADDR HMLIB_GetRecBuffAddress(HMDRV_U8 u8Index)
{
    HMDRV_ADDR result = (HMDRV_ADDR)0xFFFFFFFF;
    
    #if (HMDRV_USE_RECORDER) && (!(HMDRV_DISABLE))
    #if HMDRV_RECORDER_HANDLE_COUNT > 0
    if (u8Index < HMDRV_RECORDER_HANDLE_COUNT)
    {
        result = sRecHandle[u8Index].pBufferAddr;
    }
    #endif
    #endif
    return result;
}

HMDRV_U32 HMLIB_GetRecBufferSize(HMDRV_U8 u8Index)
{
    HMDRV_U32 result = (HMDRV_U32)0xFFFFFFFF;
    
    #if (HMDRV_USE_RECORDER) && (!(HMDRV_DISABLE))
    #if HMDRV_RECORDER_HANDLE_COUNT > 0
    if (u8Index < HMDRV_RECORDER_HANDLE_COUNT)
    {
        result = sRecHandle[u8Index].nBufferSize;
    }
    #endif
    #endif
    return result;
}

HMDRV_U16 HMLIB_GetRecSampleSize(HMDRV_U8 u8Index)
{
    HMDRV_U16 result = (HMDRV_U16)0xFFFFFFFF;
    
    #if (HMDRV_USE_RECORDER) && (!(HMDRV_DISABLE))
    #if HMDRV_RECORDER_HANDLE_COUNT > 0
    if (u8Index < HMDRV_RECORDER_HANDLE_COUNT)
    {
        result = sRecHandle[u8Index].u8RecSampleSize;
    }
    #endif
    #endif
    return result;
}


HMDRV_ADDR HMLIB_GetRecParamBuffAddress(HMDRV_U8 u8Index)
{
    HMDRV_ADDR result = (HMDRV_ADDR)0xFFFFFFFF;
    
    #if (HMDRV_USE_RECORDER_PARAMETERS) && (!(HMDRV_DISABLE))
    #if HMDRV_RECORDER_PARAM_HANDLE_COUNT > 0
    if (u8Index < HMDRV_RECORDER_PARAM_HANDLE_COUNT)
    {
        result = sRecHandleParam[u8Index].pBufferAddr;
    }
    #endif
    #endif
    return result;
}

HMDRV_U32 HMLIB_GetRecParamBufferSize(HMDRV_U8 u8Index)
{
    HMDRV_U32 result = (HMDRV_U32)0xFFFFFFFF;
    
    #if (HMDRV_USE_RECORDER_PARAMETERS) && (!(HMDRV_DISABLE))
    #if HMDRV_RECORDER_PARAM_HANDLE_COUNT > 0
    if (u8Index < HMDRV_RECORDER_PARAM_HANDLE_COUNT)
    {
        result = sRecHandleParam[u8Index].nBufferSize;
    }
    #endif
    #endif
    return result;
}

HMDRV_U16 HMLIB_GetRecParamSampleSize(HMDRV_U8 u8Index)
{
    HMDRV_U16 result = (HMDRV_U16)0xFFFFFFFF;
    
    #if (HMDRV_USE_RECORDER_PARAMETERS) && (!(HMDRV_DISABLE))
    #if HMDRV_RECORDER_PARAM_HANDLE_COUNT > 0
    if (u8Index < HMDRV_RECORDER_PARAM_HANDLE_COUNT)
    {
        result = sRecHandleParam[u8Index].u8RecSampleSize;
    }
    #endif
    #endif
    return result;
}

void HMLIB_vRecorderSetTimeBase (uint16_t u16MicroSeconds)
{
    HMPROTOCOL_vRecorderSetTimeBase(u16MicroSeconds);
}

