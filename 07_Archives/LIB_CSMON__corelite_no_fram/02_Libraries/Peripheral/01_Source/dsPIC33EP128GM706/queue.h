/* *****************************************************************************
 * File:   queue.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 06 19 10:58
 * 
 * Description: Communication Queue Management
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef QUEUE_H
#define	QUEUE_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdint.h>

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
//#define EDS_SPACE_QUEUE __eds__
#define EDS_SPACE_QUEUE 

/* *****************************************************************************
 * Constants Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */
typedef enum
{
    QUEUE_STA_IDLE,
    QUEUE_STA_FILL,
    QUEUE_STA_FULL,
    QUEUE_STA_SENDING,
    QUEUE_STA_READY,
    QUEUE_STA_COMPLETED,
            
    QUEUE_STA_COUNT      
}QUEUE_eStatus_t;

typedef enum
{
    QUEUE_CMD_WRITE,
    QUEUE_CMD_READ,
    QUEUE_CMD_COUNT      
}QUEUE_eCommand_t;

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

typedef struct
{
    uint16_t u16Lo;
    uint16_t u16Hi;
}sAddress_t;


typedef union
{
    uint32_t u32Address;
    sAddress_t sAddress;
}uAddress_t;

typedef struct
{
    uint16_t bUseHeapBuffer : 1;
}QUEUE_sElementFlags_t;

typedef union
{
    uint16_t u16Register;
    QUEUE_sElementFlags_t sFlags;
}QUEUE_uElementFlags_t;

typedef struct
{
    volatile QUEUE_eStatus_t eStatus;    
    QUEUE_eCommand_t eCommand;
    uAddress_t uAddress;
    uint16_t u16ChipAddress;    /* ex.: ChipSelect */
    uint16_t u16Size;
    uint8_t* pData;
    QUEUE_uElementFlags_t uFlags;
}QUEUE_sElement_t;

typedef struct
{
    EDS_SPACE_QUEUE volatile QUEUE_sElement_t* psQueueElementNextFill;          /* Next Element To Fill (Next that should be available for usage(fill)) */
    EDS_SPACE_QUEUE volatile QUEUE_sElement_t* psQueueElementNextFree;          /* Next Element To Free (Next that must be set free) */
    EDS_SPACE_QUEUE volatile QUEUE_sElement_t* psQueueElementNextSend;          /* Next Element To Send (Next that must be sent) */
    EDS_SPACE_QUEUE          QUEUE_sElement_t* psQueueElementLastSent;
    EDS_SPACE_QUEUE          QUEUE_sElement_t* psQueueElementLastFill;
    EDS_SPACE_QUEUE          QUEUE_sElement_t* psQueueElementStart;
    EDS_SPACE_QUEUE          QUEUE_sElement_t* psQueueElementFinal;
             QUEUE_eStatus_t   eQueueStatus;
}QUEUE_sQueue_t;

/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */


/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void QUEUE_vInit(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue, EDS_SPACE_QUEUE QUEUE_sElement_t* pQueueElements, uint16_t nCountElements);
EDS_SPACE_QUEUE QUEUE_sElement_t* QUEUE_vGetElementFillNext(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue);
EDS_SPACE_QUEUE QUEUE_sElement_t* QUEUE_vGetElementFillNextEmbeddedQueue(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue, EDS_SPACE_QUEUE QUEUE_sElement_t* pEmbeddedQueuePrevElement);

EDS_SPACE_QUEUE QUEUE_sElement_t* QUEUE_vGetElementSendNext(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue);
EDS_SPACE_QUEUE QUEUE_sElement_t* QUEUE_vGetElementSendNextISR(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue);

void QUEUE_vSetElementFreeTryISR(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue, EDS_SPACE_QUEUE QUEUE_sElement_t* pQueueElement);
void QUEUE_vSetElementFreeTry(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue, EDS_SPACE_QUEUE QUEUE_sElement_t* pQueueElement);
void QUEUE_vSetElementFreeForce(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue, EDS_SPACE_QUEUE QUEUE_sElement_t* pQueueElement);
void QUEUE_vSetElementFreeLastSent(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue);
void QUEUE_vSetElementFreeProcess(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue);
void QUEUE_vSetElementFree(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue, EDS_SPACE_QUEUE QUEUE_sElement_t* psQueueElement);
void QUEUE_vSetLastElementSentReady(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue);
void QUEUE_vSetLastElementSentCompleted(EDS_SPACE_QUEUE QUEUE_sQueue_t* pQueue);
void QUEUE_vSetElementReady(EDS_SPACE_QUEUE QUEUE_sElement_t* psQueueElement);
void QUEUE_vSetElementCompleted(EDS_SPACE_QUEUE QUEUE_sElement_t* psQueueElement);
#endif	/* QUEUE_H */
