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

/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 
extern int16_t TripCurrentStack1Offset[3];
extern int16_t TripCurrentStack2Offset[3];
extern int16_t TripVoltageStack1Offset;
extern int16_t TripVoltageStack2Offset;

extern int16_t TripCFBKStack1Offset;
extern int16_t TripCFBKStack2Offset;

extern int16_t TripCurrentStack1[3];
extern int16_t TripCurrentStack2[3];
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
