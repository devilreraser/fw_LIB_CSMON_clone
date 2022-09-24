/* *****************************************************************************
 * File:   modbusreg.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 05 21 11:58
 * 
 * Description: Modbus Registers
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
#ifndef MODBUSREG_H
#define	MODBUSREG_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdint.h>
#include <stdbool.h>
#include "hmlib.h"
#include "boardcfg.h"
#include "hmmodbus.h"

#include "modbusreg_t.h"
    
#if USE_SPI_FLIR
#include "flir.h"
#endif
    
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Constants Definitions
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
//extern const HMMODBUS_sRegisterAddressTable_t HMMODBUS_u32RegisterAddressTable[MODBUSREG_REGISTER_TABLE_SIZE];
//extern HMMODBUS_sRegisterAddressTableInRam_t HMMODBUS_u32RegisterAddressTableInRam[MODBUSREG_REGISTER_TABLE_SIZE];
extern uint16_t HMMODBUS_u16RegisterAddressTableControlParametersStartIndex;
extern HMDRV_U16 u16ModbusCompleteReceiveMessageCounterThisDevice;
/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void MODBUSREG_AddressTablesInit(void);
MODBUSREGLIB_API void MODBUSREG_vInit(void);
void MODBUSREG_vInitHoldingRegistersTableChanges(void);
void MODBUSREG_vProcess(void);


#if USE_SPI_FLIR
HMDRV_U32 MODBUSREG_u32GetFlirCameraDataAddressZIP(void);
HMDRV_U32 MODBUSREG_u32GetFlirCameraDataAddressZIPPing(void);
HMDRV_U32 MODBUSREG_u32GetFlirCameraDataAddressZIPPong(void);
void MODBUSREG_vSetFlirCameraDataAddressZIPPing(void);
void MODBUSREG_vSetFlirCameraDataAddressZIPPong(void);
void MODBUSREG_vSetFlirCameraDataAddressZIPPingPong(HMDRV_U32 nAddress);

HMDRV_U32 MODBUSREG_u32GetFlirCameraDataAddress(void);
HMDRV_U32 MODBUSREG_u32GetFlirCameraDataAddressPing(void);
HMDRV_U32 MODBUSREG_u32GetFlirCameraDataAddressPong(void);
HMDRV_U32 MODBUSREG_u32GetFlirCameraDataAddress3(void);

void MODBUSREG_vSetFlirCameraDataAddressPing(void);
void MODBUSREG_vSetFlirCameraDataAddressPong(void);
void MODBUSREG_vSetFlirCameraDataAddressPingPong(HMDRV_U32 nAddress);
#endif  //USE_SPI_FLIR


void MODBUSREG_vSetParamCount           (HMDRV_U16 u16Input);
void MODBUSREG_vSetModbusParamCount     (HMDRV_U16 u16Input);
void MODBUSREG_vSetParamRecorderConfigAddress(HMDRV_U32 u32Input);
void MODBUSREG_vSetParamScopeConfigAddress(HMDRV_U32 u32Input);
void MODBUSREG_vSetParamListAddress     (HMDRV_U32 u32Input);
void MODBUSREG_vSetParamNameAddress     (HMDRV_U32 u32Input);
void MODBUSREG_vSetParamNumbAddress     (HMDRV_U32 u32Input);
void MODBUSREG_vSetParamStringAddress   (HMDRV_U32 u32Input);
void MODBUSREG_vSetParamAuxAddress      (HMDRV_U32 u32Input);
void MODBUSREG_vSetParamReservedAddress (HMDRV_U32 u32Input);
void MODBUSREG_vSetHoldRegFillCount     (HMDRV_U16 u16Input);
void MODBUSREG_vSetFileRTC_FunctionAfterWriteFileRecord(HMDRV_U32 u32Input);
void MODBUSREG_vSetFileRTC_FunctionAfterReadFileRecord(HMDRV_U32 u32Input);
void MODBUSREG_vSetFileParamCount_FunctionBeforeReadFileRecord(HMDRV_U32 u32Input);
void MODBUSREG_vSetFileAutorization_FunctionAfterWriteFileRecordTable(HMDRV_U32 u32Input);


HMDRV_U16 MODBUSREG_u16GetHoldRegFillCount(void);

HMDRV_U16 MODBUSREG_u16GetHoldRegUsedCount(void);
HMDRV_U32 MODBUSREG_u32GetRegisterAddressTable(void);
HMDRV_U32 MODBUSREG_u32GetRegisterAddressTableInRam(void);
HMDRV_U32 MODBUSREG_u32GetRegisterAccessTableInRam(void);


HMDRV_U16 MODBUSREG_u16GetInputRegisterUsedCount(void);
HMDRV_U32 MODBUSREG_u32GetInputRegisterAddressTable(void);
HMDRV_U32 MODBUSREG_u32GetInputRegisterAddressTableInRam(void);
HMDRV_U32 MODBUSREG_u32GetInputRegisterAccessTableInRam(void);


HMDRV_U16 MODBUSREG_u16GetHoldingCoilUsedCount(void);
HMDRV_U32 MODBUSREG_u32GetHoldingCoilAddressTable(void);
HMDRV_U32 MODBUSREG_u32GetHoldingCoilAddressTableInRam(void);


HMDRV_U16 MODBUSREG_u16GetInputCoilUsedCount(void);
HMDRV_U32 MODBUSREG_u32GetInputCoilAddressTable(void);
HMDRV_U32 MODBUSREG_u32GetInputCoilAddressTableInRam(void);

HMDRV_U32 MODBUSREG_u32GetAddressMessageCounterThisDevice(void);


void  MODBUSREG_vFileDenyAccess(void);
void  MODBUSREG_vFileAllowAccess(void);


#endif	/* MODBUSREG_H */


#ifdef __cplusplus
}
#endif // __cplusplus
