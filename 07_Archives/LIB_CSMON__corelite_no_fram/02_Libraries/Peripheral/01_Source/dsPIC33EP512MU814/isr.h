/* *****************************************************************************
 * File:   isr.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 06 04 13:27
 * 
 * Description: ISR Driver dsPIC33
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef ISR_H
#define	ISR_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "mclib.h"
#include "dma_drv.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define DMA_USE_LIBRARY_ISR_DMA     1

/* *****************************************************************************
 * Constants Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

typedef struct
{
    uint16_t* pu16IFSBase;
    uint16_t* pu16IECBase;
    uint16_t* pu16IPCBase;
}ISR_sHandles_t;






/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */
#define ISR_vEnterCritical()   __builtin_disi(0x3FFF) /* disable interrupts */
#define ISR_vExitCritical()    __builtin_disi(0x0000) /* enable interrupts */



/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */

extern ISR_sHandles_t ISR_sHandles;


/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void ISR_vInit(void);
void ISR_vSetBitIFS (uint8_t bitOffset);
void ISR_vSetBitIEC (uint8_t bitOffset);
void ISR_vClrBitIFS (uint8_t bitOffset);
void ISR_vClrBitIEC (uint8_t bitOffset);
void ISR_vSetDatIPC (uint8_t bitOffset, uint8_t data);

bool ISR_vChkBitIFS (uint8_t bitOffset);
bool ISR_vChkBitIEC (uint8_t bitOffset);

void ISR_vSetISRDMA (pfVoid_t pfVoid, DMA_eModule_t eDMAModule);

#endif	/* ISR_H */
