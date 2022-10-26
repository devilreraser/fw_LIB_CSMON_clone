/*
 * MB85RS4MT.h
 *
 *  MB85RS4MT serial MRAM
 *
 *  Created on: Nov 21, 2020
 *      Author: dpenev
 */

#ifndef MB85RS4MT_old_H_
#define MB85RS4MT_old_H_
#include <stdint.h>

// MB85RS4MT opcodes.
#define OPCODE_WREN     0x06    /* Write enable */
#define OPCODE_WRDI     0x04    /* Write disable */
#define OPCODE_RDSR     0x05    /* Read status register */
#define OPCODE_WRSR     0x01    /* Write status register */
#define OPCODE_READ     0x03    /* Read data bytes */
#define OPCODE_WRITE    0x02    /* Write data bytes */
#define OPCODE_SLEEP    0xB9    /* Enter Sleep Mode */
#define OPCODE_WAKE     0xAB    /* Exit Sleep Mode */

//SPI related constants
#define SPI_TIMEOUT           10000
#define SPI_TIMEOUT_ERROR     777
#define SPI_OK                0

#define SPI_BAUDRATE        ((200E6 / 4) / 1E6) - 1

#define FIFO_WORDSIZE       (8)


#define ASSERT(x)           do {} while(!x)

typedef struct {
    uint16_t opcode;       // Last MB85RS4MT command
    uint16_t complete;     // 1 - previous command has complete
    uint16_t words_left;   // bytes still to be transfered
    uint16_t *buf;         // points to the read/write buffer,
                           // both bytes of the word is used.
}mb85rs4mt_spi_tag;


//MB85RS4MT related functions
void InitSPIC_mb85rs4mt(void);
int mb85rs4mt_write_enable();
int mb85rs4mt_busy(void);
int mb85rs4mt_read_statreg(uint16_t *val);
int mb85rs4mt_write_statreg(uint16_t val);
int mb85rs4mt_write_data(uint16_t address, uint16_t *data, uint16_t len);
int mb85rs4mt_read_data(uint16_t address, uint16_t *data, uint16_t len);
interrupt void mb85rs4mt_TXFIFO_ISR(void);
interrupt void mb85rs4mt_RXFIFO_ISR(void);
#endif /* MB85RS4MT_H_ */
