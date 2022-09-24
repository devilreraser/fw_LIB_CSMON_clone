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

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */
#define FPGA_COMMON_STACK1      0
#define FPGA_COMMON_STACK2      1
#define FPGA_COMMON_STACK3      2



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
    uint16_t bOCP2Error : 1;
    uint16_t bOCN2Error : 1;
    uint16_t bOVError : 1;
    uint16_t bDETError : 1;
    uint16_t bDRVError : 1;
    uint16_t bTOPError : 1;
    uint16_t bBOTError : 1;
    uint16_t bGLBError : 1;
    uint16_t bStackError : 1;
    uint16_t bReserved : 1;


}FPGA_sErrorCode_t;

typedef union
{
    uint16_t u16Register;
    FPGA_sErrorCode_t sErrorCode;
}FPGA_uErrorCode_t;




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
void FPGA_COMMON_vResetTripStatus(void);

void FPGA_COMMON_vEnablePWM(void);
void FPGA_COMMON_vDisablePWM(void);
bool FPGA_COMMON_bIsError(void);
FPGA_uErrorCode_t FPGA_COMMON_uGetError(void);
void FPGA_COMMON_vSetStackErrorMask(uint16_t nStack);
void FPGA_COMMON_vClrStackErrorMask(uint16_t nStack);


#endif	/* FPGA_COMMON_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
