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
#define ADC1            A0
#define ADC2            A1
#define ADC3            B0
#define ADC4            B1
#define ADC5            B2
#define ADC6            B3
#define ADC7            C0
#define ADC8            C1
#define ADC9            C2
#define ADC10           A11
#define ADC11           A12
#define ADC12           C11
#define ADC13           E12
#define ADC14           E13
#define ADC15           E14
#define ADC16           E15
#define ADC17           G9
#define ADC18           G6
#define ADC19           E9
#define ADC20           E8
#define VREF            A7        
#endif

/* *****************************************************************************
 * Digital
 **************************************************************************** */
#define DO_DIR1         G2
#define DO_DIR2         G3
#define DO_DIR3         G10
#define DO_DIR4         G11
#define DO_DIR5         E0
#define DO_DIR6         E1
#define DO_DIR7         D12
#define DO_DIR8         C15


#define DI_DIR1         F0
#define DI_DIR2         F1
#define DI_DIR3         F4
#define DI_DIR4         F5
#define DI_DIR5         F6
#define DI_DIR6         F7
#define DI_DIR7         F12
#define DI_DIR8         F13


#define SPI1_CS1        D14
#define SPI1_CS2        D15

#define SPI2_CS1        C8
#define SPI2_CS2        D13
#define SPI2_CS3        F9

#define SCK1            C3
#define SDO1            A4
#define SDI1            A9

#define SCL1            C5


#define U3RTS           B8
#define U3CTS           C10


//#define DAC_LOAD        SPI4_CS3      now controlled only from fpga
//#define DAC_CLEAR       SPI4_CS4      not implemented even in i2c dac module




#define PCB_NET_EN1         B9           
#define PCB_NET_EN2         C6          
#define PCB_NET_EN3         C7               


#define PCB_NET_FLT1        A14             
#define PCB_NET_FLT2        A15           
#define PCB_NET_FLT3        D8           

/* *****************************************************************************
 * Signal Names - default is digital input otherwise point below
 **************************************************************************** */

/* *****************************************************************************
 * Outputs
 **************************************************************************** */
//#define DO_EN       

//#define DO_EN_START_HIGH


#define DO1         DO_DIR1
#define DO2         DO_DIR2
#define DO3         DO_DIR3
#define DO4         DO_DIR4
#define DO5         DO_DIR5
#define DO6         DO_DIR6
#define DO7         DO_DIR7
#define DO8         DO_DIR8

//#define DO1_START_HIGH
//#define DO2_START_HIGH
//#define DO3_START_HIGH
//#define DO4_START_HIGH
//#define DO5_START_HIGH
//#define DO6_START_HIGH
//#define DO7_START_HIGH
//#define DO8_START_HIGH


#define EN1         PCB_NET_FLT1           
#define EN2         PCB_NET_FLT2         
#define EN3         PCB_NET_FLT3               
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


#define CS1_SPI1        SPI1_CS1
#define CS1_SPI2        SPI2_CS1
//#define CS1_SPI3       
//#define CS1_SPI4

#define CS1_SPI1_START_HIGH
#define CS1_SPI2_START_HIGH
#define CS1_SPI3_START_HIGH
#define CS1_SPI4_START_HIGH


#define CS2_SPI1        SPI1_CS2
#define CS2_SPI2        SPI2_CS2
//#define CS2_SPI3
//#define CS2_SPI4

#define CS2_SPI1_START_HIGH
#define CS2_SPI2_START_HIGH
#define CS2_SPI3_START_HIGH
#define CS2_SPI4_START_HIGH


//#define CS3_SPI1
#define CS3_SPI2        SPI2_CS3      
//#define CS3_SPI3
//#define CS3_SPI4

#define CS3_SPI1_START_HIGH
#define CS3_SPI2_START_HIGH
#define CS3_SPI3_START_HIGH
#define CS3_SPI4_START_HIGH



/* For Now in I2C DAC */
#define LOAD_DAC        SPI2_CS3 
#define CLEAR_DAC       DAC_CLEAR 

//#define LOAD_DAC_START_HIGH
//#define CLEAR_DAC_START_HIGH




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
#define FPGA_RST    EN2            
#define FPGA_RST_START_HIGH
#define FPGA_RST_OPEN_DRAIN
#else
//#define FPGA_RST    F10             /* MCC Net Name (Not Pin F10) */         
//#define FPGA_RST_FROM_OTHER_MCC_NAME
#endif


#define DBG1        U3CTS               /* Closer to Board Outline (MCLR-Like on connector) */
#define DBG2        U3RTS               /* Closer to Battery Side (Pin-6 Like on connector) */
//#define DBG3
//#define DBG4


/* *****************************************************************************
 * Inputs
 **************************************************************************** */

#define DI1         DI_DIR1                   
#define DI2         DI_DIR2                   
#define DI3         DI_DIR3                   
#define DI4         DI_DIR4                   
#define DI5         DI_DIR5                   
#define DI6         DI_DIR6                   
#define DI7         DI_DIR7                   
#define DI8         DI_DIR8 

#define FLT1        PCB_NET_EN1          
#define FLT2        PCB_NET_EN2          
#define FLT3        PCB_NET_EN3           
//#define FLT4                   
//#define FLT5                   
//#define FLT6                   
//#define FLT7                   
//#define FLT8 

//#define FLIR_V_SYNC                
                 
                 
                 
                 
/* *****************************************************************************
 * Analog
 **************************************************************************** */
#ifndef BOARDCFG_USE_MCC_PIN
#define IFBK_S1L1   ADC1                              
#define IFBK_S1L2   ADC2                              
#define IFBK_S1L3   ADC3

#define IFBK_S2L1   ADC4
#define IFBK_S2L2   ADC5
#define IFBK_S2L3   ADC6

//#define IFBK_S3L1   
//#define IFBK_S3L2   
//#define IFBK_S3L3                              

#define U_DC_LINK   ADC13
#define UFBK_2      ADC20

#define IFBK_DRV1   ADC7                              
#define IFBK_DRV2   ADC8                           
#define IFBK_DRV3   ADC9                             

#define IFBK_DRV4   ADC10                              
#define IFBK_DRV5   ADC11                           
#define IFBK_DRV6   ADC12                             

#define TEMP_FBK1   ADC14                         
#define TEMP_FBK2   ADC15                        
#define TEMP_FBK3   ADC16                        

#define TEMP_FBK4   ADC17                         
#define TEMP_FBK5   ADC18                        
#define TEMP_FBK6   ADC19                        

//#define UFBK_3      
//#define UFBK_4      

//#define UFBK_5      
#endif
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




/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 


/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */


#endif	/* PIN_CFG_H */
