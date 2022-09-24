/* *****************************************************************************
 * File:   trip.h
 * Author: Dimitar Lilov
 *
 * Created on 2021 01 04 19:06
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
#ifndef TRIP_H
#define	TRIP_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdbool.h>
#include <stdint.h>
#include "stdextend.h"
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define TRIP_CURRENT_DEFAULT        1600.0
#define TRIP_VOLTAGE_DEFAULT        1200.0


/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */


/* GLOBAL CONSTANTS from fpga_common */
#define PMP_CONFIGURATION_UNLOCK_KEY                        0xA5    /* 1 byte */
#define PMP_ADDRESS_FPGA_RADDR_UNLOCK                       0x0080  /* 1 byte */
#define PMP_ADDRESS_OFFSET_VER                              0x0081  /* 1 byte */
#define PMP_ADDRESS_OFFSET_REV                              0x0082  /* 1 byte */
#define PMP_ADDRESS_OFFSET_SYS                              0x0010  /* 1 byte */
#define PMP_ADDRESS_OFFSET_FREG                             0x0000  /* 1 byte */


/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
typedef struct
{
    uint16_t bErrBot  : 1;
    uint16_t bErrTop  : 1;
    uint16_t bErrDet  : 1;
    uint16_t bErrCh1    : 1;
    uint16_t bErrCh2    : 1;
    uint16_t bErrCh3    : 1;
    uint16_t bReserved      : 1;
    uint16_t bGlobalFault   : 1;
    uint16_t u8Reserved     : 8;
}sFREGPWM_t;




typedef union
{
    uint16_t u16Register;
    sFREGPWM_t sFREGPWM;
}uFREGPWM_t;


/* driver detection */
typedef struct
{
    uint8_t ErrDET1 : 1;    /* Latched */
    uint8_t ErrDET2 : 1;    /* Latched */
    uint8_t ErrDET3 : 1;    /* Latched */
    uint8_t DET1 : 1;
    uint8_t DET2 : 1;
    uint8_t DET3 : 1;
}sDET_t;

typedef union
{
    uint8_t u8Register;
    sDET_t sDET;
}uDET_t;


/* gate driver errors */
typedef struct
{
    uint8_t ErrBot1 : 1;    /* Latched */
    uint8_t ErrBot2 : 1;    /* Latched */
    uint8_t ErrBot3 : 1;    /* Latched */
    uint8_t ErrTop1 : 1;    /* Latched */
    uint8_t ErrTop2 : 1;    /* Latched */
    uint8_t ErrTop3 : 1;    /* Latched */
}sGDE_t;

typedef union
{
    uint8_t u8Register;
    sGDE_t sGDE;
}uGDE_t;

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


extern uDET_t  uStatDET;
extern uGDE_t  uStatGDE;


extern int16_t TripCurrentStack1Offset[3];
extern int16_t TripCurrentStack2Offset[3];
extern int16_t TripVoltageStack1Offset;
extern int16_t TripVoltageStack2Offset;

extern int16_t TripCFBKStack1Offset;
extern int16_t TripCFBKStack2Offset;

extern int16_t TripCurrentStack1[3];
extern int16_t TripVoltageStack1;
extern int16_t TripVoltageStack2;

extern int16_t TripCFBKStack1;
extern int16_t TripCFBKStack2;


extern int16_t TripCurrentStack1Fbk[3];
extern int16_t TripVoltageStack1Fbk;
extern int16_t TripVoltageStack2Fbk;

extern int16_t TripCFBKStack1Fbk;
extern int16_t TripCFBKStack2Fbk;

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
 
#endif	/* TRIP_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
