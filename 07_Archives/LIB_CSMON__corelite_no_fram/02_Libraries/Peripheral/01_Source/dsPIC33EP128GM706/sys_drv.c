/* *****************************************************************************
 * File:   sys_drv.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 04 04 16:31
 * 
 * Description: System Initialization
 * 
 **************************************************************************** */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "boardcfg.h"

#include "sys_drv.h"

#include "i2c_drv.h"
#include "spi_drv.h"
#include "pin_drv.h"
#include "isr_drv.h"
#include "tmr_drv.h"
#include "adcpwm_drv.h"


#include "pin.h"
#if USE_CONTROL
#include "adcpwm.h"
#endif
#include "uart_drv.h"


#if USE_CONTROL
#include "control.h"
#endif

#include "i2cmem.h"
#include "spimem.h"

#if USE_SPI_DAC
#include "spidac.h"
#endif

#include "hmlib.h"


#include "modbusreg.h"
#include "timer.h"

#if HMDRV_USE_EXT_PMP
#include "pmpmem.h"
#endif


/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
bool bSystemFirstInit = true;



/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */


/* *****************************************************************************
 * Functions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */



/* *****************************************************************************
 * SYSTEM_vInit
 * 
 * Description: SYSTEM Initialization. 
 * Init the peripherals: PLL, ADC, PWM & GPIO         
 **************************************************************************** */
void SYS_DRV_vInit(void)
{
    CLK_DRV_vInit();

    ISR_DRV_vInit();
    
    PIN_DRV_vInit();

    TMR_DRV_vInit();
    
    #if USE_CONTROL
    
    ADCPWM_DRV_vInit();
    
    if (bSystemFirstInit)
    {
        bSystemFirstInit = false;
        CONTROL_vInitOnce();
    }
    
    CONTROL_vInit();    /* must be after ADCPWM_vStackModeInit inside is trigger initialization */
    
    #endif
    
    UART_DRV_vInit();
    
    SPI_DRV_vInit();

    #if HMDRV_USE_EXT_PMP
    PMPMEM_vInit();
    #endif

    I2C_DRV_vInit();
    
    HMLIB_vInit();
    MODBUSREG_vInit();
    
    #ifdef BOARDCFG_USE_MCC
    INTERRUPT_GlobalEnable();
    SYSTEM_CORCONModeOperatingSet(CORCON_MODE_PORVALUES);
    #else
    __builtin_enable_interrupts();
    #endif
    
}


/* *****************************************************************************
 * SYSTEM_vProcess
 * 
 * Description: SYSTEM Re-Initialization Process. 
 * due to clock change or other major configuration change
 **************************************************************************** */
void SYS_DRV_vProcess(void)
{
    
    //to do : make when uart only changes not to reset whole chip
    
    if ( CLK_DRV_bClockChangeRequest()
       || ADCPWM_DRV_vConfigurationChangeRequest()     
       || UART_DRV_bConfigurationChangeRequest()   
       )
    {
        SYS_DRV_vInit();
    }

    if ( UART_DRV_bConfigurationChangeRequest())
    {
        UART_DRV_vInit();
    }

}


