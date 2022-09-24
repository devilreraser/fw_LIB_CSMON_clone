/* *****************************************************************************
 * File:   uart_drv.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 02 11 16:16
 * 
 * Description: UART Initialization and Error Check
 * 
 **************************************************************************** */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <xc.h>
#include <stdbool.h>
#include "uart_drv.h" /* Uart Include must be before boardcfg(projectcfg uses uart definitions of UART_1 and UART2 and Parity) */
#include "boardcfg.h"
#include "sys_drv.h"
#include "isr.h"
#include "hmlib.h"

#include "ctreader.h"
#include "hmserial.h"
#include "hmmodbus.h"

#include "incap.h"

#ifdef BOARDCFG_USE_MCC_UART
#include "uart1.h"
#include "uart2.h"
#include "uart3.h"
#include "uart4.h"
#endif

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#ifndef TXEN_U1_START_HIGH
#define TXEN_U1_SetEnable()  TXEN_U1_SetHigh() 
#define TXEN_U1_SetDisable() TXEN_U1_SetLow() 
#else
#define TXEN_U1_SetEnable()  TXEN_U1_SetLow() 
#define TXEN_U1_SetDisable() TXEN_U1_SetHigh() 
#endif

#ifndef TXEN_U2_START_HIGH
#define TXEN_U2_SetEnable()  TXEN_U2_SetHigh() 
#define TXEN_U2_SetDisable() TXEN_U2_SetLow() 
#else
#define TXEN_U2_SetEnable()  TXEN_U2_SetLow() 
#define TXEN_U2_SetDisable() TXEN_U2_SetHigh() 
#endif

#ifndef TXEN_U3_START_HIGH
#define TXEN_U3_SetEnable()  TXEN_U3_SetHigh() 
#define TXEN_U3_SetDisable() TXEN_U3_SetLow() 
#else
#define TXEN_U3_SetEnable()  TXEN_U3_SetLow() 
#define TXEN_U3_SetDisable() TXEN_U3_SetHigh() 
#endif

#ifndef TXEN_U4_START_HIGH
#define TXEN_U4_SetEnable()  TXEN_U4_SetHigh() 
#define TXEN_U4_SetDisable() TXEN_U4_SetLow() 
#else
#define TXEN_U4_SetEnable()  TXEN_U4_SetLow() 
#define TXEN_U4_SetDisable() TXEN_U4_SetHigh() 
#endif



#ifdef RXEN_U1_START_HIGH
#define RXEN_U1_SetEnable()  RXEN_U1_SetHigh() 
#define RXEN_U1_SetDisable() RXEN_U1_SetLow() 
#else
#define RXEN_U1_SetEnable()  RXEN_U1_SetLow() 
#define RXEN_U1_SetDisable() RXEN_U1_SetHigh() 
#endif

#ifdef RXEN_U2_START_HIGH
#define RXEN_U2_SetEnable()  RXEN_U2_SetHigh() 
#define RXEN_U2_SetDisable() RXEN_U2_SetLow() 
#else
#define RXEN_U2_SetEnable()  RXEN_U2_SetLow() 
#define RXEN_U2_SetDisable() RXEN_U2_SetHigh() 
#endif

#ifdef RXEN_U3_START_HIGH
#define RXEN_U3_SetEnable()  RXEN_U3_SetHigh() 
#define RXEN_U3_SetDisable() RXEN_U3_SetLow() 
#else
#define RXEN_U3_SetEnable()  RXEN_U3_SetLow() 
#define RXEN_U3_SetDisable() RXEN_U3_SetHigh() 
#endif

#ifdef RXEN_U4_START_HIGH
#define RXEN_U4_SetEnable()  RXEN_U4_SetHigh() 
#define RXEN_U4_SetDisable() RXEN_U4_SetLow() 
#else
#define RXEN_U4_SetEnable()  RXEN_U4_SetLow() 
#define RXEN_U4_SetDisable() RXEN_U4_SetHigh() 
#endif





#ifndef BOARDCFG_USE_MCC_UART
#define USE_DMA_UART       0           /* 0 or 1 - Not Implemented 1 */
#define USE_INT_UART_ERR   1           /* 0 or 1 */
#define USE_INT_UART_RX    HMDRV_SCI_RX_ISR           /* 0 or 1 */
#define USE_INT_UART_TX    HMDRV_SCI_TX_ISR           /* 0 or 1 */
#else
#define USE_INT_UART_ERR   1           /* 0 or 1 */
#define USE_INT_UART_RX    1           /* 0 or 1 */
#define USE_INT_UART_TX    1           /* 0 or 1 */
#define MCC_RX_BUFFER_NOT_USED 0       /* When MCC Library Reads multiple bytes on one Rx ISR buffer possible not to be used and specially when time between bytes measure needed */
#endif

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */



/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */

UART_eModule_t SYSTEM_eUartInCapt = HMLIB_HEXMON_UART_MODULE;

UART_eModule_t SYSTEM_eUartModBus = HMLIB_MODBUS_UART_MODULE;
UART_eModule_t SYSTEM_eUartHexMon = HMLIB_HEXMON_UART_MODULE;
#if USE_CTREADER  
UART_eModule_t SYSTEM_eUartCTRead = CTREADER_UART_MODULE;
#endif
UART_eModule_t SYSTEM_eUartInCaptOld;

UART_eModule_t SYSTEM_eUartModBusOld;
UART_eModule_t SYSTEM_eUartHexMonOld;
UART_eModule_t SYSTEM_eUartCTReadOld;

UART_sModuleConfiguration_t sHexMonUartModuleConfiguration = 
{
    HMDRV_HEXMON_BAUD, 
    UART_PARITY_AND_DATA_HEXMON, 
    UART_STOP_BITS,

};
UART_sModuleConfiguration_t sHexMonUartModuleConfigurationOld;

UART_sModuleConfiguration_t sModBusUartModuleConfiguration = 
{
    HMDRV_MODBUS_BAUD, 
    UART_PARITY_AND_DATA_MODBUS, 
    UART_STOP_BITS,

};
UART_sModuleConfiguration_t sModBusUartModuleConfigurationOld;

#if USE_CTREADER  
UART_sModuleConfiguration_t sCTReadUartModuleConfiguration = 
{
    CTREADER_BAUD, 
    CTREADER_PARITY_AND_DATA, 
    CTREADER_STOP_BITS,

};
UART_sModuleConfiguration_t sCTReadUartModuleConfigurationOld;
#endif







UART_sHandle_t UART_sHandle = 
{
    {
        (UART_sBaseRegisters_t*)(&U1MODE),
        (UART_sBaseRegisters_t*)(&U2MODE),
        (UART_sBaseRegisters_t*)(&U3MODE),
        (UART_sBaseRegisters_t*)(&U4MODE),
    },
    {   /* Rx */
        11 + 0*16,          /* UART1 */         
        14 + 1*16,          /* UART2 */         
         2 + 5*16,          /* UART3 */         
         8 + 5*16,          /* UART4 */                 
    },
    {   /* Tx */
        12 + 0*16,          /* UART1 */         
        15 + 1*16,          /* UART2 */         
         3 + 5*16,          /* UART3 */         
         9 + 5*16,          /* UART4 */                 
    },
    {   /* Err */
         1 + 4*16,          /* UART1 */         
         2 + 4*16,          /* UART2 */         
         1 + 5*16,          /* UART3 */         
         7 + 5*16,          /* UART4 */                 
    },

};



#define UART_RX_BUFFER_SIZE    32 /* if BOARDCFG_USE_MCC_UART set as UARTX_CONFIG_RX_BYTEQ_LENGTH for best overflow handling */

#if !defined (BOARDCFG_USE_MCC_UART) || (MCC_RX_BUFFER_NOT_USED == 1)
#endif
uint8_t UARTRxBuffer[UART_COUNT][UART_RX_BUFFER_SIZE];


uint32_t UART_RXOverflowCounter[UART_COUNT] = {0,0,0,0};

uint16_t UARTRxBufferWRCount[UART_COUNT] = {0,0,0,0};  /* How many received We have to process */
uint16_t UARTRxBufferRDCount[UART_COUNT] = {0,0,0,0};
uint16_t UARTRxBufferRDIndex[UART_COUNT] = {0,0,0,0};
uint16_t UARTRxBufferWRIndex[UART_COUNT] = {0,0,0,0};
uint32_t UARTRxBufferTimeDiff[UART_COUNT][UART_RX_BUFFER_SIZE];
uint32_t u32TimeDiffRead[UART_COUNT] = {0,0,0,0};
uint32_t u32TimeDiffNow[UART_COUNT] = {0,0,0,0};
uint32_t u32TimeDiffLast[UART_COUNT] = {0,0,0,0};
uint32_t UART_RXIntOverflowCounter[UART_COUNT] = {0,0,0,0};



/* *****************************************************************************
 * Functions
 **************************************************************************** */

#ifndef BOARDCFG_USE_MCC_UART

void UART_TxIntEna(UART_eModule_t index)
{
    uint8_t u8BitOffsetsISRTx = UART_sHandle.u8BitOffsetsISRTx[index];

    ISR_vClrBitIFS(u8BitOffsetsISRTx);   // Clear  interrupt
    ISR_vSetBitIEC(u8BitOffsetsISRTx);   // Enable  interrupt
}
    
void UART_TxIntDis(UART_eModule_t index)
{
    uint8_t u8BitOffsetsISRTx = UART_sHandle.u8BitOffsetsISRTx[index];

    ISR_vClrBitIEC(u8BitOffsetsISRTx);   // Enable  interrupt
    ISR_vClrBitIFS(u8BitOffsetsISRTx);   // Clear  interrupt
}
bool UART_IsTxReady(UART_eModule_t index)
{
    return ((((UART_sBaseRegisters_t*)(UART_sHandle.psBase[index]))->UARTSTAbits.UTXBF == 0) ? true : false);
}
bool UART_IsRxReady(UART_eModule_t index)
{
    return ((((UART_sBaseRegisters_t*)(UART_sHandle.psBase[index]))->UARTSTAbits.URXDA == 0) ? true : false);
}
bool UART_IsTxEmpty(UART_eModule_t index)
{
    return ((((UART_sBaseRegisters_t*)(UART_sHandle.psBase[index]))->UARTSTAbits.TRMT == 0) ? false : true);
}
void UART_PutChar(UART_eModule_t index, uint16_t data)
{
    ((UART_sBaseRegisters_t*)(UART_sHandle.psBase[index]))->UARTTXREG = data;
}
uint16_t UART_GetChar(UART_eModule_t index)
{
    return ((UART_sBaseRegisters_t*)(UART_sHandle.psBase[index]))->UARTRXREG;
}

#else
void UART_TxIntEna(UART_eModule_t index)
{   
}
void UART_TxIntDis(UART_eModule_t index)
{
}
bool UART_IsTxReady(UART_eModule_t index)
{
    switch(index)
    {
        case UART_1:
            return UART1_IsTxReady();
            break;
        case UART_2:
            return UART2_IsTxReady();
            break;
        case UART_3:
            return UART3_IsTxReady();
            break;
        case UART_4:
            return UART4_IsTxReady();
            break;
        default:
            break;
    }
    return false;
}
bool UART_IsRxReady(UART_eModule_t index)
{
    switch(index)
    {
        case UART_1:
            return UART1_IsRxReady();
            break;
        case UART_2:
            return UART2_IsRxReady();
            break;
        case UART_3:
            return UART3_IsRxReady();
            break;
        case UART_4:
            return UART4_IsRxReady();
            break;
        default:
            break;
    }
    return false;
}
bool UART_IsTxEmpty(UART_eModule_t index)
{
    switch(index)
    {
        case UART_1:
            return UART1_IsTxDone();
            break;
        case UART_2:
            return UART2_IsTxDone();
            break;
        case UART_3:
            return UART3_IsTxDone();
            break;
        case UART_4:
            return UART4_IsTxDone();
            break;
        default:
            break;
    }
    return false;
}
void UART_PutChar(UART_eModule_t index, uint16_t data)
{
    switch(index)
    {
        case UART_1:
            UART1_Write((uint8_t)data);
            break;
        case UART_2:
            UART2_Write((uint8_t)data);
            break;
        case UART_3:
            UART3_Write((uint8_t)data);
            break;
        case UART_4:
            UART4_Write((uint8_t)data);
            break;
        default:
            break;
    }
}

uint16_t UART_GetChar(UART_eModule_t index)
{
    switch(index)
    {
        case UART_1:
            return (uint16_t)UART1_Read();
            break;
        case UART_2:
            return (uint16_t)UART2_Read();
            break;
        case UART_3:
            return (uint16_t)UART3_Read();
            break;
        case UART_4:
            return (uint16_t)UART4_Read();
            break;
        default:
            break;
    }
    return 0;
}

#endif 










#ifndef BOARDCFG_USE_MCC_UART

void UART_vReset (void)
{
    volatile UART_sBaseRegisters_t* psBase;
    
    psBase = UART_sHandle.psBase[UART_1]; psBase->UARTMODE = 0;
    psBase = UART_sHandle.psBase[UART_2]; psBase->UARTMODE = 0;
    psBase = UART_sHandle.psBase[UART_3]; psBase->UARTMODE = 0;
    psBase = UART_sHandle.psBase[UART_4]; psBase->UARTMODE = 0;
    
}
#endif

/* *****************************************************************************
 * UART_vInitSoftwareFifo
 * 
 * Description: UART Fifo Initialization 
 **************************************************************************** */
void UART_vInitSoftwareFifo (UART_eModule_t index)
{
    u32TimeDiffLast[index] = HMDRV_GET_FREE_RUN_TIMER();
    
    #if USE_DMA_UART
    /* UART1 Rx DMA */
    CfgDma3UartRx();
    #endif

    #if USE_INT_UART_RX
    UARTRxBufferRDIndex[index] = 0;
    UARTRxBufferWRIndex[index] = 0;
    UARTRxBufferWRCount[index] = 0;
    UARTRxBufferRDCount[index] = 0;
    #endif
    
    
}



/* *****************************************************************************
 * UART_vInit
 * 
 * Description: UART Initialization 
 **************************************************************************** */
void UART_vInit ( UART_eModule_t index, uint32_t u32Fcy, UART_sModuleConfiguration_t* sModuleConfiguration)
{
    #if !defined (BOARDCFG_USE_MCC_UART)
    volatile UART_sBaseRegisters_t* psBase;
    #endif

    UART_vInitSoftwareFifo(index);
    // setup the UART peripheral

    #ifndef BOARDCFG_USE_MCC_UART    
    
    psBase = UART_sHandle.psBase[index];
    // configure psBase->UARTMODE
    psBase->UARTMODEbits.UARTEN = 0;  // Bit15 TX, RX DISABLED, ENABLE at end of function

    //psBase->UARTMODEbits.not implemented;// Bit14
    psBase->UARTMODEbits.USIDL = 0;   // Bit13 Continue in Idle
    psBase->UARTMODEbits.IREN = 0;    // Bit12 No IR translation
    psBase->UARTMODEbits.RTSMD = 0;   // Bit11 Simplex Mode

    //psBase->UARTMODEbits.not implemented;// Bit10
    psBase->UARTMODEbits.UEN = 0;     // Bits8,9 TX,RX enabled, CTS,RTS not
    psBase->UARTMODEbits.WAKE = 0;    // Bit7 No Wake up (since we don't sleep here)
    psBase->UARTMODEbits.LPBACK = 0;  // Bit6 No Loop Back
    psBase->UARTMODEbits.ABAUD = 0;   // Bit5 No AutoBaud (would require sending '55')
    psBase->UARTMODEbits.BRGH = 1;    // Bit3 16 clocks per bit period
    psBase->UARTMODEbits.PDSEL = sModuleConfiguration->eParityAndData;
    psBase->UARTMODEbits.STSEL = sModuleConfiguration->eStopBits;   // Bit0 One Stop Bit

    // Load a value into Baud Rate Generator.  Example is for 9600.
    // See section 19.3.1 of dataSheet.
    psBase->UARTBRG = (uint16_t)(((u32Fcy + 2*sModuleConfiguration->u32BaudRate)/(4 * sModuleConfiguration->u32BaudRate))-1.0);



    // Load all values in for U1STA SFR

    //psBase->UARTSTAbits.not implemented = 0;//Bit12
    psBase->UARTSTAbits.UTXBRK = 0;   //Bit11 Disabled
    psBase->UARTSTAbits.UTXEN = 0;    //Bit10 TX pins controlled by peripheral
    psBase->UARTSTAbits.UTXBF = 0;    //Bit9 *Read Only Bit*
    psBase->UARTSTAbits.TRMT = 0;     //Bit8 *Read Only bit*
    psBase->UARTSTAbits.URXISEL = 0;  //Bits6,7 Interrupt on character received
    psBase->UARTSTAbits.ADDEN = 0;    //Bit5 Address Detect Disabled
    psBase->UARTSTAbits.RIDLE = 0;    //Bit4 *Read Only Bit*
    psBase->UARTSTAbits.PERR = 0;     //Bit3 *Read Only Bit*
    psBase->UARTSTAbits.FERR = 0;     //Bit2 *Read Only Bit*
    psBase->UARTSTAbits.OERR = 0;     //Bit1 *Read Only Bit*
    psBase->UARTSTAbits.URXDA = 0;    //Bit0 *Read Only Bit*

    /*
    UTXISEL<1:0>: UARTx Transmission Interrupt Mode Selection bits
    11 = Reserved; do not use
    10 = Interrupt when a character is transferred to the Transmit Shift Register (TSR) and as a result, the
    transmit buffer becomes empty
    01 = Interrupt when the last character is shifted out of the Transmit Shift Register; all transmit
    operations are completed
    00 = Interrupt when a character is transferred to the Transmit Shift Register (this implies there is at
    least one character open in the transmit buffer)
    */
    
    psBase->UARTSTAbits.UTXISEL1 = 0; //Bit15 int when character is transferred to the Transmit Shift Register
    psBase->UARTSTAbits.UTXISEL0 = 0; //Bit13 int when character is transferred to the Transmit Shift Register
    
    //psBase->UARTSTAbits.UTXISEL1 = 0; //Bit15 int when last character is shifted out of the Transmit Shift Register
    //psBase->UARTSTAbits.UTXISEL0 = 1; //Bit13 int when last character is shifted out of the Transmit Shift Register
    
    //psBase->UARTSTAbits.UTXISEL1 = 1; //Bit15 int when transmit buffer becomes empty
    //psBase->UARTSTAbits.UTXISEL0 = 0; //Bit13 int when transmit buffer becomes empty
    
    psBase->UARTSTAbits.UTXINV = 0;   //Bit14 N/A, IRDA configuration



//    IPC2bits.U1RXIP = ISR_PRIORITY_UARTSRX;    // If RX and Tx Isr used must have equal priority??? 
//    IPC3bits.U1TXIP = ISR_PRIORITY_UARTSTX;    // If RX and Tx Isr used must have equal priority???  (Tx=4 and Rx=6 Fails)  
//    IFS0bits.U1TXIF = 0;    // Clear the Transmit Interrupt Flag
//    IEC0bits.U1TXIE = 0;    // Enable/Disable Transmit Interrupts
//    IFS0bits.U1RXIF = 0;    // Clear the Receive Interrupt Flag
//    IEC0bits.U1RXIE = USE_INT_UART_RX;    // Enable/Disable Receive Interrupts
    #endif

    #if !defined (BOARDCFG_USE_MCC_UART) || (MCC_RX_BUFFER_NOT_USED == 0)

    ISR_vSetDatIPC(UART_sHandle.u8BitOffsetsISRRx[index],ISR_PRIORITY_UARTSRX);    // interrupt priority level
    ISR_vSetDatIPC(UART_sHandle.u8BitOffsetsISRTx[index],ISR_PRIORITY_UARTSTX);    // interrupt priority level
    ISR_vClrBitIFS(UART_sHandle.u8BitOffsetsISRTx[index]);    // Clear interrupt
    ISR_vClrBitIEC(UART_sHandle.u8BitOffsetsISRTx[index]);    // disable interrupt
    ISR_vClrBitIFS(UART_sHandle.u8BitOffsetsISRRx[index]);    // Clear interrupt
    #if USE_INT_UART_RX
    ISR_vSetBitIEC(UART_sHandle.u8BitOffsetsISRRx[index]);    // Enable interrupt
    #else
    ISR_vClrBitIEC(UART_sHandle.u8BitOffsetsISRRx[index]);    // disable interrupt
    #endif

    ISR_vSetDatIPC(UART_sHandle.u8BitOffsetsISRErr[index],ISR_PRIORITY_UARTSERR);
    #endif

    ISR_vClrBitIFS(UART_sHandle.u8BitOffsetsISRErr[index]);    // Clear interrupt
    #if USE_INT_UART_ERR
    ISR_vSetBitIEC(UART_sHandle.u8BitOffsetsISRErr[index]);    // enable interrupt
    #else
    ISR_vClrBitIEC(UART_sHandle.u8BitOffsetsISRErr[index]);    // disable interrupt
    #endif
    
    #if !defined (BOARDCFG_USE_MCC_UART)
    psBase->UARTMODEbits.UARTEN = 1;  // And turn the peripheral on
    psBase->UARTSTAbits.UTXEN = 1;  //UTXEN after UARTEN otherwise no transmissions
    
    /* UARTs PPS */
	__builtin_write_OSCCONL(OSCCON & (~(1<<6))); // clear bit 6 


    #ifdef U1TX_PIN_PPS
    U1TX_PIN_PPS = 1;                   //RP as U1TX
    #endif
    #ifdef U1RX_PIN_PPS
    RPINR18bits.U1RXR = U1RX_PIN_PPS;   //RP as U1RX
    #endif

    #ifdef U2TX_PIN_PPS
    U2TX_PIN_PPS = 3;                   //RP as U2TX
    #endif
    #ifdef U2RX_PIN_PPS
    RPINR19bits.U2RXR = U2RX_PIN_PPS;   //RP as U2RX
    #endif

    #ifdef U3TX_PIN_PPS
    U3TX_PIN_PPS = 0b011011;            //RP as U3TX
    #endif
    #ifdef U3RX_PIN_PPS
    RPINR27bits.U3RXR = U3RX_PIN_PPS;   //RP as U3RX
    #endif

    #ifdef U4TX_PIN_PPS
    U4TX_PIN_PPS = 0b011101;            //RP as U4TX
    #endif
    #ifdef U4RX_PIN_PPS
    RPINR28bits.U4RXR = U4RX_PIN_PPS;   //RP as U4RX
    #endif


	__builtin_write_OSCCONL(OSCCON | (1<<6)); 	 // Set bit 6 	

    #endif
}




void UART_TransmitEnable(UART_eModule_t index)
{
    switch(index)
    {
        case UART_1:
            TXEN_U1_SetEnable();
            break;
        case UART_2:
            TXEN_U2_SetEnable();
            break;
        case UART_3:
            TXEN_U3_SetEnable();
            break;
        case UART_4:
            TXEN_U4_SetEnable();
            break;
        default:
            break;
    }
}


void UART_TransmitDisable(UART_eModule_t index)
    {
    switch(index)
    {
        case UART_1:
            TXEN_U1_SetDisable();
            break;
        case UART_2:
            TXEN_U2_SetDisable();
            break;
        case UART_3:
            TXEN_U3_SetDisable();
            break;
        case UART_4:
            TXEN_U4_SetDisable();
            break;
        default:
            break;
    }
}




/* *****************************************************************************
 * UART1_bOverflowCheckClear
 * 
 * Description: UART Overflow Check and Reset if present.
 **************************************************************************** */
bool UART_bOverflowCheckClear(UART_eModule_t index)
{
#if USE_INT_UART_ERR == 0
    volatile UART_sBaseRegisters_t* psBase = UART_sHandle.psBase[index];
    
    bool bResult = 0;
    
    /* Uart Overflow Check */
    if ((psBase->UARTSTAbits.OERR == 1))
    {
        bResult = 1;
        psBase->UARTSTAbits.OERR = 0;
        UART_RXOverflowCounter[index]++;
    }
    return bResult;
#else
    return 0;
#endif
}




bool UART_ReadDataRxAvailable(UART_eModule_t index)
{
    bool result;
    
    #if !defined (BOARDCFG_USE_MCC_UART) || (MCC_RX_BUFFER_NOT_USED == 1)
    if (UARTRxBufferRDCount[index] != UARTRxBufferWRCount[index])
    #else
    if (UART_IsRxReady(index))
    #endif
    {
        result = true;
    }
    else   
    {
        result = false;
    }
    return result;    
}


uint32_t UART_ReadDataRxTimeDiff(UART_eModule_t index)
{
    return u32TimeDiffRead[index];   
}

uint8_t UART_ReadDataRx(UART_eModule_t index)
{
    uint8_t result;
    
    #if !defined (BOARDCFG_USE_MCC_UART) || (MCC_RX_BUFFER_NOT_USED == 1)
    if (UARTRxBufferRDCount[index] != UARTRxBufferWRCount[index])
    #else
    if (UART_IsRxReady(index))
    #endif
    {
        #if defined (BOARDCFG_USE_MCC_UART) && (MCC_RX_BUFFER_NOT_USED == 0)
        if (UARTRxBufferRDCount[index] != UARTRxBufferWRCount[index])
        #endif
        {
            UARTRxBufferRDCount[index]++;
            u32TimeDiffRead[index] = UARTRxBufferTimeDiff[index][UARTRxBufferRDIndex[index]];
            #if !defined (BOARDCFG_USE_MCC_UART) || (MCC_RX_BUFFER_NOT_USED == 1)
            result = UARTRxBuffer[index][UARTRxBufferRDIndex[index]++];
            #else
            result = UART_GetChar(index);
            UARTRxBuffer[index][UARTRxBufferRDIndex[index]] = result;
            UARTRxBufferRDIndex[index]++;
            #endif
            if (UARTRxBufferRDIndex[index] >= UART_RX_BUFFER_SIZE)
            {
                UARTRxBufferRDIndex[index] = 0;
            }        
        }
        #if defined (BOARDCFG_USE_MCC_UART) && (MCC_RX_BUFFER_NOT_USED == 0)
        else    /* multiple bytes were read in Rx Isr and assume no time differencre between bytes */
        {
            u32TimeDiffRead[index] = 0;
            result = UART_GetChar(index);
        }
        #endif
    }
    else
    {
        result = 0xEE;      /* invalid data */
    }
    return result;
}



#if USE_INT_UART_ERR
/******************************************************************************
 * Function:        void __attribute__ ((interrupt, auto_psv)) _U1ErrInterrupt(void)
 *
 * PreCondition:    UART Module must be Initialized with transmit interrupt enabled.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Clear the UART1 Error Interrupt Flag
 *****************************************************************************/
void __attribute__ ( (interrupt, auto_psv) ) _U1ErrInterrupt( void )
{
    DEBUG_SET_USART1_ERR();
    if ((U1STAbits.OERR == 1))
    {
        UART_RXOverflowCounter[UART_1]++;
        U1STAbits.OERR = 0;
    }
    IFS4bits.U1EIF = 0;                 // Clear the UART1 Error Interrupt Flag
    DEBUG_CLR_USART1_ERR();
}
#endif

#ifndef BOARDCFG_USE_MCC_UART
/*******************************************************************************
 * Function:  void __attribute__ ((interrupt, auto_psv)) _U1RXInterrupt(void)
 *
 * PreCondition: UART Module must be Initialized with receive interrupt enabled.
 *
 * Input:        None
 *                  
 * Output:       None
 *
 * Side Effects: None
 *
 * Overview:     UART receive interrupt service routine called whenever a byte
 *               of data received in UART Rx buffer.
 *****************************************************************************/
void __attribute__ ( (interrupt, auto_psv) ) _U1RXInterrupt( void )
{
    DEBUG_SET_USART1_RX();
    
    //could use here that Tx Interrupt is disabled
    HMLIB_vProcessFastTD(UART_1);
    #if USE_CTREADER  
    CTREADER_vProcessFastTD(UART_1);
    #endif  

    while (U1STAbits.URXDA == 1) 
    {
        if ((UARTRxBufferWRCount[UART_1] - UARTRxBufferRDCount[UART_1]) <= UART_RX_BUFFER_SIZE) 
        {
            u32TimeDiffNow[UART_1] = HMDRV_GET_FREE_RUN_TIMER();
            UARTRxBufferTimeDiff[UART_1][UARTRxBufferWRIndex[UART_1]] = u32TimeDiffNow[UART_1] - u32TimeDiffLast[UART_1];
            u32TimeDiffLast[UART_1] = u32TimeDiffNow[UART_1];
            UARTRxBuffer[UART_1][UARTRxBufferWRIndex[UART_1]++] = U1RXREG;
            UARTRxBufferWRCount[UART_1]++;
            if (UARTRxBufferWRIndex[UART_1] >= UART_RX_BUFFER_SIZE)
            {
                UARTRxBufferWRIndex[UART_1] = 0;
            } 
            HMDRV_RESET_TX_DELAY(UART_1);
        }
        else
        {
            U1RXREG;
            UART_RXIntOverflowCounter[UART_1]++;
        }
    }
   
    IFS0bits.U1RXIF = 0;
    DEBUG_CLR_USART1_RX();
}



/******************************************************************************
 * Function:   void __attribute__ ((interrupt, auto_psv)) _U1TXInterrupt(void)
 *
 * PreCondition: UART Module must be Initialized with transmit interrupt enabled.
 *
 * Input:        None
 *                  
 * Output:       None
 *
 * Side Effects: None
 *
 * Overview:     UART transmit interrupt service routine called whenever a data
 *               is sent from UART Tx buffer
 *****************************************************************************/
void __attribute__ ( (interrupt, auto_psv) ) _U1TXInterrupt( void )
{
    DEBUG_SET_USART1_TX();
    IFS0bits.U1TXIF = 0;
    //HMLIB_vProcessSCITXISR();
    HMLIB_vProcessSCITXISRWithIndex(UART_1);
    #if USE_CTREADER  
    CTREADER_vProcessUartTx(UART_1);
    #endif  
    DEBUG_CLR_USART1_TX();
}

#else

#if (MCC_RX_BUFFER_NOT_USED == 1)
void __attribute__ ( (interrupt, auto_psv) ) _U1RXInterrupt( void )
{
    UART1_Receive_CallBack();
    IFS0bits.U1RXIF = 0;
}
#endif

/******************************************************************************
 * UART1_Receive_CallBack
 *****************************************************************************/
void UART1_Receive_CallBack ( void )
{ 
    DEBUG_SET_USART1_RX();
    
    //could use here that Tx Interrupt is disabled
    HMLIB_vProcessFastTD(UART_1);
    #if USE_CTREADER  
    CTREADER_vProcessFastTD(UART_1);
    #endif
    
    #if MCC_RX_BUFFER_NOT_USED
    while(U1STAbits.URXDA == 1)
    #endif
    {
    
    if ((UARTRxBufferWRCount[UART_1] - UARTRxBufferRDCount[UART_1]) <= UART_RX_BUFFER_SIZE) 
    {
        u32TimeDiffNow[UART_1] = HMDRV_GET_FREE_RUN_TIMER();
        UARTRxBufferTimeDiff[UART_1][UARTRxBufferWRIndex[UART_1]] = u32TimeDiffNow[UART_1] - u32TimeDiffLast[UART_1];
        u32TimeDiffLast[UART_1] = u32TimeDiffNow[UART_1];
        #if MCC_RX_BUFFER_NOT_USED
        UARTRxBuffer[UART_1][UARTRxBufferWRIndex[UART_1]++] = U1RXREG;
        #else
        UARTRxBufferWRIndex[UART_1]++;
        #endif
        UARTRxBufferWRCount[UART_1]++;
        if (UARTRxBufferWRIndex[UART_1] >= UART_RX_BUFFER_SIZE)
        {
            UARTRxBufferWRIndex[UART_1] = 0;
        } 
        HMDRV_RESET_TX_DELAY(UART_1);
    }
    else
    {
        #if MCC_RX_BUFFER_NOT_USED
        U1RXREG;
        #endif
        UART_RXIntOverflowCounter[UART_1]++;
    }
    }
    DEBUG_CLR_USART1_RX();
}
/******************************************************************************
 * UART1_Transmit_CallBack
 *****************************************************************************/
void UART1_Transmit_CallBack ( void )
{ 
    DEBUG_SET_USART1_TX();
    HMLIB_vProcessSCITXISRWithIndex(UART_1);
    #if USE_CTREADER  
    CTREADER_vProcessUartTx(UART_1);
    #endif
    DEBUG_CLR_USART1_TX();
}
#endif



#if USE_INT_UART_ERR
/******************************************************************************
 * Function:        void __attribute__ ((interrupt, auto_psv)) _U2ErrInterrupt(void)
 *
 * PreCondition:    UART Module must be Initialized with transmit interrupt enabled.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Clear the UART Error Interrupt Flag
 *****************************************************************************/
void __attribute__ ( (interrupt, auto_psv) ) _U2ErrInterrupt( void )
{
    DEBUG_SET_USART2_ERR();
    if ((U2STAbits.OERR == 1))
    {
        UART_RXOverflowCounter[UART_2]++;
        U2STAbits.OERR = 0;
    }
    IFS4bits.U2EIF = 0;                 // Clear the UART Error Interrupt Flag
    DEBUG_CLR_USART2_ERR();
}
#endif


#ifndef BOARDCFG_USE_MCC_UART
/*******************************************************************************
 * Function:  void __attribute__ ((interrupt, auto_psv)) _U2RXInterrupt(void)
 *
 * PreCondition: UART Module must be Initialized with receive interrupt enabled.
 *
 * Input:        None
 *                  
 * Output:       None
 *
 * Side Effects: None
 *
 * Overview:     UART receive interrupt service routine called whenever a byte
 *               of data received in UART Rx buffer.
 *****************************************************************************/
void __attribute__ ( (interrupt, auto_psv) ) _U2RXInterrupt( void )
{
    DEBUG_SET_USART2_RX();

    HMLIB_vProcessFastTD(UART_2);
    #if USE_CTREADER  
    CTREADER_vProcessFastTD(UART_2);
    #endif  
    
    while (U2STAbits.URXDA == 1) 
    {
        if((UARTRxBufferWRCount[UART_2] - UARTRxBufferRDCount[UART_2]) <= UART_RX_BUFFER_SIZE) 
        {
            u32TimeDiffNow[UART_2] = HMDRV_GET_FREE_RUN_TIMER();
            UARTRxBufferTimeDiff[UART_2][UARTRxBufferWRIndex[UART_2]] = u32TimeDiffNow[UART_2] - u32TimeDiffLast[UART_2];
            u32TimeDiffLast[UART_2] = u32TimeDiffNow[UART_2];
            UARTRxBuffer[UART_2][UARTRxBufferWRIndex[UART_2]++] = U2RXREG;
            UARTRxBufferWRCount[UART_2]++;
            if (UARTRxBufferWRIndex[UART_2] >= UART_RX_BUFFER_SIZE)
            {
                UARTRxBufferWRIndex[UART_2] = 0;
            } 
            HMDRV_RESET_TX_DELAY(UART_2);
        }
        else
        {
            U2RXREG;
            UART_RXIntOverflowCounter[UART_2]++;
        }
    }
    IFS1bits.U2RXIF = 0;
    DEBUG_CLR_USART2_RX();
}

/******************************************************************************
 * Function:   void __attribute__ ((interrupt, auto_psv)) _U2TXInterrupt(void)
 *
 * PreCondition: UART Module must be Initialized with transmit interrupt enabled.
 *
 * Input:        None
 *                  
 * Output:       None
 *
 * Side Effects: None
 *
 * Overview:     UART transmit interrupt service routine called whenever a data
 *               is sent from UART Tx buffer
 *****************************************************************************/
void __attribute__ ( (interrupt, auto_psv) ) _U2TXInterrupt( void )
{
    DEBUG_SET_USART2_TX();
    IFS1bits.U2TXIF = 0;
    //HMLIB_vProcessSCITXISR();
    HMLIB_vProcessSCITXISRWithIndex(UART_2);
    #if USE_CTREADER  
    CTREADER_vProcessUartTx(UART_2);
    #endif  
    DEBUG_CLR_USART2_TX();
}

#else
#if (MCC_RX_BUFFER_NOT_USED == 1)
void __attribute__ ( (interrupt, auto_psv) ) _U2RXInterrupt( void )
{
    UART2_Receive_CallBack();
    IFS1bits.U2RXIF = 0;
}
#endif
/******************************************************************************
 * UART2_Receive_CallBack
 *****************************************************************************/
void UART2_Receive_CallBack ( void )
{ 
    DEBUG_SET_USART2_RX();
    
    //could use here that Tx Interrupt is disabled
    HMLIB_vProcessFastTD(UART_2);
    #if USE_CTREADER  
    CTREADER_vProcessFastTD(UART_2);
    #endif
    
    #if MCC_RX_BUFFER_NOT_USED
    while(U2STAbits.URXDA == 1)
    #endif
    {
    if ((UARTRxBufferWRCount[UART_2] - UARTRxBufferRDCount[UART_2]) <= UART_RX_BUFFER_SIZE) 
    {
        u32TimeDiffNow[UART_2] = HMDRV_GET_FREE_RUN_TIMER();
        UARTRxBufferTimeDiff[UART_2][UARTRxBufferWRIndex[UART_2]] = u32TimeDiffNow[UART_2] - u32TimeDiffLast[UART_2];
        u32TimeDiffLast[UART_2] = u32TimeDiffNow[UART_2];
        #if MCC_RX_BUFFER_NOT_USED
        UARTRxBuffer[UART_2][UARTRxBufferWRIndex[UART_2]++] = U2RXREG;
        #else
        UARTRxBufferWRIndex[UART_2]++;
        #endif
        UARTRxBufferWRCount[UART_2]++;
        if (UARTRxBufferWRIndex[UART_2] >= UART_RX_BUFFER_SIZE)
        {
            UARTRxBufferWRIndex[UART_2] = 0;
        } 
        HMDRV_RESET_TX_DELAY(UART_2);
    }
    else
    {
        #if MCC_RX_BUFFER_NOT_USED
        U2RXREG;
        #endif
        UART_RXIntOverflowCounter[UART_2]++;
    }
    }
    DEBUG_CLR_USART2_RX();
}
/******************************************************************************
 * UART2_Transmit_CallBack
 *****************************************************************************/
void UART2_Transmit_CallBack ( void )
{ 
    DEBUG_SET_USART2_TX();
    HMLIB_vProcessSCITXISRWithIndex(UART_2);
    #if USE_CTREADER  
    CTREADER_vProcessUartTx(UART_2);
    #endif
    DEBUG_CLR_USART2_TX();
}
#endif


#if USE_INT_UART_ERR
/******************************************************************************
 * Function:        void __attribute__ ((interrupt, auto_psv)) _U3ErrInterrupt(void)
 *
 * PreCondition:    UART Module must be Initialized with transmit interrupt enabled.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Clear the UART3 Error Interrupt Flag
 *****************************************************************************/
void __attribute__ ( (interrupt, auto_psv) ) _U3ErrInterrupt( void )
{
    DEBUG_SET_USART3_ERR();
    if ((U3STAbits.OERR == 1))
    {
        UART_RXOverflowCounter[UART_3]++;
        U3STAbits.OERR = 0;
    }
    IFS5bits.U3EIF = 0;                 // Clear the UART3 Error Interrupt Flag
    DEBUG_CLR_USART3_ERR();
}
#endif

#ifndef BOARDCFG_USE_MCC_UART
/*******************************************************************************
 * Function:  void __attribute__ ((interrupt, auto_psv)) _U3RXInterrupt(void)
 *
 * PreCondition: UART Module must be Initialized with receive interrupt enabled.
 *
 * Input:        None
 *                  
 * Output:       None
 *
 * Side Effects: None
 *
 * Overview:     UART receive interrupt service routine called whenever a byte
 *               of data received in UART Rx buffer.
 *****************************************************************************/
void __attribute__ ( (interrupt, auto_psv) ) _U3RXInterrupt( void )
{
    DEBUG_SET_USART3_RX();

    HMLIB_vProcessFastTD(UART_3);
    #if USE_CTREADER  
    CTREADER_vProcessFastTD(UART_3);
    #endif  

    while (U3STAbits.URXDA == 1) 
    {
        if ((UARTRxBufferWRCount[UART_3] - UARTRxBufferRDCount[UART_3]) <= UART_RX_BUFFER_SIZE) 
        {
            u32TimeDiffNow[UART_3] = HMDRV_GET_FREE_RUN_TIMER();
            UARTRxBufferTimeDiff[UART_3][UARTRxBufferWRIndex[UART_3]] = u32TimeDiffNow[UART_3] - u32TimeDiffLast[UART_3];
            u32TimeDiffLast[UART_3] = u32TimeDiffNow[UART_3];
            UARTRxBuffer[UART_3][UARTRxBufferWRIndex[UART_3]++] = U3RXREG;
            UARTRxBufferWRCount[UART_3]++;
            if (UARTRxBufferWRIndex[UART_3] >= UART_RX_BUFFER_SIZE)
            {
                UARTRxBufferWRIndex[UART_3] = 0;
            } 
            HMDRV_RESET_TX_DELAY(UART_3);
        }
        else
        {
            U3RXREG;
            UART_RXIntOverflowCounter[UART_3]++;
        }
    }
    IFS5bits.U3RXIF = 0;
    DEBUG_CLR_USART3_RX();
}

/******************************************************************************
 * Function:   void __attribute__ ((interrupt, auto_psv)) _U3TXInterrupt(void)
 *
 * PreCondition: UART Module must be Initialized with transmit interrupt enabled.
 *
 * Input:        None
 *                  
 * Output:       None
 *
 * Side Effects: None
 *
 * Overview:     UART transmit interrupt service routine called whenever a data
 *               is sent from UART Tx buffer
 *****************************************************************************/
void __attribute__ ( (interrupt, auto_psv) ) _U3TXInterrupt( void )
{
    DEBUG_SET_USART3_TX();
    IFS5bits.U3TXIF = 0;
    //HMLIB_vProcessSCITXISR();
    HMLIB_vProcessSCITXISRWithIndex(UART_3);
    #if USE_CTREADER  
    CTREADER_vProcessUartTx(UART_3);
    #endif  
    DEBUG_CLR_USART3_TX();
}

#else
#if (MCC_RX_BUFFER_NOT_USED == 1)
void __attribute__ ( (interrupt, auto_psv) ) _U3RXInterrupt( void )
{
    UART3_Receive_CallBack();
    IFS5bits.U3RXIF = 0;
}
#endif
/******************************************************************************
 * UART3_Receive_CallBack
 *****************************************************************************/
void UART3_Receive_CallBack ( void )
{ 
    DEBUG_SET_USART3_RX();
    
    //could use here that Tx Interrupt is disabled
    HMLIB_vProcessFastTD(UART_3);
    #if USE_CTREADER  
    CTREADER_vProcessFastTD(UART_3);
    #endif
    
    #if MCC_RX_BUFFER_NOT_USED
    while(U3STAbits.URXDA == 1)
    #endif
    {
    if ((UARTRxBufferWRCount[UART_3] - UARTRxBufferRDCount[UART_3]) <= UART_RX_BUFFER_SIZE) 
    {
        u32TimeDiffNow[UART_3] = HMDRV_GET_FREE_RUN_TIMER();
        UARTRxBufferTimeDiff[UART_3][UARTRxBufferWRIndex[UART_3]] = u32TimeDiffNow[UART_3] - u32TimeDiffLast[UART_3];
        u32TimeDiffLast[UART_3] = u32TimeDiffNow[UART_3];
        #if MCC_RX_BUFFER_NOT_USED
        UARTRxBuffer[UART_3][UARTRxBufferWRIndex[UART_3]++] = U3RXREG;
        #else
        UARTRxBufferWRIndex[UART_3]++;
        #endif
        UARTRxBufferWRCount[UART_3]++;
        if (UARTRxBufferWRIndex[UART_3] >= UART_RX_BUFFER_SIZE)
        {
            UARTRxBufferWRIndex[UART_3] = 0;
        } 
        HMDRV_RESET_TX_DELAY(UART_3);
    }
    else
    {
        #if MCC_RX_BUFFER_NOT_USED
        U3RXREG;
        #endif
        UART_RXIntOverflowCounter[UART_3]++;
    }
    }
    DEBUG_CLR_USART3_RX();
}
/******************************************************************************
 * UART3_Transmit_CallBack
 *****************************************************************************/
void UART3_Transmit_CallBack ( void )
{ 
    DEBUG_SET_USART3_TX();
    HMLIB_vProcessSCITXISRWithIndex(UART_3);
    #if USE_CTREADER  
    CTREADER_vProcessUartTx(UART_3);
    #endif
    DEBUG_CLR_USART3_TX();
}
#endif










#if USE_INT_UART_ERR
/******************************************************************************
 * Function:        void __attribute__ ((interrupt, auto_psv)) _U4ErrInterrupt(void)
 *
 * PreCondition:    UART Module must be Initialized with transmit interrupt enabled.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Clear the UART4 Error Interrupt Flag
 *****************************************************************************/
void __attribute__ ( (interrupt, auto_psv) ) _U4ErrInterrupt( void )
{
    DEBUG_SET_USART4_ERR();
    if ((U4STAbits.OERR == 1))
    {
        UART_RXOverflowCounter[UART_4]++;
        U4STAbits.OERR = 0;
    }
    IFS5bits.U4EIF = 0;                 // Clear the UART4 Error Interrupt Flag
    DEBUG_CLR_USART4_ERR();
}
#endif


#ifndef BOARDCFG_USE_MCC_UART
/*******************************************************************************
 * Function:  void __attribute__ ((interrupt, auto_psv)) _U4RXInterrupt(void)
 *
 * PreCondition: UART Module must be Initialized with receive interrupt enabled.
 *
 * Input:        None
 *                  
 * Output:       None
 *
 * Side Effects: None
 *
 * Overview:     UART receive interrupt service routine called whenever a byte
 *               of data received in UART Rx buffer.
 *****************************************************************************/
void __attribute__ ( (interrupt, auto_psv) ) _U4RXInterrupt( void )
{
    DEBUG_SET_USART4_RX();

    HMLIB_vProcessFastTD(UART_4);
    #if USE_CTREADER  
    CTREADER_vProcessFastTD(UART_4);
    #endif  

    while (U4STAbits.URXDA == 1) 
    {
        if ((UARTRxBufferWRCount[UART_4] - UARTRxBufferRDCount[UART_4]) <= UART_RX_BUFFER_SIZE)
        {
            u32TimeDiffNow[UART_4] = HMDRV_GET_FREE_RUN_TIMER();
            UARTRxBufferTimeDiff[UART_4][UARTRxBufferWRIndex[UART_4]] = u32TimeDiffNow[UART_4] - u32TimeDiffLast[UART_4];
            u32TimeDiffLast[UART_4] = u32TimeDiffNow[UART_4];
            UARTRxBuffer[UART_4][UARTRxBufferWRIndex[UART_4]++] = U4RXREG;
            UARTRxBufferWRCount[UART_4]++;
            if (UARTRxBufferWRIndex[UART_4] >= UART_RX_BUFFER_SIZE)
            {
                UARTRxBufferWRIndex[UART_4] = 0;
            } 
            HMDRV_RESET_TX_DELAY(UART_4);
        }
        else
        {
            U4RXREG;
            UART_RXIntOverflowCounter[UART_4]++;
        }
    }
    IFS5bits.U4RXIF = 0;
    DEBUG_CLR_USART4_RX();
}

/******************************************************************************
 * Function:   void __attribute__ ((interrupt, auto_psv)) _U4TXInterrupt(void)
 *
 * PreCondition: UART Module must be Initialized with transmit interrupt enabled.
 *
 * Input:        None
 *                  
 * Output:       None
 *
 * Side Effects: None
 *
 * Overview:     UART transmit interrupt service routine called whenever a data
 *               is sent from UART Tx buffer
 *****************************************************************************/
void __attribute__ ( (interrupt, auto_psv) ) _U4TXInterrupt( void )
{
    DEBUG_SET_USART4_TX();
    IFS5bits.U4TXIF = 0;
    //HMLIB_vProcessSCITXISR();
    HMLIB_vProcessSCITXISRWithIndex(UART_4);
    #if USE_CTREADER  
    CTREADER_vProcessUartTx(UART_4);
    #endif  
    DEBUG_CLR_USART4_TX();
}

#else
#if (MCC_RX_BUFFER_NOT_USED == 1)
void __attribute__ ( (interrupt, auto_psv) ) _U4RXInterrupt( void )
{
    UART4_Receive_CallBack();
    IFS5bits.U4RXIF = 0;
}
#endif
/******************************************************************************
 * UART4_Receive_CallBack
 *****************************************************************************/
void UART4_Receive_CallBack ( void )
{ 
    DEBUG_SET_USART4_RX();
    
    //could use here that Tx Interrupt is disabled
    HMLIB_vProcessFastTD(UART_4);
    #if USE_CTREADER  
    CTREADER_vProcessFastTD(UART_4);
    #endif
    
    #if MCC_RX_BUFFER_NOT_USED
    while(U4STAbits.URXDA == 1)
    #endif
    {
    if ((UARTRxBufferWRCount[UART_4] - UARTRxBufferRDCount[UART_4]) <= UART_RX_BUFFER_SIZE) 
    {
        u32TimeDiffNow[UART_4] = HMDRV_GET_FREE_RUN_TIMER();
        UARTRxBufferTimeDiff[UART_4][UARTRxBufferWRIndex[UART_4]] = u32TimeDiffNow[UART_4] - u32TimeDiffLast[UART_4];
        u32TimeDiffLast[UART_4] = u32TimeDiffNow[UART_4];
        #if MCC_RX_BUFFER_NOT_USED
        UARTRxBuffer[UART_4][UARTRxBufferWRIndex[UART_4]++] = U4RXREG;
        #else
        UARTRxBufferWRIndex[UART_4]++;
        #endif
        UARTRxBufferWRCount[UART_4]++;
        if (UARTRxBufferWRIndex[UART_4] >= UART_RX_BUFFER_SIZE)
        {
            UARTRxBufferWRIndex[UART_4] = 0;
        } 
        HMDRV_RESET_TX_DELAY(UART_4);
    }
    else
    {
        #if MCC_RX_BUFFER_NOT_USED
        U4RXREG;
        #endif
        UART_RXIntOverflowCounter[UART_4]++;
    }
    }
    DEBUG_CLR_USART4_RX();
}
/******************************************************************************
 * UART4_Transmit_CallBack
 *****************************************************************************/
void UART4_Transmit_CallBack ( void )
{ 
    DEBUG_SET_USART4_TX();
    HMLIB_vProcessSCITXISRWithIndex(UART_4);
    #if USE_CTREADER  
    CTREADER_vProcessUartTx(UART_4);
    #endif
    DEBUG_CLR_USART4_TX();
}
#endif



void UART_DRV_vInit(void)
{
    #ifndef BOARDCFG_USE_MCC_UART

    UART_vReset();
    UART_vInit(SYSTEM_eUartHexMon, CLK_DRV_u32GetFcy(), &sHexMonUartModuleConfiguration);
    if (SYSTEM_eUartModBus != SYSTEM_eUartHexMon)
    {
        UART_vInit(SYSTEM_eUartModBus, CLK_DRV_u32GetFcy(), &sModBusUartModuleConfiguration);
    }
    #if USE_CTREADER  
    UART_vInit(SYSTEM_eUartCTRead, CLK_DRV_u32GetFcy(), &sCTReadUartModuleConfiguration);
    #endif  
    #if USE_INPUT_CAPTURE    
    INCAP_vInit(SYSTEM_eUartInCapt);   
    #endif

    #else

    #if BOARDCFG_USE_UART_1
    UART1_Initialize();
    #endif
    #if BOARDCFG_USE_UART_2
    UART2_Initialize();
    #endif
    #if BOARDCFG_USE_UART_3
    UART3_Initialize();
    #endif
    #if BOARDCFG_USE_UART_4
    UART4_Initialize();
    #endif
    
    #if BOARDCFG_USE_UART_1    
    UART_vInit(UART_1, 0, 0);
    #endif
    #if BOARDCFG_USE_UART_2
    UART_vInit(UART_2, 0, 0);
    #endif
    #if BOARDCFG_USE_UART_3
    UART_vInit(UART_3, 0, 0);
    #endif
    #if BOARDCFG_USE_UART_4
    UART_vInit(UART_4, 0, 0);
    #endif

    #endif

    /* Note: Consider when used MCC to initialize properly variables used below */
    HMSERIAL_vSetPortIndex(SYSTEM_eUartHexMon);
    HMMODBUS_vSetPortIndex(SYSTEM_eUartModBus);
    HMLIB_vSetPortIndexHexMon(SYSTEM_eUartHexMon);
    HMLIB_vSetPortIndexModBus(SYSTEM_eUartModBus);
    #if USE_CTREADER  
    CTREADER_u8UartIndex = SYSTEM_eUartCTRead;
    #endif
    
    
    #ifndef BOARDCFG_USE_MCC_UART
    sHexMonUartModuleConfigurationOld.eParityAndData = sHexMonUartModuleConfiguration.eParityAndData;
    sHexMonUartModuleConfigurationOld.u32BaudRate = sHexMonUartModuleConfiguration.u32BaudRate;
    sHexMonUartModuleConfigurationOld.eStopBits = sHexMonUartModuleConfiguration.eStopBits;
    
    sModBusUartModuleConfigurationOld.eParityAndData = sModBusUartModuleConfiguration.eParityAndData;
    sModBusUartModuleConfigurationOld.u32BaudRate = sModBusUartModuleConfiguration.u32BaudRate;
    sModBusUartModuleConfigurationOld.eStopBits = sModBusUartModuleConfiguration.eStopBits;
    
    SYSTEM_eUartModBusOld = SYSTEM_eUartModBus;
    SYSTEM_eUartHexMonOld = SYSTEM_eUartHexMon;
    SYSTEM_eUartInCaptOld = SYSTEM_eUartInCapt;
    
    #if USE_CTREADER  
    sCTReadUartModuleConfigurationOld.eParityAndData = sCTReadUartModuleConfiguration.eParityAndData;
    sCTReadUartModuleConfigurationOld.u32BaudRate = sCTReadUartModuleConfiguration.u32BaudRate;
    sCTReadUartModuleConfigurationOld.eStopBits = sCTReadUartModuleConfiguration.eStopBits;
    SYSTEM_eUartCTReadOld = SYSTEM_eUartCTRead;
    #endif
    
    #endif
    
    
}

#ifndef BOARDCFG_USE_MCC_UART
bool UART_DRV_bConfigurationChangeRequest(void)
{
    return    (   (SYSTEM_eUartModBusOld != SYSTEM_eUartModBus)     
                ||(SYSTEM_eUartHexMonOld != SYSTEM_eUartHexMon)     
                ||(SYSTEM_eUartInCaptOld != SYSTEM_eUartInCapt)     
                #if USE_CTREADER  
                ||(SYSTEM_eUartCTReadOld != SYSTEM_eUartCTRead)     
                ||(sCTReadUartModuleConfigurationOld.eParityAndData != sCTReadUartModuleConfiguration.eParityAndData)     
                ||(sCTReadUartModuleConfigurationOld.u32BaudRate != sCTReadUartModuleConfiguration.u32BaudRate)   
                ||(sCTReadUartModuleConfigurationOld.eStopBits != sCTReadUartModuleConfiguration.eStopBits)   
                #endif  
                ||(sHexMonUartModuleConfigurationOld.eParityAndData != sHexMonUartModuleConfiguration.eParityAndData)     
                ||(sHexMonUartModuleConfigurationOld.u32BaudRate != sHexMonUartModuleConfiguration.u32BaudRate)   
                ||(sHexMonUartModuleConfigurationOld.eStopBits != sHexMonUartModuleConfiguration.eStopBits)   
                ||(sModBusUartModuleConfigurationOld.eParityAndData != sModBusUartModuleConfiguration.eParityAndData)     
                ||(sModBusUartModuleConfigurationOld.u32BaudRate != sModBusUartModuleConfiguration.u32BaudRate)   
                ||(sModBusUartModuleConfigurationOld.eStopBits != sModBusUartModuleConfiguration.eStopBits)   );

}
#endif