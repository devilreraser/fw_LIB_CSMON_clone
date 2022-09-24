/* *****************************************************************************
 * File:   fpga_trip.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 08 14 12:51
 * 
 * Description: FPGA AP TRIP (Analog PWM board TRIP fpga)
 * 
 **************************************************************************** */
#ifdef _WIN32
#pragma once
#endif /* _WIN32 */

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "boardcfg.h"

#if BOARDCFG_USE_FPGA_TRIP

#include "fpga_cfg.h"
#include "fpga_common.h"
#include "fpga_trip.h"
#include "dio.h"
#include "control_common.h"

#include "hmlib.h"
#if HMDRV_USE_EXT_PMP
#include "pmpmem.h"
#include "control.h"
#endif

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

/* TRIP FPGA */
/*
constant CLOCKS_CYCLE : natural := 24; -- used clock is 240MHz each clock gives 4.16ns delay. Minimum value is 2
constant PMALL_COUNT : natural := 3;
constant PMALH_COUNT : natural := 14;
constant CS_INIT_VAL : STD_LOGIC_VECTOR(CLOCKS_CYCLE - 2 downto 0) := (OTHERS => '0');

constant VERSION		: STD_LOGIC_VECTOR(7 downto 0):= X"01";
constant REVISION		: STD_LOGIC_VECTOR(7 downto 0):= X"01";

constant ADDR_FPGA		: STD_LOGIC_VECTOR(5 downto 0):= "000001";

constant UNLOCK_KEY		: STD_LOGIC_VECTOR(7 downto 0):= X"A5";


constant WADDR_UNLOCK	: STD_LOGIC_VECTOR(7 downto 0):= X"80"; 
constant WADDR_MOCA		: STD_LOGIC_VECTOR(7 downto 0):= X"11";
constant WADDR_MOCB		: STD_LOGIC_VECTOR(7 downto 0):= X"12";
constant WADDR_MOCFE	: STD_LOGIC_VECTOR(7 downto 0):= X"13";
constant WADDR_MOCFW	: STD_LOGIC_VECTOR(7 downto 0):= X"14";
constant WADDR_MOV		: STD_LOGIC_VECTOR(7 downto 0):= X"15";


constant RADDR_UNLOCK	: STD_LOGIC_VECTOR(7 downto 0):= X"80"; 
constant RADDR_VER		: STD_LOGIC_VECTOR(7 downto 0):= X"81";
constant RADDR_REV		: STD_LOGIC_VECTOR(7 downto 0):= X"82";
constant RADDR_FREG		: STD_LOGIC_VECTOR(7 downto 0):= X"00";
constant RADDR_OCA		: STD_LOGIC_VECTOR(7 downto 0):= X"01";
constant RADDR_OCB		: STD_LOGIC_VECTOR(7 downto 0):= X"02";
constant RADDR_OCFE		: STD_LOGIC_VECTOR(7 downto 0):= X"03";
constant RADDR_OCFW		: STD_LOGIC_VECTOR(7 downto 0):= X"04";
constant RADDR_OV		: STD_LOGIC_VECTOR(7 downto 0):= X"05";
constant RADDR_SYS		: STD_LOGIC_VECTOR(7 downto 0):= X"10";
constant RADDR_MOCA		: STD_LOGIC_VECTOR(7 downto 0):= X"11";
constant RADDR_MOCB		: STD_LOGIC_VECTOR(7 downto 0):= X"12";
constant RADDR_MOCFE	: STD_LOGIC_VECTOR(7 downto 0):= X"13";
constant RADDR_MOCFW	: STD_LOGIC_VECTOR(7 downto 0):= X"14";
constant RADDR_MOV		: STD_LOGIC_VECTOR(7 downto 0):= X"15";

signal s_flt_reg  : std_logic_vector(7 downto 0);
-- s_flt_reg(7) - Global Fault; 
-- s_flt_reg(5) - Overvoltage; 
-- s_flt_reg(4) - Overcurrent; 
-- s_flt_reg(3) - Overcurrent FE; 
-- s_flt_reg(2) - Overcurrent FW; 
-- s_flt_reg(1) - Overcurrent B; 
-- s_flt_reg(0) - Overcurrent A; 
 * 
 * 
 * 
*/

#define PMP_ADDRESS_FPGA_TRIP_ADDRESS_OFFSET                0x0100

#define PMP_ADDRESS_FPGA_TRIP_RADDR_UNLOCK                  0x0080  /* 1 byte */
#define PMP_ADDRESS_FPGA_TRIP_RADDR_VER                     0x0081  /* 1 byte */
#define PMP_ADDRESS_FPGA_TRIP_RADDR_REV                     0x0082  /* 1 byte */
#define PMP_ADDRESS_FPGA_TRIP_RADDR_FREG                    0x0000  /* 1 byte */
#define PMP_ADDRESS_FPGA_TRIP_RADDR_OCA                     0x0001  /* 1 byte */
#define PMP_ADDRESS_FPGA_TRIP_RADDR_OCB                     0x0002  /* 1 byte */
#define PMP_ADDRESS_FPGA_TRIP_RADDR_OCFE                    0x0003  /* 1 byte */
#define PMP_ADDRESS_FPGA_TRIP_RADDR_OCFW                    0x0004  /* 1 byte */
#define PMP_ADDRESS_FPGA_TRIP_RADDR_OV                      0x0005  /* 1 byte */
#define PMP_ADDRESS_FPGA_TRIP_RADDR_SYS                     0x0010  /* 1 byte */
#define PMP_ADDRESS_FPGA_TRIP_RADDR_MOCA                    0x0011  /* 1 byte */
#define PMP_ADDRESS_FPGA_TRIP_RADDR_MOCB                    0x0012  /* 1 byte */
#define PMP_ADDRESS_FPGA_TRIP_RADDR_MOCFE                   0x0013  /* 1 byte */
#define PMP_ADDRESS_FPGA_TRIP_RADDR_MOCFW                   0x0014  /* 1 byte */
#define PMP_ADDRESS_FPGA_TRIP_RADDR_MOV                     0x0015  /* 1 byte */

#define PMP_ADDRESS_FPGA_TRIP_WADDR_UNLOCK                  0x0080  /* 1 byte */
#define PMP_ADDRESS_FPGA_TRIP_WADDR_MOCA                    0x0011  /* 1 byte */
#define PMP_ADDRESS_FPGA_TRIP_WADDR_MOCB                    0x0012  /* 1 byte */
#define PMP_ADDRESS_FPGA_TRIP_WADDR_MOCFE                   0x0013  /* 1 byte */
#define PMP_ADDRESS_FPGA_TRIP_WADDR_MOCFW                   0x0014  /* 1 byte */
#define PMP_ADDRESS_FPGA_TRIP_WADDR_MOV                     0x0015  /* 1 byte */



#if FPGA_CFG_USE_DIDO_FPGA == 0
#define PMP_ADDRESS_FPGA_TRIP_ADDRESS_OFFSET                0x3F00

#define PMP_ADDRESS_FPGA_TRIP_STATUS_OCA_OCB                0x0000  /* 2 bytes */
#define PMP_ADDRESS_FPGA_TRIP_STATUS_OV_OCFE                0x0002  /* 2 bytes - includes bits for warnings */
#define PMP_ADDRESS_FPGA_TRIP_STATUS_OCFW                   0x0004  /* 1 byte */
//			ADR_OCA 	: natural							:= 0000;	--  Neg N3  N2  N1  Pos P3  P2  P1
//			ADR_OCB 	: natural							:= 0001;	--  Neg N3  N2  N1  Pos P3  P2  P1
//			ADR_OV  	: natural							:= 0002;	--  UB4 UB3 UB2 UB1 UA4 UA3 UA2 UA1
//			ADR_OCFE	: natural							:= 0003;	--  NFW N3  N2  N1  PFW P3  P2  P1	NFW - Neg Fault Warning PFW - Pos Fault Warning	
//			ADR_OCFW	: natural							:= 0004;	--  Neg N3  N2  N1  Pos P3  P2  P1
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
uControlTrip_t uTripRequest;
uControlTrip_t uTripFbkOC;     /* Read Data From FPGA */      
uControlTrip_t uTripFbkOV_OCFE;     /* Read Data From FPGA */      
uControlTrip_t uTripFbkOCFW;     /* Read Data From FPGA */      
uControlTrip_t uControlTripFbkOC;     /* Read Data From FPGA */      
uControlTrip_t uControlTripFbkOV_OCFE;     /* Read Data From FPGA */      
uControlTrip_t uControlTripFbkOCFW;     /* Read Data From FPGA */ 

uFREGTrip_t     uFREGTrip = {0};


uOCTrip_t StatOCA   = {0xFF};
uOCTrip_t StatOCB   = {0xFF};
uOCTrip_t StatOCFE  = {0xFF};
uOCTrip_t StatOCFW  = {0xFF};
uOVTrip_t StatOV    = {0xFF};

uOCTrip_t MaskOCA   = {0xFF};
uOCTrip_t MaskOCB   = {0x00};
uOCTrip_t MaskOCFE  = {0x00};
uOCTrip_t MaskOCFW  = {0x00};
uOVTrip_t MaskOV    = {0x01};

uOCTrip_t MaskOCAFbk    = {0};
uOCTrip_t MaskOCBFbk    = {0};
uOCTrip_t MaskOCFEFbk   = {0};
uOCTrip_t MaskOCFWFbk   = {0};
uOVTrip_t MaskOVFbk     = {0};

bool bSetupTripMaskConfiguration = 0;

uint8_t u8PMPConfigurationUnlockKeyTRIP = PMP_CONFIGURATION_UNLOCK_KEY;
uint8_t u8PMPConfigurationLockKeyTRIP   = 0x00;


/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */
bool FPGA_TRIP_vGetTripError(void)
{
    bool bResult = false;
    
    if ((uFREGTrip.sFREGTrip.bGlobalFault > 0) 
            || ((StatOCA.u8Register & MaskOCA.u8Register) > 0)  
            || ((StatOCB.u8Register & MaskOCB.u8Register) > 0)  
            || ((StatOCFE.u8Register & MaskOCFE.u8Register) > 0)  
            || ((StatOCFW.u8Register & MaskOCFW.u8Register) > 0) 
            || ((StatOV.u8Register & MaskOV.u8Register) > 0) 
            || (uFaultStatus.sFaultInput.bFLT2 > 0))
    {
        bResult = true;
    }
    return bResult;
}

void FPGA_TRIP_vClearTrip(void)
{
    
}

void FPGA_TRIP_vClearErrorStatus(void)
{
    uFREGTrip.u16Register = 0;
    StatOCA.u8Register = 0;
    StatOCB.u8Register = 0;
    StatOCFE.u8Register = 0;
    StatOCFW.u8Register = 0;
    StatOV.u8Register = 0;
    uFaultStatus.u8Register = 0;
}  



void FPGA_TRIP_vGetTripStat(void)
{
    
#if HMDRV_USE_EXT_PMP
    if (bUsePMPwithControl)
    {
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&uFREGTrip,  (PMP_ADDRESS_FPGA_TRIP_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_TRIP_RADDR_FREG), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&StatOCA,    (PMP_ADDRESS_FPGA_TRIP_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_TRIP_RADDR_OCA ), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&StatOCB,    (PMP_ADDRESS_FPGA_TRIP_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_TRIP_RADDR_OCB ), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&StatOCFE,   (PMP_ADDRESS_FPGA_TRIP_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_TRIP_RADDR_OCFE), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&StatOCFW,   (PMP_ADDRESS_FPGA_TRIP_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_TRIP_RADDR_OCFW), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&StatOV,     (PMP_ADDRESS_FPGA_TRIP_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_TRIP_RADDR_OV  ), 1);
    }
#endif
    
    
    
}

void FPGA_TRIP_vRunTimeSetupProcess(void)
{
    if ( (*((uint8_t*)&MaskOCAFbk)   != *((uint8_t*)&MaskOCA))
      || (*((uint8_t*)&MaskOCBFbk)   != *((uint8_t*)&MaskOCB))
      || (*((uint8_t*)&MaskOCFEFbk)  != *((uint8_t*)&MaskOCFE))
      || (*((uint8_t*)&MaskOCFWFbk)  != *((uint8_t*)&MaskOCFW))
      || (*((uint8_t*)&MaskOVFbk)    != *((uint8_t*)&MaskOV)) )
    {
        bSetupTripMaskConfiguration = 1;
    }

}



void FPGA_TRIP_vMaskConfigure (void)
{
    if (bSetupTripMaskConfiguration)
    {
            bSetupTripMaskConfiguration = 0;
            
#if HMDRV_USE_EXT_PMP
        if (bUsePMPwithControl)
        {           
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_TRIP_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_TRIP_WADDR_UNLOCK), &u8PMPConfigurationUnlockKeyTRIP         , 1);
          
        //DEBUG_SET_MAIN();  
        
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_TRIP_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_TRIP_WADDR_MOCA),     (HMDRV_BPTR)&MaskOCA            , 1);
        //DEBUG_CLR_MAIN();
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_TRIP_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_TRIP_WADDR_MOCB),     (HMDRV_BPTR)&MaskOCB            , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_TRIP_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_TRIP_WADDR_MOCFE),    (HMDRV_BPTR)&MaskOCFE           , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_TRIP_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_TRIP_WADDR_MOCFW),    (HMDRV_BPTR)&MaskOCFW           , 1);
            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_TRIP_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_TRIP_WADDR_MOV),      (HMDRV_BPTR)&MaskOV             , 1);

            PMPMEM_vWriteToExtMemSingle((PMP_ADDRESS_FPGA_TRIP_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_TRIP_WADDR_UNLOCK), &u8PMPConfigurationLockKeyTRIP           , 1);
        }
#endif
    }
    
}

void FPGA_TRIP_vGetTripMasks(void)
{
#if HMDRV_USE_EXT_PMP
    if (bUsePMPwithControl)
    {
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&MaskOCAFbk,   (PMP_ADDRESS_FPGA_TRIP_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_TRIP_RADDR_MOCA ), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&MaskOCBFbk,   (PMP_ADDRESS_FPGA_TRIP_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_TRIP_RADDR_MOCB ), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&MaskOCFEFbk,  (PMP_ADDRESS_FPGA_TRIP_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_TRIP_RADDR_MOCFE), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&MaskOCFWFbk,  (PMP_ADDRESS_FPGA_TRIP_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_TRIP_RADDR_MOCFW), 1);
        PMPMEM_vReadFromExtMemSingle((HMDRV_BPTR)&MaskOVFbk,    (PMP_ADDRESS_FPGA_TRIP_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_TRIP_RADDR_MOV  ), 1);
    }
#endif
}


#if FPGA_CFG_USE_DIDO_FPGA == 0
void CONTROL_vClearTrip(void)
{
    uControlTripFbkOC.u16Register = 0;  /* clear trip signals */ 
    uControlTripFbkOV_OCFE.u16Register = 0;  /* clear trip signals */ 
    uControlTripFbkOCFW.u16Register = 0;  /* clear trip signals */ 
}
void CONTROL_vGetTripStat(void)
{
#if HMDRV_USE_EXT_PMP
    if (bUsePMPwithControl)
    {
        //PMPMEM_vWriteToExtMem((PMP_ADDRESS_FPGA_TRIP_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_TRIP_STATUS), &uTripRequest, 2);
        PMPMEM_vReadFromExtMem(&uTripFbkOC.u16Register, (PMP_ADDRESS_FPGA_TRIP_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_TRIP_STATUS_OCA_OCB), 2);
        PMPMEM_vReadFromExtMem(&uTripFbkOV_OCFE.u16Register, (PMP_ADDRESS_FPGA_TRIP_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_TRIP_STATUS_OV_OCFE), 2);
        PMPMEM_vReadFromExtMem(&uTripFbkOCFW.u16Register, (PMP_ADDRESS_FPGA_TRIP_ADDRESS_OFFSET + PMP_ADDRESS_FPGA_TRIP_STATUS_OCFW), 1);
    }
#endif

    uControlTripFbkOC.u16Register      = uTripFbkOC.u16Register;
    uControlTripFbkOV_OCFE.u16Register = uTripFbkOV_OCFE.u16Register;
    uControlTripFbkOCFW.u16Register    = uTripFbkOCFW.u16Register;
    
    if (
            (uControlTripFbkOC.u16Register != 0)
         || (uControlTripFbkOV_OCFE.u16Register != 0)   
         || (uControlTripFbkOCFW.u16Register != 0)   
        )
    {
        sControlAccess.bFault = 1;
    }
    
    
}
#endif




#endif /* BOARDCFG_USE_FPGA_TRIP */
