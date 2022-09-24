/* *****************************************************************************
 * File:   boardcfg.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 01 23 14:49
 * 
 * Description: Board Configuration
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef BOARDCFG_H
#define	BOARDCFG_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <xc.h>
#include "projectcfg.h"
#include "mcc_cfg.h"

/* *****************************************************************************
 * Board Variation
 **************************************************************************** */
#define BOADRCFG_VARIANT_10101_04_16TSC 0   /* Test Stand Board With SPI */
#define BOADRCFG_VARIANT_10101_01_16TSC 1   /* Old Test Stand Board Without SPI */

#define BOADRCFG_VARIANT_10101    BOADRCFG_VARIANT_10101_01_16TSC

#define BOADRCFG_VARIANT_EXT_MEM 0      /* Board With SPI */
#define BOADRCFG_VARIANT_NO_EXT_MEM 1   /* Board Without SPI */

#if BOADRCFG_VARIANT_10101 == BOADRCFG_VARIANT_10101_04_16TSC
#define BOADRCFG_VARIANT    BOADRCFG_VARIANT_EXT_MEM
#elif BOADRCFG_VARIANT_10101 ==  BOADRCFG_VARIANT_10101_01_16TSC
#define BOADRCFG_VARIANT    BOADRCFG_VARIANT_NO_EXT_MEM
#endif

/* *****************************************************************************
 * Configuration Definitions - Driver Modules
 **************************************************************************** */
#define BOARDCFG_USE_UART_1     1
#define BOARDCFG_USE_UART_2     1
#define BOARDCFG_USE_UART_3     1
#define BOARDCFG_USE_UART_4     1

#define BOARDCFG_USE_SPI_1      1
#define BOARDCFG_USE_SPI_2      1
#define BOARDCFG_USE_SPI_3      1
#define BOARDCFG_USE_SPI_4      1

#define BOARDCFG_USE_I2C_1      1
#define BOARDCFG_USE_I2C_2      1


/* *****************************************************************************
 * NON_MCC DEBUG PINs
 **************************************************************************** */
#define ICD_PGD1 0
#define ICD_PGD2 1
#define ICD_PGD3 2

#define BOARDCFG_DEBUG ICD_PGD2

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define BOARDCFG_CSMON_PROCESS_CALL_HMLIB               0

#define BOARDCFG_RW_NON_VOLATILE_MEMORY_SIZE_EEPROM_KB  PROJECTCFG_RW_NON_VOLATILE_MEMORY_SIZE_EEPROM_KB

#define BOARDCFG_USE_ANDREAS_INTERFACE_RTC              0
#define BOARDCFG_USE_ANDREAS_SCI_INIT                   0

#define BOARDCFG_MODBUSREG_USE_CONTROL                  1

/* MODBUS READ SERVER ID */
#define BOARDCFG_VENDORID_BYTE0     'C'
#define BOARDCFG_VENDORID_BYTE1     'S'
#define BOARDCFG_VENDORID_BYTE2     ' '
#define BOARDCFG_VENDORID_BYTE3     ' '
#define BOARDCFG_PRODUCTID_BYTE0    'T'
#define BOARDCFG_PRODUCTID_BYTE1    'S'
#define BOARDCFG_PRODUCTID_BYTE2    'C'
#define BOARDCFG_PRODUCTID_BYTE3    '4'

#define BOARDCFG_SERIALNUM_BYTE0    0x11
#define BOARDCFG_SERIALNUM_BYTE1    0x22
#define BOARDCFG_SERIALNUM_BYTE2    0x33
#define BOARDCFG_SERIALNUM_BYTE3    0x44
#define BOARDCFG_SERIALNUM_BYTE4    0x55
#define BOARDCFG_SERIALNUM_BYTE5    0x66
#define BOARDCFG_SERIALNUM_BYTE6    0x77
#define BOARDCFG_SERIALNUM_BYTE7    0x88

#define BOARDCFG_SERVERID_NUMBER    0x00
#define BOARDCFG_SERVERID_LENGTH    16

#define BOARDCFG_VENDOR_NAME            "CONVESYS"
#define BOARDCFG_PRODUCT_CODE           "TSC"
#define BOARDCFG_MAJOR_MINOR_REVISION   "1.0"
#define BOARDCFG_VENDOR_URL             "www.convesys.bg"
#define BOARDCFG_PRODUCT_NAME           "TSC-TSC4"
#define BOARDCFG_MODEL_NAME             "TSC-TSC4-00"
#define BOARDCFG_USERAPPLICATION_NAME   "TestStandControl"
#define BOARDCFG_USERRESERVE0_NAME      "Reserve0"
#define BOARDCFG_USERRESERVE1_NAME      "Reserve1"
#define BOARDCFG_USERRESERVE2_NAME      "Reserve2"


#define MODBUSREG_DEVICE_ID_VENDORNAME_LENGTH               16
#define MODBUSREG_DEVICE_ID_PRODUCTCODE_LENGTH              16
#define MODBUSREG_DEVICE_ID_MAJORMINORREVISION_LENGTH       16
#define MODBUSREG_DEVICE_ID_VENDORURL_LENGTH                16
#define MODBUSREG_DEVICE_ID_PRODUCTNAME_LENGTH              16
#define MODBUSREG_DEVICE_ID_MODELNAME_LENGTH                16
#define MODBUSREG_DEVICE_ID_USERAPPLICATIONNAME_LENGTH      16
#define MODBUSREG_DEVICE_ID_USERRESERVE0_LENGTH             16
#define MODBUSREG_DEVICE_ID_USERRESERVE1_LENGTH             16
#define MODBUSREG_DEVICE_ID_USERRESERVE2_LENGTH             16


/* *****************************************************************************
 * Configuration Definitions UART
 **************************************************************************** */
#if defined __dsPIC33EP128GM706__
#define UART1_IsTxReady() (UART1_TransmitBufferIsFull() == 0)
#define UART2_IsTxReady() (UART2_TransmitBufferIsFull() == 0)
#define UART3_IsTxReady() (UART3_TransmitBufferIsFull() == 0)
#define UART4_IsTxReady() (UART4_TransmitBufferIsFull() == 0)

#define UART1_IsRxReady() (UART1_ReceiveBufferSizeGet() >  0)
#define UART2_IsRxReady() (UART2_ReceiveBufferSizeGet() >  0)
#define UART3_IsRxReady() (UART3_ReceiveBufferSizeGet() >  0)
#define UART4_IsRxReady() (UART4_ReceiveBufferSizeGet() >  0)

#define UART1_IsTxDone()  ((UART1_TransferStatusGet() & UART1_TRANSFER_STATUS_TX_EMPTY) > 0)
#define UART2_IsTxDone()  ((UART2_TransferStatusGet() & UART2_TRANSFER_STATUS_TX_EMPTY) > 0)
#define UART3_IsTxDone()  ((UART3_TransferStatusGet() & UART3_TRANSFER_STATUS_TX_EMPTY) > 0)
#define UART4_IsTxDone()  ((UART4_TransferStatusGet() & UART4_TRANSFER_STATUS_TX_EMPTY) > 0)
#endif

/* *****************************************************************************
 * Configuration Definitions FPGA Pin
 **************************************************************************** */

#define BOARDCFG_FPGA_RESET()           {FPGA_RST_SetHigh();}
#define BOARDCFG_FPGA_RESET_RELEASE()   {FPGA_RST_SetLow(); }

#define BOARDCFG_FPGA_ENABLE_PWM()      {EN1_SetHigh();}
#define BOARDCFG_FPGA_DISABLE_PWM()     {EN1_SetLow(); }

/* *****************************************************************************
 * Configuration Definitions TESTPROC IO
 **************************************************************************** */

#define BOARDCFG_USE_TSC_SMALL_RELAY_IO_CONFIGURATION   1 /* 1 - means use real IO */


/* *****************************************************************************
 * Configuration Definitions Optimize Functionality Disable 
 **************************************************************************** */
#define BOARDCFG_USE_INFO_CALC_MIN_MAX_AVG              1



/* *****************************************************************************
 * Configuration Definitions Global
 **************************************************************************** */

#define BOARDCFG_STACK_COUNT                        2

#define BOARDCFG_USE_TEMPERATURE_MEASUREMENT_TRIP   1

#define BOARDCFG_USE_DAC_AUTOMATION 0
 
#define BOARDCFG_USE_TIMER_AND_UART_FUNCTIONALITY   1

#define BOARDCFG_USE_FPGA       1      

#define BOARDCFG_USE_FPGA_TRIP  1		/* Gate Driver Error and Detection included */

#define BOARDCFG_USE_FPGA_DIO   1

#define BOARDCFG_USE_FPGA_PWM   1

#define BOARDCFG_USE_DIO        1       /* use IO processing - mainly in main loop */

#define BOARDCFG_USE_DAC_TRIP   1

#define BOARDCFG_USE_TESTPROC   1

#define BOARDCFG_USE_TSC_DSPIC  1

#define BOARDCFG_VOLTAGE_STACK1_OFFSET_CALC_AUTO_MANIPULATION   1
 
#define FCY_HZ          		PROJECTCFG_FCY_HZ

#define OSC_HZ                  PROJECTCFG_OSC_HZ

#define USE_DYNAMIC_MEMORY_RD   PROJECTCFG_USE_DYNAMIC_MEMORY
#define USE_DYNAMIC_MEMORY_WR   PROJECTCFG_USE_DYNAMIC_MEMORY

#define USE_DYNAMIC_MEMORY_RW   (USE_DYNAMIC_MEMORY_WR || USE_DYNAMIC_MEMORY_RD)
        
#define HMDRV_MODBUS_BAUD  		PROJECTCFG_HMDRV_MODBUS_BAUD
#define HMDRV_HEXMON_BAUD  		PROJECTCFG_HMDRV_HEXMON_BAUD

#define HMDRV_TX_DELAY          PROJECTCFG_USE_HMDRV_TX_DELAY

#define HMDRV_MODBUS_TX_DELAY   PROJECTCFG_USE_HMDRV_MODBUS_TX_DELAY

#define UART_PARITY_AND_DATA_MODBUS    PROJECTCFG_UART_PARITY_AND_DATA_MODBUS
#define UART_PARITY_AND_DATA_HEXMON    PROJECTCFG_UART_PARITY_AND_DATA_HEXMON

#define UART_STOP_BITS          PROJECTCFG_STOP_BITS  

#define I2C_DEFAULT_MODULE      PROJECTCFG_I2C_DEFAULT_MODULE

#define SPI_DEFAULT_MODE        PROJECTCFG_SPI_DEFAULT_MODE



#ifdef PROJECTCFG_TIMER_MAIN_USEC
#define TIMER_MAIN_USEC         PROJECTCFG_TIMER_MAIN_USEC
#endif

#ifndef TIMER_MAIN_USEC
#define TIMER_MAIN_USEC         100.0
#endif

#define BOARDCFG_START_WITH_CALIBRATION_FIRST_AFTER_RESET   1

#define BOARDCFG_TESTPROC_IO_FBK_EMULATION   0

#define BOARDCFG_UDCL_MIN_PRECHARGE  900  /*  90V (0.1 V Resolution) */
#define BOARDCFG_UDCL_MIN_PRECHARGE_CHECK  (BOARDCFG_UDCL_MIN_PRECHARGE + 100)  /*  90V + 10V (0.1 V Resolution) */
#define BOARDCFG_UDCL_MAX_PRECHARGE 9000  /* 900V (0.1 V Resolution) */

#define BOARDCFG_UDCL_DEF_REQUEST   1000  /* 100V (0.1 V Resolution) */

#define BOARDCFG_PWM_FREQUENCY_HZ    2500            /* Global Set PWM Frequency in Hz (default 2500Hz) */



/* *****************************************************************************
 * CSMON PARAMETERS and DATA TABLES
 **************************************************************************** */

#define BOADRCFG_CSMON_PARAM_LIST_READ_ONLY 1
#define BOADRCFG_CSMON_PARAM_LIST_RW_ADDR   0
#define BOADRCFG_CSMON_PARAM_LIST_RW_SIZE   0
#define BOADRCFG_CSMON_PARAM_NAME_RW_ADDR   0
#define BOADRCFG_CSMON_PARAM_NAME_RW_SIZE   0
#define BOADRCFG_CSMON_PARAM_NUMB_RW_ADDR   0
#define BOADRCFG_CSMON_PARAM_NUMB_RW_SIZE   0
#define BOADRCFG_CSMON_PARAM_STRG_RW_ADDR   0
#define BOADRCFG_CSMON_PARAM_STRG_RW_SIZE   0
#define BOADRCFG_CSMON_PARAM_SAUX_RW_ADDR   0
#define BOADRCFG_CSMON_PARAM_SAUX_RW_SIZE   0
#define BOADRCFG_CSMON_PARAM_RSRV_RW_ADDR   0
#define BOADRCFG_CSMON_PARAM_RSRV_RW_SIZE   0

#define BOADRCFG_MODBUSREG_HOLDREG_ADDR_TABLE_READ_ONLY  BOADRCFG_CSMON_PARAM_LIST_READ_ONLY
#define BOADRCFG_MODBUSREG_HOLDREG_LIST_RW_ADDR 0
#define BOADRCFG_MODBUSREG_HOLDREG_LIST_RW_SIZE 0





/* *****************************************************************************
 * PWM
 **************************************************************************** */
#define PWM_MIN_DUTY_COEF   (0.02F)
#define PWM_MAX_DUTY_COEF   (0.98F)
#define PWM_START_DUTY_COEF (0.50F)

#define FRAC_AMPLITUDE_PWM_MIN_DUTY_COEF   (0.00F)
#define FRAC_AMPLITUDE_PWM_MAX_DUTY_COEF   (1.00F)

/* *****************************************************************************
 * Analog and Not Only Configuration Definitions
 **************************************************************************** */
/* ADC */
#define ADC_RESULT_BITS                     16  /* 12 */

#define DEAD_TIME_CLOCK_FREQ_HZ             (100000000.0F)      /* FPGA Clock 100 MHz */
#define MIN_PULSE_CLOCK_FREQ_HZ             (100000000.0F)      /* FPGA Clock 100 MHz */

#define SPEED_RESOLUTION_HZ                 (0.05F)

#define DAC_RESULT_BITS                     10
#define DAC_REFERENCE_VOLTAGE_VOLTS         (5.0F)

#define CURRENT_SHUNT_RESISTANCE_OHM        (5.0F)                              /* Current Shunt implemented on CS-1021 Board */

#define CURRENT_TRANSFORMER_AMPERES_COEF    (5000.0F / 1.0F)  
#define CURRENT_TRANSFORMER_COEF            (1.0F / 5000.0F)                    /* Used Current Transformer */ 
#define CURRENT_AMP_COEF_CS_1021_BOARD      (51000.0F / 10000.0F)               /* 5.10 (CS-1021 Board) */
#define ANALOG_CONDITIONING_COEF            ((10000.0F +  1600.0F) /  1600.0F)   /* 7.25 (Control Board) */

#define CS_1021_VOLTAGE_10V_TO_REAL_CURRENT_MULT       (CURRENT_TRANSFORMER_AMPERES_COEF / (CURRENT_AMP_COEF_CS_1021_BOARD * CURRENT_SHUNT_RESISTANCE_OHM))



#define ANALOG_CONDITIONING_CONTROL_BOARD    ANALOG_CONDITIONING_COEF     

#define VOLTAGE_MEASURE_BOARD_DIVIDER       ( ((12.0F * 470000.0F) + 30000.0F) / 30000.0F)                  /* 189 V/V */
//#define VOLTAGE_MEASURE_BOARD_DIVIDER       ( ((12.0F * 470000.0F) +  6800.0F) /  6800.0F) /* 830.4117647058824 * 7.25 = 6020.485294117647 */

#define CURRENT_AMPLIFICATION_COEF          (ANALOG_CONDITIONING_CONTROL_BOARD * CS_1021_VOLTAGE_10V_TO_REAL_CURRENT_MULT)
#define POWER_SUPPLY_VOLTAGE_DIVIDER        ( VOLTAGE_MEASURE_BOARD_DIVIDER * ANALOG_CONDITIONING_CONTROL_BOARD) 

#define CURRRENT_TO_VOLTAGE_CS_1021_MULT    ((CURRENT_AMP_COEF_CS_1021_BOARD * CURRENT_SHUNT_RESISTANCE_OHM) / CURRENT_TRANSFORMER_AMPERES_COEF)                                                        /* 0.0051 V/A (51k/10k; 1/5000; 5Ohm) */

#define ADC_RESULT_TO_AMP_IFBK_MULT         (( CURRENT_AMPLIFICATION_COEF * ADC_REFERENCE_VOLTAGE_VOLTS)/((uint32_t)1 << ADC_RESULT_BITS))

/* CURRENT_TRANSFORMER_AMPERES_COEF not included */
#define ADC_RESULT_TO_AMP_IFBK_INPUT_VOLTAGE_MULT ((ANALOG_CONDITIONING_CONTROL_BOARD * ADC_REFERENCE_VOLTAGE_VOLTS)/((uint32_t)1 << ADC_RESULT_BITS))

/* CS_1021_VOLTAGE_10V_TO_REAL_CURRENT_MULT not included mult with this -> ADC_RESULT_TO_AMP_IFBK_MULT */
#define VOLTAGE_ON_CURRENT_INPUT_TO_AMPERES_MULT    CS_1021_VOLTAGE_10V_TO_REAL_CURRENT_MULT

/*  0,30315554001752068014705882352941                       3,3V          6020,4852941176470588235294117647                  16         */
#define ADC_RESULT_TO_VOLTAGE_MULT          ((ADC_REFERENCE_VOLTAGE_VOLTS * POWER_SUPPLY_VOLTAGE_DIVIDER)/((uint32_t)1 << ADC_RESULT_BITS))

#define ADC_RESULT_TO_VOLTS_DC_MULT	ADC_RESULT_TO_VOLTAGE_MULT    
#define ADC_RESULT_TO_VOLT_LN_MULT	ADC_RESULT_TO_VOLTAGE_MULT    



//compatibility with SCC TI
#define GATE_DRIVER_CURRENT_SENSOR_VOLTAGE_MULT     (1.0F)


//#define CURRENT_FEEDBACK_FIX_MULTIPLIER_CONTROL     (1.0F)
#define CURRENT_FEEDBACK_FIX_MULT_SHIFT_CONTROL     (0)                         /* How Many Times To shift right ADC Value */
//#define CURRENT_FEEDBACK_FIX_MULTIPLIER_STACK1      (1.0F)
#define CURRENT_FEEDBACK_FIX_MULT_SHIFT_STACK1      CURRENT_FEEDBACK_FIX_MULT_SHIFT_CONTROL                         /* How Many Times To shift right ADC Value */
//#define CURRENT_FEEDBACK_FIX_MULTIPLIER_STACK2      (1.0F)
#define CURRENT_FEEDBACK_FIX_MULT_SHIFT_STACK2      CURRENT_FEEDBACK_FIX_MULT_SHIFT_CONTROL                         /* How Many Times To shift right ADC Value */
//#define CURRENT_FEEDBACK_FIX_MULTIPLIER_STACK3      (1.0F)
#define CURRENT_FEEDBACK_FIX_MULT_SHIFT_STACK3      CURRENT_FEEDBACK_FIX_MULT_SHIFT_CONTROL                         /* How Many Times To shift right ADC Value */





    
    //below data to be evaluated and tested
//#define CFBK_AMPERES_PER_VOLT_2400A         (125.0F * (2400.0 / 1000.0)            /* this should be 2400A module and 2000A module */
//#define CFBK_AMPERES_PER_VOLT_1500A         (125.0F * (1500.0 / 1000.0)            /* this should be 1500A module */
//#define CFBK_AMPERES_PER_VOLT_1000A         (125.0F * (1000.0 / 1000.0)            /* this should be 1000A module - same as LSC */
 
#define CFBK_MODULE_NOMINAL_AMPERES         1000   
//#define CFBK_MODULE_NOMINAL_AMPERES         1500   
//#define CFBK_MODULE_NOMINAL_AMPERES         2400   
 
//#define CFBK_AMPERES_PER_VOLT_LSC           (125.0F)            /* this should be 1000A module (ctreader 1,3125 coef) */
//#define CFBK_AMPERES_PER_VOLT_MSC           (187.5F)            /* this should be 1500A module */

#define CFBK_AMPERES_PER_VOLT_BASE_MULT     (float)(125.0 / 1000.0)  //This is Volt Per Amp           
//#define CFBK_AMPERES_PER_VOLT_BASE(x)       ((125.0F * x) / 1000.0)             
#define CFBK_AMPERES_PER_VOLT_BASE(x)       (x / 8)             /* same as above */     
    
//#define CFBK_AMPERES_PER_VOLT_DEF           (CFBK_AMPERES_PER_VOLT_LSC)
#define CFBK_AMPERES_PER_VOLT_DEF           CFBK_AMPERES_PER_VOLT_BASE(CFBK_MODULE_NOMINAL_AMPERES)


                                 /* degrees at 10V     1V      */ 
#define TFBK_DEGREES_PER_VOLT_DEF           ((115.0 - 30.0) / (10.0 - 1.0))
    
#define TFBK_OFFSET_DEGREES                 (30.0 - TFBK_DEGREES_PER_VOLT_DEF)

    
#define ADC_RESULT_TO_AMP_CFBK_MULT     ( (CFBK_AMPERES_PER_VOLT_DEF * ANALOG_CONDITIONING_CONTROL_BOARD) * ((ADC_REFERENCE_VOLTAGE_VOLTS)/((uint32_t)1 << ADC_RESULT_BITS)) )   
    
#define ADC_RESULT_TO_AMP_CFBK_MULT_BASE_1_AMPERE     ( (ANALOG_CONDITIONING_CONTROL_BOARD) * ((ADC_REFERENCE_VOLTAGE_VOLTS)/((uint32_t)1 << ADC_RESULT_BITS)) )   /* 1 Volt on Input Connector */
    
#define ADC_RESULT_TO_DEG_TFBK_MULT     ( (TFBK_DEGREES_PER_VOLT_DEF * ANALOG_CONDITIONING_CONTROL_BOARD) * ((ADC_REFERENCE_VOLTAGE_VOLTS)/((uint32_t)1 << ADC_RESULT_BITS)) )

#define ADC_RESULT_TO_DEG_TFBK_OFFS     TFBK_OFFSET_DEGREES  

    

/* 0-5VDAC -> 5-10V Ref Comparator */
/* DAC Reference Coefficients */
#define DAC_RESULT_TO_AMP_IFBK_OFFS         ( DAC_REFERENCE_VOLTAGE_VOLTS / CURRRENT_TO_VOLTAGE_CS_1021_MULT )                                                                          /* 980.392156862745 A (5V; 51k/10k; 1/5000; 5Ohm) */
#define DAC_RESULT_TO_AMP_IFBK_MULT         ( DAC_REFERENCE_VOLTAGE_VOLTS / (((uint32_t)1 << DAC_RESULT_BITS) * CURRRENT_TO_VOLTAGE_CS_1021_MULT) )                                     /* 0.957414 A/DACBit (10 Bits) */

#define DAC_RESULT_TO_AMP_CFBK_OFFS			DAC_RESULT_TO_AMP_IFBK_OFFS
#define DAC_RESULT_TO_AMP_CFBK_MULT			DAC_RESULT_TO_AMP_IFBK_MULT

#define DAC_RESULT_TO_VOLTS_DC_OFFS         ( DAC_REFERENCE_VOLTAGE_VOLTS * VOLTAGE_MEASURE_BOARD_DIVIDER )                                                                             /* 945 V (5V; 12*470k/30k) */
#define DAC_RESULT_TO_VOLTS_DC_MULT         ((DAC_REFERENCE_VOLTAGE_VOLTS * VOLTAGE_MEASURE_BOARD_DIVIDER)/((uint32_t)1 << DAC_RESULT_BITS) )                                           /* 0.9228515625 V/DACBit (10 Bits) */






/* *****************************************************************************
 * Other Configuration Definitions
 **************************************************************************** */

#ifdef PROJECTCFG_HMLIB_HEXMON_UART_MODULE
#define HMLIB_HEXMON_UART_MODULE       PROJECTCFG_HMLIB_HEXMON_UART_MODULE
#else
#define HMLIB_HEXMON_UART_MODULE       UART_1
#endif

#ifdef PROJECTCFG_HMLIB_MODBUS_UART_MODULE
#define HMLIB_MODBUS_UART_MODULE       PROJECTCFG_HMLIB_MODBUS_UART_MODULE
#else
#define HMLIB_MODBUS_UART_MODULE       UART_1
#endif


#define HMLIB_MODBUS_DEBUG_RESPONSES                    0

#if HMLIB_HEXMON_UART_MODULE != HMLIB_MODBUS_UART_MODULE
#define HMLIB_MODBUS_AND_HEXMON_UART_MODULE_DIFFERENT   1
#else
#define HMLIB_MODBUS_AND_HEXMON_UART_MODULE_DIFFERENT   0
#endif



#define SPI_RAM_HZ      		PROJECTCFG_SPI_RAM_HZ
#define SPI_EEPROM_HZ    		PROJECTCFG_SPI_EEPROM_HZ
#define SPI_FLIR_HZ     		PROJECTCFG_SPI_FLIR_HZ

#define SPI_DAC_HZ              PROJECTCFG_SPI_DAC_HZ

#define SPI_PRESCALER(frequency)    (uint16_t)((FCY_HZ / frequency) + 0.0) /* no roundup - here is set the maximum frequency */

#define EXT_MEM_SPI_MODULE      SPI_2
#define EXT_MEM_SPI_DMA_TX      DMA_0
#define EXT_MEM_SPI_DMA_RX      DMA_1

#define DAC_SPI_MODULE          SPI_1

#define EXT_MEM_PMP_DMA         DMA_2





#define PMP_USE_DMA             1       /* use dma only if freemaster used of fix while(busy) loops */
#define PMP_BUS_SIZE_BYTES      1       /* 1 or 2 bytes select */

#define USE_IN_FPGA_CS_DELAY_INSTEAD_OF_PMALL_AND_PMALH_STROBES 1   /* TSC Board */




/* *****************************************************************************
 * Interrupt Priority Definitions
 **************************************************************************** */

#define ISR_PRIORITY_UARTSRX    5
#define ISR_PRIORITY_UARTSTX    5
#define ISR_PRIORITY_UARTSERR   5

#define ISR_PRIORITY_TMR1       7


#define ISR_PRIORITY_PWMADC_CRITICAL    6

#if BOARDCFG_USE_OLD_NON_MCC_ADC
#define ISR_PRIORITY_ADC1       ISR_PRIORITY_PWMADC_CRITICAL
#define ISR_PRIORITY_ADC2       ISR_PRIORITY_PWMADC_CRITICAL
#else
#define ISR_PRIORITY_ADC1       2
#define ISR_PRIORITY_ADC2       2
#endif
 
#define ISR_PRIORITY_PWMSEVTIP  ISR_PRIORITY_PWMADC_CRITICAL
#define ISR_PRIORITY_PWMSSEVTIP ISR_PRIORITY_PWMADC_CRITICAL

#define ISR_PRIORITY_PWM1TO6    ISR_PRIORITY_PWMADC_CRITICAL    /* Phase Shifting and trigger for shifted modes. Shifted modes must have the same ISR_PRIORITY_PWMADC_CRITICAL */
#define ISR_PRIORITY_PWM1       ISR_PRIORITY_PWM1TO6
#define ISR_PRIORITY_PWM2       ISR_PRIORITY_PWM1TO6
#define ISR_PRIORITY_PWM3       ISR_PRIORITY_PWM1TO6
#define ISR_PRIORITY_PWM4       ISR_PRIORITY_PWM1TO6
#define ISR_PRIORITY_PWM5       ISR_PRIORITY_PWM1TO6
#define ISR_PRIORITY_PWM6       ISR_PRIORITY_PWM1TO6

#define ISR_PRIORITY_PWM7       2   /* At least One Level Less than PWM ADC Criticals */

#define ISR_PRIORITY_I2C1       4
#define ISR_PRIORITY_I2C2       4

#define ISR_PRIORITY_SPIRXDMA   4       /* SPI Transfer ISR Priority */
#define ISR_PRIORITY_SPITXDMA   4       

#define ISR_PRIORITY_SPITRANS   4       /* SPI Transfer ISR Priority */
#define ISR_PRIORITY_SPIERROR   4       /* SPI Error ISR Priority */       

#define ISR_PRIORITY_INCAP1     3
#define ISR_PRIORITY_INCAP2     3

#define ISR_PRIORITY_TMR7       1   /* input capture use */
#define ISR_PRIORITY_TMR9       1

/* *****************************************************************************
 * Pin Definitions
 **************************************************************************** */
/* *****************************************************************************
 * PPS
 **************************************************************************** */
#if defined __dsPIC33EP512MU814__
#define U1TX_PIN_PPS _RP113R            //RP as U1TX   G1
#define U1RX_PIN_PPS 31                 //RP as U1RX   A15
#define U2TX_PIN_PPS _RP112R            //RP as U2TX   G0
#define U2RX_PIN_PPS 30                 //RP as U2RX   A14
#define U3TX_PIN_PPS _RP127R            //RP as U3TX   G15
#define U3RX_PIN_PPS 121                //RP as U3RX   G9
#define U4TX_PIN_PPS _RP71R             //RP as U4TX   D7
#define U4RX_PIN_PPS 72                 //RP as U4RX   D8
#elif defined __dsPIC33EP512GM710__
#define U1TX_PIN_PPS _RP127R            //RP as U1TX   G15
#define U1RX_PIN_PPS 124                //RP as U1RX   G12
#define U2TX_PIN_PPS _RP38R             //RP as U2TX   B6
#define U2RX_PIN_PPS 37                 //RP as U2RX   B5
#define U3TX_PIN_PPS _RP39R             //RP as U3TX   B7
#define U3RX_PIN_PPS 61                 //RP as U3RX   C13   
#elif defined __dsPIC33EP128GM706__
#define U1TX_PIN_PPS _RP70R             //RP as U1TX   D6
#define U1RX_PIN_PPS 72                 //RP as U1RX   D8
#else
#error "define pins if needed"
#endif

/* *****************************************************************************
 * Other Peripheral Auxiliary - Enable, Open Drain and Chip Select
 **************************************************************************** */

#if USE_I2C1
#define I2C1_SDA_SetOpenDrain()       { ODCDbits.ODCD9  = 1; }
#define I2C1_SCL_SetOpenDrain()       { ODCDbits.ODCD10 = 1; }
#endif

#if USE_I2C2
#define I2C2_SDA_SetOpenDrain()       { ODCFbits.ODCF4  = 1; }
#define I2C2_SCL_SetOpenDrain()       { ODCFbits.ODCF5  = 1; }
#endif


#if USE_SPI_RAM
#define CS_RAM_SetHigh()    { CS2_SPI2_SetHigh(); }
#define CS_RAM_SetLow()     { CS2_SPI2_SetLow();  }         /* RAM MR25H40 (U304) - Old Net Name RP101_F9 */
#endif

#if USE_SPI_EEPROM
#define CS_EEPROM_SetHigh() { CS1_SPI2_SetHigh(); }
#define CS_EEPROM_SetLow()  { CS1_SPI2_SetLow();  }         /* EEPROM M95M02 (U303) - Old Net Name RP100_F8 */
#endif

#if USE_SPI_FPGA
#define CS_FPGA_SetHigh()  /* { CS3_SPI2_SetHigh(); } */
#define CS_FPGA_SetLow()   /* { CS3_SPI2_SetLow();  } */         /* FPGA  */
#endif

#if USE_SPI_DAC
#define LD_DAC_SetHigh()     { CS1_SPI1_SetHigh(); } 
#define LD_DAC_SetLow()      { CS1_SPI1_SetLow();  }            /* DAC BU2506 trough TRIP FPGA */        
#endif

#if USE_SPI_FLIR
#define CS_FLIR_SetHigh()  /* { CS1_SPI1_SetHigh(); } */
#define CS_FLIR_SetLow()   /* { CS1_SPI1_SetLow();  } */         /* FLIR CAMERA  */

#define FLIR_RESET_SetHigh()  /* { FLIR_RST_SetHigh(); } */
#define FLIR_RESET_SetLow()   /* { FLIR_RST_SetLow();  } */      /* FLIR CAMERA Reset Pin - Must Be Configured As Open Drain  */

#define FLIR_VSYNC_GetValue() /* FLIR_VSYNC_GetValue() */
#endif



/* *****************************************************************************
 * Function Definitions 
 **************************************************************************** */

#define BOARDCFG_USE_FUNCTION_PINS 1

#if BOARDCFG_USE_FUNCTION_PINS
#define FUNC_NONE 0
#define FUNC_INPUT 1
#define FUNC_OUTPUT 2
#define FUNC_DEBUG 3
#define FUNC_INIT_OUTPUT 4

/* Forced Func Initialization */
#define USE_TSC_F0       FUNC_DEBUG
#define USE_TSC_F1       FUNC_OUTPUT    /* SSR */        
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

#endif



/* *****************************************************************************
 * Debug Definitions 
 **************************************************************************** */

#define DEBUG_CTRL_PIN0 1
#define DEBUG_MAIN_PIN0 0

#define DEBUG_PWM7      0
#define DEBUG_MAIN      0
#define DEBUG_MAIN2     1
#define DEBUG_CTRL      1
#define DEBUG_ADC1_ISR  0
#define DEBUG_UART_ERR  0
#define DEBUG_UART_RX   0



#if DEBUG_UART_RX
#define DEBUG_SET_USART1_RX()   {F11_SetHigh();}
#define DEBUG_CLR_USART1_RX()   {F11_SetLow(); }
        
#define DEBUG_SET_USART2_RX()   {F11_SetHigh();}
#define DEBUG_CLR_USART2_RX()   {F11_SetLow(); }
        
#define DEBUG_SET_USART3_RX()   {F11_SetHigh();}
#define DEBUG_CLR_USART3_RX()   {F11_SetLow(); }
        
#define DEBUG_SET_USART4_RX()   {F11_SetHigh();}
#define DEBUG_CLR_USART4_RX()   {F11_SetLow(); }
#else
#define DEBUG_SET_USART1_RX()   
#define DEBUG_CLR_USART1_RX()   
        
#define DEBUG_SET_USART2_RX()   
#define DEBUG_CLR_USART2_RX()   
        
#define DEBUG_SET_USART3_RX()   
#define DEBUG_CLR_USART3_RX()   
        
#define DEBUG_SET_USART4_RX()   
#define DEBUG_CLR_USART4_RX()   
#endif


#if DEBUG_UART_ERR
#define DEBUG_SET_USART1_ERR()  {F11_SetHigh();}
#define DEBUG_CLR_USART1_ERR()  {F11_SetLow(); }

#define DEBUG_SET_USART2_ERR()  {F11_SetHigh();}
#define DEBUG_CLR_USART2_ERR()  {F11_SetLow(); }

#define DEBUG_SET_USART3_ERR()  {F11_SetHigh();}
#define DEBUG_CLR_USART3_ERR()  {F11_SetLow(); }

#define DEBUG_SET_USART4_ERR()  {F11_SetHigh();}
#define DEBUG_CLR_USART4_ERR()  {F11_SetLow(); }
#else
#define DEBUG_SET_USART1_ERR()  
#define DEBUG_CLR_USART1_ERR()  

#define DEBUG_SET_USART2_ERR()  
#define DEBUG_CLR_USART2_ERR()  

#define DEBUG_SET_USART3_ERR()  
#define DEBUG_CLR_USART3_ERR()  

#define DEBUG_SET_USART4_ERR()  
#define DEBUG_CLR_USART4_ERR()  
#endif

#define DEBUG_SET_USART1_TX()   
#define DEBUG_CLR_USART1_TX()   

#define DEBUG_SET_USART2_TX()   
#define DEBUG_CLR_USART2_TX()   

#define DEBUG_SET_USART3_TX()   
#define DEBUG_CLR_USART3_TX()   

#define DEBUG_SET_USART4_TX()   
#define DEBUG_CLR_USART4_TX()   


#define DEBUG_SET_TMR_1()       
#define DEBUG_CLR_TMR_1()       
        

#if DEBUG_ADC1_ISR
#define DEBUG_SET_ADC_1()       {F11_SetHigh();}
#define DEBUG_CLR_ADC_1()       {F11_SetLow(); }
#else
#define DEBUG_SET_ADC_1()       
#define DEBUG_CLR_ADC_1()       
#endif

#define DEBUG_SET_ADC_2()       
#define DEBUG_CLR_ADC_2()       


#define DEBUG_SET_PWM_PSEV()    
#define DEBUG_CLR_PWM_PSEV()    

#define DEBUG_SET_PWM_SSEV()    
#define DEBUG_CLR_PWM_SSEV()    

#define DEBUG_SET_PWM_1TO6()    
#define DEBUG_CLR_PWM_1TO6()    

#if DEBUG_PWM7
#define DEBUG_SET_PWM_7()       {F0_SetHigh();}
#define DEBUG_CLR_PWM_7()       {F0_SetLow(); }
#else
#define DEBUG_SET_PWM_7()       
#define DEBUG_CLR_PWM_7()       
#endif


#if DEBUG_MAIN  != 0
#define DEBUG_SET_MAIN()       {F11_SetHigh();}
#define DEBUG_CLR_MAIN()       {F11_SetLow(); }
#else 
#define DEBUG_SET_MAIN()       
#define DEBUG_CLR_MAIN()       
#endif     


#if DEBUG_MAIN2 != 0
#if DEBUG_MAIN_PIN0 != 0
#define DEBUG_SET_MAIN2()       {F0_SetHigh();}
#define DEBUG_CLR_MAIN2()       {F0_SetLow(); }
#else 
#define DEBUG_SET_MAIN2()       {F11_SetHigh();}
#define DEBUG_CLR_MAIN2()       {F11_SetLow(); }
#endif     
#else 
#define DEBUG_SET_MAIN2()   
#define DEBUG_CLR_MAIN2()    
#endif     


#if DEBUG_CTRL
#if DEBUG_CTRL_PIN0
#define DEBUG_SET_CTRL()       {F0_SetHigh();}
#define DEBUG_CLR_CTRL()       {F0_SetLow(); }
#else
#define DEBUG_SET_CTRL()       {F11_SetHigh();}
#define DEBUG_CLR_CTRL()       {F11_SetLow(); }
#endif
#else
#define DEBUG_SET_CTRL()       
#define DEBUG_CLR_CTRL()       
#endif





/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
#endif	/* BOARDCFG_H */
