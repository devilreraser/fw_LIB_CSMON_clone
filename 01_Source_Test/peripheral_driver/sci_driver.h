/* *****************************************************************************
 * File:   sci_driver.h
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
    SCI_DRV_A,
    SCI_DRV_B,
    SCI_DRV_C,
    SCI_DRV_D,
    SCI_DRV_COUNT
}SCI_DRV_eModule_t;



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
__interrupt void SCIA_DRV_TXFIFOISR(void);
__interrupt void SCIA_DRV_RXFIFOISR(void);
__interrupt void SCIB_DRV_TXFIFOISR(void);
__interrupt void SCIB_DRV_RXFIFOISR(void);
__interrupt void SCIC_DRV_TXFIFOISR(void);
__interrupt void SCIC_DRV_RXFIFOISR(void);
__interrupt void SCID_DRV_TXFIFOISR(void);
__interrupt void SCID_DRV_RXFIFOISR(void);
void SCI_DRV_vErrorResetProcess(uint32_t base);
void SCI_DRV_vInitFIFO(uint32_t base, uint32_t baud, uint32_t config);
SCI_DRV_eModule_t SCI_DRV_eGetModuleIndexFromBase(uint32_t base);
uint32_t SCI_DRV_u32GetBaseFromModuleIndex(SCI_DRV_eModule_t index);
#endif	/* SCI_DRV_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
