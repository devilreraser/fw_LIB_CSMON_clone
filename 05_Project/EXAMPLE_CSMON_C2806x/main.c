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

    CSMON_eSetParameterListParameterID((uint16_t *)&asParameterList[0].u16ParameterIndexID, sizeof(asParameterList[0]));
    CSMON_eSetParameterListRegisterSize((uint16_t *)&asParameterList[0].uParameterSize.u16Register, sizeof(asParameterList[0]));
    CSMON_eSetParameterListRegisterFlags((uint16_t *)&asParameterList[0].uParameterFlags.u16Register, sizeof(asParameterList[0]));

#if _CSMON_USE_EXTERNAL_PARAMETER_LIST
    CSMON_eSetParameterListFunctionCode((uint16_t *)&asParameterList[0].uFunctionCode, sizeof(asParameterList[0]));
    CSMON_eSetParameterListAccessLevel((uint16_t *)&asParameterList[0].uAccessLevel, sizeof(asParameterList[0]));
    CSMON_eSetParameterListBitsCount((uint16_t *)&asParameterList[0].uBitsCount, sizeof(asParameterList[0]));
    CSMON_eSetParameterListShortNaming((uint_least8_t *)&asParameterList[0].au8NameUnit, sizeof(asParameterList[0]));
    CSMON_eSetParameterListDataType((uint16_t *)&asParameterList[0].eDataTypeAttribute, sizeof(asParameterList[0]));
#endif

    CSMON_eSetParameterListDataMaximum((uint32_t *)&asParameterList[0].u32Max.u32Register, sizeof(asParameterList[0]));
    CSMON_eSetParameterListDataMinimum((uint32_t *)&asParameterList[0].u32Min.u32Register, sizeof(asParameterList[0]));
    CSMON_eSetParameterListDataDefault((uint32_t *)&asParameterList[0].u32Def.u32Register, sizeof(asParameterList[0]));
    CSMON_eSetParameterListValueFormat((float *)&asParameterList[0].Norm, sizeof(asParameterList[0]));                               /* 0.0 - Default HEX Visualization; Any other -> Default Decimal Visualization */
}

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



    /* Recorder 0 Configuration */
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetRecorderConfigurationSkipSamples (
            CSMON_RECORDER_0,
            RECORDER0_PRETRIGGER_SAMPLE_COUNT,   /* PreTriggerSampleCount */
            RECORDER0_TOTAL_SAMPLE_COUNT,   /* TotalSampleCount */
            RECORDER0_SAMPLE_FREQUENCY_HZ / 3.0); /* Sample Frequency in Hz */



    /* Note !!! CSMON_eSetRecorderTriggerAtPosition call after CSMON_eSetRecorderConfiguration */


    /* Trigger Recorder 0 */
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetRecorderTriggerAtPosition (
            CSMON_RECORDER_0,
            PARAM_ID_STARUNNINGMODE,
            (uint32_t)true,
            (uint16_t)CSMON_TRIGGER_MODE_FALLING_EDGE);




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
    DINT;

    //
    // Disable the watchdog
    //
    SysCtl_disableWatchdog();

#ifdef _2806x_ISO_CONTROL_CARD
    EALLOW;
    //  LED2 - red led (the middle led between LED3 and the power Supply green Led)
    GpioCtrlRegs.GPAPUD.bit.GPIO31 = 0;   // Enable pullup on GPIO31
    GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;  // GPIO31 = GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;  // GPIO31 = output
    GpioDataRegs.GPADAT.bit.GPIO31 = 0; // level

    //  LED3 - red led nearest to the micro USB
    GpioCtrlRegs.GPBPUD.bit.GPIO34 = 0;  // Enable pullup on GPIO34
    GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0; // GPIO34 = GPIO34
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;  // GPIO34 = output
    GpioDataRegs.GPBDAT.bit.GPIO34 = 0; // level
    EDIS;

#endif

    //while(1);

    // Configure PLL, disable WD, enable peripheral clocks.
    Device_init();

    // Disable pin locks and enable internal PullUps.
    Device_initGPIO();


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

    ExternalParametersInitialization();

    /* Recorder And Scope Initialization Made Once after parameter initialized */
    RecordersInitialization();

    // CSMON Internal Recorders Setup with Already Made Configuration
    CSMON_vAddSetupRecorderParameterMask(CSMON_MASK_RECORDER_0);


    //
    // Disable the watchdog
    //
    SysCtl_enableWatchdog();

    EINT;
    ERTM;

    for (;;)
    {
        // Reset the WatchDog counter
        SysCtl_serviceWatchdog();

        // CSMON Process In Main Loop - Big Delays On Disconnect 4-5ms; On Connect 12-35ms If Not Interrupted (EMIF Checksum PC Application)
        eResponseCode_CSMON_eProcess = CSMON_eProcess();
        // Check CSMON Response Code if needed
        ASSERT(eResponseCode_CSMON_eProcess != CSMON_RESPONSE_CODE_OK);

    }
}



