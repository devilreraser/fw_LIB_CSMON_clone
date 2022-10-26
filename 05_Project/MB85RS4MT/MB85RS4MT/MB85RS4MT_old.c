/*
 * mb85rs4mt.c
 *
 *  MB85RS4MT serial MRAM - support functions
 *
 *  Created on: Nov 21, 2020
 *      Author: dpenev
 */

#include <MB85RS4MT/MB85RS4MT_old.h>
#include "F2837xD_device.h"
#include "F2837xD_spi.h"

#ifndef _USE_LIBRARY
volatile mb85rs4mt_spi_tag mb85rs4mt_spi;    // Status of the SPI transaction
#else
extern volatile mb85rs4mt_spi_tag mb85rs4mt_spi;    // Status of the SPI transaction
#endif

/*
 * InitSPIA - This function initializes the SPIA for use with MB85RS4MT
 */
void InitSPIC_mb85rs4mt(void)
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

    SpicRegs.SPIBRR.bit.SPI_BIT_RATE = SPI_BAUDRATE; // Set the baud rate

    SpicRegs.SPIPRI.bit.FREE = 1;               // Debugger doesn't halt the SPI

    SpicRegs.SPICCR.bit.SPISWRESET = 1;         // Release the SPI from reset

    SpicRegs.SPIFFTX.bit.TXFIFO=1;              // Release the TX FIFO

    SpicRegs.SPIFFRX.bit.RXFIFORESET=1;         // Release the RX FIFO

    mb85rs4mt_spi.complete = 1;                   // Previous command is complete
}


/*
 * Sends write enable command
 */
int mb85rs4mt_write_enable(void)
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
int mb85rs4mt_write_disable(void)
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

int mb85rs4mt_busy(void)
{
    return !mb85rs4mt_spi.complete;
}

/*
 * Writes the status register
 */
int mb85rs4mt_write_statreg(uint16_t val)
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
int mb85rs4mt_read_statreg(uint16_t *val)
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
int mb85rs4mt_write_data(uint16_t address, uint16_t *data, uint16_t len)
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
int mb85rs4mt_read_data(uint16_t address, uint16_t *buf, uint16_t len)
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


//
// RX FIFO ISR
//
__interrupt void mb85rs4mt_RXFIFO_ISR(void){
    uint16_t i, words_in_batch;
    static uint16_t idx, read_has_started, write_has_started;
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
                mb85rs4mt_spi.complete = 1;                         // Command completes now
            } else {
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

