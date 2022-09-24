/* *****************************************************************************
 * File:   dac_trip.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 08 14 14:32
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
#ifndef DAC_TRIP_H
#define	DAC_TRIP_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdint.h>
    
    
#include "boardcfg.h"

#if BOARDCFG_USE_DAC_TRIP

#if USE_I2C_DAC
#include "i2cdac.h"
#endif
    
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */
#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0

#define DAC_TRIP_CURRENT_ONE_STACK_COUNT    3

#else

#define DAC_TRIP_CURRENT_ONE_STACK_COUNT    2

#endif
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
#if BOARDCFG_DAC_POSITIVE_AND_NEGATIVE_LEVELS == 0
    
extern int16_t TripCurrentStack1Offset[DAC_TRIP_CURRENT_ONE_STACK_COUNT];
extern int16_t TripCurrentStack2Offset[DAC_TRIP_CURRENT_ONE_STACK_COUNT];
extern int16_t TripVoltageStack1Offset;
extern int16_t TripVoltageStack2Offset;

extern int16_t TripCurrentStack1[DAC_TRIP_CURRENT_ONE_STACK_COUNT];
extern int16_t TripCurrentStack2[DAC_TRIP_CURRENT_ONE_STACK_COUNT];
extern int16_t TripVoltageStack1;
extern int16_t TripVoltageStack2;

extern int16_t TripCurrentStack1Fbk[DAC_TRIP_CURRENT_ONE_STACK_COUNT];
extern int16_t TripVoltageStack1Fbk;
extern int16_t TripVoltageStack2Fbk;

extern int16_t TripCFBKStack1Offset;
extern int16_t TripCFBKStack2Offset;

extern int16_t TripCFBKStack1Fbk;
extern int16_t TripCFBKStack2Fbk;

extern int16_t TripCFBKStack1;
extern int16_t TripCFBKStack2;

#else

extern uint16_t TripCurrentStack1Offset[DAC_TRIP_CURRENT_ONE_STACK_COUNT];
extern uint16_t TripCurrentStack2Offset[DAC_TRIP_CURRENT_ONE_STACK_COUNT];
extern uint16_t TripVoltageStack1Offset;
extern uint16_t TripVoltageStack2Offset;

extern uint16_t TripCurrentStack1[DAC_TRIP_CURRENT_ONE_STACK_COUNT];
extern uint16_t TripCurrentStack2[DAC_TRIP_CURRENT_ONE_STACK_COUNT];
extern uint16_t TripVoltageStack1;
extern uint16_t TripVoltageStack2;

extern uint16_t TripCurrentStack1Fbk[DAC_TRIP_CURRENT_ONE_STACK_COUNT];
extern uint16_t TripCurrentStack2Fbk[DAC_TRIP_CURRENT_ONE_STACK_COUNT];
extern uint16_t TripVoltageStack1Fbk;
extern uint16_t TripVoltageStack2Fbk;

#endif


/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void DAC_TRIP_vSetOverVoltageOverCurrentLevelsDAC(void);
void DAC_TRIP_vGetOverVoltageOverCurrentLevelsDAC(void);
 
#else

#define DAC_TRIP_vSetOverVoltageOverCurrentLevelsDAC();
#define DAC_TRIP_vGetOverVoltageOverCurrentLevelsDAC();

#endif /* BOARDCFG_USE_DAC_TRIP */


#endif	/* DAC_TRIP_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
