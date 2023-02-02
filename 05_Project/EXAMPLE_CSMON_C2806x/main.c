/* *****************************************************************************
 * File:   main.c
 * Author: Damyan Neev
 *
 * Created on 2023 01 24 09:06
 * 
 * Description: Example for CSMON_C28xx library integration.
 * 
 **************************************************************************** */
 
/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "MAIN_sDateTime_t.h"

#include "device.h"
#include "csmon_config.h"
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
#define RECORDER0_PRETRIGGER_SAMPLE_COUNT   56
#define RECORDER0_TOTAL_SAMPLE_COUNT        64

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


#define STAT_LED_EQEP1I_PIN 23



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
bool bDummyStatsDevRunning = false;
bool bDummyReqstDevRunning = false;
int16_t s16ScaleDataTst;
int16_t s16DummyIncrementLoop;
int16_t s16DummyCurrentPhaseA = 0;
int16_t s16DummyCurrentPhaseB = 0;
int16_t s16DummyCurrentPhaseC = 0;
uint32_t u32GetBaudError_PPM;
uint16_t u16WatchdogPrescalerTime;
int16_t s16DummyVoltageDCLink;
int32_t s32DummyVoltageDCLink = 0;
int32_t s32DummyCurrentPhaseA = 0;
int32_t s32DummyCurrentPhaseB = 0;
int32_t s32DummyCurrentPhaseC = 0;
int16_t s16DummyVoltageDCLinkStartup = 0;
int16_t s16DummyCurrentPhaseAStartup = 10;
int16_t s16DummyCurrentPhaseBStartup = 20;
int16_t s16DummyCurrentPhaseCStartup = 30;
int16_t s16DummyVoltageDCLinkStop = -1;
int16_t s16DummyCurrentPhaseAStop = -1;
int16_t s16DummyCurrentPhaseBStop = -1;
int16_t s16DummyCurrentPhaseCStop = -1;
int16_t s16DummyVoltageDCLinkIdle = -1;
int16_t s16DummyCurrentPhaseAIdle = -1;
int16_t s16DummyCurrentPhaseBIdle = -1;
int16_t s16DummyCurrentPhaseCIdle = -1;
int16_t s16DummyIncrementLoopA = 1;
int16_t s16DummyIncrementLoopB = 1;
int16_t s16DummyIncrementLoopC = 1;
int16_t s16DummyIncrementLoopV = 1;
int16_t s16DummyIncrementLoopCDiff;




char UserStringConfiguration[16] = "charAlabalaNica";

#define STR_PADDED1(x) (x "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0")
#define ZIP_STRING1(x) { \
    ((uint16_t)STR_PADDED1(x)[ 0] & 0xFF) | ((uint16_t)STR_PADDED1(x)[ 1] << 8), \
    ((uint16_t)STR_PADDED1(x)[ 2] & 0xFF) | ((uint16_t)STR_PADDED1(x)[ 3] << 8), \
    ((uint16_t)STR_PADDED1(x)[ 4] & 0xFF) | ((uint16_t)STR_PADDED1(x)[ 5] << 8), \
    ((uint16_t)STR_PADDED1(x)[ 6] & 0xFF) | ((uint16_t)STR_PADDED1(x)[ 7] << 8), \
    ((uint16_t)STR_PADDED1(x)[ 8] & 0xFF) | ((uint16_t)STR_PADDED1(x)[ 9] << 8), \
    ((uint16_t)STR_PADDED1(x)[10] & 0xFF) | ((uint16_t)STR_PADDED1(x)[11] << 8), \
    ((uint16_t)STR_PADDED1(x)[12] & 0xFF) | ((uint16_t)STR_PADDED1(x)[13] << 8), \
    ((uint16_t)STR_PADDED1(x)[14] & 0xFF) | ((uint16_t)STR_PADDED1(x)[15] << 8), \
    ((uint16_t)STR_PADDED1(x)[16] & 0xFF) | ((uint16_t)STR_PADDED1(x)[17] << 8), \
    ((uint16_t)STR_PADDED1(x)[18] & 0xFF) | ((uint16_t)STR_PADDED1(x)[19] << 8), \
    ((uint16_t)STR_PADDED1(x)[20] & 0xFF) | ((uint16_t)STR_PADDED1(x)[21] << 8), \
    ((uint16_t)STR_PADDED1(x)[22] & 0xFF) | ((uint16_t)STR_PADDED1(x)[23] << 8), \
    ((uint16_t)STR_PADDED1(x)[24] & 0xFF) | ((uint16_t)STR_PADDED1(x)[25] << 8), \
    ((uint16_t)STR_PADDED1(x)[26] & 0xFF) | ((uint16_t)STR_PADDED1(x)[27] << 8), \
    ((uint16_t)STR_PADDED1(x)[28] & 0xFF) | ((uint16_t)STR_PADDED1(x)[29] << 8), \
    ((uint16_t)STR_PADDED1(x)[30] & 0xFF) | ((uint16_t)STR_PADDED1(x)[31] << 8), \
    }

char UserZippedString[16] = ZIP_STRING1("au8AlabalaNica");

char UserUnicodeString[16] = {'u', '1', '6', 'A', 'l', 'a', 'b', 'a', 'l', 'a', 'N', 'i', 'c', 'a', '\0'};



CSMON_eResponseCode_t eResponseCode_CSMON_eInit = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eProcess = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetServerOnStatus = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetRecorder = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetFlagProcessPassed = CSMON_RESPONSE_CODE_OK;
uint32_t u32GetBaudError_PPM = 0;

uint32_t u32TimeMainLoopProcessCSMON_Bgn_Ticks = 0;
uint32_t u32TimeMainLoopProcessCSMON_End_Ticks = 0;
uint32_t u32TimeMainLoopProcessCSMON_Now_Ticks = 0;

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
    CSMON_eSetParameterListRealAddress((uint16_t *)&asParameterList[0].u16RealAddress, sizeof(asParameterList[0]), CSMON_CONFIG_PARAMETER_COUNT_MAX);                     /* First Put Real Address to calculate count parameters internally (last index is NULL) */
#else
    CSMON_eSetParameterListRealAddress((uint32_t *)&asParameterList[0].u32RealAddress, sizeof(asParameterList[0]), CSMON_CONFIG_PARAMETER_COUNT_MAX);                     /* First Put Real Address to calculate count parameters internally (last index is NULL) */
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
 * ControlProcess
 **************************************************************************** */
void ControlProcess(void)
{
#ifdef _CS_1291
    GPIO_writePin_2806x(STAT_LED_EQEP1I_PIN, 1);     /* J17 at board corner before ground (pin 13 - second outside pin corner to middle) */
#endif

    //
    // Test For Data Consistency and Control Emulation
    //
    if (bDummyReqstDevRunning)
    {
        if (bDummyStatsDevRunning == false)
        {
            bDummyStatsDevRunning = true;

            CSMON_vSetStartRecorderParameterMask(CSMON_MASK_RECORDER_0);

            s16DummyCurrentPhaseA = s16DummyCurrentPhaseAStartup;
            s16DummyCurrentPhaseB = s16DummyCurrentPhaseBStartup;
            s16DummyCurrentPhaseC = s16DummyCurrentPhaseCStartup;
            s16DummyVoltageDCLink = s16DummyVoltageDCLinkStartup;

            s32DummyCurrentPhaseA = (int32_t)s16DummyCurrentPhaseAStartup * 10000;
            s32DummyCurrentPhaseB = (int32_t)s16DummyCurrentPhaseBStartup * 10000;
            s32DummyCurrentPhaseC = (int32_t)s16DummyCurrentPhaseCStartup * 10000;
            s32DummyVoltageDCLink = (int32_t)s16DummyVoltageDCLinkStartup * 10000;
        }
        else
        {
            s16DummyCurrentPhaseA += s16DummyIncrementLoopA;
            s16DummyCurrentPhaseB += s16DummyIncrementLoopB;
            s16DummyCurrentPhaseC += s16DummyIncrementLoopC;
            s16DummyVoltageDCLink += s16DummyIncrementLoopV;

            s32DummyCurrentPhaseA += (int32_t)s16DummyIncrementLoopA * 10000;
            s32DummyCurrentPhaseB += (int32_t)s16DummyIncrementLoopB * 10000;
            s32DummyCurrentPhaseC += (int32_t)s16DummyIncrementLoopC * 10000;
            s32DummyVoltageDCLink += (int32_t)s16DummyIncrementLoopV * 10000;

            //s16DummyCurrentPhaseC += s16DummyIncrementLoopCDiff;
            //s16DummyIncrementLoopCDiff = 0 - s16DummyIncrementLoopCDiff;
        }

    }
    else
    {
        if (bDummyStatsDevRunning == true)
        {
            bDummyStatsDevRunning = false;
            CSMON_vSetStopRecorderParameterMask(CSMON_MASK_RECORDER_0);

//            s16DummyCurrentPhaseAIdle = s16DummyCurrentPhaseA;
//            s16DummyCurrentPhaseBIdle = s16DummyCurrentPhaseB;
//            s16DummyCurrentPhaseCIdle = s16DummyCurrentPhaseC;
//            s16DummyVoltageDCLinkIdle = s16DummyVoltageDCLink;
//
//            s16DummyCurrentPhaseA = s16DummyCurrentPhaseAStop;
//            s16DummyCurrentPhaseB = s16DummyCurrentPhaseBStop;
//            s16DummyCurrentPhaseC = s16DummyCurrentPhaseCStop;
//            s16DummyVoltageDCLink = s16DummyVoltageDCLinkStop;
//
//            s32DummyCurrentPhaseA = (int32_t)s16DummyCurrentPhaseAStop;
//            s32DummyCurrentPhaseB = (int32_t)s16DummyCurrentPhaseBStop;
//            s32DummyCurrentPhaseC = (int32_t)s16DummyCurrentPhaseCStop;
//            s32DummyVoltageDCLink = (int32_t)s16DummyVoltageDCLinkStop;
        }
        else
        {
//            s16DummyCurrentPhaseA = s16DummyCurrentPhaseAIdle;
//            s16DummyCurrentPhaseB = s16DummyCurrentPhaseBIdle;
//            s16DummyCurrentPhaseC = s16DummyCurrentPhaseCIdle;
//            s16DummyVoltageDCLink = s16DummyVoltageDCLinkIdle;
        }

    }

    //
    // Artificial Delay Control Loop
    //
    SysCtl_delay(u32DelayCtrlLoop_Ticks);

    // Process Passed Flag Set - Need to be called from Processes with higher priority level in order CSMON to be able to get meaning-full (consistent) data
    eResponseCode_CSMON_eSetFlagProcessPassed = CSMON_eSetFlagProcessPassed (CSMON_ID_PROCESS_CONTROL_PRIMARY);
    ASSERT(eResponseCode_CSMON_eSetFlagProcessPassed != CSMON_RESPONSE_CODE_OK);
    // Check CSMON Response Code (... or Embed Assert For Debug) if needed

#ifdef _CS_1291
    GPIO_writePin_2806x(STAT_LED_EQEP1I_PIN, 0);     /* J17 at board corner before ground (pin 13 - second outside pin corner to middle) */
#endif
}


/* *****************************************************************************
 * Board_init
 **************************************************************************** */
static void Board_init(void)
{
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
}


/* *****************************************************************************
 * CsMon_init
 **************************************************************************** */
void CsMon_init(void)
{

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
}


/* *****************************************************************************
 * init
 **************************************************************************** */
void init(void)
{
    // Configure PLL, disable WD, enable peripheral clocks.
    Device_init();

    // Disable pin locks and enable internal PullUps.
    Device_initGPIO();

    Board_init();

    CsMon_init();

    //
    // Enable the WatchDog
    //
    SysCtl_serviceWatchdog();
    SysCtl_enableWatchdog();
    //SysCtl_disableWatchdog();

    EINT;
    ERTM;
}


/* *****************************************************************************
 * main
 **************************************************************************** */
void main(void)
{
    init();

    int controlPrescaler = 10;
    int controlPrescalerCounter = 10;

    for (;;)
    {
        // Reset the WatchDog counter
        SysCtl_serviceWatchdog();

        // CSMON Process In Main Loop - Big Delays On Disconnect 4-5ms; On Connect 12-35ms If Not Interrupted (EMIF Checksum PC Application)
        eResponseCode_CSMON_eProcess = CSMON_eProcess();
        // Check CSMON Response Code if needed
        ASSERT(eResponseCode_CSMON_eProcess != CSMON_RESPONSE_CODE_OK);

        controlPrescalerCounter++;
        if (controlPrescalerCounter >= controlPrescaler)
        {
            controlPrescalerCounter = 0;
            ControlProcess();
        }

        //
        // Device Running Control Indication - Set on Enter/Exit Run Mode
        //
        eResponseCode_CSMON_eSetServerOnStatus = CSMON_eSetServerOnStatus(bDummyStatsDevRunning);
        // Check CSMON Response Code if needed
        ASSERT(eResponseCode_CSMON_eSetServerOnStatus != CSMON_RESPONSE_CODE_OK);
    }
}

