/* *****************************************************************************
 * File:   spimem.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 01 22 11:15
 * 
 * Description: SPI Interface To External Memory
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef SPIMEM_H
#define	SPIMEM_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdint.h>
#include "extmemcfg.h"
#include "hmlib.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants Definitions
 **************************************************************************** */

#define SPIMEM_EXT_MEM_USE_SPIMEM_QUEUE_OLD 0   /* Use or Not Old Queue in spimem module (the queue in spi module is different) */
/* with New Queue Should be fixed Recorder call from different ISR Levels -> #define SPIMEM_EXT_MEM_USE_SPIMEM_QUEUE_OLD 0 */


#if SPIMEM_EXT_MEM_USE_SPIMEM_QUEUE_OLD
#define SPIMEM_DATA_BUFFER_SIZE         32      //was 256   consider to fit for recorder memory read or implement chunking if needed to shrink the buffer or implement variable buffer size
#define EXT_MEM_RAM_CMD_QUEUE_COUNT     16      /* Count Queue Commands increased from 8 to 16 to fix sometimes read 0x0000 0xFFFF from external ram */
#define EXT_MEM_EEPROM_CMD_QUEUE_COUNT  8       /* Count Queue Commands */
#define EXT_MEM_CHUNKED_SIZE_MAX_COUNT  8
#else
#define SPIMEM_EXT_MEM_QUEUE_COUNT      16      /* New Queue Implementation */  
#define SPIMEM_DATA_BUFFER_SIZE         128     /* New Queue Implementation */  
#endif


#if USE_SPI_FLIR
#define EXT_MEM_FLIR_CMD_QUEUE_COUNT   8      /* Count Queue Commands */
#define SPI_FLIR_DATA_BUFFER_SIZE    240        /* Payload Size (Packet Without ID and CRC) */
#endif


/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */
typedef enum
{
    EXT_MEM_STA_IDLE,
    EXT_MEM_STA_FILL,
    EXT_MEM_STA_FULL,
    EXT_MEM_STA_BUSY,
    EXT_MEM_STA_READ,
            
    EXT_MEM_STA_COUNT  
}eExtMemStatus_t;

typedef enum
{
    EXT_MEM_CMD_IDLE,
    EXT_MEM_CMD_WRSR    = 0x01,     /* Write Status Register */
    EXT_MEM_CMD_WR      = 0x02,
    EXT_MEM_CMD_RD      = 0x03,
    EXT_MEM_CMD_WRDI    = 0x04,     /* Write Disable */
    EXT_MEM_CMD_RDSR    = 0x05,     /* Read Status Register */
    EXT_MEM_CMD_WREN    = 0x06,     /* Write Enable */
    EXT_MEM_CMD_WAKE    = 0xAB,     /* Wake */
    EXT_MEM_CMD_SLEEP   = 0xB9,     /* Sleep */
    EXT_MEM_CMD_RDID_DEV= 0x9F,     /* MB85RS4MT Read Device ID */
    EXT_MEM_CMD_FSTRD   = 0x0B,     /* MB85RS4MT */
      
    /* see Datasheet */             
    EXT_MEM_CMD_WRIDPAGE= 0x82,     /* M95M02 - Write Identification Page */
    EXT_MEM_CMD_RDIDPAGE= 0x83,     /* M95M02 - Read Identification Page */
    /* see Datasheet */        
    EXT_MEM_CMD_LID     = 0x82,     /* M95M02 - Locks the Identification page in read-only mode */
    EXT_MEM_CMD_RDLS    = 0x83,     /* M95M02 - Reads the Identification page lock status */
            
    EXT_MEM_CMD_COUNT  
}eExtMemCommand_t;

typedef enum
{
    EXT_MEM_SEL_ALL = 0xFF,                    
    EXT_MEM_SEL_EEPROM = 0,
    EXT_MEM_SEL_RAM,
            
    #if USE_SPI_FLIR
    EXT_MEM_SEL_FLIR,
    #endif
            
    EXT_MEM_SEL_COUNT  
}eExtMemSelect_t;

#if USE_SPI_FLIR

typedef enum
{
    /* Pyramid ZIP Blank Frame Dimensions */
    PYRAMID_BLANK_PIXELS_LEFT = 3,
    PYRAMID_BLANK_PIXELS_TOP = 3,
    PYRAMID_BLANK_PIXELS_RIGHT = 3,
    PYRAMID_BLANK_PIXELS_BOTTOM = 3,
    PYRAMID_BLANK_PIXEL_BYTES_SIZE = 2   /* With RAW Data it is Always 2 bytes */
}ePyramidBlankPixel_t;

typedef enum
{
    VOSPI_IDLE,                    
    VOSPI_START,
    VOSPI_SYNC,
    VOSPI_RUN,
    VOSPI_STOP,
    VOSPI_SINGLE,
            
    VOSPI_COUNT  
}eVoSPIState_t;

typedef enum
{
    VOSPI_ZIP_NONE,                    
    VOSPI_ZIP_BYTE,
    VOSPI_ZIP_VBIT,
    VOSPI_ZIP_PYRAMID,
    VOSPI_ZIP_BYTE_PREV_PIXEL_DIFF,
            
    VOSPI_ZIP_COUNT  
}eVoSPIPacketZIP_t;

#endif

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
typedef struct
{
    uint8_t Command;
    uint8_t AddressHiOrStatusData;      /* Read Status Register Command returned Data */
    uint8_t AddressMi;                  /* Middle Address */
    uint8_t AddressLo;                  /* Low Address */
    uint8_t Data[SPIMEM_DATA_BUFFER_SIZE];
}sSPIDMATransferBuffer_t;



typedef struct
{
    volatile eExtMemStatus_t eExtMemStatus;     
    uint16_t ExtMemDataCnt;
    eExtMemCommand_t eExtMemCommand;        
    uint32_t ExtMemAddress;             
}sExtMemCmd_t;



#if USE_SPI_FLIR
typedef struct
{
    uint8_t IDHi;
    uint8_t IDLo;
    uint8_t CRCHi;
    uint8_t CRCLo;
    uint8_t Data[SPI_FLIR_DATA_BUFFER_SIZE];
    
    //for now compatibility not needed
    //uint8_t Data[SPIMEM_DATA_BUFFER_SIZE];    // made for compatibility with sSPIDMATransferBuffer_t or fix whole project 
                                                 // might be not needed compatibility because the expected bug was not fixed 
}sSPIDMAFlirTransferBuffer_t;

typedef struct
{
    volatile eExtMemStatus_t eExtMemStatus; /* Same order as in sExtMemCmd_t */          
    uint16_t ExtMemDataCnt;                 /* Same order as in sExtMemCmd_t */
    /* data below not used - only for compatibility with  */
    /* to remove fields below - fix in functions vExtMemGetFillIndex and vExtMemFindSendIndex */
    eExtMemCommand_t eExtMemCommand;        
    uint32_t ExtMemAddress;             
}sFlirCmd_t;
#endif




/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */
extern sSPIDMATransferBuffer_t sTestExtMemTrsfBuf;
extern sExtMemCmd_t sTestExtMemCmd;

#if USE_SPI_FLIR
extern eVoSPIState_t eVoSPIStateReq;
extern HMDRV_U16 VoSPIPacketLength;
extern HMDRV_U16 VoSPIPacketModbusLengthPingPong;
extern HMDRV_U16 FlirImageSwitchPingPong; 
extern eVoSPIPacketZIP_t eVoSPIPacketZIP; 
#endif

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */

/* *****************************************************************************
 * SPI Initialization
 **************************************************************************** */
void        SPIMEM_vInit(uint32_t u32Fcy);

/* *****************************************************************************
 * SPI External Memory Interface Functions
 **************************************************************************** */
HMDRV_ADDR  SPIMEM_pExtMemRamToIntRam(HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSizeReq, HMDRV_ADDR_PNTR_MODIFIER HMDRV_SIZE8* pnSizeRead, HMDRV_BUFF_PNTR_MODIFIER HMDRV_U16* pu16GlobalQueueID);
void        SPIMEM_vReleaseQueueBuffer(HMDRV_BUFF_PNTR_MODIFIER HMDRV_U16* pu16GlobalQueueID);
HMDRV_ADDR  SPIMEM_pExtMemRamFromIntRam(HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize, HMDRV_BUFF_PNTR_MODIFIER HMDRV_U16* pu16GlobalQueueID);/* nSize trunk from SPI Transfer buffer size->bad write to memory location may occur */
void        SPIMEM_vExtMemRamFromIntRamCopyData(HMDRV_BUFF_PNTR_MODIFIER HMDRV_U16* pu16GlobalQueueID);
HMDRV_ADDR  SPIMEM_pBufferAddressGet(HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize, HMDRV_BUFF_PNTR_MODIFIER HMDRV_U16* pu16GlobalQueueID);
void        SPIMEM_vSendBufferToSPI(HMDRV_SIZE8 nSize, HMDRV_BUFF_PNTR_MODIFIER HMDRV_U16* pu16GlobalQueueID);
void        SPIMEM_vSPICommandQueue(eExtMemSelect_t eMemoryIndex, eExtMemCommand_t command, uint32_t address, uint16_t size, sSPIDMATransferBuffer_t* buffer);

/* *****************************************************************************
 * Test Related
 **************************************************************************** */
void        SPIMEM_vTestSPICommandInit(sExtMemCmd_t* handle, eExtMemCommand_t command, uint32_t address, uint16_t size);
void        SPIMEM_vSPITestCommandQueue(void);


#if USE_SPI_FLIR
/* *****************************************************************************
 * FLIR Related
 **************************************************************************** */
void SPIMEM_vProcessVoSPI(void);
void SPIMEM_vSetStateVoSPIReq(eVoSPIState_t eState);  

uint32_t SPIMEM_vGetFlirImageDataBuffer(void);
void SPIMEM_vSetFlirImageDataBuffer(uint32_t u32DataAddress);
void SPIMEM_vSetFlirImageDataBufferPing(uint32_t u32DataAddress);
void SPIMEM_vSetFlirImageDataBufferPong(uint32_t u32DataAddress);

uint32_t SPIMEM_vGetFlirImageDataBufferZIP(void);
void SPIMEM_vSetFlirImageDataBufferZIP(uint32_t u32DataAddress);
void SPIMEM_vSetFlirImageDataBufferZIPPing(uint32_t u32DataAddress);
void SPIMEM_vSetFlirImageDataBufferZIPPong(uint32_t u32DataAddress);

void SPIMEM_vGetFlirVoSPIPacket(HMDRV_SIZE8 nSize);
void SPIMEM_vSetVoSPIPacketModbusLengthPingPong(void);
#endif

#endif	/* SPIMEM_H */
