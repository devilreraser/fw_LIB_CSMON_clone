/* *****************************************************************************
 * File:   csmon_adr.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 08 14 12:51
 * 
 * Description: ...Variables and Functions externally used as address pointers
 * 
 **************************************************************************** */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Guard condition file contents not included more than once */  
/* #pragma once */
#ifndef CSMON_ADR_H
#define	CSMON_ADR_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdbool.h>
#include <stdint.h>

#include "csmon_t.h"
#include "csmon_cfg.h"

    
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

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
extern CSMON_sDateTime_t CSMON_sDateTime;
extern bool CSMON_abFileAutorization_RecordReadOnly[CSMON_FILE_AUTORIZATION_RECORD_COUNT];
extern bool CSMON_abFileAutorization_RecordStringData[CSMON_FILE_AUTORIZATION_RECORD_COUNT];
extern uint16_t CSMON_au16FileAutorization_RecordOffsets[CSMON_FILE_AUTORIZATION_RECORD_COUNT];
extern uint32_t CSMON_au32FileParameterDefinitionsFileRecordElementsRealAddressTable[CSMON_sFileParameterDefinitions_t_ElementsPartsCount];
extern uint16_t CSMON_au32FileParameterDefinitionsFileRecordElementsRecordToRecordOffsetSizeBytesTable[CSMON_sFileParameterDefinitions_t_ElementsPartsCount];
extern uint32_t CSMON_au32FileParameterDefinitionsFileRecordElementsSizeTable[CSMON_sFileParameterDefinitions_t_ElementsPartsCount];
extern const uint8_t CSMON_au8FileImplementedFunctionCodes[CSMON_FILE_IMPLEMENTED_FUNCTION_CODES_COUNT_MAX];
extern uint8_t CSMON_au8FileParameterCountElementsSizeTable[CSMON_sFileParameterCount_t_ElementsCount];
extern uint8_t CSMON_au8FileRTCElementsSizeTable[CSMON_sDateTime_t_ElementsCount];
extern uint8_t CSMON_au8FileParameterNumericElementsSizeTable[CSMON_uFileParameterNumeric_t_ElementsCount];
extern const uint8_t CSMON_au8FileParameterListElementsCountInPartTable [CSMON_sFileParameterDefinitions_t_ElementsPartsCount];
extern uint8_t CSMON_au8FileRecorderConfigElementsSizeTable[HMRECPRM_sConfig_t_ElementsCount];
extern uint8_t CSMON_au8FileRecorderStatusElementsSizeTable[HMRECPRM_sStatus_t_ElementsCount];
extern uint8_t CSMON_au8FileScopeConfigurationElementsSizeTable[CSMON_sFileScopeConfiguration_t_ElementsCount];
extern uint8_t CSMON_au8FileScopeStatusElementsSizeTable[CSMON_sFileScopeStatus_t_ElementsCount];
extern __EXTENDED_MEMORY_MODIFIER__ CSMON_sFileAutorization_t        CSMON_sFileAutorization;
extern CSMON_sFileParameterCount_t      CSMON_sFileParameterCount;
//extern const CSMON_sFileParameterList_t       CSMON_sFileParameterList;
//extern const CSMON_sFileParameterShortName_t  CSMON_asFileParameterShortName[CSMON_FILE_PARAMETER_COUNT_MAX];
extern CSMON_sFileProtocolVersion_t     CSMON_sFileProtocolVersion;


/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
uint16_t CSMON_u16ParametersAccessListIndexGenerate(uint16_t u16Index);

 
#endif	/* CSMON_ADR_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
