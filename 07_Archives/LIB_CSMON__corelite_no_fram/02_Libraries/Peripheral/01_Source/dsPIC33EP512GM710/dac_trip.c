/* *****************************************************************************
 * File:   dac_trip.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 08 14 14:32
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
#include "boardcfg.h"

#if BOARDCFG_USE_DAC_TRIP


#include "dac_trip.h"
#include "mclib.h"

#include "control_common.h"

#include "hmlib.h"
#if USE_SPI_DAC
#include "spidac.h"
#endif
#if USE_I2C_DAC
#include "i2cdac.h"
#endif

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */
typedef enum
{
    #if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
    DAC_TRIP_PHASEA,
    DAC_TRIP_PHASEB,
    DAC_TRIP_PHASEC,
    #else
    DAC_TRIP_POSITIVE,
    DAC_TRIP_NEGATIVE,
    #endif
} eTripCurrent;
/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
#define TRIP_CURRENT_DEFAULT        1600.0
#define TRIP_VOLTAGE_DEFAULT        1200.0

#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0

#define TRIP_CURRENT_STACK1_PHASEA      TRIP_CURRENT_DEFAULT 
#define TRIP_CURRENT_STACK1_PHASEB      TRIP_CURRENT_DEFAULT 
#define TRIP_CURRENT_STACK1_PHASEC      TRIP_CURRENT_DEFAULT 

#define TRIP_CURRENT_STACK2_PHASEA      TRIP_CURRENT_DEFAULT 
#define TRIP_CURRENT_STACK2_PHASEB      TRIP_CURRENT_DEFAULT 
#define TRIP_CURRENT_STACK2_PHASEC      TRIP_CURRENT_DEFAULT 
 
#define TRIP_VOLTAGE_STACK1_MEASUR      TRIP_VOLTAGE_DEFAULT
#define TRIP_VOLTAGE_STACK2_MEASUR      TRIP_VOLTAGE_DEFAULT

#define TRIP_CURRENT_STACK1_FEEDBK      TRIP_CURRENT_DEFAULT 
#define TRIP_CURRENT_STACK2_FEEDBK      TRIP_CURRENT_DEFAULT

#else

#define TRIP_CURRENT_STACK1_POSITIVE    TRIP_CURRENT_DEFAULT 
#define TRIP_CURRENT_STACK1_NEGATIVE    (0 - TRIP_CURRENT_DEFAULT) 

#define TRIP_CURRENT_STACK2_POSITIVE    TRIP_CURRENT_DEFAULT 
#define TRIP_CURRENT_STACK2_NEGATIVE    (0 - TRIP_CURRENT_DEFAULT)

#define TRIP_VOLTAGE_POSITIVE           TRIP_VOLTAGE_DEFAULT
#define TRIP_VOLTAGE_NEGATIVE           (0 - TRIP_VOLTAGE_DEFAULT)

//#define TRIP_CURRENT_STACK1_FEEDBK      TRIP_CURRENT_DEFAULT 
//#define TRIP_CURRENT_STACK2_FEEDBK      TRIP_CURRENT_DEFAULT

#endif
 
#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
 int16_t TripCurrentStack1[DAC_TRIP_CURRENT_ONE_STACK_COUNT] = 
{
    MCLIB_s16d(TRIP_CURRENT_STACK1_PHASEA),
    MCLIB_s16d(TRIP_CURRENT_STACK1_PHASEB),
    MCLIB_s16d(TRIP_CURRENT_STACK1_PHASEC),
#else
uint16_t TripCurrentStack1[DAC_TRIP_CURRENT_ONE_STACK_COUNT] = 
{
    MCLIB_s16d(TRIP_CURRENT_STACK1_POSITIVE),
    MCLIB_s16d(TRIP_CURRENT_STACK1_NEGATIVE),
#endif
};
#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
 int16_t TripCurrentStack2[DAC_TRIP_CURRENT_ONE_STACK_COUNT] = 
{
    MCLIB_s16d(TRIP_CURRENT_STACK2_PHASEA),
    MCLIB_s16d(TRIP_CURRENT_STACK2_PHASEB),
    MCLIB_s16d(TRIP_CURRENT_STACK2_PHASEC),
#else
uint16_t TripCurrentStack2[DAC_TRIP_CURRENT_ONE_STACK_COUNT] = 
{
    MCLIB_s16d(TRIP_CURRENT_STACK2_POSITIVE),
    MCLIB_s16d(TRIP_CURRENT_STACK2_NEGATIVE),
#endif
};

#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
 int16_t TripVoltageStack1 = MCLIB_s16d(TRIP_VOLTAGE_STACK1_MEASUR);
 int16_t TripVoltageStack2 = MCLIB_s16d(TRIP_VOLTAGE_STACK2_MEASUR);
#else
uint16_t TripVoltageStack1 = MCLIB_s16d(TRIP_VOLTAGE_POSITIVE);
uint16_t TripVoltageStack2 = MCLIB_s16d(TRIP_VOLTAGE_NEGATIVE);
#endif

#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
 int16_t TripCFBKStack1 = MCLIB_s16d(TRIP_CURRENT_STACK1_FEEDBK);
 int16_t TripCFBKStack2 = MCLIB_s16d(TRIP_CURRENT_STACK2_FEEDBK);
#else
#endif

 
#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
 int16_t TripCurrentStack1Offset[DAC_TRIP_CURRENT_ONE_STACK_COUNT] = 
{
    0,
    0,
    0,
#else
uint16_t TripCurrentStack1Offset[DAC_TRIP_CURRENT_ONE_STACK_COUNT] = 
{
    MCLIB_u16From_u32(DACResultToFromDeciAmperesOffset),
    MCLIB_u16From_u32(DACResultToFromDeciAmperesOffset),
#endif
};
#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
 int16_t TripCurrentStack2Offset[DAC_TRIP_CURRENT_ONE_STACK_COUNT] = 
{
    0,
    0,
    0,
#else
uint16_t TripCurrentStack2Offset[DAC_TRIP_CURRENT_ONE_STACK_COUNT] = 
{
    MCLIB_u16From_u32(DACResultToFromDeciAmperesOffset),
    MCLIB_u16From_u32(DACResultToFromDeciAmperesOffset),
#endif
};


#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
 int16_t TripVoltageStack1Offset = 0;
 int16_t TripVoltageStack2Offset = 0;
#else
uint16_t TripVoltageStack1Offset = MCLIB_u16From_u32(DACResultToFromDeciVoltageOffset);
uint16_t TripVoltageStack2Offset = MCLIB_u16From_u32(DACResultToFromDeciVoltageOffset);
#endif

 
#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
 int16_t TripCFBKStack1Offset = 0;
 int16_t TripCFBKStack2Offset = 0;
 
 int16_t TripVoltage2Stack1Offset = 0;
 int16_t TripVoltage2Stack2Offset = 0;
#endif

 
 
 
 
#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
 int16_t TripCurrentStack1NoOffset[DAC_TRIP_CURRENT_ONE_STACK_COUNT] = 
{
    0,
    0,
    0,
#else
uint16_t TripCurrentStack1NoOffset[DAC_TRIP_CURRENT_ONE_STACK_COUNT] = 
{
    0,
    0,
#endif
};
#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
 int16_t TripCurrentStack2NoOffset[DAC_TRIP_CURRENT_ONE_STACK_COUNT] = 
{
    0,
    0,
    0,
#else
uint16_t TripCurrentStack2NoOffset[DAC_TRIP_CURRENT_ONE_STACK_COUNT] = 
{
    0,
    0,
#endif
};
 
 
#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
 int16_t TripVoltageStack1NoOffset = 0;
 int16_t TripVoltageStack2NoOffset = 0;
#else
uint16_t TripVoltageStack1NoOffset = 0;
uint16_t TripVoltageStack2NoOffset = 0;
#endif

#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
 int16_t TripCFBKStack1NoOffset = 0;
 int16_t TripCFBKStack2NoOffset = 0;

 int16_t TripVoltage2Stack1NoOffset = 0;
 int16_t TripVoltage2Stack2NoOffset = 0;
#endif

#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
 int16_t TripCurrentStack1Fbk[DAC_TRIP_CURRENT_ONE_STACK_COUNT];
 int16_t TripCurrentStack2Fbk[DAC_TRIP_CURRENT_ONE_STACK_COUNT];
 
 int16_t TripVoltageStack1Fbk;
 int16_t TripVoltageStack2Fbk;
#else
uint16_t TripCurrentStack1Fbk[DAC_TRIP_CURRENT_ONE_STACK_COUNT];
uint16_t TripCurrentStack2Fbk[DAC_TRIP_CURRENT_ONE_STACK_COUNT];
 
uint16_t TripVoltageStack1Fbk;
uint16_t TripVoltageStack2Fbk;
#endif

#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
 int16_t TripCFBKStack1Fbk;
 int16_t TripCFBKStack2Fbk;
#endif
 

uint16_t TripCurrentStack1RAW[DAC_TRIP_CURRENT_ONE_STACK_COUNT] = 
{
#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
    (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(TRIP_CURRENT_STACK1_PHASEA) * DeciAmperesToDACResultMultiplier),
    (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(TRIP_CURRENT_STACK1_PHASEB) * DeciAmperesToDACResultMultiplier),
    (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(TRIP_CURRENT_STACK1_PHASEC) * DeciAmperesToDACResultMultiplier),
#else
    (uint16_t)(MCLIB_s16d(TRIP_CURRENT_STACK1_POSITIVE) * DeciAmperesToDACResultMultiplier),
    (uint16_t)(MCLIB_s16d(TRIP_CURRENT_STACK1_NEGATIVE) * DeciAmperesToDACResultMultiplier),
#endif
};
uint16_t TripCurrentStack2RAW[DAC_TRIP_CURRENT_ONE_STACK_COUNT] = 
{
#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
    (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(TRIP_CURRENT_STACK2_PHASEA) * DeciAmperesToDACResultMultiplier),
    (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(TRIP_CURRENT_STACK2_PHASEB) * DeciAmperesToDACResultMultiplier),
    (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(TRIP_CURRENT_STACK2_PHASEC) * DeciAmperesToDACResultMultiplier),
#else
    (uint16_t)(MCLIB_s16d(TRIP_CURRENT_STACK2_POSITIVE) * DeciAmperesToDACResultMultiplier),
    (uint16_t)(MCLIB_s16d(TRIP_CURRENT_STACK2_NEGATIVE) * DeciAmperesToDACResultMultiplier),
#endif
};

#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
uint16_t TripVoltageStack1RAW = (uint16_t)(DACResultToFromDeciVoltageOffset + MCLIB_s16d(TRIP_VOLTAGE_STACK1_MEASUR) * DeciVoltageToDACResultMultiplier);
uint16_t TripVoltageStack2RAW = (uint16_t)(DACResultToFromDeciVoltageOffset + MCLIB_s16d(TRIP_VOLTAGE_STACK2_MEASUR) * DeciVoltageToDACResultMultiplier);
#else
uint16_t TripVoltageStack1RAW = (uint16_t)(MCLIB_s16d(TRIP_VOLTAGE_POSITIVE) * DeciVoltageToDACResultMultiplier);
uint16_t TripVoltageStack2RAW = (uint16_t)(MCLIB_s16d(TRIP_VOLTAGE_NEGATIVE) * DeciVoltageToDACResultMultiplier);
#endif


#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
uint16_t TripCFBKStack1RAW = (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(TRIP_CURRENT_STACK1_FEEDBK) * DeciAmperesToDACResultMultiplier);
uint16_t TripCFBKStack2RAW = (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(TRIP_CURRENT_STACK2_FEEDBK) * DeciAmperesToDACResultMultiplier);
#else
#endif


uint16_t TripCurrentStack1RAWFbk[DAC_TRIP_CURRENT_ONE_STACK_COUNT] = 
{
#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
    (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(1.0) * DeciAmperesToDACResultMultiplier),
    (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(2.0) * DeciAmperesToDACResultMultiplier),
    (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(3.0) * DeciAmperesToDACResultMultiplier),
#else
#endif
};
uint16_t TripCurrentStack2RAWFbk[DAC_TRIP_CURRENT_ONE_STACK_COUNT] = 
{
#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
    (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(4.0) * DeciAmperesToDACResultMultiplier),
    (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(5.0) * DeciAmperesToDACResultMultiplier),
    (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(6.0) * DeciAmperesToDACResultMultiplier),
#else
#endif
};

#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
uint16_t TripVoltageStack1RAWFbk = (uint16_t)(DACResultToFromDeciVoltageOffset + MCLIB_s16d(7.0) * DeciVoltageToDACResultMultiplier);
uint16_t TripVoltageStack2RAWFbk = (uint16_t)(DACResultToFromDeciVoltageOffset + MCLIB_s16d(8.0) * DeciVoltageToDACResultMultiplier);
#else
uint16_t TripVoltageStack1RAWFbk;
uint16_t TripVoltageStack2RAWFbk;
#endif

#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
uint16_t TripCFBKStack1RAWFbk = (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(7.0) * DeciAmperesToDACResultMultiplier);
uint16_t TripCFBKStack2RAWFbk = (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(8.0) * DeciAmperesToDACResultMultiplier);
#else
#endif



/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */



void DAC_TRIP_vSetOverVoltageOverCurrentLevelsDAC(void)
{
    
#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
    TripCurrentStack1NoOffset[DAC_TRIP_PHASEA] = TripCurrentStack1[DAC_TRIP_PHASEA] + TripCurrentStack1Offset[DAC_TRIP_PHASEA];
    TripCurrentStack1NoOffset[DAC_TRIP_PHASEB] = TripCurrentStack1[DAC_TRIP_PHASEB] + TripCurrentStack1Offset[DAC_TRIP_PHASEB];
    TripCurrentStack1NoOffset[DAC_TRIP_PHASEC] = TripCurrentStack1[DAC_TRIP_PHASEC] + TripCurrentStack1Offset[DAC_TRIP_PHASEC];
    
    TripCurrentStack2NoOffset[DAC_TRIP_PHASEA] = TripCurrentStack2[DAC_TRIP_PHASEA] + TripCurrentStack2Offset[DAC_TRIP_PHASEA];
    TripCurrentStack2NoOffset[DAC_TRIP_PHASEB] = TripCurrentStack2[DAC_TRIP_PHASEB] + TripCurrentStack2Offset[DAC_TRIP_PHASEB];
    TripCurrentStack2NoOffset[DAC_TRIP_PHASEC] = TripCurrentStack2[DAC_TRIP_PHASEC] + TripCurrentStack2Offset[DAC_TRIP_PHASEC];
    
    TripVoltageStack1NoOffset = TripVoltageStack1 - TripVoltageStack1Offset;
    TripVoltageStack2NoOffset = TripVoltageStack2 - TripVoltageStack2Offset;
    
    TripCFBKStack1NoOffset = TripCFBKStack1 - TripCFBKStack1Offset;
    TripCFBKStack2NoOffset = TripCFBKStack2 - TripCFBKStack2Offset;
    
    TripCurrentStack1RAW[0] = MCLIB_s16Mul_s16u32((TripCurrentStack1NoOffset[0] - MCLIB_s16From_u32(sControlConfig.u32DACResultToFromDeciAmperesOffset)), sControlConfig.u32DeciAmperesToDACResultMultiplier);
    TripCurrentStack1RAW[1] = MCLIB_s16Mul_s16u32((TripCurrentStack1NoOffset[1] - MCLIB_s16From_u32(sControlConfig.u32DACResultToFromDeciAmperesOffset)), sControlConfig.u32DeciAmperesToDACResultMultiplier);
    TripCurrentStack1RAW[2] = MCLIB_s16Mul_s16u32((TripCurrentStack1NoOffset[2] - MCLIB_s16From_u32(sControlConfig.u32DACResultToFromDeciAmperesOffset)), sControlConfig.u32DeciAmperesToDACResultMultiplier);

    TripCurrentStack2RAW[0] = MCLIB_s16Mul_s16u32((TripCurrentStack2NoOffset[0] - MCLIB_s16From_u32(sControlConfig.u32DACResultToFromDeciAmperesOffset)), sControlConfig.u32DeciAmperesToDACResultMultiplier);
    TripCurrentStack2RAW[1] = MCLIB_s16Mul_s16u32((TripCurrentStack2NoOffset[1] - MCLIB_s16From_u32(sControlConfig.u32DACResultToFromDeciAmperesOffset)), sControlConfig.u32DeciAmperesToDACResultMultiplier);
    TripCurrentStack2RAW[2] = MCLIB_s16Mul_s16u32((TripCurrentStack2NoOffset[2] - MCLIB_s16From_u32(sControlConfig.u32DACResultToFromDeciAmperesOffset)), sControlConfig.u32DeciAmperesToDACResultMultiplier);
    
    TripVoltageStack1RAW    = MCLIB_s16Mul_s16u32((TripVoltageStack1NoOffset    - MCLIB_s16From_u32(sControlConfig.u32DACResultToFromDeciVoltageOffset)), sControlConfig.u32DeciVoltsToDACResultMultiplier);
    TripVoltageStack2RAW    = MCLIB_s16Mul_s16u32((TripVoltageStack2NoOffset    - MCLIB_s16From_u32(sControlConfig.u32DACResultToFromDeciVoltageOffset)), sControlConfig.u32DeciVoltsToDACResultMultiplier);
    
    TripCFBKStack1RAW       = MCLIB_s16Mul_s16u32((TripCFBKStack1NoOffset       - MCLIB_s16From_u32(sControlConfig.u32DACResultToFromDeciAmperesOffset)), sControlConfig.u32DeciAmperesToDACResultMultiplier);
    TripCFBKStack2RAW       = MCLIB_s16Mul_s16u32((TripCFBKStack2NoOffset       - MCLIB_s16From_u32(sControlConfig.u32DACResultToFromDeciAmperesOffset)), sControlConfig.u32DeciAmperesToDACResultMultiplier);   
#else
    TripCurrentStack1NoOffset[DAC_TRIP_POSITIVE] = TripCurrentStack1[DAC_TRIP_POSITIVE] + TripCurrentStack1Offset[DAC_TRIP_POSITIVE];
    TripCurrentStack1NoOffset[DAC_TRIP_NEGATIVE] = TripCurrentStack1[DAC_TRIP_NEGATIVE] + TripCurrentStack1Offset[DAC_TRIP_NEGATIVE];
    
    TripCurrentStack2NoOffset[DAC_TRIP_POSITIVE] = TripCurrentStack2[DAC_TRIP_POSITIVE] + TripCurrentStack2Offset[DAC_TRIP_POSITIVE];
    TripCurrentStack2NoOffset[DAC_TRIP_NEGATIVE] = TripCurrentStack2[DAC_TRIP_NEGATIVE] + TripCurrentStack2Offset[DAC_TRIP_NEGATIVE];  
    
    TripVoltageStack1NoOffset = TripVoltageStack1 + TripVoltageStack1Offset;
    TripVoltageStack2NoOffset = TripVoltageStack2 + TripVoltageStack2Offset;
    
    TripCurrentStack1RAW[0] = MCLIB_u16Mul_u16u32(TripCurrentStack1NoOffset[0], sControlConfig.u32DeciAmperesToDACResultMultiplier);
    TripCurrentStack1RAW[1] = MCLIB_u16Mul_u16u32(TripCurrentStack1NoOffset[1], sControlConfig.u32DeciAmperesToDACResultMultiplier);

    TripCurrentStack2RAW[0] = MCLIB_u16Mul_u16u32(TripCurrentStack2NoOffset[0], sControlConfig.u32DeciAmperesToDACResultMultiplier);
    TripCurrentStack2RAW[1] = MCLIB_u16Mul_u16u32(TripCurrentStack2NoOffset[1], sControlConfig.u32DeciAmperesToDACResultMultiplier);
    
    TripVoltageStack1RAW    = MCLIB_u16Mul_u16u32(TripVoltageStack1NoOffset, sControlConfig.u32DeciVoltsToDACResultMultiplier);
    TripVoltageStack2RAW    = MCLIB_u16Mul_u16u32(TripVoltageStack2NoOffset, sControlConfig.u32DeciVoltsToDACResultMultiplier);
#endif

    
    
#if USE_SPI_DAC
    SPIDAC_vSetChannel(SPIDAC_TRIP_CURR_STACK1_PHASEA, TripCurrentStack1RAW[0]);
    SPIDAC_vSetChannel(SPIDAC_TRIP_CURR_STACK1_PHASEB, TripCurrentStack1RAW[1]);
    SPIDAC_vSetChannel(SPIDAC_TRIP_CURR_STACK1_PHASEC, TripCurrentStack1RAW[2]);
    
    SPIDAC_vSetChannel(SPIDAC_TRIP_CURR_STACK2_PHASEA, TripCurrentStack2RAW[0]);
    SPIDAC_vSetChannel(SPIDAC_TRIP_CURR_STACK2_PHASEB, TripCurrentStack2RAW[1]);
    SPIDAC_vSetChannel(SPIDAC_TRIP_CURR_STACK2_PHASEC, TripCurrentStack2RAW[2]);
    
    SPIDAC_vSetChannel(SPIDAC_TRIP_VOLT_STACK1_CHANNEL, TripVoltageStack1RAW);
    SPIDAC_vSetChannel(SPIDAC_TRIP_VOLT_STACK2_CHANNEL, TripVoltageStack2RAW);
    
    SPIDAC_vSetChannel(SPIDAC_TRIP_CFBK_STACK1_CHANNEL, TripCFBKStack1RAW);
    SPIDAC_vSetChannel(SPIDAC_TRIP_CFBK_STACK2_CHANNEL, TripCFBKStack2RAW);
#else
    if (I2CDAC_bUpdating() == false)
    {
        I2CDAC_vSetChannel(I2CDAC_TRIP_CURR_STACK1_POSITIVE, TripCurrentStack1RAW[0]);
        I2CDAC_vSetChannel(I2CDAC_TRIP_CURR_STACK1_NEGATIVE, TripCurrentStack1RAW[1]);
        I2CDAC_vSetChannel(I2CDAC_TRIP_CURR_STACK2_POSITIVE, TripCurrentStack2RAW[0]);
        I2CDAC_vSetChannel(I2CDAC_TRIP_CURR_STACK2_NEGATIVE, TripCurrentStack2RAW[1]);
        I2CDAC_vSetChannel(I2CDAC_TRIP_VOLT_DCLINK_POSITIVE, TripVoltageStack1RAW);
        I2CDAC_vSetChannel(I2CDAC_TRIP_VOLT_DCLINK_NEGATIVE, TripVoltageStack2RAW);
        I2CDAC_vUpdate();
    }
#endif
}


void DAC_TRIP_vGetOverVoltageOverCurrentLevelsDAC(void)
{
    
#if USE_SPI_DAC
    
    (void)SPIDAC_vGetChannel(SPIDAC_TRIP_CURR_STACK1_PHASEA, &TripCurrentStack1RAWFbk[0]);
    (void)SPIDAC_vGetChannel(SPIDAC_TRIP_CURR_STACK1_PHASEB, &TripCurrentStack1RAWFbk[1]);
    (void)SPIDAC_vGetChannel(SPIDAC_TRIP_CURR_STACK1_PHASEC, &TripCurrentStack1RAWFbk[2]);
    
    (void)SPIDAC_vGetChannel(SPIDAC_TRIP_CURR_STACK2_PHASEA, &TripCurrentStack2RAWFbk[0]);
    (void)SPIDAC_vGetChannel(SPIDAC_TRIP_CURR_STACK2_PHASEB, &TripCurrentStack2RAWFbk[1]);
    (void)SPIDAC_vGetChannel(SPIDAC_TRIP_CURR_STACK2_PHASEC, &TripCurrentStack2RAWFbk[2]);
    
    (void)SPIDAC_vGetChannel(SPIDAC_TRIP_VOLT_STACK1_CHANNEL,&TripVoltageStack1RAWFbk);
    (void)SPIDAC_vGetChannel(SPIDAC_TRIP_VOLT_STACK2_CHANNEL,&TripVoltageStack2RAWFbk);
    
    (void)SPIDAC_vGetChannel(SPIDAC_TRIP_CFBK_STACK1_CHANNEL,&TripCFBKStack1RAWFbk);
    (void)SPIDAC_vGetChannel(SPIDAC_TRIP_CFBK_STACK2_CHANNEL,&TripCFBKStack2RAWFbk);
    
#else
    {
        {
            I2CDAC_vGetChannel(I2CDAC_TRIP_CURR_STACK1_POSITIVE, &TripCurrentStack1RAWFbk[0]);
            I2CDAC_vGetChannel(I2CDAC_TRIP_CURR_STACK1_NEGATIVE, &TripCurrentStack1RAWFbk[1]);
            I2CDAC_vGetChannel(I2CDAC_TRIP_CURR_STACK2_POSITIVE, &TripCurrentStack2RAWFbk[0]);
            I2CDAC_vGetChannel(I2CDAC_TRIP_CURR_STACK2_NEGATIVE, &TripCurrentStack2RAWFbk[1]);
            I2CDAC_vGetChannel(I2CDAC_TRIP_VOLT_DCLINK_POSITIVE, &TripVoltageStack1RAWFbk);
            I2CDAC_vGetChannel(I2CDAC_TRIP_VOLT_DCLINK_NEGATIVE, &TripVoltageStack2RAWFbk);
        }
    }

            
#endif
    
#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
    TripCurrentStack1Fbk[0] = MCLIB_s16Mul_s16u32(TripCurrentStack1RAWFbk[0], sControlConfig.u32DACResultToDeciAmperesMultiplier) + MCLIB_s16From_u32(sControlConfig.u32DACResultToFromDeciAmperesOffset);
    TripCurrentStack1Fbk[1] = MCLIB_s16Mul_s16u32(TripCurrentStack1RAWFbk[1], sControlConfig.u32DACResultToDeciAmperesMultiplier) + MCLIB_s16From_u32(sControlConfig.u32DACResultToFromDeciAmperesOffset);
    TripCurrentStack1Fbk[2] = MCLIB_s16Mul_s16u32(TripCurrentStack1RAWFbk[2], sControlConfig.u32DACResultToDeciAmperesMultiplier) + MCLIB_s16From_u32(sControlConfig.u32DACResultToFromDeciAmperesOffset);
    
    TripCurrentStack2Fbk[0] = MCLIB_s16Mul_s16u32(TripCurrentStack2RAWFbk[0], sControlConfig.u32DACResultToDeciAmperesMultiplier) + MCLIB_s16From_u32(sControlConfig.u32DACResultToFromDeciAmperesOffset);
    TripCurrentStack2Fbk[1] = MCLIB_s16Mul_s16u32(TripCurrentStack2RAWFbk[1], sControlConfig.u32DACResultToDeciAmperesMultiplier) + MCLIB_s16From_u32(sControlConfig.u32DACResultToFromDeciAmperesOffset);
    TripCurrentStack2Fbk[2] = MCLIB_s16Mul_s16u32(TripCurrentStack2RAWFbk[2], sControlConfig.u32DACResultToDeciAmperesMultiplier) + MCLIB_s16From_u32(sControlConfig.u32DACResultToFromDeciAmperesOffset);
    
    TripVoltageStack1Fbk    = MCLIB_s16Mul_s16u32(TripVoltageStack1RAWFbk   , sControlConfig.u32DACResultToDeciVoltageMultiplier) + MCLIB_s16From_u32(sControlConfig.u32DACResultToFromDeciVoltageOffset);
    TripVoltageStack2Fbk    = MCLIB_s16Mul_s16u32(TripVoltageStack2RAWFbk   , sControlConfig.u32DACResultToDeciVoltageMultiplier) + MCLIB_s16From_u32(sControlConfig.u32DACResultToFromDeciVoltageOffset);
    
    TripCFBKStack1Fbk    = MCLIB_s16Mul_s16u32(TripCFBKStack1RAWFbk   , sControlConfig.u32DACResultToDeciAmperesMultiplier) + MCLIB_s16From_u32(sControlConfig.u32DACResultToFromDeciAmperesOffset);
    TripCFBKStack2Fbk    = MCLIB_s16Mul_s16u32(TripCFBKStack2RAWFbk   , sControlConfig.u32DACResultToDeciAmperesMultiplier) + MCLIB_s16From_u32(sControlConfig.u32DACResultToFromDeciAmperesOffset);
#else
    TripCurrentStack1Fbk[0] = MCLIB_s16Mul_s16u32(TripCurrentStack1RAWFbk[0], sControlConfig.u32DACResultToDeciAmperesMultiplier) - MCLIB_u16From_u32(sControlConfig.u32DACResultToFromDeciAmperesOffset);
    TripCurrentStack1Fbk[1] = MCLIB_s16Mul_s16u32(TripCurrentStack1RAWFbk[1], sControlConfig.u32DACResultToDeciAmperesMultiplier) - MCLIB_u16From_u32(sControlConfig.u32DACResultToFromDeciAmperesOffset);
    
    TripCurrentStack2Fbk[0] = MCLIB_s16Mul_s16u32(TripCurrentStack2RAWFbk[0], sControlConfig.u32DACResultToDeciAmperesMultiplier) - MCLIB_u16From_u32(sControlConfig.u32DACResultToFromDeciAmperesOffset);
    TripCurrentStack2Fbk[1] = MCLIB_s16Mul_s16u32(TripCurrentStack2RAWFbk[1], sControlConfig.u32DACResultToDeciAmperesMultiplier) - MCLIB_u16From_u32(sControlConfig.u32DACResultToFromDeciAmperesOffset);
    
    TripVoltageStack1Fbk    = MCLIB_s16Mul_s16u32(TripVoltageStack1RAWFbk   , sControlConfig.u32DACResultToDeciVoltageMultiplier) - MCLIB_u16From_u32(sControlConfig.u32DACResultToFromDeciVoltageOffset);
    TripVoltageStack2Fbk    = MCLIB_s16Mul_s16u32(TripVoltageStack2RAWFbk   , sControlConfig.u32DACResultToDeciVoltageMultiplier) - MCLIB_u16From_u32(sControlConfig.u32DACResultToFromDeciVoltageOffset);
#endif
}

#endif /* BOARDCFG_USE_DAC_TRIP */