/* *****************************************************************************
 * File:   projectcfg.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 04 09 13:13
 * 
 * Description: Project Configuration for CSMON Test
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef PROJECTCFG_H
#define	PROJECTCFG_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */

//needed for READ_ONLY PARAMETER TABLE (_PARAM_LIST_READ_ONLY). For consistency of the data match projectcfg.h in library project
#define PROJECTCFG_HMDRV_HEXMON_BAUD            460800
#define PROJECTCFG_HMDRV_MODBUS_BAUD            460800

#define PROJECTCFG_STOP_BITS                    SCI_CONFIG_STOP_ONE  //!< One stop bit //     SCI_CONFIG_STOP_TWO   //!< Two stop bits //

#define PROJECTCFG_UART_PARITY_HEXMON           SCI_CONFIG_PAR_NONE
#define PROJECTCFG_UART_PARITY_MODBUS           SCI_CONFIG_PAR_NONE


#ifdef _CSMON_FREE_RUN_TIMER_EXTERNAL
#define PROJECTCFG_FREE_RUN_TIMER_EXTERNAL 1
#else
#define PROJECTCFG_FREE_RUN_TIMER_EXTERNAL 0
#endif

#ifdef _PARAM_LIST_READ_ONLY
#define PROJECTCFG_PARAM_LIST_READ_ONLY 1
#else
#define PROJECTCFG_PARAM_LIST_READ_ONLY 0
#endif


/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

#endif	/* PROJECTCFG_H */
