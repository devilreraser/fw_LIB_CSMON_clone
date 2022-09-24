/* *****************************************************************************
 * File:   pmpmem.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 02 01 15:05
 * 
 * Description: PMP Interface To External Memory
 * 
 **************************************************************************** */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <xc.h>
#include <stdbool.h>
#include <stdint.h>

#include "hmdrv.h"
#include "pmp.h"
#include "isr.h"
#include "pmpmem.h"
#if USE_DYNAMIC_MEMORY_RW
#include "memory.h"
#endif
#include "queue.h"
#include "boardcfg.h"


/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define PMP_BUFFER_SIZE (uint16_t)(1 << 3)     /* 8 Bytes */


/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */






/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */



PMP_eUsePMPDMA_t eUsePMPDMA = (PMP_eUsePMPDMA_t)PMP_USE_DMA;

DMA_eModule_t eModuleDMAPMP  = EXT_MEM_PMP_DMA;




uint16_t u16PMPAddress = 0x0000;

uintPMP_t udPMPBufferRD[PMP_BUFFER_SIZE] = 
{
    0xFF,0xFE,0xFD,0xFC,0xFB,0xFA,0xF9,0xF8,
};
uintPMP_t udPMPBufferWR[PMP_BUFFER_SIZE] = 
{
    0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,
};


uintPMP_t udPMPBufferRDMainRW[PMP_BUFFER_SIZE] = 
{
    0xFF,0xFE,0xFD,0xFC,0xFB,0xFA,0xF9,0xF8,
};
uintPMP_t udPMPBufferWRMainRW[PMP_BUFFER_SIZE] = 
{
    0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,
};

/* not used the one below */
uintPMP_t udPMPBufferWRMainLoop[PMP_BUFFER_SIZE] = 
{
    0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,
};
ePMPSelect_t u16PMPChipSelect = PMP_CS_1;  
                                
uint16_t u16PMPSizeWR = 8;
uint16_t u16PMPSizeRD = 16;
uint16_t u16PMPSizeCLR = 16;

uintPMP_t* pPMPDataWRMainLoop = &udPMPBufferWRMainLoop[0];
uintPMP_t* pPMPDataRDMainRW = &udPMPBufferRDMainRW[0];
uintPMP_t* pPMPDataWRMainRW = &udPMPBufferWRMainRW[0];
uintPMP_t* pPMPDataRD = &udPMPBufferRD[0];
uintPMP_t* pPMPDataWR = &udPMPBufferWR[0];

volatile ePMPCmd_t ePMPCmd = PMP_CMD_IDLE;

volatile ePMPCmd_t ePMPCmdReserve = PMP_CMD_IDLE;

bool bUsePMPQueue = true;

#define PMP_QUEUE_SIZE  8
EDS_SPACE_QUEUE QUEUE_sQueue_t sPMPQueue;
EDS_SPACE_QUEUE QUEUE_sElement_t asPMPQueueElements[PMP_QUEUE_SIZE];  

EDS_SPACE_QUEUE QUEUE_sElement_t* psPMPQueueElementInProcessRW = NULL;    

EDS_SPACE_QUEUE QUEUE_sElement_t* psPMPQueueElementRWExtMemRamToIntRam = NULL;

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

void PMPMEM_vTransmitCompleteQueueProcess(void);
void PMPMEM_vTrySendFromQueue(void);

void PMPMEM_vTrySendFromQueueMainLoop(void);














/* *****************************************************************************
 * PMPMEM_vInit
 * 
 * Description: PMPMEM Init Function
 **************************************************************************** */
void PMPMEM_vInit(void) 
{    
    PMP_vInit(eUsePMPDMA);
    
    if (eUsePMPDMA)  
    {
        PMP_vInitDMA(eModuleDMAPMP);
        ISR_vSetISRDMA(PMPMEM_vTransmitCompleteQueueProcess, eModuleDMAPMP);        /* Re-register ISR_vSetISRDMA. Previous register in PMP_vInitDMA of PMP_vDMACompleteProcess */
    }
    QUEUE_vInit(&sPMPQueue, (EDS_SPACE_QUEUE QUEUE_sElement_t*)&asPMPQueueElements[0], PMP_QUEUE_SIZE);
}


EDS_SPACE_QUEUE QUEUE_sElement_t* PMPMEM_vTryQueue(QUEUE_eCommand_t eCommand, ePMPSelect_t ePMPSelect, uint16_t u16Address, uintPMP_t* pdData, uint16_t u16Size, bool bUseHeap)
{
    EDS_SPACE_QUEUE QUEUE_sElement_t* psQueueElement = QUEUE_vGetElementFillNext(&sPMPQueue);

        //DEBUG_SET_MAIN();
        //DEBUG_CLR_MAIN();
    if (psQueueElement != NULL)
    {
        /* place in queue reserved */
        psQueueElement->eCommand = eCommand;
        psQueueElement->uAddress.sAddress.u16Lo = u16Address;
        psQueueElement->u16ChipAddress = ePMPSelect;
        psQueueElement->u16Size = u16Size;
        psQueueElement->pData = pdData;
        psQueueElement->eStatus = QUEUE_STA_FULL;
        psQueueElement->uFlags.sFlags.bUseHeapBuffer = bUseHeap;
        
        /* Try To Send From Queue */
                   //DEBUG_SET_MAIN2();

        PMPMEM_vTrySendFromQueueMainLoop();
                   //DEBUG_CLR_MAIN2();

    }
    return psQueueElement;
}

void PMPMEM_vReleaseQueueBuffer(uint8_t* pBuffer)
{
    if ((psPMPQueueElementRWExtMemRamToIntRam) 
            && (psPMPQueueElementRWExtMemRamToIntRam->pData == pBuffer)
            && (psPMPQueueElementRWExtMemRamToIntRam->eStatus == QUEUE_STA_READY))
    {
        QUEUE_vSetElementCompleted(psPMPQueueElementRWExtMemRamToIntRam);
        QUEUE_vSetElementFreeTry(&sPMPQueue,psPMPQueueElementRWExtMemRamToIntRam);
    }
}

volatile uint8_t CounterDMAISR = 0;

void PMPMEM_vTransmitCompleteQueueProcess(void)
{
    
    //send data from PMP queue. This function to be registered on DMA isr
    
    if (bUseDirectCopyRD == true) 
    {
        
        QUEUE_vSetLastElementSentReady(&sPMPQueue); //for faster execution sPMPQueue.psQueueElementLastSent->eStatus = QUEUE_STA_READY;//
        if (sPMPQueue.psQueueElementLastSent->eCommand == QUEUE_CMD_READ)
        {
            QUEUE_vSetElementCompleted(sPMPQueue.psQueueElementLastSent); //for faster execution sPMPQueue.psQueueElementLastSent->eStatus = QUEUE_STA_COMPLETED;//
        }
        QUEUE_vSetElementFreeTryISR(&sPMPQueue,sPMPQueue.psQueueElementLastSent);//QUEUE_vSetElementFree(&sPMPQueue,sPMPQueue.psQueueElementLastSent);
    }
    else
    {
        QUEUE_vSetLastElementSentReady(&sPMPQueue);  
    }

    
    /* process data from/to PMP if needed */
    
    //QUEUE_vSetLastElementSentCompleted(&sPMPQueue);  
    PMP_vDMACompleteProcess();  /* old flag system support */
    /* Try To Send From Queue */
    PMPMEM_vTrySendFromQueue();
}





void PMPMEM_vTrySendFromQueue(void)
{
    EDS_SPACE_QUEUE QUEUE_sElement_t* psQueueElement;

    //if(PMP_bBusyStatus() == false) not needed here in isr
    {
    
        //DEBUG_SET_MAIN2();
        psQueueElement = QUEUE_vGetElementSendNextISR(&sPMPQueue);
        //DEBUG_CLR_MAIN2();

        if (psQueueElement != NULL)
        {
            {
                if(psQueueElement->eCommand == QUEUE_CMD_READ)
                {
                    (void)PMP_vRead(psQueueElement->u16ChipAddress, psQueueElement->uAddress.sAddress.u16Lo, psQueueElement->pData, psQueueElement->u16Size, psQueueElement->uFlags.sFlags.bUseHeapBuffer);
                }
                else if(psQueueElement->eCommand == QUEUE_CMD_WRITE)
                {
                   (void)PMP_vWrite(psQueueElement->u16ChipAddress, psQueueElement->uAddress.sAddress.u16Lo, psQueueElement->pData, psQueueElement->u16Size, psQueueElement->uFlags.sFlags.bUseHeapBuffer);
                }
            } 
        }
        
    }
    //ISR_vExitCritical();
}

void PMPMEM_vTrySendFromQueueMainLoop(void)
{
    EDS_SPACE_QUEUE QUEUE_sElement_t* psQueueElement;

    if(PMP_bBusyStatus() == false)
    {

        ISR_vEnterCritical();
    
        //DEBUG_SET_MAIN2();
        psQueueElement = QUEUE_vGetElementSendNext(&sPMPQueue);
        //DEBUG_CLR_MAIN2();

        if (psQueueElement != NULL)
        {
            {
                if(psQueueElement->eCommand == QUEUE_CMD_READ)
                {
                    if (PMP_vRead(psQueueElement->u16ChipAddress, psQueueElement->uAddress.sAddress.u16Lo, psQueueElement->pData, psQueueElement->u16Size, psQueueElement->uFlags.sFlags.bUseHeapBuffer) == false)
                    {
                        //QUEUE_vSetLastElementSentReady(&sPMPQueue);
                    }
                }
                else if(psQueueElement->eCommand == QUEUE_CMD_WRITE)
                {

                    if (PMP_vWrite(psQueueElement->u16ChipAddress, psQueueElement->uAddress.sAddress.u16Lo, psQueueElement->pData, psQueueElement->u16Size, psQueueElement->uFlags.sFlags.bUseHeapBuffer) == false)
                    {
                        //QUEUE_vSetLastElementSentReady(&sPMPQueue);
                    }
                }
            } 
        }
        ISR_vExitCritical();
    }
    
}

HMDRV_U8* pHeapBufferMainLoop = NULL;
HMDRV_U16 indexCopy;

void PMPMEM_vProcessRW(void)
{
    #if PMP_FAST_RW == 0
    HMDRV_U8* pHeapBuffer;
    HMDRV_BOOL bUseHeap = false;
    #endif
    
    if (ePMPCmdReserve == PMP_CMD_IDLE)
    {
        switch (ePMPCmd)
        {
            case PMP_CMD_IDLE:
                //psPMPQueueElementInProcessRW = NULL;
                break;
            case PMP_CMD_WR:
                
                #if PMP_FAST_RW
                PMP_vWriteDefault(u16PMPAddress, pPMPDataWRMainRW, u16PMPSizeWR);  
                ePMPCmd = PMP_CMD_IDLE;
                #else
                
                if (bUseDynamicMemoryWR)
                {
                    bUseHeap = true;
                    pHeapBuffer = MEMORY_pAlloc(u16PMPSizeWR);
                }
                else
                {
                    pHeapBuffer = pPMPDataWRMainRW;
                }

                if ((pHeapBuffer != NULL) && (pHeapBuffer != pPMPDataWRMainRW))
                {
                    for (indexCopy = 0; indexCopy < u16PMPSizeWR; indexCopy++)
                    {
                        *(pHeapBuffer+indexCopy) = *(pPMPDataWRMainRW+indexCopy);
                    }
                    
                }
                
                
                if (bUsePMPQueue)
                {
                    psPMPQueueElementInProcessRW = PMPMEM_vTryQueue(QUEUE_CMD_WRITE, u16PMPChipSelect, u16PMPAddress, pHeapBuffer , u16PMPSizeWR, bUseHeap);
                    if (psPMPQueueElementInProcessRW)
                    {
                        ePMPCmd = PMP_CMD_BUSY;
                    }
                }
                else
                {
                    if (PMP_bBusyStatus() == false)
                    {
                        PMP_vWrite(u16PMPChipSelect, u16PMPAddress, pHeapBuffer , u16PMPSizeWR, bUseHeap);
                        ePMPCmd = PMP_CMD_BUSY;
                    }
                }
                #endif
                break;
            case PMP_CMD_RD:
                #if PMP_FAST_RW
                PMP_vReadDefault(u16PMPAddress, pPMPDataRDMainRW, u16PMPSizeRD);  
                ePMPCmd = PMP_CMD_IDLE;
                #else
                if (bUseDynamicMemoryRD)
                {
                    bUseHeap = true;
                    pHeapBuffer = MEMORY_pAlloc(u16PMPSizeRD);
                }
                else
                {
                    pHeapBuffer = pPMPDataRDMainRW;
                }
                
                if (bUsePMPQueue)
                {
                    psPMPQueueElementInProcessRW = PMPMEM_vTryQueue(QUEUE_CMD_READ, u16PMPChipSelect, u16PMPAddress, pHeapBuffer , u16PMPSizeRD, bUseHeap);
                    if (psPMPQueueElementInProcessRW)
                    {
                        pHeapBufferMainLoop = pHeapBuffer;
                        ePMPCmd = PMP_CMD_BUSY;
                    }
                }
                else
                {
                    if (PMP_bBusyStatus() == false)
                    {
                        pHeapBufferMainLoop = pHeapBuffer;
                        PMP_vRead (u16PMPChipSelect, u16PMPAddress, pHeapBuffer , u16PMPSizeRD, bUseHeap);
                        ePMPCmd = PMP_CMD_BUSY;
                    }
                }
                #endif
                break;
            case PMP_CMD_CLR:
                #if PMP_FAST_RW
                PMP_vClearDefault(u16PMPAddress, u16PMPSizeWR);  
                ePMPCmd = PMP_CMD_IDLE;
                #else
                if (bUsePMPQueue)
                {
                    psPMPQueueElementInProcessRW = PMPMEM_vTryQueue(QUEUE_CMD_WRITE, u16PMPChipSelect, u16PMPAddress, NULL , u16PMPSizeCLR, bUseHeap);
                    if (psPMPQueueElementInProcessRW)
                    {
                        ePMPCmd = PMP_CMD_BUSY;
                    }
                }
                else
                {
                    if (PMP_bBusyStatus() == false)
                    {
                        PMP_vWrite(u16PMPChipSelect, u16PMPAddress, NULL , u16PMPSizeCLR, bUseHeap);
                        ePMPCmd = PMP_CMD_BUSY;
                    }
                }
                #endif
                break;

            case PMP_CMD_BUSY:
                #if PMP_FAST_RW
                ePMPCmd = PMP_CMD_IDLE;
                #else
                if (bUsePMPQueue)
                {
                    if (psPMPQueueElementInProcessRW->eCommand != QUEUE_CMD_READ)
                    {
                        ePMPCmd = PMP_CMD_IDLE;
                    }
                    else
                    if ((psPMPQueueElementInProcessRW) && (psPMPQueueElementInProcessRW->eStatus == QUEUE_STA_READY))
                    {
                        if ((pHeapBufferMainLoop != NULL) && (pHeapBufferMainLoop != pPMPDataRDMainRW))
                        {
                            /* copy to pPMPDataRD */
                            for (indexCopy = 0; indexCopy < u16PMPSizeRD; indexCopy++)
                            {
                                *(pPMPDataRDMainRW+indexCopy) = *(pHeapBufferMainLoop+indexCopy);
                            }
                            pHeapBufferMainLoop = NULL;
                        }
                        #if 1
                        QUEUE_vSetElementCompleted(psPMPQueueElementInProcessRW);
                        QUEUE_vSetElementFreeTry(&sPMPQueue, psPMPQueueElementInProcessRW);
                        #else
                        QUEUE_vSetElementFreeForce(&sPMPQueue, psPMPQueueElementInProcessRW);  
                        #endif
                        //psPMPQueueElementInProcessRW = NULL;
                        ePMPCmd = PMP_CMD_IDLE;
                    }
                    
                }
                else
                {
                    
                    if (PMP_bBusyStatus() == false)
                    {
                        if ((pHeapBufferMainLoop != NULL) && (pHeapBufferMainLoop != pPMPDataRDMainRW))
                        {
                            /* copy to pPMPDataRD */
                            for (indexCopy = 0; indexCopy < u16PMPSizeRD; indexCopy++)
                            {
                                *(pPMPDataRDMainRW+indexCopy) = *(pHeapBufferMainLoop+indexCopy);
                            }
                            if (bUseDynamicMemoryRD)
                            {
                                
                                MEMORY_vFree(pHeapBufferMainLoop);
                            }
                            pHeapBufferMainLoop = NULL;
                        }
                    
                        ePMPCmd = PMP_CMD_IDLE;
                    }
                }
                #endif
                break;

            default:
                break;
        }
    }
}



/* *****************************************************************************
 * PMPMEM_vCopyToExtMem
 * 
 * Description: HMLIB Compatible copy function:
 * HMDRV_BPTR HMDRV_CopyFromBuffer         (HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
 **************************************************************************** */
void PMPMEM_vCopyToExtMem(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_U16 addr = (HMDRV_U16)(HMDRV_ADDR_SIZE) nDestAddr;
    HMDRV_U8* pHeapBuffer;
    HMDRV_U8* pb;
    HMDRV_SIZE8 size = nSize;
    HMDRV_BOOL bUseHeap = false;
    
    
    
    if (bUseDirectCopyWR)
    {
        pHeapBuffer = pSrcBuff;
        
    }
    else
    {
    
    
        if (bUseDynamicMemoryWR)
        {
            bUseHeap = true;
            pHeapBuffer = MEMORY_pAlloc(nSize);
        }
        else
        {
            pHeapBuffer = pPMPDataWRMainLoop;
        }
        pb = pHeapBuffer;
    }
    
    if (bUseDirectCopyWR == false)
    {
        while(nSize--)
            *pb++ = *ps++;

        nSize = size;
    }
    
    if (bUsePMPQueue)
    {
            //DEBUG_SET_MAIN2();
        while(PMPMEM_vTryQueue(QUEUE_CMD_WRITE, u16PMPChipSelect, addr, pHeapBuffer , nSize, bUseHeap) == NULL)
        {
           //DEBUG_CLR_MAIN2();
            QUEUE_vSetElementFreeProcess(&sPMPQueue);
           //DEBUG_SET_MAIN2();
        };
            //DEBUG_CLR_MAIN2();
    }
    else
    {
        while(PMP_bBusyStatus()){;}/* wait pending transfer */
         PMP_vWrite(u16PMPChipSelect, addr, pHeapBuffer , nSize, bUseHeap);
    }
    
    
}


/* *****************************************************************************
 * PMPMEM_vCopyToExtMemWithMask
 * 
 * Description: HMLIB Compatible copy function:
 * void HMDRV_CopyFromBufferWithMask(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
 **************************************************************************** */
void PMPMEM_vCopyToExtMemWithMask(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_U16 addr = (HMDRV_U16)(HMDRV_ADDR_SIZE) nDestAddr;
    HMDRV_U8* pm = ps + nSize;
    HMDRV_U8 mask, stmp, dtmp;
    
    HMDRV_U8* pbRD;
    HMDRV_U8* pbWR;
    HMDRV_SIZE8 size = nSize;

    HMDRV_U8* pHeapBufferRD;
    HMDRV_U8* pHeapBufferWR;
    HMDRV_BOOL bUseHeapRD = false;
    HMDRV_BOOL bUseHeapWR = false;
    #if PMP_FAST_RW == 0
    EDS_SPACE_QUEUE QUEUE_sElement_t* psPMPQueueElementRW = NULL;
    HMDRV_BOOL useQueue = bUsePMPQueue;
    #endif
    
    if (bUseDynamicMemoryRD)
    {
        bUseHeapRD = true;
        pHeapBufferRD = MEMORY_pAlloc(nSize);
    }
    else
    {
        pHeapBufferRD = pPMPDataRD;
    }

    if (bUseDynamicMemoryWR)
    {
        bUseHeapWR = true;
        pHeapBufferWR = MEMORY_pAlloc(nSize);
    }
    else
    {
        pHeapBufferWR = pPMPDataWR;
    }

    pbRD = pHeapBufferRD;
    pbWR = pHeapBufferWR;
    
    #if PMP_FAST_RW == 0 


    /* can implement reserve buffer and wait buffer ready before use the data after use data - release the buffer  */
    if (useQueue)
    {
        do
        {
            psPMPQueueElementRW = PMPMEM_vTryQueue(QUEUE_CMD_READ, u16PMPChipSelect, addr, pHeapBufferRD , nSize, bUseHeapRD);
            if (psPMPQueueElementRW == NULL)
            {
                QUEUE_vSetElementFreeProcess(&sPMPQueue);
            }
        }
        while(psPMPQueueElementRW == NULL);
        
        do
        {
             ;/* wait read */
        }
        while(psPMPQueueElementRW->eStatus != QUEUE_STA_READY);
    }
    else
    {
        while(PMP_bBusyStatus()){;}/* wait pending transfer */
        PMP_vRead(u16PMPChipSelect, addr, pHeapBufferRD , nSize, bUseHeapRD);
        while(PMP_bBusyStatus()){;}/* wait pending transfer */
    }
    #else
    PMP_vReadDefault(addr, pbRD, nSize);  
    #endif


    
    while(nSize--)
    {
        mask = *pm++;
        stmp = *ps++;
        dtmp = *pbRD++;

        /* perform AND-masking */
        stmp = (HMDRV_U8) ((stmp & mask) | (dtmp & ~mask));

        /* put the result in write buffer */
        *pbWR++ = stmp;
    }
    nSize = size;
    
    
    #if PMP_FAST_RW == 0
    if (useQueue)
    {
        
        QUEUE_vSetElementCompleted(psPMPQueueElementRW);
        QUEUE_vSetElementFreeTry(&sPMPQueue,psPMPQueueElementRW);
        do
        {
            psPMPQueueElementRW = PMPMEM_vTryQueue(QUEUE_CMD_WRITE, u16PMPChipSelect, addr, pHeapBufferWR , nSize, bUseHeapWR);
            if (psPMPQueueElementRW == NULL)
            {
                QUEUE_vSetElementFreeProcess(&sPMPQueue);
            }
        }
        while(psPMPQueueElementRW == NULL);
    }
    else
    {
        while(PMP_bBusyStatus()){;}/* wait pending transfer */
        if (bUseDynamicMemoryRD)
        {
            MEMORY_vFree(pHeapBufferRD);
        }
        PMP_vWrite(u16PMPChipSelect, addr, pHeapBufferWR , nSize, bUseHeapWR);
    }
    #else
        if (bUseDynamicMemoryRD)
        {
            MEMORY_vFree(pHeapBufferRD);
        }
    PMP_vWriteDefault(addr, pbWR, nSize);  
        if (bUseDynamicMemoryWR)
        {
            MEMORY_vFree(pHeapBufferWR);
        }
    #endif
    
}


/* *****************************************************************************
 * PMPMEM_vExtMemRamToIntRam
 * 
 * Description: Copy Data from external memory to internal RAM and 
 *              returns pointer to internal RAM
 **************************************************************************** */
HMDRV_ADDR PMPMEM_pExtMemRamToIntRam(HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_U16 addr = (HMDRV_U16)(HMDRV_ADDR_SIZE) nSrcAddr;
    HMDRV_U8* pHeapBufferRD;
    HMDRV_U8* ps;
    #if PMP_FAST_RW == 0
    EDS_SPACE_QUEUE QUEUE_sElement_t* psPMPQueueElementRW;
    #endif
    HMDRV_BOOL bUseHeap = false;
	
    
    
    if (bUseDynamicMemoryRD)
    {
        bUseHeap = true;
        pHeapBufferRD = MEMORY_pAlloc(nSize);
    }
    else
    {
        pHeapBufferRD = pPMPDataRD;
    }
    ps = pHeapBufferRD;

    #if PMP_FAST_RW == 0
    /* can implement reserve buffer and wait buffer ready before use the data after use data - release the buffer  */
    if (bUsePMPQueue)
    {
        do
        {
            psPMPQueueElementRW = PMPMEM_vTryQueue(QUEUE_CMD_READ, u16PMPChipSelect, addr, pHeapBufferRD , nSize, bUseHeap);
            if (psPMPQueueElementRW == NULL)
            {
                QUEUE_vSetElementFreeProcess(&sPMPQueue);
            }
        }
        while(psPMPQueueElementRW == NULL);
        
        do
        {
             ;/* wait read */
        }
        while(psPMPQueueElementRW->eStatus != QUEUE_STA_READY);
        psPMPQueueElementRWExtMemRamToIntRam = psPMPQueueElementRW;
    }
    else
    {
        while(PMP_bBusyStatus()){;}/* wait pending transfer */
        PMP_vRead(u16PMPChipSelect, addr, pHeapBufferRD , nSize, bUseHeap);
        while(PMP_bBusyStatus()){;}/* wait pending transfer */
    }
    #else
    PMP_vReadDefault(addr, ps, nSize);  
    #endif
    return (HMDRV_ADDR)(HMDRV_ADDR_SIZE)ps;  /* need to free dinamic data after use (ps) */
}

/* *****************************************************************************
 * PMPMEM_pExtMemRamFromIntRam
 * 
 * Description: returns pointer to internal RAM for 
 *              Copy Data to external memory from internal RAM 
 *              
 **************************************************************************** */
HMDRV_ADDR PMPMEM_pExtMemRamFromIntRam(HMDRV_SIZE8 nSize)
{
    HMDRV_U8* pb;
    HMDRV_U8* pHeapBufferWR;
    if (bUseDynamicMemoryWR)
    {
        pHeapBufferWR = MEMORY_pAlloc(nSize);
    }
    else
    {
        pHeapBufferWR = pPMPDataWR;
    }
    pb = pHeapBufferWR;
    return (HMDRV_ADDR)(HMDRV_ADDR_SIZE)pb;
}

/* *****************************************************************************
 * PMPMEM_vExtMemRamFromIntRamCopyData
 * 
 * Description: Copy Data to external memory from already filled internal RAM buffer
 **************************************************************************** */
void PMPMEM_vExtMemRamFromIntRamCopyData(HMDRV_ADDR nInternalRAMBufferAddr, HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_U16 addr = (HMDRV_U16)(HMDRV_ADDR_SIZE) nDestAddr;
    
    
    #if PMP_FAST_RW == 0
    EDS_SPACE_QUEUE QUEUE_sElement_t* psPMPQueueElementRW = NULL;
    HMDRV_BOOL bUseHeap = true;
    #endif

    
    //HMDRV_U8* pb = pPMPDataWR;
    //while(nSize--)
    //    *pb++ = *ps++;    

    HMDRV_U8* pHeapBufferWR;

    pHeapBufferWR = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nInternalRAMBufferAddr;
    
    #if PMP_FAST_RW == 0
    if (bUsePMPQueue)
    {     
        do
        {
            psPMPQueueElementRW = PMPMEM_vTryQueue(QUEUE_CMD_WRITE, u16PMPChipSelect, addr, pHeapBufferWR , nSize, bUseHeap);
            if (psPMPQueueElementRW == NULL)
            {
                QUEUE_vSetElementFreeProcess(&sPMPQueue);
            }
        }
        while(psPMPQueueElementRW == NULL);
    }
    else
    {
        while(PMP_bBusyStatus()){;}/* wait pending transfer */
        PMP_vWrite(u16PMPChipSelect, addr, pHeapBufferWR , nSize, bUseHeap);
    }
    #else
    PMP_vWriteDefault(addr, pHeapBufferWR, nSize);  
    if (bUseDynamicMemoryWR)
    {
        MEMORY_vFree(pHeapBufferWR);
    }
    #endif
}

/* *****************************************************************************
 * PMPMEM_vCopyFromExtMem
 * 
 * Description: HMLIB Compatible copy function:
 * HMDRV_BPTR HMDRV_CopyToBuffer(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
 **************************************************************************** */
void PMPMEM_vCopyFromExtMem(HMDRV_BPTR pDestAddr, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_U16 addr = (HMDRV_U16)(HMDRV_ADDR_SIZE) nSrcAddr;
    HMDRV_U8* pd = (HMDRV_U8*)pDestAddr;
    HMDRV_U8* pHeapBufferRD;
    HMDRV_U8* ps = NULL;
    EDS_SPACE_QUEUE QUEUE_sElement_t* psPMPQueueElementRW = NULL;
    HMDRV_BOOL useQueue = bUsePMPQueue;
    HMDRV_BOOL bUseHeap = false;
	
    if (bUseDirectCopyRD)
    {
        pHeapBufferRD = pDestAddr;
    }
    else
    {
        if (bUseDynamicMemoryRD)
        {
            bUseHeap = true;
            pHeapBufferRD = MEMORY_pAlloc(nSize);
        }
        else
        {
            pHeapBufferRD = pPMPDataRD;
        }
        ps = pHeapBufferRD;
    }
    /* can implement reserve buffer and wait buffer ready before use the data after use data - release the buffer  */
    if (useQueue)
    {
        do
        {
            psPMPQueueElementRW = PMPMEM_vTryQueue(QUEUE_CMD_READ, u16PMPChipSelect, addr, pHeapBufferRD , nSize, bUseHeap);
            if (psPMPQueueElementRW == NULL)
            {
                QUEUE_vSetElementFreeProcess(&sPMPQueue);
            }           
        }
        while(psPMPQueueElementRW == NULL);
        

        do
        {
             ;/* wait read */
        }
        while((bUseDirectCopyRD == false)&&(psPMPQueueElementRW->eStatus != QUEUE_STA_READY));
    }
    else
    {
        while(PMP_bBusyStatus()){;}/* wait pending transfer */
        PMP_vRead(u16PMPChipSelect, addr, pHeapBufferRD , nSize, bUseHeap);
        while((bUseDirectCopyRD == false) && PMP_bBusyStatus()){;}/* wait pending transfer */
    }
    if (bUseDirectCopyRD == false)
    {
        while(nSize--)
            *pd++ = *ps++;
    }
    
    if(useQueue)
    {
        if (bUseDirectCopyRD == false)
        {
            QUEUE_vSetElementCompleted(psPMPQueueElementRW);
            QUEUE_vSetElementFreeTry(&sPMPQueue,psPMPQueueElementRW);
        }
    }
    else
    {
        if (bUseDynamicMemoryRD)
        {
            MEMORY_vFree(pHeapBufferRD);
        }      
    }
}  

/* *****************************************************************************
 * PMPMEM_pBufferAddressGet
 * PMPMEM_vSendBufferToPMP
 * 
 * Description: Functions used with the Recorder Function
 **************************************************************************** */
uint16_t u16PMPTargetAddress;
HMDRV_U8* pPMPTargetHeapBufferWR;

HMDRV_ADDR PMPMEM_pBufferAddressGet(HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_U8* pResult;
    HMDRV_U8* pHeapBufferWR;

    while(PMP_bBusyStatus()){;}/* wait pending transfer */
    ePMPCmdReserve = PMP_CMD_BUSY;

    if (bUseDynamicMemoryWR)
    {
        pHeapBufferWR = MEMORY_pAlloc(nSize);
    }
    else
    {
        pHeapBufferWR = pPMPDataWR;
    }
    pResult = pHeapBufferWR;
    
    u16PMPTargetAddress = (uint16_t)(HMDRV_ADDR_SIZE)nDestAddr;
    pPMPTargetHeapBufferWR = pResult;


    return (HMDRV_ADDR)(HMDRV_ADDR_SIZE)pResult;
}


void PMPMEM_vSendBufferToPMP(HMDRV_SIZE8 nSize)
{
    
    HMDRV_U8* pHeapBufferWR;
    #if PMP_FAST_RW == 0
    HMDRV_BOOL bUseHeap = true;
    #endif
    
    pHeapBufferWR = pPMPTargetHeapBufferWR;
    
    #if PMP_FAST_RW == 0
    EDS_SPACE_QUEUE QUEUE_sElement_t* psPMPQueueElementRW = NULL;
    if (bUsePMPQueue)
    {
        do
        {
            psPMPQueueElementRW = PMPMEM_vTryQueue(QUEUE_CMD_WRITE, u16PMPChipSelect, u16PMPTargetAddress, pHeapBufferWR , nSize, bUseHeap);
            if (psPMPQueueElementRW == NULL)
            {
                QUEUE_vSetElementFreeProcess(&sPMPQueue);
            }
        }
        while(psPMPQueueElementRW == NULL);
    }
    else
    {
        while(PMP_bBusyStatus()){;}/* wait pending transfer */
        PMP_vWrite(u16PMPChipSelect, u16PMPTargetAddress, pHeapBufferWR , nSize, bUseHeap);
        ePMPCmdReserve = PMP_CMD_IDLE;
    }
    #else
    PMP_vWriteDefault(u16PMPTargetAddress, pHeapBufferWR, nSize);  
    if (bUseDynamicMemoryWR)
    {
        MEMORY_vFree(pHeapBufferWR);
    }
    #endif
}

