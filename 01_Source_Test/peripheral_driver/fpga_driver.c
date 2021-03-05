/* *****************************************************************************
 * File:   fpga_driver.c
 * Author: Dimitar Lilov
 *
 * Created on 2021 03 04 16:52
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
#include <stdint.h>
//#include "hw_types.h"   /* uint8_t  defined here but not always */
#include "device.h"
#include "fpga_driver.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */






/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */
typedef enum
{
    ADC_SENSE_0V9,
    ADC_SENSE_1V1,
    ADC_SENSE_1V2,
    ADC_SENSE_1V8,
    ADC_SENSE_2V5,
    ADC_SENSE_3V3,
    ADC_SENSE_CNT,

}eADCSense_t;

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
typedef struct
{
    uint16_t u8Second   : 8;
    uint16_t u8Minute   : 8;
    uint16_t u8Hour     : 8;
    uint16_t u8WeekDay  : 8;
    uint16_t u8Day      : 8;
    uint16_t u8Month    : 8;
    uint16_t u8Year     : 8;
    uint16_t u8Control  : 8;
    uint16_t u16Sense_mV[ADC_SENSE_CNT];
} FPGA_sData_t; /* Do Not Use This To Read/Write From/To FPGA */

typedef struct
{
    uint16_t u16SecMin      ;
    uint16_t u16HourWeekDay ;
    uint16_t u16DayMonth    ;
    uint16_t u16YearControl ;
    uint16_t u16Sense_mV[ADC_SENSE_CNT];
} FPGA_sDataRead_t;

typedef struct
{
    uint16_t u16SecMin      ;
    uint16_t u16HourWeekDay ;
    uint16_t u16DayMonth    ;
    uint16_t u16YearControl ;
} FPGA_sDataWrite_t;

typedef union
{
    FPGA_sDataWrite_t sWrite;
    FPGA_sDataRead_t  sRead ;
} FPGA_uData_t;

typedef union
{
    FPGA_sData_t      sTemp ;   /* Do Not Use This To Read/Write From/To FPGA */
    FPGA_sDataWrite_t sWrite;
    FPGA_sDataRead_t  sRead ;
} FPGA_uDataTemp_t;             /* Use Only For Local Variables */

/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */

#pragma DATA_SECTION(FPGA_uData,".fpgaram");
FPGA_uData_t FPGA_uData;

FPGA_uDataTemp_t FPGA_uDataLocal;

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */
void FPGA_DRV_vSetDateTime (
        uint_least8_t u8BCDSeconds,
        uint_least8_t u8BCDMinutes,
        uint_least8_t u8BCDHours,
        uint_least8_t u8BCDWeekdays,
        uint_least8_t u8BCDDay,
        uint_least8_t u8BCDMonth,
        uint_least8_t u8BCDYear)
{
    FPGA_uDataTemp_t FPGA_uDataTemp;

    FPGA_uDataTemp.sTemp.u8Second = u8BCDSeconds;
    FPGA_uDataTemp.sTemp.u8Minute = u8BCDMinutes;
    FPGA_uDataTemp.sTemp.u8Hour   = u8BCDHours;
    FPGA_uDataTemp.sTemp.u8WeekDay= u8BCDWeekdays;
    FPGA_uDataTemp.sTemp.u8Day    = u8BCDDay;
    FPGA_uDataTemp.sTemp.u8Month  = u8BCDMonth;
    FPGA_uDataTemp.sTemp.u8Year   = u8BCDYear;
    FPGA_uDataTemp.sTemp.u8Control= 0x00;


    /* Not Obligatory - First Write Seconds and Minutes @ address 0x00300000 */
    FPGA_uData.sWrite.u16SecMin         = FPGA_uDataTemp.sWrite.u16SecMin;
    FPGA_uData.sWrite.u16HourWeekDay    = FPGA_uDataTemp.sWrite.u16HourWeekDay;
    FPGA_uData.sWrite.u16DayMonth       = FPGA_uDataTemp.sWrite.u16DayMonth;
    FPGA_uData.sWrite.u16YearControl    = FPGA_uDataTemp.sWrite.u16YearControl;
    /* Obligatory !!! - Last Write Year and Control @ address 0x00300003 */
}

void FPGA_DRV_vGetData (void)
{
    uint16_t u16Index;

    /* Obligatory !!! - First Read Seconds and Minutes @ address 0x00300000 */
    FPGA_uDataLocal.sRead.u16SecMin         = FPGA_uData.sRead.u16SecMin;
    FPGA_uDataLocal.sRead.u16HourWeekDay    = FPGA_uData.sRead.u16HourWeekDay;
    FPGA_uDataLocal.sRead.u16DayMonth       = FPGA_uData.sRead.u16DayMonth;
    FPGA_uDataLocal.sRead.u16YearControl    = FPGA_uData.sRead.u16YearControl;
    for (u16Index = 0; u16Index < ADC_SENSE_CNT; u16Index++)
    {
        FPGA_uDataLocal.sRead.u16Sense_mV[u16Index]    = FPGA_uData.sRead.u16Sense_mV[u16Index];
    }
}


void FPGA_DRV_vGetDateTime (
        uint_least8_t* pu8BCDSeconds,
        uint_least8_t* pu8BCDMinutes,
        uint_least8_t* pu8BCDHours,
        uint_least8_t* pu8BCDWeekdays,
        uint_least8_t* pu8BCDDay,
        uint_least8_t* pu8BCDMonth,
        uint_least8_t* pu8BCDYear)
{
    FPGA_DRV_vGetData();

    *pu8BCDSeconds  = FPGA_uDataLocal.sTemp.u8Second;
    *pu8BCDMinutes  = FPGA_uDataLocal.sTemp.u8Minute;
    *pu8BCDHours    = FPGA_uDataLocal.sTemp.u8Hour;
    *pu8BCDWeekdays = FPGA_uDataLocal.sTemp.u8WeekDay;
    *pu8BCDDay      = FPGA_uDataLocal.sTemp.u8Day;
    *pu8BCDMonth    = FPGA_uDataLocal.sTemp.u8Month;
    *pu8BCDYear     = FPGA_uDataLocal.sTemp.u8Year;
}
