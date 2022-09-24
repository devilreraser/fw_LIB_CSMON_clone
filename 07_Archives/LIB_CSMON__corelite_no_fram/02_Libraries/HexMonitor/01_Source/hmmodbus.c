/* *****************************************************************************
 * File:   hmmodbus_t.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 02 11 18:34
 * 
 * Description: Hex Monitor ModBus Extention Communication 
 * 
 **************************************************************************** */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "hmdrv.h"

/* *****************************************************************************
 * Check if Not Used
 **************************************************************************** */
#if (HMDRV_USE_MODBUS)  && (!(HMDRV_DISABLE))

#include "hmprotocol.h"
#include "hmmodbus.h"

#if HMDRV_USE_EXT_MEM
#if HMDRV_USE_EXT_SPI
#include "spimem.h"
#endif
#if HMDRV_USE_EXT_PMP
#include "pmpmem.h"
#endif
#if HMDRV_USE_EXT_I2C
#include "i2cmem.h"
#endif
#endif

#if 1
#include "modbusreg_t.h"      //commented on 2020 01 05 when implementing TI
#else
#define MODBUSREG_REGID_MODBUS_DUMMY_USAGE  0xFFFF
#endif

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
#ifndef MCLIB_H
typedef void (*pfVoid_t)(void); 
#endif

#ifndef CSMON_T_H
/* index generation function prototype */
typedef uint16_t (*CSMON_pfIndexGeneration)(uint16_t);

/* execute function prototype */
typedef void (*CSMON_pfFunctionExecute)(void);
#endif

/* serial runtime flags */
typedef struct
{
        unsigned bRxMsgLengthNext : 1;  /* expect the length byte next time */
        unsigned bRxMsgMEITypeNext : 1; /* expect the MEI Type byte next time */
        unsigned bRxMsgHeader : 1;      /* expect the header finish */
        unsigned bRxMsgCommand : 1;     /* expect the command next */

        //unsigned bTxLastCharSOB : 1;    /* Start of Block Detected */
        //unsigned bTxFirstSobSend : 1;   /* used in USB_CDC, MQX, JTAG */
        //unsigned bRxLastCharSOB   : 1;  /* last received character was SOB */
}HMMODBUS_sFlags_t;

typedef union 
{
    HMDRV_FLAGS     nAll;
    HMMODBUS_sFlags_t    sFlg;   
} HMMODBUS_uFlags_t;




/* *****************************************************************************
 * Variables
 **************************************************************************** */
HMMODBUS_uFlags_t HMMODBUS_wFlags;
HMMODBUS_uFlagsRxTx_t HMMODBUS_wFlagsRxTx;

/* receive and transmit buffers and counters */
volatile HMDRV_SIZE8 HMMODBUS_nTxTodo;     /* transmission to-do counter (0 when tx is idle) */
volatile HMDRV_SIZE8 HMMODBUS_nRxTodoEcho;     /* transmission read-back to-do counter */

volatile HMDRV_SIZE8 HMMODBUS_nLastResponseCmd;   
volatile HMDRV_SIZE8 HMMODBUS_nLastResponseCode;    

volatile HMDRV_SIZE8 HMMODBUS_nRxTodo;     /* reception to-do counter (0 when rx is idle) */
HMDRV_BPTR  HMMODBUS_pTxBuff;     /* pointer to next byte to transmit */
HMDRV_BPTR  HMMODBUS_pRxBuff;     /* pointer to next free place in RX buffer */
volatile HMDRV_CRC   HMMODBUS_nRxCheckSum; /* checksum of data being received */
volatile HMDRV_CRC   HMMODBUS_nTxCheckSum; /* checksum of data being received */

HMDRV_BOOL HMMODBUS_bOverflowReached;


volatile HMDRV_CRC   HMMODBUS_anTxCheckSum[HMDRV_MODBUS_BUFFER_SIZE]; /* checksum of data being received */

volatile HMDRV_SIZE8 u8UartModBusIndex = HMLIB_MODBUS_UART_MODULE;


#if HMDRV_MODBUS_SHARED_RX_TX_BUFFER
HMDRV_BCHR HMMODBUS_pCommBuffer[HMDRV_MODBUS_BUFFER_SIZE];
#define HMMODBUS_pCommBufferTx HMMODBUS_pCommBuffer
#define HMMODBUS_pCommBufferRx HMMODBUS_pCommBuffer
#else
HMDRV_BCHR HMMODBUS_pCommBufferTx[HMDRV_MODBUS_BUFFER_SIZE];
HMDRV_BCHR HMMODBUS_pCommBufferRx[HMDRV_MODBUS_BUFFER_SIZE];
#endif



/* communication buffer (in/out) */

HMDRV_U8 HMMODBUS_u8DeviceAddress = HMDRV_MODBUS_DEVICE_ADDRESS;

HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sRegisterAddressTable_t* HMMODBUS_pu32HoldingRegisterAddressTable = NULL;
HMMODBUS_sRegisterAddressTableInRam_t* HMMODBUS_pu32HoldingRegisterAddressTableInRam = NULL;
HMMODBUS_sRegisterAccessTableInRam_t* HMMODBUS_pu32HoldingRegisterAccessTableInRam = NULL;
volatile HMDRV_U32 HMMODBUS_u32HoldingRegisterAddressTableSize = 0;
volatile HMDRV_U32 HMMODBUS_u32HoldingRegisterAddressTableInRamSize = 0;
volatile HMDRV_U32 HMMODBUS_u32HoldingRegisterValuesTableSize = 0;

HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sRegisterAddressTable_t* HMMODBUS_pu32InputRegisterAddressTable = NULL;
HMMODBUS_sRegisterAddressTableInRam_t* HMMODBUS_pu32InputRegisterAddressTableInRam = NULL;
HMMODBUS_sRegisterAccessTableInRam_t* HMMODBUS_pu32InputRegisterAccessTableInRam = NULL;
volatile HMDRV_U32 HMMODBUS_u32InputRegisterAddressTableSize = 0;
volatile HMDRV_U32 HMMODBUS_u32InputRegisterAddressTableInRamSize = 0;
volatile HMDRV_U32 HMMODBUS_u32InputRegisterValuesTableSize = 0;

HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sCoilAddressTable_t* HMMODBUS_pu32HoldingCoilAddressTable = NULL;
HMMODBUS_sCoilAddressTableInRam_t* HMMODBUS_pu32HoldingCoilAddressTableInRam = NULL;
volatile HMDRV_U32 HMMODBUS_u32HoldingCoilAddressTableSize = 0;
volatile HMDRV_U32 HMMODBUS_u32HoldingCoilAddressTableInRamSize = 0;
volatile HMDRV_U32 HMMODBUS_u32HoldingCoilValuesTableSize = 0;

HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sCoilAddressTable_t* HMMODBUS_pu32InputCoilAddressTable = NULL;
HMMODBUS_sCoilAddressTableInRam_t* HMMODBUS_pu32InputCoilAddressTableInRam = NULL;
volatile HMDRV_U32 HMMODBUS_u32InputCoilAddressTableSize = 0;
volatile HMDRV_U32 HMMODBUS_u32InputCoilAddressTableInRamSize = 0;
volatile HMDRV_U32 HMMODBUS_u32InputCoilValuesTableSize = 0;

HMMODBUS_sQueueTable_t* HMMODBUS_pasQueueRegisterAddressTable = NULL;
volatile HMDRV_U32 HMMODBUS_u32QueueRegisterAddressTableSize = 0;
volatile HMDRV_U8 HMMODBUS_u8QueueActiveIndex = 0xFF;

volatile HMDRV_U16 u16RegisterAddressDummyValue;

HMDRV_U32 HMMODBUS_u32TxDelayCounter;


volatile HMDRV_U32 HMMODBUS_nFilePrimaryAddrWriteLast[DEBUG_MAX_FILE_RECORD_COUNT];
volatile HMDRV_U32 HMMOSBUS_nFileWritePrimaryAddrReadLast;
volatile HMDRV_U32 HMMOSBUS_nFileWriteAddrReadLast[DEBUG_MAX_FILE_RECORD_COUNT];
volatile HMDRV_U32 HMMOSBUS_nDummy;

HMMODBUS_sRegisterAddressTable_t HMMODBUS_u32RegisterAddressDummy;
HMMODBUS_sRegisterAddressTableInRam_t HMMODBUS_u32RegisterAddressInRamDummy;
//HMMODBUS_sRegisterValuesTable_t HMMODBUS_u32RegisterValuesDummy;

HMMODBUS_sCoilAddressTable_t HMMODBUS_u32CoilAddressDummy;
HMMODBUS_sCoilAddressTableInRam_t HMMODBUS_u32CoilAddressInRamDummy;
//HMMODBUS_sCoilValuesTable_t HMMODBUS_u32CoilValuesDummy;


HMDRV_U32 HMMODBUS_u32FreeRunTimerReceivedMessage = 0;
HMDRV_BOOL HMMODBUS_bReceivedMessage = false;



HMMODBUS_sFileTable_t* HMMODBUS_pasFileAddressTable = NULL;
volatile HMDRV_U32 HMMODBUS_u32FileCount = 0;

volatile HMDRV_BOOL bInputRegsGlobal = HMDRV_FALSE;
volatile HMDRV_BOOL bBareRamTable = HMDRV_FALSE;
    
volatile HMDRV_BOOL bUsedDummyDataAddressInResponseData = HMDRV_FALSE;

volatile HMDRV_BOOL bLengthException = HMDRV_FALSE;

volatile HMDRV_BOOL bInputRegTableSorted = HMMODBUS_INPUT_REGISTER_TABLE_SORTED;
volatile HMDRV_BOOL bHoldingRegTableSorted = HMMODBUS_HOLDING_REGISTER_TABLE_SORTED;

volatile HMDRV_BOOL bInputRegTableBareRam = HMMODBUS_INPUT_REGISTER_TABLE_BARE_RAM;
volatile HMDRV_BOOL bHoldingRegTableBareRam = HMMODBUS_HOLDING_REGISTER_TABLE_BARE_RAM;

volatile HMDRV_BOOL bInputCoilTableSorted = HMMODBUS_INPUT_COILS_TABLE_SORTED;
volatile HMDRV_BOOL bHoldingCoilTableSorted = HMMODBUS_HOLDING_COILS_TABLE_SORTED;

volatile HMDRV_BOOL bInputCoilTableBareRam = HMMODBUS_INPUT_COILS_TABLE_BARE_RAM;
volatile HMDRV_BOOL bHoldingCoilTableBareRam = HMMODBUS_HOLDING_COILS_TABLE_BARE_RAM;


/* Default in uSec */

#define HMMODBUS_TIMER_TICK_DELAY   400
#define HMMODBUS_TIMER_TICK_ERROR   400     /* Max Time Error between Receive Byte and Get Time */
#if HMDRV_MODBUS_BAUD >= 19200
#define HMMODBUS_MAX_BYTE_TO_BYTE_EXACT   (uint32_t)(((1.0 * 11 * 1000000.0)/HMDRV_MODBUS_BAUD) +  750)
#define HMMODBUS_MAX_MESS_TO_MESS_EXACT   (uint32_t)(((1.0 * 11 * 1000000.0)/HMDRV_MODBUS_BAUD) + 1750)
#else
#define HMMODBUS_MAX_BYTE_TO_BYTE_EXACT   (uint32_t)(((2.5 * 11 * 1000000.0)/HMDRV_MODBUS_BAUD) + 0) /* 1.0 + 1.5 */
#define HMMODBUS_MAX_MESS_TO_MESS_EXACT   (uint32_t)(((4.5 * 11 * 1000000.0)/HMDRV_MODBUS_BAUD) + 0) /* 1.0 + 3.5 */
#endif

#define HMMODBUS_MIN_MESS_TO_MESS_EXACT   (uint32_t)(2500)  /* usec */

#define HMMODBUS_MAX_BYTE_TO_BYTE   (uint32_t)(HMMODBUS_MAX_BYTE_TO_BYTE_EXACT + HMMODBUS_TIMER_TICK_ERROR)
#define HMMODBUS_MAX_MESS_TO_MESS   (uint32_t)(HMMODBUS_MAX_MESS_TO_MESS_EXACT - HMMODBUS_TIMER_TICK_ERROR)
#define HMMODBUS_MIN_MESS_TO_MESS   HMMODBUS_MIN_MESS_TO_MESS_EXACT


HMDRV_U16 u16FreeRunTimerPrescaller = 0;        /* 0 means prescaller 1:1 (u16FreeRunTimerPrescaller+1  :  1) */
HMDRV_U32 HMMODBUS_u32DeviceSYSCLKFreq = (uint32_t)(HMDRV_FREE_RUN_TIMER_FREQ_MHZ * 1000000);

HMDRV_U32 u32ModbusMaxByteToByte = (uint32_t)(HMMODBUS_MAX_BYTE_TO_BYTE*HMDRV_FREE_RUN_TIMER_FREQ_MHZ); 
HMDRV_U32 u32ModbusMaxMessToMess = (uint32_t)(HMMODBUS_MAX_MESS_TO_MESS*HMDRV_FREE_RUN_TIMER_FREQ_MHZ); 
HMDRV_U32 u32ModbusMinMessToMess = (uint32_t)(HMMODBUS_MIN_MESS_TO_MESS*HMDRV_FREE_RUN_TIMER_FREQ_MHZ); 
HMDRV_U16 u16ModbusMinMessToMess_usec = (uint16_t)(HMMODBUS_MIN_MESS_TO_MESS); 
//HMDRV_U32 u32ModbusTimeTickDelay = HMMODBUS_TIMER_TICK_DELAY*HMDRV_FREE_RUN_TIMER_FREQ_MHZ; 

HMDRV_U32 u32ModbusMaxByteToByteExact = (uint32_t)(HMMODBUS_MAX_BYTE_TO_BYTE_EXACT*HMDRV_FREE_RUN_TIMER_FREQ_MHZ); 
HMDRV_U32 u32ModbusMaxMessToMessExact = (uint32_t)(HMMODBUS_MAX_MESS_TO_MESS_EXACT*HMDRV_FREE_RUN_TIMER_FREQ_MHZ); 

volatile HMDRV_U32 u32ModbusTimeout = (uint32_t)(HMMODBUS_MAX_MESS_TO_MESS*HMDRV_FREE_RUN_TIMER_FREQ_MHZ);

//HMDRV_TIMER nTimeTickDelay;


/* Read Server ID */
HMDRV_U8 HMMODBUS_u8ReadServerIDLength = 16;
HMDRV_U8 HMMODBUS_u8ReadServerIDNumber = 0;
HMDRV_U8 HMMODBUS_u8ReadServerOnStatus = 0; /* 0x00 or 0xFF */
HMDRV_ADDR HMMODBUS_pu8ReadServerIDDatAdr = NULL;

HMDRV_U8 HMMODBUS_u8ReadDeviceIDConfLvlMask = HMMODBUS_CONF_LVL_MASK_BOTH_MODES;  //HMMODBUS_CONF_LVL_MASK_STREAM_ONLY
HMDRV_U8 HMMODBUS_u8ReadDeviceIDConfLvl = HMMODBUS_CONF_LVL_EXTND_BOTH_MODES;
HMDRV_ADDR HMMODBUS_pu8ReadDeviceIDVendorName = NULL;
HMDRV_ADDR HMMODBUS_pu8ReadDeviceIDProductCode = NULL;
HMDRV_ADDR HMMODBUS_pu8ReadDeviceIDMajorMinorRevision = NULL;
HMDRV_ADDR HMMODBUS_pu8ReadDeviceIDVendorUrl = NULL;
HMDRV_ADDR HMMODBUS_pu8ReadDeviceIDProductName = NULL;
HMDRV_ADDR HMMODBUS_pu8ReadDeviceIDModelName = NULL;
HMDRV_ADDR HMMODBUS_pu8ReadDeviceIDUserApplicationName = NULL;

HMDRV_U8 HMMODBUS_u8ReadDeviceIDVendorNameLength = 16;
HMDRV_U8 HMMODBUS_u8ReadDeviceIDProductCodeLength = 16;
HMDRV_U8 HMMODBUS_u8ReadDeviceIDMajorMinorRevisionLength = 16;
HMDRV_U8 HMMODBUS_u8ReadDeviceIDVendorUrlLength = 16;
HMDRV_U8 HMMODBUS_u8ReadDeviceIDProductNameLength = 16;
HMDRV_U8 HMMODBUS_u8ReadDeviceIDModelNameLength = 16;
HMDRV_U8 HMMODBUS_u8ReadDeviceIDUserApplicationNameLength = 16;

HMDRV_ADDR HMMODBUS_pu8ReadDeviceIDUserReserve0 = NULL;
HMDRV_ADDR HMMODBUS_pu8ReadDeviceIDUserReserve1 = NULL;
HMDRV_ADDR HMMODBUS_pu8ReadDeviceIDUserReserve2 = NULL;


HMDRV_U8 HMMODBUS_u8ReadDeviceIDUserReserve0Length = 16;
HMDRV_U8 HMMODBUS_u8ReadDeviceIDUserReserve1Length = 16;
HMDRV_U8 HMMODBUS_u8ReadDeviceIDUserReserve2Length = 16;



/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
HMDRV_BOOL HMMODBUS_bDecoder(HMDRV_BPTR pMessageIO);
void HMMODBUS_vSendResponse(HMDRV_BPTR pResponse, HMDRV_SIZE8 nLength, HMDRV_BOOL bAddCheckSum);

HMDRV_U16 HMMODBUS_GetRealQueueNumber(HMDRV_U16 nNumber);

HMDRV_U16 HMMODBUS_GetRealFileNumber(HMDRV_U16 nNumber);



/* *****************************************************************************
 * Functions
 **************************************************************************** */

void HMMODBUS_vSetFreeRunTimerPrescaller(uint16_t u16Prescaller)
{
    uint32_t u32DeviceSYSCLKFreqNew;

    u32ModbusMaxByteToByte        = (uint32_t)(((uint64_t)(u32ModbusMaxByteToByte      * (u16FreeRunTimerPrescaller+1)) + ((u16Prescaller+1)>>1)) / (u16Prescaller+1));
    u32ModbusMaxMessToMess        = (uint32_t)(((uint64_t)(u32ModbusMaxMessToMess      * (u16FreeRunTimerPrescaller+1)) + ((u16Prescaller+1)>>1)) / (u16Prescaller+1));
    u32ModbusMinMessToMess        = (uint32_t)(((uint64_t)(u32ModbusMinMessToMess      * (u16FreeRunTimerPrescaller+1)) + ((u16Prescaller+1)>>1)) / (u16Prescaller+1));
    u32ModbusMaxByteToByteExact   = (uint32_t)(((uint64_t)(u32ModbusMaxByteToByteExact * (u16FreeRunTimerPrescaller+1)) + ((u16Prescaller+1)>>1)) / (u16Prescaller+1));
    u32ModbusMaxMessToMessExact   = (uint32_t)(((uint64_t)(u32ModbusMaxMessToMessExact * (u16FreeRunTimerPrescaller+1)) + ((u16Prescaller+1)>>1)) / (u16Prescaller+1));
    u32ModbusTimeout              = (uint32_t)(((uint64_t)(u32ModbusTimeout            * (u16FreeRunTimerPrescaller+1)) + ((u16Prescaller+1)>>1)) / (u16Prescaller+1));

    u32DeviceSYSCLKFreqNew = HMDRV_u32GetDeviceSYSCLKFreq();
    u16FreeRunTimerPrescaller = u16Prescaller;

    if (u32DeviceSYSCLKFreqNew != HMMODBUS_u32DeviceSYSCLKFreq)
    {
        u32ModbusMaxByteToByte      = (uint32_t)(((uint64_t)(u32ModbusMaxByteToByte      * HMMODBUS_u32DeviceSYSCLKFreq) + (u32DeviceSYSCLKFreqNew>>1)) / u32DeviceSYSCLKFreqNew);
        u32ModbusMaxMessToMess      = (uint32_t)(((uint64_t)(u32ModbusMaxMessToMess      * HMMODBUS_u32DeviceSYSCLKFreq) + (u32DeviceSYSCLKFreqNew>>1)) / u32DeviceSYSCLKFreqNew);
        u32ModbusMinMessToMess      = (uint32_t)(((uint64_t)(u32ModbusMinMessToMess      * HMMODBUS_u32DeviceSYSCLKFreq) + (u32DeviceSYSCLKFreqNew>>1)) / u32DeviceSYSCLKFreqNew);
        u32ModbusMaxByteToByteExact = (uint32_t)(((uint64_t)(u32ModbusMaxByteToByteExact * HMMODBUS_u32DeviceSYSCLKFreq) + (u32DeviceSYSCLKFreqNew>>1)) / u32DeviceSYSCLKFreqNew);
        u32ModbusMaxMessToMessExact = (uint32_t)(((uint64_t)(u32ModbusMaxMessToMessExact * HMMODBUS_u32DeviceSYSCLKFreq) + (u32DeviceSYSCLKFreqNew>>1)) / u32DeviceSYSCLKFreqNew);
        u32ModbusTimeout            = (uint32_t)(((uint64_t)(u32ModbusTimeout            * HMMODBUS_u32DeviceSYSCLKFreq) + (u32DeviceSYSCLKFreqNew>>1)) / u32DeviceSYSCLKFreqNew);

        HMMODBUS_u32DeviceSYSCLKFreq = u32DeviceSYSCLKFreqNew;
    }


}

void HMMODBUS_vSetServerIDDataAddress (HMDRV_ADDR pInput)
{
    HMMODBUS_pu8ReadServerIDDatAdr = pInput;
}

void HMMODBUS_vSetServerIDLength (HMDRV_U8 u8Input)
{
    HMMODBUS_u8ReadServerIDLength = u8Input;
}

void HMMODBUS_vSetServerIDNumber (HMDRV_U8 u8Input)
{
    HMMODBUS_u8ReadServerIDNumber = u8Input;
}

void HMMODBUS_vSetServerOnStatus (HMDRV_U8 u8Input)
{
    HMMODBUS_u8ReadServerOnStatus = u8Input;
}

void HMMODBUS_vSetDeviceIDVendorName (HMDRV_ADDR pInput)
{
    HMMODBUS_pu8ReadDeviceIDVendorName = pInput;
}
void HMMODBUS_vSetDeviceIDVendorNameLength (HMDRV_U8 u8Input)
{
    HMMODBUS_u8ReadDeviceIDVendorNameLength = u8Input;
}

void HMMODBUS_vSetDeviceIDProductCode (HMDRV_ADDR pInput)
{
    HMMODBUS_pu8ReadDeviceIDProductCode = pInput;
}
void HMMODBUS_vSetDeviceIDProductCodeLength (HMDRV_U8 u8Input)
{
    HMMODBUS_u8ReadDeviceIDProductCodeLength = u8Input;
}

void HMMODBUS_vSetDeviceIDMajorMinorRevision (HMDRV_ADDR pInput)
{
    HMMODBUS_pu8ReadDeviceIDMajorMinorRevision = pInput;
}
void HMMODBUS_vSetDeviceIDMajorMinorRevisionLength (HMDRV_U8 u8Input)
{
    HMMODBUS_u8ReadDeviceIDMajorMinorRevisionLength = u8Input;
}

void HMMODBUS_vSetDeviceIDVendorUrl (HMDRV_ADDR pInput)
{
    HMMODBUS_pu8ReadDeviceIDVendorUrl = pInput;
}
void HMMODBUS_vSetDeviceIDVendorUrlLength (HMDRV_U8 u8Input)
{
    HMMODBUS_u8ReadDeviceIDVendorUrlLength = u8Input;
}

void HMMODBUS_vSetDeviceIDProductName (HMDRV_ADDR pInput)
{
    HMMODBUS_pu8ReadDeviceIDProductName = pInput;
}
void HMMODBUS_vSetDeviceIDProductNameLength (HMDRV_U8 u8Input)
{
    HMMODBUS_u8ReadDeviceIDProductNameLength = u8Input;
}

void HMMODBUS_vSetDeviceIDModelName (HMDRV_ADDR pInput)
{
    HMMODBUS_pu8ReadDeviceIDModelName = pInput;
}
void HMMODBUS_vSetDeviceIDModelNameLength (HMDRV_U8 u8Input)
{
    HMMODBUS_u8ReadDeviceIDModelNameLength = u8Input;
}

void HMMODBUS_vSetDeviceIDUserApplicationName (HMDRV_ADDR pInput)
{
    HMMODBUS_pu8ReadDeviceIDUserApplicationName = pInput;
}
void HMMODBUS_vSetDeviceIDUserApplicationNameLength (HMDRV_U8 u8Input)
{
    HMMODBUS_u8ReadDeviceIDUserApplicationNameLength = u8Input;
}

void HMMODBUS_vSetDeviceIDUserReserve0 (HMDRV_ADDR pInput)
{
    HMMODBUS_pu8ReadDeviceIDUserReserve0 = pInput;
}
void HMMODBUS_vSetDeviceIDUserReserve0Length (HMDRV_U8 u8Input)
{
    HMMODBUS_u8ReadDeviceIDUserReserve0Length = u8Input;
}

void HMMODBUS_vSetDeviceIDUserReserve1 (HMDRV_ADDR pInput)
{
    HMMODBUS_pu8ReadDeviceIDUserReserve1 = pInput;
}
void HMMODBUS_vSetDeviceIDUserReserve1Length (HMDRV_U8 u8Input)
{
    HMMODBUS_u8ReadDeviceIDUserReserve1Length = u8Input;
}

void HMMODBUS_vSetDeviceIDUserReserve2 (HMDRV_ADDR pInput)
{
    HMMODBUS_pu8ReadDeviceIDUserReserve2 = pInput;
}
void HMMODBUS_vSetDeviceIDUserReserve2Length (HMDRV_U8 u8Input)
{
    HMMODBUS_u8ReadDeviceIDUserReserve2Length = u8Input;
}


void HMMODBUS_vSetupRegisterDummy (void)
{
    HMMODBUS_u32RegisterAddressDummy.u16VirtAddress = MODBUSREG_REGID_MODBUS_DUMMY_USAGE;
    //HMMODBUS_u32RegisterAddressDummy.u32RealAddress = (HMDRV_ADDR_SIZE)&u16RegisterAddressDummyValue;
    HMMODBUS_u32RegisterAddressDummy.uSize.sSize.u8SizeElement = 2;//sizeof(u16RegisterAddressDummyValue);
}


void HMMODBUS_vSetupCoilDummy (void)
{
    #ifndef MODBUSREG_COILID_MODBUS_DUMMY_USAGE
    #define MODBUSREG_COILID_MODBUS_DUMMY_USAGE 0xFFFE
    #endif

    HMMODBUS_u32CoilAddressDummy.u16VirtAddress = MODBUSREG_COILID_MODBUS_DUMMY_USAGE;

    HMMODBUS_u32CoilAddressDummy.u16BitOffset = 0;
}


void HMMODBUS_vSetupActiveQueueIndex(HMDRV_U8 u8Index)
{ 
    HMMODBUS_u8QueueActiveIndex = HMMODBUS_GetRealQueueNumber(u8Index);
}


void HMMODBUS_vSetupQueueRegistersAddressTable(HMMODBUS_sQueueTable_t* pTable, HMDRV_U32 u32Size)
{
    int index;
    
    HMMODBUS_pasQueueRegisterAddressTable = pTable;
    HMMODBUS_u32QueueRegisterAddressTableSize = u32Size;
    
    for (index = 0; index < u32Size; index++)
    {
        (HMMODBUS_pasQueueRegisterAddressTable + index)->u32RecordCountValid = 0xFFFFFFFF;
    }
}

void HMMODBUS_vSetupInputRegistersAddressTable(HMDRV_U32 u32TableAddress, HMDRV_U32 u32Size)
{
    HMMODBUS_pu32InputRegisterAddressTable = (HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sRegisterAddressTable_t*)u32TableAddress;
    HMMODBUS_u32InputRegisterAddressTableSize = u32Size;
}

void HMMODBUS_vSetupHoldingRegistersAddressTable(HMDRV_U32 u32TableAddress, HMDRV_U32 u32Size)
{
    HMMODBUS_pu32HoldingRegisterAddressTable = (HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sRegisterAddressTable_t*)u32TableAddress;
    HMMODBUS_u32HoldingRegisterAddressTableSize = u32Size;
}

void HMMODBUS_vSetupInputRegistersAddressTableInRam(HMMODBUS_sRegisterAddressTableInRam_t* pTable, HMMODBUS_sRegisterAccessTableInRam_t* pTableAccess, HMDRV_U32 u32Size)
{
    HMMODBUS_pu32InputRegisterAddressTableInRam = pTable;
    HMMODBUS_pu32InputRegisterAccessTableInRam = pTableAccess;
    HMMODBUS_u32InputRegisterAddressTableInRamSize = u32Size;
}

void HMMODBUS_vSetupHoldingRegistersAddressTableInRam(HMMODBUS_sRegisterAddressTableInRam_t* pTable, HMMODBUS_sRegisterAccessTableInRam_t* pTableAccess, HMDRV_U32 u32Size)
{
    HMMODBUS_pu32HoldingRegisterAddressTableInRam = pTable;
    HMMODBUS_pu32HoldingRegisterAccessTableInRam = pTableAccess;
    HMMODBUS_u32HoldingRegisterAddressTableInRamSize = u32Size;
}



void HMMODBUS_vSetupInputCoilsAddressTable(HMDRV_U32 u32TableAddress, HMDRV_U32 u32Size)
{
    HMMODBUS_pu32InputCoilAddressTable = (HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sCoilAddressTable_t*) u32TableAddress;
    HMMODBUS_u32InputCoilAddressTableSize = u32Size;
}

void HMMODBUS_vSetupHoldingCoilsAddressTable(HMDRV_U32 u32TableAddress, HMDRV_U32 u32Size)
{
    HMMODBUS_pu32HoldingCoilAddressTable = (HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sCoilAddressTable_t*) u32TableAddress;
    HMMODBUS_u32HoldingCoilAddressTableSize = u32Size;
}

void HMMODBUS_vSetupInputCoilsAddressTableInRam(HMMODBUS_sCoilAddressTableInRam_t* pTable, HMDRV_U32 u32Size)
{
    HMMODBUS_pu32InputCoilAddressTableInRam = pTable;
    HMMODBUS_u32InputCoilAddressTableInRamSize = u32Size;
}

void HMMODBUS_vSetupHoldingCoilsAddressTableInRam(HMMODBUS_sCoilAddressTableInRam_t* pTable, HMDRV_U32 u32Size)
{
    HMMODBUS_pu32HoldingCoilAddressTableInRam = pTable;
    HMMODBUS_u32HoldingCoilAddressTableInRamSize = u32Size;
}






void HMMODBUS_vFileTable(HMMODBUS_sFileTable_t* pTable, HMDRV_U32 u32Size)
{
    HMMODBUS_pasFileAddressTable = pTable;
    HMMODBUS_u32FileCount = u32Size;
}



void HMMODBUS_vFileTableSetValidRecord(HMDRV_U16 u16FileIndex, HMMODBUS_uFileValidData_t uValidData)
{
    (HMMODBUS_pasFileAddressTable + u16FileIndex)->uFileValidData.sFileValidData.u8RecordBytesValid = 0;
    (HMMODBUS_pasFileAddressTable + u16FileIndex)->uFileValidData.sFileValidData.u15RecordCountValid = uValidData.sFileValidData.u15RecordCountValid;
    (HMMODBUS_pasFileAddressTable + u16FileIndex)->uFileValidData.sFileValidData.u8RecordBytesValid = uValidData.sFileValidData.u8RecordBytesValid;
}



/**************************************************************************//*!
*
* @brief    Finalize transmit buffer before transmitting 
*
* @param    nLength - response length (1 for status + data length)
*
*
* This Function takes the data already prepared in the transmit buffer 
* (including the status byte). It computes the check sum and kicks on transmit.
*
******************************************************************************/

void HMMODBUS_vSendResponse(HMDRV_BPTR pResponse, HMDRV_SIZE8 nLength, HMDRV_BOOL bAddCheckSum)
{
    HMDRV_U16 chSum;
    HMDRV_U8 i, c;

    /* remember the buffer to be sent */
    HMMODBUS_pTxBuff = pResponse;
    
    HMMODBUS_nTxTodo = (HMDRV_SIZE8) (nLength);
    
    HMMODBUS_nLastResponseCmd = *(HMMODBUS_pTxBuff + 1);
    HMMODBUS_nLastResponseCode = *(HMMODBUS_pTxBuff + 2);
    
    if (bAddCheckSum)
    {
        /* status byte and data are already there, compute checksum only     */
        chSum = 0xFFFF;
        HMMODBUS_nTxCheckSum = chSum;
        for (i=0U; i<nLength; i++)
        {

            c = 0U;
            pResponse = HMDRV_ValueFromBuffer8(&c, pResponse);
            /* add character to checksum */
            chSum = HMMODBUS_CalcCRC(chSum, c);
            HMMODBUS_nTxCheckSum = chSum;
            HMMODBUS_anTxCheckSum[i] = chSum;
        }

        /* store checksum after the message */
        pResponse = HMDRV_ValueToBuffer8(pResponse, (HMDRV_U8)(chSum & 0x00FF));
        pResponse = HMDRV_ValueToBuffer8(pResponse, (HMDRV_U8)(chSum >> 8));
        
        HMMODBUS_nTxTodo += 2;
    }
    
    HMMODBUS_nRxTodoEcho = HMMODBUS_nTxTodo;
        
    /* now transmitting the response */
    HMMODBUS_wFlagsRxTx.sFlg.bTxActive = 1U;

    HMMODBUS_wFlagsRxTx.sFlg.bTxWaitTC = 0U;

    /* do not replicate the initial SOB  */
    //pcm_wFlags.sFlg.bTxLastCharSOB = 0U;  //not needed SOB
    
    /* disable receiver, enable transmitter (single-wire communication) */
    #if !HMDRV_SCI_TWOWIRE_ONLY
    HMDRV_SCI_RD(u8UartModBusIndex);
    #if HMDRV_TE_MIN_TIME == 0
    HMDRV_SCI_TE(u8UartModBusIndex);
    #endif
    #endif  

    HMMODBUS_nTxTodo--; /* one byte less to send - send first symbol does not decrease HMMODBUS_nTxTodo  */
    
#if HMDRV_SCI_TX_DELAY_ISR    
    HMDRV_SETUP_TX_DELAY_DATA(u8UartModBusIndex, *HMMODBUS_pTxBuff++);
    HMDRV_SETUP_TX_DELAY(u8UartModBusIndex, u16ModbusMinMessToMess_usec);
#elif HMDRV_MODBUS_TX_DELAY
    HMMODBUS_wFlagsRxTx.sFlg.bTxDelay = 1U;
    HMMODBUS_wFlagsRxTx.sFlg.bTxDelaySkipReduce = 1U;   /* reduce first time delay reduce */
    HMMODBUS_u32TxDelayCounter = u32ModbusMinMessToMess;
#else
    /* kick on the SCI transmission (also clears TX Empty flag on some platforms) */
    #if HMDRV_SCI_TX_ISR == 1
    HMDRV_TX_INT_ENA(u8UartModBusIndex);
    #endif
    #if HMDRV_TE_MIN_TIME
    HMDRV_SCI_TE(u8UartModBusIndex);
    #endif
    HMDRV_SCI_PUTCHAR(u8UartModBusIndex, *HMMODBUS_pTxBuff++);
    
#endif
}

void HMMODBUS_vProcessFastTD (void)
{
    if (HMMODBUS_nRxTodoEcho > 0)
    {
        HMMODBUS_nRxTodoEcho--;
        if (HMMODBUS_nRxTodoEcho == 0)
        {
            HMDRV_SCI_TD(u8UartModBusIndex);
        }
    }
}


#if 0
/**************************************************************************//*!
*
* @brief    Send response of given error code (no data) 
*
* @param    nErrCode - error code to be sent
*
******************************************************************************/

volatile void HMMODBUS_SendError(HMDRV_BCHR nErrCode)
{
    /* fill & send single-byte response */
    *HMMODBUS_pCommBufferTx = nErrCode;
    HMMODBUS_vSendResponse((HMDRV_BPTR)HMMODBUS_pCommBufferTx, 1U, HMDRV_TRUE);
}
#endif



/**************************************************************************//*!
*
* @brief    Output buffer transmission
* 
* @param  pTxChar  The character to be transmit 
*
* get ready buffer(prepare data to send)
*
******************************************************************************/

HMDRV_BOOL HMMODBUS_Tx(HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pTxChar)
{
    if (HMMODBUS_nTxTodo)
    {
        /* fetch & send character ready to transmit */
        /*lint -e{534} ignoring return value */
        (void)HMDRV_ValueFromBuffer8(pTxChar, HMMODBUS_pTxBuff);
        
        /* advance tx buffer pointer */
        HMMODBUS_nTxTodo--;
        HMMODBUS_pTxBuff = HMDRV_SkipInBuffer(HMMODBUS_pTxBuff, 1U);
        return HMDRV_FALSE;
    }
    
    /* when SCI TX buffering is enabled, we must first wait until all 
       characters are physically transmitted (before disabling transmitter) */

    HMMODBUS_wFlagsRxTx.sFlg.bTxWaitTC = 1;
    
    //IFS0bits.U1TXIF = 0;
    
    #if HMDRV_SCI_TX_ISR == 1
    HMDRV_TX_INT_DIS(u8UartModBusIndex);
    #endif
    
    /* start listening immediately if no Tx Queue */
    //HMDRV_Listen();


    return HMDRV_TRUE;
}



void HMMODBUS_vProcessRxDelay(HMDRV_TIMER nTimeTickDelay)
{
    HMDRV_U32 u32ModbusTimeoutTemp;
    
    u32ModbusTimeoutTemp = u32ModbusTimeout + nTimeTickDelay;
    if (u32ModbusTimeoutTemp < u32ModbusTimeout)
    {
        u32ModbusTimeoutTemp = 0xFFFFFFFF;
    }
    u32ModbusTimeout = u32ModbusTimeoutTemp;
}








/* Table of CRC values for high?order byte */
volatile const HMDRV_U8 auchCRCHi[256] =
{
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
} ;

/* Table of CRC values for low?order byte */
volatile const HMDRV_U8 auchCRCLo[] =
{
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
};

HMDRV_U16 HMMODBUS_CalcCRC(HMDRV_CRC nRxCheckSum,HMDRV_BCHR nRxChar)
{
    HMDRV_U8 uchCRCHi = (HMDRV_U8)(nRxCheckSum >> 8) ; /* high byte of CRC */
    HMDRV_U8 uchCRCLo = (HMDRV_U8)((nRxCheckSum & 0x00FF) >> 0) ; /* low byte of CRC */
    
    HMDRV_U8 uIndex = uchCRCLo ^ nRxChar;
    uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex];
    uchCRCHi = auchCRCLo[uIndex];
    HMDRV_U16 crc = ((HMDRV_U16)uchCRCHi << 8 | uchCRCLo) ;
    return crc;
}



HMDRV_U32 u32ModbusTimeoutBuffer[HMDRV_MODBUS_BUFFER_SIZE];
HMDRV_U32 u32ModbusTimeoutBufferIndex = 0;
#define USE_SWITCH_LIGHT    0
/**************************************************************************//*!
*
* @brief  Handle received character 
*
* @param  nRxChar  The character to be processed 
* 
* Handle the character received and -if the message is complete- call the 
* protocol decode routine. 
*
******************************************************************************/
volatile HMDRV_U32 u32RxTimeout;
volatile HMDRV_BOOL bNeedResetBackup = HMDRV_FALSE;
volatile HMDRV_U32 realRxByteDelay;
volatile HMDRV_U32 realRxByteDelayLastFail;
volatile HMDRV_U32 u32RxTimeoutLastFail;

volatile HMDRV_U16 realRxByteDelayLastFailBytesReceived = 0;

volatile HMDRV_U16 u16ModbusStartedReceiveMessageCounter = 0;
volatile HMDRV_U16 u16ModbusCompleteReceiveMessageCounter = 0;
HMDRV_U16 u16ModbusCompleteReceiveMessageCounterThisDevice = 0;

volatile HMDRV_U16 u16ModbusBufferOverflowReceiveMessageCounter = 0;
volatile HMDRV_U16 u16ModbusBadCSReceiveMessageCounter = 0;
volatile HMDRV_U16 u16ModbusBadTimeoutReceiveMessageCounter = 0;


HMDRV_BOOL HMMODBUS_Rx(HMDRV_BCHR nRxChar)
{
    
    
    /* ??? check code below !!! - was realRxByteDelay = UART1_ReadDataRxTimeDiff(); only*/
    #if (defined (HMDRV_SCI_RX_ISR) && HMDRV_SCI_RX_ISR)
    realRxByteDelay = UART_ReadDataRxTimeDiff((UART_eModule_t)u8UartModBusIndex);
    #else
    realRxByteDelay = 0;
    #endif
    
    #if HMDRV_MODBUS_DO_NOT_RESET_ON_TIMEOUT_IF_NOT_COMPLETED_PREV_RECEIVE
    HMDRV_BOOL bNeedReset = HMDRV_TRUE;
    #endif
    
    HMMODBUS_uFlags_t * pflg = &HMMODBUS_wFlags;
    
    u32RxTimeout = u32ModbusTimeout;
    u32ModbusTimeout = 0;
    
    
    if (u32ModbusTimeoutBufferIndex >= HMDRV_MODBUS_BUFFER_SIZE)
    {
        u32ModbusTimeoutBufferIndex = 0;
    }
    u32ModbusTimeoutBuffer[u32ModbusTimeoutBufferIndex++] = u32RxTimeout;
    if (u32ModbusTimeoutBufferIndex > 1)
    {
        //u16ModbusTimeoutBuffer[u16ModbusTimeoutBufferIndex] = 0;
    }
    
    
    /* First Byte Of a Message (Device Address) with timeout detection */
    if  (
#if 0    
            (u32RxTimeout >= u32ModbusMaxMessToMess)
#else
            (u32RxTimeout >= 0x80000000)
         || (realRxByteDelay >= u32ModbusMaxMessToMessExact) 
#endif
        )
    {
        /* check already started receive */
        if (HMMODBUS_pRxBuff != (HMDRV_BPTR)HMMODBUS_pCommBufferRx)
        {
            u16ModbusBadTimeoutReceiveMessageCounter++;
            u32RxTimeoutLastFail = u32RxTimeout;
            realRxByteDelayLastFail = realRxByteDelay;
            realRxByteDelayLastFailBytesReceived = HMMODBUS_pRxBuff-HMMODBUS_pCommBufferRx;
            
            #if HMDRV_MODBUS_RESEND_RECEIVED_DATA_ON_TIMEOUT_IF_NOT_COMPLETED_PREV_RECEIVE         
            #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
            HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)HMMODBUS_pCommBufferTx), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)HMMODBUS_pCommBufferRx), HMDRV_SIZEADDRINTBYTES(1));
            #endif
             HMDRV_ValueToBuffer8(HMMODBUS_pCommBufferTx+1, (*(HMMODBUS_pCommBufferRx+1) | HMMODBUS_ERR_CMD_OR_MASK));
            HMDRV_ConstToBuffer8(HMMODBUS_pCommBufferTx+2, HMMODBUS_ERR_RECEIVE_BYTE_TIMEOUT);
            HMMODBUS_vSendResponse(HMMODBUS_pCommBufferTx, 3, HMDRV_TRUE); /* debug error code */
            
            //#if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
            //HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)HMMODBUS_pCommBufferTx), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)HMMODBUS_pCommBufferRx), HMDRV_SIZEADDRINTBYTES(HMMODBUS_pRxBuff-HMMODBUS_pCommBufferRx));
            //#endif
            //HMMODBUS_vSendResponse(HMMODBUS_pCommBufferTx, HMMODBUS_pRxBuff-HMMODBUS_pCommBufferRx, HMDRV_FALSE); //do not send without double buffering
            #endif

            #if HMDRV_MODBUS_DO_NOT_RESET_ON_TIMEOUT_IF_NOT_COMPLETED_PREV_RECEIVE
            bNeedReset = HMDRV_FALSE;
            bNeedResetBackup = HMDRV_TRUE;
            #endif


            
        }
        else
        {
            u16ModbusStartedReceiveMessageCounter++;
        }
        
        //if (nRxChar == 0x47)
        {
        //    bNeedReset = HMDRV_FALSE;
        //    bNeedResetBackup = HMDRV_TRUE;
        }
        
        #if HMDRV_MODBUS_DO_NOT_RESET_ON_TIMEOUT_IF_NOT_COMPLETED_PREV_RECEIVE
        if (bNeedReset)
        #endif
        {
        /* reset receiving process */
        HMMODBUS_pRxBuff = (HMDRV_BPTR)HMMODBUS_pCommBufferRx;
        *HMMODBUS_pRxBuff++ = nRxChar;
        
        //u16ModbusTimeoutBufferIndex = 0;

        /* start computing the checksum */
        HMMODBUS_nRxCheckSum = 0xFFFF;
        HMMODBUS_nRxCheckSum = HMMODBUS_CalcCRC(HMMODBUS_nRxCheckSum, nRxChar);
        HMMODBUS_nRxTodo = 0U; 
        
        pflg->sFlg.bRxMsgCommand = 1;
        pflg->sFlg.bRxMsgLengthNext = 0U;
        pflg->sFlg.bRxMsgMEITypeNext = 0U;
        
        return HMDRV_FALSE;
        }
    }
    
    /* we are waiting for the length byte */
    if(pflg->sFlg.bRxMsgLengthNext)
    {
        /* this byte (Write Byte Count), total data length (N Bytes) and the checksum (2 Bytes) */
        HMMODBUS_nRxTodo = (HMDRV_SIZE8) (1U + ((HMDRV_SIZE8)nRxChar) + 2U);
        pflg->sFlg.bRxMsgLengthNext = 0U;

    }

    /* we are waiting for the MEI Type byte */
    #define HMMODBUS_D_TYPE_USE_FIX_LEN 0
    #define HMMODBUS_E_TYPE_LEN 2   /* ReadDevIDCode (1 Byte) + ObjectID (1 Byte) */
    #define HMMODBUS_D_TYPE_FIX_LEN 16   /* Fixed Length CanOpen Data */
    if(pflg->sFlg.bRxMsgMEITypeNext)
    {
        if (nRxChar == HMMODBUS_CMD_SUB_RD_DEVICE_ID)
        {
            /* this byte (MEI Type Byte), total data length (N Bytes) and the checksum (2 Bytes) */
            HMMODBUS_nRxTodo = (HMDRV_SIZE8) (1U + ((HMDRV_SIZE8)HMMODBUS_E_TYPE_LEN) + 2U);
        }
        else if (nRxChar == HMMODBUS_CMD_SUB_CANOPEN_GEN_REF)
        {
            #if HMMODBUS_D_TYPE_USE_FIX_LEN
            /* this byte (MEI Type Byte), total data length (N Bytes) and the checksum (2 Bytes) */
            HMMODBUS_nRxTodo = (HMDRV_SIZE8) (1U + ((HMDRV_SIZE8)HMMODBUS_D_TYPE_USE_FIX_LEN) + 2U);
            #else
            /* next byte is length */
            pflg->sFlg.bRxMsgLengthNext = 1U;
            HMMODBUS_nRxTodo = 2;
            #endif
        }
        else    /* error sub command */
        {
            HMMODBUS_nRxTodo = 2+1; /* this byte + CheckSum */
        }
        pflg->sFlg.bRxMsgMEITypeNext = 0U;

    }

   
    /* we are waiting for the Command byte */
    if(pflg->sFlg.bRxMsgCommand)
    {
        pflg->sFlg.bRxMsgCommand = 0;
        HMMODBUS_nRxTodo = 0;
        
        switch (nRxChar)
        {
            case HMMODBUS_CMD_ENCAPSULATED_INTERFACE:
                pflg->sFlg.bRxMsgMEITypeNext = 1U;
                HMMODBUS_nRxTodo += 2;/* as least 2 to get next time to bRxMsgMEITypeNext check */
                break;
            
            #if USE_SWITCH_LIGHT == 0                
            case HMMODBUS_CMD_RW_HOLD_REGS:
                pflg->sFlg.bRxMsgHeader = 1U;
                HMMODBUS_nRxTodo += 10-1; /* Without Write Byte Count */
                break;
              
            case HMMODBUS_CMD_WR_COILS:    
            case HMMODBUS_CMD_WR_HOLD_REGS:
                pflg->sFlg.bRxMsgHeader = 1U;
                HMMODBUS_nRxTodo += 6-1; /* Without Byte Count */
                break;
            #else
            case HMMODBUS_CMD_RW_HOLD_REGS:
                HMMODBUS_nRxTodo += 10-6;
            case HMMODBUS_CMD_WR_COILS:    
            case HMMODBUS_CMD_WR_HOLD_REGS:
                HMMODBUS_nRxTodo += 6-2;
            #endif
            case HMMODBUS_CMD_RD_FILE_RECORD:
            case HMMODBUS_CMD_WR_FILE_RECORD:
                pflg->sFlg.bRxMsgHeader = 1U;
                HMMODBUS_nRxTodo += 2-1; /* Without Byte Count */
                break;
                
            #if USE_SWITCH_LIGHT == 0                
            case HMMODBUS_CMD_WR_HOLD_REG_MASK:
                pflg->sFlg.bRxMsgHeader = 0U;
                HMMODBUS_nRxTodo += 7+2; /* With CheckSum */
                break;
                
            case HMMODBUS_CMD_RD_COILS:
            case HMMODBUS_CMD_RD_DISCRETE_INPUTS:
            case HMMODBUS_CMD_RD_HOLD_REGS:
            case HMMODBUS_CMD_RD_INPUT_REGS:
            case HMMODBUS_CMD_WR_COIL:
            case HMMODBUS_CMD_WR_HOLD_REG:
            case HMMODBUS_CMD_DIAGNOSTICS:
                pflg->sFlg.bRxMsgHeader = 0U;
                HMMODBUS_nRxTodo += 5+2; /* With CheckSum */
                break;
                
            case HMMODBUS_CMD_RD_FIFO_QUEUE:
                pflg->sFlg.bRxMsgHeader = 0U;
                HMMODBUS_nRxTodo += 3+2; /* With CheckSum */
                break;
            #else
            case HMMODBUS_CMD_RD_FILE_RECORD:
            case HMMODBUS_CMD_WR_FILE_RECORD:
                HMMODBUS_nRxTodo += 7-5;
            case HMMODBUS_CMD_RD_COILS:
            case HMMODBUS_CMD_RD_DISCRETE_INPUTS:
            case HMMODBUS_CMD_RD_HOLD_REGS:
            case HMMODBUS_CMD_RD_INPUT_REGS:
            case HMMODBUS_CMD_WR_COIL:
            case HMMODBUS_CMD_WR_HOLD_REG:
            case HMMODBUS_CMD_DIAGNOSTICS:
                HMMODBUS_nRxTodo += 5-3;
            case HMMODBUS_CMD_RD_FIFO_QUEUE:
                HMMODBUS_nRxTodo += 3-1;
            #endif
            case HMMODBUS_CMD_RD_EXCEPTION_STATUS:    
            case HMMODBUS_CMD_RD_EVENT_COUNTER: 
            case HMMODBUS_CMD_RD_EVENT_LOG:
            case HMMODBUS_CMD_RD_SERVER_ID:
            default:
                pflg->sFlg.bRxMsgHeader = 0U;
                HMMODBUS_nRxTodo += 1+2 /* This(Command Byte) With(+) CheckSum */;
                break;
        }
    }
    
    /* waiting for a data byte? */
    if(HMMODBUS_nRxTodo)
    {
        /* add this byte to checksum */
        HMMODBUS_nRxCheckSum = HMMODBUS_CalcCRC(HMMODBUS_nRxCheckSum, nRxChar);

        

        /* is there still a space in the buffer? */
        if(HMMODBUS_pRxBuff)
        {
            /*lint -e{946} pointer arithmetic is okay here (same array) */
            if(HMMODBUS_pRxBuff < (HMMODBUS_pCommBufferRx + HMDRV_MODBUS_BUFFER_SIZE))
            {
                /* store byte  */
                *HMMODBUS_pRxBuff++ = nRxChar;
                HMMODBUS_bOverflowReached = 0;
            }
            /* buffer is full! */
            else
            {
                /* NULL rx pointer means buffer overflow - but we still need */
                /* to receive all message characters (for the single-wire mode) */
                /* so keep "receiving" - but throw away all characters from now */
                HMMODBUS_pRxBuff = (HMMODBUS_pCommBufferRx + HMDRV_MODBUS_BUFFER_SIZE);
                HMMODBUS_bOverflowReached = 1;
            }
        }

        
        /* decrease number of expected bytes */
        HMMODBUS_nRxTodo--;
        
        
        /* last character */
        if(!HMMODBUS_nRxTodo)
        {
            
            /* was this end of header */
            if(pflg->sFlg.bRxMsgHeader)
            {
                pflg->sFlg.bRxMsgHeader = 0;
                pflg->sFlg.bRxMsgLengthNext = 1;
            }
            /* was it the last byte of the message (checksum)? */
            else
            {
                
                /* receive buffer overflow? */
                if((HMMODBUS_pRxBuff == (HMMODBUS_pCommBufferRx + HMDRV_MODBUS_BUFFER_SIZE)) && (HMMODBUS_bOverflowReached))
                {
                    u16ModbusBufferOverflowReceiveMessageCounter++;
                    #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                    HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)HMMODBUS_pCommBufferTx), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)HMMODBUS_pCommBufferRx), HMDRV_SIZEADDRINTBYTES(1));
                    #endif
                    #if HMDRV_MODBUS_RESEND_RECEIVED_DATA_ON_RECEIVE_BUFFER_OVERFLOW
                    HMDRV_ValueToBuffer8(HMMODBUS_pCommBufferTx+1, (*(HMMODBUS_pCommBufferRx+1) | HMMODBUS_ERR_CMD_OR_MASK));
                    HMDRV_ConstToBuffer8(HMMODBUS_pCommBufferTx+2, HMMODBUS_ERR_RECEIVE_BUFFER_OVERFLOW);
                    HMMODBUS_vSendResponse(HMMODBUS_pCommBufferTx, 3, HMDRV_TRUE); /* debug error code */
                    //#if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                    //HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)HMMODBUS_pCommBufferTx), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)HMMODBUS_pCommBufferRx), HMDRV_SIZEADDRINTBYTES(HMDRV_MODBUS_BUFFER_SIZE));
                    //#endif
                    //HMMODBUS_vSendResponse(HMMODBUS_pCommBufferTx, HMDRV_MODBUS_BUFFER_SIZE, HMDRV_FALSE);
                    #endif
                    HMMODBUS_pRxBuff = (HMDRV_BPTR)HMMODBUS_pCommBufferRx;
                    //HMMODBUS_SendError(HMPROTOCOL_STC_CMDTOOLONG);
                    
                    
                }
                /* checksum error? */
                else 
                {
                    if(HMMODBUS_nRxCheckSum  != 0U)
                    {
                        u16ModbusBadCSReceiveMessageCounter++;
                        #if HMDRV_MODBUS_RESEND_RECEIVED_DATA_ON_BAD_CS
                        /* re-send with fixed checksum */
                        #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                        HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)HMMODBUS_pCommBufferTx), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)HMMODBUS_pCommBufferRx), HMDRV_SIZEADDRINTBYTES(1));
                        #endif
                        HMDRV_ValueToBuffer8(HMMODBUS_pCommBufferTx+1, (*(HMMODBUS_pCommBufferRx+1) | HMMODBUS_ERR_CMD_OR_MASK));
                        HMDRV_ConstToBuffer8(HMMODBUS_pCommBufferTx+2, HMMODBUS_ERR_RECEIVE_BAD_CS);
                        HMMODBUS_vSendResponse(HMMODBUS_pCommBufferTx, 3, HMDRV_TRUE); /* debug error code */
                        //#if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                        //HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)HMMODBUS_pCommBufferTx), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)HMMODBUS_pCommBufferRx), HMDRV_SIZEADDRINTBYTES(HMMODBUS_pRxBuff-HMMODBUS_pCommBufferRx-2));
                        //#endif
                        //HMMODBUS_vSendResponse(HMMODBUS_pCommBufferTx, HMMODBUS_pRxBuff-HMMODBUS_pCommBufferRx-2, HMDRV_TRUE);
                        #endif
                        HMMODBUS_pRxBuff = (HMDRV_BPTR)HMMODBUS_pCommBufferRx;
                        //HMMODBUS_SendError(HMPROTOCOL_STC_CMDCSERR);
                    }
                    /* message is okay */
                    else 
                    {
                        u16ModbusCompleteReceiveMessageCounter++;
                        if (((*HMMODBUS_pCommBufferRx) & 0x00FF) == HMMODBUS_u8DeviceAddress)
                        {
                            u16ModbusCompleteReceiveMessageCounterThisDevice++;
                        }
                        /* do decode now! */
                        (void)HMMODBUS_bDecoder((HMDRV_BPTR)HMMODBUS_pCommBufferRx);
                        /* Decode protocol and send response in Poll function if in ISR */
                        //pflg->sFlg.bUsbReadyToDecode = 1U;
                        HMMODBUS_pRxBuff = (HMDRV_BPTR)HMMODBUS_pCommBufferRx;  /* here can swap if used 2 Rx Buffers */

                        HMMODBUS_u32FreeRunTimerReceivedMessage = HMDRV_GET_FREE_RUN_TIMER();
                        HMMODBUS_bReceivedMessage = HMDRV_TRUE;
                    }
                }
                return HMDRV_TRUE;
            }
        }
        
        

    }
    return HMDRV_FALSE;	
}

HMDRV_U16 HMMODBUS_GetRealQueueNumber(HMDRV_U16 nNumber)
{
    HMMODBUS_sQueueTable_t* pTable;
    HMDRV_U16 index;
    HMDRV_U16 u16Result;
    
    pTable = HMMODBUS_pasQueueRegisterAddressTable;
    u16Result = HMMODBUS_u32QueueRegisterAddressTableSize;
    
    for (index = 0; index < HMMODBUS_u32QueueRegisterAddressTableSize; index++)
    {
        if ((pTable+index)->u16VirtAddress == nNumber)
        {
            u16Result = index;
            break;
        }
    }
    return u16Result;
}

HMDRV_U16 HMMODBUS_GetRealFileNumber(HMDRV_U16 nNumber)
{
    HMMODBUS_sFileTable_t* pTable;
    HMDRV_U16 index;
    HMDRV_U16 u16Result;
    
    
    pTable = HMMODBUS_pasFileAddressTable;
    
    u16Result = HMMODBUS_u32FileCount;
    
    for (index = 0; index < HMMODBUS_u32FileCount; index++)
    {
        if ((pTable+index)->u16VirtAddress == (nNumber-1))
        {
            u16Result = index;
            break;
        }
    }
    return u16Result+1;
}

HMDRV_U32 HMMODBUS_GetRealRegisterAddressAndSize(HMDRV_U16 nAddr, HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* pu16RealSize, HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* pu16ElemSize, HMMODBUS_eRegCoilTypeAccessOptions eRegCoilTypeAccessOptions)
{
    volatile HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sRegisterAddressTable_t* pTable;
    volatile HMMODBUS_sRegisterAddressTableInRam_t* pTableInRam;
    volatile HMMODBUS_sRegisterAccessTableInRam_t* pTableInRamAccess;
    volatile HMDRV_U32 u32Size;
    volatile HMDRV_BOOL bSortedTable = HMDRV_FALSE;
    volatile HMDRV_U16 index;
    volatile HMDRV_U32 result = NULL;
    
    HMDRV_BOOL bReadRequest = HMDRV_FALSE;
    HMDRV_BOOL bInputRegs = HMDRV_FALSE;
    HMDRV_BOOL bFullAccess = HMDRV_FALSE;

    if (eRegCoilTypeAccessOptions & HMMODBUS_REG_COIL_INPUT_MASK)
    {
        bInputRegs = HMDRV_TRUE;
    }
    if (eRegCoilTypeAccessOptions & HMMODBUS_REG_COIL_READ_MASK)
    {
        bReadRequest = HMDRV_TRUE;
    }
    if (eRegCoilTypeAccessOptions & HMMODBUS_REG_COIL_FULL_ACCESS_MASK)
    {
        bFullAccess = HMDRV_TRUE;
    }

    if (bInputRegs)
    {
        pTable = HMMODBUS_pu32InputRegisterAddressTable;
        pTableInRam = HMMODBUS_pu32InputRegisterAddressTableInRam;
        pTableInRamAccess = HMMODBUS_pu32InputRegisterAccessTableInRam;
        u32Size = HMMODBUS_u32InputRegisterAddressTableSize;
        bSortedTable = bInputRegTableSorted;
    }
    else
    {
        pTable = HMMODBUS_pu32HoldingRegisterAddressTable;
        pTableInRam = HMMODBUS_pu32HoldingRegisterAddressTableInRam;
        pTableInRamAccess = HMMODBUS_pu32HoldingRegisterAccessTableInRam;
        u32Size = HMMODBUS_u32HoldingRegisterAddressTableSize;
        bSortedTable = bHoldingRegTableSorted;
    }
    
    
    if (bSortedTable)
    {
        if (nAddr < u32Size)
        {
            if (((pTable+nAddr)->uFlags.sFlags.bArray) && ((pTable+nAddr)->uFlags.sFlags.u8ElementCount > 1))
            {
                *pu16RealSize = (pTable+nAddr)->uSize.sSize.u8SizeElement * (pTable+nAddr)->uFlags.sFlags.u8ElementCount;
            }
            else
            {
                *pu16RealSize = (pTable+nAddr)->uSize.sSize.u8SizeElement;
            }
            *pu16ElemSize = (pTable+nAddr)->uSize.sSize.u8SizeElement;
            result = (pTableInRam+nAddr)->u32RealAddress;

        }
        else
        {
            bUsedDummyDataAddressInResponseData = HMDRV_TRUE;
            *pu16RealSize = HMMODBUS_u32RegisterAddressDummy.uSize.sSize.u8SizeElement;
            *pu16ElemSize = HMMODBUS_u32RegisterAddressDummy.uSize.sSize.u8SizeElement;
            result = HMMODBUS_u32RegisterAddressInRamDummy.u32RealAddress;
        }
    }
    else
    {
        for (index = 0; index < u32Size; index++)
        {
            if ((pTable+index)->u16VirtAddress == nAddr)
            {
                if (bReadRequest == HMDRV_FALSE)
                {
                    if ((pTableInRamAccess[index].uFlags.sFlags.bWriteDisabled == 0) || (bFullAccess))
                    {
                        if ((pTable+index)->uFlags.sFlags.bBitField)
                        {
                            *pu16RealSize = (pTable+index)->uFlags.sFlags.u8ElementCount;
                            *pu16RealSize |= ((pTable+index)->uSize.sSize.u8BitOffet * 256);
                            *pu16ElemSize = (pTable+index)->uSize.sSize.u8SizeElement;
                            *pu16ElemSize |= 0x8000;
                        }
                        else if (((pTable+index)->uFlags.sFlags.bArray) && ((pTable+index)->uFlags.sFlags.u8ElementCount > 1))
                        {
                            if ((pTable+index)->uSize.sSize.u8SizeElement == 0)
                            {
                                /* string size element default 1 byte */
                                *pu16RealSize = 1 * (pTable+index)->uFlags.sFlags.u8ElementCount;
                            }
                            else
                            {
                                *pu16RealSize = (pTable+index)->uSize.sSize.u8SizeElement * (pTable+index)->uFlags.sFlags.u8ElementCount;
                            }
                            *pu16ElemSize = (pTable+index)->uSize.sSize.u8SizeElement;
                        }
                        else
                        {
                            *pu16RealSize = (pTable+index)->uSize.sSize.u8SizeElement;
                            *pu16ElemSize = (pTable+index)->uSize.sSize.u8SizeElement;
                        }
                        result = (pTableInRam+index)->u32RealAddress;
                        break;
                    }
                }
                else
                {
                    if ((pTableInRamAccess[index].uFlags.sFlags.bReadDisabled == 0) || (bFullAccess))
                    {
                        if ((pTable+index)->uFlags.sFlags.bBitField)
                        {
                            *pu16RealSize = (pTable+index)->uFlags.sFlags.u8ElementCount;
                            *pu16RealSize |= ((pTable+index)->uSize.sSize.u8BitOffet * 256);
                            *pu16ElemSize = (pTable+index)->uSize.sSize.u8SizeElement;
                            *pu16ElemSize |= 0x8000;
                        }
                        else if (((pTable+index)->uFlags.sFlags.bArray) && ((pTable+index)->uFlags.sFlags.u8ElementCount > 1))
                        {
                            if ((pTable+index)->uSize.sSize.u8SizeElement == 0)
                            {
                                /* string size element default 1 byte */
                                *pu16RealSize = 1 * (pTable+index)->uFlags.sFlags.u8ElementCount;
                            }
                            else
                            {
                                *pu16RealSize = (pTable+index)->uSize.sSize.u8SizeElement * (pTable+index)->uFlags.sFlags.u8ElementCount;
                            }


                            *pu16ElemSize = (pTable+index)->uSize.sSize.u8SizeElement;
                        }
                        else
                        {
                            *pu16RealSize = (pTable+index)->uSize.sSize.u8SizeElement;
                            *pu16ElemSize = (pTable+index)->uSize.sSize.u8SizeElement;
                        }
                        result = (pTableInRam+index)->u32RealAddress;
                        break;
                    }
                    
                }
            }
        }
        
        if (result == NULL)
        {
            bUsedDummyDataAddressInResponseData = HMDRV_TRUE;
            *pu16RealSize = HMMODBUS_u32RegisterAddressDummy.uSize.sSize.u8SizeElement;
            *pu16ElemSize = HMMODBUS_u32RegisterAddressDummy.uSize.sSize.u8SizeElement;
            result = HMMODBUS_u32RegisterAddressInRamDummy.u32RealAddress;
        }
    }
    return result;
}



HMDRV_U32 HMMODBUS_GetRealCoilAddressAndOffset(HMDRV_U16 nAddr, HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* pu16RealOffset, HMMODBUS_eRegCoilTypeAccessOptions eRegCoilTypeAccessOptions)
{
    volatile HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sCoilAddressTable_t* pTable;
    volatile HMMODBUS_sCoilAddressTableInRam_t* pTableInRam;
    volatile HMDRV_U32 u32Size;
    volatile HMDRV_BOOL bSortedTable = HMDRV_FALSE;
    volatile HMDRV_U16 index;
    volatile HMDRV_U32 result = NULL;
    
    HMDRV_BOOL bReadRequest = HMDRV_FALSE;
    HMDRV_BOOL bInputRegs = HMDRV_FALSE;
    HMDRV_BOOL bFullAccess = HMDRV_FALSE;

    if (eRegCoilTypeAccessOptions & HMMODBUS_REG_COIL_INPUT_MASK)
    {
        bInputRegs = HMDRV_TRUE;
    }
    if (eRegCoilTypeAccessOptions & HMMODBUS_REG_COIL_READ_MASK)
    {
        bReadRequest = HMDRV_TRUE;
    }
    if (eRegCoilTypeAccessOptions & HMMODBUS_REG_COIL_FULL_ACCESS_MASK)
    {
        bFullAccess = HMDRV_TRUE;
    }

    if (bInputRegs)
    {
        pTable = HMMODBUS_pu32InputCoilAddressTable;
        pTableInRam = HMMODBUS_pu32InputCoilAddressTableInRam;
        u32Size = HMMODBUS_u32InputCoilAddressTableSize;
        bSortedTable = bInputRegTableSorted;
    }
    else
    {
        pTable = HMMODBUS_pu32HoldingCoilAddressTable;
        pTableInRam = HMMODBUS_pu32HoldingCoilAddressTableInRam;
        u32Size = HMMODBUS_u32HoldingCoilAddressTableSize;
        bSortedTable = bHoldingRegTableSorted;
    }
    
    
    if (bSortedTable)
    {
        if (nAddr < u32Size)
        {
            *pu16RealOffset = (pTable+nAddr)->u16BitOffset;
            result = (pTableInRam+nAddr)->u32RealAddress;
        }
        else
        {
            bUsedDummyDataAddressInResponseData = HMDRV_TRUE;
            *pu16RealOffset = HMMODBUS_u32CoilAddressDummy.u16BitOffset;
            result = HMMODBUS_u32CoilAddressInRamDummy.u32RealAddress;
        }
    }
    else
    {
        for (index = 0; index < u32Size; index++)
        {
            if ((pTable+index)->u16VirtAddress == nAddr)
            {
                if (bReadRequest == HMDRV_FALSE)
                {
                    if ((pTableInRam[index].uFlags.sFlags.bWriteDisabled == 0) || (bFullAccess))
                    {
                        *pu16RealOffset = (pTable+index)->u16BitOffset;
                        result = (pTableInRam+index)->u32RealAddress;
                        break;
                    }
                }
                else
                {
                    if ((pTableInRam[index].uFlags.sFlags.bReadDisabled == 0) || (bFullAccess))
                    {
                        *pu16RealOffset = (pTable+index)->u16BitOffset;
                        result = (pTableInRam+index)->u32RealAddress;
                        break;
                    }
                    
                }
            }
        }
        
        if (result == NULL)
        {
            bUsedDummyDataAddressInResponseData = HMDRV_TRUE;
            *pu16RealOffset = HMMODBUS_u32CoilAddressDummy.u16BitOffset;
            result = HMMODBUS_u32CoilAddressInRamDummy.u32RealAddress;
        }
    }
    return result;
}


HMDRV_U32 HMMODBUS_u32GetAddressMessageCounterThisDevice(void)
{
    HMDRV_U32 u32Result;
    u32Result = (uint32_t)(HMDRV_ADDR_SIZE)&u16ModbusCompleteReceiveMessageCounterThisDevice;
    return u32Result;
}



HMDRV_U32 HMMODBUS_GetRegisterFunction(HMDRV_U16 nAddr, HMDRV_BOOL bReadRequest, HMDRV_BOOL bInputRegs)
{
    volatile HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sRegisterAddressTable_t* pTable;
    volatile HMMODBUS_sRegisterAddressTableInRam_t* pTableInRam;
    volatile HMMODBUS_sRegisterAccessTableInRam_t* pTableInRamAccess;
    volatile HMDRV_U32 u32Size;
    volatile HMDRV_BOOL bSortedTable = HMDRV_FALSE;
    volatile HMDRV_U16 index;
    volatile HMDRV_U32 result = NULL;
    
    if (bInputRegs)
    {
        pTable = HMMODBUS_pu32InputRegisterAddressTable;
        pTableInRam = HMMODBUS_pu32InputRegisterAddressTableInRam;
        pTableInRamAccess = HMMODBUS_pu32InputRegisterAccessTableInRam;
        u32Size = HMMODBUS_u32InputRegisterAddressTableSize;
        bSortedTable = bInputRegTableSorted;
    }
    else
    {
        pTable = HMMODBUS_pu32HoldingRegisterAddressTable;
        pTableInRam = HMMODBUS_pu32HoldingRegisterAddressTableInRam;
        pTableInRamAccess = HMMODBUS_pu32HoldingRegisterAccessTableInRam;
        u32Size = HMMODBUS_u32HoldingRegisterAddressTableSize;
        bSortedTable = bHoldingRegTableSorted;
    }
    
    
    if (bSortedTable)
    {
        if (nAddr < u32Size)
        {

            result = (pTableInRam+nAddr)->u32PointerToProcessFunction;
        }
        else
        {
            bUsedDummyDataAddressInResponseData = HMDRV_TRUE;
            result = HMMODBUS_u32RegisterAddressInRamDummy.u32PointerToProcessFunction;
        }
    }
    else
    {
        for (index = 0; index < u32Size; index++)
        {
            if ((pTable+index)->u16VirtAddress == nAddr)
            {
                if (bReadRequest == HMDRV_FALSE)
                {
                    if (pTableInRamAccess[index].uFlags.sFlags.bWriteDisabled == 0)
                    {
                        result = (pTableInRam+index)->u32PointerToProcessFunction;
                        break;
                    }
                }
                else
                {
                    if (pTableInRamAccess[index].uFlags.sFlags.bReadDisabled == 0)
                    {
                        result = (pTableInRam+index)->u32PointerToProcessFunction;
                        break;
                    }
                    
                }
            }
        }
        
        if (result == NULL)
        {
            bUsedDummyDataAddressInResponseData = HMDRV_TRUE;
            result = HMMODBUS_u32RegisterAddressInRamDummy.u32PointerToProcessFunction;
        }
    }
    return result;
}


HMDRV_U32 HMMODBUS_GetCoilFunction(HMDRV_U16 nAddr, HMDRV_BOOL bReadRequest, HMDRV_BOOL bInputRegs)
{
    volatile HMDRV_CNST_PNTR_MODIFIER HMMODBUS_sCoilAddressTable_t* pTable;
    volatile HMMODBUS_sCoilAddressTableInRam_t* pTableInRam;
    volatile HMDRV_U32 u32Size;
    volatile HMDRV_BOOL bSortedTable = HMDRV_FALSE;
    volatile HMDRV_U16 index;
    volatile HMDRV_U32 result = NULL;
    
    if (bInputRegs)
    {
        pTable = HMMODBUS_pu32InputCoilAddressTable;
        pTableInRam = HMMODBUS_pu32InputCoilAddressTableInRam;
        u32Size = HMMODBUS_u32InputCoilAddressTableSize;
        bSortedTable = bInputRegTableSorted;
    }
    else
    {
        pTable = HMMODBUS_pu32HoldingCoilAddressTable;
        pTableInRam = HMMODBUS_pu32HoldingCoilAddressTableInRam;
        u32Size = HMMODBUS_u32HoldingCoilAddressTableSize;
        bSortedTable = bHoldingRegTableSorted;
    }
    
    
    if (bSortedTable)
    {
        if (nAddr < u32Size)
        {

            result = (pTableInRam+nAddr)->u32PointerToProcessFunction;
        }
        else
        {
            bUsedDummyDataAddressInResponseData = HMDRV_TRUE;
            result = HMMODBUS_u32CoilAddressInRamDummy.u32PointerToProcessFunction;
        }
    }
    else
    {
        for (index = 0; index < u32Size; index++)
        {
            if ((pTable+index)->u16VirtAddress == nAddr)
            {
                if (bReadRequest == HMDRV_FALSE)
                {
                    if (pTableInRam[index].uFlags.sFlags.bWriteDisabled == 0)
                    {
                        result = (pTableInRam+index)->u32PointerToProcessFunction;
                        break;
                    }
                }
                else
                {
                    if (pTableInRam[index].uFlags.sFlags.bReadDisabled == 0)
                    {
                        result = (pTableInRam+index)->u32PointerToProcessFunction;
                        break;
                    }
                    
                }
            }
        }
        
        if (result == NULL)
        {
            bUsedDummyDataAddressInResponseData = HMDRV_TRUE;
            result = HMMODBUS_u32CoilAddressInRamDummy.u32PointerToProcessFunction;
        }
    }
    return result;
}








/* *****************************************************************************
 * HMMODBUS_CopyFromBuffer
 * 
 * Description: ModBus Write Register. nSize - bytes
 **************************************************************************** */
HMMODBUS_eErrorCodes HMMODBUS_CopyFromBuffer(HMDRV_U16 nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
    volatile HMDRV_U32 pRealAddress;
    volatile HMDRV_U16 u16Size;
    volatile HMDRV_U16 u16ElemSize;
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    volatile HMDRV_U8* pd;
    volatile HMMODBUS_eErrorCodes eException = HMMODBUS_ERR_NO_EXCEPTION;

    if (bBareRamTable)
    {
        pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;
        ps = HMDRV_CopyFromBufferWithSuccess((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pd, ps, nSize);
        if (ps == NULL)
        {
            eException = HMMODBUS_ERR_SERVER_DEVICE_FAILURE;
        }
    }
    else
    {
        volatile HMDRV_SIZE8 regSize;
        volatile HMDRV_SIZE8 elemSize;
        //nSize >>= 1;    /* shrink size to register count */
        //while(nSize--) Number of 16-bit registers should be used in read request instead internal size
        {
            pRealAddress = HMMODBUS_GetRealRegisterAddressAndSize((HMDRV_U16)nDestAddr++, (HMDRV_U16*)&u16Size, (HMDRV_U16*)&u16ElemSize, HMMODBUS_REG_COIL_WRITE_HOLDING);//HMDRV_FALSE, bInputRegsGlobal

            if ((u16ElemSize & 0x8000) > 0) /* bit field */
            {
                volatile HMDRV_SIZE8 bitOffset;
                volatile HMDRV_SIZE8 bitCount;

                bitOffset = (HMDRV_SIZE8)(u16Size >> 8);
                bitCount = (HMDRV_SIZE8)(u16Size & 0x00FF);
                elemSize = (HMDRV_SIZE8)(u16ElemSize & 0x00FF);
                pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)pRealAddress;
                /* to do: check integrity of address data (HMMODBUS_ERR_ILLEGAL_ADDRESS) */
                /* to do: check integrity of written data (HMMODBUS_ERR_WRITE_FAILED) */


                u16ElemSize = (u16ElemSize + 1) & 0xFE;  /* must be fill up-to even for the comparison */


                if (nSize != u16ElemSize)
                {
                    eException = HMMODBUS_ERR_ILLEGAL_LENGTH;
                    //break;
                }
                else
                {

                    #if HMMODBUS_ILLEGAL_ADDR_STOP_WRITE_DATA_REG == 1
                    if (bUsedDummyDataAddressInResponseData == HMDRV_TRUE)
                    {
                        //break;
                    }
                    else
                    #endif
                    {
                        ps = HMDRV_CopyFromBufferModbusBitField((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pd, ps, bitCount, bitOffset, elemSize, NULL, HMDRV_FALSE);
                        if (ps == NULL)
                        {
                            eException = HMMODBUS_ERR_SERVER_DEVICE_FAILURE;
                            //break;
                        }
                    }
                }

            }
            else /* normal register or array */
            {
                regSize = (HMDRV_SIZE8)u16Size;
                elemSize = (HMDRV_SIZE8)u16ElemSize;
                pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)pRealAddress;
                /* to do: check integrity of address data (HMMODBUS_ERR_ILLEGAL_ADDRESS) */
                /* to do: check integrity of written data (HMMODBUS_ERR_WRITE_FAILED) */


                u16Size = (u16Size + 1) & 0xFE;  /* must be fill up-to even for the comparison */


                if (nSize != u16Size)
                {
                    eException = HMMODBUS_ERR_ILLEGAL_LENGTH;
                    //break;
                }
                else
                {

                    #if HMMODBUS_ILLEGAL_ADDR_STOP_WRITE_DATA_REG == 1
                    if (bUsedDummyDataAddressInResponseData == HMDRV_TRUE)
                    {
                        //break;
                    }
                    else
                    #endif
                    {
                        ps = HMDRV_CopyFromBufferModbus((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pd, ps, regSize, elemSize, NULL, HMDRV_FALSE);
                        if (ps == NULL)
                        {
                            eException = HMMODBUS_ERR_SERVER_DEVICE_FAILURE;
                            //break;
                        }
                    }
                }
            }
        }
    }
    return eException;
}

/* *****************************************************************************
 * HMMODBUS_CopyFromBuffer
 * 
 * Description: ModBus Write Register. nSize - bytes
 **************************************************************************** */
HMMODBUS_eErrorCodes HMMODBUS_CopyFromBufferCoil(HMDRV_U16 nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
    volatile HMDRV_U32 pRealAddress;

    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    volatile HMDRV_U8* pd;
    volatile HMMODBUS_eErrorCodes eException = HMMODBUS_ERR_NO_EXCEPTION;
    #if HMMODBUS_COILS_FROM_HOLDING_REGS == HMDRV_TRUE
    HMDRV_U16 u16Dummy;
    #endif
    HMDRV_U16 u16BitOffset;
    HMDRV_U8 nOffsetSrc;
    HMDRV_U16 u16BitCount;

    if (bBareRamTable)
    {
        pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;
        ps = HMDRV_CopyFromBufferWithSuccessCoil((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pd, ps, nSize);
        if (ps == NULL)
        {
            eException = HMMODBUS_ERR_SERVER_DEVICE_FAILURE;
        }
    }
    else
    {
        nOffsetSrc = 0;
        while(nSize > 0)
        {
            #if HMMODBUS_COILS_FROM_HOLDING_REGS == HMDRV_TRUE
            pRealAddress = HMMODBUS_GetRealRegisterAddressAndSize((HMDRV_U16)nDestAddr, (HMDRV_U16*)&u16BitOffset, &u16Dummy, HMMODBUS_REG_COIL_WRITE_HOLDING);
            u16BitCount = u16BitOffset & 0x00FF;
            u16BitOffset >>= 8;
            if (u16BitCount > nSize )
            {
                u16BitCount = nSize;
            }
            nSize -= u16BitCount;
            #else
            pRealAddress = HMMODBUS_GetRealCoilAddressAndOffset((HMDRV_U16)nDestAddr, (HMDRV_U16*)&u16BitOffset, HMMODBUS_REG_COIL_WRITE_HOLDING);//HMDRV_FALSE, bInputRegsGlobal
            u16BitCount = 1;//rework for more than one bit
            nSize--;
            #endif
            pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)pRealAddress;
            /* to do: check integrity of address data (HMMODBUS_ERR_ILLEGAL_ADDRESS) */
            /* to do: check integrity of written data (HMMODBUS_ERR_WRITE_FAILED) */

            #if HMMODBUS_ILLEGAL_ADDR_STOP_WRITE_DATA_REG == 1
            if (bUsedDummyDataAddressInResponseData == HMDRV_TRUE)
            {
                break;
            }
            #endif

            while (u16BitCount-- > 0)
            {


                ps = HMDRV_CopyFromBufferModbusCoil((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pd, ps, nOffsetSrc, u16BitOffset);
                nOffsetSrc++;
                if (nOffsetSrc >= 8)
                {
                    nOffsetSrc = 0;
                    ps++;
                }

                u16BitOffset++;
                while (u16BitOffset >= (8 * HMDRV_ADDR_WIDTH))
                {
                    u16BitOffset -= (8 * HMDRV_ADDR_WIDTH);
                    pd++;
                }

                if (ps == NULL)
                {
                    eException = HMMODBUS_ERR_SERVER_DEVICE_FAILURE;
                    break;
                }
            }
        }
    }
    return eException;
}

/* *****************************************************************************
 * HMMODBUS_CopyFromBuffer
 * 
 * Description: ModBus Write Register. nSize - bytes
 **************************************************************************** */
HMMODBUS_eErrorCodes HMMODBUS_CopyFromBufferSingleCoil(HMDRV_U16 nDestAddr, HMDRV_BPTR pSrcBuff)
{
    volatile HMDRV_U32 pRealAddress;
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    volatile HMDRV_U8* pd;
    volatile HMMODBUS_eErrorCodes eException = HMMODBUS_ERR_NO_EXCEPTION;
    HMDRV_U16 u16BitOffset;
    #if HMMODBUS_COILS_FROM_HOLDING_REGS == HMDRV_TRUE
    HMDRV_U16 u16Dummy;
    #endif


    if (bBareRamTable)
    {
        pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;
        ps = HMDRV_CopyFromBufferWithSuccessSingleCoil((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pd, ps);
        if (ps == NULL)
        {
            eException = HMMODBUS_ERR_SERVER_DEVICE_FAILURE;
        }
    }
    else
    {
        
        {
            #if HMMODBUS_COILS_FROM_HOLDING_REGS == HMDRV_TRUE
            pRealAddress = HMMODBUS_GetRealRegisterAddressAndSize((HMDRV_U16)nDestAddr, (HMDRV_U16*)&u16BitOffset, &u16Dummy, HMMODBUS_REG_COIL_WRITE_HOLDING);
            u16BitOffset >>= 8;
            #else
            pRealAddress = HMMODBUS_GetRealCoilAddressAndOffset((HMDRV_U16)nDestAddr, (HMDRV_U16*)&u16BitOffset, HMMODBUS_REG_COIL_WRITE_HOLDING);//HMDRV_FALSE, bInputRegsGlobal
            #endif
            pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)pRealAddress;
            /* to do: check integrity of address data (HMMODBUS_ERR_ILLEGAL_ADDRESS) */
            /* to do: check integrity of written data (HMMODBUS_ERR_WRITE_FAILED) */

            #if HMMODBUS_ILLEGAL_ADDR_STOP_WRITE_DATA_REG == 1
            if (bUsedDummyDataAddressInResponseData == HMDRV_TRUE)
            {
                //break;        to do: check error response code may be below ???
            }
            else
            #endif
            {
            volatile HMDRV_SIZE8 coilOffset;
            coilOffset = (HMDRV_SIZE8)u16BitOffset;

            ps = HMDRV_CopyFromBufferModbusSingleCoil((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pd, ps, coilOffset);
            if (ps == NULL)
            {
                eException = HMMODBUS_ERR_SERVER_DEVICE_FAILURE;
                //break;
            }
            }
        }
    }
    return eException;
}

/* *****************************************************************************
 * HMMODBUS_CopyToBuffer
 * 
 * Description: ModBus Read Register. 
 *      nSize - count registers (#if bBareRamTable == 0)
 *      nSize - count bytes     (#if bBareRamTable == 1)
 *                                          
 * 
 **************************************************************************** */

HMDRV_BPTR HMMODBUS_CopyToBuffer(HMDRV_BPTR pDestBuff, HMDRV_U16 nSrcAddr, HMDRV_SIZE8 nSize)
{
    volatile HMDRV_U32 pRealAddress;
    HMDRV_U16 u16Size;
    HMDRV_U16 u16ElemSize;

    HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;
    volatile HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* ps;
 	
    if (bBareRamTable)
    {
        ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;// was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr
        pd = HMDRV_CopyToBufferWithSuccess(pd, ((HMDRV_ADDR)(HMDRV_ADDR_SIZE_LONG)ps), nSize);
    }
    else
    {
        volatile HMDRV_SIZE8 regSize;
        volatile HMDRV_SIZE8 elemSize;
        //while(nSize--) Number of 16-bit registers should be used in read request instead internal size
        nSize <<= 1;    //bytes
        {
            HMMODBUS_eRegCoilTypeAccessOptions eRegCoilTypeAccessOptions;
            if (bInputRegsGlobal)
            {
                eRegCoilTypeAccessOptions = HMMODBUS_REG_COIL_READ_INPUT;
            }
            {
                eRegCoilTypeAccessOptions = HMMODBUS_REG_COIL_READ_HOLDING;
            }

            pRealAddress = HMMODBUS_GetRealRegisterAddressAndSize((HMDRV_U16)nSrcAddr, (HMDRV_U16*)&u16Size, (HMDRV_U16*)&u16ElemSize, eRegCoilTypeAccessOptions);
            nSrcAddr++;
            if ((u16ElemSize & 0x8000) > 0) /* bit field */
            {
                regSize = (HMDRV_SIZE8)u16ElemSize & 0x00FF;
                elemSize = (HMDRV_SIZE8)u16ElemSize & 0x00FF;
            }
            else
            {
                regSize = (HMDRV_SIZE8)u16Size & 0x00FF;
                elemSize = (HMDRV_SIZE8)u16ElemSize & 0x00FF;
            }
            ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)pRealAddress;
            
            u16Size = (u16Size + 1) & 0xFE;  /* must be fill up-to even for the comparison */

            if (nSize != u16Size)
            {
                bLengthException = HMDRV_TRUE;
                pd = NULL;
                //break;
            }
            else
            {
                pd = HMDRV_CopyToBufferModbus(pd, (HMDRV_ADDR)(HMDRV_ADDR_SIZE_LONG)ps, regSize, elemSize, NULL); //was pd = HMDRV_CopyToBufferModbus(pd, (HMDRV_ADDR)(HMDRV_ADDR_SIZE_SHORT)ps, regSize, regSize, NULL)
            }


            //if (pd == NULL)
            //{
            //    break;
            //}
        }
    }
    return (HMDRV_BPTR) pd;
}


/* *****************************************************************************
 * HMMODBUS_CopyToBufferCoil
 * 
 * Description: ModBus Read Register. 
 *      nSize - count registers (#if bBareRamTable == 0)
 *      nSize - count bytes     (#if bBareRamTable == 1)
 *                                          
 * 
 **************************************************************************** */

HMDRV_BPTR HMMODBUS_CopyToBufferCoil(HMDRV_BPTR pDestBuff, HMDRV_U16 nSrcAddr, HMDRV_SIZE8 nSize)
{
    volatile HMDRV_U32 pRealAddress;
    HMDRV_U16 u16BitOffset;
    HMDRV_U16 u16BitOffsetDest;
    #if HMMODBUS_COILS_FROM_HOLDING_REGS == HMDRV_TRUE
    HMDRV_U16 u16Dummy;
    #endif
    HMDRV_U16 u16BitCount;

    HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;
    volatile HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* ps;
 	
    if (bBareRamTable)
    {
        ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;// was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr
        pd = HMDRV_CopyToBufferWithSuccessCoil(pd, (HMDRV_ADDR)(HMDRV_ADDR_SIZE_LONG)ps, nSize); //was pd = HMDRV_CopyToBufferWithSuccess(pd, (HMDRV_ADDR)(HMDRV_ADDR_SIZE_SHORT)ps, nSize)
    }
    else
    {
        u16BitOffsetDest = 0;
        volatile HMDRV_SIZE8 coilOffset;
        while(nSize > 0)
        {
            HMMODBUS_eRegCoilTypeAccessOptions eRegCoilTypeAccessOptions;
            if (bInputRegsGlobal)
            {
                eRegCoilTypeAccessOptions = HMMODBUS_REG_COIL_READ_INPUT;
            }
            {
                eRegCoilTypeAccessOptions = HMMODBUS_REG_COIL_READ_HOLDING;
            }
            #if HMMODBUS_COILS_FROM_HOLDING_REGS == HMDRV_TRUE
            pRealAddress = HMMODBUS_GetRealRegisterAddressAndSize((HMDRV_U16)nSrcAddr, (HMDRV_U16*)&u16BitOffset, &u16Dummy, eRegCoilTypeAccessOptions);
            u16BitCount = u16BitOffset & 0x00FF;
            u16BitOffset >>= 8;
            if (u16BitCount > nSize )
            {
                u16BitCount = nSize;
            }
            nSize -= u16BitCount;
            #else
            pRealAddress = HMMODBUS_GetRealCoilAddressAndOffset((HMDRV_U16)nSrcAddr, (HMDRV_U16*)&u16BitOffset, eRegCoilTypeAccessOptions);
            u16BitCount = 1;//rework for more than one bit
            nSize--;
            #endif
            nSrcAddr++;
            coilOffset = (HMDRV_SIZE8)u16BitOffset;
            ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)pRealAddress;
            
            while (u16BitCount-- > 0)
            {
                (void)HMDRV_CopyToBufferModbusCoil(pd, (HMDRV_ADDR)(HMDRV_ADDR_SIZE_LONG)ps, coilOffset, u16BitOffsetDest);

                coilOffset++;
                while (coilOffset >= (8 * HMDRV_ADDR_WIDTH))
                {
                    ps++;
                    coilOffset -= (8 * HMDRV_ADDR_WIDTH);
                }

                u16BitOffsetDest++;
                if (u16BitOffsetDest >= 8)
                {
                    u16BitOffsetDest = 0;
                    pd++;
                }
            }
            
            if (pd == NULL)
            {
                break;
            }
        }
        if (u16BitOffsetDest != 0)
        {
            pd++;
        }
    }
    return (HMDRV_BPTR) pd;
}


/* *****************************************************************************
 * HMMODBUS_CopyToBufferCoil
 * 
 * Description: ModBus Read Register. 
 *      nSize - count registers (#if bBareRamTable == 0)
 *      nSize - count bytes     (#if bBareRamTable == 1)
 *                                          
 * 
 **************************************************************************** */

HMDRV_BPTR HMMODBUS_CopyToBufferSingleCoil(HMDRV_BPTR pDestBuff, HMDRV_U16 nSrcAddr)
{
    volatile HMDRV_U32 pRealAddress;
    HMDRV_U16 u16BitOffset;
    #if HMMODBUS_COILS_FROM_HOLDING_REGS == HMDRV_TRUE
    HMDRV_U16 u16Dummy;
    #endif

    HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;
    volatile HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* ps;
 	
    if (bBareRamTable)
    {
        ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;// was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr
        pd = HMDRV_CopyToBufferWithSuccessSingleCoil(pd, (HMDRV_ADDR)(HMDRV_ADDR_SIZE_LONG)ps); //was pd = HMDRV_CopyToBufferWithSuccess(pd, (HMDRV_ADDR)(HMDRV_ADDR_SIZE_SHORT)ps, nSize)
    }
    else
    {
        volatile HMDRV_SIZE8 coilOffset;
        {
            HMMODBUS_eRegCoilTypeAccessOptions eRegCoilTypeAccessOptions;
            if (bInputRegsGlobal)
            {
                eRegCoilTypeAccessOptions = HMMODBUS_REG_COIL_READ_INPUT;
            }
            {
                eRegCoilTypeAccessOptions = HMMODBUS_REG_COIL_READ_HOLDING;
            }

            #if HMMODBUS_COILS_FROM_HOLDING_REGS == HMDRV_TRUE
            pRealAddress = HMMODBUS_GetRealRegisterAddressAndSize((HMDRV_U16)nSrcAddr, (HMDRV_U16*)&u16BitOffset, &u16Dummy, eRegCoilTypeAccessOptions);
            u16BitOffset >>= 8;
            #else
            pRealAddress = HMMODBUS_GetRealCoilAddressAndOffset((HMDRV_U16)nSrcAddr, (HMDRV_U16*)&u16BitOffset, eRegCoilTypeAccessOptions);
            #endif
            nSrcAddr++;
            coilOffset = (HMDRV_SIZE8)u16BitOffset;
            ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)pRealAddress;
            
            pd = HMDRV_CopyToBufferModbusSingleCoil(pd, (HMDRV_ADDR)(HMDRV_ADDR_SIZE_LONG)ps, coilOffset); 
        }
    }
    return (HMDRV_BPTR) pd;
}



#if 0
/* *****************************************************************************
 * HMMODBUS_CopyQueueToBufferInternal
 * 
 * Description: ModBus Read Register. 
 *      nSize - count registers (#if bBareRamTable == 0)
 *      nSize - count bytes     (#if bBareRamTable == 1)
 *                                          
 * 
 **************************************************************************** */
HMDRV_BPTR HMMODBUS_CopyQueueToBufferInternal(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSizeBytes)
{
    HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;
    HMDRV_U8* ps;
 	

    HMDRV_SIZE8 regSize;
    while(nSizeBytes > 0)
    {
        regSize = (HMDRV_SIZE8)nSizeBytes;
        ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;

        if (regSize >= 2)
        {
            *pd++ = *(ps+1);
            *pd++ = *(ps+0);
            nSizeBytes -= 2;
        }
        else /* if (regSize == 1) */
        {
            *pd++ = 0;   /* Zero High Byte in Buffer */
            *pd++ = *(ps+0);
            nSizeBytes -= 1;
        }
        nSrcAddr += 2;
    }
   
    return (HMDRV_BPTR) pd;
}

/* *****************************************************************************
 * HMMODBUS_CopyQueueToBuffer
 * 
 * Description: memcpy function when used external memory external module
 **************************************************************************** */
HMDRV_BPTR HMMODBUS_CopyQueueToBuffer(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_U8* pd;
    
    
    if (nSrcAddr >= EXT_MEM_MIN_VIRTUAL_ADDRESS)
    {
        if (nSrcAddr >= EXT_MEM_SPI_MIN_VIRTUAL_ADDRESS)
        {
            SPIMEM_vCopyFromExtMemRamBigEndian(pDestBuff, nSrcAddr, nSize);
        }
        else
        {
            #if USE_SPI_FLIR
            if (nSrcAddr >= EXT_MEM_FLIR_CCI_TWI_VIRTUAL_ADDRESS)
            {
                I2CMEM_vCopyFromFlirRegBigEndian(pDestBuff, nSrcAddr, nSize);
            }
            else
            #endif
            if (nSrcAddr >= EXT_MEM_I2C_VIRTUAL_ADDRESS)
            {
                I2CMEM_vCopyFromExtMemBigEndian(pDestBuff, nSrcAddr, nSize);
            }
            else
            {
                #if HMDRV_USE_EXT_PMP
                PMPMEM_vCopyFromExtMemBigEndian(pDestBuff, nSrcAddr, nSize)
                #endif
            }
        }
        
        pd = (HMDRV_U8*) pDestBuff;
        pd += nSize;
    }
    else
    {
        pd = HMMODBUS_CopyQueueToBufferInternal(pDestBuff, (HMDRV_ADDR)((HMDRV_ADDR_SIZE)nSrcAddr), nSize);
    }
    return (HMDRV_BPTR) pd;
}


#endif







#if 0
volatile HMDRV_FILE_PNTR_MODIFIER HMDRV_U8* psTest;


/* *****************************************************************************
 * HMMODBUS_CopyFileRecordToBuffer
 * 
 * Description: ModBus Read Register. nSize - count bytes
 **************************************************************************** */
HMDRV_BPTR HMMODBUS_CopyFileRecordToBuffer(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_U32 pRealAddress;

    HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;
    HMDRV_FILE_PNTR_MODIFIER HMDRV_U8* ps  ;
 	
    HMDRV_SIZE8 regSize;
    while(nSize > 0)
    {
        
        pRealAddress = nSrcAddr;
        nSrcAddr += 2;
        regSize = (HMDRV_SIZE8)nSize;
        ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)pRealAddress;
        psTest = (HMDRV_U8*)(HMDRV_ADDR_SIZE)pRealAddress;
        if (regSize >= 2)
        {
            *pd++ = *(ps+1);
            *pd++ = *(ps+0);
            nSize -= 2;
        }
        else /* if (regSize == 1) */
        {
            *pd++ = 0;   /* Zero High Byte in Buffer */
            *pd++ = *(ps+0);
            nSize--;
        }
        
    }
    return (HMDRV_BPTR) pd;
}

#endif

/* *****************************************************************************
 * HMMODBUS_CopyFileRecordFromBuffer
 * 
 * Description: ModBus Write Register. nSize - bytes
 **************************************************************************** */
HMDRV_BOOL HMMODBUS_CopyFileRecordFromBuffer(HMDRV_U32 nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
    HMDRV_U32 pRealAddress;
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_FILE_PNTR_MODIFIER HMDRV_U8* pd;
    HMMODBUS_eErrorCodes eException = HMMODBUS_ERR_NO_EXCEPTION;
    
    HMDRV_SIZE8 regSize;

    while(nSize > 0)
    {
        pRealAddress = nDestAddr;
        nDestAddr += 2;
        regSize = (HMDRV_SIZE8)nSize;
        pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)pRealAddress;
        /* to do: check integrity of address data (HMMODBUS_ERR_ILLEGAL_ADDRESS) */
        /* to do: check integrity of written data (HMMODBUS_ERR_WRITE_FAILED) */
        
        if (regSize >= 2)
        {
            *(pd+1) = *ps++;
            *(pd+0) = *ps++;
            nSize -= 2;
        }
        else /* if (regSize == 1) */
        {
            ps++;   /* Skip High Byte in Buffer */
            *(pd+0) = *ps++;
            nSize--;
        }
    }
    return eException;
}



/* *****************************************************************************
 * HMMODBUS_bDecoder
 * 
 * Description: ModBus Protocol Decoder
 **************************************************************************** */
HMDRV_BOOL HMMODBUS_bDecoder(HMDRV_BPTR pMessageIO)
{
    //HMDRV_U32 pRealAddress;
    //HMDRV_U16 u16Size;

	volatile HMDRV_BOOL bStringNullTerminationPossible = HMDRV_FALSE;
    volatile HMDRV_ADDR nAddrWrite = 0;
	volatile HMDRV_BOOL bStringDataExpected = HMDRV_FALSE;
	volatile HMDRV_BOOL bException = HMDRV_FALSE;
	volatile HMDRV_BOOL bResult = HMDRV_FALSE;
	volatile HMDRV_BOOL bParameterTroughFile;
    volatile HMDRV_U8 nDeviceAddress;
    volatile HMDRV_U16 nAddrQueue;
    volatile HMDRV_U16 nFileNumber;
    volatile HMDRV_U16 nSizeReadBits;
    
    volatile HMDRV_U16 u16ElementSize;
    volatile HMDRV_U8 u8ElementSize;
    HMDRV_U8* pu8ElementSizeTable;
    HMDRV_CNST_PNTR_MODIFIER HMDRV_U8* pu8ElementSizeTablePtr;

    volatile HMDRV_U8 nLoopIndex;
    volatile HMDRV_U8 nLoopIndex2;
    volatile HMDRV_U8 nLoopCount;

    volatile HMDRV_U16 nSizeWriteBits;
    volatile HMDRV_U32 nAddrFuncExecute;
    volatile HMDRV_BOOL bReadOnlyFile;
    volatile HMDRV_U16 nResponseBytes;
    volatile HMDRV_U16 nRecordNumber;
    volatile HMDRV_U16 nSizeRead;
    volatile HMDRV_U16 nBytesRead;
    volatile HMDRV_U16 nSizeReadCheck;
    volatile HMDRV_U16 nSizeReadRequest;
    volatile HMDRV_U16 nSizeBytes;
    volatile HMDRV_U32 nSizeOffset;                     //was 16
    volatile HMDRV_U16 nBytesResponseTotal;
    volatile HMDRV_U32 nSizeAllRecords;                 //was 16
    volatile HMDRV_U16 nSizeCurrentRecord;
    volatile HMDRV_U32 nSizeOffsetCurrentRecord;        //was 16
    
    CSMON_pfIndexGeneration pfIndexGeneration = NULL;
    HMDRV_U32 nAddrRead = 0;    /* Porting to Delfino - was:  HMDRV_ADDR nAddrRead = 0; */
    volatile HMDRV_U16 nSizeWrite;
    volatile HMDRV_U8 nSizeWriteBytesAllParts;
    volatile HMDRV_U8 nReferenceType;
    volatile HMDRV_U8 nSizeWriteBytes;
    volatile HMDRV_U8 nSizeReadBytes;
    //HMDRV_U16 nAndMask;
    //HMDRV_U16 nOrMask;
    //HMDRV_U16 nValueWrite;
    volatile HMDRV_U8 nCmd;
    volatile HMDRV_U8 nSubCmd;
    volatile HMDRV_U8 nIDCode;
    volatile HMDRV_BPTR pResponseStart;
    volatile HMDRV_BPTR pResponseEnd;
    volatile HMDRV_BPTR pMessageIn;
    volatile HMDRV_BPTR pMessageOut;
    volatile HMDRV_BPTR pMessageInCopy;
    //HMDRV_BPTR pValueWrite;
    volatile HMMODBUS_eErrorCodes eException;
    
    volatile HMDRV_U32 nSizeOffsetMultiplier;
    volatile HMDRV_U32 nSizeOffsetRequestMultMode;
    volatile HMDRV_U16 u16LastFileNumberRequest;
    volatile HMDRV_BOOL bFileFirstRequest;

    volatile HMDRV_U16 nRecordNumberRequest;
    volatile HMDRV_U16 nSizeReadRequestLoop;
    volatile HMDRV_U16 nLoopIndexedRequest;

    pMessageIn = pMessageIO;
    /* Backup start Buffer address */
    #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
    //HMMODBUS_pTxBuff = HMMODBUS_pCommBufferTx;
    pMessageOut = HMMODBUS_pCommBufferTx; /* swap Tx Buffers can be implemented here */
    #else
    pMessageOut = pMessageIn;
    #endif
    pResponseStart = pMessageOut;
    pResponseEnd = pMessageOut;
    
     /* no EX access by default */
    HMDRV_SetExAddr(HMDRV_FALSE);
    
   /* Device Address code comes first in the message */
    pMessageIn = HMDRV_ValueFromBuffer8(&nDeviceAddress, pMessageIn);

    /* Device address ignore if needed */
    #if HMDRV_MODBUS_RESPONSE_TO_OTHER_ADDRESSES == 0
    if (nDeviceAddress == HMMODBUS_u8DeviceAddress)
    #endif
    {

        /* Response starts with Device Address */
        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, HMMODBUS_u8DeviceAddress);       /* always return device address ??? */

        /* command */
        pMessageIn = HMDRV_ValueFromBuffer8(&nCmd, pMessageIn);
        


        /* Response leaves the command as is - made below if needed */
        //pResponseEnd = HMDRV_SkipInBuffer(pResponseEnd, 1U);

        /* default - use holding registers */
        bInputRegsGlobal = HMDRV_FALSE;
        bBareRamTable = bHoldingRegTableBareRam;
        bUsedDummyDataAddressInResponseData = HMDRV_FALSE;
        eException = HMMODBUS_ERR_NO_EXCEPTION;

        switch (nCmd)
        {
            case HMMODBUS_CMD_WR_FILE_RECORD:

                /* message bytes count */
                pMessageIn = HMDRV_ValueFromBuffer8(&nSizeReadBytes, pMessageIn);

                /* response is the same as input if no error */

                //nBytesResponseTotal = 1+1+1;            
                nResponseBytes = 0;

                /* check message bytes count good */
                if((nSizeReadBytes < 9) || (nSizeReadBytes > 0xFB))    
                {
                    /* Error message bytes count */
                    pResponseEnd = pResponseStart + 1;
                    pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                    pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_LENGTH);
                    bException = HMDRV_TRUE;
                    //pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nSizeReadBytes); was for testing
                }
                else
                {

                    /* Advance with Func Code, SizeBytes */
                    #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                    HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pResponseEnd), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(pMessageIn - 2)), HMDRV_SIZEADDRINTBYTES(2));
                    #endif
                    pResponseEnd += 2;     

                    nSizeOffsetMultiplier = 0;
                    nSizeOffsetRequestMultMode = 0;
                    u16LastFileNumberRequest = 0;
                    bFileFirstRequest = 0;
                    
                    while(nSizeReadBytes >= 9)
                    {
                        pMessageInCopy = pMessageIn;
                        
                        nSizeReadBytes -= 7;
                        /* Reference Number - 0x06 */
                        pMessageIn = HMDRV_ValueFromBuffer8(&nReferenceType, pMessageIn);
                        if (nReferenceType != 0x06) 
                        {
                            /* Error Reference Number */
                            pResponseEnd = pResponseStart + 1;
                            pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                            pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_ADDRESS);
                            bException = HMDRV_TRUE;
                            //pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nReferenceType);
                            //pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nSizeReadBytes); 
                            nSizeReadBytes = 0;
                            break;
                        }
                        else
                        {
                            /* File Number */
                            pMessageIn = HMDRV_ValueFromBuffer16BigEndian(&nFileNumber, pMessageIn);
                            
                            nFileNumber = HMMODBUS_GetRealFileNumber(nFileNumber);

                            if ((nFileNumber > HMMODBUS_u32FileCount) || (nFileNumber == 0) || ((HMMODBUS_pasFileAddressTable+(nFileNumber-1))->bWriteAccess == HMDRV_FALSE))    /* File Number -> 0x0001 .. 0xFFFF */
                            {
                                /* Error File Number */
                                pResponseEnd = pResponseStart + 1;
                                pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                                pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_ADDRESS);
                                bException = HMDRV_TRUE;
                                nSizeReadBytes = 0;
                                break;

                            }
                            else
                            {
                                nFileNumber--;  /* zero-based file number offset */

                                /* Record Number */
                                pMessageIn = HMDRV_ValueFromBuffer16BigEndian(&nRecordNumber, pMessageIn);



                                /* Get Size RW Request */
                                pMessageIn = HMDRV_ValueFromBuffer16BigEndian(&nSizeRead, pMessageIn);
                                nSizeRead <<= 1;
                                bStringDataExpected = (HMMODBUS_pasFileAddressTable+nFileNumber)->bStringDataExpected;
                                nSizeReadRequest = nSizeRead;
                                nSizeReadRequestLoop = nSizeReadRequest;



                                /* Record Index Generation If Needed - to do: test the Implementation !!! */

                                if ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToRecordIndexGenerationFunction != NULL)
                                {
                                    pfIndexGeneration = (CSMON_pfIndexGeneration)(HMDRV_PTR_FUNC_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToRecordIndexGenerationFunction);

                                }


                                nRecordNumberRequest = nRecordNumber;
                                nLoopIndexedRequest = 0;
                                while (nSizeReadRequestLoop > 0)
                                {
                                    nLoopIndexedRequest++;

                                    /* string data with index not implemented */
                                     if (bStringDataExpected)
                                     {
                                         nSizeReadRequestLoop = 0;
                                     }


                                    nAddrRead = (HMMODBUS_pasFileAddressTable+nFileNumber)->u32RealAddress;
                                    bParameterTroughFile = HMDRV_FALSE;
                                    if (nAddrRead == NULL)
                                    {
                                        if ( ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordRealAddressTable) != NULL)
                                        {
                                            nAddrRead = (((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordRealAddressTable) + nRecordNumber);
                                        }
                                        if (nAddrRead == NULL)
                                        {
                                            /* Parameter Access trough File */
                                            bParameterTroughFile = HMDRV_TRUE;
                                        }
                                    }



                                    if (pfIndexGeneration != NULL)
                                    {
                                        nRecordNumber = pfIndexGeneration(nRecordNumberRequest++);

                                        /* more than one record to be processed */
                                        if (nSizeReadBytes > 0)
                                        {
                                            bException = HMDRV_FALSE;
                                        }

                                    }
                                    else
                                    {
                                        nSizeReadRequestLoop = 0;
                                    }


                                    if ((HMMODBUS_pasFileAddressTable+nFileNumber)->u16RecordCount == 0)    /* Mode Multiplier in the first Request */
                                    {
                                        if ((bFileFirstRequest == 0) || (u16LastFileNumberRequest != nFileNumber))
                                        {
                                            nSizeOffsetMultiplier = nRecordNumber + 1;
                                            bFileFirstRequest = 1;
                                        }
                                        else
                                        {
                                            nSizeOffsetRequestMultMode = nRecordNumber;
                                            bFileFirstRequest = 0;
                                        }
                                        u16LastFileNumberRequest = nFileNumber;
                                        nRecordNumber = 0;
                                    }

                                    if ( (bParameterTroughFile == HMDRV_FALSE) && ( (nRecordNumber >= (HMMODBUS_pasFileAddressTable+nFileNumber)->u16RecordCount) && ((HMMODBUS_pasFileAddressTable+nFileNumber)->u16RecordCount != 0) ))
                                    {
                                        /* Error Record Number */
                                        pResponseEnd = pResponseStart + 1;
                                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                                        pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_ADDRESS);
                                        bException = HMDRV_TRUE;
                                        nSizeReadBytes = 0;
                                        break;
                                    }
                                    else
                                    {

                                        /* Check Read Only */
                                        if (bFileFirstRequest == 1)        /* Multiplier Mode First Request */
                                        {
                                            bReadOnlyFile = 0;  /* can always write additional offset */
                                        }
                                        else
                                        {
                                            bReadOnlyFile = (HMMODBUS_pasFileAddressTable+nFileNumber)->bReadOnlyFile;
                                            if (bReadOnlyFile == 0)
                                            {
                                                if ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordReadOnlyTable != NULL)
                                                {
                                                    if(*((HMDRV_BOOL*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordReadOnlyTable) + nRecordNumber) == 1)
                                                    {
                                                        bReadOnlyFile = 1;
                                                    }
                                                }
                                            }
                                        }

                                        /* Get Function After Write Command */
                                        nAddrFuncExecute = (HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToProcessFunctionAfterWriteCommand;
                                        if((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordProcessFunctionAfterWriteCommandTable != NULL)
                                        {
                                            nAddrFuncExecute = *((HMDRV_U32*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordProcessFunctionAfterWriteCommandTable) + nRecordNumber);
                                        }
                                        
                                        /* Get nSizeOffsetCurrentRecord */
                                        if (bFileFirstRequest == 1)        /* Multiplier Mode First Request */
                                        {
                                            nSizeRead = 2;   /* force Write Request for one register (u16RecordToRecordOffsetSizeBytes) */
                                        }
                                        else if ((HMMODBUS_pasFileAddressTable+nFileNumber)->u16RecordCount == 0) /* Multiplier Mode Second Request */
                                        {
                                            nSizeOffset = (HMMODBUS_pasFileAddressTable+nFileNumber)->u16RecordToRecordOffsetSizeBytes;
                                            nSizeAllRecords = 0xFFFFFFFF;       /* No limitation Now - consider put limitation for file size */
                                        }
                                        else if (bParameterTroughFile == HMDRV_TRUE)
                                        {
                                            nAddrFuncExecute = HMMODBUS_GetRegisterFunction((HMDRV_U16)nRecordNumber, HMDRV_FALSE, HMDRV_FALSE);
                                            nAddrRead = HMMODBUS_GetRealRegisterAddressAndSize((HMDRV_U16)nRecordNumber, (HMDRV_U16*)&nSizeBytes, (HMDRV_U16*)&u16ElementSize, HMMODBUS_REG_COIL_WRITE_HOLDING);//HMDRV_FALSE, bInputRegsGlobal

                                            if ((u16ElementSize & 0x8000) > 0) /* bit field */
                                            {
                                                //to do : copy mask prepare here or handle copy bitfield below
                                                nSizeBytes = (HMDRV_SIZE8)u16ElementSize & 0x00FF;
                                                u16ElementSize = (HMDRV_SIZE8)u16ElementSize & 0x00FF;
                                            }
                                            else
                                            {
                                                nSizeBytes = (HMDRV_SIZE8)nSizeBytes & 0x00FF;
                                                u16ElementSize = (HMDRV_SIZE8)u16ElementSize & 0x00FF;
                                            }

                                            nSizeCurrentRecord = nSizeBytes;
                                            if ((nSizeRead) > nSizeBytes)
                                            {
                                                nSizeRead = nSizeBytes;
                                            }

                                            if (u16ElementSize == HMDRV_ELEMENT_SIZE_STRING)
                                            {
                                                bStringDataExpected = 1;
                                            }

                                        }
                                        else if ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordOffsetTable == NULL)
                                        {
                                            nSizeBytes = (HMMODBUS_pasFileAddressTable+nFileNumber)->u16RecordSizeBytes;
                                            nSizeCurrentRecord = nSizeBytes;
                                            nSizeOffset = (HMMODBUS_pasFileAddressTable+nFileNumber)->u16RecordToRecordOffsetSizeBytes;

                                            /* u16RecordToRecordOffsetSizeBytes comes from requested RegisterCount if not specified */
                                            if (nSizeOffset == 0)
                                            {
                                                /* ??? to do- extract u16RecordToRecordOffsetSizeBytes from write request bytes count */
                                                nSizeOffset = nSizeReadRequest;
                                                nSizeBytes = nSizeReadRequest;
                                                nSizeCurrentRecord = nSizeReadRequest;
                                            }

                                            nSizeAllRecords = nSizeOffset * ((HMMODBUS_pasFileAddressTable+nFileNumber)->u16RecordCount - 1) + nSizeBytes; /* Check inside file (allow bigger read write than record size) */
                                            nSizeOffsetCurrentRecord = nSizeOffset * nRecordNumber;
                                            //#if HMMODBUS_TRUNK_SIZE_IN_BIGGER_RECORD_LEGTH_REQUEST_IN_FIXED_RECORD_SIZE_FILE
                                            if ((HMMODBUS_pasFileAddressTable+nFileNumber)->bTrunkBiggerLengthRequest)
                                            {
                                                if ((nSizeRead) > nSizeBytes)
                                                {
                                                    nSizeRead = nSizeBytes;
                                                }
                                            }
                                            //#endif
                                        }
                                        else
                                        {
                                            //nSizeBytes = (HMMODBUS_pasFileAddressTable+nFileNumber)->u16RecordSizeBytes; not needed
                                            if (nRecordNumber == 0)
                                            {
                                                nSizeOffsetCurrentRecord = 0;
                                            }
                                            else
                                            {
                                                nSizeOffsetCurrentRecord = *((HMDRV_U16*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordOffsetTable) + (nRecordNumber-1));
                                            }
                                            #if HMMODBUS_ALLOW_FILE_RW_DATA_COUNT_BIGGER_THAN_RECORD_SIZE_IN_VARIABLE_RECORD_SIZE_FILE
                                            nSizeAllRecords = *((HMDRV_U16*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordOffsetTable) + ((HMMODBUS_pasFileAddressTable+nFileNumber)->u16RecordCount-1));  /* Check inside file (allow bigger read write than record size) */
                                            #else
                                            nSizeAllRecords = *((HMDRV_U16*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordOffsetTable) + (nRecordNumber));       /* check inside record */
                                            #endif



                                            nSizeCurrentRecord = *((HMDRV_U16*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordOffsetTable) +(nRecordNumber)) - nSizeOffsetCurrentRecord;

                                            #if HMMODBUS_TRUNK_SIZE_IN_BIGGER_RECORD_LEGTH_REQUEST_IN_VARIABLE_RECORD_SIZE_FILE
                                            if (nSizeRead > nSizeCurrentRecord)
                                            {
                                                nSizeRead = nSizeCurrentRecord;
                                            }
                                            #endif

                                            if (bStringDataExpected == 0)
                                            {
                                                if ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordStringDataTable != NULL)
                                                {
                                                    if(*((HMDRV_BOOL*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordStringDataTable) + nRecordNumber) == 1)
                                                    {
                                                        bStringDataExpected = 1;
                                                    }
                                                }
                                            }
                                        }

                                        /* Get nAddr */
                                        if (bParameterTroughFile == HMDRV_TRUE)
                                        {
                                            //nAddrRead calculated above
                                        }
                                        else if (bFileFirstRequest == 1)        /* Multiplier Mode First Request */
                                        {
                                            nAddrRead =(HMDRV_U32)(HMDRV_ADDR_TO_PNTR_SIZE) (&((HMMODBUS_pasFileAddressTable+nFileNumber)->u16RecordToRecordOffsetSizeBytes));    /* RW Request of additional Multiplier */
                                        }
                                        else if ((HMMODBUS_pasFileAddressTable+nFileNumber)->u16RecordCount == 0) /* Multiplier Mode Second Request */
                                        {
                                            /* nSizeOffset contains u16RecordToRecordOffsetSizeBytes (... or the additional multiplier) add 1 and the result is in count registers  */
                                            /* nSizeOffsetMultiplier - main offset multiplier */
                                            /* nSizeOffsetRequestMultMode - offset that must be added and multiplied with additional multiplier */
                                            #if HMDRV_ADDR_WIDTH == 2
                                            nAddrRead += (nSizeOffsetMultiplier*10000 + nSizeOffsetRequestMultMode)*(nSizeOffset+1);
                                            #else
                                            nAddrRead += (nSizeOffsetMultiplier*10000 + nSizeOffsetRequestMultMode)*((nSizeOffset+1)<<1);
                                            #endif
                                        }
                                        else                    /* Normal Mode */
                                        {
                                            #if HMDRV_ADDR_WIDTH == 2
                                            nAddrRead += ((nSizeOffsetCurrentRecord+1) >> 1);
                                            #else
                                            nAddrRead += nSizeOffsetCurrentRecord;
                                            #endif
                                        }

                                        if (bParameterTroughFile == HMDRV_FALSE)
                                        {
                                            if (bStringDataExpected)
                                            {
                                                //terminations and trunk
                                            }
                                        }

                                        nAddrWrite = (HMDRV_ADDR)(HMDRV_ADDR_SIZE_LONG)nAddrRead;   /* consider PIC works with this */

                                        /* fix Index generation write multiple records*/
                                        if (nSizeReadRequestLoop > 0)
                                        {
                                            if (nSizeReadCheck >= nSizeCurrentRecord)
                                            {
                                                nSizeReadCheck = nSizeRead = nSizeCurrentRecord;
                                            }
                                            nSizeReadRequestLoop -= nSizeCurrentRecord;
                                        }

                                        nSizeReadBytes -= nSizeReadRequest;


                                        if (bParameterTroughFile == HMDRV_FALSE)
                                        {
                                            if ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32FileSizeBytes != NULL)
                                            {
                                                nSizeAllRecords = (HMMODBUS_pasFileAddressTable+nFileNumber)->u32FileSizeBytes;
                                            }

                                            /* Trunk request even in variable record size */
                                            if (((HMMODBUS_pasFileAddressTable+nFileNumber)->bTrunkBiggerLengthRequest) && (bFileFirstRequest == 0))
                                            {
                                                /* if calculated start address in range */
                                                if ((nAddrRead * HMDRV_ADDR_WIDTH) < ( ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32RealAddress * HMDRV_ADDR_WIDTH) + nSizeAllRecords))
                                                {
                                                    /* fix read request size to be inside limits */
                                                    if ((((nAddrRead * HMDRV_ADDR_WIDTH) + nSizeRead) > ( ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32RealAddress * HMDRV_ADDR_WIDTH) + nSizeAllRecords)) && (bFileFirstRequest == 0))
                                                    {
                                                        nSizeReadCheck = nSizeRead = ( ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32RealAddress * HMDRV_ADDR_WIDTH) + nSizeAllRecords) - (nAddrRead * HMDRV_ADDR_WIDTH);
                                                        nSizeReadRequest = nSizeRead;
                                                        nSizeCurrentRecord = nSizeRead;
                                                    }
                                                }
                                            }
                                        }

                                        nSizeWriteBytes = nSizeRead;
                                        nResponseBytes += nSizeWriteBytes;

                                        if
                                        (
                                                (bFileFirstRequest == 0) &&
                                                (
                                                     ( (bParameterTroughFile == HMDRV_TRUE) && (nAddrRead == NULL) ) ||
                                                     ( (bParameterTroughFile == HMDRV_FALSE) && ( ((nAddrRead * HMDRV_ADDR_WIDTH) + nSizeRead) > ( ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32RealAddress * HMDRV_ADDR_WIDTH) + nSizeAllRecords) ) )
                                                )
                                        )
                                        {
                                            /* Error Record address */
                                            pResponseEnd = pResponseStart + 1;
                                            pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                                            pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_ADDRESS);
                                            bException = HMDRV_TRUE;
                                            nSizeReadBytes = 0;
                                            break;
                                        }
                                        else
                                        {
                                            /* Write To Memory */
                                            //if (nSizeWriteBytes > (HMMODBUS_pasFileAddressTable+nFileNumber)->u16RecordSizeBytes)
                                            {
                                            //    nSizeWriteBytes = (HMMODBUS_pasFileAddressTable+nFileNumber)->u16RecordSizeBytes;
                                            }


                                            //if (pResponseEnd != pMessageInput) //just commented 2019 11 13 dont know for what is this - maybe was debug
                                            //{
                                            //    pResponseEnd = pMessageInput;   /* should not enter here except when reading not full string??? */
                                            //}



                                            //if(nLoopIndexedRequest <= 1)
                                            {

                                            /* Advance with 0x06, FileNum, RecNum, RecLen - rework if different write size answer */
                                            #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                                            HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pResponseEnd), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(pMessageIn - 7)), HMDRV_SIZEADDRINTBYTES(7));
                                            //HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pResponseEnd), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pMessageInCopy), HMDRV_SIZEADDRINTBYTES(7));  //same as above
                                            #endif
                                            pResponseEnd += 7;
                                            nResponseBytes += 7;

                                            }
                                            //else
                                            //{
                                            //    nResponseBytes += 7;
                                            //}


                                            /* null terminate string */
                                            bStringNullTerminationPossible = HMDRV_FALSE;
                                            if (bStringDataExpected)
                                            {
                                                if (nSizeRead  < nSizeCurrentRecord)
                                                {
                                                    bStringNullTerminationPossible = HMDRV_TRUE;
                                                }
                                            }

                                            /* extract pu8ElementSizeTable and u8ElementSize */
                                            pu8ElementSizeTable = NULL;
                                            if (bStringDataExpected)
                                            {
                                                if (bParameterTroughFile == HMDRV_FALSE)
                                                {
                                                    u8ElementSize = (HMMODBUS_pasFileAddressTable+nFileNumber)->u8RecordElementsCountSize;/* default u8ElementSize taken from u8RecordElementsCountSize */
                                                }
                                                else
                                                {
                                                    //u16ElementSize taken above
                                                    u8ElementSize = (HMDRV_U8)u16ElementSize;
                                                }
                                                if (u8ElementSize != HMDRV_ELEMENT_SIZE_STRING)
                                                {
                                                    u8ElementSize |= HMDRV_ELEMENT_MASK_STRING;
                                                }
                                            }
                                            else if (bParameterTroughFile == HMDRV_FALSE)
                                            {
                                                pu8ElementSizeTable = (HMDRV_U8*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsSizeTable);
                                                u8ElementSize = (HMMODBUS_pasFileAddressTable+nFileNumber)->u8RecordElementsCountSize;/* default u8ElementSize taken from u8RecordElementsCountSize */

                                                if (pu8ElementSizeTable != NULL)
                                                {
                                                    if (u8ElementSize == 0)     /* Zero Element Count - Error should not go here */
                                                    {
                                                        pu8ElementSizeTable = NULL;
                                                    }
                                                }
                                                else
                                                {
                                                    if (u8ElementSize == 0) /* if u8RecordElementsCountSize == NULL and u32PointerToFileRecordElementsSizeTable== NULL get size from current record size */
                                                    {
                                                        u8ElementSize = nSizeCurrentRecord;
                                                    }
                                                    else    /* u8ElementSize taken from u8RecordElementsCountSize */
                                                    {
                                                        //u8ElementSize = (HMMODBUS_pasFileAddressTable+nFileNumber)->u8RecordElementsCountSize; - made above
                                                    }
                                                }
                                            }

                                            
                                            //bReadOnlyFile - implement functionality;
                                            if(bReadOnlyFile == HMDRV_FALSE)
                                            {
                                                if (((HMMODBUS_pasFileAddressTable+nFileNumber)->u8RecordElementsFromTableCount == 0) ||
                                                    ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsRealAddressTable == NULL) ||
                                                    ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsFromDiffLocationsSizeTable == NULL) ||
                                                    ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable == NULL))
                                                {
                                                    HMMODBUS_nFilePrimaryAddrWriteLast[0] = (HMDRV_U32)(HMDRV_ADDR_SIZE_LONG)nAddrWrite;

                                                    if (bParameterTroughFile == HMDRV_TRUE)
                                                    {
                                                        if (HMMODBUS_CopyFromBuffer((HMDRV_U16)(HMDRV_ADDR_SIZE)nAddrWrite, pMessageIn, (HMDRV_SIZE8)nSizeWriteBytes) == NULL)
                                                        {
                                                            pMessageIn = NULL;/* fail write data */
                                                        }
                                                        else
                                                        {
                                                            pMessageIn += nSizeReadRequest;  /* advance read buffer pointer with the whole write request size */
                                                        }
                                                    }
                                                    else
                                                    {
                                                        if (HMDRV_CopyFromBufferModbus(nAddrWrite, pMessageIn, (HMDRV_SIZE8)nSizeWriteBytes, u8ElementSize, pu8ElementSizeTable, bStringNullTerminationPossible) == NULL)
                                                        {
                                                            pMessageIn = NULL;/* fail write data */
                                                        }
                                                        else
                                                        {
                                                            pMessageIn += nSizeReadRequest;  /* advance read buffer pointer with the whole write request size */
                                                        }
                                                    }
                                                }
                                                else
                                                {
                                                    nLoopCount = (HMMODBUS_pasFileAddressTable+nFileNumber)->u8RecordElementsFromTableCount;
                                                    
                                                    
                                                    pu8ElementSizeTablePtr = (HMDRV_CNST_PNTR_MODIFIER HMDRV_U8*)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsCountInPartTable);

                                                    for (nLoopIndex = 0; nLoopIndex < nLoopCount; nLoopIndex++)
                                                    {
                                                        if (DEBUG_MAX_FILE_RECORD_COUNT > nLoopIndex)
                                                        {
                                                            HMMODBUS_nFilePrimaryAddrWriteLast[nLoopIndex] = (HMDRV_U32)(HMDRV_ADDR_SIZE_LONG)nAddrWrite;
                                                        }
                                                                                    /* PIC works with (HMDRV_ADDR_SIZE_LONG) */
                                                        nAddrWrite = (HMDRV_ADDR)(HMDRV_ADDR_SIZE_LONG)((HMDRV_U32*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsRealAddressTable))[nLoopIndex];
                                                        //was nAddrWrite = (HMDRV_ADDR)(HMDRV_ADDR_SIZE_LONG)((HMDRV_U32*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsRealAddressTable))[nLoopIndex];
                                                        nSizeWriteBytesAllParts = nSizeReadRequest;
                                                        pu8ElementSizeTable = (HMDRV_U8*)(HMDRV_ADDR_TO_PNTR_SIZE)(((HMDRV_U32*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsFromDiffLocationsSizeTable))[nLoopIndex]);
                                                        //was pu8ElementSizeTable = (HMDRV_U8*)(HMDRV_ADDR_TO_PNTR_SIZE)(((HMDRV_U32*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsFromDiffLocationsSizeTable))[nLoopIndex]);
                                                        //u8ElementSize = ((HMDRV_U8*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsCountInPartTable))[nLoopIndex];
                                                        //u8ElementSize = ((HMDRV_CNST_PNTR_MODIFIER HMDRV_U8*)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsCountInPartTable))[nLoopIndex];
                                                        u8ElementSize = *pu8ElementSizeTablePtr++;
                                                        nSizeWriteBytes = 0;
                                                        for (nLoopIndex2 = 0; nLoopIndex2 < u8ElementSize; nLoopIndex2++)
                                                        {
                                                            nSizeWriteBytes += pu8ElementSizeTable[nLoopIndex2];
                                                        }
                                                        nSizeOffset = ((HMDRV_U16*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable))[nLoopIndex];
                                                        nSizeOffsetCurrentRecord = nSizeOffset * nRecordNumber;
                                                        nAddrWrite += nSizeOffsetCurrentRecord;
                                                        nSizeWriteBytesAllParts += nSizeWriteBytes;
                                                        if (nSizeWriteBytesAllParts >= nSizeWriteBytes)
                                                        {
                                                             nSizeWriteBytesAllParts -= nSizeWriteBytes;
                                                        }
                                                        else
                                                        {
                                                            nSizeWriteBytes = nSizeWriteBytesAllParts;  /* shrink count to copy - not tested and unknown if will be used at all */
                                                            nSizeWriteBytesAllParts = 0;
                                                        }

                                                        if (nSizeWriteBytes == 0)
                                                        {
                                                            pMessageIn = NULL;/* consider zero bytes to copy??? */
                                                        }
                                                        else
                                                        if (HMDRV_CopyFromBufferModbus(nAddrWrite, pMessageIn, (HMDRV_SIZE8)nSizeWriteBytes, u8ElementSize, pu8ElementSizeTable, bStringNullTerminationPossible) == NULL)
                                                        {
                                                            /* fail write data */
                                                            pMessageIn = NULL;
                                                        }
                                                        else
                                                        {
                                                            pMessageIn += nSizeReadRequest;  /* advance read buffer pointer with the whole write request size */
                                                        }
                                                    } 
                                                }
                                                
                                                
                                                
                                                





                                            }

                                            if ((pMessageIn == NULL) || (eException != HMMODBUS_ERR_NO_EXCEPTION))
                                            {
                                                bException = HMDRV_TRUE;
                                                if (eException == HMMODBUS_ERR_NO_EXCEPTION)
                                                {
                                                    eException = HMMODBUS_ERR_SERVER_DEVICE_FAILURE;
                                                }
                                                pResponseEnd = pResponseStart + 1;
                                                pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                                                pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, eException);
                                                bException = HMDRV_TRUE;
                                                nSizeReadBytes = 0;
                                                break;
                                            }
                                            else
                                            {
                                                //nAddrFuncExecute            - implement functionality;
                                                if (nAddrFuncExecute != NULL)
                                                {
                                                    ((pfVoid_t)(HMDRV_PTR_FUNC_SIZE)nAddrFuncExecute)();
                                                }

                                                HMMOSBUS_nDummy = 0x1234567;


                                                // to do check ??? fix if need to return real data back never mind the read only
                                                //bReadOnlyFile - implement functionality;
                                                if(bReadOnlyFile == HMDRV_FALSE)
                                                {
                                                    nSizeWriteBytes = (nSizeWriteBytes & 1) + nSizeWriteBytes;
                                                    //#if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                                                    //HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pResponseEnd), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(HMMODBUS_pCommBufferRx+(pResponseEnd - HMMODBUS_pCommBufferTx))), HMDRV_SIZEADDRINTBYTES(nSizeWriteBytes));
                                                    //#endif
                                                    //pResponseEnd += nSizeWriteBytes;
                                                    #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                                                    HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pResponseEnd), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(HMMODBUS_pCommBufferRx+(pResponseEnd - HMMODBUS_pCommBufferTx))), HMDRV_SIZEADDRINTBYTES(nSizeReadRequest));
                                                    #endif
                                                    pResponseEnd += nSizeReadRequest;   /* return same as requested */
                                                    HMMOSBUS_nDummy = 0x12345;
                                                }
                                                else
                                                {
                                                    //return real data here instead of write data above - not checked string data support for now
                                                    //pResponseEnd = HMDRV_CopyToBufferModbus(pResponseEnd, nAddrRead, (HMDRV_SIZE8) nSizeReadRequest, u8ElementSize, pu8ElementSizeTable); was only this



                                                    if (((HMMODBUS_pasFileAddressTable+nFileNumber)->u8RecordElementsFromTableCount == 0) ||
                                                        ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsRealAddressTable == NULL) ||
                                                        ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsFromDiffLocationsSizeTable == NULL) ||
                                                        ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable == NULL))
                                                    {
                                                        HMMOSBUS_nFileWriteAddrReadLast[0] = nAddrRead;
                                                        HMMOSBUS_nDummy = 0x12345678;
                                                        nSizeReadCheck = nSizeReadRequest;
                                                        pResponseEnd = HMDRV_CopyToBufferModbus(pResponseEnd, (HMDRV_ADDR)(HMDRV_ADDR_SIZE)nAddrRead, (HMDRV_SIZE8) nSizeReadCheck, u8ElementSize, pu8ElementSizeTable);
                                                        //#if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                                                        //HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pResponseEnd), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(HMMODBUS_pCommBufferRx+(pResponseEnd - HMMODBUS_pCommBufferTx))), HMDRV_SIZEADDRINTBYTES(nSizeReadRequest));
                                                        //#endif
                                                        //pResponseEnd += nSizeReadRequest;
                                                    }
                                                    else
                                                    {
                                                        HMMOSBUS_nDummy = 0x100000;

                                                        nLoopCount = (HMMODBUS_pasFileAddressTable+nFileNumber)->u8RecordElementsFromTableCount;

                                                        pu8ElementSizeTablePtr = (HMDRV_CNST_PNTR_MODIFIER HMDRV_U8*)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsCountInPartTable);

                                                        for (nLoopIndex = 0; nLoopIndex < nLoopCount; nLoopIndex++)
                                                        {
                                                            nAddrRead = ((HMDRV_U32*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsRealAddressTable))[nLoopIndex];

                                                            HMMOSBUS_nFileWriteAddrReadLast[nLoopIndex] = nAddrRead;
                                                            HMMOSBUS_nDummy++;

                                                            pu8ElementSizeTable = (HMDRV_U8*)(HMDRV_ADDR_TO_PNTR_SIZE)(((HMDRV_U32*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsFromDiffLocationsSizeTable))[nLoopIndex]);
                                                            //u8ElementSize = ((HMDRV_U8*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsCountInPartTable))[nLoopIndex];
                                                            //u8ElementSize = ((HMDRV_CNST_PNTR_MODIFIER HMDRV_U8*)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsCountInPartTable))[nLoopIndex];
                                                            u8ElementSize = *pu8ElementSizeTablePtr++;
                                                            nSizeReadCheck = 0;
                                                            for (nLoopIndex2 = 0; nLoopIndex2 < u8ElementSize; nLoopIndex2++)
                                                            {
                                                                nSizeReadCheck += pu8ElementSizeTable[nLoopIndex2];
                                                            }
                                                            nSizeOffset = ((HMDRV_U16*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable))[nLoopIndex];
                                                            nSizeOffsetCurrentRecord = nSizeOffset * nRecordNumber;
                                                            nAddrRead += nSizeOffsetCurrentRecord;

                                                            if (nSizeReadCheck > nSizeRead)
                                                            {
                                                                nSizeReadRequest = nSizeRead;
                                                            }
                                                            else
                                                            {
                                                                nSizeReadRequest = nSizeReadCheck;
                                                            }

                                                            pResponseEnd = HMDRV_CopyToBufferModbus(pResponseEnd, (HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8) nSizeReadCheck, u8ElementSize, pu8ElementSizeTable);

                                                            nSizeReadCheck -= nSizeReadRequest;
                                                            if (nSizeReadCheck == 0)
                                                            {
                                                                break;
                                                            }
                                                        }
                                                    }





                                                }
                                            }
                                        }
                                    }
                                }//nSizeReadRequestLoop
                            }
                        }

                        if (bException == HMDRV_TRUE)
                        {
                            break;
                        }
                    }
                }

                if (bException == HMDRV_FALSE)
                {
                    (void)HMDRV_ValueToBuffer8(pResponseStart + 2, nResponseBytes);   
                }

                break;

                
                
                
                
                
                
                
                
            case HMMODBUS_CMD_RD_FILE_RECORD:

                /* message bytes count */
                pMessageIn = HMDRV_ValueFromBuffer8(&nSizeReadBytes, pMessageIn);

                #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(pResponseEnd)), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(HMMODBUS_pCommBufferRx+(pResponseEnd - HMMODBUS_pCommBufferTx))), HMDRV_SIZEADDRINTBYTES(1));
                #endif
                /* Response leaves the command as is for now */
                pResponseEnd = HMDRV_SkipInBuffer(pResponseEnd, 1U);

                /* Response set 0 for Resp.Data Length for now */
                //pResponseEnd = HMDRV_SkipInBuffer(pResponseEnd, 1U);

                nBytesResponseTotal = 1+1+1;            
                nResponseBytes = 0;

                /* check message bytes count good */
                if((nSizeReadBytes < 7) || (nSizeReadBytes > 0xF5) || ((nSizeReadBytes % 7) != 0))    
                {
                    /* Error message bytes count */
                    pResponseEnd = pResponseStart + 1;
                    pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                    pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_LENGTH);
                    bException = HMDRV_TRUE;
                    //pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nSizeReadBytes); was for testing
                }
                else
                {

                    nSizeOffsetMultiplier = 0;
                    nSizeOffsetRequestMultMode = 0;
                    u16LastFileNumberRequest = 0;
                    bFileFirstRequest = 0;

                    /* Resp. Data Length */
                    pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, 0);   /* fixed at end */


                    while(nSizeReadBytes >= 7)
                    {
                        nSizeReadBytes -= 7;
                        /* Reference Number - 0x06 */
                        pMessageIn = HMDRV_ValueFromBuffer8(&nReferenceType, pMessageIn);
                        if (nReferenceType != 0x06) 
                        {
                            /* Error Reference Number */
                            pResponseEnd = pResponseStart + 1;
                            pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                            pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_ADDRESS);
                            bException = HMDRV_TRUE;
                            //pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nReferenceType);
                            //pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nSizeReadBytes); 
                            break;
                        }
                        else
                        {


                            /* File Number */
                            pMessageIn = HMDRV_ValueFromBuffer16BigEndian(&nFileNumber, pMessageIn);
                            
                            nFileNumber = HMMODBUS_GetRealFileNumber(nFileNumber);
                            

                            if ((nFileNumber > HMMODBUS_u32FileCount) || (nFileNumber == 0) || ((HMMODBUS_pasFileAddressTable+(nFileNumber-1))->bReadAccess == HMDRV_FALSE))    /* File Number -> 0x0001 .. 0xFFFF */
                            {
                                /* Error File Number */
                                pResponseEnd = pResponseStart + 1;
                                pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                                pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_ADDRESS);
                                bException = HMDRV_TRUE;
                                nSizeReadBytes = 0;
                                break;

                            }
                            else
                            {
                                nFileNumber--;  /* zero-based file number offsetting */

                                /* Record Number */
                                pMessageIn = HMDRV_ValueFromBuffer16BigEndian(&nRecordNumber, pMessageIn);
                                
                                /* Get Size RW Request */
                                pMessageIn = HMDRV_ValueFromBuffer16BigEndian(&nSizeRead, pMessageIn);
                                nSizeRead <<= 1;
                                bStringDataExpected = (HMMODBUS_pasFileAddressTable+nFileNumber)->bStringDataExpected;
                                nSizeReadRequest = nSizeRead;
                                nSizeReadRequestLoop = nSizeReadRequest;

                                /* Get Function Before Read Command */
                                nAddrFuncExecute = (HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToProcessFunctionBeforeReadCommand;
                                if (nAddrFuncExecute != NULL)
                                {
                                    ((pfVoid_t)(HMDRV_PTR_FUNC_SIZE)nAddrFuncExecute)();
                                }

                                /* Record Index Generation If Needed */
                                if ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToRecordIndexGenerationFunction != NULL)
                                {
                                    pfIndexGeneration = (CSMON_pfIndexGeneration)(HMDRV_PTR_FUNC_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToRecordIndexGenerationFunction);
                                }

                                nRecordNumberRequest = nRecordNumber;
                                nLoopIndexedRequest = 0;
                                while (nSizeReadRequestLoop > 0)
                                {
                                    if (nLoopIndexedRequest > 0)
                                    {
                                        nSizeReadRequest = nSizeRead = nSizeReadRequestLoop;
                                    }
                                        nLoopIndexedRequest++;

                                    /* string data with index not implemented */
                                    if (bStringDataExpected)
                                    {
                                        nSizeReadRequestLoop = 0;
                                    }

                                    nAddrRead = (HMMODBUS_pasFileAddressTable+nFileNumber)->u32RealAddress;
                                    bParameterTroughFile = HMDRV_FALSE;
                                    if (nAddrRead == NULL)
                                    {
                                        if ( ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordRealAddressTable) != NULL)
                                        {
                                            nAddrRead = (((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordRealAddressTable) + nRecordNumber);
                                        }
                                        if (nAddrRead == NULL)
                                        {
                                            /* Parameter Access trough File */
                                            bParameterTroughFile = HMDRV_TRUE;
                                        }
                                    }

                                    if (pfIndexGeneration != NULL)
                                    {
                                        nRecordNumber = pfIndexGeneration(nRecordNumberRequest++);

                                        /* more than one record to be processed */
                                        if (nSizeReadBytes > 0)
                                        {

                                            bException = HMDRV_FALSE;
                                        }

                                    }
                                    else
                                    {
                                        nSizeReadRequestLoop = 0;
                                    }
                                    



                                    if ((HMMODBUS_pasFileAddressTable+nFileNumber)->u16RecordCount == 0)    /* Mode Multiplier in the first Request */
                                    {
                                        if ((bFileFirstRequest == 0) || (u16LastFileNumberRequest != nFileNumber))
                                        {
                                            nSizeOffsetMultiplier = nRecordNumber + 1;
                                            bFileFirstRequest = 1;
                                        }
                                        else
                                        {
                                            nSizeOffsetRequestMultMode = nRecordNumber;
                                            bFileFirstRequest = 0;
                                        }
                                        u16LastFileNumberRequest = nFileNumber;
                                        nRecordNumber = 0;
                                    }

                                    if ( (bParameterTroughFile == HMDRV_FALSE) && ((nRecordNumber >= (HMMODBUS_pasFileAddressTable+nFileNumber)->u16RecordCount) && ((HMMODBUS_pasFileAddressTable+nFileNumber)->u16RecordCount != 0)))
                                    {
                                        /* Error Record Number */
                                        pResponseEnd = pResponseStart + 1;
                                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                                        pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_ADDRESS);
                                        bException = HMDRV_TRUE;
                                        nSizeReadBytes = 0;
                                        break;
                                    }
                                    else
                                    {
                                        
                                        /* Get Function After Read Command */
                                        nAddrFuncExecute = (HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToProcessFunctionAfterReadCommand;
                                        if((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordProcessFunctionAfterReadCommandTable != NULL)
                                        {
                                            nAddrFuncExecute = *((HMDRV_U32*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordProcessFunctionAfterReadCommandTable) + nRecordNumber);
                                        }

                                        /* Get nSizeOffsetCurrentRecord */
                                        if (bFileFirstRequest == 1)        /* Multiplier Mode First Request */
                                        {
                                            nSizeRead = 2;   /* force Write Request for one register (u16RecordToRecordOffsetSizeBytes) */
                                        }
                                        else if ((HMMODBUS_pasFileAddressTable+nFileNumber)->u16RecordCount == 0) /* Multiplier Mode Second Request */
                                        {
                                            nSizeOffset = (HMMODBUS_pasFileAddressTable+nFileNumber)->u16RecordToRecordOffsetSizeBytes;
                                            nSizeAllRecords = 0xFFFFFFFF;       /* No limitation Now - consider put limitation for file size */
                                        }
                                        else if (bParameterTroughFile == HMDRV_TRUE)
                                        {
                                            nAddrFuncExecute = HMMODBUS_GetRegisterFunction((HMDRV_U16)nRecordNumber, HMDRV_TRUE, HMDRV_FALSE);
                                            nAddrRead = HMMODBUS_GetRealRegisterAddressAndSize((HMDRV_U16)nRecordNumber, (HMDRV_U16*)&nSizeBytes, (HMDRV_U16*)&u16ElementSize, HMMODBUS_REG_COIL_READ_HOLDING);//HMDRV_TRUE, bInputRegsGlobal
                                            if ((u16ElementSize & 0x8000) > 0) /* bit field */
                                            {
                                                nSizeBytes = (HMDRV_SIZE8)u16ElementSize & 0x00FF;
                                                u16ElementSize = (HMDRV_SIZE8)u16ElementSize & 0x00FF;
                                            }
                                            else
                                            {
                                                nSizeBytes = (HMDRV_SIZE8)nSizeBytes & 0x00FF;
                                                u16ElementSize = (HMDRV_SIZE8)u16ElementSize & 0x00FF;
                                            }
                                            nSizeCurrentRecord = nSizeBytes;
                                            if ((nSizeRead) > nSizeBytes)
                                            {
                                                nSizeRead = nSizeBytes;
                                            }
                                            if (u16ElementSize == HMDRV_ELEMENT_SIZE_STRING)
                                            {
                                                bStringDataExpected = 1;
                                            }


                                        }
                                        else if ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordOffsetTable == NULL)
                                        {
                                            nSizeBytes = (HMMODBUS_pasFileAddressTable+nFileNumber)->u16RecordSizeBytes;
                                            nSizeCurrentRecord = nSizeBytes;
                                            nSizeOffset = (HMMODBUS_pasFileAddressTable+nFileNumber)->u16RecordToRecordOffsetSizeBytes;

                                            /* u16RecordToRecordOffsetSizeBytes comes from requested RegisterCount if not specified */
                                            if (nSizeOffset == 0)
                                            {
                                                /* ??? to do- extract u16RecordToRecordOffsetSizeBytes from write request bytes count */
                                                nSizeOffset = nSizeReadRequest;
                                                nSizeBytes = nSizeReadRequest;
                                                nSizeCurrentRecord = nSizeReadRequest;
                                            }

                                            nSizeAllRecords = nSizeOffset * ((HMMODBUS_pasFileAddressTable+nFileNumber)->u16RecordCount - 1) + nSizeBytes; /* Check inside file (allow bigger read write than record size) */
                                            nSizeOffsetCurrentRecord = nSizeOffset * nRecordNumber;
                                            //#if HMMODBUS_TRUNK_SIZE_IN_BIGGER_RECORD_LEGTH_REQUEST_IN_FIXED_RECORD_SIZE_FILE
                                            if ((HMMODBUS_pasFileAddressTable+nFileNumber)->bTrunkBiggerLengthRequest)
                                            {
                                                if ((nSizeRead) > nSizeBytes)
                                                {
                                                    nSizeRead = nSizeBytes;
                                                }
                                            }
                                            //#endif
                                        }
                                        else
                                        {
                                            //nSizeBytes = (HMMODBUS_pasFileAddressTable+nFileNumber)->u16RecordSizeBytes; not needed
                                            if (nRecordNumber == 0)
                                            {
                                                nSizeOffsetCurrentRecord = 0;
                                            }
                                            else
                                            {
                                                nSizeOffsetCurrentRecord = *((HMDRV_U16*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordOffsetTable) + (nRecordNumber-1));
                                            }
                                            #if HMMODBUS_ALLOW_FILE_RW_DATA_COUNT_BIGGER_THAN_RECORD_SIZE_IN_VARIABLE_RECORD_SIZE_FILE
                                            nSizeAllRecords = *((HMDRV_U16*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordOffsetTable) + ((HMMODBUS_pasFileAddressTable+nFileNumber)->u16RecordCount-1));  /* Check inside file (allow bigger read write than record size) */
                                            #else
                                            nSizeAllRecords = *((HMDRV_U16*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordOffsetTable) + (nRecordNumber));       /* check inside record */
                                            #endif



                                            nSizeCurrentRecord = *((HMDRV_U16*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordOffsetTable) +(nRecordNumber)) - nSizeOffsetCurrentRecord;

                                            #if HMMODBUS_TRUNK_SIZE_IN_BIGGER_RECORD_LEGTH_REQUEST_IN_VARIABLE_RECORD_SIZE_FILE
                                            if ((nSizeRead) > nSizeCurrentRecord)
                                            {
                                                nSizeRead = nSizeCurrentRecord;
                                            }
                                            #endif

                                            if (bStringDataExpected == 0)
                                            {
                                                if ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordStringDataTable != NULL)
                                                {
                                                    if(*((HMDRV_BOOL*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordStringDataTable) + nRecordNumber) == 1)
                                                    {
                                                        bStringDataExpected = 1;
                                                    }
                                                }
                                            }
                                        }
                                        
                                        /* Get nAddr */
                                        if (bParameterTroughFile == HMDRV_TRUE)
                                        {
                                            //nAddrRead calculated above
                                        }
                                        else if (bFileFirstRequest == 1)        /* Multiplier Mode First Request */
                                        {
                                            nAddrRead =(HMDRV_U32)(HMDRV_ADDR_TO_PNTR_SIZE) (&((HMMODBUS_pasFileAddressTable+nFileNumber)->u16RecordToRecordOffsetSizeBytes));    /* RW Request of additional Multiplier */
                                        }
                                        else if ((HMMODBUS_pasFileAddressTable+nFileNumber)->u16RecordCount == 0) /* Multiplier Mode Second Request */
                                        {
                                            /* nSizeOffset contains u16RecordToRecordOffsetSizeBytes (... or the additional multiplier) add 1 and the result is in count registers  */
                                            /* nSizeOffsetMultiplier - main offset multiplier */
                                            /* nSizeOffsetRequestMultMode - offset that must be added and multiplied with additional multiplier */
                                            #if HMDRV_ADDR_WIDTH == 2
                                            nAddrRead += (nSizeOffsetMultiplier*10000 + nSizeOffsetRequestMultMode)*(nSizeOffset+1);
                                            #else
                                            nAddrRead += (nSizeOffsetMultiplier*10000 + nSizeOffsetRequestMultMode)*((nSizeOffset+1)<<1);
                                            #endif
                                        }
                                        else                    /* Normal Mode */
                                        {
                                            #if HMDRV_ADDR_WIDTH == 2
                                            nAddrRead += ((nSizeOffsetCurrentRecord+1) >> 1);
                                            #else
                                            nAddrRead += nSizeOffsetCurrentRecord;
                                            #endif
                                        }


                                        bStringNullTerminationPossible = 0;
                                        nSizeReadCheck = nSizeRead;


                                        if (bStringDataExpected)
                                        {

                                            if (nSizeRead > nSizeCurrentRecord) /* request bigger than record size */
                                            {
                                                nSizeRead = nSizeCurrentRecord;
                                            }

                                            if (bParameterTroughFile == HMDRV_FALSE)
                                            {
                                                //trunk with string size
                                                u8ElementSize = (HMMODBUS_pasFileAddressTable+nFileNumber)->u8RecordElementsCountSize;/* default u8ElementSize taken from u8RecordElementsCountSize */
                                            }
                                            else
                                            {
                                                //u16ElementSize taken above
                                                u8ElementSize = (HMDRV_U8)u16ElementSize;
                                            }

                                            if (u8ElementSize != HMDRV_ELEMENT_SIZE_STRING)
                                            {
                                                u8ElementSize |= HMDRV_ELEMENT_MASK_STRING;
                                                nSizeRead = HMDRV_GetStringLengthFromWord((HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8)nSizeRead);
                                            }
                                            else
                                            {
                                                nSizeRead = HMDRV_GetStringLength((HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8)nSizeRead);
                                            }

                                            if (nSizeRead == 0)
                                            {
                                                nSizeRead = 1;
                                            }
                                            nSizeReadCheck = nSizeRead;
                                            //add zero at the end automatically added in copy function ?)
                                            if ((nSizeRead & 1) > 0)
                                            {
                                                nSizeRead++;    /* 16-bits register size compatibility */
                                                bStringNullTerminationPossible = 1; /* request not to copy last byte from memory but force 0 */
                                            }
                                        }



                                       /* fix Index generation read multiple records*/
                                        if (nSizeReadRequestLoop > 0)
                                        {
                                            if (nSizeReadCheck >= nSizeCurrentRecord)
                                            {
                                                nSizeReadCheck = nSizeRead = nSizeCurrentRecord;
                                            }
                                            nSizeReadRequestLoop -= nSizeCurrentRecord;
                                        }


                                        if (bParameterTroughFile == HMDRV_FALSE)
                                        {
                                            if ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32FileSizeBytes != NULL)
                                            {
                                                nSizeAllRecords = (HMMODBUS_pasFileAddressTable+nFileNumber)->u32FileSizeBytes;
                                            }

                                            /* Trunk request even in variable record size */
                                            if (((HMMODBUS_pasFileAddressTable+nFileNumber)->bTrunkBiggerLengthRequest) && (bFileFirstRequest == 0))
                                            {
                                                /* if calculated start address in range */
                                                if ((nAddrRead * HMDRV_ADDR_WIDTH) < ( ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32RealAddress * HMDRV_ADDR_WIDTH) + nSizeAllRecords))
                                                {
                                                    /* fix read request size to be inside limits */
                                                    if ((((nAddrRead * HMDRV_ADDR_WIDTH) + nSizeReadCheck) > ( ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32RealAddress * HMDRV_ADDR_WIDTH) + nSizeAllRecords)) && (bFileFirstRequest == 0))
                                                    {
                                                        nSizeReadCheck = nSizeRead = ( ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32RealAddress * HMDRV_ADDR_WIDTH) + nSizeAllRecords) - (nAddrRead * HMDRV_ADDR_WIDTH);
                                                    }
                                                }
                                            }
                                        }

                                        if
                                        (
                                                (bFileFirstRequest == 0) &&
                                                (
                                                     ( (bParameterTroughFile == HMDRV_TRUE) && (nAddrRead == NULL) ) ||
                                                     ( (bParameterTroughFile == HMDRV_FALSE) && ( ((nAddrRead * HMDRV_ADDR_WIDTH) + nSizeRead) > ( ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32RealAddress * HMDRV_ADDR_WIDTH) + nSizeAllRecords) ) )
                                                )
                                        )
                                        //if ((((nAddrRead * HMDRV_ADDR_WIDTH) + nSizeReadCheck) > ( ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32RealAddress * HMDRV_ADDR_WIDTH) + nSizeAllRecords)) && (bFileFirstRequest == 0))
                                        //if ((nAddrRead + nSizeReadCheck) > ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32RealAddress + nSizeAllRecords))
                                        {
                                            /* Error Record address */
                                            pResponseEnd = pResponseStart + 1;
                                            pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                                            pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_ADDRESS);
                                            bException = HMDRV_TRUE;
                                            nSizeReadBytes = 0;
                                            break;
                                        }
                                        else
                                        {
                                            nResponseBytes += nSizeRead;
                                            nBytesResponseTotal += nSizeRead;
                                            if(nLoopIndexedRequest <= 1)
                                            {
                                                nResponseBytes += 2;
                                                nBytesResponseTotal += 2;

                                            }

                                            if (nBytesResponseTotal > 0xFF) /* Modbus  Protocol limitation with buffer 256 bytes */
                                            {
                                                /* Error Length Response Message (too big) */
                                                pResponseEnd = pResponseStart + 1;
                                                pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                                                pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_LENGTH);
                                                bException = HMDRV_TRUE;
                                                nSizeReadBytes = 0;
                                                break;
                                            }
                                            else
                                            #if HMDRV_MODBUS_BUFFER_SIZE < 256
                                            if (
                                                 (nBytesResponseTotal > 0xFF)       /* Modbus  Protocol limitation with buffer 256 bytes */
                                              || (nBytesResponseTotal > HMDRV_MODBUS_BUFFER_SIZE) /* Limitation local buffer size */
                                               )
                                            {
                                                /* Error Length Response Message (too big) */
                                                pResponseEnd = pResponseStart + 1;
                                                pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                                                pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_RESPONSE_NOT_FIT_IN_BUFFER);
                                                bException = HMDRV_TRUE;
                                                nSizeReadBytes = 0;
                                                break;

                                            }
                                            else
                                            #endif
                                            {
                                                /* Check Record Length and Fix To send only Valid Data */
                                                if ((HMMODBUS_pasFileAddressTable+nFileNumber)->uFileValidData.sFileValidBits.u1UseValidDataStructure)
                                                {
                                                    if (nRecordNumber < (HMMODBUS_pasFileAddressTable+nFileNumber)->uFileValidData.sFileValidData.u15RecordCountValid)
                                                    {
                                                        if (nRecordNumber == ((HMMODBUS_pasFileAddressTable+nFileNumber)->uFileValidData.sFileValidData.u15RecordCountValid - 1))
                                                        {
                                                            nResponseBytes -= nSizeRead;
                                                            nBytesResponseTotal -= nSizeRead;
                                                            /* Some OK */
                                                            if (nSizeReadCheck > (HMMODBUS_pasFileAddressTable+nFileNumber)->uFileValidData.sFileValidData.u8RecordBytesValid)
                                                            {
                                                                nSizeReadCheck = (HMMODBUS_pasFileAddressTable+nFileNumber)->uFileValidData.sFileValidData.u8RecordBytesValid;
                                                            }
                                                            if ((nSizeReadCheck & 1) > 0)
                                                            {
                                                                nSizeReadCheck++;    /* 16-bits register size compatibility */
                                                            }

                                                            nResponseBytes += nSizeReadCheck;
                                                            nBytesResponseTotal += nSizeReadCheck;
                                                            nSizeRead = nSizeReadCheck;



                                                        }
                                                        else
                                                        {
                                                            /* All OK */
                                                        }
                                                    }
                                                    else
                                                    {
                                                        /* No Valid Data */

                                                        /* fix bytescount */
                                                        nResponseBytes -= nSizeRead;
                                                        nBytesResponseTotal -= nSizeRead;
                                                        nSizeRead = 0;

                                                        nSizeReadCheck = 0;


                                                    }
                                                }
                                                /* Valid bits in file used below */
                                                else
                                                {
                                                    /* to do */
                                                }
                                                ///* Resp. Data Length */ moved above
                                                //pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nSizeRead+2);
                                                if(nLoopIndexedRequest <= 1)
                                                {
                                                    /* File Resp. Length */
                                                    if (nSizeRead < 2)
                                                    {
                                                        /* fix this response byte count come from valid data */
                                                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, 2+1);
                                                    }
                                                    else
                                                    {
                                                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nSizeRead+1);
                                                    }
                                                    /* Ref. Type */
                                                    pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, 0x06);
                                                }
                                                /* Data */

                                                //pResponseEnd = HMMODBUS_CopyFileRecordToBuffer(pResponseEnd, nAddrRead, (HMDRV_SIZE8) nSizeRead);

                                                //pResponseEnd = pResponseStart + 5;



                                                /* extract pu8ElementSizeTable and u8ElementSize */
                                                pu8ElementSizeTable = NULL;

                                                if (bStringDataExpected)
                                                {
                                                    if (bParameterTroughFile == HMDRV_FALSE)
                                                    {
                                                        u8ElementSize = (HMMODBUS_pasFileAddressTable+nFileNumber)->u8RecordElementsCountSize;/* default u8ElementSize taken from u8RecordElementsCountSize */
                                                    }
                                                    else
                                                    {
                                                        //u16ElementSize taken above
                                                        u8ElementSize = (HMDRV_U8)u16ElementSize;
                                                    }
                                                    if (u8ElementSize != HMDRV_ELEMENT_SIZE_STRING)
                                                    {
                                                        u8ElementSize |= HMDRV_ELEMENT_MASK_STRING;
                                                    }
                                                }
                                                else if (bParameterTroughFile == HMDRV_FALSE)
                                                {
                                                    pu8ElementSizeTable = (HMDRV_U8*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsSizeTable);
                                                    u8ElementSize = (HMMODBUS_pasFileAddressTable+nFileNumber)->u8RecordElementsCountSize;/* default u8ElementSize taken from u8RecordElementsCountSize */

                                                    if (pu8ElementSizeTable != NULL)
                                                    {
                                                        if (u8ElementSize == 0)     /* Zero Element Count - Error should not go here */
                                                        {
                                                            pu8ElementSizeTable = NULL;
                                                        }
                                                    }
                                                    else
                                                    {
                                                        if (u8ElementSize == 0) /* if u8RecordElementsCountSize == NULL and u32PointerToFileRecordElementsSizeTable== NULL get size from current record size */
                                                        {
                                                            u8ElementSize = nSizeCurrentRecord;
                                                        }
                                                        else    /* u8ElementSize taken from u8RecordElementsCountSize */
                                                        {
                                                            //u8ElementSize = (HMMODBUS_pasFileAddressTable+nFileNumber)->u8RecordElementsCountSize; - made above
                                                        }
                                                    }
                                                }

                                                if(nSizeReadCheck)
                                                {

                                                    if (((HMMODBUS_pasFileAddressTable+nFileNumber)->u8RecordElementsFromTableCount == 0) ||
                                                        ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsRealAddressTable == NULL) ||
                                                        ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsFromDiffLocationsSizeTable == NULL) ||
                                                        ((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable == NULL))
                                                    {
                                                        pResponseEnd = HMDRV_CopyToBufferModbus(pResponseEnd, (HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8) nSizeReadCheck, u8ElementSize, pu8ElementSizeTable);
                                                    }
                                                    else
                                                    {
                                                        nLoopCount = (HMMODBUS_pasFileAddressTable+nFileNumber)->u8RecordElementsFromTableCount;

                                                        pu8ElementSizeTablePtr = (HMDRV_CNST_PNTR_MODIFIER HMDRV_U8*)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsCountInPartTable);

                                                        for (nLoopIndex = 0; nLoopIndex < nLoopCount; nLoopIndex++)
                                                        {
                                                            nAddrRead = ((HMDRV_U32*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsRealAddressTable))[nLoopIndex];

                                                            pu8ElementSizeTable = (HMDRV_U8*)(HMDRV_ADDR_TO_PNTR_SIZE)(((HMDRV_U32*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsFromDiffLocationsSizeTable))[nLoopIndex]);
                                                            //u8ElementSize = ((HMDRV_U8*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsCountInPartTable))[nLoopIndex];

                                                            u8ElementSize = *pu8ElementSizeTablePtr++;
                                                            //u8ElementSize = ((HMDRV_CNST_PNTR_MODIFIER HMDRV_U8*)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsCountInPartTable))[nLoopIndex];
                                                            nSizeRead = 0;
                                                            for (nLoopIndex2 = 0; nLoopIndex2 < u8ElementSize; nLoopIndex2++)
                                                            {
                                                                nSizeRead += pu8ElementSizeTable[nLoopIndex2];
                                                            }
                                                            nSizeOffset = ((HMDRV_U16*)(HMDRV_ADDR_TO_PNTR_SIZE)((HMMODBUS_pasFileAddressTable+nFileNumber)->u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable))[nLoopIndex];
                                                            nSizeOffsetCurrentRecord = nSizeOffset * nRecordNumber;
                                                            nAddrRead += nSizeOffsetCurrentRecord;

                                                            if (nSizeReadCheck > nSizeRead)
                                                            {
                                                                nSizeReadRequest = nSizeRead;
                                                            }
                                                            else
                                                            {
                                                                nSizeReadRequest = nSizeReadCheck;
                                                            }

                                                            pResponseEnd = HMDRV_CopyToBufferModbus(pResponseEnd, (HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8) nSizeReadRequest, u8ElementSize, pu8ElementSizeTable);

                                                            nSizeReadCheck -= nSizeReadRequest;
                                                            if (nSizeReadCheck == 0)
                                                            {
                                                                break;
                                                            }
                                                        }
                                                    }
                                                }
                                                else
                                                {
                                                    #if HMMODBUS_DATA_NOT_READY_RAISE_EXCEPTION
                                                    /* Error input message data overwritten */
                                                    pResponseEnd = pResponseStart + 1;
                                                    pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                                                    pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_DATA_NOT_READY);
                                                    bException = HMDRV_TRUE;
                                                    nSizeReadBytes = 0;
                                                    break;
                                                    #elif HMMODBUS_DATA_NOT_READY_RETURN_EMPTY_REGISTER
                                                    /* fix bytescount */
                                                    nResponseBytes -= nSizeRead;
                                                    nBytesResponseTotal -= nSizeRead;
                                                    nSizeRead = 2;
                                                    nResponseBytes += nSizeRead;
                                                    nBytesResponseTotal += nSizeRead;
                                                    nSizeReadCheck = 2; /* return one empty register instead of 0 if invalid data */
                                                    pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, 0x00);
                                                    pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, 0x00);
                                                    #elif HMMODBUS_DATA_NOT_READY_RETURN_NO_DATA
                                                    /* return 0 data count here */
                                                    #else
                                                    #error "Unimplemented Data Not Ready Handling"
                                                    #endif

                                                }



                                                //if (bStringNullTerminationPossible)  not needed made above
                                                //{
                                                //    pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, NULL);
                                                //}

                                                /* detect input buffer data overwritten */
                                                #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER
                                                if ((pResponseEnd > pMessageIn) && (nSizeReadBytes > 0))
                                                {
                                                    /* Error input message data overwritten */
                                                    pResponseEnd = pResponseStart + 1;
                                                    pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                                                    pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_RECEIVE_BUFFER_OVERWRITTEN);
                                                    bException = HMDRV_TRUE;
                                                    nSizeReadBytes = 0;
                                                    break;
                                                }
                                                #endif
                                                if (pResponseEnd == NULL)
                                                {
                                                    /* Error read data */
                                                    pResponseEnd = pResponseStart + 1;
                                                    pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                                                    pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_SERVER_DEVICE_FAILURE);
                                                    bException = HMDRV_TRUE;
                                                    nSizeReadBytes = 0;
                                                    break;
                                                }

                                                if (nAddrFuncExecute != NULL)
                                                {
                                                    ((pfVoid_t)(HMDRV_PTR_FUNC_SIZE)nAddrFuncExecute)();
                                                }
                                            }
                                        }
                                    }
                                }/* nSizeReadRequestLoop */
                            }
                        }

                        if (bException == HMDRV_TRUE)
                        {
                            break;
                        }

                    }
                }

                if (bException == HMDRV_FALSE)
                {
                    (void)HMDRV_ValueToBuffer8(pResponseStart + 2, nResponseBytes);   
                    if(nLoopIndexedRequest > 1)
                    {
                        (void)HMDRV_ValueToBuffer8(pResponseStart + 3, nResponseBytes-1);
                    }

                }

                break;

            case HMMODBUS_CMD_RD_INPUT_REGS:

                #if HMMODBUS_INPUT_REGS_FROM_HOLDING_REGS == HMDRV_FALSE
                bInputRegsGlobal = HMDRV_TRUE;
                bBareRamTable = bInputRegTableBareRam;
                #endif
                /* no break */



            case HMMODBUS_CMD_RD_HOLD_REGS:


                pMessageIn = HMDRV_AddressFromBufferBigEndian((HMDRV_ADDR_PNTR_MODIFIER HMDRV_ADDR*) &nAddrRead, pMessageIn);
                pMessageIn = HMDRV_ValueFromBuffer16BigEndian(&nSizeRead, pMessageIn);



                /* to do: check nSizeWriteBytes and nSizeWrite values for error */
                if ( (HMMODBUS_MIN_MULTIPLE_REGISTERS_READ  <= nSizeRead)  && (0x007D >= nSizeRead)
                   )
                {
                    if (bBareRamTable)
                    {
                        nBytesRead = nSizeRead;
                    }
                    else
                    {
                        nBytesRead = nSizeRead << 1;
                    }
                    /* check the response will safely fit into comm buffer */
                    #if HMDRV_MODBUS_BUFFER_SIZE < 256
                    if(nBytesRead > HMDRV_MODBUS_BUFFER_SIZE - 5)    /* device address; function command; bytes count; checksum 2 Bytes; */
                    {
                        /* Error Buffer does not fit */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                        pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_RESPONSE_NOT_FIT_IN_BUFFER);
                    }
                    else
                    #endif
                    {

                        /* success  */
                        /* Response leaves the command as is */
                        #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                        HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pResponseEnd), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(HMMODBUS_pCommBufferRx+(pResponseEnd - HMMODBUS_pCommBufferTx))), HMDRV_SIZEADDRINTBYTES(1));
                        #endif
                        pResponseEnd = HMDRV_SkipInBuffer(pResponseEnd, 1U);

                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nBytesRead);

                        bLengthException = HMDRV_FALSE; /* modify below */
                        pResponseEnd = HMMODBUS_CopyToBuffer(pResponseEnd, nAddrRead, (HMDRV_SIZE8) nSizeRead);

                        if (bLengthException)
                        {
                            pResponseEnd = pResponseStart + 1;
                            pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                            pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_LENGTH);
                        }
                        else if (pResponseEnd == NULL)
                        {
                            /* Error read data */                                          
                            pResponseEnd = pResponseStart + 1;
                            pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                            pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_SERVER_DEVICE_FAILURE);
                            break;
                        }
                        #if HMMODBUS_ILLEGAL_ADDR_REG_NOT_IMPLEMENTED == 1
                        if (bUsedDummyDataAddressInResponseData == HMDRV_TRUE)
                        {
                            pResponseEnd = pResponseStart + 1;
                            pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                            pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_ADDRESS);

                        }
                        #endif

                    }     
                }
                else
                {
                    /* Error Exception Code 03 HMMODBUS_ERR_ILLEGAL_LENGTH */
                    pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                    pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_LENGTH);   
                }     
                break;
                
                
            case HMMODBUS_CMD_RD_DISCRETE_INPUTS: 

                #if HMMODBUS_DISCRETE_INPUTS_FROM_COILS == HMDRV_FALSE
                bInputRegsGlobal = HMDRV_TRUE;
                bBareRamTable = bInputCoilTableBareRam;
                #endif
                /* no break */



            case HMMODBUS_CMD_RD_COILS:

                pMessageIn = HMDRV_AddressFromBufferBigEndian((HMDRV_ADDR_PNTR_MODIFIER HMDRV_ADDR*)&nAddrRead, pMessageIn);
                pMessageIn = HMDRV_ValueFromBuffer16BigEndian(&nSizeReadBits, pMessageIn);

                
                

                /* to do: check nSizeWriteBytes and nSizeWrite values for error */
                if ( (HMMODBUS_MIN_MULTIPLE_COILS_READ  <= nSizeReadBits)  && (2000 >= nSizeReadBits)
                   )
                {
                    if (bBareRamTable)
                    {
                        nSizeRead = (nSizeReadBits + 7) >> 3;
                        nBytesRead = nSizeRead;
                    }
                    else
                    {
                        nSizeRead = (nSizeReadBits + 7) >> 3;
                        nBytesRead = nSizeRead;
                    }
                    #if HMDRV_MODBUS_BUFFER_SIZE < 256
                    /* check the response will safely fit into comm buffer */
                    if(nBytesRead > HMDRV_MODBUS_BUFFER_SIZE - 5)    /* device address; function command; bytes count; checksum 2 Bytes; */
                    {
                        /* Error Buffer does not fit */
                        pResponseEnd = pResponseStart + 1;/* might be not needed */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                        pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_RESPONSE_NOT_FIT_IN_BUFFER);
                    }
                    else
                    #endif
                    {

                        /* success  */
                        /* Response leaves the command as is */
                        #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                        HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pResponseEnd), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(HMMODBUS_pCommBufferRx+(pResponseEnd - HMMODBUS_pCommBufferTx))), HMDRV_SIZEADDRINTBYTES(1));
                        #endif
                        pResponseEnd = HMDRV_SkipInBuffer(pResponseEnd, 1U);

                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nBytesRead);

                        pResponseEnd = HMMODBUS_CopyToBufferCoil(pResponseEnd, nAddrRead, (HMDRV_SIZE8) nSizeReadBits);


                        if (pResponseEnd == NULL)
                        {
                            /* Error read data */                                          
                            pResponseEnd = pResponseStart + 1;
                            pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                            pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_SERVER_DEVICE_FAILURE);
                            break;
                        }
                        #if HMMODBUS_ILLEGAL_ADDR_REG_NOT_IMPLEMENTED == 1
                        if (bUsedDummyDataAddressInResponseData == HMDRV_TRUE)
                        {
                            pResponseEnd = pResponseStart + 1;
                            pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                            pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_ADDRESS);

                        }
                        #endif

                    }     
                }
                else
                {
                    /* Error Exception Code 03 HMMODBUS_ERR_ILLEGAL_LENGTH */
                    pResponseEnd = pResponseStart + 1; /* might be not needed */
                    pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                    pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_LENGTH);   
                }     
                break;
                

            case HMMODBUS_CMD_RD_FIFO_QUEUE:
                pMessageIn = HMDRV_AddressFromBufferBigEndian((HMDRV_ADDR_PNTR_MODIFIER HMDRV_ADDR*)&nAddrRead, pMessageIn);

                if (((HMDRV_U32)HMMODBUS_u8QueueActiveIndex < HMMODBUS_u32QueueRegisterAddressTableSize)
                     && (HMMODBUS_u32QueueRegisterAddressTableSize > 0)
                        )
                {
                    nAddrQueue = (HMMODBUS_pasQueueRegisterAddressTable + HMMODBUS_u8QueueActiveIndex)->u32RealAddress;
                    if ((nAddrQueue != NULL)
                        && (nAddrRead < (HMMODBUS_pasQueueRegisterAddressTable + HMMODBUS_u8QueueActiveIndex)->u16RecordCount)
                        && ((HMMODBUS_pasQueueRegisterAddressTable + HMMODBUS_u8QueueActiveIndex)->u16RecordCount > 0)
                        && ((HMMODBUS_pasQueueRegisterAddressTable + HMMODBUS_u8QueueActiveIndex)->u16RecordSizeBytes > 0)
                        )
                    {
                        if ((HMMODBUS_pasQueueRegisterAddressTable + HMMODBUS_u8QueueActiveIndex)->u16RecordCount <= 31)
                        {
                            if ((HMDRV_S32)nAddrRead < (HMDRV_S32)(HMMODBUS_pasQueueRegisterAddressTable + HMMODBUS_u8QueueActiveIndex)->u32RecordCountValid)
                            {
                                nAddrRead = nAddrQueue + nAddrRead*(HMMODBUS_pasQueueRegisterAddressTable + HMMODBUS_u8QueueActiveIndex)->u16RecordSizeBytes;
                                nSizeRead = (HMMODBUS_pasQueueRegisterAddressTable + HMMODBUS_u8QueueActiveIndex)->u16RecordSizeBytes;
                                nBytesRead = nSizeRead;

                                nSizeBytes =(nBytesRead + (nBytesRead & 1));   
                                nSizeRead = nSizeBytes >> 1; /* registers */
                                /* copy to buffer */

                                /* success  */
                                /* Response leaves the command as is */
                                #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                                HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pResponseEnd), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(HMMODBUS_pCommBufferRx+(pResponseEnd - HMMODBUS_pCommBufferTx))), HMDRV_SIZEADDRINTBYTES(1));
                                #endif
                                pResponseEnd = HMDRV_SkipInBuffer(pResponseEnd, 1U);

                                pResponseEnd = HMDRV_ValueToBuffer16BigEndian(pResponseEnd, (nSizeBytes+2));
                                pResponseEnd = HMDRV_ValueToBuffer16BigEndian(pResponseEnd, nSizeRead);

                                
                                
                                bStringDataExpected = 0;    //to do implement strings with queue 
                                u8ElementSize = HMDRV_ELEMENT_SIZE_SHORT;
                                pu8ElementSizeTable = NULL; 
                               
                                
                                
                                
                                
                                pResponseEnd = HMDRV_CopyToBufferModbus(pResponseEnd, (HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8) nBytesRead, u8ElementSize, pu8ElementSizeTable);

                                if (pResponseEnd == NULL)
                                {
                                    /* Error read data */                                          
                                    eException = HMMODBUS_ERR_SERVER_DEVICE_FAILURE;
                                }




                            }
                            else
                            {
                                eException = HMMODBUS_ERR_SERVER_DEVICE_FAILURE;    /* data in fifo not filled yet */
                            }




                        }
                        else
                        {
                            eException = HMMODBUS_ERR_ILLEGAL_LENGTH;
                        }
                    }
                    else
                    {
                        eException = HMMODBUS_ERR_ILLEGAL_ADDRESS;
                    }
                }
                else
                {
                    eException = HMMODBUS_ERR_ILLEGAL_ADDRESS;
                }

                if (eException != HMMODBUS_ERR_NO_EXCEPTION)
                {
                    /* Error Exception */
                    pResponseEnd = pResponseStart + 1;  /* night be not needed */
                    pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                    pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, eException);   
                }       
                break;

            case HMMODBUS_CMD_WR_HOLD_REG_MASK:
                pMessageIn = HMDRV_AddressFromBufferBigEndian((HMDRV_ADDR_PNTR_MODIFIER HMDRV_ADDR*)&nAddrWrite, pMessageIn);

                HMDRV_U32 pRealAddress;
                HMDRV_U16 u16Size;   
                HMDRV_U16 u16ElemSize;
                pRealAddress = HMMODBUS_GetRealRegisterAddressAndSize((HMDRV_U16)(HMDRV_ADDR_SIZE)nAddrWrite, (HMDRV_U16*)&u16Size, (HMDRV_U16*)&u16ElemSize, HMMODBUS_REG_COIL_WRITE_HOLDING);//HMDRV_FALSE, bInputRegsGlobal

                if ((u16ElemSize & 0x8000) > 0) /* bit field */
                {
                    u16Size = (HMDRV_SIZE8)u16ElemSize & 0x00FF;
                    u16ElemSize = (HMDRV_SIZE8)u16ElemSize & 0x00FF;
                }
                else
                {
                    u16Size = (HMDRV_SIZE8)u16Size & 0x00FF;
                    u16ElemSize = (HMDRV_SIZE8)u16ElemSize & 0x00FF;
                }


                if (u16Size > 2)
                {
                    u16Size = 2;
                }

                if (HMDRV_CopyFromBufferWithMaskModbus((HMDRV_ADDR)pRealAddress, pMessageIn, u16Size) == HMDRV_FALSE)
                {
                    eException = HMMODBUS_ERR_SERVER_DEVICE_FAILURE; 
                }

                #if HMMODBUS_ILLEGAL_ADDR_REG_NOT_IMPLEMENTED == 1
                if (bUsedDummyDataAddressInResponseData == HMDRV_TRUE)
                {
                    eException = HMMODBUS_ERR_ILLEGAL_ADDRESS;
                }
                #endif

                /* to do: HMMODBUS_CopyFromBuffer must Return or to be checked StartAddress OK and StartAddress+QuantityRegisters OK (HMMODBUS_ERR_ILLEGAL_ADDRESS ExceptionCode 2) - done??? to be checked */
                if (eException == HMMODBUS_ERR_NO_EXCEPTION)
                {
                    /* success  */
                    /* Response leaves the command as is */
                    #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                    HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pResponseEnd), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(HMMODBUS_pCommBufferRx+(pResponseEnd - HMMODBUS_pCommBufferTx))), HMDRV_SIZEADDRINTBYTES(7));
                    #endif
                    pResponseEnd = HMDRV_SkipInBuffer(pResponseEnd, 1U);
                    /* Response leaves the address as is */
                    pResponseEnd = HMDRV_SkipInBuffer(pResponseEnd, 2U);
                    /* Response leaves the and mask as is */
                    pResponseEnd = HMDRV_SkipInBuffer(pResponseEnd, 2U);
                    /* Response leaves the 0r mask as is */
                    pResponseEnd = HMDRV_SkipInBuffer(pResponseEnd, 2U);
                }  
                else /* exception */
                {
                    /* Error Exception Code 02 HMMODBUS_ERR_ILLEGAL_ADDRESS or write failed */
                    pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                    pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, eException);                  
                }

                break;

            case HMMODBUS_CMD_RW_HOLD_REGS:
                pMessageIn = HMDRV_AddressFromBufferBigEndian((HMDRV_ADDR_PNTR_MODIFIER HMDRV_ADDR*)&nAddrRead, pMessageIn);
                pMessageIn = HMDRV_ValueFromBuffer16BigEndian(&nSizeRead, pMessageIn);
                pMessageIn = HMDRV_AddressFromBufferBigEndian((HMDRV_ADDR_PNTR_MODIFIER HMDRV_ADDR*)&nAddrWrite, pMessageIn);
                pMessageIn = HMDRV_ValueFromBuffer16BigEndian(&nSizeWrite, pMessageIn);
                pMessageIn = HMDRV_ValueFromBuffer8(&nSizeWriteBytes, pMessageIn);


                /* to do: check nSizeWriteBytes and nSizeWrite values for error */
                if ( (HMMODBUS_MIN_MULTIPLE_REGISTERS_WRITE <= nSizeWrite) && (0x0079 >= nSizeWrite) 
                  && ((nSizeWriteBytes == (nSizeWrite << 1)) || (bBareRamTable == HMDRV_FALSE)) 
                  && (HMMODBUS_MIN_MULTIPLE_REGISTERS_READ  <= nSizeRead )  && (0x007D >= nSizeRead)                                             
                   )
                {


                    /* Write To Memory */
                    eException = HMMODBUS_CopyFromBuffer((HMDRV_U16)(HMDRV_ADDR_SIZE)nAddrWrite, pMessageIn, (HMDRV_SIZE8)nSizeWriteBytes);

                    #if HMMODBUS_ILLEGAL_ADDR_REG_NOT_IMPLEMENTED == 1
                    if (bUsedDummyDataAddressInResponseData == HMDRV_TRUE)
                    {
                        eException = HMMODBUS_ERR_ILLEGAL_ADDRESS;
                    }
                    #endif



                    if (eException == HMMODBUS_ERR_NO_EXCEPTION)
                    {
                        if (bBareRamTable)
                        {
                            nBytesRead = nSizeRead;
                        }
                        else
                        {
                            nBytesRead = nSizeRead << 1;
                        }
                        #if HMDRV_MODBUS_BUFFER_SIZE < 256
                        /* check the response will safely fit into comm buffer */
                        if(nBytesRead > HMDRV_MODBUS_BUFFER_SIZE - 5)    /* device address; function command; bytes count; checksum 2 Bytes; */
                        {
                            /* Error Buffer does not fit */
                            pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                            pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_RESPONSE_NOT_FIT_IN_BUFFER);
                        }
                        else
                        #endif
                        {
                            /* success  */
                            /* Response leaves the command as is */
                            #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                            HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pResponseEnd), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(HMMODBUS_pCommBufferRx+(pResponseEnd - HMMODBUS_pCommBufferTx))), HMDRV_SIZEADDRINTBYTES(1));
                            #endif
                            pResponseEnd = HMDRV_SkipInBuffer(pResponseEnd, 1U);

                            pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nBytesRead);

                            bLengthException = HMDRV_FALSE; /* modify below */
                            pResponseEnd = HMMODBUS_CopyToBuffer(pResponseEnd, nAddrRead, (HMDRV_SIZE8) nSizeRead);

                            if (bLengthException)
                            {
                                pResponseEnd = pResponseStart + 1;
                                pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                                pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_LENGTH);
                            }
                            else if (pResponseEnd == NULL)
                            {
                                /* Error read data */                                          
                                pResponseEnd = pResponseStart + 1;
                                pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                                pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_SERVER_DEVICE_FAILURE);
                                break;
                            }

                            #if HMMODBUS_ILLEGAL_ADDR_REG_NOT_IMPLEMENTED == 1
                            if (bUsedDummyDataAddressInResponseData == HMDRV_TRUE)
                            {
                                pResponseEnd = pResponseStart + 1;
                                pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                                pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_ADDRESS);

                            }
                            #endif

                        }     
                    }
                    else
                    {
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                        pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, eException);                  
                    }
                }
                else
                {
                    /* Error Exception Code 03 HMMODBUS_ERR_ILLEGAL_LENGTH */
                    pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                    pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_LENGTH);   
                }     


                break;

            case HMMODBUS_CMD_WR_HOLD_REG:
                pMessageIn = HMDRV_AddressFromBufferBigEndian((HMDRV_ADDR_PNTR_MODIFIER HMDRV_ADDR*)&nAddrWrite, pMessageIn);

                /* Write To Memory */
                eException = HMMODBUS_CopyFromBuffer((HMDRV_U16)(HMDRV_ADDR_SIZE)nAddrWrite, pMessageIn, (HMDRV_SIZE8)2);

                #if HMMODBUS_ILLEGAL_ADDR_REG_NOT_IMPLEMENTED == 1
                if (bUsedDummyDataAddressInResponseData == HMDRV_TRUE)
                {
                    eException = HMMODBUS_ERR_ILLEGAL_ADDRESS;
                }
                #endif

                /* to do: HMMODBUS_CopyFromBuffer must Return or to be checked StartAddress OK and StartAddress+QuantityRegisters OK (HMMODBUS_ERR_ILLEGAL_ADDRESS ExceptionCode 2) - done??? to be checked */
                if (eException == HMMODBUS_ERR_NO_EXCEPTION)
                {
                    /* success  */
                    

                    /* execute function */
                    nAddrFuncExecute = HMMODBUS_GetRegisterFunction((HMDRV_U16)(HMDRV_ADDR_SIZE)nAddrWrite, HMDRV_FALSE, bInputRegsGlobal);
                    if (nAddrFuncExecute != NULL)
                    {                                          
                        ((pfVoid_t)(HMDRV_PTR_FUNC_SIZE)nAddrFuncExecute)();
                    }
                    
                    
                    /* Response leaves the command as is */
                    #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                    HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pResponseEnd), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(HMMODBUS_pCommBufferRx+(pResponseEnd - HMMODBUS_pCommBufferTx))), HMDRV_SIZEADDRINTBYTES(1));
                    #endif
                    pResponseEnd = HMDRV_SkipInBuffer(pResponseEnd, 1U);

                    pResponseEnd = HMDRV_ValueToBuffer16BigEndian(pResponseEnd, (HMDRV_U16)(HMDRV_ADDR_SIZE)nAddrWrite);
                    
                    
                    
                    
                    
                    if (nAddrFuncExecute != NULL)
                    {
                        bLengthException = HMDRV_FALSE; /* modify below */
                        /* Return real Readback Data */
                        pResponseEnd = HMMODBUS_CopyToBuffer(pResponseEnd, (HMDRV_U16)(HMDRV_ADDR_SIZE)nAddrWrite, (HMDRV_SIZE8)1);//return one register
                        if (bLengthException)
                        {
                            pResponseEnd = pResponseStart + 1;
                            pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                            pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_LENGTH);
                        }
                    }
                    else
                    {
                        /* Return Requested data as is  */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, *(pMessageIn + 0));
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, *(pMessageIn + 1));
                    }
                    
                    /* If parameter connection list implemented in auxiliary data -> set connected read register in invalid read state until processed read value update */
                    
                }  
                else /* exception */
                {
                    /* Error Exception Code 02 HMMODBUS_ERR_ILLEGAL_ADDRESS or write failed */
                    pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                    pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, eException);                  
                }

                break;

            case HMMODBUS_CMD_WR_HOLD_REGS:
                pMessageIn = HMDRV_AddressFromBufferBigEndian( (HMDRV_ADDR_PNTR_MODIFIER HMDRV_ADDR*)&nAddrWrite, pMessageIn);
                pMessageIn = HMDRV_ValueFromBuffer16BigEndian(&nSizeWrite, pMessageIn);
                pMessageIn = HMDRV_ValueFromBuffer8(&nSizeWriteBytes, pMessageIn);

                /* to do: check nSizeWriteBytes and nSizeWrite values for error */
                if ( (HMMODBUS_MIN_MULTIPLE_REGISTERS_WRITE <= nSizeWrite) && (0x007B >= nSizeWrite)  
                  && ((nSizeWriteBytes == (nSizeWrite << 1)) || (bBareRamTable == HMDRV_FALSE))
                   )
                {
                    /* Write To Memory */
                    eException = HMMODBUS_CopyFromBuffer((HMDRV_U16)(HMDRV_ADDR_SIZE)nAddrWrite, pMessageIn, (HMDRV_SIZE8)nSizeWriteBytes);



                    #if HMMODBUS_ILLEGAL_ADDR_REG_NOT_IMPLEMENTED == 1
                    if (bUsedDummyDataAddressInResponseData == HMDRV_TRUE)
                    {
                        eException = HMMODBUS_ERR_ILLEGAL_ADDRESS;
                    }
                    #endif

                    /* to do: HMMODBUS_CopyFromBuffer must Return or to be checked StartAddress OK and StartAddress+QuantityRegisters OK (HMMODBUS_ERR_ILLEGAL_ADDRESS ExceptionCode 2) - done??? to be checked */
                    if (eException == HMMODBUS_ERR_NO_EXCEPTION)
                    {
                        /* success  */
                        
                        /* execute function */
                        nAddrFuncExecute = HMMODBUS_GetRegisterFunction((HMDRV_U16)(HMDRV_ADDR_SIZE)nAddrWrite, HMDRV_FALSE, bInputRegsGlobal);
                        if (nAddrFuncExecute != NULL)
                        {                                          
                            ((pfVoid_t)(HMDRV_PTR_FUNC_SIZE)nAddrFuncExecute)();
                        }
                    
                        /* Response leaves the command as is */
                        #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                        HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pResponseEnd), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(HMMODBUS_pCommBufferRx+(pResponseEnd - HMMODBUS_pCommBufferTx))), HMDRV_SIZEADDRINTBYTES(1));
                        #endif
                        pResponseEnd = HMDRV_SkipInBuffer(pResponseEnd, 1U);

                        pResponseEnd = HMDRV_ValueToBuffer16BigEndian(pResponseEnd, (HMDRV_U16)(HMDRV_ADDR_SIZE)nAddrWrite);
                        pResponseEnd = HMDRV_ValueToBuffer16BigEndian(pResponseEnd, nSizeWrite);
                    }  
                    else /* exception */
                    {
                        /* Error Exception Code 02 HMMODBUS_ERR_ILLEGAL_ADDRESS or write failed */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                        pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, eException);                  
                    }

                }
                else
                {
                    /* Error Exception Code 03 HMMODBUS_ERR_ILLEGAL_LENGTH */
                    pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                    pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_LENGTH);   
                }     
                break;

            case HMMODBUS_CMD_WR_COIL:
                pMessageIn = HMDRV_AddressFromBufferBigEndian((HMDRV_ADDR_PNTR_MODIFIER HMDRV_ADDR*)&nAddrWrite, pMessageIn);

                /* Write To Memory */
                eException = HMMODBUS_CopyFromBufferSingleCoil((HMDRV_U16)(HMDRV_ADDR_SIZE)nAddrWrite, pMessageIn);

                #if HMMODBUS_ILLEGAL_ADDR_REG_NOT_IMPLEMENTED == 1
                if (bUsedDummyDataAddressInResponseData == HMDRV_TRUE)
                {
                    eException = HMMODBUS_ERR_ILLEGAL_ADDRESS;
                }
                #endif

                /* to do: HMMODBUS_CopyFromBuffer must Return or to be checked StartAddress OK and StartAddress+QuantityCoils OK (HMMODBUS_ERR_ILLEGAL_ADDRESS ExceptionCode 2) - done??? to be checked */
                if (eException == HMMODBUS_ERR_NO_EXCEPTION)
                {
                    /* success  */
                    

                    /* execute function */
                    nAddrFuncExecute = HMMODBUS_GetCoilFunction((HMDRV_U16)(HMDRV_ADDR_SIZE)nAddrWrite, HMDRV_FALSE, bInputRegsGlobal);
                    if (nAddrFuncExecute != NULL)
                    {                                          
                        ((pfVoid_t)(HMDRV_PTR_FUNC_SIZE)nAddrFuncExecute)();
                    }
                    
                    
                    /* Response leaves the command as is */
                    #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                    HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pResponseEnd), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(HMMODBUS_pCommBufferRx+(pResponseEnd - HMMODBUS_pCommBufferTx))), HMDRV_SIZEADDRINTBYTES(1));
                    #endif
                    pResponseEnd = HMDRV_SkipInBuffer(pResponseEnd, 1U);

                    pResponseEnd = HMDRV_ValueToBuffer16BigEndian(pResponseEnd, (HMDRV_U16)(HMDRV_ADDR_SIZE)nAddrWrite);
                    
                    
                    //was before new add below to return real read data
                    //pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, *(pMessageIn + 0));
                    //pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, *(pMessageIn + 1));
                    
                    /* Return real Readback Data */
                    pResponseEnd = HMMODBUS_CopyToBufferSingleCoil(pResponseEnd, (HMDRV_U16)(HMDRV_ADDR_SIZE)nAddrWrite);
                    
                    
                }  
                else /* exception */
                {
                    /* Error Exception Code 02 HMMODBUS_ERR_ILLEGAL_ADDRESS or write failed */
                    pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                    pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, eException);                  
                }

                break;

            case HMMODBUS_CMD_WR_COILS:
                pMessageIn = HMDRV_AddressFromBufferBigEndian( (HMDRV_ADDR_PNTR_MODIFIER HMDRV_ADDR*)&nAddrWrite, pMessageIn);
                pMessageIn = HMDRV_ValueFromBuffer16BigEndian(&nSizeWriteBits, pMessageIn);
                pMessageIn = HMDRV_ValueFromBuffer8(&nSizeWriteBytes, pMessageIn);

                /* to do: check nSizeWriteBytes and nSizeWrite values for error */
                if ( (HMMODBUS_MIN_MULTIPLE_COILS_WRITE <= nSizeWriteBits) && (2000 >= nSizeWriteBits)  
                  && (nSizeWriteBytes == ((nSizeWriteBits+7) >> 3))
                   )
                {
                    /* Write To Memory */
                    eException = HMMODBUS_CopyFromBufferCoil((HMDRV_U16)(HMDRV_ADDR_SIZE)nAddrWrite, pMessageIn, (HMDRV_SIZE8)nSizeWriteBits);



                    #if HMMODBUS_ILLEGAL_ADDR_REG_NOT_IMPLEMENTED == 1
                    if (bUsedDummyDataAddressInResponseData == HMDRV_TRUE)
                    {
                        eException = HMMODBUS_ERR_ILLEGAL_ADDRESS;
                    }
                    #endif

                    /* to do: HMMODBUS_CopyFromBuffer must Return or to be checked StartAddress OK and StartAddress+QuantityCoils OK (HMMODBUS_ERR_ILLEGAL_ADDRESS ExceptionCode 2) - done??? to be checked */
                    if (eException == HMMODBUS_ERR_NO_EXCEPTION)
                    {
                        /* success  */
                        /* Response leaves the command as is */
                        #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                        HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pResponseEnd), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(HMMODBUS_pCommBufferRx+(pResponseEnd - HMMODBUS_pCommBufferTx))), HMDRV_SIZEADDRINTBYTES(1));
                        #endif
                        pResponseEnd = HMDRV_SkipInBuffer(pResponseEnd, 1U);

                        pResponseEnd = HMDRV_ValueToBuffer16BigEndian(pResponseEnd, (HMDRV_U16)(HMDRV_ADDR_SIZE)nAddrWrite);
                        pResponseEnd = HMDRV_ValueToBuffer16BigEndian(pResponseEnd, nSizeWriteBits);
                    }  
                    else /* exception */
                    {
                        /* Error Exception Code 02 HMMODBUS_ERR_ILLEGAL_ADDRESS or write failed */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                        pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, eException);                  
                    }

                }
                else
                {
                    /* Error Exception Code 03 HMMODBUS_ERR_ILLEGAL_LENGTH */
                    pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                    pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_LENGTH);   
                }     
                break;



            case HMMODBUS_CMD_RD_SERVER_ID:
                /* Response leaves the command as is */
                #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pResponseEnd), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(HMMODBUS_pCommBufferRx+(pResponseEnd - HMMODBUS_pCommBufferTx))), HMDRV_SIZEADDRINTBYTES(1));
                #endif
                pResponseEnd = HMDRV_SkipInBuffer(pResponseEnd, 1U);

                pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, HMMODBUS_u8ReadServerIDLength + 2);
                pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, HMMODBUS_u8ReadServerIDNumber);
                pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, HMMODBUS_u8ReadServerOnStatus);   /* 0 or 0xFF */

                /* Return RD_SERVER_ID */
                bStringDataExpected = 0;    //to do implement strings with queue
                u8ElementSize = HMDRV_ELEMENT_SIZE_BYTE;
                pu8ElementSizeTable = NULL;
                nAddrRead = (HMDRV_U32)HMMODBUS_pu8ReadServerIDDatAdr;
                nBytesRead = HMMODBUS_u8ReadServerIDLength;
                pResponseEnd = HMDRV_CopyToBufferModbus(pResponseEnd, (HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8) nBytesRead, u8ElementSize, pu8ElementSizeTable);
                break;


            case HMMODBUS_CMD_DIAGNOSTICS:
                /* sub command */
                pMessageIn = HMDRV_ValueFromBuffer8(&nSubCmd, pMessageIn);
                if (nSubCmd == HMMODBUS_CMD_SUB_RETURN_SERVER_MSG_CNT)
                {
                    /* Response leaves the command and sub-command as is */
                    #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                    HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pResponseEnd), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(HMMODBUS_pCommBufferRx+(pResponseEnd - HMMODBUS_pCommBufferTx))), HMDRV_SIZEADDRINTBYTES(2));
                    #endif
                    pResponseEnd = HMDRV_SkipInBuffer(pResponseEnd, 2U);

                    /* Return Complete Received Messages Count */
                    pResponseEnd = HMDRV_ValueToBuffer16(pResponseEnd, u16ModbusCompleteReceiveMessageCounterThisDevice);
                }
                else if (nSubCmd == HMMODBUS_CMD_SUB_CLEAR_CNT_AND_DIAG_REG)
                {
                    /* Response leaves the command and sub-command as is */
                    #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                    HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pResponseEnd), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(HMMODBUS_pCommBufferRx+(pResponseEnd - HMMODBUS_pCommBufferTx))), HMDRV_SIZEADDRINTBYTES(2));
                    #endif
                    pResponseEnd = HMDRV_SkipInBuffer(pResponseEnd, 2U);

                    /* Clear The counters */
                    u16ModbusCompleteReceiveMessageCounterThisDevice = 0;

                    u16ModbusStartedReceiveMessageCounter = 0;
                    u16ModbusCompleteReceiveMessageCounter = 0;

                    u16ModbusBufferOverflowReceiveMessageCounter = 0;
                    u16ModbusBadCSReceiveMessageCounter = 0;
                    u16ModbusBadTimeoutReceiveMessageCounter = 0;



                    /* Response echoes Received Data (should be 0x0000) */
                    #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                    HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pResponseEnd), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(HMMODBUS_pCommBufferRx+(pResponseEnd - HMMODBUS_pCommBufferTx))), HMDRV_SIZEADDRINTBYTES(2));
                    #endif
                    pResponseEnd = HMDRV_SkipInBuffer(pResponseEnd, 2U);

                }
                else
                {
                    pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                    pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_FUNCTION);
                }


                break;

            case HMMODBUS_CMD_ENCAPSULATED_INTERFACE:
                /* sub command */
                pMessageIn = HMDRV_ValueFromBuffer8(&nSubCmd, pMessageIn);
                if (nSubCmd == HMMODBUS_CMD_SUB_RD_DEVICE_ID)
                {
                    /* Response leaves the command and sub-command as is */
                    #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                    HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pResponseEnd), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(HMMODBUS_pCommBufferRx+(pResponseEnd - HMMODBUS_pCommBufferTx))), HMDRV_SIZEADDRINTBYTES(2));
                    #endif
                    pResponseEnd = HMDRV_SkipInBuffer(pResponseEnd, 2U);

                    /* Device ID Code */
                    pMessageIn = HMDRV_ValueFromBuffer8(&nIDCode, pMessageIn);

                    switch (nIDCode)
                    {

                    /* request to get the basic device identification */
                    case 0x01:
                        /* Response leaves the Device ID Code as is */
                        #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                        HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pResponseEnd), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(HMMODBUS_pCommBufferRx+(pResponseEnd - HMMODBUS_pCommBufferTx))), HMDRV_SIZEADDRINTBYTES(1));
                        #endif
                        pResponseEnd = HMDRV_SkipInBuffer(pResponseEnd, 1U);

                        /* Conformity Level */
                        //pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nIDCode | HMMODBUS_u8ReadDeviceIDConfLvlMask));
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, HMMODBUS_u8ReadDeviceIDConfLvl);

                        /* More Follows */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, 0x00);

                        /* Next Object Id - if more follows */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, 0x00);

                        /* Number of objects */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, 0x03);

                        pu8ElementSizeTable = NULL;
                        u8ElementSize = HMDRV_ELEMENT_SIZE_STRING;

                        /* object 0 (VendorName) */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, HMMODBUS_ID_OBJ_VENDORNAME);
                        nAddrRead = (HMDRV_U32)HMMODBUS_pu8ReadDeviceIDVendorName;
                        nBytesRead = HMMODBUS_u8ReadDeviceIDVendorNameLength;
                        nBytesRead = HMDRV_GetStringLength((HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8)nBytesRead);
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nBytesRead);
                        (void)HMDRV_CopyToBufferModbus(pResponseEnd, (HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8) nBytesRead, u8ElementSize, pu8ElementSizeTable);
                        pResponseEnd += nBytesRead;

                        /* object 1 (ProductCode) */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, HMMODBUS_ID_OBJ_PRODUCTCODE);
                        nAddrRead = (HMDRV_U32)HMMODBUS_pu8ReadDeviceIDProductCode;
                        nBytesRead = HMMODBUS_u8ReadDeviceIDProductCodeLength;
                        nBytesRead = HMDRV_GetStringLength((HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8)nBytesRead);
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nBytesRead);
                        (void)HMDRV_CopyToBufferModbus(pResponseEnd, (HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8) nBytesRead, u8ElementSize, pu8ElementSizeTable);
                        pResponseEnd += nBytesRead;

                        /* object 2 (MajorMinorRevision) */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, HMMODBUS_ID_OBJ_MAJORMINORREVISION);
                        nAddrRead = (HMDRV_U32)HMMODBUS_pu8ReadDeviceIDMajorMinorRevision;
                        nBytesRead = HMMODBUS_u8ReadDeviceIDMajorMinorRevisionLength;
                        nBytesRead = HMDRV_GetStringLength((HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8)nBytesRead);
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nBytesRead);
                        (void)HMDRV_CopyToBufferModbus(pResponseEnd, (HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8) nBytesRead, u8ElementSize, pu8ElementSizeTable);
                        pResponseEnd += nBytesRead;

                        break;


                    /* request to get the regular device identification */
                    case 0x02:
                        /* Response leaves the Device ID Code as is */
                        #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                        HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pResponseEnd), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(HMMODBUS_pCommBufferRx+(pResponseEnd - HMMODBUS_pCommBufferTx))), HMDRV_SIZEADDRINTBYTES(1));
                        #endif
                        pResponseEnd = HMDRV_SkipInBuffer(pResponseEnd, 1U);

                        /* Conformity Level */
                        //pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nIDCode | HMMODBUS_u8ReadDeviceIDConfLvlMask));
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, HMMODBUS_u8ReadDeviceIDConfLvl);

                        /* More Follows */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, 0x00);

                        /* Next Object Id - if more follows */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, 0x00);

                        /* Number of objects */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, 0x03);

                        pu8ElementSizeTable = NULL;
                        u8ElementSize = HMDRV_ELEMENT_SIZE_STRING;

                        /* object 3 (VendorUrl) */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, HMMODBUS_ID_OBJ_VENDORURL);
                        nAddrRead = (HMDRV_U32)HMMODBUS_pu8ReadDeviceIDVendorUrl;
                        nBytesRead = HMMODBUS_u8ReadDeviceIDVendorUrlLength;
                        nBytesRead = HMDRV_GetStringLength((HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8)nBytesRead);
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nBytesRead);
                        (void)HMDRV_CopyToBufferModbus(pResponseEnd, (HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8) nBytesRead, u8ElementSize, pu8ElementSizeTable);
                        pResponseEnd += nBytesRead;

                        /* object 4 (ProductName) */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, HMMODBUS_ID_OBJ_PRODUCTNAME);
                        nAddrRead = (HMDRV_U32)HMMODBUS_pu8ReadDeviceIDProductName;
                        nBytesRead = HMMODBUS_u8ReadDeviceIDProductNameLength;
                        nBytesRead = HMDRV_GetStringLength((HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8)nBytesRead);
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nBytesRead);
                        (void)HMDRV_CopyToBufferModbus(pResponseEnd, (HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8) nBytesRead, u8ElementSize, pu8ElementSizeTable);
                        pResponseEnd += nBytesRead;

                        /* object 5 (ModelName) */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, HMMODBUS_ID_OBJ_MODELNAME);
                        nAddrRead = (HMDRV_U32)HMMODBUS_pu8ReadDeviceIDModelName;
                        nBytesRead = HMMODBUS_u8ReadDeviceIDModelNameLength;
                        nBytesRead = HMDRV_GetStringLength((HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8)nBytesRead);
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nBytesRead);
                        (void)HMDRV_CopyToBufferModbus(pResponseEnd, (HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8) nBytesRead, u8ElementSize, pu8ElementSizeTable);
                        pResponseEnd += nBytesRead;

                        /* object 6 (UserApplicationName) */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, HMMODBUS_ID_OBJ_USERAPPLICATIONNAME);
                        nAddrRead = (HMDRV_U32)HMMODBUS_pu8ReadDeviceIDUserApplicationName;
                        nBytesRead = HMMODBUS_u8ReadDeviceIDUserApplicationNameLength;
                        nBytesRead = HMDRV_GetStringLength((HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8)nBytesRead);
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nBytesRead);
                        (void)HMDRV_CopyToBufferModbus(pResponseEnd, (HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8) nBytesRead, u8ElementSize, pu8ElementSizeTable);
                        pResponseEnd += nBytesRead;

                        break;

                    /* request to get the extended device identification */
                    case 0x03:

                        /* Response leaves the Device ID Code as is */
                        #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                        HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pResponseEnd), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(HMMODBUS_pCommBufferRx+(pResponseEnd - HMMODBUS_pCommBufferTx))), HMDRV_SIZEADDRINTBYTES(1));
                        #endif
                        pResponseEnd = HMDRV_SkipInBuffer(pResponseEnd, 1U);

                        /* Conformity Level */
                        //pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nIDCode | HMMODBUS_u8ReadDeviceIDConfLvlMask));
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, HMMODBUS_u8ReadDeviceIDConfLvl);

                        /* More Follows */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, 0x00);

                        /* Next Object Id - if more follows */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, 0x00);

                        /* Number of objects */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, 0x03);

                        pu8ElementSizeTable = NULL;
                        u8ElementSize = HMDRV_ELEMENT_SIZE_STRING;

                        /* object 0x80 (UserReserve0) */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, HMMODBUS_ID_OBJ_USERRESERVE0);
                        nAddrRead = (HMDRV_U32)HMMODBUS_pu8ReadDeviceIDUserReserve0;
                        nBytesRead = HMMODBUS_u8ReadDeviceIDUserReserve0Length;
                        nBytesRead = HMDRV_GetStringLength((HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8)nBytesRead);
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nBytesRead);
                        (void)HMDRV_CopyToBufferModbus(pResponseEnd, (HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8) nBytesRead, u8ElementSize, pu8ElementSizeTable);
                        pResponseEnd += nBytesRead;

                        /* object 0x81 (UserReserve1) */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, HMMODBUS_ID_OBJ_USERRESERVE1);
                        nAddrRead = (HMDRV_U32)HMMODBUS_pu8ReadDeviceIDUserReserve1;
                        nBytesRead = HMMODBUS_u8ReadDeviceIDUserReserve1Length;
                        nBytesRead = HMDRV_GetStringLength((HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8)nBytesRead);
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nBytesRead);
                        (void)HMDRV_CopyToBufferModbus(pResponseEnd, (HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8) nBytesRead, u8ElementSize, pu8ElementSizeTable);
                        pResponseEnd += nBytesRead;

                        /* object 0x82 (UserReserve2) */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, HMMODBUS_ID_OBJ_USERRESERVE2);
                        nAddrRead = (HMDRV_U32)HMMODBUS_pu8ReadDeviceIDUserReserve2;
                        nBytesRead = HMMODBUS_u8ReadDeviceIDUserReserve2Length;
                        nBytesRead = HMDRV_GetStringLength((HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8)nBytesRead);
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nBytesRead);
                        (void)HMDRV_CopyToBufferModbus(pResponseEnd, (HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8) nBytesRead, u8ElementSize, pu8ElementSizeTable);
                        pResponseEnd += nBytesRead;

                        break;

                    /* request to get device identification by object */
                    case 0x04:

                        /* Response leaves the Device ID Code as is */
                        #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                        HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pResponseEnd), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(HMMODBUS_pCommBufferRx+(pResponseEnd - HMMODBUS_pCommBufferTx))), HMDRV_SIZEADDRINTBYTES(1));
                        #endif
                        pResponseEnd = HMDRV_SkipInBuffer(pResponseEnd, 1U);






                        /* Conformity Level */
                        //pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nIDCode | HMMODBUS_u8ReadDeviceIDConfLvlMask)); //???
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, HMMODBUS_u8ReadDeviceIDConfLvl);

                        /* More Follows */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, 0x00);

                        /* Next Object Id - if more follows */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, 0x00);

                        /* Number of objects */
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, 0x01);

                        pu8ElementSizeTable = NULL;
                        u8ElementSize = HMDRV_ELEMENT_SIZE_STRING;


                        /* Object ID Code */
                        pMessageIn = HMDRV_ValueFromBuffer8(&nIDCode, pMessageIn);

                        /* Response leaves the Object ID Code as is */
                        #if HMDRV_MODBUS_SHARED_RX_TX_BUFFER == 0
                        HMDRV_CopyMemory(HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)pResponseEnd), HMDRV_ADDRFROMPTR((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(HMMODBUS_pCommBufferRx+(pResponseEnd - HMMODBUS_pCommBufferTx))), HMDRV_SIZEADDRINTBYTES(1));
                        #endif
                        pResponseEnd = HMDRV_SkipInBuffer(pResponseEnd, 1U);

                        switch (nIDCode)
                        {
                        case HMMODBUS_ID_OBJ_VENDORNAME:
                            nAddrRead = (HMDRV_U32)HMMODBUS_pu8ReadDeviceIDVendorName;
                            nBytesRead = HMMODBUS_u8ReadDeviceIDVendorNameLength;
                            nBytesRead = HMDRV_GetStringLength((HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8)nBytesRead);
                            pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nBytesRead);
                            (void)HMDRV_CopyToBufferModbus(pResponseEnd, (HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8) nBytesRead, u8ElementSize, pu8ElementSizeTable);
                            pResponseEnd += nBytesRead;
                            break;

                        case HMMODBUS_ID_OBJ_PRODUCTCODE:
                            nAddrRead = (HMDRV_U32)HMMODBUS_pu8ReadDeviceIDProductCode;
                            nBytesRead = HMMODBUS_u8ReadDeviceIDProductCodeLength;
                            nBytesRead = HMDRV_GetStringLength((HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8)nBytesRead);
                            pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nBytesRead);
                            (void)HMDRV_CopyToBufferModbus(pResponseEnd, (HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8) nBytesRead, u8ElementSize, pu8ElementSizeTable);
                            pResponseEnd += nBytesRead;
                            break;

                        case HMMODBUS_ID_OBJ_MAJORMINORREVISION:
                            nAddrRead = (HMDRV_U32)HMMODBUS_pu8ReadDeviceIDMajorMinorRevision;
                            nBytesRead = HMMODBUS_u8ReadDeviceIDMajorMinorRevisionLength;
                            nBytesRead = HMDRV_GetStringLength((HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8)nBytesRead);
                            pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nBytesRead);
                            (void)HMDRV_CopyToBufferModbus(pResponseEnd, (HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8) nBytesRead, u8ElementSize, pu8ElementSizeTable);
                            pResponseEnd += nBytesRead;
                            break;

                        case HMMODBUS_ID_OBJ_VENDORURL:
                            nAddrRead = (HMDRV_U32)HMMODBUS_pu8ReadDeviceIDVendorUrl;
                            nBytesRead = HMMODBUS_u8ReadDeviceIDVendorUrlLength;
                            nBytesRead = HMDRV_GetStringLength((HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8)nBytesRead);
                            pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nBytesRead);
                            (void)HMDRV_CopyToBufferModbus(pResponseEnd, (HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8) nBytesRead, u8ElementSize, pu8ElementSizeTable);
                            pResponseEnd += nBytesRead;
                            break;

                        case HMMODBUS_ID_OBJ_PRODUCTNAME:
                            nAddrRead = (HMDRV_U32)HMMODBUS_pu8ReadDeviceIDProductName;
                            nBytesRead = HMMODBUS_u8ReadDeviceIDProductNameLength;
                            nBytesRead = HMDRV_GetStringLength((HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8)nBytesRead);
                            pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nBytesRead);
                            (void)HMDRV_CopyToBufferModbus(pResponseEnd, (HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8) nBytesRead, u8ElementSize, pu8ElementSizeTable);
                            pResponseEnd += nBytesRead;
                            break;

                        case HMMODBUS_ID_OBJ_MODELNAME:
                            nAddrRead = (HMDRV_U32)HMMODBUS_pu8ReadDeviceIDModelName;
                            nBytesRead = HMMODBUS_u8ReadDeviceIDModelNameLength;
                            nBytesRead = HMDRV_GetStringLength((HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8)nBytesRead);
                            pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nBytesRead);
                            (void)HMDRV_CopyToBufferModbus(pResponseEnd, (HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8) nBytesRead, u8ElementSize, pu8ElementSizeTable);
                            pResponseEnd += nBytesRead;
                            break;

                        case HMMODBUS_ID_OBJ_USERAPPLICATIONNAME:
                            nAddrRead = (HMDRV_U32)HMMODBUS_pu8ReadDeviceIDUserApplicationName;
                            nBytesRead = HMMODBUS_u8ReadDeviceIDUserApplicationNameLength;
                            nBytesRead = HMDRV_GetStringLength((HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8)nBytesRead);
                            pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nBytesRead);
                            (void)HMDRV_CopyToBufferModbus(pResponseEnd, (HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8) nBytesRead, u8ElementSize, pu8ElementSizeTable);
                            pResponseEnd += nBytesRead;
                            break;

                        case HMMODBUS_ID_OBJ_USERRESERVE0:
                            nAddrRead = (HMDRV_U32)HMMODBUS_pu8ReadDeviceIDUserReserve0;
                            nBytesRead = HMMODBUS_u8ReadDeviceIDUserReserve0Length;
                            nBytesRead = HMDRV_GetStringLength((HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8)nBytesRead);
                            pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nBytesRead);
                            (void)HMDRV_CopyToBufferModbus(pResponseEnd, (HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8) nBytesRead, u8ElementSize, pu8ElementSizeTable);
                            pResponseEnd += nBytesRead;
                            break;

                        case HMMODBUS_ID_OBJ_USERRESERVE1:
                            nAddrRead = (HMDRV_U32)HMMODBUS_pu8ReadDeviceIDUserReserve1;
                            nBytesRead = HMMODBUS_u8ReadDeviceIDUserReserve1Length;
                            nBytesRead = HMDRV_GetStringLength((HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8)nBytesRead);
                            pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nBytesRead);
                            (void)HMDRV_CopyToBufferModbus(pResponseEnd, (HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8) nBytesRead, u8ElementSize, pu8ElementSizeTable);
                            pResponseEnd += nBytesRead;
                            break;

                        case HMMODBUS_ID_OBJ_USERRESERVE2:
                            nAddrRead = (HMDRV_U32)HMMODBUS_pu8ReadDeviceIDUserReserve2;
                            nBytesRead = HMMODBUS_u8ReadDeviceIDUserReserve2Length;
                            nBytesRead = HMDRV_GetStringLength((HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8)nBytesRead);
                            pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, nBytesRead);
                            (void)HMDRV_CopyToBufferModbus(pResponseEnd, (HMDRV_ADDR)nAddrRead, (HMDRV_SIZE8) nBytesRead, u8ElementSize, pu8ElementSizeTable);
                            pResponseEnd += nBytesRead;
                            break;

                        default:
                            /* Error message bytes count */
                            pResponseEnd = pResponseStart + 1;
                            pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                            pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_ADDRESS);
                            break;

                        }


                        break;

                    default:
                        /* Error message bytes count */
                        pResponseEnd = pResponseStart + 1;
                        pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                        pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_FUNCTION);
                        break;
                    }




                }
                else if (nSubCmd == HMMODBUS_CMD_SUB_CANOPEN_GEN_REF)
                {
                    pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                    pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_FUNCTION);
                }
                else
                {
                    pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                    pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_FUNCTION);
                }


                break;

            default:
                /* Error Function Not Supported */
                pResponseEnd = HMDRV_ValueToBuffer8(pResponseEnd, (nCmd | HMMODBUS_ERR_CMD_OR_MASK));
                pResponseEnd = HMDRV_ConstToBuffer8(pResponseEnd, HMMODBUS_ERR_ILLEGAL_FUNCTION);
                break;
        }

        
        #if !HMDRV_MODBUS_RESPONSE_TO_BROADCAST_MESSAGE
        if (nDeviceAddress != HMMODBUS_DEVICE_ADDRESS_BROADCAST)
        #endif
        {
            HMDRV_SIZE8 nSize = (HMDRV_SIZE8)(pResponseEnd - pResponseStart);
            HMMODBUS_vSendResponse(pResponseStart, nSize, HMDRV_TRUE);
            return HMDRV_TRUE;
        }
    }
    

    return bResult;
}


void HMMODBUS_DelayedTx(HMDRV_U8 u8TxChar)
{
    #if HMDRV_SCI_TX_ISR == 1
    HMDRV_TX_INT_ENA(u8UartModBusIndex);
    #endif
    #if HMDRV_TE_MIN_TIME
    HMDRV_SCI_TE(u8UartModBusIndex);
    #endif
    HMDRV_SCI_PUTCHAR(u8UartModBusIndex, u8TxChar);
}


/**************************************************************************//*!
*
* @brief    Start listening on a serial line
*
* Reset the receiver machine and start listening on a serial line
*
******************************************************************************/

void HMMODBUS_Listen(void)
{
    uint16_t index;
    
    /* Several times check received last byte from own transmit response message - there is slight delay one or 2 mostly supposed the stop bits from TxComplete to RxComplete */
    #if !HMDRV_SCI_TWOWIRE_ONLY
    /* read-out and ignore any received character (loopback) */
    #if HMDRV_SCI_READ_MULTIPLE_RX_DATA
    while (HMDRV_READY_RX(u8UartModBusIndex))
    #else
    if (HMDRV_READY_RX(u8UartModBusIndex))
    #endif
    {
        /*lint -esym(550, nRxChar) */
        volatile HMDRV_U16 nRxChar;
        nRxChar = HMDRV_SCI_GETCHAR(u8UartModBusIndex);
    }
    #endif
    
    /* Flush Tx Buffer For Any case */
    for (index = 0; index < HMDRV_MODBUS_BUFFER_SIZE; index++)
    {
        HMMODBUS_pCommBufferTx[index] = 0;
    }
    
    /* Flush Rx Buffer For Any case */
    for (index = 0; index < HMDRV_MODBUS_BUFFER_SIZE; index++)
    {
        HMMODBUS_pCommBufferRx[index] = 0;
    }
    
    
    
    HMMODBUS_nRxTodo = 0U;
    
    /* disable transmitter state machine */
    HMMODBUS_wFlagsRxTx.sFlg.bTxActive = 0U;

    HMMODBUS_wFlagsRxTx.sFlg.bTxWaitTC = 0U;
    
    
    /* Several times check received last byte from own transmit response message - there is slight delay one or 2 mostly supposed the stop bits from TxComplete to RxComplete */
    #if !HMDRV_SCI_TWOWIRE_ONLY
    /* read-out and ignore any received character (loopback) */
    #if HMDRV_SCI_READ_MULTIPLE_RX_DATA
    while (HMDRV_READY_RX(u8UartModBusIndex))
    #else
    if (HMDRV_READY_RX(u8UartModBusIndex))
    #endif
    {
        /*lint -esym(550, nRxChar) */
        volatile HMDRV_U16 nRxChar;
        nRxChar = HMDRV_SCI_GETCHAR(u8UartModBusIndex);
    }
    #endif
    
    

    /* disable transmitter, enable receiver (enables single-wire connection) */
    #if !HMDRV_SCI_TWOWIRE_ONLY
    HMDRV_SCI_TD(u8UartModBusIndex);
    HMDRV_SCI_RE(u8UartModBusIndex);
    #endif

}



void HMMODBUS_vSetPortIndex(HMDRV_U8 u8Index)
{ 
    u8UartModBusIndex = u8Index;
}


/**************************************************************************//*!
*
* @brief    Serial communication initialization
*
******************************************************************************/

void HMMODBUS_vInit(void)
{   
    /* initialize all state variables */
    HMMODBUS_wFlags.nAll = 0U;
    HMMODBUS_wFlagsRxTx.nAll = 0U;
    HMMODBUS_nTxTodo = 0U;
    HMMODBUS_pRxBuff = (HMDRV_BPTR)HMMODBUS_pCommBufferRx;
    HMMODBUS_pTxBuff = (HMDRV_BPTR)HMMODBUS_pCommBufferTx;

    /* Initialize SCI and JTAG interface */
    #if HMDRV_SCI_TWOWIRE_ONLY
    /* to enable TX and RX together in FreeMASTER initialization */
    HMDRV_SCI_TE_RE(u8UartModBusIndex);
    #endif
    
    #if HMDRV_SCI_TX_DELAY_ISR    
    HMDRV_SETUP_TX_DELAY_FUNCTION(u8UartModBusIndex, HMMODBUS_DelayedTx);
    #endif

    HMMODBUS_vSetFreeRunTimerPrescaller(TMR_u16GetFreeRunTimerPrescaller());

    /* start listening for commands */
    HMMODBUS_Listen();
}

void HMMODBUS_vProcessTxDelay(HMDRV_TIMER nTimeTickDelay)
{
    if (HMMODBUS_u32TxDelayCounter > 0)
    {
        if (HMMODBUS_wFlagsRxTx.sFlg.bTxDelaySkipReduce)
        {
                HMMODBUS_wFlagsRxTx.sFlg.bTxDelaySkipReduce = 0U;   /* reduce first time delay skip here */
        }
        else
        {
            if (HMMODBUS_u32TxDelayCounter > nTimeTickDelay)
            {
                HMMODBUS_u32TxDelayCounter -= nTimeTickDelay;
            }
            else
            {
                HMMODBUS_u32TxDelayCounter = 0;
            }
        }
    }
}




HMDRV_TIMER HMMODBUS_vProcessFreeTimer(void)
{
    static HMDRV_TIMER nLastTimerValue = 0;
    
    HMDRV_TIMER nTimerValue;
    HMDRV_TIMER nTimeTickDelay;

    nTimerValue = HMDRV_GET_FREE_RUN_TIMER(); 
    nTimeTickDelay = nTimerValue - nLastTimerValue;
#if HMDRV_FREE_RUN_TIMER_DECREMENT_COUNTER
    nTimeTickDelay = 0 - nTimeTickDelay;
#endif
    
    nLastTimerValue = nTimerValue;
    
    return nTimeTickDelay;
}

/**************************************************************************//*!
*
* @brief    Handle SCI communication (TX on ISR)
*
* This function checks the SCI flags and calls the Tx functions
*
* @note This function can be called from SCI ISR
*
******************************************************************************/

void HMMODBUS_vProcessSCITXISR (void)
{
    if (HMMODBUS_wFlagsRxTx.sFlg.bTxActive)
    {
        /* ready to accept another character? */
        //if (HMDRV_READY_TX())
        {
            HMDRV_U8 ch;
            /* just put the byte into the SCI transmit buffer */
            if(!HMMODBUS_Tx(&ch)) 
            {
                HMDRV_SCI_PUTCHAR(u8UartModBusIndex, (HMDRV_U8) ch); 
            }
        }
    } 
    else
    {
        /* should not enter here */
        #if HMDRV_SCI_TX_ISR == 1
        HMDRV_TX_INT_DIS(u8UartModBusIndex);
        #endif
    }
}


/**************************************************************************//*!
*
* @brief    Handle SCI communication (both TX and RX)
*
* This function checks the SCI flags and calls the Rx and/or Tx functions
*
* @note This function can be called either from SCI ISR or from the polling routine
*
******************************************************************************/
volatile HMDRV_TIMER nTimeTickDelayModbus;
volatile HMDRV_TIMER nTimeTickDelayModbusMin;
volatile HMDRV_TIMER nTimeTickDelayModbusMax;

void HMMODBUS_vProcessSCI(void)
{
    
    
    nTimeTickDelayModbus = HMMODBUS_vProcessFreeTimer();
    if (nTimeTickDelayModbusMin > nTimeTickDelayModbus)
    {
        nTimeTickDelayModbusMin = nTimeTickDelayModbus;
    }
    if (nTimeTickDelayModbusMax < nTimeTickDelayModbus)
    {
        nTimeTickDelayModbusMax= nTimeTickDelayModbus;
    }
    
    HMMODBUS_vProcessRxDelay(nTimeTickDelayModbus);
    HMMODBUS_vProcessTxDelay(nTimeTickDelayModbus);
            
    /* read & clear status     */
    

    /* transmitter active MODBUS? */
    if (HMMODBUS_wFlagsRxTx.sFlg.bTxActive)
    {
        
        
        
        
        #if HMDRV_MODBUS_TX_DELAY
        if (HMMODBUS_wFlagsRxTx.sFlg.bTxDelay)  
        {
            if (HMMODBUS_u32TxDelayCounter == 0)
            {
                HMMODBUS_wFlagsRxTx.sFlg.bTxDelay = 0;
                
                #if HMDRV_SCI_TX_ISR == 1
                HMDRV_TX_INT_ENA(u8UartModBusIndex);
                #endif
                #if HMDRV_TE_MIN_TIME
                HMDRV_SCI_TE(u8UartModBusIndex);
                #endif
                HMDRV_SCI_PUTCHAR(u8UartModBusIndex, *HMMODBUS_pTxBuff++);
            }
        }
        
        
        if (HMMODBUS_wFlagsRxTx.sFlg.bTxDelay == 0U)    
        #endif
        {
            #if HMDRV_SCI_TX_ISR == 0
            /* ready to accept another character? */
            if (HMDRV_READY_TX(u8UartModBusIndex))
            {
                HMDRV_U8 ch;
                /* just put the byte into the SCI transmit buffer */
                if(!HMMODBUS_Tx(&ch)) 
                {
                    HMDRV_SCI_PUTCHAR(u8UartModBusIndex, (HMDRV_U8) ch);
                }
            }
            #endif
        }
        /* waiting for transmission complete flag? */
        if(HMMODBUS_wFlagsRxTx.sFlg.bTxWaitTC && (HMDRV_EMPTY_TX(u8UartModBusIndex)))
        {
            /* after TC, we can switch to listen mode safely */
            HMMODBUS_Listen();
        }


        #if !HMDRV_SCI_TWOWIRE_ONLY
        /* read-out and ignore any received character (loopback) */
        #if HMDRV_SCI_READ_MULTIPLE_RX_DATA
        while (HMDRV_READY_RX(u8UartModBusIndex))
        #else
        if (HMDRV_READY_RX(u8UartModBusIndex))
        #endif
        {
            /*lint -esym(550, nRxChar) */
            volatile HMDRV_U16 nRxChar;
            nRxChar = HMDRV_SCI_GETCHAR(u8UartModBusIndex);
        }
        #endif
    }
    /* transmitter not active, ready to receive */
    else
    {
        /* data byte received? */
        #if HMDRV_SCI_READ_MULTIPLE_RX_DATA
        while (HMDRV_READY_RX(u8UartModBusIndex))
        #else
        if (HMDRV_READY_RX(u8UartModBusIndex))
        #endif
        {
            HMDRV_BCHR nRxChar = 0U;
            nRxChar = (HMDRV_BCHR) HMDRV_SCI_GETCHAR(u8UartModBusIndex);
            (void)HMMODBUS_Rx(nRxChar);  
        }
    }

}


HMDRV_U32 HMMODBUS_u32GetFreeRunTimerReceivedMessage (void)
{
    return HMMODBUS_u32FreeRunTimerReceivedMessage;
}

HMDRV_BOOL HMMODBUS_bGetReceivedMessage (void)
{
    HMDRV_BOOL bResult = HMMODBUS_bReceivedMessage;

    HMMODBUS_bReceivedMessage = false;

    return bResult;
}




#endif
