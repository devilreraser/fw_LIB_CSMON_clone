/* *****************************************************************************
 * File:   i2c_driver.h
 * Author: Dimitar Lilov
 *
 * Created on 2021 02 23 14:55
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
#ifndef I2C_DRIVER_H
#define	I2C_DRIVER_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdbool.h>
#include <stdint.h>

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define I2CA                        0
#define I2CB                        1
#define I2C_COUNT                   2       //I2CA, I2CB

#define I2C_WR                      0
#define I2C_RD                      1

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

//
// I2C message states for I2CMsg struct
//
#define I2C_STATUS_INACTIVE         0x0000 // Message not in use, do not send
#define I2C_STATUS_SEND_WITHSTOP    0x0010 // Send message with stop bit
#define I2C_STATUS_WRITE_BUSY       0x0011 // Message sent, wait for stop
#define I2C_STATUS_SEND_NOSTOP      0x0020 // Send message without stop bit
#define I2C_STATUS_SEND_NOSTOP_BUSY 0x0021 // Message sent, wait for ARDY
#define I2C_STATUS_RESTART          0x0022 // Ready to become master-receiver
#define I2C_STATUS_READ_BUSY        0x0023 // Wait for stop before reading data

#define I2C_STATUS_FAILED           0x00FF // Failed Message

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void I2C_DRIVER_vInit(void);
void I2C_DRIVER_vProcess(void);
void I2C_DRIVER_vSetupMessageSlave (uint16_t indexI2C, uint16_t slaveAddress, uint16_t registerAddressBytes, bool bRead);
bool I2C_DRIVER_vReadData (uint16_t indexI2C, uint16_t registerStartAddress, uint16_t countBytes, uint_least8_t* pData, uint16_t* pStatus);
bool I2C_DRIVER_vWriteData (uint16_t indexI2C, uint16_t registerStartAddress, uint16_t countBytes, uint_least8_t* pData, uint16_t* pStatus);
 
#endif	/* I2C_DRIVER_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
