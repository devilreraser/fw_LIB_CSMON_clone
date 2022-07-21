/* *****************************************************************************
 * File:   MB85RS4MT.h
 * Author: Dimitar Lilov
 *
 * Created on 2022 07 20 12:51
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
#ifndef MB85RS4MT_H
#define	MB85RS4MT_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdint.h>
    
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define MB85RS4MT_SPI_BAUDRATE              ((200E6 / 4) / 1E6) - 1

#define MB85RS4MT_USE_RAM_BUFFER            1

#if MB85RS4MT_USE_RAM_BUFFER
#ifndef MB85RS4MT_RAM_BUFFER_SIZE
#define MB85RS4MT_RAM_BUFFER_SIZE           4096
#endif
#ifndef MB85RS4MT_RAM_BUFFER_START_ADDRESS
#define MB85RS4MT_RAM_BUFFER_START_ADDRESS  0
#endif
#endif


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
int MB85RS4MT_Init(void);
int MB85RS4MT_Start(void);
int MB85RS4MT_WriteEnable(void);
int MB85RS4MT_WriteDisable(void);
int MB85RS4MT_WriteData(uint32_t address, uint16_t *data, uint16_t len);
int MB85RS4MT_ReadData(uint32_t address, uint16_t *buf, uint16_t len);
int MB85RS4MT_ReadDataRaw(uint32_t address, uint16_t *buf, uint16_t len);
int MB85RS4MT_IsBusy(void);
int MB85RS4MT_WriteStatReg(uint16_t val);
int MB85RS4MT_ReadStatReg(uint16_t *val);
 
#endif	/* MB85RS4MT_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
