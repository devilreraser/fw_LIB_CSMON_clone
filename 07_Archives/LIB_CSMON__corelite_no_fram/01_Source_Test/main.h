/* *****************************************************************************
 * File:   main.h
 * Author: Dimitar Lilov
 *
 * Created on 2020 03 13 09:06
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
#ifndef MAIN_H
#define	MAIN_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
    
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */
#define TEST_CSMON_APPLICATION_VERSION_HI    1
#define TEST_CSMON_APPLICATION_VERSION_LO    0

#define TEST_CSMON_APPLICATION_VERSION       (uint16_t)(((uint16_t)((uint_least8_t)(TEST_CSMON_APPLICATION_VERSION_HI & 0x00FF)) << 8) | ((uint_least8_t)(TEST_CSMON_APPLICATION_VERSION_LO & 0x00FF)))


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
 
#endif	/* MAIN_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
