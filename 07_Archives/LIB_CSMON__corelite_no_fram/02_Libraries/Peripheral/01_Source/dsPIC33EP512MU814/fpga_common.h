/* *****************************************************************************
 * File:   fpga_common.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 08 14 12:51
 * 
 * Description: FPGA Common
 * 
 **************************************************************************** */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Guard condition file contents not included more than once */  
/* #pragma once */
#ifndef FPGA_COMMON_H
#define	FPGA_COMMON_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdint.h>
#include <stdbool.h>
   
#include "boardcfg.h"

   
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define FPGA_COMMON_STACK1      0
#define FPGA_COMMON_STACK2      1
#define FPGA_COMMON_STACK3      2


/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

/* GLOBAL CONSTANTS */
#define PMP_CONFIGURATION_UNLOCK_KEY                        0xA5    /* 1 byte */
#define PMP_ADDRESS_FPGA_RADDR_UNLOCK                       0x0080  /* 1 byte */
#define PMP_ADDRESS_OFFSET_VER                              0x0081  /* 1 byte */
#define PMP_ADDRESS_OFFSET_REV                              0x0082  /* 1 byte */
#define PMP_ADDRESS_OFFSET_SYS                              0x0010  /* 1 byte */
#define PMP_ADDRESS_OFFSET_FREG                             0x0000  /* 1 byte */


/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
typedef struct
{
    uint16_t bTripError : 1;
    uint16_t bPWMError : 1;
    uint16_t bTempError : 1;
    uint16_t bOCError : 1;
    uint16_t bOCPError : 1;
    uint16_t bOCNError : 1;
    uint16_t bOVError : 1;
    uint16_t bOCP2Error : 1;
    uint16_t bOCN2Error : 1;
    uint16_t bDETError : 1;
    uint16_t bDRVError : 1;
    uint16_t bTOPError : 1;
    uint16_t bBOTError : 1;


}FPGA_sErrorCode_t;

typedef union
{
    uint16_t u16Register;
    FPGA_sErrorCode_t sErrorCode;
}FPGA_uErrorCode_t;




#if BOARDCFG_USE_FPGA

/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 
extern bool FPGA_COMMON_bUseFPGANoErrorEmulation;

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void FPGA_COMMON_vResetEnable(void);
void FPGA_COMMON_vResetRelease(void);
void FPGA_COMMON_vEnablePWM(void);
void FPGA_COMMON_vDisablePWM(void);
bool FPGA_COMMON_bIsError(void);
FPGA_uErrorCode_t FPGA_COMMON_uGetError(void);
void FPGA_COMMON_vSetStackErrorMask(uint16_t nStack);
void FPGA_COMMON_vClrStackErrorMask(uint16_t nStack);
void FPGA_COMMON_vResetTripStatus(void);

#else   /* #if BOARDCFG_USE_FPGA */
#define FPGA_COMMON_vResetEnable()
#define FPGA_COMMON_vResetRelease()
#define FPGA_COMMON_vEnablePWM()
#define FPGA_COMMON_vDisablePWM()
#define FPGA_COMMON_bIsError() false
#define FPGA_COMMON_uGetError() 0
#define FPGA_COMMON_vSetStackErrorMask()
#define FPGA_COMMON_vClrStackErrorMask()
#define FPGA_COMMON_vResetTripStatus()
#endif   /* #if BOARDCFG_USE_FPGA */



#endif	/* FPGA_COMMON_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
