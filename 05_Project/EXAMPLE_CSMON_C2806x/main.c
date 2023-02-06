/* *****************************************************************************
 * File:   main.c
 * Author: Damyan Neev
 *
 * Created on 2023 01 24 09:06
 * 
 * Description: Example for CSMON_C28xx library integration.
 * 
 **************************************************************************** */
 
/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "MotorParam/Generated/csmon_config.h"

#include "csmon_lib_support/csmon_lib_support.h"
#include "dev/dbg/ctrl/dbg_ctrl.h"
#include "hal.h"

#include "device.h"
#include "csmon_config.h"


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

static const uint32_t u32DelayCtrlLoop_Ticks = 1;


/* *****************************************************************************
 * Prototypes of functions definitions
 **************************************************************************** */



/* *****************************************************************************
 * Functions
 **************************************************************************** */
/* *****************************************************************************
 * ControlProcess
 **************************************************************************** */
void ControlProcess(void)
{
    DBG_CTRL_ControlProcessOnEnter();

    //
    // Test For Data Consistency and Control Emulation
    //
    if (bDummyReqstDevRunning)
    {
        if (bDummyStatsDevRunning == false)
        {
            bDummyStatsDevRunning = true;

            CSMON_vSetStartRecorderParameterMask(CSMON_MASK_RECORDER_0);

            s16DummyCurrentPhaseA = s16DummyCurrentPhaseAStartup;
            s16DummyCurrentPhaseB = s16DummyCurrentPhaseBStartup;
            s16DummyCurrentPhaseC = s16DummyCurrentPhaseCStartup;
            s16DummyVoltageDCLink = s16DummyVoltageDCLinkStartup;

            s32DummyCurrentPhaseA = (int32_t)s16DummyCurrentPhaseAStartup * 10000;
            s32DummyCurrentPhaseB = (int32_t)s16DummyCurrentPhaseBStartup * 10000;
            s32DummyCurrentPhaseC = (int32_t)s16DummyCurrentPhaseCStartup * 10000;
            s32DummyVoltageDCLink = (int32_t)s16DummyVoltageDCLinkStartup * 10000;
        }
        else
        {
            s16DummyCurrentPhaseA += s16DummyIncrementLoopA;
            s16DummyCurrentPhaseB += s16DummyIncrementLoopB;
            s16DummyCurrentPhaseC += s16DummyIncrementLoopC;
            s16DummyVoltageDCLink += s16DummyIncrementLoopV;

            s32DummyCurrentPhaseA += (int32_t)s16DummyIncrementLoopA * 10000;
            s32DummyCurrentPhaseB += (int32_t)s16DummyIncrementLoopB * 10000;
            s32DummyCurrentPhaseC += (int32_t)s16DummyIncrementLoopC * 10000;
            s32DummyVoltageDCLink += (int32_t)s16DummyIncrementLoopV * 10000;
        }

    }
    else
    {
        if (bDummyStatsDevRunning == true)
        {
            bDummyStatsDevRunning = false;
            CSMON_vSetStopRecorderParameterMask(CSMON_MASK_RECORDER_0);
        }

    }

    //
    // Artificial Delay Control Loop
    //
    SysCtl_delay(u32DelayCtrlLoop_Ticks);

    // Process Passed Flag Set - Need to be called from Processes with higher priority level in order CSMON to be able to get meaning-full (consistent) data
    csMonResponses.eSetFlagProcessPassed = CSMON_eSetFlagProcessPassed (CSMON_ID_PROCESS_CONTROL_PRIMARY);
    ASSERT(eSetFlagProcessPassed != CSMON_RESPONSE_CODE_OK);
    // Check CSMON Response Code (... or Embed Assert For Debug) if needed


    DBG_CTRL_ControlProcessOnExit();
}


/* *****************************************************************************
 * init
 **************************************************************************** */
void init(void)
{
    // Configure PLL, disable WD, enable peripheral clocks.
    Device_init();

    // Disable pin locks and enable internal PullUps.
    Device_initGPIO();

    HAL_GPIO_setup();

    CSMON_LIB_SUPPORT_init();

    //
    // Enable the WatchDog
    //
    SysCtl_serviceWatchdog();
    SysCtl_enableWatchdog();
    //SysCtl_disableWatchdog();

    EINT;
    ERTM;
}


/* *****************************************************************************
 * main
 **************************************************************************** */
void main(void)
{
    init();

    int controlPrescaler = 10;
    int controlPrescalerCounter = 10;

    for (;;)
    {
        // Reset the WatchDog counter
        SysCtl_serviceWatchdog();

        // CSMON Process In Main Loop - Big Delays On Disconnect 4-5ms; On Connect 12-35ms If Not Interrupted (EMIF Checksum PC Application)
        csMonResponses.eProcess = CSMON_eProcess();
        // Check CSMON Response Code if needed
        ASSERT(csMonResponses.eProcess != CSMON_RESPONSE_CODE_OK);

        controlPrescalerCounter++;
        if (controlPrescalerCounter >= controlPrescaler)
        {
            controlPrescalerCounter = 0;
            ControlProcess();
        }

        //
        // Device Running Control Indication - Set on Enter/Exit Run Mode
        //
        csMonResponses.eSetServerOnStatus = CSMON_eSetServerOnStatus(bDummyStatsDevRunning);
        // Check CSMON Response Code if needed
        ASSERT(csMonResponses.eSetServerOnStatus != CSMON_RESPONSE_CODE_OK);
    }
}

