/* *****************************************************************************
 * File:   control.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 02 11 15:49
 * 
 * Description: Control of Two 3-Phase Inverters 
 * 
  *
 *
 * Re-factoring Notes:
 * s16Speed1Hz  ->  sControlParameter.sStack[STACK_1].s16FieldSpeedEmulation_HZ
 *
 *
 *
 *
 **************************************************************************** */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdbool.h>
#include <stdint.h>
//#include "stdextend.h"
#include <stdlib.h>

#include "boardcfg.h"
#include "sys_drv.h"
#include "control.h"

#include "adcpwm.h"
#include "mclib.h"
#include "hmlib.h"

/* FPGA's */

#if BOARDCFG_USE_FPGA_TRIP
#include "fpga_trip.h"
#endif

#if BOARDCFG_USE_FPGA_PWM
#include "fpga_pwm.h"
#endif

#if BOARDCFG_USE_DAC_TRIP
#include "dac_trip.h"
#endif

#if BOARDCFG_USE_DIO
#include "dio.h"
#endif

#if USE_CSMON
#include "csmon.h"
#endif

#if BOARDCFG_USE_FPGA
#include "fpga_common.h" 
#endif

#if USE_CTREADER
#include "ctreader.h" 
#endif


#include "testproc.h"


//#include "isr.h"


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
    uint16_t bErrorIO      : 1;
    uint16_t bErrorFPGA      : 1;
    uint16_t bErrorTrip      : 1;
    uint16_t bErrorPWM       : 1;
    uint16_t bErrorTemp      : 1;

}CONTROL_sStopError_t;

typedef union
{
    uint16_t u16Register;
    CONTROL_sStopError_t sStopError;

}CONTROL_uStopError_t;





/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
CONTROL_uStopError_t CONTROL_uStopError = {0};

bool bUseControlTemperatureTrip = BOARDCFG_USE_TEMPERATURE_MEASUREMENT_TRIP;

/* Fault Reset State Hold */
uint16_t u16FaultResetDelay = CONTROL_FAULT_RESET_DELAY_TICKS;
uint16_t u16FaultResetTimeout = CONTROL_FAULT_RESET_DELAY_TICKS;

/* Main Control State */
eControlState_t eControlState = CTRL_STA_INIT;

/* Main Control Data */
sControlData_t sControlData;

/* Main Control Access Flags */
sControlAccess_t sControlAccess;





/* Control Request Parameters Limitation */
sControlLimit_t sControlLimitReal =
{
     CONTROL_CURRENT_REQUEST_MAX_A,                 /* s16CurrentRequestMax;        */
     CONTROL_FIELD_FREQUENCY_REQUEST_MAX_HZ,        /* s16FieldFrequencyRequestMax; */
     CONTROL_FIELD_FREQUENCY_REQUEST_MIN_HZ,        /* s16FieldFrequencyRequestMin; */
     CONTROL_PWM_FREQUENCY_REQUEST_MAX_HZ,          /* s16PWMFrequencyRequestMax;   */
     CONTROL_PWM_FREQUENCY_REQUEST_MIN_HZ,          /* s16PWMFrequencyRequestMin;   */
};

sControlLimit_t sControlLimit =
{
     MCLIB_s16Fractional((CONTROL_CURRENT_REQUEST_MAX_A * AmperesToADCResultMultiplier * (1 << CURRENT_FEEDBACK_FIX_MULT_SHIFT_CONTROL))),                 /* s16CurrentRequestMax;        */
     (CONTROL_FIELD_FREQUENCY_REQUEST_MAX_HZ / SPEED_RESOLUTION_HZ),        /* s16FieldFrequencyRequestMax; */
     (CONTROL_FIELD_FREQUENCY_REQUEST_MIN_HZ / SPEED_RESOLUTION_HZ),        /* s16FieldFrequencyRequestMin; */
     CONTROL_PWM_FREQUENCY_REQUEST_MAX_HZ,          /* s16PWMFrequencyRequestMax;   */
     CONTROL_PWM_FREQUENCY_REQUEST_MIN_HZ,          /* s16PWMFrequencyRequestMin;   */
};


sBoardParameter_t sBoardParameter;
sControlParameter_t sControlParameter;
sControlParameterLast_t sControlParameterLast;


int16_t s16CurrentRMSReq1Ph_dA = 0;
int16_t s16CurrentRMSReq1Ph_dA_Old;

int16_t s16RMSCurrent1PHRequestAmperes = RMS_CURRENT_1_PH_REQ_A;
int16_t s16RMSCurrent1PHRequestAmperes_Old = RMS_CURRENT_1_PH_REQ_A;

int16_t s16RMSCurrent1PHEmulationAmperes = RMS_CURRENT_1_PH_EMULATION_A;
int16_t s16RMSCurrent1PHEmulationAmperes_Old = RMS_CURRENT_1_PH_EMULATION_A;




int16_t s16PositionIntegratorOld;

bool bUseNewRMS = CONTROL_USE_NEW_RMS;


FPGA_uErrorCode_t FPGA_uErrorCode;

uTempError_t uTempError = {0x00};

uint32_t u32CurrentSensingGateDriverVoltageBaseMultiplierOld = 0;  /* Put Invalid Value Here to Execute On Startup */

uint16_t ADCStack1Offset[ADC_STACK_COUNT] =
{
    0x8000,
    0x8000,
    0x8000,
    0x8000,     /* was 0 before new TSC */
};
uint16_t ADCStack2Offset[ADC_STACK_COUNT] =
{
    0x8000,
    0x8000,
    0x8000,
    0x8000,     /* was 0 before new TSC */
};
uint16_t ADCStack3Offset[ADC_STACK_COUNT] =
{
    0x8000,
    0x8000,
    0x8000,
    0x8000,     /* was 0 before new TSC */
};
uint16_t ADCDCLinkOffset[ADC_STACK_COUNT] =
{
    0x8000,
    0x8000,
    0x8000,
    0,
};
uint16_t ADCLine_1Offset[ADC_STACK_COUNT] =
{
    0x8000,
    0x8000,
    0,
    0,
};
uint16_t ADCLine_2Offset[ADC_STACK_COUNT]=
{
    0,
    0,
    0,
    0,
};

uint16_t ADC2Stack1Offset[ADC2_STACK_COUNT] = 
{
    /* CFBK */
    0x8000,
    0x8000,
    0x8000,
    
    /* TFBK */
    0x0000,
    0x0000,
    0x0000,
      
    /* VOLT2 */
    0x0000, 
};

uint16_t ADC2Stack2Offset[ADC2_STACK_COUNT] = 
{
    /* CFBK */
    0x8000,
    0x8000,
    0x8000,
    
    /* TFBK */
    0x0000,
    0x0000,
    0x0000,
      
    /* VOLT2 */
    0x0000, 
};




uint32_t ADCStack1OffsetBuffer[ADC_STACK_COUNT];
uint32_t ADCStack2OffsetBuffer[ADC_STACK_COUNT];
uint32_t ADCStack3OffsetBuffer[ADC_STACK_COUNT];
uint32_t ADCDCLinkOffsetBuffer[ADC_STACK_COUNT];
uint32_t ADCLine_1OffsetBuffer[ADC_STACK_COUNT];
uint32_t ADCLine_2OffsetBuffer[ADC_STACK_COUNT];

int16_t ADCStack1NoOffset[ADC_STACK_COUNT];
int16_t ADCStack2NoOffset[ADC_STACK_COUNT];
int16_t ADCStack3NoOffset[ADC_STACK_COUNT];
int16_t ADCDCLinkNoOffset[ADC_STACK_COUNT];
int16_t ADCLine_1NoOffset[ADC_STACK_COUNT];
int16_t ADCLine_2NoOffset[ADC_STACK_COUNT];


uint16_t ADCLine_1NoOffset_LLV3_H2O_IN ;
uint16_t ADCLine_2NoOffset_LLV4_H2O_OUT;


uint16_t u16HeatExchangerFanStartTemperature_Deg = 40;
uint16_t u16HeatExchangerFanStopTemperature_Deg = 30;
uint16_t u16HeatExchangerFanStartTemperature = 0;
uint16_t u16HeatExchangerFanStopTemperature = 0;

int16_t ADCStack1NoOffsetReal[ADC_STACK_COUNT];
int16_t ADCStack2NoOffsetReal[ADC_STACK_COUNT];
int16_t ADCStack3NoOffsetReal[ADC_STACK_COUNT];


uint32_t ADC2Stack1OffsetBuffer[ADC2_STACK_COUNT];
uint32_t ADC2Stack2OffsetBuffer[ADC2_STACK_COUNT];


int16_t ADC2Stack1NoOffset[ADC2_STACK_COUNT];
int16_t ADC2Stack2NoOffset[ADC2_STACK_COUNT];



CONTROL_uStackUsageMask_t CONTROL_uStackMask =
{
     0x7
};

#if BOARDCFG_USE_DAC_TRIP

#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
int16_t DACOffsetCurrentStack1[DAC_TRIP_CURRENT_ONE_STACK_COUNT]; 
int16_t DACOffsetCurrentStack2[DAC_TRIP_CURRENT_ONE_STACK_COUNT];
int16_t DACOffsetVoltageStack1;
int16_t DACOffsetVoltageStack2;


int16_t DACOffsetCurrFbkStack1[DAC_TRIP_CURRENT_ONE_STACK_COUNT];
int16_t DACOffsetVoltage2Stack1;
int16_t DACOffsetVoltage2Stack2;
#else

uint16_t DACOffsetCurrentStack1[DAC_TRIP_CURRENT_ONE_STACK_COUNT]; 
uint16_t DACOffsetCurrentStack2[DAC_TRIP_CURRENT_ONE_STACK_COUNT];
uint16_t DACOffsetVoltageStack1;
uint16_t DACOffsetVoltageStack2;


uint16_t DACOffsetVoltage2Stack1;
uint16_t DACOffsetVoltage2Stack2;

uint16_t DACOffsetCurrentStack1Avg;
uint16_t DACOffsetCurrentStack2Avg;

#endif

#endif

eErrorState_t eErrorState = CTRL_ERR_NO_INIT;
eErrorState_t eErrorStateLast = CTRL_ERR_NO_INIT;

eErrorState_t eFirstDetectedError = CTRL_ERR_NO_ERROR;

bool bStartupFlag = true;

bool bUseOffsetOnTempFbk = 1;
bool bUseOffsetOnCurrFbk = 1;

bool bCalibrationWithPWMEnabled = 0;

bool bUseADCResultFix = CONTROL_CFG_USE_CALC_ADC_FIX;
bool bUseSVM = 0;

/* 0.1 Resolution dA(0.1A), dV(0.1V), dHz(0.1Hz) */
int16_t CurrentStack1[3];
int16_t CurrentStack2[3];
uint16_t VoltageStack1;
int16_t VoltageStack2;
int16_t SpeedStack1;
int16_t SpeedStack2;

int16_t CurrFbkStack1[3];
int16_t TempFbkStack1[3];
int16_t CurrFbkStack2[3];
int16_t TempFbkStack2[3];
int16_t Voltage2Stack1;
int16_t Voltage2Stack2;


int16_t u16_irms_sns1;
int16_t u16_irms_sns2;
int16_t u16_irms_sns3;

int16_t u16_irms_mod1;
int16_t u16_irms_mod2;
int16_t u16_irms_mod3;

uint16_t u16_urms_filt;

bool MAIN_bDebugResolutionIncrease = 0;

volatile bool bPWMEnableRequest = 0;
volatile bool bPWMEnableRequestDelay = 1;

bool bUseControlNoErrorEmulation = 0;

bool bUseIOError = 1;

bool bUseRunDelay = 1;

uint32_t u32PropCoefBase;
uint32_t u32IntgCoefBase;

int32_t s32PropCoefBase;
int32_t s32IntgCoefBase;

int32_t s32PropCoef;
int32_t s32IntgCoef;

bool bOffsetPerformedCurrentStack1 = 0;
bool bOffsetPerformedVoltageStack1 = 0;
bool bOffsetPerformedCurrentStack2 = 0;
bool bOffsetPerformedVoltageStack2 = 0;

bool bOffsetPerformedCurrentStack3 = 0;
bool bOffsetPerformedVoltageStack3 = 0;
bool bOffsetPerformedCurrentDCLink = 0;
bool bOffsetPerformedVoltageDCLink = 0;

bool bOffsetPerformedCurrentLine_1_LLV12 = 0;
bool bOffsetPerformedCurrentLine_1_LLV3 = 0;
bool bOffsetPerformedVoltageLine_1 = 0;
bool bOffsetPerformedCurrentLine_2 = 0;
bool bOffsetPerformedVoltageLine_2 = 0;


bool bOffsetPerformedCurrFbkStack1 = 0;
bool bOffsetPerformedTmp1FbkStack1 = 0;
bool bOffsetPerformedTmp2FbkStack1 = 0;
bool bOffsetPerformedTmp3FbkStack1 = 0;
bool bOffsetPerformedTempFbkStack1 = 0;
bool bOffsetPerformedVoltag2Stack1 = 0;

bool bOffsetPerformedCurrFbkStack2 = 0;
bool bOffsetPerformedTmp1FbkStack2 = 0;
bool bOffsetPerformedTmp2FbkStack2 = 0;
bool bOffsetPerformedTmp3FbkStack2 = 0;
bool bOffsetPerformedTempFbkStack2 = 0;
bool bOffsetPerformedVoltag2Stack2 = 0;

bool bOffsetCalcAllowSetVoltageStack1 = 1;

bool bOffsetCalcAllowCurrentStack1 = 1;     /* IFBK1..3 */


bool bOffsetCalcAllowVoltageStack1UseAutoManipulation = BOARDCFG_VOLTAGE_STACK1_OFFSET_CALC_AUTO_MANIPULATION;
bool bOffsetCalcAllowVoltageStack1 = 0;     /* DCLV1 (Manipulated Inside Control - See bOffsetCalcAllowVoltageStack1UseAutoManipulation) */

bool bOffsetCalcAllowCurrentStack2 = 1;     /* IFBK4..6 */
bool bOffsetCalcAllowVoltageStack2 = 1;     /* DCLV2 */

bool bOffsetCalcAllowCurrentStack3 = 1;     /* IFBK7..9 */
bool bOffsetCalcAllowVoltageStack3 = 1;     /* DCLV3 */
bool bOffsetCalcAllowCurrentDCLink = 1;     /* DCLI1..3 */
bool bOffsetCalcAllowVoltageDCLink = 0;     /* DCLV4 */

bool bOffsetCalcAllowCurrentLine_1_LLV12 = 1;     /* LLV1..2 */
bool bOffsetCalcAllowCurrentLine_1_LLV3 = 0;      /* LLV3 */
bool bOffsetCalcAllowVoltageLine_1 = 0;     /* DACOUTA */
bool bOffsetCalcAllowCurrentLine_2 = 0;     /* LLV4..6 */
bool bOffsetCalcAllowVoltageLine_2 = 0;     /* DACOUTB */


bool bOffsetCalcAllowCurrFbkStack1 = 1;
bool bOffsetCalcAllowTempFbkStack1 = 0;
bool bOffsetCalcAllowVoltag2Stack1 = 1;

bool bOffsetCalcAllowCurrFbkStack2 = 1;
bool bOffsetCalcAllowTempFbkStack2 = 0;
bool bOffsetCalcAllowVoltag2Stack2 = 1;

bool bDetectDriverPhase1Stack1 = 0;
bool bDetectDriverPhase2Stack1 = 0;
bool bDetectDriverPhase3Stack1 = 0;

bool bDetectDriverPhase1Stack2 = 0;
bool bDetectDriverPhase2Stack2 = 0;
bool bDetectDriverPhase3Stack2 = 0;

bool bAllOffsetPerformed = 0;
bool bAllOffsetPerformedReset = 0;

#if BOARDCFG_USE_INFO_CALC_MIN_MAX_AVG
bool bCalculateMinMaxReset = 1;
bool bCalculateMinMax = 0;

bool bCalcADCMinMax = CONTROL_CFG_USE_CALC_ADC_MIN_MAX;
bool bCalcADCAvg = CONTROL_CFG_USE_CALC_ADC_AVG;
bool bCalcADCAvgSampleCountChangeRuntime = CONTROL_CFG_USE_CALC_ADC_AVG_INIT_RUNTIME;
bool bUseRuntimeOffsetFix = 0;
bool bResetADCMinMax = 1;


int16_t CurrentStack1Min[3];
int16_t CurrentStack2Min[3];
uint16_t VoltageStack1Min;
int16_t VoltageStack2Min;
int16_t CurrFbkStack1Min[3];
int16_t TempFbkStack1Min[3];
int16_t Voltage2Stack1Min;
int16_t Voltage2Stack2Min;

int16_t CurrentStack1Max[3];
int16_t CurrentStack2Max[3];
uint16_t VoltageStack1Max;
int16_t VoltageStack2Max;
int16_t CurrFbkStack1Max[3];
int16_t TempFbkStack1Max[3];
int16_t Voltage2Stack1Max;
int16_t Voltage2Stack2Max;


uint16_t ADCStack1ResultMin[ADC_STACK_COUNT];
uint16_t ADCStack2ResultMin[ADC_STACK_COUNT];

#if 0
uint16_t ADCStack3ResultMin[ADC_STACK_COUNT];
uint16_t ADCDCLinkResultMin[ADC_STACK_COUNT];
uint16_t ADCLine_1ResultMin[ADC_STACK_COUNT];
uint16_t ADCLine_2ResultMin[ADC_STACK_COUNT];
#endif

uint16_t ADCStack1ResultMax[ADC_STACK_COUNT];
uint16_t ADCStack2ResultMax[ADC_STACK_COUNT];

#if 0
uint16_t ADCStack3ResultMax[ADC_STACK_COUNT];
uint16_t ADCDCLinkResultMax[ADC_STACK_COUNT];
uint16_t ADCLine_1ResultMax[ADC_STACK_COUNT];
uint16_t ADCLine_2ResultMax[ADC_STACK_COUNT];
#endif

uint16_t ADCStack1ResultMinNoOffset[ADC_STACK_COUNT];
uint16_t ADCStack2ResultMinNoOffset[ADC_STACK_COUNT];

#if 0
uint16_t ADCStack3ResultMinNoOffset[ADC_STACK_COUNT];
uint16_t ADCDCLinkResultMinNoOffset[ADC_STACK_COUNT];
uint16_t ADCLine_1ResultMinNoOffset[ADC_STACK_COUNT];
uint16_t ADCLine_2ResultMinNoOffset[ADC_STACK_COUNT];
#endif

uint16_t ADCStack1ResultMaxNoOffset[ADC_STACK_COUNT];
uint16_t ADCStack2ResultMaxNoOffset[ADC_STACK_COUNT];

#if 0
uint16_t ADCStack3ResultMaxNoOffset[ADC_STACK_COUNT];
uint16_t ADCDCLinkResultMaxNoOffset[ADC_STACK_COUNT];
uint16_t ADCLine_1ResultMaxNoOffset[ADC_STACK_COUNT];
uint16_t ADCLine_2ResultMaxNoOffset[ADC_STACK_COUNT];
#endif

uint16_t ADCStack1ResultAvg[ADC_STACK_COUNT];
uint16_t ADCStack2ResultAvg[ADC_STACK_COUNT];

#if 0
uint16_t ADCStack3ResultAvg[ADC_STACK_COUNT];
uint16_t ADCDCLinkResultAvg[ADC_STACK_COUNT];
uint16_t ADCLine_1ResultAvg[ADC_STACK_COUNT];
uint16_t ADCLine_2ResultAvg[ADC_STACK_COUNT];
#endif

uint16_t ADCStack1ResultAmplitude[ADC_STACK_COUNT];
uint16_t ADCStack2ResultAmplitude[ADC_STACK_COUNT];

#if 0
uint16_t ADCStack3ResultAmplitude[ADC_STACK_COUNT];
uint16_t ADCDCLinkResultAmplitude[ADC_STACK_COUNT];
uint16_t ADCLine_1ResultAmplitude[ADC_STACK_COUNT];
uint16_t ADCLine_2ResultAmplitude[ADC_STACK_COUNT];
#endif

MCLIB_sFilterAVGu16_t asADCStack1ResultAvg[ADC_STACK_COUNT];
MCLIB_sFilterAVGu16_t asADCStack2ResultAvg[ADC_STACK_COUNT];

#if 0
MCLIB_sFilterAVGu16_t asADCStack3ResultAvg[ADC_STACK_COUNT];
MCLIB_sFilterAVGu16_t asADCDCLinkResultAvg[ADC_STACK_COUNT];
MCLIB_sFilterAVGu16_t asADCLine_1ResultAvg[ADC_STACK_COUNT];
MCLIB_sFilterAVGu16_t asADCLine_2ResultAvg[ADC_STACK_COUNT];
#endif

uint16_t au16ADCStack1ResultAvgFilterSamplesCount[ADC_STACK_COUNT];
uint16_t au16ADCStack2ResultAvgFilterSamplesCount[ADC_STACK_COUNT];

#if 0
uint16_t au16ADCStack3ResultAvgFilterSamplesCount[ADC_STACK_COUNT];
uint16_t au16ADCDCLinkResultAvgFilterSamplesCount[ADC_STACK_COUNT];
uint16_t au16ADCLine_1ResultAvgFilterSamplesCount[ADC_STACK_COUNT];
uint16_t au16ADCLine_2ResultAvgFilterSamplesCount[ADC_STACK_COUNT];
#endif

#endif

bool bControlStack1Use3SensorFeedbacks = CONTROL_CFG_USE_3_SENSOR_FBK_STACK1_CONTROL; /* Flag Set Priority */
bool bControlStack1Use2SensorFeedbacks = 1; /* Flag Set Priority */
bool bControlStack1UseSensorsStack2 = 0;

bool bControlStack2Use2SensorFeedbacks = 1; /* to do implementation */
bool bControlStack2UseSensorsStack2 = 0; /* to do implementation */


bool bUseStartTestAuto = 1;

bool bSwitchMainContactors = 0;

bool bUseHeatManagementAuto = 0;
bool bUseHeatManagementAutoOff = 1;

bool bUseHeatExchangerFanControlAuto = 0;
bool bUseHeatExchangerPumpControlAuto = 0;
bool bUseSection1FanControlAuto = 0;
bool bUseSection2FanControlAuto = 0;


int16_t s16DutyMinFracCoef = MCLIB_s16Fractional(FRAC_AMPLITUDE_PWM_MIN_DUTY_COEF);
int16_t s16DutyMaxFracCoef = MCLIB_s16Fractional(FRAC_AMPLITUDE_PWM_MAX_DUTY_COEF);

uint16_t u16VoltageDCBusFbckEmulationAtStartRequest = AMP_VOLTAGE_FBCK_EMULATION_STARTUP_V;
uint16_t u16VoltageDCBusFbckEmulationRunningRequest = AMP_VOLTAGE_FBCK_EMULATION_V;


uint32_t u32AmpVoltageFbkEmulationRampUpVoltsPerPWMPeriod = MCLIB_u32FractionalQ_16_16(AMP_VOLTAGE_FBCK_EMULATION_RAMPUP_V_PER_PWM_PERIOD);
uint32_t u32AmpVoltageFbkEmulationRampDnVoltsPerPWMPeriod = MCLIB_u32FractionalQ_16_16(AMP_VOLTAGE_FBCK_EMULATION_RAMPDN_V_PER_PWM_PERIOD);

uint16_t u16ScaleModulationIndexSineWave = MCLIB_u16Fractional(MCLIB_SIN_AMP_MODULATION_INDEX);
uint16_t u16ScaleModulationIndexSVM = MCLIB_u16Fractional(MCLIB_SVM_AMP_MODULATION_INDEX);

uint16_t u16VoltageStartupRequest = RMS_VOLTAGE_STARTUP;
uint16_t u16PIControllerVoltageToCurrentMaxLimitRequest = MCLIB_u16Fractional(CURRENT_TO_VOLTAGE_MAX_LIMIT);
uint16_t u16PIControllerVoltageToCurrentMinLimitRequest = MCLIB_u16Fractional(CURRENT_TO_VOLTAGE_MIN_LIMIT);

uint16_t u16VoltageDCBusDeltaCalibrationRequest = VOLTAGE_OFFSET_MAX_DELTA_IN_CALIBRATION;

int16_t s16CFKBNominalCurrentModule = BOARDCFG_CFBK_MODULE_NOMINAL_AMPERES;     /* CFBK_MODULE_NOMINAL_AMPERES */

uint32_t u32CFKBNominalCurrentModuleBase;
uint32_t u32CFKBNominalCurrentModuleBaseAmperesToVoltMult;
uint32_t u32CFKBNominalCurrentModuleBaseAmperesToVolt;
uint32_t u32CFKBADCResultToAmperesMultBaseOneAmpere;
uint32_t u32CFKBADCResultToAmperesMultiplier;
uint32_t u32CFKBADCResultToDeciAmperesMultiplier;
uint32_t u32ADCResultToDeciAmperesMultiplierCFBK;


uint16_t u16SmpleCountADCAverageFilter = (LOOPTIME1_HZ / CONTROL_FIELD_SPEED_HZ);

bool bProcessAllPWMWithStack1Process = 1;
bool bProcessControlStack1 = 1;
bool bProcessControlStack2 = 0;

CSMON_eRecorderParameterMask_t CTRL_u8MaskStartRecorderRunModeEnter = CSMON_MASK_RECORDER_NONE;    /* do not start recorder by default */
//CSMON_eRecorderParameterMask_t CTRL_u8MaskStartRecorderRunModeEnter = CSMON_MASK_RECORDER_1;    /* start recorder 1 by default */
//CSMON_eRecorderParameterMask_t CTRL_u8MaskStartRecorderRunModeEnter = CSMON_MASK_RECORDERS_012;    /* start recorders 0 1 2 by default */


CSMON_eRecorderParameterMask_t CTRL_u8MaskSetupRecorderRunModeRequest = CSMON_MASK_RECORDER_NONE;    /* do not setup recorder by default */
//CSMON_eRecorderParameterMask_t CTRL_u8MaskSetupRecorderRunModeRequest = CSMON_MASK_RECORDER_1;    /* start recorder 1 by default */
//CSMON_eRecorderParameterMask_t CTRL_u8MaskSetupRecorderRunModeRequest = CSMON_MASK_RECORDERS_012;    /* start recorders 0 1 2 by default */

uint16_t CONTROL_u16PWM1FreqHz = LOOPTIME1_HZ;
uint16_t CONTROL_u16PWM2FreqHz = LOOPTIME2_HZ;







/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */


void ADC_vOffsetBufferInit(void);
void ADC_vReadADCData(void);
void vStackPrescalersInit(void);
void vStateRunSkipPulse(void);
void vCurrentVoltageFbkPrepare(void);

#if 0
typedef enum {
    CTRL_STA_INIT,
    CTRL_STA_FAULT_RESET,
    CTRL_STA_FAULT,
    CTRL_STA_READY,
    CTRL_STA_CALIB,
    CTRL_STA_RUN,
    CTRL_STA_COUNT
} eControlState_t;
#endif

void vStateInit(void);
void vStateFaultReset(void);
void vStateFault(void);
void vStateReady(void);
void vStateCalib(void);
void vStateRun(void);
void vStateRunDelay(void);


void vInitControlParameters (void);
void vInitBoardParameters (void);

void vRunInitialize(void);



/* *****************************************************************************
 * Variables using function pointers Definitions
 **************************************************************************** */
pfVoid_t apfControlState[CTRL_STA_COUNT] ={
    vStateInit,
    vStateFaultReset,
    vStateFault,
    vStateReady,
    vStateCalib,
    vStateRun,
    vStateRunDelay
};









/* *****************************************************************************
 * Function Definitions
 **************************************************************************** */




void vInitControlParameters (void)
{
    sControlParameter.sStack[STACK_1].s16FieldSpeed_dHz             = MCLIB_s16d(CONTROL_FIELD_SPEED_HZ);
    sControlParameter.sStack[STACK_1].s16FieldSpeedEmulation_dHz    = MCLIB_s16d(CONTROL_FIELD_SPEED_EMULATION_HZ);

    #if BOARDCFG_STACK_COUNT >= 2
    sControlParameter.sStack[STACK_2].s16FieldSpeed_dHz             = MCLIB_s16d(CONTROL_FIELD_SPEED_HZ);
    sControlParameter.sStack[STACK_2].s16FieldSpeedEmulation_dHz    = MCLIB_s16d(CONTROL_FIELD_SPEED_EMULATION_HZ);
    #endif

    #if BOARDCFG_STACK_COUNT >= 3
    sControlParameter.sStack[STACK_3].s16FieldSpeed_dHz             = MCLIB_s16d(CONTROL_FIELD_SPEED_HZ);
    sControlParameter.sStack[STACK_3].s16FieldSpeedEmulation_dHz    = MCLIB_s16d(CONTROL_FIELD_SPEED_EMULATION_HZ);
    #endif

    sControlParameter.s16TripTemperature_dDeg               = MCLIB_s16d(CONTROL_TRIP_TEMPERATURE_DEG);
    sControlParameter.u16VoltageRequestDCLink_dV            = MCLIB_s16d(CONTROL_VOLTAGE_DCLINK_REQUEST_V);
}


void vInitBoardParameters (void)
{
    sBoardParameter.u32ADCReferenceVoltage = MCLIB_u32FractionalQ_16_16(BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS);
    sBoardParameter.u32DACReferenceVoltage = MCLIB_u32FractionalQ_16_16(BOARDCFG_DAC_REFERENCE_VOLTAGE_VOLTS);
}


int16_t CONTROL_s16GetFieldFrequencyStack1_dHz (void)
{
    return sControlParameter.sStack[STACK_1].s16FieldSpeed_dHz;
}
void CONTROL_s16SetFieldFrequencyStack1_dHz(int16_t s16Input)
{
    sControlParameter.sStack[STACK_1].s16FieldSpeed_dHz = s16Input;
}
uint32_t CONTROL_u32GetAddrFieldFrequencyStack1_dHz (void)
{
    return (uint32_t)(HMDRV_ADDR_SIZE)&sControlParameter.sStack[STACK_1].s16FieldSpeed_dHz;
}


uint32_t CONTROL_u32GetAddrVoltageRequestDCLink_dV (void)
{
    return (uint32_t)(HMDRV_ADDR_SIZE)&sControlParameter.u16VoltageRequestDCLink_dV;
}



int16_t CONTROL_s16GetTripTemperature_dDeg (void)
{
    return sControlParameter.s16TripTemperature_dDeg;
}
void CONTROL_s16SetTripTemperature_dDeg(int16_t s16Input)
{
    sControlParameter.s16TripTemperature_dDeg = s16Input;
}








void CONTROL_vRunRequest(bool bInput) 
{
    sControlAccess.bStart = bInput;
}



void CONTROL_vReset(void) 
{
    sControlData.s16DutyLimitMinFrac = MCLIB_s16Fractional(1.0) - sControlData.s16DutyMaxFrac;
    //sControlData.s16DutyLimitMinFrac++; /* fix 0x7FFF + 1 */
    if (sControlData.s16DutyLimitMinFrac < sControlData.s16DutyMinFrac) {
        sControlData.s16DutyLimitMinFrac = sControlData.s16DutyMinFrac;
    }
    sControlData.s16DutyLimitMinFrac <<= 1;
    sControlData.s16DutyLimitMaxFrac = MCLIB_s16Fractional(1.0) - sControlData.s16DutyLimitMinFrac;
    //sControlData.s16DutyLimitMaxFrac++; /* fix 0x7FFF + 1 */


}

void CONTROL_vInitOnce(void) 
{
    
    vInitControlParameters();
    vInitBoardParameters();

    sControlData.sStack[STACK_1].u32DummyEnd = 0x12345678;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].u32DummyEnd = 0x55667788;
    #endif
    
    sControlAccess.bEmulateVoltageFbck              = CONTROL_CFG_USE_VOLTAGE_FBCK_EMULATION;
    sControlAccess.bEmulateCurrentFbck              = CONTROL_CFG_USE_CURRENT_FBCK_EMULATION;
    sControlAccess.bCalculateRMSABC                 = CONTROL_CFG_USE_CALC_RMS_ABC;
    sControlAccess.bCalculateCFBKRMSABC             = CONTROL_CFG_USE_CALC_CFBK_DCLI_RMS_ABC;
    sControlAccess.bCalculateRMSDCBus               = CONTROL_CFG_USE_CALC_RMS_DC_LINK;
    sControlAccess.bUseControllerPI                 = CONTROL_CFG_USE_PI_CONTROLLER;
    sControlAccess.bUseWrongVoltageOffsetDetection  = CONTROL_CFG_USE_WRONG_OFFSET_VOLTAGE_DETECTION;

    sControlData.u16VoltageRunFilterBits = VOLTAGE_RUN_FILTER_DEPTH_BITS;
    sControlData.u16CurrentRunFilterBits = CURRENT_RUN_FILTER_DEPTH_BITS;
    sControlData.u16OffsetCalcFilterBits = ADC_OFFSET_FILTER_DEPTH_BITS;
    sControlData.u16CFBKRunFilterBits = CURRENT_RUN_FILTER_DEPTH_BITS;
    sControlData.u16RunRmsFilterBits = RUN_RMS_FILTER_SAMPLES;

    s32PropCoefBase = MCLIB_s32FractionalQ_15_16(CURRENT_TO_VOLTAGE_PROP_COEF);
    s32IntgCoefBase = MCLIB_s32FractionalQ_15_16(CURRENT_TO_VOLTAGE_INTG_COEF);

    u32PropCoefBase = MCLIB_u32FractionalQ_16_16(CURRENT_TO_VOLTAGE_PROP_COEF);
    u32IntgCoefBase = MCLIB_u32FractionalQ_16_16(CURRENT_TO_VOLTAGE_INTG_COEF);

    sControlData.sStack[STACK_1].u16ControlPrescaler = 1;
    #if BOARDCFG_STACK_COUNT >= 2
    	sControlData.sStack[STACK_2].u16ControlPrescaler = 1;
    #endif

    #if BOARDCFG_USE_TSC_DSPIC
		if (bProcessControlStack1 && bProcessControlStack2)
		{
			sControlData.sStack[STACK_1].u16ControlPrescaler = 2;
			#if BOARDCFG_STACK_COUNT >= 2
			sControlData.sStack[STACK_2].u16ControlPrescaler = 2;
			#endif
		}
    #endif

    sControlConfig.u32ADCScaleMaxValue = ((uint32_t)1 << BOARDCFG_ADC_RESULT_BITS);
    sControlConfig.u32DACScaleMaxValue = ((uint32_t)1 << BOARDCFG_DAC_RESULT_BITS);

    sControlConfig.u32CurrentSensingGateDriverVoltageBaseMultiplier = MCLIB_u32FractionalQ_16_16(BOARDCFG_GATE_DRIVER_CURRENT_SENSOR_VOLTAGE_MULT);  /* GATE_DRIVER_CURRENT_SENSOR_VOLTAGE_MULT */

}

void CONTROL_vCFBKCurrentMultiplierCalc(void)
{
    u32CFKBNominalCurrentModuleBase = MCLIB_u32FractionalQ_16_16(s16CFKBNominalCurrentModule);

    u32CFKBNominalCurrentModuleBaseAmperesToVoltMult = MCLIB_u32FractionalQ_16_16(BOARDCFG_CFBK_AMPERES_PER_VOLT_BASE_MULT);     /* CFBK_AMPERES_PER_VOLT_BASE_MULT */
    u32CFKBNominalCurrentModuleBaseAmperesToVolt = MCLIB_u32Mul_u32u32(u32CFKBNominalCurrentModuleBase, u32CFKBNominalCurrentModuleBaseAmperesToVoltMult);
    u32CFKBADCResultToAmperesMultBaseOneAmpere = MCLIB_u32FractionalQ_16_16(BOARDCFG_ADC_RESULT_TO_CFBK_MULT_BASE_1_AMP); /* ADC_RESULT_TO_AMP_CFBK_MULT_BASE_1_AMPERE */

    u32CFKBADCResultToAmperesMultiplier = MCLIB_u32Mul_u32u32(u32CFKBNominalCurrentModuleBaseAmperesToVolt, u32CFKBADCResultToAmperesMultBaseOneAmpere);
    u32CFKBADCResultToDeciAmperesMultiplier = MCLIB_u32Mul_u32u32(u32CFKBADCResultToAmperesMultiplier, MCLIB_u32FractionalQ_16_16(10));

    sControlConfig.u16ADCResultToDeciAmperesMultiplierCFBK = (uint16_t)u32CFKBADCResultToDeciAmperesMultiplier;

    #if 0  
    sControlConfig.u32ADCResultToDeciAmperesMultiplierCFBK = ADCResultToDeciAmperesMultiplierCFBK_Q_0_16;
    /* CFBK MEASUREMENT */
    #define ADCResultToDeciAmperesMultiplierCFBK MCLIB_u32FractionalQ_16_16(ADC_RESULT_TO_AMP_CFBK_MULT * 10)
    #define ADC_RESULT_TO_AMP_CFBK_MULT     ( (CFBK_AMPERES_PER_VOLT_DEF * ANALOG_CONDITIONING_CONTROL_BOARD) * ((BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS)/((uint32_t)1 << BOARDCFG_ADC_RESULT_BITS)) )   
    //#define CFBK_AMPERES_PER_VOLT_DEF           (CFBK_AMPERES_PER_VOLT_LSC)
    #define CFBK_AMPERES_PER_VOLT_DEF           CFBK_AMPERES_PER_VOLT_BASE(CFBK_MODULE_NOMINAL_AMPERES)
    #define CFBK_AMPERES_PER_VOLT_BASE_MULT     (125.0F / 1000.0)             
    #define CFBK_AMPERES_PER_VOLT_BASE(x)       (x / 8)             /* same as above */     
    #define CFBK_MODULE_NOMINAL_AMPERES         1000   
    #endif

}




void CONTROL_vInitMultipliers(void)
{
    uint16_t u16Index;

    if (sControlConfig.u32CurrentSensingGateDriverVoltageBaseMultiplier == u32CurrentSensingGateDriverVoltageBaseMultiplierOld)
    {
        return;
    }

    sControlConfig.u32ADCResultToVoltageOnConnectorMultiplier =
            ADCResultToVoltageOnConnectorMultiplier;
    sControlConfig.u32ADCVoltageOnConnectorToAmperesMultiplier =
            ADCVoltageOnConnectorToAmperesMultiplier;
    sControlConfig.u32ADCVoltageOnConnectorToDeciAmperesMultiplier =
            ADCVoltageOnConnectorToDeciAmperesMultiplier;

    sControlConfig.u16ADCResultToAmperesFixShiftControl = CURRENT_FEEDBACK_FIX_MULT_SHIFT_CONTROL;
    sControlConfig.u16ADCResultToAmperesFixShiftStack1  = CURRENT_FEEDBACK_FIX_MULT_SHIFT_STACK1 ;
    sControlConfig.u16ADCResultToAmperesFixShiftStack2  = CURRENT_FEEDBACK_FIX_MULT_SHIFT_STACK2 ;
    sControlConfig.u16ADCResultToAmperesFixShiftStack3  = CURRENT_FEEDBACK_FIX_MULT_SHIFT_STACK3 ;


    sControlConfig.u32DeciAmperesToADCResultMultiplierCheck  = DeciAmperesToADCResultMultiplier;
    sControlConfig.u32DeciAmperesToADCResultMultiplier       = (sControlConfig.u32DeciAmperesToADCResultMultiplierCheck << sControlConfig.u16ADCResultToAmperesFixShiftControl);//???u32DeciAmperesToADCResultMultiplier to be checked
    sControlConfig.u32DeciAmperesToADCResultMultiplierVisual = (sControlConfig.u32DeciAmperesToADCResultMultiplierCheck >> sControlConfig.u16ADCResultToAmperesFixShiftControl);//???u32DeciAmperesToADCResultMultiplier to be checked
    sControlConfig.u32DeciAmperesToADCResultMultiplierRequest= (sControlConfig.u32DeciAmperesToADCResultMultiplierCheck >> sControlConfig.u16ADCResultToAmperesFixShiftControl);//???u32DeciAmperesToADCResultMultiplier to be checked



    sControlConfig.u32ADCResultToDeciAmperesMultiplierCheck  = ADCResultToDeciAmperesMultiplier;


    sControlConfig.u32ADCResultToDeciAmperesMultiplier       = (sControlConfig.u32ADCResultToDeciAmperesMultiplierCheck >> sControlConfig.u16ADCResultToAmperesFixShiftControl);
    u16Index = 0;
    while ((uint32_t)(sControlConfig.u32ADCResultToDeciAmperesMultiplier >> u16Index) > (uint32_t)UINT16_MAX)
    {
        u16Index++;
    }
    sControlConfig.u16ADCResultToDeciAmperesMult       = (uint16_t)(sControlConfig.u32ADCResultToDeciAmperesMultiplier >> u16Index);
    sControlConfig.u16ADCResultToDeciAmperesShft = u16Index;



    sControlConfig.u32ADCResultToDeciAmperesMultiplierVisual = (sControlConfig.u32ADCResultToDeciAmperesMultiplierCheck << sControlConfig.u16ADCResultToAmperesFixShiftControl);
    u16Index = 0;
    while ((uint32_t)(sControlConfig.u32ADCResultToDeciAmperesMultiplierVisual >> u16Index) > (uint32_t)UINT16_MAX)
    {
        u16Index++;
    }
    sControlConfig.u16ADCResultToDeciAmperesMultVisual = (uint16_t)(sControlConfig.u32ADCResultToDeciAmperesMultiplierVisual >> u16Index);
    sControlConfig.u16ADCResultToDeciAmperesShftVisual = u16Index;




    sControlConfig.u32ADCResultToDeciAmperesMultiplierStack1 = (sControlConfig.u32ADCResultToDeciAmperesMultiplierCheck >> sControlConfig.u16ADCResultToAmperesFixShiftStack1);
    sControlConfig.u32ADCResultToDeciAmperesMultiplierStack2 = (sControlConfig.u32ADCResultToDeciAmperesMultiplierCheck >> sControlConfig.u16ADCResultToAmperesFixShiftStack2);
    sControlConfig.u32ADCResultToDeciAmperesMultiplierStack3 = (sControlConfig.u32ADCResultToDeciAmperesMultiplierCheck >> sControlConfig.u16ADCResultToAmperesFixShiftStack3);

    sControlConfig.u32AmperesToADCResultMultiplierCheck     = AmperesToADCResultMultiplier;
    sControlConfig.u32AmperesToADCResultMultiplier          = (sControlConfig.u32AmperesToADCResultMultiplierCheck << sControlConfig.u16ADCResultToAmperesFixShiftControl);//???u32AmperesToADCResultMultiplier to be checked
    sControlConfig.u32AmperesToADCResultMultiplierVisual    = (sControlConfig.u32AmperesToADCResultMultiplierCheck >> sControlConfig.u16ADCResultToAmperesFixShiftControl);//???u32AmperesToADCResultMultiplier to be checked
    sControlConfig.u32AmperesToADCResultMultiplierRequest   = (sControlConfig.u32AmperesToADCResultMultiplierCheck >> sControlConfig.u16ADCResultToAmperesFixShiftControl);//???u32AmperesToADCResultMultiplier to be checked






    sControlConfig.u16ADCResultToDeciVoltageMultiplier =
            ADCResultToDeciVoltageMultiplier;

    sControlConfig.u16ADCResultToDeciDegreesMultiplierTFBK =
            ADCResultToDeciDegreesMultiplierTFBK;
    sControlConfig.s16ADCResultToDeciDegreesOffsetTFBK =
            ADCResultToDeciDegreesOffsetTFBK;


    u32ADCResultToDeciAmperesMultiplierCFBK =
            ADCResultToDeciAmperesMultiplierCFBK;
    sControlConfig.u16ADCResultToDeciAmperesMultiplierCFBK =
            ADCResultToDeciAmperesMultiplierCFBK_Q_0_16;
    CONTROL_vCFBKCurrentMultiplierCalc();

    sControlConfig.u32DACResultToDeciAmperesMultiplier =
            DACResultToDeciAmperesMultiplier;
    sControlConfig.u32DACResultToDeciVoltageMultiplier =
            DACResultToDeciVoltageMultiplier;
    sControlConfig.u32DACResultToFromDeciAmperesOffset =
            DACResultToFromDeciAmperesOffset;
    sControlConfig.u32DACResultToFromDeciVoltageOffset =
            DACResultToFromDeciVoltageOffset;
    sControlConfig.s32DACDeciAmperesOffset =
            DACDeciAmperesOffset;
    sControlConfig.s32DACDeciVoltageOffset =
            DACDeciVoltageOffset;
    sControlConfig.u16SpeedToDeciHzMultiplier =
            SpeedToDeciHzMultiplier;


    sControlConfig.u32ADCResultToAmperesMultiplier =        /* not Used in code only perhaps in freemaster */
            ADCResultToAmperesMultiplier;

    sControlConfig.u32ADCResultToVoltageMultiplier =
            ADCResultToVoltageMultiplier;
    sControlConfig.u32DACResultToAmperesMultiplier =
            DACResultToAmperesMultiplier;
    sControlConfig.u32DACResultToVoltageMultiplier =
            DACResultToVoltageMultiplier;
    sControlConfig.u32DACResultToFromAmperesOffset = 
            DACResultToFromAmperesOffset;
    sControlConfig.u32DACResultToFromVoltageOffset =
            DACResultToFromVoltageOffset;
    sControlConfig.u32SpeedToHzMultiplier =
            SpeedToHzMultiplier;

    /* ADC To DAC */
    sControlConfig.u32ADCResultToDACResultAmperesMultiplier =
            ADCResultToDACResultAmperesMultiplier;
    sControlConfig.u32ADCResultToDACResultVoltageMultiplier =
            ADCResultToDACResultVoltageMultiplier;


    sControlConfig.u32DeciVoltsToADCResultMultiplier =
            DeciVoltsToADCResultMultiplier;
    sControlConfig.u32DeciAmperesToDACResultMultiplier =
            DeciAmperesToDACResultMultiplier;
    sControlConfig.u32DeciVoltsToDACResultMultiplier =
            DeciVoltageToDACResultMultiplier;
    sControlConfig.u32DeciHzToSpeedMultiplier =
            DeciHzToSpeedMultiplier;

    sControlConfig.u32VoltsToADCResultMultiplier =
            VoltsToADCResultMultiplier;
    sControlConfig.u32AmperesToDACResultMultiplier =
            AmperesToDACResultMultiplier;
    sControlConfig.u32VoltsToDACResultMultiplier =
            VoltageToDACResultMultiplier;
    sControlConfig.u32HzToSpeedMultiplier =
            HzToSpeedMultiplier;

    sControlConfig.u32DeadTimeMCUNanoSecondsToTicksMultiplier =
            DeadTimeMCUNanoSecondsToTicksMultiplier;
    sControlConfig.u32DeadTimeMCUTicksToNanoSecondsMultiplier =
            DeadTimeMCUTicksToNanoSecondsMultiplier;

    sControlConfig.u32DeadTimeNanoSecondsToTicksMultiplier =
            DeadTimeNanoSecondsToTicksMultiplier;
    sControlConfig.u32DeadTimeTicksToNanoSecondsMultiplier =
            DeadTimeTicksToNanoSecondsMultiplier;

    sControlConfig.u32MinPulseNanoSecondsToTicksMultiplier =
            MinPulseNanoSecondsToTicksMultiplier;
    sControlConfig.u32MinPulseTicksToNanoSecondsMultiplier =
            MinPulseTicksToNanoSecondsMultiplier;

    #if BOARDCFG_USE_TSC_DSPIC == 0
    /* ADC Scale To Board Connector Multipliers  */
    sControlConfig.u32ADCScaleToIFBKInputConnectorVoltageMultiplierCheck = MCLIB_u32FractionalQ_16_16(ADC_SCALE_TO_AMP_IFBK_INPUT_VOLTAGE_MULT);
    sControlConfig.u32ADCResultToIFBKInputConnectorVoltageMultiplier       = (sControlConfig.u32ADCScaleToIFBKInputConnectorVoltageMultiplierCheck << sControlConfig.u16ADCResultToAmperesFixShiftControl);
    sControlConfig.u32ADCScaleToDCLVInputConnectorVoltageMultiplier = MCLIB_u32FractionalQ_16_16(ADC_SCALE_TO_VOLTS_DC_INPUT_VOLTAGE_MULT);
    sControlConfig.u32ADCScaleToLN_VInputConnectorVoltageMultiplier = MCLIB_u32FractionalQ_16_16(ADC_SCALE_TO_VOLTS_LN_INPUT_VOLTAGE_MULT);
    sControlConfig.u32ADCScaleToPRESInputConnectorVoltageMultiplier = MCLIB_u32FractionalQ_16_16(ADC_SCALE_TO_PRESSURE_INPUT_VOLTAGE_MULT);
    sControlConfig.u32ADCScaleToDCLIInputConnectorVoltageMultiplier = MCLIB_u32FractionalQ_16_16(ADC_SCALE_TO_AMP_DCLI_INPUT_VOLTAGE_MULT);
    sControlConfig.u32ADCScaleToTH2OInputConnectorVoltageMultiplier = MCLIB_u32FractionalQ_16_16(ADC_SCALE_TO_TEMP_H2O_INPUT_VOLTAGE_MULT);
    sControlConfig.u32ADCScaleToTAIRInputConnectorVoltageMultiplier = MCLIB_u32FractionalQ_16_16(ADC_SCALE_TO_TEMP_AIR_INPUT_VOLTAGE_MULT);

    /* ADC Result To Board Connector Multipliers  */
    sControlConfig.u32ADCResultToIFBKInputConnectorVoltageMultiplierCheck = MCLIB_u32FractionalQ_16_16(BOARDCFG_ADC_RESULT_TO_10V_IFBK_MULT);
    sControlConfig.u32ADCResultToIFBKInputConnectorVoltageMultiplier       = (sControlConfig.u32ADCResultToIFBKInputConnectorVoltageMultiplier << sControlConfig.u16ADCResultToAmperesFixShiftControl);

    sControlConfig.u32ADCResultToDCLVInputConnectorVoltageMultiplier = MCLIB_u32FractionalQ_16_16(ADC_RESULT_TO_VOLTS_DC_INPUT_VOLTAGE_MULT);
    sControlConfig.u32ADCResultToLN_VInputConnectorVoltageMultiplier = MCLIB_u32FractionalQ_16_16(ADC_RESULT_TO_VOLTS_LN_INPUT_VOLTAGE_MULT);
    sControlConfig.u32ADCResultToPRESInputConnectorVoltageMultiplier = MCLIB_u32FractionalQ_16_16(ADC_RESULT_TO_PRESSURE_INPUT_VOLTAGE_MULT);
    sControlConfig.u32ADCResultToDCLIInputConnectorVoltageMultiplier = MCLIB_u32FractionalQ_16_16(ADC_RESULT_TO_AMP_DCLI_INPUT_VOLTAGE_MULT);
    sControlConfig.u32ADCResultToTH2OInputConnectorVoltageMultiplier = MCLIB_u32FractionalQ_16_16(ADC_RESULT_TO_TEMP_H2O_INPUT_VOLTAGE_MULT);
    sControlConfig.u32ADCResultToTAIRInputConnectorVoltageMultiplier = MCLIB_u32FractionalQ_16_16(ADC_RESULT_TO_TEMP_AIR_INPUT_VOLTAGE_MULT);

    /* ADC Scale To Real Values Multipliers  */
    sControlConfig.u32ADCScaleToIFBKRealValueMultiplierCheck    = MCLIB_u32FractionalQ_16_16(ADC_SCALE_TO_AMP_IFBK_MULT);
    sControlConfig.u32ADCScaleToIFBKRealValueMultiplier         = (sControlConfig.u32ADCScaleToIFBKRealValueMultiplierCheck << sControlConfig.u16ADCResultToAmperesFixShiftControl);
    //sControlConfig.u32ADCScaleToIFBKRealValueMultiplierVisual = (sControlConfig.u32ADCScaleToIFBKRealValueMultiplierCheck << sControlConfig.u16ADCResultToAmperesFixShiftControl);
    //sControlConfig.u32ADCScaleToIFBKRealValueMultiplierStack1 = (sControlConfig.u32ADCScaleToIFBKRealValueMultiplierCheck << sControlConfig.u16ADCResultToAmperesFixShiftStack1);
    //sControlConfig.u32ADCScaleToIFBKRealValueMultiplierStack2 = (sControlConfig.u32ADCScaleToIFBKRealValueMultiplierCheck << sControlConfig.u16ADCResultToAmperesFixShiftStack2);
    //sControlConfig.u32ADCScaleToIFBKRealValueMultiplierStack3 = (sControlConfig.u32ADCScaleToIFBKRealValueMultiplierCheck << sControlConfig.u16ADCResultToAmperesFixShiftStack3);

    sControlConfig.u32ADCScaleToDCLVRealValueMultiplier = MCLIB_u32FractionalQ_16_16(ADC_SCALE_TO_VOLTS_DC_MULT);
    sControlConfig.u32ADCScaleToLN_VRealValueMultiplier = MCLIB_u32FractionalQ_16_16(ADC_SCALE_TO_VOLTS_LN_MULT);
    sControlConfig.u32ADCScaleToPRESRealValueMultiplier = MCLIB_u32FractionalQ_16_16(ADC_SCALE_TO_PRESSURE_MULT);
    sControlConfig.u32ADCScaleToDCLIRealValueMultiplier = MCLIB_u32FractionalQ_16_16(ADC_SCALE_TO_AMP_DCLI_MULT);
    sControlConfig.s32ADCScaleToTH2ORealValueMultiplier = MCLIB_s32FractionalQ_15_16(ADC_SCALE_TO_TEMP_H2O_MULT);
    sControlConfig.s32ADCScaleToTAIRRealValueMultiplier = MCLIB_s32FractionalQ_15_16(ADC_SCALE_TO_TEMP_AIR_MULT);




    sControlConfig.s32ADCScaleToTH2ORealValueOffset = MCLIB_s32FractionalQ_15_16(ADC_SCALE_TO_TEMP_H2O_OFFS);
    sControlConfig.s32ADCScaleToTAIRRealValueOffset = MCLIB_s32FractionalQ_15_16(ADC_SCALE_TO_TEMP_AIR_OFFS);

    sControlConfig.s32ADCScaleTH2ORealValueOffset = MCLIB_s32FractionalQ_15_16(VOLTAGE_ON_TEMP_LIQUID_INPUT_TO_DEG_OFFS);
    sControlConfig.s32ADCScaleTAIRRealValueOffset = MCLIB_s32FractionalQ_15_16(VOLTAGE_ON_TEMP_AIRFLW_INPUT_TO_DEG_OFFS);


    sControlConfig.s32ADCScaleTH2ORealValueMult = MCLIB_s32FractionalQ_15_16(VOLTAGE_ON_TEMP_LIQUID_INPUT_TO_DEG_MULT);
    sControlConfig.s32ADCScaleTAIRRealValueMult = MCLIB_s32FractionalQ_15_16(VOLTAGE_ON_TEMP_AIRFLW_INPUT_TO_DEG_MULT);

    /* ADC Result To Real Values Multipliers  */
    sControlConfig.u32ADCResultToIFBKRealValueMultiplier = MCLIB_u32FractionalQ_16_16(ADC_RESULT_TO_AMP_IFBK_MULT);
    sControlConfig.u32ADCResultToDCLVRealValueMultiplier = MCLIB_u32FractionalQ_16_16(BOARDCFG_ADC_RESULT_TO_DC_LINK_MULT);
    sControlConfig.u32ADCResultToLN_VRealValueMultiplier = MCLIB_u32FractionalQ_16_16(ADC_RESULT_TO_VOLTS_LN_MULT);
    sControlConfig.u32ADCResultToPRESRealValueMultiplier = MCLIB_u32FractionalQ_16_16(ADC_RESULT_TO_PRESSURE_MULT);
    sControlConfig.u32ADCResultToDCLIRealValueMultiplier = MCLIB_u32FractionalQ_16_16(ADC_RESULT_TO_AMP_DCLI_MULT);
    sControlConfig.s32ADCResultToTH2ORealValueMultiplier = MCLIB_s32Fractional(ADC_RESULT_TO_TEMP_H2O_MULT);
    sControlConfig.s32ADCResultToTAIRRealValueMultiplier = MCLIB_s32Fractional(ADC_RESULT_TO_TEMP_AIR_MULT);

    sControlConfig.s32ADCResultToTH2ORealValueDivider = MCLIB_s32FractionalQ_15_16(1.0 / (ADC_RESULT_TO_TEMP_H2O_MULT));
    sControlConfig.s32ADCResultToTAIRRealValueDivider = MCLIB_s32FractionalQ_15_16(1.0 / (ADC_RESULT_TO_TEMP_AIR_MULT));

    sControlConfig.s32ADCResultToTH2ORealValueOffset = MCLIB_s32FractionalQ_15_16(ADC_RESULT_TO_TEMP_H2O_OFFS);
    sControlConfig.s32ADCResultToTAIRRealValueOffset = MCLIB_s32FractionalQ_15_16(ADC_RESULT_TO_TEMP_AIR_OFFS);





    /* DAC Scale To Board Connector Multipliers  */
    sControlConfig.u32DACScaleToIFBKInputConnectorVoltageMultiplier = MCLIB_u32FractionalQ_16_16(DAC_SCALE_TO_AMP_IFBK_INPUT_VOLTAGE_MULT);
    sControlConfig.u32DACScaleToDCLVInputConnectorVoltageMultiplier = MCLIB_u32FractionalQ_16_16(DAC_SCALE_TO_VOLTS_DC_INPUT_VOLTAGE_MULT);
    sControlConfig.u32DACScaleToDCLIInputConnectorVoltageMultiplier = MCLIB_u32FractionalQ_16_16(DAC_SCALE_TO_AMP_DCLI_INPUT_VOLTAGE_MULT);

    /* DAC Result To Board Connector Multipliers  */
    sControlConfig.u32DACResultToIFBKInputConnectorVoltageMultiplier = MCLIB_u32FractionalQ_16_16(DAC_RESULT_TO_AMP_IFBK_INPUT_VOLTAGE_MULT);
    sControlConfig.u32DACResultToDCLVInputConnectorVoltageMultiplier = MCLIB_u32FractionalQ_16_16(DAC_RESULT_TO_VOLTS_DC_INPUT_VOLTAGE_MULT);
    sControlConfig.u32DACResultToDCLIInputConnectorVoltageMultiplier = MCLIB_u32FractionalQ_16_16(DAC_RESULT_TO_AMP_DCLI_INPUT_VOLTAGE_MULT);

    /* DAC Scale To Real Values Multipliers  */
    sControlConfig.u32DACScaleToIFBKRealValueMultiplier = MCLIB_u32FractionalQ_16_16(DAC_SCALE_TO_AMP_IFBK_MULT);
    sControlConfig.u32DACScaleToDCLVRealValueMultiplier = MCLIB_u32FractionalQ_16_16(DAC_SCALE_TO_VOLTS_DC_MULT);
    sControlConfig.u32DACScaleToDCLIRealValueMultiplier = MCLIB_u32FractionalQ_16_16(DAC_SCALE_TO_AMP_DCLI_MULT);

    /* DAC Result To Real Values Multipliers  */
    sControlConfig.u32DACResultToIFBKRealValueMultiplier = MCLIB_u32FractionalQ_16_16(DAC_RESULT_TO_AMP_IFBK_MULT);
    sControlConfig.u32DACResultToDCLVRealValueMultiplier = MCLIB_u32FractionalQ_16_16(DAC_RESULT_TO_VOLTS_DC_MULT);
    sControlConfig.u32DACResultToDCLIRealValueMultiplier = MCLIB_u32FractionalQ_16_16(DAC_RESULT_TO_AMP_DCLI_MULT);
    #endif

    
    u32CurrentSensingGateDriverVoltageBaseMultiplierOld = sControlConfig.u32CurrentSensingGateDriverVoltageBaseMultiplier;



    sControlLimit.s16CurrentRequestMax = MCLIB_s16Mul_s16u32(sControlLimitReal.s16CurrentRequestMax, sControlConfig.u32AmperesToADCResultMultiplierRequest);

    sControlLimit.s16FieldFrequencyRequestMax = MCLIB_s16Mul_s16u32(sControlLimitReal.s16FieldFrequencyRequestMax, sControlConfig.u32HzToSpeedMultiplier);
    sControlLimit.s16FieldFrequencyRequestMin = MCLIB_s16Mul_s16u32(sControlLimitReal.s16FieldFrequencyRequestMin, sControlConfig.u32HzToSpeedMultiplier);

    sControlLimit.s16PWMFrequencyRequestMax = sControlLimitReal.s16PWMFrequencyRequestMax;
    sControlLimit.s16PWMFrequencyRequestMin = sControlLimitReal.s16PWMFrequencyRequestMin;



}

uint16_t CONTROL_u16LimitPWMFrequency(uint16_t u16Input)
{
    uint16_t u16Result = u16Input;
    /* Request Limitation */
    MCLIB_mValueLimitWithMaxMin(u16Result, sControlLimit.s16PWMFrequencyRequestMax, sControlLimit.s16PWMFrequencyRequestMin);
    return u16Result;
}




void CONTROL_vInit(void)
{
    //PWM_vOutputDisable(); PWM Initialization Disables PWM

    #if BOARDCFG_USE_FPGA
    FPGA_COMMON_vResetRelease();
    #endif

    #if BOARDCFG_USE_FPGA_TRIP
    FPGA_TRIP_vClearTrip(); /* consider create fault module */
    #endif
    
    #if BOARDCFG_USE_DIO
    DIO_vEnablePinRequestDeny(DIO_PIN_EN_1); /* EN1 is used for pwm enable/disable -  Prevent pin from update in main loop  */
    #endif

    CONTROL_vInitMultipliers();

    #if !defined(BOARDCFG_USE_MCC_ADC) && (BOARDCFG_USE_OLD_NON_MCC_ADC)
    sControlData.u16PeriodPWM = u16LoopTime1Tcy;
    #else
    sControlData.u16PeriodPWM = LOOPTIME1_TCY;
    #endif
    
    #if !defined(BOARDCFG_USE_MCC_ADC) && (BOARDCFG_USE_OLD_NON_MCC_ADC)
    sControlData.u16DutyMin = PWM_u16MinDutyTcy;
    sControlData.u16DutyMax = PWM_u16MaxDutyTcy;
    #else
    sControlData.u16DutyMin = MCLIB_u16Mul_u16u16Std(MCLIB_u16Fractional(PWM_MIN_DUTY_COEF), LOOPTIME1_TCY);
    sControlData.u16DutyMax = MCLIB_u16Mul_u16u16Std(MCLIB_u16Fractional(PWM_MAX_DUTY_COEF), LOOPTIME1_TCY);
    #endif
    
    sControlData.s16DutyMinFrac = s16DutyMinFracCoef;
    sControlData.s16DutyMaxFrac = s16DutyMaxFracCoef;

    /* Request Limitation */
    #if !defined(BOARDCFG_USE_MCC_ADC) && (BOARDCFG_USE_OLD_NON_MCC_ADC)
    CONTROL_u16PWM1FreqHz = ADCPWM_u16GetPWM1FreqHz();
    CONTROL_u16PWM2FreqHz = ADCPWM_u16GetPWM2FreqHz();
    #endif

    MCLIB_mValueLimitWithMaxMin(CONTROL_u16PWM1FreqHz, sControlLimit.s16PWMFrequencyRequestMax, sControlLimit.s16PWMFrequencyRequestMin);
    MCLIB_mValueLimitWithMaxMin(CONTROL_u16PWM2FreqHz, sControlLimit.s16PWMFrequencyRequestMax, sControlLimit.s16PWMFrequencyRequestMin);


    u16SmpleCountADCAverageFilter = MCLIB_u16DivSat_u32u16 (MCLIB_rToDeci((uint32_t)CONTROL_u16PWM1FreqHz), (uint16_t)sControlParameter.sStack[STACK_1].s16FieldSpeed_dHz);
    
    #if BOARDCFG_USE_INFO_CALC_MIN_MAX_AVG
    au16ADCStack1ResultAvgFilterSamplesCount[ADC_STACK_CURRENT_L1] = u16SmpleCountADCAverageFilter;
    au16ADCStack1ResultAvgFilterSamplesCount[ADC_STACK_CURRENT_L2] = u16SmpleCountADCAverageFilter;
    au16ADCStack1ResultAvgFilterSamplesCount[ADC_STACK_CURRENT_L3] = u16SmpleCountADCAverageFilter;
    au16ADCStack1ResultAvgFilterSamplesCount[ADC_STACK_DC_VOLTAGE] = u16SmpleCountADCAverageFilter;
    
    au16ADCStack2ResultAvgFilterSamplesCount[ADC_STACK_CURRENT_L1] = u16SmpleCountADCAverageFilter;
    au16ADCStack2ResultAvgFilterSamplesCount[ADC_STACK_CURRENT_L2] = u16SmpleCountADCAverageFilter;
    au16ADCStack2ResultAvgFilterSamplesCount[ADC_STACK_CURRENT_L3] = u16SmpleCountADCAverageFilter;
    au16ADCStack2ResultAvgFilterSamplesCount[ADC_STACK_DC_VOLTAGE] = u16SmpleCountADCAverageFilter;
    
    MCLIB_u16FilterAvgInit(au16ADCStack1ResultAvgFilterSamplesCount[ADC_STACK_CURRENT_L1], &asADCStack1ResultAvg[ADC_STACK_CURRENT_L1]);
    MCLIB_u16FilterAvgInit(au16ADCStack1ResultAvgFilterSamplesCount[ADC_STACK_CURRENT_L2], &asADCStack1ResultAvg[ADC_STACK_CURRENT_L2]);
    MCLIB_u16FilterAvgInit(au16ADCStack1ResultAvgFilterSamplesCount[ADC_STACK_CURRENT_L3], &asADCStack1ResultAvg[ADC_STACK_CURRENT_L3]);
    MCLIB_u16FilterAvgInit(au16ADCStack1ResultAvgFilterSamplesCount[ADC_STACK_DC_VOLTAGE], &asADCStack1ResultAvg[ADC_STACK_DC_VOLTAGE]);

    MCLIB_u16FilterAvgInit(au16ADCStack2ResultAvgFilterSamplesCount[ADC_STACK_CURRENT_L1], &asADCStack2ResultAvg[ADC_STACK_CURRENT_L1]);
    MCLIB_u16FilterAvgInit(au16ADCStack2ResultAvgFilterSamplesCount[ADC_STACK_CURRENT_L2], &asADCStack2ResultAvg[ADC_STACK_CURRENT_L2]);
    MCLIB_u16FilterAvgInit(au16ADCStack2ResultAvgFilterSamplesCount[ADC_STACK_CURRENT_L3], &asADCStack2ResultAvg[ADC_STACK_CURRENT_L3]);
    MCLIB_u16FilterAvgInit(au16ADCStack2ResultAvgFilterSamplesCount[ADC_STACK_DC_VOLTAGE], &asADCStack2ResultAvg[ADC_STACK_DC_VOLTAGE]);
    #endif

    sControlData.sStack[STACK_1].s16Speed = MCLIB_s16Mul_s16u32(
            sControlParameter.sStack[STACK_1].s16FieldSpeed_dHz,
            sControlConfig.u32DeciHzToSpeedMultiplier);
    
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].s16Speed = MCLIB_s16Mul_s16u32(
            sControlParameter.sStack[STACK_2].s16FieldSpeed_dHz,
            sControlConfig.u32DeciHzToSpeedMultiplier);
    #endif

    sControlData.sStack[STACK_1].s16SpeedFbckEmulation = MCLIB_s16Mul_s16u32(
            sControlParameter.sStack[STACK_1].s16FieldSpeedEmulation_dHz,
            sControlConfig.u32DeciHzToSpeedMultiplier);
    
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].s16SpeedFbckEmulation = MCLIB_s16Mul_s16u32(
            sControlParameter.sStack[STACK_1].s16FieldSpeedEmulation_dHz,
            sControlConfig.u32DeciHzToSpeedMultiplier);
    #endif

    #if BOARDCFG_STACK_COUNT >= 2
    #endif

    sControlData.sStack[STACK_1].s16CurrentRMS1PhFbckEmulation = MCLIB_s16Mul_s16u32(s16RMSCurrent1PHEmulationAmperes, sControlConfig.u32AmperesToADCResultMultiplierRequest);
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].s16CurrentRMS1PhFbckEmulation = MCLIB_s16Mul_s16u32(s16RMSCurrent1PHEmulationAmperes, sControlConfig.u32AmperesToADCResultMultiplierRequest);
    #endif
    sControlData.sStack[STACK_1].s16CurrentRMSReq1Ph = MCLIB_s16Mul_s16u32(s16RMSCurrent1PHRequestAmperes, sControlConfig.u32AmperesToADCResultMultiplierRequest);
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].s16CurrentRMSReq1Ph = MCLIB_s16Mul_s16u32(s16RMSCurrent1PHRequestAmperes, sControlConfig.u32AmperesToADCResultMultiplierRequest);
    #endif




    sControlData.sStack[STACK_1].u16VoltageDCBusFbckEmulationAtStart = MCLIB_u16Mul_u16u32(
            u16VoltageDCBusFbckEmulationAtStartRequest,
            sControlConfig.u32VoltsToADCResultMultiplier);
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].u16VoltageDCBusFbckEmulationAtStart = MCLIB_u16Mul_u16u32(
            u16VoltageDCBusFbckEmulationAtStartRequest,
            sControlConfig.u32VoltsToADCResultMultiplier);
    #endif

    sControlData.sStack[STACK_1].u16VoltageDCBusFbckEmulationRunning = MCLIB_u16Mul_u16u32(
            u16VoltageDCBusFbckEmulationRunningRequest,
            sControlConfig.u32VoltsToADCResultMultiplier);
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].u16VoltageDCBusFbckEmulationRunning = MCLIB_u16Mul_u16u32(
            u16VoltageDCBusFbckEmulationRunningRequest,
            sControlConfig.u32VoltsToADCResultMultiplier);
    #endif



    sControlData.sStack[STACK_1].u16VoltageDCBusFbckEmulation =
            sControlData.sStack[STACK_1].u16VoltageDCBusFbckEmulationAtStart;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].u16VoltageDCBusFbckEmulation =
            sControlData.sStack[STACK_2].u16VoltageDCBusFbckEmulationAtStart;
    #endif

    sControlData.sStack[STACK_1].u16VoltageDCBusFbckEmulationReq =
            sControlData.sStack[STACK_1].u16VoltageDCBusFbckEmulationAtStart;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].u16VoltageDCBusFbckEmulationReq =
            sControlData.sStack[STACK_2].u16VoltageDCBusFbckEmulationAtStart;
    #endif

    sControlData.sStack[STACK_1].sVoltageDCFbckEmulationRamp.u32State =
            (uint32_t) sControlData.sStack[STACK_1].u16VoltageDCBusFbckEmulationReq << 16;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sVoltageDCFbckEmulationRamp.u32State =
            (uint32_t) sControlData.sStack[STACK_2].u16VoltageDCBusFbckEmulationReq << 16;
    #endif


    #if 0
    sControlData.sStack[STACK_1].sVoltageDCFbckEmulationRamp.u32RampUp =
            (uint32_t) (MCLIB_s32Mul_s32u32((int16_t) ((int32_t) MCLIB_s32FractionalQ_15_16(AMP_VOLTAGE_FBCK_EMULATION_RAMPUP_V_PER_PWM_PERIOD) >> 16), sControlConfig.u32VoltsToADCResultMultiplier));
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sVoltageDCFbckEmulationRamp.u32RampUp =
            (uint32_t) (MCLIB_s32Mul_s32u32((int16_t) ((int32_t) MCLIB_s32FractionalQ_15_16(AMP_VOLTAGE_FBCK_EMULATION_RAMPUP_V_PER_PWM_PERIOD) >> 16), sControlConfig.u32VoltsToADCResultMultiplier));
    #endif
    sControlData.sStack[STACK_1].sVoltageDCFbckEmulationRamp.u32RampDown =
            (uint32_t) (MCLIB_s32Mul_s32u32((int16_t) MCLIB_u16Fractional(AMP_VOLTAGE_FBCK_EMULATION_RAMPDN_V_PER_PWM_PERIOD), sControlConfig.u32VoltsToADCResultMultiplier) >> 16);
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sVoltageDCFbckEmulationRamp.u32RampDown =
            (uint32_t) (MCLIB_s32Mul_s32u32((int16_t) MCLIB_u16Fractional(AMP_VOLTAGE_FBCK_EMULATION_RAMPDN_V_PER_PWM_PERIOD), sControlConfig.u32VoltsToADCResultMultiplier) >> 16);
    #endif
    #else
    sControlData.sStack[STACK_1].sVoltageDCFbckEmulationRamp.u32RampUp =
            (uint32_t) (MCLIB_u32Mul_u32u32(u32AmpVoltageFbkEmulationRampUpVoltsPerPWMPeriod, sControlConfig.u32VoltsToADCResultMultiplier));
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sVoltageDCFbckEmulationRamp.u32RampUp =
            (uint32_t) (MCLIB_u32Mul_u32u32(u32AmpVoltageFbkEmulationRampUpVoltsPerPWMPeriod, sControlConfig.u32VoltsToADCResultMultiplier));
    #endif
    sControlData.sStack[STACK_1].sVoltageDCFbckEmulationRamp.u32RampDown =
            (uint32_t) (MCLIB_u32Mul_u32u32(u32AmpVoltageFbkEmulationRampDnVoltsPerPWMPeriod, sControlConfig.u32VoltsToADCResultMultiplier));
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sVoltageDCFbckEmulationRamp.u32RampDown =
            (uint32_t) (MCLIB_u32Mul_u32u32(u32AmpVoltageFbkEmulationRampDnVoltsPerPWMPeriod, sControlConfig.u32VoltsToADCResultMultiplier));
    #endif
    #endif

    #if BOARDCFG_USE_FPGA
    FPGA_COMMON_vResetEnable();
    #endif


    sControlData.sStack[STACK_1].u16ScaleModulationIndex =
            u16ScaleModulationIndexSineWave;
    //u16ScaleModulationIndexSVM;

    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].u16ScaleModulationIndex =
            u16ScaleModulationIndexSineWave;
    //u16ScaleModulationIndexSVM;
    #endif


    /* Startup Voltage Output */
    sControlData.sStack[STACK_1].u16VoltageStartup = MCLIB_u16Mul_u16u32(
            u16VoltageStartupRequest,
            sControlConfig.u32VoltsToADCResultMultiplier);
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].u16VoltageStartup = MCLIB_u16Mul_u16u32(
            u16VoltageStartupRequest,
            sControlConfig.u32VoltsToADCResultMultiplier);
    #endif


    /* Voltage Request Controller Initialize Stack1 */
    sControlData.sStack[STACK_1].sVoltageReqPI.u16Max = u16PIControllerVoltageToCurrentMaxLimitRequest;
    sControlData.sStack[STACK_1].sVoltageReqPI.u16Min = u16PIControllerVoltageToCurrentMinLimitRequest;

    #if !defined(BOARDCFG_USE_MCC_ADC) && (BOARDCFG_USE_OLD_NON_MCC_ADC)
    s32RealSamplingFreqCoef = MCLIB_s32Mul_s16u32((int16_t) CONTROL_u16PWM1FreqHz, MCLIB_u32FractionalQ_16_16(SAMPLING_PERIOD_SEC)); /* in format Q15_16 */
    s32PropCoef = MCLIB_s32Mul_s32u32(s32RealSamplingFreqCoef, u32PropCoefBase);
    s32IntgCoef = MCLIB_s32Mul_s32u32(s32RealSamplingFreqCoef, u32IntgCoefBase);
    #else
    int32_t s32RealSamplingFreqCoef = MCLIB_s32Mul_s16u32((int16_t) CONTROL_u16PWM1FreqHz, MCLIB_u32FractionalQ_16_16(SAMPLING_PERIOD_SEC)); /* in format Q15_16 */
    s32PropCoef = MCLIB_s32Mul_s32u32(s32RealSamplingFreqCoef, u32PropCoefBase);
    s32IntgCoef = MCLIB_s32Mul_s32u32(s32RealSamplingFreqCoef, u32IntgCoefBase);
    #endif



    sControlData.sStack[STACK_1].sVoltageReqPI.s16PropGainShift = 15 -
            MCLIB_u16CountLeadingBits_s32(s32PropCoef);

    if (sControlData.sStack[STACK_1].sVoltageReqPI.s16PropGainShift >= 0) {
        sControlData.sStack[STACK_1].sVoltageReqPI.s16PropGain =
                (int16_t) (s32PropCoef >> (sControlData.sStack[STACK_1].sVoltageReqPI.s16PropGainShift + 1));
    } 
    else
    {
        sControlData.sStack[STACK_1].sVoltageReqPI.s16PropGain =
                (int16_t) (s32PropCoef << (MCLIB_s16NegSat(sControlData.sStack[STACK_1].sVoltageReqPI.s16PropGainShift) - 1));
    }

    sControlData.sStack[STACK_1].sVoltageReqPI.s16IntegGainShift = 15 -
            MCLIB_u16CountLeadingBits_s32(s32IntgCoef);

    if (sControlData.sStack[STACK_1].sVoltageReqPI.s16IntegGainShift >= 0) {
        sControlData.sStack[STACK_1].sVoltageReqPI.s16IntegGain =
                (int16_t) (s32IntgCoef >> (sControlData.sStack[STACK_1].sVoltageReqPI.s16IntegGainShift + 1));
    } 
    else
    {
        sControlData.sStack[STACK_1].sVoltageReqPI.s16IntegGain =
                (int16_t) (s32IntgCoef << (MCLIB_s16NegSat(sControlData.sStack[STACK_1].sVoltageReqPI.s16IntegGainShift) - 1));
    }


    //sControlData.sStack[STACK_1].sVoltageReqPI.s16PropGain = MCLIB_s16Fractional(CURRENT_TO_VOLTAGE_PROP_COEF_SH);
    //sControlData.sStack[STACK_1].sVoltageReqPI.s16PropGainShift = CURRENT_TO_VOLTAGE_PROP_SH;

    //sControlData.sStack[STACK_1].sVoltageReqPI.s16IntegGain = MCLIB_s16Fractional(CURRENT_TO_VOLTAGE_INTG_COEF_SH);
    //sControlData.sStack[STACK_1].sVoltageReqPI.s16IntegGainShift = CURRENT_TO_VOLTAGE_INTG_SH;


    /* Voltage Request Controller Initialize Stack2 */
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sVoltageReqPI.u16Max = u16PIControllerVoltageToCurrentMaxLimitRequest;
    sControlData.sStack[STACK_2].sVoltageReqPI.u16Min = u16PIControllerVoltageToCurrentMinLimitRequest;

    sControlData.sStack[STACK_2].sVoltageReqPI.s16PropGainShift = 15 -
            MCLIB_u16CountLeadingBits_s32(
            s32PropCoef);

    if (sControlData.sStack[STACK_2].sVoltageReqPI.s16PropGainShift >= 0) {
        sControlData.sStack[STACK_2].sVoltageReqPI.s16PropGain =
                (int16_t) (s32PropCoef
                >> (sControlData.sStack[STACK_2].sVoltageReqPI.s16PropGainShift + 1));
    } 
    else
    {
        sControlData.sStack[STACK_2].sVoltageReqPI.s16PropGain =
                (int16_t) (s32PropCoef
                << (MCLIB_s16NegSat(sControlData.sStack[STACK_2].sVoltageReqPI.s16PropGainShift) - 1));
    }

    sControlData.sStack[STACK_2].sVoltageReqPI.s16IntegGainShift = 15 -
            MCLIB_u16CountLeadingBits_s32(
            s32IntgCoef);

    if (sControlData.sStack[STACK_2].sVoltageReqPI.s16IntegGainShift >= 0) {
        sControlData.sStack[STACK_2].sVoltageReqPI.s16IntegGain =
                (int16_t) (s32IntgCoef
                >> (sControlData.sStack[STACK_2].sVoltageReqPI.s16IntegGainShift + 1));
    } 
    else
    {
        sControlData.sStack[STACK_2].sVoltageReqPI.s16IntegGain =
                (int16_t) (s32IntgCoef
                << (MCLIB_s16NegSat(sControlData.sStack[STACK_2].sVoltageReqPI.s16IntegGainShift) - 1));
    }

    //sControlData.sStack[STACK_2].sVoltageReqPI.s16PropGain = MCLIB_s16Fractional(CURRENT_TO_VOLTAGE_PROP_COEF_SH);
    //sControlData.sStack[STACK_2].sVoltageReqPI.s16PropGainShift = CURRENT_TO_VOLTAGE_PROP_SH;

    //sControlData.sStack[STACK_2].sVoltageReqPI.s16IntegGain = MCLIB_s16Fractional(CURRENT_TO_VOLTAGE_INTG_COEF_SH);
    //sControlData.sStack[STACK_2].sVoltageReqPI.s16IntegGainShift = CURRENT_TO_VOLTAGE_INTG_SH;
    #endif



    sControlData.sStack[STACK_1].u16VoltageDCBusDeltaCalibration = MCLIB_u16Mul_u16u32(
            u16VoltageDCBusDeltaCalibrationRequest,
            sControlConfig.u32VoltsToADCResultMultiplier);

    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].u16VoltageDCBusDeltaCalibration = MCLIB_u16Mul_u16u32(
            u16VoltageDCBusDeltaCalibrationRequest,
            sControlConfig.u32VoltsToADCResultMultiplier);
    #endif



    #if BOARDCFG_USE_FPGA
    FPGA_COMMON_vResetRelease();
    #endif



    CONTROL_vReset();
}

void CONTROL_vSetStartUpEmulationVoltage(void) {
    sControlData.sStack[STACK_1].u16VoltageDCBusFbckEmulationReq =
            sControlData.sStack[STACK_1].u16VoltageDCBusFbckEmulationAtStart;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].u16VoltageDCBusFbckEmulationReq =
            sControlData.sStack[STACK_2].u16VoltageDCBusFbckEmulationAtStart;
    #endif
}

void CONTROL_vSetRunningEmulationVoltage(void) {
    sControlData.sStack[STACK_1].u16VoltageDCBusFbckEmulationReq =
            sControlData.sStack[STACK_1].u16VoltageDCBusFbckEmulationRunning;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].u16VoltageDCBusFbckEmulationReq =
            sControlData.sStack[STACK_2].u16VoltageDCBusFbckEmulationRunning;
    #endif
}

void ADC_vOffsetBufferInit(void) {
    ADCStack1OffsetBuffer[ADC_STACK_CURRENT_L1] = 0;
    ADCStack1OffsetBuffer[ADC_STACK_CURRENT_L2] = 0;
    ADCStack1OffsetBuffer[ADC_STACK_CURRENT_L3] = 0;
    ADCStack1OffsetBuffer[ADC_STACK_DC_VOLTAGE] = 0;
    ADCStack2OffsetBuffer[ADC_STACK_CURRENT_L1] = 0;
    ADCStack2OffsetBuffer[ADC_STACK_CURRENT_L2] = 0;
    ADCStack2OffsetBuffer[ADC_STACK_CURRENT_L3] = 0;
    ADCStack2OffsetBuffer[ADC_STACK_DC_VOLTAGE] = 0;

    ADCStack3OffsetBuffer[ADC_STACK_CURRENT_L1] = 0;
    ADCStack3OffsetBuffer[ADC_STACK_CURRENT_L2] = 0;
    ADCStack3OffsetBuffer[ADC_STACK_CURRENT_L3] = 0;
    ADCStack3OffsetBuffer[ADC_STACK_DC_VOLTAGE] = 0;
    ADCDCLinkOffsetBuffer[ADC_STACK_CURRENT_L1] = 0;
    ADCDCLinkOffsetBuffer[ADC_STACK_CURRENT_L2] = 0;
    ADCDCLinkOffsetBuffer[ADC_STACK_CURRENT_L3] = 0;
    ADCDCLinkOffsetBuffer[ADC_STACK_DC_VOLTAGE] = 0;

    ADCLine_1OffsetBuffer[ADC_STACK_CURRENT_L1] = 0;
    ADCLine_1OffsetBuffer[ADC_STACK_CURRENT_L2] = 0;
    ADCLine_1OffsetBuffer[ADC_STACK_CURRENT_L3] = 0;
    ADCLine_1OffsetBuffer[ADC_STACK_DC_VOLTAGE] = 0;
    ADCLine_2OffsetBuffer[ADC_STACK_CURRENT_L1] = 0;
    ADCLine_2OffsetBuffer[ADC_STACK_CURRENT_L2] = 0;
    ADCLine_2OffsetBuffer[ADC_STACK_CURRENT_L3] = 0;
    ADCLine_2OffsetBuffer[ADC_STACK_DC_VOLTAGE] = 0;

    ADC2Stack1OffsetBuffer[ADC2_STACK_CURRFBK_L1] = 0;
    ADC2Stack1OffsetBuffer[ADC2_STACK_CURRFBK_L2] = 0;
    ADC2Stack1OffsetBuffer[ADC2_STACK_CURRFBK_L3] = 0;
    ADC2Stack1OffsetBuffer[ADC2_STACK_DCVOLTAGE2] = 0;
    ADC2Stack1OffsetBuffer[ADC2_STACK_TEMPFBK_L1] = 0;
    ADC2Stack1OffsetBuffer[ADC2_STACK_TEMPFBK_L2] = 0;
    ADC2Stack1OffsetBuffer[ADC2_STACK_TEMPFBK_L3] = 0;
    ADC2Stack2OffsetBuffer[ADC2_STACK_DCVOLTAGE2] = 0;
}

void ADC_vReadADCData(void) 
{
    if (sControlAccess.bEmulateVoltageFbck) 
    {
        #if BOARDCFG_USE_TSC_DSPIC
        ADCStack1Buffered[ADC_STACK_DC_VOLTAGE] = sControlData.sStack[STACK_1].u16VoltageDCBusFbckEmulation;
        #if BOARDCFG_STACK_COUNT >= 2
        ADCStack2Buffered[ADC_STACK_DC_VOLTAGE] = sControlData.sStack[STACK_2].u16VoltageDCBusFbckEmulation;
        #endif
        #else
        ADCDCLinkBuffered[ADC_STACK_DC_VOLTAGE] = sControlData.sStack[STACK_1].u16VoltageDCBusFbckEmulation;
        #endif
    }
    
    #if BOARDCFG_USE_INFO_CALC_MIN_MAX_AVG
    if(bResetADCMinMax)
    {
        bResetADCMinMax = 0;
        
        ADCStack1ResultMin[ADC_STACK_CURRENT_L1] = UINT16_MAX;
        ADCStack1ResultMin[ADC_STACK_CURRENT_L2] = UINT16_MAX;
        ADCStack1ResultMin[ADC_STACK_CURRENT_L3] = UINT16_MAX;
        ADCStack1ResultMin[ADC_STACK_DC_VOLTAGE] = UINT16_MAX;
        ADCStack2ResultMin[ADC_STACK_CURRENT_L1] = UINT16_MAX;
        ADCStack2ResultMin[ADC_STACK_CURRENT_L2] = UINT16_MAX;
        ADCStack2ResultMin[ADC_STACK_CURRENT_L3] = UINT16_MAX;
        ADCStack2ResultMin[ADC_STACK_DC_VOLTAGE] = UINT16_MAX;
        
        ADCStack1ResultMax[ADC_STACK_CURRENT_L1] = 0;
        ADCStack1ResultMax[ADC_STACK_CURRENT_L2] = 0;
        ADCStack1ResultMax[ADC_STACK_CURRENT_L3] = 0;
        ADCStack1ResultMax[ADC_STACK_DC_VOLTAGE] = 0;
        ADCStack2ResultMax[ADC_STACK_CURRENT_L1] = 0;
        ADCStack2ResultMax[ADC_STACK_CURRENT_L2] = 0;
        ADCStack2ResultMax[ADC_STACK_CURRENT_L3] = 0;
        ADCStack2ResultMax[ADC_STACK_DC_VOLTAGE] = 0;   
    }

    if (bCalcADCMinMax) 
    {
        ADCStack1ResultMin[ADC_STACK_CURRENT_L1] = MCLIB_u16Min(ADCStack1ResultMin[ADC_STACK_CURRENT_L1], ADCStack1Buffered[ADC_STACK_CURRENT_L1]);
        ADCStack1ResultMin[ADC_STACK_CURRENT_L2] = MCLIB_u16Min(ADCStack1ResultMin[ADC_STACK_CURRENT_L2], ADCStack1Buffered[ADC_STACK_CURRENT_L2]);
        ADCStack1ResultMin[ADC_STACK_CURRENT_L3] = MCLIB_u16Min(ADCStack1ResultMin[ADC_STACK_CURRENT_L3], ADCStack1Buffered[ADC_STACK_CURRENT_L3]);
        ADCStack1ResultMin[ADC_STACK_DC_VOLTAGE] = MCLIB_u16Min(ADCStack1ResultMin[ADC_STACK_DC_VOLTAGE], ADCStack1Buffered[ADC_STACK_DC_VOLTAGE]);

        ADCStack1ResultMax[ADC_STACK_CURRENT_L1] = MCLIB_u16Max(ADCStack1ResultMax[ADC_STACK_CURRENT_L1], ADCStack1Buffered[ADC_STACK_CURRENT_L1]);
        ADCStack1ResultMax[ADC_STACK_CURRENT_L2] = MCLIB_u16Max(ADCStack1ResultMax[ADC_STACK_CURRENT_L2], ADCStack1Buffered[ADC_STACK_CURRENT_L2]);
        ADCStack1ResultMax[ADC_STACK_CURRENT_L3] = MCLIB_u16Max(ADCStack1ResultMax[ADC_STACK_CURRENT_L3], ADCStack1Buffered[ADC_STACK_CURRENT_L3]);
        ADCStack1ResultMax[ADC_STACK_DC_VOLTAGE] = MCLIB_u16Max(ADCStack1ResultMax[ADC_STACK_DC_VOLTAGE], ADCStack1Buffered[ADC_STACK_DC_VOLTAGE]);

        ADCStack2ResultMin[ADC_STACK_CURRENT_L1] = MCLIB_u16Min(ADCStack2ResultMin[ADC_STACK_CURRENT_L1], ADCStack2Buffered[ADC_STACK_CURRENT_L1]);
        ADCStack2ResultMin[ADC_STACK_CURRENT_L2] = MCLIB_u16Min(ADCStack2ResultMin[ADC_STACK_CURRENT_L2], ADCStack2Buffered[ADC_STACK_CURRENT_L2]);
        ADCStack2ResultMin[ADC_STACK_CURRENT_L3] = MCLIB_u16Min(ADCStack2ResultMin[ADC_STACK_CURRENT_L3], ADCStack2Buffered[ADC_STACK_CURRENT_L3]);
        ADCStack2ResultMin[ADC_STACK_DC_VOLTAGE] = MCLIB_u16Min(ADCStack2ResultMin[ADC_STACK_DC_VOLTAGE], ADCStack2Buffered[ADC_STACK_DC_VOLTAGE]);

        ADCStack2ResultMax[ADC_STACK_CURRENT_L1] = MCLIB_u16Max(ADCStack2ResultMax[ADC_STACK_CURRENT_L1], ADCStack2Buffered[ADC_STACK_CURRENT_L1]);
        ADCStack2ResultMax[ADC_STACK_CURRENT_L2] = MCLIB_u16Max(ADCStack2ResultMax[ADC_STACK_CURRENT_L2], ADCStack2Buffered[ADC_STACK_CURRENT_L2]);
        ADCStack2ResultMax[ADC_STACK_CURRENT_L3] = MCLIB_u16Max(ADCStack2ResultMax[ADC_STACK_CURRENT_L3], ADCStack2Buffered[ADC_STACK_CURRENT_L3]);
        ADCStack2ResultMax[ADC_STACK_DC_VOLTAGE] = MCLIB_u16Max(ADCStack2ResultMax[ADC_STACK_DC_VOLTAGE], ADCStack2Buffered[ADC_STACK_DC_VOLTAGE]);
    }
    if (bCalcADCAvg) 
    {
        ADCStack1ResultAvg[ADC_STACK_CURRENT_L1] = MCLIB_u16FilterAvg(ADCStack1Buffered[ADC_STACK_CURRENT_L1], &asADCStack1ResultAvg[ADC_STACK_CURRENT_L1]);
        ADCStack1ResultAvg[ADC_STACK_CURRENT_L2] = MCLIB_u16FilterAvg(ADCStack1Buffered[ADC_STACK_CURRENT_L2], &asADCStack1ResultAvg[ADC_STACK_CURRENT_L2]);
        ADCStack1ResultAvg[ADC_STACK_CURRENT_L3] = MCLIB_u16FilterAvg(ADCStack1Buffered[ADC_STACK_CURRENT_L3], &asADCStack1ResultAvg[ADC_STACK_CURRENT_L3]);
        ADCStack1ResultAvg[ADC_STACK_DC_VOLTAGE] = MCLIB_u16FilterAvg(ADCStack1Buffered[ADC_STACK_DC_VOLTAGE], &asADCStack1ResultAvg[ADC_STACK_DC_VOLTAGE]);
 
        ADCStack2ResultAvg[ADC_STACK_CURRENT_L1] = MCLIB_u16FilterAvg(ADCStack2Buffered[ADC_STACK_CURRENT_L1], &asADCStack2ResultAvg[ADC_STACK_CURRENT_L1]);
        ADCStack2ResultAvg[ADC_STACK_CURRENT_L2] = MCLIB_u16FilterAvg(ADCStack2Buffered[ADC_STACK_CURRENT_L2], &asADCStack2ResultAvg[ADC_STACK_CURRENT_L2]);
        ADCStack2ResultAvg[ADC_STACK_CURRENT_L3] = MCLIB_u16FilterAvg(ADCStack2Buffered[ADC_STACK_CURRENT_L3], &asADCStack2ResultAvg[ADC_STACK_CURRENT_L3]);
        ADCStack2ResultAvg[ADC_STACK_DC_VOLTAGE] = MCLIB_u16FilterAvg(ADCStack2Buffered[ADC_STACK_DC_VOLTAGE], &asADCStack2ResultAvg[ADC_STACK_DC_VOLTAGE]);
    }
    
    if (bCalcADCAvgSampleCountChangeRuntime) 
    {
        MCLIB_u16FilterAvgInit(au16ADCStack1ResultAvgFilterSamplesCount[ADC_STACK_CURRENT_L1], &asADCStack1ResultAvg[ADC_STACK_CURRENT_L1]);
        MCLIB_u16FilterAvgInit(au16ADCStack1ResultAvgFilterSamplesCount[ADC_STACK_CURRENT_L2], &asADCStack1ResultAvg[ADC_STACK_CURRENT_L2]);
        MCLIB_u16FilterAvgInit(au16ADCStack1ResultAvgFilterSamplesCount[ADC_STACK_CURRENT_L3], &asADCStack1ResultAvg[ADC_STACK_CURRENT_L3]);
        MCLIB_u16FilterAvgInit(au16ADCStack1ResultAvgFilterSamplesCount[ADC_STACK_DC_VOLTAGE], &asADCStack1ResultAvg[ADC_STACK_DC_VOLTAGE]);

        MCLIB_u16FilterAvgInit(au16ADCStack2ResultAvgFilterSamplesCount[ADC_STACK_CURRENT_L1], &asADCStack2ResultAvg[ADC_STACK_CURRENT_L1]);
        MCLIB_u16FilterAvgInit(au16ADCStack2ResultAvgFilterSamplesCount[ADC_STACK_CURRENT_L2], &asADCStack2ResultAvg[ADC_STACK_CURRENT_L2]);
        MCLIB_u16FilterAvgInit(au16ADCStack2ResultAvgFilterSamplesCount[ADC_STACK_CURRENT_L3], &asADCStack2ResultAvg[ADC_STACK_CURRENT_L3]);
        MCLIB_u16FilterAvgInit(au16ADCStack2ResultAvgFilterSamplesCount[ADC_STACK_DC_VOLTAGE], &asADCStack2ResultAvg[ADC_STACK_DC_VOLTAGE]);
    }
    

    if (bUseRuntimeOffsetFix)
    {
        ADCStack1Offset[ADC_STACK_CURRENT_L1] = ADCStack1ResultAvg[ADC_STACK_CURRENT_L1];
        ADCStack1Offset[ADC_STACK_CURRENT_L2] = ADCStack1ResultAvg[ADC_STACK_CURRENT_L2];
        ADCStack1Offset[ADC_STACK_CURRENT_L3] = ADCStack1ResultAvg[ADC_STACK_CURRENT_L3];
        ADCStack2Offset[ADC_STACK_CURRENT_L1] = ADCStack2ResultAvg[ADC_STACK_CURRENT_L1];
        ADCStack2Offset[ADC_STACK_CURRENT_L2] = ADCStack2ResultAvg[ADC_STACK_CURRENT_L2];
        ADCStack2Offset[ADC_STACK_CURRENT_L3] = ADCStack2ResultAvg[ADC_STACK_CURRENT_L3];
        
        //if (eControlState == CTRL_STA_CALIB)
        //ADCStack1Offset[ADC_STACK_DC_VOLTAGE] = ADCStack1ResultAvg[ADC_STACK_DC_VOLTAGE];
        //ADCStack2Offset[ADC_STACK_DC_VOLTAGE] = ADCStack2ResultAvg[ADC_STACK_DC_VOLTAGE];  
    }
    #endif

    /* Read New Data Here From ADCBuffered */

    ADCStack1NoOffset[ADC_STACK_CURRENT_L1] = ADCStack1Buffered[ADC_STACK_CURRENT_L1] - ADCStack1Offset[ADC_STACK_CURRENT_L1];
    ADCStack1NoOffset[ADC_STACK_CURRENT_L2] = ADCStack1Buffered[ADC_STACK_CURRENT_L2] - ADCStack1Offset[ADC_STACK_CURRENT_L2];
    ADCStack1NoOffset[ADC_STACK_CURRENT_L3] = ADCStack1Buffered[ADC_STACK_CURRENT_L3] - ADCStack1Offset[ADC_STACK_CURRENT_L3];
    ADCStack1NoOffset[ADC_STACK_DC_VOLTAGE] = ADCStack1Buffered[ADC_STACK_DC_VOLTAGE] - ADCStack1Offset[ADC_STACK_DC_VOLTAGE];
    ADCStack2NoOffset[ADC_STACK_CURRENT_L1] = ADCStack2Buffered[ADC_STACK_CURRENT_L1] - ADCStack2Offset[ADC_STACK_CURRENT_L1];
    ADCStack2NoOffset[ADC_STACK_CURRENT_L2] = ADCStack2Buffered[ADC_STACK_CURRENT_L2] - ADCStack2Offset[ADC_STACK_CURRENT_L2];
    ADCStack2NoOffset[ADC_STACK_CURRENT_L3] = ADCStack2Buffered[ADC_STACK_CURRENT_L3] - ADCStack2Offset[ADC_STACK_CURRENT_L3];
    ADCStack2NoOffset[ADC_STACK_DC_VOLTAGE] = ADCStack2Buffered[ADC_STACK_DC_VOLTAGE] - ADCStack2Offset[ADC_STACK_DC_VOLTAGE];

    ADCStack3NoOffset[ADC_STACK_CURRENT_L1] = ADCStack3Buffered[ADC_STACK_CURRENT_L1] - ADCStack3Offset[ADC_STACK_CURRENT_L1];
    ADCStack3NoOffset[ADC_STACK_CURRENT_L2] = ADCStack3Buffered[ADC_STACK_CURRENT_L2] - ADCStack3Offset[ADC_STACK_CURRENT_L2];
    ADCStack3NoOffset[ADC_STACK_CURRENT_L3] = ADCStack3Buffered[ADC_STACK_CURRENT_L3] - ADCStack3Offset[ADC_STACK_CURRENT_L3];
    ADCStack3NoOffset[ADC_STACK_DC_VOLTAGE] = ADCStack3Buffered[ADC_STACK_DC_VOLTAGE] - ADCStack3Offset[ADC_STACK_DC_VOLTAGE];
    ADCDCLinkNoOffset[ADC_STACK_CURRENT_L1] = ADCDCLinkBuffered[ADC_STACK_CURRENT_L1] - ADCDCLinkOffset[ADC_STACK_CURRENT_L1];
    ADCDCLinkNoOffset[ADC_STACK_CURRENT_L2] = ADCDCLinkBuffered[ADC_STACK_CURRENT_L2] - ADCDCLinkOffset[ADC_STACK_CURRENT_L2];
    ADCDCLinkNoOffset[ADC_STACK_CURRENT_L3] = ADCDCLinkBuffered[ADC_STACK_CURRENT_L3] - ADCDCLinkOffset[ADC_STACK_CURRENT_L3];
    ADCDCLinkNoOffset[ADC_STACK_DC_VOLTAGE] = ADCDCLinkBuffered[ADC_STACK_DC_VOLTAGE] - ADCDCLinkOffset[ADC_STACK_DC_VOLTAGE];

    ADCLine_1NoOffset[ADC_STACK_CURRENT_L1] = ADCLine_1Buffered[ADC_STACK_CURRENT_L1] - ADCLine_1Offset[ADC_STACK_CURRENT_L1];
    ADCLine_1NoOffset[ADC_STACK_CURRENT_L2] = ADCLine_1Buffered[ADC_STACK_CURRENT_L2] - ADCLine_1Offset[ADC_STACK_CURRENT_L2];
    ADCLine_1NoOffset[ADC_STACK_CURRENT_L3] = ADCLine_1Buffered[ADC_STACK_CURRENT_L3] - ADCLine_1Offset[ADC_STACK_CURRENT_L3];
    ADCLine_1NoOffset[ADC_STACK_DC_VOLTAGE] = ADCLine_1Buffered[ADC_STACK_DC_VOLTAGE] - ADCLine_1Offset[ADC_STACK_DC_VOLTAGE];
    ADCLine_2NoOffset[ADC_STACK_CURRENT_L1] = ADCLine_2Buffered[ADC_STACK_CURRENT_L1] - ADCLine_2Offset[ADC_STACK_CURRENT_L1];
    ADCLine_2NoOffset[ADC_STACK_CURRENT_L2] = ADCLine_2Buffered[ADC_STACK_CURRENT_L2] - ADCLine_2Offset[ADC_STACK_CURRENT_L2];
    ADCLine_2NoOffset[ADC_STACK_CURRENT_L3] = ADCLine_2Buffered[ADC_STACK_CURRENT_L3] - ADCLine_2Offset[ADC_STACK_CURRENT_L3];
    ADCLine_2NoOffset[ADC_STACK_DC_VOLTAGE] = ADCLine_2Buffered[ADC_STACK_DC_VOLTAGE] - ADCLine_2Offset[ADC_STACK_DC_VOLTAGE];

    ADCLine_1NoOffset_LLV3_H2O_IN  = (uint16_t)ADCLine_1NoOffset[ADC_STACK_CURRENT_L3];
    ADCLine_2NoOffset_LLV4_H2O_OUT = (uint16_t)ADCLine_2NoOffset[ADC_STACK_CURRENT_L1];

    
    if (bUseADCResultFix)
    {
        ADCStack1NoOffsetReal[ADC_STACK_CURRENT_L1] = ADCStack1NoOffset[ADC_STACK_CURRENT_L1];
        ADCStack1NoOffsetReal[ADC_STACK_CURRENT_L2] = ADCStack1NoOffset[ADC_STACK_CURRENT_L2];
        ADCStack1NoOffsetReal[ADC_STACK_CURRENT_L3] = ADCStack1NoOffset[ADC_STACK_CURRENT_L3];

        ADCStack2NoOffsetReal[ADC_STACK_CURRENT_L1] = ADCStack2NoOffset[ADC_STACK_CURRENT_L1];
        ADCStack2NoOffsetReal[ADC_STACK_CURRENT_L2] = ADCStack2NoOffset[ADC_STACK_CURRENT_L2];
        ADCStack2NoOffsetReal[ADC_STACK_CURRENT_L3] = ADCStack2NoOffset[ADC_STACK_CURRENT_L3];

        ADCStack3NoOffsetReal[ADC_STACK_CURRENT_L1] = ADCStack3NoOffset[ADC_STACK_CURRENT_L1];
        ADCStack3NoOffsetReal[ADC_STACK_CURRENT_L2] = ADCStack3NoOffset[ADC_STACK_CURRENT_L2];
        ADCStack3NoOffsetReal[ADC_STACK_CURRENT_L3] = ADCStack3NoOffset[ADC_STACK_CURRENT_L3];

        ADCStack1NoOffset[ADC_STACK_CURRENT_L1] = (ADCStack1NoOffsetReal[ADC_STACK_CURRENT_L1] >> sControlConfig.u16ADCResultToAmperesFixShiftStack1);
        ADCStack1NoOffset[ADC_STACK_CURRENT_L2] = (ADCStack1NoOffsetReal[ADC_STACK_CURRENT_L2] >> sControlConfig.u16ADCResultToAmperesFixShiftStack1);
        ADCStack1NoOffset[ADC_STACK_CURRENT_L3] = (ADCStack1NoOffsetReal[ADC_STACK_CURRENT_L3] >> sControlConfig.u16ADCResultToAmperesFixShiftStack1);

        ADCStack2NoOffset[ADC_STACK_CURRENT_L1] = (ADCStack2NoOffsetReal[ADC_STACK_CURRENT_L1] >> sControlConfig.u16ADCResultToAmperesFixShiftStack2);
        ADCStack2NoOffset[ADC_STACK_CURRENT_L2] = (ADCStack2NoOffsetReal[ADC_STACK_CURRENT_L2] >> sControlConfig.u16ADCResultToAmperesFixShiftStack2);
        ADCStack2NoOffset[ADC_STACK_CURRENT_L3] = (ADCStack2NoOffsetReal[ADC_STACK_CURRENT_L3] >> sControlConfig.u16ADCResultToAmperesFixShiftStack2);

        ADCStack3NoOffset[ADC_STACK_CURRENT_L1] = (ADCStack3NoOffsetReal[ADC_STACK_CURRENT_L1] >> sControlConfig.u16ADCResultToAmperesFixShiftStack3);
        ADCStack3NoOffset[ADC_STACK_CURRENT_L2] = (ADCStack3NoOffsetReal[ADC_STACK_CURRENT_L2] >> sControlConfig.u16ADCResultToAmperesFixShiftStack3);
        ADCStack3NoOffset[ADC_STACK_CURRENT_L3] = (ADCStack3NoOffsetReal[ADC_STACK_CURRENT_L3] >> sControlConfig.u16ADCResultToAmperesFixShiftStack3);
    }
    
    #if BOARDCFG_USE_INFO_CALC_MIN_MAX_AVG
    if (bCalcADCMinMax)
    {
        ADCStack1ResultMinNoOffset[ADC_STACK_CURRENT_L1] = MCLIB_s16Sub(ADCStack1ResultMin[ADC_STACK_CURRENT_L1], ADCStack1Offset[ADC_STACK_CURRENT_L1]);
        ADCStack1ResultMinNoOffset[ADC_STACK_CURRENT_L2] = MCLIB_s16Sub(ADCStack1ResultMin[ADC_STACK_CURRENT_L1], ADCStack1Offset[ADC_STACK_CURRENT_L2]);
        ADCStack1ResultMinNoOffset[ADC_STACK_CURRENT_L3] = MCLIB_s16Sub(ADCStack1ResultMin[ADC_STACK_CURRENT_L1], ADCStack1Offset[ADC_STACK_CURRENT_L3]);
        ADCStack1ResultMinNoOffset[ADC_STACK_DC_VOLTAGE] = MCLIB_s16Sub(ADCStack1ResultMin[ADC_STACK_CURRENT_L1], ADCStack1Offset[ADC_STACK_DC_VOLTAGE]);

        ADCStack1ResultMaxNoOffset[ADC_STACK_CURRENT_L1] = MCLIB_s16Sub(ADCStack1ResultMax[ADC_STACK_CURRENT_L1], ADCStack1Offset[ADC_STACK_CURRENT_L1]);
        ADCStack1ResultMaxNoOffset[ADC_STACK_CURRENT_L2] = MCLIB_s16Sub(ADCStack1ResultMax[ADC_STACK_CURRENT_L1], ADCStack1Offset[ADC_STACK_CURRENT_L2]);
        ADCStack1ResultMaxNoOffset[ADC_STACK_CURRENT_L3] = MCLIB_s16Sub(ADCStack1ResultMax[ADC_STACK_CURRENT_L1], ADCStack1Offset[ADC_STACK_CURRENT_L3]);
        ADCStack1ResultMaxNoOffset[ADC_STACK_DC_VOLTAGE] = MCLIB_s16Sub(ADCStack1ResultMax[ADC_STACK_CURRENT_L1], ADCStack1Offset[ADC_STACK_DC_VOLTAGE]);

        ADCStack2ResultMinNoOffset[ADC_STACK_CURRENT_L1] = MCLIB_s16Sub(ADCStack2ResultMin[ADC_STACK_CURRENT_L1], ADCStack2Offset[ADC_STACK_CURRENT_L1]);
        ADCStack2ResultMinNoOffset[ADC_STACK_CURRENT_L2] = MCLIB_s16Sub(ADCStack2ResultMin[ADC_STACK_CURRENT_L1], ADCStack2Offset[ADC_STACK_CURRENT_L2]);
        ADCStack2ResultMinNoOffset[ADC_STACK_CURRENT_L3] = MCLIB_s16Sub(ADCStack2ResultMin[ADC_STACK_CURRENT_L1], ADCStack2Offset[ADC_STACK_CURRENT_L3]);
        ADCStack2ResultMinNoOffset[ADC_STACK_DC_VOLTAGE] = MCLIB_s16Sub(ADCStack2ResultMin[ADC_STACK_CURRENT_L1], ADCStack2Offset[ADC_STACK_DC_VOLTAGE]);

        ADCStack2ResultMaxNoOffset[ADC_STACK_CURRENT_L1] = MCLIB_s16Sub(ADCStack2ResultMax[ADC_STACK_CURRENT_L1], ADCStack2Offset[ADC_STACK_CURRENT_L1]);
        ADCStack2ResultMaxNoOffset[ADC_STACK_CURRENT_L2] = MCLIB_s16Sub(ADCStack2ResultMax[ADC_STACK_CURRENT_L1], ADCStack2Offset[ADC_STACK_CURRENT_L2]);
        ADCStack2ResultMaxNoOffset[ADC_STACK_CURRENT_L3] = MCLIB_s16Sub(ADCStack2ResultMax[ADC_STACK_CURRENT_L1], ADCStack2Offset[ADC_STACK_CURRENT_L3]);
        ADCStack2ResultMaxNoOffset[ADC_STACK_DC_VOLTAGE] = MCLIB_s16Sub(ADCStack2ResultMax[ADC_STACK_CURRENT_L1], ADCStack2Offset[ADC_STACK_DC_VOLTAGE]);
    }    
    
    if (bCalcADCAvg)
    {
        ADCStack1ResultAmplitude[ADC_STACK_CURRENT_L1] = MCLIB_u16SubSat_u16u16(ADCStack1ResultMaxNoOffset[ADC_STACK_CURRENT_L1], ADCStack1ResultMinNoOffset[ADC_STACK_CURRENT_L1]);
        ADCStack1ResultAmplitude[ADC_STACK_CURRENT_L2] = MCLIB_u16SubSat_u16u16(ADCStack1ResultMaxNoOffset[ADC_STACK_CURRENT_L2], ADCStack1ResultMinNoOffset[ADC_STACK_CURRENT_L2]);
        ADCStack1ResultAmplitude[ADC_STACK_CURRENT_L3] = MCLIB_u16SubSat_u16u16(ADCStack1ResultMaxNoOffset[ADC_STACK_CURRENT_L3], ADCStack1ResultMinNoOffset[ADC_STACK_CURRENT_L3]);
        ADCStack1ResultAmplitude[ADC_STACK_DC_VOLTAGE] = MCLIB_u16SubSat_u16u16(ADCStack1ResultMaxNoOffset[ADC_STACK_DC_VOLTAGE], ADCStack1ResultMinNoOffset[ADC_STACK_DC_VOLTAGE]);

        ADCStack2ResultAmplitude[ADC_STACK_CURRENT_L1] = MCLIB_u16SubSat_u16u16(ADCStack2ResultMax[ADC_STACK_CURRENT_L1], ADCStack2ResultMin[ADC_STACK_CURRENT_L1]);
        ADCStack2ResultAmplitude[ADC_STACK_CURRENT_L2] = MCLIB_u16SubSat_u16u16(ADCStack2ResultMax[ADC_STACK_CURRENT_L2], ADCStack2ResultMin[ADC_STACK_CURRENT_L2]);
        ADCStack2ResultAmplitude[ADC_STACK_CURRENT_L3] = MCLIB_u16SubSat_u16u16(ADCStack2ResultMax[ADC_STACK_CURRENT_L3], ADCStack2ResultMin[ADC_STACK_CURRENT_L3]);
        ADCStack2ResultAmplitude[ADC_STACK_DC_VOLTAGE] = MCLIB_u16SubSat_u16u16(ADCStack2ResultMax[ADC_STACK_DC_VOLTAGE], ADCStack2ResultMin[ADC_STACK_DC_VOLTAGE]);
    }    
    #endif

    if (bUseOffsetOnCurrFbk) 
    {
        ADC2Stack1NoOffset[ADC2_STACK_CURRFBK_L1] = ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L1] - ADC2Stack1Offset[ADC2_STACK_CURRFBK_L1];
        ADC2Stack1NoOffset[ADC2_STACK_CURRFBK_L2] = ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L2] - ADC2Stack1Offset[ADC2_STACK_CURRFBK_L2];
        ADC2Stack1NoOffset[ADC2_STACK_CURRFBK_L3] = ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L3] - ADC2Stack1Offset[ADC2_STACK_CURRFBK_L3];
        
        ADC2Stack2NoOffset[ADC2_STACK_CURRFBK_L1] = ADC2Stack2Buffered[ADC2_STACK_CURRFBK_L1] - ADC2Stack2Offset[ADC2_STACK_CURRFBK_L1];
        ADC2Stack2NoOffset[ADC2_STACK_CURRFBK_L2] = ADC2Stack2Buffered[ADC2_STACK_CURRFBK_L2] - ADC2Stack2Offset[ADC2_STACK_CURRFBK_L2];
        ADC2Stack2NoOffset[ADC2_STACK_CURRFBK_L3] = ADC2Stack2Buffered[ADC2_STACK_CURRFBK_L3] - ADC2Stack2Offset[ADC2_STACK_CURRFBK_L3];
    } 
    else 
    {  
        ADC2Stack1NoOffset[ADC2_STACK_CURRFBK_L1] = ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L1] - 0;
        ADC2Stack1NoOffset[ADC2_STACK_CURRFBK_L2] = ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L2] - 0;
        ADC2Stack1NoOffset[ADC2_STACK_CURRFBK_L3] = ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L3] - 0;
        
        ADC2Stack2NoOffset[ADC2_STACK_CURRFBK_L1] = ADC2Stack2Buffered[ADC2_STACK_CURRFBK_L1] - 0;
        ADC2Stack2NoOffset[ADC2_STACK_CURRFBK_L2] = ADC2Stack2Buffered[ADC2_STACK_CURRFBK_L2] - 0;
        ADC2Stack2NoOffset[ADC2_STACK_CURRFBK_L3] = ADC2Stack2Buffered[ADC2_STACK_CURRFBK_L3] - 0;
    }
    ADC2Stack1NoOffset[ADC2_STACK_DCVOLTAGE2] = ADC2Stack1Buffered[ADC2_STACK_DCVOLTAGE2] - ADC2Stack1Offset[ADC2_STACK_DCVOLTAGE2];
    
    if (bUseOffsetOnTempFbk) 
    {
        ADC2Stack1NoOffset[ADC2_STACK_TEMPFBK_L1] = ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L1] - ADC2Stack1Offset[ADC2_STACK_TEMPFBK_L1];
        ADC2Stack1NoOffset[ADC2_STACK_TEMPFBK_L2] = ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L2] - ADC2Stack1Offset[ADC2_STACK_TEMPFBK_L2];
        ADC2Stack1NoOffset[ADC2_STACK_TEMPFBK_L3] = ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L3] - ADC2Stack1Offset[ADC2_STACK_TEMPFBK_L3];
        
        ADC2Stack2NoOffset[ADC2_STACK_TEMPFBK_L1] = ADC2Stack2Buffered[ADC2_STACK_TEMPFBK_L1] - ADC2Stack2Offset[ADC2_STACK_TEMPFBK_L1];
        ADC2Stack2NoOffset[ADC2_STACK_TEMPFBK_L2] = ADC2Stack2Buffered[ADC2_STACK_TEMPFBK_L2] - ADC2Stack2Offset[ADC2_STACK_TEMPFBK_L2];
        ADC2Stack2NoOffset[ADC2_STACK_TEMPFBK_L3] = ADC2Stack2Buffered[ADC2_STACK_TEMPFBK_L3] - ADC2Stack2Offset[ADC2_STACK_TEMPFBK_L3];
    } 
    else 
    {
        ADC2Stack1NoOffset[ADC2_STACK_TEMPFBK_L1] = ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L1] - 0;
        ADC2Stack1NoOffset[ADC2_STACK_TEMPFBK_L2] = ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L2] - 0;
        ADC2Stack1NoOffset[ADC2_STACK_TEMPFBK_L3] = ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L3] - 0;
        
        ADC2Stack2NoOffset[ADC2_STACK_TEMPFBK_L1] = ADC2Stack2Buffered[ADC2_STACK_TEMPFBK_L1] - 0;
        ADC2Stack2NoOffset[ADC2_STACK_TEMPFBK_L2] = ADC2Stack2Buffered[ADC2_STACK_TEMPFBK_L2] - 0;
        ADC2Stack2NoOffset[ADC2_STACK_TEMPFBK_L3] = ADC2Stack2Buffered[ADC2_STACK_TEMPFBK_L3] - 0;
    }
    ADC2Stack2NoOffset[ADC2_STACK_DCVOLTAGE2] = ADC2Stack2Buffered[ADC2_STACK_DCVOLTAGE2] - ADC2Stack2Offset[ADC2_STACK_DCVOLTAGE2];

    /* Temperature Measurement Error Detection */
    
    if (bUseControlTemperatureTrip)
    {
        if (TempFbkStack1[0] > sControlParameter.s16TripTemperature_dDeg) 
        {
            uTempError.sTempError.bErrTemp1 = 1;
        }
        if (TempFbkStack1[1] > sControlParameter.s16TripTemperature_dDeg) 
        {
            uTempError.sTempError.bErrTemp2 = 1;
        }
        if (TempFbkStack1[2] > sControlParameter.s16TripTemperature_dDeg) 
        {
            uTempError.sTempError.bErrTemp3 = 1;
        }
    }







}

void CONTROL_vProcessMainLoop(void) 
{
    #if BOARDCFG_USE_FPGA
    if (bPWMEnableRequest == 0)
    {
        FPGA_COMMON_vDisablePWM();
    }
    #endif

    #if BOARDCFG_USE_FPGA_PWM
    /* Temp Offset Calculation Allow (trough bDetectDriver) */
    bDetectDriverPhase1 = FPGA_PWM_vGetDetectDriverPhase1();
    bDetectDriverPhase2 = FPGA_PWM_vGetDetectDriverPhase2();
    bDetectDriverPhase3 = FPGA_PWM_vGetDetectDriverPhase3();
    #endif

    #if BOARDCFG_USE_TESTPROC
    if (bOffsetCalcAllowSetVoltageStack1)
    {
        if ((bVoltageDischarging == 0) && (bVoltageApplied == 0))
        {
            if (bOffsetCalcAllowVoltageStack1UseAutoManipulation)
            {
                bOffsetCalcAllowVoltageStack1 = 1;

            }
        }
        else
        {
            if (bOffsetCalcAllowVoltageStack1UseAutoManipulation)
            {
                bOffsetCalcAllowVoltageStack1 = 0;
            }
        } 
    }
    #endif

    if (bAllOffsetPerformedReset)
    {
        bAllOffsetPerformedReset = 0;
        bAllOffsetPerformed = 0;
        bOffsetPerformedVoltageStack1 = 0;
        bOffsetPerformedVoltageStack2 = 0;
        bOffsetPerformedCurrentStack1 = 0;
        bOffsetPerformedCurrentStack2 = 0;
        bOffsetPerformedTempFbkStack1 = 0;
        bOffsetPerformedCurrFbkStack1 = 0;
    }

    if (sControlParameterLast.s16FieldSpeedLastStack1_dHz != sControlParameter.sStack[STACK_1].s16FieldSpeed_dHz)
    {
        sControlParameterLast.s16FieldSpeedLastStack1_dHz = sControlParameter.sStack[STACK_1].s16FieldSpeed_dHz;
        sControlData.sStack[STACK_1].s16Speed = MCLIB_s16Mul_s16u32(sControlParameter.sStack[STACK_1].s16FieldSpeed_dHz, sControlConfig.u32DeciHzToSpeedMultiplier);
    }

    if (s16RMSCurrent1PHRequestAmperes_Old != s16RMSCurrent1PHRequestAmperes) {
        s16RMSCurrent1PHRequestAmperes_Old = s16RMSCurrent1PHRequestAmperes;
        sControlData.sStack[STACK_1].s16CurrentRMSReq1Ph = MCLIB_s16Mul_s16u32(s16RMSCurrent1PHRequestAmperes, sControlConfig.u32AmperesToADCResultMultiplierRequest);
    }


    if (s16CurrentRMSReq1Ph_dA_Old != s16CurrentRMSReq1Ph_dA) {
        s16CurrentRMSReq1Ph_dA_Old = s16CurrentRMSReq1Ph_dA;
        sControlData.sStack[STACK_1].s16CurrentRMSReq1Ph = MCLIB_s16Mul_s16u32(s16CurrentRMSReq1Ph_dA, sControlConfig.u32DeciAmperesToADCResultMultiplierRequest);
    }

    if (s16RMSCurrent1PHEmulationAmperes_Old != s16RMSCurrent1PHEmulationAmperes) {
        s16RMSCurrent1PHEmulationAmperes_Old = s16RMSCurrent1PHEmulationAmperes;
        sControlData.sStack[STACK_1].s16CurrentRMS1PhFbckEmulation = MCLIB_s16Mul_s16u32(s16RMSCurrent1PHEmulationAmperes, sControlConfig.u32AmperesToADCResultMultiplierRequest);
    }



    if (sControlParameterLast.u16VoltageRequestDCLinkLast_dV != sControlParameter.u16VoltageRequestDCLink_dV)
    {
        sControlParameterLast.u16VoltageRequestDCLinkLast_dV = sControlParameter.u16VoltageRequestDCLink_dV;
        //sControlData.sStack[STACK_1].s16VoltageAmpScaled = MCLIB_s16Mul_s16u32(s16VoltageReqDCLink_dV,MCLIB_u32FractionalQ_16_16(1.0 / ((10.0 * 100.0) / 32768)));
        
        #if USE_CTREADER
        CTREADER_sh_u_dcl_set = sControlParameter.u16VoltageRequestDCLink_dV;
        #endif
    }
    else
    {
        #if USE_CTREADER
        sControlParameter.u16VoltageRequestDCLink_dV = CTREADER_sh_u_dcl_set;
        #endif
    }

    #if BOARDCFG_USE_TESTPROC
    sh_u_dcl_set = sControlParameter.u16VoltageRequestDCLink_dV;
    #endif

    #if BOARDCFG_USE_TSC_DSPIC == 0
    if (bUseHeatManagementAuto)
    {
        bUseHeatExchangerFanControlAuto = 1;
        bUseHeatExchangerPumpControlAuto = 1;
        bUseSection1FanControlAuto = 1;
        bUseSection2FanControlAuto = 1;
    }
    else
    {
        bUseHeatExchangerFanControlAuto = 0;
        bUseHeatExchangerPumpControlAuto = 0;
        bUseSection1FanControlAuto = 0;
        bUseSection2FanControlAuto = 0;


        if (bUseHeatManagementAutoOff)
        {
            BOARDCFG_vClrIO(DO_SIGNAL_HEAT_EXCHANGER_FAN_CONTROL);
            BOARDCFG_vClrIO(DO_SIGNAL_COOLING_PUMP_CONTROL);
            BOARDCFG_vClrIO(DO_SIGNAL_FANS_CONTROL_SECTION1);
            BOARDCFG_vClrIO(DO_SIGNAL_FANS_CONTROL_SECTION2);
        }
    }

    if (bUseHeatExchangerFanControlAuto)
    {
        u16HeatExchangerFanStartTemperature = (uint16_t)MCLIB_s16Mul_s16s32((((int16_t)(u16HeatExchangerFanStartTemperature_Deg << 4) - (sControlConfig.s32ADCScaleTH2ORealValueOffset >> (16-4))) >> 4), sControlConfig.s32ADCResultToTH2ORealValueDivider);
        u16HeatExchangerFanStopTemperature  = (uint16_t)MCLIB_s16Mul_s16s32((((int16_t)(u16HeatExchangerFanStopTemperature_Deg  << 4) - (sControlConfig.s32ADCScaleTH2ORealValueOffset >> (16-4))) >> 4), sControlConfig.s32ADCResultToTH2ORealValueDivider);

        //deg = adc*mult + offs;
        //adc = (deg-offs)/mult

        if ((ADCLine_1NoOffset_LLV3_H2O_IN < u16HeatExchangerFanStartTemperature)
          ||(ADCLine_2NoOffset_LLV4_H2O_OUT < u16HeatExchangerFanStartTemperature))
        {
            BOARDCFG_vSetIO(DO_SIGNAL_HEAT_EXCHANGER_FAN_CONTROL);
        }
        else
        if ((ADCLine_1NoOffset_LLV3_H2O_IN > u16HeatExchangerFanStopTemperature)
          &&(ADCLine_2NoOffset_LLV4_H2O_OUT > u16HeatExchangerFanStopTemperature))
        {
            BOARDCFG_vClrIO(DO_SIGNAL_HEAT_EXCHANGER_FAN_CONTROL);
        }
    }

    if (bUseHeatExchangerPumpControlAuto)
    {
        BOARDCFG_vSetIO(DO_SIGNAL_COOLING_PUMP_CONTROL);
    }

    if (bUseSection1FanControlAuto)
    {
        BOARDCFG_vSetIO(DO_SIGNAL_FANS_CONTROL_SECTION1);
    }

    if (bUseSection2FanControlAuto)
    {
        BOARDCFG_vSetIO(DO_SIGNAL_FANS_CONTROL_SECTION2);
    }
    #endif

}

void CONTROL_vProcess(void)
{

    DEBUG_SET_CTRL();
    
    #if BOARDCFG_USE_FPGA

    if ((bPWMEnableRequestDelay) && (bPWMEnableRequest)) {

        FPGA_COMMON_vEnablePWM();
    }
    

    if (bPWMEnableRequest) 
    {
        if(bPWMEnableRequestDelay == 0)
        {
            FPGA_COMMON_vEnablePWM();
        }
    } 
    else 
    {
        FPGA_COMMON_vDisablePWM();
    }
    #endif
    
    TESTPROC_vProcessVoltageRegulaion();


    //if (eControlState > CTRL_STA_INIT)    /* do not raise error in Init State */
    //if (eControlState >= CTRL_STA_READY)    /* do not raise error in error, error reset or Init State */
    if (eControlState >= CTRL_STA_RUN) /* do not raise error in error, error reset, Init, Ready, or Calib State */
    {
#if BOARDCFG_USE_FPGA
        if (FPGA_COMMON_bIsError())
        {
            FPGA_uErrorCode = FPGA_COMMON_uGetError();                          /* get detailed error */
            CONTROL_uStopError.sStopError.bErrorFPGA = 1;
            if(bUseControlNoErrorEmulation == 0)
            {
                sControlAccess.bFault = true;
            }
            if (FPGA_uErrorCode.sErrorCode.bTripError)
            {
                CONTROL_uStopError.sStopError.bErrorTrip = 1;

                if(bUseControlNoErrorEmulation == 0)
                {
                    sControlAccess.bFault = true;
                }
                eErrorState = CTRL_ERR_TRIP_FPGA;
                eErrorStateLast = CTRL_ERR_TRIP_FPGA;
                if (eFirstDetectedError == CTRL_ERR_NO_ERROR)
                {
                    eFirstDetectedError = CTRL_ERR_TRIP_FPGA;
                }
            }
            if (FPGA_uErrorCode.sErrorCode.bPWMError)
            {
                CONTROL_uStopError.sStopError.bErrorPWM = 1;

                if(bUseControlNoErrorEmulation == 0)
                {
                    sControlAccess.bFault = true;
                }
                eErrorState = CTRL_ERR_PWM_FPGA;
                eErrorStateLast = CTRL_ERR_PWM_FPGA;
                if (eFirstDetectedError == CTRL_ERR_NO_ERROR)
                {
                    eFirstDetectedError = CTRL_ERR_PWM_FPGA;
                }
            }
        }
#endif

        #if BOARDCFG_USE_DIO
        if (IO_COMMON_bIsError())
        {
            CONTROL_uStopError.sStopError.bErrorIO = 1;
            if((bUseControlNoErrorEmulation == 0) && (bUseIOError == 1))
            {
                sControlAccess.bFault = true;
            }
        }
        #endif

        if (uTempError.u16Register > 0)
        {
            CONTROL_uStopError.sStopError.bErrorTemp = 1;
            if(bUseControlNoErrorEmulation == 0)
            {
                sControlAccess.bFault = true;
            }
            eErrorState = CTRL_ERR_TEMPERATURE;
            eErrorStateLast = CTRL_ERR_TEMPERATURE;
            if (eFirstDetectedError == CTRL_ERR_NO_ERROR)
            {
                eFirstDetectedError = CTRL_ERR_TEMPERATURE;
            }
        }
    }

    #if !defined(BOARDCFG_USE_MCC_ADC) && (BOARDCFG_USE_OLD_NON_MCC_ADC)
    sControlData.u16SkippedPulses = u16ExecControlNewDataSkipBuffered;
    #endif

    ADC_vReadADCData();
    vCurrentVoltageFbkPrepare();
    if (sControlAccess.bEmulateVoltageFbck) 
    {
        sControlData.sStack[STACK_1].u16VoltageDCBusFbckEmulation = (uint16_t) (MCLIB_u32RampLinear(
                ((uint32_t) sControlData.sStack[STACK_1].u16VoltageDCBusFbckEmulationReq << 16),
                &sControlData.sStack[STACK_1].sVoltageDCFbckEmulationRamp) >> 16);

        #if BOARDCFG_STACK_COUNT >= 2
        sControlData.sStack[STACK_2].u16VoltageDCBusFbckEmulation = (int16_t) (MCLIB_u32RampLinear(
                ((uint32_t) sControlData.sStack[STACK_2].u16VoltageDCBusFbckEmulationReq << 16),
                &sControlData.sStack[STACK_2].sVoltageDCFbckEmulationRamp) >> 16);
        #endif

    }

    SpeedStack1 = MCLIB_s16Mul_s16u16(sControlData.sStack[STACK_1].s16Speed,
            sControlConfig.u16SpeedToDeciHzMultiplier);
    #if BOARDCFG_STACK_COUNT >= 2
    SpeedStack2 = MCLIB_s16Mul_s16u16(sControlData.sStack[STACK_2].s16Speed,
            sControlConfig.u16SpeedToDeciHzMultiplier);
    #endif


    //DEBUG_CLR_CTRL();
  
    apfControlState[eControlState]();
    //DEBUG_SET_CTRL();
    
    
    
    if (sControlAccess.bErrorClearForced)
    {
        
        if (eControlState != CTRL_STA_FAULT)
        {
            sControlAccess.bErrorClearForced = 0;
            sControlAccess.bClearFaults = 0;

            sControlAccess.bFault = 0;

            eErrorState = CTRL_ERR_NO_INIT;
            //eErrorStateLast = CTRL_ERR_NO_INIT;
            eFirstDetectedError = CTRL_ERR_NO_INIT;
        }

        #if BOARDCFG_USE_FPGA
        FPGA_COMMON_vResetEnable(); /* error FPGA Clear */
        #endif
        CONTROL_uStopError.u16Register = 0;
        uTempError.u16Register = 0;
    }



#if BOARDCFG_USE_DAC_AUTOMATION
    DAC_vProcessAutomation(eControlState == CTRL_STA_READY);
#endif




    //CurrentStack1[0] = MCLIB_s16Mul_s16u16(sControlData.sStack[STACK_1].sCurrentABCFbck.s16Arg1, sControlConfig.u16ADCResultToDeciAmperesMultVisual) << sControlConfig.u16ADCResultToDeciAmperesShftVisual;
    //CurrentStack1[1] = MCLIB_s16Mul_s16u16(sControlData.sStack[STACK_1].sCurrentABCFbck.s16Arg2, sControlConfig.u16ADCResultToDeciAmperesMultVisual) << sControlConfig.u16ADCResultToDeciAmperesShftVisual;
    //CurrentStack1[2] = MCLIB_s16Mul_s16u16(sControlData.sStack[STACK_1].sCurrentABCFbck.s16Arg3, sControlConfig.u16ADCResultToDeciAmperesMultVisual) << sControlConfig.u16ADCResultToDeciAmperesShftVisual;

    CurrentStack1[0] = MCLIB_s16Mul_s16u16(ADCStack1NoOffset[ADC_STACK_CURRENT_L1], sControlConfig.u16ADCResultToDeciAmperesMult) << sControlConfig.u16ADCResultToDeciAmperesShft;
    CurrentStack1[1] = MCLIB_s16Mul_s16u16(ADCStack1NoOffset[ADC_STACK_CURRENT_L2], sControlConfig.u16ADCResultToDeciAmperesMult) << sControlConfig.u16ADCResultToDeciAmperesShft;
    CurrentStack1[2] = MCLIB_s16Mul_s16u16(ADCStack1NoOffset[ADC_STACK_CURRENT_L3], sControlConfig.u16ADCResultToDeciAmperesMult) << sControlConfig.u16ADCResultToDeciAmperesShft;

    CurrentStack2[0] = MCLIB_s16Mul_s16u16(ADCStack2NoOffset[ADC_STACK_CURRENT_L1], sControlConfig.u16ADCResultToDeciAmperesMult) << sControlConfig.u16ADCResultToDeciAmperesShft;
    CurrentStack2[1] = MCLIB_s16Mul_s16u16(ADCStack2NoOffset[ADC_STACK_CURRENT_L2], sControlConfig.u16ADCResultToDeciAmperesMult) << sControlConfig.u16ADCResultToDeciAmperesShft;
    CurrentStack2[2] = MCLIB_s16Mul_s16u16(ADCStack2NoOffset[ADC_STACK_CURRENT_L3], sControlConfig.u16ADCResultToDeciAmperesMult) << sControlConfig.u16ADCResultToDeciAmperesShft;

    //Voltage unsigned
#if BOARDCFG_USE_TSC_DSPIC
    /* VoltageStack1 -> DC-Link */
    VoltageStack1 = MCLIB_u16Mul_u16u16(MCLIB_u16From_s16(ADCStack1NoOffset[ADC_STACK_DC_VOLTAGE]), sControlConfig.u16ADCResultToDeciVoltageMultiplier);
    /* VoltageStack2 -> Spare Voltage */
    VoltageStack2 = MCLIB_u16Mul_u16u16(MCLIB_u16From_s16(ADCStack2NoOffset[ADC_STACK_DC_VOLTAGE]), sControlConfig.u16ADCResultToDeciVoltageMultiplier);
#else
    /* VoltageStack1 -> DC-Link */
    VoltageStack1 = MCLIB_u16Mul_u16u16((uint16_t)(ADCDCLinkNoOffset[ADC_STACK_DC_VOLTAGE]), sControlConfig.u16ADCResultToDeciVoltageMultiplier);
    /* VoltageStack2 -> PV */
    VoltageStack2 = MCLIB_u16Mul_u16u16((uint16_t)(ADCStack1NoOffset[ADC_STACK_DC_VOLTAGE]), sControlConfig.u16ADCResultToDeciVoltageMultiplier);
#endif

    //    if (sControlAccess.bEmulateVoltageFbck)
    //    {
    //        VoltageStack1    = MCLIB_u16Mul_u16u16(sControlData.sStack[STACK_1].u16VoltageDCBusFbckEmulation,sControlConfig.u16ADCResultToDeciVoltageMultiplier);
    //        VoltageStack2    = MCLIB_u16Mul_u16u16(sControlData.sStack[STACK_2].u16VoltageDCBusFbckEmulation,sControlConfig.u16ADCResultToDeciVoltageMultiplier);
    //    }

    CurrFbkStack1[0] = MCLIB_s16Mul_s16u16(ADC2Stack1NoOffset[ADC2_STACK_CURRFBK_L1], sControlConfig.u16ADCResultToDeciAmperesMultiplierCFBK);
    CurrFbkStack1[1] = MCLIB_s16Mul_s16u16(ADC2Stack1NoOffset[ADC2_STACK_CURRFBK_L2], sControlConfig.u16ADCResultToDeciAmperesMultiplierCFBK);
    CurrFbkStack1[2] = MCLIB_s16Mul_s16u16(ADC2Stack1NoOffset[ADC2_STACK_CURRFBK_L3], sControlConfig.u16ADCResultToDeciAmperesMultiplierCFBK);

    CurrFbkStack2[0] = MCLIB_s16Mul_s16u16(ADC2Stack2NoOffset[ADC2_STACK_CURRFBK_L1], sControlConfig.u16ADCResultToDeciAmperesMultiplierCFBK);
    CurrFbkStack2[1] = MCLIB_s16Mul_s16u16(ADC2Stack2NoOffset[ADC2_STACK_CURRFBK_L2], sControlConfig.u16ADCResultToDeciAmperesMultiplierCFBK);
    CurrFbkStack2[2] = MCLIB_s16Mul_s16u16(ADC2Stack2NoOffset[ADC2_STACK_CURRFBK_L3], sControlConfig.u16ADCResultToDeciAmperesMultiplierCFBK);

    TempFbkStack1[0] = MCLIB_s16Mul_s16u16(ADC2Stack1NoOffset[ADC2_STACK_TEMPFBK_L1], sControlConfig.u16ADCResultToDeciDegreesMultiplierTFBK);
    TempFbkStack1[1] = MCLIB_s16Mul_s16u16(ADC2Stack1NoOffset[ADC2_STACK_TEMPFBK_L2], sControlConfig.u16ADCResultToDeciDegreesMultiplierTFBK);
    TempFbkStack1[2] = MCLIB_s16Mul_s16u16(ADC2Stack1NoOffset[ADC2_STACK_TEMPFBK_L3], sControlConfig.u16ADCResultToDeciDegreesMultiplierTFBK);

    TempFbkStack1[0] += sControlConfig.s16ADCResultToDeciDegreesOffsetTFBK;
    TempFbkStack1[1] += sControlConfig.s16ADCResultToDeciDegreesOffsetTFBK;
    TempFbkStack1[2] += sControlConfig.s16ADCResultToDeciDegreesOffsetTFBK;

    TempFbkStack2[0] = MCLIB_s16Mul_s16u16(ADC2Stack2NoOffset[ADC2_STACK_TEMPFBK_L1], sControlConfig.u16ADCResultToDeciDegreesMultiplierTFBK);
    TempFbkStack2[1] = MCLIB_s16Mul_s16u16(ADC2Stack2NoOffset[ADC2_STACK_TEMPFBK_L2], sControlConfig.u16ADCResultToDeciDegreesMultiplierTFBK);
    TempFbkStack2[2] = MCLIB_s16Mul_s16u16(ADC2Stack2NoOffset[ADC2_STACK_TEMPFBK_L3], sControlConfig.u16ADCResultToDeciDegreesMultiplierTFBK);

    TempFbkStack2[0] += sControlConfig.s16ADCResultToDeciDegreesOffsetTFBK;
    TempFbkStack2[1] += sControlConfig.s16ADCResultToDeciDegreesOffsetTFBK;
    TempFbkStack2[2] += sControlConfig.s16ADCResultToDeciDegreesOffsetTFBK;

    //Voltage2 signed
    Voltage2Stack1 = MCLIB_s16Mul_s16u16(ADC2Stack1NoOffset[ADC2_STACK_DCVOLTAGE2], sControlConfig.u16ADCResultToDeciVoltageMultiplier);
    Voltage2Stack2 = MCLIB_s16Mul_s16u16(ADC2Stack2NoOffset[ADC2_STACK_DCVOLTAGE2], sControlConfig.u16ADCResultToDeciVoltageMultiplier);





    u16_irms_sns1 = MCLIB_s16Mul_s16u16(sControlData.sStack[STACK_1].sCurrentABCFbckRMSFilt.s16Arg1, sControlConfig.u16ADCResultToDeciAmperesMultVisual) << sControlConfig.u16ADCResultToDeciAmperesShftVisual;
    u16_irms_sns2 = MCLIB_s16Mul_s16u16(sControlData.sStack[STACK_1].sCurrentABCFbckRMSFilt.s16Arg2, sControlConfig.u16ADCResultToDeciAmperesMultVisual) << sControlConfig.u16ADCResultToDeciAmperesShftVisual;
    u16_irms_sns3 = MCLIB_s16Mul_s16u16(sControlData.sStack[STACK_1].sCurrentABCFbckRMSFilt.s16Arg3, sControlConfig.u16ADCResultToDeciAmperesMultVisual) << sControlConfig.u16ADCResultToDeciAmperesShftVisual;

    u16_irms_mod1 = MCLIB_s16Mul_s16u16(sControlData.sStack[STACK_1].sCFBKABCFbckRMSFilt.s16Arg1, sControlConfig.u16ADCResultToDeciAmperesMultiplierCFBK);
    u16_irms_mod2 = MCLIB_s16Mul_s16u16(sControlData.sStack[STACK_1].sCFBKABCFbckRMSFilt.s16Arg2, sControlConfig.u16ADCResultToDeciAmperesMultiplierCFBK);
    u16_irms_mod3 = MCLIB_s16Mul_s16u16(sControlData.sStack[STACK_1].sCFBKABCFbckRMSFilt.s16Arg3, sControlConfig.u16ADCResultToDeciAmperesMultiplierCFBK);

    u16_urms_filt = MCLIB_u16Mul_u16u16(sControlData.sStack[STACK_1].u16VoltageDCBusRMSFilt, sControlConfig.u16ADCResultToDeciVoltageMultiplier);

    #if BOARDCFG_USE_INFO_CALC_MIN_MAX_AVG
    if (bCalculateMinMax)
    {
        CurrentStack1Min[0] = MCLIB_s16Min(CurrentStack1Min[0], CurrentStack1[0]);
        CurrentStack1Min[1] = MCLIB_s16Min(CurrentStack1Min[1], CurrentStack1[1]);
        CurrentStack1Min[2] = MCLIB_s16Min(CurrentStack1Min[2], CurrentStack1[2]);

        CurrentStack2Min[0] = MCLIB_s16Min(CurrentStack2Min[0], CurrentStack2[0]);
        CurrentStack2Min[1] = MCLIB_s16Min(CurrentStack2Min[1], CurrentStack2[1]);
        CurrentStack2Min[2] = MCLIB_s16Min(CurrentStack2Min[2], CurrentStack2[2]);

        VoltageStack1Min = MCLIB_u16Min(VoltageStack1Min, VoltageStack1);
        VoltageStack2Min = MCLIB_s16Min(VoltageStack2Min, VoltageStack2);
        Voltage2Stack1Min = MCLIB_s16Min(Voltage2Stack1Min, Voltage2Stack1);
        Voltage2Stack2Min = MCLIB_s16Min(Voltage2Stack2Min, Voltage2Stack2);

        CurrFbkStack1Min[0] = MCLIB_s16Min(CurrFbkStack1Min[0], CurrFbkStack1[0]);
        CurrFbkStack1Min[1] = MCLIB_s16Min(CurrFbkStack1Min[1], CurrFbkStack1[1]);
        CurrFbkStack1Min[2] = MCLIB_s16Min(CurrFbkStack1Min[2], CurrFbkStack1[2]);

        TempFbkStack1Min[0] = MCLIB_s16Min(TempFbkStack1Min[0], TempFbkStack1[0]);
        TempFbkStack1Min[1] = MCLIB_s16Min(TempFbkStack1Min[1], TempFbkStack1[1]);
        TempFbkStack1Min[2] = MCLIB_s16Min(TempFbkStack1Min[2], TempFbkStack1[2]);



        CurrentStack1Max[0] = MCLIB_s16Max(CurrentStack1Max[0], CurrentStack1[0]);
        CurrentStack1Max[1] = MCLIB_s16Max(CurrentStack1Max[1], CurrentStack1[1]);
        CurrentStack1Max[2] = MCLIB_s16Max(CurrentStack1Max[2], CurrentStack1[2]);

        CurrentStack2Max[0] = MCLIB_s16Max(CurrentStack2Max[0], CurrentStack2[0]);
        CurrentStack2Max[1] = MCLIB_s16Max(CurrentStack2Max[1], CurrentStack2[1]);
        CurrentStack2Max[2] = MCLIB_s16Max(CurrentStack2Max[2], CurrentStack2[2]);

        VoltageStack1Max = MCLIB_u16Max(VoltageStack1Max, VoltageStack1);
        VoltageStack2Max = MCLIB_s16Max(VoltageStack2Max, VoltageStack2);
        Voltage2Stack1Max = MCLIB_s16Max(Voltage2Stack1Max, Voltage2Stack1);
        Voltage2Stack2Max = MCLIB_s16Max(Voltage2Stack2Max, Voltage2Stack2);

        CurrFbkStack1Max[0] = MCLIB_s16Max(CurrFbkStack1Max[0], CurrFbkStack1[0]);
        CurrFbkStack1Max[1] = MCLIB_s16Max(CurrFbkStack1Max[1], CurrFbkStack1[1]);
        CurrFbkStack1Max[2] = MCLIB_s16Max(CurrFbkStack1Max[2], CurrFbkStack1[2]);

        TempFbkStack1Max[0] = MCLIB_s16Max(TempFbkStack1Max[0], TempFbkStack1[0]);
        TempFbkStack1Max[1] = MCLIB_s16Max(TempFbkStack1Max[1], TempFbkStack1[1]);
        TempFbkStack1Max[2] = MCLIB_s16Max(TempFbkStack1Max[2], TempFbkStack1[2]);

        if (bCalculateMinMaxReset) {
            bCalculateMinMaxReset = 0;

            CurrentStack1Min[0] = CurrentStack1[0];
            CurrentStack1Min[1] = CurrentStack1[1];
            CurrentStack1Min[2] = CurrentStack1[2];

            CurrentStack2Min[0] = CurrentStack2[0];
            CurrentStack2Min[1] = CurrentStack2[1];
            CurrentStack2Min[2] = CurrentStack2[2];

            VoltageStack1Min = VoltageStack1;
            VoltageStack2Min = VoltageStack2;
            Voltage2Stack1Min = Voltage2Stack1;
            Voltage2Stack2Min = Voltage2Stack2;

            CurrFbkStack1Min[0] = CurrFbkStack1[0];
            CurrFbkStack1Min[1] = CurrFbkStack1[1];
            CurrFbkStack1Min[2] = CurrFbkStack1[2];

            TempFbkStack1Min[0] = TempFbkStack1[0];
            TempFbkStack1Min[1] = TempFbkStack1[1];
            TempFbkStack1Min[2] = TempFbkStack1[2];



            CurrentStack1Max[0] = CurrentStack1[0];
            CurrentStack1Max[1] = CurrentStack1[1];
            CurrentStack1Max[2] = CurrentStack1[2];

            CurrentStack2Max[0] = CurrentStack2[0];
            CurrentStack2Max[1] = CurrentStack2[1];
            CurrentStack2Max[2] = CurrentStack2[2];

            VoltageStack1Max = VoltageStack1;
            VoltageStack2Max = VoltageStack2;
            Voltage2Stack1Max = Voltage2Stack1;
            Voltage2Stack2Max = Voltage2Stack2;

            CurrFbkStack1Max[0] = CurrFbkStack1[0];
            CurrFbkStack1Max[1] = CurrFbkStack1[1];
            CurrFbkStack1Max[2] = CurrFbkStack1[2];

            TempFbkStack1Max[0] = TempFbkStack1[0];
            TempFbkStack1Max[1] = TempFbkStack1[1];
            TempFbkStack1Max[2] = TempFbkStack1[2];

        }
    }
    #endif

    
#if HMDRV_USE_PROCESS_ALL_RECORDERS

    #if (HMDRV_USE_RECORDER_PARAMETERS) && (!(HMDRV_DISABLE))
    #if HMDRV_RECORDER_PARAM_HANDLE_COUNT > 0
    CSMON_vRecorderParamStartStopProcess();
    #endif
    #endif

    HMLIB_vProcessRecorders();

#else

#if HMDRV_USE_RECORDER
    #if HMDRV_RECORDER_HANDLE_COUNT > 0
    HMLIB_vRecorder(0);
    #endif
    #if HMDRV_RECORDER_HANDLE_COUNT > 1
    HMLIB_vRecorder(1);
    #endif
    #if HMDRV_RECORDER_HANDLE_COUNT > 2
    HMLIB_vRecorder(2);
    #endif

#endif

#if HMDRV_USE_RECORDER_PARAMETERS
    #if HMDRV_RECORDER_PARAM_HANDLE_COUNT > 0
    HMLIB_vRecorderParam(0);
    #endif
    #if HMDRV_RECORDER_PARAM_HANDLE_COUNT > 1
    HMLIB_vRecorderParam(1);
    #endif
    #if HMDRV_RECORDER_PARAM_HANDLE_COUNT > 2
    HMLIB_vRecorderParam(2);
    #endif
    HMLIB_vRecorderParamTimeSync();
#endif

#endif


#if BOARDCFG_USE_FPGA
    if (bPWMEnableRequest == 0) 
    {
        FPGA_COMMON_vDisablePWM();
    }
#endif
    
    DEBUG_CLR_CTRL();

}

void vStateInit(void) {
    ADC_vOffsetBufferInit();

    CONTROL_vReset();

    sControlData.u32CalibCounter = 0;

    eControlState = CTRL_STA_READY;
    eErrorState = CTRL_ERR_NO_ERROR;
    //eErrorStateLast = CTRL_ERR_NO_ERROR;
    eFirstDetectedError = CTRL_ERR_NO_ERROR;

    sControlData.sStack[STACK_1].sCurrentABCFbckRMSFilt.s16Arg1 = 0;
    sControlData.sStack[STACK_1].sCurrentABCFbckRMSFilt.s16Arg2 = 0;
    sControlData.sStack[STACK_1].sCurrentABCFbckRMSFilt.s16Arg3 = 0;

    sControlData.sStack[STACK_1].sCFBKABCFbckRMSFilt.s16Arg1 = 0;
    sControlData.sStack[STACK_1].sCFBKABCFbckRMSFilt.s16Arg2 = 0;
    sControlData.sStack[STACK_1].sCFBKABCFbckRMSFilt.s16Arg3 = 0;

    sControlData.sStack[STACK_1].u16VoltageDCBusRMSFilt = 0;

    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCurrentABCFbckRMSFilt.s16Arg1 = 0;
    sControlData.sStack[STACK_2].sCurrentABCFbckRMSFilt.s16Arg2 = 0;
    sControlData.sStack[STACK_2].sCurrentABCFbckRMSFilt.s16Arg3 = 0;

    sControlData.sStack[STACK_2].sCFBKABCFbckRMSFilt.s16Arg1 = 0;
    sControlData.sStack[STACK_2].sCFBKABCFbckRMSFilt.s16Arg2 = 0;
    sControlData.sStack[STACK_2].sCFBKABCFbckRMSFilt.s16Arg3 = 0;

    sControlData.sStack[STACK_2].u16VoltageDCBusRMSFilt = 0;
    #endif

#if BOARDCFG_USE_FPGA
    FPGA_COMMON_vResetRelease();
#endif
}

void vStateFault(void) {
    if (sControlAccess.bClearFaults) {
        sControlAccess.bClearFaults = 0;

        sControlAccess.bFault = 0;

        eErrorState = CTRL_ERR_NO_INIT;
        //eErrorStateLast = CTRL_ERR_NO_INIT;

        uTempError.u16Register = 0;

        eFirstDetectedError = CTRL_ERR_NO_INIT;

#if BOARDCFG_USE_FPGA
        FPGA_COMMON_vResetEnable(); /* error FPGA Clear */
#endif

        CONTROL_uStopError.u16Register = 0;


        /* (clear fault) */
        eControlState = CTRL_STA_FAULT_RESET;
        u16FaultResetDelay = u16FaultResetTimeout;

    } else {
        if (bStartupFlag) {
            bStartupFlag = false;
            sControlAccess.bClearFaults = 1;
        }

    }
    
}

void vStateFaultReset(void) {
    if (u16FaultResetDelay > 0) {
        u16FaultResetDelay--;
    }
    if (u16FaultResetDelay == 0) {
        /* reset actuators */
        eControlState = CTRL_STA_INIT;
    }
}

void vStateReady(void) {
    //    DACOffsetCurrentStack1[0] = MCLIB_s16Mul_s16u32(ADCStack1Offset[ADC_STACK_CURRENT_L1],
    //            sControlConfig.u32ADCResultToDACResultAmperesMultiplier);
    //    DACOffsetCurrentStack1[1] = MCLIB_s16Mul_s16u32(ADCStack1Offset[ADC_STACK_CURRENT_L2],
    //            sControlConfig.u32ADCResultToDACResultAmperesMultiplier);
    //    DACOffsetCurrentStack1[2] = MCLIB_s16Mul_s16u32(ADCStack1Offset[ADC_STACK_CURRENT_L3],
    //            sControlConfig.u32ADCResultToDACResultAmperesMultiplier);
    //    
    //    DACOffsetVoltageStack1    = MCLIB_s16Mul_s16u32(ADCStack1Offset[ADC_STACK_DC_VOLTAGE],
    //            sControlConfig.u32ADCResultToDACResultVoltageMultiplier);
    //    
    //    DACOffsetCurrentStack2[0] = MCLIB_s16Mul_s16u32(ADCStack2Offset[ADC_STACK_CURRENT_L1],
    //            sControlConfig.u32ADCResultToDACResultAmperesMultiplier);
    //    DACOffsetCurrentStack2[1] = MCLIB_s16Mul_s16u32(ADCStack2Offset[ADC_STACK_CURRENT_L2],
    //            sControlConfig.u32ADCResultToDACResultAmperesMultiplier);
    //    DACOffsetCurrentStack2[2] = MCLIB_s16Mul_s16u32(ADCStack2Offset[ADC_STACK_CURRENT_L3],
    //            sControlConfig.u32ADCResultToDACResultAmperesMultiplier);
    //    
    //    DACOffsetVoltageStack2    = MCLIB_s16Mul_s16u32(ADCStack2Offset[ADC_STACK_DC_VOLTAGE],
    //            sControlConfig.u32ADCResultToDACResultVoltageMultiplier);
    //


#if BOARDCFG_USE_FPGA
    FPGA_COMMON_vResetRelease();
#endif

    if (sControlAccess.bStart) {
        /* start actuators during calibration if needed for better calibration */
        if (bCalibrationWithPWMEnabled)
        {
            PWM_vOutputEnable();
            #if BOARDCFG_USE_FPGA
            //FPGA_COMMON_vEnablePWM();was commented - should be commanded by bPWMEnableRequest
            #endif
            bPWMEnableRequest = 1;
        }
        
        eControlState = CTRL_STA_CALIB;
        
        bStartupFlag = false;
        sControlAccess.bClearFaults = 0;
        CSMON_vSetSetupRecorderParameterMask(CTRL_u8MaskSetupRecorderRunModeRequest);
    }


    if (sControlAccess.bStartNoCalib) {
        sControlAccess.bStart = 1; /* stop with this bit */
        sControlAccess.bStartNoCalib = 0;
        
        CSMON_vSetSetupRecorderParameterMask(CTRL_u8MaskSetupRecorderRunModeRequest);

        /* start actuators after calibration if needed */
        PWM_vOutputEnable();
        #if BOARDCFG_USE_FPGA
        //FPGA_COMMON_vEnablePWM(); was commented - should be commanded by bPWMEnableRequest
        #endif
        bPWMEnableRequest = 1;

        if (bUseRunDelay) {
            eControlState = CTRL_STA_RUN_DELAY;
        } else {
            eControlState = CTRL_STA_RUN;
            CSMON_vSetStartRecorderParameterMask(CTRL_u8MaskStartRecorderRunModeEnter);
            vStackPrescalersInit();
        }
        vRunInitialize();
        
        bStartupFlag = false;
        sControlAccess.bClearFaults = 0;

    }

    if (sControlAccess.bStartCalibOnly) {
        /* start actuators during calibration if needed for better calibration */
        if (bCalibrationWithPWMEnabled)
        {
            PWM_vOutputEnable();
            #if BOARDCFG_USE_FPGA
            //FPGA_COMMON_vEnablePWM(); was commented - should be commanded by bPWMEnableRequest
            #endif
            bPWMEnableRequest = 1;
        }
        eControlState = CTRL_STA_CALIB;
        
        CSMON_vSetSetupRecorderParameterMask(CTRL_u8MaskSetupRecorderRunModeRequest);

        sControlAccess.bStart = 1; /* stop with this bit */
        //sControlAccess.bUseCalibrationOnly = 1;
        
        bStartupFlag = false;
        sControlAccess.bClearFaults = 0;

    }



    if (sControlAccess.bFault == 1) {
        /* stop actuators */
        eControlState = CTRL_STA_FAULT;
#if BOARDCFG_USE_FPGA
        FPGA_COMMON_vDisablePWM();
#endif
        PWM_vOutputDisable();
        bPWMEnableRequest = 0;
    }








    #if BOARDCFG_USE_TSC_DSPIC == 0

    if (bUseStartTestAuto)
    {
        if ((sControlAccess.bStartCalibOnly == 0) && (sControlAccess.bStart == 1))
        {
            bSwitchMainContactors = 1;
            bUseHeatManagementAuto = 1;
        }
        else
        {
            //bSwitchMainContactors = 0;
        }
        if (bSwitchMainContactors)
        {
            if (CONTROL_uStackMask.sStackUsageMask.bStack1)
            {
                BOARDCFG_vSetIO(DO_SIGNAL_AC1_CONTACTOR_CONTROL);
            }
            else
            {
                BOARDCFG_vClrIO(DO_SIGNAL_AC1_CONTACTOR_CONTROL);
            }

            if (CONTROL_uStackMask.sStackUsageMask.bStack2)
            {
                BOARDCFG_vSetIO(DO_SIGNAL_AC2_CONTACTOR_CONTROL);
            }
            else
            {
                BOARDCFG_vClrIO(DO_SIGNAL_AC2_CONTACTOR_CONTROL);
            }

            if (CONTROL_uStackMask.sStackUsageMask.bStack3)
            {
                BOARDCFG_vSetIO(DO_SIGNAL_AC3_CONTACTOR_CONTROL);
            }
            else
            {
                BOARDCFG_vClrIO(DO_SIGNAL_AC3_CONTACTOR_CONTROL);
            }
        }
        else
        {
            BOARDCFG_vClrIO(DO_SIGNAL_AC1_CONTACTOR_CONTROL);
            BOARDCFG_vClrIO(DO_SIGNAL_AC2_CONTACTOR_CONTROL);
            BOARDCFG_vClrIO(DO_SIGNAL_AC3_CONTACTOR_CONTROL);
        }

    }
    else
    {

    }

    #endif









}


void vStackPrescalersInit(void)
{
    sControlData.sStack[STACK_1].u16ControlPrescalerCounter = 0;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].u16ControlPrescalerCounter = 1;
    #endif
}



void vRunInitialize(void) {
    sControlData.u32RunDelayCounter = ((uint32_t) 1 << sControlData.u16VoltageRunFilterBits)*6; /* 6*time of voltage run filter */


    /* FilterBits */

    /* DC Bus Voltage Filter */
    sControlData.sStack[STACK_1].sVoltageDCBusFilt.u16NSamples =
            sControlData.u16VoltageRunFilterBits;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sVoltageDCBusFilt.u16NSamples =
            sControlData.u16VoltageRunFilterBits;
    #endif
    /* sCurrentAmpFbckFilt */
    sControlData.sStack[STACK_1].sCurrentAmpFbckFilt.u16NSamples =
            sControlData.u16CurrentRunFilterBits;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCurrentAmpFbckFilt.u16NSamples =
            sControlData.u16CurrentRunFilterBits;
    #endif

//#if USE_NEW_RMS

    MCLIB_u32FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_1].sVoltageDCBusRMSFiltNew);
    #if BOARDCFG_STACK_COUNT >= 2
    MCLIB_u32FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_2].sVoltageDCBusRMSFiltNew);
    #endif
    
    MCLIB_s32FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_1].sCurrentAFbckRMSFiltNew);
    #if BOARDCFG_STACK_COUNT >= 2
    MCLIB_s32FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_2].sCurrentAFbckRMSFiltNew);
    #endif
    MCLIB_s32FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_1].sCurrentBFbckRMSFiltNew);
    #if BOARDCFG_STACK_COUNT >= 2
    MCLIB_s32FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_2].sCurrentBFbckRMSFiltNew);
    #endif
    MCLIB_s32FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_1].sCurrentCFbckRMSFiltNew);
    #if BOARDCFG_STACK_COUNT >= 2
    MCLIB_s32FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_2].sCurrentCFbckRMSFiltNew);
    #endif
    
    MCLIB_s32FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_1].sCFBKAFbckRMSFiltNew);
    #if BOARDCFG_STACK_COUNT >= 2
    MCLIB_s32FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_2].sCFBKAFbckRMSFiltNew);
    #endif
    MCLIB_s32FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_1].sCFBKBFbckRMSFiltNew);
    #if BOARDCFG_STACK_COUNT >= 2
    MCLIB_s32FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_2].sCFBKBFbckRMSFiltNew);
    #endif
    MCLIB_s32FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_1].sCFBKCFbckRMSFiltNew);
    #if BOARDCFG_STACK_COUNT >= 2
    MCLIB_s32FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_2].sCFBKCFbckRMSFiltNew);
    #endif
    
    
    MCLIB_u16FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_1].sVoltageDCBusRMSFiltNewPass2);
    #if BOARDCFG_STACK_COUNT >= 2
    MCLIB_u16FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_2].sVoltageDCBusRMSFiltNewPass2);
    #endif
    
    MCLIB_s16FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_1].sCurrentAFbckRMSFiltNewPass2);
    #if BOARDCFG_STACK_COUNT >= 2
    MCLIB_s16FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_2].sCurrentAFbckRMSFiltNewPass2);
    #endif
    MCLIB_s16FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_1].sCurrentBFbckRMSFiltNewPass2);
    #if BOARDCFG_STACK_COUNT >= 2
    MCLIB_s16FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_2].sCurrentBFbckRMSFiltNewPass2);
    #endif
    MCLIB_s16FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_1].sCurrentCFbckRMSFiltNewPass2);
    #if BOARDCFG_STACK_COUNT >= 2
    MCLIB_s16FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_2].sCurrentCFbckRMSFiltNewPass2);
    #endif
    
    MCLIB_s16FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_1].sCFBKAFbckRMSFiltNewPass2);
    #if BOARDCFG_STACK_COUNT >= 2
    MCLIB_s16FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_2].sCFBKAFbckRMSFiltNewPass2);
    #endif
    MCLIB_s16FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_1].sCFBKBFbckRMSFiltNewPass2);
    #if BOARDCFG_STACK_COUNT >= 2
    MCLIB_s16FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_2].sCFBKBFbckRMSFiltNewPass2);
    #endif
    MCLIB_s16FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_1].sCFBKCFbckRMSFiltNewPass2);
    #if BOARDCFG_STACK_COUNT >= 2
    MCLIB_s16FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_2].sCFBKCFbckRMSFiltNewPass2);
    #endif
    
    MCLIB_s16FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_1].sCFBKAFbckFilterAvg);
    #if BOARDCFG_STACK_COUNT >= 2
    MCLIB_s16FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_2].sCFBKAFbckFilterAvg);
    #endif
    MCLIB_s16FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_1].sCFBKBFbckFilterAvg);
    #if BOARDCFG_STACK_COUNT >= 2
    MCLIB_s16FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_2].sCFBKBFbckFilterAvg);
    #endif
    MCLIB_s16FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_1].sCFBKCFbckFilterAvg);
    #if BOARDCFG_STACK_COUNT >= 2
    MCLIB_s16FilterAvgInit(sControlData.u16RunRmsFilterBits, &sControlData.sStack[STACK_2].sCFBKCFbckFilterAvg);
    #endif

    
    
   

    
    
    
#if 0
    sControlData.sStack[STACK_1].sVoltageDCBusRMSFiltNew.u16NSamples =
            sControlData.u16RunRmsFilterBits;
    sControlData.sStack[STACK_2].sVoltageDCBusRMSFiltNew.u16NSamples =
            sControlData.u16RunRmsFilterBits;

    sControlData.sStack[STACK_1].sCurrentAFbckRMSFiltNew.u16NSamples =
            sControlData.u16RunRmsFilterBits;
    sControlData.sStack[STACK_2].sCurrentAFbckRMSFiltNew.u16NSamples =
            sControlData.u16RunRmsFilterBits;
    sControlData.sStack[STACK_1].sCurrentBFbckRMSFiltNew.u16NSamples =
            sControlData.u16RunRmsFilterBits;
    sControlData.sStack[STACK_2].sCurrentBFbckRMSFiltNew.u16NSamples =
            sControlData.u16RunRmsFilterBits;
    sControlData.sStack[STACK_1].sCurrentCFbckRMSFiltNew.u16NSamples =
            sControlData.u16RunRmsFilterBits;
    sControlData.sStack[STACK_2].sCurrentCFbckRMSFiltNew.u16NSamples =
            sControlData.u16RunRmsFilterBits;

    sControlData.sStack[STACK_1].sCFBKAFbckRMSFiltNew.u16NSamples =
            sControlData.u16RunRmsFilterBits;
    sControlData.sStack[STACK_2].sCFBKAFbckRMSFiltNew.u16NSamples =
            sControlData.u16RunRmsFilterBits;
    sControlData.sStack[STACK_1].sCFBKBFbckRMSFiltNew.u16NSamples =
            sControlData.u16RunRmsFilterBits;
    sControlData.sStack[STACK_2].sCFBKBFbckRMSFiltNew.u16NSamples =
            sControlData.u16RunRmsFilterBits;
    sControlData.sStack[STACK_1].sCFBKCFbckRMSFiltNew.u16NSamples =
            sControlData.u16RunRmsFilterBits;
    sControlData.sStack[STACK_2].sCFBKCFbckRMSFiltNew.u16NSamples =
            sControlData.u16RunRmsFilterBits;
#endif
//#else
    //remove code below after testing
    /*sCurrentAFbckFilter */
    sControlData.sStack[STACK_1].sCurrentAFbckFilter.u16NSamples =
            sControlData.u16CurrentRunFilterBits;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCurrentAFbckFilter.u16NSamples =
            sControlData.u16CurrentRunFilterBits;
    #endif
    /*sCurrentBFbckFilter */
    sControlData.sStack[STACK_1].sCurrentBFbckFilter.u16NSamples =
            sControlData.u16CurrentRunFilterBits;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCurrentBFbckFilter.u16NSamples =
            sControlData.u16CurrentRunFilterBits;
    #endif
    /*sCurrentCFbckFilter */
    sControlData.sStack[STACK_1].sCurrentCFbckFilter.u16NSamples =
            sControlData.u16CurrentRunFilterBits;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCurrentCFbckFilter.u16NSamples =
            sControlData.u16CurrentRunFilterBits;
    #endif
    /*sCurrentAFbckRMSFilter */
    sControlData.sStack[STACK_1].sCurrentAFbckRMSFilter.u16NSamples =
            sControlData.u16CurrentRunFilterBits;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCurrentAFbckRMSFilter.u16NSamples =
            sControlData.u16CurrentRunFilterBits;
    #endif
    /*sCurrentBFbckRMSFilter */
    sControlData.sStack[STACK_1].sCurrentBFbckRMSFilter.u16NSamples =
            sControlData.u16CurrentRunFilterBits;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCurrentBFbckRMSFilter.u16NSamples =
            sControlData.u16CurrentRunFilterBits;
    #endif
    /*sCurrentCFbckRMSFilter */
    sControlData.sStack[STACK_1].sCurrentCFbckRMSFilter.u16NSamples =
            sControlData.u16CurrentRunFilterBits;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCurrentCFbckRMSFilter.u16NSamples =
            sControlData.u16CurrentRunFilterBits;
    #endif


    /*sCFBKAFbckFilter */
    sControlData.sStack[STACK_1].sCFBKAFbckFilter.u16NSamples =
            sControlData.u16CFBKRunFilterBits;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCFBKAFbckFilter.u16NSamples =
            sControlData.u16CFBKRunFilterBits;
    #endif
    /*sCFBKBFbckFilter */
    sControlData.sStack[STACK_1].sCFBKBFbckFilter.u16NSamples =
            sControlData.u16CFBKRunFilterBits;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCFBKBFbckFilter.u16NSamples =
            sControlData.u16CFBKRunFilterBits;
    #endif
    /*sCFBKCFbckFilter */
    sControlData.sStack[STACK_1].sCFBKCFbckFilter.u16NSamples =
            sControlData.u16CFBKRunFilterBits;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCFBKCFbckFilter.u16NSamples =
            sControlData.u16CFBKRunFilterBits;
    #endif
    /*sCFBKAFbckRMSFilter */
    sControlData.sStack[STACK_1].sCFBKAFbckRMSFilter.u16NSamples =
            sControlData.u16CFBKRunFilterBits;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCFBKAFbckRMSFilter.u16NSamples =
            sControlData.u16CFBKRunFilterBits;
    #endif
    /*sCFBKBFbckRMSFilter */
    sControlData.sStack[STACK_1].sCFBKBFbckRMSFilter.u16NSamples =
            sControlData.u16CFBKRunFilterBits;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCFBKBFbckRMSFilter.u16NSamples =
            sControlData.u16CFBKRunFilterBits;
    #endif
    /*sCFBKCFbckRMSFilter */
    sControlData.sStack[STACK_1].sCFBKCFbckRMSFilter.u16NSamples =
            sControlData.u16CFBKRunFilterBits;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCFBKCFbckRMSFilter.u16NSamples =
            sControlData.u16CFBKRunFilterBits;
    #endif




    /*sVoltageDCBusRootMeanFilter */
    sControlData.sStack[STACK_1].sVoltageDCBusRootMeanFilter.u16NSamples =
            sControlData.u16VoltageRunFilterBits;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sVoltageDCBusRootMeanFilter.u16NSamples =
            sControlData.u16VoltageRunFilterBits;
    #endif
    /*sVoltageDCBusRMSFilter */
    sControlData.sStack[STACK_1].sVoltageDCBusRMSFilter.u16NSamples =
            sControlData.u16VoltageRunFilterBits;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sVoltageDCBusRMSFilter.u16NSamples =
            sControlData.u16VoltageRunFilterBits;
    #endif

//#endif







    /* DC Bus Voltage Filter */
    sControlData.sStack[STACK_1].sVoltageDCBusFilt.u32Acc =
            ((int32_t) sControlData.sStack[STACK_1].u16VoltageDCBus <<
            sControlData.sStack[STACK_1].sVoltageDCBusFilt.u16NSamples);

    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sVoltageDCBusFilt.u32Acc =
            ((int32_t) sControlData.sStack[STACK_2].u16VoltageDCBus <<
            sControlData.sStack[STACK_2].sVoltageDCBusFilt.u16NSamples);
    #endif

    /* sCurrentAmpFbckFilt */
    sControlData.sStack[STACK_1].sCurrentAmpFbckFilt.s32Acc =
            ((int32_t) 0 <<
            sControlData.sStack[STACK_1].sCurrentAmpFbckFilt.u16NSamples);

    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCurrentAmpFbckFilt.s32Acc =
            ((int32_t) 0 <<
            sControlData.sStack[STACK_2].sCurrentAmpFbckFilt.u16NSamples);
    #endif

//#if USE_NEW_RMS

    sControlData.sStack[STACK_1].sVoltageDCBusRMSFiltNew.u32Acc = 0;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sVoltageDCBusRMSFiltNew.u32Acc = 0;
    #endif

    sControlData.sStack[STACK_1].sCurrentAFbckRMSFiltNew.s32Acc = 0;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCurrentAFbckRMSFiltNew.s32Acc = 0;
    #endif
    sControlData.sStack[STACK_1].sCurrentBFbckRMSFiltNew.s32Acc = 0;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCurrentBFbckRMSFiltNew.s32Acc = 0;
    #endif
    sControlData.sStack[STACK_1].sCurrentCFbckRMSFiltNew.s32Acc = 0;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCurrentCFbckRMSFiltNew.s32Acc = 0;
    #endif

    sControlData.sStack[STACK_1].sCFBKAFbckRMSFiltNew.s32Acc = 0;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCFBKAFbckRMSFiltNew.s32Acc = 0;
    #endif
    sControlData.sStack[STACK_1].sCFBKBFbckRMSFiltNew.s32Acc = 0;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCFBKBFbckRMSFiltNew.s32Acc = 0;
    #endif
    sControlData.sStack[STACK_1].sCFBKCFbckRMSFiltNew.s32Acc = 0;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCFBKCFbckRMSFiltNew.s32Acc = 0;
    #endif

//#else
    //remove code below after testing new RMS calc ok
    /*sCurrentAFbckFilter */
    sControlData.sStack[STACK_1].sCurrentAFbckFilter.s32Acc = 0;

    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCurrentAFbckFilter.s32Acc = 0;
    #endif


    /*sCurrentBFbckFilter */
    sControlData.sStack[STACK_1].sCurrentBFbckFilter.s32Acc = 0;

    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCurrentBFbckFilter.s32Acc = 0;
    #endif


    /*sCurrentCFbckFilter */
    sControlData.sStack[STACK_1].sCurrentCFbckFilter.s32Acc = 0;

    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCurrentCFbckFilter.s32Acc = 0;
    #endif




    /*sCurrentAFbckRMSFilter */
    sControlData.sStack[STACK_1].sCurrentAFbckRMSFilter.s32Acc = 0;

    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCurrentAFbckRMSFilter.s32Acc = 0;
    #endif


    /*sCurrentBFbckRMSFilter */
    sControlData.sStack[STACK_1].sCurrentBFbckRMSFilter.s32Acc = 0;

    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCurrentBFbckRMSFilter.s32Acc = 0;
    #endif


    /*sCurrentCFbckRMSFilter */
    sControlData.sStack[STACK_1].sCurrentCFbckRMSFilter.s32Acc = 0;

    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCurrentCFbckRMSFilter.s32Acc = 0;
    #endif






    /*sCFBKAFbckFilter */
    sControlData.sStack[STACK_1].sCFBKAFbckFilter.s32Acc = 0;

    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCFBKAFbckFilter.s32Acc = 0;
    #endif


    /*sCFBKBFbckFilter */
    sControlData.sStack[STACK_1].sCFBKBFbckFilter.s32Acc = 0;

    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCFBKBFbckFilter.s32Acc = 0;
    #endif


    /*sCFBKCFbckFilter */
    sControlData.sStack[STACK_1].sCFBKCFbckFilter.s32Acc = 0;

    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCFBKCFbckFilter.s32Acc = 0;
    #endif




    /*sCFBKAFbckRMSFilter */
    sControlData.sStack[STACK_1].sCFBKAFbckRMSFilter.s32Acc = 0;

    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCFBKAFbckRMSFilter.s32Acc = 0;
    #endif


    /*sCFBKBFbckRMSFilter */
    sControlData.sStack[STACK_1].sCFBKBFbckRMSFilter.s32Acc = 0;

    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCFBKBFbckRMSFilter.s32Acc = 0;
    #endif


    /*sCFBKCFbckRMSFilter */
    sControlData.sStack[STACK_1].sCFBKCFbckRMSFilter.s32Acc = 0;

    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCFBKCFbckRMSFilter.s32Acc = 0;
    #endif












    /*sVoltageDCBusRootMeanFilter */
    sControlData.sStack[STACK_1].sVoltageDCBusRootMeanFilter.u32Acc = 0;

    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sVoltageDCBusRootMeanFilter.u32Acc = 0;
    #endif


    /*sVoltageDCBusRMSFilter */
    sControlData.sStack[STACK_1].sVoltageDCBusRMSFilter.u32Acc = 0;

    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sVoltageDCBusRMSFilter.u32Acc = 0;
    #endif

//#endif




    /* Request Limitation */
    #if !defined(BOARDCFG_USE_MCC_ADC) && (BOARDCFG_USE_OLD_NON_MCC_ADC)
    CONTROL_u16PWM1FreqHz = ADCPWM_u16GetPWM1FreqHz();
    CONTROL_u16PWM2FreqHz = ADCPWM_u16GetPWM2FreqHz();
    #endif
    MCLIB_mValueLimitWithMaxMin(CONTROL_u16PWM1FreqHz, sControlLimit.s16PWMFrequencyRequestMax, sControlLimit.s16PWMFrequencyRequestMin);
    MCLIB_mValueLimitWithMaxMin(CONTROL_u16PWM2FreqHz, sControlLimit.s16PWMFrequencyRequestMax, sControlLimit.s16PWMFrequencyRequestMin);


    /* Position Integrator */
    s16PositionIntegratorOld =
            POSITION_INTEGRATOR_SCALE_COEF;

    sControlData.sStack[STACK_1].sPositionIntegrator.s16C1 =
            MCLIB_s16FracFromFrac_u16(MCLIB_u16Div_u32u16( MCLIB_s32Fractional(SPEED_RESOLUTION_HZ / SPEED_INTEGRATOR_FIX_PRESCALER) , (int16_t)CONTROL_u16PWM1FreqHz ));
    sControlData.sStack[STACK_1].sPositionIntegrator.u16NShift =
            POSITION_INTEGRATOR_SCALE_SHIFT;
    sControlData.sStack[STACK_1].sPositionIntegrator.s32State =
            ((int32_t) sControlData.sStack[STACK_1].s16Position <<
            (16 + sControlData.sStack[STACK_1].sPositionIntegrator.u16NShift));

    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sPositionIntegrator.s16C1 =
            MCLIB_s16FracFromFrac_u16(MCLIB_u16Div_u32u16( MCLIB_s32Fractional(SPEED_RESOLUTION_HZ / SPEED_INTEGRATOR_FIX_PRESCALER) , (int16_t)CONTROL_u16PWM2FreqHz ));
    sControlData.sStack[STACK_2].sPositionIntegrator.u16NShift =
            POSITION_INTEGRATOR_SCALE_SHIFT;
    sControlData.sStack[STACK_2].sPositionIntegrator.s32State =
            ((int32_t) sControlData.sStack[STACK_2].s16Position <<
            (16 + sControlData.sStack[STACK_2].sPositionIntegrator.u16NShift));
    #endif

    /* PositionFbckEmulation Integrator */
    sControlData.sStack[STACK_1].sPositionFbckEmulationIntegrator.s16C1 =
            MCLIB_s16FracFromFrac_u16(MCLIB_u16Div_u32u16( MCLIB_s32Fractional(SPEED_RESOLUTION_HZ / SPEED_INTEGRATOR_FIX_PRESCALER) , (int16_t)CONTROL_u16PWM1FreqHz ));
    sControlData.sStack[STACK_1].sPositionFbckEmulationIntegrator.u16NShift =
            POSITION_INTEGRATOR_SCALE_SHIFT;
    sControlData.sStack[STACK_1].sPositionFbckEmulationIntegrator.s32State =
            ((int32_t) sControlData.sStack[STACK_1].s16PositionFbckEmulation <<
            (16 + sControlData.sStack[STACK_1].sPositionFbckEmulationIntegrator.u16NShift));

    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sPositionFbckEmulationIntegrator.s16C1 =
            MCLIB_s16FracFromFrac_u16(MCLIB_u16Div_u32u16( MCLIB_s32Fractional(SPEED_RESOLUTION_HZ / SPEED_INTEGRATOR_FIX_PRESCALER) , (int16_t)CONTROL_u16PWM2FreqHz ));
    sControlData.sStack[STACK_2].sPositionFbckEmulationIntegrator.u16NShift =
            POSITION_INTEGRATOR_SCALE_SHIFT;
    sControlData.sStack[STACK_2].sPositionFbckEmulationIntegrator.s32State =
            ((int32_t) sControlData.sStack[STACK_2].s16PositionFbckEmulation <<
            (16 + sControlData.sStack[STACK_2].sPositionFbckEmulationIntegrator.u16NShift));
    #endif




    /* Startup Voltage Output */
    sControlData.sStack[STACK_1].u16VoltageRMSReq = (int16_t)sControlData.sStack[STACK_1].u16VoltageStartup;
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].u16VoltageRMSReq = (int16_t)sControlData.sStack[STACK_2].u16VoltageStartup;
    #endif



    /* Voltage Request Controller PI Reset Stack1 */
    sControlData.sStack[STACK_1].sVoltageReqPI.s16PrevError = 0;
    sControlData.sStack[STACK_1].sVoltageReqPI.u32PrevIntegPart =
            ((int32_t) 0 << 16);

    #if BOARDCFG_STACK_COUNT >= 2
    /* Voltage Request Controller PI Reset Stack2 */
    sControlData.sStack[STACK_2].sVoltageReqPI.s16PrevError = 0;
    sControlData.sStack[STACK_2].sVoltageReqPI.u32PrevIntegPart =
            ((int32_t) 0 << 16);
    #endif




}



void vCurrentVoltageFbkPrepare(void)
{

    sControlData.sStack[STACK_1].sCurrentABCFbckTotal.s32Arg1 = ADCStack1NoOffset[ADC_STACK_CURRENT_L1];
    sControlData.sStack[STACK_1].sCurrentABCFbckTotal.s32Arg2 = ADCStack1NoOffset[ADC_STACK_CURRENT_L2];
    sControlData.sStack[STACK_1].sCurrentABCFbckTotal.s32Arg3 = ADCStack1NoOffset[ADC_STACK_CURRENT_L3];


    /* Input data from ADC Measurements */
    sControlData.sStack[STACK_1].sCurrentABCFbck.s16Arg1 = ADCStack1NoOffset[ADC_STACK_CURRENT_L1];
    sControlData.sStack[STACK_1].sCurrentABCFbck.s16Arg2 = ADCStack1NoOffset[ADC_STACK_CURRENT_L2];
    sControlData.sStack[STACK_1].sCurrentABCFbck.s16Arg3 = ADCStack1NoOffset[ADC_STACK_CURRENT_L3];

#if BOARDCFG_USE_TSC_DSPIC
    sControlData.sStack[STACK_1].u16VoltageDCBus = MCLIB_u16From_s16(ADCStack1NoOffset[ADC_STACK_DC_VOLTAGE]);
#else
    sControlData.sStack[STACK_1].u16VoltageDCBus = (uint16_t)(ADCDCLinkNoOffset[ADC_STACK_DC_VOLTAGE]);
#endif

    //if (sControlData.sStack[STACK_1].u16VoltageDCBus < 0) {
    //    sControlData.sStack[STACK_1].u16VoltageDCBus = 0;
    //}

    sControlData.sStack[STACK_1].sCurrentABCFbckReal.s16Arg1 = sControlData.sStack[STACK_1].sCurrentABCFbck.s16Arg1;
    sControlData.sStack[STACK_1].sCurrentABCFbckReal.s16Arg2 = sControlData.sStack[STACK_1].sCurrentABCFbck.s16Arg2;
    sControlData.sStack[STACK_1].sCurrentABCFbckReal.s16Arg3 = sControlData.sStack[STACK_1].sCurrentABCFbck.s16Arg3;


    if ((bControlStack1Use2SensorFeedbacks) || (bControlStack1Use3SensorFeedbacks))
    {
        if (bControlStack1Use2SensorFeedbacks)
        {
            sControlData.sStack[STACK_1].sCurrentABCFbckTotal.s32Arg1 += ADCStack2NoOffset[ADC_STACK_CURRENT_L1];
            sControlData.sStack[STACK_1].sCurrentABCFbckTotal.s32Arg2 += ADCStack2NoOffset[ADC_STACK_CURRENT_L2];
            sControlData.sStack[STACK_1].sCurrentABCFbckTotal.s32Arg3 += ADCStack2NoOffset[ADC_STACK_CURRENT_L3];

            sControlData.sStack[STACK_1].sCurrentABCFbck.s16Arg1 += ADCStack2NoOffset[ADC_STACK_CURRENT_L1];
            sControlData.sStack[STACK_1].sCurrentABCFbck.s16Arg2 += ADCStack2NoOffset[ADC_STACK_CURRENT_L2];
            sControlData.sStack[STACK_1].sCurrentABCFbck.s16Arg3 += ADCStack2NoOffset[ADC_STACK_CURRENT_L3];
        }
        if (bControlStack1Use3SensorFeedbacks)
        {
            sControlData.sStack[STACK_1].sCurrentABCFbckTotal.s32Arg1 += ADCStack3NoOffset[ADC_STACK_CURRENT_L1];
            sControlData.sStack[STACK_1].sCurrentABCFbckTotal.s32Arg2 += ADCStack3NoOffset[ADC_STACK_CURRENT_L2];
            sControlData.sStack[STACK_1].sCurrentABCFbckTotal.s32Arg3 += ADCStack3NoOffset[ADC_STACK_CURRENT_L3];

            sControlData.sStack[STACK_1].sCurrentABCFbck.s16Arg1 += ADCStack3NoOffset[ADC_STACK_CURRENT_L1];
            sControlData.sStack[STACK_1].sCurrentABCFbck.s16Arg2 += ADCStack3NoOffset[ADC_STACK_CURRENT_L2];
            sControlData.sStack[STACK_1].sCurrentABCFbck.s16Arg3 += ADCStack3NoOffset[ADC_STACK_CURRENT_L3];
        }
    }
    else
    if (bControlStack1UseSensorsStack2)
    {
        sControlData.sStack[STACK_1].sCurrentABCFbckTotal.s32Arg1 = ADCStack2NoOffset[ADC_STACK_CURRENT_L1];
        sControlData.sStack[STACK_1].sCurrentABCFbckTotal.s32Arg2 = ADCStack2NoOffset[ADC_STACK_CURRENT_L2];
        sControlData.sStack[STACK_1].sCurrentABCFbckTotal.s32Arg3 = ADCStack2NoOffset[ADC_STACK_CURRENT_L3];

        sControlData.sStack[STACK_1].sCurrentABCFbck.s16Arg1 = ADCStack2NoOffset[ADC_STACK_CURRENT_L1];
        sControlData.sStack[STACK_1].sCurrentABCFbck.s16Arg2 = ADCStack2NoOffset[ADC_STACK_CURRENT_L2];
        sControlData.sStack[STACK_1].sCurrentABCFbck.s16Arg3 = ADCStack2NoOffset[ADC_STACK_CURRENT_L3];

    }

    sControlData.sStack[STACK_1].u16CurrentABCFbckTotalDecreaseResolution = 0;

    while ( ((sControlData.sStack[STACK_1].sCurrentABCFbckTotal.s32Arg1 < INT16_MIN) || (sControlData.sStack[STACK_1].sCurrentABCFbckTotal.s32Arg1 > INT16_MAX))
         || ((sControlData.sStack[STACK_1].sCurrentABCFbckTotal.s32Arg2 < INT16_MIN) || (sControlData.sStack[STACK_1].sCurrentABCFbckTotal.s32Arg2 > INT16_MAX))
         || ((sControlData.sStack[STACK_1].sCurrentABCFbckTotal.s32Arg3 < INT16_MIN) || (sControlData.sStack[STACK_1].sCurrentABCFbckTotal.s32Arg3 > INT16_MAX)) )
    {
        sControlData.sStack[STACK_1].sCurrentABCFbckTotal.s32Arg1 >>= 1;
        sControlData.sStack[STACK_1].sCurrentABCFbckTotal.s32Arg2 >>= 1;
        sControlData.sStack[STACK_1].sCurrentABCFbckTotal.s32Arg3 >>= 1;
        sControlData.sStack[STACK_1].u16CurrentABCFbckTotalDecreaseResolution++;    /* not implemented yet */
    }

    sControlData.sStack[STACK_1].sCurrentABCFbck.s16Arg1 = (int16_t)sControlData.sStack[STACK_1].sCurrentABCFbckTotal.s32Arg1;
    sControlData.sStack[STACK_1].sCurrentABCFbck.s16Arg2 = (int16_t)sControlData.sStack[STACK_1].sCurrentABCFbckTotal.s32Arg2;
    sControlData.sStack[STACK_1].sCurrentABCFbck.s16Arg3 = (int16_t)sControlData.sStack[STACK_1].sCurrentABCFbckTotal.s32Arg3;



    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].sCurrentABCFbck.s16Arg1 = ADCStack2NoOffset[ADC_STACK_CURRENT_L1];
    sControlData.sStack[STACK_2].sCurrentABCFbck.s16Arg2 = ADCStack2NoOffset[ADC_STACK_CURRENT_L2];
    sControlData.sStack[STACK_2].sCurrentABCFbck.s16Arg3 = ADCStack2NoOffset[ADC_STACK_CURRENT_L3];

    #if BOARDCFG_USE_TSC_DSPIC
    sControlData.sStack[STACK_2].u16VoltageDCBus = MCLIB_u16From_s16(ADCStack2NoOffset[ADC_STACK_DC_VOLTAGE]);
    #else
    sControlData.sStack[STACK_2].u16VoltageDCBus = (uint16_t)(ADCDCLinkNoOffset[ADC_STACK_DC_VOLTAGE]);
    #endif


    //if (sControlData.sStack[STACK_2].u16VoltageDCBus < 0) {
    //    sControlData.sStack[STACK_2].u16VoltageDCBus = 0;
    //}

    sControlData.sStack[STACK_2].sCurrentABCFbckReal.s16Arg1 = sControlData.sStack[STACK_2].sCurrentABCFbck.s16Arg1;
    sControlData.sStack[STACK_2].sCurrentABCFbckReal.s16Arg2 = sControlData.sStack[STACK_2].sCurrentABCFbck.s16Arg2;
    sControlData.sStack[STACK_2].sCurrentABCFbckReal.s16Arg3 = sControlData.sStack[STACK_2].sCurrentABCFbck.s16Arg3;

    #endif


#if BOARDCFG_USE_TSC_DSPIC
    /* Input data from ADC Measurements stack 1 from driver */
    sControlData.sStack[STACK_1].sCFBKABCFbck.s16Arg1 = ADC2Stack1NoOffset[ADC2_STACK_CURRFBK_L1];
    sControlData.sStack[STACK_1].sCFBKABCFbck.s16Arg2 = ADC2Stack1NoOffset[ADC2_STACK_CURRFBK_L2];
    sControlData.sStack[STACK_1].sCFBKABCFbck.s16Arg3 = ADC2Stack1NoOffset[ADC2_STACK_CURRFBK_L3];
    /* Input data from ADC Measurements stack 2 from driver */
    sControlData.sStack[STACK_2].sCFBKABCFbck.s16Arg1 = ADC2Stack2NoOffset[ADC2_STACK_CURRFBK_L1];
    sControlData.sStack[STACK_2].sCFBKABCFbck.s16Arg2 = ADC2Stack2NoOffset[ADC2_STACK_CURRFBK_L2];
    sControlData.sStack[STACK_2].sCFBKABCFbck.s16Arg3 = ADC2Stack2NoOffset[ADC2_STACK_CURRFBK_L3];
#else
    /* Input data from ADC Measurements DCLI */
    sControlData.sStack[STACK_1].sCFBKABCFbck.s16Arg1 = ADCDCLinkNoOffset[ADC_STACK_CURRENT_L1];
    sControlData.sStack[STACK_1].sCFBKABCFbck.s16Arg2 = ADCDCLinkNoOffset[ADC_STACK_CURRENT_L2];
    sControlData.sStack[STACK_1].sCFBKABCFbck.s16Arg3 = ADCDCLinkNoOffset[ADC_STACK_CURRENT_L3];
#endif


}














void vStateCalib(void)
{
    uint16_t u16AddRoundUp;

    if (sControlAccess.bEmulateVoltageFbck)
    {
#if BOARDCFG_USE_TSC_DSPIC
        ADCStack1Buffered[ADC_STACK_DC_VOLTAGE] = sControlData.sStack[STACK_1].u16VoltageDCBusFbckEmulation;
        #if BOARDCFG_STACK_COUNT >= 2
        ADCStack2Buffered[ADC_STACK_DC_VOLTAGE] = sControlData.sStack[STACK_2].u16VoltageDCBusFbckEmulation;
        #endif
#else
        ADCDCLinkBuffered[ADC_STACK_DC_VOLTAGE] = sControlData.sStack[STACK_1].u16VoltageDCBusFbckEmulation;
#endif
    }

    ADCStack1OffsetBuffer[ADC_STACK_CURRENT_L1] += ADCStack1Buffered[ADC_STACK_CURRENT_L1];
    ADCStack1OffsetBuffer[ADC_STACK_CURRENT_L2] += ADCStack1Buffered[ADC_STACK_CURRENT_L2];
    ADCStack1OffsetBuffer[ADC_STACK_CURRENT_L3] += ADCStack1Buffered[ADC_STACK_CURRENT_L3];
    ADCStack1OffsetBuffer[ADC_STACK_DC_VOLTAGE] += ADCStack1Buffered[ADC_STACK_DC_VOLTAGE];
    ADCStack2OffsetBuffer[ADC_STACK_CURRENT_L1] += ADCStack2Buffered[ADC_STACK_CURRENT_L1];
    ADCStack2OffsetBuffer[ADC_STACK_CURRENT_L2] += ADCStack2Buffered[ADC_STACK_CURRENT_L2];
    ADCStack2OffsetBuffer[ADC_STACK_CURRENT_L3] += ADCStack2Buffered[ADC_STACK_CURRENT_L3];
    ADCStack2OffsetBuffer[ADC_STACK_DC_VOLTAGE] += ADCStack2Buffered[ADC_STACK_DC_VOLTAGE];

    ADCStack3OffsetBuffer[ADC_STACK_CURRENT_L1] += ADCStack3Buffered[ADC_STACK_CURRENT_L1];
    ADCStack3OffsetBuffer[ADC_STACK_CURRENT_L2] += ADCStack3Buffered[ADC_STACK_CURRENT_L2];
    ADCStack3OffsetBuffer[ADC_STACK_CURRENT_L3] += ADCStack3Buffered[ADC_STACK_CURRENT_L3];
    ADCStack3OffsetBuffer[ADC_STACK_DC_VOLTAGE] += ADCStack3Buffered[ADC_STACK_DC_VOLTAGE];
    ADCDCLinkOffsetBuffer[ADC_STACK_CURRENT_L1] += ADCDCLinkBuffered[ADC_STACK_CURRENT_L1];
    ADCDCLinkOffsetBuffer[ADC_STACK_CURRENT_L2] += ADCDCLinkBuffered[ADC_STACK_CURRENT_L2];
    ADCDCLinkOffsetBuffer[ADC_STACK_CURRENT_L3] += ADCDCLinkBuffered[ADC_STACK_CURRENT_L3];
    ADCDCLinkOffsetBuffer[ADC_STACK_DC_VOLTAGE] += ADCDCLinkBuffered[ADC_STACK_DC_VOLTAGE];

    ADCLine_1OffsetBuffer[ADC_STACK_CURRENT_L1] += ADCLine_1Buffered[ADC_STACK_CURRENT_L1];
    ADCLine_1OffsetBuffer[ADC_STACK_CURRENT_L2] += ADCLine_1Buffered[ADC_STACK_CURRENT_L2];
    ADCLine_1OffsetBuffer[ADC_STACK_CURRENT_L3] += ADCLine_1Buffered[ADC_STACK_CURRENT_L3];
    ADCLine_1OffsetBuffer[ADC_STACK_DC_VOLTAGE] += ADCLine_1Buffered[ADC_STACK_DC_VOLTAGE];
    ADCLine_2OffsetBuffer[ADC_STACK_CURRENT_L1] += ADCLine_2Buffered[ADC_STACK_CURRENT_L1];
    ADCLine_2OffsetBuffer[ADC_STACK_CURRENT_L2] += ADCLine_2Buffered[ADC_STACK_CURRENT_L2];
    ADCLine_2OffsetBuffer[ADC_STACK_CURRENT_L3] += ADCLine_2Buffered[ADC_STACK_CURRENT_L3];
    ADCLine_2OffsetBuffer[ADC_STACK_DC_VOLTAGE] += ADCLine_2Buffered[ADC_STACK_DC_VOLTAGE];

    ADC2Stack1OffsetBuffer[ADC2_STACK_CURRFBK_L1] += ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L1];
    ADC2Stack1OffsetBuffer[ADC2_STACK_CURRFBK_L2] += ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L2];
    ADC2Stack1OffsetBuffer[ADC2_STACK_CURRFBK_L3] += ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L3];
    ADC2Stack1OffsetBuffer[ADC2_STACK_TEMPFBK_L1] += ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L1];
    ADC2Stack1OffsetBuffer[ADC2_STACK_TEMPFBK_L2] += ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L2];
    ADC2Stack1OffsetBuffer[ADC2_STACK_TEMPFBK_L3] += ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L3];
    ADC2Stack1OffsetBuffer[ADC2_STACK_DCVOLTAGE2] += ADC2Stack1Buffered[ADC2_STACK_DCVOLTAGE2];
    
    ADC2Stack2OffsetBuffer[ADC2_STACK_CURRFBK_L1] += ADC2Stack2Buffered[ADC2_STACK_CURRFBK_L1];
    ADC2Stack2OffsetBuffer[ADC2_STACK_CURRFBK_L2] += ADC2Stack2Buffered[ADC2_STACK_CURRFBK_L2];
    ADC2Stack2OffsetBuffer[ADC2_STACK_CURRFBK_L3] += ADC2Stack2Buffered[ADC2_STACK_CURRFBK_L3];
    ADC2Stack2OffsetBuffer[ADC2_STACK_TEMPFBK_L1] += ADC2Stack2Buffered[ADC2_STACK_TEMPFBK_L1];
    ADC2Stack2OffsetBuffer[ADC2_STACK_TEMPFBK_L2] += ADC2Stack2Buffered[ADC2_STACK_TEMPFBK_L2];
    ADC2Stack2OffsetBuffer[ADC2_STACK_TEMPFBK_L3] += ADC2Stack2Buffered[ADC2_STACK_TEMPFBK_L3];
    ADC2Stack2OffsetBuffer[ADC2_STACK_DCVOLTAGE2] += ADC2Stack2Buffered[ADC2_STACK_DCVOLTAGE2];

    if (sControlAccess.bUseWrongVoltageOffsetDetection)
    {

        if (sControlData.u32CalibCounter == 0)
        {
            sControlData.sStack[STACK_1].u16VoltageDCBusStartCalibration = ADCStack1Buffered[ADC_STACK_DC_VOLTAGE];
        }

        sControlData.u32CalibCounter++;

        if (sControlData.u32CalibCounter >= sControlConfig.u32CalibCount)
        {
            sControlData.sStack[STACK_1].u16VoltageDCBusEndCalibration = ADCStack1Buffered[ADC_STACK_DC_VOLTAGE];
            sControlData.sStack[STACK_1].u16VoltageDCBusDiffCalibration =
                    (uint16_t)MCLIB_s16AbsSat(sControlData.sStack[STACK_1].u16VoltageDCBusEndCalibration - sControlData.sStack[STACK_1].u16VoltageDCBusStartCalibration);

            if (bOffsetCalcAllowVoltageStack1UseAutoManipulation)
            {
                if ((sControlData.sStack[STACK_1].u16VoltageDCBusDiffCalibration > sControlData.sStack[STACK_1].u16VoltageDCBusDeltaCalibration) || (bOffsetCalcAllowVoltageStack1 == 0))
                {
                    sControlData.u32CalibCounter = 0;
                    ADC_vOffsetBufferInit();
                }
            }
        }
    }
    else
    {
        sControlData.u32CalibCounter++;
    }




    sControlConfig.u32CalibCount = ((uint32_t) 1 << sControlData.u16OffsetCalcFilterBits);



    if (sControlData.u32CalibCounter >= sControlConfig.u32CalibCount) {
        if (sControlConfig.u32CalibCount == 0) {
            sControlConfig.u32CalibCount = 1;
        }
        u16AddRoundUp = sControlConfig.u32CalibCount >> 1;
        //u16AddRoundUp = 0;

        ADCStack1OffsetBuffer[ADC_STACK_CURRENT_L1] += u16AddRoundUp;
        ADCStack1OffsetBuffer[ADC_STACK_CURRENT_L2] += u16AddRoundUp;
        ADCStack1OffsetBuffer[ADC_STACK_CURRENT_L3] += u16AddRoundUp;
        ADCStack1OffsetBuffer[ADC_STACK_DC_VOLTAGE] += u16AddRoundUp;
        ADCStack2OffsetBuffer[ADC_STACK_CURRENT_L1] += u16AddRoundUp;
        ADCStack2OffsetBuffer[ADC_STACK_CURRENT_L2] += u16AddRoundUp;
        ADCStack2OffsetBuffer[ADC_STACK_CURRENT_L3] += u16AddRoundUp;
        ADCStack2OffsetBuffer[ADC_STACK_DC_VOLTAGE] += u16AddRoundUp;

        ADCStack3OffsetBuffer[ADC_STACK_CURRENT_L1] += u16AddRoundUp;
        ADCStack3OffsetBuffer[ADC_STACK_CURRENT_L2] += u16AddRoundUp;
        ADCStack3OffsetBuffer[ADC_STACK_CURRENT_L3] += u16AddRoundUp;
        ADCStack3OffsetBuffer[ADC_STACK_DC_VOLTAGE] += u16AddRoundUp;
        ADCDCLinkOffsetBuffer[ADC_STACK_CURRENT_L1] += u16AddRoundUp;
        ADCDCLinkOffsetBuffer[ADC_STACK_CURRENT_L2] += u16AddRoundUp;
        ADCDCLinkOffsetBuffer[ADC_STACK_CURRENT_L3] += u16AddRoundUp;
        ADCDCLinkOffsetBuffer[ADC_STACK_DC_VOLTAGE] += u16AddRoundUp;

        ADCLine_1OffsetBuffer[ADC_STACK_CURRENT_L1] += u16AddRoundUp;
        ADCLine_1OffsetBuffer[ADC_STACK_CURRENT_L2] += u16AddRoundUp;
        ADCLine_1OffsetBuffer[ADC_STACK_CURRENT_L3] += u16AddRoundUp;
        ADCLine_1OffsetBuffer[ADC_STACK_DC_VOLTAGE] += u16AddRoundUp;
        ADCLine_2OffsetBuffer[ADC_STACK_CURRENT_L1] += u16AddRoundUp;
        ADCLine_2OffsetBuffer[ADC_STACK_CURRENT_L2] += u16AddRoundUp;
        ADCLine_2OffsetBuffer[ADC_STACK_CURRENT_L3] += u16AddRoundUp;
        ADCLine_2OffsetBuffer[ADC_STACK_DC_VOLTAGE] += u16AddRoundUp;

        ADC2Stack1OffsetBuffer[ADC2_STACK_CURRFBK_L1] += u16AddRoundUp;
        ADC2Stack1OffsetBuffer[ADC2_STACK_CURRFBK_L2] += u16AddRoundUp;
        ADC2Stack1OffsetBuffer[ADC2_STACK_CURRFBK_L3] += u16AddRoundUp;
        ADC2Stack1OffsetBuffer[ADC2_STACK_TEMPFBK_L1] += u16AddRoundUp;
        ADC2Stack1OffsetBuffer[ADC2_STACK_TEMPFBK_L2] += u16AddRoundUp;
        ADC2Stack1OffsetBuffer[ADC2_STACK_TEMPFBK_L3] += u16AddRoundUp;
        ADC2Stack1OffsetBuffer[ADC2_STACK_DCVOLTAGE2] += u16AddRoundUp;
        
        ADC2Stack2OffsetBuffer[ADC2_STACK_CURRFBK_L1] += u16AddRoundUp;
        ADC2Stack2OffsetBuffer[ADC2_STACK_CURRFBK_L2] += u16AddRoundUp;
        ADC2Stack2OffsetBuffer[ADC2_STACK_CURRFBK_L3] += u16AddRoundUp;
        ADC2Stack2OffsetBuffer[ADC2_STACK_TEMPFBK_L1] += u16AddRoundUp;
        ADC2Stack2OffsetBuffer[ADC2_STACK_TEMPFBK_L2] += u16AddRoundUp;
        ADC2Stack2OffsetBuffer[ADC2_STACK_TEMPFBK_L3] += u16AddRoundUp;
        ADC2Stack2OffsetBuffer[ADC2_STACK_DCVOLTAGE2] += u16AddRoundUp;








        if (bOffsetCalcAllowCurrentStack1) {
            ADCStack1Offset[ADC_STACK_CURRENT_L1] = ADCStack1OffsetBuffer[ADC_STACK_CURRENT_L1] / sControlConfig.u32CalibCount;
            ADCStack1Offset[ADC_STACK_CURRENT_L2] = ADCStack1OffsetBuffer[ADC_STACK_CURRENT_L2] / sControlConfig.u32CalibCount;
            ADCStack1Offset[ADC_STACK_CURRENT_L3] = ADCStack1OffsetBuffer[ADC_STACK_CURRENT_L3] / sControlConfig.u32CalibCount;
            bOffsetPerformedCurrentStack1 = 1;
        }
        if (bOffsetCalcAllowVoltageStack1) {
            ADCStack1Offset[ADC_STACK_DC_VOLTAGE] = ADCStack1OffsetBuffer[ADC_STACK_DC_VOLTAGE] / sControlConfig.u32CalibCount;
            bOffsetPerformedVoltageStack1 = 1;
        }

        if (bOffsetCalcAllowCurrentStack2) {
            ADCStack2Offset[ADC_STACK_CURRENT_L1] = ADCStack2OffsetBuffer[ADC_STACK_CURRENT_L1] / sControlConfig.u32CalibCount;
            ADCStack2Offset[ADC_STACK_CURRENT_L2] = ADCStack2OffsetBuffer[ADC_STACK_CURRENT_L2] / sControlConfig.u32CalibCount;
            ADCStack2Offset[ADC_STACK_CURRENT_L3] = ADCStack2OffsetBuffer[ADC_STACK_CURRENT_L3] / sControlConfig.u32CalibCount;
            bOffsetPerformedCurrentStack2 = 1;
        }
        if (bOffsetCalcAllowVoltageStack2) {
            ADCStack2Offset[ADC_STACK_DC_VOLTAGE] = ADCStack2OffsetBuffer[ADC_STACK_DC_VOLTAGE] / sControlConfig.u32CalibCount;
            bOffsetPerformedVoltageStack2 = 1;
        }



        if (bOffsetCalcAllowCurrentStack3) {
            ADCStack3Offset[ADC_STACK_CURRENT_L1] = ADCStack3OffsetBuffer[ADC_STACK_CURRENT_L1] / sControlConfig.u32CalibCount;
            ADCStack3Offset[ADC_STACK_CURRENT_L2] = ADCStack3OffsetBuffer[ADC_STACK_CURRENT_L2] / sControlConfig.u32CalibCount;
            ADCStack3Offset[ADC_STACK_CURRENT_L3] = ADCStack3OffsetBuffer[ADC_STACK_CURRENT_L3] / sControlConfig.u32CalibCount;
            bOffsetPerformedCurrentStack3 = 1;
        }
        if (bOffsetCalcAllowVoltageStack3) {
            ADCStack3Offset[ADC_STACK_DC_VOLTAGE] = ADCStack3OffsetBuffer[ADC_STACK_DC_VOLTAGE] / sControlConfig.u32CalibCount;
            bOffsetPerformedVoltageStack3 = 1;
        }

        if (bOffsetCalcAllowCurrentDCLink) {
            ADCDCLinkOffset[ADC_STACK_CURRENT_L1] = ADCDCLinkOffsetBuffer[ADC_STACK_CURRENT_L1] / sControlConfig.u32CalibCount;
            ADCDCLinkOffset[ADC_STACK_CURRENT_L2] = ADCDCLinkOffsetBuffer[ADC_STACK_CURRENT_L2] / sControlConfig.u32CalibCount;
            ADCDCLinkOffset[ADC_STACK_CURRENT_L3] = ADCDCLinkOffsetBuffer[ADC_STACK_CURRENT_L3] / sControlConfig.u32CalibCount;
            bOffsetPerformedCurrentDCLink = 1;
        }
        if (bOffsetCalcAllowVoltageDCLink) {
            ADCDCLinkOffset[ADC_STACK_DC_VOLTAGE] = ADCDCLinkOffsetBuffer[ADC_STACK_DC_VOLTAGE] / sControlConfig.u32CalibCount;
            bOffsetPerformedVoltageDCLink = 1;
        }



        if (bOffsetCalcAllowCurrentLine_1_LLV12) {
            ADCLine_1Offset[ADC_STACK_CURRENT_L1] = ADCLine_1OffsetBuffer[ADC_STACK_CURRENT_L1] / sControlConfig.u32CalibCount;
            ADCLine_1Offset[ADC_STACK_CURRENT_L2] = ADCLine_1OffsetBuffer[ADC_STACK_CURRENT_L2] / sControlConfig.u32CalibCount;
            bOffsetPerformedCurrentLine_1_LLV12 = 1;
        }
        if (bOffsetCalcAllowCurrentLine_1_LLV3) {
            ADCLine_1Offset[ADC_STACK_CURRENT_L3] = ADCLine_1OffsetBuffer[ADC_STACK_CURRENT_L3] / sControlConfig.u32CalibCount;
            bOffsetPerformedCurrentLine_1_LLV3 = 1;
        }
        if (bOffsetCalcAllowVoltageLine_1) {
            ADCLine_1Offset[ADC_STACK_DC_VOLTAGE] = ADCLine_1OffsetBuffer[ADC_STACK_DC_VOLTAGE] / sControlConfig.u32CalibCount;
            bOffsetPerformedVoltageLine_1 = 1;
        }

        if (bOffsetCalcAllowCurrentLine_2) {
            ADCLine_2Offset[ADC_STACK_CURRENT_L1] = ADCLine_2OffsetBuffer[ADC_STACK_CURRENT_L1] / sControlConfig.u32CalibCount;
            ADCLine_2Offset[ADC_STACK_CURRENT_L2] = ADCLine_2OffsetBuffer[ADC_STACK_CURRENT_L2] / sControlConfig.u32CalibCount;
            ADCLine_2Offset[ADC_STACK_CURRENT_L3] = ADCLine_2OffsetBuffer[ADC_STACK_CURRENT_L3] / sControlConfig.u32CalibCount;
            bOffsetPerformedCurrentLine_2 = 1;
        }
        if (bOffsetCalcAllowVoltageLine_2) {
            ADCLine_2Offset[ADC_STACK_DC_VOLTAGE] = ADCLine_2OffsetBuffer[ADC_STACK_DC_VOLTAGE] / sControlConfig.u32CalibCount;
            bOffsetPerformedVoltageLine_2 = 1;
        }




        if (bOffsetCalcAllowCurrFbkStack1) {
            ADC2Stack1Offset[ADC2_STACK_CURRFBK_L1] = ADC2Stack1OffsetBuffer[ADC2_STACK_CURRFBK_L1] / sControlConfig.u32CalibCount;
            ADC2Stack1Offset[ADC2_STACK_CURRFBK_L2] = ADC2Stack1OffsetBuffer[ADC2_STACK_CURRFBK_L2] / sControlConfig.u32CalibCount;
            ADC2Stack1Offset[ADC2_STACK_CURRFBK_L3] = ADC2Stack1OffsetBuffer[ADC2_STACK_CURRFBK_L3] / sControlConfig.u32CalibCount;
            bOffsetPerformedCurrFbkStack1 = 1;
        }

        if (bOffsetCalcAllowCurrFbkStack2) {
            ADC2Stack2Offset[ADC2_STACK_CURRFBK_L1] = ADC2Stack2OffsetBuffer[ADC2_STACK_CURRFBK_L1] / sControlConfig.u32CalibCount;
            ADC2Stack2Offset[ADC2_STACK_CURRFBK_L2] = ADC2Stack2OffsetBuffer[ADC2_STACK_CURRFBK_L2] / sControlConfig.u32CalibCount;
            ADC2Stack2Offset[ADC2_STACK_CURRFBK_L3] = ADC2Stack2OffsetBuffer[ADC2_STACK_CURRFBK_L3] / sControlConfig.u32CalibCount;
            bOffsetPerformedCurrFbkStack2 = 1;
        }

        /* Temperature Stack1 */
        if ((bDetectDriverPhase1Stack1 == 0) || (bOffsetCalcAllowTempFbkStack1)) {
            ADC2Stack1Offset[ADC2_STACK_TEMPFBK_L1] = ADC2Stack1OffsetBuffer[ADC2_STACK_TEMPFBK_L1] / sControlConfig.u32CalibCount;
            bOffsetPerformedTmp1FbkStack1 = 1;
        }
        if ((bDetectDriverPhase2Stack1 == 0) || (bOffsetCalcAllowTempFbkStack1)) {
            ADC2Stack1Offset[ADC2_STACK_TEMPFBK_L2] = ADC2Stack1OffsetBuffer[ADC2_STACK_TEMPFBK_L2] / sControlConfig.u32CalibCount;
            bOffsetPerformedTmp2FbkStack1 = 1;
        }
        if ((bDetectDriverPhase3Stack1 == 0) || (bOffsetCalcAllowTempFbkStack1)) {
            ADC2Stack1Offset[ADC2_STACK_TEMPFBK_L3] = ADC2Stack1OffsetBuffer[ADC2_STACK_TEMPFBK_L3] / sControlConfig.u32CalibCount;
            bOffsetPerformedTmp3FbkStack1 = 1;
        }

        if ((bOffsetPerformedTmp1FbkStack1) && (bOffsetPerformedTmp2FbkStack1) && (bOffsetPerformedTmp3FbkStack1)) {
            bOffsetPerformedTempFbkStack1 = 1;
        } else {
            bOffsetPerformedTempFbkStack1 = 0;
        }

        /* Temperature Stack2 */
        if ((bDetectDriverPhase1Stack2 == 0) || (bOffsetCalcAllowTempFbkStack2)) {
            ADC2Stack2Offset[ADC2_STACK_TEMPFBK_L1] = ADC2Stack2OffsetBuffer[ADC2_STACK_TEMPFBK_L1] / sControlConfig.u32CalibCount;
            bOffsetPerformedTmp1FbkStack2 = 1;
        }
        if ((bDetectDriverPhase2Stack2 == 0) || (bOffsetCalcAllowTempFbkStack2)) {
            ADC2Stack2Offset[ADC2_STACK_TEMPFBK_L2] = ADC2Stack2OffsetBuffer[ADC2_STACK_TEMPFBK_L2] / sControlConfig.u32CalibCount;
            bOffsetPerformedTmp2FbkStack2 = 1;
        }
        if ((bDetectDriverPhase3Stack2 == 0) || (bOffsetCalcAllowTempFbkStack2)) {
            ADC2Stack2Offset[ADC2_STACK_TEMPFBK_L3] = ADC2Stack2OffsetBuffer[ADC2_STACK_TEMPFBK_L3] / sControlConfig.u32CalibCount;
            bOffsetPerformedTmp3FbkStack2 = 1;
        }

        if ((bOffsetPerformedTmp1FbkStack2) && (bOffsetPerformedTmp2FbkStack2) && (bOffsetPerformedTmp3FbkStack2)) {
            bOffsetPerformedTempFbkStack2 = 1;
        } else {
            bOffsetPerformedTempFbkStack2 = 0;
        }

        
        
        if (bOffsetCalcAllowVoltag2Stack1) {
            ADC2Stack1Offset[ADC2_STACK_DCVOLTAGE2] = ADC2Stack1OffsetBuffer[ADC2_STACK_DCVOLTAGE2] / sControlConfig.u32CalibCount;
            bOffsetPerformedVoltag2Stack1 = 1;
        }
        if (bOffsetCalcAllowVoltag2Stack2) {
            ADC2Stack2Offset[ADC2_STACK_DCVOLTAGE2] = ADC2Stack2OffsetBuffer[ADC2_STACK_DCVOLTAGE2] / sControlConfig.u32CalibCount;
            bOffsetPerformedVoltag2Stack2 = 1;
        }


        if ((bOffsetPerformedVoltageStack1) && (bOffsetPerformedVoltageStack2) && (bOffsetPerformedCurrentStack1) && (bOffsetPerformedCurrentStack2) && (bOffsetPerformedTempFbkStack1) && (bOffsetPerformedCurrFbkStack1)) {
            bAllOffsetPerformed = 1;
        } else {
            bAllOffsetPerformed = 0;
        }
        
        /* force offsets with average */
        #if BOARDCFG_USE_INFO_CALC_MIN_MAX_AVG
        if (bUseRuntimeOffsetFix)
        {
            ADCStack1Offset[ADC_STACK_CURRENT_L1] = ADCStack1ResultAvg[ADC_STACK_CURRENT_L1];
            ADCStack1Offset[ADC_STACK_CURRENT_L2] = ADCStack1ResultAvg[ADC_STACK_CURRENT_L2];
            ADCStack1Offset[ADC_STACK_CURRENT_L3] = ADCStack1ResultAvg[ADC_STACK_CURRENT_L3];
            ADCStack2Offset[ADC_STACK_CURRENT_L1] = ADCStack2ResultAvg[ADC_STACK_CURRENT_L1];
            ADCStack2Offset[ADC_STACK_CURRENT_L2] = ADCStack2ResultAvg[ADC_STACK_CURRENT_L2];
            ADCStack2Offset[ADC_STACK_CURRENT_L3] = ADCStack2ResultAvg[ADC_STACK_CURRENT_L3];
            ADCStack1Offset[ADC_STACK_DC_VOLTAGE] = ADCStack1ResultAvg[ADC_STACK_DC_VOLTAGE];
            ADCStack2Offset[ADC_STACK_DC_VOLTAGE] = ADCStack2ResultAvg[ADC_STACK_DC_VOLTAGE];  
        }
        #endif

        #if BOARDCFG_USE_TSC_DSPIC
        #if BOARDCFG_USE_DAC_TRIP

        /* DAC Offset */
        
        #if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0

        DACOffsetCurrentStack1[0] = MCLIB_s16Mul_s16u32(ADCStack1Offset[ADC_STACK_CURRENT_L1], sControlConfig.u32ADCResultToDACResultAmperesMultiplier);
        DACOffsetCurrentStack1[1] = MCLIB_s16Mul_s16u32(ADCStack1Offset[ADC_STACK_CURRENT_L2], sControlConfig.u32ADCResultToDACResultAmperesMultiplier);
        DACOffsetCurrentStack1[2] = MCLIB_s16Mul_s16u32(ADCStack1Offset[ADC_STACK_CURRENT_L3], sControlConfig.u32ADCResultToDACResultAmperesMultiplier);

        DACOffsetVoltageStack1 = MCLIB_s16Mul_s16u32(ADCStack1Offset[ADC_STACK_DC_VOLTAGE], sControlConfig.u32ADCResultToDACResultVoltageMultiplier);

        DACOffsetCurrentStack2[0] = MCLIB_s16Mul_s16u32(ADCStack2Offset[ADC_STACK_CURRENT_L1], sControlConfig.u32ADCResultToDACResultAmperesMultiplier);
        DACOffsetCurrentStack2[1] = MCLIB_s16Mul_s16u32(ADCStack2Offset[ADC_STACK_CURRENT_L2], sControlConfig.u32ADCResultToDACResultAmperesMultiplier);
        DACOffsetCurrentStack2[2] = MCLIB_s16Mul_s16u32(ADCStack2Offset[ADC_STACK_CURRENT_L3], sControlConfig.u32ADCResultToDACResultAmperesMultiplier);

        DACOffsetVoltageStack2 = MCLIB_s16Mul_s16u32(ADCStack2Offset[ADC_STACK_DC_VOLTAGE], sControlConfig.u32ADCResultToDACResultVoltageMultiplier);



        DACOffsetCurrFbkStack1[0] = MCLIB_s16Mul_s16u32(ADC2Stack1Offset[ADC2_STACK_CURRFBK_L1], sControlConfig.u32ADCResultToDACResultAmperesMultiplier);
        DACOffsetCurrFbkStack1[1] = MCLIB_s16Mul_s16u32(ADC2Stack1Offset[ADC2_STACK_CURRFBK_L2], sControlConfig.u32ADCResultToDACResultAmperesMultiplier);
        DACOffsetCurrFbkStack1[2] = MCLIB_s16Mul_s16u32(ADC2Stack1Offset[ADC2_STACK_CURRFBK_L3], sControlConfig.u32ADCResultToDACResultAmperesMultiplier);

        DACOffsetVoltage2Stack1 = MCLIB_s16Mul_s16u32(ADC2Stack1Offset[ADC2_STACK_DCVOLTAGE2], sControlConfig.u32ADCResultToDACResultVoltageMultiplier);
        DACOffsetVoltage2Stack2 = MCLIB_s16Mul_s16u32(ADC2Stack2Offset[ADC2_STACK_DCVOLTAGE2], sControlConfig.u32ADCResultToDACResultVoltageMultiplier);


        TripCurrentStack1Offset[0] = MCLIB_s16Mul_s16u32(DACOffsetCurrentStack1[0], sControlConfig.u32DACResultToDeciAmperesMultiplier);
        TripCurrentStack1Offset[1] = MCLIB_s16Mul_s16u32(DACOffsetCurrentStack1[1], sControlConfig.u32DACResultToDeciAmperesMultiplier);
        TripCurrentStack1Offset[2] = MCLIB_s16Mul_s16u32(DACOffsetCurrentStack1[2], sControlConfig.u32DACResultToDeciAmperesMultiplier);
        TripCurrentStack2Offset[0] = MCLIB_s16Mul_s16u32(DACOffsetCurrentStack2[0], sControlConfig.u32DACResultToDeciAmperesMultiplier);
        TripCurrentStack2Offset[1] = MCLIB_s16Mul_s16u32(DACOffsetCurrentStack2[1], sControlConfig.u32DACResultToDeciAmperesMultiplier);
        TripCurrentStack2Offset[2] = MCLIB_s16Mul_s16u32(DACOffsetCurrentStack2[2], sControlConfig.u32DACResultToDeciAmperesMultiplier);

        TripVoltageStack1Offset = MCLIB_s16Mul_s16u32(DACOffsetVoltageStack1, sControlConfig.u32DACResultToDeciVoltageMultiplier);
        TripVoltageStack2Offset = MCLIB_s16Mul_s16u32(DACOffsetVoltageStack2, sControlConfig.u32DACResultToDeciVoltageMultiplier);


        TripCFBKStack1Offset = MCLIB_s16Mul_s16u32(DACOffsetCurrFbkStack1[0], sControlConfig.u32DACResultToDeciAmperesMultiplier);
        //TripCFBKStack2Offset = MCLIB_s16Mul_s16u32(DACOffsetCurrFbkStack1[0],sControlConfig.u32DACResultToDeciAmperesMultiplier);   /* To do: use offset cfbk stack2 */
        
        #else

        DACOffsetCurrentStack1[0] = MCLIB_u16Mul_u16u32(ADCStack1Offset[ADC_STACK_CURRENT_L1], sControlConfig.u32ADCResultToDACResultAmperesMultiplier);
        DACOffsetCurrentStack1[1] = MCLIB_u16Mul_u16u32(ADCStack1Offset[ADC_STACK_CURRENT_L2], sControlConfig.u32ADCResultToDACResultAmperesMultiplier);
        DACOffsetCurrentStack1[2] = MCLIB_u16Mul_u16u32(ADCStack1Offset[ADC_STACK_CURRENT_L3], sControlConfig.u32ADCResultToDACResultAmperesMultiplier);

        DACOffsetVoltageStack1 = MCLIB_u16Mul_u16u32(ADCStack1Offset[ADC_STACK_DC_VOLTAGE], sControlConfig.u32ADCResultToDACResultVoltageMultiplier);

        DACOffsetCurrentStack2[0] = MCLIB_u16Mul_u16u32(ADCStack2Offset[ADC_STACK_CURRENT_L1], sControlConfig.u32ADCResultToDACResultAmperesMultiplier);
        DACOffsetCurrentStack2[1] = MCLIB_u16Mul_u16u32(ADCStack2Offset[ADC_STACK_CURRENT_L2], sControlConfig.u32ADCResultToDACResultAmperesMultiplier);
        DACOffsetCurrentStack2[2] = MCLIB_u16Mul_u16u32(ADCStack2Offset[ADC_STACK_CURRENT_L3], sControlConfig.u32ADCResultToDACResultAmperesMultiplier);

        //DACOffsetVoltageStack2 = MCLIB_s16Mul_s16u32(ADCStack2Offset[ADC_STACK_DC_VOLTAGE], sControlConfig.u32ADCResultToDACResultVoltageMultiplier);

        

        //DACOffsetCurrFbkStack1[0] = MCLIB_s16Mul_s16u32(ADC2Stack1Offset[ADC2_STACK_CURRFBK_L1], sControlConfig.u32ADCResultToDACResultAmperesMultiplier);
        //DACOffsetCurrFbkStack1[1] = MCLIB_s16Mul_s16u32(ADC2Stack1Offset[ADC2_STACK_CURRFBK_L2], sControlConfig.u32ADCResultToDACResultAmperesMultiplier);
        //DACOffsetCurrFbkStack1[2] = MCLIB_s16Mul_s16u32(ADC2Stack1Offset[ADC2_STACK_CURRFBK_L3], sControlConfig.u32ADCResultToDACResultAmperesMultiplier);

        //DACOffsetVoltage2Stack1 = MCLIB_s16Mul_s16u32(ADC2Stack1Offset[ADC2_STACK_DCVOLTAGE2], sControlConfig.u32ADCResultToDACResultVoltageMultiplier);
        //DACOffsetVoltage2Stack2 = MCLIB_s16Mul_s16u32(ADC2Stack2Offset[ADC2_STACK_DCVOLTAGE2], sControlConfig.u32ADCResultToDACResultVoltageMultiplier);

        DACOffsetCurrentStack1Avg = ( (DACOffsetCurrentStack1[0] + DACOffsetCurrentStack1[1] + DACOffsetCurrentStack1[2]) / 3 );
        DACOffsetCurrentStack2Avg = ( (DACOffsetCurrentStack2[0] + DACOffsetCurrentStack2[1] + DACOffsetCurrentStack2[2]) / 3 );

        TripCurrentStack1Offset[0] = TripCurrentStack1Offset[1] = MCLIB_u16Mul_u16u32(DACOffsetCurrentStack1Avg, sControlConfig.u32DACResultToDeciAmperesMultiplier);
        TripCurrentStack2Offset[0] = TripCurrentStack2Offset[1] = MCLIB_u16Mul_u16u32(DACOffsetCurrentStack2Avg, sControlConfig.u32DACResultToDeciAmperesMultiplier);

        TripVoltageStack1Offset = MCLIB_u16Mul_u16u32(DACOffsetVoltageStack1, sControlConfig.u32DACResultToDeciVoltageMultiplier);
        TripVoltageStack2Offset = MCLIB_u16Mul_u16u32(DACOffsetVoltageStack1, sControlConfig.u32DACResultToDeciVoltageMultiplier);
        
        #endif
        
        #endif
        #endif

        /* Control Initialization before run mode */

        //vCurrentVoltageFbkPrepare();



        vRunInitialize();


        sControlData.u16SkippedPulses = 0;

        sControlData.u32SkippedPulsesProcessed = 0;

        if (sControlAccess.bStartCalibOnly) {
            sControlAccess.bStart = 0;
            sControlAccess.bStartCalibOnly = 0;
        } else {
            /* start actuators after calibration if needed */
            PWM_vOutputEnable();
            #if BOARDCFG_USE_FPGA
            //FPGA_COMMON_vEnablePWM(); was commented - should be commanded by bPWMEnableRequest
            #endif
            bPWMEnableRequest = 1;

            if (bUseRunDelay) {
                eControlState = CTRL_STA_RUN_DELAY;
            } else {
                eControlState = CTRL_STA_RUN;
                CSMON_vSetStartRecorderParameterMask(CTRL_u8MaskStartRecorderRunModeEnter);
                vStackPrescalersInit();
            }

        }


    }

    /* Stop Request Check */
    if (sControlAccess.bStart == 0) {
        /* stop actuators */
        eControlState = CTRL_STA_INIT;
#if BOARDCFG_USE_FPGA
        FPGA_COMMON_vDisablePWM();
#endif
        PWM_vOutputDisable();
        bPWMEnableRequest = 0;

    }
    /* Fault Detected Check */
    if (sControlAccess.bFault == 1) {
        /* stop actuators */
        eControlState = CTRL_STA_FAULT;
#if BOARDCFG_USE_FPGA
        FPGA_COMMON_vDisablePWM();
#endif
        PWM_vOutputDisable();
        bPWMEnableRequest = 0;
    }


}

int16_t p1, p2, p3, p4;

void vProcessStack(sStackData_t* psStack) {
    //    if (sControlAccess.bEmulateVoltageFbck)
    //    {
    //        psStack->u16VoltageDCBus =
    //                psStack->u16VoltageDCBusFbckEmulation;
    //    }
    
    
    
    if (psStack->u16ControlPrescalerCounter < UINT16_MAX)
    {
        psStack->u16ControlPrescalerCounter++;
    }

    if (psStack->u16ControlPrescalerCounter >= psStack->u16ControlPrescaler)
    {
        psStack->u16ControlPrescalerCounter = 0;
    
        /* Request Limitation */
        MCLIB_mValueLimitWithMaxMin(psStack->s16CurrentRMSReq1Ph, sControlLimit.s16CurrentRequestMax, 0);

        /* Current Request RMS 3-Phases from RMS 1-Phase */
        //psStack->s16CurrentRMSReq = MCLIB_s16MulSat_s16u32(psStack->s16CurrentRMSReq1Ph, MCLIB_u32FractionalQ_16_16(MCLIB_1PH_TO_3PH_RMS));
        psStack->s16CurrentRMSReq = MCLIB_s16Mul_s16u16(psStack->s16CurrentRMSReq1Ph, MCLIB_u16Fractional(MCLIB_1PH_TO_3PH_RMS/2)); /* (MCLIB_1PH_TO_3PH_RMS/2) < 1 */
        psStack->s16CurrentRMSReq = MCLIB_s16MulSat_s16u16Int(psStack->s16CurrentRMSReq, 2);

        
        /* Position Calculation */
        psStack->s16Position = MCLIB_s16Integrator(
                psStack->s16Speed,
                &psStack->sPositionIntegrator);

        //__builtin_disable_interrupts();
        /* Sine & Cosine */
        MCLIB_vSinCos(
                psStack->s16Position,
                &psStack->sPositionSinCos);
        //__builtin_enable_interrupts();
        
        
    //DEBUG_SET_CTRL();  
        if (sControlAccess.bEmulateCurrentFbck) {
            #if CONTROL_USE_EMULATION_POSITION_FROM_GENERATION_POSITION == 0

            /* PositionFbckEmulation Calculation */
            psStack->s16PositionFbckEmulation = MCLIB_s16Integrator(
                    psStack->s16SpeedFbckEmulation,
                    &psStack->sPositionFbckEmulationIntegrator);

            /* Sine & Cosine */
            MCLIB_vSinCos(
                    psStack->s16PositionFbckEmulation,
                    &psStack->sPositionSinCosFbckEmulation);
            #endif

            /* RMS 3-Phases from RMS 1-Phase */
            //psStack->s16CurrentRMSFbckEmulation = MCLIB_s16Mul_s16u32(psStack->s16CurrentRMS1PhFbckEmulation, MCLIB_u32FractionalQ_16_16(MCLIB_1PH_TO_3PH_RMS));
            psStack->s16CurrentRMSFbckEmulation = MCLIB_s16Mul_s16u16(psStack->s16CurrentRMS1PhFbckEmulation, MCLIB_u16Fractional(MCLIB_1PH_TO_3PH_RMS/2)); /* (MCLIB_1PH_TO_3PH_RMS/2) < 1 */
            psStack->s16CurrentRMSFbckEmulation = MCLIB_s16MulSat_s16u16Int(psStack->s16CurrentRMSFbckEmulation, 2);

            /* Amp from RMS 3-Phases */
            //psStack->s16CurrentAmpFbckEmulation = MCLIB_s16Mul_s16u32(psStack->s16CurrentRMSFbckEmulation, MCLIB_u32FractionalQ_16_16(MCLIB_3PH_RMS_TO_AMP));
            psStack->s16CurrentAmpFbckEmulation = MCLIB_s16Mul_s16u16(psStack->s16CurrentRMSFbckEmulation, MCLIB_u16Fractional(MCLIB_3PH_RMS_TO_AMP)); /* (MCLIB_3PH_RMS_TO_AMP) < 1 */

            /* Assume d-vector s16CurrentAmpFbckEmulation */
            /* DQ */
            psStack->sCurrentDQFbckEmulation.s16Arg1 =
                    psStack->s16CurrentAmpFbckEmulation;
            psStack->sCurrentDQFbckEmulation.s16Arg2 = 0;

            /* AlBe */
            MCLIB_vParkInv(
                    &psStack->sCurrentAlBeFbckEmulation,
            #if CONTROL_USE_EMULATION_POSITION_FROM_GENERATION_POSITION == 0
                    &psStack->sPositionSinCosFbckEmulation,
            #else
                    &psStack->sPositionSinCos,
            #endif
                    &psStack->sCurrentDQFbckEmulation);

            /* ABC */
            MCLIB_vClarkeInv(
                    &psStack->sCurrentABCFbck,
                    &psStack->sCurrentAlBeFbckEmulation,
                    s16EmulStackTemp1, s16EmulStackTemp2);

        }

    //DEBUG_CLR_CTRL();  

        /* Round Trigger - still not used  */
        #if 0 
        psStack->u16RoundTicksCounter++;
        if ((psStack->s16Position >= 0) && (psStack->s16PositionOld < 0)) {
            psStack->u16RoundTicksCount = psStack->u16RoundTicksCounter;
            psStack->u16RoundTicksCounter = 0;
        }
        psStack->s16PositionOld = psStack->s16Position;
        #endif

        /* Clarke Transformation */
        MCLIB_vClarke(&psStack->sCurrentAlBeFbck,
                &psStack->sCurrentABCFbck);



    //DEBUG_SET_CTRL();  
        if (sControlAccess.bCalculateRMSDCBus)
        {
//#if USE_NEW_RMS
            if (bUseNewRMS)
            {
            psStack->u32VoltageDCBusPower2 = MCLIB_u32Power2(psStack->u16VoltageDCBus);   
                
            psStack->u32VoltageDCBusPower2Avg = MCLIB_u32FilterAvg(
                    psStack->u32VoltageDCBusPower2 ,
                    &psStack->sVoltageDCBusRMSFiltNew);
            
            psStack->u16VoltageDCBusRMSFiltPass1 = MCLIB_u16SQRT_u32(psStack->u32VoltageDCBusPower2Avg);
            
            psStack->u16VoltageDCBusRMSFiltPass2 = MCLIB_u16FilterAvg(
                    psStack->u16VoltageDCBusRMSFiltPass1,
                    &psStack->sVoltageDCBusRMSFiltNewPass2);
           
            psStack->u16VoltageDCBusRMSFilt = psStack->u16VoltageDCBusRMSFiltPass2;
            }
//#else
            else
            {
            /* find count leading bits */
            psStack->u16ShiftIncResolutionLastDCBus =
                    MCLIB_u16CountLeadingBits_u16(psStack->u16VoltageDCBus);

            psStack->u16VoltageDCBusIncResolution =
                    psStack->u16VoltageDCBus;

            psStack->u16VoltageDCBusIncResolution <<=
                    psStack->u16ShiftIncResolutionLastDCBus;

            psStack->u16VoltageDCBusFiltPower2 = MCLIB_u16FilterMA(
                    MCLIB_u16Power2(psStack->u16VoltageDCBusIncResolution),
                    &psStack->sVoltageDCBusRootMeanFilter);

            psStack->u16VoltageDCBusRMS = MCLIB_u16SQRT(
                    psStack->u16VoltageDCBusFiltPower2);

            psStack->u16VoltageDCBusRMS >>=
                    psStack->u16ShiftIncResolutionLastDCBus;

            psStack->u16VoltageDCBusRMSFilt = MCLIB_u16FilterMA(
                    psStack->u16VoltageDCBusRMS,
                    &psStack->sVoltageDCBusRMSFilter);
            }
//#endif
        } /* if (sControlAccess.bCalculateRMSDCBus) */

    //DEBUG_CLR_CTRL();  





        if (sControlAccess.bCalculateRMSABC)
        {
//#if USE_NEW_RMS
            if (bUseNewRMS)
            {
                
            psStack->sCurrentABCFbckPower2.s32Arg1 = MCLIB_s32Power2(psStack->sCurrentABCFbck.s16Arg1);   
            psStack->sCurrentABCFbckPower2.s32Arg2 = MCLIB_s32Power2(psStack->sCurrentABCFbck.s16Arg2);   
            psStack->sCurrentABCFbckPower2.s32Arg3 = MCLIB_s32Power2(psStack->sCurrentABCFbck.s16Arg3);   
                
            psStack->sCurrentABCFbckPower2Avg.s32Arg1 = MCLIB_s32FilterAvg(
                    psStack->sCurrentABCFbckPower2.s32Arg1 ,
                    &psStack->sCurrentAFbckRMSFiltNew);
            psStack->sCurrentABCFbckPower2Avg.s32Arg2 = MCLIB_s32FilterAvg(
                    psStack->sCurrentABCFbckPower2.s32Arg2 ,
                    &psStack->sCurrentBFbckRMSFiltNew);
            psStack->sCurrentABCFbckPower2Avg.s32Arg3 = MCLIB_s32FilterAvg(
                    psStack->sCurrentABCFbckPower2.s32Arg3 ,
                    &psStack->sCurrentCFbckRMSFiltNew);
            
            psStack->sCurrentABCFbckRMSFiltPass1.s16Arg1 = MCLIB_s16SQRT_s32(psStack->sCurrentABCFbckPower2Avg.s32Arg1);
            psStack->sCurrentABCFbckRMSFiltPass1.s16Arg2 = MCLIB_s16SQRT_s32(psStack->sCurrentABCFbckPower2Avg.s32Arg2);
            psStack->sCurrentABCFbckRMSFiltPass1.s16Arg3 = MCLIB_s16SQRT_s32(psStack->sCurrentABCFbckPower2Avg.s32Arg3);
            
            psStack->sCurrentABCFbckRMSFiltPass2.s16Arg1 = MCLIB_s16FilterAvg(
                    psStack->sCurrentABCFbckRMSFiltPass1.s16Arg1,
                    &psStack->sCurrentAFbckRMSFiltNewPass2);
            psStack->sCurrentABCFbckRMSFiltPass2.s16Arg2 = MCLIB_s16FilterAvg(
                    psStack->sCurrentABCFbckRMSFiltPass1.s16Arg2,
                    &psStack->sCurrentBFbckRMSFiltNewPass2);
            psStack->sCurrentABCFbckRMSFiltPass2.s16Arg3 = MCLIB_s16FilterAvg(
                    psStack->sCurrentABCFbckRMSFiltPass1.s16Arg3,
                    &psStack->sCurrentCFbckRMSFiltNewPass2);
           
            psStack->sCurrentABCFbckRMSFilt.s16Arg1 = psStack->sCurrentABCFbckRMSFiltPass2.s16Arg1;
            psStack->sCurrentABCFbckRMSFilt.s16Arg2 = psStack->sCurrentABCFbckRMSFiltPass2.s16Arg2;
            psStack->sCurrentABCFbckRMSFilt.s16Arg3 = psStack->sCurrentABCFbckRMSFiltPass2.s16Arg3;
            
            }
//#else
            else
            {
            /* find count leading bits */
            psStack->u16ShiftIncResolutionRMSABC = MCLIB_u16Min(
                    MCLIB_u16CountLeadingBits_s16(
                    psStack->sCurrentABCFbck.s16Arg1),
                    MCLIB_u16CountLeadingBits_s16(
                    psStack->sCurrentABCFbck.s16Arg2));

            psStack->u16ShiftIncResolutionRMSABC = MCLIB_u16Min(
                    psStack->u16ShiftIncResolutionRMSABC,
                    MCLIB_u16CountLeadingBits_s16(
                    psStack->sCurrentABCFbck.s16Arg3));

            /* get values for resolution increase ABC */
            psStack->sCurrentABCFbckIncResolution.s16Arg1 =
                    psStack->sCurrentABCFbck.s16Arg1;
            psStack->sCurrentABCFbckIncResolution.s16Arg2 =
                    psStack->sCurrentABCFbck.s16Arg2;
            psStack->sCurrentABCFbckIncResolution.s16Arg3 =
                    psStack->sCurrentABCFbck.s16Arg3;

            /* Feedback Increase Resolution ABC */
            psStack->sCurrentABCFbckIncResolution.s16Arg1 <<=
                    psStack->u16ShiftIncResolutionRMSABC;
            psStack->sCurrentABCFbckIncResolution.s16Arg2 <<=
                    psStack->u16ShiftIncResolutionRMSABC;
            psStack->sCurrentABCFbckIncResolution.s16Arg3 <<=
                    psStack->u16ShiftIncResolutionRMSABC;


            /* Filter Current^2 Each Phase */
            /* sCurrentAFbckFilter */
            psStack->sCurrentABCFbckFilt.s16Arg1 = MCLIB_s16FilterMA(
                    MCLIB_s16Power2(psStack->sCurrentABCFbckIncResolution.s16Arg1),
                    &psStack->sCurrentAFbckFilter);

            /* sCurrentBFbckFilter */
            psStack->sCurrentABCFbckFilt.s16Arg2 = MCLIB_s16FilterMA(
                    MCLIB_s16Power2(psStack->sCurrentABCFbckIncResolution.s16Arg2),
                    &psStack->sCurrentBFbckFilter);

            /* sCurrentCFbckFilter */
            psStack->sCurrentABCFbckFilt.s16Arg3 = MCLIB_s16FilterMA(
                    MCLIB_s16Power2(psStack->sCurrentABCFbckIncResolution.s16Arg3),
                    &psStack->sCurrentCFbckFilter);

            /* RMS Current Each Phase */
            /* Phase A */
            psStack->sCurrentABCFbckRMS.s16Arg1 = MCLIB_s16SQRT(
                    psStack->sCurrentABCFbckFilt.s16Arg1);

            /* Phase B */
            psStack->sCurrentABCFbckRMS.s16Arg2 = MCLIB_s16SQRT(
                    psStack->sCurrentABCFbckFilt.s16Arg2);

            /* Phase C */
            psStack->sCurrentABCFbckRMS.s16Arg3 = MCLIB_s16SQRT(
                    psStack->sCurrentABCFbckFilt.s16Arg3);





            /* Fix resolution RMS ABC */
            psStack->sCurrentABCFbckRMS.s16Arg1 >>= psStack->u16ShiftIncResolutionRMSABC;
            psStack->sCurrentABCFbckRMS.s16Arg2 >>= psStack->u16ShiftIncResolutionRMSABC;
            psStack->sCurrentABCFbckRMS.s16Arg3 >>= psStack->u16ShiftIncResolutionRMSABC;

            /* Filter RMS ABC */
            psStack->sCurrentABCFbckRMSFilt.s16Arg1 = MCLIB_s16FilterMA(
                    psStack->sCurrentABCFbckRMS.s16Arg1,
                    &psStack->sCurrentAFbckRMSFilter);
            psStack->sCurrentABCFbckRMSFilt.s16Arg2 = MCLIB_s16FilterMA(
                    psStack->sCurrentABCFbckRMS.s16Arg2,
                    &psStack->sCurrentBFbckRMSFilter);
            psStack->sCurrentABCFbckRMSFilt.s16Arg3 = MCLIB_s16FilterMA(
                    psStack->sCurrentABCFbckRMS.s16Arg3,
                    &psStack->sCurrentCFbckRMSFilter);
            }
//#endif
        } /* if (sControlAccess.bCalculateRMSABC) */




        if (sControlAccess.bCalculateCFBKRMSABC)
        {
//#if USE_NEW_RMS
            if (bUseNewRMS)
            {
                
            psStack->sCFBKABCFbckPower2.s32Arg1 = MCLIB_s32Power2(psStack->sCFBKABCFbck.s16Arg1);   
            psStack->sCFBKABCFbckPower2.s32Arg2 = MCLIB_s32Power2(psStack->sCFBKABCFbck.s16Arg2);   
            psStack->sCFBKABCFbckPower2.s32Arg3 = MCLIB_s32Power2(psStack->sCFBKABCFbck.s16Arg3);   
                
            psStack->sCFBKABCFbckPower2Avg.s32Arg1 = MCLIB_s32FilterAvg(
                    psStack->sCFBKABCFbckPower2.s32Arg1 ,
                    &psStack->sCFBKAFbckRMSFiltNew);
            psStack->sCFBKABCFbckPower2Avg.s32Arg2 = MCLIB_s32FilterAvg(
                    psStack->sCFBKABCFbckPower2.s32Arg2 ,
                    &psStack->sCFBKBFbckRMSFiltNew);
            psStack->sCFBKABCFbckPower2Avg.s32Arg3 = MCLIB_s32FilterAvg(
                    psStack->sCFBKABCFbckPower2.s32Arg3 ,
                    &psStack->sCFBKCFbckRMSFiltNew);
            
            psStack->sCFBKABCFbckRMSFilt.s16Arg1 = MCLIB_s16SQRT_s32(psStack->sCFBKABCFbckPower2Avg.s32Arg1);
            psStack->sCFBKABCFbckRMSFilt.s16Arg2 = MCLIB_s16SQRT_s32(psStack->sCFBKABCFbckPower2Avg.s32Arg2);
            psStack->sCFBKABCFbckRMSFilt.s16Arg3 = MCLIB_s16SQRT_s32(psStack->sCFBKABCFbckPower2Avg.s32Arg3);

            psStack->sCFBKABCFbckRMSFiltPass1.s16Arg1 = MCLIB_s16SQRT_s32(psStack->sCFBKABCFbckPower2Avg.s32Arg1);
            psStack->sCFBKABCFbckRMSFiltPass1.s16Arg2 = MCLIB_s16SQRT_s32(psStack->sCFBKABCFbckPower2Avg.s32Arg2);
            psStack->sCFBKABCFbckRMSFiltPass1.s16Arg3 = MCLIB_s16SQRT_s32(psStack->sCFBKABCFbckPower2Avg.s32Arg3);
            
            psStack->sCFBKABCFbckRMSFiltPass2.s16Arg1 = MCLIB_s16FilterAvg(
                    psStack->sCFBKABCFbckRMSFiltPass1.s16Arg1,
                    &psStack->sCFBKAFbckRMSFiltNewPass2);
            psStack->sCFBKABCFbckRMSFiltPass2.s16Arg2 = MCLIB_s16FilterAvg(
                    psStack->sCFBKABCFbckRMSFiltPass1.s16Arg2,
                    &psStack->sCFBKBFbckRMSFiltNewPass2);
            psStack->sCFBKABCFbckRMSFiltPass2.s16Arg3 = MCLIB_s16FilterAvg(
                    psStack->sCFBKABCFbckRMSFiltPass1.s16Arg3,
                    &psStack->sCFBKCFbckRMSFiltNewPass2);
           
            psStack->sCFBKABCFbckRMSFilt.s16Arg1 = psStack->sCFBKABCFbckRMSFiltPass2.s16Arg1;
            psStack->sCFBKABCFbckRMSFilt.s16Arg2 = psStack->sCFBKABCFbckRMSFiltPass2.s16Arg2;
            psStack->sCFBKABCFbckRMSFilt.s16Arg3 = psStack->sCFBKABCFbckRMSFiltPass2.s16Arg3;





            /* sCFBKAFbckFilterAvg */
            psStack->sCFBKABCFbckFilt.s16Arg1 = MCLIB_s16FilterAvg(psStack->sCFBKABCFbck.s16Arg1, &psStack->sCFBKAFbckFilterAvg);

            /* sCFBKBFbckFilterAvg */
            psStack->sCFBKABCFbckFilt.s16Arg2 = MCLIB_s16FilterAvg(psStack->sCFBKABCFbck.s16Arg2, &psStack->sCFBKBFbckFilterAvg);

            /* sCFBKCFbckFilterAvg */
            psStack->sCFBKABCFbckFilt.s16Arg3 = MCLIB_s16FilterAvg(psStack->sCFBKABCFbck.s16Arg3, &psStack->sCFBKCFbckFilterAvg);





            }
//#else
            else
            {
            /* find count leading bits */
            psStack->u16ShiftIncResolutionRMSCFBK = MCLIB_u16Min(
                    MCLIB_u16CountLeadingBits_s16(
                    psStack->sCFBKABCFbck.s16Arg1),
                    MCLIB_u16CountLeadingBits_s16(
                    psStack->sCFBKABCFbck.s16Arg2));

            psStack->u16ShiftIncResolutionRMSCFBK = MCLIB_u16Min(
                    psStack->u16ShiftIncResolutionRMSCFBK,
                    MCLIB_u16CountLeadingBits_s16(
                    psStack->sCFBKABCFbck.s16Arg3));

            /* get values for resolution increase ABC */
            psStack->sCFBKABCFbckIncResolution.s16Arg1 =
                    psStack->sCFBKABCFbck.s16Arg1;
            psStack->sCFBKABCFbckIncResolution.s16Arg2 =
                    psStack->sCFBKABCFbck.s16Arg2;
            psStack->sCFBKABCFbckIncResolution.s16Arg3 =
                    psStack->sCFBKABCFbck.s16Arg3;

            /* Feedback Increase Resolution ABC */
            psStack->sCFBKABCFbckIncResolution.s16Arg1 <<=
                    psStack->u16ShiftIncResolutionRMSCFBK;
            psStack->sCFBKABCFbckIncResolution.s16Arg2 <<=
                    psStack->u16ShiftIncResolutionRMSCFBK;
            psStack->sCFBKABCFbckIncResolution.s16Arg3 <<=
                    psStack->u16ShiftIncResolutionRMSCFBK;


            /* Filter CFBK^2 Each Phase */
            /* sCFBKAFbckFilter */
            psStack->sCFBKABCFbckFilt.s16Arg1 = MCLIB_s16FilterMA(
                    MCLIB_s16Power2(psStack->sCFBKABCFbckIncResolution.s16Arg1),
                    &psStack->sCFBKAFbckFilter);

            /* sCFBKBFbckFilter */
            psStack->sCFBKABCFbckFilt.s16Arg2 = MCLIB_s16FilterMA(
                    MCLIB_s16Power2(psStack->sCFBKABCFbckIncResolution.s16Arg2),
                    &psStack->sCFBKBFbckFilter);

            /* sCFBKCFbckFilter */
            psStack->sCFBKABCFbckFilt.s16Arg3 = MCLIB_s16FilterMA(
                    MCLIB_s16Power2(psStack->sCFBKABCFbckIncResolution.s16Arg3),
                    &psStack->sCFBKCFbckFilter);

            /* RMS CFBK Each Phase */
            /* Phase A */
            psStack->sCFBKABCFbckRMS.s16Arg1 = MCLIB_s16SQRT(
                    psStack->sCFBKABCFbckFilt.s16Arg1);

            /* Phase B */
            psStack->sCFBKABCFbckRMS.s16Arg2 = MCLIB_s16SQRT(
                    psStack->sCFBKABCFbckFilt.s16Arg2);

            /* Phase C */
            psStack->sCFBKABCFbckRMS.s16Arg3 = MCLIB_s16SQRT(
                    psStack->sCFBKABCFbckFilt.s16Arg3);





            /* Fix resolution RMS ABC */
            psStack->sCFBKABCFbckRMS.s16Arg1 >>= psStack->u16ShiftIncResolutionRMSCFBK;
            psStack->sCFBKABCFbckRMS.s16Arg2 >>= psStack->u16ShiftIncResolutionRMSCFBK;
            psStack->sCFBKABCFbckRMS.s16Arg3 >>= psStack->u16ShiftIncResolutionRMSCFBK;

            /* Filter RMS ABC */
            psStack->sCFBKABCFbckRMSFilt.s16Arg1 = MCLIB_s16FilterMA(
                    psStack->sCFBKABCFbckRMS.s16Arg1,
                    &psStack->sCFBKAFbckRMSFilter);
            psStack->sCFBKABCFbckRMSFilt.s16Arg2 = MCLIB_s16FilterMA(
                    psStack->sCFBKABCFbckRMS.s16Arg2,
                    &psStack->sCFBKBFbckRMSFilter);
            psStack->sCFBKABCFbckRMSFilt.s16Arg3 = MCLIB_s16FilterMA(
                    psStack->sCFBKABCFbckRMS.s16Arg3,
                    &psStack->sCFBKCFbckRMSFilter);
            }
//#endif
        } /* if (sControlAccess.bCalculateCFBKRMSABC) */





        /* get values for resolution increase */
        psStack->sCurrentAlBeFbckIncResolution.s16Arg1 =
                psStack->sCurrentAlBeFbck.s16Arg1;
        psStack->sCurrentAlBeFbckIncResolution.s16Arg2 =
                psStack->sCurrentAlBeFbck.s16Arg2;

        /* find count leading bits */
        psStack->u16ShiftIncResolutionLast = MCLIB_u16Min(
                MCLIB_u16CountLeadingBits_s16(
                psStack->sCurrentAlBeFbckIncResolution.s16Arg1),
                MCLIB_u16CountLeadingBits_s16(
                psStack->sCurrentAlBeFbckIncResolution.s16Arg2));

        /* filter 1 bit change jitter */
        #if 0
        if (psStack->u16ShiftIncResolutionLast >
                (psStack->u16ShiftIncResolution + 1)) {
            psStack->u16ShiftIncResolution =
                    psStack->u16ShiftIncResolutionLast;
        }
        if (psStack->u16ShiftIncResolutionLast <
                psStack->u16ShiftIncResolution) {
            psStack->u16ShiftIncResolution =
                    psStack->u16ShiftIncResolutionLast;
        }
        #else
            psStack->u16ShiftIncResolution =
                    psStack->u16ShiftIncResolutionLast;
        #endif



        /* Feedback AlBe Increase Resolution */
        psStack->sCurrentAlBeFbckIncResolution.s16Arg1 <<=
                psStack->u16ShiftIncResolution;
        psStack->sCurrentAlBeFbckIncResolution.s16Arg2 <<=
                psStack->u16ShiftIncResolution;

        /* Stack Debug */
        if (MAIN_bDebugResolutionIncrease) {
            p1 = MCLIB_s16Mul_s16s16(
                    psStack->sCurrentAlBeFbckIncResolution.s16Arg1,
                    psStack->sCurrentAlBeFbckIncResolution.s16Arg1),
                    p2 = MCLIB_s16Mul_s16s16(
                    psStack->sCurrentAlBeFbckIncResolution.s16Arg2,
                    psStack->sCurrentAlBeFbckIncResolution.s16Arg2),
                    p3 = MCLIB_s16AddSat_s16s16(p1, p2);

            p4 = MCLIB_s16SQRT(p3);
        }



        /* Calculate Current Amplitude */
        psStack->s16CurrentAmpFbckIncResolution = MCLIB_s16SQRT(
                MCLIB_s16AddSat_s16s16(
                MCLIB_s16Mul_s16s16(
                psStack->sCurrentAlBeFbckIncResolution.s16Arg1,
                psStack->sCurrentAlBeFbckIncResolution.s16Arg1),
                MCLIB_s16Mul_s16s16(
                psStack->sCurrentAlBeFbckIncResolution.s16Arg2,
                psStack->sCurrentAlBeFbckIncResolution.s16Arg2)));

        psStack->s16CurrentAmpFbck = psStack->s16CurrentAmpFbckIncResolution >> psStack->u16ShiftIncResolution;

        /* Filter CurrentAmp Feedback */
        psStack->s16CurrentAmpFbckFilt = MCLIB_s16FilterMA(
                psStack->s16CurrentAmpFbck,
                &psStack->sCurrentAmpFbckFilt);

        /* RMS from Amp 3Ph */
        psStack->s16CurrentRMSFbck = MCLIB_s16Mul_s16u32(
                psStack->s16CurrentAmpFbck,
                MCLIB_u32FractionalQ_16_16(1.0 / MCLIB_3PH_RMS_TO_AMP));

        /* Error Current Request - Feedback */
        if (psStack->uControlBypass.sFlags.bCurrentError == 0)
        {
            psStack->s16CurrentRMSError = MCLIB_s16SubSat_s16s16(
                    psStack->s16CurrentRMSReq,
                    psStack->s16CurrentRMSFbck);
        }
        else
        {
            psStack->s16CurrentRMSError = 0;
        }

        /* PI Controller RMS Current To RMS Voltage */
        if (psStack->uControlBypass.sFlags.bCurrToVoltPICtrl == 0)
        {
            if (sControlAccess.bUseControllerPI)
            {
                psStack->u16VoltageRMSReq = MCLIB_u16ControllerPIpAW(
                        psStack->s16CurrentRMSError,
                        &psStack->sVoltageReqPI);
            }
        }
        /* Filter DC-Link Voltage */
        psStack->u16VoltageDCBusFilt = MCLIB_u16FilterMA(
                psStack->u16VoltageDCBus,
                &psStack->sVoltageDCBusFilt);


        /* Amplitude from RMS 3Ph - caution overflow not checked */
        if (psStack->uControlBypass.sFlags.bVoltageReq == 0)
        {
            psStack->u16VoltageAmpReq = MCLIB_u16Mul_u16u32(
                    psStack->u16VoltageRMSReq,
                    MCLIB_u32FractionalQ_16_16(MCLIB_3PH_RMS_TO_AMP));
        }

        /* Scale Request With filtered DC-link Value */
        if (psStack->uControlBypass.sFlags.bFixWithDCLink == 0) {
            psStack->s16VoltageAmpScaled = MCLIB_s16FracFromFrac_u16(MCLIB_u16ScaleVoltageWithDCLink(
                    psStack->u16VoltageAmpReq,
                    psStack->u16VoltageDCBusFilt,
                    psStack->u16ScaleModulationIndex));
        } else {
            psStack->s16VoltageAmpScaled = MCLIB_s16FracFromFrac_u16(psStack->u16VoltageAmpReq);
        }



        /* Duty Cycle Limitation Fix */
        if (psStack->s16VoltageAmpScaled >= 0) {
            if (psStack->s16VoltageAmpScaled > sControlData.s16DutyLimitMaxFrac) {
                psStack->s16VoltageAmpScaled = sControlData.s16DutyLimitMaxFrac;
            }
            if (psStack->s16VoltageAmpScaled < sControlData.s16DutyLimitMinFrac) {
                psStack->s16VoltageAmpScaled = sControlData.s16DutyLimitMinFrac;
            }
        } else {
            if (psStack->s16VoltageAmpScaled < MCLIB_s16NegSat(sControlData.s16DutyLimitMaxFrac)) {
                psStack->s16VoltageAmpScaled = MCLIB_s16NegSat(sControlData.s16DutyLimitMaxFrac);
            }
            if (psStack->s16VoltageAmpScaled > MCLIB_s16NegSat(sControlData.s16DutyLimitMinFrac)) {
                psStack->s16VoltageAmpScaled = MCLIB_s16NegSat(sControlData.s16DutyLimitMinFrac);
            }
        }

        /* Assume d-vector s16VoltageAmpScaled */
        /* DQ */
        if (psStack->uControlBypass.sFlags.bVoltageDQReq == 0) {
            psStack->sVoltageDQScaled.s16Arg1 =
                    psStack->s16VoltageAmpScaled;
            psStack->sVoltageDQScaled.s16Arg2 = 0;
        }

        /* AlBe */
        if (psStack->uControlBypass.sFlags.bParkInv == 0) {
            MCLIB_vParkInv(
                    &psStack->sVoltageAlBeScaled,
                    &psStack->sPositionSinCos,
                    &psStack->sVoltageDQScaled);
        }


        if (bUseSVM)
        {
            /* AlBe->ABC */
            if (psStack->uControlBypass.sFlags.bClarkeInv == 0)
            {
                psStack->s16SVMState = MCLIB_PIM_s16SvmStd(&psStack->sVoltageABCScaledSVM, &psStack->sVoltageAlBeScaled);
            }
            /* Duty Cycle SVM Based */
            psStack->sDutyCycleSVM.u16Arg1 =
                    MCLIB_u16Mul_u16u16(MCLIB_u16FracFromFrac_s16(psStack->sVoltageABCScaledSVM.s16Arg1), sControlData.u16PeriodPWM);
            psStack->sDutyCycleSVM.u16Arg2 =
                    MCLIB_u16Mul_u16u16(MCLIB_u16FracFromFrac_s16(psStack->sVoltageABCScaledSVM.s16Arg2), sControlData.u16PeriodPWM);
            psStack->sDutyCycleSVM.u16Arg3 =
                    MCLIB_u16Mul_u16u16(MCLIB_u16FracFromFrac_s16(psStack->sVoltageABCScaledSVM.s16Arg3), sControlData.u16PeriodPWM);

            psStack->sDutyCycle.u16Arg1 = psStack->sDutyCycleSVM.u16Arg1;
            psStack->sDutyCycle.u16Arg2 = psStack->sDutyCycleSVM.u16Arg2;
            psStack->sDutyCycle.u16Arg3 = psStack->sDutyCycleSVM.u16Arg3;
        }
        else
        {
            /* AlBe->ABC */
            if (psStack->uControlBypass.sFlags.bClarkeInv == 0)
            {
                MCLIB_vClarkeInv(
                        &psStack->sVoltageABCScaled,
                        &psStack->sVoltageAlBeScaled,
                        s16StackTemp1, s16StackTemp2);
            }

            /* Duty Cycle Sin Cos Based (No SVM) */
            psStack->sDutyCycleSin.u16Arg1 =
                    (uint16_t) (((uint32_t) (psStack->sVoltageABCScaled.s16Arg1 + MCLIB_u16Fractional(0.5)) *
                    sControlData.u16PeriodPWM) >> 16);
            psStack->sDutyCycleSin.u16Arg2 =
                    (uint16_t) (((uint32_t) (psStack->sVoltageABCScaled.s16Arg2 + MCLIB_u16Fractional(0.5)) *
                    sControlData.u16PeriodPWM) >> 16);
            psStack->sDutyCycleSin.u16Arg3 =
                    (uint16_t) (((uint32_t) (psStack->sVoltageABCScaled.s16Arg3 + MCLIB_u16Fractional(0.5)) *
                    sControlData.u16PeriodPWM) >> 16);
            
            psStack->sDutyCycle.u16Arg1 = psStack->sDutyCycleSin.u16Arg1;
            psStack->sDutyCycle.u16Arg2 = psStack->sDutyCycleSin.u16Arg2;
            psStack->sDutyCycle.u16Arg3 = psStack->sDutyCycleSin.u16Arg3;
        }



        /* Duty Cycle MinMax - consider limitation at coefficient scaled stage */
        if (psStack->sDutyCycle.u16Arg1 < sControlData.u16DutyMin) {
            psStack->sDutyCycle.u16Arg1 = sControlData.u16DutyMin;
        } else
            if (psStack->sDutyCycle.u16Arg1 > sControlData.u16DutyMax) {
            psStack->sDutyCycle.u16Arg1 = sControlData.u16DutyMax;
        }

        if (psStack->sDutyCycle.u16Arg2 < sControlData.u16DutyMin) {
            psStack->sDutyCycle.u16Arg2 = sControlData.u16DutyMin;
        } else
            if (psStack->sDutyCycle.u16Arg2 > sControlData.u16DutyMax) {
            psStack->sDutyCycle.u16Arg2 = sControlData.u16DutyMax;
        }

        if (psStack->sDutyCycle.u16Arg3 < sControlData.u16DutyMin) {
            psStack->sDutyCycle.u16Arg3 = sControlData.u16DutyMin;
        } else
            if (psStack->sDutyCycle.u16Arg3 > sControlData.u16DutyMax) {
            psStack->sDutyCycle.u16Arg3 = sControlData.u16DutyMax;
        }

    }  
    else
    {
        #if CONTROL_USE_EMULATION_POSITION_FROM_GENERATION_POSITION == 0
        if (sControlAccess.bEmulateCurrentFbck) {
            /* PositionFbckEmulation Calculation */
            psStack->s16PositionFbckEmulation = MCLIB_s16Integrator(
                    psStack->s16SpeedFbckEmulation,
                    &psStack->sPositionFbckEmulationIntegrator);
        }
        #endif

        
        /* Fix Duty Cycles here */
        
        /* Position Calculation */
        psStack->s16Position = MCLIB_s16Integrator(
                psStack->s16Speed,
                &psStack->sPositionIntegrator);

        
        
        /* Sine & Cosine */
        MCLIB_vSinCos(
                psStack->s16Position,
                &psStack->sPositionSinCos);

        /* Assume d-vector s16VoltageAmpScaled */
        /* DQ */
        if (psStack->uControlBypass.sFlags.bVoltageDQReq == 0) {
            psStack->sVoltageDQScaled.s16Arg1 =
                    psStack->s16VoltageAmpScaled;
            psStack->sVoltageDQScaled.s16Arg2 = 0;
        }

        /* AlBe */
        if (psStack->uControlBypass.sFlags.bParkInv == 0) {
            MCLIB_vParkInv(
                    &psStack->sVoltageAlBeScaled,
                    &psStack->sPositionSinCos,
                    &psStack->sVoltageDQScaled);
        }

        if (bUseSVM)
        {
            /* AlBe->ABC */
            if (psStack->uControlBypass.sFlags.bClarkeInv == 0)
            {
                psStack->s16SVMState = MCLIB_PIM_s16SvmStd(&psStack->sVoltageABCScaledSVM, &psStack->sVoltageAlBeScaled);
            }
            /* Duty Cycle SVM Based */
            psStack->sDutyCycleSVM.u16Arg1 =
                    MCLIB_u16Mul_u16u16(MCLIB_u16FracFromFrac_s16(psStack->sVoltageABCScaledSVM.s16Arg1), sControlData.u16PeriodPWM);
            psStack->sDutyCycleSVM.u16Arg2 =
                    MCLIB_u16Mul_u16u16(MCLIB_u16FracFromFrac_s16(psStack->sVoltageABCScaledSVM.s16Arg2), sControlData.u16PeriodPWM);
            psStack->sDutyCycleSVM.u16Arg3 =
                    MCLIB_u16Mul_u16u16(MCLIB_u16FracFromFrac_s16(psStack->sVoltageABCScaledSVM.s16Arg3), sControlData.u16PeriodPWM);

            psStack->sDutyCycle.u16Arg1 = psStack->sDutyCycleSVM.u16Arg1;
            psStack->sDutyCycle.u16Arg2 = psStack->sDutyCycleSVM.u16Arg2;
            psStack->sDutyCycle.u16Arg3 = psStack->sDutyCycleSVM.u16Arg3;
        }
        else
        {
            /* AlBe->ABC */
            if (psStack->uControlBypass.sFlags.bClarkeInv == 0)
            {
                MCLIB_vClarkeInv(
                        &psStack->sVoltageABCScaled,
                        &psStack->sVoltageAlBeScaled,
                        s16StackTemp1, s16StackTemp2);
            }

            /* Duty Cycle Sin Cos Based (No SVM) */
            psStack->sDutyCycleSin.u16Arg1 =
                    (uint16_t) (((uint32_t) (psStack->sVoltageABCScaled.s16Arg1 + MCLIB_u16Fractional(0.5)) *
                    sControlData.u16PeriodPWM) >> 16);
            psStack->sDutyCycleSin.u16Arg2 =
                    (uint16_t) (((uint32_t) (psStack->sVoltageABCScaled.s16Arg2 + MCLIB_u16Fractional(0.5)) *
                    sControlData.u16PeriodPWM) >> 16);
            psStack->sDutyCycleSin.u16Arg3 =
                    (uint16_t) (((uint32_t) (psStack->sVoltageABCScaled.s16Arg3 + MCLIB_u16Fractional(0.5)) *
                    sControlData.u16PeriodPWM) >> 16);
            
            psStack->sDutyCycle.u16Arg1 = psStack->sDutyCycleSin.u16Arg1;
            psStack->sDutyCycle.u16Arg2 = psStack->sDutyCycleSin.u16Arg2;
            psStack->sDutyCycle.u16Arg3 = psStack->sDutyCycleSin.u16Arg3;
        }


        /* Duty Cycle MinMax - consider limitation at coefficient scaled stage */
        if (psStack->sDutyCycle.u16Arg1 < sControlData.u16DutyMin) {
            psStack->sDutyCycle.u16Arg1 = sControlData.u16DutyMin;
        } else
            if (psStack->sDutyCycle.u16Arg1 > sControlData.u16DutyMax) {
            psStack->sDutyCycle.u16Arg1 = sControlData.u16DutyMax;
        }

        if (psStack->sDutyCycle.u16Arg2 < sControlData.u16DutyMin) {
            psStack->sDutyCycle.u16Arg2 = sControlData.u16DutyMin;
        } else
            if (psStack->sDutyCycle.u16Arg2 > sControlData.u16DutyMax) {
            psStack->sDutyCycle.u16Arg2 = sControlData.u16DutyMax;
        }

        if (psStack->sDutyCycle.u16Arg3 < sControlData.u16DutyMin) {
            psStack->sDutyCycle.u16Arg3 = sControlData.u16DutyMin;
        } else
            if (psStack->sDutyCycle.u16Arg3 > sControlData.u16DutyMax) {
            psStack->sDutyCycle.u16Arg3 = sControlData.u16DutyMax;
        }

    }
    
  

}



void vStateRunSkipPulse(void)
{
    sControlData.u32SkippedPulsesProcessed++;
#if CONTROL_USE_EMULATION_POSITION_FROM_GENERATION_POSITION == 0
    if (sControlAccess.bEmulateCurrentFbck) {
        /* PositionFbckEmulation Calculation */
        sControlData.sStack[STACK_1].s16PositionFbckEmulation = MCLIB_s16Integrator(
                sControlData.sStack[STACK_1].s16SpeedFbckEmulation,
                &sControlData.sStack[STACK_1].sPositionFbckEmulationIntegrator);

        sControlData.sStack[STACK_2].s16PositionFbckEmulation = MCLIB_s16Integrator(
                sControlData.sStack[STACK_2].s16SpeedFbckEmulation,
                &sControlData.sStack[STACK_2].sPositionFbckEmulationIntegrator);
    }
#endif
    sControlData.sStack[STACK_1].s16Position = MCLIB_s16Integrator(
            sControlData.sStack[STACK_1].s16Speed,
            &sControlData.sStack[STACK_1].sPositionIntegrator);

    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].s16Position = MCLIB_s16Integrator(
            sControlData.sStack[STACK_2].s16Speed,
            &sControlData.sStack[STACK_2].sPositionIntegrator);
    #endif
    
}


void vStateRunDelay(void)
{
    //vCurrentVoltageFbkPrepare();


    /* Filter DC-Link Voltage */
    sControlData.sStack[STACK_1].u16VoltageDCBusFilt = MCLIB_u16FilterMA(
            sControlData.sStack[STACK_1].u16VoltageDCBus,
            &sControlData.sStack[STACK_1].sVoltageDCBusFilt);
    #if BOARDCFG_STACK_COUNT >= 2
    sControlData.sStack[STACK_2].u16VoltageDCBusFilt = MCLIB_u16FilterMA(
            sControlData.sStack[STACK_2].u16VoltageDCBus,
            &sControlData.sStack[STACK_2].sVoltageDCBusFilt);
    #endif


    if (sControlData.u32RunDelayCounter > 0) {
        sControlData.u32RunDelayCounter--;
    }

    if (sControlData.u32RunDelayCounter == 0) {
        eControlState = CTRL_STA_RUN;
        CSMON_vSetStartRecorderParameterMask(CTRL_u8MaskStartRecorderRunModeEnter);
        vStackPrescalersInit();
    }

    /* Stop Request Check */
    if (sControlAccess.bStart == 0) {
        /* stop actuators */
        eControlState = CTRL_STA_INIT;
#if BOARDCFG_USE_FPGA
        FPGA_COMMON_vDisablePWM();
#endif
        PWM_vOutputDisable();
        bPWMEnableRequest = 0;
    }
    /* Fault Detected Check */
    if (sControlAccess.bFault == 1) {
        /* stop actuators */
        eControlState = CTRL_STA_FAULT;
#if BOARDCFG_USE_FPGA
        FPGA_COMMON_vDisablePWM();
#endif
        PWM_vOutputDisable();
        bPWMEnableRequest = 0;
    }
}



void vStateRun(void) {


//#if BOARDCFG_USE_TSC_DSPIC == 0
#if 0
    /* PWM Set */

    if (bProcessAllPWMWithStack1Process)
    {
        ADCPWM_vSetDutyCycleStack1(&sControlData.sStack[STACK_1].sDutyCycle);
        ADCPWM_vSetDutyCycleStack2(&sControlData.sStack[STACK_1].sDutyCycle);
        ADCPWM_vSetDutyCycleStack3(&sControlData.sStack[STACK_1].sDutyCycle);
    }
    else
    {
        ADCPWM_vSetDutyCycleStack1(&sControlData.sStack[STACK_1].sDutyCycle);
        ADCPWM_vSetDutyCycleStack2(&sControlData.sStack[STACK_2].sDutyCycle);
    }
#endif


    /* Request Limitation */
    MCLIB_mValueLimitWithMaxMin(sControlData.sStack[STACK_1].s16Speed, sControlLimit.s16FieldFrequencyRequestMax, sControlLimit.s16FieldFrequencyRequestMin);
    #if BOARDCFG_STACK_COUNT >= 2
    MCLIB_mValueLimitWithMaxMin(sControlData.sStack[STACK_2].s16Speed, sControlLimit.s16FieldFrequencyRequestMax, sControlLimit.s16FieldFrequencyRequestMin);
    #endif



    /* Fix Skipped Pulses */
    while (sControlData.u16SkippedPulses > 0) {

        sControlData.u16SkippedPulses--;
        vStateRunSkipPulse();
    }
    
    //vCurrentVoltageFbkPrepare();

    //DEBUG_SET_CTRL(); 
    
    if (bProcessControlStack1)
    {
        vProcessStack(&sControlData.sStack[STACK_1]);
    }
    
    #if BOARDCFG_STACK_COUNT >= 2
    if (bProcessControlStack2)
    {
        vProcessStack(&sControlData.sStack[STACK_2]);
    }
    #endif
    
    //DEBUG_CLR_CTRL();      


// #if BOARDCFG_USE_TSC_DSPIC
#if 1
    /* PWM Set */

    if (bProcessAllPWMWithStack1Process)
    {
        ADCPWM_vSetDutyCycleStack1(&sControlData.sStack[STACK_1].sDutyCycle);
        ADCPWM_vSetDutyCycleStack2(&sControlData.sStack[STACK_1].sDutyCycle);
        ADCPWM_vSetDutyCycleStack3(&sControlData.sStack[STACK_1].sDutyCycle);
    }
    else
    {
        ADCPWM_vSetDutyCycleStack1(&sControlData.sStack[STACK_1].sDutyCycle);
        #if BOARDCFG_STACK_COUNT >= 2
        ADCPWM_vSetDutyCycleStack2(&sControlData.sStack[STACK_2].sDutyCycle);
        #endif
    }
#endif


    /* Stop Request Check */
    if (sControlAccess.bStart == 0) {
        /* stop actuators */
        eControlState = CTRL_STA_INIT;
#if BOARDCFG_USE_FPGA
        FPGA_COMMON_vDisablePWM();
#endif
        PWM_vOutputDisable();
        bPWMEnableRequest = 0;
    }
    /* Fault Detected Check */
    if (sControlAccess.bFault == 1) {
        /* stop actuators */
        eControlState = CTRL_STA_FAULT;
#if BOARDCFG_USE_FPGA
        FPGA_COMMON_vDisablePWM();
#endif
        PWM_vOutputDisable();
        bPWMEnableRequest = 0;
    }
}

eControlState_t CONTROL_eGetState(void) {
    return eControlState;
}

void CONTROL_vSetStackMask(uint16_t u16StackMask)
{
    CONTROL_uStackMask.u16Register = u16StackMask;
}


