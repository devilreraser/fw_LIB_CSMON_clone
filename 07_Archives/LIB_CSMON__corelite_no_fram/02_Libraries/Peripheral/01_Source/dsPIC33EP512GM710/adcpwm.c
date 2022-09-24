/* *****************************************************************************
 * File:   adcpwm.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 02 11 11:18
 * 
 * Description: ADC and PWM Driver
 * 
 **************************************************************************** */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <xc.h>
#include <stdbool.h>
#include <stdint.h>


#include "boardcfg.h"
#include "sys_drv.h"
#include "adcpwm.h"
#include "control.h"
#include "isr.h"
#include "control_common.h"
#include "hmlib.h"

#ifdef BOARDCFG_USE_MCC_PWM
#include "pwm.h"
#endif

#ifdef BOARDCFG_USE_MCC_ADC
#include "adc1.h"
#include "adc2.h"
#endif

#include "adc_cfg.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define TCY_SEC         (1.0/FCY_HZ)        /* Instruction cycle period (sec) */
#define TAD_TCY                 nSecToTicks(133.33,FCY_HZ)  /* rounded to 0.5 ticks min request not guaranteed. Max 256 TCY not checked. (117.6 nsec min on datasheet) */
//#define SAMPLETIME_TCY_10BIT_CALC    uSecToTicks(0.800,(FCY_HZ/TAD_TCY)) /* 31 TAD max not checked. Used for Sequenced Channels */
#define SAMPLETIME_TCY_10BIT_CALC    uSecToTicks(4.133,(FCY_HZ/TAD_TCY)) /* 31 TAD max not checked. Used for Sequenced Channels. 0.8 uSec minimum */
//#define SAMPLETIME_TCY_12BIT_CALC    uSecToTicks(4.133,(FCY_HZ/TAD_TCY)) /* 31 TAD max not checked. Used for Sequenced Channels */
#define SAMPLETIME_TCY_12BIT_CALC    uSecToTicks(4.133,(FCY_HZ/TAD_TCY)) /* 31 TAD max not checked. Used for Sequenced Channels */
 
#define SAMPLETIME_TCY_10BIT    (SAMPLETIME_TCY_10BIT_CALC <= 31) ? SAMPLETIME_TCY_10BIT_CALC : 31 /* 31 TAD max check */
#define SAMPLETIME_TCY_12BIT    (SAMPLETIME_TCY_12BIT_CALC <= 31) ? SAMPLETIME_TCY_12BIT_CALC : 31 /* 31 TAD max check */

#define AD1CSSL_NO_SHIFT (uint16_t)((1 << L1_CURRENT_1) | (1 << L2_CURRENT_1) | (1 << L3_CURRENT_1) | (1 << DC_VOLTAGE_1) | (1 << L1_CURRENT_2) | (1 << L2_CURRENT_2) | (1 << L3_CURRENT_2) | (1 << DC_VOLTAGE_2))
#define AD2CSSL_NO_SHIFT (uint16_t)((1 << DC_VOLTAGE_3) | (1 << L1_CURRFBK_1) | (1 << L2_CURRFBK_1) | (1 << L3_CURRFBK_1) | (1 << DC_VOLTAGE_4) | (1 << L1_TEMPFBK_1) | (1 << L2_TEMPFBK_1) | (1 << L3_TEMPFBK_1))

#ifndef AD1CSSH_MASK
#define AD1CSSH_MASK    0
#endif
#ifndef AD1CSSL_MASK
#define AD1CSSL_MASK    0xFF
#endif
#ifndef AD1CSS_COUNT
#define AD1CSS_COUNT    8
#endif

#ifndef AD2CSSH_MASK
#define AD2CSSH_MASK    0
#endif
#ifndef AD2CSSL_MASK
#define AD2CSSL_MASK    0xFF00
#endif
#ifndef AD2CSS_COUNT
#define AD2CSS_COUNT    8
#endif



#if !defined(BOARDCFG_USE_MCC_ADC) || defined(__dsPIC33EP128GM706__)
#if !defined(_ADC1_H) || defined(__dsPIC33EP128GM706__)
#define ADC1_ConversionResultGet(x) ((uint16_t*)(&ADC1BUF0))[x]       
#endif
#if !defined(_ADC2_H) || defined(__dsPIC33EP128GM706__)
#define ADC2_ConversionResultGet(x) ((uint16_t*)(&ADC2BUF0))[x]        
#endif
#endif

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#if BOARDCFG_USE_OLD_NON_MCC_ADC
#ifndef BOARDCFG_USE_MCC_ADC
/* channels for sequence */
#define AD1CSSL_PSEV (uint16_t)((1 <<  L1_CURRENT_1) | (1 <<  L2_CURRENT_1) | (1 <<  L3_CURRENT_1) | (1 <<  DC_VOLTAGE_1))
#define AD1CSSL_SSEV (uint16_t)((1 <<  L1_CURRENT_2) | (1 <<  L2_CURRENT_2) | (1 <<  L3_CURRENT_2) | (1 <<  DC_VOLTAGE_2))


#define AD2CSSL_PSEV (uint16_t)((1 <<  DC_VOLTAGE_3) | (1 <<  L1_CURRFBK_1) | (1 <<  L2_CURRFBK_1) | (1 <<  L3_CURRFBK_1))
#define AD2CSSL_SSEV (uint16_t)((1 <<  DC_VOLTAGE_4) | (1 <<  L1_TEMPFBK_1) | (1 <<  L2_TEMPFBK_1) | (1 <<  L3_TEMPFBK_1))


#define AD1CSSL_NO_SHIFT (uint16_t)((1 << L1_CURRENT_1) | (1 << L2_CURRENT_1) | (1 << L3_CURRENT_1) | (1 << DC_VOLTAGE_1) | (1 << L1_CURRENT_2) | (1 << L2_CURRENT_2) | (1 << L3_CURRENT_2) | (1 << DC_VOLTAGE_2))
#define AD2CSSL_NO_SHIFT (uint16_t)((1 << DC_VOLTAGE_3) | (1 << L1_CURRFBK_1) | (1 << L2_CURRFBK_1) | (1 << L3_CURRFBK_1) | (1 << DC_VOLTAGE_4) | (1 << L1_TEMPFBK_1) | (1 << L2_TEMPFBK_1) | (1 << L3_TEMPFBK_1))



#define ADC_BUFFER_LENGTH 8

 

/* channels for no sequence */
#define ADC1_PWM1_NO_SEQUENCE_CH0   L1_CURRENT_1
#define ADC1_PWM2_NO_SEQUENCE_CH0   L2_CURRENT_1
#define ADC1_PWM3_NO_SEQUENCE_CH0   L3_CURRENT_1
#define ADC1_PWM4_NO_SEQUENCE_CH0   L1_CURRENT_2
#define ADC1_PWM5_NO_SEQUENCE_CH0   L2_CURRENT_2
#define ADC1_PWM6_NO_SEQUENCE_CH0   L3_CURRENT_2
#define ADC1_PWM7_NO_SEQUENCE_CH0   16  /* not used */
#define ADC1_PSEV_NO_SEQUENCE_CH0   DC_VOLTAGE_1
#define ADC1_SSEV_NO_SEQUENCE_CH0   DC_VOLTAGE_2

#define ADC2_PWM1_NO_SEQUENCE_CH0   L1_CURRFBK_1
#define ADC2_PWM2_NO_SEQUENCE_CH0   L2_CURRFBK_1
#define ADC2_PWM3_NO_SEQUENCE_CH0   L3_CURRFBK_1
#define ADC2_PWM4_NO_SEQUENCE_CH0   L1_TEMPFBK_1
#define ADC2_PWM5_NO_SEQUENCE_CH0   L2_TEMPFBK_1
#define ADC2_PWM6_NO_SEQUENCE_CH0   L3_TEMPFBK_1
#define ADC2_PWM7_NO_SEQUENCE_CH0   16  /* not used */
#define ADC2_PSEV_NO_SEQUENCE_CH0   DC_VOLTAGE_3
#define ADC2_SSEV_NO_SEQUENCE_CH0   DC_VOLTAGE_4



/* Phase Shift Sequenced Mode */
#define SHIFT_PWM1_COEF ((1.0 / 6) * 0)
#define SHIFT_PSEV_COEF ((1.0 / 6) * 0.5)
#define SHIFT_PWM6_COEF ((1.0 / 6) * 1)
#define SHIFT_PWM2_COEF ((1.0 / 6) * 2)
#define SHIFT_PWM4_COEF ((1.0 / 6) * 3)
#define SHIFT_SSEV_COEF ((1.0 / 6) * 3.5)
#define SHIFT_PWM3_COEF ((1.0 / 6) * 4)
#define SHIFT_PWM5_COEF ((1.0 / 6) * 5)
#define SHIFT_PWM7_COEF ((1.0 / 6) * 5.5) /* Control Algorithm Execution (must be after last ADC Measure Completed) */

#define SHIFT_PWM1_TCY  CoefToTicks(SHIFT_PWM1_COEF,LOOPTIME1_TCY)      /* to do make af fractional uint16 and runtime multiply */
#define SHIFT_PWM2_TCY  CoefToTicks(SHIFT_PWM2_COEF,LOOPTIME1_TCY)
#define SHIFT_PWM3_TCY  CoefToTicks(SHIFT_PWM3_COEF,LOOPTIME1_TCY)

#define SHIFT_PWM4_TCY  CoefToTicks(SHIFT_PWM4_COEF,LOOPTIME2_TCY)
#define SHIFT_PWM5_TCY  CoefToTicks(SHIFT_PWM5_COEF,LOOPTIME2_TCY)
#define SHIFT_PWM6_TCY  CoefToTicks(SHIFT_PWM6_COEF,LOOPTIME2_TCY)

#define SHIFT_PSEV_TCY  CoefToTicks(SHIFT_PSEV_COEF,LOOPTIME1_TCY)
#define SHIFT_SSEV_TCY  CoefToTicks(SHIFT_SSEV_COEF,LOOPTIME1_TCY)
#define SHIFT_PWM7_TCY  CoefToTicks(SHIFT_PWM7_COEF,LOOPTIME1_TCY)

#define SHIFT_PSEV_DUAL_STACK_TCY   SHIFT_PWM6_TCY
#define SHIFT_SSEV_DUAL_STACK_TCY   SHIFT_PWM4_TCY

#define SHIFT_PSEV_DUAL_STACK_COEF   SHIFT_PWM6_COEF
#define SHIFT_SSEV_DUAL_STACK_COEF   SHIFT_PWM4_COEF



/* Phase Shift Not Sequenced Mode */
#define SHIFT_PWM1_SEQ_COEF ((1.0 / 6) * 0)
#define SHIFT_PWM2_SEQ_COEF ((1.0 / 6) * 0)
#define SHIFT_PWM3_SEQ_COEF ((1.0 / 6) * 0)
#define SHIFT_PSEV_SEQ_COEF ((1.0 / 6) * 0)
#define SHIFT_PWM4_SEQ_COEF ((1.0 / 6) * 3)
#define SHIFT_PWM5_SEQ_COEF ((1.0 / 6) * 3)
#define SHIFT_PWM6_SEQ_COEF ((1.0 / 6) * 3)
#define SHIFT_SSEV_SEQ_COEF ((1.0 / 6) * 3)
#define SHIFT_PWM7_SEQ_COEF ((1.0 / 6) * 5.5) /* Control Algorithm Execution (must be after last ADC Measure Completed) */

#define SHIFT_PWM1_SEQ_TCY  CoefToTicks(SHIFT_PWM1_SEQ_COEF,LOOPTIME1_TCY)
#define SHIFT_PWM2_SEQ_TCY  CoefToTicks(SHIFT_PWM2_SEQ_COEF,LOOPTIME1_TCY)
#define SHIFT_PWM3_SEQ_TCY  CoefToTicks(SHIFT_PWM3_SEQ_COEF,LOOPTIME1_TCY)

#define SHIFT_PWM4_SEQ_TCY  CoefToTicks(SHIFT_PWM4_SEQ_COEF,LOOPTIME2_TCY)
#define SHIFT_PWM5_SEQ_TCY  CoefToTicks(SHIFT_PWM5_SEQ_COEF,LOOPTIME2_TCY)
#define SHIFT_PWM6_SEQ_TCY  CoefToTicks(SHIFT_PWM6_SEQ_COEF,LOOPTIME2_TCY)

#define SHIFT_PSEV_SEQ_TCY  CoefToTicks(SHIFT_PSEV_SEQ_COEF,LOOPTIME1_TCY)
#define SHIFT_SSEV_SEQ_TCY  CoefToTicks(SHIFT_SSEV_SEQ_COEF,LOOPTIME1_TCY)
#define SHIFT_PWM7_SEQ_TCY  CoefToTicks(SHIFT_PWM7_SEQ_COEF,LOOPTIME1_TCY)



#endif
#endif /* BOARDCFG_USE_OLD_NON_MCC_ADC */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */
#if BOARDCFG_USE_OLD_NON_MCC_ADC

#ifndef BOARDCFG_USE_MCC_ADC


#define MODE_OFFSET_SETUP_INIT     MODE_OFFSET_IMMEDIATE_AFTER_ONE_PERIOD


typedef enum
{
     MODE_OFFSET_IMMEDIATE,         /* Not Working Correct */
     MODE_OFFSET_IMMEDIATE_AFTER_ONE_PERIOD,
     MODE_OFFSET_SMOOTH, /* Seems Working Correct (but not sure - see MODE_OFFSET_IMMEDIATE) with all modes except SHIFT_L1L2L3_S1S2 */
     MODE_OFFSET_SMOOTH_AFTER_ONE_PERIOD, /* Working Correct with all modes except SHIFT_L1L2L3_S1S2 */
             
     MODE_OFFSET_COUNT
}eModeOffsetSetup_t;
#endif

#endif /* BOARDCFG_USE_OLD_NON_MCC_ADC */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
bool MAIN_bADC1SyncPrimaryTriggersStatus = 0;  
bool MAIN_bADC2SyncPrimaryTriggersStatus = 0;  

uint16_t ADCStack1[ADC_STACK_COUNT];
uint16_t ADCStack2[ADC_STACK_COUNT];

uint16_t ADCStack1Buffered[ADC_STACK_COUNT];
uint16_t ADCStack2Buffered[ADC_STACK_COUNT];
uint16_t ADCStack3Buffered[ADC_STACK_COUNT];
uint16_t ADCDCLinkBuffered[ADC_STACK_COUNT];
uint16_t ADCLine_1Buffered[ADC_STACK_COUNT];
uint16_t ADCLine_2Buffered[ADC_STACK_COUNT];

uint16_t ADC2Stack1[ADC2_STACK_COUNT];
uint16_t ADC2Stack2[ADC2_STACK_COUNT];

uint16_t ADC2Stack1Buffered[ADC2_STACK_COUNT];
uint16_t ADC2Stack2Buffered[ADC2_STACK_COUNT];


#if BOARDCFG_USE_OLD_NON_MCC_ADC

#ifndef BOARDCFG_USE_MCC_ADC
int32_t s32RealSamplingFreqCoef;
uint16_t u16PWM1FreqHz = LOOPTIME1_HZ;
uint16_t u16PWM2FreqHz = LOOPTIME2_HZ;


uint16_t u16PWM1FreqHzReq = LOOPTIME1_HZ;
uint16_t u16PWM2FreqHzReq = LOOPTIME2_HZ;


uint32_t u32Fp = FCY_HZ;

uint16_t ADCValues[ADC_BUFFER_LENGTH];
uint16_t ADCValues2[ADC_BUFFER_LENGTH];
uint16_t ADC1ResultOffset = 0;


eModeOffsetSetup_t eModeOffsetSetup = MODE_OFFSET_SETUP_INIT;

bool MAIN_bResetPWM = 0;
bool MAIN_bRecorderOnPWM_ISR = 0;


bool MAIN_bUseSequenceADC1 = 0;
bool MAIN_bUseSequenceADC1State;

bool MAIN_bUseSequenceADC2 = 0;
bool MAIN_bUseSequenceADC2State;


bool MAIN_bUseStack1ADC1 = 1;
bool MAIN_bUseStack1ADC1State;

bool MAIN_bUseStack1ADC2 = 1;
bool MAIN_bUseStack1ADC2State;

bool MAIN_bADC1Preferre12BitMode = 1;
bool MAIN_bADC1Status12BitMode;

bool MAIN_bADC1PreferreFourChannelOperation = 0;
bool MAIN_bADC1StatusFourChannelOperation;


bool MAIN_bADC2PreferreFourChannelOperation = 0;
bool MAIN_bADC2StatusFourChannelOperation;

bool MAIN_bADC1Trigger;
bool MAIN_bADC2Trigger;

bool MAIN_bADC1TriggerFreemaster;
bool MAIN_bADC2TriggerFreemaster;

bool MAIN_bADC1MainFreemasterTrigger = 1;   /* Use Slower ADC in PWM period */


bool MAIN_bUseSPHASEForPhaseOffset = 1;

bool MAIN_bUseADC1SyncPrimaryTriggersRequest = 1; 
bool MAIN_bUseADC1SyncPrimaryTriggers; 

bool MAIN_bUseADC2SyncPrimaryTriggersRequest = 1; 
bool MAIN_bUseADC2SyncPrimaryTriggers; 



bool MAIN_bPhase1OffsetRequested = 0;
bool MAIN_bPhase2OffsetRequested = 0;
bool MAIN_bPhase3OffsetRequested = 0;
bool MAIN_bPhase4OffsetRequested = 0;
bool MAIN_bPhase5OffsetRequested = 0;
bool MAIN_bPhase6OffsetRequested = 0;

bool MAIN_bPhase1OffsetEnd = 0;
bool MAIN_bPhase2OffsetEnd = 0;
bool MAIN_bPhase3OffsetEnd = 0;
bool MAIN_bPhase4OffsetEnd = 0;
bool MAIN_bPhase5OffsetEnd = 0;
bool MAIN_bPhase6OffsetEnd = 0;

bool MAIN_bPhase1OffsetCompleted = 0;
bool MAIN_bPhase2OffsetCompleted = 0;
bool MAIN_bPhase3OffsetCompleted = 0;
bool MAIN_bPhase4OffsetCompleted = 0;
bool MAIN_bPhase5OffsetCompleted = 0;
bool MAIN_bPhase6OffsetCompleted = 0;

bool MAIN_bPhase1OffsetRequestSmooth = 0;
bool MAIN_bPhase2OffsetRequestSmooth = 0;
bool MAIN_bPhase3OffsetRequestSmooth = 0;
bool MAIN_bPhase4OffsetRequestSmooth = 0;
bool MAIN_bPhase5OffsetRequestSmooth = 0;
bool MAIN_bPhase6OffsetRequestSmooth = 0;

uint16_t MAIN_u16Phase1OffsetCounter = 0;
uint16_t MAIN_u16Phase2OffsetCounter = 0;
uint16_t MAIN_u16Phase3OffsetCounter = 0;
uint16_t MAIN_u16Phase4OffsetCounter = 0;
uint16_t MAIN_u16Phase5OffsetCounter = 0;
uint16_t MAIN_u16Phase6OffsetCounter = 0;


uint8_t MAIN_u8ADC1Trigger = 0;
uint8_t MAIN_u8ADC2Trigger = 0;

uint8_t MAIN_u8ADC1TriggerIndex = 0;
uint8_t MAIN_u8ADC2TriggerIndex = 0;

uint8_t MAIN_bUsePWMShiftL1L2L3 = 1;
uint8_t MAIN_bUsePWMShiftStack2 = 1;

eStackMode_t eStackMode = STACK_MODE_NO_SHIFT;     //STACK_MODE_SHIFT_L1L2L3_S1S2;  //STACK_MODE_SHIFT_S1S2;  //STACK_MODE_SHIFT_L1L2L3;  //
eStackMode_t eStackModeStatus = STACK_MODE_NONE;
eStackMode_t eStackModeStatusLast = STACK_MODE_NONE;

uint8_t bExecControlNewData = 0;
uint16_t u16ExecControlNewDataSkip;
uint16_t u16ExecControlNewDataSkipBuffered;

uint16_t PWM_u16MinDutyTcy =   MCLIB_u16Mul_u16u16Std( MCLIB_u16Fractional(PWM_MIN_DUTY_COEF), LOOPTIME1_TCY );
uint16_t PWM_u16MaxDutyTcy =   MCLIB_u16Mul_u16u16Std( MCLIB_u16Fractional(PWM_MAX_DUTY_COEF), LOOPTIME1_TCY );
uint16_t PWM_u16MinDutyCoef = MCLIB_u16Fractional(PWM_MIN_DUTY_COEF);
uint16_t PWM_u16MaxDutyCoef = MCLIB_u16Fractional(PWM_MAX_DUTY_COEF);
uint16_t PWM_u16StartDutyCoef = MCLIB_u16Fractional(PWM_START_DUTY_COEF);

uint16_t u16LoopTime1Tcy = LOOPTIME1_TCY;
uint16_t u16LoopTime2Tcy = LOOPTIME2_TCY;

bool bStack1FreqReset = 0;
bool bStack2FreqReset = 0;

#endif
#endif /* BOARDCFG_USE_OLD_NON_MCC_ADC */


uint16_t PWM_u16StartDutyTcy =  MCLIB_u16Mul_u16u16Std( MCLIB_u16Fractional(PWM_START_DUTY_COEF), LOOPTIME1_TCY );


#if BOARDCFG_USE_OLD_NON_MCC_PWM
#ifndef BOARDCFG_USE_MCC_PWM

/* Dead Time Resolution 1 nsec */ 
uint16_t DeadTimeMCUStack1[3] = 
{
    DEADTIME_MCU_NSEC,
    DEADTIME_MCU_NSEC,
    DEADTIME_MCU_NSEC,
};

uint16_t DeadTimeMCUStack2[3] = 
{
    DEADTIME_MCU_NSEC,
    DEADTIME_MCU_NSEC,
    DEADTIME_MCU_NSEC,
};


/* Dead Time RAW - Resolution MCU Freq Peripheral Bus  */ 
uint16_t DeadTimeMCUStack1RAW[3] = 
{
    10,
    20,
    30,
};

uint16_t DeadTimeMCUStack2RAW[3] = 
{
    40,
    50,
    60,
};


#endif

#endif /* BOARDCFG_USE_OLD_NON_MCC_PWM */

/* *****************************************************************************
 * ADC Buffers
 **************************************************************************** */
#if BOARDCFG_USE_OLD_NON_MCC_ADC

#ifndef BOARDCFG_USE_MCC_ADC

bool MAIN_bADC1SyncPrimaryTriggersStatusWait = 0;  
bool MAIN_bADC2SyncPrimaryTriggersStatusWait = 0;  

/* no sequence */
#define ADC1_SINGLE_SAMPLE_TRIGGERS_USED  8 
#define ADC1_SINGLE_SAMPLE_TRIGGERS_COUNT TRIGGER_COUNT   /* Count ADC1 Triggers Per PWM Period  PWM1..PWM7=0..6, SpEvTrig=7, SecSpEvTrig=8 */
#define ADC1_SINGLE_SAMPLE_COUNT 4
uint16_t ADC1ResultSingleSample[ADC1_SINGLE_SAMPLE_TRIGGERS_COUNT][ADC1_SINGLE_SAMPLE_COUNT];
uint16_t ADC1ResultSingleSampleCopy[ADC1_SINGLE_SAMPLE_TRIGGERS_COUNT][ADC1_SINGLE_SAMPLE_COUNT];

#define ADC2_SINGLE_SAMPLE_TRIGGERS_USED  8 
#define ADC2_SINGLE_SAMPLE_TRIGGERS_COUNT TRIGGER_COUNT   /* Count ADC2 Triggers Per PWM Period  PWM1..PWM7=0..6, SpEvTrig=7, SecSpEvTrig=8 */
#define ADC2_SINGLE_SAMPLE_COUNT 4
uint16_t ADC2ResultSingleSample[ADC2_SINGLE_SAMPLE_TRIGGERS_COUNT][ADC2_SINGLE_SAMPLE_COUNT];
uint16_t ADC2ResultSingleSampleCopy[ADC2_SINGLE_SAMPLE_TRIGGERS_COUNT][ADC2_SINGLE_SAMPLE_COUNT];

/* sequence */
#define ADC1_SEQUENCE_TRIGGERS_USED_L1L2L3 3 
#define ADC1_SEQUENCE_TRIGGERS_USED 2 
#define ADC1_SEQUENCE_CHANNELS_USED 4
#define ADC1_SEQUENCE_TRIGGERS_COUNT TRIGGER_COUNT   /* Count ADC1 Triggers Per PWM Period  PWM1..PWM7=0..6, SpEvTrig=7, SecSpEvTrig=8 */
#define ADC1_SEQUENCE_COUNT 8
uint16_t ADC1ResultSequence[ADC1_SEQUENCE_TRIGGERS_COUNT][ADC1_SEQUENCE_COUNT];
uint16_t ADC1ResultSequenceCopy[ADC1_SEQUENCE_TRIGGERS_COUNT][ADC1_SEQUENCE_COUNT];

#define ADC2_SEQUENCE_TRIGGERS_USED_L1L2L3 3 
#define ADC2_SEQUENCE_TRIGGERS_USED 2 
#define ADC2_SEQUENCE_CHANNELS_USED 4
#define ADC2_SEQUENCE_TRIGGERS_COUNT TRIGGER_COUNT   /* Count ADC2 Triggers Per PWM Period  PWM1..PWM7=0..6, SpEvTrig=7, SecSpEvTrig=8 */
#define ADC2_SEQUENCE_COUNT 8
uint16_t ADC2ResultSequence[ADC2_SEQUENCE_TRIGGERS_COUNT][ADC2_SEQUENCE_COUNT];
uint16_t ADC2ResultSequenceCopy[ADC2_SEQUENCE_TRIGGERS_COUNT][ADC2_SEQUENCE_COUNT];

#define ADC1_TRIGGERS_COUNT 6   /* Count ADC1 Triggers Per PWM Period */
#define ADC1_MEASURES_COUNT 2   /* Count ADC1 Measures Per Trigger */
uint16_t ADC1Result[ADC1_TRIGGERS_COUNT][ADC1_MEASURES_COUNT];  //??? swap array dimensions

#define ADC2_TRIGGERS_COUNT 6   /* Count ADC2 Triggers Per PWM Period */
#define ADC2_MEASURES_COUNT 2   /* Count ADC2 Measures Per Trigger */
uint16_t ADC2Result[ADC2_TRIGGERS_COUNT][ADC2_MEASURES_COUNT];  //??? swap array dimensions


uint16_t PWM_u16PWMGlobalCoef[TRIGGER_COUNT] = 
{
    MCLIB_u16Fractional(SHIFT_PWM1_COEF),
    MCLIB_u16Fractional(SHIFT_PWM2_COEF),
    MCLIB_u16Fractional(SHIFT_PWM3_COEF),
    MCLIB_u16Fractional(SHIFT_PWM4_COEF),
    MCLIB_u16Fractional(SHIFT_PWM5_COEF),
    MCLIB_u16Fractional(SHIFT_PWM6_COEF),
    MCLIB_u16Fractional(SHIFT_PWM7_COEF),
    MCLIB_u16Fractional(SHIFT_PSEV_COEF),
    MCLIB_u16Fractional(SHIFT_SSEV_COEF),
};

uint16_t PWM_u16PWMGlobalTcy[TRIGGER_COUNT] = 
{
    SHIFT_PWM1_SEQ_TCY,
    SHIFT_PWM2_SEQ_TCY,
    SHIFT_PWM3_SEQ_TCY,
    SHIFT_PWM4_SEQ_TCY,
    SHIFT_PWM5_SEQ_TCY,
    SHIFT_PWM6_SEQ_TCY,
    SHIFT_PWM7_SEQ_TCY, /* PWM7 */   
    SHIFT_PSEV_SEQ_TCY,
    SHIFT_SSEV_SEQ_TCY,
};



uint16_t MAIN_u16PWMTriggersSingleSampleCoef[TRIGGER_COUNT] = 
{
    MCLIB_u16Fractional(SHIFT_PWM1_COEF),
    MCLIB_u16Fractional(SHIFT_PWM2_COEF),
    MCLIB_u16Fractional(SHIFT_PWM3_COEF),
    MCLIB_u16Fractional(SHIFT_PWM4_COEF),
    MCLIB_u16Fractional(SHIFT_PWM5_COEF),
    MCLIB_u16Fractional(SHIFT_PWM6_COEF),
    MCLIB_u16Fractional(SHIFT_PWM7_COEF),
    MCLIB_u16Fractional(SHIFT_PSEV_COEF),
    MCLIB_u16Fractional(SHIFT_SSEV_COEF),
};

uint16_t MAIN_u16PWMTriggersSingleSampleBothStacksCoef[TRIGGER_COUNT] = 
{
    MCLIB_u16Fractional(SHIFT_PWM1_COEF),
    MCLIB_u16Fractional(SHIFT_PWM2_COEF),
    MCLIB_u16Fractional(SHIFT_PWM3_COEF),
    MCLIB_u16Fractional(SHIFT_PWM1_COEF),
    MCLIB_u16Fractional(SHIFT_PWM2_COEF),
    MCLIB_u16Fractional(SHIFT_PWM3_COEF),
    MCLIB_u16Fractional(SHIFT_PWM7_COEF), /* PWM7 */   
    MCLIB_u16Fractional(SHIFT_PSEV_DUAL_STACK_COEF),
    MCLIB_u16Fractional(SHIFT_SSEV_DUAL_STACK_COEF),
};



uint16_t MAIN_u16PWMTriggersSingleSample[TRIGGER_COUNT] = 
{
    SHIFT_PWM1_TCY,
    SHIFT_PWM2_TCY,
    SHIFT_PWM3_TCY,
    SHIFT_PWM4_TCY,
    SHIFT_PWM5_TCY,
    SHIFT_PWM6_TCY,
    SHIFT_PWM7_TCY, /* PWM7 */   
    SHIFT_PSEV_TCY,
    SHIFT_SSEV_TCY,
};

uint16_t MAIN_u16PWMTriggersSingleSampleBothStacks[TRIGGER_COUNT] = 
{
    SHIFT_PWM1_TCY,
    SHIFT_PWM2_TCY,
    SHIFT_PWM3_TCY,
    SHIFT_PWM1_TCY,
    SHIFT_PWM2_TCY,
    SHIFT_PWM3_TCY,
    SHIFT_PWM7_TCY, /* PWM7 */   
    SHIFT_PSEV_DUAL_STACK_TCY,
    SHIFT_SSEV_DUAL_STACK_TCY,
};




uint8_t MAIN_u8ADC1TriggerSequence[TRIGGER_COUNT] = 
{
    TRIGGER_PWM1,
    TRIGGER_PSEV,   /* Primary Special Event */
    TRIGGER_PWM6,   
    TRIGGER_PWM2,
    TRIGGER_PWM4,
    TRIGGER_SSEV,   /* Secondary Special Event */
    TRIGGER_PWM3,
    TRIGGER_PWM5,
    TRIGGER_PWM7,
};

uint8_t MAIN_u8ADC2TriggerSequence[TRIGGER_COUNT] = 
{
    TRIGGER_PWM1,
    TRIGGER_PSEV,   /* Primary Special Event */
    TRIGGER_PWM6,   
    TRIGGER_PWM2,
    TRIGGER_PWM4,
    TRIGGER_SSEV,   /* Secondary Special Event */
    TRIGGER_PWM3,
    TRIGGER_PWM5,
    TRIGGER_PWM7,
};







uint16_t MAIN_u16PWMTriggersSequenceCoef[TRIGGER_COUNT] = 
{
    MCLIB_u16Fractional(SHIFT_PWM1_SEQ_COEF),
    MCLIB_u16Fractional(SHIFT_PWM2_SEQ_COEF),
    MCLIB_u16Fractional(SHIFT_PWM3_SEQ_COEF),
    MCLIB_u16Fractional(SHIFT_PWM4_SEQ_COEF),
    MCLIB_u16Fractional(SHIFT_PWM5_SEQ_COEF),
    MCLIB_u16Fractional(SHIFT_PWM6_SEQ_COEF),
    MCLIB_u16Fractional(SHIFT_PWM7_SEQ_COEF), /* PWM7 */   
    MCLIB_u16Fractional(SHIFT_PSEV_SEQ_COEF),
    MCLIB_u16Fractional(SHIFT_SSEV_SEQ_COEF),
};

uint16_t MAIN_u16PWMTriggersSequenceBothStacksCoef[TRIGGER_COUNT] = 
{
    MCLIB_u16Fractional(SHIFT_PWM1_SEQ_COEF),
    MCLIB_u16Fractional(SHIFT_PWM2_SEQ_COEF),
    MCLIB_u16Fractional(SHIFT_PWM3_SEQ_COEF),
    MCLIB_u16Fractional(SHIFT_PWM1_SEQ_COEF),
    MCLIB_u16Fractional(SHIFT_PWM2_SEQ_COEF),
    MCLIB_u16Fractional(SHIFT_PWM3_SEQ_COEF),
    MCLIB_u16Fractional(SHIFT_PWM7_SEQ_COEF), /* PWM7 */   
    MCLIB_u16Fractional(SHIFT_PSEV_SEQ_COEF),
    MCLIB_u16Fractional(SHIFT_SSEV_SEQ_COEF),
};




uint16_t MAIN_u16PWMTriggersSequence[TRIGGER_COUNT] = 
{
    SHIFT_PWM1_SEQ_TCY,
    SHIFT_PWM2_SEQ_TCY,
    SHIFT_PWM3_SEQ_TCY,
    SHIFT_PWM4_SEQ_TCY,
    SHIFT_PWM5_SEQ_TCY,
    SHIFT_PWM6_SEQ_TCY,
    SHIFT_PWM7_SEQ_TCY, /* PWM7 */   
    SHIFT_PSEV_SEQ_TCY,
    SHIFT_SSEV_SEQ_TCY,
};

uint16_t MAIN_u16PWMTriggersSequenceBothStacks[TRIGGER_COUNT] = 
{
    SHIFT_PWM1_SEQ_TCY,
    SHIFT_PWM2_SEQ_TCY,
    SHIFT_PWM3_SEQ_TCY,
    SHIFT_PWM1_SEQ_TCY,
    SHIFT_PWM2_SEQ_TCY,
    SHIFT_PWM3_SEQ_TCY,
    SHIFT_PWM7_SEQ_TCY, /* PWM7 */   
    SHIFT_PSEV_SEQ_TCY,
    SHIFT_SSEV_SEQ_TCY,
};

uint8_t MAIN_u8ADC1SequenceSequence[TRIGGER_COUNT] = 
{
    TRIGGER_PSEV,   /* Primary Special Event */
    TRIGGER_SSEV,   /* Secondary Special Event */
    TRIGGER_PWM1,
    TRIGGER_PWM2,   
    TRIGGER_PWM3,
    TRIGGER_PWM4,
    TRIGGER_PWM5,
    TRIGGER_PWM6,
    TRIGGER_PWM7,
};

uint8_t MAIN_u8ADC2SequenceSequence[TRIGGER_COUNT] = 
{
    TRIGGER_PSEV,   /* Primary Special Event */
    TRIGGER_SSEV,   /* Secondary Special Event */
    TRIGGER_PWM1,
    TRIGGER_PWM2,   
    TRIGGER_PWM3,
    TRIGGER_PWM4,
    TRIGGER_PWM5,
    TRIGGER_PWM6,
    TRIGGER_PWM7,
};


uint8_t MAIN_u8ADC1SequenceShiftL1L2L3[TRIGGER_COUNT] = 
{
    TRIGGER_PWM1,
    TRIGGER_PWM2,   
    TRIGGER_PWM3,
};

uint8_t MAIN_u8ADC2SequenceShiftL1L2L3[TRIGGER_COUNT] = 
{
    TRIGGER_PWM1,
    TRIGGER_PWM2,   
    TRIGGER_PWM3,
};

#endif

#endif /* BOARDCFG_USE_OLD_NON_MCC_ADC */

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

void PWM_SpecialEvent_CallBack(void);

#if BOARDCFG_USE_OLD_NON_MCC_PWM
#ifndef BOARDCFG_USE_MCC_PWM
void PWM1InterruptFunction(void);
void PWM2InterruptFunction(void);
void PWM3InterruptFunction(void);
void PWM4InterruptFunction(void);
void PWM5InterruptFunction(void);
void PWM6InterruptFunction(void);
#endif
#endif /* BOARDCFG_USE_OLD_NON_MCC_PWM */

#if BOARDCFG_USE_OLD_NON_MCC_ADC

#ifndef BOARDCFG_USE_MCC_ADC
void PWMSecSpEvInterruptFunction(void);
void PWM7InterruptFunction(void);
void vReadNewADCData(void);
void vReadNewADC2Data(void);
#endif

#endif /* BOARDCFG_USE_OLD_NON_MCC_ADC */

void ADC1_CallBack(void);
void ADC2_CallBack(void);






/* *****************************************************************************
 * Functions
 **************************************************************************** */


#ifndef BOARDCFG_USE_MCC_ADC
#if (BOARDCFG_USE_OLD_NON_MCC_ADC == 0)

typedef enum
{
    ADC_1,
    ADC_2,
    ADC_COUNT
}ADC_eModule_t;

typedef struct
{
    union 
    {
        struct 
        {
            uint16_t ADCBUF[16];
            uint16_t ADCON1;
            uint16_t ADCON2;
            uint16_t ADCON3;
            uint16_t ADCHS123;
            uint16_t ADCHS0;
            uint16_t :16;
            uint16_t :16;
            uint16_t ADCSSH;
            uint16_t ADCSSL;
            uint16_t ADCON4;
        };
        
        struct
        {
            uint16_t        ADCBUF0;
            uint16_t        ADCBUF1;
            uint16_t        ADCBUF2;
            uint16_t        ADCBUF3;
            uint16_t        ADCBUF4;
            uint16_t        ADCBUF5;
            uint16_t        ADCBUF6;
            uint16_t        ADCBUF7;
            uint16_t        ADCBUF8;
            uint16_t        ADCBUF9;
            uint16_t        ADCBUFA;
            uint16_t        ADCBUFB;
            uint16_t        ADCBUFC;
            uint16_t        ADCBUFD;
            uint16_t        ADCBUFE;
            uint16_t        ADCBUFF;
            AD1CON1BITS     ADCON1bits;
            AD1CON2BITS     ADCON2bits;
            AD1CON3BITS     ADCON3bits;
            AD1CHS123BITS   ADCHS123bits;
            AD1CHS0BITS     ADCHS0bits;
            uint16_t        :16;
            uint16_t        :16;
            AD1CSSHBITS     ADCSSHbits;
            AD1CSSLBITS     ADCSSLbits;
            AD1CON4BITS     ADCON4bits;
            uint16_t :16;
        };
    };
}ADC_sBaseRegisters_t;

typedef struct
{
    ADC_sBaseRegisters_t* psBase[ADC_COUNT];
    uint8_t u8BitOffsetsISR[ADC_COUNT];
}ADC_sHandle_t;


ADC_sHandle_t ADC_sHandle = 
{
    {
        (ADC_sBaseRegisters_t*)(&ADC1BUF0),
        (ADC_sBaseRegisters_t*)(&ADC2BUF0),
    },
    {   /* Isr ADC Done */
        13 + 0*16,          /* ADC1 */         
         5 + 1*16,          /* ADC2 */                       
    },
};




void ADC_vInitialize(ADC_eModule_t eModule)
{
    ADC_sBaseRegisters_t* psBase;
    psBase = ADC_sHandle.psBase[eModule];
    uint8_t u8Bitoffset = ADC_sHandle.u8BitOffsetsISR[eModule];

    psBase->ADCON1bits.ADON = 0;
    psBase->ADCON1 = 0;
    psBase->ADCON2 = 0;
    psBase->ADCON3 = 0;
    psBase->ADCON4 = 0;

    psBase->ADCON1bits.FORM = 2;                        /* Unsigned fractional (DOUT = dddd dddd dd00 0000) - !!! fix to work with this on old TSC */
    //psBase->ADCON1bits.FORM = 3;                        /* Signed fractional (DOUT = sddd dddd dd00 0000 where s = .NOT.d<11>) */
    psBase->ADCON1bits.AD12B = 1;                       /* 12 Bit 1 Channel Operation by default where possible */
    psBase->ADCON2bits.ALTS = 0;                        /* Always uses channel input selects for Sample A */

    /* TAD Clock */
    psBase->ADCON3bits.ADRC = 0;                        /* if 1 ADCS ignored */
    psBase->ADCON3bits.ADCS = (uint8_t)(TAD_TCY - 1);   /* 7+1 Tcy -> 133ns per TAD @ 60MHz (117.6 nsec min on datasheet)) */

    #if defined __dsPIC33EP512MU814__
    /* psBase->ADCON2bits.VCFG = 0b000;                    AVDD Avss */
    psBase->ADCON2bits.VCFG = 0b001;                    /* External VREF+ Avss */
    /* psBase->ADCON2bits.VCFG = 0b010;                    AVDD External VREF- */
    /* psBase->ADCON2bits.VCFG = 0b011;                    External VREF+ External VREF- */
    #else
    if (eModule == ADC_1)
    {
        psBase->ADCON2bits.VCFG = 0b001;                    /* External VREF+ Avss */
    }
    else    /* VREF not supported from ADC2 */
    {
        psBase->ADCON2bits.VCFG = 0b000;                    /* AVDD Avss */    
    }
    #endif
    
    
    psBase->ADCON3bits.SAMC = SAMPLETIME_TCY_12BIT;     /* count TAD sampling */
    psBase->ADCON1bits.SIMSAM = 0;                      /* in sequence */                 
    psBase->ADCON2bits.CHPS = 0;                        /* Sample/Convert one channel */
    psBase->ADCON1bits.SSRCG = 0; 

    switch (eModule)
    {
        case ADC_1:
            psBase->ADCSSH = AD1CSSH_MASK;
            psBase->ADCSSL = AD1CSSL_MASK;
            psBase->ADCON2bits.SMPI = AD1CSS_COUNT-1;   /* count scan channels on special event trigger */            
            ISR_vSetDatIPC(u8Bitoffset,ISR_PRIORITY_ADC1);
            break;
        case ADC_2:
            psBase->ADCSSH = AD2CSSH_MASK;
            psBase->ADCSSL = AD2CSSL_MASK;
            psBase->ADCON2bits.SMPI = AD2CSS_COUNT-1;   /* count scan channels on special event trigger */            
            ISR_vSetDatIPC(u8Bitoffset,ISR_PRIORITY_ADC2);
            break;
        default:
            psBase->ADCSSH = 0;
            psBase->ADCSSL = 0;
            psBase->ADCON2bits.SMPI = 0;                /* count scan channels on special event trigger */            
            ISR_vSetDatIPC(u8Bitoffset,4);
            break;
    }
    
    psBase->ADCON2bits.CSCNA = 0;                       /* deselect scan inputs in ADCSS at startup */
    psBase->ADCON1bits.ASAM = 0;                        /* stop AutoSample */
    psBase->ADCON1bits.SSRC = 0;                        /* 0- Clearing the Sample bit (SAMP) ends sampling and starts conversion (Manual mode) */
    psBase->ADCON1bits.SAMP = 1;

    /* clear and Enable ADC interrupt */
    ISR_vClrBitIFS(u8Bitoffset);
    ISR_vSetBitIEC(u8Bitoffset);
    
    psBase->ADCON1bits.ADON = 1;   /* Turn on A/D module */      
}


void ADC_vInit(void)
{  
    ADC_vInitialize(ADC_1);
    ADC_vInitialize(ADC_2);
}


#endif /* BOARDCFG_USE_OLD_NON_MCC_ADC */
#endif






#if BOARDCFG_USE_OLD_NON_MCC_ADC
#ifndef BOARDCFG_USE_MCC_ADC

/* *****************************************************************************
 * ADC Related Functions - ADC Modules
 **************************************************************************** */
void ADC_vStop(void)
{
    AD1CON1bits.ADON = 0;
    AD2CON1bits.ADON = 0;
    
    IEC0bits.AD1IE = 0;
    IFS0bits.AD1IF = 0; 
    
    IEC1bits.AD2IE = 0;
    IFS1bits.AD2IF = 0; 
}

void ADC1_vReset(void)
{
    /* ADC1 */

    /* Default Configuration Below */
    {
        AD1CON1 = 0;
        AD1CON2 = 0;
        AD1CON3 = 0;
        AD1CON4 = 0;
        
        //AD1CON1bits.FORM = 0; 
        AD1CON1bits.FORM = 3;/* Signed fractional (DOUT = sddd dddd dd00 0000) */
        
        AD1CON1bits.AD12B = 0;  /* 10 Bit 1 Channel Operation by default */
        AD1CON2bits.ALTS = 0;
        /* TAD Clock */
        AD1CON3bits.ADRC = 0;/* if 1 ADCS ignored */
        AD1CON3bits.ADCS = (uint8_t)(TAD_TCY - 1);/* 7+1 Tcy -> 133ns per TAD @ 60MHz (117.6 nsec min on datasheet)) */
        
        //AD1CON2bits.VCFG = 0b000;   /* AVDD Avss */
        AD1CON2bits.VCFG = 0b001;   /* External VREF+ Avss */
        //AD1CON2bits.VCFG = 0b010;   /* AVDD External VREF- */
        //AD1CON2bits.VCFG = 0b011;   /* External VREF+ External VREF- */
    }
    
    MAIN_bUseADC1SyncPrimaryTriggers = MAIN_bUseADC1SyncPrimaryTriggersRequest;
    
    MAIN_bADC1Status12BitMode = MAIN_bADC1Preferre12BitMode;
    MAIN_bADC1StatusFourChannelOperation = MAIN_bADC1PreferreFourChannelOperation;
    
    /* Specific Configuration Below for Sample/Convert Method */
    if(MAIN_bUseSequenceADC1)
    {
        MAIN_bADC1StatusFourChannelOperation = 0; /* denied in sequence mode */
        
        if (MAIN_bADC1Status12BitMode)
        {
            AD1CON1bits.AD12B = 1;      /* 12 Bit 1 Channel Operation preferred */
            AD1CON3bits.SAMC = SAMPLETIME_TCY_12BIT;  //count TAD sampling

        }
        else
        {
            AD1CON1bits.AD12B = 0;      /* 10 Bit 1..4 Channel Operation preferred */
            AD1CON3bits.SAMC = SAMPLETIME_TCY_10BIT;  //count TAD sampling 
        }
        AD1CON1bits.SIMSAM = 0;  /* in sequence */
        AD1CON2bits.CSCNA = 1;  /* scan inputs in AD1CSS */
        
        AD1CHS0bits.CH0SA = L1_CURRENT_1;
        
        AD1CSSH = 0;
        
        AD1CON2bits.CHPS = 0;  /* Sample/Convert one channel */
        AD1CON1bits.SSRCG = 0;
        AD1CON1bits.SSRC = 7;//7- internal counter start conversion
        //AD1CON1bits.SSRC = 0;//0- Clearing the Sample bit (SAMP) ends sampling and starts conversion (Manual mode)
        
        if (MAIN_bUseADC1SyncPrimaryTriggers)   /* Shift S1S2   or   Shift L1L2L3 */
        {
           
            
            MAIN_u8ADC1TriggerIndex = 0;
            if (MAIN_bUsePWMShiftStack2)    /* Shift S1S2 */
            {   
                AD1CSSL = AD1CSSL_PSEV;     /* Start with First Trigger */  
                MAIN_u8ADC1Trigger = MAIN_u8ADC1SequenceSequence[MAIN_u8ADC1TriggerIndex];
            }
            else                            /* Shift L1L2L3 */
            {
                AD1CSSL = ADC1_PWM1_CHANNELS_L1L2L3;     /* Start with First Trigger */  
                MAIN_u8ADC1Trigger = MAIN_u8ADC1SequenceShiftL1L2L3[MAIN_u8ADC1TriggerIndex];  
            }
            
            //MAIN_bADC1SyncPrimaryTriggersStatus = 0;
            //MAIN_bADC1SyncPrimaryTriggersStatusWait = 0;    
            AD1CON2bits.SMPI = ADC1_SEQUENCE_CHANNELS_USED-1; //scan on trigger - both variants 4 channels in sequence
            //AD1CON1bits.ASAM = 1;   // SAMP bit is auto set.
            AD1CON1bits.ASAM = 0;   // no SAMP bit is auto set.
        }
        else    /* No Shift */
        {
            AD1CSSL  = AD1CSSL_NO_SHIFT;
            AD1CON2bits.SMPI = 8-1; //scan 8 channels on special event trigger
            AD1CON1bits.ASAM = 0;   // no SAMP bit is auto set.
        }
        

    }
    else /* Not Sequence (AD1CSS not used) */
    {
        if (MAIN_bUseADC1SyncPrimaryTriggers)   /* Shift L1L2L3_S1S2 */
        {
            /* Start with First Trigger */
            AD1CHS0bits.CH0SA = ADC1_PWM1_NO_SEQUENCE_CH0;   
            AD1CHS123bits.CH123SA = 0;  
            AD1CON1bits.SSRC = 0;
            AD1CON1bits.SSRCG = 1;
            MAIN_u8ADC1TriggerIndex = 0;
            MAIN_u8ADC1Trigger = MAIN_u8ADC1TriggerSequence[MAIN_u8ADC1TriggerIndex];
        }
        else /* on special event trigger */
        {
            MAIN_bADC1Status12BitMode = 0;    /* Override Request For 12-bit measurement */
            MAIN_bADC1StatusFourChannelOperation = 1; /* override in this mode mode */
            AD1CON1bits.SSRC = 3;//3- PWM primary Special Event Trigger start conversion

            /* Specific Configuration Below for Selected Stack Or Channels */
            if (MAIN_bUseStack1ADC1)
            {
                AD1CHS0bits.CH0SA = DC_VOLTAGE_1;   /* Power supply DC-Link */
                AD1CHS123bits.CH123SA = 0;  /* CH1 is AN0, CH2 is AN1, CH3 is AN2 */ 
            }
            else
            {
                AD1CHS0bits.CH0SA = DC_VOLTAGE_2;   /* Power supply DC-Link */
                AD1CHS123bits.CH123SA = 1;  /* CH1 is AN3, CH2 is AN4, CH3 is AN5 */
            }
            MAIN_bUseStack1ADC1State = MAIN_bUseStack1ADC1;
            AD1CON1bits.SSRCG = 0;
        }

        if (MAIN_bADC1Status12BitMode)
        {
            AD1CON1bits.AD12B = 1;      /* 12 Bit 1 Channel Operation preferred */
            MAIN_bADC1StatusFourChannelOperation = 0; /* override in this mode */
        }
        
        if(MAIN_bADC1StatusFourChannelOperation)
        {
            AD1CON1bits.SIMSAM = 1;  /*  simultaneously */
            AD1CON2bits.CHPS = 2; /* Samples CH0, CH1, CH2, CH3 simultaneously */ 
        }
        else
        {
            AD1CON1bits.SIMSAM = 0;  /*  single channel sample */
            AD1CON2bits.CHPS = 0; /* Samples CH0 only */ 
        }
        
        AD1CON3bits.SAMC = 0;  //count TAD sampling 
        AD1CON2bits.SMPI = 1-1; //scan simultaneously 4 channels or 1 channel
        AD1CON2bits.CSCNA = 0;  /* no scan inputs in AD1CSS */
        AD1CON1bits.ASAM = 1;   /* each measurement is triggered from the ISR */

    }
    MAIN_bUseSequenceADC1State = MAIN_bUseSequenceADC1;      
}

void ADC2_vReset (void)
{
    /* ADC2 */

    /* Default Configuration Below */
    {
        AD2CON1 = 0;
        AD2CON2 = 0;
        AD2CON3 = 0;
        AD2CON4 = 0;
        
        //AD2CON1bits.FORM = 0; 
        AD2CON1bits.FORM = 3;/* Signed fractional (DOUT = sddd dddd dd00 0000) */
        
        AD2CON2bits.ALTS = 0;
        /* TAD Clock */
        AD2CON3bits.ADRC = 0;/* if 1 ADCS ignored */
        AD2CON3bits.ADCS = (uint8_t)(TAD_TCY - 1);/* 7+1 Tcy -> 133ns per TAD @ 60MHz (117.6 nsec min on datasheet)) */
        
        //AD2CON2bits.VCFG = 0b000;   /* AVDD Avss */
        AD2CON2bits.VCFG = 0b001;   /* External VREF+ Avss */
        //AD2CON2bits.VCFG = 0b010;   /* AVDD External VREF- */
        //AD2CON2bits.VCFG = 0b011;   /* External VREF+ External VREF- */
    }
    
    MAIN_bUseADC2SyncPrimaryTriggers = MAIN_bUseADC2SyncPrimaryTriggersRequest;
    MAIN_bADC2StatusFourChannelOperation = MAIN_bADC2PreferreFourChannelOperation;

    /* Specific Configuration Below for Sample/Convert Method */
    if(MAIN_bUseSequenceADC2)
    {
        MAIN_bADC2StatusFourChannelOperation = 0; /* denied in sequence mode */
        
        AD2CON3bits.SAMC = (uint8_t)(SAMPLETIME_TCY_10BIT);  //count TAD sampling 

        AD2CON1bits.SIMSAM = 0;  /* in sequence */
        AD2CON2bits.CSCNA = 1;  /* scan inputs in AD2CSS */
        
        AD2CON2bits.CHPS = 0;  /* Sample/Convert one channel */
        AD2CON1bits.SSRCG = 0;
        AD2CON1bits.SSRC = 7;//7- internal counter start conversion
        //AD2CON1bits.SSRC = 0;//0- Clearing the Sample bit (SAMP) ends sampling and starts conversion (Manual mode)
        
        if (MAIN_bUseADC2SyncPrimaryTriggers)
        {
            MAIN_u8ADC2TriggerIndex = 0;
            if (MAIN_bUsePWMShiftStack2)    /* Shift S1S2 */
            {   
                AD2CSSL = AD2CSSL_PSEV;     /* Start with First Trigger */  
                MAIN_u8ADC2Trigger = MAIN_u8ADC2SequenceSequence[MAIN_u8ADC2TriggerIndex];
            }
            else                            /* Shift L1L2L3 */
            {
                AD2CSSL = ADC2_PWM1_CHANNELS_L1L2L3;     /* Start with First Trigger */  
                MAIN_u8ADC2Trigger = MAIN_u8ADC2SequenceShiftL1L2L3[MAIN_u8ADC2TriggerIndex];  
            }
 
            //MAIN_bADC2SyncPrimaryTriggersStatus = 0;
            //MAIN_bADC2SyncPrimaryTriggersStatusWait = 0;
            AD2CON2bits.SMPI = ADC2_SEQUENCE_CHANNELS_USED-1; //scan on trigger
            //AD2CON1bits.ASAM = 1;   // SAMP bit is auto set.
            AD2CON1bits.ASAM = 0;   // no SAMP bit is auto set.
        }
        else
        {
            AD2CSSL  = AD2CSSL_NO_SHIFT; 
            AD2CON2bits.SMPI = 8-1; //scan 8 channels on special event trigger
            AD2CON1bits.ASAM = 0;   // no SAMP bit is auto set.
        }
        
        
    }
    else /* Not Sequence (AD2CSS not used) */
    {
        if (MAIN_bUseADC2SyncPrimaryTriggers)
        {
            /* Start with PWM1 Trigger */
            AD2CHS0bits.CH0SA = ADC2_PWM1_NO_SEQUENCE_CH0;   
            AD2CHS123bits.CH123SA = 0;  
            AD2CON1bits.SSRC = 0;
            AD2CON1bits.SSRCG = 1;
            MAIN_u8ADC2TriggerIndex = 0;
            MAIN_u8ADC2Trigger = MAIN_u8ADC2TriggerSequence[MAIN_u8ADC2TriggerIndex];
        }
        else /* on special event trigger */
        {
            MAIN_bADC2StatusFourChannelOperation = 1; /* override in this mode mode */
            
            AD2CON1bits.SSRC = 5;//5- PWM secondary Special Event Trigger start conversion

            /* Specific Configuration Below for Selected Stack Or Channels */
            if (MAIN_bUseStack1ADC2)
            {
                AD2CHS0bits.CH0SA = DC_VOLTAGE_1;   /* Power supply DC-Link */
                AD2CHS123bits.CH123SA = 0;  /* CH1 is AN0, CH2 is AN1, CH3 is AN2 */ 
            }
            else
            {
                AD2CHS0bits.CH0SA = DC_VOLTAGE_2;   /* Power supply DC-Link */
                AD2CHS123bits.CH123SA = 1;  /* CH1 is AN3, CH2 is AN4, CH3 is AN5 */
            }
            MAIN_bUseStack1ADC2State = MAIN_bUseStack1ADC2;
            AD2CON1bits.SSRCG = 0;
        }

        if(MAIN_bADC2StatusFourChannelOperation)
        {
            AD2CON1bits.SIMSAM = 1;  /*  simultaneously */
            AD2CON2bits.CHPS = 2; /* Samples CH0, CH1, CH2, CH3 simultaneously */ 
        }
        else
        {
            AD2CON1bits.SIMSAM = 0;  /*  single channel sample */
            AD2CON2bits.CHPS = 0; /* Samples CH0 only */ 
        }
        
        AD2CON3bits.SAMC = 0;  //count TAD sampling 
        AD2CON2bits.SMPI = 1-1; //scan simultaneously 4 channels or 1 channel
        AD2CON2bits.CSCNA = 0;  /* scan inputs in AD2CSS */
        AD2CON1bits.ASAM = 1;   /* auto start sampling */     
        
    }
    MAIN_bUseSequenceADC2State = MAIN_bUseSequenceADC2;
}

void ADC_vInterruptsSetup(void)
{
    // ADC Interrupt Priority
    _AD1IP = ISR_PRIORITY_ADC1;
    _AD2IP = ISR_PRIORITY_ADC2;
    
    // clear and Enable ADC interrupt 
    IFS0bits.AD1IF = 0; 
    IEC0bits.AD1IE = 1;

    // clear and Enable ADC interrupt 
    IFS1bits.AD2IF = 0; 
    IEC1bits.AD2IE = 1;
}

void ADC_vStart(void)
{
    AD1CON1bits.ADON = 1;   /* Turn on A/D module */      
    AD2CON1bits.ADON = 1;   /* Turn on A/D module */      
}

void ADC_vInit(void)
{  
    /* ANSEL: ADC Port Configuration Register - set ports as analog  */
	ANC1_ANSEL = 1;	
	ANC2_ANSEL = 1;	
	ANC3_ANSEL = 1;	
	ANC4_ANSEL = 1;	
	ANC5_ANSEL = 1;	
	ANC6_ANSEL = 1;	
	ANC7_ANSEL = 1;	
	ANC8_ANSEL = 1;	
	ANC9_ANSEL = 1;	
	ANC10_ANSEL = 1;	
	ANC11_ANSEL = 1;	
	ANC12_ANSEL = 1;	
	ANC13_ANSEL = 1;	
	ANC14_ANSEL = 1;	
	ANC15_ANSEL = 1;	
	ANC16_ANSEL = 1;	
}

void vReadNewADCData(void)
{
    eStackModeStatusLast = eStackModeStatus; //to be fixed if runtime eStackModeStatus change requested

    switch (eStackModeStatusLast)
    {
        
        case STACK_MODE_SHIFT_L1L2L3_S1S2:
            ADCStack1[ADC_STACK_CURRENT_L1] = ADC1ResultSingleSampleCopy[0][0]; /* Trigger PWM1 */
            ADCStack1[ADC_STACK_CURRENT_L2] = ADC1ResultSingleSampleCopy[1][0]; /* Trigger PWM2 */
            ADCStack1[ADC_STACK_CURRENT_L3] = ADC1ResultSingleSampleCopy[2][0]; /* Trigger PWM3 */
            ADCStack1[ADC_STACK_DC_VOLTAGE] = ADC1ResultSingleSampleCopy[7][0]; /* Trigger PSEV */
            ADCStack2[ADC_STACK_CURRENT_L1] = ADC1ResultSingleSampleCopy[3][0]; /* Trigger PWM4 */ 
            ADCStack2[ADC_STACK_CURRENT_L2] = ADC1ResultSingleSampleCopy[4][0]; /* Trigger PWM5 */
            ADCStack2[ADC_STACK_CURRENT_L3] = ADC1ResultSingleSampleCopy[5][0]; /* Trigger PWM6 */
            ADCStack2[ADC_STACK_DC_VOLTAGE] = ADC1ResultSingleSampleCopy[8][0]; /* Trigger SSEV */
            break;
            
        case STACK_MODE_SHIFT_S1S2:
            ADCStack1[ADC_STACK_CURRENT_L1] = ADC1ResultSequenceCopy[0][0]; /* Trigger PSEV */
            ADCStack1[ADC_STACK_CURRENT_L2] = ADC1ResultSequenceCopy[0][1];
            ADCStack1[ADC_STACK_CURRENT_L3] = ADC1ResultSequenceCopy[0][2];
            ADCStack1[ADC_STACK_DC_VOLTAGE] = ADC1ResultSequenceCopy[0][3];
            ADCStack2[ADC_STACK_CURRENT_L1] = ADC1ResultSequenceCopy[1][0]; /* Trigger SSEV */
            ADCStack2[ADC_STACK_CURRENT_L2] = ADC1ResultSequenceCopy[1][1];
            ADCStack2[ADC_STACK_CURRENT_L3] = ADC1ResultSequenceCopy[1][2];
            ADCStack2[ADC_STACK_DC_VOLTAGE] = ADC1ResultSequenceCopy[1][3];
            break;
            
        case STACK_MODE_SHIFT_L1L2L3:
            ADCStack1[ADC_STACK_CURRENT_L1] = ADC1ResultSequenceCopy[0][0]; /* Trigger PWM1 */
            ADCStack1[ADC_STACK_CURRENT_L2] = ADC1ResultSequenceCopy[1][0]; /* Trigger PWM2 */
            ADCStack1[ADC_STACK_CURRENT_L3] = ADC1ResultSequenceCopy[2][0]; /* Trigger PWM3 */
            ADCStack1[ADC_STACK_DC_VOLTAGE] = ADC1ResultSequenceCopy[0][2]; /* Trigger PWM1 */
            ADCStack2[ADC_STACK_CURRENT_L1] = ADC1ResultSequenceCopy[0][1]; /* Trigger PWM1 */ 
            ADCStack2[ADC_STACK_CURRENT_L2] = ADC1ResultSequenceCopy[1][1]; /* Trigger PWM2 */
            ADCStack2[ADC_STACK_CURRENT_L3] = ADC1ResultSequenceCopy[2][1]; /* Trigger PWM3 */
            ADCStack2[ADC_STACK_DC_VOLTAGE] = ADC1ResultSequenceCopy[0][3]; /* Trigger PWM1 */
            break;
            
        case STACK_MODE_NO_SHIFT:
            ADCStack1[ADC_STACK_CURRENT_L1] = ADC1ResultSequenceCopy[0][0]; /* Trigger PSEV */
            ADCStack1[ADC_STACK_CURRENT_L2] = ADC1ResultSequenceCopy[0][1];
            ADCStack1[ADC_STACK_CURRENT_L3] = ADC1ResultSequenceCopy[0][2];
            ADCStack1[ADC_STACK_DC_VOLTAGE] = ADC1ResultSequenceCopy[0][6];
            ADCStack2[ADC_STACK_CURRENT_L1] = ADC1ResultSequenceCopy[0][3]; 
            ADCStack2[ADC_STACK_CURRENT_L2] = ADC1ResultSequenceCopy[0][4];
            ADCStack2[ADC_STACK_CURRENT_L3] = ADC1ResultSequenceCopy[0][5];
            ADCStack2[ADC_STACK_DC_VOLTAGE] = ADC1ResultSequenceCopy[0][7];
            break;
            
        default:
            break;
    }
    
}

void vReadNewADC2Data(void)
{
    eStackModeStatusLast = eStackModeStatus; //to be fixed if runtime eStackModeStatus change requested

    switch (eStackModeStatusLast)
    {

        case STACK_MODE_SHIFT_L1L2L3_S1S2:
            ADC2Stack1[ADC2_STACK_CURRFBK_L1] = ADC2ResultSingleSampleCopy[0][0]; /* Trigger PWM1 */
            ADC2Stack1[ADC2_STACK_CURRFBK_L2] = ADC2ResultSingleSampleCopy[1][0]; /* Trigger PWM2 */
            ADC2Stack1[ADC2_STACK_CURRFBK_L3] = ADC2ResultSingleSampleCopy[2][0]; /* Trigger PWM3 */
            ADC2Stack1[ADC2_STACK_DCVOLTAGE2] = ADC2ResultSingleSampleCopy[7][0]; /* Trigger PSEV */
            ADC2Stack1[ADC2_STACK_TEMPFBK_L1] = ADC2ResultSingleSampleCopy[3][0]; /* Trigger PWM4 */ 
            ADC2Stack1[ADC2_STACK_TEMPFBK_L2] = ADC2ResultSingleSampleCopy[4][0]; /* Trigger PWM5 */
            ADC2Stack1[ADC2_STACK_TEMPFBK_L3] = ADC2ResultSingleSampleCopy[5][0]; /* Trigger PWM6 */
            ADC2Stack2[ADC2_STACK_DCVOLTAGE2] = ADC2ResultSingleSampleCopy[8][0]; /* Trigger SSEV */
            break;
      
        case STACK_MODE_SHIFT_S1S2:
            ADC2Stack1[ADC2_STACK_CURRFBK_L1] = ADC2ResultSequenceCopy[0][1]; /* Trigger PSEV */
            ADC2Stack1[ADC2_STACK_CURRFBK_L2] = ADC2ResultSequenceCopy[0][2];
            ADC2Stack1[ADC2_STACK_CURRFBK_L3] = ADC2ResultSequenceCopy[0][3];
            ADC2Stack1[ADC2_STACK_DCVOLTAGE2] = ADC2ResultSequenceCopy[0][0];
            ADC2Stack1[ADC2_STACK_TEMPFBK_L1] = ADC2ResultSequenceCopy[1][1]; /* Trigger SSEV */
            ADC2Stack1[ADC2_STACK_TEMPFBK_L2] = ADC2ResultSequenceCopy[1][2];
            ADC2Stack1[ADC2_STACK_TEMPFBK_L3] = ADC2ResultSequenceCopy[1][3];
            ADC2Stack2[ADC2_STACK_DCVOLTAGE2] = ADC2ResultSequenceCopy[1][0];
            break;
            
        case STACK_MODE_SHIFT_L1L2L3:
            ADC2Stack1[ADC2_STACK_CURRFBK_L1] = ADC2ResultSequenceCopy[0][2]; /* Trigger PWM1 */
            ADC2Stack1[ADC2_STACK_CURRFBK_L2] = ADC2ResultSequenceCopy[1][2]; /* Trigger PWM2 */
            ADC2Stack1[ADC2_STACK_CURRFBK_L3] = ADC2ResultSequenceCopy[2][2]; /* Trigger PWM3 */
            ADC2Stack1[ADC2_STACK_DCVOLTAGE2] = ADC2ResultSequenceCopy[0][0]; /* Trigger PWM1 */
            ADC2Stack1[ADC2_STACK_TEMPFBK_L1] = ADC2ResultSequenceCopy[0][3]; /* Trigger PWM1 */ 
            ADC2Stack1[ADC2_STACK_TEMPFBK_L2] = ADC2ResultSequenceCopy[1][3]; /* Trigger PWM2 */
            ADC2Stack1[ADC2_STACK_TEMPFBK_L3] = ADC2ResultSequenceCopy[2][3]; /* Trigger PWM3 */
            ADC2Stack2[ADC2_STACK_DCVOLTAGE2] = ADC2ResultSequenceCopy[0][1]; /* Trigger PWM1 */
            break;
            
        case STACK_MODE_NO_SHIFT:
            ADC2Stack1[ADC2_STACK_CURRFBK_L1] = ADC2ResultSequenceCopy[0][2]; /* Trigger PSEV */
            ADC2Stack1[ADC2_STACK_CURRFBK_L2] = ADC2ResultSequenceCopy[0][3];
            ADC2Stack1[ADC2_STACK_CURRFBK_L3] = ADC2ResultSequenceCopy[0][4];
            ADC2Stack1[ADC2_STACK_DCVOLTAGE2] = ADC2ResultSequenceCopy[0][0];
            ADC2Stack1[ADC2_STACK_TEMPFBK_L1] = ADC2ResultSequenceCopy[0][5]; 
            ADC2Stack1[ADC2_STACK_TEMPFBK_L2] = ADC2ResultSequenceCopy[0][6];
            ADC2Stack1[ADC2_STACK_TEMPFBK_L3] = ADC2ResultSequenceCopy[0][7];
            ADC2Stack2[ADC2_STACK_DCVOLTAGE2] = ADC2ResultSequenceCopy[0][1];
            break;
            
        default:
            break;
    }
    
}

void AD1Interrupt(void)
{
    uint8_t bExecControl;
    //uint16_t u16SelectedChannels = 0;
    uint8_t u8CurrentChannel;
    uint16_t* pu16ADCBUF;
    uint8_t indexTrigger;
    uint8_t indexChannel;
    
    DEBUG_SET_ADC_1();  
    
    bExecControl = 0;
    u8CurrentChannel = 0;
    //D0_LAT = 1;   
    //D1_LAT = 1;
    
    if (MAIN_bUseADC1SyncPrimaryTriggers)
    {

        if(MAIN_bUseSequenceADC1State)  /* Shift S1S2  or Shift L1L2L3 */
        {
 
#if 1
            
            if (MAIN_bADC1SyncPrimaryTriggersStatus == 0)   /* skip not triggered result */
            {
                AD1CON1bits.ASAM = 0; 
                //AD1CON2bits.CSCNA = 1;  /* scan inputs from beginning */
                //AD1CON1bits.SSRC = 0;//0- Clearing the Sample bit (SAMP) ends sampling and starts conversion (Manual mode)
                //AD1CON1bits.SAMP = 1; 
                //MAIN_bADC1SyncPrimaryTriggersStatusWait = 1;
            }
            else
            {
                pu16ADCBUF = (uint16_t*)&ADC1BUF0;
                u8CurrentChannel = 0;
                while(u8CurrentChannel < ADC1_SEQUENCE_CHANNELS_USED)
                {
                    ADC1ResultSequence[MAIN_u8ADC1TriggerIndex][u8CurrentChannel++] = *pu16ADCBUF++;
                }

                if (MAIN_bUsePWMShiftStack2)        /* Shift S1S2 */
                {
                    MAIN_u8ADC1TriggerIndex++;
                    if (MAIN_u8ADC1TriggerIndex >= ADC1_SEQUENCE_TRIGGERS_USED)
                    {
                        MAIN_u8ADC1TriggerIndex = 0;
                        for (indexTrigger = 0; indexTrigger < ADC1_SEQUENCE_TRIGGERS_COUNT; indexTrigger++)
                        {
                            for (indexChannel = 0; indexChannel < ADC1_SEQUENCE_COUNT; indexChannel++)
                            {
                                ADC1ResultSequenceCopy[indexTrigger][indexChannel] = ADC1ResultSequence[indexTrigger][indexChannel];
                            }
                        }
                        bExecControl = 1;
                    }
                    
                    MAIN_u8ADC1Trigger = MAIN_u8ADC1SequenceSequence[MAIN_u8ADC1TriggerIndex];
                    switch (MAIN_u8ADC1Trigger)
                    {
                        case TRIGGER_PSEV:
                            AD1CSSL = AD1CSSL_PSEV;
                            break;

                        case TRIGGER_SSEV:
                            AD1CSSL = AD1CSSL_SSEV;
                            break;

                        default:
                            break;
                    }
                }
                else            /* Shift L1L2L3 */
                {
                    MAIN_u8ADC1TriggerIndex++;
                    if (MAIN_u8ADC1TriggerIndex >= ADC1_SEQUENCE_TRIGGERS_USED_L1L2L3)
                    {
                        MAIN_u8ADC1TriggerIndex = 0;
                        bExecControl = 1;
                    }
                    
                    MAIN_u8ADC1Trigger = MAIN_u8ADC1SequenceShiftL1L2L3[MAIN_u8ADC1TriggerIndex];
                    switch (MAIN_u8ADC1Trigger)
                    {
                        case TRIGGER_PWM1:
                            AD1CSSL = ADC1_PWM1_CHANNELS_L1L2L3;
                            break;

                        case TRIGGER_PWM2:
                            AD1CSSL = ADC1_PWM2_CHANNELS_L1L2L3;
                            break;

                        case TRIGGER_PWM3:
                            AD1CSSL = ADC1_PWM3_CHANNELS_L1L2L3;
                            break;

                        default:
                            break;
                    }                    
                }
                //IEC0bits.AD1IE = 0;
                //AD1CON2bits.CSCNA = 1;  /* scan inputs from beginning */
                MAIN_bADC1SyncPrimaryTriggersStatus = 0;
            }
            
#else
            if (MAIN_bADC1SyncPrimaryTriggersStatus == 1)
            {
                pu16ADCBUF = (uint16_t*)&ADC1BUF0;
                //pu16ADCBUF += ADC1ResultOffset;  /* first are with the previous channels and to be skipped */
                u8CurrentChannel = 0;
                while(u8CurrentChannel < ADC1_MEASURES_COUNT)
                {
                    ADC1Result[MAIN_u8ADC1Trigger][u8CurrentChannel++] = *pu16ADCBUF++;
                }
                ADC1ResultOffset += ADC1_MEASURES_COUNT;
                ADC1ResultOffset &= 0xF;

                pu16ADCBUF = (uint16_t*)&ADC1BUF0;
                u16SelectedChannels = AD1CSSL;
                u8CurrentChannel = 0;
                while (u16SelectedChannels > 0)
                {
                    if (((u16SelectedChannels & 1) > 0) && (u8CurrentChannel < ADC_BUFFER_LENGTH))
                    {
                        ADCValues[u8CurrentChannel] = *pu16ADCBUF++;
                    }
                    u8CurrentChannel++;
                    u16SelectedChannels >>= 1;
                }

                if (MAIN_u8ADC1Trigger == 5)
                {
                    if (MAIN_bADC1MainFreemasterTrigger == 1)
                    {
                        if (MAIN_bADC2Trigger == 0)
                        {
                            bExecControl = 1;
                        }
                        else
                        {
                            MAIN_bADC2TriggerFreemaster = 1;
                        }

                    }
                    else
                    {
                        if (MAIN_bADC1TriggerFreemaster == 1)
                        {
                            bExecControl = 1;
                        }
                    }
                    MAIN_bADC1Trigger = 0;
                }
            }
#endif
        }
        else /* MAIN_bUseADC1SyncPrimaryTriggers without MAIN_bUseSequenceADC1State */  /* Shift L1L2L3_S1S2 */
        {
            pu16ADCBUF = (uint16_t*)&ADC1BUF0;
            u8CurrentChannel = 0;
            while(u8CurrentChannel < ADC1_SINGLE_SAMPLE_COUNT)
            {
                ADC1ResultSingleSample[MAIN_u8ADC1Trigger][u8CurrentChannel++] = *pu16ADCBUF++;
            }
            
            if (MAIN_u8ADC1Trigger == MAIN_u8ADC1TriggerSequence[ADC1_SINGLE_SAMPLE_TRIGGERS_USED-1])
            {

                for (indexTrigger = 0; indexTrigger < ADC1_SINGLE_SAMPLE_TRIGGERS_COUNT; indexTrigger++)
                {
                    for (indexChannel = 0; indexChannel < ADC1_SINGLE_SAMPLE_COUNT; indexChannel++)
                    {
                        ADC1ResultSingleSampleCopy[indexTrigger][indexChannel] = ADC1ResultSingleSample[indexTrigger][indexChannel];
                    }
                }
                bExecControl = 1;
            }
            
            MAIN_u8ADC1TriggerIndex++;
            if (MAIN_u8ADC1TriggerIndex >= ADC1_SINGLE_SAMPLE_TRIGGERS_USED)
            {
                MAIN_u8ADC1TriggerIndex = 0;
            }
            MAIN_u8ADC1Trigger = MAIN_u8ADC1TriggerSequence[MAIN_u8ADC1TriggerIndex];
            switch (MAIN_u8ADC1Trigger)
            {
                case 0:
                    AD1CHS0bits.CH0SA = ADC1_PWM1_NO_SEQUENCE_CH0;   
                    AD1CHS123bits.CH123SA = 0;  
                    AD1CON1bits.SSRC = 0;
                    AD1CON1bits.SSRCG = 1;
                    break;
                    
                case 1:
                    AD1CHS0bits.CH0SA = ADC1_PWM2_NO_SEQUENCE_CH0;   
                    AD1CHS123bits.CH123SA = 0;  
                    AD1CON1bits.SSRC = 1;
                    AD1CON1bits.SSRCG = 1;
                    break;
                    
                case 2:
                    AD1CHS0bits.CH0SA = ADC1_PWM3_NO_SEQUENCE_CH0;   
                    AD1CHS123bits.CH123SA = 0;  
                    AD1CON1bits.SSRC = 2;
                    AD1CON1bits.SSRCG = 1;
                    break;
                    
                case 3:
                    AD1CHS0bits.CH0SA = ADC1_PWM4_NO_SEQUENCE_CH0;   
                    AD1CHS123bits.CH123SA = 0;  
                    AD1CON1bits.SSRC = 3;
                    AD1CON1bits.SSRCG = 1;
                    break;
                    
                case 4:
                    AD1CHS0bits.CH0SA = ADC1_PWM5_NO_SEQUENCE_CH0;   
                    AD1CHS123bits.CH123SA = 0;  
                    AD1CON1bits.SSRC = 4;
                    AD1CON1bits.SSRCG = 1;
                    break;
                    
                case 5:
                    AD1CHS0bits.CH0SA = ADC1_PWM6_NO_SEQUENCE_CH0;   
                    AD1CHS123bits.CH123SA = 0;  
                    AD1CON1bits.SSRC = 5;
                    AD1CON1bits.SSRCG = 1;
                    break;
                    
                case 6:
                    AD1CHS0bits.CH0SA = ADC1_PWM7_NO_SEQUENCE_CH0;   
                    AD1CHS123bits.CH123SA = 0;  
                    AD1CON1bits.SSRC = 6;
                    AD1CON1bits.SSRCG = 1;
                    break;
                    
                case 7:
                    AD1CHS0bits.CH0SA = ADC1_PSEV_NO_SEQUENCE_CH0;   
                    AD1CHS123bits.CH123SA = 0;  
                    AD1CON1bits.SSRC = 3;
                    AD1CON1bits.SSRCG = 0;
                    break;
                    
                case 8:
                    AD1CHS0bits.CH0SA = ADC1_SSEV_NO_SEQUENCE_CH0;   
                    AD1CHS123bits.CH123SA = 0;  
                    AD1CON1bits.SSRC = 5;
                    AD1CON1bits.SSRCG = 0;
                    break;
                default:
                    break;
            }
            

        }
        
    }
    else    
    {
        
        if(MAIN_bUseSequenceADC1State)  /* No Shift */
        {
            /* ADC NoShift ?????  */
            AD1CON2bits.CSCNA = 0;
            AD1CON1bits.ASAM = 0;   /* stop AutoSample */
            AD1CON1bits.SSRC = 0;//0- Clearing the Sample bit (SAMP) ends sampling and starts conversion (Manual mode)
            AD1CON1bits.SAMP = 1;
            
            if (MAIN_bADC1SyncPrimaryTriggersStatus)
            {
                MAIN_bADC1SyncPrimaryTriggersStatus = 0;
                

                pu16ADCBUF = (uint16_t*)&ADC1BUF0;
                u8CurrentChannel = 0;
                while(u8CurrentChannel < ADC1_SEQUENCE_COUNT)
                {
                    
                    ADC1ResultSequence[0][u8CurrentChannel] = *pu16ADCBUF++;
                    ADCValues[u8CurrentChannel] = ADC1ResultSequence[0][u8CurrentChannel];
                    u8CurrentChannel++;
                }
                for (indexTrigger = 0; indexTrigger < ADC1_SEQUENCE_TRIGGERS_COUNT; indexTrigger++)
                {
                    for (indexChannel = 0; indexChannel < ADC1_SEQUENCE_COUNT; indexChannel++)
                    {
                        ADC1ResultSequenceCopy[indexTrigger][indexChannel] = ADC1ResultSequence[indexTrigger][indexChannel];
                    }
                }
            }
        
            
        }
        else
        {
            
            pu16ADCBUF = (uint16_t*)&ADC1BUF0;
            u8CurrentChannel = 0;


            if(MAIN_bUseStack1ADC1State == 1)
            {


                while(u8CurrentChannel < ADC1_SINGLE_SAMPLE_COUNT)
                {
                    ADC1ResultSingleSample[MAIN_u8ADC1Trigger][u8CurrentChannel] = *pu16ADCBUF++;
                    ADCValues[u8CurrentChannel] = ADC1ResultSingleSample[MAIN_u8ADC1Trigger][u8CurrentChannel];
                    u8CurrentChannel++;           
                }

                for (indexTrigger = 0; indexTrigger < ADC1_SINGLE_SAMPLE_TRIGGERS_COUNT; indexTrigger++)
                {
                    for (indexChannel = 0; indexChannel < ADC1_SINGLE_SAMPLE_COUNT; indexChannel++)
                    {
                        ADC1ResultSingleSampleCopy[indexTrigger][indexChannel] = ADC1ResultSingleSample[indexTrigger][indexChannel];
                    }
                }
            }
            else
            {
                while(u8CurrentChannel < ADC1_SINGLE_SAMPLE_COUNT)
                {
                    ADC1ResultSingleSample[MAIN_u8ADC2Trigger][u8CurrentChannel] = *pu16ADCBUF++;
                    ADCValues[u8CurrentChannel+4] = ADC1ResultSingleSample[MAIN_u8ADC2Trigger][u8CurrentChannel];
                    u8CurrentChannel++;           
                }

                for (indexTrigger = 0; indexTrigger < ADC1_SINGLE_SAMPLE_TRIGGERS_COUNT; indexTrigger++)
                {
                    for (indexChannel = 0; indexChannel < ADC1_SINGLE_SAMPLE_COUNT; indexChannel++)
                    {
                        ADC1ResultSingleSampleCopy[indexTrigger][indexChannel] = ADC1ResultSingleSample[indexTrigger][indexChannel];
                    }
                }
            }

        }
        
        if (MAIN_bADC1MainFreemasterTrigger == 1)
        {
            if (MAIN_bADC2Trigger == 0)
            {
                bExecControl = 1;
            }
            else
            {
                MAIN_bADC2TriggerFreemaster = 1;
            }

        }
        else
        {
            if (MAIN_bADC1TriggerFreemaster == 1)
            {
                bExecControl = 1;
            }
        }
        MAIN_bADC1Trigger = 0;
    }
    
    if (bExecControl)
    {
        if (bExecControlNewData == 0)
        {
            vReadNewADCData();
            vReadNewADC2Data();
            bExecControlNewData = 1;
        }
        else
        {
            u16ExecControlNewDataSkip++;
        }
    }
    
	IFS0bits.AD1IF = 0; 
    
    //D0_LAT = 0;     
    //D1_LAT = 0;
    
    DEBUG_CLR_ADC_1();

    
}

void ADC2_CallBack(void)
{
    uint8_t bExecControl;
    //uint16_t u16SelectedChannels = 0;
    uint8_t u8CurrentChannel;
    uint16_t* pu16ADCBUF;
    uint8_t indexTrigger;
    uint8_t indexChannel;

    DEBUG_SET_ADC_2();      
    
    //D0_LAT = 1;   
    //D1_LAT = 1;
    u8CurrentChannel = 0;
    bExecControl = 0;

    if (MAIN_bUseADC2SyncPrimaryTriggers)
    {


        if(MAIN_bUseSequenceADC2State)  /* Shift S1S2 */
        {          
            
#if 1
            
            if (MAIN_bADC2SyncPrimaryTriggersStatus == 0)   /* skip not triggered result */
            {
                AD2CON1bits.ASAM = 0; 
                //AD2CON2bits.CSCNA = 1;  /* scan inputs from beginning */
                //AD2CON1bits.SSRC = 0;//0- Clearing the Sample bit (SAMP) ends sampling and starts conversion (Manual mode)
                //AD2CON1bits.SAMP = 1; 
                //MAIN_bADC2SyncPrimaryTriggersStatusWait = 1;
            }
            else
            {
                pu16ADCBUF = (uint16_t*)&ADC2BUF0;
                u8CurrentChannel = 0;
                while(u8CurrentChannel < ADC2_SEQUENCE_CHANNELS_USED)
                {
                    ADC2ResultSequence[MAIN_u8ADC2Trigger][u8CurrentChannel++] = *pu16ADCBUF++;
                }

                if (MAIN_bUsePWMShiftStack2)        /* Shift S1S2 */
                {
                    MAIN_u8ADC2TriggerIndex++;
                    if (MAIN_u8ADC2TriggerIndex >= ADC2_SEQUENCE_TRIGGERS_USED)
                    {
                        MAIN_u8ADC2TriggerIndex = 0;
                        //bExecControl = 1;
                        for (indexTrigger = 0; indexTrigger < ADC1_SEQUENCE_TRIGGERS_COUNT; indexTrigger++)
                        {
                            for (indexChannel = 0; indexChannel < ADC1_SEQUENCE_COUNT; indexChannel++)
                            {
                                ADC2ResultSequenceCopy[indexTrigger][indexChannel] = ADC2ResultSequence[indexTrigger][indexChannel];
                            }
                        }
                    }



                    MAIN_u8ADC2Trigger = MAIN_u8ADC2SequenceSequence[MAIN_u8ADC2TriggerIndex];
                    switch (MAIN_u8ADC2Trigger)
                    {
                        case TRIGGER_PSEV:
                            AD2CSSL = AD2CSSL_SSEV;
                            break;

                        case TRIGGER_SSEV:
                            AD2CSSL = AD2CSSL_PSEV;
                            break;

                        default:
                            break;
                    }
                }
                else            /* Shift L1L2L3 */
                {
                    MAIN_u8ADC2TriggerIndex++;
                    if (MAIN_u8ADC2TriggerIndex >= ADC2_SEQUENCE_TRIGGERS_USED_L1L2L3)
                    {
                        MAIN_u8ADC2TriggerIndex = 0;
                        //bExecControl = 1;
                    }



                    MAIN_u8ADC2Trigger = MAIN_u8ADC2SequenceShiftL1L2L3[MAIN_u8ADC2TriggerIndex];
                    switch (MAIN_u8ADC2Trigger)
                    {
                        case TRIGGER_PWM1:
                            AD2CSSL = ADC2_PWM1_CHANNELS_L1L2L3;
                            break;

                        case TRIGGER_PWM2:
                            AD2CSSL = ADC2_PWM2_CHANNELS_L1L2L3;
                            break;

                        case TRIGGER_PWM3:
                            AD2CSSL = ADC2_PWM3_CHANNELS_L1L2L3;
                            break;

                        default:
                            break;
                    }                    
                }
                
                
                
                
                
                
                
                MAIN_bADC2SyncPrimaryTriggersStatus = 0;
            }
            
            
#else
            if (MAIN_bADC2SyncPrimaryTriggersStatus)
            {
                pu16ADCBUF = (uint16_t*)&ADC2BUF0;
                u8CurrentChannel = 0;
                while(u8CurrentChannel < ADC2_MEASURES_COUNT)
                {
                    ADC2Result[MAIN_u8ADC2Trigger][u8CurrentChannel++] = *pu16ADCBUF++;
                }



                pu16ADCBUF = (uint16_t*)&ADC2BUF0;
                u16SelectedChannels = AD2CSSL;
                u8CurrentChannel = 0;
                while (u16SelectedChannels > 0)
                {
                    if (((u16SelectedChannels & 1) > 0) && (u8CurrentChannel < ADC_BUFFER_LENGTH))
                    {
                        ADCValues2[u8CurrentChannel] = *pu16ADCBUF++;
                    }
                    u8CurrentChannel++;
                    u16SelectedChannels >>= 1;
                }

                if (MAIN_u8ADC2Trigger == 5)
                {
                    if (MAIN_bADC1MainFreemasterTrigger == 0)
                    {
                        if (MAIN_bADC1Trigger == 0)
                        {
                            bExecControl = 1;
                        }
                        else
                        {
                            MAIN_bADC1TriggerFreemaster = 1;
                        }
                    }
                    else
                    {
                        if (MAIN_bADC2TriggerFreemaster == 1)
                        {
                            bExecControl = 1;
                        }
                    }
                    MAIN_bADC2Trigger = 0;
                }
            }
#endif
        }
        else /* MAIN_bUseADC2SyncPrimaryTriggers without MAIN_bUseSequenceADC2State */
        {
            pu16ADCBUF = (uint16_t*)&ADC2BUF0;
            u8CurrentChannel = 0;
            while(u8CurrentChannel < ADC2_SINGLE_SAMPLE_COUNT)
            {
                ADC2ResultSingleSample[MAIN_u8ADC2Trigger][u8CurrentChannel++] = *pu16ADCBUF++;
            }
            
            MAIN_u8ADC2TriggerIndex++;
            if (MAIN_u8ADC2TriggerIndex >= ADC2_SINGLE_SAMPLE_TRIGGERS_USED)
            {
                MAIN_u8ADC2TriggerIndex = 0;
                for (indexTrigger = 0; indexTrigger < ADC1_SINGLE_SAMPLE_TRIGGERS_COUNT; indexTrigger++)
                {
                    for (indexChannel = 0; indexChannel < ADC1_SINGLE_SAMPLE_COUNT; indexChannel++)
                    {
                        ADC2ResultSingleSampleCopy[indexTrigger][indexChannel] = ADC2ResultSingleSample[indexTrigger][indexChannel];
                    }
                }
            }
            MAIN_u8ADC2Trigger = MAIN_u8ADC2TriggerSequence[MAIN_u8ADC2TriggerIndex];
            switch (MAIN_u8ADC2Trigger)
            {
                case 0:
                    AD2CHS0bits.CH0SA = ADC2_PWM1_NO_SEQUENCE_CH0;   
                    AD2CHS123bits.CH123SA = 0;  
                    AD2CON1bits.SSRC = 0;
                    AD2CON1bits.SSRCG = 1;
                    break;
                    
                case 1:
                    AD2CHS0bits.CH0SA = ADC2_PWM2_NO_SEQUENCE_CH0;   
                    AD2CHS123bits.CH123SA = 0;  
                    AD2CON1bits.SSRC = 1;
                    AD2CON1bits.SSRCG = 1;
                    break;
                    
                case 2:
                    AD2CHS0bits.CH0SA = ADC2_PWM3_NO_SEQUENCE_CH0;   
                    AD2CHS123bits.CH123SA = 0;  
                    AD2CON1bits.SSRC = 2;
                    AD2CON1bits.SSRCG = 1;
                    break;
                    
                case 3:
                    AD2CHS0bits.CH0SA = ADC2_PWM4_NO_SEQUENCE_CH0;   
                    AD2CHS123bits.CH123SA = 0;  
                    AD2CON1bits.SSRC = 3;
                    AD2CON1bits.SSRCG = 1;
                    break;
                    
                case 4:
                    AD2CHS0bits.CH0SA = ADC2_PWM5_NO_SEQUENCE_CH0;   
                    AD2CHS123bits.CH123SA = 0;  
                    AD2CON1bits.SSRC = 4;
                    AD2CON1bits.SSRCG = 1;
                    break;
                    
                case 5:
                    AD2CHS0bits.CH0SA = ADC2_PWM6_NO_SEQUENCE_CH0;   
                    AD2CHS123bits.CH123SA = 0;  
                    AD2CON1bits.SSRC = 5;
                    AD2CON1bits.SSRCG = 1;
                    break;
                    
                case 6:
                    AD2CHS0bits.CH0SA = ADC2_PWM7_NO_SEQUENCE_CH0;   
                    AD2CHS123bits.CH123SA = 0;  
                    AD2CON1bits.SSRC = 6;
                    AD2CON1bits.SSRCG = 1;
                    break;
                    
                case 7:
                    AD2CHS0bits.CH0SA = ADC2_PSEV_NO_SEQUENCE_CH0;   
                    AD2CHS123bits.CH123SA = 0;  
                    AD2CON1bits.SSRC = 3;
                    AD2CON1bits.SSRCG = 0;
                    break;
                    
                case 8:
                    AD2CHS0bits.CH0SA = ADC2_SSEV_NO_SEQUENCE_CH0;   
                    AD2CHS123bits.CH123SA = 0;  
                    AD2CON1bits.SSRC = 5;
                    AD2CON1bits.SSRCG = 0;
                    break;
                default:
                    break;
            }
            

      
        }
    }
    else
    {
        if(MAIN_bUseSequenceADC2State)  /* No Shift */
        {
            
            /* ADC NoShift ?????  */
            AD2CON2bits.CSCNA = 0;
            AD2CON1bits.ASAM = 0;   /* stop AutoSample */
            AD2CON1bits.SSRC = 0;//0- Clearing the Sample bit (SAMP) ends sampling and starts conversion (Manual mode)
            AD2CON1bits.SAMP = 1;
            
            if (MAIN_bADC2SyncPrimaryTriggersStatus)
            {
                MAIN_bADC2SyncPrimaryTriggersStatus = 0;
                
                pu16ADCBUF = (uint16_t*)&ADC2BUF0;
                u8CurrentChannel = 0;
                while(u8CurrentChannel < ADC2_SEQUENCE_COUNT)
                {
                    
                    ADC2ResultSequence[0][u8CurrentChannel] = *pu16ADCBUF++;
                    ADCValues2[u8CurrentChannel] = ADC2ResultSequence[0][u8CurrentChannel];
                    u8CurrentChannel++;
                }

                for (indexTrigger = 0; indexTrigger < ADC1_SEQUENCE_TRIGGERS_COUNT; indexTrigger++)
                {
                    for (indexChannel = 0; indexChannel < ADC1_SEQUENCE_COUNT; indexChannel++)
                    {
                        ADC2ResultSequenceCopy[indexTrigger][indexChannel] = ADC2ResultSequence[indexTrigger][indexChannel];
                    }
                }
            }

        }
        else
        {
            pu16ADCBUF = (uint16_t*)&ADC2BUF0;
            u8CurrentChannel = 0;


            if(MAIN_bUseStack1ADC2State == 1)
            {


                while(u8CurrentChannel < ADC2_SINGLE_SAMPLE_COUNT)
                {
                    ADC2ResultSingleSample[MAIN_u8ADC2Trigger][u8CurrentChannel] = *pu16ADCBUF++;
                    ADCValues2[u8CurrentChannel] = ADC2ResultSingleSample[MAIN_u8ADC2Trigger][u8CurrentChannel];
                    u8CurrentChannel++;           
                }
                for (indexTrigger = 0; indexTrigger < ADC1_SINGLE_SAMPLE_TRIGGERS_COUNT; indexTrigger++)
                {
                    for (indexChannel = 0; indexChannel < ADC1_SINGLE_SAMPLE_COUNT; indexChannel++)
                    {
                        ADC2ResultSingleSampleCopy[indexTrigger][indexChannel] = ADC2ResultSingleSample[indexTrigger][indexChannel];
                    }
                }

            }
            else
            {
                while(u8CurrentChannel < ADC2_SINGLE_SAMPLE_COUNT)
                {
                    ADC2ResultSingleSample[MAIN_u8ADC2Trigger][u8CurrentChannel] = *pu16ADCBUF++;
                    ADCValues2[u8CurrentChannel+4] = ADC2ResultSingleSample[MAIN_u8ADC2Trigger][u8CurrentChannel];
                    u8CurrentChannel++;           
                }

                for (indexTrigger = 0; indexTrigger < ADC1_SINGLE_SAMPLE_TRIGGERS_COUNT; indexTrigger++)
                {
                    for (indexChannel = 0; indexChannel < ADC1_SINGLE_SAMPLE_COUNT; indexChannel++)
                    {
                        ADC2ResultSingleSampleCopy[indexTrigger][indexChannel] = ADC2ResultSingleSample[indexTrigger][indexChannel];
                    }
                }
            }
        }



        if (MAIN_bADC1MainFreemasterTrigger == 0)
        {
            if (MAIN_bADC1Trigger == 0)
            {
                bExecControl = 1;
            }
            else
            {
                MAIN_bADC1TriggerFreemaster = 1;
            }
        }
        else
        {
            if (MAIN_bADC2TriggerFreemaster == 1)
            {
                bExecControl = 1;
            }
        }
        MAIN_bADC2Trigger = 0;
    }
    
    if (bExecControl)
    {
        if (bExecControlNewData == 0)
        {
            vReadNewADCData();
            vReadNewADC2Data();
            bExecControlNewData = 1;
        }
        else
        {
            u16ExecControlNewDataSkip++;
        }
    }
    
    
    //D0_LAT = 0;     
    //D1_LAT = 0;

    DEBUG_CLR_ADC_2();  
   
   

    
}



#endif /* BOARDCFG_USE_OLD_NON_MCC_ADC */

#endif /* BOARDCFG_USE_MCC_ADC */














/* *****************************************************************************
 * _AD1Interrupt
 * 
 * Description: AD1 Interrupt. 
 * The ADC sample and conversion is triggered by the PWM period. 
 **************************************************************************** */
void __attribute__((interrupt, auto_psv)) _AD1Interrupt(void)
{
    ADC1_CallBack();
    
    // clear the ADC interrupt flag
    IFS0bits.AD1IF = false;
}

/* *****************************************************************************
 * _AD2Interrupt
 * 
 * Description: AD2 Interrupt. 
 * The ADC sample and conversion is triggered by the PWM period. 
 **************************************************************************** */
void __attribute__((interrupt, auto_psv)) _AD2Interrupt(void)
{
    ADC2_CallBack();
    
    // clear the ADC interrupt flag
    IFS1bits.AD2IF = false; 
}






#if (BOARDCFG_USE_OLD_NON_MCC_ADC == 0) || defined(BOARDCFG_USE_MCC_ADC)



void ADC1_CallBack(void)
{
    #if 0
    uint16_t* pu16ADCBUF;
    uint8_t indexTrigger;
    uint8_t indexChannel;
    uint8_t u8CurrentChannel;
    #endif
    
    DEBUG_SET_ADC_1();  
  
    /* No Shift */
    AD1CON2bits.CSCNA = 0;
    AD1CON1bits.ASAM = 0;   /* stop AutoSample */
    AD1CON1bits.SSRC = 0;   //0- Clearing the Sample bit (SAMP) ends sampling and starts conversion (Manual mode)
    AD1CON1bits.SAMP = 1;

    if (MAIN_bADC1SyncPrimaryTriggersStatus)
    {
        MAIN_bADC1SyncPrimaryTriggersStatus = 0;
    
        #if 1
        ADCStack1[ADC_STACK_CURRENT_L1] = ADC1_ConversionResultGet(ADC_BUF_POS_CURRENT_STACK1_PHASEA); 
        ADCStack1[ADC_STACK_CURRENT_L2] = ADC1_ConversionResultGet(ADC_BUF_POS_CURRENT_STACK1_PHASEB); 
        ADCStack1[ADC_STACK_CURRENT_L3] = ADC1_ConversionResultGet(ADC_BUF_POS_CURRENT_STACK1_PHASEC); 
        ADCStack1[ADC_STACK_DC_VOLTAGE] = ADC1_ConversionResultGet(ADC_BUF_POS_VOLTAGE_DCLINK);
        ADCStack2[ADC_STACK_CURRENT_L1] = ADC1_ConversionResultGet(ADC_BUF_POS_CURRENT_STACK2_PHASEA); 
        ADCStack2[ADC_STACK_CURRENT_L2] = ADC1_ConversionResultGet(ADC_BUF_POS_CURRENT_STACK2_PHASEB); 
        ADCStack2[ADC_STACK_CURRENT_L3] = ADC1_ConversionResultGet(ADC_BUF_POS_CURRENT_STACK2_PHASEC); 
        #ifdef ADC_BUF_POS_VOLTAGE_2 
        ADCStack2[ADC_STACK_DC_VOLTAGE] = ADC1_ConversionResultGet(ADC_BUF_POS_VOLTAGE_2); 
        #endif
        #else
        pu16ADCBUF = (uint16_t*)&ADC1BUF0;
        u8CurrentChannel = 0;
        while(u8CurrentChannel < ADC1_SEQUENCE_COUNT)
        {

            ADC1ResultSequence[0][u8CurrentChannel] = *pu16ADCBUF++;
            ADCValues[u8CurrentChannel] = ADC1ResultSequence[0][u8CurrentChannel];
            u8CurrentChannel++;
        }
        for (indexTrigger = 0; indexTrigger < ADC1_SEQUENCE_TRIGGERS_COUNT; indexTrigger++)
        {
            for (indexChannel = 0; indexChannel < ADC1_SEQUENCE_COUNT; indexChannel++)
            {
                ADC1ResultSequenceCopy[indexTrigger][indexChannel] = ADC1ResultSequence[indexTrigger][indexChannel];
            }
        }
        
        /* vReadNewADCData(); */
        ADCStack1[ADC_STACK_CURRENT_L1] = ADC1ResultSequenceCopy[0][0]; /* Trigger PSEV */
        ADCStack1[ADC_STACK_CURRENT_L2] = ADC1ResultSequenceCopy[0][1];
        ADCStack1[ADC_STACK_CURRENT_L3] = ADC1ResultSequenceCopy[0][2];
        ADCStack1[ADC_STACK_DC_VOLTAGE] = ADC1ResultSequenceCopy[0][6];
        ADCStack2[ADC_STACK_CURRENT_L1] = ADC1ResultSequenceCopy[0][3]; 
        ADCStack2[ADC_STACK_CURRENT_L2] = ADC1ResultSequenceCopy[0][4];
        ADCStack2[ADC_STACK_CURRENT_L3] = ADC1ResultSequenceCopy[0][5];
        ADCStack2[ADC_STACK_DC_VOLTAGE] = ADC1ResultSequenceCopy[0][7];
        #endif

        /* If Other ADC Already Ready */
        if (MAIN_bADC2SyncPrimaryTriggersStatus == 0)
        {
            ADCStack1Buffered[ADC_STACK_CURRENT_L1] = ADCStack1[ADC_STACK_CURRENT_L1];
            ADCStack1Buffered[ADC_STACK_CURRENT_L2] = ADCStack1[ADC_STACK_CURRENT_L2];
            ADCStack1Buffered[ADC_STACK_CURRENT_L3] = ADCStack1[ADC_STACK_CURRENT_L3];
            ADCStack1Buffered[ADC_STACK_DC_VOLTAGE] = ADCStack1[ADC_STACK_DC_VOLTAGE];
            ADCStack2Buffered[ADC_STACK_CURRENT_L1] = ADCStack2[ADC_STACK_CURRENT_L1];
            ADCStack2Buffered[ADC_STACK_CURRENT_L2] = ADCStack2[ADC_STACK_CURRENT_L2];
            ADCStack2Buffered[ADC_STACK_CURRENT_L3] = ADCStack2[ADC_STACK_CURRENT_L3];
            ADCStack2Buffered[ADC_STACK_DC_VOLTAGE] = ADCStack2[ADC_STACK_DC_VOLTAGE];

            ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L1] = ADC2Stack1[ADC2_STACK_CURRFBK_L1];
            ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L2] = ADC2Stack1[ADC2_STACK_CURRFBK_L2];
            ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L3] = ADC2Stack1[ADC2_STACK_CURRFBK_L3];
            ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L1] = ADC2Stack1[ADC2_STACK_TEMPFBK_L1];
            ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L2] = ADC2Stack1[ADC2_STACK_TEMPFBK_L2];
            ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L3] = ADC2Stack1[ADC2_STACK_TEMPFBK_L3];
            ADC2Stack1Buffered[ADC2_STACK_DCVOLTAGE2] = ADC2Stack1[ADC2_STACK_DCVOLTAGE2];
            
            ADC2Stack2Buffered[ADC2_STACK_CURRFBK_L1] = ADC2Stack2[ADC2_STACK_CURRFBK_L1];
            ADC2Stack2Buffered[ADC2_STACK_CURRFBK_L2] = ADC2Stack2[ADC2_STACK_CURRFBK_L2];
            ADC2Stack2Buffered[ADC2_STACK_CURRFBK_L3] = ADC2Stack2[ADC2_STACK_CURRFBK_L3];
            ADC2Stack2Buffered[ADC2_STACK_TEMPFBK_L1] = ADC2Stack2[ADC2_STACK_TEMPFBK_L1];
            ADC2Stack2Buffered[ADC2_STACK_TEMPFBK_L2] = ADC2Stack2[ADC2_STACK_TEMPFBK_L2];
            ADC2Stack2Buffered[ADC2_STACK_TEMPFBK_L3] = ADC2Stack2[ADC2_STACK_TEMPFBK_L3];
            ADC2Stack2Buffered[ADC2_STACK_DCVOLTAGE2] = ADC2Stack2[ADC2_STACK_DCVOLTAGE2];
            
            CONTROL_vProcess();
        }
    }
    DEBUG_CLR_ADC_1();
}

void ADC2_CallBack(void)
{
    #if 0
    uint16_t* pu16ADCBUF;
    uint8_t indexTrigger;
    uint8_t indexChannel;
    uint8_t u8CurrentChannel;
    #endif

    DEBUG_SET_ADC_2();      
    


            
    /* No Shift */
    AD2CON2bits.CSCNA = 0;
    AD2CON1bits.ASAM = 0;   /* stop AutoSample */
    AD2CON1bits.SSRC = 0;//0- Clearing the Sample bit (SAMP) ends sampling and starts conversion (Manual mode)
    AD2CON1bits.SAMP = 1;

    if (MAIN_bADC2SyncPrimaryTriggersStatus)
    {
        MAIN_bADC2SyncPrimaryTriggersStatus = 0;

        #if 1
        ADC2Stack1[ADC2_STACK_CURRFBK_L1] = ADC2_ConversionResultGet(ADC_BUF_POS_CURRENT_DRIVER_PHASEA); 
        ADC2Stack1[ADC2_STACK_CURRFBK_L2] = ADC2_ConversionResultGet(ADC_BUF_POS_CURRENT_DRIVER_PHASEB); 
        ADC2Stack1[ADC2_STACK_CURRFBK_L3] = ADC2_ConversionResultGet(ADC_BUF_POS_CURRENT_DRIVER_PHASEC); 
        
        #ifdef ADC_BUF_POS_VOLTAGE_3 
        ADC2Stack1[ADC2_STACK_DCVOLTAGE2] = ADC2_ConversionResultGet(ADC_BUF_POS_VOLTAGE_3);
        #endif

        ADC2Stack1[ADC2_STACK_TEMPFBK_L1] = ADC2_ConversionResultGet(ADC_BUF_POS_TEMPERATURE_DRIVER_PHASEA); 
        ADC2Stack1[ADC2_STACK_TEMPFBK_L2] = ADC2_ConversionResultGet(ADC_BUF_POS_TEMPERATURE_DRIVER_PHASEB); 
        ADC2Stack1[ADC2_STACK_TEMPFBK_L3] = ADC2_ConversionResultGet(ADC_BUF_POS_TEMPERATURE_DRIVER_PHASEC); 
        
        #ifdef ADC_BUF_POS_VOLTAGE_4 
        ADC2Stack2[ADC2_STACK_DCVOLTAGE2] = ADC2_ConversionResultGet(ADC_BUF_POS_VOLTAGE_4);
        #endif  
        
        #ifdef ADC_BUF_POS_CURR_STACK2_DRIVER_PHASEA
        ADC2Stack2[ADC2_STACK_CURRFBK_L1] = ADC2_ConversionResultGet(ADC_BUF_POS_CURR_STACK2_DRIVER_PHASEA); 
        ADC2Stack2[ADC2_STACK_CURRFBK_L2] = ADC2_ConversionResultGet(ADC_BUF_POS_CURR_STACK2_DRIVER_PHASEB); 
        ADC2Stack2[ADC2_STACK_CURRFBK_L3] = ADC2_ConversionResultGet(ADC_BUF_POS_CURR_STACK2_DRIVER_PHASEC); 
        #endif
        
        #ifdef ADC_BUF_POS_TEMP_STACK2_DRIVER_PHASEA
        ADC2Stack2[ADC2_STACK_TEMPFBK_L1] = ADC2_ConversionResultGet(ADC_BUF_POS_TEMP_STACK2_DRIVER_PHASEA); 
        ADC2Stack2[ADC2_STACK_TEMPFBK_L2] = ADC2_ConversionResultGet(ADC_BUF_POS_TEMP_STACK2_DRIVER_PHASEB); 
        ADC2Stack2[ADC2_STACK_TEMPFBK_L3] = ADC2_ConversionResultGet(ADC_BUF_POS_TEMP_STACK2_DRIVER_PHASEC); 
        #endif
        
        #else
        pu16ADCBUF = (uint16_t*)&ADC2BUF0;
        u8CurrentChannel = 0;
        while(u8CurrentChannel < ADC2_SEQUENCE_COUNT)
        {

            ADC2ResultSequence[0][u8CurrentChannel] = *pu16ADCBUF++;
            ADCValues2[u8CurrentChannel] = ADC2ResultSequence[0][u8CurrentChannel];
            u8CurrentChannel++;
        }

        for (indexTrigger = 0; indexTrigger < ADC1_SEQUENCE_TRIGGERS_COUNT; indexTrigger++)
        {
            for (indexChannel = 0; indexChannel < ADC1_SEQUENCE_COUNT; indexChannel++)
            {
                ADC2ResultSequenceCopy[indexTrigger][indexChannel] = ADC2ResultSequence[indexTrigger][indexChannel];
            }
        }
        
        /* vReadNewADC2Data(); */
        ADC2Stack1[ADC2_STACK_CURRFBK_L1] = ADC2ResultSequenceCopy[0][2]; /* Trigger PSEV */
        ADC2Stack1[ADC2_STACK_CURRFBK_L2] = ADC2ResultSequenceCopy[0][3];
        ADC2Stack1[ADC2_STACK_CURRFBK_L3] = ADC2ResultSequenceCopy[0][4];
        ADC2Stack1[ADC2_STACK_DCVOLTAGE2] = ADC2ResultSequenceCopy[0][0];
        ADC2Stack1[ADC2_STACK_TEMPFBK_L1] = ADC2ResultSequenceCopy[0][5]; 
        ADC2Stack1[ADC2_STACK_TEMPFBK_L2] = ADC2ResultSequenceCopy[0][6];
        ADC2Stack1[ADC2_STACK_TEMPFBK_L3] = ADC2ResultSequenceCopy[0][7];
        ADC2Stack2[ADC2_STACK_DCVOLTAGE2] = ADC2ResultSequenceCopy[0][1];
        #endif

        /* If Other ADC Already Ready */
        if (MAIN_bADC1SyncPrimaryTriggersStatus == 0)
        {
            ADCStack1Buffered[ADC_STACK_CURRENT_L1] = ADCStack1[ADC_STACK_CURRENT_L1];
            ADCStack1Buffered[ADC_STACK_CURRENT_L2] = ADCStack1[ADC_STACK_CURRENT_L2];
            ADCStack1Buffered[ADC_STACK_CURRENT_L3] = ADCStack1[ADC_STACK_CURRENT_L3];
            ADCStack1Buffered[ADC_STACK_DC_VOLTAGE] = ADCStack1[ADC_STACK_DC_VOLTAGE];
            ADCStack2Buffered[ADC_STACK_CURRENT_L1] = ADCStack2[ADC_STACK_CURRENT_L1];
            ADCStack2Buffered[ADC_STACK_CURRENT_L2] = ADCStack2[ADC_STACK_CURRENT_L2];
            ADCStack2Buffered[ADC_STACK_CURRENT_L3] = ADCStack2[ADC_STACK_CURRENT_L3];
            ADCStack2Buffered[ADC_STACK_DC_VOLTAGE] = ADCStack2[ADC_STACK_DC_VOLTAGE];

            ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L1] = ADC2Stack1[ADC2_STACK_CURRFBK_L1];
            ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L2] = ADC2Stack1[ADC2_STACK_CURRFBK_L2];
            ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L3] = ADC2Stack1[ADC2_STACK_CURRFBK_L3];
            ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L1] = ADC2Stack1[ADC2_STACK_TEMPFBK_L1];
            ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L2] = ADC2Stack1[ADC2_STACK_TEMPFBK_L2];
            ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L3] = ADC2Stack1[ADC2_STACK_TEMPFBK_L3];
            ADC2Stack1Buffered[ADC2_STACK_DCVOLTAGE2] = ADC2Stack1[ADC2_STACK_DCVOLTAGE2];
            
            ADC2Stack2Buffered[ADC2_STACK_CURRFBK_L1] = ADC2Stack2[ADC2_STACK_CURRFBK_L1];
            ADC2Stack2Buffered[ADC2_STACK_CURRFBK_L2] = ADC2Stack2[ADC2_STACK_CURRFBK_L2];
            ADC2Stack2Buffered[ADC2_STACK_CURRFBK_L3] = ADC2Stack2[ADC2_STACK_CURRFBK_L3];
            ADC2Stack2Buffered[ADC2_STACK_TEMPFBK_L1] = ADC2Stack2[ADC2_STACK_TEMPFBK_L1];
            ADC2Stack2Buffered[ADC2_STACK_TEMPFBK_L2] = ADC2Stack2[ADC2_STACK_TEMPFBK_L2];
            ADC2Stack2Buffered[ADC2_STACK_TEMPFBK_L3] = ADC2Stack2[ADC2_STACK_TEMPFBK_L3];
            ADC2Stack2Buffered[ADC2_STACK_DCVOLTAGE2] = ADC2Stack2[ADC2_STACK_DCVOLTAGE2];
            
            CONTROL_vProcess();            
        }       
    }
    DEBUG_CLR_ADC_2();      
}

#if 0
/* *****************************************************************************
 * ADC_RegisterFunctionsInISR 
 **************************************************************************** */
void ADC_RegisterFunctionsInISR(void)
{
    ADC1_SetInterruptHandler(ADC1_CallBack);
    ADC2_SetInterruptHandler(ADC2_CallBack);   
}
#endif

#endif


/* *****************************************************************************
 * PWM Related Functions - PWM Module
 **************************************************************************** */

#if BOARDCFG_USE_OLD_NON_MCC_ADC
#ifndef BOARDCFG_USE_MCC_ADC

void PWM_vStop(void)
{
    PTCONbits.PTEN = 0; 
    
#if 1       /* new - only clear flag */   
    // PWM Special Event Match Interrupt
    IFS3bits.PSEMIF = 0; 
    // PWM Secondary Special Event Match Interrupt
    IFS4bits.PSESMIF = 0; 
    // PWM1 Interrupt
	IFS5bits.PWM1IF = 0;	//clear PWM interrupt flag
    // PWM2 Interrupt
	IFS5bits.PWM2IF = 0;	//clear PWM interrupt flag
    // PWM3 Interrupt
	IFS6bits.PWM3IF = 0;	//clear PWM interrupt flag
    // PWM4 Interrupt
	IFS6bits.PWM4IF = 0;	//clear PWM interrupt flag
    // PWM5 Interrupt
	IFS6bits.PWM5IF = 0;	//clear PWM interrupt flag
    // PWM6 Interrupt
	IFS6bits.PWM6IF = 0;	//clear PWM interrupt flag
    // PWM7 Interrupt
	IFS6bits.PWM7IF = 0;	//clear PWM interrupt flag
#else
    // PWM Special Event Match Interrupt
    IFS3bits.PSEMIF = 0; 
    IEC3bits.PSEMIE = 0; 

    // PWM Secondary Special Event Match Interrupt
    IFS4bits.PSESMIF = 0; 
    IEC4bits.PSESMIE = 0; 
    

    // PWM1 Interrupt
	IFS5bits.PWM1IF = 0;	//clear PWM interrupt flag
	IEC5bits.PWM1IE = 0;	// Enable PWM interrupts not working
    
    
    // PWM2 Interrupt
	IFS5bits.PWM2IF = 0;	//clear PWM interrupt flag
	IEC5bits.PWM2IE = 0;	// Enable PWM interrupts not working

    // PWM3 Interrupt
	IFS6bits.PWM3IF = 0;	//clear PWM interrupt flag
	IEC6bits.PWM3IE = 0;	// Enable PWM interrupts not working
    
    // PWM4 Interrupt
	IFS6bits.PWM4IF = 0;	//clear PWM interrupt flag
	IEC6bits.PWM4IE = 0;	// Enable PWM interrupts not working

    // PWM5 Interrupt
	IFS6bits.PWM5IF = 0;	//clear PWM interrupt flag
	IEC6bits.PWM5IE = 0;	// Enable PWM interrupts not working
    
    // PWM6 Interrupt
	IFS6bits.PWM6IF = 0;	//clear PWM interrupt flag
	IEC6bits.PWM6IE = 0;	// Enable PWM interrupts not working  
    
    // PWM7 Interrupt
	IFS6bits.PWM7IF = 0;	//clear PWM interrupt flag
	IEC6bits.PWM7IE = 0;	// Enable PWM interrupts not working  
#endif
    
    
    
}

void PWM_vStart(void)
{
    PTCONbits.PTEN = 1;
}

#endif

#endif /* BOARDCFG_USE_OLD_NON_MCC_ADC */

void PWM_vOutputEnable(void)
{
    PDC1 = PDC2 = PDC3 = PDC4 = PDC5 = PDC6 = PWM_u16StartDutyTcy;
    
}

void PWM_vOutputDisable(void)
{
    /* zero duty cycle */ 
    PDC1 = 0;
    PDC2 = 0;
    PDC3 = 0;

    /* zero duty cycle */ 
    PDC4 = 0;
    PDC5 = 0;
    PDC6 = 0;
}

#if BOARDCFG_USE_OLD_NON_MCC_PWM
#ifndef BOARDCFG_USE_MCC_PWM
void PWM_vReset(void)
{
    PWM_vOutputDisable();


    /* Dead Time From MCU */
    DeadTimeMCUStack1RAW[0] = MCLIB_s16Mul_s16u32(DeadTimeMCUStack1[0], sControlConfig.u32DeadTimeMCUNanoSecondsToTicksMultiplier);
    DeadTimeMCUStack1RAW[1] = MCLIB_s16Mul_s16u32(DeadTimeMCUStack1[1], sControlConfig.u32DeadTimeMCUNanoSecondsToTicksMultiplier);
    DeadTimeMCUStack1RAW[2] = MCLIB_s16Mul_s16u32(DeadTimeMCUStack1[2], sControlConfig.u32DeadTimeMCUNanoSecondsToTicksMultiplier);
    
    DeadTimeMCUStack2RAW[0] = MCLIB_s16Mul_s16u32(DeadTimeMCUStack2[0], sControlConfig.u32DeadTimeMCUNanoSecondsToTicksMultiplier);
    DeadTimeMCUStack2RAW[1] = MCLIB_s16Mul_s16u32(DeadTimeMCUStack2[1], sControlConfig.u32DeadTimeMCUNanoSecondsToTicksMultiplier);
    DeadTimeMCUStack2RAW[2] = MCLIB_s16Mul_s16u32(DeadTimeMCUStack2[2], sControlConfig.u32DeadTimeMCUNanoSecondsToTicksMultiplier);

    
	ALTDTR1 = DeadTimeMCUStack1RAW[0];    
	ALTDTR2 = DeadTimeMCUStack1RAW[1];    
	ALTDTR3 = DeadTimeMCUStack1RAW[2]; 
    
	ALTDTR4 = DeadTimeMCUStack2RAW[0];    
	ALTDTR5 = DeadTimeMCUStack2RAW[1];    
	ALTDTR6 = DeadTimeMCUStack2RAW[2];    

    
    
    
    
    
    if (MAIN_bUsePWMShiftL1L2L3)
    {
        
        if(MAIN_bUsePWMShiftStack2)
        {
            SPHASE1 = MAIN_u16PWMTriggersSingleSample[TRIGGER_PWM1];   
            SPHASE2 = MAIN_u16PWMTriggersSingleSample[TRIGGER_PWM2];   
            SPHASE3 = MAIN_u16PWMTriggersSingleSample[TRIGGER_PWM3];   

            SPHASE4 = MAIN_u16PWMTriggersSingleSample[TRIGGER_PWM4];   
            SPHASE5 = MAIN_u16PWMTriggersSingleSample[TRIGGER_PWM5];   
            SPHASE6 = MAIN_u16PWMTriggersSingleSample[TRIGGER_PWM6]; 

            SEVTCMP = u16LoopTime1Tcy + (MAIN_u16PWMTriggersSingleSample[TRIGGER_PSEV]*2);
            if (SEVTCMP >= (u16LoopTime1Tcy*2))
            {
                SEVTCMP -= (u16LoopTime1Tcy*2);
            }

            SSEVTCMP = u16LoopTime2Tcy + (MAIN_u16PWMTriggersSingleSample[TRIGGER_SSEV]*2);
            if (SSEVTCMP >= (u16LoopTime2Tcy*2))
            {
                SSEVTCMP -= (u16LoopTime2Tcy*2);
            }

            SPHASE7 = MAIN_u16PWMTriggersSingleSample[TRIGGER_PWM7]; 
        }
        else
        {
            SPHASE1 = MAIN_u16PWMTriggersSingleSampleBothStacks[TRIGGER_PWM1];   
            SPHASE2 = MAIN_u16PWMTriggersSingleSampleBothStacks[TRIGGER_PWM2];   
            SPHASE3 = MAIN_u16PWMTriggersSingleSampleBothStacks[TRIGGER_PWM3];   

            SPHASE4 = MAIN_u16PWMTriggersSingleSampleBothStacks[TRIGGER_PWM4];   
            SPHASE5 = MAIN_u16PWMTriggersSingleSampleBothStacks[TRIGGER_PWM5];   
            SPHASE6 = MAIN_u16PWMTriggersSingleSampleBothStacks[TRIGGER_PWM6]; 

            SEVTCMP = u16LoopTime1Tcy + (MAIN_u16PWMTriggersSingleSampleBothStacks[TRIGGER_PSEV]*2);
            if (SEVTCMP >= (u16LoopTime1Tcy*2))
            {
                SEVTCMP -= (u16LoopTime1Tcy*2);
            }

            SSEVTCMP = u16LoopTime2Tcy + (MAIN_u16PWMTriggersSingleSampleBothStacks[TRIGGER_SSEV]*2);
            if (SSEVTCMP >= (u16LoopTime2Tcy*2))
            {
                SSEVTCMP -= (u16LoopTime2Tcy*2);
            }

            SPHASE7 = MAIN_u16PWMTriggersSingleSampleBothStacks[TRIGGER_PWM7]; 
        }
    }
    else
    {
        if(MAIN_bUsePWMShiftStack2)
        {
            SPHASE1 = MAIN_u16PWMTriggersSequence[TRIGGER_PWM1];   
            SPHASE2 = MAIN_u16PWMTriggersSequence[TRIGGER_PWM2];   
            SPHASE3 = MAIN_u16PWMTriggersSequence[TRIGGER_PWM3];   

            SPHASE4 = MAIN_u16PWMTriggersSequence[TRIGGER_PWM4];   
            SPHASE5 = MAIN_u16PWMTriggersSequence[TRIGGER_PWM5];   
            SPHASE6 = MAIN_u16PWMTriggersSequence[TRIGGER_PWM6]; 

            SEVTCMP = u16LoopTime1Tcy + (MAIN_u16PWMTriggersSequence[TRIGGER_PSEV]*2);
            if (SEVTCMP >= (u16LoopTime1Tcy*2))
            {
                SEVTCMP -= (u16LoopTime1Tcy*2);
            }

            SSEVTCMP = u16LoopTime2Tcy + (MAIN_u16PWMTriggersSequence[TRIGGER_SSEV]*2);
            if (SSEVTCMP >= (u16LoopTime2Tcy*2))
            {
                SSEVTCMP -= (u16LoopTime2Tcy*2);
            }

            SPHASE7 = MAIN_u16PWMTriggersSequence[TRIGGER_PWM7]; 
        }
        else
        {
            SPHASE1 = MAIN_u16PWMTriggersSequenceBothStacks[TRIGGER_PWM1];   
            SPHASE2 = MAIN_u16PWMTriggersSequenceBothStacks[TRIGGER_PWM2];   
            SPHASE3 = MAIN_u16PWMTriggersSequenceBothStacks[TRIGGER_PWM3];   

            SPHASE4 = MAIN_u16PWMTriggersSequenceBothStacks[TRIGGER_PWM4];   
            SPHASE5 = MAIN_u16PWMTriggersSequenceBothStacks[TRIGGER_PWM5];   
            SPHASE6 = MAIN_u16PWMTriggersSequenceBothStacks[TRIGGER_PWM6];  

            SEVTCMP = u16LoopTime1Tcy + (MAIN_u16PWMTriggersSequenceBothStacks[TRIGGER_PSEV]*2);
            if (SEVTCMP >= (u16LoopTime1Tcy*2))
            {
                SEVTCMP -= (u16LoopTime1Tcy*2);
            }

            SSEVTCMP = u16LoopTime2Tcy + (MAIN_u16PWMTriggersSequenceBothStacks[TRIGGER_SSEV]*2);
            if (SSEVTCMP >= (u16LoopTime2Tcy*2))
            {
                SSEVTCMP -= (u16LoopTime2Tcy*2);
            }

            SPHASE7 = MAIN_u16PWMTriggersSequenceBothStacks[TRIGGER_PWM7]; 
        }
    }

    
#if 0
    
    // SEVTCMP: Special Event Compare Count Register 
    // Phase of ADC capture set relative to PWM cycle: 0 offset and counting up
	SEVTCMP = u16LoopTime1Tcy + (PWM_u16PWMGlobalTcy[TRIGGER_PSEV]*2);   /* ??? center aligned */
    
    if (SEVTCMP >= (u16LoopTime1Tcy*2))
    {
        SEVTCMP -= (u16LoopTime1Tcy*2);
    }
    
    SSEVTCMP = u16LoopTime2Tcy + (PWM_u16PWMGlobalTcy[TRIGGER_SSEV]*2);    /* ??? edge aligned */
    
    if (SSEVTCMP >= (u16LoopTime2Tcy*2))
    {
        SSEVTCMP -= (u16LoopTime2Tcy*2);
    }
#endif
    
    
    
    if (MAIN_bUseSPHASEForPhaseOffset)
    {
        MAIN_bPhase1OffsetEnd = 0;
        MAIN_bPhase2OffsetEnd = 0;
        MAIN_bPhase3OffsetEnd = 0;
        MAIN_bPhase4OffsetEnd = 0;
        MAIN_bPhase5OffsetEnd = 0;
        MAIN_bPhase6OffsetEnd = 0;
        
        MAIN_bPhase1OffsetRequested = 0;
        MAIN_bPhase2OffsetRequested = 0;
        MAIN_bPhase3OffsetRequested = 0;
        MAIN_bPhase4OffsetRequested = 0;
        MAIN_bPhase5OffsetRequested = 0;
        MAIN_bPhase6OffsetRequested = 0;
        
        MAIN_bPhase1OffsetCompleted = 0;
        MAIN_bPhase2OffsetCompleted = 0;
        MAIN_bPhase3OffsetCompleted = 0;
        MAIN_bPhase4OffsetCompleted = 0;
        MAIN_bPhase5OffsetCompleted = 0;
        MAIN_bPhase6OffsetCompleted = 0;
        
        MAIN_u16Phase1OffsetCounter = 0;
        MAIN_u16Phase2OffsetCounter = 0;
        MAIN_u16Phase3OffsetCounter = 0;
        MAIN_u16Phase4OffsetCounter = 0;
        MAIN_u16Phase5OffsetCounter = 0;
        MAIN_u16Phase6OffsetCounter = 0;
        
        switch(eModeOffsetSetup)
        {
            case MODE_OFFSET_IMMEDIATE:
                PHASE1 = SPHASE1;   
                PHASE2 = SPHASE2;   
                PHASE3 = SPHASE3;   
                PHASE4 = SPHASE4;   
                PHASE5 = SPHASE5;   
                PHASE6 = SPHASE6;
#if 1
                MAIN_bPhase1OffsetEnd = 1;
                MAIN_bPhase2OffsetEnd = 1;
                MAIN_bPhase3OffsetEnd = 1;
                MAIN_bPhase4OffsetEnd = 1;
                MAIN_bPhase5OffsetEnd = 1;
                MAIN_bPhase6OffsetEnd = 1;
#else
                MAIN_u16Phase1OffsetCounter = 1;
                MAIN_u16Phase2OffsetCounter = 1;
                MAIN_u16Phase3OffsetCounter = 1;
                MAIN_u16Phase4OffsetCounter = 1;
                MAIN_u16Phase5OffsetCounter = 1;
                MAIN_u16Phase6OffsetCounter = 1;
#endif
                break;
            case MODE_OFFSET_IMMEDIATE_AFTER_ONE_PERIOD:
                MAIN_bPhase1OffsetRequested = 1;
                MAIN_bPhase2OffsetRequested = 1;
                MAIN_bPhase3OffsetRequested = 1;
                MAIN_bPhase4OffsetRequested = 1;
                MAIN_bPhase5OffsetRequested = 1;
                MAIN_bPhase6OffsetRequested = 1; 
                break;
            case MODE_OFFSET_SMOOTH:
                if (SPHASE1 > 0)
                {                
                    PHASE1 += 1;   
                    MAIN_u16Phase1OffsetCounter = SPHASE1;
                }
                if (SPHASE2 > 0)
                {
                    PHASE2 += 1;   
                    MAIN_u16Phase2OffsetCounter = SPHASE2;
                }
                if (SPHASE3 > 0)
                {
                    PHASE3 += 1;   
                    MAIN_u16Phase3OffsetCounter = SPHASE3;
                }
                if (SPHASE4 > 0)
                {
                    PHASE4 += 1;   
                    MAIN_u16Phase4OffsetCounter = SPHASE4;
                }
                if (SPHASE5 > 0)
                {
                    PHASE5 += 1;   
                    MAIN_u16Phase5OffsetCounter = SPHASE5;
                }
                if (SPHASE6 > 0)
                {
                    PHASE6 += 1;
                    MAIN_u16Phase6OffsetCounter = SPHASE6;
                }

                break;
            case MODE_OFFSET_SMOOTH_AFTER_ONE_PERIOD:
                if (SPHASE1 > 0)
                {                
                    MAIN_bPhase1OffsetRequestSmooth = 1;
                }
                if (SPHASE2 > 0)
                {
                    MAIN_bPhase2OffsetRequestSmooth = 1;
                }
                if (SPHASE3 > 0)
                {
                    MAIN_bPhase3OffsetRequestSmooth = 1;
                }
                if (SPHASE4 > 0)
                {
                    MAIN_bPhase4OffsetRequestSmooth = 1;
                }
                if (SPHASE5 > 0)
                {
                    MAIN_bPhase5OffsetRequestSmooth = 1;
                }
                if (SPHASE6 > 0)
                {
                    MAIN_bPhase6OffsetRequestSmooth = 1; 
                }
                break;
                
            default:
                break;
        }
        
        PHASE7 = SPHASE7 * 2; /* PWM7 is edge aligned */
    }  
}

void PWM_vInterruptsSetup(void)
{


    _PWM1IP = ISR_PRIORITY_PWM1;
    _PWM2IP = ISR_PRIORITY_PWM2;
    _PWM3IP = ISR_PRIORITY_PWM3;
    _PWM4IP = ISR_PRIORITY_PWM4;
    _PWM5IP = ISR_PRIORITY_PWM5;
    _PWM6IP = ISR_PRIORITY_PWM6;

    _PWM7IP = ISR_PRIORITY_PWM7;
    
    _PSEMIP = ISR_PRIORITY_PWMSEVTIP;
    _PSESMIP = ISR_PRIORITY_PWMSSEVTIP;
    
    // PWM Special Event Match Interrupt
    IFS3bits.PSEMIF = 0; 
    IEC3bits.PSEMIE = 1; 

    // PWM Secondary Special Event Match Interrupt
    IFS4bits.PSESMIF = 0; 
    IEC4bits.PSESMIE = 1; 

    // PWM1 Interrupt
	IFS5bits.PWM1IF = 0;	//clear PWM interrupt flag
	IEC5bits.PWM1IE = 1;	// Enable PWM interrupts not working
    
    
    // PWM2 Interrupt
	IFS5bits.PWM2IF = 0;	//clear PWM interrupt flag
	IEC5bits.PWM2IE = 1;	// Enable PWM interrupts not working

    // PWM3 Interrupt
	IFS6bits.PWM3IF = 0;	//clear PWM interrupt flag
	IEC6bits.PWM3IE = 1;	// Enable PWM interrupts not working
    
    // PWM4 Interrupt
	IFS6bits.PWM4IF = 0;	//clear PWM interrupt flag
	IEC6bits.PWM4IE = 1;	// Enable PWM interrupts not working

    // PWM5 Interrupt
	IFS6bits.PWM5IF = 0;	//clear PWM interrupt flag
	IEC6bits.PWM5IE = 1;	// Enable PWM interrupts not working
    
    // PWM6 Interrupt
	IFS6bits.PWM6IF = 0;	//clear PWM interrupt flag
	IEC6bits.PWM6IE = 1;	// Enable PWM interrupts not working  
    
    // PWM7 Interrupt
	IFS6bits.PWM7IF = 0;	//clear PWM interrupt flag
	IEC6bits.PWM7IE = 1;	// Enable PWM interrupts not working  
}

void PWM_vInit(void)
{
    PTCON = 0;
    STCON = 0;
    
    
    
	PTPER = 2*u16LoopTime1Tcy+1;		//one trigger per PWM period
    
    
    // ============= PWM STACK1 ======================

    // Center aligned PWM.
    // Note: The PWM period is set to dLoopInTcy/2 but since it counts up and 
    // and then down => the interrupt flag is set to 1 at zero => actual 
    // interrupt period is dLoopInTcy

	PHASE1 = u16LoopTime1Tcy;
	PHASE2 = u16LoopTime1Tcy;
	PHASE3 = u16LoopTime1Tcy;

	PWMCON1 = 0x0204;	// Enable PWM output pins and configure them as 
	PWMCON2 = 0x0204;	// complementary mode
	PWMCON3 = 0x0204;

	//I/O pins controlled by PWM
	IOCON1 = 0xC000;
	IOCON2 = 0xC000;
	IOCON3 = 0xC000;

	DTR1 = 0x0000;
	DTR2 = 0x0000;
	DTR3 = 0x0000;

	ALTDTR1 = 0;    //0x0063;	// more than 700ns of dead time
	ALTDTR2 = 0;    //0x0063;	// more than 700ns of dead time
	ALTDTR3 = 0;    //0x0063;	// more than 700ns of dead time

	//fault enabled	
	//FCLCON1 = 0x0005;
	//FCLCON2 = 0x0005;
	//FCLCON3 = 0x0005;
	//fault disabled	
	FCLCON1 = 0x0003;
	FCLCON2 = 0x0003;
	FCLCON3 = 0x0003;
	

    PDC1 = 0;
    PDC2 = 0;
    PDC3 = 0;
    
    
    // ============= PWM STACK2 ======================

    // Center aligned PWM.
    // Note: The PWM period is set to dLoopInTcy/2 but since it counts up and 
    // and then down => the interrupt flag is set to 1 at zero => actual 
    // interrupt period is dLoopInTcy

	PHASE4 = u16LoopTime2Tcy;
	PHASE5 = u16LoopTime2Tcy;
	PHASE6 = u16LoopTime2Tcy;

	PWMCON4 = 0x0204;	// Enable PWM output pins and configure them as 
	PWMCON5 = 0x0204;	// complementary mode
	PWMCON6 = 0x0204;

	//I/O pins controlled by PWM
	IOCON4 = 0xC000;
	IOCON5 = 0xC000;
	IOCON6 = 0xC000;

	DTR4 = 0x0000;
	DTR5 = 0x0000;
	DTR6 = 0x0000;

	ALTDTR4 = 0;    //0x0063;	// more than 700ns of dead time
	ALTDTR5 = 0;    //0x0063;	// more than 700ns of dead time
	ALTDTR6 = 0;    //0x0063;	// more than 700ns of dead time

	//fault enabled	
	//FCLCON4 = 0x0005;
	//FCLCON5 = 0x0005;
	//FCLCON6 = 0x0005;
	//fault disabled	
	FCLCON4 = 0x0003;
	FCLCON5 = 0x0003;
	FCLCON6 = 0x0003;
	

    PDC4 = 0;
    PDC5 = 0;
    PDC6 = 0;
    
    
	PTCON2 = 0x0000;	// Divide by 1 to generate PWM
    
    PTCON = 0x0000;         
    
    STCON = 0x0000; 
    
    
    STCON2 = 0x0000;
    
    
    STPER = 2*u16LoopTime2Tcy+1;		//one trigger per PWM period
    
    
    PTCONbits.EIPU = 0; /* Active Secondary Period register updates occur on PWM cycle boundaries */
    STCONbits.EIPU = 0; /* Active Secondary Period register updates occur on PWM cycle boundaries */
    
    

    // SEVTCMP: Special Event Compare Count Register 
    // Phase of ADC capture set relative to PWM cycle: 0 offset and counting up
	SEVTCMP = u16LoopTime1Tcy + (PWM_u16PWMGlobalTcy[TRIGGER_PSEV]*2);   /* ??? center aligned */
    
    if (SEVTCMP >= (u16LoopTime1Tcy*2))
    {
        SEVTCMP -= (u16LoopTime1Tcy*2);
}

    SSEVTCMP = u16LoopTime2Tcy + (PWM_u16PWMGlobalTcy[TRIGGER_SSEV]*2);    /* ??? edge aligned */
    
    if (SSEVTCMP >= (u16LoopTime2Tcy*2))
    {
        SSEVTCMP -= (u16LoopTime2Tcy*2);
    }
    
    PTCONbits.SEIEN = 1;        /* PWM primary Special Event Trigger interrupt Enable */
    STCONbits.SEIEN = 1;        /* PWM secondary Special Event Trigger interrupt Enable */
    
    PWMCON1bits.TRGIEN = 1;         /* PWM1 Interrupt Enable */
    PWMCON2bits.TRGIEN = 1;         /* PWM2 Interrupt Enable */
    PWMCON3bits.TRGIEN = 1;         /* PWM3 Interrupt Enable */
    PWMCON4bits.TRGIEN = 1;         /* PWM4 Interrupt Enable */
    PWMCON5bits.TRGIEN = 1;         /* PWM5 Interrupt Enable */
    PWMCON6bits.TRGIEN = 1;         /* PWM6 Interrupt Enable */
    PWMCON7bits.TRGIEN = 1;         /* PWM7 Interrupt Enable */
    
    /* SPHASE Will be used for Phase Shift Value For Evaluation and Debug */
    SPHASE1 = PWM_u16PWMGlobalTcy[TRIGGER_PWM1];   
    SPHASE2 = PWM_u16PWMGlobalTcy[TRIGGER_PWM2];   
    SPHASE3 = PWM_u16PWMGlobalTcy[TRIGGER_PWM3];   
    
    SPHASE4 = PWM_u16PWMGlobalTcy[TRIGGER_PWM4];   
    SPHASE5 = PWM_u16PWMGlobalTcy[TRIGGER_PWM5];   
    SPHASE6 = PWM_u16PWMGlobalTcy[TRIGGER_PWM6];  
    
    SPHASE7 = PWM_u16PWMGlobalTcy[TRIGGER_PWM7];   
    
    //PTCONbits.PTEN = 1; enable with reset PWM Function below
    
    //----------------------------------		
	//	Unlock the Re-programmable Pin Mechanism
	//__builtin_write_OSCCONL(OSCCON & (~(1<<6))); // clear bit 6 
    
	// Select RP88 as input for fault 1
	//RPINR12 = 0x0058;       ????pwm not nork
    
	//	Lock the Re-programmable Pin Mechanism
	//__builtin_write_OSCCONL(OSCCON | (1<<6)); 	 // Set bit 6

}
#endif

#endif /* BOARDCFG_USE_OLD_NON_MCC_PWM */


#if (BOARDCFG_USE_OLD_NON_MCC_PWM == 0) && !defined(BOARDCFG_USE_MCC_PWM)



typedef enum
{
    PWM_1,
    PWM_2,
    PWM_3,
    PWM_4,
    PWM_5,
    PWM_6,
    PWM_COUNT
}PWM_eModule_t;

typedef struct
{
    union 
    {
        struct 
        {
            uint16_t PTCON;
            uint16_t PTCON2;
            uint16_t PTPER;
            uint16_t SEVTCMP;
            uint16_t :16;
            uint16_t MDC;
            uint16_t :16;
            uint16_t STCON;
            uint16_t STCON2;
            uint16_t STPER;
            uint16_t SSEVTCMP;
            uint16_t :16;
            uint16_t :16;
            uint16_t CHOP;
            uint16_t :16;
            uint16_t PWMKEY;
        };
        
        struct
        {
            PTCONBITS       PTCONbits;
            PTCON2BITS      PTCON2bits;
            uint16_t        :16;
            SEVTCMPBITS     SEVTCMPbits;
            uint16_t        :16;
            uint16_t        :16;
            uint16_t        :16;
            STCONBITS       STCONbits;
            STCON2BITS      STCON2bits;
            uint16_t        :16;
            SSEVTCMPBITS    SSEVTCMPbits;
            uint16_t        :16;
            uint16_t        :16;
            CHOPBITS        CHOPbits;
            uint16_t        :16;
            uint16_t        :16;
        };
    };
}PWM_sBaseRegisters_t;


typedef struct
{
    union 
    {
        struct 
        {
            uint16_t PWMCON;
            uint16_t IOCON;
            uint16_t FCLCON;
            uint16_t PDC;
            uint16_t PHASE;
            uint16_t DTR;
            uint16_t ALTDTR;
            uint16_t SDC;
            uint16_t SPHASE;
            uint16_t TRIG;
            uint16_t TRGCON;
            uint16_t :16;
            uint16_t PWMCAP;
            uint16_t LEBCON;
            uint16_t LEBDLY;
            uint16_t AUXCON;
        };
        
        struct
        {
            PWMCON1BITS     PWMCONbits;
            IOCON1BITS      IOCONbits;
            FCLCON1BITS     FCLCONbits;
            uint16_t        :16;
            uint16_t        :16;
            uint16_t        :16;
            uint16_t        :16;
            uint16_t        :16;
            uint16_t        :16;
            TRIG1BITS       TRIGbits;
            TRGCON1BITS     TRGCONbits;
            uint16_t        :16;
            PWMCAP1BITS     PWMCAPbits;
            LEBCON1BITS     LEBCONbits;
            LEBDLY1BITS     LEBDLYbits;
            AUXCON1BITS     AUXCONbits;
        };
    };
}PWM_sBaseGenerator_t;



typedef struct
{
    PWM_sBaseRegisters_t* psBaseRegisters;
    PWM_sBaseGenerator_t* psBaseGenerator[PWM_COUNT];
    uint8_t u8BitOffsetsISR_PSEV;
    uint8_t u8BitOffsetsISR_SSEV;
    uint8_t u8BitOffsetsISR_GEN[PWM_COUNT];
}PWM_sHandle_t;


PWM_sHandle_t PWM_sHandle = 
{
        (PWM_sBaseRegisters_t*)(&PTCON),
    {
        (PWM_sBaseGenerator_t*)(&PWMCON1),
        (PWM_sBaseGenerator_t*)(&PWMCON2),
        (PWM_sBaseGenerator_t*)(&PWMCON3),
        (PWM_sBaseGenerator_t*)(&PWMCON4),
        (PWM_sBaseGenerator_t*)(&PWMCON5),
        (PWM_sBaseGenerator_t*)(&PWMCON6),
    },
        /* Isr */
         9 + 3*16,          /* PSEV */         
         9 + 4*16,          /* SSEV */                       
    {   
        14 + 5*16,          /* PWM1 */                       
        15 + 5*16,          /* PWM2 */                       
         0 + 6*16,          /* PWM3 */                       
         1 + 6*16,          /* PWM4 */                       
         2 + 6*16,          /* PWM5 */                       
         3 + 6*16,          /* PWM6 */                       
    },
};

uint16_t PWM_u16PeriodTicksRequest = LOOPTIME1_TCY; /* must be less than 0x7FFF for center-aligned PWM with PSEV once per PWM PERIOD */
uint16_t PWM_u16PeriodTicks = 0;

void PWM_vInitGenerator(PWM_eModule_t eModule)
{
    PWM_sBaseGenerator_t* psBase = PWM_sHandle.psBaseGenerator[eModule];
    uint8_t u8PriorityISR = 4;
    
 	psBase->PHASE = PWM_u16PeriodTicks;
    /* Enable PWM output pins and configure them as complementary mode */
	psBase->PWMCON = 0x0204;	
    /* Dead Time */
	psBase->DTR = 0;
	psBase->ALTDTR = 0;   
    /* Duty Cycle */
    psBase->PDC = 0;
    
    #if defined __dsPIC33EP512MU814__
    /* I/O pins controlled by PWM */
	psBase->IOCON = 0xC000;
    /* Fault Disabled */
    psBase->FCLCON = 0x0003;    
    #endif


    psBase->PWMCONbits.TRGIEN = 1;         /* PWM Interrupt Enable */
    switch(eModule)
    {
        case PWM_1:
            #if !defined __dsPIC33EP512MU814__
            //FLTDAT PWM1L Low, PWM1H Low; SWAP disabled; OVRENH disabled; PENL enabled; PMOD Complementary Output Mode; OVRENL disabled; OSYNC disabled; POLL disabled; PENH enabled; CLDAT PWM1L Low, PWM1H Low; OVRDAT PWM1L Low, PWM1H Low; POLH disabled; 
            __builtin_write_PWMSFR(&IOCON1, 0xC000, &PWMKEY);
            //FLTPOL disabled; CLPOL disabled; CLSRC FLT1; CLMOD disabled; FLTMOD Fault input is disabled; IFLTMOD disabled; FLTSRC FLT32; 
            __builtin_write_PWMSFR(&FCLCON1, 0xFB, &PWMKEY);
            #endif
            u8PriorityISR = ISR_PRIORITY_PWM1;
            break;
        case PWM_2:
            #if !defined __dsPIC33EP512MU814__
            //FLTDAT PWM2L Low, PWM2H Low; SWAP disabled; OVRENH disabled; PENL enabled; PMOD Complementary Output Mode; OVRENL disabled; OSYNC disabled; POLL disabled; PENH enabled; CLDAT PWM2L Low, PWM2H Low; OVRDAT PWM2L Low, PWM2H Low; POLH disabled; 
            __builtin_write_PWMSFR(&IOCON2, 0xC000, &PWMKEY);
            //FLTPOL disabled; CLPOL disabled; CLSRC FLT1; CLMOD disabled; FLTMOD Fault input is disabled; IFLTMOD disabled; FLTSRC FLT32; 
            __builtin_write_PWMSFR(&FCLCON2, 0xFB, &PWMKEY);
            #endif
            u8PriorityISR = ISR_PRIORITY_PWM2;
            break;
        case PWM_3:
            #if !defined __dsPIC33EP512MU814__
            //FLTDAT PWM3L Low, PWM3H Low; SWAP disabled; OVRENH disabled; PENL enabled; PMOD Complementary Output Mode; OVRENL disabled; OSYNC disabled; POLL disabled; PENH enabled; CLDAT PWM3L Low, PWM3H Low; OVRDAT PWM3L Low, PWM3H Low; POLH disabled; 
            __builtin_write_PWMSFR(&IOCON3, 0xC000, &PWMKEY);
            //FLTPOL disabled; CLPOL disabled; CLSRC FLT1; CLMOD disabled; FLTMOD Fault input is disabled; IFLTMOD disabled; FLTSRC FLT32; 
            __builtin_write_PWMSFR(&FCLCON3, 0xFB, &PWMKEY);
            #endif
            u8PriorityISR = ISR_PRIORITY_PWM3;
            break;
        case PWM_4:
            #if !defined __dsPIC33EP512MU814__
            //FLTDAT PWM4L Low, PWM4H Low; SWAP disabled; OVRENH disabled; PENL enabled; PMOD Complementary Output Mode; OVRENL disabled; OSYNC disabled; POLL disabled; PENH enabled; CLDAT PWM4L Low, PWM4H Low; OVRDAT PWM4L Low, PWM4H Low; POLH disabled; 
            __builtin_write_PWMSFR(&IOCON4, 0xC000, &PWMKEY);
            //FLTPOL disabled; CLPOL disabled; CLSRC FLT1; CLMOD disabled; FLTMOD Fault input is disabled; IFLTMOD disabled; FLTSRC FLT32; 
            __builtin_write_PWMSFR(&FCLCON4, 0xFB, &PWMKEY);
            #endif
            u8PriorityISR = ISR_PRIORITY_PWM4;
            break;
        case PWM_5:
            #if !defined __dsPIC33EP512MU814__
            //FLTDAT PWM5L Low, PWM5H Low; SWAP disabled; OVRENH disabled; PENL enabled; PMOD Complementary Output Mode; OVRENL disabled; OSYNC disabled; POLL disabled; PENH enabled; CLDAT PWM5L Low, PWM5H Low; OVRDAT PWM5L Low, PWM5H Low; POLH disabled; 
            __builtin_write_PWMSFR(&IOCON5, 0xC000, &PWMKEY);
            //FLTPOL disabled; CLPOL disabled; CLSRC FLT1; CLMOD disabled; FLTMOD Fault input is disabled; IFLTMOD disabled; FLTSRC FLT32; 
            __builtin_write_PWMSFR(&FCLCON5, 0xFB, &PWMKEY);
            #endif
            u8PriorityISR = ISR_PRIORITY_PWM5;
            break;
        case PWM_6:
            #if !defined __dsPIC33EP512MU814__
            //FLTDAT PWM6L Low, PWM6H Low; SWAP disabled; OVRENH disabled; PENL enabled; PMOD Complementary Output Mode; OVRENL disabled; OSYNC disabled; POLL disabled; PENH enabled; CLDAT PWM6L Low, PWM6H Low; OVRDAT PWM6L Low, PWM6H Low; POLH disabled; 
            __builtin_write_PWMSFR(&IOCON6, 0xC000, &PWMKEY);
            //FLTPOL disabled; CLPOL disabled; CLSRC FLT1; CLMOD disabled; FLTMOD Fault input is disabled; IFLTMOD disabled; FLTSRC FLT32; 
            __builtin_write_PWMSFR(&FCLCON6, 0xFB, &PWMKEY);
            #endif
            u8PriorityISR = ISR_PRIORITY_PWM6;
            break;
        #if PWM_COUNT >= 7
        case PWM_7:
            u8PriorityISR = ISR_PRIORITY_PWM7;
            break;
        #endif
        default:
            break;
    }
    
    
    
    ISR_vSetDatIPC(PWM_sHandle.u8BitOffsetsISR_GEN[eModule],u8PriorityISR);
    ISR_vClrBitIFS(PWM_sHandle.u8BitOffsetsISR_GEN[eModule]);
    ISR_vClrBitIEC(PWM_sHandle.u8BitOffsetsISR_GEN[eModule]);
}


void PWM_vInit(void)
{
    PWM_sBaseRegisters_t* psBase = PWM_sHandle.psBaseRegisters;
    uint16_t u16Index;
       
    PWM_u16PeriodTicks = PWM_u16PeriodTicksRequest;
            
    psBase->PTCON = 0;
    psBase->STCON = 0;
    
    /* one trigger per PWM period with center aligned PWM */
	psBase->PTPER = 2*PWM_u16PeriodTicks+1; 

    /* Initialize Generators */
    for (u16Index = 0; u16Index < PWM_COUNT; u16Index++)
    {
        PWM_vInitGenerator(u16Index);
    }

	psBase->PTCON2 = 0x0000;
    psBase->STCON2 = 0x0000;
    
    /* one trigger per PWM period with center aligned PWM */
    psBase->STPER = 2*PWM_u16PeriodTicks+1;
    
    psBase->PTCONbits.EIPU = 0; /* Primary Period register updates occur on PWM cycle boundaries */
    psBase->STCONbits.EIPU = 0; /* Secondary Period register updates occur on PWM cycle boundaries */
    
    /* Phase of ADC capture set relative to PWM cycle: 0 offset and counting up */
	psBase->SEVTCMP = 0;   
    psBase->SSEVTCMP = 0;
    
    psBase->PTCONbits.SEIEN = 1;        /* PWM primary Special Event Trigger interrupt Enable */
    psBase->STCONbits.SEIEN = 1;        /* PWM secondary Special Event Trigger interrupt Enable */    

    ISR_vSetDatIPC(PWM_sHandle.u8BitOffsetsISR_PSEV,ISR_PRIORITY_PWMSEVTIP);
    ISR_vClrBitIFS(PWM_sHandle.u8BitOffsetsISR_PSEV);
    ISR_vSetBitIEC(PWM_sHandle.u8BitOffsetsISR_PSEV);   /* Enable PSEV */

    ISR_vSetDatIPC(PWM_sHandle.u8BitOffsetsISR_SSEV,ISR_PRIORITY_PWMSSEVTIP);
    ISR_vClrBitIFS(PWM_sHandle.u8BitOffsetsISR_SSEV);
    ISR_vClrBitIEC(PWM_sHandle.u8BitOffsetsISR_SSEV);   /* Disable SSEV */
    
    psBase->PTCONbits.PTEN = 1;    
}

#endif /* BOARDCFG_USE_OLD_NON_MCC_PWM == 0 */

#if BOARDCFG_USE_OLD_NON_MCC_ADC
#ifndef BOARDCFG_USE_MCC_ADC
/* *****************************************************************************
 * PWM_SpecialEvent_CallBack
 **************************************************************************** */
void PWM_SpecialEvent_CallBack(void)
{
    DEBUG_SET_PWM_PSEV();
    DEBUG_SET_PIN3();

    
    /* ADC1 */
    if (MAIN_bUseADC1SyncPrimaryTriggers == 0)
    {
        if (MAIN_bUseADC2SyncPrimaryTriggers == 0)
        {
            MAIN_bADC1Trigger = 1;
            MAIN_bADC1TriggerFreemaster = 0;
        }
        
        if(MAIN_bUseSequenceADC1State)  /* No Shift */
        {
            //AD1CON1bits.SAMP = 1;
            //AD1CON1bits.ASAM = 0; 
            //AD1CON2bits.CSCNA = 0;  /* reset scan */
            AD1CON1bits.SSRC = 7;//7- internal counter start conversion
            AD1CON2bits.CSCNA = 1;  /* scan inputs in AD1CSS starts from the beginning */
            AD1CON1bits.ASAM = 1;  
            
            MAIN_bADC1SyncPrimaryTriggersStatus = 1;
        }
    }
    else /* MAIN_bUseADC1SyncPrimaryTriggers */
    {
        if(MAIN_bUseSequenceADC1State)  /* Shift S1S2 */
        {
            if(MAIN_u8ADC1Trigger == TRIGGER_PSEV)
            {
                //AD1CON1bits.SAMP = 1;
                //AD1CON1bits.ASAM = 0;  
                //AD1CON2bits.CSCNA = 0;  /* reset scan */
                AD1CON1bits.SSRC = 7;//7- internal counter start conversion
                AD1CON2bits.CSCNA = 1;  /* scan inputs in AD1CSS starts from the beginning */
                AD1CON1bits.ASAM = 1;  
                MAIN_bADC1SyncPrimaryTriggersStatus = 1;
            }
            
            
//            if ((MAIN_u8ADC1Trigger == TRIGGER_PSEV) && (MAIN_bADC1SyncPrimaryTriggersStatusWait == 1))
//            {
//                AD1CON1bits.SSRC = 7;//7- internal counter start conversion
//                //AD1CON1bits.SAMP = 0;
//                AD1CON1bits.ASAM = 1;  
//                MAIN_bADC1SyncPrimaryTriggersStatus = 1;
//                MAIN_bADC1SyncPrimaryTriggersStatusWait = 0;
//            }
        }
        else
        {
            /* select channels for ADC1 */
            //AD1CHS0bits.CH0SA = DC_VOLTAGE_1;   /* Power supply DC-Link */
            //AD1CHS123bits.CH123SA = 0;  /* CH1 is AN0, CH2 is AN1, CH3 is AN2 */ 
            //AD1CON1bits.SAMP = 0;
        }
        //MAIN_u8ADC1Trigger = 10;
        
    }
    
    
    /* ADC2 */
    if (MAIN_bUseADC2SyncPrimaryTriggers == 0)
    {
        
    }
    else /* MAIN_bUseADC2SyncPrimaryTriggers */
    {
        if(MAIN_bUseSequenceADC2State)  /* Shift S1S2 */
        {
            if(MAIN_u8ADC2Trigger == TRIGGER_PSEV)
            {
                //AD2CON1bits.SAMP = 1;
                //AD2CON1bits.ASAM = 0;  
                //AD2CON2bits.CSCNA = 0;  
                AD2CON1bits.SSRC = 7;//7- internal counter start conversion
                AD2CON2bits.CSCNA = 1;  /* scan inputs in AD2CSS starts from the beginning */
                AD2CON1bits.ASAM = 1;  
                MAIN_bADC2SyncPrimaryTriggersStatus = 1;
            }
            
//            if ((MAIN_u8ADC2Trigger == TRIGGER_PSEV) && (MAIN_bADC2SyncPrimaryTriggersStatusWait == 1))
//            {
//                AD2CON1bits.SSRC = 7;//7- internal counter start conversion
//                AD2CON1bits.SAMP = 0;
//                AD2CON1bits.ASAM = 1;   
//                MAIN_bADC2SyncPrimaryTriggersStatus = 1;
//                MAIN_bADC2SyncPrimaryTriggersStatusWait = 0;
//            }
        }
        else
        {
            /* select channels for ADC2 */
            //AD2CHS0bits.CH0SA = DC_VOLTAGE_1;   /* Power supply DC-Link */
            //AD2CHS123bits.CH123SA = 0;  /* CH1 is AN0, CH2 is AN1, CH3 is AN2 */ 
            //AD2CON1bits.SAMP = 0;
        }
        //MAIN_u8ADC2Trigger = 10;
    }
    
    
    
    //vControlProcess();
   
    DEBUG_CLR_PIN3();
    DEBUG_CLR_PWM_PSEV();
}

/* *****************************************************************************
 * PWMSecSpEvInterruptFunction
 **************************************************************************** */
void PWMSecSpEvInterruptFunction(void)
{
    DEBUG_SET_PWM_SSEV();
    DEBUG_SET_PIN4();
   
	IFS4bits.PSESMIF = 0; 
    
    
    
    
    /* ADC1 */
    if (MAIN_bUseADC1SyncPrimaryTriggers == 0)
    {
        
    }
    else /* MAIN_bUseADC1SyncPrimaryTriggers */
    {
        if(MAIN_bUseSequenceADC1State)  /* Shift S1S2 */
        {
            if(MAIN_u8ADC1Trigger == TRIGGER_SSEV)
            {
                //AD1CON1bits.SAMP = 1;
                //AD1CON1bits.ASAM = 0;  
                //AD1CON2bits.CSCNA = 0; 
                AD1CON1bits.SSRC = 7;//7- internal counter start conversion
                AD1CON2bits.CSCNA = 1;  /* scan inputs in AD1CSS starts from the beginning */
                AD1CON1bits.ASAM = 1; 
                MAIN_bADC1SyncPrimaryTriggersStatus = 1;
            }

//            if ((MAIN_u8ADC1Trigger == TRIGGER_SSEV) && (MAIN_bADC1SyncPrimaryTriggersStatusWait == 1))
//            {
//                AD1CON1bits.SSRC = 7;//7- internal counter start conversion
//                AD1CON1bits.SAMP = 0;
//                AD1CON1bits.ASAM = 1;  
//                MAIN_bADC1SyncPrimaryTriggersStatus = 1;
//                MAIN_bADC1SyncPrimaryTriggersStatusWait = 0;
//            }
        }
        else
        {
            /* select channels for ADC1 */
            //AD1CHS0bits.CH0SA = DC_VOLTAGE_1;   /* Power supply DC-Link */
            //AD1CHS123bits.CH123SA = 0;  /* CH1 is AN0, CH2 is AN1, CH3 is AN2 */ 
            //AD1CON1bits.SAMP = 0;
        }
        //MAIN_u8ADC1Trigger = 11;
        
    }
    
    
    
    
    
    /* ADC2 */
    if (MAIN_bUseADC2SyncPrimaryTriggers == 0)
    {
        if (MAIN_bUseADC1SyncPrimaryTriggers == 0)
        {
            MAIN_bADC2Trigger = 1;
            MAIN_bADC2TriggerFreemaster = 0;
        }
    
        if(MAIN_bUseSequenceADC2State)  /* No Shift */
        {
            //AD2CON1bits.SAMP = 1;
            //AD2CON1bits.ASAM = 0;  
            //AD2CON2bits.CSCNA = 0;  /* reset scan */
            AD2CON1bits.SSRC = 7;//7- internal counter start conversion
            AD2CON2bits.CSCNA = 1;  /* scan inputs in AD2CSS starts from the beginning */
            AD2CON1bits.ASAM = 1;
            MAIN_bADC2SyncPrimaryTriggersStatus = 1;
        }
    }
    else /* MAIN_bUseADC2SyncPrimaryTriggers */
    {
        if(MAIN_bUseSequenceADC2State) /* Shift S1S2 */
        {
            if(MAIN_u8ADC2Trigger == TRIGGER_SSEV)
            {
                //AD2CON1bits.SAMP = 1;
                //AD2CON1bits.ASAM = 0;  
                //AD2CON2bits.CSCNA = 0;  /* reset scan */
                AD2CON1bits.SSRC = 7;//7- internal counter start conversion
                AD2CON2bits.CSCNA = 1;  /* scan inputs in AD2CSS starts from the beginning */
                AD2CON1bits.ASAM = 1;  
                MAIN_bADC2SyncPrimaryTriggersStatus = 1;
            }
//            if ((MAIN_u8ADC2Trigger == TRIGGER_SSEV) && (MAIN_bADC2SyncPrimaryTriggersStatusWait == 1))
//            {
//                AD2CON1bits.SSRC = 7;//7- internal counter start conversion
//                AD2CON1bits.SAMP = 0;
//                AD2CON1bits.ASAM = 1;   
//                MAIN_bADC2SyncPrimaryTriggersStatus = 1;
//                MAIN_bADC2SyncPrimaryTriggersStatusWait = 0;
//            }
        }
        else
        {
            /* select channels for ADC2 */
            //AD2CHS0bits.CH0SA = DC_VOLTAGE_1;   /* Power supply DC-Link */
            //AD2CHS123bits.CH123SA = 0;  /* CH1 is AN0, CH2 is AN1, CH3 is AN2 */ 
            //AD2CON1bits.SAMP = 0;
        }
        //MAIN_u8ADC2Trigger = 11;
    }










    
    //vControlProcess();

    DEBUG_CLR_PIN4();
    DEBUG_CLR_PWM_SSEV();
}

#endif /* BOARDCFG_USE_MCC_ADC */

#endif /* BOARDCFG_USE_OLD_NON_MCC_ADC */


#if (BOARDCFG_USE_OLD_NON_MCC_ADC == 0) || defined(BOARDCFG_USE_MCC_ADC)

/* *****************************************************************************
 * PWM_SpecialEvent_CallBack
 **************************************************************************** */
void PWM_SpecialEvent_CallBack(void)
{
    DEBUG_SET_PWM_PSEV();
    AD1CON1bits.SSRC = 7;   /* 7- internal counter start conversion */
    AD1CON2bits.CSCNA = 1;  /* scan inputs in AD1CSS starts from the beginning */
    AD1CON1bits.ASAM = 1;  
    MAIN_bADC1SyncPrimaryTriggersStatus = 1;
    
    AD2CON1bits.SSRC = 7;   /* 7- internal counter start conversion */
    AD2CON2bits.CSCNA = 1;  /* scan inputs in AD2CSS starts from the beginning */
    AD2CON1bits.ASAM = 1;
    MAIN_bADC2SyncPrimaryTriggersStatus = 1;
    DEBUG_CLR_PWM_PSEV();
}
#endif



#if BOARDCFG_USE_OLD_NON_MCC_PWM

#ifndef BOARDCFG_USE_MCC_PWM

/* *****************************************************************************
 * PWM1InterruptFunction
 **************************************************************************** */
void PWM1InterruptFunction(void)
{
    DEBUG_SET_PWM_1TO6();
 
	IFS5bits.PWM1IF = 0; 
    
    if (MAIN_bPhase1OffsetEnd)
    {
        PHASE1 = u16LoopTime1Tcy; 
        MAIN_bPhase1OffsetEnd = 0;
        MAIN_bPhase1OffsetCompleted = 1;
    }

    if (MAIN_u16Phase1OffsetCounter > 0)
    {
        MAIN_u16Phase1OffsetCounter--;
        if (MAIN_u16Phase1OffsetCounter == 0)
        {
            MAIN_bPhase1OffsetEnd = 1;
        }
    }
    
    if (MAIN_bPhase1OffsetRequested)
    {
        MAIN_bPhase1OffsetRequested = 0;
        PHASE1 += SPHASE1;  
        MAIN_bPhase1OffsetEnd = 1;
    }
    
    if (MAIN_bPhase1OffsetRequestSmooth)
    {
        MAIN_bPhase1OffsetRequestSmooth = 0;
        PHASE1 += 1;  
        MAIN_u16Phase1OffsetCounter = SPHASE1-1;
    }
    
    
    
    if (MAIN_bUsePWMShiftL1L2L3 == 0)
    {
        if (MAIN_bPhase1OffsetCompleted == 1)
        {
            IEC5bits.PWM1IE = 0;
        }
    }
    else
    {
    
    
        if (MAIN_bUseADC1SyncPrimaryTriggers)
        {     
            if(MAIN_bUseSequenceADC1State)
            {
                if (MAIN_bUsePWMShiftStack2 == 0)
                {
                    if(MAIN_u8ADC1Trigger == TRIGGER_PWM1)
                    {
                        AD1CON2bits.CSCNA = 1;  /* scan inputs in AD1CSS starts from the beginning */
                        AD1CON1bits.ASAM = 1; 
                        MAIN_bADC1SyncPrimaryTriggersStatus = 1;
                    }
                }
                else
                {                
                    AD1CSSL = ADC1_PWM1_CHANNELS;
                    AD1CON1bits.SAMP = 0;
                    AD1CON1bits.ASAM = 1;  
                    MAIN_bADC1SyncPrimaryTriggersStatus = 0;
                    MAIN_u8ADC1Trigger = 0;
                }
            }
            else
            {
                /* select channels for ADC1 */
                //AD1CHS0bits.CH0SA = L1_CURRENT_1;
                //AD1CHS123bits.CH123SA = 0;  /* CH1 is AN0, CH2 is AN1, CH3 is AN2 */ 
                //AD1CON1bits.DONE = 0;
                //AD1CON1bits.SAMP = 0;
                //AD1CON1bits.ASAM = 1;  
            }
        }

        if (MAIN_bUseADC2SyncPrimaryTriggers)
        {
            if(MAIN_bUseSequenceADC2State)
            {
                if (MAIN_bUsePWMShiftStack2 == 0)
                {
                    if(MAIN_u8ADC2Trigger == TRIGGER_PWM1)
                    {
                        AD2CON2bits.CSCNA = 1;  /* scan inputs in AD2CSS starts from the beginning */
                        AD2CON1bits.ASAM = 1; 
                        MAIN_bADC2SyncPrimaryTriggersStatus = 1;
                    }
                }
                else
                {                
                    AD2CSSL  = ADC2_PWM1_CHANNELS;
                    AD2CON1bits.SAMP = 0;
                    AD2CON1bits.ASAM = 1;  
                    MAIN_bADC2SyncPrimaryTriggersStatus = 0;
                    MAIN_u8ADC2Trigger = 0;
                }
            }
            else
            {
                /* select channels for ADC2 */
                //AD2CHS0bits.CH0SA = L1_CURRENT_1;
                //AD2CHS123bits.CH123SA = 0;  /* CH1 is AN0, CH2 is AN1, CH3 is AN2 */ 
                //AD2CON1bits.SAMP = 0;
                //AD1CON1bits.ASAM = 1;  
            }
        }

    
    }
    
    DEBUG_CLR_PWM_1TO6();
}

/* *****************************************************************************
 * PWM2InterruptFunction
 **************************************************************************** */
void PWM2InterruptFunction(void)
{
    DEBUG_SET_PWM_1TO6();

	IFS5bits.PWM2IF = 0; 
    
    
    if (MAIN_bPhase2OffsetEnd)
    {
        PHASE2 = u16LoopTime1Tcy;
        MAIN_bPhase2OffsetEnd = 0;
        MAIN_bPhase2OffsetCompleted = 1;
}

    if (MAIN_u16Phase2OffsetCounter > 0)
    {
        MAIN_u16Phase2OffsetCounter--;
        if (MAIN_u16Phase2OffsetCounter == 0)
        {
            MAIN_bPhase2OffsetEnd = 1;
        }
    }
    
    if (MAIN_bPhase2OffsetRequested)
    {
        MAIN_bPhase2OffsetRequested = 0;
        PHASE2 += SPHASE2;  
        MAIN_bPhase2OffsetEnd = 1;
    }
    
    if (MAIN_bPhase2OffsetRequestSmooth)
    {
        MAIN_bPhase2OffsetRequestSmooth = 0;
        PHASE2 += 1;  
        MAIN_u16Phase2OffsetCounter = SPHASE2-1;
    }


    if (MAIN_bUsePWMShiftL1L2L3 == 0)
    {
        if (MAIN_bPhase2OffsetCompleted == 1)
        {
            IEC5bits.PWM2IE = 0;
        }
    }
    else
    {
        if (MAIN_bUseADC1SyncPrimaryTriggers)
        {     
            if(MAIN_bUseSequenceADC1State)
            {
                if (MAIN_bUsePWMShiftStack2 == 0)
                {
                    if(MAIN_u8ADC1Trigger == TRIGGER_PWM2)
                    {
                        AD1CON2bits.CSCNA = 1;  /* scan inputs in AD1CSS starts from the beginning */
                        AD1CON1bits.ASAM = 1; 
                        MAIN_bADC1SyncPrimaryTriggersStatus = 1;
                    }
                }
                else
                {                
                    AD1CSSL  = ADC1_PWM2_CHANNELS;
                    AD1CON1bits.SAMP = 0;
                    AD1CON1bits.ASAM = 1;  
                    MAIN_bADC1SyncPrimaryTriggersStatus = 0;
                    MAIN_u8ADC1Trigger = 1;
                }
            }
            else
            {
                /* select channels for ADC1 */
                //AD1CHS0bits.CH0SA = L2_CURRENT_1;
                //AD1CHS123bits.CH123SA = 0;  /* CH1 is AN0, CH2 is AN1, CH3 is AN2 */ 
                //AD1CON1bits.SAMP = 0;
            }
        }

        if (MAIN_bUseADC2SyncPrimaryTriggers)
        {
            if(MAIN_bUseSequenceADC2State)
            {
                if (MAIN_bUsePWMShiftStack2 == 0)
                {
                    if(MAIN_u8ADC2Trigger == TRIGGER_PWM2)
                    {
                        AD2CON2bits.CSCNA = 1;  /* scan inputs in AD2CSS starts from the beginning */
                        AD2CON1bits.ASAM = 1; 
                        MAIN_bADC2SyncPrimaryTriggersStatus = 1;
                    }
                }
                else
                {                
                    AD2CSSL  = ADC2_PWM2_CHANNELS;
                    AD2CON1bits.SAMP = 0;
                    AD2CON1bits.ASAM = 1;  
                    MAIN_bADC2SyncPrimaryTriggersStatus = 0;
                    MAIN_u8ADC2Trigger = 1;
                }
            }
            else
            {
                /* select channels for ADC2 */
                //AD2CHS0bits.CH0SA = L2_CURRENT_1;
                //AD2CHS123bits.CH123SA = 0;  /* CH1 is AN0, CH2 is AN1, CH3 is AN2 */ 
                //AD2CON1bits.SAMP = 0;
            }
        }
    }
    
    DEBUG_CLR_PWM_1TO6();

}

/* *****************************************************************************
 * PWM3InterruptFunction
 **************************************************************************** */
void PWM3InterruptFunction(void)
{
    DEBUG_SET_PWM_1TO6();
	IFS6bits.PWM3IF = 0; 
    
    if (MAIN_bPhase3OffsetEnd)
    {
        PHASE3 = u16LoopTime1Tcy; 
        MAIN_bPhase3OffsetEnd = 0;
        MAIN_bPhase3OffsetCompleted = 1;
    }

    if (MAIN_u16Phase3OffsetCounter > 0)
    {
        MAIN_u16Phase3OffsetCounter--;
        if (MAIN_u16Phase3OffsetCounter == 0)
        {
            MAIN_bPhase3OffsetEnd = 1;
        }
    }
    
    if (MAIN_bPhase3OffsetRequested)
    {
        MAIN_bPhase3OffsetRequested = 0;
        PHASE3 += SPHASE3;  
        MAIN_bPhase3OffsetEnd = 1;
    }
    
    if (MAIN_bPhase3OffsetRequestSmooth)
    {
        MAIN_bPhase3OffsetRequestSmooth = 0;
        PHASE3 += 1;  
        MAIN_u16Phase3OffsetCounter = SPHASE3-1;
    }

    if (MAIN_bUsePWMShiftL1L2L3 == 0)
    {
        if (MAIN_bPhase3OffsetCompleted == 1)
        {
            IEC6bits.PWM3IE = 0;
        }
    }
    else
    {
        if (MAIN_bUseADC1SyncPrimaryTriggers)
        {     
            if(MAIN_bUseSequenceADC1State)
            {
                if (MAIN_bUsePWMShiftStack2 == 0)
                {
                    if(MAIN_u8ADC1Trigger == TRIGGER_PWM3)
                    {
                        AD1CON2bits.CSCNA = 1;  /* scan inputs in AD1CSS starts from the beginning */
                        AD1CON1bits.ASAM = 1; 
                        MAIN_bADC1SyncPrimaryTriggersStatus = 1;
                    }
                }
                else
                {                
                    AD1CSSL  = ADC1_PWM3_CHANNELS;
                    AD1CON1bits.SAMP = 0;
                    AD1CON1bits.ASAM = 1;  
                    MAIN_bADC1SyncPrimaryTriggersStatus = 0;
                    MAIN_u8ADC1Trigger = 2;
                }
            }
            else
            {
                /* select channels for ADC1 */
                //AD1CHS0bits.CH0SA = L3_CURRENT_1;
                //AD1CHS123bits.CH123SA = 0;  /* CH1 is AN0, CH2 is AN1, CH3 is AN2 */ 
                //AD1CON1bits.SAMP = 0;
            }
        }

        if (MAIN_bUseADC2SyncPrimaryTriggers)
        {
            if(MAIN_bUseSequenceADC2State)
            {
                if (MAIN_bUsePWMShiftStack2 == 0)
                {
                    if(MAIN_u8ADC2Trigger == TRIGGER_PWM3)
                    {
                        AD2CON2bits.CSCNA = 1;  /* scan inputs in AD2CSS starts from the beginning */
                        AD2CON1bits.ASAM = 1; 
                        MAIN_bADC2SyncPrimaryTriggersStatus = 1;
                    }
                }
                else
                {                
                    AD2CSSL  = ADC2_PWM3_CHANNELS;
                    AD2CON1bits.SAMP = 0;
                    AD2CON1bits.ASAM = 1;  
                    MAIN_bADC2SyncPrimaryTriggersStatus = 0;
                    MAIN_u8ADC2Trigger = 2;
                }
            }
            else
            {
                /* select channels for ADC2 */
                //AD2CHS0bits.CH0SA = L3_CURRENT_1;
                //AD2CHS123bits.CH123SA = 0;  /* CH1 is AN0, CH2 is AN1, CH3 is AN2 */ 
                //AD2CON1bits.SAMP = 0;
            }
        }
    }
    DEBUG_CLR_PWM_1TO6();

}

/* *****************************************************************************
 * PWM4InterruptFunction
 **************************************************************************** */
void PWM4InterruptFunction(void)
{
    DEBUG_SET_PWM_1TO6();
	IFS6bits.PWM4IF = 0; 
    
    if (MAIN_bPhase4OffsetEnd)
    {
        PHASE4 = u16LoopTime2Tcy; 
        MAIN_bPhase4OffsetEnd = 0;
        MAIN_bPhase4OffsetCompleted = 1;
    }

    if (MAIN_u16Phase4OffsetCounter > 0)
    {
        MAIN_u16Phase4OffsetCounter--;
        if (MAIN_u16Phase4OffsetCounter == 0)
        {
            MAIN_bPhase4OffsetEnd = 1;
        }
    }
    
    if (MAIN_bPhase4OffsetRequested)
    {
        MAIN_bPhase4OffsetRequested = 0;
        PHASE4 += SPHASE4;  
        MAIN_bPhase4OffsetEnd = 1;
    }
    
    if (MAIN_bPhase4OffsetRequestSmooth)
    {
        MAIN_bPhase4OffsetRequestSmooth = 0;
        PHASE4 += 1;  
        MAIN_u16Phase4OffsetCounter = SPHASE4-1;
    }

    if (MAIN_bUsePWMShiftL1L2L3 == 0)
    {
        if (MAIN_bPhase4OffsetCompleted == 1)
        {
            IEC6bits.PWM4IE = 0;
        }
    }
    else
    {
        if (MAIN_bUseADC1SyncPrimaryTriggers)
        {     
            if(MAIN_bUseSequenceADC1State)
            {
                if (MAIN_bUsePWMShiftStack2 == 0)
                {
                    if(MAIN_u8ADC1Trigger == TRIGGER_PWM4)
                    {
                        AD1CON2bits.CSCNA = 1;  /* scan inputs in AD1CSS starts from the beginning */
                        AD1CON1bits.ASAM = 1; 
                        MAIN_bADC1SyncPrimaryTriggersStatus = 1;
                    }
                }
                else
                {                
                    AD1CSSL  = ADC1_PWM4_CHANNELS;
                    AD1CON1bits.SAMP = 0;
                    AD1CON1bits.ASAM = 1;  
                    MAIN_bADC1SyncPrimaryTriggersStatus = 0;
                    MAIN_u8ADC1Trigger = 3;
                }
            }
            else
            {
                /* select channels for ADC1 */
                //AD1CHS0bits.CH0SA = L1_CURRENT_2;
                //AD1CHS123bits.CH123SA = 1;  /* CH1 is AN3, CH2 is AN4, CH3 is AN5 */ 
                //AD1CON1bits.SAMP = 0;
            }
        }

        if (MAIN_bUseADC2SyncPrimaryTriggers)
        {
            if(MAIN_bUseSequenceADC2State)
            {
                if (MAIN_bUsePWMShiftStack2 == 0)
                {
                    if(MAIN_u8ADC2Trigger == TRIGGER_PWM4)
                    {
                        AD2CON2bits.CSCNA = 1;  /* scan inputs in AD2CSS starts from the beginning */
                        AD2CON1bits.ASAM = 1; 
                        MAIN_bADC2SyncPrimaryTriggersStatus = 1;
                    }
                }
                else
                {                
                    AD2CSSL  = ADC2_PWM4_CHANNELS;
                    AD2CON1bits.SAMP = 0;
                    AD2CON1bits.ASAM = 1;  
                    MAIN_bADC2SyncPrimaryTriggersStatus = 0;
                    MAIN_u8ADC2Trigger = 3;
                }
            }
            else
            {
                /* select channels for ADC2 */
                //AD2CHS0bits.CH0SA = L1_CURRENT_2;
                //AD2CHS123bits.CH123SA = 1;  /* CH1 is AN3, CH2 is AN4, CH3 is AN5 */ 
                //AD2CON1bits.SAMP = 0;
            }
        }
    }
    
    DEBUG_CLR_PWM_1TO6();

}

/* *****************************************************************************
 * PWM5InterruptFunction
 **************************************************************************** */
void PWM5InterruptFunction(void)
{
    DEBUG_SET_PWM_1TO6();
    
	IFS6bits.PWM5IF = 0; 
    
    if (MAIN_bPhase5OffsetEnd)
    {
        PHASE5 = u16LoopTime2Tcy; 
        MAIN_bPhase5OffsetEnd = 0;
        MAIN_bPhase5OffsetCompleted = 1;
    }

    if (MAIN_u16Phase5OffsetCounter > 0)
    {
        MAIN_u16Phase5OffsetCounter--;
        if (MAIN_u16Phase5OffsetCounter == 0)
        {
            MAIN_bPhase5OffsetEnd = 1;
        }
    }
    
    if (MAIN_bPhase5OffsetRequested)
    {
        MAIN_bPhase5OffsetRequested = 0;
        PHASE5 += SPHASE5;  
        MAIN_bPhase5OffsetEnd = 1;
    }
    
    if (MAIN_bPhase5OffsetRequestSmooth)
    {
        MAIN_bPhase5OffsetRequestSmooth = 0;
        PHASE5 += 1;  
        MAIN_u16Phase5OffsetCounter = SPHASE5-1;
    }

    if (MAIN_bUsePWMShiftL1L2L3 == 0)
    {
        if (MAIN_bPhase5OffsetCompleted == 1)
        {
            IEC6bits.PWM5IE = 0;
        }
    }
    else
    {
        if (MAIN_bUseADC1SyncPrimaryTriggers)
        {     
            if(MAIN_bUseSequenceADC1State)
            {
                if (MAIN_bUsePWMShiftStack2 == 0)
                {
                    if(MAIN_u8ADC1Trigger == TRIGGER_PWM5)
                    {
                        AD1CON2bits.CSCNA = 1;  /* scan inputs in AD1CSS starts from the beginning */
                        AD1CON1bits.ASAM = 1; 
                        MAIN_bADC1SyncPrimaryTriggersStatus = 1;
                    }
                }
                else
                {                
                    AD1CSSL  = ADC1_PWM5_CHANNELS;
                    AD1CON1bits.SAMP = 0;
                    AD1CON1bits.ASAM = 1;  
                    MAIN_bADC1SyncPrimaryTriggersStatus = 0;
                    MAIN_u8ADC1Trigger = 4;
                }
            }
            else
            {
                /* select channels for ADC1 */
                //AD1CHS0bits.CH0SA = L2_CURRENT_2;
                //AD1CHS123bits.CH123SA = 1;  /* CH1 is AN3, CH2 is AN4, CH3 is AN5 */ 
                //AD1CON1bits.SAMP = 0;
            }
        }

        if (MAIN_bUseADC2SyncPrimaryTriggers)
        {
            if(MAIN_bUseSequenceADC2State)
            {
                if (MAIN_bUsePWMShiftStack2 == 0)
                {
                    if(MAIN_u8ADC2Trigger == TRIGGER_PWM5)
                    {
                        AD2CON2bits.CSCNA = 1;  /* scan inputs in AD2CSS starts from the beginning */
                        AD2CON1bits.ASAM = 1; 
                        MAIN_bADC2SyncPrimaryTriggersStatus = 1;
                    }
                }
                else
                {                
                    AD2CSSL  = ADC2_PWM5_CHANNELS;
                    AD2CON1bits.SAMP = 0;
                    AD2CON1bits.ASAM = 1;  
                    MAIN_bADC2SyncPrimaryTriggersStatus = 0;
                    MAIN_u8ADC2Trigger = 4;
                }
            }
            else
{
                /* select channels for ADC2 */
                //AD2CHS0bits.CH0SA = L2_CURRENT_2;
                //AD2CHS123bits.CH123SA = 1;  /* CH1 is AN3, CH2 is AN4, CH3 is AN5 */ 
                //AD2CON1bits.SAMP = 0;
            }
}
    }

    DEBUG_CLR_PWM_1TO6();
}

/* *****************************************************************************
 * PWM6InterruptFunction
 **************************************************************************** */
void PWM6InterruptFunction(void)
{
    DEBUG_SET_PWM_1TO6();
	IFS6bits.PWM6IF = 0; 

    if (MAIN_bPhase6OffsetEnd)
{
        PHASE6 = u16LoopTime2Tcy; 
        MAIN_bPhase6OffsetEnd = 0;
        MAIN_bPhase6OffsetCompleted = 1;
}

    if (MAIN_u16Phase6OffsetCounter > 0)
    {
        MAIN_u16Phase6OffsetCounter--;
        if (MAIN_u16Phase6OffsetCounter == 0)
        {
            MAIN_bPhase6OffsetEnd = 1;
        }
    }

    if (MAIN_bPhase6OffsetRequested)
    {
        MAIN_bPhase6OffsetRequested = 0;
        PHASE6 += SPHASE6;  
        MAIN_bPhase6OffsetEnd = 1;
    }

    if (MAIN_bPhase6OffsetRequestSmooth)
    {
        MAIN_bPhase6OffsetRequestSmooth = 0;
        PHASE6 += 1;  
        MAIN_u16Phase6OffsetCounter = SPHASE6-1;
    }

    if (MAIN_bUsePWMShiftL1L2L3 == 0)
    {
        if (MAIN_bPhase6OffsetCompleted == 1)
        {
            IEC6bits.PWM6IE = 0;
        }
    }
    else
    {
        if (MAIN_bUseADC1SyncPrimaryTriggers)
        {     
            MAIN_bADC1Trigger = 1;
            MAIN_bADC1TriggerFreemaster = 0;

            if(MAIN_bUseSequenceADC1State)
            {
                if (MAIN_bUsePWMShiftStack2 == 0)
                {
                    if(MAIN_u8ADC1Trigger == TRIGGER_PWM1)
                    {
                        AD1CON2bits.CSCNA = 1;  /* scan inputs in AD1CSS starts from the beginning */
                        AD1CON1bits.ASAM = 1; 
                        MAIN_bADC1SyncPrimaryTriggersStatus = 1;
                    }
                }
                else
                {                
                    AD1CSSL  = ADC1_PWM6_CHANNELS;
                    AD1CON1bits.SAMP = 0;
                    AD1CON1bits.ASAM = 1;  
                    MAIN_bADC1SyncPrimaryTriggersStatus = 0;
                    MAIN_u8ADC1Trigger = 5;
                }
            }
            else
            {
                /* select channels for ADC1 */
                //AD1CHS0bits.CH0SA = L3_CURRENT_2;
                //AD1CHS123bits.CH123SA = 1;  /* CH1 is AN3, CH2 is AN4, CH3 is AN5 */ 
                //AD1CON1bits.SAMP = 0;
            }
        }

        if (MAIN_bUseADC2SyncPrimaryTriggers)
        {
            MAIN_bADC2Trigger = 1;
            MAIN_bADC2TriggerFreemaster = 0;

            if(MAIN_bUseSequenceADC2State)
            {
                if (MAIN_bUsePWMShiftStack2 == 0)
                {
                    if(MAIN_u8ADC2Trigger == TRIGGER_PWM6)
                    {
                        AD2CON2bits.CSCNA = 1;  /* scan inputs in AD2CSS starts from the beginning */
                        AD2CON1bits.ASAM = 1; 
                        MAIN_bADC2SyncPrimaryTriggersStatus = 1;
                    }
                }
                else
                {                
                    AD2CSSL  = ADC2_PWM6_CHANNELS;
                    AD2CON1bits.SAMP = 0;
                    AD2CON1bits.ASAM = 1;  
                    MAIN_bADC2SyncPrimaryTriggersStatus = 0;
                    MAIN_u8ADC2Trigger = 5;
                }
            }
            else
            {
                /* select channels for ADC2 */
                //AD2CHS0bits.CH0SA = L3_CURRENT_2;
                //AD2CHS123bits.CH123SA = 1;  /* CH1 is AN3, CH2 is AN4, CH3 is AN5 */ 
                //AD2CON1bits.SAMP = 0;
            }
        }
    }

    DEBUG_CLR_PWM_1TO6();
}
#endif
#endif /* BOARDCFG_USE_OLD_NON_MCC_PWM */

#if BOARDCFG_USE_OLD_NON_MCC_ADC
#ifndef BOARDCFG_USE_MCC_ADC
/* *****************************************************************************
 * PWM7InterruptFunction
 **************************************************************************** */
void PWM7InterruptFunction(void)
{
    DEBUG_SET_PWM_7();
    //D1_LAT = 1;
	IFS6bits.PWM7IF = 0; 


    if (u16ExecControlNewDataSkip > 0)
{
        /* to do: compensation of skipped control pulses */
        //u16ExecControlNewDataSkip--;
        //vControlProcessSkippedPulses();
    }
    if (bExecControlNewData)
    {
        //D1_LAT = 1;
    ADCStack1Buffered[ADC_STACK_CURRENT_L1] = ADCStack1[ADC_STACK_CURRENT_L1];
    ADCStack1Buffered[ADC_STACK_CURRENT_L2] = ADCStack1[ADC_STACK_CURRENT_L2];
    ADCStack1Buffered[ADC_STACK_CURRENT_L3] = ADCStack1[ADC_STACK_CURRENT_L3];
    ADCStack1Buffered[ADC_STACK_DC_VOLTAGE] = ADCStack1[ADC_STACK_DC_VOLTAGE];
    ADCStack2Buffered[ADC_STACK_CURRENT_L1] = ADCStack2[ADC_STACK_CURRENT_L1];
    ADCStack2Buffered[ADC_STACK_CURRENT_L2] = ADCStack2[ADC_STACK_CURRENT_L2];
    ADCStack2Buffered[ADC_STACK_CURRENT_L3] = ADCStack2[ADC_STACK_CURRENT_L3];
    ADCStack2Buffered[ADC_STACK_DC_VOLTAGE] = ADCStack2[ADC_STACK_DC_VOLTAGE];

    ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L1] = ADC2Stack1[ADC2_STACK_CURRFBK_L1];
    ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L2] = ADC2Stack1[ADC2_STACK_CURRFBK_L2];
    ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L3] = ADC2Stack1[ADC2_STACK_CURRFBK_L3];
    ADC2Stack1Buffered[ADC2_STACK_DCVOLTAGE2] = ADC2Stack1[ADC2_STACK_DCVOLTAGE2];
    ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L1] = ADC2Stack1[ADC2_STACK_TEMPFBK_L1];
    ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L2] = ADC2Stack1[ADC2_STACK_TEMPFBK_L2];
    ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L3] = ADC2Stack1[ADC2_STACK_TEMPFBK_L3];
    ADC2Stack2Buffered[ADC2_STACK_DCVOLTAGE2] = ADC2Stack2[ADC2_STACK_DCVOLTAGE2];    

        u16ExecControlNewDataSkipBuffered = u16ExecControlNewDataSkip;
        u16ExecControlNewDataSkip = 0;
        

        bExecControlNewData = 0;

        CONTROL_vProcess();
        //D1_LAT = 0;
}
    DEBUG_CLR_PWM_7();    
}
#endif

#endif /* BOARDCFG_USE_OLD_NON_MCC_ADC */


#ifndef BOARDCFG_USE_MCC_PWM
/* *****************************************************************************
 * _PWMSpEventMatchInterrupt
 * 
 * Description: PWM Special Event Match Interrupt. 
 **************************************************************************** */
void __attribute__((interrupt, auto_psv)) _PWMSpEventMatchInterrupt(void)
{
    PWM_SpecialEvent_CallBack();
    
    IFS3bits.PSEMIF = 0; 
}

#endif

#if BOARDCFG_USE_OLD_NON_MCC_ADC
#ifndef BOARDCFG_USE_MCC_ADC
/* *****************************************************************************
 * _PWMSecSpEventMatchInterrupt
 * 
 * Description: PWM Secondary Special Event Match Interrupt. 
 **************************************************************************** */
void __attribute__((interrupt, auto_psv)) _PWMSecSpEventMatchInterrupt(void)
{
    PWMSecSpEvInterruptFunction();
}
#endif
#endif /* BOARDCFG_USE_OLD_NON_MCC_ADC */



#if (!defined(BOARDCFG_USE_MCC_ADC) && (BOARDCFG_USE_OLD_NON_MCC_ADC) ) || (!defined(BOARDCFG_USE_MCC_PWM)&& (BOARDCFG_USE_OLD_NON_MCC_PWM))
/* *****************************************************************************
 * _PWM7Interrupt
 * 
 * Description: PWM7 Interrupt. 
 **************************************************************************** */
void __attribute__((interrupt, auto_psv)) _PWM7Interrupt(void)
{
    PWM7InterruptFunction();
}
#endif

#ifndef BOARDCFG_USE_MCC_PWM
#if BOARDCFG_USE_OLD_NON_MCC_PWM
/* *****************************************************************************
 * _PWM1Interrupt
 * 
 * Description: PWM1 Interrupt. 
 **************************************************************************** */
void __attribute__((interrupt, auto_psv)) _PWM1Interrupt(void)
{
    PWM1InterruptFunction();
}

/* *****************************************************************************
 * _PWM2Interrupt
 * 
 * Description: PWM2 Interrupt. 
 **************************************************************************** */
void __attribute__((interrupt, auto_psv)) _PWM2Interrupt(void)
{
    PWM2InterruptFunction();
}

/* *****************************************************************************
 * _PWM3Interrupt
 * 
 * Description: PWM3 Interrupt. 
 **************************************************************************** */
void __attribute__((interrupt, auto_psv)) _PWM3Interrupt(void)
{
    PWM3InterruptFunction();
}

/* *****************************************************************************
 * _PWM4Interrupt
 * 
 * Description: PWM4 Interrupt. 
 **************************************************************************** */
void __attribute__((interrupt, auto_psv)) _PWM4Interrupt(void)
{
    PWM4InterruptFunction();
}

/* *****************************************************************************
 * _PWM5Interrupt
 * 
 * Description: PWM5 Interrupt. 
 **************************************************************************** */
void __attribute__((interrupt, auto_psv)) _PWM5Interrupt(void)
{
    PWM5InterruptFunction();
}

/* *****************************************************************************
 * _PWM6Interrupt
 * 
 * Description: PWM6 Interrupt. 
 **************************************************************************** */
void __attribute__((interrupt, auto_psv)) _PWM6Interrupt(void)
{
    PWM6InterruptFunction();
}
#endif /* BOARDCFG_USE_OLD_NON_MCC_PWM */
#else
/* *****************************************************************************
 * PWM_RegisterFunctionsInISR 
 **************************************************************************** */
void PWM_RegisterFunctionsInISR(void)
{
    #if 0
    PWM_SetSpecialEventInterruptHandler(PWM_SpecialEvent_CallBack);
    #endif
    
    #ifndef BOARDCFG_USE_MCC_PWM
    PWM_SetGenerator1InterruptHandler(PWM1InterruptFunction);
    PWM_SetGenerator2InterruptHandler(PWM2InterruptFunction);
    PWM_SetGenerator3InterruptHandler(PWM3InterruptFunction);
    PWM_SetGenerator4InterruptHandler(PWM4InterruptFunction);
    PWM_SetGenerator5InterruptHandler(PWM5InterruptFunction);
    PWM_SetGenerator6InterruptHandler(PWM6InterruptFunction);
    #endif

    #ifndef BOARDCFG_USE_MCC_ADC
    //PWM_SetGenerator7InterruptHandler(PWM7InterruptFunction);
    #endif
}
#endif



#if BOARDCFG_USE_OLD_NON_MCC_ADC
#ifndef BOARDCFG_USE_MCC_ADC

/* *****************************************************************************
 * ADCPWM_vPeripheralStop
 * 
 * Description: Stop ADC. Stop PWM. 
 **************************************************************************** */
void ADCPWM_vPeripheralStop(void)
{
    PWM_vStop();
    ADC_vStop();
}

/* *****************************************************************************
 * ADCPWM_vPeripheralReset
 * 
 * Description: Re-configure ADC. Re-configure PWM. 
 **************************************************************************** */
void ADCPWM_vPeripheralReset(void)
{    
#ifndef BOARDCFG_USE_MCC_PWM
    PWM_vReset();
#endif
    ADC1_vReset();
    ADC2_vReset();
}

/* *****************************************************************************
 * ADCPWM_vPeripheralStart
 * 
 * Description: Start ADC. Start PWM. 
 **************************************************************************** */
void ADCPWM_vPeripheralStart(void)
{    
    ADC_vInterruptsSetup();
#ifndef BOARDCFG_USE_MCC_PWM
    PWM_vInterruptsSetup();
#endif
    
    ADC_vStart();
    PWM_vStart();
}

void ADCPWM_vCalcTriggers(void)
{

    PWM_u16MinDutyTcy = MCLIB_u16Mul_u16u16(PWM_u16MinDutyCoef,u16LoopTime1Tcy);
    PWM_u16MaxDutyTcy = MCLIB_u16Mul_u16u16(PWM_u16MaxDutyCoef,u16LoopTime1Tcy);
    PWM_u16StartDutyTcy = MCLIB_u16Mul_u16u16(PWM_u16StartDutyCoef,u16LoopTime1Tcy);

    
    PWM_u16PWMGlobalCoef[TRIGGER_PWM1] = MCLIB_u16Mul_u16u16(PWM_u16PWMGlobalTcy[TRIGGER_PWM1],u16LoopTime1Tcy);
    PWM_u16PWMGlobalCoef[TRIGGER_PWM2] = MCLIB_u16Mul_u16u16(PWM_u16PWMGlobalTcy[TRIGGER_PWM2],u16LoopTime1Tcy);
    PWM_u16PWMGlobalCoef[TRIGGER_PWM3] = MCLIB_u16Mul_u16u16(PWM_u16PWMGlobalTcy[TRIGGER_PWM3],u16LoopTime1Tcy);
    PWM_u16PWMGlobalCoef[TRIGGER_PWM4] = MCLIB_u16Mul_u16u16(PWM_u16PWMGlobalTcy[TRIGGER_PWM4],u16LoopTime2Tcy);
    PWM_u16PWMGlobalCoef[TRIGGER_PWM5] = MCLIB_u16Mul_u16u16(PWM_u16PWMGlobalTcy[TRIGGER_PWM5],u16LoopTime2Tcy);
    PWM_u16PWMGlobalCoef[TRIGGER_PWM6] = MCLIB_u16Mul_u16u16(PWM_u16PWMGlobalTcy[TRIGGER_PWM6],u16LoopTime2Tcy);
    PWM_u16PWMGlobalCoef[TRIGGER_PWM7] = MCLIB_u16Mul_u16u16(PWM_u16PWMGlobalTcy[TRIGGER_PWM7],u16LoopTime1Tcy);
    PWM_u16PWMGlobalCoef[TRIGGER_PSEV] = MCLIB_u16Mul_u16u16(PWM_u16PWMGlobalTcy[TRIGGER_PSEV],u16LoopTime1Tcy);
    PWM_u16PWMGlobalCoef[TRIGGER_SSEV] = MCLIB_u16Mul_u16u16(PWM_u16PWMGlobalTcy[TRIGGER_SSEV],u16LoopTime2Tcy);
    
    
    
   
    MAIN_u16PWMTriggersSingleSample[TRIGGER_PWM1] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSingleSampleCoef[TRIGGER_PWM1],u16LoopTime1Tcy);
    MAIN_u16PWMTriggersSingleSample[TRIGGER_PWM2] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSingleSampleCoef[TRIGGER_PWM2],u16LoopTime1Tcy);
    MAIN_u16PWMTriggersSingleSample[TRIGGER_PWM3] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSingleSampleCoef[TRIGGER_PWM3],u16LoopTime1Tcy);
    MAIN_u16PWMTriggersSingleSample[TRIGGER_PWM4] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSingleSampleCoef[TRIGGER_PWM4],u16LoopTime2Tcy);
    MAIN_u16PWMTriggersSingleSample[TRIGGER_PWM5] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSingleSampleCoef[TRIGGER_PWM5],u16LoopTime2Tcy);
    MAIN_u16PWMTriggersSingleSample[TRIGGER_PWM6] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSingleSampleCoef[TRIGGER_PWM6],u16LoopTime2Tcy);
    MAIN_u16PWMTriggersSingleSample[TRIGGER_PWM7] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSingleSampleCoef[TRIGGER_PWM7],u16LoopTime1Tcy);
    MAIN_u16PWMTriggersSingleSample[TRIGGER_PSEV] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSingleSampleCoef[TRIGGER_PSEV],u16LoopTime1Tcy);
    MAIN_u16PWMTriggersSingleSample[TRIGGER_SSEV] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSingleSampleCoef[TRIGGER_SSEV],u16LoopTime2Tcy);
   
 
    MAIN_u16PWMTriggersSingleSampleBothStacks[TRIGGER_PWM1] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSingleSampleBothStacksCoef[TRIGGER_PWM1],u16LoopTime1Tcy);
    MAIN_u16PWMTriggersSingleSampleBothStacks[TRIGGER_PWM2] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSingleSampleBothStacksCoef[TRIGGER_PWM2],u16LoopTime1Tcy);
    MAIN_u16PWMTriggersSingleSampleBothStacks[TRIGGER_PWM3] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSingleSampleBothStacksCoef[TRIGGER_PWM3],u16LoopTime1Tcy);
    MAIN_u16PWMTriggersSingleSampleBothStacks[TRIGGER_PWM4] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSingleSampleBothStacksCoef[TRIGGER_PWM4],u16LoopTime2Tcy);
    MAIN_u16PWMTriggersSingleSampleBothStacks[TRIGGER_PWM5] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSingleSampleBothStacksCoef[TRIGGER_PWM5],u16LoopTime2Tcy);
    MAIN_u16PWMTriggersSingleSampleBothStacks[TRIGGER_PWM6] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSingleSampleBothStacksCoef[TRIGGER_PWM6],u16LoopTime2Tcy);
    MAIN_u16PWMTriggersSingleSampleBothStacks[TRIGGER_PWM7] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSingleSampleBothStacksCoef[TRIGGER_PWM7],u16LoopTime1Tcy);
    MAIN_u16PWMTriggersSingleSampleBothStacks[TRIGGER_PSEV] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSingleSampleBothStacksCoef[TRIGGER_PSEV],u16LoopTime1Tcy);
    MAIN_u16PWMTriggersSingleSampleBothStacks[TRIGGER_SSEV] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSingleSampleBothStacksCoef[TRIGGER_SSEV],u16LoopTime2Tcy);
   
    MAIN_u16PWMTriggersSequence[TRIGGER_PWM1] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSequenceCoef[TRIGGER_PWM1],u16LoopTime1Tcy);
    MAIN_u16PWMTriggersSequence[TRIGGER_PWM2] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSequenceCoef[TRIGGER_PWM2],u16LoopTime1Tcy);
    MAIN_u16PWMTriggersSequence[TRIGGER_PWM3] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSequenceCoef[TRIGGER_PWM3],u16LoopTime1Tcy);
    MAIN_u16PWMTriggersSequence[TRIGGER_PWM4] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSequenceCoef[TRIGGER_PWM4],u16LoopTime2Tcy);
    MAIN_u16PWMTriggersSequence[TRIGGER_PWM5] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSequenceCoef[TRIGGER_PWM5],u16LoopTime2Tcy);
    MAIN_u16PWMTriggersSequence[TRIGGER_PWM6] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSequenceCoef[TRIGGER_PWM6],u16LoopTime2Tcy);
    MAIN_u16PWMTriggersSequence[TRIGGER_PWM7] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSequenceCoef[TRIGGER_PWM7],u16LoopTime1Tcy);
    MAIN_u16PWMTriggersSequence[TRIGGER_PSEV] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSequenceCoef[TRIGGER_PSEV],u16LoopTime1Tcy);
    MAIN_u16PWMTriggersSequence[TRIGGER_SSEV] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSequenceCoef[TRIGGER_SSEV],u16LoopTime2Tcy);
   
 
    MAIN_u16PWMTriggersSequenceBothStacks[TRIGGER_PWM1] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSequenceBothStacksCoef[TRIGGER_PWM1],u16LoopTime1Tcy);
    MAIN_u16PWMTriggersSequenceBothStacks[TRIGGER_PWM2] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSequenceBothStacksCoef[TRIGGER_PWM2],u16LoopTime1Tcy);
    MAIN_u16PWMTriggersSequenceBothStacks[TRIGGER_PWM3] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSequenceBothStacksCoef[TRIGGER_PWM3],u16LoopTime1Tcy);
    MAIN_u16PWMTriggersSequenceBothStacks[TRIGGER_PWM4] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSequenceBothStacksCoef[TRIGGER_PWM4],u16LoopTime2Tcy);
    MAIN_u16PWMTriggersSequenceBothStacks[TRIGGER_PWM5] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSequenceBothStacksCoef[TRIGGER_PWM5],u16LoopTime2Tcy);
    MAIN_u16PWMTriggersSequenceBothStacks[TRIGGER_PWM6] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSequenceBothStacksCoef[TRIGGER_PWM6],u16LoopTime2Tcy);
    MAIN_u16PWMTriggersSequenceBothStacks[TRIGGER_PWM7] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSequenceBothStacksCoef[TRIGGER_PWM7],u16LoopTime1Tcy);
    MAIN_u16PWMTriggersSequenceBothStacks[TRIGGER_PSEV] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSequenceBothStacksCoef[TRIGGER_PSEV],u16LoopTime1Tcy);
    MAIN_u16PWMTriggersSequenceBothStacks[TRIGGER_SSEV] = MCLIB_u16Mul_u16u16(MAIN_u16PWMTriggersSequenceBothStacksCoef[TRIGGER_SSEV],u16LoopTime2Tcy);
   

}


uint16_t u16PWMForRecorderPeriod_usec = (1000000 / LOOPTIME1_HZ);

/* *****************************************************************************
 * ADCPWM_vStackFreqSetup
 * 
 * Description: 
 **************************************************************************** */
void ADCPWM_vStackFreqSetup( void )
{
    if (u16PWM1FreqHzReq != u16PWM1FreqHz)
    {
        ADCPWM_vPeripheralStop();
        
        u16PWM1FreqHz = u16PWM1FreqHzReq;
        bStack1FreqReset = 1;
        u16LoopTime1Tcy = MCLIB_u16DivSat_u32u16(u32Fp, u16PWM1FreqHz);
        ADCPWM_vCalcTriggers();
        MAIN_bResetPWM = 1;
        
        u16PWMForRecorderPeriod_usec = MCLIB_u16DivSat_u32u16(1000000, u16PWM1FreqHz);
        HMLIB_vRecorderSetTimeBase(u16PWMForRecorderPeriod_usec);


    }   
    
    if (u16PWM2FreqHzReq != u16PWM2FreqHz)
    {
        ADCPWM_vPeripheralStop();
        bStack2FreqReset = 1;
        u16PWM2FreqHz = u16PWM2FreqHzReq;
        u16LoopTime2Tcy = MCLIB_u16DivSat_u32u16(u32Fp, u16PWM2FreqHz);
        ADCPWM_vCalcTriggers();
        MAIN_bResetPWM = 1;
    }    
}

/* *****************************************************************************
 * ADCPWM_vStackModeSetup
 * 
 * Description: 
 **************************************************************************** */
void ADCPWM_vStackModeSetup( void )
{
    
    if (eStackModeStatus != eStackMode)
    {
        
        ADCPWM_vPeripheralStop();
        
        eStackModeStatus = eStackMode;
        
        switch (eStackMode)
        {
            
            case STACK_MODE_NO_SHIFT:
                /* Start ADC1 Sequence on PSEV */
                /* Start ADC2 Sequence on SSEV */
                MAIN_bUsePWMShiftL1L2L3 = 0;
                MAIN_bUsePWMShiftStack2 = 0;
                MAIN_bUseSequenceADC1 = 1;
                MAIN_bUseSequenceADC2 = 1;
                MAIN_bUseADC1SyncPrimaryTriggersRequest = 0;
                MAIN_bUseADC2SyncPrimaryTriggersRequest = 0;
                MAIN_bResetPWM = 1;
                break;
                
            case STACK_MODE_SHIFT_S1S2:     /* Issue with Sequence not changed - Mixing Channels - To be fixed */
                /* Start ADC1 Sequences on PSEV and SSEV */
                /* Start ADC2 Sequences on SSEV and PSEV */
                MAIN_bUsePWMShiftL1L2L3 = 0;
                MAIN_bUsePWMShiftStack2 = 1;
                MAIN_bUseSequenceADC1 = 1;
                MAIN_bUseSequenceADC2 = 1;
                MAIN_bUseADC1SyncPrimaryTriggersRequest = 1;
                MAIN_bUseADC2SyncPrimaryTriggersRequest = 1;
                MAIN_bResetPWM = 1;
                break;
                
            case STACK_MODE_SHIFT_L1L2L3:   /* Issue with Sequence not changed - Mixing Channels - To be fixed */
                MAIN_bUsePWMShiftL1L2L3 = 1;
                MAIN_bUsePWMShiftStack2 = 0;
                MAIN_bUseSequenceADC1 = 1;
                MAIN_bUseSequenceADC2 = 1;
                MAIN_bUseADC1SyncPrimaryTriggersRequest = 1;
                MAIN_bUseADC2SyncPrimaryTriggersRequest = 1;             
                MAIN_bResetPWM = 1;
                break;
                
            case STACK_MODE_SHIFT_L1L2L3_S1S2:
                MAIN_bUsePWMShiftL1L2L3 = 1;
                MAIN_bUsePWMShiftStack2 = 1;
                MAIN_bUseSequenceADC1 = 0;
                MAIN_bUseSequenceADC2 = 0;
                MAIN_bUseADC1SyncPrimaryTriggersRequest = 1;
                MAIN_bUseADC2SyncPrimaryTriggersRequest = 1;
                MAIN_bResetPWM = 1;
                break;
                
            default:
                break;
    
                
        }
        
    }
    
    
    
}

/* *****************************************************************************
 * ADCPWM_vStackModeChangeProcess
 * 
 * Description: 
 **************************************************************************** */
void ADCPWM_vStackModeChangeProcess( void )
{
    ADCPWM_vStackFreqSetup();
    ADCPWM_vStackModeSetup();
    
    if (MAIN_bResetPWM)
    {
        MAIN_bResetPWM = 0;
        ADCPWM_vPeripheralReset();
        ADCPWM_vPeripheralStart();
    }
}

/* *****************************************************************************
 * ADCPWM_vStackModeInit
 * 
 * Description: 
 **************************************************************************** */
void ADCPWM_vStackModeInit( uint32_t u32Fcy )
{
    MAIN_bResetPWM = 1; /* on initialization always reset peripherals */
    u32Fp = u32Fcy;
    ADCPWM_vStackFreqSetup();
    ADCPWM_vStackModeSetup();
    
    
    
    
    if (MAIN_bResetPWM)
    {
        MAIN_bResetPWM = 0;
        ADCPWM_vPeripheralReset();
        ADCPWM_vPeripheralStart();
    }
}

#endif

#endif /* BOARDCFG_USE_OLD_NON_MCC_ADC */


void ADCPWM_vSetDutyCycleStack1 (MCLIB_s3Param_u16_t* psDutyCycles)
{
    PDC1 = psDutyCycles->u16Arg1;
    PDC2 = psDutyCycles->u16Arg2;
    PDC3 = psDutyCycles->u16Arg3;
}

void ADCPWM_vSetDutyCycleStack2 (MCLIB_s3Param_u16_t* psDutyCycles)
{
    PDC4 = psDutyCycles->u16Arg1;
    PDC5 = psDutyCycles->u16Arg2;
    PDC6 = psDutyCycles->u16Arg3;
}

void ADCPWM_vSetDutyCycleStack3 (MCLIB_s3Param_u16_t* psDutyCycles)
{
}



#if BOARDCFG_USE_OLD_NON_MCC_ADC
#ifndef BOARDCFG_USE_MCC_ADC

uint16_t ADCPWM_u16GetPWM1FreqHzReq (void)
{
    return u16PWM1FreqHzReq;
}

uint16_t ADCPWM_u16GetPWM2FreqHzReq (void)
{
    return u16PWM2FreqHzReq;
}

uint16_t ADCPWM_u16GetPWM1FreqHz (void)
{
    return u16PWM1FreqHz;
}

uint16_t ADCPWM_u16GetPWM2FreqHz (void)
{
    return u16PWM2FreqHz;
}

uint16_t ADCPWM_u16GetLoopTime1Tcy (void)
{
    return u16LoopTime1Tcy;
}

uint16_t ADCPWM_u16GetLoopTime2Tcy (void)
{
    return u16LoopTime2Tcy;
}

void ADCPWM_vSetPWM1FreqHzReq (uint16_t u16Input)
{
    u16PWM1FreqHzReq = u16Input;
}

void ADCPWM_vSetPWM2FreqHzReq (uint16_t u16Input)
{
    u16PWM2FreqHzReq = u16Input;
}

#endif
#endif /* BOARDCFG_USE_OLD_NON_MCC_ADC */
