/* *****************************************************************************
 * File:   hmcfg.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 08 06 17:18
 * 
 * Description: Hex Monitor Driver Identification
 * 
 **************************************************************************** */

/* Guard condition file contents not included more than once */  
#ifndef HMDRVID_H
#define	HMDRVID_H


/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* consider HMDRV_IDT_STRING string definition in external board configuration header file instead of below  */
#define HMDRV_STRING_TSC_dsPIC33        "TSC_dsPIC33"
#define HMDRV_STRING_TSC_PIC16F18345    "TSC_PIC16F18345"
#define HMDRV_STRING_FLIR_dsPIC33       "FLIR_dsPIC33"
#define HMDRV_STRING_SCC_Delfino        "SCC_Delfino"
#define HMDRV_STRING_SCC_TMS320         "SCC_TMS320"

#define HMDRV_BOARD_UNKNOWN         0
#define HMDRV_BOARD_TSC_dsPIC33     1
#define HMDRV_BOARD_TSC_PIC16F18345 2
#define HMDRV_BOARD_FLIR_dsPIC33    3
#define HMDRV_BOARD_SCC_Delfino     4
#define HMDRV_BOARD_SCC_TMS320      5

#define HMDRV_BOARD_COUNT           255




/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */


#endif	/* HMCFG_H */
