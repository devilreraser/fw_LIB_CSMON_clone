/* *****************************************************************************
 * File:   hmdrv.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 01 02 19:56
 * 
 * Description: Hex Monitor Library MCU and Platform Dependent Driver
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef HMDRV_H
#define	HMDRV_H

/*****************************************************************************
* BOARD Definitions
******************************************************************************/

#include "hmdrvid.h"


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdint.h>/*  */
#include <stdlib.h>
#include <stdbool.h>
#ifndef _WIN32

//#if !defined(CPU1) && !defined(CPU2)
//#define CPU1
//#endif

#include "hmcfg.h"

#if HMDRV_USE_C2000_WARE_LIB
#if HMDRV_USE_C2000_WARE_LIB_DEVICE_SUPPORT
#include "F2806x_Device.h"
#else
#include "device.h"
#endif
#elif HMDRV_USE_CONTROL_SUITE_LIB
#include "F2806x_Device.h"
#endif

//#include "driverlib.h"    /* was device.h */
//#include "sci.h"    /* was device.h */
#endif // !1





/*****************************************************************************
* Project / Board configuration information 
******************************************************************************/


#ifndef HMDRV_BOARD
#define HMDRV_BOARD HMDRV_BOARD_COUNT
#endif



/*****************************************************************************
* External Memory And Virtual Address configuration information 
******************************************************************************/



#ifndef HMDRV_USE_EXT_MEM
#define HMDRV_USE_EXT_MEM       0
#endif

#ifndef HMDRV_USE_EXT_PMP
#define HMDRV_USE_EXT_PMP       0
#endif



/* TI Address Is Multiplied by 2 if used byte address. With this each byte in 16-bit variable can be addressed */
#ifndef HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY
#define HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY      0
#endif


/*****************************************************************************
* Board configuration information 
******************************************************************************/

#define HMDRV_PROT_VER              3      /* protocol version 3 */

#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY
#define HMDRV_CFG_BUS_WIDTH         1      /* data bus width */
#else
#define HMDRV_CFG_BUS_WIDTH         2      /* data bus width */
#endif

#define HMDRV_CFGFLAG               HMPROTOCOL_CFGFLAG_LITTLEENDIAN
#define HMDRV_GLOB_VERSION_MAJOR    2      /* driver version */
#define HMDRV_GLOB_VERSION_MINOR    0

#ifndef HMDRV_IDT_STRING
#define HMDRV_IDT_STRING            "Unknown"
#endif

#define HMDRV_TSA_FLAGS             0


#ifndef HMDRV_USE_MODBUS
#define HMDRV_USE_MODBUS            0
#endif

#ifndef HMDRV_MODBUS_BAUD
#define HMDRV_MODBUS_BAUD           9600
#endif

#ifndef HMDRV_HEXMON_BAUD
#define HMDRV_HEXMON_BAUD           9600
#endif

#ifndef HMLIB_HEXMON_UART_MODULE
#define HMLIB_HEXMON_UART_MODULE    0
#endif

#ifndef HMLIB_MODBUS_UART_MODULE
#define HMLIB_MODBUS_UART_MODULE    0
#endif

#ifndef HMDRV_SCI_BASE
#define HMDRV_SCI_BASE              SCIA_BASE
#endif


#ifndef HMDRV_PARITY_HEXMON
#define HMDRV_PARITY_HEXMON         SCI_CONFIG_PAR_NONE
#endif

#ifndef HMDRV_PARITY_MODBUS
#define HMDRV_PARITY_MODBUS         SCI_CONFIG_PAR_EVEN
#endif

#ifndef HMDRV_STOP_BITS
#define HMDRV_STOP_BITS             SCI_CONFIG_STOP_ONE
#endif









#define HMDRV_MODBUS_DEVICE_ADDRESS 0x01  
#define HMDRV_MODBUS_RESPONSE_TO_BROADCAST_MESSAGE    0
#define HMDRV_MODBUS_RESPONSE_TO_OTHER_ADDRESSES      0

#if (HMLIB_MODBUS_AND_HEXMON_UART_MODULE_DIFFERENT) && (HMLIB_MODBUS_DEBUG_RESPONSES)
#define HMDRV_MODBUS_RESEND_RECEIVED_DATA_ON_TIMEOUT_IF_NOT_COMPLETED_PREV_RECEIVE  1
#define HMDRV_MODBUS_RESEND_RECEIVED_DATA_ON_RECEIVE_BUFFER_OVERFLOW                1
#define HMDRV_MODBUS_RESEND_RECEIVED_DATA_ON_BAD_CS                                 1
#else
#define HMDRV_MODBUS_RESEND_RECEIVED_DATA_ON_TIMEOUT_IF_NOT_COMPLETED_PREV_RECEIVE  0
#define HMDRV_MODBUS_RESEND_RECEIVED_DATA_ON_RECEIVE_BUFFER_OVERFLOW                0
#define HMDRV_MODBUS_RESEND_RECEIVED_DATA_ON_BAD_CS                                 0
#endif

#define HMDRV_MODBUS_DO_NOT_RESET_ON_TIMEOUT_IF_NOT_COMPLETED_PREV_RECEIVE          0

#ifndef HMDRV_TX_DELAY
#define HMDRV_TX_DELAY  0
#endif

#ifndef HMDRV_MODBUS_TX_DELAY
#define HMDRV_MODBUS_TX_DELAY  1
#endif

#ifndef HMDRV_TX_DELAY_TIME
#define HMDRV_TX_DELAY_TIME       1000        /* microseconds */
#endif

#ifndef HMDRV_SCI_READ_MULTIPLE_RX_DATA
#define HMDRV_SCI_READ_MULTIPLE_RX_DATA 0
#endif

/*****************************************************************************
* HM Library Configuration
******************************************************************************/

#define HMDRV_USE_BRIEFINFO         0   /* brief board info not tested */





#ifndef HMDRV_LARGE_MODEL
#define HMDRV_LARGE_MODEL           0   /* keep 0 when 16-bit address used; set to 1 if address space exceeds 16-bits */
#endif

#define HMDRV_USE_EX_CMDS           HMDRV_LARGE_MODEL   /* extended address commands used if large model set */


#define HMDRV_USE_NOEX_CMDS         1   /* 16-bit address command used - set no use (0) if not needed - does almost nothing if disabled */


#ifndef HMDRV_USE_PROCESS_ALL_RECORDERS
#define HMDRV_USE_PROCESS_ALL_RECORDERS         1
#endif

#ifndef HMDRV_USE_RECORDER
#define HMDRV_USE_RECORDER          1   /* enable recorder */
#endif

#ifndef HMDRV_USE_RECORDER_PARAMETERS
#define HMDRV_USE_RECORDER_PARAMETERS          0   /* disable recorder */
#endif

#ifndef HMDRV_FLOAT_TRIG_ENABLE
#define HMDRV_FLOAT_TRIG_ENABLE             0
#endif



#ifndef HMDRV_USE_SCOPE
#define HMDRV_USE_SCOPE             1   /* enable scope */
#endif

#ifndef HMDRV_USE_SCOPE_PARAMETERS
#define HMDRV_USE_SCOPE_PARAMETERS             0   /* disable scope */
#endif

#ifndef HMDRV_HMSCOPEPRM_BUFFER_SIZE
#define HMDRV_HMSCOPEPRM_BUFFER_SIZE 256
#endif

#ifndef HMDRV_VALID_SCOPE_DATA_ONLY
#define HMDRV_VALID_SCOPE_DATA_ONLY 1   /* Use Valid Data flag In Scope */
#endif

#ifndef HMDRV_USE_SERIAL
#define HMDRV_USE_SERIAL            1   /* enable serial communication */
#endif

#ifndef HMDRV_USE_FREEMASTER
#define HMDRV_USE_FREEMASTER        1
#endif


#define HMDRV_USE_READMEM           1    /* enable read memory commands */
#define HMDRV_USE_WRITEMEM          1    /* enable write memory commands */
#define HMDRV_USE_WRITEMEMMASK      1    /* enable write memory bits commands */

#ifndef HMDRV_DISABLE
#define HMDRV_DISABLE               0
#endif

#ifndef HMDRV_COMMON_ERR_CODES
#define HMDRV_COMMON_ERR_CODES      0
#endif

#ifndef HMDRV_SCI_TWOWIRE_ONLY
#define HMDRV_SCI_TWOWIRE_ONLY      0
#endif

#ifndef HMDRV_SCI_RX_ISR
#define HMDRV_SCI_RX_ISR            0   /* default no use ISR if Available */
#endif

#ifndef HMDRV_SCI_TX_ISR
#define HMDRV_SCI_TX_ISR            0   /* default no use ISR if Available */
#endif

#ifndef HMDRV_SCI_TX_DELAY_ISR
#define HMDRV_SCI_TX_DELAY_ISR      0   /* default no use ISR if Available */
#endif


#if HMDRV_SCI_TX_ISR
#define HMDRV_TE_MIN_TIME      1   /* default TE minimum possible time enabled if Tx ISR Used  */
#endif

#ifndef HMDRV_TE_MIN_TIME
#define HMDRV_TE_MIN_TIME      0   /* default TE minimum possible time disabled */
#endif




/*****************************************************************************
* Recorder support
******************************************************************************/

#ifndef HMDRV_RECORDER_HANDLE_COUNT
#define HMDRV_RECORDER_HANDLE_COUNT 1
#endif

#ifndef HMDRV_RECORDER_BUFINT_COUNT
#define HMDRV_RECORDER_BUFINT_COUNT 1   /* Used Module Internal Buffers Count */ 
#endif
        
#ifndef HMDRV_RECORDER_BUFFER_COUNT
#define HMDRV_RECORDER_BUFFER_COUNT 1
#endif

#ifndef HMDRV_USE_BOARD_WITHOUT_EXT_MEM
#define HMDRV_USE_BOARD_WITHOUT_EXT_MEM 0
#endif




#if HMDRV_USE_EXT_MEM
        
#if HMDRV_USE_BOARD_WITHOUT_EXT_MEM
#define HMDRV_RECORDER_USED_FREEMASTER   0   /* Recorder (0, 1, 2) Used in Freemaster Protocol Decoder (0 if used board without External Ram) */
#else
#define HMDRV_RECORDER_USED_FREEMASTER   1   /* Recorder (0, 1, 2) Used in Freemaster Protocol Decoder */
#endif

#ifndef HMDRV_RECORDER_BUFFER0_SIZE
#define HMDRV_RECORDER_BUFFER0_SIZE 0x0800      /* Size In bytes (2kB) */
#endif

#ifndef HMDRV_RECORDER_BUFFER1_SIZE
#define HMDRV_RECORDER_BUFFER1_SIZE 0xFFFF      /* Size In bytes (Freemaster supports only 16-bits size) */
#endif

#ifndef HMDRV_RECORDER_BUFFER2_SIZE
#if HMDRV_USE_EXT_PMP
//#define HMDRV_RECORDER_BUFFER2_SIZE 0x1000      /* Size In bytes (Freemaster supports only 16-bits size) */
#define HMDRV_RECORDER_BUFFER2_SIZE EXT_MEM_PMP_SIZE      /* Size In bytes (Freemaster supports only 16-bits size) */
#else
#define HMDRV_RECORDER_BUFFER2_SIZE 0xFFFF      /* Size In bytes (Freemaster supports only 16-bits size) */
#endif
#endif

#define HMDRV_RECORDER_BUFFER0_ADDR (HMDRV_ADDR)(HMDRV_ADDR_SIZE)&au8RecBuffer0[0]

#define HMDRV_RECORDER_BUFFER1_ADDR (HMDRV_ADDR)(EXT_MEM_RAM_VIRTUAL_ADDRESS)

#if HMDRV_USE_EXT_PMP
#define HMDRV_RECORDER_BUFFER2_ADDR (HMDRV_ADDR)(EXT_MEM_PMP_VIRTUAL_ADDRESS)
#else
#define HMDRV_RECORDER_BUFFER2_ADDR (HMDRV_ADDR)(EXT_MEM_RAM_VIRTUAL_ADDRESS + HMDRV_RECORDER_BUFFER1_SIZE)
#endif


#else   /* Without External Memory Default one Recorder usage */

#ifndef HMDRV_RECORDER_USED_FREEMASTER
#define HMDRV_RECORDER_USED_FREEMASTER   0   /* Recorder (0, 1, 2) Used in Freemaster Protocol Decoder */
#endif

#ifndef HMDRV_RECORDER_BUFFER0_SIZE
#define HMDRV_RECORDER_BUFFER0_SIZE 0x0800      /* Size In bytes (2kB) */
#endif

#ifndef HMDRV_RECORDER_BUFFER1_SIZE
#define HMDRV_RECORDER_BUFFER1_SIZE 0xFFFF      /* Size In bytes (Freemaster supports only 16-bits size) */
#endif

#ifndef HMDRV_RECORDER_BUFFER2_SIZE
#define HMDRV_RECORDER_BUFFER2_SIZE 0xFFFF      /* Size In bytes (Freemaster supports only 16-bits size) */
#endif

#ifndef HMDRV_RECORDER_BUFFER0_ADDR
#define HMDRV_RECORDER_BUFFER0_ADDR (HMDRV_ADDR)(HMDRV_ADDR_SIZE)&au8RecBuffer0[0]
#endif

#ifndef HMDRV_RECORDER_BUFFER1_ADDR
#define HMDRV_RECORDER_BUFFER1_ADDR (HMDRV_ADDR)(HMDRV_ADDR_SIZE)BOARDCFG_EMIF_START_ADDRESS
#endif

#ifndef HMDRV_RECORDER_BUFFER2_ADDR
#define HMDRV_RECORDER_BUFFER2_ADDR (HMDRV_ADDR)(HMDRV_ADDR_SIZE)&au8RecBuffer0[0]
#endif


/*
#define HMDRV_RECORDER_BUFFER1_ADDR (HMDRV_ADDR)(HMDRV_ADDR_SIZE)&au8RecBuffer1[0]
#define HMDRV_RECORDER_BUFFER2_ADDR (HMDRV_ADDR)0x80000000
*/



#endif


#ifndef HMDRV_USE_WITH_FREEMASTER_DECODER
#define HMDRV_USE_WITH_FREEMASTER_DECODER 0
#endif




#ifndef HMDRV_MAX_REC_VARS
#define HMDRV_MAX_REC_VARS  20  //consider 8
#endif



/*****************************************************************************
* Scope
******************************************************************************/

#ifndef HMDRV_MAX_SCOPE_VARS
#define HMDRV_MAX_SCOPE_VARS  20  //consider 8
#endif







/*****************************************************************************
* Recorder Parameters support
******************************************************************************/

#ifndef HMDRV_RECORDER_PARAM_HANDLE_COUNT
#define HMDRV_RECORDER_PARAM_HANDLE_COUNT 1
#endif

#ifndef HMDRV_RECORDER_PARAM_BUFINT_COUNT
#define HMDRV_RECORDER_PARAM_BUFINT_COUNT 3   /* Used Module Internal Buffers Count */
#endif
        
#ifndef HMDRV_RECORDER_PARAM_BUFFER_COUNT
#define HMDRV_RECORDER_PARAM_BUFFER_COUNT 1
#endif

#ifndef HMDRV_RECORDER_PARAM_USES_FREEMASTER_COMMUNICATION
#define HMDRV_RECORDER_PARAM_USES_FREEMASTER_COMMUNICATION 0    /* to do: implement if needed */
#endif

#ifndef HMDRV_RECORDER_TIME_BASE_COUNT
#define HMDRV_RECORDER_TIME_BASE_COUNT 1
#endif




#if HMDRV_USE_EXT_MEM
        

#define HMDRV_RECORDER_PARAM_USED_FREEMASTER   1   /* Recorder (0, 1, 2) Used in Freemaster Protocol Decoder */

#ifndef HMDRV_RECORDER_PARAM_BUFFER0_SIZE
#define HMDRV_RECORDER_PARAM_BUFFER0_SIZE 0x0800      /* Size In bytes (2kB) */
#endif

#ifndef HMDRV_RECORDER_PARAM_BUFFER1_SIZE
#define HMDRV_RECORDER_PARAM_BUFFER1_SIZE_FREEMASTER    0xFFFF       /* Size In bytes (compatible with Freemaster supports only 16-bits size) 512kB(0x80000) leave 0xFFFF+1 for HMDRV_RECORDER_BUFFER1 */
#define HMDRV_RECORDER_PARAM_BUFFER1_SIZE               (HMDRV_SIZE_FLEX)0x70000      /* Size In bytes (compatible with Freemaster supports only 16-bits size) 512kB(0x80000) leave 0xFFFF+1 for HMDRV_RECORDER_BUFFER1 */
#endif

#ifndef HMDRV_RECORDER_PARAM_BUFFER2_SIZE
#if HMDRV_USE_EXT_PMP
//#define HMDRV_RECORDER_PARAM_BUFFER2_SIZE 0x1000      /* Size In bytes (Freemaster supports only 16-bits size) */
#define HMDRV_RECORDER_PARAM_BUFFER2_SIZE EXT_MEM_PMP_SIZE      /* Size In bytes (Freemaster supports only 16-bits size) */
#else
#define HMDRV_RECORDER_PARAM_BUFFER2_SIZE 0xFFFF      /* Size In bytes (Freemaster supports only 16-bits size) */
#endif
#endif

#define HMDRV_RECORDER_PARAM_BUFFER0_ADDR (HMDRV_ADDR)(HMDRV_ADDR_SIZE)&au8RecParamBuffer0[0]

#define HMDRV_RECORDER_PARAM_BUFFER1_ADDR (HMDRV_ADDR)(EXT_MEM_RAM_VIRTUAL_ADDRESS + 0x000 )  //Buffer memory location is after HMDRV_RECORDER_BUFFER1

#if HMDRV_USE_EXT_PMP
#define HMDRV_RECORDER_PARAM_BUFFER2_ADDR (HMDRV_ADDR)(EXT_MEM_PMP_VIRTUAL_ADDRESS)
#else
#define HMDRV_RECORDER_PARAM_BUFFER2_ADDR (HMDRV_ADDR)(EXT_MEM_RAM_VIRTUAL_ADDRESS + HMDRV_RECORDER_PARAM_BUFFER1_SIZE)
#endif


#else   /* Without External Memory Default one Recorder usage */


#define HMDRV_RECORDER_PARAM_USED_FREEMASTER   0   /* Recorder (0, 1, 2) Used in Freemaster Protocol Decoder */

#ifndef HMDRV_RECORDER_PARAM_BUFFER0_SIZE
#define HMDRV_RECORDER_PARAM_BUFFER0_SIZE 0x0800      /* Size In bytes (2kB) */
#endif

#ifndef HMDRV_RECORDER_PARAM_BUFFER1_SIZE
#define HMDRV_RECORDER_PARAM_BUFFER1_SIZE_FREEMASTER    0x0800       /* Size In bytes (compatible with Freemaster supports only 16-bits size) 512kB(0x80000) leave 0xFFFF+1 for HMDRV_RECORDER_BUFFER1 */
#define HMDRV_RECORDER_PARAM_BUFFER1_SIZE 0x0800      /* Size In bytes (Freemaster supports only 16-bits size) */
#endif

#ifndef HMDRV_RECORDER_PARAM_BUFFER2_SIZE
#define HMDRV_RECORDER_PARAM_BUFFER2_SIZE 0x0800      /* Size In bytes (Freemaster supports only 16-bits size) */
#endif

#ifndef HMDRV_RECORDER_PARAM_BUFFER0_ADDR
#define HMDRV_RECORDER_PARAM_BUFFER0_ADDR (HMDRV_ADDR)(HMDRV_ADDR_SIZE)&au8RecParamBuffer0[0]
#endif

#ifndef HMDRV_RECORDER_PARAM_BUFFER1_ADDR
#define HMDRV_RECORDER_PARAM_BUFFER1_ADDR (HMDRV_ADDR)(HMDRV_ADDR_SIZE)&au8RecParamBuffer1[0]
#endif

#ifndef HMDRV_RECORDER_PARAM_BUFFER2_ADDR
#define HMDRV_RECORDER_PARAM_BUFFER2_ADDR (HMDRV_ADDR)(HMDRV_ADDR_SIZE)&au8RecParamBuffer2[0]
#endif

/*
#define HMDRV_RECORDER_PARAM_BUFFER1_ADDR (HMDRV_ADDR)(HMDRV_ADDR_SIZE)&au8RecParamBuffer1[0]
#define HMDRV_RECORDER_PARAM_BUFFER2_ADDR (HMDRV_ADDR)0x80000000
*/



#endif


#ifndef HMDRV_USE_WITH_FREEMASTER_DECODER
#define HMDRV_USE_WITH_FREEMASTER_DECODER 0
#endif




#ifndef HMDRV_MAX_REC_PARAM_VARS
#define HMDRV_MAX_REC_PARAM_VARS  20
#endif



/*****************************************************************************
* Scope Parameters
******************************************************************************/


#ifndef HMDRV_SCOPE_PARAM_USES_FREEMASTER_COMMUNICATION
#define HMDRV_SCOPE_PARAM_USES_FREEMASTER_COMMUNICATION 0    /* to do: implement if needed */
#endif

#ifndef HMDRV_MAX_SCOPE_PARAM_VARS              
#define HMDRV_MAX_SCOPE_PARAM_VARS  20          
#endif








/*****************************************************************************
* Communication Buffer Size
******************************************************************************/

/* automatic buffer size by default */
#ifndef HMDRV_MODBUS_BUFFER_SIZE
#define HMDRV_MODBUS_BUFFER_SIZE 0
#endif

#if !HMDRV_MODBUS_BUFFER_SIZE
    #undef  HMDRV_MODBUS_BUFFER_SIZE
    #define HMDRV_MODBUS_BUFFER_SIZE 256
#endif




/* automatic buffer size by default */
#ifndef HMDRV_COMM_BUFFER_SIZE
#define HMDRV_COMM_BUFFER_SIZE 0
#endif

/* automatic: determine required buffer size based on features enabled */
#if !HMDRV_COMM_BUFFER_SIZE
    /* smallest for basic commands (get info brief, write/read memory etc.) */
    #undef  HMDRV_COMM_BUFFER_SIZE
    #define HMDRV_COMM_BUFFER_SIZE 11

    /* full info required */
    #if !(HMDRV_USE_BRIEFINFO) && HMDRV_COMM_BUFFER_SIZE < 35
    #undef  HMDRV_COMM_BUFFER_SIZE
    #define HMDRV_COMM_BUFFER_SIZE 35
    #endif

    /* using application commands (must accommodate maximal app. cmd data length) */
    #if HMDRV_USE_APPCMD && HMDRV_COMM_BUFFER_SIZE < ((HMDRV_APPCMD_BUFF_SIZE)+1)
    #undef  HMDRV_COMM_BUFFER_SIZE
    #define HMDRV_COMM_BUFFER_SIZE ((HMDRV_APPCMD_BUFF_SIZE)+1)
    #endif

    /* configuring scope (EX) */
    #if HMDRV_USE_SCOPE && HMDRV_COMM_BUFFER_SIZE < ((HMDRV_MAX_SCOPE_VARS)*5+1)
    #undef  HMDRV_COMM_BUFFER_SIZE
    #define HMDRV_COMM_BUFFER_SIZE ((HMDRV_MAX_SCOPE_VARS)*5+1)
    #endif

    /* configuring scope (EX) */
    #if HMDRV_SCOPE_PARAM_USES_FREEMASTER_COMMUNICATION
    #if HMDRV_USE_SCOPE_PARAMETERS && HMDRV_COMM_BUFFER_SIZE < ((HMDRV_MAX_SCOPE_PARAM_VARS)*5+1)
    #undef  HMDRV_COMM_BUFFER_SIZE
    #define HMDRV_COMM_BUFFER_SIZE ((HMDRV_MAX_SCOPE_PARAM_VARS)*5+1)
    #endif
    #endif

    /* configuring recorder (EX) */
    #if HMDRV_USE_RECORDER && HMDRV_COMM_BUFFER_SIZE < ((HMDRV_MAX_REC_VARS)*5+18)
    #undef  HMDRV_COMM_BUFFER_SIZE
    #define HMDRV_COMM_BUFFER_SIZE ((HMDRV_MAX_REC_VARS)*5+18)
    #endif

    /* configuring recorder (EX) */
    #if HMDRV_RECORDER_PARAM_USES_FREEMASTER_COMMUNICATION
    #if HMDRV_USE_RECORDER_PARAMETERS && HMDRV_COMM_BUFFER_SIZE < ((HMDRV_MAX_REC_PARAM_VARS)*5+18)
    #undef  HMDRV_COMM_BUFFER_SIZE
    #define HMDRV_COMM_BUFFER_SIZE ((HMDRV_MAX_REC_PARAM_VARS)*5+18)
    #endif
    #endif

    /* SFIO encapsulation (in buffer) */
    #if HMDRV_USE_SFIO
    #if HMDRV_COMM_BUFFER_SIZE < ((SFIO_MAX_INPUT_DATA_LENGTH)+1)
    #undef  HMDRV_COMM_BUFFER_SIZE
    #define HMDRV_COMM_BUFFER_SIZE ((SFIO_MAX_INPUT_DATA_LENGTH)+1)
    #endif
    #endif
    
    /* SFIO encapsulation (out buffer) */
    #if HMDRV_USE_SFIO
    #if HMDRV_COMM_BUFFER_SIZE < ((SFIO_MAX_OUTPUT_DATA_LENGTH)+1)
    #undef  HMDRV_COMM_BUFFER_SIZE
    #define HMDRV_COMM_BUFFER_SIZE ((SFIO_MAX_OUTPUT_DATA_LENGTH)+1)
    #endif
    #endif
    
#endif



/*****************************************************************************
* Extended Address Commands Check and Error Directives Generation
******************************************************************************/

/* at least one of EX or no-EX command handling must be enabled */
#if !(HMDRV_USE_EX_CMDS) && !(HMDRV_USE_NOEX_CMDS)
    #error At least one of EX or no-EX command handling must be enabled (please set HMDRV_USE_NOEX_CMDS)
    #undef  HMDRV_USE_NOEX_CMDS
    #define HMDRV_USE_NOEX_CMDS 1
#endif

/* in large model, the 32bit addresses must be enabled */
#if (HMDRV_LARGE_MODEL) && !(HMDRV_USE_EX_CMDS)
    #error You must enable HMDRV_USE_EX_CMDS when using large memory model (when HMDRV_LARGE_MODEL is set)
    #undef  HMDRV_USE_EX_CMDS
    #define HMDRV_USE_EX_CMDS 1
#endif

/* when 32bit addresses are enabled, the large model must be used */
#if !(HMDRV_LARGE_MODEL) && (HMDRV_USE_EX_CMDS)
    #error You must enable HMDRV_LARGE_MODEL when EX commands are enabled (when HMDRV_USE_EX_CMDS is set)
#endif




/******************************************************************************
* platform-specific modifiers
******************************************************************************/
#ifndef _WIN32
#define HMDRV_FILE_PNTR_MODIFIER
#define HMDRV_ADDR_PNTR_MODIFIER
#define HMDRV_BUFF_PNTR_MODIFIER
#define HMDRV_CNST_PNTR_MODIFIER
#else
#define HMDRV_FILE_PNTR_MODIFIER
#define HMDRV_ADDR_PNTR_MODIFIER
#define HMDRV_BUFF_PNTR_MODIFIER
#define HMDRV_CNST_PNTR_MODIFIER
#endif // !_WIN32


/******************************************************************************
* enumerations
******************************************************************************/

typedef enum
{
    HMDRV_ELEMENT_SIZE_STRING = 0,
    HMDRV_ELEMENT_SIZE_BYTE,
    HMDRV_ELEMENT_SIZE_SHORT,
    HMDRV_ELEMENT_SIZE_24_BIT,
    HMDRV_ELEMENT_SIZE_LONG,
    HMDRV_ELEMENT_SIZE_40_BIT,
    HMDRV_ELEMENT_SIZE_48_BIT,
    HMDRV_ELEMENT_SIZE_56_BIT,
    HMDRV_ELEMENT_SIZE_LONG_LONG,

    HMDRV_ELEMENT_MASK_STRING = 0x80,
     
    HMDRV_ELEMENT_SIZE_MAX = 0xFF        
}HMDRV_eElementSize_t;



/******************************************************************************
* platform-specific types
******************************************************************************/

typedef unsigned char  HMDRV_U8;        /* smallest memory entity */
typedef unsigned short HMDRV_U16;       /* 16bit value */
typedef unsigned long  HMDRV_U32;       /* 32bit value */
typedef unsigned long long  HMDRV_U64;  /* 64bit value */

typedef signed char    HMDRV_S8;        /* signed 8bit value */
typedef signed short   HMDRV_S16;       /* signed 16bit value */
typedef signed long    HMDRV_S32;       /* signed 32bit value */
typedef signed long long  HMDRV_S64;    /* 64bit value */

/*****************************************************************************
* Global types
******************************************************************************/
#ifndef _WIN32
#if HMDRV_USE_EXT_MEM == 0
typedef unsigned char*  HMDRV_ADDR;      /* MCU address type (1,2,3,4 bytes) */
typedef unsigned long   HMDRV_ADDR_SIZE;        /* Internal use mcu address type when global address different size */
typedef unsigned long   HMDRV_ADDR_SIZE_LONG;   /* Internal use mcu address type when global address different size */
typedef unsigned short  HMDRV_ADDR_SIZE_SHORT;  /* size of unsigned char* */
typedef unsigned long   HMDRV_ADDR_TO_PNTR_SIZE; /* Size of Type Cast From ADDR to Internal RAM PNTR (without modifiers for Extended Data Space) */
#else
typedef unsigned long   HMDRV_ADDR;      /* MCU address type (1,2,3,4 bytes) */
typedef unsigned short  HMDRV_ADDR_SIZE;        /* Internal use mcu address type when global address different size */
typedef unsigned long   HMDRV_ADDR_SIZE_LONG;   /* Internal use mcu address type when global address different size */
typedef unsigned short  HMDRV_ADDR_SIZE_SHORT;  /* size of unsigned char* */
typedef unsigned long   HMDRV_ADDR_TO_PNTR_SIZE; /* Size of Type Cast From ADDR to Internal RAM PNTR (without modifiers for Extended Data Space) */
#endif
#else
typedef unsigned long long   HMDRV_ADDR;      /* MCU address type (8 bytes) */
typedef unsigned long long HMDRV_ADDR_SIZE;        /* Internal use mcu address type when global address different size */
#endif // !_WIN32


//typedef unsigned short  HMDRV_PTR_FUNC_SIZE;
typedef unsigned long  HMDRV_PTR_FUNC_SIZE;

typedef unsigned long HMDRV_SIZE_FLEX;         /* flexible size type (at least 16 bits) */

typedef unsigned short HMDRV_SIZE;              /* general size type (at least 16 bits) */
typedef unsigned char  HMDRV_BOOL;              /* general boolean type */

#define HMDRV_RECBUFF_BYTE_SIZE 1               /* Byte Size For the Recorder Buffer */

typedef float          HMDRV_FLOAT;     /* float value */

typedef unsigned char  HMDRV_BANK;      /* bank of the memory space */

typedef unsigned char  HMDRV_FLAGS;     /* type to be union-ed with flags (at least 8 bits) */
typedef unsigned char  HMDRV_SIZE8;     /* one-byte size value */
typedef signed short   HMDRV_INDEX;     /* general for-loop index (must be signed) */

typedef unsigned char  HMDRV_BCHR;      /* type of a single character in comm.buffer */

typedef unsigned short  HMDRV_CRC;      /* type of a checksum */

typedef unsigned char* HMDRV_BPTR;      /* pointer within a communication buffer */


typedef unsigned long HMDRV_TIMER;      /* timer type */


typedef unsigned char  HMDRV_SCISR;     /* data type to store SCI status register */

typedef unsigned char  HMDRV_MESSAGESR;     /* data type to store message status register */



typedef struct
{
    HMDRV_U8 u7ValueOffset  : 7;
    HMDRV_U8 bBitField      : 1;
}HMDRV_sVariableOffset_t;
typedef struct
{
    HMDRV_U8 u7ValueSize    : 7;
    HMDRV_U8 bBitField      : 1;
}HMDRV_sVariableSize_t;

typedef union
{
    HMDRV_U8 u8Register;
    HMDRV_sVariableOffset_t sOffset;
    HMDRV_sVariableSize_t sSize;
}HMDRV_uVariableSizeOffset_t;


/****************************************************************************************
* Other helper macros
*****************************************************************************************/

/* This macro assigns C pointer to HMDRV_ADDR-typed variable */
#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY == 1      /* 16-bit memory address multiplied with 2 to use byte address */
#define HMDRV_PTR2ADDR(pAddr,pArray) ( pAddr = (HMDRV_ADDR) ((HMDRV_U32)((HMDRV_U8*) pArray) << 1) )
#define HMDRV_ADDRFROMPTR(ptr)  (HMDRV_ADDR) ((HMDRV_U32)((HMDRV_U8*)ptr) << 1)
#define HMDRV_PTRFROMADDR(addr) (HMDRV_BPTR) (HMDRV_ADDR_TO_PNTR_SIZE) ((HMDRV_U32)((HMDRV_ADDR)addr) >> 1)
#define HMDRV_SIZEADDRINTBYTES(size)    ((size) << 1)
#else
#define HMDRV_PTR2ADDR(pAddr,pArray) ( pAddr = pArray )
#define HMDRV_ADDRFROMPTR(ptr)  ptr
#define HMDRV_SIZEADDRINTBYTES(size)    size
#endif
#define HMDRV_ARR2ADDR HMDRV_PTR2ADDR

/* This macro assigns C pointer to HMDRV_SIZE-typed variable */
#define HMDRV_ARR2SIZE(nSize,nArrSz) ( nSize = (HMDRV_SIZE) nArrSz )
#define HMDRV_ARR2SIZEFLEX(nSize,nArrSz) ( nSize = (HMDRV_SIZE_FLEX) nArrSz )


#define HMDRV_RECINDEX2PTR(nIndex)   ( &sRecHandle[nIndex] )

#define HMDRV_RECPRMINDEX2PTR(nIndex)   ( &sRecHandleParam[nIndex] )


/*********************************************************************************
* communication buffer access functions
*********************************************************************************/

#define HMDRV_ValueFromBuffer8(pDest, pSrc) \
    ( (*((HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8*)(pDest)) = *(HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8*)(pSrc+0)), (((HMDRV_BPTR)(pSrc))+1) )

#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY == 1      /* 16-bit memory address multiplied with 2 to use byte address */
#define HMDRV_ValueFromBuffer16(pDest, pSrc) \
    ( (*((HMDRV_U16*)(pDest)) = (*(HMDRV_U8*)(pSrc)) + ((*(HMDRV_U8*)(pSrc+1))<<8)), (((HMDRV_BPTR)(pSrc))+2) )
#else
#define HMDRV_ValueFromBuffer16(pDest, pSrc) \
    ( (*((HMDRV_U16*)(pDest)) = (HMDRV_U16)(*(HMDRV_U8*)(pSrc+0)) + ((HMDRV_U16)(*(HMDRV_U8*)(pSrc+1))<<8)), (HMDRV_BPTR)(((HMDRV_BPTR)(pSrc))+2) )
#endif

#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY == 1      /* 16-bit memory address multiplied with 2 to use byte address */
#define HMDRV_ValueFromBuffer32(pDest, pSrc) \
    ( (*((HMDRV_U32*)(pDest)) =  (HMDRV_U32)(*(HMDRV_U8*)(pSrc)) + ( (HMDRV_U32)(*(HMDRV_U8*)(pSrc+1))<<8) + ((HMDRV_U32)(*(HMDRV_U8*)(pSrc+1))<<16) + ((HMDRV_U32)(*(HMDRV_U8*)(pSrc+1))<<24)), (((HMDRV_BPTR)(pSrc))+4) )
#else
#define HMDRV_ValueFromBuffer32(pDest, pSrc) \
    ( (*((HMDRV_U32*)(pDest)) = (HMDRV_U32)(*(HMDRV_U8*)(pSrc+0)) + ((HMDRV_U32)(*(HMDRV_U8*)(pSrc+1))<<8) + ((HMDRV_U32)(*(HMDRV_U8*)(pSrc+2))<<16) + ((HMDRV_U32)(*(HMDRV_U8*)(pSrc+3))<<24)), (((HMDRV_BPTR)(pSrc))+4) )
#endif

#define HMDRV_ValueFromBuffer16BigEndian(pDest, pSrc) \
    ( (*((HMDRV_U16*)(pDest)) = (HMDRV_U16)(*(HMDRV_U8*)(pSrc+1)) + ((HMDRV_U16)(*(HMDRV_U8*)(pSrc+0))<<8)), (((HMDRV_BPTR)(pSrc))+2) )
	

#define HMDRV_ValueToBuffer8(pDest, src) \
    ( (*((HMDRV_U8*)(pDest)) = (HMDRV_U8)(src)), (((HMDRV_BPTR)(pDest))+1) )

#define HMDRV_ValueToBuffer16(pDest, src) \
   ( (*((HMDRV_U8*)(pDest+0)) = (HMDRV_U8)((HMDRV_U8)src & 0x00FF)), (*((HMDRV_U8*)(pDest+1)) = (HMDRV_U8)(src>>8)), (((HMDRV_BPTR)(pDest))+2))

#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY == 1      /* 16-bit memory address multiplied with 2 to use byte address */
#define HMDRV_ValueToBuffer32(pDest, src) \
   ( (*((HMDRV_U8*)(pDest)) = (HMDRV_U8)((HMDRV_U8)src & 0x00FF)), (*((HMDRV_U8*)(pDest+1)) = (HMDRV_U8)((HMDRV_U8)(src>>8) & 0x00FF)), (*((HMDRV_U8*)(pDest+2)) = (HMDRV_U8)((HMDRV_U8)(src>>16) & 0x00FF)), (*((HMDRV_U8*)(pDest+3)) = (HMDRV_U8)(src>>24)), (((HMDRV_BPTR)(pDest))+4))
#else
#define HMDRV_ValueToBuffer32(pDest, src) \
   ( (*((HMDRV_U8*)(pDest+0)) = (HMDRV_U8)((HMDRV_U8)src & 0x00FF)), (*((HMDRV_U8*)(pDest+1)) = (HMDRV_U8)((HMDRV_U8)(src>>8) & 0x00FF)), (*((HMDRV_U8*)(pDest+2)) = (HMDRV_U8)((HMDRV_U8)(src>>16) & 0x00FF)), (*((HMDRV_U8*)(pDest+3)) = (HMDRV_U8)(src>>24)), (((HMDRV_BPTR)(pDest))+4))
#endif

#define HMDRV_ValueToBuffer16BigEndian(pDest, src) \
   ( (*((HMDRV_U8*)(pDest+1)) = (HMDRV_U8)((HMDRV_U8)src & 0x00FF)), (*((HMDRV_U8*)(pDest+0)) = (HMDRV_U8)(src>>8)), (((HMDRV_BPTR)(pDest))+2))


#define HMDRV_SkipInBuffer(pDest, nSize) \
    ( ((HMDRV_BPTR)(pDest)) + (nSize) )


#define HMDRV_ConstToBuffer8  HMDRV_ValueToBuffer8
#define HMDRV_ConstToBuffer16 HMDRV_ValueToBuffer16


/* Address fetching is different in small and large data models. In large model 
  (or when EX and no-EX commands mey be mixed), we must handle the addresses
  in functions (translate addresses from logical to global format if needed). */
#if HMDRV_LARGE_MODEL 
    HMDRV_BPTR HMDRV_AddressFromBuffer(HMDRV_ADDR_PNTR_MODIFIER HMDRV_ADDR* pAddr, HMDRV_BPTR pSrc);
    HMDRV_BPTR HMDRV_AddressFromBufferBigEndian(HMDRV_ADDR_PNTR_MODIFIER HMDRV_ADDR* pAddr, HMDRV_BPTR pSrc);
    HMDRV_BPTR HMDRV_AddressToBuffer(HMDRV_BPTR pDest, HMDRV_ADDR nAddr);
    HMDRV_BPTR HMDRV_AddressToBufferBigEndian(HMDRV_BPTR pDest, HMDRV_ADDR nAddr);
#else


#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY == 1      /* 16-bit memory address multiplied with 2 to use byte address */
#define HMDRV_AddressFromBuffer(pDest, pSrc) \
    ( (*((HMDRV_ADDR*)(pDest)) = *(HMDRV_ADDR*)(pSrc)), (((HMDRV_BPTR)(pSrc))+2) )
#else
#define HMDRV_AddressFromBuffer(pAddr, pSrc) \
    ( (*((HMDRV_ADDR*)(pAddr)) = (HMDRV_ADDR)((HMDRV_U16)(*(HMDRV_U8*)(pSrc)) + ((HMDRV_U16)(*(HMDRV_U8*)(pSrc+1))<<8))), (((HMDRV_BPTR)(pSrc))+2) )
#endif

#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY == 1      /* 16-bit memory address multiplied with 2 to use byte address */
#define HMDRV_AddressToBuffer(pDest, nAddr) \
    ( (*((HMDRV_ADDR*)(pDest)) = (HMDRV_ADDR)(nAddr)), (((HMDRV_BPTR)(pDest))+2) )
#else
#define HMDRV_AddressToBuffer(pDest, nAddr) \
    ( (*((HMDRV_U8*)((HMDRV_U16)pDest)) = (HMDRV_U8)((HMDRV_U16)nAddr & 0x00FF)), (*((HMDRV_U8*)((HMDRV_U16)pDest+1)) = (HMDRV_U8)((HMDRV_U8)((HMDRV_U16)nAddr>>8) & 0x00FF)), (HMDRV_BPTR)(((HMDRV_BPTR)((HMDRV_U16)pDest))+2) )
#endif



#endif

#define HMRDV_FixEdsAddress(x) (x)


#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY == 1      /* 16-bit memory address multiplied with 2 to use byte address */
#define HMDRV_GetS8(addr)       ( *(HMDRV_S8*   )((HMDRV_U32)addr >> 1) )
#define HMDRV_GetU8(addr)       ( *(HMDRV_U8*   )((HMDRV_U32)addr >> 1) )
#define HMDRV_GetS16(addr)      ( *(HMDRV_S16*  )((HMDRV_U32)addr >> 1) )
#define HMDRV_GetU16(addr)      ( *(HMDRV_U16*  )((HMDRV_U32)addr >> 1) )
#define HMDRV_GetS32(addr)      ( *(HMDRV_S32*  )((HMDRV_U32)addr >> 1) )
#define HMDRV_GetU32(addr)      ( *(HMDRV_U32*  )((HMDRV_U32)addr >> 1) )
#define HMDRV_GetFloat(addr)    ( *(HMDRV_FLOAT*)((HMDRV_U32)addr >> 1) )
#else
#define HMDRV_GetS8(addr)       ( *(HMDRV_S8*)(HMDRV_ADDR_SIZE)(addr) )
#define HMDRV_GetU8(addr)       ( *(HMDRV_U8*)(HMDRV_ADDR_SIZE)(addr) )
#define HMDRV_GetS16(addr)      ( *(HMDRV_S16*)(HMDRV_ADDR_SIZE)(addr) )
#define HMDRV_GetU16(addr)      ( *(HMDRV_U16*)(HMDRV_ADDR_SIZE)(addr) )
#define HMDRV_GetS32(addr)      ( *(HMDRV_S32*)(HMDRV_ADDR_SIZE)(addr) )
#define HMDRV_GetU32(addr)      ( *(HMDRV_U32*)(HMDRV_ADDR_SIZE)(addr) )
#define HMDRV_GetFloat(addr)    ( *(HMDRV_FLOAT*)(HMDRV_ADDR_SIZE)(addr) )
#endif

/****************************************************************************************
* Potentially unused variable declaration
*****************************************************************************************/
#if defined(_lint) || defined(__IAR_SYSTEMS_ICC__) || defined(__ARMCC_VERSION) 
#define HMDRV_UNUSED(sym) /*lint -esym(715,sym) -esym(818,sym) -esym(529,sym) -e{960} */
#elif defined(__GNUC__)
#define HMDRV_UNUSED(x) (void)(x)
#else
#define HMDRV_UNUSED(sym) ((sym),0)
#endif


/******************************************************************************
* NULL needed
******************************************************************************/

#ifndef NULL
#define NULL ((void *) 0)
#endif

/******************************************************************************
* Boolean values
******************************************************************************/

#ifdef true 
#define HMDRV_TRUE true
#endif

#ifdef false 
#define HMDRV_FALSE false
#endif

#ifndef HMDRV_TRUE
#define HMDRV_TRUE (1U)
#endif

#ifndef HMDRV_FALSE
#define HMDRV_FALSE (0U)
#endif


/******************************************************************************
* inline functions 
******************************************************************************/

/* we do not assume the inline is always supported by compiler
  rather each platform header defines its HMDRV_INLINE */
#ifndef HMDRV_INLINE
#define HMDRV_INLINE static
#endif

/* building macro-based inline code */
#define HMDRV_MACROCODE_BEGIN()     do{
#define HMDRV_MACROCODE_END()       }while(0)


/*******************************************************************************
* SCI access macros
*******************************************************************************/

    /* transmitter enable/disable */
#if 1
#define HMDRV_SCI_TE(eUartIndex)  UART_TxEna((UART_eModule_t)eUartIndex)
#define HMDRV_SCI_TD(eUartIndex)  UART_TxDis((UART_eModule_t)eUartIndex)
#else    
#define HMDRV_SCI_TE(eUartIndex) 				/* not used - already enabled */
#define HMDRV_SCI_TD(eUartIndex) 				/* not used  */
#endif
    
    



/* receiver enable/disable */
#if 0
#define HMDRV_SCI_RE(eUartIndex)  UART_RxEna((UART_eModule_t)eUartIndex)
#define HMDRV_SCI_RD(eUartIndex)  UART_RxDis((UART_eModule_t)eUartIndex)
#else
#define HMDRV_SCI_RE(eUartIndex)                /* not used - already enabled */
#define HMDRV_SCI_RD(eUartIndex)                /* not used  */
#endif

    
/*******************************************************************************
* SCI access Independent from MCU Family or Library
*******************************************************************************/

#ifdef HMDRV_USE_BOARDSCC
#define HMDRV_MAX_UART_FIFO_DEPTH   16
#elif defined(HMDRV_USE_BOARDLAUNCHPADXLTMS320F28379D)
#define HMDRV_MAX_UART_FIFO_DEPTH   16
#elif defined(HMDRV_USE_BOARDLAUNCHPADXLTMS320F28069M)
#define HMDRV_MAX_UART_FIFO_DEPTH   4
#endif

#if HMDRV_USE_SCIA
#define SCIRegsSCIRXBUF   SciaRegs.SCIRXBUF.all
#define SCIRegsSCIRXSTA   SciaRegs.SCIFFRX.bit.RXFFST
#define SCIRegsSCITXBUF   SciaRegs.SCITXBUF.all
#define SCIRegsSCITXSTA   SciaRegs.SCIFFTX.bit.TXFFST
#define SCIRegsSCITXEMP   SciaRegs.SCICTL2.bit.TXEMPTY
#elif HMDRV_USE_SCIB
#define SCIRegsSCIRXBUF   ScibRegs.SCIRXBUF.all
#define SCIRegsSCIRXSTA   ScibRegs.SCIFFRX.bit.RXFFST
#define SCIRegsSCITXBUF   ScibRegs.SCITXBUF.all
#define SCIRegsSCITXSTA   ScibRegs.SCIFFTX.bit.TXFFST
#define SCIRegsSCITXEMP   ScibRegs.SCICTL2.bit.TXEMPTY
#else
#error "SCIRegsSCIRXBUF not defined"   
#error "SCIRegsSCIRXSTA not defined"   
#error "SCIRegsSCITXBUF not defined"   
#error "SCIRegsSCITXSTA not defined"    
#error "SCIRegsSCITXEMP not defined"    
#endif

#define HMDRV_SCI_APPLICATION_INIT(eUartIndex)   UART_vInit((UART_eModule_t)eUartIndex)

#if HMDRV_SCI_RX_ISR  
/* Get received character */
#define HMDRV_SCI_GETCHAR(eUartIndex)     	UART_ReadDataRx((UART_eModule_t)eUartIndex)
/* Rx Possible */
#define HMDRV_READY_RX(eUartIndex)        	UART_ReadDataRxAvailable((UART_eModule_t)eUartIndex)
#endif  

#if HMDRV_SCI_TX_ISR
/* Tranmsit character */
#define HMDRV_SCI_PUTCHAR(eUartIndex, ch)	UART_PutChar((UART_eModule_t)eUartIndex, ch)
/* Tx Possible */
#define HMDRV_READY_TX(eUartIndex)        	UART_IsTxReady((UART_eModule_t)eUartIndex)
/* Tx Complete/Empty */
#define HMDRV_EMPTY_TX(eUartIndex)        	UART_IsTxEmpty((UART_eModule_t)eUartIndex)
/* ISR Ena/Dis */    
#define HMDRV_TX_INT_ENA(eUartIndex)      	UART_TxIntEna((UART_eModule_t)eUartIndex)
#define HMDRV_TX_INT_DIS(eUartIndex)      	UART_TxIntDis((UART_eModule_t)eUartIndex)
#endif    

#if HMDRV_SCI_TX_DELAY_ISR    
#if BOARDCFG_USE_TIMER_AND_UART_FUNCTIONALITY
#define HMDRV_RESET_TX_DELAY(eUartIndex)                  TIMER_vResetTimerMainFunctionDelayCounter((UART_eModule_t)eUartIndex)
#define HMDRV_SETUP_TX_DELAY(eUartIndex, usec)            TIMER_vSetupTimerMainFunctionDelayCompare((UART_eModule_t)eUartIndex, usec)
#define HMDRV_SETUP_TX_DELAY_FUNCTION(eUartIndex, pFunc)  TIMER_vSetupTimerMainDelayedFunctionSendData((UART_eModule_t)eUartIndex, (pfVoid_u8_t)pFunc)
#define HMDRV_SETUP_TX_DELAY_DATA(eUartIndex, data)       TIMER_vSetupTimerMainDelayedData((UART_eModule_t)eUartIndex, data)
#else
#error "Make BOARDCFG_USE_TIMER_AND_UART_FUNCTIONALITY = 1"
#endif
#endif


/*******************************************************************************
* SCI access Depending on MCU Family or Library
*******************************************************************************/
#if ((HMDRV_USE_CONTROL_SUITE_LIB) || (HMDRV_USE_C2000_WARE_LIB && HMDRV_USE_C2000_WARE_LIB_DEVICE_SUPPORT))

#if HMDRV_SCI_RX_ISR == 0 
/* Get received character */
#define HMDRV_SCI_GETCHAR(eUartIndex)     (unsigned char)(SCIRegsSCIRXBUF)
/* Rx Possible */
#define HMDRV_READY_RX(eUartIndex)        (SCIRegsSCIRXSTA > 0)
#endif

#if HMDRV_SCI_TX_ISR == 0
/* Tranmsit character */
#define HMDRV_SCI_PUTCHAR(eUartIndex,ch)   SCIRegsSCITXBUF = ch
/* Tx Possible */
#define HMDRV_READY_TX(eUartIndex)        (SCIRegsSCITXSTA < HMDRV_MAX_UART_FIFO_DEPTH)
/* Tx Complete/Empty */
#define HMDRV_EMPTY_TX(eUartIndex)        ( ((SCIRegsSCITXSTA == 0) && (SCIRegsSCITXEMP == 1)) ? true : false )
/* ISR Ena/Dis */    
//#define HMDRV_TX_INT_ENA(eUartIndex)       
//#define HMDRV_TX_INT_DIS(eUartIndex)       
#endif  

#define HMDRV_GET_FREE_RUN_TIMER() 			(HMDRV_TIMER)CPUTimer_getTimerCount(CPUTIMER1_BASE)
    
#else

#if HMDRV_SCI_RX_ISR == 0 
/* Get received character */
#define HMDRV_SCI_GETCHAR(eUartIndex)     (unsigned char)(SCI_readCharNonBlocking(HMDRV_SCI_BASE))
/* Rx Possible */
#define HMDRV_READY_RX(eUartIndex)        (SCI_getRxFIFOStatus(HMDRV_SCI_BASE) > 0)
#endif

#if HMDRV_SCI_TX_ISR == 0
/* Tranmsit character */
#define HMDRV_SCI_PUTCHAR(eUartIndex,ch)   SCI_writeCharNonBlocking(HMDRV_SCI_BASE, ch)
/* Tx Possible */
#define HMDRV_READY_TX(eUartIndex)        (SCI_getTxFIFOStatus(HMDRV_SCI_BASE) < HMDRV_MAX_UART_FIFO_DEPTH)
/* Tx Complete/Empty */
#define HMDRV_EMPTY_TX(eUartIndex)        ( ((SCI_getTxFIFOStatus(HMDRV_SCI_BASE) == 0) && ((HWREGH(HMDRV_SCI_BASE + SCI_O_CTL2) & SCI_CTL2_TXEMPTY) == SCI_CTL2_TXEMPTY)) ? true : false)
/* ISR Ena/Dis */    
//#define HMDRV_TX_INT_ENA(eUartIndex)       
//#define HMDRV_TX_INT_DIS(eUartIndex)       
#endif  
    
#define HMDRV_GET_FREE_RUN_TIMER()  (HMDRV_TIMER)CPUTimer_getTimerCount(CPUTIMER1_BASE)

#endif

#ifndef HMDRV_FREE_RUN_TIMER_FREQ_MHZ
#define HMDRV_FREE_RUN_TIMER_FREQ_MHZ           1
#endif

#ifndef HMDRV_FREE_RUN_TIMER_DECREMENT_COUNTER
#define HMDRV_FREE_RUN_TIMER_DECREMENT_COUNTER  0       /* Default FreeRun Timer is with Incrementing Counter */
#endif

#ifndef HMDRV_USE_DEBUG_PIN
#define HMDRV_USE_DEBUG_PIN 0
#endif

#if HMDRV_USE_DEBUG_PIN
#define HMDRV_DEBUG_PIN             30      /* Amber LED (middle Led) On 1038 CC Board */
#define HMDRV_vSetDebugPin()        GPIO_writePin(HMDRV_DEBUG_PIN, 1 )
#define HMDRV_vClrDebugPin()        GPIO_writePin(HMDRV_DEBUG_PIN, 0 )
#else
#define HMDRV_vSetDebugPin()
#define HMDRV_vClrDebugPin()
#endif

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void HMDRV_vInit(void);

#if HMDRV_USE_UART_INIT
void HMDRV_vProcess(void);
#endif
void HMDRV_vDebugSend(void);

void       HMDRV_CopyMemory                     (HMDRV_ADDR nDestAddr, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize);




#if HMDRV_USE_EXT_MEM == 0

#define    HMDRV_CopyFromBuffer                         HMDRV_CopyFromBufferInternal
HMDRV_BPTR HMDRV_CopyFromBufferInternal                 (HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize);
HMDRV_BPTR HMDRV_CopyFromBufferInternalCoil             (HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize);
HMDRV_BPTR HMDRV_CopyFromBufferInternalSingleCoil       (HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff);
#define    HMDRV_CopyFromBufferWithMask                 HMDRV_CopyFromBufferInternalWithMask
void       HMDRV_CopyFromBufferInternalWithMask         (HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize);
#define    HMDRV_CopyToBuffer                           HMDRV_CopyToBufferInternal
HMDRV_BPTR HMDRV_CopyToBufferInternal                   (HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize);
HMDRV_BPTR HMDRV_CopyToBufferInternalCoil               (HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize);
HMDRV_BPTR HMDRV_CopyToBufferInternalSingleCoil         (HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr);



#define    HMDRV_CopyFromBufferModbus                   HMDRV_CopyFromBufferInternalModbus
HMDRV_BPTR HMDRV_CopyFromBufferInternalModbus           (HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize, HMDRV_U8 u8ElementSize, HMDRV_U8* pu8ElementSizeArray, HMDRV_BOOL bStringNullTerminationPossible);

#define    HMDRV_CopyFromBufferModbusBitField           HMDRV_CopyFromBufferInternalModbusBitField
HMDRV_BPTR HMDRV_CopyFromBufferInternalModbusBitField   (HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSizeBits, HMDRV_SIZE8 nStartBit, HMDRV_U8 u8ElementSize, HMDRV_U8* pu8ElementSizeArray, HMDRV_BOOL bStringNullTerminationPossible);

#define    HMDRV_CopyFromBufferModbusCoil               HMDRV_CopyFromBufferInternalModbusCoil
HMDRV_BPTR HMDRV_CopyFromBufferInternalModbusCoil       (HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nBitOffset, HMDRV_SIZE8 nBitOffsetDest);

#define    HMDRV_CopyFromBufferModbusSingleCoil         HMDRV_CopyFromBufferInternalModbusSingleCoil
HMDRV_BPTR HMDRV_CopyFromBufferInternalModbusSingleCoil (HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_U8 nBitOffset);

HMDRV_BOOL HMDRV_CopyFromBufferWithMaskModbus           (HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize);


#define    HMDRV_CopyToBufferModbus                     HMDRV_CopyToBufferInternalModbus
HMDRV_BPTR HMDRV_CopyToBufferInternalModbus             (HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize, HMDRV_U8 u8ElementSize, HMDRV_U8* pu8ElementSizeArray);

#define    HMDRV_CopyToBufferModbusCoil                 HMDRV_CopyToBufferInternalModbusCoil
HMDRV_BPTR HMDRV_CopyToBufferInternalModbusCoil         (HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nBitOffset, HMDRV_SIZE8 nBitOffsetDest);

#define    HMDRV_CopyToBufferModbusSingleCoil           HMDRV_CopyToBufferInternalModbusSingleCoil
HMDRV_BPTR HMDRV_CopyToBufferInternalModbusSingleCoil   (HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nBitOffset);


#define    HMDRV_GetStringLength                        HMDRV_GetStringLengthInternal
HMDRV_SIZE8 HMDRV_GetStringLengthInternal               (HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize);

#define    HMDRV_GetStringLengthFromWord                HMDRV_GetStringLengthFromWordInternal
HMDRV_SIZE8 HMDRV_GetStringLengthFromWordInternal       (HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize);

#define     HMDRV_CopyFromBufferWithSuccess             HMDRV_CopyFromBufferInternal
#define     HMDRV_CopyFromBufferWithSuccessCoil         HMDRV_CopyFromBufferInternalCoil
#define     HMDRV_CopyFromBufferWithSuccessSingleCoil   HMDRV_CopyFromBufferInternalSingleCoil

#define     HMDRV_CopyToBufferWithSuccess               HMDRV_CopyToBufferInternal
#define     HMDRV_CopyToBufferWithSuccessCoil           HMDRV_CopyToBufferInternalCoil
#define     HMDRV_CopyToBufferWithSuccessSingleCoil     HMDRV_CopyToBufferInternalSingleCoil

#endif



#if HMDRV_USE_EXT_MEM == 1

HMDRV_BPTR HMDRV_CopyFromBuffer                         (HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize);
void       HMDRV_CopyFromBufferWithMask                 (HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize);
HMDRV_BPTR HMDRV_CopyToBuffer                           (HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize);



HMDRV_BPTR HMDRV_CopyFromBufferModbus                   (HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize, HMDRV_U8 u8ElementSize, HMDRV_U8* pu8ElementSizeArray, HMDRV_BOOL bStringNullTerminationPossible);
HMDRV_BPTR HMDRV_CopyFromBufferModbusCoil               (HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nBitOffset, HMDRV_SIZE8 nBitOffsetDest);
HMDRV_BPTR HMDRV_CopyFromBufferModbusSingleCoil         (HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nBitOffset);

HMDRV_BOOL HMDRV_CopyFromBufferWithMaskModbus           (HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize);

HMDRV_BPTR HMDRV_CopyToBufferModbus                     (HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize, HMDRV_U8 u8ElementSize, HMDRV_U8* pu8ElementSizeArray);
HMDRV_BPTR HMDRV_CopyToBufferModbusCoil                 (HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nBitOffset, HMDRV_SIZE8 nBitOffsetDest);
HMDRV_BPTR HMDRV_CopyToBufferModbusSingleCoil           (HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nBitOffset);

HMDRV_SIZE8 HMDRV_GetStringLength                       (HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize);

HMDRV_SIZE8 HMDRV_GetStringLengthFromWord               (HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize);

HMDRV_BPTR HMDRV_CopyFromBufferWithSuccess              (HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize);
HMDRV_BPTR HMDRV_CopyFromBufferWithSuccessCoil          (HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize);
HMDRV_BPTR HMDRV_CopyFromBufferWithSuccessSingleCoil    (HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff);

HMDRV_BPTR HMDRV_CopyToBufferWithSuccess                (HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize);
HMDRV_BPTR HMDRV_CopyToBufferWithSuccessCoil            (HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize);
HMDRV_BPTR HMDRV_CopyToBufferWithSuccessSingleCoil      (HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr);

HMDRV_BOOL HMDRV_bIsExternalAddress                     (HMDRV_ADDR nDestAddr);
HMDRV_U8*  HMDRV_pSnapshotAddressGet                    (HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize);
void       HMDRV_CopySnapshotToMemory                   (HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize);

#endif


/* mixed EX and non-EX commands may occur */
#if (HMDRV_USE_EX_CMDS) && (HMDRV_USE_NOEX_CMDS)
void       HMDRV_SetExAddr                      (HMDRV_BOOL bNextAddrIsEx);
#else
/* otherwise, we always know what addresses are used, (ignore HMDRV_SetExAddr) */
#define HMDRV_SetExAddr(bNextAddrIsEx)
#endif

/*

* To Do:
*
* Implement size 0...n support in functions
*
* HMDRV_CopyToBufferInternalModbus
* HMDRV_CopyFromBufferInternalWithMaskModbus
* HMDRV_CopyFromBufferInternalModbus
*
* Size 0 -> copy as in memory - lower to higher address in Little endian systems
* Size 1,2 ok
* Size 3 ->  0HML First padding byte after that High, Medium, Low
* Size 4 ->  UHML First uppest  byte after that High, Medium, Low
* ...
* Size n


*/


#if HMDRV_USE_UART_INIT
uint32_t HMDRV_u32GetSCIErrorPPM(void);
#else
#define HMDRV_u32GetSCIErrorPPM() 0
#endif

HMDRV_U32 HMDRV_u32GetDeviceSYSCLKFreq(void);


#endif	/* HMDRV_H */
