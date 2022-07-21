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
//SPI related constants
#define SPI_TIMEOUT           10000
#define SPI_TIMEOUT_ERROR     777
#define SPI_OK                0

// MB85RS4MT opcodes.
#define OPCODE_WREN     0x06    /* Write enable */
#define OPCODE_WRDI     0x04    /* Write disable */
#define OPCODE_RDSR     0x05    /* Read status register */
#define OPCODE_WRSR     0x01    /* Write status register */
#define OPCODE_READ     0x03    /* Read data bytes */
#define OPCODE_WRITE    0x02    /* Write data bytes */
#define OPCODE_SLEEP    0xB9    /* Enter Sleep Mode */
#define OPCODE_WAKE     0xAB    /* Exit Sleep Mode */

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
#define ASSERT(x)           do {} while(!x)

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

/*
 * Sends write enable command
 */
int MB85RS4MT_WriteEnable(void)
{
    uint16_t i=0;

    while(!mb85rs4mt_spi.complete && (i<SPI_TIMEOUT)) i++;//Wait for previous command completion
    if(i == SPI_TIMEOUT) return SPI_TIMEOUT_ERROR;

    ASSERT(SpicRegs.SPIFFRX.bit.RXFFST == 0);           // At this point the RX FIFO should be empty

    mb85rs4mt_spi.complete = 0;                         // Command just started

    GpioDataRegs.GPCDAT.bit.GPIO72 = 0;                 // Assert CS

    mb85rs4mt_spi.opcode = OPCODE_WREN;                 // Set the opcode

    SpicRegs.SPIFFRX.bit.RXFFIL = 1;                    // We will send/receive one byte

    SpicRegs.SPITXBUF = OPCODE_WREN<<8;                 // Send the opcode

    return SPI_OK;
}

/*
 * Sends write disable command
 */
int MB85RS4MT_WriteDisable(void)
{
    uint16_t i=0;

    while(!mb85rs4mt_spi.complete && (i<SPI_TIMEOUT)) i++;//Wait for previous command completion
    if(i == SPI_TIMEOUT) return SPI_TIMEOUT_ERROR;

    ASSERT(SpicRegs.SPIFFRX.bit.RXFFST == 0);           // At this point the RX FIFO should be empty

    mb85rs4mt_spi.complete = 0;                         // Command just started

    GpioDataRegs.GPCDAT.bit.GPIO72 = 0;                 // Assert CS

    mb85rs4mt_spi.opcode = OPCODE_WRDI;                 // Set the opcode

    SpicRegs.SPIFFRX.bit.RXFFIL = 1;                    // We will send/receive one byte

    SpicRegs.SPITXBUF = OPCODE_WRDI<<8;                 // Send the opcode

    return SPI_OK;
}


/*
 * Writes data array at given offset
 *
 * address - offset for the data to be written (0 .. 1024)
 * data     - array with data words(16bits) to be written, the most significant byte
 *            of the word is written at the lower memory location.
 * len      - amount of words (16bits) to be written
 *
 * If previous command is not complete in given time frame this function returns SPI_TIMEOUT_ERROR.
 * Nonblocking function, it doesn't wait the whole buffer to be send, returns immediately SPI_OK instead
 */
int MB85RS4MT_WriteData(uint16_t address, uint16_t *data, uint16_t len)
{
    uint16_t i=0;

    while(!mb85rs4mt_spi.complete && (i<SPI_TIMEOUT)) i++;              //Wait for previous command completion
    if(i == SPI_TIMEOUT) return SPI_TIMEOUT_ERROR;

    ASSERT(SpicRegs.SPIFFRX.bit.RXFFST == 0);                           // At this point the RX FIFO should be empty

    mb85rs4mt_spi.complete = 0;                                         // Command just started

    GpioDataRegs.GPCDAT.bit.GPIO72 = 0;                                 // Assert CS

    mb85rs4mt_spi.opcode = OPCODE_WRITE;                                // Set the opcode

    mb85rs4mt_spi.buf = data;                                           // Data to be sent
    mb85rs4mt_spi.words_left = len;                                     // Words to be send

    SpicRegs.SPIFFRX.bit.RXFFIL = 4;                                    // We will send/receive 4 bytes = opcode
                                                                        // and 3 bytes for the address
    SpicRegs.SPITXBUF = OPCODE_WRITE<<8;

    SpicRegs.SPITXBUF = 0x0000;                                         // Address is 3 bytes,
                                                                        // the chip ignores the first byte
    SpicRegs.SPITXBUF = address&0xFF00;                                 // Address MSB
    SpicRegs.SPITXBUF = address<<8;                                     // Address LSB

    return SPI_OK;
}

/*
 * Reads data at given offset
 *
 * address - offset for the data to be written (0 .. 1024)
 * buf     - buffer (with 16bit words) for the data read from the memory.
 *           The buffer should be pre-allocated
 *           The most significant byte of the word is written at the lower memory location.
 * len     - amount of words (16bits) to be read
 *
 * If previous command is not complete in given time frame this function returns SPI_TIMEOUT_ERROR.
 * Nonblocking function, it doesn't wait the whole buffer to be send, returns immediately SPI_OK instead
 */
int MB85RS4MT_ReadData(uint16_t address, uint16_t *buf, uint16_t len)
{
    uint16_t i=0;

    while(!mb85rs4mt_spi.complete && (i<SPI_TIMEOUT)) i++;                //Wait for previous command completion
    if(i == SPI_TIMEOUT) return SPI_TIMEOUT_ERROR;

    ASSERT(SpicRegs.SPIFFRX.bit.RXFFST == 0);                             // At this point the RX FIFO should be empty

    mb85rs4mt_spi.complete = 0;                                           // Command just started

    GpioDataRegs.GPCDAT.bit.GPIO72 = 0;                                   // Assert CS

    mb85rs4mt_spi.opcode = OPCODE_READ;                                   // Set the opcode

    mb85rs4mt_spi.buf = buf;                                              // Data to be sent
    mb85rs4mt_spi.words_left = len;                                       // Words to be send

    SpicRegs.SPIFFRX.bit.RXFFIL = 4;                                      // We will send/receive 4 bytes = opcode
                                                                          // and 3 bytes for the address
    SpicRegs.SPITXBUF = OPCODE_READ<<8;
    SpicRegs.SPITXBUF = 0x0000;                                           // Address is 3 bytes,
                                                                          // the chip ignores the first byte
    SpicRegs.SPITXBUF = address&0xFF00;                                   // Address MSB
    SpicRegs.SPITXBUF = address<<8;                                       // Address LSB

    return SPI_OK;
}

int MB85RS4MT_IsBusy(void)
{
    return !mb85rs4mt_spi.complete;
}

/*
 * Writes the status register
 */
int MB85RS4MT_WriteStatReg(uint16_t val)
{
    uint16_t i=0;

    while(!mb85rs4mt_spi.complete && (i<SPI_TIMEOUT)) i++;//Wait for previous command completion
    if(i == SPI_TIMEOUT) return SPI_TIMEOUT_ERROR;

    ASSERT(SpicRegs.SPIFFRX.bit.RXFFST == 0);           // At this point the RX FIFO should be empty

    mb85rs4mt_spi.complete = 0;                         // Command just started

    GpioDataRegs.GPCDAT.bit.GPIO72 = 0;                 // Assert CS

    mb85rs4mt_spi.opcode = OPCODE_WRSR;                 // Set the opcode

    SpicRegs.SPIFFRX.bit.RXFFIL = 2;                    // We will send/receive two bytes

    SpicRegs.SPITXBUF = OPCODE_WRSR<<8;                 // Send the opcode
    SpicRegs.SPITXBUF = val<<8;                         // Send the value

    return SPI_OK;
}

/*
 * Reads the status register, returning its value in the location val
 * Returns SPI_TIMEOUT_ERROR if error.
 */
int MB85RS4MT_ReadStatReg(uint16_t *val)
{
    uint16_t i=0;
    volatile uint16_t dummy;

    while(!mb85rs4mt_spi.complete && (i<SPI_TIMEOUT)) i++;            //Wait for previous command completion
    if(i == SPI_TIMEOUT) goto timeout;

    ASSERT(SpicRegs.SPIFFRX.bit.RXFFST == 0);                         // At this point the RX FIFO should be empty

    mb85rs4mt_spi.complete = 0;                                       // Command just started

    GpioDataRegs.GPCDAT.bit.GPIO72 = 0;                               // Assert CS

    mb85rs4mt_spi.opcode = OPCODE_RDSR;                               // Set the opcode

    SpicRegs.SPIFFRX.bit.RXFFIL = 2;                                  // We will send/receive two bytes

    SpicRegs.SPITXBUF = OPCODE_RDSR<<8;                               // Send the opcode
    SpicRegs.SPITXBUF = 0x0000;                                       // Dummy transfer to initiate read

    i=0;
    while(!mb85rs4mt_spi.complete && (i<SPI_TIMEOUT)) i++;            //Wait for previous command completion
    if(i == SPI_TIMEOUT) goto timeout;

    *val = SpicRegs.SPIRXBUF&0xFF;                                    // The second byte in the FIFO is the STATREG value
                                                                      // Previous byte was read in the ISR

    GpioDataRegs.GPCDAT.bit.GPIO72 = 1;                               // Release CS

    return SPI_OK;

timeout:

    return SPI_TIMEOUT_ERROR;
}



