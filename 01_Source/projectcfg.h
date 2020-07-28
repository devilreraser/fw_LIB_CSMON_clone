/* *****************************************************************************
 * File:   projectcfg.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 04 09 13:13
 * 
 * Description: Project Configuration
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef PROJECTCFG_H
#define	PROJECTCFG_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */


#define PROJECTCFG_FCY_HZ                       200000000.0 	/* Instruction cycle frequency (Hz) for DSPIC and SYSCLK for C2000 */

#ifndef _USE_CONTROL
#define USE_CONTROL             0
#else
#define USE_CONTROL             1
#endif

#define USE_CSMON               1

#define PROJECTCFG_TIMER_MAIN_USEC          50.0

//#define PROJECTCFG_RW_NON_VOLATILE_MEMORY_SIZE_EEPROM_KB    256 /* used data in BOADRCFG_CSMON_PARAM_STRG_RW_SIZE instead */

#define HERCULES_DEBUG  0

#if HERCULES_DEBUG
#define PROJECTCFG_HMDRV_HEXMON_BAUD            115200
#define PROJECTCFG_HMDRV_MODBUS_BAUD            115200
#else
#define PROJECTCFG_HMDRV_HEXMON_BAUD            460800
#define PROJECTCFG_HMDRV_MODBUS_BAUD            460800
#endif


#ifdef _CSMON_EXTERNAL_RECORDERS_TEST_REDIRECT_TO_INTERNAL_RECORDERS
#define PROJECTCFG_EXTERNAL_RECORDERS_TEST_REDIRECT_TO_INTERNAL_RECORDERS _CSMON_EXTERNAL_RECORDERS_TEST_REDIRECT_TO_INTERNAL_RECORDERS
#else
#define PROJECTCFG_EXTERNAL_RECORDERS_TEST_REDIRECT_TO_INTERNAL_RECORDERS 0
#endif


#ifdef _CSMON_RECORDERS_IN_FLAG_PROCESS_PASSED_INTERFACE
#define PROJECTCFG_RECORDERS_IN_FLAG_PROCESS_PASSED_INTERFACE 1
#else
#define PROJECTCFG_RECORDERS_IN_FLAG_PROCESS_PASSED_INTERFACE 0
#endif

#ifdef _CSMON_RTC_INTERFACE_THROUGH_FPGA_SCI
#define PROJECTCFG_CSMON_RTC_INTERFACE_THROUGH_FPGA_SCI
#endif

#ifdef _CSMON_FREE_RUN_TIMER_EXTERNAL
#define PROJECTCFG_FREE_RUN_TIMER_EXTERNAL 1
#else
#define PROJECTCFG_FREE_RUN_TIMER_EXTERNAL 0
#endif

#ifdef _PARAM_LIST_READ_ONLY
#define PROJECTCFG_PARAM_LIST_READ_ONLY 1
#else
#define PROJECTCFG_PARAM_LIST_READ_ONLY 0
#endif




#define PROJECTCFG_USE_HMDRV_TX_DELAY           0
#define PROJECTCFG_USE_HMDRV_MODBUS_TX_DELAY    1
#define PROJECTCFG_STOP_BITS                    SCI_CONFIG_STOP_ONE  //!< One stop bit //     SCI_CONFIG_STOP_TWO   //!< Two stop bits //
 // UART_TWO_STOP_BITS  //


#define UART_A  0   /* SCI_A */
#define UART_B  1   /* SCI_B */
#define UART_C  2   /* SCI_C */
#define UART_D  3   /* SCI_D */


#if defined CPU1

#ifdef _CSMON_USES_SCIA

#if _CSMON_USES_SCIA

#ifndef _CSMON_HEXMON_USES_SCIA
#define _CSMON_HEXMON_USES_SCIA 1
#endif
#ifndef _CSMON_MODBUS_USES_SCIA
#define _CSMON_MODBUS_USES_SCIA 1
#endif

#else

#ifndef _CSMON_HEXMON_USES_SCIA
#define _CSMON_HEXMON_USES_SCIA 0
#endif
#ifndef _CSMON_MODBUS_USES_SCIA
#define _CSMON_MODBUS_USES_SCIA 0
#endif

#endif

#else

#ifndef _CSMON_HEXMON_USES_SCIA
#define _CSMON_HEXMON_USES_SCIA 0
#endif
#ifndef _CSMON_MODBUS_USES_SCIA
#define _CSMON_MODBUS_USES_SCIA 0
#endif

#endif

#ifdef _CSMON_HEXMON_USES_SCIA
#if _CSMON_HEXMON_USES_SCIA == 1
#define PROJECTCFG_HMLIB_HEXMON_UART_MODULE     UART_A
#else
#define PROJECTCFG_HMLIB_HEXMON_UART_MODULE     UART_B
#endif
#else
#define PROJECTCFG_HMLIB_HEXMON_UART_MODULE     UART_B
#endif

#ifdef _CSMON_MODBUS_USES_SCIA
#if _CSMON_MODBUS_USES_SCIA == 1
define PROJECTCFG_HMLIB_MODBUS_UART_MODULE     UART_A
#else
#define PROJECTCFG_HMLIB_MODBUS_UART_MODULE     UART_B
#endif
#else
define PROJECTCFG_HMLIB_MODBUS_UART_MODULE     UART_B
#endif

#elif defined CPU2

#define PROJECTCFG_HMLIB_HEXMON_UART_MODULE     UART_B
#define PROJECTCFG_HMLIB_MODBUS_UART_MODULE     UART_B


#else
/* USE defaults From hmdrv.h */
#endif





#if PROJECTCFG_HMLIB_HEXMON_UART_MODULE == PROJECTCFG_HMLIB_MODBUS_UART_MODULE
#ifdef _CSMON_FORCE_EVEN_PARITY
#if _CSMON_FORCE_EVEN_PARITY
#define PROJECTCFG_UART_PARITY_HEXMON           SCI_CONFIG_PAR_EVEN
#define PROJECTCFG_UART_PARITY_MODBUS           SCI_CONFIG_PAR_EVEN
#else
#define PROJECTCFG_UART_PARITY_HEXMON           SCI_CONFIG_PAR_NONE
#define PROJECTCFG_UART_PARITY_MODBUS           SCI_CONFIG_PAR_NONE
#endif
#else
#define PROJECTCFG_UART_PARITY_HEXMON           SCI_CONFIG_PAR_NONE
#define PROJECTCFG_UART_PARITY_MODBUS           SCI_CONFIG_PAR_NONE
#endif
#else
#define PROJECTCFG_UART_PARITY_HEXMON           SCI_CONFIG_PAR_NONE
#define PROJECTCFG_UART_PARITY_MODBUS           SCI_CONFIG_PAR_EVEN
#endif












#if 0

//#define PROJECTCFG_FCY_HZ          			57000000.0 	/* Instruction cycle frequency (Hz) (PMP CS-1021 Board Only no errors - with Active Low PMALL PMALH) */
//#define PROJECTCFG_FCY_HZ          			56000000.0 	/* Instruction cycle frequency (Hz) (PMP CS-1021 Board Only no errors - with Active Low PMALL PMALH Board In Work) */
//#define PROJECTCFG_FCY_HZ          			54000000.0 	/* Instruction cycle frequency (Hz) (PMP CS-1021 Board Only no errors) */
//#define PROJECTCFG_FCY_HZ          			70000000.0 	/* Instruction cycle frequency (Hz) */

#define PROJECTCFG_OSC_HZ                       8000000.0

#define PROJECTCFG_USE_DYNAMIC_MEMORY           1       /* 1 - use heap    or    0 - owm buffer */



#define PROJECTCFG_SPI_RAM_HZ      			40000000.0
#define PROJECTCFG_SPI_EEPROM_HZ    		 5000000.0
#define PROJECTCFG_SPI_FLIR_HZ     			20000000.0

#define PROJECTCFG_SPI_DAC_HZ     			 1000000.0


#define PROJECTCFG_I2C_DEFAULT_MODULE     	0       /* 0 - I2C1; 1 - I2C2 */

#define PROJECTCFG_SPI_DEFAULT_MODE         3       /* bit0-CPOL bit1-CPHA */


/* *****************************************************************************
 * Configuration Definitions Enable Disable
 **************************************************************************** */

#define USE_UART1_DE            1

#define USE_UART1_RE            0       /* disable if 485 not used */
#define USE_UART1_RE_DISABLED   0       /* disable if 485 not used but uart is used on flir board */

#define USE_UART2_DE            1
#define USE_UART3_DE            1

#define USE_I2C1                1
#define USE_I2C2                0




#define USE_SPI_RAM             1          /* TSC -> RAM   MR25H40 (U304) - Old Net Name RP101_F9 */
#define USE_SPI_EEPROM          1          /* TSC -> EEPROM M95M02 (U303) - Old Net Name RP100_F8 */
#define USE_SPI_FLIR           	0
#define USE_TSC_DO_DI_ADC		1
#define USE_FLIR_RESET          0
#define USE_FLIR_VSYNC          0

#define USE_SPI_DAC             1

#define USE_UART1_RX_PIN        1

#define USE_DEBUG_LED1          0
#define USE_DEBUG_LED2          0

#define FUNC_NONE 0
#define FUNC_INPUT 1
#define FUNC_OUTPUT 2
#define FUNC_DEBUG 3
#define FUNC_INIT_OUTPUT 4

/* Forced Func Initialization */
#define USE_TSC_F0       FUNC_DEBUG
#define USE_TSC_F1       FUNC_OUTPUT         
#define USE_TSC_F2       FUNC_NONE         
#define USE_TSC_F3       FUNC_NONE 
#define USE_TSC_F4       FUNC_NONE 
#define USE_TSC_F5       FUNC_NONE 
#define USE_TSC_F6       FUNC_NONE 
#define USE_TSC_F7       FUNC_NONE 
#define USE_TSC_F8       FUNC_NONE          /* Used For CS2_EEPROM */
#define USE_TSC_F9       FUNC_NONE          /* Used For CS2_RAM */
#define USE_TSC_F10      FUNC_INIT_OUTPUT   /* Has Pull Up - used as output to reset FPGA's Error */
#define USE_TSC_F11      FUNC_DEBUG         /* Has Pull Up */
#define USE_TSC_F12      FUNC_INPUT         /* Has Pull Up */
#define USE_TSC_F13      FUNC_NONE          /* Used For PGED */
#define USE_TSC_F14      FUNC_NONE          /* Used For PGEC */




#define USE_TSC_FLT             1
#define USE_TSC_EN              1

/* *****************************************************************************
 * Debug Definitions 
 **************************************************************************** */
#if USE_TSC_F0 == FUNC_DEBUG
#define DEBUG_SET_PIN0()        {F0_LAT = 1;}
#define DEBUG_CLR_PIN0()        {F0_LAT = 0;}
#else
#define DEBUG_SET_PIN0()       
#define DEBUG_CLR_PIN0()       
#endif

#if USE_TSC_F1 == FUNC_DEBUG
#define DEBUG_SET_PIN1()        {F1_LAT = 1;}
#define DEBUG_CLR_PIN1()        {F1_LAT = 0;}
#else
#define DEBUG_SET_PIN1()       
#define DEBUG_CLR_PIN1()       
#endif


#if USE_TSC_F2 == FUNC_DEBUG
#define DEBUG_SET_PIN2()        {F2_LAT = 1;}
#define DEBUG_CLR_PIN2()        {F2_LAT = 0;}
#else
#define DEBUG_SET_PIN2()       
#define DEBUG_CLR_PIN2()       
#endif


#if USE_TSC_F3 == FUNC_DEBUG
#define DEBUG_SET_PIN3()        {F3_LAT = 1;}
#define DEBUG_CLR_PIN3()        {F3_LAT = 0;}
#else
#define DEBUG_SET_PIN3()       
#define DEBUG_CLR_PIN3()       
#endif


#if USE_TSC_F4 == FUNC_DEBUG
#define DEBUG_SET_PIN4()        {F4_LAT = 1;}
#define DEBUG_CLR_PIN4()        {F4_LAT = 0;}
#else
#define DEBUG_SET_PIN4()       
#define DEBUG_CLR_PIN4()       
#endif


#if USE_TSC_F5 == FUNC_DEBUG
#define DEBUG_SET_PIN5()        {F5_LAT = 1;}
#define DEBUG_CLR_PIN5()        {F5_LAT = 0;}
#else
#define DEBUG_SET_PIN5()       
#define DEBUG_CLR_PIN5()       
#endif


#if USE_TSC_F6 == FUNC_DEBUG
#define DEBUG_SET_PIN6()        {F6_LAT = 1;}
#define DEBUG_CLR_PIN6()        {F6_LAT = 0;}
#else
#define DEBUG_SET_PIN6()       
#define DEBUG_CLR_PIN6()       
#endif


#if USE_TSC_F7 == FUNC_DEBUG
#define DEBUG_SET_PIN7()        {F7_LAT = 1;}
#define DEBUG_CLR_PIN7()        {F7_LAT = 0;}
#else
#define DEBUG_SET_PIN7()       
#define DEBUG_CLR_PIN7()       
#endif


#if USE_TSC_F8 == FUNC_DEBUG
#define DEBUG_SET_PIN8()        {F8_LAT = 1;}
#define DEBUG_CLR_PIN8()        {F8_LAT = 0;}
#else
#define DEBUG_SET_PIN8()       
#define DEBUG_CLR_PIN8()       
#endif


#if USE_TSC_F9 == FUNC_DEBUG
#define DEBUG_SET_PIN9()        {F9_LAT = 1;}
#define DEBUG_CLR_PIN9()        {F9_LAT = 0;}
#else
#define DEBUG_SET_PIN9()       
#define DEBUG_CLR_PIN9()       
#endif


#if USE_TSC_F10 == FUNC_DEBUG
#define DEBUG_SET_PIN10()       {F10_LAT = 1;}
#define DEBUG_CLR_PIN10()       {F10_LAT = 0;}
#else
#define DEBUG_SET_PIN10()       
#define DEBUG_CLR_PIN10()       
#endif


#if USE_TSC_F11 == FUNC_DEBUG
#define DEBUG_SET_PIN11()       {F11_LAT = 1;}
#define DEBUG_CLR_PIN11()       {F11_LAT = 0;}
#else
#define DEBUG_SET_PIN11()       
#define DEBUG_CLR_PIN11()       
#endif


#if USE_TSC_F12 == FUNC_DEBUG
#define DEBUG_SET_PIN12()       {F12_LAT = 1;}
#define DEBUG_CLR_PIN12()       {F12_LAT = 0;}
#else
#define DEBUG_SET_PIN12()       
#define DEBUG_CLR_PIN12()       
#endif


#if USE_TSC_F13 == FUNC_DEBUG
#define DEBUG_SET_PIN13()       {F13_LAT = 1;}
#define DEBUG_CLR_PIN13()       {F13_LAT = 0;}
#else
#define DEBUG_SET_PIN13()       
#define DEBUG_CLR_PIN13()       
#endif


#if USE_TSC_F14 == FUNC_DEBUG
#define DEBUG_SET_PIN14()       {F14_LAT = 1;}
#define DEBUG_CLR_PIN14()       {F14_LAT = 0;}
#else
#define DEBUG_SET_PIN14()       
#define DEBUG_CLR_PIN14()       
#endif



/* Debugs */

#define DEBUG_SET_TMR_1()       DEBUG_SET_PIN1()
#define DEBUG_CLR_TMR_1()       DEBUG_CLR_PIN1()
        


#define DEBUG_SET_USART1_RX()   DEBUG_SET_PIN1()
#define DEBUG_CLR_USART1_RX()   DEBUG_CLR_PIN1()
        
#define DEBUG_SET_USART1_TX()   DEBUG_SET_PIN1()
#define DEBUG_CLR_USART1_TX()   DEBUG_CLR_PIN1()

#define DEBUG_SET_USART1_ERR()  DEBUG_SET_PIN1()
#define DEBUG_CLR_USART1_ERR()  DEBUG_CLR_PIN1()

#define DEBUG_SET_USART2_RX()   DEBUG_SET_PIN1()
#define DEBUG_CLR_USART2_RX()   DEBUG_CLR_PIN1()
        
#define DEBUG_SET_USART2_TX()   DEBUG_SET_PIN1()
#define DEBUG_CLR_USART2_TX()   DEBUG_CLR_PIN1()

#define DEBUG_SET_USART2_ERR()  DEBUG_SET_PIN1()
#define DEBUG_CLR_USART2_ERR()  DEBUG_CLR_PIN1()

#define DEBUG_SET_USART3_RX()   DEBUG_SET_PIN1()
#define DEBUG_CLR_USART3_RX()   DEBUG_CLR_PIN1()
        
#define DEBUG_SET_USART3_TX()   DEBUG_SET_PIN1()
#define DEBUG_CLR_USART3_TX()   DEBUG_CLR_PIN1()

#define DEBUG_SET_USART3_ERR()  DEBUG_SET_PIN1()
#define DEBUG_CLR_USART3_ERR()  DEBUG_CLR_PIN1()

#define DEBUG_SET_USART4_RX()   DEBUG_SET_PIN1()
#define DEBUG_CLR_USART4_RX()   DEBUG_CLR_PIN1()
        
#define DEBUG_SET_USART4_TX()   DEBUG_SET_PIN1()
#define DEBUG_CLR_USART4_TX()   DEBUG_CLR_PIN1()

#define DEBUG_SET_USART4_ERR()  DEBUG_SET_PIN1()
#define DEBUG_CLR_USART4_ERR()  DEBUG_CLR_PIN1()



#define DEBUG_SET_ADC_1()       DEBUG_SET_PIN5()
#define DEBUG_CLR_ADC_1()       DEBUG_CLR_PIN5()

#define DEBUG_SET_ADC_2()       DEBUG_SET_PIN5()
#define DEBUG_CLR_ADC_2()       DEBUG_CLR_PIN5()



#define DEBUG_SET_PWM_PSEV()    DEBUG_SET_PIN5()
#define DEBUG_CLR_PWM_PSEV()    DEBUG_CLR_PIN5()

#define DEBUG_SET_PWM_SSEV()    DEBUG_SET_PIN5()
#define DEBUG_CLR_PWM_SSEV()    DEBUG_CLR_PIN5()

#define DEBUG_SET_PWM_1TO6()    DEBUG_SET_PIN5()
#define DEBUG_CLR_PWM_1TO6()    DEBUG_CLR_PIN5()





#define DEBUG_MAIN_PIN0 1
#define DEBUG_MAIN      1
#define DEBUG_MAIN2     1



#if DEBUG_MAIN_PIN0 == 0
#define DEBUG_SET_PWM_7()       DEBUG_SET_PIN0()
#define DEBUG_CLR_PWM_7()       DEBUG_CLR_PIN0()
#else
#define DEBUG_SET_PWM_7()       
#define DEBUG_CLR_PWM_7()       
#endif


#if DEBUG_MAIN  != 0
#define DEBUG_SET_MAIN()       DEBUG_SET_PIN11()
#define DEBUG_CLR_MAIN()       DEBUG_CLR_PIN11()
#else 
#define DEBUG_SET_MAIN()       
#define DEBUG_CLR_MAIN()       
#endif     


#if DEBUG_MAIN2 != 0
#if DEBUG_MAIN_PIN0 != 0
#define DEBUG_SET_MAIN2()       DEBUG_SET_PIN0()
#define DEBUG_CLR_MAIN2()       DEBUG_CLR_PIN0()
#else 
#define DEBUG_SET_MAIN2()       DEBUG_SET_PIN11()
#define DEBUG_CLR_MAIN2()       DEBUG_CLR_PIN11()
#endif     
#else 
#define DEBUG_SET_MAIN2()   
#define DEBUG_CLR_MAIN2()    
#endif     


#endif

#endif	/* PROJECTCFG_H */
