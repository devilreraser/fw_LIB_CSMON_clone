/* *****************************************************************************
 * File:   csmon_config.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 08 14 12:51
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
#include "csmon_config.h"


#if CSMON_CONFIG

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */
//Notes to do:
//
//  CSMON_REALADR_16BIT - 16-bit or 32-bit Real Address
//
//

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */
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

#define STRING_CONCAT_BASE(_a_, _b_) _a_##_b_
#define STRING_CONCAT(_a_, _b_) STRING_CONCAT_BASE(_a_, _b_)

//#define PAR(eType,eAccess,ePass) (uint16_t)(STRING_CONCAT(PAR_TYPE,eType) | (STRING_CONCAT(PAR_ACCESS,eAccess) << PAR_ACCESS_SHIFT) | (STRING_CONCAT(PAR_PASSRW,ePass) << PAR_PASSRW_SHIFT))

#define CSMON_DATA_TYPE(DataTypeAttribute) STRING_CONCAT(CSMON_DATA_TYPE_,DataTypeAttribute)  /* U8, S8, U16, S16, U32, S32 */

//#define xstr(s) str(s)
//#define str(s) #s
//#define GET_CHAR(string, pos) string[pos]

//#define STRING_CONCAT_INDIRECT(token_1, token_2) STRING_CONCAT(token_1, token_2)


#define FIELD_TYPE(DataTypeAttribute) STRING_CONCAT(reg, DataTypeAttribute)
#define BYTES_COUNT(DataTypeAttribute) (1 << (CSMON_DATA_TYPE(DataTypeAttribute) & 0x3))
#define BITS_COUNT(DataTypeAttribute) (BYTES_COUNT(DataTypeAttribute) << 3)


#define INIT_PARAMFULL(u16ID, paramAccess, funcWr, funcRd, accessWr, accessRd, bStorable, u8Offs, DataTypeAttribute, u32Address, strName, strUnit, u32Maximum, u32Minimum, u32Default, floatScale, VisualAttribute, u8Elements, bitField, arrayType, bNR, bNW, pFunc) \
 { \
    .u16ParameterIndexID = u16ID,                       /* u16ID */\
    /* .u16ParamAttributes = (PAR(paramType, paramAccess, paramPass)),                u16Attributes */\
    .uBitsCount.sBitsCount.u8BitsCount = BITS_COUNT(DataTypeAttribute),      /* uBitsCount.sBitsCount.u8BitsCount; */\
    .uBitsCount.sBitsCount.u8PaddingByte = (0),         /* uBitsCount.sBitsCount.u8PaddingByte; */\
    .u16RealAddress = (uint16_t)((uint32_t)((uint32_t*)(u32Address))),           /* u32Address */\
    .au8NameUnit = ZIP_STRING(strName "\t" strUnit),    /* au8NameUnit */\
    .eDataTypeAttribute = CSMON_DATA_TYPE(DataTypeAttribute),               /* eDataTypeAttribute */\
    /* .eVisualTypeAttribute = VisualAttribute,               eVisualTypeAttribute */\
    .u32Max.FIELD_TYPE(DataTypeAttribute) = (u32Maximum),                       /* u32Maximum */\
    .u32Min.FIELD_TYPE(DataTypeAttribute) = (u32Minimum),                       /* u32Minimum */\
    .u32Def.FIELD_TYPE(DataTypeAttribute) = (u32Default),                       /* u32Default */\
    /* .u32Offset.eType = 0,                       u32Offset */\
    .Norm = floatScale,                                 /* floatScale */\
    .uParameterSize.sSize.u8SizeElement = BYTES_COUNT(DataTypeAttribute), /*  */\
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

#define INIT_PARAMETER(u16ID, paramAccess, funcWr, funcRd, accessWr, accessRd, bStorable, u8Offs, DataTypeAttribute, u32Address, strName, strUnit, u32Maximum, u32Minimum, u32Default, floatScale) \
        INIT_PARAMFULL(u16ID, paramAccess, funcWr, funcRd, accessWr, accessRd, bStorable, u8Offs, DataTypeAttribute, u32Address, strName, strUnit, u32Maximum, u32Minimum, u32Default, floatScale, CSMON_VISUAL_TYPE_HEX, 1, 0, 0, 0, 0, NULL)


/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */

extern uint16_t u16PeriodControl_usec;
extern uint16_t u16DelayCtrlLoop_100nsec;
extern uint16_t u16DelayMainLoop_usec;
extern uint16_t u16CountSetParameterFail;
extern uint32_t u32TimeMainLoopProcessCSMON_Max_Ticks;
extern uint32_t u32TimeCSMON_ISR_Max_Ticks;
extern uint32_t u32TimeMainLoopCycle_Max_Ticks;
extern uint32_t u32TimeCtrlLoopMax_Ticks;
extern uint32_t u32ParamTime_Ticks;
extern bool bResetAllTimeMeasures;
extern uint16_t u16DummyDataCnt;
extern bool bDummyStatsDevRunning;
extern bool bDummyReqstDevRunning;
extern int16_t s16ScaleDataTst;
extern int16_t s16DummyIncrementLoop;
extern int16_t s16DummyCurrentPhaseA;
extern int16_t s16DummyCurrentPhaseB;
extern int16_t s16DummyCurrentPhaseC;
extern uint32_t u32GetBaudError_PPM;
extern uint16_t u16WatchdogPrescalerTime;
extern int16_t s16DummyVoltageDCLink;
extern int32_t s32DummyVoltageDCLink;
extern int32_t s32DummyCurrentPhaseA;
extern int32_t s32DummyCurrentPhaseB;
extern int32_t s32DummyCurrentPhaseC;
extern int16_t s16DummyVoltageDCLinkStartup;
extern int16_t s16DummyCurrentPhaseAStartup;
extern int16_t s16DummyCurrentPhaseBStartup;
extern int16_t s16DummyCurrentPhaseCStartup;
extern int16_t s16DummyVoltageDCLinkStop;
extern int16_t s16DummyCurrentPhaseAStop;
extern int16_t s16DummyCurrentPhaseBStop;
extern int16_t s16DummyCurrentPhaseCStop;
extern int16_t s16DummyVoltageDCLinkIdle;
extern int16_t s16DummyCurrentPhaseAIdle;
extern int16_t s16DummyCurrentPhaseBIdle;
extern int16_t s16DummyCurrentPhaseCIdle;
extern int16_t s16DummyIncrementLoopA;
extern int16_t s16DummyIncrementLoopB;
extern int16_t s16DummyIncrementLoopC;
extern int16_t s16DummyIncrementLoopV;
extern int16_t s16DummyIncrementLoopCDiff;


volatile const MAIN_sParameterList_t asParameterList[CSMON_CONFIG_PARAMETER_COUNT_MAX] =
{
/*                ID,        Attrib,   fucnWr, funcRd, accessWr, accessRd, bStorable, Offs,     CSMON_DATA_TYPE,  MCU Address                             Name               Unit,              Max,         Min,   Def,    Norm*/
INIT_PARAMETER(60000,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 U16,  &u16PeriodControl_usec,                 "CtrlLoopPeriod",   "usec",         65535,           0,     0,    1.0F),
INIT_PARAMETER(60001,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 U16,  &u16DelayCtrlLoop_100nsec,              "CtrlLoopAddDelay", "usec",         65535,           0,     0,    0.100000001490116F),
INIT_PARAMETER(60002,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 U16,  &u16DelayMainLoop_usec,                 "MainLoopAddDelay", "usec",         65535,           0,     0,    1.0F),
INIT_PARAMETER(60003,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 U16,  &u16CountSetParameterFail,              "Parameters_Fail",  "unit",         65535,           0,     0,    1.0F),
INIT_PARAMETER(60004,      CSMON_RO,     0x00,   0x03,        4,        1,         1,    0,                 U32,  &u32TimeMainLoopProcessCSMON_Max_Ticks, "MainLoopCSMON",    "usec",    4294967295,           0,     0,    1.0F),
INIT_PARAMETER(60005,      CSMON_RO,     0x00,   0x03,        4,        1,         1,    0,                 U32,  &u32TimeCSMON_ISR_Max_Ticks,            "CtrlLoopCSMON",    "usec",    4294967295,           0,     0,    1.0F),
INIT_PARAMETER(60006,      CSMON_RO,     0x00,   0x03,        4,        1,         1,    0,                 U32,  &u32TimeMainLoopCycle_Max_Ticks,        "MainLoopMeasMax",  "usec",    4294967295,           0,     0,    1.0F),
INIT_PARAMETER(60007,      CSMON_RO,     0x00,   0x03,        4,        1,         1,    0,                 U32,  &u32TimeCtrlLoopMax_Ticks,              "CtrlLoopMeasMax",  "usec",    4294967295,           0,     0,    1.0F),
INIT_PARAMETER(60008,      CSMON_RO,     0x00,   0x03,        4,        1,         1,    0,                 U32,  &u32ParamTime_Ticks,                    "ParamInitMeas",    "usec",    4294967295,           0,     0,    1.0F),
INIT_PARAMETER(60009,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                  U8,  &bResetAllTimeMeasures,                 "MeasuresReset",    "boolean",          1,           0,     0,    1.0F),
INIT_PARAMETER(65534,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 U16,  &u16DummyDataCnt,                       "ModbusMsgCntr",    "unit",         65535,           0,     0,    0.0F),
INIT_PARAMETER(    0,      CSMON_RO,     0x10,   0x03,        4,        1,         1,    0,                  U8,  &bDummyStatsDevRunning,                 "DeviceRunning",    "boolean",          1,           0,     0,    1.0F),
INIT_PARAMETER(    0,      CSMON_WO,     0x10,   0x03,        4,        1,         1,    0,                  U8,  &bDummyReqstDevRunning,                 "DeviceRunning",    "boolean",          1,           0,     0,    1.0F),
INIT_PARAMETER(    1,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S16,  &s16ScaleDataTst,                       "ReadWriteScale",   "*3.1415",      10000,      -10000,     0,    3.1414999961853F),
INIT_PARAMETER(    2,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S16,  &s16DummyIncrementLoop,                 "IncLoopTest",      "A(0.5V)",      10000,      -10000,     0,    1.0F),
INIT_PARAMETER(    3,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S16,  &s16DummyCurrentPhaseA,                 "CurrentPhA_0",     "A",            10000,      -10000,     0,    1.0F),
INIT_PARAMETER(    4,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S16,  &s16DummyCurrentPhaseB,                 "CurrentPhB_0",     "A",            10000,      -10000,     0,    1.0F),
INIT_PARAMETER(    5,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S16,  &s16DummyCurrentPhaseC,                 "CurrentPhC_0",     "A",            10000,      -10000,     0,    1.0F),
INIT_PARAMETER(    6,      CSMON_RO,     0x00,   0x03,        4,        1,         1,    0,                 U32,  &u32GetBaudError_PPM,                   "BaudError",        "%",            10000,           0,     0,    9.99999974737875E-05F),
INIT_PARAMETER(    7,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 U16,  &u16WatchdogPrescalerTime,              "WatchdogTime",     "msec",            64,           1,     0,    13.1071996688843F),
INIT_PARAMETER(    8,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S16,  &s16DummyVoltageDCLink,                 "VoltageBus",       "V",            10000,      -10000,     0,    1.0F),
INIT_PARAMETER(    9,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S16,  &s16DummyCurrentPhaseA,                 "CurrentPhA",       "A",            10000,      -10000,     0,    1.0F),
INIT_PARAMETER(   10,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S16,  &s16DummyCurrentPhaseB,                 "CurrentPhB",       "A",            10000,      -10000,     0,    1.0F),
INIT_PARAMETER(   11,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S16,  &s16DummyCurrentPhaseC,                 "CurrentPhC",       "A",            10000,      -10000,     0,    1.0F),
INIT_PARAMETER(   28,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S32,  &s32DummyVoltageDCLink,                 "VoltageBus32",     "V",       1000000000, -1000000000,     0,    9.99999997475243E-07F),
INIT_PARAMETER(   29,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S32,  &s32DummyCurrentPhaseA,                 "CurrentPhA32",     "A",       1000000000, -1000000000,     0,    9.99999997475243E-07F),
INIT_PARAMETER(   30,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S32,  &s32DummyCurrentPhaseB,                 "CurrentPhB32",     "A",       1000000000, -1000000000,     0,    9.99999997475243E-07F),
INIT_PARAMETER(   31,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S32,  &s32DummyCurrentPhaseC,                 "CurrentPhC32",     "A",       1000000000, -1000000000,     0,    9.99999997475243E-07F),
INIT_PARAMETER(  108,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S16,  &s16DummyVoltageDCLinkStartup,          "VoltageBusStart",  "V",            10000,      -10000,     0,    1.0F),
INIT_PARAMETER(  109,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S16,  &s16DummyCurrentPhaseAStartup,          "CurrentPhAStart",  "A",            10000,      -10000,     0,    1.0F),
INIT_PARAMETER(  110,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S16,  &s16DummyCurrentPhaseBStartup,          "CurrentPhBStart",  "A",            10000,      -10000,     0,    1.0F),
INIT_PARAMETER(  111,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S16,  &s16DummyCurrentPhaseCStartup,          "CurrentPhCStart",  "A",            10000,      -10000,     0,    1.0F),
INIT_PARAMETER(  208,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S16,  &s16DummyVoltageDCLinkStop,             "VoltageBusStop",   "V",            10000,      -10000,     0,    1.0F),
INIT_PARAMETER(  209,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S16,  &s16DummyCurrentPhaseAStop,             "CurrentPhAStop",   "A",            10000,      -10000,     0,    1.0F),
INIT_PARAMETER(  210,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S16,  &s16DummyCurrentPhaseBStop,             "CurrentPhBStop",   "A",            10000,      -10000,     0,    1.0F),
INIT_PARAMETER(  211,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S16,  &s16DummyCurrentPhaseCStop,             "CurrentPhCStop",   "A",            10000,      -10000,     0,    1.0F),
INIT_PARAMETER(  308,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S16,  &s16DummyVoltageDCLinkIdle,             "VoltageBusIdle",   "V",            10000,      -10000,     0,    1.0F),
INIT_PARAMETER(  309,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S16,  &s16DummyCurrentPhaseAIdle,             "CurrentPhAIdle",   "A",            10000,      -10000,     0,    1.0F),
INIT_PARAMETER(  310,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S16,  &s16DummyCurrentPhaseBIdle,             "CurrentPhBIdle",   "A",            10000,      -10000,     0,    1.0F),
INIT_PARAMETER(  311,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S16,  &s16DummyCurrentPhaseCIdle,             "CurrentPhCIdle",   "A",            10000,      -10000,     0,    1.0F),
INIT_PARAMETER(  408,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S16,  &s16DummyIncrementLoopA,                "IncLoopTestA",     "A",            10000,      -10000,     0,    1.0F),
INIT_PARAMETER(  409,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S16,  &s16DummyIncrementLoopB,                "IncLoopTestB",     "A",            10000,      -10000,     0,    1.0F),
INIT_PARAMETER(  410,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S16,  &s16DummyIncrementLoopC,                "IncLoopTestC",     "A",            10000,      -10000,     0,    1.0F),
INIT_PARAMETER(  411,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S16,  &s16DummyIncrementLoopV,                "IncLoopTestV",     "V",            10000,      -10000,     0,    1.0F),
INIT_PARAMETER(  412,      CSMON_RW,     0x10,   0x03,        4,        1,         1,    0,                 S16,  &s16DummyIncrementLoopCDiff,            "IncLoopTestCDiff", "A",            10000,      -10000,     0,    1.0F),

};
/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */














#endif /* CSMON_CONFIG */

