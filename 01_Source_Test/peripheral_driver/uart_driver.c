/* *****************************************************************************
 * File:   uart_driver.c
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
#include <stdbool.h>

#include "uart_driver.h"
#include "sci_driver.h"
#include "fpga_sci_driver.h"
#include "device.h"


/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define USE_DMA_UART                0           /* 0 or 1 (1 is not Implemented) */
#define USE_INT_UART_RX             1           /* 0 or 1 */
#define USE_INT_UART_TX             1           /* 0 or 1 */

#define UART_DRV_FIFO_DEPTH         16          /* Change this to 4 for Picollo */
#define UART_DRV_RX_BUFFER_SIZE     64
#define UART_DRV_GET_FREE_RUN_TIMER_DECREMENT_COUNTER   1

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Function-Like Macro Definitions
 **************************************************************************** */
#define UART_DRV_GET_FREE_RUN_TIMER()          CPUTimer_getTimerCount(CPUTIMER1_BASE)

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */

uint16_t UART_DRV_RxBuffer[UART_DRV_COUNT][UART_DRV_RX_BUFFER_SIZE];
uint32_t UART_DRV_RxBufferTimeDiff[UART_DRV_COUNT][UART_DRV_RX_BUFFER_SIZE];
uint16_t UART_DRV_RxBufferRDIndex[UART_DRV_COUNT] = {0,0,0,0};
uint16_t UART_DRV_RxBufferWRIndex[UART_DRV_COUNT] = {0,0,0,0};

uint16_t UART_DRV_RxBufferWRCount[UART_DRV_COUNT] = {0,0,0,0};  /* How many received We have to process */
uint16_t UART_DRV_RxBufferRDCount[UART_DRV_COUNT] = {0,0,0,0};

uint32_t UART_DRV_RXOverflowCounter[UART_DRV_COUNT] = {0,0,0,0};
uint32_t UART_DRV_RXIntOverflowCounter[UART_DRV_COUNT] = {0,0,0,0};

uint32_t UART_DRV_u32TimeDiffRead[UART_DRV_COUNT] = {0,0,0,0};
uint32_t UART_DRV_u32TimeDiffNow[UART_DRV_COUNT] = {0,0,0,0};
uint32_t UART_DRV_u32TimeDiffLast[UART_DRV_COUNT] = {0,0,0,0};

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */

void UART_DRV_TxEna(UART_DRV_eModule_t index)
{
    if (index == 0)
    {
        #ifdef SCIDEA_PIN
        GPIO_writePin(SCIDEA_PIN, SCIDE_ENABLE);
        #endif
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

void UART_DRV_TxDis(UART_DRV_eModule_t index)
{
    if (index == 0)
    {
        #ifdef SCIDEA_PIN
        GPIO_writePin(SCIDEA_PIN, SCIDE_DISABLE);
        #endif

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


void UART_DRV_RxEna(UART_DRV_eModule_t index)
{
    if (index == 0)
    {
        #ifdef SCIREA_PIN
        GPIO_writePin(SCIREA_PIN, SCIRE_ENABLE);
        #endif
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

void UART_DRV_RxDis(UART_DRV_eModule_t index)
{
    if (index == 0)
    {
        #ifdef SCIREA_PIN
        GPIO_writePin(SCIREA_PIN, SCIRE_DISABLE);
        #endif
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




void UART_DRV_TxIntEna(UART_DRV_eModule_t index)
{
    uint32_t base = SCI_DRV_u32GetBaseFromModuleIndex((SCI_DRV_eModule_t)index);
    SCI_clearInterruptStatus(base, SCI_INT_TXFF);
    SCI_enableInterrupt(base, SCI_INT_TXFF);
}
    
void UART_DRV_TxIntDis(UART_DRV_eModule_t index)
{
    uint32_t base = SCI_DRV_u32GetBaseFromModuleIndex((SCI_DRV_eModule_t)index);
    SCI_disableInterrupt(base, SCI_INT_TXFF);
    SCI_clearInterruptStatus(base, SCI_INT_TXFF);
}
    

bool UART_DRV_IsTxReady(UART_DRV_eModule_t index)
{
    uint32_t base = SCI_DRV_u32GetBaseFromModuleIndex((SCI_DRV_eModule_t)index);
    return ((SCI_getTxFIFOStatus(base) < UART_DRV_FIFO_DEPTH) ? true : false);
}

void UART_DRV_PutChar(UART_DRV_eModule_t index, uint16_t data)
{
    uint32_t base = SCI_DRV_u32GetBaseFromModuleIndex((SCI_DRV_eModule_t)index);
    SCI_writeCharNonBlocking(base, data);
}

void UART_DRV_PutNextAddress(UART_DRV_eModule_t index)  /* TxWake Mode TI */
{
    uint32_t base = SCI_DRV_u32GetBaseFromModuleIndex((SCI_DRV_eModule_t)index);
    HWREGH(base + SCI_O_CTL1) |= SCI_CTL1_TXWAKE;
}

bool UART_DRV_IsTxEmpty(UART_DRV_eModule_t index)
{
    uint32_t base = SCI_DRV_u32GetBaseFromModuleIndex((SCI_DRV_eModule_t)index);
    return ( ((SCI_getTxFIFOStatus(base) == 0) && ((HWREGH(base + SCI_O_CTL2) & SCI_CTL2_TXEMPTY) == SCI_CTL2_TXEMPTY)) ? true : false);

}

bool UART_DRV_IsRxReady(UART_DRV_eModule_t index)
{
    uint32_t base = SCI_DRV_u32GetBaseFromModuleIndex((SCI_DRV_eModule_t)index);
    return ((SCI_getRxFIFOStatus(base) > 0) ? true : false);
}

uint16_t UART_DRV_GetChar(UART_DRV_eModule_t index)
{
    uint32_t base = SCI_DRV_u32GetBaseFromModuleIndex((SCI_DRV_eModule_t)index);
    return SCI_readCharNonBlocking(base);
}
/* *****************************************************************************
 * UART_DRV_vInit
 * 
 * Description: UART Initialization 
 **************************************************************************** */
void UART_DRV_vInit ( UART_DRV_eModule_t index)
{
    // setup the UART peripheral
    UART_DRV_u32TimeDiffLast[index] = UART_DRV_GET_FREE_RUN_TIMER();

    #if USE_INT_UART_RX
    UART_DRV_RxBufferRDIndex[index] = 0;
    UART_DRV_RxBufferWRIndex[index] = 0;
    UART_DRV_RxBufferWRCount[index]=  0;
    UART_DRV_RxBufferRDCount[index] = 0;
    #endif
}



bool UART_DRV_ReadDataRxAvailable(UART_DRV_eModule_t index)
{
    bool result;
    
    if (UART_DRV_RxBufferRDCount[index] != UART_DRV_RxBufferWRCount[index])
    {
        result = true;
    }
    else   
    {
        result = false;
    }
    return result;    
}

void UART_DRV_TransmitEnableDisable(UART_DRV_eModule_t index, bool pinLevel)
{
    if(index == UART_DRV_1)
    {
        #ifdef SCIDEA_PIN
        GPIO_writePin(SCIDEA_PIN, pinLevel);
        #endif
    }
    else if(index == UART_DRV_2)
    {
        #ifdef SCIDEB_PIN
        GPIO_writePin(SCIDEB_PIN, pinLevel);
        #endif
    }
    else if(index == UART_DRV_3)
    {
        #ifdef SCIDEC_PIN
        GPIO_writePin(SCIDEC_PIN, pinLevel);
        #endif
    }
    else if(index == UART_DRV_4)
    {
        #ifdef SCIDED_PIN
        GPIO_writePin(SCIDED_PIN, pinLevel);
        #endif
    }

}


uint32_t UART_DRV_ReadDataRxTimeDiff(UART_DRV_eModule_t index)
{

    return UART_DRV_u32TimeDiffRead[index];
}

uint16_t UART_DRV_ReadDataRx(UART_DRV_eModule_t index)
{
    uint16_t result;
    
    if (UART_DRV_RxBufferRDCount[index] != UART_DRV_RxBufferWRCount[index])
    {
        UART_DRV_RxBufferRDCount[index]++;
        UART_DRV_u32TimeDiffRead[index] = UART_DRV_RxBufferTimeDiff[index][UART_DRV_RxBufferRDIndex[index]];
        result = UART_DRV_RxBuffer[index][UART_DRV_RxBufferRDIndex[index]++];
        if (UART_DRV_RxBufferRDIndex[index] >= UART_DRV_RX_BUFFER_SIZE)
        {
            UART_DRV_RxBufferRDIndex[index] = 0;
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
void UART_DRV_RXInterrupt(UART_DRV_eModule_t index)
{
    //could use here that Tx Interrupt is disabled
    FPGA_SCI_DRV_vProcessFastTD(index);
    
    if ((UART_DRV_RxBufferWRCount[index] - UART_DRV_RxBufferRDCount[index]) <= UART_DRV_RX_BUFFER_SIZE)
    {
        UART_DRV_u32TimeDiffNow[index] = UART_DRV_GET_FREE_RUN_TIMER();
        UART_DRV_RxBufferTimeDiff[index][UART_DRV_RxBufferWRIndex[index]] = UART_DRV_u32TimeDiffNow[index] - UART_DRV_u32TimeDiffLast[index];
#if UART_DRV_GET_FREE_RUN_TIMER_DECREMENT_COUNTER
        UART_DRV_RxBufferTimeDiff[index][UART_DRV_RxBufferWRIndex[index]] = 0 - UART_DRV_RxBufferTimeDiff[index][UART_DRV_RxBufferWRIndex[index]];
#endif
        UART_DRV_u32TimeDiffLast[index] = UART_DRV_u32TimeDiffNow[index];
        UART_DRV_RxBuffer[index][UART_DRV_RxBufferWRIndex[index]++] = UART_DRV_GetChar(index);
        UART_DRV_RxBufferWRCount[index]++;
        if (UART_DRV_RxBufferWRIndex[index] >= UART_DRV_RX_BUFFER_SIZE)
        {
            UART_DRV_RxBufferWRIndex[index] = 0;
        } 
    }
    else
    {
        UART_DRV_GetChar(index);
        UART_DRV_RXIntOverflowCounter[index]++;
    }
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
void UART_DRV_TXInterrupt(UART_DRV_eModule_t index)
{
    FPGA_SCI_DRV_vProcessUartTx(index);
}





