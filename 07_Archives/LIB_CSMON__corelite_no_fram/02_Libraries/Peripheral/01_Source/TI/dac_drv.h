/* *****************************************************************************
 * File:   dac_drv.h
 * Author: Dimitar Lilov
 *
 * Created on 2020 01 16 14:47
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
#ifndef DAC_DRV_H
#define	DAC_DRV_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdint.h>
#include <stdbool.h>

#include "driverlib.h"
#include "device.h"
#include "boardcfg.h"


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

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
#if (BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC) || (BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG)

void DAC_vInit(uint32_t base, DAC_ReferenceVoltage sourceReference);
void DAC_vProcess(void);
void DAC_vProcessAutomation(bool bIdleMode);
void DAC_vSetShadowValue(uint32_t base, uint16_t value);
bool DAC_bADCFeedbackMeasureNeeded(void);

#else

#define DAC_vInit(x,y)
#define DAC_vProcess()
#define DAC_vProcessAutomation(x)
#define DAC_vSetShadowValue(x,y)
#define DAC_bADCFeedbackMeasureNeeded()

#endif





#endif	/* DAC_DRV_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
