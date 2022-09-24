/* *****************************************************************************
 * File:   sci_drv.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 11 23 12:59
 * 
 * Description: ...
 * 
 **************************************************************************** */
#ifdef _WIN32
#pragma once
#endif /* _WIN32 */

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "sci_drv.h"
#include "driverlib.h"
#include "device.h"
#include "boardcfg.h"
#include "uart_drv.h"

#if BOARDCFG_USE_ANDREAS_SCI_INIT
#include "F2837xD_device.h"     /* F2837xD Headerfile Include File - For SCI Init For Andreas */
#endif
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
uint32_t SCI_ErrorRx[SCI_COUNT];

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */


void SCI_vErrorResetProcess(uint32_t base)
{
    if ((HWREGH(base + SCI_O_RXST) & SCI_RXST_RXERROR) == SCI_RXST_RXERROR)
    {
        SCI_performSoftwareReset(base);
        SCI_ErrorRx[SCI_eGetModuleIndexFromBase(base)]++;
    }
    if ((HWREGH(base + SCI_O_FFRX) & SCI_FFRX_RXFFOVF) == SCI_FFRX_RXFFOVF)
    {
        SCI_resetRxFIFO(base);
        SCI_ErrorRx[SCI_eGetModuleIndexFromBase(base)]++;
    }
}




#define SCIC_BAUDRATE     115200
#define SCIC_LSPCLKHZ   50000000
#define SCIC_BAUD_VAL   (uint16_t)((SCIC_LSPCLKHZ/(SCIC_BAUDRATE*8))-1)   /* 53.25 ~ 53 */


void SCI_vInitFIFORegisters(void)
{
#if BOARDCFG_USE_ANDREAS_SCI_INIT

    /* Clear Everything in CCR */
    ScicRegs.SCICCR.all = 0;

    /* Stop the SCI */
    ScicRegs.SCIFFTX.bit.TXFFIENA = 0;              /* Disable the FIFO */
    ScicRegs.SCICTL1.bit.TXENA = 0;                 /* Disable the SCI TX */
    ScicRegs.SCICTL1.bit.RXENA = 0;                 /* Disable the SCI RX */

    /* Baudrate Setup */
    ScicRegs.SCIHBAUD.bit.BAUD = (SCIC_BAUD_VAL & 0xFF00U) >> 8U;
    ScicRegs.SCILBAUD.bit.BAUD = SCIC_BAUD_VAL & 0x00FFU;

    /* Set parity, data length, number of stop bits, addr mode */
    ScicRegs.SCICCR.bit.ADDRIDLE_MODE = 1;          /* ADDR Mode */
    ScicRegs.SCICCR.bit.LOOPBKENA = 0;              /* Loopback OFF */
    ScicRegs.SCICCR.bit.PARITY = 1;                 /* Parity Even */
    ScicRegs.SCICCR.bit.PARITYENA = 1;              /* Parity Enabled */
    ScicRegs.SCICCR.bit.STOPBITS = 1;               /* TWO Stop Bits */
    ScicRegs.SCICCR.bit.SCICHAR = 7;                /* 8 Bits */

    /* Start the SCI */
    ScicRegs.SCICTL1.bit.TXENA = 1;                 /* Enable the SCI TX */
    ScicRegs.SCICTL1.bit.RXENA = 1;                 /* Enable the SCI RX */
    ScicRegs.SCICTL1.bit.SWRESET = 1;               /* Reset SCI */

    /* Enable the FIFO */
    ScicRegs.SCIFFTX.bit.SCIRST = 1;                /* Reset Rx/Tx SCI Channels */
    ScicRegs.SCIFFTX.bit.SCIFFENA = 1;              /* Enable the Fifo */
    ScicRegs.SCIFFTX.bit.TXFIFORESET = 1;           /* Reset Fifo TX */
    ScicRegs.SCIFFRX.bit.RXFIFORESET = 1;           /* Reset Fifo RX */

    /* Enable Interrupts */
    ScicRegs.SCIFFTX.bit.TXFFIENA = 1;
    ScicRegs.SCIFFRX.bit.RXFFIENA = 1;
    ScicRegs.SCICTL1.bit.RXERRINTENA = 0;           /* errors handled in main loop */

    /* Set the FIFO interrupt levels */
    ScicRegs.SCIFFTX.bit.TXFFIL = 1;                /* interrupt on each byte */
    ScicRegs.SCIFFRX.bit.RXFFIL = 1;                /* interrupt on each byte */

    /* Software Reset */
    ScicRegs.SCICTL1.bit.SWRESET = 0;               /* Reset SCI */
    ScicRegs.SCICTL1.bit.SWRESET = 1;               /* Reset SCI */

    /* Reset Tx Fifo */
    ScicRegs.SCIFFTX.bit.TXFIFORESET = 0;
    ScicRegs.SCIFFTX.bit.TXFIFORESET = 1;

    /* Reset Rx Fifo */
    ScicRegs.SCIFFRX.bit.RXFIFORESET = 0;
    ScicRegs.SCIFFRX.bit.RXFIFORESET = 1;
#endif
}



void SCI_vInitFIFO(uint32_t base, uint32_t baud, uint32_t config)
{

    HWREGH(base + SCI_O_CCR) = 0;   /* explicitly clear ADDRIDLE_MODE (in SCI_setConfig not cleared) */
    SCI_setConfig(base, DEVICE_LSPCLK_FREQ, baud, config);


    //to do -fix it
//    if (base == SCIC_BASE) //only TX
//    {
//        SCI_disableModule(base);
//        HWREGH(base + SCI_O_CTL1) |= (SCI_CTL1_TXENA |
//                                      SCI_CTL1_SWRESET);
//    }
//    else
//    {
        SCI_enableModule(base);
//    }



    SCI_disableLoopback(base);
    SCI_resetChannels(base);
    SCI_enableFIFO(base);

    //
    // RX and TX FIFO Interrupts Enabled
    //
    SCI_enableInterrupt(base, (SCI_INT_RXFF | SCI_INT_TXFF));
    SCI_disableInterrupt(base, SCI_INT_RXERR);

    SCI_setFIFOInterruptLevel(base, SCI_FIFO_TX1, SCI_FIFO_RX1);
    SCI_performSoftwareReset(base);

    SCI_resetTxFIFO(base);
    SCI_resetRxFIFO(base);

#ifdef AUTOBAUD
    //
    // Perform an autobaud lock.
    // SCI expects an 'a' or 'A' to lock the baud rate.
    //
    SCI_lockAutobaud(base);
#endif
}

SCI_eModule_t SCI_eGetModuleIndexFromBase(uint32_t base)
{
    return (SCI_eModule_t)((base - SCIA_BASE)/(SCIB_BASE - SCIA_BASE));
}

uint32_t SCI_u32GetBaseFromModuleIndex(SCI_eModule_t index)
{
    return (index * (SCIB_BASE - SCIA_BASE) + SCIA_BASE);
}

__interrupt void SCIA_TXFIFOISR(void)
{
    UART_eModule_t index = (UART_eModule_t)SCI_eGetModuleIndexFromBase(SCIA_BASE);
    UART_TXInterrupt(index);
    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXFF);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

__interrupt void SCIA_RXFIFOISR(void)
{
    UART_eModule_t index = (UART_eModule_t)SCI_eGetModuleIndexFromBase(SCIA_BASE);
    UART_RXInterrupt(index);
    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_RXFF);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

__interrupt void SCIB_TXFIFOISR(void)
{
    UART_eModule_t index = (UART_eModule_t)SCI_eGetModuleIndexFromBase(SCIB_BASE);
    UART_TXInterrupt(index);
    SCI_clearInterruptStatus(SCIB_BASE, SCI_INT_TXFF);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

__interrupt void SCIB_RXFIFOISR(void)
{
    UART_eModule_t index = (UART_eModule_t)SCI_eGetModuleIndexFromBase(SCIB_BASE);
    UART_RXInterrupt(index);
    SCI_clearInterruptStatus(SCIB_BASE, SCI_INT_RXFF);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

__interrupt void SCIC_TXFIFOISR(void)
{
    UART_eModule_t index = (UART_eModule_t)SCI_eGetModuleIndexFromBase(SCIC_BASE);
    UART_TXInterrupt(index);
    SCI_clearInterruptStatus(SCIC_BASE, SCI_INT_TXFF);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP8);
}

__interrupt void SCIC_RXFIFOISR(void)
{
    UART_eModule_t index = (UART_eModule_t)SCI_eGetModuleIndexFromBase(SCIC_BASE);
    UART_RXInterrupt(index);
    SCI_clearInterruptStatus(SCIC_BASE, SCI_INT_RXFF);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP8);
}

__interrupt void SCID_TXFIFOISR(void)
{
    UART_eModule_t index = (UART_eModule_t)SCI_eGetModuleIndexFromBase(SCID_BASE);
    UART_TXInterrupt(index);
    SCI_clearInterruptStatus(SCID_BASE, SCI_INT_TXFF);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP8);
}

__interrupt void SCID_RXFIFOISR(void)
{
    UART_eModule_t index = (UART_eModule_t)SCI_eGetModuleIndexFromBase(SCID_BASE);
    UART_RXInterrupt(index);
    SCI_clearInterruptStatus(SCID_BASE, SCI_INT_RXFF);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP8);
}




