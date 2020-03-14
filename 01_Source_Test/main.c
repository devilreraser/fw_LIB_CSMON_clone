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

 int16_t u16DummyCurrentPhaseA = (0 << 14);
 int16_t u16DummyCurrentPhaseB = (1 << 14);
 int16_t u16DummyCurrentPhaseC = (2 << 14);
 int16_t u16DummyVoltageDCLink = (0 << 14);
 int16_t u16DummyIncrementLoop = (1 << 8);


CSMON_eResponseCode_t eResponseCode_CSMON_eInit = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eProcess = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetServerOnStatus = CSMON_RESPONSE_CODE_OK;

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */
void ControlProcess(void);

/* *****************************************************************************
 * Functions
 **************************************************************************** */

/* *****************************************************************************
 * ControlProcess
 **************************************************************************** */
void ControlProcess(void)
{
    //
    // Test For Data Consistency
    //
    u16DummyCurrentPhaseA += u16DummyIncrementLoop;
    u16DummyCurrentPhaseB += u16DummyIncrementLoop;
    u16DummyCurrentPhaseC += u16DummyIncrementLoop;
    u16DummyVoltageDCLink +=(u16DummyIncrementLoop>>1);

    //
    // Process Passed Flag Set - Need to be called from Processes with higher priority level in order CSMON to be able to get meaning-full (consistent) data
    ASSERT(CSMON_eSetFlagProcessPassed (CSMON_ID_PROCESS_CONTROL_PRIMARY) != CSMON_RESPONSE_CODE_OK);
    // Check CSMON Response Code (... or Embed Assert For Debug) if needed

}


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
    if (eResponseCode_CSMON_eInit != CSMON_RESPONSE_CODE_OK)
    {
        /* If enters here - Fix Peripheral Frequency for Better Performance and Stability (DEVICE_LSPCLK_FREQ) */
        ASSERT(CSMON_u32GetBaudError_PPM(CSMON_ID_PERIPHERAL_SCI_MODBUS) >= CSMON_u32PercentToPPM(3.0));
    }

    //
    // Register Function Call In CSMON Timer Period ISR (default Timer Period is 50 usec)
    // For Debug and Control Process Emulation here is registered the ControlProcess Function
    //
    ASSERT(CSMON_eSetTimerPeriodISRFunctionRegister(ControlProcess) != CSMON_RESPONSE_CODE_OK);
    // Check CSMON Response Code (... or Embed Assert For Debug) if needed

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

