/* *****************************************************************************
 * File:   dac_drv.c
 * Author: Dimitar Lilov
 *
 * Created on 2020 01 16 14:47
 * 
 * Description: ...
 * 
 **************************************************************************** */
#ifdef _WIN32
#pragma once
#endif /* _WIN32 */

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "dac_drv.h"
#include "adc_drv.h"
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
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */

#if (BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG) || (BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC)

#define DACA_TRIP_CURRENT_A (1000.0)    /* Max Current Amplitude of One Phase (for 3 stacks in parallel - divide by 3 the total current amplitude) */
#define DACB_TRIP_VOLTAGE_V (1350.0)    /* To Be Set to 1350 V */


uint16_t DACA_u16ValueReq = (uint16_t)(DACA_TRIP_CURRENT_A * (1.0 / DAC_RESULT_TO_AMP_IFBK_MULT));
uint16_t DACB_u16ValueReq = (uint16_t)(DACB_TRIP_VOLTAGE_V * (1.0 / DAC_RESULT_TO_VOLTS_DC_MULT));



uint16_t DACA_u16ValueReqOld = 0xFFFF;
uint16_t DACB_u16ValueReqOld = 0xFFFF;
uint16_t DACA_u16Value = 0;
uint16_t DACB_u16Value = 0;
 int16_t DACA_s16ValueFix = 0;
 int16_t DACB_s16ValueFix = 0;
 int16_t DACA_s16ValueFixMin = -50;
 int16_t DACB_s16ValueFixMin = -50;
 int16_t DACA_s16ValueFixMax =  50;
 int16_t DACB_s16ValueFixMax =  50;

 bool bDACSetupInTdleMode = 1;


 bool bDACIdleMode = 0;
 bool bDACFeedbackMeasure = 0;
 bool bDACUpdateEnabled = 1;

 bool bDACAFixWithFeedback = 1;
 bool bDACBFixWithFeedback = 1;

 bool bDACAFixWithFeedbackOld = 0;
 bool bDACBFixWithFeedbackOld = 0;

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */

//
// Configure DAC - Setup the reference voltage and output value for the DAC
//
void DAC_vInit(uint32_t base, DAC_ReferenceVoltage sourceReference)
{
    // Set VDAC as the DAC reference voltage.
    // Edit here to use ADC VREF as the reference voltage.
    DAC_setReferenceVoltage(base, sourceReference);

    // Enable the DAC output
    DAC_enableOutput(base);

    // Set the DAC shadow output to 0
    DAC_setShadowValue(base, 0);

    // Delay for buffered DAC to power up
    DEVICE_DELAY_US(10);
}


void DAC_vSetShadowValue(uint32_t base, uint16_t value)
{
    DAC_setShadowValue(base, value);
}


void DAC_vFixWithFeedbackDACA(uint16_t value)
{
    if (value > (DACA_u16ValueReq+1)) {
        if (DACA_s16ValueFix > DACA_s16ValueFixMin) {
            DACA_s16ValueFix--;
        }
    } else if (value < (DACA_u16ValueReq-1)) {
        if (DACA_s16ValueFix < DACA_s16ValueFixMax) {
            DACA_s16ValueFix++;
        }
    }
}


void DAC_vFixWithFeedbackDACB(uint16_t value)
{
    if (value > (DACB_u16ValueReq+1)) {
        if (DACB_s16ValueFix > DACB_s16ValueFixMin) {
            DACB_s16ValueFix--;
        }
    } else if (value < (DACB_u16ValueReq-1)) {
        if (DACB_s16ValueFix < DACB_s16ValueFixMax) {
            DACB_s16ValueFix++;
        }
    }
}


bool DAC_bADCFeedbackMeasureNeeded(void)
{
    return bDACFeedbackMeasure;
}


void DAC_vProcessAutomation(bool bIdleMode)
{
    if (bIdleMode) {
        bDACIdleMode = 1;
    } else {
        bDACIdleMode = 0;
    }
    if (bDACIdleMode) {
        bDACFeedbackMeasure = 1;
    } else {
        bDACFeedbackMeasure = 0;
    }
}



void DAC_vProcess(void)
{
    if (bDACFeedbackMeasure) {
        if (DACA_u16ValueReqOld != DACA_u16ValueReq) {
            DACA_u16ValueReqOld = DACA_u16ValueReq;
            DACA_u16Value = DACA_u16ValueReq;
            DACA_s16ValueFix = 0;
        } else {
            if(bDACAFixWithFeedbackOld != bDACAFixWithFeedback) {
                bDACAFixWithFeedbackOld = bDACAFixWithFeedback;
                DACA_s16ValueFix = 0;
            }
            if(bDACAFixWithFeedback) {
#if (BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC)
            	DAC_vFixWithFeedbackDACA(u16Adc_DAC_outA);
#elif (BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG)
                DAC_vFixWithFeedbackDACA(u16Adc_DACOUTA);
#endif
                if (DACA_s16ValueFix >= 0) {
                    if ( (DACA_u16ValueReq + DACA_s16ValueFix) < (1 << BOARDCFG_DAC_RESULT_BITS) ) {
                        DACA_u16Value = DACA_u16ValueReq + DACA_s16ValueFix;
                    } else {
                        DACA_u16Value = (1 << BOARDCFG_DAC_RESULT_BITS) - 1;
                    }
                } else {  /*  if (DACA_s16ValueFix < 0) */
                    if ( DACA_u16ValueReq >= (0-DACA_s16ValueFix) ) {
                        DACA_u16Value = DACA_u16ValueReq + DACA_s16ValueFix;
                    } else {
                        DACA_u16Value = 0;
                    }
                }
            } else {
                DACA_u16Value = DACA_u16ValueReq + DACA_s16ValueFix;
            }
        }
        if (DACB_u16ValueReqOld != DACB_u16ValueReq) {
            DACB_u16ValueReqOld = DACB_u16ValueReq;
            DACB_u16Value = DACB_u16ValueReq;
            DACB_s16ValueFix = 0;
        } else {
            if(bDACBFixWithFeedbackOld != bDACBFixWithFeedback) {
                bDACBFixWithFeedbackOld = bDACBFixWithFeedback;
                DACB_s16ValueFix = 0;
            }
            if(bDACBFixWithFeedback) {
#if (BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC)
            	DAC_vFixWithFeedbackDACB(u16Adc_DAC_outB);
#elif (BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG)
                DAC_vFixWithFeedbackDACB(u16Adc_DACOUTB);
#endif
                if (DACB_s16ValueFix >= 0) {
                    if ( (DACB_u16ValueReq + DACB_s16ValueFix) < (1 << BOARDCFG_DAC_RESULT_BITS) ) {
                        DACB_u16Value = DACB_u16ValueReq + DACB_s16ValueFix;
                    } else {
                        DACB_u16Value = (1 << BOARDCFG_DAC_RESULT_BITS) - 1;
                    }
                } else {  /*  if (DACB_s16ValueFix < 0) */
                    if ( DACB_u16ValueReq >= (0 - DACB_s16ValueFix) ) {
                        DACB_u16Value = DACB_u16ValueReq + DACB_s16ValueFix;
                    } else {
                        DACB_u16Value = 0;
                    }
                }
            } else {
                DACB_u16Value = DACB_u16ValueReq + DACB_s16ValueFix;
            }
        }
    } else {   /* no compensation running */
        if (DACA_u16ValueReqOld != DACA_u16ValueReq) {
            DACA_u16Value = DACA_u16ValueReq + DACA_s16ValueFix;
        }
        if (DACB_u16ValueReqOld != DACB_u16ValueReq) {
            DACB_u16Value = DACB_u16ValueReq + DACB_s16ValueFix;
        }
    }

    if (bDACUpdateEnabled) {
        if (DACA_u16Value >= (1 << BOARDCFG_DAC_RESULT_BITS)) {
            DACA_u16Value = (1 << BOARDCFG_DAC_RESULT_BITS) - 1;
        }
        if (DACB_u16Value >= (1 << BOARDCFG_DAC_RESULT_BITS)) {
            DACB_u16Value = (1 << BOARDCFG_DAC_RESULT_BITS) - 1;
        }
        DAC_vSetShadowValue(DACA_BASE, DACA_u16Value);
        DAC_vSetShadowValue(DACB_BASE, DACB_u16Value);
    }
}

#endif
