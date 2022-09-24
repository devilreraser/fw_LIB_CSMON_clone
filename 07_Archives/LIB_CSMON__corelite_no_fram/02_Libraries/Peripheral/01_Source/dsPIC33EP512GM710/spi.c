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
#include "pin_macro.h"

#if SPI_USE_DMA
#include "dma_drv.h"
#endif

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define SPI_COUNT_TRANSFER_HANDLES  4       /* Queue */
#define SPI_FIFO_DEPTH              8

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
            volatile uint16_t SPISTAT;
            volatile uint16_t SPICON1;
            volatile uint16_t SPICON2;
            uint16_t :16;
            volatile uint16_t SPIBUF;
        };
        
        struct
        {
            volatile SPI1STATBITS SPISTATbits;
            volatile SPI1CON1BITS SPICON1bits;
            volatile SPI1CON2BITS SPICON2bits;
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
    uint8_t* pu8DataBuffer;
    uint16_t u16TransfersCount;
    SPI_eMemoryIndex_t eMemoryIndex;
    volatile SPI_eTransferStatus_t* peTransferStatus;
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

typedef struct
{
    /* following needed to recover from error */
    uint8_t*               pu8CurrentBufWRBackup; /* Write Data To SPI */
    uint16_t                                        u16CurrentCntWRBackup;
    uint8_t*               pu8CurrentBufRDBackup; /* Read Data From SPI */
    uint16_t                                        u16CurrentCntRDBackup;
    
    uint8_t*               pu8CurrentBufWR; /* Write Data To SPI */
    uint16_t                                        u16CurrentCntWR;
    uint8_t*               pu8CurrentBufRD; /* Read Data From SPI */
    uint16_t                                        u16CurrentCntRD;
    bool                                            bProcessing;
    bool                                            bSwapBytes;
    volatile SPI_eTransferStatus_t* peStatus;
    //volatile SPI_eTransferStatus_t* peStatusHandle;
    SPI_eMemoryIndex_t                              eMemoryIndex;
    SPI_eTransferStatus_t                           eStatus;
    uint16_t                                        u16TransactionID;
    
}SPI_sCurrentTransfer_t;



/******************************************************************************
 * Macros Below Need Rework For Compatibility
 *****************************************************************************/

#define SPI1_CS_DisableAll() \
{ \
    CS_EEPROM_SetHigh(); \
    CS_RAM_SetHigh(); \
} \

#define SPI1_CS_Enable(eMemoryIndex, eMode16Bit) \
if(eMemoryIndex == SPI_MEM_EEPROM) \
{ \
    SPI1STATbits.SPIEN = 0x0; /* disable the SPI module */ \
    SPI1CON1bits.SPRE = 0;    /* must not have prescalers both 1:1  */ \
    SPI1CON1bits.PPRE = ePriDividerEeprom; \
    SPI1CON1bits.SPRE = eSecDividerEeprom; \
    SPI1CON1bits.MODE16 = eMode16Bit; \
    SPI1STATbits.SPIEN = 0x1; /* enable the SPI module */ \
    CS_EEPROM_SetLow(); \
} \
else \
{ \
    SPI1STATbits.SPIEN = 0x0; /* disable the SPI module */ \
    SPI1CON1bits.SPRE = 0;    /* must not have prescalers both 1:1  */ \
    SPI1CON1bits.PPRE = ePriDividerRam; \
    SPI1CON1bits.SPRE = eSecDividerRam; \
    SPI1CON1bits.MODE16 = eMode16Bit; \
    SPI1STATbits.SPIEN = 0x1; /* enable the SPI module */ \
    CS_RAM_SetLow();\
} \

#define SPI1_CS_EnableSameMode(eMemoryIndex) \
if(eMemoryIndex == SPI_MEM_EEPROM) \
{ \
    SPI1STATbits.SPIEN = 0x0; /* disable the SPI module */ \
    SPI1CON1bits.SPRE = 0;    /* must not have prescalers both 1:1  */ \
    SPI1CON1bits.PPRE = ePriDividerEeprom; \
    SPI1CON1bits.SPRE = eSecDividerEeprom; \
    SPI1STATbits.SPIEN = 0x1; /* enable the SPI module */ \
    CS_EEPROM_SetLow(); \
} \
else \
{ \
    SPI1STATbits.SPIEN = 0x0; /* disable the SPI module */ \
    SPI1CON1bits.SPRE = 0;    /* must not have prescalers both 1:1  */ \
    SPI1CON1bits.PPRE = ePriDividerRam; \
    SPI1CON1bits.SPRE = eSecDividerRam; \
    SPI1STATbits.SPIEN = 0x1; /* enable the SPI module */ \
    CS_RAM_SetLow();\
} \


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

volatile SPI_sCurrentTransfer_t SPI_sCurrentTransfer[SPI_COUNT];


uint16_t SPI_u16Error = 0;

bool SPI_bDisableChipSelects[SPI_COUNT] = SPI_NULL_INIT;

SPI_pfVoid_t            SPI_pfChipSelectDisableAll[SPI_COUNT] = SPI_NULL_INIT;

SPI_pfVoid_u16_t        SPI_pfChipSelectEnable[SPI_COUNT] = SPI_NULL_INIT;
SPI_pfVoid_u16_t        SPI_pfChipSelectDisable[SPI_COUNT] = SPI_NULL_INIT;
SPI_pfVoid_u16_t        SPI_pfTransferCompleted[SPI_COUNT] = SPI_NULL_INIT;

SPI_pfVoid_u16_t        SPI_pfISRTransferComplete[SPI_COUNT] = SPI_NULL_INIT;
SPI_pfVoid_u16_t        SPI_pfISRError[SPI_COUNT] = SPI_NULL_INIT;

uint8_t u8HardwareFifoDepth = 1;


SPI_eModule_t SPI_eExtMemSPIModule = EXT_MEM_SPI_MODULE;



uint16_t u16SPIPrescalerRam = SPI_PRESCALER(SPI_RAM_HZ);
uint16_t u16SPIPrescalerEeprom = SPI_PRESCALER(SPI_EEPROM_HZ);
#if USE_SPI_FLIR
uint16_t u16SPIPrescalerFlir = SPI_PRESCALER(SPI_FLIR_HZ);
#endif

SPI_ePrimaryDivider_t      ePriDividerRam = SPI_PRIMARY_DIVIDER_1_DIV_1;
SPI_eSecondaryDivider_t    eSecDividerRam = SPI_SECONDARY_DIVIDER_1_DIV_2;

SPI_ePrimaryDivider_t   ePriDividerEeprom = SPI_PRIMARY_DIVIDER_1_DIV_4;
SPI_eSecondaryDivider_t eSecDividerEeprom = SPI_SECONDARY_DIVIDER_1_DIV_3;

#if USE_SPI_FLIR
SPI_ePrimaryDivider_t   ePriDividerFlir = SPI_PRIMARY_DIVIDER_1_DIV_1;
SPI_eSecondaryDivider_t eSecDividerFlir = SPI_SECONDARY_DIVIDER_1_DIV_3;
#endif


/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */
void SPI_vErrorProcess(SPI_eModule_t index);
void SPI_vInitPinout (SPI_eModule_t index);

void SPI_vExtMemChipSelectEnable(uint16_t u16Memory);
void SPI_vExtMemChipSelectDisableAll(void);
void SPI_vExtMemChipSelectDisable(uint16_t u16Memory);



void SPI1_vQueueProcess(void);


/* *****************************************************************************
 * Functions definitions
 **************************************************************************** */


/* *****************************************************************************
 * ISR Functions
 **************************************************************************** */
void __attribute__ ( (interrupt, auto_psv) ) _SPI1Interrupt( void )
{
    DEBUG_SET_SPI1();
    /* write optional extern function call here after register in the module */
    IFS0bits.SPI1IF = 0;
    
#if 1
    if (SPI_bDisableChipSelects[SPI_1])
    {
        while((SPI1STATbits.SRXMPT == 0) && (SPI1STATbits.SRMPT == 0))
        {
            SPI1BUF;
        }

        SPI_bDisableChipSelects[SPI_1] = 0;
        SPI1_CS_DisableAll();
    }
    SPI_vQueueProcess(SPI_1);
    //SPI1_vQueueProcess(); /not made fast read while writing/
#else
    
    if (SPI_bDisableChipSelects[SPI_1])
    {
        SPI_bDisableChipSelects[SPI_1] = 0;
        SPI_pfChipSelectDisableAll[SPI_1]();
    }
    if (SPI_pfISRTransferComplete[SPI_1] != NULL)
    {
        SPI_pfISRTransferComplete[SPI_1](SPI_1);
    }
#endif
    DEBUG_CLR_SPI1();
}

void __attribute__ ( (interrupt, auto_psv) ) _SPI2Interrupt( void )
{
    /* write optional extern function call here after register in the module */
    IFS2bits.SPI2IF = 0;
    if (SPI_bDisableChipSelects[SPI_2])
    {
        SPI_bDisableChipSelects[SPI_2] = 0;
        SPI_pfChipSelectDisableAll[SPI_2]();
    }
    
    if (SPI_pfISRTransferComplete[SPI_2] != NULL)
    {
        SPI_pfISRTransferComplete[SPI_2](SPI_2);
    }
}

void __attribute__ ( (interrupt, auto_psv) ) _SPI3Interrupt( void )
{
    /* write optional extern function call here after register in the module */
    IFS5bits.SPI3IF = 0;
    if (SPI_bDisableChipSelects[SPI_3])
    {
        SPI_bDisableChipSelects[SPI_3] = 0;
        SPI_pfChipSelectDisableAll[SPI_3]();
    }
    
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
    if (SPI_bDisableChipSelects[SPI_4])
    {
        SPI_bDisableChipSelects[SPI_4] = 0;
        SPI_pfChipSelectDisableAll[SPI_4]();
    }
    
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
    volatile SPI_sBaseRegisters_t* psBase = SPI_sHandles[index].psBase;
    
    SPI_u16Error++;
    
#define REVERSE_QUEUE 0    
#if REVERSE_QUEUE == 0
    if (SPI_pfChipSelectDisable[index] != NULL)
    {
        SPI_pfChipSelectDisable[index](SPI_sCurrentTransfer[index].eMemoryIndex);
    }
#endif
    /* reset SPI Module */
    psBase->SPISTATbits.SPIEN = 0;
    psBase->SPISTATbits.SPIEN = 1;
    ISR_vClrBitIEC(SPI_sBitOffsetsISR.u8BitOffsetsISRTrnsf[index]);    // reset interrupt - check enough to enter ISR
    ISR_vClrBitIFS(SPI_sBitOffsetsISR.u8BitOffsetsISRTrnsf[index]);    // reset  flag force enter ISR
    
#if REVERSE_QUEUE
    SPI_sCurrentTransfer[index].u16CurrentCntRD = 0;
    SPI_sCurrentTransfer[index].u16CurrentCntWR = 0;
    SPI_sCurrentTransfer[index].bProcessing = 0;
    
    /* Reverse the head and re-send - consider not to fill too fast to overwrite with new data */
    #if SPI_COUNT_TRANSFER_HANDLES > 1 
    // check if the end of the array is reached
    if ((SPI_sObject[index].pTrHead) == SPI_sTransferHandle[index])
    {
        // adjust to restart at the beginning of the array
        SPI_sObject[index].pTrHead = SPI_sTransferHandle[index] + (SPI_COUNT_TRANSFER_HANDLES - 1);
    }
    else
    {
        SPI_sObject[index].pTrHead--;
    }
    #endif
    SPI_sObject[index].trStatus.s.empty = false;
#else
    SPI_sCurrentTransfer[index].u16CurrentCntRD = SPI_sCurrentTransfer[index].u16CurrentCntRDBackup;
    SPI_sCurrentTransfer[index].pu8CurrentBufRD = SPI_sCurrentTransfer[index].pu8CurrentBufRDBackup;
    SPI_sCurrentTransfer[index].u16CurrentCntWR = SPI_sCurrentTransfer[index].u16CurrentCntWRBackup;
    SPI_sCurrentTransfer[index].pu8CurrentBufWR = SPI_sCurrentTransfer[index].pu8CurrentBufWRBackup;
    
    /* consider enough or not for CS toggle */
    if (SPI_pfChipSelectEnable[index] != NULL)
    {
        SPI_pfChipSelectEnable[index](SPI_sCurrentTransfer[index].eMemoryIndex);
    }
    
#endif

    ISR_vSetBitIEC(SPI_sBitOffsetsISR.u8BitOffsetsISRTrnsf[index]);    // enable interrupt - check enough to enter ISR
    ISR_vSetBitIFS(SPI_sBitOffsetsISR.u8BitOffsetsISRTrnsf[index]);    // set  flag force enter ISR
 
    
    
    
    
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
            RPOR14bits.RP125R = 6;  //RP125/RG13 as SCK1
            RPOR15bits.RP126R = 5;  //RP126/RG14 as SDO1
            RPINR20bits.SDI1R = 124;//RP124/RG12 as SDI1
            #endif
            __builtin_write_OSCCONL(OSCCON | (1<<6)); 	 // Set bit 6 	
            break;
        case SPI_2:
            __builtin_write_OSCCONL(OSCCON & (~(1<<6))); // clear bit 6 
            #if defined __dsPIC33EP512GM710__
            _RP70R = 0b001000;      //RP70/RD6 as SDO2
            _RP57R = 0b001001;      //RP57/RC9 as SCK2
            _SCK2R = 57;            //RP57/RC9 as SCK2
            _SDI2R = 69;            //RP69/RD5 as SDI2
            #endif
            __builtin_write_OSCCONL(OSCCON | (1<<6)); 	 // Set bit 6 	
            break;
        case SPI_3:
            __builtin_write_OSCCONL(OSCCON & (~(1<<6))); // clear bit 6 
            #if defined __dsPIC33EP512GM710__
            //_RPXXR = 0b011111;    //RPXX/R?? as SDO3
            //_RPXXR = 0b100000;    //RPXX/R?? as SCK3
            //_SCK3R = XX;          //RPXX/R?? as SCK3
            //_SDI3R = XX;          //RPXX/R?? as SDI3
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
    //psBase->SPISTAT = 0x0;                             // disable the SPI module (just in case)
    psBase->SPISTATbits.SPIEN = 0;                     // disable the SPI module
    
    
    psBase->SPICON1bits.DISSDO = 0;
    psBase->SPICON1bits.MODE16 = 0;                          
    psBase->SPICON1bits.SSEN = 0;
    psBase->SPICON1bits.MSTEN = 1; 
    
    /* Do not set both SPRE and PPRE to 1:2 - Fp=60MHz (Fcy=60MHz; Fp=Fcy) */
    //psBase->SPICON1bits.SPRE = 0b111;                  //(1:1)
    //psBase->SPICON1bits.SPRE = 0b110;                //(1:2)       /* 1:2 -> 30MHz SPI Clock */
    //psBase->SPICON1bits.PPRE = 0b11;                   //(1:1)
    psBase->SPICON1bits.SPRE = 0b000;                //(1:2)       /* 1:2 -> 30MHz SPI Clock */
    psBase->SPICON1bits.PPRE = 0b00;                   //(1:1)
    
    
    psBase->SPICON1bits.SMP = 0x00;                    /* set if needed only after MSTEN=1 */
    psBase->SPICON1bits.CKE = ((~((SPI_DEFAULT_MODE & 0x02) >> 1)) & 0x01);  /* CKE bit is inverted CPHA */
    psBase->SPICON1bits.CKP = (SPI_DEFAULT_MODE & 0x01);                     /* CKP bit is equal to CPOL */
    psBase->SPICON2 = 0;                               // FRAMEN = 0, SPIFSD = 0, SPIBEN = 0
    
    if (eUseSPIDMA == 1)
    {
        psBase->SPISTATbits.SISEL = 0b000;                 /* interrupt when the last data in the receive buffer is read and the buffer is empty (SRXMPT == 1) */
        psBase->SPICON2bits.SPIBEN = 0;                    /* enhanced buffer disabled */
    }
    else
    {
        //psBase->SPISTATbits.SISEL = 0b000;                 /* Interrupt when the last data in the receive buffer is read and the buffer is empty (SRXMPT == 1) */
        psBase->SPISTATbits.SISEL = 0b101;                 /* Interrupt when the last bit is shifted out and the transmit is complete */
        //psBase->SPISTATbits.SISEL = 0b110;                 /* Interrupt when the last bit is shifted into SPIxSR, and as a result, the TX FIFO is empty */
        psBase->SPICON2bits.SPIBEN = 1;                    /* enhanced buffer enable */        
    }
    
    
    psBase->SPISTATbits.SPIEN = 1;                     // enable the SPI module
    
    if (psBase->SPICON2bits.SPIBEN == 1)
    {
        u8HardwareFifoDepth = SPI_FIFO_DEPTH-1; //do not relay on last fill position SPIBEC 3 bits
    }
    
    SPI_vInitPinout(index);

    //SPI interrupts enable/setup
    ISR_vSetDatIPC(SPI_sBitOffsetsISR.u8BitOffsetsISRTrnsf[index],ISR_PRIORITY_SPITRANS);    // interrupt priority level
    ISR_vSetDatIPC(SPI_sBitOffsetsISR.u8BitOffsetsISRError[index],ISR_PRIORITY_SPIERROR);    // interrupt priority level
    ISR_vClrBitIFS(SPI_sBitOffsetsISR.u8BitOffsetsISRTrnsf[index]);    // Clear interrupt
    ISR_vClrBitIEC(SPI_sBitOffsetsISR.u8BitOffsetsISRTrnsf[index]);    // disable interrupt
    ISR_vClrBitIFS(SPI_sBitOffsetsISR.u8BitOffsetsISRError[index]);    // Clear interrupt
    ISR_vSetBitIEC(SPI_sBitOffsetsISR.u8BitOffsetsISRError[index]);    // enable interrupt
    
    SPI_vQueueInitialize(index);
    
    /* to do fix this used only if ext mem spi used */
    if (index == SPI_eExtMemSPIModule)
    {
    SPI_vChipSelectFunctionsInit(SPI_eExtMemSPIModule, SPI_vExtMemChipSelectEnable, SPI_vExtMemChipSelectDisable, SPI_vExtMemChipSelectDisableAll );
    }


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
    psBase->SPICON1bits.CKE = ((~((eMode & 0x02) >> 1)) & 0x01);  /* CKE bit is inverted CPHA */
    psBase->SPICON1bits.CKP = (eMode & 0x01);                     /* CKP bit is equal to CPOL */   
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
    psBase->SPISTATbits.SPIEN = 0x0;                    // disable the SPI module
    psBase->SPICON1bits.MODE16 = eMode;     
    psBase->SPISTATbits.SPIEN = 0x1;                    // enable the SPI module
}

/* *****************************************************************************
 * SPI_vSetFrequencyDivider
 * 
 * Description: SPI_vSetFrequencyDivider 
 **************************************************************************** */
void SPI_vSetFrequencyDivider( SPI_eModule_t index, SPI_eMemoryIndex_t eMemoryIndex)
{
    SPI_sBaseRegisters_t* psBase = SPI_sHandles[index].psBase;

    psBase->SPISTATbits.SPIEN = 0x0;                    // disable the SPI module
    psBase->SPICON1bits.SPRE = 0;    /* must not have prescalers both 1:1  */
    switch(eMemoryIndex)
    {
        case SPI_MEM_EEPROM:
            psBase->SPICON1bits.PPRE = ePriDividerEeprom; 
            psBase->SPICON1bits.SPRE = eSecDividerEeprom;   
            break;
        case SPI_MEM_RAM:
            psBase->SPICON1bits.PPRE = ePriDividerRam; 
            psBase->SPICON1bits.SPRE = eSecDividerRam;   
            break;
#if USE_SPI_FLIR
        case SPI_MEM_FLIR:
            psBase->SPICON1bits.PPRE = ePriDividerFlir; 
            psBase->SPICON1bits.SPRE = eSecDividerFlir;   
            break;
#endif
#if 0
        case SPI_MEM_FPGA:
            psBase->SPICON1bits.PPRE = ePriDividerFpga; 
            psBase->SPICON1bits.SPRE = eSecDividerFpga;   
            break;
#endif
        default:
            psBase->SPICON1bits.PPRE = 0; /* default min */
            break;
    }
    psBase->SPISTATbits.SPIEN = 0x1;                    // enable the SPI module
}

/* *****************************************************************************
 * SPI_vSetFrequency
 * 
 * Description: SPI_vSetFrequency
 **************************************************************************** */
void SPI_vSetFrequency( SPI_eModule_t index, uint32_t u32SPIFreqHz, uint32_t u32Fcy)
{
    SPI_sBaseRegisters_t* psBase = SPI_sHandles[index].psBase;
    uint16_t prescallerRequest;
    uint16_t prescallerCalc;
    SPI_ePrimaryDivider_t ePriDivider;
    SPI_eSecondaryDivider_t eSecDivider;
    uint16_t primaryPre = 1;
    
    //*u16SPIPrescaler = (uint16_t)(((u32Fcy + (u32SPIFreqHz/2))/ u32SPIFreqHz));   /* allow 50 % frequency overflow */
    prescallerCalc = (uint16_t)(((u32Fcy + (u32SPIFreqHz-1))/ u32SPIFreqHz));/* not possible frequency overflow */
    prescallerRequest = prescallerCalc;
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
        eSecDivider = 8 - prescallerRequest;    
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
    
    
    ePriDivider = 4 - primaryPre;
    eSecDivider = 8 - prescallerRequest;   


    psBase->SPISTATbits.SPIEN = 0x0;                    // disable the SPI module
    psBase->SPICON1bits.SPRE = 0;    /* must not have prescalers both 1:1  */
    psBase->SPICON1bits.PPRE = ePriDivider; 
    psBase->SPICON1bits.SPRE = eSecDivider;   
    psBase->SPISTATbits.SPIEN = 0x1;                    // enable the SPI module
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
 * Function:       SPI_vInitializeSPIPrescaler
 *
 * Overview:       Calculates u16SPIPrescaler from u32SPIFreqHz and u32Fcy
 * After That Calculates 
 * eSPIPrimaryDivider and eSPISecondaryDivider from prescallerRequest
 *****************************************************************************/
void SPI_vInitializeSPIPrescaler(SPI_eMemoryIndex_t eMemoryIndex, uint32_t u32SPIFreqHz, uint32_t u32Fcy)
{
    uint16_t prescallerRequest;
    uint16_t prescallerCalc;
    SPI_ePrimaryDivider_t ePriDivider;
    SPI_eSecondaryDivider_t eSecDivider;
    uint16_t primaryPre = 1;
    
    //*u16SPIPrescaler = (uint16_t)(((u32Fcy + (u32SPIFreqHz/2))/ u32SPIFreqHz));   /* allow 50 % frequency overflow */
    prescallerCalc = (uint16_t)(((u32Fcy + (u32SPIFreqHz-1))/ u32SPIFreqHz));/* not possible frequency overflow */
    prescallerRequest = prescallerCalc;
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
        eSecDivider = 8 - prescallerRequest;    
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
    
    
    ePriDivider = 4 - primaryPre;
    eSecDivider = 8 - prescallerRequest;   

    switch (eMemoryIndex)
    {
        case SPI_MEM_EEPROM: 
            u16SPIPrescalerEeprom = prescallerCalc;
            ePriDividerEeprom = ePriDivider;
            eSecDividerEeprom = eSecDivider;
            break;
        case SPI_MEM_RAM: 
            u16SPIPrescalerRam = prescallerCalc;
            ePriDividerRam = ePriDivider;
            eSecDividerRam = eSecDivider;
            break;
#if USE_SPI_FLIR
        case SPI_MEM_FLIR: 
            u16SPIPrescalerFlir = prescallerCalc;
            ePriDividerFlir = ePriDivider;
            eSecDividerFlir = eSecDivider;
            break;
#endif
#if USE_SPI_FPGA
        case SPI_MEM_FPGA: 
            to do...
            break;
#endif
        default: 
            break;
    }
    
}


/* *****************************************************************************
 * SPI_vExtMemChipSelectEnable - SPI Master Chip Select Enable - Used From SPI ISR
 **************************************************************************** */
void SPI_vExtMemChipSelectEnable(uint16_t u16Memory)
{
    SPI_vSetFrequencyDivider(SPI_eExtMemSPIModule,u16Memory);
    switch((SPI_eMemoryIndex_t)u16Memory)
    {
        #if USE_SPI_EEPROM
        case SPI_MEM_EEPROM:
            CS_EEPROM_SetLow(); /* SPI_CHIP_SELECT_ENABLE */
            break;
        #endif
        #if USE_SPI_RAM
        case SPI_MEM_RAM:
            CS_RAM_SetLow(); /* SPI_CHIP_SELECT_ENABLE */
            break;
        #endif
        #if USE_SPI_FLIR
        case SPI_MEM_FLIR:
            CS_FLIR_SetLow(); /* SPI_CHIP_SELECT_ENABLE */
            break;
        #endif
        #if USE_SPI_FPGA
        case SPI_MEM_FPGA:
            CS_FPGA_SetLow(); /* SPI_CHIP_SELECT_ENABLE */
            break;
        #endif
        default:
            /* do nothing - assume error request */
            break;
    }
}
/* *****************************************************************************
 * SPI_vExtMemChipSelectDisable - SPI Master Chip Select Enable - Used From SPI ISR
 **************************************************************************** */
void SPI_vExtMemChipSelectDisableAll(void)
{
    #if USE_SPI_EEPROM
    CS_EEPROM_SetHigh(); /* SPI_CHIP_SELECT_DISABLE */
    #endif  
    #if USE_SPI_RAM
    CS_RAM_SetHigh(); /* SPI_CHIP_SELECT_DISABLE */
    #endif
    #if USE_SPI_FLIR
    CS_FLIR_SetHigh(); /* SPI_CHIP_SELECT_DISABLE */
    #endif
    #if USE_SPI_FPGA
    CS_FPGA_SetHigh(); /* SPI_CHIP_SELECT_DISABLE */
    #endif
}
/* *****************************************************************************
 * SPI_vExtMemChipSelectDisable - SPI Master Chip Select Enable - Used From SPI ISR
 **************************************************************************** */
void SPI_vExtMemChipSelectDisable(uint16_t u16Memory)
{
    switch((SPI_eMemoryIndex_t)u16Memory)
    {
        #if USE_SPI_EEPROM
        case SPI_MEM_EEPROM:
            CS_EEPROM_SetHigh(); /* SPI_CHIP_SELECT_DISABLE */
            break;
        #endif
        #if USE_SPI_RAM
        case SPI_MEM_RAM:
            CS_RAM_SetHigh(); /* SPI_CHIP_SELECT_DISABLE */
            break;
        #endif
        #if USE_SPI_FLIR
        case SPI_MEM_FLIR:
            CS_FLIR_SetHigh(); /* SPI_CHIP_SELECT_DISABLE */
            break;
        #endif
        #if USE_SPI_FPGA
        case SPI_MEM_FPGA:
            CS_FPGA_SetHigh(); /* SPI_CHIP_SELECT_DISABLE */
            break;
        #endif
        default:
            /* disable all */
            #if USE_SPI_EEPROM
            CS_EEPROM_SetHigh(); /* SPI_CHIP_SELECT_DISABLE */
            #endif  
            #if USE_SPI_RAM
            CS_RAM_SetHigh(); /* SPI_CHIP_SELECT_DISABLE */
            #endif
            #if USE_SPI_FLIR
            CS_FLIR_SetHigh(); /* SPI_CHIP_SELECT_DISABLE */
            #endif
            #if USE_SPI_FPGA
            CS_FPGA_SetHigh(); /* SPI_CHIP_SELECT_DISABLE */
            #endif
            break;
    }
}




/******************************************************************************
 * SPI Read/Write Simple (in main loop)
 *****************************************************************************/



/******************************************************************************
 * Function:       SPI_GetFifoFill
 *
 * Overview:       Read SPI Buffer Fill
 *****************************************************************************/
uint16_t SPI_GetFifoFill ( SPI_eModule_t eModuleSPI )
{
    uint16_t u16Result;
    u16Result = SPI_sHandles[eModuleSPI].psBase->SPISTATbits.SPIBEC;
    return u16Result;
}




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


#if 1
/******************************************************************************
 * Function:       SPI1_WriteBuffer
 *
 * Overview:       Write Data Buffer To SPI As Fast As Possible
 *****************************************************************************/
void SPI_WriteBuffer( SPI_eModule_t eModuleSPI, uint8_t* pu8Data, uint16_t u16BytesCount)
{
    uint16_t u16Data;
    bool bCritical = false;
    bool bSwapNeed = false;
    SPI_eModeBitsCount_t eMode16Bit = 0;
    uint16_t* pu16;
    SPI_eMemoryIndex_t eMemoryIndex = ((eModuleSPI >> 4) & 0x000F);     /* u16TransactionID also contains eMemoryIndex */
    eModuleSPI &= 0x000F;
    
    // assure interrupt is not pending and wait complete if needed
    do
    {
        if (bCritical)
        {
            ISR_vExitCritical();
            bCritical = false;
        }
        if (SPI_sCurrentTransfer[eModuleSPI].bProcessing == 0)
        {
            ISR_vEnterCritical();
            bCritical = true;
        }
        
    } while (SPI_sCurrentTransfer[eModuleSPI].bProcessing);
    

    if (eModuleSPI == SPI_eExtMemSPIModule)
    {
        #if SPI_USE_MIXED_8_16_BIT_MODE

        eMode16Bit = (SPI_eModeBitsCount_t)((u16BytesCount+1) & 0x0001);
        bSwapNeed = 1;
#if 0
        if (eMode16Bit)
        {
            uint16_t u16Index;
            uint16_t u16Temp;
            for (u16Index = 0; u16Index < u16BytesCount; u16Index += 2)
            {
                u16Temp = pu8Data[u16Index];
                pu8Data[u16Index] = pu8Data[u16Index+1];
                pu8Data[u16Index+1] = u16Temp;
            }
        }
#endif
        #else
        eMode16Bit = 0; /* consider to use default initialization mode instead of force 8-bit */
        #endif
    }
    else
    {
        
    }

    if (eModuleSPI > SPI_1)
    {
        if (eModuleSPI == SPI_eExtMemSPIModule)
        {
            SPI_vSetModeBitsCount(eModuleSPI, eMode16Bit);
        }
        if (SPI_pfChipSelectEnable[eModuleSPI] != NULL)
        {
            SPI_pfChipSelectEnable[eModuleSPI](eMemoryIndex);
        }
    }
    else
    {
        if (eModuleSPI == SPI_eExtMemSPIModule)
        {
            SPI1_CS_Enable(eMemoryIndex, eMode16Bit);
        }
        else
        {
            SPI1_CS_EnableSameMode(eMemoryIndex);
        }
    }
    
    
    
    switch (eModuleSPI)
    {
        case SPI_1:        /* used for ExtMemory */
            if (SPI1CON1bits.MODE16)
            {
                pu16 = (uint16_t*)pu8Data;
                u16BytesCount = (u16BytesCount + 1) >> 1;       /* fix to transfer count */
                while(u16BytesCount-- > 0)
                {
                    while(SPI1STATbits.SPITBF){;}
                    u16Data = *pu16++;
                    if (bSwapNeed)
                    {
                        SPI1BUF = (u16Data >> 8) | (u16Data << 8);
                    }
                    else
                    {
                        SPI1BUF = u16Data;
                    }
                    
                    
                }
            }
            else
            {
                DEBUG_SET_SPI1();
                 while(u16BytesCount-- > 0)
                {
                    while(SPI1STATbits.SPITBF){;}
                    SPI1BUF = *pu8Data++; 
                }
                DEBUG_CLR_SPI1();
            }
            if (eMemoryIndex == SPI_MEM_RAM)        /* CS Disable Now */
            {
                while ((SPI1STATbits.SRXMPT == 0) || (SPI1STATbits.SRMPT == 0))
                {
                    SPI1BUF;
                }
                SPI1_CS_DisableAll();
            }
            else
            {
                SPI_bDisableChipSelects[SPI_1] = true;
                _SPI1IE = 1;    /* on interrupt - disable CS */
            }
            break;
            
        case SPI_2:
            if (SPI2CON1bits.MODE16)
            {
                pu16 = (uint16_t*)pu8Data;
                u16BytesCount = (u16BytesCount + 1) >> 1;       /* fix to transfer count */
                while(u16BytesCount-- > 0)
                {
                    while(SPI2STATbits.SPITBF){;}
                    u16Data = *pu16++;
                    if (bSwapNeed)
                    {
                        SPI2BUF = (u16Data >> 8) | (u16Data << 8);
                    }
                    else
                    {
                        SPI2BUF = u16Data;
                    }
                }
            }
            else
            {
                while(u16BytesCount-- > 0)
                {
                    while(SPI3STATbits.SPITBF){;}
                    SPI2BUF = *pu8Data++; 
                }
            }
            break;
            
        case SPI_3:
            if (SPI3CON1bits.MODE16)
            {
                pu16 = (uint16_t*)pu8Data;
                u16BytesCount = (u16BytesCount + 1) >> 1;       /* fix to transfer count */
                while(u16BytesCount-- > 0)
                {
                    while(SPI3STATbits.SPITBF){;}
                    u16Data = *pu16++;
                    if (bSwapNeed)
                    {
                        SPI3BUF = (u16Data >> 8) | (u16Data << 8);
                    }
                    else
                    {
                        SPI3BUF = u16Data;
                    }
                }
            }
            else
            {
                while(u16BytesCount-- > 0)
                {
                    while(SPI3STATbits.SPITBF){;}
                    SPI3BUF = *pu8Data++; 
                }
            }
            break;
            
        #if defined __dsPIC33EP512MU814__
        case SPI_4:
            if (SPI4CON1bits.MODE16)
            {
                pu16 = (uint16_t*)pu8Data;
                u16BytesCount = (u16BytesCount + 1) >> 1;       /* fix to transfer count */
                while(u16BytesCount-- > 0)
                {
                    while(SPI4STATbits.SPITBF){;}
                    u16Data = *pu16++;
                    if (bSwapNeed)
                    {
                        SPI4BUF = (u16Data >> 8) | (u16Data << 8);
                    }
                    else
                    {
                        SPI4BUF = u16Data;
                    }
                }
            }
            else
            {
                while(u16BytesCount-- > 0)
                {
                    while(SPI4STATbits.SPITBF){;}
                    SPI4BUF = *pu8Data++; 
                }
            }
            break;
        #endif

        default:
            break;
    }
    
    
#if 0   /* made in isr */
    if (SPI_pfChipSelectDisable[eModuleSPI] != NULL)
    {
        SPI_pfChipSelectDisable[eModuleSPI](eMemoryIndex);
    }
#endif
    ISR_vExitCritical();
    
}
#else
/******************************************************************************
 * Function:       SPI_WriteBuffer
 *
 * Overview:       Write Data Buffer To SPI As Fast As Possible
 *****************************************************************************/
void SPI_WriteBuffer ( SPI_eModule_t eModuleSPI, uint8_t* pu8Data, uint16_t u16BytesCount)
{
    SPI_eMemoryIndex_t eMemoryIndex = ((eModuleSPI >> 4) & 0x000F);     /* u16TransactionID also contains eMemoryIndex */
    eModuleSPI &= 0x000F;
    SPI_sBaseRegisters_t* psBase = SPI_sHandles[eModuleSPI].psBase;
    
    bool bSwapNeeded = 0;
    uint16_t u16Data;
    
    // assure interrupt is not pending and wait complete if needed
    while(ISR_vChkBitIEC(SPI_sBitOffsetsISR.u8BitOffsetsISRTrnsf[eModuleSPI]) == true){;}
    
    ISR_vEnterCritical();

    
    if (eModuleSPI == SPI_eExtMemSPIModule)
    {
        #if SPI_USE_MIXED_8_16_BIT_MODE
        SPI_vSetModeBitsCount(eModuleSPI, (SPI_eModeBitsCount_t)((u16BytesCount+1) & 0x0001));
        bSwapNeeded = 1;
        if ((u16BytesCount & 0x0001) == 0)
        {       
            uint16_t u16Index;
            uint16_t u16Temp;
            for (u16Index = 0; u16Index < u16BytesCount; u16Index += 2)
            {
                u16Temp = pu8Data[u16Index];
                pu8Data[u16Index] = pu8Data[u16Index+1];
                pu8Data[u16Index+1] = u16Temp;
            }
        }

        #else
        SPI_vSetModeBitsCount(eModuleSPI, SPI_MODE_8BITS);  /* consider to use default initialization mode instead of force 8-bit */
        #endif
    }
    else
    {
        
    }
    
    
    
    
    

    if (SPI_pfChipSelectEnable[eModuleSPI] != NULL)
    {
        SPI_pfChipSelectEnable[eModuleSPI](eMemoryIndex);
    }
    if (psBase->SPICON1bits.MODE16)
    {
        uint16_t* pu16 = (uint16_t*)pu8Data;
        u16BytesCount = (u16BytesCount + 1) >> 1;       /* fix to transfer count */
        while(u16BytesCount > 0)
        {
            while(psBase->SPISTATbits.SPITBF){;}
            u16Data = *pu16++;
            if (bSwapNeed)
            {
                psBase->SPIBUF = (u16Data >> 8) | (u16Data << 8);
            }
            else
            {
                psBase->SPIBUF = u16Data;
            }
            u16BytesCount--;
        }
    }
    else
    {
        while(u16BytesCount > 0)
        {
            while(psBase->SPISTATbits.SPITBF){;}
            DEBUG_SET_SPI1();
            psBase->SPIBUF = *pu8Data++; rework function for esch spi
            u16BytesCount--;
            DEBUG_CLR_SPI1();
        }
    }
    if (SPI_pfChipSelectDisable[eModuleSPI] != NULL)
    {
        SPI_pfChipSelectDisable[eModuleSPI](eMemoryIndex);
    }

    ISR_vExitCritical();
}
#endif
/* *****************************************************************************
 * SPI_ReadWriteBuffer
 * 
 * 
 * Overview:       Read Write Data To/From SPI Buffer queue up and fill
 *                 Data Width Extracted From SPI Configuration
 **************************************************************************** */
void SPI_ReadWriteBuffer ( SPI_eModule_t eModuleSPI, uint8_t* pu8Data, uint16_t u16BytesCount, volatile SPI_eTransferStatus_t* peStatus, uint16_t u16TransactionID)
{
    SPI_eMemoryIndex_t eMemoryIndex = ((eModuleSPI >> 4) & 0x000F);     /* u16TransactionID also contains eMemoryIndex */
    eModuleSPI &= 0x000F;
    
    ISR_vEnterCritical();
    // check if there is space in the queue
    if (SPI_sObject[eModuleSPI].trStatus.s.full != true)
    {
        if (peStatus != NULL)
        {
            *peStatus = SPI_TRANSF_STA_PENDING;
        }
        SPI_sObject[eModuleSPI].pTrTail->pu8DataBuffer      = pu8Data;
        SPI_sObject[eModuleSPI].pTrTail->peTransferStatus   = peStatus;
        SPI_sObject[eModuleSPI].pTrTail->eTransferStatus    = SPI_TRANSF_STA_PENDING;
        SPI_sObject[eModuleSPI].pTrTail->eMemoryIndex       = eMemoryIndex;
        SPI_sObject[eModuleSPI].pTrTail->u16TransactionID   = u16TransactionID;
        
        
        bool bFix16BitTransfer = true;
        #if SPI_USE_MIXED_8_16_BIT_MODE
        if (eModuleSPI == SPI_eExtMemSPIModule)
        {
            bFix16BitTransfer = false;  /* always request 8 Bit Mode by Default inside queue */
        }
        #endif

        if (bFix16BitTransfer)
        {
            if (SPI_sHandles[eModuleSPI].psBase->SPICON1bits.MODE16)
            {
                u16BytesCount = (u16BytesCount + 1) >> 1;       /* fix to transfer count */
            }
        }
        SPI_sObject[eModuleSPI].pTrTail->u16TransfersCount  = u16BytesCount;

        // since we added one item to be processed, we know
        // it is not empty, so set the empty status to false
        SPI_sObject[eModuleSPI].trStatus.s.empty = false;

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
        
        
        // for interrupt based - if disabled interrupt
        if(ISR_vChkBitIEC(SPI_sBitOffsetsISR.u8BitOffsetsISRTrnsf[eModuleSPI]) == false)
        {
            
            
            //uint16_t u16Data;
            //while((SPI_sHandles[eModuleSPI].psBase->SPISTATbits.SRXMPT == 0) && (SPI_sHandles[eModuleSPI].psBase->SPISTATbits.SRXMPT == 0))
            //{
            //    u16Data = SPI_sHandles[eModuleSPI].psBase->SPIBUF;
            //}
 
            // force the task to run since we know that the queue has
            // something that needs to be sent
            ISR_vSetBitIEC(SPI_sBitOffsetsISR.u8BitOffsetsISRTrnsf[eModuleSPI]);    // enable interrupt - check enough to enter ISR
            ISR_vSetBitIFS(SPI_sBitOffsetsISR.u8BitOffsetsISRTrnsf[eModuleSPI]);    // set  flag force enter ISR
            //SPI_sHandles[eModuleSPI].psBase->SPIBUF = u16Data;
        }
    }
    else
    {
        if (peStatus != NULL)
        {
            *peStatus = SPI_TRANSF_STA_FAIL;
        }
    }
    ISR_vExitCritical();
    //return *peStatus;
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
void SPI_vChipSelectFunctionsInit(SPI_eModule_t eModuleSPI, SPI_pfVoid_u16_t pfChipSelectEnable, SPI_pfVoid_u16_t pfChipSelectDisable, SPI_pfVoid_t pfChipSelectDisableAll)
{
    SPI_pfChipSelectEnable [eModuleSPI] = pfChipSelectEnable;
    SPI_pfChipSelectDisable[eModuleSPI] = pfChipSelectDisable;  
    SPI_pfChipSelectDisableAll[eModuleSPI] = pfChipSelectDisableAll;  
}

/******************************************************************************
 * SPI_vRxCompleteFunctionsInit
 *****************************************************************************/
void SPI_vRxCompleteFunctionsInit(SPI_eModule_t eModuleSPI, SPI_pfVoid_u16_t pfTransferCompleted)
{
    SPI_pfTransferCompleted[eModuleSPI] = pfTransferCompleted;  
}

uint32_t SPI_u32Error = 0;
uint32_t SPI_u32PreError = 0;





/******************************************************************************
 * SPI1_vQueueProcess - Call In ISR (consider also main loop)
 *****************************************************************************/
void SPI1_vQueueProcess(void)
{
    uint16_t u16CountTx;
    uint16_t u16CountFree;
    
    DEBUG_CLR_SPI1();
    /* Process Received Data Transmit */
    
    while(SPI1STATbits.SRXMPT == 0)
    {
        uint16_t u16Data = SPI1BUF;
        if (SPI_sCurrentTransfer[SPI_1].bProcessing)
        {
            if(SPI_sCurrentTransfer[SPI_1].u16CurrentCntRD > 0)
            {
                // spi read
                if (SPI1CON1bits.MODE16)
                {
                    uint16_t* pu16 = (uint16_t*)SPI_sCurrentTransfer[SPI_1].pu8CurrentBufRD;
                    if (SPI_sCurrentTransfer[SPI_1].bSwapBytes)
                    {
                        u16Data = (u16Data>>8) | (u16Data<<8);
                    }
                    *pu16++ = u16Data;
                    SPI_sCurrentTransfer[SPI_1].pu8CurrentBufRD = (uint8_t*)pu16;
                }
                else
                {
                    *(SPI_sCurrentTransfer[SPI_1].pu8CurrentBufRD)++ = (uint8_t)u16Data;
                }
                SPI_sCurrentTransfer[SPI_1].u16CurrentCntRD--;
                if (SPI_sCurrentTransfer[SPI_1].u16CurrentCntRD == 0)
                {
                    if (SPI_sCurrentTransfer[SPI_1].u16CurrentCntWR != 0)
                    {
                        SPI_u32Error++;
                    }
                    
                    
                    if (SPI_sCurrentTransfer[SPI_1].bProcessing)    
                    {
                        SPI_sCurrentTransfer[SPI_1].eStatus = SPI_TRANSF_STA_COMPLETED;
                        
                        if (SPI_sCurrentTransfer[SPI_1].peStatus != NULL)
                        {
                            *(SPI_sCurrentTransfer[SPI_1].peStatus) = SPI_TRANSF_STA_COMPLETED;
                        }
                        //if (SPI_sCurrentTransfer[SPI_1].peStatusHandle != NULL) do not write in here because freed
                        //{
                        //    *(SPI_sCurrentTransfer[SPI_1].peStatusHandle) = SPI_TRANSF_STA_COMPLETED;
                        //}
                    }
                    SPI1_CS_DisableAll();
                }
            }
        }
    }
    
    
    if (SPI_sCurrentTransfer[SPI_1].u16CurrentCntWR == 0)
    {
        if (SPI_sCurrentTransfer[SPI_1].u16CurrentCntRD != 0)
        {
            SPI_u32PreError++;
        }
    }
                    
     DEBUG_SET_SPI1();
       
    if ((SPI_sCurrentTransfer[SPI_1].bProcessing == false) || ((SPI_sCurrentTransfer[SPI_1].u16CurrentCntWR == 0) && (SPI_sCurrentTransfer[SPI_1].u16CurrentCntRD == 0)))
    {
        //transfer finished - try start new
        ISR_vEnterCritical();
        if((SPI_sObject[SPI_1].trStatus.s.empty != true) && (SPI_sObject[SPI_1].pTrHead->eTransferStatus == SPI_TRANSF_STA_PENDING))
        {
            // grab the item pointed by the head

            SPI_sCurrentTransfer[SPI_1].bProcessing = true;
            SPI_sCurrentTransfer[SPI_1].peStatus = SPI_sObject[SPI_1].pTrHead->peTransferStatus;
            SPI_sCurrentTransfer[SPI_1].eMemoryIndex = SPI_sObject[SPI_1].pTrHead->eMemoryIndex;
            SPI_sCurrentTransfer[SPI_1].eStatus = SPI_sObject[SPI_1].pTrHead->eTransferStatus;
            SPI_sCurrentTransfer[SPI_1].u16TransactionID = SPI_sObject[SPI_1].pTrHead->u16TransactionID;

            SPI_sCurrentTransfer[SPI_1].u16CurrentCntRD = SPI_sCurrentTransfer[SPI_1].u16CurrentCntWR = SPI_sObject[SPI_1].pTrHead->u16TransfersCount;
            SPI_sCurrentTransfer[SPI_1].pu8CurrentBufRD = SPI_sCurrentTransfer[SPI_1].pu8CurrentBufWR = SPI_sObject[SPI_1].pTrHead->pu8DataBuffer;
            
            SPI_sObject[SPI_1].pTrHead->eTransferStatus = SPI_TRANSF_STA_IDLE;
            
            #if SPI_COUNT_TRANSFER_HANDLES > 1 
            // check if the end of the array is reached
            if ((SPI_sObject[SPI_1].pTrHead+1) == (SPI_sTransferHandle[SPI_1] + SPI_COUNT_TRANSFER_HANDLES))
            {
                // adjust to restart at the beginning of the array
                SPI_sObject[SPI_1].pTrHead = SPI_sTransferHandle[SPI_1];
            }
            else
            {
                SPI_sObject[SPI_1].pTrHead++;
            }
            // check if the queue is empty
            if(SPI_sObject[SPI_1].pTrHead == SPI_sObject[SPI_1].pTrTail)
            #endif
            {
                // it is empty so set the empty status to true
                SPI_sObject[SPI_1].trStatus.s.empty = true;
            }

            // since we moved one item to be processed, we know
            // it is not full, so set the full status to false
            SPI_sObject[SPI_1].trStatus.s.full = false;

            SPI_sCurrentTransfer[SPI_1].bSwapBytes = 0;
            if (SPI_1 == SPI_eExtMemSPIModule)
            {
                #if SPI_USE_MIXED_8_16_BIT_MODE
                SPI_sCurrentTransfer[SPI_1].bSwapBytes = 1;
                uint16_t u16BytesCount = SPI_sCurrentTransfer[SPI_1].u16CurrentCntWR;
                SPI1_CS_Enable(SPI_sCurrentTransfer[SPI_1].eMemoryIndex, (SPI_eModeBitsCount_t)((u16BytesCount+1) & 0x0001));
                if ((u16BytesCount & 0x0001) == 0)
                {
                    SPI_sCurrentTransfer[SPI_1].u16CurrentCntRD >>= 1;
                    SPI_sCurrentTransfer[SPI_1].u16CurrentCntWR >>= 1;
                    
                    #if 0
                    uint16_t u16Index;
                    uint16_t u16Temp;
                    
                    uint8_t* pu8Data = SPI_sCurrentTransfer[SPI_1].pu8CurrentBufWR;
                    for (u16Index = 0; u16Index < u16BytesCount; u16Index += 2)
                    {
                        u16Temp = pu8Data[u16Index];
                        pu8Data[u16Index] = pu8Data[u16Index+1];
                        pu8Data[u16Index+1] = u16Temp;
                    }
                    #endif
                }

                #else
                SPI1_CS_Enable(SPI_sCurrentTransfer[SPI_1].eMemoryIndex, SPI_MODE_8BITS);   /* consider to use default initialization mode instead of force 8-bit */
                #endif
            }
            else
            {

            }
    
            SPI_sCurrentTransfer[SPI_1].u16CurrentCntRDBackup = SPI_sCurrentTransfer[SPI_1].u16CurrentCntRD;
            SPI_sCurrentTransfer[SPI_1].pu8CurrentBufRDBackup = SPI_sCurrentTransfer[SPI_1].pu8CurrentBufRD;
            SPI_sCurrentTransfer[SPI_1].u16CurrentCntWRBackup = SPI_sCurrentTransfer[SPI_1].u16CurrentCntWR;
            SPI_sCurrentTransfer[SPI_1].pu8CurrentBufWRBackup = SPI_sCurrentTransfer[SPI_1].pu8CurrentBufWR;

        }
        else
        {
            //queue empty or has to wait (next element not ready for transmit)
            SPI_sCurrentTransfer[SPI_1].bProcessing = false;
            _SPI1IE = 0;    // disable interrupt
            _SPI1IF = 0;    // clear flag
        }
        ISR_vExitCritical();
    }
    DEBUG_CLR_SPI1();

    /* Fill Data For Transmit */
    if (SPI_sCurrentTransfer[SPI_1].bProcessing)
    {
        u16CountTx = SPI_sCurrentTransfer[SPI_1].u16CurrentCntWR;
        u16CountFree = u8HardwareFifoDepth;
        u16CountFree -= SPI1STATbits.SPIBEC;
        
        if (u16CountTx > u16CountFree)
        {
            u16CountTx = u16CountFree;
        }
        u16CountFree = 0;
        // spi send
        if (SPI1CON1bits.MODE16)
        {
            uint16_t* pu16 = (uint16_t*)SPI_sCurrentTransfer[SPI_1].pu8CurrentBufWR;
            //while((SPI1STATbits.SPITBF == 0) && (u16CountTx > 0))
            while((SPI1STATbits.SPIBEC < u8HardwareFifoDepth) && (u16CountTx > 0))
            {
                uint16_t u16Data = *pu16++;
                
                if (SPI_sCurrentTransfer[SPI_1].bSwapBytes)
                {
                    SPI1BUF = (u16Data >> 8) | (u16Data << 8);
                }
                else
                {
                    SPI1BUF = u16Data;
                }
                
                u16CountTx--;
                u16CountFree++;
            }
            SPI_sCurrentTransfer[SPI_1].pu8CurrentBufWR = (uint8_t*)pu16;
            SPI_sCurrentTransfer[SPI_1].u16CurrentCntWR -=u16CountFree;
        }
        else
        {
            uint8_t* pu8 = (uint8_t*)SPI_sCurrentTransfer[SPI_1].pu8CurrentBufWR;
            while((SPI1STATbits.SPITBF == 0) && (u16CountTx > 0))
            while((SPI1STATbits.SPIBEC < u8HardwareFifoDepth) && (u16CountTx > 0))
            {
                SPI1BUF = *pu8++;
                u16CountTx--;
                u16CountFree++;
            }
            SPI_sCurrentTransfer[SPI_1].pu8CurrentBufWR = (uint8_t*)pu8;
            SPI_sCurrentTransfer[SPI_1].u16CurrentCntWR -=u16CountFree;
        }
    }
    DEBUG_SET_SPI1();
        
    
}



/******************************************************************************
 * SPI_vQueueProcess - Call In ISR (consider also main loop)
 *****************************************************************************/
void SPI_vQueueProcess(SPI_eModule_t eModuleSPI)
{
    volatile SPI_sBaseRegisters_t* psBase = SPI_sHandles[eModuleSPI].psBase;
    volatile SPI_sCurrentTransfer_t* psCurrentTransfer = &SPI_sCurrentTransfer[eModuleSPI];
    uint16_t u16CountTx;
    uint16_t u16CountRx;
    uint16_t u16CountFree;
    uint16_t u16CountTxMax;
    SPI_sObject_t* psObject;
    uint16_t u16Data;
    bool bRead = false;
    
    DEBUG_CLR_SPI1();
    /* Process Received Data Transmit */
    
    
    // spi read
    if (psBase->SPICON1bits.MODE16)
    {
        uint16_t* pu16 = (uint16_t*)psCurrentTransfer->pu8CurrentBufRD;
        
        while(psBase->SPISTATbits.SRXMPT == 0)
        {
            u16Data = psBase->SPIBUF;
            if(psCurrentTransfer->u16CurrentCntRD > 0)
            {
                if (psCurrentTransfer->bSwapBytes)
                {
                    u16Data = (u16Data>>8) | (u16Data<<8);
                }
                *pu16++ = u16Data; 
                psCurrentTransfer->u16CurrentCntRD--;
                bRead = true;
            }
            //else
            //{
            //    SPI_u32Error++;
            //}
        }
        psCurrentTransfer->pu8CurrentBufRD = (uint8_t*)pu16;
    }
    else
    {
        while(psBase->SPISTATbits.SRXMPT == 0)
        {
            u16Data = psBase->SPIBUF;
            if(psCurrentTransfer->u16CurrentCntRD > 0)
            {
                *(psCurrentTransfer->pu8CurrentBufRD)++ = (uint8_t)u16Data;
                psCurrentTransfer->u16CurrentCntRD--;
                bRead = true;
            }
            //else
            //{
            //    SPI_u32Error++;
            //}
        }
    }
   
    if ((psCurrentTransfer->u16CurrentCntRD == 0) && (bRead == true))
    {
        //if (psCurrentTransfer->u16CurrentCntWR != 0)
        //{
        //    SPI_u32Error++;
        //}

        if (psCurrentTransfer->bProcessing)    
        {
            psCurrentTransfer->eStatus = SPI_TRANSF_STA_COMPLETED;

            if (psCurrentTransfer->peStatus != NULL)
            {
                *(psCurrentTransfer->peStatus) = SPI_TRANSF_STA_COMPLETED;
            }
            //if (psCurrentTransfer->peStatusHandle != NULL)
            //{
            //    *(psCurrentTransfer->peStatusHandle) = SPI_TRANSF_STA_COMPLETED;
            //}
        }
        if (SPI_pfChipSelectDisable[eModuleSPI] != NULL)
        {
            SPI_pfChipSelectDisable[eModuleSPI](psCurrentTransfer->eMemoryIndex);
        }
        if (SPI_pfTransferCompleted[eModuleSPI] != NULL)
        {
            SPI_pfTransferCompleted[eModuleSPI](psCurrentTransfer->u16TransactionID);
        }
        
        
        
        
    }
    
    //if (psCurrentTransfer->u16CurrentCntWR == 0)
    //{
    //    if (psCurrentTransfer->u16CurrentCntRD != 0)
    //    {
    //        SPI_u32PreError++;
    //    }
    //}
                    
    DEBUG_SET_SPI1();
       
    if ((psCurrentTransfer->bProcessing == false) || ((psCurrentTransfer->u16CurrentCntWR == 0) && (psCurrentTransfer->u16CurrentCntRD == 0)))
    {
        psObject = &SPI_sObject[eModuleSPI];

        ISR_vEnterCritical();
        //transfer finished - try start new
        if((psObject->trStatus.s.empty != true) && (psObject->pTrHead->eTransferStatus == SPI_TRANSF_STA_PENDING))
        {
            // grab the item pointed by the head

            psCurrentTransfer->bProcessing = true;
            psCurrentTransfer->peStatus = psObject->pTrHead->peTransferStatus;
            psCurrentTransfer->eMemoryIndex = psObject->pTrHead->eMemoryIndex;
            psCurrentTransfer->eStatus = psObject->pTrHead->eTransferStatus;
            psCurrentTransfer->u16TransactionID = psObject->pTrHead->u16TransactionID;

            psCurrentTransfer->u16CurrentCntRD = psCurrentTransfer->u16CurrentCntWR = psObject->pTrHead->u16TransfersCount;
            psCurrentTransfer->pu8CurrentBufRD = psCurrentTransfer->pu8CurrentBufWR = psObject->pTrHead->pu8DataBuffer;
            
            psObject->pTrHead->eTransferStatus = SPI_TRANSF_STA_IDLE;
            
            #if SPI_COUNT_TRANSFER_HANDLES > 1 
            // check if the end of the array is reached
            if ((SPI_sObject[eModuleSPI].pTrHead+1) == (SPI_sTransferHandle[eModuleSPI] + SPI_COUNT_TRANSFER_HANDLES))
            {
                // adjust to restart at the beginning of the array
                psObject->pTrHead = SPI_sTransferHandle[eModuleSPI];
            }
            else
            {
                psObject->pTrHead++;
            }
            // check if the queue is empty
            if(psObject->pTrHead == psObject->pTrTail)
            #endif
            {
                // it is empty so set the empty status to true
                psObject->trStatus.s.empty = true;
            }

            // since we moved one item to be processed, we know
            // it is not full, so set the full status to false
            psObject->trStatus.s.full = false;
            

            psCurrentTransfer->bSwapBytes = 0;
            if ((SPI_1 == SPI_eExtMemSPIModule) && (SPI_1 == eModuleSPI))
            {
                #if SPI_USE_MIXED_8_16_BIT_MODE
                psCurrentTransfer->bSwapBytes = 1;
                uint16_t u16BytesCount = psCurrentTransfer->u16CurrentCntWR;
                SPI_vSetModeBitsCount(eModuleSPI, (SPI_eModeBitsCount_t)((u16BytesCount+1) & 0x0001));
                if ((u16BytesCount & 0x0001) == 0)
                {
                    
                    psCurrentTransfer->u16CurrentCntRD >>= 1;
                    psCurrentTransfer->u16CurrentCntWR >>= 1;
                    
                    #if 0                    
                    uint16_t u16Index;
                    uint16_t u16Temp;
                    
                    uint8_t* pu8Data = psCurrentTransfer->pu8CurrentBufWR;
                    for (u16Index = 0; u16Index < u16BytesCount; u16Index += 2)
                    {
                        u16Temp = pu8Data[u16Index];
                        pu8Data[u16Index] = pu8Data[u16Index+1];
                        pu8Data[u16Index+1] = u16Temp;
                    }
                    #endif
                }

                #else
                SPI_vSetModeBitsCount(eModuleSPI, SPI_MODE_8BITS);  /* consider to use default initialization mode instead of force 8-bit */
                #endif
            }
            else
            {

            }

            psCurrentTransfer->u16CurrentCntRDBackup = psCurrentTransfer->u16CurrentCntRD;
            psCurrentTransfer->pu8CurrentBufRDBackup = psCurrentTransfer->pu8CurrentBufRD;
            psCurrentTransfer->u16CurrentCntWRBackup = psCurrentTransfer->u16CurrentCntWR;
            psCurrentTransfer->pu8CurrentBufWRBackup = psCurrentTransfer->pu8CurrentBufWR;
            if (SPI_pfChipSelectEnable[eModuleSPI] != NULL)
            {
                SPI_pfChipSelectEnable[eModuleSPI](psCurrentTransfer->eMemoryIndex);
            }
        }
        else
        {
            //queue empty or has to wait (next element not ready for transmit)
            psCurrentTransfer->bProcessing = false;
            ISR_vClrBitIEC(SPI_sBitOffsetsISR.u8BitOffsetsISRTrnsf[eModuleSPI]);    // disable interrupt
            ISR_vClrBitIFS(SPI_sBitOffsetsISR.u8BitOffsetsISRTrnsf[eModuleSPI]);    // clear flag
        }
        ISR_vExitCritical();

    }
        DEBUG_CLR_SPI1();

    /* Fill Data For Transmit */
    if (psCurrentTransfer->bProcessing)
    {
        u16CountTxMax = psCurrentTransfer->u16CurrentCntWR;
        u16CountTx = psCurrentTransfer->u16CurrentCntWR;
        u16CountRx = psCurrentTransfer->u16CurrentCntRD;
        u16CountFree = u8HardwareFifoDepth;
        u16CountFree -= psBase->SPISTATbits.SPIBEC;
        
        if (u16CountTx > u16CountFree)
        {
            u16CountTx = u16CountFree;
        }
        u16CountFree = 0;
        // spi send
        if (psBase->SPICON1bits.MODE16) //not updated - see below 8-bit
        {
            uint16_t* pu16   = (uint16_t*)psCurrentTransfer->pu8CurrentBufWR;
            uint16_t* pu16Rx = (uint16_t*)psCurrentTransfer->pu8CurrentBufRD;
            //while((psBase->SPISTATbits.SPITBF == 0) && (u16CountTx > 0) && (u16CountFree < u16CountTxMax))
            while((psBase->SPISTATbits.SPIBEC < u8HardwareFifoDepth) && (u16CountTx > 0) && (u16CountFree < u16CountTxMax))
            {
                uint16_t u16Data = *pu16++;
                
                if (psCurrentTransfer->bSwapBytes)
                {
                    psBase->SPIBUF = (u16Data >> 8) | (u16Data << 8);
                }
                else
                {
                    psBase->SPIBUF = u16Data;
                }
                
                
                u16CountFree++;
                if (psBase->SPISTATbits.SRXMPT == 0)
                {
                    u16Data = psBase->SPIBUF;
                    if (u16CountRx > 0)
                    {
                        if (psCurrentTransfer->bSwapBytes)
                        {
                            u16Data = (u16Data>>8) | (u16Data<<8);
                        }
                        *pu16Rx++ = u16Data;
                        u16CountRx--;
                        //if (psCurrentTransfer->u16CurrentCntRD == 0)
                        //{
                            //if (u16CountTxMax != psCurrentTransfer->u16CurrentCntWR)
                            //{
                            //    SPI_u32Error++;
                            //}
                            
                        //}
                    }
                    //else
                    //{
                    //    if (u16CountTxMax != psCurrentTransfer->u16CurrentCntWR)
                    //    {
                    //        SPI_u32Error++;
                    //    }
                    //}
                }
                else
                {
                    u16CountTx--;
                }
            }
            psCurrentTransfer->u16CurrentCntWR -=u16CountFree;
            psCurrentTransfer->pu8CurrentBufWR = (uint8_t*)pu16;
            psCurrentTransfer->pu8CurrentBufRD = (uint8_t*)pu16Rx;
            psCurrentTransfer->u16CurrentCntRD = u16CountRx;
        }
        else
        {
            uint8_t* pu8 = (uint8_t*)psCurrentTransfer->pu8CurrentBufWR;
            uint8_t* pu8Rx = (uint8_t*)psCurrentTransfer->pu8CurrentBufRD;
            //while((psBase->SPISTATbits.SPITBF == 0) && (u16CountTx > 0) && (u16CountFree < u16CountTxMax))
            while((psBase->SPISTATbits.SPIBEC < u8HardwareFifoDepth) && (u16CountTx > 0) && (u16CountFree < u16CountTxMax))
            {
                psBase->SPIBUF = *pu8++;
                
                u16CountFree++;
                if (psBase->SPISTATbits.SRXMPT == 0)
                {
                    u16Data = psBase->SPIBUF;
                    if (u16CountRx > 0)
                    {
                        *pu8Rx++ = u16Data;
                        u16CountRx--;
                        //if (psCurrentTransfer->u16CurrentCntRD == 0)
                        //{
                            //if (u16CountTxMax != psCurrentTransfer->u16CurrentCntWR)
                            //{
                            //    SPI_u32Error++;
                            //}
                            
                        //}
                    }
                    //else
                    //{
                    //    if (u16CountTxMax != psCurrentTransfer->u16CurrentCntWR)
                    //    {
                    //        SPI_u32Error++;
                    //    }
                    //}
                }
                else
                {
                    u16CountTx--;
                }
            }
            psCurrentTransfer->u16CurrentCntWR -=u16CountFree;
            psCurrentTransfer->pu8CurrentBufWR = (uint8_t*)pu8;
            psCurrentTransfer->pu8CurrentBufRD = (uint8_t*)pu8Rx;
            psCurrentTransfer->u16CurrentCntRD = u16CountRx;
        }
    }
    DEBUG_SET_SPI1();
        
    
}
