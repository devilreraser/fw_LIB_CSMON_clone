/* *****************************************************************************
 * File:   main.c
 * Author: Dimitar Lilov
 *
 * Created on 2020 03 13 09:06
 * 
 * Description: Test and Example for CSMON_C28xx library integration.
 * 
 **************************************************************************** */
#ifdef _WIN32
#pragma once
#endif /* _WIN32 */

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdbool.h>
#include "main.h"

#include "device.h"

#include "csmon.h"

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
bool bDummyStatusDeviceRunning = false;

CSMON_eResponseCode_t eResponseCode_CSMON_eInit = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eProcess = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetServerOnStatus = CSMON_RESPONSE_CODE_OK;

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */

/* *****************************************************************************
 * main
 **************************************************************************** */
void main(void)
{
    //
    // Configure PLL, disable WD, enable peripheral clocks.
    //
    Device_init();

    //
    // Disable pin locks and enable internal pullups.
    //
    Device_initGPIO();

    //
    // Clear all interrupts and initialize PIE vector table:
    // Disable CPU interrupts
    // Initialize interrupt controller and vector table.
    //
    Interrupt_initModule();
    Interrupt_initVectorTable();


    //
    // CSMON Initialization
    //
    eResponseCode_CSMON_eInit = CSMON_eInit();
    // Check CSMON Response Code if needed

    //
    // Reset the watchdog counter
    //
    SysCtl_serviceWatchdog();

    //
    // Enable the watchdog
    //
    SysCtl_enableWatchdog();

    //
    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    //
    EINT;
    ERTM;

    for (;;)
    {
        //
        // Reset the watchdog counter
        //
        SysCtl_serviceWatchdog();

        //
        // CSMON Process In Main Loop
        //
        eResponseCode_CSMON_eSetServerOnStatus = CSMON_eProcess();
        // Check CSMON Response Code if needed

        //
        // Device Running Control Indication - Set on Enter/Exit Run Mode
        //
        eResponseCode_CSMON_eSetServerOnStatus = CSMON_eSetServerOnStatus(bDummyStatusDeviceRunning);
        // Check CSMON Response Code if needed

    }

}

