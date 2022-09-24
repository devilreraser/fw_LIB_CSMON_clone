/* *****************************************************************************
 * File:   trip.c
 * Author: Dimitar Lilov
 *
 * Created on 2021 01 04 19:06
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
#include <stdint.h>
//#include "stdextend.h"
#include "boardcfg.h"
#include "trip.h"
#include "dac_drv.h"

#if USE_CONTROL
#include "control_common.h"
#else
#include "mclib.h"
/* CURRENT REFERENCE */
#define DACResultToFromDeciAmperesOffset \
        MCLIB_u32FractionalQ_16_16(DAC_RESULT_TO_AMP_IFBK_OFFS * 10)
#define DeciAmperesToDACResultMultiplier \
        MCLIB_u32FractionalQ_16_16(1.0 / (DAC_RESULT_TO_AMP_IFBK_MULT * 10))

/* VOLTAGE REFERENCE */
#define DACResultToFromDeciVoltageOffset \
        MCLIB_u32FractionalQ_16_16(DAC_RESULT_TO_VOLTS_DC_OFFS * 10)
#define DeciVoltageToDACResultMultiplier \
        MCLIB_u32FractionalQ_16_16(1.0 / (DAC_RESULT_TO_VOLTS_DC_MULT * 10))

#endif

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
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

/* from fpga_trip */
uControlTrip_t uTripRequest;
uControlTrip_t uTripFbkOC;     /* Read Data From FPGA */
uControlTrip_t uTripFbkOV_OCFE;     /* Read Data From FPGA */
uControlTrip_t uTripFbkOCFW;     /* Read Data From FPGA */
uControlTrip_t uControlTripFbkOC;     /* Read Data From FPGA */
uControlTrip_t uControlTripFbkOV_OCFE;     /* Read Data From FPGA */
uControlTrip_t uControlTripFbkOCFW;     /* Read Data From FPGA */

uFREGTrip_t     uFREGTrip = {0};


uOCTrip_t StatOCA   = {0xFF};
uOCTrip_t StatOCB   = {0xFF};
uOCTrip_t StatOCFE  = {0xFF};
uOCTrip_t StatOCFW  = {0xFF};
uOVTrip_t StatOV    = {0xFF};

uOCTrip_t MaskOCA   = {0xFF};
uOCTrip_t MaskOCB   = {0x00};
uOCTrip_t MaskOCFE  = {0x00};
uOCTrip_t MaskOCFW  = {0x00};
uOVTrip_t MaskOV    = {0x01};

uOCTrip_t MaskOCAFbk    = {0};
uOCTrip_t MaskOCBFbk    = {0};
uOCTrip_t MaskOCFEFbk   = {0};
uOCTrip_t MaskOCFWFbk   = {0};
uOVTrip_t MaskOVFbk     = {0};

bool bSetupTripMaskConfiguration = 0;

uint8_t u8PMPConfigurationUnlockKeyTRIP = PMP_CONFIGURATION_UNLOCK_KEY;
uint8_t u8PMPConfigurationLockKeyTRIP   = 0x00;


/* from fpga_pwm */
uDET_t  uStatDET = {0};
uGDE_t  uStatGDE = {0};

uDET_t  uMaskDET = {0x07};  /* Force No Masking */
uGDE_t  uMaskGDE = {0xFF};

uDET_t  uMaskDETFbk = {0};
uGDE_t  uMaskGDEFbk = {0};



/* from dac_trip */
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
