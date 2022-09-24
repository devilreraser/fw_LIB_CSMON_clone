/* *****************************************************************************
 * File:   hmprotocol.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 01 03 11:35
 * 
 * Description: Hex Monitor Library Protocol
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef HMPROTOCOL_H
#define	HMPROTOCOL_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "hmdrv.h"

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */



/******************************************************************************
* Protocol constants 
*******************************************************************************/

typedef enum
{
    HMPROTOCOL_SOB                  = 0x2bU,  /* '+' - start of message*/
    HMPROTOCOL_FASTCMD              = 0xc0U,  /* code of fast cmd 0xc0> */
    HMPROTOCOL_FASTCMD_DATALEN_MASK = 0x30U,  /* mask of data length part of fast command */
    HMPROTOCOL_FASTCMD_DATALEN_SHIFT= 3,
    HMPROTOCOL_DESCR_SIZE           = 25U  ,  /* length board desription string */

    /* Board configuration flags  */
    HMPROTOCOL_CFGFLAG_BIGENDIAN    = 0x01U,  /*/< used when CPU is big endian */
    HMPROTOCOL_CFGFLAG_LITTLEENDIAN = 0x00U,  /*/< used when CPU is little endian */

}HMPROTOCOL_eConstants_t;


/*-------------------------------------
  command message - Fast Commands
  -------------------------------------*/
typedef enum
{
    /* standard messages */
    HMPROTOCOL_CMD_READMEM          = 0x01U,
    HMPROTOCOL_CMD_WRITEMEM         = 0x02U,
    HMPROTOCOL_CMD_WRITEMEMMASK     = 0x03U,
    HMPROTOCOL_CMD_READMEM_EX       = 0x04U,  /* read a block of memory */
    HMPROTOCOL_CMD_WRITEMEM_EX      = 0x05U,  /* write a block of memory */   
    HMPROTOCOL_CMD_WRITEMEMMASK_EX  = 0x06U,  /* write block of memory with bit mask */   
    HMPROTOCOL_CMD_SETUPSCOPE       = 0x08U,
    HMPROTOCOL_CMD_SETUPREC         = 0x09U,
    HMPROTOCOL_CMD_SETUPSCOPE_EX    = 0x0aU,  /* setup the oscilloscope */ 
    HMPROTOCOL_CMD_SETUPREC_EX      = 0x0bU,  /* setup the recorder */     
    HMPROTOCOL_CMD_SENDAPPCMD       = 0x10U,  /* send the application command */  
    HMPROTOCOL_CMD_GETTSAINFO       = 0x11U,  /* get TSA info */
    HMPROTOCOL_CMD_GETTSAINFO_EX    = 0x12U,  /* get TSA info 32bit */
    HMPROTOCOL_CMD_SFIOFRAME_1      = 0x13U,  /* deliver & execute SFIO frame (even) */
    HMPROTOCOL_CMD_SFIOFRAME_0      = 0x14U,  /* deliver & execute SFIO frame (odd) */
    HMPROTOCOL_CMD_PIPE             = 0x15U,  /* read/write pipe data */


    /* no data part */
    HMPROTOCOL_CMD_GETINFO          = 0xc0U,  /* retrieve board information structure */  
    HMPROTOCOL_CMD_STARTREC         = 0xc1U,  /* start data recorder */   
    HMPROTOCOL_CMD_STOPREC          = 0xc2U,  /* stop data recorder */    
    HMPROTOCOL_CMD_GETRECSTS        = 0xc3U,  /* get the recorder status */   
    HMPROTOCOL_CMD_GETRECBUFF       = 0xc4U,
    HMPROTOCOL_CMD_READSCOPE        = 0xc5U,  /* read the scope data */   
    HMPROTOCOL_CMD_GETAPPCMDSTS     = 0xc6U,  /* get the application command status */    
    HMPROTOCOL_CMD_GETINFOBRIEF     = 0xc8U,  /* retrieve a subset of board information structure */  
    HMPROTOCOL_CMD_GETRECBUFF_EX    = 0xc9U,  /* get the recorder data */ 
    HMPROTOCOL_CMD_SFIOGETRESP_0    = 0xcaU,  /* retry to get last SFIO response (even) */
    HMPROTOCOL_CMD_SFIOGETRESP_1    = 0xcbU,  /* retry to get last SFIO response (odd) */

    HMPROTOCOL_CMD_FINAL = 0xFF

}HMPROTOCOL_eCommandCode_t;


/*-------------------------------------
  response message - status byte
  -------------------------------------*/

typedef enum
{
    /* flags in response codes */
    HMPROTOCOL_STSF_ERROR           = 0x80U,  /* FLAG: error answer (no response data) */  
    HMPROTOCOL_STSF_VARLEN          = 0x40U,  /* FLAG: variable-length answer (length byte) */  
    HMPROTOCOL_STSF_EVENT           = 0x20U,  /* FLAG: reserved */  
 
    /* confirmation codes */ 
    HMPROTOCOL_STS_OK               = 0x00U,  /* operation finished successfully */  
    HMPROTOCOL_STS_RECRUN           = 0x01U,  /* data recorder is running */  
    HMPROTOCOL_STS_RECDONE          = 0x02U,  /* data recorder is stopped */  
 
    /* error codes */ 
    HMPROTOCOL_STC_INVCMD           = 0x81U,  /* unknown command code */  
    HMPROTOCOL_STC_CMDCSERR         = 0x82U,  /* command checksum error */    
    HMPROTOCOL_STC_CMDTOOLONG       = 0x83U,  /* command is too long */    
    HMPROTOCOL_STC_RSPBUFFOVF       = 0x84U,  /* the response would not fit into transmit buffer */   
    HMPROTOCOL_STC_INVBUFF          = 0x85U,  /* invalid buffer length or operation */    
    HMPROTOCOL_STC_INVSIZE          = 0x86U,  /* invalid size specified */    
    HMPROTOCOL_STC_SERVBUSY         = 0x87U,  /* service is busy */   
    HMPROTOCOL_STC_NOTINIT          = 0x88U,  /* service is not initialized */    
    HMPROTOCOL_STC_EACCESS          = 0x89U,  /* access is denied */  
    HMPROTOCOL_STC_SFIOERR          = 0x8AU,  /* Error in SFIO frame */  
    HMPROTOCOL_STC_SFIOUNMATCH      = 0x8BU,  /* Even/odd mismatch in SFIO transaction */  
    HMPROTOCOL_STC_PIPEERR          = 0x8CU,  /* Pipe error */  
    HMPROTOCOL_STC_FASTRECERR       = 0x8DU,  /* Feature not implemented in Fast Recorder */  
    HMPROTOCOL_STC_CANTGLERR        = 0x8EU,  /* CAN fragmentation (toggle bit) error */
    HMPROTOCOL_STC_CANMSGERR        = 0x8FU,  /* CAN message format error */
    HMPROTOCOL_STC_FLOATDISABLED    = 0x90U,  /* Floating point triggering is not enabled */
 
            
            
    /* Start Rec Function Error Code */        
    #if HMREC_COMMON_ERR_CODES
    HMPROTOCOL_STC_RECRUN           = HMPROTOCOL_STC_NOTINIT,     
    #else
    HMPROTOCOL_STC_RECRUN           = HMPROTOCOL_STS_RECRUN,  
    #endif

    /* Setup Rec Function Error Code */       
    #if HMREC_COMMON_ERR_CODES
    HMPROTOCOL_STE_INVBUFF          = HMPROTOCOL_STC_INVSIZE, 
    HMPROTOCOL_STE_INVSIZE          = HMPROTOCOL_STC_INVSIZE, 
    HMPROTOCOL_STE_FLOATDISABLED    = HMPROTOCOL_STC_INVSIZE,
            
    HMPROTOCOL_STE_FASTRECERR       = HMPROTOCOL_STC_INVSIZE, 
    #else
    HMPROTOCOL_STE_INVBUFF          = HMPROTOCOL_STC_INVBUFF, 
    HMPROTOCOL_STE_INVSIZE          = HMPROTOCOL_STC_INVSIZE,         
    HMPROTOCOL_STE_FLOATDISABLED    = HMPROTOCOL_STC_FLOATDISABLED,
            
    HMPROTOCOL_STE_FASTRECERR       = HMPROTOCOL_STC_FASTRECERR, 
    #endif
            
    
            
            
    HMPROTOCOL_STx_FINAL = 0xFF        
}HMPROTOCOL_eResponseCode_t;

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
#if HMDRV_USE_FREEMASTER
void HMPROTOCOL_vInit(void);
HMDRV_BOOL HMPROTOCOL_bDecoder(HMDRV_BPTR pMessageIO);
void HMPROTOCOL_vRecorderSetTimeBase(HMDRV_U16 u16Inputusec);
#else
#define HMPROTOCOL_vInit()
#define HMPROTOCOL_bDecoder(x) (0)
#define HMPROTOCOL_vRecorderSetTimeBase(x)
#endif  /* HMDRV_USE_FREEMASTER */

#endif	/* HMPROTOCOL_H */
