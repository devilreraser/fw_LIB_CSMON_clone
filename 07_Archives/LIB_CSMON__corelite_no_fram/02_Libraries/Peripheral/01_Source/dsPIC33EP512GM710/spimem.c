/* *****************************************************************************
 * File:   spimem.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 01 22 11:15
 * 
 * Description: SPI Interface To External Memory
 * 
 **************************************************************************** */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <xc.h>
#include <stdbool.h>
#include <stdint.h>

#include "boardcfg.h"
#include "hmdrv.h"    /* not needed because hmlib.h included in header */
#include "spimem.h"
#include "sys_drv.h"
#include "spi.h"
#include "isr.h"

#ifdef BOARDCFG_USE_MCC_SPI
#include "spi1.h"
#include "spi2.h"
#include "spi3.h"
#endif

#if USE_SPI_FLIR
#include "flir.h"
#endif

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#if SPIMEM_EXT_MEM_USE_SPIMEM_QUEUE_OLD                              
#define SPIMEM_MAX_COUNT_SIMULTANEOUS_READS_FROM_EXTERNAL_MEMORY    8
#define SPIMEM_MAX_COUNT_SIMULTANEOUS_WRITES_TO_EXTERNAL_MEMORY     8
#endif

//#define EDS_SPACE   __eds__


/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
#if SPIMEM_EXT_MEM_USE_SPIMEM_QUEUE_OLD                              
typedef struct 
{
    eExtMemSelect_t eExtMemSelectID;
    uint16_t u16TransactionID;
    uint16_t u16HandleStatusSPI;
    uint16_t u16IndexFill;
    uint16_t u16IndexSend;
    uint16_t u16CmdQueueCount;  
    sExtMemCmd_t* sExtMemCmd;
    sSPIDMATransferBuffer_t* sSPIDMATransferBufferBase;
    sSPIDMATransferBuffer_t* sSPIDMATransferBufferOldUsage;
}sExtMemCmdHandle_t;

typedef struct
{
    uint16_t memSelectGlobal;
    sExtMemCmd_t* cmdGlobal;
    uint16_t indexGlobalCount;
    uint16_t indexGlobalArray[EXT_MEM_CHUNKED_SIZE_MAX_COUNT];
    
}sGlobalCommand_t;
#else
typedef struct
{
    uint16_t nHead;
    uint16_t nTail;
    bool bFull;
    bool bEmpty;
    bool bMidRelease[SPIMEM_EXT_MEM_QUEUE_COUNT];
}SPIMEM_sQueueBuffer_t;

#endif

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */

SPI_eModule_t eExtMemSPIModule = EXT_MEM_SPI_MODULE;

#if SPIMEM_EXT_MEM_USE_SPIMEM_QUEUE_OLD                              
DMA_eModule_t eModuleDMASPITx  = EXT_MEM_SPI_DMA_TX;    /* DMA Module only used with spimem module queue */
DMA_eModule_t eModuleDMASPIRx  = EXT_MEM_SPI_DMA_RX;

sSPIDMATransferBuffer_t sExtMemRamTrsfBuf[EXT_MEM_RAM_CMD_QUEUE_COUNT];
sExtMemCmd_t sExtMemRamCmd[EXT_MEM_RAM_CMD_QUEUE_COUNT];

sSPIDMATransferBuffer_t sExtMemEepromTrsfBuf[EXT_MEM_EEPROM_CMD_QUEUE_COUNT];
sExtMemCmd_t sExtMemEepromCmd[EXT_MEM_EEPROM_CMD_QUEUE_COUNT];

volatile sGlobalCommand_t asGlobalCommandRead[SPIMEM_MAX_COUNT_SIMULTANEOUS_READS_FROM_EXTERNAL_MEMORY];    /* max count simultaneous reads from ext memory */
uint16_t u16IndexesGlobalCommandArrayProcessReadyRead[SPIMEM_MAX_COUNT_SIMULTANEOUS_READS_FROM_EXTERNAL_MEMORY];
uint16_t u16IndexGlobalCommandArrayRead = 0;

volatile sGlobalCommand_t asGlobalCommand[SPIMEM_MAX_COUNT_SIMULTANEOUS_WRITES_TO_EXTERNAL_MEMORY];    /* max count simultaneous writes to ext memory */
uint16_t u16IndexesGlobalCommandArrayProcessReady[SPIMEM_MAX_COUNT_SIMULTANEOUS_WRITES_TO_EXTERNAL_MEMORY];
uint16_t u16IndexGlobalCommandArray = 0;

volatile sExtMemCmd_t* cmdSetFullRead; 
volatile uint16_t indexSetFullRead;
volatile sExtMemCmd_t* cmdSetFull; 
volatile uint16_t indexSetFull;

uint16_t u16IndexFillLast = 0xFFFF;
sExtMemCmd_t* cmdLast = NULL;
uint16_t memSelectLast = EXT_MEM_SEL_ALL;
sExtMemCmdHandle_t* handleLast = NULL;

volatile uint16_t u16LastIndexSentDetect = 0;

sExtMemCmdHandle_t sExtMemCmdHandle[EXT_MEM_SEL_COUNT];

bool bSPITransferInProgress = 0;

eExtMemSelect_t eLastChipSelect = EXT_MEM_SEL_ALL;

eExtMemSelect_t eTestExtMemSelect = EXT_MEM_SEL_RAM;
sSPIDMATransferBuffer_t sTestExtMemTrsfBuf;
sExtMemCmd_t sTestExtMemCmd;
#else


uint8_t u8SPIBuffer[SPIMEM_EXT_MEM_QUEUE_COUNT][SPIMEM_DATA_BUFFER_SIZE];

SPIMEM_sQueueBuffer_t SPIMEM_sQueueBuffer = 
{
    0,
    0,
    false,
    true,
    {false, false, false, false, false, false, false, false}    /* SPIMEM_EXT_MEM_QUEUE_COUNT times */
};


#endif

uint16_t SPIMEM_u16SnapshotNoAvailableBuffer1 = 0;
uint16_t SPIMEM_u16SnapshotNoAvailableBuffer2 = 0;
uint16_t SPIMEM_u16SnapshotNoAvailableBufferSendSPI = 0;

uint16_t SPIMEM_u16CorruptedRecorderData = 0;

uint32_t u32SPIRamHz = (uint32_t)(SPI_RAM_HZ);
uint32_t u32SPIEepromHz = (uint32_t)(SPI_EEPROM_HZ);
#if USE_SPI_FLIR
uint32_t u32SPIFlirHz = (uint32_t)(SPI_FLIR_HZ);
#endif



#if USE_SPI_FLIR
volatile uint16_t u16DebugData1;
volatile uint16_t u16DebugData2;
volatile uint16_t u16DebugData3;
volatile uint16_t u16DebugData4;

eVoSPIState_t eVoSPIState = VOSPI_IDLE;
eVoSPIState_t eVoSPIStateReq = VOSPI_IDLE;

sSPIDMAFlirTransferBuffer_t sExtMemFlirTrsfBuf[EXT_MEM_FLIR_CMD_QUEUE_COUNT];
sFlirCmd_t sExtMemFlirCmd[EXT_MEM_FLIR_CMD_QUEUE_COUNT];

HMDRV_FILE_PNTR_MODIFIER HMDRV_U8* pFlirImage;        
HMDRV_FILE_PNTR_MODIFIER HMDRV_U8* pFlirImageRead;  

HMDRV_FILE_PNTR_MODIFIER HMDRV_U8* pFlirImagePing; //ping       
HMDRV_FILE_PNTR_MODIFIER HMDRV_U8* pFlirImagePong; //pong

HMDRV_FILE_PNTR_MODIFIER HMDRV_U8* pFlirImageZIPWrite;
HMDRV_FILE_PNTR_MODIFIER HMDRV_U8* pFlirImageZIPWritePing;
HMDRV_FILE_PNTR_MODIFIER HMDRV_U8* pFlirImageZIPWritePong;

HMDRV_U16 FlirImageZIPWriteOffset = 0;

HMDRV_BOOL bFlirImageDenyWrite = HMDRV_FALSE;
HMDRV_BOOL bFlirImageStartWrite = HMDRV_FALSE;

HMDRV_BOOL bFlirImageZIPDenyWrite = HMDRV_FALSE;
HMDRV_BOOL bFlirImageZIPStartWrite = HMDRV_FALSE;

HMDRV_BOOL bFlirImageInMainLoopZIP = HMDRV_FALSE;

HMDRV_U16 FlirImageSwitchPingPong = 0;    /* 0 - do not switch buffer; 1- switch ping pong buffers */
HMDRV_U16 FlirImageSwitchPingPongSPIPointer = 0;
HMDRV_BOOL FlirImageTryFillNextPong = HMDRV_FALSE;
                                

HMDRV_BOOL bFlirImageWritingPongLast = HMDRV_FALSE;
HMDRV_BOOL bFlirImageDenyWritePing = HMDRV_FALSE;
HMDRV_BOOL bFlirImageDenyWritePong = HMDRV_FALSE;

HMDRV_BOOL bFlirImageDenyWriteZIPPing = HMDRV_FALSE;
HMDRV_BOOL bFlirImageDenyWriteZIPPong = HMDRV_FALSE;

HMDRV_BOOL bFlirImageBufferFullPong = HMDRV_TRUE;
HMDRV_BOOL bFlirImageBufferFillPong = HMDRV_FALSE;
HMDRV_BOOL bFlirImageStartUsePong = HMDRV_FALSE;

HMDRV_BOOL bFlirImageBufferFullPing = HMDRV_TRUE;
HMDRV_BOOL bFlirImageBufferFillPing = HMDRV_FALSE;
HMDRV_BOOL bFlirImageStartUsePing = HMDRV_FALSE;

HMDRV_U16 VoSPIPacketLength = IR_CAMERA_BYTES_PER_DOT_RAW*IR_CAMERA_DOTS_PER_LINE_COUNT;
HMDRV_U16 VoSPIPacketModbusLengthPingPong = IR_CAMERA_DOTS_PER_LINE_COUNT;

HMDRV_U16 VoSPIHeaderZIPLength = 2;

eVoSPIPacketZIP_t eVoSPIPacketZIP = VOSPI_ZIP_NONE;
#endif

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */
#if SPIMEM_EXT_MEM_USE_SPIMEM_QUEUE_OLD                              
void vExtMemChipSelectEnable(uint16_t u16Memory);
void vExtMemChipSelectDisableAll(void);
void vExtMemChipSelectDisable(uint16_t u16Memory);
void vInitExtMemRam(void);
void vInitExtMemEeprom(void);
void vExtMemFindSendIndex(sExtMemCmdHandle_t* handle);
uint16_t vExtMemGetFillIndex(sExtMemCmdHandle_t* handle);
#endif
eExtMemSelect_t eMemoryIndexGet(HMDRV_ADDR nDestAddr);

#if SPIMEM_EXT_MEM_USE_SPIMEM_QUEUE_OLD                              
#if SPI_USE_DMA
void vRxCompleteProcess(void);
#else
void vRxCompleteProcess(uint16_t u16TransactionID);
#endif
void vSPICommandSendFromQueue(eExtMemSelect_t eMemoryIndex);
#endif


#if USE_SPI_FLIR
void vInitExtMemFlir(void);
void SPIMEM_vProcessVoSPIReceivedPacket(uint16_t index);
#endif

/* *****************************************************************************
 * Functions definitions
 **************************************************************************** */

#if SPIMEM_EXT_MEM_USE_SPIMEM_QUEUE_OLD                              
/* *****************************************************************************
 * vExtMemChipSelectEnable - SPI Master Chip Select Enable - Used From SPI ISR
 **************************************************************************** */
void vExtMemChipSelectEnable(uint16_t u16Memory)
{
    SPI_vSetFrequencyDivider(eExtMemSPIModule,u16Memory);
    switch((eExtMemSelect_t)u16Memory)
    {
        #if USE_SPI_EEPROM
        case EXT_MEM_SEL_EEPROM:
            CS_EEPROM_SetLow(); /* SPI_CHIP_SELECT_ENABLE */
            break;
        #endif
        #if USE_SPI_RAM
        case EXT_MEM_SEL_RAM:
            CS_RAM_SetLow(); /* SPI_CHIP_SELECT_ENABLE */
            break;
        #endif
        #if USE_SPI_FLIR
        case EXT_MEM_SEL_FLIR:
            CS_FLIR_SetLow(); /* SPI_CHIP_SELECT_ENABLE */
            break;
        #endif
        #if USE_SPI_FPGA
        case EXT_MEM_SEL_FPGA:
            CS_FPGA_SetLow(); /* SPI_CHIP_SELECT_ENABLE */
            break;
        #endif
        default:
            /* do nothing - assume error request */
            break;
    }
}
/* *****************************************************************************
 * vExtMemChipSelectDisable - SPI Master Chip Select Enable - Used From SPI ISR
 **************************************************************************** */
void vExtMemChipSelectDisableAll(void)
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
 * vExtMemChipSelectDisable - SPI Master Chip Select Enable - Used From SPI ISR
 **************************************************************************** */
void vExtMemChipSelectDisable(uint16_t u16Memory)
{
    switch((eExtMemSelect_t)u16Memory)
    {
        #if USE_SPI_EEPROM
        case EXT_MEM_SEL_EEPROM:
            CS_EEPROM_SetHigh(); /* SPI_CHIP_SELECT_DISABLE */
            break;
        #endif
        #if USE_SPI_RAM
        case EXT_MEM_SEL_RAM:
            CS_RAM_SetHigh(); /* SPI_CHIP_SELECT_DISABLE */
            break;
        #endif
        #if USE_SPI_FLIR
        case EXT_MEM_SEL_FLIR:
            CS_FLIR_SetHigh(); /* SPI_CHIP_SELECT_DISABLE */
            break;
        #endif
        #if USE_SPI_FPGA
        case EXT_MEM_SEL_FPGA:
            CS_FPGA_SetHigh(); /* SPI_CHIP_SELECT_DISABLE */
            break;
        #endif
        case EXT_MEM_SEL_ALL:
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
        default:
            /* do nothing - assume error request */
            break;
    }
}
/* *****************************************************************************
 * vInitExtMemRam - external RAM initialization
 **************************************************************************** */
void vInitExtMemRam(void)
{
    uint8_t index;
    
    sExtMemCmdHandle[EXT_MEM_SEL_RAM].u16CmdQueueCount = EXT_MEM_RAM_CMD_QUEUE_COUNT;
    sExtMemCmdHandle[EXT_MEM_SEL_RAM].u16IndexFill = 0;
    sExtMemCmdHandle[EXT_MEM_SEL_RAM].u16IndexSend = 0;
    sExtMemCmdHandle[EXT_MEM_SEL_RAM].eExtMemSelectID = EXT_MEM_SEL_RAM;
    
    sExtMemCmdHandle[EXT_MEM_SEL_RAM].sExtMemCmd = &sExtMemRamCmd[0];
    //sExtMemCmdHandle[EXT_MEM_SEL_RAM].sSPIDMATransferBuffer = &sExtMemRamTrsfBuf[0];
    sExtMemCmdHandle[EXT_MEM_SEL_RAM].sSPIDMATransferBufferBase = &sExtMemRamTrsfBuf[0];
    
    for (index = 0; index < (uint8_t)(sizeof(sExtMemRamCmd)/sizeof(sExtMemRamCmd[0])); index++)
    {
        sExtMemRamCmd[index].eExtMemStatus = EXT_MEM_STA_IDLE;
    }
}

/* *****************************************************************************
 * vInitExtMemEeprom - external EEPROM initialization
 **************************************************************************** */
void vInitExtMemEeprom(void)
{
    uint8_t index;
    
    sExtMemCmdHandle[EXT_MEM_SEL_EEPROM].u16CmdQueueCount = EXT_MEM_EEPROM_CMD_QUEUE_COUNT;
    sExtMemCmdHandle[EXT_MEM_SEL_EEPROM].u16IndexFill = 0;
    sExtMemCmdHandle[EXT_MEM_SEL_EEPROM].u16IndexSend = 0;
    sExtMemCmdHandle[EXT_MEM_SEL_EEPROM].eExtMemSelectID = EXT_MEM_SEL_EEPROM;
    
    sExtMemCmdHandle[EXT_MEM_SEL_EEPROM].sExtMemCmd = &sExtMemEepromCmd[0];
    //sExtMemCmdHandle[EXT_MEM_SEL_EEPROM].sSPIDMATransferBuffer = &sExtMemEepromTrsfBuf[0];
    sExtMemCmdHandle[EXT_MEM_SEL_EEPROM].sSPIDMATransferBufferBase = &sExtMemEepromTrsfBuf[0];
    
    for (index = 0; index < (uint8_t)(sizeof(sExtMemEepromCmd)/sizeof(sExtMemEepromCmd[0])); index++)
    {
        sExtMemEepromCmd[index].eExtMemStatus = EXT_MEM_STA_IDLE;
    }
}


/* *****************************************************************************
 * vExtMemFindSendIndex - Find Next Queue Index To be sent
 **************************************************************************** */
void vExtMemFindSendIndex(sExtMemCmdHandle_t* handle)
{
    sExtMemCmd_t* command;
    
    command = handle->sExtMemCmd;    
    
    if ((command[handle->u16IndexSend].eExtMemStatus == EXT_MEM_STA_IDLE) 
     || (command[handle->u16IndexSend].eExtMemStatus == EXT_MEM_STA_READ))
    {
        if (handle->u16IndexFill != handle->u16IndexSend)
        {
            handle->u16IndexSend++;
            if (handle->u16IndexSend >= handle->u16CmdQueueCount)
            {
                handle->u16IndexSend = 0;
            }
            //if (command[handle->u16IndexSend]->eExtMemStatus != EXT_MEM_STA_IDLE)
            //{
            //    break;
            //}
        }
    }
}



/* *****************************************************************************
 * vExtMemGetFillIndex - Find Next Queue Index To Fill
 **************************************************************************** */
uint16_t vExtMemGetFillIndex(sExtMemCmdHandle_t* handle)   
{
    uint16_t index;
    uint16_t u16Result;
    sExtMemCmd_t* command;

    command = handle->sExtMemCmd;      
    u16Result = handle->u16CmdQueueCount;/* invalid index by default */
    
    ISR_vEnterCritical();
    index = handle->u16IndexFill;
    if (command[index].eExtMemStatus != EXT_MEM_STA_IDLE)
    {
        index++;
        if (index >= handle->u16CmdQueueCount)
        {
            index = 0;
        }
        if (index != handle->u16IndexSend)
        {
            /* index can be used */
            command[index].eExtMemStatus = EXT_MEM_STA_FILL;
            u16Result = index;
            
            handle->u16IndexFill = index;
            /* Try advance Index */
            index++;
            if (index >= handle->u16CmdQueueCount)
            {
                index = 0;
            }
            if (index != handle->u16IndexSend)
            {
                handle->u16IndexFill = index;
            }

        }
        else
        {
            /* reached send index - not possible and not valid */
            //handle->u16HandleStatus |= (1 << 15);
        }
    }
    else
    {
        /* index can be used */
        command[index].eExtMemStatus = EXT_MEM_STA_FILL;
        u16Result = index;
        
        handle->u16IndexFill = index;
        /* Try advance Index */
        index++;
        if (index >= handle->u16CmdQueueCount)
        {
            index = 0;
        }
        if (index != handle->u16IndexSend)
        {
            handle->u16IndexFill = index;
        }
    }
    ISR_vExitCritical();
    return u16Result;
}
#endif


/* *****************************************************************************
 * eMemoryIndexGet - Get Memory Index From Virtual Address
 **************************************************************************** */
eExtMemSelect_t eMemoryIndexGet(HMDRV_ADDR nDestAddr)
{
    eExtMemSelect_t eResult;
    
    if (nDestAddr >= EXT_MEM_RAM_VIRTUAL_ADDRESS)
    {
        eResult = EXT_MEM_SEL_RAM;
    }
    #if USE_SPI_FLIR
    else
    if (nDestAddr >= EXT_MEM_SPI_FLIR_VIRTUAL_ADDRESS)
    {
        eResult = EXT_MEM_SEL_FLIR;
    }
    #endif
    else
    {
        eResult = EXT_MEM_SEL_EEPROM;
    }
    return eResult;
}

#if SPIMEM_EXT_MEM_USE_SPIMEM_QUEUE_OLD                              
/******************************************************************************
 * vRxCompleteProcess - Received SPI data process
 *****************************************************************************/
#if SPI_USE_DMA
void vRxCompleteProcess(void)
{
    vExtMemChipSelectDisable((uint16_t)EXT_MEM_SEL_ALL);
    bSPITransferInProgress = 0;
   
    /* Queue Transfer state clear */
    
    if (sExtMemCmdHandle[eLastChipSelect].sExtMemCmd[sExtMemCmdHandle[eLastChipSelect].u16IndexSend].eExtMemCommand == EXT_MEM_CMD_RD)
    {
        sExtMemCmdHandle[eLastChipSelect].sExtMemCmd[sExtMemCmdHandle[eLastChipSelect].u16IndexSend].eExtMemStatus = EXT_MEM_STA_READ; 
    }
    else
    {
        sExtMemCmdHandle[eLastChipSelect].sExtMemCmd[sExtMemCmdHandle[eLastChipSelect].u16IndexSend].eExtMemStatus = EXT_MEM_STA_IDLE; 
    }
    
    //sExtMemCmdHandle[eLastChipSelect].sExtMemCmd[sExtMemCmdHandle[eLastChipSelect].u16IndexSend].eExtMemStatus = EXT_MEM_STA_IDLE; 
    
    
 
    #if USE_SPI_FLIR
    /* Flir Automatic Process After Transfer and request new transfer */
    if (eLastChipSelect == EXT_MEM_SEL_FLIR)
    {
        if (eVoSPIState == VOSPI_RUN)
        {
            
            SPIMEM_vSwitchFlirImagePingPong();
            
            u16LastIndexSentDetect = sExtMemCmdHandle[eLastChipSelect].u16IndexSend;
            SPIMEM_vProcessVoSPIReceivedPacket(sExtMemCmdHandle[eLastChipSelect].u16IndexSend);
            if (eVoSPIStateReq == VOSPI_SINGLE)
            {
                eVoSPIStateReq = VOSPI_STOP;
                eVoSPIState = VOSPI_STOP;
            }
            else
            {
                SPIMEM_vGetFlirVoSPIPacket(VoSPIPacketLength + 4);    /* Raw14 Mode - consider use variable here */
            }
        }
    }
    vSPICommandSendFromQueue(EXT_MEM_SEL_FLIR);      /* process next command if available */
    #endif

    vSPICommandSendFromQueue(EXT_MEM_SEL_RAM);       /* process next command if available */
    vSPICommandSendFromQueue(EXT_MEM_SEL_EEPROM);    /* process next command if available */
}
#else
void vRxCompleteProcess(uint16_t u16TransactionID) 
{
    /* Queue Transfer state clear */
    eExtMemSelect_t eMemoryIndex = ((u16TransactionID >> 12) & 0x000F); /* Highest 4 Bits are the memory(chip select) index */
    uint16_t u16QueueIndex = (u16TransactionID & 0x0FFF);               /* Low 12 Bits are the queue (handle) index */
    
    if (sExtMemCmdHandle[eMemoryIndex].sExtMemCmd[u16QueueIndex].eExtMemCommand == EXT_MEM_CMD_RD)
    {
        sExtMemCmdHandle[eMemoryIndex].sExtMemCmd[u16QueueIndex].eExtMemStatus = EXT_MEM_STA_READ; 
    }
    else
    {
        sExtMemCmdHandle[eMemoryIndex].sExtMemCmd[u16QueueIndex].eExtMemStatus = EXT_MEM_STA_IDLE; 
    }
    
    //sExtMemCmdHandle[eMemoryIndex].sExtMemCmd[u16QueueIndex].eExtMemStatus = EXT_MEM_STA_IDLE; 
    
    
 
    #if USE_SPI_FLIR
    /* Flir Automatic Process After Transfer and request new transfer */
    if (eMemoryIndex == EXT_MEM_SEL_FLIR)
    {
        if (eVoSPIState == VOSPI_RUN)
        {
            
            SPIMEM_vSwitchFlirImagePingPong();
            
            u16LastIndexSentDetect = u16QueueIndex;
            SPIMEM_vProcessVoSPIReceivedPacket(u16QueueIndex);
            if (eVoSPIStateReq == VOSPI_SINGLE)
            {
                eVoSPIStateReq = VOSPI_STOP;
                eVoSPIState = VOSPI_STOP;
            }
            else
            {
                SPIMEM_vGetFlirVoSPIPacket(VoSPIPacketLength + 4);    /* Raw14 Mode - consider use variable here */
            }
        }
    }
    vSPICommandSendFromQueue(EXT_MEM_SEL_FLIR);      /* process next command if available */
    #endif

    vSPICommandSendFromQueue(EXT_MEM_SEL_RAM);       /* process next command if available */
    vSPICommandSendFromQueue(EXT_MEM_SEL_EEPROM);    /* process next command if available */
}
#endif



/* *****************************************************************************
 * vSPICommandSendFromQueue
 **************************************************************************** */
#if SPI_USE_DMA
void vSPICommandSendFromQueue(eExtMemSelect_t eMemoryIndex)
{
    static uint16_t SPIFunctionProcess = 0; /* Prevent mix function execution from different interrupt levels */

    DMA_sBaseRegisters_t* psBaseDMATx;
    uint8_t u8BitOffsetsISRDMATx;

    DMA_sBaseRegisters_t* psBaseDMARx;
    uint8_t u8BitOffsetsISRDMARx;

    
    uint16_t index;
    sExtMemCmdHandle_t* handle;
    sExtMemCmd_t* command;
    sSPIDMATransferBuffer_t* buffer;
    
    #if USE_SPI_FLIR
    sFlirCmd_t* commandFlir;
    sSPIDMAFlirTransferBuffer_t* bufferFlir;
    #endif

    ISR_vEnterCritical();
    if (SPIFunctionProcess > 0)
    {
        ISR_vExitCritical();
    }
    else //if (SPIFunctionProcess == 0)
    {
        SPIFunctionProcess = 1;
        ISR_vExitCritical();
        psBaseDMATx = DMA_sHandles.psBase + eModuleDMASPITx;
        u8BitOffsetsISRDMATx = DMA_sHandles.u8BitOffsetsISR[eModuleDMASPITx];
        psBaseDMARx = DMA_sHandles.psBase + eModuleDMASPIRx;
        u8BitOffsetsISRDMARx = DMA_sHandles.u8BitOffsetsISR[eModuleDMASPIRx];
        
        handle = &sExtMemCmdHandle[eMemoryIndex];
        
        ISR_vEnterCritical();
        if (bSPITransferInProgress > 0)
        {
            /* Previous command execution not completed */
            ISR_vExitCritical();
        }
        else //if (bSPITransferInProgress == 0)
        {
            //bSPITransferInProgress = 1;
            
            vExtMemFindSendIndex(handle);

            index = handle->u16IndexSend;

            
            #if USE_SPI_FLIR
            if (eMemoryIndex == EXT_MEM_SEL_FLIR)
            {
 
                commandFlir = (sFlirCmd_t*)handle->sExtMemCmd;
                bufferFlir = (sSPIDMAFlirTransferBuffer_t*)handle->sSPIDMATransferBuffer;

                commandFlir += index;
                
                if (commandFlir->eExtMemStatus == EXT_MEM_STA_FULL)
                {
                    psBaseDMATx->DMACONbits.CHEN = 0;   // Disable DMA Channel
                    psBaseDMARx->DMACONbits.CHEN = 0;   // Disable DMA Channel

                    /* DMAxCNT must be one less than the transfer count */
                    psBaseDMATx->DMACNT = commandFlir->ExtMemDataCnt-1;   /* Command Bytes Count */
                    psBaseDMARx->DMACNT = commandFlir->ExtMemDataCnt-1;   /* Command Bytes Count */
               
                
                    psBaseDMATx->DMASTAL = ( uint16_t ) bufferFlir;
                    psBaseDMATx->DMASTAH = ( uint16_t )((uint32_t)(uint16_t)(bufferFlir) >> 16);      /* Should be 0 or read the datasheet */
                    psBaseDMARx->DMASTAL = ( uint16_t ) bufferFlir;
                    psBaseDMARx->DMASTAH = ( uint16_t )((uint32_t)(uint16_t)(bufferFlir) >> 16);      /* Should be 0 or read the datasheet */

                    ISR_vClrBitIFS(u8BitOffsetsISRDMATx);    // Clear DMA interrupt
                    ISR_vClrBitIFS(u8BitOffsetsISRDMARx);    // Clear DMA interrupt

                    psBaseDMATx->DMACONbits.CHEN = 1;   // Enable DMA Channel
                    psBaseDMARx->DMACONbits.CHEN = 1;   // Enable DMA Channel

                    commandFlir->eExtMemStatus = EXT_MEM_STA_BUSY;
                    //sExtMemRamCmd.eExtMemCommand = EXT_MEM_CMD_IDLE;    /* consider clear here or not */

                    bSPITransferInProgress = 1;
                    vExtMemChipSelectEnable((uint16_t)eMemoryIndex);
                    eLastChipSelect = eMemoryIndex;
                    psBaseDMATx->DMAREQbits.FORCE = 1;  /* Start DMA Transfer */


                }
                else
                {
                    /* nothing requested */
                }
            }
            else
            #endif
            {
            
                command = handle->sExtMemCmd;
                buffer = handle->sSPIDMATransferBufferBase; 

                command += index;
                buffer += index; // buffer not explicitly specified and index is needed

                if (command->eExtMemStatus == EXT_MEM_STA_FULL)
                {
                    psBaseDMATx->DMACONbits.CHEN = 0;   // Disable DMA Channel
                    psBaseDMARx->DMACONbits.CHEN = 0;   // Disable DMA Channel

                    buffer->Command                 = (uint8_t)(command->eExtMemCommand);
                    buffer->AddressHiOrStatusData   = (uint8_t)(command->ExtMemAddress >> 16);
                    buffer->AddressMi               = (uint8_t)(command->ExtMemAddress >>  8);
                    buffer->AddressLo               = (uint8_t)(command->ExtMemAddress >>  0);

                    /* DMAxCNT must be one less than the transfer count */
                    psBaseDMATx->DMACNT = 1-1;   /* Command Bytes Count */
                    psBaseDMARx->DMACNT = 1-1;   /* Command Bytes Count */



                    if (command->eExtMemCommand == EXT_MEM_CMD_WR)
                    {
                        if (eMemoryIndex == EXT_MEM_SEL_EEPROM)
                        {
                            handle->u16HandleID++;
                        }

                        if (eMemoryIndex == EXT_MEM_SEL_RAM)
                        {
                            handle->u16HandleID--;
                        }

                        psBaseDMATx->DMACNT += 3;   /* Address Bytes Count */                    
                        psBaseDMARx->DMACNT += 3;   /* Address Bytes Count */
                        psBaseDMATx->DMASTAL = ( uint16_t ) buffer;
                        psBaseDMATx->DMASTAH = ( uint16_t )((uint32_t)(uint16_t)(buffer) >> 16);      /* Should be 0 or read the datasheet */
                        psBaseDMARx->DMASTAL = ( uint16_t ) buffer;
                        psBaseDMARx->DMASTAH = ( uint16_t )((uint32_t)(uint16_t)(buffer) >> 16);      /* Should be 0 or read the datasheet */

                    }
                    else if (command->eExtMemCommand == EXT_MEM_CMD_RD)
                    {
                        if (eMemoryIndex == EXT_MEM_SEL_EEPROM)
                        {
                            handle->u16HandleID += 2;
                        }
                        if (eMemoryIndex == EXT_MEM_SEL_RAM)
                        {
                            handle->u16HandleID -= 2;
                        }

                        psBaseDMATx->DMACNT += 3;   /* Address Bytes Count */                    
                        psBaseDMARx->DMACNT += 3;   /* Address Bytes Count */
                        psBaseDMATx->DMASTAL = ( uint16_t ) buffer;
                        psBaseDMATx->DMASTAH = ( uint16_t )((uint32_t)(uint16_t)(buffer) >> 16);      /* Should be 0 or read the datasheet */
                        psBaseDMARx->DMASTAL = ( uint16_t ) buffer;
                        psBaseDMARx->DMASTAH = ( uint16_t )((uint32_t)(uint16_t)(buffer) >> 16);      /* Should be 0 or read the datasheet */
                    }
                    else    /* All other commands */
                    {
                        if (eMemoryIndex == EXT_MEM_SEL_EEPROM)
                        {
                            handle->u16HandleID += 3;
                        }
                        if (eMemoryIndex == EXT_MEM_SEL_RAM)
                        {
                            handle->u16HandleID -= 3;
                        }

                        psBaseDMATx->DMASTAL = ( uint16_t ) buffer;
                        psBaseDMATx->DMASTAH = ( uint16_t )((uint32_t)(uint16_t)(buffer) >> 16);      /* Should be 0 or read the datasheet */
                        psBaseDMARx->DMASTAL = ( uint16_t ) buffer;
                        psBaseDMARx->DMASTAH = ( uint16_t )((uint32_t)(uint16_t)(buffer) >> 16);      /* Should be 0 or read the datasheet */
                    }


                    psBaseDMATx->DMACNT += command->ExtMemDataCnt;  /* To be modified only when DMA Channel is disabled */
                    psBaseDMARx->DMACNT += command->ExtMemDataCnt;  /* To be modified only when DMA Channel is disabled */



                    ISR_vClrBitIFS(u8BitOffsetsISRDMATx);    // Clear DMA interrupt
                    ISR_vClrBitIFS(u8BitOffsetsISRDMARx);    // Clear DMA interrupt

                    psBaseDMATx->DMACONbits.CHEN = 1;   // Enable DMA Channel
                    psBaseDMARx->DMACONbits.CHEN = 1;   // Enable DMA Channel

                    command->eExtMemStatus = EXT_MEM_STA_BUSY;
                    //sExtMemRamCmd.eExtMemCommand = EXT_MEM_CMD_IDLE;    /* consider clear here or not */

                    bSPITransferInProgress = 1;
                    vExtMemChipSelectEnable((uint16_t)eMemoryIndex);
                    eLastChipSelect = eMemoryIndex;
                    psBaseDMATx->DMAREQbits.FORCE = 1;  /* Start DMA Transfer */

                }
                else
                {
                    /* nothing requested */
                }
            
            }
            ISR_vExitCritical();
        }
        SPIFunctionProcess = 0;
    }
}
#else
void vSPICommandSendFromQueue(eExtMemSelect_t eMemoryIndex)
{
    static bool SPIFunctionProcess = false; /* Prevent mix function execution from different interrupt levels */
    
    uint16_t index;
    sExtMemCmdHandle_t* handle;
    sExtMemCmd_t* command;
    sSPIDMATransferBuffer_t* buffer;
    
    #if USE_SPI_FLIR
    sFlirCmd_t* commandFlir;
    sSPIDMAFlirTransferBuffer_t* bufferFlir;
    #endif

    ISR_vEnterCritical();
    if(SPIFunctionProcess == true)
    {
        ISR_vExitCritical();
    }
    else
    {
        SPIFunctionProcess = true;
        if (SPI_bQueueIsFull(eExtMemSPIModule) == true)    /* in whole project configuration this is called once or must be called from same level (ISR, main loop level) */
        {
            SPIFunctionProcess = false;
            ISR_vExitCritical();
        }
        else
        {
            handle = &sExtMemCmdHandle[eMemoryIndex]; 
            vExtMemFindSendIndex(handle);
            ISR_vExitCritical();
            index = handle->u16IndexSend;

            handle->u16TransactionID = (handle->u16IndexSend) | (eMemoryIndex << 12);    /* used To Apply Status Feedback When Transaction is Completed in vRxCompleteProcess */

            #if USE_SPI_FLIR
            if (eMemoryIndex == EXT_MEM_SEL_FLIR)
            {
                commandFlir = (sFlirCmd_t*)handle->sExtMemCmd;
                bufferFlir = (sSPIDMAFlirTransferBuffer_t*)handle->sSPIDMATransferBuffer;

                commandFlir += index;

                if (commandFlir->eExtMemStatus == EXT_MEM_STA_FULL)
                {

                    handle->u16HandleID = 2;

                    SPI_ReadWriteBuffer ( (eExtMemSPIModule | (eMemoryIndex << 4)), (uint8_t*)bufferFlir, commandFlir->ExtMemDataCnt, (SPI_eTransferStatus_t*)(&(handle->u16HandleStatusSPI)), handle->u16TransactionID);

                    commandFlir->eExtMemStatus = EXT_MEM_STA_BUSY;
                    //sExtMemRamCmd.eExtMemCommand = EXT_MEM_CMD_IDLE;    /* consider clear here or not */


                    // to be made in spi.c
                    //vExtMemChipSelectEnable((uint16_t)eMemoryIndex);
                    //eLastChipSelect = eMemoryIndex;


                }
                else
                {
                    /* nothing requested */
                }
            }
            else
            #endif
            {
                command = handle->sExtMemCmd;
                buffer = handle->sSPIDMATransferBufferBase; 

                command += index;
                buffer += index; // buffer not explicitly specified and index is needed

                if (command->eExtMemStatus == EXT_MEM_STA_FULL)
                {
                    uint16_t u16Count;

                    buffer->Command                 = (uint8_t)(command->eExtMemCommand);
                    buffer->AddressHiOrStatusData   = (uint8_t)(command->ExtMemAddress >> 16);
                    buffer->AddressMi               = (uint8_t)(command->ExtMemAddress >>  8);
                    buffer->AddressLo               = (uint8_t)(command->ExtMemAddress >>  0);

                    u16Count = 1;   /* Command Bytes Count */

                    if (command->eExtMemCommand == EXT_MEM_CMD_WR)
                    {
                        u16Count += 3;   /* Address Bytes Count */                    
                    }
                    else if (command->eExtMemCommand == EXT_MEM_CMD_RD)
                    {
                        u16Count += 3;   /* Address Bytes Count */                    
                    }
                    else    /* All other commands */
                    {
                    }

                    u16Count += command->ExtMemDataCnt;  /* Data Bytes Count */

                    SPI_ReadWriteBuffer ( (eExtMemSPIModule | (eMemoryIndex << 4)), (uint8_t*)buffer, u16Count, (SPI_eTransferStatus_t*)(&(handle->u16HandleStatusSPI)), handle->u16TransactionID);

                    command->eExtMemStatus = EXT_MEM_STA_BUSY;
                    //sExtMemRamCmd.eExtMemCommand = EXT_MEM_CMD_IDLE;    /* consider clear here or not */

                    //to be made in spi.c
                    //vExtMemChipSelectEnable((uint16_t)eMemoryIndex);
                    //eLastChipSelect = eMemoryIndex;

                }
                else
                {
                    /* nothing requested */
                }

            }
            SPIFunctionProcess = false;
        }
    }
}
#endif
        
#endif /* #if SPIMEM_EXT_MEM_USE_SPIMEM_QUEUE_OLD */

/* *****************************************************************************
 * SPI Initialization
 **************************************************************************** */


/* *****************************************************************************
 * SPIMEM_vInit
 * 
 * Description: Module Initialization
 **************************************************************************** */
void SPIMEM_vInit(uint32_t u32Fcy)
{
    #ifndef BOARDCFG_USE_MCC_SPI
    SPI_vInit(eExtMemSPIModule, SPI_USE_DMA);   /* used for the External Memory (SPI_vQueueInitialize() Inside) */
    #else
    SPI_vQueueInitialize(eExtMemSPIModule);
    #endif
    
    
#if SPIMEM_EXT_MEM_USE_SPIMEM_QUEUE_OLD                              
    SPI_vRxCompleteFunctionsInit(eExtMemSPIModule, vRxCompleteProcess );
#endif

    SPI_vInitializeSPIPrescaler(SPI_MEM_RAM,u32SPIRamHz,u32Fcy);
    SPI_vInitializeSPIPrescaler(SPI_MEM_EEPROM,u32SPIEepromHz,u32Fcy);
#if USE_SPI_FLIR
    SPI_vInitializeSPIPrescaler(SPI_MEM_FLIR,u32SPIFlirHz,u32Fcy);
#endif
    
#if SPI_USE_DMA
    /* SPI Receive with DMA Support Init */
    ISR_vSetISRDMA(vRxCompleteProcess, eModuleDMASPIRx);
    SPI_vInitRxDMA(eExtMemSPIModule, eModuleDMASPIRx);
    
    /* SPI Transmit with DMA Support Init */
    SPI_vInitTxDMA(eExtMemSPIModule, eModuleDMASPITx);
#endif
    
#if SPIMEM_EXT_MEM_USE_SPIMEM_QUEUE_OLD                              
    vInitExtMemRam();
    vInitExtMemEeprom();   
#endif
    #if USE_SPI_FLIR
    vInitExtMemFlir();    
    #endif
}



/* *****************************************************************************
 * SPI External Memory Interface Functions
 **************************************************************************** */



#if SPIMEM_EXT_MEM_USE_SPIMEM_QUEUE_OLD                              
/* *****************************************************************************
 * SPIMEM_vExtMemRamToIntRam
 * 
 * Description: Copy Data from external memory to internal RAM and 
 *              returns pointer to internal RAM
 **************************************************************************** */
HMDRV_ADDR SPIMEM_pExtMemRamToIntRam(HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSizeReq, HMDRV_ADDR_PNTR_MODIFIER HMDRV_SIZE8* pnSizeRead, HMDRV_BUFF_PNTR_MODIFIER HMDRV_U16* pu16GlobalQueueID)
{
    HMDRV_U8* ps = NULL;
    uint16_t indexRead;
    sExtMemCmdHandle_t* handle;
    //eExtMemStatus_t status;
    
    uint16_t indexCurrentGlobalCommandArray;  
        
    ISR_vEnterCritical();
    if (u16IndexGlobalCommandArrayRead >= SPIMEM_MAX_COUNT_SIMULTANEOUS_READS_FROM_EXTERNAL_MEMORY)
    {
        u16IndexGlobalCommandArrayRead = 0;
    }
    indexCurrentGlobalCommandArray = u16IndexGlobalCommandArrayRead;
    u16IndexGlobalCommandArrayRead++;
    ISR_vExitCritical();
    
    *pu16GlobalQueueID = indexCurrentGlobalCommandArray;

    asGlobalCommandRead[indexCurrentGlobalCommandArray].memSelectGlobal = eMemoryIndexGet(nSrcAddr);
    handle = &sExtMemCmdHandle[0];
    handle += asGlobalCommandRead[indexCurrentGlobalCommandArray].memSelectGlobal;
    indexRead = vExtMemGetFillIndex(handle);
    
    asGlobalCommandRead[indexCurrentGlobalCommandArray].cmdGlobal = handle->sExtMemCmd;
    
    asGlobalCommandRead[indexCurrentGlobalCommandArray].indexGlobalCount = 0;
    asGlobalCommandRead[indexCurrentGlobalCommandArray].indexGlobalArray[0] = 0xFFFF;
    
    
    if (nSizeReq > SPIMEM_DATA_BUFFER_SIZE)
    {
        nSizeReq = SPIMEM_DATA_BUFFER_SIZE;
    }
    
    *pnSizeRead = nSizeReq;

    
    if (indexRead < handle->u16CmdQueueCount)
    {  
        //Made in vExtMemGetFillIndex()  cmd[indexRead].eExtMemStatus = EXT_MEM_STA_FILL;
        asGlobalCommandRead[indexCurrentGlobalCommandArray].cmdGlobal[indexRead].ExtMemAddress = nSrcAddr;
        asGlobalCommandRead[indexCurrentGlobalCommandArray].cmdGlobal[indexRead].eExtMemCommand = EXT_MEM_CMD_RD; 
        asGlobalCommandRead[indexCurrentGlobalCommandArray].cmdGlobal[indexRead].ExtMemDataCnt = nSizeReq;
        //handle->sSPIDMATransferBuffer = handle->sSPIDMATransferBufferBase + indexRead;
        //ps = (HMDRV_U8*) handle->sSPIDMATransferBuffer->Data;
        ps = (HMDRV_U8*) handle->sSPIDMATransferBufferBase[indexRead].Data;
        asGlobalCommandRead[indexCurrentGlobalCommandArray].cmdGlobal[indexRead].eExtMemStatus = EXT_MEM_STA_FULL;
        asGlobalCommandRead[indexCurrentGlobalCommandArray].indexGlobalArray[asGlobalCommandRead[indexCurrentGlobalCommandArray].indexGlobalCount] = indexRead;
        asGlobalCommandRead[indexCurrentGlobalCommandArray].indexGlobalCount++; //for now not needed
        vSPICommandSendFromQueue(asGlobalCommandRead[indexCurrentGlobalCommandArray].memSelectGlobal);
        
        /* wait SPI Command Complete */
        //while ((asGlobalCommandRead[indexCurrentGlobalCommandArray].cmdGlobal[indexRead].eExtMemStatus) != EXT_MEM_STA_IDLE)
        while ((asGlobalCommandRead[indexCurrentGlobalCommandArray].cmdGlobal[indexRead].eExtMemStatus) != EXT_MEM_STA_READ)
        {
            //ClrWdt();
        };
   
    }
    else
    {
        /* Command Not Executed. No available buffer found */
        //handle->u16HandleStatus |= (1 << 5);
    }
    return (HMDRV_ADDR)(HMDRV_ADDR_SIZE)ps;
}



/* *****************************************************************************
 * SPIMEM_vReleaseQueueBuffer
 * 
 * Description: 
 **************************************************************************** */
void SPIMEM_vReleaseQueueBuffer(HMDRV_BUFF_PNTR_MODIFIER HMDRV_U16* pu16GlobalQueueID)
{
    uint16_t indexGlobalArrayLoop;
    uint16_t indexProcessReadyLoop = 0xFFFF;
    
    
    
    indexProcessReadyLoop = *pu16GlobalQueueID;
    *pu16GlobalQueueID = 0xFFFF;
    if (indexProcessReadyLoop < SPIMEM_MAX_COUNT_SIMULTANEOUS_READS_FROM_EXTERNAL_MEMORY)
    {
    
    //u16IndexesGlobalCommandArrayProcessReadyRead[*pu16GlobalQueueID] = 1;
    
    //for (indexProcessReadyLoop = 0; indexProcessReadyLoop < SPIMEM_MAX_COUNT_SIMULTANEOUS_READS_FROM_EXTERNAL_MEMORY; indexProcessReadyLoop++)
    {
        //if (u16IndexesGlobalCommandArrayProcessReadyRead[indexProcessReadyLoop] > 0)
        {
            //u16IndexesGlobalCommandArrayProcessReadyRead[indexProcessReadyLoop] = 0;
            
            
            //cmdGlobal[indexGlobal].eExtMemStatus = EXT_MEM_STA_FULL;
            for (indexGlobalArrayLoop = 0; indexGlobalArrayLoop < asGlobalCommandRead[indexProcessReadyLoop].indexGlobalCount; indexGlobalArrayLoop++)
            {
                indexSetFullRead = asGlobalCommandRead[indexProcessReadyLoop].indexGlobalArray[indexGlobalArrayLoop];
                cmdSetFullRead = (sExtMemCmd_t*)asGlobalCommandRead[indexProcessReadyLoop].cmdGlobal;
                cmdSetFullRead += indexSetFullRead;
                cmdSetFullRead->eExtMemStatus = EXT_MEM_STA_IDLE;
            }
            //vSPICommandSendFromQueue(asGlobalCommand[indexProcessReadyLoop].memSelectGlobal); not needed for read command
            
        }
    }
    
    }
}

/* *****************************************************************************
 * SPIMEM_pExtMemRamFromIntRam
 * 
 * Description: returns pointer to internal RAM for 
 *              Copy Data to external memory from internal RAM 
 *              
 **************************************************************************** */
HMDRV_ADDR SPIMEM_pExtMemRamFromIntRam(HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize, HMDRV_BUFF_PNTR_MODIFIER HMDRV_U16* pu16GlobalQueueID)
{
    HMDRV_U8* pd = NULL;
    uint16_t index;
    uint16_t indexFirst;
    sExtMemCmdHandle_t* handle;
    uint16_t indexCurrentGlobalCommandArray;
    
        
    ISR_vEnterCritical();
    if (u16IndexGlobalCommandArray >= SPIMEM_MAX_COUNT_SIMULTANEOUS_WRITES_TO_EXTERNAL_MEMORY)
    {
        u16IndexGlobalCommandArray = 0;
    }
    indexCurrentGlobalCommandArray = u16IndexGlobalCommandArray;
    u16IndexGlobalCommandArray++;
    ISR_vExitCritical();
    
    *pu16GlobalQueueID = indexCurrentGlobalCommandArray;

    asGlobalCommand[indexCurrentGlobalCommandArray].memSelectGlobal = eMemoryIndexGet(nDestAddr);
    handle = &sExtMemCmdHandle[0];
    handle += asGlobalCommand[indexCurrentGlobalCommandArray].memSelectGlobal;
    index = vExtMemGetFillIndex(handle);
    
    asGlobalCommand[indexCurrentGlobalCommandArray].cmdGlobal = handle->sExtMemCmd;
    
    asGlobalCommand[indexCurrentGlobalCommandArray].indexGlobalCount = 0;
    asGlobalCommand[indexCurrentGlobalCommandArray].indexGlobalArray[0] = 0xFFFF;
    
    if (index < handle->u16CmdQueueCount)
    {  
        
        /* if EEPROM Memory - WREN Each Write Command */
        if (asGlobalCommand[indexCurrentGlobalCommandArray].memSelectGlobal == EXT_MEM_SEL_EEPROM)
        {
            //Made in vExtMemGetFillIndex()  cmd[index].eExtMemStatus = EXT_MEM_STA_FILL;
            asGlobalCommand[indexCurrentGlobalCommandArray].cmdGlobal[index].ExtMemAddress = nDestAddr;
            asGlobalCommand[indexCurrentGlobalCommandArray].cmdGlobal[index].eExtMemCommand = EXT_MEM_CMD_WREN; 
            asGlobalCommand[indexCurrentGlobalCommandArray].cmdGlobal[index].ExtMemDataCnt = 0;
            //handle->sSPIDMATransferBuffer = handle->sSPIDMATransferBufferBase + index;
            asGlobalCommand[indexCurrentGlobalCommandArray].cmdGlobal[index].eExtMemStatus = EXT_MEM_STA_FULL;
            vSPICommandSendFromQueue(asGlobalCommand[indexCurrentGlobalCommandArray].memSelectGlobal);
            indexFirst = index;
            index = vExtMemGetFillIndex(handle);
            
            indexFirst++;
            if (indexFirst >= handle->u16CmdQueueCount)
            {
                indexFirst = 0;
            }
            if (indexFirst != index)    
            {
                /* detected WREN not exactly before WR */
                //handle->u16HandleStatus |= (1 << 8);
            }
            
        }
        while(nSize > 0)
        {
            
            if (index < handle->u16CmdQueueCount)
            {  

                //Made in vExtMemGetFillIndex()  cmdGlobal[index].eExtMemStatus = EXT_MEM_STA_FILL;
                asGlobalCommand[indexCurrentGlobalCommandArray].cmdGlobal[index].ExtMemAddress = nDestAddr;
                asGlobalCommand[indexCurrentGlobalCommandArray].cmdGlobal[index].eExtMemCommand = EXT_MEM_CMD_WR; 
                
                
                if (nSize > SPIMEM_DATA_BUFFER_SIZE)
                {
                    asGlobalCommand[indexCurrentGlobalCommandArray].cmdGlobal[index].ExtMemDataCnt = SPIMEM_DATA_BUFFER_SIZE;
                    nSize -= SPIMEM_DATA_BUFFER_SIZE;
                }
                else
                {
                    asGlobalCommand[indexCurrentGlobalCommandArray].cmdGlobal[index].ExtMemDataCnt = nSize;
                    nSize = 0;
                }
                
                
                //handle->sSPIDMATransferBuffer = handle->sSPIDMATransferBufferBase + index;


                //ps = (HMDRV_U8*) pSrcBuff;
                //pd = (HMDRV_U8*) handle->sSPIDMATransferBuffer->Data;
                pd = (HMDRV_U8*) handle->sSPIDMATransferBufferBase[index].Data;

                //while(nSize--)
                //    *pd++ = *ps++;
                
                //cmdGlobal[index].eExtMemStatus = EXT_MEM_STA_FULL;
                //vSPICommandSendFromQueue(memSelectGlobal);
                
                //indexGlobal = index;
                asGlobalCommand[indexCurrentGlobalCommandArray].indexGlobalArray[asGlobalCommand[indexCurrentGlobalCommandArray].indexGlobalCount] = index;
                asGlobalCommand[indexCurrentGlobalCommandArray].indexGlobalCount++;
                if (asGlobalCommand[indexCurrentGlobalCommandArray].indexGlobalCount >= EXT_MEM_CHUNKED_SIZE_MAX_COUNT )
                {
                    /* Command Not Executed. No available buffer found */
                    //handle->u16HandleStatus |= (1 << 2);
                    nSize = 0;//force break
                }

                asGlobalCommand[indexCurrentGlobalCommandArray].cmdGlobal[index].eExtMemStatus = EXT_MEM_STA_FILL;
            }
            else
            {
                /* Command Not Executed. No available buffer found */
                //handle->u16HandleStatus |= (1 << 0);
                nSize = 0;//break
            }
            
        }
    }
    else
    {
        /* Command Not Executed. No available buffer found */
        //handle->u16HandleStatus |= (1 << 1);
    }
    
    //u16IndexesGlobalCommandArrayProcessReady[indexCurrentGlobalCommandArray] = 1;
    
    return (HMDRV_ADDR)(HMDRV_ADDR_SIZE)pd;
}

/* *****************************************************************************
 * SPIMEM_vExtMemRamFromIntRamCopyData
 * 
 * Description: Copy Data to external memory from already filled internal RAM buffer
 **************************************************************************** */
void SPIMEM_vExtMemRamFromIntRamCopyData(HMDRV_BUFF_PNTR_MODIFIER HMDRV_U16* pu16GlobalQueueID)
{
    uint16_t indexGlobalArrayLoop;
    uint16_t indexProcessReadyLoop;
    
    
    indexProcessReadyLoop = *pu16GlobalQueueID;
    
    //u16IndexesGlobalCommandArrayProcessReady[*pu16GlobalQueueID] = 1;
    
    //for (indexProcessReadyLoop = 0; indexProcessReadyLoop < SPIMEM_MAX_COUNT_SIMULTANEOUS_WRITES_TO_EXTERNAL_MEMORY; indexProcessReadyLoop++)
    {
        //if (u16IndexesGlobalCommandArrayProcessReady[indexProcessReadyLoop] > 0)
        {
            //u16IndexesGlobalCommandArrayProcessReady[indexProcessReadyLoop] = 0;
            
            
            //cmdGlobal[indexGlobal].eExtMemStatus = EXT_MEM_STA_FULL;
            for (indexGlobalArrayLoop = 0; indexGlobalArrayLoop < asGlobalCommand[indexProcessReadyLoop].indexGlobalCount; indexGlobalArrayLoop++)
            {
                indexSetFull = asGlobalCommand[indexProcessReadyLoop].indexGlobalArray[indexGlobalArrayLoop];
                cmdSetFull = (sExtMemCmd_t*)asGlobalCommand[indexProcessReadyLoop].cmdGlobal;
                cmdSetFull += indexSetFull;
                cmdSetFull->eExtMemStatus = EXT_MEM_STA_FULL;
            }
            vSPICommandSendFromQueue(asGlobalCommand[indexProcessReadyLoop].memSelectGlobal);
            
        }

        
    }
}

/* *****************************************************************************
 * SPIMEM_pBufferAddressGet
 **************************************************************************** */
HMDRV_ADDR SPIMEM_pBufferAddressGet(HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize, HMDRV_BUFF_PNTR_MODIFIER HMDRV_U16* pu16GlobalQueueID)
{
    HMDRV_ADDR pResult;

    uint16_t memSelect;
    sExtMemCmdHandle_t* handle;
    sExtMemCmd_t* cmd;
    
    memSelect = eMemoryIndexGet(nDestAddr);
    handle = &sExtMemCmdHandle[memSelect];

    cmd = handle->sExtMemCmd;

    u16IndexFillLast = vExtMemGetFillIndex(handle);
    
    if (u16IndexFillLast < handle->u16CmdQueueCount)
    {  
        /* if EEPROM Memory - WREN Each Write Command */
        if (memSelect == EXT_MEM_SEL_EEPROM)
        {
            //Made in vExtMemGetFillIndex()  cmd[u16IndexFillLast].eExtMemStatus = EXT_MEM_STA_FILL;
            cmd[u16IndexFillLast].ExtMemAddress = nDestAddr;
            cmd[u16IndexFillLast].eExtMemCommand = EXT_MEM_CMD_WREN; 
            cmd[u16IndexFillLast].ExtMemDataCnt = 0;
            //handle->sSPIDMATransferBuffer = handle->sSPIDMATransferBufferBase + u16IndexFillLast;
            cmd[u16IndexFillLast].eExtMemStatus = EXT_MEM_STA_FULL;
            vSPICommandSendFromQueue(memSelect);

            u16IndexFillLast = vExtMemGetFillIndex(handle);
        }
    
        if (u16IndexFillLast < handle->u16CmdQueueCount)
        {  
            handleLast = handle;
            memSelectLast = memSelect;
            cmdLast = cmd;
            //Made in vExtMemGetFillIndex()  cmd[u16IndexFillLast].eExtMemStatus = EXT_MEM_STA_FILL;
            cmd[u16IndexFillLast].ExtMemAddress = nDestAddr;
            //handle->sSPIDMATransferBuffer = handle->sSPIDMATransferBufferBase + u16IndexFillLast;
            //pResult = (HMDRV_ADDR)(HMDRV_ADDR_SIZE_LONG) handle->sSPIDMATransferBuffer->Data;
            pResult = (HMDRV_ADDR)(HMDRV_ADDR_SIZE) (handle->sSPIDMATransferBufferBase[u16IndexFillLast].Data); 
        } 
        else
        {
            pResult = NULL; /* No available buffer found */
            //handle->u16HandleStatus |= (1 << 6);
            
            SPIMEM_u16SnapshotNoAvailableBuffer2++;
        }
    } 
    else
    {
        pResult = NULL; /* No available buffer found */
        //handle->u16HandleStatus |= (1 << 7);
        SPIMEM_u16SnapshotNoAvailableBuffer1++;
    }
    
    
    return pResult;
}

/* *****************************************************************************
 * SPIMEM_vSendBufferToSPI
 **************************************************************************** */
void SPIMEM_vSendBufferToSPI(HMDRV_SIZE8 nSize, HMDRV_BUFF_PNTR_MODIFIER HMDRV_U16* pu16GlobalQueueID)
{
    if ((handleLast != NULL) &&
        (u16IndexFillLast < handleLast->u16CmdQueueCount))
    {  
        cmdLast[u16IndexFillLast].eExtMemCommand = EXT_MEM_CMD_WR; 
        cmdLast[u16IndexFillLast].ExtMemDataCnt = nSize;
        cmdLast[u16IndexFillLast].eExtMemStatus = EXT_MEM_STA_FULL;
        vSPICommandSendFromQueue(memSelectLast);
    }
    else
    {
        SPIMEM_u16SnapshotNoAvailableBufferSendSPI++;
    }
        
    u16IndexFillLast = 0xFFFF;
}

/* *****************************************************************************
 * SPIMEM_vSPICommandQueue
 **************************************************************************** */
void SPIMEM_vSPICommandQueue(eExtMemSelect_t eMemoryIndex, eExtMemCommand_t command, uint32_t address, uint16_t size, sSPIDMATransferBuffer_t* buffer)
{  
    sExtMemCmdHandle_t* sHandle;
    sExtMemCmd_t* sCommand;
    uint16_t index;
    
    sHandle = &sExtMemCmdHandle[eMemoryIndex];
    sCommand = sHandle->sExtMemCmd;

    index = vExtMemGetFillIndex(sHandle);
    
    if (index < sHandle->u16CmdQueueCount)
    {
        //Made in vExtMemGetFillIndex()  sCommand[index].eExtMemStatus = EXT_MEM_STA_FILL;
        sCommand[index].ExtMemAddress = address;
        sCommand[index].ExtMemDataCnt = size;
        sCommand[index].eExtMemCommand = command;
        if (buffer == NULL)     /* buffer not specified - use default buffer */
        {
            if(eMemoryIndex == EXT_MEM_SEL_RAM)
            {
                //sHandle->sSPIDMATransferBuffer = &sExtMemRamTrsfBuf[index];
            }
            else
            {
                //sHandle->sSPIDMATransferBuffer = &sExtMemEepromTrsfBuf[index];
            }
        }
        else
        {
            //sHandle->sSPIDMATransferBuffer = buffer;
        }
        sCommand[index].eExtMemStatus = EXT_MEM_STA_FULL;
        vSPICommandSendFromQueue(eMemoryIndex);
    }
}



#else /* #if SPIMEM_EXT_MEM_USE_SPIMEM_QUEUE_OLD */

#define SPIMEM_QUEUE_LEVEL_COUNT_MAX    8
typedef struct
{
    eExtMemSelect_t eMemoryIndex;
    uint16_t u16Count;   
    uint16_t u16BufferIndex;   
}sGlobalCopyNoQueue_t;      /* was sGlobalCopyNoQueue_t */

sGlobalCopyNoQueue_t sGlobalCopyNoQueue[SPIMEM_QUEUE_LEVEL_COUNT_MAX];    /* was sGlobalCopyNoQueue */

uint16_t u16QueueRsrvLevel = 0;

/* *****************************************************************************
 * u16GetQueueRsrvLevel - get current queue level and increase the level - must be executed with critical
 **************************************************************************** */
uint16_t u16GetQueueRsrvLevel(void)
{
    uint16_t u16CurrentLevel = u16QueueRsrvLevel;
    if (u16QueueRsrvLevel < SPIMEM_QUEUE_LEVEL_COUNT_MAX)
    {
        u16QueueRsrvLevel++;
    }
    else
    {
        /* overflow reached */
    }
    return u16CurrentLevel;  
}

/* *****************************************************************************
 * vReleaseQueueLevel - free current queue level - must be executed with critical
 **************************************************************************** */
void vReleaseQueueLevel(uint16_t u16Level)
{
    if((u16QueueRsrvLevel - 1) == u16Level)
    {
        u16QueueRsrvLevel--;
    }
    else
    {
        /* something went wrong - first should be released the current level */
    }
}


/* *****************************************************************************
 * nQueueBufferRsrv - something like memory allocate
 **************************************************************************** */
uint16_t nQueueBufferRsrv(void)
{
    uint16_t nBufferIndex = SPIMEM_EXT_MEM_QUEUE_COUNT;
    
    ISR_vEnterCritical();
    if (SPIMEM_sQueueBuffer.bFull != true)
    {
        SPIMEM_sQueueBuffer.nTail++;
        
        // check if the end is reached
        if (SPIMEM_sQueueBuffer.nTail == SPIMEM_EXT_MEM_QUEUE_COUNT)      
        {
            // adjust to restart at the beginning
            SPIMEM_sQueueBuffer.nTail = 0;
        }
        // check if full
        if (SPIMEM_sQueueBuffer.nHead == SPIMEM_sQueueBuffer.nTail)
        {
            // it is full, set the full status to true
            SPIMEM_sQueueBuffer.bFull = true;
        }
        nBufferIndex = SPIMEM_sQueueBuffer.nTail;
        SPIMEM_sQueueBuffer.bEmpty = false;
    }
    ISR_vExitCritical();
    
    return nBufferIndex;
}

/* *****************************************************************************
 * vQueueBufferFree - something like memory free
 **************************************************************************** */
void vQueueBufferFree(uint16_t nBufferIndex)
{
    bool bFound;
    bool bFreed;  /* check freed not from midpoint -> queue not full anymore */
    uint16_t nIndex;
    if (nBufferIndex < SPIMEM_EXT_MEM_QUEUE_COUNT)
    {
        bFound = false;
        bFreed = false; 
        ISR_vEnterCritical();
        nIndex = SPIMEM_sQueueBuffer.nTail;
        do      /* Check Free Buffer From Tail to Head */
        {
            if (nBufferIndex == nIndex) 
            {
                bFound = true;
                if (nIndex == SPIMEM_sQueueBuffer.nTail)
                {
                    bFreed = true;
                    SPIMEM_sQueueBuffer.nTail--;
                    if (SPIMEM_sQueueBuffer.nTail >= SPIMEM_EXT_MEM_QUEUE_COUNT)
                    {
                        SPIMEM_sQueueBuffer.nTail = SPIMEM_EXT_MEM_QUEUE_COUNT-1;
                    }
                    /* Release previous mid-points from Tail side */
                    do
                    {
                        nIndex--;
                        if (nIndex >= SPIMEM_EXT_MEM_QUEUE_COUNT)
                        {
                            nIndex = SPIMEM_EXT_MEM_QUEUE_COUNT-1;
                        }
                        if(SPIMEM_sQueueBuffer.bMidRelease[nIndex] == true)
                        {
                            SPIMEM_sQueueBuffer.bMidRelease[nIndex] = false;
                            SPIMEM_sQueueBuffer.nTail--;
                            if (SPIMEM_sQueueBuffer.nTail >= SPIMEM_EXT_MEM_QUEUE_COUNT)
                            {
                                SPIMEM_sQueueBuffer.nTail = SPIMEM_EXT_MEM_QUEUE_COUNT-1;
                            }
                        }
                        else
                        {
                            break;
                        }
                    }while (nIndex != SPIMEM_sQueueBuffer.nHead);

                }
                else if (nIndex == SPIMEM_sQueueBuffer.nHead)
                {
                    bFreed = true;
                    SPIMEM_sQueueBuffer.nHead++;
                    if (SPIMEM_sQueueBuffer.nHead >= SPIMEM_EXT_MEM_QUEUE_COUNT)
                    {
                        SPIMEM_sQueueBuffer.nHead = 0;
                    }
                    /* Release previous mid-points from Head side */
                    while ((SPIMEM_sQueueBuffer.bMidRelease[SPIMEM_sQueueBuffer.nHead]) && (SPIMEM_sQueueBuffer.nHead != SPIMEM_sQueueBuffer.nTail))
                    {
                        SPIMEM_sQueueBuffer.bMidRelease[SPIMEM_sQueueBuffer.nHead] = false;
                        SPIMEM_sQueueBuffer.nHead++;
                        if (SPIMEM_sQueueBuffer.nHead >= SPIMEM_EXT_MEM_QUEUE_COUNT)
                        {
                            SPIMEM_sQueueBuffer.nHead = 0;
                        }
                    }
                }
                else    /* mid-point */
                {
                    SPIMEM_sQueueBuffer.bMidRelease[nIndex] = true;
                }
                break;
            }

            nIndex--;
            if (nIndex >= SPIMEM_EXT_MEM_QUEUE_COUNT)
            {
                nIndex = SPIMEM_EXT_MEM_QUEUE_COUNT-1;
            }
        }while (nIndex != SPIMEM_sQueueBuffer.nHead);
        if (bFreed)
        {
            SPIMEM_sQueueBuffer.bFull = false;
        }
        if (SPIMEM_sQueueBuffer.nHead == SPIMEM_sQueueBuffer.nTail)
        {
            if (bFreed == false)
            {
                SPIMEM_sQueueBuffer.bFull = false; /* something went wrong - should not enter here */
            }        
            SPIMEM_sQueueBuffer.bEmpty = true;
        }
        ISR_vExitCritical();  
    }
}



/* *****************************************************************************
 * SPIMEM_vExtMemRamToIntRam - used mainly from low main loop level
 * 
 * Input pu16GlobalQueueID points to BufferIndex
 * 
 * Description: Copy Data from external memory to internal RAM and 
 *              returns pointer to internal RAM
 **************************************************************************** */
HMDRV_ADDR SPIMEM_pExtMemRamToIntRam(HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSizeReq, HMDRV_ADDR_PNTR_MODIFIER HMDRV_SIZE8* pnSizeRead, HMDRV_BUFF_PNTR_MODIFIER HMDRV_U16* pu16GlobalQueueID)
{
    HMDRV_U8* ps = NULL;
    
    uint16_t u16BufferIndex;
    uint16_t u16Count;
    sSPIDMATransferBuffer_t* buffer;
    eExtMemSelect_t eMemoryIndex;
    volatile SPI_eTransferStatus_t eStatus;
    
    
    u16BufferIndex = nQueueBufferRsrv();
    *pu16GlobalQueueID = u16BufferIndex;
    if (u16BufferIndex < SPIMEM_EXT_MEM_QUEUE_COUNT)
    {
        buffer = (sSPIDMATransferBuffer_t*)u8SPIBuffer[u16BufferIndex];
        
        eMemoryIndex = eMemoryIndexGet(nSrcAddr);
       
        buffer->Command = EXT_MEM_CMD_RD;
        buffer->AddressHiOrStatusData   = (uint8_t)(nSrcAddr >> 16);
        buffer->AddressMi               = (uint8_t)(nSrcAddr >>  8);
        buffer->AddressLo               = (uint8_t)(nSrcAddr >>  0);

        if (nSizeReq > SPIMEM_DATA_BUFFER_SIZE)
        {
            nSizeReq = SPIMEM_DATA_BUFFER_SIZE;
        }
        u16Count = 4 + nSizeReq;
        *pnSizeRead = nSizeReq;
        do
        {
            SPI_ReadWriteBuffer ( (eExtMemSPIModule | (eMemoryIndex << 4)), (uint8_t*)buffer, u16Count, &eStatus, NULL); 
        } while (eStatus == SPI_TRANSF_STA_FAIL); /* never fail and wait for transfer */

        /* wait SPI Command Complete */
        while (eStatus == SPI_TRANSF_STA_PENDING){;}
        if (eStatus != SPI_TRANSF_STA_FAIL) /* if Completed (NotFailed) */
        {
            ps = (HMDRV_U8*)(&(buffer->Data));
        }
        else
        {
            /* *pnSizeRead = 0;    consider needed or not - not zeroed because if fails SPI will loop to call this function forever from the function that call this function */
        }
    }

    return (HMDRV_ADDR)(HMDRV_ADDR_SIZE)ps;
}

/* *****************************************************************************
 * SPIMEM_vReleaseQueueBuffer - used mainly from low main loop level
 * 
 * Input pu16GlobalQueueID points to BufferIndex
 * 
 * Description: 
 **************************************************************************** */
void SPIMEM_vReleaseQueueBuffer(HMDRV_BUFF_PNTR_MODIFIER HMDRV_U16* pu16GlobalQueueID)
{
    vQueueBufferFree(*pu16GlobalQueueID);
}

/* *****************************************************************************
 * SPIMEM_pExtMemRamFromIntRam - used mainly from low main loop level
 * 
 * Description: returns pointer to internal RAM for 
 *              Copy Data to external memory from internal RAM 
 * 
 * Input pu16GlobalQueueID points to QueueLevel
 * 
 * !!! SPIMEM_pExtMemRamFromIntRam -> SPIMEM_vExtMemRamFromIntRamCopyData should not be interrupted by another SPI Write command
 *              
 **************************************************************************** */
HMDRV_ADDR SPIMEM_pExtMemRamFromIntRam(HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize, HMDRV_BUFF_PNTR_MODIFIER HMDRV_U16* pu16GlobalQueueID) 
{
    HMDRV_U8* pd = NULL;
    
    uint16_t nSizeToCopy;
    uint16_t u16QueueLevel;
    uint16_t u16BufferIndex;
    uint16_t u16Count;
    sSPIDMATransferBuffer_t* buffer;
    eExtMemSelect_t eMemoryIndex;
    volatile SPI_eTransferStatus_t eStatus;
    
    ISR_vEnterCritical();
    u16QueueLevel = u16GetQueueRsrvLevel();
    ISR_vExitCritical();
    *pu16GlobalQueueID = u16QueueLevel;
    if (u16QueueLevel < SPIMEM_QUEUE_LEVEL_COUNT_MAX)
    {
        u16BufferIndex = nQueueBufferRsrv();
        sGlobalCopyNoQueue[u16QueueLevel].u16BufferIndex = u16BufferIndex;
        if (u16BufferIndex < SPIMEM_EXT_MEM_QUEUE_COUNT)
        {
            buffer = (sSPIDMATransferBuffer_t*)u8SPIBuffer[u16BufferIndex];

            eMemoryIndex = eMemoryIndexGet(nDestAddr);

            if (eMemoryIndex == EXT_MEM_SEL_EEPROM)
            {
                buffer->Command = EXT_MEM_CMD_WREN;
                u16Count = 1;
                do
                {
                    SPI_ReadWriteBuffer ( (eExtMemSPIModule | (eMemoryIndex << 4)), (uint8_t*)buffer, u16Count, &eStatus, NULL); 
                } while (eStatus == SPI_TRANSF_STA_FAIL); /* never fail and wait for transfer */
                while (eStatus == SPI_TRANSF_STA_PENDING){;}
                if (eStatus == SPI_TRANSF_STA_FAIL) /* if Failed */
                {
                    vQueueBufferFree(u16BufferIndex);
                    sGlobalCopyNoQueue[u16QueueLevel].u16BufferIndex = SPIMEM_EXT_MEM_QUEUE_COUNT; //might be not needed
                    ISR_vEnterCritical();
                    vReleaseQueueLevel(u16QueueLevel);  
                    ISR_vExitCritical();
                    *pu16GlobalQueueID = SPIMEM_QUEUE_LEVEL_COUNT_MAX;
                    return NULL;        /* EXT_MEM_CMD_WREN failed */
                } 
            }

            nSizeToCopy = nSize;
            if (nSizeToCopy > SPIMEM_DATA_BUFFER_SIZE)
            {
                nSizeToCopy = SPIMEM_DATA_BUFFER_SIZE;
            }

            buffer->Command = EXT_MEM_CMD_WR;
            buffer->AddressHiOrStatusData   = (uint8_t)(nDestAddr >> 16);
            buffer->AddressMi               = (uint8_t)(nDestAddr >>  8);
            buffer->AddressLo               = (uint8_t)(nDestAddr >>  0);
            sGlobalCopyNoQueue[u16QueueLevel].u16Count = 4 + nSizeToCopy;
            sGlobalCopyNoQueue[u16QueueLevel].eMemoryIndex = eMemoryIndex;
            pd = buffer->Data;
        }
    }

    return (HMDRV_ADDR)(HMDRV_ADDR_SIZE)pd;
}

/* *****************************************************************************
 * SPIMEM_vExtMemRamFromIntRamCopyData - used mainly from low main loop level
 * 
 * Input pu16GlobalQueueID points to QueueLevel
 * 
 *  * Description: Copy Data to external memory from already filled internal RAM buffer
 **************************************************************************** */
void SPIMEM_vExtMemRamFromIntRamCopyData(HMDRV_BUFF_PNTR_MODIFIER HMDRV_U16* pu16GlobalQueueID)
{
    uint16_t u16QueueLevel;
    uint16_t u16BufferIndex;
    uint16_t u16Count;
    sSPIDMATransferBuffer_t* buffer;
    eExtMemSelect_t eMemoryIndex;
    volatile SPI_eTransferStatus_t eStatus;
    
    u16QueueLevel = *pu16GlobalQueueID;
    if (u16QueueLevel < SPIMEM_QUEUE_LEVEL_COUNT_MAX)
    {
        u16BufferIndex = sGlobalCopyNoQueue[u16QueueLevel].u16BufferIndex;
        if (u16BufferIndex < SPIMEM_EXT_MEM_QUEUE_COUNT)
        {
            buffer = (sSPIDMATransferBuffer_t*)u8SPIBuffer[u16BufferIndex];
            eMemoryIndex = sGlobalCopyNoQueue[u16QueueLevel].eMemoryIndex;
            
            //u16Count = nSize;
            u16Count = sGlobalCopyNoQueue[u16QueueLevel].u16Count;
            
            ISR_vEnterCritical();
            vReleaseQueueLevel(u16QueueLevel);  
            ISR_vExitCritical();
            
            do
            {
                SPI_ReadWriteBuffer ( (eExtMemSPIModule | (eMemoryIndex << 4)), (uint8_t*)buffer, u16Count, &eStatus, NULL); 
            } while (eStatus == SPI_TRANSF_STA_FAIL); /* never fail and wait for transfer */
            vQueueBufferFree(u16BufferIndex);
        }
    }
}


/* *****************************************************************************
 * SPIMEM_pBufferAddressGet - used mainly for recorders -> similar to SPIMEM_pExtMemRamFromIntRam
 * 
 * Input pu16GlobalQueueID points to QueueLevel
 * 
 **************************************************************************** */
HMDRV_ADDR SPIMEM_pBufferAddressGet(HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize, HMDRV_BUFF_PNTR_MODIFIER HMDRV_U16* pu16GlobalQueueID)
{
    HMDRV_U8* pd = NULL;
    
    HMDRV_SIZE8 nSizeToCopy;
    uint16_t u16QueueLevel;
    uint16_t u16BufferIndex;
    uint16_t u16Count;
    sSPIDMATransferBuffer_t* buffer;
    eExtMemSelect_t eMemoryIndex;
    volatile SPI_eTransferStatus_t eStatus;
    
    ISR_vEnterCritical();
    u16QueueLevel = u16GetQueueRsrvLevel();
    ISR_vExitCritical();
    *pu16GlobalQueueID = u16QueueLevel;
    if (u16QueueLevel < SPIMEM_QUEUE_LEVEL_COUNT_MAX)
    {
        u16BufferIndex = nQueueBufferRsrv();
        sGlobalCopyNoQueue[u16QueueLevel].u16BufferIndex = u16BufferIndex;
        if (u16BufferIndex < SPIMEM_EXT_MEM_QUEUE_COUNT)
        {
            buffer = (sSPIDMATransferBuffer_t*)u8SPIBuffer[u16BufferIndex];

            eMemoryIndex = eMemoryIndexGet(nDestAddr);

            if (eMemoryIndex == EXT_MEM_SEL_EEPROM)
            {
                buffer->Command = EXT_MEM_CMD_WREN;
                u16Count = 1;
                do
                {
                    SPI_ReadWriteBuffer ( (eExtMemSPIModule | (eMemoryIndex << 4)), (uint8_t*)buffer, u16Count, &eStatus, NULL); 
                } while (eStatus == SPI_TRANSF_STA_FAIL); /* never fail and wait for transfer */
                while (eStatus == SPI_TRANSF_STA_PENDING){;}
                if (eStatus == SPI_TRANSF_STA_FAIL) /* if Failed */
                {
                    vQueueBufferFree(u16BufferIndex);
                    sGlobalCopyNoQueue[u16QueueLevel].u16BufferIndex = SPIMEM_EXT_MEM_QUEUE_COUNT; //might be not needed
                    ISR_vEnterCritical();
                    vReleaseQueueLevel(u16QueueLevel);  
                    ISR_vExitCritical();
                    *pu16GlobalQueueID = SPIMEM_QUEUE_LEVEL_COUNT_MAX;
                    return NULL;        /* EXT_MEM_CMD_WREN failed */
                } 
            }

            nSizeToCopy = nSize;
            if (nSizeToCopy > SPIMEM_DATA_BUFFER_SIZE)
            {
                nSizeToCopy = SPIMEM_DATA_BUFFER_SIZE;
            }

            buffer->Command = EXT_MEM_CMD_WR;
            buffer->AddressHiOrStatusData   = (uint8_t)(nDestAddr >> 16);
            buffer->AddressMi               = (uint8_t)(nDestAddr >>  8);
            buffer->AddressLo               = (uint8_t)(nDestAddr >>  0);
            sGlobalCopyNoQueue[u16QueueLevel].u16Count = 4 + nSizeToCopy;
            sGlobalCopyNoQueue[u16QueueLevel].eMemoryIndex = eMemoryIndex;
            pd = buffer->Data;
        }
    }

    return (HMDRV_ADDR)(HMDRV_ADDR_SIZE)pd;
}

/* *****************************************************************************
 * SPIMEM_vSendBufferToSPI - used mainly for recorders -> similar to SPIMEM_vExtMemRamFromIntRamCopyData
 * Input pu16GlobalQueueID points to QueueLevel
 **************************************************************************** */
void SPIMEM_vSendBufferToSPI(HMDRV_SIZE8 nSize, HMDRV_BUFF_PNTR_MODIFIER HMDRV_U16* pu16GlobalQueueID)
{
    uint16_t u16QueueLevel;
    uint16_t u16BufferIndex;
    uint16_t u16Count;
    sSPIDMATransferBuffer_t* buffer;
    eExtMemSelect_t eMemoryIndex;
    
    u16QueueLevel = *pu16GlobalQueueID;
    if (u16QueueLevel < SPIMEM_QUEUE_LEVEL_COUNT_MAX)
    {
        u16BufferIndex = sGlobalCopyNoQueue[u16QueueLevel].u16BufferIndex;
        if (u16BufferIndex < SPIMEM_EXT_MEM_QUEUE_COUNT)
        {
            buffer = (sSPIDMATransferBuffer_t*)u8SPIBuffer[u16BufferIndex];
            eMemoryIndex = sGlobalCopyNoQueue[u16QueueLevel].eMemoryIndex;
            
            //u16Count = nSize;
            u16Count = sGlobalCopyNoQueue[u16QueueLevel].u16Count;
            
            ISR_vEnterCritical();
            vReleaseQueueLevel(u16QueueLevel);  
            ISR_vExitCritical();
            
#if ISR_PRIORITY_SPITRANS_BIGGER_THAN_CONTROL == 0
            volatile SPI_eTransferStatus_t eStatus;
            SPI_ReadWriteBuffer ( (eExtMemSPIModule | (eMemoryIndex << 4)), (uint8_t*)buffer, u16Count, &eStatus, NULL); 
            if (eStatus == SPI_TRANSF_STA_FAIL)
            {
                SPIMEM_u16CorruptedRecorderData++;
            }
#else
            SPI_WriteBuffer( (eExtMemSPIModule | (eMemoryIndex << 4)), (uint8_t*)buffer, u16Count);
#endif
            vQueueBufferFree(u16BufferIndex);
        }
    }
}

/* *****************************************************************************
 * SPIMEM_vSPICommandQueue
 **************************************************************************** */
void SPIMEM_vSPICommandQueue(eExtMemSelect_t eMemoryIndex, eExtMemCommand_t command, uint32_t address, uint16_t size, sSPIDMATransferBuffer_t* buffer)
{  
    uint16_t u16BufferIndex;
    uint16_t u16Count;

    volatile SPI_eTransferStatus_t eStatus;
    
    if (buffer == NULL)
    {
        u16BufferIndex = nQueueBufferRsrv();
        buffer = (sSPIDMATransferBuffer_t*)u8SPIBuffer[u16BufferIndex];
    }
    else
    {
        u16BufferIndex = SPIMEM_EXT_MEM_QUEUE_COUNT;
    }

    if (command == EXT_MEM_CMD_WR)
    {
        if (eMemoryIndex == EXT_MEM_SEL_EEPROM)
        {
            buffer->Command = EXT_MEM_CMD_WREN;
            u16Count = 1;
            do
            {
                SPI_ReadWriteBuffer ( (eExtMemSPIModule | (eMemoryIndex << 4)), (uint8_t*)buffer, u16Count, &eStatus, NULL); 
            } while (eStatus == SPI_TRANSF_STA_FAIL); /* never fail and wait for transfer */
            while (eStatus == SPI_TRANSF_STA_PENDING){;}
            if (eStatus == SPI_TRANSF_STA_FAIL) /* if Failed */
            {
                /* !!! consider return error code !!! */
                vQueueBufferFree(u16BufferIndex);
                return;        /* EXT_MEM_CMD_WREN failed */
            } 
        }
    }
       
    buffer->Command = command;
    buffer->AddressHiOrStatusData   = (uint8_t)(address >> 16);
    buffer->AddressMi               = (uint8_t)(address >>  8);
    buffer->AddressLo               = (uint8_t)(address >>  0);

    do 
    {
        u16Count = 1;   /* Command Bytes Count */

        if (command == EXT_MEM_CMD_WR)
        {
            u16Count += 3;   /* Address Bytes Count */    

        }
        else if (command == EXT_MEM_CMD_RD)
        {
            u16Count += 3;   /* Address Bytes Count */                    
        }
        else    /* All other commands */
        {
        }

        u16Count += size;  /* Data Bytes Count */

        do
        {
            SPI_ReadWriteBuffer ( (eExtMemSPIModule | (eMemoryIndex << 4)), (uint8_t*)buffer, u16Count, &eStatus, NULL); 
        } while (eStatus == SPI_TRANSF_STA_FAIL); /* never fail and wait for transfer */

        /* wait SPI Command Complete */
        while (eStatus == SPI_TRANSF_STA_PENDING){;} 
    } while(size > 0);
    vQueueBufferFree(u16BufferIndex);
    return;
}
#endif  /* #if SPIMEM_EXT_MEM_USE_SPIMEM_QUEUE_OLD */


/* *****************************************************************************
 * Test Related
 **************************************************************************** */


#if SPIMEM_EXT_MEM_USE_SPIMEM_QUEUE_OLD                              
/* *****************************************************************************
 * SPIMEM_vTestSPICommandInit
 **************************************************************************** */
void SPIMEM_vTestSPICommandInit(sExtMemCmd_t* handle, eExtMemCommand_t command, uint32_t address, uint16_t size)
{  
    handle->eExtMemStatus = EXT_MEM_STA_IDLE;
    handle->ExtMemAddress = address;
    handle->ExtMemDataCnt = size;
    handle->eExtMemCommand = command;
}

/* *****************************************************************************
 * SPIMEM_vSPITestCommandQueue
 **************************************************************************** */
void SPIMEM_vSPITestCommandQueue(void)
{    
    if (sTestExtMemCmd.eExtMemStatus == EXT_MEM_STA_FULL)
    {
        SPIMEM_vSPICommandQueue(
                eTestExtMemSelect, 
                sTestExtMemCmd.eExtMemCommand, 
                sTestExtMemCmd.ExtMemAddress, 
                sTestExtMemCmd.ExtMemDataCnt,
                &sTestExtMemTrsfBuf);
        sTestExtMemCmd.eExtMemStatus = EXT_MEM_STA_IDLE; 
    }
}
#endif




#if USE_SPI_FLIR
/* *****************************************************************************
 * FLIR Related
 **************************************************************************** */

/* *****************************************************************************
 * vInitExtMemFlir
 **************************************************************************** */
void vInitExtMemFlir(void)
{
    uint8_t index;
    
    sExtMemCmdHandle[EXT_MEM_SEL_FLIR].u16CmdQueueCount = EXT_MEM_FLIR_CMD_QUEUE_COUNT;
    sExtMemCmdHandle[EXT_MEM_SEL_FLIR].u16IndexFill = 0;
    sExtMemCmdHandle[EXT_MEM_SEL_FLIR].u16IndexSend = 0;
    sExtMemCmdHandle[EXT_MEM_SEL_FLIR].eExtMemSelectID = EXT_MEM_SEL_FLIR;
    
    sExtMemCmdHandle[EXT_MEM_SEL_FLIR].sExtMemCmd = &sExtMemFlirCmd[0];
    sExtMemCmdHandle[EXT_MEM_SEL_FLIR].sSPIDMATransferBuffer = &sExtMemFlirTrsfBuf[0];
    sExtMemCmdHandle[EXT_MEM_SEL_FLIR].sSPIDMATransferBufferBase = &sExtMemFlirTrsfBuf[0];
    
    for (index = 0; index < (uint8_t)(sizeof(sExtMemFlirCmd)/sizeof(sExtMemFlirCmd[0])); index++)
    {
        sExtMemFlirCmd[index].eExtMemStatus = EXT_MEM_STA_IDLE;
    }
}

/* *****************************************************************************
 * SPIMEM_vSetStateVoSPIReq
 * 
 * Description: VoSPI State Change Request
 **************************************************************************** */
void SPIMEM_vSetStateVoSPIReq(eVoSPIState_t eState)
{
    eVoSPIStateReq = eState;
}
  
/* *****************************************************************************
 * SPIMEM_vSetFlirImageDataBufferZIP
 * 
 * Description:
 **************************************************************************** */
void SPIMEM_vSetFlirImageDataBufferZIP(uint32_t u32DataAddress)
{
    pFlirImageZIPWrite =(HMDRV_U8*) u32DataAddress;
}
  
/* *****************************************************************************
 * SPIMEM_vSetFlirImageDataBufferZIPPing
 * 
 * Description:
 **************************************************************************** */
void SPIMEM_vSetFlirImageDataBufferZIPPing(uint32_t u32DataAddress)
{
    pFlirImageZIPWritePing =(HMDRV_U8*) u32DataAddress;
}

/* *****************************************************************************
 * SPIMEM_vSetFlirImageDataBufferZIPPong
 * 
 * Description:
 **************************************************************************** */
void SPIMEM_vSetFlirImageDataBufferZIPPong(uint32_t u32DataAddress)
{
    pFlirImageZIPWritePong =(HMDRV_U8*) u32DataAddress;
}
  
/* *****************************************************************************
 * SPIMEM_vSetFlirImageDataBuffer
 * 
 * Description:
 **************************************************************************** */
void SPIMEM_vSetFlirImageDataBuffer(uint32_t u32DataAddress)
{
    pFlirImage =(HMDRV_U8*) u32DataAddress;
    pFlirImageRead =(HMDRV_U8*) u32DataAddress;
}
  
/* *****************************************************************************
 * SPIMEM_vSetFlirImageDataBufferPing
 * 
 * Description:
 **************************************************************************** */
void SPIMEM_vSetFlirImageDataBufferPing(uint32_t u32DataAddress)
{
    pFlirImagePing =(HMDRV_U8*) u32DataAddress;
}
  

/* *****************************************************************************
 * SPIMEM_vSetFlirImageDataBufferPong
 * 
 * Description:
 **************************************************************************** */
void SPIMEM_vSetFlirImageDataBufferPong(uint32_t u32DataAddress)
{
    pFlirImagePong =(HMDRV_U8*) u32DataAddress;
}

/* *****************************************************************************
 * SPIMEM_vSwitchFlirImagePingPong
 * 
 * Description: 
 **************************************************************************** */
void SPIMEM_vSwitchFlirImagePingPong(void)
{
    if (FlirImageSwitchPingPong == 1)
    {
        FlirImageSwitchPingPong = 0;
                
        
        if (bFlirImageInMainLoopZIP == HMDRV_FALSE) /* consider need or not - better not used */
        {  
            /* no ZIP or ZIP on the Fly requested */
            
            
            /* code below was and consider to be pFlirImageRead = pFlirImage; */
            if (bFlirImageWritingPongLast == HMDRV_FALSE)
            {
                /* use ping buffer for modbus and release pong buffer */
                bFlirImageStartUsePing = HMDRV_TRUE;
                bFlirImageStartUsePong = HMDRV_FALSE;
                pFlirImageRead = pFlirImagePing;
            }
            else
            {
                /* use pong buffer for modbus and release ping buffer */
                bFlirImageStartUsePong = HMDRV_TRUE;
                bFlirImageStartUsePing = HMDRV_FALSE;
                pFlirImageRead = pFlirImagePong;
            }
        }
        else
        {
            /* ZIP in separate independent process */
        }
        
        
    }
}

uint32_t SPIMEM_vGetFlirImageDataBufferZIP(void)
{
    return (HMDRV_U32)pFlirImageZIPWrite;
}

uint32_t SPIMEM_vGetFlirImageDataBuffer(void)
{
    return (HMDRV_U32)pFlirImageRead;
}
void SPIMEM_vSetVoSPIPacketModbusLengthPingPong(void)
{
    if (eVoSPIPacketZIP == VOSPI_ZIP_PYRAMID)
    {   
        VoSPIPacketModbusLengthPingPong = (IR_CAMERA_DOTS_PER_LINE_COUNT * IR_CAMERA_BYTES_PER_DOT_RAW14) + ((PYRAMID_BLANK_PIXELS_LEFT + PYRAMID_BLANK_PIXELS_RIGHT) * PYRAMID_BLANK_PIXEL_BYTES_SIZE);
    }
    else
    {
        if (eVoSPIPacketZIP == VOSPI_ZIP_BYTE) 
        {
            VoSPIPacketModbusLengthPingPong = IR_CAMERA_DOTS_PER_LINE_COUNT + VoSPIHeaderZIPLength;
        }
        else
        {
            if (eVoSPIPacketZIP == VOSPI_ZIP_BYTE_PREV_PIXEL_DIFF)
            {
                VoSPIPacketModbusLengthPingPong = IR_CAMERA_DOTS_PER_LINE_COUNT + VoSPIHeaderZIPLength;
            }
            else
            {
                VoSPIPacketModbusLengthPingPong = IR_CAMERA_DOTS_PER_LINE_COUNT;
                //VoSPIPacketModbusLengthPingPong = VoSPIPacketLength;
            }        
        }
    }
}

/* *****************************************************************************
 * SPIMEM_vSetFlirImageSPIPointer
 * 
 * Description: 
 **************************************************************************** */
void SPIMEM_vSetFlirImageSPIPointer(void)
{
}

HMDRV_BOOL FlirCRCCalc (sSPIDMAFlirTransferBuffer_t* pPacket, uint16_t nSize)
{
    return HMDRV_TRUE;
}

/* *****************************************************************************
 * SPIMEM_vProcessVoSPI
 * 
 * Description: VoSPI Process
 **************************************************************************** */
void SPIMEM_vProcessVoSPIReceivedPacket(uint16_t index)
{
    eExtMemSelect_t memSelect;
    sExtMemCmdHandle_t* handle;
    sFlirCmd_t* cmd;
    uint16_t indexRead;
    //uint8_t nTemp;
    uint16_t nSize;
    uint16_t nSizeZIP;
    uint16_t nSizeBackup;
    //uint32_t address;
    uint16_t nRow;
    uint16_t nCRC;
    sSPIDMAFlirTransferBuffer_t* pPacket;
    HMDRV_U8* ps;
    HMDRV_FILE_PNTR_MODIFIER HMDRV_U8* psZIP;    
    HMDRV_FILE_PNTR_MODIFIER HMDRV_U8* pd;    
    HMDRV_FILE_PNTR_MODIFIER HMDRV_S16* pds16;    
    HMDRV_FILE_PNTR_MODIFIER HMDRV_U16* pd16;    
    //HMDRV_FILE_PNTR_MODIFIER sFlirRow_t* pDestRow;
    HMDRV_U16 dataMin,dataMax,dataNow,dataDif,dataMid;

    
    memSelect = EXT_MEM_SEL_FLIR;
    
    handle = &sExtMemCmdHandle[memSelect];
    cmd = (sFlirCmd_t*)handle->sExtMemCmd;
    indexRead = index;

    if (indexRead < handle->u16CmdQueueCount)
    {  
        
        
        nSize = cmd[indexRead].ExtMemDataCnt - 4;
        pPacket = (sSPIDMAFlirTransferBuffer_t*)handle->sSPIDMATransferBuffer;
        nRow = ((uint16_t)pPacket->IDHi << 8) + pPacket->IDLo;      
        nRow &= 0x0FFF;
        nCRC = ((uint16_t)pPacket->CRCHi << 8) + pPacket->CRCLo;      
        
        if (nCRC != 0xFFFF)
        {
            u16DebugData3 = nRow;
            u16DebugData4 = nCRC;
        }
        
        u16DebugData1 = nRow;
        u16DebugData2 = nCRC;
        
        if (FlirCRCCalc(pPacket, nSize))
        {
            ps = (HMDRV_U8*)(&(pPacket->Data));

            if (nRow < IR_CAMERA_LINES_COUNT)
            {     
                if (nRow == 0)
                {
                    SPIMEM_vSetFlirImageSPIPointer();       /* currently empty - active data fill from SPI pointer selected in the code below */
                    
                   
                    
                    
                    /* get current fill request image file */
                    if (FlirImageTryFillNextPong == HMDRV_FALSE)
                    {
                        
                        /* release ping buffer if not used */
                        if (bFlirImageStartUsePing == HMDRV_FALSE)
                        {
                            bFlirImageDenyWritePing = HMDRV_FALSE;
                        }
                        
                        
                        
                        /* next ping buffer try start fill */
                        if (bFlirImageDenyWritePing == HMDRV_FALSE)
                        {
                            bFlirImageBufferFillPing = HMDRV_TRUE;
                            pFlirImage = pFlirImagePing;
                            bFlirImageWritingPongLast = HMDRV_FALSE;
                            
                            pFlirImageZIPWrite = pFlirImageZIPWritePing;
                            FlirImageZIPWriteOffset = 0;
                            
                        }
                        
                        bFlirImageDenyWrite = bFlirImageDenyWritePing;
                    }
                    else
                    {
                        /* release pong buffer if not used */
                        if (bFlirImageStartUsePong == HMDRV_FALSE)
                        {
                            bFlirImageDenyWritePong = HMDRV_FALSE;
                        }
                        
                        
                        
                        /* next pong buffer try start fill */
                        if (bFlirImageDenyWritePong == HMDRV_FALSE)
                        {
                            bFlirImageBufferFillPong = HMDRV_TRUE;
                            pFlirImage = pFlirImagePong;
                            bFlirImageWritingPongLast = HMDRV_TRUE;
                            
                            pFlirImageZIPWrite = pFlirImageZIPWritePong;
                            FlirImageZIPWriteOffset = 0;
                        }

                        bFlirImageDenyWrite = bFlirImageDenyWritePong;
                    }
                    
                    if (bFlirImageDenyWrite == HMDRV_FALSE)
                    {
                        bFlirImageStartWrite = HMDRV_TRUE;
                    }
                    else
                    {
                        bFlirImageStartWrite = HMDRV_FALSE;
                    }
                    
#if 0
                    if (bFlirImageZIPDenyWrite == HMDRV_FALSE)
                    {

                        /* switch ZIP Buffers */
                        if (pFlirImageZIPWrite == pFlirImageZIPWritePong)
                        {
                            
                            pFlirImageZIPWrite = pFlirImageZIPWritePing;
                        }
                        else
                        {
                            pFlirImageZIPWrite = pFlirImageZIPWritePong;
                        }
                        
                        FlirImageZIPWriteOffset = 0;
                        
                        bFlirImageZIPStartWrite = HMDRV_TRUE;
                        
                        bFlirImageDenyWrite = HMDRV_TRUE;
                    }
#endif
                }
                
                
                if (bFlirImageStartWrite == HMDRV_TRUE)     /* enabled before read zero row and disabled after read last row */
                {


                    pd = pFlirImage;


                    if (pFlirImage != NULL)
                    {

                        switch (eVoSPIPacketZIP)
                        {
                            /* One Byte compression with possible Losses at highest and lowest temperature */
                            case VOSPI_ZIP_BYTE_PREV_PIXEL_DIFF:

                                pd += ((VoSPIPacketModbusLengthPingPong)*nRow);                                                                              

                                pd16 = pd;
                                nSizeBackup = nSize;
                                while(nSize > 0)
                                {
                                    #if 1
                                    *(pd+1) = *ps++;
                                    *(pd+0) = *ps++;
                                    nSize -= 2;
                                    pd++;
                                    pd++;
                                    #else
                                    *pd++ = *ps++;
                                    nSize ---;
                                    #endif
                                }

                                nSize = nSizeBackup;
                                pd = pd16;





                                dataMid = *pd16;
                                dataDif = dataMid;
                                /* compress to 8-bits */
                                while(nSize > 0)
                                {

                                    *pd++ = (HMDRV_U8)(HMDRV_S8)((HMDRV_S16)(*pd16) - (HMDRV_S16)dataMid);
                                    dataMid = *pd16++;
                                    nSize -= 2;
                                }
                                pd16 = pd;
                                *pd16 = dataDif;        /* write Base at end */

                                break;

                            /* One Byte compression with possible Losses at highest and lowest temperature */
                            case VOSPI_ZIP_BYTE:

                                pd += ((VoSPIPacketModbusLengthPingPong)*nRow);                                                                              

                                pd16 = pd;
                                dataMin = 0xFFFF;
                                dataMax = 0;
                                nSizeBackup = nSize;

                                while(nSize > 0)
                                {
                                    #if 1
                                    *(pd+1) = *ps++;
                                    *(pd+0) = *ps++;
                                    nSize -= 2;

                                    pd++;
                                    pd++;
                                    dataNow = *pd16++;
                                    if (dataNow < dataMin)
                                    {
                                        dataMin = dataNow;
                                    }
                                    if (dataNow > dataMax)
                                    {
                                        dataMax = dataNow;
                                    }

                                    #else
                                    *pd++ = *ps++;
                                    nSize ---;
                                    #endif
                                }

                                //??? if data difference > 256 overflow
                                dataDif = dataMax-dataMin;
                                dataDif >>= 1;
                                dataMid = dataMax-dataDif;  /* this is the base */


                                nSize = nSizeBackup;
                                pd -= VoSPIPacketLength;
                                pd16 = pd;

                                /* fix first pixel to fit the byte range */

    #if FLIR_FIRST_PIXEL_ALWAYS_IN_RANGE
                                dataDif = (HMDRV_S16)*pd16 - (HMDRV_S16)dataMid;
                                if ((HMDRV_S16)dataDif > 127)
                                {
                                    dataMid = *pd16 - 127;
                                }
                                if ((HMDRV_S16)dataDif < -128)
                                {
                                    dataMid = *pd16 + 128;
                                }
    #endif

                                /* compress to 8-bits */
                                while(nSize > 0)
                                {
                                    *pd++ = (HMDRV_U8)(HMDRV_S8)((HMDRV_S16)(*pd16++) - (HMDRV_S16)dataMid);
                                    nSize -= 2;
                                }
                                pd16 = pd;
                                *pd16 = dataMid;        /* write Base at end */

                                break;

                            case VOSPI_ZIP_PYRAMID:
                                /* blank frame needed */
                                pd += PYRAMID_BLANK_PIXELS_LEFT*PYRAMID_BLANK_PIXEL_BYTES_SIZE + ((VoSPIPacketLength + ((PYRAMID_BLANK_PIXELS_LEFT + PYRAMID_BLANK_PIXELS_RIGHT) * PYRAMID_BLANK_PIXEL_BYTES_SIZE))*(nRow + PYRAMID_BLANK_PIXELS_TOP));

                                while(nSize > 0)
                                {
                                    #if 1
                                    *(pd+1) = *ps++;
                                    *(pd+0) = *ps++;
                                    nSize -= 2;
                                    pd++;
                                    pd++;
                                    #else
                                    *pd++ = *ps++;
                                    nSize ---;
                                    #endif
                                }
                                break;

                            default:

                                pd += ((VoSPIPacketLength)*nRow);



                                while(nSize > 0)
                                {
                                    #if 1
                                    *(pd+1) = *ps++;
                                    *(pd+0) = *ps++;
                                    nSize -= 2;
                                    pd++;
                                    pd++;
                                    #else
                                    *pd++ = *ps++;
                                    nSize ---;
                                    #endif
                                }



                                break;

                        }

                        if (eVoSPIPacketZIP == VOSPI_ZIP_VBIT)
                        {
                            psZIP = pFlirImage;
                            psZIP += ((VoSPIPacketLength)*nRow);

                            pd  = pFlirImageZIPWrite;
                            pd += FlirImageZIPWriteOffset;
                            
                            if (4096 > FlirImageZIPWriteOffset)
                            {
                                nSizeZIP = 4096 - FlirImageZIPWriteOffset;
                            }
                            FlirImageZIPWriteOffset += FLIRZIP_vRowCompress(psZIP, pd, nSizeZIP);
                        }


                        if (nRow == (IR_CAMERA_LINES_COUNT-1))
                        {
                            
                            if (bFlirImageStartWrite == HMDRV_TRUE)
                            {
                                bFlirImageStartWrite = HMDRV_FALSE;
                                
                                /* deny fill filled image till processed */
                                if (bFlirImageWritingPongLast == HMDRV_FALSE)
                                {
                                    /* just filled ping buffer */
                                    bFlirImageDenyWritePing = HMDRV_TRUE;
                                    bFlirImageBufferFullPing = HMDRV_TRUE;
                                    bFlirImageBufferFillPing = HMDRV_FALSE;
                                                                   
                                    FlirImageTryFillNextPong = HMDRV_TRUE; /* next try fill pong buffer */
                                } 
                                else
                                {
                                    /* just filled pong buffer */
                                    bFlirImageDenyWritePong = HMDRV_TRUE;
                                    bFlirImageBufferFullPong = HMDRV_TRUE;
                                    bFlirImageBufferFillPong = HMDRV_FALSE;

                                    FlirImageTryFillNextPong = HMDRV_FALSE; /* next try fill ping buffer */
                                }
                            }
                        }

                    }
                    else
                    {
                        //handle->u16HandleStatus |= (1 << 15);
                    }

                }
            }
            else
            {
                //handle->u16HandleStatus |= (1 << 6);
            }
        }
        else
        {
            //handle->u16HandleStatus |= (1 << 1);
        }
        
    }
    else
    {
        /* Command Not Executed. error index */
        //handle->u16HandleStatus |= (1 << 4);
    } 
}
 
/* *****************************************************************************
 * SPIMEM_vProcessVoSPI
 * 
 * Description: VoSPI Process
 **************************************************************************** */
void SPIMEM_vProcessVoSPI(void)
{
    switch (eVoSPIState)
    {
        case VOSPI_IDLE:
            if (eVoSPIStateReq == VOSPI_START)
            {
                eVoSPIState = VOSPI_RUN;
                SPIMEM_vGetFlirVoSPIPacket(VoSPIPacketLength + 4);    /* Raw14 Mode - consider use variable here */
            }
            if (eVoSPIStateReq == VOSPI_SINGLE)
            {
                eVoSPIState = VOSPI_RUN;
                SPIMEM_vGetFlirVoSPIPacket(VoSPIPacketLength + 4);    /* Raw14 Mode - consider use variable here */
            }
            break;
        case VOSPI_START:
            break;
        case VOSPI_SYNC:
            break;
        case VOSPI_RUN:
            if (eVoSPIStateReq == VOSPI_STOP)
            {
                eVoSPIState = VOSPI_IDLE;
            }
            break;
        case VOSPI_STOP:
            if (eVoSPIStateReq == VOSPI_STOP)
            {
                eVoSPIState = VOSPI_IDLE;
            }            
            break;
        default:
            break;
    }
}

/* *****************************************************************************
 * SPIMEM_vGetFlirVoSPIPacket
 * 
 * Description: Copy Data from external memory to internal RAM buffer
 **************************************************************************** */
void SPIMEM_vGetFlirVoSPIPacket(HMDRV_SIZE8 nSize)
{
    eExtMemSelect_t memSelect;
    sExtMemCmdHandle_t* handle;
    sSPIDMAFlirTransferBuffer_t* bufferBase;
    sFlirCmd_t* cmd;
    uint16_t indexRead;
    uint16_t index;
    HMDRV_U8* ps;
    //HMDRV_U8* pd;
    
    memSelect = EXT_MEM_SEL_FLIR;
    
    handle = &sExtMemCmdHandle[memSelect];
    cmd = (sFlirCmd_t*)handle->sExtMemCmd;
    indexRead = vExtMemGetFillIndex(handle);

    if (indexRead < handle->u16CmdQueueCount)
    {  
        //Made in vExtMemGetFillIndex()  cmd[indexRead].eExtMemStatus = EXT_MEM_STA_FILL;
        cmd[indexRead].ExtMemDataCnt = nSize;
        bufferBase = (sSPIDMAFlirTransferBuffer_t*)handle->sSPIDMATransferBufferBase;
        handle->sSPIDMATransferBuffer = (sSPIDMATransferBuffer_t*) (bufferBase + indexRead);
        
        ps = (HMDRV_U8*)handle->sSPIDMATransferBuffer;
        for (index = 0; index < SPI_FLIR_DATA_BUFFER_SIZE + 4; index++)
        {
            *ps++ = 0;
        }
        
        cmd[indexRead].eExtMemStatus = EXT_MEM_STA_FULL;
        vSPICommandSendFromQueue(memSelect);
        
#if 0
        ????wait is bad
        /* wait SPI Command Complete */
        while (((cmd+indexRead)->eExtMemStatus) != EXT_MEM_STA_IDLE)
        {
            //ClrWdt();
        };
        
        ps = (HMDRV_U8*) ((sSPIDMAFlirTransferBuffer_t*)handle->sSPIDMATransferBuffer)->Data;
        pd = (HMDRV_U8*) pDestAddr;
        
        while(nSize--)
            *pd++ = *ps++;
#endif  
        
    }
    else
    {
        /* Command Not Executed. No available buffer found */
        //handle->u16HandleStatus |= (1 << 3);
    }
    
}

/* *****************************************************************************
 * SPIMEM_vCopyFromExtMemRam
 * 
 * Description: HMLIB Compatible copy function:
 * HMDRV_BPTR HMDRV_CopyToBuffer(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
 **************************************************************************** */
void SPIMEM_vCopyFromExtMemRam(HMDRV_BPTR pDestAddr, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_U8* ps;
    HMDRV_U8* pd;
    uint16_t indexRead;
    uint16_t memSelect;
    sExtMemCmdHandle_t* handle;
    sExtMemCmd_t* cmd;
    //eExtMemStatus_t status;
    
    memSelect = eMemoryIndexGet(nSrcAddr);
    handle = &sExtMemCmdHandle[memSelect];

    cmd = handle->sExtMemCmd;
    
    indexRead = vExtMemGetFillIndex(handle);

    
    
    if (indexRead < handle->u16CmdQueueCount)
    {  
        //Made in vExtMemGetFillIndex()  cmd[indexRead].eExtMemStatus = EXT_MEM_STA_FILL;
        cmd[indexRead].ExtMemAddress = nSrcAddr;
        cmd[indexRead].eExtMemCommand = EXT_MEM_CMD_RD; 
        cmd[indexRead].ExtMemDataCnt = nSize;
        handle->sSPIDMATransferBuffer = handle->sSPIDMATransferBufferBase + indexRead;
        cmd[indexRead].eExtMemStatus = EXT_MEM_STA_FULL;
        vSPICommandSendFromQueue(memSelect);
        
        /* wait SPI Command Complete */
#if 0
        if (memSelect == EXT_MEM_SEL_RAM)
        {
            while (sExtMemRamCmd[indexRead].eExtMemStatus != EXT_MEM_STA_IDLE)
            {
                //ClrWdt();
            }
        }
        else         
        {
            while (sExtMemEepromCmd[indexRead].eExtMemStatus != EXT_MEM_STA_IDLE)
            {
                //ClrWdt();
            }
        }
#else
        //??? not working
        while (((cmd+indexRead)->eExtMemStatus) != EXT_MEM_STA_IDLE)
        {
            //ClrWdt();
        };


#endif
        
        ps = (HMDRV_U8*) handle->sSPIDMATransferBuffer->Data;
        pd = (HMDRV_U8*) pDestAddr;
        
        while(nSize--)
            *pd++ = *ps++;
        
    }
    else
    {
        /* Command Not Executed. No available buffer found */
        //handle->u16HandleStatus |= (1 << 5);
    }
}
#endif
