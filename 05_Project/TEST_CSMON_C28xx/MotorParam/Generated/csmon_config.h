/* *****************************************************************************
 * File:   csmon_config.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 08 14 12:51
 *
 * Generated on 01/23/2023 16:45:22 UTC
 * Generator version: 1.06
 * Generator template: C:\repo_github\fw_BasicAFE\02_Libraries\LIB_CSMON\05_Project\TEST_CSMON_C28xx\MotorParam\Template\
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
#ifndef CSMON_CONFIG_H
#define	CSMON_CONFIG_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "csmon.h"
    
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define CSMON_CONFIG    1

#if CSMON_CONFIG

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */
#define CSMON_CONFIG_PARAMETER_COUNT_MAX    200

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
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

     uint32_t regU32;
      int32_t regS32;
     uint16_t regU16;
      int16_t regS16;
     uint_least8_t regU8;
      int_least8_t regS8;
} uAnyType32_t;



typedef struct
{
    uint16_t u16ParameterIndexID;
    CSMON_uParameterDefinitionsBitsCount_t uBitsCount;
    char au8NameUnit[(CSMON_SET_PARAMETER_NAMEUNIT_LENGTH_MAX)/2];
    uAnyType32_t u32Max;
    uAnyType32_t u32Min;
    uAnyType32_t u32Def;
    //uAnyType32_t u32Offset;
    float Norm;                 /* 0.0 - Default HEX Visualization; Any other -> Default Decimal Visualization */
    CSMON_uParameterDefinitionsFunctionCode_t uFunctionCode;
    CSMON_uParameterDefinitionsAccessLevel_t uAccessLevel;
    CSMON_uRegisterAddressTableSize uParameterSize;
    CSMON_uRegisterAddressTableFlags uParameterFlags;
    uint16_t u16RealAddress;
    CSMON_eDataType_t eDataTypeAttribute;
}MAIN_sParameterList_t;


/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 
extern volatile const MAIN_sParameterList_t asParameterList[CSMON_CONFIG_PARAMETER_COUNT_MAX];

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */

#endif /* CSMON_CONFIG */
 
#endif	/* CSMON_CONFIG_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
