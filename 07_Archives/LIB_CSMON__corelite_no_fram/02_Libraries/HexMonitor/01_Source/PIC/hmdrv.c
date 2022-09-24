/* *****************************************************************************
 * File:   hmdrv.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 01 02 19:56
 * 
 * Description: Hex Monitor Library MCU and Platform Dependent Driver
 * 
 **************************************************************************** */

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "hmdrv.h"

#if HMDRV_USE_EXT_MEM
#if HMDRV_USE_EXT_SPI
#include "spimem.h"
#endif
#if HMDRV_USE_EXT_PMP
#include "pmpmem.h"
#endif
#if HMDRV_USE_EXT_I2C
#include "i2cmem.h"
#endif
#endif


#if (!HMDRV_DISABLE)



/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
HMDRV_ADDR HMDRV_pGetInternalMemoryAddressWrite(HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize, HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* pu16ID);
HMDRV_ADDR HMDRV_pGetInternalMemoryAddressRead(HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize, HMDRV_ADDR_PNTR_MODIFIER HMDRV_SIZE8* pnSizeRead, HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* pu16ID);
void ProcessExternalMemory(HMDRV_ADDR nInternalRamBufferAddr, HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize, HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* pu16ID);


/* *****************************************************************************
 * Variables Definitions - mainly for Debug
 **************************************************************************** */
//volatile HMDRV_U8*   HM_pCopyToSrc = 0;
//volatile HMDRV_U8*   HM_pCopyToDest = 0;
//volatile HMDRV_SIZE8 HM_pCopyToSize = 0;

//volatile HMDRV_U8*   HM_pCopyFromSrc  = 0;
//volatile HMDRV_U8*   HM_pCopyFromDest = 0;
//volatile HMDRV_SIZE8 HM_pCopyFromSize = 0;

volatile HMDRV_ADDR HMDRV_CopyToBuffer_pSrcAddr;
volatile HMDRV_ADDR HMDRV_CopyToBufferInternal_ps;
volatile HMDRV_U8 HMDRV_CopyToBufferInternal_Data;

__eds__ HMDRV_U8* HMDRV_CopyToBufferInternal_ps_Use;

/* *****************************************************************************
 * HMDRV_CopyMemory
 * 
 * Description: memcpy function
 **************************************************************************** */
void HMDRV_CopyMemory(HMDRV_ADDR nDestAddr, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{
#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY


    HMDRV_U16* ps = (HMDRV_U16*)(((HMDRV_U32)nSrcAddr) >> 1);
    HMDRV_U16* pd = (HMDRV_U16*)(((HMDRV_U32)nDestAddr)>> 1);
    HMDRV_U16 data = 0;
    HMDRV_U8 oddSrc = (HMDRV_U8)((HMDRV_U32)nSrcAddr & 0x01);
    HMDRV_U8 oddDst = (HMDRV_U8)((HMDRV_U32)nDestAddr & 0x01);





    //pCopyToSrc  = ps;
    //pCopyToDest = pd;
    //pCopyToSize = nSize;
    /* FIX INVALID Address Infineon */
    //if (ps == 0) return 0;
    //if (pd == 0) return 0;




    if (oddSrc == 0)
    {
        if (oddDst == 0)
        {
            while(nSize--)
            {
                data = *ps++;

                if (nSize > 0)
                {
                    *pd++ = data;
                    nSize--;
                }
                else
                {
                    *pd &= 0xFF00;
                    *pd |= (data & 0xFF);
                }
            }
        }
        else
        {
            while(nSize--)
            {
                data = *ps++;

                *pd   &= 0x00FF;
                *pd++ |= (data << 8);

                if (nSize--)
                {
                   *pd &= 0xFF00;
                   *pd |= (data >> 8);
                }
                else
                {
                    break;
                }
            }
        }

    }
    else
    {
        if (oddDst == 1)
        {
            data = *ps;
            while(nSize--)
            {
                *pd   &= 0x00FF;
                *pd++ |= (data & 0xFF00);

                *pd++ = data >> 8;
                if (nSize--)
                {
                    data = *ps++;
                    *pd   &= 0xFF00;
                    *pd   |= (data & 0xFF);
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            data = *ps;
            while(nSize--)
            {
                *pd   &= 0xFF00;
                *pd   |= (data >> 8);

                *pd++ = data >> 8;
                if (nSize--)
                {
                    data = *ps++;
                    *pd   &= 0x00FF;
                    *pd++ |= (data << 8);
                }
                else
                {
                    break;
                }
            }

        }

    }
#elif HMDRV_CFG_BUS_WIDTH == 2

    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* ps = (HMDRV_U16*)(HMDRV_ADDR_SIZE)nSrcAddr;
    HMDRV_U16* pd = (HMDRV_U16*)(HMDRV_ADDR_SIZE)nDestAddr;
    HMDRV_U16 data;

    if(nSize == 1)
    {
        nSize = 2;
    }

    while(nSize)
    {
        data = *ps++;
        *pd++ = data;
        nSize-=2;
    }


#else
    HMDRV_U8* ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE) nSrcAddr;
    HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE) nDestAddr;

    while(nSize--)
        *pd++ = *ps++;
#endif


}




/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/

HMDRV_BPTR HMDRV_CopyFromBufferInternal(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_U16* pd = (HMDRV_U16*)(((HMDRV_U32)nDestAddr) >> 1);
    HMDRV_U16 data = 0;
    HMDRV_U8 data8 = 0;
    HMDRV_U16 data16 = 0;
    HMDRV_U8 odd = (HMDRV_U8)((HMDRV_U32)nDestAddr & 0x01);

    //pCopyFromSrc  = ps;
    //pCopyFromDest = pd;
    //pCopyFromSize = nSize;

    data = *pd;

    if (odd == 0)
    {
        while(nSize--)
        {
            data = *pd;

            data &= 0xFF00;
            data8 = *ps++;
            data8 &= 0x00FF;
            data += (HMDRV_U16)data8;

            if (nSize--)
            {
                data &= 0x00FF;
                data8 = *ps++;
                data16 = (HMDRV_U16)data8 << 8;
                data += data16;
                *pd++ = data;
            }
            else
            {
                *pd = data;
                break;
            }

        }

    }
    else
    {

        while(nSize--)
        {
            data &= 0x00FF;
            data8 = *ps++;
            data16 = (HMDRV_U16)data8 << 8;
            data += data16;
            *pd++ = data;

            if (nSize--)
            {
                data = *pd;

                data &= 0xFF00;
                data8 = *ps++;
                data8 &= 0x00FF;
                data += (HMDRV_U16)data8;
                if (nSize == 0)
                {
                    *pd = data;
                }
            }
            else
            {
                *pd = data;
                break;
            }


        }

    }
#elif HMDRV_CFG_BUS_WIDTH == 2
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* pd = (HMDRV_U16*)(HMDRV_ADDR_SIZE)nDestAddr;
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_U16 data;
    while(nSize)
    {
        data = *ps++;
        if ((nSize & 1) == 0)
        {
            data += (HMDRV_U16)*ps++ << 8;
            nSize--;
        }
        *pd++ = data;
        nSize--;
    }
#else

    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff; 
    
    nDestAddr = HMRDV_FixEdsAddress(nDestAddr); /* fix U32 eds Address */
    
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;
    
    if (nDestAddr > 0xFFFF)
    {
        /* forbidden psv write - just advance read buffer pointer */
        ps += nSize;
        return (HMDRV_BPTR) ps;
    }
    
    
    HMDRV_SIZE8 nSizeBytes = nSize;      
    while(nSizeBytes > 0)
    {
        *pd++ = *ps++;
        nSizeBytes--;
    }

#endif
    return (HMDRV_BPTR) ps;
}


/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/

HMDRV_BPTR HMDRV_CopyFromBufferInternalCoil(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_U16* pd = (HMDRV_U16*)(((HMDRV_U32)nDestAddr) >> 1);
    HMDRV_U16 data = 0;
    HMDRV_U8 data8 = 0;
    HMDRV_U16 data16 = 0;
    HMDRV_U8 odd = (HMDRV_U8)((HMDRV_U32)nDestAddr & 0x01);

    //pCopyFromSrc  = ps;
    //pCopyFromDest = pd;
    //pCopyFromSize = nSize;

    data = *pd;

    if (odd == 0)
    {
        while(nSize--)
        {
            data = *pd;

            data &= 0xFF00;
            data8 = *ps++;
            data8 &= 0x00FF;
            data += (HMDRV_U16)data8;

            if (nSize--)
            {
                data &= 0x00FF;
                data8 = *ps++;
                data16 = (HMDRV_U16)data8 << 8;
                data += data16;
                *pd++ = data;
            }
            else
            {
                *pd = data;
                break;
            }

        }

    }
    else
    {

        while(nSize--)
        {
            data &= 0x00FF;
            data8 = *ps++;
            data16 = (HMDRV_U16)data8 << 8;
            data += data16;
            *pd++ = data;

            if (nSize--)
            {
                data = *pd;

                data &= 0xFF00;
                data8 = *ps++;
                data8 &= 0x00FF;
                data += (HMDRV_U16)data8;
                if (nSize == 0)
                {
                    *pd = data;
                }
            }
            else
            {
                *pd = data;
                break;
            }


        }

    }
#elif HMDRV_CFG_BUS_WIDTH == 2
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* pd = (HMDRV_U16*)(HMDRV_ADDR_SIZE)nDestAddr;
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_U16 data;
    while(nSize)
    {
        data = *ps++;
        if ((nSize & 1) == 0)
        {
            data += (HMDRV_U16)*ps++ << 8;
            nSize--;
        }
        *pd++ = data;
        nSize--;
    }
#else

    


    #if 1

    HMDRV_SIZE8 nBitOffset = 0;
    HMDRV_SIZE8 nBitOffsetDest = 0;
    
    HMDRV_U8 nextByte;
    HMDRV_U8 readBit;
    
    
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff; 
    nDestAddr = HMRDV_FixEdsAddress(nDestAddr); /* fix U32 eds Address */
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;
    if (nDestAddr > 0xFFFF)
    {
        /* forbidden psv write - just advance read buffer pointer */
        ps += ( (nSize + 7) >> 3 );
        return (HMDRV_BPTR) ps;
    }

    while(nSize--)
    {
        nextByte = *ps;

        if (nextByte & (1 << nBitOffset))
        {
            readBit = 1;
        }
        else
        {
            readBit = 0;
        }

    #if 1
        if (readBit)
        {
            *pd |= (1 << nBitOffsetDest);
        }
        else
        {
            *pd &= ~(1 << nBitOffsetDest);
        }
    #else
        if (nBitOffsetDest == 0)
        {
            *pd = readBit;
        }
        else
        {
            *pd |= (readBit << nBitOffsetDest);
        }
    #endif

        nBitOffsetDest++;
        if (nBitOffsetDest == 0)
        {
            pd++;
        }

        nBitOffset++;
        if (nBitOffset == 0)
        {
            ps++;
        }

    }
    /* fix point to next byte in buffer */
    if (nBitOffset != 0)
    {
        ps++;
    }

    
    #else
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff; 
    nDestAddr = HMRDV_FixEdsAddress(nDestAddr); /* fix U32 eds Address */
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;
    if (nDestAddr > 0xFFFF)
    {
        /* forbidden psv write - just advance read buffer pointer */
        ps += nSize;
        return (HMDRV_BPTR) ps;
    }
    HMDRV_SIZE8 nSizeBytes = nSize;      
    while(nSizeBytes > 0)
    {
        *pd++ = *ps++;
        nSizeBytes--;
    }
    #endif

    
    
#endif
    return (HMDRV_BPTR) ps;
}


/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/

HMDRV_BPTR HMDRV_CopyFromBufferInternalSingleCoil(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff)
{
#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_U16* pd = (HMDRV_U16*)(((HMDRV_U32)nDestAddr) >> 1);
    HMDRV_U16 data = 0;
    HMDRV_U8 data8 = 0;
    HMDRV_U16 data16 = 0;
    HMDRV_U8 odd = (HMDRV_U8)((HMDRV_U32)nDestAddr & 0x01);

    //pCopyFromSrc  = ps;
    //pCopyFromDest = pd;
    //pCopyFromSize = nSize;

    data = *pd;

    if (odd == 0)
    {
        while(nSize--)
        {
            data = *pd;

            data &= 0xFF00;
            data8 = *ps++;
            data8 &= 0x00FF;
            data += (HMDRV_U16)data8;

            if (nSize--)
            {
                data &= 0x00FF;
                data8 = *ps++;
                data16 = (HMDRV_U16)data8 << 8;
                data += data16;
                *pd++ = data;
            }
            else
            {
                *pd = data;
                break;
            }

        }

    }
    else
    {

        while(nSize--)
        {
            data &= 0x00FF;
            data8 = *ps++;
            data16 = (HMDRV_U16)data8 << 8;
            data += data16;
            *pd++ = data;

            if (nSize--)
            {
                data = *pd;

                data &= 0xFF00;
                data8 = *ps++;
                data8 &= 0x00FF;
                data += (HMDRV_U16)data8;
                if (nSize == 0)
                {
                    *pd = data;
                }
            }
            else
            {
                *pd = data;
                break;
            }


        }

    }
#elif HMDRV_CFG_BUS_WIDTH == 2
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* pd = (HMDRV_U16*)(HMDRV_ADDR_SIZE)nDestAddr;
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_U16 data;
    while(nSize)
    {
        data = *ps++;
        if ((nSize & 1) == 0)
        {
            data += (HMDRV_U16)*ps++ << 8;
            nSize--;
        }
        *pd++ = data;
        nSize--;
    }
#else

    #if 1
    HMDRV_SIZE8 nBitOffset = 0;
    HMDRV_SIZE8 nBitOffsetDest = 0;
    
    HMDRV_U8 nextByte;
    HMDRV_U8 readBit;

    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff; 
    
    nDestAddr = HMRDV_FixEdsAddress(nDestAddr); /* fix U32 eds Address */
    
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;
    
    if (nDestAddr > 0xFFFF)
    {
        /* forbidden psv write - just advance read buffer pointer */
        ps += 1;
        return (HMDRV_BPTR) ps;
    }
    nextByte = *ps;
    
    if (nextByte & (1 << nBitOffset))
    {
        readBit = 1;
    }
    else
    {
        readBit = 0;
    }
    
#if 1
    if (readBit)
    {
        *pd |= (1 << nBitOffsetDest);
    }
    else
    {
        *pd &= ~(1 << nBitOffsetDest);
    }
#else
    if (nBitOffsetDest == 0)
    {
        *pd = readBit;
    }
    else
    {
        *pd |= (readBit << nBitOffsetDest);
    }
#endif
    
    nBitOffsetDest++;
    if (nBitOffsetDest == 0)
    {
        pd++;
    }
    

    #else
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff; 
    
    nDestAddr = HMRDV_FixEdsAddress(nDestAddr); /* fix U32 eds Address */
    
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;
    
    if (nDestAddr > 0xFFFF)
    {
        /* forbidden psv write - just advance read buffer pointer */
        ps += nSize;
        return (HMDRV_BPTR) ps;
    }
    HMDRV_SIZE8 nSizeBytes = nSize;      
    while(nSizeBytes > 0)
    {
        *pd++ = *ps++;
        nSizeBytes--;
    }
    #endif
#endif
    return (HMDRV_BPTR) ps;
}



#if HMDRV_USE_EXT_MEM == 0
/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer, perform AND-masking
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - source memory in communication buffer, mask follows data
* @param  nSize     - buffer size (always in bytes)
*
******************************************************************************/

void HMDRV_CopyFromBufferInternalWithMask(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;
    HMDRV_U8* pm = ps + nSize;
    HMDRV_U8 mask, stmp, dtmp;

    while(nSize--)
    {
        mask = *pm++;
        stmp = *ps++;
        dtmp = *pd;

        /* perform AND-masking */
        stmp = (HMDRV_U8) ((stmp & mask) | (dtmp & ~mask));

        /* put the result back */
        *pd++ = stmp;
    }
#elif HMDRV_CFG_BUS_WIDTH == 2

    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* pd = (HMDRV_U16*)(HMDRV_ADDR_SIZE)nDestAddr;
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_U16 mask, stmp, dtmp;
    HMDRV_U8* pm = ps + nSize;

    while(nSize)
    {
        mask = *pm++;
        stmp = *ps++;
        dtmp = *pd;
        if ((nSize & 1) == 0)
        {
            mask += (HMDRV_U16)*pm++ << 8;
            stmp += (HMDRV_U16)*ps++ << 8;
            nSize--;
        }

        /* perform AND-masking */
        stmp = ((stmp & mask) | (dtmp & ~mask));


        *pd++ = stmp;

        nSize--;
    }

#else
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;
    HMDRV_U8* pm = ps + nSize;
    HMDRV_U8 mask, stmp, dtmp;

    while(nSize--)
    {
        mask = *pm++;
        stmp = *ps++;
        dtmp = *pd;

        /* perform AND-masking */
        stmp = (HMDRV_U8) ((stmp & mask) | (dtmp & ~mask));

        /* put the result back */
        *pd++ = stmp;
     }
#endif
}

#endif


volatile uint32_t AddressEds; 
volatile uint32_t AddressEdsNoConv; 
volatile uint32_t AddressPsv; 
volatile uint32_t AddressPsvNoConv; 
volatile uint32_t AddressSub; 
volatile uint32_t AddressIn; 
volatile uint32_t AddressOut; 
//int8_t CSMON_sAccessLevelPasswordSelectedNonConstNoModifierAssign[256];

//HMDRV_ADDR_PNTR_MODIFIER uint8_t* CSMON_pasAccessLevelPasswordNonConstNoModifierAssign = NULL;

/**************************************************************************//*!
*
* @brief  Write-into the communication buffer memory
*
* @param  pDestBuff - pointer to destination memory in communication buffer
* @param  nSrcAddr  - source memory address
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/
HMDRV_BPTR HMDRV_CopyToBufferInternal(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{

#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY

    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* ps = (HMDRV_U16*)(((HMDRV_U32)nSrcAddr) >> 1);
    HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;
    HMDRV_U16 data = 0;
    HMDRV_U8 odd = (HMDRV_U8)((HMDRV_U32)nSrcAddr & 0x01);
    if (odd == 0)
    {
        while(nSize--)
        {
            data = *ps++;
            *pd++ = data & 0xFF;
            if (nSize--)
            {
                *pd++ = data >> 8;
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        data = *ps;
        while(nSize--)
        {
            *pd++ = data >> 8;
            if (nSize--)
            {
                data = *ps++;
                *pd++ = data & 0xFF;
            }
            else
            {
                break;
            }
        }
    }
#elif HMDRV_CFG_BUS_WIDTH == 2

    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* ps = (HMDRV_U16*)(HMDRV_ADDR_SIZE)nSrcAddr;
    HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;
    HMDRV_U16 data;
    while(nSize)
    {
        data = *ps++;
        *pd++ = data & 0x00FF;

        if ((nSize & 1) == 0)
        {
            *pd++ = data >> 8;
            nSize--;
        }
        nSize--;
    }


#else

    //__psv__ HMDRV_U8* ps_psv_conv = (__psv__ HMDRV_U8*)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    //__psv__ HMDRV_U8* ps_psv_noconv = nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    //__eds__ HMDRV_U8* psEds = (__eds__ HMDRV_U8*)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    __eds__ HMDRV_U8* ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    __psv__ HMDRV_U8* ps_psv = (__psv__ HMDRV_U8*)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    volatile HMDRV_BPTR pd = pDestBuff;
	volatile HMDRV_SIZE8 nSizeLoop = nSize;
    
    if (nSrcAddr > 0xFFFF)
    {   
        //AddressPsvNoConv = ps_psv_noconv;
        //AddressEds = psEds;
        //AddressEdsNoConv = ps;
        //AddressPsv = ps_psv_conv;
        //AddressIn = nSrcAddr;
        //AddressOut = ps_psv;
        
        ps_psv = (__psv__ HMDRV_U8*)(HMRDV_FixEdsAddress(ps_psv));
        
        while(nSizeLoop > 0)
        {
            *pd++ = *ps_psv++;
            nSizeLoop--;
        }
    }
    else
    {  
        while(nSizeLoop > 0)
        {
            *pd++ = *ps++;
            nSizeLoop--;
        }
    }
#endif

    return pd;
}


/**************************************************************************//*!
*
* @brief  Write-into the communication buffer memory
*
* @param  pDestBuff - pointer to destination memory in communication buffer
* @param  nSrcAddr  - source memory address
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/
HMDRV_BPTR HMDRV_CopyToBufferInternalCoil(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{

#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY

    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* ps = (HMDRV_U16*)(((HMDRV_U32)nSrcAddr) >> 1);
    HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;
    HMDRV_U16 data = 0;
    HMDRV_U8 odd = (HMDRV_U8)((HMDRV_U32)nSrcAddr & 0x01);
    if (odd == 0)
    {
        while(nSize--)
        {
            data = *ps++;
            *pd++ = data & 0xFF;
            if (nSize--)
            {
                *pd++ = data >> 8;
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        data = *ps;
        while(nSize--)
        {
            *pd++ = data >> 8;
            if (nSize--)
            {
                data = *ps++;
                *pd++ = data & 0xFF;
            }
            else
            {
                break;
            }
        }
    }
#elif HMDRV_CFG_BUS_WIDTH == 2

    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* ps = (HMDRV_U16*)(HMDRV_ADDR_SIZE)nSrcAddr;
    HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;
    HMDRV_U16 data;
    while(nSize)
    {
        data = *ps++;
        *pd++ = data & 0x00FF;

        if ((nSize & 1) == 0)
        {
            *pd++ = data >> 8;
            nSize--;
        }
        nSize--;
    }


#else

    #if 1

    HMDRV_SIZE8 nBitOffset = 0;
    HMDRV_SIZE8 nBitOffsetDest = 0;
    
    HMDRV_U8 nextByte;
    HMDRV_U8 readBit;
    volatile HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;
    __eds__ HMDRV_U8* ps;   //was HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* ps;
    __psv__ HMDRV_U8* ps_psv = (__psv__ HMDRV_U8*)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    
    ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    if (nSrcAddr > 0xFFFF)
    {
        ps_psv = (__psv__ HMDRV_U8*)(HMRDV_FixEdsAddress(ps_psv));
    }

    while(nSize--)
    {
    
        if (nSrcAddr > 0xFFFF)
        {
            nextByte = *ps_psv;
        }
        else
        {
            nextByte = *ps;
        }

        if (nextByte & (1 << nBitOffset))
        {
            readBit = 1;
        }
        else
        {
            readBit = 0;
        }

    #if 1
        if (readBit)
        {
            *pd |= (1 << nBitOffsetDest);
        }
        else
        {
            *pd &= ~(1 << nBitOffsetDest);
        }
    #else
        if (nBitOffsetDest == 0)
        {
            *pd = readBit;
        }
        else
        {
            *pd |= (readBit << nBitOffsetDest);
        }
    #endif

        nBitOffsetDest++;
        if (nBitOffsetDest == 0)
        {
            pd++;
        }

        nBitOffset++;
        if (nBitOffset == 0)
        {
            ps++;
            ps_psv++;
        }

    }
    
    /* fix point to next byte in buffer */
    if (nBitOffset != 0)
    {
        ps++;
    }
    
    
    #else
    //__psv__ HMDRV_U8* ps_psv_conv = (__psv__ HMDRV_U8*)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    //__psv__ HMDRV_U8* ps_psv_noconv = nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    //__eds__ HMDRV_U8* psEds = (__eds__ HMDRV_U8*)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    __eds__ HMDRV_U8* ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    __psv__ HMDRV_U8* ps_psv = (__psv__ HMDRV_U8*)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    volatile HMDRV_BPTR pd = pDestBuff;
	volatile HMDRV_SIZE8 nSizeLoop = nSize;
    
    if (nSrcAddr > 0xFFFF)
    {   
        //AddressPsvNoConv = ps_psv_noconv;
        //AddressEds = psEds;
        //AddressEdsNoConv = ps;
        //AddressPsv = ps_psv_conv;
        //AddressIn = nSrcAddr;
        //AddressOut = ps_psv;
        
        ps_psv = (__psv__ HMDRV_U8*)(HMRDV_FixEdsAddress(ps_psv));
        
        while(nSizeLoop > 0)
        {
            *pd++ = *ps_psv++;
            nSizeLoop--;
        }
    }
    else
    {  
        while(nSizeLoop > 0)
        {
            *pd++ = *ps++;
            nSizeLoop--;
        }
    }
    #endif
#endif

    return (HMDRV_BPTR)pd;
}




/**************************************************************************//*!
*
* @brief  Write-into the communication buffer memory
*
* @param  pDestBuff - pointer to destination memory in communication buffer
* @param  nSrcAddr  - source memory address
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/
HMDRV_BPTR HMDRV_CopyToBufferInternalSingleCoil(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr)
{

#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY

    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* ps = (HMDRV_U16*)(((HMDRV_U32)nSrcAddr) >> 1);
    HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;
    HMDRV_U16 data = 0;
    HMDRV_U8 odd = (HMDRV_U8)((HMDRV_U32)nSrcAddr & 0x01);
    if (odd == 0)
    {
        while(nSize--)
        {
            data = *ps++;
            *pd++ = data & 0xFF;
            if (nSize--)
            {
                *pd++ = data >> 8;
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        data = *ps;
        while(nSize--)
        {
            *pd++ = data >> 8;
            if (nSize--)
            {
                data = *ps++;
                *pd++ = data & 0xFF;
            }
            else
            {
                break;
            }
        }
    }
#elif HMDRV_CFG_BUS_WIDTH == 2

    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* ps = (HMDRV_U16*)(HMDRV_ADDR_SIZE)nSrcAddr;
    HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;
    HMDRV_U16 data;
    while(nSize)
    {
        data = *ps++;
        *pd++ = data & 0x00FF;

        if ((nSize & 1) == 0)
        {
            *pd++ = data >> 8;
            nSize--;
        }
        nSize--;
    }


#else

    #if 1

    HMDRV_SIZE8 nBitOffset = 0; /* to be fixed !!! */
    
    HMDRV_U8 nextByte;
    HMDRV_U8 readBit;
    volatile HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;
    __eds__ HMDRV_U8* ps;   //was HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* ps;
    __psv__ HMDRV_U8* ps_psv = (__psv__ HMDRV_U8*)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    
    ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    if (nSrcAddr > 0xFFFF)
    {
        ps_psv = (__psv__ HMDRV_U8*)(HMRDV_FixEdsAddress(ps_psv));
    }

    if (nSrcAddr > 0xFFFF)
    {
        nextByte = *ps_psv;
    }
    else
    {
        nextByte = *ps;
    }
    if (nextByte & (1 << nBitOffset))
    {
        readBit = 1;
        *pd++ = 0xFF;
    }
    else
    {
        readBit = 0;
        *pd++ = 0x00;
    }
    *pd++ = 0;
    

    
    
    
    #else
    //__psv__ HMDRV_U8* ps_psv_conv = (__psv__ HMDRV_U8*)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    //__psv__ HMDRV_U8* ps_psv_noconv = nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    //__eds__ HMDRV_U8* psEds = (__eds__ HMDRV_U8*)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    __eds__ HMDRV_U8* ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    __psv__ HMDRV_U8* ps_psv = (__psv__ HMDRV_U8*)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    volatile HMDRV_BPTR pd = pDestBuff;
	volatile HMDRV_SIZE8 nSizeLoop = nSize;
    
    if (nSrcAddr > 0xFFFF)
    {   
        //AddressPsvNoConv = ps_psv_noconv;
        //AddressEds = psEds;
        //AddressEdsNoConv = ps;
        //AddressPsv = ps_psv_conv;
        //AddressIn = nSrcAddr;
        //AddressOut = ps_psv;
        
        ps_psv = (__psv__ HMDRV_U8*)(HMRDV_FixEdsAddress(ps_psv));
        
        while(nSizeLoop > 0)
        {
            *pd++ = *ps_psv++;
            nSizeLoop--;
        }
    }
    else
    {  
        while(nSizeLoop > 0)
        {
            *pd++ = *ps++;
            nSizeLoop--;
        }
    }
    #endif
#endif

    return (HMDRV_BPTR)pd;
}






#if HMDRV_USE_MODBUS

/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/
HMDRV_BPTR HMDRV_CopyFromBufferInternalModbus(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize, HMDRV_U8 u8ElementSize, HMDRV_U8* pu8ElementSizeArray, HMDRV_BOOL bStringNullTerminationPossible)
{
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    
    nDestAddr = HMRDV_FixEdsAddress(nDestAddr); /* fix U32 eds Address */
    
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;
    
    if (nDestAddr > 0xFFFF)
    {
        /* forbidden psv write */
        ps = NULL;
        return (HMDRV_BPTR) ps;
    }

	//HM_pCopyFromSrc  = ps;
	//HM_pCopyFromDest = pd;
	//HM_pCopyFromSize = nSize;
    
    HMDRV_SIZE8 nSizeBytes; 
    
   
#if HMDRV_ADDR_WIDTH == 2
    volatile uint16_t* pd16;
    HMDRV_U16 u16TempData;
#endif
    
    HMDRV_U8 u8ElementSizeCounter = 0;
    HMDRV_U8 u8ElementSizeArrayIndex = 0;
    HMDRV_U8 u8ElementSizeNow = 0;
    
    
    nSizeBytes = nSize; 
    while(nSizeBytes > 0)
    {
        /* Extract u8ElementSizeCounter */
        if (u8ElementSizeCounter == 0)
        {
            if (pu8ElementSizeArray == NULL)
            {
                u8ElementSizeNow = u8ElementSize;
            }
            else
            {
                if (u8ElementSizeArrayIndex >= u8ElementSize)
                {
                    u8ElementSizeArrayIndex = 0;
                }
                u8ElementSizeNow = pu8ElementSizeArray[u8ElementSizeArrayIndex++]; 
            }
            
            if (u8ElementSizeNow == HMDRV_ELEMENT_SIZE_STRING)
            {
                u8ElementSizeCounter = HMDRV_ELEMENT_SIZE_STRING;  
            }
            else
            {
                u8ElementSizeCounter = u8ElementSizeNow;
            }
            
        }
        
        if (u8ElementSizeNow == HMDRV_ELEMENT_SIZE_STRING)
        {
            /* string expected */
            while(nSizeBytes > 0)
            {
                *pd++ = *ps;
                if (*ps++ == 0)
                {
                    nSizeBytes = 0;
                }
                else
                {
                    nSizeBytes--;
                    if (nSizeBytes == 0) /* force null termination */
                    {
                        *pd = 0;
                    }
                }  
            }
        }        
        else
        {
#if 0 //before 2020 01 14
            /* process copy fixed size data */
            
            if (((nSize&1) == 1) && (u8ElementSizeNow == nSize)) /* if odd bytes and single element  -> get padding zero first */
            {
                ps++;  
            }
            
            while ((u8ElementSizeCounter > 0) && (nSizeBytes > 0))
            {
                u8ElementSizeCounter--;
                *(pd+u8ElementSizeCounter) = *ps++;
                nSizeBytes--;   





                if (nSizeBytes == 0)
                {
                    if (((nSize&1) == 1) && (u8ElementSizeNow != nSize))/* if odd bytes and not single element  -> send padding zero at end */
                    {
                        ps++;
                    }
                }
            }
            pd += u8ElementSizeNow;
#endif
            /* process copy fixed size data */
            if (((nSize&1) == 1) && (u8ElementSizeNow == nSize)) /* if odd bytes and single element  -> get padding zero first */
            {
                ps++;
            }

#if HMDRV_ADDR_WIDTH == 2
            pd16 = (uint16_t*)pd + ((u8ElementSizeNow+1)>>1)-1;
#endif
            //u8ElementSizeNowAddressAdd = 0;

            while ((u8ElementSizeCounter > 0) && (nSizeBytes > 0))
            {
#if HMDRV_ADDR_WIDTH == 2

                //odd u8ElementSizeCounter
                if (u8ElementSizeCounter & 1 == 1)
                {
                    *pd16 = (*pd16 & 0xFF00) | ((*ps++) & 0x00FF);  /* preserve current high byte */
                    //*pd16 = (*ps++) & 0x00FF;
                    u8ElementSizeCounter--;
                    nSizeBytes--;
                }
                //even u8ElementSizeCounter
                else
                {
                    u16TempData = ((uint16_t)(*ps++) << 8);
                    u16TempData |= ((*ps++) & 0x00FF);
                    *pd16 = u16TempData;
                    u8ElementSizeCounter -= 2;
                    nSizeBytes -= 2;
                }
                pd16--;

                //u8ElementSizeNowAddressAdd++;

#else
                u8ElementSizeCounter--;
                *(pd+u8ElementSizeCounter) = *ps++;
                nSizeBytes--;
                //u8ElementSizeNowAddressAdd++;
#endif
                
                if (nSizeBytes == 0)
                {
                    if (((nSize&1) == 1) && (u8ElementSizeNow != nSize))/* if odd bytes and not single element  -> send padding zero at end */
                    {
                        ps++;
                    }
                }
            }

            // HMDRV_ADDR_WIDTH dependency made below
            //pd += u8ElementSizeNowAddressAdd;//2020 06 10 was pd += u8ElementSizeNow;
            //pd += u8ElementSizeNow;

#if HMDRV_ADDR_WIDTH == 2

            if (u8ElementSizeNow == 1)
            {
                u8ElementSizeNow = 2; /* uint8_t if 2 bytes long  */
            }
            pd += u8ElementSizeNow >> 1;


#else
            pd += u8ElementSizeNow;
#endif


        }
    }
    
	
    return (HMDRV_BPTR) ps;
}

/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/
HMDRV_BPTR HMDRV_CopyFromBufferInternalModbusCoil(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nBitOffset, HMDRV_SIZE8 nBitOffsetDest)
{
    HMDRV_U8 nextByte;
    HMDRV_U8 readBit;
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    
    nDestAddr = HMRDV_FixEdsAddress(nDestAddr); /* fix U32 eds Address */
    
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;
    
    if (nDestAddr > 0xFFFF)
    {
        /* forbidden psv write */
        ps = NULL;
        return (HMDRV_BPTR) ps;
    }

    nextByte = *ps;
    
    if (nextByte & (1 << nBitOffset))
    {
        readBit = 1;
    }
    else
    {
        readBit = 0;
    }
    
#if 1
    if (readBit)
    {
        *pd |= (1 << nBitOffsetDest);
    }
    else
    {
        *pd &= ~(1 << nBitOffsetDest);
    }
#else
    if (nBitOffsetDest == 0)
    {
        *pd = readBit;
    }
    else
    {
        *pd |= (readBit << nBitOffsetDest);
    }
#endif

    return (HMDRV_BPTR) ps;
}





/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/
HMDRV_BPTR HMDRV_CopyFromBufferInternalModbusSingleCoil(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nBitOffset)
{
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_U8 inByte;
    HMDRV_U8 inByteL;
    HMDRV_U8 readBit;
    
    nDestAddr = HMRDV_FixEdsAddress(nDestAddr); /* fix U32 eds Address */
    
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;
    
    if (nDestAddr > 0xFFFF)
    {
        /* forbidden psv write */
        ps = NULL;
        return (HMDRV_BPTR) ps;
    }
    
    inByte = *ps++;
    inByteL = *ps++;
    if (inByteL != 0x00)
    {
        ps = NULL;
    }
    else
    {
        //inByte = *ps++;
        if (inByte == 0xFF)
        {
            readBit = 1;
        }
        else if (inByte == 0x00)
        {
            readBit = 0;
        }
        else
        {
            ps = NULL;
            return (HMDRV_BPTR) ps;
        }
        
        /* do the write */
        *pd &= ~(1 << nBitOffset);  /* clear first */
        *pd |= (readBit << nBitOffset);
    }
	
    return (HMDRV_BPTR) ps;
}



/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer, perform AND-masking
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - source memory in communication buffer, mask follows data
* @param  nSize     - buffer size (always in bytes)
*
******************************************************************************/


void HMDRV_CopyFromBufferInternalWithMaskModbus(HMDRV_ADDR nDestAddr, HMDRV_ADDR nSrcAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* po = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;   /* old data */
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;  /* new data */
    HMDRV_U8* pm = pSrcBuff;
    HMDRV_U8 maskAnd, maskOr, stmp;

    HMDRV_SIZE8 nSizeBytes = nSize;      
    while(nSizeBytes > 0)
    { 
        if (nSizeBytes >= 2)
        {
            /* Low Byte */
            maskAnd = *(pm+1);
            maskOr = *(pm+3);
            stmp = *po++;
            /* perform masking */
            stmp = (HMDRV_U8) ((stmp & maskAnd) | (maskOr & ~maskAnd));
            /* put the result back */
            *pd++ = stmp;            
            
            /* High Byte */
            maskAnd = *(pm+0);
            maskOr = *(pm+2);
            stmp = *po++;
            /* perform masking */
            stmp = (HMDRV_U8) ((stmp & maskAnd) | (maskOr & ~maskAnd));
            /* put the result back */
            *pd++ = stmp;            
            

            nSizeBytes -= 2;
            pm += 4;
        }
        else /* if (nSizeBytes == 1) */
        {  
            
            /* Low Byte */
            maskAnd = *(pm+1);
            maskOr = *(pm+3);
            stmp = *po++;
            /* perform masking */
            stmp = (HMDRV_U8) ((stmp & maskAnd) | (maskOr & ~maskAnd));
            /* put the result back */
            *pd++ = stmp;            
            
            nSizeBytes -= 1;
            pm += 4;
        }
    }
}

#if HMDRV_USE_EXT_MEM == 0
/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer, perform AND-masking
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - source memory in communication buffer, mask follows data
* @param  nSize     - buffer size (always in bytes)
*
******************************************************************************/

HMDRV_BOOL HMDRV_CopyFromBufferWithMaskModbus(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
    HMDRV_BOOL result = HMDRV_FALSE;
    //HMDRV_U8* ps;
    //HMDRV_U8* pd;
    HMDRV_ADDR pDestAddr;
    HMDRV_ADDR pSrcAddr;
    HMDRV_ADDR nSrcAddr;
    HMDRV_BPTR pDestBuff;

    pDestAddr = nDestAddr;

    pDestBuff = pSrcBuff;
    pSrcAddr = pDestAddr;
    nSrcAddr = nDestAddr;

    HMDRV_CopyFromBufferInternalWithMaskModbus(pDestAddr, pSrcAddr, pSrcBuff, nSize);

    result = HMDRV_TRUE;


    return result;
}

#endif


/**************************************************************************//*!
*
* @brief  Write-into the communication buffer memory
*
* @param  pDestBuff - pointer to destination memory in communication buffer
* @param  nSrcAddr  - source memory address
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*         Data in buffer is written in 16-bit big-endian mode Modbus compatible
*
******************************************************************************/
HMDRV_BPTR HMDRV_CopyToBufferInternalModbus(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize, HMDRV_U8 u8ElementSize, HMDRV_U8* pu8ElementSizeArray)
{
    HMDRV_U8 nextByte;
    volatile HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* ps;   //was HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* ps;    2020 01 05 was __eds__ HMDRV_U8* ps;
    HMDRV_CNST_PNTR_MODIFIER HMDRV_U8* ps_psv = (HMDRV_CNST_PNTR_MODIFIER HMDRV_U8*)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;

    
    ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    if (nSrcAddr > 0xFFFF)
    {
        ps_psv = (HMDRV_CNST_PNTR_MODIFIER HMDRV_U8*)(HMRDV_FixEdsAddress(ps_psv));
    }


    //psTest2 = (HMDRV_U8*)nSrcAddr;
    
    volatile HMDRV_SIZE8 nSizeBytes;
    
    volatile HMDRV_U8 u8ElementSizeCounter = 0;
    volatile HMDRV_U8 u8ElementSizeArrayIndex = 0;
    volatile HMDRV_U8 u8ElementSizeNow = 0;
       
    nSizeBytes = nSize; 
  
    while(nSizeBytes > 0)
    {
        /* Extract u8ElementSizeCounter */
        if (u8ElementSizeCounter == 0)
        {
            if (pu8ElementSizeArray == NULL)
            {
                u8ElementSizeNow = u8ElementSize;
            }
            else
            {
                if (u8ElementSizeArrayIndex >= u8ElementSize)
                {
                    u8ElementSizeArrayIndex = 0;
                }
                u8ElementSizeNow = pu8ElementSizeArray[u8ElementSizeArrayIndex++]; 
            }
            
            if (u8ElementSizeNow == HMDRV_ELEMENT_SIZE_STRING)
            {
                u8ElementSizeCounter = HMDRV_ELEMENT_SIZE_STRING;  
            }
            else
            {
                u8ElementSizeCounter = u8ElementSizeNow;
            }
            
        }
        
        
        if (u8ElementSizeNow == HMDRV_ELEMENT_SIZE_STRING)
        {
            /* string expected */
            while(nSizeBytes > 0)
            {
                if (nSrcAddr > 0xFFFF)
                {
                    nextByte = *ps_psv;
                }
                else
                {
                    nextByte = *ps;
                }
                
                
                
                if (nextByte == 0)
                {
                    if (u8ElementSizeCounter == 0)
                    {
                        *pd++ = 0;
                        u8ElementSizeCounter++;
                    }
                    /* force string null termination if needed */
                    if ((u8ElementSizeCounter & 1) > 0)
                    {
                        *pd++ = 0;
                        u8ElementSizeCounter++;
                    }
                    
                    nSizeBytes = 0;
                    
                }
                else
                {
                    if (nSrcAddr > 0xFFFF)
                    {
                        *pd++ = *ps_psv++;
                    }
                    else
                    {
                        *pd++ = *ps++;
                    }
                    
                    u8ElementSizeCounter++;                
                    nSizeBytes--; 
                    
                    if (nSizeBytes == 0)
                    {
                        /* force string null termination if needed */
                        if ((u8ElementSizeCounter & 1) > 0)
                        {
                            *pd++ = 0;
                            u8ElementSizeCounter++;
                        } 
                    }
                    
                }
                
            }
        }
        else
        {
            /* process copy fixed size data */
            
            if (((nSize&1) == 1) && (u8ElementSizeNow == nSize)) /* if odd bytes and single element  -> send padding zero first */
            {
                *pd++ = 0;  
            }
            
            while ((u8ElementSizeCounter > 0) && (nSizeBytes > 0))
            {
                u8ElementSizeCounter--;
                if (nSrcAddr > 0xFFFF)
                {
                    *pd++ = *(ps_psv+u8ElementSizeCounter);
                }
                else
                {
                    *pd++ = *(ps+u8ElementSizeCounter);
                }
                nSizeBytes--;   
                
                if (nSizeBytes == 0)
                {
                    if (((nSize&1) == 1) && (u8ElementSizeNow != nSize))/* if odd bytes and not single element  -> send padding zero at end */
                    {
                        *pd++ = 0;
                    }
                }
            }       
            ps += u8ElementSizeNow;
            ps_psv += u8ElementSizeNow;
        }

    }

    return (HMDRV_BPTR) pd;
}




/**************************************************************************//*!
*
* @brief  Write-into the communication buffer memory
*
* @param  pDestBuff - pointer to destination memory in communication buffer
* @param  nSrcAddr  - source memory address
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*         Data in buffer is written in 16-bit big-endian mode Modbus compatible
*
******************************************************************************/
HMDRV_BPTR HMDRV_CopyToBufferInternalModbusCoil(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nBitOffset, HMDRV_SIZE8 nBitOffsetDest)
{
    HMDRV_U8 nextByte;
    HMDRV_U8 readBit;
    volatile HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* ps;   //was HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* ps;
    HMDRV_CNST_PNTR_MODIFIER HMDRV_U8* ps_psv = (HMDRV_CNST_PNTR_MODIFIER HMDRV_U8*)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    
    ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    if (nSrcAddr > 0xFFFF)
    {
        ps_psv = (HMDRV_CNST_PNTR_MODIFIER HMDRV_U8*)(HMRDV_FixEdsAddress(ps_psv));
    }

    if (nSrcAddr > 0xFFFF)
    {
        nextByte = *ps_psv;
    }
    else
    {
        nextByte = *ps;
    }
    
    if (nextByte & (1 << nBitOffset))
    {
        readBit = 1;
    }
    else
    {
        readBit = 0;
    }
    
#if 1
    if (readBit)
    {
        *pd |= (1 << nBitOffsetDest);
    }
    else
    {
        *pd &= ~(1 << nBitOffsetDest);
    }
#else
    if (nBitOffsetDest == 0)
    {
        *pd = readBit;
    }
    else
    {
        *pd |= (readBit << nBitOffsetDest);
    }
#endif
    

    return (HMDRV_BPTR) pd;
}


/**************************************************************************//*!
*
* @brief  Write-into the communication buffer memory
*
* @param  pDestBuff - pointer to destination memory in communication buffer
* @param  nSrcAddr  - source memory address
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*         Data in buffer is written in 16-bit big-endian mode Modbus compatible
*
******************************************************************************/
HMDRV_BPTR HMDRV_CopyToBufferInternalModbusSingleCoil(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nBitOffset)
{
    HMDRV_U8 nextByte;
    HMDRV_U8 readBit;
    volatile HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* ps;   //was HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* ps;
    HMDRV_CNST_PNTR_MODIFIER HMDRV_U8* ps_psv = (HMDRV_CNST_PNTR_MODIFIER HMDRV_U8*)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    
    ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    if (nSrcAddr > 0xFFFF)
    {
        ps_psv = (HMDRV_CNST_PNTR_MODIFIER HMDRV_U8*)(HMRDV_FixEdsAddress(ps_psv));
    }

    if (nSrcAddr > 0xFFFF)
    {
        nextByte = *ps_psv;
    }
    else
    {
        nextByte = *ps;
    }
    if (nextByte & (1 << nBitOffset))
    {
        readBit = 1;
        *pd++ = 0xFF;
    }
    else
    {
        readBit = 0;
        *pd++ = 0x00;
    }
    *pd++ = 0;
    
    
    
    
    return (HMDRV_BPTR) pd;
}


#endif





/**************************************************************************//*!
*
* @brief  strlen function
*
* @param  nSrcAddr  - source memory address
* @param  nSize     - buffer size (always in bytes)
*
* @return string length
*
******************************************************************************/
HMDRV_SIZE8 HMDRV_GetStringLengthInternal(HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* ps;
    ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    HMDRV_CNST_PNTR_MODIFIER HMDRV_U8* ps_psv = (HMDRV_CNST_PNTR_MODIFIER HMDRV_U8*)nSrcAddr;

    if (nSrcAddr > 0xFFFF)
    {
        ps_psv = (HMDRV_CNST_PNTR_MODIFIER HMDRV_U8*)(HMRDV_FixEdsAddress(ps_psv));
    }
    
    HMDRV_SIZE8 nSizeBytes = 0;
    if (nSrcAddr > 0xFFFF)
    {
    while(nSizeBytes < nSize)
    {
        if (*ps_psv++ == NULL)
        {
            break;
        }
        nSizeBytes++;
    }
    }
    else
    {
        while(nSizeBytes < nSize)
        {
            if (*ps++ == NULL)
            {
                break;
            }
            nSizeBytes++;
        }
    }
    
    return nSizeBytes;
}















#if HMDRV_USE_EXT_MEM == 1

/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/

HMDRV_BPTR HMDRV_CopyFromBuffer(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
    HMDRV_U8* ps;
    HMDRV_ADDR pDestAddr;
    HMDRV_U16 u16ExtMemID;
    
    pDestAddr = HMDRV_pGetInternalMemoryAddressWrite(nDestAddr, nSize, &u16ExtMemID);
    
    if ((pDestAddr != NULL) || (nDestAddr == NULL))
    {
        ps = HMDRV_CopyFromBufferInternal(pDestAddr, pSrcBuff, nSize);
        
        if (pDestAddr != nDestAddr)
        {
            ProcessExternalMemory(pDestAddr, nDestAddr, nSize, &u16ExtMemID);
        }
    }
    else
    {
        ps = pSrcBuff;
        ps += nSize;
        
        //ps = NULL;
    }

    
    
    
    return (HMDRV_BPTR) ps;
}


/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer, perform AND-masking
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - source memory in communication buffer, mask follows data
* @param  nSize     - buffer size (always in bytes)
*
******************************************************************************/

volatile HMDRV_U8* psTest5;
volatile HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pdTest5;
volatile HMDRV_U8* pmTest5;
volatile HMDRV_U8  maskTest5;

void HMDRV_CopyFromBufferInternalWithMask(HMDRV_ADDR nDestAddr, HMDRV_ADDR nSrcAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* po = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_U8* pm = pSrcBuff + nSize;
    HMDRV_U8 mask, stmp, dtmp;

    while(nSize--)
    {
        pmTest5 = pm;
        
        mask = *pm++;
        
        maskTest5 = mask;    
        pdTest5 = pd;   
        psTest5 = ps;
        
        stmp = *ps++;
        dtmp = *po++;

        /* perform AND-masking */
        stmp = (HMDRV_U8) ((stmp & mask) | (dtmp & ~mask));

        /* put the result back */
        *pd++ = stmp;
    }
}





/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer, perform AND-masking
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - source memory in communication buffer, mask follows data
* @param  nSize     - buffer size (always in bytes)
*
******************************************************************************/
volatile HMDRV_ADDR pDestTest6;
volatile HMDRV_ADDR nDestTest6;
volatile HMDRV_ADDR pSrcTest6;



void HMDRV_CopyFromBufferWithMask(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
    //HMDRV_U8* ps;
    //HMDRV_U8* pd;
    HMDRV_ADDR pDestAddr;
    HMDRV_ADDR pSrcAddr;
    HMDRV_ADDR nSrcAddr;
    HMDRV_BPTR pDestBuff;
    HMDRV_SIZE8 nSizeRead;
    HMDRV_U16 u16ExtMemID;
    HMDRV_U16 u16ExtMemIDRead = 0xFFFF;
   
    HMDRV_SIZE8 nSizeReq = nSize;
    while(nSizeReq)
    {    
        pDestAddr = HMDRV_pGetInternalMemoryAddressRead(nDestAddr, nSizeReq, &nSizeRead, &u16ExtMemIDRead);
        nSizeReq -= nSizeRead;
        nSize = nSizeRead;
        
        pDestBuff = pSrcBuff;
        pSrcAddr = pDestAddr;
        nSrcAddr = nDestAddr;

        if ((pSrcAddr != NULL) || (nSrcAddr == NULL))
        {
            //pd = HMDRV_CopyToBufferInternal(pDestBuff, pSrcAddr, nSize);  

            pDestAddr = HMDRV_pGetInternalMemoryAddressWrite(nDestAddr, nSize, &u16ExtMemID);

            pDestTest6 = pDestAddr;
            nDestTest6 = nDestAddr;
            pSrcTest6 = pSrcAddr;

            if ((pDestAddr != NULL) || (nDestAddr == NULL))
            {
                HMDRV_CopyFromBufferInternalWithMask(pDestAddr, pSrcAddr, pSrcBuff, nSize);


                if (pDestAddr != nDestAddr)
                {
                    ProcessExternalMemory(pDestAddr, nDestAddr, nSize, &u16ExtMemID);
                }
            }
            else
            {
                //ps = pSrcBuff;
                //ps += nSize;
            }

        }
        else
        {
            /* raise error implement */
            //pd = pDestBuff;
            //while(nSize--) *pd++ = 0;   /* copy 0 data to Buffer on error */

            //ps = pSrcBuff;
            //ps += nSize;
        }
        #if HMDRV_USE_EXT_MEM
        #if HMDRV_USE_EXT_PMP
        PMPMEM_vReleaseQueueBuffer((HMDRV_U8*)(HMDRV_ADDR_SIZE)pSrcAddr);
        #endif
        #if HMDRV_USE_EXT_SPI
        SPIMEM_vReleaseQueueBuffer(&u16ExtMemIDRead);
        #endif
        #endif

        nDestAddr+= nSizeRead;        
    }

    //return (HMDRV_BPTR) pd;

    
    //return (HMDRV_BPTR) ps;
}

#define DEBUG_BYTE_ARRAY_READ_LEN   10
volatile HMDRV_U8 u8DebugDataArrayRead[DEBUG_BYTE_ARRAY_READ_LEN];

volatile HMDRV_ADDR u32DebugAddressRead;
volatile HMDRV_ADDR u32DebugAddressReadMin;
volatile HMDRV_ADDR u32DebugAddressReadMax;
volatile HMDRV_U8 u8DebugDataLowRead;
volatile HMDRV_U8 u8DebugDataHighRead;

/* *****************************************************************************
 * HMDRV_CopyToBuffer
 * 
 * Description: memcpy function when used external memory external module
 **************************************************************************** */
HMDRV_BPTR HMDRV_CopyToBuffer(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_U8* pd = NULL;
    volatile HMDRV_ADDR pSrcAddr;
    HMDRV_SIZE8 nSizeRead;
    
    HMDRV_SIZE8 nSizeReq = nSize;
    
    HMDRV_U16 u16ExtMemIDRead = 0xFFFF;
    HMDRV_U8* pDebug = NULL;
    
    volatile HMDRV_U16 u16IndexDebug = 0;
    
    while(nSizeReq)
    {    
        pSrcAddr = HMDRV_pGetInternalMemoryAddressRead(nSrcAddr, nSizeReq, &nSizeRead, &u16ExtMemIDRead);
        nSizeReq -= nSizeRead;
        nSize = nSizeRead;
        u16IndexDebug = 0;


        if (pSrcAddr != (HMDRV_ADDR)(HMDRV_ADDR_TO_PNTR_SIZE)&HMDRV_CopyToBuffer_pSrcAddr)
        {
            HMDRV_CopyToBuffer_pSrcAddr = pSrcAddr;
        }
        if ((pSrcAddr != NULL) || (nSrcAddr == NULL))
        {
            pDestBuff = HMDRV_CopyToBufferInternal(pDestBuff, pSrcAddr, nSize);
            
            for (u16IndexDebug = 0; u16IndexDebug < DEBUG_BYTE_ARRAY_READ_LEN; u16IndexDebug++)
            {
                pDebug = (HMDRV_U8*)(HMDRV_ADDR_SIZE_SHORT)pSrcAddr + u16IndexDebug;
                u8DebugDataArrayRead[u16IndexDebug] = *pDebug;
            }
            if (((u32DebugAddressRead == nSrcAddr) || ((u32DebugAddressReadMin <= nSrcAddr) && (u32DebugAddressReadMax >= nSrcAddr)))
                    && (u8DebugDataArrayRead[1] == u8DebugDataHighRead) && (u8DebugDataArrayRead[0] == u8DebugDataLowRead))
            {
                u8DebugDataArrayRead[9] = 5;
            }
            
        }
        else
        {
            /* raise error implement */
            pd = pDestBuff;
            while(nSize--) *pd++ = 'E';   /* copy 'E' data to Buffer on error */
            pDestBuff = pd;
            //pd = NULL;
        }
        #if HMDRV_USE_EXT_MEM
        #if HMDRV_USE_EXT_PMP
        PMPMEM_vReleaseQueueBuffer((HMDRV_U8*)(HMDRV_ADDR_SIZE)pSrcAddr);
        #endif
        #if HMDRV_USE_EXT_SPI
        SPIMEM_vReleaseQueueBuffer(&u16ExtMemIDRead);
        #endif
        #endif

        nSrcAddr+= nSizeRead;        
    }
    pd = pDestBuff;
    return (HMDRV_BPTR) pd;
}




#if HMDRV_USE_MODBUS

/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/
HMDRV_BPTR HMDRV_CopyFromBufferModbus(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize, HMDRV_U8 u8ElementSize, HMDRV_U8* pu8ElementSizeArray, HMDRV_BOOL bStringNullTerminationPossible)
{
    HMDRV_U8* ps;
    HMDRV_ADDR pDestAddr;
    HMDRV_U16 u16ExtMemID;
    
    pDestAddr = HMDRV_pGetInternalMemoryAddressWrite(nDestAddr, nSize, &u16ExtMemID);
    
    if ((pDestAddr != NULL) || (nDestAddr == NULL))
    {
        ps = HMDRV_CopyFromBufferInternalModbus(pDestAddr, pSrcBuff, nSize, u8ElementSize, pu8ElementSizeArray, bStringNullTerminationPossible);
            
        if (pDestAddr != nDestAddr)
        {
            ProcessExternalMemory(pDestAddr, nDestAddr, nSize, &u16ExtMemID);
        }
    }
    else
    {
        ps = pSrcBuff;
        ps += nSize;
        
        ps = NULL;  /* return exception */
    }

    
    
    
    return (HMDRV_BPTR) ps;
}

/* *****************************************************************************
 * HMDRV_CopyFromBufferModbusCoil
 * 
 * Description: memcpy function when used external memory external module
 **************************************************************************** */
HMDRV_BPTR HMDRV_CopyFromBufferModbusCoil(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nBitOffset, HMDRV_SIZE8 nBitOffsetDest)
{
    HMDRV_U8* ps;
    HMDRV_ADDR pDestAddr;
    HMDRV_U16 u16ExtMemID;
    
    pDestAddr = HMDRV_pGetInternalMemoryAddressWrite(nDestAddr, 1, &u16ExtMemID);
    
    if ((pDestAddr != NULL) || (nDestAddr == NULL))
    {
        ps = HMDRV_CopyFromBufferInternalModbusCoil(pDestAddr, pSrcBuff, nBitOffset, nBitOffsetDest);
            
        if (pDestAddr != nDestAddr)
        {
            ProcessExternalMemory(pDestAddr, nDestAddr, 1, &u16ExtMemID);
        }
    }
    else
    {
        ps = NULL;  /* return exception */
    }
    return (HMDRV_BPTR) ps;
}


/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/
HMDRV_BPTR HMDRV_CopyFromBufferModbusSingleCoil(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nBitOffset)
{
    HMDRV_U8* ps;
    HMDRV_ADDR pDestAddr;
    HMDRV_U16 u16ExtMemID;
    
    pDestAddr = HMDRV_pGetInternalMemoryAddressWrite(nDestAddr, 1, &u16ExtMemID);
    
    if ((pDestAddr != NULL) || (nDestAddr == NULL))
    {
        ps = HMDRV_CopyFromBufferInternalModbusSingleCoil(pDestAddr, pSrcBuff, nBitOffset);
            
        if (pDestAddr != nDestAddr)
        {
            ProcessExternalMemory(pDestAddr, nDestAddr, 1, &u16ExtMemID);
        }
    }
    else
    {
        ps = NULL;  /* return exception */
    }
    return (HMDRV_BPTR) ps;
}





/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer, perform AND-masking
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - source memory in communication buffer, mask follows data
* @param  nSize     - buffer size (always in bytes)
*
******************************************************************************/

HMDRV_BOOL HMDRV_CopyFromBufferWithMaskModbus(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
    HMDRV_BOOL result = HMDRV_FALSE;
    //HMDRV_U8* ps;
    //HMDRV_U8* pd;
    HMDRV_ADDR pDestAddr;
    HMDRV_ADDR pSrcAddr;
    HMDRV_ADDR nSrcAddr;
    HMDRV_BPTR pDestBuff;
    HMDRV_SIZE8 nSizeRead;
    HMDRV_U16 u16ExtMemID;
    HMDRV_U16 u16ExtMemIDRead = 0xFFFF;
    
    HMDRV_SIZE8 nSizeReq = nSize;
    while(nSizeReq)
    {    
        pDestAddr = HMDRV_pGetInternalMemoryAddressRead(nDestAddr, nSizeReq, &nSizeRead, &u16ExtMemIDRead);        
        nSizeReq -= nSizeRead;
        nSize = nSizeRead;

        pDestBuff = pSrcBuff;
        pSrcAddr = pDestAddr;
        nSrcAddr = nDestAddr;

        if ((pSrcAddr != NULL) || (nSrcAddr == NULL))
        {
            //pd = HMDRV_CopyToBufferInternal(pDestBuff, pSrcAddr, nSize);  

            pDestAddr = HMDRV_pGetInternalMemoryAddressWrite(nDestAddr, nSize, &u16ExtMemID);

            //pDestTest6 = pDestAddr;
           // nDestTest6 = nDestAddr;
            //pSrcTest6 = pSrcAddr;

            if ((pDestAddr != NULL) || (nDestAddr == NULL))
            {
                HMDRV_CopyFromBufferInternalWithMaskModbus(pDestAddr, pSrcAddr, pSrcBuff, nSize);   

                if (pDestAddr != nDestAddr)
                {
                    ProcessExternalMemory(pDestAddr, nDestAddr, nSize, &u16ExtMemID);
                }
                result = HMDRV_TRUE;
            }
            else
            {
                //ps = pSrcBuff;
                //ps += nSize;
            }

        }
        else
        {
            /* raise error implement */
            //pd = pDestBuff;
            //while(nSize--) *pd++ = 0;   /* copy 0 data to Buffer on error */

            //ps = pSrcBuff;
            //ps += nSize;
        }

        #if HMDRV_USE_EXT_MEM
        #if HMDRV_USE_EXT_PMP
        PMPMEM_vReleaseQueueBuffer((HMDRV_U8*)(HMDRV_ADDR_SIZE)pSrcAddr);
        #endif
        #if HMDRV_USE_EXT_SPI
        SPIMEM_vReleaseQueueBuffer(&u16ExtMemIDRead);
        #endif
        #endif

        nDestAddr+= nSizeRead;        

    }
    
    //return (HMDRV_BPTR) pd;

    
    //return (HMDRV_BPTR) ps;
    
    return result;
}




/* *****************************************************************************
 * HMDRV_CopyToBufferModbus
 * 
 * Description: memcpy function when used external memory external module
 **************************************************************************** */
HMDRV_BPTR HMDRV_CopyToBufferModbus(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize, HMDRV_U8 u8ElementSize, HMDRV_U8* pu8ElementSizeArray)
//HMDRV_BPTR HMDRV_CopyToBufferModbus(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize, HMDRV_BOOL bStringDataExpected)
{
    volatile HMDRV_U8* pd = NULL;
    HMDRV_ADDR pSrcAddr;
    HMDRV_SIZE8 nSizeRead;
    
    HMDRV_SIZE8 nSizeReq = nSize;
    HMDRV_U16 u16ExtMemIDRead = 0xFFFF;
    
    while(nSizeReq)
    {    
        pSrcAddr = HMDRV_pGetInternalMemoryAddressRead(nSrcAddr, nSizeReq, &nSizeRead, &u16ExtMemIDRead);
        nSizeReq -= nSizeRead;
        nSize = nSizeRead;

        if ((pSrcAddr != NULL) || (nSrcAddr == NULL))
        {
            pDestBuff = HMDRV_CopyToBufferInternalModbus(pDestBuff, (HMDRV_ADDR)pSrcAddr, nSize, u8ElementSize, pu8ElementSizeArray);
        }
        else
        {
            /* raise error implement */
            pd = pDestBuff;
            while(nSize--) *pd++ = 0;   /* copy 0 data to Buffer on error */

            pDestBuff = NULL;  /* return exception */
        }

        #if HMDRV_USE_EXT_MEM
        #if HMDRV_USE_EXT_PMP
        PMPMEM_vReleaseQueueBuffer((HMDRV_U8*)(HMDRV_ADDR_SIZE)pSrcAddr);
        #endif
        #if HMDRV_USE_EXT_SPI
        SPIMEM_vReleaseQueueBuffer(&u16ExtMemIDRead);
        #endif
        #endif

        nSrcAddr+= nSizeRead;        
    }
    
    pd = pDestBuff;
    
    return (HMDRV_BPTR) pd;
}


/* *****************************************************************************
 * HMDRV_CopyToBufferModbusCoil
 * 
 * Description: memcpy function when used external memory external module
 **************************************************************************** */
HMDRV_BPTR HMDRV_CopyToBufferModbusCoil(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nBitOffset, HMDRV_SIZE8 nBitOffsetDest)
{
    volatile HMDRV_U8* pd;
    HMDRV_ADDR pSrcAddr;
    HMDRV_SIZE8 nSizeRead;
    HMDRV_SIZE8 nSize = 1;
    
    HMDRV_SIZE8 nSizeReq = nSize;
    HMDRV_U16 u16ExtMemIDRead = 0xFFFF;
    
    while(nSizeReq)
    {    
        pSrcAddr = HMDRV_pGetInternalMemoryAddressRead(nSrcAddr, nSizeReq, &nSizeRead, &u16ExtMemIDRead);
        nSizeReq -= nSizeRead;
        nSize = nSizeRead;

        if ((pSrcAddr != NULL) || (nSrcAddr == NULL))
        {
            pDestBuff = HMDRV_CopyToBufferInternalModbusCoil(pDestBuff, (HMDRV_ADDR)pSrcAddr, nBitOffset, nBitOffsetDest);
        }
        else
        {
            /* raise error implement */
            pd = pDestBuff;
            *pd = 0;   /* copy 0 data to Buffer on error */

            pDestBuff = NULL;  /* return exception */
        }

        #if HMDRV_USE_EXT_MEM
        #if HMDRV_USE_EXT_PMP
        PMPMEM_vReleaseQueueBuffer((HMDRV_U8*)(HMDRV_ADDR_SIZE)pSrcAddr);
        #endif
        #if HMDRV_USE_EXT_SPI
        SPIMEM_vReleaseQueueBuffer(&u16ExtMemIDRead);
        #endif
        #endif

        nSrcAddr += nSizeRead;
    }
    pd = pDestBuff;
    return (HMDRV_BPTR) pd;
}


/* *****************************************************************************
 * HMDRV_CopyToBufferModbusCoil
 * 
 * Description: memcpy function when used external memory external module
 **************************************************************************** */
HMDRV_BPTR HMDRV_CopyToBufferModbusSingleCoil(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nBitOffset)
{
    volatile HMDRV_U8* pd;
    HMDRV_ADDR pSrcAddr;
    HMDRV_SIZE8 nSizeRead;
    HMDRV_SIZE8 nSize = 1;
    
    HMDRV_SIZE8 nSizeReq = nSize;
    HMDRV_U16 u16ExtMemIDRead = 0xFFFF;
    
    while(nSizeReq)
    {    
        pSrcAddr = HMDRV_pGetInternalMemoryAddressRead(nSrcAddr, nSizeReq, &nSizeRead, &u16ExtMemIDRead);
        nSizeReq -= nSizeRead;
        nSize = nSizeRead;

        if ((pSrcAddr != NULL) || (nSrcAddr == NULL))
        {
            pDestBuff = HMDRV_CopyToBufferInternalModbusSingleCoil(pDestBuff, (HMDRV_ADDR)pSrcAddr, nBitOffset);
        }
        else
        {
            /* raise error implement */
            pd = pDestBuff;
            *pd = 0;   /* copy 0 data to Buffer on error */

            pDestBuff = NULL;  /* return exception */
        }

        #if HMDRV_USE_EXT_MEM
        #if HMDRV_USE_EXT_PMP
        PMPMEM_vReleaseQueueBuffer((HMDRV_U8*)(HMDRV_ADDR_SIZE)pSrcAddr);
        #endif
        #if HMDRV_USE_EXT_SPI
        SPIMEM_vReleaseQueueBuffer(&u16ExtMemIDRead);
        #endif
        #endif

        nSrcAddr += nSizeRead;
    }
    pd = pDestBuff;
    return (HMDRV_BPTR) pd;
}


/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/

HMDRV_BPTR HMDRV_CopyFromBufferWithSuccess(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
    HMDRV_U8* ps;
    HMDRV_ADDR pDestAddr;
    HMDRV_U16 u16ExtMemID;
    
    pDestAddr = HMDRV_pGetInternalMemoryAddressWrite(nDestAddr, nSize, &u16ExtMemID);
    
    if ((pDestAddr != NULL) || (nDestAddr == NULL))
    {
        ps = HMDRV_CopyFromBufferInternal(pDestAddr, pSrcBuff, nSize);
        
        if (pDestAddr != nDestAddr)
        {
            ProcessExternalMemory(pDestAddr, nDestAddr, nSize, &u16ExtMemID);
        }
    }
    else
    {
        ps = pSrcBuff;
        ps += nSize;
        
        ps = NULL;
    }

    
    
    
    return (HMDRV_BPTR) ps;
}


/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/

HMDRV_BPTR HMDRV_CopyFromBufferWithSuccessCoil(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
    HMDRV_U8* ps;
    HMDRV_ADDR pDestAddr;
    HMDRV_U16 u16ExtMemID;
    
    pDestAddr = HMDRV_pGetInternalMemoryAddressWrite(nDestAddr, nSize, &u16ExtMemID);
    
    if ((pDestAddr != NULL) || (nDestAddr == NULL))
    {
        ps = HMDRV_CopyFromBufferInternalCoil(pDestAddr, pSrcBuff, nSize);
        
        if (pDestAddr != nDestAddr)
        {
            ProcessExternalMemory(pDestAddr, nDestAddr, nSize, &u16ExtMemID);
        }
    }
    else
    {
        ps = pSrcBuff;
        ps += nSize;
        
        ps = NULL;
    }

    
    
    
    return (HMDRV_BPTR) ps;
}



/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/

HMDRV_BPTR HMDRV_CopyFromBufferWithSuccessSingleCoil(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff)
{
    HMDRV_U8* ps;
    HMDRV_ADDR pDestAddr;
    HMDRV_U16 u16ExtMemID;
    
    pDestAddr = HMDRV_pGetInternalMemoryAddressWrite(nDestAddr, 1, &u16ExtMemID);
    
    if ((pDestAddr != NULL) || (nDestAddr == NULL))
    {
        ps = HMDRV_CopyFromBufferInternalSingleCoil(pDestAddr, pSrcBuff);
        
        if (pDestAddr != nDestAddr)
        {
            ProcessExternalMemory(pDestAddr, nDestAddr, 1, &u16ExtMemID);
        }
    }
    else
    {
        ps = NULL;
    }

    return (HMDRV_BPTR) ps;
}





/* *****************************************************************************
 * HMDRV_CopyToBuffer
 * 
 * Description: memcpy function when used external memory external module
 **************************************************************************** */
HMDRV_BPTR HMDRV_CopyToBufferWithSuccess(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_U8* pd = NULL;
    HMDRV_ADDR pSrcAddr;
    HMDRV_SIZE8 nSizeRead;
    
    HMDRV_SIZE8 nSizeReq = nSize;
    HMDRV_U16 u16ExtMemIDRead = 0xFFFF;
    
    while(nSizeReq)
    {    
        pSrcAddr = HMDRV_pGetInternalMemoryAddressRead(nSrcAddr, nSizeReq, &nSizeRead, &u16ExtMemIDRead);
        nSizeReq -= nSizeRead;
        nSize = nSizeRead;

        if ((pSrcAddr != NULL) || (nSrcAddr == NULL))
        {
            pDestBuff = HMDRV_CopyToBufferInternal(pDestBuff, pSrcAddr, nSize);
        }
        else
        {
            /* raise error implement */
            pd = pDestBuff;
            while(nSize--) *pd++ = 0;   /* copy 0 data to Buffer on error */

            pDestBuff = NULL;
        }
        #if HMDRV_USE_EXT_MEM
        #if HMDRV_USE_EXT_PMP
        PMPMEM_vReleaseQueueBuffer((HMDRV_U8*)(HMDRV_ADDR_SIZE)pSrcAddr);
        #endif
        #if HMDRV_USE_EXT_SPI
        SPIMEM_vReleaseQueueBuffer(&u16ExtMemIDRead);
        #endif
        #endif

        nSrcAddr += nSizeRead;
    }
    pd = pDestBuff;
    return (HMDRV_BPTR) pd;
}


/* *****************************************************************************
 * HMDRV_CopyToBuffer
 * 
 * Description: memcpy function when used external memory external module
 **************************************************************************** */
HMDRV_BPTR HMDRV_CopyToBufferWithSuccessCoil(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_U8* pd = NULL;
    HMDRV_ADDR pSrcAddr;
    HMDRV_SIZE8 nSizeRead;
    
    HMDRV_SIZE8 nSizeReq = nSize;
    HMDRV_U16 u16ExtMemIDRead = 0xFFFF;
    
    while(nSizeReq)
    {    
        pSrcAddr = HMDRV_pGetInternalMemoryAddressRead(nSrcAddr, nSizeReq, &nSizeRead, &u16ExtMemIDRead);
        nSizeReq -= nSizeRead;
        nSize = nSizeRead;

        if ((pSrcAddr != NULL) || (nSrcAddr == NULL))
        {
            pDestBuff = HMDRV_CopyToBufferInternalCoil(pDestBuff, pSrcAddr, nSize);
        }
        else
        {
            /* raise error implement */
            pd = pDestBuff;
            while(nSize--) *pd++ = 0;   /* copy 0 data to Buffer on error */

            pDestBuff = NULL;
        }

        #if HMDRV_USE_EXT_MEM
        #if HMDRV_USE_EXT_PMP
        PMPMEM_vReleaseQueueBuffer((HMDRV_U8*)(HMDRV_ADDR_SIZE)pSrcAddr);
        #endif
        #if HMDRV_USE_EXT_SPI
        SPIMEM_vReleaseQueueBuffer(&u16ExtMemIDRead);
        #endif
        #endif

        nSrcAddr += nSizeRead;
    }
    pd = pDestBuff;
    return (HMDRV_BPTR) pd;
}



/* *****************************************************************************
 * HMDRV_CopyToBuffer
 * 
 * Description: memcpy function when used external memory external module
 **************************************************************************** */
HMDRV_BPTR HMDRV_CopyToBufferWithSuccessSingleCoil(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr)
{
    HMDRV_U8* pd;
    HMDRV_ADDR pSrcAddr;
    HMDRV_SIZE8 nSizeRead;
    HMDRV_SIZE8 nSize = 1;
    
    HMDRV_SIZE8 nSizeReq = nSize;
    HMDRV_U16 u16ExtMemIDRead = 0xFFFF;
    
    while(nSizeReq)
    {    
        pSrcAddr = HMDRV_pGetInternalMemoryAddressRead(nSrcAddr, nSizeReq, &nSizeRead, &u16ExtMemIDRead);
        nSizeReq -= nSizeRead;
        nSize = nSizeRead;

        if ((pSrcAddr != NULL) || (nSrcAddr == NULL))
        {
            pDestBuff = HMDRV_CopyToBufferInternalSingleCoil(pDestBuff, pSrcAddr);
        }
        else
        {
            /* raise error implement */
            pd = pDestBuff;
            *pd++ = 0;   /* copy 0 data to Buffer on error */
            *pDestBuff   = 0;   /* copy 0 data to Buffer on error */

            pd = NULL;
        }
        
        #if HMDRV_USE_EXT_MEM
        #if HMDRV_USE_EXT_PMP
        PMPMEM_vReleaseQueueBuffer((HMDRV_U8*)(HMDRV_ADDR_SIZE)pSrcAddr);
        #endif
        #if HMDRV_USE_EXT_SPI
        SPIMEM_vReleaseQueueBuffer(&u16ExtMemIDRead);
        #endif
        #endif

        nSrcAddr += nSizeRead;
    }
    pd = pDestBuff;
    return (HMDRV_BPTR) pd;
}






#endif           /* #if HMDRV_USE_MODBUS */




/* *****************************************************************************
 * HMDRV_GetStringLength
 * 
 * Description: strlen function when used external memory external module
 **************************************************************************** */
HMDRV_SIZE8 HMDRV_GetStringLength(HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_ADDR pSrcAddr;
    
    //HMDRV_SIZE8 nSizeFbck = nSize;
    HMDRV_SIZE8 nSizeFbckLoop = 0;
    HMDRV_SIZE8 nSizeRead;
    
    HMDRV_SIZE8 nSizeReq = nSize;
    HMDRV_U16 u16ExtMemIDRead = 0xFFFF;
    
    while(nSizeReq)
    {    
        pSrcAddr = HMDRV_pGetInternalMemoryAddressRead(nSrcAddr, nSizeReq, &nSizeRead, &u16ExtMemIDRead);
        nSizeReq -= nSizeRead;
        nSize = nSizeRead;

        if ((pSrcAddr != NULL) || (nSrcAddr == NULL))
        {
            nSize = HMDRV_GetStringLengthInternal((HMDRV_ADDR)pSrcAddr, nSize);
            nSizeFbckLoop += nSize;
        }

        #if HMDRV_USE_EXT_MEM
        #if HMDRV_USE_EXT_PMP
        PMPMEM_vReleaseQueueBuffer((HMDRV_U8*)(HMDRV_ADDR_SIZE)pSrcAddr);
        #endif
        #if HMDRV_USE_EXT_SPI
        SPIMEM_vReleaseQueueBuffer(&u16ExtMemIDRead);
        #endif
        #endif

        nSrcAddr += nSizeRead;
    }
    return nSizeFbckLoop;
}



HMDRV_BOOL HMDRV_bIsExternalAddress(HMDRV_ADDR nDestAddr)
{
    HMDRV_BOOL bResult = HMDRV_FALSE;
    if (nDestAddr >= EXT_MEM_MIN_VIRTUAL_ADDRESS)
    {
        bResult = HMDRV_TRUE;
    }
    return bResult;
}

HMDRV_ADDR  HMDRV_pSnapshotAddressGet(HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize, HMDRV_BUFF_PNTR_MODIFIER HMDRV_U16* pu16GlobalQueueID)
{
    if (nDestAddr >= EXT_MEM_SPI_MIN_VIRTUAL_ADDRESS)
    {
        #if HMDRV_USE_EXT_SPI
        return SPIMEM_pBufferAddressGet(nDestAddr, nSize, pu16GlobalQueueID);
        #else
        return NULL;
        #endif
    }
    else
    {
        #if HMDRV_USE_EXT_PMP
        return PMPMEM_pBufferAddressGet(nDestAddr, nSize);
        #else
        return NULL;
        #endif
    }
    
}

void HMDRV_CopySnapshotToMemory(HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize, HMDRV_BUFF_PNTR_MODIFIER HMDRV_U16* pu16GlobalQueueID)
{
    if (nDestAddr >= EXT_MEM_SPI_MIN_VIRTUAL_ADDRESS)
    {
        #if HMDRV_USE_EXT_SPI
        SPIMEM_vSendBufferToSPI(nSize, pu16GlobalQueueID);
        #endif

    }
    else
    {
        #if HMDRV_USE_EXT_PMP
        PMPMEM_vSendBufferToPMP(nSize);
        #endif
    }
}





/* *****************************************************************************
 * HMDRV_pGetInternalMemoryAddressRead
 *
 * Description: convert virtual to real address
 **************************************************************************** */
HMDRV_ADDR HMDRV_pGetInternalMemoryAddressRead(HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize, HMDRV_ADDR_PNTR_MODIFIER HMDRV_SIZE8* pnSizeRead, HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* pu16ID)
{
    HMDRV_ADDR pSrcAddr;
    
    *pnSizeRead = nSize;
    if (nSrcAddr >= EXT_MEM_MIN_VIRTUAL_ADDRESS)
    {
        if (nSrcAddr >= EXT_MEM_SPI_MIN_VIRTUAL_ADDRESS)
        {
            #if HMDRV_USE_EXT_SPI
            pSrcAddr = SPIMEM_pExtMemRamToIntRam(nSrcAddr, nSize, pnSizeRead, pu16ID);
            #else
            pSrcAddr = NULL;
            #endif
        }
        else
        {
            #if USE_SPI_FLIR
            if (nSrcAddr >= EXT_MEM_FLIR_CCI_TWI_VIRTUAL_ADDRESS)
            {
                #if HMDRV_USE_EXT_I2C
                pSrcAddr = I2CMEM_pFlirRegToIntRam(nSrcAddr, nSize);
                #else
                pSrcAddr = NULL;
                #endif
            }
            else
            #endif
            if (nSrcAddr >= EXT_MEM_I2C_VIRTUAL_ADDRESS)
            {
                #if HMDRV_USE_EXT_I2C
                pSrcAddr = I2CMEM_pExtMemRamToIntRam(nSrcAddr, nSize);
                #else
                pSrcAddr = NULL;
                #endif
            }
            else
            {
                #if HMDRV_USE_EXT_PMP
                pSrcAddr = PMPMEM_pExtMemRamToIntRam(nSrcAddr, nSize);
                #else
                pSrcAddr = NULL;
                #endif
            }
        }
    }
    else
    {
        pSrcAddr = nSrcAddr;
    }
    return pSrcAddr;
}




/* *****************************************************************************
 * HMDRV_pGetInternalMemoryAddressWrite
 *
 * Description: convert virtual to real address
 **************************************************************************** */
HMDRV_ADDR HMDRV_pGetInternalMemoryAddressWrite(HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize, HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* pu16ID)
{
    HMDRV_ADDR pDestAddr;
    
    if (nDestAddr >= EXT_MEM_MIN_VIRTUAL_ADDRESS)
    {
        if (nDestAddr >= EXT_MEM_SPI_MIN_VIRTUAL_ADDRESS)
        {
            #if HMDRV_USE_EXT_SPI
            pDestAddr = SPIMEM_pExtMemRamFromIntRam(nDestAddr, nSize, pu16ID);
            #else
            pDestAddr = NULL;   /* Error Address */
            #endif

        }
        else
        {
            if (nDestAddr >= EXT_MEM_I2C_VIRTUAL_ADDRESS)
            {
              #if HMDRV_USE_EXT_I2C
                #if USE_SPI_FLIR
                if (nDestAddr >= EXT_MEM_FLIR_CCI_TWI_VIRTUAL_ADDRESS)
                {
                    pDestAddr = I2CMEM_pFlirRegFromIntRam();
                }
                else
                #endif
                {
                    pDestAddr = I2CMEM_pExtMemRamFromIntRam(nDestAddr);
                }
              #else
                pDestAddr = NULL;   /* Error Address */
              #endif
            }
            else
            {
                #if HMDRV_USE_EXT_PMP
                pDestAddr = PMPMEM_pExtMemRamFromIntRam(nSize);
                #else
                pDestAddr = NULL;   /* Error Address */
                #endif

            }
        }
    }
    else
    {
        pDestAddr = nDestAddr;
    }
    return pDestAddr;
}


/* *****************************************************************************
 * ProcessExternalMemory
 *
 * Description: Update data in Extended memory area
 *
 **************************************************************************** */
void ProcessExternalMemory(HMDRV_ADDR nInternalRamBufferAddr, HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize, HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* pu16ID)
{
    if (nDestAddr >= EXT_MEM_MIN_VIRTUAL_ADDRESS)
    {
        if (nDestAddr >= EXT_MEM_SPI_MIN_VIRTUAL_ADDRESS)
        {
            #if HMDRV_USE_EXT_SPI
            SPIMEM_vExtMemRamFromIntRamCopyData(pu16ID);
            #endif
        }
        else
        {
            
            #if USE_SPI_FLIR
            if (nDestAddr >= EXT_MEM_FLIR_CCI_TWI_VIRTUAL_ADDRESS)
            {
                #if HMDRV_USE_EXT_I2C
                I2CMEM_vFlirRegFromIntRamCopyData(nDestAddr, nSize);
                #endif
            }
            else
            #endif
            if (nDestAddr >= EXT_MEM_I2C_VIRTUAL_ADDRESS)
            {
                #if HMDRV_USE_EXT_I2C
                I2CMEM_vExtMemRamFromIntRamCopyData(nDestAddr, nSize);
                #endif
            }
            else
            {
                #if HMDRV_USE_EXT_PMP
                PMPMEM_vExtMemRamFromIntRamCopyData(nInternalRamBufferAddr, nDestAddr, nSize);
                #endif
            }
        }
    }
}






#endif      /* #if HMDRV_USE_EXT_MEM == 1 */




/* *****************************************************************************
 * EX Commands Handling (Extended Address Commands)
 **************************************************************************** */

/* Used EX Commands? */
#if (HMDRV_USE_EX_CMDS)

/* mixed EX and no-EX commands? */
#if (HMDRV_USE_NOEX_CMDS)

/**************************************************************************//*!
*
* @brief  When mixed EX and no-EX command may occur, this variable is
*         here to remember what command is just being handled.
*
******************************************************************************/

static HMDRV_BOOL pcm_bNextAddrIsEx;

void HMDRV_SetExAddr(HMDRV_BOOL bNextAddrIsEx)
{
    pcm_bNextAddrIsEx = bNextAddrIsEx;
}

/* only no-EX commands (we are in LARGE model) */
#else

/**************************************************************************//*!
*
* @brief  When mixed EX and no-EX commands are disabled, we are sure
*         only the EX are used (we are in LARGE mode here).
*
******************************************************************************/

static const HMDRV_BOOL pcm_bNextAddrIsEx = 1;

#endif /* mixed EX and no-EX commands */



/**************************************************************************//*!
*
* @brief  Store (global) address to communication buffer
*
******************************************************************************/

HMDRV_BPTR HMDRV_AddressToBufferBigEndian(HMDRV_BPTR pDest, HMDRV_ADDR nAddr)
{
    if(pcm_bNextAddrIsEx)
    {
        //*(HMDRV_U32*) pDest = ((HMDRV_U32)nAddr);
        *pDest++ = (HMDRV_U8)((HMDRV_U8)((HMDRV_U32)nAddr>>24) & 0x00FF); 
        *pDest++ = (HMDRV_U8)((HMDRV_U8)((HMDRV_U32)nAddr>>16) & 0x00FF); 
        *pDest++ = (HMDRV_U8)((HMDRV_U8)((HMDRV_U32)nAddr>> 8) & 0x00FF); 
        *pDest++ = (HMDRV_U8)((HMDRV_U8)((HMDRV_U32)nAddr>> 0) & 0x00FF); 
    }
    else
    {
        //*(HMDRV_U16*) pDest = ((HMDRV_U16)nAddr);
        *pDest++ = (HMDRV_U8)((HMDRV_U8)((HMDRV_U16)nAddr>> 8) & 0x00FF); 
        *pDest++ = (HMDRV_U8)((HMDRV_U8)((HMDRV_U16)nAddr>> 0) & 0x00FF); 
    }
    
    return pDest;
}

/**************************************************************************//*!
*
* @brief  Store (global) address to communication buffer
*
******************************************************************************/

HMDRV_BPTR HMDRV_AddressToBuffer(HMDRV_BPTR pDest, HMDRV_ADDR nAddr)
{
#if 0       //HMDRV_BIG_TO_LITTLE_ENDIAN
    //first work with TI ??? same as below???
    HMDRV_U32 addr = (HMDRV_U32)nAddr;
    //addr <<= 1;
    *pDest++ = ((HMDRV_U8)(addr >>  0)) & 0xFF;
    *pDest++ = ((HMDRV_U8)(addr >>  8)) & 0xFF;
    *pDest++ = ((HMDRV_U8)(addr >> 16)) & 0xFF;
    *pDest++ = ((HMDRV_U8)(addr >> 24)) & 0xFF;
#else
    if(pcm_bNextAddrIsEx)
    {
        //*(HMDRV_U32*) pDest = ((HMDRV_U32)nAddr);
        *pDest++ = (HMDRV_U8)((HMDRV_U8)((HMDRV_U32)nAddr>> 0) & 0x00FF); 
        *pDest++ = (HMDRV_U8)((HMDRV_U8)((HMDRV_U32)nAddr>> 8) & 0x00FF); 
        *pDest++ = (HMDRV_U8)((HMDRV_U8)((HMDRV_U32)nAddr>>16) & 0x00FF); 
        *pDest++ = (HMDRV_U8)((HMDRV_U8)((HMDRV_U32)nAddr>>24) & 0x00FF); 
    }
    else
    {
        //*(HMDRV_U16*) pDest = ((HMDRV_U16)nAddr);
        *pDest++ = (HMDRV_U8)((HMDRV_U8)((HMDRV_U16)nAddr>> 0) & 0x00FF); 
        *pDest++ = (HMDRV_U8)((HMDRV_U8)((HMDRV_U16)nAddr>> 8) & 0x00FF); 
    }
#endif

    return pDest;
}

/**************************************************************************//*!
*
* @brief  Fetch address from communication buffer
*
******************************************************************************/

HMDRV_BPTR HMDRV_AddressFromBufferBigEndian(HMDRV_ADDR_PNTR_MODIFIER HMDRV_ADDR* pAddr, HMDRV_BPTR pSrc)
{
    /* treat address as long word to determine the kind of the address */
    HMDRV_U32 addr = 0;

    if(pcm_bNextAddrIsEx)
    {
        addr = (HMDRV_U32)(
                ((HMDRV_U32)(*(HMDRV_U8*)(pSrc+0))<<24) + 
                ((HMDRV_U32)(*(HMDRV_U8*)(pSrc+1))<<16) + 
                ((HMDRV_U32)(*(HMDRV_U8*)(pSrc+2))<< 8) + 
                ((HMDRV_U32)(*(HMDRV_U8*)(pSrc+3))<< 0));
        pSrc += 4;
    }
    else
    {
        
        addr = (HMDRV_U32)(
                ((HMDRV_U16)(*(HMDRV_U8*)(pSrc+0))<< 8) + 
                ((HMDRV_U16)(*(HMDRV_U8*)(pSrc+1))<< 0));

        pSrc += 2;
    }
    
    *pAddr = (HMDRV_ADDR)addr;
    
    return pSrc;
}

/**************************************************************************//*!
*
* @brief  Fetch address from communication buffer
*
******************************************************************************/

HMDRV_BPTR HMDRV_AddressFromBuffer(HMDRV_ADDR_PNTR_MODIFIER HMDRV_ADDR* pAddr, HMDRV_BPTR pSrc)
{
    HMDRV_U32 addr = 0;
#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY
     HMDRV_U32 src = 0;

     src = *pSrc++;
     src &= 0x000000FF;
     addr =  src;
     src = *pSrc++;
     src &= 0x000000FF;
     src <<= 8;
     addr += src;
     src = *pSrc++;
     src &= 0x000000FF;
     src <<= 16;
     addr += src;
     src = *pSrc++;
     src &= 0x000000FF;
     src <<= 24;
     addr += src;
     //addr >>=1;
     *pAddr = (HMDRV_ADDR)addr;
     return pSrc;
#elif HMDRV_CFG_BUS_WIDTH == 2
     HMDRV_U16 src = 0;

     src = *pSrc++;
     src += *pSrc++ << 8;
     addr = src;

     if(pcm_bNextAddrIsEx)
     {
         src = *pSrc++;
         src += *pSrc++ << 8;
         addr += (HMDRV_U32)src << 16;
     }

     *pAddr = (HMDRV_ADDR)addr;
     return pSrc;
#else
     
     /* treat address as long word to determine the kind of the address */

    if(pcm_bNextAddrIsEx)
    {
        addr = (HMDRV_U32)(
                ((HMDRV_U32)(*(HMDRV_U8*)(pSrc+0))<< 0) + 
                ((HMDRV_U32)(*(HMDRV_U8*)(pSrc+1))<< 8) + 
                ((HMDRV_U32)(*(HMDRV_U8*)(pSrc+2))<<16) + 
                ((HMDRV_U32)(*(HMDRV_U8*)(pSrc+3))<<24));
        pSrc += 4;
    }
    else
    {
        
        addr = (HMDRV_U32)(
                ((HMDRV_U16)(*(HMDRV_U8*)(pSrc+0))<< 0) + 
                ((HMDRV_U16)(*(HMDRV_U8*)(pSrc+1))<< 8));

        pSrc += 2;
    }
    
    *pAddr = (HMDRV_ADDR)addr;
    
    return pSrc;
#endif

}




#endif  /* Used EX Commands */



#endif  /* #if (!HMDRV_DISABLE) */





#if USE_OLD_MASKING
/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer, perform AND-masking
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - source memory in communication buffer, mask follows data
* @param  nSize     - buffer size (always in bytes)
*
******************************************************************************/

void HMDRV_CopyFromBufferWithMaskInternal(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;
    HMDRV_U8* pm = ps + nSize;
    HMDRV_U8 mask, stmp, dtmp;

    while(nSize--)
    {
        mask = *pm++;
        stmp = *ps++;
        dtmp = *pd;

        /* perform AND-masking */
        stmp = (HMDRV_U8) ((stmp & mask) | (dtmp & ~mask));

        /* put the result back */
        *pd++ = stmp;
    }
}

/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer, perform AND-masking
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - source memory in communication buffer, mask follows data
* @param  nSize     - buffer size (always in bytes)
*
******************************************************************************/

void HMDRV_CopyFromBufferWithMask(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
    if (nDestAddr >= EXT_MEM_MIN_VIRTUAL_ADDRESS)
    {
        if (nDestAddr >= EXT_MEM_SPI_MIN_VIRTUAL_ADDRESS)
        {
            SPIMEM_vCopyToExtMemWithMask(nDestAddr, pSrcBuff, nSize);
        }
        else
        {
            #if USE_SPI_FLIR
            if (nDestAddr >= EXT_MEM_FLIR_CCI_TWI_VIRTUAL_ADDRESS)
            {
                I2CMEM_vCopyToFlirRegWithMask(nDestAddr, pSrcBuff, nSize);
            }
            else
            #endif
            if (nDestAddr >= EXT_MEM_I2C_VIRTUAL_ADDRESS)
            {
                I2CMEM_vCopyToExtMemWithMask(nDestAddr, pSrcBuff, nSize);
            }
            else
            {
                #if HMDRV_USE_EXT_PMP
                PMPMEM_vCopyToExtMemWithMask(nDestAddr, pSrcBuff, nSize);
                #endif        
            }
        }
    }
    else
    {
        HMDRV_CopyFromBufferWithMaskInternal(nDestAddr, pSrcBuff, nSize);
    }
}

#endif







