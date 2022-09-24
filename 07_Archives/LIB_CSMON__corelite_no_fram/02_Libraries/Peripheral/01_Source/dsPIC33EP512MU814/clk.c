/* *****************************************************************************
 * File:   clk.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 04 04 16:12
 * 
 * Description: System Clock Initialization
 * 
 **************************************************************************** */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <xc.h>
#include <stdbool.h>
#include "boardcfg.h"
#include "clk.h"


/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define USE_CLOCK_CHANGE 0

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
#if USE_CLOCK_CHANGE
uint32_t CLK_u32Fcy = FCY_HZ;
uint32_t u32FcyOld;
#else
#define CLK_u32Fcy  FCY_HZ
#define u32FcyOld   FCY_HZ
#endif

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */


/* *****************************************************************************
 * Functions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */
void CLK_vInit(void)
{
	// Configure Oscillator to operate the device at 120Mhz
	// Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
	// Fosc= 8*60/(2*2)= 120Mhz for 8M input clock

    /* input 8 MHz -> Fref= 4MHz -> N1 = input/Fref */
	CLKDIVbits.PLLPRE = (uint16_t)(OSC_HZ / 4000000.0) - 2;	// N1=2 = (PLLPRE+2)
    
    /* 60 * 4 -> 240MHz (120-340MHz)*/
	PLLFBD = (uint16_t)(CLK_u32Fcy/1000000) - 2;                  // M=60 = (PLLFBD.PLLDIV+2)
    
    /* 240MHz/2 -> 120MHz */
	CLKDIVbits.PLLPOST = 1-1;		// N2=2 = 2*(PLLPOST+1))
    
	CLKDIVbits.DOZE = 0;
	CLKDIVbits.DOZEN = 0;
	
	__builtin_write_OSCCONH(0x03);
	//__builtin_write_OSCCONH(0x02);
	__builtin_write_OSCCONL(0x01);

    // Wait for oscillator switch
	while(OSCCONbits.COSC != 0b011);
	//while(OSCCONbits.COSC != 0b010);
	// Wait for PLL to lock
	while(OSCCONbits.LOCK != 1);
    
    
    
	__builtin_write_OSCCONL(OSCCON & (~(1<<6))); // clear bit 6 
    
    
    #if defined __dsPIC33EP512MU814__
    RPOR12bits.RP109R = 0b110001;    //RP109/RF13 as REFCLK  /  FPCLK on TSC Board
    RPOR3bits.RP70R   = 0b110001;    //RP70 /RD6  as REFCLK  / FPCLKA on TSC Board
    #elif defined __dsPIC33EP512GM710__

    #elif defined __dsPIC33EP128GM706__

    #else

    #endif
    
    
	__builtin_write_OSCCONL(OSCCON | (1<<6)); 	 // Set bit 6 	
    
    REFOCONbits.RODIV = 2;  /* 1/4 (1/2^2) Frequency On the Reference Oscillator Output */
    REFOCONbits.ROON = 1;  /* Enable Reference Oscillator Output */
    
    #if USE_CLOCK_CHANGE
    u32FcyOld = CLK_u32Fcy;
    #endif
}

uint32_t CLK_u32GetFcy (void)
{
    return CLK_u32Fcy;
}

uint32_t CLK_u32GetFpb (void)
{
    return CLK_u32Fcy;
}

bool CLK_bClockChangeRequest(void)
{
    return (CLK_u32Fcy != u32FcyOld);
}