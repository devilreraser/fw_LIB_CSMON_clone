/* *****************************************************************************
 * File:   csmon.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 10 01 23:05
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
#ifndef CSMON_H
#define	CSMON_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdbool.h>
#include <stdint.h>

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* used in eSetPrameter Function - combined size must not exceed 63 characters or will be trunked */
#define CSMON_SET_PARAMETER_NAME_LENGTH_MAX     48
#define CSMON_SET_PARAMETER_UNIT_LENGTH_MAX     16

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */
#define CSMON_LIBRARY_VERSION_HI    0
#define CSMON_LIBRARY_VERSION_LO    0

#define CSMON_LIBRARY_VERSION       (uint16_t)(((uint16_t)((uint_least8_t)(CSMON_LIBRARY_VERSION_HI & 0x00FF)) << 8) | ((uint_least8_t)(CSMON_LIBRARY_VERSION_LO & 0x00FF)))

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */
typedef enum
{
    /* Success */
    CSMON_RESPONSE_CODE_OK = 0,

    /* No Successes or Statuses */
    CSMON_RESPONSE_CODE_FALSE = 1,
    CSMON_RESPONSE_CODE_SCI_ERROR_MIN_5_PERCENT = 2,
    CSMON_RESPONSE_CODE_SCI_ERROR_MIN_1_PERCENT = 3,
    CSMON_RESPONSE_CODE_NOT_FULL_COPY_STRING = 4,

    /* Errors and Failures */
    CSMON_RESPONSE_FAIL_NOT_IMPLEMENTED = -1,
    CSMON_RESPONSE_FAIL_GENERAL = -2,
    CSMON_RESPONSE_FAIL_SCI_ERROR_MIN_10_PERCENT = -3,
    CSMON_RESPONSE_FAIL_CSMON_PARAM_ARRAY_FULL = -4,
    CSMON_RESPONSE_FAIL_MODBUS_HOLDREG_ADD_ARRAY_FULL = -5,
    CSMON_RESPONSE_FAIL_CSMON_PARAM_INPUTS_INVALID = -6,
    CSMON_RESPONSE_FAIL_CSMON_PARAM_ARRAY_INDEX_OUT_OF_RANGE = -7,     /* Parameter ID Not Found */
    CSMON_RESPONSE_FAIL_CSMON_RECORDER_SCOPE_INDEX_OUT_OF_RANGE = -8,
    CSMON_RESPONSE_FAIL_CSMON_PARAMETER_INDEX_IN_RECORDER_SCOPE_OUT_OF_RANGE = -9,
    CSMON_RESPONSE_FAIL_CSMON_PARAMETER_COUNT_IN_RECORDER_SCOPE_OUT_OF_RANGE = -10,
    CSMON_RESPONSE_FAIL_CSMON_TRIGGER_INDEX_IN_RECORDER_SCOPE_OUT_OF_RANGE = -11,
    CSMON_RESPONSE_FAIL_CSMON_PARAM_INTERNAL_NULL_POINTER_TO_ARRAY_DETECTED = -12,

}CSMON_eResponseCode_t;

typedef enum
{
    CSMON_ID_PROCESS_CONTROL_PRIMARY = 0,
    CSMON_ID_PROCESS_CONTROL_SECONDARY = 1,

    CSMON_ID_PROCESS_COUNT

}CSMON_eIDProcess_t;

typedef enum
{
    CSMON_ID_PERIPHERAL_SCI_MODBUS = 0,
    CSMON_ID_PERIPHERAL_SCI_HEXMON = 1,

    CSMON_ID_PERIPHERAL_COUNT
}CSMON_eIDPeripheral_t;

typedef enum
{
    CSMON_MASK_RECORDER_NONE = 0,
    CSMON_MASK_RECORDER_0    = (1 << 0),
    CSMON_MASK_RECORDER_1    = (1 << 1),
    CSMON_MASK_RECORDER_2    = (1 << 2),
    CSMON_MASK_RECORDERS_0_1 =((1 << 0)|(1 << 1)),
    CSMON_MASK_RECORDERS_1_2 =((1 << 1)|(1 << 2)),
    CSMON_MASK_RECORDERS_2_0 =((1 << 2)|(1 << 0)),
    CSMON_MASK_RECORDERS_012 =((1 << 0)|(1 << 1)|(1 << 2)),
}CSMON_eRecorderParameterMask_t;

typedef enum
{
    CSMON_RECORDER_0    = 0,
    CSMON_RECORDER_1    = 1,
    CSMON_RECORDER_2    = 2,
}CSMON_eRecorderIndex_t;

typedef enum
{
    CSMON_POSITION_IN_RECORDER_0    = 0,
    CSMON_POSITION_IN_RECORDER_1    = 1,
    CSMON_POSITION_IN_RECORDER_2    = 2,
    CSMON_POSITION_IN_RECORDER_3    = 3,
    CSMON_POSITION_IN_RECORDER_4    = 4,
    CSMON_POSITION_IN_RECORDER_5    = 5,
    CSMON_POSITION_IN_RECORDER_6    = 6,
    CSMON_POSITION_IN_RECORDER_7    = 7,
    CSMON_POSITION_IN_RECORDER_8    = 8,
    CSMON_POSITION_IN_RECORDER_9    = 9,
    CSMON_POSITION_IN_RECORDER_10   = 10,
    CSMON_POSITION_IN_RECORDER_11   = 11,
    CSMON_POSITION_IN_RECORDER_12   = 12,
    CSMON_POSITION_IN_RECORDER_13   = 13,
    CSMON_POSITION_IN_RECORDER_14   = 14,
    CSMON_POSITION_IN_RECORDER_15   = 15,
    CSMON_POSITION_IN_RECORDER_16   = 16,
    CSMON_POSITION_IN_RECORDER_17   = 17,
    CSMON_POSITION_IN_RECORDER_18   = 18,
    CSMON_POSITION_IN_RECORDER_19   = 19,
    CSMON_POSITION_IN_RECORDER_COUNT
}CSMON_ePositionInRecorder_t;

typedef enum
{
    CSMON_SCOPE_0    = 0,
    CSMON_SCOPE_1    = 1,
    CSMON_SCOPE_2    = 2,
}CSMON_eScopeIndex_t;

typedef enum
{
    CSMON_POSITION_IN_SCOPE_0    = 0,
    CSMON_POSITION_IN_SCOPE_1    = 1,
    CSMON_POSITION_IN_SCOPE_2    = 2,
    CSMON_POSITION_IN_SCOPE_3    = 3,
    CSMON_POSITION_IN_SCOPE_4    = 4,
    CSMON_POSITION_IN_SCOPE_5    = 5,
    CSMON_POSITION_IN_SCOPE_6    = 6,
    CSMON_POSITION_IN_SCOPE_7    = 7,
    CSMON_POSITION_IN_SCOPE_8    = 8,
    CSMON_POSITION_IN_SCOPE_9    = 9,
    CSMON_POSITION_IN_SCOPE_10   = 10,
    CSMON_POSITION_IN_SCOPE_11   = 11,
    CSMON_POSITION_IN_SCOPE_12   = 12,
    CSMON_POSITION_IN_SCOPE_13   = 13,
    CSMON_POSITION_IN_SCOPE_14   = 14,
    CSMON_POSITION_IN_SCOPE_15   = 15,
    CSMON_POSITION_IN_SCOPE_16   = 16,
    CSMON_POSITION_IN_SCOPE_17   = 17,
    CSMON_POSITION_IN_SCOPE_18   = 18,
    CSMON_POSITION_IN_SCOPE_19   = 19,
    CSMON_POSITION_IN_SCOPE_COUNT
}CSMON_ePositionInScope_t;

typedef enum
{
    CSMON_COUNT_PARAMETERS_0    = 0,
    CSMON_COUNT_PARAMETERS_1    = 1,
    CSMON_COUNT_PARAMETERS_2    = 2,
    CSMON_COUNT_PARAMETERS_3    = 3,
    CSMON_COUNT_PARAMETERS_4    = 4,
    CSMON_COUNT_PARAMETERS_5    = 5,
    CSMON_COUNT_PARAMETERS_6    = 6,
    CSMON_COUNT_PARAMETERS_7    = 7,
    CSMON_COUNT_PARAMETERS_8    = 8,
    CSMON_COUNT_PARAMETERS_9    = 9,
    CSMON_COUNT_PARAMETERS_10   = 10,
    CSMON_COUNT_PARAMETERS_11   = 11,
    CSMON_COUNT_PARAMETERS_12   = 12,
    CSMON_COUNT_PARAMETERS_13   = 13,
    CSMON_COUNT_PARAMETERS_14   = 14,
    CSMON_COUNT_PARAMETERS_15   = 15,
    CSMON_COUNT_PARAMETERS_16   = 16,
    CSMON_COUNT_PARAMETERS_17   = 17,
    CSMON_COUNT_PARAMETERS_18   = 18,
    CSMON_COUNT_PARAMETERS_19   = 19,
    CSMON_COUNT_PARAMETERS_20   = 20,
}CSMON_eCountParameters_t;

/* trigger mode (0 = disabled, 1 = _/, 2 = \_) ??? consider both */
typedef enum
{
    CSMON_TRIGGER_MODE_NONE,
    CSMON_TRIGGER_MODE_RISING_EDGE,
    CSMON_TRIGGER_MODE_FALLING_EDGE,
    CSMON_TRIGGER_MODE_ANY_EDGE,  /* not implemented for now */
}CSMON_eTriggerMode_t;

typedef enum
{
    CSMON_VISUAL_TYPE_MASK = 0x0007,

    CSMON_VISUAL_TYPE_DEF = 0x0007,     /* type not specified (default hex if multiplier 0; if multiplier != 0 decimal) */

    CSMON_VISUAL_TYPE_HEX = 0x0000,
    CSMON_VISUAL_TYPE_DEC = 0x0001,
    CSMON_VISUAL_TYPE_BIN = 0x0002,
    CSMON_VISUAL_TYPE_SCI = 0x0003,     /* scientific */
    CSMON_VISUAL_TYPE_ENG = 0x0004,     /* engineering */

    CSMON_VISUAL_MASK     = 0xFFFF

}CSMON_eVisualType_t; /* 16 bits - Library Internal Note! -> change also HMRECPRM_eVisualType_t */

typedef enum
{
    CSMON_TIME_MICRO_SECONDS = 0,        /* u32MicroSec0BuffOvf1 in Micro-Seconds  */
    CSMON_TIME_BUFFER_OVERFLOW = 1,      /* u32MicroSec0BuffOvf1 is buffer overflow counter (ticks(value) from free rolling timer) */
}CSMON_eMicroSec0BuffOvf1Mode_t;



/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
typedef void (*CSMON_pfVoid_t)(void);







typedef struct
{
    uint16_t bTriggered         : 1;    /* set bit -> trigger condition detected                            #define TSP_CAPTURE 0x0001 */
    uint16_t bReady             : 1;    /* set bit -> trigger condition. captured and post trigger expired  #define TSP_READY 0x0002 */
    uint16_t bWrongConfig       : 1;    /* Error during configuration TSPDAT_T TSdat                        #define TSP_WRONG_CONFIG 0x0004 */
    uint16_t bNotRunning        : 1;    /* TSP not released                                                 #define TSP_DISABLED 0x0008 */
    uint16_t bInit              : 1;    /*                                                                  #define TSP_IS_INIT 0x0010 */

}CSMON_sExternalRecorderStatus;

typedef struct
{
    uint16_t bReleased          : 1;    /* set bit -> TSP is released       #define TSP_ENABLE 0x0001 */
    uint16_t bStart             : 1;    /* set bit -> TSP starts            #define TSP_START 0x0002 */

}CSMON_sExternalRecorderControl;







/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */
#define CSMON_u32PercentToPPM(x)    (uint32_t)(x*10000)

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */

/* *****************************************************************************
 * CSMON_eInit   -   Call in main initialization block.
 *
 * Note - The Peripheral Bus Clock must be Initialized already in order to
 * calculate properly the baud rate.
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eInit(void);

/* *****************************************************************************
 * CSMON_eProcess   -   Call in main loop
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eProcess(void);

/* *****************************************************************************
 * CSMON_eSetServerOnStatus   -   Call when Control Run Status Changed
 *
 * bInput - boolean
 *                  false   - Off Status - Control Stopped
 *                  true    - Run Status - Control Running
 *
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eSetServerOnStatus(bool bInput);

/* *****************************************************************************
 * CSMON_u32GetSCIError_PPM   -   Error Baud Rate (per Million)
 **************************************************************************** */
uint32_t CSMON_u32GetBaudError_PPM (CSMON_eIDPeripheral_t eID);

/* *****************************************************************************
 * CSMON_eSetFlagProcessPassed   -   Call at end of Priority Process Execution
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eSetFlagProcessPassed (CSMON_eIDProcess_t eID);

/* *****************************************************************************
 * CSMON_eSetTimerPeriodISRFunctionRegister   -   Register Function Call in
 * Timer Period ISR
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eSetTimerPeriodISRFunctionRegister (CSMON_pfVoid_t pfInput);

#if 0
/* *****************************************************************************
 * CSMON_eSetParameter
 *
 * Input:
 *      uint16_t u16ParameterIndexID - Parameter ID or Index in Table
 *      uint32_t u32RealAddress      - Address of Parameter in DSP Memory Space (pVal Type-casted to uint32_t)
 *      uint16_t u16ParamAttributes  - Parameter Attributes                     (Attr)
 *      uint_least8_t* pu8Name       - Parameter Name                           (Name)
 *      uint_least8_t* pu8Unit       - Parameter Units Description              (Unit)
 *      uint32_t u32Max              - Maximum Value Type-casted to uint32_t    (Max Type-casted to uint32_t)
 *      uint32_t u32Min              - Minimum Value Type-casted to uint32_t    (Min Type-casted to uint32_t)
 *      uint32_t u32Def              - Default Value Type-casted to uint32_t    (Def Type-casted to uint32_t)
 *      float Norm                   - Normalization Scaling Factor             (Norm)
 *
 **************************************************************************** */
#define CSMON_eSetParameter (       \
            u16ParameterIndexID,    \
            u32RealAddress,         \
            u16ParamAttributes,     \
            pu8Name,                \
            pu8Unit,                \
            u32Max,                 \
            u32Min,                 \
            u32Def,                 \
            Norm)                   \
        CSMON_eSetParameter (       \
            u16ParameterIndexID,    \
            u32RealAddress,         \
            u16ParamAttributes,     \
            pu8Name,                \
            pu8Unit,                \
            u32Max,                 \
            u32Min,                 \
            u32Def,                 \
            Norm,                   \
            0 , /* u8BitCount */    \
            0)  /* u8StartBitOrArrayElementCount */

#endif

/* *****************************************************************************
 * CSMON_eSetParameter
 *
 * Input:
 *      uint16_t u16ParameterIndexID - Parameter ID or Index in Table (must be 0..9999 for Array(string) Parameters)
 *      uint32_t u32RealAddress      - Address of Parameter in DSP Memory Space (pVal Type-casted to uint32_t)
 *      uint16_t u16ParamAttributes  - Parameter Attributes                     (Attr)
 *      uint_least8_t* pu8Name       - Parameter Name                           (Name)
 *      uint_least8_t* pu8Unit       - Parameter Units Description              (Unit)
 *      uint32_t u32Max              - Maximum Value Type-casted to uint32_t    (Max Type-casted to uint32_t)
 *      uint32_t u32Min              - Minimum Value Type-casted to uint32_t    (Min Type-casted to uint32_t)
 *      uint32_t u32Def              - Default Value Type-casted to uint32_t    (Def Type-casted to uint32_t)
 *      float Norm                   - Normalization Scaling Factor             (Norm)
 *      uint_least8_t u8BitCountOrArrayElementSize
 *                                   - BitCount for bit field parameters.
 *                                   - for non bit field parameters set to 0.
 *                                   - if 0 the bit count (array element size) taken from the Parameter Attributes
 *      uint_least8_t u8StartBitOrArrayElementCount
 *                                   - BitOffset 0..15 in 16-bit Parameter (The Start Bit Index Of Parameter)
 *                                   - BitOffset 0..31 in 32-bit Parameter (The Start Bit Index Of Parameter)
 *                                   - BitOffset 0..64 in 64-bit Parameter (The Start Bit Index Of Parameter)
 *                                   - For Array Parameter - the number of elements:(8,16,32,64 Bit Element)
 *                                   - For Array the element size is calculated from Parameter Attributes
 *
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eSetParameter (
        uint16_t u16ParameterIndexID,
        uint32_t u32RealAddress,
        uint16_t u16ParamAttributes,
        uint_least8_t* pu8Name,
        uint_least8_t* pu8Unit,
        uint32_t u32Max,
        uint32_t u32Min,
        uint32_t u32Def,
        float Norm,
        uint_least8_t u8BitCountOrArrayElementSize,
        uint_least8_t u8StartBitOrArrayElementCount
        );


/* *****************************************************************************
 * CSMON_eApplyParameterChanges
 *
 * Hint:
 *  Call This After Used (one or several times) CSMON_eSetParameter()
 *
 * Description:
 *  Internal Library Runtime Initialization
 *
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eApplyParameterChanges (void);


/* *****************************************************************************
 * CSMON_eResetParameterTable
 *
 * Hint:
 *  Call This When detected Invalid Parameter Table
 *
 * Description:
 *  Internal Library Used Parameter Count Reset
 *
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eResetParameterTable (void);


/* *****************************************************************************
 * CSMON_u32GetParameterCheckSum
 *
 * Description:
 *  Calculates and returns CheckSum of the Parameter Table
 *
 **************************************************************************** */
uint32_t CSMON_u32GetParameterCheckSum (void);


/* *****************************************************************************
 * CSMON_eSetRecorderConfiguration
 *
 * Input:
 *      uint16_t u16RecorderIndex   - Recorder Index (Recorder0, ... RecorderN)
 *      uint32_t u32PreTriggerSampleCount
 *      uint32_t u32TotalSampleCount
 *      float f32SampleRateHz       - Sample Rate in Hz
 *
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eSetRecorderConfiguration (
        uint16_t u16RecorderIndex,
        uint32_t u32PreTriggerSampleCount,
        uint32_t u32TotalSampleCount,
        float f32SampleRateHz);

/* *****************************************************************************
 * CSMON_eSetRecorderTriggerAtPosition
 *
 * Input:
 *      uint16_t u16RecorderIndex   - Recorder Index (Recorder0, ... RecorderN)
 *      uint16_t u16ParameterID     - Parameter ID or Index in Table
        uint32_t u32TresholdValue   - TypeCast To uint32_t
        uint16_t u16TriggerMode     - See CSMON_eTriggerMode_t
 *
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eSetRecorderTriggerAtPosition (
        uint16_t u16RecorderIndex,
        uint16_t u16ParameterID,
        uint32_t u32TresholdValue,
        uint16_t u16TriggerMode);

/* *****************************************************************************
 * CSMON_eSetParameterInRecorderAtPosition
 *
 * Input:
 *      uint16_t u16RecorderIndex    - Recorder Index (Recorder0, ... RecorderN)
 *      uint16_t u16ParameterID      - Parameter ID or Index in Table
 *      uint16_t u16ParameterPos     - Position of the Parameter In The Recorder Sequence
 *
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eSetParameterInRecorderAtPosition (
        uint16_t u16RecorderIndex,
        uint16_t u16ParameterID,
        uint16_t u16ParameterPos);

/* *****************************************************************************
 * CSMON_eSetParameterCountInRecorder
 *
 * Input:
 *      uint16_t u16RecorderIndex    - Recorder Index (Recorder0, ... RecorderN)
 *      uint16_t u16ParameterCount   - Count Parameters used In The Recorder Sequence
 *
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eSetParameterCountInRecorder (
        uint16_t u16RecorderIndex,
        uint16_t u16ParameterCount);

/* *****************************************************************************
 * CSMON_eSetExternalRecorderUsage
 *
 * Inputs:
 *
 * Recorder ID:
 *      uint16_t u16RecorderIndex               - Recorder Index (Recorder0, ... RecorderN)
 *
 * Recorder Status:
 *      uint16_t* pu16PntrRecorderStatus        - Pointer to status of type CSMON_sExternalRecorderStatus
 *
 * Recorder Buffer:
 *      uint16_t* pu16DataFirstSampleAddress    - Pointer to Address In Recorder Buffer Of the First (Start) Data Sample (currenly pointed address in external buffer)
 *      uint32_t u32CircleBufferSampleCount     - Count Samples In The circular Buffer (circle buffer size in samples)
 *      uint32_t u32CircleBufferStartAddress    - Starting Address of The circular Buffer
 *
 * Recorder Trigger Time:
 *      CSMON_eMicroSec0BuffOvf1Mode_t eTriggerSubSecondMode    - Sub-second mode
 *      uint32_t* pu32TriggerMicrosecondsOrRollingTimerTicks    - Pointer to Trigger Sub-Second Value (value interpretation depends on Sub-second mode)
 *      uint_least8_t* pu8TriggerBCDSeconds                     - Pointer to Trigger Seconds (Binary Coded Decimal Value)
 *      uint_least8_t* pu8TriggerBCDMinutes                     - Pointer to Trigger Minutes (Binary Coded Decimal Value)
 *      uint_least8_t* pu8TriggerBCDHours                       - Pointer to Trigger Hours   (Binary Coded Decimal Value)
 *      uint_least8_t* pu8TriggerBCDWeekdays                    - Pointer to Trigger Weekdays(Binary Coded Decimal Value)
 *      uint_least8_t* pu8TriggerBCDDay                         - Pointer to Trigger Day     (Binary Coded Decimal Value)
 *      uint_least8_t* pu8TriggerBCDMonth                       - Pointer to Trigger Month   (Binary Coded Decimal Value)
 *      uint_least8_t* pu8TriggerBCDYear                        - Pointer to Trigger Year    (Binary Coded Decimal Value)
 *
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eSetExternalRecorderUsage (
        uint16_t u16RecorderIndex,
        uint16_t* pu16PntrRecorderStatus,
        uint16_t* pu16DataFirstSampleAddress,
        uint32_t u32CircleBufferSampleCount,
        uint32_t u32CircleBufferStartAddress,
        CSMON_eMicroSec0BuffOvf1Mode_t eTriggerSubSecondMode,
        uint32_t* pu32TriggerMicrosecondsOrRollingTimerTicks,
        uint_least8_t* pu8TriggerBCDSeconds,
        uint_least8_t* pu8TriggerBCDMinutes,
        uint_least8_t* pu8TriggerBCDHours,
        uint_least8_t* pu8TriggerBCDWeekdays,
        uint_least8_t* pu8TriggerBCDDay,
        uint_least8_t* pu8TriggerBCDMonth,
        uint_least8_t* pu8TriggerBCDYear
        );


/* *****************************************************************************
 * CSMON_eSetParameterInScopeAtPosition
 *
 * Input:
 *      uint16_t u16ScopeIndex       - Scope Index (Scope0, Scope1, ... ScopeN)
 *      uint16_t u16ParameterID      - Parameter ID or Index in Table
 *      uint16_t u16ParameterPos     - Position of the Parameter In The Scope Sequence
 *
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eSetParameterInScopeAtPosition (
        uint16_t u16ScopeIndex,
        uint16_t u16ParameterID,
        uint16_t u16ParameterPos);

/* *****************************************************************************
 * CSMON_eSetParameterCountInScope
 *
 * Input:
 *      uint16_t u16ScopeIndex    - Scope Index (Scope0, Scope1, ... ScopeN)
 *      uint16_t u16ParameterCount   - Count Parameters used In The Scope Sequence
 *
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eSetParameterCountInScope (
        uint16_t u16ScopeIndex,
        uint16_t u16ParameterCount);



/* *****************************************************************************
 * CSMON_vSetSetupRecorderParameterMask
 **************************************************************************** */
void CSMON_vSetSetupRecorderParameterMask (CSMON_eRecorderParameterMask_t eMask);

/* *****************************************************************************
 * CSMON_vSetStartRecorderParameterMask
 **************************************************************************** */
void CSMON_vSetStartRecorderParameterMask (CSMON_eRecorderParameterMask_t eMask);

/* *****************************************************************************
 * CSMON_vSetStopRecorderParameterMask
 **************************************************************************** */
void CSMON_vSetStopRecorderParameterMask (CSMON_eRecorderParameterMask_t eMask);

/* *****************************************************************************
 * CSMON_u32GetModbusMessageCounterRegisterRealAddress
 **************************************************************************** */
uint32_t CSMON_u32GetModbusMessageCounterRegisterRealAddress(void);


/* *****************************************************************************
 * CSMON_vRecorderParamSetupProcess
 **************************************************************************** */
void CSMON_vRecorderParamSetupProcess(void);

/* *****************************************************************************
 * CSMON_vRecorderParamStartStopProcess
 **************************************************************************** */
void CSMON_vRecorderParamStartStopProcess(void);


/* *****************************************************************************
 * CSMON_eSetFirmwareApplicationVersion
 *
 * Input:
 *      uint16_t u16Version       - Firmware Application Version
 *                                  High Byte Is Major Version
 *                                  Low Byte Is Minor Version
 *
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eSetFirmwareApplicationVersion (uint16_t u16Version);

/* *****************************************************************************
 * CSMON_eSetAutoServiceWatchdogInternalSlowCalculationsInMainLoop
 *
 * Input:
 *      bool bAutoServiceWatchDog
 *
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eSetAutoServiceWatchdogInternalSlowCalculationsInMainLoop (bool bAutoServiceWatchDog);


/* *****************************************************************************
 * CSMON_eSetInternalTimerPeriod
 *
 * Input:
 *      uint16_t microseconds
 *
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eSetInternalTimerPeriod (uint16_t microseconds);

/* *****************************************************************************
 * CSMON_eSetMaxTimeInISR
 *
 * Input:
 *      uint16_t microseconds
 *
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eSetMaxTimeInISR (uint16_t microseconds);

/* *****************************************************************************
 * CSMON_eSetMinGuaranteedTimeBetweenTwoISRs
 *
 * Input:
 *      uint16_t microseconds
 *
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eSetMinGuaranteedTimeBetweenTwoISRs (uint16_t microseconds);


/* *****************************************************************************
 * External Function Prototypes - need to be implemented externally
 **************************************************************************** */

/* *****************************************************************************
 * CSMON_vSetDateTime - external interface Set RTC Value
 **************************************************************************** */
extern void CSMON_vSetDateTime (
        uint_least8_t u8BCDSeconds,
        uint_least8_t u8BCDMinutes,
        uint_least8_t u8BCDHours,
        uint_least8_t u8BCDWeekdays,
        uint_least8_t u8BCDDay,
        uint_least8_t u8BCDMonth,
        uint_least8_t u8BCDYear);

/* *****************************************************************************
 * CSMON_vGetDateTime - external interface Get RTC Value
 **************************************************************************** */
extern void CSMON_vGetDateTime (
        uint_least8_t* pu8BCDSeconds,
        uint_least8_t* pu8BCDMinutes,
        uint_least8_t* pu8BCDHours,
        uint_least8_t* pu8BCDWeekdays,
        uint_least8_t* pu8BCDDay,
        uint_least8_t* pu8BCDMonth,
        uint_least8_t* pu8BCDYear);



#endif	/* CSMON_H */

#ifdef __cplusplus
}
#endif /* __cplusplus */
