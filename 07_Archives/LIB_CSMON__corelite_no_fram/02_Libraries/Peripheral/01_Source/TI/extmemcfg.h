/* *****************************************************************************
 * File:   extmemcfg.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 01 27 01:14
 * 
 * Description: External Memory Configuration
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef EXTMEMCFG_H
#define	EXTMEMCFG_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define EXT_MEM_MIN_VIRTUAL_ADDRESS             0x10000000
#define EXT_MEM_PMP_VIRTUAL_ADDRESS             0x10000000

//#define EXT_MEM_PMP_SIZE                        (0x1C00 + 0x1C00 + 0x0700)  /* 0x3F00 Total */
#define EXT_MEM_PMP_SIZE                        (0x1C00 + 0x1C00 + 0x0800)  /* 0x4000 Total */

#define EXT_MEM_I2C_VIRTUAL_ADDRESS             0x20000000
#define EXT_MEM_I2C_ADDR_OFFSET_RTC_SRAM            0x0000
#define EXT_MEM_I2C_ADDR_OFFSET_RTC_EEPROM          0x0100
#define EXT_MEM_I2C_ADDR_OFFSET_PICAUX              0x1000
#define EXT_MEM_I2C_ADDR_OFFSET_FLIR               0x10000

#define EXT_MEM_FLIR_CCI_TWI_VIRTUAL_ADDRESS    0x30000000

#define EXT_MEM_SPI_MIN_VIRTUAL_ADDRESS         0x40000000
#define EXT_MEM_EEPROM_VIRTUAL_ADDRESS          0x40000000
#define EXT_MEM_RAM_VIRTUAL_ADDRESS             0x80000000
#define EXT_MEM_SPI_FLIR_VIRTUAL_ADDRESS        0x60000000

#endif	/* EXTMEMCFG_H */
