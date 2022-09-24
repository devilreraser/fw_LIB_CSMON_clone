/* *****************************************************************************
 * File:   spi_drv.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 12 02 16:08
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
#include "spi_drv.h"
#include "driverlib.h"
#include "device.h"

#include "io_drv.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define SPI_DATA_BUFFER_LEN 1

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */

volatile uint16_t sData[SPI_DATA_BUFFER_LEN];         // Send data buffer

volatile uint16_t sDataPos[SPI_DATA_BUFFER_LEN];         // Send data buffer
volatile uint16_t sDataNeg[SPI_DATA_BUFFER_LEN];         // Send data buffer
volatile uint16_t rData[SPI_DATA_BUFFER_LEN];         // Receive data buffer
volatile uint16_t rDataPoint = 0;   // To keep track of where we are in the
                                    // data stream to check received data
volatile uint16_t SPIB_DataRoll = 1;

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */
//
// Function to configure SPI in FIFO mode.
//
void SPI_vInit(uint32_t base)
{
    //
    // Must put SPI into reset before configuring it
    //
    SPI_disableModule(base);

    //
    // SPI configuration. Use a 1MHz SPICLK and 16-bit word size.
    //
    SPI_setConfig(base, DEVICE_LSPCLK_FREQ, SPI_PROT_POL0PHA0,
                  SPI_MODE_MASTER, 500000, 16);
    SPI_disableLoopback(base);
    SPI_setEmulationMode(base, SPI_EMULATION_FREE_RUN);

    //
    // FIFO and interrupt configuration
    //
    SPI_enableFIFO(base);
    SPI_clearInterruptStatus(base, SPI_INT_TXFF);
    //SPI_setFIFOInterruptLevel(base, SPI_FIFO_TX2, SPI_FIFO_RX2);
    SPI_setFIFOInterruptLevel(base, (SPI_TxFIFOLevel)SPI_DATA_BUFFER_LEN, (SPI_RxFIFOLevel)SPI_DATA_BUFFER_LEN);
    //SPI_enableInterrupt(base, SPI_INT_TXFF);
    //SPI_enableInterrupt(base, SPI_INT_RXFF);

    //
    // Configuration complete. Enable the module.
    //
    SPI_enableModule(base);

    SPIB_DataRoll = 0;
    sDataPos[0] = 0xFFFF;
    sDataNeg[0] = 0xFFFF;
}


uint16_t SPIB_Status = 2;

uint16_t sDataNegOld = 0;
uint16_t sDataPosOld = 0;

void SPIB_vProcess(void)
{
    //
    // Send data
    //
#if 0   /* 28075 */
    switch (SPIB_Status)
    {
    /* neg */
    case 0:
        if (SPI_getTxFIFOStatus(SPIB_BASE) == SPI_FIFO_TXEMPTY)
        {
            GPIO_writePin(CS_NEG_PIN, 0);
            SPI_writeDataNonBlocking(SPIB_BASE, sDataNeg[0]);
            SPIB_Status = 2;
        }
        break;

    /* pos */
    case 1:
        if (SPI_getTxFIFOStatus(SPIB_BASE) == SPI_FIFO_TXEMPTY)
        {
            GPIO_writePin(CS_POS_PIN, 0);
            SPI_writeDataNonBlocking(SPIB_BASE, sDataPos[0]);
            SPIB_Status = 2;
        }
        break;

    /* wait idle */
    case 2:
        if (SPI_getRxFIFOStatus(SPIB_BASE) != SPI_FIFO_RXEMPTY)
        {
            SPI_readDataNonBlocking(SPIB_BASE);
            SPIB_Status = 3;
            GPIO_writePin(CS_NEG_PIN, 1);
            GPIO_writePin(CS_POS_PIN, 1);
        }

        break;

    /* idle */
    default:




        while (SPI_getRxFIFOStatus(SPIB_BASE) != SPI_FIFO_RXEMPTY)
        {
            SPI_readDataNonBlocking(SPIB_BASE);
        }

        if(SPIB_DataRoll > 0)
        {
            sDataPos[0] = sDataPos[0] + 1;
            sDataNeg[0] = sDataNeg[0] + 1;
        }

        if (sDataNeg[0] != sDataNegOld)
        {
            sDataNegOld = sDataNeg[0];
            SPIB_Status = 0;
        }
        else
        if (sDataPos[0] != sDataPosOld)
        {
            sDataPosOld = sDataPos[0];
            SPIB_Status = 1;
        }

        break;
    }


#endif



}


//
// SPI A Transmit FIFO ISR
//
__interrupt void SPIA_TxFIFOISR(void)
{

#if 0   //28075
    uint16_t i;

    //
    // Send data
    //
    for(i = 0; i < SPI_DATA_BUFFER_LEN; i++)
    {
       SPI_writeDataNonBlocking(SPIA_BASE, sData[i]);
    }

    //
    // Increment data for next cycle
    //
    for(i = 0; i < SPI_DATA_BUFFER_LEN; i++)
    {
       sData[i] = sData[i] + 1;
    }

    //
    // Clear interrupt flag and issue ACK
    //
    SPI_clearInterruptStatus(SPIA_BASE, SPI_INT_TXFF);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP6);
#endif
}

//
// SPI A Receive FIFO ISR
//
 __interrupt void SPIA_RxFIFOISR(void)
{
#if 0   //28075
     uint16_t i;

    //
    // Read data
    //
    for(i = 0; i < SPI_DATA_BUFFER_LEN; i++)
    {
        rData[i] = SPI_readDataNonBlocking(SPIA_BASE);
    }

    //
    // Check received data
    //
    for(i = 0; i < SPI_DATA_BUFFER_LEN; i++)
    {
        if(rData[i] != (rDataPoint + i))
        {
            // Something went wrong. rData doesn't contain expected data.
            Example_Fail = 1;
            ESTOP0;
        }
    }

    rDataPoint++;

    //
    // Clear interrupt flag and issue ACK
    //
    SPI_clearInterruptStatus(SPIA_BASE, SPI_INT_RXFF);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP6);

    Example_PassCount++;
#endif
}


uint16_t PositiveSelect = 0;

 //
 // SPI B Transmit FIFO ISR
 //
 __interrupt void SPIB_TxFIFOISR(void)
 {

#if 0   //28075
     uint16_t i;

     GPIO_writePin(CS_NEG_PIN, 1);
     GPIO_writePin(CS_POS_PIN, 1);

     //
     // Send data
     //


     if (PositiveSelect == 0)
     {
         GPIO_writePin(CS_NEG_PIN, 0);
         for(i = 0; i < SPI_DATA_BUFFER_LEN; i++)
         {
            SPI_writeDataNonBlocking(SPIB_BASE, sDataNeg[i]);
         }
     }
     else
     {
         GPIO_writePin(CS_POS_PIN, 0);
         for(i = 0; i < SPI_DATA_BUFFER_LEN; i++)
         {
            SPI_writeDataNonBlocking(SPIB_BASE, sDataPos[i]);
         }
     }

     PositiveSelect++;

     if (PositiveSelect >= 2)
     {
         PositiveSelect = 0;


         //
         // Increment data for next cycle
         //
         if(SPIB_DataRoll > 0)
         {
             for(i = 0; i < SPI_DATA_BUFFER_LEN; i++)
             {
                 sDataPos[i] = sDataPos[i] + 1;
                 sDataNeg[i] = sDataNeg[i] + 1;
             }
         }



     }


     //
     // Clear interrupt flag and issue ACK
     //
     SPI_clearInterruptStatus(SPIB_BASE, SPI_INT_TXFF);
     Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP6);
#endif
 }

 //
 // SPI B Receive FIFO ISR
 //
  __interrupt void SPIB_RxFIFOISR(void)
 {
#if 0   //28075
      uint16_t i;

     //
     // Read data
     //
     for(i = 0; i < SPI_DATA_BUFFER_LEN; i++)
     {
         rData[i] = SPI_readDataNonBlocking(SPIB_BASE);
     }

     //
     // Check received data
     //
     for(i = 0; i < SPI_DATA_BUFFER_LEN; i++)
     {
         if(rData[i] != (rDataPoint + i))
         {
             // Something went wrong. rData doesn't contain expected data.
             Example_Fail = 1;
             //ESTOP0;
         }
     }

     rDataPoint++;

     //
     // Clear interrupt flag and issue ACK
     //
     SPI_clearInterruptStatus(SPIB_BASE, SPI_INT_RXFF);
     Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP6);

     Example_PassCount++;
#endif
 }







