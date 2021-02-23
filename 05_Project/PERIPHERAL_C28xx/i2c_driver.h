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
    
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

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
 
#endif	/* I2C_DRIVER_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
