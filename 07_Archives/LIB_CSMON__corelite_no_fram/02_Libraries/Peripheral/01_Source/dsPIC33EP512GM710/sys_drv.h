/* *****************************************************************************
 * File:   sys_drv.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 04 04 16:31
 * 
 * Description: System Initialization
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef SYS_DRV_H
#define	SYS_DRV_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "mcc_cfg.h"

#ifdef BOARDCFG_USE_MCC
#ifndef BOARDCFG_USE_MCC_PIN
#define _PIN_MANAGER_H          /* Force exclude from use pin_manager.h  */
#endif
#include "mcc.h"
#endif

#include "boardcfg.h"
#include "pin_macro.h" 

#include "clk_drv.h"

#include "adcpwm.h"
#include "spimem.h"
#include "pmpmem.h"
#include "i2cmem.h"

#include "isr.h"
#include "control.h"
#include "dio.h"
#include "fpga_dio.h"
#include "fpga_pwm.h"
#include "fpga_trip.h"
#include "dac_trip.h"

#include "spidac.h"
#include "testproc.h"

#include "uart_drv.h"
#include "hmlib.h"
#include "modbusreg.h"

#include "csmon.h"

#include "ctreader.h"
#include "rtc_drv.h"

#if USE_SPI_FPGA_NO_MEM
#include "fpga_spi.h"
#endif

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Constants Definitions
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
extern UART_eModule_t SYSTEM_eUartModBus;
extern UART_eModule_t SYSTEM_eUartHexMon;

extern UART_sModuleConfiguration_t sModBusUartModuleConfiguration;
extern UART_sModuleConfiguration_t sHexMonUartModuleConfiguration;

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void SYS_DRV_vInit(void);
void SYS_DRV_vProcess(void);

#endif	/* SYS_DRV_H */
