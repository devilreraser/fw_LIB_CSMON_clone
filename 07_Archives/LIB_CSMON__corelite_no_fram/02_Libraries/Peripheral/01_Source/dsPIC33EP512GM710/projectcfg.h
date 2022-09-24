/* *****************************************************************************
 * File:   projectcfg.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 04 09 13:13
 * 
 * Description: Project Configuration
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef PROJECTCFG_H
#define	PROJECTCFG_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

#define PROJECTCFG_RW_NON_VOLATILE_MEMORY_SIZE_EEPROM_KB    0

#define PROJECTCFG_FCY_HZ                       60000000.0 	/* Instruction cycle frequency (Hz) */
//#define PROJECTCFG_FCY_HZ          			57000000.0 	/* Instruction cycle frequency (Hz) (PMP CS-1021 Board Only no errors - with Active Low PMALL PMALH) */
//#define PROJECTCFG_FCY_HZ          			56000000.0 	/* Instruction cycle frequency (Hz) (PMP CS-1021 Board Only no errors - with Active Low PMALL PMALH Board In Work) */
//#define PROJECTCFG_FCY_HZ          			54000000.0 	/* Instruction cycle frequency (Hz) (PMP CS-1021 Board Only no errors) */
//#define PROJECTCFG_FCY_HZ          			70000000.0 	/* Instruction cycle frequency (Hz) */

#define PROJECTCFG_OSC_HZ                       8000000.0


#define PROJECTCFG_HMDRV_HEXMON_BAUD            460800
#define PROJECTCFG_HMDRV_MODBUS_BAUD            460800

#define PROJECTCFG_USE_DYNAMIC_MEMORY           1       /* 1 - use heap    or    0 - owm buffer */

#define PROJECTCFG_USE_HMDRV_TX_DELAY           0
#define PROJECTCFG_USE_HMDRV_MODBUS_TX_DELAY    1
#define PROJECTCFG_STOP_BITS                    UART_ONE_STOP_BIT // UART_TWO_STOP_BITS  // 


#define PROJECTCFG_HMLIB_HEXMON_UART_MODULE     2   /* UART_3 */
#define PROJECTCFG_HMLIB_MODBUS_UART_MODULE     2   /* UART_3 */


#if PROJECTCFG_HMLIB_HEXMON_UART_MODULE == PROJECTCFG_HMLIB_MODBUS_UART_MODULE
#define PROJECTCFG_UART_PARITY_AND_DATA_HEXMON  UART_PARITY_NONE_DATA_8_BITS
#define PROJECTCFG_UART_PARITY_AND_DATA_MODBUS  UART_PARITY_NONE_DATA_8_BITS
#else
#define PROJECTCFG_UART_PARITY_AND_DATA_HEXMON  UART_PARITY_NONE_DATA_8_BITS
#define PROJECTCFG_UART_PARITY_AND_DATA_MODBUS  UART_PARITY_EVEN_DATA_8_BITS
#endif


     			
#define PROJECTCFG_SPI_RAM_HZ      			25000000.0 /* Limitation Datasheet 25000000.0 */
#define PROJECTCFG_SPI_EEPROM_HZ    		 5000000.0
#define PROJECTCFG_SPI_FLIR_HZ     			20000000.0

#define PROJECTCFG_SPI_DAC_HZ     			 1000000.0
#define PROJECTCFG_SPI_FPGA_HZ_NO_MEM        9000000.0  /* Limitation Datasheet  9000000.0 */


#define PROJECTCFG_I2C_DEFAULT_MODULE     	1       /* 0 - I2C1; 1 - I2C2 */

//#define PROJECTCFG_SPI_DEFAULT_MODE         3       /* bit0-CPOL bit1-CPHA */
#define PROJECTCFG_SPI_DEFAULT_MODE         0       /* bit0-CPOL bit1-CPHA needed for memories with new queue implementation or for 512GM710. Most probable queue */

#define PROJECTCFG_TIMER_MAIN_USEC          50.0


/* *****************************************************************************
 * Configuration Definitions Enable Disable
 **************************************************************************** */
#define USE_CONTROL             1

#define USE_CTREADER            0

#define USE_CSMON               1

#define USE_UART1_DE            1

#define USE_UART1_RE            0       /* disable if 485 not used */
#define USE_UART1_RE_DISABLED   0       /* disable if 485 not used but uart is used on flir board */

#define USE_UART2_DE            1
#define USE_UART3_DE            1

#define USE_I2C1                1
#define USE_I2C2                1




#define USE_SPI_RAM             1          /* TSC -> RAM   MR25H40 (U304) - Old Net Name RP101_F9 */
#define USE_SPI_EEPROM          1          /* TSC -> EEPROM M95M02 (U303) - Old Net Name RP100_F8 */
#define USE_SPI_FPGA            0
#define USE_SPI_FLIR           	0
#define USE_TSC_DO_DI_ADC       1
#define USE_FLIR_RESET          0
#define USE_FLIR_VSYNC          0

#define USE_SPI_DAC             0
#define USE_I2C_DAC             1

#define USE_SPI_FPGA_NO_MEM     1           /* use different form SPIMEM spi for fpga */


#define USE_UART1_RX_PIN        1
#define USE_INPUT_CAPTURE       0


#endif	/* PROJECTCFG_H */
