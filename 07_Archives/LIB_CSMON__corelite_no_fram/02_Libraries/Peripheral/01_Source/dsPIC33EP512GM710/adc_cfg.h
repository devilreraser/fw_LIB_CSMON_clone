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
    ADC_CHANNEL_ADC1,
    ADC_CHANNEL_ADC2,
    ADC_CHANNEL_ADC3,
    ADC_CHANNEL_ADC4,
    ADC_CHANNEL_ADC5,
    ADC_CHANNEL_ADC6,
    ADC_CHANNEL_ADC7,
    ADC_CHANNEL_ADC8,
    ADC_CHANNEL_ADC9,
    ADC_CHANNEL_ADC10,
    ADC_CHANNEL_ADC11,
    ADC_CHANNEL_ADC12,
    ADC_CHANNEL_ADC13,
    ADC_CHANNEL_ADC14,
    ADC_CHANNEL_ADC15,
    ADC_CHANNEL_ADC16,
    ADC_CHANNEL_ADC17,
    ADC_CHANNEL_CAN1TX,
    ADC_CHANNEL_CAN1RX,         
    ADC_CHANNEL_ADC18,
    ADC_CHANNEL_ADC19,
    ADC_CHANNEL_ADC20,
} ADC_CHANNEL;



/* *****************************************************************************
 * Signal Names
 **************************************************************************** */

/* ADC Result Position */
/* ADC1 */
#define ADC_BUF_POS_CURRENT_STACK1_PHASEA       0 
#define ADC_BUF_POS_CURRENT_STACK1_PHASEB       1 
#define ADC_BUF_POS_CURRENT_STACK1_PHASEC       2 
#define ADC_BUF_POS_CURRENT_STACK2_PHASEA       3 
#define ADC_BUF_POS_CURRENT_STACK2_PHASEB       4 
#define ADC_BUF_POS_CURRENT_STACK2_PHASEC       5 
#define ADC_BUF_POS_VOLTAGE_DCLINK              6 
#define ADC_BUF_POS_VOLTAGE_2                   7

/* ADC2 */
#define ADC_BUF_POS_CURRENT_DRIVER_PHASEA       0 
#define ADC_BUF_POS_CURRENT_DRIVER_PHASEB       1 
#define ADC_BUF_POS_CURRENT_DRIVER_PHASEC       2 
#define ADC_BUF_POS_CURR_STACK2_DRIVER_PHASEA   3 
#define ADC_BUF_POS_CURR_STACK2_DRIVER_PHASEB   4 
#define ADC_BUF_POS_CURR_STACK2_DRIVER_PHASEC   5 
#define ADC_BUF_POS_TEMPERATURE_DRIVER_PHASEA   6
#define ADC_BUF_POS_TEMPERATURE_DRIVER_PHASEB   7 
#define ADC_BUF_POS_TEMPERATURE_DRIVER_PHASEC   8 
#define ADC_BUF_POS_TEMP_STACK2_DRIVER_PHASEA   9
#define ADC_BUF_POS_TEMP_STACK2_DRIVER_PHASEB   10 
#define ADC_BUF_POS_TEMP_STACK2_DRIVER_PHASEC   11 


#ifndef BOARDCFG_USE_MCC_ADC

#define ADC_CHANNEL_CURRENT_STACK1_PHASEA       ADC_CHANNEL_ADC1 
#define ADC_CHANNEL_CURRENT_STACK1_PHASEB       ADC_CHANNEL_ADC2 
#define ADC_CHANNEL_CURRENT_STACK1_PHASEC       ADC_CHANNEL_ADC3 
#define ADC_CHANNEL_CURRENT_STACK2_PHASEA       ADC_CHANNEL_ADC4 
#define ADC_CHANNEL_CURRENT_STACK2_PHASEB       ADC_CHANNEL_ADC5 
#define ADC_CHANNEL_CURRENT_STACK2_PHASEC       ADC_CHANNEL_ADC6 
#define ADC_CHANNEL_VOLTAGE_DCLINK              ADC_CHANNEL_ADC13 
#define ADC_CHANNEL_VOLTAGE_2                   ADC_CHANNEL_ADC20

#define ADC_CHANNEL_CURRENT_DRIVER_PHASEA       ADC_CHANNEL_ADC7 
#define ADC_CHANNEL_CURRENT_DRIVER_PHASEB       ADC_CHANNEL_ADC8 
#define ADC_CHANNEL_CURRENT_DRIVER_PHASEC       ADC_CHANNEL_ADC9 
#define ADC_CHANNEL_TEMPERATURE_DRIVER_PHASEA   ADC_CHANNEL_ADC14
#define ADC_CHANNEL_TEMPERATURE_DRIVER_PHASEB   ADC_CHANNEL_ADC15 
#define ADC_CHANNEL_TEMPERATURE_DRIVER_PHASEC   ADC_CHANNEL_ADC16 
//#define ADC_CHANNEL_VOLTAGE_3                   ADC_CHANNEL_ADC?
//#define ADC_CHANNEL_VOLTAGE_4                   ADC_CHANNEL_ADC?

#define ADC_CHANNEL_CURR_STACK2_DRIVER_PHASEA   ADC_CHANNEL_ADC10 
#define ADC_CHANNEL_CURR_STACK2_DRIVER_PHASEB   ADC_CHANNEL_ADC11 
#define ADC_CHANNEL_CURR_STACK2_DRIVER_PHASEC   ADC_CHANNEL_ADC12 
#define ADC_CHANNEL_TEMP_STACK2_DRIVER_PHASEA   ADC_CHANNEL_ADC17
#define ADC_CHANNEL_TEMP_STACK2_DRIVER_PHASEB   ADC_CHANNEL_ADC18 
#define ADC_CHANNEL_TEMP_STACK2_DRIVER_PHASEC   ADC_CHANNEL_ADC19 

#else

#define ADC_CHANNEL_CURRENT_STACK1_PHASEA       ADC1 
#define ADC_CHANNEL_CURRENT_STACK1_PHASEB       ADC2 
#define ADC_CHANNEL_CURRENT_STACK1_PHASEC       ADC3 
#define ADC_CHANNEL_CURRENT_STACK2_PHASEA       ADC4 
#define ADC_CHANNEL_CURRENT_STACK2_PHASEB       ADC5 
#define ADC_CHANNEL_CURRENT_STACK2_PHASEC       ADC6 
#define ADC_CHANNEL_VOLTAGE_DCLINK              ADC13
#define ADC_CHANNEL_VOLTAGE_2                   ADC20

#define ADC_CHANNEL_CURRENT_DRIVER_PHASEA       ADC7 
#define ADC_CHANNEL_CURRENT_DRIVER_PHASEB       ADC8 
#define ADC_CHANNEL_CURRENT_DRIVER_PHASEC       ADC9 
#define ADC_CHANNEL_TEMPERATURE_DRIVER_PHASEA   ADC14 
#define ADC_CHANNEL_TEMPERATURE_DRIVER_PHASEB   ADC15 
#define ADC_CHANNEL_TEMPERATURE_DRIVER_PHASEC   ADC16 
//#define ADC_CHANNEL_VOLTAGE_3                   ADC?
//#define ADC_CHANNEL_VOLTAGE_4                   ADC?

#define ADC_CHANNEL_CURR_STACK2_DRIVER_PHASEA   ADC10 
#define ADC_CHANNEL_CURR_STACK2_DRIVER_PHASEB   ADC11 
#define ADC_CHANNEL_CURR_STACK2_DRIVER_PHASEC   ADC12 
#define ADC_CHANNEL_TEMP_STACK2_DRIVER_PHASEA   ADC17
#define ADC_CHANNEL_TEMP_STACK2_DRIVER_PHASEB   ADC18 
#define ADC_CHANNEL_TEMP_STACK2_DRIVER_PHASEC   ADC19 

#endif

/* *****************************************************************************
 * Configuration Definitions ADC
 **************************************************************************** */
#define ADC1_CHANNELS_MASK  \
    (uint32_t)( \
                ((uint32_t)1 << ADC_CHANNEL_CURRENT_STACK1_PHASEA)| \
                ((uint32_t)1 << ADC_CHANNEL_CURRENT_STACK1_PHASEB)| \
                ((uint32_t)1 << ADC_CHANNEL_CURRENT_STACK1_PHASEC)| \
                ((uint32_t)1 << ADC_CHANNEL_CURRENT_STACK2_PHASEA)| \
                ((uint32_t)1 << ADC_CHANNEL_CURRENT_STACK2_PHASEB)| \
                ((uint32_t)1 << ADC_CHANNEL_CURRENT_STACK2_PHASEC)| \
                ((uint32_t)1 << ADC_CHANNEL_VOLTAGE_DCLINK)| \
                ((uint32_t)1 << ADC_CHANNEL_VOLTAGE_2) )
             
#define ADC2_CHANNELS_MASK  \
    (uint32_t)( \
                ((uint32_t)1 << ADC_CHANNEL_CURRENT_DRIVER_PHASEA)| \
                ((uint32_t)1 << ADC_CHANNEL_CURRENT_DRIVER_PHASEB)| \
                ((uint32_t)1 << ADC_CHANNEL_CURRENT_DRIVER_PHASEC)| \
                ((uint32_t)1 << ADC_CHANNEL_CURR_STACK2_DRIVER_PHASEA)| \
                ((uint32_t)1 << ADC_CHANNEL_CURR_STACK2_DRIVER_PHASEB)| \
                ((uint32_t)1 << ADC_CHANNEL_CURR_STACK2_DRIVER_PHASEC)| \
                ((uint32_t)1 << ADC_CHANNEL_TEMPERATURE_DRIVER_PHASEA)| \
                ((uint32_t)1 << ADC_CHANNEL_TEMPERATURE_DRIVER_PHASEB)| \
                ((uint32_t)1 << ADC_CHANNEL_TEMPERATURE_DRIVER_PHASEC)| \
                ((uint32_t)1 << ADC_CHANNEL_TEMP_STACK2_DRIVER_PHASEA)| \
                ((uint32_t)1 << ADC_CHANNEL_TEMP_STACK2_DRIVER_PHASEB)| \
                ((uint32_t)1 << ADC_CHANNEL_TEMP_STACK2_DRIVER_PHASEC) )
             

#define AD1CSSH_MASK (uint16_t)(ADC1_CHANNELS_MASK >> 16)
#define AD1CSSL_MASK (uint16_t)(ADC1_CHANNELS_MASK)
#define AD1CSS_COUNT 8

#define AD2CSSH_MASK (uint16_t)(ADC2_CHANNELS_MASK >> 16)
#define AD2CSSL_MASK (uint16_t)(ADC2_CHANNELS_MASK)
#define AD2CSS_COUNT 12




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
