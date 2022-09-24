/* *****************************************************************************
 * File:   queue.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 06 19 10:58
 * 
 * Description: Communication Queue Management
 * 
 **************************************************************************** */



/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "isr.h"
#include "boardcfg.h"
#include "queue.h"
#if USE_DYNAMIC_MEMORY_RW
#include "memory.h"
#include "pmp.h"
#endif

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions Prototypes
 **************************************************************************** */


/* *****************************************************************************
 * Functions Definitions
 **************************************************************************** */



/* *****************************************************************************
 * QUEUE_vInit
 * 
 * Description: Initialize Queue
 **************************************************************************** */
void QUEUE_vInit(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue, EDS_SPACE_QUEUE QUEUE_sElement_t* pQueueElements, uint16_t nCountElements)
{
    pQueue->psQueueElementNextSend = pQueueElements;
    pQueue->psQueueElementNextFree = pQueueElements;
    pQueue->psQueueElementNextFill = pQueueElements;
    pQueue->psQueueElementLastSent = NULL;
    pQueue->psQueueElementLastFill = NULL;
    
    pQueue->psQueueElementStart = pQueueElements;
    pQueue->psQueueElementFinal = pQueueElements;
    pQueue->psQueueElementFinal += nCountElements;  /* End Invalid address */
    while(nCountElements-- > 0)
    {
        pQueueElements->eStatus = QUEUE_STA_IDLE;
        pQueueElements++;
    }
}


/* *****************************************************************************
 * QUEUE_vGetElementFillNext
 * 
 * Description: Extract and mark Reserved Next Queue Element To Fill Data In
 **************************************************************************** */
EDS_SPACE_QUEUE QUEUE_sElement_t* QUEUE_vGetElementFillNext(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue)
{
    EDS_SPACE_QUEUE QUEUE_sElement_t* psResult = (EDS_SPACE_QUEUE QUEUE_sElement_t*)NULL;   /* Default Invalid Pointer */
    EDS_SPACE_QUEUE QUEUE_sElement_t* psFindNextLoop = (EDS_SPACE_QUEUE QUEUE_sElement_t*)pQueue->psQueueElementNextFill;
    
    //DEBUG_SET_MAIN2();

    if (psFindNextLoop)
    {
        ISR_vEnterCritical();
        do
        {
            //DEBUG_SET_MAIN2();
            if(psFindNextLoop->eStatus == QUEUE_STA_IDLE)
            {
                psResult = psFindNextLoop;
                psResult->eStatus = QUEUE_STA_FILL;
                pQueue->psQueueElementLastFill = psResult;
                if (pQueue->psQueueElementNextFill == psResult)
                {
                    pQueue->psQueueElementNextFill++;
                    if(pQueue->psQueueElementNextFill >= pQueue->psQueueElementFinal)
                    {
                        pQueue->psQueueElementNextFill = pQueue->psQueueElementStart;
                    }
                }                
            }
            else
            {
                psFindNextLoop++;
                if (psFindNextLoop >= pQueue->psQueueElementFinal)
                {
                    psFindNextLoop = pQueue->psQueueElementStart;
                }
            }
            //DEBUG_CLR_MAIN2();
        }
        while ((psFindNextLoop != pQueue->psQueueElementNextFill) && (psResult == NULL));
        ISR_vExitCritical();
    }
    
    //DEBUG_CLR_MAIN2();

    return psResult;
}

#if 0
void QUEUE_vAdvanceElementNextFill(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue)
{  
    //ISR_vEnterCritical(); made in caller function
    while
        (  
          //  (pQueue->psQueueElementNextFill != pQueue->psQueueElementNextFill)
          //&&
            ( 
                (pQueue->psQueueElementNextFill->eStatus == QUEUE_STA_IDLE)
             || (pQueue->psQueueElementNextFill->eStatus == QUEUE_STA_COMPLETED)
             || (pQueue->psQueueElementNextFill->eStatus == QUEUE_STA_READY)
             || (pQueue->psQueueElementNextFill->eStatus == QUEUE_STA_SENDING )       
            )
        )
    {    
        pQueue->psQueueElementNextFill++;
        if (pQueue->psQueueElementNextFill >= pQueue->psQueueElementFinal)
        {
            pQueue->psQueueElementNextFill = pQueue->psQueueElementStart;
        }     
    }
    //ISR_vExitCritical();
}
#endif




#if 0
/* *****************************************************************************
 * QUEUE_vGetElementFillNextEmbeddedQueue
 * 
 * Description: Extract and mark Reserved Next Queue Element To Fill Data In
 *              This Next element must be obligatory after the previous element in the embedded queue
 **************************************************************************** */
EDS_SPACE_QUEUE QUEUE_sElement_t* QUEUE_vGetElementFillNextEmbeddedQueue(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue, EDS_SPACE_QUEUE QUEUE_sElement_t* pEmbeddedQueuePrevElement)
{
    EDS_SPACE_QUEUE QUEUE_sElement_t* psResult = NULL;   /* Default Invalid Pointer */
    EDS_SPACE_QUEUE QUEUE_sElement_t* psFindNextLoop = pEmbeddedQueuePrevElement;    /* no assessment made here for correct input !!! */
    
    if (psFindNextLoop)
    {
        ISR_vEnterCritical();
        do
        {
            if(psFindNextLoop->eStatus == QUEUE_STA_IDLE)
            {
                psResult = psFindNextLoop;
                psResult->eStatus = QUEUE_STA_FILL;
                pQueue->psQueueElementLastFill = psResult;
                if (pQueue->psQueueElementNextFill == psResult)
                {
                    pQueue->psQueueElementNextFill++;
                    if(pQueue->psQueueElementNextFill >= pQueue->psQueueElementFinal)
                    {
                        pQueue->psQueueElementNextFill = pQueue->psQueueElementStart;
                    }
                }
            }
            else
            {
                psFindNextLoop++;
                if (psFindNextLoop >= pQueue->psQueueElementFinal)
                {
                    psFindNextLoop = pQueue->psQueueElementStart;
                }
            }
        }
        //while ((psFindNextLoop != pQueue->psQueueElementNext) && (psResult == NULL));
        while ((psFindNextLoop != pEmbeddedQueuePrevElement) && (psResult == NULL));
        ISR_vExitCritical();
    }
    
    return psResult;
}
#endif
/* *****************************************************************************
 * QUEUE_vGetElementSendNext
 * 
 * Description: Extract Next Queue Element Ready To Send
 **************************************************************************** */
EDS_SPACE_QUEUE QUEUE_sElement_t* QUEUE_vGetElementSendNext(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue)
{
    EDS_SPACE_QUEUE QUEUE_sElement_t* psResult = (EDS_SPACE_QUEUE QUEUE_sElement_t*)NULL;   /* Default Invalid Pointer */
    EDS_SPACE_QUEUE QUEUE_sElement_t* psFindNextLoop = (EDS_SPACE_QUEUE QUEUE_sElement_t*)pQueue->psQueueElementNextSend;
    
    ISR_vEnterCritical();
    if (pQueue->eQueueStatus != QUEUE_STA_SENDING)
    {
        if (pQueue->psQueueElementNextFill != psFindNextLoop)
        {
            do
            {
                if(psFindNextLoop->eStatus == QUEUE_STA_FULL)
                {
                    psResult = psFindNextLoop;
                    psFindNextLoop->eStatus = QUEUE_STA_SENDING;
                    pQueue->psQueueElementLastSent = psFindNextLoop;
                    pQueue->eQueueStatus = QUEUE_STA_SENDING;

                    //QUEUE_vAdvanceElementNextSend(pQueue);
                    pQueue->psQueueElementNextSend++;
                    if (pQueue->psQueueElementNextSend >= pQueue->psQueueElementFinal)
                    {
                        pQueue->psQueueElementNextSend = pQueue->psQueueElementStart;
                    }     


                }
                psFindNextLoop++;
                if (psFindNextLoop >= pQueue->psQueueElementFinal)
                {
                    psFindNextLoop = pQueue->psQueueElementStart;
                }
            }
            while ((psFindNextLoop != pQueue->psQueueElementNextFill) && (psResult == NULL));
        }
    }
    ISR_vExitCritical();
    
    return psResult;
    
}


/* *****************************************************************************
 * QUEUE_vGetElementSendNext
 * 
 * Description: Extract Next Queue Element Ready To Send
 **************************************************************************** */
EDS_SPACE_QUEUE QUEUE_sElement_t* QUEUE_vGetElementSendNextISR(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue)
{
    EDS_SPACE_QUEUE QUEUE_sElement_t* psResult = (EDS_SPACE_QUEUE QUEUE_sElement_t*)NULL;   /* Default Invalid Pointer */
    EDS_SPACE_QUEUE QUEUE_sElement_t* psFindNextLoop = (EDS_SPACE_QUEUE QUEUE_sElement_t*)pQueue->psQueueElementNextSend;
    
    //ISR_vEnterCritical();
    if (pQueue->eQueueStatus != QUEUE_STA_SENDING)
    {
        if (pQueue->psQueueElementNextFill != psFindNextLoop)
        {
            do
            {
                if(psFindNextLoop->eStatus == QUEUE_STA_FULL)
                {
                    psResult = psFindNextLoop;
                    psFindNextLoop->eStatus = QUEUE_STA_SENDING;
                    pQueue->psQueueElementLastSent = psFindNextLoop;
                    pQueue->eQueueStatus = QUEUE_STA_SENDING;

                    //QUEUE_vAdvanceElementNextSend(pQueue);
                    pQueue->psQueueElementNextSend++;
                    if (pQueue->psQueueElementNextSend >= pQueue->psQueueElementFinal)
                    {
                        pQueue->psQueueElementNextSend = pQueue->psQueueElementStart;
                    }     


                }
                psFindNextLoop++;
                if (psFindNextLoop >= pQueue->psQueueElementFinal)
                {
                    psFindNextLoop = pQueue->psQueueElementStart;
                }
            }
            while ((psFindNextLoop != pQueue->psQueueElementNextFill) && (psResult == NULL));
        }
    }
    //ISR_vExitCritical();
    
    return psResult;
    
}






void QUEUE_vAdvanceElementNextSend(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue)
{ 

    //ISR_vEnterCritical(); made in caller function
    while
        (  
            (pQueue->psQueueElementNextSend != pQueue->psQueueElementNextFill)
          &&( 
                (pQueue->psQueueElementNextSend->eStatus == QUEUE_STA_IDLE)
             || (pQueue->psQueueElementNextSend->eStatus == QUEUE_STA_COMPLETED)
             || (pQueue->psQueueElementNextSend->eStatus == QUEUE_STA_READY)
             || (pQueue->psQueueElementNextSend->eStatus == QUEUE_STA_SENDING )       
            )
        )
    {    
        pQueue->psQueueElementNextSend++;
        if (pQueue->psQueueElementNextSend >= pQueue->psQueueElementFinal)
        {
            pQueue->psQueueElementNextSend = pQueue->psQueueElementStart;
        }     
    }
    //ISR_vExitCritical();

 
}





/* *****************************************************************************
 * QUEUE_vSetLastElementSentReady
 * 
 * Description: Last Element  Sent mark as ready
 **************************************************************************** */
void QUEUE_vSetLastElementSentReady(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue)
{
    if (pQueue->psQueueElementLastSent != NULL)
    {
        pQueue->psQueueElementLastSent->eStatus = QUEUE_STA_READY;
        QUEUE_vSetElementFreeTryISR(pQueue,pQueue->psQueueElementLastSent);
    }
    pQueue->eQueueStatus = QUEUE_STA_IDLE;
}

/* *****************************************************************************
 * QUEUE_vSetLastElementSentCompleted
 * 
 * Description: Last Element Sent Mark As Completed
 **************************************************************************** */
void QUEUE_vSetLastElementSentCompleted(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue)
{
    if (pQueue->psQueueElementLastSent != NULL)
    {
        pQueue->psQueueElementLastSent->eStatus = QUEUE_STA_COMPLETED;
        QUEUE_vSetElementFreeTryISR(pQueue,pQueue->psQueueElementLastSent);
    }
    pQueue->eQueueStatus = QUEUE_STA_IDLE;
}

/* *****************************************************************************
 * QUEUE_vSetElementReady
 * 
 * Description: Element Mark As Ready
 **************************************************************************** */
void QUEUE_vSetElementReady(EDS_SPACE_QUEUE QUEUE_sElement_t* psQueueElement)
{
    if (psQueueElement != NULL)
    {
        psQueueElement->eStatus = QUEUE_STA_READY;
    }
}


/* *****************************************************************************
 * QUEUE_vSetElementSentCompleted
 * 
 * Description: Element Mark As Completed
 **************************************************************************** */
void QUEUE_vSetElementCompleted(EDS_SPACE_QUEUE QUEUE_sElement_t* psQueueElement)
{
    if (psQueueElement != NULL)
    {
        psQueueElement->eStatus = QUEUE_STA_COMPLETED;
        
        if (psQueueElement->uFlags.sFlags.bUseHeapBuffer)
        {
            if (psQueueElement->pData != NULL)
            {
                MEMORY_vFree(psQueueElement->pData);
            }
        }
#if 0        
        if (bUseDirectCopyRD == false)
        {
            if (bUseDynamicMemoryRD)
            {
                if (psQueueElement->pData != NULL)
                {
                    MEMORY_vFree(psQueueElement->pData);
                }
            }
        }
#endif
    }
}

/* *****************************************************************************
 * QUEUE_vSetElementFree
 * 
 * Description: Element Mark As Completed
 **************************************************************************** */
void QUEUE_vSetElementFree(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue, EDS_SPACE_QUEUE QUEUE_sElement_t* psQueueElement)
{
    if (psQueueElement != NULL)
    {
        psQueueElement->eStatus = QUEUE_STA_IDLE;
        pQueue->psQueueElementNextFree++;
        if (pQueue->psQueueElementNextFree >= pQueue->psQueueElementFinal)
        {
            pQueue->psQueueElementNextFree = pQueue->psQueueElementStart;
        }     
    }
}

/* *****************************************************************************
 * QUEUE_vSetElementFreeTry
 * 
 * Description: Release Element Try
 **************************************************************************** */
void QUEUE_vSetElementFreeTry(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue, EDS_SPACE_QUEUE QUEUE_sElement_t* pQueueElement)
{
    if  (   (pQueueElement->eStatus == QUEUE_STA_COMPLETED)
         || (   (pQueueElement->eStatus  == QUEUE_STA_READY)
             && (pQueueElement->eCommand != QUEUE_CMD_READ )   
            )    
        )
    {
        pQueueElement->eStatus = QUEUE_STA_IDLE;
        ISR_vEnterCritical();
        //QUEUE_vAdvanceElementNextFree(pQueue);
        pQueue->psQueueElementNextFree++;
        if (pQueue->psQueueElementNextFree >= pQueue->psQueueElementFinal)
        {
            pQueue->psQueueElementNextFree = pQueue->psQueueElementStart;
        }     
        ISR_vExitCritical();
    }      
}

/* *****************************************************************************
 * QUEUE_vSetElementFreeTry
 * 
 * Description: Release Element Try
 **************************************************************************** */
void QUEUE_vSetElementFreeTryISR(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue, EDS_SPACE_QUEUE QUEUE_sElement_t* pQueueElement)
{
    if  (   (pQueueElement->eStatus == QUEUE_STA_COMPLETED)
         || (   (pQueueElement->eStatus  == QUEUE_STA_READY)
             && (pQueueElement->eCommand != QUEUE_CMD_READ )   
            )    
        )
    {
        pQueueElement->eStatus = QUEUE_STA_IDLE;
        //ISR_vEnterCritical();
        //QUEUE_vAdvanceElementNextFree(pQueue);
        pQueue->psQueueElementNextFree++;
        if (pQueue->psQueueElementNextFree >= pQueue->psQueueElementFinal)
        {
            pQueue->psQueueElementNextFree = pQueue->psQueueElementStart;
        }     
        //ISR_vExitCritical();
    }      
}

void QUEUE_vAdvanceElementNextFree(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue)
{  
    ISR_vEnterCritical();
    while
        (  
            (pQueue->psQueueElementNextFree != pQueue->psQueueElementNextSend)
          &&( 
                (pQueue->psQueueElementNextFree->eStatus == QUEUE_STA_IDLE)
             || (pQueue->psQueueElementNextFree->eStatus == QUEUE_STA_COMPLETED)
             || (   (pQueue->psQueueElementNextFree->eStatus  == QUEUE_STA_READY)
                 && (pQueue->psQueueElementNextFree->eCommand != QUEUE_CMD_READ )   
                )    
            )
        )
    {    
        pQueue->psQueueElementNextFree++;
        if (pQueue->psQueueElementNextFree >= pQueue->psQueueElementFinal)
        {
            pQueue->psQueueElementNextFree = pQueue->psQueueElementStart;
        }     
    }
    ISR_vExitCritical();
}


#if 0
/* *****************************************************************************
 * QUEUE_vSetElementFreeForce
 * 
 * Description: Release Element Forced - use when sure that the element is completed
 *              same as QUEUE_vSetElementFreeTry without check the elenet
 **************************************************************************** */
void QUEUE_vSetElementFreeForce(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue, EDS_SPACE_QUEUE QUEUE_sElement_t* pQueueElement)
{
    pQueueElement->eStatus = QUEUE_STA_IDLE;
    
    ISR_vEnterCritical();
    if (pQueueElement == pQueue->psQueueElementNextFree)
    {
        pQueue->psQueueElementNextFree++;
        if (pQueue->psQueueElementNextFree >= pQueue->psQueueElementFinal)
        {
            pQueue->psQueueElementNextFree = pQueue->psQueueElementStart;
        }               
    } 
    ISR_vExitCritical();
}

/* *****************************************************************************
 * QUEUE_vSetElementFreeLastSent
 * 
 * Description: Release Last Sent Element 
 **************************************************************************** */
void QUEUE_vSetElementFreeLastSent(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue)
{
    if (pQueue->psQueueElementLastSent)
    {
        pQueue->psQueueElementLastSent->eStatus = QUEUE_STA_IDLE;
        ISR_vEnterCritical();
        if (pQueue->psQueueElementLastSent == pQueue->psQueueElementNextFree)
        {
            pQueue->psQueueElementNextFree++;
            if (pQueue->psQueueElementNextFree >= pQueue->psQueueElementFinal)
            {
                pQueue->psQueueElementNextFree = pQueue->psQueueElementStart;
            }               
        } 
        ISR_vExitCritical();
    }
}
#endif
/* *****************************************************************************
 * QUEUE_vSetElementFreeProcess
 * 
 * Description: Release Elements Process - must be called after every transfer 
 *              or in collaboration with QUEUE_vSetElementFree or/and QUEUE_vSetElementFreeLastSent
 **************************************************************************** */
void QUEUE_vSetElementFreeProcess(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue)
{
    EDS_SPACE_QUEUE QUEUE_sElement_t* psNextBackup = (EDS_SPACE_QUEUE QUEUE_sElement_t*)pQueue->psQueueElementNextFree;
    EDS_SPACE_QUEUE QUEUE_sElement_t* psFindNextLoop = (EDS_SPACE_QUEUE QUEUE_sElement_t*)pQueue->psQueueElementNextFree;
    
    //if (pQueue->psQueueElementLastFill)
    {
        do
        {
            QUEUE_vSetElementFreeTry(pQueue, psFindNextLoop);
            
            //if (psFindNextLoop != pQueue->psQueueElementLastFill)
            {
                psFindNextLoop++;
                if (psFindNextLoop >= pQueue->psQueueElementFinal)
                {
                    psFindNextLoop = pQueue->psQueueElementStart;
                }
            }
        }
        while ((psFindNextLoop != psNextBackup) 
                //&& (psFindNextLoop != pQueue->psQueueElementLastFill)
              );  
    }
}
