/* *****************************************************************************
 * File:   spi_drv.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 12 02 16:08
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
#ifndef SPI_DRV_H
#define	SPI_DRV_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdint.h>

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

/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void SPI_vInit(uint32_t base);
__interrupt void SPIA_TxFIFOISR(void);
__interrupt void SPIA_RxFIFOISR(void);
__interrupt void SPIB_TxFIFOISR(void);
__interrupt void SPIB_RxFIFOISR(void);
void SPIB_vProcess(void);

#endif	/* SPI_DRV_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
