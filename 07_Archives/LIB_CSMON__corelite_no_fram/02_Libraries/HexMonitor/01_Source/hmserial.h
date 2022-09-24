/* *****************************************************************************
 * File:   hmserial.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 01 12 10:56
 * 
 * Description: Hex Monitor Serial Communication
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef HMSERIAL_H
#define	HMSERIAL_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "hmdrv.h"
#include "hmprotocol.h"

/*****************************************************************************
* Configuration Definitions
******************************************************************************/


/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void HMSERIAL_vSendResponse(HMDRV_BPTR pResponse, HMDRV_SIZE8 nLength);
void HMSERIAL_vSetPortIndex(HMDRV_U8 u8Index);
#if HMDRV_USE_FREEMASTER
void HMSERIAL_vInit(void);
#else
#define HMSERIAL_vInit()
#endif

void HMSERIAL_vProcessSCI(void);            /* combined with modbus */

#if HMDRV_USE_FREEMASTER
void HMSERIAL_vProcessSCIFreemasterOnly(void);
#else
#define HMSERIAL_vProcessSCIFreemasterOnly()
#endif

#if HMDRV_SCI_TX_ISR
void HMSERIAL_vProcessSCITXISR (void);      /* combined with modbus */
#if HMDRV_USE_FREEMASTER
void HMSERIAL_vProcessSCITXISRFreemasterOnly (void);
#else
#define HMSERIAL_vProcessSCITXISRFreemasterOnly()
#endif
#endif

#if HMDRV_TX_DELAY
void HMSERIAL_vProcessTxDelay(HMDRV_TIMER nTimeTickDelay);
#endif

#if (HMDRV_USE_MODBUS && HMDRV_MODBUS_TX_DELAY) || HMDRV_TX_DELAY
HMDRV_TIMER HMSERIAL_vProcessFreeTimer(void); /* combined with modbus */
#endif

void HMSERIAL_vProcessFastTD (void);

#endif	/* HMSERIAL_H */
