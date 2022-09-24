/* *****************************************************************************
 * File:   fpga_pwm.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 08 14 12:51
 * 
 * Description: FPGA AP PWM (Analog PWM board PWM fpga)
 * 
 **************************************************************************** */
#ifdef _WIN32
#pragma once
#endif /* _WIN32 */

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "boardcfg.h"

#if BOARDCFG_USE_FPGA_PWM

#include "sys_drv.h"
#include "fpga_cfg.h"
#include "fpga_common.h"
#include "fpga_pwm.h"
#include "mclib.h"
#include "control_cfg.h"
#include "control_common.h"
#include "dio.h"

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


/* PWM FPGA */
/*
constant CLOCKS_CYCLE : natural := 24; -- used clock is 240MHz each clock gives 4.16ns delay. Minimum value is 2
constant PMALL_COUNT : natural := 3;
constant PMALH_COUNT : natural := 14;
constant CS_INIT_VAL : STD_LOGIC_VECTOR(CLOCKS_CYCLE - 2 downto 0) := (OTHERS => '0');

constant VERSION		: STD_LOGIC_VECTOR(7 downto 0):= X"01";
constant REVISION		: STD_LOGIC_VECTOR(7 downto 0):= X"01";


constant ADDR_FPGA		: STD_LOGIC_VECTOR(5 downto 0):= "000010";

constant UNLOCK_KEY		: STD_LOGIC_VECTOR(7 downto 0):= X"A5";


constant WADDR_UNLOCK	: STD_LOGIC_VECTOR(7 downto 0):= X"80"; 
constant WADDR_MDET		: STD_LOGIC_VECTOR(7 downto 0):= X"11";
constant WADDR_MGDE		: STD_LOGIC_VECTOR(7 downto 0):= X"12";
constant WADDR_MPULSH	: STD_LOGIC_VECTOR(7 downto 0):= X"21";
constant WADDR_MPULSL	: STD_LOGIC_VECTOR(7 downto 0):= X"22";
constant WADDR_DTH		: STD_LOGIC_VECTOR(7 downto 0):= X"23";
constant WADDR_DTL		: STD_LOGIC_VECTOR(7 downto 0):= X"24";


constant RADDR_UNLOCK	: STD_LOGIC_VECTOR(7 downto 0):= X"80"; 
constant RADDR_VER		: STD_LOGIC_VECTOR(7 downto 0):= X"81";
constant RADDR_REV		: STD_LOGIC_VECTOR(7 downto 0):= X"82";
constant RADDR_FREG		: STD_LOGIC_VECTOR(7 downto 0):= X"00";
constant RADDR_DET		: STD_LOGIC_VECTOR(7 downto 0):= X"01";
constant RADDR_ERR		: STD_LOGIC_VECTOR(7 downto 0):= X"02";
constant RADDR_SYS		: STD_LOGIC_VECTOR(7 downto 0):= X"10";
constant RADDR_MDET		: STD_LOGIC_VECTOR(7 downto 0):= X"11";
constant RADDR_MGDE		: STD_LOGIC_VECTOR(7 downto 0):= X"12";
constant RADDR_MPULSH	: STD_LOGIC_VECTOR(7 downto 0):= X"21";
constant RADDR_MPULSL	: STD_LOGIC_VECTOR(7 downto 0):= X"22";
constant RADDR_DTH		: STD_LOGIC_VECTOR(7 downto 0):= X"23";
constant RADDR_DTL		: STD_LOGIC_VECTOR(7 downto 0):= X"24";

constant PWM_MIN_DEADTIME : STD_LOGIC_VECTOR(11 downto 0):= x"0FA"; -- 250 with 10ns step gives 2.5us dead time
constant PWM_MIN_MINPULS : STD_LOGIC_VECTOR(11 downto 0):= x"0C8"; -- 200 with 10ns step gives 2.0us min puls time

	s_flt_reg <= s_flt &								-- bit 7 Global Fault
				 '0' &									-- bit 6 Not Used
				 s_GECH & 								-- bit 5, 4, 3 channel fault CH3, Ch2, CH1
				 s_GEDET &								-- bit 2 Error in detected module
				 s_GERRT &								-- bit 1 Error in TOP
				 s_GERRB;								-- bit 0 Error in BOT	
*/


#define PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET                 0x0200  

#define PMP_ADDRESS_FPGA_PWM_RADDR_UNLOCK                   0x0080  /* 1 byte */
#define PMP_ADDRESS_FPGA_PWM_RADDR_VER                      0x0081  /* 1 byte */
#define PMP_ADDRESS_FPGA_PWM_RADDR_REV                      0x0082  /* 1 byte */
#define PMP_ADDRESS_FPGA_PWM_RADDR_FREG                     0x0000  /* 1 byte */
#define PMP_ADDRESS_FPGA_PWM_RADDR_DET                      0x0001  /* 1 byte */
#define PMP_ADDRESS_FPGA_PWM_RADDR_ERR                      0x0002  /* 1 byte */
#define PMP_ADDRESS_FPGA_PWM_RADDR_SYS                      0x0010  /* 1 byte */
#define PMP_ADDRESS_FPGA_PWM_RADDR_MDET                     0x0011  /* 1 byte */
#define PMP_ADDRESS_FPGA_PWM_RADDR_MGDE                     0x0012  /* 1 byte */
#define PMP_ADDRESS_FPGA_PWM_RADDR_MPULSH                   0x0021  /* 1 byte */
#define PMP_ADDRESS_FPGA_PWM_RADDR_MPULSL                   0x0022  /* 1 byte */
#define PMP_ADDRESS_FPGA_PWM_RADDR_DTH                      0x0023  /* 1 byte */
#define PMP_ADDRESS_FPGA_PWM_RADDR_DTL                      0x0024  /* 1 byte */

#define PMP_ADDRESS_FPGA_PWM_WADDR_UNLOCK                   0x0080  /* 1 byte */
#define PMP_ADDRESS_FPGA_PWM_WADDR_MDET                     0x0011  /* 1 byte */
#define PMP_ADDRESS_FPGA_PWM_WADDR_MGDE                     0x0012  /* 1 byte */
#define PMP_ADDRESS_FPGA_PWM_WADDR_MPULSH                   0x0021  /* 1 byte */
#define PMP_ADDRESS_FPGA_PWM_WADDR_MPULSL                   0x0022  /* 1 byte */
#define PMP_ADDRESS_FPGA_PWM_WADDR_DTH                      0x0023  /* 1 byte */
#define PMP_ADDRESS_FPGA_PWM_WADDR_DTL                      0x0024  /* 1 byte */






#if FPGA_CFG_USE_DIDO_FPGA == 0
#define PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET                 0x3F40 //   0x0000  //

#define PMP_ADDRESS_FPGA_PWM_DEAD_TIME                      0x0000
#define PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK1               0x0000
#define PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK1_PHASEA        0x0000
#define PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK1_PHASEB        0x0002
#define PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK1_PHASEC        0x0004
#define PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK1_WR            0x0006
#define PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK1_PHASEA_WR     0x0006
#define PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK1_PHASEB_WR     0x0008
#define PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK1_PHASEC_WR     0x000A
#define PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK2               0x0010
#define PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK2_PHASEA        0x0010
#define PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK2_PHASEB        0x0012
#define PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK2_PHASEC        0x0014
#define PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK2_WR            0x0016
#define PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK2_PHASEA_WR     0x0016
#define PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK2_PHASEB_WR     0x0018
#define PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK2_PHASEC_WR     0x001A
#endif




/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
uint8_t u8PMPConfigurationUnlockKeyPWM = PMP_CONFIGURATION_UNLOCK_KEY;
uint8_t u8PMPConfigurationLockKeyPWM   = 0x00;


uDET_t  uStatDET = {0};
uGDE_t  uStatGDE = {0};
    
uDET_t  uMaskDET = {0x07};  /* Force No Masking */
uGDE_t  uMaskGDE = {0xFF};
    
uDET_t  uMaskDETFbk = {0};
uGDE_t  uMaskGDEFbk = {0};
    
/* Dead Time Resolution 1 nsec */ 
uint16_t DeadTimeStack1[3] = 
{
    DEADTIME_FPGA_NSEC,
    DEADTIME_FPGA_NSEC,
    DEADTIME_FPGA_NSEC,
};

uint16_t DeadTimeStack2[3] = 
{
    DEADTIME_FPGA_NSEC,
    DEADTIME_FPGA_NSEC,
    DEADTIME_FPGA_NSEC,
};


uint16_t DeadTimeStack1Fbk[3] = 
{
    0xDEAD,
    0xDEAD,
    0xDEAD,
};

uint16_t DeadTimeStack2Fbk[3] = 
{
    0xDEAD,
    0xDEAD,
    0xDEAD,
};

/* Dead Time RAW - Resolution FPGA Dead Time Clock  */ 
uint16_t DeadTimeStack1RAW[3] = 
{
    0xDEAD,
    0xDEAD,
    0xDEAD,
};

uint16_t DeadTimeStack2RAW[3] = 
{
    0xDEAD,
    0xDEAD,
    0xDEAD,
};


uint16_t DeadTimeStack1FbkRAW[3] = 
{
    0xDEAD,
    0xDEAD,
    0xDEAD,
};

uint16_t DeadTimeStack2FbkRAW[3] = 
{
    0xDEAD,
    0xDEAD,
    0xDEAD,
};


uint16_t MinPulseStack1 = MINPULSE_FPGA_NSEC;
uint16_t MinPulseStack2 = MINPULSE_FPGA_NSEC;
uint16_t MinPulseStack1Fbk = 0xDEAD;
uint16_t MinPulseStack2Fbk = 0xDEAD;
        
uint16_t MinPulseStack1RAW = 0xDEAD;
uint16_t MinPulseStack2RAW = 0xDEAD;
uint16_t MinPulseStack1FbkRAW = 0xDEAD;
uint16_t MinPulseStack2FbkRAW = 0xDEAD;
        
bool bSetupDeadTimeRequest = 0;
bool bSetupMinPulseRequest = 0;
bool bSetupDETAndGDEMaskRequest = 0;        

uFREGPWM_t     uFREGPWM = {0};

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */
bool FPGA_PWM_vGetPWMError(void)
{
    bool bResult = false;
    
    if ((uFREGPWM.sFREGPWM.bGlobalFault > 0) || (uStatGDE.u8Register > 0) || (uFaultStatus.sFaultInput.bFLT1 > 0))
    {
        bResult = true;
    }
    
    if ((uStatDET.sDET.ErrDET1 > 0) || (uStatDET.sDET.ErrDET2 > 0) || (uStatDET.sDET.ErrDET3 > 0))
    {
        bResult = true;
    }
    
    return bResult;
}

bool FPGA_PWM_vGetDetectDriverPhase1(void)
{
    bool bResult;

    bResult = uStatDET.sDET.DET1;
            
    return bResult;
}

bool FPGA_PWM_vGetDetectDriverPhase2(void)
{
    bool bResult;

    bResult = uStatDET.sDET.DET2;
            
    return bResult;
}

bool FPGA_PWM_vGetDetectDriverPhase3(void)
{
    bool bResult;

    bResult = uStatDET.sDET.DET3;
            
    return bResult;
}


void FPGA_PWM_vClearErrorStatus(void)
{
    uFREGPWM.u16Register = 0;
    uStatGDE.u8Register = 0;
    uFaultStatus.u8Register = 0;
    uStatDET.u8Register = 0;
}  

void FPGA_PWM_vEnable(void)
{
    BOARDCFG_FPGA_ENABLE_PWM();
}

void FPGA_PWM_vDisable(void)
{
    BOARDCFG_FPGA_DISABLE_PWM();
}


void FPGA_PWM_vRunTimeSetupProcess(void)
{
    if (DeadTimeStack1Fbk[0] != DeadTimeStack1[0])
    {
        bSetupDeadTimeRequest = 1;
    }
    else
    {
        bSetupDeadTimeRequest = 0;
    }
    if (MinPulseStack1Fbk != MinPulseStack1)
    {
        bSetupMinPulseRequest = 1;
    }
    else
    {
        bSetupMinPulseRequest = 0;
    }
    if ((uMaskDETFbk.u8Register != uMaskDET.u8Register)
      ||(uMaskGDEFbk.u8Register != uMaskGDE.u8Register))
    {
        bSetupDETAndGDEMaskRequest = 1;
    }
    else
    {
        bSetupDETAndGDEMaskRequest = 0;
    }
    
    
    

}
void FPGA_PWM_vGetPWMStat(void)
{
    
    if (bUsePMPwithControl)
    {
#if HMDRV_USE_EXT_PMP
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&uFREGPWM,  (PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_RADDR_FREG), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&uStatDET,  (PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_RADDR_DET ), 1); 
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&uStatGDE,  (PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_RADDR_ERR ), 1);
#endif
    }
    
    
    
}

void FPGA_PWM_vSetDeadTimeAndMinPulse(void)
{
    DeadTimeStack1RAW[0] = MCLIB_s16Mul_s16u32(DeadTimeStack1[0], sControlConfig.u32DeadTimeNanoSecondsToTicksMultiplier);
    DeadTimeStack1RAW[1] = MCLIB_s16Mul_s16u32(DeadTimeStack1[1], sControlConfig.u32DeadTimeNanoSecondsToTicksMultiplier);
    DeadTimeStack1RAW[2] = MCLIB_s16Mul_s16u32(DeadTimeStack1[2], sControlConfig.u32DeadTimeNanoSecondsToTicksMultiplier);
    
    DeadTimeStack2RAW[0] = MCLIB_s16Mul_s16u32(DeadTimeStack2[0], sControlConfig.u32DeadTimeNanoSecondsToTicksMultiplier);
    DeadTimeStack2RAW[1] = MCLIB_s16Mul_s16u32(DeadTimeStack2[1], sControlConfig.u32DeadTimeNanoSecondsToTicksMultiplier);
    DeadTimeStack2RAW[2] = MCLIB_s16Mul_s16u32(DeadTimeStack2[2], sControlConfig.u32DeadTimeNanoSecondsToTicksMultiplier);
   
    
    MinPulseStack1RAW    = MCLIB_s16Mul_s16u32(MinPulseStack1   , sControlConfig.u32MinPulseNanoSecondsToTicksMultiplier);
    MinPulseStack2RAW    = MCLIB_s16Mul_s16u32(MinPulseStack2   , sControlConfig.u32MinPulseNanoSecondsToTicksMultiplier);
    
    if (bUsePMPwithControl)
    {  
        if (bSetupDeadTimeRequest)
        {
            bSetupDeadTimeRequest = 0;
#if HMDRV_USE_EXT_PMP
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_WADDR_UNLOCK), &u8PMPConfigurationUnlockKeyPWM      , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_WADDR_DTL   ), (HMDRV_BPTR)(&DeadTimeStack1RAW[0])+0, 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_WADDR_DTH   ), (HMDRV_BPTR)(&DeadTimeStack1RAW[0])+1, 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_WADDR_UNLOCK), &u8PMPConfigurationLockKeyPWM        , 1);
#endif
        }
        if (bSetupMinPulseRequest)
        {
            bSetupMinPulseRequest = 0;
#if HMDRV_USE_EXT_PMP
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_WADDR_UNLOCK), &u8PMPConfigurationUnlockKeyPWM     , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_WADDR_MPULSL), (HMDRV_BPTR)(&MinPulseStack1RAW)+0  , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_WADDR_MPULSH), (HMDRV_BPTR)(&MinPulseStack1RAW)+1  , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_WADDR_UNLOCK), &u8PMPConfigurationLockKeyPWM       , 1);
#endif
        }
        
        if (bSetupDETAndGDEMaskRequest)
        {
            bSetupDETAndGDEMaskRequest = 0;
#if HMDRV_USE_EXT_PMP
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_WADDR_UNLOCK), &u8PMPConfigurationUnlockKeyPWM     , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_WADDR_MDET  ), (HMDRV_BPTR)(&uMaskDET)+0           , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_WADDR_MGDE  ), (HMDRV_BPTR)(&uMaskGDE)+0           , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_WADDR_UNLOCK), &u8PMPConfigurationLockKeyPWM       , 1);
#endif
        }
    }
    
    
}

void FPGA_PWM_vGetDeadTimeAndMinPulse(void)
{
    if (bUsePMPwithControl)
    {
#if HMDRV_USE_EXT_PMP
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)(&DeadTimeStack1FbkRAW[0])+0,    (PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_RADDR_DTL   ), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)(&DeadTimeStack1FbkRAW[0])+1,    (PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_RADDR_DTH   ), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)(&MinPulseStack1FbkRAW)+0,       (PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_RADDR_MPULSL), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)(&MinPulseStack1FbkRAW)+1,       (PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_RADDR_MPULSH), 1);
        
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&uMaskDETFbk,                    (PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_RADDR_MDET  ), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&uMaskGDEFbk,                    (PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_RADDR_MGDE  ), 1);
#endif
    }

    DeadTimeStack1Fbk[0] = MCLIB_s16Mul_s16u32(DeadTimeStack1FbkRAW[0], sControlConfig.u32DeadTimeTicksToNanoSecondsMultiplier);
    DeadTimeStack1Fbk[1] = MCLIB_s16Mul_s16u32(DeadTimeStack1FbkRAW[1], sControlConfig.u32DeadTimeTicksToNanoSecondsMultiplier);
    DeadTimeStack1Fbk[2] = MCLIB_s16Mul_s16u32(DeadTimeStack1FbkRAW[2], sControlConfig.u32DeadTimeTicksToNanoSecondsMultiplier);
    
    DeadTimeStack2Fbk[0] = MCLIB_s16Mul_s16u32(DeadTimeStack2FbkRAW[0], sControlConfig.u32DeadTimeTicksToNanoSecondsMultiplier);
    DeadTimeStack2Fbk[1] = MCLIB_s16Mul_s16u32(DeadTimeStack2FbkRAW[1], sControlConfig.u32DeadTimeTicksToNanoSecondsMultiplier);
    DeadTimeStack2Fbk[2] = MCLIB_s16Mul_s16u32(DeadTimeStack2FbkRAW[2], sControlConfig.u32DeadTimeTicksToNanoSecondsMultiplier);
    
    MinPulseStack1Fbk    = MCLIB_s16Mul_s16u32(MinPulseStack1FbkRAW   , sControlConfig.u32MinPulseTicksToNanoSecondsMultiplier);
    MinPulseStack2Fbk    = MCLIB_s16Mul_s16u32(MinPulseStack2FbkRAW   , sControlConfig.u32MinPulseTicksToNanoSecondsMultiplier);
    
}

#if FPGA_CFG_USE_DIDO_FPGA == 0
void CONTROL_vSetDeadTimeAndMinPulse(void)
{
    DeadTimeStack1RAW[0] = MCLIB_s16Mul_s16u32(DeadTimeStack1[0], sControlConfig.u32DeadTimeNanoSecondsToTicksMultiplier);
    DeadTimeStack1RAW[1] = MCLIB_s16Mul_s16u32(DeadTimeStack1[1], sControlConfig.u32DeadTimeNanoSecondsToTicksMultiplier);
    DeadTimeStack1RAW[2] = MCLIB_s16Mul_s16u32(DeadTimeStack1[2], sControlConfig.u32DeadTimeNanoSecondsToTicksMultiplier);
    
    DeadTimeStack2RAW[0] = MCLIB_s16Mul_s16u32(DeadTimeStack2[0], sControlConfig.u32DeadTimeNanoSecondsToTicksMultiplier);
    DeadTimeStack2RAW[1] = MCLIB_s16Mul_s16u32(DeadTimeStack2[1], sControlConfig.u32DeadTimeNanoSecondsToTicksMultiplier);
    DeadTimeStack2RAW[2] = MCLIB_s16Mul_s16u32(DeadTimeStack2[2], sControlConfig.u32DeadTimeNanoSecondsToTicksMultiplier);
    
    if (bUsePMPwithControl)
    {   
#if HMDRV_USE_EXT_PMP
        //PMPMEM_vWriteToExtMem((PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK1_WR), &DeadTimeStack1RAW[0], 6);
        //PMPMEM_vWriteToExtMem((PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK2_WR), &DeadTimeStack2RAW[0], 6);
        PMPMEM_vWriteToExtMem((PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK1_PHASEA_WR), &DeadTimeStack1RAW[0], 2);
        PMPMEM_vWriteToExtMem((PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK1_PHASEB_WR), &DeadTimeStack1RAW[1], 2);
        PMPMEM_vWriteToExtMem((PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK1_PHASEC_WR), &DeadTimeStack1RAW[2], 2);
        PMPMEM_vWriteToExtMem((PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK2_PHASEA_WR), &DeadTimeStack2RAW[0], 2);
        PMPMEM_vWriteToExtMem((PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK2_PHASEB_WR), &DeadTimeStack2RAW[1], 2);
        PMPMEM_vWriteToExtMem((PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK2_PHASEC_WR), &DeadTimeStack2RAW[2], 2);
#endif
    }
}


void CONTROL_vGetDeadTimeAndMinPulse(void)
{
    if (bUsePMPwithControl)
    {
#if HMDRV_USE_EXT_PMP
        //PMPMEM_vReadFromExtMem(&DeadTimeStack1FbkRAW[0], (PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK1), 6);
        //PMPMEM_vReadFromExtMem(&DeadTimeStack2FbkRAW[0], (PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK2), 6);
        PMPMEM_vReadFromExtMem(&DeadTimeStack1FbkRAW[0], (PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK1_PHASEA), 2);
        PMPMEM_vReadFromExtMem(&DeadTimeStack1FbkRAW[1], (PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK1_PHASEB), 2);
        PMPMEM_vReadFromExtMem(&DeadTimeStack1FbkRAW[2], (PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK1_PHASEC), 2);
        PMPMEM_vReadFromExtMem(&DeadTimeStack2FbkRAW[0], (PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK2_PHASEA), 2);
        PMPMEM_vReadFromExtMem(&DeadTimeStack2FbkRAW[1], (PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK2_PHASEB), 2);
        PMPMEM_vReadFromExtMem(&DeadTimeStack2FbkRAW[2], (PMP_ADDRESS_FPGA_PWM_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_PWM_DEAD_TIME_STACK2_PHASEC), 2);
#endif
    }

    DeadTimeStack1Fbk[0] = MCLIB_s16Mul_s16u32(DeadTimeStack1FbkRAW[0], sControlConfig.u32DeadTimeTicksToNanoSecondsMultiplier);
    DeadTimeStack1Fbk[1] = MCLIB_s16Mul_s16u32(DeadTimeStack1FbkRAW[1], sControlConfig.u32DeadTimeTicksToNanoSecondsMultiplier);
    DeadTimeStack1Fbk[2] = MCLIB_s16Mul_s16u32(DeadTimeStack1FbkRAW[2], sControlConfig.u32DeadTimeTicksToNanoSecondsMultiplier);
    
    DeadTimeStack2Fbk[0] = MCLIB_s16Mul_s16u32(DeadTimeStack2FbkRAW[0], sControlConfig.u32DeadTimeTicksToNanoSecondsMultiplier);
    DeadTimeStack2Fbk[1] = MCLIB_s16Mul_s16u32(DeadTimeStack2FbkRAW[1], sControlConfig.u32DeadTimeTicksToNanoSecondsMultiplier);
    DeadTimeStack2Fbk[2] = MCLIB_s16Mul_s16u32(DeadTimeStack2FbkRAW[2], sControlConfig.u32DeadTimeTicksToNanoSecondsMultiplier);
}
#endif

#endif  /* #if BOARDCFG_USE_FPGA_PWM */
