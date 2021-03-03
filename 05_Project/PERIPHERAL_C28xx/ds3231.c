/*
 * ds3231.c
 *  DS3231 RTC support functions
 *
 *  Created on: Nov 21, 2020
 *      Author: dpenev
 */

#include "F2837xD_device.h"        // F2837xD Headerfile Include File
#include "F2837xD_I2c_defines.h"
#include "ds3231.h"
#include "date_time.h"

//
// Set the DS3231 RTC =======================================================================================================
//
uint16_t DS3231_Set(date_time_t *date_time){

    uint16_t i2c_res = DATE_TIME_OK;

    if(date_time->year>100 || date_time->month==0 || date_time->month>12 || \
       date_time->day==0  || date_time->day>31 || date_time->hour>23 || \
       date_time->minute>59 || date_time->second>59)
        return DATE_TIME_ERROR;


    i2c_res = DS3231_WriteRegister(DS3231_YEAR, BIN2BCD(date_time->year));
    i2c_res |= DS3231_WriteRegister(DS3231_MONTH, BIN2BCD(date_time->month));
    i2c_res |= DS3231_WriteRegister(DS3231_DATE, BIN2BCD(date_time->day));
    i2c_res |= DS3231_WriteRegister(DS3231_HOUR, BIN2BCD(date_time->hour));
    i2c_res |= DS3231_WriteRegister(DS3231_MINUTE, BIN2BCD(date_time->minute));
    i2c_res |= DS3231_WriteRegister(DS3231_SECOND, BIN2BCD(date_time->second));

    return i2c_res;
}

//
// Read Date/Time from the DS3231 RTC ======================================================================================
//
uint16_t DS3231_Get(date_time_t *date_time){

    uint16_t i2c_res = DATE_TIME_OK;
    uint16_t year, month, day, hour, minute, second;

    i2c_res = DS3231_ReadRegister(DS3231_YEAR, &year);
    i2c_res |= DS3231_ReadRegister(DS3231_MONTH, &month);
    i2c_res |= DS3231_ReadRegister(DS3231_DATE, &day);
    i2c_res |= DS3231_ReadRegister(DS3231_HOUR, &hour);
    i2c_res |= DS3231_ReadRegister(DS3231_MINUTE, &minute);
    i2c_res |= DS3231_ReadRegister(DS3231_SECOND, &second);



    if(i2c_res){
        return DATE_TIME_ERROR;
    } else {

        date_time->year = BCD2BIN(year);
        date_time->month = BCD2BIN(month);
        date_time->day = BCD2BIN(day);
        date_time->hour = BCD2BIN(hour);
        date_time->minute = BCD2BIN(minute);
        date_time->second = BCD2BIN(second);

        return DATE_TIME_OK;
    }
}

//
// I2CB_Init - Initialize I2CB settings ======================================================================================
//
void DS3231_I2CB_Init(void)
{
    EALLOW;

    GpioCtrlRegs.GPAGMUX1.bit.GPIO2 = 1; //GPIO02 is SDAB
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 2;

    GpioCtrlRegs.GPBGMUX1.bit.GPIO35 = 1;//GPIO35 is SCLB
    GpioCtrlRegs.GPBMUX1.bit.GPIO35 = 2;

    GpioCtrlRegs.GPAQSEL1.bit.GPIO2 = 3; //Async, no Sync or Qualification
    GpioCtrlRegs.GPBQSEL1.bit.GPIO35 = 3;//Async, no Sync or Qualification

    EDIS;

    I2cbRegs.I2CMDR.bit.IRS = 0;    //I2C in reset so we can adjust the module clock

    I2cbRegs.I2CPSC.all = 16;       // Prescaler - need 7-12 Mhz on module clk 11.76MHz
    I2cbRegs.I2CCLKL = 44;          //  I2C CLK 120KHz
    I2cbRegs.I2CCLKH = 44;

    I2cbRegs.I2CFFTX.all |= 0x6000;      /* Enable TxFIFO mode*/
    I2cbRegs.I2CFFRX.all |= 0x2000;      /* Enable RxFIFO mode*/

    I2cbRegs.I2CMDR.bit.IRS = 1;    // Enable I2C.

    return;
}


//
// DS3231_WriteRegister, I2CB is used
//
// Address - register address
// Data -  byte to be written
uint16_t DS3231_WriteRegister(uint16_t Address, uint16_t Data)
{

    uint16_t i;

    //
    // Wait until the STP bit is cleared from any previous master communication.
    // Clearing of this bit by the module is delayed until after the SCD bit is
    // set. If this bit is not checked prior to initiating a new message, the
    // I2C could get confused.
    //
    if(I2cbRegs.I2CMDR.bit.STP == 1)
    {
        return I2C_STP_NOT_READY_ERROR;
    }

    //
    // Check if bus busy
    //
    if(I2cbRegs.I2CSTR.bit.BB == 1)
    {
        return I2C_BUS_BUSY_ERROR;
    }

    I2cbRegs.I2CSAR.all = 0x0068;   // Slave address - DS3231

    I2cbRegs.I2CCNT= 2;             // One data byte

    I2cbRegs.I2CMDR.all = 0x6E20;   //FREE | STT | STP | MST | TRX | IRS, non repeat mode

    i=0;
    while ((I2cbRegs.I2CFFTX.bit.TXFFST != 0) && (i<I2C_TIMEOUT)) i++; // wait for transmit FIFO empty
    if(i==I2C_TIMEOUT) return I2C_TIMEOUT_ERROR;

    I2cbRegs.I2CDXR.all = Address;  //Register Address we will write in DS3231

    i=0;
    while ((I2cbRegs.I2CFFTX.bit.TXFFST != 0) && (i<I2C_TIMEOUT)) i++; // wait for transmit FIFO empty
    if(i==I2C_TIMEOUT) return I2C_TIMEOUT_ERROR;

    I2cbRegs.I2CDXR.all = Data;     //Data to be written in DS3231

    i=0;
    while((I2cbRegs.I2CMDR.bit.STP == 1) && (i<I2C_TIMEOUT)) i++; // wait for STOP to be generated.
    if(i==I2C_TIMEOUT) return I2C_TIMEOUT_ERROR;

    return I2C_SUCCESS;
}


// DS3231_ReadRegister, I2CB is used
//
// Address - register address
// Data -  result stored here
uint16_t DS3231_ReadRegister(uint16_t Address, uint16_t *Data)
{

    unsigned int i;

    //
    // Wait until the STP bit is cleared from any previous master communication.
    // Clearing of this bit by the module is delayed until after the SCD bit is
    // set. If this bit is not checked prior to initiating a new message, the
    // I2C could get confused.
    //
    if(I2cbRegs.I2CMDR.bit.STP == 1)
    {
        return I2C_STP_NOT_READY_ERROR;
    }

    //
    // Check if bus busy
    //
    if(I2cbRegs.I2CSTR.bit.BB == 1)
    {
        return I2C_BUS_BUSY_ERROR;
    }

    I2cbRegs.I2CSAR.all = 0x0068;   // Slave address - DS3231

    I2cbRegs.I2CCNT= 1;             // One data byte

    I2cbRegs.I2CMDR.all = 0x6620;   //FREE | STT | TRX | MST | IRS, non repeat mode

    i=0;
    while ((I2cbRegs.I2CFFTX.bit.TXFFST != 0) && (i<I2C_TIMEOUT)) i++; // wait for transmit FIFO empty
    if(i==I2C_TIMEOUT) return I2C_TIMEOUT_ERROR;

    I2cbRegs.I2CDXR.all = Address;  //Register Address we will write in DS3231

    i=0;
    while ((I2cbRegs.I2CFFTX.bit.TXFFST != 0) && (i<I2C_TIMEOUT)) i++; // wait for transmit FIFO empty
    if(i==I2C_TIMEOUT) return I2C_TIMEOUT_ERROR;

    I2cbRegs.I2CCNT= 1;             // Two data bytes

    I2cbRegs.I2CMDR.all = 0x6C20;   //FREE | STT | STP | MST | IRS, non repeat mode

    i=0;
    while ((I2cbRegs.I2CFFRX.bit.RXFFST == 0) && (i<I2C_TIMEOUT)) i++; // wait for receive FIFO not empty
    if(i==I2C_TIMEOUT) return I2C_TIMEOUT_ERROR;

    *Data=I2cbRegs.I2CDRR.all;      //Data read from DS3231

    i=0;
    while((I2cbRegs.I2CMDR.bit.STP == 1) && (i<I2C_TIMEOUT)) i++; // wait for STOP to be generated.
    if(i==I2C_TIMEOUT) return I2C_TIMEOUT_ERROR;

    return I2C_SUCCESS;
}


