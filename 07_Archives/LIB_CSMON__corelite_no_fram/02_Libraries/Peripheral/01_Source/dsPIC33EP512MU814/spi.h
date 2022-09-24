/* *****************************************************************************
 * File:   spi.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 06 03 13:23
 * 
 * Description: SPI Driver dsPIC33
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef SPI_H
#define	SPI_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdint.h>
#include <stdbool.h>
#include <xc.h>
#include "hmlib.h"
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define SPI_USE_DMA     SPI_DMA_NOT_USED    /* SPI_DMA_USE */


/* *****************************************************************************
 * Constants Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

typedef enum
{
    SPI_1,
    SPI_2,
    SPI_3,
    #if defined __dsPIC33EP512MU814__
    SPI_4,
    #endif
    SPI_COUNT,  /* Reserved Upto 16 SPI Modules */
    
    SPI_CS_START = 0x10,     /* see eExtMemSelect_t */  
    SPI_CS1 = SPI_CS_START,
    SPI_CS2,
    SPI_CS3,
    #if defined __dsPIC33EP512MU814__
    SPI_CS4,
    #endif
    SPI_CS_END,
    SPI_CS_COUNT = (SPI_CS_END - SPI_CS_START)     /* Reserved Upto 16 CS per SPI Module */
            
}SPI_eModule_t;


typedef enum
{
    SPI_MEM_EEPROM = 0,
    SPI_MEM_RAM,    
}SPI_eMemoryIndex_t;


typedef enum
{
    SPI_SECONDARY_DIVIDER_1_DIV_8 = 0,
    SPI_SECONDARY_DIVIDER_1_DIV_7 = 1,
    SPI_SECONDARY_DIVIDER_1_DIV_6 = 2,
    SPI_SECONDARY_DIVIDER_1_DIV_5 = 3,
    SPI_SECONDARY_DIVIDER_1_DIV_4 = 4,
    SPI_SECONDARY_DIVIDER_1_DIV_3 = 5,
    SPI_SECONDARY_DIVIDER_1_DIV_2 = 6,
    SPI_SECONDARY_DIVIDER_1_DIV_1 = 7,
    SPI_SECONDARY_DIVIDER_COUNT
}SPI_eSecondaryDivider_t;

typedef enum
{
    SPI_PRIMARY_DIVIDER_1_DIV_64 = 0,
    SPI_PRIMARY_DIVIDER_1_DIV_16 = 1,
    SPI_PRIMARY_DIVIDER_1_DIV_4  = 2,
    SPI_PRIMARY_DIVIDER_1_DIV_1  = 3,
    SPI_PRIMARY_DIVIDER_COUNT
}SPI_ePrimaryDivider_t;

typedef enum
{
    SPI_MODE_8BITS = 0,
    SPI_MODE_16BITS = 1,
    SPI_MODE_COUNT
}SPI_eModeBitsCount_t;

typedef enum
{
    SPI_MODE_0,
    SPI_MODE_1,
    SPI_MODE_2,
    SPI_MODE_3,
            
}SPI_eMode_t;



typedef enum
{
    SPI_CHIP_SELECT_ENABLE,
    SPI_CHIP_SELECT_DISABLE,      
            
    SPI_CHIP_SELECT_COUNT  
}SPI_eChipSelect_t;

typedef enum
{
    SPI_DMA_NOT_USED,
    SPI_DMA_USE,      
            
    SPI_DMA_COUNT  
}SPI_eUseSPIDMA_t;

typedef enum
{
    SPI_TRANSF_STA_IDLE,        /* Not Used */
    SPI_TRANSF_STA_FILLIND,     /* Reserved, not Ready To Transmit */
    SPI_TRANSF_STA_PENDING,     /* Ready To Transmit or Processing */
    SPI_TRANSF_STA_COMPLETED,   /* Transmit Completed Status Return */
    SPI_TRANSF_STA_FAIL,        /* Fail Status Return */
}SPI_eTransferStatus_t;


/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
typedef void (*SPI_pfVoid_u16_t)(uint16_t);
//typedef void (*SPI_pfVoid_t)(void);



/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void SPI_vInit( SPI_eModule_t index , SPI_eUseSPIDMA_t eUseSPIDMA);
void SPI_vSetMode ( SPI_eModule_t index, SPI_eMode_t eMode );
void SPI_vSetModeBitsCount( SPI_eModule_t index, SPI_eModeBitsCount_t eMode);

void SPI_vSetFrequencyDivider( 
        SPI_eModule_t index, 
        SPI_ePrimaryDivider_t ePriDivider, 
        SPI_eSecondaryDivider_t eSecDivider);

#if SPI_USE_DMA
void SPI_vInitTxDMA ( SPI_eModule_t eModuleSPI, DMA_eModule_t eModuleDMA );
void SPI_vInitRxDMA ( SPI_eModule_t eModuleSPI, DMA_eModule_t eModuleDMA );
#endif

void SPI_vCalcSPIPrescaler(uint32_t u32SPIFreqHz, uint32_t u32Fcy, uint16_t* u16SPIPrescaler);
void SPI_vInitSPIPrescalers(uint16_t prescallerRequest, SPI_ePrimaryDivider_t* pePriDivider, SPI_eSecondaryDivider_t* peSecDivider);

/******************************************************************************
 * SPI Read/Write Simple (in main loop)
 *****************************************************************************/
void SPI_Write ( SPI_eModule_t eModuleSPI, uint16_t u16Data );
uint16_t SPI_Read ( SPI_eModule_t eModuleSPI );
bool SPI_RxBufferEmpty ( SPI_eModule_t eModuleSPI );
bool SPI_TxBufferFull ( SPI_eModule_t eModuleSPI );
bool SPI_TxCompleted ( SPI_eModule_t eModuleSPI );

/******************************************************************************
 * SPI Read/Write ISR used
 *****************************************************************************/
void SPI_ReadWriteBuffer ( SPI_eModule_t eModuleSPI, HMDRV_ADDR_PNTR_MODIFIER uint8_t* pu8Data, uint16_t u16BytesCount, HMDRV_ADDR_PNTR_MODIFIER SPI_eTransferStatus_t* peStatus, uint16_t u16TransactionID);
#if 1
bool SPI_bQueueIsEmpty(SPI_eModule_t eModuleSPI);
bool SPI_bQueueIsFull(SPI_eModule_t eModuleSPI);
#endif
void SPI_vQueueInitialize(SPI_eModule_t eModuleSPI); /* If need to run externally */
void SPI_vChipSelectFunctionsInit(SPI_eModule_t eModuleSPI, SPI_pfVoid_u16_t pfChipSelectEnable, SPI_pfVoid_u16_t pfChipSelectDisable);
void SPI_vRxCompleteFunctionsInit(SPI_eModule_t eModuleSPI, SPI_pfVoid_u16_t pfTransferCompleted);
void SPI_vQueueProcess(SPI_eModule_t eModuleSPI);   /* If need to run externally */




#endif	/* SPI_H */
