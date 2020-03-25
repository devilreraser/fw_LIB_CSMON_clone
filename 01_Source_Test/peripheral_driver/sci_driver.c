/* *****************************************************************************
 * File:   sci_driver.c
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
#include "device.h"
#include "sci_driver.h"
#include "uart_driver.h"

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
uint32_t SCI_DRV_ErrorRx[SCI_DRV_COUNT];

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */


void SCI_DRV_vErrorResetProcess(uint32_t base)
{
    if ((HWREGH(base + SCI_O_RXST) & SCI_RXST_RXERROR) == SCI_RXST_RXERROR)
    {
        SCI_performSoftwareReset(base);
        SCI_DRV_ErrorRx[SCI_DRV_eGetModuleIndexFromBase(base)]++;
    }
}





void SCI_DRV_vInitFIFO(uint32_t base, uint32_t baud, uint32_t config)
{

    HWREGH(base + SCI_O_CCR) = 0;   /* explicitly clear ADDRIDLE_MODE (in SCI_setConfig not cleared) */
    SCI_setConfig(base, SysCtl_getLowSpeedClock(DEVICE_OSCSRC_FREQ), baud, config);
    SCI_enableModule(base);

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

}

SCI_DRV_eModule_t SCI_DRV_eGetModuleIndexFromBase(uint32_t base)
{
    return (SCI_DRV_eModule_t)((base - SCIA_BASE)/(SCIB_BASE - SCIA_BASE));
}

uint32_t SCI_DRV_u32GetBaseFromModuleIndex(SCI_DRV_eModule_t index)
{
    return (index * (SCIB_BASE - SCIA_BASE) + SCIA_BASE);
}

__interrupt void SCIA_DRV_TXFIFOISR(void)
{
    UART_DRV_eModule_t index = (UART_DRV_eModule_t)SCI_DRV_eGetModuleIndexFromBase(SCIA_BASE);
    UART_DRV_TXInterrupt(index);
    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXFF);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

__interrupt void SCIA_DRV_RXFIFOISR(void)
{
    UART_DRV_eModule_t index = (UART_DRV_eModule_t)SCI_DRV_eGetModuleIndexFromBase(SCIA_BASE);
    UART_DRV_RXInterrupt(index);
    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_RXFF);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

__interrupt void SCIB_DRV_TXFIFOISR(void)
{
    UART_DRV_eModule_t index = (UART_DRV_eModule_t)SCI_DRV_eGetModuleIndexFromBase(SCIB_BASE);
    UART_DRV_TXInterrupt(index);
    SCI_clearInterruptStatus(SCIB_BASE, SCI_INT_TXFF);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

__interrupt void SCIB_DRV_RXFIFOISR(void)
{
    UART_DRV_eModule_t index = (UART_DRV_eModule_t)SCI_DRV_eGetModuleIndexFromBase(SCIB_BASE);
    UART_DRV_RXInterrupt(index);
    SCI_clearInterruptStatus(SCIB_BASE, SCI_INT_RXFF);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

__interrupt void SCIC_DRV_TXFIFOISR(void)
{
    UART_DRV_eModule_t index = (UART_DRV_eModule_t)SCI_DRV_eGetModuleIndexFromBase(SCIC_BASE);
    UART_DRV_TXInterrupt(index);
    SCI_clearInterruptStatus(SCIC_BASE, SCI_INT_TXFF);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP8);
}

__interrupt void SCIC_DRV_RXFIFOISR(void)
{
    UART_DRV_eModule_t index = (UART_DRV_eModule_t)SCI_DRV_eGetModuleIndexFromBase(SCIC_BASE);
    UART_DRV_RXInterrupt(index);
    SCI_clearInterruptStatus(SCIC_BASE, SCI_INT_RXFF);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP8);
}

__interrupt void SCID_DRV_TXFIFOISR(void)
{
    UART_DRV_eModule_t index = (UART_DRV_eModule_t)SCI_DRV_eGetModuleIndexFromBase(SCID_BASE);
    UART_DRV_TXInterrupt(index);
    SCI_clearInterruptStatus(SCID_BASE, SCI_INT_TXFF);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP8);
}

__interrupt void SCID_DRV_RXFIFOISR(void)
{
    UART_DRV_eModule_t index = (UART_DRV_eModule_t)SCI_DRV_eGetModuleIndexFromBase(SCID_BASE);
    UART_DRV_RXInterrupt(index);
    SCI_clearInterruptStatus(SCID_BASE, SCI_INT_RXFF);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP8);
}




