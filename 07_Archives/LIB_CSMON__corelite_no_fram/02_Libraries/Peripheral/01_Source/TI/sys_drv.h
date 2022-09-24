/* *****************************************************************************
 * File:   sys_drv.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 11 23 13:21
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
#ifndef SYS_DRV_H
#define	SYS_DRV_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "boardcfg.h"
#include "sci_drv.h"
#include "epwm_drv.h"
#include "adc_drv.h"
#include "io_drv.h"
#include "spi_drv.h"
#include "emif_drv.h"
#include "tmr_drv.h"
#include "uart_drv.h"
#include "dac_drv.h"
#include "fpga_drv.h"
#include "fpga_sci.h"
#include "rtc_drv.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define GPIO_CORE_CPUA          GPIO_CORE_CPU1


#define USE_CPU2_FROM_CPU1      1
#if USE_CPU2_FROM_CPU1
#define GPIO_CORE_CPUB          GPIO_CORE_CPU2
#define SYSCTL_CPUSEL_CPUB      SYSCTL_CPUSEL_CPU2
#define MEMCFG_GSRAMMASTER_CPUB MEMCFG_GSRAMMASTER_CPU2
#endif




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

/* Macro String Concatenation - consider move to private library */
#define STRING_CONCAT_BASE(_a_, _b_) _a_##_b_
#define STRING_CONCAT(_a_, _b_) STRING_CONCAT_BASE(_a_, _b_)



/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 
extern UART_sModuleConfiguration_t sModBusUartModuleConfiguration;
extern UART_sModuleConfiguration_t sHexMonUartModuleConfiguration;




/* *****************************************************************************
 * Extern Function Prototypes
 **************************************************************************** */
void InitEPwm1Gpio(void);
void InitEPwm2Gpio(void);
void InitEPwm3Gpio(void);
void InitEPwm4Gpio(void);
void InitEPwm5Gpio(void);
void InitEPwm6Gpio(void);
void InitEPwm7Gpio(void);
void InitEPwm8Gpio(void);
void InitEPwm9Gpio(void);
void InitEPwm10Gpio(void);
void InitEPwm11Gpio(void);
void InitEPwm12Gpio(void);

void InitEPwmGpio(void);



/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void SYS_InitGPIO(void);






#endif	/* SYS_DRV_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
