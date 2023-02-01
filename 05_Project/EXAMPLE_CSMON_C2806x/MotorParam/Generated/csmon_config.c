/* *****************************************************************************
 * File:   csmon_config.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 08 14 12:51
 *
 * Generated on 02/01/2023 14:23:05 UTC
 * Generator version: 1.09
 * Generator template: C:\repo_github\fw_BasicAFE\02_Libraries\LIB_CSMON\05_Project\EXAMPLE_CSMON_C2806x\MotorParam\Template\
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

#define CSMON_DATA_TYPE(DataTypeAttribute) STRING_CONCAT(CSMON_DATA_TYPE_,DataTypeAttribute)  /* U8, S8, U16, S16, U32, S32, AU8 */

//#define xstr(s) str(s)
//#define str(s) #s
//#define GET_CHAR(string, pos) string[pos]

//#define STRING_CONCAT_INDIRECT(token_1, token_2) STRING_CONCAT(token_1, token_2)


#define FIELD_TYPE(DataTypeAttribute) STRING_CONCAT(reg, DataTypeAttribute)
#define BYTES_COUNT_HMMODBUS(DataTypeAttribute) ((1 << (CSMON_DATA_TYPE(DataTypeAttribute) & 0x3)) * (CSMON_DATA_TYPE(DataTypeAttribute) != CSMON_DATA_TYPE_CHAR))
#define BYTES_COUNT(DataTypeAttribute) (1 << (CSMON_DATA_TYPE(DataTypeAttribute) & 0x3))
#define BITS_COUNT(DataTypeAttribute) (BYTES_COUNT(DataTypeAttribute) << 3)


#define INIT_PARAMFULL(u16ID, paramAccess, funcWr, funcRd, accessWr, accessRd, bStorable, u8Offs, u8Elements, DataTypeAttribute, u32Address, strName, strUnit, u32Maximum, u32Minimum, u32Default, floatScale, VisualAttribute, bitField, arrayType, bNR, bNW, pFunc) \
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
    /* .uParameterSize.sSize.u8SizeElement = (u8Elements > 1) * 0 + (u8Elements == 1) * BYTES_COUNT_HMMODBUS(DataTypeAttribute), String Encoded Normal (u8Elements > 1) u8SizeElement = 0 */\
    .uParameterSize.sSize.u8SizeElement = BYTES_COUNT_HMMODBUS(DataTypeAttribute), /* String Encoded also by using the high part of the 16-bit word (u8Elements > 1) u8SizeElement = 1 */\
    .uParameterSize.sSize.u8CountElements = u8Elements,\
    .uParameterFlags.sFlags.u8BitOffset = (u8Elements > 1) * u8Elements + (u8Elements == 1) * u8Offs, \
    .uParameterFlags.sFlags.bBitField = bitField, \
    .uParameterFlags.sFlags.bArray = (u8Elements > 1),         /* Array Type Register */ \
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

#define INIT_PARAMETER(u16ID, paramAccess, funcWr, funcRd, accessWr, accessRd, bStorable, u8Offs, u8Elements, DataTypeAttribute, u32Address, strName, strUnit, u32Maximum, u32Minimum, u32Default, floatScale) \
        INIT_PARAMFULL(u16ID, paramAccess, funcWr, funcRd, accessWr, accessRd, bStorable, u8Offs, u8Elements, DataTypeAttribute, u32Address, strName, strUnit, u32Maximum, u32Minimum, u32Default, floatScale, CSMON_VISUAL_TYPE_HEX, 0, 0, 0, 0, NULL)

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */

extern char UserStringConfiguration[16];
extern char UserZippedString[16];
extern char UserUnicodeString[16];

#pragma diag_push
#pragma diag_suppress 70
/* XXXXX warning generator */

volatile const MAIN_sParameterList_t asParameterList[CSMON_CONFIG_PARAMETER_COUNT_MAX] =
{
/*                ID,        Attrib,   fucnWr, funcRd, accessWr, accessRd, bStorable, Offs, Elements, CSMON_DATA_TYPE,  MCU Address,              Name,                   Unit,        Max,         Min,   Def,    Norm*/
INIT_PARAMETER(   35,      CSMON_RW,     0x10,   0x03,        0,        0,         1,    0,       16,            CHAR,  &UserStringConfiguration, "ConfigNameNormalReg",  "",        255,           0,     0,    1.0F),
INIT_PARAMETER(   36,      CSMON_RW,     0x10,   0x03,        0,        0,         1,    0,       16,             AU8,  &UserZippedString,        "ConfigNameZippedReg",  "",        255,           0,     0,    1.0F),
INIT_PARAMETER(   37,      CSMON_RW,     0x10,   0x03,        0,        0,         1,    0,       16,            AU16,  &UserUnicodeString,       "ConfigNameUnicodeReg", "",      65535,           0,     0,    1.0F),
INIT_PARAMETER(   32,      CSMON_RW,     0x15,   0x14,        0,        0,         1,    0,       16,            CHAR,  &UserStringConfiguration, "ConfigNameNormal",     "",        255,           0,     0,    1.0F),
INIT_PARAMETER(   33,      CSMON_RW,     0x15,   0x14,        0,        0,         1,    0,       16,             AU8,  &UserZippedString,        "ConfigNameZipped",     "",        255,           0,     0,    1.0F),
INIT_PARAMETER(   34,      CSMON_RW,     0x15,   0x14,        0,        0,         1,    0,       16,            AU16,  &UserUnicodeString,       "ConfigNameUnicode",    "",      65535,           0,     0,    1.0F),

};

#pragma diag_pop

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */














#endif /* CSMON_CONFIG */

