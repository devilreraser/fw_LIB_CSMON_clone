/* *****************************************************************************
 * File:   csmon_config.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 08 14 12:51
 *
 * Generated on 02/03/2023 13:20:38 UTC
 * Generator version: 1.10
 * Generator template: D:\repos\github_cs\fw_LIB_CSMON\05_Project\EXAMPLE_CSMON_C2806x\MotorParam\Template\
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


/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define CSMON_CONFIG    1

#if CSMON_CONFIG

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */
#define CSMON_CONFIG_PARAMETER_COUNT_MAX    51

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
         uint16_t regAU16;
          int16_t regS16;
    uint_least8_t regU8;
    uint_least8_t regAU8;
             char regCHAR;
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

extern char UserStringConfiguration[16];
extern char UserZippedString[16];
extern char UserUnicodeString[16];
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

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */

#endif /* CSMON_CONFIG */
 
#endif	/* CSMON_CONFIG_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
