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

#include "boardcfg.h"   /* might not needed because included in sys_drv.h */
#include "sys_drv.h"    /* boardcfg.h included here */


#if BOARDCFG_USE_ANDREAS_INTERFACE_RTC == 0
#else
#include "sys_rtc.h"
#endif


/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define HMDRV_USE_CSMON                     1

//#define NO_FREEMASTER

#ifndef HMDRV_USE_FREEMASTER
#ifndef NO_FREEMASTER
#define HMDRV_USE_FREEMASTER                1
#else
#define HMDRV_USE_FREEMASTER                0
#endif
#endif

#define HMDRV_USE_EXT_MEM                   0
#define HMDRV_USE_EXT_SPI                   0
#define HMDRV_USE_EXT_I2C                   0
#define HMDRV_USE_EXT_PMP                   0
#define HMDRV_USE_MODBUS                    1
#define HMDRV_USE_RECORDER                  0 /* DPN was 1 */

#define HMDRV_USE_RECORDER_PARAMETERS       0 /* DPN was 1 */
#define HMDRV_USE_SCOPE_PARAMETERS          0 /* DPN was 1 */


#define HMDRV_FLOAT_TRIG_ENABLE             1


#ifndef HMDRV_USE_DEBUG_PIN
#define HMDRV_USE_DEBUG_PIN                 0
#endif

/* UART Init Configuration */

#ifndef _HMDRV_USE_UART_INIT
#define HMDRV_USE_UART_INIT   0      /* Default */      /* For Now Only Support Both HexMon and Modbus on the same port */
#else
#define HMDRV_USE_UART_INIT   1      /* For Now Only Support Both HexMon and Modbus on the same port */
#endif

#ifdef BOARDCFG_ADDR_WIDTH
#define HMDRV_ADDR_WIDTH    BOARDCFG_ADDR_WIDTH
#else
#define HMDRV_ADDR_WIDTH    1
#endif

#if _LAUNCHXL_F28379D
#define HMDRV_USE_BOARDLAUNCHPADXLTMS320F28379D
#elif defined (_LAUNCHXL_F28069M)
#define HMDRV_USE_BOARDLAUNCHPADXLTMS320F28069M
#else
#define HMDRV_USE_BOARDSCC
#endif

/* UART Init Configuration END */


#define HMDRV_USE_CONTROL_SUITE_LIB     0
#define HMDRV_USE_C2000_WARE_LIB        1
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




#if 0       /* obsolete */
#if HMDRV_USE_SCIA
#define HMDRV_USE_SCIB 0
#define HMDRV_SCI_BASE                      SCIA_BASE
#else
#define HMDRV_USE_SCIB 1
#define HMDRV_SCI_BASE                      SCIB_BASE
#endif
#endif


#if (HMLIB_HEXMON_UART_MODULE == UART_A)
#define HMDRV_SCI_BASE_HEXMON               SCIA_BASE
#else
#define HMDRV_SCI_BASE_HEXMON               SCIB_BASE
#endif

#if (HMLIB_MODBUS_UART_MODULE == UART_A)
#define HMDRV_SCI_BASE_MODBUS               SCIA_BASE
#else
#define HMDRV_SCI_BASE_MODBUS               SCIB_BASE
#endif



#if (HMLIB_HEXMON_UART_MODULE == UART_A) || (HMLIB_MODBUS_UART_MODULE == UART_A)
#define HMDRV_USE_SCIA 1
#else
#define HMDRV_USE_SCIA 0
#endif

#if (HMLIB_HEXMON_UART_MODULE == UART_B) || (HMLIB_MODBUS_UART_MODULE == UART_B)
#define HMDRV_USE_SCIB 1
#else
#define HMDRV_USE_SCIB 0
#endif






#ifdef HMDRV_HEXMON_BAUD
#ifndef HMDRV_SCI_BAUD
#define HMDRV_SCI_BAUD                      HMDRV_HEXMON_BAUD
#endif
#endif


#define HMDRV_PARITY_MODBUS         UART_PARITY_MODBUS
#define HMDRV_PARITY_HEXMON         UART_PARITY_HEXMON
#define HMDRV_STOP_BITS             UART_STOP_BITS



#ifdef _FLASH
#define HMDRV_MAX_REC_PARAM_VARS  20
#define HMDRV_MAX_SCOPE_PARAM_VARS  20
#define HMDRV_MAX_REC_VARS                  20
#define HMDRV_MAX_SCOPE_VARS                20
#else
#define HMDRV_MAX_REC_PARAM_VARS  8
#define HMDRV_MAX_SCOPE_PARAM_VARS  8
#define HMDRV_MAX_REC_VARS                  8
#define HMDRV_MAX_SCOPE_VARS                8
#endif



/* Use Byte address instead of real address value (real address value * 2) */
#define HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY      1

#define HMDRV_LARGE_MODEL                   1       /* 32 Bit Address Support */

//#define HMDRV_BIG_TO_LITTLE_ENDIAN          1
#define HMDRV_BIG_TO_LITTLE_ENDIAN          0

#define HMDRV_RECORDER_HANDLE_COUNT         3
#define HMDRV_RECORDER_BUFFER_COUNT         3


#define HMDRV_RECORDER_PARAM_HANDLE_COUNT   3

#ifdef _FLASH
#define HMDRV_RECORDER_PARAM_BUFFER_COUNT   3
#define HMDRV_RECORDER_PARAM_BUFINT_COUNT   3
#else
#define HMDRV_RECORDER_PARAM_BUFFER_COUNT   1
#define HMDRV_RECORDER_PARAM_BUFINT_COUNT   1
#endif


#ifndef BOARDCFG_EMIF_ADDRESS_START
#define HMDRV_EMIF_ADDRESS_START            0x00100000                    /* TI 16-bit Memory address */
#else
#define HMDRV_EMIF_ADDRESS_START            BOARDCFG_EMIF_ADDRESS_START /* TI 16-bit Memory address */
#endif
#ifndef BOARDCFG_EMIF_SIZE_BYTES
#define HMDRV_EMIF_SIZE_BYTES               0x00200000                    /* 1M * 16-bit (2Mb) */
#else
#define HMDRV_EMIF_SIZE_BYTES               BOARDCFG_EMIF_SIZE_BYTES    /* 1M * 16-bit (2Mb) */
#endif


#ifdef BOADRCFG_CSMON_PARAM_BUFF_RW_ADDR    /* Recorder and Parameter Recorder EMIF Addresses if CSMON Uses EMIF For Data Storage  */


#define HMDRV_RECORDER_PARAM_BUFFER0_SIZE               (BOADRCFG_CSMON_PARAM_BUFF_RW_SIZE / 3)        /* Size In bytes */
#define HMDRV_RECORDER_PARAM_BUFFER1_SIZE_FREEMASTER    0x0000FFFF        /* Size In bytes (compatible with Freemaster supports only 16-bits size) 512kB(0x80000) leave 0xFFFF+1 for HMDRV_RECORDER_BUFFER1 */
#define HMDRV_RECORDER_PARAM_BUFFER1_SIZE               HMDRV_RECORDER_PARAM_BUFFER0_SIZE        /* Size In bytes */
#define HMDRV_RECORDER_PARAM_BUFFER2_SIZE               (HMDRV_RECORDER_PARAM_BUFFER0_SIZE - 0x10000)        /* Size In bytes of the last less than others to leave space for the freemaster recorder */

#define HMDRV_RECORDER_PARAM_BUFFER0_ADDR               (HMDRV_ADDR)(BOADRCFG_CSMON_PARAM_BUFF_RW_ADDR)
#define HMDRV_RECORDER_PARAM_BUFFER1_ADDR               (HMDRV_ADDR)(HMDRV_RECORDER_PARAM_BUFFER0_ADDR + (HMDRV_RECORDER_PARAM_BUFFER0_SIZE/HMDRV_ADDR_WIDTH) )
#define HMDRV_RECORDER_PARAM_BUFFER2_ADDR               (HMDRV_ADDR)(HMDRV_RECORDER_PARAM_BUFFER1_ADDR + (HMDRV_RECORDER_PARAM_BUFFER1_SIZE/HMDRV_ADDR_WIDTH) )
#define HMDRV_RECORDER_BUFFER1_ADDR                     (HMDRV_ADDR)(HMDRV_RECORDER_PARAM_BUFFER2_ADDR + (HMDRV_RECORDER_PARAM_BUFFER2_SIZE/HMDRV_ADDR_WIDTH) )



#else

#ifdef _FLASH
#define HMDRV_RECORDER_PARAM_BUFFER0_SIZE               0x00040000        /* Size In bytes (256kB) */
#define HMDRV_RECORDER_PARAM_BUFFER1_SIZE_FREEMASTER    0x0000FFFF        /* Size In bytes (compatible with Freemaster supports only 16-bits size) 512kB(0x80000) leave 0xFFFF+1 for HMDRV_RECORDER_BUFFER1 */
#define HMDRV_RECORDER_PARAM_BUFFER1_SIZE               0x00040000        /* Size In bytes (256kB) */
#define HMDRV_RECORDER_PARAM_BUFFER2_SIZE               0x00040000        /* Size In bytes (256kB) */
#else
#define HMDRV_RECORDER_PARAM_BUFFER0_SIZE   0x100
#define HMDRV_RECORDER_PARAM_BUFFER1_SIZE_FREEMASTER    0x100       /* Size In bytes (compatible with Freemaster supports only 16-bits size) 512kB(0x80000) leave 0xFFFF+1 for HMDRV_RECORDER_BUFFER1 */
#define HMDRV_RECORDER_PARAM_BUFFER1_SIZE   0x100
#define HMDRV_RECORDER_PARAM_BUFFER2_SIZE   0x100
#endif


#define HMDRV_RECORDER_PARAM_BUFFER0_ADDR               (HMDRV_ADDR)(HMDRV_EMIF_ADDRESS_START)
#define HMDRV_RECORDER_PARAM_BUFFER1_ADDR               (HMDRV_ADDR)(HMDRV_RECORDER_PARAM_BUFFER0_ADDR + (HMDRV_RECORDER_PARAM_BUFFER0_SIZE/HMDRV_ADDR_WIDTH) )
#define HMDRV_RECORDER_PARAM_BUFFER2_ADDR               (HMDRV_ADDR)(HMDRV_RECORDER_PARAM_BUFFER1_ADDR + (HMDRV_RECORDER_PARAM_BUFFER1_SIZE/HMDRV_ADDR_WIDTH) )
#define HMDRV_RECORDER_BUFFER1_ADDR                     (HMDRV_ADDR)(HMDRV_RECORDER_PARAM_BUFFER2_ADDR + (HMDRV_RECORDER_PARAM_BUFFER2_SIZE/HMDRV_ADDR_WIDTH) )

#endif


#define HMDRV_SCOPE_PARAM_HANDLE_COUNT      3

#define HMDRV_FREEMASTER_SCOPE_PARAM_INDEX  0

#define HMDRV_RECORDER_TIME_BASE_COUNT      1

#define HMDRV_RECORDER_USED_FREEMASTER      1

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

#define HMDRV_BOARD                         HMDRV_BOARD_SCC_Delfino
#define HMDRV_IDT_STRING                    HMDRV_STRING_SCC_Delfino
//#define HMREC_EXECUTE_NANOSEC               100
//#define HMREC_EXECUTE_MICROSEC                1
#define HMREC_EXECUTE_MICROSEC              400
#define HMDRV_FREE_RUN_TIMER_FREQ_MHZ       (FCY_HZ/1000000.0)
#define HMDRV_FREE_RUN_TIMER_DECREMENT_COUNTER  1
#endif  /* HMCFG_H */
