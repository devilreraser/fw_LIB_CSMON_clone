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
#define TRIP_CURRENT_DEFAULT        1600.0
#define TRIP_VOLTAGE_DEFAULT        1200.0

#define TRIP_CURRENT_STACK1_PHASEA  TRIP_CURRENT_DEFAULT 
#define TRIP_CURRENT_STACK1_PHASEB  TRIP_CURRENT_DEFAULT 
#define TRIP_CURRENT_STACK1_PHASEC  TRIP_CURRENT_DEFAULT 

#define TRIP_CURRENT_STACK2_PHASEA  TRIP_CURRENT_DEFAULT 
#define TRIP_CURRENT_STACK2_PHASEB  TRIP_CURRENT_DEFAULT 
#define TRIP_CURRENT_STACK2_PHASEC  TRIP_CURRENT_DEFAULT 
 
#define TRIP_VOLTAGE_STACK1_MEASUR  TRIP_VOLTAGE_DEFAULT
#define TRIP_VOLTAGE_STACK2_MEASUR  TRIP_VOLTAGE_DEFAULT

#define TRIP_CURRENT_STACK1_FEEDBK  TRIP_CURRENT_DEFAULT 
#define TRIP_CURRENT_STACK2_FEEDBK  TRIP_CURRENT_DEFAULT
 
 int16_t TripCurrentStack1[3] = 
{
    MCLIB_s16d(TRIP_CURRENT_STACK1_PHASEA),
    MCLIB_s16d(TRIP_CURRENT_STACK1_PHASEB),
    MCLIB_s16d(TRIP_CURRENT_STACK1_PHASEC),
};
 int16_t TripCurrentStack2[3] = 
{
    MCLIB_s16d(TRIP_CURRENT_STACK2_PHASEA),
    MCLIB_s16d(TRIP_CURRENT_STACK2_PHASEB),
    MCLIB_s16d(TRIP_CURRENT_STACK2_PHASEC),
};
 int16_t TripVoltageStack1 = MCLIB_s16d(TRIP_VOLTAGE_STACK1_MEASUR);
 int16_t TripVoltageStack2 = MCLIB_s16d(TRIP_VOLTAGE_STACK2_MEASUR);

 int16_t TripCFBKStack1 = MCLIB_s16d(TRIP_CURRENT_STACK1_FEEDBK);
 int16_t TripCFBKStack2 = MCLIB_s16d(TRIP_CURRENT_STACK2_FEEDBK);

 
 int16_t TripCurrentStack1Offset[3] = 
{
    0,
    0,
    0,
};
 int16_t TripCurrentStack2Offset[3] = 
{
    0,
    0,
    0,
};
 int16_t TripVoltageStack1Offset = 0;
 int16_t TripVoltageStack2Offset = 0;

 int16_t TripCFBKStack1Offset = 0;
 int16_t TripCFBKStack2Offset = 0;

 
 
 int16_t TripVoltage2Stack1Offset = 0;
 int16_t TripVoltage2Stack2Offset = 0;


 
 
 
 
 int16_t TripCurrentStack1NoOffset[3] = 
{
    0,
    0,
    0,
};
 int16_t TripCurrentStack2NoOffset[3] = 
{
    0,
    0,
    0,
};
 int16_t TripVoltageStack1NoOffset = 0;
 int16_t TripVoltageStack2NoOffset = 0;

 int16_t TripCFBKStack1NoOffset = 0;
 int16_t TripCFBKStack2NoOffset = 0;


 
 int16_t TripVoltage2Stack1NoOffset = 0;
 int16_t TripVoltage2Stack2NoOffset = 0;


 
 int16_t TripCurrentStack1Fbk[3] = 
{
    MCLIB_s16d(10.0),
    MCLIB_s16d(20.0),
    MCLIB_s16d(30.0),
};
 int16_t TripCurrentStack2Fbk[3] = 
{
    MCLIB_s16d(40.0),
    MCLIB_s16d(50.0),
    MCLIB_s16d(60.0),
};
 int16_t TripVoltageStack1Fbk = MCLIB_s16d(70.0);
 int16_t TripVoltageStack2Fbk = MCLIB_s16d(80.0);

 int16_t TripCFBKStack1Fbk = MCLIB_s16d(90.0);
 int16_t TripCFBKStack2Fbk = MCLIB_s16d(95.0);

uint16_t TripCurrentStack1RAW[3] = 
{
    (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(TRIP_CURRENT_STACK1_PHASEA) * DeciAmperesToDACResultMultiplier),
    (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(TRIP_CURRENT_STACK1_PHASEB) * DeciAmperesToDACResultMultiplier),
    (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(TRIP_CURRENT_STACK1_PHASEC) * DeciAmperesToDACResultMultiplier),
};
uint16_t TripCurrentStack2RAW[3] = 
{
    (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(TRIP_CURRENT_STACK2_PHASEA) * DeciAmperesToDACResultMultiplier),
    (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(TRIP_CURRENT_STACK2_PHASEB) * DeciAmperesToDACResultMultiplier),
    (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(TRIP_CURRENT_STACK2_PHASEC) * DeciAmperesToDACResultMultiplier),
};
uint16_t TripVoltageStack1RAW = (uint16_t)(DACResultToFromDeciVoltageOffset + MCLIB_s16d(TRIP_VOLTAGE_STACK1_MEASUR) * DeciVoltageToDACResultMultiplier);
uint16_t TripVoltageStack2RAW = (uint16_t)(DACResultToFromDeciVoltageOffset + MCLIB_s16d(TRIP_VOLTAGE_STACK2_MEASUR) * DeciVoltageToDACResultMultiplier);


uint16_t TripCFBKStack1RAW = (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(TRIP_CURRENT_STACK1_FEEDBK) * DeciAmperesToDACResultMultiplier);
uint16_t TripCFBKStack2RAW = (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(TRIP_CURRENT_STACK2_FEEDBK) * DeciAmperesToDACResultMultiplier);


uint16_t TripCurrentStack1RAWFbk[3] = 
{
    (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(1.0) * DeciAmperesToDACResultMultiplier),
    (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(2.0) * DeciAmperesToDACResultMultiplier),
    (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(3.0) * DeciAmperesToDACResultMultiplier),
};
uint16_t TripCurrentStack2RAWFbk[3] = 
{
    (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(4.0) * DeciAmperesToDACResultMultiplier),
    (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(5.0) * DeciAmperesToDACResultMultiplier),
    (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(6.0) * DeciAmperesToDACResultMultiplier),
};
uint16_t TripVoltageStack1RAWFbk = (uint16_t)(DACResultToFromDeciVoltageOffset + MCLIB_s16d(7.0) * DeciVoltageToDACResultMultiplier);
uint16_t TripVoltageStack2RAWFbk = (uint16_t)(DACResultToFromDeciVoltageOffset + MCLIB_s16d(8.0) * DeciVoltageToDACResultMultiplier);

uint16_t TripCFBKStack1RAWFbk = (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(7.0) * DeciAmperesToDACResultMultiplier);
uint16_t TripCFBKStack2RAWFbk = (uint16_t)(DACResultToFromDeciAmperesOffset + MCLIB_s16d(8.0) * DeciAmperesToDACResultMultiplier);



/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */



void DAC_TRIP_vSetOverVoltageOverCurrentLevelsDAC(void)
{
    
    TripCurrentStack1NoOffset[0] = TripCurrentStack1[0] + TripCurrentStack1Offset[0];
    TripCurrentStack1NoOffset[1] = TripCurrentStack1[1] + TripCurrentStack1Offset[1];
    TripCurrentStack1NoOffset[2] = TripCurrentStack1[2] + TripCurrentStack1Offset[2];

    TripCurrentStack2NoOffset[0] = TripCurrentStack2[0] + TripCurrentStack2Offset[0];
    TripCurrentStack2NoOffset[1] = TripCurrentStack2[1] + TripCurrentStack2Offset[1];
    TripCurrentStack2NoOffset[2] = TripCurrentStack2[2] + TripCurrentStack2Offset[2];
    
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
    
    TripCFBKStack1RAW    = MCLIB_s16Mul_s16u32((TripCFBKStack1NoOffset    - MCLIB_s16From_u32(sControlConfig.u32DACResultToFromDeciAmperesOffset)), sControlConfig.u32DeciAmperesToDACResultMultiplier);
    TripCFBKStack2RAW    = MCLIB_s16Mul_s16u32((TripCFBKStack2NoOffset    - MCLIB_s16From_u32(sControlConfig.u32DACResultToFromDeciAmperesOffset)), sControlConfig.u32DeciAmperesToDACResultMultiplier);
    
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
#endif
    
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
    
}

#endif /* BOARDCFG_USE_DAC_TRIP */