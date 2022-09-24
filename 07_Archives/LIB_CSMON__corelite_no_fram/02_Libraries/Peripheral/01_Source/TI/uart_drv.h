/* *****************************************************************************
 * File:   uart_drv.h
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
#include "stdextend.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#ifdef _CSMON_USE_SCIA_ON_USB_CONNECTOR
#if _CSMON_USE_SCIA_ON_USB_CONNECTOR
#define USE_SCIA_ON_USB_CONNECTOR   1
#else
#define USE_SCIA_ON_USB_CONNECTOR   0
#endif
#else
#define USE_SCIA_ON_USB_CONNECTOR   0
#endif

/* *****************************************************************************
 * Constants Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */
typedef enum
{
    UART_1,
    UART_2,
    UART_3,
    UART_4,
    UART_COUNT
}UART_eModule_t;

typedef enum
{
    UART_PARITY_NONE_DATA_9_BITS = 0b11,
    UART_PARITY_ODD_DATA_8_BITS  = 0b10,
    UART_PARITY_EVEN_DATA_8_BITS = 0b01,
    UART_PARITY_NONE_DATA_8_BITS = 0b00,
}UART_eParityAndData_t;

typedef enum
{
    UART_ONE_STOP_BIT = 0,
    UART_TWO_STOP_BITS = 1,

}UART_eStopBits_t;




/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
typedef struct
{
    uint32_t u32BaudRate;
    UART_eParityAndData_t eParityAndData;
    UART_eStopBits_t eStopBits;
    
}UART_sModuleConfiguration_t;



typedef struct
{
    union 
    {
        struct 
        {
            uint16_t RealRegisters;

        };
        
        struct
        {
            uint16_t RealRegistersBits:16;
        };
    };
}UART_sBaseRegisters_t;


typedef struct
{
    UART_sBaseRegisters_t* psBase[UART_COUNT];
    uint8_t u8BitOffsetsISRRx[UART_COUNT];
    uint8_t u8BitOffsetsISRTx[UART_COUNT];
    uint8_t u8BitOffsetsISRErr[UART_COUNT];
}UART_sHandle_t;


/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */
#if 0
extern uint32_t UART_u32U1Baud;
extern UART_eParityAndData_t UART_eParityAndData;
#endif

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */

void UART_vInit ( UART_eModule_t index);

bool UART_IsTxReady(UART_eModule_t index);
void UART_PutChar(UART_eModule_t index, uint16_t data);
void UART_PutNextAddress(UART_eModule_t index);
bool UART_IsTxEmpty(UART_eModule_t index);
bool UART_IsRxReady(UART_eModule_t index);
uint16_t UART_GetChar(UART_eModule_t index);

void UART_vReset (void);

bool UART_ReadDataRxAvailable(UART_eModule_t index);
uint16_t UART_ReadDataRx(UART_eModule_t index);
uint32_t UART_ReadDataRxTimeDiff(UART_eModule_t index);
void UART_TxIntEna(UART_eModule_t index);
void UART_TxIntDis(UART_eModule_t index);
void UART_TransmitEnable(UART_eModule_t index);
void UART_TransmitDisable(UART_eModule_t index);

void UART_RXInterrupt(UART_eModule_t index);
void UART_TXInterrupt(UART_eModule_t index);

void UART_TxEna(UART_eModule_t index);
void UART_TxDis(UART_eModule_t index);
void UART_RxEna(UART_eModule_t index);
void UART_RxDis(UART_eModule_t index);
bool UART_bUseSCIA_On_USB_Connector(void);

#endif	/* UART_DRV_H */
