/* *****************************************************************************
 * File:   csmon.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 10 01 23:05
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

#include "boardcfg.h"
#include "csmon.h"

#include "datetime.h"

#include "csmon_t.h"        /* inside included csmon_cfg.h */

#include "parameter.h"

#include "modbusreg.h"

#include "hmrecparam.h"
#include "hmscopeparam.h"

#include "tmr_drv.h"

#include "Hash.h"

#include "mclib.h"




/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */
#if CSMON_PARAMETER_LIST_READ_ONLY
#define MEMORY_MODIFIER __CONSTANT_MEMORY_MODIFIER__
#else
#define MEMORY_MODIFIER __EXTENDED_MEMORY_MODIFIER__
#endif

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* Consider how to make this controllable without two places edit */
typedef enum
{
#if USE_CONTROL
    /* 1000 */
    CSMON_PARAMETER_REGID_STACK1_RESERVED_REQUEST_STATUS              = MODBUSREG_REGID_STACK1_RESERVED_REQUEST_STATUS,
    CSMON_PARAMETER_REGID_STACK1_CONTROL_STATE                        = MODBUSREG_REGID_STACK1_CONTROL_STATE          ,
    CSMON_PARAMETER_REGID_STACK1_CONTROL_ERROR                        = MODBUSREG_REGID_STACK1_CONTROL_ERROR          ,
    CSMON_PARAMETER_REGID_STACK1_ERROR_STATE                          = MODBUSREG_REGID_STACK1_ERROR_STATE            ,
    CSMON_PARAMETER_REGID_STACK1_CLEAR_FAULTS                         = MODBUSREG_REGID_STACK1_CLEAR_FAULTS           ,
    CSMON_PARAMETER_REGID_STACK1_ERROR_STATUS                         = MODBUSREG_REGID_STACK1_ERROR_STATUS           ,
    CSMON_PARAMETER_REGID_STACK1_SPEED_REQUEST                        = MODBUSREG_REGID_STACK1_SPEED_REQUEST          ,
    CSMON_PARAMETER_REGID_STACK1_VOLTAGE_DCLINK_REQUEST               = MODBUSREG_REGID_STACK1_VOLTAGE_DCLINK_REQUEST ,
    CSMON_PARAMETER_REGID_STACK1_CURRENT_REQUEST                      = MODBUSREG_REGID_STACK1_CURRENT_REQUEST        ,
    CSMON_PARAMETER_REGID_STACK1_RUN_REQUEST_STATUS                   = MODBUSREG_REGID_STACK1_RUN_REQUEST_STATUS     ,

    /* 1010 */
    CSMON_PARAMETER_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEA            = MODBUSREG_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEA  ,
    CSMON_PARAMETER_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEB            = MODBUSREG_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEB  ,
    CSMON_PARAMETER_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEC            = MODBUSREG_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEC  ,
    CSMON_PARAMETER_REGID_STACK1_VOLTAGE_DCLINK_RMS                   = MODBUSREG_REGID_STACK1_VOLTAGE_DCLINK_RMS  ,
    CSMON_PARAMETER_REGID_STACK1_CURRENT_MODULE_RMS_PHASEA            = MODBUSREG_REGID_STACK1_CURRENT_MODULE_RMS_PHASEA  ,
    CSMON_PARAMETER_REGID_STACK1_CURRENT_MODULE_RMS_PHASEB            = MODBUSREG_REGID_STACK1_CURRENT_MODULE_RMS_PHASEB  ,
    CSMON_PARAMETER_REGID_STACK1_CURRENT_MODULE_RMS_PHASEC            = MODBUSREG_REGID_STACK1_CURRENT_MODULE_RMS_PHASEC  ,
    CSMON_PARAMETER_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEA   = MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEA  ,
    CSMON_PARAMETER_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEB   = MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEB  ,
    CSMON_PARAMETER_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEC   = MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEC  ,

    /* 1020 */
    CSMON_PARAMETER_REGID_STACK1_CURRENT_SENSOR_PHASEA                = MODBUSREG_REGID_STACK1_CURRENT_SENSOR_PHASEA  ,
    CSMON_PARAMETER_REGID_STACK1_CURRENT_SENSOR_PHASEB                = MODBUSREG_REGID_STACK1_CURRENT_SENSOR_PHASEB  ,
    CSMON_PARAMETER_REGID_STACK1_CURRENT_SENSOR_PHASEC                = MODBUSREG_REGID_STACK1_CURRENT_SENSOR_PHASEC  ,
    CSMON_PARAMETER_REGID_STACK1_VOLTAGE_DCLINK_FDBACK                = MODBUSREG_REGID_STACK1_VOLTAGE_DCLINK_FEEDBACK  ,
    CSMON_PARAMETER_REGID_STACK1_CURRENT_MODULE_PHASEA                = MODBUSREG_REGID_STACK1_CURRENT_MODULE_PHASEA  ,
    CSMON_PARAMETER_REGID_STACK1_CURRENT_MODULE_PHASEB                = MODBUSREG_REGID_STACK1_CURRENT_MODULE_PHASEB  ,
    CSMON_PARAMETER_REGID_STACK1_CURRENT_MODULE_PHASEC                = MODBUSREG_REGID_STACK1_CURRENT_MODULE_PHASEC  ,
    CSMON_PARAMETER_REGID_STACK1_TEMPERATURE_MODULE_PHASEA            = MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_PHASEA  ,
    CSMON_PARAMETER_REGID_STACK1_TEMPERATURE_MODULE_PHASEB            = MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_PHASEB  ,
    CSMON_PARAMETER_REGID_STACK1_TEMPERATURE_MODULE_PHASEC            = MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_PHASEC  ,

    /* Test Mix + MODBUS_RECEIVED_MESSAGES_COUNTER */
    /* 3000 */
    CSMON_PARAMETER_COILID_TEST                                       = MODBUSREG_COILID_CONTROL_TEST,
    /* 4000 */
    CSMON_PARAMETER_INCOILID_DUMMY                                    = MODBUSREG_INCOILID_CONTROL_DUMMY,
    /* 2000 */
    CSMON_PARAMETER_INREGID_DUMMY                                     = MODBUSREG_INREGID_CONTROL_DUMMY,

    /* 1040 */
    CSMON_PARAMETER_REGID_STACK1_CFBK_MODULE_NOMINAL_CURRENT          = MODBUSREG_REGID_STACK1_CFBK_MODULE_NOMINAL_CURRENT,

#endif

    /* 65534 */
    CSMON_PARAMETER_REGID_MODBUS_RECEIVED_MESSAGES_COUNTER            = MODBUSREG_REGID_MODBUS_RECEIVED_MESSAGES_COUNTER,

    /* Below Start From Zero in order to make indexes to the Parameters in Table for the Recorder Demo Test */

#if USE_CONTROL
    /* 1000 */
    CSMON_PARAMETER_POSITION_REGID_STACK1_RESERVED_REQUEST_STATUS = 0          ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_CONTROL_STATE                        ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_CONTROL_ERROR                        ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_ERROR_STATE                          ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_CLEAR_FAULTS                         ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_ERROR_STATUS                         ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_SPEED_REQUEST                        ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_VOLTAGE_DCLINK_REQUEST               ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_CURRENT_REQUEST                      ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_RUN_REQUEST_STATUS                   ,

    /* 1010 */
    CSMON_PARAMETER_POSITION_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEA            ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEB            ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEC            ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_VOLTAGE_DCLINK_RMS                   ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_CURRENT_MODULE_RMS_PHASEA            ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_CURRENT_MODULE_RMS_PHASEB            ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_CURRENT_MODULE_RMS_PHASEC            ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEA   ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEB   ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEC   ,

    /* 1020 */
    CSMON_PARAMETER_POSITION_REGID_STACK1_CURRENT_SENSOR_PHASEA                ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_CURRENT_SENSOR_PHASEB                ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_CURRENT_SENSOR_PHASEC                ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_VOLTAGE_DCLINK_FDBACK                ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_CURRENT_MODULE_PHASEA                ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_CURRENT_MODULE_PHASEB                ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_CURRENT_MODULE_PHASEC                ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_TEMPERATURE_MODULE_PHASEA            ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_TEMPERATURE_MODULE_PHASEB            ,
    CSMON_PARAMETER_POSITION_REGID_STACK1_TEMPERATURE_MODULE_PHASEC            ,

    /* Test Mix + MODBUS_RECEIVED_MESSAGES_COUNTER */
    /* 3000 */
    CSMON_PARAMETER_POSITION_COILID_TEST                                  ,
    /* 4000 */
    CSMON_PARAMETER_POSITION_INCOILID_DUMMY                               ,
    /* 2000 */
    CSMON_PARAMETER_POSITION_INREGID_DUMMY                                ,

    /* 1040 */
    CSMON_PARAMETER_POSITION_REGID_STACK1_CFBK_MODULE_NOMINAL_CURRENT     ,
            
    /* 65534 */
    CSMON_PARAMETER_POSITION_REGID_MODBUS_RECEIVED_MESSAGES_COUNTER       ,

#else
    /* 65534 */
    CSMON_PARAMETER_POSITION_REGID_MODBUS_RECEIVED_MESSAGES_COUNTER = 0       ,

            
#endif

    CSMON_PARAMETER_POSITION_END


}CSMON_eFileParameterListIndex;



/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
typedef union
{
    uint64_t u64Register;
     int64_t s64Register;
    uint32_t u32Register;
     int32_t s32Register;
    uint16_t u16Register;
     int16_t s16Register;
    uint_least8_t u8Register;
     int_least8_t s8Register;
} uAnyType64_t;

typedef struct
{
    int16_t a;
    int16_t b;
} S;

typedef union
{
    int32_t i32;
    S s;
} U;


U arr[2] =
{
    [0].s.a = 0,[0].s.b = 1,
    [1].s.a = 2,[1].s.b = 3,
};

U arr2 =
{
    0, .s.b = 3
};

typedef struct
{
#if 0
    char ID;    /* Hash Algorithm ID */
#endif
    char YY;
    char MM;
    char DD;
    char hh;
    char mm;
    char ss;
    char sub3;
    char sub2;
    char sub1;
    char sub0;
#if 0
    char id3;
    char id2;
    char id1;
    char id0;
#endif
}sHashKey_t;



/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */
//#define bitsizeof(x)    (sizeof(x) << 3)
#define bitsizeof(x)    ((x) << 3)  /* converts Bytes to bits */


#if CSMON_PACKED_16_BIT_PARAMETER_DEFINITIONS

#define PARAM_INIT_WITH_CS(V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11) \
{ \
 {{.u16ValueAccessKeyStoradeID = V1, \
   .u8ValueAccessKeyStartBitIndex = V2, \
   .u6Reserved = V3, \
   .bArray = V4, \
   .bBitField = V5, \
 }}, \
    V6, /* uint16_t ModbusAddress */ \
    V8, /* CSMON_eAccessLevels eWriteAccessLevelMin */ \
    V7, /* CSMON_eAccessLevels eReadAccessLevelMin */ \
    V10,/* HMMODBUS_eCommands eDefaultWriteFunctionCode */ \
    V9, /* HMMODBUS_eCommands eDefaultReadFunctionCode */ \
    ((0 - ((V1 >> 8) + (V1 & 0xFF) + V2 + (V3 + (V4 << 6) + (V5 << 7)) + (V6 >> 8) + (V6 & 0xFF) + V7 + V8 + V9 + V10 + V11)) & 0x00FF),/* uint8_t u8PaddingByte/CheckSum */ \
    V11/* uint8_t u8BitsCount */ \
}

#else

#define PARAM_INIT_WITH_CS(V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11) \
{ \
 {{.u16ValueAccessKeyStoradeID = V1, \
   .u8ValueAccessKeyStartBitIndex = V2, \
   .u6Reserved = V3, \
   .bArray = V4, \
   .bBitField = V5, \
 }}, \
    V6, /* uint16_t ModbusAddress */ \
    V7, /* CSMON_eAccessLevels eReadAccessLevelMin */ \
    V8, /* CSMON_eAccessLevels eWriteAccessLevelMin */ \
    V9, /* HMMODBUS_eCommands eDefaultReadFunctionCode */ \
    V10,/* HMMODBUS_eCommands eDefaultWriteFunctionCode */ \
    V11,/* uint8_t u8BitsCount */ \
    ((0 - ((V1 >> 8) + (V1 & 0xFF) + V2 + (V3 + (V4 << 6) + (V5 << 7)) + (V6 >> 8) + (V6 & 0xFF) + V7 + V8 + V9 + V10 + V11)) & 0x00FF)/* uint8_t u8PaddingByte/CheckSum */ \
}

#endif

#define PARAM_LIST_CALC_CS(V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11) \
        ((0 - ((V1 >> 8) + (V1 & 0xFF) + V2 + (V3 + (V4 << 6) + (V5 << 7)) + (V6 >> 8) + (V6 & 0xFF) + V7 + V8 + V9 + V10 + V11)) & 0x00FF)

























/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */

sHashKey_t sHashKey =
{
#if 0
 0x00,     /* char ID;          */
#endif
 0x01,     /* char YY;          */
 0x01,     /* char MM;          */
 0x01,     /* char DD;          */
 0x00,     /* char hh;          */
 0x00,     /* char mm;          */
 0x00,     /* char ss;          */
 0x00,     /* char sub3;        */
 0x00,     /* char sub2;        */
 0x00,     /* char sub1;        */
 0x00,     /* char sub0;        */
#if 0
 0x00,     /* char id3;         */
 0x00,     /* char id2;         */
 0x00,     /* char id1;         */
 0x00,     /* char id0;         */
#endif
};

/* Date and Time */

CSMON_sDateTime_t CSMON_sDateTime =
{
     0x00,     /* uint_least8_t u8HashAlgorithmID; */
     0x01,     /* uint_least8_t u8Year;     */
     0x01,     /* uint_least8_t u8Month;    */
     0x01,     /* uint_least8_t u8Day;      */
     0x01,     /* uint_least8_t u8WeekDays; */
     0x00,     /* uint_least8_t u8Hours;    */
     0x00,     /* uint_least8_t u8Minutes;  */
     0x00,     /* uint_least8_t u8Seconds;  */
     0,        /* uint32_t u32Microseconds; */
     0,        /* uint32_t u32ClientID;     */
};

uint32_t u32FreeRunTimerSysClkMHz = (uint32_t)(HMDRV_FREE_RUN_TIMER_FREQ_MHZ);
uint32_t u32FreeRunTimerSysClkDiv = 1;  /* 1 means 1:1; 7 means 1:7 */



/* Scope Configuration */
#if HMDRV_SCOPE_PARAM_HANDLE_COUNT > 0
HMSCOPEPRM_sConfig_t  sScopeConfigParam[HMDRV_SCOPE_PARAM_HANDLE_COUNT];
#endif



/* Recorder Configuration */
#if HMDRV_RECORDER_PARAM_HANDLE_COUNT > 0
HMRECPRM_sConfig_t  sRecConfigParam[HMDRV_RECORDER_PARAM_HANDLE_COUNT];
#endif

/* Do Not Initialize Here - use CTRL_u8MaskSetupRecorderRunModeRequest or TESTPROC_u8MaskSetupRecorderRunModeEnter instead */
/* Meaning: 0x01 - recorder0; 0x02 - recorder1; 0x04 - recorder2 */
HMDRV_SIZE8 CSMON_u16MaskSetupRecorderParam = 0;
HMDRV_SIZE8 CSMON_u16MaskStartRecorderParam = 0;
HMDRV_SIZE8 CSMON_u16MaskStopRecorderParam = 0;


HMRECPRM_eResponseCode_t eResponseCodeStartRecParam0 = HMRECPRM_CODE_OK;
HMRECPRM_eResponseCode_t eResponseCodeStartRecParam1 = HMRECPRM_CODE_OK;
HMRECPRM_eResponseCode_t eResponseCodeStartRecParam2 = HMRECPRM_CODE_OK;

int CSMON_nHashOutputLength[CSMON_ACCESS_COUNT];

bool CSMON_bGetDateTimeAutorization = false;
bool CSMON_bPasswordGenerate[CSMON_ACCESS_COUNT] = {false, false, false, false, false};

bool CSMON_bSetDateTimeRequest = false;
bool CSMON_bGetDateTimeRequest = false;

uint32_t CSMON_u32SCIErrorPPM = 0;
uint16_t CSMON_u16SCIErrorPercent = 0;

bool CSMON_abFlagProcessPassed[CSMON_ID_PROCESS_COUNT];


bool bServiceWatchdog = false;

uint32_t CSMON_u32FreeRunTimerCheckSumListBegin;
uint32_t CSMON_u32FreeRunTimerCheckSumListEnd;
uint32_t CSMON_u32FreeRunTimerCheckSumListMax;

uint32_t CSMON_u32FreeRunTimerCheckSumNameBegin;
uint32_t CSMON_u32FreeRunTimerCheckSumNameEnd;
uint32_t CSMON_u32FreeRunTimerCheckSumNameMax;

uint32_t CSMON_u32FreeRunTimerCheckSumNumbBegin;
uint32_t CSMON_u32FreeRunTimerCheckSumNumbEnd;
uint32_t CSMON_u32FreeRunTimerCheckSumNumbMax;

uint32_t CSMON_u32FreeRunTimerCheckSumStrgBegin;
uint32_t CSMON_u32FreeRunTimerCheckSumStrgEnd;
uint32_t CSMON_u32FreeRunTimerCheckSumStrgMax;


uint32_t CSMON_u32FreeRunTimerAccessChangeBegin;
uint32_t CSMON_u32FreeRunTimerAccessChangeEnd;
uint32_t CSMON_u32FreeRunTimerAccessChangeMax;

uint32_t CSMON_u32FreeRunTimerPasswordHashBegin;
uint32_t CSMON_u32FreeRunTimerPasswordHashEnd;
uint32_t CSMON_u32FreeRunTimerPasswordHashMax;

uint32_t CSMON_u32FreeRunTimerProcessBegin;
uint32_t CSMON_u32FreeRunTimerProcessNow;
uint32_t CSMON_u32FreeRunTimerProcessEnd;
uint32_t CSMON_u32FreeRunTimerProcessMax;
bool CSMON_bResetMaxMeasure = 1;




/* Get watchdog period */
bool CSMON_bWatchdogCalcNeeded = 1;
uint16_t CSMON_u16WatchdogPrescaler = 1;

uint32_t CSMON_u32WatchdogPeriod_usec;
uint32_t CSMON_u32WatchdogPeriod_Ticks;

uint16_t CSMON_u16MaxTimeInISR_usec = TIMER_MAIN_USEC;
uint32_t CSMON_u32MaxTimeInISR_Ticks;

uint32_t CSMON_u32MaxTimeProcess_Ticks;

uint32_t CSMON_u32MaxTimeTillWdtClearISRs;


uint16_t CSMON_u16MinTimeProcessExit_usec = 10;  /* min time that will be always available to exit the process (between 2 ISR enterings) */
uint32_t CSMON_u32MinTimeProcessExit_Ticks;

uint16_t CSMON_u16MaxTimeTillWdtClear_usec = 200;  /* max time that is needed to reach to clear Watchdog */
uint32_t CSMON_u32MaxTimeTillWdtClear_Ticks;

uint16_t CSMON_u16MinTimeProcessAllowedNeeded_usec = 1000;  /*  */
uint32_t CSMON_u32MinTimeProcessAllowedNeeded_Ticks;

uint32_t CSMON_u32MinTimeProcessAllowedNeeded_Ticks;
 int32_t CSMON_s32MaxTimeProcessAllowedCalc_Ticks;
uint32_t CSMON_u32MaxTimeProcessAllowed_Ticks;



uint16_t u16SkipProcessCounter = 0;
uint16_t u16SkipProcessTag;  /* Tag Of The interruption */
bool bSkipProcessFlag = 0;   /* Flag interruption for code after to Skip, not to run */












bool CSMON_bUseAccessLevelForHoldingRegistersAndCoils = 1;
bool CSMON_bUseAccessLevelForInputRegistersAndCoils = 1;


uint16_t CSMON_au16FileAutorization_RecordOffsets[CSMON_FILE_AUTORIZATION_RECORD_COUNT] = 
{                                                   /* offset of first element is 0 and is not specified here */
    CSMON_FILE_AUTORIZATION_ACCESS_LEVEL_LENGTH,    /* first one is Offset of Element 1 (size of element 0) */
    CSMON_FILE_AUTORIZATION_ACCESS_LEVEL_LENGTH + CSMON_FILE_AUTORIZATION_PASSWORD_LENGTH_BYTES_IN_MEMORY,    /*  */
    CSMON_FILE_AUTORIZATION_ACCESS_LEVEL_LENGTH + CSMON_FILE_AUTORIZATION_PASSWORD_LENGTH_BYTES_IN_MEMORY + CSMON_FILE_AUTORIZATION_ACCESS_LEVEL_LENGTH, /* last one is Size of whole file (offset of the element after the last element) */
};

uint32_t CSMON_au32FileAutorization_FunctionAfterWriteFileRecord[CSMON_FILE_AUTORIZATION_RECORD_COUNT] = 
{                                                  
    (uint32_t)NULL, /* Requested Access */       
    (uint32_t)NULL, /* Password */
    (uint32_t)NULL, /* Accepted Access */
};


bool CSMON_abFileAutorization_RecordStringData[CSMON_FILE_AUTORIZATION_RECORD_COUNT] = 
{
    HMDRV_FALSE,    /* Requested Access */
    HMDRV_TRUE,     /* Password */
    HMDRV_FALSE,    /* Accepted Access */
};

bool CSMON_abFileAutorization_RecordReadOnly[CSMON_FILE_AUTORIZATION_RECORD_COUNT] = 
{
    HMDRV_FALSE,    /* Requested Access */
    HMDRV_FALSE,    /* Password */
    HMDRV_TRUE,     /* Accepted Access */
};

const CSMON_sAccessLevelPasswords_t       CSMON_asAccessLevelPasswords[CSMON_ACCESS_COUNT] = 
{
    {"0"},
    {"01"},
    {"012"},
    {"0123"},
    {"01234"},
};

__EXTENDED_MEMORY_MODIFIER__ CSMON_sFileAutorization_t __EXTENDED_SPACE_ATTRIBUTE__((space(eds)))          CSMON_sFileAutorization =
{
    CSMON_ACCESS_DENIED,        /* Requested Access */
    "0000\0",                   /* Password */
    CSMON_ACCESS_INIT,          /* Accepted Access */
};

#if CSMON_FILE_AUTORIZATION_WITH_HASH
__EXTENDED_MEMORY_MODIFIER__ CSMON_sFileAutorization_t __EXTENDED_SPACE_ATTRIBUTE__((space(eds)))          CSMON_sFileAutorizationHashOut[CSMON_ACCESS_COUNT] =
{
 {
    CSMON_ACCESS_DENIED,        /* Requested Access */
    "0000\0",                   /* Password */
    CSMON_ACCESS_INIT,          /* Accepted Access */
 },
 {
    CSMON_ACCESS_DENIED,        /* Requested Access */
    "0000\0",                   /* Password */
    CSMON_ACCESS_INIT,          /* Accepted Access */
 },
 {
    CSMON_ACCESS_DENIED,        /* Requested Access */
    "0000\0",                   /* Password */
    CSMON_ACCESS_INIT,          /* Accepted Access */
 },
 {
    CSMON_ACCESS_DENIED,        /* Requested Access */
    "0000\0",                   /* Password */
    CSMON_ACCESS_INIT,          /* Accepted Access */
 },
 {
    CSMON_ACCESS_DENIED,        /* Requested Access */
    "0000\0",                   /* Password */
    CSMON_ACCESS_INIT,          /* Accepted Access */
 },
};
#endif

uint16_t CSMON_u16CurrentAccessLevelPrev = CSMON_ACCESS_DENIED;

CSMON_sFileProtocolVersion_t        CSMON_sFileProtocolVersion =
{
    CSMON_PROTOCOL_VERSION,                 //uint16_t u16ProtocolVersion;
    CSMON_LIBRARY_VERSION,                  //uint16_t u16FirmwareCommunicationVersion;
    0,                                      //uint16_t u16FirmwareApplicationVersion;
};

CSMON_sFileParameterCount_t         CSMON_sFileParameterCount = 
{
    0,
    HMDRV_RECORDER_PARAM_HANDLE_COUNT,                              //uint16_t u16RecorderCount;
    HMDRV_SCOPE_PARAM_HANDLE_COUNT,                                 //uint16_t u16ScopeCount;
    0,                                                              //uint16_t Padding;
    BOARDCFG_RW_NON_VOLATILE_MEMORY_SIZE_EEPROM_KB,                 //uint32_t u32RWNVMemorySize_kB (EEPROM);
};

CSMON_sFileParameterAccessList_t    CSMON_sFileParameterAccessList = 
{
    { 0,}
};

CSMON_sFileParameterDefinitionsFromAccessLevel_t CSMON_asFileParameterDefinitionsFromAccessLevel[CSMON_FILE_PARAMETER_COUNT_MAX] =
{
    
};

uint32_t CSMON_au32FileParameterDefinitionsFileRecordElementsRealAddressTable[CSMON_sFileParameterDefinitions_t_ElementsPartsCount] = 
{
    
};

uint32_t CSMON_au32FileParameterDefinitionsFileRecordElementsSizeTable[CSMON_sFileParameterDefinitions_t_ElementsPartsCount] = 
{
    
};

uint16_t CSMON_au32FileParameterDefinitionsFileRecordElementsRecordToRecordOffsetSizeBytesTable[CSMON_sFileParameterDefinitions_t_ElementsPartsCount] = 
{
    
};

uint8_t CSMON_au8FileParameterCountElementsSizeTable[CSMON_sFileParameterCount_t_ElementsCount] =
{
    2, /* uint16_t u16ParameterCount;       */
    2, /* uint16_t u16RecorderCount;        */
    2, /* uint16_t u16ScopeCount;           */
    2, /* uint16_t u16Padding;           */
    4, /* uint32_t u32RWNVMemorySize_kB;    */
    4, /* uint32_t u32ParameterListCheckSum; */
    4, /* uint32_t u32ParameterNumbCheckSum; */
    4, /* uint32_t u32ParameterStringCheckSum; */
    4, /* uint32_t u32ParameterAuxCheckSum; */
    4, /* uint32_t u32ParameterShortNameCheckSum; */
};

uint8_t CSMON_au8FileRTCElementsSizeTable[CSMON_sDateTime_t_ElementsCount] =
{
    1, /* uint_least8_t u8Reserved; */
    1, /* uint_least8_t u8Year;     */
    1, /* uint_least8_t u8Month;    */
    1, /* uint_least8_t u8Day;      */
    1, /* uint_least8_t u8WeekDays; */
    1, /* uint_least8_t u8Hours;    */
    1, /* uint_least8_t u8Minutes;  */
    1, /* uint_least8_t u8Seconds;  */
    4, /* uint32_t u32Microseconds; */
    4, /* uint32_t u32Reserved;     */
};

#if 0
uint8_t CSMON_au8FileParameterNumericElementsSizeTable[CSMON_uFileParameterNumeric_t_ElementsCount] =
{
    4, /* HMDRV_U32 u32VirtAddress; */
    4, /* HMDRV_U32 u32DataType;    */
    8, /* HMDRV_U64 u64Default;     */
    8, /* HMDRV_U64 u64Minimum;     */
    8, /* HMDRV_U64 u64Maximum;     */
    8, /* HMDRV_S64 s64Multply;     */
    8, /* HMDRV_S64 s64Offset;      */
};
#else
uint8_t CSMON_au8FileParameterNumericElementsSizeTable[CSMON_uFileParameterNumeric_t_ElementsCount] =
{
     /* structure CSMON_sFileParameterNumericStructure_t */
    2, /* CSMON_uFileParameterValueAccessKey_t.u16ValueAccessKeyStoradeID),*/
    2, /* CSMON_uFileParameterValueAccessKey_t.u8ValueAccessKeyFlags + u8ValueAccessKeyStartBitIndex),*/
    2, /* CSMON_eDataType_t eDataType */
    2, /* CSMON_eVisualType_t eVisualType */
    8, /* HMDRV_U64 u64Default;     */
    8, /* HMDRV_U64 u64Minimum;     */
    8, /* HMDRV_U64 u64Maximum;     */
    8, /* HMDRV_S64 s64Multply;     */
    8, /* HMDRV_S64 s64Offset;      */
};
#endif



uint8_t CSMON_au8FileRecorderStatusElementsSizeTable[HMRECPRM_sStatus_t_ElementsCount] =
{
    4,  /* HMRECPRM_sTimeSyncModeYYMMDD_t. Flags & b2TimeSyncSampleMode : u8Year : u8Month : u8Day */
    4,  /* HMRECPRM_sTimeSyncWWHHMMSS_t    u8WeekDays: u8Hour : u8Minute : u8Second */
    4,  /* u32MicroSec0BuffOvf1 */
    4,  /* u32AvailableSampleCount */
};

uint8_t CSMON_au8FileScopeStatusElementsSizeTable[CSMON_sFileScopeStatus_t_ElementsCount] =
{
    2, /* uint16_t  Dummy */
};



uint8_t CSMON_au8FileRecorderConfigElementsSizeTable[HMRECPRM_sConfig_t_ElementsCount] =
{

    4,            /* HMRECPRM_sConfig_t.HMRECPRM_sSampleFormat_t.nSampleRateHz */
    4,            /* HMRECPRM_sConfig_t.HMRECPRM_sSampleFormat_t.nSampleCountMax */
    4,            /* HMRECPRM_sConfig_t.HMRECPRM_sSampleFormat_t.nPreTriggerSampleCount */
    2,            /* HMRECPRM_sConfig_t.HMRECPRM_sSampleFormat_t.sAlignments */
    2,            /* HMRECPRM_sConfig_t.HMRECPRM_sSampleFormat_t.nCountVariables */

    2,2,          /* HMRECPRM_sConfig_t.HMRECPRM_sSampleFormat_t.nVariableIDRecord[00] */
    2,2,          /* HMRECPRM_sConfig_t.HMRECPRM_sSampleFormat_t.nVariableIDRecord[01] */
    2,2,          /* HMRECPRM_sConfig_t.HMRECPRM_sSampleFormat_t.nVariableIDRecord[02] */
    2,2,          /* HMRECPRM_sConfig_t.HMRECPRM_sSampleFormat_t.nVariableIDRecord[03] */
    2,2,          /* HMRECPRM_sConfig_t.HMRECPRM_sSampleFormat_t.nVariableIDRecord[04] */
    2,2,          /* HMRECPRM_sConfig_t.HMRECPRM_sSampleFormat_t.nVariableIDRecord[05] */
    2,2,          /* HMRECPRM_sConfig_t.HMRECPRM_sSampleFormat_t.nVariableIDRecord[06] */
    2,2,          /* HMRECPRM_sConfig_t.HMRECPRM_sSampleFormat_t.nVariableIDRecord[07] */
    2,2,          /* HMRECPRM_sConfig_t.HMRECPRM_sSampleFormat_t.nVariableIDRecord[08] */
    2,2,          /* HMRECPRM_sConfig_t.HMRECPRM_sSampleFormat_t.nVariableIDRecord[09] */

    2,2,          /* HMRECPRM_sConfig_t.HMRECPRM_sSampleFormat_t.nVariableIDRecord[10] */
    2,2,          /* HMRECPRM_sConfig_t.HMRECPRM_sSampleFormat_t.nVariableIDRecord[11] */
    2,2,          /* HMRECPRM_sConfig_t.HMRECPRM_sSampleFormat_t.nVariableIDRecord[12] */
    2,2,          /* HMRECPRM_sConfig_t.HMRECPRM_sSampleFormat_t.nVariableIDRecord[13] */
    2,2,          /* HMRECPRM_sConfig_t.HMRECPRM_sSampleFormat_t.nVariableIDRecord[14] */
    2,2,          /* HMRECPRM_sConfig_t.HMRECPRM_sSampleFormat_t.nVariableIDRecord[15] */
    2,2,          /* HMRECPRM_sConfig_t.HMRECPRM_sSampleFormat_t.nVariableIDRecord[16] */
    2,2,          /* HMRECPRM_sConfig_t.HMRECPRM_sSampleFormat_t.nVariableIDRecord[17] */
    2,2,          /* HMRECPRM_sConfig_t.HMRECPRM_sSampleFormat_t.nVariableIDRecord[18] */
    2,2,          /* HMRECPRM_sConfig_t.HMRECPRM_sSampleFormat_t.nVariableIDRecord[19] */

};


uint8_t CSMON_au8FileScopeConfigurationElementsSizeTable[CSMON_sFileScopeConfiguration_t_ElementsCount] = 
{
    2,            /*HMSCOPEPRM_sConfig.u16ScopeVarCount */
    2,            /*HMSCOPEPRM_sConfig.u16Reserved */
    2,2,          /*HMSCOPEPRM_sConfig.pScopeVarAddr[00] */
    2,2,          /*HMSCOPEPRM_sConfig.pScopeVarAddr[01] */
    2,2,          /*HMSCOPEPRM_sConfig.pScopeVarAddr[02] */
    2,2,          /*HMSCOPEPRM_sConfig.pScopeVarAddr[03] */
    2,2,          /*HMSCOPEPRM_sConfig.pScopeVarAddr[04] */
    2,2,          /*HMSCOPEPRM_sConfig.pScopeVarAddr[05] */
    2,2,          /*HMSCOPEPRM_sConfig.pScopeVarAddr[06] */
    2,2,          /*HMSCOPEPRM_sConfig.pScopeVarAddr[08] */
    2,2,          /*HMSCOPEPRM_sConfig.pScopeVarAddr[09] */
    2,2,          /*HMSCOPEPRM_sConfig.pScopeVarAddr[10] */
    2,2,          /*HMSCOPEPRM_sConfig.pScopeVarAddr[11] */
    2,2,          /*HMSCOPEPRM_sConfig.pScopeVarAddr[12] */
    2,2,          /*HMSCOPEPRM_sConfig.pScopeVarAddr[13] */
    2,2,          /*HMSCOPEPRM_sConfig.pScopeVarAddr[14] */
    2,2,          /*HMSCOPEPRM_sConfig.pScopeVarAddr[15] */
    2,2,          /*HMSCOPEPRM_sConfig.pScopeVarAddr[16] */
    2,2,          /*HMSCOPEPRM_sConfig.pScopeVarAddr[17] */
    2,2,          /*HMSCOPEPRM_sConfig.pScopeVarAddr[18] */
    2,2,          /*HMSCOPEPRM_sConfig.pScopeVarAddr[19] */
   
};

#if CSMON_PACKED_16_BIT_PARAMETER_DEFINITIONS

#if 0
uint8_t CSMON_au8FileParameterListElementsSizeTable[CSMON_sFileParameterDefinitions_t_ElementsCount] =
{
    2,             /*CSMON_sFileParameterDefinitions_t.u16ValueAccessKeyStoradeID),*/
    2,             /*CSMON_sFileParameterDefinitions_t.u8ValueAccessKeyFlags + u8ValueAccessKeyStartBitIndex),*/
    2,             /*CSMON_sFileParameterDefinitions_t.ModbusAddress),*/
    2,             /*CSMON_sFileParameterDefinitions_t.eReadAccessLevelMin + eWriteAccessLevelMin),*/
    2,             /*CSMON_sFileParameterDefinitions_t.eDefaultReadFunctionCode + eDefaultWriteFunctionCode),*/
    2,             /*CSMON_sFileParameterDefinitions_t.u8BitsCount + u8PaddingByte),*/
};
#endif
uint8_t CSMON_au8FileParameterListElementsSizeTablePart1[CSMON_sFileParameterDefinitions_t_ElementsFirst] =
{
    2,            /*CSMON_sFileParameterDefinitions_t.u16ValueAccessKeyStoradeID),*/
    2,            /*CSMON_sFileParameterDefinitions_t.u8ValueAccessKeyFlags + u8ValueAccessKeyStartBitIndex),*/
    2,            /*CSMON_sFileParameterDefinitions_t.ModbusAddress),*/
    2,            /*CSMON_sFileParameterDefinitions_t.eReadAccessLevelMin + eWriteAccessLevelMin),*/
};
uint8_t CSMON_au8FileParameterListElementsSizeTablePart2[CSMON_sFileParameterDefinitions_t_ElementsSecond] =
{
    2,             /*CSMON_sFileParameterDefinitions_t.eDefaultReadFunctionCode + eDefaultWriteFunctionCode),*/
};
uint8_t CSMON_au8FileParameterListElementsSizeTablePart3[CSMON_sFileParameterDefinitions_t_ElementsThird] =
{
    2,             /*CSMON_sFileParameterDefinitions_t.u8BitsCount + u8PaddingByte),*/
};

#else

#if 0
uint8_t CSMON_au8FileParameterListElementsSizeTable[CSMON_sFileParameterDefinitions_t_ElementsCount] =
{
    2,             /*CSMON_sFileParameterDefinitions_t.u16ValueAccessKeyStoradeID),*/
    2,             /*CSMON_sFileParameterDefinitions_t.u8ValueAccessKeyFlags + u8ValueAccessKeyStartBitIndex),*/
    2,             /*CSMON_sFileParameterDefinitions_t.ModbusAddress),*/
    1,             /*CSMON_sFileParameterDefinitions_t.eReadAccessLevelMin),*/
    1,             /*CSMON_sFileParameterDefinitions_t.eWriteAccessLevelMin),*/
    1,             /*CSMON_sFileParameterDefinitions_t.eDefaultReadFunctionCode),*/
    1,             /*CSMON_sFileParameterDefinitions_t.eDefaultWriteFunctionCode)*/
    1,             /*CSMON_sFileParameterDefinitions_t.u8BitsCount),*/
    1,             /*CSMON_sFileParameterDefinitions_t.u8PaddingByte),*/
};
#endif
uint8_t CSMON_au8FileParameterListElementsSizeTablePart1[CSMON_sFileParameterDefinitions_t_ElementsFirst] = 
{
    2,            /*CSMON_sFileParameterDefinitions_t.u16ValueAccessKeyStoradeID),*/
    2,            /*CSMON_sFileParameterDefinitions_t.u8ValueAccessKeyFlags + u8ValueAccessKeyStartBitIndex),*/
    2,            /*CSMON_sFileParameterDefinitions_t.ModbusAddress),*/ 
    1,            /*CSMON_sFileParameterDefinitions_t.eReadAccessLevelMin),*/
    1,            /*CSMON_sFileParameterDefinitions_t.eWriteAccessLevelMin),*/
};
uint8_t CSMON_au8FileParameterListElementsSizeTablePart2[CSMON_sFileParameterDefinitions_t_ElementsSecond] = 
{
    1,             /*CSMON_sFileParameterDefinitions_t.eDefaultReadFunctionCode),*/
    1,             /*CSMON_sFileParameterDefinitions_t.eDefaultWriteFunctionCode)*/
};
uint8_t CSMON_au8FileParameterListElementsSizeTablePart3[CSMON_sFileParameterDefinitions_t_ElementsThird] = 
{
    1,             /*CSMON_sFileParameterDefinitions_t.u8BitsCount),*/
    1,             /*CSMON_sFileParameterDefinitions_t.u8PaddingByte),*/
};

#endif

const uint8_t CSMON_au8FileParameterListElementsCountInPartTable [CSMON_sFileParameterDefinitions_t_ElementsPartsCount] = 
{
    CSMON_sFileParameterDefinitions_t_ElementsFirst,
    CSMON_sFileParameterDefinitions_t_ElementsSecond,
    CSMON_sFileParameterDefinitions_t_ElementsThird,
};












uint16_t CSMON_u16FileParameterFillCount = 0; /* Count Filled Elements */
uint16_t CSMON_u16FileParameterUseMaxCnt = 0; /* Count Maximum Used Elements */
uint16_t CSMON_u16FileParameterListCount = 0; /* Count Fixed in CSMON_eInit */
uint16_t CSMON_u16FileParameterNameCount = 0; /* Count Fixed in CSMON_eInit */
uint16_t CSMON_u16FileParameterNumeric   = 0; /* Count Fixed in CSMON_eInit */

#if CSMON_PARAMETER_LIST_READ_ONLY
__CONSTANT_MEMORY_MODIFIER__ CSMON_sFileParameterDefinitions_t* CSMON_pasFileParameterList = NULL; /* Address Initialized in CSMON_eInit */
#else
__EXTENDED_MEMORY_MODIFIER__ CSMON_sFileParameterDefinitions_t* CSMON_pasFileParameterList = NULL; /* Address Initialized in CSMON_eInit */
#if _CSMON_IN_INTERNAL_RAM
CSMON_sFileParameterDefinitions_t CSMON_pasFileParameterListRealocatable[BOARDCFG_CSMON_PARAM_COUNT_REQUEST_DPN];   //DPN Table with parameters. 100 should be OK.
#endif                                                                                                              //In the original code this table is allocated in
                                                                                                                    //external RAM. Let's put it in the internal CPU RAM
#endif

#if CSMON_PARAMETER_LIST_READ_ONLY

const CSMON_sFileParameterList_t          CSMON_sFileParameterList = 
{
    {
        

#if USE_CONTROL
    /* 1000 */      
            /*
    MODBUSREG_REGID_STACK1_RESERVED_REQUEST_STATUS  ,
    MODBUSREG_REGID_STACK1_CONTROL_STATE            ,
    MODBUSREG_REGID_STACK1_CONTROL_ERROR            ,                               
    MODBUSREG_REGID_STACK1_ERROR_STATE              , 
    MODBUSREG_REGID_STACK1_CLEAR_FAULTS             ,        
    MODBUSREG_REGID_STACK1_ERROR_STATUS             ,            
    MODBUSREG_REGID_STACK1_SPEED_REQUEST            ,
    MODBUSREG_REGID_STACK1_VOLTAGE_DCLINK_REQUEST   ,        
    MODBUSREG_REGID_STACK1_CURRENT_REQUEST          , 
    MODBUSREG_REGID_STACK1_RUN_REQUEST_STATUS       ,             */
     
     PARAM_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_RESERVED_REQUEST_STATUS,                          /* u16ValueAccessKeyStoradeID */
             0,                          /* u8ValueAccessKeyStartBitIndex */
             0,                          /* u6Reserved */
             0,                          /* bArray */
             0,                          /* bBitField */
             MODBUSREG_REGID_STACK1_RESERVED_REQUEST_STATUS,
             CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
             CSMON_ACCESS_OPERATOR,      /* eWriteAccessLevelMin */
             HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
             HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
             bitsizeof(1)                /* bTestDummyStatus/bTestDummyRequest */),

     PARAM_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_CONTROL_STATE,                          /* u16ValueAccessKeyStoradeID */
             0,                          /* u8ValueAccessKeyStartBitIndex */
             0,                          /* u6Reserved */
             0,                          /* bArray */
             0,                          /* bBitField */
             MODBUSREG_REGID_STACK1_CONTROL_STATE,
             CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
             CSMON_ACCESS_PROGRAMMER,    /* eWriteAccessLevelMin */
             HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
             HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
             bitsizeof(2)                /* eControlState */),

     PARAM_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_CONTROL_ERROR,                          /* u16ValueAccessKeyStoradeID */
             0,                          /* u8ValueAccessKeyStartBitIndex */
             0,                          /* u6Reserved */
             0,                          /* bArray */
             0,                          /* bBitField */
             MODBUSREG_REGID_STACK1_CONTROL_ERROR,
             CSMON_ACCESS_PROGRAMMER,    /* eReadAccessLevelMin */
             CSMON_ACCESS_PROGRAMMER,    /* eWriteAccessLevelMin */
             HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
             HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
             bitsizeof(2)                /* eErrorState */),

     PARAM_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_ERROR_STATE,                          /* u16ValueAccessKeyStoradeID */
             0,                          /* u8ValueAccessKeyStartBitIndex */
             0,                          /* u6Reserved */
             0,                          /* bArray */
             0,                          /* bBitField */
             MODBUSREG_REGID_STACK1_ERROR_STATE,
             CSMON_ACCESS_SERVICE,       /* eReadAccessLevelMin */
             CSMON_ACCESS_PROGRAMMER,    /* eWriteAccessLevelMin */
             HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
             HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
             bitsizeof(2)                /* eTestProcErrorState */),

     PARAM_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_CLEAR_FAULTS,                          /* u16ValueAccessKeyStoradeID */
             0,                          /* u8ValueAccessKeyStartBitIndex */
             0,                          /* u6Reserved */
             0,                          /* bArray */
             0,                          /* bBitField */
             MODBUSREG_REGID_STACK1_CLEAR_FAULTS,
             CSMON_ACCESS_FREE,          /* eReadAccessLevelMin */
             CSMON_ACCESS_OPERATOR,      /* eWriteAccessLevelMin */
             HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
             HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
             bitsizeof(1)                /* bClearFaultStatus/bClearFaultRequest */),

     PARAM_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_ERROR_STATUS,                          /* u16ValueAccessKeyStoradeID */
             0,                          /* u8ValueAccessKeyStartBitIndex */
             0,                          /* u6Reserved */
             0,                          /* bArray */
             0,                          /* bBitField */
             MODBUSREG_REGID_STACK1_ERROR_STATUS,
             CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
             CSMON_ACCESS_DENIED,        /* eWriteAccessLevelMin */
             HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
             HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
             bitsizeof(2)                /* bTestErrorStatus */),

     PARAM_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_SPEED_REQUEST,                          /* u16ValueAccessKeyStoradeID */
             0,                          /* u8ValueAccessKeyStartBitIndex */
             0,                          /* u6Reserved */
             0,                          /* bArray */
             0,                          /* bBitField */
             MODBUSREG_REGID_STACK1_SPEED_REQUEST,
             CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
             CSMON_ACCESS_ENGINEER,      /* eWriteAccessLevelMin */
             HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
             HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
             bitsizeof(2)                /* sControlParameter.sStack[STACK_1].s16FieldSpeed_dHz */),

     PARAM_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_VOLTAGE_DCLINK_REQUEST,                          /* u16ValueAccessKeyStoradeID */
             0,                          /* u8ValueAccessKeyStartBitIndex */
             0,                          /* u6Reserved */
             0,                          /* bArray */
             0,                          /* bBitField */
             MODBUSREG_REGID_STACK1_VOLTAGE_DCLINK_REQUEST,
             CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
             CSMON_ACCESS_SERVICE,       /* eWriteAccessLevelMin */
             HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
             HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
             bitsizeof(2)                /* s16VoltageReqDCLink_dV */),

     PARAM_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_CURRENT_REQUEST,                          /* u16ValueAccessKeyStoradeID */
             0,                          /* u8ValueAccessKeyStartBitIndex */
             0,                          /* u6Reserved */
             0,                          /* bArray */
             0,                          /* bBitField */
             MODBUSREG_REGID_STACK1_CURRENT_REQUEST,
             CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
             CSMON_ACCESS_SERVICE,       /* eWriteAccessLevelMin */
             HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
             HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
             bitsizeof(2)                /* s16CurrentRMSReq1Ph_dA */),

     PARAM_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_RUN_REQUEST_STATUS,                          /* u16ValueAccessKeyStoradeID */
             0,                          /* u8ValueAccessKeyStartBitIndex */
             0,                          /* u6Reserved */
             0,                          /* bArray */
             0,                          /* bBitField */
             MODBUSREG_REGID_STACK1_RUN_REQUEST_STATUS,
             CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
             CSMON_ACCESS_SERVICE,       /* eWriteAccessLevelMin */
             HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
             HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
             bitsizeof(1)                /* eTestRunRequest/bTestRunStatusFbk */),
                
              /* 1010 */         
 /**
  * 
  * 
            
    MODBUSREG_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEA  ,
    MODBUSREG_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEB  ,                          
    MODBUSREG_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEC  ,
    MODBUSREG_REGID_STACK1_VOLTAGE_DCLINK_RMS  ,
    MODBUSREG_REGID_STACK1_CURRENT_MODULE_RMS_PHASEA  ,
    MODBUSREG_REGID_STACK1_CURRENT_MODULE_RMS_PHASEB  ,                          
    MODBUSREG_REGID_STACK1_CURRENT_MODULE_RMS_PHASEC  ,
    MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEA  ,   
    MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEB  ,   
    MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEC  ,   

       
            

  * 
  * 
  */

        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEA,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                0,                          /* bBitField */
                MODBUSREG_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEA,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_PROGRAMMER,    /* eWriteAccessLevelMin */
                HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
                bitsizeof(2)                /* u16_irms_sns1 */),

        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEB,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                0,                          /* bBitField */
                MODBUSREG_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEB,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_PROGRAMMER,    /* eWriteAccessLevelMin */
                HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
                bitsizeof(2)                /* u16_irms_sns2 */),

        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEC,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                0,                          /* bBitField */
                MODBUSREG_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEC,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_PROGRAMMER,    /* eWriteAccessLevelMin */
                HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
                bitsizeof(2)                /* u16_irms_sns3 */),

        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_REGID_STACK1_VOLTAGE_DCLINK_RMS,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                0,                          /* bBitField */
                MODBUSREG_REGID_STACK1_VOLTAGE_DCLINK_RMS,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_PROGRAMMER,    /* eWriteAccessLevelMin */
                HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
                bitsizeof(2)                /* VoltageStack1 */),

        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_REGID_STACK1_CURRENT_MODULE_RMS_PHASEA,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                0,                          /* bBitField */
                MODBUSREG_REGID_STACK1_CURRENT_MODULE_RMS_PHASEA,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_PROGRAMMER,    /* eWriteAccessLevelMin */
                HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
                bitsizeof(2)                /* u16_irms_mod1 */),

        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_REGID_STACK1_CURRENT_MODULE_RMS_PHASEB,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                0,                          /* bBitField */
                MODBUSREG_REGID_STACK1_CURRENT_MODULE_RMS_PHASEB,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_PROGRAMMER,    /* eWriteAccessLevelMin */
                HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
                bitsizeof(2)                /* u16_irms_mod2 */),

        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_REGID_STACK1_CURRENT_MODULE_RMS_PHASEC,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                0,                          /* bBitField */
                MODBUSREG_REGID_STACK1_CURRENT_MODULE_RMS_PHASEC,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_PROGRAMMER,    /* eWriteAccessLevelMin */
                HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
                bitsizeof(2)                /* u16_irms_mod3 */),

        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEA,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                0,                          /* bBitField */
                MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEA,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_PROGRAMMER,    /* eWriteAccessLevelMin */
                HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
                bitsizeof(2)                /* TempFbkStack1[0] */),

        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEB,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                0,                          /* bBitField */
                MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEB,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_PROGRAMMER,    /* eWriteAccessLevelMin */
                HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
                bitsizeof(2)                /* TempFbkStack1[1] */),

        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEC,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                0,                          /* bBitField */
                MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEC,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_PROGRAMMER,    /* eWriteAccessLevelMin */
                HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
                bitsizeof(2)                /* TempFbkStack1[2] */),

                
    /* 1020
    MODBUSREG_REGID_STACK1_CURRENT_SENSOR_PHASEA  ,
    MODBUSREG_REGID_STACK1_CURRENT_SENSOR_PHASEB  ,                          
    MODBUSREG_REGID_STACK1_CURRENT_SENSOR_PHASEC  ,
    MODBUSREG_REGID_STACK1_VOLTAGE_DCLINK_FEEDBACK  ,
    MODBUSREG_REGID_STACK1_CURRENT_MODULE_PHASEA  ,
    MODBUSREG_REGID_STACK1_CURRENT_MODULE_PHASEB  ,                          
    MODBUSREG_REGID_STACK1_CURRENT_MODULE_PHASEC  ,
    MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_PHASEA  ,
    MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_PHASEB  ,
    MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_PHASEC  ,
*/
     
        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_REGID_STACK1_CURRENT_SENSOR_PHASEA,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                0,                          /* bBitField */
                MODBUSREG_REGID_STACK1_CURRENT_SENSOR_PHASEA,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_PROGRAMMER,    /* eWriteAccessLevelMin */
                HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
                bitsizeof(2)                /* CurrentStack1[0] */),

        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_REGID_STACK1_CURRENT_SENSOR_PHASEB,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                0,                          /* bBitField */
                MODBUSREG_REGID_STACK1_CURRENT_SENSOR_PHASEB,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_PROGRAMMER,    /* eWriteAccessLevelMin */
                HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
                bitsizeof(2)                /* CurrentStack1[1] */),

        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_REGID_STACK1_CURRENT_SENSOR_PHASEC,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                0,                          /* bBitField */
                MODBUSREG_REGID_STACK1_CURRENT_SENSOR_PHASEC,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_PROGRAMMER,    /* eWriteAccessLevelMin */
                HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
                bitsizeof(2)                /* CurrentStack1[2] */),

        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_REGID_STACK1_VOLTAGE_DCLINK_FDBACK,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                0,                          /* bBitField */
                MODBUSREG_REGID_STACK1_VOLTAGE_DCLINK_FEEDBACK,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_PROGRAMMER,    /* eWriteAccessLevelMin */
                HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
                bitsizeof(2)                /* VoltageStack1 */),

        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_REGID_STACK1_CURRENT_MODULE_PHASEA,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                0,                          /* bBitField */
                MODBUSREG_REGID_STACK1_CURRENT_MODULE_PHASEA,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_PROGRAMMER,    /* eWriteAccessLevelMin */
                HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
                bitsizeof(2)                /* CurrFbkStack1[0] */),

        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_REGID_STACK1_CURRENT_MODULE_PHASEB,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                0,                          /* bBitField */
                MODBUSREG_REGID_STACK1_CURRENT_MODULE_PHASEB,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_PROGRAMMER,    /* eWriteAccessLevelMin */
                HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
                bitsizeof(2)                /* CurrFbkStack1[1] */),

        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_REGID_STACK1_CURRENT_MODULE_PHASEC,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                0,                          /* bBitField */
                MODBUSREG_REGID_STACK1_CURRENT_MODULE_PHASEC,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_PROGRAMMER,    /* eWriteAccessLevelMin */
                HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
                bitsizeof(2)                /* CurrFbkStack1[2] */),

        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_REGID_STACK1_TEMPERATURE_MODULE_PHASEA,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                0,                          /* bBitField */
                MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_PHASEA,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_PROGRAMMER,    /* eWriteAccessLevelMin */
                HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
                bitsizeof(2)                /* TempFbkStack1[0] */),

        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_REGID_STACK1_TEMPERATURE_MODULE_PHASEB,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                0,                          /* bBitField */
                MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_PHASEB,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_PROGRAMMER,    /* eWriteAccessLevelMin */
                HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
                bitsizeof(2)                /* TempFbkStack1[1] */),

        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_REGID_STACK1_TEMPERATURE_MODULE_PHASEC,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                0,                          /* bBitField */
                MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_PHASEC,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_PROGRAMMER,    /* eWriteAccessLevelMin */
                HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
                bitsizeof(2)                /* TempFbkStack1[2] */),



    #if CSMON_USE_NO_COILS == 0
        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_COILID_TEST,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                1,                          /* bBitField */
                MODBUSREG_COILID_CONTROL_TEST,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_OPERATOR,      /* eWriteAccessLevelMin */
                HMMODBUS_CMD_RD_COILS,      /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_WR_COIL,       /* eDefaultWriteFunctionCode */
                1                           /*  */),
    #else
        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_COILID_TEST,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                0,                          /* bBitField */
                MODBUSREG_REGID_MODBUS_DUMMY_USAGE,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_OPERATOR,      /* eWriteAccessLevelMin */
                HMMODBUS_CMD_INVALID,                          /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_INVALID,                          /* eDefaultWriteFunctionCode */
                bitsizeof(1)                /*  */),
    #endif

    #if CSMON_USE_NO_COILS == 0
        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_INCOILID_DUMMY,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                1,                          /* bBitField */
                MODBUSREG_INCOILID_CONTROL_DUMMY,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_OPERATOR,      /* eWriteAccessLevelMin */
                HMMODBUS_CMD_RD_DISCRETE_INPUTS,      /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_INVALID,                          /* eDefaultWriteFunctionCode */
                1                           /*  */),
    #else
        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_INCOILID_DUMMY,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                0,                          /* bBitField */
                MODBUSREG_REGID_MODBUS_DUMMY_USAGE,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_OPERATOR,      /* eWriteAccessLevelMin */
                HMMODBUS_CMD_INVALID,                          /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_INVALID,                          /* eDefaultWriteFunctionCode */
                bitsizeof(1)                /*  */),
    #endif

    #if CSMON_USE_NO_INPUT_REGISTERS == 0
        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_INREGID_DUMMY,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                0,                          /* bBitField */
                MODBUSREG_INREGID_CONTROL_DUMMY,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_OPERATOR,      /* eWriteAccessLevelMin */
                HMMODBUS_CMD_RD_INPUT_REGS, /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_INVALID,                          /* eDefaultWriteFunctionCode */
                bitsizeof(2)                /*  */),
    #else
        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_INREGID_DUMMY,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                0,                          /* bBitField */
                MODBUSREG_REGID_MODBUS_DUMMY_USAGE,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_OPERATOR,      /* eWriteAccessLevelMin */
                HMMODBUS_CMD_INVALID,                          /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_INVALID,                          /* eDefaultWriteFunctionCode */
                bitsizeof(2)                /*  */),
    #endif


        /* 1040
        MODBUSREG_REGID_STACK1_CFBK_MODULE_NOMINAL_CURRENT = MODBUSREG_REGID_1040,
        */
        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_REGID_STACK1_CFBK_MODULE_NOMINAL_CURRENT,                         /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                0,                          /* bBitField */
                MODBUSREG_REGID_STACK1_CFBK_MODULE_NOMINAL_CURRENT,
                CSMON_ACCESS_OPERATOR,      /* eReadAccessLevelMin */
                CSMON_ACCESS_SERVICE,       /* eWriteAccessLevelMin */
                HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
                bitsizeof(2)                /* ??? */),



#endif  /* USE_CONTROL */

        PARAM_INIT_WITH_CS(
                CSMON_PARAMETER_REGID_MODBUS_RECEIVED_MESSAGES_COUNTER,   /* u16ValueAccessKeyStoradeID */
                0,                          /* u8ValueAccessKeyStartBitIndex */
                0,                          /* u6Reserved */
                0,                          /* bArray */
                0,                          /* bBitField */
                MODBUSREG_REGID_MODBUS_RECEIVED_MESSAGES_COUNTER,
                CSMON_ACCESS_FREE,          /* eReadAccessLevelMin */
                CSMON_ACCESS_PROGRAMMER,    /* eWriteAccessLevelMin */
                HMMODBUS_CMD_RD_HOLD_REGS,  /* eDefaultReadFunctionCode */
                HMMODBUS_CMD_WR_HOLD_REG,   /* eDefaultWriteFunctionCode */
                bitsizeof(2)                /* u16ModbusCompleteReceiveMessageCounterThisDevice */),
    }
};

#endif



#if CSMON_PARAMETER_LIST_READ_ONLY
__CONSTANT_MEMORY_MODIFIER__ CSMON_sFileParameterShortName_t* CSMON_pasFileParameterShortName = NULL; /* Address Initialized in CSMON_eInit */
#else
__EXTENDED_MEMORY_MODIFIER__ CSMON_sFileParameterShortName_t* CSMON_pasFileParameterShortName = NULL; /* Address Initialized in CSMON_eInit */
#if _CSMON_IN_INTERNAL_RAM
CSMON_sFileParameterShortName_t CSMON_pasFileParameterShortNameRealocatable[BOARDCFG_CSMON_PARAM_COUNT_REQUEST_DPN]; //DPN Table with Parameter short names (32 char each)
#endif                                                                                                               //In the original code this table is allocated in
                                                                                                                     //external RAM. Let's put it in the internal CPU RAM
#endif

#if CSMON_PARAMETER_LIST_READ_ONLY

const CSMON_sFileParameterShortName_t     CSMON_asFileParameterShortName[CSMON_FILE_PARAMETER_COUNT_MAX] = 
{
#if USE_CONTROL
    /* 1000 */
    { "S1Reserved\tBool" }, /* Reserved Write Request / Reserved Read Status  */
    { "S1CtrlSta\tEnum"  }, /* ReadOnly */
    { "S1CtrlErr\tEnum"  }, /* ReadOnly */
    { "S1ErrSta\tEnum"   }, /* ReadOnly */
    { "S1ClrFlt\tEnum"   }, /* Clear Fault Write Request / Clear Fault Read Status */
    { "S1TestErr\tEnum"  }, /* Read Only */
    { "S1FreqReq\tEnum"  }, /* Read Write Resolution 0.1Hz(dHz) */
    { "S1VoltReq\tEnum"  }, /* Read Write Resolution 0.1V (dV) */
    { "S1CurrReq\tEnum"  }, /* Read Write Resolution 0.1A (dA) */
    { "S1OnOff\tBool"    }, /* Write Run Request / Read Run Status */
    
    /* 1010 */
    { "S1I_SnsRMSL1\tdA"  },/* ReadOnly Resolution 0.1A (dA) */
    { "S1I_SnsRMSL2\tdA"  },/* ReadOnly Resolution 0.1A (dA) */
    { "S1I_SnsRMSL3\tdA"  },/* ReadOnly Resolution 0.1A (dA) */
    { "S1VdcLinkRMS\tdV"  },/* ReadOnly Resolution 0.1V (dV) */
    { "S1I_DrvRMSL1\tdA"  },/* ReadOnly Resolution 0.1A (dA) */
    { "S1I_DrvRMSL2\tdA"  },/* ReadOnly Resolution 0.1A (dA) */
    { "S1I_DrvRMSL3\tdA"  },/* ReadOnly Resolution 0.1A (dA) */
    { "S1TempFltrL1\tdoC" },/* ReadOnly Resolution 0.1oC (doC) */
    { "S1TempFltrL2\tdoC" },/* ReadOnly Resolution 0.1oC (doC) */
    { "S1TempFltrL3\tdoC" },/* ReadOnly Resolution 0.1oC (doC) */
    
    /* 1020 */
    { "S1I_SnsL1\tdA"   },  /* ReadOnly Resolution 0.1A (dA) */
    { "S1I_SnsL2\tdA"   },  /* ReadOnly Resolution 0.1A (dA) */
    { "S1I_SnsL3\tdA"   },  /* ReadOnly Resolution 0.1A (dA) */
    { "S1VdcLink\tdV"   },  /* ReadOnly Resolution 0.1V (dV) */
    { "S1I_DrvL1\tdA"   },  /* ReadOnly Resolution 0.1A (dA) */
    { "S1I_DrvL2\tdA"   },  /* ReadOnly Resolution 0.1A (dA) */
    { "S1I_DrvL3\tdA"   },  /* ReadOnly Resolution 0.1A (dA) */
    { "S1Temp_L1\tdoC"  },  /* ReadOnly Resolution 0.1oC (doC) */
    { "S1Temp_L2\tdoC"  },  /* ReadOnly Resolution 0.1oC (doC) */
    { "S1Temp_L3\tdoC"  },  /* ReadOnly Resolution 0.1oC (doC) */
    
    /* CoilAdress 0 */
    { "CoilTest\tBool" },
    
    /* InputCoilAdress 0 */
    { "InputCoil\tBool" },
    
    /* InputRegisterAdress 0 */
    { "InputReg\tUnit" },

    /* 1020 */
    { "S1INominal\tA" },        /* ReadOnly Resolution 1A (A) */

#endif

    /* 0xFFFE */
    { "MsgCntr"  }, /* u16ModbusCompleteReceiveMessageCounterThisDevice */
};

#endif


uint32_t CSMON_pu32FileParameterString = NULL;
#if _CSMON_IN_INTERNAL_RAM
uint32_t CSMON_pu32FileParameterStringRealocatable[BOARDCFG_CSMON_PARAM_COUNT_REQUEST_DPN]; //DPN Table for Parameter description string. 100 chars shoul dbe enought
#endif                                                                                      //In the original code this table is allocated in external RAM.
                                                                                            //Let's put it in the internal CPU RAM


#if CSMON_PARAMETER_LIST_READ_ONLY == 0
CSMON_sFileParameterAux_t* CSMON_psFileParameterAux = NULL;
#if _CSMON_IN_INTERNAL_RAM
CSMON_sFileParameterAux_t  CSMON_psFileParameterAuxRealocatable[BOARDCFG_CSMON_PARAM_COUNT_REQUEST_DPN]; //DPN Auxilary Table for Parameter description string?
#endif                                                                                                   //In the original code this table is allocated in external RAM.
                                                                                                         //Let's put it in the internal CPU RAM
#else
CSMON_sFileParameterAux_t* CSMON_psFileParameterAux = NULL;
#endif
uint32_t CSMON_pu32FileParameterReserved = NULL;

uint32_t CSMON_pu32HoldingRegistersAddressTable = NULL;
uint32_t CSMON_pu32HoldingRegistersAddressTableInRam = NULL;
uint32_t CSMON_pu32HoldingRegistersAccessTableInRam = NULL;

#if CSMON_PARAMETER_LIST_READ_ONLY
__CONSTANT_MEMORY_MODIFIER__ CSMON_uFileParameterNumeric_t* CSMON_pauFileParameterNumeric = NULL; /* Address Initialized in CSMON_eInit */
#else
__EXTENDED_MEMORY_MODIFIER__ CSMON_uFileParameterNumeric_t* CSMON_pauFileParameterNumeric = NULL; /* Address Initialized in CSMON_eInit */
#if _CSMON_IN_INTERNAL_RAM
CSMON_uFileParameterNumeric_t CSMON_pauFileParameterNumericRealocatable[BOARDCFG_CSMON_PARAM_COUNT_REQUEST_DPN]; //DPN Table with the properties of the Numeric parameters.
#endif                                                                                                           //In the original code this table is allocated in
                                                                                                                 //external RAM. Let's put it in the internal CPU RAM
#endif


#if CSMON_PARAMETER_LIST_READ_ONLY

#define s16DeciFrom_Float(x)    (int16_t)(x*10)
#define u16DeciFrom_Float(x)   (uint16_t)(x*10)

#define fMult1  0x3f800000
#define fMult01 0x3dcccccd
#define fMultCalc()

#define PARAM_NUMB_INIT_WITH_CS(V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12) \
{ \
 {{.u16ValueAccessKeyStoradeID = V1, \
   .u8ValueAccessKeyStartBitIndex = V2, \
   .u6Reserved = V3, \
   .bArray = V4, \
   .bBitField = V5, \
 }}, \
 { \
    V6 , /* eDataType */ \
    V7   /* eVisualType */ \
 }, \
    V8, /* u64Default */ \
    V9, /* u64Minimum */ \
    V10,/* u64Maximum */ \
    V11,/* s64Multply */ \
    V12 /* s64Offset */ \
}


const CSMON_uFileParameterNumeric_t CSMON_uFileParameterNumeric[CSMON_FILE_PARAMETER_COUNT_MAX] =
{
    #if USE_CONTROL
    /* 1000 */
    /*  CSMON_PARAMETER_REGID_STACK1_RESERVED_REQUEST_STATUS
        CSMON_PARAMETER_REGID_STACK1_CONTROL_STATE
        CSMON_PARAMETER_REGID_STACK1_CONTROL_ERROR
        CSMON_PARAMETER_REGID_STACK1_ERROR_STATE
        CSMON_PARAMETER_REGID_STACK1_CLEAR_FAULTS
        CSMON_PARAMETER_REGID_STACK1_ERROR_STATUS
        CSMON_PARAMETER_REGID_STACK1_SPEED_REQUEST
        CSMON_PARAMETER_REGID_STACK1_VOLTAGE_DCLINK_REQUEST
        CSMON_PARAMETER_REGID_STACK1_CURRENT_REQUEST
        CSMON_PARAMETER_REGID_STACK1_RUN_REQUEST_STATUS  */


    {   /* CSMON_uFileParameterNumeric_t */
        PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_RESERVED_REQUEST_STATUS,                          /* u16ValueAccessKeyStoradeID */
             0,                          /* u8ValueAccessKeyStartBitIndex */
             0,                          /* u6Reserved */
             0,                          /* bArray */
             0,                          /* bBitField */
             CSMON_DATA_TYPE_BOOL_FLAG,        /* eDataType */
             CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
             0,                         /* u64Default */
             0,                         /* u64Minimum */
             1,                         /* u64Maximum */
             .s64MultplyFloat = 0.0F,                       /* s64Multply */
             0                           /* s64Offset */),
    },



    {
     PARAM_NUMB_INIT_WITH_CS(
          CSMON_PARAMETER_REGID_STACK1_CONTROL_STATE,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_ENUM_FLAG,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          0,                         /* u64Default */
          0,                         /* u64Minimum */
          0xFFFF,                         /* u64Maximum */
          .s64MultplyFloat = 0.0F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_CONTROL_ERROR,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_ENUM_FLAG,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          0,                         /* u64Default */
          0,                         /* u64Minimum */
          0xFFFF,                         /* u64Maximum */
          .s64MultplyFloat = 0.0F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_ERROR_STATE,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_ENUM_FLAG,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          0,                         /* u64Default */
          0,                         /* u64Minimum */
          0xFFFF,                         /* u64Maximum */
          .s64MultplyFloat = 0.0F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_CLEAR_FAULTS,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_BOOL_FLAG,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          0,                         /* u64Default */
          0,                         /* u64Minimum */
          0x1,                         /* u64Maximum */
          .s64MultplyFloat = 0.0F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_ERROR_STATUS,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_ENUM_FLAG,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          0,                         /* u64Default */
          0,                         /* u64Minimum */
          0xFFFF,                         /* u64Maximum */
          .s64MultplyFloat = 0.0F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_SPEED_REQUEST,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_S16,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          s16DeciFrom_Float(50.0),                         /* u64Default */
          s16DeciFrom_Float(10.0),                         /* u64Minimum */
          s16DeciFrom_Float(100.0),                         /* u64Maximum */
          .s64MultplyFloat = 0.1F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_VOLTAGE_DCLINK_REQUEST,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_S16,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          s16DeciFrom_Float(100.0),                         /* u64Default */
          s16DeciFrom_Float(0.0),                         /* u64Minimum */
          s16DeciFrom_Float(1500.0),                         /* u64Maximum */
          .s64MultplyFloat = 0.1F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_CURRENT_REQUEST,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_S16,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          s16DeciFrom_Float(50.0),                         /* u64Default */
          s16DeciFrom_Float(0.0),                         /* u64Minimum */
          s16DeciFrom_Float(400.0),                         /* u64Maximum */
          .s64MultplyFloat = 0.1F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_RUN_REQUEST_STATUS,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_BOOL_FLAG,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          0,                         /* u64Default */
          0,                         /* u64Minimum */
          0x1,                         /* u64Maximum */
          .s64MultplyFloat = 0.0F,                       /* s64Multply */
          0                           /* s64Offset */),
    },


    /* 1010 */
    /*  CSMON_PARAMETER_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEA
        CSMON_PARAMETER_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEB
        CSMON_PARAMETER_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEC
        CSMON_PARAMETER_REGID_STACK1_VOLTAGE_DCLINK_RMS
        CSMON_PARAMETER_REGID_STACK1_CURRENT_MODULE_RMS_PHASEA
        CSMON_PARAMETER_REGID_STACK1_CURRENT_MODULE_RMS_PHASEB
        CSMON_PARAMETER_REGID_STACK1_CURRENT_MODULE_RMS_PHASEC
        CSMON_PARAMETER_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEA
        CSMON_PARAMETER_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEB
        CSMON_PARAMETER_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEC */



    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEA,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_S16,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          s16DeciFrom_Float(50.0),                         /* u64Default */
          s16DeciFrom_Float(0.0),                         /* u64Minimum */
          s16DeciFrom_Float(400.0),                         /* u64Maximum */
          .s64MultplyFloat = 0.1F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEB,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_S16,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          s16DeciFrom_Float(50.0),                         /* u64Default */
          s16DeciFrom_Float(0.0),                         /* u64Minimum */
          s16DeciFrom_Float(400.0),                         /* u64Maximum */
          .s64MultplyFloat = 0.1F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEC,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_S16,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          s16DeciFrom_Float(50.0),                         /* u64Default */
          s16DeciFrom_Float(0.0),                         /* u64Minimum */
          s16DeciFrom_Float(400.0),                         /* u64Maximum */
          .s64MultplyFloat = 0.1F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_VOLTAGE_DCLINK_RMS,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_S16,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          s16DeciFrom_Float(100.0),                         /* u64Default */
          s16DeciFrom_Float(0.0),                         /* u64Minimum */
          s16DeciFrom_Float(1500.0),                         /* u64Maximum */
          .s64MultplyFloat = 0.1F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_CURRENT_MODULE_RMS_PHASEA,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_S16,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          s16DeciFrom_Float(50.0),                         /* u64Default */
          s16DeciFrom_Float(0.0),                         /* u64Minimum */
          s16DeciFrom_Float(400.0),                         /* u64Maximum */
          .s64MultplyFloat = 0.1F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_CURRENT_MODULE_RMS_PHASEB,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_S16,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          s16DeciFrom_Float(50.0),                         /* u64Default */
          s16DeciFrom_Float(0.0),                         /* u64Minimum */
          s16DeciFrom_Float(400.0),                         /* u64Maximum */
          .s64MultplyFloat = 0.1F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_CURRENT_MODULE_RMS_PHASEC,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_S16,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          s16DeciFrom_Float(50.0),                         /* u64Default */
          s16DeciFrom_Float(0.0),                         /* u64Minimum */
          s16DeciFrom_Float(400.0),                         /* u64Maximum */
          .s64MultplyFloat = 0.1F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEA,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_S16,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          s16DeciFrom_Float(22.5),                         /* u64Default */
          s16DeciFrom_Float(0.0),                         /* u64Minimum */
          s16DeciFrom_Float(100.0),                         /* u64Maximum */
          .s64MultplyFloat = 0.1F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEB,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_S16,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          s16DeciFrom_Float(22.5),                         /* u64Default */
          s16DeciFrom_Float(0.0),                         /* u64Minimum */
          s16DeciFrom_Float(100.0),                         /* u64Maximum */
          .s64MultplyFloat = 0.1F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEC,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_S16,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          s16DeciFrom_Float(22.5),                         /* u64Default */
          s16DeciFrom_Float(0.0),                         /* u64Minimum */
          s16DeciFrom_Float(100.0),                         /* u64Maximum */
          .s64MultplyFloat = 0.1F,                       /* s64Multply */
          0                           /* s64Offset */),
    },


    /* 1020 */
    /*  CSMON_PARAMETER_REGID_STACK1_CURRENT_SENSOR_PHASEA
        CSMON_PARAMETER_REGID_STACK1_CURRENT_SENSOR_PHASEB
        CSMON_PARAMETER_REGID_STACK1_CURRENT_SENSOR_PHASEC
        CSMON_PARAMETER_REGID_STACK1_VOLTAGE_DCLINK_FDBACK
        CSMON_PARAMETER_REGID_STACK1_CURRENT_MODULE_PHASEA
        CSMON_PARAMETER_REGID_STACK1_CURRENT_MODULE_PHASEB
        CSMON_PARAMETER_REGID_STACK1_CURRENT_MODULE_PHASEC
        CSMON_PARAMETER_REGID_STACK1_TEMPERATURE_MODULE_PHASEA
        CSMON_PARAMETER_REGID_STACK1_TEMPERATURE_MODULE_PHASEB
        CSMON_PARAMETER_REGID_STACK1_TEMPERATURE_MODULE_PHASEC */

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_CURRENT_SENSOR_PHASEA,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_S16,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          s16DeciFrom_Float(50.0),                         /* u64Default */
          s16DeciFrom_Float(0.0),                         /* u64Minimum */
          s16DeciFrom_Float(400.0),                         /* u64Maximum */
          .s64MultplyFloat = 0.1F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_CURRENT_SENSOR_PHASEB,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_S16,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          s16DeciFrom_Float(50.0),                         /* u64Default */
          s16DeciFrom_Float(0.0),                         /* u64Minimum */
          s16DeciFrom_Float(400.0),                         /* u64Maximum */
          .s64MultplyFloat = 0.1F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_CURRENT_SENSOR_PHASEC,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_S16,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          s16DeciFrom_Float(50.0),                         /* u64Default */
          s16DeciFrom_Float(0.0),                         /* u64Minimum */
          s16DeciFrom_Float(400.0),                         /* u64Maximum */
          .s64MultplyFloat = 0.1F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_VOLTAGE_DCLINK_FDBACK,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_S16,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          s16DeciFrom_Float(100.0),                         /* u64Default */
          s16DeciFrom_Float(0.0),                         /* u64Minimum */
          s16DeciFrom_Float(1500.0),                         /* u64Maximum */
          .s64MultplyFloat = 0.1F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_CURRENT_MODULE_PHASEA,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_S16,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          s16DeciFrom_Float(50.0),                         /* u64Default */
          s16DeciFrom_Float(0.0),                         /* u64Minimum */
          s16DeciFrom_Float(400.0),                         /* u64Maximum */
          .s64MultplyFloat = 0.1F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_CURRENT_MODULE_PHASEB,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_S16,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          s16DeciFrom_Float(50.0),                         /* u64Default */
          s16DeciFrom_Float(0.0),                         /* u64Minimum */
          s16DeciFrom_Float(400.0),                         /* u64Maximum */
          .s64MultplyFloat = 0.1F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_CURRENT_MODULE_PHASEC,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_S16,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          s16DeciFrom_Float(50.0),                         /* u64Default */
          s16DeciFrom_Float(0.0),                         /* u64Minimum */
          s16DeciFrom_Float(400.0),                         /* u64Maximum */
          .s64MultplyFloat = 0.1F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_TEMPERATURE_MODULE_PHASEA,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_S16,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          s16DeciFrom_Float(22.5),                         /* u64Default */
          s16DeciFrom_Float(0.0),                         /* u64Minimum */
          s16DeciFrom_Float(100.0),                         /* u64Maximum */
          .s64MultplyFloat = 0.1F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_TEMPERATURE_MODULE_PHASEB,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_S16,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          s16DeciFrom_Float(22.5),                         /* u64Default */
          s16DeciFrom_Float(0.0),                         /* u64Minimum */
          s16DeciFrom_Float(100.0),                         /* u64Maximum */
          .s64MultplyFloat = 0.1F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_TEMPERATURE_MODULE_PHASEC,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_S16,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          s16DeciFrom_Float(22.5),                         /* u64Default */
          s16DeciFrom_Float(0.0),                         /* u64Minimum */
          s16DeciFrom_Float(100.0),                         /* u64Maximum */
          .s64MultplyFloat = 0.1F,                       /* s64Multply */
          0                           /* s64Offset */),
    },




           /* 3000 */
    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_COILID_TEST,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_BOOL_FLAG,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          0,                         /* u64Default */
          0,                         /* u64Minimum */
          1,                         /* u64Maximum */
          .s64MultplyFloat = 0.0F,                       /* s64Multply */
          0                           /* s64Offset */),
    },


    /* 4000 */
    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_INCOILID_DUMMY,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_BOOL_FLAG,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          0,                         /* u64Default */
          0,                         /* u64Minimum */
          1,                         /* u64Maximum */
          .s64MultplyFloat = 0.0F,                       /* s64Multply */
          0                           /* s64Offset */),
    },

    /* 2000 */
    {
      PARAM_NUMB_INIT_WITH_CS(
              CSMON_PARAMETER_INREGID_DUMMY,                          /* u16ValueAccessKeyStoradeID */
           0,                          /* u8ValueAccessKeyStartBitIndex */
           0,                          /* u6Reserved */
           0,                          /* bArray */
           0,                          /* bBitField */
           CSMON_DATA_TYPE_U16,        /* eDataType */
           CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
           0,                         /* u64Default */
           0,                         /* u64Minimum */
           0xFFFF,                         /* u64Maximum */
           .s64MultplyFloat = 0.0F,                       /* s64Multply */
           0                           /* s64Offset */),
    },

    /* 1040 */
    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_STACK1_CFBK_MODULE_NOMINAL_CURRENT,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_U16,        /* eDataType */
          CSMON_VISUAL_TYPE_HEX,      /* eVisualType */
          u16DeciFrom_Float(1000.0),                         /* u64Default */
          u16DeciFrom_Float(0.0),                         /* u64Minimum */
          u16DeciFrom_Float(5000.0),                         /* u64Maximum */
          .s64MultplyFloat = 1.0F,                       /* s64Multply */
          0                           /* s64Offset */),
    },


    #endif  //USE_CONTROL


    /* 65534 */
    {
     PARAM_NUMB_INIT_WITH_CS(
             CSMON_PARAMETER_REGID_MODBUS_RECEIVED_MESSAGES_COUNTER,                          /* u16ValueAccessKeyStoradeID */
          0,                          /* u8ValueAccessKeyStartBitIndex */
          0,                          /* u6Reserved */
          0,                          /* bArray */
          0,                          /* bBitField */
          CSMON_DATA_TYPE_U16,        /* eDataType */
          CSMON_VISUAL_TYPE_DEC,      /* eVisualType */
          0,                         /* u64Default */
          0,                         /* u64Minimum */
          0xFFFF,                         /* u64Maximum */
          .s64MultplyFloat = 0.0F,                       /* s64Multply */
          0                           /* s64Offset */),
    },





};


#endif







        
        
const uint8_t CSMON_au8FileImplementedFunctionCodes[CSMON_FILE_IMPLEMENTED_FUNCTION_CODES_COUNT_MAX] = 
{
    HMMODBUS_CMD_RD_COILS                   /* 0x01 */ ,
    HMMODBUS_CMD_RD_DISCRETE_INPUTS         /* 0x02 */ ,
    HMMODBUS_CMD_RD_HOLD_REGS               /* 0x03 */ ,
    HMMODBUS_CMD_RD_INPUT_REGS              /* 0x04 */ ,
    HMMODBUS_CMD_WR_COIL                    /* 0x05 */ ,
    HMMODBUS_CMD_WR_HOLD_REG                /* 0x06 */ ,
    /* HMMODBUS_CMD_RD_EXCEPTION_STATUS        0x07 , */
    HMMODBUS_CMD_DIAGNOSTICS                /* 0x08 */ ,
    /* HMMODBUS_CMD_RD_EVENT_COUNTER           0x0B , */
    /* HMMODBUS_CMD_RD_EVENT_LOG               0x0C , */
    HMMODBUS_CMD_WR_COILS                   /* 0x0F */ ,
    HMMODBUS_CMD_WR_HOLD_REGS               /* 0x10 */ ,
    HMMODBUS_CMD_RD_SERVER_ID               /* 0x11 */ ,
    HMMODBUS_CMD_RD_FILE_RECORD             /* 0x14 */ ,
    HMMODBUS_CMD_WR_FILE_RECORD             /* 0x15 */ ,
    HMMODBUS_CMD_WR_HOLD_REG_MASK           /* 0x16 */ ,
    HMMODBUS_CMD_RW_HOLD_REGS               /* 0x17 */ ,
    HMMODBUS_CMD_RD_FIFO_QUEUE              /* 0x18 */ ,
    HMMODBUS_CMD_ENCAPSULATED_INTERFACE     /* 0x2B */ ,        /* interpreted as READ_DEVICE_IDENTIFICATION in CSMon */
    
    /* HMMODBUS_CMD_ENCAPSULATED_INTERFACE Sub-Commands */
    /* HMMODBUS_CMD_SUB_CANOPEN_GEN_REF        0x0D , */
    /* HMMODBUS_CMD_SUB_RD_DEVICE_ID           0x0E , */
    
    0x00,   /* Null- Terminate Character If there is space in the array */
};



















/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */



void CSMON_vFileParameterCountCheckSumUpdate(void);
bool CSMON_bPasswordGenerateFromHash(char* pu8HashKey, char* pu8Password, char* pu8HashOutput, int* pnHashOutputLength);
bool CSMON_bAutorizationPasswordCheck(void);
void CSMON_vPrepareFileParameterAccessList(void);

uint32_t CSMON_u32GetRealAddressAndSizeOffsetFromParameter(uint32_t variableID, HMDRV_BUFF_PNTR_MODIFIER HMDRV_U16* pu16RealOffset);
int16_t CSMON_u16FindIndexParameterTable(uint32_t variableID);
void CSMON_vInitHoldingRegistersTableChanges(void);
uint32_t CSMON_u32GetFreeRunningTimerMicroseconds(void);
uint32_t CSMON_u32GetFreeRunningTimerTicks(void);

void CSMON_vSetupHoldingRegistersAndCoilsAddressTableAccessAll(void);
void CSMON_vSetupInputRegistersAndCoilsAddressTableAccessAll(void);
void CSMON_vSetupHoldingRegistersAndCoilsAddressTableFromCSMONParameterList(HMDRV_U16 u16CurrentAccessLevel);
void CSMON_vSetupInputRegistersAndCoilsAddressTableFromCSMONParameterList(HMDRV_U16 u16CurrentAccessLevel);

/* Scope Support */
#if (HMDRV_USE_SCOPE_PARAMETERS) && (!(HMDRV_DISABLE))
#if HMDRV_SCOPE_PARAM_HANDLE_COUNT > 0
HMSCOPEPRM_eResponseCode_t CSMON_eSetUpScopeParam(HMDRV_U16 u16ScopeIndex);
void CSMON_vInitScopes (void);
#endif
#endif
/* Recorder Support */
#if (HMDRV_USE_RECORDER_PARAMETERS) && (!(HMDRV_DISABLE))
#if HMDRV_RECORDER_PARAM_HANDLE_COUNT > 0
HMRECPRM_eResponseCode_t CSMON_eSetUpRecParam(HMDRV_U16 u16RecorderIndex);
void CSMON_vInitRecorders (void);
#endif
#endif


void CSMON_vSetDateTimeAutorization(void);
void CSMON_vSetDateTimeRequest(void);


uint32_t CSMON_u32GetParameterListCheckSum (void);
uint32_t CSMON_u32GetParameterShortNameCheckSum (void);
uint32_t CSMON_u32GetParameterNumbCheckSum (void);
uint32_t CSMON_u32GetParameterStringCheckSum (void);
uint32_t CSMON_u32GetParameterAuxCheckSum (void);



/* *****************************************************************************
 * Functions
 **************************************************************************** */




/* *****************************************************************************
 * CSMON_u16ParametersAccessListIndexGenerate - prototype in csmon_adr.h
 **************************************************************************** */
uint16_t CSMON_u16ParametersAccessListIndexGenerate(uint16_t u16Index)
{
    return CSMON_sFileParameterAccessList.u16ParameterIndex[u16Index];
}

void CSMON_vFileParameterCountCheckSumUpdate(void)
{
    CSMON_sFileParameterCount.u32ParameterListCheckSum   = CSMON_u32GetParameterListCheckSum();
    CSMON_sFileParameterCount.u32ParameterShortNameCheckSum   = CSMON_u32GetParameterShortNameCheckSum() + CSMON_sFileParameterCount.u32ParameterListCheckSum;
    //CSMON_sFileParameterCount.u32ParameterNumbCheckSum   = CSMON_u32GetParameterNumbCheckSum() + CSMON_sFileParameterCount.u32ParameterListCheckSum;
    CSMON_sFileParameterCount.u32ParameterNumbCheckSum   = CSMON_u32GetParameterNumbCheckSum();
    CSMON_sFileParameterCount.u32ParameterStringCheckSum = CSMON_u32GetParameterStringCheckSum();
    CSMON_sFileParameterCount.u32ParameterAuxCheckSum    = CSMON_u32GetParameterAuxCheckSum();
}

/* *****************************************************************************
 * CSMON_bPasswordGenerateFromHash
 **************************************************************************** */
bool CSMON_bPasswordGenerateFromHash(char* pu8HashKey, char* pu8Password, char* pu8HashOutput, int* pnHashOutputLength)
{
    int nResponseHash;
    int nPasswordSize;
    int index;
    bool bResult;

    CSMON_u32FreeRunTimerPasswordHashBegin = CSMON_u32GetFreeRunningTimerTicks();   /* Measure max Time in PasswordHash Prepare */

#ifdef STAT_LED_G_PIN
    //GPIO_writePin(STAT_LED_G_PIN, STAT_LED_ENABLE);
#endif

    bResult = false;

    /* Length of Pre-Defined Password that must match  */
    nPasswordSize = CSMON_FILE_AUTORIZATION_PASSWORD_LENGTH_INTERNAL;
    for (index = 0; index < CSMON_FILE_AUTORIZATION_PASSWORD_LENGTH_INTERNAL; index++)
    {
        if(pu8Password[index] == NULL)
        {
            nPasswordSize = index;
            break;
        }
    }

    nResponseHash = Hash(
            pu8HashKey,
            sizeof(sHashKey_t),
            pu8Password,
            nPasswordSize,
            pu8HashOutput,
            CSMON_FILE_AUTORIZATION_HASH_MAX_LENGTH,
            pnHashOutputLength
            );

    if (nResponseHash == 0) /* no error */
    {
        bResult = true;
    }

#ifdef STAT_LED_G_PIN
    //GPIO_writePin(STAT_LED_G_PIN, STAT_LED_DISABLE);
#endif

    /* Measure max Time in PasswordHash */
    CSMON_u32FreeRunTimerPasswordHashEnd = CSMON_u32GetFreeRunningTimerTicks() - CSMON_u32FreeRunTimerPasswordHashBegin;
    if (CSMON_u32FreeRunTimerPasswordHashEnd > CSMON_u32FreeRunTimerPasswordHashMax)
    {
        CSMON_u32FreeRunTimerPasswordHashMax = CSMON_u32FreeRunTimerPasswordHashEnd;
    }

    return bResult;
}


/* *****************************************************************************
 * CSMON_bAutorizationPasswordCheck
 **************************************************************************** */
bool CSMON_bAutorizationPasswordCheck(void)
{
    bool bResult = 0;
    uint16_t index;
    uint16_t indexCount;
    __CONSTANT_MEMORY_MODIFIER__ CSMON_sAccessLevelPasswords_t* psAccessLevelPassword;
    __CONSTANT_MEMORY_MODIFIER__ int8_t* ps8AccessLevelPassword;
    __EXTENDED_MEMORY_MODIFIER__ int8_t* ps8PasswordRequest;
    

    indexCount = CSMON_FILE_AUTORIZATION_PASSWORD_LENGTH;
    if (indexCount > CSMON_FILE_AUTORIZATION_PASSWORD_LENGTH_INTERNAL)
    {
        indexCount = CSMON_FILE_AUTORIZATION_PASSWORD_LENGTH_INTERNAL;
    }
    
    
    ps8PasswordRequest = (__EXTENDED_MEMORY_MODIFIER__ int8_t*)&CSMON_sFileAutorization.as8Password[0];
    if (CSMON_sFileAutorization.u16AccessLevelReq < CSMON_ACCESS_COUNT)
    {


        #if CSMON_NO_PASSWORD_IN_FREE_ACCESS_LEVEL
        if (CSMON_sFileAutorization.u16AccessLevelReq == CSMON_ACCESS_FREE)
        {
            bResult = 1;
        }
        else
        #endif
        {
            psAccessLevelPassword = (__CONSTANT_MEMORY_MODIFIER__ CSMON_sAccessLevelPasswords_t*)&CSMON_asAccessLevelPasswords[0].as8Password[0];   /* password internal that must be matched */
            psAccessLevelPassword += CSMON_sFileAutorization.u16AccessLevelReq;
            ps8AccessLevelPassword = (__CONSTANT_MEMORY_MODIFIER__ int8_t*)psAccessLevelPassword;   /* password internal that must be matched */
            ps8PasswordRequest = (__EXTENDED_MEMORY_MODIFIER__ int8_t*)&CSMON_sFileAutorization.as8Password[0];   /* received password (or password hash) */

            #if CSMON_FILE_AUTORIZATION_WITH_HASH
            int nHashOutputLength;
            if (CSMON_bGetDateTimeAutorization)     /* to do: times to fail not only once */
            {
                bResult = CSMON_bPasswordGenerateFromHash((char*)&sHashKey, (char*)ps8AccessLevelPassword, (char*) &CSMON_sFileAutorizationHashOut[0].as8Password[0], &nHashOutputLength);
                ps8AccessLevelPassword = &CSMON_sFileAutorizationHashOut[0].as8Password[0];       /* calculated hash from password */
                CSMON_bGetDateTimeAutorization = false;
            }
            else if (CSMON_bPasswordGenerate[CSMON_sFileAutorization.u16AccessLevelReq])     /* to do: times to fail not only once */
            {
                bResult = true;
                nHashOutputLength = CSMON_nHashOutputLength[CSMON_sFileAutorization.u16AccessLevelReq];
                ps8AccessLevelPassword = &CSMON_sFileAutorizationHashOut[CSMON_sFileAutorization.u16AccessLevelReq].as8Password[0];       /* calculated hash from password */
                CSMON_bPasswordGenerate[1] = false;
                CSMON_bPasswordGenerate[2] = false;
                CSMON_bPasswordGenerate[3] = false;
                CSMON_bPasswordGenerate[4] = false;
            }
            else
            {
                bResult = false;
            }
            if (bResult == true)
            {
                if ((*ps8PasswordRequest == *ps8AccessLevelPassword) && (nHashOutputLength))
                {
                    bResult = true;
                }
                else
                {
                    bResult = true;
                }


                for(index = 0; index < nHashOutputLength; index++)
                {
                    if (*ps8PasswordRequest != *ps8AccessLevelPassword)
                    {
                        bResult = false;
                        break;
                    }

                    if (index == (nHashOutputLength-1))
                    {
                        bResult = false;
                    }
                    ps8PasswordRequest++;
                    ps8AccessLevelPassword++;
                }
            }
            else
            {
                bResult = false;        /* Access Denied */
            }
            #else

            if (*ps8PasswordRequest == *ps8AccessLevelPassword)
            {
                bResult = true;
            }
            else
            {
                bResult = false;
            }

            for(index = 0; index < indexCount; index++)
            {
                if (*ps8PasswordRequest != *ps8AccessLevelPassword)
                {
                    bResult = false;
                    break;
                }
                if (*ps8PasswordRequest == 0)
                {
                    break;
                }
                if (*ps8AccessLevelPassword == 0)
                {
                    break;
                }
                ps8PasswordRequest++;
                ps8AccessLevelPassword++;
            }
            #endif

        }
       
    }
    else
    {
        /* Deny Access */
    }
    

    return bResult;
}


/* *****************************************************************************
 * CSMON_vPrepareFileParameterAccessList
 **************************************************************************** */
void CSMON_vPrepareFileParameterAccessList(void)
{
    uint16_t index;
    uint16_t indexAccessed = 0;
    uint16_t indexCount = CSMON_u16FileParameterFillCount;
    uint16_t u16ReadFunctionCode;
    uint16_t u16WriteFunctionCode;
    uint16_t u16ReadAccessLevelMin;
    uint16_t u16WriteAccessLevelMin;
    
    HMDRV_vSetDebugPin();  /* AmberLed on 1038 */


    if ((CSMON_sFileAutorization.u16AccessLevelNow < CSMON_ACCESS_DENIED) && (CSMON_pasFileParameterList != NULL))
    {
        for (index = 0; index < indexCount; index++)
        {
            u16ReadFunctionCode = CSMON_pasFileParameterList[index].eDefaultReadFunctionCode;
            u16WriteFunctionCode = CSMON_pasFileParameterList[index].eDefaultWriteFunctionCode;

            if (   (u16WriteFunctionCode != 0xFF)
                && (u16ReadFunctionCode  != 0xFF)
                && ( (u16ReadFunctionCode  >  0x00) || (u16WriteFunctionCode >  0x00) )     )
            {
                u16ReadAccessLevelMin = CSMON_pasFileParameterList[index].eReadAccessLevelMin;
                u16WriteAccessLevelMin = CSMON_pasFileParameterList[index].eWriteAccessLevelMin;

                if (CSMON_sFileAutorization.u16AccessLevelNow >= u16ReadAccessLevelMin)
                {
                    CSMON_asFileParameterDefinitionsFromAccessLevel[index].eDefaultReadFunctionCode = u16ReadFunctionCode;
                }
                else
                {
                    CSMON_asFileParameterDefinitionsFromAccessLevel[index].eDefaultReadFunctionCode = HMMODBUS_CMD_INVALID;
                }

                if (CSMON_sFileAutorization.u16AccessLevelNow >= u16WriteAccessLevelMin)
                {
                    CSMON_asFileParameterDefinitionsFromAccessLevel[index].eDefaultWriteFunctionCode = u16WriteFunctionCode;
                }
                else
                {
                    CSMON_asFileParameterDefinitionsFromAccessLevel[index].eDefaultWriteFunctionCode = HMMODBUS_CMD_INVALID;
                }

                if ((CSMON_sFileAutorization.u16AccessLevelNow >= u16ReadAccessLevelMin)
                 || (CSMON_sFileAutorization.u16AccessLevelNow >= u16WriteAccessLevelMin))
                {
                    CSMON_sFileParameterAccessList.u16ParameterIndex[indexAccessed] = index;
                    indexAccessed++;
                }
            }
        }

        MODBUSREG_vFileAllowAccess();

        CSMON_vFileParameterCountCheckSumUpdate();
    }
    else
    {
        for (index = 0; index < indexCount; index++)
        {
            CSMON_asFileParameterDefinitionsFromAccessLevel[index].eDefaultReadFunctionCode = HMMODBUS_CMD_INVALID;
            CSMON_asFileParameterDefinitionsFromAccessLevel[index].eDefaultWriteFunctionCode = HMMODBUS_CMD_INVALID;
        }

        MODBUSREG_vFileDenyAccess();

    }
    CSMON_sFileParameterCount.u16ParameterCount = indexAccessed;

    while(indexAccessed < CSMON_FILE_PARAMETER_COUNT_MAX)
    {
        CSMON_sFileParameterAccessList.u16ParameterIndex[indexAccessed++] = 0xFFFF;
    }
    HMDRV_vClrDebugPin();  /* AmberLed on 1038 */

}











/* *****************************************************************************
 * CSMON_u32GetRealAddressAndSizeOffsetFromParameter
 **************************************************************************** */
uint32_t CSMON_u32GetRealAddressAndSizeOffsetFromParameter(uint32_t variableID, HMDRV_BUFF_PNTR_MODIFIER HMDRV_U16* pu16RealOffset)
{
    int16_t s16Index;
    HMDRV_U16 addressModbusParam;
    HMDRV_ADDR addressRealParam;
    HMDRV_U16 sizeOffsetRealParam;
    HMDRV_U16 elemSizeRealParam;

    #if HMMODBUS_COILS_FROM_HOLDING_REGS == HMDRV_FALSE
    HMDRV_uVariableSizeOffset_t tempSize;
    #endif

    /* Get Modbus Address From CSMON ID */
    s16Index = CSMON_u16FindIndexParameterTable(variableID);

    if ((s16Index >= 0) && (CSMON_pasFileParameterList != NULL))
    {
        addressModbusParam = CSMON_pasFileParameterList[s16Index].ModbusAddress;
        switch (CSMON_pasFileParameterList[s16Index].eDefaultReadFunctionCode)
        {
            case HMMODBUS_CMD_RD_DISCRETE_INPUTS:
                #if (HMMODBUS_DISCRETE_INPUTS_FROM_COILS == HMDRV_FALSE) && (HMMODBUS_COILS_FROM_HOLDING_REGS == HMDRV_FALSE)
                addressRealParam = (HMDRV_ADDR)HMMODBUS_GetRealCoilAddressAndOffset(addressModbusParam, &sizeOffsetRealParam, HMMODBUS_REG_COIL_FULL_ACCESS_READ_INPUT);//HMDRV_TRUE, HMDRV_TRUE
                tempSize.u8Register = (HMDRV_U8)sizeOffsetRealParam;
                tempSize.sOffset.bBitField = 1;
                sizeOffsetRealParam = (HMDRV_U16)(HMDRV_U8)tempSize.u8Register;
                #endif
                break;
            case HMMODBUS_CMD_RD_COILS:
                #if HMMODBUS_COILS_FROM_HOLDING_REGS == HMDRV_FALSE
                addressRealParam = (HMDRV_ADDR)HMMODBUS_GetRealCoilAddressAndOffset(addressModbusParam, &sizeOffsetRealParam, HMMODBUS_REG_COIL_FULL_ACCESS_RD_HOLDING);//HMDRV_TRUE, HMDRV_FALSE
                tempSize.u8Register = (HMDRV_U8)sizeOffsetRealParam;
                tempSize.sOffset.bBitField = 1;
                sizeOffsetRealParam = (HMDRV_U16)(HMDRV_U8)tempSize.u8Register;
                #endif
                break;
            case HMMODBUS_CMD_RD_INPUT_REGS:
                #if HMMODBUS_INPUT_REGS_FROM_HOLDING_REGS == HMDRV_FALSE
                addressRealParam = (HMDRV_ADDR)HMMODBUS_GetRealRegisterAddressAndSize(addressModbusParam, &sizeOffsetRealParam, &elemSizeRealParam, HMMODBUS_REG_COIL_FULL_ACCESS_READ_INPUT);//HMDRV_TRUE, HMDRV_TRUE
                if ((elemSizeRealParam & 0x8000) > 0) /* bit field */
                {
                    sizeOffsetRealParam = (HMDRV_SIZE8)elemSizeRealParam & 0x00FF;
                    elemSizeRealParam = (HMDRV_SIZE8)elemSizeRealParam & 0x00FF;
                }
                else
                {
                    sizeOffsetRealParam = (HMDRV_SIZE8)sizeOffsetRealParam & 0x00FF;
                    elemSizeRealParam = (HMDRV_SIZE8)elemSizeRealParam & 0x00FF;
                }
                #endif
                break;
            case HMMODBUS_CMD_RD_HOLD_REGS:
                addressRealParam = (HMDRV_ADDR)HMMODBUS_GetRealRegisterAddressAndSize(addressModbusParam, &sizeOffsetRealParam, &elemSizeRealParam, HMMODBUS_REG_COIL_FULL_ACCESS_RD_HOLDING);//HMDRV_TRUE, HMDRV_FALSE
                if ((elemSizeRealParam & 0x8000) > 0) /* bit field */
                {
                    sizeOffsetRealParam = (HMDRV_SIZE8)elemSizeRealParam & 0x00FF;
                    elemSizeRealParam = (HMDRV_SIZE8)elemSizeRealParam & 0x00FF;
                }
                else
                {
                    sizeOffsetRealParam = (HMDRV_SIZE8)sizeOffsetRealParam & 0x00FF;
                    elemSizeRealParam = (HMDRV_SIZE8)elemSizeRealParam & 0x00FF;
                }
                break;
            default:
                addressRealParam = NULL;
                sizeOffsetRealParam = 0;                   
                break;
        }


    }
    else
    {
        /* null trigger Modbus address */
        addressRealParam = NULL;
        sizeOffsetRealParam = 0;
    }
    
    *pu16RealOffset = (HMDRV_U16)sizeOffsetRealParam;

    return (uint32_t)addressRealParam;
}



/* *****************************************************************************
 * CSMON_vAutorizationReset
 **************************************************************************** */
void CSMON_vAutorizationReset(void)
{
    if (CSMON_sFileAutorization.u16AccessLevelNow != CSMON_sFileAutorization.u16AccessLevelReq)    /* New Request */
    {
        if (CSMON_bAutorizationPasswordCheck())
        {
            CSMON_sFileAutorization.u16AccessLevelNow = CSMON_sFileAutorization.u16AccessLevelReq;
            CSMON_u16CurrentAccessLevelPrev = CSMON_ACCESS_INIT;    /* Force Access Setup */
        }
        else
        {
            if (CSMON_sFileAutorization.u16AccessLevelNow != CSMON_ACCESS_DENIED)
            {
                CSMON_sFileAutorization.u16AccessLevelNow = CSMON_ACCESS_DENIED;
                CSMON_u16CurrentAccessLevelPrev = CSMON_ACCESS_INIT;    /* Force Access Setup */
            }
        }
    }
    else
    {
        if (CSMON_bAutorizationPasswordCheck() == 0)
        {
            if (CSMON_sFileAutorization.u16AccessLevelNow != CSMON_ACCESS_DENIED)
            {
                CSMON_sFileAutorization.u16AccessLevelNow = CSMON_ACCESS_DENIED;
                CSMON_u16CurrentAccessLevelPrev = CSMON_ACCESS_INIT;    /* Force Access Setup */
            }
        }
    }
}


/* *****************************************************************************
 * CSMON_u16FindIndexParameterTable
 **************************************************************************** */
int16_t CSMON_u16FindIndexParameterTable(uint32_t variableID)
{
    int16_t s16Index = -1;
    uint16_t index;

    if (CSMON_pasFileParameterList != NULL)
    {
        for (index = 0 ; index < CSMON_u16FileParameterFillCount; index++)
        {
            if (variableID == CSMON_pasFileParameterList[index].ValueAccessKey.u32Register)
            {
                s16Index = index;
                break;
            }
        }
    }
    return s16Index;
}


/* *****************************************************************************
 * CSMON_vInitHoldingRegistersTableChanges
 **************************************************************************** */
void CSMON_vInitHoldingRegistersTableChanges(void)
{
    MODBUSREG_vSetParamCount(CSMON_u16FileParameterFillCount);  /* fixed MODBUSREG_u16ParamCount */
    MODBUSREG_AddressTablesInit();  /* Called to fix CSMON_u16FileParameterFillCount changes (MODBUSREG_u16ParamCount) in file tables */
    CSMON_vSetupHoldingRegistersAndCoilsAddressTableFromCSMONParameterList(CSMON_sFileAutorization.u16AccessLevelNow);
    CSMON_vSetupInputRegistersAndCoilsAddressTableFromCSMONParameterList(CSMON_sFileAutorization.u16AccessLevelNow);
}

/* *****************************************************************************
 * CSMON_u32GetFreeRunningTimerMicroseconds
 **************************************************************************** */
uint32_t CSMON_u32GetFreeRunningTimerMicroseconds(void)
{
    uint32_t u32Result;

    u32Result = HMDRV_GET_FREE_RUN_TIMER();
    #if HMDRV_FREE_RUN_TIMER_DECREMENT_COUNTER
    u32Result = 0 - u32Result;
    #endif
    if (u32FreeRunTimerSysClkMHz != u32FreeRunTimerSysClkDiv)   /* 1:1 Divider and 1Mhz Clock (or equal SysClkDiv and SysClkMHz) -> Not needed divide and multiply  */
    {
        u32Result = (uint32_t)((uint64_t)(u32Result * u32FreeRunTimerSysClkDiv) / u32FreeRunTimerSysClkMHz);
    }

    return u32Result;
}

/* *****************************************************************************
 * CSMON_u32GetFreeRunningTimerTicks
 **************************************************************************** */
uint32_t CSMON_u32GetFreeRunningTimerTicks(void)
{
    uint32_t u32Result;

    u32Result = HMDRV_GET_FREE_RUN_TIMER();
    #if HMDRV_FREE_RUN_TIMER_DECREMENT_COUNTER
    u32Result = 0 - u32Result;
    #endif
    return u32Result;
}



/* *****************************************************************************
 * CSMON_vSetupHoldingRegistersAndCoilsAddressTableAccessAll
 **************************************************************************** */
void CSMON_vSetupHoldingRegistersAndCoilsAddressTableAccessAll(void)
{
    HMDRV_U16 indexAddressTable;
    HMDRV_U16 u16HoldRegCount;

    HMDRV_vSetDebugPin();  /* AmberLed on 1038 */


    u16HoldRegCount = MODBUSREG_u16GetHoldRegFillCount();
    HMMODBUS_sRegisterAccessTableInRam_t* pTableInRam = (HMMODBUS_sRegisterAccessTableInRam_t*)(HMDRV_ADDR_TO_PNTR_SIZE)MODBUSREG_u32GetRegisterAccessTableInRam();
    HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sRegisterAddressTable_t* pTable = (HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sRegisterAddressTable_t*)(HMDRV_ADDR_SIZE_LONG)MODBUSREG_u32GetRegisterAddressTable();

    if ((pTable != NULL) && (pTableInRam != NULL))
    {
        for (indexAddressTable = 0; indexAddressTable < u16HoldRegCount; indexAddressTable++)
        {
            pTableInRam[indexAddressTable].uFlags.sFlags.bReadDisabled  = pTable[indexAddressTable].uFlags.sFlags.bWriteOnly ^ 1;
            pTableInRam[indexAddressTable].uFlags.sFlags.bWriteDisabled = pTable[indexAddressTable].uFlags.sFlags.bReadOnly ^ 1;
        }
    }

    HMDRV_U16 u16HoldingCoilCount = MODBUSREG_u16GetHoldingCoilUsedCount(); /* use Fill if not read only table */
    HMMODBUS_sCoilAddressTableInRam_t* pTableCoilInRam =(HMMODBUS_sCoilAddressTableInRam_t*)(HMDRV_ADDR_TO_PNTR_SIZE)MODBUSREG_u32GetHoldingCoilAddressTableInRam();
    HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sCoilAddressTable_t* pTableCoil = (HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sCoilAddressTable_t*)(HMDRV_ADDR_SIZE_LONG)MODBUSREG_u32GetHoldingCoilAddressTable();

    if ((pTableCoil != NULL) && (pTableCoilInRam != NULL))
    {
        for (indexAddressTable = 0; indexAddressTable < u16HoldingCoilCount; indexAddressTable++)
        {
            pTableCoilInRam[indexAddressTable].uFlags.sFlags.bReadDisabled  = pTableCoil[indexAddressTable].uFlags.sFlags.bWriteOnly ^ 1;
            pTableCoilInRam[indexAddressTable].uFlags.sFlags.bWriteDisabled = pTableCoil[indexAddressTable].uFlags.sFlags.bReadOnly ^ 1;
        }
    }
    HMDRV_vClrDebugPin();  /* AmberLed on 1038 */

}



/* *****************************************************************************
 * CSMON_vSetupInputRegistersAndCoilsAddressTableAccessAll
 **************************************************************************** */
void CSMON_vSetupInputRegistersAndCoilsAddressTableAccessAll(void)
{
    HMDRV_U16 indexAddressTable;

    HMDRV_vSetDebugPin();  /* AmberLed on 1038 */


    HMDRV_U16 u16InputRegisterCount = MODBUSREG_u16GetInputRegisterUsedCount(); /* change with FillCount if not read only */
    HMMODBUS_sRegisterAccessTableInRam_t* pTableInRam = (HMMODBUS_sRegisterAccessTableInRam_t*)(HMDRV_ADDR_TO_PNTR_SIZE)MODBUSREG_u32GetInputRegisterAccessTableInRam();
    HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sRegisterAddressTable_t* pTable = (HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sRegisterAddressTable_t*)(HMDRV_ADDR_SIZE_LONG)MODBUSREG_u32GetInputRegisterAddressTable();

    if ((pTable != NULL) && (pTableInRam != NULL))
    {
        for (indexAddressTable = 0; indexAddressTable < u16InputRegisterCount; indexAddressTable++)
        {
            pTableInRam[indexAddressTable].uFlags.sFlags.bReadDisabled  = pTable[indexAddressTable].uFlags.sFlags.bWriteOnly ^ 1;
        }
    }

    HMDRV_U16 u16InputCoilCount = MODBUSREG_u16GetInputCoilUsedCount(); /* change with FillCount if not read only */
    HMMODBUS_sCoilAddressTableInRam_t* pTableCoilInRam = (HMMODBUS_sCoilAddressTableInRam_t*)(HMDRV_ADDR_TO_PNTR_SIZE)MODBUSREG_u32GetInputCoilAddressTableInRam();
    HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sCoilAddressTable_t* pTableCoil = (HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sCoilAddressTable_t*)(HMDRV_ADDR_SIZE_LONG)MODBUSREG_u32GetInputCoilAddressTable();

    if ((pTableCoil != NULL) && (pTableCoilInRam != NULL))
    {
        for (indexAddressTable = 0; indexAddressTable < u16InputCoilCount; indexAddressTable++)
        {
            pTableCoilInRam[indexAddressTable].uFlags.sFlags.bReadDisabled  = pTableCoil[indexAddressTable].uFlags.sFlags.bWriteOnly ^ 1;
        }
    }
    HMDRV_vClrDebugPin();  /* AmberLed on 1038 */

}


/* *****************************************************************************
 * CSMON_vSetupHoldingRegistersAndCoilsAddressTableFromCSMONParameterList
 **************************************************************************** */
void CSMON_vSetupHoldingRegistersAndCoilsAddressTableFromCSMONParameterList(HMDRV_U16 u16CurrentAccessLevel)
{
    HMDRV_BOOL bFound;
    //HMDRV_U16 addressModbusParam;
    //HMDRV_U8 sizeModbusParam;
    HMDRV_U8 readFuncCode;
    HMDRV_U8 writeFuncCode;
    HMDRV_U16 addressModbus;
    HMDRV_U16 indexAddressTable;
    HMDRV_U16 indexParameterList;
    HMDRV_U16 indexParameterListNext;
    HMDRV_U16 indexParameterListLoop;

    HMDRV_vSetDebugPin();  /* AmberLed on 1038 */


    HMDRV_U16 u16HoldRegCount = MODBUSREG_u16GetHoldRegFillCount();
    HMMODBUS_sRegisterAccessTableInRam_t* pTableInRam = (HMMODBUS_sRegisterAccessTableInRam_t*)(HMDRV_ADDR_TO_PNTR_SIZE)MODBUSREG_u32GetRegisterAccessTableInRam();
    HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sRegisterAddressTable_t* pTable = (HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sRegisterAddressTable_t*)(HMDRV_ADDR_SIZE_LONG)MODBUSREG_u32GetRegisterAddressTable();

    if ((pTable != NULL) && (pTableInRam != NULL))
    {
        indexParameterList = 0;
        indexParameterListNext = 0;

        for (indexAddressTable = 0; indexAddressTable < u16HoldRegCount; indexAddressTable++)
        {
            
            addressModbus = pTable[indexAddressTable].u16VirtAddress;
            bFound = HMDRV_FALSE;
            if (CSMON_pasFileParameterList != NULL)
            {
            for (indexParameterListLoop = 0; indexParameterListLoop < CSMON_u16FileParameterFillCount; indexParameterListLoop++)
            {
                indexParameterList = indexParameterListNext;
                if (indexParameterList >= CSMON_u16FileParameterFillCount)
                {
                    indexParameterList = 0;
                }
                indexParameterListNext = indexParameterList + 1;

                //addressModbusParam = CSMON_pasFileParameterList[indexParameterList].ModbusAddress;
                if (addressModbus == CSMON_pasFileParameterList[indexParameterList].ModbusAddress)
                {

                //sizeModbusParam = CSMON_pasFileParameterList[indexParameterList].u8BitsCount;
                readFuncCode = CSMON_pasFileParameterList[indexParameterList].eDefaultReadFunctionCode;
                writeFuncCode = CSMON_pasFileParameterList[indexParameterList].eDefaultWriteFunctionCode;

                if (       (readFuncCode == HMMODBUS_CMD_RD_HOLD_REGS)
                        || (writeFuncCode == HMMODBUS_CMD_WR_HOLD_REGS)
                        || (writeFuncCode == HMMODBUS_CMD_WR_HOLD_REG)
                        || (writeFuncCode == HMMODBUS_CMD_WR_HOLD_REG)
                        || (writeFuncCode == HMMODBUS_CMD_RW_HOLD_REGS)
                        || (readFuncCode == HMMODBUS_CMD_RW_HOLD_REGS)
                        || (writeFuncCode == HMMODBUS_CMD_WR_HOLD_REG_MASK)
                        || (readFuncCode == HMMODBUS_CMD_RD_FILE_RECORD)
                        || (writeFuncCode == HMMODBUS_CMD_WR_FILE_RECORD)
                        #if HMMODBUS_INPUT_REGS_FROM_HOLDING_REGS
                        || (readFuncCode == HMMODBUS_CMD_RD_INPUT_REGS)
                        #endif
                        #if HMMODBUS_COILS_FROM_HOLDING_REGS
                        #if HMMODBUS_DISCRETE_INPUTS_FROM_COILS
                        || (readFuncCode == HMMODBUS_CMD_RD_DISCRETE_INPUTS)
                        #endif
                        || (readFuncCode == HMMODBUS_CMD_RD_COILS)
                        || (writeFuncCode == HMMODBUS_CMD_WR_COILS)
                        || (writeFuncCode == HMMODBUS_CMD_WR_COIL)
                        #endif
                   )
                {
                    /* Read */
                    if ( (readFuncCode == 0)
                      || (CSMON_asFileParameterDefinitionsFromAccessLevel[indexParameterList].eDefaultReadFunctionCode == 0)
                      || (CSMON_pasFileParameterList[indexParameterList].eReadAccessLevelMin > u16CurrentAccessLevel)
                      || (pTable[indexAddressTable].uFlags.sFlags.bWriteOnly == 1))
                    {
                        pTableInRam[indexAddressTable].uFlags.sFlags.bReadDisabled = 1;
                    }
                    else
                    {
                        pTableInRam[indexAddressTable].uFlags.sFlags.bReadDisabled = 0;
                    }
                    /* Write */
                    if ( (writeFuncCode == 0)
                      || (CSMON_asFileParameterDefinitionsFromAccessLevel[indexParameterList].eDefaultWriteFunctionCode == 0)
                      || (CSMON_pasFileParameterList[indexParameterList].eWriteAccessLevelMin > u16CurrentAccessLevel)
                      || (pTable[indexAddressTable].uFlags.sFlags.bReadOnly == 1))
                    {
                        pTableInRam[indexAddressTable].uFlags.sFlags.bWriteDisabled = 1;
                    }
                    else
                    {
                        pTableInRam[indexAddressTable].uFlags.sFlags.bWriteDisabled = 0;
                    }

                    bFound = HMDRV_TRUE;
                    break;
                }
                }
            }
            }
            /* deny All NonCSMON parameters */
            if (bFound == HMDRV_FALSE)
            {
                if (pTable[indexAddressTable].uFlags.sFlags.bWriteDenySkipCSMON)
                {
                    if (pTable[indexAddressTable].uFlags.sFlags.bReadOnly == 1)
                    {
                      pTableInRam[indexAddressTable].uFlags.sFlags.bWriteDisabled = 1;
                    }
                    else
                    {
                      pTableInRam[indexAddressTable].uFlags.sFlags.bWriteDisabled = 0;
                    }
                }
                else
                {
                    pTableInRam[indexAddressTable].uFlags.sFlags.bWriteDisabled = 1;
                }

                if (pTable[indexAddressTable].uFlags.sFlags.bReadDenySkipCSMON)
                {
                    if (pTable[indexAddressTable].uFlags.sFlags.bWriteOnly == 1)
                    {
                      pTableInRam[indexAddressTable].uFlags.sFlags.bReadDisabled = 1;
                    }
                    else
                    {
                      pTableInRam[indexAddressTable].uFlags.sFlags.bReadDisabled = 0;
                    }
                }
                else
                {
                    pTableInRam[indexAddressTable].uFlags.sFlags.bReadDisabled = 1;
                }
            }
        }

    }

    HMDRV_U16 u16HoldCoilCount = MODBUSREG_u16GetHoldingCoilUsedCount(); /* change with FillCount if not read only */
    HMMODBUS_sCoilAddressTableInRam_t* pTableCoilInRam =(HMMODBUS_sCoilAddressTableInRam_t*)(HMDRV_ADDR_TO_PNTR_SIZE)MODBUSREG_u32GetHoldingCoilAddressTableInRam();
    HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sCoilAddressTable_t* pTableCoil = (HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sCoilAddressTable_t*)(HMDRV_ADDR_SIZE_LONG)MODBUSREG_u32GetHoldingCoilAddressTable();

    if ((pTableCoil != NULL) && (pTableCoilInRam != NULL))
    {
        for (indexAddressTable = 0; indexAddressTable < u16HoldCoilCount; indexAddressTable++)
        {
            addressModbus = pTableCoil[indexAddressTable].u16VirtAddress;
            bFound = HMDRV_FALSE;
            if (CSMON_pasFileParameterList != NULL)
            {
            for (indexParameterList = 0; indexParameterList < CSMON_u16FileParameterFillCount; indexParameterList++)
            {
                //addressModbusParam = CSMON_pasFileParameterList[indexParameterList].ModbusAddress;
                if (addressModbus == CSMON_pasFileParameterList[indexParameterList].ModbusAddress)
                {
                //sizeModbusParam = CSMON_pasFileParameterList[indexParameterList].u8BitsCount;
                readFuncCode = CSMON_pasFileParameterList[indexParameterList].eDefaultReadFunctionCode;
                writeFuncCode = CSMON_pasFileParameterList[indexParameterList].eDefaultWriteFunctionCode;

                if (       (readFuncCode == HMMODBUS_CMD_RD_COILS)
                        || (writeFuncCode == HMMODBUS_CMD_WR_COILS)
                        || (writeFuncCode == HMMODBUS_CMD_WR_COIL)
                        || (readFuncCode == HMMODBUS_CMD_RD_FILE_RECORD)
                        || (writeFuncCode == HMMODBUS_CMD_WR_FILE_RECORD)
                   )
                {
                    /* Read */
                    if ( (readFuncCode == 0)
                      || (CSMON_asFileParameterDefinitionsFromAccessLevel[indexParameterList].eDefaultReadFunctionCode == 0)
                      || (CSMON_pasFileParameterList[indexParameterList].eReadAccessLevelMin > u16CurrentAccessLevel)
                      || (pTableCoil[indexAddressTable].uFlags.sFlags.bWriteOnly == 1))
                    {
                        pTableCoilInRam[indexAddressTable].uFlags.sFlags.bReadDisabled = 1;
                    }
                    else
                    {
                        pTableCoilInRam[indexAddressTable].uFlags.sFlags.bReadDisabled = 0;
                    }
                    /* Write */
                    if ( (writeFuncCode == 0)
                      || (CSMON_asFileParameterDefinitionsFromAccessLevel[indexParameterList].eDefaultWriteFunctionCode == 0)
                      || (CSMON_pasFileParameterList[indexParameterList].eWriteAccessLevelMin > u16CurrentAccessLevel)
                      || (pTableCoil[indexAddressTable].uFlags.sFlags.bReadOnly == 1))
                    {
                        pTableCoilInRam[indexAddressTable].uFlags.sFlags.bWriteDisabled = 1;
                    }
                    else
                    {
                        pTableCoilInRam[indexAddressTable].uFlags.sFlags.bWriteDisabled = 0;
                    }

                    bFound = HMDRV_TRUE;
                    break;
                }
                }
            }
            }
            /* deny All NonCSMON parameters */
            if (bFound == HMDRV_FALSE)
            {
                //pTableCoilInRam[indexAddressTable].uFlags.sFlags.bWriteDisabled = 1;
                //pTableCoilInRam[indexAddressTable].uFlags.sFlags.bReadDisabled = 1;
                if (pTableCoil[indexAddressTable].uFlags.sFlags.bWriteDenySkipCSMON)
                {
                    if (pTableCoil[indexAddressTable].uFlags.sFlags.bReadOnly == 1)
                    {
                      pTableCoilInRam[indexAddressTable].uFlags.sFlags.bWriteDisabled = 1;
                    }
                    else
                    {
                      pTableCoilInRam[indexAddressTable].uFlags.sFlags.bWriteDisabled = 0;
                    }
                }
                else
                {
                    pTableCoilInRam[indexAddressTable].uFlags.sFlags.bWriteDisabled = 1;
                }

                if (pTableCoil[indexAddressTable].uFlags.sFlags.bReadDenySkipCSMON)
                {
                    if (pTableCoil[indexAddressTable].uFlags.sFlags.bWriteOnly == 1)
                    {
                      pTableCoilInRam[indexAddressTable].uFlags.sFlags.bReadDisabled = 1;
                    }
                    else
                    {
                      pTableCoilInRam[indexAddressTable].uFlags.sFlags.bReadDisabled = 0;
                    }
                }
                else
                {
                    pTableCoilInRam[indexAddressTable].uFlags.sFlags.bReadDisabled = 1;
                }
            }
        }
    }
    HMDRV_vClrDebugPin();  /* AmberLed on 1038 */
}


/* *****************************************************************************
 * CSMON_vSetupInputRegistersAndCoilsAddressTableFromCSMONParameterList
 **************************************************************************** */
void CSMON_vSetupInputRegistersAndCoilsAddressTableFromCSMONParameterList(HMDRV_U16 u16CurrentAccessLevel)
{
    HMDRV_BOOL bFound;
    //HMDRV_U16 addressModbusParam;
    //HMDRV_U8 sizeModbusParam;
    HMDRV_U8 readFuncCode;
    //HMDRV_U8 writeFuncCode;
    HMDRV_U16 addressModbus;
    HMDRV_U32 indexAddressTable;
    HMDRV_U32 indexParameterList;

    HMDRV_vSetDebugPin();  /* AmberLed on 1038 */


    HMDRV_U16 u16InputRegisterCount = MODBUSREG_u16GetInputRegisterUsedCount(); /* change with FillCount if not read only */
    HMMODBUS_sRegisterAccessTableInRam_t* pTableInRam = (HMMODBUS_sRegisterAccessTableInRam_t*)(HMDRV_ADDR_TO_PNTR_SIZE)MODBUSREG_u32GetInputRegisterAccessTableInRam();
    HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sRegisterAddressTable_t* pTable = (HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sRegisterAddressTable_t*)(HMDRV_ADDR_SIZE_LONG)MODBUSREG_u32GetInputRegisterAddressTable();

    if ((pTable != NULL) && (pTableInRam != NULL))
    {
        for (indexAddressTable = 0; indexAddressTable < u16InputRegisterCount; indexAddressTable++)
        {
            addressModbus = pTable[indexAddressTable].u16VirtAddress;
            bFound = HMDRV_FALSE;
            if (CSMON_pasFileParameterList != NULL)
            {
            for (indexParameterList = 0; indexParameterList < CSMON_u16FileParameterFillCount; indexParameterList++)
            {

                //addressModbusParam = CSMON_pasFileParameterList[indexParameterList].ModbusAddress;
                if (addressModbus == CSMON_pasFileParameterList[indexParameterList].ModbusAddress)
                {
                //sizeModbusParam = CSMON_pasFileParameterList[indexParameterList].u8BitsCount;

                readFuncCode = CSMON_pasFileParameterList[indexParameterList].eDefaultReadFunctionCode;
                //writeFuncCode = CSMON_pasFileParameterList[indexParameterList].eDefaultWriteFunctionCode;

                if (       (readFuncCode == HMMODBUS_CMD_RD_INPUT_REGS)
                        || (readFuncCode == HMMODBUS_CMD_RD_FILE_RECORD)
                   )
                {
                    /* Read */
                    if ( (readFuncCode == 0)
                      || (CSMON_asFileParameterDefinitionsFromAccessLevel[indexParameterList].eDefaultReadFunctionCode == 0)
                      || (CSMON_pasFileParameterList[indexParameterList].eReadAccessLevelMin > u16CurrentAccessLevel)
                      || (pTable[indexAddressTable].uFlags.sFlags.bWriteOnly == 1))
                    {
                        pTableInRam[indexAddressTable].uFlags.sFlags.bReadDisabled = 1;
                    }
                    else
                    {
                        pTableInRam[indexAddressTable].uFlags.sFlags.bReadDisabled = 0;
                    }

                    bFound = HMDRV_TRUE;
                    break;
                }
                }
            }
            }
            /* deny All NonCSMON parameters */
            if (bFound == HMDRV_FALSE)
            {
                //pTableInRam[indexAddressTable].uFlags.sFlags.bWriteDisabled = 1;  not needed for inputs only
                pTableInRam[indexAddressTable].uFlags.sFlags.bReadDisabled = 1;
                if (pTable[indexAddressTable].uFlags.sFlags.bReadDenySkipCSMON)
                {
                    if (pTable[indexAddressTable].uFlags.sFlags.bWriteOnly == 1)
                    {
                      pTableInRam[indexAddressTable].uFlags.sFlags.bReadDisabled = 1;
                    }
                    else
                    {
                      pTableInRam[indexAddressTable].uFlags.sFlags.bReadDisabled = 0;
                    }
                }
                else
                {
                    pTableInRam[indexAddressTable].uFlags.sFlags.bReadDisabled = 1;
                }
            }
        }

    }

    HMDRV_U16 u16InputCoilCount = MODBUSREG_u16GetInputCoilUsedCount(); /* change with FillCount if not read only */
    HMMODBUS_sCoilAddressTableInRam_t* pTableCoilInRam = (HMMODBUS_sCoilAddressTableInRam_t*)(HMDRV_ADDR_TO_PNTR_SIZE)MODBUSREG_u32GetInputCoilAddressTableInRam();
    HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sCoilAddressTable_t* pTableCoil = (HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sCoilAddressTable_t*)(HMDRV_ADDR_SIZE_LONG)MODBUSREG_u32GetInputCoilAddressTable();

    if ((pTableCoil != NULL) && (pTableCoilInRam != NULL))
    {
        for (indexAddressTable = 0; indexAddressTable < u16InputCoilCount; indexAddressTable++)
        {
            addressModbus = pTableCoil[indexAddressTable].u16VirtAddress;
            bFound = HMDRV_FALSE;
            if (CSMON_pasFileParameterList != NULL)
            {
            for (indexParameterList = 0; indexParameterList < CSMON_u16FileParameterFillCount; indexParameterList++)
            {
                //addressModbusParam = CSMON_pasFileParameterList[indexParameterList].ModbusAddress;
                if (addressModbus == CSMON_pasFileParameterList[indexParameterList].ModbusAddress)
                {
                //sizeModbusParam = CSMON_pasFileParameterList[indexParameterList].u8BitsCount;

                readFuncCode = CSMON_pasFileParameterList[indexParameterList].eDefaultReadFunctionCode;
                //writeFuncCode = CSMON_pasFileParameterList[indexParameterList].eDefaultWriteFunctionCode;

                if (       (readFuncCode == HMMODBUS_CMD_RD_DISCRETE_INPUTS)
                        || (readFuncCode == HMMODBUS_CMD_RD_FILE_RECORD)
                   )

                {
                    /* Read */
                    if ( (readFuncCode == 0)
                      || (CSMON_asFileParameterDefinitionsFromAccessLevel[indexParameterList].eDefaultReadFunctionCode == 0)
                      || (CSMON_pasFileParameterList[indexParameterList].eReadAccessLevelMin > u16CurrentAccessLevel)
                      || (pTableCoil[indexAddressTable].uFlags.sFlags.bWriteOnly == 1))
                    {
                        pTableCoilInRam[indexAddressTable].uFlags.sFlags.bReadDisabled = 1;
                    }
                    else
                    {
                        pTableCoilInRam[indexAddressTable].uFlags.sFlags.bReadDisabled = 0;
                    }

                    bFound = HMDRV_TRUE;
                    break;
                }
                }
            }
            }
            /* deny All NonCSMON parameters */
            if (bFound == HMDRV_FALSE)
            {
                //pTableCoilInRam[indexAddressTable].uFlags.sFlags.bWriteDisabled = 1;  not needed for inputs only
                pTableCoilInRam[indexAddressTable].uFlags.sFlags.bReadDisabled = 1;
                if (pTableCoil[indexAddressTable].uFlags.sFlags.bReadDenySkipCSMON)
                {
                    if (pTableCoil[indexAddressTable].uFlags.sFlags.bWriteOnly == 1)
                    {
                      pTableCoilInRam[indexAddressTable].uFlags.sFlags.bReadDisabled = 1;
                    }
                    else
                    {
                      pTableCoilInRam[indexAddressTable].uFlags.sFlags.bReadDisabled = 0;
                    }
                }
                else
                {
                    pTableCoilInRam[indexAddressTable].uFlags.sFlags.bReadDisabled = 1;
                }
            }

        }
    }
    HMDRV_vClrDebugPin();  /* AmberLed on 1038 */

}







/* Parameter Scope Support */
#if (HMDRV_USE_SCOPE_PARAMETERS) && (!(HMDRV_DISABLE))
#if HMDRV_SCOPE_PARAM_HANDLE_COUNT > 0
/* *****************************************************************************
 * CSMON_eSetUpScopeParam
 *
 * Description: Setup Parameter Scope
 **************************************************************************** */
HMSCOPEPRM_eResponseCode_t CSMON_eSetUpScopeParam(HMDRV_U16 u16ScopeIndex)
{
    HMSCOPEPRM_sConfig_t*       sConfig = &sScopeConfigParam[u16ScopeIndex];
    HMSCOPEPRM_sHandle_t*       sHandle = &sScopeHandlePrm[u16ScopeIndex];
    HMDRV_U8 i, sz, nVarCnt;
    HMDRV_U16 nRecVarsetSize;
    HMSCOPEPRM_eResponseCode_t eResponseCode = HMSCOPEPRM_CODE_OK;

    HMDRV_ADDR addressRealParam;
    HMDRV_U16 sizeOffsetRealParam;


    /* uninitialize scope */
    sHandle->u16ScopeVarCount = 0U;

    /* scope variable count  */
    nVarCnt = (HMDRV_U8)(sConfig->nCountVariables);

    /* scope variable information must fit into our buffers */
    if(!nVarCnt || nVarCnt > (HMDRV_U8)HMSCOPEPRM_MAX_VARS)
    {
        eResponseCode = HMSCOPEPRM_CODE_INVBUFF;
    }
    else
    {
        nRecVarsetSize = 0;
        /* get all addresses and sizes */
        for(i=0U; i<nVarCnt; i++)
        {
            /* Get Address and Size/Offset From CSMON ID */
            addressRealParam = (HMDRV_ADDR)CSMON_u32GetRealAddressAndSizeOffsetFromParameter(sConfig->nVariableIDScope[i],  &sizeOffsetRealParam);

            /* variable size */
            sHandle->nScopeVarSize[i].u8Register = sizeOffsetRealParam;
            if (((HMDRV_uVariableSizeOffset_t)(HMDRV_U8)sizeOffsetRealParam).sOffset.bBitField == 1)
            {
                nRecVarsetSize += 1;
            }
            else
            {
                nRecVarsetSize += ((HMDRV_uVariableSizeOffset_t)(HMDRV_U8)sizeOffsetRealParam).sSize.u7ValueSize;
            }

            /* variable address */
            sHandle->pScopeVarAddr[i] = addressRealParam;

            /* valid numeric variable sizes only */
            sz = ((HMDRV_uVariableSizeOffset_t)(HMDRV_U8)sizeOffsetRealParam).sSize.u7ValueSize;
            if(sz == 0U || sz > 8U)
            {
                eResponseCode = HMSCOPEPRM_CODE_INVSIZE;
                i = nVarCnt;
            }

            #if HMDRV_CFG_BUS_WIDTH > 1U
            /* even sizes only */
            if(sz & 0x1)
            {
                ???limitation fix
                eResponseCode = HMSCOPEPRM_CODE_INVSIZE;
                i = nVarCnt;
            }
            #endif

            if (nRecVarsetSize > HMDRV_HMSCOPEPRM_BUFFER_SIZE)
            {
                eResponseCode = HMSCOPEPRM_CODE_INVSIZE;
                i = nVarCnt;
            }
        }

        /* activate scope */
        if(eResponseCode == HMSCOPEPRM_CODE_OK)
        {
            sHandle->u16ScopeVarCount = nVarCnt;
        }
    }
    /* return just a status */
    return eResponseCode;
 }

#endif
#endif








/* Parameter Recorder Support */
#if (HMDRV_USE_RECORDER_PARAMETERS) && (!(HMDRV_DISABLE))
#if HMDRV_RECORDER_PARAM_HANDLE_COUNT > 0

/* *****************************************************************************
 * CSMON_eSetUpRecParam
 *
 * Description: Setup Parameter Recorder
 **************************************************************************** */
HMRECPRM_eResponseCode_t CSMON_eSetUpRecParam(HMDRV_U16 u16RecorderIndex)
{
    HMRECPRM_sHandle_t* sHandle = &sRecHandleParam[u16RecorderIndex];
    HMRECPRM_sConfig_t* sConfig = &sRecConfigParam[u16RecorderIndex];
    HMRECPRM_sStatus_t* sStatus = &sRecStatusParam[u16RecorderIndex];

    HMRECPRM_eResponseCode_t eResponseCode = HMRECPRM_CODE_OK;
    volatile HMDRV_SIZE8 nRecVarsetSize;
    volatile HMDRV_SIZE8 nRecVarCurrSize;
    //HMDRV_S16 s16Index;
    volatile HMDRV_SIZE_FLEX blen;
    //HMDRV_BOOL bInputReg;
    //HMDRV_BOOL bCoil;
    volatile HMDRV_U8 i, sz;


    volatile HMDRV_ADDR addressRealParam;
    HMDRV_U16 sizeOffsetRealParam;

    /* de-initialize first   */
    HMRECPRM_AbortRec(sHandle);

    /* check if buffer valid */
    if(!sHandle->pBufferAddr || !sHandle->nBufferSize)
    {
        eResponseCode = HMRECPRM_CODE_INVBUFF;
    }
    else
    {

        sHandle->uRecTotalSmps    = sConfig->nTotalCountSamples;

        /* seek the setup data */
        sHandle->uRecPostTrigger  = sConfig->nTotalCountSamples - sConfig->sTrigerConfig.nPreTriggerSamples;
        sHandle->u8RecTriggerMode   = (uint8_t )sConfig->sTrigerConfig.eTriggerMode;
        if ((HMDRV_SIZE8)sConfig->sTrigerConfig.sTriggerThreshold.eType & HMRECPRM_VARIANT_TYPE_FLOAT_FLAG)
        {
            if ((HMDRV_SIZE8)sConfig->sTrigerConfig.sTriggerThreshold.eType & HMRECPRM_VARIANT_TYPE_SIGNED_FLAG)
            {
                sHandle->u8TrgVarSigned = HMRECPRM_FLOAT_TRIG_MASK | HMRECPRM_TRIGGER_SIGNED;
            }
            else
            {
                sHandle->u8TrgVarSigned = HMRECPRM_FLOAT_TRIG_MASK | HMRECPRM_TRIGGER_UNSIGNED;
            }
        }
        else
        {
            if ((HMDRV_SIZE8)sConfig->sTrigerConfig.sTriggerThreshold.eType & HMRECPRM_VARIANT_TYPE_SIGNED_FLAG)
            {
                sHandle->u8TrgVarSigned = HMRECPRM_TRIGGER_SIGNED;
            }
            else
            {
                sHandle->u8TrgVarSigned = HMRECPRM_TRIGGER_UNSIGNED;
            }
        }

        sHandle->uTrgThreshold.u32  = sConfig->sTrigerConfig.sTriggerThreshold.eValue.u32;
        sHandle->u8RecVarCount      = (HMDRV_SIZE8)sConfig->sSampleFormat.nCountVariables;
        sHandle->u16RecTimeDiv      = (uint16_t)sConfig->sSampleRate.sSampleRateLookUp.nTimeDividerOrSamplesSkip;
        /* Get Address and Size/Offset From CSMON ID */
        addressRealParam = (HMDRV_ADDR)CSMON_u32GetRealAddressAndSizeOffsetFromParameter(sConfig->sTrigerConfig.nVariableIDTrigger,  &sizeOffsetRealParam);

        /* Fix Real Address From Modbus */
        addressRealParam = HMDRV_ADDRFROMPTR(addressRealParam);

        sHandle->pu8TrgVarAddr      = (HMDRV_ADDR)addressRealParam;
        sHandle->uTrgVarSize.u8Register       = (HMDRV_SIZE8)sizeOffsetRealParam;

        /* rec variable information must fit into our buffers */
        if(!sHandle->u8RecVarCount || sHandle->u8RecVarCount > (HMDRV_U8)HMRECPRM_MAX_VARS)
        {
            eResponseCode = HMRECPRM_CODE_INVBUFF;
        }
        else
        {
            /* calculate sum of sizes of all variables */
            nRecVarsetSize = 0U;

            /* get all addresses and sizes */
            for(i=0U; i<sHandle->u8RecVarCount; i++)
            {
                /* Get Address and Size/Offset From CSMON ID */
                addressRealParam = (HMDRV_ADDR)CSMON_u32GetRealAddressAndSizeOffsetFromParameter(sConfig->sSampleFormat.nVariableIDRecord[i],  &sizeOffsetRealParam);
                /* Fix Real Address From Modbus */
                addressRealParam = HMDRV_ADDRFROMPTR(addressRealParam);

                /* variable size */
                sHandle->uRecVarSize[i].u8Register = sizeOffsetRealParam;
                if (((HMDRV_uVariableSizeOffset_t)(HMDRV_U8)sizeOffsetRealParam).sOffset.bBitField == 1)
                {
                    nRecVarCurrSize = 1;
                    nRecVarsetSize += 1;
                }
                else
                {
                    nRecVarCurrSize = ((HMDRV_uVariableSizeOffset_t)(HMDRV_U8)sizeOffsetRealParam).sSize.u7ValueSize;
                    nRecVarsetSize += ((HMDRV_uVariableSizeOffset_t)(HMDRV_U8)sizeOffsetRealParam).sSize.u7ValueSize;
                }


                /* variable address */
                sHandle->pu8RecVarAddr[i] = addressRealParam;

                /* valid numeric variable sizes only */
                sz = nRecVarCurrSize;
                //sz = ((HMDRV_uVariableSizeOffset_t)(HMDRV_U8)sizeOffsetRealParam).sSize.u7ValueSize;
                if(sz == 0U || sz > 8U)
                {
                    eResponseCode = HMRECPRM_CODE_INVSIZE;
                    i = sHandle->u8RecVarCount;
                }

                #if HMDRV_CFG_BUS_WIDTH > 1U
                /* even sizes only */
                if(sz & 0x1) ??? consider try to fix this limitation - can be used for texas instruments
                {
                    eResponseCode = HMRECPRM_CODE_INVSIZE;
                    i = sHandle->u8RecVarCount;
                }
                #endif /* HMDRV_CFG_BUS_WIDTH > 1U */

            }
            sHandle->u8RecSampleSize = nRecVarsetSize;

            if (eResponseCode == HMRECPRM_CODE_OK)
            {

                /* any trigger? */
                sHandle->pfCompareFunc = NULL;
                if(sHandle->u8RecTriggerMode)
                {
                    /* access to trigger variable? */
                    /* get compare function */

                    if(sHandle->u8TrgVarSigned & HMRECPRM_FLOAT_TRIG_MASK)
                    {
                        #if HMRECPRM_FLOAT_TRIG
                        sHandle->pfCompareFunc = (HMRECPRM_PCOMPAREFUNC_Dummy)HMRECPRM_Comparefloat;
                        #else
                        eResponseCode = HMRECPRM_CODE_FLOATDISABLED;
                        #endif
                    }
                    else
                    {
                        if (sHandle->uTrgVarSize.sOffset.bBitField)
                        {
                            sHandle->pfCompareFunc = (HMRECPRM_PCOMPAREFUNC_Dummy)HMRECPRM_CompareBOOL;
                        }
                        else
                        {
                            switch(sHandle->uTrgVarSize.u8Register)
                            {
                            #if HMDRV_CFG_BUS_WIDTH == 1U
                            case 1:
                                sHandle->pfCompareFunc = sHandle->u8TrgVarSigned
                                        ? (HMRECPRM_PCOMPAREFUNC_Dummy)HMRECPRM_Compare8S
                                        : (HMRECPRM_PCOMPAREFUNC_Dummy)HMRECPRM_Compare8U;
                                break;
                            #endif
                            case 2:
                                sHandle->pfCompareFunc = sHandle->u8TrgVarSigned
                                        ? (HMRECPRM_PCOMPAREFUNC_Dummy)HMRECPRM_Compare16S
                                        : (HMRECPRM_PCOMPAREFUNC_Dummy)HMRECPRM_Compare16U;
                                break;

                            case 4:
                                sHandle->pfCompareFunc = sHandle->u8TrgVarSigned
                                        ? (HMRECPRM_PCOMPAREFUNC_Dummy)HMRECPRM_Compare32S
                                        : (HMRECPRM_PCOMPAREFUNC_Dummy)HMRECPRM_Compare32U;
                                break;
                            /* invalid trigger variable size  */
                            default:
                                eResponseCode = HMRECPRM_CODE_INVSIZE;
                                break;
                            }
                        }
                    }
                }

                if (eResponseCode == HMRECPRM_CODE_OK)
                {
                    /* total recorder buffer length in native sizeof units (=bytes on most platforms) */
                    blen =  ((sHandle->uRecTotalSmps * (HMDRV_SIZE_FLEX)nRecVarsetSize) / HMDRV_CFG_BUS_WIDTH);

                    /* recorder memory available? */
                    if(blen > HMRECPRM_GetRecBuffSize(sHandle))
                    {
                        eResponseCode = HMRECPRM_CODE_INVSIZE;
                    }
                    else
                    {
                        /* remember the effective end of circular buffer */
                        sHandle->pEndBuffPtr = sHandle->pBufferAddr + blen;

                        /* everything is okay    */
                        sHandle->uFlags.sFlg.bIsConfigured = 1U;
                        sStatus->sTimeSyncModeYYMMDD.b2RecorderStatus = HMRECPRM_REC_STA_NOT_STARTED;

                        /* eResponseCode = HMPROTOCOL_STS_OK; - not needed - made above */
                    }
                }
            }
        }
    }
    return eResponseCode;
}



#endif
#endif







/* *****************************************************************************
 * CSMON_vInitRecorders
 **************************************************************************** */
void CSMON_vInitRecorders (void)
{
    HMRECPRM_sConfig_t* sConfig;


#if HMDRV_RECORDER_PARAM_HANDLE_COUNT >= 1
    sConfig = &sRecConfigParam[0];

    #if (CSMON_PARAMETER_LIST_READ_ONLY == 0) || (USE_CONTROL == 0)
    sConfig->sSampleFormat.nCountVariables = 0;
    #else
    sConfig->sSampleFormat.nCountVariables = 4;
    sConfig->sSampleFormat.nVariableIDRecord[0] = CSMON_pasFileParameterList[CSMON_PARAMETER_POSITION_REGID_STACK1_CURRENT_SENSOR_PHASEA].ValueAccessKey.u32Register;
    sConfig->sSampleFormat.nVariableIDRecord[1] = CSMON_pasFileParameterList[CSMON_PARAMETER_POSITION_REGID_STACK1_CURRENT_SENSOR_PHASEB].ValueAccessKey.u32Register;
    sConfig->sSampleFormat.nVariableIDRecord[2] = CSMON_pasFileParameterList[CSMON_PARAMETER_POSITION_REGID_STACK1_CURRENT_SENSOR_PHASEC].ValueAccessKey.u32Register;
    sConfig->sSampleFormat.nVariableIDRecord[3] = CSMON_pasFileParameterList[CSMON_PARAMETER_POSITION_REGID_STACK1_VOLTAGE_DCLINK_FDBACK].ValueAccessKey.u32Register;

    #endif

    sConfig->sSampleRate.sSampleRateLookUp.eTimeBase = HMRECPRM_TIME_BASE_0;
    sConfig->sTrigerConfig.eTriggerMode = HMRECPRM_TRIGGER_NONE;                    /* Trigger not used */
    sConfig->sTrigerConfig.sTriggerThreshold.eType = HMRECPRM_VARIANT_TYPE_S16;
    sConfig->sTrigerConfig.sTriggerThreshold.eValue.s32 = 0;
    sConfig->sTrigerConfig.nPreTriggerSamples = CSMON_RECORDER_SAMPLE_COUNT_DEFAULT - CSMON_RECORDER_POSTTRIGGER_SAMPLE_COUNT_DEFAULT;
    sConfig->sTrigerConfig.nVariableIDTrigger = NULL;
    sConfig->sSampleRate.sSampleRateLookUp.nTimeDividerOrSamplesSkip = 0;

    //sConfig->sSampleFormat.sAlignments.u16LastRecorder = 0;
    sConfig->sSampleFormat.nSampleRateHz = 2500;
    sConfig->sSampleFormat.nSampleCountMax = CSMON_RECORDER_SAMPLE_COUNT_DEFAULT; /* no valid samples on setup */
    sConfig->sSampleFormat.nPreTriggerSampleCount = CSMON_RECORDER_PRETRIGGER_SAMPLE_COUNT_DEFAULT; /* no valid samples on setup */
    sConfig->sSampleFormat.sAlignments.u16ValueByteAlignment = 0;
    sConfig->sSampleFormat.sAlignments.u16SampleByteAlignment = 0;
    sConfig->sSampleFormat.sAlignments.u16Reserved = 0;

    sConfig->nTotalCountSamples = CSMON_RECORDER_SAMPLE_COUNT_DEFAULT;
#endif

#if HMDRV_RECORDER_PARAM_HANDLE_COUNT >= 2
    sConfig = &sRecConfigParam[1];



    #if (CSMON_PARAMETER_LIST_READ_ONLY == 0) || (USE_CONTROL == 0)
    sConfig->sSampleFormat.nCountVariables = 0;
    #else
    sConfig->sSampleFormat.nCountVariables = 0;
    sConfig->sSampleFormat.nVariableIDRecord[0] = CSMON_pasFileParameterList[CSMON_PARAMETER_POSITION_REGID_STACK1_TEMPERATURE_MODULE_PHASEA].ValueAccessKey.u32Register;
    sConfig->sSampleFormat.nVariableIDRecord[1] = CSMON_pasFileParameterList[CSMON_PARAMETER_POSITION_REGID_STACK1_TEMPERATURE_MODULE_PHASEB].ValueAccessKey.u32Register;
    sConfig->sSampleFormat.nVariableIDRecord[2] = CSMON_pasFileParameterList[CSMON_PARAMETER_POSITION_REGID_STACK1_TEMPERATURE_MODULE_PHASEC].ValueAccessKey.u32Register;
    sConfig->sSampleFormat.nVariableIDRecord[3] = CSMON_pasFileParameterList[CSMON_PARAMETER_POSITION_REGID_STACK1_VOLTAGE_DCLINK_RMS].ValueAccessKey.u32Register;
    #endif

    sConfig->sSampleRate.sSampleRateLookUp.eTimeBase = HMRECPRM_TIME_BASE_0;
    sConfig->sTrigerConfig.eTriggerMode = HMRECPRM_TRIGGER_NONE;                    /* Trigger not used */
    sConfig->sTrigerConfig.sTriggerThreshold.eType = HMRECPRM_VARIANT_TYPE_S16;
    sConfig->sTrigerConfig.sTriggerThreshold.eValue.s32 = 0;
    sConfig->sTrigerConfig.nPreTriggerSamples = 0;
    sConfig->sTrigerConfig.nVariableIDTrigger = NULL;
    sConfig->sSampleRate.sSampleRateLookUp.nTimeDividerOrSamplesSkip = 0;

    //sConfig->sSampleFormat.sAlignments.u16LastRecorder = 0;
    sConfig->sSampleFormat.nSampleRateHz = 2500;
    sConfig->sSampleFormat.nSampleCountMax = CSMON_RECORDER_SAMPLE_COUNT_DEFAULT; /* no valid samples on setup */
    sConfig->sSampleFormat.nPreTriggerSampleCount = CSMON_RECORDER_PRETRIGGER_SAMPLE_COUNT_DEFAULT; /* no valid samples on setup */
    sConfig->sSampleFormat.sAlignments.u16ValueByteAlignment = 0;
    sConfig->sSampleFormat.sAlignments.u16SampleByteAlignment = 0;
    sConfig->sSampleFormat.sAlignments.u16Reserved = 0;

    sConfig->nTotalCountSamples = CSMON_RECORDER_SAMPLE_COUNT_DEFAULT;
#endif


#if HMDRV_RECORDER_PARAM_HANDLE_COUNT >= 3
    sConfig = &sRecConfigParam[2];




    #if (CSMON_PARAMETER_LIST_READ_ONLY == 0) || (USE_CONTROL == 0)
    sConfig->sSampleFormat.nCountVariables = 0;
    #else
    sConfig->sSampleFormat.nCountVariables = 0;
    sConfig->sSampleFormat.nVariableIDRecord[0] = CSMON_pasFileParameterList[CSMON_PARAMETER_POSITION_REGID_STACK1_CURRENT_MODULE_PHASEA].ValueAccessKey.u32Register;
    sConfig->sSampleFormat.nVariableIDRecord[1] = CSMON_pasFileParameterList[CSMON_PARAMETER_POSITION_REGID_STACK1_CURRENT_MODULE_PHASEB].ValueAccessKey.u32Register;
    sConfig->sSampleFormat.nVariableIDRecord[2] = CSMON_pasFileParameterList[CSMON_PARAMETER_POSITION_REGID_STACK1_CURRENT_MODULE_PHASEC].ValueAccessKey.u32Register;
    sConfig->sSampleFormat.nVariableIDRecord[3] = CSMON_pasFileParameterList[CSMON_PARAMETER_POSITION_REGID_STACK1_VOLTAGE_DCLINK_FDBACK].ValueAccessKey.u32Register;
    #endif

    sConfig->sSampleRate.sSampleRateLookUp.eTimeBase = HMRECPRM_TIME_BASE_0;
    sConfig->sTrigerConfig.eTriggerMode = HMRECPRM_TRIGGER_NONE;                    /* Trigger not used */
    sConfig->sTrigerConfig.sTriggerThreshold.eType = HMRECPRM_VARIANT_TYPE_S16;
    sConfig->sTrigerConfig.sTriggerThreshold.eValue.s32 = 0;
    sConfig->sTrigerConfig.nPreTriggerSamples = 0;
    sConfig->sTrigerConfig.nVariableIDTrigger = NULL;
    sConfig->sSampleRate.sSampleRateLookUp.nTimeDividerOrSamplesSkip = 0;

    //sConfig->sSampleFormat.sAlignments.u16LastRecorder = 0;
    sConfig->sSampleFormat.nSampleRateHz = 2500;
    sConfig->sSampleFormat.nSampleCountMax = CSMON_RECORDER_SAMPLE_COUNT_DEFAULT; /* no valid samples on setup */
    sConfig->sSampleFormat.nPreTriggerSampleCount = CSMON_RECORDER_PRETRIGGER_SAMPLE_COUNT_DEFAULT; /* no valid samples on setup */
    sConfig->sSampleFormat.sAlignments.u16ValueByteAlignment = 0;
    sConfig->sSampleFormat.sAlignments.u16SampleByteAlignment = 0;
    sConfig->sSampleFormat.sAlignments.u16Reserved = 0;

    sConfig->nTotalCountSamples = CSMON_RECORDER_SAMPLE_COUNT_DEFAULT;
#endif


}

/* *****************************************************************************
 * CSMON_vInitScopes
 **************************************************************************** */

void CSMON_vInitScopes (void)
{
    HMSCOPEPRM_sConfig_t* sConfig;

#if HMDRV_SCOPE_PARAM_HANDLE_COUNT >= 1
    sConfig = &sScopeConfigParam[0];

#if (CSMON_PARAMETER_LIST_READ_ONLY == 0) || (USE_CONTROL == 0)
    sConfig->nCountVariables = 0;
#else
    sConfig->nCountVariables = 4;
    sConfig->nVariableIDScope[0] = CSMON_pasFileParameterList[CSMON_PARAMETER_POSITION_REGID_STACK1_CURRENT_SENSOR_PHASEA].ValueAccessKey.u32Register;
    sConfig->nVariableIDScope[1] = CSMON_pasFileParameterList[CSMON_PARAMETER_POSITION_REGID_STACK1_CURRENT_SENSOR_PHASEB].ValueAccessKey.u32Register;
    sConfig->nVariableIDScope[2] = CSMON_pasFileParameterList[CSMON_PARAMETER_POSITION_REGID_STACK1_CURRENT_SENSOR_PHASEC].ValueAccessKey.u32Register;
    sConfig->nVariableIDScope[3] = CSMON_pasFileParameterList[CSMON_PARAMETER_POSITION_REGID_STACK1_VOLTAGE_DCLINK_FDBACK].ValueAccessKey.u32Register;
#endif
#endif
    
#if HMDRV_SCOPE_PARAM_HANDLE_COUNT >= 2
    sConfig = &sScopeConfigParam[1];

#if (CSMON_PARAMETER_LIST_READ_ONLY == 0) || (USE_CONTROL == 0)
    sConfig->nCountVariables = 0;
#else
    sConfig->nCountVariables = 4;
    sConfig->nVariableIDScope[0] = CSMON_pasFileParameterList[CSMON_PARAMETER_POSITION_REGID_STACK1_TEMPERATURE_MODULE_PHASEA].ValueAccessKey.u32Register;
    sConfig->nVariableIDScope[1] = CSMON_pasFileParameterList[CSMON_PARAMETER_POSITION_REGID_STACK1_TEMPERATURE_MODULE_PHASEB].ValueAccessKey.u32Register;
    sConfig->nVariableIDScope[2] = CSMON_pasFileParameterList[CSMON_PARAMETER_POSITION_REGID_STACK1_TEMPERATURE_MODULE_PHASEC].ValueAccessKey.u32Register;
    sConfig->nVariableIDScope[3] = CSMON_pasFileParameterList[CSMON_PARAMETER_POSITION_REGID_STACK1_VOLTAGE_DCLINK_RMS].ValueAccessKey.u32Register;
#endif
#endif

#if HMDRV_SCOPE_PARAM_HANDLE_COUNT >= 3
    sConfig = &sScopeConfigParam[2];


#if (CSMON_PARAMETER_LIST_READ_ONLY == 0) || (USE_CONTROL == 0)
    sConfig->nCountVariables = 0;
#else
    sConfig->nCountVariables = 4;
    sConfig->nVariableIDScope[0] = CSMON_pasFileParameterList[CSMON_PARAMETER_POSITION_REGID_STACK1_CURRENT_MODULE_PHASEA].ValueAccessKey.u32Register;
    sConfig->nVariableIDScope[1] = CSMON_pasFileParameterList[CSMON_PARAMETER_POSITION_REGID_STACK1_CURRENT_MODULE_PHASEB].ValueAccessKey.u32Register;
    sConfig->nVariableIDScope[2] = CSMON_pasFileParameterList[CSMON_PARAMETER_POSITION_REGID_STACK1_CURRENT_MODULE_PHASEC].ValueAccessKey.u32Register;
    sConfig->nVariableIDScope[3] = CSMON_pasFileParameterList[CSMON_PARAMETER_POSITION_REGID_STACK1_VOLTAGE_DCLINK_FDBACK].ValueAccessKey.u32Register;
#endif
#endif
}


/* *****************************************************************************
 * CSMON_vSetDateTimeAutorization
 *
 * Description:
 **************************************************************************** */
void CSMON_vSetDateTimeAutorization(void)
{
    bool bUpdateData = false;

    CSMON_bGetDateTimeAutorization = true;

    #if 0
    sHashKey.ID = 0x00;
    #endif
    if (sHashKey.YY != (char)CSMON_sDateTime.u8Year)
    {
        sHashKey.YY = (char)CSMON_sDateTime.u8Year;
        bUpdateData = true;
    }
    if (sHashKey.MM != (char)CSMON_sDateTime.u8Month)
    {
        sHashKey.MM = (char)CSMON_sDateTime.u8Month;
        bUpdateData = true;
    }
    if (sHashKey.DD != (char)CSMON_sDateTime.u8Day)
    {
        sHashKey.DD = (char)CSMON_sDateTime.u8Day;
        bUpdateData = true;
    }
    if (sHashKey.hh != (char)CSMON_sDateTime.u8Hours)
    {
        sHashKey.hh = (char)CSMON_sDateTime.u8Hours;
        bUpdateData = true;
    }
    if (sHashKey.mm != (char)CSMON_sDateTime.u8Minutes)
    {
        sHashKey.mm = (char)CSMON_sDateTime.u8Minutes;
        bUpdateData = true;
    }
    if (sHashKey.ss != (char)CSMON_sDateTime.u8Seconds)
    {
        sHashKey.ss = (char)CSMON_sDateTime.u8Seconds;
        bUpdateData = true;
    }
    if (sHashKey.sub3 != (char)((CSMON_sDateTime.u32Microseconds >> 24) & (0x000000FF)))
    {
        sHashKey.sub3 = (char)((CSMON_sDateTime.u32Microseconds >> 24) & (0x000000FF));
        bUpdateData = true;
    }
    if (sHashKey.sub2 != (char)((CSMON_sDateTime.u32Microseconds >>  16) & (0x000000FF)))
    {
        sHashKey.sub2 = (char)((CSMON_sDateTime.u32Microseconds >> 16) & (0x000000FF));
        bUpdateData = true;
    }
    if (sHashKey.sub1 != (char)((CSMON_sDateTime.u32Microseconds >>  8) & (0x000000FF)))
    {
        sHashKey.sub1 = (char)((CSMON_sDateTime.u32Microseconds >>  8) & (0x000000FF));
        bUpdateData = true;
    }
    if (sHashKey.sub0 != (char)((CSMON_sDateTime.u32Microseconds >>  0) & (0x000000FF)))
    {
        sHashKey.sub0 = (char)((CSMON_sDateTime.u32Microseconds >>  0) & (0x000000FF));
        bUpdateData = true;
    }

    #if 0
    sHashKey.id3 = (char)((CSMON_sDateTime.u32ClientID >> 24) & (0x000000FF));
    sHashKey.id2 = (char)((CSMON_sDateTime.u32ClientID >> 16) & (0x000000FF));
    sHashKey.id1 = (char)((CSMON_sDateTime.u32ClientID >>  8) & (0x000000FF));
    sHashKey.id0 = (char)((CSMON_sDateTime.u32ClientID >>  0) & (0x000000FF));
    #endif

    if (bUpdateData == false)
    {
        CSMON_sDateTime.u32Microseconds++;

        if (sHashKey.sub3 != (char)((CSMON_sDateTime.u32Microseconds >> 24) & (0x000000FF)))
        {
            sHashKey.sub3 = (char)((CSMON_sDateTime.u32Microseconds >> 24) & (0x000000FF));
            bUpdateData = true;
        }
        if (sHashKey.sub2 != (char)((CSMON_sDateTime.u32Microseconds >>  16) & (0x000000FF)))
        {
            sHashKey.sub2 = (char)((CSMON_sDateTime.u32Microseconds >> 16) & (0x000000FF));
            bUpdateData = true;
        }
        if (sHashKey.sub1 != (char)((CSMON_sDateTime.u32Microseconds >>  8) & (0x000000FF)))
        {
            sHashKey.sub1 = (char)((CSMON_sDateTime.u32Microseconds >>  8) & (0x000000FF));
            bUpdateData = true;
        }
        if (sHashKey.sub0 != (char)((CSMON_sDateTime.u32Microseconds >>  0) & (0x000000FF)))
        {
            sHashKey.sub0 = (char)((CSMON_sDateTime.u32Microseconds >>  0) & (0x000000FF));
            bUpdateData = true;
        }
    }


}


/* *****************************************************************************
 * CSMON_vSetDateTimeRequest
 *
 * Description:
 **************************************************************************** */
void CSMON_vSetDateTimeRequest(void)
{
    CSMON_bSetDateTimeRequest = 1;
}






#if 0
uint32_t u32List[8] =
{
 2415069869,
  197832619,
 4050698081,
 1506978467,
  697832461,
 1197832667,
 3824150819,
 2924151019,
};

#endif


#define CSMON_vCheckSumInit() \
    uint32_t u32RecordIndex; \
    uint32_t u32RecordLength; \
    uint32_t u32RecordNumber; \
    uWord32_t uCheckSumRec


#define CSMON_vCheckSumReset(nRecLength) \
{ \
    u32RecordLength = nRecLength; \
    u32RecordNumber = 0; \
    u32RecordIndex = 0; \
    uCheckSumRec.u32Register = 0; \
}



#define CSMON_vCheckSumLoop(pFileAddr) \
{ \
    uCheckSumRec.u32Register += pFileAddr[u32FileDataIndex]; \
     \
    u32RecordIndex++; \
    if (u32RecordIndex >= u32RecordLength) \
    { \
        uCheckSumRec.u32Register = uCheckSumRec.u32Register ^ u32RecordNumber; \
        uCheckSum.u32Register += uCheckSumRec.u32Register; \
        u32RecordNumber += 0x00010001; \
        u32RecordIndex = 0; \
        uCheckSumRec.u32Register = 0; \
    } \
}




/* *****************************************************************************
 * CSMON_u32GetParameterListCheckSum
 *
 * Description:
 *  Calculates and returns CheckSum of the Parameter List Table
 *
 **************************************************************************** */
uint32_t CSMON_u32GetParameterListCheckSum (void)
{
    uWord32_t uCheckSum;
    //uWord32_t uCheckSumXor;

    //uCheckSumXor.u32Register = 3241507183;

    CSMON_u32FreeRunTimerCheckSumListBegin = CSMON_u32GetFreeRunningTimerTicks();   /* Measure max Time in CheckSum Prepare */

    CSMON_vCheckSumInit();

    uint32_t u32FileDataIndex;
    uint32_t u32FileDataLength;

    //uint16_t* pFileParameter;    /* 16-Bit Read From Memory - CS 16Bit Add */
#ifdef OLD_TI
    uint32_t* pFileParameter;    /* 32-Bit Read From Memory - CS 32Bit Add */
#else
    #if CSMON_PARAMETER_LIST_READ_ONLY
    __CONSTANT_MEMORY_MODIFIER__ uint32_t* pFileParameter;    /* 32-Bit Read From Memory - CS 32Bit Add */
    #else
    __EXTENDED_MEMORY_MODIFIER__ uint32_t* pFileParameter;    /* 32-Bit Read From Memory - CS 32Bit Add to do for dsPIC */
    #endif

    #if CSMON_PARAMETER_LIST_READ_ONLY
    __EXTENDED_MEMORY_MODIFIER__ uint32_t* pFileParameterRegRealAddr;    /* 32-Bit Read From Memory - CS 32Bit Add */
    #else
    __EXTENDED_MEMORY_MODIFIER__ uint32_t* pFileParameterRegRealAddr;    /* 32-Bit Read From Memory - CS 32Bit Add to do for dsPIC */
    #endif
#endif

    //uCheckSum.u32Register = DATETIME_BUILD;
    uCheckSum.u32Register = 0xDEADDEAD;
    //uCheckSumXor[0].u32Register = 0xAA5555AA;
    //uCheckSumXor[1].u32Register = 0x55AAAA55;

    /* Parameter List Table */
    //pFileParameter = (uint16_t*)CSMON_pasFileParameterList;

    #if CSMON_PARAMETER_LIST_READ_ONLY
    pFileParameter = (__CONSTANT_MEMORY_MODIFIER__ uint32_t*)CSMON_pasFileParameterList;
    #else
    pFileParameter = (__EXTENDED_MEMORY_MODIFIER__ uint32_t*)CSMON_pasFileParameterList;/* to do for dsPIC */
    #endif
   
    if  (pFileParameter != NULL)
    {
        u32FileDataLength = (CSMON_FILE_PARAMETER_LIST_LENGTH_BYTES_IN_MEMORY) * CSMON_u16FileParameterFillCount;

        //u32FileDataLength >>= 1;    /* 16-Bit Read From Memory */
        u32FileDataLength >>= 2;    /* 32-Bit Read From Memory */

        CSMON_vCheckSumReset((CSMON_FILE_PARAMETER_LIST_LENGTH_BYTES_IN_MEMORY) >> 2);/* 32-Bit Read From Memory */

        if (bServiceWatchdog)
        {
            #ifndef __XC16
            SysCtl_serviceWatchdog();
            #else
            ClrWdt();
            #endif
        }


        HMDRV_vSetDebugPin();   /* AmberLed on 1038 */
        for (u32FileDataIndex = 0; u32FileDataIndex < u32FileDataLength; u32FileDataIndex++)
        {
            CSMON_vCheckSumLoop(pFileParameter);

            //uCheckSum.u32Register += pFileParameter[u32FileDataIndex];
            //uCheckSum.u32Register += pFileParameter[u32FileDataIndex] ^ uCheckSumXor[u32FileDataIndex & 00000001].u32Register ^ u32FileDataIndex;
            //uCheckSum.u32Register ^= (pFileParameter[u32FileDataIndex] + u32FileDataIndex);

#if 0
            uint32_t vx = pFileParameter[u32FileDataIndex] ^ uCheckSumXor.u32Register;
            uCheckSum.u32Register += vx;

            uint16_t tmp = uCheckSumXor.au16Word[0];
            uCheckSumXor.au16Word[0] = uCheckSumXor.au16Word[1];
            uCheckSumXor.au16Word[0] = tmp;
            uCheckSum.u32Register ^= uCheckSumXor.u32Register;

            uCheckSumXor.u32Register += u32List[u32FileDataIndex & 0x3];
#endif
        }
        HMDRV_vClrDebugPin();   /* AmberLed on 1038 */
    }

    /* Modbus Address Virtual Table */
    //pFileParameter = (uint16_t*)CSMON_pu32HoldingRegistersAddressTable;
 
    #if CSMON_PARAMETER_LIST_READ_ONLY
    pFileParameter = (__CONSTANT_MEMORY_MODIFIER__ uint32_t*)CSMON_pu32HoldingRegistersAddressTable;
    #else
    pFileParameter = (__EXTENDED_MEMORY_MODIFIER__ uint32_t*)CSMON_pu32HoldingRegistersAddressTable;/* to do for dsPIC */
    #endif

    if  (pFileParameter != NULL)
    {
        u32FileDataLength = ((sizeof(HMMODBUS_sRegisterAddressTable_t) * HMDRV_ADDR_WIDTH)) * MODBUSREG_u16GetHoldRegFillCount();
        //u32FileDataLength >>= 1;    /* 16-Bit Read From Memory */
        u32FileDataLength >>= 2;    /* 32-Bit Read From Memory */

        CSMON_vCheckSumReset(((sizeof(HMMODBUS_sRegisterAddressTable_t) * HMDRV_ADDR_WIDTH)) >> 2);   /* 32-Bit Read From Memory */

        if (bServiceWatchdog)
        {
            #ifndef __XC16
            SysCtl_serviceWatchdog();
            #else
            ClrWdt();
            #endif
        }

        HMDRV_vSetDebugPin();   /* AmberLed on 1038 */
        for (u32FileDataIndex = 0; u32FileDataIndex < u32FileDataLength; u32FileDataIndex++)
        {
            CSMON_vCheckSumLoop(pFileParameter);
            //uCheckSum.u32Register += pFileParameter[u32FileDataIndex];
            //uCheckSum.u32Register += pFileParameter[u32FileDataIndex] ^ uCheckSumXor[u32FileDataIndex & 00000001].u32Register ^ u32FileDataIndex;
            //uCheckSum.u32Register ^= (pFileParameter[u32FileDataIndex] + u32FileDataIndex);
#if 0
            uint32_t vx = pFileParameter[u32FileDataIndex] ^ uCheckSumXor.u32Register;
            uCheckSum.u32Register += vx;

            uint16_t tmp = uCheckSumXor.au16Word[0];
            uCheckSumXor.au16Word[0] = uCheckSumXor.au16Word[1];
            uCheckSumXor.au16Word[0] = tmp;
            uCheckSum.u32Register ^= uCheckSumXor.u32Register;

            uCheckSumXor.u32Register += u32List[u32FileDataIndex & 0x3];
#endif
        }
        HMDRV_vClrDebugPin();   /* AmberLed on 1038 */
    }

    /* Modbus Address Real Table */
    //pFileParameterRegRealAddr = (uint16_t*)CSMON_pu32HoldingRegistersAddressTableInRam;
    //pFileParameterRegRealAddr = (uint32_t*)(HMDRV_ADDR_SIZE)CSMON_pu32HoldingRegistersAddressTableInRam;

    #if CSMON_PARAMETER_LIST_READ_ONLY
    pFileParameterRegRealAddr = (__EXTENDED_MEMORY_MODIFIER__ uint32_t*)CSMON_pu32HoldingRegistersAddressTableInRam;
    #else
    pFileParameterRegRealAddr = (__EXTENDED_MEMORY_MODIFIER__ uint32_t*)CSMON_pu32HoldingRegistersAddressTableInRam;/* to do for dsPIC */
    #endif
    
    
    if  (pFileParameterRegRealAddr != NULL)
    {
        u32FileDataLength = ((sizeof(HMMODBUS_sRegisterAddressTableInRam_t) * HMDRV_ADDR_WIDTH)) * MODBUSREG_u16GetHoldRegFillCount();
        //u32FileDataLength >>= 1;    /* 16-Bit Read From Memory */
        u32FileDataLength >>= 2;    /* 32-Bit Read From Memory */

        CSMON_vCheckSumReset(((sizeof(HMMODBUS_sRegisterAddressTableInRam_t) * HMDRV_ADDR_WIDTH)) >> 2);    /* 32-Bit Read From Memory */

        if (bServiceWatchdog)
        {
            #ifndef __XC16
            SysCtl_serviceWatchdog();
            #else
            ClrWdt();
            #endif
        }

        HMDRV_vSetDebugPin();   /* AmberLed on 1038 */
        for (u32FileDataIndex = 0; u32FileDataIndex < u32FileDataLength; u32FileDataIndex++)
        {
            CSMON_vCheckSumLoop(pFileParameterRegRealAddr);


            //uCheckSum.u32Register += pFileParameter[u32FileDataIndex];
            //uCheckSum.u32Register += pFileParameter[u32FileDataIndex] ^ uCheckSumXor[u32FileDataIndex & 00000001].u32Register ^ u32FileDataIndex;
            //uCheckSum.u32Register ^= (pFileParameter[u32FileDataIndex] + u32FileDataIndex);
#if 0
            uint32_t vx = pFileParameterRegRealAddr[u32FileDataIndex] ^ uCheckSumXor.u32Register;
            uCheckSum.u32Register += vx;

            uint16_t tmp = uCheckSumXor.au16Word[0];
            uCheckSumXor.au16Word[0] = uCheckSumXor.au16Word[1];
            uCheckSumXor.au16Word[0] = tmp;
            uCheckSum.u32Register ^= uCheckSumXor.u32Register;

            uCheckSumXor.u32Register += u32List[u32FileDataIndex & 0x3];
#endif
        }
        HMDRV_vClrDebugPin();   /* AmberLed on 1038 */
    }

    uCheckSum.u32Register = (uint32_t)0 - uCheckSum.u32Register;

    /* Measure max Time in CheckSum */
    CSMON_u32FreeRunTimerCheckSumListEnd = CSMON_u32GetFreeRunningTimerTicks() - CSMON_u32FreeRunTimerCheckSumListBegin;
    if (CSMON_u32FreeRunTimerCheckSumListEnd > CSMON_u32FreeRunTimerCheckSumListMax)
    {
        CSMON_u32FreeRunTimerCheckSumListMax = CSMON_u32FreeRunTimerCheckSumListEnd;
    }

    return uCheckSum.u32Register;
}



/* *****************************************************************************
 * CSMON_u32GetParameterShortNameCheckSum
 *
 * Description:
 *  Calculates and returns CheckSum of the Parameter ShortName Table
 *
 **************************************************************************** */
uint32_t CSMON_u32GetParameterShortNameCheckSum (void)
{
    uWord32_t uCheckSum;
    //uWord32_t uCheckSumXor;

    //uCheckSumXor.u32Register = 3241507183;
    uint32_t u32FileDataIndex;
    uint32_t u32FileDataLength;

    CSMON_u32FreeRunTimerCheckSumNameBegin = CSMON_u32GetFreeRunningTimerTicks();   /* Measure max Time in CheckSum Prepare */
    CSMON_vCheckSumInit();


    //uint16_t* pFileParameter;    /* 16-Bit Read From Memory - CS 16Bit Add */
#ifdef OLD_TI
    uint32_t* pFileParameter;    /* 32-Bit Read From Memory - CS 32Bit Add */
#else
    #if CSMON_PARAMETER_LIST_READ_ONLY
    __CONSTANT_MEMORY_MODIFIER__ uint32_t* pFileParameter;    /* 32-Bit Read From Memory - CS 32Bit Add */
    #else
    __EXTENDED_MEMORY_MODIFIER__ uint32_t* pFileParameter;    /* 32-Bit Read From Memory - CS 32Bit Add to do for dsPIC */
    #endif
#endif

    //uCheckSum.u32Register = DATETIME_BUILD;
    uCheckSum.u32Register = 0xDEADDEAD;
    //uCheckSumXor[0].u32Register = 0xAA5555AA;
    //uCheckSumXor[1].u32Register = 0x55AAAA55;

    /* Parameter Short Name Table */
    //pFileParameter = (uint16_t*)CSMON_pasFileParameterShortName;
    //pFileParameter = (__CONSTANT_MEMORY_MODIFIER__ uint32_t*)CSMON_pasFileParameterShortName;
    
    #if CSMON_PARAMETER_LIST_READ_ONLY
    pFileParameter = (__CONSTANT_MEMORY_MODIFIER__ uint32_t*)CSMON_pasFileParameterShortName;
    #else
    pFileParameter = (__EXTENDED_MEMORY_MODIFIER__ uint32_t*)CSMON_pasFileParameterShortName;/* to do for dsPIC */
    #endif
    
    u32FileDataLength = (CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH_BYTES_IN_MEMORY) * CSMON_u16FileParameterFillCount;

    //u32FileDataLength >>= 1;    /* 16-Bit Read From Memory */
    u32FileDataLength >>= 2;    /* 32-Bit Read From Memory */

    if  (pFileParameter != NULL)
    {
        u32FileDataLength = (CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH_BYTES_IN_MEMORY) * CSMON_u16FileParameterFillCount;

        //u32FileDataLength >>= 1;    /* 16-Bit Read From Memory */
        u32FileDataLength >>= 2;    /* 32-Bit Read From Memory */

        CSMON_vCheckSumReset((CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH_BYTES_IN_MEMORY) >> 2);    /* 32-Bit Read From Memory */


        if (bServiceWatchdog)
        {
            #ifndef __XC16
            SysCtl_serviceWatchdog();
            #else
            ClrWdt();
            #endif
        }

        HMDRV_vSetDebugPin();   /* AmberLed on 1038 */
        for (u32FileDataIndex = 0; u32FileDataIndex < u32FileDataLength; u32FileDataIndex++)
        {
            CSMON_vCheckSumLoop(pFileParameter);

            //uCheckSum.u32Register += pFileParameter[u32FileDataIndex];
            //uCheckSum.u32Register += pFileParameter[u32FileDataIndex] ^ uCheckSumXor[u32FileDataIndex & 00000001].u32Register ^ u32FileDataIndex;
            //uCheckSum.u32Register ^= (pFileParameter[u32FileDataIndex] + u32FileDataIndex);

#if 0
            uint32_t vx = pFileParameter[u32FileDataIndex] ^ uCheckSumXor.u32Register;
            uCheckSum.u32Register += vx;

            uint16_t tmp = uCheckSumXor.au16Word[0];
            uCheckSumXor.au16Word[0] = uCheckSumXor.au16Word[1];
            uCheckSumXor.au16Word[0] = tmp;
            uCheckSum.u32Register ^= uCheckSumXor.u32Register;

            uCheckSumXor.u32Register += u32List[u32FileDataIndex & 0x3];
#endif
        }
        HMDRV_vClrDebugPin();   /* AmberLed on 1038 */
    }


    uCheckSum.u32Register = (uint32_t)0 - uCheckSum.u32Register;

    /* Measure max Time in CheckSum */
    CSMON_u32FreeRunTimerCheckSumNameEnd = CSMON_u32GetFreeRunningTimerTicks() - CSMON_u32FreeRunTimerCheckSumNameBegin;
    if (CSMON_u32FreeRunTimerCheckSumNameEnd > CSMON_u32FreeRunTimerCheckSumNameMax)
    {
        CSMON_u32FreeRunTimerCheckSumNameMax = CSMON_u32FreeRunTimerCheckSumNameEnd;
    }
    return uCheckSum.u32Register;
}


/* *****************************************************************************
 * CSMON_u32GetParameterNumbCheckSum
 *
 * Description:
 *  Calculates and returns CheckSum of the Parameter Numb Table
 *
 **************************************************************************** */
uint32_t CSMON_u32GetParameterNumbCheckSum (void)
{
    uWord32_t uCheckSum;
#if 1
    //uWord32_t uCheckSumXor;
    //uCheckSumXor.u32Register = 3241507183;
#else
    uWord32_t uCheckSumAux[7];
    uint16_t u16CheckSumAuxIndex = 0;
#endif
    CSMON_u32FreeRunTimerCheckSumNumbBegin = CSMON_u32GetFreeRunningTimerTicks();   /* Measure max Time in CheckSum Prepare */

    
    //#if BOARDCFG_USE_CSMON_NUMERIC_TABLE
    CSMON_vCheckSumInit();

    uint32_t u32FileDataIndex;
    uint32_t u32FileDataLength;

    //uint16_t* pFileParameter;    /* 16-Bit Read From Memory - CS 16Bit Add */
    #ifdef OLD_TI
    uint32_t* pFileParameter;    /* 32-Bit Read From Memory - CS 32Bit Add */
    #else
    
    #if CSMON_PARAMETER_LIST_READ_ONLY
    __CONSTANT_MEMORY_MODIFIER__ uint32_t* pFileParameter;    /* 32-Bit Read From Memory - CS 32Bit Add */
    #else
    __EXTENDED_MEMORY_MODIFIER__ uint32_t* pFileParameter;    /* 32-Bit Read From Memory - CS 32Bit Add to do for dsPIC */
    #endif

    #endif  /* #ifdef OLD_TI */


    //#endif  /* #if BOARDCFG_USE_CSMON_NUMERIC_TABLE */
    
    //uCheckSum.u32Register = DATETIME_BUILD;
    uCheckSum.u32Register = 0xDEADDEAD;
    //uCheckSumXor[0].u32Register = 0xAA5555AA;
    //uCheckSumXor[1].u32Register = 0x55AAAA55;
    
    
    //#if BOARDCFG_USE_CSMON_NUMERIC_TABLE

    //pFileParameter = (uint16_t*)CSMON_pauFileParameterNumeric;
    pFileParameter = (__CONSTANT_MEMORY_MODIFIER__ uint32_t*)CSMON_pauFileParameterNumeric;

    #if CSMON_PARAMETER_LIST_READ_ONLY
    pFileParameter = (__CONSTANT_MEMORY_MODIFIER__ uint32_t*)CSMON_pauFileParameterNumeric;
    #else
    pFileParameter = (__EXTENDED_MEMORY_MODIFIER__ uint32_t*)CSMON_pauFileParameterNumeric;/* to do for dsPIC */
    #endif
    
    
    if  (pFileParameter != NULL)
    {
        u32FileDataLength = (CSMON_FILE_PARAMETER_NUMERIC_LENGTH_BYTES_IN_MEMORY) * CSMON_u16FileParameterFillCount;

        //u32FileDataLength >>= 1;    /* 16-Bit Read From Memory */
        u32FileDataLength >>= 2;    /* 32-Bit Read From Memory */

        CSMON_vCheckSumReset((CSMON_FILE_PARAMETER_NUMERIC_LENGTH_BYTES_IN_MEMORY) >> 2);    /* 32-Bit Read From Memory */

        if (bServiceWatchdog)
        {
            #ifndef __XC16
            SysCtl_serviceWatchdog();
            #else
            ClrWdt();
            #endif
        }

#if 0
        uCheckSumAux[0].u32Register = 0;
        uCheckSumAux[1].u32Register = 0;
        uCheckSumAux[2].u32Register = 0;
        uCheckSumAux[3].u32Register = 0;
        uCheckSumAux[4].u32Register = 0;
        uCheckSumAux[5].u32Register = 0;
        uCheckSumAux[6].u32Register = 0;
#endif

        HMDRV_vSetDebugPin();   /* AmberLed on 1038 */

        for (u32FileDataIndex = 0; u32FileDataIndex < u32FileDataLength; u32FileDataIndex++)
        {
            CSMON_vCheckSumLoop(pFileParameter);

            //uCheckSum.u32Register += pFileParameter[u32FileDataIndex];
            //uCheckSum.u32Register += pFileParameter[u32FileDataIndex] ^ uCheckSumXor[u32FileDataIndex & 00000001].u32Register ^ u32FileDataIndex;
            //uCheckSum.u32Register ^= (pFileParameter[u32FileDataIndex] + u32FileDataIndex);
#if 0
#if 0
            uint32_t vx = pFileParameter[u32FileDataIndex] ^ uCheckSumXor.u32Register;
            uCheckSum.u32Register += vx;

            uint16_t tmp = uCheckSumXor.au16Word[0];
            uCheckSumXor.au16Word[0] = uCheckSumXor.au16Word[1];
            uCheckSumXor.au16Word[0] = tmp;
            uCheckSum.u32Register ^= uCheckSumXor.u32Register;

            uCheckSumXor.u32Register += u32List[u32FileDataIndex & 0x3];
#else
            if (u16CheckSumAuxIndex >= 7)
            {
                u16CheckSumAuxIndex = 0;
            }
            uCheckSumAux[u16CheckSumAuxIndex++].u32Register += pFileParameter[u32FileDataIndex];
#endif
#endif
        }
        HMDRV_vClrDebugPin();   /* AmberLed on 1038 */

#if 0
        uCheckSum.au16Word[0] =
                       (uCheckSumAux[0].au16Word[0] -
                        uCheckSumAux[1].au16Word[1] ^
                        uCheckSumAux[2].au16Word[0] +
                        uCheckSumAux[3].au16Word[1] ^
                        uCheckSumAux[4].au16Word[0] -
                        uCheckSumAux[5].au16Word[1] ^
                        uCheckSumAux[6].au16Word[0]);
        uCheckSum.au16Word[1] =
                       (uCheckSumAux[6].au16Word[1] -
                        uCheckSumAux[5].au16Word[0] ^
                        uCheckSumAux[4].au16Word[1] +
                        uCheckSumAux[3].au16Word[0] ^
                        uCheckSumAux[2].au16Word[1] -
                        uCheckSumAux[1].au16Word[0] ^
                        uCheckSumAux[0].au16Word[1]);
#endif
    }
    //#endif  /* BOARDCFG_USE_CSMON_NUMERIC_TABLE */
    uCheckSum.u32Register = (uint32_t)0 - uCheckSum.u32Register;

    /* Measure max Time in CheckSum */
    CSMON_u32FreeRunTimerCheckSumNumbEnd = CSMON_u32GetFreeRunningTimerTicks() - CSMON_u32FreeRunTimerCheckSumNumbBegin;
    if (CSMON_u32FreeRunTimerCheckSumNumbEnd > CSMON_u32FreeRunTimerCheckSumNumbMax)
    {
        CSMON_u32FreeRunTimerCheckSumNumbMax = CSMON_u32FreeRunTimerCheckSumNumbEnd;
    }
    return uCheckSum.u32Register;
}



/* 32 Bit CRC From Mladen */
#if 0

    uCheckSumXor[0].u32Register = 32057;
    uCheckSumXor[1].u32Register = 37049;

    for (u32FileDataIndex = 0; u32FileDataIndex < u32FileDataLength; u32FileDataIndex++)
    {
        //uCheckSum.u32Register += pFileParameter[u32FileDataIndex];
        uint32_t vx = pFileParameter[u32FileDataIndex] ^ uCheckSumXor[0].u32Register;
        uint32_t ix = u32FileDataIndex ^ uCheckSumXor[1].u32Register;
        uint32_t cs = vx + ix;
        uCheckSum.u32Register += cs;

        uint16_t tmp = uCheckSumXor[0].au16Word[0];
        uCheckSumXor[0].au16Word[0] = uCheckSumXor[1].au16Word[1] + 42061;
        uCheckSumXor[1].au16Word[1] = uCheckSumXor[0].au16Word[1] + 47087;
        uCheckSumXor[0].au16Word[1] = uCheckSumXor[1].au16Word[0] + 52067;
        uCheckSumXor[1].au16Word[0] = tmp + 57077;
    }
#endif

#if 0

    uint32_t u32List[8] =
    {
     2415069869,
      197832619,
     4050698081,
     1506978467,
      697832461,
     1197832667,
     3824150819,
     2924151019,
    };

    UCheckSumXor uCheckSumXor;

    uCheckSumXor.u32Register = 3241507183;

    for (u32FileDataIndex = 0; u32FileDataIndex < u32FileDataLength; u32FileDataIndex++)
    {
        //uCheckSum.u32Register += pFileParameter[u32FileDataIndex];
        uint32_t vx = pFileParameter[u32FileDataIndex] ^ uCheckSumXor.u32Register;
        uCheckSum.u32Register += vx;

        uint16_t tmp = uCheckSumXor.au16Word[0];
        uCheckSumXor.au16Word[0] = uCheckSumXor.au16Word[1];
        uCheckSumXor.au16Word[0] = tmp;

        uCheckSumXor.u32Register += u32List[u32FileDataIndex & 0x3];
    }
#endif



/* *****************************************************************************
 * CSMON_u32GetParameterStringCheckSum
 *
 * Description:
 *  Calculates and returns CheckSum of the Parameter String Table
 *
 **************************************************************************** */
uint32_t CSMON_u32GetParameterStringCheckSum (void)
{
    uWord32_t uCheckSum;
    
    CSMON_u32FreeRunTimerCheckSumStrgBegin = CSMON_u32GetFreeRunningTimerTicks();   /* Measure max Time in CheckSum Prepare */

    #if BOARDCFG_USE_CSMON_STRING_TABLE

    uint32_t u32FileDataIndex;
    uWord32_t u32FileDataLength;    /* in file this is big endian -> First16Bit is the High word; next16 bit is the low word */
    uint16_t u16ServiceWatchdogCounter = 1; /* Default Never Reset */
    uint16_t u16ServiceWatchdogAdd = 0;

    //uint16_t* pFileParameter;    /* 16-Bit Read From Memory - CS 16Bit Add */
    uint32_t* pFileParameter;    /* 32-Bit Read From Memory - CS 32Bit Add */

    uint16_t* pFileParameterResetSize;    /* 16-Bit Write To Memory */

    #endif /* #if BOARDCFG_USE_CSMON_STRING_TABLE */
    
    uCheckSum.u32Register = 0xDEADDEAD;
    
    #if BOARDCFG_USE_CSMON_STRING_TABLE

    

    pFileParameterResetSize = (uint16_t*)CSMON_pu32FileParameterString;


    //pFileParameter = (uint16_t*)CSMON_pu32FileParameterString;    /* 16-Bit Read From Memory */
    pFileParameter = (uint32_t*)CSMON_pu32FileParameterString;    /* 32-Bit Read From Memory */

    if  (pFileParameter != NULL)
    {
        /* in file this is big endian -> First16Bit is the High word; next16 bit is the low word */
        u32FileDataLength.au16Word[0] = pFileParameterResetSize[1];
        u32FileDataLength.au16Word[1] = pFileParameterResetSize[0];

        //u32FileDataLength = ((uint32_t*)CSMON_pu32FileParameterString)[0]; if like old normal little endian

        if (u32FileDataLength.u32Register > CSMON_PARAMETER_STRG_RW_SIZE) /* Bad Size Read From Size Location */
        {
            u32FileDataLength.u32Register = 0;

            /* Reset Size if bad size detected */
            pFileParameterResetSize[0] = 0;
            pFileParameterResetSize[1] = 0;
        }

        u32FileDataLength.u32Register += 4;
        if (u32FileDataLength.u32Register > CSMON_PARAMETER_STRG_RW_SIZE) /* Bad Size Read From Size Location */
        {
            u32FileDataLength.u32Register = 0;

            /* Reset Size if bad size detected */
            pFileParameterResetSize[0] = 0;
            pFileParameterResetSize[1] = 0;
        }

        //u32FileDataLength >>= 1;    /* 16-Bit Read From Memory */
        u32FileDataLength.u32Register >>= 2;    /* 32-Bit Read From Memory */

        if (bServiceWatchdog)
        {
            u16ServiceWatchdogCounter = 0;  /* Reset Used */
            u16ServiceWatchdogAdd = (uint16_t)(0x10000 / 0x1000);  /* Reset Each 4kW (0x1000) */
            #ifndef __XC16
            SysCtl_serviceWatchdog();
            #else
            ClrWdt();
            #endif
        }

        HMDRV_vSetDebugPin();   /* AmberLed on 1038 */
        for (u32FileDataIndex = 0; u32FileDataIndex < u32FileDataLength.u32Register; u32FileDataIndex++)
        {
            uCheckSum.u32Register += pFileParameter[u32FileDataIndex];

            u16ServiceWatchdogCounter += u16ServiceWatchdogAdd;
            if (u16ServiceWatchdogCounter == 0x0000) /* Reset Each 4kW */
            {
                u16ServiceWatchdogCounter = 0;
                #ifndef __XC16
                SysCtl_serviceWatchdog();
                #else
                ClrWdt();
                #endif
            }

        }
        HMDRV_vClrDebugPin();   /* AmberLed on 1038 */
    }
    #endif  /* BOARDCFG_USE_CSMON_STRING_TABLE */
    uCheckSum.u32Register = (uint32_t)0 - uCheckSum.u32Register;

    /* Measure max Time in CheckSum */
    CSMON_u32FreeRunTimerCheckSumStrgEnd = CSMON_u32GetFreeRunningTimerTicks() - CSMON_u32FreeRunTimerCheckSumStrgBegin;
    if (CSMON_u32FreeRunTimerCheckSumStrgEnd > CSMON_u32FreeRunTimerCheckSumStrgMax)
    {
        CSMON_u32FreeRunTimerCheckSumStrgMax = CSMON_u32FreeRunTimerCheckSumStrgEnd;
    }

    return uCheckSum.u32Register;
}

/* *****************************************************************************
 * CSMON_u32GetParameterAuxCheckSum
 *
 * Description:
 *  Calculates and returns CheckSum of the Parameter Aux Table
 *
 **************************************************************************** */
uint32_t CSMON_u32GetParameterAuxCheckSum (void)
{
    uWord32_t uCheckSum;

    #if CSMON_PARAMETER_LIST_READ_ONLY == 0
    uint32_t u32FileDataIndex;
    uint32_t u32FileDataLength;

    uint16_t* pu16FileParameter;
    #endif

    uCheckSum.u32Register = 0xDEADDEAD;

    #if CSMON_PARAMETER_LIST_READ_ONLY == 0
    pu16FileParameter = (uint16_t*)CSMON_psFileParameterAux;

    u32FileDataLength = (CSMON_FILE_PARAMETER_AUX_LENGTH_BYTES_IN_MEMORY / 2);

    if  (pu16FileParameter != NULL)
    {
        HMDRV_vSetDebugPin();   /* AmberLed on 1038 */
        for (u32FileDataIndex = 0; u32FileDataIndex < u32FileDataLength; u32FileDataIndex++)
        {
            uCheckSum.u32Register += pu16FileParameter[u32FileDataIndex];
            if (bServiceWatchdog)
            {
                //if ((u32FileDataIndex & 0x000003FF) == 0x000003FF)  /* Reset Each 1kW */
                {
                    #ifndef __XC16
                    SysCtl_serviceWatchdog();
                    #else
                    ClrWdt();
                    #endif
                }
            }
        }
        HMDRV_vClrDebugPin();   /* AmberLed on 1038 */
    }
    #endif
    uCheckSum.u32Register = (uint32_t)0 - uCheckSum.u32Register;
    return uCheckSum.u32Register;
}













































/* !!! csmon.h interfaces below !!! */










































bool CSMON_bUseDisconnectOnTimeout = CSMON_USE_DISCONNECT_ON_TIMEOUT;
uint32_t CSMON_u32FreeRunTimerNow;
uint32_t CSMON_u32FreeRunTimerPrev;
uint32_t CSMON_u32FreeRunTimerDiff;
uint32_t CSMON_u32NoMessageTime = 0;
uint16_t CSMON_u16NoMessageTimeout_msec = CSMON_NO_MESSAGE_TIMEOUT_MILLISECONDS;
uint16_t CSMON_u16NoMessageTimeoutOld_msec = CSMON_NO_MESSAGE_TIMEOUT_MILLISECONDS;
uint32_t CSMON_u32NoMessageTimeout_Ticks = (uint32_t)(CSMON_NO_MESSAGE_TIMEOUT_MILLISECONDS * 1000.0 * HMDRV_FREE_RUN_TIMER_FREQ_MHZ);




/* *****************************************************************************
 * CSMON_eInit   -   Call in main initialization block
 **************************************************************************** */

CSMON_eResponseCode_t CSMON_eInit(void)
{
    CSMON_eResponseCode_t eResponseCode = CSMON_RESPONSE_CODE_OK;
    
#if CSMON_PARAMETER_LIST_READ_ONLY == 0
    uint32_t u32Checksum;
    uint32_t u32ParamCheckSum;
#endif

    #if CSMON_PROCESS_CALL_HMLIB

    HMLIB_vInit();

    #define PPMFromPercent(x) (x*10000)
    #define PromilesFromPPM(x) (x/10000)

    CSMON_u32SCIErrorPPM = HMDRV_u32GetSCIErrorPPM();

    if (CSMON_u32SCIErrorPPM >= PPMFromPercent(10.0))
    {
        eResponseCode = CSMON_RESPONSE_FAIL_SCI_ERROR_MIN_10_PERCENT;
    }
    else if (CSMON_u32SCIErrorPPM >= PPMFromPercent(5.0))
    {
        eResponseCode = CSMON_RESPONSE_CODE_SCI_ERROR_MIN_5_PERCENT;
    }
    else if (CSMON_u32SCIErrorPPM >= PPMFromPercent(1.0))
    {
        eResponseCode = CSMON_RESPONSE_CODE_SCI_ERROR_MIN_1_PERCENT;
    }
    CSMON_u16SCIErrorPercent = PromilesFromPPM(CSMON_u32SCIErrorPPM);

    #endif

    u32FreeRunTimerSysClkDiv = TMR_u16GetFreeRunTimerPrescaller() + 1;  /* TMR_u16GetFreeRunTimerPrescaller -> 0 means 1:1; 6 means 1:7 */


    if (u32FreeRunTimerSysClkDiv != 1)
    {
        CSMON_u32NoMessageTimeout_Ticks /= u32FreeRunTimerSysClkDiv;
    }



#if _CSMON_IN_INTERNAL_RAM
    CSMON_pu32FileParameterString   = (uint32_t)CSMON_pu32FileParameterStringRealocatable; //DPN Table in internal RAM
    CSMON_psFileParameterAux        = CSMON_psFileParameterAuxRealocatable;                //DPN Table in internal RAM
#else
    CSMON_pu32FileParameterString   = (uint32_t)CSMON_PARAMETER_STRG_RW_ADDR;
    CSMON_psFileParameterAux        = (CSMON_sFileParameterAux_t*)CSMON_PARAMETER_SAUX_RW_ADDR;
    CSMON_pu32FileParameterReserved = (uint32_t)CSMON_PARAMETER_RSRV_RW_ADDR;              //DPN is not used ?
#endif





#if CSMON_PARAMETER_LIST_READ_ONLY

    CSMON_u16FileParameterUseMaxCnt = CSMON_FILE_PARAMETER_COUNT_MAX;
    CSMON_u16FileParameterListCount = CSMON_FILE_PARAMETER_COUNT_MAX;
    CSMON_u16FileParameterNameCount = CSMON_FILE_PARAMETER_COUNT_MAX;
    CSMON_u16FileParameterNumeric   = CSMON_FILE_PARAMETER_COUNT_MAX;
    CSMON_pasFileParameterList      = (__CONSTANT_MEMORY_MODIFIER__ CSMON_sFileParameterDefinitions_t*)&CSMON_sFileParameterList;
    CSMON_pasFileParameterShortName = (__CONSTANT_MEMORY_MODIFIER__ CSMON_sFileParameterShortName_t*)&CSMON_asFileParameterShortName;
    CSMON_pauFileParameterNumeric   = (__CONSTANT_MEMORY_MODIFIER__ CSMON_uFileParameterNumeric_t*)&CSMON_uFileParameterNumeric;
    CSMON_u16FileParameterFillCount = CSMON_FILE_PARAMETER_COUNT_MAX;

#else

    CSMON_u16FileParameterUseMaxCnt = CSMON_FILE_PARAMETER_COUNT_MAX;
    CSMON_u16FileParameterListCount = (uint16_t)(CSMON_PARAMETER_LIST_RW_SIZE / (sizeof(CSMON_sFileParameterDefinitions_t) * HMDRV_ADDR_WIDTH));
    CSMON_u16FileParameterNameCount = (uint16_t)(CSMON_PARAMETER_NAME_RW_SIZE / (sizeof(CSMON_sFileParameterShortName_t)   * HMDRV_ADDR_WIDTH));
    CSMON_u16FileParameterNumeric   = (uint16_t)(CSMON_PARAMETER_NUMB_RW_SIZE / (sizeof(CSMON_uFileParameterNumeric_t)     * HMDRV_ADDR_WIDTH));

    /* Use The Min of three */
    CSMON_u16FileParameterUseMaxCnt = MCLIB_u16Min(CSMON_u16FileParameterUseMaxCnt, CSMON_u16FileParameterListCount);
    CSMON_u16FileParameterUseMaxCnt = MCLIB_u16Min(CSMON_u16FileParameterUseMaxCnt, CSMON_u16FileParameterNameCount);
    CSMON_u16FileParameterUseMaxCnt = MCLIB_u16Min(CSMON_u16FileParameterUseMaxCnt, CSMON_u16FileParameterNumeric  );

#if _CSMON_IN_INTERNAL_RAM
    CSMON_pasFileParameterList      = CSMON_pasFileParameterListRealocatable;      //DPN Table in internal RAM
    CSMON_pasFileParameterShortName = CSMON_pasFileParameterShortNameRealocatable; //DPN Table in internal RAM
    CSMON_pauFileParameterNumeric   = CSMON_pauFileParameterNumericRealocatable;   //DPN Table in internal RAM
#else
    CSMON_pasFileParameterList      = (CSMON_sFileParameterDefinitions_t*)CSMON_PARAMETER_LIST_RW_ADDR;
    CSMON_pasFileParameterShortName = (CSMON_sFileParameterShortName_t*)CSMON_PARAMETER_NAME_RW_ADDR;
    CSMON_pauFileParameterNumeric   = (CSMON_uFileParameterNumeric_t*)CSMON_PARAMETER_NUMB_RW_ADDR;
#endif


    /* Check Valid Table */
    CSMON_sFileParameterCount.u32ParameterListCheckSum   = CSMON_u32GetParameterListCheckSum();





    if (CSMON_psFileParameterAux != NULL)                  /* reset count if Checksum mismatch */
    {

        /* Check CSMON_psFileParameterAux Parameter Count */
        if((CSMON_psFileParameterAux->u16ParamCountUse != (CSMON_psFileParameterAux->u16ParamCountUseCheck ^ 0xFFFF))
        || (CSMON_psFileParameterAux->u16ParamCountMax != (CSMON_psFileParameterAux->u16ParamCountMaxCheck ^ 0xFFFF))
        || (CSMON_psFileParameterAux->u16ModbusCountUse != (CSMON_psFileParameterAux->u16ModbusCountUseCheck ^ 0xFFFF))
        || (CSMON_psFileParameterAux->u16ModbusCountMax != (CSMON_psFileParameterAux->u16ModbusCountMaxCheck ^ 0xFFFF)))
        {
            CSMON_psFileParameterAux->u16ParamCountMax = 0;
            CSMON_psFileParameterAux->u16ParamCountUse = 0;
            CSMON_psFileParameterAux->u16ModbusCountMax = 0;
            CSMON_psFileParameterAux->u16ModbusCountUse = 0;
        }






        /* Max Count Parameters */
        if (CSMON_psFileParameterAux->u16ParamCountMax != CSMON_u16FileParameterUseMaxCnt)
        {
            CSMON_psFileParameterAux->u16ParamCountMax = CSMON_u16FileParameterUseMaxCnt;
            CSMON_psFileParameterAux->u16ParamCountMaxCheck = CSMON_u16FileParameterUseMaxCnt ^ 0xFFFF;
        }



        /* Used Parameters */
        CSMON_u16FileParameterFillCount = CSMON_psFileParameterAux->u16ParamCountUse;       /* first set the count to use */
        if (CSMON_u16FileParameterFillCount <= CSMON_u16FileParameterUseMaxCnt)
        {
            u32Checksum = CSMON_u32GetParameterCheckSum();                                  /* calculate checksum for this count */
            u32ParamCheckSum = CSMON_psFileParameterAux->uParamCheckSum.u32Register;
            if (u32ParamCheckSum != u32Checksum)                                            /* reset count if Checksum mismatch */
            {
                CSMON_u16FileParameterFillCount = 0;
            }
        }
        else
        {
            CSMON_u16FileParameterFillCount = 0;
        }
    }
    else
    {
        CSMON_u16FileParameterFillCount = 0;
    }

    if (CSMON_psFileParameterAux != NULL)
    {
        /* Max Count Modbus Parameters */
        if (CSMON_psFileParameterAux->u16ModbusCountMax != (uint16_t)(MODBUSREG_HOLDREG_LIST_RW_SIZE / (sizeof(HMMODBUS_sRegisterAddressTable_t) * HMDRV_ADDR_WIDTH)) )
        {
            CSMON_psFileParameterAux->u16ModbusCountMax = (uint16_t)(MODBUSREG_HOLDREG_LIST_RW_SIZE / (sizeof(HMMODBUS_sRegisterAddressTable_t) * HMDRV_ADDR_WIDTH));
            CSMON_psFileParameterAux->u16ModbusCountMaxCheck = (uint16_t)(MODBUSREG_HOLDREG_LIST_RW_SIZE / (sizeof(HMMODBUS_sRegisterAddressTable_t) * HMDRV_ADDR_WIDTH)) ^ 0xFFFF;
        }
        MODBUSREG_vSetModbusParamCount(CSMON_psFileParameterAux->u16ModbusCountUse);
    }


#endif

    MODBUSREG_vSetParamStringAddress   (CSMON_pu32FileParameterString);
#if CSMON_PARAMETER_LIST_READ_ONLY == 0
    MODBUSREG_vSetParamAuxAddress      ((uint32_t)(HMDRV_ADDR_SIZE_LONG)CSMON_psFileParameterAux);
#endif
    MODBUSREG_vSetParamReservedAddress (CSMON_pu32FileParameterReserved);





    MODBUSREG_vSetParamCount(CSMON_u16FileParameterFillCount);
    MODBUSREG_vSetParamListAddress((HMDRV_U32)(HMDRV_ADDR_SIZE_LONG)(CSMON_pasFileParameterList));
    MODBUSREG_vSetParamNameAddress((HMDRV_U32)(HMDRV_ADDR_SIZE_LONG)(CSMON_pasFileParameterShortName));
    MODBUSREG_vSetParamNumbAddress((HMDRV_U32)(HMDRV_ADDR_SIZE_LONG)(CSMON_pauFileParameterNumeric));

    MODBUSREG_vSetParamRecorderConfigAddress((HMDRV_U32)(HMDRV_ADDR_SIZE)&sRecConfigParam);
    MODBUSREG_vSetParamScopeConfigAddress((HMDRV_U32)(HMDRV_ADDR_SIZE)&sScopeConfigParam);

    MODBUSREG_vSetFileRTC_FunctionAfterWriteFileRecord((uint32_t)(HMDRV_PTR_FUNC_SIZE)(CSMON_pfFunctionExecute)CSMON_vSetDateTimeRequest);
    MODBUSREG_vSetFileRTC_FunctionAfterReadFileRecord((uint32_t)(HMDRV_PTR_FUNC_SIZE)(CSMON_pfFunctionExecute)CSMON_vSetDateTimeAutorization);
    MODBUSREG_vSetFileParamCount_FunctionBeforeReadFileRecord((uint32_t)(HMDRV_PTR_FUNC_SIZE)(CSMON_pfFunctionExecute)CSMON_vFileParameterCountCheckSumUpdate);

    CSMON_au32FileAutorization_FunctionAfterWriteFileRecord[CSMON_FILE_RECORD_AUTORIZATION_ACCESS_PASSWRD] = (uint32_t)(HMDRV_PTR_FUNC_SIZE)(CSMON_pfFunctionExecute)CSMON_vAutorizationReset;
    MODBUSREG_vSetFileAutorization_FunctionAfterWriteFileRecordTable((uint32_t)(HMDRV_ADDR_SIZE)&CSMON_au32FileAutorization_FunctionAfterWriteFileRecord[0]);

    CSMON_au32FileParameterDefinitionsFileRecordElementsRealAddressTable[0] = (uint32_t)(HMDRV_ADDR_SIZE_LONG)(&(CSMON_pasFileParameterList[0].ValueAccessKey.sValueAccessKey.u16ValueAccessKeyStoradeID));// was (uint32_t)(HMDRV_ADDR_SIZE)(&CSMON_pasFileParameterList[0].u16ValueAccessKeyStoradeID);
    CSMON_au32FileParameterDefinitionsFileRecordElementsRealAddressTable[1] = (uint32_t)(HMDRV_ADDR_SIZE)(&CSMON_asFileParameterDefinitionsFromAccessLevel);
#if CSMON_PACKED_16_BIT_PARAMETER_DEFINITIONS
    //CSMON_au32FileParameterDefinitionsFileRecordElementsRealAddressTable[2] = (uint32_t)(HMDRV_ADDR_SIZE_LONG)(&((CSMON_sFileParameterDefinitions16Bit_t)(CSMON_pasFileParameterList).u16PaddingByteBitsCount));//was (uint32_t)(HMDRV_ADDR_SIZE)(&CSMON_pasFileParameterList[0].u8BitsCount);
    CSMON_au32FileParameterDefinitionsFileRecordElementsRealAddressTable[2] = (uint32_t)(HMDRV_ADDR_SIZE_LONG)&((((__CONSTANT_MEMORY_MODIFIER__ CSMON_sFileParameterDefinitions16Bit_t*)CSMON_pasFileParameterList))[0].u16PaddingByteBitsCount);//was (uint32_t)(HMDRV_ADDR_SIZE)(&CSMON_pasFileParameterList[0].u8BitsCount);
#else
    CSMON_au32FileParameterDefinitionsFileRecordElementsRealAddressTable[2] = (uint32_t)(HMDRV_ADDR_SIZE_LONG)(&(CSMON_pasFileParameterList[0].u8BitsCount));//was (uint32_t)(HMDRV_ADDR_SIZE)(&CSMON_pasFileParameterList[0].u8BitsCount);
#endif
    CSMON_au32FileParameterDefinitionsFileRecordElementsSizeTable[0] = (uint32_t)(HMDRV_ADDR_SIZE)(&CSMON_au8FileParameterListElementsSizeTablePart1);
    CSMON_au32FileParameterDefinitionsFileRecordElementsSizeTable[1] = (uint32_t)(HMDRV_ADDR_SIZE)(&CSMON_au8FileParameterListElementsSizeTablePart2);
    CSMON_au32FileParameterDefinitionsFileRecordElementsSizeTable[2] = (uint32_t)(HMDRV_ADDR_SIZE)(&CSMON_au8FileParameterListElementsSizeTablePart3);

    CSMON_au32FileParameterDefinitionsFileRecordElementsRecordToRecordOffsetSizeBytesTable[0] = sizeof(CSMON_sFileParameterDefinitions_t);
    CSMON_au32FileParameterDefinitionsFileRecordElementsRecordToRecordOffsetSizeBytesTable[1] = sizeof(CSMON_sFileParameterDefinitionsFromAccessLevel_t);
    CSMON_au32FileParameterDefinitionsFileRecordElementsRecordToRecordOffsetSizeBytesTable[2] = sizeof(CSMON_sFileParameterDefinitions_t);

    //&(((CSMON_sFileParameterDefinitions16Bit_t)(*(&CSMON_pasFileParameterList)))[0].u16PaddingByteBitsCount)


    CSMON_vInitRecorders();
    CSMON_vInitScopes();

    MODBUSREG_vInit();  /* Consider if needed to be somewhere above or in main initialization block */

    CSMON_pu32HoldingRegistersAddressTable = MODBUSREG_u32GetRegisterAddressTable();
    CSMON_pu32HoldingRegistersAddressTableInRam = MODBUSREG_u32GetRegisterAddressTableInRam();
    CSMON_pu32HoldingRegistersAccessTableInRam = MODBUSREG_u32GetRegisterAccessTableInRam();

    CSMON_vSetupHoldingRegistersAndCoilsAddressTableFromCSMONParameterList(CSMON_sFileAutorization.u16AccessLevelNow);
    CSMON_vSetupInputRegistersAndCoilsAddressTableFromCSMONParameterList(CSMON_sFileAutorization.u16AccessLevelNow);

    

    /* Clear String Table Size On First Run */
    if (CSMON_psFileParameterAux != NULL)
    {
        if (( DATETIME_BUILD_LO != CSMON_psFileParameterAux->uParamDateTimeCopy.au16Word[0]) ||
            ( DATETIME_BUILD_HI != CSMON_psFileParameterAux->uParamDateTimeCopy.au16Word[1])    )
        {
            CSMON_psFileParameterAux->uParamDateTimeCopy.au16Word[0] = DATETIME_BUILD_LO;
            CSMON_psFileParameterAux->uParamDateTimeCopy.au16Word[1] = DATETIME_BUILD_HI;
            #if BOARDCFG_USE_CSMON_STRING_TABLE
            if (CSMON_pu32FileParameterString != NULL)
            {
                ((uint16_t*)CSMON_pu32FileParameterString)[0] = 0;        /* Clear Length Of String Table */
                ((uint16_t*)CSMON_pu32FileParameterString)[1] = 0;        /* Clear Length Of String Table */
            }
            #endif /* BOARDCFG_USE_CSMON_STRING_TABLE */
        }
    }
    #if BOARDCFG_USE_CSMON_STRING_TABLE
    else
    if (CSMON_pu32FileParameterString != NULL)
    {
        ((uint16_t*)CSMON_pu32FileParameterString)[0] = 0;        /* Clear Length Of String Table */
        ((uint16_t*)CSMON_pu32FileParameterString)[1] = 0;        /* Clear Length Of String Table */
    }
    #endif /* BOARDCFG_USE_CSMON_STRING_TABLE */


    return eResponseCode;
}


/* Clear String Table Size On First Run */
#if 0
#if (CSMON_PARAMETER_STRG_RW_ADDR > 0)
const uint32_t StringTableSize @CSMON_PARAMETER_STRG_RW_ADDR = 0;
#endif
#endif


/* *****************************************************************************
 * CSMON_eProcess   -   Call in main loop
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eProcess (void)
{
    CSMON_eResponseCode_t eResponseCode = CSMON_RESPONSE_CODE_OK;

#ifdef __XC16
        /*to do for pic*/
#else

    /* Get watchdog period Limit Check Prepare */
    CSMON_u32FreeRunTimerProcessBegin = CSMON_u32GetFreeRunningTimerTicks();
    bSkipProcessFlag = 0;   /* reset bSkipProcessFlag */
    if (CSMON_bWatchdogCalcNeeded)
    {
        CSMON_bWatchdogCalcNeeded = 0;
        CSMON_u16WatchdogPrescaler = (HWREGH(WD_BASE + SYSCTL_O_WDCR) & SYSCTL_WDCR_WDPS_M);
        CSMON_u32WatchdogPeriod_usec = (uint32_t)((1000000.0*512*256)/10000000.0) * CSMON_u16WatchdogPrescaler;  /* 1usec; 512WatchdogDiv; 256WatchdogCnt; 10MHz INTOSC */
        CSMON_u32WatchdogPeriod_Ticks = ((uint32_t)CSMON_u32WatchdogPeriod_usec * u32FreeRunTimerSysClkMHz) / u32FreeRunTimerSysClkDiv;
        CSMON_u32MaxTimeInISR_Ticks = ((uint32_t)CSMON_u16MaxTimeInISR_usec * u32FreeRunTimerSysClkMHz) / u32FreeRunTimerSysClkDiv;
        CSMON_u32MinTimeProcessExit_Ticks = ((uint32_t)CSMON_u16MinTimeProcessExit_usec * u32FreeRunTimerSysClkMHz) / u32FreeRunTimerSysClkDiv;
        CSMON_u32MaxTimeTillWdtClear_Ticks = ((uint32_t)CSMON_u16MaxTimeTillWdtClear_usec * u32FreeRunTimerSysClkMHz) / u32FreeRunTimerSysClkDiv;
        CSMON_u32MinTimeProcessAllowedNeeded_Ticks = ((uint32_t)CSMON_u16MinTimeProcessAllowedNeeded_usec * u32FreeRunTimerSysClkMHz) / u32FreeRunTimerSysClkDiv;

        CSMON_u32MaxTimeProcess_Ticks = CSMON_u32WatchdogPeriod_Ticks - CSMON_u32MaxTimeInISR_Ticks;
        CSMON_u32MaxTimeProcess_Ticks += (CSMON_u32MinTimeProcessExit_Ticks - CSMON_u32MaxTimeTillWdtClear_Ticks);

        CSMON_u32MaxTimeTillWdtClearISRs = CSMON_u32MaxTimeTillWdtClear_Ticks / CSMON_u32MinTimeProcessExit_Ticks;  /* Count ISRs till WdtClear */
        CSMON_u32MaxTimeProcess_Ticks = CSMON_u32MaxTimeTillWdtClearISRs * CSMON_u32MaxTimeInISR_Ticks;
        CSMON_s32MaxTimeProcessAllowedCalc_Ticks = CSMON_u32WatchdogPeriod_Ticks - CSMON_u32MaxTimeProcess_Ticks;
        if (CSMON_s32MaxTimeProcessAllowedCalc_Ticks < (int32_t)CSMON_u32MinTimeProcessAllowedNeeded_Ticks)
        {
            CSMON_u32MaxTimeProcessAllowed_Ticks = CSMON_u32MinTimeProcessAllowedNeeded_Ticks;
        }
        else
        {
            CSMON_u32MaxTimeProcessAllowed_Ticks = (uint32_t)CSMON_s32MaxTimeProcessAllowedCalc_Ticks;
        }
    }

    /* Where check needed */
    CSMON_u32FreeRunTimerProcessNow = CSMON_u32GetFreeRunningTimerTicks() - CSMON_u32FreeRunTimerProcessBegin;
    if (CSMON_u32FreeRunTimerProcessNow > CSMON_u32MaxTimeProcessAllowed_Ticks)
    {
        u16SkipProcessCounter++;
        u16SkipProcessTag = 1;  /* Tag Of The interruption (0 - there was no interruption) */
        bSkipProcessFlag = 1;   /* Flag interruption for code after to Skip, not to run */
    }


    if (CSMON_bResetMaxMeasure)
    {
        CSMON_bResetMaxMeasure = 0;
        CSMON_u32FreeRunTimerProcessMax = 0;
        CSMON_u32FreeRunTimerAccessChangeMax = 0;
        CSMON_u32FreeRunTimerPasswordHashMax = 0;
        CSMON_u32FreeRunTimerCheckSumListMax = 0;
        CSMON_u32FreeRunTimerCheckSumNameMax = 0;
        CSMON_u32FreeRunTimerCheckSumNumbMax = 0;
        CSMON_u32FreeRunTimerCheckSumStrgMax = 0;
    }

#endif

    #if CSMON_PROCESS_CALL_HMLIB

    GPIO_writePin(STAT_LED_G_PIN, STAT_LED_DISABLE); //DPN
    HMLIB_vProcess();
    GPIO_writePin(STAT_LED_G_PIN, STAT_LED_ENABLE);  //DPN

    #if (HMDRV_USE_RECORDER_PARAMETERS) && (!(HMDRV_DISABLE))
    #if HMDRV_RECORDER_PARAM_HANDLE_COUNT > 0
    CSMON_vRecorderParamSetupProcess();
    #endif
    #endif

    #endif


#ifdef __XC16
        /*to do for pic*/
#else
    /* Disconnect on timeout */
    if (CSMON_bUseDisconnectOnTimeout)
    {
        CSMON_u32FreeRunTimerNow = CSMON_u32GetFreeRunningTimerTicks();
        CSMON_u32FreeRunTimerDiff = CSMON_u32FreeRunTimerNow - CSMON_u32FreeRunTimerPrev;
        CSMON_u32FreeRunTimerPrev = CSMON_u32FreeRunTimerNow;
        if (HMMODBUS_bGetReceivedMessage())
        {
            //CSMON_u32FreeRunTimerReceivedMessage = CSMON_u32GetFreeRunningTimerTicks();
            CSMON_u32FreeRunTimerDiff = 0;
            CSMON_u32NoMessageTime = 0;
        }
        CSMON_u32NoMessageTime = MCLIB_u32AddSat_u32u32(CSMON_u32NoMessageTime, CSMON_u32FreeRunTimerDiff);
        if (CSMON_sFileAutorization.u16AccessLevelNow != CSMON_ACCESS_DENIED)
        {
            if (CSMON_u32NoMessageTime >= CSMON_u32NoMessageTimeout_Ticks)
            {
                CSMON_sFileAutorization.u16AccessLevelReq = CSMON_ACCESS_DENIED;
            }
        }
    }

    if (CSMON_u16NoMessageTimeoutOld_msec != CSMON_u16NoMessageTimeout_msec)
    {
        CSMON_u16NoMessageTimeoutOld_msec = CSMON_u16NoMessageTimeout_msec;
        CSMON_u32NoMessageTimeout_Ticks = ((uint64_t)((uint32_t)CSMON_u16NoMessageTimeout_msec * 1000) * u32FreeRunTimerSysClkDiv) / (u32FreeRunTimerSysClkMHz);
    }
#endif



    if (CSMON_bSetDateTimeRequest)
    {
        CSMON_bSetDateTimeRequest = false;

        CSMON_vSetDateTime
        (
            CSMON_sDateTime.u8Seconds,
            CSMON_sDateTime.u8Minutes,
            CSMON_sDateTime.u8Hours,
            CSMON_sDateTime.u8WeekDays,
            CSMON_sDateTime.u8Day,
            CSMON_sDateTime.u8Month,
            CSMON_sDateTime.u8Year
        );
    }


    CSMON_bGetDateTimeRequest = true;
    if (CSMON_bGetDateTimeRequest)
    {
        CSMON_bGetDateTimeRequest = false;

        CSMON_vGetDateTime
        (
            &CSMON_sDateTime.u8Seconds,
            &CSMON_sDateTime.u8Minutes,
            &CSMON_sDateTime.u8Hours,
            &CSMON_sDateTime.u8WeekDays,
            &CSMON_sDateTime.u8Day,
            &CSMON_sDateTime.u8Month,
            &CSMON_sDateTime.u8Year
        );
        CSMON_sDateTime.u32Microseconds = CSMON_u32GetFreeRunningTimerMicroseconds();
    }


    CSMON_vAutorizationReset();

    /* Preliminary Calculate All Password */
    #if CSMON_FILE_AUTORIZATION_WITH_HASH
    if (CSMON_bGetDateTimeAutorization)     /* to do: times to fail not only once */
    {
        __CONSTANT_MEMORY_MODIFIER__ int8_t* ps8AccessLevelPassword;
        ps8AccessLevelPassword = (__CONSTANT_MEMORY_MODIFIER__ int8_t*)&CSMON_asAccessLevelPasswords[1].as8Password[0];   /* password internal that must be matched */
        CSMON_bPasswordGenerate[1] = CSMON_bPasswordGenerateFromHash((char*)&sHashKey, (char*)ps8AccessLevelPassword, (char*) &CSMON_sFileAutorizationHashOut[1].as8Password[0], &CSMON_nHashOutputLength[1]);
        ps8AccessLevelPassword = (__CONSTANT_MEMORY_MODIFIER__ int8_t*)&CSMON_asAccessLevelPasswords[2].as8Password[0];   /* password internal that must be matched */
        CSMON_bPasswordGenerate[2] = CSMON_bPasswordGenerateFromHash((char*)&sHashKey, (char*)ps8AccessLevelPassword, (char*) &CSMON_sFileAutorizationHashOut[2].as8Password[0], &CSMON_nHashOutputLength[2]);
        ps8AccessLevelPassword = (__CONSTANT_MEMORY_MODIFIER__ int8_t*)&CSMON_asAccessLevelPasswords[3].as8Password[0];   /* password internal that must be matched */
        CSMON_bPasswordGenerate[3] = CSMON_bPasswordGenerateFromHash((char*)&sHashKey, (char*)ps8AccessLevelPassword, (char*) &CSMON_sFileAutorizationHashOut[3].as8Password[0], &CSMON_nHashOutputLength[3]);
        ps8AccessLevelPassword = (__CONSTANT_MEMORY_MODIFIER__ int8_t*)&CSMON_asAccessLevelPasswords[4].as8Password[0];   /* password internal that must be matched */
        CSMON_bPasswordGenerate[4] = CSMON_bPasswordGenerateFromHash((char*)&sHashKey, (char*)ps8AccessLevelPassword, (char*) &CSMON_sFileAutorizationHashOut[4].as8Password[0], &CSMON_nHashOutputLength[4]);
        CSMON_bGetDateTimeAutorization = false;
    }
    #endif


    if (CSMON_u16CurrentAccessLevelPrev != CSMON_sFileAutorization.u16AccessLevelNow)
    {
        CSMON_u16CurrentAccessLevelPrev = CSMON_sFileAutorization.u16AccessLevelNow;

        CSMON_u32FreeRunTimerAccessChangeBegin = CSMON_u32GetFreeRunningTimerTicks();   /* Measure max Time in AccessChange Prepare */

        CSMON_vPrepareFileParameterAccessList();

        if(CSMON_bUseAccessLevelForHoldingRegistersAndCoils)
        {
            CSMON_vSetupHoldingRegistersAndCoilsAddressTableFromCSMONParameterList(CSMON_sFileAutorization.u16AccessLevelNow);
        }
        else
        {
            /* AccessAll */
            CSMON_vSetupHoldingRegistersAndCoilsAddressTableAccessAll();
        }

        if(CSMON_bUseAccessLevelForInputRegistersAndCoils)
        {
            CSMON_vSetupInputRegistersAndCoilsAddressTableFromCSMONParameterList(CSMON_sFileAutorization.u16AccessLevelNow);
        }
        else
        {
            /* AccessAll */
            CSMON_vSetupInputRegistersAndCoilsAddressTableAccessAll();
        }


        /* Measure max Time in Autorization */
        CSMON_u32FreeRunTimerAccessChangeEnd = CSMON_u32GetFreeRunningTimerTicks() - CSMON_u32FreeRunTimerAccessChangeBegin;
        if (CSMON_u32FreeRunTimerAccessChangeEnd > CSMON_u32FreeRunTimerAccessChangeMax)
        {
            CSMON_u32FreeRunTimerAccessChangeMax = CSMON_u32FreeRunTimerAccessChangeEnd;
        }
    }



    /* Last check and measure max Time in Process */
    CSMON_u32FreeRunTimerProcessEnd = CSMON_u32GetFreeRunningTimerTicks() - CSMON_u32FreeRunTimerProcessBegin;
    if (CSMON_u32FreeRunTimerProcessEnd > CSMON_u32MaxTimeProcessAllowed_Ticks)
    {
        u16SkipProcessCounter++;
        u16SkipProcessTag = 65535;  /* Tag Of The interruption (0 - there was no interruption) */
        bSkipProcessFlag = 1;   /* Flag interruption for code after to Skip, not to run */
    }
    if (CSMON_u32FreeRunTimerProcessEnd > CSMON_u32FreeRunTimerProcessMax)
    {
        CSMON_u32FreeRunTimerProcessMax = CSMON_u32FreeRunTimerProcessEnd;
    }

    return eResponseCode;
}

/* *****************************************************************************
 * CSMON_eSetServerOnStatus   -   Call when Control Run Status Changed
 *
 * bInput - boolean
 *                  false   - Off Status - Control Stopped
 *                  true    - Run Status - Control Running
 *
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eSetServerOnStatus (bool bInput)
{
    CSMON_eResponseCode_t eResponseCode = CSMON_RESPONSE_CODE_OK;

    if (bInput)
    {
        HMMODBUS_vSetServerOnStatus(0xFF);
    }
    else
    {
        HMMODBUS_vSetServerOnStatus(0x00);
    }
    return eResponseCode;
}

/* *****************************************************************************
 * CSMON_u32GetSCIError_PPM   -   Error Baud Rate (per Million)
 **************************************************************************** */
uint32_t CSMON_u32GetBaudError_PPM (CSMON_eIDPeripheral_t eID)
{
    uint32_t u32SCIErrorPPM = 0;
    /* if (eID == CSMON_ID_PERIPHERAL_SCI_MODBUS) */
        u32SCIErrorPPM = CSMON_u32SCIErrorPPM;  /* for first implementation MODBUS and HEXMON share same SCI */
    return u32SCIErrorPPM;
}


/* *****************************************************************************
 * CSMON_eSetFlagProcessPassed   -   Call at end of Priority Process Execution
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eSetFlagProcessPassed (CSMON_eIDProcess_t eID)
{
    CSMON_eResponseCode_t eResponseCode = CSMON_RESPONSE_CODE_OK;

    if (eID < CSMON_ID_PROCESS_COUNT)
    {
        CSMON_abFlagProcessPassed[eID] = true;
    }
    else
    {
        eResponseCode = CSMON_RESPONSE_FAIL_NOT_IMPLEMENTED;
    }

#if CSMON_RECORDERS_IN_FLAG_PROCESS_PASSED_INTERFACE

    #if (HMDRV_USE_RECORDER_PARAMETERS) && (!(HMDRV_DISABLE))
    #if HMDRV_RECORDER_PARAM_HANDLE_COUNT > 0
    CSMON_vRecorderParamStartStopProcess();
    #endif
    #endif

    HMLIB_vProcessRecorders();

#endif



    return eResponseCode;
}

/* *****************************************************************************
 * CSMON_eSetTimerPeriodISRFunctionRegister   -   Register Function Call in
 * Timer Period ISR
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eSetTimerPeriodISRFunctionRegister (CSMON_pfVoid_t pfInput)
{
    CSMON_eResponseCode_t eResponseCode = CSMON_RESPONSE_CODE_OK;

    TIMER_vSetupTimerMainCSMONFunction(pfInput);
    return eResponseCode;
}


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
        )
{
#if CSMON_PARAMETER_LIST_READ_ONLY == 0
    CSMON_eResponseCode_t eResponseCode = CSMON_RESPONSE_CODE_OK;

    uAnyType64_t Max;
    uAnyType64_t Min;
    uAnyType64_t Def;

    uint16_t u16IndexRD;
    uint16_t u16IndexWR;
    bool bNameFullWrite;
    bool bUnitFullWrite;

    int16_t s16FileParameterFillIndex;
    CSMON_uFileParameterValueAccessKey_t u32ValueAccessKey;
    uint16_t u16FileParameterFillIndex;


    HMMODBUS_sRegisterAddressTable_t* pasHoldingRegistersAddressTable;
    HMMODBUS_uRegisterAddressTableInRam_t* pasHoldingRegistersAddressTableInRam;
    HMMODBUS_sRegisterAccessTableInRam_t* pasHoldingRegistersAccessTableInRam;
    uint16_t u16HoldRegFillCount;
    uint16_t u16HoldRegUsedCount;

    uWord32_t uRealAddress;
    uWord32_t uExecuteFunction;




    MEMORY_MODIFIER CSMON_sFileParameterDefinitions_t* pasFileParameterList;
    MEMORY_MODIFIER CSMON_uFileParameterNumeric_t* pauFileParameterNumeric;
    MEMORY_MODIFIER CSMON_sFileParameterShortName_t* pasFileParameterShortName;

    /* Local Variables (EMIF Acceleration) consider to remove - time not optimized */
    CSMON_sFileParameterDefinitions_t sFileParameterList;
    CSMON_uFileParameterNumeric_t uFileParameterNumeric;
    //CSMON_sFileParameterShortName_t sFileParameterShortName;
    HMMODBUS_sRegisterAddressTable_t sHoldingRegistersAddressTable;



    PARAMETER_uAttributes_t uAttributes;

    /* Check Valid Parameter */
    if (u32RealAddress == NULL)
    {
        eResponseCode = CSMON_RESPONSE_FAIL_CSMON_PARAM_INPUTS_INVALID;
    }
    else
    {
        u16HoldRegFillCount = MODBUSREG_u16GetHoldRegFillCount();
        u16HoldRegUsedCount = MODBUSREG_u16GetHoldRegUsedCount();

        u32ValueAccessKey.sValueAccessKey.u16ValueAccessKeyStoradeID = u16ParameterIndexID;
        u32ValueAccessKey.sValueAccessKey.bArray = false;
        u32ValueAccessKey.sValueAccessKey.bBitField = false;
        u32ValueAccessKey.sValueAccessKey.u6Reserved = 0;
        u32ValueAccessKey.sValueAccessKey.u8ValueAccessKeyStartBitIndex = u8StartBitOrArrayElementCount;            /* size elements if array */



        /* try find Table Index From CSMON ID */
        s16FileParameterFillIndex = CSMON_u16FindIndexParameterTable(u32ValueAccessKey.u32Register);
        if (s16FileParameterFillIndex < 0)
        {
            u16FileParameterFillIndex = CSMON_u16FileParameterFillCount;
        }
        else
        {
            u16FileParameterFillIndex = s16FileParameterFillIndex;
        }
        
        if ((CSMON_pasFileParameterList == NULL) || (CSMON_pauFileParameterNumeric == NULL) || (CSMON_pasFileParameterShortName == NULL))
        {
            eResponseCode = CSMON_RESPONSE_FAIL_CSMON_PARAM_INTERNAL_NULL_POINTER_TO_ARRAY_DETECTED;
        }
        else if ((u16FileParameterFillIndex < CSMON_u16FileParameterUseMaxCnt) && (u16HoldRegFillCount < u16HoldRegUsedCount))
        {

            pasHoldingRegistersAddressTable = (HMMODBUS_sRegisterAddressTable_t*)(HMDRV_ADDR_TO_PNTR_SIZE)CSMON_pu32HoldingRegistersAddressTable;
            pasHoldingRegistersAddressTableInRam = (HMMODBUS_uRegisterAddressTableInRam_t*)(HMDRV_ADDR_TO_PNTR_SIZE)CSMON_pu32HoldingRegistersAddressTableInRam;
            pasHoldingRegistersAccessTableInRam = (HMMODBUS_sRegisterAccessTableInRam_t*)(HMDRV_ADDR_TO_PNTR_SIZE)CSMON_pu32HoldingRegistersAccessTableInRam;



            pasFileParameterList = CSMON_pasFileParameterList;
            pauFileParameterNumeric = CSMON_pauFileParameterNumeric;
            pasFileParameterShortName = CSMON_pasFileParameterShortName;

            


            uAttributes.u16Register = u16ParamAttributes;


            uRealAddress.u32Register = u32RealAddress;
            uExecuteFunction.u32Register = NULL;

            pasHoldingRegistersAddressTableInRam[u16HoldRegFillCount].HMMODBUS_sRegisterAddressTableInRam16bit.au16RealAddress[0] = uRealAddress.au16Word[0];
            pasHoldingRegistersAddressTableInRam[u16HoldRegFillCount].HMMODBUS_sRegisterAddressTableInRam16bit.au16RealAddress[1] = uRealAddress.au16Word[1];
            pasHoldingRegistersAddressTableInRam[u16HoldRegFillCount].HMMODBUS_sRegisterAddressTableInRam16bit.au16PointerToProcessFunction[0] = uExecuteFunction.au16Word[0];
            pasHoldingRegistersAddressTableInRam[u16HoldRegFillCount].HMMODBUS_sRegisterAddressTableInRam16bit.au16PointerToProcessFunction[1] = uExecuteFunction.au16Word[1];

            pasHoldingRegistersAccessTableInRam[u16HoldRegFillCount].uFlags.sFlags.bReadDisabled = true;
            pasHoldingRegistersAccessTableInRam[u16HoldRegFillCount].uFlags.sFlags.bWriteDisabled = true;


            sHoldingRegistersAddressTable.u16VirtAddress = u16ParameterIndexID;
            sHoldingRegistersAddressTable.uFlags.u16Register = 0;
            sHoldingRegistersAddressTable.uSize.u16Register = 0;



            uFileParameterNumeric.sRegister.u64Default = (uint64_t)u32Def;
            uFileParameterNumeric.sRegister.u64Maximum = (uint64_t)u32Max;
            uFileParameterNumeric.sRegister.u64Minimum = (uint64_t)u32Min;
            uFileParameterNumeric.sRegister.s64Offset = (int64_t)0;
            uFileParameterNumeric.sRegister.s64Multply = 0;

            Def.u64Register = (uint64_t)u32Def;
            Max.u64Register = (uint64_t)u32Max;
            Min.u64Register = (uint64_t)u32Min;

            uFileParameterNumeric.sStructure.sDataType.eVisualType = CSMON_VISUAL_TYPE_HEX;

            switch(uAttributes.sAttributes.eDataType)
            {
            case PAR_TYPE_A_UINT08:
                sHoldingRegistersAddressTable.uSize.sSize.u8SizeElement = 0;   /* string */
                uFileParameterNumeric.sStructure.sDataType.eDataType = CSMON_DATA_TYPE_U8;
                uFileParameterNumeric.sRegister.u64Default = (uint64_t)Def.u8Register & 0x000000FF;
                uFileParameterNumeric.sRegister.u64Maximum = (uint64_t)Max.u8Register & 0x000000FF;
                uFileParameterNumeric.sRegister.u64Minimum = (uint64_t)Min.u8Register & 0x000000FF;
                u32ValueAccessKey.sValueAccessKey.bArray = true;
                sHoldingRegistersAddressTable.uFlags.sFlags.bArray = true;
                sHoldingRegistersAddressTable.uFlags.sFlags.u8ElementCount = u8StartBitOrArrayElementCount;
                break;
            case PAR_TYPE_UINT08:
                sHoldingRegistersAddressTable.uSize.sSize.u8SizeElement = 1;   /* byte */
                uFileParameterNumeric.sStructure.sDataType.eDataType = CSMON_DATA_TYPE_U8;
                uFileParameterNumeric.sRegister.u64Default = (uint64_t)Def.u8Register & 0x000000FF;
                uFileParameterNumeric.sRegister.u64Maximum = (uint64_t)Max.u8Register & 0x000000FF;
                uFileParameterNumeric.sRegister.u64Minimum = (uint64_t)Min.u8Register & 0x000000FF;
                break;
            case PAR_TYPE_UINT16:
                sHoldingRegistersAddressTable.uSize.sSize.u8SizeElement = 2;   /* 16-bit */
                uFileParameterNumeric.sStructure.sDataType.eDataType = CSMON_DATA_TYPE_U16;
                uFileParameterNumeric.sRegister.u64Default = (uint64_t)Def.u16Register & 0x0000FFFF;
                uFileParameterNumeric.sRegister.u64Maximum = (uint64_t)Max.u16Register & 0x0000FFFF;
                uFileParameterNumeric.sRegister.u64Minimum = (uint64_t)Min.u16Register & 0x0000FFFF;
                break;
            case PAR_TYPE_SINT16:
                sHoldingRegistersAddressTable.uSize.sSize.u8SizeElement = 2;   /* 16-bit */
                uFileParameterNumeric.sStructure.sDataType.eDataType = CSMON_DATA_TYPE_S16;
                uFileParameterNumeric.sRegister.u64Default = (uint64_t)Def.s16Register & 0x0000FFFF;
                uFileParameterNumeric.sRegister.u64Maximum = (uint64_t)Max.s16Register & 0x0000FFFF;
                uFileParameterNumeric.sRegister.u64Minimum = (uint64_t)Min.s16Register & 0x0000FFFF;
                break;
            case PAR_TYPE_WORD16:
                sHoldingRegistersAddressTable.uSize.sSize.u8SizeElement = 2;   /* 16-bit */
                uFileParameterNumeric.sStructure.sDataType.eDataType = CSMON_DATA_TYPE_U16;
                uFileParameterNumeric.sRegister.u64Default = (uint64_t)Def.u16Register & 0x0000FFFF;
                uFileParameterNumeric.sRegister.u64Maximum = (uint64_t)Max.u16Register & 0x0000FFFF;
                uFileParameterNumeric.sRegister.u64Minimum = (uint64_t)Min.u16Register & 0x0000FFFF;
                //uFileParameterNumeric.sStructure.sDataType.eVisualType = CSMON_VISUAL_TYPE_HEX;
                break;
            case PAR_TYPE_UINT32:
                sHoldingRegistersAddressTable.uSize.sSize.u8SizeElement = 4;   /* 32-bit */
                uFileParameterNumeric.sStructure.sDataType.eDataType = CSMON_DATA_TYPE_U32;
                uFileParameterNumeric.sRegister.u64Default = (uint64_t)Def.u32Register & 0xFFFFFFFF;
                uFileParameterNumeric.sRegister.u64Maximum = (uint64_t)Max.u32Register & 0xFFFFFFFF;
                uFileParameterNumeric.sRegister.u64Minimum = (uint64_t)Min.u32Register & 0xFFFFFFFF;
                break;
            case PAR_TYPE_SINT32:
                sHoldingRegistersAddressTable.uSize.sSize.u8SizeElement = 4;   /* 32-bit */
                uFileParameterNumeric.sStructure.sDataType.eDataType = CSMON_DATA_TYPE_S32;
                uFileParameterNumeric.sRegister.u64Default = (uint64_t)Def.s32Register & 0xFFFFFFFF;
                uFileParameterNumeric.sRegister.u64Maximum = (uint64_t)Max.s32Register & 0xFFFFFFFF;
                uFileParameterNumeric.sRegister.u64Minimum = (uint64_t)Min.s32Register & 0xFFFFFFFF;
                break;
            case PAR_TYPE_WORD32:
                sHoldingRegistersAddressTable.uSize.sSize.u8SizeElement = 4;   /* 32-bit */
                uFileParameterNumeric.sStructure.sDataType.eDataType = CSMON_DATA_TYPE_U32;
                uFileParameterNumeric.sRegister.u64Default = (uint64_t)Def.u32Register & 0xFFFFFFFF;
                uFileParameterNumeric.sRegister.u64Maximum = (uint64_t)Max.u32Register & 0xFFFFFFFF;
                uFileParameterNumeric.sRegister.u64Minimum = (uint64_t)Min.u32Register & 0xFFFFFFFF;
                //uFileParameterNumeric.sStructure.sDataType.eVisualType = CSMON_VISUAL_TYPE_HEX;
                break;
            case PAR_TYPE_A_UINT16:
                sHoldingRegistersAddressTable.uSize.sSize.u8SizeElement = 2;   /* 16-bit Array */
                uFileParameterNumeric.sStructure.sDataType.eDataType = CSMON_DATA_TYPE_U16;
                uFileParameterNumeric.sRegister.u64Default = (uint64_t)Def.u16Register & 0x0000FFFF;
                uFileParameterNumeric.sRegister.u64Maximum = (uint64_t)Max.u16Register & 0x0000FFFF;
                uFileParameterNumeric.sRegister.u64Minimum = (uint64_t)Min.u16Register & 0x0000FFFF;
                //uFileParameterNumeric.sStructure.sDataType.eVisualType = CSMON_VISUAL_TYPE_HEX;
                u32ValueAccessKey.sValueAccessKey.bArray = true;
                sHoldingRegistersAddressTable.uFlags.sFlags.bArray = true;
                sHoldingRegistersAddressTable.uFlags.sFlags.u8ElementCount = u8StartBitOrArrayElementCount;
                break;
            case PAR_TYPE_A_SINT16:
                sHoldingRegistersAddressTable.uSize.sSize.u8SizeElement = 2;   /* 16-bit Array */
                uFileParameterNumeric.sStructure.sDataType.eDataType = CSMON_DATA_TYPE_S16;
                uFileParameterNumeric.sRegister.u64Default = (uint64_t)Def.s16Register & 0x0000FFFF;
                uFileParameterNumeric.sRegister.u64Maximum = (uint64_t)Max.s16Register & 0x0000FFFF;
                uFileParameterNumeric.sRegister.u64Minimum = (uint64_t)Min.s16Register & 0x0000FFFF;
                u32ValueAccessKey.sValueAccessKey.bArray = true;
                sHoldingRegistersAddressTable.uFlags.sFlags.bArray = true;
                sHoldingRegistersAddressTable.uFlags.sFlags.u8ElementCount = u8StartBitOrArrayElementCount;
                break;
            case PAR_TYPE_A_WORD16:
                sHoldingRegistersAddressTable.uSize.sSize.u8SizeElement = 2;   /* 16-bit Array */
                uFileParameterNumeric.sStructure.sDataType.eDataType = CSMON_DATA_TYPE_U16;
                uFileParameterNumeric.sRegister.u64Default = (uint64_t)Def.u16Register & 0x0000FFFF;
                uFileParameterNumeric.sRegister.u64Maximum = (uint64_t)Max.u16Register & 0x0000FFFF;
                uFileParameterNumeric.sRegister.u64Minimum = (uint64_t)Min.u16Register & 0x0000FFFF;
                //uFileParameterNumeric.sStructure.sDataType.eVisualType = CSMON_VISUAL_TYPE_HEX;
                u32ValueAccessKey.sValueAccessKey.bArray = true;
                sHoldingRegistersAddressTable.uFlags.sFlags.bArray = true;
                sHoldingRegistersAddressTable.uFlags.sFlags.u8ElementCount = u8StartBitOrArrayElementCount;
                break;
            case PAR_TYPE_A_UINT32:
                sHoldingRegistersAddressTable.uSize.sSize.u8SizeElement = 4;   /* 32-bit Array */
                uFileParameterNumeric.sStructure.sDataType.eDataType = CSMON_DATA_TYPE_U32;
                uFileParameterNumeric.sRegister.u64Default = (uint64_t)Def.u32Register & 0xFFFFFFFF;
                uFileParameterNumeric.sRegister.u64Maximum = (uint64_t)Max.u32Register & 0xFFFFFFFF;
                uFileParameterNumeric.sRegister.u64Minimum = (uint64_t)Min.u32Register & 0xFFFFFFFF;
                u32ValueAccessKey.sValueAccessKey.bArray = true;
                sHoldingRegistersAddressTable.uFlags.sFlags.bArray = true;
                sHoldingRegistersAddressTable.uFlags.sFlags.u8ElementCount = u8StartBitOrArrayElementCount;
                break;
            case PAR_TYPE_A_SINT32:
                sHoldingRegistersAddressTable.uSize.sSize.u8SizeElement = 4;   /* 32-bit Array */
                uFileParameterNumeric.sStructure.sDataType.eDataType = CSMON_DATA_TYPE_S32;
                uFileParameterNumeric.sRegister.u64Default = (uint64_t)Def.s32Register & 0xFFFFFFFF;
                uFileParameterNumeric.sRegister.u64Maximum = (uint64_t)Max.s32Register & 0xFFFFFFFF;
                uFileParameterNumeric.sRegister.u64Minimum = (uint64_t)Min.s32Register & 0xFFFFFFFF;
                u32ValueAccessKey.sValueAccessKey.bArray = true;
                sHoldingRegistersAddressTable.uFlags.sFlags.bArray = true;
                sHoldingRegistersAddressTable.uFlags.sFlags.u8ElementCount = u8StartBitOrArrayElementCount;
                break;
            case PAR_TYPE_A_WORD32:
                sHoldingRegistersAddressTable.uSize.sSize.u8SizeElement = 4;   /* 32-bit Array */
                uFileParameterNumeric.sStructure.sDataType.eDataType = CSMON_DATA_TYPE_U32;
                uFileParameterNumeric.sRegister.u64Default = (uint64_t)Def.u32Register & 0xFFFFFFFF;
                uFileParameterNumeric.sRegister.u64Maximum = (uint64_t)Max.u32Register & 0xFFFFFFFF;
                uFileParameterNumeric.sRegister.u64Minimum = (uint64_t)Min.u32Register & 0xFFFFFFFF;
                //uFileParameterNumeric.sStructure.sDataType.eVisualType = CSMON_VISUAL_TYPE_HEX;
                u32ValueAccessKey.sValueAccessKey.bArray = true;
                sHoldingRegistersAddressTable.uFlags.sFlags.bArray = true;
                sHoldingRegistersAddressTable.uFlags.sFlags.u8ElementCount = u8StartBitOrArrayElementCount;
                break;
            default:
                sHoldingRegistersAddressTable.uSize.sSize.u8SizeElement = 2;   /* Not Specified / Default */
                uFileParameterNumeric.sStructure.sDataType.eDataType = CSMON_DATA_TYPE_S16;
                uFileParameterNumeric.sRegister.u64Default = (uint64_t)Def.s16Register & 0x0000FFFF;
                uFileParameterNumeric.sRegister.u64Maximum = (uint64_t)Max.s16Register & 0x0000FFFF;
                uFileParameterNumeric.sRegister.u64Minimum = (uint64_t)Min.s16Register & 0x0000FFFF;
                break;
            }


#if 0
            if (uFileParameterNumeric.sStructure.sDataType.eVisualType != CSMON_VISUAL_TYPE_DEF)
            {
                uFileParameterNumeric.sStructure.sDataType.eVisualType = eVisualAttribute;
            }
#endif
            if (u8BitCountOrArrayElementSize == 0)
            {
                if (sHoldingRegistersAddressTable.uSize.sSize.u8SizeElement == 0)
                {
                    /* Default String Size -> 7 or 8 bits */
                    sFileParameterList.u8BitsCount = 8;
                }
                else
                {
                    sFileParameterList.u8BitsCount = sHoldingRegistersAddressTable.uSize.sSize.u8SizeElement * 8;
                }
            }
            else
            {
                u32ValueAccessKey.sValueAccessKey.bBitField = true;
                sFileParameterList.u8BitsCount = u8BitCountOrArrayElementSize;
                sHoldingRegistersAddressTable.uSize.sSize.u8BitOffet = u8StartBitOrArrayElementCount;
                sHoldingRegistersAddressTable.uFlags.sFlags.bBitField = true;
                sHoldingRegistersAddressTable.uFlags.sFlags.u8ElementCount = u8BitCountOrArrayElementSize;
            }


            if (u16FileParameterFillIndex == CSMON_u16FileParameterFillCount)    /* first time write csmon parameter */
            {

                sFileParameterList.eDefaultReadFunctionCode = HMMODBUS_CMD_INVALID;
                sFileParameterList.eDefaultWriteFunctionCode = HMMODBUS_CMD_INVALID;
            }

            switch (uAttributes.sAttributes.eAccess)
            {
            case PAR_ACCESS_RW:
                sHoldingRegistersAddressTable.uFlags.sFlags.bReadOnly = false;
                sHoldingRegistersAddressTable.uFlags.sFlags.bWriteOnly = false;
                sFileParameterList.eDefaultReadFunctionCode = HMMODBUS_CMD_RD_HOLD_REGS;
                sFileParameterList.eDefaultWriteFunctionCode = HMMODBUS_CMD_WR_HOLD_REG;
                break;
            case PAR_ACCESS_WO:
                sHoldingRegistersAddressTable.uFlags.sFlags.bReadOnly = false;
                sHoldingRegistersAddressTable.uFlags.sFlags.bWriteOnly = true;
                sFileParameterList.eDefaultWriteFunctionCode = HMMODBUS_CMD_WR_HOLD_REG;
                break;
            case PAR_ACCESS_RO:
                sHoldingRegistersAddressTable.uFlags.sFlags.bReadOnly = true;
                sHoldingRegistersAddressTable.uFlags.sFlags.bWriteOnly = false;
                sFileParameterList.eDefaultReadFunctionCode = HMMODBUS_CMD_RD_HOLD_REGS;
                break;
            case PAR_ACCESS_NA:
                sHoldingRegistersAddressTable.uFlags.sFlags.bReadOnly = true;
                sHoldingRegistersAddressTable.uFlags.sFlags.bWriteOnly = true;
                sFileParameterList.eDefaultReadFunctionCode = HMMODBUS_CMD_INVALID;
                sFileParameterList.eDefaultWriteFunctionCode = HMMODBUS_CMD_INVALID;
                break;
            }

            /* array parameter file. or register */
            if (u32ValueAccessKey.sValueAccessKey.bArray)
            {
#if 0
                if ((u32ValueAccessKey.sValueAccessKey.u16ValueAccessKeyStoradeID & 1) > 0)
                {
                    if (sFileParameterList.eDefaultWriteFunctionCode != HMMODBUS_CMD_INVALID)
                    {
                        sFileParameterList.eDefaultWriteFunctionCode = HMMODBUS_CMD_WR_HOLD_REGS;
                    }
                }
                else
#endif
                {
                    if (u16ParameterIndexID >= 10000)
                    {
                        while(1)
                        {

                            SysCtl_serviceWatchdog();   /* limitation Modbus Implementation for now: -> halt here because u16ParameterIndexID of array variable must be 0..9999 */
                        }
                    }

                    if (sFileParameterList.eDefaultReadFunctionCode != HMMODBUS_CMD_INVALID)
                    {
                        sFileParameterList.eDefaultReadFunctionCode = HMMODBUS_CMD_RD_FILE_RECORD;
                    }
                    if (sFileParameterList.eDefaultWriteFunctionCode != HMMODBUS_CMD_INVALID)
                    {
                        sFileParameterList.eDefaultWriteFunctionCode = HMMODBUS_CMD_WR_FILE_RECORD;
                    }
                }
            }
            /* bit Field coil */
            else if (u32ValueAccessKey.sValueAccessKey.bBitField)
            {

                //if ((u32ValueAccessKey.sValueAccessKey.u16ValueAccessKeyStoradeID & 1) > 0)
                {
                    if (sFileParameterList.eDefaultReadFunctionCode != HMMODBUS_CMD_INVALID)
                    {
                        sFileParameterList.eDefaultReadFunctionCode = HMMODBUS_CMD_RD_COILS;
                    }
                    if (sFileParameterList.eDefaultWriteFunctionCode != HMMODBUS_CMD_INVALID)
                    {
                        if (sFileParameterList.u8BitsCount == 1)
                        {
                            sFileParameterList.eDefaultWriteFunctionCode = HMMODBUS_CMD_WR_COIL;
                        }
                        else
                        {
                            sFileParameterList.eDefaultWriteFunctionCode = HMMODBUS_CMD_WR_COILS;
                        }
                    }
                }
            }
            else
            {
                /* multiple reg. Write */
                if ((sHoldingRegistersAddressTable.uSize.sSize.u8SizeElement > 2) && (sFileParameterList.eDefaultWriteFunctionCode != HMMODBUS_CMD_INVALID))
                {
                    sFileParameterList.eDefaultWriteFunctionCode = HMMODBUS_CMD_WR_HOLD_REGS;
                }

                /* mask write upto 16 bit */
#if 0
                if ((sFileParameterList.u8BitsCount != (sHoldingRegistersAddressTable.u8SizeElement*8)) && (sHoldingRegistersAddressTable.u8SizeElement <= 2) && (sFileParameterList.eDefaultWriteFunctionCode != HMMODBUS_CMD_INVALID))
                {
                    sFileParameterList.eDefaultWriteFunctionCode = HMMODBUS_CMD_WR_HOLD_REG_MASK;
                }
#endif
            }










            sFileParameterList.ValueAccessKey.sValueAccessKey.u8ValueAccessKeyStartBitIndex = u32ValueAccessKey.sValueAccessKey.u8ValueAccessKeyStartBitIndex;
            sFileParameterList.ModbusAddress = u16ParameterIndexID;
            sFileParameterList.ValueAccessKey.sValueAccessKey.u16ValueAccessKeyStoradeID = u32ValueAccessKey.sValueAccessKey.u16ValueAccessKeyStoradeID;
            sFileParameterList.ValueAccessKey.sValueAccessKey.u6Reserved = u32ValueAccessKey.sValueAccessKey.u6Reserved;
            sFileParameterList.ValueAccessKey.sValueAccessKey.bBitField = u32ValueAccessKey.sValueAccessKey.bBitField;
            sFileParameterList.ValueAccessKey.sValueAccessKey.bArray = u32ValueAccessKey.sValueAccessKey.bArray;



            uFileParameterNumeric.sStructure.s64MultplyFloat = Norm;
            uFileParameterNumeric.sStructure.ValueAccessKey.u32Register = sFileParameterList.ValueAccessKey.u32Register;





            if(uAttributes.sAttributes.bPassRead == false)
            {
                if(uAttributes.sAttributes.bPassWrite == false)
                {
                    sFileParameterList.eReadAccessLevelMin = CSMON_ACCESS_FREE;
                    sFileParameterList.eWriteAccessLevelMin = CSMON_ACCESS_FREE;
                }
                else
                {
                    sFileParameterList.eReadAccessLevelMin = CSMON_ACCESS_FREE;
                    sFileParameterList.eWriteAccessLevelMin = CSMON_ACCESS_OPERATOR;
                }

            }
            else
            {
                if(uAttributes.sAttributes.bPassWrite == false)
                {
                    sFileParameterList.eReadAccessLevelMin = CSMON_ACCESS_ENGINEER;
                    sFileParameterList.eWriteAccessLevelMin = CSMON_ACCESS_SERVICE;
                }
                else
                {
                    sFileParameterList.eReadAccessLevelMin = CSMON_ACCESS_ENGINEER;
                    sFileParameterList.eWriteAccessLevelMin = CSMON_ACCESS_PROGRAMMER;
                }

            }




            sFileParameterList.u8PaddingByte =
                    PARAM_LIST_CALC_CS(
                            sFileParameterList.ValueAccessKey.sValueAccessKey.u16ValueAccessKeyStoradeID,
                            sFileParameterList.ValueAccessKey.sValueAccessKey.u8ValueAccessKeyStartBitIndex,
                            sFileParameterList.ValueAccessKey.sValueAccessKey.u6Reserved,
                            sFileParameterList.ValueAccessKey.sValueAccessKey.bArray,
                            sFileParameterList.ValueAccessKey.sValueAccessKey.bBitField,
                            sFileParameterList.ModbusAddress,
                            sFileParameterList.eReadAccessLevelMin,
                            sFileParameterList.eWriteAccessLevelMin,
                            sFileParameterList.eDefaultReadFunctionCode,
                            sFileParameterList.eDefaultWriteFunctionCode,
                            sFileParameterList.u8BitsCount);




            /* Write To Memory - with SCC 32 bit Writes are not supported */
            pasFileParameterList[u16FileParameterFillIndex].ValueAccessKey.uRegister.au16Word[0]  = sFileParameterList.ValueAccessKey.uRegister.au16Word[0];
            pasFileParameterList[u16FileParameterFillIndex].ValueAccessKey.uRegister.au16Word[1]  = sFileParameterList.ValueAccessKey.uRegister.au16Word[1];
            pasFileParameterList[u16FileParameterFillIndex].ModbusAddress               = sFileParameterList.ModbusAddress;
            pasFileParameterList[u16FileParameterFillIndex].eReadAccessLevelMin         = sFileParameterList.eReadAccessLevelMin;
            pasFileParameterList[u16FileParameterFillIndex].eWriteAccessLevelMin        = sFileParameterList.eWriteAccessLevelMin;
            pasFileParameterList[u16FileParameterFillIndex].eDefaultReadFunctionCode    = sFileParameterList.eDefaultReadFunctionCode;
            pasFileParameterList[u16FileParameterFillIndex].eDefaultWriteFunctionCode   = sFileParameterList.eDefaultWriteFunctionCode;
            pasFileParameterList[u16FileParameterFillIndex].u8BitsCount                 = sFileParameterList.u8BitsCount;
            pasFileParameterList[u16FileParameterFillIndex].u8PaddingByte               = sFileParameterList.u8PaddingByte;

            pauFileParameterNumeric[u16FileParameterFillIndex].sRegisterWord.uDefault.au16Word[0]     = uFileParameterNumeric.sRegisterWord.uDefault.au16Word[0]     ;
            pauFileParameterNumeric[u16FileParameterFillIndex].sRegisterWord.uDefault.au16Word[1]     = uFileParameterNumeric.sRegisterWord.uDefault.au16Word[1]     ;
            pauFileParameterNumeric[u16FileParameterFillIndex].sRegisterWord.uDefault.au16Word[2]     = uFileParameterNumeric.sRegisterWord.uDefault.au16Word[2]     ;
            pauFileParameterNumeric[u16FileParameterFillIndex].sRegisterWord.uDefault.au16Word[3]     = uFileParameterNumeric.sRegisterWord.uDefault.au16Word[3]     ;
            pauFileParameterNumeric[u16FileParameterFillIndex].sRegisterWord.uMaximum.au16Word[0]     = uFileParameterNumeric.sRegisterWord.uMaximum.au16Word[0]     ;
            pauFileParameterNumeric[u16FileParameterFillIndex].sRegisterWord.uMaximum.au16Word[1]     = uFileParameterNumeric.sRegisterWord.uMaximum.au16Word[1]     ;
            pauFileParameterNumeric[u16FileParameterFillIndex].sRegisterWord.uMaximum.au16Word[2]     = uFileParameterNumeric.sRegisterWord.uMaximum.au16Word[2]     ;
            pauFileParameterNumeric[u16FileParameterFillIndex].sRegisterWord.uMaximum.au16Word[3]     = uFileParameterNumeric.sRegisterWord.uMaximum.au16Word[3]     ;
            pauFileParameterNumeric[u16FileParameterFillIndex].sRegisterWord.uMinimum.au16Word[0]     = uFileParameterNumeric.sRegisterWord.uMinimum.au16Word[0]     ;
            pauFileParameterNumeric[u16FileParameterFillIndex].sRegisterWord.uMinimum.au16Word[1]     = uFileParameterNumeric.sRegisterWord.uMinimum.au16Word[1]     ;
            pauFileParameterNumeric[u16FileParameterFillIndex].sRegisterWord.uMinimum.au16Word[2]     = uFileParameterNumeric.sRegisterWord.uMinimum.au16Word[2]     ;
            pauFileParameterNumeric[u16FileParameterFillIndex].sRegisterWord.uMinimum.au16Word[3]     = uFileParameterNumeric.sRegisterWord.uMinimum.au16Word[3]     ;
            pauFileParameterNumeric[u16FileParameterFillIndex].sRegisterWord.sMultply.au16Word[0]     = uFileParameterNumeric.sRegisterWord.sMultply.au16Word[0]     ;
            pauFileParameterNumeric[u16FileParameterFillIndex].sRegisterWord.sMultply.au16Word[1]     = uFileParameterNumeric.sRegisterWord.sMultply.au16Word[1]     ;
            pauFileParameterNumeric[u16FileParameterFillIndex].sRegisterWord.sMultply.au16Word[2]     = uFileParameterNumeric.sRegisterWord.sMultply.au16Word[2]     ;
            pauFileParameterNumeric[u16FileParameterFillIndex].sRegisterWord.sMultply.au16Word[3]     = uFileParameterNumeric.sRegisterWord.sMultply.au16Word[3]     ;
            pauFileParameterNumeric[u16FileParameterFillIndex].sRegisterWord.sOffset.au16Word[0]      = uFileParameterNumeric.sRegisterWord.sOffset.au16Word[0]      ;
            pauFileParameterNumeric[u16FileParameterFillIndex].sRegisterWord.sOffset.au16Word[1]      = uFileParameterNumeric.sRegisterWord.sOffset.au16Word[1]      ;
            pauFileParameterNumeric[u16FileParameterFillIndex].sRegisterWord.sOffset.au16Word[2]      = uFileParameterNumeric.sRegisterWord.sOffset.au16Word[2]      ;
            pauFileParameterNumeric[u16FileParameterFillIndex].sRegisterWord.sOffset.au16Word[3]      = uFileParameterNumeric.sRegisterWord.sOffset.au16Word[3]      ;
            pauFileParameterNumeric[u16FileParameterFillIndex].sRegisterWord.uVirtAddress.au16Word[0] = uFileParameterNumeric.sRegisterWord.uVirtAddress.au16Word[0] ;
            pauFileParameterNumeric[u16FileParameterFillIndex].sRegisterWord.uVirtAddress.au16Word[1] = uFileParameterNumeric.sRegisterWord.uVirtAddress.au16Word[1] ;
            pauFileParameterNumeric[u16FileParameterFillIndex].sRegisterWord.uDataType.au16Word[0]    = uFileParameterNumeric.sRegisterWord.uDataType.au16Word[0]    ;
            pauFileParameterNumeric[u16FileParameterFillIndex].sRegisterWord.uDataType.au16Word[1]    = uFileParameterNumeric.sRegisterWord.uDataType.au16Word[1]    ;

            pasHoldingRegistersAddressTable[u16HoldRegFillCount].u16VirtAddress         = sHoldingRegistersAddressTable.u16VirtAddress; //DPN where the actuall HMMODBUS_sRegisterAddressTable_t is created
            pasHoldingRegistersAddressTable[u16HoldRegFillCount].uSize.u16Register      = sHoldingRegistersAddressTable.uSize.u16Register;
            pasHoldingRegistersAddressTable[u16HoldRegFillCount].uFlags.u16Register     = sHoldingRegistersAddressTable.uFlags.u16Register;





            bNameFullWrite = false;
            bUnitFullWrite = false;
            u16IndexWR = 0;
            u16IndexRD = 0;


            #if BOARDCFG_SHORT_NAME_CODED_IN_HIGH_BYTE_TI

            if ((pu8Name == NULL) || (pu8Name[0] == 0))
            {
                bNameFullWrite = true;
            }
            else
            {

                while ((u16IndexWR < CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH) && (u16IndexRD < CSMON_SET_PARAMETER_NAME_LENGTH_MAX) && (pu8Name[u16IndexRD] != 0))
                {
                    if ((u16IndexWR & 0x0001) == 0)
                    {
                        pasFileParameterShortName[u16FileParameterFillIndex].as8ShortName[u16IndexWR>>1] = pu8Name[u16IndexRD++];
                        u16IndexWR++;
                    }
                    else
                    {
                        pasFileParameterShortName[u16FileParameterFillIndex].as8ShortName[u16IndexWR>>1] |=  ((uint16_t)pu8Name[u16IndexRD++] << 8);
                        u16IndexWR++;
                    }

                    if (u16IndexRD == CSMON_SET_PARAMETER_NAME_LENGTH_MAX)
                    {
                        bNameFullWrite = true;
                    }
                    if (pu8Name[u16IndexRD] == 0)
                    {
                        bNameFullWrite = true;
                    }
                }
            }
            if (u16IndexWR < CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH)
            {
                if ((u16IndexWR & 0x0001) == 0)
                {
                    pasFileParameterShortName[u16FileParameterFillIndex].as8ShortName[u16IndexWR>>1] = '\t';
                    u16IndexWR++;
                }
                else
                {
                    pasFileParameterShortName[u16FileParameterFillIndex].as8ShortName[u16IndexWR>>1] |=  ((uint16_t)('\t') << 8);
                    u16IndexWR++;
                }

            }
            u16IndexRD = 0;


            if ((pu8Unit == NULL) || (pu8Unit[0] == 0))
            {
                bUnitFullWrite = true;
            }
            else
            {

                while ((u16IndexWR < CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH) && (u16IndexRD < CSMON_SET_PARAMETER_UNIT_LENGTH_MAX) && (pu8Unit[u16IndexRD] != 0))
                {
                    if ((u16IndexWR & 0x0001) == 0)
                    {
                        pasFileParameterShortName[u16FileParameterFillIndex].as8ShortName[u16IndexWR>>1] = pu8Unit[u16IndexRD++];
                        u16IndexWR++;
                    }
                    else
                    {
                        pasFileParameterShortName[u16FileParameterFillIndex].as8ShortName[u16IndexWR>>1] |=  ((uint16_t)pu8Unit[u16IndexRD++] << 8);
                        u16IndexWR++;
                    }
                    if (u16IndexRD == CSMON_SET_PARAMETER_UNIT_LENGTH_MAX)
                    {
                        bUnitFullWrite = true;
                    }
                    if (pu8Unit[u16IndexRD] == 0)
                    {
                        bUnitFullWrite = true;
                    }
                }
            }
            while (u16IndexWR < CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH)
            //if (u16IndexWR < CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH)
            {
                if ((u16IndexWR & 0x0001) == 0)
                {
                    pasFileParameterShortName[u16FileParameterFillIndex].as8ShortName[u16IndexWR>>1] = '\0';
                    u16IndexWR++;
                }
                else
                {
                    pasFileParameterShortName[u16FileParameterFillIndex].as8ShortName[u16IndexWR>>1] |=  ((uint16_t)('\0') << 8);
                    u16IndexWR++;
                }
            }

            #else

            if ((pu8Name == NULL) || (pu8Name[0] == 0))
            {
                bNameFullWrite = true;
            }
            else
            {

                while ((u16IndexWR < CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH) && (u16IndexRD < CSMON_SET_PARAMETER_NAME_LENGTH_MAX) && (pu8Name[u16IndexRD] != 0))
                {
                    pasFileParameterShortName[u16FileParameterFillIndex].as8ShortName[u16IndexWR++] = pu8Name[u16IndexRD++];
                    if (u16IndexRD == CSMON_SET_PARAMETER_NAME_LENGTH_MAX)
                    {
                        bNameFullWrite = true;
                    }
                    if (pu8Name[u16IndexRD] == 0)
                    {
                        bNameFullWrite = true;
                    }
                }
            }
            if (u16IndexWR < CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH)
            {
                pasFileParameterShortName[u16FileParameterFillIndex].as8ShortName[u16IndexWR++] = '\t';
            }
            u16IndexRD = 0;


            if ((pu8Unit == NULL) || (pu8Unit[0] == 0))
            {
                bUnitFullWrite = true;
            }
            else
            {

                while ((u16IndexWR < CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH) && (u16IndexRD < CSMON_SET_PARAMETER_UNIT_LENGTH_MAX) && (pu8Unit[u16IndexRD] != 0))
                {
                    pasFileParameterShortName[u16FileParameterFillIndex].as8ShortName[u16IndexWR++] = pu8Unit[u16IndexRD++];
                    if (u16IndexRD == CSMON_SET_PARAMETER_UNIT_LENGTH_MAX)
                    {
                        bUnitFullWrite = true;
                    }
                    if (pu8Unit[u16IndexRD] == 0)
                    {
                        bUnitFullWrite = true;
                    }
                }
            }
            while (u16IndexWR < CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH)
            //if (u16IndexWR < CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH)
            {
                pasFileParameterShortName[u16FileParameterFillIndex].as8ShortName[u16IndexWR++] = '\0';
            }

            #endif


            if ((bNameFullWrite == false) || (bUnitFullWrite == false))
            {
                eResponseCode = CSMON_RESPONSE_CODE_NOT_FULL_COPY_STRING;
            }


            /* MODBUSREG module fix */
            MODBUSREG_vSetHoldRegFillCount(u16HoldRegFillCount + 1);
            MODBUSREG_vInitHoldingRegistersTableChanges();

            if (CSMON_psFileParameterAux != NULL)
            {
                CSMON_psFileParameterAux->u16ModbusCountUse = (u16HoldRegFillCount + 1);   /* set the new count to use (Modbus table count differs from Parameter Table count) */
                CSMON_psFileParameterAux->u16ModbusCountUseCheck = (u16HoldRegFillCount + 1) ^ 0xFFFF;
            }


            /* CSMON module fix */
            u16FileParameterFillIndex++;
            if (u16FileParameterFillIndex > CSMON_u16FileParameterFillCount)
            {
                CSMON_u16FileParameterFillCount = u16FileParameterFillIndex;
                if (CSMON_psFileParameterAux != NULL)
                {
                    CSMON_psFileParameterAux->u16ParamCountUse = CSMON_u16FileParameterFillCount;   /* set the new count to use */
                    CSMON_psFileParameterAux->u16ParamCountUseCheck = CSMON_u16FileParameterFillCount ^ 0xFFFF;   /* set the new count to use */
                }
            }
        }
        else
        {
            if (u16FileParameterFillIndex >= CSMON_u16FileParameterUseMaxCnt)
            {
                eResponseCode = CSMON_RESPONSE_FAIL_CSMON_PARAM_ARRAY_FULL;
            }
            else /* if (u16HoldRegFillCount >= u16HoldRegUsedCount) */
            {
                eResponseCode = CSMON_RESPONSE_FAIL_MODBUS_HOLDREG_ADD_ARRAY_FULL;
            }
        }
    }

    return eResponseCode;
#else
    return CSMON_RESPONSE_FAIL_NOT_IMPLEMENTED;
#endif
}


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
CSMON_eResponseCode_t CSMON_eApplyParameterChanges (void)
{
    CSMON_eResponseCode_t eResponseCode = CSMON_RESPONSE_CODE_OK;
    
    #if CSMON_PARAMETER_LIST_READ_ONLY == 0
    uWord32_t uCheckSum;
    uCheckSum.u32Register = CSMON_u32GetParameterCheckSum();
    CSMON_psFileParameterAux->uParamCheckSum.au16Word[0] = uCheckSum.au16Word[0];   /* 32 Bit Writes Does Not Work on SCC */
    CSMON_psFileParameterAux->uParamCheckSum.au16Word[1] = uCheckSum.au16Word[1];

    CSMON_vInitHoldingRegistersTableChanges();
    #endif

    return eResponseCode;
}



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
CSMON_eResponseCode_t CSMON_eResetParameterTable (void)
{
    CSMON_eResponseCode_t eResponseCode = CSMON_RESPONSE_CODE_OK;

    #if CSMON_PARAMETER_LIST_READ_ONLY == 0
    CSMON_u16FileParameterFillCount = 0;
    MODBUSREG_vSetHoldRegFillCount(0);
    MODBUSREG_vInitHoldingRegistersTableChanges();
    MODBUSREG_vSetParamCount(0);
    #endif

    #if BOARDCFG_USE_CSMON_STRING_TABLE
    if (CSMON_pu32FileParameterString != NULL)
    {
        ((uint16_t*)CSMON_pu32FileParameterString)[0] = 0;        /* Clear Length Of String Table */
        ((uint16_t*)CSMON_pu32FileParameterString)[1] = 0;        /* Clear Length Of String Table */
    }
    #endif /* BOARDCFG_USE_CSMON_STRING_TABLE */
    return eResponseCode;

}





/* *****************************************************************************
 * CSMON_u32GetParameterCheckSum
 *
 * Description:
 *  Calculates and returns CheckSum of the Parameter Table
 *
 **************************************************************************** */
uint32_t CSMON_u32GetParameterCheckSum (void)
{
    uWord32_t uCheckSum;
    uint16_t u16FileParameterIndex;

    MEMORY_MODIFIER CSMON_sFileParameterDefinitions_t* pasFileParameterList;


    uCheckSum.u32Register = 0xAABBCCDD;

    pasFileParameterList = CSMON_pasFileParameterList;

    if  (pasFileParameterList != NULL)
    {
        for (u16FileParameterIndex = 0; u16FileParameterIndex < CSMON_u16FileParameterFillCount; u16FileParameterIndex++)
        {
            uCheckSum.au16Word[1] += pasFileParameterList[u16FileParameterIndex].u8BitsCount;
            uCheckSum.u32Register += pasFileParameterList[u16FileParameterIndex].u8PaddingByte;
        }
    }
    uCheckSum.u32Register = (uint32_t)0 - uCheckSum.u32Register;

    return uCheckSum.u32Register;
}

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
        float f32SampleRateHz)
{
    CSMON_eResponseCode_t eResponseCode = CSMON_RESPONSE_CODE_OK;

    if (u16RecorderIndex < HMDRV_RECORDER_PARAM_HANDLE_COUNT)
    {
        /* Fill Recorder Configuration */
        sRecConfigParam[u16RecorderIndex].sSampleFormat.nSampleCountMax = u32TotalSampleCount;
        sRecConfigParam[u16RecorderIndex].nTotalCountSamples = u32TotalSampleCount;

        sRecConfigParam[u16RecorderIndex].sSampleFormat.nPreTriggerSampleCount = u32PreTriggerSampleCount;
        sRecConfigParam[u16RecorderIndex].sTrigerConfig.nPreTriggerSamples = u32PreTriggerSampleCount;

        sRecConfigParam[u16RecorderIndex].sSampleFormat.nSampleRateHz = f32SampleRateHz;
        sRecConfigParam[u16RecorderIndex].sSampleRate.fSampleFrequency = f32SampleRateHz;
        sRecConfigParam[u16RecorderIndex].sSampleRate.sSampleRateLookUp.nTimeDividerOrSamplesSkip = 0;
        sRecConfigParam[u16RecorderIndex].sSampleRate.sSampleRateLookUp.eTimeBase = HMRECPRM_TIME_BASE_0;
    }
    else
    {
        eResponseCode = CSMON_RESPONSE_FAIL_CSMON_RECORDER_SCOPE_INDEX_OUT_OF_RANGE;
    }

    return eResponseCode;
}

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
        uint16_t u16TriggerMode)
{
    CSMON_eResponseCode_t eResponseCode = CSMON_RESPONSE_CODE_OK;

     int16_t s16FileParameterFillIndex;
    uint16_t u16FileParameterFillIndex;
    CSMON_uFileParameterValueAccessKey_t u32ValueAccessKey;


    if (u16RecorderIndex < HMDRV_RECORDER_PARAM_HANDLE_COUNT)
    {
        if (u16TriggerMode == (uint16_t)CSMON_TRIGGER_MODE_NONE)
        {
            /* Clear Recorder Trigger Configuration */
            sRecConfigParam[u16RecorderIndex].sTrigerConfig.eTriggerMode = (HMRECPRM_eTriggerMode_t)u16TriggerMode;
        }
        else
        {
            u32ValueAccessKey.sValueAccessKey.u16ValueAccessKeyStoradeID = u16ParameterID;
            u32ValueAccessKey.sValueAccessKey.bArray = false;
            u32ValueAccessKey.sValueAccessKey.u8ValueAccessKeyStartBitIndex = 0;
            u32ValueAccessKey.sValueAccessKey.bBitField = 0;
            u32ValueAccessKey.sValueAccessKey.u6Reserved = 0;

            /* try find Table Index From CSMON ID */
            s16FileParameterFillIndex = CSMON_u16FindIndexParameterTable(u32ValueAccessKey.u32Register);
            if (s16FileParameterFillIndex < 0)
            {
                eResponseCode = CSMON_RESPONSE_FAIL_CSMON_PARAM_ARRAY_INDEX_OUT_OF_RANGE;   /* Parameter ID Not Found */
            }
            else
            {
                u16FileParameterFillIndex = s16FileParameterFillIndex;
                /* Fill Recorder Trigger Configuration */
                sRecConfigParam[u16RecorderIndex].sTrigerConfig.nVariableIDTrigger = CSMON_pasFileParameterList[u16FileParameterFillIndex].ValueAccessKey.u32Register;
                sRecConfigParam[u16RecorderIndex].sTrigerConfig.sTriggerThreshold.eValue.u32 = u32TresholdValue;
                sRecConfigParam[u16RecorderIndex].sTrigerConfig.eTriggerMode = (HMRECPRM_eTriggerMode_t)u16TriggerMode;

                sRecConfigParam[u16RecorderIndex].sTrigerConfig.sTriggerThreshold.eType =
                        (HMRECPRM_eVariantType_t)CSMON_pauFileParameterNumeric[u16FileParameterFillIndex].sStructure.sDataType.eDataType;
                sRecConfigParam[u16RecorderIndex].sTrigerConfig.sTriggerThreshold.eVisual =
                        (HMRECPRM_eVisualType_t)CSMON_pauFileParameterNumeric[u16FileParameterFillIndex].sStructure.sDataType.eVisualType;
            }
        }
    }
    else
    {
        eResponseCode = CSMON_RESPONSE_FAIL_CSMON_RECORDER_SCOPE_INDEX_OUT_OF_RANGE;
    }

    return eResponseCode;
}



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
        uint16_t u16ParameterPos)
{
    CSMON_eResponseCode_t eResponseCode = CSMON_RESPONSE_CODE_OK;

     int16_t s16FileParameterFillIndex;
    uint16_t u16FileParameterFillIndex;
    CSMON_uFileParameterValueAccessKey_t u32ValueAccessKey;

    if (u16RecorderIndex < HMDRV_RECORDER_PARAM_HANDLE_COUNT)
    {
        if (u16ParameterPos < HMDRV_MAX_REC_PARAM_VARS)
        {
            u32ValueAccessKey.sValueAccessKey.u16ValueAccessKeyStoradeID = u16ParameterID;
            u32ValueAccessKey.sValueAccessKey.bArray = false;
            u32ValueAccessKey.sValueAccessKey.u8ValueAccessKeyStartBitIndex = 0;
            u32ValueAccessKey.sValueAccessKey.bBitField = 0;
            u32ValueAccessKey.sValueAccessKey.u6Reserved = 0;

            /* try find Table Index From CSMON ID */
            s16FileParameterFillIndex = CSMON_u16FindIndexParameterTable(u32ValueAccessKey.u32Register);
            if (s16FileParameterFillIndex < 0)
            {
                eResponseCode = CSMON_RESPONSE_FAIL_CSMON_PARAM_ARRAY_INDEX_OUT_OF_RANGE;   /* Parameter ID Not Found */
            }
            else
            {
                u16FileParameterFillIndex = s16FileParameterFillIndex;
                /* Fill Recorder Configuration */
                sRecConfigParam[u16RecorderIndex].sSampleFormat.nVariableIDRecord[u16ParameterPos] = CSMON_pasFileParameterList[u16FileParameterFillIndex].ValueAccessKey.u32Register;
            }
        }
        else
        {
            eResponseCode = CSMON_RESPONSE_FAIL_CSMON_PARAMETER_INDEX_IN_RECORDER_SCOPE_OUT_OF_RANGE;
        }
    }
    else
    {
        eResponseCode = CSMON_RESPONSE_FAIL_CSMON_RECORDER_SCOPE_INDEX_OUT_OF_RANGE;
    }

    return eResponseCode;
}

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
        uint16_t u16ParameterCount)
{
    CSMON_eResponseCode_t eResponseCode = CSMON_RESPONSE_CODE_OK;

    if (u16RecorderIndex < HMDRV_RECORDER_PARAM_HANDLE_COUNT)
    {
        if (u16ParameterCount < HMDRV_MAX_REC_PARAM_VARS)
        {
            /* Fill Recorder Configuration */
            sRecConfigParam[u16RecorderIndex].sSampleFormat.nCountVariables = u16ParameterCount;
        }
        else
        {
            eResponseCode = CSMON_RESPONSE_FAIL_CSMON_PARAMETER_COUNT_IN_RECORDER_SCOPE_OUT_OF_RANGE;
        }
    }
    else
    {
        eResponseCode = CSMON_RESPONSE_FAIL_CSMON_RECORDER_SCOPE_INDEX_OUT_OF_RANGE;
    }

    return eResponseCode;
}





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
        uint16_t u16ParameterPos)
{
    CSMON_eResponseCode_t eResponseCode = CSMON_RESPONSE_CODE_OK;

     int16_t s16FileParameterFillIndex;
    uint16_t u16FileParameterFillIndex;
    CSMON_uFileParameterValueAccessKey_t u32ValueAccessKey;

    if (u16ScopeIndex < HMDRV_SCOPE_PARAM_HANDLE_COUNT)
    {
        if (u16ParameterPos < HMDRV_MAX_SCOPE_PARAM_VARS)
        {
            u32ValueAccessKey.sValueAccessKey.u16ValueAccessKeyStoradeID = u16ParameterID;
            u32ValueAccessKey.sValueAccessKey.bArray = false;
            u32ValueAccessKey.sValueAccessKey.u8ValueAccessKeyStartBitIndex = 0;
            u32ValueAccessKey.sValueAccessKey.bBitField = 0;
            u32ValueAccessKey.sValueAccessKey.u6Reserved = 0;

            /* try find Table Index From CSMON ID */
            s16FileParameterFillIndex = CSMON_u16FindIndexParameterTable(u32ValueAccessKey.u32Register);
            if (s16FileParameterFillIndex < 0)
            {
                eResponseCode = CSMON_RESPONSE_FAIL_CSMON_PARAM_ARRAY_INDEX_OUT_OF_RANGE;   /* Parameter ID Not Found */
            }
            else
            {
                u16FileParameterFillIndex = s16FileParameterFillIndex;
                /* Fill Recorder Configuration */
                sScopeConfigParam[u16ScopeIndex].nVariableIDScope[u16ParameterPos] = CSMON_pasFileParameterList[u16FileParameterFillIndex].ValueAccessKey.u32Register;
            }
        }
        else
        {
            eResponseCode = CSMON_RESPONSE_FAIL_CSMON_PARAMETER_INDEX_IN_RECORDER_SCOPE_OUT_OF_RANGE;
        }
    }
    else
    {
        eResponseCode = CSMON_RESPONSE_FAIL_CSMON_RECORDER_SCOPE_INDEX_OUT_OF_RANGE;
    }

    return eResponseCode;
}

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
        uint16_t u16ParameterCount)
{
    CSMON_eResponseCode_t eResponseCode = CSMON_RESPONSE_CODE_OK;

    if (u16ScopeIndex < HMDRV_SCOPE_PARAM_HANDLE_COUNT)
    {
        if (u16ParameterCount < HMDRV_MAX_SCOPE_PARAM_VARS)
        {
            /* Fill Recorder Configuration */
            sScopeConfigParam[u16ScopeIndex].nCountVariables = u16ParameterCount;
        }
        else
        {
            eResponseCode = CSMON_RESPONSE_FAIL_CSMON_PARAMETER_COUNT_IN_RECORDER_SCOPE_OUT_OF_RANGE;
        }
    }
    else
    {
        eResponseCode = CSMON_RESPONSE_FAIL_CSMON_RECORDER_SCOPE_INDEX_OUT_OF_RANGE;
    }

    return eResponseCode;
}




/* *****************************************************************************
 * CSMON_vSetSetupRecorderParameterMask
 **************************************************************************** */
void CSMON_vSetSetupRecorderParameterMask (CSMON_eRecorderParameterMask_t eMask)
{
    CSMON_u16MaskSetupRecorderParam = eMask;
}

/* *****************************************************************************
 * CSMON_vSetStartRecorderParameterMask
 **************************************************************************** */
void CSMON_vSetStartRecorderParameterMask (CSMON_eRecorderParameterMask_t eMask)
{
    CSMON_u16MaskStartRecorderParam = eMask;
}

/* *****************************************************************************
 * CSMON_vSetStopRecorderParameterMask
 **************************************************************************** */
void CSMON_vSetStopRecorderParameterMask (CSMON_eRecorderParameterMask_t eMask)
{
    CSMON_u16MaskStopRecorderParam = eMask;
}



/* *****************************************************************************
 * CSMON_u32GetModbusMessageCounterRegisterRealAddress
 *
 * Description:
 *
 *
 **************************************************************************** */
uint32_t CSMON_u32GetModbusMessageCounterRegisterRealAddress(void)
{
    return MODBUSREG_u32GetAddressMessageCounterThisDevice();
}





/* *****************************************************************************
 * CSMON_vRecorderParamSetupProcess
 *
 * Description: Automation of Setup of the Parameter Recorders
 **************************************************************************** */
void CSMON_vRecorderParamSetupProcess(void)
{
#if (HMDRV_USE_RECORDER_PARAMETERS) && (!(HMDRV_DISABLE))
#if HMDRV_RECORDER_PARAM_HANDLE_COUNT > 0
    if (CSMON_u16MaskSetupRecorderParam > 0)
    {
        HMLIB_vInitRecParamBuff(false);
        if ((CSMON_u16MaskSetupRecorderParam & (1 << 0)) > 0)
            eResponseCodeStartRecParam0 = CSMON_eSetUpRecParam(0);
        if ((CSMON_u16MaskSetupRecorderParam & (1 << 1)) > 0)
            eResponseCodeStartRecParam1 = CSMON_eSetUpRecParam(1);
        if ((CSMON_u16MaskSetupRecorderParam & (1 << 2)) > 0)
            eResponseCodeStartRecParam2 = CSMON_eSetUpRecParam(2);

        CSMON_u16MaskSetupRecorderParam = 0;
    }
#endif
#endif
}


/* *****************************************************************************
 * CSMON_vRecorderParamStartStopProcess
 *
 * Description: Automation of Setup, Start and Stop of the Parameter Recorders
 **************************************************************************** */
void CSMON_vRecorderParamStartStopProcess(void)
{
#if (HMDRV_USE_RECORDER_PARAMETERS) && (!(HMDRV_DISABLE))
#if HMDRV_RECORDER_PARAM_HANDLE_COUNT > 0
    if (CSMON_u16MaskStartRecorderParam > 0)
    {
        if ((CSMON_u16MaskStartRecorderParam & (1 << 0)) > 0)
            eResponseCodeStartRecParam0 = HMRECPRM_StartRecParam(0);
        if ((CSMON_u16MaskStartRecorderParam & (1 << 1)) > 0)
            eResponseCodeStartRecParam1 = HMRECPRM_StartRecParam(1);
        if ((CSMON_u16MaskStartRecorderParam & (1 << 2)) > 0)
            eResponseCodeStartRecParam2 = HMRECPRM_StartRecParam(2);

        CSMON_u16MaskStartRecorderParam = 0;
    }

    if (CSMON_u16MaskStopRecorderParam > 0)
    {
        if ((CSMON_u16MaskStopRecorderParam & (1 << 0)) > 0)
            HMRECPRM_StopRecParam(0);
        if ((CSMON_u16MaskStopRecorderParam & (1 << 1)) > 0)
            HMRECPRM_StopRecParam(1);
        if ((CSMON_u16MaskStopRecorderParam & (1 << 2)) > 0)
            HMRECPRM_StopRecParam(2);

        CSMON_u16MaskStopRecorderParam = 0;
    }
#endif
#endif
}






/* *****************************************************************************
 * CSMON_eSetFirmwareApplicationVersion
 *
 * Input:
 *      uint16_t u16Version       - Firmware Application Version
 *                                  High Byte Is Major Version
 *                                  Low Byte Is Minor Version
 *
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eSetFirmwareApplicationVersion (uint16_t u16Version)
{
    CSMON_eResponseCode_t eResponseCode = CSMON_RESPONSE_CODE_OK;

    CSMON_sFileProtocolVersion.u16FirmwareApplicationVersion = u16Version;

    return eResponseCode;
}


/* *****************************************************************************
 * CSMON_eSetAutoServiceWatchdogInternalSlowCalculationsInMainLoop
 *
 * Input:
 *      bool bAutoServiceWatchDog
 *
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eSetAutoServiceWatchdogInternalSlowCalculationsInMainLoop (bool bAutoServiceWatchDog)
{
    CSMON_eResponseCode_t eResponseCode = CSMON_RESPONSE_CODE_OK;

    bServiceWatchdog = bAutoServiceWatchDog;

    return eResponseCode;
}


/* *****************************************************************************
 * CSMON_eSetInternalTimerPeriod
 *
 * Input:
 *      uint16_t microseconds
 *
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eSetInternalTimerPeriod (uint16_t microseconds)
{
    CSMON_eResponseCode_t eResponseCode = CSMON_RESPONSE_CODE_OK;

    TIMER_u16ResetTimerMainPeriod(microseconds);

    return eResponseCode;
}


/* *****************************************************************************
 * CSMON_eSetMaxTimeInISR
 *
 * Input:
 *      uint16_t microseconds
 *
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eSetMaxTimeInISR (uint16_t microseconds)
{
    CSMON_eResponseCode_t eResponseCode = CSMON_RESPONSE_CODE_OK;

    CSMON_u16MaxTimeInISR_usec = microseconds;

    return eResponseCode;
}



/* *****************************************************************************
 * CSMON_eSetMinGuaranteedTimeBetweenTwoISRs
 *
 * Input:
 *      uint16_t microseconds
 *
 **************************************************************************** */
CSMON_eResponseCode_t CSMON_eSetMinGuaranteedTimeBetweenTwoISRs (uint16_t microseconds)
{
    CSMON_eResponseCode_t eResponseCode = CSMON_RESPONSE_CODE_OK;

    CSMON_u16MinTimeProcessExit_usec = microseconds;

    return eResponseCode;
}


/* *****************************************************************************
 * HMRECPRM_vGetDateTime - Get RTC Value - called from HMLIB library
 **************************************************************************** */
void HMRECPRM_vGetDateTime (
        uint_least8_t* pu8BCDSeconds,
        uint_least8_t* pu8BCDMinutes,
        uint_least8_t* pu8BCDHours,
        uint_least8_t* pu8BCDWeekdays,
        uint_least8_t* pu8BCDDay,
        uint_least8_t* pu8BCDMonth,
        uint_least8_t* pu8BCDYear)
{
        *pu8BCDSeconds  = CSMON_sDateTime.u8Seconds;
        *pu8BCDMinutes  = CSMON_sDateTime.u8Minutes;
        *pu8BCDHours    = CSMON_sDateTime.u8Hours;
        *pu8BCDWeekdays = CSMON_sDateTime.u8WeekDays;
        *pu8BCDDay      = CSMON_sDateTime.u8Day;
        *pu8BCDMonth    = CSMON_sDateTime.u8Month;
        *pu8BCDYear     = CSMON_sDateTime.u8Year;
}









