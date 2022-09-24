/* *****************************************************************************
 * File:   spidac.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 05 31 15:44
 * 
 * Description: SPI DAC
 * 
 **************************************************************************** */
#ifdef _WIN32
#pragma once
#endif // _WIN32

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#include "boardcfg.h"
#if USE_SPI_DAC      /* Global Module Disable */

#include "sys_drv.h"
#include "spidac.h"
#include "spi.h"


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
    uint16_t D9 : 1;
    uint16_t D8 : 1;
    uint16_t D7 : 1;
    uint16_t D6 : 1;
    uint16_t D5 : 1;
    uint16_t D4 : 1;
    uint16_t D3 : 1;
    uint16_t D2 : 1;
    uint16_t D1 : 1;
    uint16_t D0 : 1;
    
    uint16_t A3 : 1;
    uint16_t A2 : 1;
    uint16_t A1 : 1;
    uint16_t A0 : 1;
   
}SPIDAC_sDataBitsReversePinOpenHigh;

typedef struct
{
    uint16_t D0 : 1;
    uint16_t D1 : 1;
    uint16_t D2 : 1;
    uint16_t D3 : 1;
    uint16_t D4 : 1;
    uint16_t D5 : 1;
    uint16_t D6 : 1;
    uint16_t D7 : 1;
    uint16_t D8 : 1;
    uint16_t D9 : 1;
    
    uint16_t A3 : 1;
    uint16_t A2 : 1;
    uint16_t A1 : 1;
    uint16_t A0 : 1;
   
}SPIDAC_sDataBitsReversePinLow;

typedef struct
{
    uint16_t u10Data : 10;
    uint16_t u4Address : 4;
    uint16_t u2Dummy : 2;
    
}SPIDAC_sData;


typedef union
{
    uint16_t u16Data;
    SPIDAC_sData sData;
    SPIDAC_sDataBitsReversePinLow sBitsPinLow;
    SPIDAC_sDataBitsReversePinOpenHigh sBitsPinOpenHigh;
    
}SPIDAC_uData;


/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
SPI_eModule_t eDacSPIModule = DAC_SPI_MODULE;

SPI_ePrimaryDivider_t      ePriDividerDac = SPI_PRIMARY_DIVIDER_1_DIV_1;
SPI_eSecondaryDivider_t    eSecDividerDac = SPI_SECONDARY_DIVIDER_1_DIV_3;

uint32_t u32SPIDacHz = (uint32_t)(SPI_DAC_HZ);
uint16_t u16SPIPrescalerDac = SPI_PRESCALER(SPI_DAC_HZ);

SPIDAC_eState_t SPIDAC_eState = SPIDAC_STA_IDLE;
SPIDAC_eState_t SPIDAC_eRequest = SPIDAC_STA_REQ_UPDATE_ALL;

uint16_t SPIDAC_u16WriteDACDataCount;
uint16_t SPIDAC_u16WriteCounter;
uint16_t SPIDAC_u16ReadCounter;

uint16_t* pu16DACDataWr;
uint16_t* pu16DACDataRd;
uint16_t* pu16DACDataRdAll;




uint16_t SPIDAC_u16AnalogAddress[DAC_CHANNELS_USAGE] = 
{
    (SPIDAC_ADDRESS_AO1  << 10),
    (SPIDAC_ADDRESS_AO2  << 10),
    (SPIDAC_ADDRESS_AO3  << 10),
    (SPIDAC_ADDRESS_AO4  << 10),
    (SPIDAC_ADDRESS_AO5  << 10),
    (SPIDAC_ADDRESS_AO6  << 10),
    (SPIDAC_ADDRESS_AO7  << 10),
    (SPIDAC_ADDRESS_AO8  << 10),
    (SPIDAC_ADDRESS_AO9  << 10),
    (SPIDAC_ADDRESS_AO10 << 10),
    
};

bool SPIDAC_bValidFbk[DAC_CHANNELS_USAGE] = 
{
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,

    
};



uint16_t SPIDAC_u16AnalogDataReq[DAC_CHANNELS_USAGE] = 
{
    0b0000000001,
    0b0000000011,
    0b0000000111,
    0b0000001111,
    0b0000011111,
    0b0000111111,
    0b0001111111,
    0b0011111111,
    0b0111111111,
    0b1111111111,
    
};

uint16_t SPIDAC_u16AnalogReadFbk[DAC_CHANNELS_USAGE] = 
{
    1,2,3,4,5,6,7,8,9,10
};

uint16_t SPIDAC_u16AnalogAddrFbk[DAC_CHANNELS_USAGE] = 
{
    12,12,12,12,12,12,12,12,12,12
};

uint16_t SPIDAC_u16AnalogDataFbk[DAC_CHANNELS_USAGE] = 
{
    65,65,65,65,65,65,65,65,65,65
};

SPIDAC_uData au16DACDataReq[DAC_CHANNELS_USAGE] = 
{
    {(SPIDAC_ADDRESS_AO1  << 10) + 0b0000000000},
    {(SPIDAC_ADDRESS_AO2  << 10) + 0b0000000010},
    {(SPIDAC_ADDRESS_AO3  << 10) + 0b0000000110},
    {(SPIDAC_ADDRESS_AO4  << 10) + 0b0000001110},
    {(SPIDAC_ADDRESS_AO5  << 10) + 0b0000011110},
    {(SPIDAC_ADDRESS_AO6  << 10) + 0b0000111110},
    {(SPIDAC_ADDRESS_AO7  << 10) + 0b0001111110},
    {(SPIDAC_ADDRESS_AO8  << 10) + 0b0011111110},
    {(SPIDAC_ADDRESS_AO9  << 10) + 0b0111111110},
    {(SPIDAC_ADDRESS_AO10 << 10) + 0b1111111110},
};

SPIDAC_uData au16DACDataFbk[DAC_CHANNELS_USAGE] = 
{
    {0x100},
    {0x200},
    {0x300},
    {0x400},
    {0x500},
    {0x600},
    {0x700},
    {0x800},
    {0x900},
    {0xA00},
};

SPIDAC_uData au16DACDataAllFbk[DAC_CHANNELS_USAGE*2] = 
{
    {0x100},
    {0x200},
    {0x300},
    {0x400},
    {0x500},
    {0x600},
    {0x700},
    {0x800},
    {0x900},
    {0xA00},
    {0x100},
    {0x200},
    {0x300},
    {0x400},
    {0x500},
    {0x600},
    {0x700},
    {0x800},
    {0x900},
    {0xA00},
};


/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */
int16_t s16FindChannelPosition(SPIDAC_eSignalAddress_t eChannel);
uint16_t ReverseData(uint16_t u16Input);


/* *****************************************************************************
 * Functions
 **************************************************************************** */

/* *****************************************************************************
 * SPIDAC_vInit
 * 
 * Description: Module Initialization
 **************************************************************************** */

void SPIDAC_vInit(uint32_t u32Fcy)
{
    /* DAC DI must be triggered on the falling edge and is read on the rising edge of CLK - SPI Mode 3 */
    /* DAC DO triggers on Rising edge of CLK - SPI Mode 2 */
    #ifndef BOARDCFG_USE_MCC_SPI
    SPI_vInit(eDacSPIModule, SPI_DMA_NOT_USED);   /* used for the External DAC */
    #endif
    
    SPI_vCalcSPIPrescaler(u32SPIDacHz,u32Fcy, &u16SPIPrescalerDac);
    
    SPI_vInitSPIPrescalers(u16SPIPrescalerDac,&ePriDividerDac, &eSecDividerDac);
    
    /* SPI Receive with DMA Support Init */
    //ISR_vSetISRDMA(SPIMEM_vRxCompleteProcess, eModuleDMASPIRx);
    //SPI_vInitRxDMA(eDacSPIModule, eModuleDMASPIRx);
    
    /* SPI Transmit with DMA Support Init */
    //SPI_vInitTxDMA(eDacSPIModule, eModuleDMASPITx);
    SPI_vSetModeBitsCount(eDacSPIModule, SPI_MODE_16BITS);
    SPI_vSetFrequencyDivider(eDacSPIModule, ePriDividerDac, eSecDividerDac);
    
}


void SPIDAC_vChipSelect(SPI_eChipSelect_t eChipSelect)
{
    if (eChipSelect == SPI_CHIP_SELECT_ENABLE)
    {
        LD_DAC_SetLow();
    }
    else
    {
        LD_DAC_SetHigh();
    }
}


uint16_t ReverseData(uint16_t u16Input)
{
    SPIDAC_uData inputData;
    SPIDAC_uData outputData;
    uint16_t result;
    
    inputData.u16Data = u16Input;
    outputData.u16Data = u16Input;
    //(uint16_t)outputData = (uint16_t)inputData;
    outputData.sBitsPinOpenHigh.D0 = inputData.sBitsPinLow.D0;
    outputData.sBitsPinOpenHigh.D1 = inputData.sBitsPinLow.D1;
    outputData.sBitsPinOpenHigh.D2 = inputData.sBitsPinLow.D2;
    outputData.sBitsPinOpenHigh.D3 = inputData.sBitsPinLow.D3;
    outputData.sBitsPinOpenHigh.D4 = inputData.sBitsPinLow.D4;
    outputData.sBitsPinOpenHigh.D5 = inputData.sBitsPinLow.D5;
    outputData.sBitsPinOpenHigh.D6 = inputData.sBitsPinLow.D6;
    outputData.sBitsPinOpenHigh.D7 = inputData.sBitsPinLow.D7;
    outputData.sBitsPinOpenHigh.D8 = inputData.sBitsPinLow.D8;
    outputData.sBitsPinOpenHigh.D9 = inputData.sBitsPinLow.D9;
    
    result = (uint16_t)outputData.u16Data;
    
    return result;
}

void SPIDAC_vSetChannel(SPIDAC_eSignalAddress_t eChannel, uint16_t u16Value)
{
    int16_t index = s16FindChannelPosition(eChannel);
    if (index >= 0)
    {
        SPIDAC_bValidFbk[index] = false;
        SPIDAC_u16AnalogDataReq[index] = u16Value;
    }
    
}

int16_t s16FindChannelPosition(SPIDAC_eSignalAddress_t eChannel)
{
    int16_t s16Result = -1;
    uint16_t u16CompareChannelValue;
    uint16_t index;
    u16CompareChannelValue = (eChannel << 10);
    
    for (index = 0; index < DAC_CHANNELS_USAGE; index++)
    {
        if (SPIDAC_u16AnalogAddress[index] == u16CompareChannelValue)
        {
            s16Result = index;
            break;
        }
    }
    return s16Result;
}

bool SPIDAC_vGetChannel(SPIDAC_eSignalAddress_t eChannel, uint16_t* pu16Value)
{
    int16_t index = s16FindChannelPosition(eChannel);
    bool bResult = false;
    if (index >= 0)
    {
        if (SPIDAC_bValidFbk[index])
        {
            bResult = true;
            *pu16Value = SPIDAC_u16AnalogDataFbk[index];
        }
    }
    return bResult;
}


void SPIDAC_vProcess(void)
{
    uint16_t index;
    uint16_t data;
    
    switch(SPIDAC_eState)
    {
        case SPIDAC_STA_IDLE:
            if (SPIDAC_eRequest == SPIDAC_STA_REQ_UPDATE_ALL)
            {
                SPIDAC_eState = SPIDAC_STA_WRITE_DATA;
                
                SPIDAC_u16WriteDACDataCount = DAC_CHANNELS_USAGE*2; /* 20 writes for 10 channels */
                                
                for (index = 0; index < DAC_CHANNELS_USAGE; index++)
                {
                    data = SPIDAC_u16AnalogDataReq[index];
                    #if DAC_REVERSE_PIN_OPEN_HIGH == 1
                    data =  ReverseData(data);
                    #endif
                    au16DACDataReq[index].u16Data = SPIDAC_u16AnalogAddress[index] | data;
                }
                
                pu16DACDataWr = (uint16_t*)(&au16DACDataReq);
                pu16DACDataRd = (uint16_t*)(&au16DACDataFbk);
                pu16DACDataRdAll = (uint16_t*)(&au16DACDataAllFbk);
                
                SPIDAC_u16WriteCounter = 0;
                SPIDAC_u16ReadCounter = 0;
                
                /* assure start with empty SPI buffers - to do check if enough  */
                while (SPI_TxCompleted(eDacSPIModule) == false)
                {
                     ;
                }
                while (SPI_RxBufferEmpty(eDacSPIModule) == false)
                {
                    (void)SPI_Read(eDacSPIModule);
                }
            }
            break;
            
        #if DAC_SPI_MODE_3_WRITE_SPI_MODE_2_READ

        case SPIDAC_STA_WRITE_DATA:
            
            if((SPIDAC_u16WriteCounter & 1)  == 0)  /* First is Write with Mode 3 */
            {
                /* DAC Reads data (DI) in SPI Mode 3 */            
                SPI_vSetMode(eDacSPIModule, SPI_MODE_3);
            }
            else     /* First is Read with Mode 2 */
            {
                /* DAC Sends data (DO) in SPI Mode 2 (on the rising CLK) */            
                SPI_vSetMode(eDacSPIModule, SPI_MODE_2);
            }
            
            SPI_Write(eDacSPIModule, *pu16DACDataWr );
            SPIDAC_u16WriteCounter++;
            if((SPIDAC_u16WriteCounter & 1)  == 0)  /* Increase Pointer After Second Write */
            {
                pu16DACDataWr++;

            }
            SPIDAC_eState = SPIDAC_STA_SEND_DATA_WAIT;
            
            break;
            
        case SPIDAC_STA_SEND_DATA_WAIT:  
            
            if (SPI_RxBufferEmpty(eDacSPIModule) == false)
            {
                data = SPI_Read(eDacSPIModule);
                *pu16DACDataRdAll++ = data;
                SPIDAC_u16ReadCounter++;
                if((SPIDAC_u16ReadCounter & 1)  == 0)   /* Increase Pointer After Second Read */
                {
                    *pu16DACDataRd = data;
                    pu16DACDataRd++;
                }
                SPIDAC_eState = SPIDAC_STA_LOAD_DATA_START;
            }
            
            if (SPIDAC_u16ReadCounter == SPIDAC_u16WriteCounter)
            {
                #if TOGGLE_LD_ONLY_ON_FIRST_WRITE_COMPLETED
                if((SPIDAC_u16WriteCounter & 1)  == 1)  /* Set/Reset LD After First Write */
                {
                    SPIDAC_eState = SPIDAC_STA_LOAD_DATA_START;
                }
                else
                {
                    if ( (SPIDAC_u16WriteCounter >= SPIDAC_u16WriteDACDataCount)
                      && (SPIDAC_u16ReadCounter  >= SPIDAC_u16WriteDACDataCount))
                    {
                        SPIDAC_eState = SPIDAC_STA_WRITE_DATA_COMPLETED;
                    }
                    else
                    {
                        SPIDAC_eState = SPIDAC_STA_WRITE_DATA;
                    }
                }
                #else
                SPIDAC_eState = SPIDAC_STA_LOAD_DATA_START;
                #endif
            }
            
            break;
            
            
            
        #else
        case SPIDAC_STA_WRITE_DATA:
            
                        
            while (     (SPI_TxBufferFull(eDacSPIModule) == false)
                     && (SPIDAC_u16WriteCounter < SPIDAC_u16WriteDACDataCount)
                     //&& (SPIDAC_u16ReadCounter < SPIDAC_u16WriteDACDataCount)
                     
                       )
            {
            
                if (SPI_RxBufferEmpty(eDacSPIModule) == false)
                {
                    data = SPI_Read(eDacSPIModule);
                    *pu16DACDataRdAll++ = data;  
                    SPIDAC_u16ReadCounter++;
                    if((SPIDAC_u16ReadCounter & 1)  == 0)   /* Increase Pointer After Second Read */
                    {
                        *pu16DACDataRd = data;
                        pu16DACDataRd++;
                    }
                }
                
                if (SPI_TxBufferFull(eDacSPIModule) == false)
                {
                    SPI_Write(eDacSPIModule, *pu16DACDataWr );
                    SPIDAC_u16WriteCounter++;
                    if((SPIDAC_u16WriteCounter & 1)  == 0)  /* Increase Pointer After Second Write */
                    {
                        pu16DACDataWr++;
                        
                    }
                    SPIDAC_eState = SPIDAC_STA_SEND_DATA_WAIT;
                    break;
                }
                
            }
            
            
            
            break;
            
        case SPIDAC_STA_SEND_DATA_WAIT:  
            while (SPI_RxBufferEmpty(eDacSPIModule) == false)
            {
                data = SPI_Read(eDacSPIModule);
                *pu16DACDataRdAll++ = data;
                SPIDAC_u16ReadCounter++;
                if((SPIDAC_u16ReadCounter & 1)  == 0)   /* Increase Pointer After Second Read */
                {
                    *pu16DACDataRd = data;
                    pu16DACDataRd++;
                }
            }
            
            if (SPIDAC_u16ReadCounter == SPIDAC_u16WriteCounter)
            {
                #if TOGGLE_LD_ONLY_ON_FIRST_WRITE_COMPLETED
                if((SPIDAC_u16WriteCounter & 1)  == 1)  /* Set/Reset LD After First Write */
                {
                    SPIDAC_eState = SPIDAC_STA_LOAD_DATA_START;
                }
                else
                {
                    if ( (SPIDAC_u16WriteCounter >= SPIDAC_u16WriteDACDataCount)
                      && (SPIDAC_u16ReadCounter  >= SPIDAC_u16WriteDACDataCount))
                    {
                        SPIDAC_eState = SPIDAC_STA_WRITE_DATA_COMPLETED;
                    }
                    else
                    {
                        SPIDAC_eState = SPIDAC_STA_WRITE_DATA;
                    }
                }
                #else
                SPIDAC_eState = SPIDAC_STA_LOAD_DATA_START;
                #endif
            }
            
            break;
            
        #endif    

            
        case SPIDAC_STA_LOAD_DATA_START:  
            /* assure 50 nsec pulse available enable to disable */
            SPIDAC_vChipSelect(SPIDAC_LD_ENABLE);
            SPIDAC_eState = SPIDAC_STA_LOAD_DATA_END;
            break;
            
        case SPIDAC_STA_LOAD_DATA_END:  
            /* assure 50 nsec pulse available enable to disable */
            SPIDAC_vChipSelect(SPIDAC_LD_DISABLE);
            
            if ( (SPIDAC_u16WriteCounter >= SPIDAC_u16WriteDACDataCount)
              && (SPIDAC_u16ReadCounter  >= SPIDAC_u16WriteDACDataCount))
            {
                SPIDAC_eState = SPIDAC_STA_WRITE_DATA_COMPLETED;
            }
            else
            {
                SPIDAC_eState = SPIDAC_STA_WRITE_DATA;
            }
            
            
            break;
            
            
        case SPIDAC_STA_WRITE_DATA_COMPLETED:
            
            for (index = 0; index < DAC_CHANNELS_USAGE; index++)
            {
                data = au16DACDataFbk[index].u16Data;
                SPIDAC_u16AnalogReadFbk[index] = data;
                
                data >>= 2;
                
                #if DAC_REVERSE_PIN_OPEN_HIGH == 1
                data =  ReverseData(data);
                #endif

                SPIDAC_u16AnalogDataFbk[index] = data & ((1 << 10)-1);
                data >>= 10;
                SPIDAC_u16AnalogAddrFbk[index] = data << 10;
                
                if (SPIDAC_u16AnalogAddrFbk[index] == SPIDAC_u16AnalogAddress[index])
                {
                    SPIDAC_bValidFbk[index] = true;
                }
                
                
            }

            
            SPIDAC_eState = SPIDAC_STA_IDLE;
            break;
            
        default:
            break;
    }
    

}
#endif