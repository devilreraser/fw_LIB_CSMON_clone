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
#define PARAMETER_COUNT         10

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
typedef struct
{
    uint_least8_t u8Seconds;
    uint_least8_t u8Minutes;
    uint_least8_t u8Hours;
    uint_least8_t u8WeekDays;
    uint_least8_t u8Day;
    uint_least8_t u8Month;
    uint_least8_t u8Year;
    uint_least8_t u8Reserved;
}MAIN_sDateTime_t;

typedef struct
{
    uint16_t u16ParameterIndexID;
    uint32_t u32RealAddress;
    uint16_t u16ParamAttributes;
    char* pu8Name[16];
    char* pu8Unit[8];
    uint32_t u32Max;
    uint32_t u32Min;
    uint32_t u32Def;
    float Norm;

}MAIN_sParameterList_t;

/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */
MAIN_sDateTime_t MAIN_sDateTime =
{
   0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00 /* 2001-01-01-Mon-00:00:00 */
};

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
bool bDummyStatusDeviceRunning = false;
bool bDummyRequestDeviceRunning = false;

 int16_t s16DummyCurrentPhaseA = (0 << 14);
 int16_t s16DummyCurrentPhaseB = (1 << 14);
 int16_t s16DummyCurrentPhaseC = (2 << 14);
 int16_t s16DummyVoltageDCLink = (0 << 14);
 int16_t s16DummyIncrementLoop = (1 << 8);


CSMON_eResponseCode_t eResponseCode_CSMON_eInit = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eProcess = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetServerOnStatus = CSMON_RESPONSE_CODE_OK;

/* Dummy Parameter List */
const MAIN_sParameterList_t asParameterList[PARAMETER_COUNT] =
{
/* u16ParameterIndexID;                 u32RealAddress;           u16ParamAttributes;     pu8Name;            pu8Unit;            u32Max;                 u32Min;              u32Def;             Norm; */

 {      1000            ,  (uint32_t)&s16DummyCurrentPhaseA   ,      0x0002           , {"CurrentPhA"}    ,    {"A"}      ,   (uint32_t)10000   ,   (uint32_t)(-10000)  ,      (uint32_t)(0)    ,    0.1 },
 {      1001            ,  (uint32_t)&s16DummyCurrentPhaseB   ,      0x0002           , {"CurrentPhB"}    ,    {"A"}      ,   (uint32_t)10000   ,   (uint32_t)(-10000)  ,      (uint32_t)(0)    ,    0.1 },
 {      1002            ,  (uint32_t)&s16DummyCurrentPhaseC   ,      0x0002           , {"CurrentPhC"}    ,    {"A"}      ,   (uint32_t)10000   ,   (uint32_t)(-10000)  ,      (uint32_t)(0)    ,    0.1 },
 {      1003            ,  (uint32_t)&s16DummyVoltageDCLink   ,      0x0002           , {"VoltageBus"}    ,    {"V"}      ,   (uint32_t)13500   ,     (uint32_t)(5000)  ,    (uint32_t)(800)    ,    0.1 },
 {      1004            ,  (uint32_t)&s16DummyIncrementLoop   ,      0x0002           , {"IncLoopTst"}    ,    {"A(0.5V)"},    (uint32_t)1024   ,    (uint32_t)(-1024)  ,    (uint32_t)(256)    ,    0.1 },
 {      1005            ,(uint32_t)&bDummyRequestDeviceRunning,      0x0001           , {"RunRequest"}    ,    {"boolean"},    (uint32_t)true   ,    (uint32_t)false    ,    (uint32_t)false    ,      1 },
 {      1006            , (uint32_t)&bDummyStatusDeviceRunning,      0x0001           , {"RunStatus"}     ,    {"boolean"},    (uint32_t)true   ,    (uint32_t)false    ,    (uint32_t)false    ,      1 },

};

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */
void ControlProcess(void);

/* *****************************************************************************
 * Functions
 **************************************************************************** */

/* *****************************************************************************
 * CSMON_vSetDateTime - Set RTC Value - called from CSMON library
 **************************************************************************** */
void CSMON_vSetDateTime (
        uint_least8_t u8BCDSeconds,
        uint_least8_t u8BCDMinutes,
        uint_least8_t u8BCDHours,
        uint_least8_t u8BCDWeekdays,
        uint_least8_t u8BCDDay,
        uint_least8_t u8BCDMonth,
        uint_least8_t u8BCDYear)
{
    MAIN_sDateTime.u8Seconds    = u8BCDSeconds;
    MAIN_sDateTime.u8Minutes    = u8BCDMinutes;
    MAIN_sDateTime.u8Hours      = u8BCDHours;
    MAIN_sDateTime.u8WeekDays   = u8BCDWeekdays;
    MAIN_sDateTime.u8Day        = u8BCDDay;
    MAIN_sDateTime.u8Month      = u8BCDMonth;
    MAIN_sDateTime.u8Year       = u8BCDYear;
}

/* *****************************************************************************
 * CSMON_vGetDateTime - Get RTC Value - called from CSMON library
 **************************************************************************** */
void CSMON_vGetDateTime (
        uint_least8_t* pu8BCDSeconds,
        uint_least8_t* pu8BCDMinutes,
        uint_least8_t* pu8BCDHours,
        uint_least8_t* pu8BCDWeekdays,
        uint_least8_t* pu8BCDDay,
        uint_least8_t* pu8BCDMonth,
        uint_least8_t* pu8BCDYear)
{
        *pu8BCDSeconds  = MAIN_sDateTime.u8Seconds;
        *pu8BCDMinutes  = MAIN_sDateTime.u8Minutes;
        *pu8BCDHours    = MAIN_sDateTime.u8Hours;
        *pu8BCDWeekdays = MAIN_sDateTime.u8WeekDays;
        *pu8BCDDay      = MAIN_sDateTime.u8Day;
        *pu8BCDMonth    = MAIN_sDateTime.u8Month;
        *pu8BCDYear     = MAIN_sDateTime.u8Year;
}



/* *****************************************************************************
 * ControlProcess
 **************************************************************************** */
void ControlProcess(void)
{
    //
    // Test For Data Consistency and Control Emulation
    //
    if (bDummyRequestDeviceRunning)
    {
        bDummyStatusDeviceRunning = 1;
        s16DummyCurrentPhaseA += s16DummyIncrementLoop;
        s16DummyCurrentPhaseB += s16DummyIncrementLoop;
        s16DummyCurrentPhaseC += s16DummyIncrementLoop;
        s16DummyVoltageDCLink +=(s16DummyIncrementLoop>>1);
    }
    else
    {
        bDummyStatusDeviceRunning = 0;
        s16DummyCurrentPhaseA = 0;
        s16DummyCurrentPhaseB = 0;
        s16DummyCurrentPhaseC = 0;
        s16DummyVoltageDCLink = 0;
    }

    //
    // Process Passed Flag Set - Need to be called from Processes with higher priority level in order CSMON to be able to get meaning-full (consistent) data
    ASSERT(CSMON_eSetFlagProcessPassed (CSMON_ID_PROCESS_CONTROL_PRIMARY) != CSMON_RESPONSE_CODE_OK);
    // Check CSMON Response Code (... or Embed Assert For Debug) if needed
}

void ParameterInitialization(void)
{
    uint16_t u16Index;

    for (u16Index = 0; u16Index < PARAMETER_COUNT; u16Index++)
    {
        ASSERT(
            CSMON_eSetParameter (
                asParameterList.u16ParameterIndexID,
                asParameterList.u32RealAddress,
                asParameterList.u16ParamAttributes,
                asParameterList.pu8Name,
                asParameterList.pu8Unit,
                asParameterList.u32Max,
                asParameterList.u32Min,
                asParameterList.u32Def,
                asParameterList.Norm) != CSMON_RESPONSE_CODE_OK);
    }
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
    // CSMON Parameter Initialization
    //
    ParameterInitialization();

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



