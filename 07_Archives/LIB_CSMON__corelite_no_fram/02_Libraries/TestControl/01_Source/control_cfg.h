/* *****************************************************************************
 * File:   control_cfg.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 08 14 14:18
 * 
 * Description: Control configuration of Two 3-Phase Inverters 
 * 
 **************************************************************************** */

/* Guard condition file contents not included more than once */  
#ifndef CONTROL_CFG_H
#define	CONTROL_CFG_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "boardcfg.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* Application Definitions */
#define USE_AUTOMATIC_RUN_REQUEST       0
#define CONTROL_CFG_USE_VOLTAGE_FBCK_EMULATION              0
#define CONTROL_CFG_USE_CURRENT_FBCK_EMULATION              0
#define CONTROL_USE_EMULATION_POSITION_FROM_GENERATION_POSITION 1



#if CURRENT_FEEDBACK_FIX_MULT_SHIFT_CONTROL == 0        /* in boardcfg.h */
#define CONTROL_CFG_USE_CALC_ADC_FIX                        0      
#else
#define CONTROL_CFG_USE_CALC_ADC_FIX                        1
#endif

/* CFBK For Test Stand and DCLI for SCC */
#if BOARDCFG_USE_TSC_DSPIC
#define CONTROL_CFG_USE_CALC_RMS_ABC                        0
#define CONTROL_CFG_USE_CALC_ADC_MIN_MAX                    0
#define CONTROL_CFG_USE_CALC_CFBK_DCLI_RMS_ABC              0
#define CONTROL_CFG_USE_CALC_ADC_AVG                        0
#define CONTROL_CFG_USE_CALC_ADC_AVG_INIT_RUNTIME           0
#define CONTROL_CFG_USE_CALC_RMS_DC_LINK                    0

#define CONTROL_CFG_USE_3_SENSOR_FBK_STACK1_CONTROL         0

#else
#define CONTROL_CFG_USE_CALC_RMS_ABC                        1
#define CONTROL_CFG_USE_CALC_RMS_DC_LINK                    1
#define CONTROL_CFG_USE_CALC_ADC_MIN_MAX                    1
#define CONTROL_CFG_USE_CALC_CFBK_DCLI_RMS_ABC              1
#define CONTROL_CFG_USE_CALC_ADC_AVG                        1
#define CONTROL_CFG_USE_CALC_ADC_AVG_INIT_RUNTIME           1

#define CONTROL_CFG_USE_3_SENSOR_FBK_STACK1_CONTROL         1

#endif


#define CONTROL_CFG_USE_PI_CONTROLLER                       1

/* Calibration */
#if BOARDCFG_USE_TSC_DSPIC
#define CONTROL_CFG_USE_WRONG_OFFSET_VOLTAGE_DETECTION      1
#else
#define CONTROL_CFG_USE_WRONG_OFFSET_VOLTAGE_DETECTION      0
#endif


/* Dead Time */
#define USE_DEADTIME_MCU    0
#define USE_DEADTIME_FPGA   1


#if USE_DEADTIME_MCU
#define DEADTIME_MCU_NSEC   4500                /* DeadTime in nSec */
#else
#define DEADTIME_MCU_NSEC   0               
#endif

#if USE_DEADTIME_FPGA
#define DEADTIME_FPGA_NSEC  4500                /* DeadTime in nSec */
#else
#define DEADTIME_FPGA_NSEC  0
#endif


#define MINPULSE_FPGA_NSEC  2500                /* Min Pulse in nSec */


/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */
#define NANOSECOND_CLOCK_FREQ_HZ             (1000000000.0F)      /* 1 GHz */

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

#endif	/* CONTROL_CFG_H */
