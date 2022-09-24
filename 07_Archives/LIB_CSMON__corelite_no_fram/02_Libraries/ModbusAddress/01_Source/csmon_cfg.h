/* *****************************************************************************
 * File:   csmon_cfg.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 08 14 12:51
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
#ifndef CSMON_CFG_H
#define	CSMON_CFG_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "boardcfg.h"


/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

#define CSMON_PARAMETER_LIST_READ_ONLY                      BOADRCFG_CSMON_PARAM_LIST_READ_ONLY
#define CSMON_PARAMETER_LIST_RW_ADDR                        BOADRCFG_CSMON_PARAM_LIST_RW_ADDR
#define CSMON_PARAMETER_LIST_RW_SIZE                        BOADRCFG_CSMON_PARAM_LIST_RW_SIZE
#define CSMON_PARAMETER_NAME_RW_ADDR                        BOADRCFG_CSMON_PARAM_NAME_RW_ADDR
#define CSMON_PARAMETER_NAME_RW_SIZE                        BOADRCFG_CSMON_PARAM_NAME_RW_SIZE
#define CSMON_PARAMETER_NUMB_RW_ADDR                        BOADRCFG_CSMON_PARAM_NUMB_RW_ADDR
#define CSMON_PARAMETER_NUMB_RW_SIZE                        BOADRCFG_CSMON_PARAM_NUMB_RW_SIZE
#define CSMON_PARAMETER_STRG_RW_ADDR                        BOADRCFG_CSMON_PARAM_STRG_RW_ADDR
#define CSMON_PARAMETER_STRG_RW_SIZE                        BOADRCFG_CSMON_PARAM_STRG_RW_SIZE
#define CSMON_PARAMETER_SAUX_RW_ADDR                        BOADRCFG_CSMON_PARAM_SAUX_RW_ADDR
#define CSMON_PARAMETER_SAUX_RW_SIZE                        BOADRCFG_CSMON_PARAM_SAUX_RW_SIZE
#define CSMON_PARAMETER_RSRV_RW_ADDR                        BOADRCFG_CSMON_PARAM_RSRV_RW_ADDR
#define CSMON_PARAMETER_RSRV_RW_SIZE                        BOADRCFG_CSMON_PARAM_RSRV_RW_SIZE

#ifndef BOARDCFG_CSMON_PROCESS_CALL_HMLIB
#define CSMON_PROCESS_CALL_HMLIB                            1   /* Default */
#else
#define CSMON_PROCESS_CALL_HMLIB                            BOARDCFG_CSMON_PROCESS_CALL_HMLIB
#endif

#define CSMON_USE_NO_COILS                                  0   /* Debug */

#define CSMON_USE_NO_INPUT_REGISTERS                        0   /* Debug */

#define CSMON_PROTOCOL_VERSION                              1

#define CSMON_NO_PASSWORD_IN_FREE_ACCESS_LEVEL              1

#ifndef BOARDCFG_RECORDERS_IN_FLAG_PROCESS_PASSED_INTERFACE
#define CSMON_RECORDERS_IN_FLAG_PROCESS_PASSED_INTERFACE    0
#else
#define CSMON_RECORDERS_IN_FLAG_PROCESS_PASSED_INTERFACE    BOARDCFG_RECORDERS_IN_FLAG_PROCESS_PASSED_INTERFACE
#endif

#if BOARDCFG_USE_TSC_DSPIC
//#define CSMON_EXTENDED_MEMORY_MODIFIER  __eds__
#define __CONSTANT_MEMORY_MODIFIER__ __psv__
#define __EXTENDED_SPACE_ATTRIBUTE__(x) __attribute__(x)
#define __EXTENDED_MEMORY_MODIFIER__ __eds__
#else
//#define CSMON_EXTENDED_MEMORY_MODIFIER
#define __CONSTANT_MEMORY_MODIFIER__
#define __EXTENDED_SPACE_ATTRIBUTE__(x)
#define __EXTENDED_MEMORY_MODIFIER__
//#include <stdint.h>
//#include "stdextend.h"
#endif

#define CSMON_FILE_AUTORIZATION_ACCESS_LEVEL_LENGTH                             2
#define CSMON_FILE_AUTORIZATION_RECORD_COUNT                                    3

#define CSMON_FILE_IMPLEMENTED_FUNCTION_CODES_COUNT_MAX                         20
/*  CSMON_FILE_IMPLEMENTED_FUNCTION_CODES is treated as null terminated string */
#define CSMON_FILE_IMPLEMENTED_FUNCTION_CODES_RECORD_COUNT                      1
#define CSMON_FILE_IMPLEMENTED_FUNCTION_CODES_RECORD_SIZE                       CSMON_FILE_IMPLEMENTED_FUNCTION_CODES_COUNT_MAX

#ifndef BOARDCFG_CSMON_FILE_PARAMETER_COUNT_MAX
#define CSMON_FILE_PARAMETER_COUNT_MAX                                          (1030-1000) + (1041-1040) + 1 + 1 + 1 + 1 //((MODBUSREG_REGID_CONTROL_END - MODBUSREG_REGID_CONTROL_START) + 1) REG 0xFFFF    COIL MODBUSREG_COILID_TEST; COILIN; INPUTREG
#else
#define CSMON_FILE_PARAMETER_COUNT_MAX                                          BOARDCFG_CSMON_FILE_PARAMETER_COUNT_MAX
#endif

#define CSMON_FILE_PROTOCOL_VERSION_RECORD_COUNT                                1
#define CSMON_FILE_PROTOCOL_VERSION_RECORD_SIZE                                 CSMON_FILE_PROTOCOL_VERSION_LENGTH_BYTES_IN_MEMORY

#define CSMON_FILE_PARAMETER_COUNT_RECORD_COUNT                                 1
#define CSMON_FILE_PARAMETER_COUNT_RECORD_SIZE                                  CSMON_FILE_PARAMETER_COUNT_LENGTH_BYTES_IN_MEMORY

#define CSMON_FILE_RTC_RECORD_COUNT                                             1
#define CSMON_FILE_RTC_RECORD_SIZE                                              CSMON_FILE_RTC_LENGTH_BYTES_IN_MEMORY


#define CSMON_FILE_PARAMETER_LIST_RECORD_SIZE                                   CSMON_FILE_PARAMETER_LIST_LENGTH_BYTES_IN_RECORD
#define CSMON_FILE_PARAMETER_SHORT_NAME_RECORD_SIZE                             CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH //CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH_BYTES_IN_MEMORY
#define CSMON_FILE_PARAMETER_SHORT_NAME_RECORD_TO_RECORD_OFFSET                 CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH_BYTES_IN_MEMORY
#define CSMON_FILE_PARAMETER_NUMERIC_RECORD_SIZE                                CSMON_FILE_PARAMETER_NUMERIC_LENGTH_BYTES_IN_MEMORY

#define CSMON_FILE_PARAMETER_AUXILIARY_RECORD_SIZE                              CSMON_PARAMETER_SAUX_RW_SIZE

#define CSMON_FILE_PARAMETER_STRING_BYTES_SIZE                                  CSMON_PARAMETER_STRG_RW_SIZE

/* Recorder Default Sample Configuration */
#define CSMON_RECORDER_SAMPLE_COUNT_DEFAULT                                     6000
#define CSMON_RECORDER_POSTTRIGGER_SAMPLE_COUNT_DEFAULT                         100
#define CSMON_RECORDER_PRETRIGGER_SAMPLE_COUNT_DEFAULT                          (CSMON_RECORDER_SAMPLE_COUNT_DEFAULT - CSMON_RECORDER_POSTTRIGGER_SAMPLE_COUNT_DEFAULT)

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
 
#endif	/* CSMON_CFG_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
