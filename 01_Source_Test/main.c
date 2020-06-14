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
#define PARAMETER_COUNT_MAX        (819 + 1)/* Parameter 9 was independent RD and WR */
#define PARAMETER_ID_START_1000     0       /* Small and Big Numbers for ID (0 or 1) */


/* Some Pinout */
#ifdef _LAUNCHXL_F28379D
#define STAT_LED_G_PIN      28      /* N/A */
#define STAT_LED_A_B_PIN    31      /* D10 Blue */
#define STAT_LED_R_PIN      34      /* D9 */
#else
#define STAT_LED_G_PIN      28
#define STAT_LED_A_B_PIN    30      /* Amber */
#define STAT_LED_R_PIN      32
#endif

#define STAT_LED_ENABLE 0
#define STAT_LED_DISABLE   (!STAT_LED_ENABLE)

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
    uint32_t u32Max;
    uint32_t u32Min;
    uint32_t u32Def;
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


CSMON_eResponseCode_t eResponseCode_CSMON_eInit = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eProcess = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetServerOnStatus = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetParameter = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetRecorder = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetScope = CSMON_RESPONSE_CODE_OK;
CSMON_eResponseCode_t eResponseCode_CSMON_eSetTimerPeriodISRFunctionRegister = CSMON_RESPONSE_CODE_OK;
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


volatile const MAIN_sParameterList_t asParameterList[PARAMETER_COUNT_MAX] =
{
/* u16ParameterIndexID;                 u32RealAddress;           u16ParamAttributes;     pu8Name;            pu8Unit;            u32Max;                 u32Min;              u32Def;             Norm; */
/*                                                                   Type Access Pass                                                                                                                    */


#if 0

 {1000                   , (uint32_t)&s16DummyCurrentPhaseA   ,  PAR(_UINT16,_RW,_NO)  , {"CurrentPhA"}          ,    {""}      ,    (uint32_t)(65535)  ,   (uint32_t)(     0)  ,    (uint32_t)(999)    ,   1.0 },
// {1001                   , (uint32_t)&s16DummyCurrentPhaseB   ,  PAR(_UINT16,_RW,_NO)  , {"CurrentPhB"}          ,    {""}      ,    (uint32_t)(65535)  ,   (uint32_t)(     0)  ,    (uint32_t)(999)    ,   1.0 },
// {1002                   , (uint32_t)&s16DummyCurrentPhaseC   ,  PAR(_UINT16,_RW,_NO)  , {"CurrentPhC"}          ,    {""}      ,    (uint32_t)(65535)  ,   (uint32_t)(     0)  ,    (uint32_t)(999)    ,   1.0 },


#else

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



 {PARAM_ID_RD_WR_SCALETST, (uint32_t)&s16ScaleDataTst         ,  PAR(_SINT16,_RW,_NO)  , {"RDWR_Scale"}    ,    {"\n"}     ,   (uint32_t)(  300)  ,   (uint32_t)(  -100)  ,    (uint32_t)(200)    ,   2.5 },
 {PARAM_ID_READ_WRITE_TST, (uint32_t)&s16DummyDataTst         ,  PAR(_SINT16,_RW,_NO)  , {"RD_WR_Test"}    ,    {" "}      ,   (uint32_t)(  300)  ,   (uint32_t)(  -100)  ,    (uint32_t)(200)    ,   0.0 },
 {PARAM_ID_MODBUS_MSG_CNT, (uint32_t)&u16DummyDataCnt         ,  PAR(_UINT16,_RW,_NO)  , {"Modbus_Cnt"}    ,    {""}       ,   (uint32_t)(65535)  ,   (uint32_t)(     0)  ,    (uint32_t)(  0)    ,   0.0 },
 {PARAM_ID_CURRENT_PHASEA, (uint32_t)&s16DummyCurrentPhaseA   ,  PAR(_SINT16,_RW,_RD)  , {"CurrentPhA"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {PARAM_ID_CURRENT_PHASEB, (uint32_t)&s16DummyCurrentPhaseB   ,  PAR(_SINT16,_RW,_RD)  , {"CurrentPhB"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {PARAM_ID_CURRENT_PHASEC, (uint32_t)&s16DummyCurrentPhaseC   ,  PAR(_SINT16,_RW,_RD)  , {"CurrentPhC"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {PARAM_ID_VOLTAGE_DCLINK, (uint32_t)&s16DummyVoltageDCLink   ,  PAR(_SINT16,_RW,_RD)  , {"VoltageBus"}    ,    {"V"}      ,   (uint32_t)(13500)  ,   (uint32_t)(  5000)  ,    (uint32_t)(  0)    ,   0.1 },
 {PARAM_ID_INCREMENT_LOOP, (uint32_t)&s16DummyIncrementLoop   ,  PAR(_SINT16,_RW,_WR)  , {"IncLoopTst"}    ,    {"A(0.5V)"},   (uint32_t)( 1024)  ,   (uint32_t)( -1024)  ,    (uint32_t)(256)    ,   0.1 },
 {PARAM_ID_REQRUNNINGMODE, (uint32_t)&bDummyReqstDevRunning   ,  PAR(_UINT08,_WO,_WR)  , {"DevRunning"}    ,    {"boolean"},   (uint32_t)( true)  ,   (uint32_t)( false)  ,    (uint32_t)false    ,     1 }, /* If Parameter ID 9 Write Request From PC */
 {PARAM_ID_STARUNNINGMODE, (uint32_t)&bDummyStatsDevRunning   ,  PAR(_UINT08,_RO,_NO)  , {"DevRunning"}    ,    {"boolean"},   (uint32_t)( true)  ,   (uint32_t)( false)  ,    (uint32_t)false    ,     1 }, /* If Parameter ID 9 Read  Request From PC (can be with Different Real Address)*/

 {10000                  , (uint32_t)&u16TestData0           ,  PAR(_UINT16,_WO,_RD)  , {"Test_10000"}     ,    {"unit"}   ,   (uint32_t)(65535)  ,   (uint32_t)(     0)  ,    (uint32_t)(  0)    ,   0.00 },
 {10001                  , (uint32_t)&u16TestData1           ,  PAR(_UINT16,_RW,_RD)  , {"Test_10001"}     ,    {"unit"}   ,   (uint32_t)(65535)  ,   (uint32_t)(     0)  ,    (uint32_t)(  0)    ,   0.01 },
 {10002                  , (uint32_t)&u16TestData2           ,  PAR(_UINT16,_RW,_RD)  , {"Test_10002"}     ,    {"unit"}   ,   (uint32_t)(65535)  ,   (uint32_t)(     0)  ,    (uint32_t)(  0)    ,   0.02 },
 {10003                  , (uint32_t)&u16TestData3           ,  PAR(_UINT16,_RW,_RD)  , {"Test_10003"}     ,    {"unit"}   ,   (uint32_t)(65535)  ,   (uint32_t)(     0)  ,    (uint32_t)(  0)    ,   0.03 },
 {10004                  , (uint32_t)&u16TestData4           ,  PAR(_UINT16,_RW,_RD)  , {"Test_10004"}     ,    {"unit"}   ,   (uint32_t)(65535)  ,   (uint32_t)(     0)  ,    (uint32_t)(  0)    ,   0.04 },
 {10005                  , (uint32_t)&u16TestData5           ,  PAR(_UINT16,_RW,_RD)  , {"Test_10005"}     ,    {"unit"}   ,   (uint32_t)(65535)  ,   (uint32_t)(     0)  ,    (uint32_t)(  0)    ,   0.05 },
 {10006                  , (uint32_t)&u16TestData6           ,  PAR(_UINT16,_RW,_RD)  , {"Test_10006"}     ,    {"unit"}   ,   (uint32_t)(65535)  ,   (uint32_t)(     0)  ,    (uint32_t)(  0)    ,   0.06 },
 {10007                  , (uint32_t)&u16TestData7           ,  PAR(_UINT16,_RW,_RD)  , {"Test_10007"}     ,    {"unit"}   ,   (uint32_t)(65535)  ,   (uint32_t)(     0)  ,    (uint32_t)(  0)    ,   0.07 },
 {10008                  , (uint32_t)&u16TestData8           ,  PAR(_UINT16,_RW,_RD)  , {"Test_10008"}     ,    {"unit"}   ,   (uint32_t)(65535)  ,   (uint32_t)(     0)  ,    (uint32_t)(  0)    ,   0.08 },
 {10009                  , (uint32_t)&u16TestData9           ,  PAR(_UINT16,_RW,_RD)  , {"Test_10009"}     ,    {"unit"}   ,   (uint32_t)(65535)  ,   (uint32_t)(     0)  ,    (uint32_t)(  0)    ,   0.09 },

 {1000                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_WO,_RD)  , {"Param_1000"}    ,    {"WROnly"} ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.0 },
 {1001                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1001"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1002                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1002"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.2 },
 {1003                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1003"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.3 },
 {1004                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1004"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.4 },
 {1005                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1005"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.5 },
 {1006                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1006"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.6 },
 {1007                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1007"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.7 },
 {1008                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1008"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.8 },
 {1009                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1009"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.9 },
 {1010                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1010"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   1.0 },
 {1011                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1011"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1012                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1012"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1013                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1013"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1014                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1014"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1015                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1015"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1016                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1016"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1017                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1017"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1018                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1018"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1019                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1019"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1020                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1020"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1021                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1021"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1022                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1022"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1023                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1023"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1024                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1024"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1025                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1025"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1026                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1026"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1027                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1027"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1028                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1028"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1029                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1029"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1030                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1030"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1031                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1031"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1032                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1032"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1033                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1033"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1034                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1034"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1035                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1035"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1036                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1036"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1037                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1037"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1038                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1038"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1039                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1039"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1040                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1040"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1041                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1041"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1042                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1042"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1043                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1043"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1044                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1044"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1045                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1045"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1046                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1046"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1047                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1047"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1048                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1048"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1049                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1049"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1050                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1050"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1051                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1051"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1052                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1052"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1053                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1053"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1054                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1054"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1055                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1055"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1056                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1056"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1057                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1057"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1058                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1058"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1059                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1059"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1060                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1060"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1061                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1061"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1062                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1062"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1063                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1063"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1064                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1064"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1065                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1065"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1066                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1066"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1067                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1067"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1068                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1068"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1069                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1069"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1070                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1070"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1071                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1071"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1072                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1072"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1073                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1073"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1074                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1074"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1075                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1075"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1076                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1076"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1077                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1077"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1078                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1078"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1079                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1079"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1080                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1080"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1081                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1081"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1082                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1082"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1083                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1083"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1084                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1084"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1085                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1085"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1086                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1086"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1087                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1087"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1088                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1088"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1089                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1089"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1090                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1090"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1091                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1091"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1092                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1092"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1093                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1093"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1094                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1094"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1095                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1095"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1096                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1096"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1097                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1097"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1098                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1098"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1099                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1099"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1100                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1100"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1101                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1101"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1102                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1102"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1103                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1103"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1104                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1104"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1105                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1105"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1106                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1106"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1107                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1107"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1108                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1108"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1109                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1109"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1110                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1110"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1111                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1111"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1112                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1112"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1113                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1113"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1114                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1114"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1115                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1115"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1116                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1116"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1117                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1117"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1118                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1118"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1119                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1119"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1120                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1120"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1121                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1121"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1122                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1122"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1123                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1123"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1124                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1124"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1125                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1125"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1126                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1126"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1127                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1127"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1128                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1128"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1129                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1129"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1130                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1130"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1131                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1131"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1132                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1132"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1133                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1133"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1134                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1134"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1135                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1135"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1136                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1136"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1137                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1137"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1138                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1138"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1139                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1139"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1140                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1140"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1141                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1141"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1142                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1142"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1143                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1143"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1144                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1144"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1145                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1145"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1146                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1146"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1147                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1147"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1148                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1148"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1149                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1149"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1150                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1150"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1151                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1151"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1152                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1152"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1153                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1153"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1154                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1154"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1155                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1155"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1156                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1156"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1157                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1157"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1158                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1158"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1159                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1159"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1160                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1160"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1161                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1161"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1162                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1162"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1163                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1163"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1164                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1164"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1165                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1165"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1166                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1166"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1167                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1167"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1168                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1168"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1169                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1169"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1170                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1170"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1171                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1171"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1172                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1172"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1173                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1173"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1174                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1174"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1175                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1175"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1176                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1176"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1177                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1177"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1178                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1178"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1179                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1179"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1180                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1180"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1181                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1181"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1182                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1182"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1183                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1183"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1184                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1184"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1185                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1185"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1186                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1186"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1187                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1187"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1188                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1188"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1189                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1189"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1190                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1190"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1191                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1191"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1192                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1192"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1193                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1193"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1194                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1194"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1195                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1195"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1196                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1196"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1197                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1197"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1198                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1198"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1199                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1199"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1200                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1200"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1201                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1201"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1202                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1202"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1203                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1203"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1204                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1204"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1205                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1205"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1206                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1206"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1207                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1207"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1208                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1208"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1209                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1209"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1210                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1210"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1211                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1211"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1212                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1212"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1213                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1213"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1214                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1214"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1215                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1215"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1216                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1216"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1217                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1217"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1218                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1218"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1219                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1219"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1220                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1220"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1221                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1221"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1222                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1222"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1223                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1223"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1224                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1224"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1225                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1225"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1226                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1226"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1227                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1227"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1228                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1228"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1229                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1229"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1230                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1230"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1231                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1231"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1232                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1232"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1233                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1233"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1234                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1234"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1235                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1235"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1236                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1236"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1237                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1237"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1238                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1238"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1239                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1239"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1240                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1240"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1241                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1241"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1242                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1242"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1243                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1243"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1244                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1244"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1245                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1245"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1246                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1246"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1247                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1247"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1248                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1248"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1249                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1249"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1250                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1250"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1251                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1251"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1252                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1252"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1253                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1253"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1254                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1254"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1255                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1255"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1256                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1256"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1257                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1257"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1258                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1258"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1259                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1259"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1260                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1260"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1261                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1261"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1262                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1262"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1263                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1263"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1264                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1264"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1265                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1265"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1266                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1266"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1267                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1267"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1268                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1268"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1269                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1269"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1270                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1270"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1271                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1271"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1272                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1272"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1273                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1273"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1274                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1274"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1275                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1275"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1276                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1276"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1277                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1277"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1278                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1278"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1279                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1279"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1280                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1280"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1281                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1281"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1282                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1282"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1283                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1283"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1284                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1284"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1285                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1285"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1286                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1286"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1287                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1287"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1288                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1288"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1289                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1289"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1290                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1290"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1291                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1291"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1292                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1292"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1293                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1293"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1294                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1294"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1295                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1295"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1296                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1296"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1297                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1297"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1298                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1298"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1299                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1299"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1300                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1300"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1301                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1301"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1302                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1302"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1303                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1303"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1304                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1304"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1305                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1305"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1306                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1306"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1307                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1307"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1308                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1308"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1309                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1309"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1310                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1310"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1311                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1311"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1312                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1312"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1313                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1313"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1314                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1314"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1315                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1315"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1316                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1316"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1317                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1317"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1318                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1318"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1319                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1319"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1320                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1320"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1321                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1321"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1322                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1322"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1323                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1323"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1324                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1324"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1325                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1325"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1326                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1326"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1327                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1327"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1328                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1328"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1329                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1329"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1330                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1330"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1331                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1331"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1332                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1332"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1333                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1333"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1334                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1334"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1335                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1335"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1336                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1336"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1337                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1337"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1338                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1338"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1339                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1339"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1340                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1340"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1341                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1341"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1342                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1342"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1343                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1343"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1344                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1344"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1345                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1345"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1346                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1346"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1347                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1347"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1348                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1348"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1349                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1349"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1350                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1350"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1351                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1351"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1352                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1352"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1353                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1353"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1354                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1354"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1355                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1355"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1356                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1356"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1357                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1357"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1358                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1358"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1359                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1359"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1360                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1360"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1361                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1361"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1362                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1362"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1363                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1363"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1364                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1364"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1365                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1365"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1366                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1366"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1367                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1367"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1368                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1368"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1369                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1369"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1370                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1370"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1371                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1371"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1372                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1372"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1373                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1373"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1374                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1374"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1375                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1375"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1376                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1376"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1377                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1377"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1378                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1378"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1379                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1379"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1380                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1380"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1381                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1381"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1382                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1382"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1383                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1383"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1384                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1384"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1385                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1385"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1386                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1386"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1387                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1387"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1388                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1388"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1389                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1389"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1390                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1390"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1391                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1391"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1392                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1392"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1393                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1393"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1394                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1394"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1395                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1395"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1396                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1396"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1397                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1397"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1398                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1398"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1399                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1399"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1400                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1400"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1401                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1401"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1402                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1402"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1403                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1403"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1404                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1404"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1405                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1405"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1406                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1406"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1407                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1407"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1408                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1408"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1409                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1409"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1410                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1410"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1411                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1411"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1412                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1412"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1413                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1413"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1414                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1414"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1415                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1415"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1416                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1416"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1417                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1417"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1418                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1418"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1419                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1419"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1420                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1420"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1421                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1421"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1422                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1422"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1423                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1423"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1424                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1424"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1425                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1425"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1426                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1426"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1427                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1427"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1428                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1428"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1429                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1429"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1430                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1430"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1431                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1431"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1432                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1432"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1433                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1433"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1434                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1434"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1435                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1435"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1436                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1436"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1437                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1437"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1438                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1438"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1439                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1439"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1440                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1440"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1441                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1441"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1442                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1442"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1443                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1443"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1444                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1444"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1445                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1445"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1446                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1446"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1447                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1447"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1448                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1448"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1449                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1449"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1450                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1450"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1451                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1451"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1452                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1452"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1453                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1453"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1454                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1454"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1455                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1455"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1456                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1456"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1457                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1457"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1458                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1458"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1459                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1459"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1460                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1460"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1461                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1461"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1462                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1462"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1463                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1463"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1464                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1464"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1465                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1465"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1466                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1466"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1467                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1467"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1468                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1468"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1469                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1469"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1470                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1470"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1471                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1471"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1472                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1472"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1473                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1473"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1474                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1474"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1475                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1475"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1476                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1476"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1477                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1477"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1478                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1478"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1479                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1479"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1480                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1480"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1481                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1481"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1482                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1482"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1483                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1483"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1484                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1484"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1485                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1485"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1486                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1486"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1487                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1487"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1488                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1488"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1489                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1489"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1490                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1490"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1491                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1491"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1492                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1492"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1493                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1493"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1494                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1494"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1495                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1495"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1496                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1496"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1497                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1497"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1498                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1498"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1499                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1499"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1500                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1500"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1501                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1501"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1502                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1502"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1503                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1503"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1504                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1504"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1505                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1505"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1506                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1506"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1507                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1507"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1508                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1508"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1509                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1509"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1510                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1510"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1511                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1511"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1512                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1512"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1513                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1513"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1514                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1514"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1515                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1515"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1516                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1516"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1517                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1517"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1518                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1518"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1519                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1519"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1520                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1520"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1521                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1521"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1522                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1522"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1523                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1523"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1524                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1524"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1525                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1525"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1526                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1526"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1527                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1527"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1528                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1528"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1529                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1529"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1530                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1530"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1531                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1531"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1532                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1532"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1533                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1533"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1534                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1534"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1535                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1535"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1536                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1536"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1537                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1537"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1538                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1538"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1539                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1539"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1540                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1540"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1541                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1541"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1542                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1542"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1543                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1543"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1544                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1544"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1545                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1545"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1546                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1546"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1547                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1547"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1548                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1548"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1549                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1549"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1550                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1550"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1551                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1551"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1552                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1552"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1553                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1553"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1554                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1554"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1555                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1555"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1556                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1556"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1557                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1557"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1558                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1558"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1559                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1559"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1560                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1560"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1561                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1561"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1562                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1562"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1563                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1563"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1564                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1564"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1565                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1565"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1566                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1566"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1567                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1567"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1568                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1568"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1569                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1569"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1570                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1570"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1571                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1571"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1572                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1572"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1573                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1573"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1574                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1574"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1575                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1575"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1576                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1576"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1577                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1577"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1578                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1578"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1579                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1579"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1580                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1580"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1581                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1581"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1582                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1582"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1583                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1583"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1584                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1584"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1585                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1585"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1586                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1586"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1587                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1587"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1588                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1588"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1589                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1589"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1590                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1590"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1591                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1591"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1592                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1592"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1593                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1593"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1594                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1594"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1595                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1595"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1596                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1596"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1597                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1597"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1598                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1598"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1599                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1599"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1600                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1600"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1601                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1601"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1602                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1602"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1603                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1603"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1604                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1604"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1605                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1605"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1606                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1606"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1607                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1607"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1608                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1608"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1609                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1609"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1610                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1610"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1611                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1611"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1612                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1612"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1613                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1613"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1614                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1614"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1615                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1615"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1616                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1616"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1617                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1617"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1618                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1618"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1619                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1619"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1620                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1620"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1621                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1621"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1622                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1622"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1623                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1623"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1624                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1624"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1625                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1625"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1626                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1626"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1627                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1627"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1628                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1628"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1629                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1629"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1630                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1630"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1631                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1631"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1632                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1632"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1633                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1633"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1634                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1634"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1635                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1635"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1636                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1636"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1637                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1637"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1638                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1638"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1639                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1639"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1640                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1640"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1641                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1641"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1642                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1642"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1643                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1643"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1644                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1644"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1645                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1645"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1646                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1646"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1647                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1647"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1648                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1648"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1649                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1649"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1650                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1650"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1651                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1651"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1652                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1652"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1653                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1653"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1654                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1654"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1655                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1655"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1656                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1656"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1657                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1657"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1658                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1658"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1659                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1659"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1660                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1660"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1661                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1661"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1662                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1662"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1663                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1663"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1664                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1664"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1665                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1665"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1666                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1666"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1667                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1667"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1668                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1668"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1669                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1669"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1670                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1670"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1671                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1671"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1672                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1672"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1673                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1673"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1674                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1674"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1675                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1675"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1676                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1676"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1677                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1677"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1678                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1678"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1679                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1679"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1680                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1680"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1681                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1681"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1682                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1682"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1683                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1683"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1684                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1684"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1685                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1685"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1686                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1686"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1687                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1687"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1688                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1688"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1689                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1689"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1690                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1690"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1691                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1691"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1692                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1692"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1693                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1693"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1694                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1694"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1695                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1695"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1696                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1696"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1697                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1697"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1698                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1698"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1699                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1699"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1700                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1700"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1701                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1701"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1702                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1702"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1703                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1703"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1704                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1704"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1705                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1705"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1706                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1706"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1707                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1707"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1708                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1708"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1709                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1709"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1710                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1710"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1711                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1711"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1712                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1712"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1713                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1713"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1714                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1714"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1715                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1715"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1716                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1716"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1717                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1717"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1718                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1718"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1719                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1719"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1720                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1720"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1721                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1721"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1722                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1722"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1723                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1723"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1724                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1724"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1725                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1725"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1726                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1726"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1727                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1727"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1728                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1728"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1729                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1729"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1730                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1730"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1731                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1731"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1732                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1732"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1733                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1733"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1734                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1734"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1735                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1735"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1736                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1736"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1737                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1737"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1738                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1738"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1739                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1739"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1740                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1740"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1741                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1741"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1742                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1742"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1743                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1743"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1744                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1744"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1745                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1745"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1746                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1746"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1747                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1747"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1748                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1748"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1749                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1749"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1750                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1750"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1751                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1751"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1752                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1752"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1753                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1753"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1754                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1754"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1755                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1755"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1756                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1756"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1757                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1757"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1758                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1758"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1759                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1759"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1760                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1760"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1761                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1761"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1762                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1762"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1763                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1763"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1764                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1764"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1765                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1765"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1766                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1766"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1767                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1767"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1768                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1768"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1769                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1769"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1770                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1770"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1771                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1771"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1772                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1772"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1773                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1773"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1774                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1774"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1775                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1775"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1776                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1776"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1777                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1777"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1778                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1778"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1779                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1779"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1780                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1780"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1781                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1781"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1782                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1782"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1783                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1783"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1784                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1784"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1785                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1785"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1786                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1786"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1787                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1787"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1788                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1788"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1789                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1789"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1790                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1790"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1791                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1791"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1792                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1792"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1793                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1793"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1794                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1794"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1795                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1795"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1796                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1796"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1797                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1797"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1798                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1798"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
 {1799                   , (uint32_t)&s16DummyData            ,  PAR(_SINT16,_RW,_RD)  , {"Param_1799"}    ,    {"A"}      ,   (uint32_t)(10000)  ,   (uint32_t)(-10000)  ,    (uint32_t)(  0)    ,   0.1 },
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
    // Process Passed Flag Set - Need to be called from Processes with higher priority level in order CSMON to be able to get meaning-full (consistent) data
    eResponseCode_CSMON_eSetTimerPeriodISRFunctionRegister = CSMON_eSetFlagProcessPassed (CSMON_ID_PROCESS_CONTROL_PRIMARY);
    ASSERT(eResponseCode_CSMON_eSetTimerPeriodISRFunctionRegister != CSMON_RESPONSE_CODE_OK);
    // Check CSMON Response Code (... or Embed Assert For Debug) if needed
}


/* *****************************************************************************
 * ParameterInitialization
 **************************************************************************** */
void ParameterInitialization(void)
{
    uint16_t u16Index;

    uint32_t u32ParamRealAddress;

    uint32_t u32ParamVer;
    uint32_t u32DateTime;
    uint32_t u32CheckSum;

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


    if ( (uParamVerBackup.u32Register != u32ParamVer)
      || (uDateTimeBackup.u32Register != u32DateTime)
      || (uCheckSumBackup.u32Register != u32CheckSum) )                /* ParamVer or DateTime or Checksum MisMatch */
    {
        /* Invalid Table - Reset Parameter Table */
        CSMON_eResetParameterTable();                                   /* Reset Internal Used Parameters Count */

        /* Add Parameters */
        for (u16Index = 0; u16Index < PARAMETER_COUNT_MAX; u16Index++)
        {
            GPIO_writePin(STAT_LED_G_PIN, (u16Index & 1) );     /* Green LED (the closest to the MCU Led) */

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
                    asParameterList[u16Index].u32Max,
                    asParameterList[u16Index].u32Min,
                    asParameterList[u16Index].u32Def,
                    asParameterList[u16Index].Norm);
            ASSERT(eResponseCode_CSMON_eSetParameter != CSMON_RESPONSE_CODE_OK);
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


        GPIO_writePin(STAT_LED_G_PIN, STAT_LED_ENABLE);         /* Green LED (the closest to the MCU Led) */
        CSMON_eApplyParameterChanges();                         /* Internal Library Apply Written Parameters */
        GPIO_writePin(STAT_LED_G_PIN, STAT_LED_DISABLE);        /* Green LED (the closest to the MCU Led) */
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
    // STAT_LED_G_PIN is the LED STATUS pin.
    GPIO_setPinConfigOutput(STAT_LED_G_PIN);
    GPIO_writePin(STAT_LED_G_PIN, STAT_LED_ENABLE);
    GPIO_writePin(STAT_LED_G_PIN, STAT_LED_DISABLE);
    // STAT_LED_A_B_PIN is the LED STATUS pin.
    //
    GPIO_setPinConfigOutput(STAT_LED_A_B_PIN);
    GPIO_writePin(STAT_LED_A_B_PIN, STAT_LED_ENABLE);
    // STAT_LED_R_PIN is the LED STATUS pin.
    GPIO_setPinConfigOutput(STAT_LED_R_PIN);
    GPIO_writePin(STAT_LED_R_PIN, STAT_LED_ENABLE);

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






    GPIO_writePin(STAT_LED_G_PIN, STAT_LED_ENABLE);
    //
    // CSMON Initialization -> ~ 4.5ms
    //
    eResponseCode_CSMON_eInit = CSMON_eInit();
    // Check CSMON Response Code if needed
    if (eResponseCode_CSMON_eInit != CSMON_RESPONSE_CODE_OK)
    {
        /* If enters here - Fix Peripheral Frequency for Better Performance and Stability (DEVICE_LSPCLK_FREQ) */
        u32GetBaudError_PPM = CSMON_u32GetBaudError_PPM(CSMON_ID_PERIPHERAL_SCI_MODBUS);
        ASSERT(u32GetBaudError_PPM >= CSMON_u32PercentToPPM(3.0));
    }
    GPIO_writePin(STAT_LED_G_PIN, STAT_LED_DISABLE);


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
    // Set the WatchDogPrescaler
    //
    //SysCtl_setWatchdogPrescaler(SYSCTL_WD_PRESCALE_1);        /*  1 * 512 * 256 @ 10Mhz -> ~ 13ms */
    //SysCtl_setWatchdogPrescaler(SYSCTL_WD_PRESCALE_2);        /*  2 * 512 * 256 @ 10Mhz -> ~ 26ms */
    //SysCtl_setWatchdogPrescaler(SYSCTL_WD_PRESCALE_64);       /* 64 * 512 * 256 @ 10Mhz -> ~832ms */


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
        // Artificial Delay
        //
        //DEVICE_DELAY_US(1140);

        GPIO_writePin(STAT_LED_G_PIN, STAT_LED_ENABLE);
        //
        // CSMON Process In Main Loop
        //
        eResponseCode_CSMON_eProcess = CSMON_eProcess();
        // Check CSMON Response Code if needed
        ASSERT(eResponseCode_CSMON_eProcess != CSMON_RESPONSE_CODE_OK);
        GPIO_writePin(STAT_LED_G_PIN, STAT_LED_DISABLE);


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



