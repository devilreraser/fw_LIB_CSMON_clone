/* *****************************************************************************
 * File:   pmpmem.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 02 01 15:05
 * 
 * Description: PMP Interface To External Memory
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef PMPMEM_H
#define	PMPMEM_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "extmemcfg.h"
#include "hmlib.h"

#include "pmp.h"    /* If Used Fast Interface */

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants Definitions
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
#if PMP_FAST_RW
#define PMPMEM_vWriteToExtMemSingle(u16Address, pdData, dummy)      PMP_vWriteFastPMCS1Byte(u16Address, pdData)
#define PMPMEM_vReadFromExtMemSingle(pdData, u16Address, dummy)     PMP_vReadFastPMCS1Byte(u16Address, pdData)
#define PMPMEM_vWriteToExtMem(u16Address, pdData, u16Size)      PMP_vWriteFastPMCS1(u16Address, pdData, u16Size)
#define PMPMEM_vReadFromExtMem(pdData, u16Address, u16Size)     PMP_vReadFastPMCS1(u16Address, pdData, u16Size)
#else
#define PMPMEM_vWriteToExtMemSingle(nDestAddr, pSrcBuff, dummy)      PMPMEM_vCopyToExtMem(nDestAddr, pSrcBuff, 1)
#define PMPMEM_vReadFromExtMemSingle(pDestAddr, nSrcAddr, dummy)     PMPMEM_vCopyFromExtMem(pDestAddr, nSrcAddr, 1)
#define PMPMEM_vWriteToExtMem(nDestAddr, pSrcBuff, nSize)    PMPMEM_vCopyToExtMem(nDestAddr, pSrcBuff, nSize)
#define PMPMEM_vReadFromExtMem(pDestAddr, nSrcAddr, nSize)   PMPMEM_vCopyFromExtMem(pDestAddr, nSrcAddr, nSize)
#endif




/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */


/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void PMPMEM_vInit(void);
void PMPMEM_vProcessRW(void);

void PMPMEM_vCopyToExtMem(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize);
void PMPMEM_vCopyToExtMemWithMask(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize);
void PMPMEM_vCopyFromExtMem(HMDRV_BPTR pDestAddr, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize);
//void PMPMEM_vCopyFromExtMemBigEndian(HMDRV_BPTR pDestAddr, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize);
HMDRV_ADDR PMPMEM_pBufferAddressGet(HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize);
void PMPMEM_vSendBufferToPMP(HMDRV_SIZE8 nSize);

HMDRV_ADDR PMPMEM_pExtMemRamToIntRam(HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize);
HMDRV_ADDR PMPMEM_pExtMemRamFromIntRam(HMDRV_SIZE8 nSize);
void       PMPMEM_vExtMemRamFromIntRamCopyData(HMDRV_ADDR nInternalRAMBufferAddr, HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize);

void PMPMEM_vReleaseQueueBuffer(uint8_t* pBuffer);

#endif	/* PMPMEM_H */
