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

#define PROJECTCFG_FCY_HZ          			60000000.0 	/* Instruction cycle frequency (Hz) */

#define PROJECTCFG_OSC_HZ          			 8000000.0

#define PROJECTCFG_UART1_BAUD      		    	9600

#define PROJECTCFG_HMDRV_SCI_BAUD  			PROJECTCFG_UART1_BAUD

#define PROJECTCFG_UART_PARITY_AND_DATA  	UART_PARITY_NONE_DATA_8_BITS // UART_PARITY_EVEN_DATA_8_BITS // 


#define PROJECTCFG_SPI_RAM_HZ      			40000000.0
#define PROJECTCFG_SPI_EEPROM_HZ    		 5000000.0
#define PROJECTCFG_SPI_FLIR_HZ     			20000000.0


#define PROJECTCFG_I2C_DEFAULT_MODULE     	0       /* 0 - I2C1; 1 - I2C2 */


/* *****************************************************************************
 * Configuration Definitions Enable Disable
 **************************************************************************** */

#define USE_UART1_DE            1
#define USE_UART1_RE            1
#define USE_UART2_DE            0

#define USE_I2C1                1
#define USE_I2C2                1

#define USE_SPI_RAM             0
#define USE_SPI_EEPROM          0
#define USE_SPI_FLIR           	1

#define USE_TSC_DO_DI_ADC		0


 
#endif	/* PROJECTCFG_H */
