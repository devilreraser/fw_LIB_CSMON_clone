/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for .
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.7
        Device            :  PIC16F18446
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.31 and above
        MPLAB 	          :  MPLAB X 5.45	
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

/**
  Section: Included Files
*/

#include <xc.h>

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set SENSE_1V1 aliases
#define SENSE_1V1_TRIS                 TRISAbits.TRISA2
#define SENSE_1V1_LAT                  LATAbits.LATA2
#define SENSE_1V1_PORT                 PORTAbits.RA2
#define SENSE_1V1_WPU                  WPUAbits.WPUA2
#define SENSE_1V1_OD                   ODCONAbits.ODCA2
#define SENSE_1V1_ANS                  ANSELAbits.ANSA2
#define SENSE_1V1_SetHigh()            do { LATAbits.LATA2 = 1; } while(0)
#define SENSE_1V1_SetLow()             do { LATAbits.LATA2 = 0; } while(0)
#define SENSE_1V1_Toggle()             do { LATAbits.LATA2 = ~LATAbits.LATA2; } while(0)
#define SENSE_1V1_GetValue()           PORTAbits.RA2
#define SENSE_1V1_SetDigitalInput()    do { TRISAbits.TRISA2 = 1; } while(0)
#define SENSE_1V1_SetDigitalOutput()   do { TRISAbits.TRISA2 = 0; } while(0)
#define SENSE_1V1_SetPullup()          do { WPUAbits.WPUA2 = 1; } while(0)
#define SENSE_1V1_ResetPullup()        do { WPUAbits.WPUA2 = 0; } while(0)
#define SENSE_1V1_SetPushPull()        do { ODCONAbits.ODCA2 = 0; } while(0)
#define SENSE_1V1_SetOpenDrain()       do { ODCONAbits.ODCA2 = 1; } while(0)
#define SENSE_1V1_SetAnalogMode()      do { ANSELAbits.ANSA2 = 1; } while(0)
#define SENSE_1V1_SetDigitalMode()     do { ANSELAbits.ANSA2 = 0; } while(0)

// get/set RESET_RTC aliases
#define RESET_RTC_TRIS                 TRISAbits.TRISA4
#define RESET_RTC_LAT                  LATAbits.LATA4
#define RESET_RTC_PORT                 PORTAbits.RA4
#define RESET_RTC_WPU                  WPUAbits.WPUA4
#define RESET_RTC_OD                   ODCONAbits.ODCA4
#define RESET_RTC_ANS                  ANSELAbits.ANSA4
#define RESET_RTC_SetHigh()            do { LATAbits.LATA4 = 1; } while(0)
#define RESET_RTC_SetLow()             do { LATAbits.LATA4 = 0; } while(0)
#define RESET_RTC_Toggle()             do { LATAbits.LATA4 = ~LATAbits.LATA4; } while(0)
#define RESET_RTC_GetValue()           PORTAbits.RA4
#define RESET_RTC_SetDigitalInput()    do { TRISAbits.TRISA4 = 1; } while(0)
#define RESET_RTC_SetDigitalOutput()   do { TRISAbits.TRISA4 = 0; } while(0)
#define RESET_RTC_SetPullup()          do { WPUAbits.WPUA4 = 1; } while(0)
#define RESET_RTC_ResetPullup()        do { WPUAbits.WPUA4 = 0; } while(0)
#define RESET_RTC_SetPushPull()        do { ODCONAbits.ODCA4 = 0; } while(0)
#define RESET_RTC_SetOpenDrain()       do { ODCONAbits.ODCA4 = 1; } while(0)
#define RESET_RTC_SetAnalogMode()      do { ANSELAbits.ANSA4 = 1; } while(0)
#define RESET_RTC_SetDigitalMode()     do { ANSELAbits.ANSA4 = 0; } while(0)

// get/set RB5 procedures
#define RB5_SetHigh()            do { LATBbits.LATB5 = 1; } while(0)
#define RB5_SetLow()             do { LATBbits.LATB5 = 0; } while(0)
#define RB5_Toggle()             do { LATBbits.LATB5 = ~LATBbits.LATB5; } while(0)
#define RB5_GetValue()              PORTBbits.RB5
#define RB5_SetDigitalInput()    do { TRISBbits.TRISB5 = 1; } while(0)
#define RB5_SetDigitalOutput()   do { TRISBbits.TRISB5 = 0; } while(0)
#define RB5_SetPullup()             do { WPUBbits.WPUB5 = 1; } while(0)
#define RB5_ResetPullup()           do { WPUBbits.WPUB5 = 0; } while(0)
#define RB5_SetAnalogMode()         do { ANSELBbits.ANSB5 = 1; } while(0)
#define RB5_SetDigitalMode()        do { ANSELBbits.ANSB5 = 0; } while(0)

// get/set SENSE_0V9 aliases
#define SENSE_0V9_TRIS                 TRISBbits.TRISB6
#define SENSE_0V9_LAT                  LATBbits.LATB6
#define SENSE_0V9_PORT                 PORTBbits.RB6
#define SENSE_0V9_WPU                  WPUBbits.WPUB6
#define SENSE_0V9_OD                   ODCONBbits.ODCB6
#define SENSE_0V9_ANS                  ANSELBbits.ANSB6
#define SENSE_0V9_SetHigh()            do { LATBbits.LATB6 = 1; } while(0)
#define SENSE_0V9_SetLow()             do { LATBbits.LATB6 = 0; } while(0)
#define SENSE_0V9_Toggle()             do { LATBbits.LATB6 = ~LATBbits.LATB6; } while(0)
#define SENSE_0V9_GetValue()           PORTBbits.RB6
#define SENSE_0V9_SetDigitalInput()    do { TRISBbits.TRISB6 = 1; } while(0)
#define SENSE_0V9_SetDigitalOutput()   do { TRISBbits.TRISB6 = 0; } while(0)
#define SENSE_0V9_SetPullup()          do { WPUBbits.WPUB6 = 1; } while(0)
#define SENSE_0V9_ResetPullup()        do { WPUBbits.WPUB6 = 0; } while(0)
#define SENSE_0V9_SetPushPull()        do { ODCONBbits.ODCB6 = 0; } while(0)
#define SENSE_0V9_SetOpenDrain()       do { ODCONBbits.ODCB6 = 1; } while(0)
#define SENSE_0V9_SetAnalogMode()      do { ANSELBbits.ANSB6 = 1; } while(0)
#define SENSE_0V9_SetDigitalMode()     do { ANSELBbits.ANSB6 = 0; } while(0)

// get/set RB7 procedures
#define RB7_SetHigh()            do { LATBbits.LATB7 = 1; } while(0)
#define RB7_SetLow()             do { LATBbits.LATB7 = 0; } while(0)
#define RB7_Toggle()             do { LATBbits.LATB7 = ~LATBbits.LATB7; } while(0)
#define RB7_GetValue()              PORTBbits.RB7
#define RB7_SetDigitalInput()    do { TRISBbits.TRISB7 = 1; } while(0)
#define RB7_SetDigitalOutput()   do { TRISBbits.TRISB7 = 0; } while(0)
#define RB7_SetPullup()             do { WPUBbits.WPUB7 = 1; } while(0)
#define RB7_ResetPullup()           do { WPUBbits.WPUB7 = 0; } while(0)
#define RB7_SetAnalogMode()         do { ANSELBbits.ANSB7 = 1; } while(0)
#define RB7_SetDigitalMode()        do { ANSELBbits.ANSB7 = 0; } while(0)

// get/set SENSE_1V8 aliases
#define SENSE_1V8_TRIS                 TRISCbits.TRISC0
#define SENSE_1V8_LAT                  LATCbits.LATC0
#define SENSE_1V8_PORT                 PORTCbits.RC0
#define SENSE_1V8_WPU                  WPUCbits.WPUC0
#define SENSE_1V8_OD                   ODCONCbits.ODCC0
#define SENSE_1V8_ANS                  ANSELCbits.ANSC0
#define SENSE_1V8_SetHigh()            do { LATCbits.LATC0 = 1; } while(0)
#define SENSE_1V8_SetLow()             do { LATCbits.LATC0 = 0; } while(0)
#define SENSE_1V8_Toggle()             do { LATCbits.LATC0 = ~LATCbits.LATC0; } while(0)
#define SENSE_1V8_GetValue()           PORTCbits.RC0
#define SENSE_1V8_SetDigitalInput()    do { TRISCbits.TRISC0 = 1; } while(0)
#define SENSE_1V8_SetDigitalOutput()   do { TRISCbits.TRISC0 = 0; } while(0)
#define SENSE_1V8_SetPullup()          do { WPUCbits.WPUC0 = 1; } while(0)
#define SENSE_1V8_ResetPullup()        do { WPUCbits.WPUC0 = 0; } while(0)
#define SENSE_1V8_SetPushPull()        do { ODCONCbits.ODCC0 = 0; } while(0)
#define SENSE_1V8_SetOpenDrain()       do { ODCONCbits.ODCC0 = 1; } while(0)
#define SENSE_1V8_SetAnalogMode()      do { ANSELCbits.ANSC0 = 1; } while(0)
#define SENSE_1V8_SetDigitalMode()     do { ANSELCbits.ANSC0 = 0; } while(0)

// get/set SENSE_2V5 aliases
#define SENSE_2V5_TRIS                 TRISCbits.TRISC1
#define SENSE_2V5_LAT                  LATCbits.LATC1
#define SENSE_2V5_PORT                 PORTCbits.RC1
#define SENSE_2V5_WPU                  WPUCbits.WPUC1
#define SENSE_2V5_OD                   ODCONCbits.ODCC1
#define SENSE_2V5_ANS                  ANSELCbits.ANSC1
#define SENSE_2V5_SetHigh()            do { LATCbits.LATC1 = 1; } while(0)
#define SENSE_2V5_SetLow()             do { LATCbits.LATC1 = 0; } while(0)
#define SENSE_2V5_Toggle()             do { LATCbits.LATC1 = ~LATCbits.LATC1; } while(0)
#define SENSE_2V5_GetValue()           PORTCbits.RC1
#define SENSE_2V5_SetDigitalInput()    do { TRISCbits.TRISC1 = 1; } while(0)
#define SENSE_2V5_SetDigitalOutput()   do { TRISCbits.TRISC1 = 0; } while(0)
#define SENSE_2V5_SetPullup()          do { WPUCbits.WPUC1 = 1; } while(0)
#define SENSE_2V5_ResetPullup()        do { WPUCbits.WPUC1 = 0; } while(0)
#define SENSE_2V5_SetPushPull()        do { ODCONCbits.ODCC1 = 0; } while(0)
#define SENSE_2V5_SetOpenDrain()       do { ODCONCbits.ODCC1 = 1; } while(0)
#define SENSE_2V5_SetAnalogMode()      do { ANSELCbits.ANSC1 = 1; } while(0)
#define SENSE_2V5_SetDigitalMode()     do { ANSELCbits.ANSC1 = 0; } while(0)

// get/set SENSE_3V3 aliases
#define SENSE_3V3_TRIS                 TRISCbits.TRISC2
#define SENSE_3V3_LAT                  LATCbits.LATC2
#define SENSE_3V3_PORT                 PORTCbits.RC2
#define SENSE_3V3_WPU                  WPUCbits.WPUC2
#define SENSE_3V3_OD                   ODCONCbits.ODCC2
#define SENSE_3V3_ANS                  ANSELCbits.ANSC2
#define SENSE_3V3_SetHigh()            do { LATCbits.LATC2 = 1; } while(0)
#define SENSE_3V3_SetLow()             do { LATCbits.LATC2 = 0; } while(0)
#define SENSE_3V3_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)
#define SENSE_3V3_GetValue()           PORTCbits.RC2
#define SENSE_3V3_SetDigitalInput()    do { TRISCbits.TRISC2 = 1; } while(0)
#define SENSE_3V3_SetDigitalOutput()   do { TRISCbits.TRISC2 = 0; } while(0)
#define SENSE_3V3_SetPullup()          do { WPUCbits.WPUC2 = 1; } while(0)
#define SENSE_3V3_ResetPullup()        do { WPUCbits.WPUC2 = 0; } while(0)
#define SENSE_3V3_SetPushPull()        do { ODCONCbits.ODCC2 = 0; } while(0)
#define SENSE_3V3_SetOpenDrain()       do { ODCONCbits.ODCC2 = 1; } while(0)
#define SENSE_3V3_SetAnalogMode()      do { ANSELCbits.ANSC2 = 1; } while(0)
#define SENSE_3V3_SetDigitalMode()     do { ANSELCbits.ANSC2 = 0; } while(0)

// get/set SENSE_1V2 aliases
#define SENSE_1V2_TRIS                 TRISCbits.TRISC5
#define SENSE_1V2_LAT                  LATCbits.LATC5
#define SENSE_1V2_PORT                 PORTCbits.RC5
#define SENSE_1V2_WPU                  WPUCbits.WPUC5
#define SENSE_1V2_OD                   ODCONCbits.ODCC5
#define SENSE_1V2_ANS                  ANSELCbits.ANSC5
#define SENSE_1V2_SetHigh()            do { LATCbits.LATC5 = 1; } while(0)
#define SENSE_1V2_SetLow()             do { LATCbits.LATC5 = 0; } while(0)
#define SENSE_1V2_Toggle()             do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define SENSE_1V2_GetValue()           PORTCbits.RC5
#define SENSE_1V2_SetDigitalInput()    do { TRISCbits.TRISC5 = 1; } while(0)
#define SENSE_1V2_SetDigitalOutput()   do { TRISCbits.TRISC5 = 0; } while(0)
#define SENSE_1V2_SetPullup()          do { WPUCbits.WPUC5 = 1; } while(0)
#define SENSE_1V2_ResetPullup()        do { WPUCbits.WPUC5 = 0; } while(0)
#define SENSE_1V2_SetPushPull()        do { ODCONCbits.ODCC5 = 0; } while(0)
#define SENSE_1V2_SetOpenDrain()       do { ODCONCbits.ODCC5 = 1; } while(0)
#define SENSE_1V2_SetAnalogMode()      do { ANSELCbits.ANSC5 = 1; } while(0)
#define SENSE_1V2_SetDigitalMode()     do { ANSELCbits.ANSC5 = 0; } while(0)

// get/set RC6 procedures
#define RC6_SetHigh()            do { LATCbits.LATC6 = 1; } while(0)
#define RC6_SetLow()             do { LATCbits.LATC6 = 0; } while(0)
#define RC6_Toggle()             do { LATCbits.LATC6 = ~LATCbits.LATC6; } while(0)
#define RC6_GetValue()              PORTCbits.RC6
#define RC6_SetDigitalInput()    do { TRISCbits.TRISC6 = 1; } while(0)
#define RC6_SetDigitalOutput()   do { TRISCbits.TRISC6 = 0; } while(0)
#define RC6_SetPullup()             do { WPUCbits.WPUC6 = 1; } while(0)
#define RC6_ResetPullup()           do { WPUCbits.WPUC6 = 0; } while(0)
#define RC6_SetAnalogMode()         do { ANSELCbits.ANSC6 = 1; } while(0)
#define RC6_SetDigitalMode()        do { ANSELCbits.ANSC6 = 0; } while(0)

// get/set RC7 procedures
#define RC7_SetHigh()            do { LATCbits.LATC7 = 1; } while(0)
#define RC7_SetLow()             do { LATCbits.LATC7 = 0; } while(0)
#define RC7_Toggle()             do { LATCbits.LATC7 = ~LATCbits.LATC7; } while(0)
#define RC7_GetValue()              PORTCbits.RC7
#define RC7_SetDigitalInput()    do { TRISCbits.TRISC7 = 1; } while(0)
#define RC7_SetDigitalOutput()   do { TRISCbits.TRISC7 = 0; } while(0)
#define RC7_SetPullup()             do { WPUCbits.WPUC7 = 1; } while(0)
#define RC7_ResetPullup()           do { WPUCbits.WPUC7 = 0; } while(0)
#define RC7_SetAnalogMode()         do { ANSELCbits.ANSC7 = 1; } while(0)
#define RC7_SetDigitalMode()        do { ANSELCbits.ANSC7 = 0; } while(0)

/**
   @Param
    none
   @Returns
    none
   @Description
    GPIO and peripheral I/O initialization
   @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize (void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handling routine
 * @Example
    PIN_MANAGER_IOC();
 */
void PIN_MANAGER_IOC(void);



#endif // PIN_MANAGER_H
/**
 End of File
*/