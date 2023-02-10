/* *****************************************************************************
 * File:   csmon_config.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 08 14 12:51
 *
 * Generated on $genTime$
 * Generator version: $genVer$
 * Generator template: $genTemplate$
 *
 * Description: ...
 * 
 **************************************************************************** */


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


#define INIT_PARAMFULL(u16ID, paramAccess, $funcWr$, $funcRd$, $accessWr$, $accessRd$, $bStorable$, u8Offs, u8Elements, DataTypeAttribute, u32Address, $strName$, $strUnit$, $u32Maximum$, $u32Minimum$, $u32Default$, $floatScale$, VisualAttribute, bitField, arrayType, bNR, bNW, pFunc) \
 { \
    .u16ParameterIndexID = u16ID,                       /* u16ID */\
    /* .u16ParamAttributes = (PAR(paramType, paramAccess, paramPass)),                u16Attributes */\
    .uBitsCount.sBitsCount.u8BitsCount = BITS_COUNT(DataTypeAttribute),      /* uBitsCount.sBitsCount.u8BitsCount; */\
    .uBitsCount.sBitsCount.u8PaddingByte = (0),         /* uBitsCount.sBitsCount.u8PaddingByte; */\
    .u16RealAddress = (uint16_t)((uint32_t)((uint32_t*)(u32Address))),           /* u32Address */\
    .$au8NameUnit$ = ZIP_STRING(strName "\t" strUnit),    /* au8NameUnit */\
    .eDataTypeAttribute = CSMON_DATA_TYPE(DataTypeAttribute),               /* eDataTypeAttribute */\
    /* .eVisualTypeAttribute = VisualAttribute,               eVisualTypeAttribute */\
    .$u32Max$.FIELD_TYPE(DataTypeAttribute) = (u32Maximum),                       /* u32Maximum */\
    .$u32Min$.FIELD_TYPE(DataTypeAttribute) = (u32Minimum),                       /* u32Minimum */\
    .$u32Def$.FIELD_TYPE(DataTypeAttribute) = (u32Default),                       /* u32Default */\
    /* .u32Offset.eType = 0,                       u32Offset */\
    .$Norm$ = floatScale,                                 /* floatScale */\
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
    .$uFunctionCode$.sFunctionCode.eDefaultWriteFunctionCode = funcWr, \
    .$uFunctionCode$.sFunctionCode.eDefaultReadFunctionCode = funcRd, \
    .$uAccessLevel$.sAccessLevel.eWriteAccessLevelMin = accessWr, \
    .$uAccessLevel$.sAccessLevel.eReadAccessLevelMin = accessRd, \
    .$uAccessLevel$.sAccessLevel.bStore = bStorable, \
 }

#define INIT_PARAMETER(u16ID, paramAccess, $funcWr$, $funcRd$, $accessWr$, $accessRd$, $bStorable$, u8Offs, u8Elements, DataTypeAttribute, u32Address, $strName$, $strUnit$, $u32Maximum$, $u32Minimum$, $u32Default$, $floatScale$) \
        INIT_PARAMFULL(u16ID, paramAccess, $funcWr$, $funcRd$, $accessWr$, $accessRd$, $bStorable$, u8Offs, u8Elements, DataTypeAttribute, u32Address, $strName$, $strUnit$, $u32Maximum$, $u32Minimum$, $u32Default$, $floatScale$, CSMON_VISUAL_TYPE_HEX, 0, 0, 0, 0, NULL)

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */

#pragma diag_push
#pragma diag_suppress 70
/* XXXXX warning generator */

volatile const MAIN_sParameterList_t asParameterList[CSMON_CONFIG_PARAMETER_COUNT_MAX] =
{
$paramsList$
};

#pragma diag_pop

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */














#endif /* CSMON_CONFIG */

