/* *****************************************************************************
 * File:   hmmodbus.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 02 11 18:34
 * 
 * Description: Hex Monitor ModBus Extention Communication
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef HMMODBUS_H
#define	HMMODBUS_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "hmlib.h"
#include "hmprotocol.h"

//#if HMDRV_USE_CSMON
//#include "csmon_t.h"
//#endif

/*****************************************************************************
* Configuration Definitions
******************************************************************************/

#define HMMODBUS_INPUT_REGS_FROM_HOLDING_REGS       1
#define HMMODBUS_DISCRETE_INPUTS_FROM_COILS         1

#define HMMODBUS_COILS_FROM_HOLDING_REGS            1       /* if here 1 -> Make HMMODBUS_INPUT_REGS_FROM_HOLDING_REGS = 1 !!! */



#define HMMODBUS_INPUT_REGISTER_TABLE_BARE_RAM      0       /* Hi Priority Usage */
#define HMMODBUS_HOLDING_REGISTER_TABLE_BARE_RAM    0       /* Hi Priority Usage */

#define HMMODBUS_INPUT_REGISTER_TABLE_SORTED        0       /* Lo Priority Usage */
#define HMMODBUS_HOLDING_REGISTER_TABLE_SORTED      0       /* Lo Priority Usage */


#define HMMODBUS_INPUT_COILS_TABLE_BARE_RAM         0       /* Hi Priority Usage */
#define HMMODBUS_HOLDING_COILS_TABLE_BARE_RAM       0       /* Hi Priority Usage */

#define HMMODBUS_INPUT_COILS_TABLE_SORTED           0       /* Lo Priority Usage */
#define HMMODBUS_HOLDING_COILS_TABLE_SORTED         0       /* Lo Priority Usage */



#define HMMODBUS_ILLEGAL_ADDR_REG_NOT_IMPLEMENTED   1       /* Illegal Address Exception Return if Register not implemented */       
#define HMMODBUS_ILLEGAL_ADDR_STOP_WRITE_DATA_REG   1       /* on Illegal Address Exception do not modify the data anymore */       




#define HMMODBUS_DEVICE_ADDRESS_BROADCAST           0x00

#define HMMODBUS_MIN_MULTIPLE_REGISTERS_WRITE       0x0001      /* 0x0001 in Modbus Spec */
#define HMMODBUS_MIN_MULTIPLE_REGISTERS_READ        0x0001      /* 0x0001 in Modbus Spec */

#define HMMODBUS_MIN_MULTIPLE_COILS_WRITE           0x0001      /* 0x0001 in Modbus Spec ? */
#define HMMODBUS_MIN_MULTIPLE_COILS_READ            0x0001      /* 0x0001 in Modbus Spec ? */



#define HMMODBUS_ALLOW_FILE_RW_DATA_COUNT_BIGGER_THAN_RECORD_SIZE_IN_VARIABLE_RECORD_SIZE_FILE  0
#define HMMODBUS_TRUNK_SIZE_IN_BIGGER_RECORD_LEGTH_REQUEST_IN_VARIABLE_RECORD_SIZE_FILE         1
//#define HMMODBUS_TRUNK_SIZE_IN_BIGGER_RECORD_LEGTH_REQUEST_IN_FIXED_RECORD_SIZE_FILE            0       /* implemented bTrunkBiggerLengthRequest insted */


#define HMMODBUS_DATA_NOT_READY_RAISE_EXCEPTION         0                       /* Highest Priority */
#define HMMODBUS_DATA_NOT_READY_RETURN_EMPTY_REGISTER   1                       /* Medium  Priority - default selection for records with invalid data */
#define HMMODBUS_DATA_NOT_READY_RETURN_NO_DATA          0                       /* Lowest  Priority */

                    



/*****************************************************************************
* Enumeration Definitions
******************************************************************************/
typedef enum
{
    HMMODBUS_CMD_INVALID                    = 0x00,
    HMMODBUS_CMD_RD_COILS                   = 0x01,
    HMMODBUS_CMD_RD_DISCRETE_INPUTS         = 0x02,
    HMMODBUS_CMD_RD_HOLD_REGS               = 0x03,
    HMMODBUS_CMD_RD_INPUT_REGS              = 0x04,
    HMMODBUS_CMD_WR_COIL                    = 0x05,
    HMMODBUS_CMD_WR_HOLD_REG                = 0x06,
    HMMODBUS_CMD_RD_EXCEPTION_STATUS        = 0x07,
    HMMODBUS_CMD_DIAGNOSTICS                = 0x08,
    HMMODBUS_CMD_RD_EVENT_COUNTER           = 0x0B,
    HMMODBUS_CMD_RD_EVENT_LOG               = 0x0C,
    HMMODBUS_CMD_WR_COILS                   = 0x0F,
    HMMODBUS_CMD_WR_HOLD_REGS               = 0x10,
    HMMODBUS_CMD_RD_SERVER_ID               = 0x11,
    HMMODBUS_CMD_RD_FILE_RECORD             = 0x14,
    HMMODBUS_CMD_WR_FILE_RECORD             = 0x15,
    HMMODBUS_CMD_WR_HOLD_REG_MASK           = 0x16,
    HMMODBUS_CMD_RW_HOLD_REGS               = 0x17,
    HMMODBUS_CMD_RD_FIFO_QUEUE              = 0x18,
    HMMODBUS_CMD_ENCAPSULATED_INTERFACE     = 0x2B,
    
    /* HMMODBUS_CMD_ENCAPSULATED_INTERFACE Sub-Commands */
    HMMODBUS_CMD_SUB_CANOPEN_GEN_REF        = 0x0D,
    HMMODBUS_CMD_SUB_RD_DEVICE_ID           = 0x0E,

    /* HMMODBUS_CMD_DIAGNOSTICS Sub-Commands */
    HMMODBUS_CMD_SUB_RETURN_QUERY_DATA      = 0x00,
    HMMODBUS_CMD_SUB_RESTART_COMMUNICATION  = 0x01,
    HMMODBUS_CMD_SUB_RETURN_DIAGNOSTIC_REG  = 0x02,
    HMMODBUS_CMD_SUB_CHANGE_ASCII_DELIMITER = 0x03,
    HMMODBUS_CMD_SUB_FORCE_LISTEN_ONLY_MODE = 0x04,
    HMMODBUS_CMD_SUB_CLEAR_CNT_AND_DIAG_REG = 0x0A,         /* Clear Counters and Diagnostic Register */
    HMMODBUS_CMD_SUB_RETURN_BUS_MSG_CNT     = 0x0B,
    HMMODBUS_CMD_SUB_RETURN_COMM_ERR_CNT    = 0x0C,
    HMMODBUS_CMD_SUB_RETURN_BUS_EXCEPT_CNT  = 0x0D,
    HMMODBUS_CMD_SUB_RETURN_SERVER_MSG_CNT  = 0x0E,         /* Counter Processed Messages By the Device */
    HMMODBUS_CMD_SUB_RETURN_NO_RESPONSE_CNT = 0x0F,
    HMMODBUS_CMD_SUB_RETURN_NO_ACK_CNT      = 0x10,
    HMMODBUS_CMD_SUB_RETURN_BUSY_CNT        = 0x11,
    HMMODBUS_CMD_SUB_RETURN_CHAR_OV_RUN_CNT = 0x12,
    HMMODBUS_CMD_SUB_CLEAR_OV_RUN_CNT_FLAG  = 0x14,


}HMMODBUS_eCommands;




typedef enum
{
    HMMODBUS_ERR_CMD_OR_MASK                = 0x80,
    HMMODBUS_ERR_NO_EXCEPTION               = 0x00,
    HMMODBUS_ERR_ILLEGAL_FUNCTION           = 0x01,
    HMMODBUS_ERR_ILLEGAL_ADDRESS            = 0x02,
    HMMODBUS_ERR_ILLEGAL_LENGTH             = 0x03,
    HMMODBUS_ERR_SERVER_DEVICE_FAILURE      = 0x04,
    HMMODBUS_ERR_RESPONSE_NOT_FIT_IN_BUFFER = 0x10, /* Not ModBus Error Code generally not used and not needed because message length is checked with exception 3 except if used smaller than 256 bytes buffer */
    HMMODBUS_ERR_RECEIVE_BUFFER_OVERWRITTEN = 0x11, /* Not ModBus Error Code generally not used but when used shared buffer for Rx and Tx and Request Data Overwritten before read. Possible on File Read Command if multiple records read with bigger length */
    HMMODBUS_ERR_DATA_NOT_READY             = 0x12, /* Not ModBus Error Code generally not used only possible on File Read Command when the size of the read record is 0 (no valid data available for this record). Raise Exception need to be configured */
    HMMODBUS_ERR_RECEIVE_BYTE_TIMEOUT       = 0x13, /* Not ModBus Error Code generally not used */
    HMMODBUS_ERR_RECEIVE_BUFFER_OVERFLOW    = 0x14, /* Not ModBus Error Code generally not used */
    HMMODBUS_ERR_RECEIVE_BAD_CS             = 0x15, /* Not ModBus Error Code generally not used */
}HMMODBUS_eErrorCodes;


typedef enum
{
    HMMODBUS_CONF_LVL_BASIC_STREAM_ONLY     = 0x01,
    HMMODBUS_CONF_LVL_REGUL_STREAM_ONLY     = 0x02,
    HMMODBUS_CONF_LVL_EXTND_STREAM_ONLY     = 0x03,
    HMMODBUS_CONF_LVL_BASIC_BOTH_MODES      = 0x81,
    HMMODBUS_CONF_LVL_REGUL_BOTH_MODES      = 0x82,
    HMMODBUS_CONF_LVL_EXTND_BOTH_MODES      = 0x83,
    HMMODBUS_CONF_LVL_MASK_BOTH_MODES       = 0x80,
    HMMODBUS_CONF_LVL_MASK_STREAM_ONLY      = 0x00,
}HMMODBUS_eIDConformityLevel;

typedef enum
{
    HMMODBUS_ID_OBJ_VENDORNAME              = 0x00,
    HMMODBUS_ID_OBJ_PRODUCTCODE             = 0x01,
    HMMODBUS_ID_OBJ_MAJORMINORREVISION      = 0x02,
    HMMODBUS_ID_OBJ_VENDORURL               = 0x03,
    HMMODBUS_ID_OBJ_PRODUCTNAME             = 0x04,
    HMMODBUS_ID_OBJ_MODELNAME               = 0x05,
    HMMODBUS_ID_OBJ_USERAPPLICATIONNAME     = 0x06,
    HMMODBUS_ID_OBJ_USERRESERVE0            = 0x80,
    HMMODBUS_ID_OBJ_USERRESERVE1            = 0x81,
    HMMODBUS_ID_OBJ_USERRESERVE2            = 0x82,
}HMMODBUS_eDeviceIDObject;

typedef enum
{
    HMMODBUS_REG_COIL_INPUT_MASK            = 0x01,
    HMMODBUS_REG_COIL_READ_MASK             = 0x02,
    HMMODBUS_REG_COIL_FULL_ACCESS_MASK      = 0x80,

    HMMODBUS_REG_COIL_WRITE_HOLDING         = 0x00,
    HMMODBUS_REG_COIL_READ_HOLDING          = 0x02,
    HMMODBUS_REG_COIL_READ_INPUT            = 0x03,

    HMMODBUS_REG_COIL_FULL_ACCESS_WR_HOLDING= 0x80,
    HMMODBUS_REG_COIL_FULL_ACCESS_RD_HOLDING= 0x82,
    HMMODBUS_REG_COIL_FULL_ACCESS_READ_INPUT= 0x83,


}HMMODBUS_eRegCoilTypeAccessOptions;


/*****************************************************************************
* Type Definitions
******************************************************************************/


/* HMMODBUS_sRegisterAddressTableFlags */
#define setReadOnlyField_HMMODBUS_sRegisterAddressTableFlags        (1 << 0)
#define setWriteOnlyField_HMMODBUS_sRegisterAddressTableFlags       (1 << 1)
#define setRDDenySkipCSMON_HMMODBUS_sRegisterAddressTableFlags      (1 << 2)
#define setWRDenySkipCSMON_HMMODBUS_sRegisterAddressTableFlags      (1 << 3)
#define setRWDenySkipCSMON_HMMODBUS_sRegisterAddressTableFlags      (setRDDenySkipCSMON_HMMODBUS_sRegisterAddressTableFlags | setWRDenySkipCSMON_HMMODBUS_sRegisterAddressTableFlags)

typedef struct
{
    HMDRV_U16 bReadOnly : 1;
    HMDRV_U16 bWriteOnly : 1; 
    HMDRV_U16 bReadDenySkipCSMON : 1;           /* When Not Found as CSMON Parameter Skip Deny Read through ModBus */
    HMDRV_U16 bWriteDenySkipCSMON : 1;          /* When Not Found as CSMON Parameter Skip Deny Write through ModBus */
    HMDRV_U16 bArray : 1;                       /* Array Type Register */
    HMDRV_U16 bBitField : 1;                    /*  */
    HMDRV_U16 bReserved : 2;                    /*  */
    HMDRV_U16 u8ElementCount : 8;               /* element count (multiples of element size) or bit if bit field */

}HMMODBUS_sRegisterAddressTableFlags;

typedef union
{
    HMDRV_U16 u16Register;
    HMMODBUS_sRegisterAddressTableFlags sFlags;
}HMMODBUS_uRegisterAddressTableFlags;


typedef struct
{
    HMDRV_U16 bWriteDisabled : 1;
    HMDRV_U16 bReadDisabled : 1;   
}HMMODBUS_sRegisterAddressTableInRamFlags;

typedef union
{
    HMDRV_U16 u16Register;
    HMMODBUS_sRegisterAddressTableInRamFlags sFlags;
}HMMODBUS_uRegisterAccessTableInRamFlags;



typedef struct
{
    HMDRV_U16 u8SizeElement : 8;
    HMDRV_U16 u8BitOffet : 8;

}HMMODBUS_sRegisterAddressTableSize;

typedef union
{
    HMDRV_U16 u16Register;
    HMMODBUS_sRegisterAddressTableSize sSize;
}HMMODBUS_uRegisterAddressTableSize;


typedef struct 
{
    HMDRV_U16 u16VirtAddress;
    HMMODBUS_uRegisterAddressTableSize uSize;
    HMMODBUS_uRegisterAddressTableFlags uFlags;
}HMMODBUS_sRegisterAddressTable_t;

typedef struct 
{
    HMDRV_U32 u32RealAddress;
    HMDRV_U32 u32PointerToProcessFunction;    /* If not NULL - execute this function for now only after write single register request */
}HMMODBUS_sRegisterAddressTableInRam_t;

typedef struct
{
    HMDRV_U16 au16RealAddress[2];
    HMDRV_U16 au16PointerToProcessFunction[2];    /* If not NULL - execute this function for now only after write single register request */
}HMMODBUS_sRegisterAddressTableInRam16bit_t;

typedef union
{
    HMMODBUS_sRegisterAddressTableInRam_t HMMODBUS_sRegisterAddressTableInRam;
    HMMODBUS_sRegisterAddressTableInRam16bit_t HMMODBUS_sRegisterAddressTableInRam16bit;
}HMMODBUS_uRegisterAddressTableInRam_t;

typedef struct
{
    HMMODBUS_uRegisterAccessTableInRamFlags uFlags;
}HMMODBUS_sRegisterAccessTableInRam_t;










/* HMMODBUS_sCoilAddressTableFlags */
#define setReadOnlyField_HMMODBUS_sCoilAddressTableFlags    (1 << 0)
#define setWriteOnlyField_HMMODBUS_sCoilAddressTableFlags   (1 << 1)


typedef struct
{
    HMDRV_U16 bReadOnly : 1;
    HMDRV_U16 bWriteOnly : 1; 
    HMDRV_U16 bReadDenySkipCSMON : 1;           /* When Not Found as CSMON Parameter Skip Deny Read through ModBus */
    HMDRV_U16 bWriteDenySkipCSMON : 1;          /* When Not Found as CSMON Parameter Skip Deny Write through ModBus */
}HMMODBUS_sCoilAddressTableFlags;

typedef union
{
    HMDRV_U16 u16Register;
    HMMODBUS_sCoilAddressTableFlags sFlags;
}HMMODBUS_uCoilAddressTableFlags;


typedef struct
{
    HMDRV_U16 bWriteDisabled : 1;
    HMDRV_U16 bReadDisabled : 1;   
}HMMODBUS_sCoilAddressTableInRamFlags;

typedef union
{
    HMDRV_U16 u16Register;
    HMMODBUS_sCoilAddressTableInRamFlags sFlags;
}HMMODBUS_uCoilAddressTableInRamFlags;


typedef struct 
{
    HMDRV_U16 u16VirtAddress;
    HMDRV_U16 u16BitOffset;
    //HMDRV_U16 u16Size;
    HMMODBUS_uCoilAddressTableFlags uFlags;
    
}HMMODBUS_sCoilAddressTable_t;

typedef struct 
{
    HMDRV_U32 u32RealAddress;
    HMDRV_U32 u32PointerToProcessFunction;    /* If not NULL - execute this function for now only after write single register request */
    HMMODBUS_uCoilAddressTableInRamFlags uFlags;
    
}HMMODBUS_sCoilAddressTableInRam_t;


typedef struct 
{
#ifdef __XC8
    HMDRV_U8 u8RecordBitsValid;   /* 0..8 */
    HMDRV_U8 u8RecordBytesValid;   /* 0..255 */
    HMDRV_U8 u8RecordCountValidLo;  /* 0..9999 */
    HMDRV_U8 u8RecordCountValidHi : 7;  /* 0..9999 */
    HMDRV_U8 u1UseValidDataStructure :1;  /* 0 - use 32 valid bits in file; 1 - use HMMODBUS_sFileValidData_t structure  */
#else
    HMDRV_U16 u8RecordBitsValid : 8;   /* 0..8 */
    HMDRV_U16 u8RecordBytesValid : 8;   /* 0..255 */
    HMDRV_U16 u15RecordCountValid : 15;  /* 0..9999 */
    HMDRV_U16 u1UseValidDataStructure :1;  /* 0 - use 32 valid bits in file; 1 - use HMMODBUS_sFileValidData_t structure  */
#endif
}HMMODBUS_sFileValidData_t;

typedef struct 
{
#ifdef __XC8
    HMDRV_U8 u3FileBitsValid : 3;   /* 0,1..7 (means 8,1..7) valid bits after last valid byte - 0 means all bits valid in last valid byte */
    HMDRV_U8 u1FileRegBytesValid : 1;   /* 0 means all bytes (2) valid in last valid 16-bit register  */
    HMDRV_U8 u4FileRegistersValidLo : 4;   /* 2^27-1 registers in file could be supported - (delete by (u16RecordSizeBytes/2) to estimate Valid Records) */
    HMDRV_U8 u8FileRegistersValidMid;   /* 2^27-1 registers in file could be supported - (delete by (u16RecordSizeBytes/2) to estimate Valid Records) */
    HMDRV_U8 u8FileRegistersValidHi : 7;   /* 2^27-1 registers in file could be supported - (delete by (u16RecordSizeBytes/2) to estimate Valid Records) */
    HMDRV_U8 u1UseValidDataStructure :1;  /* 0 - use 32 valid bits in file; 1 - use HMMODBUS_sFileValidData_t structure  */
#else
    HMDRV_U32 u3FileBitsValid : 3;   /* 0,1..7 (means 8,1..7) valid bits after last valid byte - 0 means all bits valid in last valid byte */
    HMDRV_U32 u1FileRegBytesValid : 1;   /* 0 means all bytes (2) valid in last valid 16-bit register  */
    HMDRV_U32 u27FileRegistersValid : 27;   /* 2^27-1 registers in file could be supported - (delete by (u16RecordSizeBytes/2) to estimate Valid Records) */
    HMDRV_U32 u1UseValidDataStructure :1;  /* 0 - use 32 valid bits in file; 1 - use HMMODBUS_sFileValidData_t structure  */
#endif
}HMMODBUS_sFileValidBits_t;

typedef struct 
{
#ifdef __XC8
    HMDRV_U8 u3FileBitsValid : 3;   /* 0,1..7 (means 8,1..7) valid bits after last valid byte - 0 means all bits valid in last valid byte */
    HMDRV_U8 u4FileRegistersValidLo : 5;   /* 2^28-1 bytes in file could be supported - (delete by (u16RecordSizeBytes) to estimate Valid Records) */
    HMDRV_U8 u8FileRegistersValidMid;   /* 2^28-1 bytes in file could be supported - (delete by (u16RecordSizeBytes) to estimate Valid Records) */
    HMDRV_U8 u8FileRegistersValidHi : 7;   /* 2^28-1 bytes in file could be supported - (delete by (u16RecordSizeBytes) to estimate Valid Records) */
    HMDRV_U8 u1UseValidDataStructure :1;  /* 0 - use 32 valid bits in file; 1 - use HMMODBUS_sFileValidData_t structure  */
#else
    HMDRV_U32 u3FileBitsValid : 3;   /* 0,1..7 (means 8,1..7) valid bits after last valid byte - 0 means all bits valid in last valid byte */
    HMDRV_U32 u28FileBytesValid : 28;   /* 2^28-1 bytes in file could be supported - (delete by (u16RecordSizeBytes) to estimate Valid Records) */
    HMDRV_U32 u1UseValidDataStructure :1;  /* 0 - use 32 valid bits in file; 1 - use HMMODBUS_sFileValidData_t structure  */
#endif
}HMMODBUS_sFileValidBytes_t;


typedef union 
{
    HMDRV_U32 u32ValidBits;  /* Valid Bits In the File */
    HMMODBUS_sFileValidBits_t sFileValidBits;  
    HMMODBUS_sFileValidBytes_t sFileValidBytes;  
    HMMODBUS_sFileValidData_t sFileValidData;  
}HMMODBUS_uFileValidData_t;


typedef struct 
{
    HMDRV_U16 u16VirtAddress;
    HMDRV_U16 u16RecordCount; /* if 0 then First Record number request in message is offset multiplied with 10000. Second record number request is additional offset add. If u16RecordToRecordOffsetSizeBytes > 0 then it is used for additional offset multiplier for the both offsets  */
    HMDRV_U32 u32RealAddress;                               /* if 0 read from u32PointerToFileRecordRealAddressTable or file with Parameter Data */
    HMDRV_U16 u16RecordSizeBytes;
    HMDRV_U16 u16RecordToRecordOffsetSizeBytes;             /* if 0 and u32PointerToFileRecordOffsetTable = NULL -> u16RecordToRecordOffsetSizeBytes comes from requested RegisterCount */
    HMDRV_U32 u32PointerToFileRecordOffsetTable;            /* u32PointerToFileRecordOffsetTable used as File bytes Size if multiplied offset used */
    HMDRV_U32 u32PointerToFileRecordStringDataTable;
    HMDRV_U32 u32PointerToRecordIndexGenerationFunction;    /* If not NULL - used this function for record indexing  */
    HMDRV_U32 u32PointerToProcessFunctionAfterWriteCommand;    /* If not NULL - execute this function after write file record request */
    HMDRV_U32 u32PointerToProcessFunctionAfterReadCommand;    /* If not NULL - execute this function after read file record request */
    HMDRV_U32 u32PointerToProcessFunctionBeforeReadCommand;    /* If not NULL - execute this function before read file record request */
    HMDRV_U32 u32PointerToFileRecordProcessFunctionAfterWriteCommandTable;    /* table for each record to execute function after write file record request (in fact this is pointer to pointer to function) */
    HMDRV_U32 u32PointerToFileRecordProcessFunctionAfterReadCommandTable;    /* table for each record to execute function after write file record request (in fact this is pointer to pointer to function) */
    HMDRV_U32 u32PointerToFileRecordReadOnlyTable;
    HMDRV_U32 u32PointerToFileRecordRealAddressTable;       /* u32RealAddress data for each record */
    HMDRV_U32 u32PointerToFileRecordSizeBytesTable;        /* u16RecordSizeBytes data for each record */
    HMDRV_U32 u32PointerToFileRecordElementsSizeTable;
    HMDRV_U32 u32PointerToFileRecordElementsRealAddressTable;
    HMDRV_U32 u32PointerToFileRecordElementsFromDiffLocationsSizeTable;
    HMDRV_U32 u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable;
    HMDRV_U32 u32PointerToFileRecordElementsCountInPartTable;
    HMMODBUS_uFileValidData_t uFileValidData;     /* Valid Data In the File */
    HMDRV_U32 u32FileSizeBytes;                     /* If not 0 used to limit the file size in bytes - raise exception if operation outside limits */
    
    HMDRV_U8 u8RecordElementsCountSize; /* if u32PointerToFileRecordElementsSizeTable == null -> if u8RecordElementsCountSize > 0 this is the u8ElementsSize */
    HMDRV_BOOL bStringDataExpected;
    HMDRV_BOOL bReadOnlyFile;
    HMDRV_BOOL bTrunkBiggerLengthRequest;       /* For now used only in fixed size record */
    HMDRV_U8 u8RecordElementsFromTableCount;
    HMDRV_BOOL bReadAccess;
    HMDRV_BOOL bWriteAccess;
    
}HMMODBUS_sFileTable_t;

/* u16RecordToRecordOffsetSizeBytes
 *
 * If multiplied offset used (u16RecordCount == 0) this is the number+1 of registers additional multiplier. Write from PC possible. If 0 then assume 1 register additional multiplier
 *
 * If 0 and u32PointerToFileRecordOffsetTable = NULL -> u16RecordToRecordOffsetSizeBytes comes from requested RegisterCount
 *
 *  */



typedef struct 
{
    HMDRV_U16 u16VirtAddress;
    HMDRV_U16 u16RecordCount;
    HMDRV_U32 u32RealAddress;
    HMDRV_U16 u16RecordSizeBytes;
    HMDRV_U32 u32RecordCountValid;
}HMMODBUS_sQueueTable_t;





/* serial runtime flags */
typedef struct
{
    unsigned bTxActive : 1;         /* transmit response */
    unsigned bTxWaitTC : 1;         /* wait Transmit Complete */
    unsigned bTxDelay : 1;         /* transmit response delay */
    unsigned bTxDelaySkipReduce : 1;         /* transmit response delay first time skip (was not fully used) */

}HMMODBUS_sFlagsRxTx_t;
typedef union 
{
    HMDRV_FLAGS     nAll;
    HMMODBUS_sFlagsRxTx_t    sFlg;   
} HMMODBUS_uFlagsRxTx_t;

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */
#if HMDRV_USE_MODBUS
extern HMMODBUS_uFlagsRxTx_t HMMODBUS_wFlagsRxTx;
extern HMDRV_U32 HMMODBUS_u32TxDelayCounter;
extern HMDRV_BPTR  HMMODBUS_pTxBuff;    
#endif


#define DEBUG_MAX_FILE_RECORD_COUNT 5
extern volatile HMDRV_U32 HMMODBUS_nFilePrimaryAddrWriteLast[DEBUG_MAX_FILE_RECORD_COUNT];
extern volatile HMDRV_U32 HMMOSBUS_nFileWritePrimaryAddrReadLast;

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void HMMODBUS_vSetFreeRunTimerPrescaller(uint16_t u16Prescaller);
void HMMODBUS_vSetServerIDDataAddress (HMDRV_ADDR pInput);
void HMMODBUS_vSetServerIDLength (HMDRV_U8 u8Input);
void HMMODBUS_vSetServerIDNumber (HMDRV_U8 u8Input);
void HMMODBUS_vSetServerOnStatus (HMDRV_U8 u8Input);

void HMMODBUS_vSetDeviceIDVendorName (HMDRV_ADDR pInput);
void HMMODBUS_vSetDeviceIDVendorNameLength (HMDRV_U8 u8Input);
void HMMODBUS_vSetDeviceIDProductCode (HMDRV_ADDR pInput);
void HMMODBUS_vSetDeviceIDProductCodeLength (HMDRV_U8 u8Input);
void HMMODBUS_vSetDeviceIDMajorMinorRevision (HMDRV_ADDR pInput);
void HMMODBUS_vSetDeviceIDMajorMinorRevisionLength (HMDRV_U8 u8Input);
void HMMODBUS_vSetDeviceIDVendorUrl (HMDRV_ADDR pInput);
void HMMODBUS_vSetDeviceIDVendorUrlLength (HMDRV_U8 u8Input);
void HMMODBUS_vSetDeviceIDProductName (HMDRV_ADDR pInput);
void HMMODBUS_vSetDeviceIDProductNameLength (HMDRV_U8 u8Input);
void HMMODBUS_vSetDeviceIDModelName (HMDRV_ADDR pInput);
void HMMODBUS_vSetDeviceIDModelNameLength (HMDRV_U8 u8Input);
void HMMODBUS_vSetDeviceIDUserApplicationName (HMDRV_ADDR pInput);
void HMMODBUS_vSetDeviceIDUserApplicationNameLength (HMDRV_U8 u8Input);
void HMMODBUS_vSetDeviceIDUserReserve0 (HMDRV_ADDR pInput);
void HMMODBUS_vSetDeviceIDUserReserve0Length (HMDRV_U8 u8Input);
void HMMODBUS_vSetDeviceIDUserReserve1 (HMDRV_ADDR pInput);
void HMMODBUS_vSetDeviceIDUserReserve1Length (HMDRV_U8 u8Input);
void HMMODBUS_vSetDeviceIDUserReserve2 (HMDRV_ADDR pInput);
void HMMODBUS_vSetDeviceIDUserReserve2Length (HMDRV_U8 u8Input);




void HMMODBUS_vSetPortIndex(HMDRV_U8 u8Index);
void HMMODBUS_vInit(void);

void HMMODBUS_vProcessSCI(void);
void HMMODBUS_vProcessSCITXISR (void);

HMDRV_BOOL HMMODBUS_Rx(HMDRV_BCHR nRxChar);
HMDRV_BOOL HMMODBUS_Tx(HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pTxChar);
void HMMODBUS_Listen(void);
HMDRV_U16 HMMODBUS_CalcCRC(HMDRV_CRC nRxCheckSum,HMDRV_BCHR nRxChar);

HMDRV_TIMER HMMODBUS_vProcessFreeTimer(void);
void HMMODBUS_vProcessRxDelay(HMDRV_TIMER nTimeTickDelay);
void HMMODBUS_vProcessTxDelay(HMDRV_TIMER nTimeTickDelay);

void HMMODBUS_vSetupRegisterDummy (void);
void HMMODBUS_vSetupHoldingRegistersAddressTable(HMDRV_U32 u32TableAddress, HMDRV_U32 u32Size);
void HMMODBUS_vSetupHoldingRegistersValuesTable(HMDRV_U32 u32TableAddress, HMDRV_U32 u32Size);
void HMMODBUS_vSetupHoldingRegistersAddressTableInRam(HMMODBUS_sRegisterAddressTableInRam_t* pTable, HMMODBUS_sRegisterAccessTableInRam_t* pTableAccess, HMDRV_U32 u32Size);
void HMMODBUS_vSetupInputRegistersAddressTable(HMDRV_U32 u32TableAddress, HMDRV_U32 u32Size);
void HMMODBUS_vSetupInputRegistersValuesTable(HMDRV_U32 u32TableAddress, HMDRV_U32 u32Size);
void HMMODBUS_vSetupInputRegistersAddressTableInRam(HMMODBUS_sRegisterAddressTableInRam_t* pTable, HMMODBUS_sRegisterAccessTableInRam_t* pTableAccess, HMDRV_U32 u32Size);

void HMMODBUS_vSetupCoilDummy (void);
void HMMODBUS_vSetupHoldingCoilsAddressTable(HMDRV_U32 u32TableAddress, HMDRV_U32 u32Size);
void HMMODBUS_vSetupHoldingCoilsValuesTable(HMDRV_U32 u32TableAddress, HMDRV_U32 u32Size);
void HMMODBUS_vSetupHoldingCoilsAddressTableInRam(HMMODBUS_sCoilAddressTableInRam_t* pTable, HMDRV_U32 u32Size);
void HMMODBUS_vSetupInputCoilsAddressTable(HMDRV_U32 u32TableAddress, HMDRV_U32 u32Size);
void HMMODBUS_vSetupInputCoilsValuesTable(HMDRV_U32 u32TableAddress, HMDRV_U32 u32Size);
void HMMODBUS_vSetupInputCoilsAddressTableInRam(HMMODBUS_sCoilAddressTableInRam_t* pTable, HMDRV_U32 u32Size);

void HMMODBUS_vFileTable(HMMODBUS_sFileTable_t* pTable, HMDRV_U32 u32Size);
void HMMODBUS_vSetupQueueRegistersAddressTable(HMMODBUS_sQueueTable_t* pTable, HMDRV_U32 u32Size);
void HMMODBUS_vSetupActiveQueueIndex(HMDRV_U8 u8Index);


void HMMODBUS_vFileTableSetValidRecord(HMDRV_U16 u16FileIndex, HMMODBUS_uFileValidData_t uValidData);
void HMMODBUS_vProcessFastTD (void);


HMDRV_U32 HMMODBUS_GetRealRegisterAddressAndSize(HMDRV_U16 nAddr, HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* pu16RealSize, HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* pu16ElemSize, HMMODBUS_eRegCoilTypeAccessOptions eRegCoilTypeAccessOptions);
HMDRV_U32 HMMODBUS_GetRealCoilAddressAndOffset(HMDRV_U16 nAddr, HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* pu16RealOffset, HMMODBUS_eRegCoilTypeAccessOptions eRegCoilTypeAccessOptions);

HMDRV_U32 HMMODBUS_u32GetAddressMessageCounterThisDevice(void);

HMDRV_U32 HMMODBUS_u32GetFreeRunTimerReceivedMessage (void);
HMDRV_BOOL HMMODBUS_bGetReceivedMessage (void);


#endif	/* HMMODBUS_H */
