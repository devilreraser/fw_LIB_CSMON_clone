/* *****************************************************************************
 * File:   mcc_cfg.h
 * Author: Dimitar Lilov
 *
 * Created on 2020 04 17 17:39
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
#ifndef MCC_CFG_H
#define	MCC_CFG_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
    
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
    
    
/* *****************************************************************************
 * Driver Configuration Definitions - mainly MCC Configuration Usage
 **************************************************************************** */
#define BOARDCFG_USE_MCC            /* Comment this not to use MCC */      

#ifdef BOARDCFG_USE_MCC             /* If MCC Used Enable Below Modules Used */

#define BOARDCFG_USE_MCC_CONFIG     /* Include for build mcc.c */

#define BOARDCFG_USE_MCC_CLK        /* Include for build clock.c (also system.c) */

#define BOARDCFG_USE_MCC_PIN        /* Include for build pin_manager.c (also system.c) */

#define BOARDCFG_USE_MCC_TMR        /* Include for build timerx.c (also system.c) */

#define BOARDCFG_USE_MCC_PWM        /* Include for build pwm.c (also system.c) */ 

#ifdef BOARDCFG_USE_MCC_PWM
#define BOARDCFG_USE_MCC_ADC        /* Include for build adcx.c (also system.c) BOARDCFG_USE_MCC_PWM must be defined !!! */ 
#endif

#define BOARDCFG_USE_MCC_UART       /* Include for build uartx.c (also system.c) */

#define BOARDCFG_USE_MCC_I2C        /* Include for build i2cx.c (also system.c) */

#define BOARDCFG_USE_MCC_SPI        /* Include for build spix.c (also system.c) */ //- not tested

#endif

#define BOARDCFG_USE_OLD_NON_MCC_ADC    0 
#define BOARDCFG_USE_OLD_NON_MCC_PWM    0 








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
 
#endif	/* MCC_CFG_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
