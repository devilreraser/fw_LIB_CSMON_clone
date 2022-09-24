/* *****************************************************************************
 * File:   fpga_dio.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 08 14 12:51
 * 
 * Description: FPGA DIO (Digital Inputs Outputs)
 * 
 **************************************************************************** */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Guard condition file contents not included more than once */  
/* #pragma once */
#ifndef FPGA_DIO_H
#define	FPGA_DIO_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdbool.h>
#include <stdint.h>
    
#include "boardcfg.h"    
#include "dio.h"    
    
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
 * Function-Like Macro
 **************************************************************************** */
#if BOARDCFG_USE_FPGA_DIO
/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 
extern uControlIO_t uDOExternal;
extern uControlIO_t uDIExternal;
extern uControlIO_t uDOExternalFbk;
extern uControlIO_t uDIExternalFbk;

#define u16DOExternal uDOExternal.u16Register

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void FPGA_DIO_vIOInit (void);
void FPGA_DIO_vIOProcess (void);
void FPGA_DIO_vIOConfigure (void);
void FPGA_DIO_vRunTimeSetupProcess(void);
void FPGA_DIO_vGetIOConfiguration(void);

#else

#define FPGA_DIO_vIOInit()
#define FPGA_DIO_vIOProcess()
#define FPGA_DIO_vIOConfigure()
#define FPGA_DIO_vRunTimeSetupProcess()
#define FPGA_DIO_vGetIOConfiguration()

#endif


#endif	/* FPGA_DIO_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
