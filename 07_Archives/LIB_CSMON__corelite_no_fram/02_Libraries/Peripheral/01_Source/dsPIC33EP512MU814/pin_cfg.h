/* *****************************************************************************
 * File:   pin_cfg.h
 * Author: Dimitar Lilov
 *
 * Created on 2020 04 15 11:55
 * 
 * Description: Pin Configuration
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef PIN_CFG_H
#define	PIN_CFG_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <xc.h>

#include "mcc_cfg.h"
#ifdef BOARDCFG_USE_MCC
#ifndef BOARDCFG_USE_MCC_PIN
#define _PIN_MANAGER_H          /* Force exclude from use pin_manager.h  */
#endif
#include "mcc.h"
#endif




/* *****************************************************************************
 * String Concatenation For Macro
 **************************************************************************** */
#ifndef STRING_CONCAT
#define STRING_CONCAT_BASE(_a_, _b_) _a_##_b_
#define STRING_CONCAT(_a_, _b_) STRING_CONCAT_BASE(_a_, _b_)
#endif

/* *****************************************************************************
 * Definitions General Functions Prototypes - From PORT and PIN
 **************************************************************************** */
#define PIN_vSetDigitalInput(x)  { STRING_CONCAT(_TRIS,x)   = 1; }
#define PIN_vSetDigitalOutput(x) { STRING_CONCAT(_TRIS,x)   = 0; }
#define PIN_vSetAnalog(x)        { STRING_CONCAT(_ANS,x)  = 1; }
#define PIN_vSetDigital(x)       { STRING_CONCAT(_ANS,x)  = 0; }
#define PIN_vSetHigh(x)          { STRING_CONCAT(_LAT,x)    = 1; }
#define PIN_vSetLow(x)           { STRING_CONCAT(_LAT,x)    = 0; }
#define PIN_vGetValue(x)           STRING_CONCAT(_R,x)
#define PIN_vSetOpenDrain(x)     { STRING_CONCAT(_ODC,x)    = 1; }
#define PIN_vClrOpenDrain(x)     { STRING_CONCAT(_ODC,x)    = 0; }

/* *****************************************************************************
 * Definitions General Functions Prototypes - From MCC net NAME
 **************************************************************************** */
#define MCC_vSetDigitalInput(x)  { STRING_CONCAT(x,_SetDigitalInput)();}
#define MCC_vSetDigitalOutput(x) { STRING_CONCAT(x,_SetDigitalOutput)();}
#define MCC_vSetHigh(x)          { STRING_CONCAT(x,_SetHigh)();}
#define MCC_vSetLow(x)           { STRING_CONCAT(x,_SetLow)();}
#define MCC_vGetValue(x)         { STRING_CONCAT(x,_GetValue)();}


/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define MCC_PIN_FORCE_PRIORITY_INCLUDE 0


/* *****************************************************************************
 * Configuration Fix
 **************************************************************************** */
#if MCC_PIN_FORCE_PRIORITY_INCLUDE
#undef _PIN_MANAGER_H
#include "pin_manager.h"
#endif


/* *****************************************************************************
 * Constants Definitions PIN - PCB Net Names
 **************************************************************************** */

/* *****************************************************************************
 * Analog
 **************************************************************************** */
#ifndef BOARDCFG_USE_MCC_PIN
#define ANC1    B0
#define ANC2    B1
#define ANC3    B2
#define ANC4    B3
#define ANC5    B4
#define ANC6    B5
#define ANC7    B6
#define ANC8    B7
#define ANC9    B8
#define ANC10   B9
#define ANC11   B10
#define ANC12   B11
#define ANC13   B12
#define ANC14   B13
#define ANC15   B14
#define ANC16   B15
#endif

/* *****************************************************************************
 * Digital
 **************************************************************************** */

#define FUNC_0  A0
#define FUNC_1  A1
#define FUNC_2  A4
#define FUNC_3  A6
#define FUNC_4  D4
#define FUNC_5  D5
#define FUNC_6  F2
#define FUNC_7  F3
#define FUNC_8  F4    /* Used For CS2_EEPROM */
#define FUNC_9  F5    /* Used For CS2_RAM */
#define FUNC_10 K12   /* PMP Ussue ??? always high */
#define FUNC_11 K13   /* PMP Ussue ??? always high */
#define FUNC_12 J15   /* PMP Ussue ??? always high */
#define FUNC_13 C13   /* Used For PGED */
#define FUNC_14 C14   /* Used For PGEC */

/* 
 * PMCS1/PMA14 and PMCS2/PMA15 pin
functionality is duplicated on the RJ14/15 pins in addition to
the expected RK11/RK12 pins
 * 
 * 
 *  */



/* *****************************************************************************
 * Signal Names - default is digital input otherwise point below
 **************************************************************************** */

/* *****************************************************************************
 * Outputs
 **************************************************************************** */
#define DO_EN       A9

//#define DO_EN_START_HIGH

#define DO1         J2
#define DO2         J3
#define DO3         J4
#define DO4         J5
#define DO5         J6
#define DO6         J7
#define DO7         J10
#define DO8         J11

//#define DO1_START_HIGH
//#define DO2_START_HIGH
//#define DO3_START_HIGH
//#define DO4_START_HIGH
//#define DO5_START_HIGH
//#define DO6_START_HIGH
//#define DO7_START_HIGH
//#define DO8_START_HIGH


#define EN1             D0                  
#define EN2             D1  
#define EN3             D2      
#define EN4             D3      
//#define EN5                   
//#define EN6                   
//#define EN7                   
//#define EN8                   

//#define EN1_START_HIGH                   
//#define EN2_START_HIGH                   
//#define EN3_START_HIGH                   
//#define EN4_START_HIGH                   
//#define EN5_START_HIGH                   
//#define EN6_START_HIGH                   
//#define EN7_START_HIGH                   
//#define EN8_START_HIGH                   


#define CS1_SPI1            J13     /* DAC */
#define CS1_SPI2            FUNC_8  /* EEPROM */
//#define CS1_SPI3
//#define CS1_SPI4

#define CS1_SPI1_START_HIGH
#define CS1_SPI2_START_HIGH
#define CS1_SPI3_START_HIGH
#define CS1_SPI4_START_HIGH


//#define CS2_SPI1            
#define CS2_SPI2            FUNC_9  /* RAM */
//#define CS2_SPI3
//#define CS2_SPI4

#define CS2_SPI1_START_HIGH
#define CS2_SPI2_START_HIGH
#define CS2_SPI3_START_HIGH
#define CS2_SPI4_START_HIGH


//#define CS3_SPI1
//#define CS3_SPI2          
//#define CS3_SPI3
//#define CS3_SPI4

#define CS3_SPI1_START_HIGH
#define CS3_SPI2_START_HIGH
#define CS3_SPI3_START_HIGH
#define CS3_SPI4_START_HIGH


#define TXEN_U1             K0 
#define TXEN_U2             K1
#define TXEN_U3             A5
//#define TXEN_U4

//#define TXEN_U1_START_HIGH        /* Use this As TXEN Polarity (if defined Transmiter Enable Low Level ) */
//#define TXEN_U2_START_HIGH        /* Use this As TXEN Polarity (if defined Transmiter Enable Low Level ) */
//#define TXEN_U3_START_HIGH        /* Use this As TXEN Polarity (if defined Transmiter Enable Low Level ) */
//#define TXEN_U4_START_HIGH        /* Use this As TXEN Polarity (if defined Transmiter Enable Low Level ) */


//#define RXEN_U1             
//#define RXEN_U2
//#define RXEN_U3
//#define RXEN_U4

//#define RXEN_U1_START_HIGH        /* Use this As RXEN Polarity (if defined Receiver Enable High Level ) */
//#define RXEN_U2_START_HIGH        /* Use this As RXEN Polarity (if defined Receiver Enable High Level ) */
//#define RXEN_U3_START_HIGH        /* Use this As RXEN Polarity (if defined Receiver Enable High Level ) */
//#define RXEN_U4_START_HIGH        /* Use this As RXEN Polarity (if defined Receiver Enable High Level ) */


//#define FLIR_RST    H4                
#define FLIR_RST_START_HIGH
#define FLIR_RST_OPEN_DRAIN

#if MCC_PIN_FORCE_PRIORITY_INCLUDE == 0
#define FPGA_RST    FUNC_10            
#define FPGA_RST_START_HIGH
#define FPGA_RST_OPEN_DRAIN
#else
#define FPGA_RST    F10             /* MCC Net Name (Not Pin F10) */         
#define FPGA_RST_FROM_OTHER_MCC_NAME
#endif


/* *****************************************************************************
 * Inputs
 **************************************************************************** */

#define DI1             H8                   
#define DI2             H9                   
#define DI3             H10                   
#define DI4             H11                   
#define DI5             H12                   
#define DI6             H13                   
#define DI7             H14                   
#define DI8             H15 

#define FLT1            D12     /* FPGA TRIP Error */
#define FLT2            D13     /* FPGA PWM Error */
#define FLT3            D14       
#define FLT4            D15       
//#define FLT5                   
//#define FLT6                   
//#define FLT7                   
//#define FLT8 

//#define FLIR_V_SYNC    H3                

               
                 
                 
/* *****************************************************************************
 * Analog
 **************************************************************************** */
#ifndef BOARDCFG_USE_MCC_PIN                 
#define IFBK_S1L1   ANC1                              
#define IFBK_S1L2   ANC2                              
#define IFBK_S1L3   ANC3                              

#define IFBK_S2L1   ANC4                              
#define IFBK_S2L2   ANC5                           
#define IFBK_S2L3   ANC6                             

//#define IFBK_S3L1                                 
//#define IFBK_S3L2                              
//#define IFBK_S3L3                                

#define U_DC_LINK   ANC7
#define UFBK_2      ANC8

#define IFBK_DRV1   ANC11                              
#define IFBK_DRV2   ANC12                           
#define IFBK_DRV3   ANC13                             

#define TEMP_FBK1   ANC14                         
#define TEMP_FBK2   ANC15                        
#define TEMP_FBK3   ANC16                        

#define UFBK_3      ANC9
#define UFBK_4      ANC10

//#define UFBK_5

#define ADC_REF     A10
#endif

/* *****************************************************************************
 * Function (IO) - Similar Like Output
 **************************************************************************** */


#define F0      FUNC_0             
#define F1      FUNC_1             
#define F2      FUNC_2             
#define F3      FUNC_3             
#define F4      FUNC_4             
#define F5      FUNC_5             
#define F6      FUNC_6             
#define F7      FUNC_7             
#define F8      FUNC_8             
#define F9      FUNC_9             
#define F10     FUNC_10            
#define F11     FUNC_11            
#define F12     FUNC_12            
#define F13     FUNC_13            
#define F14     FUNC_14            
//#define F15     FUNC_15            
/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

#ifdef FPGA_RST_FROM_OTHER_MCC_NAME /* Use From Pin Manager If Request Use MCC_PIN */  
#define FPGA_RST_SetDigitalOutput() MCC_vSetDigitalOutput(FPGA_RST)
#define FPGA_RST_SetHigh()          MCC_vSetHigh(FPGA_RST)
#define FPGA_RST_SetLow()           MCC_vSetLow(FPGA_RST)
#define FPGA_RST_GetValue()         MCC_vGetValue(FPGA_RST)
#define FPGA_RST_SetStart()         /* Just Dummy for no compile errors  ...  */
#define FPGA_RST_SetOpenDrain()     /* ...Initialization Function Is From MCC */
#endif




/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 


/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */


#endif	/* PIN_CFG_H */
