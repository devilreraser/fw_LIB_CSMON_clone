/* *****************************************************************************
 * File:   i2cdac.h
 * Author: Dimitar Lilov
 *
 * Created on 2020 04 30 13:20
 * 
 * Description: ...
 * 
 **************************************************************************** */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Guard condition file contents not included more than once */  
/* #pragma once */
#ifndef I2CDAC_H
#define	I2CDAC_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdbool.h>
#include <stdint.h>
    
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define A0_PIN_AGND                 0
#define A0_PIN_VDD                  1
#define A0_PIN_SDA                  2
#define A0_PIN_SCL                  3



#define I2CDAC_DACx3608             1
#define I2CDAC_DACx3608_BASE_ADR    (uint16_t)(0b1001 << 3)
#define I2CDAC_DACx3608_A0_PIN      A0_PIN_AGND 
    
#define I2CDAC_FBKPIC16             1
#define I2CDAC_FBKPIC16_BASE_ADR    (uint16_t)(0b1000 << 3)
#define I2CDAC_FBKPIC16_A0_PIN      A0_PIN_AGND 
    
/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */
#if I2CDAC_DACx3608
#define I2CDAC_DEVICE_ADDRESS_7BIT  (uint16_t)(I2CDAC_DACx3608_BASE_ADR | I2CDAC_DACx3608_A0_PIN)
#define I2CDAC_DEVICE_ADDRESS       (uint16_t)(I2CDAC_DEVICE_ADDRESS_7BIT << 1)
#define I2CDAC_SW_RST_ADDRESS       0x00
#define I2CDAC_SW_RST_COMMAND       0x06
#else
#error "Device not Inplemented!"
#endif
    
    
#if I2CDAC_FBKPIC16
#define I2CDAC_DEVFBK_ADDRESS_7BIT  (uint16_t)(I2CDAC_FBKPIC16_BASE_ADR | I2CDAC_FBKPIC16_A0_PIN)
#define I2CDAC_DEVFBK_ADDRESS       (uint16_t)(I2CDAC_DEVFBK_ADDRESS_7BIT << 1)
#else
#error "Device not Inplemented!"
#endif
    
    
#define DEVICE_ID_DAC53608          0b001100
#define DEVICE_ID_DAC43608          0b010100
/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

typedef enum
{
    DACx3608_CMD_DEVICE_CONFIG  = 0x01,
    DACx3608_CMD_STATUS_TRIGGER = 0x02,
    DACx3608_CMD_BRDCAST        = 0x03,
    DACx3608_CMD_DACA_DATA      = 0x08,
    DACx3608_CMD_DACB_DATA      = 0x09,
    DACx3608_CMD_DACC_DATA      = 0x0A,
    DACx3608_CMD_DACD_DATA      = 0x0B,
    DACx3608_CMD_DACE_DATA      = 0x0C,
    DACx3608_CMD_DACF_DATA      = 0x0D,
    DACx3608_CMD_DACG_DATA      = 0x0E,
    DACx3608_CMD_DACH_DATA      = 0x0F,
            
} I2CDAC_eCommandByteDACx3608_t;


typedef enum
{
    FBKPIC16_AN0 =  0x0,
    FBKPIC16_AN2 =  0x2,
    FBKPIC16_AN3 =  0x3,
    FBKPIC16_AN4 =  0x4,
    FBKPIC16_AN5 =  0x5,
    FBKPIC16_AN6 =  0x6,
    FBKPIC16_AN7 =  0x7,
    FBKPIC16_channel_Temp =  0x1D,
    FBKPIC16_channel_DAC =  0x1E,
    FBKPIC16_channel_FVR =  0x1F
} I2CDAC_eFBKPIC16ADCChannel_t;


typedef enum
{
    FBKPIC16_CMD_DACA_DATA      = FBKPIC16_AN4,
    FBKPIC16_CMD_DACB_DATA      = FBKPIC16_AN6,
    FBKPIC16_CMD_DACC_DATA      = FBKPIC16_AN2,
    FBKPIC16_CMD_DACD_DATA      = FBKPIC16_AN5,
    FBKPIC16_CMD_DACE_DATA      = FBKPIC16_AN7,
    FBKPIC16_CMD_DACF_DATA      = FBKPIC16_AN3,
    //FBKPIC16_CMD_DACG_DATA      = 0x0E,
    //FBKPIC16_CMD_DACH_DATA      = 0x0F,
} I2CDAC_eCommandByteFBKPIC16_t;




/* Consider Move this to boardcfg.h */
typedef enum 
{
    I2CDAC_TRIP_CURR_STACK1_POSITIVE    = DACx3608_CMD_DACA_DATA ,
    I2CDAC_TRIP_CURR_STACK1_NEGATIVE    = DACx3608_CMD_DACB_DATA ,
    I2CDAC_TRIP_CURR_STACK2_POSITIVE    = DACx3608_CMD_DACC_DATA ,
    I2CDAC_TRIP_CURR_STACK2_NEGATIVE    = DACx3608_CMD_DACD_DATA ,
    I2CDAC_TRIP_VOLT_DCLINK_POSITIVE    = DACx3608_CMD_DACE_DATA ,
    I2CDAC_TRIP_VOLT_DCLINK_NEGATIVE    = DACx3608_CMD_DACF_DATA ,

    I2CDAC_TRIP_COUNT                   = 6
}I2CDAC_eCommandChannelAddress_t;


/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
typedef struct 
{
    uint16_t bPDNA :1;
    uint16_t bPDNB :1;
    uint16_t bPDNC :1;
    uint16_t bPDND :1;
    uint16_t bPDNE :1;
    uint16_t bPDNF :1;
    uint16_t bPDNG :1;
    uint16_t bPDNH :1;
    uint16_t bPDNAll :1;
    uint16_t u3Zero :3;
    uint16_t :4;
    
}I2CDAC_sDeviceConfigDACx3608_t;

typedef union 
{
    uint16_t u16Register;
    I2CDAC_sDeviceConfigDACx3608_t sDeviceConfig;
    uint8_t au8Bytes[2];
    
}I2CDAC_uDeviceConfigDACx3608_t;


typedef struct 
{
    uint16_t u4SoftwareReset :4;
    uint16_t :2;
    uint16_t u6DeviceID :6;
    uint16_t :4;
    
}I2CDAC_sStatusTriggerDACx3608_t;

typedef union 
{
    uint16_t u16Register;
    I2CDAC_sStatusTriggerDACx3608_t sStatusTrigger;
    uint8_t au8Bytes[2];
    
}I2CDAC_uStatusTriggerDACx3608_t;


typedef struct 
{
    uint16_t :2;
    uint16_t u10Data :10;        /* MSB Left Aligned */
    uint16_t :4;
    
}I2CDAC_sDataDACx3608_t;

typedef union 
{
    uint16_t u16Register;
    I2CDAC_sDataDACx3608_t sData;
    uint8_t au8Bytes[2];
    
}I2CDAC_uDataDACx3608_t;


/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
bool I2CDAC_bReadDeny(void);
bool I2CDAC_bUpdating(void);
void I2CDAC_vSetUpdateReg(bool bInput);
void I2CDAC_vSetDenyReadFbk(bool bInput);
void I2CDAC_vUpdate(void);
void I2CDAC_vSetChannel(I2CDAC_eCommandChannelAddress_t eChannel, uint16_t u16Value);
void I2CDAC_vGetChannel(I2CDAC_eCommandChannelAddress_t eChannel, uint16_t* pu16Value);
void I2CDAC_vSetChannels(void);

#endif	/* I2CDAC_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
