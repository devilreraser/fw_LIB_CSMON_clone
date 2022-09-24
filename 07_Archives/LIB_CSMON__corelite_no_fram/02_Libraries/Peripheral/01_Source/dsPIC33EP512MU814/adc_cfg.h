/* *****************************************************************************
 * File:   adc_cfg.h
 * Author: Dimitar Lilov
 *
 * Created on 2020 04 15 11:55
 * 
 * Description: Pin Configuration
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef ADC_CFG_H
#define	ADC_CFG_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <xc.h>

#include "boardcfg.h"

/* *****************************************************************************
 * Constants Definitions ADC Channels - PCB Net Names - Like
 **************************************************************************** */

/* ADC Channel Value non-MCC */
typedef enum 
{
    ANC1_CHANNEL,
    ANC2_CHANNEL,
    ANC3_CHANNEL,
    ANC4_CHANNEL,
    ANC5_CHANNEL,
    ANC6_CHANNEL,
    ANC7_CHANNEL,
    ANC8_CHANNEL,
    ANC9_CHANNEL,
    ANC10_CHANNEL,
    ANC11_CHANNEL,
    ANC12_CHANNEL,
    ANC13_CHANNEL,
    ANC14_CHANNEL,
    ANC15_CHANNEL,
    ANC16_CHANNEL,
} ADC_CHANNEL;

typedef enum 
{
    L1_CURRENT_1,
    L2_CURRENT_1,
    L3_CURRENT_1,
    L1_CURRENT_2,
    L2_CURRENT_2,
    L3_CURRENT_2,
    DC_VOLTAGE_1,
    DC_VOLTAGE_2,
    DC_VOLTAGE_3,
    DC_VOLTAGE_4,
    L1_CURRFBK_1,
    L2_CURRFBK_1,
    L3_CURRFBK_1,
    L1_TEMPFBK_1,
    L2_TEMPFBK_1,
    L3_TEMPFBK_1,
} ADC_SIGNAL;


/* *****************************************************************************
 * Signal Names
 **************************************************************************** */


#ifndef BOARDCFG_USE_MCC_ADC

#define ADC_CHANNEL_CURRENT_STACK1_PHASEA       L1_CURRENT_1 
#define ADC_CHANNEL_CURRENT_STACK1_PHASEB       L2_CURRENT_1 
#define ADC_CHANNEL_CURRENT_STACK1_PHASEC       L3_CURRENT_1 
#define ADC_CHANNEL_CURRENT_STACK2_PHASEA       L1_CURRENT_2 
#define ADC_CHANNEL_CURRENT_STACK2_PHASEB       L2_CURRENT_2 
#define ADC_CHANNEL_CURRENT_STACK2_PHASEC       L3_CURRENT_2 
#define ADC_CHANNEL_VOLTAGE_DCLINK              DC_VOLTAGE_1 
#define ADC_CHANNEL_VOLTAGE_2                   DC_VOLTAGE_2

#define ADC_CHANNEL_CURRENT_DRIVER_PHASEA       L1_CURRFBK_1 
#define ADC_CHANNEL_CURRENT_DRIVER_PHASEB       L2_CURRFBK_1 
#define ADC_CHANNEL_CURRENT_DRIVER_PHASEC       L3_CURRFBK_1 
#define ADC_CHANNEL_TEMPERATURE_DRIVER_PHASEA   L1_TEMPFBK_1
#define ADC_CHANNEL_TEMPERATURE_DRIVER_PHASEB   L2_TEMPFBK_1 
#define ADC_CHANNEL_TEMPERATURE_DRIVER_PHASEC   L3_TEMPFBK_1 
#define ADC_CHANNEL_VOLTAGE_3                   DC_VOLTAGE_3
#define ADC_CHANNEL_VOLTAGE_4                   DC_VOLTAGE_4

#else

#define ADC_CHANNEL_CURRENT_STACK1_PHASEA       Current_Stack1_PhaseA 
#define ADC_CHANNEL_CURRENT_STACK1_PHASEB       Current_Stack1_PhaseB 
#define ADC_CHANNEL_CURRENT_STACK1_PHASEC       Current_Stack1_PhaseC 
#define ADC_CHANNEL_CURRENT_STACK2_PHASEA       Current_Stack2_PhaseA 
#define ADC_CHANNEL_CURRENT_STACK2_PHASEB       Current_Stack2_PhaseB 
#define ADC_CHANNEL_CURRENT_STACK2_PHASEC       Current_Stack2_PhaseC 
#define ADC_CHANNEL_VOLTAGE_DCLINK              Voltage_DCLink 
#define ADC_CHANNEL_VOLTAGE_2                   Voltage_2

#define ADC_CHANNEL_CURRENT_DRIVER_PHASEA       Current_Driver_PhaseA 
#define ADC_CHANNEL_CURRENT_DRIVER_PHASEB       Current_Driver_PhaseB 
#define ADC_CHANNEL_CURRENT_DRIVER_PHASEC       Current_Driver_PhaseC 
#define ADC_CHANNEL_TEMPERATURE_DRIVER_PHASEA   Temperature_Driver_PhaseA 
#define ADC_CHANNEL_TEMPERATURE_DRIVER_PHASEB   Temperature_Driver_PhaseB 
#define ADC_CHANNEL_TEMPERATURE_DRIVER_PHASEC   Temperature_Driver_PhaseC 
#define ADC_CHANNEL_VOLTAGE_3                   Voltage_3 
#define ADC_CHANNEL_VOLTAGE_4                   Voltage_4


#endif

/* *****************************************************************************
 * Configuration Definitions ADC
 **************************************************************************** */
// CSS26 disabled; CSS25 disabled; CSS24 disabled; CSS31 disabled; CSS30 disabled; CSS19 disabled; CSS18 disabled; CSS29 disabled; CSS17 disabled; CSS28 disabled; CSS16 disabled; CSS27 disabled; 
#define AD1CSSH_MASK 0x0000
// CSS9 disabled; CSS8 disabled; CSS7 enabled; CSS6 enabled; CSS5 disabled; CSS4 disabled; CSS3 disabled; CSS2 enabled; CSS15 disabled; CSS1 enabled; CSS14 disabled; CSS0 enabled; CSS13 disabled; CSS12 disabled; CSS11 enabled; CSS10 disabled; 
#define AD1CSSL_MASK 0x00FF
#define AD1CSS_COUNT 8

// CSS26 disabled; CSS25 disabled; CSS24 enabled; CSS31 disabled; CSS30 disabled; CSS19 enabled; CSS18 enabled; CSS29 disabled; CSS17 enabled; CSS28 disabled; CSS16 enabled; CSS27 disabled; 
#define AD2CSSH_MASK 0x0000
// CSS9 enabled; CSS8 enabled; CSS7 disabled; CSS6 disabled; CSS5 disabled; CSS4 disabled; CSS3 disabled; CSS2 disabled; CSS15 disabled; CSS1 disabled; CSS14 disabled; CSS0 disabled; CSS13 disabled; CSS12 disabled; CSS11 disabled; CSS10 enabled; 
#define AD2CSSL_MASK 0xFF00
#define AD2CSS_COUNT 8




/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */
#ifndef BOARDCFG_USE_MCC_ADC
#ifndef _ADC1_H
#define ADC1_ConversionResultGet(x) ((uint16_t*)(&ADC1BUF0))[x]       
#endif
#ifndef _ADC2_H
#define ADC2_ConversionResultGet(x) ((uint16_t*)(&ADC2BUF0))[x]        
#endif
#endif


/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 


/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */


#endif	/* PIN_CFG_H */
