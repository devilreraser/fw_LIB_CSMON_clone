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
//#include "emif_driver.h"
#include "sci_driver.h"
#include "uart_driver.h"
#include "fpga_sci_driver.h"
#include "csmon.h"
#include "parameter.h"
#include "datetime.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define CSMON_AUTOMATIC_SERVICE_WATCHDOG_IN_MAIN_LOOP   1

/* *****************************************************************************
 * Configuration Parameter Definitions
 **************************************************************************** */

#define CSMON_PAR_LIST_MINIMUM_COUNT                                    0
#define CSMON_PAR_LIST_MAXIMUM_COUNT                                    1                   /* Maximum Count */
#define CSMON_PAR_LIST_EACH_TYPE_REPEATED_ALL_TYPES_COUNT_TIMES         2                   /* Each Type Repeated All Types Count Times  */
#define CSMON_PAR_LIST_ALL_TYPES_REPEATED_ALL_TYPES_COUNT_TIMES         3                   /* All Types Repeated All Types Count Times  */
#define CSMON_PAR_LIST_RECORDER_DEBUG                                   4

//#define CSMON_PARAMETER_LIST_TEST   CSMON_PAR_LIST_MINIMUM_COUNT
//#define CSMON_PARAMETER_LIST_TEST   CSMON_PAR_LIST_MAXIMUM_COUNT
//#define CSMON_PARAMETER_LIST_TEST   CSMON_PAR_LIST_EACH_TYPE_REPEATED_ALL_TYPES_COUNT_TIMES
//#define CSMON_PARAMETER_LIST_TEST   CSMON_PAR_LIST_ALL_TYPES_REPEATED_ALL_TYPES_COUNT_TIMES
#define CSMON_PARAMETER_LIST_TEST   CSMON_PAR_LIST_RECORDER_DEBUG



#define PARAMETER_COUNT_MAX         256


/* Some Pinout */
#if _LAUNCHXL_F28379D
#define STAT_LED_G_PIN      28      /* N/A */
#define STAT_LED_A_B_PIN    31      /* D10 Blue */
#define STAT_LED_R_PIN      34      /* D9 */
#else
#define STAT_LED_G_PIN      16      /* CoreLite LEDs */
#define STAT_LED_A_B_PIN    17
#define STAT_LED_R_PIN      18
#endif

#define STAT_LED_ENABLE_LEVEL_LOW 0
#define STAT_LED_DISABLE_LVL_HIGH   (!STAT_LED_ENABLE_LEVEL_LOW)

//#define CLK_EN_FPGA_PIN     33

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */
#define INIT_MAX_MIN_DEF(_type, max, min, def_) \
        .u32Max._type = (max), .u32Min._type = (min), .u32Def._type = (def_)

#define INIT_PARAMFULL(u16ID, u16Attributes,eType,  u32Address, au8Name, au8Unit, u32Maximum, u32Minimum, u32Default, floatScale, u8BitCount, u8StartBit, eVisualAttribute) \
 { \
    u16ID,                          /* u16ID */\
    (u16Attributes),                /* u16Attributes */\
    (uint32_t)(u32Address),         /* u32Address */\
    {au8Name},                      /* au8Name */\
    {au8Unit},                      /* au8Unit */\
    .u32Max.eType = (u32Maximum),   /* u32Maximum */\
    .u32Min.eType = (u32Minimum),   /* u32Minimum */\
    .u32Def.eType = (u32Default),   /* u32Default */\
    floatScale,                     /* floatScale */\
    u8BitCount,                     /* u8BitCount */\
    u8StartBit,                     /* u8StartBit */\
    eVisualAttribute                /* eVisualAttribute */\
 }

#define INIT_PARAMETER(u16ID, u16Attributes,eType,  u32Address, au8Name, au8Unit, u32Maximum, u32Minimum, u32Default, floatScale) \
        INIT_PARAMFULL(u16ID, u16Attributes,eType,  u32Address, au8Name, au8Unit, u32Maximum, u32Minimum, u32Default, floatScale, 0, 0, CSMON_VISUAL_TYPE_HEX)


/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
#ifndef uWord32_t
typedef union
{
    uint32_t u32Register;
    uint16_t au16Word[2];
} uWord32_t;
#endif

typedef union
{
    uint32_t u32Register;
     int32_t s32Register;
    uint16_t u16Register;
     int16_t s16Register;
    uint_least8_t u8Register;
     int_least8_t s8Register;
} uAnyType32_t;



typedef struct
{
    uint_least8_t u8Seconds;
    uint_least8_t u8Minutes;
    uint_least8_t u8Hours;
    uint_least8_t u8Weekdays;
    uint_least8_t u8Day;
    uint_least8_t u8Month;
    uint_least8_t u8Year;
    uint_least8_t u8Reserved;
}MAIN_sDateTime_t;

typedef struct
{
    uint16_t u16ParameterIndexID;
    uint16_t u16ParamAttributes;
    uint32_t u32RealAddress;
    char au8Name[CSMON_SET_PARAMETER_NAME_LENGTH_MAX];
    char au8Unit[CSMON_SET_PARAMETER_UNIT_LENGTH_MAX];
    uAnyType32_t u32Max;
    uAnyType32_t u32Min;
    uAnyType32_t u32Def;
    float Norm;                 /* 0.0 - Default HEX Visualization; Any other -> Default Decimal Visualization */
    uint_least8_t u8BitCountOrArrayElementSize;
    uint_least8_t u8StartBitOrArrayElementCount;
    CSMON_eVisualType_t eVisualAttribute;

}MAIN_sParameterList_t;

/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */
#ifndef STRING_CONCAT
#define STRING_CONCAT_BASE(_a_, _b_) _a_##_b_
#define STRING_CONCAT(_a_, _b_) STRING_CONCAT_BASE(_a_, _b_)
#endif
#ifndef GPIO_PIN_MODE
#define GPIO_PIN_MODE(_pin_, _mode_) STRING_CONCAT(GPIO_, STRING_CONCAT(_pin_, STRING_CONCAT(_, _mode_)))
#define GPIO_PIN_MODE_GPIO(_pin_) STRING_CONCAT(GPIO_, STRING_CONCAT(_pin_, STRING_CONCAT(_GPIO, _pin_)))
#endif
#ifndef IO_DRV_H
#define GPIO_setPinConfigInput(_pin_) \
        GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(_pin_));\
        GPIO_setDirectionMode(_pin_, GPIO_DIR_MODE_IN);\
        GPIO_setPadConfig(_pin_, GPIO_PIN_TYPE_STD);\
        GPIO_setQualificationMode(_pin_, GPIO_QUAL_6SAMPLE);\
        GPIO_setMasterCore(_pin_, GPIO_CORE_CPU1)

#define GPIO_setPinConfigOutput(_pin_) \
        GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(_pin_));\
        GPIO_setDirectionMode(_pin_, GPIO_DIR_MODE_OUT);\
        GPIO_setPadConfig(_pin_, GPIO_PIN_TYPE_STD);\
        GPIO_setQualificationMode(_pin_, GPIO_QUAL_ASYNC);\
        GPIO_writePin(_pin_, 0);\
        GPIO_setMasterCore(_pin_, GPIO_CORE_CPU1)
#endif

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
MAIN_sDateTime_t MAIN_sDateTimeGet =
{
    0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00 /* 2001-01-01-Mon-00:00:00 */
};
MAIN_sDateTime_t MAIN_sDateTimeSet =
{
    BUILD_RTC_SEC,
    BUILD_RTC_MIN,
    BUILD_RTC_HOUR,
    0x00,
    BUILD_RTC_DAY,
    BUILD_RTC_MONTH,
    BUILD_RTC_YEAR,
    0x00,
    /* 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00 2001-01-01-Mon-00:00:00 */
};

    bool MAIN_bDateTimeSet = false;

    bool bDummyStatsDevRunning = false;
    bool bDummyReqstDevRunning = false;

    int16_t s16DummyCurrentPhaseAStartup = 0;
    int16_t s16DummyCurrentPhaseBStartup = 0;
    int16_t s16DummyCurrentPhaseCStartup = 0;
    int16_t s16DummyVoltageDCLinkStartup = 0;

    int16_t s16DummyCurrentPhaseAStop = -1000;
    int16_t s16DummyCurrentPhaseBStop = -2000;
    int16_t s16DummyCurrentPhaseCStop = -3000;
    int16_t s16DummyVoltageDCLinkStop = -10000;

    int16_t s16DummyCurrentPhaseAIdle = 100;
    int16_t s16DummyCurrentPhaseBIdle = 200;
    int16_t s16DummyCurrentPhaseCIdle = 300;
    int16_t s16DummyVoltageDCLinkIdle = 1000;

    int16_t s16DummyIncrementLoopCDiff = 5; //(1 << 8);

    int16_t s16DummyIncrementLoopA = 3; //(1 << 8);
    int16_t s16DummyIncrementLoopB = 10; //(1 << 8);
    int16_t s16DummyIncrementLoopC = 1; //(1 << 8);
    int16_t s16DummyIncrementLoopV = 1; //(1 << 8);

    int16_t s16DummyCurrentPhaseA = (0 << 14);
    int16_t s16DummyCurrentPhaseB = (1 << 14);
    int16_t s16DummyCurrentPhaseC = (2 << 14);
    int16_t s16DummyVoltageDCLink = (0 << 14);
    int16_t s16DummyIncrementLoop = 10; //(1 << 8);

 uint64_t u64DummyDataCnt = 0;
 uint32_t u32DummyDataCnt = 0;
 uint16_t u16DummyDataCnt = 0;
 uint16_t* pu16ModbusMessageCounter = &u16DummyDataCnt;

 int16_t s16DummyDataTst = 100;
 int16_t s16ScaleDataTst = 100;

 int16_t s16DummyData = 0x1234;
 int32_t aDummyDataTable = 0x1234;


 uint16_t u16TestData0 = 0;
 uint16_t u16TestData1 = 1;
 uint16_t u16TestData2 = 2;
 uint16_t u16TestData3 = 3;
 uint16_t u16TestData4 = 4;
 uint16_t u16TestData5 = 5;
 uint16_t u16TestData6 = 6;
 uint16_t u16TestData7 = 7;
 uint16_t u16TestData8 = 8;
 uint16_t u16TestData9 = 9;

 uint16_t u16WatchdogPrescaler = 0;
 uint16_t u16WatchdogPrescalerOld = 0;

 uint16_t u16WatchdogPrescalerTime = 0;
 uint16_t u16WatchdogPrescalerTimeOld = 0;

uint16_t u16DelayCtrlLoop_100nsec = 0;   /* Control loop Period 50usec - to be checked */
uint16_t u16DelayCtrlLoopOld_100nsec = 0;

uint16_t u16DelayMainLoop_usec = 0;
uint16_t u16DelayMainLoopOld_usec = 0;

uint16_t u16PeriodControl_usec = 50;            /* Default Initialization Value for ISR Period */
uint16_t u16PeriodControlOld_usec;

uint32_t u32DelayCtrlLoop_Ticks = 1;
uint32_t u32DelayMainLoop_Ticks = 1;

uint16_t u16CountSetParameterFail = 0;
uint16_t u16CountMaxParameterTest = PARAMETER_COUNT_MAX;/* Parameter 9 was independent RD and WR --> to be fixed depending on test configuration */

bool bResetAllTimeMeasures = 0;

uint32_t u32TimeMainLoopProcessCSMON_Bgn_Ticks;
uint32_t u32TimeMainLoopProcessCSMON_End_Ticks;
uint32_t u32TimeMainLoopProcessCSMON_Now_Ticks;
uint32_t u32TimeMainLoopProcessCSMON_Max_Ticks;

uint32_t u32TimeMainLoopCycle_Bgn_Ticks;
uint32_t u32TimeMainLoopCycle_End_Ticks;
uint32_t u32TimeMainLoopCycle_Now_Ticks;
uint32_t u32TimeMainLoopCycle_Max_Ticks;


uint32_t u32TimeCSMON_ISR_Ticks;
uint32_t u32TimeCSMON_ISR_Max_Ticks;
uint32_t u32TimeCtrlLoop_Ticks;
uint32_t u32TimeCtrlLoopMax_Ticks;


uint32_t u32ParamTime_Ticks;

char au8TestCharArray32[32] = "au8TestCharArray32";
char au8TestCharArray16[16] = "auChar16";
char au8TestCharArray8[8]   = "Tst08";



CSMON_eResponseCode_t eResponseCode_CSMON_eInit = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eProcess = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetServerOnStatus = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetParameter = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetRecorder = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetScope = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetTimerPeriodISRFunctionRegister = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetFlagProcessPassed = CSMON_RESPONSE_CODE_OK;
uint32_t u32GetBaudError_PPM = 0;




/* Parameter Table Version, DateTime, Checksum */
#define PARAM_TABLE_VERSION                 (uint32_t)0
#define PARAM_TABLE_DATETIME                (uint32_t)DATETIME_BUILD


#define EMIF_AUX_PARAM_TABLE_VER_ADDRESS    0x00120006      /* This address here I put only for the example. You should use your memory space address */
#define EMIF_AUX_BUILD_DATE_TIME_ADDRESS    0x00120008      /* This address here I put only for the example. You should use your memory space address */
#define EMIF_AUX_BACKUP_CHECKSUM_ADDRESS    0x0012000A      /* This address here I put only for the example. You should use your memory space address */


volatile uint16_t* EMIF_AUX_pu16ParamVerBackupInEmif = (uint16_t*)(EMIF_AUX_PARAM_TABLE_VER_ADDRESS);
volatile uint16_t* EMIF_AUX_pu16DateTimeBackupInEmif = (uint16_t*)(EMIF_AUX_BUILD_DATE_TIME_ADDRESS);
volatile uint16_t* EMIF_AUX_pu16CheckSumBackupInEmif = (uint16_t*)(EMIF_AUX_BACKUP_CHECKSUM_ADDRESS);





/* Dummy Parameter List */

#define PARAM_ID_MODBUS_MSG_CNT    65534

#define PARAM_ID_STARUNNINGMODE     0
#define PARAM_ID_VOLTAGE_DCLINK     8
#define PARAM_ID_CURRENT_PHASEA     9
#define PARAM_ID_CURRENT_PHASEB     10
#define PARAM_ID_CURRENT_PHASEC     11

volatile const MAIN_sParameterList_t asParameterList[PARAMETER_COUNT_MAX] =
{
/* u16ParameterIndexID;                 u32RealAddress;           u16ParamAttributes;     pu8Name;            pu8Unit;            u32Max;                 u32Min;              u32Def;             Norm; */
/*                                                                   Type Access Pass                                                                                                                    */

#if CSMON_PARAMETER_LIST_TEST == CSMON_PAR_LIST_RECORDER_DEBUG


 /*                ID         Attributes      uAnyType32_t         MCU Address                          Name               Unit         Max        Min         Def      Norm */
  INIT_PARAMETER(60000, PAR(_UINT16,_RW,_NO), u16Register, &u16PeriodControl_usec,                  "CtrlLoopPeriod",     "usec",    0x0000FFFF,    0,          0,      1.0F),
  INIT_PARAMETER(60001, PAR(_UINT16,_RW,_NO), u16Register, &u16DelayCtrlLoop_100nsec,               "CtrlLoopAddDelay",   "usec",    0x0000FFFF,    0,          0,      0.1F),
  INIT_PARAMETER(60002, PAR(_UINT16,_RW,_NO), u16Register, &u16DelayMainLoop_usec,                  "MainLoopAddDelay",   "usec",    0x0000FFFF,    0,          0,      1.0F),
  INIT_PARAMETER(60003, PAR(_UINT16,_RW,_NO), u16Register, &u16CountSetParameterFail,               "Parameters_Fail",    "unit",    0x0000FFFF,    0,          0,      1.0F),
  INIT_PARAMETER(60004, PAR(_UINT32,_RO,_NO), u32Register, &u32TimeMainLoopProcessCSMON_Max_Ticks,  "MainLoopCSMON",      "usec",    0xFFFFFFFF,    0,          0,      1.0F),
  INIT_PARAMETER(60005, PAR(_UINT32,_RO,_NO), u32Register, &u32TimeCSMON_ISR_Max_Ticks,             "CtrlLoopCSMON",      "usec",    0xFFFFFFFF,    0,          0,      1.0F),
  INIT_PARAMETER(60006, PAR(_UINT32,_RO,_NO), u32Register, &u32TimeMainLoopCycle_Max_Ticks,         "MainLoopMeasMax",    "usec",    0xFFFFFFFF,    0,          0,      1.0F),
  INIT_PARAMETER(60007, PAR(_UINT32,_RO,_NO), u32Register, &u32TimeCtrlLoopMax_Ticks,               "CtrlLoopMeasMax",    "usec",    0xFFFFFFFF,    0,          0,      1.0F),
  INIT_PARAMETER(60008, PAR(_UINT32,_RO,_NO), u32Register, &u32ParamTime_Ticks,                     "ParamInitMeas",      "usec",    0xFFFFFFFF,    0,          0,      1.0F),
  INIT_PARAMETER(60009, PAR(_UINT08,_RW,_NO), u8Register,  &bResetAllTimeMeasures,                  "MeasuresReset",      "unit",    true,          false,      false,  1.0F),

  INIT_PARAMETER(65534, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "ModbusMsgCntr",      "unit",    0x0000FFFF,    0,          0,      0.0F),
  INIT_PARAMETER(    0, PAR(_UINT08,_WO,_WR), u8Register,  &bDummyReqstDevRunning,                  "DeviceRunning",      "boolean", true,          false,      false,  1.0F),    /* Parameter ID 0 - Wr Addr */
  INIT_PARAMETER(    0, PAR(_UINT08,_RO,_NO), u8Register,  &bDummyStatsDevRunning,                  "DeviceRunning",      "boolean", true,          false,      false,  1.0F),    /* Parameter ID 0 - Rd Addr */
  INIT_PARAMETER(    1, PAR(_SINT16,_RW,_NO), s16Register, &s16ScaleDataTst,                        "ReadWriteScale",     "*3.1415", 10000,         -10000,     0,      3.1415F),
  INIT_PARAMETER(    2, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyIncrementLoop,                  "IncLoopTest",        "A(0.5V)", 10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    3, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseA,                  "CurrentPhA",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    4, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseB,                  "CurrentPhB",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    5, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseC,                  "CurrentPhC",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    6, PAR(_UINT32,_RW,_WR), u32Register, &u32GetBaudError_PPM,                    "BaudError",          "%",       10000,         0,          0,      0.0001F),
  INIT_PARAMETER(    7, PAR(_UINT16,_RW,_WR), u16Register, &u16WatchdogPrescalerTime,               "WatchdogTime",       "msec",    64,            1,          0,      ((1000.0*512*256)/10000000.0)),  /* 1msec; 512WatchdogDiv; 256WatchdogCnt; 10MHz INTOSC */

  INIT_PARAMETER(    8, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyVoltageDCLink,                  "VoltageBus",         "V",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    9, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseA,                  "CurrentPhA",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(   10, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseB,                  "CurrentPhB",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(   11, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseC,                  "CurrentPhC",         "A",       10000,         -10000,     0,      1.0F),

  INIT_PARAMETER(  108, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyVoltageDCLinkStartup,           "VoltageBusStart",    "V",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  109, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseAStartup,           "CurrentPhAStart",    "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  110, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseBStartup,           "CurrentPhBStart",    "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  111, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseCStartup,           "CurrentPhCStart",    "A",       10000,         -10000,     0,      1.0F),

  INIT_PARAMETER(  208, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyVoltageDCLinkStop,              "VoltageBusStop",     "V",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  209, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseAStop,              "CurrentPhAStop",     "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  210, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseBStop,              "CurrentPhBStop",     "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  211, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseCStop,              "CurrentPhCStop",     "A",       10000,         -10000,     0,      1.0F),

  INIT_PARAMETER(  308, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyVoltageDCLinkIdle,              "VoltageBusIdle",     "V",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  309, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseAIdle,              "CurrentPhAIdle",     "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  310, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseBIdle,              "CurrentPhBIdle",     "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  311, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseCIdle,              "CurrentPhCIdle",     "A",       10000,         -10000,     0,      1.0F),

  INIT_PARAMETER(  408, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyIncrementLoopA,                 "IncLoopTestA",       "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  409, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyIncrementLoopB,                 "IncLoopTestB",       "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  410, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyIncrementLoopC,                 "IncLoopTestC",       "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(  411, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyIncrementLoopV,                 "IncLoopTestV",       "V",       10000,         -10000,     0,      1.0F),

  INIT_PARAMETER(  412, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyIncrementLoopCDiff,             "IncLoopTestCDiff",   "A",       10000,         -10000,     0,      1.0F),



#elif CSMON_PARAMETER_LIST_TEST == CSMON_PAR_LIST_MINIMUM_COUNT

 /*                ID         Attributes      uAnyType32_t         MCU Address                          Name               Unit         Max        Min         Def      Norm */
 INIT_PARAMETER(65534, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "ModbusMsgCntr",      "unit",    0x0000FFFF,    0,          0,      0.0F),
 INIT_PARAMETER(    8, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyVoltageDCLink,                  "VoltageBus",         "V",       10000,         -10000,     0,      1.0F),
 INIT_PARAMETER(    9, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseA,                  "CurrentPhA",         "A",       10000,         -10000,     0,      1.0F),
 INIT_PARAMETER(   10, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseB,                  "CurrentPhB",         "A",       10000,         -10000,     0,      1.0F),
 INIT_PARAMETER(   11, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseC,                  "CurrentPhC",         "A",       10000,         -10000,     0,      1.0F),
 INIT_PARAMETER(    0, PAR(_UINT08,_WO,_WR), u8Register,  &bDummyReqstDevRunning,                  "DeviceRunning",      "boolean", true,          false,      false,  1.0F),    /* Parameter ID 0 - Wr Addr */
 INIT_PARAMETER(    0, PAR(_UINT08,_RO,_NO), u8Register,  &bDummyStatsDevRunning,                  "DeviceRunning",      "boolean", true,          false,      false,  1.0F),    /* Parameter ID 0 - Rd Addr */

 INIT_PARAMFULL(60093, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "93",                 "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 32, 0,CSMON_VISUAL_TYPE_DEF),
 //INIT_PARAMFULL(   93, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "93",                 "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 0, 32,CSMON_VISUAL_TYPE_DEF),
 //INIT_PARAMFULL(300,   PAR(_A_UINT08,_RW,_NO), u8Register, au8TestCharArray32,                     "au8TestCharArray32", "str",     0xFF,          0,          0,      0.0, 32,0,CSMON_VISUAL_TYPE_DEF),
 //INIT_PARAMFULL(301,   PAR(_A_UINT08,_RW,_NO), u8Register, au8TestCharArray16,                     "au8TestCharArray16", "str",     0xFF,          0,          0,      0.0, 16,0,CSMON_VISUAL_TYPE_DEF),
 //INIT_PARAMFULL(302,   PAR(_A_UINT08,_RW,_NO), u8Register, au8TestCharArray8,                      "au8TestCharArray8",  "str",     0xFF,          0,          0,      0.0,  8,0,CSMON_VISUAL_TYPE_DEF),

 INIT_PARAMETER(60001, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "u32DummyDataCnt 0123456789ACDEF0123456789ABCDEF",      "unituntunitunit",    0xFFFFFFFF,    0,          0,      0.0),
 INIT_PARAMFULL(60011, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "u1Start  0",         "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 1, 0,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60012, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "u1Start  1",         "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 1, 1,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60013, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "u1Start  8",         "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 1, 8,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60014, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "u1Start 18",         "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 1,18,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60041, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "u4Start22",          "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 4,22,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60042, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "u3Start2",           "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 3, 2,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60043, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "u8Start18",          "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 8,18,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60044, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "u2Start4",           "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 2, 4,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60071, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "u3Start0",           "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 3, 0,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60072, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "u5Start3",           "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 5, 3,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60073, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "u6Start1",           "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 6, 1,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60074, PAR(_UINT32,_RW,_NO), u32Register, &u32DummyDataCnt,                        "u9Start15",          "32BIN",   0xFFFFFFFF,    0,          0,      0.0, 9,15,CSMON_VISUAL_TYPE_BIN),

 INIT_PARAMETER(60000, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "u16DummyDataCnt 0123456789ACDEF0123456789ABCDEF",      "unituntunitunit",    0x0000FFFF,    0,          0,      0.0),
 INIT_PARAMFULL(60021, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "u1Start  0",         "16BIN",   0x0000FFFF,    0,          0,      0.0, 1, 0,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60022, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "u1Start  1",         "16BIN",   0x0000FFFF,    0,          0,      0.0, 1, 1,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60023, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "u1Start  8",         "16BIN",   0x0000FFFF,    0,          0,      0.0, 1, 8,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60024, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "u1Start 15",         "16BIN",   0x0000FFFF,    0,          0,      0.0, 1,15,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60031, PAR(_UINT08,_RW,_NO), u8Register,  &u16DummyDataCnt,                        "u1Start0",           "8BIN",    0x000000FF,    0,          0,      0.0, 1, 0,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60032, PAR(_UINT08,_RW,_NO), u8Register,  &u16DummyDataCnt,                        "u1Start2",           "8BIN",    0x000000FF,    0,          0,      0.0, 1, 2,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60033, PAR(_UINT08,_RW,_NO), u8Register,  &u16DummyDataCnt,                        "u1Start7",           "8BIN",    0x000000FF,    0,          0,      0.0, 1, 7,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60034, PAR(_UINT08,_RW,_NO), u8Register,  &u16DummyDataCnt,                        "u1Start4",           "8BIN",    0x000000FF,    0,          0,      0.0, 1, 4,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60051, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "u4Start12",          "16BIN",   0x0000FFFF,    0,          0,      0.0, 4,12,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60052, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "u3Start2",           "16BIN",   0x0000FFFF,    0,          0,      0.0, 3, 2,CSMON_VISUAL_TYPE_BIN),
 //INIT_PARAMFULL(60053, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "u8Start8",           "16BIN",   0x0000FFFF,    0,          0,      0.0, 8, 8,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60054, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "u2Start4",           "16BIN",   0x0000FFFF,    0,          0,      0.0, 2, 4,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60061, PAR(_UINT08,_RW,_NO), u8Register,  &u16DummyDataCnt,                        "u4Start0",           "8BIN",    0x000000FF,    0,          0,      0.0, 4, 0,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60062, PAR(_UINT08,_RW,_NO), u8Register,  &u16DummyDataCnt,                        "u3Start2",           "8BIN",    0x000000FF,    0,          0,      0.0, 3, 2,CSMON_VISUAL_TYPE_BIN),
 //INIT_PARAMFULL(60063, PAR(_UINT08,_RW,_NO), u8Register,  &u16DummyDataCnt,                        "u8Start0",           "8BIN",    0x000000FF,    0,          0,      0.0, 8, 0,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60064, PAR(_UINT08,_RW,_NO), u8Register,  &u16DummyDataCnt,                        "u2Start4",           "8BIN",    0x000000FF,    0,          0,      0.0, 2, 4,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60081, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "u3Start0",           "16BIN",   0x0000FFFF,    0,          0,      0.0, 3, 0,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60082, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "u5Start2",           "16BIN",   0x0000FFFF,    0,          0,      0.0, 5, 2,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60083, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "u7Start8",           "16BIN",   0x0000FFFF,    0,          0,      0.0, 7, 8,CSMON_VISUAL_TYPE_BIN),
 INIT_PARAMFULL(60084, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "u2Start14",          "16BIN",   0x0000FFFF,    0,          0,      0.0, 2,14,CSMON_VISUAL_TYPE_BIN),


 INIT_PARAMFULL(3000,  PAR(_A_UINT08,_RW,_NO), u8Register, au8TestCharArray32,                     "au8TestCharArray32", "str",     0xFF,          0,          0,      0.0, 0,32,CSMON_VISUAL_TYPE_DEF),
 INIT_PARAMFULL(3001,  PAR(_A_UINT08,_RW,_NO), u8Register, au8TestCharArray16,                     "au8TestCharArray16", "str",     0xFF,          0,          0,      0.0, 0,16,CSMON_VISUAL_TYPE_DEF),
 INIT_PARAMFULL(3002,  PAR(_A_UINT08,_RW,_NO), u8Register, au8TestCharArray8,                      "au8TestCharArray8",  "str",     0xFF,          0,          0,      0.0, 0, 8,CSMON_VISUAL_TYPE_DEF),



#elif CSMON_PARAMETER_LIST_TEST == CSMON_PAR_LIST_EACH_TYPE_REPEATED_ALL_TYPES_COUNT_TIMES

 /*                ID         Attributes      uAnyType32_t         MCU Address                          Name               Unit         Max        Min         Def      Norm */
  INIT_PARAMETER(60000, PAR(_UINT16,_RW,_NO), u16Register, &u16PeriodControl_usec,                  "CtrlLoopPeriod",     "usec",    0x0000FFFF,    0,          0,      1.0),
  INIT_PARAMETER(60001, PAR(_UINT16,_RW,_NO), u16Register, &u16DelayCtrlLoop_100nsec,               "CtrlLoopAddDelay",   "usec",    0x0000FFFF,    0,          0,      0.1),
  INIT_PARAMETER(60002, PAR(_UINT16,_RW,_NO), u16Register, &u16DelayMainLoop_usec,                  "MainLoopAddDelay",   "usec",    0x0000FFFF,    0,          0,      1.0),
  INIT_PARAMETER(60003, PAR(_UINT16,_RW,_NO), u16Register, &u16CountSetParameterFail,               "Parameters_Fail",    "unit",    0x0000FFFF,    0,          0,      1.0),
  INIT_PARAMETER(60004, PAR(_UINT32,_RO,_NO), u32Register, &u32TimeMainLoopProcessCSMON_Max_Ticks,  "MainLoopCSMON",      "usec",    0xFFFFFFFF,    0,          0,      1.0),
  INIT_PARAMETER(60005, PAR(_UINT32,_RO,_NO), u32Register, &u32TimeCSMON_ISR_Max_Ticks,             "CtrlLoopCSMON",      "usec",    0xFFFFFFFF,    0,          0,      1.0),
  INIT_PARAMETER(60006, PAR(_UINT32,_RO,_NO), u32Register, &u32TimeMainLoopCycle_Max_Ticks,         "MainLoopMeasMax",    "usec",    0xFFFFFFFF,    0,          0,      1.0),
  INIT_PARAMETER(60007, PAR(_UINT32,_RO,_NO), u32Register, &u32TimeCtrlLoopMax_Ticks,               "CtrlLoopMeasMax",    "usec",    0xFFFFFFFF,    0,          0,      1.0),
  INIT_PARAMETER(60008, PAR(_UINT32,_RO,_NO), u32Register, &u32ParamTime_Ticks,                     "ParamInitMeas",      "usec",    0xFFFFFFFF,    0,          0,      1.0),
  INIT_PARAMETER(60009, PAR(_UINT08,_RW,_NO), u8Register,  &bResetAllTimeMeasures,                  "MeasuresReset",      "unit",    true,          false,      false,  1.0),

  INIT_PARAMETER(65534, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "ModbusMsgCntr",      "unit",    0x0000FFFF,    0,          0,      0.0),
  INIT_PARAMETER(    0, PAR(_UINT08,_WO,_WR), u8Register,  &bDummyReqstDevRunning,                  "DeviceRunning",      "boolean", true,          false,      false,  1.0),    /* Parameter ID 0 - Wr Addr */
  INIT_PARAMETER(    0, PAR(_UINT08,_RO,_NO), u8Register,  &bDummyStatsDevRunning,                  "DeviceRunning",      "boolean", true,          false,      false,  1.0),    /* Parameter ID 0 - Rd Addr */
  INIT_PARAMETER(    1, PAR(_SINT16,_RW,_NO), s16Register, &s16ScaleDataTst,                        "ReadWriteScale",     "*3.1415", 10000,         -10000,     0,      3.1415),
  INIT_PARAMETER(    2, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyIncrementLoop,                  "IncLoopTest",        "A(0.5V)", 10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    3, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseA,                  "CurrentPhA",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    4, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseB,                  "CurrentPhB",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    5, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseC,                  "CurrentPhC",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    6, PAR(_UINT32,_RW,_WR), u32Register, &u32GetBaudError_PPM,                    "BaudError",          "%",       10000,         0,          0,      0.0001),
  INIT_PARAMETER(    7, PAR(_UINT16,_RW,_WR), u16Register, &u16WatchdogPrescalerTime,               "WatchdogTime",       "msec",    64,             1,          0,      ((1000.0*512*256)/10000000.0)),  /* 1msec; 512WatchdogDiv; 256WatchdogCnt; 10MHz INTOSC */

  INIT_PARAMETER(    8, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyVoltageDCLink,                  "VoltageBus",         "V",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    9, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseA,                  "CurrentPhA",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(   10, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseB,                  "CurrentPhB",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(   11, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseC,                  "CurrentPhC",         "A",       10000,         -10000,     0,      1.0F),

  INIT_PARAMETER(10000, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "0.0",     10000,         -10000,     0,      0.0),
  INIT_PARAMETER(10001, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "0.0",     10000,         -10000,     0,      0.0),
  INIT_PARAMETER(10002, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "0.0",     10000,         -10000,     0,      0.0),
  INIT_PARAMETER(10003, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "0.0",     10000,         -10000,     0,      0.0),
  INIT_PARAMETER(10004, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "0.0",     10000,         -10000,     0,      0.0),
  INIT_PARAMETER(10005, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "0.0",     10000,         -10000,     0,      0.0),

  INIT_PARAMETER(10100, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "1.0",     10000,         -10000,     0,      1.0),
  INIT_PARAMETER(10101, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "1.0",     10000,         -10000,     0,      1.0),
  INIT_PARAMETER(10102, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "1.0",     10000,         -10000,     0,      1.0),
  INIT_PARAMETER(10103, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "1.0",     10000,         -10000,     0,      1.0),
  INIT_PARAMETER(10104, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "1.0",     10000,         -10000,     0,      1.0),
  INIT_PARAMETER(10105, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "1.0",     10000,         -10000,     0,      1.0),

  INIT_PARAMETER(10200, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "0.0",     1000000,       -1000000,   0,      0.0),
  INIT_PARAMETER(10201, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "0.0",     1000000,       -1000000,   0,      0.0),
  INIT_PARAMETER(10202, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "0.0",     1000000,       -1000000,   0,      0.0),
  INIT_PARAMETER(10203, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "0.0",     1000000,       -1000000,   0,      0.0),
  INIT_PARAMETER(10204, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "0.0",     1000000,       -1000000,   0,      0.0),
  INIT_PARAMETER(10205, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "0.0",     1000000,       -1000000,   0,      0.0),

  INIT_PARAMETER(10300, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "1.0",     1000000,       -1000000,   0,      1.0),
  INIT_PARAMETER(10301, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "1.0",     1000000,       -1000000,   0,      1.0),
  INIT_PARAMETER(10302, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "1.0",     1000000,       -1000000,   0,      1.0),
  INIT_PARAMETER(10303, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "1.0",     1000000,       -1000000,   0,      1.0),
  INIT_PARAMETER(10304, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "1.0",     1000000,       -1000000,   0,      1.0),
  INIT_PARAMETER(10305, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "1.0",     1000000,       -1000000,   0,      1.0),

  INIT_PARAMETER(10400, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "0.0",     255,           0,          0,      0.0),
  INIT_PARAMETER(10401, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "0.0",     255,           0,          0,      0.0),
  INIT_PARAMETER(10402, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "0.0",     255,           0,          0,      0.0),
  INIT_PARAMETER(10403, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "0.0",     255,           0,          0,      0.0),
  INIT_PARAMETER(10404, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "0.0",     255,           0,          0,      0.0),
  INIT_PARAMETER(10405, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "0.0",     255,           0,          0,      0.0),

  INIT_PARAMETER(10500, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "1.0",     255,           0,          0,      1.0),
  INIT_PARAMETER(10501, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "1.0",     255,           0,          0,      1.0),
  INIT_PARAMETER(10502, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "1.0",     255,           0,          0,      1.0),
  INIT_PARAMETER(10503, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "1.0",     255,           0,          0,      1.0),
  INIT_PARAMETER(10504, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "1.0",     255,           0,          0,      1.0),
  INIT_PARAMETER(10505, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "1.0",     255,           0,          0,      1.0),


#elif CSMON_PARAMETER_LIST_TEST == CSMON_PAR_LIST_ALL_TYPES_REPEATED_ALL_TYPES_COUNT_TIMES


 /*                ID         Attributes      uAnyType32_t         MCU Address                          Name               Unit         Max        Min         Def      Norm */
  INIT_PARAMETER(60000, PAR(_UINT16,_RW,_NO), u16Register, &u16PeriodControl_usec,                  "CtrlLoopPeriod",     "usec",    0x0000FFFF,    0,          0,      1.0),
  INIT_PARAMETER(60001, PAR(_UINT16,_RW,_NO), u16Register, &u16DelayCtrlLoop_100nsec,               "CtrlLoopAddDelay",   "usec",    0x0000FFFF,    0,          0,      0.1),
  INIT_PARAMETER(60002, PAR(_UINT16,_RW,_NO), u16Register, &u16DelayMainLoop_usec,                  "MainLoopAddDelay",   "usec",    0x0000FFFF,    0,          0,      1.0),
  INIT_PARAMETER(60003, PAR(_UINT16,_RW,_NO), u16Register, &u16CountSetParameterFail,               "Parameters_Fail",    "unit",    0x0000FFFF,    0,          0,      1.0),
  INIT_PARAMETER(60004, PAR(_UINT32,_RO,_NO), u32Register, &u32TimeMainLoopProcessCSMON_Max_Ticks,  "MainLoopCSMON",      "usec",    0xFFFFFFFF,    0,          0,      1.0),
  INIT_PARAMETER(60005, PAR(_UINT32,_RO,_NO), u32Register, &u32TimeCSMON_ISR_Max_Ticks,             "CtrlLoopCSMON",      "usec",    0xFFFFFFFF,    0,          0,      1.0),
  INIT_PARAMETER(60006, PAR(_UINT32,_RO,_NO), u32Register, &u32TimeMainLoopCycle_Max_Ticks,         "MainLoopMeasMax",    "usec",    0xFFFFFFFF,    0,          0,      1.0),
  INIT_PARAMETER(60007, PAR(_UINT32,_RO,_NO), u32Register, &u32TimeCtrlLoopMax_Ticks,               "CtrlLoopMeasMax",    "usec",    0xFFFFFFFF,    0,          0,      1.0),
  INIT_PARAMETER(60008, PAR(_UINT32,_RO,_NO), u32Register, &u32ParamTime_Ticks,                     "ParamInitMeas",      "usec",    0xFFFFFFFF,    0,          0,      1.0),
  INIT_PARAMETER(60009, PAR(_UINT08,_RW,_NO), u8Register,  &bResetAllTimeMeasures,                  "MeasuresReset",      "unit",    true,          false,      false,  1.0),

  INIT_PARAMETER(65534, PAR(_UINT16,_RW,_NO), u16Register, &u16DummyDataCnt,                        "ModbusMsgCntr",      "unit",    0x0000FFFF,    0,          0,      0.0),
  INIT_PARAMETER(    0, PAR(_UINT08,_WO,_WR), u8Register,  &bDummyReqstDevRunning,                  "DeviceRunning",      "boolean", true,          false,      false,  1.0),    /* Parameter ID 0 - Wr Addr */
  INIT_PARAMETER(    0, PAR(_UINT08,_RO,_NO), u8Register,  &bDummyStatsDevRunning,                  "DeviceRunning",      "boolean", true,          false,      false,  1.0),    /* Parameter ID 0 - Rd Addr */
  INIT_PARAMETER(    1, PAR(_SINT16,_RW,_NO), s16Register, &s16ScaleDataTst,                        "ReadWriteScale",     "*3.1415", 10000,         -10000,     0,      3.1415),
  INIT_PARAMETER(    2, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyIncrementLoop,                  "IncLoopTest",        "A(0.5V)", 10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    3, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseA,                  "CurrentPhA",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    4, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseB,                  "CurrentPhB",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    5, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseC,                  "CurrentPhC",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    6, PAR(_UINT32,_RW,_WR), u32Register, &u32GetBaudError_PPM,                    "BaudError",          "%",       10000,         0,          0,      0.0001),
  INIT_PARAMETER(    7, PAR(_UINT16,_RW,_WR), u16Register, &u16WatchdogPrescalerTime,               "WatchdogTime",       "msec",    64,             1,          0,      ((1000.0*512*256)/10000000.0)),  /* 1msec; 512WatchdogDiv; 256WatchdogCnt; 10MHz INTOSC */

  INIT_PARAMETER(    8, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyVoltageDCLink,                  "VoltageBus",         "V",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(    9, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseA,                  "CurrentPhA",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(   10, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseB,                  "CurrentPhB",         "A",       10000,         -10000,     0,      1.0F),
  INIT_PARAMETER(   11, PAR(_SINT16,_RW,_WR), s16Register, &s16DummyCurrentPhaseC,                  "CurrentPhC",         "A",       10000,         -10000,     0,      1.0F),

  INIT_PARAMETER(10000, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "0.0",     10000,         -10000,     0,      0.0),
  INIT_PARAMETER(10100, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "0.0",     10000,         -10000,     0,      0.0),
  INIT_PARAMETER(10200, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "0.0",     10000,         -10000,     0,      0.0),
  INIT_PARAMETER(10300, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "0.0",     10000,         -10000,     0,      0.0),
  INIT_PARAMETER(10400, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "0.0",     10000,         -10000,     0,      0.0),
  INIT_PARAMETER(10500, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "0.0",     10000,         -10000,     0,      0.0),

  INIT_PARAMETER(10001, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "1.0",     10000,         -10000,     0,      1.0),
  INIT_PARAMETER(10101, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "1.0",     10000,         -10000,     0,      1.0),
  INIT_PARAMETER(10201, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "1.0",     10000,         -10000,     0,      1.0),
  INIT_PARAMETER(10301, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "1.0",     10000,         -10000,     0,      1.0),
  INIT_PARAMETER(10401, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "1.0",     10000,         -10000,     0,      1.0),
  INIT_PARAMETER(10501, PAR(_SINT16,_RW,_NO), s16Register, &aDummyDataTable,                        "Param_SINT16",       "1.0",     10000,         -10000,     0,      1.0),

  INIT_PARAMETER(10002, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "0.0",     1000000,       -1000000,   0,      0.0),
  INIT_PARAMETER(10102, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "0.0",     1000000,       -1000000,   0,      0.0),
  INIT_PARAMETER(10202, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "0.0",     1000000,       -1000000,   0,      0.0),
  INIT_PARAMETER(10302, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "0.0",     1000000,       -1000000,   0,      0.0),
  INIT_PARAMETER(10402, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "0.0",     1000000,       -1000000,   0,      0.0),
  INIT_PARAMETER(10502, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "0.0",     1000000,       -1000000,   0,      0.0),

  INIT_PARAMETER(10003, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "1.0",     1000000,       -1000000,   0,      1.0),
  INIT_PARAMETER(10103, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "1.0",     1000000,       -1000000,   0,      1.0),
  INIT_PARAMETER(10203, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "1.0",     1000000,       -1000000,   0,      1.0),
  INIT_PARAMETER(10303, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "1.0",     1000000,       -1000000,   0,      1.0),
  INIT_PARAMETER(10403, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "1.0",     1000000,       -1000000,   0,      1.0),
  INIT_PARAMETER(10503, PAR(_SINT32,_RW,_NO), s32Register, &aDummyDataTable,                        "Param_SINT32",       "1.0",     1000000,       -1000000,   0,      1.0),

  INIT_PARAMETER(10004, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "0.0",     255,           0,          0,      0.0),
  INIT_PARAMETER(10104, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "0.0",     255,           0,          0,      0.0),
  INIT_PARAMETER(10204, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "0.0",     255,           0,          0,      0.0),
  INIT_PARAMETER(10304, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "0.0",     255,           0,          0,      0.0),
  INIT_PARAMETER(10404, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "0.0",     255,           0,          0,      0.0),
  INIT_PARAMETER(10504, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "0.0",     255,           0,          0,      0.0),

  INIT_PARAMETER(10005, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "1.0",     255,           0,          0,      1.0),
  INIT_PARAMETER(10105, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "1.0",     255,           0,          0,      1.0),
  INIT_PARAMETER(10205, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "1.0",     255,           0,          0,      1.0),
  INIT_PARAMETER(10305, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "1.0",     255,           0,          0,      1.0),
  INIT_PARAMETER(10405, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "1.0",     255,           0,          0,      1.0),
  INIT_PARAMETER(10505, PAR(_UINT08,_RW,_NO), u8Register,  &aDummyDataTable,                        "Param_UINT08",       "1.0",     255,           0,          0,      1.0),


#else

#endif


};



/* *****************************************************************************
 * Prototypes of functions definitions
 **************************************************************************** */
void ControlProcess(void);
void ParameterInitialization(void);


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
    MAIN_sDateTimeSet.u8Seconds    = u8BCDSeconds;
    MAIN_sDateTimeSet.u8Minutes    = u8BCDMinutes;
    MAIN_sDateTimeSet.u8Hours      = u8BCDHours;
    MAIN_sDateTimeSet.u8Weekdays   = u8BCDWeekdays;
    MAIN_sDateTimeSet.u8Day        = u8BCDDay;
    MAIN_sDateTimeSet.u8Month      = u8BCDMonth;
    MAIN_sDateTimeSet.u8Year       = u8BCDYear;
    MAIN_bDateTimeSet = true;
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
        *pu8BCDSeconds  = MAIN_sDateTimeGet.u8Seconds;
        *pu8BCDMinutes  = MAIN_sDateTimeGet.u8Minutes;
        *pu8BCDHours    = MAIN_sDateTimeGet.u8Hours;
        *pu8BCDWeekdays = MAIN_sDateTimeGet.u8Weekdays;
        *pu8BCDDay      = MAIN_sDateTimeGet.u8Day;
        *pu8BCDMonth    = MAIN_sDateTimeGet.u8Month;
        *pu8BCDYear     = MAIN_sDateTimeGet.u8Year;
}



/* *****************************************************************************
 * ControlProcess
 **************************************************************************** */
void ControlProcess(void)
{
    u32TimeCtrlLoop_Ticks = CPUTimer_getTimerCount(CPUTIMER1_BASE);
    GPIO_writePin(STAT_LED_R_PIN, STAT_LED_ENABLE_LEVEL_LOW);     /* Red LED (closest to the Debug Header) */

    //
    // Test For Data Consistency and Control Emulation
    //
    if (bDummyReqstDevRunning)
    {
        if (bDummyStatsDevRunning == false)
        {
            bDummyStatsDevRunning = true;

            //
            // CSMON Internal Recorders Start with Already Setup Configuration
            //
            CSMON_vSetStartRecorderParameterMask(CSMON_MASK_RECORDERS_012);

            s16DummyCurrentPhaseA = s16DummyCurrentPhaseAStartup;
            s16DummyCurrentPhaseB = s16DummyCurrentPhaseBStartup;
            s16DummyCurrentPhaseC = s16DummyCurrentPhaseCStartup;
            s16DummyVoltageDCLink = s16DummyVoltageDCLinkStartup;
        }
        else
        {
            s16DummyCurrentPhaseA += s16DummyIncrementLoopA;
            s16DummyCurrentPhaseB += s16DummyIncrementLoopB;
            s16DummyCurrentPhaseC += s16DummyIncrementLoopC;
            s16DummyVoltageDCLink += s16DummyIncrementLoopV;

            s16DummyCurrentPhaseC += s16DummyIncrementLoopCDiff;
            s16DummyIncrementLoopCDiff = 0 - s16DummyIncrementLoopCDiff;

        }

    }
    else
    {
        if (bDummyStatsDevRunning == true)
        {
            bDummyStatsDevRunning = false;

            //
            // CSMON Internal Recorders Stop (Trigger)
            //
            CSMON_vSetStopRecorderParameterMask(CSMON_MASK_RECORDERS_012);

            s16DummyCurrentPhaseAIdle = s16DummyCurrentPhaseA;
            s16DummyCurrentPhaseBIdle = s16DummyCurrentPhaseB;
            s16DummyCurrentPhaseCIdle = s16DummyCurrentPhaseC;
            s16DummyVoltageDCLinkIdle = s16DummyVoltageDCLink;

            s16DummyCurrentPhaseA = s16DummyCurrentPhaseAStop;
            s16DummyCurrentPhaseB = s16DummyCurrentPhaseBStop;
            s16DummyCurrentPhaseC = s16DummyCurrentPhaseCStop;
            s16DummyVoltageDCLink = s16DummyVoltageDCLinkStop;
        }
        else
        {
            s16DummyCurrentPhaseA = s16DummyCurrentPhaseAIdle;
            s16DummyCurrentPhaseB = s16DummyCurrentPhaseBIdle;
            s16DummyCurrentPhaseC = s16DummyCurrentPhaseCIdle;
            s16DummyVoltageDCLink = s16DummyVoltageDCLinkIdle;

        }

    }

    u32TimeCSMON_ISR_Ticks = CPUTimer_getTimerCount(CPUTIMER1_BASE);
    //
    // Process Passed Flag Set - Need to be called from Processes with higher priority level in order CSMON to be able to get meaning-full (consistent) data
    eResponseCode_CSMON_eSetFlagProcessPassed = CSMON_eSetFlagProcessPassed (CSMON_ID_PROCESS_CONTROL_PRIMARY);
    ASSERT(eResponseCode_CSMON_eSetFlagProcessPassed != CSMON_RESPONSE_CODE_OK);
    // Check CSMON Response Code (... or Embed Assert For Debug) if needed
    u32TimeCSMON_ISR_Ticks = 0 - (CPUTimer_getTimerCount(CPUTIMER1_BASE) - u32TimeCSMON_ISR_Ticks);//down count
    if (u32TimeCSMON_ISR_Ticks > u32TimeCSMON_ISR_Max_Ticks)
    {
        u32TimeCSMON_ISR_Max_Ticks = u32TimeCSMON_ISR_Ticks;
    }

}


/* *****************************************************************************
 * ParameterInitialization
 **************************************************************************** */
void ParameterInitialization(void)
{
    uint16_t u16Index;

    u16CountSetParameterFail = 0;

    /* Invalid Table - Reset Parameter Table */
    CSMON_eResetParameterTable();                                   /* Reset Internal Used Parameters Count */

    /* Add Parameters */
    for (u16Index = 0; u16Index < PARAMETER_COUNT_MAX; u16Index++)
    {

        eResponseCode_CSMON_eSetParameter =
            CSMON_eSetParameter (
                asParameterList[u16Index].u16ParameterIndexID,
                asParameterList[u16Index].u32RealAddress,
                asParameterList[u16Index].u16ParamAttributes,
(uint_least8_t*)&asParameterList[u16Index].au8Name,
(uint_least8_t*)&asParameterList[u16Index].au8Unit,
                asParameterList[u16Index].u32Max.u32Register,
                asParameterList[u16Index].u32Min.u32Register,
                asParameterList[u16Index].u32Def.u32Register,
                asParameterList[u16Index].Norm,
                asParameterList[u16Index].u8BitCountOrArrayElementSize,
                asParameterList[u16Index].u8StartBitOrArrayElementCount
                );
        if(eResponseCode_CSMON_eSetParameter != CSMON_RESPONSE_CODE_OK)
        {
            u16CountSetParameterFail++;
        }
    }

    CSMON_eApplyParameterChanges();                         /* Internal Library Apply Written Parameters */
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
    // Disable pin locks and enable internal PullUps.
    //
    Device_initGPIO();


    //
    // EMIF
    //
    //EMIF_DRV_vInit(); //DPN required only if CSMON compiled with _CSMON_IN_INTERNAL_RAM=0

    //
    // Clear all interrupts and initialize PIE vector table:
    // Disable CPU interrupts
    // Initialize interrupt controller and vector table.
    //
    Interrupt_initModule();
    Interrupt_initVectorTable();


    #ifdef _CSMON_FREE_RUN_TIMER_EXTERNAL           // defined in Project Configuration Build Settings
    //
    // CPUTimer1 Initialization as Free-Run Mode with Maximum Period
    #define CPUTIMER1_PRESCALLER_VALUE      ((DEVICE_SYSCLK_FREQ /   1000000)-1)    // Timer Tick 1.0 microseconds
    //
    CPUTimer_setPeriod(CPUTIMER1_BASE, 0xFFFFFFFF);
    CPUTimer_setPreScaler(CPUTIMER1_BASE, CPUTIMER1_PRESCALLER_VALUE);
    CPUTimer_stopTimer(CPUTIMER1_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER1_BASE);
    CPUTimer_enableInterrupt(CPUTIMER1_BASE);
    CPUTimer_startTimer(CPUTIMER1_BASE);
    #endif

    GPIO_writePin(STAT_LED_G_PIN, STAT_LED_ENABLE_LEVEL_LOW); /* Green LED (closest to the MCU Led) */
    //
    // CSMON Initialization -> ~ 2.25ms
    //
    eResponseCode_CSMON_eInit = CSMON_eInit();
    // Check CSMON Response Code if needed
    if (eResponseCode_CSMON_eInit != CSMON_RESPONSE_CODE_OK)
    {
        /* If enters here - Fix Peripheral Frequency for Better Performance and Stability (DEVICE_LSPCLK_FREQ) */
        u32GetBaudError_PPM = CSMON_u32GetBaudError_PPM(CSMON_ID_PERIPHERAL_SCI_MODBUS);
        ASSERT(u32GetBaudError_PPM >= CSMON_u32PercentToPPM(3.0));
    }
    GPIO_writePin(STAT_LED_G_PIN, STAT_LED_DISABLE_LVL_HIGH); /* Green LED (closest to the MCU Led) */

    //
    // Firmware Application Version Set to CSMON
    //
    (void)CSMON_eSetFirmwareApplicationVersion(TEST_CSMON_APPLICATION_VERSION);

    //
    // CSMON Parameters Initialization
    //
    ParameterInitialization();

    //
    // Register Function Call In CSMON Timer Period ISR (default Timer Period is 50 usec)
    // For Debug and Control Process Emulation here is registered the ControlProcess Function
    //
    eResponseCode_CSMON_eSetTimerPeriodISRFunctionRegister = CSMON_eSetTimerPeriodISRFunctionRegister(ControlProcess);
    ASSERT(eResponseCode_CSMON_eSetTimerPeriodISRFunctionRegister != CSMON_RESPONSE_CODE_OK);
    // Check CSMON Response Code (... or Embed Assert For Debug) if needed

    //
    //
    // Enable Global Interrupt (INTM) and RealTime interrupt (DBGM)
    //
    EINT;
    ERTM;

    for (;;)
    {

        if (u16PeriodControlOld_usec != u16PeriodControl_usec)
        {
            u16PeriodControlOld_usec = u16PeriodControl_usec;
            CSMON_eSetInternalTimerPeriod(u16PeriodControl_usec);
        }

        //
        // CSMON Process In Main Loop - Big Delays On Disconnect 4-5ms; On Connect 12-35ms If Not Interrupted (EMIF Checksum PC Application)
        //
        eResponseCode_CSMON_eProcess = CSMON_eProcess();
        // Check CSMON Response Code if needed
        ASSERT(eResponseCode_CSMON_eProcess != CSMON_RESPONSE_CODE_OK);

        //
        // Device Running Control Indication - Set on Enter/Exit Run Mode
        //
        eResponseCode_CSMON_eSetServerOnStatus = CSMON_eSetServerOnStatus(bDummyStatsDevRunning);
        // Check CSMON Response Code if needed
        ASSERT(eResponseCode_CSMON_eSetServerOnStatus != CSMON_RESPONSE_CODE_OK);

    }

}



