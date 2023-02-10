#include "csmon_accessibles.h"

#include "MotorParam/Generated/csmon_config.h"

#include "csmon_lib_support/csmon_lib_support.h"


#define PARAM_ID_STARUNNINGMODE     0
#define PARAM_ID_VOLTAGE_DCLINK     8
#define PARAM_ID_CURRENT_PHASEA     9
#define PARAM_ID_CURRENT_PHASEB     10
#define PARAM_ID_CURRENT_PHASEC     11

#define PARAM_ID_VOLTAGE_DCLINK_32  28
#define PARAM_ID_CURRENT_PHASEA_32  29
#define PARAM_ID_CURRENT_PHASEB_32  30
#define PARAM_ID_CURRENT_PHASEC_32  31

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

Currents_t gCurrents = {-1, -1, -1};




char UserStringConfiguration[16] = "charAlabalaNica";

char UserZippedString[16] = ZIP_STRING("au8AlabalaNica");

char UserUnicodeString[16] = {'u', '1', '6', 'A', 'l', 'a', 'b', 'a', 'l', 'a', 'N', 'i', 'c', 'a', '\0'};




/* *****************************************************************************
 * CsMonRecordersInitialization
 * declared in csmon_lib_support.h
 **************************************************************************** */
extern void CsMonRecordersInitialization(void)
{
    uint16_t u16ValidParameters = 0;

    /* Recorder 0 */
    u16ValidParameters = CSMON_POSITION_IN_RECORDER_0;

    csMonResponses.eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_0, PARAM_ID_STARUNNINGMODE, u16ValidParameters);
    if (csMonResponses.eSetRecorder == CSMON_RESPONSE_CODE_OK)
    {
        u16ValidParameters++;
    }
    csMonResponses.eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_0, PARAM_ID_CURRENT_PHASEA, u16ValidParameters);
    if (csMonResponses.eSetRecorder == CSMON_RESPONSE_CODE_OK)
    {
        u16ValidParameters++;
    }
    csMonResponses.eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_0, PARAM_ID_CURRENT_PHASEB, u16ValidParameters);
    if (csMonResponses.eSetRecorder == CSMON_RESPONSE_CODE_OK)
    {
        u16ValidParameters++;
    }

    csMonResponses.eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_0, PARAM_ID_CURRENT_PHASEC, u16ValidParameters);
    if (csMonResponses.eSetRecorder == CSMON_RESPONSE_CODE_OK)
    {
        u16ValidParameters++;
    }

    csMonResponses.eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_0, PARAM_ID_CURRENT_PHASEA_32, u16ValidParameters);
    if (csMonResponses.eSetRecorder == CSMON_RESPONSE_CODE_OK)
    {
        u16ValidParameters++;
    }

    csMonResponses.eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_0, PARAM_ID_CURRENT_PHASEB_32, u16ValidParameters);
    if (csMonResponses.eSetRecorder == CSMON_RESPONSE_CODE_OK)
    {
        u16ValidParameters++;
    }

    csMonResponses.eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_0, PARAM_ID_CURRENT_PHASEC_32, u16ValidParameters);
    if (csMonResponses.eSetRecorder == CSMON_RESPONSE_CODE_OK)
    {
        u16ValidParameters++;
    }
    csMonResponses.eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_0, PARAM_ID_VOLTAGE_DCLINK_32, u16ValidParameters);

    if (csMonResponses.eSetRecorder == CSMON_RESPONSE_CODE_OK)
    {
        u16ValidParameters++;
    }
    csMonResponses.eSetRecorder = CSMON_eSetParameterCountInRecorder (
            CSMON_RECORDER_0, u16ValidParameters);



    /* Recorder 0 Configuration */
    csMonResponses.eSetRecorder = CSMON_eSetRecorderConfigurationSkipSamples (
            CSMON_RECORDER_0,
            RECORDER0_PRETRIGGER_SAMPLE_COUNT,   /* PreTriggerSampleCount */
            RECORDER0_TOTAL_SAMPLE_COUNT,   /* TotalSampleCount */
            RECORDER0_SAMPLE_FREQUENCY_HZ / 3.0); /* Sample Frequency in Hz */



    /* Note !!! CSMON_eSetRecorderTriggerAtPosition call after CSMON_eSetRecorderConfiguration */


    /* Trigger Recorder 0 */
    csMonResponses.eSetRecorder = CSMON_eSetRecorderTriggerAtPosition (
            CSMON_RECORDER_0,
            PARAM_ID_STARUNNINGMODE,
            (uint32_t)true,
            (uint16_t)CSMON_TRIGGER_MODE_FALLING_EDGE);
}
