/* *****************************************************************************
 * File:   emif_drv.h
 * Author: Dimitar Lilov
 *
 * Created on 2020 01 02 20:51
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
#ifndef EMIF_DRV_H
#define	EMIF_DRV_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
    
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
//#define RES150_10KOHM_AND_R546_0OHM


#define USE_16_BIT_MRAM     1

#if USE_16_BIT_MRAM
/*
 * In this mode : 2MB memory Size
 *
 *              16-bit write is ok
 *              16-bit read is ok
 *
 *              32 bit read is ok
 *
 *              32 bit write writes only the high word
 *
 */

#define EMIF_ASYNC_DATA_WIDTH   EMIF_ASYNC_DATA_WIDTH_16
	#define USE_A19_AS_BA1          1
	#define USE_EMIF2_A12_AS_BA1    1
#else
/*
 * In this mode : 4MB memory Size
 *
 *              16-bit write -> writes one and the same word to high and low address
                16-bit read is ok
 *
 *              32 bit read is ok
 *
 *              32 bit write is ok
 *
 */

#define EMIF_ASYNC_DATA_WIDTH   EMIF_ASYNC_DATA_WIDTH_32
	#define USE_A19_AS_BA1          0
#endif


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
void EMIF_vInit(void);
void EMIF_vProcess(void);
void EMIF_2_vProcess(void);

 
#endif	/* EMIF_DRV_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
