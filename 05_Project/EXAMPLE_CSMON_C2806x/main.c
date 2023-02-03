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

#include "device.h"
#include "csmon_config.h"


/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

#define STAT_LED_EQEP1I_PIN 23



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
#ifdef _CS_1291
    GPIO_writePin_2806x(STAT_LED_EQEP1I_PIN, 1);     /* J17 at board corner before ground (pin 13 - second outside pin corner to middle) */
#endif

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

#ifdef _CS_1291
    GPIO_writePin_2806x(STAT_LED_EQEP1I_PIN, 0);     /* J17 at board corner before ground (pin 13 - second outside pin corner to middle) */
#endif
}


/* *****************************************************************************
 * Board_init
 **************************************************************************** */
static void Board_init(void)
{
#ifdef _2806x_ISO_CONTROL_CARD
    EALLOW;
    //  LED2 - red led (the middle led between LED3 and the power Supply green Led)
    GpioCtrlRegs.GPAPUD.bit.GPIO31 = 0;   // Enable pullup on GPIO31
    GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;  // GPIO31 = GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;  // GPIO31 = output
    GpioDataRegs.GPADAT.bit.GPIO31 = 0; // level

    //  LED3 - red led nearest to the micro USB
    GpioCtrlRegs.GPBPUD.bit.GPIO34 = 0;  // Enable pullup on GPIO34
    GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0; // GPIO34 = GPIO34
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;  // GPIO34 = output
    GpioDataRegs.GPBDAT.bit.GPIO34 = 0; // level
    EDIS;

#endif
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

    Board_init();

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

