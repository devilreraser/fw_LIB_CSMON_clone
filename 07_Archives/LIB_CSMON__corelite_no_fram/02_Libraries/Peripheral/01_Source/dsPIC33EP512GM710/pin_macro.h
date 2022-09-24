/* *****************************************************************************
 * File:   pin_macro.h
 * Author: Dimitar Lilov
 *
 * Created on 2020 04 15 11:55
 * 
 * Description: Pin Macro Generator
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef PIN_MACRO_H
#define	PIN_MACRO_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "pin_cfg.h"
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Constants Definitions
 **************************************************************************** */


/* *****************************************************************************
 * DO Definitions - Outputs
 **************************************************************************** */
#ifndef DO_EN_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef DO_EN

#define DO_EN_SetDigitalOutput() PIN_vSetDigitalOutput(DO_EN)
#define DO_EN_SetHigh()   PIN_vSetHigh(DO_EN)
#define DO_EN_SetLow()    PIN_vSetLow(DO_EN)
#define DO_EN_GetValue()  PIN_vGetValue(DO_EN)

#ifdef DO_EN_START_HIGH
#define DO_EN_SetStart()  DO_EN_SetHigh()
#else
#define DO_EN_SetStart()  
#endif

#else

#define DO_EN_SetDigitalOutput() 
#define DO_EN_SetHigh()
#define DO_EN_SetLow()
#define DO_EN_GetValue() 0
#define DO_EN_SetStart()
#endif
#else
#define DO_EN_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef DO1_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef DO1

#define DO1_SetDigitalOutput() PIN_vSetDigitalOutput(DO1)
#define DO1_SetHigh()   PIN_vSetHigh(DO1)
#define DO1_SetLow()    PIN_vSetLow(DO1)
#define DO1_GetValue()  PIN_vGetValue(DO1)

#ifdef DO1_START_HIGH
#define DO1_SetStart()  DO1_SetHigh()
#else
#define DO1_SetStart()  
#endif

#else

#define DO1_SetDigitalOutput() 
#define DO1_SetHigh()
#define DO1_SetLow()
#define DO1_GetValue() 0
#define DO1_SetStart()
#endif
#else
#define DO1_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef DO2_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef DO2

#define DO2_SetDigitalOutput() PIN_vSetDigitalOutput(DO2)
#define DO2_SetHigh()   PIN_vSetHigh(DO2)
#define DO2_SetLow()    PIN_vSetLow(DO2)
#define DO2_GetValue()  PIN_vGetValue(DO2)

#ifdef DO2_START_HIGH
#define DO2_SetStart()  DO2_SetHigh()
#else
#define DO2_SetStart()  
#endif

#else

#define DO2_SetDigitalOutput() 
#define DO2_SetHigh()
#define DO2_SetLow()
#define DO2_GetValue() 0
#define DO2_SetStart()
#endif
#else
#define DO2_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef DO3_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef DO3

#define DO3_SetDigitalOutput() PIN_vSetDigitalOutput(DO3)
#define DO3_SetHigh()   PIN_vSetHigh(DO3)
#define DO3_SetLow()    PIN_vSetLow(DO3)
#define DO3_GetValue()  PIN_vGetValue(DO3)

#ifdef DO3_START_HIGH
#define DO3_SetStart()  DO3_SetHigh()
#else
#define DO3_SetStart()  
#endif

#else

#define DO3_SetDigitalOutput() 
#define DO3_SetHigh()
#define DO3_SetLow()
#define DO3_GetValue() 0
#define DO3_SetStart()
#endif
#else
#define DO3_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef DO4_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef DO4

#define DO4_SetDigitalOutput() PIN_vSetDigitalOutput(DO4)
#define DO4_SetHigh()   PIN_vSetHigh(DO4)
#define DO4_SetLow()    PIN_vSetLow(DO4)
#define DO4_GetValue()  PIN_vGetValue(DO4)

#ifdef DO4_START_HIGH
#define DO4_SetStart()  DO4_SetHigh()
#else
#define DO4_SetStart()  
#endif

#else

#define DO4_SetDigitalOutput() 
#define DO4_SetHigh()
#define DO4_SetLow()
#define DO4_GetValue() 0
#define DO4_SetStart()
#endif
#else
#define DO4_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef DO5_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef DO5

#define DO5_SetDigitalOutput() PIN_vSetDigitalOutput(DO5)
#define DO5_SetHigh()   PIN_vSetHigh(DO5)
#define DO5_SetLow()    PIN_vSetLow(DO5)
#define DO5_GetValue()  PIN_vGetValue(DO5)

#ifdef DO5_START_HIGH
#define DO5_SetStart()  DO5_SetHigh()
#else
#define DO5_SetStart()  
#endif

#else

#define DO5_SetDigitalOutput() 
#define DO5_SetHigh()
#define DO5_SetLow()
#define DO5_GetValue() 0
#define DO5_SetStart()
#endif
#else
#define DO5_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef DO6_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef DO6

#define DO6_SetDigitalOutput() PIN_vSetDigitalOutput(DO6)
#define DO6_SetHigh()   PIN_vSetHigh(DO6)
#define DO6_SetLow()    PIN_vSetLow(DO6)
#define DO6_GetValue()  PIN_vGetValue(DO6)

#ifdef DO6_START_HIGH
#define DO6_SetStart()  DO6_SetHigh()
#else
#define DO6_SetStart()  
#endif

#else

#define DO6_SetDigitalOutput() 
#define DO6_SetHigh()
#define DO6_SetLow()
#define DO6_GetValue() 0
#define DO6_SetStart()
#endif
#else
#define DO6_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef DO7_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef DO7

#define DO7_SetDigitalOutput() PIN_vSetDigitalOutput(DO7)
#define DO7_SetHigh()   PIN_vSetHigh(DO7)
#define DO7_SetLow()    PIN_vSetLow(DO7)
#define DO7_GetValue()  PIN_vGetValue(DO7)

#ifdef DO7_START_HIGH
#define DO7_SetStart()  DO7_SetHigh()
#else
#define DO7_SetStart()  
#endif

#else

#define DO7_SetDigitalOutput() 
#define DO7_SetHigh()
#define DO7_SetLow()
#define DO7_GetValue() 0
#define DO7_SetStart()
#endif
#else
#define DO7_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef DO8_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef DO8

#define DO8_SetDigitalOutput() PIN_vSetDigitalOutput(DO8)
#define DO8_SetHigh()   PIN_vSetHigh(DO8)
#define DO8_SetLow()    PIN_vSetLow(DO8)
#define DO8_GetValue()  PIN_vGetValue(DO8)

#ifdef DO8_START_HIGH
#define DO8_SetStart()  DO8_SetHigh()
#else
#define DO8_SetStart()  
#endif

#else

#define DO8_SetDigitalOutput() 
#define DO8_SetHigh()
#define DO8_SetLow()
#define DO8_GetValue() 0
#define DO8_SetStart()
#endif
#else
#define DO8_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



/* *****************************************************************************
 * EN Definitions - Outputs
 **************************************************************************** */
#ifndef EN1_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef EN1

#define EN1_SetDigitalOutput() PIN_vSetDigitalOutput(EN1)
#define EN1_SetHigh()   PIN_vSetHigh(EN1)
#define EN1_SetLow()    PIN_vSetLow(EN1)
#define EN1_GetValue()  PIN_vGetValue(EN1)

#ifdef EN1_START_HIGH
#define EN1_SetStart()  EN1_SetHigh()
#else
#define EN1_SetStart()  
#endif

#else

#define EN1_SetDigitalOutput() 
#define EN1_SetHigh()
#define EN1_SetLow()
#define EN1_GetValue() 0
#define EN1_SetStart()
#endif
#else
#define EN1_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef EN2_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef EN2

#define EN2_SetDigitalOutput() PIN_vSetDigitalOutput(EN2)
#define EN2_SetHigh()   PIN_vSetHigh(EN2)
#define EN2_SetLow()    PIN_vSetLow(EN2)
#define EN2_GetValue()  PIN_vGetValue(EN2)

#ifdef EN2_START_HIGH
#define EN2_SetStart()  EN2_SetHigh()
#else
#define EN2_SetStart()  
#endif

#else

#define EN2_SetDigitalOutput() 
#define EN2_SetHigh()
#define EN2_SetLow()
#define EN2_GetValue() 0
#define EN2_SetStart()
#endif
#else
#define EN2_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef EN3_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef EN3

#define EN3_SetDigitalOutput() PIN_vSetDigitalOutput(EN3)
#define EN3_SetHigh()   PIN_vSetHigh(EN3)
#define EN3_SetLow()    PIN_vSetLow(EN3)
#define EN3_GetValue()  PIN_vGetValue(EN3)

#ifdef EN3_START_HIGH
#define EN3_SetStart()  EN3_SetHigh()
#else
#define EN3_SetStart()  
#endif

#else

#define EN3_SetDigitalOutput() 
#define EN3_SetHigh()
#define EN3_SetLow()
#define EN3_GetValue() 0
#define EN3_SetStart()
#endif
#else
#define EN3_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef EN4_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef EN4

#define EN4_SetDigitalOutput() PIN_vSetDigitalOutput(EN4)
#define EN4_SetHigh()   PIN_vSetHigh(EN4)
#define EN4_SetLow()    PIN_vSetLow(EN4)
#define EN4_GetValue()  PIN_vGetValue(EN4)

#ifdef EN4_START_HIGH
#define EN4_SetStart()  EN4_SetHigh()
#else
#define EN4_SetStart()  
#endif

#else

#define EN4_SetDigitalOutput() 
#define EN4_SetHigh()
#define EN4_SetLow()
#define EN4_GetValue() 0
#define EN4_SetStart()
#endif
#else
#define EN4_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef EN5_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef EN5

#define EN5_SetDigitalOutput() PIN_vSetDigitalOutput(EN5)
#define EN5_SetHigh()   PIN_vSetHigh(EN5)
#define EN5_SetLow()    PIN_vSetLow(EN5)
#define EN5_GetValue()  PIN_vGetValue(EN5)

#ifdef EN5_START_HIGH
#define EN5_SetStart()  EN5_SetHigh()
#else
#define EN5_SetStart()  
#endif
#else

#define EN5_SetDigitalOutput() 
#define EN5_SetHigh()
#define EN5_SetLow()
#define EN5_GetValue() 0
#define EN5_SetStart()
#endif
#else
#define EN5_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef EN6_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef EN6

#define EN6_SetDigitalOutput() PIN_vSetDigitalOutput(EN6)
#define EN6_SetHigh()   PIN_vSetHigh(EN6)
#define EN6_SetLow()    PIN_vSetLow(EN6)
#define EN6_GetValue()  PIN_vGetValue(EN6)

#ifdef EN6_START_HIGH
#define EN6_SetStart()  EN6_SetHigh()
#else
#define EN6_SetStart()  
#endif

#else

#define EN6_SetDigitalOutput() 
#define EN6_SetHigh()
#define EN6_SetLow()
#define EN6_GetValue() 0
#define EN6_SetStart()
#endif
#else
#define EN6_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef EN7_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef EN7

#define EN7_SetDigitalOutput() PIN_vSetDigitalOutput(EN7)
#define EN7_SetHigh()   PIN_vSetHigh(EN7)
#define EN7_SetLow()    PIN_vSetLow(EN7)
#define EN7_GetValue()  PIN_vGetValue(EN7)

#ifdef EN7_START_HIGH
#define EN7_SetStart()  EN7_SetHigh()
#else
#define EN7_SetStart()  
#endif

#else

#define EN7_SetDigitalOutput() 
#define EN7_SetHigh()
#define EN7_SetLow()
#define EN7_GetValue() 0
#define EN7_SetStart()
#endif
#else
#define EN7_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef EN8_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef EN8

#define EN8_SetDigitalOutput() PIN_vSetDigitalOutput(EN8)
#define EN8_SetHigh()   PIN_vSetHigh(EN8)
#define EN8_SetLow()    PIN_vSetLow(EN8)
#define EN8_GetValue()  PIN_vGetValue(EN8)

#ifdef EN8_START_HIGH
#define EN8_SetStart()  EN8_SetHigh()
#else
#define EN8_SetStart()  
#endif

#else

#define EN8_SetDigitalOutput() 
#define EN8_SetHigh()
#define EN8_SetLow()
#define EN8_GetValue() 0
#define EN8_SetStart()
#endif
#else
#define EN8_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



/* *****************************************************************************
 * CS1_SPI Definitions - Outputs
 **************************************************************************** */
#ifndef CS1_SPI1_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef CS1_SPI1

#define CS1_SPI1_SetDigitalOutput() PIN_vSetDigitalOutput(CS1_SPI1)
#define CS1_SPI1_SetHigh()   PIN_vSetHigh(CS1_SPI1)
#define CS1_SPI1_SetLow()    PIN_vSetLow(CS1_SPI1)
#define CS1_SPI1_GetValue()  PIN_vGetValue(CS1_SPI1)

#ifdef CS1_SPI1_START_HIGH
#define CS1_SPI1_SetStart()  CS1_SPI1_SetHigh()
#else
#define CS1_SPI1_SetStart()  
#endif

#else
#define CS1_SPI1_NotImplemented
#define CS1_SPI1_SetDigitalOutput() 
#define CS1_SPI1_SetHigh()
#define CS1_SPI1_SetLow()
#define CS1_SPI1_GetValue() 0
#define CS1_SPI1_SetStart()
#endif
#else
#define CS1_SPI1_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef CS1_SPI2_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef CS1_SPI2

#define CS1_SPI2_SetDigitalOutput() PIN_vSetDigitalOutput(CS1_SPI2)
#define CS1_SPI2_SetHigh()   PIN_vSetHigh(CS1_SPI2)
#define CS1_SPI2_SetLow()    PIN_vSetLow(CS1_SPI2)
#define CS1_SPI2_GetValue()  PIN_vGetValue(CS1_SPI2)

#ifdef CS1_SPI2_START_HIGH
#define CS1_SPI2_SetStart()  CS1_SPI2_SetHigh()
#else
#define CS1_SPI2_SetStart()  
#endif

#else
#define CS1_SPI2_NotImplemented
#define CS1_SPI2_SetDigitalOutput() 
#define CS1_SPI2_SetHigh()
#define CS1_SPI2_SetLow()
#define CS1_SPI2_GetValue() 0
#define CS1_SPI2_SetStart()
#endif
#else
#define CS1_SPI2_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef CS1_SPI3_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef CS1_SPI3

#define CS1_SPI3_SetDigitalOutput() PIN_vSetDigitalOutput(CS1_SPI3)
#define CS1_SPI3_SetHigh()   PIN_vSetHigh(CS1_SPI3)
#define CS1_SPI3_SetLow()    PIN_vSetLow(CS1_SPI3)
#define CS1_SPI3_GetValue()  PIN_vGetValue(CS1_SPI3)

#ifdef CS1_SPI3_START_HIGH
#define CS1_SPI3_SetStart()  CS1_SPI3_SetHigh()
#else
#define CS1_SPI3_SetStart()  
#endif

#else
#define CS1_SPI3_NotImplemented
#define CS1_SPI3_SetDigitalOutput() 
#define CS1_SPI3_SetHigh()
#define CS1_SPI3_SetLow()
#define CS1_SPI3_GetValue() 0
#define CS1_SPI3_SetStart()
#endif
#else
#define CS1_SPI3_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef CS1_SPI4_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef CS1_SPI4

#define CS1_SPI4_SetDigitalOutput() PIN_vSetDigitalOutput(CS1_SPI4)
#define CS1_SPI4_SetHigh()   PIN_vSetHigh(CS1_SPI4)
#define CS1_SPI4_SetLow()    PIN_vSetLow(CS1_SPI4)
#define CS1_SPI4_GetValue()  PIN_vGetValue(CS1_SPI4)

#ifdef CS1_SPI4_START_HIGH
#define CS1_SPI4_SetStart()  CS1_SPI4_SetHigh()
#else
#define CS1_SPI4_SetStart()  
#endif

#else
#define CS1_SPI4_NotImplemented
#define CS1_SPI4_SetDigitalOutput() 
#define CS1_SPI4_SetHigh()
#define CS1_SPI4_SetLow()
#define CS1_SPI4_GetValue() 0
#define CS1_SPI4_SetStart()
#endif
#else
#define CS1_SPI4_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



/* *****************************************************************************
 * CS2_SPI Definitions - Outputs
 **************************************************************************** */
#ifndef CS2_SPI1_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef CS2_SPI1

#define CS2_SPI1_SetDigitalOutput() PIN_vSetDigitalOutput(CS2_SPI1)
#define CS2_SPI1_SetHigh()   PIN_vSetHigh(CS2_SPI1)
#define CS2_SPI1_SetLow()    PIN_vSetLow(CS2_SPI1)
#define CS2_SPI1_GetValue()  PIN_vGetValue(CS2_SPI1)

#ifdef CS2_SPI1_START_HIGH
#define CS2_SPI1_SetStart()  CS2_SPI1_SetHigh()
#else
#define CS2_SPI1_SetStart()  
#endif

#else
#define CS2_SPI1_NotImplemented
#define CS2_SPI1_SetDigitalOutput() 
#define CS2_SPI1_SetHigh()
#define CS2_SPI1_SetLow()
#define CS2_SPI1_GetValue() 0
#define CS2_SPI1_SetStart()
#endif
#else
#define CS2_SPI1_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef CS2_SPI2_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef CS2_SPI2

#define CS2_SPI2_SetDigitalOutput() PIN_vSetDigitalOutput(CS2_SPI2)
#define CS2_SPI2_SetHigh()   PIN_vSetHigh(CS2_SPI2)
#define CS2_SPI2_SetLow()    PIN_vSetLow(CS2_SPI2)
#define CS2_SPI2_GetValue()  PIN_vGetValue(CS2_SPI2)

#ifdef CS2_SPI2_START_HIGH
#define CS2_SPI2_SetStart()  CS2_SPI2_SetHigh()
#else
#define CS2_SPI2_SetStart()  
#endif

#else
#define CS2_SPI2_NotImplemented
#define CS2_SPI2_SetDigitalOutput() 
#define CS2_SPI2_SetHigh()
#define CS2_SPI2_SetLow()
#define CS2_SPI2_GetValue() 0
#define CS2_SPI2_SetStart()
#endif
#else
#define CS2_SPI2_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef CS2_SPI3_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef CS2_SPI3

#define CS2_SPI3_SetDigitalOutput() PIN_vSetDigitalOutput(CS2_SPI3)
#define CS2_SPI3_SetHigh()   PIN_vSetHigh(CS2_SPI3)
#define CS2_SPI3_SetLow()    PIN_vSetLow(CS2_SPI3)
#define CS2_SPI3_GetValue()  PIN_vGetValue(CS2_SPI3)

#ifdef CS2_SPI3_START_HIGH
#define CS2_SPI3_SetStart()  CS2_SPI3_SetHigh()
#else
#define CS2_SPI3_SetStart()  
#endif

#else
#define CS2_SPI3_NotImplemented
#define CS2_SPI3_SetDigitalOutput() 
#define CS2_SPI3_SetHigh()
#define CS2_SPI3_SetLow()
#define CS2_SPI3_GetValue() 0
#define CS2_SPI3_SetStart()
#endif
#else
#define CS2_SPI3_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif


#ifndef CS2_SPI4_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */

#ifdef CS2_SPI4

#define CS2_SPI4_SetDigitalOutput() PIN_vSetDigitalOutput(CS2_SPI4)
#define CS2_SPI4_SetHigh()   PIN_vSetHigh(CS2_SPI4)
#define CS2_SPI4_SetLow()    PIN_vSetLow(CS2_SPI4)
#define CS2_SPI4_GetValue()  PIN_vGetValue(CS2_SPI4)

#ifdef CS2_SPI4_START_HIGH
#define CS2_SPI4_SetStart()  CS2_SPI4_SetHigh()
#else
#define CS2_SPI4_SetStart()  
#endif

#else
#define CS2_SPI4_NotImplemented
#define CS2_SPI4_SetDigitalOutput() 
#define CS2_SPI4_SetHigh()
#define CS2_SPI4_SetLow()
#define CS2_SPI4_GetValue() 0
#define CS2_SPI4_SetStart()
#endif
#else
#define CS2_SPI4_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



/* *****************************************************************************
 * CS3_SPI Definitions - Outputs
 **************************************************************************** */
#ifndef CS3_SPI1_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef CS3_SPI1

#define CS3_SPI1_SetDigitalOutput() PIN_vSetDigitalOutput(CS3_SPI1)
#define CS3_SPI1_SetHigh()   PIN_vSetHigh(CS3_SPI1)
#define CS3_SPI1_SetLow()    PIN_vSetLow(CS3_SPI1)
#define CS3_SPI1_GetValue()  PIN_vGetValue(CS3_SPI1)

#ifdef CS3_SPI1_START_HIGH
#define CS3_SPI1_SetStart()  CS3_SPI1_SetHigh()
#else
#define CS3_SPI1_SetStart()  
#endif


#else
#define CS3_SPI1_NotImplemented
#define CS3_SPI1_SetDigitalOutput() 
#define CS3_SPI1_SetHigh()
#define CS3_SPI1_SetLow()
#define CS3_SPI1_GetValue() 0
#define CS3_SPI1_SetStart()
#endif
#else
#define CS3_SPI1_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef CS3_SPI2_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef CS3_SPI2

#define CS3_SPI2_SetDigitalOutput() PIN_vSetDigitalOutput(CS3_SPI2)
#define CS3_SPI2_SetHigh()   PIN_vSetHigh(CS3_SPI2)
#define CS3_SPI2_SetLow()    PIN_vSetLow(CS3_SPI2)
#define CS3_SPI2_GetValue()  PIN_vGetValue(CS3_SPI2)

#ifdef CS3_SPI2_START_HIGH
#define CS3_SPI2_SetStart()  CS3_SPI2_SetHigh()
#else
#define CS3_SPI2_SetStart()  
#endif

#else
#define CS3_SPI2_NotImplemented
#define CS3_SPI2_SetDigitalOutput() 
#define CS3_SPI2_SetHigh()
#define CS3_SPI2_SetLow()
#define CS3_SPI2_GetValue() 0
#define CS3_SPI2_SetStart()
#endif
#else
#define CS3_SPI2_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef CS3_SPI3_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef CS3_SPI3

#define CS3_SPI3_SetDigitalOutput() PIN_vSetDigitalOutput(CS3_SPI3)
#define CS3_SPI3_SetHigh()   PIN_vSetHigh(CS3_SPI3)
#define CS3_SPI3_SetLow()    PIN_vSetLow(CS3_SPI3)
#define CS3_SPI3_GetValue()  PIN_vGetValue(CS3_SPI3)

#ifdef CS3_SPI3_START_HIGH
#define CS3_SPI3_SetStart()  CS3_SPI3_SetHigh()
#else
#define CS3_SPI3_SetStart()  
#endif

#else
#define CS3_SPI3_NotImplemented
#define CS3_SPI3_SetDigitalOutput() 
#define CS3_SPI3_SetHigh()
#define CS3_SPI3_SetLow()
#define CS3_SPI3_GetValue() 0
#define CS3_SPI3_SetStart()
#endif
#else
#define CS3_SPI3_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef CS3_SPI4_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef CS3_SPI4

#define CS3_SPI4_SetDigitalOutput() PIN_vSetDigitalOutput(CS3_SPI4)
#define CS3_SPI4_SetHigh()   PIN_vSetHigh(CS3_SPI4)
#define CS3_SPI4_SetLow()    PIN_vSetLow(CS3_SPI4)
#define CS3_SPI4_GetValue()  PIN_vGetValue(CS3_SPI4)

#ifdef CS3_SPI4_START_HIGH
#define CS3_SPI4_SetStart()  CS3_SPI4_SetHigh()
#else
#define CS3_SPI4_SetStart()  
#endif

#else
#define CS3_SPI4_NotImplemented
#define CS3_SPI4_SetDigitalOutput() 
#define CS3_SPI4_SetHigh()
#define CS3_SPI4_SetLow()
#define CS3_SPI4_GetValue() 0
#define CS3_SPI4_SetStart()
#endif
#else
#define CS3_SPI4_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif




/* *****************************************************************************
 * LOAD_DAC Definitions - Outputs
 **************************************************************************** */
#ifndef LOAD_DAC_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef LOAD_DAC

#define LOAD_DAC_SetDigitalOutput() PIN_vSetDigitalOutput(LOAD_DAC)
#define LOAD_DAC_SetHigh()   PIN_vSetHigh(LOAD_DAC)
#define LOAD_DAC_SetLow()    PIN_vSetLow(LOAD_DAC)
#define LOAD_DAC_GetValue()  PIN_vGetValue(LOAD_DAC)

#ifdef LOAD_DAC_START_HIGH
#define LOAD_DAC_SetStart()  LOAD_DAC_SetHigh()
#else
#define LOAD_DAC_SetStart()  
#endif

#else
#define LOAD_DAC_NotImplemented
#define LOAD_DAC_SetDigitalOutput() 
#define LOAD_DAC_SetHigh()
#define LOAD_DAC_SetLow()
#define LOAD_DAC_GetValue() 0
#define LOAD_DAC_SetStart()
#endif
#else
#define LOAD_DAC_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



/* *****************************************************************************
 * CLEAR_DAC Definitions - Outputs
 **************************************************************************** */
#ifndef CLEAR_DAC_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef CLEAR_DAC

#define CLEAR_DAC_SetDigitalOutput() PIN_vSetDigitalOutput(CLEAR_DAC)
#define CLEAR_DAC_SetHigh()   PIN_vSetHigh(CLEAR_DAC)
#define CLEAR_DAC_SetLow()    PIN_vSetLow(CLEAR_DAC)
#define CLEAR_DAC_GetValue()  PIN_vGetValue(CLEAR_DAC)

#ifdef CLEAR_DAC_START_HIGH
#define CLEAR_DAC_SetStart()  CLEAR_DAC_SetHigh()
#else
#define CLEAR_DAC_SetStart()  
#endif

#else
#define CLEAR_DAC_NotImplemented
#define CLEAR_DAC_SetDigitalOutput() 
#define CLEAR_DAC_SetHigh()
#define CLEAR_DAC_SetLow()
#define CLEAR_DAC_GetValue() 0
#define CLEAR_DAC_SetStart()
#endif
#else
#define CLEAR_DAC_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



/* *****************************************************************************
 * TXEN_U Definitions - Outputs
 **************************************************************************** */
#ifndef TXEN_U1_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef TXEN_U1

#define TXEN_U1_SetDigitalOutput() PIN_vSetDigitalOutput(TXEN_U1)
#define TXEN_U1_SetHigh()   PIN_vSetHigh(TXEN_U1)
#define TXEN_U1_SetLow()    PIN_vSetLow(TXEN_U1)
#define TXEN_U1_GetValue()  PIN_vGetValue(TXEN_U1)

#ifdef TXEN_U1_START_HIGH
#define TXEN_U1_SetStart()  TXEN_U1_SetHigh()
#else
#define TXEN_U1_SetStart()  
#endif

#else
#define TXEN_U1_NotImplemented
#define TXEN_U1_SetDigitalOutput() 
#define TXEN_U1_SetHigh()
#define TXEN_U1_SetLow()
#define TXEN_U1_GetValue() 0
#define TXEN_U1_SetStart()
#endif
#else
#define TXEN_U1_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef TXEN_U2_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef TXEN_U2

#define TXEN_U2_SetDigitalOutput() PIN_vSetDigitalOutput(TXEN_U2)
#define TXEN_U2_SetHigh()   PIN_vSetHigh(TXEN_U2)
#define TXEN_U2_SetLow()    PIN_vSetLow(TXEN_U2)
#define TXEN_U2_GetValue()  PIN_vGetValue(TXEN_U2)

#ifdef TXEN_U2_START_HIGH
#define TXEN_U2_SetStart()  TXEN_U2_SetHigh()
#else
#define TXEN_U2_SetStart()  
#endif

#else
#define TXEN_U2_NotImplemented
#define TXEN_U2_SetDigitalOutput() 
#define TXEN_U2_SetHigh()
#define TXEN_U2_SetLow()
#define TXEN_U2_GetValue() 0
#define TXEN_U2_SetStart()
#endif
#else
#define TXEN_U2_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif


#ifndef TXEN_U3_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */

#ifdef TXEN_U3

#define TXEN_U3_SetDigitalOutput() PIN_vSetDigitalOutput(TXEN_U3)
#define TXEN_U3_SetHigh()   PIN_vSetHigh(TXEN_U3)
#define TXEN_U3_SetLow()    PIN_vSetLow(TXEN_U3)
#define TXEN_U3_GetValue()  PIN_vGetValue(TXEN_U3)

#ifdef TXEN_U3_START_HIGH
#define TXEN_U3_SetStart()  TXEN_U3_SetHigh()
#else
#define TXEN_U3_SetStart()  
#endif

#else
#define TXEN_U3_NotImplemented
#define TXEN_U3_SetDigitalOutput() 
#define TXEN_U3_SetHigh()
#define TXEN_U3_SetLow()
#define TXEN_U3_GetValue() 0
#define TXEN_U3_SetStart()
#endif
#else
#define TXEN_U3_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef TXEN_U4_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef TXEN_U4

#define TXEN_U4_SetDigitalOutput() PIN_vSetDigitalOutput(TXEN_U4)
#define TXEN_U4_SetHigh()   PIN_vSetHigh(TXEN_U4)
#define TXEN_U4_SetLow()    PIN_vSetLow(TXEN_U4)
#define TXEN_U4_GetValue()  PIN_vGetValue(TXEN_U4)

#ifdef TXEN_U4_START_HIGH
#define TXEN_U4_SetStart()  TXEN_U4_SetHigh()
#else
#define TXEN_U4_SetStart()  
#endif

#else
#define TXEN_U4_NotImplemented
#define TXEN_U4_SetDigitalOutput() 
#define TXEN_U4_SetHigh()
#define TXEN_U4_SetLow()
#define TXEN_U4_GetValue() 0
#define TXEN_U4_SetStart()
#endif
#else
#define TXEN_U4_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



/* *****************************************************************************
 * RXEN_U Definitions - Outputs
 **************************************************************************** */
#ifndef RXEN_U1_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef RXEN_U1

#define RXEN_U1_SetDigitalOutput() PIN_vSetDigitalOutput(RXEN_U1)
#define RXEN_U1_SetHigh()   PIN_vSetHigh(RXEN_U1)
#define RXEN_U1_SetLow()    PIN_vSetLow(RXEN_U1)
#define RXEN_U1_GetValue()  PIN_vGetValue(RXEN_U1)

#ifdef RXEN_U1_START_HIGH
#define RXEN_U1_SetStart()  RXEN_U1_SetHigh()
#else
#define RXEN_U1_SetStart()  
#endif

#else
#define RXEN_U1_NotImplemented
#define RXEN_U1_SetDigitalOutput() 
#define RXEN_U1_SetHigh()
#define RXEN_U1_SetLow()
#define RXEN_U1_GetValue() 0
#define RXEN_U1_SetStart()
#endif
#else
#define RXEN_U1_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef RXEN_U2_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef RXEN_U2

#define RXEN_U2_SetDigitalOutput() PIN_vSetDigitalOutput(RXEN_U2)
#define RXEN_U2_SetHigh()   PIN_vSetHigh(RXEN_U2)
#define RXEN_U2_SetLow()    PIN_vSetLow(RXEN_U2)
#define RXEN_U2_GetValue()  PIN_vGetValue(RXEN_U2)

#ifdef RXEN_U2_START_HIGH
#define RXEN_U2_SetStart()  RXEN_U2_SetHigh()
#else
#define RXEN_U2_SetStart()  
#endif

#else
#define RXEN_U2_NotImplemented
#define RXEN_U2_SetDigitalOutput() 
#define RXEN_U2_SetHigh()
#define RXEN_U2_SetLow()
#define RXEN_U2_GetValue() 0
#define RXEN_U2_SetStart()
#endif
#else
#define RXEN_U2_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef RXEN_U3_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef RXEN_U3

#define RXEN_U3_SetDigitalOutput() PIN_vSetDigitalOutput(RXEN_U3)
#define RXEN_U3_SetHigh()   PIN_vSetHigh(RXEN_U3)
#define RXEN_U3_SetLow()    PIN_vSetLow(RXEN_U3)
#define RXEN_U3_GetValue()  PIN_vGetValue(RXEN_U3)

#ifdef RXEN_U3_START_HIGH
#define RXEN_U3_SetStart()  RXEN_U3_SetHigh()
#else
#define RXEN_U3_SetStart()  
#endif


#else
#define RXEN_U3_NotImplemented
#define RXEN_U3_SetDigitalOutput() 
#define RXEN_U3_SetHigh()
#define RXEN_U3_SetLow()
#define RXEN_U3_GetValue() 0
#define RXEN_U3_SetStart()
#endif
#else
#define RXEN_U3_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef RXEN_U4_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */  

#ifdef RXEN_U4

#define RXEN_U4_SetDigitalOutput() PIN_vSetDigitalOutput(RXEN_U4)
#define RXEN_U4_SetHigh()   PIN_vSetHigh(RXEN_U4)
#define RXEN_U4_SetLow()    PIN_vSetLow(RXEN_U4)
#define RXEN_U4_GetValue()  PIN_vGetValue(RXEN_U4)

#ifdef RXEN_U4_START_HIGH
#define RXEN_U4_SetStart()  RXEN_U4_SetHigh()
#else
#define RXEN_U4_SetStart()  
#endif

#else
#define RXEN_U4_NotImplemented
#define RXEN_U4_SetDigitalOutput() 
#define RXEN_U4_SetHigh()
#define RXEN_U4_SetLow()
#define RXEN_U4_GetValue() 0
#define RXEN_U4_SetStart()

#endif
#else
#define RXEN_U4_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif


/* *****************************************************************************
 * FLIR Definitions - Outputs
 **************************************************************************** */
#ifndef FLIR_RST_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */  

#ifdef FLIR_RST

#define FLIR_RST_SetDigitalOutput() PIN_vSetDigitalOutput(FLIR_RST)
#define FLIR_RST_SetHigh()   PIN_vSetHigh(FLIR_RST)
#define FLIR_RST_SetLow()    PIN_vSetLow(FLIR_RST)
#define FLIR_RST_GetValue()  PIN_vGetValue(FLIR_RST)

#ifdef FLIR_RST_START_HIGH
#define FLIR_RST_SetStart()  FLIR_RST_SetHigh()
#else
#define FLIR_RST_SetStart()  
#endif

#ifdef FLIR_RST_OPEN_DRAIN
#define FLIR_RST_SetOpenDrain()  PIN_vSetOpenDrain(FLIR_RST)
#else
#define FLIR_RST_SetOpenDrain()  
#endif

#else
#define FLIR_RST_NotImplemented
#define FLIR_RST_SetDigitalOutput() 
#define FLIR_RST_SetHigh()
#define FLIR_RST_SetLow()
#define FLIR_RST_GetValue() 0
#define FLIR_RST_SetStart()
#define FLIR_RST_SetOpenDrain()  

#endif
#else
#define FLIR_RST_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#define FLIR_RST_SetOpenDrain()/* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif

/* *****************************************************************************
 * FPGA Definitions - Outputs
 **************************************************************************** */

#ifndef FPGA_RST_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */  

#ifdef FPGA_RST

#define FPGA_RST_SetDigitalOutput() PIN_vSetDigitalOutput(FPGA_RST)
#define FPGA_RST_SetHigh()   PIN_vSetHigh(FPGA_RST)
#define FPGA_RST_SetLow()    PIN_vSetLow(FPGA_RST)
#define FPGA_RST_GetValue()  PIN_vGetValue(FPGA_RST)

#ifdef FPGA_RST_START_HIGH
#define FPGA_RST_SetStart()  FPGA_RST_SetHigh()
#else
#define FPGA_RST_SetStart()  
#endif

#ifdef FPGA_RST_OPEN_DRAIN
#define FPGA_RST_SetOpenDrain()  PIN_vSetOpenDrain(FPGA_RST)
#else
#define FPGA_RST_SetOpenDrain()  
#endif

#else
#define FPGA_RST_NotImplemented
#define FPGA_RST_SetDigitalOutput() 
#define FPGA_RST_SetHigh()
#define FPGA_RST_SetLow()
#define FPGA_RST_GetValue() 0
#define FPGA_RST_SetStart()
#define FPGA_RST_SetOpenDrain()  

#endif
#else
#define FPGA_RST_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#define FPGA_RST_SetOpenDrain()/* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif




/* *****************************************************************************
 * DGB Definitions - Outputs
 **************************************************************************** */

#ifndef DBG1_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef DBG1

#define DBG1_SetDigitalOutput() PIN_vSetDigitalOutput(DBG1)
#define DBG1_SetHigh()   PIN_vSetHigh(DBG1)
#define DBG1_SetLow()    PIN_vSetLow(DBG1)
#define DBG1_GetValue()  PIN_vGetValue(DBG1)

#ifdef DBG1_START_HIGH
#define DBG1_SetStart()  DBG1_SetHigh()
#else
#define DBG1_SetStart()  
#endif

#else

#define DBG1_SetDigitalOutput() 
#define DBG1_SetHigh()
#define DBG1_SetLow()
#define DBG1_GetValue() 0
#define DBG1_SetStart()
#endif
#else
#define DBG1_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef DBG2_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef DBG2

#define DBG2_SetDigitalOutput() PIN_vSetDigitalOutput(DBG2)
#define DBG2_SetHigh()   PIN_vSetHigh(DBG2)
#define DBG2_SetLow()    PIN_vSetLow(DBG2)
#define DBG2_GetValue()  PIN_vGetValue(DBG2)

#ifdef DBG2_START_HIGH
#define DBG2_SetStart()  DBG2_SetHigh()
#else
#define DBG2_SetStart()  
#endif

#else

#define DBG2_SetDigitalOutput() 
#define DBG2_SetHigh()
#define DBG2_SetLow()
#define DBG2_GetValue() 0
#define DBG2_SetStart()
#endif
#else
#define DBG2_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef DBG3_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef DBG3

#define DBG3_SetDigitalOutput() PIN_vSetDigitalOutput(DBG3)
#define DBG3_SetHigh()   PIN_vSetHigh(DBG3)
#define DBG3_SetLow()    PIN_vSetLow(DBG3)
#define DBG3_GetValue()  PIN_vGetValue(DBG3)

#ifdef DBG3_START_HIGH
#define DBG3_SetStart()  DBG3_SetHigh()
#else
#define DBG3_SetStart()  
#endif

#else

#define DBG3_SetDigitalOutput() 
#define DBG3_SetHigh()
#define DBG3_SetLow()
#define DBG3_GetValue() 0
#define DBG3_SetStart()
#endif
#else
#define DBG3_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif



#ifndef DBG4_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef DBG4

#define DBG4_SetDigitalOutput() PIN_vSetDigitalOutput(DBG4)
#define DBG4_SetHigh()   PIN_vSetHigh(DBG4)
#define DBG4_SetLow()    PIN_vSetLow(DBG4)
#define DBG4_GetValue()  PIN_vGetValue(DBG4)

#ifdef DBG4_START_HIGH
#define DBG4_SetStart()  DBG4_SetHigh()
#else
#define DBG4_SetStart()  
#endif

#else

#define DBG4_SetDigitalOutput() 
#define DBG4_SetHigh()
#define DBG4_SetLow()
#define DBG4_GetValue() 0
#define DBG4_SetStart()
#endif
#else
#define DBG4_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif








/* *****************************************************************************
 * DI Definitions - Inputs
 **************************************************************************** */
#ifndef DI1_GetValue     /* Use From Pin Manager If Request Use MCC_PIN */
#ifdef DI1
#define DI1_SetDigitalInput()  PIN_vSetDigitalInput(DI1)
#define DI1_GetValue()  PIN_vGetValue(DI1)
#else
#define DI1_SetDigitalInput() 
#define DI1_GetValue() 0
#endif
#endif

#ifndef DI2_GetValue     /* Use From Pin Manager If Request Use MCC_PIN */     
#ifdef DI2
#define DI2_SetDigitalInput()  PIN_vSetDigitalInput(DI2)
#define DI2_GetValue()  PIN_vGetValue(DI2)
#else
#define DI2_SetDigitalInput() 
#define DI2_GetValue() 0
#endif
#endif

#ifndef DI3_GetValue     /* Use From Pin Manager If Request Use MCC_PIN */    
#ifdef DI3
#define DI3_SetDigitalInput()  PIN_vSetDigitalInput(DI3)
#define DI3_GetValue()  PIN_vGetValue(DI3)
#else
#define DI3_SetDigitalInput() 
#define DI3_GetValue() 0
#endif
#endif

#ifndef DI4_GetValue     /* Use From Pin Manager If Request Use MCC_PIN */    
#ifdef DI4
#define DI4_SetDigitalInput()  PIN_vSetDigitalInput(DI4)
#define DI4_GetValue()  PIN_vGetValue(DI4)
#else
#define DI4_SetDigitalInput() 
#define DI4_GetValue() 0
#endif
#endif

#ifndef DI5_GetValue     /* Use From Pin Manager If Request Use MCC_PIN */     
#ifdef DI5
#define DI5_SetDigitalInput()  PIN_vSetDigitalInput(DI5)
#define DI5_GetValue()  PIN_vGetValue(DI5)
#else
#define DI5_SetDigitalInput() 
#define DI5_GetValue() 0
#endif
#endif

#ifndef DI6_GetValue     /* Use From Pin Manager If Request Use MCC_PIN */    
#ifdef DI6
#define DI6_SetDigitalInput()  PIN_vSetDigitalInput(DI6)
#define DI6_GetValue()  PIN_vGetValue(DI6)
#else
#define DI6_SetDigitalInput() 
#define DI6_GetValue() 0
#endif
#endif

#ifndef DI7_GetValue     /* Use From Pin Manager If Request Use MCC_PIN */    
#ifdef DI7
#define DI7_SetDigitalInput()  PIN_vSetDigitalInput(DI7)
#define DI7_GetValue()  PIN_vGetValue(DI7)
#else
#define DI7_SetDigitalInput() 
#define DI7_GetValue() 0
#endif
#endif

#ifndef DI8_GetValue     /* Use From Pin Manager If Request Use MCC_PIN */     
#ifdef DI8
#define DI8_SetDigitalInput()  PIN_vSetDigitalInput(DI8)
#define DI8_GetValue()  PIN_vGetValue(DI8)
#else
#define DI8_SetDigitalInput() 
#define DI8_GetValue() 0
#endif
#endif



/* *****************************************************************************
 * FLT Definitions - Inputs
 **************************************************************************** */
#ifndef FLT1_GetValue     /* Use From Pin Manager If Request Use MCC_PIN */    
#ifdef FLT1
#define FLT1_SetDigitalInput()  PIN_vSetDigitalInput(FLT1)
#define FLT1_GetValue()  PIN_vGetValue(FLT1)
#else
#define FLT1_SetDigitalInput() 
#define FLT1_GetValue() 0
#endif
#endif

#ifndef FLT2_GetValue     /* Use From Pin Manager If Request Use MCC_PIN */   
#ifdef FLT2
#define FLT2_SetDigitalInput()  PIN_vSetDigitalInput(FLT2)
#define FLT2_GetValue()  PIN_vGetValue(FLT2)
#else
#define FLT2_SetDigitalInput() 
#define FLT2_GetValue() 0
#endif
#endif

#ifndef FLT3_GetValue     /* Use From Pin Manager If Request Use MCC_PIN */   
#ifdef FLT3
#define FLT3_SetDigitalInput()  PIN_vSetDigitalInput(FLT3)
#define FLT3_GetValue()  PIN_vGetValue(FLT3)
#else
#define FLT3_SetDigitalInput() 
#define FLT3_GetValue() 0
#endif
#endif

#ifndef FLT4_GetValue     /* Use From Pin Manager If Request Use MCC_PIN */    
#ifdef FLT4
#define FLT4_SetDigitalInput()  PIN_vSetDigitalInput(FLT4)
#define FLT4_GetValue()  PIN_vGetValue(FLT4)
#else
#define FLT4_SetDigitalInput() 
#define FLT4_GetValue() 0
#endif
#endif

#ifndef FLT5_GetValue     /* Use From Pin Manager If Request Use MCC_PIN */    
#ifdef FLT5
#define FLT5_SetDigitalInput()  PIN_vSetDigitalInput(FLT5)
#define FLT5_GetValue()  PIN_vGetValue(FLT5)
#else
#define FLT5_SetDigitalInput() 
#define FLT5_GetValue() 0
#endif
#endif

#ifndef FLT6_GetValue     /* Use From Pin Manager If Request Use MCC_PIN */    
#ifdef FLT6
#define FLT6_SetDigitalInput()  PIN_vSetDigitalInput(FLT6)
#define FLT6_GetValue()  PIN_vGetValue(FLT6)
#else
#define FLT6_SetDigitalInput() 
#define FLT6_GetValue() 0
#endif
#endif

#ifndef FLT7_GetValue     /* Use From Pin Manager If Request Use MCC_PIN */   
#ifdef FLT7
#define FLT7_SetDigitalInput()  PIN_vSetDigitalInput(FLT7)
#define FLT7_GetValue()  PIN_vGetValue(FLT7)
#else
#define FLT7_SetDigitalInput() 
#define FLT7_GetValue() 0
#endif
#endif

#ifndef FLT8_GetValue     /* Use From Pin Manager If Request Use MCC_PIN */    
#ifdef FLT8
#define FLT8_SetDigitalInput()  PIN_vSetDigitalInput(FLT8)
#define FLT8_GetValue()  PIN_vGetValue(FLT8)
#else
#define FLT8_SetDigitalInput() 
#define FLT8_GetValue() 0
#endif
#endif

#ifndef FLIR_V_SYNC_GetValue     /* Use From Pin Manager If Request Use MCC_PIN */    
#ifdef FLIR_V_SYNC
#define FLIR_V_SYNC_SetDigitalInput()  PIN_vSetDigitalInput(FLIR_V_SYNC)
#define FLIR_V_SYNC_GetValue()  PIN_vGetValue(FLIR_V_SYNC)
#else
#define FLIR_V_SYNC_SetDigitalInput() 
#define FLIR_V_SYNC_GetValue() 0
#endif
#endif


/* *****************************************************************************
 * Analog - Inputs
 **************************************************************************** */

#ifdef IFBK_S1L1
#define IFBK_S1L1_SetAnalog()  PIN_vSetAnalog(IFBK_S1L1)
#else
#define IFBK_S1L1_SetAnalog()  
#endif

#ifdef IFBK_S1L2
#define IFBK_S1L2_SetAnalog()  PIN_vSetAnalog(IFBK_S1L2)
#else
#define IFBK_S1L2_SetAnalog()  
#endif

#ifdef IFBK_S1L3
#define IFBK_S1L3_SetAnalog()  PIN_vSetAnalog(IFBK_S1L3)
#else
#define IFBK_S1L3_SetAnalog()  
#endif

#ifdef IFBK_S2L1
#define IFBK_S2L1_SetAnalog()  PIN_vSetAnalog(IFBK_S2L1)
#else
#define IFBK_S2L1_SetAnalog()  
#endif

#ifdef IFBK_S2L2
#define IFBK_S2L2_SetAnalog()  PIN_vSetAnalog(IFBK_S2L2)
#else
#define IFBK_S2L2_SetAnalog()  
#endif

#ifdef IFBK_S2L3
#define IFBK_S2L3_SetAnalog()  PIN_vSetAnalog(IFBK_S2L3)
#else
#define IFBK_S2L3_SetAnalog()  
#endif

#ifdef IFBK_S3L1
#define IFBK_S3L1_SetAnalog()  PIN_vSetAnalog(IFBK_S3L1)
#else
#define IFBK_S3L1_SetAnalog()  
#endif

#ifdef IFBK_S3L2
#define IFBK_S3L2_SetAnalog()  PIN_vSetAnalog(IFBK_S3L2)
#else
#define IFBK_S3L2_SetAnalog()  
#endif

#ifdef IFBK_S3L3
#define IFBK_S3L3_SetAnalog()  PIN_vSetAnalog(IFBK_S3L3)
#else
#define IFBK_S3L3_SetAnalog()  
#endif

#ifdef U_DC_LINK
#define U_DC_LINK_SetAnalog()  PIN_vSetAnalog(U_DC_LINK)
#else
#define U_DC_LINK_SetAnalog()  
#endif

#ifdef UFBK_2
#define UFBK_2_SetAnalog()  PIN_vSetAnalog(UFBK_2)
#else
#define UFBK_2_SetAnalog()  
#endif

#ifdef UFBK_3
#define UFBK_3_SetAnalog()  PIN_vSetAnalog(UFBK_3)
#else
#define UFBK_3_SetAnalog()  
#endif

#ifdef UFBK_4
#define UFBK_4_SetAnalog()  PIN_vSetAnalog(UFBK_4)
#else
#define UFBK_4_SetAnalog()  
#endif

#ifdef UFBK_5
#define UFBK_5_SetAnalog()  PIN_vSetAnalog(UFBK_5)
#else
#define UFBK_5_SetAnalog()  
#endif

#ifdef IFBK_DRV1
#define IFBK_DRV1_SetAnalog()  PIN_vSetAnalog(IFBK_DRV1)
#else
#define IFBK_DRV1_SetAnalog()  
#endif

#ifdef IFBK_DRV2
#define IFBK_DRV2_SetAnalog()  PIN_vSetAnalog(IFBK_DRV2)
#else
#define IFBK_DRV2_SetAnalog()  
#endif

#ifdef IFBK_DRV3
#define IFBK_DRV3_SetAnalog()  PIN_vSetAnalog(IFBK_DRV3)
#else
#define IFBK_DRV3_SetAnalog()  
#endif

#ifdef IFBK_DRV4
#define IFBK_DRV4_SetAnalog()  PIN_vSetAnalog(IFBK_DRV4)
#else
#define IFBK_DRV4_SetAnalog()  
#endif

#ifdef IFBK_DRV5
#define IFBK_DRV5_SetAnalog()  PIN_vSetAnalog(IFBK_DRV5)
#else
#define IFBK_DRV5_SetAnalog()  
#endif

#ifdef IFBK_DRV6
#define IFBK_DRV6_SetAnalog()  PIN_vSetAnalog(IFBK_DRV6)
#else
#define IFBK_DRV6_SetAnalog()  
#endif

#ifdef TEMP_FBK1
#define TEMP_FBK1_SetAnalog()  PIN_vSetAnalog(TEMP_FBK1)
#else
#define TEMP_FBK1_SetAnalog()  
#endif

#ifdef TEMP_FBK2
#define TEMP_FBK2_SetAnalog()  PIN_vSetAnalog(TEMP_FBK2)
#else
#define TEMP_FBK2_SetAnalog()  
#endif

#ifdef TEMP_FBK3
#define TEMP_FBK3_SetAnalog()  PIN_vSetAnalog(TEMP_FBK3)
#else
#define TEMP_FBK3_SetAnalog()  
#endif

#ifdef TEMP_FBK4
#define TEMP_FBK4_SetAnalog()  PIN_vSetAnalog(TEMP_FBK4)
#else
#define TEMP_FBK4_SetAnalog()  
#endif

#ifdef TEMP_FBK5
#define TEMP_FBK5_SetAnalog()  PIN_vSetAnalog(TEMP_FBK5)
#else
#define TEMP_FBK5_SetAnalog()  
#endif

#ifdef TEMP_FBK6
#define TEMP_FBK6_SetAnalog()  PIN_vSetAnalog(TEMP_FBK6)
#else
#define TEMP_FBK6_SetAnalog()  
#endif

#ifdef ADC_REF
#define ADC_REF_SetAnalog()  PIN_vSetAnalog(ADC_REF)
#else
#define ADC_REF_SetAnalog()  
#endif

/* *****************************************************************************
 * Function Definitions - Outputs and Inputs
 **************************************************************************** */

#ifndef F0_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */  
#ifdef F0

#define F0_SetDigitalInput() PIN_vSetDigitalInput(F0)
#define F0_SetDigitalOutput() PIN_vSetDigitalOutput(F0)
#define F0_SetHigh()   PIN_vSetHigh(F0)
#define F0_SetLow()    PIN_vSetLow(F0)
#define F0_GetValue()  PIN_vGetValue(F0)

#ifdef F0_START_HIGH
#define F0_SetStart()  F0_SetHigh()
#else
#define F0_SetStart()  
#endif

#ifdef F0_OPEN_DRAIN
#define F0_SetOpenDrain()  PIN_vSetOpenDrain(F0)
#else
#define F0_SetOpenDrain()  
#endif

#else
#define F0_NotImplemented
#define F0_SetDigitalInput() 
#define F0_SetDigitalOutput() 
#define F0_SetHigh()
#define F0_SetLow()
#define F0_GetValue() 0
#define F0_SetStart()
#define F0_SetOpenDrain()  

#endif
#else
#define F0_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#define F0_SetOpenDrain()/* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif


#ifndef F1_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */  
#ifdef F1

#define F1_SetDigitalInput() PIN_vSetDigitalInput(F1)
#define F1_SetDigitalOutput() PIN_vSetDigitalOutput(F1)
#define F1_SetHigh()   PIN_vSetHigh(F1)
#define F1_SetLow()    PIN_vSetLow(F1)
#define F1_GetValue()  PIN_vGetValue(F1)

#ifdef F1_START_HIGH
#define F1_SetStart()  F1_SetHigh()
#else
#define F1_SetStart()  
#endif

#ifdef F1_OPEN_DRAIN
#define F1_SetOpenDrain()  PIN_vSetOpenDrain(F1)
#else
#define F1_SetOpenDrain()  
#endif

#else
#define F1_NotImplemented
#define F1_SetDigitalInput() 
#define F1_SetDigitalOutput() 
#define F1_SetHigh()
#define F1_SetLow()
#define F1_GetValue() 0
#define F1_SetStart()
#define F1_SetOpenDrain()  

#endif
#else
#define F1_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#define F1_SetOpenDrain()/* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif


#ifndef F2_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */  
#ifdef F2

#define F2_SetDigitalInput() PIN_vSetDigitalInput(F2)
#define F2_SetDigitalOutput() PIN_vSetDigitalOutput(F2)
#define F2_SetHigh()   PIN_vSetHigh(F2)
#define F2_SetLow()    PIN_vSetLow(F2)
#define F2_GetValue()  PIN_vGetValue(F2)

#ifdef F2_START_HIGH
#define F2_SetStart()  F2_SetHigh()
#else
#define F2_SetStart()  
#endif

#ifdef F2_OPEN_DRAIN
#define F2_SetOpenDrain()  PIN_vSetOpenDrain(F2)
#else
#define F2_SetOpenDrain()  
#endif

#else
#define F2_NotImplemented
#define F2_SetDigitalInput() 
#define F2_SetDigitalOutput() 
#define F2_SetHigh()
#define F2_SetLow()
#define F2_GetValue() 0
#define F2_SetStart()
#define F2_SetOpenDrain()  

#endif
#else
#define F2_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#define F2_SetOpenDrain()/* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif


#ifndef F3_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */  
#ifdef F3

#define F3_SetDigitalInput() PIN_vSetDigitalInput(F3)
#define F3_SetDigitalOutput() PIN_vSetDigitalOutput(F3)
#define F3_SetHigh()   PIN_vSetHigh(F3)
#define F3_SetLow()    PIN_vSetLow(F3)
#define F3_GetValue()  PIN_vGetValue(F3)

#ifdef F3_START_HIGH
#define F3_SetStart()  F3_SetHigh()
#else
#define F3_SetStart()  
#endif

#ifdef F3_OPEN_DRAIN
#define F3_SetOpenDrain()  PIN_vSetOpenDrain(F3)
#else
#define F3_SetOpenDrain()  
#endif

#else
#define F3_NotImplemented
#define F3_SetDigitalInput() 
#define F3_SetDigitalOutput() 
#define F3_SetHigh()
#define F3_SetLow()
#define F3_GetValue() 0
#define F3_SetStart()
#define F3_SetOpenDrain()  

#endif
#else
#define F3_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#define F3_SetOpenDrain()/* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif


#ifndef F4_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */  
#ifdef F4

#define F4_SetDigitalInput() PIN_vSetDigitalInput(F4)
#define F4_SetDigitalOutput() PIN_vSetDigitalOutput(F4)
#define F4_SetHigh()   PIN_vSetHigh(F4)
#define F4_SetLow()    PIN_vSetLow(F4)
#define F4_GetValue()  PIN_vGetValue(F4)

#ifdef F4_START_HIGH
#define F4_SetStart()  F4_SetHigh()
#else
#define F4_SetStart()  
#endif

#ifdef F4_OPEN_DRAIN
#define F4_SetOpenDrain()  PIN_vSetOpenDrain(F4)
#else
#define F4_SetOpenDrain()  
#endif

#else
#define F4_NotImplemented
#define F4_SetDigitalInput() 
#define F4_SetDigitalOutput() 
#define F4_SetHigh()
#define F4_SetLow()
#define F4_GetValue() 0
#define F4_SetStart()
#define F4_SetOpenDrain()  

#endif
#else
#define F4_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#define F4_SetOpenDrain()/* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif


#ifndef F5_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */  
#ifdef F5

#define F5_SetDigitalInput() PIN_vSetDigitalInput(F5)
#define F5_SetDigitalOutput() PIN_vSetDigitalOutput(F5)
#define F5_SetHigh()   PIN_vSetHigh(F5)
#define F5_SetLow()    PIN_vSetLow(F5)
#define F5_GetValue()  PIN_vGetValue(F5)

#ifdef F5_START_HIGH
#define F5_SetStart()  F5_SetHigh()
#else
#define F5_SetStart()  
#endif

#ifdef F5_OPEN_DRAIN
#define F5_SetOpenDrain()  PIN_vSetOpenDrain(F5)
#else
#define F5_SetOpenDrain()  
#endif

#else
#define F5_NotImplemented
#define F5_SetDigitalInput() 
#define F5_SetDigitalOutput() 
#define F5_SetHigh()
#define F5_SetLow()
#define F5_GetValue() 0
#define F5_SetStart()
#define F5_SetOpenDrain()  

#endif
#else
#define F5_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#define F5_SetOpenDrain()/* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif


#ifndef F6_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */  
#ifdef F6

#define F6_SetDigitalInput() PIN_vSetDigitalInput(F6)
#define F6_SetDigitalOutput() PIN_vSetDigitalOutput(F6)
#define F6_SetHigh()   PIN_vSetHigh(F6)
#define F6_SetLow()    PIN_vSetLow(F6)
#define F6_GetValue()  PIN_vGetValue(F6)

#ifdef F6_START_HIGH
#define F6_SetStart()  F6_SetHigh()
#else
#define F6_SetStart()  
#endif

#ifdef F6_OPEN_DRAIN
#define F6_SetOpenDrain()  PIN_vSetOpenDrain(F6)
#else
#define F6_SetOpenDrain()  
#endif

#else
#define F6_NotImplemented
#define F6_SetDigitalInput() 
#define F6_SetDigitalOutput() 
#define F6_SetHigh()
#define F6_SetLow()
#define F6_GetValue() 0
#define F6_SetStart()
#define F6_SetOpenDrain()  

#endif
#else
#define F6_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#define F6_SetOpenDrain()/* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif


#ifndef F7_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */  
#ifdef F7

#define F7_SetDigitalInput() PIN_vSetDigitalInput(F7)
#define F7_SetDigitalOutput() PIN_vSetDigitalOutput(F7)
#define F7_SetHigh()   PIN_vSetHigh(F7)
#define F7_SetLow()    PIN_vSetLow(F7)
#define F7_GetValue()  PIN_vGetValue(F7)

#ifdef F7_START_HIGH
#define F7_SetStart()  F7_SetHigh()
#else
#define F7_SetStart()  
#endif

#ifdef F7_OPEN_DRAIN
#define F7_SetOpenDrain()  PIN_vSetOpenDrain(F7)
#else
#define F7_SetOpenDrain()  
#endif

#else
#define F7_NotImplemented
#define F7_SetDigitalInput() 
#define F7_SetDigitalOutput() 
#define F7_SetHigh()
#define F7_SetLow()
#define F7_GetValue() 0
#define F7_SetStart()
#define F7_SetOpenDrain()  

#endif
#else
#define F7_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#define F7_SetOpenDrain()/* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif


#ifndef F8_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */  
#ifdef F8

#define F8_SetDigitalInput() PIN_vSetDigitalInput(F8)
#define F8_SetDigitalOutput() PIN_vSetDigitalOutput(F8)
#define F8_SetHigh()   PIN_vSetHigh(F8)
#define F8_SetLow()    PIN_vSetLow(F8)
#define F8_GetValue()  PIN_vGetValue(F8)

#ifdef F8_START_HIGH
#define F8_SetStart()  F8_SetHigh()
#else
#define F8_SetStart()  
#endif

#ifdef F8_OPEN_DRAIN
#define F8_SetOpenDrain()  PIN_vSetOpenDrain(F8)
#else
#define F8_SetOpenDrain()  
#endif

#else
#define F8_NotImplemented
#define F8_SetDigitalInput() 
#define F8_SetDigitalOutput() 
#define F8_SetHigh()
#define F8_SetLow()
#define F8_GetValue() 0
#define F8_SetStart()
#define F8_SetOpenDrain()  

#endif
#else
#define F8_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#define F8_SetOpenDrain()/* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif


#ifndef F9_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */  
#ifdef F9

#define F9_SetDigitalInput() PIN_vSetDigitalInput(F9)
#define F9_SetDigitalOutput() PIN_vSetDigitalOutput(F9)
#define F9_SetHigh()   PIN_vSetHigh(F9)
#define F9_SetLow()    PIN_vSetLow(F9)
#define F9_GetValue()  PIN_vGetValue(F9)

#ifdef F9_START_HIGH
#define F9_SetStart()  F9_SetHigh()
#else
#define F9_SetStart()  
#endif

#ifdef F9_OPEN_DRAIN
#define F9_SetOpenDrain()  PIN_vSetOpenDrain(F9)
#else
#define F9_SetOpenDrain()  
#endif

#else
#define F9_NotImplemented
#define F9_SetDigitalInput() 
#define F9_SetDigitalOutput() 
#define F9_SetHigh()
#define F9_SetLow()
#define F9_GetValue() 0
#define F9_SetStart()
#define F9_SetOpenDrain()  

#endif
#else
#define F9_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#define F9_SetOpenDrain()/* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif


#ifndef F10_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */  
#ifdef F10

#define F10_SetDigitalInput() PIN_vSetDigitalInput(F10)
#define F10_SetDigitalOutput() PIN_vSetDigitalOutput(F10)
#define F10_SetHigh()   PIN_vSetHigh(F10)
#define F10_SetLow()    PIN_vSetLow(F10)
#define F10_GetValue()  PIN_vGetValue(F10)

#ifdef F10_START_HIGH
#define F10_SetStart()  F10_SetHigh()
#else
#define F10_SetStart()  
#endif

#ifdef F10_OPEN_DRAIN
#define F10_SetOpenDrain()  PIN_vSetOpenDrain(F10)
#else
#define F10_SetOpenDrain()  
#endif

#else
#define F10_NotImplemented
#define F10_SetDigitalInput() 
#define F10_SetDigitalOutput() 
#define F10_SetHigh()
#define F10_SetLow()
#define F10_GetValue() 0
#define F10_SetStart()
#define F10_SetOpenDrain()  

#endif
#else
#define F10_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#define F10_SetOpenDrain()/* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif


#ifndef F11_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */  
#ifdef F11

#define F11_SetDigitalInput() PIN_vSetDigitalInput(F11)
#define F11_SetDigitalOutput() PIN_vSetDigitalOutput(F11)
#define F11_SetHigh()   PIN_vSetHigh(F11)
#define F11_SetLow()    PIN_vSetLow(F11)
#define F11_GetValue()  PIN_vGetValue(F11)

#ifdef F11_START_HIGH
#define F11_SetStart()  F11_SetHigh()
#else
#define F11_SetStart()  
#endif

#ifdef F11_OPEN_DRAIN
#define F11_SetOpenDrain()  PIN_vSetOpenDrain(F11)
#else
#define F11_SetOpenDrain()  
#endif

#else
#define F11_NotImplemented
#define F11_SetDigitalInput() 
#define F11_SetDigitalOutput() 
#define F11_SetHigh()
#define F11_SetLow()
#define F11_GetValue() 0
#define F11_SetStart()
#define F11_SetOpenDrain()  

#endif
#else
#define F11_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#define F11_SetOpenDrain()/* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif


#ifndef F12_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */  
#ifdef F12

#define F12_SetDigitalInput() PIN_vSetDigitalInput(F12)
#define F12_SetDigitalOutput() PIN_vSetDigitalOutput(F12)
#define F12_SetHigh()   PIN_vSetHigh(F12)
#define F12_SetLow()    PIN_vSetLow(F12)
#define F12_GetValue()  PIN_vGetValue(F12)

#ifdef F12_START_HIGH
#define F12_SetStart()  F12_SetHigh()
#else
#define F12_SetStart()  
#endif

#ifdef F12_OPEN_DRAIN
#define F12_SetOpenDrain()  PIN_vSetOpenDrain(F12)
#else
#define F12_SetOpenDrain()  
#endif

#else
#define F12_NotImplemented
#define F12_SetDigitalInput() 
#define F12_SetDigitalOutput() 
#define F12_SetHigh()
#define F12_SetLow()
#define F12_GetValue() 0
#define F12_SetStart()
#define F12_SetOpenDrain()  

#endif
#else
#define F12_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#define F12_SetOpenDrain()/* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif


#ifndef F13_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */  
#ifdef F13

#define F13_SetDigitalInput() PIN_vSetDigitalInput(F13)
#define F13_SetDigitalOutput() PIN_vSetDigitalOutput(F13)
#define F13_SetHigh()   PIN_vSetHigh(F13)
#define F13_SetLow()    PIN_vSetLow(F13)
#define F13_GetValue()  PIN_vGetValue(F13)

#ifdef F13_START_HIGH
#define F13_SetStart()  F13_SetHigh()
#else
#define F13_SetStart()  
#endif

#ifdef F13_OPEN_DRAIN
#define F13_SetOpenDrain()  PIN_vSetOpenDrain(F13)
#else
#define F13_SetOpenDrain()  
#endif

#else
#define F13_NotImplemented
#define F13_SetDigitalInput() 
#define F13_SetDigitalOutput() 
#define F13_SetHigh()
#define F13_SetLow()
#define F13_GetValue() 0
#define F13_SetStart()
#define F13_SetOpenDrain()  

#endif
#else
#define F13_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#define F13_SetOpenDrain()/* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif


#ifndef F14_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */  
#ifdef F14

#define F14_SetDigitalInput() PIN_vSetDigitalInput(F14)
#define F14_SetDigitalOutput() PIN_vSetDigitalOutput(F14)
#define F14_SetHigh()   PIN_vSetHigh(F14)
#define F14_SetLow()    PIN_vSetLow(F14)
#define F14_GetValue()  PIN_vGetValue(F14)

#ifdef F14_START_HIGH
#define F14_SetStart()  F14_SetHigh()
#else
#define F14_SetStart()  
#endif

#ifdef F14_OPEN_DRAIN
#define F14_SetOpenDrain()  PIN_vSetOpenDrain(F14)
#else
#define F14_SetOpenDrain()  
#endif

#else
#define F14_NotImplemented
#define F14_SetDigitalInput() 
#define F14_SetDigitalOutput() 
#define F14_SetHigh()
#define F14_SetLow()
#define F14_GetValue() 0
#define F14_SetStart()
#define F14_SetOpenDrain()  

#endif
#else
#define F14_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#define F14_SetOpenDrain()/* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif


#ifndef F15_SetHigh     /* Use From Pin Manager If Request Use MCC_PIN */  
#ifdef F15

#define F15_SetDigitalInput() PIN_vSetDigitalInput(F15)
#define F15_SetDigitalOutput() PIN_vSetDigitalOutput(F15)
#define F15_SetHigh()   PIN_vSetHigh(F15)
#define F15_SetLow()    PIN_vSetLow(F15)
#define F15_GetValue()  PIN_vGetValue(F15)

#ifdef F15_START_HIGH
#define F15_SetStart()  F15_SetHigh()
#else
#define F15_SetStart()  
#endif

#ifdef F15_OPEN_DRAIN
#define F15_SetOpenDrain()  PIN_vSetOpenDrain(F15)
#else
#define F15_SetOpenDrain()  
#endif

#else
#define F15_NotImplemented
#define F15_SetDigitalInput() 
#define F15_SetDigitalOutput() 
#define F15_SetHigh()
#define F15_SetLow()
#define F15_GetValue() 0
#define F15_SetStart()
#define F15_SetOpenDrain()  

#endif
#else
#define F15_SetStart()    /* Not Used But Give Something To Compiler Not To Raise Error */ 
#define F15_SetOpenDrain()/* Not Used But Give Something To Compiler Not To Raise Error */ 
#endif




/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 


/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */


#endif	/* PIN_MACRO_H */
