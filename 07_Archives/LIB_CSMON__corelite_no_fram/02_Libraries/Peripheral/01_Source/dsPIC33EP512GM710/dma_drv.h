/* *****************************************************************************
 * File:   dma_drv.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 06 03 16:48
 * 
 * Description: DMA Driver dsPIC33
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef DMA_DRV_H
#define	DMA_DRV_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdint.h>
#include <xc.h>

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */
typedef enum
{
    DMA_0,
    DMA_1,
    DMA_2,
    DMA_3,
    DMA_4,
    DMA_5,
    DMA_6,
    DMA_7,
    DMA_8,
    DMA_9,
    DMA_10,
    DMA_11,
    DMA_12,
    DMA_13,
    DMA_14,
    DMA_COUNT        
}DMA_eModule_t;

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

typedef struct
{
    union 
    {
        struct 
        {
            uint16_t DMACON;
            uint16_t DMAREQ;
            uint16_t DMASTAL;
            uint16_t DMASTAH;
            uint16_t DMASTBL;
            uint16_t DMASTBH;
            uint16_t DMAPAD;
            uint16_t DMACNT;
        };
        
        struct
        {
            DMA0CONBITS DMACONbits;
            DMA0REQBITS DMAREQbits;
            uint16_t    :16;
            DMA0STAHBITS DMASTAHbits;
            uint16_t    :16;
            DMA0STBHBITS DMASTBHbits;
            uint16_t    :16;
            DMA0CNTBITS DMACNTbits;
        };
    };
}DMA_sBaseRegisters_t;

typedef struct
{
    DMA_sBaseRegisters_t* psBase;
    uint8_t u8BitOffsetsISR[DMA_COUNT];
}DMA_sHandles_t;






/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */

extern DMA_sHandles_t DMA_sHandles;


/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */

#endif	/* DMA_DRV_H */
