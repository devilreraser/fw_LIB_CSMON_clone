/* *****************************************************************************
 * File:   pmp.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 06 18 11:36
 * 
 * Description: PMP Driver dsPIC33
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef PMP_H
#define	PMP_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdint.h>
#include <stdbool.h>
#include "dma_drv.h"
#include "boardcfg.h"
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define PMP_FAST_RW     1

#define PMCS1_MASK          (1 << 14)
#define PMCS2_MASK          (1 << 15)

#define PMCS1_LAT           LATKbits.LATK11  
#define PMCS1_TRIS          TRISKbits.TRISK11  

#define PMCS1A_LAT          LATJbits.LATJ14  
#define PMCS1A_TRIS         TRISJbits.TRISJ14  
 


/* *****************************************************************************
 * Constants Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */
typedef enum
{
    PMP_1,
    PMP_2,
    PMP_3,
    PMP_4,
    PMP_COUNT
}PMP_eModule_t;

#if PMP_BUS_SIZE_BYTES != 2
typedef uint8_t uintPMP_t;
#else
typedef uint16_t uintPMP_t;
#endif

typedef enum
{
    PMP_MODE_8BITS = 0,
    PMP_MODE_16BITS = 1,
    PMP_MODE_COUNT
}PMP_eModeBitsCount_t;



typedef enum
{
    PMP_DMA_NOT_USED,
    PMP_DMA_USE,      
            
    PMP_DMA_COUNT  
}PMP_eUsePMPDMA_t;

typedef enum 
{
    PMP_CS_1,       /* 0 - position 14 bit address PMA14 (pin PMCS1) */
    PMP_CS_2,       /* 1 - position 15 bit address PMA15 (pin PMCS2) */
    PMP_CS_12,      /* 2 - position 14,15 bit address PMA14 (pin PMCS1),PMA15 (pin PMCS2) */
    PMP_CS_COUNT
         
}ePMPSelect_t;

typedef enum 
{
    PMP_CMD_IDLE,
    PMP_CMD_BUSY,
    PMP_CMD_WR,
    PMP_CMD_RD,
    PMP_CMD_CLR,
    PMP_CMD_COUNT
         
}ePMPCmd_t;



/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */




/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

#if 0
#define PMP_vReadFastPMCS1Byte(u16Address, pdData) \
    PMADDR = u16Address | PMCS1_MASK; \
    PMDIN1; \
    while (PMMODEbits.BUSY); \
    *(pdData) = PMDIN1


#define PMP_vWriteFastPMCS1Byte(u16Address, pdData) \
    PMADDR = u16Address | PMCS1_MASK; \
    while (PMMODEbits.BUSY); \
    PMDIN1 = *(pdData) 
#endif


#if PMP_FAST_RW
#define PMP_vWriteDefault(u16Address, pdData, u16Size)      PMP_vWriteFastPMCS1(u16Address, pdData, u16Size)
#define PMP_vReadDefault(u16Address, pdData, u16Size)       PMP_vReadFastPMCS1(u16Address, pdData, u16Size)
#define PMP_vClearDefault(u16Address, u16Size)              PMP_vClearFastPMCS1(u16Address, u16Size)
#else
//#define PMP_vWriteDefault(ePMPSelect, u16Address, pdData, u16Size, bUseHeap)    PMP_vWrite(ePMPSelect, u16Address, pdData, u16Size, bUseHeap)
//#define PMP_vReadDefault(ePMPSelect, u16Address, pdData, u16Size, bUseHeap)     PMP_vRead(ePMPSelect, u16Address, pdData, u16Size, bUseHeap)
#endif


/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */
extern bool bUseDynamicMemoryRD;
extern bool bUseDynamicMemoryWR;
extern bool bUseDirectCopyRD;
extern bool bUseDirectCopyWR;


/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void PMP_vInit( PMP_eUsePMPDMA_t eUsePMPDMA);
void PMP_vInitDMA(DMA_eModule_t eModuleDMA);

bool PMP_vWrite(ePMPSelect_t ePMPSelect, uint16_t u16Address, uintPMP_t* pdData, uint16_t u16Size, bool bUseHeap);
bool PMP_vRead(ePMPSelect_t ePMPSelect, uint16_t u16Address, uintPMP_t* pdData, uint16_t u16Size, bool bUseHeap);

bool PMP_bBusyStatus(void);
void PMP_vDMACompleteProcess ( void );

void PMP_vWriteFastPMCS1(uint16_t u16Address, uintPMP_t* pdData, uint16_t u16Size);
void PMP_vReadFastPMCS1(uint16_t u16Address, uintPMP_t* pdData, uint16_t u16Size);
void PMP_vClearFastPMCS1(uint16_t u16Address, uint16_t u16Size);
void PMP_vWriteFastPMCS1Byte(uint16_t u16Address, uintPMP_t* pdData);
void PMP_vReadFastPMCS1Byte(uint16_t u16Address, uintPMP_t* pdData);
#endif	/* PMP_H */
