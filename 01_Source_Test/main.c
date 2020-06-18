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

#include "emif_driver.h"
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

#define CSMON_PAR_LIST_EACH_TYPE_REPEATED_ALL_TYPES_COUNT_TIMES         0                   /* Each Type Repeated All Types Count Times  */
#define CSMON_PAR_LIST_ALL_TYPES_REPEATED_ALL_TYPES_COUNT_TIMES         1                   /* All Types Repeated All Types Count Times  */
#define CSMON_PAR_LIST_MAXIMUM_COUNT                                    2                   /* Maximum Count */


#define CSMON_PARAMETER_LIST_TEST   CSMON_PAR_LIST_EACH_TYPE_REPEATED_ALL_TYPES_COUNT_TIMES
//#define CSMON_PARAMETER_LIST_TEST   CSMON_PAR_LIST_ALL_TYPES_REPEATED_ALL_TYPES_COUNT_TIMES
//#define CSMON_PARAMETER_LIST_TEST   CSMON_PAR_LIST_MAXIMUM_COUNT



#define PARAMETER_COUNT_MAX        (1024 + 1)/* Parameter 9 was independent RD and WR --> to be fixed depending on test configuration */
#define PARAMETER_ID_START_1000     0       /* Small and Big Numbers for ID (0 or 1) */


/* Some Pinout */
#ifdef _LAUNCHXL_F28379D
#define STAT_LED_G_PIN      28      /* N/A */
#define STAT_LED_A_B_PIN    31      /* D10 Blue */
#define STAT_LED_R_PIN      34      /* D9 */
#else
#define STAT_LED_G_PIN      28      /* Green LED (closest to the MCU Led) */
#define STAT_LED_A_B_PIN    30      /* Amber LED (middle Led) */
#define STAT_LED_R_PIN      32      /* Red LED (closest to the Debug Header) */
#endif

#define STAT_LED_ENABLE_LEVEL_LOW 0
#define STAT_LED_DISABLE_LVL_HIGH   (!STAT_LED_ENABLE_LEVEL_LOW)

#define CLK_EN_FPGA_PIN     33

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

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
    uint32_t u32RealAddress;
    uint16_t u16ParamAttributes;
    char au8Name[16];
    char au8Unit[8];
    uAnyType32_t u32Max;
    uAnyType32_t u32Min;
    uAnyType32_t u32Def;
    float Norm;

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

 int16_t s16DummyCurrentPhaseA = (0 << 14);
 int16_t s16DummyCurrentPhaseB = (1 << 14);
 int16_t s16DummyCurrentPhaseC = (2 << 14);
 int16_t s16DummyVoltageDCLink = (0 << 14);
 int16_t s16DummyIncrementLoop = (1 << 8);

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

uint16_t u16DelayCtrlLoop_100nsec = 0;   /* Control loop Period 50usec - to be checked */
uint16_t u16DelayCtrlLoopOld_100nsec = 0;

uint16_t u16DelayMainLoop_usec = 0;
uint16_t u16DelayMainLoopOld_usec = 0;

uint16_t u16PeriodControl_usec = 50;
uint16_t u16PeriodControlOld_usec;

uint32_t u32DelayCtrlLoop_Ticks = 1;
uint32_t u32DelayMainLoop_Ticks = 1;

uint16_t u16CountSetParameterFail = 0;
uint16_t u16CountMaxParameterTest = PARAMETER_COUNT_MAX - 1;/* Parameter 9 was independent RD and WR --> to be fixed depending on test configuration */

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
#define PARAM_ID_READ_WRITE_TST        0
#define PARAM_ID_RD_WR_SCALETST        1

#define PARAM_ID_PARAMETER_TST0    10000
#define PARAM_ID_PARAMETER_TST1    10001
#define PARAM_ID_PARAMETER_TST2    10002
#define PARAM_ID_PARAMETER_TST3    10003
#define PARAM_ID_PARAMETER_TST4    10004
#define PARAM_ID_PARAMETER_TST5    10005
#define PARAM_ID_PARAMETER_TST6    10006
#define PARAM_ID_PARAMETER_TST7    10007
#define PARAM_ID_PARAMETER_TST8    10008
#define PARAM_ID_PARAMETER_TST9    10009

#if PARAMETER_ID_START_1000 == 0
#define PARAM_ID_CURRENT_PHASEA       20
#define PARAM_ID_CURRENT_PHASEB       21
#define PARAM_ID_CURRENT_PHASEC       22
#define PARAM_ID_VOLTAGE_DCLINK       23
#define PARAM_ID_INCREMENT_LOOP        8
#define PARAM_ID_REQRUNNINGMODE        9    /* ID 9 Write Request From PC */
#define PARAM_ID_STARUNNINGMODE        9    /* ID 9 Read  Request From PC (can be with Different Real Address) */
#else
#define PARAM_ID_CURRENT_PHASEA     1000
#define PARAM_ID_CURRENT_PHASEB     1001
#define PARAM_ID_CURRENT_PHASEC     1002
#define PARAM_ID_VOLTAGE_DCLINK     1003
#define PARAM_ID_INCREMENT_LOOP     1004
#define PARAM_ID_REQRUNNINGMODE     1005
#define PARAM_ID_STARUNNINGMODE     1006
#endif

#define PARAM_ID_PARAMETER_TST0    10000
#define PARAM_ID_PARAMETER_TST1    10001
#define PARAM_ID_PARAMETER_TST2    10002
#define PARAM_ID_PARAMETER_TST3    10003
#define PARAM_ID_PARAMETER_TST4    10004
#define PARAM_ID_PARAMETER_TST5    10005
#define PARAM_ID_PARAMETER_TST6    10006
#define PARAM_ID_PARAMETER_TST7    10007
#define PARAM_ID_PARAMETER_TST8    10008
#define PARAM_ID_PARAMETER_TST9    10009


#define PARAM_ID_CTRL_LOOP_NSEC    30000
#define PARAM_ID_CTRL_LOOP_USEC    30001
#define PARAM_ID_MAIN_LOOP_USEC    30002
#define PARAM_ID_PARAM_SET_FAIL    30003
#define PARAM_ID_PARAM_MAX_TEST    30004
#define PARAM_ID_CTRL_PERIOD_US    30005

volatile const MAIN_sParameterList_t asParameterList[PARAMETER_COUNT_MAX] =
{
/* u16ParameterIndexID;                 u32RealAddress;           u16ParamAttributes;     pu8Name;            pu8Unit;            u32Max;                 u32Min;              u32Def;             Norm; */
/*                                                                   Type Access Pass                                                                                                                    */



#if CSMON_PARAMETER_LIST_TEST == CSMON_PAR_LIST_MAXIMUM_COUNT


#if 0

 {1000                   , (uint32_t)&s16DummyCurrentPhaseA   ,  PAR(_UINT16,_RW,_NO)  , {"CurrentPhA"}          ,    {""}      ,    (uint32_t)(65535)  ,   (uint32_t)(     0)  ,    (uint32_t)(999)    ,   1.0 },
// {1001                   , (uint32_t)&s16DummyCurrentPhaseB   ,  PAR(_UINT16,_RW,_NO)  , {"CurrentPhB"}          ,    {""}      ,    (uint32_t)(65535)  ,   (uint32_t)(     0)  ,    (uint32_t)(999)    ,   1.0 },
// {1002                   , (uint32_t)&s16DummyCurrentPhaseC   ,  PAR(_UINT16,_RW,_NO)  , {"CurrentPhC"}          ,    {""}      ,    (uint32_t)(65535)  ,   (uint32_t)(     0)  ,    (uint32_t)(999)    ,   1.0 },


#else



 {10000                  , (uint32_t)&u16TestData0            ,  PAR(_UINT16,_WO,_RD)  , {"Test_10000"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.00 },
 {10001                  , (uint32_t)&u16TestData1            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10001"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.01 },
 {10002                  , (uint32_t)&u16TestData2            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10002"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.02 },
 {10003                  , (uint32_t)&u16TestData3            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10003"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.03 },
 {10004                  , (uint32_t)&u16TestData4            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10004"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.04 },
 {10005                  , (uint32_t)&u16TestData5            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10005"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.05 },
 {10006                  , (uint32_t)&u16TestData6            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10006"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.06 },
 {10007                  , (uint32_t)&u16TestData7            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10007"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.07 },
 {10008                  , (uint32_t)&u16TestData8            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10008"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.08 },
 {10009                  , (uint32_t)&u16TestData9            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10009"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.09 },


 {PARAM_ID_CTRL_PERIOD_US, (uint32_t)&u16PeriodControl_usec   ,  PAR(_UINT16,_RW,_WR)  , {"CtrlPeriod"}    ,    {"usec"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   1.0 },
 {PARAM_ID_CTRL_LOOP_NSEC, (uint32_t)&u16DelayCtrlLoop_100nsec,  PAR(_UINT16,_RW,_WR)  , {"Ctrl_Delay"}    ,    {"nsec"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    , 100.0 },
 {PARAM_ID_CTRL_LOOP_USEC, (uint32_t)&u16DelayCtrlLoop_100nsec,  PAR(_UINT16,_RW,_WR)  , {"Ctrl_Delay"}    ,    {"usec"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.1 },
 {PARAM_ID_MAIN_LOOP_USEC, (uint32_t)&u16DelayMainLoop_usec   ,  PAR(_UINT16,_RW,_WR)  , {"Main_Delay"}    ,    {"usec"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   1.0 },
 {PARAM_ID_PARAM_SET_FAIL, (uint32_t)&u16CountSetParameterFail,  PAR(_UINT16,_RW,_WR)  , {"Param_Fail"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   1.0 },

 {PARAM_ID_PARAM_MAX_TEST, (uint32_t)&u16CountMaxParameterTest,  PAR(_UINT16,_RW,_WR)  , {"Param_Test"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   1.0 },
 {PARAM_ID_RD_WR_SCALETST, (uint32_t)&s16ScaleDataTst         ,  PAR(_SINT16,_RW,_NO)  , {"RDWR_Scale"}    ,    {"\n"}     ,   ( int16_t)(  300)  ,   (uint32_t)( int16_t)(  -100)  ,    ( int16_t)(200)    ,   2.5 },
 {PARAM_ID_READ_WRITE_TST, (uint32_t)&s16DummyDataTst         ,  PAR(_SINT16,_RW,_NO)  , {"RD_WR_Test"}    ,    {" "}      ,   ( int16_t)(  300)  ,   (uint32_t)( int16_t)(  -100)  ,    ( int16_t)(200)    ,   0.0 },
 {PARAM_ID_MODBUS_MSG_CNT, (uint32_t)&u16DummyDataCnt         ,  PAR(_UINT16,_RW,_NO)  , {"Modbus_Cnt"}    ,    {""}       ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.0 },
 {PARAM_ID_CURRENT_PHASEA, (uint32_t)&s16DummyCurrentPhaseA   ,  PAR(_SINT16,_RW,_RD)  , {"CurrentPhA"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {PARAM_ID_CURRENT_PHASEB, (uint32_t)&s16DummyCurrentPhaseB   ,  PAR(_SINT16,_RW,_RD)  , {"CurrentPhB"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {PARAM_ID_CURRENT_PHASEC, (uint32_t)&s16DummyCurrentPhaseC   ,  PAR(_SINT16,_RW,_RD)  , {"CurrentPhC"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {PARAM_ID_VOLTAGE_DCLINK, (uint32_t)&s16DummyVoltageDCLink   ,  PAR(_SINT16,_RW,_RD)  , {"VoltageBus"}    ,    {"V"}      ,   ( int16_t)(13500)  ,   (uint32_t)( int16_t)(  5000)  ,    ( int16_t)(  0)    ,   0.1 },
 {PARAM_ID_INCREMENT_LOOP, (uint32_t)&s16DummyIncrementLoop   ,  PAR(_SINT16,_RW,_WR)  , {"IncLoopTst"}    ,    {"A(0.5V)"},   ( int16_t)( 1024)  ,   (uint32_t)( int16_t)( -1024)  ,    ( int16_t)(256)    ,   0.1 },
 {PARAM_ID_REQRUNNINGMODE, (uint32_t)&bDummyReqstDevRunning   ,  PAR(_UINT08,_WO,_WR)  , {"DevRunning"}    ,    {"boolean"},             ( true)  ,             (uint32_t)( false)  ,              false    ,     1 }, /* If Parameter ID 9 Write Request From PC */
 {PARAM_ID_STARUNNINGMODE, (uint32_t)&bDummyStatsDevRunning   ,  PAR(_UINT08,_RO,_NO)  , {"DevRunning"}    ,    {"boolean"},             ( true)  ,             (uint32_t)( false)  ,              false    ,     1 }, /* If Parameter ID 9 Read  Request From PC (can be with Different Real Address)*/


 {1000                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_WO,_RD)  , {"Param_1000"}    ,    {"WROnly"} ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.0 },
 {1001                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1001"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1002                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1002"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.2 },
 {1003                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1003"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.3 },
 {1004                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1004"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.4 },
 {1005                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1005"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.5 },
 {1006                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1006"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.6 },
 {1007                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1007"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.7 },
 {1008                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1008"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.8 },
 {1009                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1009"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.9 },
 {1010                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1010"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   1.0 },
 {1011                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1011"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1012                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1012"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1013                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1013"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1014                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1014"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1015                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1015"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1016                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1016"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1017                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1017"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1018                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1018"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1019                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1019"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1020                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1020"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1021                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1021"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1022                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1022"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1023                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1023"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1024                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1024"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1025                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1025"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1026                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1026"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1027                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1027"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1028                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1028"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1029                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1029"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1030                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1030"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1031                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1031"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1032                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1032"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1033                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1033"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1034                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1034"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1035                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1035"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1036                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1036"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1037                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1037"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1038                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1038"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1039                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1039"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1040                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1040"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1041                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1041"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1042                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1042"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1043                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1043"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1044                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1044"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1045                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1045"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1046                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1046"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1047                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1047"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1048                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1048"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1049                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1049"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1050                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1050"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1051                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1051"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1052                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1052"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1053                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1053"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1054                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1054"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1055                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1055"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1056                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1056"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1057                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1057"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1058                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1058"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1059                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1059"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1060                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1060"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1061                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1061"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1062                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1062"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1063                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1063"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1064                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1064"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1065                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1065"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1066                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1066"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1067                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1067"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1068                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1068"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1069                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1069"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1070                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1070"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1071                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1071"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1072                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1072"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1073                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1073"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1074                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1074"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1075                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1075"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1076                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1076"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1077                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1077"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1078                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1078"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1079                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1079"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1080                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1080"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1081                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1081"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1082                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1082"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1083                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1083"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1084                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1084"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1085                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1085"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1086                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1086"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1087                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1087"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1088                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1088"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1089                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1089"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1090                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1090"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1091                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1091"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1092                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1092"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1093                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1093"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1094                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1094"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1095                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1095"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1096                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1096"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1097                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1097"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1098                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1098"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1099                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1099"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1100                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1100"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1101                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1101"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1102                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1102"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1103                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1103"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1104                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1104"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1105                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1105"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1106                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1106"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1107                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1107"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1108                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1108"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1109                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1109"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1110                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1110"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1111                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1111"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1112                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1112"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1113                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1113"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1114                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1114"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1115                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1115"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1116                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1116"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1117                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1117"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1118                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1118"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1119                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1119"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1120                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1120"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1121                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1121"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1122                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1122"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1123                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1123"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1124                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1124"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1125                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1125"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1126                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1126"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1127                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1127"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1128                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1128"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1129                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1129"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1130                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1130"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1131                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1131"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1132                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1132"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1133                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1133"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1134                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1134"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1135                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1135"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1136                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1136"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1137                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1137"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1138                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1138"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1139                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1139"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1140                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1140"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1141                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1141"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1142                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1142"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1143                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1143"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1144                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1144"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1145                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1145"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1146                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1146"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1147                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1147"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1148                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1148"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1149                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1149"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1150                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1150"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1151                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1151"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1152                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1152"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1153                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1153"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1154                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1154"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1155                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1155"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1156                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1156"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1157                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1157"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1158                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1158"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1159                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1159"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1160                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1160"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1161                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1161"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1162                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1162"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1163                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1163"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1164                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1164"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1165                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1165"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1166                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1166"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1167                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1167"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1168                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1168"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1169                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1169"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1170                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1170"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1171                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1171"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1172                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1172"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1173                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1173"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1174                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1174"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1175                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1175"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1176                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1176"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1177                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1177"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1178                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1178"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1179                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1179"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1180                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1180"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1181                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1181"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1182                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1182"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1183                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1183"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1184                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1184"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1185                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1185"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1186                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1186"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1187                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1187"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1188                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1188"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1189                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1189"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1190                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1190"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1191                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1191"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1192                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1192"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1193                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1193"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1194                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1194"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1195                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1195"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1196                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1196"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1197                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1197"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1198                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1198"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1199                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1199"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1200                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1200"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1201                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1201"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1202                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1202"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1203                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1203"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1204                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1204"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1205                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1205"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1206                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1206"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1207                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1207"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1208                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1208"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1209                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1209"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1210                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1210"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1211                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1211"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1212                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1212"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1213                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1213"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1214                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1214"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1215                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1215"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1216                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1216"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1217                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1217"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1218                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1218"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1219                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1219"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1220                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1220"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1221                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1221"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1222                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1222"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1223                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1223"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1224                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1224"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1225                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1225"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1226                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1226"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1227                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1227"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1228                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1228"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1229                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1229"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1230                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1230"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1231                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1231"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1232                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1232"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1233                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1233"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1234                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1234"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1235                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1235"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1236                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1236"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1237                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1237"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1238                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1238"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1239                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1239"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1240                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1240"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1241                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1241"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1242                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1242"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1243                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1243"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1244                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1244"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1245                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1245"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1246                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1246"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1247                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1247"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1248                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1248"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1249                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1249"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1250                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1250"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1251                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1251"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1252                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1252"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1253                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1253"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1254                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1254"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1255                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1255"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1256                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1256"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1257                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1257"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1258                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1258"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1259                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1259"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1260                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1260"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1261                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1261"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1262                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1262"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1263                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1263"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1264                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1264"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1265                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1265"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1266                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1266"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1267                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1267"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1268                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1268"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1269                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1269"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1270                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1270"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1271                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1271"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1272                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1272"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1273                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1273"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1274                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1274"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1275                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1275"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1276                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1276"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1277                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1277"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1278                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1278"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1279                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1279"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1280                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1280"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1281                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1281"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1282                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1282"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1283                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1283"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1284                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1284"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1285                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1285"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1286                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1286"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1287                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1287"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1288                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1288"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1289                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1289"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1290                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1290"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1291                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1291"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1292                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1292"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1293                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1293"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1294                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1294"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1295                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1295"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1296                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1296"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1297                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1297"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1298                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1298"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1299                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1299"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1300                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1300"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1301                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1301"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1302                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1302"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1303                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1303"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1304                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1304"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1305                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1305"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1306                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1306"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1307                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1307"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1308                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1308"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1309                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1309"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1310                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1310"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1311                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1311"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1312                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1312"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1313                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1313"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1314                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1314"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1315                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1315"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1316                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1316"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1317                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1317"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1318                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1318"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1319                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1319"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1320                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1320"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1321                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1321"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1322                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1322"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1323                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1323"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1324                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1324"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1325                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1325"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1326                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1326"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1327                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1327"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1328                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1328"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1329                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1329"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1330                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1330"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1331                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1331"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1332                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1332"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1333                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1333"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1334                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1334"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1335                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1335"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1336                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1336"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1337                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1337"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1338                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1338"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1339                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1339"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1340                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1340"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1341                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1341"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1342                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1342"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1343                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1343"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1344                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1344"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1345                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1345"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1346                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1346"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1347                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1347"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1348                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1348"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1349                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1349"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1350                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1350"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1351                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1351"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1352                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1352"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1353                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1353"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1354                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1354"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1355                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1355"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1356                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1356"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1357                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1357"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1358                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1358"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1359                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1359"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1360                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1360"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1361                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1361"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1362                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1362"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1363                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1363"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1364                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1364"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1365                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1365"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1366                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1366"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1367                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1367"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1368                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1368"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1369                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1369"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1370                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1370"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1371                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1371"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1372                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1372"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1373                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1373"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1374                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1374"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1375                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1375"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1376                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1376"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1377                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1377"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1378                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1378"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1379                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1379"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1380                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1380"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1381                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1381"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1382                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1382"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1383                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1383"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1384                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1384"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1385                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1385"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1386                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1386"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1387                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1387"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1388                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1388"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1389                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1389"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1390                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1390"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1391                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1391"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1392                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1392"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1393                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1393"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1394                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1394"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1395                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1395"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1396                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1396"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1397                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1397"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1398                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1398"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1399                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1399"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1400                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1400"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1401                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1401"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1402                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1402"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1403                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1403"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1404                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1404"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1405                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1405"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1406                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1406"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1407                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1407"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1408                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1408"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1409                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1409"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1410                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1410"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1411                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1411"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1412                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1412"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1413                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1413"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1414                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1414"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1415                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1415"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1416                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1416"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1417                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1417"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1418                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1418"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1419                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1419"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1420                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1420"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1421                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1421"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1422                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1422"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1423                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1423"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1424                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1424"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1425                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1425"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1426                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1426"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1427                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1427"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1428                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1428"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1429                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1429"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1430                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1430"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1431                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1431"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1432                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1432"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1433                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1433"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1434                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1434"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1435                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1435"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1436                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1436"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1437                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1437"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1438                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1438"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1439                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1439"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1440                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1440"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1441                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1441"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1442                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1442"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1443                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1443"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1444                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1444"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1445                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1445"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1446                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1446"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1447                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1447"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1448                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1448"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1449                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1449"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1450                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1450"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1451                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1451"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1452                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1452"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1453                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1453"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1454                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1454"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1455                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1455"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1456                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1456"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1457                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1457"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1458                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1458"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1459                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1459"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1460                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1460"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1461                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1461"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1462                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1462"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1463                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1463"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1464                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1464"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1465                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1465"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1466                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1466"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1467                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1467"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1468                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1468"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1469                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1469"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1470                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1470"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1471                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1471"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1472                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1472"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1473                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1473"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1474                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1474"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1475                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1475"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1476                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1476"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1477                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1477"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1478                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1478"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1479                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1479"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1480                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1480"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1481                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1481"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1482                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1482"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1483                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1483"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1484                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1484"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1485                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1485"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1486                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1486"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1487                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1487"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1488                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1488"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1489                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1489"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1490                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1490"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1491                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1491"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1492                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1492"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1493                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1493"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1494                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1494"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1495                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1495"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1496                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1496"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1497                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1497"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1498                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1498"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1499                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1499"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1500                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1500"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1501                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1501"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1502                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1502"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1503                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1503"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1504                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1504"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1505                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1505"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1506                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1506"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1507                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1507"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1508                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1508"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1509                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1509"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1510                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1510"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1511                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1511"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1512                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1512"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1513                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1513"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1514                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1514"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1515                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1515"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1516                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1516"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1517                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1517"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1518                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1518"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1519                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1519"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1520                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1520"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1521                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1521"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1522                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1522"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1523                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1523"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1524                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1524"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1525                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1525"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1526                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1526"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1527                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1527"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1528                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1528"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1529                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1529"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1530                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1530"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1531                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1531"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1532                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1532"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1533                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1533"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1534                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1534"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1535                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1535"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1536                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1536"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1537                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1537"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1538                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1538"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1539                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1539"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1540                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1540"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1541                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1541"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1542                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1542"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1543                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1543"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1544                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1544"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1545                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1545"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1546                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1546"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1547                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1547"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1548                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1548"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1549                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1549"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1550                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1550"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1551                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1551"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1552                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1552"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1553                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1553"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1554                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1554"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1555                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1555"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1556                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1556"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1557                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1557"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1558                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1558"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1559                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1559"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1560                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1560"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1561                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1561"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1562                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1562"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1563                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1563"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1564                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1564"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1565                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1565"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1566                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1566"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1567                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1567"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1568                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1568"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1569                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1569"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1570                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1570"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1571                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1571"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1572                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1572"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1573                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1573"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1574                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1574"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1575                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1575"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1576                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1576"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1577                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1577"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1578                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1578"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1579                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1579"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1580                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1580"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1581                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1581"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1582                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1582"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1583                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1583"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1584                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1584"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1585                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1585"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1586                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1586"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1587                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1587"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1588                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1588"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1589                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1589"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1590                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1590"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1591                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1591"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1592                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1592"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1593                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1593"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1594                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1594"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1595                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1595"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1596                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1596"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1597                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1597"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1598                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1598"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1599                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1599"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1600                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1600"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1601                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1601"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1602                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1602"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1603                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1603"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1604                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1604"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1605                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1605"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1606                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1606"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1607                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1607"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1608                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1608"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1609                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1609"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1610                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1610"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1611                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1611"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1612                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1612"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1613                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1613"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1614                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1614"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1615                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1615"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1616                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1616"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1617                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1617"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1618                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1618"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1619                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1619"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1620                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1620"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1621                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1621"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1622                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1622"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1623                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1623"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1624                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1624"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1625                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1625"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1626                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1626"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1627                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1627"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1628                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1628"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1629                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1629"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1630                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1630"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1631                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1631"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1632                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1632"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1633                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1633"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1634                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1634"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1635                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1635"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1636                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1636"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1637                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1637"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1638                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1638"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1639                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1639"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1640                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1640"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1641                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1641"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1642                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1642"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1643                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1643"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1644                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1644"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1645                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1645"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1646                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1646"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1647                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1647"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1648                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1648"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1649                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1649"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1650                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1650"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1651                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1651"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1652                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1652"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1653                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1653"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1654                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1654"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1655                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1655"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1656                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1656"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1657                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1657"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1658                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1658"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1659                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1659"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1660                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1660"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1661                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1661"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1662                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1662"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1663                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1663"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1664                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1664"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1665                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1665"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1666                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1666"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1667                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1667"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1668                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1668"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1669                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1669"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1670                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1670"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1671                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1671"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1672                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1672"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1673                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1673"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1674                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1674"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1675                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1675"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1676                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1676"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1677                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1677"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1678                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1678"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1679                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1679"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1680                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1680"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1681                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1681"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1682                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1682"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1683                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1683"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1684                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1684"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1685                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1685"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1686                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1686"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1687                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1687"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1688                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1688"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1689                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1689"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1690                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1690"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1691                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1691"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1692                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1692"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1693                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1693"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1694                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1694"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1695                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1695"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1696                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1696"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1697                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1697"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1698                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1698"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1699                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1699"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1700                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1700"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1701                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1701"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1702                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1702"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1703                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1703"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1704                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1704"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1705                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1705"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1706                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1706"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1707                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1707"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1708                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1708"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1709                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1709"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1710                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1710"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1711                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1711"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1712                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1712"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1713                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1713"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1714                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1714"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1715                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1715"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1716                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1716"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1717                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1717"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1718                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1718"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1719                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1719"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1720                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1720"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1721                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1721"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1722                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1722"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1723                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1723"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1724                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1724"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1725                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1725"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1726                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1726"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1727                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1727"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1728                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1728"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1729                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1729"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1730                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1730"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1731                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1731"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1732                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1732"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1733                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1733"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1734                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1734"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1735                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1735"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1736                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1736"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1737                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1737"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1738                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1738"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1739                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1739"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1740                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1740"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1741                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1741"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1742                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1742"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1743                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1743"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1744                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1744"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1745                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1745"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1746                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1746"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1747                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1747"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1748                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1748"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1749                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1749"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1750                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1750"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1751                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1751"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1752                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1752"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1753                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1753"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1754                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1754"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1755                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1755"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1756                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1756"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1757                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1757"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1758                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1758"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1759                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1759"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1760                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1760"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1761                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1761"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1762                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1762"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1763                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1763"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1764                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1764"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1765                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1765"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1766                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1766"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1767                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1767"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1768                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1768"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1769                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1769"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1770                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1770"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1771                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1771"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1772                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1772"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1773                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1773"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1774                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1774"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1775                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1775"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1776                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1776"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1777                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1777"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1778                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1778"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1779                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1779"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1780                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1780"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1781                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1781"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1782                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1782"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1783                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1783"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1784                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1784"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1785                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1785"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1786                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1786"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1787                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1787"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1788                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1788"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1789                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1789"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1790                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1790"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1791                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1791"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1792                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1792"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1793                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1793"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1794                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1794"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1795                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1795"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1796                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1796"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1797                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1797"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1798                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1798"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1799                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1799"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1800                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1800"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1801                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1801"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1802                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1802"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1803                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1803"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1804                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1804"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1805                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1805"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1806                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1806"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1807                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1807"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1808                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1808"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1809                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1809"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1810                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1810"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1811                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1811"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1812                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1812"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1813                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1813"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1814                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1814"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1815                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1815"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1816                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1816"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1817                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1817"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1818                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1818"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1819                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1819"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1820                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1820"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1821                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1821"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1822                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1822"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1823                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1823"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1824                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1824"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1825                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1825"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1826                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1826"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1827                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1827"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1828                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1828"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1829                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1829"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1830                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1830"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1831                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1831"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1832                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1832"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1833                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1833"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1834                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1834"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1835                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1835"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1836                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1836"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1837                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1837"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1838                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1838"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1839                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1839"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1840                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1840"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1841                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1841"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1842                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1842"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1843                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1843"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1844                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1844"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1845                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1845"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1846                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1846"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1847                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1847"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1848                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1848"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1849                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1849"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1850                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1850"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1851                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1851"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1852                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1852"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1853                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1853"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1854                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1854"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1855                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1855"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1856                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1856"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1857                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1857"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1858                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1858"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1859                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1859"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1860                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1860"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1861                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1861"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1862                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1862"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1863                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1863"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1864                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1864"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1865                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1865"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1866                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1866"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1867                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1867"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1868                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1868"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1869                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1869"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1870                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1870"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1871                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1871"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1872                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1872"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1873                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1873"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1874                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1874"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1875                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1875"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1876                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1876"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1877                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1877"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1878                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1878"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1879                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1879"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1880                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1880"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1881                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1881"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1882                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1882"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1883                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1883"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1884                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1884"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1885                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1885"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1886                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1886"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1887                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1887"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1888                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1888"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1889                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1889"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1890                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1890"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1891                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1891"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1892                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1892"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1893                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1893"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1894                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1894"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1895                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1895"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1896                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1896"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1897                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1897"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1898                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1898"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1899                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1899"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1900                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1900"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1901                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1901"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1902                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1902"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1903                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1903"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1904                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1904"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1905                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1905"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1906                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1906"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1907                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1907"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1908                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1908"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1909                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1909"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1910                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1910"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1911                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1911"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1912                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1912"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1913                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1913"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1914                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1914"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1915                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1915"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1916                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1916"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1917                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1917"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1918                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1918"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1919                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1919"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1920                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1920"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1921                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1921"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1922                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1922"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1923                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1923"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1924                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1924"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1925                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1925"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1926                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1926"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1927                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1927"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1928                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1928"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1929                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1929"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1930                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1930"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1931                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1931"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1932                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1932"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1933                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1933"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1934                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1934"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1935                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1935"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1936                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1936"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1937                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1937"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1938                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1938"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1939                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1939"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1940                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1940"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1941                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1941"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1942                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1942"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1943                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1943"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1944                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1944"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1945                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1945"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1946                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1946"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1947                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1947"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1948                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1948"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1949                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1949"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1950                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1950"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1951                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1951"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1952                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1952"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1953                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1953"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1954                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1954"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1955                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1955"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1956                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1956"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1957                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1957"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1958                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1958"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1959                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1959"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1960                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1960"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1961                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1961"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1962                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1962"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1963                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1963"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1964                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1964"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1965                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1965"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1966                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1966"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1967                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1967"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1968                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1968"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1969                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1969"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1970                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1970"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1971                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1971"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1972                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1972"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1973                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1973"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1974                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1974"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1975                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1975"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1976                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1976"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1977                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1977"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1978                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1978"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1979                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1979"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1980                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1980"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1981                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1981"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1982                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1982"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1983                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1983"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1984                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1984"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1985                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1985"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1986                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1986"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1987                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1987"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1988                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1988"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1989                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1989"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1990                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1990"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1991                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1991"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1992                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1992"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1993                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1993"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1994                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1994"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1995                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1995"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1996                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1996"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1997                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1997"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1998                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1998"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {1999                   , ( int16_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1999"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
#endif

#elif CSMON_PARAMETER_LIST_TEST == CSMON_PAR_LIST_EACH_TYPE_REPEATED_ALL_TYPES_COUNT_TIMES

 {10000                  , (uint32_t)&u16TestData0            ,  PAR(_UINT16,_WO,_RD)  , {"Test_10000"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.00 },
 {10001                  , (uint32_t)&u16TestData1            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10001"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.01 },
 {10002                  , (uint32_t)&u16TestData2            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10002"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.02 },
 {10003                  , (uint32_t)&u16TestData3            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10003"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.03 },
 {10004                  , (uint32_t)&u16TestData4            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10004"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.04 },
 {10005                  , (uint32_t)&u16TestData5            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10005"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.05 },
 {10006                  , (uint32_t)&u16TestData6            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10006"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.06 },
 {10007                  , (uint32_t)&u16TestData7            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10007"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.07 },
 {10008                  , (uint32_t)&u16TestData8            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10008"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.08 },
 {10009                  , (uint32_t)&u16TestData9            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10009"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.09 },


 {PARAM_ID_CTRL_PERIOD_US, (uint32_t)&u16PeriodControl_usec   ,  PAR(_UINT16,_RW,_WR)  , {"CtrlPeriod"}    ,    {"usec"}   ,   (uint16_t)(65535)  ,   (uint16_t)(     0)  ,    (uint16_t)(  0)    ,   1.0 },
 {PARAM_ID_CTRL_LOOP_NSEC, (uint32_t)&u16DelayCtrlLoop_100nsec,  PAR(_UINT16,_RW,_WR)  , {"Ctrl_Delay"}    ,    {"nsec"}   ,   (uint16_t)(65535)  ,   (uint16_t)(     0)  ,    (uint16_t)(  0)    , 100.0 },
 {PARAM_ID_CTRL_LOOP_USEC, (uint32_t)&u16DelayCtrlLoop_100nsec,  PAR(_UINT16,_RW,_WR)  , {"Ctrl_Delay"}    ,    {"usec"}   ,   (uint16_t)(65535)  ,   (uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.1 },
 {PARAM_ID_MAIN_LOOP_USEC, (uint32_t)&u16DelayMainLoop_usec   ,  PAR(_UINT16,_RW,_WR)  , {"Main_Delay"}    ,    {"usec"}   ,   (uint16_t)(65535)  ,   (uint16_t)(     0)  ,    (uint16_t)(  0)    ,   1.0 },
 {PARAM_ID_PARAM_SET_FAIL, (uint32_t)&u16CountSetParameterFail,  PAR(_UINT16,_RW,_WR)  , {"Param_Fail"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint16_t)(     0)  ,    (uint16_t)(  0)    ,   1.0 },

 {PARAM_ID_PARAM_MAX_TEST, (uint32_t)&u16CountMaxParameterTest,  PAR(_UINT16,_RW,_WR)  , {"Param_Test"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   1.0 },
 {PARAM_ID_RD_WR_SCALETST, (uint32_t)&s16ScaleDataTst         ,  PAR(_SINT16,_RW,_NO)  , {"RDWR_Scale"}    ,    {"\n"}     ,   ( int16_t)(  300)  ,   (uint32_t)( int16_t)(  -100)  ,    ( int16_t)(200)    ,   2.5 },
 {PARAM_ID_READ_WRITE_TST, (uint32_t)&s16DummyDataTst         ,  PAR(_SINT16,_RW,_NO)  , {"RD_WR_Test"}    ,    {" "}      ,   ( int16_t)(  300)  ,   (uint32_t)( int16_t)(  -100)  ,    ( int16_t)(200)    ,   0.0 },
 {PARAM_ID_MODBUS_MSG_CNT, (uint32_t)&u16DummyDataCnt         ,  PAR(_UINT16,_RW,_NO)  , {"Modbus_Cnt"}    ,    {""}       ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.0 },
 {PARAM_ID_CURRENT_PHASEA, (uint32_t)&s16DummyCurrentPhaseA   ,  PAR(_SINT16,_RW,_RD)  , {"CurrentPhA"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {PARAM_ID_CURRENT_PHASEB, (uint32_t)&s16DummyCurrentPhaseB   ,  PAR(_SINT16,_RW,_RD)  , {"CurrentPhB"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {PARAM_ID_CURRENT_PHASEC, (uint32_t)&s16DummyCurrentPhaseC   ,  PAR(_SINT16,_RW,_RD)  , {"CurrentPhC"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {PARAM_ID_VOLTAGE_DCLINK, (uint32_t)&s16DummyVoltageDCLink   ,  PAR(_SINT16,_RW,_RD)  , {"VoltageBus"}    ,    {"V"}      ,   ( int16_t)(13500)  ,   (uint32_t)( int16_t)(  5000)  ,    ( int16_t)(  0)    ,   0.1 },
 {PARAM_ID_INCREMENT_LOOP, (uint32_t)&s16DummyIncrementLoop   ,  PAR(_SINT16,_RW,_WR)  , {"IncLoopTst"}    ,    {"A(0.5V)"},   ( int16_t)( 1024)  ,   (uint32_t)( int16_t)( -1024)  ,    ( int16_t)(256)    ,   0.1 },
 {PARAM_ID_REQRUNNINGMODE, (uint32_t)&bDummyReqstDevRunning   ,  PAR(_UINT08,_WO,_WR)  , {"DevRunning"}    ,    {"boolean"},             ( true)  ,             (uint32_t)( false)  ,              false    ,     1 }, /* If Parameter ID 9 Write Request From PC */
 {PARAM_ID_STARUNNINGMODE, (uint32_t)&bDummyStatsDevRunning   ,  PAR(_UINT08,_RO,_NO)  , {"DevRunning"}    ,    {"boolean"},             ( true)  ,             (uint32_t)( false)  ,              false    ,     1 }, /* If Parameter ID 9 Read  Request From PC (can be with Different Real Address)*/


 {20000                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"0.0"}    ,   (int16_t)(10000)   ,   (uint32_t)(int16_t)(-10000)   ,    ( int16_t)(  0)    ,   0.0 },
 {20001                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"0.0"}    ,   (int16_t)(10000)   ,   (uint32_t)(int16_t)(-10000)   ,    ( int16_t)(  0)    ,   0.0 },
 {20002                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"0.0"}    ,   (int16_t)(10000)   ,   (uint32_t)(int16_t)(-10000)   ,    ( int16_t)(  0)    ,   0.0 },
 {20003                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"0.0"}    ,   (int16_t)(10000)   ,   (uint32_t)(int16_t)(-10000)   ,    ( int16_t)(  0)    ,   0.0 },
 {20004                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"0.0"}    ,   (int16_t)(10000)   ,   (uint32_t)(int16_t)(-10000)   ,    ( int16_t)(  0)    ,   0.0 },
 {20005                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"0.0"}    ,   (int16_t)(10000)   ,   (uint32_t)(int16_t)(-10000)   ,    ( int16_t)(  0)    ,   0.0 },

 {20010                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"1.0"}    ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   1.0 },
 {20011                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"1.0"}    ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   1.0 },
 {20012                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"1.0"}    ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   1.0 },
 {20013                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"1.0"}    ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   1.0 },
 {20014                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"1.0"}    ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   1.0 },
 {20015                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"1.0"}    ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   1.0 },

 {20020                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"0.0"}    ,   (uint32_t)(1000000),   (uint32_t)(uint32_t)(-1000000),    (uint32_t)(  0)    ,   0.0 },
 {20021                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"0.0"}    ,   (uint32_t)(1000000),   (uint32_t)(uint32_t)(-1000000),    (uint32_t)(  0)    ,   0.0 },
 {20022                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"0.0"}    ,   (uint32_t)(1000000),   (uint32_t)(uint32_t)(-1000000),    (uint32_t)(  0)    ,   0.0 },
 {20023                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"0.0"}    ,   (uint32_t)(1000000),   (uint32_t)(uint32_t)(-1000000),    (uint32_t)(  0)    ,   0.0 },
 {20024                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"0.0"}    ,   (uint32_t)(1000000),   (uint32_t)(uint32_t)(-1000000),    (uint32_t)(  0)    ,   0.0 },
 {20025                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"0.0"}    ,   (uint32_t)(1000000),   (uint32_t)(uint32_t)(-1000000),    (uint32_t)(  0)    ,   0.0 },

 {20030                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"1.0"}    ,   (uint32_t)(1000000),   (uint32_t)(uint32_t)(-1000000),    (uint32_t)(  0)    ,   1.0 },
 {20031                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"1.0"}    ,   (uint32_t)(1000000),   (uint32_t)(uint32_t)(-1000000),    (uint32_t)(  0)    ,   1.0 },
 {20032                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"1.0"}    ,   (uint32_t)(1000000),   (uint32_t)(uint32_t)(-1000000),    (uint32_t)(  0)    ,   1.0 },
 {20033                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"1.0"}    ,   (uint32_t)(1000000),   (uint32_t)(uint32_t)(-1000000),    (uint32_t)(  0)    ,   1.0 },
 {20034                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"1.0"}    ,   (uint32_t)(1000000),   (uint32_t)(uint32_t)(-1000000),    (uint32_t)(  0)    ,   1.0 },
 {20035                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"1.0"}    ,   (uint32_t)(1000000),   (uint32_t)(uint32_t)(-1000000),    (uint32_t)(  0)    ,   1.0 },

 {20040                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"0.0"}    ,(uint_least8_t)(255)  ,   (uint32_t)(uint_least8_t)(0)  , (uint_least8_t)(0)    ,   0.0 },
 {20041                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"0.0"}    ,(uint_least8_t)(255)  ,   (uint32_t)(uint_least8_t)(0)  , (uint_least8_t)(0)    ,   0.0 },
 {20042                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"0.0"}    ,(uint_least8_t)(255)  ,   (uint32_t)(uint_least8_t)(0)  , (uint_least8_t)(0)    ,   0.0 },
 {20043                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"0.0"}    ,(uint_least8_t)(255)  ,   (uint32_t)(uint_least8_t)(0)  , (uint_least8_t)(0)    ,   0.0 },
 {20044                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"0.0"}    ,(uint_least8_t)(255)  ,   (uint32_t)(uint_least8_t)(0)  , (uint_least8_t)(0)    ,   0.0 },
 {20045                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"0.0"}    ,(uint_least8_t)(255)  ,   (uint32_t)(uint_least8_t)(0)  , (uint_least8_t)(0)    ,   0.0 },

 {20050                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"1.0"}    ,(uint_least8_t)(255)  ,   (uint32_t)(uint_least8_t)(0)  , (uint_least8_t)(0)    ,   1.0 },
 {20051                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"1.0"}    ,(uint_least8_t)(255)  ,   (uint32_t)(uint_least8_t)(0)  , (uint_least8_t)(0)    ,   1.0 },
 {20052                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"1.0"}    ,(uint_least8_t)(255)  ,   (uint32_t)(uint_least8_t)(0)  , (uint_least8_t)(0)    ,   1.0 },
 {20053                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"1.0"}    ,(uint_least8_t)(255)  ,   (uint32_t)(uint_least8_t)(0)  , (uint_least8_t)(0)    ,   1.0 },
 {20054                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"1.0"}    ,(uint_least8_t)(255)  ,   (uint32_t)(uint_least8_t)(0)  , (uint_least8_t)(0)    ,   1.0 },
 {20055                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"1.0"}    ,(uint_least8_t)(255)  ,   (uint32_t)(uint_least8_t)(0)  , (uint_least8_t)(0)    ,   1.0 },


#elif CSMON_PARAMETER_LIST_TEST == CSMON_PAR_LIST_ALL_TYPES_REPEATED_ALL_TYPES_COUNT_TIMES

 {10000                  , (uint32_t)&u16TestData0            ,  PAR(_UINT16,_WO,_RD)  , {"Test_10000"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.00 },
 {10001                  , (uint32_t)&u16TestData1            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10001"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.01 },
 {10002                  , (uint32_t)&u16TestData2            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10002"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.02 },
 {10003                  , (uint32_t)&u16TestData3            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10003"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.03 },
 {10004                  , (uint32_t)&u16TestData4            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10004"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.04 },
 {10005                  , (uint32_t)&u16TestData5            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10005"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.05 },
 {10006                  , (uint32_t)&u16TestData6            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10006"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.06 },
 {10007                  , (uint32_t)&u16TestData7            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10007"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.07 },
 {10008                  , (uint32_t)&u16TestData8            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10008"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.08 },
 {10009                  , (uint32_t)&u16TestData9            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10009"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.09 },


 {PARAM_ID_CTRL_PERIOD_US, (uint32_t)&u16PeriodControl_usec   ,  PAR(_UINT16,_RW,_WR)  , {"CtrlPeriod"}    ,    {"usec"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   1.0 },
 {PARAM_ID_CTRL_LOOP_NSEC, (uint32_t)&u16DelayCtrlLoop_100nsec,  PAR(_UINT16,_RW,_WR)  , {"Ctrl_Delay"}    ,    {"nsec"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    , 100.0 },
 {PARAM_ID_CTRL_LOOP_USEC, (uint32_t)&u16DelayCtrlLoop_100nsec,  PAR(_UINT16,_RW,_WR)  , {"Ctrl_Delay"}    ,    {"usec"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.1 },
 {PARAM_ID_MAIN_LOOP_USEC, (uint32_t)&u16DelayMainLoop_usec   ,  PAR(_UINT16,_RW,_WR)  , {"Main_Delay"}    ,    {"usec"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   1.0 },
 {PARAM_ID_PARAM_SET_FAIL, (uint32_t)&u16CountSetParameterFail,  PAR(_UINT16,_RW,_WR)  , {"Param_Fail"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   1.0 },

 {PARAM_ID_PARAM_MAX_TEST, (uint32_t)&u16CountMaxParameterTest,  PAR(_UINT16,_RW,_WR)  , {"Param_Test"}    ,    {"unit"}   ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   1.0 },
 {PARAM_ID_RD_WR_SCALETST, (uint32_t)&s16ScaleDataTst         ,  PAR(_SINT16,_RW,_NO)  , {"RDWR_Scale"}    ,    {"\n"}     ,   ( int16_t)(  300)  ,   (uint32_t)( int16_t)(  -100)  ,    ( int16_t)(200)    ,   2.5 },
 {PARAM_ID_READ_WRITE_TST, (uint32_t)&s16DummyDataTst         ,  PAR(_SINT16,_RW,_NO)  , {"RD_WR_Test"}    ,    {" "}      ,   ( int16_t)(  300)  ,   (uint32_t)( int16_t)(  -100)  ,    ( int16_t)(200)    ,   0.0 },
 {PARAM_ID_MODBUS_MSG_CNT, (uint32_t)&u16DummyDataCnt         ,  PAR(_UINT16,_RW,_NO)  , {"Modbus_Cnt"}    ,    {""}       ,   (uint16_t)(65535)  ,   (uint32_t)(uint16_t)(     0)  ,    (uint16_t)(  0)    ,   0.0 },
 {PARAM_ID_CURRENT_PHASEA, (uint32_t)&s16DummyCurrentPhaseA   ,  PAR(_SINT16,_RW,_RD)  , {"CurrentPhA"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {PARAM_ID_CURRENT_PHASEB, (uint32_t)&s16DummyCurrentPhaseB   ,  PAR(_SINT16,_RW,_RD)  , {"CurrentPhB"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {PARAM_ID_CURRENT_PHASEC, (uint32_t)&s16DummyCurrentPhaseC   ,  PAR(_SINT16,_RW,_RD)  , {"CurrentPhC"}    ,    {"A"}      ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.1 },
 {PARAM_ID_VOLTAGE_DCLINK, (uint32_t)&s16DummyVoltageDCLink   ,  PAR(_SINT16,_RW,_RD)  , {"VoltageBus"}    ,    {"V"}      ,   ( int16_t)(13500)  ,   (uint32_t)( int16_t)(  5000)  ,    ( int16_t)(  0)    ,   0.1 },
 {PARAM_ID_INCREMENT_LOOP, (uint32_t)&s16DummyIncrementLoop   ,  PAR(_SINT16,_RW,_WR)  , {"IncLoopTst"}    ,    {"A(0.5V)"},   ( int16_t)( 1024)  ,   (uint32_t)( int16_t)( -1024)  ,    ( int16_t)(256)    ,   0.1 },
 {PARAM_ID_REQRUNNINGMODE, (uint32_t)&bDummyReqstDevRunning   ,  PAR(_UINT08,_WO,_WR)  , {"DevRunning"}    ,    {"boolean"},             ( true)  ,             (uint32_t)( false)  ,              false    ,     1 }, /* If Parameter ID 9 Write Request From PC */
 {PARAM_ID_STARUNNINGMODE, (uint32_t)&bDummyStatsDevRunning   ,  PAR(_UINT08,_RO,_NO)  , {"DevRunning"}    ,    {"boolean"},             ( true)  ,             (uint32_t)( false)  ,              false    ,     1 }, /* If Parameter ID 9 Read  Request From PC (can be with Different Real Address)*/


 {20000                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"0.0"}    ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.0 },
 {20001                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"1.0"}    ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   1.0 },
 {20002                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"0.0"}    ,   ( int32_t)(1000000),   (uint32_t)( int32_t)(-1000000),    ( int32_t)(  0)    ,   0.0 },
 {20003                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"1.0"}    ,   ( int32_t)(1000000),   (uint32_t)( int32_t)(-1000000),    ( int32_t)(  0)    ,   1.0 },
 {20004                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"0.0"}    ,(uint_least8_t)(255)  ,   (uint32_t)(uint_least8_t)(0)  , (uint_least8_t)(  0)  ,   0.0 },
 {20005                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"1.0"}    ,(uint_least8_t)(255)  ,   (uint32_t)(uint_least8_t)(0)  , (uint_least8_t)(  0)  ,   1.0 },

 {20010                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"0.0"}    ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.0 },
 {20011                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"1.0"}    ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   1.0 },
 {20012                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"0.0"}    ,   ( int32_t)(1000000),   (uint32_t)( int32_t)(-1000000),    ( int32_t)(  0)    ,   0.0 },
 {20013                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"1.0"}    ,   ( int32_t)(1000000),   (uint32_t)( int32_t)(-1000000),    ( int32_t)(  0)    ,   1.0 },
 {20014                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"0.0"}    ,(uint_least8_t)(255)  ,   (uint32_t)(uint_least8_t)(0)  , (uint_least8_t)(  0)  ,   0.0 },
 {20015                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"1.0"}    ,(uint_least8_t)(255)  ,   (uint32_t)(uint_least8_t)(0)  , (uint_least8_t)(  0)  ,   1.0 },

 {20020                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"0.0"}    ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.0 },
 {20021                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"1.0"}    ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   1.0 },
 {20022                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"0.0"}    ,   ( int32_t)(1000000),   (uint32_t)( int32_t)(-1000000),    ( int32_t)(  0)    ,   0.0 },
 {20023                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"1.0"}    ,   ( int32_t)(1000000),   (uint32_t)( int32_t)(-1000000),    ( int32_t)(  0)    ,   1.0 },
 {20024                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"0.0"}    ,(uint_least8_t)(255)  ,   (uint32_t)(uint_least8_t)(0)  , (uint_least8_t)(  0)  ,   0.0 },
 {20025                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"1.0"}    ,(uint_least8_t)(255)  ,   (uint32_t)(uint_least8_t)(0)  , (uint_least8_t)(  0)  ,   1.0 },

 {20030                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"0.0"}    ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.0 },
 {20031                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"1.0"}    ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   1.0 },
 {20032                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"0.0"}    ,   ( int32_t)(1000000),   (uint32_t)( int32_t)(-1000000),    ( int32_t)(  0)    ,   0.0 },
 {20033                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"1.0"}    ,   ( int32_t)(1000000),   (uint32_t)( int32_t)(-1000000),    ( int32_t)(  0)    ,   1.0 },
 {20034                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"0.0"}    ,(uint_least8_t)(255)  ,   (uint32_t)(uint_least8_t)(0)  , (uint_least8_t)(  0)  ,   0.0 },
 {20035                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"1.0"}    ,(uint_least8_t)(255)  ,   (uint32_t)(uint_least8_t)(0)  , (uint_least8_t)(  0)  ,   1.0 },

 {20040                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"0.0"}    ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.0 },
 {20041                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"1.0"}    ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   1.0 },
 {20042                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"0.0"}    ,   ( int32_t)(1000000),   (uint32_t)( int32_t)(-1000000),    ( int32_t)(  0)    ,   0.0 },
 {20043                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"1.0"}    ,   ( int32_t)(1000000),   (uint32_t)( int32_t)(-1000000),    ( int32_t)(  0)    ,   1.0 },
 {20044                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"0.0"}    ,(uint_least8_t)(255)  ,   (uint32_t)(uint_least8_t)(0)  , (uint_least8_t)(  0)  ,   0.0 },
 {20045                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"1.0"}    ,(uint_least8_t)(255)  ,   (uint32_t)(uint_least8_t)(0)  , (uint_least8_t)(  0)  ,   1.0 },

 {20050                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"0.0"}    ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   0.0 },
 {20051                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"1.0"}    ,   ( int16_t)(10000)  ,   (uint32_t)( int16_t)(-10000)  ,    ( int16_t)(  0)    ,   1.0 },
 {20052                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"0.0"}    ,   ( int32_t)(1000000),   (uint32_t)( int32_t)(-1000000),    ( int32_t)(  0)    ,   0.0 },
 {20053                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"1.0"}    ,   ( int32_t)(1000000),   (uint32_t)( int32_t)(-1000000),    ( int32_t)(  0)    ,   1.0 },
 {20054                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"0.0"}    ,(uint_least8_t)(255)  ,   (uint32_t)(uint_least8_t)(0)  , (uint_least8_t)(  0)  ,   0.0 },
 {20055                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"1.0"}    ,(uint_least8_t)(255)  ,   (uint32_t)(uint_least8_t)(0)  , (uint_least8_t)(  0)  ,   1.0 },


#else

#endif


};



/* *****************************************************************************
 * Prototypes of functions definitions
 **************************************************************************** */
void ControlProcess(void);
void ParameterInitialization(void);
void RecordersInitialization(void);
void ScopesInitialization(void);

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
        }


        s16DummyCurrentPhaseA += s16DummyIncrementLoop;
        s16DummyCurrentPhaseB += s16DummyIncrementLoop;
        s16DummyCurrentPhaseC += s16DummyIncrementLoop;
        s16DummyVoltageDCLink +=(s16DummyIncrementLoop>>1);
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
        }

        s16DummyCurrentPhaseA = 0;
        s16DummyCurrentPhaseB = 0;
        s16DummyCurrentPhaseC = 0;
        s16DummyVoltageDCLink = 0;
    }


    //
    // Artificial Delay Control Loop
    //
    SysCtl_delay(u32DelayCtrlLoop_Ticks);



    //
    // Process Passed Flag Set - Need to be called from Processes with higher priority level in order CSMON to be able to get meaning-full (consistent) data
    eResponseCode_CSMON_eSetFlagProcessPassed = CSMON_eSetFlagProcessPassed (CSMON_ID_PROCESS_CONTROL_PRIMARY);
    ASSERT(eResponseCode_CSMON_eSetFlagProcessPassed != CSMON_RESPONSE_CODE_OK);
    // Check CSMON Response Code (... or Embed Assert For Debug) if needed


    GPIO_writePin(STAT_LED_R_PIN, STAT_LED_DISABLE_LVL_HIGH);    /* Red LED (closest to the Debug Header) */
}


/* *****************************************************************************
 * ParameterInitialization
 **************************************************************************** */
void ParameterInitialization(void)
{
    uint16_t u16Index;

    uint32_t u32ParamRealAddress;

    volatile uint32_t u32ParamVer;
    volatile uint32_t u32DateTime;
    volatile uint32_t u32CheckSum;

    uWord32_t uParamVerBackup;
    uWord32_t uDateTimeBackup;
    uWord32_t uCheckSumBackup;

    /* Note: with current EMIF board configuration 32Bit Writes Are not working - need use 16-bit writes */
    uParamVerBackup.au16Word[0] = EMIF_AUX_pu16ParamVerBackupInEmif[0];              /* Get Stored In MRAM TableVer Backup */
    uParamVerBackup.au16Word[1] = EMIF_AUX_pu16ParamVerBackupInEmif[1];              /* Get Stored In MRAM TableVer Backup */
    uDateTimeBackup.au16Word[0] = EMIF_AUX_pu16DateTimeBackupInEmif[0];              /* Get Stored In MRAM DateTime Backup */
    uDateTimeBackup.au16Word[1] = EMIF_AUX_pu16DateTimeBackupInEmif[1];              /* Get Stored In MRAM DateTime Backup */
    uCheckSumBackup.au16Word[0] = EMIF_AUX_pu16CheckSumBackupInEmif[0];              /* Get Stored In MRAM CheckSum Backup */
    uCheckSumBackup.au16Word[1] = EMIF_AUX_pu16CheckSumBackupInEmif[1];              /* Get Stored In MRAM CheckSum Backup */

    u32ParamVer = PARAM_TABLE_VERSION;
    u32DateTime = PARAM_TABLE_DATETIME;
    u32CheckSum = CSMON_u32GetParameterCheckSum();                        /* Get Checksum From CSMON */

    u16CountSetParameterFail = 0;

    if ( (uParamVerBackup.u32Register != u32ParamVer)
      || (uDateTimeBackup.u32Register != u32DateTime)
      || (uCheckSumBackup.u32Register != u32CheckSum) )                /* ParamVer or DateTime or Checksum MisMatch */
    {
        /* Invalid Table - Reset Parameter Table */
        CSMON_eResetParameterTable();                                   /* Reset Internal Used Parameters Count */

        /* Add Parameters */
        for (u16Index = 0; u16Index < PARAMETER_COUNT_MAX; u16Index++)
        {
            GPIO_writePin(STAT_LED_A_B_PIN, (u16Index & 1) );     /* Amber LED (middle Led) */

            u32ParamRealAddress = asParameterList[u16Index].u32RealAddress;

            if (asParameterList[u16Index].u16ParameterIndexID == PARAM_ID_MODBUS_MSG_CNT)
            {
                pu16ModbusMessageCounter = (uint16_t*)CSMON_u32GetModbusMessageCounterRegisterRealAddress();
                u32ParamRealAddress = (uint32_t)pu16ModbusMessageCounter;
            }

            eResponseCode_CSMON_eSetParameter =
                CSMON_eSetParameter (
                    asParameterList[u16Index].u16ParameterIndexID,
                    u32ParamRealAddress,
                    asParameterList[u16Index].u16ParamAttributes,
   (uint_least8_t*)&asParameterList[u16Index].au8Name,
   (uint_least8_t*)&asParameterList[u16Index].au8Unit,
                    asParameterList[u16Index].u32Max.u32Register,
                    asParameterList[u16Index].u32Min.u32Register,
                    asParameterList[u16Index].u32Def.u32Register,
                    asParameterList[u16Index].Norm);
            if(eResponseCode_CSMON_eSetParameter != CSMON_RESPONSE_CODE_OK)
            {
                u16CountSetParameterFail++;
            }
        }

        /* Backup ParamVer */
        uParamVerBackup.u32Register = u32ParamVer;
        EMIF_AUX_pu16ParamVerBackupInEmif[0] = uParamVerBackup.au16Word[0];
        EMIF_AUX_pu16ParamVerBackupInEmif[1] = uParamVerBackup.au16Word[1];
        /* Backup DateTime */
        uDateTimeBackup.u32Register = u32DateTime;
        EMIF_AUX_pu16DateTimeBackupInEmif[0] = uDateTimeBackup.au16Word[0];
        EMIF_AUX_pu16DateTimeBackupInEmif[1] = uDateTimeBackup.au16Word[1];
        /* Backup Checksum */
        uCheckSumBackup.u32Register = CSMON_u32GetParameterCheckSum();
        EMIF_AUX_pu16CheckSumBackupInEmif[0] = uCheckSumBackup.au16Word[0];
        EMIF_AUX_pu16CheckSumBackupInEmif[1] = uCheckSumBackup.au16Word[1];


        GPIO_writePin(STAT_LED_A_B_PIN, STAT_LED_ENABLE_LEVEL_LOW);         /* Amber LED (middle Led) */
        CSMON_eApplyParameterChanges();                         /* Internal Library Apply Written Parameters */
        GPIO_writePin(STAT_LED_A_B_PIN, STAT_LED_DISABLE_LVL_HIGH);        /* Amber LED (middle Led) */
    }


}

/* *****************************************************************************
 * RecordersInitialization
 **************************************************************************** */
void RecordersInitialization(void)
{
    /* Recorder 0 */
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_0, PARAM_ID_CURRENT_PHASEA, CSMON_POSITION_IN_RECORDER_0);
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_0, PARAM_ID_CURRENT_PHASEB, CSMON_POSITION_IN_RECORDER_1);
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_0, PARAM_ID_CURRENT_PHASEC, CSMON_POSITION_IN_RECORDER_2);
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_0, PARAM_ID_VOLTAGE_DCLINK, CSMON_POSITION_IN_RECORDER_3);
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterCountInRecorder (
            CSMON_RECORDER_0, CSMON_COUNT_PARAMETERS_4);

    /* Recorder 1 */
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_1, PARAM_ID_CURRENT_PHASEA, CSMON_POSITION_IN_RECORDER_0);
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_1, PARAM_ID_CURRENT_PHASEB, CSMON_POSITION_IN_RECORDER_1);
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_1, PARAM_ID_CURRENT_PHASEC, CSMON_POSITION_IN_RECORDER_2);
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_1, PARAM_ID_VOLTAGE_DCLINK, CSMON_POSITION_IN_RECORDER_3);
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterCountInRecorder (
            CSMON_RECORDER_1, CSMON_COUNT_PARAMETERS_4);

    /* Recorder 2 */
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_2, PARAM_ID_CURRENT_PHASEA, CSMON_POSITION_IN_RECORDER_0);
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_2, PARAM_ID_CURRENT_PHASEB, CSMON_POSITION_IN_RECORDER_1);
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_2, PARAM_ID_CURRENT_PHASEC, CSMON_POSITION_IN_RECORDER_2);
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterInRecorderAtPosition (
            CSMON_RECORDER_2, PARAM_ID_VOLTAGE_DCLINK, CSMON_POSITION_IN_RECORDER_3);
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetParameterCountInRecorder (
            CSMON_RECORDER_2, CSMON_COUNT_PARAMETERS_4);


    /* Trigger Recorder 0 */
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetRecorderTriggerAtPosition (
            CSMON_RECORDER_0,
            PARAM_ID_STARUNNINGMODE,
            (uint32_t)true,
            (uint16_t)CSMON_TRIGGER_MODE_FALLING_EDGE);

    /* Trigger Recorder 1 */
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetRecorderTriggerAtPosition (
            CSMON_RECORDER_1,
            PARAM_ID_STARUNNINGMODE,
            (uint32_t)true,
            (uint16_t)CSMON_TRIGGER_MODE_FALLING_EDGE);

    /* Trigger Recorder 2 */
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetRecorderTriggerAtPosition (
            CSMON_RECORDER_2,
            (uint16_t)0,
            (uint32_t)0,
            (uint16_t)CSMON_TRIGGER_MODE_NONE);


    /* Recorder 0 Configuration */
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetRecorderConfiguration (
            CSMON_RECORDER_0,
            5900,   /* PreTriggerSampleCount */
            6000,   /* TotalSampleCount */
            20000.0); /* Sample Frequency in Hz */

    /* Recorder 1 Configuration */
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetRecorderConfiguration (
            CSMON_RECORDER_1,
            5900,   /* PreTriggerSampleCount */
            6000,   /* TotalSampleCount */
            20000.0); /* Sample Frequency in Hz */

    /* Recorder 2 Configuration */
    eResponseCode_CSMON_eSetRecorder = CSMON_eSetRecorderConfiguration (
            CSMON_RECORDER_2,
            5900,   /* PreTriggerSampleCount */
            6000,   /* TotalSampleCount */
            20000.0); /* Sample Frequency in Hz */

}

/* *****************************************************************************
 * ScopesInitialization
 **************************************************************************** */
void ScopesInitialization(void)
{
    /* Scope 0 */
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterInScopeAtPosition (
            CSMON_SCOPE_0, PARAM_ID_CURRENT_PHASEA, CSMON_POSITION_IN_SCOPE_0);
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterInScopeAtPosition (
            CSMON_SCOPE_0, PARAM_ID_CURRENT_PHASEB, CSMON_POSITION_IN_SCOPE_1);
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterInScopeAtPosition (
            CSMON_SCOPE_0, PARAM_ID_CURRENT_PHASEC, CSMON_POSITION_IN_SCOPE_2);
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterInScopeAtPosition (
            CSMON_SCOPE_0, PARAM_ID_VOLTAGE_DCLINK, CSMON_POSITION_IN_SCOPE_3);
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterCountInScope (
            CSMON_SCOPE_0, CSMON_COUNT_PARAMETERS_4);

    /* Scope 1 */
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterInScopeAtPosition (
            CSMON_SCOPE_1, PARAM_ID_CURRENT_PHASEA, CSMON_POSITION_IN_SCOPE_0);
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterInScopeAtPosition (
            CSMON_SCOPE_1, PARAM_ID_CURRENT_PHASEB, CSMON_POSITION_IN_SCOPE_1);
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterInScopeAtPosition (
            CSMON_SCOPE_1, PARAM_ID_CURRENT_PHASEC, CSMON_POSITION_IN_SCOPE_2);
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterInScopeAtPosition (
            CSMON_SCOPE_1, PARAM_ID_VOLTAGE_DCLINK, CSMON_POSITION_IN_SCOPE_3);
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterCountInScope (
            CSMON_SCOPE_1, CSMON_COUNT_PARAMETERS_4);

    /* Scope 2 */
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterInScopeAtPosition (
            CSMON_SCOPE_2, PARAM_ID_CURRENT_PHASEA, CSMON_POSITION_IN_SCOPE_0);
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterInScopeAtPosition (
            CSMON_SCOPE_2, PARAM_ID_CURRENT_PHASEB, CSMON_POSITION_IN_SCOPE_1);
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterInScopeAtPosition (
            CSMON_SCOPE_2, PARAM_ID_CURRENT_PHASEC, CSMON_POSITION_IN_SCOPE_2);
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterInScopeAtPosition (
            CSMON_SCOPE_2, PARAM_ID_VOLTAGE_DCLINK, CSMON_POSITION_IN_SCOPE_3);
    eResponseCode_CSMON_eSetScope = CSMON_eSetParameterCountInScope (
            CSMON_SCOPE_2, CSMON_COUNT_PARAMETERS_4);

}


/* *****************************************************************************
 * main
 **************************************************************************** */
void main(void)
{
    uint16_t u16FreeRunningTimerTicksPerMicroSecond;
    uint16_t u16FreeRunningTimerPrescaller;

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
    EMIF_DRV_vInit();


    //
    // Clear all interrupts and initialize PIE vector table:
    // Disable CPU interrupts
    // Initialize interrupt controller and vector table.
    //
    Interrupt_initModule();
    Interrupt_initVectorTable();



    //
    // LEDs
    //

    // STAT_LED_G_PIN is the LED STATUS pin. - Init and Main Loop CSMON CPU Load -> Green LED (closest to the MCU Led)
    GPIO_setPinConfigOutput(STAT_LED_G_PIN);
    GPIO_writePin(STAT_LED_G_PIN, STAT_LED_ENABLE_LEVEL_LOW);
    GPIO_writePin(STAT_LED_G_PIN, STAT_LED_DISABLE_LVL_HIGH);

    // STAT_LED_A_B_PIN is the LED STATUS pin. - Parameter Initialize -> Amber LED (middle Led)
    GPIO_setPinConfigOutput(STAT_LED_A_B_PIN);
    GPIO_writePin(STAT_LED_A_B_PIN, STAT_LED_ENABLE_LEVEL_LOW);
    GPIO_writePin(STAT_LED_A_B_PIN, STAT_LED_DISABLE_LVL_HIGH);

    // STAT_LED_R_PIN is the LED STATUS pin. - Control Process -> Red LED (closest to the Debug Header)
    GPIO_setPinConfigOutput(STAT_LED_R_PIN);
    GPIO_writePin(STAT_LED_R_PIN, STAT_LED_ENABLE_LEVEL_LOW);
    GPIO_writePin(STAT_LED_R_PIN, STAT_LED_DISABLE_LVL_HIGH);



    //
    // CLK_EN_FPGA_PIN is the FPGA Clock Enable pin. - generally not needed - there is pull-up
    //
    GPIO_setPinConfigOutput(CLK_EN_FPGA_PIN);
    GPIO_writePin(CLK_EN_FPGA_PIN, 1);




    #ifdef _CSMON_FREE_RUN_TIMER_EXTERNAL           /* defined in Project Configuration Build Settings */
    //
    // CPUTimer1 Initialization as Free-Run Mode with Maximum Period
    #define CPUTIMER1_PRESCALLER_VALUE      ((DEVICE_SYSCLK_FREQ /   1000000)-1)    /* Timer Tick 1.0 microseconds */
    //#define CPUTIMER1_PRESCALLER_VALUE      ((DEVICE_SYSCLK_FREQ / 200000000)-1)    /* Timer Tick 5.0 nanoseconds */
    //
    CPUTimer_setPeriod(CPUTIMER1_BASE, 0xFFFFFFFF);
    CPUTimer_setPreScaler(CPUTIMER1_BASE, CPUTIMER1_PRESCALLER_VALUE);
    CPUTimer_stopTimer(CPUTIMER1_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER1_BASE);
    CPUTimer_enableInterrupt(CPUTIMER1_BASE);
    CPUTimer_startTimer(CPUTIMER1_BASE);
    #endif

    //
    //  Calculate Free Running Timer Ticks Per microsecond
    //
    u16FreeRunningTimerPrescaller = (HWREGH(CPUTIMER1_BASE + CPUTIMER_O_TPRH) << 8U) + (HWREGH(CPUTIMER1_BASE + CPUTIMER_O_TPR) & CPUTIMER_TPR_TDDR_M);
    u16FreeRunningTimerTicksPerMicroSecond = ((DEVICE_SYSCLK_FREQ / 1000000) / (u16FreeRunningTimerPrescaller+1));
    if (u16FreeRunningTimerTicksPerMicroSecond == 0)
    {
        u16FreeRunningTimerTicksPerMicroSecond = 1;
    }


    //
    //  FPGA SCI Communication
    //
    #define SCIRXDC_PIN         139     /* MIX With FLT4_PIN */
    #define SCITXDC_PIN         140     /* MIX With FLT5_PIN */
    //
    // SCIRXDC_PIN is the SCI Rx pin.
    //
    GPIO_setPinConfig(GPIO_PIN_MODE(SCIRXDC_PIN, SCIRXDC));
    GPIO_setDirectionMode(SCIRXDC_PIN, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(SCIRXDC_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SCIRXDC_PIN, GPIO_QUAL_ASYNC);
    GPIO_setMasterCore(SCIRXDC_PIN, GPIO_CORE_CPU1);
    //
    // SCITXDC_PIN is the SCI Tx pin.
    //
    GPIO_setPinConfig(GPIO_PIN_MODE(SCITXDC_PIN, SCITXDC));
    GPIO_setDirectionMode(SCITXDC_PIN, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(SCITXDC_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SCITXDC_PIN, GPIO_QUAL_ASYNC);
    GPIO_setMasterCore(SCITXDC_PIN, GPIO_CORE_CPU1);
    //
    //  SCIC Init and FPGA_SCI_DRV Init
    //
    Interrupt_register(INT_SCIC_RX, &SCIC_DRV_RXFIFOISR);
    Interrupt_register(INT_SCIC_TX, &SCIC_DRV_TXFIFOISR);
    SCI_DRV_vInitFIFO(
            SCI_DRV_u32GetBaseFromModuleIndex(FPGA_SCI_DRV_UART_MODULE), FPGA_SCI_DRV_UART_BAUD,
            (SCI_CONFIG_WLEN_8 | FPGA_SCI_DRV_UART_STOP_BITS | FPGA_SCI_DRV_UART_PARITY | FPGA_SCI_DRV_UART_ADDRESS_MODE));
    SCI_disableInterrupt(SCI_DRV_u32GetBaseFromModuleIndex(FPGA_SCI_DRV_UART_MODULE), SCI_INT_TXFF);
    UART_DRV_vInit((UART_DRV_eModule_t)FPGA_SCI_DRV_UART_MODULE);
    FPGA_SCI_DRV_vSetupFreeTimerTicksPerMicroSecond(u16FreeRunningTimerTicksPerMicroSecond);
    Interrupt_enable(INT_SCIC_RX);
    Interrupt_enable(INT_SCIC_TX);






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
    // CSMON Parameter, Recorder, Scope Test Initialization
    //
    ParameterInitialization();
    RecordersInitialization();
    ScopesInitialization();


    //
    // CSMON Internal Recorders Setup with Already Made Configuration
    //
    CSMON_vSetSetupRecorderParameterMask(CSMON_MASK_RECORDERS_012);


    //
    // Register Function Call In CSMON Timer Period ISR (default Timer Period is 50 usec)
    // For Debug and Control Process Emulation here is registered the ControlProcess Function
    //
    eResponseCode_CSMON_eSetTimerPeriodISRFunctionRegister = CSMON_eSetTimerPeriodISRFunctionRegister(ControlProcess);
    ASSERT(eResponseCode_CSMON_eSetTimerPeriodISRFunctionRegister != CSMON_RESPONSE_CODE_OK);
    // Check CSMON Response Code (... or Embed Assert For Debug) if needed

    //
    // Reset the WatchDog counter
    //
    SysCtl_serviceWatchdog();



    //
    // Set the WatchDogPrescaler - Default SYSCTL_WD_PRESCALE_1
    //
    //SysCtl_setWatchdogPrescaler(SYSCTL_WD_PRESCALE_1);        /*  1 * 512 * 256 @ 10Mhz -> ~ 13ms */
    //SysCtl_setWatchdogPrescaler(SYSCTL_WD_PRESCALE_2);        /*  2 * 512 * 256 @ 10Mhz -> ~ 26ms */
    //SysCtl_setWatchdogPrescaler(SYSCTL_WD_PRESCALE_4);        /*  4 * 512 * 256 @ 10Mhz -> ~ 52ms */
    //SysCtl_setWatchdogPrescaler(SYSCTL_WD_PRESCALE_8);        /*  8 * 512 * 256 @ 10Mhz -> ~104ms */
    //SysCtl_setWatchdogPrescaler(SYSCTL_WD_PRESCALE_16);       /* 16 * 512 * 256 @ 10Mhz -> ~208ms */
    //SysCtl_setWatchdogPrescaler(SYSCTL_WD_PRESCALE_32);       /* 32 * 512 * 256 @ 10Mhz -> ~416ms */
    //SysCtl_setWatchdogPrescaler(SYSCTL_WD_PRESCALE_64);       /* 64 * 512 * 256 @ 10Mhz -> ~832ms */

    #if CSMON_AUTOMATIC_SERVICE_WATCHDOG_IN_MAIN_LOOP == 0

    SysCtl_setWatchdogPrescaler(SYSCTL_WD_PRESCALE_8);        /*  8 * 512 * 256 @ 10Mhz -> ~104ms */
    u16WatchdogPrescalerOld = u16WatchdogPrescaler = SYSCTL_WD_PRESCALE_8;
    //
    // CSMON_eSetAutoServiceWatchdogInternalSlowCalculationsInMainLoop
    //
    //(void)CSMON_eSetAutoServiceWatchdogInternalSlowCalculationsInMainLoop(false);  /* Not Needed - Default false */


    #else

    SysCtl_setWatchdogPrescaler(SYSCTL_WD_PRESCALE_1);        /*  1 * 512 * 256 @ 10Mhz -> ~ 13ms */
    u16WatchdogPrescalerOld = u16WatchdogPrescaler = SYSCTL_WD_PRESCALE_1;
    //
    // CSMON_eSetAutoServiceWatchdogInternalSlowCalculationsInMainLoop
    //
    (void)CSMON_eSetAutoServiceWatchdogInternalSlowCalculationsInMainLoop(true);

    #endif


    //
    // Enable the WatchDog
    //
    SysCtl_serviceWatchdog();
    SysCtl_enableWatchdog();


    //
    // Enable Global Interrupt (INTM) and RealTime interrupt (DBGM)
    //
    EINT;
    ERTM;

    for (;;)
    {
        //
        // Reset the WatchDog counter
        //
        SysCtl_serviceWatchdog();

        //
        // Manage WatchDog Prescaler
        //
        if (u16WatchdogPrescaler != u16WatchdogPrescalerOld)
        {
            //SysCtl_disableWatchdog();
            //SysCtl_serviceWatchdog();
            u16WatchdogPrescalerOld = u16WatchdogPrescaler;
            SysCtl_setWatchdogPrescaler((SysCtl_WDPrescaler)u16WatchdogPrescaler);
            //SysCtl_serviceWatchdog();
            //SysCtl_enableWatchdog();
        }

        if (u16PeriodControlOld_usec != u16PeriodControl_usec)
        {
            u16PeriodControlOld_usec = u16PeriodControl_usec;
            CSMON_eSetInternalTimerPeriod(u16PeriodControl_usec);
        }


        //
        // Artificial Delay Main Loop
        //
        SysCtl_delay(u32DelayMainLoop_Ticks);


        //
        // Artificial Delay Setup
        //
        if (u16DelayCtrlLoop_100nsec != u16DelayCtrlLoopOld_100nsec)
        {
            u16DelayCtrlLoopOld_100nsec = u16DelayCtrlLoop_100nsec;
            u32DelayCtrlLoop_Ticks = (uint32_t)(((((long double)(u16DelayCtrlLoop_100nsec) * 0.1L) / (1000000.0L / (long double)DEVICE_SYSCLK_FREQ)) - 9.0L) / 5.0L);
            if (u32DelayCtrlLoop_Ticks == 0)
            {
                u32DelayCtrlLoop_Ticks = 1;
            }
        }
        if (u16DelayMainLoop_usec != u16DelayMainLoopOld_usec)
        {
            u16DelayMainLoopOld_usec = u16DelayMainLoop_usec;
            u32DelayMainLoop_Ticks = (uint32_t)(((((long double)(u16DelayMainLoop_usec)) / (1000000.0L / (long double)DEVICE_SYSCLK_FREQ)) - 9.0L) / 5.0L);
            if (u32DelayMainLoop_Ticks == 0)
            {
                u32DelayMainLoop_Ticks = 1;
            }
        }





        GPIO_writePin(STAT_LED_G_PIN, STAT_LED_ENABLE_LEVEL_LOW); /* Green LED (closest to the MCU Led) */
        //
        // CSMON Process In Main Loop - Big Delays On Disconnect 4-5ms; On Connect 12-35ms If Not Interrupted (EMIF Checksum PC Application)
        //
        eResponseCode_CSMON_eProcess = CSMON_eProcess();
        // Check CSMON Response Code if needed
        ASSERT(eResponseCode_CSMON_eProcess != CSMON_RESPONSE_CODE_OK);
        GPIO_writePin(STAT_LED_G_PIN, STAT_LED_DISABLE_LVL_HIGH); /* Green LED (closest to the MCU Led) */


        //
        // Device Running Control Indication - Set on Enter/Exit Run Mode
        //
        eResponseCode_CSMON_eSetServerOnStatus = CSMON_eSetServerOnStatus(bDummyStatsDevRunning);
        // Check CSMON Response Code if needed
        ASSERT(eResponseCode_CSMON_eSetServerOnStatus != CSMON_RESPONSE_CODE_OK);


        //
        // RTC Process To From FPGA
        //
        if (MAIN_bDateTimeSet)
        {
            FPGA_SCI_DRV_vSetDateTime (
                    MAIN_sDateTimeSet.u8Seconds,
                    MAIN_sDateTimeSet.u8Minutes,
                    MAIN_sDateTimeSet.u8Hours,
                    MAIN_sDateTimeSet.u8Weekdays,
                    MAIN_sDateTimeSet.u8Day,
                    MAIN_sDateTimeSet.u8Month,
                    MAIN_sDateTimeSet.u8Year);
            FPGA_SCI_DRV_vClockSetup();
            MAIN_bDateTimeSet = false;
        }

        FPGA_SCI_DRV_vGetDateTime (
                &MAIN_sDateTimeGet.u8Seconds,
                &MAIN_sDateTimeGet.u8Minutes,
                &MAIN_sDateTimeGet.u8Hours,
                &MAIN_sDateTimeGet.u8Weekdays,
                &MAIN_sDateTimeGet.u8Day,
                &MAIN_sDateTimeGet.u8Month,
                &MAIN_sDateTimeGet.u8Year);


        //
        // FPGA Communication Process
        //
        SCI_DRV_vErrorResetProcess(SCI_DRV_u32GetBaseFromModuleIndex((SCI_DRV_eModule_t)FPGA_SCI_DRV_UART_MODULE));
        FPGA_SCI_DRV_vProcessUartRx();
        FPGA_SCI_DRV_vProcess();


    }

}



