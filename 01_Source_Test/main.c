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

#if _CSMON_USE_EXTERNAL_PARAMETER_LIST
    #include "csmon_config.h"
#endif

#if defined(__TMS320F2806x__)
    #include "boardcfg.h"
    #include "sci_driver.h"
    #include "uart_driver.h"
#else

    #ifdef _CS_1211
    #define BOARDCFG_CSMON_FILE_PARAMETER_COUNT_MAX 200
    #else
    #define BOARDCFG_CSMON_FILE_PARAMETER_COUNT_MAX 1024
    #endif

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

#endif  //defined(__TMS320F2806x__)

#include "csmon.h"
#include "parameter.h"
#include "datetime.h"

#ifdef _CS_1107_SCC_R01
#include "peripheral.h"
#endif

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

#ifndef _C2000WARE_VERSION_LESS_THAN_4_3

#ifndef GPIO_setMasterCore
#define GPIO_setMasterCore GPIO_setControllerCore
#endif

#endif



#define CSMON_AUTOMATIC_SERVICE_WATCHDOG_IN_MAIN_LOOP   1


#if defined(__TMS320F2806x__)

#define RECORDER0_ONLY_TEST                 1
#define RECORDER0_TOTAL_SAMPLE_COUNT        64
#define RECORDER0_PRETRIGGER_SAMPLE_COUNT   (RECORDER0_TOTAL_SAMPLE_COUNT-8)
#define RECORDER_COUNT                      1

#else


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
#ifdef _CS_1211

//    #define LED1_PIN     16
//    #define LED2_PIN     17
//    #define LED3_PIN     18
//    #define LED4_PIN     19
    #define STAT_LED_G_PIN      16      /* Green LED (closest to the MCU Led) */
    #define STAT_LED_A_B_PIN    17      /* Amber LED (middle Led) */
    #define STAT_LED_R_PIN      18      /* Red LED (closest to the Debug Header) */
    #define CLK_EN_FPGA_PIN     19

#elif __TMS320F2806x__

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

 uint32_t au32NumberArray[10];
 uint16_t au16NumberArray[10];
 uint_least8_t au8NumberArray[10];



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


#if CSMON_CONFIG == 0


#if _CSMON_USE_EXTERNAL_PARAMETER_TABLE


#include "parameter_table.h"



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

uint16_t limit_count_params = 1500;
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

#if _CSMON_USE_EXTERNAL_PARAMETER_TABLE
//void Parameter132IncreaseValue( void )
//{
//    int parIndex;
//    int nParams = GetNumberOfParams();
//
//    for ( parIndex = 0; parIndex < nParams; parIndex++ )
//    {
//        if ( ParTable[parIndex].index == 132 )
//        {
//            break;
//        }
//    }
//
//    if ( parIndex < nParams )
//    {
//        ((PAR_DES_I16_T*)(ParTable[parIndex].ParDes))->pVal[0]++;
//    }
//}



uint16_t indexTable[BOARDCFG_CSMON_FILE_PARAMETER_COUNT_MAX];

//typedef const struct
//{
//    uint16_t index;                       //!< El. 0, ParameterNr.
//    void* ParDes;                       //!< Zeiger auf Beschreibungsstruktur
//} PARAMETER_TABLE_ENTRY_T;
//PARAMETER_TABLE_ENTRY_T ParTable

int paramIndex;
uint16_t newIndex = 0;
int dummyIndex = 0;
uint16_t nParams;

void IndexTableInitialization( void )
{
//    int parIndex;
//    int newIndex = 0;
//    int nParams = GetNumberOfParams();
//    nParams = BOARDCFG_CSMON_FILE_PARAMETER_COUNT_MAX;
    nParams = GetNumberOfParams();

    for ( paramIndex = 0; paramIndex < BOARDCFG_CSMON_FILE_PARAMETER_COUNT_MAX; paramIndex++ )
    {
        indexTable[paramIndex] = 65535;
    }

    for ( paramIndex = 0; paramIndex < nParams; paramIndex++ )
    {
        newIndex = ParTable[paramIndex].index;
        if ( newIndex <= nParams )
        {
            indexTable[newIndex] = paramIndex;
        }
    }
}


void SetParameterDummyValue( uint16_t parameter, INT16 value )
{
    uint16_t parIndex;

    parIndex = indexTable[parameter];

    if ( parIndex < nParams )
    {
        ((PAR_DES_I16_T*)(ParTable[parIndex].ParDes))->pVal[0] = value;
    }
}

//void SetParameterDummyValue( uint16_t parameter, INT16 value )
//{
//    int parIndex;
//    int nParams = GetNumberOfParams();
//
//    for ( parIndex = 0; parIndex < nParams; parIndex++ )
//    {
//        if ( ParTable[parIndex].index == parameter )
//        {
//            break;
//        }
//    }
//
//    if ( parIndex < nParams )
//    {
//        ((PAR_DES_I16_T*)(ParTable[parIndex].ParDes))->pVal[0] = value;
//    }
//}


INT16 GetParameterValue( uint16_t parameter )
{
    INT16 value;
    int parIndex;
    int nParams = GetNumberOfParams();

    for ( parIndex = 0; parIndex < nParams; parIndex++ )
    {
        if ( ParTable[parIndex].index == parameter )
        {
            break;
        }
    }

    if ( parIndex < nParams )
    {
        value = ((PAR_DES_I16_T*)(ParTable[parIndex].ParDes))->pVal[0];
    }
    return value;
}

INT16 v = 9;
#endif


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

#if _CSMON_USE_EXTERNAL_PARAMETER_TABLE
//    typedef const struct
//    {
//      UINT16    Attr;                                   //!< El. 1, Attribute des Parameters
//      UINT8*    Name;                                   //!< El. 2, Name
//      UINT8*    Unit;                                   //!< El. 3, Einheit
//      INT16     Max;                                    //!< El. 4, Maximalwert
//      INT16     Min;                                    //!< El. 5, Minimalwert
//      INT16     Def;                                    //!< El. 6, Defaultwert
//      float     Norm;                                   //!< El. 7, Normierungsfaktor nur f�r Darstellung auf Anzeigeger�t
//      INT16*    pVal;                                   //!< El. 8, Zeiger auf Wert
//      RC_PAR_T (*pfFct) (INT16);        //!< Zeiger auf spezielle Behandlungsfunktion
//    } PAR_DES_I16_T;

//    Parameter132IncreaseValue();
    v++;
    GPIO_writePin( STAT_LED_A_B_PIN, STAT_LED_ENABLE_LEVEL_LOW );
    SetParameterDummyValue( 131, v );
    GPIO_writePin( STAT_LED_A_B_PIN, STAT_LED_DISABLE_LVL_HIGH );  //13us

//    SetParameterDummyValue( 132, v );
//    SetParameterDummyValue( 133, v );
//    SetParameterDummyValue( 134, v );
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

#ifndef CSMON_CONFIG_PARAMETER_COUNT_MAX
#ifdef BOARDCFG_CSMON_FILE_PARAMETER_COUNT_MAX
//#define CSMON_CONFIG_PARAMETER_COUNT_MAX BOARDCFG_CSMON_FILE_PARAMETER_COUNT_MAX
#define CSMON_CONFIG_PARAMETER_COUNT_MAX (uint16_t)(sizeof(asParameterList)/sizeof(asParameterList[0]))
#else
#define CSMON_CONFIG_PARAMETER_COUNT_MAX 0
#endif
#endif

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

#ifdef _CS_1309
    #define POWER_SAVE_MODE_PIN  97
    uint16_t u16_POWER_SAVE_MODE = 1;

    GPIO_setPinConfigOutput( POWER_SAVE_MODE_PIN );
    GPIO_writePin( POWER_SAVE_MODE_PIN, u16_POWER_SAVE_MODE );
#endif

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
    count_params = limit_count_params;
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

    IndexTableInitialization();

#elif _CSMON_USE_EXTERNAL_PARAMETER_LIST
    ParameterInitialization();
    ExternalParametersInitialization();
    /* Recorder And Scope Initialization Made Once after parameter initialized */
    RecordersInitialization();
    ScopesInitialization();

#else
    ParameterInitialization();
    /* Recorder And Scope Initialization Made Once after parameter initialized */
    RecordersInitialization();
    ScopesInitialization();

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
    //SysCtl_disableWatchdog();


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



