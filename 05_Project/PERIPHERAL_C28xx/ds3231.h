/*
 * ds3231.h
 *
 *  Created on: Nov 21, 2020
 *      Author: dpenev
 */

#ifndef DS3231_H_
#define DS3231_H_

#include "date_time.h"

// I2C RELATED CONSTANTS -------------------
#define I2C_TIMEOUT         10000
#define I2C_TIMEOUT_ERROR   0x666

// DS3231 Registers ------------------------
#define DS3231_SECOND           0x00
#define DS3231_MINUTE           0x01
#define DS3231_HOUR             0x02
#define DS3231_DAY              0x03
#define DS3231_DATE             0x04
#define DS3231_MONTH            0x05
#define DS3231_YEAR             0x06
#define DS3231_ALRM1_SECOND     0x07
#define DS3231_ALRM1_MINUTE     0x08
#define DS3231_ALRM1_HOUR       0x09
#define DS3231_ALRM1_DAY_DATE   0x0A
#define DS3231_ALRM2_MINUTE     0x0B
#define DS3231_ALRM2_HOUR       0x0C
#define DS3231_ALRM2_DAY_DATE   0x0D
#define DS3231_CTRL             0x0E
#define DS3231_CTRL_STATS       0x0F
#define DS3231_AGING_OFFS       0x10
#define DS3231_TEMP_MSB         0x11
#define DS3231_TEMP_LSB         0x12


// DS3231 functions ------------------------
void DS3231_I2CB_Init(void);
uint16_t DS3231_WriteRegister(uint16_t Address, uint16_t Data);
uint16_t DS3231_ReadRegister(uint16_t Address, uint16_t *Data);
uint16_t DS3231_Set(date_time_t *date_time);
uint16_t DS3231_Get(date_time_t *date_time);

#endif /* DS3231_H_ */
