/* *****************************************************************************
 * File:   fpga_dio.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 08 14 12:51
 * 
 * Description: FPGA DIO (Digital Inputs Outputs)
 * 
 **************************************************************************** */
#ifdef _WIN32
#pragma once
#endif /* _WIN32 */

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "boardcfg.h"

#if BOARDCFG_USE_FPGA_DIO
#include "sys_drv.h"

#include "fpga_cfg.h"
#include "fpga_common.h"
#include "fpga_dio.h"
#include "control_common.h"
#include "hmlib.h"
#if HMDRV_USE_EXT_PMP
#include "pmpmem.h"
#endif



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

        
uControlIO_t uDOExternal = {0};
uControlIO_t uDIExternal;
uControlIO_t uDOExternalFbk;
uControlIO_t uDIExternalFbk;

uint8_t u8PMPConfigurationUnlockKeyDIO = PMP_CONFIGURATION_UNLOCK_KEY;
uint8_t u8PMPConfigurationLockKeyDIO   = 0x00;

bool bSetupIOConfiguration = 0;


/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */

void FPGA_DIO_vIOInit (void)
{
    uDOExternal.u16Register = 0;    
}


void FPGA_DIO_vRunTimeSetupProcess(void)
{
    //if ( )
    {
        bSetupIOConfiguration = 1;
    }

}

void FPGA_DIO_vGetIOConfiguration(void)
{
    
    
}


void FPGA_DIO_vIOConfigure (void)
{
    if (bUsePMPwithControl)
    {  
        if (bSetupIOConfiguration)
        {
            bSetupIOConfiguration = 0;

        }
    }
    
}
void FPGA_DIO_vIOProcess (void)
{
    
    uDIExternal.u16Register = uDIExternalFbk.u16Register;     
}





#if FPGA_CFG_USE_DIDO_FPGA == 0

void CONTROL_vIOProcess (void)
{
    uDIExternal.u16Register = uDIExternalFbk.u16Register;  
    
}

#endif /* #if USE_DIDO_FPGA == 0 */


#endif /* #if USE_FPGA_DIO */




