/* *****************************************************************************
 * File:   modbusreg_t.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 05 21 11:58
 * 
 * Description: Modbus Registers Types
 * 
 **************************************************************************** */

#ifdef _WIN32
#   ifdef MODBUSREGLIB_EXPORTS
#       define MODBUSREGLIB_API __declspec(dllexport)		/* If Used in DLL Project */
#   elif defined(MODBUSREGLIB_STATIC)
#       define MODBUSREGLIB_API							/* If Used in LIB Project */
#   else
#       define MODBUSREGLIB_API __declspec(dllimport)		/* If Used in APP Project */
#   endif
#else
#		define MODBUSREGLIB_API 
#endif

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus



/* Guard condition file contents not included more than once */  
//#pragma once
#ifndef MODBUSREG_T_H
#define	MODBUSREG_T_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdint.h>
#include <stdbool.h>

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#include "boardcfg.h"

/* *****************************************************************************
 * Constants Definitions
 **************************************************************************** */

#define MODBUSREG_FILE_MAX_RECORD_COUNT     10000                /* 10000 get from modbus spec */

#define MODBUSREG_HOLDREG_ADDR_TABLE_READ_ONLY              BOADRCFG_MODBUSREG_HOLDREG_ADDR_TABLE_READ_ONLY

#define MODBUSREG_HOLDREG_LIST_RW_ADDR                        BOADRCFG_MODBUSREG_HOLDREG_LIST_RW_ADDR
#define MODBUSREG_HOLDREG_ADDR_RW_ADDR                        BOADRCFG_MODBUSREG_HOLDREG_ADDR_RW_ADDR
#define MODBUSREG_HOLDREG_LIST_RW_SIZE                        BOADRCFG_MODBUSREG_HOLDREG_LIST_RW_SIZE
//#define MODBUSREG_HOLDREG_NAME_RW_ADDR                        BOADRCFG_MODBUSREG_HOLDREG_NAME_RW_ADDR
//#define MODBUSREG_HOLDREG_NAME_RW_SIZE                        BOADRCFG_MODBUSREG_HOLDREG_NAME_RW_SIZE
    
/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */




typedef enum 
{
    #if USE_NO_MAIN == 0
    MODBUSREG_REGID_MAIN_START = 9000,
    MODBUSREG_REGID_MAIN_BAUDRATE_HI = MODBUSREG_REGID_MAIN_START     ,
    MODBUSREG_REGID_MAIN_BAUDRATE_LO     ,
    MODBUSREG_REGID_MAIN_PARITY,        
    MODBUSREG_REGID_MAIN_END,       
            
    MODBUSREG_REGID_MAIN_COUNT = MODBUSREG_REGID_MAIN_END - MODBUSREG_REGID_MAIN_START,
    #else
    MODBUSREG_REGID_MAIN_COUNT = 0,
    #endif        
          
            
            

    #if USE_CONTROL
    MODBUSREG_REGID_CONTROL_START = 1000,
    /* CSMON */        
    MODBUSREG_REGID_CONTROL_BEFORE_FIRST_REGISTER = MODBUSREG_REGID_CONTROL_START - 1 , 
    
    /* 1000 */         
    MODBUSREG_REGID_STACK1_RESERVED_REQUEST_STATUS  ,   /* 1000 */
    MODBUSREG_REGID_STACK1_RESERVED_REQUEST_WRITE   = MODBUSREG_REGID_STACK1_RESERVED_REQUEST_STATUS    ,
    MODBUSREG_REGID_STACK1_RESERVED_STATUS_READ     = MODBUSREG_REGID_STACK1_RESERVED_REQUEST_STATUS    ,     /* fixed MODBUSREG_REGID_CONTROL_COUNT_ADDITIONAL_READ */
    MODBUSREG_REGID_STACK1_CONTROL_STATE            ,   /* 1001 */
    MODBUSREG_REGID_STACK1_CONTROL_ERROR            ,   /* 1002 */
    MODBUSREG_REGID_STACK1_ERROR_STATE              ,   /* 1003 */
    MODBUSREG_REGID_STACK1_CLEAR_FAULTS             ,   /* 1004 */
    MODBUSREG_REGID_STACK1_CLEAR_FAULTS_WRITE       = MODBUSREG_REGID_STACK1_CLEAR_FAULTS    ,
    MODBUSREG_REGID_STACK1_CLEAR_FAULTS_READ        = MODBUSREG_REGID_STACK1_CLEAR_FAULTS    ,     /* fixed MODBUSREG_REGID_CONTROL_COUNT_ADDITIONAL_READ */
    MODBUSREG_REGID_STACK1_ERROR_STATUS             ,   /* 1005 */
    MODBUSREG_REGID_STACK1_SPEED_REQUEST            ,   /* 1006 */
    MODBUSREG_REGID_STACK1_VOLTAGE_DCLINK_REQUEST   ,   /* 1007 */
    MODBUSREG_REGID_STACK1_CURRENT_REQUEST          ,   /* 1008 */
    MODBUSREG_REGID_STACK1_RUN_REQUEST_STATUS       ,   /* 1009 */
    MODBUSREG_REGID_STACK1_RUN_REQUEST_WRITE        = MODBUSREG_REGID_STACK1_RUN_REQUEST_STATUS    ,
    MODBUSREG_REGID_STACK1_RUN_STATUS_READ          = MODBUSREG_REGID_STACK1_RUN_REQUEST_STATUS    ,     /* fixed MODBUSREG_REGID_CONTROL_COUNT_ADDITIONAL_READ */
            
    /* 1010 */         
    MODBUSREG_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEA  ,
    MODBUSREG_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEB  ,                          
    MODBUSREG_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEC  ,
    MODBUSREG_REGID_STACK1_VOLTAGE_DCLINK_RMS  ,
    MODBUSREG_REGID_STACK1_CURRENT_MODULE_RMS_PHASEA  ,
    MODBUSREG_REGID_STACK1_CURRENT_MODULE_RMS_PHASEB  ,                          
    MODBUSREG_REGID_STACK1_CURRENT_MODULE_RMS_PHASEC  ,
    MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEA  ,    /* For now same as non-filtered value */
    MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEB  ,    /* For now same as non-filtered value */
    MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEC  ,    /* For now same as non-filtered value */
    
    /* 1020 */         
    MODBUSREG_REGID_STACK1_CURRENT_SENSOR_PHASEA  ,
    MODBUSREG_REGID_STACK1_CURRENT_SENSOR_PHASEB  ,                          
    MODBUSREG_REGID_STACK1_CURRENT_SENSOR_PHASEC  ,
    MODBUSREG_REGID_STACK1_VOLTAGE_DCLINK_FEEDBACK  ,
    MODBUSREG_REGID_STACK1_CURRENT_MODULE_PHASEA  ,
    MODBUSREG_REGID_STACK1_CURRENT_MODULE_PHASEB  ,                          
    MODBUSREG_REGID_STACK1_CURRENT_MODULE_PHASEC  ,
    MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_PHASEA  ,
    MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_PHASEB  ,
    MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_PHASEC  ,   
            
            
    /* 1030 */ 
    MODBUSREG_REGID_NOT_IMPLEMENTED = 1040 - 1030,        
            
    MODBUSREG_REGID_1040 = 1040, 
            
    /* 1040 */        
    MODBUSREG_REGID_STACK1_CFBK_MODULE_NOMINAL_CURRENT = MODBUSREG_REGID_1040,       
            
                     
    MODBUSREG_REGID_CONTROL_END,
            
    MODBUSREG_REGID_CONTROL_COUNT = MODBUSREG_REGID_CONTROL_END - MODBUSREG_REGID_CONTROL_START - MODBUSREG_REGID_NOT_IMPLEMENTED,   
    MODBUSREG_REGID_CONTROL_COUNT_ADDITIONAL_READ = 3,
    #else
    MODBUSREG_REGID_CONTROL_COUNT_ADDITIONAL_READ = 0,
    MODBUSREG_REGID_CONTROL_COUNT = 0,
    #endif        
    
    #if USE_SPI_FLIR
    MODBUSREG_REGID_FLIR_START = 0x8000,
    MODBUSREG_REGID_FLIR_VOSPI_STATE_REQ = 0x8000,
    MODBUSREG_REGID_FLIR_VOSPI_PACKET_LENGTH = 0x8001,
    MODBUSREG_REGID_FLIR_RESET = 0x8002,  
    MODBUSREG_REGID_FLIR_SWITCH_PING_PONG = 0x8003,
    MODBUSREG_REGID_FLIR_VOSPI_PACKET_ZIP_MODE = 0x8004,
    MODBUSREG_REGID_FLIR_VOSPI_PACKET_PING_PONG_LEN = 0x8005,
    MODBUSREG_REGID_FLIR_END,
    MODBUSREG_REGID_FLIR_COUNT = MODBUSREG_REGID_FLIR_END - MODBUSREG_REGID_FLIR_START,
    #else
    MODBUSREG_REGID_FLIR_COUNT = 0,
    #endif     
            
    MODBUSREG_REGID_MODBUS_RECEIVED_MESSAGES_COUNTER = 0xFFFE,      /* used */  
    MODBUSREG_REGID_MODBUS_DUMMY_USAGE = 0xFFFF,      /* internal, not used in initialization of the tables */  
    
    MODBUSREG_REGID_COUNT =  MODBUSREG_REGID_MAIN_COUNT + MODBUSREG_REGID_CONTROL_COUNT + MODBUSREG_REGID_CONTROL_COUNT_ADDITIONAL_READ + MODBUSREG_REGID_FLIR_COUNT + 1,       
            
            
    MODBUSREG_REGID_END = 0xFFFF

}MODBUSREG_eHoldingRegisterID;


#ifndef BOARDCFG_MODBUSREG_HOLDREG_ADDR_TABLE_COUNT_MAX
#define MODBUSREG_REGISTER_TABLE_SIZE   MODBUSREG_REGID_COUNT
#else
#define MODBUSREG_REGISTER_TABLE_SIZE   BOARDCFG_MODBUSREG_HOLDREG_ADDR_TABLE_COUNT_MAX
#endif





typedef enum 
{
    MODBUSREG_INREGID_CONTROL_START = 2000,
    /* CSMON */
    MODBUSREG_INREGID_CONTROL_BEFORE_FIRST_REGISTER = MODBUSREG_INREGID_CONTROL_START - 1 ,

    /* 2000 */
    MODBUSREG_INREGID_CONTROL_DUMMY,

    MODBUSREG_INREGID_CONTROL_END,
    MODBUSREG_INREGID_CONTROL_COUNT = MODBUSREG_INREGID_CONTROL_END - MODBUSREG_INREGID_CONTROL_START,


    MODBUSREG_INREGID_COUNT = MODBUSREG_INREGID_CONTROL_COUNT,
    MODBUSREG_INREGID_END = 0xFFFF
}MODBUSREG_eInputRegisterID;

#define MODBUSREG_INPUT_REGISTER_TABLE_SIZE    MODBUSREG_INREGID_COUNT









typedef enum 
{
    MODBUSREG_COILID_CONTROL_START = 3000,
    /* CSMON */
    MODBUSREG_COILID_CONTROL_BEFORE_FIRST_REGISTER = MODBUSREG_COILID_CONTROL_START - 1 ,

    /* 3000 */
    MODBUSREG_COILID_CONTROL_TEST,
            
    MODBUSREG_COILID_CONTROL_END,
    MODBUSREG_COILID_CONTROL_COUNT = MODBUSREG_COILID_CONTROL_END - MODBUSREG_COILID_CONTROL_START,


    MODBUSREG_COILID_COUNT = MODBUSREG_COILID_CONTROL_COUNT,
    MODBUSREG_COILID_MODBUS_DUMMY_USAGE = 0xFFFE,      /* internal, not used in initialization of the tables */  
    MODBUSREG_COILID_END = 0xFFFF
}MODBUSREG_eHoldingCoilID;

#define MODBUSREG_COIL_TABLE_SIZE    MODBUSREG_COILID_COUNT



typedef enum 
{
    MODBUSREG_INCOILID_CONTROL_START = 4000,
    /* CSMON */
    MODBUSREG_INCOILID_CONTROL_BEFORE_FIRST_REGISTER = MODBUSREG_INCOILID_CONTROL_START - 1 ,

    /* 4000 */
    MODBUSREG_INCOILID_CONTROL_DUMMY,

    MODBUSREG_INCOILID_CONTROL_END,
    MODBUSREG_INCOILID_CONTROL_COUNT = MODBUSREG_INCOILID_CONTROL_END - MODBUSREG_INCOILID_CONTROL_START,


    MODBUSREG_INCOILID_COUNT = MODBUSREG_INCOILID_CONTROL_COUNT,

    MODBUSREG_INCOILID_END = 0xFFFF
}MODBUSREG_eInputCoilID;

#define MODBUSREG_INPUT_COIL_TABLE_SIZE    MODBUSREG_INCOILID_COUNT







typedef enum 
{
    MODBUSREG_FILE_START = 0,
    
    #if USE_SPI_FLIR
    /* ZERO- Based File number offsetting */
    MODBUSREG_FILE_FLIR_START = 0x1000,     /* consider for flir files -> change file number not to match with other projects */
    MODBUSREG_FILE_FLIR_I2C = MODBUSREG_FILE_FLIR_START,
    MODBUSREG_FILE_IRCAMERA_DEF,
    MODBUSREG_FILE_IRCAMERA_ZIP_DEF,
            
    MODBUSREG_FILE_IRCAMERA_ZIP,
    MODBUSREG_FILE_IRCAMERA_RAW,
    MODBUSREG_FILE_IRCAMERA_RGB,

    MODBUSREG_FILE_IRCAM_01_ZIP,
    MODBUSREG_FILE_IRCAM_01_RAW,
    MODBUSREG_FILE_IRCAM_01_RGB,
            
    MODBUSREG_FILE_IRCAM_02_ZIP,
    MODBUSREG_FILE_IRCAM_02_RAW,
    MODBUSREG_FILE_IRCAM_02_RGB,
            
    MODBUSREG_FILE_IRCAM_03_ZIP,
    MODBUSREG_FILE_IRCAM_03_RAW,
    MODBUSREG_FILE_IRCAM_03_RGB,
    MODBUSREG_FILE_FLIR_END,
    MODBUSREG_FILE_FLIR_COUNT = MODBUSREG_FILE_FLIR_END - MODBUSREG_FILE_FLIR_START,
    #else
    MODBUSREG_FILE_FLIR_COUNT = 0,
    #endif

            

    #if USE_CSMON 
    /* ZERO- Based File number offset (0 means file 1) */
    MODBUSREG_FILE_CSMON_BASE_START = 0,
    MODBUSREG_FILE_CSMON_AUTORIZATION = MODBUSREG_FILE_CSMON_BASE_START,    /* 01 - CSMON_sFileAutorization_t */
    MODBUSREG_FILE_CSMON_PROTOCOL_VERSION,                                  /* 02 - CSMON_sFileProtocolVersion_t */
    MODBUSREG_FILE_CSMON_PARAMETER_COUNT,                                   /* 03 - CSMON_sFileParameterCount_t (DataSectionSize) */
    MODBUSREG_FILE_CSMON_PARAMETER_LIST,                                    /* 04 - CSMON_sFileParameterDefinitions_t */
    MODBUSREG_FILE_CSMON_PARAMETER_SHORT_NAME,                              /* 05 - CSMON_sFileParameterShortName_t */
    MODBUSREG_FILE_CSMON_IMPLEMENTED_FUNCTION_CODES,                        /* 06 - uint8_t CSMON_au8FileImplementedFunctionCodes */
    MODBUSREG_FILE_CSMON_RTC,                                               /* 07 - CSMON_sDateTime_t */
    MODBUSREG_FILE_CSMON_PARAMETER_DESCRIPTION_NUMERIC,                     /* 08 - CSMON_sFileParameterNumericStructure_t */
    MODBUSREG_FILE_CSMON_PARAMETER_DESCRIPTION_STRING,                      /* 09 */
    MODBUSREG_FILE_CSMON_PARAMETER_DESCRIPTION_AUXULIARY,                   /* 10 - CSMON_sFileParameterAux_t */
    MODBUSREG_FILE_CSMON_PARAMETER_ACCESS,                                  /* 11 - Access to Parameter trough file Read Write commands */
    MODBUSREG_FILE_CSMON_BASE_END,




    MODBUSREG_FILE_CSMON_PARAMETER_RECORDER_START = 0x20,                   /* 32 */
    MODBUSREG_FILE_CSMON_PARAMETER_RECORDER_CONFIGURATION = MODBUSREG_FILE_CSMON_PARAMETER_RECORDER_START,                  /* 32 */
    MODBUSREG_FILE_CSMON_PARAMETER_RECORDER_STATUS,                         /* 33 */
    MODBUSREG_FILE_CSMON_PARAMETER_RECORDER0_DATA_SAMPLES,                  /* 34 */     /* One Sample or Multiple Samples Read per Record. One sample read: 0..0xFFFF SampleIndexes. Two samples read: 0..0x1FFFE SampleIndexes ... */
    MODBUSREG_FILE_CSMON_PARAMETER_RECORDER1_DATA_SAMPLES,                  /* 35 */
    MODBUSREG_FILE_CSMON_PARAMETER_RECORDER2_DATA_SAMPLES,                  /* 36 */
    MODBUSREG_FILE_CSMON_PARAMETER_RECORDER_END,

    MODBUSREG_FILE_CSMON_PARAMETER_SCOPE_START = 0x30,                      /* 48 */
    MODBUSREG_FILE_CSMON_PARAMETER_SCOPE_CONFIGURATION = MODBUSREG_FILE_CSMON_PARAMETER_SCOPE_START,                        /* 48 */    /* was Record 0 -> Configuration; Record 1 -> ReadDataSample */
    MODBUSREG_FILE_CSMON_PARAMETER_SCOPE_STATUS,                            /* 49 */     /* was Record 0 -> Configuration; Record 1 -> ReadDataSample */
    MODBUSREG_FILE_CSMON_PARAMETER_SCOPE0_DATA_SAMPLES,                     /* 50 */     /* was not implemented - to clarify with Mladen -> Record 0 -> Configuration; Record 1 -> ReadDataSample */
    MODBUSREG_FILE_CSMON_PARAMETER_SCOPE1_DATA_SAMPLES,                     /* 51 */
    MODBUSREG_FILE_CSMON_PARAMETER_SCOPE2_DATA_SAMPLES,                     /* 52 */
    MODBUSREG_FILE_CSMON_PARAMETER_SCOPE_END,
    

            
    MODBUSREG_FILE_CSMON_COUNT = (MODBUSREG_FILE_CSMON_BASE_END - MODBUSREG_FILE_CSMON_BASE_START) + (MODBUSREG_FILE_CSMON_PARAMETER_RECORDER_END - MODBUSREG_FILE_CSMON_PARAMETER_RECORDER_START) + (MODBUSREG_FILE_CSMON_PARAMETER_SCOPE_END - MODBUSREG_FILE_CSMON_PARAMETER_SCOPE_START),
    #else
    MODBUSREG_FILE_CSMON_COUNT = 0,
    #endif
            
    MODBUSREG_FILE_COUNT = MODBUSREG_FILE_FLIR_COUNT + MODBUSREG_FILE_CSMON_COUNT,
    MODBUSREG_FILE_END = 0xFFFF
}MODBUSREG_eFileID;
        
typedef enum 
{
    #if USE_SPI_FLIR
    MODBUSREG_FILE_RECORD_SIZE_FLIR_I2C = 1,
    MODBUSREG_FILE_RECORD_SIZE_IRCAMERA_DEF = IR_CAMERA_DOTS_PER_LINE_COUNT,
    MODBUSREG_FILE_RECORD_SIZE_IRCAMERA_ZIP_DEF = 1,
           
    MODBUSREG_FILE_RECORD_SIZE_IRCAMERA_ZIP = 80*1,
    MODBUSREG_FILE_RECORD_SIZE_IRCAMERA_RAW = 80*2,
    MODBUSREG_FILE_RECORD_SIZE_IRCAMERA_RGB = 80*3,
    #endif

    MODBUSREG_FILE_RECORD_SIZE_DEFAULT = 0
}MODBUSREG_eFileRecordSize;
        
typedef enum 
{
    #if USE_SPI_FLIR
    MODBUSREG_FILE_RECORD_COUNT_FLIR_I2C = 0x1000,
    MODBUSREG_FILE_RECORD_COUNT_IRCAMERA_DEF_MAX = 60*3,
    MODBUSREG_FILE_RECORD_COUNT_IRCAMERA_DEF = 60,
            
    MODBUSREG_FILE_RECORD_COUNT_IRCAMERA_ZIP_DEF_MAX = 0x8000,           
            
    MODBUSREG_FILE_RECORD_COUNT_IRCAMERA_ZIP = 60,
    MODBUSREG_FILE_RECORD_COUNT_IRCAMERA_RAW = 60,
    MODBUSREG_FILE_RECORD_COUNT_IRCAMERA_RGB = 60,
    #endif
    MODBUSREG_FILE_RECORD_COUNT_DEFAULT = 0

}MODBUSREG_eFileRecordCount;       
        
#define MODBUSREG_FILE_TABLE_SIZE    MODBUSREG_FILE_COUNT






typedef enum 
{
    MODBUSREG_QUEUE_TEST_TABLE,
    MODBUSREG_QUEUE_RECORDER0,
    MODBUSREG_QUEUE_RECORDER1,
    MODBUSREG_QUEUE_RECORDER2,

    MODBUSREG_QUEUE_COUNT,
    MODBUSREG_QUEUE_END = 0xFFFF
}MODBUSREG_eQueueID;
        
#define MODBUSREG_QUEUE_TABLE_SIZE    MODBUSREG_QUEUE_COUNT



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


#endif	/* MODBUSREG_T_H */


#ifdef __cplusplus
}
#endif // __cplusplus
