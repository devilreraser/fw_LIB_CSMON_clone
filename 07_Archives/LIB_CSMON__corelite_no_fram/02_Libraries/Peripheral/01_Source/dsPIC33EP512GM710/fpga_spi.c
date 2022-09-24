/* *****************************************************************************
 * File:   fpga_spi.c
 * Author: Dimitar Lilov
 *
 * Created on 2020 05 13 10:55
 * 
 * Description: ...
 * 
 **************************************************************************** */
#ifdef _WIN32
#pragma once
#endif /* _WIN32 */

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "fpga_spi.h"


#include "boardcfg.h"
#include "sys_drv.h"
#include "spi.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define FPGA_SPI_USE_ISR_DRV        1       

#define FPGA_SPI_MODE_16_BITS       1

#if FPGA_SPI_MODE_16_BITS
#define FPGA_SPI_MODE_BITS          SPI_MODE_16BITS
#else
#define FPGA_SPI_MODE_BITS          SPI_MODE_8BITS
#endif
/*
 
-- spi communication to DSP
-- mode 0 active clock is 1 inactive 0 and data is ready from inactive to active
-- data description:
-- s_faults_flt
-- "00" & s_drv_det_flt
-- "00" & s_otemp_flt
-- "00" & s_err_flt
-- s_ovn_flt & "0" & s_ocn_flt
-- s_ovp_flt & "0" & s_ocp_flt
-- "0000" & s_LED you have to write what you want to see always, no matter what is next unlock 0xA5, leds are shown 
-- X"A5" to unlock the writing possible only if PWM_EN is inactive reading is always x"00" 
-- s_pwm_dtclc - 8 bit max 255 which corresponds to 21.25us resolution 0.08(3)us
-- s_pwm_mpclc - 8 bit max 255 which corresponds to 21.25us resolution 0.08(3)us
-- "00" & s_pserr_mask
-- s_faults_mask
-- VERSION
-- REVISION
-- to have a succesfull writing VERSION & Revision should not be included 
-- if clocks for VERSION and REVISION are clocked then write is disabled
 
 */

#define FPGA_SCI_DEAD_TIME_USEC             2.5
#define FPGA_SCI_MIN_PULSE_USEC             2.0

#define FPGA_SPI_CLOCK_PWM_RESOLUTION_HZ    (120000000.0)
#define FPGA_SPI_CLOCK_TICK_PRESCALER       (10.0)

#define FPGA_SPI_TICK_TO_USEC_MULTIPLIER    (FPGA_SPI_CLOCK_TICK_PRESCALER / (FPGA_SPI_CLOCK_PWM_RESOLUTION_HZ / 1000000.0))
#define FPGA_SPI_USEC_TO_TICK_MULTIPLIER    (1.0 / FPGA_SPI_TICK_TO_USEC_MULTIPLIER)

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */
#define FPGA_SPI_WRITE_UNLOCK_MASK  0xA5
#define FPGA_SPI_WRITE_LOCK_MASK    0x00
    
#define FPGA_SPI_READ_REG_BYTES     14      /* All For Normal Read */
#define FPGA_SPI_READ_ERR_BYTES     6       /* Min For Error Data Read */
#define FPGA_SPI_WRITE_BYTES        12      /* Write Count */


/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
#ifndef uWord16_t
typedef union
{
    uint16_t u16Register;
    uint8_t au8Word[2];
} uWord16_t;
#endif



typedef struct
{
    uint8_t     bOCP    : 1;
    uint8_t     bOCN    : 1;
    uint8_t     bDRVERR : 1;
    uint8_t     bOTEMP  : 1;
    uint8_t     bDRVDET : 1;
    uint8_t     bOV     : 1;
    uint8_t             : 1;
    uint8_t     bFLT    : 1;
    
}sErrGlobal_t;

typedef struct
{
    uint8_t     bDRV1   : 1;
    uint8_t     bDRV2   : 1;
    uint8_t     bDRV3   : 1;
    uint8_t     bDRV4   : 1;
    uint8_t     bDRV5   : 1;
    uint8_t     bDRV6   : 1;
    uint8_t             : 1;
    uint8_t     bVOLT   : 1;
    
}sErrDriver_t;

typedef struct
{
    uint8_t     bLED1   : 1;
    uint8_t     bLED2   : 1;
    uint8_t     bLED3   : 1;
    uint8_t     bLED4   : 1;
    uint8_t             : 1;
    uint8_t             : 1;
    uint8_t             : 1;
    uint8_t             : 1;
    
}sDebugLeds_t;

typedef struct
{
    uint8_t     u8ErrGlobal;    /* FLT, 0, OV, DRVDET, OTEMP, DRVERR, OCN, OCP When there is fault bit is 1 */
    uint8_t     u8ErrDetect;
    uint8_t     u8ErrOvTemp;
    uint8_t     u8ErrDriver;
    uint8_t     u8ErrAnaNeg;    /* Over Voltage and Over Current 0xVXCCCCCC (V - Voltage; C - Current; X - Don't care) */
    uint8_t     u8ErrAnaPos;    /* Over Voltage and Over Current 0xVXCCCCCC (V - Voltage; C - Current; X - Don't care) */
    uint8_t     u8DebugLeds;    /* can be written without unlock mask 0xA5 */
    uint8_t     u8MskUnlock;    /* Write 0xA5 to write the data. the writing possible only if PWM_EN is inactive reading is always x"00" */
    uint8_t     u8DeadTime;     /* 8 bit max 255 which corresponds to 25.5us resolution 0.1us @ 50 MHz FPGA Clock - consider clock 60 MHz -> 0.08(3)us 21.25 max */
    uint8_t     u8MinPulse;     /* 8 bit max 255 which corresponds to 25.5us resolution 0.1us @ 50 MHz FPGA Clock - consider clock 60 MHz -> 0.08(3)us 21.25 max */
    uint8_t     u8MskDriver;
    uint8_t     u8MskGlobal;   
    uint8_t     u8Version;    
    uint8_t     u8Revision;    
}sFPGARegisters_t;

typedef struct
{
    sErrGlobal_t    sErrGlobal;     /* FLT, 0, OV, DRVDET, OTEMP, DRVERR, OCN, OCP When there is fault bit is 1 */
    sErrDriver_t    sErrDetect;
    sErrDriver_t    sErrOvTemp;
    sErrDriver_t    sErrDriver;
    sErrDriver_t    sErrAnaNeg;     /* Over Voltage and Over Current 0xVXCCCCCC (V - Voltage; C - Current; X - Don't care) */
    sErrDriver_t    sErrAnaPos;     /* Over Voltage and Over Current 0xVXCCCCCC (V - Voltage; C - Current; X - Don't care) */
    sDebugLeds_t    sDebugLeds;
    uint8_t         :8;
    uint8_t         :8;
    uint8_t         :8;
    sErrDriver_t    sMskDriver;     /* sErrDetect sErrOvTemp sErrDriver sErrAnaNeg sErrAnaPos mask*/
    sErrGlobal_t    sMskGlobal;     /* sErrGlobal mask */
    uint8_t         :8;    
    uint8_t         :8;    
    
}sFPGAStructures_t;

typedef union
{
    sFPGARegisters_t    sFPGARegistersBytes;
    sFPGAStructures_t   sFPGAStructures;
}uFPGARegisters_t;

/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

#define FPGA_SPI_uSecToTicks(x) (uint8_t)((x + (FPGA_SPI_TICK_TO_USEC_MULTIPLIER / 2.0)) * FPGA_SPI_USEC_TO_TICK_MULTIPLIER) /* to do: prevent overflow */


/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
SPI_eModule_t eFPGASPIModule = FPGA_SPI_MODULE;
#if FPGA_SPI_MODE_16_BITS
typedef uint16_t FPGA_SPI_buff_t;
FPGA_SPI_buff_t buffSPITransactionBuffer[(sizeof(sFPGARegisters_t)+1)/2]; /* sizeof(sFPGARegisters_t) must be less than 8 or  Rework SPI */
FPGA_SPI_buff_t buffSPITransactionBufferFbk[(sizeof(sFPGARegisters_t)+1)/2]; /* sizeof(sFPGARegisters_t) must be less than 8 or  Rework SPI */
#define FPGA_SPI_TRANSMISSION_COUNT (uint16_t)((sizeof(sFPGARegisters_t)+1)/2) 
#define FPGA_SPI_TRANSMISSION_BYTES (uint16_t)(sizeof(sFPGARegisters_t)) 
#else
typedef uint8_t FPGA_SPI_buff_t;
FPGA_SPI_buff_t buffSPITransactionBuffer[sizeof(sFPGARegisters_t)]; /* sizeof(sFPGARegisters_t) must be less than 8 or  Rework SPI */
FPGA_SPI_buff_t buffSPITransactionBufferFbk[sizeof(sFPGARegisters_t)]; /* sizeof(sFPGARegisters_t) must be less than 8 or  Rework SPI */
#define FPGA_SPI_TRANSMISSION_COUNT (uint16_t)(sizeof(sFPGARegisters_t)) 
#define FPGA_SPI_TRANSMISSION_BYTES (uint16_t)(sizeof(sFPGARegisters_t)) 
#endif
bool bFPGAUpdate = false;

#if FPGA_SPI_MODE_16_BITS
#define FPGA_SPI_READ_REG_TRANSMISSION_COUNT    (uint16_t)((FPGA_SPI_READ_REG_BYTES+1)/2) 
#define FPGA_SPI_READ_ERR_TRANSMISSION_COUNT    (uint16_t)((FPGA_SPI_READ_ERR_BYTES+1)/2) 
#define FPGA_SPI_WRITE_TRANSMISSION_COUNT       (uint16_t)((FPGA_SPI_WRITE_BYTES+1)/2) 
#else
#define FPGA_SPI_READ_REG_TRANSMISSION_COUNT    (uint16_t)(FPGA_SPI_READ_REG_BYTES) 
#define FPGA_SPI_READ_ERR_TRANSMISSION_COUNT    (uint16_t)(FPGA_SPI_READ_ERR_BYTES) 
#define FPGA_SPI_WRITE_TRANSMISSION_COUNT       (uint16_t)(FPGA_SPI_WRITE_BYTES) 
#endif


sFPGARegisters_t sFPGARegisters = 
{
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00001111, /* LEDs */
    0xA5,
    FPGA_SPI_uSecToTicks(FPGA_SCI_DEAD_TIME_USEC),
    FPGA_SPI_uSecToTicks(FPGA_SCI_MIN_PULSE_USEC),
    0b00111111, /* sMskDriver */
    0b10111111, /* sMskGlobal */
    0x00,
    0x00,
    
};

sFPGARegisters_t sFPGARegistersFbk = 
{
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
};

bool bSendData = false;
bool bSentData = false;

bool bTransferProcess = false;
bool bTransferStarted = false;
bool bTransferWrite = false;
bool bTransferRead = false;


SPI_ePrimaryDivider_t      FPGA_ePriDividerFpga = SPI_PRIMARY_DIVIDER_1_DIV_1;
SPI_eSecondaryDivider_t    FPGA_eSecDividerFpga = SPI_SECONDARY_DIVIDER_1_DIV_6;

uint32_t u32SPIFpgaHz = (uint32_t)(SPI_FPGA_HZ_NO_MEM);
uint16_t u16SPIPrescalerFpga = SPI_PRESCALER(SPI_FPGA_HZ_NO_MEM);

volatile SPI_eTransferStatus_t eFPGASPIStatus = SPI_TRANSF_STA_IDLE;

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */


/* *****************************************************************************
 * FPGA_SPI_vChipSelectEnable - SPI Master Chip Select Enable - Used From SPI ISR
 **************************************************************************** */
void FPGA_SPI_vChipSelectEnable(uint16_t u16Memory)
{
    FPGA_SPI_CS_SetLow();
}
/* *****************************************************************************
 * FPGA_SPI_vChipSelectDisableAll - SPI Master Chip Select Enable - Used From SPI ISR
 **************************************************************************** */
void FPGA_SPI_vChipSelectDisableAll(void)
{
    FPGA_SPI_CS_SetHigh();
}
/* *****************************************************************************
 * FPGA_SPI_vChipSelectDisable - SPI Master Chip Select Enable - Used From SPI ISR
 **************************************************************************** */
void FPGA_SPI_vChipSelectDisable(uint16_t u16Memory)
{
    FPGA_SPI_CS_SetHigh();
}






void FPGA_SPI_vInit(uint32_t u32Fcy)
{
    SPI_vInit(eFPGASPIModule, SPI_DMA_NOT_USED);   /* used for the External DAC */
    SPI_vSetMode(eFPGASPIModule, SPI_FPGA_MODE_NO_MEM);
    SPI_vSetModeBitsCount(eFPGASPIModule,FPGA_SPI_MODE_BITS);
    SPI_vSetFrequency( eFPGASPIModule, u32SPIFpgaHz, u32Fcy);
    
#if FPGA_SPI_USE_ISR_DRV == 1
    SPI_vChipSelectFunctionsInit(eFPGASPIModule, FPGA_SPI_vChipSelectEnable, FPGA_SPI_vChipSelectDisable, FPGA_SPI_vChipSelectDisableAll );
#endif
}

void FPGA_SPI_vTransferRequest(void)
{
#if FPGA_SPI_USE_ISR_DRV == 0
    if (bSentData == true)
    {
        bSendData = true;
    }
#else
    if (bTransferProcess == false)
    {
        bTransferProcess = true;
        bTransferStarted = true;
    }
#endif
}

uint16_t u16IndexWr = 0xFFFF;
uint16_t u16IndexRd = 0xFFFF;

void FPGA_SPI_vProcessISR(void)
{
    
}

void FPGA_SPI_vProcess(void)
{
#if FPGA_SPI_USE_ISR_DRV
    
    uint16_t u16Index;
    
    if (bTransferProcess)
    {
        if (bTransferStarted)
        {
            bTransferStarted = false;
            bTransferWrite = true;
            bTransferRead = true;
        }
        
        
        if (bTransferWrite)
        {
            bTransferWrite = false;
            DBG1_SetHigh();
            sFPGARegisters.u8MskUnlock = FPGA_SPI_WRITE_UNLOCK_MASK;
            for (u16Index = 0; u16Index < FPGA_SPI_TRANSMISSION_COUNT; u16Index++)
            {
                /* Reverse Low and High Bytes if 16-bit spi */
                #if FPGA_SPI_MODE_16_BITS
                ((uWord16_t*)&buffSPITransactionBuffer)[u16Index].au8Word[0] = ((uWord16_t*)&sFPGARegisters)[u16Index].au8Word[1];
                ((uWord16_t*)&buffSPITransactionBuffer)[u16Index].au8Word[1] = ((uWord16_t*)&sFPGARegisters)[u16Index].au8Word[0];
                #else
                buffSPITransactionBuffer[u16Index] = ((FPGA_SPI_buff_t*)&sFPGARegisters)[u16Index];
                #endif
            }
            sFPGARegisters.u8MskUnlock = FPGA_SPI_WRITE_LOCK_MASK;
            SPI_ReadWriteBuffer ( (eFPGASPIModule | (SPI_MEM_FPGA << 4)), (uint8_t*)&buffSPITransactionBuffer, FPGA_SPI_WRITE_BYTES, &eFPGASPIStatus, NULL);
            DBG1_SetLow(); 
        }
        else
        {
            if (eFPGASPIStatus == SPI_TRANSF_STA_COMPLETED)
            {
                if (bTransferRead)
                {
                    bTransferRead = false;
                    DBG1_SetHigh();
                    
                    for (u16Index = 0; u16Index < FPGA_SPI_TRANSMISSION_COUNT; u16Index++)
                    {
                        #if FPGA_SPI_MODE_16_BITS
                        ((uWord16_t*)&buffSPITransactionBuffer)[u16Index].au8Word[0] = ((uWord16_t*)&sFPGARegisters)[u16Index].au8Word[1];
                        ((uWord16_t*)&buffSPITransactionBuffer)[u16Index].au8Word[1] = ((uWord16_t*)&sFPGARegisters)[u16Index].au8Word[0];
                        #else
                        buffSPITransactionBuffer[u16Index] = ((FPGA_SPI_buff_t*)&sFPGARegisters)[u16Index];
                        #endif
                    }
                    SPI_ReadWriteBuffer ( (eFPGASPIModule | (SPI_MEM_FPGA << 4)), (uint8_t*)&buffSPITransactionBuffer, FPGA_SPI_READ_REG_BYTES, &eFPGASPIStatus, NULL);
                    DBG1_SetLow(); 
                }
                else
                {
                    
                    for (u16Index = 0; u16Index < FPGA_SPI_TRANSMISSION_COUNT; u16Index++)
                    {
                        
                        #if FPGA_SPI_MODE_16_BITS
                        ((uWord16_t*)&sFPGARegistersFbk)[u16Index].au8Word[1] = ((uWord16_t*)&buffSPITransactionBuffer)[u16Index].au8Word[0];
                        ((uWord16_t*)&sFPGARegistersFbk)[u16Index].au8Word[0] = ((uWord16_t*)&buffSPITransactionBuffer)[u16Index].au8Word[1];
                        #else
                        ((FPGA_SPI_buff_t*)&sFPGARegistersFbk)[u16Index] = buffSPITransactionBuffer[u16Index];
                        #endif
                    }      
                    eFPGASPIStatus = SPI_TRANSF_STA_IDLE;
                    bTransferProcess = false;
                }
            }
        }
    }
    
    
    
#else
    
    //DBG1_SetHigh();

    if (u16IndexWr >= FPGA_SPI_TRANSMISSION_COUNT)  /* All data already sent */  
    {
        if (bSentData == false)
        {
            /* try to read all data and CS idle - don't care the last transmission (dummy) */
            if (SPI_GetFifoFill(eFPGASPIModule) == 0)
            {
                for ( ; u16IndexRd < FPGA_SPI_TRANSMISSION_COUNT; u16IndexRd++)
                {
                    /* spi2 and spi3 not working SRXMPT and SRMPT */
                    buffSPITransactionBufferFbk[u16IndexRd] = SPI_Read(eFPGASPIModule);
                    ((FPGA_SPI_buff_t*)&sFPGARegistersFbk)[u16IndexRd] = buffSPITransactionBufferFbk[u16IndexRd];
                }
                bSentData = true;
                FPGA_SPI_CS_SetHigh();
            }
        }
        if (bSendData)
        {
            bSendData = false;
            bSentData = false;
            u16IndexWr = 0;
            u16IndexRd = 0;

            SPI_Read(eFPGASPIModule);   /* one dummy read because can have one old dummy transmission in the read buffer */
            FPGA_SPI_CS_SetLow();
        }
    }
    
    if (u16IndexWr < FPGA_SPI_TRANSMISSION_COUNT)   /* sending data */ 
    {
        /* read all transmission except the last one - not sure completed */
        while(SPI_GetFifoFill(eFPGASPIModule) < (u16IndexWr - u16IndexRd))
        {
            buffSPITransactionBufferFbk[u16IndexRd] = SPI_Read(eFPGASPIModule);
            ((FPGA_SPI_buff_t*)&sFPGARegistersFbk)[u16IndexRd] = buffSPITransactionBufferFbk[u16IndexRd];
            u16IndexRd++;
        }
        
        
        for (u16IndexWr = 0; u16IndexWr < FPGA_SPI_TRANSMISSION_COUNT; u16IndexWr++)
        {
            if (SPI_TxBufferFull(eFPGASPIModule) == false)
            {
                buffSPITransactionBuffer[u16IndexWr] = ((FPGA_SPI_buff_t*)&sFPGARegisters)[u16IndexWr];
                SPI_Write(eFPGASPIModule, buffSPITransactionBuffer[u16IndexWr] );  
                
                /* read all transmissions except the last one - not sure completed */
                if (SPI_GetFifoFill(eFPGASPIModule) < (u16IndexWr - u16IndexRd))
                {
                    buffSPITransactionBufferFbk[u16IndexRd] = SPI_Read(eFPGASPIModule);
                    ((FPGA_SPI_buff_t*)&sFPGARegistersFbk)[u16IndexRd] = buffSPITransactionBufferFbk[u16IndexRd];
                    u16IndexRd++;
                }
            }
            else
            {
                break;
            }
        }

        
        if (u16IndexWr >= FPGA_SPI_TRANSMISSION_COUNT)
        {
           /* try to read all data and CS idle - don't care the last transmission (dummy) */
           if (SPI_GetFifoFill(eFPGASPIModule) == 0)
           {
               for ( ; u16IndexRd < FPGA_SPI_TRANSMISSION_COUNT; u16IndexRd++)
               {
                   /* spi2 and spi3 not working SRXMPT and SRMPT */
                   buffSPITransactionBufferFbk[u16IndexRd] = SPI_Read(eFPGASPIModule);
                   ((FPGA_SPI_buff_t*)&sFPGARegistersFbk)[u16IndexRd] = buffSPITransactionBufferFbk[u16IndexRd];
               }
               bSentData = true;
               FPGA_SPI_CS_SetHigh();
           }
        }
    }
    //DBG1_SetLow();
#endif
}


