/* *****************************************************************************
 * File:   MB85RS4MT.c
 * Author: Dimitar Lilov
 *
 * Created on 2022 07 20 12:51
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
#include "MB85RS4MT.h"

#include "F2837xD_device.h"
#include "F2837xD_spi.h"


/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
typedef struct  {
    uint16_t opcode;       // Last MB85RS4MT command
    uint16_t complete;     // 1 - previous command has complete
    uint16_t words_left;   // bytes still to be transfered
    uint16_t *buf;         // points to the read/write buffer,
                           // both bytes of the word is used.
} mb85rs4mt_spi_tag;


/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
volatile mb85rs4mt_spi_tag mb85rs4mt_spi;    // Status of the SPI transaction

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */
int MB85RS4MT_Init(void)
{
    EALLOW;

    GpioCtrlRegs.GPCGMUX1.bit.GPIO69 = 3;       // GPIO069 is SPI_SIMOC
    GpioCtrlRegs.GPCMUX1.bit.GPIO69 = 3;


    GpioCtrlRegs.GPCGMUX1.bit.GPIO70 = 3;       // GPIO070 is SPI_SOMIC
    GpioCtrlRegs.GPCMUX1.bit.GPIO70 = 3;

    GpioCtrlRegs.GPCGMUX1.bit.GPIO71 = 3;       // GPIO071 is SPI_CLKC
    GpioCtrlRegs.GPCMUX1.bit.GPIO71 = 3;


    GpioCtrlRegs.GPCGMUX1.bit.GPIO72 = 0;       // GPIO72 is GPIO
    GpioCtrlRegs.GPCDIR.bit.GPIO72 = 1;         // 1=Output,  0=Input
    GpioDataRegs.GPCDAT.bit.GPIO72 = 1;         // Set High initially


    GpioCtrlRegs.GPCQSEL1.bit.GPIO69 = 3;       //Async, no Sync or Qualification
    GpioCtrlRegs.GPCQSEL1.bit.GPIO70 = 3;       //Async, no Sync or Qualification
    GpioCtrlRegs.GPCQSEL1.bit.GPIO71 = 3;       //Async, no Sync or Qualification
    GpioCtrlRegs.GPCQSEL1.bit.GPIO72 = 3;       //Async, no Sync or Qualification

    EDIS;


    SpicRegs.SPICCR.bit.SPISWRESET = 0;         // SPI in reset

    SpicRegs.SPIFFTX.all = 0xC020;              // Enable TX FIFOs
                                                // TX FIFO in RESET
                                                // Enable TX FIFO interrupt
                                                // Interrupt if TX FIFO empty (zero bytes)

    SpicRegs.SPIFFCT.all = 0x0;                 // Send next bite immediately after the previous one

    SpicRegs.SPIFFRX.all = 0x2030;              // Enable RX FIFOs
                                                // RX FIFO in RESET
                                                // RX FIFO in RESET
                                                // Enable RX FIFO interrupt
                                                // Interrupt if FIFO full (16 bytes)


    SpicRegs.SPICCR.bit.CLKPOLARITY = 0;        // Data is output on rising edge
                                                // and input on rising edge.
    SpicRegs.SPICCR.bit.SPICHAR = (8-1);        // 8 bit mode
    SpicRegs.SPICCR.bit.SPILBK = 0;             // No loopback

    SpicRegs.SPICTL.bit.MASTER_SLAVE = 1;       // Master
    SpicRegs.SPICTL.bit.TALK = 1;               // Enable transmission
    SpicRegs.SPICTL.bit.CLK_PHASE = 1;          // SPICLK half-cycle delay
    SpicRegs.SPICTL.bit.SPIINTENA = 1;          // Interrupts disabled, in FIFO mode we enable the interrupt
                                                // using RXFFIENA and TXFFIENA

    SpicRegs.SPIBRR.bit.SPI_BIT_RATE = MB85RS4MT_SPI_BAUDRATE; // Set the baud rate

    SpicRegs.SPIPRI.bit.FREE = 1;               // Debugger doesn't halt the SPI

    SpicRegs.SPICCR.bit.SPISWRESET = 1;         // Release the SPI from reset

    SpicRegs.SPIFFTX.bit.TXFIFO=1;              // Release the TX FIFO

    SpicRegs.SPIFFRX.bit.RXFIFORESET=1;         // Release the RX FIFO

    mb85rs4mt_spi.complete = 1;                   // Previous command is complete

    return 0;
}

int MB85RS4MT_WriteData(uint16_t address, uint16_t *data, uint16_t len)
{

    return 0;
}

int MB85RS4MT_ReadData(uint16_t address, uint16_t *buf, uint16_t len)
{

    return 0;
}


