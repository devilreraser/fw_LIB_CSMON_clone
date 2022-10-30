/* *****************************************************************************
 * File:   main.c
 * Author: Dimitar Lilov
 *
 * Created on 2020 03 13 09:06
 * 
 * Description: Test and Example for CSMON_C28xx library integration.
 * 
 **************************************************************************** */
#ifdef _WIN32
#pragma once
#endif /* _WIN32 */
 
/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdbool.h>
#include <stddef.h>
#include <math.h>

#include "main.h"
#include "device.h"



#if defined(__TMS320F2806x__)
#include "boardcfg.h"
#include "csmon_config.h"
#include "sci_driver.h"
#include "uart_driver.h"

#else

#define BOARDCFG_CSMON_FILE_PARAMETER_COUNT_MAX 1024

#ifndef _CS_1211
#include "emif_driver.h"
#endif

#if defined(_CS_1107_SCC_R01)
#include "fpga_driver.h"
#else
#include "sci_driver.h"
#include "uart_driver.h"
#ifndef _CS_1211
#include "fpga_sci_driver.h"
#endif
#endif

#endif

#include "csmon.h"
#include "parameter.h"
#include "datetime.h"

#ifdef _CS_1107_SCC_R01
#include "peripheral.h"
#endif

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
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
 * Configuration Parameter Definitions
 **************************************************************************** */

#define CSMON_PAR_LIST_MINIMUM_COUNT                                    0
#define CSMON_PAR_LIST_MAXIMUM_COUNT                                    1                   /* Maximum Count */
#define CSMON_PAR_LIST_EACH_TYPE_REPEATED_ALL_TYPES_COUNT_TIMES         2                   /* Each Type Repeated All Types Count Times  */
#define CSMON_PAR_LIST_ALL_TYPES_REPEATED_ALL_TYPES_COUNT_TIMES         3                   /* All Types Repeated All Types Count Times  */
#define CSMON_PAR_LIST_RECORDER_DEBUG                                   4

//#define CSMON_PARAMETER_LIST_TEST   CSMON_PAR_LIST_MINIMUM_COUNT
//#define CSMON_PARAMETER_LIST_TEST   CSMON_PAR_LIST_MAXIMUM_COUNT
//#define CSMON_PARAMETER_LIST_TEST   CSMON_PAR_LIST_EACH_TYPE_REPEATED_ALL_TYPES_COUNT_TIMES
//#define CSMON_PARAMETER_LIST_TEST   CSMON_PAR_LIST_ALL_TYPES_REPEATED_ALL_TYPES_COUNT_TIMES
#define CSMON_PARAMETER_LIST_TEST   CSMON_PAR_LIST_RECORDER_DEBUG


/* Some Pinout */
#ifdef __TMS320F2806x__

#define STAT_LED_EQEP1I_PIN 23
#define STAT_LED_EQEP1S_PIN 22
#define STAT_LED_EQEP1B_PIN 21
#define STAT_LED_EQEP1A_PIN 20

#elif defined(_LAUNCHXL_F28379D)
#define STAT_LED_G_PIN      28      /* N/A */
#define STAT_LED_A_B_PIN    31      /* D10 Blue */
#define STAT_LED_R_PIN      34      /* D9 */
#else
//1038
#define STAT_LED_G_PIN      28      /* Green LED (closest to the MCU Led) */
#define STAT_LED_A_B_PIN    30      /* Amber LED (middle Led) */
#define STAT_LED_R_PIN      32      /* Red LED (closest to the Debug Header) */
#define CLK_EN_FPGA_PIN     33
#endif

#define STAT_LED_ENABLE_LEVEL_LOW 0
#define STAT_LED_DISABLE_LVL_HIGH   (!STAT_LED_ENABLE_LEVEL_LOW)


/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

#if CSMON_CONFIG == 0


#define INIT_MAX_MIN_DEF(_type, max, min, def_) \
        .u32Max._type = (max), .u32Min._type = (min), .u32Def._type = (def_)


#if _CSMON_USE_EXTERNAL_PARAMETER_LIST == 0

#define INIT_PARAMFULL(u16ID, paramType, paramAccess, paramPass, funcWr, funcRd, accessWr, accessRd, bStorable, u8Bits, u8Offs, eType,  DataTypeAttribute, u32Address, strName, strUnit, u32Maximum, u32Minimum, u32Default, floatScale, VisualAttribute, u8Elements, bitField, arrayType, bNR, bNW, pFunc) \
 { \
    .u16ParameterIndexID = u16ID,                          /* u16ID */\
    .u16ParamAttributes = (PAR(paramType, paramAccess, paramPass)),                /* u16Attributes */\
    .u32RealAddress = (uint32_t)(u32Address),         /* u32Address */\
    .au8Name = {strName},                      /* au8Name */\
    .au8Unit = {strUnit},                      /* au8Unit */\
    .u32Max.eType = (u32Maximum),   /* u32Maximum */\
    .u32Min.eType = (u32Minimum),   /* u32Minimum */\
    .u32Def.eType = (u32Default),   /* u32Default */\
    .Norm = floatScale,                     /* floatScale */\
    .u8BitCountOrArrayElementSize = 0,                              /* u8BitCountOrArrayElementSize - can be u8Bits (here used default 0 and generated from type(attribute)) */\
    .u8StartBitOrArrayElementCount = 0,                              /* u8StartBitOrArrayElementCount - can be u8Offs (here used default 0) */\
    .eVisualTypeAttribute = VisualAttribute,               /* eVisualTypeAttribute */\
    .uParameterSize.sSize.u8SizeElement = u8Bits / 8, \
    .uParameterSize.sSize.u8CountElements = u8Elements,\
    .uParameterFlags.sFlags.u8BitOffset = u8Offs, \
    .uParameterFlags.sFlags.bBitField = bitField, \
    .uParameterFlags.sFlags.bArray = arrayType, /* Array Type Register */ \
    .uParameterFlags.sFlags.bReadOnly = 1^(paramAccess >> 1), \
    .uParameterFlags.sFlags.bWriteOnly = 1^(paramAccess & 1), \
    .uParameterFlags.sFlags.bReadDenySkipCSMON = bNR, \
    .uParameterFlags.sFlags.bWriteDenySkipCSMON = bNW, \
    .u32ProcessFunc = pFunc \
 }

#define INIT_PARAMETER(u16ID, paramType, paramAccess, paramPass, funcWr, funcRd, accessWr, accessRd, bStorable, u8Bits, u8Offs, eType,  DataTypeAttribute, u32Address, strName, strUnit, u32Maximum, u32Minimum, u32Default, floatScale) \
        INIT_PARAMFULL(u16ID, paramType, paramAccess, paramPass, funcWr, funcRd, accessWr, accessRd, bStorable, u8Bits, u8Offs, eType,  DataTypeAttribute, u32Address, strName, strUnit, u32Maximum, u32Minimum, u32Default, floatScale, CSMON_VISUAL_TYPE_HEX, 1, 0, 0, 0, 0, NULL)
#else



#define STRLENS(a,i)        !a[i] ? i : // repetitive stuff
#define STRLENPADDED(a)     (STRLENS(a,0) STRLENS(a,1) STRLENS(a,2) STRLENS(a,3) STRLENS(a,4) STRLENS(a,5) STRLENS(a,6) STRLENS(a,7) STRLENS(a,8) STRLENS(a,9) -1)
#define STRLEN(a)           STRLENPADDED((a "\0\0\0\0\0\0\0\0\0")) // padding required to prevent 'index out of range' issues.



#define STR_LENGTH(x) sizeof(x)

#if CSMON_SET_PARAMETER_NAMEUNIT_LENGTH_MAX == 64
#define STR_PADDED(x) (x "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0")
#define ZIP_STRING(x) { \
    ((uint16_t)STR_PADDED(x)[ 0] & 0xFF) | ((uint16_t)STR_PADDED(x)[ 1] << 8), \
    ((uint16_t)STR_PADDED(x)[ 2] & 0xFF) | ((uint16_t)STR_PADDED(x)[ 3] << 8), \
    ((uint16_t)STR_PADDED(x)[ 4] & 0xFF) | ((uint16_t)STR_PADDED(x)[ 5] << 8), \
    ((uint16_t)STR_PADDED(x)[ 6] & 0xFF) | ((uint16_t)STR_PADDED(x)[ 7] << 8), \
    ((uint16_t)STR_PADDED(x)[ 8] & 0xFF) | ((uint16_t)STR_PADDED(x)[ 9] << 8), \
    ((uint16_t)STR_PADDED(x)[10] & 0xFF) | ((uint16_t)STR_PADDED(x)[11] << 8), \
    ((uint16_t)STR_PADDED(x)[12] & 0xFF) | ((uint16_t)STR_PADDED(x)[13] << 8), \
    ((uint16_t)STR_PADDED(x)[14] & 0xFF) | ((uint16_t)STR_PADDED(x)[15] << 8), \
    ((uint16_t)STR_PADDED(x)[16] & 0xFF) | ((uint16_t)STR_PADDED(x)[17] << 8), \
    ((uint16_t)STR_PADDED(x)[18] & 0xFF) | ((uint16_t)STR_PADDED(x)[19] << 8), \
    ((uint16_t)STR_PADDED(x)[20] & 0xFF) | ((uint16_t)STR_PADDED(x)[21] << 8), \
    ((uint16_t)STR_PADDED(x)[22] & 0xFF) | ((uint16_t)STR_PADDED(x)[23] << 8), \
    ((uint16_t)STR_PADDED(x)[24] & 0xFF) | ((uint16_t)STR_PADDED(x)[25] << 8), \
    ((uint16_t)STR_PADDED(x)[26] & 0xFF) | ((uint16_t)STR_PADDED(x)[27] << 8), \
    ((uint16_t)STR_PADDED(x)[28] & 0xFF) | ((uint16_t)STR_PADDED(x)[29] << 8), \
    ((uint16_t)STR_PADDED(x)[30] & 0xFF) | ((uint16_t)STR_PADDED(x)[31] << 8), \
    ((uint16_t)STR_PADDED(x)[32] & 0xFF) | ((uint16_t)STR_PADDED(x)[33] << 8), \
    ((uint16_t)STR_PADDED(x)[34] & 0xFF) | ((uint16_t)STR_PADDED(x)[35] << 8), \
    ((uint16_t)STR_PADDED(x)[36] & 0xFF) | ((uint16_t)STR_PADDED(x)[37] << 8), \
    ((uint16_t)STR_PADDED(x)[38] & 0xFF) | ((uint16_t)STR_PADDED(x)[39] << 8), \
    ((uint16_t)STR_PADDED(x)[40] & 0xFF) | ((uint16_t)STR_PADDED(x)[41] << 8), \
    ((uint16_t)STR_PADDED(x)[42] & 0xFF) | ((uint16_t)STR_PADDED(x)[43] << 8), \
    ((uint16_t)STR_PADDED(x)[44] & 0xFF) | ((uint16_t)STR_PADDED(x)[45] << 8), \
    ((uint16_t)STR_PADDED(x)[46] & 0xFF) | ((uint16_t)STR_PADDED(x)[47] << 8), \
    ((uint16_t)STR_PADDED(x)[48] & 0xFF) | ((uint16_t)STR_PADDED(x)[49] << 8), \
    ((uint16_t)STR_PADDED(x)[50] & 0xFF) | ((uint16_t)STR_PADDED(x)[51] << 8), \
    ((uint16_t)STR_PADDED(x)[52] & 0xFF) | ((uint16_t)STR_PADDED(x)[53] << 8), \
    ((uint16_t)STR_PADDED(x)[54] & 0xFF) | ((uint16_t)STR_PADDED(x)[55] << 8), \
    ((uint16_t)STR_PADDED(x)[56] & 0xFF) | ((uint16_t)STR_PADDED(x)[57] << 8), \
    ((uint16_t)STR_PADDED(x)[58] & 0xFF) | ((uint16_t)STR_PADDED(x)[59] << 8), \
    ((uint16_t)STR_PADDED(x)[60] & 0xFF) | ((uint16_t)STR_PADDED(x)[61] << 8), \
    ((uint16_t)STR_PADDED(x)[62] & 0xFF) | ((uint16_t)STR_PADDED(x)[63] << 8), \
    }
#elif CSMON_SET_PARAMETER_NAMEUNIT_LENGTH_MAX == 32
#define STR_PADDED(x) (x "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0")
#define ZIP_STRING(x) { \
    ((uint16_t)STR_PADDED(x)[ 0] & 0xFF) | ((uint16_t)STR_PADDED(x)[ 1] << 8), \
    ((uint16_t)STR_PADDED(x)[ 2] & 0xFF) | ((uint16_t)STR_PADDED(x)[ 3] << 8), \
    ((uint16_t)STR_PADDED(x)[ 4] & 0xFF) | ((uint16_t)STR_PADDED(x)[ 5] << 8), \
    ((uint16_t)STR_PADDED(x)[ 6] & 0xFF) | ((uint16_t)STR_PADDED(x)[ 7] << 8), \
    ((uint16_t)STR_PADDED(x)[ 8] & 0xFF) | ((uint16_t)STR_PADDED(x)[ 9] << 8), \
    ((uint16_t)STR_PADDED(x)[10] & 0xFF) | ((uint16_t)STR_PADDED(x)[11] << 8), \
    ((uint16_t)STR_PADDED(x)[12] & 0xFF) | ((uint16_t)STR_PADDED(x)[13] << 8), \
    ((uint16_t)STR_PADDED(x)[14] & 0xFF) | ((uint16_t)STR_PADDED(x)[15] << 8), \
    ((uint16_t)STR_PADDED(x)[16] & 0xFF) | ((uint16_t)STR_PADDED(x)[17] << 8), \
    ((uint16_t)STR_PADDED(x)[18] & 0xFF) | ((uint16_t)STR_PADDED(x)[19] << 8), \
    ((uint16_t)STR_PADDED(x)[20] & 0xFF) | ((uint16_t)STR_PADDED(x)[21] << 8), \
    ((uint16_t)STR_PADDED(x)[22] & 0xFF) | ((uint16_t)STR_PADDED(x)[23] << 8), \
    ((uint16_t)STR_PADDED(x)[24] & 0xFF) | ((uint16_t)STR_PADDED(x)[25] << 8), \
    ((uint16_t)STR_PADDED(x)[26] & 0xFF) | ((uint16_t)STR_PADDED(x)[27] << 8), \
    ((uint16_t)STR_PADDED(x)[28] & 0xFF) | ((uint16_t)STR_PADDED(x)[29] << 8), \
    ((uint16_t)STR_PADDED(x)[30] & 0xFF) | ((uint16_t)STR_PADDED(x)[31] << 8), \
    }
#endif

#if CSMON_REALADR_16BIT
#define INIT_PARAMFULL(u16ID, paramType, paramAccess, paramPass, funcWr, funcRd, accessWr, accessRd, bStorable, u8Bits, u8Offs, eType,  DataTypeAttribute, u32Address, strName, strUnit, u32Maximum, u32Minimum, u32Default, floatScale, VisualAttribute, u8Elements, bitField, arrayType, bNR, bNW, pFunc) \
 { \
    .u16ParameterIndexID = u16ID,                       /* u16ID */\
    /* .u16ParamAttributes = (PAR(paramType, paramAccess, paramPass)),                u16Attributes */\
    .uBitsCount.sBitsCount.u8BitsCount = (u8Bits),      /* uBitsCount.sBitsCount.u8BitsCount; */\
    .uBitsCount.sBitsCount.u8PaddingByte = (0),         /* uBitsCount.sBitsCount.u8PaddingByte; */\
    .u16RealAddress = (uint16_t)((uint32_t)((uint32_t*)(u32Address))),           /* u32Address */\
    .au8NameUnit = ZIP_STRING(strName "\t" strUnit),    /* au8NameUnit */\
    .eDataTypeAttribute = DataTypeAttribute,               /* eDataTypeAttribute */\
    /* .eVisualTypeAttribute = VisualAttribute,               eVisualTypeAttribute */\
    .u32Max.eType = (u32Maximum),                       /* u32Maximum */\
    .u32Min.eType = (u32Minimum),                       /* u32Minimum */\
    .u32Def.eType = (u32Default),                       /* u32Default */\
    /* .u32Offset.eType = 0,                       u32Offset */\
    .Norm = floatScale,                                 /* floatScale */\
    .uParameterSize.sSize.u8SizeElement = u8Bits / 8, \
    .uParameterSize.sSize.u8CountElements = u8Elements,\
    .uParameterFlags.sFlags.u8BitOffset = u8Offs, \
    .uParameterFlags.sFlags.bBitField = bitField, \
    .uParameterFlags.sFlags.bArray = arrayType,         /* Array Type Register */ \
    .uParameterFlags.sFlags.bReadOnly = 1^(paramAccess >> 1), \
    .uParameterFlags.sFlags.bWriteOnly = 1^(paramAccess & 1), \
    .uParameterFlags.sFlags.bReadDenySkipCSMON = bNR, \
    .uParameterFlags.sFlags.bWriteDenySkipCSMON = bNW, \
    .uFunctionCode.sFunctionCode.eDefaultWriteFunctionCode = funcWr, \
    .uFunctionCode.sFunctionCode.eDefaultReadFunctionCode = funcRd, \
    .uAccessLevel.sAccessLevel.eWriteAccessLevelMin = accessWr, \
    .uAccessLevel.sAccessLevel.eReadAccessLevelMin = accessRd, \
    .uAccessLevel.sAccessLevel.bStore = bStorable, \
 }
#else
#define INIT_PARAMFULL(u16ID, paramType, paramAccess, paramPass, funcWr, funcRd, accessWr, accessRd, bStorable, u8Bits, u8Offs, eType,  DataTypeAttribute, u32Address, strName, strUnit, u32Maximum, u32Minimum, u32Default, floatScale, VisualAttribute, u8Elements, bitField, arrayType, bNR, bNW, pFunc) \
 { \
    .u16ParameterIndexID = u16ID,                       /* u16ID */\
    /* .u16ParamAttributes = (PAR(paramType, paramAccess, paramPass)),                u16Attributes */\
    .uBitsCount.sBitsCount.u8BitsCount = (u8Bits),      /* uBitsCount.sBitsCount.u8BitsCount; */\
    .uBitsCount.sBitsCount.u8PaddingByte = (0),         /* uBitsCount.sBitsCount.u8PaddingByte; */\
    .u32RealAddress = (uint32_t)(u32Address),           /* u32Address */\
    .au8NameUnit = ZIP_STRING(strName "\t" strUnit),    /* au8NameUnit */\
    .eDataTypeAttribute = DataTypeAttribute,               /* eDataTypeAttribute */\
    /* .eVisualTypeAttribute = VisualAttribute,               eVisualTypeAttribute */\
    .u32Max.eType = (u32Maximum),                       /* u32Maximum */\
    .u32Min.eType = (u32Minimum),                       /* u32Minimum */\
    .u32Def.eType = (u32Default),                       /* u32Default */\
    /* .u32Offset.eType = 0,                       u32Offset */\
    .Norm = floatScale,                                 /* floatScale */\
    .uParameterSize.sSize.u8SizeElement = u8Bits / 8, \
    .uParameterSize.sSize.u8CountElements = u8Elements,\
    .uParameterFlags.sFlags.u8BitOffset = u8Offs, \
    .uParameterFlags.sFlags.bBitField = bitField, \
    .uParameterFlags.sFlags.bArray = arrayType,         /* Array Type Register */ \
    .uParameterFlags.sFlags.bReadOnly = 1^(paramAccess >> 1), \
    .uParameterFlags.sFlags.bWriteOnly = 1^(paramAccess & 1), \
    .uParameterFlags.sFlags.bReadDenySkipCSMON = bNR, \
    .uParameterFlags.sFlags.bWriteDenySkipCSMON = bNW, \
    .uFunctionCode.sFunctionCode.eDefaultWriteFunctionCode = funcWr, \
    .uFunctionCode.sFunctionCode.eDefaultReadFunctionCode = funcRd, \
    .uAccessLevel.sAccessLevel.eWriteAccessLevelMin = accessWr, \
    .uAccessLevel.sAccessLevel.eReadAccessLevelMin = accessRd, \
    .uAccessLevel.sAccessLevel.bStore = bStorable, \
    .u16Dummy = 0, \
 }
#endif

#define INIT_PARAMETER(u16ID, paramType, paramAccess, paramPass, funcWr, funcRd, accessWr, accessRd, bStorable, u8Bits, u8Offs, eType,  DataTypeAttribute, u32Address, strName, strUnit, u32Maximum, u32Minimum, u32Default, floatScale) \
        INIT_PARAMFULL(u16ID, paramType, paramAccess, paramPass, funcWr, funcRd, accessWr, accessRd, bStorable, u8Bits, u8Offs, eType,  DataTypeAttribute, u32Address, strName, strUnit, u32Maximum, u32Minimum, u32Default, floatScale, CSMON_VISUAL_TYPE_HEX, 1, 0, 0, 0, 0, NULL)
#endif

#endif

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
#if CSMON_CONFIG == 0
#ifndef uWord32_t
typedef union
{
    uint32_t u32Register;
    uint16_t au16Word[2];
} uWord32_t;
#endif

typedef union
{
    uint32_t u32Register;
     int32_t s32Register;
    uint16_t u16Register;
     int16_t s16Register;
    uint_least8_t u8Register;
     int_least8_t s8Register;
} uAnyType32_t;
#endif


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

#if CSMON_CONFIG == 0

typedef struct
{
    uint16_t u16ParameterIndexID;
#if _CSMON_USE_EXTERNAL_PARAMETER_LIST == 0
    uint16_t u16ParamAttributes;
#else
    CSMON_uParameterDefinitionsBitsCount_t uBitsCount;
#endif
#if _CSMON_USE_EXTERNAL_PARAMETER_LIST == 0
    char au8Name[CSMON_SET_PARAMETER_NAME_LENGTH_MAX];
    char au8Unit[CSMON_SET_PARAMETER_UNIT_LENGTH_MAX];
#else
    char au8NameUnit[(CSMON_SET_PARAMETER_NAMEUNIT_LENGTH_MAX)/2];
#endif
    uAnyType32_t u32Max;
    uAnyType32_t u32Min;
    uAnyType32_t u32Def;
    //uAnyType32_t u32Offset;
    float Norm;                 /* 0.0 - Default HEX Visualization; Any other -> Default Decimal Visualization */
#if _CSMON_USE_EXTERNAL_PARAMETER_LIST == 0
    uint_least8_t u8BitCountOrArrayElementSize;
    uint_least8_t u8StartBitOrArrayElementCount;
#else
    CSMON_uParameterDefinitionsFunctionCode_t uFunctionCode;
    CSMON_uParameterDefinitionsAccessLevel_t uAccessLevel;
#endif
    CSMON_uRegisterAddressTableSize uParameterSize;
    CSMON_uRegisterAddressTableFlags uParameterFlags;
#if _CSMON_USE_EXTERNAL_PARAMETER_LIST == 0
    uint32_t u32ProcessFunc;
    CSMON_eVisualType_t eVisualTypeAttribute;
#endif
#if CSMON_REALADR_16BIT
    uint16_t u16RealAddress;
    CSMON_eDataType_t eDataTypeAttribute;
#else
    uint32_t u32RealAddress;
    CSMON_eDataType_t eDataTypeAttribute;
    uint16_t u16Dummy;
#endif

}MAIN_sParameterList_t;

#endif

typedef struct
{
    CSMON_sExternalRecorderStatus sStatus;
    uint32_t u32StartAddressFirstDataSample;
    uint32_t u32CircleBufferSampleCount;
    uint32_t u32CircleBufferStartAddress;
    CSMON_eMicroSec0BuffOvf1Mode_t eTriggerSubSecondMode;   /* Used Sync Time Between Recorder's Samples at Trigger Condition detection */
    uint32_t u32TriggerMicrosecondsOrRollingTimerTicks;
    uint_least8_t u8TriggerBCDSeconds;
    uint_least8_t u8TriggerBCDMinutes;
    uint_least8_t u8TriggerBCDHours;
    uint_least8_t u8TriggerBCDWeekdays;
    uint_least8_t u8TriggerBCDDay;
    uint_least8_t u8TriggerBCDMonth;
    uint_least8_t u8TriggerBCDYear;

}MAIN_sExternalRecorderHandle_t;



/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */
#ifndef STRING_CONCAT
#define STRING_CONCAT_BASE(_a_, _b_) _a_##_b_
#define STRING_CONCAT(_a_, _b_) STRING_CONCAT_BASE(_a_, _b_)
#endif
#ifndef GPIO_PIN_MODE
#define GPIO_PIN_MODE(_pin_, _mode_) STRING_CONCAT(GPIO_, STRING_CONCAT(_pin_, STRING_CONCAT(_, _mode_)))
#define GPIO_PIN_MODE_GPIO(_pin_) STRING_CONCAT(GPIO_, STRING_CONCAT(_pin_, STRING_CONCAT(_GPIO, _pin_)))
#endif
#ifndef IO_DRV_H

#if defined(__TMS320F2806x__)

//moved to gpio.c / gpio.h
//void GPIO_writePin_2806x(uint32_t pin, uint32_t level)
//{
//    if (level == 1)
//    {
//        if (pin < 32)
//        {
//            GpioDataRegs.GPASET.all |= ((uint32_t)1 << pin);
//        }
//        else
//        {
//            GpioDataRegs.GPBSET.all |= ((uint32_t)1 << (pin-32));
//        }
//
//    }
//    else
//    {
//        if (pin < 32)
//        {
//            GpioDataRegs.GPACLEAR.all |= ((uint32_t)1 << pin);
//        }
//        else
//        {
//            GpioDataRegs.GPBCLEAR.all |= ((uint32_t)1 << (pin-32));
//        }
//    }
//}


//void GPIO_setPinConfigOutput(uint32_t PinNumber)
//{
//    Uint32 mask;
//
//    // Before calling the Toggle Test, we must setup
//    // the MUX and DIR registers.
//
//    //led_pin_write(PinNumber, 1);
//
//    if(PinNumber > (Uint16)47)
//    {
//        asm("    ESTOP0");  // Stop here. Invalid option.
//        for(;;);
//    }
//
//    // Pins GPIO32-GPIO47
//    else if(PinNumber >= 32)
//    {
//        EALLOW;
//        mask = ~( ((Uint32)1 << (PinNumber-32)*2) | ((Uint32)1 << (PinNumber-32)*2+1) );
//        GpioCtrlRegs.GPBMUX1.all &= mask;
//        GpioCtrlRegs.GPBDIR.all = GpioCtrlRegs.GPBDIR.all | ((Uint32)1 << (PinNumber-32) );
//        EDIS;
//    }
//
//    // Pins GPIO16-GPIO31
//    else if(PinNumber >= 16)
//    {
//        EALLOW;
//        mask = ~( ((Uint32)1 << (PinNumber-16)*2) | ((Uint32)1 << (PinNumber-16)*2+1) );
//        GpioCtrlRegs.GPAMUX2.all &= mask;
//        GpioCtrlRegs.GPADIR.all = GpioCtrlRegs.GPADIR.all | ((Uint32)1 << PinNumber);
//        EDIS;
//    }
//
//    // Pins GPIO0-GPIO15
//    else
//    {
//        EALLOW;
//        mask = ~( ((Uint32)1 << PinNumber*2) | ((Uint32)1 << PinNumber*2+1 ));
//        GpioCtrlRegs.GPAMUX1.all &= mask;
//        GpioCtrlRegs.GPADIR.all = GpioCtrlRegs.GPADIR.all | ((Uint32)1 << PinNumber);
//        EDIS;
//    }
//
//}
#else
#define GPIO_setPinConfigInput(_pin_) \
        GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(_pin_));\
        GPIO_setDirectionMode(_pin_, GPIO_DIR_MODE_IN);\
        GPIO_setPadConfig(_pin_, GPIO_PIN_TYPE_STD);\
        GPIO_setQualificationMode(_pin_, GPIO_QUAL_6SAMPLE);\
        GPIO_setMasterCore(_pin_, GPIO_CORE_CPU1)

#define GPIO_setPinConfigOutput(_pin_) \
        GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(_pin_));\
        GPIO_setDirectionMode(_pin_, GPIO_DIR_MODE_OUT);\
        GPIO_setPadConfig(_pin_, GPIO_PIN_TYPE_STD);\
        GPIO_setQualificationMode(_pin_, GPIO_QUAL_ASYNC);\
        GPIO_writePin(_pin_, 0);\
        GPIO_setMasterCore(_pin_, GPIO_CORE_CPU1)


#endif
#endif

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
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

#if EXTERNAL_RECORDERS == 1

#if _CSMON_USE_EXTERNAL_PARAMETER_LIST
#define RAM_BUFFER_SIZE_0   8192
uint16_t u16RecorderBufferRam[RAM_BUFFER_SIZE_0] = {0};
#endif

MAIN_sExternalRecorderHandle_t sExternalRecoderHandle[RECORDER_COUNT] =
{
#if _CSMON_USE_EXTERNAL_PARAMETER_LIST == 0
 {
  {.bTriggered = 0, .bReady = 0, .bWrongConfig = 0, .bNotRunning = 1, .bInit = 1, }, /* sStatus */
  NULL,                                     /* u32StartAddressFirstDataSample */
  RECORDER0_TOTAL_SAMPLE_COUNT,             /* u32CircleBufferSampleCount */
  0x140000,                                 /* u32CircleBufferStartAddress - EMIF */
  CSMON_TIME_MICRO_SECONDS,                 /* eTriggerSubSecondMode */
  0,                                        /* u32TriggerMicrosecondsOrRollingTimerTicks */
  0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01  /* 2001-01-01-Mon-00:00:00 */
 },
#else
 {
  {.bTriggered = 0, .bReady = 0, .bWrongConfig = 0, .bNotRunning = 1, .bInit = 1, }, /* sStatus */
  NULL,                                     /* u32StartAddressFirstDataSample */
  RAM_BUFFER_SIZE_0/(4+8),             /* u32CircleBufferSampleCount */
  (uint32_t)u16RecorderBufferRam,                                 /* u32CircleBufferStartAddress - EMIF */
  CSMON_TIME_MICRO_SECONDS,                 /* eTriggerSubSecondMode */
  0,                                        /* u32TriggerMicrosecondsOrRollingTimerTicks */
  0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01  /* 2001-01-01-Mon-00:00:00 */
 },
#endif
#if RECORDER0_ONLY_TEST == 0
 {
  {.bTriggered = 0, .bReady = 0, .bWrongConfig = 0, .bNotRunning = 1, .bInit = 1, }, /* sStatus */
  NULL,                                     /* u32StartAddressFirstDataSample */
  RECORDER1_TOTAL_SAMPLE_COUNT,             /* u32CircleBufferSampleCount */
  0x180000,                                 /* u32CircleBufferStartAddress - EMIF */
  CSMON_TIME_MICRO_SECONDS,                 /* eTriggerSubSecondMode */
  0,                                        /* u32TriggerMicrosecondsOrRollingTimerTicks */
  0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01  /* 2001-01-01-Mon-00:00:00 */
 },
 {
  {.bTriggered = 0, .bReady = 0, .bWrongConfig = 0, .bNotRunning = 1, .bInit = 1, }, /* sStatus */
  NULL,                                     /* u32StartAddressFirstDataSample */
  RECORDER2_TOTAL_SAMPLE_COUNT,             /* u32CircleBufferSampleCount */
  0x1C0000,                                 /* u32CircleBufferStartAddress - EMIF */
  CSMON_TIME_MICRO_SECONDS,                 /* eTriggerSubSecondMode */
  0,                                        /* u32TriggerMicrosecondsOrRollingTimerTicks */
  0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01  /* 2001-01-01-Mon-00:00:00 */
 },
#endif
};


#endif  /* #if EXTERNAL_RECORDERS == 1 */



    bool MAIN_bDateTimeSet = false;






    bool bDummyStatsDevRunning = false;
    bool bDummyReqstDevRunning = false;

    int16_t s16DummyCurrentPhaseAStartup = 0;
    int16_t s16DummyCurrentPhaseBStartup = 0;
    int16_t s16DummyCurrentPhaseCStartup = 0;
    int16_t s16DummyVoltageDCLinkStartup = 0;

    int16_t s16DummyCurrentPhaseAStop = -1000;
    int16_t s16DummyCurrentPhaseBStop = -2000;
    int16_t s16DummyCurrentPhaseCStop = -3000;
    int16_t s16DummyVoltageDCLinkStop = -10000;

    int16_t s16DummyCurrentPhaseAIdle = 100;
    int16_t s16DummyCurrentPhaseBIdle = 200;
    int16_t s16DummyCurrentPhaseCIdle = 300;
    int16_t s16DummyVoltageDCLinkIdle = 1000;

    int16_t s16DummyIncrementLoopCDiff = 5; //(1 << 8);

    int16_t s16DummyIncrementLoopA = 3; //(1 << 8);
    int16_t s16DummyIncrementLoopB = 10; //(1 << 8);
    int16_t s16DummyIncrementLoopC = 1; //(1 << 8);
    int16_t s16DummyIncrementLoopV = 1; //(1 << 8);

    int16_t s16DummyCurrentPhaseA = (0 << 14);
    int16_t s16DummyCurrentPhaseB = (1 << 14);
    int16_t s16DummyCurrentPhaseC = (2 << 14);
    int16_t s16DummyVoltageDCLink = (0 << 14);
    int16_t s16DummyIncrementLoop = 10; //(1 << 8);

    int32_t s32DummyCurrentPhaseA = 0;
    int32_t s32DummyCurrentPhaseB = 0;
    int32_t s32DummyCurrentPhaseC = 0;
    int32_t s32DummyVoltageDCLink = 0;
    int32_t s32DummyIncrementLoop = 0;

 uint64_t u64DummyDataCnt = 0;
 uint32_t u32DummyDataCnt = 0;
 uint16_t u16DummyDataCnt = 0;
 uint16_t* pu16ModbusMessageCounter = &u16DummyDataCnt;

 int16_t s16DummyDataTst = 100;
 int16_t s16ScaleDataTst = 100;



typedef uint16_t PARS_START[__LINE__];
 uint16_t u16TestData0 = 0;
 uint16_t u16TestData1 = 1;
 uint16_t u16TestData2 = 2;
 uint16_t u16TestData3 = 3;
 uint16_t u16TestData4 = 4;
 uint16_t u16TestData5 = 5;
 uint16_t u16TestData6 = 6;
 uint16_t u16TestData7 = 7;
 uint16_t u16TestData8 = 8;
 uint16_t u16TestData9 = 9;
typedef uint16_t PARS_END[__LINE__];

#define PARAMETER_COUNT_TEST sizeof(PARS_END)-sizeof(PARS_START)-1

 int16_t s16DummyData = PARAMETER_COUNT_TEST;
 int32_t aDummyDataTable = 0x1234;

 uint16_t u16WatchdogPrescaler = 0;
 uint16_t u16WatchdogPrescalerOld = 0;

 uint16_t u16WatchdogPrescalerTime = 0;
 uint16_t u16WatchdogPrescalerTimeOld = 0;

uint16_t u16DelayCtrlLoop_100nsec = 0;   /* Control loop Period 50usec - to be checked */
uint16_t u16DelayCtrlLoopOld_100nsec = 0;

uint16_t u16DelayMainLoop_usec = 0;
uint16_t u16DelayMainLoopOld_usec = 0;

uint16_t u16PeriodControl_usec = 50;            /* Default Initialization Value for ISR Period */
uint16_t u16PeriodControlOld_usec = 0;

uint32_t u32DelayCtrlLoop_Ticks = 1;
uint32_t u32DelayMainLoop_Ticks = 1;

uint16_t u16CountSetParameterFree = 0;
uint16_t u16CountSetParameterFail = 0;
uint16_t u16CountMaxParameterTest = BOARDCFG_CSMON_FILE_PARAMETER_COUNT_MAX;/* Parameter 9 was independent RD and WR --> to be fixed depending on test configuration */

bool bResetAllTimeMeasures = 0;

uint32_t u32TimeMainLoopProcessCSMON_Bgn_Ticks = 0;
uint32_t u32TimeMainLoopProcessCSMON_End_Ticks = 0;
uint32_t u32TimeMainLoopProcessCSMON_Now_Ticks = 0;
uint32_t u32TimeMainLoopProcessCSMON_Max_Ticks = 0;

uint32_t u32TimeMainLoopCycle_Bgn_Ticks = 0;
uint32_t u32TimeMainLoopCycle_End_Ticks = 0;
uint32_t u32TimeMainLoopCycle_Now_Ticks = 0;
uint32_t u32TimeMainLoopCycle_Max_Ticks = 0;


uint32_t u32TimeCSMON_ISR_Ticks = 0;
uint32_t u32TimeCSMON_ISR_Max_Ticks = 0;
uint32_t u32TimeCtrlLoop_Ticks = 0;
uint32_t u32TimeCtrlLoopMax_Ticks = 0;


uint32_t u32ParamTime_Ticks;

char au8TestCharArray32[32] = "au8TestCharArray32";
char au8TestCharArray16[16] = "auChar16";
char au8TestCharArray8[8]   = "Tst08";



CSMON_eResponseCode_t eResponseCode_CSMON_eInit = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eProcess = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetServerOnStatus = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetParameter = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetRecorder = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetScope = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetTimerPeriodISRFunctionRegister = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetFlagProcessPassed = CSMON_RESPONSE_CODE_OK;
uint32_t u32GetBaudError_PPM = 0;




/* Parameter Table Version, DateTime, Checksum */
#define PARAM_TABLE_VERSION                 (uint32_t)1
#define PARAM_TABLE_DATETIME                (uint32_t)DATETIME_BUILD

#ifndef __TMS320F2806x__
#define EMIF_AUX_PARAM_TABLE_VER_ADDRESS    0x00120006      /* This address here I put only for the example. You should use your memory space address */
#define EMIF_AUX_BUILD_DATE_TIME_ADDRESS    0x00120008      /* This address here I put only for the example. You should use your memory space address */
#define EMIF_AUX_BACKUP_CHECKSUM_ADDRESS    0x0012000A      /* This address here I put only for the example. You should use your memory space address */


volatile uint16_t* EMIF_AUX_pu16ParamVerBackupInEmif = (uint16_t*)(EMIF_AUX_PARAM_TABLE_VER_ADDRESS);
volatile uint16_t* EMIF_AUX_pu16DateTimeBackupInEmif = (uint16_t*)(EMIF_AUX_BUILD_DATE_TIME_ADDRESS);
volatile uint16_t* EMIF_AUX_pu16CheckSumBackupInEmif = (uint16_t*)(EMIF_AUX_BACKUP_CHECKSUM_ADDRESS);

#endif



/* Dummy Parameter List */

#define PARAM_ID_MODBUS_MSG_CNT    65534

#define PARAM_ID_STARUNNINGMODE     0
#define PARAM_ID_VOLTAGE_DCLINK     8
#define PARAM_ID_CURRENT_PHASEA     9
#define PARAM_ID_CURRENT_PHASEB     10
#define PARAM_ID_CURRENT_PHASEC     11

#define PARAM_ID_VOLTAGE_DCLINK_32  28
#define PARAM_ID_CURRENT_PHASEA_32  29
#define PARAM_ID_CURRENT_PHASEB_32  30
#define PARAM_ID_CURRENT_PHASEC_32  31


#if CSMON_CONFIG == 0


#if _CSMON_USE_EXTERNAL_PARAMETER_TABLE

/* *****************************************************************************
 * EnerDrive Definitions
 **************************************************************************** */
#define STRING_PARAMS

#define LANGUAGE_ENGLISH    1
//#define  LANGUAGE_GERMAN 1

#define PROJECT_NAME        "Convesys DFIG"
#define DEVICE_TYPE     "Convesys DFIG CPU1"
#define HW_VERSION      "1"
#define SW_VERSION      "600"
#define SW_BUILD_NUM        "0"
#define SW_BUILD_DATE   __DATE__
#define SW_TOOL_VERSION "9.30"


/********************************************************************************
 *  macros                                                                                                                                          *
 ********************************************************************************/
//#define SPIE2P_TEST       1


#define MAX_NAME_LEN    60             // must be even value!
#define MAX_EVENTSTRING_LEN MAX_NAME_LEN
#define MAX_UNIT_LEN    10
#if SERIELL_NV_MEM
   #define SIZE_OF_EEP_SHADOW 5000     // 128kB EEPROM = 8192 word max.
#elif PARALLEL_NV_MEM
   #define SIZE_OF_EEP_SHADOW 5000
#else
   #define SIZE_OF_EEP_SHADOW 0
#endif

#define PAR_TYPES   0x000f          // Maske für Typencodierung im Parameterattribut

#define _UINT16         0x0001          // UINT16
#define _INT16          0x0002          // INT16
#define _UINT32         0x0003          // UINT32
#define _INT32          0x0004          // INT32
#define _WORD               0x0005          // WORD
#define _DWORD          0x0006          // DWORD
#define _A_UINT16       0x0007          // Array of UINT16
#define _A_INT16        0x0008          // Array of INT16
#define _A_UINT32       0x0009          // Array of UINT32
#define _A_INT32        0x000a          // Array of INT32
#define _A_WORD         0x000b          // Array of WORD
#define _A_DWORD        0x000c          // Array of DWORD
#define _UINT8          0x000d          //!< byte
#define _A_UINT8        0x000e          //!< string

#define PAR_ACCESS  0x0030          // Maske für Zugriffscodierung im Parameterattribut
#define PAR_NA          0x0000          // kein Zugriff möglich
#define PAR_RO          0x0010          // ReadOnly
#define PAR_WO          0x0020          // WriteOnly
#define PAR_RW          0x0030          // ReadWrite

#define PAR_STATE       0x0040          // verbietet Schreibzugriffe in bestimmten Zuständen
#define PAR_PASS_PR     0x0080          // Zugriff nur mit Passcode, Parameter hidden
#define PAR_PASS_PW     0x0100          // Zugriff nur mit Passcode, read erlaubt

#define PAR_A_BIT      0x0200           // bit array

#define PAR_WRITE       0x0c00          // Schreib modi
#define PAR_WR_DIR  0x0400          // direkter Schreibzugriff möglich
#define PAR_WR_FKT  0x0800          // Schreibzugriff nur über spezielle Behandlungsfunktion

#define PAR_STORE_EE    0x4000          // Parameter for storing in EEPROM
#define PAR_STORE       0x8000          // Parameter for storing in Parameterfile

#define PAR_MENUE       0x1000          // Parameter beschreibt Parametergruppe
#define PAR_HIDE        0x2000          // Parameter erscheint nicht im Menübaum

#define PAR_UPDATE  0x0001          // Kennung eines Parameters im RAM-Spiegel des EEPROM zum Speichern
#define PAR_EQUAL       0x0002          // Kennung .... Speichern nicht nötig

#define PAR_READY       0x0000          // keine Parameter nv-mem aktion aktiv
#define PAR_BUSY        0x0001          // eine Parameter nv-mem aktion ist aktiv

#define PASSCODE        8005



#define LAST_PAR_INDEX      1999            //RS für TSP Kanalauswahlbegrenzung



#define SQRT3                  1.73205808
#define SQRT3HALBE                   (int)(MAX_INT*SQRT3/2)
#define SQRT2                            1.41421356
#define SQRT2I                         (int)(SQRT2*MAX_INT)
#define ZWEIDRITTEL                (int)(MAX_INT*0.666666)
#define EINDRITTEL                   (int)(MAX_INT*1.0/3.0)
#define DURCH_SQRT3              (int)(MAX_INT*(1/SQRT3))
#define MAX_INT                0x7fff
#define MAX_UINT               0xffff
#define MAX_LONG               0x7fffffffL
#define MAX_ULONG              0xffffffffL
#define PI                     3.141592654
#define PROZENT                      100
#define PROMILLE                       1000
#define LOG10_MOD2             3.32193      // Modul des dekadischen log. in Bezug auf den dualen



//! return codes des Parameterinterfaces
typedef enum
{
    RC_PARA_OK,                                     //!< 0, fehlerfreier Zugriff
    RC_PARA_NOT_EXIST,                      //!< 1, Parameter existiert nicht
    RC_ELEM_NOT_EXIST,                      //!< 2, Element existiert nicht
    RC_ATTR_NOT_EXIST,                      //!< 3, Attribut existiert nicht
    RC_NULL_POINTER,                            //!< 4, es wurde ein NULL-Pointer übergeben
    RC_WRONG_TYPE,                              //!< 5, falscher Typ
    RC_WRONG_SIZE,                              //!< 6, falsche Größe
    RC_VALUE_HIGH,                              //!< 7, Wert zu groß
    RC_VALUE_LOW,                                   //!< 8, Wert zu klein
    RC_READ_ONLY,                                   //!< 9, Schreibversuch auf Read-only
    RC_WRITE_ONLY,                              //!< 10, Leseversuch auf Write-only
    RC_PERM_DENIED,                             //!< 11, keine Zugriffsberechtigung
    RC_PAR_WRONG_CMD,                           //!< 12, falscher Kommandocode
    RC_PAR_WRONG_FRAME,                     //!< 13, Gesamtframe header + Daten hat falsche Länge
    RC_PAR_WRONG_STATE_WRITE,           //!< 14, Schreiben im aktuellen Zustand nicht erlaubt
    RC_PAR_UNVALID_DATA                     //!< 15, ungültige Daten
} RC_PAR_T;


//! Sprachen
typedef enum
{
    LANG_GERMAN,                                    //!< Deutsch
    LANG_ENGLISH                                    //!< Englisch
} LANGUAGE_T;

#ifndef NULL
  #define NULL  ((void *)0)
#endif

   typedef unsigned int     UINT16;
   typedef unsigned char    BOOL;
   typedef signed int       INT16;
   typedef signed long      INT32;
    typedef signed long long INT64;
   typedef char             INT8;
   typedef char                 UINT8;
   typedef unsigned long    UINT32;
   typedef unsigned long long UINT64;
    typedef unsigned int    WORD;
    typedef unsigned long   DWORD;
    typedef int int8_T;
    typedef unsigned int uint8_T;
    typedef int int16_T;
    typedef unsigned int uint16_T;
    typedef long int32_T;
    typedef unsigned long uint32_T;
    typedef float real32_T;
    typedef double real64_T;


//*******************************************************************************
//  Beschreibungsstruktur für einen U16-Parameter
typedef const struct
{
  UINT16    Attr;                                       //!< El. 1, Attribute des Parameters
  //INT8*   Name;                                       //!< El. 2, Name
  UINT8*    Name;                                       //!< El. 2, Name
  UINT8*    Unit;                                       //!< El. 3, Einheit
  UINT16    Max;                                        //!< El. 4, Maximalwert
  UINT16    Min;                                        //!< El. 5, Minimalwert
  UINT16    Def;                                        //!< El. 6, Defaultwert
  float     Norm;                                       //!< El. 7, Normierungsfaktor nur für Darstellung auf Anzeigegerät
  UINT16*   pVal;                                       //!< El. 8, Zeiger auf Wert
  RC_PAR_T (*pfFct) (UINT16);           //!< Zeiger auf spezielle Behandlungsfunktion
} PAR_DES_U16_T;
//*******************************************************************************

//  Beschreibungsstruktur für einen I16-Parameter
typedef const struct
{
  UINT16    Attr;                                   //!< El. 1, Attribute des Parameters
  UINT8*    Name;                                   //!< El. 2, Name
  UINT8*    Unit;                                   //!< El. 3, Einheit
  INT16     Max;                                    //!< El. 4, Maximalwert
  INT16     Min;                                    //!< El. 5, Minimalwert
  INT16     Def;                                    //!< El. 6, Defaultwert
  float     Norm;                                   //!< El. 7, Normierungsfaktor nur für Darstellung auf Anzeigegerät
  INT16*    pVal;                                   //!< El. 8, Zeiger auf Wert
  RC_PAR_T (*pfFct) (INT16);        //!< Zeiger auf spezielle Behandlungsfunktion
} PAR_DES_I16_T;

//  Beschreibungsstruktur für einen U32-Parameter
typedef const struct
{
  UINT16    Attr;                                       //!< El. 1, Attribute des Parameters
  UINT8*    Name;                                       //!< El. 2, Name
  UINT8*    Unit;                                       //!< El. 3, Einheit
  UINT32    Max;                                        //!< El. 4, Maximalwert
  UINT32    Min;                                        //!< El. 5, Minimalwert
  UINT32    Def;                                        //!< El. 6, Defaultwert
  float     Norm;                                       //!< El. 7, Normierungsfaktor nur für Darstellung auf Anzeigegerät
  UINT32*   pVal;                                       //!< El. 8, Zeiger auf Wert
  RC_PAR_T (*pfFct) (UINT32);           //!< Zeiger auf spezielle Behandlungsfunktion
} PAR_DES_U32_T;

// Beschreibungsstruktur für einen I32-Parameter
typedef const struct
{
  UINT16    Attr;                                   //!< El. 1, Attribute des Parameters
  UINT8*    Name;                                   //!< El. 2, Name
  UINT8*    Unit;                                   //!< El. 3, Einheit
  INT32     Max;                                    //!< El. 4, Maximalwert
  INT32     Min;                                    //!< El. 5, Minimalwert
  INT32     Def;                                    //!< El. 6, Defaultwert
  float     Norm;                                   //!< El. 7, Normierungsfaktor nur für Darstellung auf Anzeigegerät
  INT32*    pVal;                                   //!< El. 8, Zeiger auf Wert
  RC_PAR_T (*pfFct) (INT32);        //!< Zeiger auf spezielle Behandlungsfunktion
} PAR_DES_I32_T;

// Beschreibungsstruktur für einen WORD-Parameter
typedef const struct
{
  UINT16    Attr;                                   //!< El. 1, Attribute des Parameters
  UINT8*    Name;                                   //!< El. 2, Name
  WORD      Max;                                    //!< El. 4, Maximalwert
  WORD      Min;                                    //!< El. 5, Minimalwert
  WORD      Def;                                    //!< El. 6, Defaultwert
  WORD* pVal;                                       //!< El. 8, Zeiger auf Wert
  RC_PAR_T (*pfFct) (WORD);         //!< Zeiger auf spezielle Behandlungsfunktion
} PAR_DES_WORD_T;

// Beschreibungsstruktur für einen DWORD-Parameter
typedef const struct
{
  UINT16    Attr;                                   //!< El. 1, Attribute des Parameters
  UINT8*    Name;                                   //!< El. 2, Name
  DWORD     Max;                                    //!< El. 4, Maximalwert
  DWORD     Min;                                    //!< El. 5, Minimalwert
  DWORD     Def;                                    //!< El. 6, Defaultwert
  DWORD*    pVal;                                   //!< El. 8, Zeiger auf Wert
  RC_PAR_T (*pfFct) (DWORD);        //!< Zeiger auf spezielle Behandlungsfunktion
} PAR_DES_DWORD_T;

//! Beschreibungsstruktur für einen UINT8-Feld-Parameter
typedef const struct
{
  UINT16    Attr;                                   //!< El. 1, Attribute des Parameters
  UINT8*    Name;                                   //!< El. 2, Name
  UINT16    size;                                   //!< El. 3, Größe des Felds
  UINT8*    pVal;                                   //!< El. 4, Zeiger auf Wert
  RC_PAR_T (/*far*/ *pfFct)(const UINT8*, UINT16);//!< Zeiger auf spezielle Behandlungsfunktion
} PAR_DES_A_UINT8_T;



// Parameter-Variable MaxParams
//#pragma DATA_SECTION(ParVal_MaxParams,".extram");
UINT16 ParVal_MaxParams;

// größte unterstützte Parameternummer
// Parameter-Beschreibung MaxParams
PAR_DES_U16_T ParDes_MaxParams =
{
    PAR_RO | _UINT16,                                   // ReadOnly, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "groesste Parameter-Nr.",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Maximum parameter number",             // Name
#endif
#endif
    NULL,                                                           // Einheit
    0xFFFF,                                                     // Maximum
    0x0000,                                                     // Minimum
    LAST_PAR_INDEX,                                                     // Default
    1.0,                                                            // Faktor
    &ParVal_MaxParams,                              // Parameter
    NULL                                                            // Behandlungsfunktion
};

#ifdef STRING_PARAMS

// Dieser Parameter enthält den PROJEKTNAMEN der Applikation.
// Damit er als solcher erkannt wird, muß dieser Parameter
// der ERSTE PARAMETER IN DER PARAMETER-LISTE sein, der das Attribut PAR_MENUE hat!
PAR_DES_WORD_T ParDes_PROJECT_NAME =
{
  PAR_RO | _WORD | PAR_MENUE,       // Attribut
  PROJECT_NAME,                     // Name: aus Version_info.h
  0xFFFF,                           // Maximum
  0x0000,                           // Minimum
  0x0000,                           // Default
  NULL,                             // Parameter
  NULL                              // Behandlungsfunktion
};

// GruppenParameter-Geräteidentifikation
// Geräteidentifikation

// Parameter-Beschreibung GruppenParameter Geräteidentifikation
PAR_DES_WORD_T ParDes_GROUP_DEVICEIDENT =
{
    PAR_MENUE | PAR_RO | _WORD,     // Attribut
    "Device Identification",        // Name
    0xFFFF,                         // Maximum
    0x0000,                         // Minimum
    0x0000,                         // Default
    NULL,                           // Parameter
    NULL                            // Behandlungsfunktion
};


// Parameter-Variable Geräte-Identifikation
//#pragma DATA_SECTION(ParVal_DeviceType,".extram");
UINT8 ParVal_DeviceType[MAX_NAME_LEN];

// Gerätetyp
// Parameter-Beschreibung DeviceIdent
PAR_DES_A_UINT8_T ParDes_DeviceType =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
#ifdef LANGUAGE_GERMAN
    "device type",                                          // Name
#else
#ifdef LANGUAGE_ENGLISH
    "device type",              // Name
#endif
#endif
    MAX_NAME_LEN,                                           // Maximum
    ParVal_DeviceType,                                  // Parameter
    NULL                                                    // Behandlungsfunktion
};

// Parameter-Variable HW-Version
//#pragma DATA_SECTION(ParVal_HWVersion,".extram");
UINT8 ParVal_HWVersion[MAX_NAME_LEN];

// HW-Version
// Parameter-Beschreibung HW-Version
PAR_DES_A_UINT8_T ParDes_HWVersion =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
#ifdef LANGUAGE_GERMAN
    "hardware version",                                         // Name
#else
#ifdef LANGUAGE_ENGLISH
    "hardware version",             // Name
#endif
#endif
    MAX_NAME_LEN,                                           // Maximum
    ParVal_HWVersion,                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

// Parameter-Variable SW-Version
//#pragma DATA_SECTION(ParVal_SWVersion,".extram");
UINT8 ParVal_SWVersion[MAX_NAME_LEN];

// SW-Version
// Parameter-Beschreibung SW-Version
PAR_DES_A_UINT8_T ParDes_SWVersion =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
#ifdef LANGUAGE_GERMAN
    "software version",                                         // Name
#else
#ifdef LANGUAGE_ENGLISH
    "software version",             // Name
#endif
#endif
    MAX_NAME_LEN,                                           // Maximum
    ParVal_SWVersion,                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

// Parameter-Variable SWBuildNum
// build nummer, automatisch generiert
//#pragma DATA_SECTION(ParVal_SWBuildNum,".extram");
UINT8 ParVal_SWBuildNum[MAX_NAME_LEN];

// SWBuildNum
// Parameter-Beschreibung SWBuildNum
PAR_DES_A_UINT8_T ParDes_SWBuildNum =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
#ifdef LANGUAGE_GERMAN
    "software build number",                                            // Name
#else
#ifdef LANGUAGE_ENGLISH
    "software build number",                // Name
#endif
#endif
    MAX_NAME_LEN,                                           // Maximum
    ParVal_SWBuildNum,                                  // Parameter
    NULL                                                    // Behandlungsfunktion
};

// Parameter-Variable SWBuildDate
//#pragma DATA_SECTION(ParVal_SWBuildDate,".extram");
UINT8 ParVal_SWBuildDate[MAX_NAME_LEN];

// SWBuildDate
// Parameter-Beschreibung SWBuildDate
PAR_DES_A_UINT8_T ParDes_SWBuildDate =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
#ifdef LANGUAGE_GERMAN
    "software build date",                                          // Name
#else
#ifdef LANGUAGE_ENGLISH
    "software build date",              // Name
#endif
#endif
    MAX_NAME_LEN,                                           // Maximum
    ParVal_SWBuildDate,                                 // Parameter
    NULL                                                    // Behandlungsfunktion
};

// Parameter-Variable SWToolVersion
// tool chain version, automatisch generiert
//#pragma DATA_SECTION(ParVal_SWToolVersion,".extram");
UINT8 ParVal_SWToolVersion[MAX_NAME_LEN];

// SWToolVersion
// Parameter-Beschreibung SWToolVersion
PAR_DES_A_UINT8_T ParDes_SWToolVersion =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
#ifdef LANGUAGE_GERMAN
    "software tool version",                                            // Name
#else
#ifdef LANGUAGE_ENGLISH
    "software tool version",                // Name
#endif
#endif
    MAX_NAME_LEN,                                           // Maximum
    ParVal_SWToolVersion,                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

#else /* STRING_PARAMS */

// Parameter-Variable Geräte-Identifikation
//#pragma DATA_SECTION(ParVal_DeviceIdent,".extram");
//WORD ParVal_DeviceIdent;

// Gerätetyp
// Parameter-Beschreibung DeviceIdent
PAR_DES_WORD_T ParDes_DeviceIdent =
{
    PAR_MENUE | PAR_RO | _WORD,                                     // ReadOnly, WORD-Parameter
#ifdef LANGUAGE_GERMAN
    "ISUVOC",                                           // Name
#else
#ifdef LANGUAGE_ENGLISH
    "ISUVOC",               // Name
#endif
#endif
    0xFFFF,                                                     // Maximum
    0x0000,                                                     // Minimum
    DEVICE_TYPE,                                            // Default
    NULL/*ParVal_DeviceIdent*/,                         // Parameter
    NULL                                                            // Behandlungsfunktion
};

// Parameter-Variable HWVersion
// hardware version
//#pragma DATA_SECTION(ParVal_HWVersion,".extram");
//UINT16 ParVal_HWVersion;

// Parameter-Beschreibung HWVersion
PAR_DES_U16_T ParDes_HWVersion =
{
    PAR_MENUE | PAR_RO | _UINT16,                                   // ReadOnly, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "Version",                              // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Version",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    0xFFFF,                                                     // Maximum
    0x0000,                                                     // Minimum
    HW_VERSION,                                             // Default
    1.0,                                                            // Faktor
    NULL/*ParVal_HWVersion*/,                               // Parameter
    NULL                                                            // Behandlungsfunktion
};



// Parameter-Variable SWVersion
// software version
//#pragma DATA_SECTION(ParVal_SWVersion,".extram");
UINT16 ParVal_SWVersion;

// Parameter-Beschreibung SWVersion
PAR_DES_U16_T ParDes_SWVersion =
{
    PAR_STORE | PAR_RO | _UINT16,                                   // ReadOnly, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "software version",                             // Name
#else
#ifdef LANGUAGE_ENGLISH
    "software version",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    0xFFFF,                                                     // Maximum
    0x0000,                                                     // Minimum
    SW_VERSION,                                             // Default
    1.0,                                                            // Faktor
    &ParVal_SWVersion,                              // Parameter
    NULL                                                            // Behandlungsfunktion
};



// Parameter-Variable SWBuildNum
// build nummer, automatisch generiert
//#pragma DATA_SECTION(ParVal_SWBuildNum,".extram");
UINT16 ParVal_SWBuildNum;

// Parameter-Beschreibung SWBuildNum
PAR_DES_U16_T ParDes_SWBuildNum =
{
    PAR_STORE | PAR_RO | _UINT16,                                   // ReadOnly, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "software build number",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "software build number",                    // Name
#endif
#endif
    NULL,                                                           // Einheit
    0xFFFF,                                                     // Maximum
    0x0000,                                                     // Minimum
    SW_BUILD_NUM,                                           // Default
    1.0,                                                            // Faktor
    &ParVal_SWBuildNum,                             // Parameter
    NULL                                                            // Behandlungsfunktion
};



// Parameter-Variable SWBuildDate
// build date, automatisch generiert
//#pragma DATA_SECTION(ParVal_SWBuildDate,".extram");
UINT32 ParVal_SWBuildDate;

// Parameter-Beschreibung SWBuildDate
PAR_DES_U32_T ParDes_SWBuildDate =
{
    PAR_STORE | PAR_RO | _UINT32,                                   // ReadOnly, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "software build date",                      // Name
#else
#ifdef LANGUAGE_ENGLISH
    "software build date",                      // Name
#endif
#endif
    NULL,                                                           // Einheit
    0xFFFFFFFF,                                                     // Maximum
    0x00000000,                                                     // Minimum
    SW_BUILD_DATE,                                      // Default
    1.0,                                                            // Faktor
    &ParVal_SWBuildDate,                            // Parameter
    NULL                                                            // Behandlungsfunktion
};

// Parameter-Variable SWToolVersion
// tool chain version, automatisch generiert
//#pragma DATA_SECTION(ParVal_SWToolVersion,".extram");
UINT16 ParVal_SWToolVersion;

// Parameter-Beschreibung SWToolVersion
PAR_DES_U16_T ParDes_SWToolVersion =
{

     PAR_STORE | PAR_RO | _UINT16,                                  // ReadOnly, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "software tool version",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "software tool version",                    // Name
#endif
#endif
    NULL,                                                           // Einheit
    0xFFFF,                                                     // Maximum
    0x0000,                                                     // Minimum
    SW_TOOL_VERSION,                                    // Default
    1.0,                                                            // Faktor
    &ParVal_SWToolVersion,                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

#endif /* STRING_PARAMS */


// Parameter-Variable Passcode
//#pragma DATA_SECTION(ParVal_Passcode,".extram");
UINT16 ParVal_Passcode;

PAR_DES_U16_T ParDes_Passcode =
{
    PAR_RW | _UINT16,
#ifdef LANGUAGE_GERMAN
    "Passcode",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Passcode",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    65535,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_Passcode,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};



// Parameter-Variable OptimeHours
//#pragma DATA_SECTION(ParVal_OptimeHours,".extram");
UINT16 ParVal_OptimeHours;

// Parameter-Beschreibung OptimeHours
PAR_DES_U16_T ParDes_OptimeHours =
{

     PAR_STORE | PAR_STORE_EE | PAR_RO | _UINT16,                                   // ReadOnly, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "Betriebszeit h",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Operation time h",                 // Name
#endif
#endif
    "h",                                                            // Einheit
    0xFFFF,                                                     // Maximum
    0x0000,                                                     // Minimum
    0,                                  // Default
    1.0,                                                            // Faktor
    &ParVal_OptimeHours,                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable OptimeSeconds
//#pragma DATA_SECTION(ParVal_OptimeSeconds,".extram");
UINT16 ParVal_OptimeSeconds;

// Parameter-Beschreibung OptimeSeconds
PAR_DES_U16_T ParDes_OptimeSeconds =
{

     PAR_STORE | PAR_STORE_EE | PAR_RO | _UINT16,                                   // ReadOnly, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "Betriebszeit s",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Operation time s",                 // Name
#endif
#endif
    "s",                                                            // Einheit
    0xFFFF,                                                     // Maximum
    0x0000,                                                     // Minimum
    0,                                  // Default
    1.0,                                                            // Faktor
    &ParVal_OptimeSeconds,                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable OptimeWrite
//#pragma DATA_SECTION(ParVal_OptimeWrite,".extram");
UINT32 ParVal_OptimeWrite;

// Parameter-Beschreibung OptimeWrite
PAR_DES_U32_T ParDes_OptimeWrite =
{

     PAR_RW | _UINT32,                                  // ReadOnly, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "Betriebszeit setzen",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Set operation time",                   // Name
#endif
#endif
    "s",                                                            // Einheit
    0xFFFFFFFF,                                                     // Maximum
    0x00000000,                                                     // Minimum
    0,                                  // Default
    1.0,                                                            // Faktor
    &ParVal_OptimeWrite,                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Overload,".extram");
//INT16 ParVal_Overload;

PAR_DES_I16_T ParDes_Overload =
{
    PAR_PASS_PW | PAR_MENUE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Überlast",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Overload",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_Overload*/,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_F_ov,".extram");
INT16 ParVal_F_ov;

PAR_DES_I16_T ParDes_F_ov =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Überlastfaktor",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Overload ratio",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    10000,                                              // Maximum
    1000,                                                               // Minimum
    1000,                                                               // Default
    0.001,                    // Faktor
    &ParVal_F_ov,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Itrip,".extram");
INT16 ParVal_Itrip;

PAR_DES_I16_T ParDes_Itrip =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Überstromschwelle",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Current trip level",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    10000,                                              // Maximum
    1000,                                                               // Minimum
    1250,                                                               // Default
    0.001,                    // Faktor
    &ParVal_Itrip,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


INT16 ParVal_ItripAsym;

PAR_DES_I16_T ParDes_ItripAsym =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Max. Stromasymmetrie",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Max. current asymmetry",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    10000,                                              // Maximum
    1000,                                                               // Minimum
    1100,                                                               // Default
    0.001,                    // Faktor
    &ParVal_ItripAsym,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Toverload,".extram");
INT16 ParVal_Toverload;

PAR_DES_I16_T ParDes_Toverload =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Überlastzeit",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Overload time",                                // Name
#endif
#endif
    "s",                                                            // Einheit
    30000,                                              // Maximum
    1,                                                              // Minimum
    30,                                                             // Default
    1.0,                      // Faktor
    &ParVal_Toverload,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Tpause,".extram");
INT16 ParVal_Tpause;

PAR_DES_I16_T ParDes_Tpause =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Pausenzeit",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Resting time",                             // Name
#endif
#endif
    "s",                                                            // Einheit
    30000,                                              // Maximum
    1,                                                              // Minimum
    30,                                                             // Default
    1.0,                      // Faktor
    &ParVal_Tpause,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Grid
//#pragma DATA_SECTION(ParVal_Grid,".extram");
//INT16 ParVal_Grid;

// Netz
// Parameter-Beschreibung Grid
PAR_DES_I16_T ParDes_Grid =
{
    PAR_MENUE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Netz",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Grid",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_Grid*/,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable VNline
//#pragma DATA_SECTION(ParVal_VNline,".extram");
INT16 ParVal_VNline;

// Netz-Nennsspannung (L-L), in V
// Parameter-Beschreibung VNline
PAR_DES_I16_T ParDes_VNline =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Netz-Nennspannung",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Rated line voltage",                               // Name
#endif
#endif
    "V",                                                            // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    400,                                                                // Default
    1.0,                      // Faktor
    &ParVal_VNline,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable TrafoRatio
//#pragma DATA_SECTION(ParVal_TrafoRatio,".extram");
INT16 ParVal_TrafoRatio;

PAR_DES_I16_T ParDes_TrafoRatio =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Trafo Übersetzung",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Transformer ratio",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    30000,                                              // Maximum
    10,                                                             // Minimum
    10,                                                             // Default
    0.1,                      // Faktor
    &ParVal_TrafoRatio,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable TrafoPhase
//#pragma DATA_SECTION(ParVal_TrafoPhase,".extram");
INT16 ParVal_TrafoPhase;

PAR_DES_I16_T ParDes_TrafoPhase =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Trafo Phase",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Transformer phase shift",                              // Name
#endif
#endif
    "deg",                                                          // Einheit
    360,                                                // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_TrafoPhase,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable PhiVmes
//#pragma DATA_SECTION(ParVal_PhiVmes,".extram");
INT16 ParVal_PhiVmes;

PAR_DES_I16_T ParDes_PhiVmes =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Umes Phase",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Vmes phase shift",                             // Name
#endif
#endif
    "deg",                                                          // Einheit
    1800,                                               // Maximum
    -1800,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_PhiVmes,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable PhiImes
//#pragma DATA_SECTION(ParVal_PhiImes,".extram");
INT16 ParVal_PhiImes;

PAR_DES_I16_T ParDes_PhiImes =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Imes Phase",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Imes phase shift",                             // Name
#endif
#endif
    "deg",                                                          // Einheit
    1800,                                               // Maximum
    -1800,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_PhiImes,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable RangeVL
//#pragma DATA_SECTION(ParVal_RangeVL,".extram");
INT16 ParVal_RangeVL;

PAR_DES_I16_T ParDes_RangeVL =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Messbereich Netzspannung",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Line voltage range",                               // Name
#endif
#endif
    "V",                                                            // Einheit
    4000,                                               // Maximum
    -4000,                                                              // Minimum
    1200,                                                               // Default
    1.0,                      // Faktor
    &ParVal_RangeVL,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable RangeVdc
//#pragma DATA_SECTION(ParVal_RangeVdc,".extram");
INT16 ParVal_RangeVdc;

PAR_DES_I16_T ParDes_RangeVdc =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Messbereich Udc",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "DC voltage range",                             // Name
#endif
#endif
    "V",                                                            // Einheit
    4000,                                               // Maximum
    0,                                                              // Minimum
    1000,                                                               // Default
    1.0,                      // Faktor
    &ParVal_RangeVdc,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};



// Parameter-Variable FNline
//#pragma DATA_SECTION(ParVal_FNline,".extram");
INT16 ParVal_FNline;

// Netz-Nennfrequenz, in Hz
// Parameter-Beschreibung FNline
PAR_DES_I16_T ParDes_FNline =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Netz-Nennfrequenz",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Rated line frequency",                             // Name
#endif
#endif
    "Hz",                                                           // Einheit
    100,                                                // Maximum
    0,                                                              // Minimum
    50,                                                             // Default
    1.0,                      // Faktor
    &ParVal_FNline,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Inverter
//#pragma DATA_SECTION(ParVal_Inverter,".extram");
//INT16 ParVal_Inverter;

// Umrichter
// Parameter-Beschreibung Inverter
PAR_DES_I16_T ParDes_Inverter =
{
    PAR_MENUE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Umrichter",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Inverter",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_Inverter*/,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable INinv
//#pragma DATA_SECTION(ParVal_INinv,".extram");
INT32 ParVal_INinv;

// Umrichter-Nennstrom, in A
// Parameter-Beschreibung INinv
PAR_DES_I32_T ParDes_INinv =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,
#ifdef LANGUAGE_GERMAN
    "Umrichter-Nennstrom",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Rated inverter current",                               // Name
#endif
#endif
    "A",                                                            // Einheit
    300000,                                             // Maximum
    0,                                                              // Minimum
    2150,                                                               // Default
    0.1,                      // Faktor
    &ParVal_INinv,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable I_StandbyOn
//#pragma DATA_SECTION(ParVal_I_StandbyOn,".extram");
INT16 ParVal_I_StandbyOn;

PAR_DES_I16_T ParDes_I_StandbyOn =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Standby-Ein Laststrom",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Standby ON load current",                              // Name
#endif
#endif
    "A",                                                            // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_I_StandbyOn,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable I_StandbyOff
//#pragma DATA_SECTION(ParVal_I_StandbyOff,".extram");
INT16 ParVal_I_StandbyOff;

PAR_DES_I16_T ParDes_I_StandbyOff =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Standby-Aus Laststrom",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Standby OFF load current",                             // Name
#endif
#endif
    "A",                                                            // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_I_StandbyOff,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable RangeI
//#pragma DATA_SECTION(ParVal_RangeI,".extram");
INT32 ParVal_RangeI;

// Messbereich Umrichterstrom, in A
// Parameter-Beschreibung RangeI
PAR_DES_I32_T ParDes_RangeI =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,
#ifdef LANGUAGE_GERMAN
    "Messbereich Umrichterstrom",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Inverter curent range",                                // Name
#endif
#endif
    "A",                                                            // Einheit
    100000,                                             // Maximum
    -100000,                                                                // Minimum
    5000,                                                               // Default
    0.1,                      // Faktor
    &ParVal_RangeI,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable RangeIL
//#pragma DATA_SECTION(ParVal_RangeIL,".extram");
INT32 ParVal_RangeIL;

// Messbereich Netzstrom, in A
// Parameter-Beschreibung RangeIL
PAR_DES_I32_T ParDes_RangeIL =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,
#ifdef LANGUAGE_GERMAN
    "Messbereich Statorstrom",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Stator curent range",                              // Name
#endif
#endif
    "A",                                                            // Einheit
    200000,                                             // Maximum
    -200000,                                                                // Minimum
    12200,                                                              // Default
    0.1,                      // Faktor
    &ParVal_RangeIL,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Ldr
//#pragma DATA_SECTION(ParVal_Ldr,".extram");
INT16 ParVal_Ldr;

// Line choke inductance, in myH
// Parameter-Beschreibung Ldr
PAR_DES_I16_T ParDes_Ldr =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Induktivität Netztrafo",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Trafo inductance",                             // Name
#endif
#endif
    "myH",                                                          // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    508,                                                                // Default
    1.0,                      // Faktor
    &ParVal_Ldr,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Ldr_trans
//#pragma DATA_SECTION(ParVal_Ldr_trans,".extram");
INT16 ParVal_Ldr_trans;

// Line choke transient inductance, in myH
// Parameter-Beschreibung Ldr_trans
PAR_DES_I16_T ParDes_Ldr_trans =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Induktivität Netzdrossel",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Line choke inductance",                                // Name
#endif
#endif
    "myH",                                                          // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    508,                                                                // Default
    1.0,                      // Faktor
    &ParVal_Ldr_trans,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Rdr
//#pragma DATA_SECTION(ParVal_Rdr,".extram");
INT16 ParVal_Rdr;

// Line choke resistance, in mOhm
// Parameter-Beschreibung Rdr
PAR_DES_I16_T ParDes_Rdr =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Widerstand Netzdrossel",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Line choke resistance",                                // Name
#endif
#endif
    "mOhm",                                                         // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    10,                                                             // Default
    1.0,                      // Faktor
    &ParVal_Rdr,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable DC_link
//#pragma DATA_SECTION(ParVal_DC_link,".extram");
//INT16 ParVal_DC_link;

// Zwischenkreis
// Parameter-Beschreibung DC_link
PAR_DES_I16_T ParDes_DC_link =
{
    PAR_MENUE | PAR_RW | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Zwischenkreis",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "DC link",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_DC_link*/,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable VdcN
//#pragma DATA_SECTION(ParVal_VdcN,".extram");
INT16 ParVal_VdcN;

// Zwischenkreis-Nennsspannung, in V
// Parameter-Beschreibung VdcN
PAR_DES_I16_T ParDes_VdcN =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Zwischenkreis-Nennspannung",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Rated DC link voltage",                                // Name
#endif
#endif
    "V",                                                            // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    600,                                                                // Default
    1.0,                      // Faktor
    &ParVal_VdcN,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Vdcmax
//#pragma DATA_SECTION(ParVal_Vdcmax,".extram");
INT16 ParVal_Vdcmax;

// Zwischenkreis-Maxumalspannung, in V
// Parameter-Beschreibung Vdcmax
PAR_DES_I16_T ParDes_Vdcmax =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Max. Zwischenkreisspannung",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Max. DC link voltage",                             // Name
#endif
#endif
    "V",                                                            // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    650,                                                                // Default
    1.0,                      // Faktor
    &ParVal_Vdcmax,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Vdc_tripp
//#pragma DATA_SECTION(ParVal_Vdc_tripp,".extram");
INT16 ParVal_Vdc_tripp;

// Zwischenkreis-Triplevel+, in V
// Parameter-Beschreibung Vdc_tripp
PAR_DES_I16_T ParDes_Vdc_tripp =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Zwischenkreis-Tripspannung +",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "DC link trip voltage +",                               // Name
#endif
#endif
    "V",                                                            // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    730,                                                                // Default
    1.0,                      // Faktor
    &ParVal_Vdc_tripp,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Vdc_tripm
//#pragma DATA_SECTION(ParVal_Vdc_tripm,".extram");
INT16 ParVal_Vdc_tripm;

// Zwischenkreis-Triplevel-, in V
// Parameter-Beschreibung Vdc_tripm
PAR_DES_I16_T ParDes_Vdc_tripm =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Zwischenkreis-Tripspannung -",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "DC link trip voltage -",                               // Name
#endif
#endif
    "V",                                                            // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    250,                                                                // Default
    1.0,                      // Faktor
    &ParVal_Vdc_tripm,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


INT16 ParVal_Vdc_min_on;

PAR_DES_I16_T ParDes_Vdc_min_on =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "min. V_DC start",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Vdc min ON",                               // Name
#endif
#endif
    "V",                                                            // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    500,                                                                // Default
    1.0,                      // Faktor
    &ParVal_Vdc_min_on,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Cdc
//#pragma DATA_SECTION(ParVal_Cdc,".extram");
INT32 ParVal_Cdc;

// Zwischenkreis-Kapazität, in myF
// Parameter-Beschreibung Cdc
PAR_DES_I32_T ParDes_Cdc =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Zwischenkreiskapazität",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "DC link capacitance",                              // Name
#endif
#endif
    "myF",                                                          // Einheit
    500000,                                             // Maximum
    0,                                                              // Minimum
    5600,                                                               // Default
    1.0,                      // Faktor
    &ParVal_Cdc,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Cfil
//#pragma DATA_SECTION(ParVal_Cfil,".extram");
INT16 ParVal_Cfil;

// Filter-Kapazität, in myF
// Parameter-Beschreibung Cfil
PAR_DES_I16_T ParDes_Cfil =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Filterkreiskapazität",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Line filter capacitance",                              // Name
#endif
#endif
    "myF",                                                          // Einheit
    32767,                                              // Maximum
    0,                                                              // Minimum
    5600,                                                               // Default
    1.0,                      // Faktor
    &ParVal_Cfil,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable StatusWord
//#pragma DATA_SECTION(ParVal_StatusWord,".extram");
WORD ParVal_StatusWord = 0;

// Parameter-Beschreibung StatusWord
PAR_DES_WORD_T ParDes_StatusWord =
{
    PAR_RO | _WORD,                                     // ReadOnly, WORD-Parameter
#ifdef LANGUAGE_GERMAN
    "Statuswort",                                           // Name
#else
#ifdef LANGUAGE_ENGLISH
    "statusword",                                           // Name
#endif
#endif
    0xFFFF,                                                     // Maximum
    0x0000,                                                     // Minimum
    0x0000,                                                     // Default
    &ParVal_StatusWord,                             // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable ErrorWord
//#pragma DATA_SECTION(ParVal_ErrorWord,".extram");
WORD ParVal_ErrorWord;
// Parameter-Beschreibung ErrorWord
PAR_DES_WORD_T ParDes_ErrorWord =
{
    PAR_RO | _WORD,                                     // ReadOnly, WORD-Parameter
#ifdef LANGUAGE_GERMAN
    "Fehlerwort",                                           // Name
#else
#ifdef LANGUAGE_ENGLISH
    "errorword",                                            // Name
#endif
#endif
    0xFFFF,                                                     // Maximum
    0x0000,                                                     // Minimum
    0x0000,                                                     // Default
    &ParVal_ErrorWord,                              // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable NVmemContro
//#pragma DATA_SECTION(ParVal_NVmemControl,".extram");
//WORD ParVal_NVmemControl;


//! Stati für Parameter ParVal_NVmemControl
typedef enum
{
    PAR_NOTHING = 0,                                    //!< 0, nichts
    PAR_SAVE = 1,                                           //!< 1, speichern
    PAR_DEFAULT = 2,                                    //!< 2, defaultwerte laden
    PAR_RECALL = 3,                                     //!< 3, laden
    PAR_SAVE_ALL = 4                                            //!< 1, speichern
    //PAR_READ_FRICTION_TABLE = 4,          //!< 4, Reibwert-Tabelle aus NVmem laden
    //PAR_WRITE_FRICTION_TABLE = 5          //!< 5, Reibwert-Tabelle im NVmem speichern
#ifdef SPIE2P_TEST
    ,
    SPIE2P_TEST_WRITE,                      //!< 5, Test: Schreiben zum EEPROM
    SPIE2P_TEST_READ                            //!< 6, Test: Lesen vom EEPROM
#endif
} PAR_NV_CTRL_T;

uint16_T ParVal_NVmemControl;          /* '<Root>/P_Vline_tripp66' */

// Parameter-Beschreibung NVmemContro
PAR_DES_WORD_T ParDes_NVmemControl =
{
    PAR_RW | _WORD,                                     // ReadOnly, WORD-Parameter
#ifdef LANGUAGE_GERMAN
    "Steuerung nv-memory",                      // Name
#else
#ifdef LANGUAGE_ENGLISH
    "control of nv memory",                     // Name
#endif
#endif
    PAR_SAVE_ALL,                   // Maximum
    PAR_NOTHING,                                            // Minimum
    PAR_NOTHING,                                            // Default
    &ParVal_NVmemControl,                           // Parameter
    NULL/*WriteParVal_NVmemControl*/                // Behandlungsfunktion
};

uint16_T ParVal_NVmemControl1;         /* '<Root>/P_Vline_tripp91' */

PAR_DES_WORD_T ParDes_NVmemControl1 =
{
    PAR_RW | _WORD,                                     // ReadOnly, WORD-Parameter
#ifdef LANGUAGE_GERMAN
    "Steuerung nv-memory 1",                        // Name
#else
#ifdef LANGUAGE_ENGLISH
    "control of nv memory 1",                       // Name
#endif
#endif
    PAR_SAVE_ALL,                   // Maximum
    PAR_NOTHING,                                            // Minimum
    PAR_NOTHING,                                            // Default
    &ParVal_NVmemControl1,                          // Parameter
    NULL/*WriteParVal_NVmemControl*/                // Behandlungsfunktion
};


// Parameter-Variable TSTrigLevel               //RS
//#pragma DATA_SECTION(ParVal_TSTrigLevel,".extram");
INT32 ParVal_TSTrigLevel;

// Parameter-Beschreibung TSTrigLevel
PAR_DES_I32_T ParDes_TSTrigLevel =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                             // ReadWrite, INT32-Parameter
#ifdef LANGUAGE_GERMAN
    "TSP Trigger Pegel",                        // Name
#else
#ifdef LANGUAGE_ENGLISH
    "TM trigger level",                     // Name
#endif
#endif
    NULL,                                                       // Einheit
    MAX_LONG,                                               // Maximum
    -MAX_LONG,                                          // Minimum
    1,                                                          // Default
    1.0,                                                        // Faktor
    &ParVal_TSTrigLevel,                        // Parameter
    NULL                                                // Behandlungsfunktion
};


// Parameter-Variable TSStatus              //RS
//#pragma DATA_SECTION(ParVal_TSStatus,".extram");
WORD ParVal_TSStatus;

// Parameter-Beschreibung TSStatus
PAR_DES_WORD_T ParDes_TSStatus =
{
    PAR_RO | _WORD,                                     // ReadWrite, WORD-Parameter
#ifdef LANGUAGE_GERMAN
    "TSP Statuswort",                       // Name
#else
#ifdef LANGUAGE_ENGLISH
    "TM statusword",                        // Name
#endif
#endif
    0x00ff,                                                 // Maximum
    0,                                                          // Minimum
    0,                                                          // Default
    &ParVal_TSStatus,                           // Parameter
    NULL                                                        // Behandlungsfunktion
};

// Parameter-Variable TSControl             //RS
//#pragma DATA_SECTION(ParVal_TSControl,".extram");
WORD ParVal_TSControl;

RC_PAR_T WriteParVal_ControlTSP(WORD ControlWord)
{
  return(RC_PARA_OK);            // must return FALSE
}

// Parameter-Beschreibung TSControl
PAR_DES_WORD_T ParDes_TSControl =
{
    PAR_RW | _WORD,                                     // ReadWrite, WORD-Parameter
#ifdef LANGUAGE_GERMAN
    "TSP Steuerwort",       // Name
#else
#ifdef LANGUAGE_ENGLISH
    "TM controlword",       // Name
#endif
#endif
    0xffff,     // Maximum
    0,                      // Minimum
    0,                      // Default
    &ParVal_TSControl,      // Parameter
    WriteParVal_ControlTSP  // Behandlungsfunktion
};



// Parameter-Variable TSTrigCondition               //RS
//#pragma DATA_SECTION(ParVal_TSTrigCondition,".extram");
WORD ParVal_TSTrigCondition;

// Parameter-Beschreibung TSTrigCondition
PAR_DES_WORD_T ParDes_TSTrigCondition =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,                                      // ReadWrite, WORD-Parameter
#ifdef LANGUAGE_GERMAN
    "TSP Trigger Bedingung",                        // Name
#else
#ifdef LANGUAGE_ENGLISH
    "TM trigger condition",                     // Name
#endif
#endif
    3,                                                  // Maximum
    0,                                                  // Minimum
    0,                                                  // Default
    &ParVal_TSTrigCondition,                                // Parameter
    NULL                                                            // Behandlungsfunktion
};

// Parameter-Variable TSSamplStepWidthFactor                //RS
//#pragma DATA_SECTION(ParVal_TSSamplStepWidthFactor,".extram");
UINT16 ParVal_TSSamplStepWidthFactor;

// Parameter-Beschreibung TSSamplStepWidthFactor
PAR_DES_U16_T ParDes_TSSamplStepWidthFactor =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _UINT16,                                        // ReadWrite, INT16-Parameter
#ifdef LANGUAGE_GERMAN
    "TSP Abtastintervall",                      // Name
#else
#ifdef LANGUAGE_ENGLISH
    "TM sampling factor",                       // Name
#endif
#endif
    NULL,                                               // Einheit
    MAX_UINT,                                                   // Maximum
    1,                                                          // Minimum
    1,                                                          // Default
    1.0,                                                        // Faktor
    &ParVal_TSSamplStepWidthFactor,                                 // Parameter
    NULL                                                            // Behandlungsfunktion
};

// Parameter-Variable TSChannelNum              //RS
//#pragma DATA_SECTION(ParVal_TSChannelNum,".extram");
UINT16 ParVal_TSChannelNum;
#define TSP_CHANN_ANZAHL        (1*8)           // maximal mögliche Anzahl von Kanälen, only 8 or 16 are allowed!!!

RC_PAR_T WriteParVal_ChannelNumTSP(UINT16 ChannelNum)
{
  return (RC_PARA_OK);
}


// Parameter-Beschreibung TSChannelNum
PAR_DES_U16_T ParDes_TSChannelNum =
{
    PAR_RW | _UINT16,                                       // ReadWrite, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "TSP Kanalanzahl",          // Name
#else
#ifdef LANGUAGE_ENGLISH
    "TM number of channels",    // Name
#endif
#endif
    NULL,                       // Einheit
    8,                          // Maximum
    1,                          // Minimum
    8,                          // Default
    1.0,                        // Faktor
    &ParVal_TSChannelNum,       // Parameter
    WriteParVal_ChannelNumTSP   // Behandlungsfunktion
};


// Parameter-Variable TSDelayNum                //RS
//#pragma DATA_SECTION(ParVal_TSDelayNum,".extram");
INT16 ParVal_TSDelayNum;

// Parameter-Beschreibung TSDelayNum
PAR_DES_I16_T ParDes_TSDelayNum =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                     // ReadWrite, INT16-Parameter
#ifdef LANGUAGE_GERMAN
    "TSP Trigger Verzögerung",                      // Name
#else
#ifdef LANGUAGE_ENGLISH
    "TM trigger delay",                     // Name
#endif
#endif
    "%",                                            // Einheit
    100,                                            // Maximum
    0,                                              // Minimum
    10,                                             // Default
    1.0,                                            // Faktor
    &ParVal_TSDelayNum,                             // Parameter
    NULL                                                            // Behandlungsfunktion
};

// Parameter-Variable TSBaseTime                //RS
//#pragma DATA_SECTION(ParVal_TSBaseTime,".extram");
UINT16 ParVal_TSBaseTime;

// Parameter-Beschreibung TSBaseTime
PAR_DES_U16_T ParDes_TSBaseTime =
{
    PAR_RO | _UINT16,                                       // Readonly, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "TSP Zeitbasis",                        // Name
#else
#ifdef LANGUAGE_ENGLISH
    "TM time base",                     // Name
#endif
#endif
    "µs",                                                           // Einheit
    MAX_UINT,                                                   // Maximum
    0,                                                      // Minimum
    200,                                                        // Default
    1.0,                                                        // Faktor
    &ParVal_TSBaseTime,                                 // Parameter
    NULL                                                            // Behandlungsfunktion
};



// Parameter-Variable TSChannel0Par             //RS
//#pragma DATA_SECTION(ParVal_TSChannel0Par,".extram");
UINT16 ParVal_TSChannel0Par;

#define LEAST_PAR_INDEX     1           //RS


// Parameter-Beschreibung TSChannel0Par
PAR_DES_U16_T ParDes_TSChannel0Par =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _UINT16,                                        // ReadWrite, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "TSP (Trigger)Kanal 1 Parameter Nr.",                       // Name
#else
#ifdef LANGUAGE_ENGLISH
        "TM (trigger)channel 1 param. #",                       // Name
#endif
#endif
    NULL,                                                           // Einheit
    LAST_PAR_INDEX,                                                 // Maximum
    LEAST_PAR_INDEX,                                                // Minimum
    131,                                                // Default
    1.0,                                                        // Faktor
    &ParVal_TSChannel0Par,                              // Parameter
    NULL                                                            // Behandlungsfunktion
};

// Parameter-Variable TSChannel1Par             //RS
//#pragma DATA_SECTION(ParVal_TSChannel1Par,".extram");
UINT16 ParVal_TSChannel1Par;

// Parameter-Beschreibung TSChannel1Par
PAR_DES_U16_T ParDes_TSChannel1Par =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _UINT16,                                        // ReadWrite, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "TSP Kanal 2 Parameter Nr.",                        // Name
#else
#ifdef LANGUAGE_ENGLISH
        "TM channel 2 param. #",                        // Name
#endif
#endif
    NULL,                                                           // Einheit
    LAST_PAR_INDEX,                                                 // Maximum
    LEAST_PAR_INDEX,                                                // Minimum
    132,                                                // Default
    1.0,                                                            // Faktor
    &ParVal_TSChannel1Par,                              // Parameter
    NULL                                                            // Behandlungsfunktion
};

// Parameter-Variable TSChannel2Par             //RS
//#pragma DATA_SECTION(ParVal_TSChannel2Par,".extram");
UINT16 ParVal_TSChannel2Par;

// Parameter-Beschreibung TSChannel2Par
PAR_DES_U16_T ParDes_TSChannel2Par =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _UINT16,                                        // ReadWrite, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "TSP Kanal 3 Parameter Nr.",                        // Name
#else
#ifdef LANGUAGE_ENGLISH
        "TM channel 3 param. #",                        // Name
#endif
#endif
    NULL,                                                           // Einheit
    LAST_PAR_INDEX,                                                 // Maximum
    LEAST_PAR_INDEX,                                                // Minimum
    133,                                                // Default
    1.0,                                                    // Faktor
    &ParVal_TSChannel2Par,                              // Parameter
    NULL                                                            // Behandlungsfunktion
};

// Parameter-Variable TSChannel3Par             //RS
//#pragma DATA_SECTION(ParVal_TSChannel3Par,".extram");
UINT16 ParVal_TSChannel3Par;

// Parameter-Beschreibung TSChannel3Par
PAR_DES_U16_T ParDes_TSChannel3Par =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _UINT16,                                        // ReadWrite, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "TSP Kanal 4 Parameter Nr.",                        // Name
#else
#ifdef LANGUAGE_ENGLISH
        "TM channel 4 param. #",                        // Name
#endif
#endif
    NULL,                                                           // Einheit
    LAST_PAR_INDEX,                                                 // Maximum
    LEAST_PAR_INDEX,                                                // Minimum
    134,                                                // Default
    1.0,                                                            // Faktor
    &ParVal_TSChannel3Par,                              // Parameter
    NULL                                                            // Behandlungsfunktion
};

// Parameter-Variable TSChannel4Par             //RS
//#pragma DATA_SECTION(ParVal_TSChannel4Par,".extram");
UINT16 ParVal_TSChannel4Par;

// Parameter-Beschreibung TSChannel4Par
PAR_DES_U16_T ParDes_TSChannel4Par =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _UINT16,                                        // ReadWrite, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "TSP Kanal 5 Parameter Nr.",                        // Name
#else
#ifdef LANGUAGE_ENGLISH
        "TM channel 5 param. #",                        // Name
#endif
#endif
    NULL,                                                           // Einheit
    LAST_PAR_INDEX,                                                 // Maximum
    LEAST_PAR_INDEX,                                                // Minimum
    135,                                                // Default
    1.0,                                                            // Faktor
    &ParVal_TSChannel4Par,                              // Parameter
    NULL                                                            // Behandlungsfunktion
};

// Parameter-Variable TSChannel5Par             //RS
//#pragma DATA_SECTION(ParVal_TSChannel5Par,".extram");
UINT16 ParVal_TSChannel5Par;

// Parameter-Beschreibung TSChannel5Par
PAR_DES_U16_T ParDes_TSChannel5Par =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _UINT16,                                        // ReadWrite, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "TSP Kanal 6 Parameter Nr.",                        // Name
#else
#ifdef LANGUAGE_ENGLISH
        "TM channel 6 param. #",                        // Name
#endif
#endif
    NULL,                                                           // Einheit
    LAST_PAR_INDEX,                                                 // Maximum
    LEAST_PAR_INDEX,                                                // Minimum
    136,                                                // Default
    1.0,                                                    // Faktor
    &ParVal_TSChannel5Par,                              // Parameter
    NULL                                                            // Behandlungsfunktion
};

// Parameter-Variable TSChannel6Par             //RS
//#pragma DATA_SECTION(ParVal_TSChannel6Par,".extram");
UINT16 ParVal_TSChannel6Par;

// Parameter-Beschreibung TSChannel6Par
PAR_DES_U16_T ParDes_TSChannel6Par =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _UINT16,                                        // ReadWrite, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "TSP Kanal 7 Parameter Nr.",                        // Name
#else
#ifdef LANGUAGE_ENGLISH
        "TM channel 7 param. #",                        // Name
#endif
#endif
    NULL,                                                           // Einheit
    LAST_PAR_INDEX,                                                 // Maximum
    LEAST_PAR_INDEX,                                                // Minimum
    137,                                                // Default
    1.0,                                                            // Faktor
    &ParVal_TSChannel6Par,                              // Parameter
    NULL                                                            // Behandlungsfunktion
};

// Parameter-Variable TSChannel7Par             //RS
//#pragma DATA_SECTION(ParVal_TSChannel7Par,".extram");
UINT16 ParVal_TSChannel7Par;

// Parameter-Beschreibung TSChannel7Par
PAR_DES_U16_T ParDes_TSChannel7Par =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _UINT16,                                        // ReadWrite, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "TSP Kanal 8 Parameter Nr.",                        // Name
#else
#ifdef LANGUAGE_ENGLISH
        "TM channel 8 param. #",                        // Name
#endif
#endif
    NULL,                                                           // Einheit
    LAST_PAR_INDEX,                                                 // Maximum
    LEAST_PAR_INDEX,                                                // Minimum
    138,                                                // Default
    1.0,                                                            // Faktor
    &ParVal_TSChannel7Par,                              // Parameter
    NULL                                                            // Behandlungsfunktion
};


UINT16 ParVal_TSChannel8Par;
PAR_DES_U16_T ParDes_TSChannel8Par =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _UINT16,                                        // ReadWrite, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "TSP Kanal 9 Parameter Nr.",                        // Name
#else
#ifdef LANGUAGE_ENGLISH
        "TM channel 9 param. #",                        // Name
#endif
#endif
    NULL,                                                           // Einheit
    LAST_PAR_INDEX,                                                 // Maximum
    LEAST_PAR_INDEX,                                                // Minimum
    138,                                                // Default
    1.0,                                                            // Faktor
    &ParVal_TSChannel8Par,                              // Parameter
    NULL                                                            // Behandlungsfunktion
};


UINT16 ParVal_TSChannel9Par;
PAR_DES_U16_T ParDes_TSChannel9Par =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _UINT16,                                        // ReadWrite, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "TSP Kanal 10 Parameter Nr.",                       // Name
#else
#ifdef LANGUAGE_ENGLISH
        "TM channel 10 param. #",                       // Name
#endif
#endif
    NULL,                                                           // Einheit
    LAST_PAR_INDEX,                                                 // Maximum
    LEAST_PAR_INDEX,                                                // Minimum
    138,                                                // Default
    1.0,                                                            // Faktor
    &ParVal_TSChannel9Par,                              // Parameter
    NULL                                                            // Behandlungsfunktion
};


UINT16 ParVal_TSChannel10Par;
PAR_DES_U16_T ParDes_TSChannel10Par =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _UINT16,                                        // ReadWrite, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "TSP Kanal 11 Parameter Nr.",                       // Name
#else
#ifdef LANGUAGE_ENGLISH
        "TM channel 11 param. #",                       // Name
#endif
#endif
    NULL,                                                           // Einheit
    LAST_PAR_INDEX,                                                 // Maximum
    LEAST_PAR_INDEX,                                                // Minimum
    138,                                                // Default
    1.0,                                                            // Faktor
    &ParVal_TSChannel10Par,                                 // Parameter
    NULL                                                            // Behandlungsfunktion
};


UINT16 ParVal_TSChannel11Par;
PAR_DES_U16_T ParDes_TSChannel11Par =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _UINT16,                                        // ReadWrite, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "TSP Kanal 12 Parameter Nr.",                       // Name
#else
#ifdef LANGUAGE_ENGLISH
        "TM channel 12 param. #",                       // Name
#endif
#endif
    NULL,                                                           // Einheit
    LAST_PAR_INDEX,                                                 // Maximum
    LEAST_PAR_INDEX,                                                // Minimum
    138,                                                // Default
    1.0,                                                            // Faktor
    &ParVal_TSChannel11Par,                                 // Parameter
    NULL                                                            // Behandlungsfunktion
};


UINT16 ParVal_TSChannel12Par;
PAR_DES_U16_T ParDes_TSChannel12Par =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _UINT16,                                        // ReadWrite, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "TSP Kanal 13 Parameter Nr.",                       // Name
#else
#ifdef LANGUAGE_ENGLISH
        "TM channel 13 param. #",                       // Name
#endif
#endif
    NULL,                                                           // Einheit
    LAST_PAR_INDEX,                                                 // Maximum
    LEAST_PAR_INDEX,                                                // Minimum
    138,                                                // Default
    1.0,                                                            // Faktor
    &ParVal_TSChannel12Par,                                 // Parameter
    NULL                                                            // Behandlungsfunktion
};


UINT16 ParVal_TSChannel13Par;
PAR_DES_U16_T ParDes_TSChannel13Par =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _UINT16,                                        // ReadWrite, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "TSP Kanal 14 Parameter Nr.",                       // Name
#else
#ifdef LANGUAGE_ENGLISH
        "TM channel 14 param. #",                       // Name
#endif
#endif
    NULL,                                                           // Einheit
    LAST_PAR_INDEX,                                                 // Maximum
    LEAST_PAR_INDEX,                                                // Minimum
    138,                                                // Default
    1.0,                                                            // Faktor
    &ParVal_TSChannel13Par,                                 // Parameter
    NULL                                                            // Behandlungsfunktion
};


UINT16 ParVal_TSChannel14Par;
PAR_DES_U16_T ParDes_TSChannel14Par =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _UINT16,                                        // ReadWrite, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "TSP Kanal 15 Parameter Nr.",                       // Name
#else
#ifdef LANGUAGE_ENGLISH
        "TM channel 15 param. #",                       // Name
#endif
#endif
    NULL,                                                           // Einheit
    LAST_PAR_INDEX,                                                 // Maximum
    LEAST_PAR_INDEX,                                                // Minimum
    138,                                                // Default
    1.0,                                                            // Faktor
    &ParVal_TSChannel14Par,                                 // Parameter
    NULL                                                            // Behandlungsfunktion
};


UINT16 ParVal_TSChannel15Par;
PAR_DES_U16_T ParDes_TSChannel15Par =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _UINT16,                                        // ReadWrite, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "TSP Kanal 16 Parameter Nr.",                       // Name
#else
#ifdef LANGUAGE_ENGLISH
        "TM channel 16 param. #",                       // Name
#endif
#endif
    NULL,                                                           // Einheit
    LAST_PAR_INDEX,                                                 // Maximum
    LEAST_PAR_INDEX,                                                // Minimum
    138,                                                // Default
    1.0,                                                            // Faktor
    &ParVal_TSChannel15Par,                                 // Parameter
    NULL                                                            // Behandlungsfunktion
};



// Parameter-Variable TSMemSize             //RS
//#pragma DATA_SECTION(ParVal_TSMemSize,".extram");
UINT16 ParVal_TSMemSize;
#define TSP_MAX_WORDS           (1*4096)        // Anzahl speicherbarer UINT32, must be multiple of TSP_CHANN_ANZAHL

// Parameter-Beschreibung TSMemSize
PAR_DES_U16_T ParDes_TSMemSize =
{
    PAR_RO | _UINT16,                           // ReadWrite, UINT16-Parameter
#ifdef LANGUAGE_GERMAN
    "TSP Speichergröße",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
        "TM memory size",                       // Name
#endif
#endif
    "LWord",                                                // Einheit
    TSP_MAX_WORDS,                              // Maximum
    0,                                                      // Minimum
    TSP_MAX_WORDS,                              // Default
    1.0,                                                    // Faktor
    &ParVal_TSMemSize,                      // Parameter
    NULL                                                    // Behandlungsfunktion
};


// Parameter-Variable ParVal_Testmode
//#pragma DATA_SECTION(ParVal_Testmode,".extram");
INT16 ParVal_Testmode;
PAR_DES_I16_T ParDes_Testmode =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "Testmode",                         // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "Testmode",                         // Name
    #endif
    #endif
    NULL,                                           // Einheit
    6,                                  // Max
    0,                              // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &ParVal_Testmode,               // Parameter
    NULL                                            // Behandlungsfunktion
};


// Parameter-Variable ParVal_i16_Test1
//#pragma DATA_SECTION(ParVal_i16_Test1,".extram");
INT16 ParVal_i16_Test1;
// Parameter-Variable ParDes_i16_Test1
PAR_DES_I16_T ParDes_i16_Test1 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "i16_Test1",                            // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "i16_Test1",                            // Name
    #endif
    #endif
    NULL,                                           // Einheit
    MAX_INT,                                    // Max
    -MAX_INT,                               // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &ParVal_i16_Test1,              // Parameter
    NULL                                            // Behandlungsfunktion
};

// Parameter-Variable ParVal_i16_Test2
//#pragma DATA_SECTION(ParVal_i16_Test2,".extram");
INT16 ParVal_i16_Test2;
// Parameter-Variable ParDes_i16_Test2
PAR_DES_I16_T ParDes_i16_Test2 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "i16_Test2",                            // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "i16_Test2",                            // Name
    #endif
    #endif
    NULL,                                           // Einheit
    MAX_INT,                                    // Max
    -MAX_INT,                               // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &ParVal_i16_Test2,              // Parameter
    NULL                                            // Behandlungsfunktion
};

// Parameter-Variable ParVal_i16_Test3
//#pragma DATA_SECTION(ParVal_i16_Test3,".extram");
INT16 ParVal_i16_Test3;
// Parameter-Variable ParDes_i16_Test3
PAR_DES_I16_T ParDes_i16_Test3 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "i16_Test3",                            // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "i16_Test3",                            // Name
    #endif
    #endif
    NULL,                                           // Einheit
    MAX_INT,                                    // Max
    -MAX_INT,                               // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &ParVal_i16_Test3,              // Parameter
    NULL                                            // Behandlungsfunktion
};

// Parameter-Variable ParVal_i16_Test4
//#pragma DATA_SECTION(ParVal_i16_Test4,".extram");
INT16 ParVal_i16_Test4;
// Parameter-Variable ParDes_i16_Test4
PAR_DES_I16_T ParDes_i16_Test4 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "i16_Test4",                            // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "i16_Test4",                            // Name
    #endif
    #endif
    NULL,                                           // Einheit
    MAX_INT,                                    // Max
    -MAX_INT,                               // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &ParVal_i16_Test4,              // Parameter
    NULL                                            // Behandlungsfunktion
};

// Parameter-Variable ParVal_i16_Test5
//#pragma DATA_SECTION(ParVal_i16_Test5,".extram");
INT16 ParVal_i16_Test5;
// Parameter-Variable ParDes_i16_Test5
PAR_DES_I16_T ParDes_i16_Test5 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "i16_Test5",                            // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "i16_Test5",                            // Name
    #endif
    #endif
    NULL,                                           // Einheit
    MAX_INT,                                    // Max
    -MAX_INT,                               // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &ParVal_i16_Test5,              // Parameter
    NULL                                            // Behandlungsfunktion
};

// Parameter-Variable ParVal_i16_Test6
//#pragma DATA_SECTION(ParVal_i16_Test6,".extram");
INT16 ParVal_i16_Test6;
// Parameter-Variable ParDes_i16_Test6
PAR_DES_I16_T ParDes_i16_Test6 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "i16_Test6",                            // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "i16_Test6",                            // Name
    #endif
    #endif
    NULL,                                           // Einheit
    MAX_INT,                                    // Max
    -MAX_INT,                               // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &ParVal_i16_Test6,              // Parameter
    NULL                                            // Behandlungsfunktion
};

// Parameter-Variable ParVal_i16_Test7
//#pragma DATA_SECTION(ParVal_i16_Test7,".extram");
INT16 ParVal_i16_Test7;
// Parameter-Variable ParDes_i16_Test7
PAR_DES_I16_T ParDes_i16_Test7 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "i16_Test7",                            // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "i16_Test7",                            // Name
    #endif
    #endif
    NULL,                                           // Einheit
    MAX_INT,                                    // Max
    -MAX_INT,                               // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &ParVal_i16_Test7,              // Parameter
    NULL                                            // Behandlungsfunktion
};

// Parameter-Variable ParVal_i16_Test8
//#pragma DATA_SECTION(ParVal_i16_Test8,".extram");
INT16 ParVal_i16_Test8;
// Parameter-Variable ParDes_i16_Test8
PAR_DES_I16_T ParDes_i16_Test8 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "i16_Test8",                            // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "i16_Test8",                            // Name
    #endif
    #endif
    NULL,                                           // Einheit
    MAX_INT,                                    // Max
    -MAX_INT,                               // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &ParVal_i16_Test8,              // Parameter
    NULL                                            // Behandlungsfunktion
};

// Parameter-Variable ParVal_i32_Test9
//#pragma DATA_SECTION(ParVal_i16_Test9,".extram");
INT16 ParVal_i16_Test9;
// Parameter-Variable ParDes_i32_Test9
PAR_DES_I16_T ParDes_i16_Test9 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "i16_Test9",                            // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "i16_Test9",                            // Name
    #endif
    #endif
    NULL,                                           // Einheit
    MAX_INT,                                    // Max
    -MAX_INT,                               // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &ParVal_i16_Test9,              // Parameter
    NULL                                            // Behandlungsfunktion
};

// Parameter-Variable ParVal_i32_Test10
//#pragma DATA_SECTION(ParVal_i16_Test10,".extram");
INT16 ParVal_i16_Test10;
// Parameter-Variable ParDes_i32_Test10
PAR_DES_I16_T ParDes_i16_Test10 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "i16_Test10",                           // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "i16_Test10",                           // Name
    #endif
    #endif
    NULL,                                           // Einheit
    MAX_INT,                                    // Max
    -MAX_INT,                               // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &ParVal_i16_Test10,             // Parameter
    NULL                                            // Behandlungsfunktion
};

// Parameter-Variable ParVal_i32_Test11
//#pragma DATA_SECTION(ParVal_i16_Test11,".extram");
INT16 ParVal_i16_Test11;
// Parameter-Variable ParDes_i32_Test11
PAR_DES_I16_T ParDes_i16_Test11 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "i16_Test11",                           // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "i16_Test11",                           // Name
    #endif
    #endif
    NULL,                                           // Einheit
    MAX_INT,                                    // Max
    -MAX_INT,                               // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &ParVal_i16_Test11,             // Parameter
    NULL                                            // Behandlungsfunktion
};

// Parameter-Variable ParVal_i16_Test12
//#pragma DATA_SECTION(ParVal_i16_Test12,".extram");
INT16 ParVal_i16_Test12;
// Parameter-Variable ParDes_i16_Test12
PAR_DES_I16_T ParDes_i16_Test12 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "i16_Test12",                           // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "i16_Test12",                           // Name
    #endif
    #endif
    NULL,                                           // Einheit
    MAX_INT,                                    // Max
    -MAX_INT,                               // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &ParVal_i16_Test12,             // Parameter
    NULL                                            // Behandlungsfunktion
};


// Parameter-Variable Testvars
//#pragma DATA_SECTION(ParVal_Testvars,".extram");
//INT16 ParVal_Testvars;
// Parameter-Variable ParDes_Testvars
PAR_DES_I16_T ParDes_Testvars =
{
    PAR_MENUE | PAR_RW | _INT16,                    // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "Testvariablen",                            // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "Test variables",                           // Name
    #endif
    #endif
    NULL,                                           // Einheit
    1,                                  // Max
    0,                              // Min
    0,                                              // Default
    1.0,                                            // Faktor
    NULL/*&ParVal_Testvars*/,               // Parameter
    NULL                                            // Behandlungsfunktion
};


// Parameter-Variable ParVal_i16_Test13
//#pragma DATA_SECTION(ParVal_i16_Test13,".extram");
INT16 ParVal_i16_Test13;
// Parameter-Variable ParDes_i16_Test13
PAR_DES_I16_T ParDes_i16_Test13 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "i16_Test13",                           // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "i16_Test13",                           // Name
    #endif
    #endif
    NULL,                                           // Einheit
    MAX_INT,                                    // Max
    -MAX_INT,                               // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &ParVal_i16_Test13,             // Parameter
    NULL                                            // Behandlungsfunktion
};


// Parameter-Variable Feedbacks
//#pragma DATA_SECTION(ParVal_Feedbacks,".extram");
//INT16 ParVal_Feedbacks;

// Istwerte
// Parameter-Beschreibung Feedbacks
PAR_DES_I16_T ParDes_Feedbacks =
{
    PAR_MENUE | PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Istwerte",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Feedbacks",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_Feedbacks*/,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};



//#pragma DATA_SECTION(ParVal_TransMemory,".extram");
//INT16 ParVal_TransMemory;

PAR_DES_I16_T ParDes_TransMemory =
{
    PAR_MENUE | PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Transientenspeicher",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Transient memory",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_TransMemory*/,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};



// Parameter-Variable Vline
//#pragma DATA_SECTION(ParVal_Vline,".extram");
INT32 ParVal_Vline;

// Istwert Netzsspannung (L-L), in V
// Parameter-Beschreibung Vline
PAR_DES_I32_T ParDes_Vline =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Netzspannung",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Line voltage",                             // Name
#endif
#endif
    "V",                                                            // Einheit
    10000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Vline,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_Vinv,".extram");
INT32 ParVal_Vinv;

PAR_DES_I32_T ParDes_Vinv =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Umrichterspannung",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Inverter voltage",                             // Name
#endif
#endif
    "V",                                                            // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_Vinv,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_VLine1,".extram");
INT32 ParVal_VLine1;
PAR_DES_I32_T ParDes_VLine1 =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Netzspannung L1",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Line voltage L1",                              // Name
#endif
#endif
    "V",                                                            // Einheit
    10000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_VLine1,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_VLine2,".extram");
INT32 ParVal_VLine2;
PAR_DES_I32_T ParDes_VLine2 =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Netzspannung L2",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Line voltage L2",                              // Name
#endif
#endif
    "V",                                                            // Einheit
    10000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_VLine2,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_VLine3,".extram");
INT32 ParVal_VLine3;
PAR_DES_I32_T ParDes_VLine3 =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Netzspannung L3",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Line voltage L3",                              // Name
#endif
#endif
    "V",                                                            // Einheit
    10000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_VLine3,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Fline
//#pragma DATA_SECTION(ParVal_Fline,".extram");
INT32 ParVal_Fline;

// Istwert Netzfrequenz, in 0.1 Hz
// Parameter-Beschreibung Fline
PAR_DES_I32_T ParDes_Fline =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Netzfrequenz",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Line frequency",                               // Name
#endif
#endif
    "Hz",                                                           // Einheit
    10000000,                                               // Maximum
    -10000000,                                                              // Minimum
    0,                                                              // Default
    0.01,                     // Faktor
    &ParVal_Fline,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Vdc
//#pragma DATA_SECTION(ParVal_Vdc,".extram");
INT32 ParVal_Vdc;

// Istwert Zwischenkreisspannung, in V
// Parameter-Beschreibung Vdc
PAR_DES_I32_T ParDes_Vdc =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Zwischenkreisspannung",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "DC link voltage",                              // Name
#endif
#endif
    "V",                                                            // Einheit
    2000,                                               // Maximum
    -100,                                                               // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_Vdc,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Iinv
//#pragma DATA_SECTION(ParVal_Iinv,".extram");
INT32 ParVal_Iinv;

// Istwert Umrichterstrom, in A
// Parameter-Beschreibung Iinv
PAR_DES_I32_T ParDes_Iinv =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Ausgangsstrom",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Inverter current",                             // Name
#endif
#endif
    "A",                                                            // Einheit
    500000,                                             // Maximum
    -500,                                                               // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Iinv,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Pinv
//#pragma DATA_SECTION(ParVal_Pinv,".extram");
INT32 ParVal_Pinv;

// Istwert Wirkleistung, in kW
// Parameter-Beschreibung Pinv
PAR_DES_I32_T ParDes_Pinv =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Wirkleistung LSI",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Active power LSI",                             // Name
#endif
#endif
    "kW",                                                           // Einheit
    50000,                                              // Maximum
    -50000,                                                             // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Pinv,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


INT32 ParVal_Ps;
PAR_DES_I32_T ParDes_Ps =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Wirkleistung Stator",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Active power stator",                              // Name
#endif
#endif
    "kW",                                                           // Einheit
    50000,                                              // Maximum
    -50000,                                                             // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Ps,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


INT32 ParVal_Pline;
PAR_DES_I32_T ParDes_Pline =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Wirkleistung Netz",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Active power grid",                                // Name
#endif
#endif
    "kW",                                                           // Einheit
    50000,                                              // Maximum
    -50000,                                                             // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Pline,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Qinv
//#pragma DATA_SECTION(ParVal_Qinv,".extram");
INT32 ParVal_Qinv;

// Istwert Blindleistung, in kVAr
// Parameter-Beschreibung Qinv
PAR_DES_I32_T ParDes_Qinv =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Blindleistung LSI",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Reactive power LSI",                               // Name
#endif
#endif
    "kVAr",                                                         // Einheit
    50000,                                              // Maximum
    -50000,                                                             // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Qinv,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


INT32 ParVal_Qs;
PAR_DES_I32_T ParDes_Qs =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Blindleistung Stator",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Reactive power stator",                                // Name
#endif
#endif
    "kVAr",                                                         // Einheit
    50000,                                              // Maximum
    -50000,                                                             // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Qs,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


INT32 ParVal_Qline;
PAR_DES_I32_T ParDes_Qline =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Blindleistung Netz",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Reactive power grid",                              // Name
#endif
#endif
    "kVAr",                                                         // Einheit
    50000,                                              // Maximum
    -50000,                                                             // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Qline,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable ILine
//#pragma DATA_SECTION(ParVal_ILine,".extram");
INT32 ParVal_ILine;

// Istwert Netzstrom, in A
// Parameter-Beschreibung ILine
PAR_DES_I32_T ParDes_ILine =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Netzstrom",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Line current",                             // Name
#endif
#endif
    "A",                                                            // Einheit
    500000,                                             // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_ILine,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


INT32 ParVal_Is;
PAR_DES_I32_T ParDes_Is =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Statorstrom",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Stator current",                               // Name
#endif
#endif
    "A",                                                            // Einheit
    500000,                                             // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Is,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable CosPhiLine
//#pragma DATA_SECTION(ParVal_CosPhiLine,".extram");
INT32 ParVal_CosPhiLine;

// Line power factor
// Parameter-Beschreibung CosPhiLine
PAR_DES_I32_T ParDes_CosPhiLine =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Cos(phi) Netz",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Cos(phi) line",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    65536,                                              // Maximum
    -65536,                                                             // Minimum
    0,                                                              // Default
    (1.0/65536),                      // Faktor
    &ParVal_CosPhiLine,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_CosPhiInv,".extram");
INT32 ParVal_CosPhiInv;

// Line power factor
// Parameter-Beschreibung CosPhiInv
PAR_DES_I32_T ParDes_CosPhiInv =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Cos(phi) Umrichter",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Cos(phi) inverter",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    65536,                                              // Maximum
    -65536,                                                             // Minimum
    0,                                                              // Default
    (1.0/65536),                      // Faktor
    &ParVal_CosPhiInv,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};



// Parameter-Variable Tigbt
//#pragma DATA_SECTION(ParVal_Tigbt,".extram");
//#pragma DATA_SECTION(ParVal_Tigbt1,".extram");
//#pragma DATA_SECTION(ParVal_Tigbt2,".extram");
//#pragma DATA_SECTION(ParVal_Tigbt3,".extram");
//#pragma DATA_SECTION(ParVal_Tigbt4,".extram");
INT32 ParVal_Tigbt;
INT32 ParVal_Tigbt1;
INT32 ParVal_Tigbt2;
INT32 ParVal_Tigbt3;
INT32 ParVal_Tigbt4;

// Istwert IGBT-Temperatur, in °C
// Parameter-Beschreibung Tigbt
PAR_DES_I32_T ParDes_Tigbt =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "IGBT-Temperatur",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "IGBT temperature",                             // Name
#endif
#endif
    "°C",                                                           // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Tigbt,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I32_T ParDes_Tigbt1 =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "IGBT-Temperatur 1",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "IGBT temperature 1",                               // Name
#endif
#endif
    "°C",                                                           // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Tigbt1,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I32_T ParDes_Tigbt2 =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "IGBT-Temperatur 2",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "IGBT temperature 2",                               // Name
#endif
#endif
    "°C",                                                           // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Tigbt2,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I32_T ParDes_Tigbt3 =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "IGBT-Temperatur 3",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "IGBT temperature 3",                               // Name
#endif
#endif
    "°C",                                                           // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Tigbt3,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I32_T ParDes_Tigbt4 =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "IGBT-Temperatur 4",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "IGBT temperature 4",                               // Name
#endif
#endif
    "°C",                                                           // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Tigbt4,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable DigIn
//#pragma DATA_SECTION(ParVal_DigIn,".extram");
DWORD ParVal_DigIn;

// Digital Inputs
// Parameter-Beschreibung DigIn
PAR_DES_DWORD_T ParDes_DigIn =
{
    PAR_RO | _DWORD,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Digitale Eingänge",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Digital Inputs",                               // Name
#endif
#endif
    0xffffffff,                                             // Maximum
    0x00000000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_DigIn,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T ParDes_ConverterConfig =
{
    PAR_MENUE | PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Umrichterkonfiguration",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Converter configuration",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_PWM*/,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_PIBconfig,".extram");
WORD ParVal_PIBconfig;

// Digital Inputs
// Parameter-Beschreibung DigIn
PAR_DES_WORD_T ParDes_PIBconfig =
{
    PAR_RO | _WORD,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "PIB Konfiguration",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "PIB configuration",                                // Name
#endif
#endif
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_PIBconfig,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_PIBref,".extram");
WORD ParVal_PIBref;

// Digital Inputs
// Parameter-Beschreibung DigIn
PAR_DES_WORD_T ParDes_PIBref =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,                                  // ReadOnly
#ifdef LANGUAGE_GERMAN
    "PIB Referenz",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "PIB reference",                                // Name
#endif
#endif
    0x0007,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_PIBref,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_ILine_mon,".extram");
WORD ParVal_ILine_mon;

// Digital Inputs
// Parameter-Beschreibung DigIn
PAR_DES_WORD_T ParDes_ILine_mon =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,                                  // ReadOnly
#ifdef LANGUAGE_GERMAN
    "ILine Monitoring",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "ILine monitoring",                             // Name
#endif
#endif
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_ILine_mon,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_PIBtempdiff,".extram");
INT32 ParVal_PIBtempdiff;

PAR_DES_I32_T ParDes_PIBtempdiff =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,
#ifdef LANGUAGE_GERMAN
    "Max. PIB Temperaturdifferenz",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Max. PIB temp. difference",                                // Name
#endif
#endif
    "deg",                                                          // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_PIBtempdiff,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable DigOut
//#pragma DATA_SECTION(ParVal_DigOut,".extram");
WORD ParVal_DigOut;

// Digital Outputs
// Parameter-Beschreibung DigOut
PAR_DES_WORD_T ParDes_DigOut =
{
    PAR_RO | _WORD,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Digitale Ausgänge",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Digital Outputs",                              // Name
#endif
#endif
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_DigOut,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_PWM,".extram");
//INT16 ParVal_PWM;
// Parameter-Beschreibung AnInputs
PAR_DES_I16_T ParDes_PWM =
{
    PAR_MENUE | PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "PWM/Betriebssystem",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "PWM/Opsys",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_PWM*/,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Analog Inputs

// Parameter-Variable AnInputs
//#pragma DATA_SECTION(ParVal_AnInputs,".extram");
//INT16 ParVal_AnInputs;
// Parameter-Beschreibung AnInputs
PAR_DES_I16_T ParDes_AnInputs =
{
    PAR_MENUE | PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Analoge Eingänge",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Analog inputs",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_AnInputs*/,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Analog Inputs

// Parameter-Variable AnIn0
//#pragma DATA_SECTION(ParVal_AnIn0,".extram");
INT16 ParVal_AnIn0;
// Parameter-Beschreibung AnIn0
PAR_DES_I16_T ParDes_AnIn0 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Analogeingang 0",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Analog Input 0",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AnIn0,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

// Parameter-Variable AnIn1
//#pragma DATA_SECTION(ParVal_AnIn1,".extram");
INT16 ParVal_AnIn1;
// Parameter-Beschreibung AnIn1
PAR_DES_I16_T ParDes_AnIn1 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Analogeingang 1",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Analog Input 1",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AnIn1,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

// Parameter-Variable AnIn2
//#pragma DATA_SECTION(ParVal_AnIn2,".extram");
INT16 ParVal_AnIn2;
// Parameter-Beschreibung AnIn2
PAR_DES_I16_T ParDes_AnIn2 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Analogeingang 2",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Analog Input 2",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AnIn2,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

// Parameter-Variable AnIn3
//#pragma DATA_SECTION(ParVal_AnIn3,".extram");
INT16 ParVal_AnIn3;
// Parameter-Beschreibung AnIn3
PAR_DES_I16_T ParDes_AnIn3 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Analogeingang 3",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Analog Input 3",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AnIn3,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

// Parameter-Variable AnIn4
//#pragma DATA_SECTION(ParVal_AnIn4,".extram");
INT16 ParVal_AnIn4;
// Parameter-Beschreibung AnIn4
PAR_DES_I16_T ParDes_AnIn4 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Analogeingang 4",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Analog Input 4",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AnIn4,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

// Parameter-Variable AnIn5
//#pragma DATA_SECTION(ParVal_AnIn5,".extram");
INT16 ParVal_AnIn5;
// Parameter-Beschreibung AnIn5
PAR_DES_I16_T ParDes_AnIn5 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Analogeingang 5",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Analog Input 5",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AnIn5,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

// Parameter-Variable AnIn6
//#pragma DATA_SECTION(ParVal_AnIn6,".extram");
INT16 ParVal_AnIn6;
// Parameter-Beschreibung AnIn6
PAR_DES_I16_T ParDes_AnIn6 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Analogeingang 6",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Analog Input 6",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AnIn6,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

// Parameter-Variable AnIn7
//#pragma DATA_SECTION(ParVal_AnIn7,".extram");
INT16 ParVal_AnIn7;
// Parameter-Beschreibung AnIn7
PAR_DES_I16_T ParDes_AnIn7 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Analogeingang 7",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Analog Input 7",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AnIn7,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

// Parameter-Variable AnIn8
//#pragma DATA_SECTION(ParVal_AnIn8,".extram");
INT16 ParVal_AnIn8;
// Parameter-Beschreibung AnIn8
PAR_DES_I16_T ParDes_AnIn8 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Analogeingang 8",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Analog Input 8",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AnIn8,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

// Parameter-Variable AnIn9
//#pragma DATA_SECTION(ParVal_AnIn9,".extram");
INT16 ParVal_AnIn9;
// Parameter-Beschreibung AnIn9
PAR_DES_I16_T ParDes_AnIn9 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Analogeingang 9",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Analog Input 9",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AnIn9,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

// Parameter-Variable AnIn10
//#pragma DATA_SECTION(ParVal_AnIn10,".extram");
INT16 ParVal_AnIn10;
// Parameter-Beschreibung AnIn10
PAR_DES_I16_T ParDes_AnIn10 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Analogeingang 10",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Analog Input 10",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AnIn10,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

// Parameter-Variable AnIn11
//#pragma DATA_SECTION(ParVal_AnIn11,".extram");
//#pragma DATA_SECTION(ParVal_AnIn12,".extram");
//#pragma DATA_SECTION(ParVal_AnIn13,".extram");
//#pragma DATA_SECTION(ParVal_AnIn14,".extram");
//#pragma DATA_SECTION(ParVal_AnIn15,".extram");
//#pragma DATA_SECTION(ParVal_AnIn16,".extram");
//#pragma DATA_SECTION(ParVal_AnIn17,".extram");
//#pragma DATA_SECTION(ParVal_AnIn18,".extram");
//#pragma DATA_SECTION(ParVal_AnIn19,".extram");
//#pragma DATA_SECTION(ParVal_AnIn20,".extram");
//#pragma DATA_SECTION(ParVal_AnIn21,".extram");
INT16 ParVal_AnIn11;
INT16 ParVal_AnIn12;
INT16 ParVal_AnIn13;
INT16 ParVal_AnIn14;
INT16 ParVal_AnIn15;
INT16 ParVal_AnIn16;
INT16 ParVal_AnIn17;
INT16 ParVal_AnIn18;
INT16 ParVal_AnIn19;
INT16 ParVal_AnIn20;
INT16 ParVal_AnIn21;

// Parameter-Beschreibung AnIn11
PAR_DES_I16_T ParDes_AnIn11 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Analogeingang 11",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Analog Input 11",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AnIn11,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_AnIn12 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Analogeingang 12",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Analog Input 12",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AnIn12,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_AnIn13 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Analogeingang 13",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Analog Input 13",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AnIn13,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_AnIn14 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Analogeingang 14",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Analog Input 14",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AnIn14,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_AnIn15 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Analogeingang 15",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Analog Input 15",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AnIn15,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_AnIn16 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Analogeingang 16",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Analog Input 16",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AnIn16,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_AnIn17 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Analogeingang 17",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Analog Input 17",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AnIn17,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_AnIn18 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Analogeingang 18",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Analog Input 18",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AnIn18,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_AnIn19 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Analogeingang 19",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Analog Input 19",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AnIn19,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_AnIn20 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Analogeingang 20",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Analog Input 20",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AnIn20,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_AnIn21 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Analogeingang 21",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Analog Input 21",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AnIn21,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable SW_FaultCode
//#pragma DATA_SECTION(ParVal_SW_FaultCode,".extram");
DWORD ParVal_SW_FaultCode;

// Software fault code
// Parameter-Beschreibung SW_FaultCode
PAR_DES_DWORD_T ParDes_SW_FaultCode =
{
    PAR_RO | _DWORD,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Software Fehlercode",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Software fault code",                              // Name
#endif
#endif
    0xffffffff,                                             // Maximum
    0x00000000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_SW_FaultCode,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable HW_FaultCode
//#pragma DATA_SECTION(ParVal_HW_FaultCode,".extram");
DWORD ParVal_HW_FaultCode;

// Hardware fault code
// Parameter-Beschreibung HW_FaultCode
PAR_DES_DWORD_T ParDes_HW_FaultCode =
{
    PAR_RO | _DWORD,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Hardware Fehlercode",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Hardware fault code",                              // Name
#endif
#endif
    0xffffffff,                                             // Maximum
    0x00000000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_HW_FaultCode,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


DWORD ParVal_DigIn_FaultCode;
PAR_DES_DWORD_T ParDes_DigIn_FaultCode =
{
    PAR_RO | _DWORD,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "DigIn Fehlercode",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "DigIn fault code",                             // Name
#endif
#endif
    0xffffffff,                                             // Maximum
    0x00000000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_DigIn_FaultCode,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Comm_FaultCode
//#pragma DATA_SECTION(ParVal_Comm_FaultCode,".extram");
WORD ParVal_Comm_FaultCode;

// Communication fault code
// Parameter-Beschreibung Comm_FaultCode
PAR_DES_WORD_T ParDes_Comm_FaultCode =
{
    PAR_RW | _WORD,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "E2P Kommunikation Fehlercode",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "E2P communication fault code",                             // Name
#endif
#endif
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_Comm_FaultCode,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

int16_T ParVal_Parameter_FaultCode;    /* '<Root>/P_Vline_tripp45' */

// Parameter-Variable Parameter_FaultCode
//WORD ParVal_Parameter_FaultCode;

// Parameter fault code
PAR_DES_I16_T ParDes_Parameter_FaultCode =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Parameter Fehlercode",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Parameter fault code",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_Parameter_FaultCode,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

#if 0
// ILineFault
PAR_DES_WORD_T ParDes_ILineFault =
{
    PAR_RO | _WORD,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "ILine Fehlercode",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "ILine fault code",                             // Name
#endif
#endif
    65535,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    &ParVal_ILineFault,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};
#endif

// Parameter-Variable AutoresetEn
//#pragma DATA_SECTION(ParVal_AutoresetEn,".extram");
INT16 ParVal_AutoresetTime;

// Communication fault code
// Parameter-Beschreibung Comm_FaultCode
PAR_DES_I16_T ParDes_AutoresetTime =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Autoresetzeit",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Autoreset delay",                              // Name
#endif
#endif
    "s",                                                            // Einheit
    30999,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AutoresetTime,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Controls
//#pragma DATA_SECTION(ParVal_Controls,".extram");
//INT16 ParVal_Controls;
// Parameter-Beschreibung Controls
PAR_DES_I16_T ParDes_Controls =
{
    PAR_MENUE | PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Steuerung",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Controls",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1,                    // Faktor
    NULL/*&ParVal_Controls*/,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable ConvState
//#pragma DATA_SECTION(ParVal_ConvState,".extram");
INT16 ParVal_ConvState;
// Parameter-Beschreibung Converter state
PAR_DES_I16_T ParDes_ConvState =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Zustand Umrichter",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Converter state",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_ConvState,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


INT16 ParVal_InvCtrlState;
PAR_DES_I16_T ParDes_InvCtrlState =
{
    PAR_RO | _INT16,                                    // ReadOnly
    "Inverter control state",                               // Name
    NULL,                                                           // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_InvCtrlState,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Faults
//#pragma DATA_SECTION(ParVal_Faults,".extram");
//INT16 ParVal_Faults;
// Parameter-Beschreibung Faults
PAR_DES_I16_T ParDes_Faults =
{
    PAR_MENUE | PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Fehler",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Faults",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1,                    // Faktor
    NULL/*&ParVal_Faults*/,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable StateFaultCode
//#pragma DATA_SECTION(ParVal_StateFaultCode,".extram");
INT16 ParVal_StateFaultCode;
// Parameter-Beschreibung State fault code
PAR_DES_I16_T ParDes_StateFaultCode =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Zustandsfehlercode",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "State fault code",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_StateFaultCode,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable CtrlSrc
//#pragma DATA_SECTION(ParVal_CtrlSrc,".extram");
WORD ParVal_CtrlSrc;

// Parameter-Beschreibung CtrlSrc
PAR_DES_WORD_T ParDes_CtrlSrc =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "Steuerquelle",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Control source",                               // Name
#endif
#endif
    0x0002,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_CtrlSrc,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

int16_T LSI_IdentEn;                   /* '<Root>/P_Vline_tripp72' */

PAR_DES_I16_T ParDes_LSI_IdentEn =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Filtersynchronisation",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Filter synchronization",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    300,                                                // Maximum
    0,                                                              // Minimum
    100,                                                                // Default
    1,                    // Faktor
    &LSI_IdentEn,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_TempChar,".extram");
INT16 ParVal_TempChar;

PAR_DES_I16_T ParDes_TempChar =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Temperaturkennlinie",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Temperature characteristic",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    5,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_TempChar,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};



// Parameter-Variable CtrlSrc
//#pragma DATA_SECTION(ParVal_RefSrc,".extram");
WORD ParVal_RefSrc;

// Fault reset input
// Parameter-Beschreibung CtrlSrc
PAR_DES_WORD_T ParDes_RefSrc =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "Sollwertquelle",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Reference source",                             // Name
#endif
#endif
    0x0002,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_RefSrc,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable FaultResetIn
//#pragma DATA_SECTION(ParVal_FaultResetIn,".extram");
WORD ParVal_FaultResetIn;

// Fault reset input
// Parameter-Beschreibung FaultResetIn
PAR_DES_WORD_T ParDes_FaultResetIn =
{
    PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "Fehlerreset",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Fault reset",                              // Name
#endif
#endif
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_FaultResetIn,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable ForceDOs
//#pragma DATA_SECTION(ParVal_ForceDOs,".extram");
INT16 ParVal_ForceDOs;

PAR_DES_I16_T ParDes_ForceDOs =
{
    PAR_PASS_PW | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Force digital outputs",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Force digital outputs",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_ForceDOs,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable StartStopIn
//WORD ParVal_StartStopIn;
uint16_T ParVal_StartStopIn;           /* '<Root>/P_Vline_tripp42' */

// Start/Stop input
// Parameter-Beschreibung StartStopIn
PAR_DES_WORD_T ParDes_StartStopIn =
{
    PAR_RW | _WORD,
    "Start LSI",                                // Name
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_StartStopIn,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

uint16_T ParVal_StartDCIn;             /* '<Root>/P_Vline_tripp128' */

//WORD ParVal_StartDCIn;
PAR_DES_WORD_T ParDes_StartDCIn =
{
    PAR_RW | _WORD,
    "Start DC",                             // Name
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_StartDCIn,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


WORD ParVal_StartCBIn;
PAR_DES_WORD_T ParDes_StartCBIn =
{
    PAR_RW | _WORD,
    "Start CB",                             // Name
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_StartCBIn,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

uint16_T ParVal_PWMenableIn;           /* '<Root>/P_Vline_tripp41' */

//WORD ParVal_PWMenableIn;
PAR_DES_WORD_T ParDes_PWMenableIn =
{
    PAR_RW | _WORD,
    "Start GSI",                                // Name
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_PWMenableIn,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable PrechCtrlIn
//#pragma DATA_SECTION(ParVal_PrechCtrlIn,".extram");
WORD ParVal_PrechCtrlIn;

// Precharge control input
// Parameter-Beschreibung PrechCtrlIn
PAR_DES_WORD_T ParDes_PrechCtrlIn =
{
    PAR_PASS_PW | PAR_RW | _WORD,
    "Force precharge",                              // Name
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_PrechCtrlIn,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable FanCtrlIn
//#pragma DATA_SECTION(ParVal_FanCtrlIn,".extram");
WORD ParVal_FanCtrlIn;

// Fan control input
// Parameter-Beschreibung FanCtrlIn
PAR_DES_WORD_T ParDes_FanCtrlIn =
{
    PAR_PASS_PW | PAR_RW | _WORD,
    "Force fan",                                // Name
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_FanCtrlIn,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable MainsCtrlIn
//#pragma DATA_SECTION(ParVal_MainsCtrlIn,".extram");
WORD ParVal_MainsCtrlIn;

// Mains control input
// Parameter-Beschreibung MainsCtrlIn
PAR_DES_WORD_T ParDes_MainsCtrlIn =
{
    PAR_PASS_PW | PAR_RW | _WORD,
    "Force LSI cont",                               // Name
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_MainsCtrlIn,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_FaultRelIn,".extram");
WORD ParVal_FaultRelIn;

PAR_DES_WORD_T ParDes_FaultRelIn =
{
    PAR_PASS_PW | PAR_RW | _WORD,
    "Force CB on",                              // Name
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_FaultRelIn,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_Res1In,".extram");
WORD ParVal_Res1In;

PAR_DES_WORD_T ParDes_Res1In =
{
    PAR_PASS_PW | PAR_RW | _WORD,
    "Force Heatex",                             // Name
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_Res1In,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_Res2In,".extram");
WORD ParVal_Res2In;

PAR_DES_WORD_T ParDes_Res2In =
{
    PAR_PASS_PW | PAR_RW | _WORD,
    "Force Pump",                               // Name
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_Res2In,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


WORD ParVal_HeaterIn;
PAR_DES_WORD_T ParDes_HeaterIn =
{
    PAR_PASS_PW | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "Steuerung Heater",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Force Heater",                             // Name
#endif
#endif
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_HeaterIn,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


WORD ParVal_ValveIn;
PAR_DES_WORD_T ParDes_ValveIn =
{
    PAR_PASS_PW | PAR_RW | _WORD,
    "Force CB off",                             // Name
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_ValveIn,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


WORD ParVal_DO9_CtrlIn;
PAR_DES_WORD_T ParDes_DO9_CtrlIn =
{
    PAR_PASS_PW | PAR_RW | _WORD,
    "Force Chopper",                                // Name
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_DO9_CtrlIn,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


WORD ParVal_DO10_CtrlIn;
PAR_DES_WORD_T ParDes_DO10_CtrlIn =
{
    PAR_PASS_PW | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "Steuerung DO10",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Force DO10",                               // Name
#endif
#endif
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_DO10_CtrlIn,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


WORD ParVal_DO11_CtrlIn;
PAR_DES_WORD_T ParDes_DO11_CtrlIn =
{
    PAR_PASS_PW | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "Steuerung DO11",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Force DO11",                               // Name
#endif
#endif
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_DO11_CtrlIn,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


WORD ParVal_DO12_CtrlIn;
PAR_DES_WORD_T ParDes_DO12_CtrlIn =
{
    PAR_PASS_PW | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "Steuerung DO12",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Force DO12",                               // Name
#endif
#endif
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_DO12_CtrlIn,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


WORD ParVal_DO13_CtrlIn;
PAR_DES_WORD_T ParDes_DO13_CtrlIn =
{
    PAR_PASS_PW | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "Steuerung DO13",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Force DO13",                               // Name
#endif
#endif
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_DO13_CtrlIn,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


WORD ParVal_DO14_CtrlIn;
PAR_DES_WORD_T ParDes_DO14_CtrlIn =
{
    PAR_PASS_PW | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "Steuerung DO14",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Force DO14",                               // Name
#endif
#endif
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_DO14_CtrlIn,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


WORD ParVal_DO15_CtrlIn;
PAR_DES_WORD_T ParDes_DO15_CtrlIn =
{
    PAR_PASS_PW | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "Steuerung DO15",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Force DO15",                               // Name
#endif
#endif
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_DO15_CtrlIn,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


WORD ParVal_DO16_CtrlIn;
PAR_DES_WORD_T ParDes_DO16_CtrlIn =
{
    PAR_PASS_PW | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "Steuerung DO16",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Force DO16",                               // Name
#endif
#endif
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_DO16_CtrlIn,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable ReadyRunFault
//#pragma DATA_SECTION(ParVal_ReadyRunFault,".extram");
WORD ParVal_ReadyRunFault;

// Parameter-Beschreibung ReadyRunFault
PAR_DES_WORD_T ParDes_ReadyRunFault =
{
    PAR_RO | _WORD,
#ifdef LANGUAGE_GERMAN
    "Betriebszustand",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Operation state",                              // Name
#endif
#endif
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_ReadyRunFault,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Reference
//#pragma DATA_SECTION(ParVal_Reference,".extram");
//INT16 ParVal_Reference;

// Sollwerte
// Parameter-Beschreibung Reference
PAR_DES_I16_T ParDes_Reference =
{
    PAR_MENUE | PAR_RW | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Q-Regelung / Sollwerte",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Q control / Reference",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_Reference*/,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable TestRef1
//#pragma DATA_SECTION(ParVal_TestRef1,".extram");
INT16 ParVal_TestRef1;

// Testsollwert 1
// Parameter-Beschreibung TestRef1
PAR_DES_I16_T ParDes_TestRef1 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                                  // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Sollwert 1",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Reference 1",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    0x7fff,                                             // Maximum
    0x8001,                                                             // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_TestRef1,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable TestRef2
//#pragma DATA_SECTION(ParVal_TestRef2,".extram");
INT16 ParVal_TestRef2;

// Testsollwert 2
// Parameter-Beschreibung TestRef2
PAR_DES_I16_T ParDes_TestRef2 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                                  // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Sollwert 2",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Reference 2",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    0x7fff,                                             // Maximum
    0x8001,                                                             // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_TestRef2,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable IreRef
//#pragma DATA_SECTION(ParVal_IreRef,".extram");
INT32 ParVal_IreRef;

// Reactive current reference
// Parameter-Beschreibung IreRef
PAR_DES_I32_T ParDes_IreRef =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Sollwert Blindleistung",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Reactive power reference",                             // Name
#endif
#endif
    "kVAr",                                                         // Einheit
    300000,                                             // Maximum
    -300000,                                                                // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_IreRef,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable IreRef
//#pragma DATA_SECTION(ParVal_IreLim,".extram");
INT32 ParVal_IreLim;

// Reactive current reference
// Parameter-Beschreibung IreRef
PAR_DES_I32_T ParDes_IreLim =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Maximum Blindleistung",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Reactive power maximum",                               // Name
#endif
#endif
    "kVAr",                                                         // Einheit
    300000,                                             // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_IreLim,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable CosPhiRef
//#pragma DATA_SECTION(ParVal_CosPhiRef,".extram");
INT16 ParVal_CosPhiRef;

// Reactive current reference
// Parameter-Beschreibung CosPhiRef
PAR_DES_I16_T ParDes_CosPhiRef =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Sollwert Cos(phi)",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Cos(phi) reference",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32768,                                                             // Minimum
    32767,                                                              // Default
    (1.0/32767),                      // Faktor
    &ParVal_CosPhiRef,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable QMode
//#pragma DATA_SECTION(ParVal_QMode,".extram");
WORD ParVal_QMode;

// Q Mode
// Parameter-Beschreibung QMode
PAR_DES_WORD_T ParDes_QMode =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "Modus Q",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Q Mode",                               // Name
#endif
#endif
    0x0005,                                             // Maximum
    0x0000,                                                             // Minimum
    1,                                                              // Default
    &ParVal_QMode,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


WORD ParVal_FRT_Vmode;
PAR_DES_WORD_T ParDes_FRT_Vmode =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,
    "FRT V mode",                               // Name
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_FRT_Vmode,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


WORD ParVal_PMode;

PAR_DES_WORD_T ParDes_PMode =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "P Modus",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Active power mode",                                // Name
#endif
#endif
    0x0002,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_PMode,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonics
//#pragma DATA_SECTION(ParVal_Harmonics1,".extram");
//INT16 ParVal_Harmonics1;

PAR_DES_I16_T ParDes_Harmonics1 =
{
    PAR_MENUE | PAR_RW | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Harmonische 1-3",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonics 1-3",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_Harmonics1*/,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonics
//#pragma DATA_SECTION(ParVal_Harmonics2,".extram");
//INT16 ParVal_Harmonics2;

PAR_DES_I16_T ParDes_Harmonics2 =
{
    PAR_MENUE | PAR_RW | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Harmonische 4-6",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonics 4-6",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_Harmonics2*/,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonics
//#pragma DATA_SECTION(ParVal_Harmonics3,".extram");
//INT16 ParVal_Harmonics3;

PAR_DES_I16_T ParDes_Harmonics3 =
{
    PAR_MENUE | PAR_RW | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Harmonische 7-8",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonics 7-8",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_Harmonics3*/,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic1_N
//#pragma DATA_SECTION(ParVal_Harmonic1_N,".extram");
INT16 ParVal_Harmonic1_N;

// Order of harmonic 1
// Parameter-Beschreibung Harmonic1_N
PAR_DES_I16_T ParDes_Harmonic1_N =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Ordnung Harmonische 1",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Order of harmonic 1",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    150*128,                                                // Maximum
    -150*128,                                                               // Minimum
    -5*128,                                                             // Default
    (1.0/128.0),                      // Faktor
    &ParVal_Harmonic1_N,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic1_D
//#pragma DATA_SECTION(ParVal_Harmonic1_D,".extram");
INT16 ParVal_Harmonic1_D;

// D component of harmonic 1
// Parameter-Beschreibung Harmonic1_D
PAR_DES_I16_T ParDes_Harmonic1_D =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Amplitude Harmonische 1",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Amplitude of harmonic 1",                              // Name
#endif
#endif
    "A",                                                            // Einheit
    5000,                                               // Maximum
    -5000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Harmonic1_D,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic1_Q
//#pragma DATA_SECTION(ParVal_Harmonic1_Q,".extram");
INT16 ParVal_Harmonic1_Q;

// Q component of harmonic 1
// Parameter-Beschreibung Harmonic1_Q
PAR_DES_I16_T ParDes_Harmonic1_Q =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Phase Harmonische 1",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Phase of harmonic 1",                              // Name
#endif
#endif
    "deg",                                                          // Einheit
    3600,                                               // Maximum
    -3600,                                                              // Minimum
    0,                                                              // Default
    (0.1),                    // Faktor
    &ParVal_Harmonic1_Q,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic1_M
//#pragma DATA_SECTION(ParVal_Harmonic1_M,".extram");
INT16 ParVal_Harmonic1_M;

// Max of harmonic 1
// Parameter-Beschreibung Harmonic1_M
PAR_DES_I16_T ParDes_Harmonic1_M =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Maximum Harmonische 1",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Maximum of harmonic 1",                                // Name
#endif
#endif
    "A",                                                            // Einheit
    10000,                                              // Maximum
    -10000,                                                             // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Harmonic1_M,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic2_N
//#pragma DATA_SECTION(ParVal_Harmonic2_N,".extram");
INT16 ParVal_Harmonic2_N;

// Order of harmonic 2
// Parameter-Beschreibung Harmonic2_N
PAR_DES_I16_T ParDes_Harmonic2_N =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Ordnung Harmonische 2",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Order of harmonic 2",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    150*128,                                                // Maximum
    -150*128,                                                               // Minimum
    7*128,                                                              // Default
    (1.0/128.0),                      // Faktor
    &ParVal_Harmonic2_N,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic2_D
//#pragma DATA_SECTION(ParVal_Harmonic2_D,".extram");
INT16 ParVal_Harmonic2_D;

// D component of harmonic 2
// Parameter-Beschreibung Harmonic2_D
PAR_DES_I16_T ParDes_Harmonic2_D =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Amplitude Harmonische 2",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Amplitude of harmonic 2",                              // Name
#endif
#endif
    "A",                                                            // Einheit
    5000,                                               // Maximum
    -5000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Harmonic2_D,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic2_Q
//#pragma DATA_SECTION(ParVal_Harmonic2_Q,".extram");
INT16 ParVal_Harmonic2_Q;

// Q component of harmonic 2
// Parameter-Beschreibung Harmonic2_Q
PAR_DES_I16_T ParDes_Harmonic2_Q =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Phase Harmonische 2",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Phase of harmonic 2",                              // Name
#endif
#endif
    "deg",                                                          // Einheit
    3600,                                               // Maximum
    -3600,                                                              // Minimum
    0,                                                              // Default
    (0.1),                    // Faktor
    &ParVal_Harmonic2_Q,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic2_M
//#pragma DATA_SECTION(ParVal_Harmonic2_M,".extram");
INT16 ParVal_Harmonic2_M;

// Max of harmonic 2
// Parameter-Beschreibung Harmonic2M
PAR_DES_I16_T ParDes_Harmonic2_M =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Maximum Harmonische 2",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Maximum of harmonic 2",                                // Name
#endif
#endif
    "A",                                                            // Einheit
    5000,                                               // Maximum
    -5000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Harmonic2_M,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic4_N
//#pragma DATA_SECTION(ParVal_Harmonic4_N,".extram");
INT16 ParVal_Harmonic4_N;

// Order of harmonic 4
// Parameter-Beschreibung Harmonic4_N
PAR_DES_I16_T ParDes_Harmonic4_N =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Ordnung Harmonische 4",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Order of harmonic 4",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    150*128,                                                // Maximum
    -150*128,                                                               // Minimum
    13*128,                                                             // Default
    (1.0/128.0),                      // Faktor
    &ParVal_Harmonic4_N,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic4_D
//#pragma DATA_SECTION(ParVal_Harmonic4_D,".extram");
INT16 ParVal_Harmonic4_D;

// D component of harmonic 4
// Parameter-Beschreibung Harmonic4_D
PAR_DES_I16_T ParDes_Harmonic4_D =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Amplitude Harmonische 4",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Amplitude of harmonic 4",                              // Name
#endif
#endif
    "A",                                                            // Einheit
    5000,                                               // Maximum
    -5000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Harmonic4_D,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic4_Q
//#pragma DATA_SECTION(ParVal_Harmonic4_Q,".extram");
INT16 ParVal_Harmonic4_Q;

// Q component of harmonic 4
// Parameter-Beschreibung Harmonic4_Q
PAR_DES_I16_T ParDes_Harmonic4_Q =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Phase Harmonische 4",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Phase of harmonic 4",                              // Name
#endif
#endif
    "deg",                                                          // Einheit
    3600,                                               // Maximum
    -3600,                                                              // Minimum
    0,                                                              // Default
    (0.1),                    // Faktor
    &ParVal_Harmonic4_Q,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic4_M
//#pragma DATA_SECTION(ParVal_Harmonic4_M,".extram");
INT16 ParVal_Harmonic4_M;

// Max of harmonic 4
// Parameter-Beschreibung Harmonic4_M
PAR_DES_I16_T ParDes_Harmonic4_M =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Maximum Harmonische 4",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Maximum of harmonic 4",                                // Name
#endif
#endif
    "A",                                                            // Einheit
    5000,                                               // Maximum
    -5000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Harmonic4_M,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic3_N
//#pragma DATA_SECTION(ParVal_Harmonic3_N,".extram");
INT16 ParVal_Harmonic3_N;

// Order of harmonic 3
// Parameter-Beschreibung Harmonic3_N
PAR_DES_I16_T ParDes_Harmonic3_N =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Ordnung Harmonische 3",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Order of harmonic 3",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    150*128,                                                // Maximum
    -150*128,                                                               // Minimum
    -11*128,                                                                // Default
    (1.0/128.0),                      // Faktor
    &ParVal_Harmonic3_N,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic3_D
//#pragma DATA_SECTION(ParVal_Harmonic3_D,".extram");
INT16 ParVal_Harmonic3_D;

// D component of harmonic 3
// Parameter-Beschreibung Harmonic3_D
PAR_DES_I16_T ParDes_Harmonic3_D =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Amplitude Harmonische 3",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Amplitude of harmonic 3",                              // Name
#endif
#endif
    "A",                                                            // Einheit
    5000,                                               // Maximum
    -5000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Harmonic3_D,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic3_Q
//#pragma DATA_SECTION(ParVal_Harmonic3_Q,".extram");
INT16 ParVal_Harmonic3_Q;

// Q component of harmonic 3
// Parameter-Beschreibung Harmonic3_Q
PAR_DES_I16_T ParDes_Harmonic3_Q =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Phase Harmonische 3",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Phase of harmonic 3",                              // Name
#endif
#endif
    "deg",                                                          // Einheit
    3600,                                               // Maximum
    -3600,                                                              // Minimum
    0,                                                              // Default
    (0.1),                    // Faktor
    &ParVal_Harmonic3_Q,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic3_M
//#pragma DATA_SECTION(ParVal_Harmonic3_M,".extram");
INT16 ParVal_Harmonic3_M;

// Max of harmonic 3
// Parameter-Beschreibung Harmonic3_M
PAR_DES_I16_T ParDes_Harmonic3_M =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Maximum Harmonische 3",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Maximum of harmonic 3",                                // Name
#endif
#endif
    "A",                                                            // Einheit
    5000,                                               // Maximum
    -5000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Harmonic3_M,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic5_N
//#pragma DATA_SECTION(ParVal_Harmonic5_N,".extram");
INT16 ParVal_Harmonic5_N;

// Order of harmonic 5
// Parameter-Beschreibung Harmonic5_N
PAR_DES_I16_T ParDes_Harmonic5_N =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Ordnung Harmonische 5",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Order of harmonic 5",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    150*128,                                                // Maximum
    -150*128,                                                               // Minimum
    -17*128,                                                                // Default
    (1.0/128.0),                      // Faktor
    &ParVal_Harmonic5_N,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic5_D
//#pragma DATA_SECTION(ParVal_Harmonic5_D,".extram");
INT16 ParVal_Harmonic5_D;

// D component of harmonic 5
// Parameter-Beschreibung Harmonic5_D
PAR_DES_I16_T ParDes_Harmonic5_D =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Amplitude Harmonische 5",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Amplitude of harmonic 5",                              // Name
#endif
#endif
    "A",                                                            // Einheit
    5000,                                               // Maximum
    -5000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Harmonic5_D,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic5_Q
//#pragma DATA_SECTION(ParVal_Harmonic5_Q,".extram");
INT16 ParVal_Harmonic5_Q;

// Q component of harmonic 5
// Parameter-Beschreibung Harmonic5_Q
PAR_DES_I16_T ParDes_Harmonic5_Q =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Phase Harmonische 5",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Phase of harmonic 5",                              // Name
#endif
#endif
    "deg",                                                          // Einheit
    3600,                                               // Maximum
    -3600,                                                              // Minimum
    0,                                                              // Default
    (0.1),                    // Faktor
    &ParVal_Harmonic5_Q,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic5_M
//#pragma DATA_SECTION(ParVal_Harmonic5_M,".extram");
INT16 ParVal_Harmonic5_M;

// Max of harmonic 5
// Parameter-Beschreibung Harmonic5_M
PAR_DES_I16_T ParDes_Harmonic5_M =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Maximum Harmonische 5",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Maximum of harmonic 5",                                // Name
#endif
#endif
    "A",                                                            // Einheit
    5000,                                               // Maximum
    -5000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Harmonic5_M,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic6_N
//#pragma DATA_SECTION(ParVal_Harmonic6_N,".extram");
INT16 ParVal_Harmonic6_N;

// Order of harmonic 6
// Parameter-Beschreibung Harmonic6_N
PAR_DES_I16_T ParDes_Harmonic6_N =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Ordnung Harmonische 6",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Order of harmonic 6",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    150*128,                                                // Maximum
    -150*128,                                                               // Minimum
    19*128,                                                             // Default
    (1.0/128.0),                      // Faktor
    &ParVal_Harmonic6_N,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic6_D
//#pragma DATA_SECTION(ParVal_Harmonic6_D,".extram");
INT16 ParVal_Harmonic6_D;

// D component of harmonic 6
// Parameter-Beschreibung Harmonic6_D
PAR_DES_I16_T ParDes_Harmonic6_D =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Amplitude Harmonische 6",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Amplitude of harmonic 6",                              // Name
#endif
#endif
    "A",                                                            // Einheit
    5000,                                               // Maximum
    -5000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Harmonic6_D,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic6_Q
//#pragma DATA_SECTION(ParVal_Harmonic6_Q,".extram");
INT16 ParVal_Harmonic6_Q;

// Q component of harmonic 6
// Parameter-Beschreibung Harmonic6_Q
PAR_DES_I16_T ParDes_Harmonic6_Q =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Phase Harmonische 6",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Phase of harmonic 6",                              // Name
#endif
#endif
    "deg",                                                          // Einheit
    3600,                                               // Maximum
    -3600,                                                              // Minimum
    0,                                                              // Default
    (0.1),                    // Faktor
    &ParVal_Harmonic6_Q,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic6_M
//#pragma DATA_SECTION(ParVal_Harmonic6_M,".extram");
INT16 ParVal_Harmonic6_M;

// Max of harmonic 6
// Parameter-Beschreibung Harmonic6_M
PAR_DES_I16_T ParDes_Harmonic6_M =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Maximum Harmonische 6",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Maximum of harmonic 6",                                // Name
#endif
#endif
    "A",                                                            // Einheit
    5000,                                               // Maximum
    -5000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Harmonic6_M,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic7_N
//#pragma DATA_SECTION(ParVal_Harmonic7_N,".extram");
INT16 ParVal_Harmonic7_N;

// Order of harmonic 7
// Parameter-Beschreibung Harmonic7_N
PAR_DES_I16_T ParDes_Harmonic7_N =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Ordnung Harmonische 7",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Order of harmonic 7",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    150*128,                                                // Maximum
    -150*128,                                                               // Minimum
    -23*128,                                                                // Default
    (1.0/128.0),                      // Faktor
    &ParVal_Harmonic7_N,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic7_D
//#pragma DATA_SECTION(ParVal_Harmonic7_D,".extram");
INT16 ParVal_Harmonic7_D;

// D component of harmonic 7
// Parameter-Beschreibung Harmonic7_D
PAR_DES_I16_T ParDes_Harmonic7_D =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Amplitude Harmonische 7",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Amplitude of harmonic 7",                              // Name
#endif
#endif
    "A",                                                            // Einheit
    5000,                                               // Maximum
    -5000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Harmonic7_D,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic7_Q
//#pragma DATA_SECTION(ParVal_Harmonic7_Q,".extram");
INT16 ParVal_Harmonic7_Q;

// Q component of harmonic 7
// Parameter-Beschreibung Harmonic7_Q
PAR_DES_I16_T ParDes_Harmonic7_Q =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Phase Harmonische 7",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Phase of harmonic 7",                              // Name
#endif
#endif
    "deg",                                                          // Einheit
    3600,                                               // Maximum
    -3600,                                                              // Minimum
    0,                                                              // Default
    (0.1),                    // Faktor
    &ParVal_Harmonic7_Q,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic7_M
//#pragma DATA_SECTION(ParVal_Harmonic7_M,".extram");
INT16 ParVal_Harmonic7_M;

// Max of harmonic 7
// Parameter-Beschreibung Harmonic7_M
PAR_DES_I16_T ParDes_Harmonic7_M =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Maximum Harmonische 7",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Maximum of harmonic 7",                                // Name
#endif
#endif
    "A",                                                            // Einheit
    5000,                                               // Maximum
    -5000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Harmonic7_M,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic8_N
//#pragma DATA_SECTION(ParVal_Harmonic8_N,".extram");
INT16 ParVal_Harmonic8_N;

// Order of harmonic 8
// Parameter-Beschreibung Harmonic8_N
PAR_DES_I16_T ParDes_Harmonic8_N =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Ordnung Harmonische 8",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Order of harmonic 8",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    150*128,                                                // Maximum
    -150*128,                                                               // Minimum
    25*128,                                                             // Default
    (1.0/128.0),                      // Faktor
    &ParVal_Harmonic8_N,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic8_D
//#pragma DATA_SECTION(ParVal_Harmonic8_D,".extram");
INT16 ParVal_Harmonic8_D;

// D component of harmonic 8
// Parameter-Beschreibung Harmonic8_D
PAR_DES_I16_T ParDes_Harmonic8_D =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Amplitude Harmonische 8",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Amplitude of harmonic 8",                              // Name
#endif
#endif
    "A",                                                            // Einheit
    5000,                                               // Maximum
    -5000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Harmonic8_D,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic8_Q
//#pragma DATA_SECTION(ParVal_Harmonic8_Q,".extram");
INT16 ParVal_Harmonic8_Q;

// Q component of harmonic 8
// Parameter-Beschreibung Harmonic8_Q
PAR_DES_I16_T ParDes_Harmonic8_Q =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Phase Harmonische 8",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Phase of harmonic 8",                              // Name
#endif
#endif
    "deg",                                                          // Einheit
    3600,                                               // Maximum
    -3600,                                                              // Minimum
    0,                                                              // Default
    (0.1),                    // Faktor
    &ParVal_Harmonic8_Q,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Harmonic8_M
//#pragma DATA_SECTION(ParVal_Harmonic8_M,".extram");
INT16 ParVal_Harmonic8_M;

// Max of harmonic 8
// Parameter-Beschreibung Harmonic8_M
PAR_DES_I16_T ParDes_Harmonic8_M =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Maximum Harmonische 8",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Maximum of harmonic 8",                                // Name
#endif
#endif
    "A",                                                            // Einheit
    5000,                                               // Maximum
    -5000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Harmonic8_M,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Harmonic1_Vm,".extram");
INT16 ParVal_Harmonic1_Vm;

PAR_DES_I16_T ParDes_Harmonic1_Vm =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Max. Spannung Harmonische 1",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Max. voltage harmonic 1",                              // Name
#endif
#endif
    "V",                                                            // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_Harmonic1_Vm,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Harmonic2_Vm,".extram");
INT16 ParVal_Harmonic2_Vm;

PAR_DES_I16_T ParDes_Harmonic2_Vm =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Max. Spannung Harmonische 2",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Max. voltage harmonic 2",                              // Name
#endif
#endif
    "V",                                                            // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_Harmonic2_Vm,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Harmonic3_Vm,".extram");
INT16 ParVal_Harmonic3_Vm;

PAR_DES_I16_T ParDes_Harmonic3_Vm =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Max. Spannung Harmonische 3",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Max. voltage harmonic 3",                              // Name
#endif
#endif
    "V",                                                            // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_Harmonic3_Vm,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Harmonic4_Vm,".extram");
INT16 ParVal_Harmonic4_Vm;

PAR_DES_I16_T ParDes_Harmonic4_Vm =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Max. Spannung Harmonische 4",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Max. voltage harmonic 4",                              // Name
#endif
#endif
    "V",                                                            // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_Harmonic4_Vm,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Harmonic5_Vm,".extram");
INT16 ParVal_Harmonic5_Vm;

PAR_DES_I16_T ParDes_Harmonic5_Vm =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Max. Spannung Harmonische 5",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Max. voltage harmonic 5",                              // Name
#endif
#endif
    "V",                                                            // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_Harmonic5_Vm,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Harmonic6_Vm,".extram");
INT16 ParVal_Harmonic6_Vm;

PAR_DES_I16_T ParDes_Harmonic6_Vm =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Max. Spannung Harmonische 6",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Max. voltage harmonic 6",                              // Name
#endif
#endif
    "V",                                                            // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_Harmonic6_Vm,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Harmonic7_Vm,".extram");
INT16 ParVal_Harmonic7_Vm;

PAR_DES_I16_T ParDes_Harmonic7_Vm =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Max. Spannung Harmonische 7",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Max. voltage harmonic 7",                              // Name
#endif
#endif
    "V",                                                            // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_Harmonic7_Vm,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Harmonic8_Vm,".extram");
INT16 ParVal_Harmonic8_Vm;

PAR_DES_I16_T ParDes_Harmonic8_Vm =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Max. Spannung Harmonische 8",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Max. voltage harmonic 8",                              // Name
#endif
#endif
    "V",                                                            // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_Harmonic8_Vm,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable EnableHarm                                    xxxxxxx
//#pragma DATA_SECTION(ParVal_EnableHarm,".extram");
INT16 ParVal_EnableHarm;

// Enable harmonic control
// Parameter-Beschreibung EnableHarm
PAR_DES_I16_T ParDes_EnableHarm =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Freigabe Harmonische",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Enable harmonics control",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_EnableHarm,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_VP_phi,".extram");
INT32 ParVal_VP_phi;

PAR_DES_I32_T ParDes_VP_phi =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "P-Verstärkung cos(phi) control",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "P-Gain power factor control",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    10.0*65536,                                             // Maximum
    -10.0*65536,                                                                // Minimum
    1.0*65536,                                                              // Default
    1.0/65536,                    // Faktor
    &ParVal_VP_phi,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_VI_phi,".extram");
INT32 ParVal_VI_phi;

PAR_DES_I32_T ParDes_VI_phi =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "I-Verstärkung cos(phi) control",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "I-Gain power factor control",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    0,                                                              // Minimum
    0.05*65536,                                                             // Default
    1.0/65536,                    // Faktor
    &ParVal_VI_phi,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_VP_overload,".extram");
INT32 ParVal_VP_overload;

PAR_DES_I32_T ParDes_VP_overload =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "P-Verstärkung overload control",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "P-Gain overload control",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    10.0*65536,                                             // Maximum
    -10.0*65536,                                                                // Minimum
    1.0*65536,                                                              // Default
    1.0/65536,                    // Faktor
    &ParVal_VP_overload,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_VI_overload,".extram");
INT32 ParVal_VI_overload;

PAR_DES_I32_T ParDes_VI_overload =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "I-Verstärkung overload control",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "I-Gain overload control",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    0,                                                              // Minimum
    0.05*65536,                                                             // Default
    1.0/65536,                    // Faktor
    &ParVal_VI_overload,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_VI_u,".extram");
INT32 ParVal_VI_u;

PAR_DES_I32_T ParDes_VI_u =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "I-Verstärkung P-regler",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "I-gain P control",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    0,                                                              // Minimum
    0.05*65536,                                                             // Default
    1.0/65536,                    // Faktor
    &ParVal_VI_u,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_VP_u,".extram");
INT32 ParVal_VP_u;

PAR_DES_I32_T ParDes_VP_u =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "P-Verstärkung P-Regler",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "P-gain P control",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    100.0*65536,                                                // Maximum
    0,                                                              // Minimum
    1.0*65536,                                                              // Default
    1.0/65536,                    // Faktor
    &ParVal_VP_u,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_VI_i,".extram");
INT32 ParVal_VI_i;

PAR_DES_I32_T ParDes_VI_i =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "I-Verstärkung Stromregler",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "I-gain current control",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    0,                                                              // Minimum
    0.05*65536,                                                             // Default
    1.0/65536,                    // Faktor
    &ParVal_VI_i,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT32 ParVal_VI_i0;
PAR_DES_I32_T ParDes_VI_i0 =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
    "I-gain i0 control",                                // Name
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    0,                                                              // Minimum
    0.05*65536,                                                             // Default
    1.0/65536,                    // Faktor
    &ParVal_VI_i0,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_K_fil,".extram");
INT16 ParVal_K_fil;

PAR_DES_I16_T ParDes_K_fil =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Netzfilter K",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Line filter K",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    100.0*256,                                              // Maximum
    -100.0*256,                                                             // Minimum
    0,                                                              // Default
    1.0/256,                      // Faktor
    &ParVal_K_fil,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_D_fil,".extram");
INT16 ParVal_D_fil;

PAR_DES_I16_T ParDes_D_fil =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Netzfilter D",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Line filter D",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    100.0*256,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0/256,                      // Faktor
    &ParVal_D_fil,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_F_fil,".extram");
INT16 ParVal_F_fil;

PAR_DES_I16_T ParDes_F_fil =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Netzfilter F0",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Line filter F0",                               // Name
#endif
#endif
    "Hz",                                                           // Einheit
    10000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_F_fil,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_kv_HarmLim,".extram");
INT32 ParVal_kv_HarmLim;

PAR_DES_I32_T ParDes_kv_HarmLim =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Harm. Spannungsgrenze",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harm. overmodulation",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    2.0*65536,                                              // Maximum
    0,                                                              // Minimum
    1.0*65536,                                                              // Default
    1.0/65536,                    // Faktor
    &ParVal_kv_HarmLim,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_VP_i,".extram");
INT32 ParVal_VP_i;

PAR_DES_I32_T ParDes_VP_i =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "P-Verstärkung Stromregler",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "P-gain current control",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    100.0*65536,                                                // Maximum
    0,                                                              // Minimum
    1.0*65536,                                                              // Default
    1.0/65536,                    // Faktor
    &ParVal_VP_i,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT32 ParVal_VP_i0;
PAR_DES_I32_T ParDes_VP_i0 =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
    "P-gain i0 control",                                // Name
    NULL,                                                           // Einheit
    100.0*65536,                                                // Maximum
    0,                                                              // Minimum
    1.0*65536,                                                              // Default
    1.0/65536,                    // Faktor
    &ParVal_VP_i0,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_VIi,".extram");
INT32 ParVal_VIi;

PAR_DES_I32_T ParDes_VIi =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Current I-gain",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Current I-gain",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    0,                                                              // Minimum
    0.05*65536,                                                             // Default
    1.0/65536,                    // Faktor
    &ParVal_VIi,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


INT16 ParVal_Tf_inref;
PAR_DES_I16_T ParDes_Tf_inref =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                   // ReadOnly
    "Tf inref",                             // Name
    "ms",                                                           // Einheit
    100,                                                // Maximum
    1,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_Tf_inref,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_aM,".extram");
INT32 ParVal_aM;

PAR_DES_I32_T ParDes_aM =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Current closed loop",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Current closed loop",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    0,                                                              // Minimum
    0*65536,                                                                // Default
    1.0/65536,                    // Faktor
    &ParVal_aM,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_z1Vdc,".extram");
INT32 ParVal_z1Vdc;

PAR_DES_I32_T ParDes_z1Vdc =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Vdc closed loop",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Vdc closed loop",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    0,                                                              // Minimum
    0*65536,                                                                // Default
    1.0/65536,                    // Faktor
    &ParVal_z1Vdc,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};



//#pragma DATA_SECTION(ParVal_Harmonic1_V1,".extram");
INT32 ParVal_Harmonic1_V1;

PAR_DES_I32_T ParDes_Harmonic1_V1 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Verstärkung 1 Harmonische 1",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Gain 1 harmonic 1",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    100.0*65536,                                                // Maximum
    -100.0*65536,                                                               // Minimum
    1.0*65536,                                                              // Default
    1.0/65536,                    // Faktor
    &ParVal_Harmonic1_V1,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_Harmonic1_V2,".extram");
INT32 ParVal_Harmonic1_V2;

PAR_DES_I32_T ParDes_Harmonic1_V2 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Verstärkung 2 Harmonische 1",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Phase correction I harm 1",                                // Name
#endif
#endif
    "deg",                                                          // Einheit
    1800*65536,                                             // Maximum
    -1800*65536,                                                                // Minimum
    0,                                                              // Default
    0.1/65536,                    // Faktor
    &ParVal_Harmonic1_V2,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Harmonic1_Fb,".extram");
WORD ParVal_Harmonic1_Fb;

PAR_DES_WORD_T ParDes_Harmonic1_Fb =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "Regelmodus Harmonische 1",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Control mode harmonic 1",                              // Name
#endif
#endif
    0x0005,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_Harmonic1_Fb,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Harmonic2_V1,".extram");
INT32 ParVal_Harmonic2_V1;

PAR_DES_I32_T ParDes_Harmonic2_V1 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Verstärkung 1 Harmonische 2",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Gain 1 harmonic 2",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    -1.0*65536,                                                             // Minimum
    1.0*65536,                                                              // Default
    1.0/65536,                    // Faktor
    &ParVal_Harmonic2_V1,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Harmonic2_V2,".extram");
INT32 ParVal_Harmonic2_V2;

PAR_DES_I32_T ParDes_Harmonic2_V2 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Verstärkung 2 Harmonische 2",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Phase correction I harm 2",                                // Name
#endif
#endif
    "deg",                                                          // Einheit
    1800*65536,                                             // Maximum
    -1800*65536,                                                                // Minimum
    0,                                                              // Default
    0.1/65536,                    // Faktor
    &ParVal_Harmonic2_V2,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Harmonic2_Fb,".extram");
WORD ParVal_Harmonic2_Fb;

PAR_DES_WORD_T ParDes_Harmonic2_Fb =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "Regelmodus Harmonische 2",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Control mode harmonic 2",                              // Name
#endif
#endif
    0x0005,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_Harmonic2_Fb,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Harmonic3_V1,".extram");
//#pragma DATA_SECTION(ParVal_Harmonic3_V2,".extram");
//#pragma DATA_SECTION(ParVal_Harmonic3_Fb,".extram");
INT32 ParVal_Harmonic3_V1;

PAR_DES_I32_T ParDes_Harmonic3_V1 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Verstärkung 1 Harmonische 3",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Gain 1 harmonic 3",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    -1.0*65536,                                                             // Minimum
    1.0*65536,                                                              // Default
    1.0/65536,                    // Faktor
    &ParVal_Harmonic3_V1,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT32 ParVal_Harmonic3_V2;

PAR_DES_I32_T ParDes_Harmonic3_V2 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Verstärkung 2 Harmonische 3",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Phase correction I harm 3",                                // Name
#endif
#endif
    "deg",                                                          // Einheit
    1800*65536,                                             // Maximum
    -1800*65536,                                                                // Minimum
    0,                                                              // Default
    0.1/65536,                    // Faktor
    &ParVal_Harmonic3_V2,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

WORD ParVal_Harmonic3_Fb;

PAR_DES_WORD_T ParDes_Harmonic3_Fb =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "Regelmodus Harmonische 3",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Control mode harmonic 3",                              // Name
#endif
#endif
    0x0005,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_Harmonic3_Fb,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Harmonic4_V1,".extram");
//#pragma DATA_SECTION(ParVal_Harmonic4_V2,".extram");
//#pragma DATA_SECTION(ParVal_Harmonic4_Fb,".extram");
INT32 ParVal_Harmonic4_V1;

PAR_DES_I32_T ParDes_Harmonic4_V1 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Verstärkung 1 Harmonische 4",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Gain 1 harmonic 4",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    -1.0*65536,                                                             // Minimum
    1.0*65536,                                                              // Default
    1.0/65536,                    // Faktor
    &ParVal_Harmonic4_V1,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT32 ParVal_Harmonic4_V2;

PAR_DES_I32_T ParDes_Harmonic4_V2 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Verstärkung 2 Harmonische 4",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Phase correction I harm 4",                                // Name
#endif
#endif
    "deg",                                                          // Einheit
    1800*65536,                                             // Maximum
    -1800*65536,                                                                // Minimum
    0,                                                              // Default
    0.1/65536,                    // Faktor
    &ParVal_Harmonic4_V2,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

WORD ParVal_Harmonic4_Fb;

PAR_DES_WORD_T ParDes_Harmonic4_Fb =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "Regelmodus Harmonische 4",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Control mode harmonic 4",                              // Name
#endif
#endif
    0x0005,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_Harmonic4_Fb,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Harmonic5_V1,".extram");
//#pragma DATA_SECTION(ParVal_Harmonic5_V2,".extram");
//#pragma DATA_SECTION(ParVal_Harmonic5_Fb,".extram");
INT32 ParVal_Harmonic5_V1;

PAR_DES_I32_T ParDes_Harmonic5_V1 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Verstärkung 1 Harmonische 5",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Gain 1 harmonic 5",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    -1.0*65536,                                                             // Minimum
    1.0*65536,                                                              // Default
    1.0/65536,                    // Faktor
    &ParVal_Harmonic5_V1,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT32 ParVal_Harmonic5_V2;

PAR_DES_I32_T ParDes_Harmonic5_V2 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Verstärkung 2 Harmonische 5",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Phase correction I harm 5",                                // Name
#endif
#endif
    "deg",                                                          // Einheit
    1800*65536,                                             // Maximum
    -1800*65536,                                                                // Minimum
    0,                                                              // Default
    0.1/65536,                    // Faktor
    &ParVal_Harmonic5_V2,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

WORD ParVal_Harmonic5_Fb;

PAR_DES_WORD_T ParDes_Harmonic5_Fb =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "Regelmodus Harmonische 5",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Control mode harmonic 5",                              // Name
#endif
#endif
    0x0005,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_Harmonic5_Fb,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Harmonic6_V1,".extram");
//#pragma DATA_SECTION(ParVal_Harmonic6_V2,".extram");
//#pragma DATA_SECTION(ParVal_Harmonic6_Fb,".extram");
INT32 ParVal_Harmonic6_V1;

PAR_DES_I32_T ParDes_Harmonic6_V1 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Verstärkung 1 Harmonische 6",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Gain 1 harmonic 6",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    -1.0*65536,                                                             // Minimum
    1.0*65536,                                                              // Default
    1.0/65536,                    // Faktor
    &ParVal_Harmonic6_V1,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT32 ParVal_Harmonic6_V2;

PAR_DES_I32_T ParDes_Harmonic6_V2 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Verstärkung 2 Harmonische 6",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Phase correction I harm 6",                                // Name
#endif
#endif
    "deg",                                                          // Einheit
    1800*65536,                                             // Maximum
    -1800*65536,                                                                // Minimum
    0,                                                              // Default
    0.1/65536,                    // Faktor
    &ParVal_Harmonic6_V2,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

WORD ParVal_Harmonic6_Fb;

PAR_DES_WORD_T ParDes_Harmonic6_Fb =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "Regelmodus Harmonische 6",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Control mode harmonic 6",                              // Name
#endif
#endif
    0x0005,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_Harmonic6_Fb,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Harmonic7_V1,".extram");
//#pragma DATA_SECTION(ParVal_Harmonic7_V2,".extram");
//#pragma DATA_SECTION(ParVal_Harmonic7_Fb,".extram");
INT32 ParVal_Harmonic7_V1;

PAR_DES_I32_T ParDes_Harmonic7_V1 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Verstärkung 1 Harmonische 7",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Gain 1 harmonic 7",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    -1.0*65536,                                                             // Minimum
    1.0*65536,                                                              // Default
    1.0/65536,                    // Faktor
    &ParVal_Harmonic7_V1,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT32 ParVal_Harmonic7_V2;

PAR_DES_I32_T ParDes_Harmonic7_V2 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Verstärkung 2 Harmonische 7",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Phase correction I harm 7",                                // Name
#endif
#endif
    "deg",                                                          // Einheit
    1800*65536,                                             // Maximum
    -1800*65536,                                                                // Minimum
    0,                                                              // Default
    0.1/65536,                    // Faktor
    &ParVal_Harmonic7_V2,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

WORD ParVal_Harmonic7_Fb;

PAR_DES_WORD_T ParDes_Harmonic7_Fb =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "Regelmodus Harmonische 7",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Control mode harmonic 7",                              // Name
#endif
#endif
    0x0005,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_Harmonic7_Fb,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Harmonic8_V1,".extram");
//#pragma DATA_SECTION(ParVal_Harmonic8_V2,".extram");
//#pragma DATA_SECTION(ParVal_Harmonic8_Fb,".extram");
INT32 ParVal_Harmonic8_V1;

PAR_DES_I32_T ParDes_Harmonic8_V1 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Verstärkung 1 Harmonische 8",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Gain 1 harmonic 8",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    -1.0*65536,                                                             // Minimum
    1.0*65536,                                                              // Default
    1.0/65536,                    // Faktor
    &ParVal_Harmonic8_V1,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT32 ParVal_Harmonic8_V2;

PAR_DES_I32_T ParDes_Harmonic8_V2 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Verstärkung 2 Harmonische 8",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Phase correction I harm 8",                                // Name
#endif
#endif
    "deg",                                                          // Einheit
    1800*65536,                                             // Maximum
    -1800*65536,                                                                // Minimum
    0,                                                              // Default
    0.1/65536,                    // Faktor
    &ParVal_Harmonic8_V2,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

WORD ParVal_Harmonic8_Fb;

PAR_DES_WORD_T ParDes_Harmonic8_Fb =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "Regelmodus Harmonische 8",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Control mode harmonic 8",                              // Name
#endif
#endif
    0x0005,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_Harmonic8_Fb,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Harmonic1_phi,".extram");
//#pragma DATA_SECTION(ParVal_Harmonic2_phi,".extram");
//#pragma DATA_SECTION(ParVal_Harmonic3_phi,".extram");
//#pragma DATA_SECTION(ParVal_Harmonic4_phi,".extram");
//#pragma DATA_SECTION(ParVal_Harmonic5_phi,".extram");
//#pragma DATA_SECTION(ParVal_Harmonic6_phi,".extram");
//#pragma DATA_SECTION(ParVal_Harmonic7_phi,".extram");
//#pragma DATA_SECTION(ParVal_Harmonic8_phi,".extram");
INT16 ParVal_Harmonic1_phi;

PAR_DES_I16_T ParDes_Harmonic1_phi =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Phasenkorrektur Harm. 1",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Phase correction harm 1",                              // Name
#endif
#endif
    "deg",                                                          // Einheit
    1800,                                               // Maximum
    -1800,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Harmonic1_phi,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_Harmonic2_phi;

PAR_DES_I16_T ParDes_Harmonic2_phi =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Phasenkorrektur Harm. 2",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Phase correction harm 2",                              // Name
#endif
#endif
    "deg",                                                          // Einheit
    1800,                                               // Maximum
    -1800,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Harmonic2_phi,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_Harmonic3_phi;

PAR_DES_I16_T ParDes_Harmonic3_phi =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Phasenkorrektur Harm. 3",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Phase correction harm 3",                              // Name
#endif
#endif
    "deg",                                                          // Einheit
    1800,                                               // Maximum
    -1800,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Harmonic3_phi,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_Harmonic4_phi;

PAR_DES_I16_T ParDes_Harmonic4_phi =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Phasenkorrektur Harm. 4",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Phase correction harm 4",                              // Name
#endif
#endif
    "deg",                                                          // Einheit
    1800,                                               // Maximum
    -1800,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Harmonic4_phi,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_Harmonic5_phi;

PAR_DES_I16_T ParDes_Harmonic5_phi =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Phasenkorrektur Harm. 5",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Phase correction harm 5",                              // Name
#endif
#endif
    "deg",                                                          // Einheit
    1800,                                               // Maximum
    -1800,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Harmonic5_phi,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_Harmonic6_phi;

PAR_DES_I16_T ParDes_Harmonic6_phi =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Phasenkorrektur Harm. 6",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Phase correction harm 6",                              // Name
#endif
#endif
    "deg",                                                          // Einheit
    1800,                                               // Maximum
    -1800,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Harmonic6_phi,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_Harmonic7_phi;

PAR_DES_I16_T ParDes_Harmonic7_phi =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Phasenkorrektur Harm. 7",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Phase correction harm 7",                              // Name
#endif
#endif
    "deg",                                                          // Einheit
    1800,                                               // Maximum
    -1800,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Harmonic7_phi,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_Harmonic8_phi;

PAR_DES_I16_T ParDes_Harmonic8_phi =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Phasenkorrektur Harm. 8",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Phase correction harm 8",                              // Name
#endif
#endif
    "deg",                                                          // Einheit
    1800,                                               // Maximum
    -1800,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Harmonic8_phi,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_HarmonicMode,".extram");
WORD ParVal_HarmonicMode;

PAR_DES_WORD_T ParDes_HarmMode =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "Regelmodus Harmonische",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Control mode harmonics",                               // Name
#endif
#endif
    0x0002,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_HarmonicMode,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Harmonic_IGainInv,".extram");
INT32 ParVal_Harmonic_IGainInv;

PAR_DES_I32_T ParDes_Harmonic_IGainInv =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "I-Verst. Harm. Inverterstrom-Regler",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "I-gain harm. inverter current loop",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    -1.0*65536,                                                             // Minimum
    0,                                                              // Default
    1.0/65536,                    // Faktor
    &ParVal_Harmonic_IGainInv,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

// Parameter-Variable AnaRef1
//#pragma DATA_SECTION(ParVal_AnaRef1,".extram");
//#pragma DATA_SECTION(ParVal_AnaRef2,".extram");
//#pragma DATA_SECTION(ParVal_AnaRef3,".extram");
INT16 ParVal_AnaRef1;
// Parameter-Beschreibung Analog Reference 1
PAR_DES_I16_T ParDes_AnaRef1 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Analogsollwert 1",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Analog reference 1",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    0x7fff,                                             // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AnaRef1,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable AnaRef2
INT16 ParVal_AnaRef2;
// Parameter-Beschreibung Analog Reference 2
PAR_DES_I16_T ParDes_AnaRef2 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Analogsollwert 2",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Analog reference 2",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    0x7fff,                                             // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AnaRef2,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable AnaRef3
INT16 ParVal_AnaRef3;
// Parameter-Beschreibung Analog Reference 3
PAR_DES_I16_T ParDes_AnaRef3 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Analogsollwert 3",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Analog reference 3",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    0x7fff,                                             // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AnaRef3,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Tracevariablen

//INT16 ParVal_Trace;
PAR_DES_I16_T ParDes_Trace =
{
    PAR_MENUE | PAR_RW | _INT16,                    // ReadWrite, INT16-Parameter
    "Tracelist",                                                // Name
    NULL,                                                   // Einheit
    1,                                              // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    NULL/*&ParVal_Trace*/,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vLuv;
PAR_DES_I16_T ParDes_vLuv =
{
    PAR_PASS_PR | PAR_RO | _INT16,                  // ReadWrite, INT16-Parameter
    "vLuv",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vLuv,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vLwv;
PAR_DES_I16_T ParDes_vLwv =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vLwv",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vLwv,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vLu;
PAR_DES_I16_T ParDes_vLu =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vLu",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vLu,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vLv;
PAR_DES_I16_T ParDes_vLv =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vLv",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vLv,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vLw;
PAR_DES_I16_T ParDes_vLw =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vLw",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vLw,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vL_prefault;
PAR_DES_I16_T ParDes_vL_prefault =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vL_prefault",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vL_prefault,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_FRT_Iq_state;
PAR_DES_I16_T ParDes_FRT_Iq_state =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "FRT_Iq_state",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_FRT_Iq_state,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vinvu;
PAR_DES_I16_T ParDes_vinvu =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vinv_u",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vinvu,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vinvv;
PAR_DES_I16_T ParDes_vinvv =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vinv_v",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vinvv,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vinv_d;
PAR_DES_I16_T ParDes_vinv_d =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vinv_d",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vinv_d,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vinv_q;
PAR_DES_I16_T ParDes_vinv_q =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vinv_q",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vinv_q,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vLd;
PAR_DES_I16_T ParDes_vLd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vLd",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vLd,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vLq;
PAR_DES_I16_T ParDes_vLq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vLq",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vLq,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vLamp;
PAR_DES_I16_T ParDes_vLamp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vLamp",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vLamp,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vinv_amp;
PAR_DES_I16_T ParDes_vinv_amp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vinv_amp",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vinv_amp,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vDC;
PAR_DES_I16_T ParDes_vDC =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vDC",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vDC,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iInvu;
PAR_DES_I16_T ParDes_iInvu =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iInvu",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iInvu,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iInvv;
PAR_DES_I16_T ParDes_iInvv =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iInvv",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iInvv,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iInvw;
PAR_DES_I16_T ParDes_iInvw =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iInvw",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iInvw,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iInvd;
PAR_DES_I16_T ParDes_iInvd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iInvd",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iInvd,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iInvq;
PAR_DES_I16_T ParDes_iInvq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iInvq",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iInvq,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iInvpd;
PAR_DES_I16_T ParDes_iInvpd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iInvpd",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iInvpd,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iInvpq;
PAR_DES_I16_T ParDes_iInvpq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iInvpq",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iInvpq,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iInvnd;
PAR_DES_I16_T ParDes_iInvnd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iInvnd",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iInvnd,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iInvnq;
PAR_DES_I16_T ParDes_iInvnq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iInvnq",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iInvnq,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iInv0d;
PAR_DES_I16_T ParDes_iInv0d =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iInv0d",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iInv0d,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iInv0q;
PAR_DES_I16_T ParDes_iInv0q =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iInv0q",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iInv0q,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_is0d;
PAR_DES_I16_T ParDes_is0d =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "is0d",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_is0d,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_is0q;
PAR_DES_I16_T ParDes_is0q =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "is0q",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_is0q,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iL0d;
PAR_DES_I16_T ParDes_iL0d =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iL0d",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iL0d,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iL0q;
PAR_DES_I16_T ParDes_iL0q =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iL0q",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iL0q,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iInv_al;
PAR_DES_I16_T ParDes_iInv_al =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iInv_al",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iInv_al,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iInv_be;
PAR_DES_I16_T ParDes_iInv_be =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iInv_be",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iInv_be,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iInvamp;
PAR_DES_I16_T ParDes_iInvamp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iInvamp",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iInvamp,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iInvp_amp;
PAR_DES_I16_T ParDes_iInvp_amp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iInvp_amp",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iInvp_amp,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iInvn_amp;
PAR_DES_I16_T ParDes_iInvn_amp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iInvn_amp",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iInvn_amp,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_isu;
PAR_DES_I16_T ParDes_isu =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "isu",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_isu,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_isv;
PAR_DES_I16_T ParDes_isv =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "isv",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_isv,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_isw;
PAR_DES_I16_T ParDes_isw =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "isw",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_isw,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_isd;
PAR_DES_I16_T ParDes_isd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "isd",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_isd,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_isq;
PAR_DES_I16_T ParDes_isq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "isq",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_isq,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ispd;
PAR_DES_I16_T ParDes_ispd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ispd",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ispd,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ispq;
PAR_DES_I16_T ParDes_ispq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ispq",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ispq,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_isnd;
PAR_DES_I16_T ParDes_isnd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "isnd",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_isnd,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_isnq;
PAR_DES_I16_T ParDes_isnq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "isnq",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_isnq,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_is_al;
PAR_DES_I16_T ParDes_is_al =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "is_al",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_is_al,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_is_be;
PAR_DES_I16_T ParDes_is_be =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "is_be",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_is_be,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_isamp;
PAR_DES_I16_T ParDes_isamp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "isamp",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_isamp,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_isp_amp;
PAR_DES_I16_T ParDes_isp_amp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "isp_amp",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_isp_amp,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_isn_amp;
PAR_DES_I16_T ParDes_isn_amp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "isn_amp",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_isn_amp,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iLu;
PAR_DES_I16_T ParDes_iLu =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iLu",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iLu,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iLv;
PAR_DES_I16_T ParDes_iLv =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iLv",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iLv,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iLw;
PAR_DES_I16_T ParDes_iLw =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iLw",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iLw,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iLd;
PAR_DES_I16_T ParDes_iLd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iLd",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iLd,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iLq;
PAR_DES_I16_T ParDes_iLq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iLq",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iLq,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iLdav;
PAR_DES_I16_T ParDes_iLdav =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iLdav",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iLdav,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iLqav;
PAR_DES_I16_T ParDes_iLqav =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iLqav",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iLqav,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iLamp;
PAR_DES_I16_T ParDes_iLamp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iLamp",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iLamp,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_fLine;
PAR_DES_I16_T ParDes_fLine =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "fLine",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_fLine,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_thLine;
PAR_DES_I16_T ParDes_thLine =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "thLine",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT - 1,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_thLine,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_tIGBT,".extram");
INT16 ParVal_tIGBT;
PAR_DES_I16_T ParDes_tIGBT =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "tIGBT",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_tIGBT,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Vdcref,".extram");
INT16 ParVal_Vdcref;
PAR_DES_I16_T ParDes_Vdcref =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "Vdcref",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_Vdcref,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_iLqref,".extram");
INT16 ParVal_iLqref;
PAR_DES_I16_T ParDes_iLqref =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iLqref",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iLqref,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_iLdref,".extram");
INT16 ParVal_isqref;
PAR_DES_I16_T ParDes_isqref =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "isqref",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_isqref,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iLrefnd;
PAR_DES_I16_T ParDes_iLrefnd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iLrefnd",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iLrefnd,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iLrefnq;
PAR_DES_I16_T ParDes_iLrefnq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iLrefnq",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iLrefnq,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_isrefnd;
PAR_DES_I16_T ParDes_isrefnd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "isrefnd",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_isrefnd,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_isrefnq;
PAR_DES_I16_T ParDes_isrefnq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "isrefnq",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_isrefnq,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_isrefnamp;
PAR_DES_I16_T ParDes_isrefnamp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "isrefnamp",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_isrefnamp,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_idref,".extram");
INT16 ParVal_idref;
PAR_DES_I16_T ParDes_idref =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "idref",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_idref,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_iqref,".extram");
INT16 ParVal_iqref;
PAR_DES_I16_T ParDes_iqref =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iqref",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iqref,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_indref;
PAR_DES_I16_T ParDes_indref =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "indref",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_indref,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_inqref;
PAR_DES_I16_T ParDes_inqref =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "inqref",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_inqref,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_udref;
PAR_DES_I16_T ParDes_udref =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "udref",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_udref,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_u0ref;
PAR_DES_I16_T ParDes_u0ref =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "u0ref",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_u0ref,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iq_FRT;
PAR_DES_I16_T ParDes_iq_FRT =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iq_FRT",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iq_FRT,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_Qref_ramp;
PAR_DES_I16_T ParDes_Qref_ramp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "Qref_ramp",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_Qref_ramp,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_Pref_ramp;
PAR_DES_I16_T ParDes_Pref_ramp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "Pref_ramp",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_Pref_ramp,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_Mref;
PAR_DES_I16_T ParDes_Mref =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "Mref",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_Mref,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_Plimit_F;
PAR_DES_I16_T ParDes_Plimit_F =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "Plimit_F",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_Plimit_F,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_id_limit;
PAR_DES_I16_T ParDes_id_limit =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "id_limit",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_id_limit,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vLpd;
PAR_DES_I16_T ParDes_vLpd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vLpd",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vLpd,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vLpq;
PAR_DES_I16_T ParDes_vLpq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vLpq",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vLpq,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vLp_amp;
PAR_DES_I16_T ParDes_vLp_amp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vLp_amp",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vLp_amp,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vLnd;
PAR_DES_I16_T ParDes_vLnd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vLnd",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vLnd,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vLnq;
PAR_DES_I16_T ParDes_vLnq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vLnq",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vLnq,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vLn_amp;
PAR_DES_I16_T ParDes_vLn_amp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vLn_amp",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vLn_amp,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vLcompd;
PAR_DES_I16_T ParDes_vLcompd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vLcompd",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vLcompd,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vLcompq;
PAR_DES_I16_T ParDes_vLcompq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vLcompq",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vLcompq,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_VL_min;
PAR_DES_I16_T ParDes_VL_min =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "VL_min",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_VL_min,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_VL_max;
PAR_DES_I16_T ParDes_VL_max =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "VL_max",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_VL_max,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vPV1;
PAR_DES_I16_T ParDes_vPV1 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vPV1",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vPV1,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vPV2;
PAR_DES_I16_T ParDes_vPV2 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vPV2",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vPV2,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vPV3;
PAR_DES_I16_T ParDes_vPV3 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vPV3",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vPV3,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iPV1;
PAR_DES_I16_T ParDes_iPV1 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iPV1",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iPV1,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iPV2;
PAR_DES_I16_T ParDes_iPV2 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iPV2",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iPV2,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_iPV3;
PAR_DES_I16_T ParDes_iPV3 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iPV3",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iPV3,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ia1;
PAR_DES_I16_T ParDes_ia1 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ia1",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ia1,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ia2;
PAR_DES_I16_T ParDes_ia2 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ia2",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ia2,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ia3;
PAR_DES_I16_T ParDes_ia3 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ia3",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ia3,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ib1;
PAR_DES_I16_T ParDes_ib1 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ib1",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ib1,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ib2;
PAR_DES_I16_T ParDes_ib2 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ib2",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ib2,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ib3;
PAR_DES_I16_T ParDes_ib3 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ib3",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ib3,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ic1;
PAR_DES_I16_T ParDes_ic1 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ic1",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ic1,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ic2;
PAR_DES_I16_T ParDes_ic2 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ic2",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ic2,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ic3;
PAR_DES_I16_T ParDes_ic3 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ic3",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ic3,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_uqref;
PAR_DES_I16_T ParDes_uqref =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "uqref",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_uqref,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_udIrefp;
PAR_DES_I16_T ParDes_udIrefp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "udIrefp",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_udIrefp,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_uqIrefp;
PAR_DES_I16_T ParDes_uqIrefp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "uqIrefp",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_uqIrefp,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_udIrefn;
PAR_DES_I16_T ParDes_udIrefn =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "udIrefn",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_udIrefn,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_uqIrefn;
PAR_DES_I16_T ParDes_uqIrefn =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "uqIrefn",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_uqIrefn,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_udIref0;
PAR_DES_I16_T ParDes_udIref0 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "udIref0",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_udIref0,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_uqIref0;
PAR_DES_I16_T ParDes_uqIref0 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "uqIref0",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_uqIref0,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Imax,".extram");
INT16 ParVal_Imax;
PAR_DES_I16_T ParDes_Imax =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "Imax",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_Imax,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_ihdref1,".extram");
//#pragma DATA_SECTION(ParVal_ihqref1,".extram");
//#pragma DATA_SECTION(ParVal_ihLd1,".extram");
//#pragma DATA_SECTION(ParVal_ihLq1,".extram");
//#pragma DATA_SECTION(ParVal_ihinvd1,".extram");
//#pragma DATA_SECTION(ParVal_ihinvq1,".extram");
//#pragma DATA_SECTION(ParVal_ihinv_refd1,".extram");
//#pragma DATA_SECTION(ParVal_ihinv_refq1,".extram");
//#pragma DATA_SECTION(ParVal_vhrefd1,".extram");
//#pragma DATA_SECTION(ParVal_vhrefq1,".extram");
INT16 ParVal_ihdref1;
PAR_DES_I16_T ParDes_ihdref1 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihdref1",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihdref1,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihqref1;
PAR_DES_I16_T ParDes_ihqref1 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihqref1",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihqref1,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihLd1;
PAR_DES_I16_T ParDes_ihLd1 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihLd1",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihLd1,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihLq1;
PAR_DES_I16_T ParDes_ihLq1 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihLq1",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihLq1,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinvd1;
PAR_DES_I16_T ParDes_ihinvd1 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinvd1",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinvd1,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinvq1;
PAR_DES_I16_T ParDes_ihinvq1 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinvq1",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinvq1,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinv_refd1;
PAR_DES_I16_T ParDes_ihinv_refd1 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinv_refd1",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinv_refd1,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinv_refq1;
PAR_DES_I16_T ParDes_ihinv_refq1 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinv_refq1",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinv_refq1,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vhrefd1;
PAR_DES_I16_T ParDes_vhrefd1 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vhrefd1",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vhrefd1,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vhrefq1;
PAR_DES_I16_T ParDes_vhrefq1 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vhrefq1",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vhrefq1,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_ihdref2,".extram");
//#pragma DATA_SECTION(ParVal_ihqref2,".extram");
//#pragma DATA_SECTION(ParVal_ihLd2,".extram");
//#pragma DATA_SECTION(ParVal_ihLq2,".extram");
//#pragma DATA_SECTION(ParVal_ihinvd2,".extram");
//#pragma DATA_SECTION(ParVal_ihinvq2,".extram");
//#pragma DATA_SECTION(ParVal_ihinv_refd2,".extram");
//#pragma DATA_SECTION(ParVal_ihinv_refq2,".extram");
//#pragma DATA_SECTION(ParVal_vhrefd2,".extram");
//#pragma DATA_SECTION(ParVal_vhrefq2,".extram");
INT16 ParVal_ihdref2;
PAR_DES_I16_T ParDes_ihdref2 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihdref2",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihdref2,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihqref2;
PAR_DES_I16_T ParDes_ihqref2 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihqref2",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihqref2,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihLd2;
PAR_DES_I16_T ParDes_ihLd2 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihLd2",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihLd2,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihLq2;
PAR_DES_I16_T ParDes_ihLq2 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihLq2",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihLq2,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinvd2;
PAR_DES_I16_T ParDes_ihinvd2 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinvd2",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinvd2,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinvq2;
PAR_DES_I16_T ParDes_ihinvq2 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinvq2",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinvq2,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinv_refd2;
PAR_DES_I16_T ParDes_ihinv_refd2 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinv_refd2",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinv_refd2,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinv_refq2;
PAR_DES_I16_T ParDes_ihinv_refq2 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinv_refq2",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinv_refq2,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vhrefd2;
PAR_DES_I16_T ParDes_vhrefd2 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vhrefd2",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vhrefd2,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vhrefq2;
PAR_DES_I16_T ParDes_vhrefq2 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vhrefq2",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vhrefq2,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_ihdref3,".extram");
//#pragma DATA_SECTION(ParVal_ihqref3,".extram");
//#pragma DATA_SECTION(ParVal_ihLd3,".extram");
//#pragma DATA_SECTION(ParVal_ihLq3,".extram");
//#pragma DATA_SECTION(ParVal_ihinvd3,".extram");
//#pragma DATA_SECTION(ParVal_ihinvq3,".extram");
//#pragma DATA_SECTION(ParVal_ihinv_refd3,".extram");
//#pragma DATA_SECTION(ParVal_ihinv_refq3,".extram");
//#pragma DATA_SECTION(ParVal_vhrefd3,".extram");
//#pragma DATA_SECTION(ParVal_vhrefq3,".extram");
INT16 ParVal_ihdref3;
PAR_DES_I16_T ParDes_ihdref3 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihdref3",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihdref3,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihqref3;
PAR_DES_I16_T ParDes_ihqref3 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihqref3",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihqref3,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihLd3;
PAR_DES_I16_T ParDes_ihLd3 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihLd3",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihLd3,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihLq3;
PAR_DES_I16_T ParDes_ihLq3 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihLq3",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihLq3,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinvd3;
PAR_DES_I16_T ParDes_ihinvd3 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinvd3",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinvd3,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinvq3;
PAR_DES_I16_T ParDes_ihinvq3 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinvq3",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinvq3,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinv_refd3;
PAR_DES_I16_T ParDes_ihinv_refd3 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinv_refd3",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinv_refd3,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinv_refq3;
PAR_DES_I16_T ParDes_ihinv_refq3 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinv_refq3",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinv_refq3,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vhrefd3;
PAR_DES_I16_T ParDes_vhrefd3 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vhrefd3",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vhrefd3,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vhrefq3;
PAR_DES_I16_T ParDes_vhrefq3 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vhrefq3",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vhrefq3,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_ihdref4,".extram");
//#pragma DATA_SECTION(ParVal_ihqref4,".extram");
//#pragma DATA_SECTION(ParVal_ihLd4,".extram");
//#pragma DATA_SECTION(ParVal_ihLq4,".extram");
//#pragma DATA_SECTION(ParVal_ihinvd4,".extram");
//#pragma DATA_SECTION(ParVal_ihinvq4,".extram");
//#pragma DATA_SECTION(ParVal_ihinv_refd4,".extram");
//#pragma DATA_SECTION(ParVal_ihinv_refq4,".extram");
//#pragma DATA_SECTION(ParVal_vhrefd4,".extram");
//#pragma DATA_SECTION(ParVal_vhrefq4,".extram");
INT16 ParVal_ihdref4;
PAR_DES_I16_T ParDes_ihdref4 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihdref4",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihdref4,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihqref4;
PAR_DES_I16_T ParDes_ihqref4 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihqref4",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihqref4,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihLd4;
PAR_DES_I16_T ParDes_ihLd4 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihLd4",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihLd4,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihLq4;
PAR_DES_I16_T ParDes_ihLq4 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihLq4",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihLq4,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinvd4;
PAR_DES_I16_T ParDes_ihinvd4 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinvd4",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinvd4,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinvq4;
PAR_DES_I16_T ParDes_ihinvq4 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinvq4",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinvq4,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinv_refd4;
PAR_DES_I16_T ParDes_ihinv_refd4 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinv_refd4",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinv_refd4,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinv_refq4;
PAR_DES_I16_T ParDes_ihinv_refq4 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinv_refq4",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinv_refq4,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vhrefd4;
PAR_DES_I16_T ParDes_vhrefd4 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vhrefd4",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vhrefd4,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vhrefq4;
PAR_DES_I16_T ParDes_vhrefq4 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vhrefq4",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vhrefq4,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_ihdref5,".extram");
//#pragma DATA_SECTION(ParVal_ihqref5,".extram");
//#pragma DATA_SECTION(ParVal_ihLd5,".extram");
//#pragma DATA_SECTION(ParVal_ihLq5,".extram");
//#pragma DATA_SECTION(ParVal_ihinvd5,".extram");
//#pragma DATA_SECTION(ParVal_ihinvq5,".extram");
//#pragma DATA_SECTION(ParVal_ihinv_refd5,".extram");
//#pragma DATA_SECTION(ParVal_ihinv_refq5,".extram");
//#pragma DATA_SECTION(ParVal_vhrefd5,".extram");
//#pragma DATA_SECTION(ParVal_vhrefq5,".extram");
INT16 ParVal_ihdref5;
PAR_DES_I16_T ParDes_ihdref5 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihdref5",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihdref5,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihqref5;
PAR_DES_I16_T ParDes_ihqref5 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihqref5",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihqref5,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihLd5;
PAR_DES_I16_T ParDes_ihLd5 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihLd5",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihLd5,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihLq5;
PAR_DES_I16_T ParDes_ihLq5 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihLq5",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihLq5,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinvd5;
PAR_DES_I16_T ParDes_ihinvd5 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinvd5",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinvd5,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinvq5;
PAR_DES_I16_T ParDes_ihinvq5 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinvq5",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinvq5,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinv_refd5;
PAR_DES_I16_T ParDes_ihinv_refd5 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinv_refd5",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinv_refd5,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinv_refq5;
PAR_DES_I16_T ParDes_ihinv_refq5 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinv_refq5",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinv_refq5,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vhrefd5;
PAR_DES_I16_T ParDes_vhrefd5 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vhrefd5",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vhrefd5,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vhrefq5;
PAR_DES_I16_T ParDes_vhrefq5 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vhrefq5",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vhrefq5,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_ihdref6,".extram");
//#pragma DATA_SECTION(ParVal_ihqref6,".extram");
//#pragma DATA_SECTION(ParVal_ihLd6,".extram");
//#pragma DATA_SECTION(ParVal_ihLq6,".extram");
//#pragma DATA_SECTION(ParVal_ihinvd6,".extram");
//#pragma DATA_SECTION(ParVal_ihinvq6,".extram");
//#pragma DATA_SECTION(ParVal_ihinv_refd6,".extram");
//#pragma DATA_SECTION(ParVal_ihinv_refq6,".extram");
//#pragma DATA_SECTION(ParVal_vhrefd6,".extram");
//#pragma DATA_SECTION(ParVal_vhrefq6,".extram");
INT16 ParVal_ihdref6;
PAR_DES_I16_T ParDes_ihdref6 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihdref6",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihdref6,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihqref6;
PAR_DES_I16_T ParDes_ihqref6 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihqref6",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihqref6,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihLd6;
PAR_DES_I16_T ParDes_ihLd6 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihLd6",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihLd6,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihLq6;
PAR_DES_I16_T ParDes_ihLq6 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihLq6",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihLq6,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinvd6;
PAR_DES_I16_T ParDes_ihinvd6 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinvd6",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinvd6,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinvq6;
PAR_DES_I16_T ParDes_ihinvq6 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinvq6",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinvq6,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinv_refd6;
PAR_DES_I16_T ParDes_ihinv_refd6 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinv_refd6",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinv_refd6,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinv_refq6;
PAR_DES_I16_T ParDes_ihinv_refq6 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinv_refq6",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinv_refq6,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vhrefd6;
PAR_DES_I16_T ParDes_vhrefd6 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vhrefd6",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vhrefd6,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vhrefq6;
PAR_DES_I16_T ParDes_vhrefq6 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vhrefq6",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vhrefq6,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_ihdref7,".extram");
//#pragma DATA_SECTION(ParVal_ihqref7,".extram");
//#pragma DATA_SECTION(ParVal_ihLd7,".extram");
//#pragma DATA_SECTION(ParVal_ihLq7,".extram");
//#pragma DATA_SECTION(ParVal_ihinvd7,".extram");
//#pragma DATA_SECTION(ParVal_ihinvq7,".extram");
//#pragma DATA_SECTION(ParVal_ihinv_refd7,".extram");
//#pragma DATA_SECTION(ParVal_ihinv_refq7,".extram");
//#pragma DATA_SECTION(ParVal_vhrefd7,".extram");
//#pragma DATA_SECTION(ParVal_vhrefq7,".extram");
INT16 ParVal_ihdref7;
PAR_DES_I16_T ParDes_ihdref7 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihdref7",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihdref7,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihqref7;
PAR_DES_I16_T ParDes_ihqref7 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihqref7",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihqref7,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihLd7;
PAR_DES_I16_T ParDes_ihLd7 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihLd7",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihLd7,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihLq7;
PAR_DES_I16_T ParDes_ihLq7 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihLq7",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihLq7,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinvd7;
PAR_DES_I16_T ParDes_ihinvd7 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinvd7",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinvd7,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinvq7;
PAR_DES_I16_T ParDes_ihinvq7 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinvq7",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinvq7,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinv_refd7;
PAR_DES_I16_T ParDes_ihinv_refd7 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinv_refd7",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinv_refd7,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinv_refq7;
PAR_DES_I16_T ParDes_ihinv_refq7 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinv_refq7",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinv_refq7,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vhrefd7;
PAR_DES_I16_T ParDes_vhrefd7 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vhrefd7",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vhrefd7,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vhrefq7;
PAR_DES_I16_T ParDes_vhrefq7 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vhrefq7",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vhrefq7,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_ihdref8,".extram");
//#pragma DATA_SECTION(ParVal_ihqref8,".extram");
//#pragma DATA_SECTION(ParVal_ihLd8,".extram");
//#pragma DATA_SECTION(ParVal_ihLq8,".extram");
//#pragma DATA_SECTION(ParVal_ihinvd8,".extram");
//#pragma DATA_SECTION(ParVal_ihinvq8,".extram");
//#pragma DATA_SECTION(ParVal_ihinv_refd8,".extram");
//#pragma DATA_SECTION(ParVal_ihinv_refq8,".extram");
//#pragma DATA_SECTION(ParVal_vhrefd8,".extram");
//#pragma DATA_SECTION(ParVal_vhrefq8,".extram");
INT16 ParVal_ihdref8;
PAR_DES_I16_T ParDes_ihdref8 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihdref8",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihdref8,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihqref8;
PAR_DES_I16_T ParDes_ihqref8 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihqref8",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihqref8,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihLd8;
PAR_DES_I16_T ParDes_ihLd8 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihLd8",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihLd8,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihLq8;
PAR_DES_I16_T ParDes_ihLq8 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihLq8",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihLq8,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinvd8;
PAR_DES_I16_T ParDes_ihinvd8 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinvd8",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinvd8,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinvq8;
PAR_DES_I16_T ParDes_ihinvq8 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinvq8",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinvq8,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinv_refd8;
PAR_DES_I16_T ParDes_ihinv_refd8 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinv_refd8",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinv_refd8,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ihinv_refq8;
PAR_DES_I16_T ParDes_ihinv_refq8 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ihinv_refq8",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ihinv_refq8,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vhrefd8;
PAR_DES_I16_T ParDes_vhrefd8 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vhrefd8",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vhrefd8,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_vhrefq8;
PAR_DES_I16_T ParDes_vhrefq8 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "vhrefq8",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_vhrefq8,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};



//INT16 ParVal_ErrStack;
PAR_DES_I16_T ParDes_ErrStack =
{
    PAR_MENUE | PAR_RO | _INT16,                    // ReadWrite, INT16-Parameter
    "Errorstack",                                               // Name
    NULL,                                                   // Einheit
    0,                                              // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    NULL/*&ParVal_ErrStack*/,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

#define NO_ERRSTACK_ENTRIES 6

//#pragma DATA_SECTION(ParVal_ErrStackIndex,".extram");
INT16 ParVal_ErrStackIndex;
PAR_DES_I16_T ParDes_ErrStackIndex =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "Errorstack index",                                             // Name
    NULL,                                                   // Einheit
    NO_ERRSTACK_ENTRIES,                                                // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ErrStackIndex,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_ErrEvent1_Hours,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent1_Secs,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent1_StateFault,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent1_SW_Fault,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent1_HW_Fault,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent1_Warnings,".extram");
UINT16 ParVal_ErrEvent1_Hours;
PAR_DES_U16_T ParDes_ErrEvent1_Hours =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _UINT16,                    // ReadWrite, INT16-Parameter
    "Event 1 hours",                                                // Name
    "h",                                                    // Einheit
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ErrEvent1_Hours,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

UINT16 ParVal_ErrEvent1_Secs;
PAR_DES_U16_T ParDes_ErrEvent1_Secs =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _UINT16,                    // ReadWrite, INT16-Parameter
    "Event 1 seconds",                                              // Name
    "s",                                                    // Einheit
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ErrEvent1_Secs,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ErrEvent1_StateFault;
PAR_DES_I16_T ParDes_ErrEvent1_StateFault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "Event 1 state fault",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ErrEvent1_StateFault,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

WORD ParVal_ErrEvent1_SW_Fault;
PAR_DES_WORD_T ParDes_ErrEvent1_SW_Fault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _WORD,                  // ReadWrite, INT16-Parameter
    "Event 1 SW fault",                                             // Name
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    &ParVal_ErrEvent1_SW_Fault,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

DWORD ParVal_ErrEvent1_HW_Fault;
PAR_DES_DWORD_T ParDes_ErrEvent1_HW_Fault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _DWORD,                 // ReadWrite, INT16-Parameter
    "Event 1 HW fault",                                             // Name
    MAX_ULONG,                                              // Max
    0,                                          // Min
    0,                                                      // Default
    &ParVal_ErrEvent1_HW_Fault,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

WORD ParVal_ErrEvent1_Warnings;
PAR_DES_WORD_T ParDes_ErrEvent1_Warnings =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _WORD,                  // ReadWrite, INT16-Parameter
    "Event 1 warnings",                                             // Name
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    &ParVal_ErrEvent1_Warnings,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_ErrEvent2_Hours,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent2_Secs,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent2_StateFault,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent2_SW_Fault,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent2_HW_Fault,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent2_Warnings,".extram");
UINT16 ParVal_ErrEvent2_Hours;
PAR_DES_U16_T ParDes_ErrEvent2_Hours =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _UINT16,                    // ReadWrite, INT16-Parameter
    "Event 2 hours",                                                // Name
    "h",                                                    // Einheit
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ErrEvent2_Hours,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

UINT16 ParVal_ErrEvent2_Secs;
PAR_DES_U16_T ParDes_ErrEvent2_Secs =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _UINT16,                    // ReadWrite, INT16-Parameter
    "Event 2 seconds",                                              // Name
    "s",                                                    // Einheit
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ErrEvent2_Secs,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ErrEvent2_StateFault;
PAR_DES_I16_T ParDes_ErrEvent2_StateFault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "Event 2 state fault",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ErrEvent2_StateFault,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

WORD ParVal_ErrEvent2_SW_Fault;
PAR_DES_WORD_T ParDes_ErrEvent2_SW_Fault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _WORD,                  // ReadWrite, INT16-Parameter
    "Event 2 SW fault",                                             // Name
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    &ParVal_ErrEvent2_SW_Fault,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

DWORD ParVal_ErrEvent2_HW_Fault;
PAR_DES_DWORD_T ParDes_ErrEvent2_HW_Fault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _DWORD,                 // ReadWrite, INT16-Parameter
    "Event 2 HW fault",                                             // Name
    MAX_ULONG,                                              // Max
    0,                                          // Min
    0,                                                      // Default
    &ParVal_ErrEvent2_HW_Fault,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

WORD ParVal_ErrEvent2_Warnings;
PAR_DES_WORD_T ParDes_ErrEvent2_Warnings =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _WORD,                  // ReadWrite, INT16-Parameter
    "Event 2 warnings",                                             // Name
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    &ParVal_ErrEvent2_Warnings,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_ErrEvent3_Hours,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent3_Secs,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent3_StateFault,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent3_SW_Fault,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent3_HW_Fault,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent3_Warnings,".extram");
UINT16 ParVal_ErrEvent3_Hours;
PAR_DES_U16_T ParDes_ErrEvent3_Hours =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _UINT16,                    // ReadWrite, INT16-Parameter
    "Event 3 hours",                                                // Name
    "h",                                                    // Einheit
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ErrEvent3_Hours,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

UINT16 ParVal_ErrEvent3_Secs;
PAR_DES_U16_T ParDes_ErrEvent3_Secs =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _UINT16,                    // ReadWrite, INT16-Parameter
    "Event 3 seconds",                                              // Name
    "s",                                                    // Einheit
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ErrEvent3_Secs,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ErrEvent3_StateFault;
PAR_DES_I16_T ParDes_ErrEvent3_StateFault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "Event 3 state fault",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ErrEvent3_StateFault,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

WORD ParVal_ErrEvent3_SW_Fault;
PAR_DES_WORD_T ParDes_ErrEvent3_SW_Fault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _WORD,                  // ReadWrite, INT16-Parameter
    "Event 3 SW fault",                                             // Name
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    &ParVal_ErrEvent3_SW_Fault,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

DWORD ParVal_ErrEvent3_HW_Fault;
PAR_DES_DWORD_T ParDes_ErrEvent3_HW_Fault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _DWORD,                 // ReadWrite, INT16-Parameter
    "Event 3 HW fault",                                             // Name
    MAX_ULONG,                                              // Max
    0,                                          // Min
    0,                                                      // Default
    &ParVal_ErrEvent3_HW_Fault,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

WORD ParVal_ErrEvent3_Warnings;
PAR_DES_WORD_T ParDes_ErrEvent3_Warnings =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _WORD,                  // ReadWrite, INT16-Parameter
    "Event 3 warnings",                                             // Name
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    &ParVal_ErrEvent3_Warnings,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_ErrEvent4_Hours,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent4_Secs,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent4_StateFault,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent4_SW_Fault,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent4_HW_Fault,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent4_Warnings,".extram");
UINT16 ParVal_ErrEvent4_Hours;
PAR_DES_U16_T ParDes_ErrEvent4_Hours =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _UINT16,                    // ReadWrite, INT16-Parameter
    "Event 4 hours",                                                // Name
    "h",                                                    // Einheit
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ErrEvent4_Hours,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

UINT16 ParVal_ErrEvent4_Secs;
PAR_DES_U16_T ParDes_ErrEvent4_Secs =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _UINT16,                    // ReadWrite, INT16-Parameter
    "Event 4 seconds",                                              // Name
    "s",                                                    // Einheit
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ErrEvent4_Secs,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ErrEvent4_StateFault;
PAR_DES_I16_T ParDes_ErrEvent4_StateFault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "Event 4 state fault",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ErrEvent4_StateFault,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

WORD ParVal_ErrEvent4_SW_Fault;
PAR_DES_WORD_T ParDes_ErrEvent4_SW_Fault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _WORD,                  // ReadWrite, INT16-Parameter
    "Event 4 SW fault",                                             // Name
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    &ParVal_ErrEvent4_SW_Fault,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

DWORD ParVal_ErrEvent4_HW_Fault;
PAR_DES_DWORD_T ParDes_ErrEvent4_HW_Fault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _DWORD,                 // ReadWrite, INT16-Parameter
    "Event 4 HW fault",                                             // Name
    MAX_ULONG,                                              // Max
    0,                                          // Min
    0,                                                      // Default
    &ParVal_ErrEvent4_HW_Fault,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

WORD ParVal_ErrEvent4_Warnings;
PAR_DES_WORD_T ParDes_ErrEvent4_Warnings =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _WORD,                  // ReadWrite, INT16-Parameter
    "Event 4 warnings",                                             // Name
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    &ParVal_ErrEvent4_Warnings,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_ErrEvent5_Hours,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent5_Secs,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent5_StateFault,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent5_SW_Fault,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent5_HW_Fault,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent5_Warnings,".extram");
UINT16 ParVal_ErrEvent5_Hours;
PAR_DES_U16_T ParDes_ErrEvent5_Hours =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _UINT16,                    // ReadWrite, INT16-Parameter
    "Event 5 hours",                                                // Name
    "h",                                                    // Einheit
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ErrEvent5_Hours,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

UINT16 ParVal_ErrEvent5_Secs;
PAR_DES_U16_T ParDes_ErrEvent5_Secs =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _UINT16,                    // ReadWrite, INT16-Parameter
    "Event 5 seconds",                                              // Name
    "s",                                                    // Einheit
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ErrEvent5_Secs,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ErrEvent5_StateFault;
PAR_DES_I16_T ParDes_ErrEvent5_StateFault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "Event 5 state fault",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ErrEvent5_StateFault,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

WORD ParVal_ErrEvent5_SW_Fault;
PAR_DES_WORD_T ParDes_ErrEvent5_SW_Fault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _WORD,                  // ReadWrite, INT16-Parameter
    "Event 5 SW fault",                                             // Name
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    &ParVal_ErrEvent5_SW_Fault,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

DWORD ParVal_ErrEvent5_HW_Fault;
PAR_DES_DWORD_T ParDes_ErrEvent5_HW_Fault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _DWORD,                 // ReadWrite, INT16-Parameter
    "Event 5 HW fault",                                             // Name
    MAX_ULONG,                                              // Max
    0,                                          // Min
    0,                                                      // Default
    &ParVal_ErrEvent5_HW_Fault,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

WORD ParVal_ErrEvent5_Warnings;
PAR_DES_WORD_T ParDes_ErrEvent5_Warnings =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _WORD,                  // ReadWrite, INT16-Parameter
    "Event 5 warnings",                                             // Name
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    &ParVal_ErrEvent5_Warnings,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_ErrEvent6_Hours,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent6_Secs,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent6_StateFault,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent6_SW_Fault,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent6_HW_Fault,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent6_Warnings,".extram");
UINT16 ParVal_ErrEvent6_Hours;
PAR_DES_U16_T ParDes_ErrEvent6_Hours =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _UINT16,                    // ReadWrite, INT16-Parameter
    "Event 6 hours",                                                // Name
    "h",                                                    // Einheit
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ErrEvent6_Hours,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

UINT16 ParVal_ErrEvent6_Secs;
PAR_DES_U16_T ParDes_ErrEvent6_Secs =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _UINT16,                    // ReadWrite, INT16-Parameter
    "Event 6 seconds",                                              // Name
    "s",                                                    // Einheit
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ErrEvent6_Secs,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ErrEvent6_StateFault;
PAR_DES_I16_T ParDes_ErrEvent6_StateFault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "Event 6 state fault",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ErrEvent6_StateFault,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

WORD ParVal_ErrEvent6_SW_Fault;
PAR_DES_WORD_T ParDes_ErrEvent6_SW_Fault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _WORD,                  // ReadWrite, INT16-Parameter
    "Event 6 SW fault",                                             // Name
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    &ParVal_ErrEvent6_SW_Fault,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

DWORD ParVal_ErrEvent6_HW_Fault;
PAR_DES_DWORD_T ParDes_ErrEvent6_HW_Fault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _DWORD,                 // ReadWrite, INT16-Parameter
    "Event 6 HW fault",                                             // Name
    MAX_ULONG,                                              // Max
    0,                                          // Min
    0,                                                      // Default
    &ParVal_ErrEvent6_HW_Fault,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

WORD ParVal_ErrEvent6_Warnings;
PAR_DES_WORD_T ParDes_ErrEvent6_Warnings =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _WORD,                  // ReadWrite, INT16-Parameter
    "Event 6 warnings",                                             // Name
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    &ParVal_ErrEvent6_Warnings,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_ErrEvent7_Hours,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent7_Secs,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent7_StateFault,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent7_SW_Fault,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent7_HW_Fault,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent7_Warnings,".extram");
UINT16 ParVal_ErrEvent7_Hours;
PAR_DES_U16_T ParDes_ErrEvent7_Hours =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _UINT16,                    // ReadWrite, INT16-Parameter
    "Event 7 hours",                                                // Name
    "h",                                                    // Einheit
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ErrEvent7_Hours,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

UINT16 ParVal_ErrEvent7_Secs;
PAR_DES_U16_T ParDes_ErrEvent7_Secs =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _UINT16,                    // ReadWrite, INT16-Parameter
    "Event 7 seconds",                                              // Name
    "s",                                                    // Einheit
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ErrEvent7_Secs,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ErrEvent7_StateFault;
PAR_DES_I16_T ParDes_ErrEvent7_StateFault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "Event 7 state fault",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ErrEvent7_StateFault,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

WORD ParVal_ErrEvent7_SW_Fault;
PAR_DES_WORD_T ParDes_ErrEvent7_SW_Fault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _WORD,                  // ReadWrite, INT16-Parameter
    "Event 7 SW fault",                                             // Name
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    &ParVal_ErrEvent7_SW_Fault,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

DWORD ParVal_ErrEvent7_HW_Fault;
PAR_DES_DWORD_T ParDes_ErrEvent7_HW_Fault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _DWORD,                 // ReadWrite, INT16-Parameter
    "Event 7 HW fault",                                             // Name
    MAX_ULONG,                                              // Max
    0,                                          // Min
    0,                                                      // Default
    &ParVal_ErrEvent7_HW_Fault,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

WORD ParVal_ErrEvent7_Warnings;
PAR_DES_WORD_T ParDes_ErrEvent7_Warnings =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _WORD,                  // ReadWrite, INT16-Parameter
    "Event 7 warnings",                                             // Name
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    &ParVal_ErrEvent7_Warnings,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_ErrEvent8_Hours,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent8_Secs,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent8_StateFault,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent8_SW_Fault,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent8_HW_Fault,".extram");
//#pragma DATA_SECTION(ParVal_ErrEvent8_Warnings,".extram");
UINT16 ParVal_ErrEvent8_Hours;
PAR_DES_U16_T ParDes_ErrEvent8_Hours =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _UINT16,                    // ReadWrite, INT16-Parameter
    "Event 8 hours",                                                // Name
    "h",                                                    // Einheit
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ErrEvent8_Hours,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

UINT16 ParVal_ErrEvent8_Secs;
PAR_DES_U16_T ParDes_ErrEvent8_Secs =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _UINT16,                    // ReadWrite, INT16-Parameter
    "Event 8 seconds",                                              // Name
    "s",                                                    // Einheit
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ErrEvent8_Secs,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

INT16 ParVal_ErrEvent8_StateFault;
PAR_DES_I16_T ParDes_ErrEvent8_StateFault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "Event 8 state fault",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ErrEvent8_StateFault,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

WORD ParVal_ErrEvent8_SW_Fault;
PAR_DES_WORD_T ParDes_ErrEvent8_SW_Fault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _WORD,                  // ReadWrite, INT16-Parameter
    "Event 8 SW fault",                                             // Name
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    &ParVal_ErrEvent8_SW_Fault,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

DWORD ParVal_ErrEvent8_HW_Fault;
PAR_DES_DWORD_T ParDes_ErrEvent8_HW_Fault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _DWORD,                 // ReadWrite, INT16-Parameter
    "Event 8 HW fault",                                             // Name
    MAX_ULONG,                                              // Max
    0,                                          // Min
    0,                                                      // Default
    &ParVal_ErrEvent8_HW_Fault,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

WORD ParVal_ErrEvent8_Warnings;
PAR_DES_WORD_T ParDes_ErrEvent8_Warnings =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _WORD,                  // ReadWrite, INT16-Parameter
    "Event 8 warnings",                                             // Name
    MAX_UINT,                                               // Max
    0,                                          // Min
    0,                                                      // Default
    &ParVal_ErrEvent8_Warnings,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};


// Voltage control

//INT16 ParVal_VoltageControl;
PAR_DES_I16_T ParDes_VoltageControl =
{
    PAR_MENUE | PAR_RW | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "P/V/f Regelung",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "P/V/f control / reference",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_VoltageControl*/,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Current control

//INT16 ParVal_CurrentControl;
PAR_DES_I16_T ParDes_CurrentControl =
{
    PAR_MENUE | PAR_RW | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Stromregelung",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Current control",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_CurrentControl*/,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};



//#pragma DATA_SECTION(ParVal_PWM_mode0,".extram");
INT16 ParVal_PWM_mode0;
PAR_DES_I16_T ParDes_PWM_mode0 =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "PWM Modus 0",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "PWM mode 0",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    0x0005,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_PWM_mode0,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_PWM_frequ,".extram");
INT16 ParVal_PWM_frequ;
PAR_DES_I16_T ParDes_PWM_frequ =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "PWM Basisfrequenz 1",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "PWM base frequency 1",                             // Name
#endif
#endif
    "Hz",                                                           // Einheit
    6500,                                               // Maximum
    1500,                                                               // Minimum
    3250,                                                               // Default
    1.0,                      // Faktor
    &ParVal_PWM_frequ,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};



//#pragma DATA_SECTION(ParVal_PWM_Td,".extram");
INT16 ParVal_PWM_Td;
PAR_DES_I16_T ParDes_PWM_Td =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Totzeit",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Blanking time",                                // Name
#endif
#endif
    "mys",                                                          // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_PWM_Td,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_PWM_mode_i,".extram");
INT16 ParVal_PWM_mode_i;
PAR_DES_I16_T ParDes_PWM_mode_i =
{
    PAR_PASS_PR | PAR_RO | _INT16,
#ifdef LANGUAGE_GERMAN
    "PWM Modus intern",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "PWM mode intern",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    0x0005,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_PWM_mode_i,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_IregMode,".extram");
INT16 ParVal_IregMode;
PAR_DES_I16_T ParDes_IregMode =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Strommodus",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Current mode",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    0x0003,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_IregMode,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_VoltageMode,".extram");
INT16 ParVal_VoltageMode;
PAR_DES_I16_T ParDes_VoltageMode =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Spanungsregler-Modus",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Voltage control mode",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    0x0003,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_VoltageMode,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_FieldbusLocalRemote,".extram");
INT16 ParVal_FieldbusLocalRemote;
PAR_DES_I16_T ParDes_FieldbusLocalRemote =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Feldbusfreigabe",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Fieldbus enable",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_FieldbusLocalRemote,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_VoltRef,".extram");
INT16 ParVal_VoltRef;
PAR_DES_I16_T ParDes_VoltRef =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Sollwert Wirkleistung",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Active power reference",                               // Name
#endif
#endif
    "kW",                                                           // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_VoltRef,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_VoltRefRamp,".extram");
INT16 ParVal_VoltRefRamp;
PAR_DES_I16_T ParDes_VoltRefRamp =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "P Rampe",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "P ramp reference",                             // Name
#endif
#endif
    "kVA",                                                          // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_VoltRefRamp,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_FrequRef,".extram");
INT16 ParVal_FrequRef;
PAR_DES_I16_T ParDes_FrequRef =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Frequenzsollwert",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Frequency reference",                              // Name
#endif
#endif
    "Hz",                                                           // Einheit
    4000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_FrequRef,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_RampTime,".extram");
INT16 ParVal_RampTime;
PAR_DES_I16_T ParDes_RampTime =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Rampenzeit",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Ramp time",                                // Name
#endif
#endif
    "ms",                                                           // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_RampTime,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_RampTimeFrequ,".extram");
INT16 ParVal_RampTimeFrequ;
PAR_DES_I16_T ParDes_RampTimeFrequ =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Frequenz-Rampenzeit",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Frequency ramp time",                              // Name
#endif
#endif
    "ms",                                                           // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_RampTimeFrequ,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_ActivateRef,".extram");
INT16 ParVal_ActivateRef;
PAR_DES_I16_T ParDes_ActivateRef =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Sollwert aktivieren",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Activate reference",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_ActivateRef,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Fieldbus
//INT16 ParVal_Fieldbus;
PAR_DES_I16_T ParDes_Fieldbus =
{
    PAR_MENUE | PAR_RW | _INT16,                    // ReadWrite, INT16-Parameter
    "Fieldbus",                                             // Name
    NULL,                                                   // Einheit
    0,                                              // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    NULL/*&ParVal_Fieldbus*/,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

#define FIELDBUS_IN_SIZE        (32)        // in WORDs
#define FIELDBUS_OUT_SIZE       (32)
UINT16 PDP_Input [FIELDBUS_IN_SIZE];
UINT16 PDP_Output [FIELDBUS_OUT_SIZE];

PAR_DES_U16_T ParDes_FieldbusCtr =
{
    PAR_RO | _UINT16,
    "PDP control",                              // Name
    NULL,                                                           // Einheit
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &PDP_Input[0],                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

uint16_T FieldbusStatus;               /* '<Root>/P_Vline_tripp46' */

PAR_DES_U16_T ParDes_FieldbusStatus =
{
    PAR_RO | _UINT16,
    "PDP Status",                               // Name
    NULL,                                                           // Einheit
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &FieldbusStatus,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_U16_T ParDes_FieldbusVoltage =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "PDP Spannungssollwert",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "PDP voltage reference",                                // Name
#endif
#endif
    "V",                                                            // Einheit
    10000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &PDP_Input[1],                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_U16_T ParDes_FieldbusCurrent =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "PDP Stromsollwert",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "PDP current reference",                                // Name
#endif
#endif
    "A",                                                            // Einheit
    32000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &PDP_Input[2],                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_U16_T ParDes_FieldbusFrequency =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "PDP Frequenzsollwert",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "PDP frequency reference",                              // Name
#endif
#endif
    "Hz",                                                           // Einheit
    4000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &PDP_Input[4],                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_U16_T ParDes_FieldbusRamp =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "PDP Rampenzeit",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "PDP ramp time",                                // Name
#endif
#endif
    "ms",                                                           // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &PDP_Input[3],                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T ParDes_Fieldbus_H1Order =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Ordnung Harmonic 1",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonic 1 order",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    100,                                                // Maximum
    -100,                                                               // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    (INT16 *)&PDP_Input[5],                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_U16_T ParDes_Fieldbus_H1Ampl =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Amplitude Harmonic 1",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonic 1 amplitude",                             // Name
#endif
#endif
    "A",                                                            // Einheit
    10000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &PDP_Input[6],                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_U16_T ParDes_Fieldbus_H1Phase =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Phase Harmonic 1",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonic 1 phase",                             // Name
#endif
#endif
    "deg",                                                          // Einheit
    3600,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &PDP_Input[7],                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T ParDes_Fieldbus_H2Order =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Ordnung Harmonic 2",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonic 2 order",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    100,                                                // Maximum
    -100,                                                               // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    (INT16 *)&PDP_Input[8],                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_U16_T ParDes_Fieldbus_H2Ampl =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Amplitude Harmonic 2",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonic 2 amplitude",                             // Name
#endif
#endif
    "A",                                                            // Einheit
    10000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &PDP_Input[9],                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_U16_T ParDes_Fieldbus_H2Phase =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Phase Harmonic 2",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonic 2 phase",                             // Name
#endif
#endif
    "deg",                                                          // Einheit
    3600,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &PDP_Input[10],                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T ParDes_Fieldbus_H3Order =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Ordnung Harmonic 3",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonic 3 order",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    100,                                                // Maximum
    -100,                                                               // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    (INT16 *)&PDP_Input[11],                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_U16_T ParDes_Fieldbus_H3Ampl =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Amplitude Harmonic 3",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonic 3 amplitude",                             // Name
#endif
#endif
    "A",                                                            // Einheit
    10000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &PDP_Input[12],                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_U16_T ParDes_Fieldbus_H3Phase =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Phase Harmonic 3",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonic 3 phase",                             // Name
#endif
#endif
    "deg",                                                          // Einheit
    3600,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &PDP_Input[13],                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T ParDes_Fieldbus_H4Order =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Ordnung Harmonic 4",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonic 4 order",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    100,                                                // Maximum
    -100,                                                               // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    (INT16 *)&PDP_Input[14],                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_U16_T ParDes_Fieldbus_H4Ampl =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Amplitude Harmonic 4",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonic 4 amplitude",                             // Name
#endif
#endif
    "A",                                                            // Einheit
    10000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &PDP_Input[15],                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_U16_T ParDes_Fieldbus_H4Phase =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Phase Harmonic 4",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonic 4 phase",                             // Name
#endif
#endif
    "deg",                                                          // Einheit
    3600,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &PDP_Input[16],                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T ParDes_Fieldbus_H5Order =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Ordnung Harmonic 5",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonic 5 order",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    100,                                                // Maximum
    -100,                                                               // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    (INT16 *)&PDP_Input[17],                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_U16_T ParDes_Fieldbus_H5Ampl =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Amplitude Harmonic 5",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonic 5 amplitude",                             // Name
#endif
#endif
    "A",                                                            // Einheit
    10000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &PDP_Input[18],                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_U16_T ParDes_Fieldbus_H5Phase =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Phase Harmonic 5",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonic 5 phase",                             // Name
#endif
#endif
    "deg",                                                          // Einheit
    3600,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &PDP_Input[19],                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T ParDes_Fieldbus_H6Order =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Ordnung Harmonic 6",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonic 6 order",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    100,                                                // Maximum
    -100,                                                               // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    (INT16 *)&PDP_Input[20],                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_U16_T ParDes_Fieldbus_H6Ampl =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Amplitude Harmonic 6",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonic 6 amplitude",                             // Name
#endif
#endif
    "A",                                                            // Einheit
    10000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &PDP_Input[21],                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_U16_T ParDes_Fieldbus_H6Phase =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Phase Harmonic 6",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonic 6 phase",                             // Name
#endif
#endif
    "deg",                                                          // Einheit
    3600,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &PDP_Input[22],                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T ParDes_Fieldbus_H7Order =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Ordnung Harmonic 7",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonic 7 order",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    100,                                                // Maximum
    -100,                                                               // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    (INT16 *)&PDP_Input[23],                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_U16_T ParDes_Fieldbus_H7Ampl =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Amplitude Harmonic 7",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonic 7 amplitude",                             // Name
#endif
#endif
    "A",                                                            // Einheit
    10000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &PDP_Input[24],                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_U16_T ParDes_Fieldbus_H7Phase =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Phase Harmonic 7",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonic 7 phase",                             // Name
#endif
#endif
    "deg",                                                          // Einheit
    3600,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &PDP_Input[25],                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T ParDes_Fieldbus_H8Order =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Ordnung Harmonic 8",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonic 8 order",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    100,                                                // Maximum
    -100,                                                               // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    (INT16 *)&PDP_Input[26],                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_U16_T ParDes_Fieldbus_H8Ampl =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Amplitude Harmonic 8",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonic 8 amplitude",                             // Name
#endif
#endif
    "A",                                                            // Einheit
    10000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &PDP_Input[27],                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_U16_T ParDes_Fieldbus_H8Phase =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Phase Harmonic 8",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonic 8 phase",                             // Name
#endif
#endif
    "deg",                                                          // Einheit
    3600,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &PDP_Input[28],                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};



PAR_DES_U16_T ParDes_Fieldbus_VoltRamp =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Voltage ramp reference",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Voltage ramp reference",                               // Name
#endif
#endif
    "V",                                                            // Einheit
    10000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &PDP_Input[29],                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_U16_T ParDes_Fieldbus_FrequRamp =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Frequency ramp time",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Frequency ramp time",                              // Name
#endif
#endif
    "ms",                                                           // Einheit
    32767,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &PDP_Input[30],                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_CurrentOffsets,".extram");
//INT16 ParVal_CurrentOffsets;

PAR_DES_I16_T ParDes_CurrentOffsets =
{
    PAR_MENUE | PAR_RW | _INT16,                                    // ReadOnly
    "Current offsets",                              // Name
    NULL,                                                           // Einheit
    0,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_CurrentOffsets*/,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_OffsSelectIinv,".extram");
INT16 ParVal_OffsSelectIinv;

PAR_DES_I16_T ParDes_OffsSelectIinv =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Offset select Iinverter",                              // Name
    NULL,                                                           // Einheit
    4,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_OffsSelectIinv,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_OffsSelectIline,".extram");
INT16 ParVal_OffsSelectIline;

PAR_DES_I16_T ParDes_OffsSelectIline =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Offset select Istator",                                // Name
    NULL,                                                           // Einheit
    3,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_OffsSelectIline,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_OffsInvActU,".extram");
INT16 ParVal_OffsInvActU;

PAR_DES_I16_T ParDes_OffsInvActU =
{
    PAR_RW | _INT16,                                    // ReadOnly
    "Offset Iinv u",                                // Name
    NULL,                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_OffsInvActU,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_OffsInvActV,".extram");
INT16 ParVal_OffsInvActV;

PAR_DES_I16_T ParDes_OffsInvActV =
{
    PAR_RW | _INT16,                                    // ReadOnly
    "Offset Iinv v",                                // Name
    NULL,                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_OffsInvActV,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_OffsInvActW,".extram");
INT16 ParVal_OffsInvActW;

PAR_DES_I16_T ParDes_OffsInvActW =
{
    PAR_RW | _INT16,                                    // ReadOnly
    "Offset Iinv w",                                // Name
    NULL,                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_OffsInvActW,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_OffsLineActU,".extram");
INT16 ParVal_OffsLineActU;

PAR_DES_I16_T ParDes_OffsLineActU =
{
    PAR_RW | _INT16,                                    // ReadOnly
    "Offset Is u",                              // Name
    NULL,                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_OffsLineActU,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_OffsLineActV,".extram");
INT16 ParVal_OffsLineActV;

PAR_DES_I16_T ParDes_OffsLineActV =
{
    PAR_RW | _INT16,                                    // ReadOnly
    "Offset Is v",                              // Name
    NULL,                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_OffsLineActV,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_OffsLineActW,".extram");
INT16 ParVal_OffsLineActW;

PAR_DES_I16_T ParDes_OffsLineActW =
{
    PAR_RW | _INT16,                                    // ReadOnly
    "Offset Is w",                              // Name
    NULL,                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_OffsLineActW,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};



//#pragma DATA_SECTION(ParVal_OffsInvSaveU,".extram");
INT16 ParVal_OffsInvSaveU;

PAR_DES_I16_T ParDes_OffsInvSaveU =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Offset Iinv u saved",                              // Name
    NULL,                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_OffsInvSaveU,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_OffsInvSaveV,".extram");
INT16 ParVal_OffsInvSaveV;

PAR_DES_I16_T ParDes_OffsInvSaveV =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Offset Iinv v saved",                              // Name
    NULL,                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_OffsInvSaveV,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_OffsInvSaveW,".extram");
INT16 ParVal_OffsInvSaveW;

PAR_DES_I16_T ParDes_OffsInvSaveW =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Offset Iinv w saved",                              // Name
    NULL,                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_OffsInvSaveW,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_OffsLineSaveU,".extram");
INT16 ParVal_OffsLineSaveU;

PAR_DES_I16_T ParDes_OffsLineSaveU =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Offset Is u saved",                                // Name
    NULL,                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_OffsLineSaveU,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_OffsLineSaveV,".extram");
INT16 ParVal_OffsLineSaveV;

PAR_DES_I16_T ParDes_OffsLineSaveV =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Offset Is v saved",                                // Name
    NULL,                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_OffsLineSaveV,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_OffsLineSaveW,".extram");
INT16 ParVal_OffsLineSaveW;

PAR_DES_I16_T ParDes_OffsLineSaveW =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Offset Is w saved",                                // Name
    NULL,                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_OffsLineSaveW,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_kc_fil,".extram");
INT32 ParVal_kc_fil;

//#pragma DATA_SECTION(ParVal_Harmonic_IGain,".extram");
INT32 ParVal_Harmonic_IGain;

PAR_DES_I32_T ParDes_Harmonic_IGain =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Harmonics loop gain",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Harmonics loop gain",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    10.0*65536,                                             // Maximum
    -10.0*65536,                                                                // Minimum
    0,                                                              // Default
    1.0/65536,                    // Faktor
    &ParVal_Harmonic_IGain,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

// Droop control
PAR_DES_I16_T ParDes_DroopControl =
{
    PAR_MENUE | PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Droop-Regelung",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Droop control",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_HarmFeedbacks*/,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_kom_droop,".extram");
INT32 ParVal_kom_droop;

PAR_DES_I32_T ParDes_kom_droop =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,
#ifdef LANGUAGE_GERMAN
    "Droop-Verstärkung F",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Droop gain F",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    -1.0*65536,                                                             // Minimum
    1.0*65536,                                                              // Default
    1.0/65536,                    // Faktor
    &ParVal_kom_droop,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_kv_droop,".extram");
INT32 ParVal_kv_droop;

PAR_DES_I32_T ParDes_kv_droop =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,
#ifdef LANGUAGE_GERMAN
    "Droop-Verstärkung V",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Droop gain V",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    -1.0*65536,                                                             // Minimum
    1.0*65536,                                                              // Default
    1.0/65536,                    // Faktor
    &ParVal_kv_droop,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Lv_droop,".extram");
INT16 ParVal_Lv_droop;

PAR_DES_I16_T ParDes_Lv_droop =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Droop Induktivität",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Droop inductance",                             // Name
#endif
#endif
    "myH",                                                          // Einheit
    30000,                                              // Maximum
    -30000,                                                             // Minimum
    508,                                                                // Default
    1.0,                      // Faktor
    &ParVal_Lv_droop,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Rv_droop,".extram");
INT16 ParVal_Rv_droop;

PAR_DES_I16_T ParDes_Rv_droop =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Droop Widerstand",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Droop resistance",                             // Name
#endif
#endif
    "mOhm",                                                         // Einheit
    30000,                                              // Maximum
    -30000,                                                             // Minimum
    508,                                                                // Default
    1.0,                      // Faktor
    &ParVal_Rv_droop,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


DWORD ParVal_AutoresetMask;

PAR_DES_DWORD_T ParDes_AutoresetMask =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _DWORD,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Autoreset Fehlermaske",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Autoreset mask",                               // Name
#endif
#endif
    0xffffffff,                                             // Maximum
    0x00000000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_AutoresetMask,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

// DMA
PAR_DES_I16_T ParDes_DMA =
{
    PAR_MENUE | PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "DMA",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "DMA",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_HarmFeedbacks*/,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

DWORD ParVal_DMA_ReadAddress;
PAR_DES_DWORD_T ParDes_DMA_ReadAddress =
{
    PAR_RW | _DWORD,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "DMA Read Address",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "DMA Read Address",                             // Name
#endif
#endif
    0xffffffff,                                             // Maximum
    0x00000000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_DMA_ReadAddress,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

WORD ParVal_DMA_ReadData;
PAR_DES_WORD_T ParDes_DMA_ReadData =
{
    PAR_RO | _WORD,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "DMA Read Data",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "DMA Read Data",                                // Name
#endif
#endif
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_DMA_ReadData,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

WORD ParVal_DMA_WriteMode;
PAR_DES_WORD_T ParDes_DMA_WriteMode =
{
    PAR_RW | _WORD,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "DMA Write Mode",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "DMA Write Mode",                               // Name
#endif
#endif
    0x0002,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_DMA_WriteMode,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

DWORD ParVal_DMA_WriteAddress;
PAR_DES_DWORD_T ParDes_DMA_WriteAddress =
{
    PAR_RW | _DWORD,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "DMA Write Address",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "DMA Write Address",                                // Name
#endif
#endif
    0xffffffff,                                             // Maximum
    0x00000000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_DMA_WriteAddress,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

WORD ParVal_DMA_WriteData;
PAR_DES_WORD_T ParDes_DMA_WriteData =
{
    PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "DMA Write Data",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "DMA Write Data",                               // Name
#endif
#endif
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_DMA_WriteData,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

DWORD ParVal_RAM_test_error;
PAR_DES_DWORD_T ParDes_RAM_test_error =
{
    PAR_RO | _DWORD,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "RAM test error",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "RAM test error",                               // Name
#endif
#endif
    0xffffffff,                                             // Maximum
    0x00000000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_RAM_test_error,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

// RTC
PAR_DES_I16_T ParDes_RTC =
{
    PAR_MENUE | PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "RTC",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "RTC",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_HarmFeedbacks*/,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

typedef struct {
    UINT16  seconds;
    UINT16  minutes;
    UINT16  hours;
    UINT16  days;
    UINT16  weekdays;
    UINT16  month;
    UINT16  year;
} dt_RTC_CLOCK_PARS;


dt_RTC_CLOCK_PARS    RTC_time_read;
dt_RTC_CLOCK_PARS      RTC_time_set;

#if 1
PAR_DES_U16_T ParDes_RTC_year =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Jahr",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "year",                             // Name
#endif
#endif
    "y",                                                            // Einheit
    2999,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &RTC_time_read.year,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_U16_T ParDes_RTC_month =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Monat",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "month",                                // Name
#endif
#endif
    "m",                                                            // Einheit
    12,                                             // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &RTC_time_read.month,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_U16_T ParDes_RTC_days =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Tag",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "days",                             // Name
#endif
#endif
    "d",                                                            // Einheit
    31,                                             // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &RTC_time_read.days,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_U16_T ParDes_RTC_weekdays =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Wochentag",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "weekdays",                             // Name
#endif
#endif
    "d",                                                            // Einheit
    7,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &RTC_time_read.weekdays,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_U16_T ParDes_RTC_hours =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Stunde",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "hours",                                // Name
#endif
#endif
    "h",                                                            // Einheit
    24,                                             // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &RTC_time_read.hours,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_U16_T ParDes_RTC_minutes =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Minute",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "minutes",                              // Name
#endif
#endif
    "min",                                                          // Einheit
    60,                                             // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &RTC_time_read.minutes,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_U16_T ParDes_RTC_seconds =
{
    PAR_RO | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Sekunde",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "seconds",                              // Name
#endif
#endif
    "s",                                                            // Einheit
    60,                                             // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &RTC_time_read.seconds,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

UINT16                  RTC_set;

PAR_DES_WORD_T ParDes_RTC_set =
{
    PAR_RW | _WORD,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "RTC Zeit setzen",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "set RTC time",                             // Name
#endif
#endif
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &RTC_set,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_U16_T ParDes_RTC_year_set =
{
    PAR_RW | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Jahr",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "year",                             // Name
#endif
#endif
    "y",                                                            // Einheit
    2999,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &RTC_time_set.year,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_U16_T ParDes_RTC_month_set =
{
    PAR_RW | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Monat",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "month",                                // Name
#endif
#endif
    "m",                                                            // Einheit
    12,                                             // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &RTC_time_set.month,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_U16_T ParDes_RTC_days_set =
{
    PAR_RW | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Tag",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "days",                             // Name
#endif
#endif
    "d",                                                            // Einheit
    31,                                             // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &RTC_time_set.days,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_U16_T ParDes_RTC_weekdays_set =
{
    PAR_RW | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Wochentag",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "weekdays",                             // Name
#endif
#endif
    "d",                                                            // Einheit
    31,                                             // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &RTC_time_set.weekdays,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_U16_T ParDes_RTC_hours_set =
{
    PAR_RW | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Stunde",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "hours",                                // Name
#endif
#endif
    "h",                                                            // Einheit
    24,                                             // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &RTC_time_set.hours,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_U16_T ParDes_RTC_minutes_set =
{
    PAR_RW | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Minute",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "minutes",                              // Name
#endif
#endif
    "min",                                                          // Einheit
    60,                                             // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &RTC_time_set.minutes,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_U16_T ParDes_RTC_seconds_set =
{
    PAR_RW | _UINT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Sekunde",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "seconds",                              // Name
#endif
#endif
    "s",                                                            // Einheit
    60,                                             // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &RTC_time_set.seconds,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};
#endif

UINT8 RTC_time[20] = "00.00.00 00:00:00";

// RTC time (string)
PAR_DES_A_UINT8_T ParDes_RTC_time =
{
    PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
#ifdef LANGUAGE_GERMAN
    "RTC Zeit",                                         // Name
#else
#ifdef LANGUAGE_ENGLISH
    "RTC time",             // Name
#endif
#endif
    MAX_NAME_LEN,                                           // Maximum
    RTC_time,                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

WORD ParVal_PQcurrSrc;
PAR_DES_WORD_T ParDes_PQcurrSrc =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "PQ Stromistwert",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "PQ current feedback",                              // Name
#endif
#endif
    0x0002,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_PQcurrSrc,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};



// Parameter-Variable Island
//#pragma DATA_SECTION(ParVal_CB_mode,".extram");
WORD ParVal_CB_mode;

// Grid/Island mode
// Parameter-Beschreibung Island
PAR_DES_WORD_T ParDes_CB_mode =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "CB mode",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "CB mode",                              // Name
#endif
#endif
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_CB_mode,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_Tigbt_max,".extram");
INT16 ParVal_Tigbt_max;

PAR_DES_I16_T ParDes_Tigbt_max =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "Max. IGBT Teperatur",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Max. IGBT temperature",                                // Name
#endif
#endif
    "°C",                                                           // Einheit
    1200,                                               // Maximum
    0,                                                              // Minimum
    1050,                                                               // Default
    0.1,                      // Faktor
    &ParVal_Tigbt_max,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T ParDes_LVRT =
{
    PAR_MENUE | PAR_RW | _INT16,                                    // ReadOnly
    "LVRT HVRT",                                // Name
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_Harmonics1*/,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_kQ_FRT,".extram");
INT16 ParVal_kQ_FRT;

PAR_DES_I16_T ParDes_kQ_FRT =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "k-factor",                             // Name
    NULL,                                                           // Einheit
    10000,                                              // Maximum
    0,                                                              // Minimum
    2000,                                                               // Default
    0.001,                    // Faktor
    &ParVal_kQ_FRT,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_ut_FRT,".extram");
INT16 ParVal_ut_FRT;

PAR_DES_I16_T ParDes_ut_FRT =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "dead band",                                // Name
    NULL,                                                           // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_ut_FRT,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_vFaultStart,".extram");
INT16 ParVal_vFaultStart;

PAR_DES_I16_T ParDes_vFaultStart =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Voltage LVRT trigger",                             // Name
    "p.u.",                                                         // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_vFaultStart,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_vFaultEnd,".extram");
INT16 ParVal_vFaultEnd;

PAR_DES_I16_T ParDes_vFaultEnd =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Voltage LVRT end",                             // Name
    "p.u.",                                                         // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_vFaultEnd,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_vFaultStartHV,".extram");
INT16 ParVal_vFaultStartHV;

PAR_DES_I16_T ParDes_vFaultStartHV =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Voltage HVRT trigger",                             // Name
    "p.u.",                                                         // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_vFaultStartHV,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_vFaultEndHV,".extram");
INT16 ParVal_vFaultEndHV;

PAR_DES_I16_T ParDes_vFaultEndHV =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Voltage HVRT end",                             // Name
    "p.u.",                                                         // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_vFaultEndHV,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_v_FRT_IndStart;
PAR_DES_I16_T ParDes_v_FRT_IndStart =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "v_StartInjectInd",                             // Name
    "p.u.",                                                         // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_v_FRT_IndStart,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_v_FRT_CapStart;
PAR_DES_I16_T ParDes_v_FRT_CapStart =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "v_StartInjectCap",                             // Name
    "p.u.",                                                         // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_v_FRT_CapStart,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_iN_FRT,".extram");
INT32 ParVal_iN_FRT;

PAR_DES_I32_T ParDes_iN_FRT =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                 // ReadOnly
    "Rated LVRT current",                               // Name
    "A",                                                            // Einheit
    100000,                                             // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_iN_FRT,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_imax_LVRT,".extram");
INT16 ParVal_imax_LVRT;

PAR_DES_I16_T ParDes_imax_LVRT =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "max. current LVRT",                                // Name
    "p.u.",                                                         // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_imax_LVRT,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_imax_HVRT,".extram");
INT16 ParVal_imax_HVRT;

PAR_DES_I16_T ParDes_imax_HVRT =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "max. current HVRT",                                // Name
    "p.u.",                                                         // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_imax_HVRT,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_imax_asym,".extram");
INT16 ParVal_imax_asym;

PAR_DES_I16_T ParDes_imax_asym =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "max. current asymm. fault",                                // Name
    "p.u.",                                                         // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_imax_asym,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_Vlim_asym,".extram");
INT16 ParVal_Vlim_asym;

PAR_DES_I16_T ParDes_Vlim_asym =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Trigger voltage asymm. fault",                             // Name
    "p.u.",                                                         // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_Vlim_asym,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_t_FRT_overtravel;
PAR_DES_I16_T ParDes_t_FRT_overtravel =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "t_FRT_overtravel",                             // Name
    "s",                                                            // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_t_FRT_overtravel,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_Tf_FRT_recover;
PAR_DES_I16_T ParDes_Tf_FRT_recover =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Tf_FRT_recover",                               // Name
    "ms",                                                           // Einheit
    30000,                                              // Maximum
    1,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_Tf_FRT_recover,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_tLV1,".extram");
//#pragma DATA_SECTION(ParVal_tLV2,".extram");
//#pragma DATA_SECTION(ParVal_tLV3,".extram");
//#pragma DATA_SECTION(ParVal_tLV4,".extram");
//#pragma DATA_SECTION(ParVal_tLV5,".extram");
//#pragma DATA_SECTION(ParVal_tLV6,".extram");
INT16 ParVal_tLV1;
INT16 ParVal_tLV2;
INT16 ParVal_tLV3;
INT16 ParVal_tLV4;
INT16 ParVal_tLV5;
INT16 ParVal_tLV6;

PAR_DES_I16_T ParDes_tLV1 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "t_LVRT_1",                             // Name
    "s",                                                            // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_tLV1,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_tLV2 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "t_LVRT_2",                             // Name
    "s",                                                            // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_tLV2,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_tLV3 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "t_LVRT_3",                             // Name
    "s",                                                            // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_tLV3,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_tLV4 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "t_LVRT_4",                             // Name
    "s",                                                            // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_tLV4,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_tLV5 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "t_LVRT_5",                             // Name
    "s",                                                            // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_tLV5,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_tLV6 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "t_LVRT_6",                             // Name
    "s",                                                            // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_tLV6,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_tHV1,".extram");
//#pragma DATA_SECTION(ParVal_tHV2,".extram");
//#pragma DATA_SECTION(ParVal_tHV3,".extram");
//#pragma DATA_SECTION(ParVal_tHV4,".extram");
//#pragma DATA_SECTION(ParVal_tHV5,".extram");
//#pragma DATA_SECTION(ParVal_tHV6,".extram");
INT16 ParVal_tHV1;
INT16 ParVal_tHV2;
INT16 ParVal_tHV3;
INT16 ParVal_tHV4;
INT16 ParVal_tHV5;
INT16 ParVal_tHV6;

PAR_DES_I16_T ParDes_tHV1 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "t_HVRT_1",                             // Name
    "s",                                                            // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_tHV1,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_tHV2 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "t_HVRT_2",                             // Name
    "s",                                                            // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_tHV2,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_tHV3 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "t_HVRT_3",                             // Name
    "s",                                                            // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_tHV3,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_tHV4 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "t_HVRT_4",                             // Name
    "s",                                                            // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_tHV4,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_tHV5 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "t_HVRT_5",                             // Name
    "s",                                                            // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_tHV5,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_tHV6 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "t_HVRT_6",                             // Name
    "s",                                                            // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_tHV6,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_vLV1,".extram");
//#pragma DATA_SECTION(ParVal_vLV2,".extram");
//#pragma DATA_SECTION(ParVal_vLV3,".extram");
//#pragma DATA_SECTION(ParVal_vLV4,".extram");
//#pragma DATA_SECTION(ParVal_vLV5,".extram");
//#pragma DATA_SECTION(ParVal_vLV6,".extram");
INT16 ParVal_vLV1;
INT16 ParVal_vLV2;
INT16 ParVal_vLV3;
INT16 ParVal_vLV4;
INT16 ParVal_vLV5;
INT16 ParVal_vLV6;

PAR_DES_I16_T ParDes_vLV1 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "V_LVRT_1",                             // Name
    "p.u.",                                                         // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_vLV1,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_vLV2 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "V_LVRT_2",                             // Name
    "p.u.",                                                         // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_vLV2,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_vLV3 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "V_LVRT_3",                             // Name
    "p.u.",                                                         // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_vLV3,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_vLV4 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "V_LVRT_4",                             // Name
    "p.u.",                                                         // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_vLV4,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_vLV5 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "V_LVRT_5",                             // Name
    "p.u.",                                                         // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_vLV5,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_vLV6 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "V_LVRT_6",                             // Name
    "p.u.",                                                         // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_vLV6,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

//#pragma DATA_SECTION(ParVal_vHV1,".extram");
//#pragma DATA_SECTION(ParVal_vHV2,".extram");
//#pragma DATA_SECTION(ParVal_vHV3,".extram");
//#pragma DATA_SECTION(ParVal_vHV4,".extram");
//#pragma DATA_SECTION(ParVal_vHV5,".extram");
//#pragma DATA_SECTION(ParVal_vHV6,".extram");
INT16 ParVal_vHV1;
INT16 ParVal_vHV2;
INT16 ParVal_vHV3;
INT16 ParVal_vHV4;
INT16 ParVal_vHV5;
INT16 ParVal_vHV6;

PAR_DES_I16_T ParDes_vHV1 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "V_HVRT_1",                             // Name
    "p.u.",                                                         // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_vHV1,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_vHV2 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "V_HVRT_2",                             // Name
    "p.u.",                                                         // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_vHV2,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_vHV3 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "V_HVRT_3",                             // Name
    "p.u.",                                                         // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_vHV3,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_vHV4 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "V_HVRT_4",                             // Name
    "p.u.",                                                         // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_vHV4,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_vHV5 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "V_HVRT_5",                             // Name
    "p.u.",                                                         // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_vHV5,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_vHV6 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "V_HVRT_6",                             // Name
    "p.u.",                                                         // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_vHV6,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Event parameters
PAR_DES_I16_T ParDes_Events =
{
    PAR_MENUE | PAR_RO | _INT16,                                    // ReadOnly
    "Events",                               // Name
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

#define NO_EVENTPARS       40

//#pragma DATA_SECTION(ParVal_Event,".extram");
UINT8 ParVal_Event[NO_EVENTPARS][MAX_EVENTSTRING_LEN];
PAR_DES_A_UINT8_T ParDes_Event1 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 1",              // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[0],                                    // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event2 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 2",              // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[1],                                    // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event3 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 3",              // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[2],                                    // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event4 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 4",              // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[3],                                    // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event5 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 5",              // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[4],                                    // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event6 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 6",              // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[5],                                    // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event7 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 7",              // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[6],                                    // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event8 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 8",              // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[7],                                    // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event9 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 9",              // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[8],                                    // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event10 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 10",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[9],                                    // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event11 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 11",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[10],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event12 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 12",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[11],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event13 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 13",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[12],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event14 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 14",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[13],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event15 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 15",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[14],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event16 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 16",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[15],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event17 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 17",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[16],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event18 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 18",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[17],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event19 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 19",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[18],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event20 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 20",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[19],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event21 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 21",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[20],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event22 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 22",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[21],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event23 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 23",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[22],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event24 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 24",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[23],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event25 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 25",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[24],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event26 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 26",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[25],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event27 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 27",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[26],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event28 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 28",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[27],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event29 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 29",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[28],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event30 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 30",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[29],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event31 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 31",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[30],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event32 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 32",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[31],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event33 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 33",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[32],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event34 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 34",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[33],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event35 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 35",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[34],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event36 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 36",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[35],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event37 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 37",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[36],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event38 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 38",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[37],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event39 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 39",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[38],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_A_UINT8_T ParDes_Event40 =
{
    PAR_STORE | PAR_RO | _A_UINT8,                                      // ReadOnly, UINT8-Parameter
    "Event 40",             // Name
    MAX_NAME_LEN,                                           // Maximum
    ParVal_Event[39],                                   // Parameter
    NULL                                                    // Behandlungsfunktion
};

// Climate
PAR_DES_I16_T ParDes_Climate =
{
    PAR_MENUE | PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Klimatisierung",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Climatisation",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_WaterTempMinWarn;
PAR_DES_I16_T ParDes_WaterTempMinWarn =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Water temperature < (Warning)",                                // Name
    "°C",                                                           // Einheit
    1000,                                               // Maximum
    -1000,                                                              // Minimum
    50,                                                             // Default
    0.1,                      // Faktor
    &ParVal_WaterTempMinWarn,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_WaterTempMaxWarn;
PAR_DES_I16_T ParDes_WaterTempMaxWarn =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Water temperature > (Warning)",                                // Name
    "°C",                                                           // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    600,                                                                // Default
    0.1,                      // Faktor
    &ParVal_WaterTempMaxWarn,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_WaterTempMaxFault;
PAR_DES_I16_T ParDes_WaterTempMaxFault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Water temperature > (Fault)",                              // Name
    "°C",                                                           // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    700,                                                                // Default
    0.1,                      // Faktor
    &ParVal_WaterTempMaxFault,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_WaterTempHeatexOn;
PAR_DES_I16_T ParDes_WaterTempHeatexOn =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Heat exchanger On",                                // Name
    "°C",                                                           // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    700,                                                                // Default
    0.1,                      // Faktor
    &ParVal_WaterTempHeatexOn,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_WaterTempHeatexOff;
PAR_DES_I16_T ParDes_WaterTempHeatexOff =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Heat exchanger Off",                               // Name
    "°C",                                                           // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    700,                                                                // Default
    0.1,                      // Faktor
    &ParVal_WaterTempHeatexOff,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_TempHyst;
PAR_DES_I16_T ParDes_TempHyst =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Temperature hysteresis",                               // Name
    "°C",                                                           // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    50,                                                             // Default
    0.1,                      // Faktor
    &ParVal_TempHyst,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_AirTempMaxWarn;
PAR_DES_I16_T ParDes_AirTempMaxWarn =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Air temperature > (Warning)",                              // Name
    "°C",                                                           // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    700,                                                                // Default
    0.1,                      // Faktor
    &ParVal_AirTempMaxWarn,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_AirTempMaxFault;
PAR_DES_I16_T ParDes_AirTempMaxFault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Air temperature > (Fault)",                                // Name
    "°C",                                                           // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    800,                                                                // Default
    0.1,                      // Faktor
    &ParVal_AirTempMaxFault,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_FlowMinWarn;
PAR_DES_I16_T ParDes_FlowMinWarn =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Cooling flow < (Warning)",                             // Name
    "l/min",                                                            // Einheit
    10000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_FlowMinWarn,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_FlowMinFault;
PAR_DES_I16_T ParDes_FlowMinFault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Cooling flow < (Fault)",                               // Name
    "l/min",                                                            // Einheit
    10000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_FlowMinFault,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_FlowMaxWarn;
PAR_DES_I16_T ParDes_FlowMaxWarn =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Cooling flow > (Warning)",                             // Name
    "l/min",                                                            // Einheit
    10000,                                              // Maximum
    0,                                                              // Minimum
    4000,                                                               // Default
    0.1,                      // Faktor
    &ParVal_FlowMaxWarn,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_FlowMaxFault;
PAR_DES_I16_T ParDes_FlowMaxFault =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Cooling flow > (Fault)",                               // Name
    "l/min",                                                            // Einheit
    10000,                                              // Maximum
    0,                                                              // Minimum
    5000,                                                               // Default
    0.1,                      // Faktor
    &ParVal_FlowMaxFault,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_FlowHyst;
PAR_DES_I16_T ParDes_FlowHyst =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Cooling flow hysteresis",                              // Name
    "l/min",                                                            // Einheit
    10000,                                              // Maximum
    0,                                                              // Minimum
    10,                                                             // Default
    0.1,                      // Faktor
    &ParVal_FlowHyst,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_Pressure_max_trip;
PAR_DES_I16_T ParDes_Pressure_max_trip =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Cooling pressure max. trip",                               // Name
    "mbar",                                                         // Einheit
    10000,                                              // Maximum
    0,                                                              // Minimum
    2500,                                                               // Default
    1.0,                      // Faktor
    &ParVal_Pressure_max_trip,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_WaterInTemp;
PAR_DES_I16_T ParDes_WaterInTemp =
{
    PAR_RO | _INT16,                                    // ReadOnly
    "Water temperature inlet",                              // Name
    "°C",                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_WaterInTemp,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_WaterOutTemp;
PAR_DES_I16_T ParDes_WaterOutTemp =
{
    PAR_RO | _INT16,                                    // ReadOnly
    "Water temperature outlet",                             // Name
    "°C",                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_WaterOutTemp,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_Air1Temp;
PAR_DES_I16_T ParDes_Air1Temp =
{
    PAR_RO | _INT16,                                    // ReadOnly
    "Air temperature 1",                                // Name
    "°C",                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Air1Temp,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_Air2Temp;
PAR_DES_I16_T ParDes_Air2Temp =
{
    PAR_RO | _INT16,                                    // ReadOnly
    "Air temperature 2",                                // Name
    "°C",                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_Air2Temp,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_PressureIn;
PAR_DES_I16_T ParDes_PressureIn =
{
    PAR_RO | _INT16,                                    // ReadOnly
    "Water pressure inlet",                             // Name
    "mbar",                                                         // Einheit
    10000,                                              // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_PressureIn,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_PressureOut;
PAR_DES_I16_T ParDes_PressureOut =
{
    PAR_RO | _INT16,                                    // ReadOnly
    "Water pressure outlet",                                // Name
    "mbar",                                                         // Einheit
    10000,                                              // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_PressureOut,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_CoolingFlow;
PAR_DES_I16_T ParDes_CoolingFlow =
{
    PAR_RO | _INT16,                                    // ReadOnly
    "Cooling water flow",                               // Name
    "l/min",                                                            // Einheit
    10000,                                              // Maximum
    -1000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_CoolingFlow,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_Tf_Qref;
PAR_DES_I16_T ParDes_Tf_Qref =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Q ref time constant",                              // Name
    "ms",                                                           // Einheit
    30000,                                              // Maximum
    5,                                                              // Minimum
    100,                                                                // Default
    1.0,                      // Faktor
    &ParVal_Tf_Qref,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

// HW Faults
PAR_DES_I16_T ParDes_HW_fault =
{
    PAR_MENUE | PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Hardware Fehler und Masken",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Hardware faults and masks",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

typedef struct {
         UINT16 gdi_det;
         UINT16 gdi_err;
         UINT16 gdi_ovt;
         UINT16 trp_ocn;
         UINT16 trp_ocp;
         UINT16 err_glb;
} dt_HW_FAULTS;

dt_HW_FAULTS         HW_Faults;
dt_HW_FAULTS         HW_Masks;
dt_HW_FAULTS         HW_Masks_set;


PAR_DES_WORD_T ParDes_gdi_det =
{
    PAR_RO | _WORD,                                 // ReadOnly
    "Gate driver present",                              // Name
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &HW_Faults.gdi_det,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_WORD_T ParDes_gdi_err =
{
    PAR_RO | _WORD,                                 // ReadOnly
    "Gate driver error",                                // Name
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &HW_Faults.gdi_err,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_WORD_T ParDes_gdi_ovt =
{
    PAR_RO | _WORD,                                 // ReadOnly
    "Gate driver overtemp.",                                // Name
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &HW_Faults.gdi_ovt,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_WORD_T ParDes_trp_ocn =
{
    PAR_RO | _WORD,                                 // ReadOnly
    "Gate driver OCN",                              // Name
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &HW_Faults.trp_ocn,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_WORD_T ParDes_trp_ocp =
{
    PAR_RO | _WORD,                                 // ReadOnly
    "Gate driver OCP",                              // Name
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &HW_Faults.trp_ocp,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_WORD_T ParDes_err_glb =
{
    PAR_RO | _WORD,                                 // ReadOnly
    "Vdc+ / global fault",                              // Name
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &HW_Faults.err_glb,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_WORD_T ParDes_ms_gdi_det =
{
    PAR_RO | _WORD,                                 // ReadOnly
    "MS: Gate driver present",                              // Name
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &HW_Masks.gdi_det,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_WORD_T ParDes_ms_gdi_err =
{
    PAR_RO | _WORD,                                 // ReadOnly
    "MS: Gate driver error",                                // Name
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &HW_Masks.gdi_err,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_WORD_T ParDes_ms_gdi_ovt =
{
    PAR_RO | _WORD,                                 // ReadOnly
    "MS: Gate driver overtemp.",                                // Name
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &HW_Masks.gdi_ovt,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_WORD_T ParDes_ms_trp_ocn =
{
    PAR_RO | _WORD,                                 // ReadOnly
    "MS: Gate driver OCN",                              // Name
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &HW_Masks.trp_ocn,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_WORD_T ParDes_ms_trp_ocp =
{
    PAR_RO | _WORD,                                 // ReadOnly
    "MS: Gate driver OCP",                              // Name
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &HW_Masks.trp_ocp,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_WORD_T ParDes_ms_err_glb =
{
    PAR_RO | _WORD,                                 // ReadOnly
    "MS: Vdc+ / global fault",                              // Name
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &HW_Masks.err_glb,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_WORD_T ParDes_ms_set_gdi_det =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,                                  // ReadOnly
    "MS set: Gate driver present",                              // Name
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &HW_Masks_set.gdi_det,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_WORD_T ParDes_ms_set_gdi_err =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,                                  // ReadOnly
    "MS set: Gate driver error",                                // Name
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &HW_Masks_set.gdi_err,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_WORD_T ParDes_ms_set_gdi_ovt =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,                                  // ReadOnly
    "MS set: Gate driver overtemp.",                                // Name
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &HW_Masks_set.gdi_ovt,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_WORD_T ParDes_ms_set_trp_ocn =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,                                  // ReadOnly
    "MS set: Gate driver OCN",                              // Name
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &HW_Masks_set.trp_ocn,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_WORD_T ParDes_ms_set_trp_ocp =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,                                  // ReadOnly
    "MS set: Gate driver OCP",                              // Name
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &HW_Masks_set.trp_ocp,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_WORD_T ParDes_ms_set_err_glb =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,                                  // ReadOnly
    "MS set: Vdc+ / global fault",                              // Name
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &HW_Masks_set.err_glb,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

// GD Temperatures
PAR_DES_I16_T ParDes_GD_temps =
{
    PAR_MENUE | PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "GD Temperatures",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Gate driver temperatures",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


typedef struct {
         INT16 t_igbt1;
         INT16 t_igbt2;
         INT16 t_igbt3;
         INT16 t_igbt4;
         INT16 t_igbt5;
         INT16 t_igbt6;
         INT16 t_igbt7;
         INT16 t_igbt8;
         INT16 t_igbt9;
         INT16 t_brd;
         INT16 h_brd;
} dt_GD_TEMPS;

dt_GD_TEMPS          GD_Temps;

PAR_DES_I16_T ParDes_t_igbt1 =
{
    PAR_RO | _INT16,                                    // ReadOnly
    "GD 1 temperature",                             // Name
    "°C",                                                           // Einheit
    2000,                                               // Maximum
    -1000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &GD_Temps.t_igbt1,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_t_igbt2 =
{
    PAR_RO | _INT16,                                    // ReadOnly
    "GD 2 temperature",                             // Name
    "°C",                                                           // Einheit
    2000,                                               // Maximum
    -1000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &GD_Temps.t_igbt2,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_t_igbt3 =
{
    PAR_RO | _INT16,                                    // ReadOnly
    "GD 3 temperature",                             // Name
    "°C",                                                           // Einheit
    2000,                                               // Maximum
    -1000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &GD_Temps.t_igbt3,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_t_igbt4 =
{
    PAR_RO | _INT16,                                    // ReadOnly
    "GD 4 temperature",                             // Name
    "°C",                                                           // Einheit
    2000,                                               // Maximum
    -1000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &GD_Temps.t_igbt4,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_t_igbt5 =
{
    PAR_RO | _INT16,                                    // ReadOnly
    "GD 5 temperature",                             // Name
    "°C",                                                           // Einheit
    2000,                                               // Maximum
    -1000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &GD_Temps.t_igbt5,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_t_igbt6 =
{
    PAR_RO | _INT16,                                    // ReadOnly
    "GD 6 temperature",                             // Name
    "°C",                                                           // Einheit
    2000,                                               // Maximum
    -1000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &GD_Temps.t_igbt6,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_t_igbt7 =
{
    PAR_RO | _INT16,                                    // ReadOnly
    "GD 7 temperature",                             // Name
    "°C",                                                           // Einheit
    2000,                                               // Maximum
    -1000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &GD_Temps.t_igbt7,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_t_igbt8 =
{
    PAR_RO | _INT16,                                    // ReadOnly
    "GD 8 temperature",                             // Name
    "°C",                                                           // Einheit
    2000,                                               // Maximum
    -1000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &GD_Temps.t_igbt8,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_t_igbt9 =
{
    PAR_RO | _INT16,                                    // ReadOnly
    "GD 9 temperature",                             // Name
    "°C",                                                           // Einheit
    2000,                                               // Maximum
    -1000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &GD_Temps.t_igbt9,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_t_brd =
{
    PAR_RO | _INT16,                                    // ReadOnly
    "Board temperature",                                // Name
    "°C",                                                           // Einheit
    2000,                                               // Maximum
    -1000,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &GD_Temps.t_brd,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_h_brd =
{
    PAR_RO | _INT16,                                    // ReadOnly
    "Board humidity",                               // Name
    "%",                                                            // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &GD_Temps.h_brd,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


uint16_T HWM_set;                      /* '<Root>/P_Vline_tripp142' */

PAR_DES_WORD_T ParDes_HWM_set =
{
    PAR_RW | _WORD,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "HW Maske setzen",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "set HW mask",                              // Name
#endif
#endif
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &HWM_set,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


INT32 ParVal_IreLim_PD;
PAR_DES_I32_T ParDes_IreLim_PD =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Maximum Wirkleistung",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Active power maximum",                             // Name
#endif
#endif
    "kW",                                                           // Einheit
    300000,                                             // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_IreLim_PD,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T ParDes_Q_V =
{
    PAR_MENUE | PAR_RW | _INT16,                                    // ReadOnly
    "Q(V) characteristic",                              // Name
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_Tf_Q_V;
PAR_DES_I16_T ParDes_Tf_Q_V =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Q(V) time constant",                               // Name
    "ms",                                                           // Einheit
    30000,                                              // Maximum
    20,                                                             // Minimum
    100,                                                                // Default
    1.0,                      // Faktor
    &ParVal_Tf_Q_V,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_Q_V_v0;
INT16 ParVal_Q_V_v1;
INT16 ParVal_Q_V_v2;
INT16 ParVal_Q_V_v3;
INT16 ParVal_Q_V_v4;
INT16 ParVal_Q_V_v5;
INT16 ParVal_Q_V_v6;

PAR_DES_I16_T ParDes_Q_V_v0 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Q_V_v0",                               // Name
    "p.u.",                                                         // Einheit
    1000,                                               // Maximum
    -1000,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_Q_V_v0,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_Q_V_v1 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Q_V_v1",                               // Name
    "p.u.",                                                         // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_Q_V_v1,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_Q_V_v2 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Q_V_v2",                               // Name
    "p.u.",                                                         // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_Q_V_v2,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_Q_V_v3 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Q_V_v3",                               // Name
    "p.u.",                                                         // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_Q_V_v3,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_Q_V_v4 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Q_V_v4",                               // Name
    "p.u.",                                                         // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_Q_V_v4,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_Q_V_v5 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Q_V_v5",                               // Name
    "p.u.",                                                         // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_Q_V_v5,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_Q_V_v6 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Q_V_v6",                               // Name
    "p.u.",                                                         // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_Q_V_v6,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_Q_V_q1;
INT16 ParVal_Q_V_q2;
INT16 ParVal_Q_V_q3;
INT16 ParVal_Q_V_q4;
INT16 ParVal_Q_V_q5;
INT16 ParVal_Q_V_q6;

PAR_DES_I16_T ParDes_Q_V_q1 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Q_V_q1",                               // Name
    "p.u.",                                                         // Einheit
    1000,                                               // Maximum
    -1000,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_Q_V_q1,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_Q_V_q2 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Q_V_q2",                               // Name
    "p.u.",                                                         // Einheit
    1000,                                               // Maximum
    -1000,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_Q_V_q2,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_Q_V_q3 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Q_V_q3",                               // Name
    "p.u.",                                                         // Einheit
    1000,                                               // Maximum
    -1000,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_Q_V_q3,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_Q_V_q4 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Q_V_q4",                               // Name
    "p.u.",                                                         // Einheit
    1000,                                               // Maximum
    -1000,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_Q_V_q4,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_Q_V_q5 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Q_V_q5",                               // Name
    "p.u.",                                                         // Einheit
    1000,                                               // Maximum
    -1000,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_Q_V_q5,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes_Q_V_q6 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "Q_V_q6",                               // Name
    "p.u.",                                                         // Einheit
    1000,                                               // Maximum
    -1000,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_Q_V_q6,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_v_PLL_freeze_on;
PAR_DES_I16_T ParDes_v_PLL_freeze_on =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "v_PLL_freeze_on",                              // Name
    "p.u.",                                                         // Einheit
    1000,                                               // Maximum
    -1000,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_v_PLL_freeze_on,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_v_PLL_freeze_off;
PAR_DES_I16_T ParDes_v_PLL_freeze_off =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "v_PLL_freeze_off",                             // Name
    "p.u.",                                                         // Einheit
    1000,                                               // Maximum
    -1000,                                                              // Minimum
    0,                                                              // Default
    0.001,                    // Faktor
    &ParVal_v_PLL_freeze_off,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T ParDes_ChopperControl =
{
    PAR_MENUE | PAR_RW | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Chopperregelung",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Chopper control",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_MaxChopOntime;
PAR_DES_I16_T ParDes_MaxChopOntime =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Max. Chopper Ontime",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Max. chopper ontime",                              // Name
#endif
#endif
    "ms",                                                           // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_MaxChopOntime,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_VdcChopOn;
PAR_DES_I16_T ParDes_VdcChopOn =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Chopper On Spannung",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Chopper on voltage",                               // Name
#endif
#endif
    "V",                                                            // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    650,                                                                // Default
    1.0,                      // Faktor
    &ParVal_VdcChopOn,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


INT16 ParVal_VdcChopOff;
PAR_DES_I16_T ParDes_VdcChopOff =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Chopper Off Spannung",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Chopper off voltage",                              // Name
#endif
#endif
    "V",                                                            // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    650,                                                                // Default
    1.0,                      // Faktor
    &ParVal_VdcChopOff,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_ChopTempRel;
PAR_DES_I16_T ParDes_ChopTempRel =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "ChopTempRel",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_ChopTempRel,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};


PAR_DES_I16_T ParDes_Statistics =
{
    PAR_MENUE | PAR_RO | _INT16,                                    // ReadOnly
    "Statistics",                               // Name
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

UINT32 ParVal_TimeSinceReboot_h01;
UINT32 ParVal_TotalRuntime_h01;
UINT32 ParVal_TimeInFault_h01;
UINT32 ParVal_TotalRuntime_inc;
UINT32 ParVal_TimeInFault_inc;
UINT32 ParVal_NumberFaults;

PAR_DES_U32_T ParDes_TimeSinceReboot_h01 =
{
    PAR_STORE | PAR_RO | _UINT32,                                   // ReadOnly, UINT16-Parameter
    "Time since reboot",                    // Name
    "h",                                                            // Einheit
    0xFFFFFFFF,                                                     // Maximum
    0x00000000,                                                     // Minimum
    0,                                  // Default
    0.1,                                                            // Faktor
    &ParVal_TimeSinceReboot_h01,                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_U32_T ParDes_TotalRuntime_h01 =
{
    PAR_STORE | PAR_RO | _UINT32,                                   // ReadOnly, UINT16-Parameter
    "Total runtime",                    // Name
    "h",                                                            // Einheit
    0xFFFFFFFF,                                                     // Maximum
    0x00000000,                                                     // Minimum
    0,                                  // Default
    0.1,                                                            // Faktor
    &ParVal_TotalRuntime_h01,                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_U32_T ParDes_TimeInFault_h01 =
{
    PAR_STORE | PAR_RO | _UINT32,                                   // ReadOnly, UINT16-Parameter
    "Time in fault",                    // Name
    "h",                                                            // Einheit
    0xFFFFFFFF,                                                     // Maximum
    0x00000000,                                                     // Minimum
    0,                                  // Default
    0.1,                                                            // Faktor
    &ParVal_TimeInFault_h01,                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_U32_T ParDes_TotalRuntime_inc =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _UINT32,                                    // ReadOnly, UINT16-Parameter
    "Total runtime counter",                    // Name
    "",                                                         // Einheit
    0xFFFFFFFF,                                                     // Maximum
    0x00000000,                                                     // Minimum
    0,                                  // Default
    1.0,                                                            // Faktor
    &ParVal_TotalRuntime_inc,                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_U32_T ParDes_TimeInFault_inc =
{
    PAR_STORE | PAR_STORE_EE | PAR_RO | _UINT32,                                    // ReadOnly, UINT16-Parameter
    "Time in fault counter",                    // Name
    "",                                                         // Einheit
    0xFFFFFFFF,                                                     // Maximum
    0x00000000,                                                     // Minimum
    0,                                  // Default
    1.0,                                                            // Faktor
    &ParVal_TimeInFault_inc,                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_U32_T ParDes_NumberFaults =
{
    PAR_PASS_PW | PAR_RW | PAR_STORE | PAR_STORE_EE | _UINT32,                                  // ReadOnly, UINT16-Parameter
    "Number of faults",                 // Name
    "",                                                         // Einheit
    0xFFFFFFFF,                                                     // Maximum
    0x00000000,                                                     // Minimum
    0,                                  // Default
    1.0,                                                            // Faktor
    &ParVal_NumberFaults,                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


WORD ParVal_ClearStatistics;
PAR_DES_WORD_T ParDes_ClearStatistics =
{
    PAR_PASS_PW | PAR_RW | _WORD,
    "Clear statistics",                             // Name
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_ClearStatistics,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


WORD ParVal_SaveStatistics;
PAR_DES_WORD_T ParDes_SaveStatistics =
{
    PAR_PASS_PW | PAR_RW | _WORD,
    "Save statistics",                              // Name
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_SaveStatistics,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_IdleCnt;
PAR_DES_I16_T ParDes_IdleCnt =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "Idle counter",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_IdleCnt,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};


INT16 ParVal_AutoresetMaxInterval;
PAR_DES_I16_T ParDes_AutoresetMaxInterval =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Autoresetintervall",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Max. autoreset interval",                              // Name
#endif
#endif
    "min",                                                          // Einheit
    32767,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AutoresetMaxInterval,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_AutoresetMaxNumber;
PAR_DES_I16_T ParDes_AutoresetMaxNumber =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Max. Autoresets in Intervall",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "Max. autoresets in interval",                              // Name
#endif
#endif
    "",                                                         // Einheit
    32767,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &ParVal_AutoresetMaxNumber,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

WORD ParVal_ClimateFault;
PAR_DES_WORD_T ParDes_ClimateFault =
{
    PAR_RO | _WORD,                                 // ReadOnly
    "Climate faults",                               // Name
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_ClimateFault,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

WORD ParVal_ClimateWarn;
PAR_DES_WORD_T ParDes_ClimateWarn =
{
    PAR_RO | _WORD,                                 // ReadOnly
    "Climate warnings",                             // Name
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &ParVal_ClimateWarn,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


INT16 ParVal_Ifb_sel;
PAR_DES_I16_T  ParDes_Ifb_sel =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
    "Current fbk. select",                              // Name
    NULL,                                                           // Einheit
    0x000f,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &ParVal_Ifb_sel,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


INT32 ParVal_k_iref_pn;
PAR_DES_I32_T  ParDes_k_iref_pn =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
    "k_iref_pn",                                // Name
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    0,                                                              // Minimum
    65536,                                                              // Default
    1.0/65536,                    // Faktor
    &ParVal_k_iref_pn,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


INT32 ParVal_k_iref_np;
PAR_DES_I32_T  ParDes_k_iref_np =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
    "k_iref_np",                                // Name
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    0,                                                              // Minimum
    65536,                                                              // Default
    1.0/65536,                    // Faktor
    &ParVal_k_iref_np,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

INT16 ParVal_iInv_fb_pd;
INT16 ParVal_iInv_fb_pq;
INT16 ParVal_iInv_fb_nd;
INT16 ParVal_iInv_fb_nq;
PAR_DES_I16_T  ParDes_iInv_fb_pd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iInv_fb_pd",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iInv_fb_pd,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes_iInv_fb_pq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iInv_fb_pq",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iInv_fb_pq,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes_iInv_fb_nd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iInv_fb_nd",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iInv_fb_nd,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes_iInv_fb_nq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "iInv_fb_nq",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &ParVal_iInv_fb_nq,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};




// Eintrag in Parametertabelle für einen Parameter
typedef const struct
{
    UINT16 index;                       //!< El. 0, ParameterNr.
    //const UINT16* Attr;       //!< El. 1, Attribut des Parameters
    void* ParDes;                       //!< Zeiger auf Beschreibungsstruktur
} PAR_TAB_ENTRY_T;


PAR_TAB_ENTRY_T ParTable[/*MAX_PAR*/] =  // {Par.-Nr. , Par.-Attribut, Zeiger auf Beschreibungsstruktur}
{
//------Geräteidentifikation
{0,     (void*)&ParDes_MaxParams},
#ifdef STRING_PARAMS
{1,     (void*)&ParDes_PROJECT_NAME},
//------Geräteidentifikation
{2,     (void*)&ParDes_GROUP_DEVICEIDENT},
{3,     (void*)&ParDes_DeviceType},
{4,     (void*)&ParDes_HWVersion},
{5,     (void*)&ParDes_SWVersion},
{6,     (void*)&ParDes_SWBuildNum},
{7,     (void*)&ParDes_SWBuildDate},
{8,     (void*)&ParDes_SWToolVersion},
#else
{1,     (void*)&ParDes_DeviceIdent},
{2,     (void*)&ParDes_HWVersion},
{3,     (void*)&ParDes_SWVersion},
{4,     (void*)&ParDes_SWBuildNum},
{5,     (void*)&ParDes_SWBuildDate},
{6,     (void*)&ParDes_SWToolVersion},
//{7,     (void*)&ParDes_NVmemControl},
#endif
{9,     (void*)&ParDes_Passcode},

//------Netz-------------------------------------25
{10,    (void*)&ParDes_Grid},
{11,    (void*)&ParDes_VNline},

{12,    (void*)&ParDes_FNline},
{13,    (void*)&ParDes_RangeIL},
{14,    (void*)&ParDes_RangeVL},
{15,    (void*)&ParDes_PMode},
{16,    (void*)&ParDes_TrafoRatio},
{17,    (void*)&ParDes_TrafoPhase},
{18,    (void*)&ParDes_PhiVmes},
{19,    (void*)&ParDes_CB_mode},

//------------25
{20,    (void*)&ParDes_Inverter},
{21,    (void*)&ParDes_INinv},
{22,    (void*)&ParDes_RangeI},
{23,    (void*)&ParDes_PhiImes},
{24,    (void*)&ParDes_Cfil},
{25,    (void*)&ParDes_Ldr},
{26,    (void*)&ParDes_Rdr},
{27,    (void*)&ParDes_Ldr_trans},
{28,    (void*)&ParDes_LSI_IdentEn},
{29,    (void*)&ParDes_TempChar},

//----------------25
{30,    (void*)&ParDes_DC_link},
{31,    (void*)&ParDes_VdcN},
{32,    (void*)&ParDes_Vdcmax},
{33,    (void*)&ParDes_Vdc_tripp},
{34,    (void*)&ParDes_Vdc_tripm},
{35,    (void*)&ParDes_Cdc},
{36,    (void*)&ParDes_RangeVdc},
{37,    (void*)&ParDes_Vdc_min_on},
{38,    (void*)&ParDes_z1Vdc},

//------25
{40,    (void*)&ParDes_Reference},
{41,    (void*)&ParDes_IreRef},
{42,    (void*)&ParDes_CosPhiRef},
{43,    (void*)&ParDes_QMode},
{44,    (void*)&ParDes_IreLim},
{45,    (void*)&ParDes_VI_phi},
{46,    (void*)&ParDes_VP_phi},
{47,    (void*)&ParDes_IreLim_PD},
{48,    (void*)&ParDes_Tf_Qref},
{49,    (void*)&ParDes_RefSrc},

//-----------------------------
{50,    (void*)&ParDes_Feedbacks},
{51,    (void*)&ParDes_Vline},
{52,    (void*)&ParDes_Fline},
{53,    (void*)&ParDes_Vdc},
{54,    (void*)&ParDes_Iinv},
{55,    (void*)&ParDes_Tigbt},
{56,    (void*)&ParDes_DigIn},
{57,    (void*)&ParDes_DigOut},
{58,    (void*)&ParDes_ILine},
{59,    (void*)&ParDes_CosPhiLine},
{60,    (void*)&ParDes_CosPhiInv},
{61,    (void*)&ParDes_Is},
{62,    (void*)&ParDes_Ps},
{63,    (void*)&ParDes_Qs},
{64,    (void*)&ParDes_Pline},
{65,    (void*)&ParDes_Qline},
{68,    (void*)&ParDes_ReadyRunFault},
{69,    (void*)&ParDes_ConvState},
{70,    (void*)&ParDes_InvCtrlState},
{73,    (void*)&ParDes_VLine1},
{74,    (void*)&ParDes_VLine2},
{75,    (void*)&ParDes_VLine3},
{76,    (void*)&ParDes_Pinv},
{77,    (void*)&ParDes_Qinv},
{78,    (void*)&ParDes_Vinv},
{80,    (void*)&ParDes_Tigbt1},
{81,    (void*)&ParDes_Tigbt2},
{82,    (void*)&ParDes_Tigbt3},
{83,    (void*)&ParDes_Tigbt4},
{88,    (void*)&ParDes_OptimeHours},
{89,    (void*)&ParDes_OptimeSeconds},

//------Fehlerparameter--------------------------------------------41
{90,    (void*)&ParDes_Faults},
{91,    (void*)&ParDes_StateFaultCode},
{92,    (void*)&ParDes_SW_FaultCode},
{93,    (void*)&ParDes_HW_FaultCode},
//{94,  (void*)&ParDes_Fieldbus_FaultCode},
//{95,  (void*)&ParDes_LSI_IdentFault},
{94,    (void*)&ParDes_Parameter_FaultCode},
{95,    (void*)&ParDes_DigIn_FaultCode},
{96,    (void*)&ParDes_AutoresetTime},
{97,    (void*)&ParDes_AutoresetMask},
{98,    (void*)&ParDes_AutoresetMaxInterval},
{99,    (void*)&ParDes_AutoresetMaxNumber},

//------Steuerparameter-------------------------------------------- 47
{100,   (void*)&ParDes_Controls},
{101,   (void*)&ParDes_CtrlSrc},
{102,   (void*)&ParDes_ForceDOs},
{103,   (void*)&ParDes_PrechCtrlIn},
{104,   (void*)&ParDes_FanCtrlIn},
{105,   (void*)&ParDes_MainsCtrlIn},
{106,   (void*)&ParDes_FaultRelIn},
{107,   (void*)&ParDes_Res1In},
{108,   (void*)&ParDes_Res2In},
{109,   (void*)&ParDes_HeaterIn},
{110,   (void*)&ParDes_ValveIn},
{111,   (void*)&ParDes_DO9_CtrlIn},
{112,   (void*)&ParDes_FaultResetIn},
{113,   (void*)&ParDes_StartCBIn},
{114,   (void*)&ParDes_StartDCIn},
{115,   (void*)&ParDes_StartStopIn},
{116,   (void*)&ParDes_PWMenableIn},
{119,   (void*)&ParDes_OptimeWrite},

//------Overload--------------------------------------------    47
{120,   (void*)&ParDes_Overload},
{121,   (void*)&ParDes_F_ov},
{122,   (void*)&ParDes_Toverload},
{123,   (void*)&ParDes_Tpause},
{124,   (void*)&ParDes_Itrip},
{125,   (void*)&ParDes_VP_overload},
{126,   (void*)&ParDes_VI_overload},
{127,   (void*)&ParDes_Tigbt_max},
{128,   (void*)&ParDes_ItripAsym},

//------ Testvariablen --------------------------------------------71
{130,   (void*)&ParDes_Testvars},
{131,   (void*)&ParDes_i16_Test1},
{132,   (void*)&ParDes_i16_Test2},
{133,   (void*)&ParDes_i16_Test3},
{134,   (void*)&ParDes_i16_Test4},
{135,   (void*)&ParDes_i16_Test5},
{136,   (void*)&ParDes_i16_Test6},
{137,   (void*)&ParDes_i16_Test7},
{138,   (void*)&ParDes_i16_Test8},
{139,   (void*)&ParDes_i16_Test9},
{140,   (void*)&ParDes_i16_Test10},
{141,   (void*)&ParDes_i16_Test11},
{142,   (void*)&ParDes_i16_Test12},
{143,   (void*)&ParDes_i16_Test13},
{144,   (void*)&ParDes_TestRef1},
{145,   (void*)&ParDes_TestRef2},
{146,   (void*)&ParDes_Testmode},
{147,   (void*)&ParDes_FrequRef},

//------LVRT-HVRT------------------------------------25
{150,   (void*)&ParDes_LVRT},
{151,   (void*)&ParDes_kQ_FRT},
{152,   (void*)&ParDes_ut_FRT},
{153,   (void*)&ParDes_vFaultStart},
{154,   (void*)&ParDes_vFaultEnd},
{155,   (void*)&ParDes_iN_FRT},
{156,   (void*)&ParDes_imax_LVRT},
{157,   (void*)&ParDes_imax_HVRT},
{158,   (void*)&ParDes_imax_asym},
{159,   (void*)&ParDes_Vlim_asym},
{160,   (void*)&ParDes_vFaultStartHV},
{161,   (void*)&ParDes_vFaultEndHV},
{162,   (void*)&ParDes_t_FRT_overtravel},
{163,   (void*)&ParDes_v_FRT_IndStart},
{164,   (void*)&ParDes_v_FRT_CapStart},
{165,   (void*)&ParDes_FRT_Vmode},
{166,   (void*)&ParDes_Tf_FRT_recover},
{171,   (void*)&ParDes_tLV1},
{172,   (void*)&ParDes_tLV2},
{173,   (void*)&ParDes_tLV3},
{174,   (void*)&ParDes_tLV4},
{175,   (void*)&ParDes_tLV5},
{176,   (void*)&ParDes_tLV6},
{181,   (void*)&ParDes_vLV1},
{182,   (void*)&ParDes_vLV2},
{183,   (void*)&ParDes_vLV3},
{184,   (void*)&ParDes_vLV4},
{185,   (void*)&ParDes_vLV5},
{186,   (void*)&ParDes_vLV6},
{191,   (void*)&ParDes_tHV1},
{192,   (void*)&ParDes_tHV2},
{193,   (void*)&ParDes_tHV3},
{194,   (void*)&ParDes_tHV4},
{195,   (void*)&ParDes_tHV5},
{196,   (void*)&ParDes_tHV6},
{201,   (void*)&ParDes_vHV1},
{202,   (void*)&ParDes_vHV2},
{203,   (void*)&ParDes_vHV3},
{204,   (void*)&ParDes_vHV4},
{205,   (void*)&ParDes_vHV5},
{206,   (void*)&ParDes_vHV6},

//------ Q(V) --------------------------------------------71
{220,   (void*)&ParDes_Q_V},
{221,   (void*)&ParDes_Tf_Q_V},
{222,   (void*)&ParDes_Q_V_v0},
{225,   (void*)&ParDes_Q_V_v1},
{226,   (void*)&ParDes_Q_V_v2},
{227,   (void*)&ParDes_Q_V_v3},
{228,   (void*)&ParDes_Q_V_v4},
{229,   (void*)&ParDes_Q_V_v5},
{230,   (void*)&ParDes_Q_V_v6},
{231,   (void*)&ParDes_Q_V_q1},
{232,   (void*)&ParDes_Q_V_q2},
{233,   (void*)&ParDes_Q_V_q3},
{234,   (void*)&ParDes_Q_V_q4},
{235,   (void*)&ParDes_Q_V_q5},
{236,   (void*)&ParDes_Q_V_q6},

//------ P control / reference --------------------------------------------71
{240,   (void*)&ParDes_VoltageControl},
{241,   (void*)&ParDes_VoltRefRamp},
{242,   (void*)&ParDes_VoltRef},
{244,   (void*)&ParDes_RampTime},
{245,   (void*)&ParDes_ActivateRef},
{246,   (void*)&ParDes_VP_u},
{247,   (void*)&ParDes_VI_u},
{248,   (void*)&ParDes_RampTimeFrequ},
{249,   (void*)&ParDes_VoltageMode},

//------ Tracevariablen --------------------------------------------71
{250,   (void*)&ParDes_Trace},
{251,   (void*)&ParDes_vLuv},
{252,   (void*)&ParDes_vLwv},
{253,   (void*)&ParDes_vLu},
{254,   (void*)&ParDes_vLv},
{255,   (void*)&ParDes_vLd},
{256,   (void*)&ParDes_vLq},
{257,   (void*)&ParDes_vLamp},
{258,   (void*)&ParDes_vDC},
{259,   (void*)&ParDes_ChopTempRel},
{260,   (void*)&ParDes_iInvu},
{261,   (void*)&ParDes_iInvv},
{262,   (void*)&ParDes_iInvw},
{263,   (void*)&ParDes_iInvd},
{264,   (void*)&ParDes_iInvq},
{265,   (void*)&ParDes_iInvamp},
{266,   (void*)&ParDes_iInv_al},
{267,   (void*)&ParDes_iInv_be},
{268,   (void*)&ParDes_vinvu},
{269,   (void*)&ParDes_vinvv},
{270,   (void*)&ParDes_vLw},
{271,   (void*)&ParDes_vL_prefault},
{272,   (void*)&ParDes_FRT_Iq_state},
{273,   (void*)&ParDes_iLd},
{274,   (void*)&ParDes_iLq},
{275,   (void*)&ParDes_iLamp},
{276,   (void*)&ParDes_Mref},
{277,   (void*)&ParDes_vinv_amp},
{278,   (void*)&ParDes_vinv_d},
{279,   (void*)&ParDes_vinv_q},
{280,   (void*)&ParDes_fLine},
{281,   (void*)&ParDes_thLine},
{282,   (void*)&ParDes_tIGBT},
{283,   (void*)&ParDes_iLdav},
{284,   (void*)&ParDes_iLqav},
{285,   (void*)&ParDes_iLrefnd},
{286,   (void*)&ParDes_iLrefnq},
{287,   (void*)&ParDes_isqref},
{288,   (void*)&ParDes_Vdcref},
{289,   (void*)&ParDes_iLqref},
{290,   (void*)&ParDes_idref},
{291,   (void*)&ParDes_iqref},
{292,   (void*)&ParDes_indref},
{293,   (void*)&ParDes_inqref},
{294,   (void*)&ParDes_udref},
{295,   (void*)&ParDes_uqref},
{296,   (void*)&ParDes_udIrefp},
{297,   (void*)&ParDes_uqIrefp},
{298,   (void*)&ParDes_Imax},
{299,   (void*)&ParDes_u0ref},
{300,   (void*)&ParDes_iq_FRT},
{301,   (void*)&ParDes_Qref_ramp},
{302,   (void*)&ParDes_Pref_ramp},
{303,   (void*)&ParDes_Plimit_F},
{304,   (void*)&ParDes_id_limit},
{305,   (void*)&ParDes_vLpd},
{306,   (void*)&ParDes_vLpq},
{307,   (void*)&ParDes_vLp_amp},
{308,   (void*)&ParDes_vLnd},
{309,   (void*)&ParDes_vLnq},
{310,   (void*)&ParDes_vLn_amp},
{311,   (void*)&ParDes_vLcompd},
{312,   (void*)&ParDes_vLcompq},
{313,   (void*)&ParDes_VL_min},
{314,   (void*)&ParDes_VL_max},
{315,   (void*)&ParDes_ia1},
{316,   (void*)&ParDes_ia2},
{317,   (void*)&ParDes_ia3},
{318,   (void*)&ParDes_ib1},
{319,   (void*)&ParDes_ib2},
{320,   (void*)&ParDes_ib3},
{321,   (void*)&ParDes_ic1},
{322,   (void*)&ParDes_ic2},
{323,   (void*)&ParDes_ic3},
{324,   (void*)&ParDes_iInvpd},
{325,   (void*)&ParDes_iInvpq},
{326,   (void*)&ParDes_iInvp_amp},
{327,   (void*)&ParDes_isu},
{328,   (void*)&ParDes_isv},
{329,   (void*)&ParDes_isw},
{330,   (void*)&ParDes_isd},
{331,   (void*)&ParDes_isq},
{332,   (void*)&ParDes_isamp},
{333,   (void*)&ParDes_is_al},
{334,   (void*)&ParDes_is_be},
{335,   (void*)&ParDes_ispd},
{336,   (void*)&ParDes_ispq},
{337,   (void*)&ParDes_isp_amp},
{338,   (void*)&ParDes_udIrefn},
{339,   (void*)&ParDes_uqIrefn},
{340,   (void*)&ParDes_iInvnd},
{341,   (void*)&ParDes_iInvnq},
{342,   (void*)&ParDes_iInvn_amp},
{343,   (void*)&ParDes_isnd},
{344,   (void*)&ParDes_isnq},
{345,   (void*)& ParDes_iInv_fb_pd},
{346,   (void*)& ParDes_iInv_fb_pq},
{347,   (void*)& ParDes_iInv_fb_nd},
{348,   (void*)& ParDes_iInv_fb_nq},
{349,   (void*)&ParDes_iInv0d},
{350,   (void*)&ParDes_iInv0q},
{351,   (void*)&ParDes_is0d},
{352,   (void*)&ParDes_is0q},
{353,   (void*)&ParDes_udIref0},
{354,   (void*)&ParDes_uqIref0},
{355,   (void*)&ParDes_iL0d},
{356,   (void*)&ParDes_iL0q},
{357,   (void*)&ParDes_isrefnd},
{358,   (void*)&ParDes_isrefnq},
{359,   (void*)&ParDes_isrefnamp},
{360,   (void*)&ParDes_isn_amp},


//------ Current control --------------------------------------------71
{440,   (void*)&ParDes_CurrentControl},
{441,   (void*)&ParDes_IregMode},
{442,   (void*)&ParDes_VP_i},
{443,   (void*)&ParDes_VI_i},
{444,   (void*)&ParDes_aM},
{445,   (void*)&ParDes_VIi},
{446,   (void*)&ParDes_Tf_inref},
{447,   (void*)& ParDes_Ifb_sel},
{448,   (void*)& ParDes_k_iref_pn},
{449,   (void*)& ParDes_k_iref_np},
{451,   (void*)&ParDes_F_fil},
{452,   (void*)&ParDes_D_fil},
{453,   (void*)&ParDes_K_fil},
{454,   (void*)&ParDes_VP_i0},
{455,   (void*)&ParDes_VI_i0},
{456,   (void*)&ParDes_v_PLL_freeze_on},
{457,   (void*)&ParDes_v_PLL_freeze_off},

//------PWM and Opsys------------------------------------------------------
{460,   (void*)&ParDes_PWM},
{461,   (void*)&ParDes_PWM_mode0},
//{462, (void*)&ParDes_PWM_mode1},
{463,   (void*)&ParDes_PWM_mode_i},
{464,   (void*)&ParDes_PWM_frequ},
{465,   (void*)&ParDes_PWM_Td},
//{466, (void*)&ParDes_OnePhase},
//{467, (void*)&ParDes_PWM_multiply},
//{468, (void*)&ParDes_PWM_frequ2},
//{469, (void*)&ParDes_PWM_frequ_select},
{466,   (void*)&ParDes_IdleCnt},

//------Analog Inputs------------------------------------------------------
{470,   (void*)&ParDes_AnInputs},
{471,   (void*)&ParDes_AnIn0},
{472,   (void*)&ParDes_AnIn1},
{473,   (void*)&ParDes_AnIn2},
{474,   (void*)&ParDes_AnIn3},
{475,   (void*)&ParDes_AnIn4},
{476,   (void*)&ParDes_AnIn5},
{477,   (void*)&ParDes_AnIn6},
{478,   (void*)&ParDes_AnIn7},
{479,   (void*)&ParDes_AnIn8},
{480,   (void*)&ParDes_AnIn9},
{481,   (void*)&ParDes_AnIn10},
{482,   (void*)&ParDes_AnIn11},
{483,   (void*)&ParDes_AnIn12},
{484,   (void*)&ParDes_AnIn13},
{485,   (void*)&ParDes_AnIn14},
{486,   (void*)&ParDes_AnIn15},
{487,   (void*)&ParDes_AnIn16},
{488,   (void*)&ParDes_AnIn17},
{489,   (void*)&ParDes_AnIn18},
{490,   (void*)&ParDes_AnIn19},
{491,   (void*)&ParDes_AnIn20},
{492,   (void*)&ParDes_AnIn21},

{495,   (void*)&ParDes_AnaRef1},
{496,   (void*)&ParDes_AnaRef2},
{497,   (void*)&ParDes_AnaRef3},


//------ Errorstack --------------------------------------------71
{499,   (void*)&ParDes_ErrStack},
{500,   (void*)&ParDes_ErrStackIndex},
{501,   (void*)&ParDes_ErrEvent1_Hours},
{502,   (void*)&ParDes_ErrEvent1_Secs},
{503,   (void*)&ParDes_ErrEvent1_StateFault},
{504,   (void*)&ParDes_ErrEvent1_SW_Fault},
{505,   (void*)&ParDes_ErrEvent1_HW_Fault},
{506,   (void*)&ParDes_ErrEvent1_Warnings},
{511,   (void*)&ParDes_ErrEvent2_Hours},
{512,   (void*)&ParDes_ErrEvent2_Secs},
{513,   (void*)&ParDes_ErrEvent2_StateFault},
{514,   (void*)&ParDes_ErrEvent2_SW_Fault},
{515,   (void*)&ParDes_ErrEvent2_HW_Fault},
{516,   (void*)&ParDes_ErrEvent2_Warnings},
{521,   (void*)&ParDes_ErrEvent3_Hours},
{522,   (void*)&ParDes_ErrEvent3_Secs},
{523,   (void*)&ParDes_ErrEvent3_StateFault},
{524,   (void*)&ParDes_ErrEvent3_SW_Fault},
{525,   (void*)&ParDes_ErrEvent3_HW_Fault},
{526,   (void*)&ParDes_ErrEvent3_Warnings},
{531,   (void*)&ParDes_ErrEvent4_Hours},
{532,   (void*)&ParDes_ErrEvent4_Secs},
{533,   (void*)&ParDes_ErrEvent4_StateFault},
{534,   (void*)&ParDes_ErrEvent4_SW_Fault},
{535,   (void*)&ParDes_ErrEvent4_HW_Fault},
{536,   (void*)&ParDes_ErrEvent4_Warnings},
{541,   (void*)&ParDes_ErrEvent5_Hours},
{542,   (void*)&ParDes_ErrEvent5_Secs},
{543,   (void*)&ParDes_ErrEvent5_StateFault},
{544,   (void*)&ParDes_ErrEvent5_SW_Fault},
{545,   (void*)&ParDes_ErrEvent5_HW_Fault},
{546,   (void*)&ParDes_ErrEvent5_Warnings},
{551,   (void*)&ParDes_ErrEvent6_Hours},
{552,   (void*)&ParDes_ErrEvent6_Secs},
{553,   (void*)&ParDes_ErrEvent6_StateFault},
{554,   (void*)&ParDes_ErrEvent6_SW_Fault},
{555,   (void*)&ParDes_ErrEvent6_HW_Fault},
{556,   (void*)&ParDes_ErrEvent6_Warnings},
#if 0
{561,   (void*)&ParDes_ErrEvent7_Hours},
{562,   (void*)&ParDes_ErrEvent7_Secs},
{563,   (void*)&ParDes_ErrEvent7_StateFault},
{564,   (void*)&ParDes_ErrEvent7_SW_Fault},
{565,   (void*)&ParDes_ErrEvent7_HW_Fault},
{566,   (void*)&ParDes_ErrEvent7_Warnings},
{571,   (void*)&ParDes_ErrEvent8_Hours},
{572,   (void*)&ParDes_ErrEvent8_Secs},
{573,   (void*)&ParDes_ErrEvent8_StateFault},
{574,   (void*)&ParDes_ErrEvent8_SW_Fault},
{575,   (void*)&ParDes_ErrEvent8_HW_Fault},
{576,   (void*)&ParDes_ErrEvent8_Warnings},
#endif
//------- Current offsets ----------------------------------------
{580,   (void*)&ParDes_CurrentOffsets},
{581,   (void*)&ParDes_OffsSelectIinv},
{582,   (void*)&ParDes_OffsInvActU},
{583,   (void*)&ParDes_OffsInvActV},
{584,   (void*)&ParDes_OffsInvActW},
{585,   (void*)&ParDes_OffsInvSaveU},
{586,   (void*)&ParDes_OffsInvSaveV},
{587,   (void*)&ParDes_OffsInvSaveW},
{591,   (void*)&ParDes_OffsSelectIline},
{592,   (void*)&ParDes_OffsLineActU},
{593,   (void*)&ParDes_OffsLineActV},
{594,   (void*)&ParDes_OffsLineActW},
{595,   (void*)&ParDes_OffsLineSaveU},
{596,   (void*)&ParDes_OffsLineSaveV},
{597,   (void*)&ParDes_OffsLineSaveW},

//------ Fieldbus --------------------------------------------81
{600,   (void*)&ParDes_Fieldbus},
{601,   (void*)&ParDes_FieldbusLocalRemote},
{602,   (void*)&ParDes_FieldbusStatus},
{605,   (void*)&ParDes_FieldbusCtr},
{606,   (void*)&ParDes_FieldbusVoltage},
{607,   (void*)&ParDes_FieldbusCurrent},
{608,   (void*)&ParDes_FieldbusRamp},
{609,   (void*)&ParDes_FieldbusFrequency},
{610,   (void*)&ParDes_Fieldbus_H1Order},
{611,   (void*)&ParDes_Fieldbus_H1Ampl},
{612,   (void*)&ParDes_Fieldbus_H1Phase},
{613,   (void*)&ParDes_Fieldbus_H2Order},
{614,   (void*)&ParDes_Fieldbus_H2Ampl},
{615,   (void*)&ParDes_Fieldbus_H2Phase},
{616,   (void*)&ParDes_Fieldbus_H3Order},
{617,   (void*)&ParDes_Fieldbus_H3Ampl},
{618,   (void*)&ParDes_Fieldbus_H3Phase},
{619,   (void*)&ParDes_Fieldbus_H4Order},
{620,   (void*)&ParDes_Fieldbus_H4Ampl},
{621,   (void*)&ParDes_Fieldbus_H4Phase},
{622,   (void*)&ParDes_Fieldbus_H5Order},
{623,   (void*)&ParDes_Fieldbus_H5Ampl},
{624,   (void*)&ParDes_Fieldbus_H5Phase},
{625,   (void*)&ParDes_Fieldbus_H6Order},
{626,   (void*)&ParDes_Fieldbus_H6Ampl},
{627,   (void*)&ParDes_Fieldbus_H6Phase},
{628,   (void*)&ParDes_Fieldbus_H7Order},
{629,   (void*)&ParDes_Fieldbus_H7Ampl},
{630,   (void*)&ParDes_Fieldbus_H7Phase},
{631,   (void*)&ParDes_Fieldbus_H8Order},
{632,   (void*)&ParDes_Fieldbus_H8Ampl},
{633,   (void*)&ParDes_Fieldbus_H8Phase},
{634,   (void*)&ParDes_Fieldbus_VoltRamp},
{635,   (void*)&ParDes_Fieldbus_FrequRamp},

//------ TRANSIENTEN SPEICHER --------------------------------------------- 54
{700,   (void*)&ParDes_TransMemory},
{701,   (void*)&ParDes_TSControl},
{702,   (void*)&ParDes_TSStatus},
{703,   (void*)&ParDes_TSChannelNum},       // Kanalanzahl
{704,   (void*)&ParDes_TSSamplStepWidthFactor},     // TSSampleTime
{705,   (void*)&ParDes_TSTrigLevel},            // TSP Trigger Level
{706,   (void*)&ParDes_TSTrigCondition},
{707,   (void*)&ParDes_TSDelayNum},         // Verzögerungszahl
{708,   (void*)&ParDes_TSMemSize},              // Speichergröße
{709,   (void*)&ParDes_TSBaseTime},         // Zeitbasis
{710,   (void*)&ParDes_TSChannel0Par},      // Trigger Kanal
{711,   (void*)&ParDes_TSChannel1Par},
{712,   (void*)&ParDes_TSChannel2Par},
{713,   (void*)&ParDes_TSChannel3Par},
{714,   (void*)&ParDes_TSChannel4Par},
{715,   (void*)&ParDes_TSChannel5Par},
{716,   (void*)&ParDes_TSChannel6Par},
{717,   (void*)&ParDes_TSChannel7Par},

//------ Voltage control --------------------------------------------71
{730,   (void*)&ParDes_DroopControl},
{731,   (void*)&ParDes_kom_droop},
{732,   (void*)&ParDes_kv_droop},
{733,   (void*)&ParDes_Rv_droop},
{734,   (void*)&ParDes_Lv_droop},

//------ Chopper control --------------------------------------------71
{740,   (void*)&ParDes_ChopperControl},
{741,   (void*)&ParDes_VdcChopOn},
{742,   (void*)&ParDes_VdcChopOff},
{743,   (void*)&ParDes_MaxChopOntime},

//------ DMA --------------------------------------------71
{750,   (void*)&ParDes_DMA},
{751,   (void*)&ParDes_DMA_ReadAddress},
{752,   (void*)&ParDes_DMA_ReadData},
{753,   (void*)&ParDes_DMA_WriteMode},
{754,   (void*)&ParDes_DMA_WriteAddress},
{755,   (void*)&ParDes_DMA_WriteData},
{756,   (void*)&ParDes_RAM_test_error},

//------ RTC --------------------------------------------71
{760,   (void*)&ParDes_RTC},
{761,   (void*)&ParDes_RTC_year},
{762,   (void*)&ParDes_RTC_month},
{763,   (void*)&ParDes_RTC_weekdays},
{764,   (void*)&ParDes_RTC_days},
{765,   (void*)&ParDes_RTC_hours},
{766,   (void*)&ParDes_RTC_minutes},
{767,   (void*)&ParDes_RTC_seconds},
{768,   (void*)&ParDes_RTC_time},
{770,   (void*)&ParDes_RTC_set},
{771,   (void*)&ParDes_RTC_year_set},
{772,   (void*)&ParDes_RTC_month_set},
{773,   (void*)&ParDes_RTC_weekdays_set},
{774,   (void*)&ParDes_RTC_days_set},
{775,   (void*)&ParDes_RTC_hours_set},
{776,   (void*)&ParDes_RTC_minutes_set},
{777,   (void*)&ParDes_RTC_seconds_set},

//-------Climatisation-------------------------------------71
{780,   (void*)&ParDes_Climate},
{781,   (void*)&ParDes_WaterTempMinWarn},
{782,   (void*)&ParDes_WaterTempMaxWarn},
{783,   (void*)&ParDes_WaterTempMaxFault},
{784,   (void*)&ParDes_WaterTempHeatexOn},
{785,   (void*)&ParDes_WaterTempHeatexOff},
{786,   (void*)&ParDes_TempHyst},
{787,   (void*)&ParDes_AirTempMaxWarn},
{788,   (void*)&ParDes_AirTempMaxFault},
{789,   (void*)&ParDes_FlowMinWarn},
{790,   (void*)&ParDes_FlowMinFault},
{791,   (void*)&ParDes_FlowMaxWarn},
{792,   (void*)&ParDes_FlowMaxFault},
{793,   (void*)&ParDes_FlowHyst},
{794,   (void*)&ParDes_Pressure_max_trip},
{800,   (void*)&ParDes_WaterInTemp},
{801,   (void*)&ParDes_WaterOutTemp},
{802,   (void*)&ParDes_Air1Temp},
{803,   (void*)&ParDes_Air2Temp},
{804,   (void*)&ParDes_PressureIn},
{805,   (void*)&ParDes_PressureOut},
{806,   (void*)&ParDes_CoolingFlow},
{807,   (void*)&ParDes_ClimateFault},
{808,   (void*)&ParDes_ClimateWarn},

//-------HW Faults-------------------------------------71
{810,   (void*)&ParDes_HW_fault},
{811,   (void*)&ParDes_gdi_det},
{812,   (void*)&ParDes_gdi_err},
{813,   (void*)&ParDes_gdi_ovt},
{814,   (void*)&ParDes_trp_ocn},
{815,   (void*)&ParDes_trp_ocp},
{816,   (void*)&ParDes_err_glb},
{821,   (void*)&ParDes_ms_gdi_det},
{822,   (void*)&ParDes_ms_gdi_err},
{823,   (void*)&ParDes_ms_gdi_ovt},
{824,   (void*)&ParDes_ms_trp_ocn},
{825,   (void*)&ParDes_ms_trp_ocp},
{826,   (void*)&ParDes_ms_err_glb},
{830,   (void*)&ParDes_HWM_set},
{831,   (void*)&ParDes_ms_set_gdi_det},
{832,   (void*)&ParDes_ms_set_gdi_err},
{833,   (void*)&ParDes_ms_set_gdi_ovt},
{834,   (void*)&ParDes_ms_set_trp_ocn},
{835,   (void*)&ParDes_ms_set_trp_ocp},
{836,   (void*)&ParDes_ms_set_err_glb},

//-------GD temperatures-------------------------------------71
{840,   (void*)&ParDes_GD_temps},
{841,   (void*)&ParDes_t_igbt1},
{842,   (void*)&ParDes_t_igbt2},
{843,   (void*)&ParDes_t_igbt3},
{844,   (void*)&ParDes_t_igbt4},
{845,   (void*)&ParDes_t_igbt5},
{846,   (void*)&ParDes_t_igbt6},
{847,   (void*)&ParDes_t_igbt7},
{848,   (void*)&ParDes_t_igbt8},
{849,   (void*)&ParDes_t_igbt9},
{850,   (void*)&ParDes_t_brd},
{851,   (void*)&ParDes_h_brd},

//------ Statistics --------------------------------------------71
{870,   (void*)&ParDes_Statistics},
{871,   (void*)&ParDes_ClearStatistics},
{877,   (void*)&ParDes_TimeSinceReboot_h01},
{878,   (void*)&ParDes_TotalRuntime_h01},
{879,   (void*)&ParDes_TimeInFault_h01},
//------ values are written in EEPROM in optime_count() => take care of parameter numbers !!!
{880,   (void*)&ParDes_NumberFaults},
//------ values are written in EEPROM in optime_count() => take care of parameter numbers !!!
{890,   (void*)&ParDes_TotalRuntime_inc},
{891,   (void*)&ParDes_TimeInFault_inc},
{899,   (void*)&ParDes_SaveStatistics},


//------ Events --------------------------------------------71
{900,   (void*)&ParDes_Events},
{901,   (void*)&ParDes_Event1},
{902,   (void*)&ParDes_Event2},
{903,   (void*)&ParDes_Event3},
{904,   (void*)&ParDes_Event4},
{905,   (void*)&ParDes_Event5},
{906,   (void*)&ParDes_Event6},
{907,   (void*)&ParDes_Event7},
{908,   (void*)&ParDes_Event8},
{909,   (void*)&ParDes_Event9},
{910,   (void*)&ParDes_Event10},
{911,   (void*)&ParDes_Event11},
{912,   (void*)&ParDes_Event12},
{913,   (void*)&ParDes_Event13},
{914,   (void*)&ParDes_Event14},
{915,   (void*)&ParDes_Event15},
{916,   (void*)&ParDes_Event16},
{917,   (void*)&ParDes_Event17},
{918,   (void*)&ParDes_Event18},
{919,   (void*)&ParDes_Event19},
{920,   (void*)&ParDes_Event20},
{921,   (void*)&ParDes_Event21},
{922,   (void*)&ParDes_Event22},
{923,   (void*)&ParDes_Event23},
{924,   (void*)&ParDes_Event24},
{925,   (void*)&ParDes_Event25},
{926,   (void*)&ParDes_Event26},
{927,   (void*)&ParDes_Event27},
{928,   (void*)&ParDes_Event28},
{929,   (void*)&ParDes_Event29},
{930,   (void*)&ParDes_Event30},
{931,   (void*)&ParDes_Event31},
{932,   (void*)&ParDes_Event32},
{933,   (void*)&ParDes_Event33},
{934,   (void*)&ParDes_Event34},
{935,   (void*)&ParDes_Event35},
{936,   (void*)&ParDes_Event36},
{937,   (void*)&ParDes_Event37},
{938,   (void*)&ParDes_Event38},
{939,   (void*)&ParDes_Event39},
{940,   (void*)&ParDes_Event40},

};


UINT16 GetNumberOfParams(void)
{
    return sizeof(ParTable)/sizeof(PAR_TAB_ENTRY_T);
}


#else

volatile const MAIN_sParameterList_t asParameterList[BOARDCFG_CSMON_FILE_PARAMETER_COUNT_MAX] =
{

#if 0



 /*                ID         Attributes      Bits, Offs, uAnyType32_t         MCU Address                          Name \t Unit                   Max        Min         Def      Norm */
  INIT_PARAMETER(60000, PAR(_UINT16,_RW,_NO),   16,    0, u16Register, &u16PeriodControl_usec,                  "CtrlLoopPeriod\tusec",         0x0000FFFF,    0,          0,      1.0F),
  INIT_PARAMETER(60001, PAR(_UINT16,_RW,_NO),   16,    0, u16Register, &u16DelayCtrlLoop_100nsec,               "CtrlLoopAddDelay\tusec",       0x0000FFFF,    0,          0,      0.1F),
  INIT_PARAMETER(60002, PAR(_UINT16,_RW,_NO),   16,    0, u16Register, &u16DelayMainLoop_usec,                  "MainLoopAddDelay\tusec",       0x0000FFFF,    0,          0,      1.0F),
  INIT_PARAMETER(60003, PAR(_UINT16,_RW,_NO),   16,    0, u16Register, &u16CountSetParameterFail,               "Parameters_Fail\tunit",        0x0000FFFF,    0,          0,      1.0F),
  INIT_PARAMETER(60004, PAR(_UINT32,_RO,_NO),   32,    0, u32Register, &u32TimeMainLoopProcessCSMON_Max_Ticks,  "MainLoopCSMON\tusec",          0xFFFFFFFF,    0,          0,      1.0F),
  INIT_PARAMETER(60005, PAR(_UINT32,_RO,_NO),   32,    0, u32Register, &u32TimeCSMON_ISR_Max_Ticks,             "CtrlLoopCSMON\tusec",          0xFFFFFFFF,    0,          0,      1.0F),
  INIT_PARAMETER(60006, PAR(_UINT32,_RO,_NO),   32,    0, u32Register, &u32TimeMainLoopCycle_Max_Ticks,         "MainLoopMeasMax\tusec",        0xFFFFFFFF,    0,          0,      1.0F),
  INIT_PARAMETER(60007, PAR(_UINT32,_RO,_NO),   32,    0, u32Register, &u32TimeCtrlLoopMax_Ticks,               "CtrlLoopMeasMax\tusec",        0xFFFFFFFF,    0,          0,      1.0F),
  INIT_PARAMETER(60008, PAR(_UINT32,_RO,_NO),   32,    0, u32Register, &u32ParamTime_Ticks,                     "ParamInitMeas\tusec",          0xFFFFFFFF,    0,          0,      1.0F),
  INIT_PARAMETER(60009, PAR(_UINT08,_RW,_NO),    8,    0, u8Register,  &bResetAllTimeMeasures,                  "MeasuresReset\tunit",          true,          false,      false,  1.0F),

  INIT_PARAMETER(65534, PAR(_UINT16,_RW,_NO),   16,    0, u16Register, &u16DummyDataCnt,                        "ModbusMsgCntr\tunit",          0x0000FFFF,    0,          0,      0.0F),
  INIT_PARAMETER(    0, PAR(_UINT08,_WO,_WR),    8,    0, u8Register,  &bDummyReqstDevRunning,                  "DeviceRunning\tboolean",       true,          false,      false,  1.0F),    /* Parameter ID 0 - Wr Addr */
  INIT_PARAMETER(    0, PAR(_UINT08,_RO,_NO),    8,    0, u8Register,  &bDummyStatsDevRunning,                  "DeviceRunning\tboolean",       true,          false,      false,  1.0F),    /* Parameter ID 0 - Rd Addr */
  INIT_PARAMETER(    1, PAR(_SINT16,_RW,_NO),   16,    0, s16Register, &s16ScaleDataTst,                        "ReadWriteScale\t*3.1415",      10000,         -10000,     0,      3.1415F),
  INIT_PARAMETER(    2, PAR(_SINT16,_RW,_WR),   16,    0, s16Register, &s16DummyIncrementLoop,                  "IncLoopTest\tA(0.5V)",         10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    3, PAR(_SINT16,_RW,_WR),   16,    0, s16Register, &s16DummyCurrentPhaseA,                  "CurrentPhA\tA",                10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    4, PAR(_SINT16,_RW,_WR),   16,    0, s16Register, &s16DummyCurrentPhaseB,                  "CurrentPhB\tA",                10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    5, PAR(_SINT16,_RW,_WR),   16,    0, s16Register, &s16DummyCurrentPhaseC,                  "CurrentPhC\tA",                10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    6, PAR(_UINT32,_RW,_WR),   32,    0, u32Register, &u32GetBaudError_PPM,                    "BaudError\t%",                 10000,         0,          0,      0.0001F),
  INIT_PARAMETER(    7, PAR(_UINT16,_RW,_WR),   16,    0, u16Register, &u16WatchdogPrescalerTime,               "WatchdogTime\tmsec",           64,            1,          0,      ((1000.0*512*256)/10000000.0)),  /* 1msec; 512WatchdogDiv; 256WatchdogCnt; 10MHz INTOSC */

  INIT_PARAMETER(    8, PAR(_SINT16,_RW,_WR),   16,    0, s16Register, &s16DummyVoltageDCLink,                  "VoltageBus\tV",                10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    9, PAR(_SINT16,_RW,_WR),   16,    0, s16Register, &s16DummyCurrentPhaseA,                  "CurrentPhA\tA",                10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(   10, PAR(_SINT16,_RW,_WR),   16,    0, s16Register, &s16DummyCurrentPhaseB,                  "CurrentPhB\tA",                10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(   11, PAR(_SINT16,_RW,_WR),   16,    0, s16Register, &s16DummyCurrentPhaseC,                  "CurrentPhC\tA",                10000,         -10000,     0,      1.0F),

  INIT_PARAMETER(   28, PAR(_SINT32,_RW,_WR),   32,    0, s32Register, &s32DummyVoltageDCLink,                  "VoltageBus32\tV",              1000000000,    -1000000000,0,      0.000001F),
  INIT_PARAMETER(   29, PAR(_SINT32,_RW,_WR),   32,    0, s32Register, &s32DummyCurrentPhaseA,                  "CurrentPhA32\tA",              1000000000,    -1000000000,0,      0.000001F),
  INIT_PARAMETER(   30, PAR(_SINT32,_RW,_WR),   32,    0, s32Register, &s32DummyCurrentPhaseB,                  "CurrentPhB32\tA",              1000000000,    -1000000000,0,      0.000001F),
  INIT_PARAMETER(   31, PAR(_SINT32,_RW,_WR),   32,    0, s32Register, &s32DummyCurrentPhaseC,                  "CurrentPhC32\tA",              1000000000,    -1000000000,0,      0.000001F),

  INIT_PARAMETER(  108, PAR(_SINT16,_RW,_WR),   16,    0, s16Register, &s16DummyVoltageDCLinkStartup,           "VoltageBusStart\tV",           10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  109, PAR(_SINT16,_RW,_WR),   16,    0, s16Register, &s16DummyCurrentPhaseAStartup,           "CurrentPhAStart\tA",           10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  110, PAR(_SINT16,_RW,_WR),   16,    0, s16Register, &s16DummyCurrentPhaseBStartup,           "CurrentPhBStart\tA",           10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  111, PAR(_SINT16,_RW,_WR),   16,    0, s16Register, &s16DummyCurrentPhaseCStartup,           "CurrentPhCStart\tA",           10000,         -10000,     0,      1.0F),

  INIT_PARAMETER(  208, PAR(_SINT16,_RW,_WR),   16,    0, s16Register, &s16DummyVoltageDCLinkStop,              "VoltageBusStop\tV",            10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  209, PAR(_SINT16,_RW,_WR),   16,    0, s16Register, &s16DummyCurrentPhaseAStop,              "CurrentPhAStop\tA",            10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  210, PAR(_SINT16,_RW,_WR),   16,    0, s16Register, &s16DummyCurrentPhaseBStop,              "CurrentPhBStop\tA",            10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  211, PAR(_SINT16,_RW,_WR),   16,    0, s16Register, &s16DummyCurrentPhaseCStop,              "CurrentPhCStop\tA",            10000,         -10000,     0,      1.0F),

  INIT_PARAMETER(  308, PAR(_SINT16,_RW,_WR),   16,    0, s16Register, &s16DummyVoltageDCLinkIdle,              "VoltageBusIdle\tV",            10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  309, PAR(_SINT16,_RW,_WR),   16,    0, s16Register, &s16DummyCurrentPhaseAIdle,              "CurrentPhAIdle\tA",            10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  310, PAR(_SINT16,_RW,_WR),   16,    0, s16Register, &s16DummyCurrentPhaseBIdle,              "CurrentPhBIdle\tA",            10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  311, PAR(_SINT16,_RW,_WR),   16,    0, s16Register, &s16DummyCurrentPhaseCIdle,              "CurrentPhCIdle\tA",            10000,         -10000,     0,      1.0F),

  INIT_PARAMETER(  408, PAR(_SINT16,_RW,_WR),   16,    0, s16Register, &s16DummyIncrementLoopA,                 "IncLoopTestA\tA",              10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  409, PAR(_SINT16,_RW,_WR),   16,    0, s16Register, &s16DummyIncrementLoopB,                 "IncLoopTestB\tA",              10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  410, PAR(_SINT16,_RW,_WR),   16,    0, s16Register, &s16DummyIncrementLoopC,                 "IncLoopTestC\tA",              10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  411, PAR(_SINT16,_RW,_WR),   16,    0, s16Register, &s16DummyIncrementLoopV,                 "IncLoopTestV\tV",              10000,         -10000,     0,      1.0F),

  INIT_PARAMETER(  412, PAR(_SINT16,_RW,_WR),   16,    0, s16Register, &s16DummyIncrementLoopCDiff,             "IncLoopTestCDiff\tA",          10000,         -10000,     0,      1.0F),




#else /* #if _CSMON_USE_EXTERNAL_PARAMETER_LIST */



/* u16ParameterIndexID;                 u32RealAddress;           u16ParamAttributes;     pu8Name;            pu8Unit;            u32Max;                 u32Min;              u32Def;             Norm; */
/*                                                                   Type Access Pass                                                                                                                    */


//  INIT_PARAMETER(60000,   _UINT16, _RW, _NO,     0x10,   0x03,        4,        1,         1,    16,    0, u16Register, &u16PeriodControl_usec,                  "CtrlLoopPeriod",     "usec",    0x0000FFFF,    0,          0,      1.0F),

#if CSMON_PARAMETER_LIST_TEST == CSMON_PAR_LIST_RECORDER_DEBUG

#if 1
 /*                ID         Attributes       fucnWr, funcRd, accessWr, accessRd, bStorable,  Bits, Offs, uAnyType32_t                                               MCU Address                          Name               Unit         Max        Min         Def      Norm */
  INIT_PARAMETER(60000,     _UINT16,_RW,_NO,     0x10,   0x03,        4,        1,         1,    16,    0, u16Register, CSMON_DATA_TYPE_U16,  &u16PeriodControl_usec,                  "CtrlLoopPeriod",     "usec",    0x0000FFFF,    0,          0,      1.0F),
  INIT_PARAMETER(60001,     _UINT16,_RW,_NO,     0x10,   0x03,        4,        1,         1,    16,    0, u16Register, CSMON_DATA_TYPE_U16,  &u16DelayCtrlLoop_100nsec,               "CtrlLoopAddDelay",   "usec",    0x0000FFFF,    0,          0,      0.1F),
  INIT_PARAMETER(60002,     _UINT16,_RW,_NO,     0x10,   0x03,        4,        1,         1,    16,    0, u16Register, CSMON_DATA_TYPE_U16,  &u16DelayMainLoop_usec,                  "MainLoopAddDelay",   "usec",    0x0000FFFF,    0,          0,      1.0F),
  INIT_PARAMETER(60003,     _UINT16,_RW,_NO,     0x10,   0x03,        4,        1,         1,    16,    0, u16Register, CSMON_DATA_TYPE_U16,  &u16CountSetParameterFail,               "Parameters_Fail",    "unit",    0x0000FFFF,    0,          0,      1.0F),
  INIT_PARAMETER(60004,     _UINT32,_RO,_NO,     0x10,   0x03,        4,        1,         1,    32,    0, u32Register, CSMON_DATA_TYPE_U32,  &u32TimeMainLoopProcessCSMON_Max_Ticks,  "MainLoopCSMON",      "usec",    0xFFFFFFFF,    0,          0,      1.0F),
  INIT_PARAMETER(60005,     _UINT32,_RO,_NO,     0x10,   0x03,        4,        1,         1,    32,    0, u32Register, CSMON_DATA_TYPE_U32,  &u32TimeCSMON_ISR_Max_Ticks,             "CtrlLoopCSMON",      "usec",    0xFFFFFFFF,    0,          0,      1.0F),
  INIT_PARAMETER(60006,     _UINT32,_RO,_NO,     0x10,   0x03,        4,        1,         1,    32,    0, u32Register, CSMON_DATA_TYPE_U32,  &u32TimeMainLoopCycle_Max_Ticks,         "MainLoopMeasMax",    "usec",    0xFFFFFFFF,    0,          0,      1.0F),
  INIT_PARAMETER(60007,     _UINT32,_RO,_NO,     0x10,   0x03,        4,        1,         1,    32,    0, u32Register, CSMON_DATA_TYPE_U32,  &u32TimeCtrlLoopMax_Ticks,               "CtrlLoopMeasMax",    "usec",    0xFFFFFFFF,    0,          0,      1.0F),
  INIT_PARAMETER(60008,     _UINT32,_RO,_NO,     0x10,   0x03,        4,        1,         1,    32,    0, u32Register, CSMON_DATA_TYPE_U32,  &u32ParamTime_Ticks,                     "ParamInitMeas",      "usec",    0xFFFFFFFF,    0,          0,      1.0F),
  INIT_PARAMETER(60009,     _UINT08,_RW,_NO,     0x10,   0x03,        4,        1,         1,     8,    0, u8Register,  CSMON_DATA_TYPE_U8,   &bResetAllTimeMeasures,                  "MeasuresReset",      "unit",    true,          false,      false,  1.0F),

  INIT_PARAMETER(65534,     _UINT16,_RW,_NO,     0x10,   0x03,        4,        1,         1,    16,    0, u16Register, CSMON_DATA_TYPE_U16,  &u16DummyDataCnt,                        "ModbusMsgCntr",      "unit",    0x0000FFFF,    0,          0,      0.0F),
  INIT_PARAMETER(    0,     _UINT08,_WO,_WR,     0x10,   0x03,        4,        1,         1,     8,    0, u8Register,  CSMON_DATA_TYPE_U8 ,  &bDummyReqstDevRunning,                  "DeviceRunning",      "boolean", true,          false,      false,  1.0F),    /* Parameter ID 0 - Wr Addr */
  INIT_PARAMETER(    0,     _UINT08,_RO,_NO,     0x10,   0x03,        4,        1,         1,     8,    0, u8Register,  CSMON_DATA_TYPE_U8 ,  &bDummyStatsDevRunning,                  "DeviceRunning",      "boolean", true,          false,      false,  1.0F),    /* Parameter ID 0 - Rd Addr */
//  INIT_PARAMETER(    0,     _UINT08,_WO,_WR,     0x10,   0x00,        4,        1,         1,     8,    0, u8Register,  CSMON_DATA_TYPE_U8 ,  &bDummyReqstDevRunning,                  "DeviceRunning",      "boolean", true,          false,      false,  1.0F),    /* Parameter ID 0 - Wr Addr */
//  INIT_PARAMETER(    0,     _UINT08,_RO,_NO,     0x00,   0x03,        4,        1,         1,     8,    0, u8Register,  CSMON_DATA_TYPE_U8 ,  &bDummyStatsDevRunning,                  "DeviceRunning",      "boolean", true,          false,      false,  1.0F),    /* Parameter ID 0 - Rd Addr */
  INIT_PARAMETER(    1,     _SINT16,_RW,_NO,     0x10,   0x03,        4,        1,         1,    16,    0, s16Register, CSMON_DATA_TYPE_S16,  &s16ScaleDataTst,                        "ReadWriteScale",     "*3.1415", 10000,         -10000,     0,      3.1415F),
  INIT_PARAMETER(    2,     _SINT16,_RW,_WR,     0x10,   0x03,        4,        1,         1,    16,    0, s16Register, CSMON_DATA_TYPE_S16,  &s16DummyIncrementLoop,                  "IncLoopTest",        "A(0.5V)", 10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    3,     _SINT16,_RW,_WR,     0x10,   0x03,        4,        1,         1,    16,    0, s16Register, CSMON_DATA_TYPE_S16,  &s16DummyCurrentPhaseA,                  "CurrentPhA",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    4,     _SINT16,_RW,_WR,     0x10,   0x03,        4,        1,         1,    16,    0, s16Register, CSMON_DATA_TYPE_S16,  &s16DummyCurrentPhaseB,                  "CurrentPhB",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    5,     _SINT16,_RW,_WR,     0x10,   0x03,        4,        1,         1,    16,    0, s16Register, CSMON_DATA_TYPE_S16,  &s16DummyCurrentPhaseC,                  "CurrentPhC",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    6,     _UINT32,_RW,_WR,     0x10,   0x03,        4,        1,         1,    32,    0, u32Register, CSMON_DATA_TYPE_U32,  &u32GetBaudError_PPM,                    "BaudError",          "%",       10000,         0,          0,      0.0001F),
  INIT_PARAMETER(    7,     _UINT16,_RW,_WR,     0x10,   0x03,        4,        1,         1,    16,    0, u16Register, CSMON_DATA_TYPE_U16,  &u16WatchdogPrescalerTime,               "WatchdogTime",       "msec",    64,            1,          0,      ((1000.0*512*256)/10000000.0)),  /* 1msec; 512WatchdogDiv; 256WatchdogCnt; 10MHz INTOSC */

  INIT_PARAMETER(    8,     _SINT16,_RW,_WR,     0x10,   0x03,        4,        1,         1,    16,    0, s16Register, CSMON_DATA_TYPE_S16,  &s16DummyVoltageDCLink,                  "VoltageBus",         "V",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    9,     _SINT16,_RW,_WR,     0x10,   0x03,        4,        1,         1,    16,    0, s16Register, CSMON_DATA_TYPE_S16,  &s16DummyCurrentPhaseA,                  "CurrentPhA",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(   10,     _SINT16,_RW,_WR,     0x10,   0x03,        4,        1,         1,    16,    0, s16Register, CSMON_DATA_TYPE_S16,  &s16DummyCurrentPhaseB,                  "CurrentPhB",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(   11,     _SINT16,_RW,_WR,     0x10,   0x03,        4,        1,         1,    16,    0, s16Register, CSMON_DATA_TYPE_S16,  &s16DummyCurrentPhaseC,                  "CurrentPhC",         "A",       10000,         -10000,     0,      1.0F),

  INIT_PARAMETER(   28,     _SINT32,_RW,_WR,     0x10,   0x03,        4,        1,         1,    32,    0, s32Register, CSMON_DATA_TYPE_S32,  &s32DummyVoltageDCLink,                  "VoltageBus32",       "V",       1000000000,    -1000000000,0,      0.000001F),
  INIT_PARAMETER(   29,     _SINT32,_RW,_WR,     0x10,   0x03,        4,        1,         1,    32,    0, s32Register, CSMON_DATA_TYPE_S32,  &s32DummyCurrentPhaseA,                  "CurrentPhA32",       "A",       1000000000,    -1000000000,0,      0.000001F),
  INIT_PARAMETER(   30,     _SINT32,_RW,_WR,     0x10,   0x03,        4,        1,         1,    32,    0, s32Register, CSMON_DATA_TYPE_S32,  &s32DummyCurrentPhaseB,                  "CurrentPhB32",       "A",       1000000000,    -1000000000,0,      0.000001F),
  INIT_PARAMETER(   31,     _SINT32,_RW,_WR,     0x10,   0x03,        4,        1,         1,    32,    0, s32Register, CSMON_DATA_TYPE_S32,  &s32DummyCurrentPhaseC,                  "CurrentPhC32",       "A",       1000000000,    -1000000000,0,      0.000001F),

  INIT_PARAMETER(  108,     _SINT16,_RW,_WR,     0x10,   0x03,        4,        1,         1,    16,    0, s16Register, CSMON_DATA_TYPE_S16,  &s16DummyVoltageDCLinkStartup,           "VoltageBusStart",    "V",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  109,     _SINT16,_RW,_WR,     0x10,   0x03,        4,        1,         1,    16,    0, s16Register, CSMON_DATA_TYPE_S16,  &s16DummyCurrentPhaseAStartup,           "CurrentPhAStart",    "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  110,     _SINT16,_RW,_WR,     0x10,   0x03,        4,        1,         1,    16,    0, s16Register, CSMON_DATA_TYPE_S16,  &s16DummyCurrentPhaseBStartup,           "CurrentPhBStart",    "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  111,     _SINT16,_RW,_WR,     0x10,   0x03,        4,        1,         1,    16,    0, s16Register, CSMON_DATA_TYPE_S16,  &s16DummyCurrentPhaseCStartup,           "CurrentPhCStart",    "A",       10000,         -10000,     0,      1.0F),

  INIT_PARAMETER(  208,     _SINT16,_RW,_WR,     0x10,   0x03,        4,        1,         1,    16,    0, s16Register, CSMON_DATA_TYPE_S16,  &s16DummyVoltageDCLinkStop,              "VoltageBusStop",     "V",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  209,     _SINT16,_RW,_WR,     0x10,   0x03,        4,        1,         1,    16,    0, s16Register, CSMON_DATA_TYPE_S16,  &s16DummyCurrentPhaseAStop,              "CurrentPhAStop",     "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  210,     _SINT16,_RW,_WR,     0x10,   0x03,        4,        1,         1,    16,    0, s16Register, CSMON_DATA_TYPE_S16,  &s16DummyCurrentPhaseBStop,              "CurrentPhBStop",     "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  211,     _SINT16,_RW,_WR,     0x10,   0x03,        4,        1,         1,    16,    0, s16Register, CSMON_DATA_TYPE_S16,  &s16DummyCurrentPhaseCStop,              "CurrentPhCStop",     "A",       10000,         -10000,     0,      1.0F),

  INIT_PARAMETER(  308,     _SINT16,_RW,_WR,     0x10,   0x03,        4,        1,         1,    16,    0, s16Register, CSMON_DATA_TYPE_S16,  &s16DummyVoltageDCLinkIdle,              "VoltageBusIdle",     "V",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  309,     _SINT16,_RW,_WR,     0x10,   0x03,        4,        1,         1,    16,    0, s16Register, CSMON_DATA_TYPE_S16,  &s16DummyCurrentPhaseAIdle,              "CurrentPhAIdle",     "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  310,     _SINT16,_RW,_WR,     0x10,   0x03,        4,        1,         1,    16,    0, s16Register, CSMON_DATA_TYPE_S16,  &s16DummyCurrentPhaseBIdle,              "CurrentPhBIdle",     "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  311,     _SINT16,_RW,_WR,     0x10,   0x03,        4,        1,         1,    16,    0, s16Register, CSMON_DATA_TYPE_S16,  &s16DummyCurrentPhaseCIdle,              "CurrentPhCIdle",     "A",       10000,         -10000,     0,      1.0F),

  INIT_PARAMETER(  408,     _SINT16,_RW,_WR,     0x10,   0x03,        4,        1,         1,    16,    0, s16Register, CSMON_DATA_TYPE_S16,  &s16DummyIncrementLoopA,                 "IncLoopTestA",       "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  409,     _SINT16,_RW,_WR,     0x10,   0x03,        4,        1,         1,    16,    0, s16Register, CSMON_DATA_TYPE_S16,  &s16DummyIncrementLoopB,                 "IncLoopTestB",       "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  410,     _SINT16,_RW,_WR,     0x10,   0x03,        4,        1,         1,    16,    0, s16Register, CSMON_DATA_TYPE_S16,  &s16DummyIncrementLoopC,                 "IncLoopTestC",       "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  411,     _SINT16,_RW,_WR,     0x10,   0x03,        4,        1,         1,    16,    0, s16Register, CSMON_DATA_TYPE_S16,  &s16DummyIncrementLoopV,                 "IncLoopTestV",       "V",       10000,         -10000,     0,      1.0F),

  INIT_PARAMETER(  412,     _SINT16,_RW,_WR,     0x10,   0x03,        4,        1,         1,    16,    0, s16Register, CSMON_DATA_TYPE_S16,  &s16DummyIncrementLoopCDiff,             "IncLoopTestCDiff",   "A",       10000,         -10000,     0,      1.0F),
#endif


#elif CSMON_PARAMETER_LIST_TEST == CSMON_PAR_LIST_MINIMUM_COUNT

 /*                ID         Attributes      uAnyType32_t         MCU Address                          Name               Unit         Max        Min         Def      Norm */
 INIT_PARAMETER(65534, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "ModbusMsgCntr",      "unit",    0x0000FFFF,    0,          0,      0.0F),
 INIT_PARAMETER(    8, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyVoltageDCLink,                  "VoltageBus",         "V",       10000,         -10000,     0,      1.0F),
 INIT_PARAMETER(    9, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseA,                  "CurrentPhA",         "A",       10000,         -10000,     0,      1.0F),
 INIT_PARAMETER(   10, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseB,                  "CurrentPhB",         "A",       10000,         -10000,     0,      1.0F),
 INIT_PARAMETER(   11, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseC,                  "CurrentPhC",         "A",       10000,         -10000,     0,      1.0F),
 INIT_PARAMETER(    0, PAR(_UINT08,_WO,_WR), u8Register,  &bDummyReqstDevRunning,                  "DeviceRunning",      "boolean", true,          false,      false,  1.0F),    /* Parameter ID 0 - Wr Addr */
 INIT_PARAMETER(    0, PAR(_UINT08,_RO,_NO), u8Register,  &bDummyStatsDevRunning,                  "DeviceRunning",      "boolean", true,          false,      false,  1.0F),    /* Parameter ID 0 - Rd Addr */

 INIT_PARAMFULL(60093, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "93",                 "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 32, 0,CSMON_VISUAL_TYPE_DEF),
 //INIT_PARAMFULL(   93, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "93",                 "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 0, 32,CSMON_VISUAL_TYPE_DEF),
 //INIT_PARAMFULL(300,   PAR(_A_UINT08,_RW,_NO), u8Register, au8TestCharArray32,                     "au8TestCharArray32", "str",     0xFF,          0,          0,      0.0, 32,0,CSMON_VISUAL_TYPE_DEF),
 //INIT_PARAMFULL(301,   PAR(_A_UINT08,_RW,_NO), u8Register, au8TestCharArray16,                     "au8TestCharArray16", "str",     0xFF,          0,          0,      0.0, 16,0,CSMON_VISUAL_TYPE_DEF),
 //INIT_PARAMFULL(302,   PAR(_A_UINT08,_RW,_NO), u8Register, au8TestCharArray8,                      "au8TestCharArray8",  "str",     0xFF,          0,          0,      0.0,  8,0,CSMON_VISUAL_TYPE_DEF),

 INIT_PARAMETER(60001, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "u32DummyDataCnt 0123456789ACDEF0123456789ABCDEF",      "unituntunitunit",    0xFFFFFFFF,    0,          0,      0.0),
 INIT_PARAMFULL(60011, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "u1Start  0",         "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 1, 0,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60012, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "u1Start  1",         "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 1, 1,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60013, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "u1Start  8",         "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 1, 8,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60014, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "u1Start 18",         "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 1,18,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60041, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "u4Start22",          "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 4,22,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60042, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "u3Start2",           "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 3, 2,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60043, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "u8Start18",          "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 8,18,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60044, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "u2Start4",           "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 2, 4,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60071, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "u3Start0",           "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 3, 0,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60072, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "u5Start3",           "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 5, 3,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60073, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "u6Start1",           "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 6, 1,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60074, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "u9Start15",          "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 9,15,CSMON_VISUAL_TYPE_BIN),

 INIT_PARAMETER(60000, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "u16DummyDataCnt 0123456789ACDEF0123456789ABCDEF",      "unituntunitunit",    0x0000FFFF,    0,          0,      0.0),
 INIT_PARAMFULL(60021, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "u1Start  0",         "16BIN",   0x0000FFFF,    0,          0,      0.0, 1, 0,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60022, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "u1Start  1",         "16BIN",   0x0000FFFF,    0,          0,      0.0, 1, 1,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60023, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "u1Start  8",         "16BIN",   0x0000FFFF,    0,          0,      0.0, 1, 8,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60024, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "u1Start 15",         "16BIN",   0x0000FFFF,    0,          0,      0.0, 1,15,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60031, PAR(_UINT08,_RW,_NO), u8Register,  &u16DummyDataCnt,                        "u1Start0",           "8BIN",    0x000000FF,    0,          0,      0.0, 1, 0,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60032, PAR(_UINT08,_RW,_NO), u8Register,  &u16DummyDataCnt,                        "u1Start2",           "8BIN",    0x000000FF,    0,          0,      0.0, 1, 2,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60033, PAR(_UINT08,_RW,_NO), u8Register,  &u16DummyDataCnt,                        "u1Start7",           "8BIN",    0x000000FF,    0,          0,      0.0, 1, 7,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60034, PAR(_UINT08,_RW,_NO), u8Register,  &u16DummyDataCnt,                        "u1Start4",           "8BIN",    0x000000FF,    0,          0,      0.0, 1, 4,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60051, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "u4Start12",          "16BIN",   0x0000FFFF,    0,          0,      0.0, 4,12,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60052, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "u3Start2",           "16BIN",   0x0000FFFF,    0,          0,      0.0, 3, 2,CSMON_VISUAL_TYPE_BIN),
 //INIT_PARAMFULL(60053, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "u8Start8",           "16BIN",   0x0000FFFF,    0,          0,      0.0, 8, 8,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60054, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "u2Start4",           "16BIN",   0x0000FFFF,    0,          0,      0.0, 2, 4,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60061, PAR(_UINT08,_RW,_NO), u8Register,  &u16DummyDataCnt,                        "u4Start0",           "8BIN",    0x000000FF,    0,          0,      0.0, 4, 0,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60062, PAR(_UINT08,_RW,_NO), u8Register,  &u16DummyDataCnt,                        "u3Start2",           "8BIN",    0x000000FF,    0,          0,      0.0, 3, 2,CSMON_VISUAL_TYPE_BIN),
 //INIT_PARAMFULL(60063, PAR(_UINT08,_RW,_NO), u8Register,  &u16DummyDataCnt,                        "u8Start0",           "8BIN",    0x000000FF,    0,          0,      0.0, 8, 0,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60064, PAR(_UINT08,_RW,_NO), u8Register,  &u16DummyDataCnt,                        "u2Start4",           "8BIN",    0x000000FF,    0,          0,      0.0, 2, 4,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60081, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "u3Start0",           "16BIN",   0x0000FFFF,    0,          0,      0.0, 3, 0,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60082, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "u5Start2",           "16BIN",   0x0000FFFF,    0,          0,      0.0, 5, 2,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60083, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "u7Start8",           "16BIN",   0x0000FFFF,    0,          0,      0.0, 7, 8,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60084, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "u2Start14",          "16BIN",   0x0000FFFF,    0,          0,      0.0, 2,14,CSMON_VISUAL_TYPE_BIN),


 INIT_PARAMFULL(3000,  PAR(_A_UINT08,_RW,_NO), u8Register, au8TestCharArray32,                     "au8TestCharArray32", "str",     0xFF,          0,          0,      0.0, 0,32,CSMON_VISUAL_TYPE_DEF),
 INIT_PARAMFULL(3001,  PAR(_A_UINT08,_RW,_NO), u8Register, au8TestCharArray16,                     "au8TestCharArray16", "str",     0xFF,          0,          0,      0.0, 0,16,CSMON_VISUAL_TYPE_DEF),
 INIT_PARAMFULL(3002,  PAR(_A_UINT08,_RW,_NO), u8Register, au8TestCharArray8,                      "au8TestCharArray8",  "str",     0xFF,          0,          0,      0.0, 0, 8,CSMON_VISUAL_TYPE_DEF),


#elif CSMON_PARAMETER_LIST_TEST == CSMON_PAR_LIST_MAXIMUM_COUNT

/*                ID         Attributes      uAnyType32_t         MCU Address                          Name               Unit         Max        Min         Def      Norm */
 INIT_PARAMETER(60000, PAR(_UINT16,_RW,_NO), u16Register, &u16PeriodControl_usec,                  "CtrlLoopPeriod",     "usec",    0x0000FFFF,    0,          0,      1.0),
 INIT_PARAMETER(60001, PAR(_UINT16,_RW,_NO), u16Register, &u16DelayCtrlLoop_100nsec,               "CtrlLoopAddDelay",   "usec",    0x0000FFFF,    0,          0,      0.1),
 INIT_PARAMETER(60002, PAR(_UINT16,_RW,_NO), u16Register, &u16DelayMainLoop_usec,                  "MainLoopAddDelay",   "usec",    0x0000FFFF,    0,          0,      1.0),
 INIT_PARAMETER(60003, PAR(_UINT16,_RW,_NO), u16Register, &u16CountSetParameterFail,               "Parameters_Fail",    "unit",    0x0000FFFF,    0,          0,      1.0),
 INIT_PARAMETER(60004, PAR(_UINT32,_RO,_NO), u32Register, &u32TimeMainLoopProcessCSMON_Max_Ticks,  "MainLoopCSMON",      "usec",    0xFFFFFFFF,    0,          0,      1.0),
 INIT_PARAMETER(60005, PAR(_UINT32,_RO,_NO), u32Register, &u32TimeCSMON_ISR_Max_Ticks,             "CtrlLoopCSMON",      "usec",    0xFFFFFFFF,    0,          0,      1.0),
 INIT_PARAMETER(60006, PAR(_UINT32,_RO,_NO), u32Register, &u32TimeMainLoopCycle_Max_Ticks,         "MainLoopMeasMax",    "usec",    0xFFFFFFFF,    0,          0,      1.0),
 INIT_PARAMETER(60007, PAR(_UINT32,_RO,_NO), u32Register, &u32TimeCtrlLoopMax_Ticks,               "CtrlLoopMeasMax",    "usec",    0xFFFFFFFF,    0,          0,      1.0),
 INIT_PARAMETER(60008, PAR(_UINT32,_RO,_NO), u32Register, &u32ParamTime_Ticks,                     "ParamInitMeas",      "usec",    0xFFFFFFFF,    0,          0,      1.0),
 INIT_PARAMETER(60009, PAR(_UINT08,_RW,_NO), u8Register,  &bResetAllTimeMeasures,                  "MeasuresReset",      "unit",    true,          false,      false,  1.0),

 INIT_PARAMETER(65534, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "ModbusMsgCntr",      "unit",    0x0000FFFF,    0,          0,      0.0),
 INIT_PARAMETER(    0, PAR(_UINT08,_WO,_WR), u8Register,  &bDummyReqstDevRunning,                  "DeviceRunning",      "boolean", true,          false,      false,  1.0),    /* Parameter ID 0 - Wr Addr */
 INIT_PARAMETER(    0, PAR(_UINT08,_RO,_NO), u8Register,  &bDummyStatsDevRunning,                  "DeviceRunning",      "boolean", true,          false,      false,  1.0),    /* Parameter ID 0 - Rd Addr */
 INIT_PARAMETER(    1, PAR(_SINT16,_RW,_NO), s16Register, &s16ScaleDataTst,                        "ReadWriteScale",     "*3.1415", 10000,         -10000,     0,      3.1415),
 INIT_PARAMETER(    2, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyIncrementLoop,                  "IncLoopTest",        "A(0.5V)", 10000,         -10000,     0,      1.0F),
 INIT_PARAMETER(    3, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseA,                  "CurrentPhA",         "A",       10000,         -10000,     0,      1.0F),
 INIT_PARAMETER(    4, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseB,                  "CurrentPhB",         "A",       10000,         -10000,     0,      1.0F),
 INIT_PARAMETER(    5, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseC,                  "CurrentPhC",         "A",       10000,         -10000,     0,      1.0F),
 INIT_PARAMETER(    6, PAR(_UINT32,_RW,_WR), u32Register, &u32GetBaudError_PPM,                    "BaudError",          "%",       10000,         0,          0,      0.0001),
 INIT_PARAMETER(    7, PAR(_UINT16,_RW,_WR), u16Register, &u16WatchdogPrescalerTime,               "WatchdogTime",       "msec",    64,            1,          0,      ((1000.0*512*256)/10000000.0)),  /* 1msec; 512WatchdogDiv; 256WatchdogCnt; 10MHz INTOSC */

 INIT_PARAMETER(    8, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyVoltageDCLink,                  "VoltageBus",         "V",       10000,         -10000,     0,      1.0F),
 INIT_PARAMETER(    9, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseA,                  "CurrentPhA",         "A",       10000,         -10000,     0,      1.0F),
 INIT_PARAMETER(   10, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseB,                  "CurrentPhB",         "A",       10000,         -10000,     0,      1.0F),
 INIT_PARAMETER(   11, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseC,                  "CurrentPhC",         "A",       10000,         -10000,     0,      1.0F),

 INIT_PARAMETER(   28, PAR(_SINT32,_RW,_WR), s32Register, &s32DummyVoltageDCLink,                  "VoltageBus32",       "V",       1000000000,    -1000000000,0,      0.000001F),
 INIT_PARAMETER(   29, PAR(_SINT32,_RW,_WR), s32Register, &s32DummyCurrentPhaseA,                  "CurrentPhA32",       "A",       1000000000,    -1000000000,0,      0.000001F),
 INIT_PARAMETER(   30, PAR(_SINT32,_RW,_WR), s32Register, &s32DummyCurrentPhaseB,                  "CurrentPhB32",       "A",       1000000000,    -1000000000,0,      0.000001F),
 INIT_PARAMETER(   31, PAR(_SINT32,_RW,_WR), s32Register, &s32DummyCurrentPhaseC,                  "CurrentPhC32",       "A",       1000000000,    -1000000000,0,      0.000001F),


 INIT_PARAMETER( 1000, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1001, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1002, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1003, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1004, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1005, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1006, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1007, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1008, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1009, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1010, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1011, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1012, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1013, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1014, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1015, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1016, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1017, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1018, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1019, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1020, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1021, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1022, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1023, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1024, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1025, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1026, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1027, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1028, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1029, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1030, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1031, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1032, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1033, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1034, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1035, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1036, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1037, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1038, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1039, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1040, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1041, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1042, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1043, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1044, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1045, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1046, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1047, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1048, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1049, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1050, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1051, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1052, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1053, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1054, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1055, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1056, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1057, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1058, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1059, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1060, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1061, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1062, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1063, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1064, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1065, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1066, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1067, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1068, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1069, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1070, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1071, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1072, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1073, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1074, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1075, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1076, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1077, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1078, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1079, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1080, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1081, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1082, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1083, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1084, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1085, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1086, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1087, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1088, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1089, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1090, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1091, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1092, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1093, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1094, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1095, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1096, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1097, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1098, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1099, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),

 INIT_PARAMETER( 1100, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1101, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1102, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1103, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1104, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1105, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1106, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1107, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1108, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1109, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1110, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1111, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1112, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1113, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1114, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1115, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1116, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1117, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1118, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1119, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1120, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1121, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1122, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1123, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1124, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1125, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1126, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1127, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1128, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1129, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1130, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1131, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1132, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1133, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1134, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1135, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1136, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1137, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1138, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1139, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1140, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1141, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1142, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1143, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1144, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1145, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1146, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1147, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1148, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1149, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1150, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1151, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1152, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1153, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1154, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1155, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1156, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1157, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1158, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1159, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1160, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1161, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1162, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1163, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1164, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1165, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1166, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1167, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1168, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1169, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1170, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1171, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1172, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1173, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1174, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1175, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1176, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1177, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1178, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1179, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1180, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1181, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1182, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1183, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1184, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1185, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1186, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1187, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1188, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1189, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1190, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1191, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1192, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1193, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1194, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1195, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1196, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1197, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1198, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1199, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),

 INIT_PARAMETER( 1200, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1201, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1202, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1203, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1204, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1205, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1206, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1207, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1208, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1209, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1210, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1211, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1212, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1213, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1214, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1215, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1216, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1217, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1218, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1219, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1220, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1221, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1222, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1223, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1224, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1225, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1226, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1227, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1228, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1229, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1230, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1231, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1232, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1233, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1234, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1235, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1236, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1237, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1238, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1239, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1240, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1241, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1242, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1243, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1244, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1245, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1246, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1247, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1248, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1249, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1250, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1251, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1252, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1253, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1254, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1255, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1256, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1257, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1258, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1259, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1260, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1261, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1262, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1263, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1264, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1265, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1266, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1267, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1268, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1269, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1270, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1271, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1272, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1273, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1274, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1275, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1276, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1277, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1278, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1279, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1280, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1281, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1282, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1283, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1284, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1285, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1286, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1287, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1288, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1289, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1290, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1291, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1292, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1293, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1294, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1295, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1296, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1297, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1298, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1299, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),

 INIT_PARAMETER( 1300, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1301, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1302, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1303, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1304, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1305, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1306, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1307, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1308, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1309, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1310, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1311, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1312, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1313, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1314, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1315, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1316, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1317, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1318, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1319, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1320, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1321, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1322, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1323, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1324, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1325, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1326, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1327, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1328, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1329, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1330, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1331, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1332, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1333, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1334, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1335, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1336, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1337, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1338, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1339, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1340, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1341, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1342, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1343, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1344, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1345, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1346, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1347, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1348, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1349, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1350, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1351, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1352, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1353, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1354, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1355, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1356, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1357, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1358, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1359, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1360, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1361, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1362, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1363, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1364, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1365, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1366, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1367, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1368, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1369, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1370, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1371, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1372, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1373, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1374, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1375, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1376, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1377, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1378, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1379, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1380, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1381, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1382, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1383, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1384, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1385, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1386, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1387, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1388, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1389, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1390, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1391, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1392, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1393, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1394, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1395, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1396, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1397, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1398, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1399, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),

 INIT_PARAMETER( 1400, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1401, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1402, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1403, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1404, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1405, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1406, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1407, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1408, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1409, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1410, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1411, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1412, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1413, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1414, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1415, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1416, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1417, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1418, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1419, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1420, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1421, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1422, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1423, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1424, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1425, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1426, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1427, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1428, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1429, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1430, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1431, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1432, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1433, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1434, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1435, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1436, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1437, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1438, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1439, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1440, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1441, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1442, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1443, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1444, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1445, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1446, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1447, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1448, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1449, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1450, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1451, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1452, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1453, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1454, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1455, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1456, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1457, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1458, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1459, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1460, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1461, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1462, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1463, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1464, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1465, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1466, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1467, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1468, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1469, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1470, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1471, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1472, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1473, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1474, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1475, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1476, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1477, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1478, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1479, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1480, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1481, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1482, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1483, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1484, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1485, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1486, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1487, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1488, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1489, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1490, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1491, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1492, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1493, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1494, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1495, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1496, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1497, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1498, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1499, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),

 INIT_PARAMETER( 1500, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1501, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1502, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1503, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1504, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1505, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1506, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1507, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1508, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1509, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1510, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1511, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1512, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1513, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1514, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1515, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1516, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1517, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1518, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1519, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1520, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1521, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1522, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1523, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1524, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1525, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1526, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1527, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1528, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1529, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1530, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1531, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1532, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1533, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1534, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1535, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1536, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1537, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1538, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1539, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1540, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1541, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1542, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1543, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1544, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1545, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1546, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1547, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1548, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1549, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1550, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1551, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1552, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1553, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1554, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1555, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1556, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1557, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1558, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1559, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1560, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1561, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1562, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1563, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1564, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1565, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1566, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1567, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1568, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1569, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1570, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1571, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1572, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1573, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1574, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1575, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1576, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1577, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1578, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1579, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1580, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1581, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1582, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1583, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1584, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1585, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1586, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1587, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1588, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1589, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1590, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1591, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1592, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1593, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1594, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1595, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1596, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1597, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1598, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1599, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),

 INIT_PARAMETER( 1600, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1601, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1602, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1603, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1604, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1605, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1606, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1607, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1608, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1609, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1610, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1611, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1612, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1613, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1614, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1615, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1616, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1617, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1618, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1619, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1620, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1621, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1622, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1623, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1624, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1625, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1626, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1627, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1628, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1629, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1630, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1631, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1632, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1633, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1634, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1635, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1636, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1637, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1638, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1639, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1640, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1641, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1642, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1643, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1644, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1645, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1646, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1647, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1648, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1649, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1650, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1651, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1652, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1653, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1654, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1655, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1656, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1657, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1658, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1659, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1660, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1661, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1662, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1663, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1664, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1665, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1666, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1667, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1668, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1669, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1670, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1671, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1672, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1673, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1674, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1675, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1676, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1677, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1678, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1679, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1680, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1681, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1682, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1683, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1684, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1685, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1686, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1687, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1688, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1689, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1690, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1691, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1692, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1693, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1694, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1695, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1696, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1697, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1698, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1699, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),

 INIT_PARAMETER( 1700, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1701, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1702, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1703, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1704, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1705, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1706, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1707, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1708, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1709, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1710, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1711, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1712, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1713, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1714, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1715, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1716, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1717, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1718, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1719, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1720, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1721, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1722, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1723, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1724, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1725, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1726, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1727, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1728, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1729, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1730, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1731, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1732, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1733, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1734, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1735, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1736, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1737, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1738, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1739, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1740, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1741, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1742, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1743, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1744, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1745, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1746, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1747, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1748, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1749, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1750, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1751, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1752, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1753, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1754, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1755, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1756, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1757, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1758, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1759, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1760, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1761, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1762, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1763, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1764, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1765, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1766, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1767, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1768, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1769, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1770, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1771, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1772, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1773, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1774, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1775, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1776, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1777, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1778, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1779, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1780, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1781, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1782, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1783, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1784, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1785, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1786, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1787, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1788, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1789, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1790, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1791, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1792, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1793, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1794, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1795, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1796, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1797, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1798, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1799, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),

 INIT_PARAMETER( 1800, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1801, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1802, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1803, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1804, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1805, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1806, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1807, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1808, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1809, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1810, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1811, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1812, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1813, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1814, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1815, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1816, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1817, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1818, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1819, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1820, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1821, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1822, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1823, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1824, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1825, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1826, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1827, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1828, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1829, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1830, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1831, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1832, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1833, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1834, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1835, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1836, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1837, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1838, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1839, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1840, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1841, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1842, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1843, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1844, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1845, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1846, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1847, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1848, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1849, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1850, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1851, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1852, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1853, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1854, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1855, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1856, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1857, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1858, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1859, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1860, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1861, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1862, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1863, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1864, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1865, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1866, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1867, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1868, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1869, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1870, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1871, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1872, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1873, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1874, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1875, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1876, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1877, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1878, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1879, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1880, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1881, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1882, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1883, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1884, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1885, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1886, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1887, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1888, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1889, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1890, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1891, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1892, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1893, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1894, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1895, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1896, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1897, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1898, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1899, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),

 INIT_PARAMETER( 1900, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1901, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1902, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1903, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1904, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1905, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1906, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1907, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1908, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1909, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1910, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1911, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1912, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1913, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1914, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1915, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1916, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1917, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1918, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1919, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1920, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1921, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1922, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1923, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1924, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1925, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1926, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1927, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1928, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1929, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1930, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1931, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1932, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1933, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1934, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1935, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1936, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1937, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1938, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1939, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1940, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1941, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1942, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1943, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1944, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1945, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1946, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1947, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1948, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1949, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1950, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1951, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1952, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1953, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1954, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1955, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1956, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1957, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1958, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1959, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1960, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1961, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1962, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1963, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1964, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1965, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1966, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1967, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1968, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1969, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1970, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1971, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1972, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1973, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1974, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1975, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1976, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1977, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1978, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1979, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1980, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1981, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1982, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1983, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1984, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1985, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1986, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1987, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1988, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1989, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1990, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1991, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1992, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1993, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1994, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 INIT_PARAMETER( 1995, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 //INIT_PARAMETER( 1996, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 //INIT_PARAMETER( 1997, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 //INIT_PARAMETER( 1998, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),
 //INIT_PARAMETER( 1999, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyData,                           "Param_1000",         "A",       10000,         -10000,     0,      0.1),


#elif CSMON_PARAMETER_LIST_TEST == CSMON_PAR_LIST_EACH_TYPE_REPEATED_ALL_TYPES_COUNT_TIMES

 /*                ID         Attributes      uAnyType32_t         MCU Address                          Name               Unit         Max        Min         Def      Norm */
  INIT_PARAMETER(60000, PAR(_UINT16,_RW,_NO), u16Register, &u16PeriodControl_usec,                  "CtrlLoopPeriod",     "usec",    0x0000FFFF,    0,          0,      1.0),
  INIT_PARAMETER(60001, PAR(_UINT16,_RW,_NO), u16Register, &u16DelayCtrlLoop_100nsec,               "CtrlLoopAddDelay",   "usec",    0x0000FFFF,    0,          0,      0.1),
  INIT_PARAMETER(60002, PAR(_UINT16,_RW,_NO), u16Register, &u16DelayMainLoop_usec,                  "MainLoopAddDelay",   "usec",    0x0000FFFF,    0,          0,      1.0),
  INIT_PARAMETER(60003, PAR(_UINT16,_RW,_NO), u16Register, &u16CountSetParameterFail,               "Parameters_Fail",    "unit",    0x0000FFFF,    0,          0,      1.0),
  INIT_PARAMETER(60004, PAR(_UINT32,_RO,_NO), u32Register, &u32TimeMainLoopProcessCSMON_Max_Ticks,  "MainLoopCSMON",      "usec",    0xFFFFFFFF,    0,          0,      1.0),
  INIT_PARAMETER(60005, PAR(_UINT32,_RO,_NO), u32Register, &u32TimeCSMON_ISR_Max_Ticks,             "CtrlLoopCSMON",      "usec",    0xFFFFFFFF,    0,          0,      1.0),
  INIT_PARAMETER(60006, PAR(_UINT32,_RO,_NO), u32Register, &u32TimeMainLoopCycle_Max_Ticks,         "MainLoopMeasMax",    "usec",    0xFFFFFFFF,    0,          0,      1.0),
  INIT_PARAMETER(60007, PAR(_UINT32,_RO,_NO), u32Register, &u32TimeCtrlLoopMax_Ticks,               "CtrlLoopMeasMax",    "usec",    0xFFFFFFFF,    0,          0,      1.0),
  INIT_PARAMETER(60008, PAR(_UINT32,_RO,_NO), u32Register, &u32ParamTime_Ticks,                     "ParamInitMeas",      "usec",    0xFFFFFFFF,    0,          0,      1.0),
  INIT_PARAMETER(60009, PAR(_UINT08,_RW,_NO), u8Register,  &bResetAllTimeMeasures,                  "MeasuresReset",      "unit",    true,          false,      false,  1.0),

  INIT_PARAMETER(65534, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "ModbusMsgCntr",      "unit",    0x0000FFFF,    0,          0,      0.0),
  INIT_PARAMETER(    0, PAR(_UINT08,_WO,_WR), u8Register,  &bDummyReqstDevRunning,                  "DeviceRunning",      "boolean", true,          false,      false,  1.0),    /* Parameter ID 0 - Wr Addr */
  INIT_PARAMETER(    0, PAR(_UINT08,_RO,_NO), u8Register,  &bDummyStatsDevRunning,                  "DeviceRunning",      "boolean", true,          false,      false,  1.0),    /* Parameter ID 0 - Rd Addr */
  INIT_PARAMETER(    1, PAR(_SINT16,_RW,_NO), s16Register, &s16ScaleDataTst,                        "ReadWriteScale",     "*3.1415", 10000,         -10000,     0,      3.1415),
  INIT_PARAMETER(    2, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyIncrementLoop,                  "IncLoopTest",        "A(0.5V)", 10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    3, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseA,                  "CurrentPhA",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    4, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseB,                  "CurrentPhB",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    5, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseC,                  "CurrentPhC",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    6, PAR(_UINT32,_RW,_WR), u32Register, &u32GetBaudError_PPM,                    "BaudError",          "%",       10000,         0,          0,      0.0001),
  INIT_PARAMETER(    7, PAR(_UINT16,_RW,_WR), u16Register, &u16WatchdogPrescalerTime,               "WatchdogTime",       "msec",    64,             1,          0,      ((1000.0*512*256)/10000000.0)),  /* 1msec; 512WatchdogDiv; 256WatchdogCnt; 10MHz INTOSC */

  INIT_PARAMETER(    8, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyVoltageDCLink,                  "VoltageBus",         "V",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    9, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseA,                  "CurrentPhA",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(   10, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseB,                  "CurrentPhB",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(   11, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseC,                  "CurrentPhC",         "A",       10000,         -10000,     0,      1.0F),

  INIT_PARAMETER(10000, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "0.0",     10000,         -10000,     0,      0.0),
  INIT_PARAMETER(10001, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "0.0",     10000,         -10000,     0,      0.0),
  INIT_PARAMETER(10002, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "0.0",     10000,         -10000,     0,      0.0),
  INIT_PARAMETER(10003, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "0.0",     10000,         -10000,     0,      0.0),
  INIT_PARAMETER(10004, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "0.0",     10000,         -10000,     0,      0.0),
  INIT_PARAMETER(10005, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "0.0",     10000,         -10000,     0,      0.0),

  INIT_PARAMETER(10100, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "1.0",     10000,         -10000,     0,      1.0),
  INIT_PARAMETER(10101, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "1.0",     10000,         -10000,     0,      1.0),
  INIT_PARAMETER(10102, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "1.0",     10000,         -10000,     0,      1.0),
  INIT_PARAMETER(10103, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "1.0",     10000,         -10000,     0,      1.0),
  INIT_PARAMETER(10104, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "1.0",     10000,         -10000,     0,      1.0),
  INIT_PARAMETER(10105, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "1.0",     10000,         -10000,     0,      1.0),

  INIT_PARAMETER(10200, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "0.0",     1000000,       -1000000,   0,      0.0),
  INIT_PARAMETER(10201, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "0.0",     1000000,       -1000000,   0,      0.0),
  INIT_PARAMETER(10202, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "0.0",     1000000,       -1000000,   0,      0.0),
  INIT_PARAMETER(10203, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "0.0",     1000000,       -1000000,   0,      0.0),
  INIT_PARAMETER(10204, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "0.0",     1000000,       -1000000,   0,      0.0),
  INIT_PARAMETER(10205, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "0.0",     1000000,       -1000000,   0,      0.0),

  INIT_PARAMETER(10300, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "1.0",     1000000,       -1000000,   0,      1.0),
  INIT_PARAMETER(10301, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "1.0",     1000000,       -1000000,   0,      1.0),
  INIT_PARAMETER(10302, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "1.0",     1000000,       -1000000,   0,      1.0),
  INIT_PARAMETER(10303, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "1.0",     1000000,       -1000000,   0,      1.0),
  INIT_PARAMETER(10304, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "1.0",     1000000,       -1000000,   0,      1.0),
  INIT_PARAMETER(10305, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "1.0",     1000000,       -1000000,   0,      1.0),

  INIT_PARAMETER(10400, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "0.0",     255,           0,          0,      0.0),
  INIT_PARAMETER(10401, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "0.0",     255,           0,          0,      0.0),
  INIT_PARAMETER(10402, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "0.0",     255,           0,          0,      0.0),
  INIT_PARAMETER(10403, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "0.0",     255,           0,          0,      0.0),
  INIT_PARAMETER(10404, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "0.0",     255,           0,          0,      0.0),
  INIT_PARAMETER(10405, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "0.0",     255,           0,          0,      0.0),

  INIT_PARAMETER(10500, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "1.0",     255,           0,          0,      1.0),
  INIT_PARAMETER(10501, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "1.0",     255,           0,          0,      1.0),
  INIT_PARAMETER(10502, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "1.0",     255,           0,          0,      1.0),
  INIT_PARAMETER(10503, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "1.0",     255,           0,          0,      1.0),
  INIT_PARAMETER(10504, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "1.0",     255,           0,          0,      1.0),
  INIT_PARAMETER(10505, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "1.0",     255,           0,          0,      1.0),


#elif CSMON_PARAMETER_LIST_TEST == CSMON_PAR_LIST_ALL_TYPES_REPEATED_ALL_TYPES_COUNT_TIMES


 /*                ID         Attributes      uAnyType32_t         MCU Address                          Name               Unit         Max        Min         Def      Norm */
  INIT_PARAMETER(60000, PAR(_UINT16,_RW,_NO), u16Register, &u16PeriodControl_usec,                  "CtrlLoopPeriod",     "usec",    0x0000FFFF,    0,          0,      1.0),
  INIT_PARAMETER(60001, PAR(_UINT16,_RW,_NO), u16Register, &u16DelayCtrlLoop_100nsec,               "CtrlLoopAddDelay",   "usec",    0x0000FFFF,    0,          0,      0.1),
  INIT_PARAMETER(60002, PAR(_UINT16,_RW,_NO), u16Register, &u16DelayMainLoop_usec,                  "MainLoopAddDelay",   "usec",    0x0000FFFF,    0,          0,      1.0),
  INIT_PARAMETER(60003, PAR(_UINT16,_RW,_NO), u16Register, &u16CountSetParameterFail,               "Parameters_Fail",    "unit",    0x0000FFFF,    0,          0,      1.0),
  INIT_PARAMETER(60004, PAR(_UINT32,_RO,_NO), u32Register, &u32TimeMainLoopProcessCSMON_Max_Ticks,  "MainLoopCSMON",      "usec",    0xFFFFFFFF,    0,          0,      1.0),
  INIT_PARAMETER(60005, PAR(_UINT32,_RO,_NO), u32Register, &u32TimeCSMON_ISR_Max_Ticks,             "CtrlLoopCSMON",      "usec",    0xFFFFFFFF,    0,          0,      1.0),
  INIT_PARAMETER(60006, PAR(_UINT32,_RO,_NO), u32Register, &u32TimeMainLoopCycle_Max_Ticks,         "MainLoopMeasMax",    "usec",    0xFFFFFFFF,    0,          0,      1.0),
  INIT_PARAMETER(60007, PAR(_UINT32,_RO,_NO), u32Register, &u32TimeCtrlLoopMax_Ticks,               "CtrlLoopMeasMax",    "usec",    0xFFFFFFFF,    0,          0,      1.0),
  INIT_PARAMETER(60008, PAR(_UINT32,_RO,_NO), u32Register, &u32ParamTime_Ticks,                     "ParamInitMeas",      "usec",    0xFFFFFFFF,    0,          0,      1.0),
  INIT_PARAMETER(60009, PAR(_UINT08,_RW,_NO), u8Register,  &bResetAllTimeMeasures,                  "MeasuresReset",      "unit",    true,          false,      false,  1.0),

  INIT_PARAMETER(65534, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "ModbusMsgCntr",      "unit",    0x0000FFFF,    0,          0,      0.0),
  INIT_PARAMETER(    0, PAR(_UINT08,_WO,_WR), u8Register,  &bDummyReqstDevRunning,                  "DeviceRunning",      "boolean", true,          false,      false,  1.0),    /* Parameter ID 0 - Wr Addr */
  INIT_PARAMETER(    0, PAR(_UINT08,_RO,_NO), u8Register,  &bDummyStatsDevRunning,                  "DeviceRunning",      "boolean", true,          false,      false,  1.0),    /* Parameter ID 0 - Rd Addr */
  INIT_PARAMETER(    1, PAR(_SINT16,_RW,_NO), s16Register, &s16ScaleDataTst,                        "ReadWriteScale",     "*3.1415", 10000,         -10000,     0,      3.1415),
  INIT_PARAMETER(    2, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyIncrementLoop,                  "IncLoopTest",        "A(0.5V)", 10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    3, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseA,                  "CurrentPhA",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    4, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseB,                  "CurrentPhB",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    5, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseC,                  "CurrentPhC",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    6, PAR(_UINT32,_RW,_WR), u32Register, &u32GetBaudError_PPM,                    "BaudError",          "%",       10000,         0,          0,      0.0001),
  INIT_PARAMETER(    7, PAR(_UINT16,_RW,_WR), u16Register, &u16WatchdogPrescalerTime,               "WatchdogTime",       "msec",    64,             1,          0,      ((1000.0*512*256)/10000000.0)),  /* 1msec; 512WatchdogDiv; 256WatchdogCnt; 10MHz INTOSC */

  INIT_PARAMETER(    8, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyVoltageDCLink,                  "VoltageBus",         "V",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    9, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseA,                  "CurrentPhA",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(   10, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseB,                  "CurrentPhB",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(   11, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseC,                  "CurrentPhC",         "A",       10000,         -10000,     0,      1.0F),

  INIT_PARAMETER(10000, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "0.0",     10000,         -10000,     0,      0.0),
  INIT_PARAMETER(10100, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "0.0",     10000,         -10000,     0,      0.0),
  INIT_PARAMETER(10200, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "0.0",     10000,         -10000,     0,      0.0),
  INIT_PARAMETER(10300, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "0.0",     10000,         -10000,     0,      0.0),
  INIT_PARAMETER(10400, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "0.0",     10000,         -10000,     0,      0.0),
  INIT_PARAMETER(10500, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "0.0",     10000,         -10000,     0,      0.0),

  INIT_PARAMETER(10001, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "1.0",     10000,         -10000,     0,      1.0),
  INIT_PARAMETER(10101, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "1.0",     10000,         -10000,     0,      1.0),
  INIT_PARAMETER(10201, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "1.0",     10000,         -10000,     0,      1.0),
  INIT_PARAMETER(10301, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "1.0",     10000,         -10000,     0,      1.0),
  INIT_PARAMETER(10401, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "1.0",     10000,         -10000,     0,      1.0),
  INIT_PARAMETER(10501, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "1.0",     10000,         -10000,     0,      1.0),

  INIT_PARAMETER(10002, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "0.0",     1000000,       -1000000,   0,      0.0),
  INIT_PARAMETER(10102, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "0.0",     1000000,       -1000000,   0,      0.0),
  INIT_PARAMETER(10202, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "0.0",     1000000,       -1000000,   0,      0.0),
  INIT_PARAMETER(10302, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "0.0",     1000000,       -1000000,   0,      0.0),
  INIT_PARAMETER(10402, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "0.0",     1000000,       -1000000,   0,      0.0),
  INIT_PARAMETER(10502, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "0.0",     1000000,       -1000000,   0,      0.0),

  INIT_PARAMETER(10003, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "1.0",     1000000,       -1000000,   0,      1.0),
  INIT_PARAMETER(10103, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "1.0",     1000000,       -1000000,   0,      1.0),
  INIT_PARAMETER(10203, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "1.0",     1000000,       -1000000,   0,      1.0),
  INIT_PARAMETER(10303, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "1.0",     1000000,       -1000000,   0,      1.0),
  INIT_PARAMETER(10403, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "1.0",     1000000,       -1000000,   0,      1.0),
  INIT_PARAMETER(10503, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "1.0",     1000000,       -1000000,   0,      1.0),

  INIT_PARAMETER(10004, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "0.0",     255,           0,          0,      0.0),
  INIT_PARAMETER(10104, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "0.0",     255,           0,          0,      0.0),
  INIT_PARAMETER(10204, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "0.0",     255,           0,          0,      0.0),
  INIT_PARAMETER(10304, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "0.0",     255,           0,          0,      0.0),
  INIT_PARAMETER(10404, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "0.0",     255,           0,          0,      0.0),
  INIT_PARAMETER(10504, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "0.0",     255,           0,          0,      0.0),

  INIT_PARAMETER(10005, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "1.0",     255,           0,          0,      1.0),
  INIT_PARAMETER(10105, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "1.0",     255,           0,          0,      1.0),
  INIT_PARAMETER(10205, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "1.0",     255,           0,          0,      1.0),
  INIT_PARAMETER(10305, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "1.0",     255,           0,          0,      1.0),
  INIT_PARAMETER(10405, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "1.0",     255,           0,          0,      1.0),
  INIT_PARAMETER(10505, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "1.0",     255,           0,          0,      1.0),


#else

#endif

#endif /* #if _CSMON_USE_EXTERNAL_PARAMETER_LIST */


};

#endif  /* #if _CSMON_USE_EXTERNAL_PARAMETER_TABLE */

#endif  /* #if CSMON_CONFIG == 0 */


volatile uint16_t maximum_count_params = 0;

/* *****************************************************************************
 * Prototypes of functions definitions
 **************************************************************************** */
void ControlProcess(void);
void ParameterInitialization(void);
void RecordersInitialization(void);
void ScopesInitialization(void);

/* *****************************************************************************
 * Functions
 **************************************************************************** */

/* *****************************************************************************
 * CSMON_vSetDateTime - Set RTC Value - called from CSMON library
 **************************************************************************** */
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
    u32TimeCtrlLoop_Ticks = CPUTimer_getTimerCount(CPUTIMER1_BASE);



#ifdef _CS_1291
    GPIO_writePin_2806x(STAT_LED_EQEP1I_PIN, 1);     /* J17 at board corner before ground (pin 13 - second outside pin corner to middle) */
#elif defined(_CS_1107_SCC_R01)

#else
    GPIO_writePin(STAT_LED_R_PIN, STAT_LED_ENABLE_LEVEL_LOW);     /* Red LED (closest to the Debug Header) */
#endif



    //
    // Test For Data Consistency and Control Emulation
    //
    if (bDummyReqstDevRunning)
    {
        if (bDummyStatsDevRunning == false)
        {
            bDummyStatsDevRunning = true;

#if RECORDER0_ONLY_TEST == 0
            //#if EXTERNAL_RECORDERS == 0
            //
            // CSMON Internal Recorders Start with Already Setup Configuration
            //
            CSMON_vSetStartRecorderParameterMask(CSMON_MASK_RECORDERS_012);
            //#endif
#else
            CSMON_vSetStartRecorderParameterMask(CSMON_MASK_RECORDER_0);
#endif

            s16DummyCurrentPhaseA = s16DummyCurrentPhaseAStartup;
            s16DummyCurrentPhaseB = s16DummyCurrentPhaseBStartup;
            s16DummyCurrentPhaseC = s16DummyCurrentPhaseCStartup;
            s16DummyVoltageDCLink = s16DummyVoltageDCLinkStartup;

            s32DummyCurrentPhaseA += (int32_t)s16DummyCurrentPhaseAStartup*10000;
            s32DummyCurrentPhaseB += (int32_t)s16DummyCurrentPhaseBStartup*10000;
            s32DummyCurrentPhaseC += (int32_t)s16DummyCurrentPhaseCStartup*10000;
            s32DummyVoltageDCLink += (int32_t)s16DummyVoltageDCLinkStartup*10000;
        }
        else
        {
            s16DummyCurrentPhaseA += s16DummyIncrementLoopA;
            s16DummyCurrentPhaseB += s16DummyIncrementLoopB;
            s16DummyCurrentPhaseC += s16DummyIncrementLoopC;
            s16DummyVoltageDCLink += s16DummyIncrementLoopV;

            s32DummyCurrentPhaseA += (int32_t)s16DummyIncrementLoopA*10000;
            s32DummyCurrentPhaseB += (int32_t)s16DummyIncrementLoopB*10000;
            s32DummyCurrentPhaseC += (int32_t)s16DummyIncrementLoopC*10000;
            s32DummyVoltageDCLink += (int32_t)s16DummyIncrementLoopV*10000;


            { //DPN Start --------------------------------------------

                static double t=0;

                t+=50e-6; //ControlProcess is called each 50us
                s32DummyCurrentPhaseA = (int32_t)(t*1000000.0); //the parameter is 1e-6 scaled
                s32DummyCurrentPhaseB = (int32_t)(1000000000.0*sin(2*3.1415*1*t));  /* 1 Hz sine wave */

            } //DPN End --------------------------------------------


            s16DummyCurrentPhaseC += s16DummyIncrementLoopCDiff;
            s16DummyIncrementLoopCDiff = 0 - s16DummyIncrementLoopCDiff;

        }

    }
    else
    {
        if (bDummyStatsDevRunning == true)
        {
            bDummyStatsDevRunning = false;

#if RECORDER0_ONLY_TEST == 0
            //#if EXTERNAL_RECORDERS == 0
            //
            // CSMON Internal Recorders Stop (Trigger Emulation)
            //
            //CSMON_vSetStopRecorderParameterMask(CSMON_MASK_RECORDERS_012);
            CSMON_vSetStopRecorderParameterMask(CSMON_MASK_RECORDER_2);
            //#endif
#endif

            s16DummyCurrentPhaseAIdle = s16DummyCurrentPhaseA;
            s16DummyCurrentPhaseBIdle = s16DummyCurrentPhaseB;
            s16DummyCurrentPhaseCIdle = s16DummyCurrentPhaseC;
            s16DummyVoltageDCLinkIdle = s16DummyVoltageDCLink;

            s16DummyCurrentPhaseA = s16DummyCurrentPhaseAStop;
            s16DummyCurrentPhaseB = s16DummyCurrentPhaseBStop;
            s16DummyCurrentPhaseC = s16DummyCurrentPhaseCStop;
            s16DummyVoltageDCLink = s16DummyVoltageDCLinkStop;
        }
        else
        {
            s16DummyCurrentPhaseA = s16DummyCurrentPhaseAIdle;
            s16DummyCurrentPhaseB = s16DummyCurrentPhaseBIdle;
            s16DummyCurrentPhaseC = s16DummyCurrentPhaseCIdle;
            s16DummyVoltageDCLink = s16DummyVoltageDCLinkIdle;

        }

    }


    //
    // Artificial Delay Control Loop
    //
    //GPIO_writePin_2806x(STAT_LED_EQEP1I_PIN, 1);     /* J17 at board corner before ground (pin 13 - second outside pin corner to middle) */

    SysCtl_delay(u32DelayCtrlLoop_Ticks);

    //GPIO_writePin_2806x(STAT_LED_EQEP1I_PIN, 0);     /* J17 at board corner before ground (pin 13 - second outside pin corner to middle) */

    u32TimeCSMON_ISR_Ticks = CPUTimer_getTimerCount(CPUTIMER1_BASE);
    //
    // Process Passed Flag Set - Need to be called from Processes with higher priority level in order CSMON to be able to get meaning-full (consistent) data
    eResponseCode_CSMON_eSetFlagProcessPassed = CSMON_eSetFlagProcessPassed (CSMON_ID_PROCESS_CONTROL_PRIMARY);
    ASSERT(eResponseCode_CSMON_eSetFlagProcessPassed != CSMON_RESPONSE_CODE_OK);
    // Check CSMON Response Code (... or Embed Assert For Debug) if needed
    u32TimeCSMON_ISR_Ticks = 0 - (CPUTimer_getTimerCount(CPUTIMER1_BASE) - u32TimeCSMON_ISR_Ticks);//down count
    if (u32TimeCSMON_ISR_Ticks > u32TimeCSMON_ISR_Max_Ticks)
    {
        u32TimeCSMON_ISR_Max_Ticks = u32TimeCSMON_ISR_Ticks;
    }

    //GPIO_writePin_2806x(STAT_LED_EQEP1I_PIN, 1);     /* J17 at board corner before ground (pin 13 - second outside pin corner to middle) */
    //GPIO_writePin_2806x(STAT_LED_EQEP1I_PIN, 0);     /* J17 at board corner before ground (pin 13 - second outside pin corner to middle) */

    u32TimeCtrlLoop_Ticks = 0 - (CPUTimer_getTimerCount(CPUTIMER1_BASE) - u32TimeCtrlLoop_Ticks);//down count
    if (u32TimeCtrlLoop_Ticks > u32TimeCtrlLoopMax_Ticks)
    {
        u32TimeCtrlLoopMax_Ticks = u32TimeCtrlLoop_Ticks;
    }
#ifdef _CS_1291
    GPIO_writePin_2806x(STAT_LED_EQEP1I_PIN, 0);     /* J17 at board corner before ground (pin 13 - second outside pin corner to middle) */
#elif defined(_CS_1107_SCC_R01)

#else
    GPIO_writePin(STAT_LED_R_PIN, STAT_LED_DISABLE_LVL_HIGH);    /* Red LED (closest to the Debug Header) */
#endif
}



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

/* *****************************************************************************
 * ParameterInitialization
 **************************************************************************** */
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

#ifndef __TMS320F2806x__
    /* Note: with current EMIF board configuration 32Bit Writes Are not working - need use 16-bit writes */
    uParamVerBackup.au16Word[0] = EMIF_AUX_pu16ParamVerBackupInEmif[0];              /* Get Stored In MRAM TableVer Backup */
    uParamVerBackup.au16Word[1] = EMIF_AUX_pu16ParamVerBackupInEmif[1];              /* Get Stored In MRAM TableVer Backup */
    uDateTimeBackup.au16Word[0] = EMIF_AUX_pu16DateTimeBackupInEmif[0];              /* Get Stored In MRAM DateTime Backup */
    uDateTimeBackup.au16Word[1] = EMIF_AUX_pu16DateTimeBackupInEmif[1];              /* Get Stored In MRAM DateTime Backup */
    uCheckSumBackup.au16Word[0] = EMIF_AUX_pu16CheckSumBackupInEmif[0];              /* Get Stored In MRAM CheckSum Backup */
    uCheckSumBackup.au16Word[1] = EMIF_AUX_pu16CheckSumBackupInEmif[1];              /* Get Stored In MRAM CheckSum Backup */
#else
    uParamVerBackup.u32Register = 0xFFFFFFFF;
    uDateTimeBackup.u32Register = 0xFFFFFFFF;
    uCheckSumBackup.u32Register = 0xAA561234;
#endif
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
#ifdef _CS_1291

#elif defined(_CS_1107_SCC_R01)

#else
            GPIO_writePin(STAT_LED_A_B_PIN, (u16Index & 1) );     /* Amber LED (middle Led) */
#endif

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

#ifndef __TMS320F2806x__
        /* Backup ParamVer */
        uParamVerBackup.u32Register = u32ParamVer;
        EMIF_AUX_pu16ParamVerBackupInEmif[0] = uParamVerBackup.au16Word[0];
        EMIF_AUX_pu16ParamVerBackupInEmif[1] = uParamVerBackup.au16Word[1];
        /* Backup DateTime */
        uDateTimeBackup.u32Register = u32DateTime;
        EMIF_AUX_pu16DateTimeBackupInEmif[0] = uDateTimeBackup.au16Word[0];
        EMIF_AUX_pu16DateTimeBackupInEmif[1] = uDateTimeBackup.au16Word[1];
        /* Backup Checksum */
        uCheckSumBackup.u32Register = CSMON_u32GetParameterCheckSum();
        EMIF_AUX_pu16CheckSumBackupInEmif[0] = uCheckSumBackup.au16Word[0];
        EMIF_AUX_pu16CheckSumBackupInEmif[1] = uCheckSumBackup.au16Word[1];
#endif

#ifdef _CS_1291

#elif defined(_CS_1107_SCC_R01)

#else
        GPIO_writePin(STAT_LED_A_B_PIN, STAT_LED_ENABLE_LEVEL_LOW);         /* Amber LED (middle Led) */
#endif

        CSMON_eApplyParameterChanges();                         /* Internal Library Apply Written Parameters */

#ifdef _CS_1291

#elif defined(_CS_1107_SCC_R01)

#else
        GPIO_writePin(STAT_LED_A_B_PIN, STAT_LED_DISABLE_LVL_HIGH);        /* Amber LED (middle Led) */
#endif


        /* Recorder And Scope Initialization Made Once after parameter initialized */
        RecordersInitialization();
        ScopesInitialization();




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

#if 0
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetRecorderConfiguration (
            CSMON_RECORDER_0,
            RECORDER0_PRETRIGGER_SAMPLE_COUNT,   /* PreTriggerSampleCount */
            RECORDER0_TOTAL_SAMPLE_COUNT,   /* TotalSampleCount */
            RECORDER0_SAMPLE_FREQUENCY_HZ); /* Sample Frequency in Hz */
#else
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetRecorderConfigurationSkipSamples (
            CSMON_RECORDER_0,
            RECORDER0_PRETRIGGER_SAMPLE_COUNT,   /* PreTriggerSampleCount */
            RECORDER0_TOTAL_SAMPLE_COUNT,   /* TotalSampleCount */
            RECORDER0_SAMPLE_FREQUENCY_HZ / 3.0); /* Sample Frequency in Hz */
#endif



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


void vSetWatchdogPrescalerTimeDiv(uint16_t u16Prescaler)
{
#if 1
    if ((u16Prescaler >= SYSCTL_WD_PRESCALE_1) && (u16Prescaler <= SYSCTL_WD_PRESCALE_64))
    {
        u16WatchdogPrescalerTime = (1 << (u16Prescaler-1));
    }
    else
    {
        u16WatchdogPrescalerTime = 1;
    }


#else
    switch(u16Prescaler)
    {
    case SYSCTL_WD_PRESCALE_1:
        u16WatchdogPrescalerTime = 1;
        break;
    case SYSCTL_WD_PRESCALE_2:
        u16WatchdogPrescalerTime = 2;
        break;
    case SYSCTL_WD_PRESCALE_4:
        u16WatchdogPrescalerTime = 4;
        break;
    case SYSCTL_WD_PRESCALE_8:
        u16WatchdogPrescalerTime = 8;
        break;
    case SYSCTL_WD_PRESCALE_16:
        u16WatchdogPrescalerTime = 16;
        break;
    case SYSCTL_WD_PRESCALE_32:
        u16WatchdogPrescalerTime = 32;
        break;
    case SYSCTL_WD_PRESCALE_64:
        u16WatchdogPrescalerTime = 64;
        break;
    default:
        u16WatchdogPrescalerTime = 1;
        break;

    }
#endif
}

void vGetWatchdogPrescaler(uint16_t u16TimeDiv)
{
#if 1
    u16WatchdogPrescaler = 1;
    while (u16TimeDiv > 1)
    {
        u16TimeDiv >>= 1;
        u16WatchdogPrescaler++;
    }
    vSetWatchdogPrescalerTimeDiv(u16WatchdogPrescaler);
    u16WatchdogPrescalerTimeOld = u16WatchdogPrescalerTime;
#else
    if(u16TimeDiv >= 64)
    {
        u16WatchdogPrescaler = SYSCTL_WD_PRESCALE_64;
        u16WatchdogPrescalerTime = u16WatchdogPrescalerTimeOld = 64;
    }
    else if(u16TimeDiv >= 32)
    {
        u16WatchdogPrescaler = SYSCTL_WD_PRESCALE_32;
        u16WatchdogPrescalerTime = u16WatchdogPrescalerTimeOld = 32;
    }
    else if(u16TimeDiv >= 16)
    {
        u16WatchdogPrescaler = SYSCTL_WD_PRESCALE_16;
        u16WatchdogPrescalerTime = u16WatchdogPrescalerTimeOld = 16;
    }
    else if(u16TimeDiv >= 8)
    {
        u16WatchdogPrescaler = SYSCTL_WD_PRESCALE_8;
        u16WatchdogPrescalerTime = u16WatchdogPrescalerTimeOld = 8;
    }
    else if(u16TimeDiv >= 4)
    {
        u16WatchdogPrescaler = SYSCTL_WD_PRESCALE_4;
        u16WatchdogPrescalerTime = u16WatchdogPrescalerTimeOld = 4;
    }
    else if(u16TimeDiv >= 2)
    {
        u16WatchdogPrescaler = SYSCTL_WD_PRESCALE_2;
        u16WatchdogPrescalerTime = u16WatchdogPrescalerTimeOld = 2;
    }
    else
    {
        u16WatchdogPrescaler = SYSCTL_WD_PRESCALE_1;
        u16WatchdogPrescalerTime = u16WatchdogPrescalerTimeOld = 1;
    }
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





uint16_t u16FreeRunningTimerTicksPerMicroSecond;
uint16_t u16FreeRunningTimerPrescaller;


/* *****************************************************************************
 * main
 **************************************************************************** */
void main(void)
{



    //
    // Configure PLL, disable WD, enable peripheral clocks.
    //
    Device_init();

    //
    // Disable pin locks and enable internal PullUps.
    //
    Device_initGPIO();



#ifdef _CS_1211

#elif !defined(__TMS320F2806x__)
    //
    // EMIF
    //
    EMIF_DRV_vInit();
#endif

    //
    // Clear all interrupts and initialize PIE vector table:
    // Disable CPU interrupts
    // Initialize interrupt controller and vector table.
    //

#if defined(__TMS320F2806x__)
    //for __TMS320F2806x__ Interrupts fixed in Device Init?
#else
    Interrupt_initModule();
    Interrupt_initVectorTable();
#endif

    //
    // LEDs or debug GPIO
    //
#ifdef _CS_1291
    /* J17 at board corner before ground (pin 13 - second outside pin corner to middle) */
    GPIO_setPinConfigOutput(STAT_LED_EQEP1I_PIN);
    GPIO_writePin_2806x(STAT_LED_EQEP1I_PIN, 0);


#elif defined(_CS_1107_SCC_R01)

#else
    // STAT_LED_G_PIN is the LED STATUS pin. - Init and Main Loop CSMON CPU Load -> Green LED (closest to the MCU Led)
    GPIO_setPinConfigOutput(STAT_LED_G_PIN);
    GPIO_writePin(STAT_LED_G_PIN, STAT_LED_ENABLE_LEVEL_LOW);
    GPIO_writePin(STAT_LED_G_PIN, STAT_LED_DISABLE_LVL_HIGH);

    // STAT_LED_A_B_PIN is the LED STATUS pin. - Parameter Initialize -> Amber LED (middle Led)
    GPIO_setPinConfigOutput(STAT_LED_A_B_PIN);
    GPIO_writePin(STAT_LED_A_B_PIN, STAT_LED_ENABLE_LEVEL_LOW);
    GPIO_writePin(STAT_LED_A_B_PIN, STAT_LED_DISABLE_LVL_HIGH);

    // STAT_LED_R_PIN is the LED STATUS pin. - Control Process -> Red LED (closest to the Debug Header)
    GPIO_setPinConfigOutput(STAT_LED_R_PIN);
    GPIO_writePin(STAT_LED_R_PIN, STAT_LED_ENABLE_LEVEL_LOW);
    GPIO_writePin(STAT_LED_R_PIN, STAT_LED_DISABLE_LVL_HIGH);


    //
    // CLK_EN_FPGA_PIN is the FPGA Clock Enable pin. - generally not needed - there is pull-up
    //
    GPIO_setPinConfigOutput(CLK_EN_FPGA_PIN);
    GPIO_writePin(CLK_EN_FPGA_PIN, 1);
#endif




    #ifdef _CSMON_FREE_RUN_TIMER_EXTERNAL           /* defined in Project Configuration Build Settings */
    //
    // CPUTimer1 Initialization as Free-Run Mode with Maximum Period
    #define CPUTIMER1_PRESCALLER_VALUE      ((DEVICE_SYSCLK_FREQ /   1000000)-1)    /* Timer Tick 1.0 microseconds */
    //#define CPUTIMER1_PRESCALLER_VALUE      ((DEVICE_SYSCLK_FREQ / 200000000)-1)    /* Timer Tick 5.0 nanoseconds */
    //
    CPUTimer_setPeriod(CPUTIMER1_BASE, 0xFFFFFFFF);
    CPUTimer_setPreScaler(CPUTIMER1_BASE, CPUTIMER1_PRESCALLER_VALUE);
    CPUTimer_stopTimer(CPUTIMER1_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER1_BASE);
    CPUTimer_enableInterrupt(CPUTIMER1_BASE);
    CPUTimer_startTimer(CPUTIMER1_BASE);
    #endif

    //
    //  Calculate Free Running Timer Ticks Per microsecond
    //
    u16FreeRunningTimerPrescaller = (HWREGH(CPUTIMER1_BASE + CPUTIMER_O_TPRH) << 8U) + (HWREGH(CPUTIMER1_BASE + CPUTIMER_O_TPR) & CPUTIMER_TPR_TDDR_M);
    u16FreeRunningTimerTicksPerMicroSecond = ((DEVICE_SYSCLK_FREQ / 1000000) / (u16FreeRunningTimerPrescaller+1));
    if (u16FreeRunningTimerTicksPerMicroSecond == 0)
    {
        u16FreeRunningTimerTicksPerMicroSecond = 1;
    }

#ifdef _CS_1211

#elif defined(_CS_1291)

#elif defined(_CS_1107_SCC_R01)

#else
    //
    //  FPGA SCI Communication
    //
    #define SCIRXDC_PIN         139     /* MIX With FLT4_PIN */
    #define SCITXDC_PIN         140     /* MIX With FLT5_PIN */
    //
    // SCIRXDC_PIN is the SCI Rx pin.
    //
    GPIO_setPinConfig(GPIO_PIN_MODE(SCIRXDC_PIN, SCIRXDC));
    GPIO_setDirectionMode(SCIRXDC_PIN, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(SCIRXDC_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SCIRXDC_PIN, GPIO_QUAL_ASYNC);
    GPIO_setMasterCore(SCIRXDC_PIN, GPIO_CORE_CPU1);
    //
    // SCITXDC_PIN is the SCI Tx pin.
    //
    GPIO_setPinConfig(GPIO_PIN_MODE(SCITXDC_PIN, SCITXDC));
    GPIO_setDirectionMode(SCITXDC_PIN, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(SCITXDC_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SCITXDC_PIN, GPIO_QUAL_ASYNC);
    GPIO_setMasterCore(SCITXDC_PIN, GPIO_CORE_CPU1);
    //
    //  SCIC Init and FPGA_SCI_DRV Init
    //
    Interrupt_register(INT_SCIC_RX, &SCIC_DRV_RXFIFOISR);
    Interrupt_register(INT_SCIC_TX, &SCIC_DRV_TXFIFOISR);
    SCI_DRV_vInitFIFO(
            SCI_DRV_u32GetBaseFromModuleIndex(FPGA_SCI_DRV_UART_MODULE), FPGA_SCI_DRV_UART_BAUD,
            (SCI_CONFIG_WLEN_8 | FPGA_SCI_DRV_UART_STOP_BITS | FPGA_SCI_DRV_UART_PARITY | FPGA_SCI_DRV_UART_ADDRESS_MODE));
    SCI_disableInterrupt(SCI_DRV_u32GetBaseFromModuleIndex(FPGA_SCI_DRV_UART_MODULE), SCI_INT_TXFF);
    UART_DRV_vInit((UART_DRV_eModule_t)FPGA_SCI_DRV_UART_MODULE);
    FPGA_SCI_DRV_vSetupFreeTimerTicksPerMicroSecond(u16FreeRunningTimerTicksPerMicroSecond);
    Interrupt_enable(INT_SCIC_RX);
    Interrupt_enable(INT_SCIC_TX);
#endif


    //RecordersInitialization();


#ifdef _CS_1211

#elif defined(_CS_1291)

#elif defined(_CS_1107_SCC_R01)

#else
    GPIO_writePin(STAT_LED_G_PIN, STAT_LED_ENABLE_LEVEL_LOW); /* Green LED (closest to the MCU Led) */
#endif
    //
    // CSMON Initialization -> ~ 2.25ms
    //
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

#ifdef _CS_1211

#elif defined(_CS_1291)

#elif defined(_CS_1107_SCC_R01)

#else
    GPIO_writePin(STAT_LED_G_PIN, STAT_LED_DISABLE_LVL_HIGH); /* Green LED (closest to the MCU Led) */
#endif



    //
    // Firmware Application Version Set to CSMON
    //
    (void)CSMON_eSetFirmwareApplicationVersion(TEST_CSMON_APPLICATION_VERSION);

    //
    // CSMON Parameter, Recorder, Scope Test Initialization
    //
    u32ParamTime_Ticks = CPUTimer_getTimerCount(CPUTIMER1_BASE);

#if _CSMON_USE_EXTERNAL_PARAMETER_TABLE

    maximum_count_params = (uint16_t)GetNumberOfParams();

    uint16_t count_params;
    #if 1
    count_params = 100;
    if (count_params > (uint16_t)GetNumberOfParams())
    {
        count_params = (uint16_t)GetNumberOfParams();
    }
    #else
    count_params = (uint16_t)GetNumberOfParams();
    #endif

    CSMON_eSetParameterTableIndex((uint16_t *)&ParTable[0].index, (uint16_t)((uint32_t)(&ParTable[1].index) - (uint32_t)(&ParTable[0].index)), (uint16_t)count_params);
    //CSMON_eSetParameterTableIndex((uint16_t *)&ParTable[0].index, (uint16_t)((uint32_t)(&ParTable[1].index) - (uint32_t)(&ParTable[0].index)), (uint16_t)GetNumberOfParams());
    CSMON_eSetParameterTableDescr((void *)&ParTable[0].ParDes, (uint16_t)((uint32_t)(&ParTable[1].ParDes) - (uint32_t)(&ParTable[0].ParDes)));
    /* Recorder And Scope Initialization Made Once after parameter initialized */
    RecordersInitialization();
    ScopesInitialization();

#elif _CSMON_USE_EXTERNAL_PARAMETER_LIST
    ParameterInitialization();
    ExternalParametersInitialization();
#else
    ParameterInitialization();
#endif

    u32ParamTime_Ticks = 0 - (CPUTimer_getTimerCount(CPUTIMER1_BASE) - u32ParamTime_Ticks);//down count



    //
    // CSMON Internal Recorders Setup with Already Made Configuration
    //
    #if RECORDER0_ONLY_TEST == 0
    CSMON_vAddSetupRecorderParameterMask(CSMON_MASK_RECORDERS_012);
    #else
    CSMON_vAddSetupRecorderParameterMask(CSMON_MASK_RECORDER_0);
    #endif


    //
    // Register Function Call In CSMON Timer Period ISR (default Timer Period is 50 usec)
    // For Debug and Control Process Emulation here is registered the ControlProcess Function
    //
    eResponseCode_CSMON_eSetTimerPeriodISRFunctionRegister = CSMON_eSetTimerPeriodISRFunctionRegister(ControlProcess);
    ASSERT(eResponseCode_CSMON_eSetTimerPeriodISRFunctionRegister != CSMON_RESPONSE_CODE_OK);
    // Check CSMON Response Code (... or Embed Assert For Debug) if needed

    //
    // Information For CSMON for the CPU Load
    //
    CSMON_eSetMaxTimeInISR(u16PeriodControl_usec - 10);  /* SetMaxTimeInISR in usec */
    CSMON_eSetMinGuaranteedTimeBetweenTwoISRs(10);       /* SetMinGuaranteedTimeBetweenTwoISRs in usec */


#ifdef _CS_1107_SCC_R01
    //
    // PERIPHERAL
    //
    PERIPHERAL_vInit();
#endif


    //
    // Reset the WatchDog counter
    //
    SysCtl_serviceWatchdog();



    //
    // Set the WatchDogPrescaler - Default SYSCTL_WD_PRESCALE_1
    //
    //SysCtl_setWatchdogPrescaler(SYSCTL_WD_PRESCALE_1);        /*  1 * 512 * 256 @ 10Mhz -> ~ 13ms */
    //SysCtl_setWatchdogPrescaler(SYSCTL_WD_PRESCALE_2);        /*  2 * 512 * 256 @ 10Mhz -> ~ 26ms */
    //SysCtl_setWatchdogPrescaler(SYSCTL_WD_PRESCALE_4);        /*  4 * 512 * 256 @ 10Mhz -> ~ 52ms */
    //SysCtl_setWatchdogPrescaler(SYSCTL_WD_PRESCALE_8);        /*  8 * 512 * 256 @ 10Mhz -> ~104ms */
    //SysCtl_setWatchdogPrescaler(SYSCTL_WD_PRESCALE_16);       /* 16 * 512 * 256 @ 10Mhz -> ~208ms */
    //SysCtl_setWatchdogPrescaler(SYSCTL_WD_PRESCALE_32);       /* 32 * 512 * 256 @ 10Mhz -> ~416ms */
    //SysCtl_setWatchdogPrescaler(SYSCTL_WD_PRESCALE_64);       /* 64 * 512 * 256 @ 10Mhz -> ~832ms */

    #if CSMON_AUTOMATIC_SERVICE_WATCHDOG_IN_MAIN_LOOP == 0

    SysCtl_setWatchdogPrescaler(SYSCTL_WD_PRESCALE_8);        /*  8 * 512 * 256 @ 10Mhz -> ~104ms */
    u16WatchdogPrescalerOld = u16WatchdogPrescaler = SYSCTL_WD_PRESCALE_8;
    vSetWatchdogPrescalerTimeDiv(u16WatchdogPrescaler);
    //
    // CSMON_eSetAutoServiceWatchdogInternalSlowCalculationsInMainLoop
    //
    //(void)CSMON_eSetAutoServiceWatchdogInternalSlowCalculationsInMainLoop(false);  /* Not Needed - Default false */


    #else

    SysCtl_setWatchdogPrescaler(SYSCTL_WD_PRESCALE_1);        /*  1 * 512 * 256 @ 10Mhz -> ~ 13ms */
    u16WatchdogPrescalerOld = u16WatchdogPrescaler = SYSCTL_WD_PRESCALE_1;
    vSetWatchdogPrescalerTimeDiv(u16WatchdogPrescaler);
    //
    // CSMON_eSetAutoServiceWatchdogInternalSlowCalculationsInMainLoop
    //
    (void)CSMON_eSetAutoServiceWatchdogInternalSlowCalculationsInMainLoop(true);

    #endif


    //
    // Register Write Parameter Element Function
    //
    (void)CSMON_eRegisterWriteParElementCallbackFunction(eWriteParElement);


    //
    // Enable the WatchDog
    //
    SysCtl_serviceWatchdog();
    SysCtl_enableWatchdog();


    bResetAllTimeMeasures = 0;
    u32TimeMainLoopProcessCSMON_Now_Ticks = 0;
    u32TimeMainLoopProcessCSMON_Max_Ticks = 0;
    u32TimeMainLoopCycle_Now_Ticks = 0;
    u32TimeMainLoopCycle_Max_Ticks = 0;

    u32TimeCSMON_ISR_Max_Ticks = 0;
    u32TimeCtrlLoopMax_Ticks = 0;
    u32TimeMainLoopCycle_Bgn_Ticks = u32TimeMainLoopCycle_End_Ticks = CPUTimer_getTimerCount(CPUTIMER1_BASE);


    //
    // Enable Global Interrupt (INTM) and RealTime interrupt (DBGM)
    //
    EINT;
    ERTM;

#define TEST_UART_SEND 0
#if TEST_UART_SEND
    uint32_t u32TestSendCounter = 0;
#endif

    for (;;)
    {
        //
        // Reset the WatchDog counter
        //
        SysCtl_serviceWatchdog();





        if (bResetAllTimeMeasures)
        {
            bResetAllTimeMeasures = 0;
            u32TimeMainLoopProcessCSMON_Now_Ticks = 0;
            u32TimeMainLoopProcessCSMON_Max_Ticks = 0;
            u32TimeMainLoopCycle_Now_Ticks = 0;
            u32TimeMainLoopCycle_Max_Ticks = 0;

            u32TimeCSMON_ISR_Max_Ticks = 0;
            u32TimeCtrlLoopMax_Ticks = 0;

        }


        //
        // Main Loop cycle Delay Measure
        //
        u32TimeMainLoopCycle_Bgn_Ticks = u32TimeMainLoopCycle_End_Ticks;
        u32TimeMainLoopCycle_End_Ticks = CPUTimer_getTimerCount(CPUTIMER1_BASE);
        u32TimeMainLoopCycle_Now_Ticks = u32TimeMainLoopCycle_Bgn_Ticks - u32TimeMainLoopCycle_End_Ticks;//down count
        if (u32TimeMainLoopCycle_Now_Ticks > u32TimeMainLoopCycle_Max_Ticks)
        {
            u32TimeMainLoopCycle_Max_Ticks = u32TimeMainLoopCycle_Now_Ticks;
        }



        //
        // Manage WatchDog Prescaler
        //
        if (u16WatchdogPrescalerTime != u16WatchdogPrescalerTimeOld)
        {
            u16WatchdogPrescalerTimeOld = u16WatchdogPrescalerTime;
            vGetWatchdogPrescaler(u16WatchdogPrescalerTime);
        }
        if (u16WatchdogPrescaler != u16WatchdogPrescalerOld)
        {
            //SysCtl_disableWatchdog();
            //SysCtl_serviceWatchdog();
            u16WatchdogPrescalerOld = u16WatchdogPrescaler;
            SysCtl_setWatchdogPrescaler((SysCtl_WDPrescaler)u16WatchdogPrescaler);
            vSetWatchdogPrescalerTimeDiv(u16WatchdogPrescaler);
            //SysCtl_serviceWatchdog();
            //SysCtl_enableWatchdog();
        }


        if (u16PeriodControlOld_usec != u16PeriodControl_usec)
        {
            u16PeriodControlOld_usec = u16PeriodControl_usec;
            CSMON_eSetInternalTimerPeriod(u16PeriodControl_usec);
            CSMON_eSetMaxTimeInISR(u16PeriodControl_usec - 10);  /* SetMaxTimeInISR in usec */
            CSMON_eSetMinGuaranteedTimeBetweenTwoISRs(10);       /* SetMinGuaranteedTimeBetweenTwoISRs in usec */
        }


        //
        // Artificial Delay Main Loop
        //
        SysCtl_delay(u32DelayMainLoop_Ticks);


        //
        // Artificial Delay Setup
        //
        if (u16DelayCtrlLoop_100nsec != u16DelayCtrlLoopOld_100nsec)
        {
            u16DelayCtrlLoopOld_100nsec = u16DelayCtrlLoop_100nsec;
            u32DelayCtrlLoop_Ticks = (uint32_t)(((((long double)(u16DelayCtrlLoop_100nsec) * 0.1L) / (1000000.0L / (long double)DEVICE_SYSCLK_FREQ)) - 9.0L) / 5.0L);
            if (u32DelayCtrlLoop_Ticks == 0)
            {
                u32DelayCtrlLoop_Ticks = 1;
            }
        }
        if (u16DelayMainLoop_usec != u16DelayMainLoopOld_usec)
        {
            u16DelayMainLoopOld_usec = u16DelayMainLoop_usec;
            u32DelayMainLoop_Ticks = (uint32_t)(((((long double)(u16DelayMainLoop_usec)) / (1000000.0L / (long double)DEVICE_SYSCLK_FREQ)) - 9.0L) / 5.0L);
            if (u32DelayMainLoop_Ticks == 0)
            {
                u32DelayMainLoop_Ticks = 1;
            }
        }
#if TEST_UART_SEND
        u32TestSendCounter++;
        if (u32TestSendCounter >= 100000)
        {
            u32TestSendCounter = 0;
            SCI_writeCharBlockingFIFO(SCIA_BASE, 0x55);
            SCI_writeCharBlockingFIFO(SCIA_BASE, 0x56);
            SCI_writeCharBlockingFIFO(SCIA_BASE, 0x57);
            SCI_writeCharBlockingFIFO(SCIA_BASE, 0x58);
        }
#endif





        //
        // CSMON Process In Main Loop Delay Measure
        //
#ifdef _CS_1211

#elif defined(_CS_1291)

#elif defined(_CS_1107_SCC_R01)

#else
        GPIO_writePin(STAT_LED_G_PIN, STAT_LED_ENABLE_LEVEL_LOW); /* Green LED (closest to the MCU Led) */
#endif
        u32TimeMainLoopProcessCSMON_Bgn_Ticks = CPUTimer_getTimerCount(CPUTIMER1_BASE);

        //
        // CSMON Process In Main Loop - Big Delays On Disconnect 4-5ms; On Connect 12-35ms If Not Interrupted (EMIF Checksum PC Application)
        //
        eResponseCode_CSMON_eProcess = CSMON_eProcess();
        // Check CSMON Response Code if needed
        ASSERT(eResponseCode_CSMON_eProcess != CSMON_RESPONSE_CODE_OK);

        //
        // CSMON Process In Main Loop Delay Measure
        //
#ifdef _CS_1211

#elif defined(_CS_1291)

#elif defined(_CS_1107_SCC_R01)

#else
        GPIO_writePin(STAT_LED_G_PIN, STAT_LED_DISABLE_LVL_HIGH); /* Green LED (closest to the MCU Led) */
#endif
        u32TimeMainLoopProcessCSMON_End_Ticks = CPUTimer_getTimerCount(CPUTIMER1_BASE);
        u32TimeMainLoopProcessCSMON_Now_Ticks = 0 - (u32TimeMainLoopProcessCSMON_End_Ticks - u32TimeMainLoopProcessCSMON_Bgn_Ticks);//down count
        if (u32TimeMainLoopProcessCSMON_Now_Ticks > u32TimeMainLoopProcessCSMON_Max_Ticks)
        {
            u32TimeMainLoopProcessCSMON_Max_Ticks = u32TimeMainLoopProcessCSMON_Now_Ticks;
        }

        //
        // Device Running Control Indication - Set on Enter/Exit Run Mode
        //
        eResponseCode_CSMON_eSetServerOnStatus = CSMON_eSetServerOnStatus(bDummyStatsDevRunning);
        // Check CSMON Response Code if needed
        ASSERT(eResponseCode_CSMON_eSetServerOnStatus != CSMON_RESPONSE_CODE_OK);


        //
        // RTC Process
        //
#ifdef _CS_1211

#elif defined(_CS_1291)

        //TBD!!!

#elif defined(_CS_1107_SCC_R01)

        if (MAIN_bDateTimeSet)
        {
            FPGA_DRV_vSetDateTime (
                    MAIN_sDateTimeSet.u8Seconds,
                    MAIN_sDateTimeSet.u8Minutes,
                    MAIN_sDateTimeSet.u8Hours,
                    MAIN_sDateTimeSet.u8Weekdays,
                    MAIN_sDateTimeSet.u8Day,
                    MAIN_sDateTimeSet.u8Month,
                    MAIN_sDateTimeSet.u8Year);
            MAIN_bDateTimeSet = false;
        }

        FPGA_DRV_vGetDateTime (
                &MAIN_sDateTimeGet.u8Seconds,
                &MAIN_sDateTimeGet.u8Minutes,
                &MAIN_sDateTimeGet.u8Hours,
                &MAIN_sDateTimeGet.u8Weekdays,
                &MAIN_sDateTimeGet.u8Day,
                &MAIN_sDateTimeGet.u8Month,
                &MAIN_sDateTimeGet.u8Year);


#else
        //
        // RTC Process To From FPGA
        //
        if (MAIN_bDateTimeSet)
        {
            FPGA_SCI_DRV_vSetDateTime (
                    MAIN_sDateTimeSet.u8Seconds,
                    MAIN_sDateTimeSet.u8Minutes,
                    MAIN_sDateTimeSet.u8Hours,
                    MAIN_sDateTimeSet.u8Weekdays,
                    MAIN_sDateTimeSet.u8Day,
                    MAIN_sDateTimeSet.u8Month,
                    MAIN_sDateTimeSet.u8Year);
            FPGA_SCI_DRV_vClockSetup();
            MAIN_bDateTimeSet = false;
        }

        FPGA_SCI_DRV_vGetDateTime (
                &MAIN_sDateTimeGet.u8Seconds,
                &MAIN_sDateTimeGet.u8Minutes,
                &MAIN_sDateTimeGet.u8Hours,
                &MAIN_sDateTimeGet.u8Weekdays,
                &MAIN_sDateTimeGet.u8Day,
                &MAIN_sDateTimeGet.u8Month,
                &MAIN_sDateTimeGet.u8Year);


        //
        // FPGA Communication Process
        //
        SCI_DRV_vErrorResetProcess(SCI_DRV_u32GetBaseFromModuleIndex((SCI_DRV_eModule_t)FPGA_SCI_DRV_UART_MODULE));
        FPGA_SCI_DRV_vProcessUartRx();
        FPGA_SCI_DRV_vProcess();
#endif


#ifdef _CS_1107_SCC_R01
        //
        // PERIPHERAL
        //
        PERIPHERAL_vProcess();
#endif

    }

}



