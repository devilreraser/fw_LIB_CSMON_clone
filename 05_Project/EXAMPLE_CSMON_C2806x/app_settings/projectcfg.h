/* *****************************************************************************
 * File:   projectcfg.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 04 09 13:13
 * 
 * Description: Project Configuration for CSMON Test
 * 
 **************************************************************************** */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Guard condition file contents not included more than once */  
#ifndef PROJECTCFG_H
#define	PROJECTCFG_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */

#include "sci.h"


/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

//needed for READ_ONLY PARAMETER TABLE (_PARAM_LIST_READ_ONLY). For consistency of the data match projectcfg.h in library project
#define EXAMPLE_CSMON_C2806x_HMDRV_MODBUS_BAUD            460800

#define EXAMPLE_CSMON_C2806x_STOP_BITS                    SCI_CONFIG_STOP_ONE  //!< One stop bit //     SCI_CONFIG_STOP_TWO   //!< Two stop bits //

#define EXAMPLE_CSMON_C2806x_UART_PARITY_MODBUS           SCI_CONFIG_PAR_EVEN


#endif	/* PROJECTCFG_H */

#ifdef __cplusplus
}
#endif /* __cplusplus */
