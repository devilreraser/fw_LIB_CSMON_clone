/* *****************************************************************************
 * File:   fpga_dio.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 08 14 12:51
 * 
 * Description: FPGA DIO (Digital Inputs Outputs)
 * 
 **************************************************************************** */
#ifdef _WIN32
#pragma once
#endif /* _WIN32 */

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "boardcfg.h"

#if BOARDCFG_USE_FPGA_DIO
#include "sys_drv.h"

#include "fpga_cfg.h"
#include "fpga_common.h"
#include "fpga_dio.h"
#include "control_common.h"
#include "hmlib.h"
#if HMDRV_USE_EXT_PMP
#include "pmpmem.h"
#endif



/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

/* IO FPGA */
/*
constant CLOCKS_CYCLE : natural := 24; -- used clock is 240MHz each clock gives 4.16ns delay. Minimum value is 2
constant PMALL_COUNT : natural := 3;
constant PMALH_COUNT : natural := 14;
constant CS_INIT_VAL : STD_LOGIC_VECTOR(CLOCKS_CYCLE - 2 downto 0) := (OTHERS => '0');


constant VERSION		: STD_LOGIC_VECTOR(7 downto 0):= X"01";
constant REVISION		: STD_LOGIC_VECTOR(7 downto 0):= X"01";

constant ADDR_FPGA		: STD_LOGIC_VECTOR(5 downto 0):= "000011";

constant UNLOCK_KEY		: STD_LOGIC_VECTOR(7 downto 0):= X"A5";


constant WADDR_UNLOCK	: STD_LOGIC_VECTOR(7 downto 0):= X"80"; -- locking is only for writing of configuration for F pins of DSP and digital outptuts. State of outputs registers can be written whithout unlocking.
constant WADDR_DOH		: STD_LOGIC_VECTOR(7 downto 0):= X"01";
constant WADDR_DOL		: STD_LOGIC_VECTOR(7 downto 0):= X"02";

-- Configuration of F1-9. These DSP pins can be mapped to inputs on DIO board. For each F pin there is a configuration register.
-- Configuration register can be 0x00 (not configured), 0x01 to 0x10.
-- When configuration register is 0x01 then DI1 is connected to the F pin of the DSP, when 0x02 then DI2 is connected and etc. - 0x10 is DI16

-- Digital outptus can be configured to use F1-9 as source. if DX is set to 0x00 then it takes from s_do_reg it's coresponding bit D1 takes bit 0, D2 - bit 1 and etc. - D16 takes bit 15
-- When DX is set to number 0x01 to 0x09 then it takes the state from F1 to F9.

constant WADDR_F1		: STD_LOGIC_VECTOR(7 downto 0):= X"11"; 
constant WADDR_F2		: STD_LOGIC_VECTOR(7 downto 0):= X"12";
constant WADDR_F3		: STD_LOGIC_VECTOR(7 downto 0):= X"13";
constant WADDR_F4		: STD_LOGIC_VECTOR(7 downto 0):= X"14";
constant WADDR_F5		: STD_LOGIC_VECTOR(7 downto 0):= X"15";
constant WADDR_F6		: STD_LOGIC_VECTOR(7 downto 0):= X"16";
constant WADDR_F7		: STD_LOGIC_VECTOR(7 downto 0):= X"17";
constant WADDR_F8		: STD_LOGIC_VECTOR(7 downto 0):= X"18";
constant WADDR_F9		: STD_LOGIC_VECTOR(7 downto 0):= X"19";
constant WADDR_D1		: STD_LOGIC_VECTOR(7 downto 0):= X"20"; 
constant WADDR_D2		: STD_LOGIC_VECTOR(7 downto 0):= X"21"; 
constant WADDR_D3		: STD_LOGIC_VECTOR(7 downto 0):= X"22"; 
constant WADDR_D4		: STD_LOGIC_VECTOR(7 downto 0):= X"23"; 
constant WADDR_D5		: STD_LOGIC_VECTOR(7 downto 0):= X"24"; 
constant WADDR_D6		: STD_LOGIC_VECTOR(7 downto 0):= X"25"; 
constant WADDR_D7		: STD_LOGIC_VECTOR(7 downto 0):= X"26"; 
constant WADDR_D8		: STD_LOGIC_VECTOR(7 downto 0):= X"27"; 
constant WADDR_D9		: STD_LOGIC_VECTOR(7 downto 0):= X"28"; 
constant WADDR_D10		: STD_LOGIC_VECTOR(7 downto 0):= X"29"; 
constant WADDR_D11		: STD_LOGIC_VECTOR(7 downto 0):= X"2A"; 
constant WADDR_D12		: STD_LOGIC_VECTOR(7 downto 0):= X"2B"; 
constant WADDR_D13		: STD_LOGIC_VECTOR(7 downto 0):= X"2C"; 
constant WADDR_D14		: STD_LOGIC_VECTOR(7 downto 0):= X"2D"; 
constant WADDR_D15		: STD_LOGIC_VECTOR(7 downto 0):= X"2E"; 
constant WADDR_D16		: STD_LOGIC_VECTOR(7 downto 0):= X"2F"; 


constant RADDR_UNLOCK	: STD_LOGIC_VECTOR(7 downto 0):= X"80"; 
constant RADDR_VER		: STD_LOGIC_VECTOR(7 downto 0):= X"81";
constant RADDR_REV		: STD_LOGIC_VECTOR(7 downto 0):= X"82";

constant RADDR_DOH		: STD_LOGIC_VECTOR(7 downto 0):= X"01";
constant RADDR_DOL		: STD_LOGIC_VECTOR(7 downto 0):= X"02";
constant RADDR_DIH		: STD_LOGIC_VECTOR(7 downto 0):= X"03";
constant RADDR_DIL		: STD_LOGIC_VECTOR(7 downto 0):= X"04";

constant RADDR_SYS		: STD_LOGIC_VECTOR(7 downto 0):= X"10";
constant RADDR_F1		: STD_LOGIC_VECTOR(7 downto 0):= X"11";
constant RADDR_F2		: STD_LOGIC_VECTOR(7 downto 0):= X"12";
constant RADDR_F3		: STD_LOGIC_VECTOR(7 downto 0):= X"13";
constant RADDR_F4		: STD_LOGIC_VECTOR(7 downto 0):= X"14";
constant RADDR_F5		: STD_LOGIC_VECTOR(7 downto 0):= X"15";
constant RADDR_F6		: STD_LOGIC_VECTOR(7 downto 0):= X"16";
constant RADDR_F7		: STD_LOGIC_VECTOR(7 downto 0):= X"17";
constant RADDR_F8		: STD_LOGIC_VECTOR(7 downto 0):= X"18";
constant RADDR_F9		: STD_LOGIC_VECTOR(7 downto 0):= X"19";
constant RADDR_D1		: STD_LOGIC_VECTOR(7 downto 0):= X"20"; 
constant RADDR_D2		: STD_LOGIC_VECTOR(7 downto 0):= X"21"; 
constant RADDR_D3		: STD_LOGIC_VECTOR(7 downto 0):= X"22"; 
constant RADDR_D4		: STD_LOGIC_VECTOR(7 downto 0):= X"23"; 
constant RADDR_D5		: STD_LOGIC_VECTOR(7 downto 0):= X"24"; 
constant RADDR_D6		: STD_LOGIC_VECTOR(7 downto 0):= X"25"; 
constant RADDR_D7		: STD_LOGIC_VECTOR(7 downto 0):= X"26"; 
constant RADDR_D8		: STD_LOGIC_VECTOR(7 downto 0):= X"27"; 
constant RADDR_D9		: STD_LOGIC_VECTOR(7 downto 0):= X"28"; 
constant RADDR_D10		: STD_LOGIC_VECTOR(7 downto 0):= X"29"; 
constant RADDR_D11		: STD_LOGIC_VECTOR(7 downto 0):= X"2A"; 
constant RADDR_D12		: STD_LOGIC_VECTOR(7 downto 0):= X"2B"; 
constant RADDR_D13		: STD_LOGIC_VECTOR(7 downto 0):= X"2C"; 
constant RADDR_D14		: STD_LOGIC_VECTOR(7 downto 0):= X"2D"; 
constant RADDR_D15		: STD_LOGIC_VECTOR(7 downto 0):= X"2E"; 
constant RADDR_D16		: STD_LOGIC_VECTOR(7 downto 0):= X"2F"; 

*/


#define PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET                  0x0300

#define PMP_ADDRESS_FPGA_IO_RADDR_UNLOCK                    0x0080  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_VER                       0x0081  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_REV                       0x0082  /* 1 byte */

#define PMP_ADDRESS_FPGA_IO_RADDR_DOH                       0x0001  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_DOL                       0x0002  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_DIH                       0x0003  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_DIL                       0x0004  /* 1 byte */

#define PMP_ADDRESS_FPGA_IO_RADDR_SYS                       0x0010  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_F1                        0x0011  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_F2                        0x0012  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_F3                        0x0013  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_F4                        0x0014  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_F5                        0x0015  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_F6                        0x0016  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_F7                        0x0017  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_F8                        0x0018  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_F9                        0x0019  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_D1                        0x0020  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_D2                        0x0021  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_D3                        0x0022  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_D4                        0x0023  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_D5                        0x0024  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_D6                        0x0025  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_D7                        0x0026  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_D8                        0x0027  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_D9                        0x0028  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_D10                       0x0029  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_D11                       0x002A  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_D12                       0x002B  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_D13                       0x002C  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_D14                       0x002D  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_D15                       0x002E  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_RADDR_D16                       0x002F  /* 1 byte */

#define PMP_ADDRESS_FPGA_IO_WADDR_UNLOCK                    0x0080  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_WADDR_DOH                       0x0001  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_WADDR_DOL                       0x0002  /* 1 byte */

#define PMP_ADDRESS_FPGA_IO_WADDR_F1                        0x0011  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_WADDR_F2                        0x0012  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_WADDR_F3                        0x0013  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_WADDR_F4                        0x0014  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_WADDR_F5                        0x0015  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_WADDR_F6                        0x0016  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_WADDR_F7                        0x0017  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_WADDR_F8                        0x0018  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_WADDR_F9                        0x0019  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_WADDR_D1                        0x0020  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_WADDR_D2                        0x0021  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_WADDR_D3                        0x0022  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_WADDR_D4                        0x0023  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_WADDR_D5                        0x0024  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_WADDR_D6                        0x0025  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_WADDR_D7                        0x0026  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_WADDR_D8                        0x0027  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_WADDR_D9                        0x0028  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_WADDR_D10                       0x0029  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_WADDR_D11                       0x002A  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_WADDR_D12                       0x002B  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_WADDR_D13                       0x002C  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_WADDR_D14                       0x002D  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_WADDR_D15                       0x002E  /* 1 byte */
#define PMP_ADDRESS_FPGA_IO_WADDR_D16                       0x002F  /* 1 byte */


#if FPGA_CFG_USE_DIDO_FPGA == 0
#define PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET                  0x3F80

/* Not a global address - PMP Address Offset */
#define PMP_ADDRESS_FPGA_OUTPUTS                            0x0000
#define PMP_ADDRESS_FPGA_INPUTS                             0x0002
#define PMP_ADDRESS_FPGA_OUTPUTS_FBK                        0x0004
#endif




/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */
typedef enum
{
    MAP_NONE,
    MAP_D1,
    MAP_D2,
    MAP_D3,
    MAP_D4,
    MAP_D5,
    MAP_D6,
    MAP_D7,
    MAP_D8,
    MAP_D9,
    MAP_D10,
    MAP_D11,
    MAP_D12,
    MAP_D13,
    MAP_D14,
    MAP_D15,
    MAP_D16,
    MAP_DEND
    
}eMapInputToFunc_t;


typedef enum
{
    MAP_IDLE,
    MAP_F1,
    MAP_F2,
    MAP_F3,
    MAP_F4,
    MAP_F5,
    MAP_F6,
    MAP_F7,
    MAP_F8,
    MAP_F9,
//    MAP_F10,
//    MAP_F11,
//    MAP_F12,
//    MAP_F13,
//    MAP_F14,
//    MAP_F0,
    MAP_END
    
}eMapFuncToOutput_t;


/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */



/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */

eMapInputToFunc_t ePinF1 = MAP_NONE;
eMapInputToFunc_t ePinF2 = MAP_NONE;
eMapInputToFunc_t ePinF3 = MAP_NONE;
eMapInputToFunc_t ePinF4 = MAP_NONE;
eMapInputToFunc_t ePinF5 = MAP_NONE;
eMapInputToFunc_t ePinF6 = MAP_NONE;
eMapInputToFunc_t ePinF7 = MAP_NONE;
eMapInputToFunc_t ePinF8 = MAP_NONE;
eMapInputToFunc_t ePinF9 = MAP_NONE;

eMapFuncToOutput_t ePinDO1 = MAP_NONE;
eMapFuncToOutput_t ePinDO2 = MAP_NONE;
eMapFuncToOutput_t ePinDO3 = MAP_NONE;
eMapFuncToOutput_t ePinDO4 = MAP_NONE;
eMapFuncToOutput_t ePinDO5 = MAP_NONE;
eMapFuncToOutput_t ePinDO6 = MAP_NONE;
eMapFuncToOutput_t ePinDO7 = MAP_F1;
eMapFuncToOutput_t ePinDO8 = MAP_NONE;
eMapFuncToOutput_t ePinDO9 = MAP_NONE;
eMapFuncToOutput_t ePinDO10 = MAP_NONE;
eMapFuncToOutput_t ePinDO11 = MAP_NONE;
eMapFuncToOutput_t ePinDO12 = MAP_NONE;
eMapFuncToOutput_t ePinDO13 = MAP_NONE;
eMapFuncToOutput_t ePinDO14 = MAP_NONE;
eMapFuncToOutput_t ePinDO15 = MAP_NONE;
eMapFuncToOutput_t ePinDO16 = MAP_NONE;

eMapInputToFunc_t ePinF1Fbk = MAP_NONE;
eMapInputToFunc_t ePinF2Fbk = MAP_NONE;
eMapInputToFunc_t ePinF3Fbk = MAP_NONE;
eMapInputToFunc_t ePinF4Fbk = MAP_NONE;
eMapInputToFunc_t ePinF5Fbk = MAP_NONE;
eMapInputToFunc_t ePinF6Fbk = MAP_NONE;
eMapInputToFunc_t ePinF7Fbk = MAP_NONE;
eMapInputToFunc_t ePinF8Fbk = MAP_NONE;
eMapInputToFunc_t ePinF9Fbk = MAP_NONE;

eMapFuncToOutput_t ePinDO1Fbk = MAP_NONE;
eMapFuncToOutput_t ePinDO2Fbk = MAP_NONE;
eMapFuncToOutput_t ePinDO3Fbk = MAP_NONE;
eMapFuncToOutput_t ePinDO4Fbk = MAP_NONE;
eMapFuncToOutput_t ePinDO5Fbk = MAP_NONE;
eMapFuncToOutput_t ePinDO6Fbk = MAP_NONE;
eMapFuncToOutput_t ePinDO7Fbk = MAP_NONE;
eMapFuncToOutput_t ePinDO8Fbk = MAP_NONE;
eMapFuncToOutput_t ePinDO9Fbk = MAP_NONE;
eMapFuncToOutput_t ePinDO10Fbk = MAP_NONE;
eMapFuncToOutput_t ePinDO11Fbk = MAP_NONE;
eMapFuncToOutput_t ePinDO12Fbk = MAP_NONE;
eMapFuncToOutput_t ePinDO13Fbk = MAP_NONE;
eMapFuncToOutput_t ePinDO14Fbk = MAP_NONE;
eMapFuncToOutput_t ePinDO15Fbk = MAP_NONE;
eMapFuncToOutput_t ePinDO16Fbk = MAP_NONE;


        
        
uControlIO_t uDOExternal = {0};
uControlIO_t uDIExternal;
uControlIO_t uDOExternalFbk;
uControlIO_t uDIExternalFbk;

uint8_t u8PMPConfigurationUnlockKeyDIO = PMP_CONFIGURATION_UNLOCK_KEY;
uint8_t u8PMPConfigurationLockKeyDIO   = 0x00;

bool bSetupIOConfiguration = 0;


/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */

void FPGA_DIO_vIOInit (void)
{
    uDOExternal.u16Register = 0;    
}


void FPGA_DIO_vRunTimeSetupProcess(void)
{
    if ( (*((uint8_t*)&ePinF1Fbk)   != *((uint8_t*)&ePinF1))
      || (*((uint8_t*)&ePinF2Fbk)   != *((uint8_t*)&ePinF2))
      || (*((uint8_t*)&ePinF3Fbk)   != *((uint8_t*)&ePinF3))
      || (*((uint8_t*)&ePinF4Fbk)   != *((uint8_t*)&ePinF4))
      || (*((uint8_t*)&ePinF5Fbk)   != *((uint8_t*)&ePinF5))
      || (*((uint8_t*)&ePinF6Fbk)   != *((uint8_t*)&ePinF6))
      || (*((uint8_t*)&ePinF7Fbk)   != *((uint8_t*)&ePinF7))
      || (*((uint8_t*)&ePinF8Fbk)   != *((uint8_t*)&ePinF8))
      || (*((uint8_t*)&ePinF9Fbk)   != *((uint8_t*)&ePinF9))
      || (*((uint8_t*)&ePinDO1Fbk)   != *((uint8_t*)&ePinDO1))
      || (*((uint8_t*)&ePinDO2Fbk)   != *((uint8_t*)&ePinDO2))
      || (*((uint8_t*)&ePinDO3Fbk)   != *((uint8_t*)&ePinDO3))
      || (*((uint8_t*)&ePinDO4Fbk)   != *((uint8_t*)&ePinDO4))
      || (*((uint8_t*)&ePinDO5Fbk)   != *((uint8_t*)&ePinDO5))
      || (*((uint8_t*)&ePinDO6Fbk)   != *((uint8_t*)&ePinDO6))
      || (*((uint8_t*)&ePinDO7Fbk)   != *((uint8_t*)&ePinDO7))
      || (*((uint8_t*)&ePinDO8Fbk)   != *((uint8_t*)&ePinDO8))
      || (*((uint8_t*)&ePinDO9Fbk)   != *((uint8_t*)&ePinDO9))
      || (*((uint8_t*)&ePinDO10Fbk)   != *((uint8_t*)&ePinDO10))
      || (*((uint8_t*)&ePinDO11Fbk)   != *((uint8_t*)&ePinDO11))
      || (*((uint8_t*)&ePinDO12Fbk)   != *((uint8_t*)&ePinDO12))
      || (*((uint8_t*)&ePinDO13Fbk)   != *((uint8_t*)&ePinDO13))
      || (*((uint8_t*)&ePinDO14Fbk)   != *((uint8_t*)&ePinDO14))
      || (*((uint8_t*)&ePinDO15Fbk)   != *((uint8_t*)&ePinDO15))
      || (*((uint8_t*)&ePinDO16Fbk)   != *((uint8_t*)&ePinDO16))
        )
    {
        bSetupIOConfiguration = 1;
    }

}

void FPGA_DIO_vGetIOConfiguration(void)
{
    
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&ePinF1Fbk,   (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_F1 ), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&ePinF2Fbk,   (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_F2 ), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&ePinF3Fbk,   (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_F3 ), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&ePinF4Fbk,   (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_F4 ), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&ePinF5Fbk,   (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_F5 ), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&ePinF6Fbk,   (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_F6 ), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&ePinF7Fbk,   (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_F7 ), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&ePinF8Fbk,   (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_F8 ), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&ePinF9Fbk,   (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_F9 ), 1);
        
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&ePinDO1Fbk,  (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_D1 ), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&ePinDO2Fbk,  (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_D2 ), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&ePinDO3Fbk,  (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_D3 ), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&ePinDO4Fbk,  (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_D4 ), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&ePinDO5Fbk,  (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_D5 ), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&ePinDO6Fbk,  (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_D6 ), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&ePinDO7Fbk,  (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_D7 ), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&ePinDO8Fbk,  (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_D8 ), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&ePinDO9Fbk,  (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_D9 ), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&ePinDO10Fbk, (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_D10), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&ePinDO11Fbk, (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_D11), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&ePinDO12Fbk, (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_D12), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&ePinDO13Fbk, (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_D13), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&ePinDO14Fbk, (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_D14), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&ePinDO15Fbk, (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_D15), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&ePinDO16Fbk, (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_D16), 1);
    
}

void eSetupFTRIS(eMapFuncToOutput_t ePin)
{
    #if BOARDCFG_USE_FUNCTION_PINS
    switch (ePin)
    {
        case MAP_F1:
            F1_SetLow();F1_SetDigitalOutput(); 
            break;
        case MAP_F2:
            F2_SetLow();F2_SetDigitalOutput(); 
            break;
        case MAP_F3:
            F3_SetLow();F3_SetDigitalOutput(); 
            break;
        case MAP_F4:
            F4_SetLow();F4_SetDigitalOutput(); 
            break;
        case MAP_F5:
            F5_SetLow();F5_SetDigitalOutput(); 
            break;
        case MAP_F6:
            F6_SetLow();F6_SetDigitalOutput(); 
            break;
        case MAP_F7:
            F7_SetLow();F7_SetDigitalOutput(); 
            break;
        case MAP_F8:
            F8_SetLow();F8_SetDigitalOutput(); 
            break;
        case MAP_F9:
            F9_SetLow();F9_SetDigitalOutput(); 
            break;
        default:
            break;
    }
    #endif
}

void FPGA_DIO_vIOConfigure (void)
{
    if (bUsePMPwithControl)
    {  
        if (bSetupIOConfiguration)
        {
            bSetupIOConfiguration = 0;
            //DEBUG_SET_MAIN();
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_UNLOCK), &u8PMPConfigurationUnlockKeyDIO         , 1);
            //DEBUG_CLR_MAIN();
            if ((ePinF1 > MAP_NONE) && (ePinF1 < MAP_DEND))  { F1_SetDigitalInput(); }
            if ((ePinF2 > MAP_NONE) && (ePinF2 < MAP_DEND))  { F2_SetDigitalInput(); }
            if ((ePinF3 > MAP_NONE) && (ePinF3 < MAP_DEND))  { F3_SetDigitalInput(); }
            if ((ePinF4 > MAP_NONE) && (ePinF4 < MAP_DEND))  { F4_SetDigitalInput(); }
            if ((ePinF5 > MAP_NONE) && (ePinF5 < MAP_DEND))  { F5_SetDigitalInput(); }
            if ((ePinF6 > MAP_NONE) && (ePinF6 < MAP_DEND))  { F6_SetDigitalInput(); }
            if ((ePinF7 > MAP_NONE) && (ePinF7 < MAP_DEND))  { F7_SetDigitalInput(); }
            if ((ePinF8 > MAP_NONE) && (ePinF8 < MAP_DEND))  { F8_SetDigitalInput(); }
            if ((ePinF9 > MAP_NONE) && (ePinF9 < MAP_DEND))  { F9_SetDigitalInput(); }

            
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_F1), (HMDRV_BPTR)&ePinF1           , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_F2), (HMDRV_BPTR)&ePinF2           , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_F3), (HMDRV_BPTR)&ePinF3           , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_F4), (HMDRV_BPTR)&ePinF4           , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_F5), (HMDRV_BPTR)&ePinF5           , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_F6), (HMDRV_BPTR)&ePinF6           , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_F7), (HMDRV_BPTR)&ePinF7           , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_F8), (HMDRV_BPTR)&ePinF8           , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_F9), (HMDRV_BPTR)&ePinF9           , 1);

            
            eSetupFTRIS(ePinDO1);
            eSetupFTRIS(ePinDO2);
            eSetupFTRIS(ePinDO3);
            eSetupFTRIS(ePinDO4);
            eSetupFTRIS(ePinDO5);
            eSetupFTRIS(ePinDO6);
            eSetupFTRIS(ePinDO7);
            eSetupFTRIS(ePinDO8);
            eSetupFTRIS(ePinDO9);
            eSetupFTRIS(ePinDO10);
            eSetupFTRIS(ePinDO11);
            eSetupFTRIS(ePinDO12);
            eSetupFTRIS(ePinDO13);
            eSetupFTRIS(ePinDO14);
            eSetupFTRIS(ePinDO15);
            eSetupFTRIS(ePinDO16);

            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_D1),  (HMDRV_BPTR)&ePinDO1           , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_D2),  (HMDRV_BPTR)&ePinDO2           , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_D3),  (HMDRV_BPTR)&ePinDO3           , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_D4),  (HMDRV_BPTR)&ePinDO4           , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_D5),  (HMDRV_BPTR)&ePinDO5           , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_D6),  (HMDRV_BPTR)&ePinDO6           , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_D7),  (HMDRV_BPTR)&ePinDO7           , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_D8),  (HMDRV_BPTR)&ePinDO8           , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_D9),  (HMDRV_BPTR)&ePinDO9           , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_D10), (HMDRV_BPTR)&ePinDO10          , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_D11), (HMDRV_BPTR)&ePinDO11          , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_D12), (HMDRV_BPTR)&ePinDO12          , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_D13), (HMDRV_BPTR)&ePinDO13          , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_D14), (HMDRV_BPTR)&ePinDO14          , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_D15), (HMDRV_BPTR)&ePinDO15          , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_D16), (HMDRV_BPTR)&ePinDO16          , 1);
            
            
            PMPMEM_vWriteToExtMem((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_UNLOCK), &u8PMPConfigurationLockKeyDIO           , 1);
        }
    }
    
}
void FPGA_DIO_vIOProcess (void)
{
    if (bUsePMPwithControl)
    {
        PMPMEM_vWriteToExtMemSingle  ((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_DOL),   (HMDRV_BPTR)(&uDOExternal)+0, 1);
        PMPMEM_vWriteToExtMemSingle  ((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_WADDR_DOH),   (HMDRV_BPTR)(&uDOExternal)+1, 1);

        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)(&uDIExternalFbk)+0, (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_DIL), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)(&uDIExternalFbk)+1, (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_DIH), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)(&uDOExternalFbk)+0, (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_DOL), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)(&uDOExternalFbk)+1, (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_IO_RADDR_DOH), 1);
    }
    
    uDIExternal.u16Register = uDIExternalFbk.u16Register;  
}





#if FPGA_CFG_USE_DIDO_FPGA == 0

void CONTROL_vIOProcess (void)
{
    if (bUsePMPwithControl)
    {
        //PMPMEM_vWriteToExtMem((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_OUTPUTS), &uDOExternal, 4);
        PMPMEM_vWriteToExtMem((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_OUTPUTS), &uDOExternal, 2);
        //PMPMEM_vWriteToExtMem((PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_INPUTS), &uDIExternal, 2);

        //PMPMEM_vReadFromExtMem(&uDOExternalFbk, (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_OUTPUTS), 6); do not use of fix structure of uDOExternalFbk
        //PMPMEM_vReadFromExtMem(&uDOExternalFbk, (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_OUTPUTS), 2);
        PMPMEM_vReadFromExtMem(&uDIExternalFbk, (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_INPUTS), 2);
        PMPMEM_vReadFromExtMem(&uDOExternalFbk, (PMP_ADDRESS_FPGA_IO_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_OUTPUTS_FBK), 2);
    }
    
    uDIExternal.u16Register = uDIExternalFbk.u16Register;  
    
}

#endif /* #if USE_DIDO_FPGA == 0 */


#endif /* #if USE_FPGA_DIO */




