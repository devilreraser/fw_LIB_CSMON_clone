/* *****************************************************************************
 * File:   fpga_trip.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 08 14 12:51
 * 
 * Description: FPGA AP TRIP (Analog PWM board TRIP fpga)
 * 
 **************************************************************************** */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Guard condition file contents not included more than once */  
/* #pragma once */
#ifndef FPGA_TRIP_H
#define	FPGA_TRIP_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdbool.h> 
#include <stdint.h> 
    
#include "boardcfg.h"

#if BOARDCFG_USE_FPGA_TRIP

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
typedef struct
{
    uint16_t bOverCurrentA  : 1;
    uint16_t bOverCurrentB  : 1;
    uint16_t bOverCurrentFW : 1;
    uint16_t bOverCurrentFE : 1;
    uint16_t bOverCurrent   : 1;
    uint16_t bOverVoltage   : 1;
    uint16_t bReserved      : 1;
    uint16_t bGlobalFault   : 1;
    uint16_t u8Reserved     : 8;
}sFREGTrip_t;

typedef union
{
    uint16_t u16Register;
    sFREGTrip_t sFREGTrip;
}uFREGTrip_t;


typedef struct
{
    uint16_t bTripCurrentStack1PhaseA : 1;
    uint16_t bTripCurrentStack1PhaseB : 1;
    uint16_t bTripCurrentStack1PhaseC : 1;
    uint16_t bTripVoltageStack1       : 1;
    uint16_t bTripCurrentStack2PhaseA : 1;
    uint16_t bTripCurrentStack2PhaseB : 1;
    uint16_t bTripCurrentStack2PhaseC : 1;
    uint16_t bTripVoltageStack2       : 1;
    uint16_t u8Reserved               : 8;
}sControlTrip_t;

typedef union
{
    uint16_t u16Register;
    sControlTrip_t sControlTrip;
}uControlTrip_t;

typedef struct
{
    uint8_t OCP1 : 1;
    uint8_t OCP2 : 1;
    uint8_t OCP3 : 1;
    uint8_t OCN1 : 1;
    uint8_t OCN2 : 1;
    uint8_t OCN3 : 1;
}sOCTrip_t;

typedef struct
{
    uint8_t OVA1 : 1;
    uint8_t OVA2 : 1;
    uint8_t OVA3 : 1;
    uint8_t OVA4 : 1;
    uint8_t OVB1 : 1;
    uint8_t OVB2 : 1;
    uint8_t OVB3 : 1;
    uint8_t OVB4 : 1;  
}sOVTrip_t;

typedef union
{
    uint8_t u8Register;
    sOCTrip_t sOC;
}uOCTrip_t;

typedef union
{
    uint8_t u8Register;
    sOVTrip_t sOV;
}uOVTrip_t;


/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 
extern uOCTrip_t StatOCA;
extern uOCTrip_t StatOCFE;
extern uOVTrip_t StatOV;

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
bool FPGA_TRIP_vGetTripError(void);
void FPGA_TRIP_vClearTrip(void);
void FPGA_TRIP_vClearErrorStatus(void);
void FPGA_TRIP_vGetTripStat(void);
void FPGA_TRIP_vMaskConfigure (void);
void FPGA_TRIP_vGetTripMasks(void);
void FPGA_TRIP_vRunTimeSetupProcess(void);

#else
#define FPGA_TRIP_vGetTripError() 0
#define FPGA_TRIP_vClearTrip()
#define FPGA_TRIP_vClearErrorStatus()
#define FPGA_TRIP_vGetTripStat()
#define FPGA_TRIP_vMaskConfigure()
#define FPGA_TRIP_vGetTripMasks()
#define FPGA_TRIP_vRunTimeSetupProcess()
#endif

#endif	/* FPGA_TRIP_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
