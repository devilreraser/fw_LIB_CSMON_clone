/* *****************************************************************************
 * File:   sci_drv.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 11 23 12:59
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
#ifndef SCI_DRV_H
#define	SCI_DRV_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdint.h>
    
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */
typedef enum
{
    SCI_A,
    SCI_B,
    SCI_C,
    SCI_D,
    SCI_COUNT
}SCI_eModule_t;



/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
__interrupt void SCIA_TXFIFOISR(void);
__interrupt void SCIA_RXFIFOISR(void);
__interrupt void SCIB_TXFIFOISR(void);
__interrupt void SCIB_RXFIFOISR(void);
__interrupt void SCIC_TXFIFOISR(void);
__interrupt void SCIC_RXFIFOISR(void);
__interrupt void SCID_TXFIFOISR(void);
__interrupt void SCID_RXFIFOISR(void);
void SCI_vErrorResetProcess(uint32_t base);
void SCI_vInitFIFORegisters(void);
void SCI_vInitFIFO(uint32_t base, uint32_t baud, uint32_t config);
SCI_eModule_t SCI_eGetModuleIndexFromBase(uint32_t base);
uint32_t SCI_u32GetBaseFromModuleIndex(SCI_eModule_t index);
#endif	/* SCI_DRV_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
