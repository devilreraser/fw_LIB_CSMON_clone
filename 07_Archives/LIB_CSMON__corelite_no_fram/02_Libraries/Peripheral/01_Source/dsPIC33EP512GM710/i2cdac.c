/* *****************************************************************************
 * File:   i2cdac.c
 * Author: Dimitar Lilov
 *
 * Created on 2020 04 30 13:20
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
#include "i2cdac.h"
#include "boardcfg.h"
#include "i2cmem.h"
#include "pin_macro.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
I2CDAC_eCommandChannelAddress_t eTripChannel = DACx3608_CMD_DACA_DATA;
bool bUpdateDAC = false;

bool bReadFbkNeed = false;

bool bUpdateDACReq = false;
bool bValueUpdateNeeded = false;
bool bValueUpdateForce = false;
bool bUpdateDACDone = false;
bool bDenyReadFbk = false;
bool bUpdateDACReqExternal = true;

bool bReadDeviceID = false;
bool bUpdateDeviceConfig = false;
bool bReadChannel = false;
bool bWriteChannel = false;

I2CDAC_uDeviceConfigDACx3608_t uDeviceConfig;
I2CDAC_uDeviceConfigDACx3608_t uDeviceConfigByteSwap;
I2CDAC_uDeviceConfigDACx3608_t uDeviceConfigFbk;
I2CDAC_uDeviceConfigDACx3608_t uDeviceConfigFbkByteSwap;

I2CDAC_uStatusTriggerDACx3608_t uStatusTrigger;
I2CDAC_uStatusTriggerDACx3608_t uStatusTriggerByteSwap;
I2CDAC_uStatusTriggerDACx3608_t uStatusTriggerFbk;
I2CDAC_uStatusTriggerDACx3608_t uStatusTriggerFbkByteSwap;

I2CDAC_uDataDACx3608_t uChannelValues[I2CDAC_TRIP_COUNT];
I2CDAC_uDataDACx3608_t uChannelValuesOld[I2CDAC_TRIP_COUNT];
I2CDAC_uDataDACx3608_t uChannelValuesFbk[I2CDAC_TRIP_COUNT];
I2CDAC_uDataDACx3608_t uChannelValuesByteSwap[I2CDAC_TRIP_COUNT];
I2CDAC_uDataDACx3608_t uChannelValuesFbkByteSwap[I2CDAC_TRIP_COUNT];
uint16_t u16UpdateIndex;

uint16_t u16ValueUpdateNeededCount = 0;

I2CDAC_eCommandByteFBKPIC16_t aeChannelFeedbackAddress[I2CDAC_TRIP_COUNT] = 
{
    FBKPIC16_CMD_DACA_DATA,
    FBKPIC16_CMD_DACB_DATA,
    FBKPIC16_CMD_DACC_DATA,
    FBKPIC16_CMD_DACD_DATA,
    FBKPIC16_CMD_DACE_DATA,
    FBKPIC16_CMD_DACF_DATA,
}; 


uint8_t I2CDAC_eI2CModule = DAC_I2C_MODULE;

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */
bool I2CDAC_bReadDeny(void)
{
    bool bResult;
    bResult = bDenyReadFbk;
    return bResult;
}

bool I2CDAC_bUpdating(void)
{
    bool bResult;
    bResult = (bUpdateDAC || bUpdateDACReq);
    
    return bResult;
}

void I2CDAC_vSetUpdateReg(bool bInput)
{
    bUpdateDACReqExternal = bInput;
}

void I2CDAC_vSetDenyReadFbk(bool bInput)
{
    bDenyReadFbk = bInput;
}

void I2CDAC_vUpdate(void)
{
    if (bUpdateDAC == false)
    {
        bUpdateDACDone = false;
    }
    if (bUpdateDACReqExternal && (bDenyReadFbk == false))
    {
        if (bValueUpdateNeeded)
        {
            if (u16ValueUpdateNeededCount > 0)
            {
                u16ValueUpdateNeededCount--;
                bUpdateDACReq = true;
                
            }
            if (u16ValueUpdateNeededCount == 0)
            {
                bValueUpdateNeeded = false;
            }
        }
        if (bValueUpdateForce)
        {
            bValueUpdateNeeded = false;
            u16ValueUpdateNeededCount = 0;
            bUpdateDACReq = true;
        }
    }
}

void I2CDAC_vSetChannel(I2CDAC_eCommandChannelAddress_t eChannel, uint16_t u16Value)
{
    uint16_t u16Index = eChannel - DACx3608_CMD_DACA_DATA;
    
    
    
    if (u16Index < I2CDAC_TRIP_COUNT)
    {
        I2CDAC_uDataDACx3608_t uDataDACx3608;
        uDataDACx3608.u16Register = 0;
        uDataDACx3608.sData.u10Data = u16Value;
        uChannelValues[u16Index].u16Register = uDataDACx3608.u16Register ;
        if (uChannelValuesOld[u16Index].u16Register != uChannelValues[u16Index].u16Register)
        {
            uChannelValuesOld[u16Index].u16Register = uChannelValues[u16Index].u16Register;
            bValueUpdateNeeded = true;
            u16ValueUpdateNeededCount = 2;
        }
        
        uChannelValuesByteSwap[u16Index].au8Bytes[0] = uChannelValues[u16Index].au8Bytes[1] ;
        uChannelValuesByteSwap[u16Index].au8Bytes[1] = uChannelValues[u16Index].au8Bytes[0] ;
    }
}

void I2CDAC_vGetChannel(I2CDAC_eCommandChannelAddress_t eChannel, uint16_t* pu16Value)
{
    uint16_t u16Index = eChannel - DACx3608_CMD_DACA_DATA;
    
    if (u16Index < I2CDAC_TRIP_COUNT)
    {
        #if 0
        I2CDAC_uDataDACx3608_t uDataDACx3608;
        uChannelValuesFbk[u16Index].au8Bytes[0] = uChannelValuesFbkByteSwap[u16Index].au8Bytes[1] ;
        uChannelValuesFbk[u16Index].au8Bytes[1] = uChannelValuesFbkByteSwap[u16Index].au8Bytes[0] ;
        
        uDataDACx3608.u16Register = uChannelValuesFbk[u16Index].u16Register;
        *pu16Value = uDataDACx3608.sData.u10Data;
        #else
        uChannelValuesFbk[u16Index].au8Bytes[0] = uChannelValuesFbkByteSwap[u16Index].au8Bytes[0] ; /* PIC16 Feedback not swapped */
        uChannelValuesFbk[u16Index].au8Bytes[1] = uChannelValuesFbkByteSwap[u16Index].au8Bytes[1] ; /* PIC16 Feedback not swapped */
        *pu16Value = uChannelValuesFbk[u16Index].u16Register;  
        #endif
    }
    else
    {
        *pu16Value = 0;
    }
}

void I2CDAC_vSetChannels(void)
{
    if (bUpdateDAC == false)
    {
        if (bUpdateDACReq)
        {
            bUpdateDACReq = false;
            
            bReadDeviceID = true;
            bUpdateDeviceConfig = true;
            bUpdateDAC = true;
            u16UpdateIndex = 0;
            bReadChannel = false;
            bWriteChannel = false;
            
            uDeviceConfig.sDeviceConfig.bPDNA = 0;
            uDeviceConfig.sDeviceConfig.bPDNB = 0;
            uDeviceConfig.sDeviceConfig.bPDNC = 0;
            uDeviceConfig.sDeviceConfig.bPDND = 0;
            uDeviceConfig.sDeviceConfig.bPDNE = 0;
            uDeviceConfig.sDeviceConfig.bPDNF = 0;
            uDeviceConfig.sDeviceConfig.bPDNG = 0;
            uDeviceConfig.sDeviceConfig.bPDNH = 0;
            uDeviceConfig.sDeviceConfig.bPDNAll = 0;
            
            uDeviceConfigByteSwap.au8Bytes[0] = uDeviceConfig.au8Bytes[1];
            uDeviceConfigByteSwap.au8Bytes[1] = uDeviceConfig.au8Bytes[0];
            
            LOAD_DAC_SetHigh();
            //LOAD_DAC_SetLow();

        }
    }
    
    if (bUpdateDAC == true)
    {
        if (I2CMEM_bGetCmdStateModuleIdle(I2CDAC_eI2CModule))
        {
            
            if (bReadDeviceID)
            {
                    I2CMEM_vRead(I2CDAC_DEVICE_ADDRESS, DACx3608_CMD_STATUS_TRIGGER, (HMDRV_ADDR_PNTR_MODIFIER uint8_t*)&uStatusTriggerFbkByteSwap.u16Register , 2, 1, I2CDAC_eI2CModule);
                bReadDeviceID = false;
            }
            else if (bUpdateDeviceConfig)
            {
                if (bWriteChannel == false)
                {
                    I2CMEM_vWrite(I2CDAC_DEVICE_ADDRESS, DACx3608_CMD_DEVICE_CONFIG, (HMDRV_ADDR_PNTR_MODIFIER uint8_t*)&uDeviceConfigByteSwap.u16Register , 2, 1, I2CDAC_eI2CModule);
                    bWriteChannel = true;
                    bReadChannel = false;
                }
                else if (bReadChannel == false)
                {
                    I2CMEM_vRead(I2CDAC_DEVICE_ADDRESS, DACx3608_CMD_DEVICE_CONFIG, (HMDRV_ADDR_PNTR_MODIFIER uint8_t*)&uDeviceConfigFbkByteSwap.u16Register , 2, 1, I2CDAC_eI2CModule);
                    bReadChannel = true;
                    bWriteChannel = false;
                    bUpdateDeviceConfig = false;
                }
            }
            else if (u16UpdateIndex < I2CDAC_TRIP_COUNT)
            {
                I2CDAC_eCommandChannelAddress_t addr = u16UpdateIndex + DACx3608_CMD_DACA_DATA;
                I2CDAC_eCommandByteFBKPIC16_t addrFbk = aeChannelFeedbackAddress[u16UpdateIndex];
                if (bWriteChannel == false)
                {
                    I2CMEM_vWrite(I2CDAC_DEVICE_ADDRESS, addr, (HMDRV_ADDR_PNTR_MODIFIER uint8_t*)&uChannelValuesByteSwap[u16UpdateIndex].u16Register , 2, 1, I2CDAC_eI2CModule);
                    bWriteChannel = true;
                    bReadChannel = false;
                }
                else if (bReadChannel == false)
                {
                    I2CMEM_vRead(I2CDAC_DEVFBK_ADDRESS, addrFbk, (HMDRV_ADDR_PNTR_MODIFIER uint8_t*)&uChannelValuesFbkByteSwap[u16UpdateIndex].u16Register , 2, 1, I2CDAC_eI2CModule);
                    bReadChannel = true;
                    bWriteChannel = false;
                    u16UpdateIndex++;
                }
            }
            else 
            {
                bUpdateDACDone = true;
                bUpdateDAC = false;
                

                
                uDeviceConfigFbk.au8Bytes[0] = uDeviceConfigFbkByteSwap.au8Bytes[1] ;
                uDeviceConfigFbk.au8Bytes[1] = uDeviceConfigFbkByteSwap.au8Bytes[0] ;

                uStatusTriggerFbk.au8Bytes[0] = uStatusTriggerFbkByteSwap.au8Bytes[1] ;
                uStatusTriggerFbk.au8Bytes[1] = uStatusTriggerFbkByteSwap.au8Bytes[0] ;

                LOAD_DAC_SetLow();
                Nop();
                Nop();
                Nop();
                Nop();
                LOAD_DAC_SetHigh();
            }
        }
    }

}