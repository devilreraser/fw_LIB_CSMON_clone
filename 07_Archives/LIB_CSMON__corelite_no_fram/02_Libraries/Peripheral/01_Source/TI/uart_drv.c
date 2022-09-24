/* *****************************************************************************
 * File:   uart.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 02 11 16:16
 * 
 * Description: UART Initialization and Error Check
 * 
 **************************************************************************** */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#if 0
#include <xc.h>
#endif

#include <stdbool.h>

#include "boardcfg.h"
#include "uart_drv.h"
#include "sci_drv.h"
#ifdef USE_CTREADER
#include "ctreader.h"
#endif
#include "fpga_sci.h"

#include "hmlib.h"

#if 0
#include "isr.h"
#endif

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define USE_DMA_UART       0           /* 0 or 1 - Not Implemented 1 */
#define USE_INT_UART_RX    HMDRV_SCI_RX_ISR           /* 0 or 1 */
#define USE_INT_UART_TX    HMDRV_SCI_TX_ISR           /* 0 or 1 */



/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */



/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
UART_sHandle_t UART_sHandle;

/* ??? 123 Mark Change UART_RX_BUFFER_SIZE was 256 */
#define UART_RX_BUFFER_SIZE    256
uint16_t UARTRxBuffer[UART_COUNT][UART_RX_BUFFER_SIZE];
uint32_t UARTRxBufferTimeDiff[UART_COUNT][UART_RX_BUFFER_SIZE];
uint16_t UARTRxBufferRDIndex[UART_COUNT] = {0,0,0,0};
uint16_t UARTRxBufferWRIndex[UART_COUNT] = {0,0,0,0};

uint16_t UARTRxBufferWRCount[UART_COUNT] = {0,0,0,0};  /* How many received We have to process */
uint16_t UARTRxBufferRDCount[UART_COUNT] = {0,0,0,0};

uint32_t UART_RXOverflowCounter[UART_COUNT] = {0,0,0,0};
uint32_t UART_RXIntOverflowCounter[UART_COUNT] = {0,0,0,0};

uint32_t u32TimeDiffRead[UART_COUNT] = {0,0,0,0};
uint32_t u32TimeDiffNow[UART_COUNT] = {0,0,0,0};
uint32_t u32TimeDiffLast[UART_COUNT] = {0,0,0,0};

bool bUseSCIA_On_USB = USE_SCIA_ON_USB_CONNECTOR;

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */

bool UART_bUseSCIA_On_USB_Connector(void)
{
    return bUseSCIA_On_USB;
}

void UART_TxEna(UART_eModule_t index)
{
    if (index == 0)
    {
        if (bUseSCIA_On_USB == 0)
        {
            #ifdef SCIDEA_PIN
            GPIO_writePin(SCIDEA_PIN, SCIDE_ENABLE);
            #endif
        }
    }
    else if (index == 1)
    {
        #ifdef SCIDEB_PIN
        GPIO_writePin(SCIDEB_PIN, SCIDE_ENABLE);
        #endif
    }
    else if (index == 2)
    {
        #ifdef SCIDEC_PIN
        GPIO_writePin(SCIDEC_PIN, SCIDE_ENABLE);
        #endif
    }
    else if (index == 3)
    {
        #ifdef SCIDED_PIN
        GPIO_writePin(SCIDED_PIN, SCIDE_ENABLE);
        #endif
    }

}

void UART_TxDis(UART_eModule_t index)
{
    if (index == 0)
    {
        if (bUseSCIA_On_USB == 0)
        {
            #ifdef SCIDEA_PIN
            GPIO_writePin(SCIDEA_PIN, SCIDE_DISABLE);
            #endif
        }
    }
    else if (index == 1)
    {
        #ifdef SCIDEB_PIN
        GPIO_writePin(SCIDEB_PIN, SCIDE_DISABLE);
        #endif
    }
    else if (index == 2)
    {
        #ifdef SCIDEC_PIN
        GPIO_writePin(SCIDEC_PIN, SCIDE_DISABLE);
        #endif
    }
    else if (index == 3)
    {
        #ifdef SCIDED_PIN
        GPIO_writePin(SCIDED_PIN, SCIDE_DISABLE);
        #endif
    }

}


void UART_RxEna(UART_eModule_t index)
{
    if (index == 0)
    {
        if (bUseSCIA_On_USB == 0)
        {
            #ifdef SCIREA_PIN
            GPIO_writePin(SCIREA_PIN, SCIRE_ENABLE);
            #endif
        }
    }
    else if (index == 1)
    {
        #ifdef SCIREB_PIN
        GPIO_writePin(SCIREB_PIN, SCIRE_ENABLE);
        #endif
    }
    else if (index == 2)
    {
        #ifdef SCIREC_PIN
        GPIO_writePin(SCIREC_PIN, SCIRE_ENABLE);
        #endif
    }
    else if (index == 3)
    {
        #ifdef SCIRED_PIN
        GPIO_writePin(SCIRED_PIN, SCIRE_ENABLE);
        #endif
    }

}

void UART_RxDis(UART_eModule_t index)
{
    if (index == 0)
    {
        if (bUseSCIA_On_USB == 0)
        {
            #ifdef SCIREA_PIN
            GPIO_writePin(SCIREA_PIN, SCIRE_DISABLE);
            #endif
        }
    }
    else if (index == 1)
    {
        #ifdef SCIREB_PIN
        GPIO_writePin(SCIREB_PIN, SCIRE_DISABLE);
        #endif
    }
    else if (index == 2)
    {
        #ifdef SCIREC_PIN
        GPIO_writePin(SCIREC_PIN, SCIRE_DISABLE);
        #endif
    }
    else if (index == 3)
    {
        #ifdef SCIRED_PIN
        GPIO_writePin(SCIRED_PIN, SCIRE_DISABLE);
        #endif
    }

}




void UART_TxIntEna(UART_eModule_t index)
{
    uint32_t base = SCI_u32GetBaseFromModuleIndex((SCI_eModule_t)index);
    SCI_clearInterruptStatus(base, SCI_INT_TXFF);
    SCI_enableInterrupt(base, SCI_INT_TXFF);
}
    
void UART_TxIntDis(UART_eModule_t index)
{
    uint32_t base = SCI_u32GetBaseFromModuleIndex((SCI_eModule_t)index);
    SCI_disableInterrupt(base, SCI_INT_TXFF);
    SCI_clearInterruptStatus(base, SCI_INT_TXFF);
}
    

//#define HMDRV_READY_TX(uart)        ((((UART_sBaseRegisters_t*)&U1MODE)[uart].UARTSTAbits.UTXBF == 0) ? true : false)
bool UART_IsTxReady(UART_eModule_t index)
{
    uint32_t base = SCI_u32GetBaseFromModuleIndex((SCI_eModule_t)index);
    return ((SCI_getTxFIFOStatus(base) < HMDRV_MAX_UART_FIFO_DEPTH) ? true : false);
}

//#define HMDRV_SCI_PUTCHAR(uart, ch)   do {((UART_sBaseRegisters_t*)&U1MODE)[uart].UARTTXREG = ch; } while(0)
void UART_PutChar(UART_eModule_t index, uint16_t data)
{
    uint32_t base = SCI_u32GetBaseFromModuleIndex((SCI_eModule_t)index);
    SCI_writeCharNonBlocking(base, data);
}

void UART_PutNextAddress(UART_eModule_t index)  /* TxWake Mode TI */
{
    uint32_t base = SCI_u32GetBaseFromModuleIndex((SCI_eModule_t)index);
    HWREGH(base + SCI_O_CTL1) |= SCI_CTL1_TXWAKE;
}

//#define HMDRV_EMPTY_TX(uart)        (( (((UART_sBaseRegisters_t*)&U1MODE)[uart].UARTSTAbits.TRMT == 0)) ? false : true)
bool UART_IsTxEmpty(UART_eModule_t index)
{
    uint32_t base = SCI_u32GetBaseFromModuleIndex((SCI_eModule_t)index);

    //return ( SCI_isTransmitterBusy(base) ? false : true );//to do fix here
    return ( ((SCI_getTxFIFOStatus(base) == 0) && ((HWREGH(base + SCI_O_CTL2) & SCI_CTL2_TXEMPTY) == SCI_CTL2_TXEMPTY)) ? true : false);

}

//#define HMDRV_READY_RX(uart)        ((((UART_sBaseRegisters_t*)&U1MODE)[uart].UARTSTAbits.URXDA == 0) ? true : false)
bool UART_IsRxReady(UART_eModule_t index)
{
    uint32_t base = SCI_u32GetBaseFromModuleIndex((SCI_eModule_t)index);
    return ((SCI_getRxFIFOStatus(base) > 0) ? true : false);
}

//#define HMDRV_SCI_GETCHAR(uart)   ((UART_sBaseRegisters_t*)&U1MODE)[uart].UARTRXREG
uint16_t UART_GetChar(UART_eModule_t index)
{
    uint32_t base = SCI_u32GetBaseFromModuleIndex((SCI_eModule_t)index);
    return SCI_readCharNonBlocking(base);
}

void UART_vReset (void)
{

}


/* *****************************************************************************
 * UART_vInit
 * 
 * Description: UART Initialization 
 **************************************************************************** */
void UART_vInit ( UART_eModule_t index)
{
    // setup the UART peripheral
    u32TimeDiffLast[index] = HMDRV_GET_FREE_RUN_TIMER();

#if USE_INT_UART_RX
    UARTRxBufferRDIndex[index] = 0;
    UARTRxBufferWRIndex[index] = 0;
    UARTRxBufferWRCount[index]=  0;
    UARTRxBufferRDCount[index] = 0;
#endif
    
}



bool UART_ReadDataRxAvailable(UART_eModule_t index)
{

    bool result;
    
    if (UARTRxBufferRDCount[index] != UARTRxBufferWRCount[index])
    {
        result = true;
    }
    else   
    {
        result = false;
    }
    return result;    

}





void UART_TransmitEnable(UART_eModule_t index)
{
    if(index == UART_1)
    {
        #ifdef SCIDEA_PIN
        GPIO_writePin(SCIDEA_PIN, 1);
        #endif
    }
    else if(index == UART_2)
    {
        #ifdef SCIDEB_PIN
        GPIO_writePin(SCIDEB_PIN, 1);
        #endif
    }
    else if(index == UART_3)
    {
        #ifdef SCIDEC_PIN
        GPIO_writePin(SCIDEC_PIN, 1);
        #endif
    }
    else if(index == UART_4)
    {
        #ifdef SCIDED_PIN
        GPIO_writePin(SCIDED_PIN, 1);
        #endif
    }

}

void UART_TransmitDisable(UART_eModule_t index)
{
    if(index == UART_1)
    {
        #ifdef SCIDEA_PIN
        GPIO_writePin(SCIDEA_PIN, 0);
        #endif
    }
    else if(index == UART_2)
    {
        #ifdef SCIDEB_PIN
        GPIO_writePin(SCIDEB_PIN, 0);
        #endif
    }
    else if(index == UART_3)
    {
        #ifdef SCIDEC_PIN
        GPIO_writePin(SCIDEC_PIN, 0);
        #endif
    }
    else if(index == UART_4)
    {
        #ifdef SCIDED_PIN
        GPIO_writePin(SCIDED_PIN, 0);
        #endif
    }

}


uint32_t UART_ReadDataRxTimeDiff(UART_eModule_t index)
{

    return u32TimeDiffRead[index];   
}

uint16_t UART_ReadDataRx(UART_eModule_t index)
{
    uint16_t result;
    
    if (UARTRxBufferRDCount[index] != UARTRxBufferWRCount[index])
    {
        UARTRxBufferRDCount[index]++;
        u32TimeDiffRead[index] = UARTRxBufferTimeDiff[index][UARTRxBufferRDIndex[index]];
        result = UARTRxBuffer[index][UARTRxBufferRDIndex[index]++];
        if (UARTRxBufferRDIndex[index] >= UART_RX_BUFFER_SIZE)
        {
            UARTRxBufferRDIndex[index] = 0;
        }        
        
    }
    else
    {
        result = 0xEEEE;      /* invalid data */
    }
    return result;
}




/*******************************************************************************
 * Function:  void __attribute__ ((interrupt, auto_psv)) _U1RXInterrupt(void)
 *
 * PreCondition: UART Module must be Initialized with receive interrupt enabled.
 *
 * Input:        None
 *                  
 * Output:       None
 *
 * Side Effects: None
 *
 * Overview:     UART receive interrupt service routine called whenever a byte
 *               of data received in UART Rx buffer.
 *****************************************************************************/
void UART_RXInterrupt(UART_eModule_t index)
{
    //DEBUG_SET_USART_RX(index);
    
    //could use here that Tx Interrupt is disabled
    HMLIB_vProcessFastTD(index);
#ifdef USE_CTREADER
    CTREADER_vProcessFastTD(index);
#endif
    FPGA_SCI_vProcessFastTD(index);
    
    if ((UARTRxBufferWRCount[index] - UARTRxBufferRDCount[index]) <= UART_RX_BUFFER_SIZE)
    {
        u32TimeDiffNow[index] = HMDRV_GET_FREE_RUN_TIMER();
        UARTRxBufferTimeDiff[index][UARTRxBufferWRIndex[index]] = u32TimeDiffNow[index] - u32TimeDiffLast[index];
#if HMDRV_FREE_RUN_TIMER_DECREMENT_COUNTER
        UARTRxBufferTimeDiff[index][UARTRxBufferWRIndex[index]] = 0 - UARTRxBufferTimeDiff[index][UARTRxBufferWRIndex[index]];
#endif
        u32TimeDiffLast[index] = u32TimeDiffNow[index];
        UARTRxBuffer[index][UARTRxBufferWRIndex[index]++] = UART_GetChar(index);
        UARTRxBufferWRCount[index]++;
        if (UARTRxBufferWRIndex[index] >= UART_RX_BUFFER_SIZE)
        {
            UARTRxBufferWRIndex[index] = 0;
        } 
        HMDRV_RESET_TX_DELAY(index);
    }
    else
    {
        UART_GetChar(index);
        UART_RXIntOverflowCounter[index]++;
    }
    //DEBUG_CLR_USART_RX(index);
}

/******************************************************************************
 * Function:   void __attribute__ ((interrupt, auto_psv)) _U1TXInterrupt(void)
 *
 * PreCondition: UART Module must be Initialized with transmit interrupt enabled.
 *
 * Input:        None
 *                  
 * Output:       None
 *
 * Side Effects: None
 *
 * Overview:     UART transmit interrupt service routine called whenever a data
 *               is sent from UART Tx buffer
 *****************************************************************************/
void UART_TXInterrupt(UART_eModule_t index)
{
    //DEBUG_SET_USART1_TX(index);
    //HMLIB_vProcessSCITXISR();
    HMLIB_vProcessSCITXISRWithIndex(index);
#ifdef USE_CTREADER
    CTREADER_vProcessUartTx(index);
#endif
    FPGA_SCI_vProcessUartTx(index);

    //DEBUG_CLR_USART1_TX(index);
}





