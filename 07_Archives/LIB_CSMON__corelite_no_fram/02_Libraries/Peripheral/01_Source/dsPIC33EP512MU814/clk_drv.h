/* *****************************************************************************
 * File:   clk_drv.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 04 04 16:12
 * 
 * Description: System Clock Initialization
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef CLK_DRV_H
#define	CLK_DRV_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "boardcfg.h"
#include "clk.h"
#include "clock.h"

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


/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
#ifndef BOARDCFG_USE_MCC_CLK
#define CLK_DRV_vInit()     CLK_vInit()
#define CLK_DRV_u32GetFcy() CLK_u32GetFcy()
#define CLK_DRV_u32GetFpb() CLK_u32GetFpb()
#define CLK_DRV_bClockChangeRequest()   CLK_bClockChangeRequest()
#else
#define CLK_DRV_vInit()     CLOCK_Initialize();
#define CLK_DRV_u32GetFcy() CLOCK_PeripheralFrequencyGet()  /* Consider here used Fpb */
#define CLK_DRV_u32GetFpb() CLOCK_PeripheralFrequencyGet()
#define CLK_DRV_bClockChangeRequest()   false
#endif

#endif	/* CLK_H */

