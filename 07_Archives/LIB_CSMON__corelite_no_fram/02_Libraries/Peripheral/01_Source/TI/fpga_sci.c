/* *****************************************************************************
 * File:   fpga_sci.c
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
//#include "hw_types.h"   /* uint_least8_t  defined here but not always */

#include "driverlib.h"
#include "device.h"

#include "mclib.h"

#include "fpga_sci.h"
#include "fpga_drv.h"

#include "uart_drv.h"
#include "timer.h"


/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#if BOARDCFG_USE_TSC_DSPIC
#define MEMORY_MODIFIER __eds__
#define SPACE_ATTRIBUTE __attribute__((space(eds)))
#else
#define MEMORY_MODIFIER
#define SPACE_ATTRIBUTE
#endif

#define FPGA_SCI_UART_TX_BUFFER_SIZE 64
#define FPGA_SCI_UART_RX_BUFFER_SIZE 64



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
} FPGA_SCI_eRxState_t;



/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
uint16_t FPGA_SCI_u16TimeLast = 0;
uint16_t FPGA_SCI_u16TimePass = 0;
uint16_t FPGA_SCI_u16TimeCurr = 0;

bool FPGA_SCI_bDataRecv = 0;

bool FPGA_SCI_bDataRecvUse = 0;

bool FPGA_SCI_bResetUart = 0;

uint_least8_t FPGA_SCI_u8UartIndex = FPGA_UART_MODULE;

uint_least8_t FPGA_SCI_nAddressDSP = ADR_DSP; //2-send to fpga; 1-receive from fpga
uint_least8_t FPGA_SCI_nAddressFPGA = ADR_FPGA; //2-send to fpga; 1-receive from fpga


uint32_t FPGA_SCI_u32SendCounter = 0;
uint32_t FPGA_SCI_u32SendCount = 1000;
bool FPGA_SCI_bPeriodicSend = 0;



uint_least8_t FPGA_SCI_Clock_Get_Seconds  = 0x00;
uint_least8_t FPGA_SCI_Clock_Get_Minutes  = 0x00;
uint_least8_t FPGA_SCI_Clock_Get_Hours    = 0x00;
uint_least8_t FPGA_SCI_Clock_Get_Weekdays = 0x01;
uint_least8_t FPGA_SCI_Clock_Get_Days     = 0x01;
uint_least8_t FPGA_SCI_Clock_Get_Month    = 0x01;
uint_least8_t FPGA_SCI_Clock_Get_Year     = 0x01;

uint_least8_t FPGA_SCI_Clock_Get_Last_Seconds  = 0x00;
uint_least8_t FPGA_SCI_Clock_Get_Last_Minutes  = 0x00;
uint_least8_t FPGA_SCI_Clock_Get_Last_Hours    = 0x00;
uint_least8_t FPGA_SCI_Clock_Get_Last_Weekdays = 0x01;
uint_least8_t FPGA_SCI_Clock_Get_Last_Days     = 0x01;
uint_least8_t FPGA_SCI_Clock_Get_Last_Month    = 0x01;
uint_least8_t FPGA_SCI_Clock_Get_Last_Year     = 0x01;

uint_least8_t FPGA_SCI_Clock_Set_Seconds  = 0x00;
uint_least8_t FPGA_SCI_Clock_Set_Minutes  = 0x00;
uint_least8_t FPGA_SCI_Clock_Set_Hours    = 0x00;
uint_least8_t FPGA_SCI_Clock_Set_Weekdays = 0x01;
uint_least8_t FPGA_SCI_Clock_Set_Days     = 0x01;
uint_least8_t FPGA_SCI_Clock_Set_Month    = 0x01;
uint_least8_t FPGA_SCI_Clock_Set_Year     = 0x01;



uint16_t FPGA_SCI_gdi_det;
uint16_t FPGA_SCI_gdi_err;
uint16_t FPGA_SCI_gdi_ovt;
uint16_t FPGA_SCI_trp_ocn;
uint16_t FPGA_SCI_trp_ocp;
uint16_t FPGA_SCI_err_glb;

uint16_t FPGA_SCI_gdi_det_err;
uint16_t FPGA_SCI_gdi_err_err;
uint16_t FPGA_SCI_gdi_ovt_err;
uint16_t FPGA_SCI_trp_ocn_err;
uint16_t FPGA_SCI_trp_ocp_err;
uint16_t FPGA_SCI_err_glb_err;


uint16_t FPGA_SCI_meas_temp_err;


uint16_t FPGA_SCI_gdi_det_err_Trip;
uint16_t FPGA_SCI_gdi_err_err_Trip;
uint16_t FPGA_SCI_gdi_ovt_err_Trip;
uint16_t FPGA_SCI_trp_ocn_err_Trip;
uint16_t FPGA_SCI_trp_ocp_err_Trip;
uint16_t FPGA_SCI_err_glb_err_Trip;


uint16_t FPGA_SCI_meas_temp_err_Trip;


uint16_t FPGA_SCI_ms_meas_temp_err =
        (FPGA_SCI_USE_TEMP_TRIP_DRIVER1 << 0) |
        (FPGA_SCI_USE_TEMP_TRIP_DRIVER2 << 1) |
        (FPGA_SCI_USE_TEMP_TRIP_DRIVER3 << 2) |
        (FPGA_SCI_USE_TEMP_TRIP_DRIVER4 << 3) |
        (FPGA_SCI_USE_TEMP_TRIP_DRIVER5 << 4) |
        (FPGA_SCI_USE_TEMP_TRIP_DRIVER6 << 5) |
        (FPGA_SCI_USE_TEMP_TRIP_DRIVER7 << 6) |
        (FPGA_SCI_USE_TEMP_TRIP_DRIVER8 << 7) |
        (FPGA_SCI_USE_TEMP_TRIP_DRIVER9 << 8) |
        (FPGA_SCI_USE_TEMP_TRIP_BOARD   << 9) |
        (FPGA_SCI_USE_TEMP_TRIP_HUMIDITY<< 10);

#define FPGA_SCI_USE_TEMP_TRIP_DRIVER1  FPGA_SCI_USE_TEMP_TRIP_DRIVER_GLOBAL
#define FPGA_SCI_USE_TEMP_TRIP_DRIVER2  FPGA_SCI_USE_TEMP_TRIP_DRIVER_GLOBAL
#define FPGA_SCI_USE_TEMP_TRIP_DRIVER3  FPGA_SCI_USE_TEMP_TRIP_DRIVER_GLOBAL
#define FPGA_SCI_USE_TEMP_TRIP_DRIVER4  FPGA_SCI_USE_TEMP_TRIP_DRIVER_GLOBAL
#define FPGA_SCI_USE_TEMP_TRIP_DRIVER5  FPGA_SCI_USE_TEMP_TRIP_DRIVER_GLOBAL
#define FPGA_SCI_USE_TEMP_TRIP_DRIVER6  FPGA_SCI_USE_TEMP_TRIP_DRIVER_GLOBAL
#define FPGA_SCI_USE_TEMP_TRIP_DRIVER7  FPGA_SCI_USE_TEMP_TRIP_DRIVER_GLOBAL
#define FPGA_SCI_USE_TEMP_TRIP_DRIVER8  FPGA_SCI_USE_TEMP_TRIP_DRIVER_GLOBAL
#define FPGA_SCI_USE_TEMP_TRIP_DRIVER9  FPGA_SCI_USE_TEMP_TRIP_DRIVER_GLOBAL

#define FPGA_SCI_USE_TEMP_TRIP_BOARD    0
#define FPGA_SCI_USE_TEMP_TRIP_HUMIDITY 0











uint16_t FPGA_SCI_ms_det = MASK_ERRORS;
uint16_t FPGA_SCI_ms_err = MASK_ERRORS;
uint16_t FPGA_SCI_ms_ovt = MASK_ERRORS;
uint16_t FPGA_SCI_ms_ocn = MASK_ERRORS;
uint16_t FPGA_SCI_ms_ocp = MASK_ERRORS;
uint16_t FPGA_SCI_ms_glb = MASK_GLOBAL;

uint16_t FPGA_SCI_ms_det_fbk = MASK_ERRORS;
uint16_t FPGA_SCI_ms_err_fbk = MASK_ERRORS;
uint16_t FPGA_SCI_ms_ovt_fbk = MASK_ERRORS;
uint16_t FPGA_SCI_ms_ocn_fbk = MASK_ERRORS;
uint16_t FPGA_SCI_ms_ocp_fbk = MASK_ERRORS;
uint16_t FPGA_SCI_ms_glb_fbk = MASK_GLOBAL;

uint16_t FPGA_SCI_Temp1;    /* d oC */
uint16_t FPGA_SCI_Temp2;    /* d oC */
uint16_t FPGA_SCI_Temp3;    /* d oC */
uint16_t FPGA_SCI_Temp4;    /* d oC */
uint16_t FPGA_SCI_Temp5;    /* d oC */
uint16_t FPGA_SCI_Temp6;    /* d oC */
uint16_t FPGA_SCI_Temp7;    /* d oC */
uint16_t FPGA_SCI_Temp8;    /* d oC */
uint16_t FPGA_SCI_Temp9;    /* d oC */
uint16_t FPGA_SCI_HuTmp;    /* d oC */
uint16_t FPGA_SCI_Humid;    /* d oC */

uint16_t FPGA_SCI_Temp1_Trip;    /* d oC */
uint16_t FPGA_SCI_Temp2_Trip;    /* d oC */
uint16_t FPGA_SCI_Temp3_Trip;    /* d oC */
uint16_t FPGA_SCI_Temp4_Trip;    /* d oC */
uint16_t FPGA_SCI_Temp5_Trip;    /* d oC */
uint16_t FPGA_SCI_Temp6_Trip;    /* d oC */
uint16_t FPGA_SCI_Temp7_Trip;    /* d oC */
uint16_t FPGA_SCI_Temp8_Trip;    /* d oC */
uint16_t FPGA_SCI_Temp9_Trip;    /* d oC */
uint16_t FPGA_SCI_HuTmp_Trip;    /* d oC */
uint16_t FPGA_SCI_Humid_Trip;    /* d oC */




FPGA_SCI_eRxState_t  FPGA_SCI_eRxState;
uint_least8_t FPGA_SCI_nRxTodoEcho = 0;
uint16_t FPGA_SCI_nRecvTimeout;
uint_least8_t FPGA_SCI_nTxIndex;
uint_least8_t FPGA_SCI_nRxIndex;
uint_least8_t FPGA_SCI_nTxChSum;
uint_least8_t FPGA_SCI_nRxChSum;
uint16_t FPGA_SCI_nRxData;
uint_least8_t FPGA_SCI_nRxByteCounter;
uint_least8_t FPGA_SCI_nRxCommand;
uint_least8_t FPGA_SCI_nRxCommand_Mask = 0xFF;
uint_least8_t FPGA_SCI_nRxLength;
uint_least8_t FPGA_SCI_nTxTestLength = 10;
uint_least8_t FPGA_SCI_nBytesToSend;
uint_least8_t FPGA_SCI_nBytesToSendHardSet = 0;
uint_least8_t FPGA_SCI_nAddrMode = FPGA_UART_ADDRESS_MODE;
uint_least8_t FPGA_SCI_nParityMode = FPGA_UART_PARITY;


uint32_t FPGA_SCI_nMessagesCounter[CMD_CNT] =
{
 0,0,0,0
};

bool bDataUpdate;
bool bRTCGet = 0;

bool bUpdateNonLastExecuting;
bool bUpdateLastExecuting;

bool bSendTxCommand = 0;
bool bResponseRxCommand = 0;
bool bResponseRxCommandLast = 0xFF;
bool bSendRxResponse = 0;

bool FPGA_SCI_bUseCheckSum = 1;

bool bRxResponseIncrementData = 0;
uint_least8_t FPGA_SCI_nTxCommand;
uint_least8_t FPGA_SCI_nTxCommandReq = 0xFF;

MEMORY_MODIFIER uint_least8_t FPGA_SCI_au8TxBuffer[FPGA_SCI_UART_TX_BUFFER_SIZE];
MEMORY_MODIFIER uint16_t FPGA_SCI_au8RxBuffer[FPGA_SCI_UART_RX_BUFFER_SIZE];


/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */


bool FPGA_SCI_bGetRTCGet(void)
{
    return bRTCGet;
}

void FPGA_SCI_vProcessUartRxTimeout(void)
{
    /* Calculate TimePassed From Previous Call */
    FPGA_SCI_u16TimeCurr = TIMER_vGetTimerMainRollingCounter();
    FPGA_SCI_u16TimePass = FPGA_SCI_u16TimeCurr - FPGA_SCI_u16TimeLast;
    FPGA_SCI_u16TimeLast = FPGA_SCI_u16TimeCurr;

    if (FPGA_SCI_nRecvTimeout > FPGA_SCI_u16TimePass)
    {
        FPGA_SCI_nRecvTimeout -= FPGA_SCI_u16TimePass;
    }
    else
    {
        if (FPGA_SCI_nRecvTimeout > 0)
        {
            FPGA_SCI_eRxState = RX_IDLE;
        }
        FPGA_SCI_nRecvTimeout = 0;
    }
}

void FPGA_SCI_vProcessUartRx(void)
{
   uint16_t rx_data;

   FPGA_SCI_vProcessUartRxTimeout();

   while((UART_ReadDataRxAvailable((UART_eModule_t)FPGA_SCI_u8UartIndex)) && (FPGA_SCI_bDataRecv != 1))
   {
        FPGA_SCI_nRecvTimeout = RX_TOUT;
        // buffer overruns are ignored
        rx_data = UART_ReadDataRx((UART_eModule_t)FPGA_SCI_u8UartIndex);
        FPGA_SCI_nRxData = rx_data;
        FPGA_SCI_nRxChSum += rx_data;
        switch(FPGA_SCI_eRxState)
        {
            case RX_IDLE :
                if(FPGA_SCI_nAddressDSP == rx_data)
                {
                    FPGA_SCI_eRxState = RX_CMD;
                    FPGA_SCI_nRxChSum = 0x00;
                }
                break;
            case RX_CMD :
                FPGA_SCI_nRxCommand = rx_data;
                FPGA_SCI_eRxState = RX_LEN;
                break;
            case RX_LEN :
                if(rx_data)
                {
                    FPGA_SCI_nRxByteCounter = FPGA_SCI_nRxLength = rx_data;
                    FPGA_SCI_eRxState = RX_DATA;
                    FPGA_SCI_nRxIndex = 0;
                }
                else
                {
                    FPGA_SCI_eRxState = RX_CHSUM;
                }
                break;
            case RX_DATA :
                FPGA_SCI_au8RxBuffer[FPGA_SCI_nRxIndex++] = rx_data;
                if(!(--FPGA_SCI_nRxByteCounter))
                {
                    FPGA_SCI_eRxState = RX_CHSUM;
                }
                break;
            case RX_CHSUM :
                if((!(FPGA_SCI_nRxChSum & 0x00FF)) || (FPGA_SCI_bUseCheckSum == 0))
                {
                    if (FPGA_SCI_nRxCommand <= CMD_TMP)
                    {
                        FPGA_SCI_nMessagesCounter[FPGA_SCI_nRxCommand]++;
                    }

                    if( (FPGA_SCI_nRxCommand == FPGA_SCI_nRxCommand_Mask) || (FPGA_SCI_nRxCommand_Mask == 0xFF))
                    {
                        FPGA_SCI_bDataRecv = 1;
                    }
                    FPGA_SCI_eRxState = RX_IDLE;
                }
                else
                {
                    FPGA_SCI_eRxState = RX_IDLE;        /* if not here disconnection is by timeout */
                }
                break;
            default:
                FPGA_SCI_eRxState = RX_IDLE;
                break;
        }

   }
}



void FPGA_SCI_vStartUartTx(void)
{
    FPGA_SCI_nTxIndex = FPGA_SCI_nTxChSum = 0;
    if (FPGA_SCI_nBytesToSendHardSet != 0)
    {
        FPGA_SCI_nBytesToSend = FPGA_SCI_nBytesToSendHardSet;
    }
    if(FPGA_SCI_nBytesToSend)
    {
        UART_TransmitEnable((UART_eModule_t)FPGA_SCI_u8UartIndex);

        if (FPGA_SCI_nAddrMode != 0)
        {
            UART_PutNextAddress((UART_eModule_t)FPGA_SCI_u8UartIndex);  /* Next PutChar is with AddrBit Set */
        }

        UART_PutChar((UART_eModule_t)FPGA_SCI_u8UartIndex, FPGA_SCI_au8TxBuffer[FPGA_SCI_nTxIndex++]);
        UART_TxIntEna((UART_eModule_t)FPGA_SCI_u8UartIndex);    /* transmit complete interrupt (interrupt when Char is transferred) */
    }
}

void FPGA_SCI_vProcessUartTx(uint_least8_t u8Index)
{
    if (FPGA_SCI_u8UartIndex == u8Index)
    {
        if(FPGA_SCI_nTxIndex < FPGA_SCI_nBytesToSend)
        {
            if(FPGA_SCI_nTxIndex > 0)
            {
                FPGA_SCI_nTxChSum += FPGA_SCI_au8TxBuffer[FPGA_SCI_nTxIndex];
            }
            UART_PutChar((UART_eModule_t)u8Index, FPGA_SCI_au8TxBuffer[FPGA_SCI_nTxIndex++]);
        }
        else
        {
            UART_PutChar((UART_eModule_t)u8Index, ((0-FPGA_SCI_nTxChSum) & 0x00FF));
            //UART_PutChar((UART_eModule_t)u8Index, 0xAE);
            UART_TxIntDis((UART_eModule_t)u8Index);
        }
    }
}

void FPGA_SCI_vProcessFastTD (uint_least8_t u8Index)
{
    if (FPGA_SCI_u8UartIndex == u8Index)
    {

        if (FPGA_SCI_nRxTodoEcho > 0)
        {
            FPGA_SCI_nRxTodoEcho--;
            if (FPGA_SCI_nRxTodoEcho == 0)
            {
                UART_TransmitDisable((UART_eModule_t)u8Index);
            }
        }
    }
}


void FPGA_SCI_vUartOverflowDetection(void)
{
    FPGA_SCI_eRxState = RX_IDLE;
}




void FPGA_SCI_vProcessTest(SCI_eModule_t eUartFPGARW)
{
    uint16_t index;


    if (FPGA_SCI_bResetUart)
    {
        FPGA_SCI_bResetUart = 0;
        SCI_vInitFIFO(SCI_u32GetBaseFromModuleIndex(eUartFPGARW) , FPGA_UART_BAUD   , (SCI_CONFIG_WLEN_8 | FPGA_UART_STOP_BITS  | (FPGA_SCI_nParityMode & 0x00FF)  | (FPGA_SCI_nAddrMode & 0x00FF)  ));
    }

    if (bSendTxCommand)
    {
        bSendTxCommand = 0;

        //FPGA_SCI_nBytesToSend = FPGA_SCI_nTxTestLength;
        //FPGA_SCI_nRxTodoEcho = FPGA_SCI_nBytesToSend+1;
        //FPGA_SCI_vStartUartTx();

        index = 0;
        while(index < FPGA_SCI_nTxTestLength)
            UART_PutChar((UART_eModule_t)FPGA_SCI_u8UartIndex, FPGA_SCI_au8TxBuffer[index++]);

    }

}







bool FPGA_SCI_bTxCommandMaskAuto = 1;
uint32_t FPGA_SCI_nTxCommandMaskDenyTimeDelay = 0;
uint32_t FPGA_SCI_nTxCommandMaskDenyTime = 0;
uint32_t FPGA_SCI_nTxCommandMaskDenyTimeLast = 0;
uint32_t FPGA_SCI_nTxCommandMaskDenyTimeDiff = 0;

void FPGA_SCI_vErrorMasksSetup(void)
{
    if(FPGA_SCI_bTxCommandMaskAuto)
    {
        FPGA_SCI_nTxCommandMaskDenyTime = TIMER_vGetTimerMainRollingCounter();
        FPGA_SCI_nTxCommandMaskDenyTimeDiff = FPGA_SCI_nTxCommandMaskDenyTime - FPGA_SCI_nTxCommandMaskDenyTimeLast;
        FPGA_SCI_nTxCommandMaskDenyTimeLast = FPGA_SCI_nTxCommandMaskDenyTime;
        if (FPGA_SCI_nTxCommandMaskDenyTimeDelay > FPGA_SCI_nTxCommandMaskDenyTimeDiff)
        {
            FPGA_SCI_nTxCommandMaskDenyTimeDelay -= FPGA_SCI_nTxCommandMaskDenyTimeDiff;
        }
        else
        {
            FPGA_SCI_nTxCommandMaskDenyTimeDelay = 0;
        }

        if ((FPGA_SCI_nTxCommandReq == 0xFF) && (FPGA_SCI_nTxCommandMaskDenyTimeDelay == 0))
        {
            FPGA_SCI_nTxCommandReq = CMD_MSK;
            FPGA_SCI_nTxCommandMaskDenyTimeDelay = u32uSecToTimerTicks(1000000.0);
            FPGA_SCI_nTxCommandMaskDenyTime = TIMER_vGetTimerMainRollingCounter();
            FPGA_SCI_nTxCommandMaskDenyTimeLast = FPGA_SCI_nTxCommandMaskDenyTime;
        }
    }
}




bool FPGA_SCI_vClockSetup(void)
{
    bool bResult = false;

    if (FPGA_SCI_nTxCommandReq == 0xFF)
    {
        /* priority on RTC request */
        FPGA_SCI_nTxCommandReq = CMD_RTC;
        bResult = true;
    }

    return bResult;
}



/* ser_async */
//void ser3_async_main(void)
void FPGA_SCI_vProcess(void)
{
    uint_least8_t index;
    uint_least8_t i;

    bDataUpdate = 0;

    if (FPGA_SCI_nTxCommandReq != 0xFF)
    {
        FPGA_SCI_nTxCommand = FPGA_SCI_nTxCommandReq;
        bSendTxCommand = 1;
        FPGA_SCI_nTxCommandReq = 0xFF;
    }

    if (FPGA_SCI_bPeriodicSend)
    {
        FPGA_SCI_u32SendCounter++;
        if (FPGA_SCI_u32SendCounter >= FPGA_SCI_u32SendCount)
        {
            FPGA_SCI_u32SendCounter = 0;
            bSendTxCommand = 1;
        }
    }



    /* read signal mapping */
    if(FPGA_SCI_bDataRecv)
    {
        index = 0;


        switch(FPGA_SCI_nRxCommand)
        {
            case CMD_RTC :
                bUpdateNonLastExecuting = 1;
                FPGA_SCI_Clock_Get_Seconds     = FPGA_SCI_au8RxBuffer[index++];
                FPGA_SCI_Clock_Get_Minutes     = FPGA_SCI_au8RxBuffer[index++];
                FPGA_SCI_Clock_Get_Hours       = FPGA_SCI_au8RxBuffer[index++];
                FPGA_SCI_Clock_Get_Days        = FPGA_SCI_au8RxBuffer[index++];
                FPGA_SCI_Clock_Get_Weekdays    = FPGA_SCI_au8RxBuffer[index++];
                FPGA_SCI_Clock_Get_Month       = FPGA_SCI_au8RxBuffer[index++];
                FPGA_SCI_Clock_Get_Year        = FPGA_SCI_au8RxBuffer[index++];
                bUpdateNonLastExecuting = 0;
                bRTCGet = 1;
                bDataUpdate = 1;

                bUpdateLastExecuting = 1;
                FPGA_SCI_Clock_Get_Last_Seconds     = FPGA_SCI_au8RxBuffer[index++];
                FPGA_SCI_Clock_Get_Last_Minutes     = FPGA_SCI_au8RxBuffer[index++];
                FPGA_SCI_Clock_Get_Last_Hours       = FPGA_SCI_au8RxBuffer[index++];
                FPGA_SCI_Clock_Get_Last_Days        = FPGA_SCI_au8RxBuffer[index++];
                FPGA_SCI_Clock_Get_Last_Weekdays    = FPGA_SCI_au8RxBuffer[index++];
                FPGA_SCI_Clock_Get_Last_Month       = FPGA_SCI_au8RxBuffer[index++];
                FPGA_SCI_Clock_Get_Last_Year        = FPGA_SCI_au8RxBuffer[index++];
                bUpdateLastExecuting = 0;
                bDataUpdate = 1;
                break;

            case CMD_ERR :
                FPGA_SCI_gdi_det     = (uint16_t)FPGA_SCI_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_au8RxBuffer[index++] << 8);
                FPGA_SCI_gdi_err     = (uint16_t)FPGA_SCI_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_au8RxBuffer[index++] << 8);
                FPGA_SCI_gdi_ovt     = (uint16_t)FPGA_SCI_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_au8RxBuffer[index++] << 8);
                FPGA_SCI_trp_ocn     = (uint16_t)FPGA_SCI_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_au8RxBuffer[index++] << 8);
                FPGA_SCI_trp_ocp     = (uint16_t)FPGA_SCI_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_au8RxBuffer[index++] << 8);
                FPGA_SCI_err_glb     = (uint16_t)FPGA_SCI_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_au8RxBuffer[index++] << 8);
                bDataUpdate = 1;
                break;

            case CMD_MSK :
                FPGA_SCI_ms_det_fbk     = (uint16_t)FPGA_SCI_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_au8RxBuffer[index++] << 8);
                FPGA_SCI_ms_err_fbk     = (uint16_t)FPGA_SCI_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_au8RxBuffer[index++] << 8);
                FPGA_SCI_ms_ovt_fbk     = (uint16_t)FPGA_SCI_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_au8RxBuffer[index++] << 8);
                FPGA_SCI_ms_ocn_fbk     = (uint16_t)FPGA_SCI_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_au8RxBuffer[index++] << 8);
                FPGA_SCI_ms_ocp_fbk     = (uint16_t)FPGA_SCI_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_au8RxBuffer[index++] << 8);
                FPGA_SCI_ms_glb_fbk     = (uint16_t)FPGA_SCI_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_au8RxBuffer[index++] << 8);
                bDataUpdate = 1;
                break;

            case CMD_TMP :
                FPGA_SCI_Temp1      = (uint16_t)FPGA_SCI_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_au8RxBuffer[index++] << 8);
                FPGA_SCI_Temp2      = (uint16_t)FPGA_SCI_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_au8RxBuffer[index++] << 8);
                FPGA_SCI_Temp3      = (uint16_t)FPGA_SCI_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_au8RxBuffer[index++] << 8);
                FPGA_SCI_Temp4      = (uint16_t)FPGA_SCI_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_au8RxBuffer[index++] << 8);
                FPGA_SCI_Temp5      = (uint16_t)FPGA_SCI_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_au8RxBuffer[index++] << 8);
                FPGA_SCI_Temp6      = (uint16_t)FPGA_SCI_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_au8RxBuffer[index++] << 8);
                FPGA_SCI_Temp7      = (uint16_t)FPGA_SCI_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_au8RxBuffer[index++] << 8);
                FPGA_SCI_Temp8      = (uint16_t)FPGA_SCI_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_au8RxBuffer[index++] << 8);
                FPGA_SCI_Temp9      = (uint16_t)FPGA_SCI_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_au8RxBuffer[index++] << 8);
                FPGA_SCI_HuTmp      = (uint16_t)FPGA_SCI_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_au8RxBuffer[index++] << 8);
                FPGA_SCI_Humid      = (uint16_t)FPGA_SCI_au8RxBuffer[index++] + ((uint16_t)FPGA_SCI_au8RxBuffer[index++] << 8);
                bDataUpdate = 1;
                break;
            default:

                break;
        }
    }
    /* read signal mapping END */




    if((FPGA_SCI_bDataRecv) || (bSendTxCommand))
    {
        bSendRxResponse = 0;
        bResponseRxCommandLast = 0;
        if (bSendTxCommand)
        {
            bSendTxCommand = 0;
            bSendRxResponse = 1;
        }
        else
        {
            if (FPGA_SCI_bDataRecvUse)
            {
                FPGA_SCI_nTxCommand = FPGA_SCI_nRxCommand;
                if (bResponseRxCommand)
                {
                    bResponseRxCommandLast = 1;
                    bSendRxResponse = 1;
                }
            }
        }

        if (bSendRxResponse)
        {
            index = 0;
            FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_nAddressFPGA;
            FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_nTxCommand;
            FPGA_SCI_au8TxBuffer[index++] = 0;

            i = index;
            switch(FPGA_SCI_nTxCommand)
            {
                case CMD_RTC :
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_Clock_Set_Seconds;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_Clock_Set_Minutes;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_Clock_Set_Hours;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_Clock_Set_Days;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_Clock_Set_Weekdays;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_Clock_Set_Month;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_Clock_Set_Year;


                    if ((bRxResponseIncrementData) && (bResponseRxCommandLast))
                    {
                        index = i;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                    }

                    i = index - i;
                    break;

                case CMD_ERR :
#if 0
                    FPGA_SCI_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_gdi_det;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_gdi_det >> 8;
                    FPGA_SCI_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_gdi_err;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_gdi_err >> 8;
                    FPGA_SCI_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_gdi_ovt;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_gdi_ovt >> 8;
                    FPGA_SCI_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_trp_ocn;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_trp_ocn >> 8;
                    FPGA_SCI_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_trp_ocp;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_trp_ocp >> 8;
                    FPGA_SCI_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_err_glb;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_err_glb >> 8;
#endif
                    if ((bRxResponseIncrementData) && (bResponseRxCommandLast))
                    {
                        index = i;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                    }

                    i = index - i;
                    break;

                case CMD_MSK :
                    FPGA_SCI_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_ms_det;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_ms_det >> 8;
                    FPGA_SCI_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_ms_err;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_ms_err >> 8;
                    FPGA_SCI_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_ms_ovt;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_ms_ovt >> 8;
                    FPGA_SCI_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_ms_ocn;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_ms_ocn >> 8;
                    FPGA_SCI_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_ms_ocp;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_ms_ocp >> 8;
                    FPGA_SCI_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_ms_glb;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_ms_glb >> 8;

                    if ((bRxResponseIncrementData) && (bResponseRxCommandLast))
                    {
                        index = i;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                    }

                    i = index - i;
                    break;

                case CMD_TMP :
                    FPGA_SCI_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_Temp1;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_Temp1 >> 8;
                    FPGA_SCI_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_Temp2;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_Temp2 >> 8;
                    FPGA_SCI_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_Temp3;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_Temp3 >> 8;
                    FPGA_SCI_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_Temp4;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_Temp4 >> 8;
                    FPGA_SCI_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_Temp5;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_Temp5 >> 8;
                    FPGA_SCI_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_Temp6;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_Temp6 >> 8;
                    FPGA_SCI_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_Temp7;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_Temp7 >> 8;
                    FPGA_SCI_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_Temp8;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_Temp8 >> 8;
                    FPGA_SCI_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_Temp9;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_Temp9 >> 8;
                    FPGA_SCI_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_HuTmp;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_HuTmp >> 8;
                    FPGA_SCI_au8TxBuffer[index++] = (uint_least8_t)FPGA_SCI_Humid;
                    FPGA_SCI_au8TxBuffer[index++] = FPGA_SCI_Humid >> 8;

                    if ((bRxResponseIncrementData) && (bResponseRxCommandLast))
                    {
                        index = i;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                        FPGA_SCI_au8TxBuffer[index++] += 1;
                    }
                    i = index - i;
                    break;

                default:
                    FPGA_SCI_au8TxBuffer[index-2] |= 0x80;   /* CMD Error Code (not implemented) */

            }
            FPGA_SCI_au8TxBuffer[2] = i;                   /* LEN */

            FPGA_SCI_nBytesToSend = index;


            FPGA_SCI_nRxTodoEcho = FPGA_SCI_nBytesToSend+1;
            FPGA_SCI_vStartUartTx();
        }
        FPGA_SCI_bDataRecv = 0;
    }

    /* write signal mapping //FPGA_SCI_nRxCommand */
    if(bDataUpdate)
    {
        FPGA_SCI_gdi_det_err = ((~FPGA_SCI_gdi_det) & FPGA_SCI_ms_det);
        FPGA_SCI_gdi_err_err = ((~FPGA_SCI_gdi_err) & FPGA_SCI_ms_err);
        FPGA_SCI_gdi_ovt_err = ((~FPGA_SCI_gdi_ovt) & FPGA_SCI_ms_ovt);
        FPGA_SCI_trp_ocn_err = ((~FPGA_SCI_trp_ocn) & FPGA_SCI_ms_ocn);
        FPGA_SCI_trp_ocp_err = ((~FPGA_SCI_trp_ocp) & FPGA_SCI_ms_ocp);
        FPGA_SCI_err_glb_err = ((~FPGA_SCI_err_glb) & FPGA_SCI_ms_glb);



        FPGA_SCI_uTmpError meas_temp_err = {0};

        meas_temp_err.sTmpError.DrvError1 = (FPGA_SCI_Temp1 > MCLIB_s16d(BOARDCFG_TRIP_TEMPERATURE_DEG)) ? true : false;
        meas_temp_err.sTmpError.DrvError2 = (FPGA_SCI_Temp2 > MCLIB_s16d(BOARDCFG_TRIP_TEMPERATURE_DEG)) ? true : false;
        meas_temp_err.sTmpError.DrvError3 = (FPGA_SCI_Temp3 > MCLIB_s16d(BOARDCFG_TRIP_TEMPERATURE_DEG)) ? true : false;
        meas_temp_err.sTmpError.DrvError4 = (FPGA_SCI_Temp4 > MCLIB_s16d(BOARDCFG_TRIP_TEMPERATURE_DEG)) ? true : false;
        meas_temp_err.sTmpError.DrvError5 = (FPGA_SCI_Temp5 > MCLIB_s16d(BOARDCFG_TRIP_TEMPERATURE_DEG)) ? true : false;
        meas_temp_err.sTmpError.DrvError6 = (FPGA_SCI_Temp6 > MCLIB_s16d(BOARDCFG_TRIP_TEMPERATURE_DEG)) ? true : false;
        meas_temp_err.sTmpError.DrvError7 = (FPGA_SCI_Temp7 > MCLIB_s16d(BOARDCFG_TRIP_TEMPERATURE_DEG)) ? true : false;
        meas_temp_err.sTmpError.DrvError8 = (FPGA_SCI_Temp8 > MCLIB_s16d(BOARDCFG_TRIP_TEMPERATURE_DEG)) ? true : false;
        meas_temp_err.sTmpError.DrvError9 = (FPGA_SCI_Temp9 > MCLIB_s16d(BOARDCFG_TRIP_TEMPERATURE_DEG)) ? true : false;
        meas_temp_err.sTmpError.OvtBoard  = (FPGA_SCI_HuTmp > MCLIB_s16d(BOARDCFG_TRIP_TEMPERATURE_DEG)) ? true : false; /* board Temp */
        meas_temp_err.sTmpError.Humidity  = (FPGA_SCI_Humid > MCLIB_s16d(BOARDCFG_TRIP_HUMIDITY_PERCENT)) ? true : false; /* board Temp */


        FPGA_SCI_meas_temp_err = meas_temp_err.u16Register & FPGA_SCI_ms_meas_temp_err;

        if ((FPGA_SCI_meas_temp_err > 0) && (FPGA_SCI_meas_temp_err_Trip == 0))
        {
            FPGA_SCI_meas_temp_err_Trip = FPGA_SCI_meas_temp_err;

            FPGA_SCI_Temp1_Trip = FPGA_SCI_Temp1;
            FPGA_SCI_Temp2_Trip = FPGA_SCI_Temp2;
            FPGA_SCI_Temp3_Trip = FPGA_SCI_Temp3;
            FPGA_SCI_Temp4_Trip = FPGA_SCI_Temp4;
            FPGA_SCI_Temp5_Trip = FPGA_SCI_Temp5;
            FPGA_SCI_Temp6_Trip = FPGA_SCI_Temp6;
            FPGA_SCI_Temp7_Trip = FPGA_SCI_Temp7;
            FPGA_SCI_Temp8_Trip = FPGA_SCI_Temp8;
            FPGA_SCI_Temp9_Trip = FPGA_SCI_Temp9;
            FPGA_SCI_HuTmp_Trip = FPGA_SCI_HuTmp;
            FPGA_SCI_Humid_Trip = FPGA_SCI_Humid;

        }



    }
    /* write signal mapping END */

}

void FPGA_SCI_vClearTrippedErrors (void)
{
    FPGA_SCI_gdi_det_err = 0;
    FPGA_SCI_gdi_err_err = 0;
    FPGA_SCI_gdi_ovt_err = 0;
    FPGA_SCI_trp_ocn_err = 0;
    FPGA_SCI_trp_ocp_err = 0;
    FPGA_SCI_err_glb_err = 0;


    FPGA_SCI_meas_temp_err = 0;

    FPGA_SCI_gdi_det_err_Trip = 0;
    FPGA_SCI_gdi_err_err_Trip = 0;
    FPGA_SCI_gdi_ovt_err_Trip = 0;
    FPGA_SCI_trp_ocn_err_Trip = 0;
    FPGA_SCI_trp_ocp_err_Trip = 0;
    FPGA_SCI_err_glb_err_Trip = 0;

    FPGA_SCI_meas_temp_err_Trip = 0;
}

bool FPGA_SCI_bIsError (void)
{
    bool bResult = false;

    if (FPGA_SCI_meas_temp_err_Trip > 0)
    {
        bResult = true;
    }

    if (
           (FPGA_SCI_gdi_det_err > 0)
        || (FPGA_SCI_gdi_err_err > 0)
        || (FPGA_SCI_gdi_ovt_err > 0)
        || (FPGA_SCI_trp_ocn_err > 0)
        || (FPGA_SCI_trp_ocp_err > 0)
        || (FPGA_SCI_err_glb_err > 0)
        )
    {
        FPGA_SCI_gdi_det_err_Trip = FPGA_SCI_gdi_det_err;
        FPGA_SCI_gdi_err_err_Trip = FPGA_SCI_gdi_err_err;
        FPGA_SCI_gdi_ovt_err_Trip = FPGA_SCI_gdi_ovt_err;
        FPGA_SCI_trp_ocn_err_Trip = FPGA_SCI_trp_ocn_err;
        FPGA_SCI_trp_ocp_err_Trip = FPGA_SCI_trp_ocp_err;
        FPGA_SCI_err_glb_err_Trip = FPGA_SCI_err_glb_err;



        bResult = true;
    }
    return bResult;
}


bool FPGA_SCI_bIsErrorTemp (void)
{
    bool bResult = false;

    if (FPGA_SCI_meas_temp_err > 0)
    {
        bResult = true;
    }
    return bResult;
}

bool FPGA_SCI_bIsErrorDet (void)
{
    bool bResult = false;

    if (FPGA_SCI_gdi_det_err > 0)
    {
        bResult = true;
    }
    return bResult;
}

bool FPGA_SCI_bIsErrorDrv (void)
{
    bool bResult = false;

    if (FPGA_SCI_gdi_err_err > 0)
    {
        bResult = true;
    }
    return bResult;
}

bool FPGA_SCI_bIsErrorOvt (void)
{
    bool bResult = false;

    if (FPGA_SCI_meas_temp_err > 0)
    {
        bResult = true;
    }

    if (FPGA_SCI_gdi_ovt_err > 0)
    {
        bResult = true;
    }
    return bResult;
}

bool FPGA_SCI_bIsErrorOcn (void)
{
    bool bResult = false;

    if (FPGA_SCI_trp_ocn_err > 0)
    {
        bResult = true;
    }
    return bResult;
}

bool FPGA_SCI_bIsErrorOcp (void)
{
    bool bResult = false;

    if (FPGA_SCI_trp_ocp_err > 0)
    {
        bResult = true;
    }
    return bResult;
}

bool FPGA_SCI_bIsErrorVdc (void)
{
    bool bResult = false;
    FPGA_SCI_uDrvError err_glb_err;
    err_glb_err.u16Register = FPGA_SCI_err_glb_err;

    if (err_glb_err.sDrvError.OVError1 > 0)
    {
        bResult = true;
    }
    return bResult;
}

bool FPGA_SCI_bIsErrorGlb (void)
{
    bool bResult = false;
    FPGA_SCI_uDrvError err_glb_err;
    err_glb_err.u16Register = FPGA_SCI_err_glb_err;

    if (err_glb_err.u16Register > 0)
    {
        bResult = true;
    }
    return bResult;
}

void FPGA_SCI_vSetStackUsed(uint16_t nStack)
{
    uint16_t u16StartBitPos;
    uint16_t u16Mask;

    u16StartBitPos = nStack * 3;
    u16Mask = 0x7 << u16StartBitPos;


    FPGA_SCI_ms_det |= u16Mask;
    FPGA_SCI_ms_err |= u16Mask;
    FPGA_SCI_ms_ovt |= u16Mask;
    FPGA_SCI_ms_ocn |= u16Mask;
    FPGA_SCI_ms_ocp |= u16Mask;
    FPGA_SCI_ms_glb |= u16Mask;
    FPGA_SCI_ms_meas_temp_err |= u16Mask;
}

void FPGA_SCI_vSetStackDeny(uint16_t nStack)
{
    uint16_t u16StartBitPos;
    uint16_t u16Mask;

    u16StartBitPos = nStack * 3;
    u16Mask = 0x7 << u16StartBitPos;


    FPGA_SCI_ms_det &= ~u16Mask;
    FPGA_SCI_ms_err &= ~u16Mask;
    FPGA_SCI_ms_ovt &= ~u16Mask;
    FPGA_SCI_ms_ocn &= ~u16Mask;
    FPGA_SCI_ms_ocp &= ~u16Mask;
    FPGA_SCI_ms_glb &= ~u16Mask;
    FPGA_SCI_ms_meas_temp_err &= ~u16Mask;
}









FPGA_SCI_sTimeSyncModeYYMMDD_t FPGA_SCI_GetDate (void)
{
    FPGA_SCI_sTimeSyncModeYYMMDD_t sDate;

    if (bUpdateNonLastExecuting == 0)
    {
        sDate.u8Year    = FPGA_SCI_Clock_Get_Year;
        sDate.u8Day     = FPGA_SCI_Clock_Get_Days;
        sDate.u8Month   = FPGA_SCI_Clock_Get_Month;
    }
    else if (bUpdateLastExecuting == 0)
    {
        sDate.u8Year    = FPGA_SCI_Clock_Get_Last_Year;
        sDate.u8Day     = FPGA_SCI_Clock_Get_Last_Days;
        sDate.u8Month   = FPGA_SCI_Clock_Get_Last_Month;
    }
    return sDate;
}

FPGA_SCI_sTimeSyncWWHHMMSS_t FPGA_SCI_GetTime (void)
{
    FPGA_SCI_sTimeSyncWWHHMMSS_t sTime;

    if (bUpdateNonLastExecuting == 0)
    {
        sTime.u8Hour    = FPGA_SCI_Clock_Get_Hours;
        sTime.u8Minute  = FPGA_SCI_Clock_Get_Minutes;
        sTime.u8Second  = FPGA_SCI_Clock_Get_Seconds;
        sTime.u8WeekDays= FPGA_SCI_Clock_Get_Weekdays;
    }
    else if (bUpdateLastExecuting == 0)
    {
        sTime.u8Hour    = FPGA_SCI_Clock_Get_Last_Hours;
        sTime.u8Minute  = FPGA_SCI_Clock_Get_Last_Minutes;
        sTime.u8Second  = FPGA_SCI_Clock_Get_Last_Seconds;
        sTime.u8WeekDays= FPGA_SCI_Clock_Get_Last_Weekdays;
    }
    return sTime;
}



void FPGA_SCI_vSetDateTime (
        uint_least8_t u8BCDSeconds,
        uint_least8_t u8BCDMinutes,
        uint_least8_t u8BCDHours,
        uint_least8_t u8BCDWeekdays,
        uint_least8_t u8BCDDay,
        uint_least8_t u8BCDMonth,
        uint_least8_t u8BCDYear)
{
    FPGA_SCI_Clock_Set_Seconds  = u8BCDSeconds;
    FPGA_SCI_Clock_Set_Minutes  = u8BCDMinutes;
    FPGA_SCI_Clock_Set_Hours    = u8BCDHours;
    FPGA_SCI_Clock_Set_Weekdays = u8BCDWeekdays;
    FPGA_SCI_Clock_Set_Days     = u8BCDDay;
    FPGA_SCI_Clock_Set_Month    = u8BCDMonth;
    FPGA_SCI_Clock_Set_Year     = u8BCDYear;

}

void FPGA_SCI_vGetDateTime (
        uint_least8_t* pu8BCDSeconds,
        uint_least8_t* pu8BCDMinutes,
        uint_least8_t* pu8BCDHours,
        uint_least8_t* pu8BCDWeekdays,
        uint_least8_t* pu8BCDDay,
        uint_least8_t* pu8BCDMonth,
        uint_least8_t* pu8BCDYear)
{
    if (bUpdateNonLastExecuting == 0)
    {
        *pu8BCDSeconds  = FPGA_SCI_Clock_Get_Seconds;
        *pu8BCDMinutes  = FPGA_SCI_Clock_Get_Minutes;
        *pu8BCDHours    = FPGA_SCI_Clock_Get_Hours;
        *pu8BCDWeekdays = FPGA_SCI_Clock_Get_Weekdays;
        *pu8BCDDay      = FPGA_SCI_Clock_Get_Days;
        *pu8BCDMonth    = FPGA_SCI_Clock_Get_Month;
        *pu8BCDYear     = FPGA_SCI_Clock_Get_Year;
    }
    else if (bUpdateLastExecuting == 0)
    {
        *pu8BCDSeconds  = FPGA_SCI_Clock_Get_Last_Seconds;
        *pu8BCDMinutes  = FPGA_SCI_Clock_Get_Last_Minutes;
        *pu8BCDHours    = FPGA_SCI_Clock_Get_Last_Hours;
        *pu8BCDWeekdays = FPGA_SCI_Clock_Get_Last_Weekdays;
        *pu8BCDDay      = FPGA_SCI_Clock_Get_Last_Days;
        *pu8BCDMonth    = FPGA_SCI_Clock_Get_Last_Month;
        *pu8BCDYear     = FPGA_SCI_Clock_Get_Last_Year;
    }
}



