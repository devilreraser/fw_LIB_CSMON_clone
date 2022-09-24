/* *****************************************************************************
 * File:   config.c
 * Author: Dimitar Lilov
 *
 * Created on 2020 02 21 14:10
 * 
 * Description: ...
 * 
 **************************************************************************** */
#ifdef _WIN32
#pragma once
#endif /* _WIN32 */

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "config.h"
#include "boardcfg.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#ifndef BOARDCFG_USE_MCC_CONFIG
// FGS
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#if defined __dsPIC33EP512MU814__
#pragma config GSS = OFF                // General Segment Code-Protect bit (General Segment Code protect is disabled)
#pragma config GSSK = OFF               // General Segment Key bits (General Segment Write Protection and Code Protection is Disabled)
#else
#pragma config GCP = OFF    //General Segment Code-Protect bit->General Segment Code protect is Disabled
#endif

// FOSCSEL
#pragma config FNOSC = FRC              // Oscillator Source Selection bits (Internal Fast RC (FRC)FRC PRIPLL)
#pragma config IESO = ON                // Two-speed Oscillator Start-up Enable bit (Start up device with FRC, then switch to user-selected oscillator source)
#if defined __dsPIC33EP512MU814__
#else
#pragma config PWMLOCK = ON    //PWM Lock Enable bit->Certain PWM registers may only be written after key sequence
#endif

// FOSC
#if defined __dsPIC33EP512GM710__
#pragma config POSCMD = EC    //Primary Oscillator Mode Select Bit->EC External Clock
#pragma config OSCIOFNC = ON            // OSC2 Pin Function Bit (OSC2 is general purpose digital I/O pin)
#else
#pragma config POSCMD = XT    //Primary Oscillator Mode Select Bit->XT Crystal Oscillator Mode
#pragma config OSCIOFNC = OFF            // OSC2 Pin Function Bit (OSC2 is not general purpose digital I/O pin)
#endif
#pragma config IOL1WAY = OFF            // Peripheral pin select configuration (Allow multiple reconfigurations)
#pragma config FCKSM = CSECME           // Clock Switching Mode bits (Both Clock switching and Fail-safe Clock Monitor are enabled)

// FWDT
#pragma config WDTPOST = PS1024    //Watchdog Timer Postscaler bits->1:1024
#pragma config WDTPRE = PR32    //Watchdog Timer Prescaler bit->1:32
#pragma config PLLKEN = ON              // PLL Lock Wait Enable bit (Clock switch to PLL source will wait until the PLL lock signal is valid.)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (Watchdog timer enabled/disabled by user software)

// FPOR
#pragma config BOREN = ON    //Brown-out Reset (BOR) Detection Enable bit->BOR is enabled
#pragma config ALTI2C2 = OFF            // Alternate I2C pins for I2C2 (I2C2 mapped to SDA2/SCL2 pins)
#if defined __dsPIC33EP512MU814__
#pragma config ALTI2C1 = ON            // Alternate I2C pins for I2C1 (ASDA1/ASCK1 pins are selected as the I/O pins for I2C1)
#pragma config FPWRT = PWR128           // Power-on Reset Timer Value Select bits (128ms)
#else
#pragma config ALTI2C1 = OFF           // Alternate I2C pins for I2C1 (SDA1/SCK1 pins are selected as the I/O pins for I2C1)
#endif

// FICD
#if BOARDCFG_DEBUG == ICD_PGD1
#pragma config ICS = PGD1               // ICD Communication Channel Select bits (Communicate on PGEC1 and PGED1)
#elif BOARDCFG_DEBUG == ICD_PGD2
#pragma config ICS = PGD2               // ICD Communication Channel Select bits (Communicate on PGEC2 and PGED2)
#elif BOARDCFG_DEBUG == ICD_PGD3
#pragma config ICS = PGD3               // ICD Communication Channel Select bits (Communicate on PGEC3 and PGED3)
#endif
#if defined __dsPIC33EP512MU814__
#pragma config RSTPRI = PF              // Reset Target Vector Select bit (Device will obtain reset instruction from Primary flash)
#endif
#pragma config JTAGEN = OFF             // JTAG Enable Bit (JTAG is disabled)

// FAS
#if defined __dsPIC33EP512MU814__
#pragma config AWRP = OFF               // Auxiliary Segment Write-protect bit (Aux Flash may be written)
#pragma config APL = OFF                // Auxiliary Segment Code-protect bit (Aux Flash Code protect is disabled)
#pragma config APLK = OFF               // Auxiliary Segment Key bits (Aux Flash Write Protection and Code Protection is Disabled)
#endif

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
#endif


/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */
