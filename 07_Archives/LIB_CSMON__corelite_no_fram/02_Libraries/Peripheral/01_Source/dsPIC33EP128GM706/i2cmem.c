/* *****************************************************************************
 * File:   i2cmem.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 02 05 11:42
 * 
 * Description: I2C Interface To External RTCC Memory
 * 
 **************************************************************************** */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <xc.h>
#include <stdint.h>
#include <stdlib.h>
#include "hmdrv.h"
#include "i2cmem.h"
#include "boardcfg.h"

#ifdef BOARDCFG_USE_MCC_I2C  
#include "i2c1.h"
#include "i2c2.h"
#endif

/* *****************************************************************************
 * Configuration and General Definitions and Constants
 **************************************************************************** */
#define USE_I2C_ISR         1

#define RTCC_SRAM_ADDR_7BIT     0x6F            /* 0xDE / 2         -> 111 */  
#define EEPROM_ADDR_7BIT        0x57            /* 0xAE / 2         ->  87 */  
#define PICAUX_ADDR_7BIT        0x16            /* 0x16 for pic16   ->  22 */  
#define FLIR_ADDR_7BIT          0x2A            /* 0x2A             ->  42 */  

#define EXT_MEM_RTCC_SRAM_ADDRESS_OFFSET    0x20

#if USE_SPI_FLIR
#define I2C_WRITE_REVERSE_ORDER_LSB_MSB     0
#endif

#define MCC_I2C_TRANSACTION_BLOCK_BUFFER_LENGTH  64

#define EDS_SPACE   __eds__

#define Fi2c            400000      /* Hz */
#define Fi2cDelay       130         /* nSec */
      
#define I2C_MAX_RETRY   3
#define ONE_BYTE        1
#define TWO_BYTE        2

#define I2C_ADDRWIDTH   ONE_BYTE    /* ADDRESS SIZE */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */


#if USE_SPI_FLIR
/* *****************************************************************************
 * Lepton Flir Camera usage.Represents the different result codes the camera can return.
 **************************************************************************** */
typedef enum Result
{
    LEP_OK                              = 0,        /* Camera ok */
    LEP_COMM_OK                         = LEP_OK,   /* Camera comm ok (same as LEP_OK) */
    LEP_ERROR                           = -1,       /* Camera general error */
    LEP_NOT_READY                       = -2,       /* Camera not ready error */
    LEP_RANGE_ERROR                     = -3,       /* Camera range error */
    LEP_CHECKSUM_ERROR                  = -4,       /* Camera checksum error */
    LEP_BAD_ARG_POINTER_ERROR           = -5,       /* Camera Bad argument error */
    LEP_DATA_SIZE_ERROR                 = -6,       /* Camera byte count error */
    LEP_UNDEFINED_FUNCTION_ERROR        = -7,       /* Camera undefined function error */
    LEP_FUNCTION_NOT_SUPPORTED          = -8,       /* Camera function not yet supported error */
    LEP_DATA_OUT_OF_RANGE_ERROR         = -9,       /* Camera range error *//* Camera input DATA is out of valid range error */
    LEP_COMMAND_NOT_ALLOWED             = -11,      /* Camera unable to execute command due to current camera state */
    /* OTP access errors */
    LEP_OTP_WRITE_ERROR                 = -15,      /*!< Camera OTP write error */
    LEP_OTP_READ_ERROR                  = -16,      /* double bit error detected (uncorrectible) */
    LEP_OTP_NOT_PROGRAMMED_ERROR        = -18,      /* Flag read as non-zero */
    /* I2C Errors */     
    LEP_ERROR_I2C_BUS_NOT_READY         = -20,      /* I2C Bus Error - Bus Not Avaialble */
    LEP_ERROR_I2C_BUFFER_OVERFLOW       = -22,      /* I2C Bus Error - Buffer Overflow */
    LEP_ERROR_I2C_ARBITRATION_LOST      = -23,      /* I2C Bus Error - Bus Arbitration Lost */
    LEP_ERROR_I2C_BUS_ERROR             = -24,      /* I2C Bus Error - General Bus Error */
    LEP_ERROR_I2C_NACK_RECEIVED         = -25,      /* I2C Bus Error - NACK Received */
    LEP_ERROR_I2C_FAIL                  = -26,      /* I2C Bus Error - General Failure */
    /* Processing Errors */
    LEP_DIV_ZERO_ERROR                  = -80,      /* Attempted div by zero */
    /* Comm Errors */
    LEP_COMM_PORT_NOT_OPEN              = -101,     /* Comm port not open */
    LEP_COMM_INVALID_PORT_ERROR         = -102,     /* Comm port no such port error */
    LEP_COMM_RANGE_ERROR                = -103,     /* Comm port range error */
    LEP_ERROR_CREATING_COMM             = -104,     /* Error creating comm */
    LEP_ERROR_STARTING_COMM             = -105,     /* Error starting comm */
    LEP_ERROR_CLOSING_COMM              = -106,     /* Error closing comm */
    LEP_COMM_CHECKSUM_ERROR             = -107,     /* Comm checksum error */
    LEP_COMM_NO_DEV                     = -108,     /* No comm device */
    LEP_TIMEOUT_ERROR                   = -109,     /* Comm timeout error */
    LEP_COMM_ERROR_WRITING_COMM         = -110,     /* Error writing comm */
    LEP_COMM_ERROR_READING_COMM         = -111,     /* Error reading comm */
    LEP_COMM_COUNT_ERROR                = -112,     /* Comm byte count error */
    /* Other Errors */    
    LEP_OPERATION_CANCELED              = -126,     /* Camera operation canceled */
    LEP_UNDEFINED_ERROR_CODE            = -127      /* Undefined error */
} LEP_RESULT;

typedef enum 
{
    FLIR_BIT_POS_STA_LO_BUSY = 0x01,
            
    FLIR_BIT_POS_COUNT       
}eFlirI2CRegBitPos_t;



typedef enum 
{
    FLIR_ADR_REG_POW    = 0x0000,
    FLIR_ADR_REG_STA    = 0x0002,
    FLIR_ADR_REG_STA_LO = 0x0002,
    FLIR_ADR_REG_STA_HI = 0x0003,
    FLIR_ADR_REG_CMD    = 0x0004,
    FLIR_ADR_REG_LEN    = 0x0006,
    FLIR_ADR_REG_D00    = 0x0008,
    FLIR_ADR_REG_D01    = 0x000A,
    FLIR_ADR_REG_D02    = 0x000C,
    FLIR_ADR_REG_D03    = 0x000E,
    FLIR_ADR_REG_D04    = 0x0010,
    FLIR_ADR_REG_D05    = 0x0012,
    FLIR_ADR_REG_D06    = 0x0014,
    FLIR_ADR_REG_D07    = 0x0016,
    FLIR_ADR_REG_D08    = 0x0018,
    FLIR_ADR_REG_D09    = 0x001A,
    FLIR_ADR_REG_D10    = 0x001C,
    FLIR_ADR_REG_D11    = 0x001E,
    FLIR_ADR_REG_D12    = 0x0020,
    FLIR_ADR_REG_D13    = 0x0022,
    FLIR_ADR_REG_D14    = 0x0024,
    FLIR_ADR_REG_D15    = 0x0026,
            
    FLIR_ADR_REG_END    = 0xFFFF,
}eFlirI2CAdrReg_t;


typedef enum 
{
    FLIR_ADR_REG_16BIT_POW    = FLIR_ADR_REG_POW/2,
    FLIR_ADR_REG_16BIT_STA    = FLIR_ADR_REG_STA/2,
    FLIR_ADR_REG_16BIT_CMD    = FLIR_ADR_REG_CMD/2,
    FLIR_ADR_REG_16BIT_LEN    = FLIR_ADR_REG_LEN/2,
    FLIR_ADR_REG_16BIT_D00    = FLIR_ADR_REG_D00/2,
    FLIR_ADR_REG_16BIT_D01    = FLIR_ADR_REG_D01/2,
    FLIR_ADR_REG_16BIT_D02    = FLIR_ADR_REG_D02/2,
    FLIR_ADR_REG_16BIT_D03    = FLIR_ADR_REG_D03/2,
    FLIR_ADR_REG_16BIT_D04    = FLIR_ADR_REG_D04/2,
    FLIR_ADR_REG_16BIT_D05    = FLIR_ADR_REG_D05/2,
    FLIR_ADR_REG_16BIT_D06    = FLIR_ADR_REG_D06/2,
    FLIR_ADR_REG_16BIT_D07    = FLIR_ADR_REG_D07/2,
    FLIR_ADR_REG_16BIT_D08    = FLIR_ADR_REG_D08/2,
    FLIR_ADR_REG_16BIT_D09    = FLIR_ADR_REG_D09/2,
    FLIR_ADR_REG_16BIT_D10    = FLIR_ADR_REG_D10/2,
    FLIR_ADR_REG_16BIT_D11    = FLIR_ADR_REG_D11/2,
    FLIR_ADR_REG_16BIT_D12    = FLIR_ADR_REG_D12/2,
    FLIR_ADR_REG_16BIT_D13    = FLIR_ADR_REG_D13/2,
    FLIR_ADR_REG_16BIT_D14    = FLIR_ADR_REG_D14/2,
    FLIR_ADR_REG_16BIT_D15    = FLIR_ADR_REG_D15/2,
            
    FLIR_ADR_REG_16BIT_END    = 0xFFFF,
}eFlirI2CAdrReg16Bit_t;



typedef enum
{
    FLIR_I2C_STA_IDLE,
    FLIR_I2C_STA_GET_STA_001,
    FLIR_I2C_STA_SET_REG_CNT,
    FLIR_I2C_STA_SET_CMD_REG,
    FLIR_I2C_STA_GET_STA_002,
    FLIR_I2C_STA_GET_DAT_BUF,
    FLIR_I2C_STA_SET_DAT_BUF,
    FLIR_I2C_STA_ERR,
    FLIR_I2C_STA_DONE,
            
    FLIR_I2C_STA_COUNT            
    
}eFlirI2CSta_t;

#endif

typedef enum 
{
    I2C_CTRL_BYTE_RTCC_SRAM    = (RTCC_SRAM_ADDR_7BIT << 1),  /* RD or WR made in the Command Function Call */
    I2C_CTRL_BYTE_RTCC_SRAM_WR = (RTCC_SRAM_ADDR_7BIT << 1),
    I2C_CTRL_BYTE_RTCC_SRAM_RD =((RTCC_SRAM_ADDR_7BIT << 1) | 0x01),
    I2C_CTRL_BYTE_EEPROM       = (   EEPROM_ADDR_7BIT << 1),  /* RD or WR made in the Command Function Call */
    I2C_CTRL_BYTE_EEPROM_WR    = (   EEPROM_ADDR_7BIT << 1),
    I2C_CTRL_BYTE_EEPROM_RD    =((   EEPROM_ADDR_7BIT << 1) | 0x01),
    I2C_CTRL_BYTE_PICAUX       = (   PICAUX_ADDR_7BIT << 1),  /* RD or WR made in the Command Function Call */
    I2C_CTRL_BYTE_PICAUX_WR    = (   PICAUX_ADDR_7BIT << 1),
    I2C_CTRL_BYTE_PICAUX_RD    =((   PICAUX_ADDR_7BIT << 1) | 0x01),
    I2C_CTRL_BYTE_FLIR         = (     FLIR_ADDR_7BIT << 1),  /* RD or WR made in the Command Function Call */
    I2C_CTRL_BYTE_FLIR_WR      = (     FLIR_ADDR_7BIT << 1),
    I2C_CTRL_BYTE_FLIR_RD      =((     FLIR_ADDR_7BIT << 1) | 0x01),
            
    I2C_CTRL_BYTE_10BIT_SUPPORT=0xFFFF  /* to make 16-bit enumeration */
}eI2CCtrlByte_t;


typedef enum 
{
    /* Timekeeping */
    EXT_MEM_I2C_ADR_RTCSEC,
    EXT_MEM_I2C_ADR_RTCMIN,
    EXT_MEM_I2C_ADR_RTCHOUR,
    EXT_MEM_I2C_ADR_RTCWKDAY,
    EXT_MEM_I2C_ADR_RTCDATE,
    EXT_MEM_I2C_ADR_RTCMTH,
    EXT_MEM_I2C_ADR_RTCYEAR,
    EXT_MEM_I2C_ADR_CONTROL,
    EXT_MEM_I2C_ADR_OSCTRIM,
    EXT_MEM_I2C_ADR_EEUNLOCK,
  
    /* Alarm 0 */
    EXT_MEM_I2C_ADR_ALM0SEC,
    EXT_MEM_I2C_ADR_ALM0MIN,
    EXT_MEM_I2C_ADR_ALM0HOUR,
    EXT_MEM_I2C_ADR_ALM0WKDAY,
    EXT_MEM_I2C_ADR_ALM0DATE,
    EXT_MEM_I2C_ADR_ALM0MTH,
    EXT_MEM_I2C_ADR_ALM0RESERVED_DONT_USE,

    /* Alarm 1 */
    EXT_MEM_I2C_ADR_ALM1SEC,
    EXT_MEM_I2C_ADR_ALM1MIN,
    EXT_MEM_I2C_ADR_ALM1HOUR,
    EXT_MEM_I2C_ADR_ALM1WKDAY,
    EXT_MEM_I2C_ADR_ALM1DATE,
    EXT_MEM_I2C_ADR_ALM1MTH,
    EXT_MEM_I2C_ADR_ALM1RESERVED_DONT_USE,
            
    /* Power Down Time Stamp */
    EXT_MEM_I2C_ADR_PWRDNMIN,
    EXT_MEM_I2C_ADR_PWRDNHOUR,
    EXT_MEM_I2C_ADR_PWRDNDATE,
    EXT_MEM_I2C_ADR_PWRDNMTH,
            
    /* Power Up Time Stamp */
    EXT_MEM_I2C_ADR_PWRUPMIN,
    EXT_MEM_I2C_ADR_PWRUPHOUR,
    EXT_MEM_I2C_ADR_PWRUPDATE,
    EXT_MEM_I2C_ADR_PWRUPMTH,                   

}eI2CAddress_t;

typedef enum
{
    I2C_CMD_STA_IDLE                    = 0,        
    I2C_CMD_STA_START                   = 1,
    I2C_CMD_STA_START_WAIT              = 2,
    I2C_CMD_STA_SEND_CTRL               = 3,
    I2C_CMD_STA_SEND_ADDRESS_HI         = 4,
    I2C_CMD_STA_SEND_ADDRESS_LO         = 5,
    I2C_CMD_STA_SEND_DATA               = 6,
    I2C_CMD_STA_SEND_DATA_ACK           = 7,
            
    I2C_CMD_STA_READ_DATA_START         = 8,
    I2C_CMD_STA_READ_DATA_CONTROL       = 9,
    I2C_CMD_STA_READ_DATA_CONTROL_ACK   =10,
    I2C_CMD_STA_READ_DATA_RECEIVE       =11,
    I2C_CMD_STA_READ_DATA_RECEIVE_ACK   =12,
    I2C_CMD_STA_READ_DATA_RECEIVE_END   =13,        
            
    I2C_CMD_STA_STOP                    =14,
    I2C_CMD_STA_STOP_END                =15,
    I2C_CMD_STA_ERROR                   =16,        
    I2C_CMD_STA_ERROR_END               =17,
    I2C_CMD_STA_RETRY                   =18,
    I2C_CMD_STA_RETRY_END               =19,        
    I2C_CMD_STA_ACK_POOLING             =20,        
            
            
    I2C_CMD_STA_END                     =21,
            
    I2C_CMD_STA_COUNT
            
}eI2CCmdState_t;

typedef enum
{
    I2C_MESSAGE_FAIL,
    I2C_MESSAGE_PENDING,
    I2C_MESSAGE_COMPLETE,
    I2C_STUCK_START,
    I2C_MESSAGE_ADDRESS_NO_ACK,
    I2C_DATA_NO_ACK,
    I2C_LOST_STATE
} I2C_MESSAGE_STATUS;


/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */  


typedef struct 
{
    volatile eI2CCmd_t eI2CCmd;
    eI2CCtrlByte_t eI2CCmdCtrlByte;
    uint16_t u16I2CCmdAddress;
    uint8_t* pI2CCmdBuffer;
    uint16_t u16I2CCmdSize;
    uint8_t u8I2CAddrSize;
    eI2CCmd_t eI2CCmdCommand;
    volatile eI2CCmdState_t eI2CCmdState;
    
}sI2CCommandData_t;



/* *****************************************************************************
 * Variables
 **************************************************************************** */  

#if USE_SPI_FLIR
eFlirI2CSta_t eFlirI2CSta = FLIR_I2C_STA_IDLE;
eFlirI2CCmd_t eFlirI2CCmd = FLIR_I2C_CMD_IDLE;
uint16_t u16FlirI2CCommand;
uint16_t u16FlirI2CSize;
uint8_t* pu8FlirI2CData;

uint8_t u8ReverseByteOrder0 = I2C_WRITE_REVERSE_ORDER_LSB_MSB;
uint8_t u8ReverseByteOrder1 = 0;
uint8_t u8ReverseByteOrder2 = 0;

uint16_t u16FlirI2CCommandReq;
uint16_t u16FlirI2CSizeReq;

uint8_t* pI2CFlirBufferRD = &FLIRData[0];//(uint8_t*)&RTCC;
uint8_t* pI2CFlirBufferWR = &FLIRData[0];//(uint8_t*)&RTCC;
uint8_t FLIRRegisters[4+1024];
uint8_t FLIRData[1024];
#endif

uint32_t u32Fi2c = Fi2c;
uint32_t u32Fi2cDelay = (uint32_t)((1000000000.0 / Fi2cDelay)+ 0.5); /* Frequency of 130 nsec delay in Hz */
uint16_t    jDone;

#if 0   /* Could be Obsolete Code */
sI2CCommandData_t sI2CCommandDataI2C1 = 
{
    I2C_CMD_IDLE,
    0,
    0,
    NULL,
    0,
    0,
    I2C_CMD_IDLE,
    I2C_CMD_STA_IDLE,
    
};

sI2CCommandData_t sI2CCommandDataI2C2 = 
{
    I2C_CMD_IDLE,
    0,
    0,
    NULL,
    0,
    0,
    I2C_CMD_IDLE,
    I2C_CMD_STA_IDLE,
    
};

sI2CCommandData_t* psI2CCommandDataI2C1 = &sI2CCommandDataI2C1;
sI2CCommandData_t* psI2CCommandDataI2C2 = &sI2CCommandDataI2C2;
#endif

#ifndef BOARDCFG_USE_MCC_I2C
eI2CCtrlByte_t eI2CCmdCtrlByte = 0;
uint16_t u16I2CCmdAddress = 0;
EDS_SPACE uint8_t* pI2CCmdBuffer = NULL;
uint16_t u16I2CCmdSize = 0;
uint8_t u8I2CAddrSize = 0;
eI2CCmd_t eI2CCmdCommand = I2C_CMD_IDLE;
volatile eI2CCmdState_t eI2CCmdState = I2C_CMD_STA_IDLE;
uint16_t rtrycntr = 0;
uint16_t cntr = 0;
uint8_t u8UseAckPoolingWaitWriteComplete = 0;       /* Use Ack Pool To Wait Write Complete - Default Off */
#endif

eI2CCmd_t eI2CCmd = I2C_CMD_IDLE;

sRTCC_t RTCC;
EDS_SPACE uint8_t SRAM_RTCC[64];
EDS_SPACE uint8_t EEPROM_RTCC[256];
EDS_SPACE uint8_t PICAUX[256];
eI2CCtrlByte_t eI2CCtrlByte = I2C_CTRL_BYTE_RTCC_SRAM_RD;
uint16_t u16I2CAddress = 0;
uint16_t u16I2CSizeRD = 8;
uint16_t u16I2CSizeWR = 8;


HMDRV_SIZE8 u8I2CModule = I2C_DEFAULT_MODULE;
EDS_SPACE uint8_t u8I2CBufferRD[I2C_COUNT][256];
EDS_SPACE uint8_t u8I2CBufferWR[I2C_COUNT][256];
EDS_SPACE uint8_t* pI2CBufferRD[I2C_COUNT] = {u8I2CBufferRD[I2C_1],u8I2CBufferRD[I2C_2]};
EDS_SPACE uint8_t* pI2CBufferWR[I2C_COUNT] = {u8I2CBufferWR[I2C_1],u8I2CBufferWR[I2C_2]};

uint8_t u8I2CBufferMCCUse[MCC_I2C_TRANSACTION_BLOCK_BUFFER_LENGTH];
uint8_t* pu8I2CBufferMCCUse = &u8I2CBufferMCCUse[0];

I2C_MESSAGE_STATUS u8DummyStatus;
I2C_MESSAGE_STATUS* pu8StatusTransaction = &u8DummyStatus;

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */  
//#ifndef BOARDCFG_USE_MCC_I2C    
void I2CMEM_vWrite(eI2CCtrlByte_t eI2CCtrlByteLocal, uint16_t u16Address, EDS_SPACE uint8_t* pdData, uint16_t u16Size, uint8_t u8AddressSize, uint8_t u8I2CModule);
void I2CMEM_vRead (eI2CCtrlByte_t eI2CCtrlByteLocal, uint16_t u16Address, EDS_SPACE uint8_t* pdData, uint16_t u16Size, uint8_t u8AddressSize, uint8_t u8I2CModule);
//#else
//void I2CMEM_vWrite(eI2CCtrlByte_t eI2CCtrlByteLocal, uint16_t u16Address, EDS_SPACE uint8_t* pdData, uint16_t u16Size, uint8_t u8AddressSize, uint8_t u8I2CModule, I2C_MESSAGE_STATUS* pu8StatusTransaction);
//void I2CMEM_vRead (eI2CCtrlByte_t eI2CCtrlByteLocal, uint16_t u16Address, EDS_SPACE uint8_t* pdData, uint16_t u16Size, uint8_t u8AddressSize, uint8_t u8I2CModule, I2C_MESSAGE_STATUS* pu8StatusTransaction);
//#endif


void I2CMEM_vProcessRWISR(void);
void I2CMEM_vProcessRWISR2(void);

#if USE_SPI_FLIR
void I2CMEM_vFlirProcessCmd(void);
#endif





/* *****************************************************************************
 * Function Definitions
 **************************************************************************** */  





/* *****************************************************************************
 * Functions For Application Usage
 **************************************************************************** */  


/* *****************************************************************************
 * I2CMEM_vCopyFromExtMem
 * 
 * Description: HMLIB Compatible copy function:
 * HMDRV_BPTR HMDRV_CopyToBuffer(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
 **************************************************************************** */
void I2CMEM_vCopyFromExtMem(HMDRV_BPTR pDestAddr, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_U16 addr = (HMDRV_U16)(HMDRV_ADDR_SIZE) nSrcAddr;
    HMDRV_U8* pd = (HMDRV_U8*)pDestAddr;
    HMDRV_SIZE8 addrSize = 1;
    EDS_SPACE HMDRV_U8* ps;
	eI2CCtrlByte_t eI2CCtrlByteRTCRead;
    HMDRV_SIZE8 I2CModule = u8I2CModule;
    #ifdef BOARDCFG_USE_MCC_I2C    
    //I2C_MESSAGE_STATUS u8StatusI2C;
    #endif
    
    /* FLIR */
    if (addr >= EXT_MEM_I2C_ADDR_OFFSET_FLIR)  
    {
        addr -= EXT_MEM_I2C_ADDR_OFFSET_FLIR;
        eI2CCtrlByteRTCRead = I2C_CTRL_BYTE_FLIR;
        addrSize = 2;
    }
    /* PICAUX */
    else if (addr >= EXT_MEM_I2C_ADDR_OFFSET_PICAUX)  
    {
        addr -= EXT_MEM_I2C_ADDR_OFFSET_PICAUX;
        eI2CCtrlByteRTCRead = I2C_CTRL_BYTE_PICAUX;
    }
    /* RTC EERPOM */
    else if (addr >= EXT_MEM_I2C_ADDR_OFFSET_RTC_EEPROM)  
    {
        addr -= EXT_MEM_I2C_ADDR_OFFSET_RTC_EEPROM;
        eI2CCtrlByteRTCRead = I2C_CTRL_BYTE_EEPROM;
    }
    /* RTC SRAM */
    else
    {
        eI2CCtrlByteRTCRead = I2C_CTRL_BYTE_RTCC_SRAM;
    }

    ps = pI2CBufferRD[I2CModule];
    
    #ifndef BOARDCFG_USE_MCC_I2C    
    while(eI2CCmd == I2C_CMD_BUSY){;} /*  wait pending transfer */    
    I2CMEM_vRead(eI2CCtrlByteRTCRead, addr, ps , nSize, addrSize, I2CModule);
    while(eI2CCmd == I2C_CMD_BUSY){;} /*  wait pending transfer */
    #else
    I2CMEM_vRead(eI2CCtrlByteRTCRead, addr, ps , nSize, addrSize, I2CModule);
    //I2CMEM_vRead(eI2CCtrlByteRTCRead, addr, ps , nSize, addrSize, I2CModule, &u8StatusI2C);
    //while(u8StatusI2C == I2C_MESSAGE_PENDING){;} /*  wait pending transfer - if commented - made internally */
    #endif

    while(nSize--)
    {
        *pd++ = *ps++;
    }
    
}  

/* *****************************************************************************
 * I2CMEM_vCopyToExtMem
 * 
 * Description: HMLIB Compatible copy function:
 * HMDRV_BPTR HMDRV_CopyFromBuffer         (HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
 **************************************************************************** */
void I2CMEM_vCopyToExtMem(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_U16 addr = (HMDRV_U16)(HMDRV_ADDR_SIZE) nDestAddr;
    EDS_SPACE HMDRV_U8* pb;
    HMDRV_SIZE8 size = nSize;
    eI2CCtrlByte_t eI2CCtrlByteRTC;
    HMDRV_SIZE8 addrSize = 1;
    HMDRV_SIZE8 I2CModule = u8I2CModule;
    #ifdef BOARDCFG_USE_MCC_I2C    
    //I2C_MESSAGE_STATUS u8StatusI2C;
    #endif
    
    
    /* FLIR */
    if (addr >= EXT_MEM_I2C_ADDR_OFFSET_FLIR)  
    {
        addr -= EXT_MEM_I2C_ADDR_OFFSET_FLIR;
        eI2CCtrlByteRTC = I2C_CTRL_BYTE_FLIR;
        addrSize = 2;
        #ifdef EXT_MEM_I2C_MODULE_FLIR
        I2CModule = EXT_MEM_I2C_MODULE_FLIR;
        #endif
    }
    /* PICAUX */
    else if (addr >= EXT_MEM_I2C_ADDR_OFFSET_PICAUX)  
    {
        addr -= EXT_MEM_I2C_ADDR_OFFSET_PICAUX;
        eI2CCtrlByteRTC = I2C_CTRL_BYTE_PICAUX;
        #ifdef EXT_MEM_I2C_MODULE_PICAUX
        I2CModule = EXT_MEM_I2C_MODULE_PICAUX;
        #endif
    }
    /* RTC EERPOM */
    else if (addr >= EXT_MEM_I2C_ADDR_OFFSET_RTC_EEPROM)  
    {
        addr -= EXT_MEM_I2C_ADDR_OFFSET_RTC_EEPROM;
        eI2CCtrlByteRTC = I2C_CTRL_BYTE_EEPROM;
        #ifdef EXT_MEM_I2C_MODULE_RTC
        I2CModule = EXT_MEM_I2C_MODULE_RTC;
        #endif
    }
    /* RTC SRAM */
    else
    {
        eI2CCtrlByteRTC = I2C_CTRL_BYTE_RTCC_SRAM;
        #ifdef EXT_MEM_I2C_MODULE_RTC
        I2CModule = EXT_MEM_I2C_MODULE_RTC;
        #endif
    }
    
    pb = pI2CBufferWR[I2CModule];

    while(nSize--)
        *pb++ = *ps++;
    
    #ifndef BOARDCFG_USE_MCC_I2C    
    while(eI2CCmd == I2C_CMD_BUSY){;} /*  wait pending transfer */
    I2CMEM_vWrite(eI2CCtrlByteRTC, addr, pb , size, addrSize, I2CModule);
    #else
    I2CMEM_vWrite(eI2CCtrlByteRTC, addr, pb , size, addrSize, I2CModule);
    //I2CMEM_vWrite(eI2CCtrlByteRTC, addr, pb , size, addrSize, I2CModule, &u8StatusI2C);
    #endif
}

/* *****************************************************************************
 * I2CMEM_vCopyToExtMemWithMask
 * 
 * Description: HMLIB Compatible copy function:
 * void HMDRV_CopyFromBufferWithMask(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
 **************************************************************************** */
void I2CMEM_vCopyToExtMemWithMask(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_U16 addr = (HMDRV_U16)(HMDRV_ADDR_SIZE) nDestAddr;
    HMDRV_U8* pm = ps + nSize;
    HMDRV_U8 mask, stmp, dtmp;
    EDS_SPACE HMDRV_U8* pbRD;
    EDS_SPACE HMDRV_U8* pbWR;
    HMDRV_SIZE8 size = nSize;
    eI2CCtrlByte_t eI2CCtrlByteRTCMask;
    HMDRV_SIZE8 addrSize = 1;
    HMDRV_SIZE8 I2CModule = u8I2CModule;
    #ifdef BOARDCFG_USE_MCC_I2C    
    //I2C_MESSAGE_STATUS u8StatusI2C;
    #endif

    
    /* FLIR */
    if (addr >= EXT_MEM_I2C_ADDR_OFFSET_FLIR)  
    {
        addr -= EXT_MEM_I2C_ADDR_OFFSET_FLIR;
        eI2CCtrlByteRTCMask = I2C_CTRL_BYTE_FLIR;
        addrSize = 2;
        #ifdef EXT_MEM_I2C_MODULE_FLIR
        I2CModule = EXT_MEM_I2C_MODULE_FLIR;
        #endif
    }
    /* PICAUX */
    else if (addr >= EXT_MEM_I2C_ADDR_OFFSET_PICAUX)  
    {
        addr -= EXT_MEM_I2C_ADDR_OFFSET_PICAUX;
        eI2CCtrlByteRTCMask = I2C_CTRL_BYTE_PICAUX;
        #ifdef EXT_MEM_I2C_MODULE_PICAUX
        I2CModule = EXT_MEM_I2C_MODULE_PICAUX;
        #endif
    }
    /* RTC EERPOM */
    else if (addr >= EXT_MEM_I2C_ADDR_OFFSET_RTC_EEPROM)  
    {
        addr -= EXT_MEM_I2C_ADDR_OFFSET_RTC_EEPROM;
        eI2CCtrlByteRTCMask = I2C_CTRL_BYTE_EEPROM;
        #ifdef EXT_MEM_I2C_MODULE_RTC
        I2CModule = EXT_MEM_I2C_MODULE_RTC;
        #endif
    }
    /* RTC SRAM */
    else
    {
        eI2CCtrlByteRTCMask = I2C_CTRL_BYTE_RTCC_SRAM;
        #ifdef EXT_MEM_I2C_MODULE_RTC
        I2CModule = EXT_MEM_I2C_MODULE_RTC;
        #endif
    }
    
    pbRD = pI2CBufferRD[I2CModule];
    pbWR = pI2CBufferWR[I2CModule];
    
    #ifndef BOARDCFG_USE_MCC_I2C    
    while(eI2CCmd == I2C_CMD_BUSY){;} /*  wait pending transfer */
    I2CMEM_vRead(eI2CCtrlByteRTCMask, addr, pbRD , nSize, addrSize, I2CModule);
    while(eI2CCmd == I2C_CMD_BUSY){;} /*  wait pending transfer */
    #else
    I2CMEM_vRead(eI2CCtrlByteRTCMask, addr, pbRD , nSize, addrSize, I2CModule);
    //I2CMEM_vRead(eI2CCtrlByteRTCMask, addr, pbRD , nSize, addrSize, I2CModule, &u8StatusI2C);
    //while(u8StatusI2C == I2C_MESSAGE_PENDING){;} /*  wait pending transfer */
    #endif
    
    while(nSize--)
    {
        mask = *pm++;
        stmp = *ps++;
        dtmp = *pbRD++;

        /* perform AND-masking */
        stmp = (HMDRV_U8) ((stmp & mask) | (dtmp & ~mask));

        /* put the result in write buffer */
        *pbWR++ = stmp;
    }
    
    #ifndef BOARDCFG_USE_MCC_I2C    
    while(eI2CCmd == I2C_CMD_BUSY){;} /*  wait pending transfer */
    I2CMEM_vWrite(eI2CCtrlByteRTCMask, addr, pbWR , size, addrSize, I2CModule);
    #else
    I2CMEM_vWrite(eI2CCtrlByteRTCMask, addr, pbWR , size, addrSize, I2CModule);
    //I2CMEM_vWrite(eI2CCtrlByteRTCMask, addr, pbWR , size, addrSize, I2CModule, &u8StatusI2C);
    #endif
}


/* *****************************************************************************
 * I2CMEM_pExtMemRamToIntRam
 * 
 * Description: Copy Data from external memory to internal RAM and 
 *              returns pointer to internal RAM
 **************************************************************************** */
//EDS_SPACE HMDRV_BPTR I2CMEM_pExtMemRamToIntRam(HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
HMDRV_ADDR I2CMEM_pExtMemRamToIntRam(HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_U16 addr = (HMDRV_U16)(HMDRV_ADDR_SIZE) nSrcAddr;
    EDS_SPACE HMDRV_U8* ps;
	eI2CCtrlByte_t eI2CCtrlByteRTCRead;
    HMDRV_SIZE8 addrSize = 1;
    HMDRV_SIZE8 I2CModule = u8I2CModule;
    #ifdef BOARDCFG_USE_MCC_I2C    
    //I2C_MESSAGE_STATUS u8StatusI2C;
    #endif
    
    /* FLIR */
    if (addr >= EXT_MEM_I2C_ADDR_OFFSET_FLIR)  
    {
        addr -= EXT_MEM_I2C_ADDR_OFFSET_FLIR;
        eI2CCtrlByteRTCRead = I2C_CTRL_BYTE_FLIR;
        addrSize = 2;
        #ifdef EXT_MEM_I2C_MODULE_FLIR
        I2CModule = EXT_MEM_I2C_MODULE_FLIR;
        #endif
    }
    /* PICAUX */
    else if (addr >= EXT_MEM_I2C_ADDR_OFFSET_PICAUX)  
    {
        addr -= EXT_MEM_I2C_ADDR_OFFSET_PICAUX;
        eI2CCtrlByteRTCRead = I2C_CTRL_BYTE_PICAUX;
        #ifdef EXT_MEM_I2C_MODULE_PICAUX
        I2CModule = EXT_MEM_I2C_MODULE_PICAUX;
        #endif
    }
    /* RTC EERPOM */
    else if (addr >= EXT_MEM_I2C_ADDR_OFFSET_RTC_EEPROM)  
    {
        addr -= EXT_MEM_I2C_ADDR_OFFSET_RTC_EEPROM;
        eI2CCtrlByteRTCRead = I2C_CTRL_BYTE_EEPROM;
        #ifdef EXT_MEM_I2C_MODULE_RTC
        I2CModule = EXT_MEM_I2C_MODULE_RTC;
        #endif
    }
    /* RTC SRAM */
    else
    {
        eI2CCtrlByteRTCRead = I2C_CTRL_BYTE_RTCC_SRAM;
        #ifdef EXT_MEM_I2C_MODULE_RTC
        I2CModule = EXT_MEM_I2C_MODULE_RTC;
        #endif
    }
    ps = pI2CBufferRD[I2CModule];

    #ifndef BOARDCFG_USE_MCC_I2C    
    while(eI2CCmd == I2C_CMD_BUSY){;} /*  wait pending transfer */    
    I2CMEM_vRead(eI2CCtrlByteRTCRead, addr, ps , nSize, addrSize, I2CModule);
    while(eI2CCmd == I2C_CMD_BUSY){;} /*  wait pending transfer */
    #else
    I2CMEM_vRead(eI2CCtrlByteRTCRead, addr, ps , nSize, addrSize, I2CModule);
    //I2CMEM_vRead(eI2CCtrlByteRTCRead, addr, ps , nSize, addrSize, I2CModule, &u8StatusI2C);
    //while(u8StatusI2C == I2C_MESSAGE_PENDING){;} /*  wait pending transfer */
    #endif
    
    
    return (HMDRV_ADDR)ps;
}

/* *****************************************************************************
 * I2CMEM_pExtMemRamFromIntRam
 * 
 * Description: returns pointer to internal RAM for 
 *              Copy Data to external memory from internal RAM 
 *              
 **************************************************************************** */
HMDRV_ADDR I2CMEM_pExtMemRamFromIntRam(HMDRV_ADDR nDestAddr)
{
    HMDRV_SIZE8 I2CModule = u8I2CModule;
    EDS_SPACE HMDRV_U8* pb;
    
    /* FLIR */
    if (nDestAddr >= EXT_MEM_I2C_ADDR_OFFSET_FLIR)  
    {
        #ifdef EXT_MEM_I2C_MODULE_FLIR
        I2CModule = EXT_MEM_I2C_MODULE_FLIR;
        #endif
    }
    /* PICAUX */
    else if (nDestAddr >= EXT_MEM_I2C_ADDR_OFFSET_PICAUX)  
    {
        #ifdef EXT_MEM_I2C_MODULE_PICAUX
        I2CModule = EXT_MEM_I2C_MODULE_PICAUX;
        #endif
    }
    /* RTC EERPOM */
    else if (nDestAddr >= EXT_MEM_I2C_ADDR_OFFSET_RTC_EEPROM)  
    {
        #ifdef EXT_MEM_I2C_MODULE_RTC
        I2CModule = EXT_MEM_I2C_MODULE_RTC;
        #endif
    }
    /* RTC SRAM */
    else
    {
        #ifdef EXT_MEM_I2C_MODULE_RTC
        I2CModule = EXT_MEM_I2C_MODULE_RTC;
        #endif
    }
    pb = pI2CBufferWR[I2CModule];
    
    return (HMDRV_ADDR)pb;
}





/* *****************************************************************************
 * I2CMEM_vProcessCmd
 * 
 * Description: For Testing Usage In main Loop
 *              
 **************************************************************************** */
void I2CMEM_vProcessCmd(void)
{

    
    switch (eI2CCmd)
    {
        case I2C_CMD_WR:   
            
            I2CMEM_vWrite(eI2CCtrlByte, u16I2CAddress, pI2CBufferWR[u8I2CModule], u16I2CSizeWR, 1, u8I2CModule);
            break;
            
        case I2C_CMD_RD:       
            I2CMEM_vRead(eI2CCtrlByte, u16I2CAddress, pI2CBufferRD[u8I2CModule], u16I2CSizeRD, 1, u8I2CModule);
            break;
            
        case I2C_CMD_RTCC_WR:
            I2CMEM_vWrite(I2C_CTRL_BYTE_RTCC_SRAM_WR, u16I2CAddress, (EDS_SPACE uint8_t*)&RTCC + u16I2CAddress, u16I2CSizeWR, 1, u8I2CModule);
            break;
            
        case I2C_CMD_RTCC_RD:
            I2CMEM_vRead(I2C_CTRL_BYTE_RTCC_SRAM_RD, u16I2CAddress, (EDS_SPACE uint8_t*)&RTCC + u16I2CAddress, u16I2CSizeRD, 1, u8I2CModule);
            break;
            
        case I2C_CMD_SRAM_WR:
            I2CMEM_vWrite(I2C_CTRL_BYTE_RTCC_SRAM_WR, u16I2CAddress + EXT_MEM_RTCC_SRAM_ADDRESS_OFFSET, (EDS_SPACE uint8_t*)&SRAM_RTCC + u16I2CAddress, u16I2CSizeWR, 1, u8I2CModule);
            break;
            
        case I2C_CMD_SRAM_RD:
            I2CMEM_vRead(I2C_CTRL_BYTE_RTCC_SRAM_RD, u16I2CAddress + EXT_MEM_RTCC_SRAM_ADDRESS_OFFSET, (EDS_SPACE uint8_t*)&SRAM_RTCC + u16I2CAddress, u16I2CSizeRD, 1, u8I2CModule);
            break;
            
        case I2C_CMD_EEPROM_WR:
            I2CMEM_vWrite(I2C_CTRL_BYTE_EEPROM_WR, u16I2CAddress, (EDS_SPACE uint8_t*)&EEPROM_RTCC + u16I2CAddress, u16I2CSizeWR, 1, u8I2CModule);
            break;
            
        case I2C_CMD_EEPROM_RD:
            I2CMEM_vRead(I2C_CTRL_BYTE_EEPROM_RD, u16I2CAddress, (EDS_SPACE uint8_t*)&EEPROM_RTCC + u16I2CAddress, u16I2CSizeRD, 1, u8I2CModule);
            break;
            
        case I2C_CMD_PICAUX_WR:
            I2CMEM_vWrite(I2C_CTRL_BYTE_PICAUX_WR, u16I2CAddress, (EDS_SPACE uint8_t*)&PICAUX + u16I2CAddress, u16I2CSizeWR, 1, u8I2CModule);
            break;
            
        case I2C_CMD_PICAUX_RD:
            I2CMEM_vRead(I2C_CTRL_BYTE_PICAUX_RD, u16I2CAddress, (EDS_SPACE uint8_t*)&PICAUX + u16I2CAddress, u16I2CSizeRD, 1, u8I2CModule);
            break;

        #if USE_SPI_FLIR
        #ifndef BOARDCFG_USE_MCC_I2C   /* check if rework need for MCC */  
        case I2C_CMD_FLIR_WR:
            I2CMEM_vWrite(I2C_CTRL_BYTE_FLIR_WR, u16I2CAddress, (uint8_t*)&FLIRRegisters + u16I2CAddress, u16I2CSizeWR, 2, u8I2CModule);
            break;
            
        case I2C_CMD_FLIR_RD:
            I2CMEM_vRead(I2C_CTRL_BYTE_FLIR_RD, u16I2CAddress, (uint8_t*)&FLIRRegisters + u16I2CAddress, u16I2CSizeRD, 2, u8I2CModule);
            break;
          
        case I2C_CMD_FLIR_CCI_TWI_WR:
            eI2CCmd = I2C_CMD_BUSY;  
            I2CMEM_vFlirWrite(u16FlirI2CCommandReq, (uint8_t*)&FLIRData, u16FlirI2CSizeReq, u8I2CModule);
            break;
            
        case I2C_CMD_FLIR_CCI_TWI_RD:
            eI2CCmd = I2C_CMD_BUSY;  
            I2CMEM_vFlirRead(u16FlirI2CCommandReq, (uint8_t*)&FLIRData, u16FlirI2CSizeReq, u8I2CModule);
            break;
        case I2C_CMD_FLIR_CCI_TWI_RUN:
            eI2CCmd = I2C_CMD_BUSY;  
            I2CMEM_vFlirRun(u16FlirI2CCommandReq, u8I2CModule);
            break;
        #endif
        #endif  
            
        default:
            
            break;
    }

    #ifdef BOARDCFG_USE_MCC_I2C   
    eI2CCmd = I2C_CMD_IDLE;  
    #endif
}

/* *****************************************************************************
 * I2CMEM_vExtMemRamFromIntRamCopyData
 * 
 * Description: Copy Data to external memory from already filled internal RAM buffer
 **************************************************************************** */
void I2CMEM_vExtMemRamFromIntRamCopyData(HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_U16 addr = (HMDRV_U16)(HMDRV_ADDR_SIZE) nDestAddr;
    HMDRV_SIZE8 size = nSize;
    HMDRV_SIZE8 addrSize = 1;
    eI2CCtrlByte_t eI2CCtrlByteRTC;
    HMDRV_SIZE8 I2CModule = u8I2CModule;
    #ifdef BOARDCFG_USE_MCC_I2C    
    //I2C_MESSAGE_STATUS u8StatusI2C;
    #endif
    
    
    /* FLIR */
    if (addr >= EXT_MEM_I2C_ADDR_OFFSET_FLIR)  
    {
        addr -= EXT_MEM_I2C_ADDR_OFFSET_FLIR;
        eI2CCtrlByteRTC = I2C_CTRL_BYTE_FLIR;
        addrSize = 2;
        #ifdef EXT_MEM_I2C_MODULE_FLIR
        I2CModule = EXT_MEM_I2C_MODULE_FLIR;
        #endif
    }
    /* PICAUX */
    else if (addr >= EXT_MEM_I2C_ADDR_OFFSET_PICAUX)  
    {
        addr -= EXT_MEM_I2C_ADDR_OFFSET_PICAUX;
        eI2CCtrlByteRTC = I2C_CTRL_BYTE_PICAUX;
        #ifdef EXT_MEM_I2C_MODULE_PICAUX
        I2CModule = EXT_MEM_I2C_MODULE_PICAUX;
        #endif
    }
    /* RTC EERPOM */
    else if (addr >= EXT_MEM_I2C_ADDR_OFFSET_RTC_EEPROM)  
    {
        addr -= EXT_MEM_I2C_ADDR_OFFSET_RTC_EEPROM;
        eI2CCtrlByteRTC = I2C_CTRL_BYTE_EEPROM;
        #ifdef EXT_MEM_I2C_MODULE_RTC
        I2CModule = EXT_MEM_I2C_MODULE_RTC;
        #endif
    }
    /* RTC SRAM */
    else
    {
        eI2CCtrlByteRTC = I2C_CTRL_BYTE_RTCC_SRAM;
        #ifdef EXT_MEM_I2C_MODULE_RTC
        I2CModule = EXT_MEM_I2C_MODULE_RTC;
        #endif
    }

    //while(nSize--)
    //    *pb++ = *ps++;
    
    #ifndef BOARDCFG_USE_MCC_I2C    
    while(eI2CCmd == I2C_CMD_BUSY){;} /*  wait pending transfer */
    I2CMEM_vWrite(eI2CCtrlByteRTC, addr, pI2CBufferWR[I2CModule] , size, addrSize, I2CModule);
    #else
    I2CMEM_vWrite(eI2CCtrlByteRTC, addr, pI2CBufferWR[I2CModule] , size, addrSize, I2CModule);
    //I2CMEM_vWrite(eI2CCtrlByteRTC, addr, pI2CBufferWR[I2CModule] , size, addrSize, I2CModule, &u8StatusI2C);
    #endif
}


#ifndef BOARDCFG_USE_MCC_I2C    
eI2CCmd_t I2CMEM_bGetCmd(eI2CCmd_t eI2CCmdInput)
{
    
    eI2CCmd_t state = I2C_CMD_BUSY;
    state = eI2CCmdInput;
    return state;
}

bool I2CMEM_bGetCmdStateIdle(void)
{
    bool result = false;
    if (eI2CCmdState == I2C_CMD_STA_IDLE)
    {
        result = true;
    }
    if (eI2CCmd == I2C_CMD_IDLE)
    {
        result = true;
    }
    return result;
}
#endif



/* *****************************************************************************
 * The Mid Level Read-Write Functions - everything goes trough here
 **************************************************************************** */



/* *****************************************************************************
 * I2CMEM_vWrite
 * 
 * Inputs:
 *  uint16_t u16Address - make this 32 bits to support more than 2 bytes address with MCC
 * 
 * Description: Write To I2C
 *              
 **************************************************************************** */

#ifndef BOARDCFG_USE_MCC_I2C    
void I2CMEM_vWrite(eI2CCtrlByte_t eI2CCtrlByteLocal, uint16_t u16Address, EDS_SPACE uint8_t* pdData, uint16_t u16Size, uint8_t u8AddressSize, uint8_t u8I2CModule)
{
    if (eI2CCmdState == I2C_CMD_STA_IDLE)
    {
        eI2CCmd = I2C_CMD_BUSY;  
        eI2CCmdCtrlByte = eI2CCtrlByteLocal & (~(0x0001));   /* write command */
        u16I2CCmdAddress = u16Address;
        pI2CCmdBuffer = pdData;
        u16I2CCmdSize = u16Size;
        u8I2CAddrSize = u8AddressSize;
        eI2CCmdCommand = I2C_CMD_WR;
        
        rtrycntr = 0;
        cntr = 0;
        
        if (u8I2CModule == 0)
        {
            #if USE_I2C_ISR == 1
            // Start Condition
            eI2CCmdState = I2C_CMD_STA_START_WAIT;
            I2C1CONbits.SEN = 1;
            #else
            eI2CCmdState = I2C_CMD_STA_START;
            #endif
        }
        else
        {
            #if USE_I2C_ISR == 1
            // Start Condition
            eI2CCmdState = I2C_CMD_STA_START_WAIT;
            I2C2CONbits.SEN = 1;
            #else
            eI2CCmdState = I2C_CMD_STA_START;
            #endif
        }
        
    }

}
#else
void I2CMEM_vWrite(eI2CCtrlByte_t eI2CCtrlByteLocal, uint16_t u16Address, EDS_SPACE uint8_t* pdData, uint16_t u16Size, uint8_t u8AddressSize, uint8_t u8I2CModule)
{
    
    uint8_t index;
    
    index = 0;

    while (u8AddressSize > 0)
    {
        u8AddressSize--;
        pu8I2CBufferMCCUse[index++] = u16Address;
        u16Address >>= 8;
    }
    while (u16Size > 0)
    {
        u16Size--;
        pu8I2CBufferMCCUse[index++] = *pdData++;
    }
    if (u8I2CModule == 0)
    {
        while (I2C1_MasterQueueIsFull()){;}
        I2C1_MasterWrite(pu8I2CBufferMCCUse, index, 
                (eI2CCtrlByteLocal >> 1), (I2C1_MESSAGE_STATUS*)pu8StatusTransaction); 
    }
    else
    {
        while (I2C2_MasterQueueIsFull()){;}
        I2C2_MasterWrite(pu8I2CBufferMCCUse, index, 
                (eI2CCtrlByteLocal >> 1), (I2C2_MESSAGE_STATUS*)pu8StatusTransaction); 
    }

    
}
#endif



/* *****************************************************************************
 * I2CMEM_vRead
 * 
 * Description: Read From I2C
 *              
 **************************************************************************** */
#ifndef BOARDCFG_USE_MCC_I2C    
void I2CMEM_vRead(eI2CCtrlByte_t eI2CCtrlByteLocal, uint16_t u16Address, EDS_SPACE uint8_t* pdData, uint16_t u16Size, uint8_t u8AddressSize, uint8_t u8I2CModule)
{
    if (eI2CCmdState == I2C_CMD_STA_IDLE)
    {
        eI2CCmd = I2C_CMD_BUSY;  
        eI2CCmdCtrlByte = eI2CCtrlByteLocal | (0x0001);
        u16I2CCmdAddress = u16Address;
        pI2CCmdBuffer = pdData;
        u16I2CCmdSize = u16Size;
        u8I2CAddrSize = u8AddressSize;
        eI2CCmdCommand = I2C_CMD_RD;

        
        rtrycntr = 0;
        cntr = 0;
        
        
        if (u8I2CModule == 0)
        {
            #if USE_I2C_ISR == 1
            // Start Condition
            eI2CCmdState = I2C_CMD_STA_START_WAIT;
            I2C1CONbits.SEN = 1;
            #else
            eI2CCmdState = I2C_CMD_STA_START;
            #endif
        }
        else
        {
            #if USE_I2C_ISR == 1
            // Start Condition
            eI2CCmdState = I2C_CMD_STA_START_WAIT;
            I2C2CONbits.SEN = 1;
            #else
            eI2CCmdState = I2C_CMD_STA_START;
            #endif
        }
        
    }

    
}
#else



void vCopyFromI2CBuffer(uint16_t u16Size, EDS_SPACE uint8_t* pdData)
{
    uint16_t u16Index;
    for (u16Index = 0; u16Index < u16Size; u16Index++)
    {
        *pdData++ = pu8I2CBufferMCCUse[u16Index];
    }
}


void I2CMEM_vRead(eI2CCtrlByte_t eI2CCtrlByteLocal, uint16_t u16Address, EDS_SPACE uint8_t* pdData, uint16_t u16Size, uint8_t u8AddressSize, uint8_t u8I2CModule)
{
    uint16_t index;
    uint16_t size = u16Size;
    
    
    index = 0;

    while (u8AddressSize > 0)
    {
        u8AddressSize--;
        pu8I2CBufferMCCUse[index++] = u16Address;
        u16Address >>= 8;
    }
    if (u8I2CModule == 0)
    {
        do
        {
            Nop();
        }
        while (I2C1_MasterQueueIsFull());
        I2C1_MasterWrite(pu8I2CBufferMCCUse, index, (eI2CCtrlByteLocal >> 1), (I2C1_MESSAGE_STATUS*)pu8StatusTransaction); 
        do
        {
            Nop();
        }
        while (I2C1_MasterQueueIsFull());
        I2C1_MasterRead(pu8I2CBufferMCCUse, size, (eI2CCtrlByteLocal >> 1), (I2C1_MESSAGE_STATUS*)pu8StatusTransaction); /* Select line above if the whole function returns the status instead of this line and the line below */ 
    }
    else
    {
        do
        {
            Nop();
        }
        while (I2C2_MasterQueueIsFull());
        I2C2_MasterWrite(pu8I2CBufferMCCUse, index, (eI2CCtrlByteLocal >> 1), (I2C2_MESSAGE_STATUS*)pu8StatusTransaction); 
        do
        {
            Nop();
        }
        while (I2C2_MasterQueueIsFull());
        I2C2_MasterRead(pu8I2CBufferMCCUse, size, (eI2CCtrlByteLocal >> 1), (I2C2_MESSAGE_STATUS*)pu8StatusTransaction); /* Select line above if the whole function returns the status instead of this line and the line below */
    }
    
    do
    {
        Nop();
    }
    while (*pu8StatusTransaction == I2C_MESSAGE_PENDING);
    vCopyFromI2CBuffer(size, pdData);  
    
} 




#endif





#ifndef BOARDCFG_USE_MCC_I2C

/******************************************************************************
 * Function:        void I2CEMEMinit(I2CEMEM_DRV *i2cMem)
 *
 * PreCondition:    None
 *
 * Input:           *i2cMem
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        I2C initialization/configuration function.
 *                  This function is used to configure the I2C module as the
 *                  master and use 8-bit mode for communication
 *                  with the serial EEPROM.
 *****************************************************************************/
void I2CMEM_vInit(uint32_t u32Fcy)
{
#if  USE_I2C1
    // Configre SCA/SDA pin as open-drain. This may change from device to device.
    //Refer the datasheet for more information.
    I2C1_SDA_SetOpenDrain();
    I2C1_SCL_SetOpenDrain();

    I2C1CONbits.A10M = 0;
    I2C1CONbits.SCLREL = 1;
    I2C1BRG = (u32Fcy / u32Fi2c - u32Fcy / u32Fi2cDelay) - 2;  /* (1/Fi2c - Delay)*Fcy - 2 */

    I2C1ADD = 0;
    I2C1MSK = 0;

    I2C1CONbits.I2CEN = 1;
    IEC1bits.MI2C1IE = 1;
    IPC4bits.MI2C1IP = ISR_PRIORITY_I2C1;
    IFS1bits.MI2C1IF = 0;
#endif
    
#if  USE_I2C2
    // Configre SCA/SDA pin as open-drain. This may change from device to device.
    //Refer the datasheet for more information.
    I2C2_SCL_SetOpenDrain();
    I2C2_SDA_SetOpenDrain();

    I2C2CONbits.A10M = 0;
    I2C2CONbits.SCLREL = 1;
    I2C2BRG = (u32Fcy / u32Fi2c - u32Fcy / u32Fi2cDelay) - 2;  /* (1/Fi2c - Delay)*Fcy - 2 */

    I2C2ADD = 0;
    I2C2MSK = 0;

    I2C2CONbits.I2CEN = 1;
    IEC3bits.MI2C2IE = 1;
    IPC12bits.MI2C2IP = ISR_PRIORITY_I2C2;
    IFS3bits.MI2C2IF = 0;
#endif
}


/******************************************************************************
 * I2C Driver Related Functions
 *****************************************************************************/




/******************************************************************************
 * Function:  void __attribute__((interrupt, auto_psv)) _MI2C1Interrupt(void)
 * 
 * Overview:        This serves the I2C Master Interrupt Service Routine.
 *****************************************************************************/
void __attribute__ ( (interrupt, auto_psv) ) _MI2C1Interrupt( void )
{
    IFS1bits.MI2C1IF = 0;   //Clear the Interrupt Flag;
    jDone = 1;
    
    I2CMEM_vProcessRWISR(); /* Inside eI2CCmdState = I2C_CMD_STA_IDLE on Transfer Complete */
    
#if USE_SPI_FLIR
    /* Process Flir Protocol */
    if (eI2CCmdState == I2C_CMD_STA_IDLE)
    {
        I2CMEM_vFlirProcessCmd();
    }
#endif        
}

#if 0       /* Slave I2C Not Used */
/******************************************************************************
 * Function:  void __attribute__((interrupt, auto_psv)) _SI2C1Interrupt(void)
 *
 * Overview:        This serves the I2C Slave interrupt.
 *****************************************************************************/
void __attribute__ ( (interrupt, auto_psv) ) _SI2C1Interrupt( void )
{
    IFS1bits.SI2C1IF = 0;   //Clear the Interrupt Flag
}
#endif


/******************************************************************************
 * Function:  void __attribute__((interrupt, auto_psv)) _MI2C2Interrupt(void)
 *
 * Overview:        This serves the I2C Master Interrupt Service Routine.
 *****************************************************************************/
void __attribute__ ( (interrupt, auto_psv) ) _MI2C2Interrupt( void )
{
    IFS3bits.MI2C2IF = 0;   //Clear the Interrupt Flag;
    jDone = 1;
    I2CMEM_vProcessRWISR2();
    
#if USE_SPI_FLIR
    /* Process Flir Protocol */
    if (eI2CCmdState == I2C_CMD_STA_IDLE)
    {
        I2CMEM_vFlirProcessCmd();
    }
#endif
}

#if 0       /* Slave I2C Not Used */
/******************************************************************************
 * Function:  void __attribute__((interrupt, auto_psv)) _SI2C2Interrupt(void)
 *
 * Overview:        This serves the I2C Slave interrupt.
 *****************************************************************************/
void __attribute__ ( (interrupt, auto_psv) ) _SI2C2Interrupt( void )
{
    IFS3bits.SI2C2IF = 0;   //Clear the Interrupt Flag
}
#endif











            




#if USE_I2C_ISR /* ISR or Non-ISR I2C Process Loop Driver Functions */
/******************************************************************************
 * I2C ISR Related Functions
 *****************************************************************************/


void I2CMEM_vProcessRW(void)
{
    /* Dummy Function - Pooling not made in main loop - used ISR */
}

void I2CMEM_vProcessRWISR2(void)
{    
    switch (eI2CCmdState)
    {
        /*==================================*/
        /* Control/Address Phase            */
        /*==================================*/
        
//        case I2C_CMD_STA_START:
//            // Start Condition
//            I2C2CONbits.SEN = 1;
//            eI2CCmdState = I2C_CMD_STA_START_WAIT;
//            break;
        case I2C_CMD_STA_START_WAIT:
            // Start (Control) Byte with device select id
            if( jDone == 1 )
            {
                jDone = 0;
                //    I2C2TRN=(0x00A0)|(((i2cMem->oData->csel)&0x7)<<1);
                I2C2TRN = eI2CCmdCtrlByte & (~(0x0001));    /* Always Write command for Address */
                eI2CCmdState = I2C_CMD_STA_SEND_CTRL;
            }
            break;
       case I2C_CMD_STA_SEND_CTRL:
            // Send address byte 1, if ack is received. Else Retry
            if( jDone == 1 )
            {
                jDone = 0;

                if( I2C2STATbits.ACKSTAT == 1 )
                {                   // Ack Not received, Retry
                    if( rtrycntr < I2C_MAX_RETRY )
                    {
                        //eI2CCmdState = I2C_CMD_STA_RETRY;
                        I2C2CONbits.PEN = 1;
                        eI2CCmdState = I2C_CMD_STA_RETRY_END;
                        rtrycntr++;
                    }
                    else
                    {
                        //eI2CCmdState = I2C_CMD_STA_ERROR; // Flag error and exit
                        I2C2CONbits.PEN = 1;
                        eI2CCmdState = I2C_CMD_STA_ERROR_END;
                    }
                }
                else
                {
                    rtrycntr = 0;
                    if (u8I2CAddrSize == 2)
                    {
                    //#if I2C_ADDRWIDTH == TWO_BYTE
                        I2C2TRN = (u16I2CCmdAddress)  >> 8;
                        eI2CCmdState = I2C_CMD_STA_SEND_ADDRESS_HI;
                    //#endif
                    }
                    else /* if (u8I2CAddrSize == 1) */
                    {
                    //#if I2C_ADDRWIDTH == ONE_BYTE
                        I2C2TRN = (u16I2CCmdAddress);
                        eI2CCmdState = I2C_CMD_STA_SEND_ADDRESS_LO;
                    //#endif
                    }
                }
            }
            break;
            
         case I2C_CMD_STA_SEND_ADDRESS_HI:
            // Send address byte 2, if ack is received. Else Flag error and exit
            if( jDone == 1 )
            {
                jDone = 0;

                if( I2C2STATbits.ACKSTAT == 1 )
                {                   // Ack Not received, Flag error and exit
                    //eI2CCmdState = I2C_CMD_STA_ERROR;
                    I2C2CONbits.PEN = 1;
                    eI2CCmdState = I2C_CMD_STA_ERROR_END;
                }
                else
                {
                    if (u8I2CAddrSize == 2)
                    {
                    //#if I2C_ADDRWIDTH == TWO_BYTE
                        I2C2TRN = ( (u16I2CCmdAddress) & 0x00FF );
                    //#endif
                    }
                    eI2CCmdState = I2C_CMD_STA_SEND_ADDRESS_LO;
                }
            }

            break;

        case I2C_CMD_STA_SEND_ADDRESS_LO:
            // Read or Write
            if( jDone == 1 )
            {
                jDone = 0;

                if( I2C2STATbits.ACKSTAT == 1 )
                {                   // Ack Not received, Flag error and exit
                    //eI2CCmdState = I2C_CMD_STA_ERROR;
                    I2C2CONbits.PEN = 1;
                    eI2CCmdState = I2C_CMD_STA_ERROR_END;
                }
                else
                {
                    if( eI2CCmdCommand == I2C_CMD_WR )
                    {
                        //eI2CCmdState = I2C_CMD_STA_SEND_DATA;
                        
                        if (u8I2CAddrSize == 2) /* 16-bit data MSB First */
                        {
                            if ((cntr & 1) == 0)
                            {
                                I2C2TRN = *( pI2CCmdBuffer + cntr + 1 );
                            }
                            else
                            {
                                I2C2TRN = *( pI2CCmdBuffer + cntr - 1 );
                            }
                        } 
                        else /* if (u8I2CAddrSize == 1) */
                        {
                            I2C2TRN = *( pI2CCmdBuffer + cntr );
                        }
                        
                        eI2CCmdState = I2C_CMD_STA_SEND_DATA_ACK;
                        cntr = cntr + 1;
                    }

                    if( eI2CCmdCommand == I2C_CMD_RD )
                    {
                        //eI2CCmdState = I2C_CMD_STA_READ_DATA_START;
                        // Repeat Start
                        I2C2CONbits.RSEN = 1;
                        eI2CCmdState = I2C_CMD_STA_READ_DATA_CONTROL;
                    }
                }
            }

            break;

        /*==================================*/
        /* Write Data Phase                    */
        /*==================================*/
//        case I2C_CMD_STA_SEND_DATA:
//            // Send data
//            I2C2TRN = *( pI2CCmdBuffer + cntr );
//            eI2CCmdState = I2C_CMD_STA_SEND_DATA_ACK;
//            cntr = cntr + 1;
//            break;

        case I2C_CMD_STA_SEND_DATA_ACK:
            // Look for end of data or no Ack
            if( jDone == 1 )
            {
                jDone = 0;
                

                if( I2C2STATbits.ACKSTAT == 1 )
                {                   // Ack Not received, Flag error and exit
                    //eI2CCmdState = I2C_CMD_STA_ERROR;
                    I2C2CONbits.PEN = 1;
                    eI2CCmdState = I2C_CMD_STA_ERROR_END;
                }
                else
                {
                    if( cntr == u16I2CCmdSize )
                    {  
                        if (u8UseAckPoolingWaitWriteComplete)
                        {
                            //eI2CCmdState = I2C_CMD_STA_ACK_POOLING; // Go to ACK polling to wait for write to complete - check when used
                            int acktest;

                            I2C2CONbits.PEN = 1;
                            while( !_MI2C2IF );
                            while( _MI2C2IF );
                            acktest = 1;
                            while( acktest )
                            {
                                I2C2CONbits.RSEN = 1;
                                while( !_MI2C2IF );
                                while( _MI2C2IF );
                                I2C2TRN = 0x00A0;
                                while( _TRSTAT );
                                acktest = I2C2STATbits.ACKSTAT;
                            }

                            I2C2CONbits.PEN = 1;
                            while( !_MI2C2IF );
                            while( _MI2C2IF );
                            jDone = 0;
                            //eI2CCmdState = I2C_CMD_STA_STOP;
                            I2C2CONbits.PEN = 1;
                            eI2CCmdState = I2C_CMD_STA_STOP_END;
                        }
                        else
                        {
                            //eI2CCmdState = I2C_CMD_STA_STOP;   // Close the Frame without ACK polling
                            I2C2CONbits.PEN = 1;
                            eI2CCmdState = I2C_CMD_STA_STOP_END;
                        }
                        
                    }
                    else
                    {
                        //eI2CCmdState = I2C_CMD_STA_SEND_DATA;
                        if (u8I2CAddrSize == 2) /* 16-bit data MSB First */
                        {
                            if ((cntr & 1) == 0)
                            {
                                I2C2TRN = *( pI2CCmdBuffer + cntr + 1 );
                            }
                            else
                            {
                                I2C2TRN = *( pI2CCmdBuffer + cntr - 1 );
                            }
                        } 
                        else /* if (u8I2CAddrSize == 1) */
                        {
                            I2C2TRN = *( pI2CCmdBuffer + cntr );
                        }
                        eI2CCmdState = I2C_CMD_STA_SEND_DATA_ACK;//not needed state not changed
                        cntr = cntr + 1;
                    }
                }
            }

            break;
            
            
            
            
        /*==================================*/
        /* Read Data Phase                    */
        /*==================================*/
//        case I2C_CMD_STA_READ_DATA_START:
//            // Repeat Start
//            I2C2CONbits.RSEN = 1;
//            eI2CCmdState = I2C_CMD_STA_READ_DATA_CONTROL;
//            break;

        case I2C_CMD_STA_READ_DATA_CONTROL:
            // Re-send control byte with W/R=R
            if( jDone == 1 )
            {
                jDone = 0;
                eI2CCmdState = I2C_CMD_STA_READ_DATA_CONTROL_ACK;
                I2C2TRN = eI2CCmdCtrlByte | (0x0001);   /* Always read data request here */
            }
            break;

        case I2C_CMD_STA_READ_DATA_CONTROL_ACK:
            // Check, if control byte went ok
            if( jDone == 1 )
            {
                jDone = 0;

                if( I2C2STATbits.ACKSTAT == 1 )
                {                   // Ack Not received, Flag error and exit
                    //eI2CCmdState = I2C_CMD_STA_ERROR;
                    I2C2CONbits.PEN = 1;
                    eI2CCmdState = I2C_CMD_STA_ERROR_END;
                }
                else
                {
                    //eI2CCmdState = I2C_CMD_STA_READ_DATA_RECEIVE;
                    I2C2CONbits.RCEN = 1;
                    eI2CCmdState = I2C_CMD_STA_READ_DATA_RECEIVE_ACK;                    
                }
            }
            break;

//        case I2C_CMD_STA_READ_DATA_RECEIVE:
//            // Receive Enable
//            I2C2CONbits.RCEN = 1;
//            eI2CCmdState = I2C_CMD_STA_READ_DATA_RECEIVE_ACK;
//            break;

        case I2C_CMD_STA_READ_DATA_RECEIVE_ACK:
            // Receive data
            if( jDone == 1 )
            {
                jDone = 0;
                eI2CCmdState = I2C_CMD_STA_READ_DATA_RECEIVE_END;

                if (u8I2CAddrSize == 2) /* 16-bit data MSB First */
                {
                    if ((cntr & 1) == 0)
                    {
                        *( pI2CCmdBuffer + cntr + 1) = I2C2RCV;
                    }
                    else
                    {
                        *( pI2CCmdBuffer + cntr - 1) = I2C2RCV;
                    }
                } 
                else /* if (u8I2CAddrSize == 1) */
                {
                    *( pI2CCmdBuffer + cntr ) = I2C2RCV;
                }
                
                cntr++;

                if( cntr == u16I2CCmdSize )
                {
                    I2C2CONbits.ACKDT = 1;  // No ACK
                }
                else
                {
                    I2C2CONbits.ACKDT = 0;  // ACK
                }

                I2C2CONbits.ACKEN = 1;
            }
            break;

        case I2C_CMD_STA_READ_DATA_RECEIVE_END:
            if( jDone == 1 )
            {
                jDone = 0;
                if( cntr == u16I2CCmdSize )
                {
                    //eI2CCmdState = I2C_CMD_STA_STOP;
                    I2C2CONbits.PEN = 1;
                    eI2CCmdState = I2C_CMD_STA_STOP_END;
                }
                else
                {
                    //eI2CCmdState = I2C_CMD_STA_READ_DATA_RECEIVE;
                    I2C2CONbits.RCEN = 1;
                    eI2CCmdState = I2C_CMD_STA_READ_DATA_RECEIVE_ACK;
                }
            }
            break;

            
        /*==================================*/
        /* Stop Sequence                    */
        /*==================================*/
//        case I2C_CMD_STA_STOP:
//            I2C2CONbits.PEN = 1;
//            eI2CCmdState = I2C_CMD_STA_STOP_END;
//            break;

        case I2C_CMD_STA_STOP_END:
            if( jDone == 1 )
            {
                jDone = 0;
                eI2CCmd = I2C_CMD_IDLE;
                eI2CCmdState = I2C_CMD_STA_IDLE;
                cntr = 0;
                eI2CCmdCommand = I2C_CMD_IDLE;
            }

            break;

        /*==================================*/
        /* Set Error                         */
        /*==================================*/
//        case I2C_CMD_STA_ERROR:
//            I2C2CONbits.PEN = 1;
//            eI2CCmdState = I2C_CMD_STA_ERROR_END;
//            break;

        case I2C_CMD_STA_ERROR_END:
            if( jDone == 1 )
            {
                jDone = 0;
                eI2CCmd = I2C_CMD_IDLE;
                eI2CCmdState = I2C_CMD_STA_IDLE;
                rtrycntr = 0;
                cntr = 0;
                eI2CCmdCommand = 0xFFFF;
            }

            break;

        /*==================================*/
        /* Retry                             */
        /*==================================*/
//        case I2C_CMD_STA_RETRY:
//            I2C2CONbits.PEN = 1;
//            eI2CCmdState = I2C_CMD_STA_RETRY_END;
//            rtrycntr++;
//            break;

        case I2C_CMD_STA_RETRY_END:
            if( jDone == 1 )
            {
                jDone = 0;
                eI2CCmdState = I2C_CMD_STA_START_WAIT;
                I2C2CONbits.SEN = 1;
                cntr = 0;
            }

            break;

            /*==================================*/
            /* ACK Polling                        */
            /*==================================*/
//        case I2C_CMD_STA_ACK_POOLING:
//            int acktest;
//
//            I2C2CONbits.PEN = 1;
//            while( !_MI2C2IF );
//            while( _MI2C2IF );
//            acktest = 1;
//            while( acktest )
//            {
//                I2C2CONbits.RSEN = 1;
//                while( !_MI2C2IF );
//                while( _MI2C2IF );
//                I2C2TRN = 0x00A0;
//                while( _TRSTAT );
//                acktest = I2C2STATbits.ACKSTAT;
//            }
//
//            I2C2CONbits.PEN = 1;
//            while( !_MI2C2IF );
//            while( _MI2C2IF );
//            jDone = 0;
//            //eI2CCmdState = I2C_CMD_STA_STOP;
//            I2C2CONbits.PEN = 1;
//            eI2CCmdState = I2C_CMD_STA_STOP_END;
//            break;
            
            
//       case I2C_CMD_STA_END:
//            eI2CCmd = I2C_CMD_IDLE;
//            eI2CCmdState = I2C_CMD_STA_IDLE;
//            break;
            
//        case I2C_CMD_STA_IDLE:
//            break;
        default:
            eI2CCmd = I2C_CMD_IDLE;
            eI2CCmdState = I2C_CMD_STA_IDLE;
            break;
    }
}

void I2CMEM_vProcessRWISR(void)
{    
    switch (eI2CCmdState)
    {
        /*==================================*/
        /* Control/Address Phase            */
        /*==================================*/
        
//        case I2C_CMD_STA_START:
//            // Start Condition
//            I2C1CONbits.SEN = 1;
//            eI2CCmdState = I2C_CMD_STA_START_WAIT;
//            break;
        case I2C_CMD_STA_START_WAIT:
            // Start (Control) Byte with device select id
            if( jDone == 1 )
            {
                jDone = 0;
                //    I2C1TRN=(0x00A0)|(((i2cMem->oData->csel)&0x7)<<1);
                I2C1TRN = eI2CCmdCtrlByte & (~(0x0001));    /* Always Write command for Address */
                eI2CCmdState = I2C_CMD_STA_SEND_CTRL;
            }
            break;
       case I2C_CMD_STA_SEND_CTRL:
            // Send address byte 1, if ack is received. Else Retry
            if( jDone == 1 )
            {
                jDone = 0;

                if( I2C1STATbits.ACKSTAT == 1 )
                {                   // Ack Not received, Retry
                    if( rtrycntr < I2C_MAX_RETRY )
                    {
                        //eI2CCmdState = I2C_CMD_STA_RETRY;
                        I2C1CONbits.PEN = 1;
                        eI2CCmdState = I2C_CMD_STA_RETRY_END;
                        rtrycntr++;
                    }
                    else
                    {
                        //eI2CCmdState = I2C_CMD_STA_ERROR; // Flag error and exit
                        I2C1CONbits.PEN = 1;
                        eI2CCmdState = I2C_CMD_STA_ERROR_END;
                    }
                }
                else
                {
                    rtrycntr = 0;
                    if (u8I2CAddrSize == 2)
                    {
                    //#if I2C_ADDRWIDTH == TWO_BYTE
                        I2C1TRN = (u16I2CCmdAddress)  >> 8;
                        eI2CCmdState = I2C_CMD_STA_SEND_ADDRESS_HI;
                    //#endif
                    }
                    else /* if (u8I2CAddrSize == 1) */
                    {
                    //#if I2C_ADDRWIDTH == ONE_BYTE
                        I2C1TRN = (u16I2CCmdAddress);
                        eI2CCmdState = I2C_CMD_STA_SEND_ADDRESS_LO;
                    //#endif
                    }
                }
            }
            break;
            
         case I2C_CMD_STA_SEND_ADDRESS_HI:
            // Send address byte 2, if ack is received. Else Flag error and exit
            if( jDone == 1 )
            {
                jDone = 0;

                if( I2C1STATbits.ACKSTAT == 1 )
                {                   // Ack Not received, Flag error and exit
                    //eI2CCmdState = I2C_CMD_STA_ERROR;
                    I2C1CONbits.PEN = 1;
                    eI2CCmdState = I2C_CMD_STA_ERROR_END;
                }
                else
                {
                    if (u8I2CAddrSize == 2)
                    {
                    //#if I2C_ADDRWIDTH == TWO_BYTE
                        I2C1TRN = ( (u16I2CCmdAddress) & 0x00FF );
                    //#endif
                    }
                    eI2CCmdState = I2C_CMD_STA_SEND_ADDRESS_LO;
                }
            }

            break;

        case I2C_CMD_STA_SEND_ADDRESS_LO:
            // Read or Write
            if( jDone == 1 )
            {
                jDone = 0;

                if( I2C1STATbits.ACKSTAT == 1 )
                {                   // Ack Not received, Flag error and exit
                    //eI2CCmdState = I2C_CMD_STA_ERROR;
                    I2C1CONbits.PEN = 1;
                    eI2CCmdState = I2C_CMD_STA_ERROR_END;
                }
                else
                {
                    if( eI2CCmdCommand == I2C_CMD_WR )
                    {
                        //eI2CCmdState = I2C_CMD_STA_SEND_DATA;
                        if (u8I2CAddrSize == 2) /* 16-bit data MSB First */
                        {
                            if ((cntr & 1) == 0)
                            {
                                I2C2TRN = *( pI2CCmdBuffer + cntr + 1 );
                            }
                            else
                            {
                                I2C2TRN = *( pI2CCmdBuffer + cntr - 1 );
                            }
                        } 
                        else /* if (u8I2CAddrSize == 1) */
                        {
                            I2C2TRN = *( pI2CCmdBuffer + cntr );
                        }
                        eI2CCmdState = I2C_CMD_STA_SEND_DATA_ACK;
                        cntr = cntr + 1;
                    }

                    if( eI2CCmdCommand == I2C_CMD_RD )
                    {
                        //eI2CCmdState = I2C_CMD_STA_READ_DATA_START;
                        // Repeat Start
                        I2C1CONbits.RSEN = 1;
                        eI2CCmdState = I2C_CMD_STA_READ_DATA_CONTROL;
                    }
                }
            }

            break;

        /*==================================*/
        /* Write Data Phase                    */
        /*==================================*/
//        case I2C_CMD_STA_SEND_DATA:
//            // Send data
//            I2C1TRN = *( pI2CCmdBuffer + cntr );
//            eI2CCmdState = I2C_CMD_STA_SEND_DATA_ACK;
//            cntr = cntr + 1;
//            break;

        case I2C_CMD_STA_SEND_DATA_ACK:
            // Look for end of data or no Ack
            if( jDone == 1 )
            {
                jDone = 0;
                

                if( I2C1STATbits.ACKSTAT == 1 )
                {                   // Ack Not received, Flag error and exit
                    //eI2CCmdState = I2C_CMD_STA_ERROR;
                    I2C1CONbits.PEN = 1;
                    eI2CCmdState = I2C_CMD_STA_ERROR_END;
                }
                else
                {
                    if( cntr == u16I2CCmdSize )
                    {  
                        if (u8UseAckPoolingWaitWriteComplete)
                        {
                            //eI2CCmdState = I2C_CMD_STA_ACK_POOLING; // Go to ACK polling to wait for write to complete - check when used
                            int acktest;

                            I2C1CONbits.PEN = 1;
                            while( !_MI2C1IF );
                            while( _MI2C1IF );
                            acktest = 1;
                            while( acktest )
                            {
                                I2C1CONbits.RSEN = 1;
                                while( !_MI2C1IF );
                                while( _MI2C1IF );
                                I2C1TRN = 0x00A0;
                                while( _TRSTAT );
                                acktest = I2C1STATbits.ACKSTAT;
                            }

                            I2C1CONbits.PEN = 1;
                            while( !_MI2C1IF );
                            while( _MI2C1IF );
                            jDone = 0;
                            //eI2CCmdState = I2C_CMD_STA_STOP;
                            I2C1CONbits.PEN = 1;
                            eI2CCmdState = I2C_CMD_STA_STOP_END;
                        }
                        else
                        {
                            //eI2CCmdState = I2C_CMD_STA_STOP;   // Close the Frame without ACK polling
                            I2C1CONbits.PEN = 1;
                            eI2CCmdState = I2C_CMD_STA_STOP_END;
                        }
                        
                    }
                    else
                    {
                        //eI2CCmdState = I2C_CMD_STA_SEND_DATA;
                        if (u8I2CAddrSize == 2) /* 16-bit data MSB First */
                        {
                            if ((cntr & 1) == 0)
                            {
                                I2C2TRN = *( pI2CCmdBuffer + cntr + 1 );
                            }
                            else
                            {
                                I2C2TRN = *( pI2CCmdBuffer + cntr - 1 );
                            }
                        } 
                        else /* if (u8I2CAddrSize == 1) */
                        {
                            I2C2TRN = *( pI2CCmdBuffer + cntr );
                        }
                        eI2CCmdState = I2C_CMD_STA_SEND_DATA_ACK;//not needed state not changed
                        cntr = cntr + 1;
                    }
                }
            }

            break;
            
            
            
            
        /*==================================*/
        /* Read Data Phase                    */
        /*==================================*/
//        case I2C_CMD_STA_READ_DATA_START:
//            // Repeat Start
//            I2C1CONbits.RSEN = 1;
//            eI2CCmdState = I2C_CMD_STA_READ_DATA_CONTROL;
//            break;

        case I2C_CMD_STA_READ_DATA_CONTROL:
            // Re-send control byte with W/R=R
            if( jDone == 1 )
            {
                jDone = 0;
                eI2CCmdState = I2C_CMD_STA_READ_DATA_CONTROL_ACK;
                I2C1TRN = eI2CCmdCtrlByte | (0x0001);   /* Always read data request here */
            }
            break;

        case I2C_CMD_STA_READ_DATA_CONTROL_ACK:
            // Check, if control byte went ok
            if( jDone == 1 )
            {
                jDone = 0;

                if( I2C1STATbits.ACKSTAT == 1 )
                {                   // Ack Not received, Flag error and exit
                    //eI2CCmdState = I2C_CMD_STA_ERROR;
                    I2C1CONbits.PEN = 1;
                    eI2CCmdState = I2C_CMD_STA_ERROR_END;
                }
                else
                {
                    //eI2CCmdState = I2C_CMD_STA_READ_DATA_RECEIVE;
                    I2C1CONbits.RCEN = 1;
                    eI2CCmdState = I2C_CMD_STA_READ_DATA_RECEIVE_ACK;                    
                }
            }
            break;

//        case I2C_CMD_STA_READ_DATA_RECEIVE:
//            // Receive Enable
//            I2C1CONbits.RCEN = 1;
//            eI2CCmdState = I2C_CMD_STA_READ_DATA_RECEIVE_ACK;
//            break;

        case I2C_CMD_STA_READ_DATA_RECEIVE_ACK:
            // Receive data
            if( jDone == 1 )
            {
                jDone = 0;
                eI2CCmdState = I2C_CMD_STA_READ_DATA_RECEIVE_END;

                if (u8I2CAddrSize == 2) /* 16-bit data MSB First */
                {
                    if ((cntr & 1) == 0)
                    {
                        *( pI2CCmdBuffer + cntr + 1) = I2C2RCV;
                    }
                    else
                    {
                        *( pI2CCmdBuffer + cntr - 1) = I2C2RCV;
                    }
                } 
                else /* if (u8I2CAddrSize == 1) */
                {
                    *( pI2CCmdBuffer + cntr ) = I2C2RCV;
                }
                cntr++;

                if( cntr == u16I2CCmdSize )
                {
                    I2C1CONbits.ACKDT = 1;  // No ACK
                }
                else
                {
                    I2C1CONbits.ACKDT = 0;  // ACK
                }

                I2C1CONbits.ACKEN = 1;
            }
            break;

        case I2C_CMD_STA_READ_DATA_RECEIVE_END:
            if( jDone == 1 )
            {
                jDone = 0;
                if( cntr == u16I2CCmdSize )
                {
                    //eI2CCmdState = I2C_CMD_STA_STOP;
                    I2C1CONbits.PEN = 1;
                    eI2CCmdState = I2C_CMD_STA_STOP_END;
                }
                else
                {
                    //eI2CCmdState = I2C_CMD_STA_READ_DATA_RECEIVE;
                    I2C1CONbits.RCEN = 1;
                    eI2CCmdState = I2C_CMD_STA_READ_DATA_RECEIVE_ACK;
                }
            }
            break;

            
        /*==================================*/
        /* Stop Sequence                    */
        /*==================================*/
//        case I2C_CMD_STA_STOP:
//            I2C1CONbits.PEN = 1;
//            eI2CCmdState = I2C_CMD_STA_STOP_END;
//            break;

        case I2C_CMD_STA_STOP_END:
            if( jDone == 1 )
            {
                jDone = 0;
                eI2CCmd = I2C_CMD_IDLE;
                eI2CCmdState = I2C_CMD_STA_IDLE;
                cntr = 0;
                eI2CCmdCommand = I2C_CMD_IDLE;
            }

            break;

        /*==================================*/
        /* Set Error                         */
        /*==================================*/
//        case I2C_CMD_STA_ERROR:
//            I2C1CONbits.PEN = 1;
//            eI2CCmdState = I2C_CMD_STA_ERROR_END;
//            break;

        case I2C_CMD_STA_ERROR_END:
            if( jDone == 1 )
            {
                jDone = 0;
                eI2CCmd = I2C_CMD_IDLE;
                eI2CCmdState = I2C_CMD_STA_IDLE;
                rtrycntr = 0;
                cntr = 0;
                eI2CCmdCommand = 0xFFFF;
            }

            break;

        /*==================================*/
        /* Retry                             */
        /*==================================*/
//        case I2C_CMD_STA_RETRY:
//            I2C1CONbits.PEN = 1;
//            eI2CCmdState = I2C_CMD_STA_RETRY_END;
//            rtrycntr++;
//            break;

        case I2C_CMD_STA_RETRY_END:
            if( jDone == 1 )
            {
                jDone = 0;
                eI2CCmdState = I2C_CMD_STA_START_WAIT;
                I2C1CONbits.SEN = 1;
                cntr = 0;
            }

            break;

            /*==================================*/
            /* ACK Polling                        */
            /*==================================*/
//        case I2C_CMD_STA_ACK_POOLING:
//            int acktest;
//
//            I2C1CONbits.PEN = 1;
//            while( !_MI2C1IF );
//            while( _MI2C1IF );
//            acktest = 1;
//            while( acktest )
//            {
//                I2C1CONbits.RSEN = 1;
//                while( !_MI2C1IF );
//                while( _MI2C1IF );
//                I2C1TRN = 0x00A0;
//                while( _TRSTAT );
//                acktest = I2C1STATbits.ACKSTAT;
//            }
//
//            I2C1CONbits.PEN = 1;
//            while( !_MI2C1IF );
//            while( _MI2C1IF );
//            jDone = 0;
//            //eI2CCmdState = I2C_CMD_STA_STOP;
//            I2C1CONbits.PEN = 1;
//            eI2CCmdState = I2C_CMD_STA_STOP_END;
//            break;
            
            
//       case I2C_CMD_STA_END:
//            eI2CCmd = I2C_CMD_IDLE;
//            eI2CCmdState = I2C_CMD_STA_IDLE;
//            break;
            
//        case I2C_CMD_STA_IDLE:
//            break;
        default:
            eI2CCmd = I2C_CMD_IDLE;
            eI2CCmdState = I2C_CMD_STA_IDLE;
            break;
    }
}


#else   /* ISR or Non-ISR I2C Process Loop Driver Functions */
/******************************************************************************
 * I2C Non-ISR Related Functions
 *****************************************************************************/

void I2CMEM_vProcessRW(void)
{
    switch (eI2CCmdState)
    {
        /*==================================*/
        /* Control/Address Phase            */
        /*==================================*/
        case I2C_CMD_STA_START:
            // Start Condition
            I2C1CONbits.SEN = 1;
            eI2CCmdState = I2C_CMD_STA_START_WAIT;
            break;
        case I2C_CMD_STA_START_WAIT:
            // Start (Control) Byte with device select id
            if( jDone == 1 )
            {
                jDone = 0;
                //    I2C1TRN=(0x00A0)|(((i2cMem->oData->csel)&0x7)<<1);
                I2C1TRN = eI2CCmdCtrlByte & (~(0x0001));    /* Always Write command for Address */
                eI2CCmdState = I2C_CMD_STA_SEND_CTRL;
            }
            break;
       case I2C_CMD_STA_SEND_CTRL:
            // Send address byte 1, if ack is received. Else Retry
            if( jDone == 1 )
            {
                jDone = 0;

                if( I2C1STATbits.ACKSTAT == 1 )
                {                   // Ack Not received, Retry
                    if( rtrycntr < I2C_MAX_RETRY )
                    {
                        eI2CCmdState = I2C_CMD_STA_RETRY;
                    }
                    else
                    {
                        eI2CCmdState = I2C_CMD_STA_ERROR; // Flag error and exit
                    }
                }
                else
                {
                    rtrycntr = 0;
                    if (u8I2CAddrSize == 2)
                    {
                    //#if I2C_ADDRWIDTH == TWO_BYTE
                        I2C1TRN = (u16I2CCmdAddress)  >> 8;
                        eI2CCmdState = I2C_CMD_STA_SEND_ADDRESS_HI;
                    //#endif
                    }
                    else /* if (u8I2CAddrSize == 1) */
                    {
                    //#if I2C_ADDRWIDTH == ONE_BYTE
                        I2C1TRN = (u16I2CCmdAddress);
                        eI2CCmdState = I2C_CMD_STA_SEND_ADDRESS_LO;
                    //#endif
                    }
                }
            }
            break;
            
         case I2C_CMD_STA_SEND_ADDRESS_HI:
            // Send address byte 2, if ack is received. Else Flag error and exit
            if( jDone == 1 )
            {
                jDone = 0;

                if( I2C1STATbits.ACKSTAT == 1 )
                {                   // Ack Not received, Flag error and exit
                    eI2CCmdState = I2C_CMD_STA_ERROR;
                }
                else
                {
                    if (u8I2CAddrSize == 2)
                    {
                    //#if I2C_ADDRWIDTH == TWO_BYTE
                        I2C1TRN = ( (u16I2CCmdAddress) & 0x00FF );
                    //#endif
                    }
                    eI2CCmdState = I2C_CMD_STA_SEND_ADDRESS_LO;
                }
            }

            break;

        case I2C_CMD_STA_SEND_ADDRESS_LO:
            // Read or Write
            if( jDone == 1 )
            {
                jDone = 0;

                if( I2C1STATbits.ACKSTAT == 1 )
                {                   // Ack Not received, Flag error and exit
                    eI2CCmdState = I2C_CMD_STA_ERROR;
                }
                else
                {
                    if( eI2CCmdCommand == I2C_CMD_WR )
                    {
                        eI2CCmdState = I2C_CMD_STA_SEND_DATA;
                    }

                    if( eI2CCmdCommand == I2C_CMD_RD )
                    {
                        eI2CCmdState = I2C_CMD_STA_READ_DATA_START;
                    }
                }
            }

            break;

        /*==================================*/
        /* Write Data Phase                    */
        /*==================================*/
        case I2C_CMD_STA_SEND_DATA:
            // Send data
            I2C1TRN = *( pI2CCmdBuffer + cntr );
            eI2CCmdState = I2C_CMD_STA_SEND_DATA_ACK;
            cntr = cntr + 1;
            break;

        case I2C_CMD_STA_SEND_DATA_ACK:
            // Look for end of data or no Ack
            if( jDone == 1 )
            {
                jDone = 0;
                eI2CCmdState = I2C_CMD_STA_SEND_DATA;

                if( I2C1STATbits.ACKSTAT == 1 )
                {                   // Ack Not received, Flag error and exit
                    eI2CCmdState = I2C_CMD_STA_ERROR;
                }
                else
                {
                    if( cntr == u16I2CCmdSize )
                    {               
                        eI2CCmdState = I2C_CMD_STA_STOP;   // Close the Frame without ACK polling
                        //eI2CCmdState = I2C_CMD_STA_ACK_POOLING; // Go to ACK polling to wait for write to complete
                    }
                }
            }

            break;
            
            
            
            
        /*==================================*/
        /* Read Data Phase                    */
        /*==================================*/
        case I2C_CMD_STA_READ_DATA_START:
            // Repeat Start
            I2C1CONbits.RSEN = 1;
            eI2CCmdState = I2C_CMD_STA_READ_DATA_CONTROL;
            break;

        case I2C_CMD_STA_READ_DATA_CONTROL:
            // Re-send control byte with W/R=R
            if( jDone == 1 )
            {
                jDone = 0;
                eI2CCmdState = I2C_CMD_STA_READ_DATA_CONTROL_ACK;
                I2C1TRN = eI2CCmdCtrlByte | (0x0001);   /* Always read data request here */
            }

            break;

        case I2C_CMD_STA_READ_DATA_CONTROL_ACK:
            // Check, if control byte went ok
            if( jDone == 1 )
            {
                jDone = 0;
                eI2CCmdState = I2C_CMD_STA_READ_DATA_RECEIVE;

                if( I2C1STATbits.ACKSTAT == 1 )
                {                   // Ack Not received, Flag error and exit
                    eI2CCmdState = I2C_CMD_STA_ERROR;
                }
            }

            break;

        case I2C_CMD_STA_READ_DATA_RECEIVE:
            // Receive Enable
            I2C1CONbits.RCEN = 1;
            eI2CCmdState = I2C_CMD_STA_READ_DATA_RECEIVE_ACK;
            break;

        case I2C_CMD_STA_READ_DATA_RECEIVE_ACK:
            // Receive data
            if( jDone == 1 )
            {
                jDone = 0;
                eI2CCmdState = I2C_CMD_STA_READ_DATA_RECEIVE_END;

                *( pI2CCmdBuffer + cntr ) = I2C1RCV;
                cntr++;

                if( cntr == u16I2CCmdSize )
                {
                    I2C1CONbits.ACKDT = 1;  // No ACK
                }
                else
                {
                    I2C1CONbits.ACKDT = 0;  // ACK
                }

                I2C1CONbits.ACKEN = 1;
            }

            break;

        case I2C_CMD_STA_READ_DATA_RECEIVE_END:
            if( jDone == 1 )
            {
                jDone = 0;
                if( cntr == u16I2CCmdSize )
                {
                    eI2CCmdState = I2C_CMD_STA_STOP;
                }
                else
                {
                    eI2CCmdState = I2C_CMD_STA_READ_DATA_RECEIVE;
                }
            }

            break;

            
        /*==================================*/
        /* Stop Sequence                    */
        /*==================================*/
        case I2C_CMD_STA_STOP:
            I2C1CONbits.PEN = 1;
            eI2CCmdState = I2C_CMD_STA_STOP_END;
            break;

        case I2C_CMD_STA_STOP_END:
            if( jDone == 1 )
            {
                jDone = 0;
                eI2CCmdState = I2C_CMD_STA_END;
                cntr = 0;
                eI2CCmdCommand = I2C_CMD_IDLE;
            }

            break;

        /*==================================*/
        /* Set Error                         */
        /*==================================*/
        case I2C_CMD_STA_ERROR:
            I2C1CONbits.PEN = 1;
            eI2CCmdState = I2C_CMD_STA_ERROR_END;
            break;

        case I2C_CMD_STA_ERROR_END:
            if( jDone == 1 )
            {
                jDone = 0;
                eI2CCmdState = I2C_CMD_STA_END;
                rtrycntr = 0;
                cntr = 0;
                eI2CCmdCommand = 0xFFFF;
            }

            break;

        /*==================================*/
        /* Retry                             */
        /*==================================*/
        case I2C_CMD_STA_RETRY:
            I2C1CONbits.PEN = 1;
            eI2CCmdState = I2C_CMD_STA_RETRY_END;
            rtrycntr++;
            break;

        case I2C_CMD_STA_RETRY_END:
            if( jDone == 1 )
            {
                jDone = 0;
                eI2CCmdState = I2C_CMD_STA_START;
                cntr = 0;
            }

            break;

            /*==================================*/
            /* ACK Polling                        */
            /*==================================*/
            int acktest;

        case I2C_CMD_STA_ACK_POOLING:
            I2C1CONbits.PEN = 1;
            while( !_MI2C1IF );
            while( _MI2C1IF );
            acktest = 1;
            while( acktest )
            {
                I2C1CONbits.RSEN = 1;
                while( !_MI2C1IF );
                while( _MI2C1IF );
                I2C1TRN = 0x00A0;
                while( _TRSTAT );
                acktest = I2C1STATbits.ACKSTAT;
            }

            I2C1CONbits.PEN = 1;
            while( !_MI2C1IF );
            while( _MI2C1IF );
            jDone = 0;
            eI2CCmdState = I2C_CMD_STA_STOP;
            break;
            
            
       case I2C_CMD_STA_END:
            eI2CCmd = I2C_CMD_IDLE;
            eI2CCmdState = I2C_CMD_STA_IDLE;
            break;
            
        case I2C_CMD_STA_IDLE:
            break;
        default:
            break;
    }
}
#endif  /* ISR or Non-ISR I2C Process Loop Driver Functions */


#endif  /* MCC use instead */



#ifndef BOARDCFG_USE_MCC_I2C    /* to do - write support MCC and FLIR */

#if USE_SPI_FLIR
/******************************************************************************
 * FLIR Related Functions
 *****************************************************************************/

void I2CMEM_vFlirProcessCmd(void)
{
    uint8_t* ps;
    uint8_t* pd;
    uint16_t* pd16;
    //uint16_t* ps16;
    uint16_t u16Size;
    uint16_t u16Data;
   
    switch (eFlirI2CSta)
    {
        case FLIR_I2C_STA_GET_STA_001:
            ps = (uint8_t*)&FLIRRegisters + FLIR_ADR_REG_STA_LO;
            if (((*ps) & FLIR_BIT_POS_STA_LO_BUSY) != 0) /* check busy bit */
            {
                I2CMEM_vRead(I2C_CTRL_BYTE_FLIR, FLIR_ADR_REG_STA, (uint8_t*)&FLIRRegisters + FLIR_ADR_REG_STA, 2, 2, u8I2CModule);
            }
            else
            {
                if (eFlirI2CCmd == FLIR_I2C_CMD_RD)
                {  
                    if (u16FlirI2CSize == 0)
                    {
                        eFlirI2CSta = FLIR_I2C_STA_ERR;
                    }
                    else
                    {
                        if (u8ReverseByteOrder0)
                        {
                            /* copy and fix MSB First */
                            pd = ((uint8_t*)&FLIRRegisters + FLIR_ADR_REG_LEN);
                            u16Data = u16FlirI2CSize;
                            *(pd + 0) = (uint8_t)(u16Data >> 8);
                            *(pd + 1) = (uint8_t)(u16Data >> 0);
                        }
                        else
                        {
                            pd16 = ((uint16_t*)&FLIRRegisters + FLIR_ADR_REG_16BIT_LEN);
                            *pd16 = u16FlirI2CSize;
                        }
                        
                        eFlirI2CSta = FLIR_I2C_STA_SET_REG_CNT;
                        I2CMEM_vWrite(I2C_CTRL_BYTE_FLIR, FLIR_ADR_REG_LEN, (uint8_t*)&FLIRRegisters + FLIR_ADR_REG_LEN, 2, 2, u8I2CModule);
                    }
                }
                else
                if (eFlirI2CCmd == FLIR_I2C_CMD_WR)
                {
                    if (u16FlirI2CSize == 0)
                    {
                        eFlirI2CSta = FLIR_I2C_STA_ERR;
                    }
                    else
                    {
                        if (u8ReverseByteOrder1)
                        {
                            /* copy and fix MSB First */
                            ps = (uint8_t*)pu8FlirI2CData;
                            pd = ((uint8_t*)&FLIRRegisters + FLIR_ADR_REG_D00);
                            u16Size = u16FlirI2CSize;
                            while(u16Size--)
                            {
                                *(pd + 0) = *(ps + 1);
                                *(pd + 1) = *(ps + 0);
                                ps += 2;
                                pd += 2;
                            }
                        }
                        else
                        {
                            ps = (uint8_t*)pu8FlirI2CData;
                            pd = ((uint8_t*)&FLIRRegisters + FLIR_ADR_REG_D00);
                            u16Size = u16FlirI2CSize*2;
                            while(u16Size--)
                            {
                                *pd++ = *ps++;
                            }
                        }
                    
                        eFlirI2CSta = FLIR_I2C_STA_SET_DAT_BUF;
                        I2CMEM_vWrite(I2C_CTRL_BYTE_FLIR, FLIR_ADR_REG_D00, (uint8_t*)&FLIRRegisters + FLIR_ADR_REG_D00, u16FlirI2CSize*2, 2, u8I2CModule);
                    }
                }
                else
                if (eFlirI2CCmd == FLIR_I2C_CMD_RUN)
                {

                    if (u8ReverseByteOrder0)
                    {
                        /* copy and fix MSB First */
                        pd = ((uint8_t*)&FLIRRegisters + FLIR_ADR_REG_CMD);
                        u16Data = u16FlirI2CCommand;
                        *(pd + 0) = (uint8_t)(u16Data >> 8);
                        *(pd + 1) = (uint8_t)(u16Data >> 0);
                    }
                    else
                    {
                        pd16 = ((uint16_t*)&FLIRRegisters + FLIR_ADR_REG_16BIT_CMD);
                        *pd16 = u16FlirI2CCommand;
                    }
                    eFlirI2CSta = FLIR_I2C_STA_SET_CMD_REG;
                    I2CMEM_vWrite(I2C_CTRL_BYTE_FLIR, FLIR_ADR_REG_CMD, (uint8_t*)&FLIRRegisters + FLIR_ADR_REG_CMD, 2, 2, u8I2CModule);
                }
            }
            break;
        case FLIR_I2C_STA_SET_REG_CNT:
            if((eFlirI2CCmd == FLIR_I2C_CMD_RD)
            || (eFlirI2CCmd == FLIR_I2C_CMD_WR))
            {
                    if (u8ReverseByteOrder0)
                    {
                        /* copy and fix MSB First */
                        pd = ((uint8_t*)&FLIRRegisters + FLIR_ADR_REG_CMD);
                        u16Data = u16FlirI2CCommand;
                        *(pd + 0) = (uint8_t)(u16Data >> 8);
                        *(pd + 1) = (uint8_t)(u16Data >> 0);
                    }
                    else
                    {
                        pd16 = ((uint16_t*)&FLIRRegisters + FLIR_ADR_REG_16BIT_CMD);
                        *pd16 = u16FlirI2CCommand;
                    }

                eFlirI2CSta = FLIR_I2C_STA_SET_CMD_REG;
                I2CMEM_vWrite(I2C_CTRL_BYTE_FLIR, FLIR_ADR_REG_CMD, (uint8_t*)&FLIRRegisters + FLIR_ADR_REG_CMD, 2, 2, u8I2CModule);
            }
            break;
        case FLIR_I2C_STA_SET_CMD_REG:
            if((eFlirI2CCmd == FLIR_I2C_CMD_RD)
            || (eFlirI2CCmd == FLIR_I2C_CMD_WR)
            || (eFlirI2CCmd == FLIR_I2C_CMD_RUN))
            {
                eFlirI2CSta = FLIR_I2C_STA_GET_STA_002;  
                I2CMEM_vRead(I2C_CTRL_BYTE_FLIR, FLIR_ADR_REG_STA, (uint8_t*)&FLIRRegisters + FLIR_ADR_REG_STA, 2, 2, u8I2CModule);
            } 
            break;
        case FLIR_I2C_STA_GET_STA_002:
            ps = (uint8_t*)&FLIRRegisters + FLIR_ADR_REG_STA_LO;
            if (((*ps) & FLIR_BIT_POS_STA_LO_BUSY) != 0) /* check busy bit */
            {
                I2CMEM_vRead(I2C_CTRL_BYTE_FLIR, FLIR_ADR_REG_STA, (uint8_t*)&FLIRRegisters + FLIR_ADR_REG_STA, 2, 2, u8I2CModule);
            }
            else
            {
                ps = (uint8_t*)&FLIRRegisters + FLIR_ADR_REG_STA_HI;
                LEP_RESULT eLepResult = (LEP_RESULT)(*ps);
                if (eLepResult != 0)
                {
                    /* Error */
                    eFlirI2CSta = FLIR_I2C_STA_ERR;
                }
                else
                {
                    if (eFlirI2CCmd == FLIR_I2C_CMD_RD)
                    {
                        eFlirI2CSta = FLIR_I2C_STA_GET_DAT_BUF;
                        I2CMEM_vRead(I2C_CTRL_BYTE_FLIR, FLIR_ADR_REG_D00, (uint8_t*)&FLIRRegisters + FLIR_ADR_REG_D00, u16FlirI2CSize*2, 2, u8I2CModule);
                    }
                    else
                    if((eFlirI2CCmd == FLIR_I2C_CMD_WR)
                    || (eFlirI2CCmd == FLIR_I2C_CMD_RUN))
                    {
                        eFlirI2CSta = FLIR_I2C_STA_IDLE;
                        //eFlirI2CSta = FLIR_I2C_STA_DONE;
                    }
                }
            }
            break;
        case FLIR_I2C_STA_GET_DAT_BUF:
            if (eFlirI2CCmd == FLIR_I2C_CMD_RD)
            {
                
                if (u8ReverseByteOrder2)
                {
                    /* copy and fix MSB First */
                    ps = ((uint8_t*)&FLIRRegisters + FLIR_ADR_REG_D00);
                    pd = (uint8_t*)pu8FlirI2CData;
                    u16Size = u16FlirI2CSize;
                    while(u16Size--)
                    {
                        *(pd + 0) = *(ps + 1);
                        *(pd + 1) = *(ps + 0);
                        ps += 2;
                        pd += 2;
                    }
                    
                }
                else
                {
                    ps = (uint8_t*)&FLIRRegisters + FLIR_ADR_REG_D00;
                    pd = pu8FlirI2CData;
                    u16Size = u16FlirI2CSize*2;
                    while(u16Size--)
                    {
                        *pd++ = *ps++;
                    }
                    
                }
                eFlirI2CSta = FLIR_I2C_STA_IDLE;
                //eFlirI2CSta = FLIR_I2C_STA_DONE;
            }
            break;
            
        case FLIR_I2C_STA_SET_DAT_BUF:
            if (eFlirI2CCmd == FLIR_I2C_CMD_WR)
            {
                if (u8ReverseByteOrder0)
                {
                    /* copy and fix MSB First */
                    pd = ((uint8_t*)&FLIRRegisters + FLIR_ADR_REG_LEN);
                    u16Data = u16FlirI2CSize;
                    *(pd + 0) = (uint8_t)(u16Data >> 8);
                    *(pd + 1) = (uint8_t)(u16Data >> 0);
                }
                else
                {
                    pd16 = ((uint16_t*)&FLIRRegisters + FLIR_ADR_REG_16BIT_LEN);
                    *pd16 = u16FlirI2CSize;
                }
                
                eFlirI2CSta = FLIR_I2C_STA_SET_REG_CNT;
                I2CMEM_vWrite(I2C_CTRL_BYTE_FLIR, FLIR_ADR_REG_LEN, (uint8_t*)&FLIRRegisters + FLIR_ADR_REG_LEN, 2, 2, u8I2CModule);
            }
            break;
            
        case FLIR_I2C_STA_DONE:
            //eFlirI2CSta = FLIR_I2C_STA_IDLE;
            break;
        case FLIR_I2C_STA_ERR:
            break;
        default:
            //eFlirI2CSta = FLIR_I2C_STA_IDLE;
            break;
    }
}




void I2CMEM_vFlirRead(uint16_t u16Command, uint8_t* pdData, uint16_t u16Size, uint8_t u8I2CModule)
{
    if ((eFlirI2CSta == FLIR_I2C_STA_ERR) || (eFlirI2CSta == FLIR_I2C_STA_DONE))
    {
        eFlirI2CSta = FLIR_I2C_STA_IDLE;
    }

    if (eFlirI2CSta == FLIR_I2C_STA_IDLE)
    {
        eFlirI2CCmd = FLIR_I2C_CMD_RD;  
        eFlirI2CSta = FLIR_I2C_STA_GET_STA_001;  
        
        u16FlirI2CCommand = u16Command;
        /* Fix Command */
        u16FlirI2CCommand &= ~0x0001;
        u16FlirI2CCommand &= ~0x0002;
        
        pu8FlirI2CData = pdData;
        u16FlirI2CSize = u16Size;
        if (u16FlirI2CSize == 0)
        {
            u16FlirI2CSize = 1;
        }
        
        I2CMEM_vRead(I2C_CTRL_BYTE_FLIR, FLIR_ADR_REG_STA, (uint8_t*)&FLIRRegisters + FLIR_ADR_REG_STA, 2, 2, u8I2CModule);
    }
}

void I2CMEM_vFlirWrite(uint16_t u16Command, uint8_t* pdData, uint16_t u16Size, uint8_t u8I2CModule)
{
    if ((eFlirI2CSta == FLIR_I2C_STA_ERR) || (eFlirI2CSta == FLIR_I2C_STA_DONE))
    {
        eFlirI2CSta = FLIR_I2C_STA_IDLE;
    }

    if (eFlirI2CSta == FLIR_I2C_STA_IDLE)
    {
        eFlirI2CCmd = FLIR_I2C_CMD_WR;  
        eFlirI2CSta = FLIR_I2C_STA_GET_STA_001;  
        
        u16FlirI2CCommand = u16Command;
        /* Fix Command */
        u16FlirI2CCommand |= 1;
        u16FlirI2CCommand &= ~0x0002;
        
        
        
        
        
        pu8FlirI2CData = pdData;
        u16FlirI2CSize = u16Size;
        if (u16FlirI2CSize == 0)
        {
            u16FlirI2CSize = 1;
        }

        I2CMEM_vRead(I2C_CTRL_BYTE_FLIR, FLIR_ADR_REG_STA, (uint8_t*)&FLIRRegisters + FLIR_ADR_REG_STA, 2, 2, u8I2CModule);
    }
}

void I2CMEM_vFlirRun(uint16_t u16Command, uint8_t u8I2CModule)
{
    if ((eFlirI2CSta == FLIR_I2C_STA_ERR) || (eFlirI2CSta == FLIR_I2C_STA_DONE))
    {
        eFlirI2CSta = FLIR_I2C_STA_IDLE;
    }

    if (eFlirI2CSta == FLIR_I2C_STA_IDLE)
    {
        eFlirI2CCmd = FLIR_I2C_CMD_RUN;  
        eFlirI2CSta = FLIR_I2C_STA_GET_STA_001;  
        
        u16FlirI2CCommand = u16Command;
        /* Fix Command */
        u16FlirI2CCommand &= ~0x0001;
        u16FlirI2CCommand |= 2;
        
        //pu8FlirI2CData = pdData;
        //u16FlirI2CSize = u16Size;
        
        I2CMEM_vRead(I2C_CTRL_BYTE_FLIR, FLIR_ADR_REG_STA, (uint8_t*)&FLIRRegisters + FLIR_ADR_REG_STA, 2, 2, u8I2CModule);
    }
}


/* *****************************************************************************
 * I2CMEM_pFlirRegToIntRam
 * 
 * Description: Copy Data from external memory to internal RAM and 
 *              returns pointer to internal RAM
 **************************************************************************** */
HMDRV_ADDR I2CMEM_pFlirRegToIntRam(HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_U16 addr = (HMDRV_U16)(HMDRV_ADDR_SIZE) nSrcAddr;
    HMDRV_U8* ps = pI2CFlirBufferRD;
    HMDRV_SIZE8 I2CModule = u8I2CModule;
    
    /* OEM or RAD command */
    if (addr >= 0x0800)  
    {
        addr |= (1 << 14);
    }

    u16FlirI2CCommandReq = addr;

    while(eI2CCmd == I2C_CMD_BUSY){;} /*  wait pending transfer */  
    I2CMEM_vFlirRead(u16FlirI2CCommandReq, pI2CFlirBufferRD , nSize, I2CModule);
    while(eI2CCmd == I2C_CMD_BUSY){;} /*  wait pending transfer */
    
    return (HMDRV_ADDR)ps;
}
/* *****************************************************************************
 * I2CMEM_pFlirRegFromIntRam
 * 
 * Description: returns pointer to internal RAM for 
 *              Copy Data to Flir Register from internal RAM 
 *              
 **************************************************************************** */
HMDRV_ADDR I2CMEM_pFlirRegFromIntRam(void)
{
    HMDRV_U8* pb = pI2CFlirBufferWR;
    return (HMDRV_ADDR)pb;
}

/* *****************************************************************************
 * I2CMEM_vFlirRegFromIntRamCopyData
 * 
 * Description: Copy Data to external memory from already filled internal RAM buffer
 **************************************************************************** */
void I2CMEM_vFlirRegFromIntRamCopyData(HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_U16 addr = (HMDRV_U16)(HMDRV_ADDR_SIZE) nDestAddr;
    HMDRV_SIZE8 size = nSize;
    HMDRV_SIZE8 I2CModule = u8I2CModule;
    HMDRV_U16 RequestType;
    
    /* OEM or RAD command */
    if (addr >= 0x0800)  
    {
        addr |= (1 << 14);
    }

    u16FlirI2CCommandReq = addr;
    
    

    
    while(eI2CCmd == I2C_CMD_BUSY){;} /*  wait pending transfer */
    RequestType = (addr & 0x0003);
    if (RequestType == 0x02)    /* Run */
    {
        I2CMEM_vFlirRun(u16FlirI2CCommandReq, I2CModule);
    }
    else  /* Set for any other address */
    {
        I2CMEM_vFlirWrite(u16FlirI2CCommandReq, pI2CFlirBufferWR, size, I2CModule);
    }
    
    
}
#endif  /* FLIR */

#endif /* MCC */
