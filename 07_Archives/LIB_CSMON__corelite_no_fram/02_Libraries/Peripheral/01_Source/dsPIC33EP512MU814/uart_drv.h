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

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#include "boardcfg.h"
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
            uint16_t UARTMODE;
            uint16_t UARTSTA;
            uint16_t UARTTXREG;
            uint16_t UARTRXREG;
            uint16_t UARTBRG;
        };
        
        struct
        {
            U1MODEBITS UARTMODEbits;
            U1STABITS  UARTSTAbits;
            uint16_t :16;
            uint16_t :16;
            uint16_t :16;
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
#if 0
void UART1_vInit(uint32_t u32Fcy);
bool UART1_bOverflowCheckClear(void);
bool UART1_ReadDataRxAvailable(void);
uint8_t UART1_ReadDataRx(void);
uint32_t UART1_ReadDataRxTimeDiff(void);
#endif

void UART_vInitSoftwareFifo (UART_eModule_t index);
void UART_vInit ( UART_eModule_t index, uint32_t u32Fcy, UART_sModuleConfiguration_t* sModuleConfiguration);

bool UART_IsTxReady(UART_eModule_t index);
void UART_PutChar(UART_eModule_t index, uint16_t data);
bool UART_IsTxEmpty(UART_eModule_t index);
bool UART_IsRxReady(UART_eModule_t index);
uint16_t UART_GetChar(UART_eModule_t index);

void UART_vReset (void);
bool UART_bOverflowCheckClear(UART_eModule_t index);

bool UART_ReadDataRxAvailable(UART_eModule_t index);
uint8_t UART_ReadDataRx(UART_eModule_t index);
uint32_t UART_ReadDataRxTimeDiff(UART_eModule_t index);
void UART_TxIntEna(UART_eModule_t index);
void UART_TxIntDis(UART_eModule_t index);
void UART_TransmitEnable(UART_eModule_t index);
void UART_TransmitDisable(UART_eModule_t index);
void UART_DRV_vInit(void);
#ifndef BOARDCFG_USE_MCC_UART
bool UART_DRV_bConfigurationChangeRequest(void);
#else
#define UART_DRV_bConfigurationChangeRequest() false
#endif
#endif	/* UART_DRV_H */
