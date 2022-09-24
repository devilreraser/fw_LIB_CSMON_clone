/* *****************************************************************************
 * File:   fpga_drv.h
 * Author: Dimitar Lilov
 *
 * Created on 2020 01 17 10:17
 * 
 * Description: ...
 * 
 **************************************************************************** */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Guard condition file contents not included more than once */  
/* #pragma once */
#ifndef FPGA_DRV_H
#define	FPGA_DRV_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */




/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define FPGA_UART_MODULE            SCI_C
#define FPGA_UART_BAUD              115200
#define FPGA_UART_PARITY            SCI_CONFIG_PAR_EVEN
//#define FPGA_UART_PARITY            SCI_CONFIG_PAR_NONE
//#define FPGA_UART_STOP_BITS         SCI_CONFIG_STOP_ONE
#define FPGA_UART_STOP_BITS         SCI_CONFIG_STOP_TWO
#define FPGA_UART_ADDRESS_MODE      SCI_CCR_ADDRIDLE_MODE       /* 0 to disable address mode */
//#define FPGA_UART_ADDRESS_MODE      0       /* 0 to disable address mode */

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
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
 
#endif	/* FPGA_DRV_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
