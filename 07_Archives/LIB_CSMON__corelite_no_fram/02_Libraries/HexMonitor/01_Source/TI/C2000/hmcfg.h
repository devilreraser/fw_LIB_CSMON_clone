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
#define HMCFG_H

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define HMDRV_USE_EXT_MEM                   0
#define HMDRV_USE_EXT_SPI                   0
#define HMDRV_USE_EXT_I2C                   0
#define HMDRV_USE_EXT_PMP                   0
#define HMDRV_USE_MODBUS                    0
#define HMDRV_USE_RECORDER                  1


#define HMDRV_FLOAT_TRIG_ENABLE             1


#define HMDRV_USE_UART_INIT                 1


#define HMDRV_USE_BOARDLAUNCHPADXLTMS320F28069M
//#define HMDRV_USE_BOARDSCC

#define HMDRV_USE_SCIA 1        /* if 0 used SCIB */

#define HMDRV_USE_CONTROL_SUITE_LIB     1
#define HMDRV_USE_C2000_WARE_LIB        0
#define HMDRV_USE_C2000_WARE_LIB_DEVICE_SUPPORT        0

#if defined CPU1
#if HMDRV_USE_SCIA
#define HMDRV_SCI_BASE                      SCIA_BASE
#else
#define HMDRV_SCI_BASE                      SCIB_BASE
#endif

#define HMDRV_SCI_BAUD                      460800
#elif defined CPU2

#if HMDRV_USE_SCIA
#define HMDRV_SCI_BASE                      SCIA_BASE
#else
#define HMDRV_SCI_BASE                      SCIB_BASE
#endif

#define HMDRV_SCI_BAUD                      460800
#else
/* USE defaults From hmdrv.h */
#endif







/* Use Byte address instead of real address value (real address value * 2) */
#define HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY      1

#define HMDRV_LARGE_MODEL                   1       /* 32 Bit Address Support */

//#define HMDRV_BIG_TO_LITTLE_ENDIAN          1
#define HMDRV_BIG_TO_LITTLE_ENDIAN          0

#define HMDRV_RECORDER_HANDLE_COUNT         3
#define HMDRV_RECORDER_BUFFER_COUNT         3


#define HMDRV_SCI_RX_ISR                    0
#define HMDRV_SCI_TX_ISR                    0
#define HMDRV_SCI_TX_DELAY_ISR              0
#define HMDRV_SCI_READ_MULTIPLE_RX_DATA     0

#include "boardcfg.h"

#ifdef HMDRV_USE_EXT_MEM
#if HMDRV_USE_EXT_MEM
#include "extmemcfg.h"
#endif
#endif


#if (defined (HMDRV_SCI_RX_ISR) && HMDRV_SCI_RX_ISR) || (defined (HMDRV_SCI_TX_ISR) && HMDRV_SCI_TX_ISR)
#include "uart.h"
#endif

#if defined (HMDRV_SCI_TX_DELAY_ISR) && HMDRV_SCI_TX_DELAY_ISR
#include "timer.h"
#endif

#define HMDRV_BOARD                         HMDRV_BOARD_SCC_Delfino
#define HMDRV_IDT_STRING                    HMDRV_STRING_SCC_Delfino
#define HMREC_EXECUTE_MICROSEC              400

//#define HMDRV_FREE_RUN_TIMER_FREQ_MHZ       (FCY_HZ/1000000.0)

#endif  /* HMCFG_H */
