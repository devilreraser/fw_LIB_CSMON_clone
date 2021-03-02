/* *****************************************************************************
 * File:   ds3231mz.c
 * Author: Dimitar Lilov
 *
 * Created on 2021 02 23 15:03
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
#include "ds3231mz.h"

#include "device.h"
#include "i2c_driver.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define DS3231MZ_SLAVE_ADDRESS  0x68
#define DS3231MZ_REG_ADDR_SIZE  1

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */
// DS3231 Registers ------------------------
#define DS3231MZ_SECOND         0x00
#define DS3231MZ_MINUTE         0x01
#define DS3231MZ_HOUR           0x02
#define DS3231MZ_DAY            0x03
#define DS3231MZ_DATE           0x04
#define DS3231MZ_MONTH          0x05
#define DS3231MZ_YEAR           0x06
#define DS3231MZ_ALRM1_SECOND   0x07
#define DS3231MZ_ALRM1_MINUTE   0x08
#define DS3231MZ_ALRM1_HOUR     0x09
#define DS3231MZ_ALRM1_DAY_DATE 0x0A
#define DS3231MZ_ALRM2_MINUTE   0x0B
#define DS3231MZ_ALRM2_HOUR     0x0C
#define DS3231MZ_ALRM2_DAY_DATE 0x0D
#define DS3231MZ_CTRL           0x0E
#define DS3231MZ_CTRL_STATS     0x0F
#define DS3231MZ_AGING_OFFS     0x10
#define DS3231MZ_TEMP_MSB       0x11
#define DS3231MZ_TEMP_LSB       0x12

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
uint_least8_t au8ReadBuffer[(DS3231MZ_TEMP_LSB - DS3231MZ_SECOND) + 1] = {0};
uint16_t u16ReadStatus = I2C_STATUS_INACTIVE;

bool DS3231MZ_bReadRequest = false;
bool bReadAnswerWait = false;

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */
void DS3231MZ_vInit(void)
{
    I2C_DRIVER_vSetupMessageSlave(I2CB, DS3231MZ_SLAVE_ADDRESS, DS3231MZ_REG_ADDR_SIZE, I2C_RD);
    I2C_DRIVER_vSetupMessageSlave(I2CB, DS3231MZ_SLAVE_ADDRESS, DS3231MZ_REG_ADDR_SIZE, I2C_WR);
}

//connect resistor DSP_SDA_RTC <-> SDA_RTC ?
//connect resistor DSP_SCL_RTC <-> SCL_RTC ?

void DS3231MZ_vProcess(void)
{
    if (DS3231MZ_bReadRequest)
    {
        if (bReadAnswerWait == false)
        {
            if (I2C_DRIVER_vReadData(I2CB, DS3231MZ_SECOND, ((DS3231MZ_YEAR - DS3231MZ_SECOND) + 1), &au8ReadBuffer[DS3231MZ_SECOND], &u16ReadStatus))
            {
                bReadAnswerWait = true;
                DS3231MZ_bReadRequest = false;
            }
        }
    }

    if (bReadAnswerWait)
    {
        if (u16ReadStatus == I2C_STATUS_INACTIVE)
        {
            /* au8ReadBuffer Filled */
            bReadAnswerWait = false;

        }
        else if (u16ReadStatus == I2C_STATUS_FAILED)
        {
            bReadAnswerWait = false;
        }
    }

}
