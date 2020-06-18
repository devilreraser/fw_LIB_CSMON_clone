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
#define CSMON_PAR_LIST_MINIMUM_COUNT                                    3

//#define CSMON_PARAMETER_LIST_TEST   CSMON_PAR_LIST_EACH_TYPE_REPEATED_ALL_TYPES_COUNT_TIMES
#define CSMON_PARAMETER_LIST_TEST   CSMON_PAR_LIST_ALL_TYPES_REPEATED_ALL_TYPES_COUNT_TIMES
//#define CSMON_PARAMETER_LIST_TEST   CSMON_PAR_LIST_MAXIMUM_COUNT
//#define CSMON_PARAMETER_LIST_TEST   CSMON_PAR_LIST_MINIMUM_COUNT



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
#define INIT_MAX_MIN_DEF(_type, max, min, def_) \
        .u32Max._type = (max), .u32Min._type = (min), .u32Def._type = (def_)

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
#define PARAM_ID_MAIN_LOOP_TIME    30006
#define PARAM_ID_CSMON_PRC_TIME    30007
#define PARAM_ID_RESET_MSR_TIME    30008
#define PARAM_ID_CSMON_ISR_TIME    30009
#define PARAM_ID_CTRL_ISR_TIME     30010
#define PARAM_ID_PARAMLOAD_TIME    30011



volatile const MAIN_sParameterList_t asParameterList[PARAMETER_COUNT_MAX] =
{
/* u16ParameterIndexID;                 u32RealAddress;           u16ParamAttributes;     pu8Name;            pu8Unit;            u32Max;                 u32Min;              u32Def;             Norm; */
/*                                                                   Type Access Pass                                                                                                                    */



#if CSMON_PARAMETER_LIST_TEST == CSMON_PAR_LIST_MINIMUM_COUNT




 {1000                   , (uint32_t)&s16DummyCurrentPhaseA   ,  PAR(_UINT16,_RW,_NO)  , {"CurrentPhA"}          ,    {""}      ,    (uint32_t)(65535)  ,   (uint32_t)(     0)  ,    (uint32_t)(999)    ,   1.0 },
// {1001                   , (uint32_t)&s16DummyCurrentPhaseB   ,  PAR(_UINT16,_RW,_NO)  , {"CurrentPhB"}          ,    {""}      ,    (uint32_t)(65535)  ,   (uint32_t)(     0)  ,    (uint32_t)(999)    ,   1.0 },
// {1002                   , (uint32_t)&s16DummyCurrentPhaseC   ,  PAR(_UINT16,_RW,_NO)  , {"CurrentPhC"}          ,    {""}      ,    (uint32_t)(65535)  ,   (uint32_t)(     0)  ,    (uint32_t)(999)    ,   1.0 },


#elif CSMON_PARAMETER_LIST_TEST == CSMON_PAR_LIST_MAXIMUM_COUNT



 {PARAM_ID_CTRL_PERIOD_US, (uint32_t)&u16PeriodControl_usec   ,  PAR(_UINT16,_RW,_NO)  , {"CtrlPeriod"}    ,    {"usec"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   1.0 },
 {PARAM_ID_CTRL_LOOP_NSEC, (uint32_t)&u16DelayCtrlLoop_100nsec,  PAR(_UINT16,_RW,_NO)  , {"Ctrl_Delay"}    ,    {"nsec"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              , 100.0 },
 {PARAM_ID_CTRL_LOOP_USEC, (uint32_t)&u16DelayCtrlLoop_100nsec,  PAR(_UINT16,_RW,_NO)  , {"Ctrl_Delay"}    ,    {"usec"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.1 },
 {PARAM_ID_MAIN_LOOP_USEC, (uint32_t)&u16DelayMainLoop_usec   ,  PAR(_UINT16,_RW,_NO)  , {"Main_Delay"}    ,    {"usec"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   1.0 },
 {PARAM_ID_PARAM_SET_FAIL, (uint32_t)&u16CountSetParameterFail,  PAR(_UINT16,_RW,_NO)  , {"Param_Fail"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   1.0 },

 {PARAM_ID_MAIN_LOOP_TIME, (uint32_t)&u32TimeMainLoopCycle_Max_Ticks
                                                              ,  PAR(_UINT32,_RO,_NO)  , {"MainLoopMax"}   ,    {"Ticks"}  ,   INIT_MAX_MIN_DEF(u32Register, 0xFFFFFFFF,        0,          0)              ,   1.0 },
 {PARAM_ID_CSMON_PRC_TIME, (uint32_t)&u32TimeMainLoopProcessCSMON_Max_Ticks
                                                              ,  PAR(_UINT32,_RO,_NO)  , {"CSMON_Main"}    ,    {"Ticks"}  ,   INIT_MAX_MIN_DEF(u32Register, 0xFFFFFFFF,        0,          0)              ,   1.0 },

 {PARAM_ID_RESET_MSR_TIME, (uint32_t)&bResetAllTimeMeasures   ,  PAR(_UINT08,_RW,_NO)  , {"Meas_Reset"}    ,    {"boolean"},   INIT_MAX_MIN_DEF( u8Register,     true,      false,      false)              ,     1 }, /* If Parameter ID 9 Read  Request From PC (can be with Different Real Address)*/

 {PARAM_ID_CSMON_ISR_TIME, (uint32_t)&u32TimeCSMON_ISR_Max_Ticks
                                                              ,  PAR(_UINT32,_RO,_NO)  , {"CSMON_ISR"}     ,    {"Ticks"}  ,   INIT_MAX_MIN_DEF(u32Register, 0xFFFFFFFF,        0,          0)              ,   1.0 },

 {PARAM_ID_CTRL_ISR_TIME , (uint32_t)&u32TimeCtrlLoopMax_Ticks
                                                              ,  PAR(_UINT32,_RO,_NO)  , {"ControlISR"}    ,    {"Ticks"}  ,   INIT_MAX_MIN_DEF(u32Register, 0xFFFFFFFF,        0,          0)              ,   1.0 },

 {PARAM_ID_PARAMLOAD_TIME, (uint32_t)&u32ParamTime_Ticks
                                                              ,  PAR(_UINT32,_RO,_NO)  , {"Param_Init"}   ,    {"Ticks"}  ,   INIT_MAX_MIN_DEF(u32Register, 0xFFFFFFFF,        0,          0)              ,   1.0 },



 {PARAM_ID_PARAM_MAX_TEST, (uint32_t)&u16CountMaxParameterTest,  PAR(_UINT16,_RW,_WR)  , {"Param_Test"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   1.0 },
 {PARAM_ID_RD_WR_SCALETST, (uint32_t)&s16ScaleDataTst         ,  PAR(_SINT16,_RW,_NO)  , {"RDWR_Scale"}    ,    {"\n"}     ,   INIT_MAX_MIN_DEF(s16Register,      300,       -100,        200)              ,   2.5 },
 {PARAM_ID_READ_WRITE_TST, (uint32_t)&s16DummyDataTst         ,  PAR(_SINT16,_RW,_NO)  , {"RD_WR_Test"}    ,    {" "}      ,   INIT_MAX_MIN_DEF(s16Register,      300,       -100,        200)              ,   0.0 },
 {PARAM_ID_MODBUS_MSG_CNT, (uint32_t)&u16DummyDataCnt         ,  PAR(_UINT16,_RW,_NO)  , {"Modbus_Cnt"}    ,    {""}       ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.0 },
 {PARAM_ID_CURRENT_PHASEA, (uint32_t)&s16DummyCurrentPhaseA   ,  PAR(_SINT16,_RW,_RD)  , {"CurrentPhA"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {PARAM_ID_CURRENT_PHASEB, (uint32_t)&s16DummyCurrentPhaseB   ,  PAR(_SINT16,_RW,_RD)  , {"CurrentPhB"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {PARAM_ID_CURRENT_PHASEC, (uint32_t)&s16DummyCurrentPhaseC   ,  PAR(_SINT16,_RW,_RD)  , {"CurrentPhC"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {PARAM_ID_VOLTAGE_DCLINK, (uint32_t)&s16DummyVoltageDCLink   ,  PAR(_SINT16,_RW,_RD)  , {"VoltageBus"}    ,    {"V"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,          0,        900)              ,   0.1 },
 {PARAM_ID_INCREMENT_LOOP, (uint32_t)&s16DummyIncrementLoop   ,  PAR(_SINT16,_RW,_WR)  , {"IncLoopTst"}    ,    {"A(0.5V)"},   INIT_MAX_MIN_DEF(s16Register,    10000,          0,          1)              ,   0.1 },
 {PARAM_ID_REQRUNNINGMODE, (uint32_t)&bDummyReqstDevRunning   ,  PAR(_UINT08,_WO,_WR)  , {"DevRunning"}    ,    {"boolean"},   INIT_MAX_MIN_DEF( u8Register,     true,      false,      false)              ,     1 }, /* If Parameter ID 9 Write Request From PC */
 {PARAM_ID_STARUNNINGMODE, (uint32_t)&bDummyStatsDevRunning   ,  PAR(_UINT08,_RO,_NO)  , {"DevRunning"}    ,    {"boolean"},   INIT_MAX_MIN_DEF( u8Register,     true,      false,      false)              ,     1 }, /* If Parameter ID 9 Read  Request From PC (can be with Different Real Address)*/



 {10000                  , (uint32_t)&u16TestData0            ,  PAR(_UINT16,_WO,_RD)  , {"Test_10000"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.00 },
 {10001                  , (uint32_t)&u16TestData1            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10001"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.01 },
 {10002                  , (uint32_t)&u16TestData2            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10002"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.02 },
 {10003                  , (uint32_t)&u16TestData3            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10003"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.03 },
 {10004                  , (uint32_t)&u16TestData4            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10004"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.04 },
 {10005                  , (uint32_t)&u16TestData5            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10005"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.05 },
 {10006                  , (uint32_t)&u16TestData6            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10006"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.06 },
 {10007                  , (uint32_t)&u16TestData7            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10007"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.07 },
 {10008                  , (uint32_t)&u16TestData8            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10008"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.08 },
 {10009                  , (uint32_t)&u16TestData9            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10009"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.09 },

 {1000                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1001"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1001                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1001"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1002                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1002"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.2 },
 {1003                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1003"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.3 },
 {1004                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1004"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.4 },
 {1005                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1005"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.5 },
 {1006                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1006"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.6 },
 {1007                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1007"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.7 },
 {1008                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1008"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.8 },
 {1009                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1009"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.9 },
 {1010                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1010"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   1.0 },
 {1011                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1011"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1012                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1012"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1013                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1013"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1014                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1014"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1015                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1015"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1016                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1016"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1017                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1017"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1018                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1018"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1019                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1019"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1020                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1020"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1021                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1021"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1022                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1022"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1023                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1023"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1024                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1024"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1025                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1025"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1026                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1026"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1027                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1027"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1028                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1028"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1029                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1029"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1030                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1030"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1031                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1031"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1032                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1032"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1033                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1033"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1034                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1034"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1035                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1035"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1036                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1036"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1037                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1037"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1038                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1038"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1039                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1039"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1040                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1040"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1041                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1041"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1042                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1042"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1043                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1043"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1044                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1044"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1045                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1045"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1046                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1046"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1047                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1047"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1048                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1048"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1049                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1049"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1050                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1050"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1051                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1051"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1052                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1052"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1053                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1053"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1054                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1054"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1055                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1055"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1056                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1056"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1057                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1057"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1058                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1058"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1059                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1059"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1060                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1060"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1061                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1061"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1062                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1062"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1063                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1063"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1064                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1064"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1065                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1065"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1066                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1066"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1067                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1067"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1068                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1068"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1069                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1069"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1070                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1070"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1071                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1071"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1072                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1072"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1073                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1073"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1074                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1074"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1075                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1075"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1076                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1076"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1077                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1077"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1078                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1078"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1079                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1079"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1080                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1080"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1081                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1081"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1082                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1082"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1083                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1083"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1084                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1084"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1085                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1085"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1086                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1086"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1087                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1087"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1088                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1088"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1089                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1089"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1090                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1090"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1091                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1091"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1092                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1092"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1093                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1093"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1094                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1094"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1095                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1095"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1096                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1096"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1097                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1097"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1098                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1098"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1099                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1099"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1100                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1100"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1101                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1101"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1102                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1102"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1103                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1103"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1104                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1104"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1105                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1105"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1106                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1106"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1107                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1107"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1108                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1108"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1109                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1109"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1110                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1110"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1111                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1111"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1112                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1112"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1113                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1113"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1114                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1114"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1115                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1115"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1116                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1116"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1117                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1117"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1118                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1118"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1119                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1119"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1120                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1120"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1121                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1121"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1122                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1122"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1123                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1123"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1124                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1124"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1125                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1125"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1126                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1126"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1127                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1127"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1128                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1128"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1129                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1129"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1130                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1130"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1131                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1131"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1132                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1132"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1133                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1133"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1134                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1134"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1135                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1135"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1136                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1136"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1137                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1137"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1138                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1138"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1139                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1139"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1140                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1140"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1141                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1141"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1142                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1142"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1143                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1143"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1144                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1144"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1145                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1145"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1146                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1146"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1147                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1147"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1148                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1148"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1149                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1149"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1150                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1150"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1151                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1151"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1152                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1152"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1153                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1153"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1154                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1154"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1155                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1155"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1156                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1156"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1157                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1157"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1158                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1158"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1159                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1159"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1160                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1160"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1161                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1161"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1162                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1162"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1163                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1163"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1164                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1164"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1165                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1165"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1166                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1166"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1167                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1167"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1168                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1168"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1169                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1169"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1170                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1170"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1171                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1171"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1172                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1172"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1173                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1173"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1174                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1174"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1175                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1175"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1176                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1176"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1177                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1177"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1178                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1178"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1179                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1179"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1180                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1180"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1181                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1181"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1182                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1182"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1183                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1183"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1184                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1184"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1185                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1185"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1186                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1186"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1187                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1187"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1188                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1188"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1189                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1189"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1190                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1190"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1191                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1191"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1192                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1192"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1193                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1193"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1194                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1194"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1195                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1195"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1196                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1196"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1197                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1197"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1198                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1198"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1199                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1199"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1200                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1200"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1201                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1201"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {1202                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1202"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1203                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1203"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1204                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1204"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1205                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1205"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1206                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1206"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1207                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1207"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1208                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1208"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1209                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1209"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1210                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1210"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1211                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1211"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1212                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1212"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1213                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1213"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1214                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1214"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1215                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1215"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1216                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1216"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1217                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1217"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1218                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1218"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1219                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1219"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1220                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1220"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1221                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1221"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1222                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1222"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1223                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1223"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1224                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1224"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1225                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1225"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1226                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1226"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1227                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1227"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1228                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1228"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1229                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1229"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1230                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1230"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1231                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1231"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1232                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1232"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1233                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1233"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1234                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1234"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1235                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1235"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1236                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1236"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1237                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1237"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1238                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1238"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1239                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1239"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1240                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1240"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1241                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1241"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1242                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1242"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1243                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1243"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1244                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1244"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1245                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1245"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1246                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1246"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1247                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1247"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1248                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1248"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1249                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1249"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1250                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1250"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1251                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1251"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1252                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1252"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1253                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1253"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1254                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1254"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1255                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1255"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1256                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1256"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1257                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1257"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1258                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1258"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1259                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1259"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1260                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1260"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1261                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1261"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1262                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1262"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1263                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1263"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1264                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1264"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1265                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1265"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1266                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1266"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1267                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1267"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1268                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1268"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1269                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1269"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1270                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1270"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1271                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1271"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1272                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1272"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1273                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1273"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1274                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1274"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1275                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1275"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1276                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1276"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1277                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1277"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1278                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1278"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1279                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1279"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1280                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1280"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1281                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1281"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1282                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1282"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1283                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1283"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1284                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1284"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1285                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1285"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1286                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1286"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1287                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1287"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1288                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1288"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1289                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1289"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1290                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1290"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1291                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1291"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1292                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1292"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1293                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1293"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1294                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1294"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1295                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1295"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1296                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1296"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1297                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1297"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1298                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1298"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1299                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1299"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1300                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1300"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1301                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1301"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1302                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1302"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1303                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1303"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1304                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1304"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1305                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1305"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1306                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1306"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1307                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1307"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1308                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1308"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1309                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1309"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1310                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1310"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1311                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1311"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1312                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1312"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1313                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1313"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1314                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1314"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1315                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1315"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1316                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1316"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1317                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1317"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1318                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1318"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1319                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1319"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1320                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1320"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1321                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1321"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1322                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1322"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1323                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1323"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1324                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1324"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1325                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1325"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1326                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1326"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1327                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1327"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1328                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1328"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1329                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1329"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1330                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1330"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1331                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1331"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1332                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1332"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1333                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1333"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1334                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1334"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1335                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1335"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1336                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1336"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1337                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1337"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1338                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1338"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1339                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1339"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1340                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1340"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1341                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1341"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1342                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1342"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1343                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1343"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1344                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1344"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1345                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1345"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1346                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1346"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1347                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1347"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1348                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1348"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1349                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1349"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1350                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1350"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1351                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1351"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1352                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1352"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1353                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1353"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1354                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1354"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1355                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1355"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1356                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1356"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1357                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1357"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1358                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1358"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1359                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1359"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1360                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1360"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1361                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1361"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1362                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1362"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1363                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1363"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1364                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1364"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1365                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1365"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1366                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1366"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1367                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1367"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1368                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1368"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1369                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1369"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1370                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1370"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1371                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1371"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1372                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1372"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1373                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1373"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1374                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1374"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1375                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1375"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1376                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1376"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1377                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1377"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1378                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1378"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1379                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1379"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1380                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1380"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1381                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1381"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1382                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1382"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1383                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1383"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1384                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1384"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1385                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1385"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1386                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1386"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1387                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1387"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1388                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1388"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1389                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1389"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1390                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1390"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1391                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1391"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1392                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1392"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1393                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1393"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1394                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1394"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1395                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1395"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1396                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1396"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1397                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1397"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1398                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1398"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1399                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1399"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1400                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1400"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1401                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1401"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1402                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1402"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1403                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1403"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1404                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1404"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1405                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1405"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1406                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1406"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1407                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1407"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1408                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1408"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1409                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1409"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1410                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1410"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1411                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1411"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1412                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1412"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1413                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1413"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1414                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1414"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1415                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1415"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1416                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1416"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1417                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1417"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1418                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1418"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1419                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1419"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1420                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1420"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1421                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1421"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1422                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1422"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1423                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1423"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1424                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1424"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1425                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1425"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1426                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1426"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1427                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1427"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1428                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1428"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1429                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1429"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1430                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1430"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1431                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1431"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1432                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1432"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1433                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1433"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1434                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1434"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1435                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1435"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1436                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1436"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1437                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1437"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1438                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1438"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1439                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1439"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1440                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1440"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1441                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1441"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1442                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1442"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1443                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1443"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1444                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1444"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0),   0.1 },
 {1445                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1445"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1446                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1446"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1447                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1447"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1448                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1448"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1449                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1449"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1450                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1450"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1451                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1451"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1452                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1452"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1453                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1453"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1454                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1454"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1455                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1455"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1456                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1456"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1457                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1457"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1458                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1458"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1459                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1459"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1460                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1460"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1461                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1461"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1462                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1462"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1463                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1463"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1464                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1464"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1465                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1465"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1466                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1466"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1467                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1467"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1468                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1468"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1469                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1469"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1470                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1470"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1471                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1471"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1472                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1472"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1473                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1473"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1474                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1474"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1475                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1475"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1476                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1476"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1477                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1477"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1478                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1478"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1479                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1479"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1480                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1480"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1481                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1481"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1482                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1482"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1483                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1483"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1484                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1484"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1485                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1485"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1486                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1486"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1487                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1487"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1488                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1488"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1489                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1489"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1490                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1490"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1491                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1491"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1492                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1492"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1493                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1493"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1494                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1494"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1495                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1495"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1496                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1496"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1497                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1497"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1498                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1498"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1499                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1499"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1500                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1500"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1501                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1501"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1502                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1502"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1503                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1503"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1504                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1504"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1505                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1505"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1506                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1506"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1507                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1507"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1508                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1508"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1509                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1509"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1510                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1510"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1511                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1511"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1512                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1512"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1513                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1513"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1514                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1514"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1515                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1515"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1516                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1516"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1517                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1517"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1518                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1518"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1519                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1519"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1520                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1520"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1521                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1521"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1522                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1522"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1523                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1523"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1524                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1524"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1525                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1525"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1526                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1526"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1527                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1527"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1528                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1528"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1529                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1529"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1530                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1530"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1531                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1531"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1532                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1532"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1533                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1533"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1534                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1534"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1535                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1535"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1536                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1536"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1537                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1537"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1538                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1538"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1539                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1539"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1540                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1540"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1541                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1541"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1542                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1542"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1543                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1543"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1544                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1544"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1545                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1545"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1546                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1546"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1547                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1547"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1548                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1548"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1549                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1549"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1550                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1550"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1551                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1551"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1552                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1552"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1553                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1553"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1554                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1554"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1555                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1555"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1556                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1556"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1557                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1557"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1558                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1558"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1559                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1559"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1560                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1560"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1561                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1561"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1562                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1562"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1563                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1563"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1564                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1564"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1565                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1565"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1566                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1566"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1567                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1567"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1568                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1568"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1569                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1569"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1570                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1570"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1571                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1571"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1572                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1572"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1573                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1573"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1574                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1574"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1575                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1575"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1576                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1576"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1577                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1577"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1578                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1578"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1579                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1579"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1580                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1580"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1581                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1581"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1582                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1582"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1583                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1583"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1584                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1584"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1585                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1585"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1586                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1586"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1587                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1587"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1588                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1588"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1589                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1589"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1590                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1590"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1591                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1591"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1592                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1592"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1593                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1593"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1594                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1594"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1595                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1595"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1596                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1596"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1597                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1597"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1598                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1598"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1599                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1599"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1600                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1600"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1601                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1601"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1602                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1602"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1603                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1603"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1604                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1604"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1605                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1605"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1606                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1606"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1607                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1607"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1608                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1608"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1609                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1609"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1610                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1610"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1611                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1611"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1612                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1612"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1613                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1613"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1614                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1614"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1615                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1615"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1616                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1616"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1617                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1617"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1618                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1618"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1619                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1619"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1620                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1620"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1621                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1621"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1622                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1622"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1623                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1623"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1624                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1624"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1625                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1625"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1626                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1626"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1627                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1627"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1628                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1628"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1629                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1629"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1630                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1630"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1631                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1631"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1632                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1632"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1633                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1633"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1634                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1634"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1635                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1635"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1636                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1636"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1637                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1637"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1638                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1638"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1639                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1639"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1640                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1640"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1641                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1641"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1642                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1642"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1643                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1643"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1644                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1644"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1645                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1645"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1646                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1646"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1647                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1647"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1648                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1648"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1649                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1649"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1650                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1650"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1651                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1651"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1652                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1652"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1653                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1653"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1654                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1654"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1655                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1655"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1656                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1656"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1657                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1657"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1658                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1658"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1659                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1659"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1660                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1660"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1661                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1661"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1662                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1662"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1663                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1663"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1664                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1664"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1665                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1665"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1666                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1666"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1667                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1667"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1668                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1668"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1669                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1669"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1670                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1670"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1671                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1671"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1672                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1672"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1673                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1673"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1674                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1674"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1675                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1675"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1676                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1676"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1677                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1677"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1678                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1678"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1679                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1679"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1680                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1680"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1681                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1681"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1682                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1682"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1683                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1683"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1684                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1684"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1685                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1685"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1686                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1686"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1687                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1687"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1688                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1688"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1689                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1689"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1690                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1690"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1691                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1691"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1692                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1692"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1693                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1693"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1694                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1694"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1695                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1695"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1696                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1696"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1697                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1697"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1698                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1698"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1699                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1699"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1700                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1700"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1701                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1701"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1702                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1702"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1703                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1703"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1704                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1704"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1705                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1705"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1706                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1706"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1707                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1707"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1708                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1708"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1709                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1709"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1710                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1710"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1711                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1711"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1712                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1712"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1713                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1713"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1714                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1714"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1715                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1715"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1716                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1716"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1717                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1717"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1718                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1718"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1719                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1719"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1720                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1720"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1721                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1721"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1722                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1722"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1723                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1723"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1724                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1724"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1725                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1725"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1726                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1726"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1727                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1727"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1728                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1728"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1729                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1729"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1730                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1730"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1731                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1731"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1732                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1732"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1733                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1733"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1734                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1734"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1735                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1735"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1736                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1736"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1737                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1737"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1738                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1738"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1739                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1739"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1740                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1740"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1741                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1741"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1742                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1742"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1743                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1743"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1744                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1744"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1745                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1745"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1746                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1746"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1747                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1747"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1748                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1748"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1749                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1749"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1750                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1750"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1751                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1751"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1752                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1752"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1753                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1753"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1754                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1754"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1755                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1755"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1756                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1756"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1757                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1757"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1758                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1758"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1759                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1759"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1760                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1760"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1761                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1761"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1762                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1762"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1763                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1763"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1764                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1764"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1765                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1765"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1766                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1766"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1767                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1767"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1768                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1768"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1769                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1769"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1770                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1770"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1771                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1771"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1772                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1772"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1773                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1773"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1774                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1774"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1775                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1775"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1776                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1776"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1777                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1777"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1778                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1778"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1779                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1779"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1780                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1780"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1781                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1781"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1782                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1782"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1783                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1783"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1784                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1784"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1785                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1785"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1786                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1786"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1787                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1787"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1788                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1788"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1789                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1789"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1790                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1790"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1791                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1791"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1792                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1792"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1793                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1793"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1794                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1794"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1795                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1795"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1796                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1796"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1797                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1797"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1798                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1798"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1799                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1799"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1800                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1800"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1801                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1801"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1802                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1802"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1803                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1803"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1804                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1804"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1805                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1805"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1806                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1806"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1807                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1807"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1808                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1808"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1809                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1809"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1810                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1810"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1811                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1811"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1812                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1812"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1813                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1813"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1814                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1814"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1815                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1815"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1816                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1816"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1817                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1817"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1818                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1818"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1819                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1819"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1820                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1820"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1821                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1821"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1822                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1822"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1823                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1823"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1824                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1824"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1825                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1825"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1826                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1826"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1827                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1827"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1828                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1828"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1829                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1829"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1830                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1830"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1831                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1831"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1832                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1832"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1833                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1833"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1834                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1834"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1835                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1835"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1836                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1836"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1837                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1837"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1838                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1838"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1839                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1839"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1840                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1840"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1841                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1841"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1842                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1842"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1843                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1843"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1844                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1844"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1845                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1845"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1846                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1846"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1847                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1847"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1848                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1848"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1849                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1849"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1850                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1850"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1851                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1851"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1852                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1852"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1853                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1853"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1854                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1854"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1855                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1855"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1856                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1856"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1857                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1857"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1858                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1858"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1859                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1859"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1860                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1860"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1861                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1861"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1862                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1862"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1863                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1863"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1864                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1864"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1865                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1865"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1866                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1866"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1867                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1867"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1868                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1868"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1869                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1869"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1870                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1870"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1871                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1871"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1872                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1872"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1873                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1873"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1874                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1874"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1875                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1875"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1876                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1876"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1877                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1877"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1878                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1878"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1879                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1879"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1880                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1880"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1881                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1881"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1882                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1882"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1883                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1883"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1884                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1884"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1885                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1885"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1886                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1886"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1887                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1887"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1888                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1888"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1889                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1889"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1890                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1890"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1891                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1891"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1892                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1892"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1893                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1893"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1894                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1894"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1895                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1895"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1896                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1896"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1897                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1897"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1898                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1898"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1899                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1899"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1900                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1900"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1901                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1901"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1902                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1902"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1903                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1903"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1904                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1904"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1905                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1905"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1906                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1906"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1907                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1907"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1908                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1908"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1909                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1909"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1910                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1910"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1911                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1911"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1912                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1912"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1913                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1913"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1914                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1914"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1915                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1915"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1916                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1916"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1917                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1917"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1918                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1918"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1919                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1919"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1920                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1920"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1921                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1921"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1922                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1922"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1923                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1923"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1924                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1924"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1925                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1925"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1926                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1926"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1927                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1927"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1928                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1928"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1929                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1929"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1930                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1930"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1931                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1931"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1932                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1932"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1933                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1933"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1934                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1934"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1935                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1935"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1936                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1936"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1937                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1937"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1938                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1938"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1939                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1939"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1940                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1940"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1941                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1941"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1942                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1942"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1943                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1943"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1944                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1944"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1945                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1945"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1946                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1946"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1947                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1947"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1948                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1948"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1949                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1949"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1950                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1950"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1951                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1951"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1952                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1952"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1953                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1953"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1954                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1954"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1955                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1955"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1956                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1956"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1957                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1957"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1958                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1958"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1959                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1959"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1960                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1960"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1961                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1961"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1962                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1962"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1963                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1963"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1964                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1964"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1965                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1965"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1966                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1966"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1967                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1967"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1968                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1968"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1969                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1969"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1970                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1970"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1971                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1971"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1972                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1972"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1973                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1973"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1974                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1974"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1975                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1975"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1976                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1976"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1977                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1977"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1978                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1978"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1979                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1979"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1980                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1980"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1981                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1981"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1982                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1982"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1983                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1983"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1984                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1984"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1985                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1985"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1986                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1986"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1987                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1987"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1988                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1988"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1989                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1989"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1990                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1990"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1991                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1991"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1992                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1992"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1993                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1993"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1994                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1994"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1995                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1995"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1996                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1996"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 {1997                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1997"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 //{1998                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1998"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },
 //{1999                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1999"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)   ,   0.1 },


#elif CSMON_PARAMETER_LIST_TEST == CSMON_PAR_LIST_EACH_TYPE_REPEATED_ALL_TYPES_COUNT_TIMES

 {PARAM_ID_CTRL_PERIOD_US, (uint32_t)&u16PeriodControl_usec   ,  PAR(_UINT16,_RW,_NO)  , {"CtrlPeriod"}    ,    {"usec"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   1.0 },
 {PARAM_ID_CTRL_LOOP_NSEC, (uint32_t)&u16DelayCtrlLoop_100nsec,  PAR(_UINT16,_RW,_NO)  , {"Ctrl_Delay"}    ,    {"nsec"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              , 100.0 },
 {PARAM_ID_CTRL_LOOP_USEC, (uint32_t)&u16DelayCtrlLoop_100nsec,  PAR(_UINT16,_RW,_NO)  , {"Ctrl_Delay"}    ,    {"usec"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.1 },
 {PARAM_ID_MAIN_LOOP_USEC, (uint32_t)&u16DelayMainLoop_usec   ,  PAR(_UINT16,_RW,_NO)  , {"Main_Delay"}    ,    {"usec"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   1.0 },
 {PARAM_ID_PARAM_SET_FAIL, (uint32_t)&u16CountSetParameterFail,  PAR(_UINT16,_RW,_NO)  , {"Param_Fail"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   1.0 },

 {PARAM_ID_MAIN_LOOP_TIME, (uint32_t)&u32TimeMainLoopCycle_Max_Ticks
                                                              ,  PAR(_UINT32,_RO,_NO)  , {"MainLoopMax"}   ,    {"Ticks"}  ,   INIT_MAX_MIN_DEF(u32Register, 0xFFFFFFFF,        0,          0)              ,   1.0 },
 {PARAM_ID_CSMON_PRC_TIME, (uint32_t)&u32TimeMainLoopProcessCSMON_Max_Ticks
                                                              ,  PAR(_UINT32,_RO,_NO)  , {"CSMON_Main"}    ,    {"Ticks"}  ,   INIT_MAX_MIN_DEF(u32Register, 0xFFFFFFFF,        0,          0)              ,   1.0 },

 {PARAM_ID_RESET_MSR_TIME, (uint32_t)&bResetAllTimeMeasures   ,  PAR(_UINT08,_RW,_NO)  , {"Meas_Reset"}    ,    {"boolean"},   INIT_MAX_MIN_DEF( u8Register,     true,      false,      false)              ,     1 }, /* If Parameter ID 9 Read  Request From PC (can be with Different Real Address)*/

 {PARAM_ID_CSMON_ISR_TIME, (uint32_t)&u32TimeCSMON_ISR_Max_Ticks
                                                              ,  PAR(_UINT32,_RO,_NO)  , {"CSMON_ISR"}     ,    {"Ticks"}  ,   INIT_MAX_MIN_DEF(u32Register, 0xFFFFFFFF,        0,          0)              ,   1.0 },

 {PARAM_ID_CTRL_ISR_TIME , (uint32_t)&u32TimeCtrlLoopMax_Ticks
                                                              ,  PAR(_UINT32,_RO,_NO)  , {"ControlISR"}    ,    {"Ticks"}  ,   INIT_MAX_MIN_DEF(u32Register, 0xFFFFFFFF,        0,          0)              ,   1.0 },

 {PARAM_ID_PARAMLOAD_TIME, (uint32_t)&u32ParamTime_Ticks
                                                              ,  PAR(_UINT32,_RO,_NO)  , {"Param_Init"}   ,    {"Ticks"}  ,   INIT_MAX_MIN_DEF(u32Register, 0xFFFFFFFF,        0,          0)              ,   1.0 },



 {PARAM_ID_PARAM_MAX_TEST, (uint32_t)&u16CountMaxParameterTest,  PAR(_UINT16,_RW,_WR)  , {"Param_Test"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   1.0 },
 {PARAM_ID_RD_WR_SCALETST, (uint32_t)&s16ScaleDataTst         ,  PAR(_SINT16,_RW,_NO)  , {"RDWR_Scale"}    ,    {"\n"}     ,   INIT_MAX_MIN_DEF(s16Register,      300,       -100,        200)              ,   2.5 },
 {PARAM_ID_READ_WRITE_TST, (uint32_t)&s16DummyDataTst         ,  PAR(_SINT16,_RW,_NO)  , {"RD_WR_Test"}    ,    {" "}      ,   INIT_MAX_MIN_DEF(s16Register,      300,       -100,        200)              ,   0.0 },
 {PARAM_ID_MODBUS_MSG_CNT, (uint32_t)&u16DummyDataCnt         ,  PAR(_UINT16,_RW,_NO)  , {"Modbus_Cnt"}    ,    {""}       ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.0 },
 {PARAM_ID_CURRENT_PHASEA, (uint32_t)&s16DummyCurrentPhaseA   ,  PAR(_SINT16,_RW,_RD)  , {"CurrentPhA"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {PARAM_ID_CURRENT_PHASEB, (uint32_t)&s16DummyCurrentPhaseB   ,  PAR(_SINT16,_RW,_RD)  , {"CurrentPhB"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {PARAM_ID_CURRENT_PHASEC, (uint32_t)&s16DummyCurrentPhaseC   ,  PAR(_SINT16,_RW,_RD)  , {"CurrentPhC"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {PARAM_ID_VOLTAGE_DCLINK, (uint32_t)&s16DummyVoltageDCLink   ,  PAR(_SINT16,_RW,_RD)  , {"VoltageBus"}    ,    {"V"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,          0,        900)              ,   0.1 },
 {PARAM_ID_INCREMENT_LOOP, (uint32_t)&s16DummyIncrementLoop   ,  PAR(_SINT16,_RW,_WR)  , {"IncLoopTst"}    ,    {"A(0.5V)"},   INIT_MAX_MIN_DEF(s16Register,    10000,          0,          1)              ,   0.1 },
 {PARAM_ID_REQRUNNINGMODE, (uint32_t)&bDummyReqstDevRunning   ,  PAR(_UINT08,_WO,_WR)  , {"DevRunning"}    ,    {"boolean"},   INIT_MAX_MIN_DEF( u8Register,     true,      false,      false)              ,     1 }, /* If Parameter ID 9 Write Request From PC */
 {PARAM_ID_STARUNNINGMODE, (uint32_t)&bDummyStatsDevRunning   ,  PAR(_UINT08,_RO,_NO)  , {"DevRunning"}    ,    {"boolean"},   INIT_MAX_MIN_DEF( u8Register,     true,      false,      false)              ,     1 }, /* If Parameter ID 9 Read  Request From PC (can be with Different Real Address)*/

 {10000                  , (uint32_t)&u16TestData0            ,  PAR(_UINT16,_WO,_RD)  , {"Test_10000"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.00 },
 {10001                  , (uint32_t)&u16TestData1            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10001"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.01 },
 {10002                  , (uint32_t)&u16TestData2            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10002"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.02 },
 {10003                  , (uint32_t)&u16TestData3            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10003"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.03 },
 {10004                  , (uint32_t)&u16TestData4            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10004"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.04 },
 {10005                  , (uint32_t)&u16TestData5            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10005"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.05 },
 {10006                  , (uint32_t)&u16TestData6            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10006"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.06 },
 {10007                  , (uint32_t)&u16TestData7            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10007"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.07 },
 {10008                  , (uint32_t)&u16TestData8            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10008"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.08 },
 {10009                  , (uint32_t)&u16TestData9            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10009"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.09 },

 {20000                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.0 },
 {20001                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.0 },
 {20002                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.0 },
 {20003                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.0 },
 {20004                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.0 },
 {20005                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.0 },

 {20010                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   1.0 },
 {20011                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   1.0 },
 {20012                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   1.0 },
 {20013                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   1.0 },
 {20014                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   1.0 },
 {20015                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   1.0 },

 {20020                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF(s32Register,  1000000,   -1000000,          0)              ,   0.0 },
 {20021                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF(s32Register,  1000000,   -1000000,          0)              ,   0.0 },
 {20022                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF(s32Register,  1000000,   -1000000,          0)              ,   0.0 },
 {20023                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF(s32Register,  1000000,   -1000000,          0)              ,   0.0 },
 {20024                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF(s32Register,  1000000,   -1000000,          0)              ,   0.0 },
 {20025                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF(s32Register,  1000000,   -1000000,          0)              ,   0.0 },

 {20030                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF(s32Register,  1000000,   -1000000,          0)              ,   1.0 },
 {20031                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF(s32Register,  1000000,   -1000000,          0)              ,   1.0 },
 {20032                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF(s32Register,  1000000,   -1000000,          0)              ,   1.0 },
 {20033                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF(s32Register,  1000000,   -1000000,          0)              ,   1.0 },
 {20034                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF(s32Register,  1000000,   -1000000,          0)              ,   1.0 },
 {20035                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF(s32Register,  1000000,   -1000000,          0)              ,   1.0 },

 {20040                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF( u8Register,      255,          0,          0)              ,   0.0 },
 {20041                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF( u8Register,      255,          0,          0)              ,   0.0 },
 {20042                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF( u8Register,      255,          0,          0)              ,   0.0 },
 {20043                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF( u8Register,      255,          0,          0)              ,   0.0 },
 {20044                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF( u8Register,      255,          0,          0)              ,   0.0 },
 {20045                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF( u8Register,      255,          0,          0)              ,   0.0 },

 {20050                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF( u8Register,      255,          0,          0)              ,   1.0 },
 {20051                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF( u8Register,      255,          0,          0)              ,   1.0 },
 {20052                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF( u8Register,      255,          0,          0)              ,   1.0 },
 {20053                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF( u8Register,      255,          0,          0)              ,   1.0 },
 {20054                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF( u8Register,      255,          0,          0)              ,   1.0 },
 {20055                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF( u8Register,      255,          0,          0)              ,   1.0 },


#elif CSMON_PARAMETER_LIST_TEST == CSMON_PAR_LIST_ALL_TYPES_REPEATED_ALL_TYPES_COUNT_TIMES


 {PARAM_ID_CTRL_PERIOD_US, (uint32_t)&u16PeriodControl_usec   ,  PAR(_UINT16,_RW,_NO)  , {"CtrlPeriod"}    ,    {"usec"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   1.0 },
 {PARAM_ID_CTRL_LOOP_NSEC, (uint32_t)&u16DelayCtrlLoop_100nsec,  PAR(_UINT16,_RW,_NO)  , {"Ctrl_Delay"}    ,    {"nsec"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              , 100.0 },
 {PARAM_ID_CTRL_LOOP_USEC, (uint32_t)&u16DelayCtrlLoop_100nsec,  PAR(_UINT16,_RW,_NO)  , {"Ctrl_Delay"}    ,    {"usec"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.1 },
 {PARAM_ID_MAIN_LOOP_USEC, (uint32_t)&u16DelayMainLoop_usec   ,  PAR(_UINT16,_RW,_NO)  , {"Main_Delay"}    ,    {"usec"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   1.0 },
 {PARAM_ID_PARAM_SET_FAIL, (uint32_t)&u16CountSetParameterFail,  PAR(_UINT16,_RW,_NO)  , {"Param_Fail"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   1.0 },

 {PARAM_ID_MAIN_LOOP_TIME, (uint32_t)&u32TimeMainLoopCycle_Max_Ticks
                                                              ,  PAR(_UINT32,_RO,_NO)  , {"MainLoopMax"}   ,    {"Ticks"}  ,   INIT_MAX_MIN_DEF(u32Register, 0xFFFFFFFF,        0,          0)              ,   1.0 },
 {PARAM_ID_CSMON_PRC_TIME, (uint32_t)&u32TimeMainLoopProcessCSMON_Max_Ticks
                                                              ,  PAR(_UINT32,_RO,_NO)  , {"CSMON_Main"}    ,    {"Ticks"}  ,   INIT_MAX_MIN_DEF(u32Register, 0xFFFFFFFF,        0,          0)              ,   1.0 },

 {PARAM_ID_RESET_MSR_TIME, (uint32_t)&bResetAllTimeMeasures   ,  PAR(_UINT08,_RW,_NO)  , {"Meas_Reset"}    ,    {"boolean"},   INIT_MAX_MIN_DEF( u8Register,     true,      false,      false)              ,     1 }, /* If Parameter ID 9 Read  Request From PC (can be with Different Real Address)*/

 {PARAM_ID_CSMON_ISR_TIME, (uint32_t)&u32TimeCSMON_ISR_Max_Ticks
                                                              ,  PAR(_UINT32,_RO,_NO)  , {"CSMON_ISR"}     ,    {"Ticks"}  ,   INIT_MAX_MIN_DEF(u32Register, 0xFFFFFFFF,        0,          0)              ,   1.0 },

 {PARAM_ID_CTRL_ISR_TIME , (uint32_t)&u32TimeCtrlLoopMax_Ticks
                                                              ,  PAR(_UINT32,_RO,_NO)  , {"ControlISR"}    ,    {"Ticks"}  ,   INIT_MAX_MIN_DEF(u32Register, 0xFFFFFFFF,        0,          0)              ,   1.0 },

 {PARAM_ID_PARAMLOAD_TIME, (uint32_t)&u32ParamTime_Ticks
                                                              ,  PAR(_UINT32,_RO,_NO)  , {"Param_Init"}   ,    {"Ticks"}  ,   INIT_MAX_MIN_DEF(u32Register, 0xFFFFFFFF,        0,          0)              ,   1.0 },



 {PARAM_ID_PARAM_MAX_TEST, (uint32_t)&u16CountMaxParameterTest,  PAR(_UINT16,_RW,_WR)  , {"Param_Test"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   1.0 },
 {PARAM_ID_RD_WR_SCALETST, (uint32_t)&s16ScaleDataTst         ,  PAR(_SINT16,_RW,_NO)  , {"RDWR_Scale"}    ,    {"\n"}     ,   INIT_MAX_MIN_DEF(s16Register,      300,       -100,        200)              ,   2.5 },
 {PARAM_ID_READ_WRITE_TST, (uint32_t)&s16DummyDataTst         ,  PAR(_SINT16,_RW,_NO)  , {"RD_WR_Test"}    ,    {" "}      ,   INIT_MAX_MIN_DEF(s16Register,      300,       -100,        200)              ,   0.0 },
 {PARAM_ID_MODBUS_MSG_CNT, (uint32_t)&u16DummyDataCnt         ,  PAR(_UINT16,_RW,_NO)  , {"Modbus_Cnt"}    ,    {""}       ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.0 },
 {PARAM_ID_CURRENT_PHASEA, (uint32_t)&s16DummyCurrentPhaseA   ,  PAR(_SINT16,_RW,_RD)  , {"CurrentPhA"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {PARAM_ID_CURRENT_PHASEB, (uint32_t)&s16DummyCurrentPhaseB   ,  PAR(_SINT16,_RW,_RD)  , {"CurrentPhB"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {PARAM_ID_CURRENT_PHASEC, (uint32_t)&s16DummyCurrentPhaseC   ,  PAR(_SINT16,_RW,_RD)  , {"CurrentPhC"}    ,    {"A"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.1 },
 {PARAM_ID_VOLTAGE_DCLINK, (uint32_t)&s16DummyVoltageDCLink   ,  PAR(_SINT16,_RW,_RD)  , {"VoltageBus"}    ,    {"V"}      ,   INIT_MAX_MIN_DEF(s16Register,    10000,          0,        900)              ,   0.1 },
 {PARAM_ID_INCREMENT_LOOP, (uint32_t)&s16DummyIncrementLoop   ,  PAR(_SINT16,_RW,_WR)  , {"IncLoopTst"}    ,    {"A(0.5V)"},   INIT_MAX_MIN_DEF(s16Register,    10000,          0,          1)              ,   0.1 },
 {PARAM_ID_REQRUNNINGMODE, (uint32_t)&bDummyReqstDevRunning   ,  PAR(_UINT08,_WO,_WR)  , {"DevRunning"}    ,    {"boolean"},   INIT_MAX_MIN_DEF( u8Register,     true,      false,      false)              ,     1 }, /* If Parameter ID 9 Write Request From PC */
 {PARAM_ID_STARUNNINGMODE, (uint32_t)&bDummyStatsDevRunning   ,  PAR(_UINT08,_RO,_NO)  , {"DevRunning"}    ,    {"boolean"},   INIT_MAX_MIN_DEF( u8Register,     true,      false,      false)              ,     1 }, /* If Parameter ID 9 Read  Request From PC (can be with Different Real Address)*/

 {10000                  , (uint32_t)&u16TestData0            ,  PAR(_UINT16,_WO,_RD)  , {"Test_10000"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.00 },
 {10001                  , (uint32_t)&u16TestData1            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10001"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.01 },
 {10002                  , (uint32_t)&u16TestData2            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10002"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.02 },
 {10003                  , (uint32_t)&u16TestData3            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10003"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.03 },
 {10004                  , (uint32_t)&u16TestData4            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10004"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.04 },
 {10005                  , (uint32_t)&u16TestData5            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10005"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.05 },
 {10006                  , (uint32_t)&u16TestData6            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10006"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.06 },
 {10007                  , (uint32_t)&u16TestData7            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10007"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.07 },
 {10008                  , (uint32_t)&u16TestData8            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10008"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.08 },
 {10009                  , (uint32_t)&u16TestData9            ,  PAR(_UINT16,_RW,_RD)  , {"Test_10009"}    ,    {"unit"}   ,   INIT_MAX_MIN_DEF(u16Register,    65535,          0,          0)              ,   0.09 },

 {20000                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.0 },
 {20001                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   1.0 },
 {20002                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF(s32Register,  1000000,   -1000000,          0)              ,   0.0 },
 {20003                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF(s32Register,  1000000,   -1000000,          0)              ,   1.0 },
 {20004                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF( u8Register,      255,          0,          0)              ,   0.0 },
 {20005                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF( u8Register,      255,          0,          0)              ,   1.0 },

 {20010                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.0 },
 {20011                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   1.0 },
 {20012                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF(s32Register,  1000000,   -1000000,          0)              ,   0.0 },
 {20013                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF(s32Register,  1000000,   -1000000,          0)              ,   1.0 },
 {20014                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF( u8Register,      255,          0,          0)              ,   0.0 },
 {20015                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF( u8Register,      255,          0,          0)              ,   1.0 },

 {20020                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.0 },
 {20021                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   1.0 },
 {20022                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF(s32Register,  1000000,   -1000000,          0)              ,   0.0 },
 {20023                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF(s32Register,  1000000,   -1000000,          0)              ,   1.0 },
 {20024                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF( u8Register,      255,          0,          0)              ,   0.0 },
 {20025                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF( u8Register,      255,          0,          0)              ,   1.0 },

 {20030                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.0 },
 {20031                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   1.0 },
 {20032                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF(s32Register,  1000000,   -1000000,          0)              ,   0.0 },
 {20033                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF(s32Register,  1000000,   -1000000,          0)              ,   1.0 },
 {20034                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF( u8Register,      255,          0,          0)              ,   0.0 },
 {20035                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF( u8Register,      255,          0,          0)              ,   1.0 },

 {20040                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.0 },
 {20041                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   1.0 },
 {20042                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF(s32Register,  1000000,   -1000000,          0)              ,   0.0 },
 {20043                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF(s32Register,  1000000,   -1000000,          0)              ,   1.0 },
 {20044                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF( u8Register,      255,          0,          0)              ,   0.0 },
 {20045                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF( u8Register,      255,          0,          0)              ,   1.0 },

 {20050                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   0.0 },
 {20051                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT16,_RW,_NO)  , {"Param_SINT16"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF(s16Register,    10000,     -10000,          0)              ,   1.0 },
 {20052                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF(s32Register,  1000000,   -1000000,          0)              ,   0.0 },
 {20053                  , (uint32_t)&aDummyDataTable         ,  PAR(_SINT32,_RW,_NO)  , {"Param_SINT32"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF(s32Register,  1000000,   -1000000,          0)              ,   1.0 },
 {20054                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"0.0"}    ,   INIT_MAX_MIN_DEF( u8Register,      255,          0,          0)              ,   0.0 },
 {20055                  , (uint32_t)&aDummyDataTable         ,  PAR(_UINT08,_RW,_NO)  , {"Param_UINT08"}  ,    {"1.0"}    ,   INIT_MAX_MIN_DEF( u8Register,      255,          0,          0)              ,   1.0 },


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


    u32TimeCtrlLoop_Ticks = 0 - (CPUTimer_getTimerCount(CPUTIMER1_BASE) - u32TimeCtrlLoop_Ticks);//down count
    if (u32TimeCtrlLoop_Ticks > u32TimeCtrlLoopMax_Ticks)
    {
        u32TimeCtrlLoopMax_Ticks = u32TimeCtrlLoop_Ticks;
    }
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
    u32ParamTime_Ticks = CPUTimer_getTimerCount(CPUTIMER1_BASE);
    ParameterInitialization();
    u32ParamTime_Ticks = 0 - (CPUTimer_getTimerCount(CPUTIMER1_BASE) - u32ParamTime_Ticks);//down count
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
        // Main Loop cycle Delay Measure
        //
        u32TimeMainLoopCycle_Bgn_Ticks = u32TimeMainLoopCycle_End_Ticks;
        u32TimeMainLoopCycle_End_Ticks = CPUTimer_getTimerCount(CPUTIMER1_BASE);
        u32TimeMainLoopCycle_Now_Ticks = 0 - (u32TimeMainLoopCycle_End_Ticks - u32TimeMainLoopCycle_Bgn_Ticks);//down count
        if (u32TimeMainLoopCycle_Now_Ticks > u32TimeMainLoopCycle_Max_Ticks)
        {
            u32TimeMainLoopCycle_Max_Ticks = u32TimeMainLoopCycle_Now_Ticks;
        }


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




        if (bResetAllTimeMeasures)
        {
            bResetAllTimeMeasures = 0;
            u32TimeMainLoopProcessCSMON_Now_Ticks = 0;
            u32TimeMainLoopProcessCSMON_Max_Ticks = 0;
            u32TimeMainLoopCycle_Now_Ticks = 0;
            u32TimeMainLoopCycle_Max_Ticks = 0;

            u32TimeCSMON_ISR_Max_Ticks = 0;
            u32TimeCtrlLoopMax_Ticks = 0;

        }




        //
        // CSMON Process In Main Loop Delay Measure
        //
        GPIO_writePin(STAT_LED_G_PIN, STAT_LED_ENABLE_LEVEL_LOW); /* Green LED (closest to the MCU Led) */
        u32TimeMainLoopProcessCSMON_Bgn_Ticks = CPUTimer_getTimerCount(CPUTIMER1_BASE);

        //
        // CSMON Process In Main Loop - Big Delays On Disconnect 4-5ms; On Connect 12-35ms If Not Interrupted (EMIF Checksum PC Application)
        //
        eResponseCode_CSMON_eProcess = CSMON_eProcess();
        // Check CSMON Response Code if needed
        ASSERT(eResponseCode_CSMON_eProcess != CSMON_RESPONSE_CODE_OK);

        //
        // CSMON Process In Main Loop Delay Measure
        //
        GPIO_writePin(STAT_LED_G_PIN, STAT_LED_DISABLE_LVL_HIGH); /* Green LED (closest to the MCU Led) */
        u32TimeMainLoopProcessCSMON_End_Ticks = CPUTimer_getTimerCount(CPUTIMER1_BASE);
        u32TimeMainLoopProcessCSMON_Now_Ticks = 0 - (u32TimeMainLoopProcessCSMON_End_Ticks - u32TimeMainLoopProcessCSMON_Bgn_Ticks);//down count
        if (u32TimeMainLoopProcessCSMON_Now_Ticks > u32TimeMainLoopProcessCSMON_Max_Ticks)
        {
            u32TimeMainLoopProcessCSMON_Max_Ticks = u32TimeMainLoopProcessCSMON_Now_Ticks;
        }

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



