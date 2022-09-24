/* *****************************************************************************
 * File:   mem_drv.h
 * Author: Dimitar Lilov
 *
 * Created on 2020 06 24 07:35
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
#ifndef MEM_DRV_H
#define	MEM_DRV_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdint.h>
#include "boardcfg.h"   
    
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
#if BOARDCFG_USE_MEM_DRV
uint32_t MEM_DRV_u32GetRamAddressReadVirtualMemory(uint32_t u32VirtualAddress, uint16_t u16SizeBytes);
uint32_t MEM_DRV_u32GetRamAddressReserveBuffer(uint32_t u32VirtualAddress, uint16_t u16SizeBytes);
void MEM_DRV_vWriteAddress(uint32_t u32VirtualAddress, uint32_t u32BufferAddress, uint16_t u16SizeBytes);
#else
#define MEM_DRV_u32GetRamAddressReadVirtualMemory(u32VirtualAddress, u16SizeBytes) u32VirtualAddress
#define MEM_DRV_u32GetRamAddressReserveBuffer(u32VirtualAddress, u16SizeBytes) u32VirtualAddress
#define MEM_DRV_vWriteAddress(u32VirtualAddress, u32BufferAddress, u16SizeBytes)  
#endif
    
#endif	/* MEM_DRV_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
