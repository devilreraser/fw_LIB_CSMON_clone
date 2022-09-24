/* *****************************************************************************
 * File:   hmcfg.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 02 28 21:41
 * 
 * Description: Hex Monitor Configuration
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef HMCFG_H
#define	HMCFG_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "boardcfg.h"
#include "rtc_drv.h"
#include "tmr_drv.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

#define HMDRV_USE_CSMON                     1

#define HMDRV_USE_EXT_MEM                   1
#define HMDRV_USE_EXT_SPI                   1
#define HMDRV_USE_EXT_I2C                   1
#define HMDRV_USE_EXT_PMP                   1
#define HMDRV_USE_MODBUS                    1
#define HMDRV_USE_RECORDER                  1

#if BOADRCFG_VARIANT == BOADRCFG_VARIANT_NO_EXT_MEM
#define HMDRV_USE_BOARD_WITHOUT_EXT_MEM     1       /* set to 1 when Used Old TSC Board in order the recorder to use RAM of the MCU */
#elif BOADRCFG_VARIANT == BOADRCFG_VARIANT_EXT_MEM
#define HMDRV_USE_BOARD_WITHOUT_EXT_MEM     0       
#endif

#define HMDRV_USE_RECORDER_PARAMETERS       1
#define HMDRV_USE_SCOPE_PARAMETERS          1

#define HMDRV_MAX_REC_VARS                  20
#define HMDRV_MAX_SCOPE_VARS                20


#define HMDRV_LARGE_MODEL                   1       /* 32 Bit Address Support */

#define HMDRV_RECORDER_HANDLE_COUNT         3
#define HMDRV_RECORDER_BUFFER_COUNT         3


#define HMDRV_RECORDER_PARAM_HANDLE_COUNT   3
#define HMDRV_RECORDER_PARAM_BUFFER_COUNT   3


#define HMDRV_SCOPE_PARAM_HANDLE_COUNT      3

#define HMDRV_FREEMASTER_SCOPE_PARAM_INDEX  0

#define HMDRV_RECORDER_TIME_BASE_COUNT      1


#define HMDRV_MODBUS_SHARED_RX_TX_BUFFER    0

#define HMDRV_SCI_RX_ISR                    1
#define HMDRV_SCI_TX_ISR                    1
#define HMDRV_SCI_TX_DELAY_ISR              1
#define HMDRV_SCI_READ_MULTIPLE_RX_DATA     1

#ifdef HMDRV_USE_EXT_MEM
#if HMDRV_USE_EXT_MEM
#include "extmemcfg.h"
#endif
#endif


#if (defined (HMDRV_SCI_RX_ISR) && HMDRV_SCI_RX_ISR) || (defined (HMDRV_SCI_TX_ISR) && HMDRV_SCI_TX_ISR)
#include "uart_drv.h"
#endif

#if defined (HMDRV_SCI_TX_DELAY_ISR) && HMDRV_SCI_TX_DELAY_ISR
#include "timer.h"
#endif

#define HMDRV_BOARD                         HMDRV_BOARD_TSC_dsPIC33
#define HMDRV_IDT_STRING                    HMDRV_STRING_TSC_dsPIC33
#define HMREC_EXECUTE_MICROSEC              400

#define HMRECPRM_EXECUTE_MICROSEC           400

#define HMDRV_FREE_RUN_TIMER_FREQ_MHZ       (FCY_HZ/1000000.0)

#endif	/* HMCFG_H */
