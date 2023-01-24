/* *****************************************************************************
 * File:   main.c
 * Author: Damyan Neev
 *
 * Created on 2023 01 24 09:06
 * 
 * Description: Example for CSMON_C28xx library integration.
 * 
 **************************************************************************** */
#ifdef _WIN32
#pragma once
#endif /* _WIN32 */
 
/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "device.h"
#include "csmon.h"  // in "csmon_config.h"
#include "csmon_config.h"
#include "boardcfg.h"
#include "datetime.h"


/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define TEST_CSMON_APPLICATION_VERSION_HI    1
#define TEST_CSMON_APPLICATION_VERSION_LO    0

#define TEST_CSMON_APPLICATION_VERSION       (uint16_t)(((uint16_t)((uint_least8_t)(TEST_CSMON_APPLICATION_VERSION_HI & 0x00FF)) << 8) | ((uint_least8_t)(TEST_CSMON_APPLICATION_VERSION_LO & 0x00FF)))

/* Parameter Table Version, DateTime, Checksum */
#define PARAM_TABLE_VERSION                 (uint32_t)1
#define PARAM_TABLE_DATETIME                (uint32_t)DATETIME_BUILD

/* Dummy Parameter List */
#define PARAM_ID_MODBUS_MSG_CNT    65534
#if _CSMON_USE_EXTERNAL_PARAMETER_TABLE
    #define PARAM_ID_STARUNNINGMODE     131
    #define PARAM_ID_VOLTAGE_DCLINK     132
    #define PARAM_ID_CURRENT_PHASEA     133
    #define PARAM_ID_CURRENT_PHASEB     134
    #define PARAM_ID_CURRENT_PHASEC     135
    #define PARAM_ID_VOLTAGE_DCLINK_32  136
    #define PARAM_ID_CURRENT_PHASEA_32  137
    #define PARAM_ID_CURRENT_PHASEB_32  138
    #define PARAM_ID_CURRENT_PHASEC_32  139
#else
    #define PARAM_ID_STARUNNINGMODE     0
    #define PARAM_ID_VOLTAGE_DCLINK     8
    #define PARAM_ID_CURRENT_PHASEA     9
    #define PARAM_ID_CURRENT_PHASEB     10
    #define PARAM_ID_CURRENT_PHASEC     11

    #define PARAM_ID_VOLTAGE_DCLINK_32  28
    #define PARAM_ID_CURRENT_PHASEA_32  29
    #define PARAM_ID_CURRENT_PHASEB_32  30
    #define PARAM_ID_CURRENT_PHASEC_32  31
#endif

#define CSMON_AUTOMATIC_SERVICE_WATCHDOG_IN_MAIN_LOOP   1

#define RECORDER0_ONLY_TEST                             1

#if RECORDER0_ONLY_TEST
#define RECORDER0_PRETRIGGER_SAMPLE_COUNT   9000
#define RECORDER0_TOTAL_SAMPLE_COUNT        10000
//#define RECORDER_COUNT                      3
#define RECORDER_COUNT                      1
#else
#define RECORDER0_PRETRIGGER_SAMPLE_COUNT   5900
#define RECORDER0_TOTAL_SAMPLE_COUNT        6000
#define RECORDER1_PRETRIGGER_SAMPLE_COUNT   5900
#define RECORDER1_TOTAL_SAMPLE_COUNT        6000
#define RECORDER2_PRETRIGGER_SAMPLE_COUNT   5900
#define RECORDER2_TOTAL_SAMPLE_COUNT        6000
#define RECORDER_COUNT                      CSMON_RECORDER_COUNT_MAX
#endif

#define RECORDER_SAMPLE_TIME_FIX_1MS        0       /* If 1kHz sample frequency in CSMON PC Application 1ms equals 1sample */

#if RECORDER_SAMPLE_TIME_FIX_1MS
#define RECORDER0_SAMPLE_FREQUENCY_HZ       1000.0
#define RECORDER1_SAMPLE_FREQUENCY_HZ       1000.0
#define RECORDER2_SAMPLE_FREQUENCY_HZ       1000.0
#else
#define RECORDER0_SAMPLE_FREQUENCY_HZ      20000.0
#define RECORDER1_SAMPLE_FREQUENCY_HZ      20000.0
#define RECORDER2_SAMPLE_FREQUENCY_HZ      20000.0
#endif


#define SCOPE_COUNT                         1

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */



/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */



/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
typedef struct
{
    uint_least8_t u8Seconds;
    uint_least8_t u8Minutes;
    uint_least8_t u8Hours;
    uint_least8_t u8Weekdays;
    uint_least8_t u8Day;
    uint_least8_t u8Month;
    uint_least8_t u8Year;
    uint_least8_t u8Reserved;
}MAIN_sDateTime_t;


/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */


/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */

uint16_t u16PeriodControl_usec;
uint16_t u16DelayCtrlLoop_100nsec;
uint16_t u16DelayMainLoop_usec;
uint16_t u16CountSetParameterFail;
uint32_t u32TimeMainLoopProcessCSMON_Max_Ticks;
uint32_t u32TimeCSMON_ISR_Max_Ticks;
uint32_t u32TimeMainLoopCycle_Max_Ticks;
uint32_t u32TimeCtrlLoopMax_Ticks;
uint32_t u32ParamTime_Ticks;
bool bResetAllTimeMeasures;
uint16_t u16DummyDataCnt;
bool bDummyStatsDevRunning;
bool bDummyReqstDevRunning;
int16_t s16ScaleDataTst;
int16_t s16DummyIncrementLoop;
int16_t s16DummyCurrentPhaseA;
int16_t s16DummyCurrentPhaseB;
int16_t s16DummyCurrentPhaseC;
uint32_t u32GetBaudError_PPM;
uint16_t u16WatchdogPrescalerTime;
int16_t s16DummyVoltageDCLink;
int32_t s32DummyVoltageDCLink;
int32_t s32DummyCurrentPhaseA;
int32_t s32DummyCurrentPhaseB;
int32_t s32DummyCurrentPhaseC;
int16_t s16DummyVoltageDCLinkStartup;
int16_t s16DummyCurrentPhaseAStartup;
int16_t s16DummyCurrentPhaseBStartup;
int16_t s16DummyCurrentPhaseCStartup;
int16_t s16DummyVoltageDCLinkStop;
int16_t s16DummyCurrentPhaseAStop;
int16_t s16DummyCurrentPhaseBStop;
int16_t s16DummyCurrentPhaseCStop;
int16_t s16DummyVoltageDCLinkIdle;
int16_t s16DummyCurrentPhaseAIdle;
int16_t s16DummyCurrentPhaseBIdle;
int16_t s16DummyCurrentPhaseCIdle;
int16_t s16DummyIncrementLoopA;
int16_t s16DummyIncrementLoopB;
int16_t s16DummyIncrementLoopC;
int16_t s16DummyIncrementLoopV;
int16_t s16DummyIncrementLoopCDiff;

CSMON_eResponseCode_t eResponseCode_CSMON_eInit = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eProcess = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetServerOnStatus = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetParameter = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetRecorder = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetScope = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetTimerPeriodISRFunctionRegister = CSMON_RESPONSE_CODE_OK;
//CSMON_eResponseCode_t eResponseCode_CSMON_eSetFlagProcessPassed = CSMON_RESPONSE_CODE_OK;
uint32_t u32GetBaudError_PPM = 0;

uint32_t u32TimeMainLoopProcessCSMON_Bgn_Ticks = 0;
uint32_t u32TimeMainLoopProcessCSMON_End_Ticks = 0;
uint32_t u32TimeMainLoopProcessCSMON_Now_Ticks = 0;
//uint32_t u32TimeMainLoopProcessCSMON_Max_Ticks = 0;

uint32_t u32DelayCtrlLoop_Ticks = 1;
uint32_t u32DelayMainLoop_Ticks = 1;

uint16_t u16DelayCtrlLoopOld_100nsec = 0;
uint16_t u16DelayMainLoopOld_usec = 0;

bool MAIN_bDateTimeSet = false;

MAIN_sDateTime_t MAIN_sDateTimeGet =
{
    0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00 /* 2001-01-01-Mon-00:00:00 */
};
MAIN_sDateTime_t MAIN_sDateTimeSet =
{
    BUILD_RTC_SEC,
    BUILD_RTC_MIN,
    BUILD_RTC_HOUR,
    0x00,
    BUILD_RTC_DAY,
    BUILD_RTC_MONTH,
    BUILD_RTC_YEAR,
    0x00,
    /* 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00 2001-01-01-Mon-00:00:00 */
};

uint16_t u16FreeRunningTimerTicksPerMicroSecond;
uint16_t u16FreeRunningTimerPrescaller;

uint16_t* pu16ModbusMessageCounter = &u16DummyDataCnt;
uint16_t u16CountSetParameterFree = 0;

uint32_t u32TimeMainLoopCycle_Bgn_Ticks = 0;
uint32_t u32TimeMainLoopCycle_End_Ticks = 0;
uint32_t u32TimeMainLoopCycle_Now_Ticks = 0;
uint32_t u32TimeMainLoopCycle_Max_Ticks = 0;

uint16_t u16WatchdogPrescaler = 0;
uint16_t u16WatchdogPrescalerOld = 0;


/* *****************************************************************************
 * Prototypes of functions definitions
 **************************************************************************** */



/* *****************************************************************************
 * Functions
 **************************************************************************** */

#if _CSMON_USE_EXTERNAL_PARAMETER_TABLE == 0
/* *****************************************************************************
 * ExternalParametersInitialization
 **************************************************************************** */
void ExternalParametersInitialization(void)
{
#if CSMON_REALADR_16BIT
    CSMON_eSetParameterListRealAddress((uint16_t *)&asParameterList[0].u16RealAddress, sizeof(asParameterList[0]));                     /* First Put Real Address to calculate count parameters internally (last index is NULL) */
#else
    CSMON_eSetParameterListRealAddress((uint32_t *)&asParameterList[0].u32RealAddress, sizeof(asParameterList[0]));                     /* First Put Real Address to calculate count parameters internally (last index is NULL) */
#endif
    //CSMON_eSetParameterListProcessFunc((uint32_t *)&asParameterList[0].u32ProcessFunc, sizeof(asParameterList[0]));
    CSMON_eSetParameterListParameterID((uint16_t *)&asParameterList[0].u16ParameterIndexID, sizeof(asParameterList[0]));
    CSMON_eSetParameterListRegisterSize((uint16_t *)&asParameterList[0].uParameterSize.u16Register, sizeof(asParameterList[0]));
    CSMON_eSetParameterListRegisterFlags((uint16_t *)&asParameterList[0].uParameterFlags.u16Register, sizeof(asParameterList[0]));
#if _CSMON_USE_EXTERNAL_PARAMETER_LIST
    CSMON_eSetParameterListFunctionCode((uint16_t *)&asParameterList[0].uFunctionCode, sizeof(asParameterList[0]));
    CSMON_eSetParameterListAccessLevel((uint16_t *)&asParameterList[0].uAccessLevel, sizeof(asParameterList[0]));
    CSMON_eSetParameterListBitsCount((uint16_t *)&asParameterList[0].uBitsCount, sizeof(asParameterList[0]));
#endif
    //CSMON_eSetParameterListParamAttrib((uint16_t *)&asParameterList[0].u16ParamAttributes, sizeof(asParameterList[0]));
#if _CSMON_USE_EXTERNAL_PARAMETER_LIST
    CSMON_eSetParameterListShortNaming((uint_least8_t *)&asParameterList[0].au8NameUnit, sizeof(asParameterList[0]));
    CSMON_eSetParameterListDataType((uint16_t *)&asParameterList[0].eDataTypeAttribute, sizeof(asParameterList[0]));
    //CSMON_eSetParameterListVisualType((uint16_t *)&asParameterList[0].eVisualTypeAttribute, sizeof(asParameterList[0]));
#endif
    CSMON_eSetParameterListDataMaximum((uint32_t *)&asParameterList[0].u32Max.u32Register, sizeof(asParameterList[0]));
    CSMON_eSetParameterListDataMinimum((uint32_t *)&asParameterList[0].u32Min.u32Register, sizeof(asParameterList[0]));
    CSMON_eSetParameterListDataDefault((uint32_t *)&asParameterList[0].u32Def.u32Register, sizeof(asParameterList[0]));
    CSMON_eSetParameterListValueFormat((float *)&asParameterList[0].Norm, sizeof(asParameterList[0]));                               /* 0.0 - Default HEX Visualization; Any other -> Default Decimal Visualization */
    //CSMON_eSetParameterListDataOffset((uint32_t *)&asParameterList[0].u32Offset.u32Register, sizeof(asParameterList[0]));
}

void ParameterInitialization(void)
{
    volatile uint16_t u16Index;

    uint32_t u32ParamRealAddress;

    volatile uint32_t u32ParamVer;
    volatile uint32_t u32DateTime;
    volatile uint32_t u32CheckSum;

    uWord32_t uParamVerBackup;
    uWord32_t uDateTimeBackup;
    uWord32_t uCheckSumBackup;


    uParamVerBackup.u32Register = 0xFFFFFFFF;
    uDateTimeBackup.u32Register = 0xFFFFFFFF;
    uCheckSumBackup.u32Register = 0xAA561234;

    u32ParamVer = PARAM_TABLE_VERSION;
    u32DateTime = PARAM_TABLE_DATETIME;
    u32CheckSum = CSMON_u32GetParameterCheckSum();                        /* Get Checksum From CSMON */

    u16CountSetParameterFail = 0;
    u16CountSetParameterFree = 0;

    if ( (uParamVerBackup.u32Register != u32ParamVer)
      || (uDateTimeBackup.u32Register != u32DateTime)
      || (uCheckSumBackup.u32Register != u32CheckSum) )                /* ParamVer or DateTime or Checksum MisMatch */
    {
        /* Invalid Table - Reset Parameter Table */
        CSMON_eResetParameterTable();                                   /* Reset Internal Used Parameters Count */

        /* Add Parameters */
        for (u16Index = 0; u16Index < BOARDCFG_CSMON_FILE_PARAMETER_COUNT_MAX; u16Index++)
        {

#if CSMON_REALADR_16BIT
            u32ParamRealAddress = asParameterList[u16Index].u16RealAddress;
#else
            u32ParamRealAddress = asParameterList[u16Index].u32RealAddress;
#endif

            if (asParameterList[u16Index].u16ParameterIndexID == PARAM_ID_MODBUS_MSG_CNT)
            {
                CSMON_vSetModbusMessageCounterRegisterRealAddress((uint32_t)&u16DummyDataCnt);
                pu16ModbusMessageCounter = (uint16_t*)CSMON_u32GetModbusMessageCounterRegisterRealAddress();
                u32ParamRealAddress = (uint32_t)pu16ModbusMessageCounter;
            }


            eResponseCode_CSMON_eSetParameter =
                CSMON_eSetParameter (
                    asParameterList[u16Index].u16ParameterIndexID,
                    u32ParamRealAddress,
#if _CSMON_USE_EXTERNAL_PARAMETER_LIST == 0
                    asParameterList[u16Index].u16ParamAttributes,
   (uint_least8_t*)&asParameterList[u16Index].au8Name,
   (uint_least8_t*)&asParameterList[u16Index].au8Unit,
#else
                   NULL,
                   NULL,
                   NULL,
#endif
                    asParameterList[u16Index].u32Max.u32Register,
                    asParameterList[u16Index].u32Min.u32Register,
                    asParameterList[u16Index].u32Def.u32Register,
                    asParameterList[u16Index].Norm,
#if _CSMON_USE_EXTERNAL_PARAMETER_LIST == 0
                    asParameterList[u16Index].u8BitCountOrArrayElementSize,
                    asParameterList[u16Index].u8StartBitOrArrayElementCount
#else
                    0,
                    0
#endif
                    );
            if (u32ParamRealAddress == NULL)
            {
                u16CountSetParameterFree++;
            }
            else
            if(eResponseCode_CSMON_eSetParameter != CSMON_RESPONSE_CODE_OK)
            {
                u16CountSetParameterFail++;
            }
        }


        CSMON_eApplyParameterChanges();                         /* Internal Library Apply Written Parameters */
    }
}
#endif


/* *****************************************************************************
 * RecordersInitialization
 **************************************************************************** */
void RecordersInitialization(void)
{
    uint16_t u16ValidParameters = 0;

    /* Recorder 0 */
    u16ValidParameters = CSMON_POSITION_IN_RECORDER_0;

    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_0, PARAM_ID_STARUNNINGMODE, u16ValidParameters);
    if (eResponseCode_CSMON_eSetRecorder == CSMON_RESPONSE_CODE_OK)
    {
        u16ValidParameters++;
    }
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_0, PARAM_ID_CURRENT_PHASEA, u16ValidParameters);
    if (eResponseCode_CSMON_eSetRecorder == CSMON_RESPONSE_CODE_OK)
    {
        u16ValidParameters++;
    }
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_0, PARAM_ID_CURRENT_PHASEB, u16ValidParameters);
    if (eResponseCode_CSMON_eSetRecorder == CSMON_RESPONSE_CODE_OK)
    {
        u16ValidParameters++;
    }
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_0, PARAM_ID_CURRENT_PHASEC, u16ValidParameters);
    if (eResponseCode_CSMON_eSetRecorder == CSMON_RESPONSE_CODE_OK)
    {
        u16ValidParameters++;
    }
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_0, PARAM_ID_CURRENT_PHASEA_32, u16ValidParameters);
    if (eResponseCode_CSMON_eSetRecorder == CSMON_RESPONSE_CODE_OK)
    {
        u16ValidParameters++;
    }
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_0, PARAM_ID_CURRENT_PHASEB_32, u16ValidParameters);
    if (eResponseCode_CSMON_eSetRecorder == CSMON_RESPONSE_CODE_OK)
    {
        u16ValidParameters++;
    }
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_0, PARAM_ID_CURRENT_PHASEC_32, u16ValidParameters);
    if (eResponseCode_CSMON_eSetRecorder == CSMON_RESPONSE_CODE_OK)
    {
        u16ValidParameters++;
    }
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_0, PARAM_ID_VOLTAGE_DCLINK_32, u16ValidParameters);
    if (eResponseCode_CSMON_eSetRecorder == CSMON_RESPONSE_CODE_OK)
    {
        u16ValidParameters++;
    }
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterCountInRecorder (
            CSMON_RECORDER_0, u16ValidParameters);


    #if RECORDER0_ONLY_TEST == 0

    /* Recorder 1 */
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_1, PARAM_ID_CURRENT_PHASEA_32, CSMON_POSITION_IN_RECORDER_0);
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_1, PARAM_ID_CURRENT_PHASEB_32, CSMON_POSITION_IN_RECORDER_1);
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_1, PARAM_ID_CURRENT_PHASEC_32, CSMON_POSITION_IN_RECORDER_2);
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_1, PARAM_ID_VOLTAGE_DCLINK, CSMON_POSITION_IN_RECORDER_3);
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterCountInRecorder (
            CSMON_RECORDER_1, CSMON_COUNT_PARAMETERS_4);

    /* Recorder 2 */
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_2, PARAM_ID_CURRENT_PHASEA, CSMON_POSITION_IN_RECORDER_0);
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_2, PARAM_ID_CURRENT_PHASEB, CSMON_POSITION_IN_RECORDER_1);
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_2, PARAM_ID_CURRENT_PHASEC, CSMON_POSITION_IN_RECORDER_2);
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_2, PARAM_ID_VOLTAGE_DCLINK, CSMON_POSITION_IN_RECORDER_3);
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterCountInRecorder (
            CSMON_RECORDER_2, CSMON_COUNT_PARAMETERS_4);

    #endif


    /* Recorder 0 Configuration */
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetRecorderConfigurationSkipSamples (
            CSMON_RECORDER_0,
            RECORDER0_PRETRIGGER_SAMPLE_COUNT,   /* PreTriggerSampleCount */
            RECORDER0_TOTAL_SAMPLE_COUNT,   /* TotalSampleCount */
            RECORDER0_SAMPLE_FREQUENCY_HZ / 3.0); /* Sample Frequency in Hz */


    #if RECORDER0_ONLY_TEST == 0

    /* Recorder 1 Configuration */
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetRecorderConfiguration (
            CSMON_RECORDER_1,
            RECORDER1_PRETRIGGER_SAMPLE_COUNT,   /* PreTriggerSampleCount */
            RECORDER1_TOTAL_SAMPLE_COUNT,   /* TotalSampleCount */
            RECORDER1_SAMPLE_FREQUENCY_HZ); /* Sample Frequency in Hz */

    /* Recorder 2 Configuration */
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetRecorderConfiguration (
            CSMON_RECORDER_2,
            RECORDER2_PRETRIGGER_SAMPLE_COUNT,   /* PreTriggerSampleCount */
            RECORDER2_TOTAL_SAMPLE_COUNT,   /* TotalSampleCount */
            RECORDER2_SAMPLE_FREQUENCY_HZ); /* Sample Frequency in Hz */
    #endif

    /* Note !!! CSMON_eSetRecorderTriggerAtPosition call after CSMON_eSetRecorderConfiguration */


    /* Trigger Recorder 0 */
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetRecorderTriggerAtPosition (
            CSMON_RECORDER_0,
            PARAM_ID_STARUNNINGMODE,
            (uint32_t)true,
            (uint16_t)CSMON_TRIGGER_MODE_FALLING_EDGE);

    #if RECORDER0_ONLY_TEST == 0

    /* Trigger Recorder 1 */
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetRecorderTriggerAtPosition (
            CSMON_RECORDER_1,
            PARAM_ID_STARUNNINGMODE,
            (uint32_t)true,
            (uint16_t)CSMON_TRIGGER_MODE_FALLING_EDGE);

    /* Trigger Recorder 2 */
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetRecorderTriggerAtPosition (
            CSMON_RECORDER_2,
            (uint16_t)0,
            (uint32_t)0,
            (uint16_t)CSMON_TRIGGER_MODE_NONE);
    #endif


    #if EXTERNAL_RECORDERS == 1
    //
    // CSMON External Recorder Usage Setup
    //
    CSMON_eSetExternalRecorderUsage
    (
                                    CSMON_RECORDER_0,
    (uint16_t*)&sExternalRecoderHandle[CSMON_RECORDER_0].sStatus,
    (uint16_t*)&sExternalRecoderHandle[CSMON_RECORDER_0].u32StartAddressFirstDataSample,
             sExternalRecoderHandle[CSMON_RECORDER_0].u32CircleBufferSampleCount,
             sExternalRecoderHandle[CSMON_RECORDER_0].u32CircleBufferStartAddress,
             sExternalRecoderHandle[CSMON_RECORDER_0].eTriggerSubSecondMode,
            &sExternalRecoderHandle[CSMON_RECORDER_0].u32TriggerMicrosecondsOrRollingTimerTicks,
            &sExternalRecoderHandle[CSMON_RECORDER_0].u8TriggerBCDSeconds,
            &sExternalRecoderHandle[CSMON_RECORDER_0].u8TriggerBCDMinutes,
            &sExternalRecoderHandle[CSMON_RECORDER_0].u8TriggerBCDHours,
            &sExternalRecoderHandle[CSMON_RECORDER_0].u8TriggerBCDWeekdays,
            &sExternalRecoderHandle[CSMON_RECORDER_0].u8TriggerBCDDay,
            &sExternalRecoderHandle[CSMON_RECORDER_0].u8TriggerBCDMonth,
            &sExternalRecoderHandle[CSMON_RECORDER_0].u8TriggerBCDYear
    );
    CSMON_vAddSetupRecorderParameterMask(CSMON_MASK_RECORDER_0);

    #if RECORDER0_ONLY_TEST == 0
    CSMON_eSetExternalRecorderUsage
    (
                                    CSMON_RECORDER_1,
    (uint16_t*)&sExternalRecoderHandle[CSMON_RECORDER_1].sStatus,
    (uint16_t*)&sExternalRecoderHandle[CSMON_RECORDER_1].u32StartAddressFirstDataSample,
             sExternalRecoderHandle[CSMON_RECORDER_1].u32CircleBufferSampleCount,
             sExternalRecoderHandle[CSMON_RECORDER_1].u32CircleBufferStartAddress,
             sExternalRecoderHandle[CSMON_RECORDER_1].eTriggerSubSecondMode,
            &sExternalRecoderHandle[CSMON_RECORDER_1].u32TriggerMicrosecondsOrRollingTimerTicks,
            &sExternalRecoderHandle[CSMON_RECORDER_1].u8TriggerBCDSeconds,
            &sExternalRecoderHandle[CSMON_RECORDER_1].u8TriggerBCDMinutes,
            &sExternalRecoderHandle[CSMON_RECORDER_1].u8TriggerBCDHours,
            &sExternalRecoderHandle[CSMON_RECORDER_1].u8TriggerBCDWeekdays,
            &sExternalRecoderHandle[CSMON_RECORDER_1].u8TriggerBCDDay,
            &sExternalRecoderHandle[CSMON_RECORDER_1].u8TriggerBCDMonth,
            &sExternalRecoderHandle[CSMON_RECORDER_1].u8TriggerBCDYear
    );
    CSMON_vAddSetupRecorderParameterMask(CSMON_MASK_RECORDER_1);

    CSMON_eSetExternalRecorderUsage
    (
                                    CSMON_RECORDER_2,
    (uint16_t*)&sExternalRecoderHandle[CSMON_RECORDER_2].sStatus,
    (uint16_t*)&sExternalRecoderHandle[CSMON_RECORDER_2].u32StartAddressFirstDataSample,
             sExternalRecoderHandle[CSMON_RECORDER_2].u32CircleBufferSampleCount,
             sExternalRecoderHandle[CSMON_RECORDER_2].u32CircleBufferStartAddress,
             sExternalRecoderHandle[CSMON_RECORDER_2].eTriggerSubSecondMode,
            &sExternalRecoderHandle[CSMON_RECORDER_2].u32TriggerMicrosecondsOrRollingTimerTicks,
            &sExternalRecoderHandle[CSMON_RECORDER_2].u8TriggerBCDSeconds,
            &sExternalRecoderHandle[CSMON_RECORDER_2].u8TriggerBCDMinutes,
            &sExternalRecoderHandle[CSMON_RECORDER_2].u8TriggerBCDHours,
            &sExternalRecoderHandle[CSMON_RECORDER_2].u8TriggerBCDWeekdays,
            &sExternalRecoderHandle[CSMON_RECORDER_2].u8TriggerBCDDay,
            &sExternalRecoderHandle[CSMON_RECORDER_2].u8TriggerBCDMonth,
            &sExternalRecoderHandle[CSMON_RECORDER_2].u8TriggerBCDYear
    );
    CSMON_vAddSetupRecorderParameterMask(CSMON_MASK_RECORDER_2);
    #endif

#endif
}



/* *****************************************************************************
 * ScopesInitialization
 **************************************************************************** */
void ScopesInitialization(void)
{
#if SCOPE_COUNT >= 1
    /* Scope 0 */
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterInScopeAtPosition (
            CSMON_SCOPE_0, PARAM_ID_CURRENT_PHASEA, CSMON_POSITION_IN_SCOPE_0);
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterInScopeAtPosition (
            CSMON_SCOPE_0, PARAM_ID_CURRENT_PHASEB, CSMON_POSITION_IN_SCOPE_1);
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterInScopeAtPosition (
            CSMON_SCOPE_0, PARAM_ID_CURRENT_PHASEC, CSMON_POSITION_IN_SCOPE_2);
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterInScopeAtPosition (
            CSMON_SCOPE_0, PARAM_ID_VOLTAGE_DCLINK, CSMON_POSITION_IN_SCOPE_3);
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterCountInScope (
            CSMON_SCOPE_0, CSMON_COUNT_PARAMETERS_4);
#endif

#if SCOPE_COUNT >= 2
    /* Scope 1 */
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterInScopeAtPosition (
            CSMON_SCOPE_1, PARAM_ID_CURRENT_PHASEA, CSMON_POSITION_IN_SCOPE_0);
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterInScopeAtPosition (
            CSMON_SCOPE_1, PARAM_ID_CURRENT_PHASEB, CSMON_POSITION_IN_SCOPE_1);
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterInScopeAtPosition (
            CSMON_SCOPE_1, PARAM_ID_CURRENT_PHASEC, CSMON_POSITION_IN_SCOPE_2);
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterInScopeAtPosition (
            CSMON_SCOPE_1, PARAM_ID_VOLTAGE_DCLINK, CSMON_POSITION_IN_SCOPE_3);
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterCountInScope (
            CSMON_SCOPE_1, CSMON_COUNT_PARAMETERS_4);
#endif

#if SCOPE_COUNT >= 3
    /* Scope 2 */
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterInScopeAtPosition (
            CSMON_SCOPE_2, PARAM_ID_CURRENT_PHASEA, CSMON_POSITION_IN_SCOPE_0);
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterInScopeAtPosition (
            CSMON_SCOPE_2, PARAM_ID_CURRENT_PHASEB, CSMON_POSITION_IN_SCOPE_1);
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterInScopeAtPosition (
            CSMON_SCOPE_2, PARAM_ID_CURRENT_PHASEC, CSMON_POSITION_IN_SCOPE_2);
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterInScopeAtPosition (
            CSMON_SCOPE_2, PARAM_ID_VOLTAGE_DCLINK, CSMON_POSITION_IN_SCOPE_3);
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterCountInScope (
            CSMON_SCOPE_2, CSMON_COUNT_PARAMETERS_4);
#endif
}



CSMON_eReturnCodeParameter_t eWriteParElement(uint16_t u16Index, CSMON_eParameterElement_t eElement, uint16_t* pu16Len, void* pData, uint16_t u16NoStore, uint16_t u16DataMode, uint16_t u16Password)
{
    CSMON_eReturnCodeParameter_t eResult = CSMON_RC_PARA_OK;

    //uint32_t u32Data = *((uint32_t*)pData);
    //uint16_t u16Data = *((uint16_t*)pData);
    //uint16_t u16Len = *((uint16_t*)pu16Len);

    return eResult;
}


void vSetWatchdogPrescalerTimeDiv(uint16_t u16Prescaler)
{
    if ((u16Prescaler >= SYSCTL_WD_PRESCALE_1) && (u16Prescaler <= SYSCTL_WD_PRESCALE_64))
    {
        u16WatchdogPrescalerTime = (1 << (u16Prescaler-1));
    }
    else
    {
        u16WatchdogPrescalerTime = 1;
    }
}


void CSMON_vSetDateTime (
        uint_least8_t u8BCDSeconds,
        uint_least8_t u8BCDMinutes,
        uint_least8_t u8BCDHours,
        uint_least8_t u8BCDWeekdays,
        uint_least8_t u8BCDDay,
        uint_least8_t u8BCDMonth,
        uint_least8_t u8BCDYear)
{
    MAIN_sDateTimeSet.u8Seconds    = u8BCDSeconds;
    MAIN_sDateTimeSet.u8Minutes    = u8BCDMinutes;
    MAIN_sDateTimeSet.u8Hours      = u8BCDHours;
    MAIN_sDateTimeSet.u8Weekdays   = u8BCDWeekdays;
    MAIN_sDateTimeSet.u8Day        = u8BCDDay;
    MAIN_sDateTimeSet.u8Month      = u8BCDMonth;
    MAIN_sDateTimeSet.u8Year       = u8BCDYear;
    MAIN_bDateTimeSet = true;
}

/* *****************************************************************************
 * CSMON_vGetDateTime - Get RTC Value - called from CSMON library
 **************************************************************************** */
void CSMON_vGetDateTime (
        uint_least8_t* pu8BCDSeconds,
        uint_least8_t* pu8BCDMinutes,
        uint_least8_t* pu8BCDHours,
        uint_least8_t* pu8BCDWeekdays,
        uint_least8_t* pu8BCDDay,
        uint_least8_t* pu8BCDMonth,
        uint_least8_t* pu8BCDYear)
{
        *pu8BCDSeconds  = MAIN_sDateTimeGet.u8Seconds;
        *pu8BCDMinutes  = MAIN_sDateTimeGet.u8Minutes;
        *pu8BCDHours    = MAIN_sDateTimeGet.u8Hours;
        *pu8BCDWeekdays = MAIN_sDateTimeGet.u8Weekdays;
        *pu8BCDDay      = MAIN_sDateTimeGet.u8Day;
        *pu8BCDMonth    = MAIN_sDateTimeGet.u8Month;
        *pu8BCDYear     = MAIN_sDateTimeGet.u8Year;
}


/* *****************************************************************************
 * main
 **************************************************************************** */
void main(void)
{
    // Configure PLL, disable WD, enable peripheral clocks.
    Device_init();

    // Disable pin locks and enable internal PullUps.
    Device_initGPIO();

    // Clear all interrupts and initialize PIE vector table: Disable CPU interrupts
    // Initialize interrupt controller and vector table. for __TMS320F2806x__ Interrupts fixed in Device Init?

#define CPUTIMER1_PRESCALLER_VALUE      ((DEVICE_SYSCLK_FREQ /   1000000)-1)    /* Timer Tick 1.0 microseconds */
    //#define CPUTIMER1_PRESCALLER_VALUE      ((DEVICE_SYSCLK_FREQ / 200000000)-1)    /* Timer Tick 5.0 nanoseconds */
    //
    CPUTimer_setPeriod( CPUTIMER1_BASE, 0xFFFFFFFF );
    CPUTimer_setPreScaler( CPUTIMER1_BASE, CPUTIMER1_PRESCALLER_VALUE );
    CPUTimer_stopTimer( CPUTIMER1_BASE );
    CPUTimer_reloadTimerCounter( CPUTIMER1_BASE );
    CPUTimer_enableInterrupt( CPUTIMER1_BASE );
    CPUTimer_startTimer( CPUTIMER1_BASE );

    //  Calculate Free Running Timer Ticks Per microsecond
    u16FreeRunningTimerPrescaller = (HWREGH(CPUTIMER1_BASE + CPUTIMER_O_TPRH) << 8U) + (HWREGH(CPUTIMER1_BASE + CPUTIMER_O_TPR) & CPUTIMER_TPR_TDDR_M);
    u16FreeRunningTimerTicksPerMicroSecond = ((DEVICE_SYSCLK_FREQ / 1000000) / (u16FreeRunningTimerPrescaller+1));
    if (u16FreeRunningTimerTicksPerMicroSecond == 0)
    {
        u16FreeRunningTimerTicksPerMicroSecond = 1;
    }

    // CSMON Initialization -> ~ 2.25ms
    eResponseCode_CSMON_eInit = CSMON_eInit();
    // Check CSMON Response Code if needed
    if (eResponseCode_CSMON_eInit != CSMON_RESPONSE_CODE_OK)
    {
        /* If enters here - Fix Peripheral Frequency for Better Performance and Stability (DEVICE_LSPCLK_FREQ) */
        u32GetBaudError_PPM = CSMON_u32GetBaudError_PPM(CSMON_ID_PERIPHERAL_SCI_MODBUS);
        ASSERT(u32GetBaudError_PPM >= CSMON_u32PercentToPPM(3.0));
    }
    else
    {
        u32GetBaudError_PPM = CSMON_u32GetBaudError_PPM(CSMON_ID_PERIPHERAL_SCI_MODBUS);
    }

    // Firmware Application Version Set to CSMON
    (void)CSMON_eSetFirmwareApplicationVersion(TEST_CSMON_APPLICATION_VERSION);

    // CSMON Parameter, Recorder, Scope Test Initialization
    u32ParamTime_Ticks = CPUTimer_getTimerCount(CPUTIMER1_BASE);

    ParameterInitialization();
    ExternalParametersInitialization();
    /* Recorder And Scope Initialization Made Once after parameter initialized */
    RecordersInitialization();
    ScopesInitialization();

    u32ParamTime_Ticks = 0 - (CPUTimer_getTimerCount(CPUTIMER1_BASE) - u32ParamTime_Ticks);//down count

    // CSMON Internal Recorders Setup with Already Made Configuration
    CSMON_vAddSetupRecorderParameterMask(CSMON_MASK_RECORDER_0);

    // Register Function Call In CSMON Timer Period ISR (default Timer Period is 50 usec)
    // For Debug and Control Process Emulation here is registered the ControlProcess Function
//    eResponseCode_CSMON_eSetTimerPeriodISRFunctionRegister = CSMON_eSetTimerPeriodISRFunctionRegister( ControlProcess );
//    ASSERT(eResponseCode_CSMON_eSetTimerPeriodISRFunctionRegister != CSMON_RESPONSE_CODE_OK);

    // Information For CSMON for the CPU Load
    CSMON_eSetMaxTimeInISR(u16PeriodControl_usec - 10);  /* SetMaxTimeInISR in usec */
    CSMON_eSetMinGuaranteedTimeBetweenTwoISRs(10);       /* SetMinGuaranteedTimeBetweenTwoISRs in usec */


    // Reset the WatchDog counter
    SysCtl_serviceWatchdog();

    SysCtl_setWatchdogPrescaler(SYSCTL_WD_PRESCALE_1);        /*  1 * 512 * 256 @ 10Mhz -> ~ 13ms */
    u16WatchdogPrescalerOld = u16WatchdogPrescaler = SYSCTL_WD_PRESCALE_1;
    vSetWatchdogPrescalerTimeDiv( u16WatchdogPrescaler );

    // CSMON_eSetAutoServiceWatchdogInternalSlowCalculationsInMainLoop
    (void)CSMON_eSetAutoServiceWatchdogInternalSlowCalculationsInMainLoop(true);

    // Register Write Parameter Element Function
    (void)CSMON_eRegisterWriteParElementCallbackFunction( eWriteParElement );

    bResetAllTimeMeasures = 0;
    u32TimeMainLoopProcessCSMON_Now_Ticks = 0;
    u32TimeMainLoopProcessCSMON_Max_Ticks = 0;
    u32TimeMainLoopCycle_Now_Ticks = 0;
    u32TimeMainLoopCycle_Max_Ticks = 0;

    u32TimeCSMON_ISR_Max_Ticks = 0;
    u32TimeCtrlLoopMax_Ticks = 0;
    u32TimeMainLoopCycle_Bgn_Ticks = u32TimeMainLoopCycle_End_Ticks = CPUTimer_getTimerCount(CPUTIMER1_BASE);


    // Enable the WatchDog
    SysCtl_serviceWatchdog();
    SysCtl_enableWatchdog();

    // Enable Global Interrupt (INTM) and RealTime interrupt (DBGM)
    EINT;
    ERTM;


    for (;;)
    {
        // Reset the WatchDog counter
        SysCtl_serviceWatchdog();


        // Artificial Delay Main Loop
        SysCtl_delay( u32DelayMainLoop_Ticks );

        // Artificial Delay Setup
        if ( u16DelayCtrlLoop_100nsec != u16DelayCtrlLoopOld_100nsec )
        {
            u16DelayCtrlLoopOld_100nsec = u16DelayCtrlLoop_100nsec;
            u32DelayCtrlLoop_Ticks = (uint32_t)(((((long double)(u16DelayCtrlLoop_100nsec) * 0.1L) / (1000000.0L / (long double)DEVICE_SYSCLK_FREQ)) - 9.0L) / 5.0L);
            if (u32DelayCtrlLoop_Ticks == 0)
            {
                u32DelayCtrlLoop_Ticks = 1;
            }
        }
        if ( u16DelayMainLoop_usec != u16DelayMainLoopOld_usec )
        {
            u16DelayMainLoopOld_usec = u16DelayMainLoop_usec;
            u32DelayMainLoop_Ticks = (uint32_t)(((((long double)(u16DelayMainLoop_usec)) / (1000000.0L / (long double)DEVICE_SYSCLK_FREQ)) - 9.0L) / 5.0L);
            if (u32DelayMainLoop_Ticks == 0)
            {
                u32DelayMainLoop_Ticks = 1;
            }
        }


                // CSMON Process In Main Loop Delay Measure
                u32TimeMainLoopProcessCSMON_Bgn_Ticks = CPUTimer_getTimerCount(CPUTIMER1_BASE);

                // CSMON Process In Main Loop - Big Delays On Disconnect 4-5ms; On Connect 12-35ms If Not Interrupted (EMIF Checksum PC Application)
                eResponseCode_CSMON_eProcess = CSMON_eProcess();
                // Check CSMON Response Code if needed
                ASSERT(eResponseCode_CSMON_eProcess != CSMON_RESPONSE_CODE_OK);

                // CSMON Process In Main Loop Delay Measure
                u32TimeMainLoopProcessCSMON_End_Ticks = CPUTimer_getTimerCount(CPUTIMER1_BASE);
                u32TimeMainLoopProcessCSMON_Now_Ticks = 0 - (u32TimeMainLoopProcessCSMON_End_Ticks - u32TimeMainLoopProcessCSMON_Bgn_Ticks);//down count
                if (u32TimeMainLoopProcessCSMON_Now_Ticks > u32TimeMainLoopProcessCSMON_Max_Ticks)
                {
                    u32TimeMainLoopProcessCSMON_Max_Ticks = u32TimeMainLoopProcessCSMON_Now_Ticks;
                }

                // Device Running Control Indication - Set on Enter/Exit Run Mode
                eResponseCode_CSMON_eSetServerOnStatus = CSMON_eSetServerOnStatus(bDummyStatsDevRunning);
                // Check CSMON Response Code if needed
                ASSERT(eResponseCode_CSMON_eSetServerOnStatus != CSMON_RESPONSE_CODE_OK);

    }
}



