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
#if 0
#include <xc.h>
#endif
#include "projectcfg.h"



/* *****************************************************************************
 * Configuration Board Definitions
 **************************************************************************** */


#define BOARDCFG_BOARD_SCC_NONE                         0
#define BOARDCFG_BOARD_SCC_BIG                          1
#define BOARDCFG_BOARD_SCC_SMALL_TEST                   2
#define BOARDCFG_BOARD_CS_1107_SCC                      3

#ifndef BOARDCFG_BOARD
    #ifdef CPU
        #define BOARDCFG_BOARD          BOARDCFG_BOARD_SCC_SMALL_TEST
    #else
        #define BOARDCFG_BOARD          BOARDCFG_BOARD_SCC_BIG
    #endif
#endif


#define EMIF_2_SDRAM		1



/* *****************************************************************************
 * Configuration General Definitions
 **************************************************************************** */

#define BOARDCFG_SHORT_NAME_CODED_IN_HIGH_BYTE_TI 1

#define BOARDCFG_USE_CSMON_NUMERIC_TABLE    1
#define BOARDCFG_USE_CSMON_AUXILIARY_TABLE  1
#define BOARDCFG_USE_CSMON_STRING_TABLE     1


#ifndef PROJECTCFG_PARAM_LIST_READ_ONLY
#define BOADRCFG_CSMON_PARAM_LIST_READ_ONLY             0       /* set also BOADRCFG_MODBUSREG_HOLDREG_ADDR_TABLE_READ_ONLY */
#else
#define BOADRCFG_CSMON_PARAM_LIST_READ_ONLY             PROJECTCFG_PARAM_LIST_READ_ONLY
#endif

#define BOARDCFG_CSMON_PARTITIONING_MIN_SIZE_USED       1

#define BOADRCFG_MODBUSREG_HOLDREG_ADDR_TABLE_READ_ONLY BOADRCFG_CSMON_PARAM_LIST_READ_ONLY


#if BOADRCFG_CSMON_PARAM_LIST_READ_ONLY == 0
#define BOARDCFG_CSMON_FILE_PARAMETER_COUNT_MAX         1024
#endif

#if BOADRCFG_MODBUSREG_HOLDREG_ADDR_TABLE_READ_ONLY == 0
#define BOARDCFG_MODBUSREG_HOLDREG_ADDR_TABLE_COUNT_MAX 1024
#endif

#define BOARDCFG_EPWM_USE_FOR_BOARD_HARDWARE_TEST       1
#define BOARDCFG_ADC_ISR_LOW_LEVEL                      0

/* EMIF PARTITIONING */

#define BOARDCFG_EMIF_ADDRESS_START                     (uint32_t)0x00100000                    /* TI 16-bit Memory address */
#define BOARDCFG_EMIF_SIZE_BYTES                        (uint32_t)0x00200000                    /* 1M * 16-bit (2Mb) */
#define BOARDCFG_EMIF_SIZE_BYTES_RECORDERS              (uint32_t)0x00140000                    /* 0.625M * 16-bit (1.25Mb) */

#define BOARDCFG_ADDR_WIDTH                             2


/* Partitioning with Min Size Used */
#define BOARDCFG_CSMON_PARAM_COUNT_REQUEST              (uint32_t)1024
#define BOADRCFG_CSMON_PARAM_LIST_RECORD_SIZE_IN_MEMORY (uint32_t)16  /* same as CSMON_FILE_PARAMETER_LIST_LENGTH_BYTES_IN_MEMORY         (sizeof(CSMON_sFileParameterDefinitions_t)    * HMDRV_ADDR_WIDTH)    */
#define BOADRCFG_CSMON_PARAM_NAME_RECORD_SIZE_IN_MEMORY (uint32_t)64  /* same as CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH_BYTES_IN_MEMORY   (CSMON_FILE_PARAMETER_SHORT_NAME_LENGTH       * HMDRV_ADDR_WIDTH)    */
#define BOADRCFG_CSMON_PARAM_NUMB_RECORD_SIZE_IN_MEMORY (uint32_t)48  /* same as CSMON_FILE_PARAMETER_NUMERIC_LENGTH_BYTES_IN_MEMORY      (sizeof(CSMON_sFileParameterNumeric_t)        * HMDRV_ADDR_WIDTH)    */
#define BOADRCFG_MODBUS_HOLDREG_LIST_REC_SIZE_IN_MEMORY (uint32_t) 6  /* same as                                                          (sizeof(HMMODBUS_sRegisterAddressTable_t)     * HMDRV_ADDR_WIDTH)    */
#define BOADRCFG_MODBUS_HOLDREG_ADDR_REC_SIZE_IN_MEMORY (uint32_t) 8  /* same as                                                          (sizeof(HMMODBUS_sRegisterAddressTableInRam_t)* HMDRV_ADDR_WIDTH)    */
#define BOARDCFG_CSMON_PARAM_COUNT_REQUEST_DPN          200           /* DPN Maximum amount of CSMON parameters */

#define BOADRCFG_CSMON_ENER_DRIVE_RW_SIZE               (uint32_t)((uint32_t)256 * 1024)  /* 256 kB EnerDrive Usage at the end */
#define BOARDCFG_EMIF_SIZE_BYTES_RECORDERS_AND_ENERDRIVE (uint32_t)(BOADRCFG_CSMON_ENER_DRIVE_RW_SIZE+BOARDCFG_EMIF_SIZE_BYTES_RECORDERS)



#if BOARDCFG_CSMON_PARTITIONING_MIN_SIZE_USED


#if BOADRCFG_CSMON_PARAM_LIST_READ_ONLY == 0
#define BOADRCFG_CSMON_PARAM_LIST_RW_SIZE               (uint32_t)(BOARDCFG_CSMON_PARAM_COUNT_REQUEST * BOADRCFG_CSMON_PARAM_LIST_RECORD_SIZE_IN_MEMORY)  /*  16 kB */
#define BOADRCFG_CSMON_PARAM_NAME_RW_SIZE               (uint32_t)(BOARDCFG_CSMON_PARAM_COUNT_REQUEST * BOADRCFG_CSMON_PARAM_NAME_RECORD_SIZE_IN_MEMORY)  /*  64 kB */
#define BOADRCFG_CSMON_PARAM_NUMB_RW_SIZE               (uint32_t)(BOARDCFG_CSMON_PARAM_COUNT_REQUEST * BOADRCFG_CSMON_PARAM_NUMB_RECORD_SIZE_IN_MEMORY)  /*  48 kB */

#define BOADRCFG_CSMON_PARAM_BUFF_RW_SIZE               (uint32_t)(BOARDCFG_EMIF_SIZE_BYTES_RECORDERS_AND_ENERDRIVE)/* 3 * 512 kB */
#define BOADRCFG_CSMON_PARAM_SAUX_RW_SIZE               (uint32_t)((uint32_t)50 * 1024)                             /*  50 kB */
#define BOADRCFG_CSMON_PARAM_RSRV_RW_SIZE               (uint32_t)((uint32_t)64 * 1024)                             /*  If need Recorders To be placed at aligned position or Andreas Usage */
#else
#define BOADRCFG_CSMON_PARAM_LIST_RW_SIZE               (uint32_t)(0)
#define BOADRCFG_CSMON_PARAM_NAME_RW_SIZE               (uint32_t)(0)
#define BOADRCFG_CSMON_PARAM_NUMB_RW_SIZE               (uint32_t)(0)

#define BOADRCFG_CSMON_PARAM_BUFF_RW_SIZE               (uint32_t)(BOARDCFG_EMIF_SIZE_BYTES_RECORDERS_AND_ENERDRIVE)  /* 3 * 512 kB */
#define BOADRCFG_CSMON_PARAM_SAUX_RW_SIZE               (uint32_t)((uint32_t)64 * 1024)                             /*  64 kB */
#define BOADRCFG_CSMON_PARAM_RSRV_RW_SIZE               (uint32_t)((uint32_t)64 * 1024)                             /*  If need Recorders To be placed at aligned position or Andreas Usage */
#endif


#if BOADRCFG_MODBUSREG_HOLDREG_ADDR_TABLE_READ_ONLY == 0
#define BOADRCFG_MODBUSREG_HOLDREG_LIST_RW_SIZE         (BOARDCFG_CSMON_PARAM_COUNT_REQUEST * BOADRCFG_MODBUS_HOLDREG_LIST_REC_SIZE_IN_MEMORY)  /*  6 kB */
#define BOADRCFG_MODBUSREG_HOLDREG_ADDR_RW_SIZE         (BOARDCFG_CSMON_PARAM_COUNT_REQUEST * BOADRCFG_MODBUS_HOLDREG_ADDR_REC_SIZE_IN_MEMORY)  /*  8 kB */
#else
#define BOADRCFG_MODBUSREG_HOLDREG_LIST_RW_SIZE         (uint32_t)(0)
#define BOADRCFG_MODBUSREG_HOLDREG_ADDR_RW_SIZE         (uint32_t)(0)
#endif


#else


#if BOADRCFG_CSMON_PARAM_LIST_READ_ONLY == 0
#define BOADRCFG_CSMON_PARAM_LIST_RW_SIZE               (uint32_t)((uint32_t)16 * 1024)                             /*  16 kB */
#define BOADRCFG_CSMON_PARAM_NAME_RW_SIZE               (uint32_t)((uint32_t)64 * 1024)                             /*  64 kB */
#define BOADRCFG_CSMON_PARAM_NUMB_RW_SIZE               (uint32_t)((uint32_t)48 * 1024)                             /*  48 kB */

#define BOADRCFG_CSMON_PARAM_BUFF_RW_SIZE               (uint32_t)(BOARDCFG_EMIF_SIZE_BYTES_RECORDERS_AND_ENERDRIVE)  /* 3 * 512 kB */
#define BOADRCFG_CSMON_PARAM_SAUX_RW_SIZE               (uint32_t)((uint32_t)58 * 1024)                             /*  58 kB */
#define BOADRCFG_CSMON_PARAM_RSRV_RW_SIZE               (uint32_t)((uint32_t)64 * 1024)                             /*  If need Recorders To be placed at aligned position or Andreas Usage */
#else
#define BOADRCFG_CSMON_PARAM_LIST_RW_SIZE               (uint32_t)(0)
#define BOADRCFG_CSMON_PARAM_NAME_RW_SIZE               (uint32_t)(0)
#define BOADRCFG_CSMON_PARAM_NUMB_RW_SIZE               (uint32_t)(0)

#define BOADRCFG_CSMON_PARAM_BUFF_RW_SIZE               (uint32_t)(BOARDCFG_EMIF_SIZE_BYTES_RECORDERS_AND_ENERDRIVE)  /* 3 * 512 kB */
#define BOADRCFG_CSMON_PARAM_SAUX_RW_SIZE               (uint32_t)((uint32_t)64 * 1024)                             /*  64 kB */
#define BOADRCFG_CSMON_PARAM_RSRV_RW_SIZE               (uint32_t)((uint32_t)64 * 1024)                             /*  If need Recorders To be placed at aligned position or Andreas Usage */
#endif

#if BOADRCFG_MODBUSREG_HOLDREG_ADDR_TABLE_READ_ONLY == 0
#define BOADRCFG_MODBUSREG_HOLDREG_LIST_RW_SIZE         (BOARDCFG_CSMON_PARAM_COUNT_REQUEST * BOADRCFG_MODBUS_HOLDREG_LIST_REC_SIZE_IN_MEMORY)  /*  6 kB */
#define BOADRCFG_MODBUSREG_HOLDREG_ADDR_RW_SIZE         (BOARDCFG_CSMON_PARAM_COUNT_REQUEST * BOADRCFG_MODBUS_HOLDREG_ADDR_REC_SIZE_IN_MEMORY)  /*  8 kB */
#else
#define BOADRCFG_MODBUSREG_HOLDREG_LIST_RW_SIZE         (uint32_t)(0)
#define BOADRCFG_MODBUSREG_HOLDREG_ADDR_RW_SIZE         (uint32_t)(0)
#endif


#endif



#define BOADRCFG_CSMON_PARAM_ALL_MCU_SIZE               (uint32_t)(BOADRCFG_MODBUSREG_HOLDREG_LIST_RW_SIZE + BOADRCFG_MODBUSREG_HOLDREG_ADDR_RW_SIZE + BOADRCFG_CSMON_PARAM_LIST_RW_SIZE + BOADRCFG_CSMON_PARAM_NAME_RW_SIZE + BOADRCFG_CSMON_PARAM_NUMB_RW_SIZE + BOADRCFG_CSMON_PARAM_SAUX_RW_SIZE + BOADRCFG_CSMON_PARAM_RSRV_RW_SIZE)

#define BOADRCFG_CSMON_PARAM_STRG_RW_SIZE               (uint32_t)(BOARDCFG_EMIF_SIZE_BYTES - (BOADRCFG_CSMON_PARAM_ALL_MCU_SIZE + BOADRCFG_CSMON_PARAM_BUFF_RW_SIZE))                /* 256 kB or 384 kB */


#if BOADRCFG_CSMON_PARAM_LIST_READ_ONLY == 0

#define BOADRCFG_CSMON_PARAM_STRG_RW_ADDR               (uint32_t)(BOARDCFG_EMIF_ADDRESS_START)                                                                                                 /*  256 kB */
#define BOADRCFG_CSMON_PARAM_SAUX_RW_ADDR               (uint32_t)(BOADRCFG_CSMON_PARAM_STRG_RW_ADDR          + (uint32_t)(BOADRCFG_CSMON_PARAM_STRG_RW_SIZE        / BOARDCFG_ADDR_WIDTH))     /*   50 kB */
#define BOADRCFG_MODBUSREG_HOLDREG_LIST_RW_ADDR         (uint32_t)(BOADRCFG_CSMON_PARAM_SAUX_RW_ADDR          + (uint32_t)(BOADRCFG_CSMON_PARAM_SAUX_RW_SIZE        / BOARDCFG_ADDR_WIDTH))     /*    6 kB */
#define BOADRCFG_MODBUSREG_HOLDREG_ADDR_RW_ADDR         (uint32_t)(BOADRCFG_MODBUSREG_HOLDREG_LIST_RW_ADDR    + (uint32_t)(BOADRCFG_MODBUSREG_HOLDREG_LIST_RW_SIZE  / BOARDCFG_ADDR_WIDTH))     /*    8 kB */
#define BOADRCFG_CSMON_PARAM_LIST_RW_ADDR               (uint32_t)(BOADRCFG_MODBUSREG_HOLDREG_ADDR_RW_ADDR    + (uint32_t)(BOADRCFG_MODBUSREG_HOLDREG_ADDR_RW_SIZE  / BOARDCFG_ADDR_WIDTH))     /*   16 kB */
#define BOADRCFG_CSMON_PARAM_NUMB_RW_ADDR               (uint32_t)(BOADRCFG_CSMON_PARAM_LIST_RW_ADDR          + (uint32_t)(BOADRCFG_CSMON_PARAM_LIST_RW_SIZE        / BOARDCFG_ADDR_WIDTH))     /*   48 kB */
#define BOADRCFG_CSMON_PARAM_NAME_RW_ADDR               (uint32_t)(BOADRCFG_CSMON_PARAM_NUMB_RW_ADDR          + (uint32_t)(BOADRCFG_CSMON_PARAM_NUMB_RW_SIZE        / BOARDCFG_ADDR_WIDTH))     /*   64 kB */
#define BOADRCFG_CSMON_PARAM_RSRV_RW_ADDR               (uint32_t)(BOADRCFG_CSMON_PARAM_NAME_RW_ADDR          + (uint32_t)(BOADRCFG_CSMON_PARAM_NAME_RW_SIZE        / BOARDCFG_ADDR_WIDTH))     /*   64 kB */
#define BOADRCFG_CSMON_PARAM_BUFF_RW_ADDR               (uint32_t)(BOADRCFG_CSMON_PARAM_RSRV_RW_ADDR          + (uint32_t)(BOADRCFG_CSMON_PARAM_RSRV_RW_SIZE        / BOARDCFG_ADDR_WIDTH))     /* 1536 kB */

#else

#define BOADRCFG_CSMON_PARAM_STRG_RW_ADDR               (uint32_t)(BOARDCFG_EMIF_ADDRESS_START)                                                                                                 /*  384 kB */
#define BOADRCFG_CSMON_PARAM_SAUX_RW_ADDR               (uint32_t)(BOADRCFG_CSMON_PARAM_STRG_RW_ADDR          + (uint32_t)(BOADRCFG_CSMON_PARAM_STRG_RW_SIZE        / BOARDCFG_ADDR_WIDTH))     /*   64 kB */
#define BOADRCFG_MODBUSREG_HOLDREG_LIST_RW_ADDR         (uint32_t)(BOADRCFG_CSMON_PARAM_SAUX_RW_ADDR          + (uint32_t)(BOADRCFG_CSMON_PARAM_SAUX_RW_SIZE        / BOARDCFG_ADDR_WIDTH))     /*    0 kB */
#define BOADRCFG_MODBUSREG_HOLDREG_ADDR_RW_ADDR         (uint32_t)(BOADRCFG_MODBUSREG_HOLDREG_LIST_RW_ADDR    + (uint32_t)(BOADRCFG_MODBUSREG_HOLDREG_LIST_RW_SIZE  / BOARDCFG_ADDR_WIDTH))     /*    0 kB */
#define BOADRCFG_CSMON_PARAM_LIST_RW_ADDR               (uint32_t)(BOADRCFG_MODBUSREG_HOLDREG_ADDR_RW_ADDR    + (uint32_t)(BOADRCFG_MODBUSREG_HOLDREG_ADDR_RW_SIZE  / BOARDCFG_ADDR_WIDTH))     /*    0 kB */
#define BOADRCFG_CSMON_PARAM_NUMB_RW_ADDR               (uint32_t)(BOADRCFG_CSMON_PARAM_LIST_RW_ADDR          + (uint32_t)(BOADRCFG_CSMON_PARAM_LIST_RW_SIZE        / BOARDCFG_ADDR_WIDTH))     /*    0 kB */
#define BOADRCFG_CSMON_PARAM_NAME_RW_ADDR               (uint32_t)(BOADRCFG_CSMON_PARAM_NUMB_RW_ADDR          + (uint32_t)(BOADRCFG_CSMON_PARAM_NUMB_RW_SIZE        / BOARDCFG_ADDR_WIDTH))     /*    0 kB */
#define BOADRCFG_CSMON_PARAM_RSRV_RW_ADDR               (uint32_t)(BOADRCFG_CSMON_PARAM_NAME_RW_ADDR          + (uint32_t)(BOADRCFG_CSMON_PARAM_NAME_RW_SIZE        / BOARDCFG_ADDR_WIDTH))     /*   64 kB */
#define BOADRCFG_CSMON_PARAM_BUFF_RW_ADDR               (uint32_t)(BOADRCFG_CSMON_PARAM_RSRV_RW_ADDR          + (uint32_t)(BOADRCFG_CSMON_PARAM_RSRV_RW_SIZE        / BOARDCFG_ADDR_WIDTH))     /* 1536 kB */

#endif

/*
 *  Partition Name                      Partition Start Address     Partition Size      Description
 *
 *  CSMON_PARAMETER_STRING_TABLE        0x00100000                  128 kWords          Archived Data Used For The PC Application
 *  CSMON_PARAMETER_AUXILIARY_TABLE     0x00120000                   25 kWords          Memory Segments Partitioning and Additional String Data
 *  MODBUS_REGISTER_ADDRESS_TABLE_LIST  0x00126400                    3 kWords          1024 Parameters *  3 Words      Virtual Address
 *  MODBUS_REGISTER_ADDRESS_TABLE_ADDR  0x00127000                    4 kWords          1024 Parameters *  4 Words      Real Address
 *  CSMON_PARAMETER_ID_ACCESS_TABLE     0x00128000                    8 kWords          1024 Parameters *  8 Words
 *  CSMON_PARAMETER_MIN_MAX_SCALE_TABLE 0x0012A000                   24 kWords          1024 Parameters * 24 Words
 *  CSMON_PARAMETER_NAME_UNIT_TABLE     0x00130000                   32 kWords          1024 Parameters * 32 Words
 *  CSMON_PARAMETER_NAME_FREE_TABLE     0x00138000                   32 kWords          Not Used - Reserved for EnerDrive
 *  CSMON_TRANSIENT_RECORDER0_BUFFER    0x00140000                  256 kWords            20 Parameters *  2 Words * 6553.6 Samples
 *  CSMON_TRANSIENT_RECORDER1_BUFFER    0x00180000                  256 kWords            20 Parameters *  2 Words * 6553.6 Samples
 *  CSMON_TRANSIENT_RECORDER2_BUFFER    0x001C0000                  256 kWords            20 Parameters *  2 Words * 6553.6 Samples
 *
 * */






#define BOARDCFG_EMIF1_CS2_START_ADDRESS    0x00100000
#define BOARDCFG_EMIF1_CS3_START_ADDRESS    0x00300000
#define BOARDCFG_EMIF1_CS4_START_ADDRESS    0x00380000

#define BOARDCFG_EMIF1_CS2_LENGTH           0x00200000
#define BOARDCFG_EMIF1_CS3_LENGTH           0x00080000
#define BOARDCFG_EMIF1_CS4_LENGTH           0x00060000


#define BOARDCFG_EMIF2_CS0_START_ADDRESS    0x90000000
#define BOARDCFG_EMIF2_CS2_START_ADDRESS    0x00002000

//#define BOARDCFG_EMIF2_CS0_LENGTH         0x02000000   // max
#define BOARDCFG_EMIF2_CS0_LENGTH           0x00100000   // FPGA
#define BOARDCFG_EMIF2_CS2_LENGTH           0x00001000



/* Global Configuration */


#define BOARDCFG_CSMON_PROCESS_CALL_HMLIB               1

#define BOARDCFG_USE_FPGA_SCI                           1
#define BOARDCFG_IFBK_147_STACK_STYLE                   1           /* if 1 Stack1 is IFBK 147 (first FPGA ver_00E); if 0 Stack1 IFBK is 123 (last FPGA ver_00Dzz2) */

#define BOARDCFG_RW_NON_VOLATILE_MEMORY_SIZE_EEPROM_KB  (uint32_t)(BOADRCFG_CSMON_PARAM_STRG_RW_SIZE / 1024) /* PROJECTCFG_RW_NON_VOLATILE_MEMORY_SIZE_EEPROM_KB */

#define BOARDCFG_USE_ANDREAS_INTERFACE_RTC              0
#define BOARDCFG_USE_ANDREAS_SCI_INIT                   0

#ifndef PROJECTCFG_RECORDERS_IN_FLAG_PROCESS_PASSED_INTERFACE
#define BOARDCFG_RECORDERS_IN_FLAG_PROCESS_PASSED_INTERFACE    0
#else
#define BOARDCFG_RECORDERS_IN_FLAG_PROCESS_PASSED_INTERFACE    PROJECTCFG_RECORDERS_IN_FLAG_PROCESS_PASSED_INTERFACE
#endif

#ifdef PROJECTCFG_CSMON_RTC_INTERFACE_THROUGH_FPGA_SCI
#define CSMON_vSetDateTime FPGA_SCI_vSetDateTime
#define CSMON_vGetDateTime FPGA_SCI_vGetDateTime
#endif

#ifndef PROJECTCFG_FREE_RUN_TIMER_EXTERNAL
#define BOARDCFG_FREE_RUN_TIMER_INTERNAL    1
#else
#if PROJECTCFG_FREE_RUN_TIMER_EXTERNAL
#define BOARDCFG_FREE_RUN_TIMER_INTERNAL    0
#else
#define BOARDCFG_FREE_RUN_TIMER_INTERNAL    1
#endif
#endif


/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* MODBUS READ SERVER ID */
#if (BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC)

	#define BOARDCFG_VENDORID_BYTE0     'C'
	#define BOARDCFG_VENDORID_BYTE1     'S'
	#define BOARDCFG_VENDORID_BYTE2     ' '
	#define BOARDCFG_VENDORID_BYTE3     ' '
	#define BOARDCFG_PRODUCTID_BYTE0    '1'
	#define BOARDCFG_PRODUCTID_BYTE1    '1'
	#define BOARDCFG_PRODUCTID_BYTE2    '0'
	#define BOARDCFG_PRODUCTID_BYTE3    '7'
#elif (BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG)
	#define BOARDCFG_VENDORID_BYTE0     'C'
	#define BOARDCFG_VENDORID_BYTE1     'S'
	#define BOARDCFG_VENDORID_BYTE2     ' '
	#define BOARDCFG_VENDORID_BYTE3     ' '
	#define BOARDCFG_PRODUCTID_BYTE0    '1'
	#define BOARDCFG_PRODUCTID_BYTE1    '0'
	#define BOARDCFG_PRODUCTID_BYTE2    '3'
	#define BOARDCFG_PRODUCTID_BYTE3    '8'
#endif

#define BOARDCFG_SERIALNUM_BYTE0    0x03
#define BOARDCFG_SERIALNUM_BYTE1    0x90
#define BOARDCFG_SERIALNUM_BYTE2    0xFF
#define BOARDCFG_SERIALNUM_BYTE3    0x01
#define BOARDCFG_SERIALNUM_BYTE4    0x00
#define BOARDCFG_SERIALNUM_BYTE5    0x00
#define BOARDCFG_SERIALNUM_BYTE6    0x00
#define BOARDCFG_SERIALNUM_BYTE7    0x00

#define BOARDCFG_SERVERID_NUMBER    0x00
#define BOARDCFG_SERVERID_LENGTH    16

#define BOARDCFG_VENDOR_NAME            "CONVESYS"
#define BOARDCFG_PRODUCT_CODE           "SCC"
#define BOARDCFG_MAJOR_MINOR_REVISION   "1.0"                   /* For Device (Hardware) */
#define BOARDCFG_VENDOR_URL             "www.convesys.com"
#define BOARDCFG_PRODUCT_NAME           "SCC-1500"
#define BOARDCFG_MODEL_NAME             "SCC-1500-00"
#define BOARDCFG_USERAPPLICATION_NAME   "SolarConvControl"
#define BOARDCFG_USERRESERVE0_NAME      "Reserve0"
#define BOARDCFG_USERRESERVE1_NAME      "Reserve1"
#define BOARDCFG_USERRESERVE2_NAME      "Reserve2"

#define MODBUSREG_DEVICE_ID_VENDORNAME_LENGTH           16
#define MODBUSREG_DEVICE_ID_PRODUCTCODE_LENGTH          16
#define MODBUSREG_DEVICE_ID_MAJORMINORREVISION_LENGTH   16
#define MODBUSREG_DEVICE_ID_VENDORURL_LENGTH            16
#define MODBUSREG_DEVICE_ID_PRODUCTNAME_LENGTH          16
#define MODBUSREG_DEVICE_ID_MODELNAME_LENGTH            16
#define MODBUSREG_DEVICE_ID_USERAPPLICATIONNAME_LENGTH  16
#define MODBUSREG_DEVICE_ID_USERRESERVE0_LENGTH         16
#define MODBUSREG_DEVICE_ID_USERRESERVE1_LENGTH         16
#define MODBUSREG_DEVICE_ID_USERRESERVE2_LENGTH         16




/* Control Limitations */
#define BOARDCFG_CURRENT_REQUEST_MAX_A              (1800.0)
//#define BOARDCFG_VOLTAGE_REQUEST_MAX_V              (1200.0)
#define BOARDCFG_FIELD_FREQUENCY_REQUEST_MAX_HZ     (  70.0)
#define BOARDCFG_FIELD_FREQUENCY_REQUEST_MIN_HZ     (   5.0)
#define BOARDCFG_PWM_FREQUENCY_REQUEST_MAX_HZ       (3500.0)
#define BOARDCFG_PWM_FREQUENCY_REQUEST_MIN_HZ       (1500.0)



#define BOARDCFG_UDCL_MIN_PRECHARGE  200  /*  80V (0.1 V Resolution) */
#define BOARDCFG_UDCL_MAX_PRECHARGE 9000  /* 900V (0.1 V Resolution) */

#define BOARDCFG_VOLTAGE_DCLINK_REQUEST_V  (BOARDCFG_UDCL_MIN_PRECHARGE/10 + 5.0)  /*  85V (1 V Resolution) */

#define BOARDCFG_TRIP_TEMPERATURE_DEG        (  80.0)
#define BOARDCFG_TRIP_HUMIDITY_PERCENT       (  80.0)

#if BOARDCFG_USE_FPGA_SCI
#define BOARDCFG_USE_ERROR_RISE_FPGA_SCI            1   /* Communication  Rise Errors */
#define BOARDCFG_USE_ERROR_RISE_FPGA_FLT            0   /* Digital Inputs Rise Errors */
#else
#define BOARDCFG_USE_ERROR_RISE_FPGA_SCI            0   /* Communication  Rise Errors */
#define BOARDCFG_USE_ERROR_RISE_FPGA_FLT            1   /* Digital Inputs Rise Errors */
#endif


#define BOARDCFG_USE_TEMPERATURE_MEASUREMENT_TRIP   0

#define BOARDCFG_USE_DAC_AUTOMATION 1

#define BOARDCFG_USE_TIMER_AND_UART_FUNCTIONALITY   1

#define BOARDCFG_USE_FPGA       1

#define BOARDCFG_USE_FPGA_TRIP  0       /* Gate Driver Error and Detection included */

#define BOARDCFG_USE_FPGA_DIO   0

#define BOARDCFG_USE_DIO        0       /* use IO processing - mainly in main loop */

#define BOARDCFG_USE_TSC_DSPIC  0

#define BOARDCFG_USE_TESTPROC   1

#define BOARDCFG_USE_I2C_RTC    0

#define BOARDCFG_VOLTAGE_STACK1_OFFSET_CALC_AUTO_MANIPULATION   0

#define FCY_HZ          		PROJECTCFG_FCY_HZ


#if BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC
    #define BOARDCFG_EMIF_START_ADDRESS                     BOARDCFG_EMIF1_CS2_START_ADDRESS
    #define BOARDCFG_EMIF_LENGTH                            BOARDCFG_EMIF1_CS2_LENGTH
	#ifndef EMIF_2_SDRAM
		#define BOARDCFG_EMIF2_START_ADDRESS                    BOARDCFG_EMIF2_CS2_START_ADDRESS
		#define BOARDCFG_EMIF2_LENGTH                           BOARDCFG_EMIF2_CS2_LENGTH
	#else
		#define BOARDCFG_EMIF2_START_ADDRESS                    BOARDCFG_EMIF2_CS0_START_ADDRESS
		#define BOARDCFG_EMIF2_LENGTH                           BOARDCFG_EMIF2_CS0_LENGTH
	#endif
#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG
    #define BOARDCFG_EMIF_START_ADDRESS                     BOARDCFG_EMIF1_CS2_START_ADDRESS
    #define BOARDCFG_EMIF_LENGTH                            BOARDCFG_EMIF1_CS2_LENGTH
    #define BOARDCFG_EMIF2_START_ADDRESS                    BOARDCFG_EMIF2_CS2_START_ADDRESS
    #define BOARDCFG_EMIF2_LENGTH                           BOARDCFG_EMIF2_CS2_LENGTH
#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_SMALL_TEST
    #define BOARDCFG_EMIF_START_ADDRESS                     BOARDCFG_EMIF1_CS2_START_ADDRESS
    #define BOARDCFG_EMIF_LENGTH                            BOARDCFG_EMIF1_CS2_LENGTH
    //#define BOARDCFG_EMIF_START_ADDRESS                     BOARDCFG_EMIF1_CS3_START_ADDRESS
    //#define BOARDCFG_EMIF_LENGTH                            BOARDCFG_EMIF1_CS3_LENGTH
    #define BOARDCFG_EMIF2_START_ADDRESS                    BOARDCFG_EMIF2_CS2_START_ADDRESS
    #define BOARDCFG_EMIF2_LENGTH                           BOARDCFG_EMIF2_CS2_LENGTH
#endif




#if BOARDCFG_USE_FPGA_SCI
#define BOARDCFG_USE_FPGA_SCI_C_RX   1  /* mix with FLT4 */
#define BOARDCFG_USE_FPGA_SCI_C_TX   1  /* mix with FLT5 */
#else
#define BOARDCFG_USE_FPGA_SCI_C_RX   0  /* mix with FLT4 */
#define BOARDCFG_USE_FPGA_SCI_C_TX   0  /* mix with FLT5 */
#endif


#ifdef PROJECTCFG_TIMER_MAIN_USEC
#define TIMER_MAIN_USEC         PROJECTCFG_TIMER_MAIN_USEC
#endif

#ifndef TIMER_MAIN_USEC
#define TIMER_MAIN_USEC         100.0
#endif

#define BOARDCFG_START_WITH_CALIBRATION_FIRST_AFTER_RESET	1

#define BOARDCFG_TESTPROC_IO_FBK_EMULATION                  1
#define BOARDCFG_IO_FBK_EMUALTION                           0

#define BOARDCFG_UDCL_DEF_REQUEST   1000  /* 100V (0.1 V Resolution) */

#define BOARDCFG_PWM_FREQUENCY_HZ    2500            /* Global Set PWM Frequency in Hz (default 2500Hz) */



/* *****************************************************************************
 * Pinout Configuration Definitions
 **************************************************************************** */
#define DI_ESTOP                                            DI1_PIN
#define DI_EXTERNAL_ESTOP                                   DI2_PIN
#define DI_MAINS_CIRCUIT_BREAKER_TRIP                       DI3_PIN
#define DI_MAINS_CIRCUIT_BREAKER_FEEDBACK                   DI4_PIN
#define DI_ESTOP_CHAIN                                      DI5_PIN
#define DI_PV_INPUTS_FUSES_FEEDBACK                         DI6_PIN
#define DI_AC_CONTACTOR1_FEEDBACK                           DI7_PIN
#define DI_AC_CONTACTOR2_FEEDBACK                           DI8_PIN
#define DI_AC_CONTACTOR3_FEEDBACK                           DI9_PIN
#define DI_PRECHARGE_BUTTON_FEEDBACK                        DI10_PIN
#define DI_DC_PRECHARGE_CONTACTOR_FEEDBACK                  DI11_PIN
#define DI_INSULATION_MONITOR_WARNING                       DI12_PIN
#define DI_AC_CHOCKES_OVERTEMPERATURE                       DI13_PIN
#define DI_SWITCH_DISCONNECTOR_FEEDBACK                     DI14_PIN
#define DI_DC_SUPPLIES_PLUS_AUX_MCB_FEEDBACK                DI15_PIN
#define DI_3_WAY_VALVE_POSITION_FEEDBACK                    DI16_PIN    /* High Level at Open Position */
#define DI_SURGE_ARRESTORS_FEEDBACK                         DI17_PIN
#define DI_HEAT_EXCHANGER_CIRCUIT_BREAKER_FEEDBACK          DI18_PIN
#define DI_COOLING_PUMP_CIRCUIT_BREAKER_FEEDBACK            DI19_PIN
#define DI_HEAT_EXCHANGER_CONTACTOR_FEEDBACK                DI20_PIN
#define DI_COOLING_PUMP_CONTACTOR_FEEDBACK                  DI21_PIN
#define DI_ENABLE_DISABLE_CABINET                           DI22_PIN
#define DI_TRANSFORMER_READY_EXTERNAL                       DI23_PIN
#define DI_EXTERNAL_GRID_MONITOR_FEEDBACK                   DI24_PIN

#define DO_AC1_CONTACTOR_CONTROL                            DO1_PIN
#define DO_AC2_CONTACTOR_CONTROL                            DO2_PIN
#define DO_AC3_CONTACTOR_CONTROL                            DO3_PIN
#define DO_DC_SWITCH_DISCONNECTOR_CONTROL                   DO4_PIN
#define DO_DC_PRECHARGE_CONTACTOR_CONTROL                   DO5_PIN
#define DO_STATUS_LAMP_DC_LINK_PRECHARGED                   DO6_PIN
#define DO_STATUS_LAMP_X1_CONTROL                           DO7_PIN
#define DO_STATUS_LAMP_X2_CONTROL                           DO8_PIN
#define DO_COOLING_PUMP_CONTROL                             DO10_PIN
#define DO_HEAT_EXCHANGER_FAN_CONTROL                       DO9_PIN
#define DO_FANS_CONTROL_SECTION1                            DO11_PIN
#define DO_FANS_CONTROL_SECTION2                            DO12_PIN
#define DO_HEATER_COOLING_LIQUID                            DO13_PIN
#define DO_3_WAY_VALVE_CONTROL                              DO14_PIN
#define DO_RESERVED1_CONTROL                                DO15_PIN
#define DO_RESERVED2_CONTROL                                DO16_PIN


#define bitPosIO(x) (x-1)
#define orMaskIO(x) (1 << bitPosIO(x))
#define andMskIO(x) (~(1 << bitPosIO(x)))
#define BOARDCFG_vSetIO(x)  {u16DigitalOutputs |= orMaskIO(x);}
#define BOARDCFG_vClrIO(x)  {u16DigitalOutputs &= andMskIO(x);}

#define BOARDCFG_vGetOutput(x)  (bool)((u16DigitalOutputs >> bitPosIO(x)) & (uint32_t)1)

#define BOARDCFG_vGetIO(x)  (bool)((u32DigitalInputs >> bitPosIO(x)) & (uint32_t)1)




#define DI_SIGNAL_ESTOP                                            1
#define DI_SIGNAL_EXTERNAL_ESTOP                                   2
#define DI_SIGNAL_MAINS_CIRCUIT_BREAKER_TRIP                       3
#define DI_SIGNAL_MAINS_CIRCUIT_BREAKER_FEEDBACK                   4
#define DI_SIGNAL_ESTOP_CHAIN                                      5
#define DI_SIGNAL_PV_INPUTS_FUSES_FEEDBACK                         6
#define DI_SIGNAL_AC_CONTACTOR1_FEEDBACK                           7
#define DI_SIGNAL_AC_CONTACTOR2_FEEDBACK                           8
#define DI_SIGNAL_AC_CONTACTOR3_FEEDBACK                           9
#define DI_SIGNAL_PRECHARGE_BUTTON_FEEDBACK                        10
#define DI_SIGNAL_DC_PRECHARGE_CONTACTOR_FEEDBACK                  11
#define DI_SIGNAL_INSULATION_MONITOR_WARNING                       12
#define DI_SIGNAL_AC_CHOCKES_OVERTEMPERATURE                       13
#define DI_SIGNAL_SWITCH_DISCONNECTOR_FEEDBACK                     14
#define DI_SIGNAL_DC_SUPPLIES_PLUS_AUX_MCB_FEEDBACK                15
#define DI_SIGNAL_3_WAY_VALVE_POSITION_FEEDBACK                    16    /* High Level at Open Position */
#define DI_SIGNAL_SURGE_ARRESTORS_FEEDBACK                         17
#define DI_SIGNAL_HEAT_EXCHANGER_CIRCUIT_BREAKER_FEEDBACK          18
#define DI_SIGNAL_COOLING_PUMP_CIRCUIT_BREAKER_FEEDBACK            19
#define DI_SIGNAL_HEAT_EXCHANGER_CONTACTOR_FEEDBACK                20
#define DI_SIGNAL_COOLING_PUMP_CONTACTOR_FEEDBACK                  21
#define DI_SIGNAL_ENABLE_DISABLE_CABINET                           22
#define DI_SIGNAL_TRANSFORMER_READY_EXTERNAL                       23
#define DI_SIGNAL_EXTERNAL_GRID_MONITOR_FEEDBACK                   24

#define DO_SIGNAL_AC1_CONTACTOR_CONTROL                            1
#define DO_SIGNAL_AC2_CONTACTOR_CONTROL                            2
#define DO_SIGNAL_AC3_CONTACTOR_CONTROL                            3
#define DO_SIGNAL_DC_SWITCH_DISCONNECTOR_CONTROL                   4
#define DO_SIGNAL_DC_PRECHARGE_CONTACTOR_CONTROL                   5
#define DO_SIGNAL_STATUS_LAMP_DC_LINK_PRECHARGED                   6
#define DO_SIGNAL_STATUS_LAMP_X1_CONTROL                           7
#define DO_SIGNAL_STATUS_LAMP_X2_CONTROL                           8
#define DO_SIGNAL_COOLING_PUMP_CONTROL                             10
#define DO_SIGNAL_HEAT_EXCHANGER_FAN_CONTROL                       9
#define DO_SIGNAL_FANS_CONTROL_SECTION1                            11
#define DO_SIGNAL_FANS_CONTROL_SECTION2                            12
#define DO_SIGNAL_HEATER_COOLING_LIQUID                            13
#define DO_SIGNAL_3_WAY_VALVE_CONTROL                              14
#define DO_SIGNAL_RESERVED1_CONTROL                                15
#define DO_SIGNAL_RESERVED2_CONTROL                                16

/* *****************************************************************************
 * PWM
 **************************************************************************** */
#define PWM_MIN_DUTY_COEF   (0.02F)
#define PWM_MAX_DUTY_COEF   (0.98F)
#define PWM_START_DUTY_COEF (0.50F)

#define FRAC_AMPLITUDE_PWM_MIN_DUTY_COEF   (0.00F)
#define FRAC_AMPLITUDE_PWM_MAX_DUTY_COEF   (1.00F)




/* *****************************************************************************
 * Analog Configuration
 **************************************************************************** */

/* ADC */
#if _LAUNCHXL_F28379D || defined (_LAUNCHXL_F28069M)
	#define BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS         (3.3F)  /* Demo Boards */
#else
	#if (BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC)
		#define BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS         (3.3F)  /* SCC Board 1107 */
	#elif (BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG)
		#define BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS         (3.0F)  /* SCC Board */
	#endif
#endif

#define BOARDCFG_ADC_RESULT_BITS                     16  /* 12 */


/* DAC */
#if _LAUNCHXL_F28379D || defined (_LAUNCHXL_F28069M)
	#define BOARDCFG_DAC_REFERENCE_VOLTAGE_VOLTS         (3.3F)  /* Demo Boards */
#elif (BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC)
	#define BOARDCFG_DAC_REFERENCE_VOLTAGE_VOLTS         (3.3F)  /* SCC Board 1107 */
#else
	#define BOARDCFG_DAC_REFERENCE_VOLTAGE_VOLTS         (3.0F)  /* SCC Board */
#endif

#define BOARDCFG_DAC_RESULT_BITS                     12



//#define ANALOG_CONDITIONING_COEF_INP_AMP_TO_ADCREF  (20.0F / BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS)          /* -10V to 10V input to 3V Analog Conditioning Coefficient */
//#define ANALOG_CONDITIONING_COEF_INPUTDC_TO_ADCREF  (10.0F / BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS)          /*   0V to 10V input to 3V Analog Conditioning Coefficient */
//#define ANALOG_CONDITIONING_COEF_INPUTLN_TO_ADCREF  (20.0F / BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS)          /* -10V to 10V input to 3V Analog Conditioning Coefficient */

#define ADCREF_TO_POSITIVE_SIGNAL_CONDITIONING_MULT ( ((6800.0 + 2200.0) / 2200.0) / ((1300.0 + 6800.0) / 6800.0) )      /* (4.(09)) / (1.19(1176470588235294)) = 3.4(34)     (DCLV1 - DCLV4)  */
#define ADCREF_TO_BI_POLAR_SIGNAL_CONDITIONING_MULT ( ((6800.0 + 1000.0) / 1000.0) / ((1000.0 + 6800.0) / 6800.0) )      /* (7.8)    / (1.1(470588235294117))   = 6.8         (LLV1 - LLV2) (IPH1 - IPH9) (DCLI1 - DCLI3) */
#define ADCREF_WITHOUT_ANY_SIGNAL_CONDITIONING_MULT ( ((1000.0 + 1000.0) / 2000.0) / ((1000.0 + 1000.0) / 2000.0) )      /* (1.0)    / (1.0)                    = 1.0         (LLV3 - LLV6) */

/* IPH */
#define CURRENT_SENSOR_MAX_AMPERES  1000.0
#define CURRENT_SENSOR_MIN_AMPERES -1000.0
#define CURRENT_SENSOR_MAX_VOLTAGE     4.0
#define CURRENT_SENSOR_MIN_VOLTAGE    -4.0
#define CURRENT_SENSOR_VOLTAGE_TO_AMPERES_MULT  ((CURRENT_SENSOR_MAX_AMPERES - CURRENT_SENSOR_MIN_AMPERES) / (CURRENT_SENSOR_MAX_VOLTAGE - CURRENT_SENSOR_MIN_VOLTAGE))
#define GATE_DRIVER_CURRENT_SENSOR_VOLTAGE_MULT     2.0         /* CS1006 V2 */

/* Name Compatibility */
#define BOARDCFG_GATE_DRIVER_CURRENT_SENSOR_VOLTAGE_MULT    GATE_DRIVER_CURRENT_SENSOR_VOLTAGE_MULT


/* Input Connector Voltage To Real Current Multipliers */
#define VOLTAGE_ON_CURRENT_INPUT_TO_AMPERES_MULT    (CURRENT_SENSOR_VOLTAGE_TO_AMPERES_MULT / GATE_DRIVER_CURRENT_SENSOR_VOLTAGE_MULT)    /* Amperes To Voltage (A/V) on the +/-10V Input */


//fix below not  checked on TI

//#define CURRENT_FEEDBACK_FIX_MULTIPLIER_CONTROL     (0.25F)
#define CURRENT_FEEDBACK_FIX_MULT_SHIFT_CONTROL     (2)                         /* How Many Times To shift right ADC Value */
//#define CURRENT_FEEDBACK_FIX_MULTIPLIER_STACK1      (0.25F)
#define CURRENT_FEEDBACK_FIX_MULT_SHIFT_STACK1      (CURRENT_FEEDBACK_FIX_MULT_SHIFT_CONTROL)                         /* How Many Times To shift right ADC Value */
//#define CURRENT_FEEDBACK_FIX_MULTIPLIER_STACK2      (0.25F)
#define CURRENT_FEEDBACK_FIX_MULT_SHIFT_STACK2      (CURRENT_FEEDBACK_FIX_MULT_SHIFT_CONTROL)                         /* How Many Times To shift right ADC Value */
//#define CURRENT_FEEDBACK_FIX_MULTIPLIER_STACK3      (0.25F)
#define CURRENT_FEEDBACK_FIX_MULT_SHIFT_STACK3      (CURRENT_FEEDBACK_FIX_MULT_SHIFT_CONTROL)                         /* How Many Times To shift right ADC Value */


/* DCLI */
#define CUR_DCL_SENSOR_MAX_AMPERES  1500.0
#define CUR_DCL_SENSOR_MIN_AMPERES -1500.0
#define CUR_DCL_SENSOR_MAX_VOLTAGE     4.0
#define CUR_DCL_SENSOR_MIN_VOLTAGE    -4.0
#define CUR_DCL_SENSOR_VOLTAGE_TO_AMPERES_MULT  ((CUR_DCL_SENSOR_MAX_AMPERES - CUR_DCL_SENSOR_MIN_AMPERES) / (CUR_DCL_SENSOR_MAX_VOLTAGE - CUR_DCL_SENSOR_MIN_VOLTAGE))
#define GATE_DRIVER_CUR_DCL_SENSOR_VOLTAGE_MULT     1.0         /* No Gate Driver or Signal Conditioning */


/* Input Connector Voltage To Real Current Multipliers */
#define VOLTAGE_ON_CUR_DCL_INPUT_TO_AMPERES_MULT    (CUR_DCL_SENSOR_VOLTAGE_TO_AMPERES_MULT / GATE_DRIVER_CUR_DCL_SENSOR_VOLTAGE_MULT)    /* Amperes To Voltage (A/V) on the +/-10V Input */
#define CUR_DCL_SENSOR_10V_AMPERES  (VOLTAGE_ON_CUR_DCL_INPUT_TO_AMPERES_MULT * 10)

/* Pressure Sensor 0..4 Bar -> 4..20 mA; 497.94838716 Ohm for the Current To Voltage Conversion */
#define PRESSURE_CURRENT_TO_VOLTAGE_RESISTOR_OHM      280.0
#define PRESSURE_CURRENT_TO_VOLTAGE_COEF_OPAMP          1.778387097
#define PRESSURE_CURRENT_TO_VOLTAGE_COEF              (PRESSURE_CURRENT_TO_VOLTAGE_COEF_OPAMP * PRESSURE_CURRENT_TO_VOLTAGE_RESISTOR_OHM)        /* 1.778387097(Coefficient Opamp) * 280.0(Shunt Resistance) = 497.9483871 Ohm */
#define PRESSURE_SENSOR_MAX_BAR                         4.0
#define PRESSURE_SENSOR_MIN_BAR                         0.0
#define PRESSURE_SENSOR_MAX_MILLIAMPERES               20.0
#define PRESSURE_SENSOR_MIN_MILLIAMPERES                4.0

/* Pressure Related Calculations */
#define PRESSURE_VOLTAGE_TO_MILLIAMPERES_MULT           (1000.0 / PRESSURE_CURRENT_TO_VOLTAGE_COEF)
#define PRESSURE_MILLIAMPERES_TO_BAR_MULT               (PRESSURE_SENSOR_MAX_BAR - PRESSURE_SENSOR_MIN_BAR) / (PRESSURE_SENSOR_MAX_MILLIAMPERES - PRESSURE_SENSOR_MIN_MILLIAMPERES)
#define PRESSURE_MILLIAMPERES_OFFS                      PRESSURE_SENSOR_MIN_MILLIAMPERES
#define PRESSURE_BAR_OFFS                               (PRESSURE_SENSOR_MIN_MILLIAMPERES * PRESSURE_MILLIAMPERES_TO_BAR_MULT)  /* Offset that must be subtracted from the multiplied value */
#define VOLTAGE_ON_PRESSURECONNECTOR_TO_BAR_MULT        (PRESSURE_VOLTAGE_TO_MILLIAMPERES_MULT * PRESSURE_MILLIAMPERES_TO_BAR_MULT)





/* DCLink and PV Voltage 0..1500V  -> 0..25 mA; 400 Ohm for the Current To Voltage Conversion */
#define USE_BORETO_UFB_SENSOR 0

#if USE_BORETO_UFB_SENSOR == 0
#define VOLTAGE_DC_SENSOR_MAX_VOLTAGE_USED                1500.0
#define VOLTAGE_DC_SENSOR_MAX_MILLIAMPERES_USED             20.0
#else
#define VOLTAGE_DC_SENSOR_MAX_VOLTAGE_USED                1700.0
#define VOLTAGE_DC_SENSOR_MAX_MILLIAMPERES_USED             24.75866667
#endif


#define VOLTAGE_DC_CURRENT_TO_VOLTAGE_RESISTOR_OHM    150.0
#define VOLTAGE_DC_CURRENT_TO_VOLTAGE_COEF_OPAMP        2.646666667
#define VOLTAGE_DC_CURRENT_TO_VOLTAGE_COEF            (VOLTAGE_DC_CURRENT_TO_VOLTAGE_COEF_OPAMP * VOLTAGE_DC_CURRENT_TO_VOLTAGE_RESISTOR_OHM)        /* 2.646666667(Coefficient Opamp) * 150.0(Shunt Resistance) = 397 Ohm */
#define VOLTAGE_DC_SENSOR_MAX_VOLTAGE                 VOLTAGE_DC_SENSOR_MAX_VOLTAGE_USED
#define VOLTAGE_DC_SENSOR_MIN_VOLTAGE                   0.0
#define VOLTAGE_DC_SENSOR_MAX_MILLIAMPERES            VOLTAGE_DC_SENSOR_MAX_MILLIAMPERES_USED
#define VOLTAGE_DC_SENSOR_MIN_MILLIAMPERES              0.0

/* DCLink and PV Voltage Related Calculations */
#define VOLTAGE_DC_VOLTAGE_TO_MILLIAMPERES_MULT         (1000.0 / VOLTAGE_DC_CURRENT_TO_VOLTAGE_COEF)
#define VOLTAGE_DC_MILLIAMPERES_TO_VOLT_MULT            (VOLTAGE_DC_SENSOR_MAX_VOLTAGE - VOLTAGE_DC_SENSOR_MIN_VOLTAGE) / (VOLTAGE_DC_SENSOR_MAX_MILLIAMPERES - VOLTAGE_DC_SENSOR_MIN_MILLIAMPERES)
#define VOLTAGE_ON_VOLTAGE_INPUT_DC_TO_VOLT_MULT        (VOLTAGE_DC_VOLTAGE_TO_MILLIAMPERES_MULT * VOLTAGE_DC_MILLIAMPERES_TO_VOLT_MULT)




/* Line Voltage 0..1500V  -> 0..25 mA; 400 Ohm for the Current To Voltage Conversion */
#define VOLTAGE_LN_CURRENT_TO_VOLTAGE_RESISTOR_OHM    150.0
#define VOLTAGE_LN_CURRENT_TO_VOLTAGE_COEF_OPAMP        2.646666667
#define VOLTAGE_LN_CURRENT_TO_VOLTAGE_COEF            (VOLTAGE_LN_CURRENT_TO_VOLTAGE_COEF_OPAMP * VOLTAGE_LN_CURRENT_TO_VOLTAGE_RESISTOR_OHM)        /* 2.646666667(Coefficient Opamp) * 150.0(Shunt Resistance) = 397 Ohm */
#define VOLTAGE_LN_SENSOR_MAX_VOLTAGE                 800.0
#define VOLTAGE_LN_SENSOR_MIN_VOLTAGE                   0.0
#define VOLTAGE_LN_SENSOR_MAX_MILLIAMPERES             25.0
#define VOLTAGE_LN_SENSOR_MIN_MILLIAMPERES              0.0

/* DCLink and PV Voltage Related Calculations */
#define VOLTAGE_LN_VOLTAGE_TO_MILLIAMPERES_MULT         (1000.0 / VOLTAGE_LN_CURRENT_TO_VOLTAGE_COEF)
#define VOLTAGE_LN_MILLIAMPERES_TO_VOLT_MULT            (VOLTAGE_LN_SENSOR_MAX_VOLTAGE - VOLTAGE_LN_SENSOR_MIN_VOLTAGE) / (VOLTAGE_LN_SENSOR_MAX_MILLIAMPERES - VOLTAGE_LN_SENSOR_MIN_MILLIAMPERES)
#define VOLTAGE_ON_VOLTAGE_INPUT_LN_TO_VOLT_MULT        (VOLTAGE_LN_VOLTAGE_TO_MILLIAMPERES_MULT * VOLTAGE_LN_MILLIAMPERES_TO_VOLT_MULT)

/* 2k Pull-up Resistor to 3.3V */
#define NTC_PULLUP_RESISTOR_OHM                         2000.0
#define NTC_PULLUP_VOLTAGE_VOLT                         3.3

/* y = -41.7225413467x + 138.8272039243 (25-85 oC Trend Line) */
#define NTC_EPCOS_VOLTAGE_TO_DEG_MULT                   (-41.7225413467)
#define NTC_EPCOS_VOLTAGE_TO_DEG_OFFS                   (138.8272039243)

/* y = -35.8386449620x + 122.8305770470 (25-85 oC Trend Line) */
#define NTC_TEWA__VOLTAGE_TO_DEG_MULT                   (-35.8386449620)
#define NTC_TEWA__VOLTAGE_TO_DEG_OFFS                   (122.8305770470)



#define USE_TEWA__FOR_AIR    1
#define USE_EPCOS_FOR_H2O    1

#if USE_TEWA__FOR_AIR
#define VOLTAGE_ON_TEMP_AIRFLW_INPUT_TO_DEG_MULT        NTC_TEWA__VOLTAGE_TO_DEG_MULT
#define VOLTAGE_ON_TEMP_AIRFLW_INPUT_TO_DEG_OFFS        NTC_TEWA__VOLTAGE_TO_DEG_OFFS
#else
#define VOLTAGE_ON_TEMP_AIRFLW_INPUT_TO_DEG_MULT        NTC_EPCOS_VOLTAGE_TO_DEG_MULT
#define VOLTAGE_ON_TEMP_AIRFLW_INPUT_TO_DEG_OFFS        NTC_EPCOS_VOLTAGE_TO_DEG_OFFS
#endif

#if USE_EPCOS_FOR_H2O == 0
#define VOLTAGE_ON_TEMP_LIQUID_INPUT_TO_DEG_MULT        NTC_TEWA__VOLTAGE_TO_DEG_MULT
#define VOLTAGE_ON_TEMP_LIQUID_INPUT_TO_DEG_OFFS        NTC_TEWA__VOLTAGE_TO_DEG_OFFS
#else
#define VOLTAGE_ON_TEMP_LIQUID_INPUT_TO_DEG_MULT        NTC_EPCOS_VOLTAGE_TO_DEG_MULT
#define VOLTAGE_ON_TEMP_LIQUID_INPUT_TO_DEG_OFFS        NTC_EPCOS_VOLTAGE_TO_DEG_OFFS
#endif




/* ADC Scale To Input Connector Voltage Multipliers */
#define ADC_SCALE_TO_AMP_IFBK_INPUT_VOLTAGE_MULT         (BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS * ADCREF_TO_BI_POLAR_SIGNAL_CONDITIONING_MULT)
#define ADC_SCALE_TO_VOLTS_DC_INPUT_VOLTAGE_MULT         (BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS * ADCREF_TO_POSITIVE_SIGNAL_CONDITIONING_MULT)
#define ADC_SCALE_TO_VOLTS_LN_INPUT_VOLTAGE_MULT         (BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS * ADCREF_TO_BI_POLAR_SIGNAL_CONDITIONING_MULT)
#define ADC_SCALE_TO_PRESSURE_INPUT_VOLTAGE_MULT         (BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS * ADCREF_TO_POSITIVE_SIGNAL_CONDITIONING_MULT)
#define ADC_SCALE_TO_AMP_DCLI_INPUT_VOLTAGE_MULT         (BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS * ADCREF_TO_BI_POLAR_SIGNAL_CONDITIONING_MULT)
#define ADC_SCALE_TO_TEMP_H2O_INPUT_VOLTAGE_MULT         (BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS * ADCREF_WITHOUT_ANY_SIGNAL_CONDITIONING_MULT)
#define ADC_SCALE_TO_TEMP_AIR_INPUT_VOLTAGE_MULT         (BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS * ADCREF_WITHOUT_ANY_SIGNAL_CONDITIONING_MULT)

/* ADC Result To Input Connector Voltage Multipliers */
#define ADC_RESULT_TO_AMP_IFBK_INPUT_VOLTAGE_MULT        (ADC_SCALE_TO_AMP_IFBK_INPUT_VOLTAGE_MULT / ((uint32_t)1 << BOARDCFG_ADC_RESULT_BITS))
#define ADC_RESULT_TO_VOLTS_DC_INPUT_VOLTAGE_MULT        (ADC_SCALE_TO_VOLTS_DC_INPUT_VOLTAGE_MULT / ((uint32_t)1 << BOARDCFG_ADC_RESULT_BITS))
#define ADC_RESULT_TO_VOLTS_LN_INPUT_VOLTAGE_MULT        (ADC_SCALE_TO_VOLTS_LN_INPUT_VOLTAGE_MULT / ((uint32_t)1 << BOARDCFG_ADC_RESULT_BITS))
#define ADC_RESULT_TO_PRESSURE_INPUT_VOLTAGE_MULT        (ADC_SCALE_TO_PRESSURE_INPUT_VOLTAGE_MULT / ((uint32_t)1 << BOARDCFG_ADC_RESULT_BITS))
#define ADC_RESULT_TO_AMP_DCLI_INPUT_VOLTAGE_MULT        (ADC_SCALE_TO_AMP_DCLI_INPUT_VOLTAGE_MULT / ((uint32_t)1 << BOARDCFG_ADC_RESULT_BITS))
#define ADC_RESULT_TO_TEMP_H2O_INPUT_VOLTAGE_MULT        (ADC_SCALE_TO_TEMP_H2O_INPUT_VOLTAGE_MULT / ((uint32_t)1 << BOARDCFG_ADC_RESULT_BITS))
#define ADC_RESULT_TO_TEMP_AIR_INPUT_VOLTAGE_MULT        (ADC_SCALE_TO_TEMP_AIR_INPUT_VOLTAGE_MULT / ((uint32_t)1 << BOARDCFG_ADC_RESULT_BITS))

/* Name Compatibility */
#define BOARDCFG_ADC_RESULT_TO_10V_IFBK_MULT            ADC_RESULT_TO_AMP_IFBK_INPUT_VOLTAGE_MULT



/* ADC Scale To Real Voltage/Current Values Multipliers */
#define ADC_SCALE_TO_AMP_IFBK_MULT          (BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS * ADCREF_TO_BI_POLAR_SIGNAL_CONDITIONING_MULT * VOLTAGE_ON_CURRENT_INPUT_TO_AMPERES_MULT) /* IPH1  - IPH9  */
#define ADC_SCALE_TO_VOLTS_DC_MULT          (BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS * ADCREF_TO_POSITIVE_SIGNAL_CONDITIONING_MULT * VOLTAGE_ON_VOLTAGE_INPUT_DC_TO_VOLT_MULT) /* DCLV1 , DCLV4 */
#define ADC_SCALE_TO_VOLTS_LN_MULT          (BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS * ADCREF_TO_BI_POLAR_SIGNAL_CONDITIONING_MULT * VOLTAGE_ON_VOLTAGE_INPUT_LN_TO_VOLT_MULT) /* LLV1  - LLV2  */
#define ADC_SCALE_TO_PRESSURE_MULT          (BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS * ADCREF_TO_POSITIVE_SIGNAL_CONDITIONING_MULT * VOLTAGE_ON_PRESSURECONNECTOR_TO_BAR_MULT) /* DCLV2 , DCLV3 */
#define ADC_SCALE_TO_AMP_DCLI_MULT          (BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS * ADCREF_TO_BI_POLAR_SIGNAL_CONDITIONING_MULT * VOLTAGE_ON_CUR_DCL_INPUT_TO_AMPERES_MULT) /* DCLI1 - DCLI3 */
#define ADC_SCALE_TO_TEMP_H2O_MULT          (BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS * ADCREF_WITHOUT_ANY_SIGNAL_CONDITIONING_MULT * VOLTAGE_ON_TEMP_LIQUID_INPUT_TO_DEG_MULT) /* LLV3  - LLV4  */
#define ADC_SCALE_TO_TEMP_AIR_MULT          (BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS * ADCREF_WITHOUT_ANY_SIGNAL_CONDITIONING_MULT * VOLTAGE_ON_TEMP_AIRFLW_INPUT_TO_DEG_MULT) /* LLV5  - LLV6  */

#define ADC_SCALE_TO_TEMP_H2O_OFFS          (BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS * ADCREF_WITHOUT_ANY_SIGNAL_CONDITIONING_MULT * VOLTAGE_ON_TEMP_LIQUID_INPUT_TO_DEG_OFFS) /* LLV3  - LLV4  */
#define ADC_SCALE_TO_TEMP_AIR_OFFS          (BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS * ADCREF_WITHOUT_ANY_SIGNAL_CONDITIONING_MULT * VOLTAGE_ON_TEMP_AIRFLW_INPUT_TO_DEG_OFFS) /* LLV5  - LLV6  */

/* ADC Result To Real Voltage/Current Values Multipliers */
#define ADC_RESULT_TO_AMP_IFBK_MULT         ((BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS * ADCREF_TO_BI_POLAR_SIGNAL_CONDITIONING_MULT * VOLTAGE_ON_CURRENT_INPUT_TO_AMPERES_MULT) / ((uint32_t)1 << BOARDCFG_ADC_RESULT_BITS)) /* IPH1  - IPH9  */
#define ADC_RESULT_TO_VOLTS_DC_MULT         ((BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS * ADCREF_TO_POSITIVE_SIGNAL_CONDITIONING_MULT * VOLTAGE_ON_VOLTAGE_INPUT_DC_TO_VOLT_MULT) / ((uint32_t)1 << BOARDCFG_ADC_RESULT_BITS)) /* DCLV1 , DCLV4 */
#define ADC_RESULT_TO_VOLTS_LN_MULT         ((BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS * ADCREF_TO_BI_POLAR_SIGNAL_CONDITIONING_MULT * VOLTAGE_ON_VOLTAGE_INPUT_LN_TO_VOLT_MULT) / ((uint32_t)1 << BOARDCFG_ADC_RESULT_BITS)) /* LLV1  - LLV2  */
#define ADC_RESULT_TO_PRESSURE_MULT         ((BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS * ADCREF_TO_POSITIVE_SIGNAL_CONDITIONING_MULT * VOLTAGE_ON_PRESSURECONNECTOR_TO_BAR_MULT) / ((uint32_t)1 << BOARDCFG_ADC_RESULT_BITS)) /* DCLV2 , DCLV3 */
#define ADC_RESULT_TO_AMP_DCLI_MULT         ((BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS * ADCREF_TO_BI_POLAR_SIGNAL_CONDITIONING_MULT * VOLTAGE_ON_CUR_DCL_INPUT_TO_AMPERES_MULT) / ((uint32_t)1 << BOARDCFG_ADC_RESULT_BITS)) /* DCLI1 - DCLI3 */
#define ADC_RESULT_TO_TEMP_H2O_MULT         ((BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS * ADCREF_WITHOUT_ANY_SIGNAL_CONDITIONING_MULT * VOLTAGE_ON_TEMP_LIQUID_INPUT_TO_DEG_MULT) / ((uint32_t)1 << BOARDCFG_ADC_RESULT_BITS)) /* LLV3  - LLV4  */
#define ADC_RESULT_TO_TEMP_AIR_MULT         ((BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS * ADCREF_WITHOUT_ANY_SIGNAL_CONDITIONING_MULT * VOLTAGE_ON_TEMP_AIRFLW_INPUT_TO_DEG_MULT) / ((uint32_t)1 << BOARDCFG_ADC_RESULT_BITS)) /* LLV5  - LLV6  */

#define ADC_RESULT_TO_TEMP_H2O_OFFS         ((BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS * ADCREF_WITHOUT_ANY_SIGNAL_CONDITIONING_MULT * VOLTAGE_ON_TEMP_LIQUID_INPUT_TO_DEG_OFFS) / ((uint32_t)1 << BOARDCFG_ADC_RESULT_BITS)) /* LLV3  - LLV4  */
#define ADC_RESULT_TO_TEMP_AIR_OFFS         ((BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS * ADCREF_WITHOUT_ANY_SIGNAL_CONDITIONING_MULT * VOLTAGE_ON_TEMP_AIRFLW_INPUT_TO_DEG_OFFS) / ((uint32_t)1 << BOARDCFG_ADC_RESULT_BITS)) /* LLV5  - LLV6  */

/* Compatible Naming */
#define BOARDCFG_ADC_RESULT_TO_DC_LINK_MULT         ADC_RESULT_TO_VOLTS_DC_MULT
#define BOARDCFG_ADC_RESULT_TO_IFBK_MULT            ADC_RESULT_TO_AMP_IFBK_MULT
#define BOARDCFG_ADC_RESULT_TO_UFBK_MULT            ADC_RESULT_TO_VOLTS_DC_MULT                     /* to be checked not sure */
#define BOARDCFG_ADC_RESULT_TO_VOLTAGE_10V_MULT     ADC_RESULT_TO_VOLTS_DC_INPUT_VOLTAGE_MULT
#define BOARDCFG_VOLTAGE_10V_TO_IFBK_MULT           ADC_RESULT_TO_AMP_IFBK_INPUT_VOLTAGE_MULT

/* Pressure Sensors Pinout */
#define SIGNAL_PRESSURE_INSIDE       DCLV_FBK2
#define SIGNAL_PRESSURE_OUTLET       DCLV_FBK3
#define SIGNAL_TEMP_H2O_INSIDE       LLV_FBK3   /* Suppose EPCOS */
#define SIGNAL_TEMP_H2O_OUTLET       LLV_FBK4
#define SIGNAL_TEMP_AIR_PRIMER       LLV_FBK5   /* Suppose TEWA */
#define SIGNAL_TEMP_AIR_SECOND       LLV_FBK6

/* *****************************************************************************
 * Analog and Not Only Configuration Definitions
 **************************************************************************** */

#define DEAD_TIME_CLOCK_FREQ_HZ             (100000000.0F)      /* FPGA Clock 100 MHz */
#define MIN_PULSE_CLOCK_FREQ_HZ             (100000000.0F)      /* FPGA Clock 100 MHz */

#define SPEED_RESOLUTION_HZ                 (0.05F)






#if 0

#define ANALOG_CONDITIONING_CONTROL_BOARD   ADCREF_TO_BI_POLAR_SIGNAL_CONDITIONING_MULT


    //below data to be evaluated and tested
//#define CFBK_AMPERES_PER_VOLT_2400A         (125.0F * (2400.0 / 1000.0)            /* this should be 2400A module and 2000A module */
//#define CFBK_AMPERES_PER_VOLT_1500A         (125.0F * (1500.0 / 1000.0)            /* this should be 1500A module */
//#define CFBK_AMPERES_PER_VOLT_1000A         (125.0F * (1000.0 / 1000.0)            /* this should be 1000A module - same as LSC */

//#define CFBK_MODULE_NOMINAL_AMPERES         1500
//#define CFBK_MODULE_NOMINAL_AMPERES         2400

//#define CFBK_AMPERES_PER_VOLT_LSC           (125.0F)            /* this should be 1000A module (ctreader 1,3125 coef) */
//#define CFBK_AMPERES_PER_VOLT_MSC           (187.5F)            /* this should be 1500A module */

//#define CFBK_AMPERES_PER_VOLT_BASE(x)       ((125.0F * x) / 1000.0)
#define CFBK_AMPERES_PER_VOLT_BASE(x)       (x / 8)             /* same as above */

//#define CFBK_AMPERES_PER_VOLT_DEF           (CFBK_AMPERES_PER_VOLT_LSC)
#define CFBK_AMPERES_PER_VOLT_DEF           CFBK_AMPERES_PER_VOLT_BASE(CFBK_MODULE_NOMINAL_AMPERES)
#endif

#define CFBK_MODULE_NOMINAL_AMPERES         CUR_DCL_SENSOR_10V_AMPERES
//was #define CFBK_AMPERES_PER_VOLT_BASE_MULT     (float)(125.0 / 1000.0)
#define CFBK_AMPERES_PER_VOLT_BASE_MULT     (1.0 / VOLTAGE_ON_CUR_DCL_INPUT_TO_AMPERES_MULT)



#define TFBK_USE_EPCOS 1

#if TFBK_USE_EPCOS
#define TFBK_DEGREES_PER_VOLT_DEF           ((85.0 - 25.0) / (1.297371012 - 2.702911424))   /* EPCOS - data from B equation */
#else
#define TFBK_DEGREES_PER_VOLT_DEF           ((85.0 - 25.0) / (1.080026476 - 2.704918033))   /* TEWA  - data from B equation */
#endif



#define TFBK_OFFSET_DEGREES                 (25.0 - TFBK_DEGREES_PER_VOLT_DEF)


#define ADC_RESULT_TO_AMP_CFBK_MULT     	ADC_RESULT_TO_AMP_DCLI_MULT

#define ADC_RESULT_TO_AMP_CFBK_MULT_BASE_1_AMPERE     ( (ADCREF_TO_BI_POLAR_SIGNAL_CONDITIONING_MULT) * ((BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS)/((uint32_t)1 << BOARDCFG_ADC_RESULT_BITS)) )

#define ADC_RESULT_TO_DEG_TFBK_MULT     	( (TFBK_DEGREES_PER_VOLT_DEF * ADCREF_WITHOUT_ANY_SIGNAL_CONDITIONING_MULT) * ((BOARDCFG_ADC_REFERENCE_VOLTAGE_VOLTS)/((uint32_t)1 << BOARDCFG_ADC_RESULT_BITS)) )

#define ADC_RESULT_TO_DEG_TFBK_OFFS     	TFBK_OFFSET_DEGREES


#define CURRENT_TO_DAC_VOLTAGE_AMPERES  	VOLTAGE_ON_CURRENT_INPUT_TO_AMPERES_MULT
#define VOLTAGE_TO_DAC_VOLTAGE_INPUTDC  	VOLTAGE_ON_VOLTAGE_INPUT_DC_TO_VOLT_MULT

/* Name Compatibility */
#define BOARDCFG_ADC_RESULT_TO_CFBK_MULT            ADC_RESULT_TO_AMP_CFBK_MULT
#define BOARDCFG_ADC_RESULT_TO_CFBK_MULT_BASE_1_AMP ADC_RESULT_TO_AMP_CFBK_MULT_BASE_1_AMPERE
#define BOARDCFG_CFBK_AMPERES_PER_VOLT_BASE_MULT    CFBK_AMPERES_PER_VOLT_BASE_MULT
#define BOARDCFG_CFBK_MODULE_NOMINAL_AMPERES        CFBK_MODULE_NOMINAL_AMPERES
#define BOARDCFG_ADC_RESULT_TO_TFBK_MULT            ADC_RESULT_TO_DEG_TFBK_MULT
#define BOARDCFG_ADC_RESULT_TO_TFBK_OFFS            ADC_RESULT_TO_DEG_TFBK_OFFS

#define DACREF_TO_BI_POLAR_SIGNAL_CONDITIONING_MULT   ((20000.0 + 7500.0) / 7500.0)       /* (DACA) */
#define DACREF_TO_POSITIVE_SIGNAL_CONDITIONING_MULT   ((20000.0 + 7500.0) / 7500.0)       /* (DACB) */


/* 0-3VDAC -> 0-10V Ref Comparator */
/* DAC Reference Coefficients */

/* DACA */
#define DAC_RESULT_TO_AMP_IFBK_OFFS         ( 0 )                                     /* ??? */
/* DACB */
#define DAC_RESULT_TO_VOLTS_DC_OFFS         ( 0 )                                     /* ??? */
/* DAC External */
#define DAC_RESULT_TO_AMP_CFBK_OFFS			0 //to do implement if needed




/* DAC Scale To Input Connector Voltage Multipliers */
#define DAC_SCALE_TO_AMP_IFBK_INPUT_VOLTAGE_MULT         (BOARDCFG_DAC_REFERENCE_VOLTAGE_VOLTS * DACREF_TO_BI_POLAR_SIGNAL_CONDITIONING_MULT)
#define DAC_SCALE_TO_VOLTS_DC_INPUT_VOLTAGE_MULT         (BOARDCFG_DAC_REFERENCE_VOLTAGE_VOLTS * DACREF_TO_POSITIVE_SIGNAL_CONDITIONING_MULT)
#define DAC_SCALE_TO_AMP_DCLI_INPUT_VOLTAGE_MULT         (BOARDCFG_DAC_REFERENCE_VOLTAGE_VOLTS * DACREF_TO_BI_POLAR_SIGNAL_CONDITIONING_MULT)

/* DAC Result To Input Connector Voltage Multipliers */
#define DAC_RESULT_TO_AMP_IFBK_INPUT_VOLTAGE_MULT        (DAC_SCALE_TO_AMP_IFBK_INPUT_VOLTAGE_MULT / ((uint32_t)1 << BOARDCFG_DAC_RESULT_BITS))
#define DAC_RESULT_TO_VOLTS_DC_INPUT_VOLTAGE_MULT        (DAC_SCALE_TO_VOLTS_DC_INPUT_VOLTAGE_MULT / ((uint32_t)1 << BOARDCFG_DAC_RESULT_BITS))
#define DAC_RESULT_TO_AMP_DCLI_INPUT_VOLTAGE_MULT        (DAC_SCALE_TO_AMP_DCLI_INPUT_VOLTAGE_MULT / ((uint32_t)1 << BOARDCFG_DAC_RESULT_BITS))

/* DAC Scale To Real Voltage/Current Values Multipliers */
#define DAC_SCALE_TO_AMP_IFBK_MULT          (BOARDCFG_DAC_REFERENCE_VOLTAGE_VOLTS * DACREF_TO_BI_POLAR_SIGNAL_CONDITIONING_MULT * VOLTAGE_ON_CURRENT_INPUT_TO_AMPERES_MULT) /* IPH1  - IPH9  */
#define DAC_SCALE_TO_VOLTS_DC_MULT          (BOARDCFG_DAC_REFERENCE_VOLTAGE_VOLTS * DACREF_TO_POSITIVE_SIGNAL_CONDITIONING_MULT * VOLTAGE_ON_VOLTAGE_INPUT_DC_TO_VOLT_MULT) /* DCLV1 , DCLV4 */
#define DAC_SCALE_TO_AMP_DCLI_MULT          (BOARDCFG_DAC_REFERENCE_VOLTAGE_VOLTS * DACREF_TO_BI_POLAR_SIGNAL_CONDITIONING_MULT * VOLTAGE_ON_CUR_DCL_INPUT_TO_AMPERES_MULT) /* DCLI1 - DCLI3 */

/* DAC Result To Real Voltage/Current Values Multipliers */
#define DAC_RESULT_TO_AMP_IFBK_MULT          (DAC_SCALE_TO_AMP_IFBK_MULT / ((uint32_t)1 << BOARDCFG_DAC_RESULT_BITS)) /* IPH1  - IPH9  */
#define DAC_RESULT_TO_VOLTS_DC_MULT          (DAC_SCALE_TO_VOLTS_DC_MULT / ((uint32_t)1 << BOARDCFG_DAC_RESULT_BITS)) /* DCLV1 , DCLV4 */
#define DAC_RESULT_TO_AMP_DCLI_MULT          (DAC_SCALE_TO_AMP_DCLI_MULT / ((uint32_t)1 << BOARDCFG_DAC_RESULT_BITS)) /* DCLI1 - DCLI3 */

#define DAC_RESULT_TO_AMP_CFBK_MULT         DAC_RESULT_TO_AMP_DCLI_MULT //to do implement if needed


/* Name Compatibility */
#define BOARDCFG_DAC_RESULT_TO_IFBK_MULT    DAC_RESULT_TO_AMP_IFBK_MULT
#define BOARDCFG_DAC_RESULT_TO_IFBK_OFFS    DAC_RESULT_TO_AMP_IFBK_OFFS
#define BOARDCFG_DAC_RESULT_TO_UFBK_MULT    DAC_RESULT_TO_VOLTS_DC_MULT
#define BOARDCFG_DAC_RESULT_TO_UFBK_OFFS    DAC_RESULT_TO_VOLTS_DC_OFFS

/* Driver Temperature Feedback */
#define DRIVER_TEMPERATURE_ADC_RESULT_TO_DECI_DEGREES_MULTIPLIER \
(10 * ((115.0 - 30.0) / (10.0 - 1.0)) * ((30000.0F +  13000.0) /  13000.0) * (ADC_REF_VOLTS / (1 << BOARDCFG_ADC_RESULT_BITS)))



/* *****************************************************************************
 * Other Configuration Definitions
 **************************************************************************** */


#ifdef PROJECTCFG_HMLIB_HEXMON_UART_MODULE
#define HMLIB_HEXMON_UART_MODULE       PROJECTCFG_HMLIB_HEXMON_UART_MODULE
#else
#define HMLIB_HEXMON_UART_MODULE       UART_A
#endif

#ifdef PROJECTCFG_HMLIB_MODBUS_UART_MODULE
#define HMLIB_MODBUS_UART_MODULE       PROJECTCFG_HMLIB_MODBUS_UART_MODULE
#else
#define HMLIB_MODBUS_UART_MODULE       UART_A
#endif


#define HMLIB_MODBUS_DEBUG_RESPONSES                    0
#define HMLIB_MODBUS_AND_HEXMON_UART_MODULE_DIFFERENT   1   /* here still not used */

#define HMDRV_MODBUS_BAUD       PROJECTCFG_HMDRV_MODBUS_BAUD
#define HMDRV_HEXMON_BAUD       PROJECTCFG_HMDRV_HEXMON_BAUD

#define HMDRV_TX_DELAY          PROJECTCFG_USE_HMDRV_TX_DELAY

#define HMDRV_MODBUS_TX_DELAY   PROJECTCFG_USE_HMDRV_MODBUS_TX_DELAY

#define UART_PARITY_MODBUS    PROJECTCFG_UART_PARITY_MODBUS
#define UART_PARITY_HEXMON    PROJECTCFG_UART_PARITY_HEXMON

#define UART_STOP_BITS          PROJECTCFG_STOP_BITS


/* *****************************************************************************
 * Other Configuration Definitions
 **************************************************************************** */
#if (BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC)
    #define DEBUG_SET_CTRL()        //GPIO_writePin(STAT_LED_G_PIN, STAT_LED_ENABLE);
    #define DEBUG_CLR_CTRL()        //GPIO_writePin(STAT_LED_G_PIN, STAT_LED_DISABLE);
    #define DEBUG_SET_MAIN()
    #define DEBUG_CLR_MAIN()
    #define DEBUG_SET_ADC()
    #define DEBUG_CLR_ADC()
    #define DEBUG_SET_PWM1()
    #define DEBUG_CLR_PWM1()
    #define DEBUG_SET_PWM6()
    #define DEBUG_CLR_PWM6()
#elif (BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG)
    #define DEBUG_SET_CTRL()        GPIO_writePin(STAT_LED_G_PIN, STAT_LED_ENABLE);
    #define DEBUG_CLR_CTRL()        GPIO_writePin(STAT_LED_G_PIN, STAT_LED_DISABLE);
    #define DEBUG_SET_MAIN()
    #define DEBUG_CLR_MAIN()
    #define DEBUG_SET_ADC()
    #define DEBUG_CLR_ADC()
    #define DEBUG_SET_PWM1()
    #define DEBUG_CLR_PWM1()
    #define DEBUG_SET_PWM6()
    #define DEBUG_CLR_PWM6()
#elif  BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_SMALL_TEST
    #define DEBUG_SET_CTRL()        GPIO_writePin(CONTACTOR_CTRL_PIN, 1);
    #define DEBUG_CLR_CTRL()        GPIO_writePin(CONTACTOR_CTRL_PIN, 0);
    #define DEBUG_SET_MAIN()        GPIO_writePin(PRCH_REL_CTRL_PIN, 1);
    #define DEBUG_CLR_MAIN()        GPIO_writePin(PRCH_REL_CTRL_PIN, 0);
    #define DEBUG_SET_ADC()         //GPIO_writePin(CONTACTOR_CTRL_PIN, 1);
    #define DEBUG_CLR_ADC()         //GPIO_writePin(CONTACTOR_CTRL_PIN, 0);
    #define DEBUG_SET_PWM1()
    #define DEBUG_CLR_PWM1()
    #define DEBUG_SET_PWM6()        //GPIO_writePin(PRCH_REL_CTRL_PIN, 1);
    #define DEBUG_CLR_PWM6()        //GPIO_writePin(PRCH_REL_CTRL_PIN, 0);
#endif



#endif	/* BOARDCFG_H */
