/* *****************************************************************************
 * File:   ctreader.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 09 22 14:06
 * 
 * Description: ...
 * 
 **************************************************************************** */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Guard condition file contents not included more than once */  
/* #pragma once */
#ifndef CTREADER_H
#define	CTREADER_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
//#include "consts.h"
#include "boardcfg.h"
    
#if USE_CTREADER  
    
#include <stdint.h>
#include "stdextend.h"

#define SW_VERSION 0x01
#define SW_RELEASE 0x00

#if BOARDCFG_USE_TSC_DSPIC
#define FOSC    (120000000ULL)
#define FCY     (FOSC/2)
#endif

typedef unsigned long ulong;
typedef unsigned char uchar;
typedef unsigned ushort;
//#define uchar unsigned char
//#define ushort unsigned
//#define ulong unsigned long

#define TRUE 1
#define FALSE 0

typedef union {
	ushort ush;
	uchar uch[2];
} uconv;

typedef union {
	short sh;
	uchar uch[2];
} iconv;

typedef union{
	ulong ul;
	ushort ush[2];
	uchar uch[4];
} ulconv;
    
    
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

#if BOARDCFG_USE_TSC_DSPIC
#define CTREADER_UART_MODULE        UART_3    
#define CTREADER_BAUD               115200 
#define CTREADER_PARITY_AND_DATA    UART_PARITY_NONE_DATA_8_BITS 
#define CTREADER_STOP_BITS          UART_ONE_STOP_BIT
#else
#define CTREADER_UART_MODULE        SCI_D
#define CTREADER_BAUD               115200
#define CTREADER_PARITY             SCI_CONFIG_PAR_NONE
#define CTREADER_STOP_BITS          SCI_CONFIG_STOP_ONE

#endif
    
/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

/* ser_async */
#define CMD_GETDEB      0x00
#define CMD_GETCOM      0x01
#define CMD_GETTEMP     0x02
#define CMD_GETVOLT     0x03
#define CMD_GETCUR      0x04
#define CMD_GETTRLVL    0x05
#define CMD_GETDCOFF    0x06
#define CMD_GETDIG      0x07

#define CMD_SETDCV      0x08
#define CMD_SETCUR      0x09
#define CMD_SETSWFR     0x0A
#define CMD_SETFUND     0x0B
#define CMD_SETPWM      0x0C
#define CMD_SETOUT      0x0D
#define CMD_MKDCOFF     0x0E

#define CMD_GETRUN      0x11
#define CMD_GETERR      0x12
#define CMD_GETSYS      0x13
#define CMD_GETCAL      0x14
#define CMD_GETTRACE    0x1F

#define CMD_SETRUN      0x21
#define CMD_CLRERR      0x22
#define CMD_SETSYS      0x23
#define CMD_SETCAL      0x24
#define CMD_SETTRACE    0x2F

#define CMD_START       0x30
#define CMD_STOP        0x31

    
/* debug */    
#define DEBUG_CNT 20   
 
/* uartcommon */
#define NONE 0
#define EVEN 1
#define ODD 2
#define PARITY NONE

/* ??? 123 Mark Change UART_RX_BUFFER_SIZE UART_TX_BUFFER_SIZE was 256 */
#define UART_TX_BUFFER_SIZE_CTREADER 64
#define UART_RX_BUFFER_SIZE_CTREADER 64





#define RX_TOUT u32uSecToTimerTicks(50000)        //was 100 // 100*500us = 50ms
#define TX_DLAY u32uSecToTimerTicks(1000)
#define TX_DIS_DLAY u32uSecToTimerTicks(1000)





/* adc1 */    
#define NUM1_CH 10

/* data */  
#define SWFREQ_MIN  1500
#define SWFREQ_MAX  3500
//#define SWFREQ_DEF  BOARDCFG_PWM_FREQUENCY_HZ

#define UDCL_MIN    1000
#define UDCL_MAX    12000
#define UDCL_DEF    10000 
//#define UDCL_MIN_PRECHARGE 800

#define OCLVL_MIN   100
#define OCLVL_MAX   20000
#define OCLVL_DEF   16000

#define OVLVL1_MIN  3000
#define OVLVL1_MAX  13200
#define OVLVL1_DEF  11500

#define OVLVL2_MIN  3000
#define OVLVL2_MAX  13200
#define OVLVL2_DEF  12000

#define OTLVL_MIN  100
#define OTLVL_MAX  1350
#define OTLVL_DEF  800

#define MAXLNV_MIN  1000
#define MAXLNV_MAX  8100
#define MAXLNV_DEF  6600

#define MAXCUR_MIN  500
#define MAXCUR_MAX  10500
#define MAXCUR_DEF  10000

#define MAXDCL_MIN  2000
#define MAXDCL_MAX  11500
#define MAXDCL_DEF  9500

#define MAXPCD_MIN  5
#define MAXPCD_MAX  300
#define MAXPCD_DEF  50

#define C_SCALE_MIN 500
#define C_SCALE_MAX 30000
#define C_SCALE_DEF 16500

#define SC_SCALE_MIN 500
#define SC_SCALE_MAX 36000
#define SC_SCALE_DEF 31500

#define V_SCALE_MIN 7000
#define V_SCALE_MAX 15000
#define V_SCALE_DEF 13720

#define FUND_MIN    50
#define FUND_MAX    700
#define FUND_DEF    500

#define CUR_MIN     0
#define CUR_MAX     12000
#define CUR_DEF     10

#define PWM_MIN     0
#define PWM_MAX     600
#define PWM_DEF     20

#define KP_MIN      -30000
#define KP_MAX      30000
#define KP_DEF      0

#define KI_MIN      -30000
#define KI_MAX      30000
#define KI_DEF      0

#define TRACE_LENGTH 1  //was 400


    
/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* applstat */
typedef struct ApplStatus {
    union {
        struct{
            unsigned asy1_cmd_recvd : 1;
            unsigned asy3_cmd_recvd : 1;
            unsigned asy4_cmd_recvd : 1;
            unsigned check_params   : 1;
            
            unsigned check_tfbks    : 1;
            unsigned change_delta   : 1;
            unsigned change_freq    : 1;
            unsigned change_current : 1;
            
            unsigned clear_errors   : 1;
            unsigned start_test     : 1;
            unsigned stop_test      : 1;
            unsigned trace_running  : 1;
            
            unsigned output_test    : 1;
            unsigned idle           : 1;
            unsigned running        : 1;
            unsigned errors         : 1;
        };
        unsigned ush;
        uchar uch[2];
    };
} ApplStat;


typedef struct MainErrlStatus {
    union {
        struct{
            unsigned HW_POWERTRAIN_ERROR    : 1;
            unsigned HW_SWITCHGEAR_ERROR    : 1;
            unsigned SAFETY                 : 1;
            unsigned LOAD_OVERTEMPERATURE   : 1;
            
            unsigned SW_CONTROL_ERROR       : 1;
            unsigned SW_LIMITS_ERROR        : 1;
            unsigned TEST_ERROR             : 1;
            unsigned TEST_NOT_CALIBRATED    : 1;
            
            unsigned reserve3               : 1;
            unsigned reserve2               : 1;
            unsigned reserve1               : 1;
            unsigned PARAMS_ERROR           : 1;
            
            unsigned EEPROM_ERROR           : 1;
            unsigned DAC_ERROR              : 1;
            unsigned MAINPIC_ERROR          : 1;
            unsigned AUXPIC_ERROR           : 1;
        };
        unsigned ush;
        uchar uch[2];
    };
} MEStat;

/* data */
typedef struct DetectedModules 
{
    union 
	{
        struct
		{
            unsigned GE1        : 1;
            unsigned GE2        : 1;
            unsigned GE3        : 1;
            unsigned SEM1       : 1;
            unsigned SEM2       : 1;
            unsigned SEM3       : 1;
            unsigned m1         : 1;
            unsigned m2         : 1;
            unsigned m3         : 1;
            unsigned reserve    : 5;
            unsigned n          : 2;
        };
        unsigned ush;
        uchar uch[2];
    };
} DetMod;





/* *****************************************************************************
 * Type Definitions - Mapping CTReader Variables
 **************************************************************************** */

/* ush_err_ovoltcur */

//  AA 55 OV&DET CUR_SNS CUR_SEM SEM_TEMP&DRIVE GE_TOP&BOT SPARE0 SPARE1 SPARE2 SPARE3 VER REV CHS
// OV_DET OV1 OV2 SEM_DET(3, 2, 1) GE_DET(3, 2, 1)      uch_asy4_rx_buf[0]    
// CUR_SNS OCP (3, 2, 1) OCN(3, 2, 1)                   uch_asy4_rx_buf[1]
// CUR_SEM SEMP(3, 2, 1) SEMN(3, 2, 1)                  uch_asy4_rx_buf[2]
// SEM_TEM_DRIVE TEMP(3, 2, 1) DRIVE (3, 2, 1)          uch_asy4_rx_buf[3]
// GE_TOP&BOT   TOP(3, 2, 3) BOT (3, 2, 1)              uch_asy4_rx_buf[4]  
//                    
// if(!uch_asy4_rx_chsum){
//     ush_mb_in_reg[MB_ADDR_DETMOD] = ush_det_modules =  uch_asy4_rx_buf[0] & 0x3F; 
//     ush_err_ovoltcur = ((uch_asy4_rx_buf[0] & 0xC0) | (uch_asy4_rx_buf[1] & 0x3F))*256 + uch_asy4_rx_buf[2];
//     ush_err_modules = uch_asy4_rx_buf[3]*256 + uch_asy4_rx_buf[4];
//     uch_rx4_cmd_tout = RX_CMD_TOUT;
//     ush_mb_in_reg[MB_ADDR_ERROVOC] = ush_errovoc = (~(((uch_asy4_rx_buf[0] & 0xC0) | (uch_asy4_rx_buf[1] & 0x3F))*64 + (uch_asy4_rx_buf[2] & 0x3F)) & 0x3FFF);
//     ush_mb_in_reg[MB_ADDR_ERRMOD] = ush_errmod = (~(uch_asy4_rx_buf[3]*64 + (uch_asy4_rx_buf[4] & 0x3F)) & 0x0FFF);
//     as_appl_status.asy4_cmd_recvd = TRUE;
// }

typedef struct
{
    uint16_t CurSnsOCN1 : 1;
    uint16_t CurSnsOCN2 : 1;
    uint16_t CurSnsOCN3 : 1;
    uint16_t CurSnsOCP1 : 1;
    uint16_t CurSnsOCP2 : 1;
    uint16_t CurSnsOCP3 : 1;
    uint16_t Reserved1  : 1;
    uint16_t Reserved2  : 1;
    uint16_t CurSemOCN1 : 1;
    uint16_t CurSemOCN2 : 1;
    uint16_t CurSemOCN3 : 1;
    uint16_t CurSemOCP1 : 1;
    uint16_t CurSemOCP2 : 1;
    uint16_t CurSemOCP3 : 1;
    uint16_t OVDetOV1   : 1;
    uint16_t OVDetOV2   : 1;
}ush_err_ovoltcur_t;
      
typedef struct
{
    // GE_TOP&BOT   TOP(3, 2, 3) BOT (3, 2, 1)              uch_asy4_rx_buf[4]  
    uint16_t GEBot1 : 1;
    uint16_t GEBot2 : 1;
    uint16_t GEBot3 : 1;
    uint16_t GETop1 : 1;
    uint16_t GETop2 : 1;
    uint16_t GETop3 : 1;
    uint16_t Reserved1  : 1;
    uint16_t Reserved2  : 1;
    // SEM_TEM_DRIVE TEMP(3, 2, 1) DRIVE (3, 2, 1)          uch_asy4_rx_buf[3]
    uint16_t TempSem1 : 1;
    uint16_t TempSem2 : 1;
    uint16_t TempSem3 : 1;
    uint16_t DrvErrSem1 : 1;
    uint16_t DrvErrSem2 : 1;
    uint16_t DrvErrSem3 : 1;
    uint16_t Reserved3   : 1;
    uint16_t Reserved4   : 1;
}ush_err_modules_t;
      
typedef union
{
    ush_err_ovoltcur_t sSignals;
    uint16_t u16Register;
}ush_err_ovoltcur_u;

typedef union
{
    ush_err_modules_t sSignals;
    uint16_t u16Register;
}ush_err_modules_u;

/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 
extern uint8_t CTREADER_u8UartIndex;
extern uint16_t CTREADER_sh_u_dcl_set;

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void CTREADER_vProcess(void);
void CTREADER_vUartOverflowDetection(void);
void CTREADER_vProcessUartTx(uint8_t u8Index);
void CTREADER_vProcessUartRx(void);
void CTREADER_vProcessFastTD (uint8_t u8Index);

#endif  /* USE_CTREADER */

#endif	/* CTREADER_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
