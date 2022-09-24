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
    ADC_UFBK1_RA0_AN0 = 0, 
    ADC_UFBK2_RA1_AN1 = 1,  
    ADC_UFBK3_RB0_AN2 = 2,  
    ADC_UFBK12_RC0_AN6 = 6, 
    ADC_UFBK23_RC1_AN7 = 7,  
    ADC_FBK_24V_RC11_AN11 = 11, 
    ADC_FBK_D5V_RG8_AN17 = 17, 
    ADC_FBK_5V_RG7_AN18 = 18,  
    ADC_FBK_M5V_RG6_AN19 = 19,  
    ADC_THR_FBK1_RA11_AN9 = 9,  
    ADC_THR_FBK2_RA4_AN24 = 24,  
    ADC_THR_FBK_RC2_AN8 = 8,  
    ADC_FBK_13V_RG9_AN16 = 16,  
    ADC_THR_FBKSW_RA12_AN10 = 10, 
} ADC_CHANNEL;



/* *****************************************************************************
 * Signal Names
 **************************************************************************** */


#ifndef BOARDCFG_USE_MCC_ADC

#define ADC_CHANNEL_CURRENT_STACK1_PHASEA       ADC_UFBK1_RA0_AN0 
#define ADC_CHANNEL_CURRENT_STACK1_PHASEB       ADC_UFBK2_RA1_AN1 
#define ADC_CHANNEL_CURRENT_STACK1_PHASEC       ADC_UFBK3_RB0_AN2 
#define ADC_CHANNEL_CURRENT_STACK2_PHASEA       ADC_UFBK12_RC0_AN6 
#define ADC_CHANNEL_CURRENT_STACK2_PHASEB       ADC_UFBK23_RC1_AN7 
#define ADC_CHANNEL_CURRENT_STACK2_PHASEC       ADC_UFBK12_RC0_AN6 
#define ADC_CHANNEL_VOLTAGE_DCLINK              ADC_FBK_24V_RC11_AN11  
#define ADC_CHANNEL_VOLTAGE_2                   ADC_FBK_24V_RC11_AN11 

#define ADC_CHANNEL_CURRENT_DRIVER_PHASEA       ADC_FBK_D5V_RG8_AN17 
#define ADC_CHANNEL_CURRENT_DRIVER_PHASEB       ADC_FBK_5V_RG7_AN18 
#define ADC_CHANNEL_CURRENT_DRIVER_PHASEC       ADC_FBK_M5V_RG6_AN19 
#define ADC_CHANNEL_TEMPERATURE_DRIVER_PHASEA   ADC_THR_FBK1_RA11_AN9 
#define ADC_CHANNEL_TEMPERATURE_DRIVER_PHASEB   ADC_THR_FBK2_RA4_AN24 
#define ADC_CHANNEL_TEMPERATURE_DRIVER_PHASEC   ADC_THR_FBK_RC2_AN8 
#define ADC_CHANNEL_VOLTAGE_3                   ADC_FBK_13V_RG9_AN16 
#define ADC_CHANNEL_VOLTAGE_4                   ADC_THR_FBKSW_RA12_AN10

#else

#define ADC_CHANNEL_CURRENT_STACK1_PHASEA       ADC1_UFBK1_RA0_AN0 
#define ADC_CHANNEL_CURRENT_STACK1_PHASEB       ADC1_UFBK2_RA1_AN1 
#define ADC_CHANNEL_CURRENT_STACK1_PHASEC       ADC1_UFBK3_RB0_AN2 
#define ADC_CHANNEL_CURRENT_STACK2_PHASEA       ADC1_UFBK12_RC0_AN6 
#define ADC_CHANNEL_CURRENT_STACK2_PHASEB       ADC1_UFBK23_RC1_AN7 
#define ADC_CHANNEL_CURRENT_STACK2_PHASEC       ADC1_UFBK12_RC0_AN6 
#define ADC_CHANNEL_VOLTAGE_DCLINK              ADC1_FBK_24V_RC11_AN11 
#define ADC_CHANNEL_VOLTAGE_2                   ADC1_FBK_24V_RC11_AN11 

#define ADC_CHANNEL_CURRENT_DRIVER_PHASEA       ADC2_FBK_D5V_RG8_AN17 
#define ADC_CHANNEL_CURRENT_DRIVER_PHASEB       ADC2_FBK_5V_RG7_AN18 
#define ADC_CHANNEL_CURRENT_DRIVER_PHASEC       ADC2_FBK_M5V_RG6_AN19 
#define ADC_CHANNEL_TEMPERATURE_DRIVER_PHASEA   ADC2_THR_FBK1_RA11_AN9 
#define ADC_CHANNEL_TEMPERATURE_DRIVER_PHASEB   ADC2_THR_FBK2_RA4_AN24 
#define ADC_CHANNEL_TEMPERATURE_DRIVER_PHASEC   ADC2_THR_FBK_RC2_AN8 
#define ADC_CHANNEL_VOLTAGE_3                   ADC2_FBK_13V_RG9_AN16 
#define ADC_CHANNEL_VOLTAGE_4                   ADC2_THR_FBKSW_RA12_AN10


#endif

/* *****************************************************************************
 * Configuration Definitions ADC
 **************************************************************************** */
// CSS26 disabled; CSS25 disabled; CSS24 disabled; CSS31 disabled; CSS30 disabled; CSS19 disabled; CSS18 disabled; CSS29 disabled; CSS17 disabled; CSS28 disabled; CSS16 disabled; CSS27 disabled; 
#define AD1CSSH_MASK 0x0000    
// CSS9 disabled; CSS8 disabled; CSS7 enabled; CSS6 enabled; CSS5 disabled; CSS4 disabled; CSS3 disabled; CSS2 enabled; CSS15 disabled; CSS1 enabled; CSS14 disabled; CSS0 enabled; CSS13 disabled; CSS12 disabled; CSS11 enabled; CSS10 disabled; 
#define AD1CSSL_MASK 0x08C7    
#define AD1CSS_COUNT 6

// CSS26 disabled; CSS25 disabled; CSS24 enabled; CSS31 disabled; CSS30 disabled; CSS19 enabled; CSS18 enabled; CSS29 disabled; CSS17 enabled; CSS28 disabled; CSS16 enabled; CSS27 disabled; 
#define AD2CSSH_MASK 0x010F;
// CSS9 enabled; CSS8 enabled; CSS7 disabled; CSS6 disabled; CSS5 disabled; CSS4 disabled; CSS3 disabled; CSS2 disabled; CSS15 disabled; CSS1 disabled; CSS14 disabled; CSS0 disabled; CSS13 disabled; CSS12 disabled; CSS11 disabled; CSS10 enabled; 
#define AD2CSSL_MASK 0x0700;
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
