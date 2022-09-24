/* *****************************************************************************
 * File:   pmp.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 06 18 11:36
 * 
 * Description: PMP Driver dsPIC33
 * 
 **************************************************************************** */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <xc.h>
#include "pmp.h"
#include "isr.h"
#include "dma_drv.h"
#include "boardcfg.h"
#if USE_DYNAMIC_MEMORY_RW
#include "memory.h"
#endif

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

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
            uint16_t PMCON  ;
            uint16_t PMMODE ;
            uint16_t PMADDR ;
            uint16_t PMDOUT2;
            uint16_t PMDIN1 ;
            uint16_t PMDIN2 ;
            uint16_t PMAEN  ;
            uint16_t PMSTAT ;
        };
        
        struct 
        {
            uint16_t : 16;
            uint16_t : 16;
            uint16_t PMDOUT1;
        };
        
        
        
        struct
        {
            PMCONBITS  PMCONbits;
            PMMODEBITS PMMODEbits;
            PMADDRBITS PMADDRbits;
            uint16_t   PMDOUT2bits;
            uint16_t   PMDIN1bits;
            uint16_t   PMDIN2bits;
            PMAENBITS  PMAENbits;
            PMSTATBITS PMSTATbits;
        };
        
        struct 
        {
            uint16_t : 16;
            uint16_t : 16;
            PMDOUT1BITS PMDOUT1bits;
        };
        
    };
}PMP_sBaseRegisters_t;

typedef struct
{
    union 
    {
        uint16_t PADCFG1  ;
        PADCFG1BITS PADCFG1bits;
    };
}PMP_sPagConfigRegs_t;

typedef struct
{
    PMP_sBaseRegisters_t* psBase;
    PMP_sPagConfigRegs_t* psPadCfg;
}PMP_sHandle_t;



/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
PMP_sHandle_t sHandle = 
{
    (PMP_sBaseRegisters_t*)(&PMCON),
    (PMP_sPagConfigRegs_t*)(&PADCFG1),
};


volatile PMP_sBaseRegisters_t* psBasePMP = (PMP_sBaseRegisters_t*)(&PMCON);
volatile PMP_sPagConfigRegs_t* psPadCfg = (PMP_sPagConfigRegs_t*)(&PADCFG1);


DMA_eModule_t eModuleDMASelected = 0;
volatile DMA_sBaseRegisters_t* psBaseDMA = NULL;
uint8_t u8BitOffsetsISRDMA = 0;
uint16_t u16DummyZeroDMA = 0;

bool bPMPReadDMA = false;
bool bPMPWriteDMA = false;


bool bUseDynamicMemoryRD = USE_DYNAMIC_MEMORY_RD;
bool bUseDynamicMemoryWR = USE_DYNAMIC_MEMORY_WR;

#if 1
bool bUseDirectCopyRD = false;
bool bUseDirectCopyWR = false;
#else
bool bUseDirectCopyRD = true;   /* if these enabled direct copy in main loop when freemaster read write pmp crashes */
bool bUseDirectCopyWR = true;
#endif

bool bPMPBusy = false;

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */
void PMP_vWriteLoop(ePMPSelect_t ePMPSelect, uint16_t u16Address, uintPMP_t* pdData, uint16_t u16Size, bool bUseHeap);
void PMP_vReadLoop(ePMPSelect_t ePMPSelect, uint16_t u16Address, uintPMP_t* pdData, uint16_t u16Size, bool bUseHeap);
void PMP_vWriteDMA(ePMPSelect_t ePMPSelect, uint16_t u16Address, uintPMP_t* pdData, uint16_t u16Size, bool bUseHeap);
void PMP_vReadDMA(ePMPSelect_t ePMPSelect, uint16_t u16Address, uintPMP_t* pdData, uint16_t u16Size, bool bUseHeap);

void PMP_vDMACompleteProcess ( void );


/* *****************************************************************************
 * Functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * PMP_vInit
 * 
 * Description: PMP Initialization 
 **************************************************************************** */
void PMP_vInit ( PMP_eUsePMPDMA_t eUsePMPDMA )
{
    //PMCS1_TRIS = 0;
    //PMCS1A_TRIS = 0;
    
    psBasePMP->PMCONbits.PMPEN = 0;            /* Parallel Master Port Enable bit */
    
    psBasePMP->PMCONbits.PSIDL = 0;            /* Stop in Idle Mode bit */
    psBasePMP->PMCONbits.ADRMUX = 0b01;        /* Part multiplexed on PMD<7:0> pins */
    psBasePMP->PMCONbits.ADRMUX = 0b10;        /* All 16 bits of address are multiplexed on PMD<7:0> pins */
    psBasePMP->PMCONbits.PTBEEN = 0;           /* Byte Enable Port Enable bit (16-bit Master mode) ??? */
    psBasePMP->PMCONbits.PTWREN = 1;           /* Write Enable Strobe Port Enable bit (PMWR/PMENB port) */
    psBasePMP->PMCONbits.PTRDEN = 1;           /* Read/Write Strobe Port Enable bit (PMRD/nPMWR) */
    
    psBasePMP->PMCONbits.CSF = 0b00;           /* PMCS1 and PMCS2 function as address bits 15 and 14 */
    psBasePMP->PMCONbits.CSF = 0b10;           /* PMCS1 and PMCS2 function as chip select */
    
    //psBasePMP->PMCONbits.ALP = 1;              /* Address Latch Polarity bit (PMALL and PMALH) */
    psBasePMP->PMCONbits.ALP = 0;              /* Address Latch Polarity bit (PMALL and PMALH) - Active Low Level */

    psBasePMP->PMCONbits.CS2P = 1;             /* Chip Select 1 Polarity bit (PMCS2) */
    
    psBasePMP->PMCONbits.CS1P = 1;             /* Chip Select 0 Polarity bit (PMCS1/PMCS) */
    //psBasePMP->PMCONbits.CS1P = 0;             /* Chip Select 0 Polarity bit (PMCS1/PMCS) - Active Low Level */
    
    psBasePMP->PMCONbits.BEP = 1;              /* Byte Enable Polarity bit (PMBE) */
    
    psBasePMP->PMCONbits.WRSP = 1;             /* Write Strobe Polarity bit (PMWR) */
    psBasePMP->PMCONbits.RDSP = 1;             /* REad Strobe Polarity bit (PMRD) */
    //psBasePMP->PMCONbits.WRSP = 0;             /* Write Strobe Polarity bit (PMWR) - Active Low Level */
    //psBasePMP->PMCONbits.RDSP = 0;             /* REad Strobe Polarity bit (PMRD) - Active Low Level */
    
    
    while (psBasePMP->PMMODEbits.BUSY);        /* not needed here use for copy paste */
    psBasePMP->PMMODEbits.IRQM = 0b01;         /* Interrupt generated at the end of the read/write cycle */
    
    psBasePMP->PMMODEbits.INCM = 3;            /* PSP read and write buffers auto-increment (Legacy PSP mode only) */
    psBasePMP->PMMODEbits.INCM = 0;            /* No increment or decrement of address */
    psBasePMP->PMMODEbits.INCM = 1;            /* Increment ADDR by 1 every read/write cycle */
    
    #if PMP_BUS_SIZE_BYTES != 2
    psBasePMP->PMMODEbits.MODE16 = 0;          /* 8-bit mode: data register is 8 bits, a read/write to the data register invokes one 8-bit transfer */
    #else
    psBase->PMMODEbits.MODE16 = 1;          /* 16-bit mode: data register is 16 bits, a read/write to the data register invokes two 8-bit transfer */
    #endif
    
    //psBasePMP->PMMODEbits.MODE = 0b01;         /* Enhanced PSP, control signals (PMRD, PMWR, PMCSx, PMD<7:0>, and PMA<1:0>) */        
    //psBasePMP->PMMODEbits.MODE = 0b00;         /* Legacy Parallel Slave Port, control signals (PMRD, PMWR, PMCSx, and PMD<7:0>) */        
    //psBasePMP->PMMODEbits.MODE = 0b11;         /* Master Mode 1 (PMCSx, PMRD/PMWR, PMENB, PMBE, PMA<x:0>, and PMD<7:0>) */        
    psBasePMP->PMMODEbits.MODE = 0b10;         /* Master Mode 2 (PMCSx, PMRD, PMWR, PMBE, PMA<x:0>, and PMD<7:0>) */        
            
    //psBase->PMMODEbits.WAITB = 0b00;        /* Data wait of 1 TCY (demultiplexed/multiplexed); address phase of 1 TCY (multiplexed) */
    //psBase->PMMODEbits.WAITM = 0b0000;      /* No additional Wait cycles (operation forced into one TCY) */
    //psBase->PMMODEbits.WAITE = 0b00;        /* Wait of 1 TCY */
    
    #if USE_IN_FPGA_CS_DELAY_INSTEAD_OF_PMALL_AND_PMALH_STROBES
        psBasePMP->PMMODEbits.WAITB = 0b01;      /* address phase of 0+1 TCY (multiplexed) */
        //psBasePMP->PMMODEbits.WAITB = 0b10;        /* address phase of 0+1 TCY (multiplexed) do not put more than this or fix fpga's */
        psBasePMP->PMMODEbits.WAITM = 0b0010;      /* 2 additional Wait cycles for data  */
        psBasePMP->PMMODEbits.WAITE = 0b00;        /* Wait of 0+1 TCY */
    #else
    if (eUsePMPDMA)
    {
        psBasePMP->PMMODEbits.WAITB = 0b10;        /* Data wait of 0+1 TCY (demultiplexed/multiplexed); address phase of 0+1 TCY (multiplexed) */
        //psBasePMP->PMMODEbits.WAITM = 0b0010;      /* 2 additional Wait cycles  */
        psBasePMP->PMMODEbits.WAITM = 0b0011;      /* 3 additional Wait cycles needed for now for FPGA PWM */
        psBasePMP->PMMODEbits.WAITE = 0b00;        /* Wait of 0+1 TCY */
    }
    else
    {
        psBasePMP->PMMODEbits.WAITB = 0b00;        /* Data wait of 0+1 TCY (demultiplexed/multiplexed); address phase of 0+1 TCY (multiplexed) */
        psBasePMP->PMMODEbits.WAITM = 0b0010;      /* 2 additional Wait cycles  */
        
        psBasePMP->PMMODEbits.WAITE = 0b00;        /* Wait of 0+1 TCY */
        
        bPMPReadDMA = false;
        bPMPWriteDMA = false;
        
    }
    #endif
    
    psBasePMP->PMCONbits.PMPEN = 1;            /* Parallel Master Port Enable bit */
    
    psBasePMP->PMADDR = 0x0000;
    psBasePMP->PMADDRbits.ADDR = 0x0000;
    psBasePMP->PMADDRbits.CS1  = 0;
    psBasePMP->PMADDRbits.CS2  = 0;
    
    psBasePMP->PMAEN = 0x0000;                 /* All Pins function as IO by default */
    psBasePMP->PMAENbits.PTEN0 = 1;            /* PMALL */
    psBasePMP->PMAENbits.PTEN1 = 1;            /* PMALH */
    psBasePMP->PMAENbits.PTEN14 = 1;           /* PMCS1 */
    psBasePMP->PMAENbits.PTEN15 = 0;           /* PMCS2 - not used */
    
    psBasePMP->PMSTAT = 0;
    
    psPadCfg->PADCFG1bits.PMPTTL = 0;         /* PMP module uses Schmitt Trigger input buffers */
    
    
    psBasePMP->PMCONbits.ADRMUX = 0b00;        /* No multiplexing */
    psBasePMP->PMCONbits.ADRMUX = 0b10;        /* All 16 bits of address are multiplexed on PMD<7:0> pins */
   

}



/* *****************************************************************************
 * PMP_vInitDMA
 * 
 * Description: PMP Init DMA Function
 **************************************************************************** */
void PMP_vInitDMA(DMA_eModule_t eModuleDMA) 
{
    if (psBasePMP != NULL)
    {
        eModuleDMASelected = eModuleDMA;
        psBaseDMA = DMA_sHandles.psBase + eModuleDMA;
        u8BitOffsetsISRDMA = DMA_sHandles.u8BitOffsetsISR[eModuleDMA];

        psBaseDMA->DMACONbits.CHEN = 0;   // Disable DMA Channel
        psBaseDMA->DMACONbits.DIR = 1;        /* 1 = Reads from DPSRAM (or RAM) address, writes to peripheral address */
        psBaseDMA->DMACONbits.SIZE = 1;       /* 0 = Word; 1 = Byte Transfer */
        psBaseDMA->DMACONbits.HALF = 0;       /* 0 = Initiates interrupt when all of the data has been moved */
        psBaseDMA->DMACONbits.NULLW = 0;      /* 0 = Normal operation */
        psBaseDMA->DMACONbits.AMODE = 0;      /* 00 = Register Indirect with Post-Increment mode */

        psBaseDMA->DMAREQ = 0b00101101;    
        psBaseDMA->DMAPAD = ( volatile uint16_t ) &(psBasePMP->PMDIN1);

        psBaseDMA->DMACONbits.MODE = 1;       /* 11 = One Shot, Ping-Pong modes are enabled  
                                               * 10 = Continuous, Ping-Pong modes are enabled  
                                               * 01 = One Shot, Ping-Pong modes are disabled  
                                               * 00 = Continuous, Ping-Pong modes are disabled  */

        ISR_vSetDatIPC(u8BitOffsetsISRDMA,5);    // DMA interrupt priority level
        ISR_vClrBitIFS(u8BitOffsetsISRDMA);    // Clear DMA interrupt
        ISR_vSetBitIEC(u8BitOffsetsISRDMA);    // Enable DMA interrupt
        psBaseDMA->DMACONbits.CHEN = 0;   // Disable DMA Channel
        
        bPMPReadDMA = true;
        bPMPWriteDMA = true;
        
        ISR_vSetISRDMA(PMP_vDMACompleteProcess, eModuleDMA);
    
    }
}






uint16_t PMP_vAddressFix(uint16_t u16Address, ePMPSelect_t ePMPSelect)
{
    uint16_t u16Result;
    
    if (PMP_CS_12 == ePMPSelect)
    {
        u16Result = u16Address | (PMCS1_MASK | PMCS2_MASK);
    }
    else
    {
        u16Result = u16Address | (PMCS1_MASK << ePMPSelect);
    }
    
    return u16Result;
}






bool PMP_vWrite(ePMPSelect_t ePMPSelect, uint16_t u16Address, uintPMP_t* pdData, uint16_t u16Size, bool bUseHeap)
{
    bool bLoopUsed = false;
    if ((psBaseDMA != NULL) && (bPMPWriteDMA))
    {
        PMP_vWriteDMA(ePMPSelect, u16Address, pdData, u16Size, bUseHeap);
    }
    else
    {
        PMP_vWriteLoop(ePMPSelect, u16Address, pdData, u16Size, bUseHeap);
        bLoopUsed = true;
        
    }
    return bLoopUsed;
}

bool PMP_vRead(ePMPSelect_t ePMPSelect, uint16_t u16Address, uintPMP_t* pdData, uint16_t u16Size, bool bUseHeap)
{
    bool bLoopUsed = false;
    if ((psBaseDMA != NULL) && (bPMPReadDMA))
    {
        PMP_vReadDMA(ePMPSelect, u16Address, pdData, u16Size, bUseHeap);
    }
    else
    {
        PMP_vReadLoop(ePMPSelect, u16Address, pdData, u16Size, bUseHeap);
        bLoopUsed = true;
    }
    return bLoopUsed;
}

uint8_t* pdDataDMAWrite = NULL;
uint8_t* pdDataDMARead = NULL;

void PMP_vWriteDMA(ePMPSelect_t ePMPSelect, uint16_t u16Address, uintPMP_t* pdData, uint16_t u16Size, bool bUseHeap)
{
    
    PMADDR = PMP_vAddressFix(u16Address, ePMPSelect);
    DMA2CONbits.CHEN = 0;   // Disable DMA Channel
    psBaseDMA->DMACONbits.NULLW = 0;      /* 0 = Normal operation */
    if (pdData == NULL)
    {
        DMA2CONbits.AMODE = 1;      /* 01 = Register Indirect without Post-Increment mode */
        DMA2STAL = ( uint16_t ) &u16DummyZeroDMA;
        DMA2STAH = ( uint16_t )((uint32_t)(uint16_t)(&u16DummyZeroDMA) >> 16);      /* Should be 0 or read the datasheet */
    }
    else
    {
        DMA2CONbits.AMODE = 0;      /* 00 = Register Indirect with Post-Increment mode */
        DMA2STAL = ( uint16_t ) pdData;
        DMA2STAH = ( uint16_t )((uint32_t)(uint16_t)(pdData) >> 16);      /* Should be 0 or read the datasheet */
    }
    DMA2CONbits.DIR = 1;        /* 1 = Reads from DPSRAM (or RAM) address, writes to peripheral address */
    DMA2CNT = u16Size - 1;      /* data Count */                    
    
    bPMPBusy = true;

//    if (bUseDynamicMemoryWR)
//    {    
//        pdDataDMAWrite = pdData;
//    }
//    if (bUseDynamicMemoryRD)
//    {
//        pdDataDMARead = NULL;
//    }
    if (bUseHeap)
    {
        pdDataDMAWrite = pdData;
    }
    else
    {
        pdDataDMAWrite = NULL;
    }
    pdDataDMARead = NULL;
    
    IFS1bits.DMA2IF = 0;    // Clear DMA interrupt
    DMA2CONbits.CHEN = 1;   // Enable DMA Channel
    DMA2REQbits.FORCE = 1;  /* Start DMA Transfer */
}


void PMP_vReadDMA(ePMPSelect_t ePMPSelect, uint16_t u16Address, uintPMP_t* pdData, uint16_t u16Size, bool bUseHeap)
{
    uintPMP_t udDummy;
    PMADDR = PMP_vAddressFix(u16Address, ePMPSelect);
    DMA2CONbits.CHEN = 0;   // Disable DMA Channel
    psBaseDMA->DMACONbits.NULLW = 1;      /* 1 = null data write operation */
    DMA2CONbits.AMODE = 0;      /* 00 = Register Indirect with Post-Increment mode */
    DMA2CONbits.DIR = 0;        /* 0 = writes to DPSRAM (or RAM) address, Reads from peripheral address */
    DMA2CNT = u16Size - 1;      /* data Count */   
    //udDummy = *pdData++;
    DMA2STAL = ( uint16_t ) pdData;
    DMA2STAH = ( uint16_t )((uint32_t)(uint16_t)(pdData) >> 16);      /* Should be 0 or read the datasheet */
    
    bPMPBusy = true;
//    if (bUseDynamicMemoryWR)
//    {
//        pdDataDMAWrite = NULL;    
//    }
//    if (bUseDynamicMemoryRD)
//    {
//        pdDataDMARead = pdData;
//    }
    
    if (bUseHeap)
    {
        pdDataDMARead = pdData;
    }
    else
    {
        pdDataDMARead = NULL;
    }
    pdDataDMAWrite = NULL;
    
    
    IFS1bits.DMA2IF = 0;    // Clear DMA interrupt
    DMA2CONbits.CHEN = 1;   // Enable DMA Channel
    udDummy = PMDIN1;
    //DMA2REQbits.FORCE = 1;  /* Start DMA Transfer */    
}


/*=============================================================================
Interrupt Service Routines.
=============================================================================*/
/******************************************************************************
 * Function:        PMP_vDMACompleteProcess
 *
 * Overview:        DMA PMP Completed
 *****************************************************************************/
void PMP_vDMACompleteProcess ( void )
{
//#if 0
    //if (bUseDirectCopyWR == false)
    {
        //if (bUseDynamicMemoryWR)
        {
            if (pdDataDMAWrite != NULL)
            {
                MEMORY_vFree(pdDataDMAWrite);
            }   
        }
    }
//#endif
    
    bPMPBusy = false;
}



bool PMP_bBusyStatus(void)
{
    return bPMPBusy;
}







void PMP_vWriteLoop(ePMPSelect_t ePMPSelect, uint16_t u16Address, uintPMP_t* pdData, uint16_t u16Size, bool bUseHeap)
{
    
    bPMPBusy = true;  
    uintPMP_t* pdDataCopy = pdData;
    
    while (PMMODEbits.BUSY);
    PMADDR = PMP_vAddressFix(u16Address, ePMPSelect);
    while (u16Size-- > 0)
    {
        if (pdData == NULL)
        {
            PMDIN1 = 0;
        }
        else
        {
            PMDIN1 = *pdData++;     /* roll-over address not checked */
        }
        while (PMMODEbits.BUSY);
    }
    bPMPBusy = false;
    
    if (bUseHeap)
    //if (bUseDynamicMemoryWR)
    {
        if (pdDataCopy != NULL)
        {
            MEMORY_vFree(pdDataCopy);
        }   
    }   
}


void PMP_vReadLoop(ePMPSelect_t ePMPSelect, uint16_t u16Address, uintPMP_t* pdData, uint16_t u16Size, bool bUseHeap)
{
    uintPMP_t udDummy;
    
    while (PMMODEbits.BUSY);
    bPMPBusy = true;
    if (u16Size > 0)
    {
        PMADDR = PMP_vAddressFix(u16Address, ePMPSelect);
        udDummy = PMDIN1;
        while (u16Size-- > 0)
        {
            while (PMMODEbits.BUSY);
            if (PMMODEbits.BUSY == 0)   /* small delay 1-2 Fcy */
            {
              *pdData++ = PMDIN1;     /* roll-over address not checked */
            }
        }
    }
    else
    {
        PMADDR = PMP_vAddressFix(u16Address, ePMPSelect);     
    }
    bPMPBusy = false;
}







/* Null Pointer Not Checked */
void PMP_vWriteFastPMCS1(uint16_t u16Address, uintPMP_t* pdData, uint16_t u16Size)
{
    ISR_vEnterCritical();
    PMADDR = u16Address | PMCS1_MASK;
    while (u16Size-- > 0)
    {
        while (PMMODEbits.BUSY);
        PMDIN1 = *pdData++;     /* roll-over address not checked */
    }
    ISR_vExitCritical();
}


void PMP_vClearFastPMCS1(uint16_t u16Address, uint16_t u16Size)
{
    ISR_vEnterCritical();
    PMADDR = u16Address | PMCS1_MASK;
    while (u16Size-- > 0)
    {
        while (PMMODEbits.BUSY);
        PMDIN1 = 0;
    }
    ISR_vExitCritical();
}


/* Null Pointer Not Checked */
void PMP_vReadFastPMCS1(uint16_t u16Address, uintPMP_t* pdData, uint16_t u16Size)
{
    uintPMP_t udDummy;
    ISR_vEnterCritical();
    PMADDR = u16Address | PMCS1_MASK;
    udDummy = PMDIN1;
    while (u16Size-- > 0)
    {
        while (PMMODEbits.BUSY);
        //if (PMMODEbits.BUSY == 0)   /* small delay 1-2 Fcy */
        //{
          *pdData++ = PMDIN1;     /* roll-over address not checked */
        //}
    }
    ISR_vExitCritical();
}


/* Null Pointer Not Checked */
void PMP_vWriteFastPMCS1Byte(uint16_t u16Address, uintPMP_t* pdData)
{
    ISR_vEnterCritical();
    PMADDR = u16Address | PMCS1_MASK;
    while (PMMODEbits.BUSY);
    PMDIN1 = *pdData;     /* roll-over address not checked */
    ISR_vExitCritical();
}
/* Null Pointer Not Checked */
void PMP_vReadFastPMCS1Byte(uint16_t u16Address, uintPMP_t* pdData)
{
    ISR_vEnterCritical();
    uintPMP_t udDummy;
    PMADDR = u16Address | PMCS1_MASK;
    udDummy = PMDIN1;
    while (PMMODEbits.BUSY);
    *pdData = PMDIN1;     /* roll-over address not checked */
    ISR_vExitCritical();
}


