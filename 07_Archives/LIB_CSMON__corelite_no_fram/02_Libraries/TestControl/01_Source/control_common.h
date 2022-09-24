/* *****************************************************************************
 * File:   control_common.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 08 14 16:00
 * 
 * Description: ...
 * 
 **************************************************************************** */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Guard condition file contents not included more than once */  
/* #pragma once */
#ifndef CONTROL_COMMON_H
#define	CONTROL_COMMON_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdbool.h>
#include <stdint.h>
    
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#include "boardcfg.h"
#include "control_cfg.h"
#include "mclib.h"
    
/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */
    
/* Control Limitations */
#ifndef BOARDCFG_CURRENT_REQUEST_MAX_A
#define CONTROL_CURRENT_REQUEST_MAX_A               (1000.0)
#else
#define CONTROL_CURRENT_REQUEST_MAX_A               BOARDCFG_CURRENT_REQUEST_MAX_A
#endif

#if 0
#ifndef BOARDCFG_VOLTAGE_REQUEST_MAX_V
#define CONTROL_VOLTAGE_REQUEST_MAX_V               (1200.0)
#else
#define CONTROL_VOLTAGE_REQUEST_MAX_V               BOARDCFG_VOLTAGE_REQUEST_MAX_V
#endif
#endif

#ifndef BOARDCFG_FIELD_FREQUENCY_REQUEST_MAX_HZ
#define CONTROL_FIELD_FREQUENCY_REQUEST_MAX_HZ      (  70.0)
#else
#define CONTROL_FIELD_FREQUENCY_REQUEST_MAX_HZ      BOARDCFG_FIELD_FREQUENCY_REQUEST_MAX_HZ
#endif

#ifndef BOARDCFG_FIELD_FREQUENCY_REQUEST_MIN_HZ
#define CONTROL_FIELD_FREQUENCY_REQUEST_MIN_HZ      (   5.0)
#else
#define CONTROL_FIELD_FREQUENCY_REQUEST_MIN_HZ      BOARDCFG_FIELD_FREQUENCY_REQUEST_MIN_HZ
#endif

#ifndef BOARDCFG_PWM_FREQUENCY_REQUEST_MAX_HZ
#define CONTROL_PWM_FREQUENCY_REQUEST_MAX_HZ        (3500.0)
#else
#define CONTROL_PWM_FREQUENCY_REQUEST_MAX_HZ        BOARDCFG_PWM_FREQUENCY_REQUEST_MAX_HZ
#endif

#ifndef BOARDCFG_PWM_FREQUENCY_REQUEST_MIN_HZ
#define CONTROL_PWM_FREQUENCY_REQUEST_MIN_HZ        (1500.0)
#else
#define CONTROL_PWM_FREQUENCY_REQUEST_MIN_HZ        BOARDCFG_PWM_FREQUENCY_REQUEST_MIN_HZ
#endif



/* Control Parameter Configuration */
#ifndef BOARDCFG_TRIP_TEMPERATURE_DEG
#define CONTROL_TRIP_TEMPERATURE_DEG                (  80.0)
#else
#define CONTROL_TRIP_TEMPERATURE_DEG                BOARDCFG_TRIP_TEMPERATURE_DEG
#endif

#ifndef BOARDCFG_VOLTAGE_DCLINK_REQUEST_V
#define CONTROL_VOLTAGE_DCLINK_REQUEST_V            (  20.0)
#else
#define CONTROL_VOLTAGE_DCLINK_REQUEST_V            BOARDCFG_VOLTAGE_DCLINK_REQUEST_V
#endif






#define RMS_CURRENT_1_PH_REQ_A    (0.0F)
#define RMS_CURRENT_3_PH_REQ_A    (RMS_CURRENT_1_PH_REQ_A * MCLIB_1PH_TO_3PH_RMS)
#define AMP_CURRENT_3_PH_REQ_A    (RMS_CURRENT_3_PH_REQ_A * MCLIB_3PH_RMS_TO_AMP)


/* Q16.16 - integer part (high 16 bits) and 16 bits fractional part */

    
    
/* ADC To DAC */
#define ADCResultToDACResultAmperesMultiplier \
        MCLIB_u32FractionalQ_16_16(BOARDCFG_ADC_RESULT_TO_IFBK_MULT / BOARDCFG_DAC_RESULT_TO_IFBK_MULT)
    
#define ADCResultToDACResultVoltageMultiplier \
        MCLIB_u32FractionalQ_16_16(BOARDCFG_ADC_RESULT_TO_UFBK_MULT / BOARDCFG_DAC_RESULT_TO_UFBK_MULT)
    
    
    
/* CURRENT REFERENCE */
#define DACDeciAmperesOffset \
        MCLIB_s32FractionalQ_15_16(BOARDCFG_DAC_RESULT_TO_IFBK_OFFS * 10)            /* DAC_RESULT_TO_AMP_IFBK_OFFS */

#define DACAmperesOffset \
        MCLIB_s32FractionalQ_15_16(BOARDCFG_DAC_RESULT_TO_IFBK_OFFS)

#define DACResultToFromDeciAmperesOffset \
        MCLIB_u32FractionalQ_16_16(BOARDCFG_DAC_RESULT_TO_IFBK_OFFS * 10)            /* DAC_RESULT_TO_AMP_IFBK_OFFS */

#define DACResultToFromAmperesOffset \
        MCLIB_u32FractionalQ_16_16(BOARDCFG_DAC_RESULT_TO_IFBK_OFFS)

#define DACResultToDeciAmperesMultiplier \
        MCLIB_u32FractionalQ_16_16(BOARDCFG_DAC_RESULT_TO_IFBK_MULT * 10)            /* DAC_RESULT_TO_AMP_IFBK_MULT */

#define DACResultToAmperesMultiplier \
        MCLIB_u32FractionalQ_16_16(BOARDCFG_DAC_RESULT_TO_IFBK_MULT)

#define DeciAmperesToDACResultMultiplier \
        MCLIB_u32FractionalQ_16_16(1.0 / (BOARDCFG_DAC_RESULT_TO_IFBK_MULT * 10))

#define AmperesToDACResultMultiplier \
        MCLIB_u32FractionalQ_16_16(1.0 / (BOARDCFG_DAC_RESULT_TO_IFBK_MULT))


/* VOLTAGE REFERENCE */
#define DACDeciVoltageOffset \
        MCLIB_s32FractionalQ_15_16(BOARDCFG_DAC_RESULT_TO_UFBK_OFFS * 10)

#define DACVoltageOffset \
        MCLIB_s32FractionalQ_15_16(BOARDCFG_DAC_RESULT_TO_UFBK_OFFS)     /* DAC_RESULT_TO_VOLTS_DC_OFFS */

#define DACResultToFromDeciVoltageOffset \
        MCLIB_u32FractionalQ_16_16(BOARDCFG_DAC_RESULT_TO_UFBK_OFFS * 10)

#define DACResultToFromVoltageOffset \
        MCLIB_u32FractionalQ_16_16(BOARDCFG_DAC_RESULT_TO_UFBK_OFFS)     /* DAC_RESULT_TO_VOLTS_DC_OFFS */

#define DACResultToDeciVoltageMultiplier \
        MCLIB_u32FractionalQ_16_16(BOARDCFG_DAC_RESULT_TO_UFBK_MULT * 10)       /* DAC_RESULT_TO_VOLTS_DC_MULT */

#define DACResultToVoltageMultiplier \
        MCLIB_u32FractionalQ_16_16(BOARDCFG_DAC_RESULT_TO_UFBK_MULT)

#define DeciVoltageToDACResultMultiplier \
        MCLIB_u32FractionalQ_16_16(1.0 / (BOARDCFG_DAC_RESULT_TO_UFBK_MULT * 10))

#define VoltageToDACResultMultiplier \
        MCLIB_u32FractionalQ_16_16(1.0 / (BOARDCFG_DAC_RESULT_TO_UFBK_MULT))


/* CFBK MEASUREMENT */    
#define ADCResultToDeciAmperesMultiplierCFBK \
        MCLIB_u32FractionalQ_16_16(BOARDCFG_ADC_RESULT_TO_CFBK_MULT * 10)       /* ADC_RESULT_TO_AMP_CFBK_MULT */
#define ADCResultToDeciAmperesMultiplierCFBK_Q_0_16 \
        MCLIB_u16Fractional(BOARDCFG_ADC_RESULT_TO_CFBK_MULT * 10)
  
    
    
    
/* TFBK MEASUREMENT */    
#define ADCResultToDeciDegreesMultiplierTFBK \
        MCLIB_u16Fractional(BOARDCFG_ADC_RESULT_TO_TFBK_MULT * 10)              /* ADC_RESULT_TO_DEG_TFBK_MULT */
   
#define ADCResultToDeciDegreesOffsetTFBK \
        (BOARDCFG_ADC_RESULT_TO_TFBK_OFFS * 10)                                 /* ADC_RESULT_TO_DEG_TFBK_OFFS */
 
    
    
/* CURRENT MEASUREMENT */


#define ADCVoltageOnConnectorToAmperesMultiplier \
        MCLIB_u32FractionalQ_16_16(BOARDCFG_VOLTAGE_10V_TO_IFBK_MULT)           /* was VOLTAGE_ON_CURRENT_INPUT_TO_AMPERES_MULT */

#define ADCVoltageOnConnectorToDeciAmperesMultiplier \
        MCLIB_u32FractionalQ_16_16(BOARDCFG_VOLTAGE_10V_TO_IFBK_MULT * 10)

#define ADCResultToVoltageOnConnectorMultiplier \
        MCLIB_u32FractionalQ_16_16(BOARDCFG_ADC_RESULT_TO_10V_IFBK_MULT)        /* was ADC_RESULT_TO_AMP_IFBK_INPUT_VOLTAGE_MULT */

#define ADCResultToDeciAmperesMultiplier \
        MCLIB_u32FractionalQ_16_16(BOARDCFG_ADC_RESULT_TO_IFBK_MULT * 10)

#define ADCResultToAmperesMultiplier \
        MCLIB_u32FractionalQ_16_16(BOARDCFG_ADC_RESULT_TO_IFBK_MULT)

#define AmperesToADCResultMultiplier \
        MCLIB_u32FractionalQ_16_16(1.0 / (BOARDCFG_ADC_RESULT_TO_IFBK_MULT))    /* was ADC_RESULT_TO_AMP_IFBK_MULT */

#define DeciAmperesToADCResultMultiplier \
        MCLIB_u32FractionalQ_16_16(1.0 / (BOARDCFG_ADC_RESULT_TO_IFBK_MULT * 10))

/* VOLTAGE MEASUREMENT */
#define ADCResultToDeciVoltageMultiplier \
        MCLIB_u16Fractional(BOARDCFG_ADC_RESULT_TO_UFBK_MULT * 10)                           /* was ADC_RESULT_TO_VOLTS_DC_MULT */

#define ADCResultToVoltageMultiplier \
        MCLIB_u32FractionalQ_16_16(BOARDCFG_ADC_RESULT_TO_UFBK_MULT)

#define VoltsToADCResultMultiplier \
        MCLIB_u32FractionalQ_16_16(1.0 / (BOARDCFG_ADC_RESULT_TO_UFBK_MULT))

#define DeciVoltsToADCResultMultiplier \
        MCLIB_u32FractionalQ_16_16(1.0 / (BOARDCFG_ADC_RESULT_TO_UFBK_MULT * 10))

#define SpeedToDeciHzMultiplier                 MCLIB_u16Fractional(SPEED_RESOLUTION_HZ * 10)

#define SpeedToHzMultiplier                     MCLIB_u32FractionalQ_16_16(SPEED_RESOLUTION_HZ)

#define DeciHzToSpeedMultiplier                 MCLIB_u32FractionalQ_16_16(1.0/(SPEED_RESOLUTION_HZ * 10))

#define HzToSpeedMultiplier                     MCLIB_u32FractionalQ_16_16(1.0/SPEED_RESOLUTION_HZ)

#define DeadTimeMCUNanoSecondsToTicksMultiplier MCLIB_u32FractionalQ_16_16((FCY_HZ*2) / NANOSECOND_CLOCK_FREQ_HZ)       /* (FCY_HZ*2) ??? center aligned mode */
#define DeadTimeMCUTicksToNanoSecondsMultiplier MCLIB_u32FractionalQ_16_16(1.0/((FCY_HZ*2) / NANOSECOND_CLOCK_FREQ_HZ))

#define DeadTimeNanoSecondsToTicksMultiplier    MCLIB_u32FractionalQ_16_16(DEAD_TIME_CLOCK_FREQ_HZ / NANOSECOND_CLOCK_FREQ_HZ)
#define DeadTimeTicksToNanoSecondsMultiplier    MCLIB_u32FractionalQ_16_16(1.0/(DEAD_TIME_CLOCK_FREQ_HZ / NANOSECOND_CLOCK_FREQ_HZ))

#define MinPulseNanoSecondsToTicksMultiplier    MCLIB_u32FractionalQ_16_16(MIN_PULSE_CLOCK_FREQ_HZ / NANOSECOND_CLOCK_FREQ_HZ)
#define MinPulseTicksToNanoSecondsMultiplier    MCLIB_u32FractionalQ_16_16(1.0/(MIN_PULSE_CLOCK_FREQ_HZ / NANOSECOND_CLOCK_FREQ_HZ))


/* Max values Used By the control */
#define MAX_CURRENT_A   (BOARDCFG_ADC_RESULT_TO_IFBK_MULT * 32768.0)
#define MAX_VOLTAGE_V   (BOARDCFG_ADC_RESULT_TO_UFBK_MULT * 32768.0)


#define SAMPLING_FREQUENCY_HZ           LOOPTIME1_HZ
#define SAMPLING_PERIOD_SEC             (1.0 / SAMPLING_FREQUENCY_HZ)

#define RESISTANCE_PHASE_TO_ZERO        (0.03)
#define INDUCTANCE_PHASE_TO_ZERO        (0.0003)

//#define RESISTANCE_PHASE_TO_ZERO        (0.01)
//#define INDUCTANCE_PHASE_TO_ZERO        (0.0001)

#define CURRENT_TO_VOLTAGE_GLOB_DAMP    (0.00001)  /* Base Damping Factor 0.25 */
//#define CURRENT_TO_VOLTAGE_GLOB_DAMP    (1.0)  /* Base Damping Factor 0.25 */
#define CURRENT_TO_VOLTAGE_PROP_DAMP    CURRENT_TO_VOLTAGE_GLOB_DAMP
#define CURRENT_TO_VOLTAGE_INTG_DAMP    CURRENT_TO_VOLTAGE_GLOB_DAMP

#define CURRENT_TO_VOLTAGE_SCALE_MUL    (MAX_CURRENT_A/MAX_VOLTAGE_V) /* 0,47224386673278080800925597978795 A/V */

                                                                      /* * 0.25 was original now 0.5 for integral part because of the use of Bilinear method, also known as trapezoidal approximation */     
#define CURRENT_TO_VOLTAGE_PROP_COEF    (CURRENT_TO_VOLTAGE_PROP_DAMP /* * 0.25 */ * INDUCTANCE_PHASE_TO_ZERO * SAMPLING_FREQUENCY_HZ * CURRENT_TO_VOLTAGE_SCALE_MUL)    /* 0,35418290004958560600694198484096 V/A */
#define CURRENT_TO_VOLTAGE_INTG_COEF    (CURRENT_TO_VOLTAGE_INTG_DAMP    * 0.35     * RESISTANCE_PHASE_TO_ZERO * SAMPLING_FREQUENCY_HZ * CURRENT_TO_VOLTAGE_SCALE_MUL)    /* 0,17709145002479280300347099242048 V/A */

/* Shift used to improve resolution made in initialization function */
//#define CURRENT_TO_VOLTAGE_PROP_SH      -1
//#define CURRENT_TO_VOLTAGE_PROP_COEF_SH ((CURRENT_TO_VOLTAGE_PROP_COEF * ((uint32_t)1 << (16-CURRENT_TO_VOLTAGE_PROP_SH))) / ((uint32_t)1 << 16))
/* Shift to fit [-1..1] */
//#define CURRENT_TO_VOLTAGE_INTG_SH      5
//#define CURRENT_TO_VOLTAGE_INTG_COEF_SH ((CURRENT_TO_VOLTAGE_INTG_COEF * ((uint32_t)1 << (16-CURRENT_TO_VOLTAGE_INTG_SH))) / ((uint32_t)1 << 16))

#define CURRENT_TO_VOLTAGE_MAX_LIMIT    (MAX_VOLTAGE_V / MAX_VOLTAGE_V)
#define CURRENT_TO_VOLTAGE_MIN_LIMIT    (    0.0       / MAX_VOLTAGE_V)

#define ADC_OFFSET_FILTER_DEPTH_BITS    10
#define VOLTAGE_RUN_FILTER_DEPTH_BITS   4
#define CURRENT_RUN_FILTER_DEPTH_BITS   8

#define RUN_RMS_FILTER_SAMPLES          200      /* 50 for one period at 2.5kHz */



#define AMP_VOLTAGE_FBCK_EMULATION_V    (311.0F)
#define AMP_VOLTAGE_FBCK_EMULATION_STARTUP_V    (0.0F)
    
#define AMP_VOLTAGE_FBCK_EMULATION_RAMPUP_V_PER_PWM_PERIOD    (10.0F)           /* must be  > 1 (max 32767) */
#define AMP_VOLTAGE_FBCK_EMULATION_RAMPDN_V_PER_PWM_PERIOD    (0.02F)           /* must be 0..1 */
    
#define RMS_CURRENT_1_PH_EMULATION_A    (220.0F)
#define RMS_CURRENT_3_PH_EMULATION_A    (RMS_CURRENT_1_PH_EMULATION_A * MCLIB_1PH_TO_3PH_RMS)
#define AMP_CURRENT_3_PH_EMULATION_A    (RMS_CURRENT_3_PH_EMULATION_A * MCLIB_3PH_RMS_TO_AMP)
//#define AMP_CURRENT_3_PH_EMULATION_A    (RMS_CURRENT_1_PH_EMULATION_A * MCLIB_1PH_RMS_TO_AMP)
#define CONTROL_FIELD_SPEED_EMULATION_HZ      (50.0F)

#define CONTROL_FIELD_SPEED_HZ                (50.0F)
#define RMS_VOLTAGE_STARTUP             (50.0F)

#define VOLTAGE_OFFSET_MAX_DELTA_IN_CALIBRATION (10.0F)    
    
/* Speed Scale 16 bit signed with 0.05 Hz resolution ((32768 - (-32768))*0.05) loop control 2500Hz - need to make first -180 to 0 degrees (-1..0) fractional and after that make 0 to 180 degrees (0..1) for whole sinusoidal period is -1..1  (1-(-1))   */
#define POSITION_INTEGRATOR_SCALE_COEF_REAL ( MCLIB_s16Fractional(SPEED_RESOLUTION_HZ) / (BOARDCFG_PWM_FREQUENCY_HZ)) /* must be -1..1 */
#define POSITION_INTEGRATOR_SCALE_SHIFT     1       /* scale POSITION_INTEGRATOR_SCALE_COEF to fit -1..1 (calculated output is shifted left with this)  */   
//#define POSITION_INTEGRATOR_SCALE_COEF      MCLIB_s16Fractional( 1638.4F / ((uint16_t)(BOARDCFG_PWM_FREQUENCY_HZ) << POSITION_INTEGRATOR_SCALE_SHIFT) ) /* must be -1..1 */


#define SPEED_INTEGRATOR_FIX_PRESCALER (1 << POSITION_INTEGRATOR_SCALE_SHIFT)
#define POSITION_INTEGRATOR_SCALE_COEF      MCLIB_s16FracFromFrac_u16( (uint16_t)(POSITION_INTEGRATOR_SCALE_COEF_REAL) >> POSITION_INTEGRATOR_SCALE_SHIFT)  /* real divided by 2^POSITION_INTEGRATOR_SCALE_SHIFT ??? or not like this in TI - smth unknown */




/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
typedef struct
{
    uint32_t u32CalibCount;  
    uint16_t u16ADCResultToDeciVoltageMultiplier;                               /* Fractional 0..1 16-bit (Q_0_16) - always must be less than 1.0 */
    uint32_t u32DACResultToDeciAmperesMultiplier;
    uint32_t u32DACResultToDeciVoltageMultiplier;
    uint32_t u32DACResultToFromDeciAmperesOffset;
    uint32_t u32DACResultToFromDeciVoltageOffset;
     int32_t s32DACDeciAmperesOffset;
     int32_t s32DACDeciVoltageOffset;
    uint16_t u16SpeedToDeciHzMultiplier;                                        /* Fractional 0..1 16-bit (Q_0_16) - always must be less than 1.0 */
    uint32_t u32ADCResultToAmperesMultiplier;
    uint32_t u32ADCResultToVoltageMultiplier;
    uint32_t u32DACResultToAmperesMultiplier;
    uint32_t u32DACResultToVoltageMultiplier;
    uint32_t u32DACResultToFromAmperesOffset;
    uint32_t u32DACResultToFromVoltageOffset;
    uint32_t u32ADCResultToDACResultAmperesMultiplier;
    uint32_t u32ADCResultToDACResultVoltageMultiplier;
    uint32_t u32SpeedToHzMultiplier;

    uint32_t u32DeciAmperesToADCResultMultiplier;
    uint32_t u32DeciAmperesToADCResultMultiplierCheck;
    uint32_t u32DeciAmperesToADCResultMultiplierVisual;
    uint32_t u32DeciAmperesToADCResultMultiplierRequest;



    uint32_t u32AmperesToADCResultMultiplier;
    uint32_t u32AmperesToADCResultMultiplierCheck;
    uint32_t u32AmperesToADCResultMultiplierVisual;
    uint32_t u32AmperesToADCResultMultiplierRequest;

    uint32_t u32ADCResultToDeciAmperesMultiplier;
    uint16_t u16ADCResultToDeciAmperesMult;
    uint16_t u16ADCResultToDeciAmperesShft;

    uint32_t u32ADCResultToDeciAmperesMultiplierCheck;

    uint32_t u32ADCResultToDeciAmperesMultiplierVisual;
    uint16_t u16ADCResultToDeciAmperesMultVisual;
    uint16_t u16ADCResultToDeciAmperesShftVisual;


    uint32_t u32ADCResultToDeciAmperesMultiplierStack1;
    uint32_t u32ADCResultToDeciAmperesMultiplierStack2;
    uint32_t u32ADCResultToDeciAmperesMultiplierStack3;



    uint32_t u32DeciVoltsToADCResultMultiplier;
    uint32_t u32DeciAmperesToDACResultMultiplier;
    uint32_t u32DeciVoltsToDACResultMultiplier;
    uint32_t u32DeciHzToSpeedMultiplier;
    uint32_t u32VoltsToADCResultMultiplier;
    uint32_t u32AmperesToDACResultMultiplier;
    uint32_t u32VoltsToDACResultMultiplier;
    uint32_t u32HzToSpeedMultiplier;
    uint32_t u32DeadTimeMCUNanoSecondsToTicksMultiplier;
    uint32_t u32DeadTimeMCUTicksToNanoSecondsMultiplier;
    uint32_t u32DeadTimeNanoSecondsToTicksMultiplier;
    uint32_t u32DeadTimeTicksToNanoSecondsMultiplier;
    uint32_t u32MinPulseNanoSecondsToTicksMultiplier;
    uint32_t u32MinPulseTicksToNanoSecondsMultiplier;
    
    uint16_t u16ADCResultToDeciAmperesMultiplierCFBK;                           /* Fractional 0..1 16-bit (Q_0_16) - always must be less than 1.0 */
    uint16_t u16ADCResultToDeciDegreesMultiplierTFBK;                           /* Fractional 0..1 16-bit (Q_0_16) - always must be less than 1.0 */
    
     int16_t s16ADCResultToDeciDegreesOffsetTFBK;


     uint32_t u32ADCResultToIFBKInputConnectorVoltageMultiplier;    /* Bad Resolution */
     uint32_t u32ADCResultToIFBKInputConnectorVoltageMultiplierCheck;    /* Bad Resolution */
     uint32_t u32ADCResultToDCLVInputConnectorVoltageMultiplier;    /* Bad Resolution */
     uint32_t u32ADCResultToLN_VInputConnectorVoltageMultiplier;    /* Bad Resolution */
     uint32_t u32ADCResultToPRESInputConnectorVoltageMultiplier;    /* Bad Resolution */
     uint32_t u32ADCResultToDCLIInputConnectorVoltageMultiplier;    /* Bad Resolution */
     uint32_t u32ADCResultToTH2OInputConnectorVoltageMultiplier;    /* Bad Resolution */
     uint32_t u32ADCResultToTAIRInputConnectorVoltageMultiplier;    /* Bad Resolution */

     uint32_t u32ADCScaleToIFBKInputConnectorVoltageMultiplier;
     uint32_t u32ADCScaleToIFBKInputConnectorVoltageMultiplierCheck;
     uint32_t u32ADCScaleToDCLVInputConnectorVoltageMultiplier;
     uint32_t u32ADCScaleToLN_VInputConnectorVoltageMultiplier;
     uint32_t u32ADCScaleToPRESInputConnectorVoltageMultiplier;
     uint32_t u32ADCScaleToDCLIInputConnectorVoltageMultiplier;
     uint32_t u32ADCScaleToTH2OInputConnectorVoltageMultiplier;
     uint32_t u32ADCScaleToTAIRInputConnectorVoltageMultiplier;

     uint32_t u32ADCResultToIFBKRealValueMultiplier;
     uint32_t u32ADCResultToIFBKRealValueMultiplierCheck;
     uint32_t u32ADCResultToDCLVRealValueMultiplier;
     uint32_t u32ADCResultToLN_VRealValueMultiplier;
     uint32_t u32ADCResultToPRESRealValueMultiplier;
     uint32_t u32ADCResultToDCLIRealValueMultiplier;
     int32_t s32ADCResultToTH2ORealValueMultiplier;
     int32_t s32ADCResultToTAIRRealValueMultiplier;

     int32_t s32ADCResultToTH2ORealValueDivider;
     int32_t s32ADCResultToTAIRRealValueDivider;

     int32_t s32ADCResultToTH2ORealValueOffset;
     int32_t s32ADCResultToTAIRRealValueOffset;

     uint32_t u32ADCScaleToIFBKRealValueMultiplier;
     uint32_t u32ADCScaleToIFBKRealValueMultiplierCheck;
      uint32_t u32ADCScaleToDCLVRealValueMultiplier;
      uint32_t u32ADCScaleToLN_VRealValueMultiplier;
      uint32_t u32ADCScaleToPRESRealValueMultiplier;
      uint32_t u32ADCScaleToDCLIRealValueMultiplier;
      int32_t s32ADCScaleToTH2ORealValueMultiplier;
      int32_t s32ADCScaleToTAIRRealValueMultiplier;

      int32_t s32ADCScaleToTH2ORealValueOffset;
      int32_t s32ADCScaleToTAIRRealValueOffset;

      int32_t s32ADCScaleTH2ORealValueOffset;
      int32_t s32ADCScaleTAIRRealValueOffset;

      int32_t s32ADCScaleTH2ORealValueMult;
      int32_t s32ADCScaleTAIRRealValueMult;

     uint32_t u32ADCScaleMaxValue;


     /* DAC Scale To Board Connector Multipliers  */
     uint32_t u32DACScaleToIFBKInputConnectorVoltageMultiplier;
     uint32_t u32DACScaleToDCLVInputConnectorVoltageMultiplier;
     uint32_t u32DACScaleToDCLIInputConnectorVoltageMultiplier;

     /* DAC Result To Board Connector Multipliers  */
     uint32_t u32DACResultToIFBKInputConnectorVoltageMultiplier;
     uint32_t u32DACResultToDCLVInputConnectorVoltageMultiplier;
     uint32_t u32DACResultToDCLIInputConnectorVoltageMultiplier;

     /* DAC Scale To Real Values Multipliers  */
     uint32_t u32DACScaleToIFBKRealValueMultiplier;
     uint32_t u32DACScaleToDCLVRealValueMultiplier;
     uint32_t u32DACScaleToDCLIRealValueMultiplier;
    
     /* DAC Result To Real Values Multipliers  */
     uint32_t u32DACResultToIFBKRealValueMultiplier;
     uint32_t u32DACResultToDCLVRealValueMultiplier;
     uint32_t u32DACResultToDCLIRealValueMultiplier;


     uint32_t u32DACScaleMaxValue;

     uint32_t u32CurrentSensingGateDriverVoltageBaseMultiplier;

     uint32_t u32ADCResultToVoltageOnConnectorMultiplier;
     uint32_t u32ADCVoltageOnConnectorToAmperesMultiplier;
     uint32_t u32ADCVoltageOnConnectorToDeciAmperesMultiplier;



     uint16_t u16ADCResultToAmperesFixShiftControl;
     uint16_t u16ADCResultToAmperesFixShiftStack1 ;
     uint16_t u16ADCResultToAmperesFixShiftStack2 ;
     uint16_t u16ADCResultToAmperesFixShiftStack3 ;

     uint32_t u32AmperesToADCResultFixMultStack1 ;
     uint32_t u32AmperesToADCResultFixMultStack2 ;
     uint32_t u32AmperesToADCResultFixMultStack3 ;







}sControlConfig_t;


typedef struct
{
     uint16_t bStack1 : 1;
     uint16_t bStack2 : 1;
     uint16_t bStack3 : 1;

}CONTROL_sStackUsageMask_t;

typedef union
{
    uint16_t u16Register;
    CONTROL_sStackUsageMask_t sStackUsageMask;
}CONTROL_uStackUsageMask_t;


/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 
extern sControlConfig_t sControlConfig;
extern bool bUsePMPwithControl;
extern bool bUseControlTemperatureTrip;

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
 
#endif	/* CONTROL_COMMON_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
