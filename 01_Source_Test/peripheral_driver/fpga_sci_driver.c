/* *****************************************************************************
 * File:   fpga_sci_driver.c
 * Author: Dimitar Lilov
 *
 * Created on 2020 01 17 12:36
 * 
 * Description: ...
 * 
 **************************************************************************** */
#ifdef _WIN32
#pragma once
#endif /* _WIN32 */

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdint.h>
//#include "hw_types.h"   /* uint8_t  defined here but not always */
#include "device.h"
#include "fpga_sci_driver.h"
#include "uart_driver.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define MEMORY_MODIFIER
#define SPACE_ATTRIBUTE


#define FPGA_SCI_DRV_UART_DRV_TX_BUFFER_SIZE 64
#define FPGA_SCI_DRV_UART_DRV_RX_BUFFER_SIZE 64


#define FPGA_SCI_DRV_TRIP_TEMPERATURE_DEG        (  80.0)
#define FPGA_SCI_DRV_TRIP_HUMIDITY_PERCENT       (  80.0)





/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */
/* uartcommon */
typedef enum
{
            RX_IDLE = 0,
            //RX_HDR,
            RX_CMD,
            RX_LEN,
            RX_DATA,
            RX_CHSUM
} FPGA_SCI_DRV_eRxState_t;



/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

#ifndef MCLIB_H
#define MCLIB_s16d(x)   (int16_t)(10.0F * x)   /* dA(0.1A), dV(0.1V), dHz(0.1Hz) */
#endif



#define u32uSecToTimerTicks(uSec)  (uint32_t)(uSec * FPGA_SCI_DRV_u16TimerTicksPerMicroSecond)

#define FPGA_SCI_DRV_FREE_RUN_TIMER_DECREMENT_COUNTER 1
#if FPGA_SCI_DRV_FREE_RUN_TIMER_DECREMENT_COUNTER == 0
#define FPGA_SCI_DRV_GET_FREE_RUN_TIMER()          (uint32_t)(CPUTimer_getTimerCount(CPUTIMER1_BASE))
#else
#define FPGA_SCI_DRV_GET_FREE_RUN_TIMER()          (uint32_t)(0 - CPUTimer_getTimerCount(CPUTIMER1_BASE))
#endif


#define RX_TOUT_USEC 50000        /* 50 ms */



/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
uint32_t FPGA_SCI_DRV_u32TimeLast = 0;
uint32_t FPGA_SCI_DRV_u32TimePass = 0;
uint32_t FPGA_SCI_DRV_u32TimeCurr = 0;

bool FPGA_SCI_DRV_bDataRecv = 0;

bool FPGA_SCI_DRV_bDataRecvUse = 0;

bool FPGA_SCI_DRV_bResetUart = 0;

uint_least8_t FPGA_SCI_DRV_u8UartIndex = FPGA_SCI_DRV_UART_MODULE;

uint_least8_t FPGA_SCI_DRV_nAddressDSP = ADR_DSP; //2-send to fpga; 1-receive from fpga
uint_least8_t FPGA_SCI_DRV_nAddressFPGA = ADR_FPGA; //2-send to fpga; 1-receive from fpga


uint32_t FPGA_SCI_DRV_u32SendCounter = 0;
uint32_t FPGA_SCI_DRV_u32SendCount = 1000;
bool FPGA_SCI_DRV_bPeriodicSend = 0;



uint32_t FPGA_SCI_DRV_u32RxTimeout_us = RX_TOUT_USEC;
uint32_t FPGA_SCI_DRV_u32RxTimeout = RX_TOUT_USEC;
uint16_t FPGA_SCI_DRV_u16TimerTicksPerMicroSecond = 1;




uint_least8_t FPGA_SCI_DRV_Clock_Get_Seconds  = 0x00;
uint_least8_t FPGA_SCI_DRV_Clock_Get_Minutes  = 0x00;
uint_least8_t FPGA_SCI_DRV_Clock_Get_Hours    = 0x00;
uint_least8_t FPGA_SCI_DRV_Clock_Get_Weekdays = 0x01;
uint_least8_t FPGA_SCI_DRV_Clock_Get_Days     = 0x01;
uint_least8_t FPGA_SCI_DRV_Clock_Get_Month    = 0x01;
uint_least8_t FPGA_SCI_DRV_Clock_Get_Year     = 0x01;

uint_least8_t FPGA_SCI_DRV_Clock_Get_Last_Seconds  = 0x00;
uint_least8_t FPGA_SCI_DRV_Clock_Get_Last_Minutes  = 0x00;
uint_least8_t FPGA_SCI_DRV_Clock_Get_Last_Hours    = 0x00;
uint_least8_t FPGA_SCI_DRV_Clock_Get_Last_Weekdays = 0x01;
uint_least8_t FPGA_SCI_DRV_Clock_Get_Last_Days     = 0x01;
uint_least8_t FPGA_SCI_DRV_Clock_Get_Last_Month    = 0x01;
uint_least8_t FPGA_SCI_DRV_Clock_Get_Last_Year     = 0x01;

uint_least8_t FPGA_SCI_DRV_Clock_Set_Seconds  = 0x00;
uint_least8_t FPGA_SCI_DRV_Clock_Set_Minutes  = 0x00;
uint_least8_t FPGA_SCI_DRV_Clock_Set_Hours    = 0x00;
uint_least8_t FPGA_SCI_DRV_Clock_Set_Weekdays = 0x01;
uint_least8_t FPGA_SCI_DRV_Clock_Set_Days     = 0x01;
uint_least8_t FPGA_SCI_DRV_Clock_Set_Month    = 0x01;
uint_least8_t FPGA_SCI_DRV_Clock_Set_Year     = 0x01;



uint16_t FPGA_SCI_DRV_gdi_det;
uint16_t FPGA_SCI_DRV_gdi_err;
uint16_t FPGA_SCI_DRV_gdi_ovt;
uint16_t FPGA_SCI_DRV_trp_ocn;
uint16_t FPGA_SCI_DRV_trp_ocp;
uint16_t FPGA_SCI_DRV_err_glb;

uint16_t FPGA_SCI_DRV_gdi_det_err;
uint16_t FPGA_SCI_DRV_gdi_err_err;
uint16_t FPGA_SCI_DRV_gdi_ovt_err;
uint16_t FPGA_SCI_DRV_trp_ocn_err;
uint16_t FPGA_SCI_DRV_trp_ocp_err;
uint16_t FPGA_SCI_DRV_err_glb_err;


uint16_t FPGA_SCI_DRV_meas_temp_err;


uint16_t FPGA_SCI_DRV_gdi_det_err_Trip;
uint16_t FPGA_SCI_DRV_gdi_err_err_Trip;
uint16_t FPGA_SCI_DRV_gdi_ovt_err_Trip;
uint16_t FPGA_SCI_DRV_trp_ocn_err_Trip;
uint16_t FPGA_SCI_DRV_trp_ocp_err_Trip;
uint16_t FPGA_SCI_DRV_err_glb_err_Trip;


uint16_t FPGA_SCI_DRV_meas_temp_err_Trip;


uint16_t FPGA_SCI_DRV_ms_meas_temp_err =
        (FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER1 << 0) |
        (FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER2 << 1) |
        (FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER3 << 2) |
        (FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER4 << 3) |
        (FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER5 << 4) |
        (FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER6 << 5) |
        (FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER7 << 6) |
        (FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER8 << 7) |
        (FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER9 << 8) |
        (FPGA_SCI_DRV_USE_TEMP_TRIP_BOARD   << 9) |
        (FPGA_SCI_DRV_USE_TEMP_TRIP_HUMIDITY<< 10);

#define FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER1  FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER_GLOBAL
#define FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER2  FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER_GLOBAL
#define FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER3  FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER_GLOBAL
#define FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER4  FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER_GLOBAL
#define FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER5  FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER_GLOBAL
#define FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER6  FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER_GLOBAL
#define FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER7  FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER_GLOBAL
#define FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER8  FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER_GLOBAL
#define FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER9  FPGA_SCI_DRV_USE_TEMP_TRIP_DRIVER_GLOBAL

#define FPGA_SCI_DRV_USE_TEMP_TRIP_BOARD    0
#define FPGA_SCI_DRV_USE_TEMP_TRIP_HUMIDITY 0











uint16_t FPGA_SCI_DRV_ms_det = MASK_ERRORS;
uint16_t FPGA_SCI_DRV_ms_err = MASK_ERRORS;
uint16_t FPGA_SCI_DRV_ms_ovt = MASK_ERRORS;
uint16_t FPGA_SCI_DRV_ms_ocn = MASK_ERRORS;
uint16_t FPGA_SCI_DRV_ms_ocp = MASK_ERRORS;
uint16_t FPGA_SCI_DRV_ms_glb = MASK_GLOBAL;

uint16_t FPGA_SCI_DRV_ms_det_fbk = MASK_ERRORS;
uint16_t FPGA_SCI_DRV_ms_err_fbk = MASK_ERRORS;
uint16_t FPGA_SCI_DRV_ms_ovt_fbk = MASK_ERRORS;
uint16_t FPGA_SCI_DRV_ms_ocn_fbk = MASK_ERRORS;
uint16_t FPGA_SCI_DRV_ms_ocp_fbk = MASK_ERRORS;
uint16_t FPGA_SCI_DRV_ms_glb_fbk = MASK_GLOBAL;

uint16_t FPGA_SCI_DRV_Temp1;    /* d oC */
uint16_t FPGA_SCI_DRV_Temp2;    /* d oC */
uint16_t FPGA_SCI_DRV_Temp3;    /* d oC */
uint16_t FPGA_SCI_DRV_Temp4;    /* d oC */
uint16_t FPGA_SCI_DRV_Temp5;    /* d oC */
uint16_t FPGA_SCI_DRV_Temp6;    /* d oC */
uint16_t FPGA_SCI_DRV_Temp7;    /* d oC */
uint16_t FPGA_SCI_DRV_Temp8;    /* d oC */
uint16_t FPGA_SCI_DRV_Temp9;    /* d oC */
uint16_t FPGA_SCI_DRV_HuTmp;    /* d oC */
uint16_t FPGA_SCI_DRV_Humid;    /* d oC */

uint16_t FPGA_SCI_DRV_Temp1_Trip;    /* d oC */
uint16_t FPGA_SCI_DRV_Temp2_Trip;    /* d oC */
uint16_t FPGA_SCI_DRV_Temp3_Trip;    /* d oC */
uint16_t FPGA_SCI_DRV_Temp4_Trip;    /* d oC */
uint16_t FPGA_SCI_DRV_Temp5_Trip;    /* d oC */
uint16_t FPGA_SCI_DRV_Temp6_Trip;    /* d oC */
uint16_t FPGA_SCI_DRV_Temp7_Trip;    /* d oC */
uint16_t FPGA_SCI_DRV_Temp8_Trip;    /* d oC */
uint16_t FPGA_SCI_DRV_Temp9_Trip;    /* d oC */
uint16_t FPGA_SCI_DRV_HuTmp_Trip;    /* d oC */
uint16_t FPGA_SCI_DRV_Humid_Trip;    /* d oC */




FPGA_SCI_DRV_eRxState_t  FPGA_SCI_DRV_eRxState;
uint_least8_t FPGA_SCI_DRV_nRxTodoEcho = 0;
uint32_t FPGA_SCI_DRV_nRecvTimeout;
uint_least8_t FPGA_SCI_DRV_nTxIndex;
uint_least8_t FPGA_SCI_DRV_nRxIndex;
uint_least8_t FPGA_SCI_DRV_nTxChSum;
uint_least8_t FPGA_SCI_DRV_nRxChSum;
uint16_t FPGA_SCI_DRV_nRxData;
uint_least8_t FPGA_SCI_DRV_nRxByteCounter;
uint_least8_t FPGA_SCI_DRV_nRxCommand;
uint_least8_t FPGA_SCI_DRV_nRxCommand_Mask = 0xFF;
uint_least8_t FPGA_SCI_DRV_nRxLength;
uint_least8_t FPGA_SCI_DRV_nTxTestLength = 10;
uint_least8_t FPGA_SCI_DRV_nBytesToSend;
uint_least8_t FPGA_SCI_DRV_nBytesToSendHardSet = 0;
uint_least8_t FPGA_SCI_DRV_nAddrMode = SCI_CCR_ADDRIDLE_MODE;


uint32_t FPGA_SCI_DRV_nMessagesCounter[CMD_CNT] =
{
 0,0,0,0
};

bool FPGA_SCI_DRV_bDataUpdate;
bool FPGA_SCI_DRV_bRTCGet = 0;

bool FPGA_SCI_DRV_bUpdateNonLastExecuting;
bool FPGA_SCI_DRV_bUpdateLastExecuting;

bool FPGA_SCI_DRV_bSendTxCommand = 0;
bool FPGA_SCI_DRV_bResponseRxCommand = 0;
bool FPGA_SCI_DRV_bResponseRxCommandLast = 0xFF;
bool FPGA_SCI_DRV_bSendRxResponse = 0;

bool FPGA_SCI_DRV_bUseCheckSum = 1;

bool FPGA_SCI_DRV_bRxResponseIncrementData = 0;
uint_least8_t FPGA_SCI_DRV_nTxCommand;
uint_least8_t FPGA_SCI_DRV_nTxCommandReq = 0xFF;

MEMORY_MODIFIER uint_least8_t FPGA_SCI_DRV_au8TxBuffer[FPGA_SCI_DRV_UART_DRV_TX_BUFFER_SIZE];
MEMORY_MODIFIER uint16_t FPGA_SCI_DRV_au8RxBuffer[FPGA_SCI_DRV_UART_DRV_RX_BUFFER_SIZE];


/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */

void FPGA_SCI_DRV_vSetupFreeTimerTicksPerMicroSecond(uint16_t u16Input)
{
    FPGA_SCI_DRV_u16TimerTicksPerMicroSecond = u16Input;
    FPGA_SCI_DRV_u32RxTimeout = u32uSecToTimerTicks(FPGA_SCI_DRV_u32RxTimeout_us);
}

bool FPGA_SCI_DRV_bGetRTCGet(void)
{
    return FPGA_SCI_DRV_bRTCGet;
}


void FPGA_SCI_DRV_vProcessUartRxTimeout(void)
{
    /* Calculate TimePassed From Previous Call */
    FPGA_SCI_DRV_u32TimeCurr = FPGA_SCI_DRV_GET_FREE_RUN_TIMER();
    FPGA_SCI_DRV_u32TimePass = FPGA_SCI_DRV_u32TimeCurr - FPGA_SCI_DRV_u32TimeLast;
    FPGA_SCI_DRV_u32TimeLast = FPGA_SCI_DRV_u32TimeCurr;

    if (FPGA_SCI_DRV_nRecvTimeout > FPGA_SCI_DRV_u32TimePass)
    {
        FPGA_SCI_DRV_nRecvTimeout -= FPGA_SCI_DRV_u32TimePass;
    }
    else
    {
        if (FPGA_SCI_DRV_nRecvTimeout > 0)
        {
            FPGA_SCI_DRV_eRxState = RX_IDLE;
        }
        FPGA_SCI_DRV_nRecvTimeout = 0;
    }
}

void FPGA_SCI_DRV_vProcessUartRx(void)
{
   uint16_t rx_data;

   FPGA_SCI_DRV_vProcessUartRxTimeout();

   while((UART_DRV_ReadDataRxAvailable((UART_DRV_eModule_t)FPGA_SCI_DRV_u8UartIndex)) && (FPGA_SCI_DRV_bDataRecv != 1))
   {
        FPGA_SCI_DRV_nRecvTimeout = FPGA_SCI_DRV_u32RxTimeout;
        // buffer overruns are ignored
        rx_data = UART_DRV_ReadDataRx((UART_DRV_eModule_t)FPGA_SCI_DRV_u8UartIndex);
        FPGA_SCI_DRV_nRxData = rx_data;
        FPGA_SCI_DRV_nRxChSum += rx_data;
        switch(FPGA_SCI_DRV_eRxState)
        {
            case RX_IDLE :
                if(FPGA_SCI_DRV_nAddressDSP == rx_data)
                {
                    FPGA_SCI_DRV_eRxState = RX_CMD;
                    FPGA_SCI_DRV_nRxChSum = 0x00;
                }
                break;
            case RX_CMD :
                FPGA_SCI_DRV_nRxCommand = rx_data;
                FPGA_SCI_DRV_eRxState = RX_LEN;
                break;
            case RX_LEN :
                if(rx_data)
                {
                    FPGA_SCI_DRV_nRxByteCounter = FPGA_SCI_DRV_nRxLength = rx_data;
                    FPGA_SCI_DRV_eRxState = RX_DATA;
                    FPGA_SCI_DRV_nRxIndex = 0;
                }
                else
                {
                    FPGA_SCI_DRV_eRxState = RX_CHSUM;
                }
                break;
            case RX_DATA :
                FPGA_SCI_DRV_au8RxBuffer[FPGA_SCI_DRV_nRxIndex++] = rx_data;
                if(!(--FPGA_SCI_DRV_nRxByteCounter))
                {
                    FPGA_SCI_DRV_eRxState = RX_CHSUM;
                }
                break;
            case RX_CHSUM :
                if((!(FPGA_SCI_DRV_nRxChSum & 0x00FF)) || (FPGA_SCI_DRV_bUseCheckSum == 0))
                {
                    if (FPGA_SCI_DRV_nRxCommand <= CMD_TMP)
                    {
                        FPGA_SCI_DRV_nMessagesCounter[FPGA_SCI_DRV_nRxCommand]++;
                    }

                    if( (FPGA_SCI_DRV_nRxCommand == FPGA_SCI_DRV_nRxCommand_Mask) || (FPGA_SCI_DRV_nRxCommand_Mask == 0xFF))
                    {
                        FPGA_SCI_DRV_bDataRecv = 1;
                    }
                    FPGA_SCI_DRV_eRxState = RX_IDLE;
                }
                else
                {
                    FPGA_SCI_DRV_eRxState = RX_IDLE;        /* if not here disconnection is by timeout */
                }
                break;
            default:
                FPGA_SCI_DRV_eRxState = RX_IDLE;
                break;
        }

   }
}



void FPGA_SCI_DRV_vStartUartTx(void)
{
    FPGA_SCI_DRV_nTxIndex = FPGA_SCI_DRV_nTxChSum = 0;
    if (FPGA_SCI_DRV_nBytesToSendHardSet != 0)
    {
        FPGA_SCI_DRV_nBytesToSend = FPGA_SCI_DRV_nBytesToSendHardSet;
    }
    if(FPGA_SCI_DRV_nBytesToSend)
    {
        UART_DRV_TransmitEnableDisable((UART_DRV_eModule_t)FPGA_SCI_DRV_u8UartIndex,1);

        if (FPGA_SCI_DRV_nAddrMode != 0)
        {
            UART_DRV_PutNextAddress((UART_DRV_eModule_t)FPGA_SCI_DRV_u8UartIndex);  /* Next PutChar is with AddrBit Set */
        }

        UART_DRV_PutChar((UART_DRV_eModule_t)FPGA_SCI_DRV_u8UartIndex, FPGA_SCI_DRV_au8TxBuffer[FPGA_SCI_DRV_nTxIndex++]);
        UART_DRV_TxIntEna((UART_DRV_eModule_t)FPGA_SCI_DRV_u8UartIndex);    /* transmit complete interrupt (interrupt when Char is transferred) */
    }
}

void FPGA_SCI_DRV_vProcessUartTx(uint_least8_t u8Index)
{
    if (FPGA_SCI_DRV_u8UartIndex == u8Index)
    {
        if(FPGA_SCI_DRV_nTxIndex < FPGA_SCI_DRV_nBytesToSend)
        {
            if(FPGA_SCI_DRV_nTxIndex > 0)
            {
                FPGA_SCI_DRV_nTxChSum += FPGA_SCI_DRV_au8TxBuffer[FPGA_SCI_DRV_nTxIndex];
            }
            UART_DRV_PutChar((UART_DRV_eModule_t)u8Index, FPGA_SCI_DRV_au8TxBuffer[FPGA_SCI_DRV_nTxIndex++]);
        }
        else
        {
            UART_DRV_PutChar((UART_DRV_eModule_t)u8Index, ((0-FPGA_SCI_DRV_nTxChSum) & 0x00FF));
            //UART_DRV_PutChar((UART_DRV_eModule_t)u8Index, 0xAE);
            UART_DRV_TxIntDis((UART_DRV_eModule_t)u8Index);
        }
    }
}

void FPGA_SCI_DRV_vProcessFastTD (uint_least8_t u8Index)
{
    if (FPGA_SCI_DRV_u8UartIndex == u8Index)
    {

        if (FPGA_SCI_DRV_nRxTodoEcho > 0)
        {
            FPGA_SCI_DRV_nRxTodoEcho--;
            if (FPGA_SCI_DRV_nRxTodoEcho == 0)
            {
                UART_DRV_TransmitEnableDisable((UART_DRV_eModule_t)u8Index,0);
            }
        }
    }
}


void FPGA_SCI_DRV_vUartOverflowDetection(void)
{
    FPGA_SCI_DRV_eRxState = RX_IDLE;
}









bool FPGA_SCI_DRV_bTxCommandMaskAuto = 1;
uint32_t FPGA_SCI_DRV_nTxCommandMaskDenyTimeDelay = 0;
uint32_t FPGA_SCI_DRV_nTxCommandMaskDenyTime = 0;
uint32_t FPGA_SCI_DRV_nTxCommandMaskDenyTimeLast = 0;
uint32_t FPGA_SCI_DRV_nTxCommandMaskDenyTimeDiff = 0;

void FPGA_SCI_DRV_vErrorMasksSetup(void)
{
    if(FPGA_SCI_DRV_bTxCommandMaskAuto)
    {
        FPGA_SCI_DRV_nTxCommandMaskDenyTime = FPGA_SCI_DRV_GET_FREE_RUN_TIMER();
        FPGA_SCI_DRV_nTxCommandMaskDenyTimeDiff = FPGA_SCI_DRV_nTxCommandMaskDenyTime - FPGA_SCI_DRV_nTxCommandMaskDenyTimeLast;
        FPGA_SCI_DRV_nTxCommandMaskDenyTimeLast = FPGA_SCI_DRV_nTxCommandMaskDenyTime;
        if (FPGA_SCI_DRV_nTxCommandMaskDenyTimeDelay > FPGA_SCI_DRV_nTxCommandMaskDenyTimeDiff)
        {
            FPGA_SCI_DRV_nTxCommandMaskDenyTimeDelay -= FPGA_SCI_DRV_nTxCommandMaskDenyTimeDiff;
        }
        else
        {
            FPGA_SCI_DRV_nTxCommandMaskDenyTimeDelay = 0;
        }

        if ((FPGA_SCI_DRV_nTxCommandReq == 0xFF) && (FPGA_SCI_DRV_nTxCommandMaskDenyTimeDelay == 0))
        {
            FPGA_SCI_DRV_nTxCommandReq = CMD_MSK;
            FPGA_SCI_DRV_nTxCommandMaskDenyTimeDelay = u32uSecToTimerTicks(1000000.0);
            FPGA_SCI_DRV_nTxCommandMaskDenyTime = FPGA_SCI_DRV_GET_FREE_RUN_TIMER();
            FPGA_SCI_DRV_nTxCommandMaskDenyTimeLast = FPGA_SCI_DRV_nTxCommandMaskDenyTime;
        }
    }
}


bool FPGA_SCI_DRV_vClockSetup(void)
{
    bool bResult = false;

    if (FPGA_SCI_DRV_nTxCommandReq == 0xFF)
    {
        /* priority on RTC request */
        FPGA_SCI_DRV_nTxCommandReq = CMD_RTC;
        bResult = true;
    }

    return bResult;
}


//void FPGA_SCI_DRV_vClockSetup(void)
//{
//    /* priority on RTC request */
//    FPGA_SCI_DRV_nTxCommandReq = CMD_RTC;
//}



/* ser_async */
//void ser3_async_main(void)
void FPGA_SCI_DRV_vProcess(void)
{
    uint_least8_t index;
    uint_least8_t i;

    FPGA_SCI_DRV_bDataUpdate = 0;

    if (FPGA_SCI_DRV_nTxCommandReq != 0xFF)
    {
        FPGA_SCI_DRV_nTxCommand = FPGA_SCI_DRV_nTxCommandReq;
        FPGA_SCI_DRV_bSendTxCommand = 1;
        FPGA_SCI_DRV_nTxCommandReq = 0xFF;
    }

    if (FPGA_SCI_DRV_bPeriodicSend)
    {
        FPGA_SCI_DRV_u32SendCounter++;
        if (FPGA_SCI_DRV_u32SendCounter >= FPGA_SCI_DRV_u32SendCount)
        {
            FPGA_SCI_DRV_u32SendCounter = 0;
            FPGA_SCI_DRV_bSendTxCommand = 1;
        }
    }



    /* read signal mapping */
    if(FPGA_SCI_DRV_bDataRecv)
    {
        index = 0;


        switch(FPGA_SCI_DRV_nRxCommand)
        {
            case CMD_RTC :
                FPGA_SCI_DRV_bUpdateNonLastExecuting = 1;
                FPGA_SCI_DRV_Clock_Get_Seconds     = FPGA_SCI_DRV_au8RxBuffer[index++];
                FPGA_SCI_DRV_Clock_Get_Minutes     = FPGA_SCI_DRV_au8RxBuffer[index++];
                FPGA_SCI_DRV_Clock_Get_Hours       = FPGA_SCI_DRV_au8RxBuffer[index++];
                FPGA_SCI_DRV_Clock_Get_Days        = FPGA_SCI_DRV_au8RxBuffer[index++];
                FPGA_SCI_DRV_Clock_Get_Weekdays    = FPGA_SCI_DRV_au8RxBuffer[index++];
                FPGA_SCI_DRV_Clock_Get_Month       = FPGA_SCI_DRV_au8RxBuffer[index++];
                FPGA_SCI_DRV_Clock_Get_Year        = FPGA_SCI_DRV_au8RxBuffer[index++];
                FPGA_SCI_DRV_bUpdateNonLastExecuting = 0;
                FPGA_SCI_DRV_bRTCGet = 1;
                FPGA_SCI_DRV_bDataUpdate = 1;

                FPGA_SCI_DRV_bUpdateLastExecuting = 1;
                FPGA_SCI_DRV_Clock_Get_Last_Seconds     = FPGA_SCI_DRV_au8RxBuffer[index++];
                FPGA_SCI_DRV_Clock_Get_Last_Minutes     = FPGA_SCI_DRV_au8RxBuffer[index++];
                FPGA_SCI_DRV_Clock_Get_Last_Hours       = FPGA_SCI_DRV_au8RxBuffer[index++];
                FPGA_SCI_DRV_Clock_Get_Last_Days        = FPGA_SCI_DRV_au8RxBuffer[index++];
                FPGA_SCI_DRV_Clock_Get_Last_Weekdays    = FPGA_SCI_DRV_au8RxBuffer[index++];
                FPGA_SCI_DRV_Clock_Get_Last_Month       = FPGA_SCI_DRV_au8RxBuffer[index++];
                FPGA_SCI_DRV_Clock_Get_Last_Year        = FPGA_SCI_DRV_au8RxBuffer[index++];
                FPGA_SCI_DRV_bUpdateLastExecuting = 0;
                FPGA_SCI_DRV_bDataUpdate = 1;
                break;

            case CMD_ERR :
                FPGA_SCI_DRV_gdi_det     = (uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] << 8);
                FPGA_SCI_DRV_gdi_err     = (uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] << 8);
                FPGA_SCI_DRV_gdi_ovt     = (uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] << 8);
                FPGA_SCI_DRV_trp_ocn     = (uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] << 8);
                FPGA_SCI_DRV_trp_ocp     = (uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] << 8);
                FPGA_SCI_DRV_err_glb     = (uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] << 8);
                FPGA_SCI_DRV_bDataUpdate = 1;
                break;

            case CMD_MSK :
                FPGA_SCI_DRV_ms_det_fbk     = (uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] << 8);
                FPGA_SCI_DRV_ms_err_fbk     = (uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] << 8);
                FPGA_SCI_DRV_ms_ovt_fbk     = (uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] << 8);
                FPGA_SCI_DRV_ms_ocn_fbk     = (uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] << 8);
                FPGA_SCI_DRV_ms_ocp_fbk     = (uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] << 8);
                FPGA_SCI_DRV_ms_glb_fbk     = (uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] << 8);
                FPGA_SCI_DRV_bDataUpdate = 1;
                break;

            case CMD_TMP :
                FPGA_SCI_DRV_Temp1      = (uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] << 8);
                FPGA_SCI_DRV_Temp2      = (uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] << 8);
                FPGA_SCI_DRV_Temp3      = (uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] << 8);
                FPGA_SCI_DRV_Temp4      = (uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] << 8);
                FPGA_SCI_DRV_Temp5      = (uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] << 8);
                FPGA_SCI_DRV_Temp6      = (uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] << 8);
                FPGA_SCI_DRV_Temp7      = (uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] << 8);
                FPGA_SCI_DRV_Temp8      = (uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] << 8);
                FPGA_SCI_DRV_Temp9      = (uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] << 8);
                FPGA_SCI_DRV_HuTmp      = (uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] << 8);
                FPGA_SCI_DRV_Humid      = (uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_DRV_au8RxBuffer[index++] << 8);
                FPGA_SCI_DRV_bDataUpdate = 1;
                break;
            default:

                break;
        }
    }
    /* read signal mapping END */




    if((FPGA_SCI_DRV_bDataRecv) || (FPGA_SCI_DRV_bSendTxCommand))
    {
        FPGA_SCI_DRV_bSendRxResponse = 0;
        FPGA_SCI_DRV_bResponseRxCommandLast = 0;
        if (FPGA_SCI_DRV_bSendTxCommand)
        {
            FPGA_SCI_DRV_bSendTxCommand = 0;
            FPGA_SCI_DRV_bSendRxResponse = 1;
        }
        else
        {
            if (FPGA_SCI_DRV_bDataRecvUse)
            {
                FPGA_SCI_DRV_nTxCommand = FPGA_SCI_DRV_nRxCommand;
                if (FPGA_SCI_DRV_bResponseRxCommand)
                {
                    FPGA_SCI_DRV_bResponseRxCommandLast = 1;
                    FPGA_SCI_DRV_bSendRxResponse = 1;
                }
            }
        }

        if (FPGA_SCI_DRV_bSendRxResponse)
        {
            index = 0;
            FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_nAddressFPGA;
            FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_nTxCommand;
            FPGA_SCI_DRV_au8TxBuffer[index++] = 0;

            i = index;
            switch(FPGA_SCI_DRV_nTxCommand)
            {
                case CMD_RTC :
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_Clock_Set_Seconds;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_Clock_Set_Minutes;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_Clock_Set_Hours;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_Clock_Set_Days;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_Clock_Set_Weekdays;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_Clock_Set_Month;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_Clock_Set_Year;

                    if ((FPGA_SCI_DRV_bRxResponseIncrementData) && (FPGA_SCI_DRV_bResponseRxCommandLast))
                    {
                        index = i;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                    }

                    i = index - i;
                    break;

                case CMD_ERR :
#if 0
                    FPGA_SCI_DRV_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_DRV_gdi_det;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_gdi_det >> 8;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_DRV_gdi_err;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_gdi_err >> 8;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_DRV_gdi_ovt;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_gdi_ovt >> 8;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_DRV_trp_ocn;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_trp_ocn >> 8;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_DRV_trp_ocp;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_trp_ocp >> 8;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_DRV_err_glb;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_err_glb >> 8;
#endif
                    if ((FPGA_SCI_DRV_bRxResponseIncrementData) && (FPGA_SCI_DRV_bResponseRxCommandLast))
                    {
                        index = i;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                    }

                    i = index - i;
                    break;

                case CMD_MSK :
                    FPGA_SCI_DRV_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_DRV_ms_det;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_ms_det >> 8;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_DRV_ms_err;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_ms_err >> 8;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_DRV_ms_ovt;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_ms_ovt >> 8;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_DRV_ms_ocn;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_ms_ocn >> 8;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_DRV_ms_ocp;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_ms_ocp >> 8;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_DRV_ms_glb;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_ms_glb >> 8;

                    if ((FPGA_SCI_DRV_bRxResponseIncrementData) && (FPGA_SCI_DRV_bResponseRxCommandLast))
                    {
                        index = i;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                    }

                    i = index - i;
                    break;

                case CMD_TMP :
                    FPGA_SCI_DRV_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_DRV_Temp1;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_Temp1 >> 8;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_DRV_Temp2;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_Temp2 >> 8;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_DRV_Temp3;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_Temp3 >> 8;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_DRV_Temp4;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_Temp4 >> 8;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_DRV_Temp5;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_Temp5 >> 8;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_DRV_Temp6;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_Temp6 >> 8;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_DRV_Temp7;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_Temp7 >> 8;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_DRV_Temp8;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_Temp8 >> 8;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_DRV_Temp9;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_Temp9 >> 8;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_DRV_HuTmp;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_HuTmp >> 8;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_DRV_Humid;
                    FPGA_SCI_DRV_au8TxBuffer[index++] = FPGA_SCI_DRV_Humid >> 8;

                    if ((FPGA_SCI_DRV_bRxResponseIncrementData) && (FPGA_SCI_DRV_bResponseRxCommandLast))
                    {
                        index = i;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                        FPGA_SCI_DRV_au8TxBuffer[index++] += 1;
                    }
                    i = index - i;
                    break;

                default:
                    FPGA_SCI_DRV_au8TxBuffer[index-2] |= 0x80;   /* CMD Error Code (not implemented) */

            }
            FPGA_SCI_DRV_au8TxBuffer[2] = i;                   /* LEN */

            FPGA_SCI_DRV_nBytesToSend = index;


            FPGA_SCI_DRV_nRxTodoEcho = FPGA_SCI_DRV_nBytesToSend+1;
            FPGA_SCI_DRV_vStartUartTx();
        }
        FPGA_SCI_DRV_bDataRecv = 0;
    }

    /* write signal mapping //FPGA_SCI_DRV_nRxCommand */
    if(FPGA_SCI_DRV_bDataUpdate)
    {
        FPGA_SCI_DRV_gdi_det_err = ((~FPGA_SCI_DRV_gdi_det) & FPGA_SCI_DRV_ms_det);
        FPGA_SCI_DRV_gdi_err_err = ((~FPGA_SCI_DRV_gdi_err) & FPGA_SCI_DRV_ms_err);
        FPGA_SCI_DRV_gdi_ovt_err = ((~FPGA_SCI_DRV_gdi_ovt) & FPGA_SCI_DRV_ms_ovt);
        FPGA_SCI_DRV_trp_ocn_err = ((~FPGA_SCI_DRV_trp_ocn) & FPGA_SCI_DRV_ms_ocn);
        FPGA_SCI_DRV_trp_ocp_err = ((~FPGA_SCI_DRV_trp_ocp) & FPGA_SCI_DRV_ms_ocp);
        FPGA_SCI_DRV_err_glb_err = ((~FPGA_SCI_DRV_err_glb) & FPGA_SCI_DRV_ms_glb);



        FPGA_SCI_DRV_uTmpError meas_temp_err = {0};

        meas_temp_err.sTmpError.DrvError1 = (FPGA_SCI_DRV_Temp1 > MCLIB_s16d(FPGA_SCI_DRV_TRIP_TEMPERATURE_DEG)) ? true : false;
        meas_temp_err.sTmpError.DrvError2 = (FPGA_SCI_DRV_Temp2 > MCLIB_s16d(FPGA_SCI_DRV_TRIP_TEMPERATURE_DEG)) ? true : false;
        meas_temp_err.sTmpError.DrvError3 = (FPGA_SCI_DRV_Temp3 > MCLIB_s16d(FPGA_SCI_DRV_TRIP_TEMPERATURE_DEG)) ? true : false;
        meas_temp_err.sTmpError.DrvError4 = (FPGA_SCI_DRV_Temp4 > MCLIB_s16d(FPGA_SCI_DRV_TRIP_TEMPERATURE_DEG)) ? true : false;
        meas_temp_err.sTmpError.DrvError5 = (FPGA_SCI_DRV_Temp5 > MCLIB_s16d(FPGA_SCI_DRV_TRIP_TEMPERATURE_DEG)) ? true : false;
        meas_temp_err.sTmpError.DrvError6 = (FPGA_SCI_DRV_Temp6 > MCLIB_s16d(FPGA_SCI_DRV_TRIP_TEMPERATURE_DEG)) ? true : false;
        meas_temp_err.sTmpError.DrvError7 = (FPGA_SCI_DRV_Temp7 > MCLIB_s16d(FPGA_SCI_DRV_TRIP_TEMPERATURE_DEG)) ? true : false;
        meas_temp_err.sTmpError.DrvError8 = (FPGA_SCI_DRV_Temp8 > MCLIB_s16d(FPGA_SCI_DRV_TRIP_TEMPERATURE_DEG)) ? true : false;
        meas_temp_err.sTmpError.DrvError9 = (FPGA_SCI_DRV_Temp9 > MCLIB_s16d(FPGA_SCI_DRV_TRIP_TEMPERATURE_DEG)) ? true : false;
        meas_temp_err.sTmpError.OvtBoard  = (FPGA_SCI_DRV_HuTmp > MCLIB_s16d(FPGA_SCI_DRV_TRIP_TEMPERATURE_DEG)) ? true : false; /* board Temp */
        meas_temp_err.sTmpError.Humidity  = (FPGA_SCI_DRV_Humid > MCLIB_s16d(FPGA_SCI_DRV_TRIP_HUMIDITY_PERCENT)) ? true : false; /* board Temp */


        FPGA_SCI_DRV_meas_temp_err = meas_temp_err.u16Register & FPGA_SCI_DRV_ms_meas_temp_err;

        if ((FPGA_SCI_DRV_meas_temp_err > 0) && (FPGA_SCI_DRV_meas_temp_err_Trip == 0))
        {
            FPGA_SCI_DRV_meas_temp_err_Trip = FPGA_SCI_DRV_meas_temp_err;

            FPGA_SCI_DRV_Temp1_Trip = FPGA_SCI_DRV_Temp1;
            FPGA_SCI_DRV_Temp2_Trip = FPGA_SCI_DRV_Temp2;
            FPGA_SCI_DRV_Temp3_Trip = FPGA_SCI_DRV_Temp3;
            FPGA_SCI_DRV_Temp4_Trip = FPGA_SCI_DRV_Temp4;
            FPGA_SCI_DRV_Temp5_Trip = FPGA_SCI_DRV_Temp5;
            FPGA_SCI_DRV_Temp6_Trip = FPGA_SCI_DRV_Temp6;
            FPGA_SCI_DRV_Temp7_Trip = FPGA_SCI_DRV_Temp7;
            FPGA_SCI_DRV_Temp8_Trip = FPGA_SCI_DRV_Temp8;
            FPGA_SCI_DRV_Temp9_Trip = FPGA_SCI_DRV_Temp9;
            FPGA_SCI_DRV_HuTmp_Trip = FPGA_SCI_DRV_HuTmp;
            FPGA_SCI_DRV_Humid_Trip = FPGA_SCI_DRV_Humid;

        }



    }
    /* write signal mapping END */

}

void FPGA_SCI_DRV_vClearTrippedErrors (void)
{
    FPGA_SCI_DRV_gdi_det_err = 0;
    FPGA_SCI_DRV_gdi_err_err = 0;
    FPGA_SCI_DRV_gdi_ovt_err = 0;
    FPGA_SCI_DRV_trp_ocn_err = 0;
    FPGA_SCI_DRV_trp_ocp_err = 0;
    FPGA_SCI_DRV_err_glb_err = 0;


    FPGA_SCI_DRV_meas_temp_err = 0;

    FPGA_SCI_DRV_gdi_det_err_Trip = 0;
    FPGA_SCI_DRV_gdi_err_err_Trip = 0;
    FPGA_SCI_DRV_gdi_ovt_err_Trip = 0;
    FPGA_SCI_DRV_trp_ocn_err_Trip = 0;
    FPGA_SCI_DRV_trp_ocp_err_Trip = 0;
    FPGA_SCI_DRV_err_glb_err_Trip = 0;

    FPGA_SCI_DRV_meas_temp_err_Trip = 0;
}

bool FPGA_SCI_DRV_bIsError (void)
{
    bool bResult = false;

    if (FPGA_SCI_DRV_meas_temp_err_Trip > 0)
    {
        bResult = true;
    }

    if (
           (FPGA_SCI_DRV_gdi_det_err > 0)
        || (FPGA_SCI_DRV_gdi_err_err > 0)
        || (FPGA_SCI_DRV_gdi_ovt_err > 0)
        || (FPGA_SCI_DRV_trp_ocn_err > 0)
        || (FPGA_SCI_DRV_trp_ocp_err > 0)
        || (FPGA_SCI_DRV_err_glb_err > 0)
        )
    {
        FPGA_SCI_DRV_gdi_det_err_Trip = FPGA_SCI_DRV_gdi_det_err;
        FPGA_SCI_DRV_gdi_err_err_Trip = FPGA_SCI_DRV_gdi_err_err;
        FPGA_SCI_DRV_gdi_ovt_err_Trip = FPGA_SCI_DRV_gdi_ovt_err;
        FPGA_SCI_DRV_trp_ocn_err_Trip = FPGA_SCI_DRV_trp_ocn_err;
        FPGA_SCI_DRV_trp_ocp_err_Trip = FPGA_SCI_DRV_trp_ocp_err;
        FPGA_SCI_DRV_err_glb_err_Trip = FPGA_SCI_DRV_err_glb_err;



        bResult = true;
    }
    return bResult;
}


bool FPGA_SCI_DRV_bIsErrorTemp (void)
{
    bool bResult = false;

    if (FPGA_SCI_DRV_meas_temp_err > 0)
    {
        bResult = true;
    }
    return bResult;
}

bool FPGA_SCI_DRV_bIsErrorDet (void)
{
    bool bResult = false;

    if (FPGA_SCI_DRV_gdi_det_err > 0)
    {
        bResult = true;
    }
    return bResult;
}

bool FPGA_SCI_DRV_bIsErrorDrv (void)
{
    bool bResult = false;

    if (FPGA_SCI_DRV_gdi_err_err > 0)
    {
        bResult = true;
    }
    return bResult;
}

bool FPGA_SCI_DRV_bIsErrorOvt (void)
{
    bool bResult = false;

    if (FPGA_SCI_DRV_meas_temp_err > 0)
    {
        bResult = true;
    }

    if (FPGA_SCI_DRV_gdi_ovt_err > 0)
    {
        bResult = true;
    }
    return bResult;
}

bool FPGA_SCI_DRV_bIsErrorOcn (void)
{
    bool bResult = false;

    if (FPGA_SCI_DRV_trp_ocn_err > 0)
    {
        bResult = true;
    }
    return bResult;
}

bool FPGA_SCI_DRV_bIsErrorOcp (void)
{
    bool bResult = false;

    if (FPGA_SCI_DRV_trp_ocp_err > 0)
    {
        bResult = true;
    }
    return bResult;
}

bool FPGA_SCI_DRV_bIsErrorVdc (void)
{
    bool bResult = false;
    FPGA_SCI_DRV_uDrvError err_glb_err;
    err_glb_err.u16Register = FPGA_SCI_DRV_err_glb_err;

    if (err_glb_err.sDrvError.OVError1 > 0)
    {
        bResult = true;
    }
    return bResult;
}

bool FPGA_SCI_DRV_bIsErrorGlb (void)
{
    bool bResult = false;
    FPGA_SCI_DRV_uDrvError err_glb_err;
    err_glb_err.u16Register = FPGA_SCI_DRV_err_glb_err;

    if (err_glb_err.u16Register > 0)
    {
        bResult = true;
    }
    return bResult;
}

void FPGA_SCI_DRV_vSetStackUsed(uint16_t nStack)
{
    uint16_t u16StartBitPos;
    uint16_t u16Mask;

    u16StartBitPos = nStack * 3;
    u16Mask = 0x7 << u16StartBitPos;


    FPGA_SCI_DRV_ms_det |= u16Mask;
    FPGA_SCI_DRV_ms_err |= u16Mask;
    FPGA_SCI_DRV_ms_ovt |= u16Mask;
    FPGA_SCI_DRV_ms_ocn |= u16Mask;
    FPGA_SCI_DRV_ms_ocp |= u16Mask;
    FPGA_SCI_DRV_ms_glb |= u16Mask;
    FPGA_SCI_DRV_ms_meas_temp_err |= u16Mask;
}

void FPGA_SCI_DRV_vSetStackDeny(uint16_t nStack)
{
    uint16_t u16StartBitPos;
    uint16_t u16Mask;

    u16StartBitPos = nStack * 3;
    u16Mask = 0x7 << u16StartBitPos;


    FPGA_SCI_DRV_ms_det &= ~u16Mask;
    FPGA_SCI_DRV_ms_err &= ~u16Mask;
    FPGA_SCI_DRV_ms_ovt &= ~u16Mask;
    FPGA_SCI_DRV_ms_ocn &= ~u16Mask;
    FPGA_SCI_DRV_ms_ocp &= ~u16Mask;
    FPGA_SCI_DRV_ms_glb &= ~u16Mask;
    FPGA_SCI_DRV_ms_meas_temp_err &= ~u16Mask;
}









FPGA_SCI_DRV_sTimeSyncModeYYMMDD_t FPGA_SCI_DRV_GetDate (void)
{
    FPGA_SCI_DRV_sTimeSyncModeYYMMDD_t sDate;

    if (FPGA_SCI_DRV_bUpdateNonLastExecuting == 0)
    {
        sDate.u8Year    = FPGA_SCI_DRV_Clock_Get_Year;
        sDate.u8Day     = FPGA_SCI_DRV_Clock_Get_Days;
        sDate.u8Month   = FPGA_SCI_DRV_Clock_Get_Month;
    }
    else if (FPGA_SCI_DRV_bUpdateLastExecuting == 0)
    {
        sDate.u8Year    = FPGA_SCI_DRV_Clock_Get_Last_Year;
        sDate.u8Day     = FPGA_SCI_DRV_Clock_Get_Last_Days;
        sDate.u8Month   = FPGA_SCI_DRV_Clock_Get_Last_Month;
    }
    return sDate;
}

FPGA_SCI_DRV_sTimeSyncWWHHMMSS_t FPGA_SCI_DRV_GetTime (void)
{
    FPGA_SCI_DRV_sTimeSyncWWHHMMSS_t sTime;

    if (FPGA_SCI_DRV_bUpdateNonLastExecuting == 0)
    {
        sTime.u8Hour    = FPGA_SCI_DRV_Clock_Get_Hours;
        sTime.u8Minute  = FPGA_SCI_DRV_Clock_Get_Minutes;
        sTime.u8Second  = FPGA_SCI_DRV_Clock_Get_Seconds;
        sTime.u8WeekDays= FPGA_SCI_DRV_Clock_Get_Weekdays;
    }
    else if (FPGA_SCI_DRV_bUpdateLastExecuting == 0)
    {
        sTime.u8Hour    = FPGA_SCI_DRV_Clock_Get_Last_Hours;
        sTime.u8Minute  = FPGA_SCI_DRV_Clock_Get_Last_Minutes;
        sTime.u8Second  = FPGA_SCI_DRV_Clock_Get_Last_Seconds;
        sTime.u8WeekDays= FPGA_SCI_DRV_Clock_Get_Last_Weekdays;
    }
    return sTime;
}



void FPGA_SCI_DRV_vSetDateTime (
        uint_least8_t u8BCDSeconds,
        uint_least8_t u8BCDMinutes,
        uint_least8_t u8BCDHours,
        uint_least8_t u8BCDWeekdays,
        uint_least8_t u8BCDDay,
        uint_least8_t u8BCDMonth,
        uint_least8_t u8BCDYear)
{
    FPGA_SCI_DRV_Clock_Set_Seconds  = u8BCDSeconds;
    FPGA_SCI_DRV_Clock_Set_Minutes  = u8BCDMinutes;
    FPGA_SCI_DRV_Clock_Set_Hours    = u8BCDHours;
    FPGA_SCI_DRV_Clock_Set_Weekdays = u8BCDWeekdays;
    FPGA_SCI_DRV_Clock_Set_Days     = u8BCDDay;
    FPGA_SCI_DRV_Clock_Set_Month    = u8BCDMonth;
    FPGA_SCI_DRV_Clock_Set_Year     = u8BCDYear;

}

void FPGA_SCI_DRV_vGetDateTime (
        uint_least8_t* pu8BCDSeconds,
        uint_least8_t* pu8BCDMinutes,
        uint_least8_t* pu8BCDHours,
        uint_least8_t* pu8BCDWeekdays,
        uint_least8_t* pu8BCDDay,
        uint_least8_t* pu8BCDMonth,
        uint_least8_t* pu8BCDYear)
{
    if (FPGA_SCI_DRV_bUpdateNonLastExecuting == 0)
    {
        *pu8BCDSeconds  = FPGA_SCI_DRV_Clock_Get_Seconds;
        *pu8BCDMinutes  = FPGA_SCI_DRV_Clock_Get_Minutes;
        *pu8BCDHours    = FPGA_SCI_DRV_Clock_Get_Hours;
        *pu8BCDWeekdays = FPGA_SCI_DRV_Clock_Get_Weekdays;
        *pu8BCDDay      = FPGA_SCI_DRV_Clock_Get_Days;
        *pu8BCDMonth    = FPGA_SCI_DRV_Clock_Get_Month;
        *pu8BCDYear     = FPGA_SCI_DRV_Clock_Get_Year;
    }
    else if (FPGA_SCI_DRV_bUpdateLastExecuting == 0)
    {
        *pu8BCDSeconds  = FPGA_SCI_DRV_Clock_Get_Last_Seconds;
        *pu8BCDMinutes  = FPGA_SCI_DRV_Clock_Get_Last_Minutes;
        *pu8BCDHours    = FPGA_SCI_DRV_Clock_Get_Last_Hours;
        *pu8BCDWeekdays = FPGA_SCI_DRV_Clock_Get_Last_Weekdays;
        *pu8BCDDay      = FPGA_SCI_DRV_Clock_Get_Last_Days;
        *pu8BCDMonth    = FPGA_SCI_DRV_Clock_Get_Last_Month;
        *pu8BCDYear     = FPGA_SCI_DRV_Clock_Get_Last_Year;
    }
}



