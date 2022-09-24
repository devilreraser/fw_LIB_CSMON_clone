/* *****************************************************************************
 * File:   mem_drv.c
 * Author: Dimitar Lilov
 *
 * Created on 2020 06 24 07:35
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
#include "mem_drv.h"
#include "spimem.h"

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
#if BOARDCFG_USE_MEM_DRV

typedef struct 
{
    uint32_t u32VirtualAddress;
    uint32_t u32BufferAddress;
    uint32_t u32GlobalQueueID;
    
}sMemoryElement_t;
/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
sMemoryElement_t sMemoryElement[];

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */

uint32_t MEM_DRV_u32GetRamAddressReadVirtualMemory(uint32_t u32VirtualAddress, uint16_t u16SizeBytes)
{
    uint32_t u32BufferAddr;
    
    *pnSizeRead = nSize;
    if (u32VirtualAddress >= EXT_MEM_MIN_VIRTUAL_ADDRESS)
    {
        if (u32VirtualAddress >= EXT_MEM_SPI_MIN_VIRTUAL_ADDRESS)
        {
            #if HMDRV_USE_EXT_SPI
            u32BufferAddr = SPIMEM_pExtMemRamToIntRam(nSrcAddr, nSize, pnSizeRead, pu16ID);
            if(u32BufferAddr)*pnSizeRead = nSize;
            #else
            u32BufferAddr = NULL;
            #endif
        }
        else
        {
            #if USE_SPI_FLIR
            if (nSrcAddr >= EXT_MEM_FLIR_CCI_TWI_VIRTUAL_ADDRESS)
            {
                #if HMDRV_USE_EXT_I2C
                u32BufferAddr = I2CMEM_pFlirRegToIntRam(nSrcAddr, nSize);
                #else
                u32BufferAddr = NULL;
                #endif
            }
            else
            #endif
            if (u32VirtualAddress >= EXT_MEM_I2C_VIRTUAL_ADDRESS)
            {
                #if HMDRV_USE_EXT_I2C
                u32BufferAddr = I2CMEM_pExtMemRamToIntRam(nSrcAddr, nSize);
                #else
                u32BufferAddr = NULL;
                #endif
            }
            else
            {
                #if HMDRV_USE_EXT_PMP
                u32BufferAddr = PMPMEM_pExtMemRamToIntRam(nSrcAddr, nSize);
                #else
                u32BufferAddr = NULL;
                #endif
            }
        }
    }
    else
    {
        u32BufferAddr = u32VirtualAddress;
    }
    return u32BufferAddr;
}

uint32_t MEM_DRV_u32GetRamAddressReserveBuffer(uint32_t u32VirtualAddress, uint16_t u16SizeBytes)
{
}

void MEM_DRV_vWriteAddress(uint32_t u32VirtualAddress, uint32_t u32BufferAddress, uint16_t u16SizeBytes)
{
}

#endif  /* #if BOARDCFG_USE_MEM_DRV */
