/* *****************************************************************************
 * File:   hmserial.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 01 12 11:17
 * 
 * Description: Hex Monitor Serial Communication
 * 
 **************************************************************************** */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "hmdrv.h"
//#include "hmprotocol.h"
#include "hmmodbus.h"
#include "hmserial.h"




/* *****************************************************************************
 * Check if Not Used
 **************************************************************************** */
#if (HMDRV_USE_SERIAL) && (!(HMDRV_DISABLE))

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */


/* serial runtime flags */
typedef struct
{
        unsigned bTxActive : 1;         /* transmit response */
        unsigned bTxWaitTC : 1;         /* wait Transmit Complete */
        unsigned bTxDelay : 1;         /* transmit response delay */
}HMSERIAL_sFlagsRxTx_t;

typedef struct
{
        unsigned bTxLastCharSOB : 1;    /* Start of Block Detected */
        unsigned bTxFirstSobSend : 1;   /* used in USB_CDC, MQX, JTAG */
        unsigned bRxLastCharSOB   : 1;  /* last received character was SOB */
        unsigned bRxMsgLengthNext : 1;  /* expect the length byte next time */
}HMSERIAL_sFlags_t;

typedef union 
{
    HMDRV_FLAGS     nAll;
    HMSERIAL_sFlags_t    sFlg;   
} HMSERIAL_uFlags_t;

typedef union 
{
    HMDRV_FLAGS     nAll;
    HMSERIAL_sFlagsRxTx_t    sFlg;   
} HMSERIAL_uFlagsRxTx_t;



/* *****************************************************************************
 * Variables
 **************************************************************************** */
HMSERIAL_uFlags_t pcm_wFlags;

volatile HMSERIAL_uFlagsRxTx_t pcm_wFlagsRxTx;

/* receive and transmit buffers and counters */
volatile HMDRV_SIZE8 pcm_nTxTodo;     /* transmission to-do counter (0 when tx is idle) */
volatile HMDRV_SIZE8 pcm_nRxTodoEcho;     /* transmission read-back to-do counter */

volatile HMDRV_SIZE8 pcm_nRxTodo;     /* reception to-do counter (0 when rx is idle) */
volatile HMDRV_BPTR  pcm_pTxBuff;     /* pointer to next byte to transmit */
volatile HMDRV_BPTR  pcm_pRxBuff;     /* pointer to next free place in RX buffer */
volatile HMDRV_BCHR  pcm_nRxCheckSum; /* checksum of data being received */

#if HMDRV_TX_DELAY
HMDRV_U32 HMSERIAL_u32DeviceSYSCLKFreq = (uint32_t)(HMDRV_FREE_RUN_TIMER_FREQ_MHZ * 1000000);
HMDRV_U16 HMSERIAL_u16FreeRunTimerPrescaller = 0;        /* 0 means prescaller 1:1 (u16FreeRunTimerPrescaller+1  :  1) */
volatile HMDRV_U32 pcm_u32TxDelayCounter;
volatile HMDRV_U32 pcm_u32TxDelayCount = HMDRV_TX_DELAY_TIME * HMDRV_FREE_RUN_TIMER_FREQ_MHZ;
#endif


/* FreeMASTER communication buffer (in/out) plus the STS and LEN bytes */
volatile HMDRV_BCHR pcm_pCommBuffer[HMDRV_COMM_BUFFER_SIZE+3];

volatile HMDRV_SIZE8 u8UartIndex = HMLIB_HEXMON_UART_MODULE;


/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */

#if HMDRV_TX_DELAY
void HMSERIAL_vSetFreeRunTimerPrescaller(uint16_t u16Prescaller);
#endif




/* *****************************************************************************
 * Functions
 **************************************************************************** */
#if HMDRV_TX_DELAY
void HMSERIAL_vSetFreeRunTimerPrescaller(uint16_t u16Prescaller)
{
    uint32_t u32DeviceSYSCLKFreq;

    pcm_u32TxDelayCount        /= HMSERIAL_u16FreeRunTimerPrescaller+1;
    pcm_u32TxDelayCount        *= u16Prescaller+1;
    HMSERIAL_u16FreeRunTimerPrescaller = u16Prescaller;

    u32DeviceSYSCLKFreq = HMDRV_u32GetDeviceSYSCLKFreq();
    if (u32DeviceSYSCLKFreq != HMSERIAL_u32DeviceSYSCLKFreq)
    {
        pcm_u32TxDelayCount      = (uint32_t)((uint64_t)(pcm_u32TxDelayCount      * u32DeviceSYSCLKFreq) / HMSERIAL_u32DeviceSYSCLKFreq);
        HMSERIAL_u32DeviceSYSCLKFreq = u32DeviceSYSCLKFreq;
    }

}
#endif


/**************************************************************************//*!
*
* @brief    Finalize transmit buffer before transmitting 
*
* @param    nLength - response length (1 for status + data length)
*
*
* This Function takes the data already prepared in the transmit buffer 
* (including the status byte). It computes the check sum and kicks on transmit.
*
******************************************************************************/
#if HMDRV_USE_FREEMASTER

void HMSERIAL_vSendResponse(HMDRV_BPTR pResponse, HMDRV_SIZE8 nLength)
{
    HMDRV_U16 chSum = 0U;
    HMDRV_U8 i, c;

    /* remember the buffer to be sent */
    pcm_pTxBuff = pResponse;
    
    /* status byte and data are already there, compute checksum only     */
    for (i=0U; i<nLength; i++)
    {
        c = 0U;
        pResponse = HMDRV_ValueFromBuffer8(&c, pResponse);
        /* add character to checksum */
        chSum += c;
        /* prevent saturation to happen on DSP platforms */
        chSum &= 0xffU;
    }
    
    if (chSum > 0xFF)
    {
        chSum &= 0x00FF;
    }
    
    if (nLength >= 0x3A)
    {
        chSum &= 0x00FF;
    }
    
    /* store checksum after the message */
    pResponse = HMDRV_ValueToBuffer8(pResponse, (HMDRV_U8) (((HMDRV_U8)(~chSum)) + 1U));

    /* send the message and the checksum and the SOB */
    pcm_nTxTodo = (HMDRV_SIZE8) (nLength + 1U); 
    
    pcm_nRxTodoEcho = pcm_nTxTodo + 1; /* +1 for the start 0x2b(+) Byte */
    
    /* now transmitting the response */
    pcm_wFlagsRxTx.sFlg.bTxActive = 1U;

    pcm_wFlagsRxTx.sFlg.bTxWaitTC = 0U;

    /* do not replicate the initial SOB  */
    pcm_wFlags.sFlg.bTxLastCharSOB = 0U;
    
    /* disable receiver, enable transmitter (single-wire communication) */
    #if !HMDRV_SCI_TWOWIRE_ONLY
    HMDRV_SCI_RD(u8UartIndex);
    #if HMDRV_TE_MIN_TIME == 0
    HMDRV_SCI_TE(u8UartIndex);
    #endif
    #endif 
    
    #if HMDRV_SCI_TX_DELAY_ISR    
        HMDRV_SETUP_TX_DELAY_DATA(u8UartIndex, HMPROTOCOL_SOB);
        HMDRV_SETUP_TX_DELAY(u8UartIndex, HMDRV_TX_DELAY_TIME);
    #elif HMDRV_TX_DELAY
        pcm_wFlagsRxTx.sFlg.bTxDelay = 1U;
        pcm_u32TxDelayCounter = pcm_u32TxDelayCount;
    #else
        /* kick on the SCI transmission (also clears TX Empty flag on some platforms) */
        #if HMDRV_SCI_TX_ISR == 1
        HMDRV_TX_INT_ENA(u8UartIndex);
        #endif
        #if HMDRV_TE_MIN_TIME
        HMDRV_SCI_TE(u8UartIndex);
        #endif
        HMDRV_SCI_PUTCHAR(u8UartIndex, HMPROTOCOL_SOB);
    #endif
      
}
#endif


void HMSERIAL_vProcessFastTD (void)
{
    if (pcm_nRxTodoEcho > 0)
    {
        pcm_nRxTodoEcho--;
        if (pcm_nRxTodoEcho == 0)
        {
            HMDRV_SCI_TD(u8UartIndex);
        }
    }
}


#if (HMDRV_USE_MODBUS == 0)
/**************************************************************************//*!
*
* @brief    Send response of given error code (no data) 
*
* @param    nErrCode - error code to be sent
*
******************************************************************************/

static void HMSERIAL_SendError(HMDRV_BCHR nErrCode)
{
    /* fill & send single-byte response */
    *pcm_pCommBuffer = nErrCode;
    HMSERIAL_vSendResponse((HMDRV_BPTR)pcm_pCommBuffer, 1U);
}
#endif

/**************************************************************************//*!
*
* @brief    Start listening on a serial line
*
* Reset the receiver machine and start listening on a serial line
*
******************************************************************************/
#if HMDRV_USE_FREEMASTER

static void HMSERIAL_Listen(void)
{
    /* Several times check received last byte from own transmit response message - there is slight delay one or 2 mostly supposed the stop bits from TxComplete to RxComplete */
        #if !HMDRV_SCI_TWOWIRE_ONLY
        /* read-out and ignore any received character (loopback) */
        #if HMDRV_SCI_READ_MULTIPLE_RX_DATA
        while (HMDRV_READY_RX(u8UartIndex))
        #else
        if (HMDRV_READY_RX(u8UartIndex))
        #endif
        {
            /*lint -esym(550, nRxChar) */
            volatile HMDRV_U16 nRxChar;
            nRxChar = HMDRV_SCI_GETCHAR(u8UartIndex);
        }
        #endif
    
    pcm_nRxTodo = 0U;

    /* disable transmitter state machine */
    pcm_wFlagsRxTx.sFlg.bTxActive = 0U;

    pcm_wFlagsRxTx.sFlg.bTxWaitTC = 0U;

    /* Several times check received last byte from own transmit response message - there is slight delay one or 2 mostly supposed the stop bits from TxComplete to RxComplete */
        #if !HMDRV_SCI_TWOWIRE_ONLY
        /* read-out and ignore any received character (loopback) */
        #if HMDRV_SCI_READ_MULTIPLE_RX_DATA
        while (HMDRV_READY_RX(u8UartIndex))
        #else
        if (HMDRV_READY_RX(u8UartIndex))
        #endif
        {
            /*lint -esym(550, nRxChar) */
            volatile HMDRV_U16 nRxChar;
            nRxChar = HMDRV_SCI_GETCHAR(u8UartIndex);
        }
        #endif
    


    /* disable transmitter, enable receiver (enables single-wire connection) */
    #if !HMDRV_SCI_TWOWIRE_ONLY
    HMDRV_SCI_TD(u8UartIndex);
    HMDRV_SCI_RE(u8UartIndex);
    #endif

}
#endif


#if HMDRV_USE_FREEMASTER
void HMSERIAL_DelayedTx(HMDRV_U8 u8TxChar)
{
    #if HMDRV_SCI_TX_ISR == 1
    HMDRV_TX_INT_ENA(u8UartIndex);
    #endif
    #if HMDRV_TE_MIN_TIME
    HMDRV_SCI_TE(u8UartIndex);
    #endif
    HMDRV_SCI_PUTCHAR(u8UartIndex, u8TxChar);
}
#endif

#if HMDRV_USE_FREEMASTER

/**************************************************************************//*!
*
* @brief    Output buffer transmission
* 
* @param  pTxChar  The character to be transmit 
*
* get ready buffer(prepare data to send)
*
******************************************************************************/

HMDRV_BOOL HMSERIAL_Tx(HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pTxChar)
{
    if (pcm_nTxTodo)
    {
        /* fetch & send character ready to transmit */
        /*lint -e{534} ignoring return value */
        (void)HMDRV_ValueFromBuffer8(pTxChar, pcm_pTxBuff);
        
        /* first, handle the replicated SOB characters */
        if (*pTxChar == HMPROTOCOL_SOB)
        {
            pcm_wFlags.sFlg.bTxLastCharSOB ^= 1U;
            if ((pcm_wFlags.sFlg.bTxLastCharSOB))
            {
                /* yes, repeat the SOB next time */
                return HMDRV_FALSE;
            }
        }
        /* no, advance tx buffer pointer */
        pcm_nTxTodo--;
        pcm_pTxBuff = HMDRV_SkipInBuffer(pcm_pTxBuff, 1U);
        return HMDRV_FALSE;
    }
    
    /* when SCI TX buffering is enabled, we must first wait until all 
       characters are physically transmitted (before disabling transmitter) */

    #if HMDRV_SCI_TX_ISR == 1
    HMDRV_TX_INT_DIS(u8UartIndex);
    #endif
    pcm_wFlagsRxTx.sFlg.bTxWaitTC = 1;
    
    /* start listening immediately if no Tx Queue */
    //HMDRV_Listen();


    return HMDRV_TRUE;
}



/**************************************************************************//*!
*
* @brief  Handle received character 
*
* @param  nRxChar  The character to be processed 
* 
* Handle the character received and -if the message is complete- call the 
* protocol decode routine. 
*
******************************************************************************/

HMDRV_BOOL HMSERIAL_Rx(HMDRV_BCHR nRxChar)
{
    HMSERIAL_uFlags_t* pflg = &pcm_wFlags;
    /* first, handle the replicated SOB characters */
    if(nRxChar == HMPROTOCOL_SOB)
    {
        pflg->sFlg.bRxLastCharSOB ^= 1;
        if(pflg->sFlg.bRxLastCharSOB)
        {
            /* this is either the first byte of replicated SOB or a  */
            /* real Start-of-Block mark - we will decide next time in HMSERIAL_Rx */
            return HMDRV_FALSE;
        }
    }
    
    /* we have got a common character preceded by the SOB -  */
    /* this is the command code! */
    if(pflg->sFlg.bRxLastCharSOB)
    {
        /* reset receiving process */
        pcm_pRxBuff = (HMDRV_BPTR)pcm_pCommBuffer;
        *pcm_pRxBuff++ = nRxChar;

        /* start computing the checksum */
        pcm_nRxCheckSum = nRxChar;
        pcm_nRxTodo = 0U;
    
        /* if the standard command was received, the message length will come in next byte */
        pflg->sFlg.bRxMsgLengthNext = 1U;

        /* fast command? */
        if(!((~nRxChar) & HMPROTOCOL_FASTCMD))
        {
            /* fast command received, there will be no length information */
            pflg->sFlg.bRxMsgLengthNext = 0U;
            /* as it is encoded in the command byte directly */
            pcm_nRxTodo = (HMDRV_SIZE8) 
                (((((HMDRV_SIZE8)nRxChar) & HMPROTOCOL_FASTCMD_DATALEN_MASK) >> HMPROTOCOL_FASTCMD_DATALEN_SHIFT) + 1U);
        }

        /* command code stored & processed */
        pflg->sFlg.bRxLastCharSOB = 0U;
        return HMDRV_FALSE;
    }

    /* we are waiting for the length byte */
    if(pflg->sFlg.bRxMsgLengthNext)
    {
        /* this byte, total data length and the checksum */
        pcm_nRxTodo = (HMDRV_SIZE8) (1U + ((HMDRV_SIZE8)nRxChar) + 1U);
        /* now read the data bytes */
        pflg->sFlg.bRxMsgLengthNext = 0U;

    }

    /* waiting for a data byte? */
    if(pcm_nRxTodo)
    {
        /* add this byte to checksum */
        pcm_nRxCheckSum += nRxChar;

        /* decrease number of expected bytes */
        pcm_nRxTodo--;
        /* was it the last byte of the message (checksum)? */
        if(!pcm_nRxTodo)
        {
            /* receive buffer overflow? */
            if(pcm_pRxBuff == NULL)
            {
                #if (HMDRV_USE_MODBUS == 0)
                HMSERIAL_SendError(HMPROTOCOL_STC_CMDTOOLONG);
                #endif
            }
            /* checksum error? */
            else if((pcm_nRxCheckSum & 0xffU) != 0U)
            {
                #if (HMDRV_USE_MODBUS == 0)
                HMSERIAL_SendError(HMPROTOCOL_STC_CMDCSERR);
                #endif
            }
            /* message is okay */
            else 
            {
                /* do decode now! */
                (void)HMPROTOCOL_bDecoder((HMDRV_BPTR)pcm_pCommBuffer);

                /* Decode protocol and send response in Poll function if in ISR */
                //pflg->sFlg.bUsbReadyToDecode = 1U;
            }

            return HMDRV_TRUE;
        }
        /* not the last character yet */
        else 
        {   
            /* is there still a space in the buffer? */
            if(pcm_pRxBuff)
            {
                /*lint -e{946} pointer arithmetic is okay here (same array) */
                if(pcm_pRxBuff < (pcm_pCommBuffer + HMDRV_COMM_BUFFER_SIZE))
                {
                    /* store byte  */
                    *pcm_pRxBuff++ = nRxChar;
                }
                /* buffer is full! */
                else
                {
                    /* NULL rx pointer means buffer overflow - but we still need */
                    /* to receive all message characters (for the single-wire mode) */
                    /* so keep "receiving" - but throw away all characters from now */
                    pcm_pRxBuff = NULL;
                }
            }
        }
    }
    return HMDRV_FALSE;	
}

#if HMDRV_TX_DELAY
void HMSERIAL_vProcessTxDelay(HMDRV_TIMER nTimeTickDelay)
{
    if (pcm_u32TxDelayCounter > 0)
    {
        if (pcm_u32TxDelayCounter > nTimeTickDelay)
        {
            pcm_u32TxDelayCounter -= nTimeTickDelay;
        }
        else
        {
            pcm_u32TxDelayCounter = 0;
        }
    }
}
#endif


#endif

/**************************************************************************//*!
*
* @brief    Handle SCI communication (TX on ISR)
*
* This function checks the SCI flags and calls the Tx functions
*
* @note This function can be called from SCI ISR
*
******************************************************************************/
#if HMDRV_USE_FREEMASTER
#if HMDRV_SCI_TX_ISR
void HMSERIAL_vProcessSCITXISRFreemasterOnly (void)
{
    if (pcm_wFlagsRxTx.sFlg.bTxActive)
    {
        /* ready to accept another character? */
        //if (HMDRV_READY_TX())
        {
            HMDRV_U8 ch;
            /* just put the byte into the SCI transmit buffer */
            if(!HMSERIAL_Tx(&ch))
            {
                HMDRV_SCI_PUTCHAR(u8UartIndex, (HMDRV_U8) ch); 
            }
        }
    }
    else
    {
        /* should not enter here */
        HMDRV_TX_INT_DIS(u8UartIndex);
    }
}
#endif
#endif


/**************************************************************************//*!
*
* @brief    Handle SCI communication (both TX and RX)
*
* This function checks the SCI flags and calls the Rx and/or Tx functions
*
* @note This function can be called either from SCI ISR or from the polling routine
*
******************************************************************************/

#if HMDRV_USE_FREEMASTER
void HMSERIAL_vProcessSCIFreemasterOnly(void)
{
    /* read & clear status     */

    /* transmitter active and empty? */
    if (pcm_wFlagsRxTx.sFlg.bTxActive)
    {
        #if HMDRV_TX_DELAY
        if (pcm_wFlagsRxTx.sFlg.bTxDelay)  
        {
            if (pcm_u32TxDelayCounter == 0)
            {
                pcm_wFlagsRxTx.sFlg.bTxDelay = 0;
                #if HMDRV_SCI_TX_ISR == 1
                HMDRV_TX_INT_ENA(u8UartIndex);
                #endif
                #if HMDRV_TE_MIN_TIME
                HMDRV_SCI_TE(u8UartIndex);
                #endif
                HMDRV_SCI_PUTCHAR(u8UartIndex, HMPROTOCOL_SOB);
            }
        }
        
        if (pcm_wFlagsRxTx.sFlg.bTxDelay == 0U)    
        #endif
        {
            #if HMDRV_SCI_TX_ISR == 0
            /* ready to accept another character? */
            if (HMDRV_READY_TX(u8UartIndex))
            {
                HMDRV_U8 ch;
                /* just put the byte into the SCI transmit buffer */
                if(!HMSERIAL_Tx(&ch))
                {
                    HMDRV_SCI_PUTCHAR(u8UartIndex, (HMDRV_U8) ch); 
                }
            }
            #endif
        }

        /* waiting for transmission complete flag? */
        if(pcm_wFlagsRxTx.sFlg.bTxWaitTC && (HMDRV_EMPTY_TX(u8UartIndex)))
        {
            /* after TC, we can switch to listen mode safely */
            HMSERIAL_Listen();
        }

        #if !HMDRV_SCI_TWOWIRE_ONLY
        /* read-out and ignore any received character (loopback) */
        #if HMDRV_SCI_READ_MULTIPLE_RX_DATA
        while (HMDRV_READY_RX(u8UartIndex))
        #else
        if (HMDRV_READY_RX(u8UartIndex))
        #endif
        {
            /*lint -esym(550, nRxChar) */
            volatile HMDRV_U16 nRxChar;
            nRxChar = HMDRV_SCI_GETCHAR(u8UartIndex);
        }
        #endif
    }
    /* transmitter not active, ready to receive */
    else
    {
        /* data byte received? */
        #if HMDRV_SCI_READ_MULTIPLE_RX_DATA
        while (HMDRV_READY_RX(u8UartIndex))
        #else
        if (HMDRV_READY_RX(u8UartIndex))
        #endif
        {
            HMDRV_BCHR nRxChar = 0U;
            nRxChar = (HMDRV_BCHR) HMDRV_SCI_GETCHAR(u8UartIndex);
            (void)HMSERIAL_Rx(nRxChar);
        }
    }
}
#endif /* HMDRV_USE_FREEMASTER */

#if (HMDRV_USE_MODBUS && HMDRV_MODBUS_TX_DELAY) || HMDRV_TX_DELAY
HMDRV_TIMER HMSERIAL_vProcessFreeTimer(void)
{
    static HMDRV_TIMER nLastTimerValue = 0;
    
    HMDRV_TIMER nTimerValue;
    HMDRV_TIMER nTimeTickDelay;

    nTimerValue = HMDRV_GET_FREE_RUN_TIMER(); 
    nTimeTickDelay = nTimerValue - nLastTimerValue;
#if HMDRV_FREE_RUN_TIMER_DECREMENT_COUNTER
    nTimeTickDelay = 0 - nTimeTickDelay;
#endif
    nLastTimerValue = nTimerValue;
    
    return nTimeTickDelay;
}
#endif


/**************************************************************************//*!
*
* @brief    Handle SCI communication (TX on ISR)
*
* This function checks the SCI flags and calls the Tx functions
*
* @note This function can be called from SCI ISR
*
******************************************************************************/
#if HMDRV_SCI_TX_ISR
void HMSERIAL_vProcessSCITXISR (void)
{
#if HMDRV_USE_FREEMASTER
    if (pcm_wFlagsRxTx.sFlg.bTxActive)
    {
        /* ready to accept another character? */
        //if (HMDRV_READY_TX())
        {
            HMDRV_U8 ch;
            /* just put the byte into the SCI transmit buffer */
            if(!HMSERIAL_Tx(&ch))
            {
                HMDRV_SCI_PUTCHAR(u8UartIndex, (HMDRV_U8) ch); 
            }
        }
    }
    #if HMDRV_USE_MODBUS
    else
    #endif
#endif
#if HMDRV_USE_MODBUS
    if (HMMODBUS_wFlagsRxTx.sFlg.bTxActive)
    {
        /* ready to accept another character? */
        //if (HMDRV_READY_TX())
        {
            HMDRV_U8 ch;
            /* just put the byte into the SCI transmit buffer */
            if(!HMMODBUS_Tx(&ch)) 
            {
                HMDRV_SCI_PUTCHAR(u8UartIndex, (HMDRV_U8) ch); 
            }
        }
    } 
#endif
    else
    {
        /* should not enter here */
        HMDRV_TX_INT_DIS(u8UartIndex);
    }
}
#endif

/**************************************************************************//*!
*
* @brief    Handle SCI communication (both TX and RX)
*
* This function checks the SCI flags and calls the Rx and/or Tx functions
*
* @note This function can be called either from SCI ISR or from the polling routine
*
******************************************************************************/

void HMSERIAL_vProcessSCI(void)
{


    #if (HMDRV_USE_MODBUS && HMDRV_MODBUS_TX_DELAY) || HMDRV_TX_DELAY
    HMDRV_TIMER nTimeTickDelay;   
    nTimeTickDelay = HMSERIAL_vProcessFreeTimer();
    #endif

    #if HMDRV_USE_MODBUS && HMDRV_MODBUS_TX_DELAY
    HMMODBUS_vProcessTxDelay(nTimeTickDelay);
    HMMODBUS_vProcessRxDelay(nTimeTickDelay);
    #endif

    #if HMDRV_TX_DELAY
    HMSERIAL_vProcessTxDelay(nTimeTickDelay);
    #endif

    /* read & clear status     */

#if HMDRV_USE_FREEMASTER
    /* transmitter active Freemaster? */
    if (pcm_wFlagsRxTx.sFlg.bTxActive)
    {
        #if HMDRV_TX_DELAY
        if (pcm_wFlagsRxTx.sFlg.bTxDelay)  
        {
            if (pcm_u32TxDelayCounter == 0)
            {
                pcm_wFlagsRxTx.sFlg.bTxDelay = 0;
                #if HMDRV_SCI_TX_ISR == 1
                HMDRV_TX_INT_ENA(u8UartIndex);
                #endif
                #if HMDRV_TE_MIN_TIME
                HMDRV_SCI_TE(u8UartIndex);
                #endif
                HMDRV_SCI_PUTCHAR(u8UartIndex, HMPROTOCOL_SOB);
            }
        }
        
        if (pcm_wFlagsRxTx.sFlg.bTxDelay == 0U)    
        #endif
        {
            #if HMDRV_SCI_TX_ISR == 0
            /* ready to accept another character? */
            if (HMDRV_READY_TX(u8UartIndex))
            {
                HMDRV_U8 ch;
                /* just put the byte into the SCI transmit buffer */
                if(!HMSERIAL_Tx(&ch))
                {
                    HMDRV_SCI_PUTCHAR(u8UartIndex, (HMDRV_U8) ch); 
                }
            }
            #endif
        }

        /* waiting for transmission complete flag? */
        if(pcm_wFlagsRxTx.sFlg.bTxWaitTC && (HMDRV_EMPTY_TX(u8UartIndex)))
        {
            /* after TC, we can switch to listen mode safely */
            HMSERIAL_Listen();
        }

        #if !HMDRV_SCI_TWOWIRE_ONLY
        /* read-out and ignore any received character (loopback) */
        #if HMDRV_SCI_READ_MULTIPLE_RX_DATA
        while (HMDRV_READY_RX(u8UartIndex))
        #else
        if (HMDRV_READY_RX(u8UartIndex))
        #endif
        {
            /*lint -esym(550, nRxChar) */
            volatile HMDRV_U16 nRxChar;
            nRxChar = HMDRV_SCI_GETCHAR(u8UartIndex);
        }
        #endif
    }
    #if HMDRV_USE_MODBUS
    else
    #endif
#endif

    #if HMDRV_USE_MODBUS
    /* transmitter active MODBUS? */
    if (HMMODBUS_wFlagsRxTx.sFlg.bTxActive)
    {
        #if HMDRV_MODBUS_TX_DELAY
        if (HMMODBUS_wFlagsRxTx.sFlg.bTxDelay)  
        {
            if (HMMODBUS_u32TxDelayCounter == 0)
            {
                HMMODBUS_wFlagsRxTx.sFlg.bTxDelay = 0;
                #if HMDRV_SCI_TX_ISR == 1
                HMDRV_TX_INT_ENA(u8UartIndex);
                #endif
                #if HMDRV_TE_MIN_TIME
                HMDRV_SCI_TE(u8UartIndex);
                #endif
                HMDRV_SCI_PUTCHAR(u8UartIndex, *HMMODBUS_pTxBuff++);
            }
        }
        
        if (HMMODBUS_wFlagsRxTx.sFlg.bTxDelay == 0U)
        #endif
        { 
            #if HMDRV_SCI_TX_ISR == 0
            /* ready to accept another character? */
            if (HMDRV_READY_TX(u8UartIndex))
            {
                HMDRV_U8 ch;
                /* just put the byte into the SCI transmit buffer */
                if(!HMMODBUS_Tx(&ch)) 
                {
                    HMDRV_SCI_PUTCHAR(u8UartIndex, (HMDRV_U8) ch); 
                }
            }
            #endif
            

        }

        /* waiting for transmission complete flag? */
        if((HMMODBUS_wFlagsRxTx.sFlg.bTxWaitTC) && (HMDRV_EMPTY_TX(u8UartIndex)))
        {
            /* after TC, we can switch to listen mode safely */
            HMMODBUS_Listen();
        }


        #if !HMDRV_SCI_TWOWIRE_ONLY
        /* read-out and ignore any received character (loopback) */
        #if HMDRV_SCI_READ_MULTIPLE_RX_DATA
        while (HMDRV_READY_RX(u8UartIndex))
        #else
        if (HMDRV_READY_RX(u8UartIndex))
        #endif
        {
            /*lint -esym(550, nRxChar) */
            volatile HMDRV_U16 nRxChar;
            nRxChar = HMDRV_SCI_GETCHAR(u8UartIndex);
        }
        #endif
    }
    #endif
    /* transmitter not active, ready to receive */
    else
    {
        /* data byte received? */
        #if HMDRV_SCI_READ_MULTIPLE_RX_DATA
        while (HMDRV_READY_RX(u8UartIndex))
        #else
        if (HMDRV_READY_RX(u8UartIndex))
        #endif
        {
            HMDRV_BCHR nRxChar = 0U;
            nRxChar = (HMDRV_BCHR) HMDRV_SCI_GETCHAR(u8UartIndex);

            #if HMDRV_USE_FREEMASTER
            (void)HMSERIAL_Rx(nRxChar);
            #endif

            #if HMDRV_USE_MODBUS
            (void)HMMODBUS_Rx(nRxChar);  
            #endif
        }
    }

}

void HMSERIAL_vSetPortIndex(HMDRV_U8 u8Index)
{ 
    u8UartIndex = u8Index;
}


/**************************************************************************//*!
*
* @brief    Serial communication initialization
*
******************************************************************************/
#if HMDRV_USE_FREEMASTER

void HMSERIAL_vInit(void)
{   
    /* initialize all state variables */
    pcm_wFlags.nAll = 0U;
    pcm_wFlagsRxTx.nAll = 0U;
    pcm_nTxTodo = 0U;

    /* Initialize SCI and JTAG interface */
    #if HMDRV_SCI_TWOWIRE_ONLY
    /* to enable TX and RX together in FreeMASTER initialization */
    HMDRV_SCI_TE_RE(u8UartIndex);
    #endif
    
    #if HMDRV_USE_FREEMASTER
    #if HMDRV_SCI_TX_DELAY_ISR    
    HMDRV_SETUP_TX_DELAY_FUNCTION(u8UartIndex, HMSERIAL_DelayedTx);
    #endif
    #endif

    #if HMDRV_TX_DELAY
    HMSERIAL_vSetFreeRunTimerPrescaller(TMR_u16GetFreeRunTimerPrescaller());
    #endif

    /* start listening for commands */
    #if HMDRV_USE_FREEMASTER
    HMSERIAL_Listen();
    #endif
}

#endif




#endif
