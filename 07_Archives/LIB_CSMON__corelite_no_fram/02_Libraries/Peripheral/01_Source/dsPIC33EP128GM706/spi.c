/* *****************************************************************************
 * File:   spi.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 06 03 13:23
 * 
 * Description: SPI Driver dsPIC33
 * 
 **************************************************************************** */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <xc.h>
#include <stdint.h>

#include "boardcfg.h"
#include "spi.h"
#include "isr.h"

#if SPI_USE_DMA
#include "dma_drv.h"
#endif

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define SPI_COUNT_TRANSFER_HANDLES 1    /* Queue */


/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

typedef struct
{
    union 
    {
        struct 
        {
            uint16_t SPISTAT;
            uint16_t SPICON1;
            uint16_t SPICON2;
            uint16_t :16;
            uint16_t SPIBUF;
        };
        
        struct
        {
            SPI1STATBITS SPISTATbits;
            SPI1CON1BITS SPICON1bits;
            SPI1CON2BITS SPICON2bits;
            uint16_t :16;
            uint16_t :16;
        };
    };
}SPI_sBaseRegisters_t;

typedef struct
{
    SPI_sBaseRegisters_t* psBase;
}SPI_sHandles_t;

typedef struct
{
    uint8_t u8BitOffsetsISRTrnsf[SPI_COUNT];
    uint8_t u8BitOffsetsISRError[SPI_COUNT];
}SPI_sBitOffsetsISR_t;


typedef union
{
    struct
    {
            uint8_t full:1;
            uint8_t empty:1;
            uint8_t reserved:6;
    }s;
    uint8_t status;
} SPI_eQueueStatus_t;

typedef struct 
{
    HMDRV_ADDR_PNTR_MODIFIER uint8_t* pu8DataBuffer;
    uint16_t u16TransfersCount;
    SPI_eMemoryIndex_t eMemoryIndex;
    HMDRV_ADDR_PNTR_MODIFIER SPI_eTransferStatus_t* peTransferStatus;
    SPI_eTransferStatus_t eTransferStatus;
    uint16_t u16TransactionID;
}SPI_sTransferHandle_t;

typedef struct
{
    /* Read/Write Queue */
    SPI_sTransferHandle_t       *pTrTail;       // tail of the queue
    SPI_sTransferHandle_t       *pTrHead;       // head of the queue
    SPI_eQueueStatus_t          trStatus;       // status of the last transaction
    uint8_t                     spiDoneFlag;    // flag to indicate the current transaction is done
    uint8_t                     spiErrors;      // keeps track of errors
} SPI_sObject_t ;


/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
SPI_sHandles_t SPI_sHandles[SPI_COUNT] = 
{
    {
        (SPI_sBaseRegisters_t*)(&SPI1STAT)
    },
    {
        (SPI_sBaseRegisters_t*)(&SPI2STAT)
    },
    {
        (SPI_sBaseRegisters_t*)(&SPI3STAT)
    },
#if SPI_COUNT >= 4
    {
        (SPI_sBaseRegisters_t*)(&SPI4STAT)
    },
#endif
};
SPI_sBitOffsetsISR_t SPI_sBitOffsetsISR = 
{
    {   /* Tfr */
        10 + 0*16,          /* SPI1 */         
         1 + 2*16,          /* SPI2 */         
        11 + 5*16,          /* SPI3 */         
#if SPI_COUNT >= 4
        11 + 7*16,          /* SPI4 */                 
#endif
    },
    {   /* Err */
         9 + 0*16,          /* SPI1 */         
         0 + 2*16,          /* SPI2 */         
        10 + 5*16,          /* SPI3 */         
#if SPI_COUNT >= 4
        10 + 7*16,          /* SPI4 */                 
#endif
    },
};

#if SPI_COUNT >= 4
#define SPI_NULL_INIT {NULL, NULL, NULL, NULL}
#else
#define SPI_NULL_INIT {NULL, NULL, NULL}
#endif

/* Queue Implementation */
SPI_sTransferHandle_t SPI_sTransferHandle[SPI_COUNT][SPI_COUNT_TRANSFER_HANDLES];
SPI_sObject_t SPI_sObject[SPI_COUNT];

HMDRV_ADDR_PNTR_MODIFIER uint8_t*                SPI_pu8CurrentBufWR[SPI_COUNT]; /* Write Data To SPI */
uint16_t                SPI_u16CurrentCntWR[SPI_COUNT];
HMDRV_ADDR_PNTR_MODIFIER uint8_t*                SPI_pu8CurrentBufRD[SPI_COUNT]; /* Read Data From SPI */
uint16_t                SPI_u16CurrentCntRD[SPI_COUNT];
SPI_sTransferHandle_t*  SPI_psCurrentHandle[SPI_COUNT] = SPI_NULL_INIT;

SPI_pfVoid_u16_t        SPI_pfChipSelectEnable[SPI_COUNT] = SPI_NULL_INIT;
SPI_pfVoid_u16_t        SPI_pfChipSelectDisable[SPI_COUNT] = SPI_NULL_INIT;
SPI_pfVoid_u16_t        SPI_pfTransferCompleted[SPI_COUNT] = SPI_NULL_INIT;

SPI_pfVoid_u16_t        SPI_pfISRTransferComplete[SPI_COUNT] = SPI_NULL_INIT;
SPI_pfVoid_u16_t        SPI_pfISRError[SPI_COUNT] = SPI_NULL_INIT;

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */
void SPI_vErrorProcess(SPI_eModule_t index);
void SPI_ModuleDisable(SPI_eModule_t index);
void SPI_ModuleEnable(SPI_eModule_t index);
void SPI_ResetPrescalerToSlowest(SPI_eModule_t index);
void SPI_vInitPinout (SPI_eModule_t index);


/* *****************************************************************************
 * Functions definitions
 **************************************************************************** */


/* *****************************************************************************
 * ISR Functions
 **************************************************************************** */
void __attribute__ ( (interrupt, auto_psv) ) _SPI1Interrupt( void )
{
    /* write optional extern function call here after register in the module */
    IFS0bits.SPI1IF = 0;
    if (SPI_pfISRTransferComplete[SPI_1] != NULL)
    {
        SPI_pfISRTransferComplete[SPI_1](SPI_1);
    }
}

void __attribute__ ( (interrupt, auto_psv) ) _SPI2Interrupt( void )
{
    /* write optional extern function call here after register in the module */
    IFS2bits.SPI2IF = 0;
    if (SPI_pfISRTransferComplete[SPI_2] != NULL)
    {
        SPI_pfISRTransferComplete[SPI_2](SPI_2);
    }
}

void __attribute__ ( (interrupt, auto_psv) ) _SPI3Interrupt( void )
{
    /* write optional extern function call here after register in the module */
    IFS5bits.SPI3IF = 0;
    if (SPI_pfISRTransferComplete[SPI_3] != NULL)
    {
        SPI_pfISRTransferComplete[SPI_3](SPI_3);
    }
}

#if SPI_COUNT >= 4
void __attribute__ ( (interrupt, auto_psv) ) _SPI4Interrupt( void )
{
    /* write optional extern function call here after register in the module */
    IFS7bits.SPI4IF = 0;
    if (SPI_pfISRTransferComplete[SPI_4] != NULL)
    {
        SPI_pfISRTransferComplete[SPI_4](SPI_4);
    }
}
#endif

void __attribute__ ( (interrupt, auto_psv) ) _SPI1ErrInterrupt( void )
{
    /* write optional extern function call here after register in the module */
    IFS0bits.SPI1EIF = 0;
    if (SPI_pfISRError[SPI_1] != NULL)
    {
        SPI_pfISRError[SPI_1](SPI_1);
    }
}

void __attribute__ ( (interrupt, auto_psv) ) _SPI2ErrInterrupt( void )
{
    /* write optional extern function call here after register in the module */
    IFS2bits.SPI2EIF = 0;
    if (SPI_pfISRError[SPI_2] != NULL)
    {
        SPI_pfISRError[SPI_2](SPI_2);
    }
}

void __attribute__ ( (interrupt, auto_psv) ) _SPI3ErrInterrupt( void )
{
    /* write optional extern function call here after register in the module */
    IFS5bits.SPI3EIF = 0;
    if (SPI_pfISRError[SPI_3] != NULL)
    {
        SPI_pfISRError[SPI_3](SPI_3);
    }
}
#if SPI_COUNT >= 4
void __attribute__ ( (interrupt, auto_psv) ) _SPI4ErrInterrupt( void )
{
    /* write optional extern function call here after register in the module */
    IFS7bits.SPI4EIF = 0;
    if (SPI_pfISRError[SPI_4] != NULL)
    {
        SPI_pfISRError[SPI_4](SPI_4);
    }
}
#endif






/* *****************************************************************************
 * SPI Local Usage Functions
 **************************************************************************** */
void SPI_vErrorProcess(SPI_eModule_t index)
{
    
}


/* Made As function Because Of Optimization Issues */
void SPI_ModuleDisable(SPI_eModule_t index)
{
    volatile SPI_sBaseRegisters_t* psBase = SPI_sHandles[index].psBase;
    psBase->SPISTATbits.SPIEN = 0x0;                    // disable the SPI module
}

/* Made As function Because Of Optimization Issues */
void SPI_ModuleEnable(SPI_eModule_t index)
{
    volatile SPI_sBaseRegisters_t* psBase = SPI_sHandles[index].psBase;
    psBase->SPISTATbits.SPIEN = 0x1;                    // enable the SPI module
}

/* Made As function Because Of Optimization Issues */
void SPI_ResetPrescalerToSlowest(SPI_eModule_t index)
{
    volatile SPI_sBaseRegisters_t* psBase = SPI_sHandles[index].psBase;
    psBase->SPICON1bits.PPRE = 0; 
    psBase->SPICON1bits.SPRE = 0; 
}

/* *****************************************************************************
 * SPI_vInitPinout - SPI Initialization Pinout
 **************************************************************************** */
void SPI_vInitPinout (SPI_eModule_t index)
{
    switch (index)
    {
        case SPI_1:
            __builtin_write_OSCCONL(OSCCON & (~(1<<6))); // clear bit 6 
            #if defined __dsPIC33EP512MU814__
            RPOR14bits.RP125R = 6;    //RP125/RG13 as SCK1
            RPOR15bits.RP126R = 5;    //RP126/RG14 as SDO1
            RPINR20bits.SDI1R = 124; //RP124/RG12 as SDI1
            #endif
            __builtin_write_OSCCONL(OSCCON | (1<<6)); 	 // Set bit 6 	
            break;
        default:
            break;
    }
}



/* *****************************************************************************
 * SPI_vInit
 * 
 * Description: SPI Initialization 
 **************************************************************************** */
void SPI_vInit ( SPI_eModule_t index, SPI_eUseSPIDMA_t eUseSPIDMA )
{
    volatile SPI_sBaseRegisters_t* psBase = SPI_sHandles[index].psBase;

    // setup the SPI peripheral
    psBase->SPISTAT = 0x0;                             // disable the SPI module (just in case)
    
    SPI_vInitPinout(index);
    
    psBase->SPICON1bits.DISSDO = 0;
    psBase->SPICON1bits.MODE16 = 0;                          
    psBase->SPICON1bits.SSEN = 0;
    psBase->SPICON1bits.MSTEN = 1; 
    
    /* Do not set both SPRE and PPRE to 1:2 - Fp=60MHz (Fcy=60MHz; Fp=Fcy) */
    //psBase->SPICON1bits.SPRE = 0b111;                  //(1:1)
    psBase->SPICON1bits.SPRE = 0b110;                //(1:2)       /* 1:2 -> 30MHz SPI Clock */
    psBase->SPICON1bits.PPRE = 0b11;                   //(1:1)
    
    
    psBase->SPICON1bits.SMP = 0x00;                    /* set if needed only after MSTEN=1 */
    psBase->SPICON1bits.CKE = ((~(SPI_DEFAULT_MODE & 0x02 >> 1)) & 0x01);  /* CKE bit is inverted CPHA */
    psBase->SPICON1bits.CKP = SPI_DEFAULT_MODE & 0x01;                     /* CKP bit is equal to CPOL */
    psBase->SPICON2 = 0;                               // FRAMEN = 0, SPIFSD = 0, SPIBEN = 0
    if (eUseSPIDMA == 1)
    {
        psBase->SPISTATbits.SISEL = 0b000;                 /* interrupt when the last data in the receive buffer is read and the buffer is empty (SRXMPT == 1) */
        psBase->SPICON2bits.SPIBEN = 0;                    /* enhanced buffer disabled */
    }
    else
    {
        psBase->SPISTATbits.SISEL = 0b101;                 /* interrupt when the last bit is shifted out and the transmit is complete */
        psBase->SPICON2bits.SPIBEN = 1;                    /* enhanced buffer enable */        
    }
    psBase->SPISTATbits.SPIEN = 1;                     // enable the SPI module
    
    

    //SPI interrupts enable/setup
    ISR_vSetDatIPC(SPI_sBitOffsetsISR.u8BitOffsetsISRTrnsf[index],ISR_PRIORITY_SPITRANS);    // interrupt priority level
    ISR_vSetDatIPC(SPI_sBitOffsetsISR.u8BitOffsetsISRError[index],ISR_PRIORITY_SPIERROR);    // interrupt priority level
    ISR_vClrBitIFS(SPI_sBitOffsetsISR.u8BitOffsetsISRTrnsf[index]);    // Clear interrupt
    ISR_vClrBitIEC(SPI_sBitOffsetsISR.u8BitOffsetsISRTrnsf[index]);    // disable interrupt
    ISR_vClrBitIFS(SPI_sBitOffsetsISR.u8BitOffsetsISRError[index]);    // Clear interrupt
    ISR_vClrBitIEC(SPI_sBitOffsetsISR.u8BitOffsetsISRError[index]);    // disable interrupt
    
    SPI_vQueueInitialize(index);

}



/* *****************************************************************************
 * SPI Mode Description: 

SPI mode	Clock polarity(CPOL/CKP)	Clock phase(CPHA)	Clock edge(CKE/NCPHA)
    0               0                           0                   1
    1               0                           1                   0
    2               1                           0                   1
    3               1                           1                   0

 **************************************************************************** */
void SPI_vSetMode ( SPI_eModule_t index, SPI_eMode_t eMode )
{
    volatile SPI_sBaseRegisters_t* psBase = SPI_sHandles[index].psBase;

    //psBase->SPISTATbits.SPIEN = 0;                     // disable the SPI module (just in case)
    psBase->SPICON1bits.CKE = ((~(eMode & 0x02 >> 1)) & 0x01);  /* CKE bit is inverted CPHA */
    psBase->SPICON1bits.CKP = eMode & 0x01;                     /* CKP bit is equal to CPOL */   
    //psBase->SPISTATbits.SPIEN = 1;                     // enable the SPI module
}

/* *****************************************************************************
 * SPI_vSetModeBitsCount
 * 
 * Description: SPI_vSetModeBitsCount 
 **************************************************************************** */
void SPI_vSetModeBitsCount( SPI_eModule_t index, SPI_eModeBitsCount_t eMode)
{
    volatile SPI_sBaseRegisters_t* psBase = SPI_sHandles[index].psBase;

    SPI_ModuleDisable(index);
    psBase->SPICON1bits.MODE16 = eMode;     
    SPI_ModuleEnable(index);
}

/* *****************************************************************************
 * SPI_vSetFrequencyDivider
 * 
 * Description: SPI_vSetFrequencyDivider 
 **************************************************************************** */
void SPI_vSetFrequencyDivider( SPI_eModule_t index, SPI_ePrimaryDivider_t ePriDivider, SPI_eSecondaryDivider_t eSecDivider)
{
    volatile SPI_sBaseRegisters_t* psBase = SPI_sHandles[index].psBase;

    SPI_ModuleDisable(index);
    SPI_ResetPrescalerToSlowest(index);
    psBase->SPICON1bits.PPRE = ePriDivider; 
    psBase->SPICON1bits.SPRE = eSecDivider;   
    
    SPI_ModuleEnable(index);
}

#if SPI_USE_DMA
/******************************************************************************
 * Function:       void SPI_vInitTxDMA(void)
 *
 * Overview:        1.This function is used to configure the DMA module to operate with the SPI transmitter and
 *                  transfer the contents from DMA RAM to the SPI module in continuous ping pong mode with
 *                  Register indirect addressing.
 *                  2. DMA initialization/configuration function.
 *****************************************************************************/
void SPI_vInitTxDMA ( SPI_eModule_t eModuleSPI, DMA_eModule_t eModuleDMA )
{
    volatile SPI_sBaseRegisters_t* psBaseSPI = SPI_sHandles[eModuleSPI].psBase;
    volatile DMA_sBaseRegisters_t* psBaseDMA = DMA_sHandles.psBase + eModuleDMA;
    uint8_t u8BitOffsetsISRDMA = DMA_sHandles.u8BitOffsetsISR[eModuleDMA];

    
    psBaseDMA->DMACONbits.CHEN = 0;   // Disable DMA Channel
    psBaseDMA->DMACONbits.DIR = 1;        /* 1 = Reads from DPSRAM (or RAM) address, writes to peripheral address */
    psBaseDMA->DMACONbits.SIZE = 1;       /* 0 = Word; 1 = Byte Transfer */
    psBaseDMA->DMACONbits.HALF = 0;       /* 0 = Initiates interrupt when all of the data has been moved */
    psBaseDMA->DMACONbits.NULLW = 0;      /* 0 = Normal operation */
    psBaseDMA->DMACONbits.AMODE = 0;      /* 00 = Register Indirect with Post-Increment mode */
    
    psBaseDMA->DMAREQ = 33;    
    psBaseDMA->DMAPAD = ( volatile uint16_t ) &(psBaseSPI->SPIBUF);

    psBaseDMA->DMACONbits.MODE = 1;       /* 11 = One Shot, Ping-Pong modes are enabled  
                                 * 10 = Continuous, Ping-Pong modes are enabled  
                                 * 01 = One Shot, Ping-Pong modes are disabled  
                                 * 00 = Continuous, Ping-Pong modes are disabled  */
    
    ISR_vSetDatIPC(u8BitOffsetsISRDMA,ISR_PRIORITY_SPITXDMA);    // DMA interrupt priority level
    ISR_vClrBitIFS(u8BitOffsetsISRDMA);    // Clear DMA interrupt
    ISR_vSetBitIEC(u8BitOffsetsISRDMA);    // Enable DMA interrupt
    psBaseDMA->DMACONbits.CHEN = 0;   // Disable DMA Channel
}


/******************************************************************************
 * Function:       void SPI_vInitRxDMA(void)
 *
 * Overview:        1.This function is used to configure the DMA module to operate with the SPI receiver and
 *                    transfer the contents from SPI module to the DMA RAM in continuous ping pong mode
 *                    with Register indirect addressing.
 *                  2.DMA initialization/configuration function.
 *****************************************************************************/
void SPI_vInitRxDMA ( SPI_eModule_t eModuleSPI, DMA_eModule_t eModuleDMA )
{
    volatile SPI_sBaseRegisters_t* psBaseSPI = SPI_sHandles[eModuleSPI].psBase;
    volatile DMA_sBaseRegisters_t* psBaseDMA = DMA_sHandles.psBase + eModuleDMA;
    uint8_t u8BitOffsetsISRDMA = DMA_sHandles.u8BitOffsetsISR[eModuleDMA];

    psBaseDMA->DMACONbits.CHEN = 0;   // Disable DMA Channel    
    psBaseDMA->DMACONbits.DIR = 0;        /* 0 = Reads from peripheral address, writes to DPSRAM (or RAM) address */
    psBaseDMA->DMACONbits.SIZE = 1;       /* 0 = Word; 1 = Byte Transfer */
    psBaseDMA->DMACONbits.HALF = 0;       /* 0 = Initiates interrupt when all of the data has been moved */
    psBaseDMA->DMACONbits.NULLW = 0;      /* 0 = Normal operation */
    psBaseDMA->DMACONbits.AMODE = 0;      /* 00 = Register Indirect with Post-Increment mode */

    psBaseDMA->DMAREQ = 33;
    psBaseDMA->DMAPAD = ( volatile uint16_t ) &(psBaseSPI->SPIBUF);

    psBaseDMA->DMACONbits.MODE = 1;       /* 11 = One Shot, Ping-Pong modes are enabled  
                                 * 10 = Continuous, Ping-Pong modes are enabled  
                                 * 01 = One Shot, Ping-Pong modes are disabled  
                                 * 00 = Continuous, Ping-Pong modes are disabled  */
    
    ISR_vSetDatIPC(u8BitOffsetsISRDMA,ISR_PRIORITY_SPITXDMA);    // DMA interrupt priority level
    ISR_vClrBitIFS(u8BitOffsetsISRDMA);    // Clear DMA interrupt
    ISR_vSetBitIEC(u8BitOffsetsISRDMA);    // Enable DMA interrupt
    psBaseDMA->DMACONbits.CHEN = 0;   // Disable DMA Channel    
}
#endif

/******************************************************************************
 * Function:       SPI_vCalcSPIPrescaler
 *
 * Overview:       Calculates u16SPIPrescaler from u32SPIFreqHz and u32Fcy
 *****************************************************************************/
void SPI_vCalcSPIPrescaler(uint32_t u32SPIFreqHz, uint32_t u32Fcy, uint16_t* u16SPIPrescaler)
{
    //*u16SPIPrescaler = (uint16_t)(((u32Fcy + (u32SPIFreqHz/2))/ u32SPIFreqHz));   /* allow 50 % frequency overflow */
    *u16SPIPrescaler = (uint16_t)(((u32Fcy + (u32SPIFreqHz-1))/ u32SPIFreqHz));/* not possible frequency overflow */
}

/******************************************************************************
 * Function:       SPI_vInitSPIPrescalers
 *
 * Overview:       Calculates eSPIPrimaryDivider and eSPISecondaryDivider from prescallerRequest
 *****************************************************************************/
void SPI_vInitSPIPrescalers(uint16_t prescallerRequest, SPI_ePrimaryDivider_t* pePriDivider, SPI_eSecondaryDivider_t* peSecDivider)
{
    uint16_t primaryPre = 1;
    if (prescallerRequest == 0)
    {
        prescallerRequest = 1;
    }
    
    if (prescallerRequest == 1) /* Work Around dsPIC33 TSC 1:1 pre-scale not supported  */
    {
        prescallerRequest = 2;
    }
    
    while ( ((prescallerRequest % 4) == 0) && (primaryPre < 4) )
    {
        prescallerRequest >>= 2;
        primaryPre++;
    }    
    
    
    
    if (prescallerRequest <= 8)
    {
        *peSecDivider = 8 - prescallerRequest;    
    }
    else    /* exact frequency cannot be achieved */
    {
        
        while ((primaryPre < 4) && (prescallerRequest > 8))
        {
            prescallerRequest += 2; /* round */
            prescallerRequest >>= 2;
            primaryPre++;
        }

        if (prescallerRequest > 8)
        {
            prescallerRequest = 8;
        }     
    }
    
    
    *pePriDivider = 4 - primaryPre;
    *peSecDivider = 8 - prescallerRequest;       
}



/******************************************************************************
 * SPI Read/Write Simple (in main loop)
 *****************************************************************************/


/******************************************************************************
 * Function:       SPI_Write
 *
 * Overview:       Write Data To SPI Buffer
 *****************************************************************************/
void SPI_Write ( SPI_eModule_t eModuleSPI, uint16_t u16Data )
{
    SPI_sHandles[eModuleSPI].psBase->SPIBUF = u16Data;
}

/******************************************************************************
 * Function:       SPI_Read
 *
 * Overview:       Read Data From SPI Buffer
 *****************************************************************************/
uint16_t SPI_Read ( SPI_eModule_t eModuleSPI )
{
    uint16_t u16Result;
    u16Result = SPI_sHandles[eModuleSPI].psBase->SPIBUF;
    return u16Result;
}

/******************************************************************************
 * Function:       SPI_RxBufferEmpty
 *
 * Overview:       
 *****************************************************************************/
bool SPI_RxBufferEmpty ( SPI_eModule_t eModuleSPI )
{
    bool bResult = false;
    if (SPI_sHandles[eModuleSPI].psBase->SPISTATbits.SRXMPT)
    {
        bResult = true;
    }
    return bResult;
}

/******************************************************************************
 * Function:       SPI_TxBufferFull
 *
 * Overview:       
 *****************************************************************************/
bool SPI_TxBufferFull ( SPI_eModule_t eModuleSPI )
{
    bool bResult = false;
    if (SPI_sHandles[eModuleSPI].psBase->SPISTATbits.SPITBF)
    {
        bResult = true;
    }
    return bResult;
}

/******************************************************************************
 * Function:       SPI_TxCompleted
 *
 * Overview:       
 *****************************************************************************/
bool SPI_TxCompleted ( SPI_eModule_t eModuleSPI )
{
    bool bResult = false;
    if ((SPI_sHandles[eModuleSPI].psBase->SPISTATbits.SPIBEC == 0)  /* No pending Transfers */
     && (SPI_sHandles[eModuleSPI].psBase->SPISTATbits.SRMPT))       /* Shift Register Empty */
    {
        bResult = true;
    }
    return bResult;
}






/******************************************************************************
 * SPI Read/Write Queue used - ISR or main loop 
 *****************************************************************************/

/******************************************************************************
 * SPI_ReadWriteBuffer
 *
 * 
 * Overview:       Read Write Data To/From SPI Buffer queue up and fill
 *                 Data Width Extracted From SPI Configuration
 *****************************************************************************/
void SPI_ReadWriteBuffer ( SPI_eModule_t eModuleSPI, HMDRV_ADDR_PNTR_MODIFIER uint8_t* pu8Data, uint16_t u16BytesCount, HMDRV_ADDR_PNTR_MODIFIER SPI_eTransferStatus_t* peStatus, uint16_t u16TransactionID)
{
    SPI_eMemoryIndex_t eMemoryIndex = ((eModuleSPI >> 4) & 0x000F);     /* u16TransactionID also contains eMemoryIndex */
    eModuleSPI &= 0x000F;
    
    ISR_vEnterCritical();
    // check if there is space in the queue
    if (SPI_sObject[eModuleSPI].trStatus.s.full != true)
    {
        #if SPI_COUNT_TRANSFER_HANDLES > 1
        // check if the end of the array is reached
        if ((SPI_sObject[eModuleSPI].pTrTail+1) == (SPI_sTransferHandle[eModuleSPI] + SPI_COUNT_TRANSFER_HANDLES))
        {
            // adjust to restart at the beginning of the array
            SPI_sObject[eModuleSPI].pTrTail = SPI_sTransferHandle[eModuleSPI];
        }
        else
        {
            SPI_sObject[eModuleSPI].pTrTail++;
        }
        // check if full
        if (SPI_sObject[eModuleSPI].pTrHead == SPI_sObject[eModuleSPI].pTrTail)
        #endif
        {
            // it is full, set the full status to true
            SPI_sObject[eModuleSPI].trStatus.s.full = true;
        }
        ISR_vExitCritical();
        
        if (peStatus != NULL)
        {
            *peStatus = SPI_TRANSF_STA_PENDING;
        }
        
        SPI_sObject[eModuleSPI].pTrTail->pu8DataBuffer      = pu8Data;
        SPI_sObject[eModuleSPI].pTrTail->u16TransfersCount  = u16BytesCount;
        SPI_sObject[eModuleSPI].pTrTail->peTransferStatus   = peStatus;
        SPI_sObject[eModuleSPI].pTrTail->eTransferStatus    = SPI_TRANSF_STA_PENDING;
        SPI_sObject[eModuleSPI].pTrTail->eMemoryIndex       = eMemoryIndex;
        SPI_sObject[eModuleSPI].pTrTail->u16TransactionID   = u16TransactionID;
        


        // since we added one item to be processed, we know
        // it is not empty, so set the empty status to false
        SPI_sObject[eModuleSPI].trStatus.s.empty = false;

        
        // for interrupt based - if disabled interrupt
        if(ISR_vChkBitIEC(SPI_sBitOffsetsISR.u8BitOffsetsISRTrnsf[eModuleSPI]) == false)
        {    
            // force the task to run since we know that the queue has
            // something that needs to be sent
            ISR_vSetBitIEC(SPI_sBitOffsetsISR.u8BitOffsetsISRTrnsf[eModuleSPI]);    // enable interrupt - check enough to enter ISR
            //SPI_sHandles[eModuleSPI].psBase->SPIBUF = u16Data;
        }           
    }
    else
    {
        ISR_vExitCritical();
        if (peStatus != NULL)
        {
            *peStatus = SPI_TRANSF_STA_FAIL;
        }
    }
}


#if 1
/******************************************************************************
 * SPI_bQueueIsEmpty - use only if sure that will be not interrupted by another call
 *****************************************************************************/
bool SPI_bQueueIsEmpty(SPI_eModule_t eModuleSPI)
{
    return((bool)SPI_sObject[eModuleSPI].trStatus.s.empty);
}

/******************************************************************************
 * SPI_bQueueIsFull - use only if sure that will be not interrupted by another call
 *****************************************************************************/
bool SPI_bQueueIsFull(SPI_eModule_t eModuleSPI)
{
    return((bool)SPI_sObject[eModuleSPI].trStatus.s.full);
}
#endif

/******************************************************************************
 * SPI_vQueueInitialize
 *****************************************************************************/
void SPI_vQueueInitialize(SPI_eModule_t eModuleSPI)
{
    SPI_sObject[eModuleSPI].pTrHead = SPI_sTransferHandle[eModuleSPI];
    SPI_sObject[eModuleSPI].pTrTail = SPI_sTransferHandle[eModuleSPI];
    SPI_sObject[eModuleSPI].trStatus.s.empty = true;
    SPI_sObject[eModuleSPI].trStatus.s.full = false;
    SPI_sObject[eModuleSPI].spiErrors = 0;  
    SPI_pfISRTransferComplete[eModuleSPI] = SPI_vQueueProcess;
    SPI_pfISRError[eModuleSPI] = SPI_vErrorProcess;
}

/******************************************************************************
 * SPI_vChipSelectFunctionsInit
 *****************************************************************************/
void SPI_vChipSelectFunctionsInit(SPI_eModule_t eModuleSPI, SPI_pfVoid_u16_t pfChipSelectEnable, SPI_pfVoid_u16_t pfChipSelectDisable)
{
    SPI_pfChipSelectEnable [eModuleSPI] = pfChipSelectEnable;
    SPI_pfChipSelectDisable[eModuleSPI] = pfChipSelectDisable;  
}

/******************************************************************************
 * SPI_vRxCompleteFunctionsInit
 *****************************************************************************/
void SPI_vRxCompleteFunctionsInit(SPI_eModule_t eModuleSPI, SPI_pfVoid_u16_t pfTransferCompleted)
{
    SPI_pfTransferCompleted[eModuleSPI] = pfTransferCompleted;  
}



/******************************************************************************
 * SPI_vQueueProcess - Call In ISR (consider also main loop)
 *****************************************************************************/
void SPI_vQueueProcess(SPI_eModule_t eModuleSPI)
{
    /* Process Received Data Transmit */
    while(SPI_sHandles[eModuleSPI].psBase->SPISTATbits.SRXMPT)
    {
        uint16_t u16Data = SPI_sHandles[eModuleSPI].psBase->SPIBUF;
        if (SPI_psCurrentHandle[eModuleSPI] != NULL)
        {
            if(SPI_u16CurrentCntRD[eModuleSPI] > 0)
            {
                // spi read
                if (SPI_sHandles[eModuleSPI].psBase->SPICON1bits.MODE16)
                {
                    HMDRV_ADDR_PNTR_MODIFIER uint16_t* pu16 = (HMDRV_ADDR_PNTR_MODIFIER uint16_t*)SPI_pu8CurrentBufRD[eModuleSPI];
                    *pu16++ = u16Data;
                    SPI_pu8CurrentBufRD[eModuleSPI] = (HMDRV_ADDR_PNTR_MODIFIER uint8_t*)pu16;
                }
                else
                {
                    *SPI_pu8CurrentBufRD[eModuleSPI]++ = (uint8_t)u16Data;
                }
                SPI_u16CurrentCntRD[eModuleSPI]--;
                if (SPI_u16CurrentCntRD[eModuleSPI] == 0)
                {
                    if (SPI_psCurrentHandle[eModuleSPI] != NULL)
                    {
                        SPI_psCurrentHandle[eModuleSPI]->eTransferStatus = SPI_TRANSF_STA_COMPLETED;
                        
                        if (SPI_psCurrentHandle[eModuleSPI]->peTransferStatus != NULL)
                        {
                            *(SPI_psCurrentHandle[eModuleSPI]->peTransferStatus) = SPI_TRANSF_STA_COMPLETED;
                        }
                    }
                    if (SPI_pfChipSelectDisable[eModuleSPI] != NULL)
                    {
                        SPI_pfChipSelectDisable[eModuleSPI](SPI_psCurrentHandle[eModuleSPI]->eMemoryIndex);
                    }
                    if (SPI_pfTransferCompleted[eModuleSPI] != NULL)
                    {
                        SPI_pfTransferCompleted[eModuleSPI](SPI_psCurrentHandle[eModuleSPI]->u16TransactionID);
                    }
                            
                }
            }
        }
    }
        
    if ((SPI_psCurrentHandle[eModuleSPI] == NULL) || (SPI_u16CurrentCntWR[eModuleSPI] == 0))
    {
        //transfer finished - try start new
        if((SPI_sObject[eModuleSPI].trStatus.s.empty != true) && (SPI_sObject[eModuleSPI].pTrHead->eTransferStatus == SPI_TRANSF_STA_PENDING))
        {
            // grab the item pointed by the head
            SPI_psCurrentHandle[eModuleSPI] = SPI_sObject[eModuleSPI].pTrHead;
            SPI_u16CurrentCntWR[eModuleSPI] = SPI_sObject[eModuleSPI].pTrHead->u16TransfersCount;
            SPI_pu8CurrentBufWR[eModuleSPI] = SPI_sObject[eModuleSPI].pTrHead->pu8DataBuffer;
            SPI_u16CurrentCntRD[eModuleSPI] = SPI_sObject[eModuleSPI].pTrHead->u16TransfersCount;
            SPI_pu8CurrentBufRD[eModuleSPI] = SPI_sObject[eModuleSPI].pTrHead->pu8DataBuffer;

            #if SPI_COUNT_TRANSFER_HANDLES > 1 
            // check if the end of the array is reached
            if ((SPI_sObject[eModuleSPI].pTrHead+1) == (SPI_sTransferHandle[eModuleSPI] + SPI_COUNT_TRANSFER_HANDLES))
            {
                // adjust to restart at the beginning of the array
                SPI_sObject[eModuleSPI].pTrHead = SPI_sTransferHandle[eModuleSPI];
            }
            else
            {
                SPI_sObject[eModuleSPI].pTrHead++;
            }
            // check if the queue is empty
            if(SPI_sObject[eModuleSPI].pTrHead == SPI_sObject[eModuleSPI].pTrTail)
            #endif
            {
                // it is empty so set the empty status to true
                SPI_sObject[eModuleSPI].trStatus.s.empty = true;
            }

            // since we moved one item to be processed, we know
            // it is not full, so set the full status to false
            SPI_sObject[eModuleSPI].trStatus.s.full = false;

            if (SPI_pfChipSelectEnable[eModuleSPI] != NULL)
            {
                SPI_pfChipSelectEnable[eModuleSPI](SPI_psCurrentHandle[eModuleSPI]->eMemoryIndex);
            }
        }
        else
        {
            //queue empty or has to wait (next element not ready for transmit)
            ISR_vSetBitIEC(SPI_sBitOffsetsISR.u8BitOffsetsISRTrnsf[eModuleSPI]);    // disable interrupt
            SPI_psCurrentHandle[eModuleSPI] = NULL;
        }
    }
    
    /* Fill Data For Transmit */
    if (SPI_psCurrentHandle[eModuleSPI] != NULL)
    {
        while((SPI_sHandles[eModuleSPI].psBase->SPISTATbits.SPITBF) && (SPI_u16CurrentCntWR[eModuleSPI] > 0))
        {
            // spi send
            if (SPI_sHandles[eModuleSPI].psBase->SPICON1bits.MODE16)
            {
                HMDRV_ADDR_PNTR_MODIFIER uint16_t* pu16 = (HMDRV_ADDR_PNTR_MODIFIER uint16_t*)SPI_pu8CurrentBufWR[eModuleSPI];
                SPI_sHandles[eModuleSPI].psBase->SPIBUF = *pu16++;
                SPI_pu8CurrentBufWR[eModuleSPI] = (HMDRV_ADDR_PNTR_MODIFIER uint8_t*)pu16;
            }
            else
            {
                SPI_sHandles[eModuleSPI].psBase->SPIBUF = *SPI_pu8CurrentBufWR[eModuleSPI]++;
            }
            SPI_u16CurrentCntWR[eModuleSPI]--;
        }
    }
        
    
}
