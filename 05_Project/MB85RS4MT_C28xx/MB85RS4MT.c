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
#define FIFO_WORDSIZE       (8)
//#define WAIT_SPI_TIMEOUT         10000
#define WAIT_SPI_TIMEOUT         0xFFFFFFFE



/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */
//SPI related constants
#define SPI_TIMEOUT_ERROR   777
#define SPI_OK              0

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
    uint16_t wr_en;        // 1 - write enabled
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

#if MB85RS4MT_USE_RAM_BUFFER
uint16_t mb85rs4mt_ram_buffer[MB85RS4MT_RAM_BUFFER_SIZE];
uint16_t mb85rs4mt_ram_copy_len;
uint32_t mb85rs4mt_ram_copy_address;
uint16_t mb85rs4mt_ram_data_offset;
uint32_t mb85rs4mt_ram_buffer_start_address = MB85RS4MT_RAM_BUFFER_START_ADDRESS;
#endif

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */
interrupt void MB85RS4MT_RXFIFO_ISR(void);
int MB85RS4MT_ReadDataInternal(uint32_t address, uint16_t *buf, uint16_t len);

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


    GpioCtrlRegs.GPCQSEL1.bit.GPIO69 = 0;       //Async, no Sync or Qualification
    GpioCtrlRegs.GPCQSEL1.bit.GPIO70 = 0;       //Async, no Sync or Qualification
    GpioCtrlRegs.GPCQSEL1.bit.GPIO71 = 0;       //Async, no Sync or Qualification
    GpioCtrlRegs.GPCQSEL1.bit.GPIO72 = 0;       //Async, no Sync or Qualification

    EDIS;


    SpicRegs.SPICCR.bit.SPISWRESET = 0;         // SPI in reset


    SpicRegs.SPIFFTX.all = 0xC000;              // Enable TX FIFOs
                                                // TX FIFO in RESET
                                                // Disable TX FIFO interrupt
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

    SpicRegs.SPICCR.bit.HS_MODE = 0x1;
    SpicRegs.SPIBRR.bit.SPI_BIT_RATE = MB85RS4MT_SPI_BAUDRATE; // Set the baud rate


    //SpicRegs.SPIPRI.bit.FREE = 1;               // Debugger doesn't halt the SPI

    SpicRegs.SPICCR.bit.SPISWRESET = 1;         // Release the SPI from reset

    SpicRegs.SPIFFTX.bit.TXFIFO=1;              // Release the TX FIFO

    SpicRegs.SPIFFRX.bit.RXFIFORESET=1;         // Release the RX FIFO

    mb85rs4mt_spi.complete = 1;                   // Previous command is complete

    //interrupts register
    EALLOW;
    PieVectTable.SPIC_RX_INT = &MB85RS4MT_RXFIFO_ISR;   // SPIC_RX FIFO ISR
    EDIS;
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;                  // Enable the PIE block
    PieCtrlRegs.PIEIER6.bit.INTx9 = 1;                  // Enable PIE Group 6, INT 9 => SPIC_RX
    IER=M_INT6;                                         // Enable CPU INT6


    return 0;
}


/*
 * Initialization after enabled interrupts
 */
int MB85RS4MT_Start(void)
{
    MB85RS4MT_WriteEnable();

    #if MB85RS4MT_USE_RAM_BUFFER
    MB85RS4MT_ReadDataInternal(mb85rs4mt_ram_buffer_start_address, mb85rs4mt_ram_buffer, MB85RS4MT_RAM_BUFFER_SIZE);
    #endif

    return SPI_OK;
}

int wait_idle_fail(void)
{
#if WAIT_SPI_TIMEOUT == 0xFFFFFFFF
    while(mb85rs4mt_spi.complete == 0){} //Wait for previous command completion
    return SPI_OK;
#else
    uint32_t i=0;
    while(!mb85rs4mt_spi.complete && (i<WAIT_SPI_TIMEOUT)) i++;//Wait for previous command completion
    if(i >= WAIT_SPI_TIMEOUT)
        return SPI_TIMEOUT_ERROR;
    else
        return SPI_OK;
#endif
}



/*
 * Sends write enable command
 */
int MB85RS4MT_WriteEnable(void)
{

    if(wait_idle_fail()) return SPI_TIMEOUT_ERROR;

    ASSERT(SpicRegs.SPIFFRX.bit.RXFFST == 0);           // At this point the RX FIFO should be empty

    mb85rs4mt_spi.complete = 0;                         // Command just started

    GpioDataRegs.GPCDAT.bit.GPIO72 = 0;                 // Assert CS

    mb85rs4mt_spi.opcode = OPCODE_WREN;                 // Set the opcode

    SpicRegs.SPIFFRX.bit.RXFFIL = 1;                    // We will send/receive one byte

    SpicRegs.SPITXBUF = OPCODE_WREN<<8;                 // Send the opcode

    mb85rs4mt_spi.wr_en = 1;

    return SPI_OK;
}

/*
 * Sends write disable command
 */
int MB85RS4MT_WriteDisable(void)
{
    if(wait_idle_fail()) return SPI_TIMEOUT_ERROR;

    ASSERT(SpicRegs.SPIFFRX.bit.RXFFST == 0);           // At this point the RX FIFO should be empty

    mb85rs4mt_spi.complete = 0;                         // Command just started

    GpioDataRegs.GPCDAT.bit.GPIO72 = 0;                 // Assert CS

    mb85rs4mt_spi.opcode = OPCODE_WRDI;                 // Set the opcode

    SpicRegs.SPIFFRX.bit.RXFFIL = 1;                    // We will send/receive one byte

    SpicRegs.SPITXBUF = OPCODE_WRDI<<8;                 // Send the opcode

    mb85rs4mt_spi.wr_en = 0;

    return SPI_OK;
}

uint32_t address_spi = 0;

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
int MB85RS4MT_WriteData(uint32_t address, uint16_t *data, uint16_t len)
{
    if(wait_idle_fail()) return SPI_TIMEOUT_ERROR;


    #if MB85RS4MT_USE_RAM_BUFFER
    mb85rs4mt_ram_copy_len = 0;
    {
        if(address >= mb85rs4mt_ram_buffer_start_address)   /* copy from beginning */
        {
            if (address < (mb85rs4mt_ram_buffer_start_address + MB85RS4MT_RAM_BUFFER_SIZE))
            {
                mb85rs4mt_ram_copy_address = address - mb85rs4mt_ram_buffer_start_address;
                mb85rs4mt_ram_copy_len = len;
                if ((mb85rs4mt_ram_copy_address + mb85rs4mt_ram_copy_len) > MB85RS4MT_RAM_BUFFER_SIZE)
                {
                    mb85rs4mt_ram_copy_len = MB85RS4MT_RAM_BUFFER_SIZE - mb85rs4mt_ram_copy_address;
                }
                mb85rs4mt_ram_data_offset = 0;
            }
        }
        else //if(address < mb85rs4mt_ram_buffer_start_address)  /* copy at end */
        {
            if ((address + len) > mb85rs4mt_ram_buffer_start_address)
            {
                mb85rs4mt_ram_copy_address = 0;
                mb85rs4mt_ram_data_offset = mb85rs4mt_ram_buffer_start_address - address;
                mb85rs4mt_ram_copy_len = len - mb85rs4mt_ram_data_offset;
                if ((mb85rs4mt_ram_copy_address + mb85rs4mt_ram_copy_len) > MB85RS4MT_RAM_BUFFER_SIZE)
                {
                    mb85rs4mt_ram_copy_len = MB85RS4MT_RAM_BUFFER_SIZE - mb85rs4mt_ram_copy_address;
                }
            }
        }
        if (mb85rs4mt_ram_copy_len)
        {
            memcpy(&mb85rs4mt_ram_buffer[mb85rs4mt_ram_copy_address], &data[mb85rs4mt_ram_data_offset], mb85rs4mt_ram_copy_len);
        }
    }
    #endif

    address_spi = address << 1;

    ASSERT(SpicRegs.SPIFFRX.bit.RXFFST == 0);                           // At this point the RX FIFO should be empty

    mb85rs4mt_spi.complete = 0;                                         // Command just started

    GpioDataRegs.GPCDAT.bit.GPIO72 = 0;                                 // Assert CS

    mb85rs4mt_spi.opcode = OPCODE_WRITE;                                // Set the opcode

    mb85rs4mt_spi.buf = data;                                           // Data to be sent
    mb85rs4mt_spi.words_left = len;                                     // Words to be send

    SpicRegs.SPIFFRX.bit.RXFFIL = 4;                                    // We will send/receive 4 bytes = opcode
                                                                        // and 3 bytes for the address
    SpicRegs.SPITXBUF = OPCODE_WRITE<<8;

    SpicRegs.SPITXBUF = ((uint16_t)(address_spi>>8))&0xFF00;               // Address is 3 bytes,
                                                                        // the chip ignores the first byte
    SpicRegs.SPITXBUF = ((uint16_t)address_spi)&0xFF00;                     // Address MSB
    SpicRegs.SPITXBUF = ((uint16_t)address_spi)<<8;                         // Address LSB

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
int MB85RS4MT_ReadDataInternal(uint32_t address, uint16_t *buf, uint16_t len)
{
    if(wait_idle_fail()) return SPI_TIMEOUT_ERROR;

    #if MB85RS4MT_USE_RAM_BUFFER
    mb85rs4mt_ram_copy_len = 0;
    {
        if(address >= mb85rs4mt_ram_buffer_start_address)   /* copy from beginning */
        {
            if (address < (mb85rs4mt_ram_buffer_start_address + MB85RS4MT_RAM_BUFFER_SIZE))
            {
                mb85rs4mt_ram_copy_address = address - mb85rs4mt_ram_buffer_start_address;
                mb85rs4mt_ram_copy_len = len;
                if ((mb85rs4mt_ram_copy_address + mb85rs4mt_ram_copy_len) > MB85RS4MT_RAM_BUFFER_SIZE)
                {
                    mb85rs4mt_ram_copy_len = MB85RS4MT_RAM_BUFFER_SIZE - mb85rs4mt_ram_copy_address;
                }
                mb85rs4mt_ram_data_offset = 0;
            }
        }
        else //if(address < mb85rs4mt_ram_buffer_start_address)  /* copy at end */
        {
            if ((address + len) > mb85rs4mt_ram_buffer_start_address)
            {
                mb85rs4mt_ram_copy_address = 0;
                mb85rs4mt_ram_data_offset = mb85rs4mt_ram_buffer_start_address - address;
                mb85rs4mt_ram_copy_len = len - mb85rs4mt_ram_data_offset;
                if ((mb85rs4mt_ram_copy_address + mb85rs4mt_ram_copy_len) > MB85RS4MT_RAM_BUFFER_SIZE)
                {
                    mb85rs4mt_ram_copy_len = MB85RS4MT_RAM_BUFFER_SIZE - mb85rs4mt_ram_copy_address;
                }
            }
        }
    }
    #endif

    address_spi = address << 1;

    ASSERT(SpicRegs.SPIFFRX.bit.RXFFST == 0);                             // At this point the RX FIFO should be empty

    mb85rs4mt_spi.complete = 0;                                           // Command just started

    GpioDataRegs.GPCDAT.bit.GPIO72 = 0;                                   // Assert CS

    mb85rs4mt_spi.opcode = OPCODE_READ;                                   // Set the opcode

    mb85rs4mt_spi.buf = buf;                                              // Data to be sent
    mb85rs4mt_spi.words_left = len;                                       // Words to be send

    SpicRegs.SPIFFRX.bit.RXFFIL = 4;                                      // We will send/receive 4 bytes = opcode
                                                                          // and 3 bytes for the address
    SpicRegs.SPITXBUF = OPCODE_READ<<8;

    SpicRegs.SPITXBUF = ((uint16_t)(address_spi>>8))&0xFF00;                 // Address is 3 bytes,
                                                                          // the chip ignores the first byte
    SpicRegs.SPITXBUF = ((uint16_t)address_spi)&0xFF00;                       // Address MSB
    SpicRegs.SPITXBUF = ((uint16_t)address_spi)<<8;                           // Address LSB

    return SPI_OK;
}

#if MB85RS4MT_USE_RAM_BUFFER

uint16_t *buf_spi = 0;
uint32_t adr_spi = 0;
uint16_t len_spi = 0;

int MB85RS4MT_ReadData(uint32_t address, uint16_t *buf, uint16_t len)
{
    int result = SPI_OK;

    buf_spi = buf;
    adr_spi = address;
    len_spi = len;

    //mb85rs4mt_ram_copy_len = 0;
    {
        if(address < mb85rs4mt_ram_buffer_start_address)  /* address before ram buffer */
        {
            if (len_spi > (mb85rs4mt_ram_buffer_start_address - address))
            {
                len_spi = (mb85rs4mt_ram_buffer_start_address - address);
            }
            result = MB85RS4MT_ReadDataInternal(adr_spi, buf_spi, len_spi);

            if(wait_idle_fail()) return SPI_TIMEOUT_ERROR;

            len -= len_spi;
            address += len_spi;
            buf += len_spi;


            buf_spi = buf;
            adr_spi = address;
            len_spi = len;

        }
        mb85rs4mt_ram_copy_len = 0;
        if(address >= mb85rs4mt_ram_buffer_start_address)   /* copy from beginning */
        {
            if (address < (mb85rs4mt_ram_buffer_start_address + MB85RS4MT_RAM_BUFFER_SIZE))
            {
                mb85rs4mt_ram_copy_address = address - mb85rs4mt_ram_buffer_start_address;
                mb85rs4mt_ram_copy_len = len;
                if ((mb85rs4mt_ram_copy_address + mb85rs4mt_ram_copy_len) > MB85RS4MT_RAM_BUFFER_SIZE)
                {
                    mb85rs4mt_ram_copy_len = MB85RS4MT_RAM_BUFFER_SIZE - mb85rs4mt_ram_copy_address;
                }
                mb85rs4mt_ram_data_offset = 0;
            }
            adr_spi += mb85rs4mt_ram_copy_len;
            buf_spi += mb85rs4mt_ram_copy_len;
        }
        else //if(address < mb85rs4mt_ram_buffer_start_address)  /* copy at end */
        {
            if ((address + len) > mb85rs4mt_ram_buffer_start_address)
            {
                mb85rs4mt_ram_copy_address = 0;
                mb85rs4mt_ram_data_offset = mb85rs4mt_ram_buffer_start_address - address;
                mb85rs4mt_ram_copy_len = len - mb85rs4mt_ram_data_offset;
                if ((mb85rs4mt_ram_copy_address + mb85rs4mt_ram_copy_len) > MB85RS4MT_RAM_BUFFER_SIZE)
                {
                    mb85rs4mt_ram_copy_len = MB85RS4MT_RAM_BUFFER_SIZE - mb85rs4mt_ram_copy_address;
                }
            }
        }
        if (mb85rs4mt_ram_copy_len)
        {
            memcpy(&buf[mb85rs4mt_ram_data_offset], &mb85rs4mt_ram_buffer[mb85rs4mt_ram_copy_address], mb85rs4mt_ram_copy_len);
        }
    }

    len_spi -= mb85rs4mt_ram_copy_len;

    if(len_spi)
    {
        result = MB85RS4MT_ReadDataInternal(adr_spi, buf_spi, len_spi);
    }
    return result;
}
int MB85RS4MT_ReadDataRaw(uint32_t address, uint16_t *buf, uint16_t len)
{
    return MB85RS4MT_ReadDataInternal(address, buf, len);
}

#else
int MB85RS4MT_ReadData(uint32_t address, uint16_t *buf, uint16_t len)
{
    return MB85RS4MT_ReadDataInternal(address, buf, len);
}
int MB85RS4MT_ReadDataRaw(uint32_t address, uint16_t *buf, uint16_t len)
{
    return MB85RS4MT_ReadDataInternal(address, buf, len);
}
#endif

int MB85RS4MT_IsBusy(void)
{
    return !mb85rs4mt_spi.complete;
}

/*
 * Writes the status register
 */
int MB85RS4MT_WriteStatReg(uint16_t val)
{
    if (mb85rs4mt_spi.wr_en == 0)
    {
        (void)MB85RS4MT_WriteEnable();
    }

    if(wait_idle_fail()) return SPI_TIMEOUT_ERROR;

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
    volatile uint16_t dummy;

    if(wait_idle_fail()) goto timeout;

    ASSERT(SpicRegs.SPIFFRX.bit.RXFFST == 0);                         // At this point the RX FIFO should be empty

    mb85rs4mt_spi.complete = 0;                                       // Command just started

    GpioDataRegs.GPCDAT.bit.GPIO72 = 0;                               // Assert CS

    mb85rs4mt_spi.opcode = OPCODE_RDSR;                               // Set the opcode

    SpicRegs.SPIFFRX.bit.RXFFIL = 2;                                  // We will send/receive two bytes

    SpicRegs.SPITXBUF = OPCODE_RDSR<<8;                               // Send the opcode
    SpicRegs.SPITXBUF = 0x0000;                                       // Dummy transfer to initiate read

    if(wait_idle_fail()) goto timeout;

    *val = SpicRegs.SPIRXBUF&0xFF;                                    // The second byte in the FIFO is the STATREG value
                                                                      // Previous byte was read in the ISR

    GpioDataRegs.GPCDAT.bit.GPIO72 = 1;                               // Release CS

    return SPI_OK;

timeout:

    return SPI_TIMEOUT_ERROR;
}


uint16_t idx, read_has_started, write_has_started;
//
// RX FIFO ISR
//
__interrupt void MB85RS4MT_RXFIFO_ISR(void){
    uint16_t i, words_in_batch;

    volatile uint16_t dummy;

    switch(mb85rs4mt_spi.opcode) {

        case OPCODE_WREN:
        case OPCODE_WRDI:
        case OPCODE_WRSR:

            GpioDataRegs.GPCDAT.bit.GPIO72 = 1;                     // Release CS

            while(SpicRegs.SPIFFRX.bit.RXFFST > 0)                  // Read all RX FIFO
                dummy = SpicRegs.SPIRXBUF;

            mb85rs4mt_spi.complete = 1;                             // Those commands complete on the first interrupt

            break;

        case OPCODE_RDSR:
            GpioDataRegs.GPCDAT.bit.GPIO72 = 1;                     // Release CS
            dummy = SpicRegs.SPIRXBUF;                              // Read the first byte which is not needed,
                                                                    // so the ISR doesn't fire again
            mb85rs4mt_spi.complete = 1;                             // RDST completes on the first interrupt
            break;

        case OPCODE_WRITE:
            if(!write_has_started){                                 // Zero the 'idx' at the start of the transaction
                idx = 0;
                write_has_started = 1;

            }

            if (mb85rs4mt_spi.words_left == 0){
                GpioDataRegs.GPCDAT.bit.GPIO72 = 1;                 // Release CS
                mb85rs4mt_spi.complete = 1;                         // Command completes now
                idx = 0;
            } else {
                                                                    // Words to be send in the next batch
                words_in_batch = (mb85rs4mt_spi.words_left>FIFO_WORDSIZE)?FIFO_WORDSIZE:mb85rs4mt_spi.words_left;

                SpicRegs.SPIFFRX.bit.RXFFIL = 2*(words_in_batch);   // Interrupt after that many bytes

                for(i=0;i<words_in_batch; i++){                     // Put the data in the TX FIFO
                    SpicRegs.SPITXBUF = mb85rs4mt_spi.buf[idx];     // MSB part of SPITXBUF is used
                    SpicRegs.SPITXBUF = mb85rs4mt_spi.buf[idx]<<8;
                    idx++;
                }

                mb85rs4mt_spi.words_left -= words_in_batch;         // Update the remaining data
            }

            while(SpicRegs.SPIFFRX.bit.RXFFST > 0)                  // Clean the RX FIFO
                dummy = SpicRegs.SPIRXBUF;

            break;

        case OPCODE_READ:
            if(read_has_started) {                                  // Not the first batch
                 while(SpicRegs.SPIFFRX.bit.RXFFST > 0){            // Read all the data from RX FIFO
                     mb85rs4mt_spi.buf[idx] = SpicRegs.SPIRXBUF<<8; // LSB part of SPIRXBUF is used
                     mb85rs4mt_spi.buf[idx] |= (SpicRegs.SPIRXBUF&0x00FF);
                     idx++;
                 }
             } else {
                 for(i=0;i<4; i++)                                  // Clean the RX FIFO
                     dummy = SpicRegs.SPIRXBUF;
                 read_has_started = 1;
                 idx = 0;
             }

            if (mb85rs4mt_spi.words_left == 0){
                GpioDataRegs.GPCDAT.bit.GPIO72 = 1;                 // Release CS
                read_has_started = 0;                               // Reset the static variables
                idx = 0;

                #if MB85RS4MT_USE_RAM_BUFFER
                if (mb85rs4mt_ram_copy_len)
                {
                    memcpy(&mb85rs4mt_ram_buffer[mb85rs4mt_ram_copy_address], &mb85rs4mt_spi.buf[mb85rs4mt_ram_data_offset], mb85rs4mt_ram_copy_len);
                }
                #endif

                mb85rs4mt_spi.complete = 1;                         // Command completes now


            }
            else
            {
                                                                    // Words to be read in the next batch
                words_in_batch = (mb85rs4mt_spi.words_left>FIFO_WORDSIZE)?FIFO_WORDSIZE:mb85rs4mt_spi.words_left;

                SpicRegs.SPIFFRX.bit.RXFFIL = 2*words_in_batch;     // Interrupt after that many bytes

                for(i=0;i<2*words_in_batch; i++)                    // Fill the TX FIFO with dummy data to initiate next batch
                    SpicRegs.SPITXBUF = 0x0000;                     // dummy data

                mb85rs4mt_spi.words_left -= words_in_batch;         // Update the remaining data
            }

            break;

        default:
            break;
    }

    SpicRegs.SPIFFRX.bit.RXFFINTCLR=1;           // Clear Interrupt flag
    PieCtrlRegs.PIEACK.all|=M_INT6;              // Issue PIE ACK

}


