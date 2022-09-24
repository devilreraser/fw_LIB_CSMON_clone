/* *****************************************************************************
 * File:   pin.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 04 04 16:45
 * 
 * Description: Pin Initialization
 * 
 **************************************************************************** */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <xc.h>
#include <stdbool.h>
#include "pin.h"
#include "boardcfg.h"
#include "sys_drv.h"


/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */


/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
#if defined __dsPIC33EP512MU814__
typedef enum
{
    PORT_A,
    PORT_B,
    PORT_C,
    PORT_D,
    PORT_E,
    PORT_F,
    PORT_G,
    PORT_H,
    PORT_J,
    PORT_K,
    PORT_COUNT
}PORT_eModule_t;
#else
typedef enum
{
    PORT_A,
    PORT_B,
    PORT_C,
    PORT_D,
    PORT_E,
    PORT_F,
    PORT_G,
    PORT_COUNT
}PORT_eModule_t;
#endif
typedef struct
{
    union 
    {
        struct 
        {
            uint16_t TRIS;
            uint16_t PORT;
            uint16_t LAT;
            uint16_t ODC;
            uint16_t CNEN;
            uint16_t CNPU;
            uint16_t CNPD;
            uint16_t ANSEL;
        };
        
        struct
        {
            TRISABITS       TRISbits;
            PORTABITS       PORTbits;
            LATABITS        LATbits;
            ODCABITS        ODCbits;
            CNENABITS       CNENbits;
            CNPUABITS       CNPUbits;
            CNPDABITS       CNPDbits;
            ANSELABITS      ANSELbits;
        };
    };
}PORT_sBaseRegisters_t;

typedef struct
{
    PORT_sBaseRegisters_t* psBase[PORT_COUNT];
    uint8_t u8BitOffsetsISR[PORT_COUNT];    //not implemented yet
}PORT_sHandle_t;


PORT_sHandle_t PORT_sHandle = 
{
    {
        (PORT_sBaseRegisters_t*)(&TRISA),
        (PORT_sBaseRegisters_t*)(&TRISB),
        (PORT_sBaseRegisters_t*)(&TRISC),
        (PORT_sBaseRegisters_t*)(&TRISD),
        (PORT_sBaseRegisters_t*)(&TRISE),
        (PORT_sBaseRegisters_t*)(&TRISF),
        (PORT_sBaseRegisters_t*)(&TRISG),
        #if defined __dsPIC33EP512MU814__
        (PORT_sBaseRegisters_t*)(&TRISH),
        (PORT_sBaseRegisters_t*)(&TRISJ),
        (PORT_sBaseRegisters_t*)(&TRISK),
        #endif
    },
    {   /* Isr */                   
    },
};




/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */


/* *****************************************************************************
 * Functions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */

void PIN_vInit(void)
{
    /* Default Initialization as Digital Inputs */
    uint16_t u16Index;
    PORT_sBaseRegisters_t* psBase;
    for (u16Index = 0; u16Index < PORT_COUNT; u16Index++)
    {
        psBase = PORT_sHandle.psBase[u16Index];
        psBase->TRIS    = 0xFFFF;
        psBase->ANSEL   = 0x0000;
    }
     
    
    
    
    /* Fix Configuration According board configuration */
    
    
    /* Outputs */
    
    /* DO */
    DO_EN_SetStart();
    DO_EN_SetDigitalOutput();

    DO1_SetStart();
    DO1_SetDigitalOutput();

    DO2_SetStart();
    DO2_SetDigitalOutput();
    
    DO3_SetStart();
    DO3_SetDigitalOutput();
    
    DO4_SetStart();
    DO4_SetDigitalOutput();
    
    DO5_SetStart();
    DO5_SetDigitalOutput();
    
    DO6_SetStart();
    DO6_SetDigitalOutput();
    
    DO7_SetStart();
    DO7_SetDigitalOutput();
    
    DO8_SetStart();
    DO8_SetDigitalOutput();
    
    /* EN */  
    EN1_SetStart();
    EN1_SetDigitalOutput();

    EN2_SetStart();
    EN2_SetDigitalOutput();
    
    EN3_SetStart();
    EN3_SetDigitalOutput();
    
    EN4_SetStart();
    EN4_SetDigitalOutput();
    
    EN5_SetStart();
    EN5_SetDigitalOutput();
    
    EN6_SetStart();
    EN6_SetDigitalOutput();
    
    EN7_SetStart();
    EN7_SetDigitalOutput();
    
    EN8_SetStart();
    EN8_SetDigitalOutput();
    
    /* CS1 */
    CS1_SPI1_SetStart();
    CS1_SPI1_SetDigitalOutput();

    CS1_SPI2_SetStart();
    CS1_SPI2_SetDigitalOutput();
    
    CS1_SPI3_SetStart();
    CS1_SPI3_SetDigitalOutput();
    
    CS1_SPI4_SetStart();
    CS1_SPI4_SetDigitalOutput();
    
    /* CS2 */
    CS2_SPI1_SetStart();
    CS2_SPI1_SetDigitalOutput();

    CS2_SPI2_SetStart();
    CS2_SPI2_SetDigitalOutput();
    
    CS2_SPI3_SetStart();
    CS2_SPI3_SetDigitalOutput();
    
    CS2_SPI4_SetStart();
    CS2_SPI4_SetDigitalOutput();
    
    /* CS3 */
    CS3_SPI1_SetStart();
    CS3_SPI1_SetDigitalOutput();

    CS3_SPI2_SetStart();
    CS3_SPI2_SetDigitalOutput();
    
    CS3_SPI3_SetStart();
    CS3_SPI3_SetDigitalOutput();
    
    CS3_SPI4_SetStart();
    CS3_SPI4_SetDigitalOutput();
    
    /* TXEN */
    TXEN_U1_SetStart();
    TXEN_U1_SetDigitalOutput();

    TXEN_U2_SetStart();
    TXEN_U2_SetDigitalOutput();
    
    TXEN_U3_SetStart();
    TXEN_U3_SetDigitalOutput();
    
    TXEN_U4_SetStart();
    TXEN_U4_SetDigitalOutput();
    
    /* RXEN */
    RXEN_U1_SetStart();
    RXEN_U1_SetDigitalOutput();

    RXEN_U2_SetStart();
    RXEN_U2_SetDigitalOutput();
    
    RXEN_U3_SetStart();
    RXEN_U3_SetDigitalOutput();
    
    RXEN_U4_SetStart();
    RXEN_U4_SetDigitalOutput();
    
    FLIR_RST_SetOpenDrain();
    FLIR_RST_SetStart();
    FLIR_RST_SetDigitalOutput();
    
    FPGA_RST_SetOpenDrain();
    FPGA_RST_SetStart();
    FPGA_RST_SetDigitalOutput();
    

      
    /* Inputs */
    DI1_SetDigitalInput();
    DI2_SetDigitalInput();
    DI3_SetDigitalInput();
    DI4_SetDigitalInput();
    DI5_SetDigitalInput();
    DI6_SetDigitalInput();
    DI7_SetDigitalInput();
    DI8_SetDigitalInput();

    FLT1_SetDigitalInput();
    FLT2_SetDigitalInput();
    FLT3_SetDigitalInput();
    FLT4_SetDigitalInput();
    FLT5_SetDigitalInput();
    FLT6_SetDigitalInput();
    FLT7_SetDigitalInput();
    FLT8_SetDigitalInput();

    FLIR_V_SYNC_SetDigitalInput();
    
    
    /* Analog */
    
    IFBK_S1L1_SetAnalog();                              
    IFBK_S1L2_SetAnalog();                              
    IFBK_S1L3_SetAnalog();                              

    IFBK_S2L1_SetAnalog();                               
    IFBK_S2L2_SetAnalog();                            
    IFBK_S2L3_SetAnalog();                              

    IFBK_S3L1_SetAnalog();                                 
    IFBK_S3L2_SetAnalog();                              
    IFBK_S3L3_SetAnalog();                                

    U_DC_LINK_SetAnalog();   
    UFBK_2_SetAnalog();      

    IFBK_DRV1_SetAnalog();                         
    IFBK_DRV2_SetAnalog();                     
    IFBK_DRV3_SetAnalog();                        

    TEMP_FBK1_SetAnalog();                      
    TEMP_FBK2_SetAnalog();                    
    TEMP_FBK3_SetAnalog();                   

    UFBK_3_SetAnalog();      
    UFBK_4_SetAnalog();  
    
    UFBK_5_SetAnalog();      

    ADC_REF_SetAnalog();
        
    #if USE_TSC_F0 > FUNC_INPUT
    F0_SetDigitalOutput();
    #endif  
    
    #if USE_TSC_F1 > FUNC_INPUT
    F1_SetDigitalOutput();
    #endif  
    
    #if USE_TSC_F2 > FUNC_INPUT
    F2_SetDigitalOutput();
    #endif  
    
    #if USE_TSC_F3 > FUNC_INPUT
    F3_SetDigitalOutput();
    #endif  
    
    #if USE_TSC_F4 > FUNC_INPUT
    F4_SetDigitalOutput();
    #endif  
    
    #if USE_TSC_F5 > FUNC_INPUT
    F5_SetDigitalOutput();
    #endif  
    
    #if USE_TSC_F6 > FUNC_INPUT
    F6_SetDigitalOutput();
    #endif  
    
    #if USE_TSC_F7 > FUNC_INPUT
    F7_SetDigitalOutput();
    #endif  
    
    #if USE_TSC_F8 > FUNC_INPUT
    F8_SetDigitalOutput();
    #endif  
    
    #if USE_TSC_F9 > FUNC_INPUT
    F9_SetDigitalOutput();
    #endif  
    
    #if USE_TSC_F10 > FUNC_INPUT
    F10_SetDigitalOutput();
    #endif  
    
    #if USE_TSC_F11 > FUNC_INPUT
    F11_SetDigitalOutput();
    #endif  
    
    #if USE_TSC_F12 > FUNC_INPUT
    F12_SetDigitalOutput();
    #endif  
    
    #if USE_TSC_F13 > FUNC_INPUT
    F13_SetDigitalOutput();
    #endif  
    
    #if USE_TSC_F14 > FUNC_INPUT
    F14_SetDigitalOutput();
    #endif  
    
    #if USE_TSC_F15 > FUNC_INPUT
    F15_SetDigitalOutput();
    #endif  
    
}
