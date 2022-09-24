/* *****************************************************************************
 * File:   i2cmem.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 02 05 11:42
 * 
 * Description: I2C Interface To External RTCC Memory
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */
#ifndef I2CMEM_H
#define I2CMEM_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdint.h>

#include "boardcfg.h"
#include "extmemcfg.h"
#include "hmlib.h"

#ifdef __cplusplus  // Provide C++ Compatibility
extern "C"
{
#endif

/* *****************************************************************************
 * Constants
 **************************************************************************** */
    
/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

typedef enum
{
    I2C_1,
    I2C_2,
    I2C_COUNT,
}I2C_eModule_t;

    
/* I2C RW Request */    
typedef enum
{
    I2C_CMD_IDLE,
    I2C_CMD_BUSY,
    I2C_CMD_WR,
    I2C_CMD_RD,
    I2C_CMD_RTCC_WR,
    I2C_CMD_RTCC_RD,
    I2C_CMD_SRAM_WR,
    I2C_CMD_SRAM_RD,
    I2C_CMD_EEPROM_WR,
    I2C_CMD_EEPROM_RD,
    I2C_CMD_PICAUX_WR,
    I2C_CMD_PICAUX_RD,
    I2C_CMD_FLIR_WR,
    I2C_CMD_FLIR_RD,
    I2C_CMD_FLIR_CCI_TWI_WR,               /* Flir CCI/TWI Protocol */
    I2C_CMD_FLIR_CCI_TWI_RD,               /* Flir CCI/TWI Protocol */
    I2C_CMD_FLIR_CCI_TWI_RUN,              /* Flir CCI/TWI Protocol */
    I2C_CMD_COUNT            
    
}eI2CCmd_t;
    
    
    
    
/* Flir Commands */    
typedef enum
{
    FLIR_I2C_CMD_IDLE,
    FLIR_I2C_CMD_RD,
    FLIR_I2C_CMD_WR,
    FLIR_I2C_CMD_RUN,

    FLIR_I2C_CMD_COUNT            
    
}eFlirI2CCmd_t;



/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

typedef struct
{
    uint8_t SECONE : 4;
    uint8_t SECTEN : 3;
    uint8_t ST     : 1;
}sRTCSEC_t;

typedef struct
{
    uint8_t MINONE : 4;
    uint8_t MINTEN : 3;
    uint8_t        : 1;
}sRTCMIN_t;

typedef struct
{
    uint8_t HRONE : 4;
    uint8_t HRTEN : 1;
    uint8_t PM    : 1;
    uint8_t mode12: 1;
    uint8_t       : 1;
}sRTCHOUR12_t;

typedef struct
{
    uint8_t HRONE : 4;
    uint8_t HRTEN : 2;
    uint8_t mode12: 1;
    uint8_t       : 1;
}sRTCHOUR24_t;

typedef union 
{
    sRTCHOUR12_t sRTCHOUR12;
    sRTCHOUR24_t sRTCHOUR24;
}uRTCHOUR_t;

typedef struct
{
    uint8_t WKDAY   : 3;
    uint8_t VBATEN  : 1;
    uint8_t PWRFAIL : 1;
    uint8_t OSCRUN  : 1;
    uint8_t         : 2;
}sRTCWKDAY_t;

typedef struct
{
    uint8_t WKDAY : 3;
    uint8_t ALMIF : 1;
    uint8_t ALMMSK: 3;
    uint8_t ALMPOL: 1;
}sALMWKDAY_t;

typedef struct
{
    uint8_t DATEONE : 4;
    uint8_t DATETEN : 2;
    uint8_t         : 2;
}sRTCDATE_t;

typedef struct
{
    uint8_t MTHONE : 4;
    uint8_t MTHTEN : 1;
    uint8_t LPYR   : 1;
    uint8_t        : 2;
}sRTCMTH_t;

typedef struct
{
    uint8_t MTHONE : 4;
    uint8_t MTHTEN : 1;
    uint8_t WKDAY  : 3;
}sPWRMTH_t;




typedef struct
{
    uint8_t YEARONE : 4;
    uint8_t YEARTEN : 4;
}sRTCYEAR_t;

typedef struct
{
    uint8_t SQWFS   : 2;
    uint8_t CRSTRIM : 1;
    uint8_t EXTOSC  : 1;
    uint8_t ALM0EN  : 1;
    uint8_t ALM1EN  : 1;
    uint8_t SWQEN   : 1;
    uint8_t OUT     : 1;
}sRTCCONTROL_t;



typedef struct
{
    sRTCSEC_t sRTCSEC;
    sRTCMIN_t sRTCMIN;
    uRTCHOUR_t uRTCHOUR;
    sRTCWKDAY_t sRTCWKDAY;
    sRTCDATE_t sRTCDATE;
    sRTCMTH_t sRTCMTH;
    sRTCYEAR_t sRTCYEAR;
    sRTCCONTROL_t sRTCCONTROL;
    int8_t s8RTCOSCTRIM;        /* signed trim value */
    uint8_t u8EEUNLOCK;
    
}sTimekeeping_t;

typedef struct
{
    uint8_t u8RTCSEC;
    uint8_t u8RTCMIN;
    uint8_t u8RTCHOUR;
    uint8_t u8RTCWKDAY;
    uint8_t u8RTCDATE;
    uint8_t u8RTCMTH;
    uint8_t u8RTCYEAR;
    uint8_t u8RTCCONTROL;
    uint8_t u8RTCOSCTRIM;        /* unsigned trim value */
    uint8_t u8EEUNLOCK;
    
}sTimekeepReg_t;

typedef union
{
    sTimekeeping_t sTimekeeping;
    sTimekeepReg_t sTimekeepReg;
}uTimekeeping_t;

typedef struct
{
    sRTCSEC_t   sALMSEC;
    sRTCMIN_t   sALMMIN;
    uRTCHOUR_t  uALMHOUR;
    sALMWKDAY_t sALMWKDAY;
    sRTCDATE_t  sALMDATE_t;
    sRTCMTH_t   sALMMTH;
    uint8_t     Reserved;
    
}sAlarm_t;


typedef struct
{
    sRTCMIN_t   sPWRMIN;
    uRTCHOUR_t  uPWRHOUR;
    sRTCDATE_t  sPWRDATE_t;
    sPWRMTH_t   sPWRMTH;
    uint8_t     Reserved;
    
}sPowerFail_t;


typedef struct
{
    uTimekeeping_t  uTimekeeping;
    sAlarm_t        asAlarm[2];
    sPowerFail_t    sPowerDown;
    sPowerFail_t    sPowerUp;
}sRTCC_t;



/* *****************************************************************************
 * Constants
 **************************************************************************** */
#if USE_SPI_FLIR
extern eFlirI2CCmd_t eFlirI2CCmd;
#endif
extern eI2CCmd_t eI2CCmd;           /* Command For External Set */
extern sRTCC_t RTCC;
    
/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void I2CMEM_vCopyFromExtMem(HMDRV_BPTR pDestAddr, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize);
void I2CMEM_vCopyToExtMem(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize);
void I2CMEM_vCopyToExtMemWithMask(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize);
HMDRV_ADDR I2CMEM_pExtMemRamToIntRam(HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize);
HMDRV_ADDR I2CMEM_pExtMemRamFromIntRam(HMDRV_ADDR nDestAddr);
void I2CMEM_vProcessCmd(void);
void I2CMEM_vExtMemRamFromIntRamCopyData(HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize);

bool I2CMEM_bGetCmdStateIdle(void);     //same as I2CMEM_bGetMemStateIdle in new version
bool I2CMEM_bGetMemStateIdle(void);

bool I2CMEM_bGetCmdStateModuleIdle(I2C_eModule_t eModule);


#ifndef BOARDCFG_USE_MCC_I2C
void I2CMEM_vInit(uint32_t u32Fcy);
void I2CMEM_vProcessRW(void);   /* If Pooling In Main Loop Used */
#else
#define I2CMEM_vProcessRW()
#endif

#ifndef BOARDCFG_USE_MCC_I2C    
void I2CMEM_vWrite(uint16_t u16AddressI2C, uint16_t u16Address, HMDRV_ADDR_PNTR_MODIFIER uint8_t* pdData, uint16_t u16Size, uint8_t u8AddressSize, uint8_t u8I2CModule);
void I2CMEM_vRead (uint16_t u16AddressI2C, uint16_t u16Address, HMDRV_ADDR_PNTR_MODIFIER uint8_t* pdData, uint16_t u16Size, uint8_t u8AddressSize, uint8_t u8I2CModule);
#else
void I2CMEM_vWrite(uint16_t u16AddressI2C, uint16_t u16Address, HMDRV_ADDR_PNTR_MODIFIER uint8_t* pdData, uint16_t u16Size, uint8_t u8AddressSize, uint8_t u8I2CModule, I2C_MESSAGE_STATUS* pu8StatusTransaction);
void I2CMEM_vRead (uint16_t u16AddressI2C, uint16_t u16Address, HMDRV_ADDR_PNTR_MODIFIER uint8_t* pdData, uint16_t u16Size, uint8_t u8AddressSize, uint8_t u8I2CModule, I2C_MESSAGE_STATUS* pu8StatusTransaction);
#endif


#if USE_SPI_FLIR
/* Flir Protocol */
void I2CMEM_vFlirRead(uint16_t u16Command, uint8_t* pdData, uint16_t u16Size, uint8_t u8I2CModule);
void I2CMEM_vFlirWrite(uint16_t u16Command, uint8_t* pdData, uint16_t u16Size, uint8_t u8I2CModule);
void I2CMEM_vFlirRun(uint16_t u16Command, uint8_t u8I2CModule);
HMDRV_ADDR I2CMEM_pFlirRegToIntRam(HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize);
HMDRV_ADDR I2CMEM_pFlirRegFromIntRam(void);
void       I2CMEM_vFlirRegFromIntRamCopyData(HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize);
#endif

#ifdef __cplusplus  // Provide C++ Compatibility
}
#endif

#endif  /* I2CMEM_H */
