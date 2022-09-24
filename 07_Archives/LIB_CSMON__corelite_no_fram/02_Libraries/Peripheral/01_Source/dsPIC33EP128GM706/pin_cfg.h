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
#define UFBK1_RA0       A0
#define UFBK2_RA1       A1
#define UFBK3_RB0       B0
#define UFBK12_RC0      C0
#define UFBK23_RC1      C1
#define FBK_24V_RC11    C11   
#define FBK_D5V_RG8     G8
#define FBK_5V_RG7      G7
#define FBK_M5V_RG6     G6
#define THR_FBK1_RA11   A11
#define THR_FBK2_RA4    A4
#define THR_FBK_RC2     C2
#define FBK_13V_RG9     G9
#define THR_FBKSW_RA12  A12
#endif

/* *****************************************************************************
 * Digital
 **************************************************************************** */
#define OUT_CTR1        B4
#define OUT_CTR2        B5
#define OUT_CTR3        B6
#define OUT_CTR4        B7
#define OUT_CTR5        B8
#define OUT_CTR6        B9




/* *****************************************************************************
 * Signal Names - default is digital input otherwise point below
 **************************************************************************** */

/* *****************************************************************************
 * Outputs
 **************************************************************************** */
//#define DO_EN       

//#define DO_EN_START_HIGH

#ifndef BOARDCFG_USE_MCC_PIN
#define DO1         OUT_CTR1
#define DO2         OUT_CTR2
#define DO3         OUT_CTR3
#define DO4         OUT_CTR4
#define DO5         OUT_CTR5
#define DO6         OUT_CTR6
//#define DO7         
//#define DO8    
#else
#define DO1         OUT_CTR1_RB3_RB4
#define DO2         OUT_CTR2_RB4_RB5
#define DO3         OUT_CTR3_RB5_RB6
#define DO4         OUT_CTR4_RB6_RB7
#define DO5         OUT_CTR5_RB7_RB8
#define DO6         OUT_CTR6_RB8_RB9
#define DO123456_FROM_OTHER_MCC_NAME
//#define DO7         
//#define DO8    
#endif

//#define DO1_START_HIGH
//#define DO2_START_HIGH
//#define DO3_START_HIGH
//#define DO4_START_HIGH
//#define DO5_START_HIGH
//#define DO6_START_HIGH
//#define DO7_START_HIGH
//#define DO8_START_HIGH


//#define EN1                   
//#define EN2                   
//#define EN3                   
//#define EN4                   
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


//#define CS1_SPI1
//#define CS1_SPI2
//#define CS1_SPI3
//#define CS1_SPI4

#define CS1_SPI1_START_HIGH
#define CS1_SPI2_START_HIGH
#define CS1_SPI3_START_HIGH
#define CS1_SPI4_START_HIGH


//#define CS2_SPI1
//#define CS2_SPI2
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


//#define TXEN_U1
//#define TXEN_U2
//#define TXEN_U3
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


//#define FLIR_RST                  
#define FLIR_RST_START_HIGH
#define FLIR_RST_OPEN_DRAIN

#if MCC_PIN_FORCE_PRIORITY_INCLUDE == 0
//#define FPGA_RST    FUNC_10            
#define FPGA_RST_START_HIGH
#define FPGA_RST_OPEN_DRAIN
#else
//#define FPGA_RST    F10             /* MCC Net Name (Not Pin F10) */         
//#define FPGA_RST_FROM_OTHER_MCC_NAME
#endif


/* *****************************************************************************
 * Inputs
 **************************************************************************** */

//#define DI1                   
//#define DI2                   
//#define DI3                   
//#define DI4                   
//#define DI5                   
//#define DI6                   
//#define DI7                   
//#define DI8 

//#define FLT1                   
//#define FLT2                   
//#define FLT3                   
//#define FLT4                   
//#define FLT5                   
//#define FLT6                   
//#define FLT7                   
//#define FLT8 

//#define FLIR_V_SYNC                
                 
                 
                 
                 
/* *****************************************************************************
 * Analog
 **************************************************************************** */
                 
#define IFBK_S1L1   UFBK1_RA0                              
#define IFBK_S1L2   UFBK2_RA1                              
#define IFBK_S1L3   UFBK3_RB0                              

#define IFBK_S2L1   UFBK12_RC0                              
#define IFBK_S2L2   UFBK23_RC1
#define IFBK_S2L3   UFBK12_RC0                             

//#define IFBK_S3L1                                 
//#define IFBK_S3L2                              
//#define IFBK_S3L3                                

#define U_DC_LINK   FBK_24V_RC11
#define UFBK_2      FBK_24V_RC11

#define IFBK_DRV1   FBK_D5V_RG8                              
#define IFBK_DRV2   FBK_5V_RG7                           
#define IFBK_DRV3   FBK_M5V_RG6                             

#define TEMP_FBK1   THR_FBK1_RA11                         
#define TEMP_FBK2   THR_FBK2_RA4                        
#define TEMP_FBK3   THR_FBK_RC2                        

#define UFBK_3      FBK_13V_RG9
#define UFBK_4      THR_FBKSW_RA12

//#define UFBK_5

//#define ADC_REF     

/* *****************************************************************************
 * Function (IO) - Similar Like Output
 **************************************************************************** */


//#define F0                
//#define F1                  
//#define F2                  
//#define F3                  
//#define F4                
//#define F5                 
//#define F6                   
//#define F7                   
//#define F8                
//#define F9               
//#define F10                 
//#define F11            
//#define F12               
//#define F13           
//#define F14              
//#define F15              
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

#ifdef DO123456_FROM_OTHER_MCC_NAME

#define DO1_SetDigitalOutput() MCC_vSetDigitalOutput(DO1)
#define DO1_SetHigh()          MCC_vSetHigh(DO1)
#define DO1_SetLow()           MCC_vSetLow(DO1)
#define DO1_GetValue()         MCC_vGetValue(DO1)
#define DO1_SetStart()         /* Just Dummy for no compile errors  ...  */
#define DO1_SetOpenDrain()     /* ...Initialization Function Is From MCC */

#define DO2_SetDigitalOutput() MCC_vSetDigitalOutput(DO2)
#define DO2_SetHigh()          MCC_vSetHigh(DO2)
#define DO2_SetLow()           MCC_vSetLow(DO2)
#define DO2_GetValue()         MCC_vGetValue(DO2)
#define DO2_SetStart()         /* Just Dummy for no compile errors  ...  */
#define DO2_SetOpenDrain()     /* ...Initialization Function Is From MCC */

#define DO3_SetDigitalOutput() MCC_vSetDigitalOutput(DO3)
#define DO3_SetHigh()          MCC_vSetHigh(DO3)
#define DO3_SetLow()           MCC_vSetLow(DO3)
#define DO3_GetValue()         MCC_vGetValue(DO3)
#define DO3_SetStart()         /* Just Dummy for no compile errors  ...  */
#define DO3_SetOpenDrain()     /* ...Initialization Function Is From MCC */

#define DO4_SetDigitalOutput() MCC_vSetDigitalOutput(DO4)
#define DO4_SetHigh()          MCC_vSetHigh(DO4)
#define DO4_SetLow()           MCC_vSetLow(DO4)
#define DO4_GetValue()         MCC_vGetValue(DO4)
#define DO4_SetStart()         /* Just Dummy for no compile errors  ...  */
#define DO4_SetOpenDrain()     /* ...Initialization Function Is From MCC */

#define DO5_SetDigitalOutput() MCC_vSetDigitalOutput(DO5)
#define DO5_SetHigh()          MCC_vSetHigh(DO5)
#define DO5_SetLow()           MCC_vSetLow(DO5)
#define DO5_GetValue()         MCC_vGetValue(DO5)
#define DO5_SetStart()         /* Just Dummy for no compile errors  ...  */
#define DO5_SetOpenDrain()     /* ...Initialization Function Is From MCC */

#define DO6_SetDigitalOutput() MCC_vSetDigitalOutput(DO6)
#define DO6_SetHigh()          MCC_vSetHigh(DO6)
#define DO6_SetLow()           MCC_vSetLow(DO6)
#define DO6_GetValue()         MCC_vGetValue(DO6)
#define DO6_SetStart()         /* Just Dummy for no compile errors  ...  */
#define DO6_SetOpenDrain()     /* ...Initialization Function Is From MCC */

#endif

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 


/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */


#endif	/* PIN_CFG_H */
