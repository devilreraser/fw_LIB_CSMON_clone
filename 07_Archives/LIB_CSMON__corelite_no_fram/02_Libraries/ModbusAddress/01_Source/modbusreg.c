/* *****************************************************************************
 * File:   modbusreg.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 05 21 11:58
 * 
 * Description: Modbus Registers
 * 
 **************************************************************************** */
#ifdef _WIN32
#pragma once
#endif // _WIN32


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdlib.h>
#include <stdint.h>
//#include "uart_drv.h"
#include "sys_drv.h"

#include "modbusreg.h"
#include "hmlib.h"
#include "hmmodbus.h"

#include "csmon_adr.h"



#if USE_SPI_FLIR
#error "Include sys_drv.h instead"
#include "i2cmem.h"
#include "spimem.h"
#include "flir.h"
#endif

#if USE_CONTROL
#include "control.h"
#include "testproc.h"
#endif


#include "hmrecparam_t.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */





/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */
#define s16WordHi(x)          (int16_t)((uint32_t)x >> 16)
#define u16WordLo(x)          (uint16_t)(x)

#ifndef MCLIB_H
/* *****************************************************************************
 * MCLIB_s32FractionalQ_15_16 
 * 
 * Description: signed 32 bit fractional [-32768..32767.9999] representation
 **************************************************************************** */
#define  FRACTQ_15_16_MAX (32767.9999847412109375)  /* 2^31-1)/2^16 */
#define MCLIB_s32FractionalQ_15_16(fltValue)    \
    ((int32_t) (((fltValue) < FRACTQ_15_16_MAX) \
        ? (((fltValue) >= 0) \
            ? ((int32_t)((fltValue)*65536.0F + 0.5F)) \
            : (int32_t)0) \
        : (int32_t)INT32_MAX))

#endif






/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
HMDRV_U16 MODBUSREG_u16ModbusParamCount = 0;
HMDRV_U16 MODBUSREG_u16ParamCount = 0;
HMDRV_U32 MODBUSREG_u32ParamRecorderConfigAddress = NULL;
HMDRV_U32 MODBUSREG_u32ParamScopeConfigAddress = NULL;
HMDRV_U32 MODBUSREG_u32ParamListAddress = NULL;
HMDRV_U32 MODBUSREG_u32ParamNameAddress = NULL;
HMDRV_U32 MODBUSREG_u32ParamNumbAddress = NULL;
HMDRV_U32 MODBUSREG_u32ParamStringAddress = NULL;
HMDRV_U32 MODBUSREG_u32ParamAuxAddress = NULL;
HMDRV_U32 MODBUSREG_u32ParamReservedAddress = NULL;
HMDRV_U32 MODBUSREG_u32FileRTC_FunctionAfterWriteFileRecord = NULL;
HMDRV_U32 MODBUSREG_u32FileRTC_FunctionAfterReadFileRecord = NULL;
HMDRV_U32 MODBUSREG_u32FileParamCount_FunctionBeforeReadFileRecord = NULL;
HMDRV_U32 MODBUSREG_u32FileAutorization_FunctionAfterWriteFileRecordTable = NULL;


volatile HMDRV_U16 InputRegisterDummy = 0x1234;
volatile HMDRV_U16 InputCoilDummy = 0x0000;
volatile HMDRV_U16 CoilTest = 0x0000;



uint16_t HMMODBUS_u16RegisterAddressTableControlParametersStartIndex = 0;


uint16_t MODBUSREG_u16HoldRegFillCount = 0;     /* Count Filled Elements */
uint16_t MODBUSREG_u16HoldRegUsedCount = 0;     /* Count Maximum Used Elements */
uint16_t MODBUSREG_u16HoldRegListCount = 0;     /* Count Fixed in MODBUSREG_vInit */
//uint16_t MODBUSREG_u16HoldRegNameCount = 0;     /* Count Fixed in MODBUSREG_vInit */


uint32_t MODBUSREG_pu32RegisterAddressTable = NULL; /* Address Initialized in MODBUSREG_vInit */
#if _CSMON_IN_INTERNAL_RAM
HMMODBUS_sRegisterAddressTable_t MODBUSREG_pu32RegisterAddressTableRealocatable[BOARDCFG_CSMON_PARAM_COUNT_REQUEST_DPN]; //DPN Modbus Table with Parameters
#endif                                                                                                                   //In the original code this table is allocated in
                                                                                                                         //external RAM. Let's put it in the internal CPU RAM



uint32_t MODBUSREG_pu32RegisterAddressTableInRam = NULL; /* Address Initialized in MODBUSREG_vInit */
#if _CSMON_IN_INTERNAL_RAM
HMMODBUS_sRegisterAddressTableInRam_t MODBUSREG_pu32RegisterAddressTableInRamRealocatable[BOARDCFG_CSMON_PARAM_COUNT_REQUEST_DPN]; //DPN Auxilary Modbus Table with Parameters.
#endif                                                                                                                             //In the original code this table is allocated in
                                                                                                                                   //external RAM. Let's put it in the internal CPU RAM



#if MODBUSREG_HOLDREG_ADDR_TABLE_READ_ONLY

/* ??? 123 Mark Change to Const To Test */
const HMMODBUS_sRegisterAddressTable_t MODBUSREG_au32RegisterAddressTable[MODBUSREG_REGISTER_TABLE_SIZE] =
{
    /* 
    HMDRV_U16 u16VirtAddress;
    HMDRV_U16 u16Size;
    HMMODBUS_uRegisterAddressTableFlags uFlags;
    */
    
    
    {
        MODBUSREG_REGID_MAIN_BAUDRATE_HI,
        2,//sizeof(uint16_t),
    },
    {
        MODBUSREG_REGID_MAIN_BAUDRATE_LO,
        2,//sizeof(uint16_t),
    },
    {
        MODBUSREG_REGID_MAIN_PARITY,
        1,//sizeof(sHexMonUartModuleConfiguration.eParityAndData),
    },
  
    
    
    #if USE_CONTROL
    /* 1000 : 1009 */
    /*
    MODBUSREG_REGID_STACK1_RESERVED_REQUEST_STATUS  ,
    MODBUSREG_REGID_STACK1_CONTROL_STATE            ,
    MODBUSREG_REGID_STACK1_CONTROL_ERROR            ,                               
    MODBUSREG_REGID_STACK1_ERROR_STATE              , 
    MODBUSREG_REGID_STACK1_CLEAR_FAULTS             ,        
    MODBUSREG_REGID_STACK1_ERROR_STATUS             ,            
    MODBUSREG_REGID_STACK1_SPEED_REQUEST            ,
    MODBUSREG_REGID_STACK1_VOLTAGE_DCLINK_REQUEST   ,        
    MODBUSREG_REGID_STACK1_CURRENT_REQUEST          , 
    MODBUSREG_REGID_STACK1_RUN_REQUEST_STATUS       ,
    */
    
    /* 
    HMDRV_U16 u16VirtAddress;
    HMDRV_U16 u16Size;
    HMMODBUS_uRegisterAddressTableFlags uFlags;
    */
    {
        MODBUSREG_REGID_STACK1_RESERVED_REQUEST_STATUS,
        1,//sizeof(bTestDummyStatus),
        {setReadOnlyField_HMMODBUS_sRegisterAddressTableFlags}, 
    },
    {
        MODBUSREG_REGID_STACK1_RESERVED_REQUEST_STATUS,
        1,//sizeof(bTestDummyRequest),
        {setWriteOnlyField_HMMODBUS_sRegisterAddressTableFlags}, 
    },
    {
        MODBUSREG_REGID_STACK1_CONTROL_STATE,
        1,//sizeof(eControlState),
    },
    {
        MODBUSREG_REGID_STACK1_CONTROL_ERROR,
        1,//sizeof(eErrorState),
    },
    {
        MODBUSREG_REGID_STACK1_ERROR_STATE,
        1,//sizeof(eTestProcErrorState),
    },
    {
        MODBUSREG_REGID_STACK1_CLEAR_FAULTS,
        1,//sizeof(bClearFaultStatus),
        {setReadOnlyField_HMMODBUS_sRegisterAddressTableFlags}, 
    },
    {
        MODBUSREG_REGID_STACK1_CLEAR_FAULTS,
        1,//sizeof(bClearFaultRequest),
        {setWriteOnlyField_HMMODBUS_sRegisterAddressTableFlags}, 
    },
    {
        MODBUSREG_REGID_STACK1_ERROR_STATUS,
        1,//sizeof(bTestErrorStatus),
    },
    {
        MODBUSREG_REGID_STACK1_SPEED_REQUEST,
        2,//sizeof(sControlParameter.sStack[STACK_1].s16FieldSpeed_dHz),
    },
    {
        MODBUSREG_REGID_STACK1_VOLTAGE_DCLINK_REQUEST,
        2,//sizeof(s16VoltageReqDCLink_dV),
    },
    {
        MODBUSREG_REGID_STACK1_CURRENT_REQUEST,
        2,//sizeof(s16CurrentRMSReq1Ph_dA),
    },
    {
        MODBUSREG_REGID_STACK1_RUN_REQUEST_STATUS,
        1,//sizeof(bTestRunStatusFbk),
        {setReadOnlyField_HMMODBUS_sRegisterAddressTableFlags}, 
    },
    {
        MODBUSREG_REGID_STACK1_RUN_REQUEST_STATUS,
        1,//sizeof(bool),    /* eTestRunRequest enumeration here in dspic33 is 16-bit used only low 8 bits */
        {setWriteOnlyField_HMMODBUS_sRegisterAddressTableFlags}, 
    },
            
            
  
    /* 1010 : 1019 */
    /*
    MODBUSREG_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEA  ,
    MODBUSREG_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEB  ,                          
    MODBUSREG_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEC  ,
    MODBUSREG_REGID_STACK1_VOLTAGE_DCLINK_RMS  ,
    MODBUSREG_REGID_STACK1_CURRENT_MODULE_RMS_PHASEA  ,
    MODBUSREG_REGID_STACK1_CURRENT_MODULE_RMS_PHASEB  ,                          
    MODBUSREG_REGID_STACK1_CURRENT_MODULE_RMS_PHASEC  ,
    MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEA  ,  
    MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEB  ,   
    MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEC  ,  
    */
    
    {
        MODBUSREG_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEA,
        2,//sizeof(u16_irms_sns1),
    },
    {
        MODBUSREG_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEB,
        2,//sizeof(u16_irms_sns2),
    },
    {
        MODBUSREG_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEC,
        2,//sizeof(u16_irms_sns3),
    },
    {
        MODBUSREG_REGID_STACK1_VOLTAGE_DCLINK_RMS,
        2,//sizeof(VoltageStack1),
    },
    {
        MODBUSREG_REGID_STACK1_CURRENT_MODULE_RMS_PHASEA,
        2,//sizeof(u16_irms_mod1),
    },
    {
        MODBUSREG_REGID_STACK1_CURRENT_MODULE_RMS_PHASEB,
        2,//sizeof(u16_irms_mod2),
    },
    {
        MODBUSREG_REGID_STACK1_CURRENT_MODULE_RMS_PHASEC,
        2,//sizeof(u16_irms_mod3),
    },
    {
        MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEA,
        2,//sizeof(TempFbkStack1[0]),
    },
    {
        MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEB,
        2,//sizeof(TempFbkStack1[1]),
    },
    {
        MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEC,
        2,//sizeof(TempFbkStack1[2]),
    },


    
   /* 1020     
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
     */    
    
    {
        MODBUSREG_REGID_STACK1_CURRENT_SENSOR_PHASEA,
        2,//sizeof(CurrentStack1[0]),
    },
    {
        MODBUSREG_REGID_STACK1_CURRENT_SENSOR_PHASEB,
        2,//sizeof(CurrentStack1[1]),
    },
    {
        MODBUSREG_REGID_STACK1_CURRENT_SENSOR_PHASEC,
        2,//sizeof(CurrentStack1[2]),
    },
    {
        MODBUSREG_REGID_STACK1_VOLTAGE_DCLINK_FEEDBACK,
        2,//sizeof(VoltageStack1),
    },
    {
        MODBUSREG_REGID_STACK1_CURRENT_MODULE_PHASEA,
        2,//sizeof(CurrFbkStack1[0]),
    },
    {
        MODBUSREG_REGID_STACK1_CURRENT_MODULE_PHASEB,
        2,//sizeof(CurrFbkStack1[1]),
    },
    {
        MODBUSREG_REGID_STACK1_CURRENT_MODULE_PHASEC,
        2,//sizeof(CurrFbkStack1[2]),
    },
    {
        MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_PHASEA,
        2,//sizeof(TempFbkStack1[0]),
    },
    {
        MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_PHASEB,
        2,//sizeof(TempFbkStack1[1]),
    },
    {
        MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_PHASEC,
        2,//sizeof(TempFbkStack1[2]),
    },

    
    /* 1030 */
    
    /* 1040 */
    {
         MODBUSREG_REGID_STACK1_CFBK_MODULE_NOMINAL_CURRENT,
         2,//sizeof(s16CFKBNominalCurrentModule),
    },

    #endif  //USE_CONTROL

    /* 65534 */
    {
        MODBUSREG_REGID_MODBUS_RECEIVED_MESSAGES_COUNTER,
        2,//sizeof(u16ModbusCompleteReceiveMessageCounterThisDevice),
        {setRWDenySkipCSMON_HMMODBUS_sRegisterAddressTableFlags},
    },
  

};


HMMODBUS_sRegisterAddressTableInRam_t MODBUSREG_u32RegisterAddressTableInRam[MODBUSREG_REGISTER_TABLE_SIZE] =
{
};

#endif

HMMODBUS_sRegisterAccessTableInRam_t MODBUSREG_u32RegisterAccessTableInRam[MODBUSREG_REGISTER_TABLE_SIZE] =
{
};


const HMMODBUS_sRegisterAddressTable_t MODBUSREG_sInputRegisterAddressTable[MODBUSREG_INPUT_REGISTER_TABLE_SIZE] = 
{
    {
        MODBUSREG_INREGID_CONTROL_DUMMY,
        2,//sizeof(InputRegisterDummy),
    },
    
};

HMMODBUS_sRegisterAddressTableInRam_t MODBUSREG_sInputRegisterAddressTableInRam[MODBUSREG_INPUT_REGISTER_TABLE_SIZE] = 
{
};

HMMODBUS_sRegisterAccessTableInRam_t MODBUSREG_sInputRegisterAccessTableInRam[MODBUSREG_INPUT_REGISTER_TABLE_SIZE] =
{
};

    
    






const HMMODBUS_sCoilAddressTable_t HMMODBUS_u32CoilAddressTable[MODBUSREG_COIL_TABLE_SIZE] = 
{
    {
        MODBUSREG_COILID_CONTROL_TEST,
        0,  /* Bit position in CoilTest */       
    },
};

HMMODBUS_sCoilAddressTableInRam_t HMMODBUS_u32CoilAddressTableInRam[MODBUSREG_COIL_TABLE_SIZE] = 
{
 0
};

const HMMODBUS_sCoilAddressTable_t MODBUSREG_sInputCoilAddressTable[MODBUSREG_INPUT_COIL_TABLE_SIZE] = 
{
    {
        MODBUSREG_INCOILID_CONTROL_DUMMY,
        0,  /* Bit position in InputCoilDummy */       
    },
};

HMMODBUS_sCoilAddressTableInRam_t MODBUSREG_sInputCoilAddressTableInRam[MODBUSREG_INPUT_COIL_TABLE_SIZE] = 
{
 0
};






HMMODBUS_sFileTable_t MODBUSREG_asFileTable[MODBUSREG_FILE_TABLE_SIZE] = 
{
    0
};

HMMODBUS_sQueueTable_t MODBUSREG_asQueueTable[MODBUSREG_QUEUE_TABLE_SIZE] = 
{
    0
};










#define MODBUSREG_SERVER_ID_DATA_LENGTH     BOARDCFG_SERVERID_LENGTH
#define MODBUSREG_SERVER_ID_NUMBER          BOARDCFG_SERVERID_NUMBER

HMDRV_U8 MODBUSREG_au8ServerIDData[MODBUSREG_SERVER_ID_DATA_LENGTH] =
{
    BOARDCFG_VENDORID_BYTE0,
    BOARDCFG_VENDORID_BYTE1,
    BOARDCFG_VENDORID_BYTE2,
    BOARDCFG_VENDORID_BYTE3,
    BOARDCFG_PRODUCTID_BYTE0,
    BOARDCFG_PRODUCTID_BYTE1,
    BOARDCFG_PRODUCTID_BYTE2,
    BOARDCFG_PRODUCTID_BYTE3,
    BOARDCFG_SERIALNUM_BYTE0,
    BOARDCFG_SERIALNUM_BYTE1,
    BOARDCFG_SERIALNUM_BYTE2,
    BOARDCFG_SERIALNUM_BYTE3,
    BOARDCFG_SERIALNUM_BYTE4,
    BOARDCFG_SERIALNUM_BYTE5,
    BOARDCFG_SERIALNUM_BYTE6,
    BOARDCFG_SERIALNUM_BYTE7,
};

HMDRV_U8 MODBUSREG_au8DeviceIDVendorName[MODBUSREG_DEVICE_ID_VENDORNAME_LENGTH]     = { BOARDCFG_VENDOR_NAME  };
HMDRV_U8 MODBUSREG_au8DeviceIDProductCode[MODBUSREG_DEVICE_ID_PRODUCTCODE_LENGTH]   = { BOARDCFG_PRODUCT_CODE };
HMDRV_U8 MODBUSREG_au8DeviceIDMajorMinorRevision[MODBUSREG_DEVICE_ID_MAJORMINORREVISION_LENGTH]     = { BOARDCFG_MAJOR_MINOR_REVISION };
HMDRV_U8 MODBUSREG_au8DeviceIDVendorUrl[MODBUSREG_DEVICE_ID_VENDORURL_LENGTH]       = { BOARDCFG_VENDOR_URL };
HMDRV_U8 MODBUSREG_au8DeviceIDProductName[MODBUSREG_DEVICE_ID_PRODUCTNAME_LENGTH]   = { BOARDCFG_PRODUCT_NAME };
HMDRV_U8 MODBUSREG_au8DeviceIDModelName[MODBUSREG_DEVICE_ID_MODELNAME_LENGTH]       = { BOARDCFG_MODEL_NAME };
HMDRV_U8 MODBUSREG_au8DeviceIDUserApplicationName[MODBUSREG_DEVICE_ID_USERAPPLICATIONNAME_LENGTH]   = { BOARDCFG_USERAPPLICATION_NAME };
HMDRV_U8 MODBUSREG_au8DeviceIDUserReserve0[MODBUSREG_DEVICE_ID_USERRESERVE0_LENGTH] = { BOARDCFG_USERRESERVE0_NAME };
HMDRV_U8 MODBUSREG_au8DeviceIDUserReserve1[MODBUSREG_DEVICE_ID_USERRESERVE1_LENGTH] = { BOARDCFG_USERRESERVE1_NAME };
HMDRV_U8 MODBUSREG_au8DeviceIDUserReserve2[MODBUSREG_DEVICE_ID_USERRESERVE2_LENGTH] = { BOARDCFG_USERRESERVE2_NAME };




HMDRV_U8 QueueTestTable[16*12] = 
{
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 
    
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 

    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 
};






/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */


/* *****************************************************************************
 * Functions
 **************************************************************************** */
void MODBUSREG_vDataZero(uint_least8_t* pData, uint32_t u32DataCount )
{
    uint32_t indexLoop;

    for (indexLoop = 0; indexLoop < u32DataCount; indexLoop++)
    {
        *pData++ = 0;
    }
}



/* *****************************************************************************
 * MODBUSREG_AddressTablesInit
 *
 * Description: Modbus Data Holders Initialization
 **************************************************************************** */
void MODBUSREG_AddressTablesInit(void)
{
    uint16_t index;

    MODBUSREG_vDataZero((uint_least8_t*)&MODBUSREG_asFileTable, sizeof(MODBUSREG_asFileTable));

    #if MODBUSREG_HOLDREG_ADDR_TABLE_READ_ONLY
    MODBUSREG_vDataZero((uint_least8_t*)&MODBUSREG_u32RegisterAddressTableInRam, sizeof(MODBUSREG_u32RegisterAddressTableInRam));
    #endif
    MODBUSREG_vDataZero((uint_least8_t*)&MODBUSREG_u32RegisterAccessTableInRam, sizeof(MODBUSREG_u32RegisterAccessTableInRam));
    MODBUSREG_vDataZero((uint_least8_t*)&MODBUSREG_sInputRegisterAddressTableInRam, sizeof(MODBUSREG_sInputRegisterAddressTableInRam));
    MODBUSREG_vDataZero((uint_least8_t*)&MODBUSREG_sInputRegisterAccessTableInRam, sizeof(MODBUSREG_sInputRegisterAccessTableInRam));
    MODBUSREG_vDataZero((uint_least8_t*)&HMMODBUS_u32CoilAddressTableInRam, sizeof(HMMODBUS_u32CoilAddressTableInRam));
    MODBUSREG_vDataZero((uint_least8_t*)&MODBUSREG_sInputCoilAddressTableInRam, sizeof(MODBUSREG_sInputCoilAddressTableInRam));
    MODBUSREG_vDataZero((uint_least8_t*)&MODBUSREG_asQueueTable, sizeof(MODBUSREG_asQueueTable));





/* Coils */
    index = 0;
    HMMODBUS_u32CoilAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&CoilTest;
    index++;
    
/* Input Coils */
    index = 0;
    MODBUSREG_sInputCoilAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&InputCoilDummy;
    index++;

    
    
/* Holding Registers */
#if MODBUSREG_HOLDREG_ADDR_TABLE_READ_ONLY
    index = 0;
 
    
    #if USE_NO_MAIN == 0
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&sHexMonUartModuleConfiguration.u32BaudRate + 2;
    index++;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&sHexMonUartModuleConfiguration.u32BaudRate;
    index++;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&sHexMonUartModuleConfiguration.eParityAndData;
    index++;
    #endif  //USE_NO_MAIN == 0

    #if USE_CONTROL
    /* 1000 : 1009 */
    /*
    MODBUSREG_REGID_STACK1_RESERVED_REQUEST_STATUS  ,
    MODBUSREG_REGID_STACK1_CONTROL_STATE            ,
    MODBUSREG_REGID_STACK1_CONTROL_ERROR            ,                               
    MODBUSREG_REGID_STACK1_ERROR_STATE              , 
    MODBUSREG_REGID_STACK1_CLEAR_FAULTS             ,        
    MODBUSREG_REGID_STACK1_ERROR_STATUS             ,            
    MODBUSREG_REGID_STACK1_SPEED_REQUEST            ,
    MODBUSREG_REGID_STACK1_VOLTAGE_DCLINK_REQUEST   ,        
    MODBUSREG_REGID_STACK1_CURRENT_REQUEST          , 
    MODBUSREG_REGID_STACK1_RUN_REQUEST_STATUS       ,
    */
    HMMODBUS_u16RegisterAddressTableControlParametersStartIndex = index;
    
    #if BOARDCFG_USE_TESTPROC
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&bTestDummyStatus;
    #else
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&InputRegisterDummy;        /* NULL address must not be used */
    #endif
    index++;
    #if BOARDCFG_USE_TESTPROC
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&bTestDummyRequest;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32PointerToProcessFunction = (HMDRV_PTR_FUNC_SIZE)&TESTPROC_vTestDummyRequestChangeProcess;
    #else
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&InputRegisterDummy;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32PointerToProcessFunction = NULL;
    #endif
    index++;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&eControlState;
    index++;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&eErrorState;
    index++;
    #if BOARDCFG_USE_TESTPROC
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&eTestProcErrorState;
    #else
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&InputRegisterDummy;
    #endif
    index++;
    #if BOARDCFG_USE_TESTPROC
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&bClearFaultStatus;
    #else
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&InputRegisterDummy;
    #endif
    index++;
    #if BOARDCFG_USE_TESTPROC
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&bClearFaultRequest;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32PointerToProcessFunction = (HMDRV_PTR_FUNC_SIZE)&TESTPROC_vClearFaultRequestChangeProcess;
    #else
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&InputRegisterDummy;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32PointerToProcessFunction = NULL;
    #endif
    index++;
    #if BOARDCFG_USE_TESTPROC
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&bTestErrorStatus;
    #else
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&InputRegisterDummy;
    #endif
    index++;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)CONTROL_u32GetAddrFieldFrequencyStack1_dHz();
    index++;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)CONTROL_u32GetAddrVoltageRequestDCLink_dV();
    index++;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&s16CurrentRMSReq1Ph_dA;
    index++;
    #if BOARDCFG_USE_TESTPROC
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&bTestRunStatusFbk;
    #else
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&InputRegisterDummy;
    #endif
    index++;
    #if BOARDCFG_USE_TESTPROC
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&eTestRunRequest;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32PointerToProcessFunction = (HMDRV_PTR_FUNC_SIZE)&TESTPROC_vTestRunRequestChangeProcess;
    #else
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&InputRegisterDummy;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32PointerToProcessFunction = NULL;
    #endif
    index++;
    
    /* 1010 : 1019 */
    /*
    MODBUSREG_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEA  ,
    MODBUSREG_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEB  ,                          
    MODBUSREG_REGID_STACK1_CURRENT_SENSOR_RMS_PHASEC  ,
    MODBUSREG_REGID_STACK1_VOLTAGE_DCLINK_RMS  ,
    MODBUSREG_REGID_STACK1_CURRENT_MODULE_RMS_PHASEA  ,
    MODBUSREG_REGID_STACK1_CURRENT_MODULE_RMS_PHASEB  ,                          
    MODBUSREG_REGID_STACK1_CURRENT_MODULE_RMS_PHASEC  ,
    MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEA  ,  
    MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEB  ,   
    MODBUSREG_REGID_STACK1_TEMPERATURE_MODULE_FILTERED_PHASEC  ,  
    */
    
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&u16_irms_sns1;
    index++;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&u16_irms_sns2;
    index++;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&u16_irms_sns3;
    index++;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&VoltageStack1;
    index++;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&u16_irms_mod1;
    index++;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&u16_irms_mod2;
    index++;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&u16_irms_mod3;
    index++;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&TempFbkStack1[0];
    index++;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&TempFbkStack1[1];
    index++;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&TempFbkStack1[2];
    index++;
    
   /* 1020     
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
     */    
    
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&CurrentStack1[0];
    index++;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&CurrentStack1[1];
    index++;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&CurrentStack1[2];
    index++;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&VoltageStack1;
    index++;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&CurrFbkStack1[0];
    index++;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&CurrFbkStack1[1];
    index++;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&CurrFbkStack1[2];
    index++;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&TempFbkStack1[0];
    index++;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&TempFbkStack1[1];
    index++;
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&TempFbkStack1[2];
    index++;
    
    /* 1030 */
    
    /* 1040 */
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&s16CFKBNominalCurrentModule;
    index++;
    
    #endif  //USE_CONTROL
    
    
    MODBUSREG_u32RegisterAddressTableInRam[index].u32RealAddress = (HMDRV_ADDR_SIZE)&u16ModbusCompleteReceiveMessageCounterThisDevice;
    index++;
    
    
    
    #if USE_SPI_FLIR
    HMMODBUS_u32RegisterAddressTable[index].u16VirtAddress = MODBUSREG_REGID_FLIR_VOSPI_STATE_REQ;
    HMMODBUS_u32RegisterAddressTable[index].u32RealAddress = (HMDRV_ADDR_SIZE)&eVoSPIStateReq;
    HMMODBUS_u32RegisterAddressTable[index].u16Size = 1;//sizeof(eVoSPIState_t);
    index++;
    
    HMMODBUS_u32RegisterAddressTable[index].u16VirtAddress = MODBUSREG_REGID_FLIR_VOSPI_PACKET_LENGTH;
    HMMODBUS_u32RegisterAddressTable[index].u32RealAddress = (HMDRV_ADDR_SIZE)&VoSPIPacketLength;
    HMMODBUS_u32RegisterAddressTable[index].u16Size = 2;//sizeof(uint16_t);
    index++;
    
    HMMODBUS_u32RegisterAddressTable[index].u16VirtAddress = MODBUSREG_REGID_FLIR_RESET;
    HMMODBUS_u32RegisterAddressTable[index].u32RealAddress = (HMDRV_ADDR_SIZE)&eFlirReset;
    HMMODBUS_u32RegisterAddressTable[index].u16Size = 2;//sizeof(eFlirReset_t);
    index++;
    
    HMMODBUS_u32RegisterAddressTable[index].u16VirtAddress = MODBUSREG_REGID_FLIR_SWITCH_PING_PONG;
    HMMODBUS_u32RegisterAddressTable[index].u32RealAddress = (HMDRV_ADDR_SIZE)&FlirImageSwitchPingPong;
    HMMODBUS_u32RegisterAddressTable[index].u16Size = 2;//sizeof(uint16_t);
    index++;
    
    HMMODBUS_u32RegisterAddressTable[index].u16VirtAddress = MODBUSREG_REGID_FLIR_VOSPI_PACKET_ZIP_MODE;
    HMMODBUS_u32RegisterAddressTable[index].u32RealAddress = (HMDRV_ADDR_SIZE)&eVoSPIPacketZIP;
    HMMODBUS_u32RegisterAddressTable[index].u16Size = 1;//sizeof(eVoSPIPacketZIP_t);
    index++;
    
    HMMODBUS_u32RegisterAddressTable[index].u16VirtAddress = MODBUSREG_REGID_FLIR_VOSPI_PACKET_PING_PONG_LEN;
    HMMODBUS_u32RegisterAddressTable[index].u32RealAddress = (HMDRV_ADDR_SIZE)&VoSPIPacketModbusLengthPingPong;
    HMMODBUS_u32RegisterAddressTable[index].u16Size = 2;//sizeof(uint16_t);
    index++;
    #endif  //USE_SPI_FLIR
#endif
    
    
    
    /* Input Registers */
    index = 0;
    MODBUSREG_sInputRegisterAddressTableInRam[index].u32RealAddress = (HMDRV_U32)(HMDRV_ADDR_SIZE)&InputRegisterDummy;
    index++;
    
    
    
    
    /* Files */
    /* to do: !!! extract file table index from u16VirtAddress !!! */
    index = 0;
    
    #if USE_SPI_FLIR
    /* Record used for command -> Commands Supported 0x0000..0x0FFF Bit 14 will be internally set for RAD (0x0EXX) and OEM (0x08XX) Commands */
    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_FLIR_I2C;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)EXT_MEM_FLIR_CCI_TWI_VIRTUAL_ADDRESS;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = MODBUSREG_FILE_RECORD_SIZE_FLIR_I2C;
    MODBUSREG_asFileTable[index].u16RecordCount = MODBUSREG_FILE_RECORD_COUNT_FLIR_I2C; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount;
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    index++;
    
    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_IRCAMERA_DEF;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&FlirCameraData;   
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = MODBUSREG_FILE_RECORD_SIZE_IRCAMERA_DEF;
    MODBUSREG_asFileTable[index].u16RecordCount = MODBUSREG_FILE_RECORD_COUNT_IRCAMERA_DEF_MAX; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    index++;
    
    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_IRCAMERA_ZIP_DEF;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&FlirCameraData + (IR_CAMERA_BYTES_PER_DOT_RAW14 * IR_CAMERA_DOTS_PER_LINE_COUNT * IR_CAMERA_LINES_COUNT);   
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = MODBUSREG_FILE_RECORD_SIZE_IRCAMERA_ZIP_DEF;
    MODBUSREG_asFileTable[index].u16RecordCount = MODBUSREG_FILE_RECORD_COUNT_IRCAMERA_ZIP_DEF_MAX; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    index++;
    
    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_IRCAMERA_ZIP;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&FlirCameraData;   
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = MODBUSREG_FILE_RECORD_SIZE_IRCAMERA_ZIP;
    MODBUSREG_asFileTable[index].u16RecordCount = MODBUSREG_FILE_RECORD_COUNT_IRCAMERA_ZIP; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    index++;
    
    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_IRCAMERA_RAW;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&FlirCameraData;   
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = MODBUSREG_FILE_RECORD_SIZE_IRCAMERA_RAW;
    MODBUSREG_asFileTable[index].u16RecordCount = MODBUSREG_FILE_RECORD_COUNT_IRCAMERA_RAW; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    index++;
    
    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_IRCAMERA_RGB;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&FlirCameraData;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = MODBUSREG_FILE_RECORD_SIZE_IRCAMERA_RGB;
    MODBUSREG_asFileTable[index].u16RecordCount = MODBUSREG_FILE_RECORD_COUNT_IRCAMERA_RGB; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    index++;
    
    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_IRCAM_01_ZIP;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&FlirCameraData;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = MODBUSREG_FILE_RECORD_SIZE_IRCAMERA_ZIP;
    MODBUSREG_asFileTable[index].u16RecordCount = MODBUSREG_FILE_RECORD_COUNT_IRCAMERA_ZIP; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    index++;
    
    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_IRCAM_01_RAW;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&FlirCameraData;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = MODBUSREG_FILE_RECORD_SIZE_IRCAMERA_RAW;
    MODBUSREG_asFileTable[index].u16RecordCount = MODBUSREG_FILE_RECORD_COUNT_IRCAMERA_RAW; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    index++;
    
    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_IRCAM_01_RGB;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&FlirCameraData;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = MODBUSREG_FILE_RECORD_SIZE_IRCAMERA_RGB;
    MODBUSREG_asFileTable[index].u16RecordCount = MODBUSREG_FILE_RECORD_COUNT_IRCAMERA_RGB; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    index++;
    
    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_IRCAM_02_ZIP;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&FlirCameraData2;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = MODBUSREG_FILE_RECORD_SIZE_IRCAMERA_ZIP;
    MODBUSREG_asFileTable[index].u16RecordCount = MODBUSREG_FILE_RECORD_COUNT_IRCAMERA_ZIP; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    index++;
    
    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_IRCAM_02_RAW;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&FlirCameraData2;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = MODBUSREG_FILE_RECORD_SIZE_IRCAMERA_RAW;
    MODBUSREG_asFileTable[index].u16RecordCount = MODBUSREG_FILE_RECORD_COUNT_IRCAMERA_RAW; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    index++;
    
    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_IRCAM_02_RGB;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&FlirCameraData2;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = MODBUSREG_FILE_RECORD_SIZE_IRCAMERA_RGB;
    MODBUSREG_asFileTable[index].u16RecordCount = MODBUSREG_FILE_RECORD_COUNT_IRCAMERA_RGB; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    index++;
    
    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_IRCAM_03_ZIP;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&FlirCameraData3;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = MODBUSREG_FILE_RECORD_SIZE_IRCAMERA_ZIP;
    MODBUSREG_asFileTable[index].u16RecordCount = MODBUSREG_FILE_RECORD_COUNT_IRCAMERA_ZIP; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    index++;
    
    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_IRCAM_03_RAW;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&FlirCameraData3;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = MODBUSREG_FILE_RECORD_SIZE_IRCAMERA_RAW;
    MODBUSREG_asFileTable[index].u16RecordCount = MODBUSREG_FILE_RECORD_COUNT_IRCAMERA_RAW; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    index++;
    
    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_IRCAM_03_RGB;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&FlirCameraData3;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = MODBUSREG_FILE_RECORD_SIZE_IRCAMERA_RGB;
    MODBUSREG_asFileTable[index].u16RecordCount = MODBUSREG_FILE_RECORD_COUNT_IRCAMERA_RGB; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    index++;
    #endif
    
    
    
    #if USE_CSMON 

    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_CSMON_AUTORIZATION;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&CSMON_sFileAutorization;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = 0;
    MODBUSREG_asFileTable[index].u16RecordToRecordOffsetSizeBytes = 0;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&CSMON_au16FileAutorization_RecordOffsets;
    MODBUSREG_asFileTable[index].u16RecordCount = CSMON_FILE_AUTORIZATION_RECORD_COUNT; 
    MODBUSREG_asFileTable[index].u32FileSizeBytes = 0;  /* No File size Additional Limitation */
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount; 
    if (MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable == NULL)
    {
        MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    }
    else
    {
        MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = 0xFF;
    }
    MODBUSREG_asFileTable[index].u32PointerToFileRecordStringDataTable = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&CSMON_abFileAutorization_RecordStringData;
    MODBUSREG_asFileTable[index].u32PointerToRecordIndexGenerationFunction = NULL;
    MODBUSREG_asFileTable[index].bStringDataExpected = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterWriteCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterReadCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterWriteCommandTable = MODBUSREG_u32FileAutorization_FunctionAfterWriteFileRecordTable;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterReadCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordReadOnlyTable = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&CSMON_abFileAutorization_RecordReadOnly;
    MODBUSREG_asFileTable[index].bReadOnlyFile = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsCountSize = 0;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRealAddressTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsFromDiffLocationsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsFromTableCount = 0;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsCountInPartTable = NULL;
    MODBUSREG_asFileTable[index].bTrunkBiggerLengthRequest = 0;
    MODBUSREG_asFileTable[index].bReadAccess = HMDRV_TRUE;
    MODBUSREG_asFileTable[index].bWriteAccess = HMDRV_TRUE;

    index++;    
    


    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_CSMON_PROTOCOL_VERSION;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)(HMDRV_ADDR_SIZE)(HMDRV_U8*)&CSMON_sFileProtocolVersion;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = CSMON_FILE_PROTOCOL_VERSION_RECORD_SIZE;
    MODBUSREG_asFileTable[index].u16RecordToRecordOffsetSizeBytes = CSMON_FILE_PROTOCOL_VERSION_RECORD_SIZE;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable = NULL;
    MODBUSREG_asFileTable[index].u16RecordCount = CSMON_FILE_PROTOCOL_VERSION_RECORD_COUNT; 
    MODBUSREG_asFileTable[index].u32FileSizeBytes = 0;  /* No File size Additional Limitation */
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount; 
    if (MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable == NULL)
    {
        MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    }
    else
    {
        MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = 0xFF;
    }
    MODBUSREG_asFileTable[index].u32PointerToFileRecordStringDataTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToRecordIndexGenerationFunction = NULL;
    MODBUSREG_asFileTable[index].bStringDataExpected = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterWriteCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterReadCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterWriteCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterReadCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordReadOnlyTable = NULL;
    MODBUSREG_asFileTable[index].bReadOnlyFile = HMDRV_TRUE;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsCountSize = 0;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRealAddressTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsFromDiffLocationsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsFromTableCount = 0;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsCountInPartTable = NULL;
    MODBUSREG_asFileTable[index].bTrunkBiggerLengthRequest = 1;
    MODBUSREG_asFileTable[index].bReadAccess = HMDRV_TRUE;
    MODBUSREG_asFileTable[index].bWriteAccess = HMDRV_TRUE;
    index++;    
    


    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_CSMON_PARAMETER_COUNT;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&CSMON_sFileParameterCount;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = CSMON_FILE_PARAMETER_COUNT_RECORD_SIZE;
    MODBUSREG_asFileTable[index].u16RecordToRecordOffsetSizeBytes = CSMON_FILE_PARAMETER_COUNT_RECORD_SIZE;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable = NULL;
    MODBUSREG_asFileTable[index].u16RecordCount = CSMON_FILE_PARAMETER_COUNT_RECORD_COUNT; 
    MODBUSREG_asFileTable[index].u32FileSizeBytes = 0;  /* No File size Additional Limitation */
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount; 
    if (MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable == NULL)
    {
        MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    }
    else
    {
        MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = 0xFF;
    }
    MODBUSREG_asFileTable[index].u32PointerToFileRecordStringDataTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToRecordIndexGenerationFunction = NULL;
    MODBUSREG_asFileTable[index].bStringDataExpected = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterWriteCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterReadCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionBeforeReadCommand = MODBUSREG_u32FileParamCount_FunctionBeforeReadFileRecord;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterWriteCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterReadCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordReadOnlyTable = NULL;
    MODBUSREG_asFileTable[index].bReadOnlyFile = HMDRV_TRUE;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsSizeTable = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&CSMON_au8FileParameterCountElementsSizeTable;
    MODBUSREG_asFileTable[index].u8RecordElementsCountSize = CSMON_sFileParameterCount_t_ElementsCount;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRealAddressTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsFromDiffLocationsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsFromTableCount = 0;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsCountInPartTable = NULL;
    MODBUSREG_asFileTable[index].bTrunkBiggerLengthRequest = 1;
    MODBUSREG_asFileTable[index].bReadAccess = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].bWriteAccess = HMDRV_FALSE;
    index++;    



    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_CSMON_PARAMETER_LIST;
    MODBUSREG_asFileTable[index].u32RealAddress = MODBUSREG_u32ParamListAddress;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = CSMON_FILE_PARAMETER_LIST_RECORD_SIZE;
    MODBUSREG_asFileTable[index].u16RecordToRecordOffsetSizeBytes = CSMON_FILE_PARAMETER_LIST_RECORD_SIZE;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable = NULL;
    MODBUSREG_asFileTable[index].u16RecordCount = MODBUSREG_u16ParamCount;      /* bad behavior when no parameters */
    MODBUSREG_asFileTable[index].u32FileSizeBytes = 0;  /* No File size Additional Limitation */
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount; 
    if (MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable == NULL)
    {
        MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    }
    else
    {
        MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = 0xFF;
    }
    MODBUSREG_asFileTable[index].u32PointerToFileRecordStringDataTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToRecordIndexGenerationFunction = (HMDRV_U32)(HMDRV_PTR_FUNC_SIZE)(CSMON_pfIndexGeneration)CSMON_u16ParametersAccessListIndexGenerate;
    MODBUSREG_asFileTable[index].bStringDataExpected = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterWriteCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterReadCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterWriteCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterReadCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordReadOnlyTable = NULL;
    MODBUSREG_asFileTable[index].bReadOnlyFile = HMDRV_TRUE;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsSizeTable = NULL;
    //MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsSizeTable = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&CSMON_au8FileParameterListElementsSizeTable;
    MODBUSREG_asFileTable[index].u8RecordElementsCountSize = CSMON_sFileParameterDefinitions_t_ElementsCount;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRealAddressTable = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&CSMON_au32FileParameterDefinitionsFileRecordElementsRealAddressTable;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsFromDiffLocationsSizeTable = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&CSMON_au32FileParameterDefinitionsFileRecordElementsSizeTable;
    MODBUSREG_asFileTable[index].u8RecordElementsFromTableCount = CSMON_sFileParameterDefinitions_t_ElementsPartsCount;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&CSMON_au32FileParameterDefinitionsFileRecordElementsRecordToRecordOffsetSizeBytesTable;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsCountInPartTable = (HMDRV_U32)(__CONSTANT_MEMORY_MODIFIER__ HMDRV_U8*)&CSMON_au8FileParameterListElementsCountInPartTable;
    MODBUSREG_asFileTable[index].bTrunkBiggerLengthRequest = 0;
    MODBUSREG_asFileTable[index].bReadAccess = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].bWriteAccess = HMDRV_FALSE;
    index++;  
    


    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_CSMON_PARAMETER_SHORT_NAME;
    MODBUSREG_asFileTable[index].u32RealAddress = MODBUSREG_u32ParamNameAddress;

    MODBUSREG_asFileTable[index].u16RecordSizeBytes = CSMON_FILE_PARAMETER_SHORT_NAME_RECORD_SIZE;
    MODBUSREG_asFileTable[index].u16RecordToRecordOffsetSizeBytes = CSMON_FILE_PARAMETER_SHORT_NAME_RECORD_TO_RECORD_OFFSET;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable = NULL;
    MODBUSREG_asFileTable[index].u16RecordCount = MODBUSREG_u16ParamCount;      /* bad behavior when no parameters */
    MODBUSREG_asFileTable[index].u32FileSizeBytes = 0;  /* No File size Additional Limitation */
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount; 
    if (MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable == NULL)
    {
        MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    }
    else
    {
        MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = 0xFF;
    }
    MODBUSREG_asFileTable[index].u32PointerToFileRecordStringDataTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToRecordIndexGenerationFunction = (HMDRV_U32)(HMDRV_PTR_FUNC_SIZE)(CSMON_pfIndexGeneration)CSMON_u16ParametersAccessListIndexGenerate;
    MODBUSREG_asFileTable[index].bStringDataExpected = HMDRV_TRUE;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterWriteCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterReadCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterWriteCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterReadCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordReadOnlyTable = NULL;
    MODBUSREG_asFileTable[index].bReadOnlyFile = HMDRV_TRUE;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsSizeTable = NULL;
#if BOARDCFG_SHORT_NAME_CODED_IN_HIGH_BYTE_TI
    MODBUSREG_asFileTable[index].u8RecordElementsCountSize = 1;
#else
    MODBUSREG_asFileTable[index].u8RecordElementsCountSize = 0;
#endif
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRealAddressTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsFromDiffLocationsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsFromTableCount = 0;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsCountInPartTable = NULL;
    MODBUSREG_asFileTable[index].bTrunkBiggerLengthRequest = 1;
    MODBUSREG_asFileTable[index].bReadAccess = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].bWriteAccess = HMDRV_FALSE;
    index++;    
    


    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_CSMON_IMPLEMENTED_FUNCTION_CODES;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)(__CONSTANT_MEMORY_MODIFIER__ HMDRV_U8*)&CSMON_au8FileImplementedFunctionCodes; /* const modifier does not work - must be CSMON_CONSTANT_MEMORY_MODIFIER */
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = CSMON_FILE_IMPLEMENTED_FUNCTION_CODES_RECORD_SIZE;
    MODBUSREG_asFileTable[index].u16RecordToRecordOffsetSizeBytes = CSMON_FILE_IMPLEMENTED_FUNCTION_CODES_RECORD_SIZE;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable = NULL;
    MODBUSREG_asFileTable[index].u16RecordCount = CSMON_FILE_IMPLEMENTED_FUNCTION_CODES_RECORD_COUNT; 
    MODBUSREG_asFileTable[index].u32FileSizeBytes = 0;  /* No File size Additional Limitation */
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount; 
    if (MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable == NULL)
    {
        MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    }
    else
    {
        MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = 0xFF;
    }
    MODBUSREG_asFileTable[index].u32PointerToFileRecordStringDataTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToRecordIndexGenerationFunction = NULL;
    MODBUSREG_asFileTable[index].bStringDataExpected = HMDRV_TRUE;                                                 /* either u8RecordElementsCountSize = 1 or bStringDataExpected = HMDRV_TRUE */
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterWriteCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterReadCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterWriteCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterReadCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordReadOnlyTable = NULL;
    MODBUSREG_asFileTable[index].bReadOnlyFile = HMDRV_TRUE;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsCountSize = 0;                                                     /* either u8RecordElementsCountSize = 1 or bStringDataExpected = HMDRV_TRUE (if used this size of implemented functions must be explicitly defined) */
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRealAddressTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsFromDiffLocationsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsFromTableCount = 0;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsCountInPartTable = NULL;
    MODBUSREG_asFileTable[index].bTrunkBiggerLengthRequest = 1;
    MODBUSREG_asFileTable[index].bReadAccess = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].bWriteAccess = HMDRV_FALSE;
    index++;    
    
    

    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_CSMON_RTC;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&CSMON_sDateTime;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = CSMON_FILE_RTC_RECORD_SIZE;
    MODBUSREG_asFileTable[index].u16RecordToRecordOffsetSizeBytes = CSMON_FILE_RTC_RECORD_SIZE;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable = NULL;
    MODBUSREG_asFileTable[index].u16RecordCount = CSMON_FILE_RTC_RECORD_COUNT;
    MODBUSREG_asFileTable[index].u32FileSizeBytes = 0;  /* No File size Additional Limitation */
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1;
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount;
    if (MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable == NULL)
    {
        MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    }
    else
    {
        MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = 0xFF;
    }
    MODBUSREG_asFileTable[index].u32PointerToFileRecordStringDataTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToRecordIndexGenerationFunction = NULL;
    MODBUSREG_asFileTable[index].bStringDataExpected = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterWriteCommand = MODBUSREG_u32FileRTC_FunctionAfterWriteFileRecord;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterReadCommand = MODBUSREG_u32FileRTC_FunctionAfterReadFileRecord;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterWriteCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterReadCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordReadOnlyTable = NULL;
    MODBUSREG_asFileTable[index].bReadOnlyFile = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsSizeTable = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&CSMON_au8FileRTCElementsSizeTable;
    MODBUSREG_asFileTable[index].u8RecordElementsCountSize = CSMON_sDateTime_t_ElementsCount;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRealAddressTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsFromDiffLocationsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsFromTableCount = 0;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsCountInPartTable = NULL;
    MODBUSREG_asFileTable[index].bTrunkBiggerLengthRequest = 1;
    MODBUSREG_asFileTable[index].bReadAccess = HMDRV_TRUE;
    MODBUSREG_asFileTable[index].bWriteAccess = HMDRV_FALSE;
    index++;



    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_CSMON_PARAMETER_DESCRIPTION_NUMERIC;
    MODBUSREG_asFileTable[index].u32RealAddress = MODBUSREG_u32ParamNumbAddress;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = CSMON_FILE_PARAMETER_NUMERIC_RECORD_SIZE;
    MODBUSREG_asFileTable[index].u16RecordToRecordOffsetSizeBytes = CSMON_FILE_PARAMETER_NUMERIC_RECORD_SIZE;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable = NULL;
    MODBUSREG_asFileTable[index].u16RecordCount = MODBUSREG_u16ParamCount;      /* bad behavior when no parameters */
    MODBUSREG_asFileTable[index].u32FileSizeBytes = 0;  /* No File size Additional Limitation */
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1;
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount;
    if (MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable == NULL)
    {
        MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    }
    else
    {
        MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = 0xFF;
    }
    MODBUSREG_asFileTable[index].u32PointerToFileRecordStringDataTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToRecordIndexGenerationFunction = (HMDRV_U32)(HMDRV_PTR_FUNC_SIZE)(CSMON_pfIndexGeneration)CSMON_u16ParametersAccessListIndexGenerate;
    MODBUSREG_asFileTable[index].bStringDataExpected = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterWriteCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterReadCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterWriteCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterReadCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordReadOnlyTable = NULL;
    MODBUSREG_asFileTable[index].bReadOnlyFile = HMDRV_TRUE;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsSizeTable = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&CSMON_au8FileParameterNumericElementsSizeTable;
    MODBUSREG_asFileTable[index].u8RecordElementsCountSize = CSMON_uFileParameterNumeric_t_ElementsCount;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRealAddressTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsFromDiffLocationsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsFromTableCount = 0;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsCountInPartTable = NULL;
    MODBUSREG_asFileTable[index].bTrunkBiggerLengthRequest = 0;
    MODBUSREG_asFileTable[index].bReadAccess = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].bWriteAccess = HMDRV_FALSE;
    index++;


    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_CSMON_PARAMETER_DESCRIPTION_STRING;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)MODBUSREG_u32ParamStringAddress;
    #if 0 /* (CSMON_FILE_PARAMETER_STRING_BYTES_SIZE > (uint32_t)(244 * 10000)) */
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = 0;                    /* Variable Record Size Depends On Requested read and write bytes count */
    MODBUSREG_asFileTable[index].u16RecordToRecordOffsetSizeBytes = 0;      /* additional offset multiplier 1 register. (additional multiplier = u16RecordToRecordOffsetSizeBytes+1 in registers) */
    MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable = CSMON_FILE_PARAMETER_STRING_BYTES_SIZE;  /* u32PointerToFileRecordOffsetTable used as File bytes Size if multiplied offset used */
    MODBUSREG_asFileTable[index].u16RecordCount = 0;                        /* Two message request with Multiplier Offset Used (u16RecordToRecordOffsetSizeBytes is additional offset multiplier) */
    MODBUSREG_asFileTable[index].u32FileSizeBytes = CSMON_FILE_PARAMETER_STRING_BYTES_SIZE;  /* File size Limitation */
    #else
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = 0;                    /* Variable Record Size Depends On Requested read and write bytes count */
    MODBUSREG_asFileTable[index].u16RecordToRecordOffsetSizeBytes = 0;      /* Variable Record Size Depends On Requested read and write bytes count */
    MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable = NULL;
    MODBUSREG_asFileTable[index].u16RecordCount = MODBUSREG_FILE_MAX_RECORD_COUNT;  /* Use Max Record Count if Variable Record Size Used */
    MODBUSREG_asFileTable[index].u32FileSizeBytes = CSMON_FILE_PARAMETER_STRING_BYTES_SIZE;  /* File size Limitation */
    #endif
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1;
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = 0x7FFF; /* more than 10000 to disable */

    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = 0xFF;

    MODBUSREG_asFileTable[index].u32PointerToFileRecordStringDataTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToRecordIndexGenerationFunction = NULL;
    MODBUSREG_asFileTable[index].bStringDataExpected = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterWriteCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterReadCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterWriteCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterReadCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordReadOnlyTable = NULL;
    MODBUSREG_asFileTable[index].bReadOnlyFile = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsCountSize = 2;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRealAddressTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsFromDiffLocationsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsFromTableCount = 0;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsCountInPartTable = NULL;
    MODBUSREG_asFileTable[index].bTrunkBiggerLengthRequest = 1;
    MODBUSREG_asFileTable[index].bReadAccess = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].bWriteAccess = HMDRV_FALSE;
    index++;



    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_CSMON_PARAMETER_DESCRIPTION_AUXULIARY;
    MODBUSREG_asFileTable[index].u32RealAddress = MODBUSREG_u32ParamAuxAddress;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = 0;                    /* Variable Record Size Depends On Requested read and write bytes count */
    MODBUSREG_asFileTable[index].u16RecordToRecordOffsetSizeBytes = 0;      /* Variable Record Size Depends On Requested read and write bytes count */
    MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable = NULL;
    MODBUSREG_asFileTable[index].u16RecordCount = MODBUSREG_FILE_MAX_RECORD_COUNT;  /* Use Max Record Count if Variable Record Size Used */
    MODBUSREG_asFileTable[index].u32FileSizeBytes = 0;  /* No File size Additional Limitation */
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1;
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = 0x7FFF; /* more than 10000 to disable */

    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = 0xFF;

    MODBUSREG_asFileTable[index].u32PointerToFileRecordStringDataTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToRecordIndexGenerationFunction = NULL;
    MODBUSREG_asFileTable[index].bStringDataExpected = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterWriteCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterReadCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterWriteCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterReadCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordReadOnlyTable = NULL;
    MODBUSREG_asFileTable[index].bReadOnlyFile = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsCountSize = 2;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRealAddressTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsFromDiffLocationsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsFromTableCount = 0;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsCountInPartTable = NULL;
    MODBUSREG_asFileTable[index].bTrunkBiggerLengthRequest = 0;
    MODBUSREG_asFileTable[index].bReadAccess = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].bWriteAccess = HMDRV_FALSE;
    index++;


    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_CSMON_PARAMETER_ACCESS;
    MODBUSREG_asFileTable[index].u32RealAddress = NULL;

    MODBUSREG_asFileTable[index].u16RecordSizeBytes = 0;
    MODBUSREG_asFileTable[index].u16RecordToRecordOffsetSizeBytes = 0;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable = NULL;
    MODBUSREG_asFileTable[index].u16RecordCount = MODBUSREG_u16ParamCount;      /* bad behavior when no parameters */
    MODBUSREG_asFileTable[index].u32FileSizeBytes = 0;  /* No File size Additional Limitation */
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1;
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_FILE_MAX_RECORD_COUNT;
    if (MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable == NULL)
    {
        MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    }
    else
    {
        MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = 0xFF;
    }
    MODBUSREG_asFileTable[index].u32PointerToFileRecordStringDataTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToRecordIndexGenerationFunction = NULL;
    MODBUSREG_asFileTable[index].bStringDataExpected = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterWriteCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterReadCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterWriteCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterReadCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordReadOnlyTable = NULL;
    MODBUSREG_asFileTable[index].bReadOnlyFile = HMDRV_FALSE;

    MODBUSREG_asFileTable[index].u32PointerToFileRecordRealAddressTable = NULL;       /* u32RealAddress data for each record */
    //MODBUSREG_asFileTable[index].u32PointerToFileRecordSizeBytesTable = ;        /* u16RecordSizeBytes data for each record */
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsCountSize = 0;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRealAddressTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsFromDiffLocationsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsFromTableCount = 0;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsCountInPartTable = NULL;
    MODBUSREG_asFileTable[index].bTrunkBiggerLengthRequest = 1;
    MODBUSREG_asFileTable[index].bReadAccess = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].bWriteAccess = HMDRV_FALSE;
    index++;











    /* Recorder And Scope Below */
    
    #if HMDRV_USE_RECORDER_PARAMETERS
    HMDRV_U8 recorderIndex;
    HMDRV_ADDR pSampleRecorder;
    
    /* Recorder Configuration Below */
    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_CSMON_PARAMETER_RECORDER_CONFIGURATION;
    MODBUSREG_asFileTable[index].u32RealAddress = MODBUSREG_u32ParamRecorderConfigAddress; //was (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)sConfigRecorder
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = (uint16_t)(sizeof(HMRECPRM_sConfig_t) * HMDRV_ADDR_WIDTH);
    MODBUSREG_asFileTable[index].u16RecordToRecordOffsetSizeBytes = (uint16_t)(sizeof(HMRECPRM_sConfig_t) * HMDRV_ADDR_WIDTH);
    MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable = NULL;
    MODBUSREG_asFileTable[index].u16RecordCount = HMDRV_RECORDER_PARAM_HANDLE_COUNT; 
    MODBUSREG_asFileTable[index].u32FileSizeBytes = 0;  /* No File size Additional Limitation */
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount; 
    if (MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable == NULL)
    {
        MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    }
    else
    {
        MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = 0xFF;
    }
    MODBUSREG_asFileTable[index].u32PointerToFileRecordStringDataTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToRecordIndexGenerationFunction = NULL;
    MODBUSREG_asFileTable[index].bStringDataExpected = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterWriteCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterReadCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterWriteCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterReadCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordReadOnlyTable = NULL;
    MODBUSREG_asFileTable[index].bReadOnlyFile = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsSizeTable = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&CSMON_au8FileRecorderConfigElementsSizeTable;
    MODBUSREG_asFileTable[index].u8RecordElementsCountSize = HMRECPRM_sConfig_t_ElementsCount;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRealAddressTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsFromDiffLocationsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsFromTableCount = 0;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsCountInPartTable = NULL;
    MODBUSREG_asFileTable[index].bTrunkBiggerLengthRequest = 1;
    MODBUSREG_asFileTable[index].bReadAccess = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].bWriteAccess = HMDRV_FALSE;
    index++;  
    
    
    
    /* Recorder Status Below */

    HMDRV_FILE_PNTR_MODIFIER HMRECPRM_sStatus_t* sStatusRecorder;
    recorderIndex = 0;
    sStatusRecorder = HMRECPRM_GetStatusRecParamAddress(recorderIndex);


    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_CSMON_PARAMETER_RECORDER_STATUS;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)sStatusRecorder;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = (uint16_t)(sizeof(HMRECPRM_sStatus_t) * HMDRV_ADDR_WIDTH);
    MODBUSREG_asFileTable[index].u16RecordToRecordOffsetSizeBytes = (uint16_t)(sizeof(HMRECPRM_sStatus_t) * HMDRV_ADDR_WIDTH);
    MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable = NULL;
    MODBUSREG_asFileTable[index].u16RecordCount = HMDRV_RECORDER_PARAM_HANDLE_COUNT;
    MODBUSREG_asFileTable[index].u32FileSizeBytes = 0;  /* No File size Additional Limitation */
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1;
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount;
    if (MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable == NULL)
    {
        MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    }
    else
    {
        MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = 0xFF;
    }
    MODBUSREG_asFileTable[index].u32PointerToFileRecordStringDataTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToRecordIndexGenerationFunction = NULL;
    MODBUSREG_asFileTable[index].bStringDataExpected = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterWriteCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterReadCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterWriteCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterReadCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordReadOnlyTable = NULL;
    MODBUSREG_asFileTable[index].bReadOnlyFile = HMDRV_TRUE;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsSizeTable = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&CSMON_au8FileRecorderStatusElementsSizeTable;
    MODBUSREG_asFileTable[index].u8RecordElementsCountSize = HMRECPRM_sStatus_t_ElementsCount;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRealAddressTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsFromDiffLocationsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsFromTableCount = 0;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsCountInPartTable = NULL;
    MODBUSREG_asFileTable[index].bTrunkBiggerLengthRequest = 1;
    MODBUSREG_asFileTable[index].bReadAccess = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].bWriteAccess = HMDRV_FALSE;
    index++;


    

    /* Recorder Buffers Below */
    recorderIndex = 0;
    
    pSampleRecorder = HMRECPRM_GetBufferRecParamPtrAddress(recorderIndex++);
    
    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_CSMON_PARAMETER_RECORDER0_DATA_SAMPLES;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)pSampleRecorder;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = 0;                    /* Variable Record Size Depends On Requested read and write bytes count */
    MODBUSREG_asFileTable[index].u16RecordToRecordOffsetSizeBytes = 0;      /* Variable Record Size Depends On Requested read and write bytes count */
    MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable = NULL;
    MODBUSREG_asFileTable[index].u16RecordCount = MODBUSREG_FILE_MAX_RECORD_COUNT;  /* Use Max Record Count if Variable Record Size Used */
    MODBUSREG_asFileTable[index].u32FileSizeBytes = HMDRV_RECORDER_PARAM_BUFFER0_SIZE;  /* File size Limitation */
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = 0x7FFF; /* more than 10000 to disable */
    
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = 0xFF;
    
    MODBUSREG_asFileTable[index].u32PointerToFileRecordStringDataTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToRecordIndexGenerationFunction = NULL;
    MODBUSREG_asFileTable[index].bStringDataExpected = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterWriteCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterReadCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterWriteCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterReadCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordReadOnlyTable = NULL;
    MODBUSREG_asFileTable[index].bReadOnlyFile = HMDRV_TRUE;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsCountSize = 2;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRealAddressTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsFromDiffLocationsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsFromTableCount = 0;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsCountInPartTable = NULL;
    MODBUSREG_asFileTable[index].bTrunkBiggerLengthRequest = 0;
    MODBUSREG_asFileTable[index].bReadAccess = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].bWriteAccess = HMDRV_FALSE;
    index++;  
    
    
    pSampleRecorder = HMRECPRM_GetBufferRecParamPtrAddress(recorderIndex++);
    
    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_CSMON_PARAMETER_RECORDER1_DATA_SAMPLES;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)pSampleRecorder;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = 0;                    /* Variable Record Size Depends On Requested read and write bytes count */
    MODBUSREG_asFileTable[index].u16RecordToRecordOffsetSizeBytes = 0;      /* Variable Record Size Depends On Requested read and write bytes count */
    MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable = NULL;
    MODBUSREG_asFileTable[index].u16RecordCount = MODBUSREG_FILE_MAX_RECORD_COUNT;  /* Use Max Record Count if Variable Record Size Used */
    MODBUSREG_asFileTable[index].u32FileSizeBytes = HMDRV_RECORDER_PARAM_BUFFER1_SIZE;  /* File size Limitation */
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = 0x7FFF; 
    
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = 0xFF;
    
    MODBUSREG_asFileTable[index].u32PointerToFileRecordStringDataTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToRecordIndexGenerationFunction = NULL;
    MODBUSREG_asFileTable[index].bStringDataExpected = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterWriteCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterReadCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterWriteCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterReadCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordReadOnlyTable = NULL;
    MODBUSREG_asFileTable[index].bReadOnlyFile = HMDRV_TRUE;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsCountSize = 2;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRealAddressTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsFromDiffLocationsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsFromTableCount = 0;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsCountInPartTable = NULL;
    MODBUSREG_asFileTable[index].bTrunkBiggerLengthRequest = 0;
    MODBUSREG_asFileTable[index].bReadAccess = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].bWriteAccess = HMDRV_FALSE;
    index++;  
    
    

    pSampleRecorder = HMRECPRM_GetBufferRecParamPtrAddress(recorderIndex++);
    
    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_CSMON_PARAMETER_RECORDER2_DATA_SAMPLES;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)pSampleRecorder;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = 0;                    /* Variable Record Size Depends On Requested read and write bytes count */
    MODBUSREG_asFileTable[index].u16RecordToRecordOffsetSizeBytes = 0;      /* Variable Record Size Depends On Requested read and write bytes count */
    MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable = NULL;
    MODBUSREG_asFileTable[index].u16RecordCount = MODBUSREG_FILE_MAX_RECORD_COUNT;  /* Use Max Record Count if Variable Record Size Used */
    MODBUSREG_asFileTable[index].u32FileSizeBytes = HMDRV_RECORDER_PARAM_BUFFER2_SIZE;  /* File size Limitation */
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = 0x7FFF; 
    
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = 0xFF;
    
    MODBUSREG_asFileTable[index].u32PointerToFileRecordStringDataTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToRecordIndexGenerationFunction = NULL;
    MODBUSREG_asFileTable[index].bStringDataExpected = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterWriteCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterReadCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterWriteCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterReadCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordReadOnlyTable = NULL;
    MODBUSREG_asFileTable[index].bReadOnlyFile = HMDRV_TRUE;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsCountSize = 2;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRealAddressTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsFromDiffLocationsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsFromTableCount = 0;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsCountInPartTable = NULL;
    MODBUSREG_asFileTable[index].bTrunkBiggerLengthRequest = 0;
    MODBUSREG_asFileTable[index].bReadAccess = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].bWriteAccess = HMDRV_FALSE;
    index++;  
    
    #endif


    #if HMDRV_USE_SCOPE_PARAMETERS
    HMDRV_U8 scopeIndex;
    HMDRV_ADDR pSampleScope;
    
    /* Scope Configuration Below */
    //HMDRV_FILE_PNTR_MODIFIER HMSCOPEPRM_sConfig_t* sConfigScope;
    //scopeIndex = 0;
    //sConfigScope = HMSCOPEPRM_GetConfigScopeParamAddress(scopeIndex);

    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_CSMON_PARAMETER_SCOPE_CONFIGURATION;
    MODBUSREG_asFileTable[index].u32RealAddress = MODBUSREG_u32ParamScopeConfigAddress;//was (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)sConfigScope;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = (uint16_t)(sizeof(HMSCOPEPRM_sConfig_t) * HMDRV_ADDR_WIDTH);
    MODBUSREG_asFileTable[index].u16RecordToRecordOffsetSizeBytes = (uint16_t)(sizeof(HMSCOPEPRM_sConfig_t) * HMDRV_ADDR_WIDTH);
    MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable = NULL;
    MODBUSREG_asFileTable[index].u16RecordCount = HMDRV_SCOPE_PARAM_HANDLE_COUNT;
    MODBUSREG_asFileTable[index].u32FileSizeBytes = 0;  /* No File size Additional Limitation */
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1;
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount;
    if (MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable == NULL)
    {
        MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    }
    else
    {
        MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = 0xFF;
    }
    MODBUSREG_asFileTable[index].u32PointerToFileRecordStringDataTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToRecordIndexGenerationFunction = NULL;
    MODBUSREG_asFileTable[index].bStringDataExpected = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterWriteCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterReadCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterWriteCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterReadCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordReadOnlyTable = NULL;
    MODBUSREG_asFileTable[index].bReadOnlyFile = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsSizeTable = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&CSMON_au8FileScopeConfigurationElementsSizeTable;
    MODBUSREG_asFileTable[index].u8RecordElementsCountSize = CSMON_sFileScopeConfiguration_t_ElementsCount;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRealAddressTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsFromDiffLocationsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsFromTableCount = 0;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsCountInPartTable = NULL;
    MODBUSREG_asFileTable[index].bTrunkBiggerLengthRequest = 1;
    MODBUSREG_asFileTable[index].bReadAccess = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].bWriteAccess = HMDRV_FALSE;
    index++;


    /* Scope Status Below */

    HMDRV_FILE_PNTR_MODIFIER HMSCOPEPRM_sStatusModeFullSize_t* sStatusScope;
    scopeIndex = 0;
    sStatusScope = HMSCOPEPRM_GetStatusScopeParamAddress(scopeIndex);

    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_CSMON_PARAMETER_SCOPE_STATUS;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)sStatusScope;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = (uint16_t)(sizeof(HMSCOPEPRM_sStatusModeFullSize_t) * HMDRV_ADDR_WIDTH);
    MODBUSREG_asFileTable[index].u16RecordToRecordOffsetSizeBytes = (uint16_t)(sizeof(HMSCOPEPRM_sStatusModeFullSize_t) * HMDRV_ADDR_WIDTH);
    MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable = NULL;
    MODBUSREG_asFileTable[index].u16RecordCount = HMDRV_SCOPE_PARAM_HANDLE_COUNT;
    MODBUSREG_asFileTable[index].u32FileSizeBytes = 0;  /* No File size Additional Limitation */
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1;
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = MODBUSREG_asFileTable[index].u16RecordCount;
    if (MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable == NULL)
    {
        MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = MODBUSREG_asFileTable[index].u16RecordSizeBytes;
    }
    else
    {
        MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = 0xFF;
    }
    MODBUSREG_asFileTable[index].u32PointerToFileRecordStringDataTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToRecordIndexGenerationFunction = NULL;
    MODBUSREG_asFileTable[index].bStringDataExpected = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterWriteCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterReadCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterWriteCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterReadCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordReadOnlyTable = NULL;
    MODBUSREG_asFileTable[index].bReadOnlyFile = HMDRV_TRUE;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsSizeTable = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&CSMON_au8FileScopeStatusElementsSizeTable;
    MODBUSREG_asFileTable[index].u8RecordElementsCountSize = CSMON_sFileScopeStatus_t_ElementsCount;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRealAddressTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsFromDiffLocationsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsFromTableCount = 0;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsCountInPartTable = NULL;
    MODBUSREG_asFileTable[index].bTrunkBiggerLengthRequest = 1;
    MODBUSREG_asFileTable[index].bReadAccess = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].bWriteAccess = HMDRV_FALSE;
    index++;





    /* Scope Buffers Below */
    scopeIndex = 0;

    pSampleScope = HMSCOPEPRM_GetBufferScopeParamAddress(scopeIndex++);
    
    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_CSMON_PARAMETER_SCOPE0_DATA_SAMPLES;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)(HMDRV_ADDR_SIZE)pSampleScope;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = HMDRV_HMSCOPEPRM_BUFFER_SIZE;
    MODBUSREG_asFileTable[index].u16RecordToRecordOffsetSizeBytes = 0;      /* Variable Record Size Depends On Requested read and write bytes count */
    MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable = NULL;
    MODBUSREG_asFileTable[index].u16RecordCount = 1;
    MODBUSREG_asFileTable[index].u32FileSizeBytes = 0;  /* No File size Additional Limitation */
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = 0x7FFF; 
    
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = 0xFF;
    
    MODBUSREG_asFileTable[index].u32PointerToFileRecordStringDataTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToRecordIndexGenerationFunction = NULL;
    MODBUSREG_asFileTable[index].bStringDataExpected = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterWriteCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterReadCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterWriteCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterReadCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordReadOnlyTable = NULL;
    MODBUSREG_asFileTable[index].bReadOnlyFile = HMDRV_TRUE;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsCountSize = 2;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRealAddressTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsFromDiffLocationsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsFromTableCount = 0;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsCountInPartTable = NULL;
    MODBUSREG_asFileTable[index].bTrunkBiggerLengthRequest = 1;
    MODBUSREG_asFileTable[index].bReadAccess = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].bWriteAccess = HMDRV_FALSE;
    index++;  
    
    
    pSampleScope = HMSCOPEPRM_GetBufferScopeParamAddress(scopeIndex++);
    
    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_CSMON_PARAMETER_SCOPE1_DATA_SAMPLES;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)(HMDRV_ADDR_SIZE)pSampleScope;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = HMDRV_HMSCOPEPRM_BUFFER_SIZE;
    MODBUSREG_asFileTable[index].u16RecordToRecordOffsetSizeBytes = 0;      /* Variable Record Size Depends On Requested read and write bytes count */
    MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable = NULL;
    MODBUSREG_asFileTable[index].u16RecordCount = 1;
    MODBUSREG_asFileTable[index].u32FileSizeBytes = 0;  /* No File size Additional Limitation */
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = 0x7FFF; 
    
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = 0xFF;
    
    MODBUSREG_asFileTable[index].u32PointerToFileRecordStringDataTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToRecordIndexGenerationFunction = NULL;
    MODBUSREG_asFileTable[index].bStringDataExpected = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterWriteCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterReadCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterWriteCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterReadCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordReadOnlyTable = NULL;
    MODBUSREG_asFileTable[index].bReadOnlyFile = HMDRV_TRUE;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsCountSize = 2;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRealAddressTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsFromDiffLocationsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsFromTableCount = 0;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsCountInPartTable = NULL;
    MODBUSREG_asFileTable[index].bTrunkBiggerLengthRequest = 1;
    MODBUSREG_asFileTable[index].bReadAccess = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].bWriteAccess = HMDRV_FALSE;
    index++;  
    
    pSampleScope = HMSCOPEPRM_GetBufferScopeParamAddress(scopeIndex++);
    
    MODBUSREG_asFileTable[index].u16VirtAddress = MODBUSREG_FILE_CSMON_PARAMETER_SCOPE2_DATA_SAMPLES;
    MODBUSREG_asFileTable[index].u32RealAddress = (HMDRV_U32)(HMDRV_ADDR_SIZE)pSampleScope;
    MODBUSREG_asFileTable[index].u16RecordSizeBytes = HMDRV_HMSCOPEPRM_BUFFER_SIZE;
    MODBUSREG_asFileTable[index].u16RecordToRecordOffsetSizeBytes = 0;      /* Variable Record Size Depends On Requested read and write bytes count */
    MODBUSREG_asFileTable[index].u32PointerToFileRecordOffsetTable = NULL;
    MODBUSREG_asFileTable[index].u16RecordCount = 1;
    MODBUSREG_asFileTable[index].u32FileSizeBytes = 0;  /* No File size Additional Limitation */
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u1UseValidDataStructure = 1; 
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u15RecordCountValid = 0x7FFF; 
    
    MODBUSREG_asFileTable[index].uFileValidData.sFileValidData.u8RecordBytesValid = 0xFF;
    
    MODBUSREG_asFileTable[index].u32PointerToFileRecordStringDataTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToRecordIndexGenerationFunction = NULL;
    MODBUSREG_asFileTable[index].bStringDataExpected = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterWriteCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToProcessFunctionAfterReadCommand = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterWriteCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordProcessFunctionAfterReadCommandTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordReadOnlyTable = NULL;
    MODBUSREG_asFileTable[index].bReadOnlyFile = HMDRV_TRUE;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsCountSize = 2;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRealAddressTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsFromDiffLocationsSizeTable = NULL;
    MODBUSREG_asFileTable[index].u8RecordElementsFromTableCount = 0;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsRecordToRecordOffsetSizeBytesTable = NULL;
    MODBUSREG_asFileTable[index].u32PointerToFileRecordElementsCountInPartTable = NULL;
    MODBUSREG_asFileTable[index].bTrunkBiggerLengthRequest = 1;
    MODBUSREG_asFileTable[index].bReadAccess = HMDRV_FALSE;
    MODBUSREG_asFileTable[index].bWriteAccess = HMDRV_FALSE;
    index++;  
    #endif
    
    
    
    
    
    #endif
    
    
    /* Queues */
    /* to do: !!! extract queue table index from u16VirtAddress !!! */
    /* to do: !!! update Queue Size and Count When Recorder Buffer Setup Changes !!! */
    index = 0;
    
    MODBUSREG_asQueueTable[index].u16VirtAddress        = MODBUSREG_QUEUE_TEST_TABLE;
    MODBUSREG_asQueueTable[index].u32RealAddress        = (HMDRV_U32)(HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8*)&QueueTestTable;
    MODBUSREG_asQueueTable[index].u16RecordSizeBytes    = 16;
                                                          /* fix size of queue if needed */
    MODBUSREG_asQueueTable[index].u16RecordCount        = sizeof(QueueTestTable) / MODBUSREG_asQueueTable[index].u16RecordSizeBytes;    

    #if HMDRV_ADDR_WIDTH == 2
        MODBUSREG_asQueueTable[index].u16RecordCount <<= 1;
    #endif

    MODBUSREG_asQueueTable[index].u32RecordCountValid   = 0;    
    index++;
    MODBUSREG_asQueueTable[index].u16VirtAddress        = MODBUSREG_QUEUE_RECORDER0;
    MODBUSREG_asQueueTable[index].u32RealAddress        = (HMDRV_U32)HMLIB_GetRecBuffAddress(MODBUSREG_QUEUE_RECORDER0);
    MODBUSREG_asQueueTable[index].u16RecordSizeBytes    = HMLIB_GetRecSampleSize(MODBUSREG_QUEUE_RECORDER0);
    MODBUSREG_asQueueTable[index].u16RecordCount        = HMLIB_GetRecBufferSize(MODBUSREG_QUEUE_RECORDER0) / MODBUSREG_asQueueTable[index].u16RecordSizeBytes;
    MODBUSREG_asQueueTable[index].u32RecordCountValid   = 0;    
    index++;
    MODBUSREG_asQueueTable[index].u16VirtAddress        = MODBUSREG_QUEUE_RECORDER1;
    MODBUSREG_asQueueTable[index].u32RealAddress        = (HMDRV_U32)HMLIB_GetRecBuffAddress(MODBUSREG_QUEUE_RECORDER1);
    MODBUSREG_asQueueTable[index].u16RecordSizeBytes    = HMLIB_GetRecSampleSize(MODBUSREG_QUEUE_RECORDER1);
    MODBUSREG_asQueueTable[index].u16RecordCount        = HMLIB_GetRecBufferSize(MODBUSREG_QUEUE_RECORDER1) / MODBUSREG_asQueueTable[index].u16RecordSizeBytes;
    MODBUSREG_asQueueTable[index].u32RecordCountValid   = 0;    
    index++;
    MODBUSREG_asQueueTable[index].u16VirtAddress        = MODBUSREG_QUEUE_RECORDER2;
    MODBUSREG_asQueueTable[index].u32RealAddress        = (HMDRV_U32)HMLIB_GetRecBuffAddress(MODBUSREG_QUEUE_RECORDER2);
    MODBUSREG_asQueueTable[index].u16RecordSizeBytes    = HMLIB_GetRecSampleSize(MODBUSREG_QUEUE_RECORDER2);
    MODBUSREG_asQueueTable[index].u16RecordCount        = HMLIB_GetRecBufferSize(MODBUSREG_QUEUE_RECORDER2) / MODBUSREG_asQueueTable[index].u16RecordSizeBytes;
    MODBUSREG_asQueueTable[index].u32RecordCountValid   = 0;    
    index++;

}





/* *****************************************************************************
 * MODBUSREG_vInit
 *
 * Description: Iinit
 **************************************************************************** */
void MODBUSREG_vInit(void)
{

#if MODBUSREG_HOLDREG_ADDR_TABLE_READ_ONLY
    MODBUSREG_u16HoldRegUsedCount           = MODBUSREG_REGISTER_TABLE_SIZE;
    MODBUSREG_u16HoldRegListCount           = MODBUSREG_REGISTER_TABLE_SIZE;
    //MODBUSREG_u16HoldRegNameCount = MODBUSREG_REGISTER_TABLE_SIZE;
                                                        /* why short in TSC ??? */
    MODBUSREG_pu32RegisterAddressTable      = (uint32_t)(__CONSTANT_MEMORY_MODIFIER__ HMMODBUS_sRegisterAddressTable_t*)&MODBUSREG_au32RegisterAddressTable;
    MODBUSREG_pu32RegisterAddressTableInRam      = (uint32_t)(HMDRV_ADDR_SIZE)( HMMODBUS_sRegisterAddressTableInRam_t*)&MODBUSREG_u32RegisterAddressTableInRam;
    MODBUSREG_u16HoldRegFillCount           = MODBUSREG_REGISTER_TABLE_SIZE;
#else
    MODBUSREG_u16HoldRegUsedCount           = MODBUSREG_REGISTER_TABLE_SIZE;

    MODBUSREG_u16HoldRegListCount           = (uint16_t)(MODBUSREG_HOLDREG_LIST_RW_SIZE / (sizeof(HMMODBUS_sRegisterAddressTable_t) * HMDRV_ADDR_WIDTH));
    //MODBUSREG_u16HoldRegNameCount = (uint16_t)(MODBUSREG_HOLDREG_NAME_RW_SIZE / (sizeof(HMMODBUS_sRegisterValuesTable_t)   * HMDRV_ADDR_WIDTH));

    /* Use The Min of three */
    MODBUSREG_u16HoldRegUsedCount           = MCLIB_u16Min(MODBUSREG_u16HoldRegUsedCount, MODBUSREG_u16HoldRegListCount);
    //MODBUSREG_u16HoldRegUsedCount = MCLIB_u16Min(MODBUSREG_u16HoldRegUsedCount, MODBUSREG_u16HoldRegNameCount);

#if _CSMON_IN_INTERNAL_RAM
    MODBUSREG_pu32RegisterAddressTable      = (uint32_t)MODBUSREG_pu32RegisterAddressTableRealocatable;           //DPN Table in the Internal RAM
    MODBUSREG_pu32RegisterAddressTableInRam      = (uint32_t)MODBUSREG_pu32RegisterAddressTableInRamRealocatable; //DPN Table in the Internal RAM
#else
    MODBUSREG_pu32RegisterAddressTable      = MODBUSREG_HOLDREG_LIST_RW_ADDR;
    MODBUSREG_pu32RegisterAddressTableInRam      = MODBUSREG_HOLDREG_ADDR_RW_ADDR;
#endif

    //MODBUSREG_psHoldingRegisterValuesTable  = (HMMODBUS_sRegisterValuesTable_t*)MODBUSREG_HOLDREG_NAME_RW_ADDR;

    MODBUSREG_u16HoldRegFillCount           = MODBUSREG_u16ModbusParamCount;    /* from aux table */
#endif





    MODBUSREG_AddressTablesInit();
    
    HMMODBUS_vSetupRegisterDummy();

    MODBUSREG_vInitHoldingRegistersTableChanges();

    HMMODBUS_vSetupInputRegistersAddressTable((HMDRV_U32)(HMDRV_CNST_PNTR_MODIFIER HMDRV_U8*)&MODBUSREG_sInputRegisterAddressTable[0], MODBUSREG_INPUT_REGISTER_TABLE_SIZE);
    HMMODBUS_vSetupInputRegistersAddressTableInRam((HMMODBUS_sRegisterAddressTableInRam_t*)&MODBUSREG_sInputRegisterAddressTableInRam[0], (HMMODBUS_sRegisterAccessTableInRam_t*)&MODBUSREG_sInputRegisterAccessTableInRam[0], MODBUSREG_INPUT_REGISTER_TABLE_SIZE);

    HMMODBUS_vSetupCoilDummy();

    HMMODBUS_vSetupHoldingCoilsAddressTable((HMDRV_U32)(HMDRV_CNST_PNTR_MODIFIER HMDRV_U8*)&HMMODBUS_u32CoilAddressTable[0], MODBUSREG_COIL_TABLE_SIZE);
    HMMODBUS_vSetupHoldingCoilsAddressTableInRam((HMMODBUS_sCoilAddressTableInRam_t*)&HMMODBUS_u32CoilAddressTableInRam[0], MODBUSREG_COIL_TABLE_SIZE);

    HMMODBUS_vSetupInputCoilsAddressTable((HMDRV_U32)(HMDRV_CNST_PNTR_MODIFIER HMDRV_U8*)&MODBUSREG_sInputCoilAddressTable[0], MODBUSREG_INPUT_COIL_TABLE_SIZE);
    HMMODBUS_vSetupInputCoilsAddressTableInRam((HMMODBUS_sCoilAddressTableInRam_t*)&MODBUSREG_sInputCoilAddressTableInRam[0], MODBUSREG_INPUT_COIL_TABLE_SIZE);

    
    
    HMMODBUS_vFileTable((HMMODBUS_sFileTable_t*)&MODBUSREG_asFileTable[0], MODBUSREG_FILE_TABLE_SIZE);
    HMMODBUS_vSetupQueueRegistersAddressTable((HMMODBUS_sQueueTable_t*)&MODBUSREG_asQueueTable[0], MODBUSREG_QUEUE_TABLE_SIZE);
    
    /* Test Queue */
    HMMODBUS_vSetupActiveQueueIndex(MODBUSREG_QUEUE_TEST_TABLE);
    MODBUSREG_asQueueTable[MODBUSREG_QUEUE_TEST_TABLE].u32RecordCountValid   = MODBUSREG_asQueueTable[MODBUSREG_QUEUE_TEST_TABLE].u16RecordCount;

    /* Server ID */
    HMMODBUS_vSetServerIDDataAddress ((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(&MODBUSREG_au8ServerIDData));
    HMMODBUS_vSetServerIDLength (MODBUSREG_SERVER_ID_DATA_LENGTH);
    HMMODBUS_vSetServerIDNumber (MODBUSREG_SERVER_ID_NUMBER);
    HMMODBUS_vSetServerOnStatus (0x00);


    HMMODBUS_vSetDeviceIDVendorName ((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(&MODBUSREG_au8DeviceIDVendorName));
    HMMODBUS_vSetDeviceIDVendorNameLength (MODBUSREG_DEVICE_ID_VENDORNAME_LENGTH);
    HMMODBUS_vSetDeviceIDProductCode ((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(&MODBUSREG_au8DeviceIDProductCode));
    HMMODBUS_vSetDeviceIDProductCodeLength (MODBUSREG_DEVICE_ID_PRODUCTCODE_LENGTH);
    HMMODBUS_vSetDeviceIDMajorMinorRevision ((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(&MODBUSREG_au8DeviceIDMajorMinorRevision));
    HMMODBUS_vSetDeviceIDMajorMinorRevisionLength (MODBUSREG_DEVICE_ID_MAJORMINORREVISION_LENGTH);
    HMMODBUS_vSetDeviceIDVendorUrl ((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(&MODBUSREG_au8DeviceIDVendorUrl));
    HMMODBUS_vSetDeviceIDVendorUrlLength (MODBUSREG_DEVICE_ID_VENDORURL_LENGTH);
    HMMODBUS_vSetDeviceIDProductName ((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(&MODBUSREG_au8DeviceIDProductName));
    HMMODBUS_vSetDeviceIDProductNameLength (MODBUSREG_DEVICE_ID_PRODUCTNAME_LENGTH);
    HMMODBUS_vSetDeviceIDModelName ((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(&MODBUSREG_au8DeviceIDModelName));
    HMMODBUS_vSetDeviceIDModelNameLength (MODBUSREG_DEVICE_ID_MODELNAME_LENGTH);
    HMMODBUS_vSetDeviceIDUserApplicationName ((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(&MODBUSREG_au8DeviceIDUserApplicationName));
    HMMODBUS_vSetDeviceIDUserApplicationNameLength (MODBUSREG_DEVICE_ID_USERAPPLICATIONNAME_LENGTH);
    HMMODBUS_vSetDeviceIDUserReserve0 ((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(&MODBUSREG_au8DeviceIDUserReserve0));
    HMMODBUS_vSetDeviceIDUserReserve0Length (MODBUSREG_DEVICE_ID_USERRESERVE0_LENGTH);
    HMMODBUS_vSetDeviceIDUserReserve1 ((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(&MODBUSREG_au8DeviceIDUserReserve1));
    HMMODBUS_vSetDeviceIDUserReserve1Length (MODBUSREG_DEVICE_ID_USERRESERVE1_LENGTH);
    HMMODBUS_vSetDeviceIDUserReserve2 ((HMDRV_ADDR)(HMDRV_ADDR_SIZE)(&MODBUSREG_au8DeviceIDUserReserve2));
    HMMODBUS_vSetDeviceIDUserReserve2Length (MODBUSREG_DEVICE_ID_USERRESERVE2_LENGTH);


}


/* *****************************************************************************
 * MODBUSREG_vInitTableChanges
 *
 * Description: Execute on Holding Registers Table Change
 **************************************************************************** */
void MODBUSREG_vInitHoldingRegistersTableChanges(void)
{
    HMMODBUS_vSetupHoldingRegistersAddressTable((HMDRV_U32)MODBUSREG_pu32RegisterAddressTable, MODBUSREG_u16HoldRegFillCount);
    HMMODBUS_vSetupHoldingRegistersAddressTableInRam((HMMODBUS_sRegisterAddressTableInRam_t*)(HMDRV_ADDR_SIZE)MODBUSREG_pu32RegisterAddressTableInRam, (HMMODBUS_sRegisterAccessTableInRam_t*)&MODBUSREG_u32RegisterAccessTableInRam[0], MODBUSREG_u16HoldRegFillCount);
}



/* *****************************************************************************
 * MODBUSREG_vProcess
 *
 * Description: Process
 **************************************************************************** */
void MODBUSREG_vProcess(void)
{
}


#if USE_SPI_FLIR
/* *****************************************************************************
 * Flir Camera Functions
 *
 * Description: 
 **************************************************************************** */

HMDRV_U32 MODBUSREG_u32GetFlirCameraDataAddressZIP(void)
{
    return (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)(&FlirCameraData) + (IR_CAMERA_BYTES_PER_DOT_RAW14 * IR_CAMERA_DOTS_PER_LINE_COUNT * IR_CAMERA_LINES_COUNT);
}
HMDRV_U32 MODBUSREG_u32GetFlirCameraDataAddressZIPPing(void)
{
    return (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)(&FlirCameraData) + (IR_CAMERA_BYTES_PER_DOT_RAW14 * IR_CAMERA_DOTS_PER_LINE_COUNT * IR_CAMERA_LINES_COUNT);
}
HMDRV_U32 MODBUSREG_u32GetFlirCameraDataAddressZIPPong(void)
{
    return (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)(&FlirCameraData2) + (IR_CAMERA_BYTES_PER_DOT_RAW14 * IR_CAMERA_DOTS_PER_LINE_COUNT * IR_CAMERA_LINES_COUNT);
}

void MODBUSREG_vSetFlirCameraDataAddressZIPPingPong(HMDRV_U32 nAddress)
{
    MODBUSREG_asFileTable[MODBUSREG_FILE_IRCAMERA_ZIP_DEF].u32RealAddress = nAddress;   
}

void MODBUSREG_vSetFlirCameraDataRecordSizeZIPPingPong(HMDRV_U16 nSize)
{
     MODBUSREG_asFileTable[MODBUSREG_FILE_IRCAMERA_ZIP_DEF].u16RecordSizeBytes = nSize;   
}

void MODBUSREG_vSetFlirCameraDataAddressZIPPing(void)
{
    MODBUSREG_asFileTable[MODBUSREG_FILE_IRCAMERA_ZIP_DEF].u32RealAddress = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&FlirCameraData + (IR_CAMERA_BYTES_PER_DOT_RAW14 * IR_CAMERA_DOTS_PER_LINE_COUNT * IR_CAMERA_LINES_COUNT);   
}

void MODBUSREG_vSetFlirCameraDataAddressZIPPong(void)
{
    MODBUSREG_asFileTable[MODBUSREG_FILE_IRCAMERA_ZIP_DEF].u32RealAddress = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&FlirCameraData2 + (IR_CAMERA_BYTES_PER_DOT_RAW14 * IR_CAMERA_DOTS_PER_LINE_COUNT * IR_CAMERA_LINES_COUNT);   
}






HMDRV_U32 MODBUSREG_u32GetFlirCameraDataAddress(void)
{
    return (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)(&FlirCameraData);
}
HMDRV_U32 MODBUSREG_u32GetFlirCameraDataAddressPing(void)
{
    return (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)(&FlirCameraData);
}
HMDRV_U32 MODBUSREG_u32GetFlirCameraDataAddressPong(void)
{
    return (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)(&FlirCameraData2);
}
HMDRV_U32 MODBUSREG_u32GetFlirCameraDataAddress3(void)
{
    return (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)(&FlirCameraData3);
}

void MODBUSREG_vSetFlirCameraDataAddressPingPong(HMDRV_U32 nAddress)
{
    MODBUSREG_asFileTable[MODBUSREG_FILE_IRCAMERA_DEF].u32RealAddress = nAddress;   
}

void MODBUSREG_vSetFlirCameraDataRecordSizePingPong(HMDRV_U16 nSize)
{
     MODBUSREG_asFileTable[MODBUSREG_FILE_IRCAMERA_DEF].u16RecordSizeBytes = nSize;   
}

void MODBUSREG_vSetFlirCameraDataAddressPing(void)
{
    MODBUSREG_asFileTable[MODBUSREG_FILE_IRCAMERA_DEF].u32RealAddress = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&FlirCameraData;   
}

void MODBUSREG_vSetFlirCameraDataAddressPong(void)
{
    MODBUSREG_asFileTable[MODBUSREG_FILE_IRCAMERA_DEF].u32RealAddress = (HMDRV_U32)(HMDRV_FILE_PNTR_MODIFIER HMDRV_U8*)&FlirCameraData2;   
}
#endif  //USE_SPI_FLIR



void MODBUSREG_vSetParamCount(HMDRV_U16 u16Input)
{
    MODBUSREG_u16ParamCount = u16Input;
}

void MODBUSREG_vSetModbusParamCount(HMDRV_U16 u16Input)
{
    MODBUSREG_u16ModbusParamCount = u16Input;
}

void MODBUSREG_vSetParamRecorderConfigAddress(HMDRV_U32 u32Input)
{
    MODBUSREG_u32ParamRecorderConfigAddress = u32Input;
}

void MODBUSREG_vSetParamScopeConfigAddress(HMDRV_U32 u32Input)
{
    MODBUSREG_u32ParamScopeConfigAddress = u32Input;
}

void MODBUSREG_vSetParamListAddress(HMDRV_U32 u32Input)
{
    MODBUSREG_u32ParamListAddress = u32Input;
}

void MODBUSREG_vSetParamNameAddress(HMDRV_U32 u32Input)
{
    MODBUSREG_u32ParamNameAddress = u32Input;
}

void MODBUSREG_vSetParamNumbAddress(HMDRV_U32 u32Input)
{
    MODBUSREG_u32ParamNumbAddress = u32Input;
}

void MODBUSREG_vSetParamStringAddress(HMDRV_U32 u32Input)
{
    MODBUSREG_u32ParamStringAddress = u32Input;
}

void MODBUSREG_vSetParamAuxAddress(HMDRV_U32 u32Input)
{
    MODBUSREG_u32ParamAuxAddress = u32Input;
}

void MODBUSREG_vSetParamReservedAddress(HMDRV_U32 u32Input)
{
    MODBUSREG_u32ParamReservedAddress = u32Input;
}

void MODBUSREG_vSetHoldRegFillCount(HMDRV_U16 u16Input)
{
    MODBUSREG_u16HoldRegFillCount = u16Input;
}

void MODBUSREG_vSetFileRTC_FunctionAfterWriteFileRecord(HMDRV_U32 u32Input)
{
    MODBUSREG_u32FileRTC_FunctionAfterWriteFileRecord = u32Input;
}

void MODBUSREG_vSetFileRTC_FunctionAfterReadFileRecord(HMDRV_U32 u32Input)
{
    MODBUSREG_u32FileRTC_FunctionAfterReadFileRecord = u32Input;
}

void MODBUSREG_vSetFileParamCount_FunctionBeforeReadFileRecord(HMDRV_U32 u32Input)
{
    MODBUSREG_u32FileParamCount_FunctionBeforeReadFileRecord = u32Input;
}

void MODBUSREG_vSetFileAutorization_FunctionAfterWriteFileRecordTable(HMDRV_U32 u32Input)
{
    MODBUSREG_u32FileAutorization_FunctionAfterWriteFileRecordTable = u32Input;
}




HMDRV_U16 MODBUSREG_u16GetHoldRegFillCount(void)
{
    HMDRV_U16 u16Result;
    u16Result = MODBUSREG_u16HoldRegFillCount;
    return u16Result;
}

HMDRV_U16 MODBUSREG_u16GetHoldRegUsedCount(void)
{
    HMDRV_U16 u16Result;
    u16Result = MODBUSREG_u16HoldRegUsedCount;
    return u16Result;
}

HMDRV_U32 MODBUSREG_u32GetRegisterAddressTable(void)
{
    HMDRV_U32 u32Result;
    u32Result = (uint32_t)MODBUSREG_pu32RegisterAddressTable;
    return u32Result;
}

HMDRV_U32 MODBUSREG_u32GetRegisterAddressTableInRam(void)
{
    HMDRV_U32 u32Result;
    u32Result = (uint32_t)MODBUSREG_pu32RegisterAddressTableInRam;
    return u32Result;
}

HMDRV_U32 MODBUSREG_u32GetRegisterAccessTableInRam(void)
{
    HMDRV_U32 u32Result;
    u32Result = (uint32_t)(HMDRV_ADDR_SIZE)MODBUSREG_u32RegisterAccessTableInRam;
    return u32Result;
}





HMDRV_U16 MODBUSREG_u16GetInputRegisterUsedCount(void)
{
    HMDRV_U16 u16Result;
    u16Result = MODBUSREG_INPUT_REGISTER_TABLE_SIZE;
    return u16Result;
}

HMDRV_U32 MODBUSREG_u32GetInputRegisterAddressTable(void)
{
    HMDRV_U32 u32Result;
    u32Result = (uint32_t)(HMDRV_CNST_PNTR_MODIFIER HMDRV_U8*)MODBUSREG_sInputRegisterAddressTable;
    return u32Result;
}

HMDRV_U32 MODBUSREG_u32GetInputRegisterAddressTableInRam(void)
{
    HMDRV_U32 u32Result;
    u32Result = (uint32_t)(HMDRV_ADDR_SIZE)MODBUSREG_sInputRegisterAddressTableInRam;
    return u32Result;
}


HMDRV_U32 MODBUSREG_u32GetInputRegisterAccessTableInRam(void)
{
    HMDRV_U32 u32Result;
    u32Result = (uint32_t)(HMDRV_ADDR_SIZE)MODBUSREG_sInputRegisterAccessTableInRam;
    return u32Result;
}






HMDRV_U16 MODBUSREG_u16GetHoldingCoilUsedCount(void)
{
    HMDRV_U16 u16Result;
    u16Result = MODBUSREG_COIL_TABLE_SIZE;
    return u16Result;
}

HMDRV_U32 MODBUSREG_u32GetHoldingCoilAddressTable(void)
{
    HMDRV_U32 u32Result;
    u32Result = (uint32_t)(HMDRV_CNST_PNTR_MODIFIER HMDRV_U8*)HMMODBUS_u32CoilAddressTable;
    return u32Result;
}

HMDRV_U32 MODBUSREG_u32GetHoldingCoilAddressTableInRam(void)
{
    HMDRV_U32 u32Result;
    u32Result = (uint32_t)(HMDRV_ADDR_SIZE)HMMODBUS_u32CoilAddressTableInRam;
    return u32Result;
}



HMDRV_U16 MODBUSREG_u16GetInputCoilUsedCount(void)
{
    HMDRV_U16 u16Result;
    u16Result = MODBUSREG_INPUT_COIL_TABLE_SIZE;
    return u16Result;
}

HMDRV_U32 MODBUSREG_u32GetInputCoilAddressTable(void)
{
    HMDRV_U32 u32Result;
    u32Result = (uint32_t)(HMDRV_CNST_PNTR_MODIFIER HMDRV_U8*)MODBUSREG_sInputCoilAddressTable;
    return u32Result;
}

HMDRV_U32 MODBUSREG_u32GetInputCoilAddressTableInRam(void)
{
    HMDRV_U32 u32Result;
    u32Result = (uint32_t)(HMDRV_ADDR_SIZE)MODBUSREG_sInputCoilAddressTableInRam;
    return u32Result;
}

HMDRV_U32 MODBUSREG_u32GetAddressMessageCounterThisDevice(void)
{
    HMDRV_U32 u32Result;
    u32Result = HMMODBUS_u32GetAddressMessageCounterThisDevice();
    return u32Result;
}


void  MODBUSREG_vFileDenyAccess(void)
{
    uint16_t u16Index;

    for (u16Index = 0 ; u16Index < MODBUSREG_FILE_COUNT; u16Index++)
    {
        if ((u16Index != MODBUSREG_FILE_CSMON_AUTORIZATION)
                /* && (u16Index != MODBUSREG_FILE_CSMON_IMPLEMENTED_FUNCTION_CODES) */
                && (u16Index != MODBUSREG_FILE_CSMON_PROTOCOL_VERSION))
        {
            MODBUSREG_asFileTable[u16Index].bWriteAccess = 0;

            if (u16Index != MODBUSREG_FILE_CSMON_RTC)
            {
                MODBUSREG_asFileTable[u16Index].bReadAccess = 0;
            }
        }
    }
}

void  MODBUSREG_vFileAllowAccess(void)
{
    uint16_t u16Index;

    for (u16Index = 0 ; u16Index < MODBUSREG_FILE_COUNT; u16Index++)
    {
        MODBUSREG_asFileTable[u16Index].bWriteAccess = 1;
        MODBUSREG_asFileTable[u16Index].bReadAccess = 1;
    }
}
