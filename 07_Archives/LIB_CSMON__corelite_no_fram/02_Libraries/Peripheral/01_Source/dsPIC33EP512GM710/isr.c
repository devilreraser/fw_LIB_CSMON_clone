/* *****************************************************************************
 * File:   isr.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 06 04 13:27
 * 
 * Description: ISR Driver dsPIC33
 * 
 **************************************************************************** */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <xc.h>
#include "isr.h"
#include "boardcfg.h"

/* *****************************************************************************
 * Function-Like Macros - optional use MCLIB module
 **************************************************************************** */
#ifndef MCLIB_H

typedef void (*pfVoid_t)(void); 

/* log2(16)=4 log2(15)=4 */
#define LOG2_MATCH(nInput) ((nInput-1)*2)     
#define MCLIB_u16LOG2(nInput) (uint16_t)\
    ((0xff00 & LOG2_MATCH(nInput))\
        ? (0xf000 & LOG2_MATCH(nInput))\
            ? (0xc000 & LOG2_MATCH(nInput))\
                ? (0x8000 & LOG2_MATCH(nInput))\
                    ? 15U\
                    : 14U\
                : (0x2000 & LOG2_MATCH(nInput))\
                    ? 13U\
                    : 12U\
            : (0x0c00 & LOG2_MATCH(nInput))\
                ? (0x0800 & LOG2_MATCH(nInput))\
                    ? 11U\
                    : 10U\
                : (0x0200 & LOG2_MATCH(nInput))\
                    ? 9U\
                    : 8U\
        : (0x00f0 & LOG2_MATCH(nInput))\
            ? (0x00c0 & LOG2_MATCH(nInput))\
                ? (0x0080 & LOG2_MATCH(nInput))\
                    ? 7U\
                    : 6U\
                : (0x0020 & LOG2_MATCH(nInput))\
                    ? 5U\
                    : 4U\
            : (0x000c & LOG2_MATCH(nInput))\
                ? (0x0008 & LOG2_MATCH(nInput))\
                    ? 3U\
                    : 2U\
                : (0x0002 & LOG2_MATCH(nInput))\
                    ? 1U\
                    : 0U)
#endif


/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define IPC_REGISTER_POINTER_BITS       16
#define IPC_REGISTER_POINTER_SHIFT_BITS MCLIB_u16LOG2(IPC_REGISTER_POINTER_BITS)   /* 2^4 bits register pointer size (see IPC_REGISTER_POINTER_BITS) */
#define IPC_FIELD_BITS                  3
#define IPC_FIELD_OFFSET_BITS           4
#define IPC_FIELD_OFFSET_SHIFT_BITS     MCLIB_u16LOG2(IPC_FIELD_OFFSET_BITS)       /* 2^2 bits fiels offset (shift is power of 2) */




/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */

ISR_sHandles_t ISR_sHandles = 
{
    (uint16_t*)&IFS0,
    (uint16_t*)&IEC0,
    (uint16_t*)&IPC0,
};



pfVoid_t pfISRDMA0 = (pfVoid_t)NULL;
pfVoid_t pfISRDMA1 = (pfVoid_t)NULL;
pfVoid_t pfISRDMA2 = (pfVoid_t)NULL;


/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions definitions
 **************************************************************************** */


void ISR_vInit(void)
{
    __builtin_disable_interrupts();
    /* Turn saturation on to insure that overflows will be handled smoothly. */
    CORCONbits.SATA  = 0;    
}


void ISR_vSetISRDMA (pfVoid_t pfVoid, DMA_eModule_t eDMAModule)
{
    switch (eDMAModule)
    {
        case DMA_0:
            pfISRDMA0 = pfVoid;
            break;
        case DMA_1:
            pfISRDMA1 = pfVoid;
            break;
        case DMA_2:
            pfISRDMA2 = pfVoid;
            break;
        default:
            break;    
    }    
}



void ISR_vSetBitIFS (uint8_t bitOffset)
{
    uint16_t* pRegister = (uint16_t*)&IFS0;
    
    pRegister += (bitOffset >> 4);
    *pRegister |= (1 << (bitOffset & 0xF));
}

void ISR_vSetBitIEC (uint8_t bitOffset)
{
    uint16_t* pRegister = (uint16_t*)&IEC0;
    
    pRegister += (bitOffset >> 4);
    *pRegister |= (1 << (bitOffset & 0xF));
    
}

void ISR_vClrBitIFS (uint8_t bitOffset)
{
    uint16_t* pRegister = (uint16_t*)&IFS0;
    
    pRegister += (bitOffset >> 4);
    *pRegister &= ~(1 << (bitOffset & 0xF));
}

void ISR_vClrBitIEC (uint8_t bitOffset)
{
    uint16_t* pRegister = (uint16_t*)&IEC0;
    
    pRegister += (bitOffset >> 4);
    *pRegister &= ~(1 << (bitOffset & 0xF));
}



void ISR_vSetDatIPC (uint8_t bitOffset, uint8_t data)
{
    uint16_t* pRegister = (uint16_t*)&IPC0;
    uint16_t u16BitOffset = (uint16_t)bitOffset << IPC_FIELD_OFFSET_SHIFT_BITS;/* IPC field offset 4 Bits */
    uint16_t u16RegisterData;
    uint16_t u16DataMask;
    
    pRegister += (u16BitOffset >> IPC_REGISTER_POINTER_SHIFT_BITS);
    u16RegisterData = *pRegister;
    u16BitOffset &= 0x0F;
    u16DataMask = (( data & ((1 << IPC_FIELD_BITS)-1) ) << u16BitOffset);
    u16RegisterData |= u16DataMask;
    u16DataMask |= ((1 << (IPC_REGISTER_POINTER_BITS-(IPC_FIELD_BITS+u16BitOffset)))-1) << (IPC_FIELD_BITS+u16BitOffset);    /* fill left unchanged bits */
    u16DataMask |= ((1 << (u16BitOffset))-1);    /* fill right unchanged bits */
    u16RegisterData &= u16DataMask;
    *pRegister = u16RegisterData;
}

bool ISR_vChkBitIFS (uint8_t bitOffset)
{
    bool bResult = false;
    uint16_t* pRegister = (uint16_t*)&IFS0;
    
    pRegister += (bitOffset >> 4);
    if ((*pRegister & (1 << (bitOffset & 0xF))) > 0)
    {
        bResult = true;
    }
    return bResult;
}

bool ISR_vChkBitIEC (uint8_t bitOffset)
{
    bool bResult = false;
    uint16_t* pRegister = (uint16_t*)&IEC0;
    
    pRegister += (bitOffset >> 4);
    if ((*pRegister & (1 << (bitOffset & 0xF))) > 0)
    {
        bResult = true;
    }
    return bResult;
}



#if DMA_USE_LIBRARY_ISR_DMA == 0
/*=============================================================================
Interrupt Service Routines.
=============================================================================*/
/******************************************************************************
 * Function:        void  __attribute__((interrupt, auto_psv)) _DMA0Interrupt(void)
 *
 * Overview:        DMA0 Interrupt
 *****************************************************************************/
void __attribute__ ( (interrupt, auto_psv) ) _DMA0Interrupt( void )
{
    IFS0bits.DMA0IF = 0;    // Clear the DMA0 Interrupt Flag;
    if (pfISRDMA0 != NULL)
    {
        pfISRDMA0();
    }
}


/******************************************************************************
 * Function:        void __attribute__((interrupt, auto_psv)) _DMA1Interrupt(void)
 *
 * Overview:        DMA1 Interrupt
 *****************************************************************************/
void __attribute__ ( (interrupt, auto_psv) ) _DMA1Interrupt( void )
{
    IFS0bits.DMA1IF = 0;    // Clear the DMA1 Interrupt Flag 
    if (pfISRDMA1 != NULL)
    {
        pfISRDMA1();
    }
}


/******************************************************************************
 * Function:        void __attribute__((interrupt, auto_psv)) _DMA2Interrupt(void)
 *
 * Overview:        DMA2 Interrupt
 *****************************************************************************/
void __attribute__ ( (interrupt, auto_psv) ) _DMA2Interrupt( void )
{
    IFS1bits.DMA2IF = 0;    // Clear the DMA1 Interrupt Flag 
    if (pfISRDMA2 != NULL)
    {
        pfISRDMA2();
    }
}


#endif
