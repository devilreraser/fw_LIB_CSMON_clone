/* *****************************************************************************
 * File:   csmon_t.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 11 12 18:22
 * 
 * Description: ...Types Of csmon.h
 * 
 **************************************************************************** */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Guard condition file contents not included more than once */  
/* #pragma once */
#ifndef CSMON_T_H
#define	CSMON_T_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdbool.h>
#include <stdint.h>
    
#include "csmon.h"
#include "csmon_cfg.h"

#include "hmlib.h"


#include "hmrecparam_t.h"
#include "hmscopeparam_t.h"
#include "hmmodbus.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

#define CSMON_FILE_AUTORIZATION_WITH_HASH       0   /* Rearrange CSMON_FILE_AUTORIZATION_PASSWORD_LENGTH Position definition for Freemaster util */

#define CSMON_PACKED_16_BIT_PARAMETER_DEFINITIONS 1


#define CSMON_USE_DISCONNECT_ON_TIMEOUT         0
#define CSMON_NO_MESSAGE_TIMEOUT_MILLISECONDS   6000.0


#if CSMON_FILE_AUTORIZATION_WITH_HASH == 0
#define CSMON_FILE_AUTORIZATION_PASSWORD_LENGTH                                  16     /* Freemaster Util Uses first occurrence */
#define CSMON_FILE_AUTORIZATION_PASSWORD_LENGTH_INTERNAL                         CSMON_FILE_AUTORIZATION_PASSWORD_LENGTH /* if less than CSMON_FILE_AUTORIZATION_PASSWORD_LENGTH write file command overwites Access Cranted */
#else
#define CSMON_FILE_AUTORIZATION_PASSWORD_LENGTH                                  236 //64 //
#define CSMON_FILE_AUTORIZATION_PASSWORD_LENGTH_INTERNAL                         CSMON_FILE_AUTORIZATION_PASSWORD_LENGTH /* if less than CSMON_FILE_AUTORIZATION_PASSWORD_LENGTH write file command overwites Access Cranted */
#endif

#define CSMON_FILE_AUTORIZATION_HASH_MAX_LENGTH                                  CSMON_FILE_AUTORIZATION_PASSWORD_LENGTH

#define CSMON_FILE_AUTORIZATION_PASSWORD_LENGTH_BYTES_IN_MEMORY                 (CSMON_FILE_AUTORIZATION_PASSWORD_LENGTH_INTERNAL * HMDRV_ADDR_WIDTH)

#if BOARDCFG_SHORT_NAME_CODED_IN_HIGH_BYTE_TI

#define CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH                                   64
#define CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH_ADDRESS_IN_MEMORY                (CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH / HMDRV_ADDR_WIDTH)
#define CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH_BYTES_IN_MEMORY                  (CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH)
  
#else

#define CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH                                   32
#define CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH_ADDRESS_IN_MEMORY                (CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH)
#define CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH_BYTES_IN_MEMORY                  (CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH * HMDRV_ADDR_WIDTH)

#endif
    
 /* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */
    
    
/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

typedef enum
{
    CSMON_ACCESS_FREE = 0,
    CSMON_ACCESS_OPERATOR = 1,
    CSMON_ACCESS_ENGINEER = 2,      //???
    CSMON_ACCESS_SERVICE = 3,       //???
    CSMON_ACCESS_PROGRAMMER = 4,
            
    CSMON_ACCESS_COUNT = 5,
            
    CSMON_ACCESS_INIT = 0xFE,
    CSMON_ACCESS_DENIED = 0xFF
}CSMON_eAccessLevels;

typedef enum
{
    CSMON_FILE_RECORD_AUTORIZATION_ACCESS_REQUEST,
    CSMON_FILE_RECORD_AUTORIZATION_ACCESS_PASSWRD,
    CSMON_FILE_RECORD_AUTORIZATION_ACCESS_GRANTED,  /* ACCEPTED */
}CSMON_eAutorizationFileRecord;
    
    
/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* index generation function prototype */
typedef uint16_t (*CSMON_pfIndexGeneration)(uint16_t);

/* execute function prototype */
typedef void (*CSMON_pfFunctionExecute)(void);


#ifndef uWord32_t
typedef union
{
    uint32_t u32Register;
    uint16_t au16Word[2];
} uWord32_t;
#endif

#ifndef uWord64_t
typedef union
{
    uint64_t u64Register;
    uint32_t au32Word[2];
    uint16_t au16Word[4];
} uWord64_t;
#endif


typedef struct 
{
    char as8Password[CSMON_FILE_AUTORIZATION_PASSWORD_LENGTH_INTERNAL];
}CSMON_sAccessLevelPasswords_t;
    
typedef struct
{
    uint16_t u16AccessLevelReq;
    char as8Password[CSMON_FILE_AUTORIZATION_PASSWORD_LENGTH];
    uint16_t u16AccessLevelNow;
}CSMON_sFileAutorization_t;
   
typedef struct
{
    uint16_t u16ProtocolVersion;
    uint16_t u16FirmwareCommunicationVersion;
    uint16_t u16FirmwareApplicationVersion;
}CSMON_sFileProtocolVersion_t;
#define CSMON_FILE_PROTOCOL_VERSION_LENGTH_BYTES_IN_MEMORY      (uint16_t)(sizeof(CSMON_sFileProtocolVersion_t) * HMDRV_ADDR_WIDTH)

typedef struct
{
    uint16_t u16ParameterCount;
    uint16_t u16RecorderCount;
    uint16_t u16ScopeCount;
    uint16_t u16Padding;
    uint32_t u32RWNVMemorySize_kB;  /* size of string table */
    uint32_t u32ParameterListCheckSum;
    uint32_t u32ParameterNumbCheckSum;
    uint32_t u32ParameterStringCheckSum;
    uint32_t u32ParameterAuxCheckSum;
    uint32_t u32ParameterShortNameCheckSum;
}CSMON_sFileParameterCount_t;
#define CSMON_sFileParameterCount_t_ElementsCount 10  /* count CSMON_sFileParameterCount_t Fields */
#define CSMON_FILE_PARAMETER_COUNT_LENGTH_BYTES_IN_MEMORY       (uint16_t)(sizeof(CSMON_sFileParameterCount_t) * HMDRV_ADDR_WIDTH)


typedef struct
{
    uint16_t u16ParameterIndex[CSMON_FILE_PARAMETER_COUNT_MAX];
}CSMON_sFileParameterAccessList_t;



#define CSMON_FLAG_MASK_BITFIELD    (uint8_t)(1 << 7)
#define CSMON_FLAG_MASK_N_A         (uint8_t)(1 << 6)

typedef struct
{
   uint16_t u16ValueAccessKeyStoradeID;
   uint16_t u8ValueAccessKeyStartBitIndex : 8;
   uint16_t u6Reserved : 6;
   uint16_t bArray : 1;
   uint16_t bBitField : 1;
   
   
}CSMON_sFileParameterValueAccessKey_t;

typedef union
{
    CSMON_sFileParameterValueAccessKey_t sValueAccessKey;
    uint32_t u32Register;
    uWord32_t uRegister;
   
}CSMON_uFileParameterValueAccessKey_t;

#if CSMON_PACKED_16_BIT_PARAMETER_DEFINITIONS

typedef struct
{
    CSMON_uFileParameterValueAccessKey_t ValueAccessKey;
    uint16_t ModbusAddress;
    uint16_t eWriteReadAccessLevelMin;                   /* CSMON_eAccessLevels eWriteAccessLevelMin */
    uint16_t eDefaultWriteReadFunctionCode;               /* HMMODBUS_eCommands eDefaultWriteReadFunctionCode Skipped in File Read Command */
    uint16_t u16PaddingByteBitsCount;

}CSMON_sFileParameterDefinitions16Bit_t;


typedef struct
{
    CSMON_uFileParameterValueAccessKey_t ValueAccessKey;

    uint16_t ModbusAddress;


    uint16_t eWriteAccessLevelMin : 8;                   /* CSMON_eAccessLevels eWriteAccessLevelMin as uint8_t because HMDRV_ADDR_WIDTH = 1 to work */
    uint16_t eReadAccessLevelMin : 8;                    /* CSMON_eAccessLevels eReadAccessLevelMin as uint8_t because HMDRV_ADDR_WIDTH = 1 to work */

    uint16_t eDefaultWriteFunctionCode : 8;               /* HMMODBUS_eCommands eDefaultWriteFunctionCode Skipped in File Read Command */
    uint16_t eDefaultReadFunctionCode : 8;                /* HMMODBUS_eCommands eDefaultReadFunctionCode Skipped in File Read Command */

    uint16_t u8PaddingByte : 8;
    uint16_t u8BitsCount : 8;

}CSMON_sFileParameterDefinitions_t;
#define CSMON_sFileParameterDefinitions_t_ElementsCount  6  /* count CSMON_sFileParameterDefinitions_t Fields */
#define CSMON_sFileParameterDefinitions_t_ElementsFirst  4  /* count CSMON_sFileParameterDefinitions_t Fields */
#define CSMON_sFileParameterDefinitions_t_ElementsSecond 1  /* count CSMON_sFileParameterDefinitions_t Fields taken from CSMON_sFileParameterDefinitionsFromAccessLevel_t */
#define CSMON_sFileParameterDefinitions_t_ElementsThird  1  /* count CSMON_sFileParameterDefinitions_t Fields */

#if HMDRV_ADDR_WIDTH == 2
#define CSMON_FILE_PARAMETER_LIST_LENGTH_BYTES_IN_RECORD (uint16_t)(sizeof(CSMON_sFileParameterDefinitions_t) * HMDRV_ADDR_WIDTH)
#else
#define CSMON_FILE_PARAMETER_LIST_LENGTH_BYTES_IN_RECORD sizeof(CSMON_sFileParameterDefinitions_t)
#endif

#else

typedef struct
{
    CSMON_uFileParameterValueAccessKey_t ValueAccessKey;

    uint16_t ModbusAddress;

    uint8_t eReadAccessLevelMin;                    /* CSMON_eAccessLevels eReadAccessLevelMin as uint8_t because HMDRV_ADDR_WIDTH = 1 to work */
    uint8_t eWriteAccessLevelMin;                   /* CSMON_eAccessLevels eWriteAccessLevelMin as uint8_t because HMDRV_ADDR_WIDTH = 1 to work */

    uint8_t eDefaultReadFunctionCode;                /* HMMODBUS_eCommands eDefaultReadFunctionCode Skipped in File Read Command */
    uint8_t eDefaultWriteFunctionCode;               /* HMMODBUS_eCommands eDefaultWriteFunctionCode Skipped in File Read Command */

    uint8_t u8BitsCount;
    uint8_t u8PaddingByte;        
           
}CSMON_sFileParameterDefinitions_t;
#define CSMON_sFileParameterDefinitions_t_ElementsCount  9  /* count CSMON_sFileParameterDefinitions_t Fields */
#define CSMON_sFileParameterDefinitions_t_ElementsFirst  5  /* count CSMON_sFileParameterDefinitions_t Fields */
#define CSMON_sFileParameterDefinitions_t_ElementsSecond 2  /* count CSMON_sFileParameterDefinitions_t Fields taken from CSMON_sFileParameterDefinitionsFromAccessLevel_t */
#define CSMON_sFileParameterDefinitions_t_ElementsThird  2  /* count CSMON_sFileParameterDefinitions_t Fields */

#if HMDRV_ADDR_WIDTH == 2
/* When used elements tables record to record offset is calculated with the data from the element table. */
/* If 1 byte elements specified, the high byte in TI 16-bit addressing is not used for this reason the bytes in record differ from the bytes in memory and this must be taken into account. */
/* The other possibility is to rework the CSMON_sFileParameterDefinitions_t structure and the 8-bit values are specified as bit-fields in 16-bit element */
/* note just for info: 8-bit elements when written to modbus, when specified that is 1 byte element , in TI-16-bit addressing pointer to next element is moved to next real address  */
#define CSMON_FILE_PARAMETER_LIST_LENGTH_BYTES_IN_RECORD (uint16_t)((sizeof(CSMON_sFileParameterDefinitions_t) - 3) * HMDRV_ADDR_WIDTH) /* Minus 3 because uint8_t */
#else
#define CSMON_FILE_PARAMETER_LIST_LENGTH_BYTES_IN_RECORD sizeof(CSMON_sFileParameterDefinitions_t)
#endif

#endif

#define CSMON_sFileParameterDefinitions_t_ElementsPartsCount    3

#define CSMON_FILE_PARAMETER_LIST_LENGTH_BYTES_IN_MEMORY (uint16_t)(sizeof(CSMON_sFileParameterDefinitions_t) * HMDRV_ADDR_WIDTH) /*  */




typedef struct
{
    uint_least8_t u8HashAlgorithmID;
    uint_least8_t u8Year;
    uint_least8_t u8Month;
    uint_least8_t u8Day;
    uint_least8_t u8WeekDays;
    uint_least8_t u8Hours;
    uint_least8_t u8Minutes;
    uint_least8_t u8Seconds;
    uint32_t u32Microseconds;       /* MCU Clock Base */
    uint32_t u32ClientID;
}CSMON_sDateTime_t;
#define CSMON_sDateTime_t_ElementsCount 10  /* count CSMON_sDateTime_t Fields */
#define CSMON_FILE_RTC_LENGTH_BYTES_IN_MEMORY  (uint16_t)(sizeof(CSMON_sDateTime_t) * HMDRV_ADDR_WIDTH)


typedef enum
{
    CSMON_DATA_TYPE_BOOL_FLAG = 0x0200,
    CSMON_DATA_TYPE_ENUM_FLAG = 0x0100,
    CSMON_DATA_TYPE_SIGNED_FLAG = 0x0080,
    CSMON_DATA_TYPE_FLOAT_FLAG = 0x0040,
    CSMON_DATA_TYPE_SIZE_MASK = 0x0003,

    CSMON_DATA_TYPE_U8  = 0x0000,
    CSMON_DATA_TYPE_U16 = 0x0001,
    CSMON_DATA_TYPE_U32 = 0x0002,
    CSMON_DATA_TYPE_U64 = 0x0003, /* for HMRECPRM_eVariant64_t or larger */

    CSMON_DATA_TYPE_S8  = 0x0080,
    CSMON_DATA_TYPE_S16 = 0x0081,
    CSMON_DATA_TYPE_S32 = 0x0082,
    CSMON_DATA_TYPE_S64 = 0x0083, /* for HMRECPRM_eVariant64_t or larger */

    CSMON_DATA_TYPE_F16 = 0x00C1,
    CSMON_DATA_TYPE_F32 = 0x00C2,
    CSMON_DATA_TYPE_F64 = 0x00C3, /* for HMRECPRM_eVariant64_t or larger */

    CSMON_DATA_TYPE_MASK = 0xFFFF

}CSMON_eDataType_t; /* 16 bits - change also HMRECPRM_eVariantType_t */





typedef struct
{
    CSMON_eDataType_t eDataType;
    CSMON_eVisualType_t eVisualType;
}CSMON_sDataType_t;


typedef struct
{
    uWord32_t uVirtAddress;
    uWord32_t uDataType;
    uWord64_t uDefault;
    uWord64_t uMinimum;
    uWord64_t uMaximum;
    uWord64_t sMultply;
    uWord64_t sOffset;

}CSMON_sFileParameterNumericRegisterWord_t;
typedef struct
{
    HMDRV_U32 u32VirtAddress;
    HMDRV_U32 u32DataType;
    HMDRV_U64 u64Default;
    HMDRV_U64 u64Minimum;
    HMDRV_U64 u64Maximum;
    HMDRV_S64 s64Multply;
    HMDRV_S64 s64Offset;

}CSMON_sFileParameterNumericRegister_t; /* Note: fix CSMON_sFileParameterNumericRegister16Bit_t if change here */
typedef struct
{
    CSMON_uFileParameterValueAccessKey_t ValueAccessKey;
    CSMON_sDataType_t sDataType;
    HMDRV_U64 u64Default;
    HMDRV_U64 u64Minimum;
    HMDRV_U64 u64Maximum;
    union
    {
        HMDRV_U64 s64Multply;
        float s64MultplyFloat;
    };
    HMDRV_S64 s64Offset;

}CSMON_sFileParameterNumericStructure_t;
typedef union
{
    CSMON_sFileParameterNumericStructure_t sStructure;
    CSMON_sFileParameterNumericRegister_t sRegister;
    CSMON_sFileParameterNumericRegisterWord_t sRegisterWord;

}CSMON_uFileParameterNumeric_t;
#define CSMON_uFileParameterNumeric_t_ElementsCount 7 + 2 /* count CSMON_uFileParameterNumeric_t Fields */
#define CSMON_FILE_PARAMETER_NUMERIC_LENGTH_BYTES_IN_MEMORY (uint16_t)(sizeof(CSMON_uFileParameterNumeric_t) * HMDRV_ADDR_WIDTH)


#if CSMON_PACKED_16_BIT_PARAMETER_DEFINITIONS

typedef struct
{
    uint16_t eDefaultWriteFunctionCode : 8;  /* HMMODBUS_eCommands eDefaultWriteFunctionCode   -> Used in File Read Command instead of CSMON_sFileParameterDefinitions_t */
    uint16_t eDefaultReadFunctionCode : 8;   /* HMMODBUS_eCommands eDefaultReadFunctionCode  -> Used in File Read Command instead of CSMON_sFileParameterDefinitions_t */

}CSMON_sFileParameterDefinitionsFromAccessLevel_t;
#define CSMON_sFileParameterDefinitionsFromAccessLevel_t_ElementsCount 1  /* count CSMON_sFileParameterDefinitionsFromAccessLevel_t Fields */

#else

typedef struct
{
    uint8_t eDefaultReadFunctionCode;   /* HMMODBUS_eCommands eDefaultReadFunctionCode as uint8_t because HMDRV_ADDR_WIDTH = 1 to work -> Used in File Read Command instead of CSMON_sFileParameterDefinitions_t */
    uint8_t eDefaultWriteFunctionCode;  /* HMMODBUS_eCommands eDefaultWriteFunctionCode as uint8_t because HMDRV_ADDR_WIDTH = 1 to work  -> Used in File Read Command instead of CSMON_sFileParameterDefinitions_t */
              
}CSMON_sFileParameterDefinitionsFromAccessLevel_t;
#define CSMON_sFileParameterDefinitionsFromAccessLevel_t_ElementsCount 2  /* count CSMON_sFileParameterDefinitionsFromAccessLevel_t Fields */

#endif

#if CSMON_PARAMETER_LIST_READ_ONLY
typedef struct
{
    CSMON_sFileParameterDefinitions_t asFileParameterDefinitions[CSMON_FILE_PARAMETER_COUNT_MAX];
}CSMON_sFileParameterList_t;
#endif
   
typedef struct
{
    char as8ShortName[CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH_ADDRESS_IN_MEMORY];
}CSMON_sFileParameterShortName_t;

   
typedef struct
{                                           /* SCC - Addresses; Note: 32 Bit Writes Not Working - need 16-bit writes */
    uint16_t u16ParamCountUse;              /* 0x00120000 */
    uint16_t u16ParamCountMax;              /* 0x00120001 */
    uint16_t u16ModbusCountUse;             /* 0x00120002 */
    uint16_t u16ModbusCountMax;             /* 0x00120003 */
    uWord32_t uParamCheckSum;               /* 0x00120004 */
    uWord32_t uParamTableVer;               /* 0x00120006 */
    uWord32_t uParamDateTime;               /* 0x00120008 */
    uWord32_t uUser_CheckSum;               /* 0x0012000A */
    uWord32_t uParamDateTimeCopy;           /* 0x0012000C - write Build Date Time On First Run */
    uWord32_t uReserved;                    /* 0x0012000E */
    uint16_t u16ParamCountUseCheck;         /* 0x00120010 */
    uint16_t u16ParamCountMaxCheck;         /* 0x00120011 */
    uint16_t u16ModbusCountUseCheck;        /* 0x00120012 */
    uint16_t u16ModbusCountMaxCheck;        /* 0x00120013 */

}CSMON_sFileParameterAux_t;

#define CSMON_FILE_PARAMETER_AUX_LENGTH_BYTES_IN_MEMORY (uint16_t)(sizeof(CSMON_sFileParameterAux_t) * HMDRV_ADDR_WIDTH)



/* *****************************************************************************
* Parameter Scopes - Configuration And Status Structures
**************************************************************************** */
/* Scope Configuration */
typedef struct
{
    uint16_t                    nCountVariables;
    uint16_t                    u16Reserved;                        /*  */
    uint32_t                    nVariableIDScope[HMSCOPEPRM_MAX_VARS];
}HMSCOPEPRM_sConfig_t;
#define CSMON_sFileScopeConfiguration_t_ElementsCount       42  /* 1 + 1 + HMSCOPEPRM_MAX_VARS*2 */

/* Scope Status */
#if 0
typedef struct
{
    uint16_t                    Dummy;
}HMSCOPEPRM_sConfig_t;
#endif
#define CSMON_sFileScopeStatus_t_ElementsCount (1)        /* Dummy */














/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */

#endif	/* CSMON_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
