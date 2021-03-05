/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.7
        Device            :  PIC16F18446
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "mcc_generated_files/mcc.h"
#include "ds3231.h"
#include "mcc_generated_files/examples/i2c1_master_example.h"

/* If true bAutoI2C has priority */
bool bAutoI2C = true; // false; // 

bool bAutoRTC = false; // true; // 
bool bAutoVoltMeas = true; // false; // 

bool bAutoReadFPGA = true; // false; // 



bool bReadRTC = false;
bool bWriteRTC = false;
bool bWriteVoltMeas = false;
bool bReadFPGA = false;



bool bTimePassed = false;


void TimePassed(void)
{
    bTimePassed = true;
}



#define I2C_SLAVE_ADDRESS_VOLTAGE_MEASURE       0x20

#define ADC_REAL_VALUE_WORD     1               /* 12 bit ADC value + 16 samples (4 bits) moving average -> 16 Bit ADCResult Value */

#define ADC_VREF_mV             (2048.0)
#define ADC_MULT_SENSE_0V9      (uint16_t)(ADC_VREF_mV)
#define ADC_MULT_SENSE_1V1      (uint16_t)(ADC_VREF_mV)
#define ADC_MULT_SENSE_1V2      (uint16_t)(ADC_VREF_mV)
#define ADC_MULT_SENSE_1V8      (uint16_t)(ADC_VREF_mV)
#define ADC_MULT_SENSE_2V5      (uint16_t)(ADC_VREF_mV * ((4700.0 + 4700.0) / 4700.0))
#define ADC_MULT_SENSE_3V3      (uint16_t)(ADC_VREF_mV * ((4700.0 + 4700.0) / 4700.0))

typedef enum
{
    ADC_STA_SENSE_0V9,
    ADC_STA_SENSE_1V1,
    ADC_STA_SENSE_1V2,
    ADC_STA_SENSE_1V8,
    ADC_STA_SENSE_2V5,
    ADC_STA_SENSE_3V3,
    ADC_STA_SENSE_CNT,
            
}eADCStatus_t;

typedef union
{
    uint32_t u32Register;
    uint16_t u16Word[2];
    uint8_t u8Byte[4];
}uint32_u;

typedef union
{
    uint16_t u16Register;
    uint8_t u8Byte[2];
}uint16_u;

eADCStatus_t eADCStatus;


const uint16_t au16ADCMult[ADC_STA_SENSE_CNT] = 
{
    ADC_MULT_SENSE_0V9,
    ADC_MULT_SENSE_1V1,
    ADC_MULT_SENSE_1V2,
    ADC_MULT_SENSE_1V8,
    ADC_MULT_SENSE_2V5,
    ADC_MULT_SENSE_3V3,
};

uint16_t au16ADCResult[ADC_STA_SENSE_CNT];
uint32_u auADCReal[ADC_STA_SENSE_CNT];
uint16_t au16ADCReal[ADC_STA_SENSE_CNT];

uint8_t au8RTCRead[8] = 
{
    0x00,   //sec
    0x00,   //min
    0x00,   //hour
    0x01,   //weekday
    0x01,   //day
    0x01,   //month
    0x01,   //year
    0x00,   //control
};

uint8_t au8RTCWrite[8] = 
{
    0x00,   //sec
    0x00,   //min
    0x00,   //hour
    0x01,   //weekday
    0x01,   //day
    0x01,   //month
    0x01,   //year
    0x00,   //control
};

uint16_t au16I2CWriteToFPGA[(8/2)+ ADC_STA_SENSE_CNT]; /* 20 bytes */
       
uint16_t au16I2CReadFromFPGA[(8/2)]; /* 8 bytes */
       
#define ADC_u16GetRealValueFrom_auADCReal(index) auADCReal[index].u16Word[ADC_REAL_VALUE_WORD];

void ADC_vInit(void)
{
   //ADCC_SetChannel(SENSE_1V1);
   //eADCStatus = ADC_STA_SENSE_1V1;
   ADCC_DischargeSampleCapacitor();
   eADCStatus = ADC_STA_SENSE_CNT;
}

void ADC_vResultProcess(eADCStatus_t eStatus)
{
    if (eStatus < ADC_STA_SENSE_CNT)
    {
#if 0
        au16ADCResult[eStatus] = ADCC_GetConversionResult() * 16;
#else
        au16ADCResult[eStatus] -= au16ADCResult[eStatus] >> 4;  /* 2^4 (16) Values Moving Average Filter */
        au16ADCResult[eStatus] += ADCC_GetConversionResult();
#endif
    }
}

void ADC_vRealCalculate(eADCStatus_t eStatus)
{
    if (eStatus < ADC_STA_SENSE_CNT)
    {
        auADCReal[eStatus].u32Register = (uint32_t)au16ADCResult[eStatus] * au16ADCMult[eStatus];  
        au16ADCReal[eStatus] = ADC_u16GetRealValueFrom_auADCReal(eStatus);
    }
}

void ADC_vRealCalculateInMainLoop(eADCStatus_t eStatus)
{
    eADCStatus_t eADCStatusBeforeRead;
    uint16_t u16ADCResult;
    
    if (eStatus < ADC_STA_SENSE_CNT)
    {
        /* assure proper value taken from the ISR */
        do
        {
            eADCStatusBeforeRead = eADCStatus;
            u16ADCResult = au16ADCResult[eStatus];
        }while ((eADCStatus != eStatus) && (eADCStatusBeforeRead == eADCStatus));
        auADCReal[eStatus].u32Register = (uint32_t)u16ADCResult * au16ADCMult[eStatus];  
        au16ADCReal[eStatus] = ADC_u16GetRealValueFrom_auADCReal(eStatus);
    }
}
void ADC_vProcess(void)
{
    switch(eADCStatus)
    {
        case ADC_STA_SENSE_0V9:
        {
            ADC_vResultProcess(ADC_STA_SENSE_0V9);
            //ADC_vRealCalculate(ADC_STA_SENSE_0V9);
            ADCC_SetChannel(SENSE_1V1);
            eADCStatus = ADC_STA_SENSE_1V1;
            break;
        }
        case ADC_STA_SENSE_1V1:
        {
            ADC_vResultProcess(ADC_STA_SENSE_1V1);
            //ADC_vRealCalculate(ADC_STA_SENSE_1V1);
            ADCC_SetChannel(SENSE_1V2);
            eADCStatus = ADC_STA_SENSE_1V2;
            break;
        }
        case ADC_STA_SENSE_1V2:
        {
            ADC_vResultProcess(ADC_STA_SENSE_1V2);
            //ADC_vRealCalculate(ADC_STA_SENSE_1V2);
            ADCC_SetChannel(SENSE_1V8);
            eADCStatus = ADC_STA_SENSE_1V8;
            break;
        }
        case ADC_STA_SENSE_1V8:
        {
            ADC_vResultProcess(ADC_STA_SENSE_1V8);
            //ADC_vRealCalculate(ADC_STA_SENSE_1V8);
            ADCC_SetChannel(SENSE_2V5);
            eADCStatus = ADC_STA_SENSE_2V5;
            break;
        }
        case ADC_STA_SENSE_2V5:
        {
            ADC_vResultProcess(ADC_STA_SENSE_2V5);
            //ADC_vRealCalculate(ADC_STA_SENSE_2V5);
            ADCC_SetChannel(SENSE_3V3);
            eADCStatus = ADC_STA_SENSE_3V3;
            break;
        }
        case ADC_STA_SENSE_3V3:
        {
            ADC_vResultProcess(ADC_STA_SENSE_3V3);
            //ADC_vRealCalculate(ADC_STA_SENSE_3V3);
            ADCC_SetChannel(SENSE_0V9);
            eADCStatus = ADC_STA_SENSE_0V9;
            break;
        }
        default:
        {
            ADCC_SetChannel(SENSE_0V9);
            eADCStatus = ADC_STA_SENSE_0V9;
            break;
        }
    }
}


typedef enum
{
    MAIN_STA_I2C_RW_TIME,
    MAIN_STA_CALCULATE_VOLTAGES,
    MAIN_STA_I2C_VOLT_MEAS,
    MAIN_STA_I2C_RD_FPGA,
    MAIN_STA_COUNT,
            
}eMainState_t;

eMainState_t eMainState;
uint8_t u8MainIndex;

bool bReadFPGACheck = false;

bool bValidSeconds = false;
bool bValidMinutes = false;
bool bValidHours   = false;
bool bValidWeekday = false;
bool bValidDay     = false;
bool bValidMonth   = false;
bool bValidYear    = false;
bool bValidCommand = false;


/*
                        Main application
 */
void main(void)
{

    
    // initialize the device
    SYSTEM_Initialize();
    
    ADC_vInit();
    
    while (FVR_IsOutputReady() == false)
    {
        ;
    }
    
    // Set Interrupt Handler
    TMR0_SetInterruptHandler(TimePassed);
    ADCC_SetADIInterruptHandler(ADC_vProcess);

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();

    while (1)
    {
        // Add your application code
        if (bTimePassed)
        {
            bTimePassed = false;
            
            
            switch (eMainState)
            {
                case MAIN_STA_I2C_RW_TIME:
                {
                    if (bReadFPGACheck)
                    {
                        bReadFPGACheck = false;
                        
                        ((uint16_t*)au8RTCWrite)[0] = au16I2CReadFromFPGA[0];
                        ((uint16_t*)au8RTCWrite)[1] = au16I2CReadFromFPGA[1];
                        ((uint16_t*)au8RTCWrite)[2] = au16I2CReadFromFPGA[2];
                        ((uint16_t*)au8RTCWrite)[3] = au16I2CReadFromFPGA[3]; 

                        /* Check Seconds */
                        if (((au8RTCWrite[0] & 0x0F) < 10) && ((au8RTCWrite[0] >> 4) < 6)) 
                        {
                            bValidSeconds = true;
                        }
                        else
                        { 
                            bValidSeconds = false;
                        }
                        /* Check Minutes */
                        if (((au8RTCWrite[1] & 0x0F) < 10) && ((au8RTCWrite[1] >> 4) < 6)) 
                        {
                            bValidMinutes = true;
                        }
                        else
                        { 
                            bValidMinutes = false;
                        }
                        /* Check Hours */
                        if (((au8RTCWrite[2] & 0x0F) < 10) && ((au8RTCWrite[2] >> 4) < 3) && (au8RTCWrite[2] < 0x24) ) 
                        {
                            bValidHours = true;
                        }
                        else
                        { 
                            bValidHours = false;
                        }
                        /* Check Weekday */
                        if (((au8RTCWrite[3] & 0x0F) < 7) && ((au8RTCWrite[3] >> 4) < 1)) 
                        {
                            bValidWeekday = true;
                        }
                        else
                        { 
                            bValidWeekday = false;
                        }
                        /* Check Day */
                        if (((au8RTCWrite[4] & 0x0F) < 10) && ((au8RTCWrite[4] >> 4) < 4) && (au8RTCWrite[4] < 0x32) ) 
                        {
                            bValidDay = true;
                        }
                        else
                        { 
                            bValidDay = false;
                        }
                        /* Check Month */
                        if (((au8RTCWrite[5] & 0x0F) < 10) && ((au8RTCWrite[5] >> 4) < 2) && (au8RTCWrite[5] < 0x13) ) 
                        {
                            bValidMonth = true;
                        }
                        else
                        { 
                            bValidMonth = false;
                        }
                        /* Check Year */
                        if (((au8RTCWrite[6] & 0x0F) < 10) && ((au8RTCWrite[6] >> 4) < 10)) 
                        {
                            bValidYear = true;
                        }
                        else
                        { 
                            bValidYear = false;
                        }
                        /* Check Command */
                        if (au8RTCWrite[7] == 0) 
                        {
                            bValidCommand = true;
                        }
                        else
                        { 
                            bValidCommand = false;
                        }
                        



                        if (bValidSeconds && bValidMinutes && bValidHours && bValidWeekday && bValidDay && bValidMonth && bValidYear && bValidCommand) 
                        {
                            bWriteRTC = true;

                            DS3231_au8MemorySet[0] = au8RTCWrite[0];
                            DS3231_au8MemorySet[1] = au8RTCWrite[1];
                            DS3231_au8MemorySet[2] = au8RTCWrite[2];
                            DS3231_au8MemorySet[3] = au8RTCWrite[3];
                            DS3231_au8MemorySet[4] = au8RTCWrite[4];
                            DS3231_au8MemorySet[5] = au8RTCWrite[5];
                            DS3231_au8MemorySet[6] = au8RTCWrite[6];
                            DS3231_au8MemorySet[7] = au8RTCWrite[7];    //To do: use here config data??? for now only 0 is valid
                        }  
                    }
                    if (bWriteRTC)
                    {
                        bWriteRTC = false;
                        DS3231_vRTCWrite();
                    }
                    else 
                    {
                        if ((bAutoI2C) || (bAutoRTC))
                        {
                            bReadRTC = true;
                        }
                        if (bReadRTC)
                        {
                            bReadRTC = false;
                            DS3231_vRTCRead();
                        }
                    }
                    break;
                }
                case MAIN_STA_CALCULATE_VOLTAGES:
                {
                    for (u8MainIndex = 0; u8MainIndex < ADC_STA_SENSE_CNT; u8MainIndex++)
                    {
                        ADC_vRealCalculateInMainLoop(u8MainIndex);
                    }
                    break;
                }
                case MAIN_STA_I2C_VOLT_MEAS:
                {
                    if ((bAutoI2C) || (bAutoVoltMeas))
                    {
                        bWriteVoltMeas = true;
                    }
                    if (bWriteVoltMeas)
                    {
                        bWriteVoltMeas = false;
                        
                        au8RTCRead[0] = DS3231_au8MemoryGet[0];
                        au8RTCRead[1] = DS3231_au8MemoryGet[1];
                        au8RTCRead[2] = DS3231_au8MemoryGet[2];
                        au8RTCRead[3] = DS3231_au8MemoryGet[3];
                        au8RTCRead[4] = DS3231_au8MemoryGet[4];
                        au8RTCRead[5] = DS3231_au8MemoryGet[5];
                        au8RTCRead[6] = DS3231_au8MemoryGet[6];
                        au8RTCRead[7] = 0;  //use here config data???
                        
                        au16I2CWriteToFPGA[0] = ((uint16_u*)au8RTCRead)[0].u16Register;
                        au16I2CWriteToFPGA[1] = ((uint16_u*)au8RTCRead)[1].u16Register;
                        au16I2CWriteToFPGA[2] = ((uint16_u*)au8RTCRead)[2].u16Register;
                        au16I2CWriteToFPGA[3] = ((uint16_u*)au8RTCRead)[3].u16Register;
                        au16I2CWriteToFPGA[4] = au16ADCReal[0];
                        au16I2CWriteToFPGA[5] = au16ADCReal[1];
                        au16I2CWriteToFPGA[6] = au16ADCReal[2];
                        au16I2CWriteToFPGA[7] = au16ADCReal[3];
                        au16I2CWriteToFPGA[8] = au16ADCReal[4];
                        au16I2CWriteToFPGA[9] = au16ADCReal[5];
                        I2C1_WriteNBytes(I2C_SLAVE_ADDRESS_VOLTAGE_MEASURE, (uint8_t*)&au16I2CWriteToFPGA[0], sizeof(au16I2CWriteToFPGA));
                    }
                    break;
                }
                case MAIN_STA_I2C_RD_FPGA:
                {
                    
                    if ((bAutoI2C) || (bAutoReadFPGA))
                    {
                        bReadFPGA = true;
                    }
                    if (bReadFPGA)
                    {
                        bReadFPGA = false;
                        au16I2CReadFromFPGA[0] = 0xFFFF;
                        au16I2CReadFromFPGA[1] = 0xFFFF;
                        au16I2CReadFromFPGA[2] = 0xFFFF;
                        au16I2CReadFromFPGA[3] = 0xFFFF;
                        bReadFPGACheck = true;
                        I2C1_ReadNBytes(I2C_SLAVE_ADDRESS_VOLTAGE_MEASURE, (uint8_t*)&au16I2CReadFromFPGA[0], sizeof(au16I2CReadFromFPGA));
                    }
                    break;
                }
                default:
                {
                    break;
                }
            }
            eMainState++;
            if(eMainState >= MAIN_STA_COUNT)
            {
                eMainState = MAIN_STA_I2C_RW_TIME;
            }
            
        }
    }
}
/**
 End of File
*/