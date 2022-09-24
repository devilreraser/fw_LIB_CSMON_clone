/* *****************************************************************************
 * File:   uart_driver.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 02 11 16:16
 * 
 * Description: UART Initialization and Error Check
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef UART_DRV_H
#define	UART_DRV_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdbool.h>
#include <stdint.h>

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */
typedef enum
{
    UART_DRV_1,
    UART_DRV_2,
    UART_DRV_3,
    UART_DRV_4,
    UART_DRV_COUNT
}UART_DRV_eModule_t;


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

void UART_DRV_vInit ( UART_DRV_eModule_t index);
void UART_DRV_PutChar(UART_DRV_eModule_t index, uint16_t data);
bool UART_DRV_ReadDataRxAvailable(UART_DRV_eModule_t index);
uint16_t UART_DRV_ReadDataRx(UART_DRV_eModule_t index);
void UART_DRV_PutNextAddress(UART_DRV_eModule_t index);
void UART_DRV_RXInterrupt(UART_DRV_eModule_t index);
void UART_DRV_TXInterrupt(UART_DRV_eModule_t index);
void UART_DRV_TransmitEnableDisable(UART_DRV_eModule_t index, bool pinLevel);
void UART_DRV_TxIntDis(UART_DRV_eModule_t index);
void UART_DRV_TxIntEna(UART_DRV_eModule_t index);

#if 0
bool UART_DRV_IsTxReady(UART_DRV_eModule_t index);
bool UART_DRV_IsTxEmpty(UART_DRV_eModule_t index);
bool UART_DRV_IsRxReady(UART_DRV_eModule_t index);
uint16_t UART_DRV_GetChar(UART_DRV_eModule_t index);
uint32_t UART_DRV_ReadDataRxTimeDiff(UART_DRV_eModule_t index);
void UART_DRV_TxEna(UART_DRV_eModule_t index);
void UART_DRV_TxDis(UART_DRV_eModule_t index);
void UART_DRV_RxEna(UART_DRV_eModule_t index);
void UART_DRV_RxDis(UART_DRV_eModule_t index);
#endif

#endif	/* UART_DRV_H */
