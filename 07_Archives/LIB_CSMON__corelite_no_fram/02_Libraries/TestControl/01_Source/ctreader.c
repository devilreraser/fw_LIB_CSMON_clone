/* *****************************************************************************
 * File:   ctreader.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 09 22 14:06
 * 
 * Description: ...
 * 
 **************************************************************************** */
#ifdef _WIN32
#pragma once
#endif /* _WIN32 */

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "boardcfg.h"

#if USE_CTREADER

#if BOARDCFG_USE_TSC_DSPIC

#define MEMORY_MODIFIER __eds__
#define SPACE_ATTRIBUTE __attribute__((space(eds)))
#include <xc.h>
#include <libq.h>

#else

#define MEMORY_MODIFIER
#define SPACE_ATTRIBUTE
//#include "sci_drv.h"

#endif

#include "ctreader.h"

#include "uart_drv.h"
//#include "timer.h"


#include "sys_drv.h"


/* signal mapping includes */

#if BOARDCFG_USE_FPGA_TRIP
#include "fpga_trip.h"
#endif

#if BOARDCFG_USE_DAC_TRIP
#include "dac_trip.h"
#endif

#if BOARDCFG_USE_FPGA_PWM
#include "fpga_pwm.h"
#endif

#if BOARDCFG_USE_FPGA_DIO
#include "fpga_dio.h"
#endif

#if USE_CONTROL
#include "control.h"
#endif
#if BOARDCFG_USE_TESTPROC
#include "testproc.h"
#endif

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */
/* uartcommon */
typedef enum
{
            RX_IDLE = 0,
            RX_HDR,
            RX_CMD,
            RX_LEN,
            RX_DATA,
            RX_CHSUM
} eusart_rx_state;


/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */

uint8_t CTREADER_u8UartIndex = CTREADER_UART_MODULE;



/* ser_async */
ushort ush_read_trace_idx;

/* uart3 */
MEMORY_MODIFIER uchar uch_asy3_tx_buf[UART_TX_BUFFER_SIZE_CTREADER], uch_asy3_rx_buf[UART_RX_BUFFER_SIZE_CTREADER];
uchar uch_asy3_tosend;
uchar uch_asy3_rx_cmd, uch_asy3_rx_len;
uint16_t uch_rx3_tout;

uint16_t CTREADER_nTxDelay;

uint16_t CTReaderTxDisableDelay;
bool bUseCTReaderTxDisableDelay = 1;


uchar uch_asy3_tx_idx, uch_asy3_rx_idx;
uchar uch_asy3_tx_chsum, uch_asy3_rx_chsum;
uchar uch_asy3_rx_bcnt;

eusart_rx_state  ers_rx3_state;

uchar CTREADER_nRxTodoEcho = 0;

uint16_t u8PostDummyCounter = 0;
uint16_t u8PostDummyCount = 0;

/* *****************************************************************************
 * Linkage Variables Definitions - data to do linkage
 **************************************************************************** */

uchar faultDetected = 1;    /* check start command given */


/* debug */
MEMORY_MODIFIER ushort ush_debug[DEBUG_CNT] SPACE_ATTRIBUTE;

/* data */
// measurements data
ushort ush_irms_sns1, ush_irms_sns2, ush_irms_sns3;
ushort ush_irms_mod1, ush_irms_mod2, ush_irms_mod3;
short sh_t_sns1, sh_t_sns2, sh_t_sns3;
short sh_t_mod1, sh_t_mod2, sh_t_mod3;
short sh_u_dcl;
ushort ush_u_dc;
ushort ush_urms_ul12, ush_urms_ul23, ush_urms_ul31;
ushort ush_ctr_status, ush_fbk_status;

// control data
ushort ush_swfreq = LOOPTIME1_HZ;
ushort ush_fund, ush_irms_set, ush_pwm_perc_set;
ushort ush_u_dcl_set;

uint16_t CTREADER_sh_u_dcl_set;

ushort ush_ctr, ush_ctr_set;

ushort sh_irms_addRound; 

// control data for set new value
ushort ush_fund_new, ush_irms_set_new;

// overcurrent, overvoltage & overtemperature
ushort ush_oc_level, ush_ov1_level, ush_ov2_level, ush_ot_level; 

// scaling factor for current and voltage
ushort ush_cur_scale, ush_scur_scale,  ush_volt_scale;
ushort ush_max_lnvolt, ush_max_current, ush_max_dc, ush_curr_diff;

// PID coefficients
short sh_kP, sh_kI;

// errors AP board
ushort ush_err_ovoltcur, ush_err_modules, ush_det_modules;
DetMod dm_det_modules;

MEMORY_MODIFIER short sh_trace[TRACE_LENGTH][10] SPACE_ATTRIBUTE;
ushort ush_trace_idx;

/* applstat */
ApplStat as_appl_status;
MEStat mes_mainerr_status;

/* adc1 */
short sh_adc1_cal[NUM1_CH];
#define adc1_calc_offset()

/* adc2 */
#define adc2_calc_offset()


/* dac */
ushort ush_s_in_oref[3], ush_cur_in_oref[3], ush_u_in_oref[2], ush_s_ee_oref[3], ush_cur_ee_oref[3], ush_u_ee_oref[2];
ushort ush_oc_s_level_dac, ush_oc_cur_level_dac, ush_ov1_level_dac, ush_ov2_level_dac; 

/* pwm */
//_Q16 q16_irms_diff, q16_irms_set;
#define pwm_init()

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */
void CTREADER_vStartUartTx(void);

/* *****************************************************************************
 * Functions
 **************************************************************************** */

uint16_t CTREADER_u16TimeLast = 0;
uint16_t CTREADER_u16TimePass = 0;
uint16_t CTREADER_u16TimeCurr = 0;

void CTREADER_vProcessUartRxTxTimeout(void)
{
    /* Calculate TimePassed From Previous Call */
    CTREADER_u16TimeCurr = TIMER_vGetTimerMainRollingCounter();
    CTREADER_u16TimePass = CTREADER_u16TimeCurr - CTREADER_u16TimeLast;
    CTREADER_u16TimeLast = CTREADER_u16TimeCurr;
    
    if (uch_rx3_tout > CTREADER_u16TimePass)
    {
        uch_rx3_tout -= CTREADER_u16TimePass;
    }
    else
    {
        if (uch_rx3_tout > 0)
        {
            ers_rx3_state = RX_IDLE;
        }
        uch_rx3_tout = 0;
    }


    if (CTREADER_nTxDelay > CTREADER_u16TimePass)
    {
        CTREADER_nTxDelay -= CTREADER_u16TimePass;
    }
    else
    {
        if (CTREADER_nTxDelay > 0)
        {
            CTREADER_vStartUartTx();
        }
        CTREADER_nTxDelay = 0;
    }

    if (CTReaderTxDisableDelay > CTREADER_u16TimePass)
    {
        CTReaderTxDisableDelay -= CTREADER_u16TimePass;
    }
    else
    {
        if (CTReaderTxDisableDelay > 0)
        {
            UART_TransmitDisable((UART_eModule_t)CTREADER_u8UartIndex);
        }
        CTReaderTxDisableDelay = 0;
    }


}

void CTREADER_vProcessUartRx(void)
{
   uchar rx_data;
   
   CTREADER_vProcessUartRxTxTimeout();
   
   while((UART_ReadDataRxAvailable((UART_eModule_t)CTREADER_u8UartIndex)) && (as_appl_status.asy3_cmd_recvd != TRUE))
   {
        uch_rx3_tout = RX_TOUT;
        // buffer overruns are ignored
        rx_data = UART_ReadDataRx((UART_eModule_t)CTREADER_u8UartIndex);
        uch_asy3_rx_chsum += rx_data;
        switch(ers_rx3_state){
            case RX_IDLE : 
                if(0xAA == rx_data) 
                {
                    ers_rx3_state = RX_HDR;
                }
                break;
            case RX_HDR :
                if(0x55 == rx_data) 
                {
                    ers_rx3_state = RX_CMD;
                    uch_asy3_rx_chsum = 0x00;
                } 
                else if(0xAA != rx_data) 
                {
                    ers_rx3_state = RX_IDLE;
                }
                break;
            case RX_CMD :
                uch_asy3_rx_cmd = rx_data;
                ers_rx3_state = RX_LEN;
                break;
            case RX_LEN :
                if(rx_data)
                {
                    uch_asy3_rx_bcnt = uch_asy3_rx_len = rx_data;
                    ers_rx3_state = RX_DATA;
                    uch_asy3_rx_idx = 0;
                } 
                else 
                {
                    ers_rx3_state = RX_CHSUM;
                }
                break;
            case RX_DATA :
                uch_asy3_rx_buf[uch_asy3_rx_idx++] = rx_data;
                if(!(--uch_asy3_rx_bcnt))
                {
                    ers_rx3_state = RX_CHSUM;
                }
                break;
            case RX_CHSUM :
                if(!(uch_asy3_rx_chsum & 0x00FF))
                {
                    as_appl_status.asy3_cmd_recvd = TRUE;
                    //while(UART_ReadDataRxAvailable(CTREADER_u8UartIndex))rx_data = U3RXREG; consider needed or not
                    ers_rx3_state = RX_IDLE;
                }
                else
                {
                    ers_rx3_state = RX_IDLE;        /* if not here disconnection is by timeout */
                }
            default:
                ers_rx3_state = RX_IDLE;
        }

   }
}



void CTREADER_vStartUartTx(void)
{
    uch_asy3_tx_idx = uch_asy3_tx_chsum = 0;
    if(uch_asy3_tosend)
    {
        UART_TransmitEnable((UART_eModule_t)CTREADER_u8UartIndex);
        UART_PutChar((UART_eModule_t)CTREADER_u8UartIndex, uch_asy3_tx_buf[uch_asy3_tx_idx++]);
        UART_TxIntEna((UART_eModule_t)CTREADER_u8UartIndex);    /* transmit complete interrupt (interrupt when Char is transferred) */
    }
}

void CTREADER_vProcessUartTx(uint8_t u8Index)
{
    if (CTREADER_u8UartIndex == u8Index)
    {
        if(uch_asy3_tx_idx < uch_asy3_tosend)
        {
            if(uch_asy3_tx_idx > 1)
            {
                uch_asy3_tx_chsum += uch_asy3_tx_buf[uch_asy3_tx_idx];
            }
            UART_PutChar((UART_eModule_t)u8Index, uch_asy3_tx_buf[uch_asy3_tx_idx++]);
            u8PostDummyCounter = u8PostDummyCount;

        } 
        else 
        {
            if (u8PostDummyCounter == u8PostDummyCount)
            {
                UART_PutChar((UART_eModule_t)u8Index, ((0 - uch_asy3_tx_chsum) & 0x00FF));
                uch_asy3_tx_chsum = 0x55;
                CTREADER_nRxTodoEcho += u8PostDummyCount;
            }
            else
            {
                UART_PutChar((UART_eModule_t)u8Index, uch_asy3_tx_chsum);
            }

            if (u8PostDummyCounter > 0)
            {
                u8PostDummyCounter--;
            }
            else //if (u8PostDummyCounter == 0)
            {
                UART_TxIntDis((UART_eModule_t)u8Index);
                if (bUseCTReaderTxDisableDelay)
                {
                    CTReaderTxDisableDelay = TX_DIS_DLAY;
                }
            }
        }       
    }
}

void CTREADER_vProcessFastTD (uint8_t u8Index)
{
    if (CTREADER_u8UartIndex == u8Index)
    {
        
        if (CTREADER_nRxTodoEcho > 0)
        {
            CTREADER_nRxTodoEcho--;
            if (CTREADER_nRxTodoEcho == 0)
            {
                if (bUseCTReaderTxDisableDelay == 0)
                {
                    UART_TransmitDisable((UART_eModule_t)u8Index);
                }
                else
                {
                    //CTReaderTxDisableDelay = TX_DIS_DLAY;
                }
            }
        }
    }
}


void CTREADER_vUartOverflowDetection(void)
{
    ers_rx3_state = RX_IDLE;
}





ushort uCommunicationState;

/* ser_async */
//void ser3_async_main(void)
void CTREADER_vProcess(void)
{
    
    uCommunicationState = 0;
    uchar i;
    ushort ush_value;
    

    /* read signal mapping */
    if(as_appl_status.asy3_cmd_recvd)
    {
        /* CMD_GETDEB */
        ush_debug[0] = 0;
        ush_debug[1] = 0;
        ush_debug[2] = 0;
        ush_debug[3] = 0;
        ush_debug[4] = 0;
        ush_debug[5] = 0;
        ush_debug[6] = 0;
        ush_debug[7] = 0;
        ush_debug[8] = 0;
        ush_debug[9] = 0;
        ush_debug[10] = 0;
        ush_debug[11] = 0;
        ush_debug[12] = 0;
        ush_debug[13] = 0;
        ush_debug[14] = 0;
        ush_debug[15] = 0;
        ush_debug[16] = 0;
        ush_debug[17] = 0;
        ush_debug[18] = 0;
        ush_debug[19] = 0;



        
        
        /* CMD_GETDIG */
        #if BOARDCFG_USE_FPGA_DIO
        ush_fbk_status = uDIExternal.u16Register;
        #endif
        
        /* ush_err_ovoltcur */
        #if BOARDCFG_USE_FPGA_TRIP
        ush_err_ovoltcur_u ush_err_ovoltcur_s;
        ush_err_ovoltcur_s.u16Register = 0;
        ush_err_ovoltcur_s.sSignals.CurSnsOCN1 = StatOCA.sOC.OCN1;
        ush_err_ovoltcur_s.sSignals.CurSnsOCN2 = StatOCA.sOC.OCN2;
        ush_err_ovoltcur_s.sSignals.CurSnsOCN3 = StatOCA.sOC.OCN3;
        ush_err_ovoltcur_s.sSignals.CurSemOCN1 = StatOCFE.sOC.OCN1;
        ush_err_ovoltcur_s.sSignals.CurSemOCN2 = StatOCFE.sOC.OCN2;
        ush_err_ovoltcur_s.sSignals.CurSemOCN3 = StatOCFE.sOC.OCN3;
        ush_err_ovoltcur_s.sSignals.CurSnsOCP1 = StatOCA.sOC.OCP1;
        ush_err_ovoltcur_s.sSignals.CurSnsOCP2 = StatOCA.sOC.OCP2;
        ush_err_ovoltcur_s.sSignals.CurSnsOCP3 = StatOCA.sOC.OCP3;
        ush_err_ovoltcur_s.sSignals.CurSemOCP1 = StatOCFE.sOC.OCP1;
        ush_err_ovoltcur_s.sSignals.CurSemOCP2 = StatOCFE.sOC.OCP2;
        ush_err_ovoltcur_s.sSignals.CurSemOCP3 = StatOCFE.sOC.OCP3;
        ush_err_ovoltcur_s.sSignals.OVDetOV1 = StatOV.sOV.OVA1;
        ush_err_ovoltcur_s.sSignals.OVDetOV2 = StatOV.sOV.OVA2;
        ush_err_ovoltcur = ~ush_err_ovoltcur_s.u16Register;
        #endif
        
        #if BOARDCFG_USE_FPGA_PWM
        ush_err_modules_u ush_err_modules_s;
        ush_err_modules_s.u16Register = 0;
        ush_err_modules_s.sSignals.GEBot1 = uStatGDE.sGDE.ErrBot1;
        ush_err_modules_s.sSignals.GEBot2 = uStatGDE.sGDE.ErrBot2;
        ush_err_modules_s.sSignals.GEBot3 = uStatGDE.sGDE.ErrBot3;
        ush_err_modules_s.sSignals.GETop1 = uStatGDE.sGDE.ErrTop1;
        ush_err_modules_s.sSignals.GETop2 = uStatGDE.sGDE.ErrTop2;
        ush_err_modules_s.sSignals.GETop3 = uStatGDE.sGDE.ErrTop3;
        ush_err_modules_s.sSignals.DrvErrSem1 = uStatDET.sDET.ErrDET1;
        ush_err_modules_s.sSignals.DrvErrSem2 = uStatDET.sDET.ErrDET2;
        ush_err_modules_s.sSignals.DrvErrSem3 = uStatDET.sDET.ErrDET3;
        ush_err_modules = ~ush_err_modules_s.u16Register;
                
        dm_det_modules.ush = 0;
        dm_det_modules.SEM1 = uStatDET.sDET.DET1;
        dm_det_modules.SEM2 = uStatDET.sDET.DET2;
        dm_det_modules.SEM3 = uStatDET.sDET.DET3;
        #endif
        
        
        /* CMD_GETCOM */
        ush_irms_sns1 = MCLIB_s16Mul_s16u16(sControlData.sStack[STACK_1].sCurrentABCFbckRMSFilt.s16Arg1,sControlConfig.u16ADCResultToDeciAmperesMultVisual) << sControlConfig.u16ADCResultToDeciAmperesShftVisual;
        ush_irms_sns2 = MCLIB_s16Mul_s16u16(sControlData.sStack[STACK_1].sCurrentABCFbckRMSFilt.s16Arg2,sControlConfig.u16ADCResultToDeciAmperesMultVisual) << sControlConfig.u16ADCResultToDeciAmperesShftVisual;
        ush_irms_sns3 = MCLIB_s16Mul_s16u16(sControlData.sStack[STACK_1].sCurrentABCFbckRMSFilt.s16Arg3,sControlConfig.u16ADCResultToDeciAmperesMultVisual) << sControlConfig.u16ADCResultToDeciAmperesShftVisual;
        
        ush_irms_mod1 = MCLIB_s16Mul_s16u16(sControlData.sStack[STACK_1].sCFBKABCFbckRMSFilt.s16Arg1,sControlConfig.u16ADCResultToDeciAmperesMultiplierCFBK);
        ush_irms_mod2 = MCLIB_s16Mul_s16u16(sControlData.sStack[STACK_1].sCFBKABCFbckRMSFilt.s16Arg2,sControlConfig.u16ADCResultToDeciAmperesMultiplierCFBK);
        ush_irms_mod3 = MCLIB_s16Mul_s16u16(sControlData.sStack[STACK_1].sCFBKABCFbckRMSFilt.s16Arg3,sControlConfig.u16ADCResultToDeciAmperesMultiplierCFBK);
        
        sh_t_sns1 = 0;
        sh_t_sns2 = 0;
        sh_t_sns3 = 0;
        
        sh_t_mod1 = TempFbkStack1[0];
        sh_t_mod2 = TempFbkStack1[1];
        sh_t_mod3 = TempFbkStack1[2];
        
        sh_u_dcl = VoltageStack1;
        
        ush_urms_ul12 = 1;
        ush_urms_ul23 = 1;
        ush_urms_ul31 = 1;
        
        #if BOARDCFG_USE_FPGA_DIO
        ush_ctr_status = uDOExternalFbk.u16Register;
        #endif
        
        mes_mainerr_status.ush = 0;
        mes_mainerr_status.AUXPIC_ERROR = 0;
        mes_mainerr_status.DAC_ERROR = 0;
        mes_mainerr_status.EEPROM_ERROR = 0;
        mes_mainerr_status.HW_POWERTRAIN_ERROR = 0;
        mes_mainerr_status.HW_SWITCHGEAR_ERROR = 0;
        mes_mainerr_status.LOAD_OVERTEMPERATURE = 0;
        mes_mainerr_status.MAINPIC_ERROR = 0;
        mes_mainerr_status.PARAMS_ERROR = 0;
        mes_mainerr_status.SAFETY = 0;
        #if USE_CONTROL
        mes_mainerr_status.SW_CONTROL_ERROR = sControlAccess.bFault;
        #endif
        mes_mainerr_status.SW_LIMITS_ERROR = 0;
        #if BOARDCFG_USE_TESTPROC
        mes_mainerr_status.TEST_ERROR = bTestProcessErrorDetection;
        mes_mainerr_status.TEST_NOT_CALIBRATED = ~bTestCalibrationCompleted;
        #endif
        mes_mainerr_status.reserve3 = 0;
        mes_mainerr_status.reserve2 = 0;
        mes_mainerr_status.reserve1 = 0;

        /* as_appl_status - to do !!! consider additional mapping here or somewhere else */ 
        
        
        
        /* CMD_GETRUN */
        #if !defined(BOARDCFG_USE_MCC_ADC) && (BOARDCFG_USE_OLD_NON_MCC_ADC)
        ush_swfreq = ADCPWM_u16GetPWM1FreqHz();
        #endif
        ush_fund = SpeedStack1;
        
        sh_irms_addRound = (MCLIB_s16Mul_s16u32(MCLIB_s16Fractional(0.5),sControlConfig.u32DeciAmperesToADCResultMultiplier) >> 14);
        ush_irms_set = MCLIB_s16Mul_s16u32(MCLIB_s16AddSat_s16s16( sControlData.sStack[STACK_1].s16CurrentRMSReq1Ph , sh_irms_addRound),sControlConfig.u32ADCResultToDeciAmperesMultiplier);

        
        ush_u_dcl_set = CTREADER_sh_u_dcl_set;
        
        ush_pwm_perc_set = MCLIB_s16Mul_s16u32(sControlData.sStack[STACK_1].s16VoltageAmpScaled,MCLIB_u32FractionalQ_16_16((10.0 * 100.0) / 32768)) ;
        //ush_pwm_perc_set = MCLIB_s16Mul_s16u32(MCLIB_s16Fractional(52.5 / 100.0),MCLIB_u32FractionalQ_16_16((10.0 * 100.0) / 32768)) ;
        
        /* CMD_GETSYS */
        #if BOARDCFG_USE_DAC_TRIP
        ush_oc_level = TripCurrentStack1[0];
        ush_ov1_level = TripVoltageStack1;
        ush_ov2_level = TripVoltageStack2;
        ush_max_lnvolt = TripVoltageStack2;     //line voltage not measured
        ush_max_current = TripCFBKStack1Fbk;    //not implemented trip level compared with RMS current value  
        ush_max_dc = TripVoltageStack1;         
        #endif

        ush_ot_level = CONTROL_s16GetTripTemperature_dDeg();         //not implemented -> default must be 80 deg
        ush_curr_diff = 1000;    //not implemented
        ush_cur_scale = 1000;    //not implemented
        ush_scur_scale = 1000;    //not implemented
        ush_volt_scale = 1000;    //not implemented
        sh_kP = 1000;    //not implemented
        sh_kI = 1000;    //not implemented
        
        /* CMD_GETDCOFF */
        sh_adc1_cal[0] = MCLIB_s16Mul_s16u16( ADC2Stack1Offset[ADC2_STACK_CURRFBK_L1],sControlConfig.u16ADCResultToDeciAmperesMultiplierCFBK);      //ush_irms_mod1
        sh_adc1_cal[1] = MCLIB_s16Mul_s16u16( ADC2Stack1Offset[ADC2_STACK_CURRFBK_L2],sControlConfig.u16ADCResultToDeciAmperesMultiplierCFBK);      //ush_irms_mod2
        sh_adc1_cal[2] = MCLIB_s16Mul_s16u16( ADC2Stack1Offset[ADC2_STACK_CURRFBK_L3],sControlConfig.u16ADCResultToDeciAmperesMultiplierCFBK);      //ush_irms_mod2
        sh_adc1_cal[3] = MCLIB_s16Mul_s16u32( ADCStack1Offset[ADC_STACK_CURRENT_L1],sControlConfig.u32ADCResultToDeciAmperesMultiplier);      //ush_irms_sns1
        sh_adc1_cal[4] = MCLIB_s16Mul_s16u32( ADCStack1Offset[ADC_STACK_CURRENT_L2],sControlConfig.u32ADCResultToDeciAmperesMultiplier);      //ush_irms_sns2
        sh_adc1_cal[5] = MCLIB_s16Mul_s16u32( ADCStack1Offset[ADC_STACK_CURRENT_L3],sControlConfig.u32ADCResultToDeciAmperesMultiplier);      //ush_irms_sns3
        sh_adc1_cal[6] = 1000;      //ACVoltage Not Used
        sh_adc1_cal[7] = 1000;      //ACVoltage Not Used
        sh_adc1_cal[8] = MCLIB_s16Mul_s16u16( ADCStack1Offset[ADC_STACK_DC_VOLTAGE],sControlConfig.u16ADCResultToDeciVoltageMultiplier);      //sh_u_dcl
        sh_adc1_cal[9] = 1000;      //ACVoltage Not Used
        
        /* CMD_GETTRLVL */
        #if BOARDCFG_USE_DAC_TRIP
        ush_u_in_oref[0] = TripVoltageStack1Fbk;
        ush_s_in_oref[2] = TripCurrentStack1Fbk[2];
        ush_s_in_oref[1] = TripCurrentStack1Fbk[1];
        ush_s_in_oref[0] = TripCurrentStack1Fbk[0];
        ush_u_in_oref[1] = TripVoltageStack2Fbk;
        ush_cur_in_oref[2] = TripCFBKStack1Fbk;
        ush_cur_in_oref[1] = TripCFBKStack1Fbk;
        ush_cur_in_oref[0] = TripCFBKStack1Fbk;
        //??? below not sure this
        ush_u_ee_oref[0] = TripVoltageStack1;
        ush_s_ee_oref[2] = TripCurrentStack1[2];
        ush_s_ee_oref[1] = TripCurrentStack1[1];
        ush_s_ee_oref[0] = TripCurrentStack1[0];
        ush_u_ee_oref[1] = TripVoltageStack2;
        #endif
        #if BOARDCFG_USE_DAC_TRIP
        ush_cur_ee_oref[2] = TripCFBKStack1;
        ush_cur_ee_oref[1] = TripCFBKStack1;
        ush_cur_ee_oref[0] = TripCFBKStack1;
        ush_ov1_level_dac = TripVoltageStack1;
        ush_oc_s_level_dac = TripCurrentStack1[0];
        ush_ov2_level_dac = TripVoltageStack2;
        ush_oc_cur_level_dac = TripCFBKStack1;
        #endif

        /* CMD_GETTRACE */
        /* sh_trace fill - see pwm.c -> like recorder */
        

        
        
    }
    /* read signal mapping END */ 
    
    if(as_appl_status.asy3_cmd_recvd)
    {
        uch_asy3_tx_buf[0] = 0x55;
        uch_asy3_tx_buf[1] = 0xAA;
        uch_asy3_tx_buf[2] = uch_asy3_rx_cmd;
        i = 0;
        switch(uch_asy3_rx_cmd)
        {
            case CMD_GETDEB : 
                for(i = 0;i < DEBUG_CNT; i++)
                {
                    uch_asy3_tx_buf[4+i*2] = ush_debug[i] / 256;
                    uch_asy3_tx_buf[5+i*2] = ush_debug[i] & 0xff;
                }
                i *= 2;
                break;
                
            case CMD_GETDIG :
                uch_asy3_tx_buf[4] = ush_fbk_status / 256;
                uch_asy3_tx_buf[5] = ush_fbk_status & 0xFF;
                uch_asy3_tx_buf[6] = ush_ctr_status / 256;
                uch_asy3_tx_buf[7] = ush_ctr_status & 0xFF;
                uch_asy3_tx_buf[8] = ~(ush_err_ovoltcur / 256);
                uch_asy3_tx_buf[9] = (~(ush_err_ovoltcur & 0xFF))&0x3F;
                uch_asy3_tx_buf[10] = (~(ush_err_modules / 256))&0x3F;
                uch_asy3_tx_buf[11] = (~(ush_err_modules & 0xFF))&0x3F;
                uch_asy3_tx_buf[12] = dm_det_modules.uch[1];
                uch_asy3_tx_buf[13] = dm_det_modules.uch[0];
                i = 10;
                break;
            
            case CMD_GETCOM : 
                uch_asy3_tx_buf[4] = ush_irms_sns1 / 256;
                uch_asy3_tx_buf[5] = ush_irms_sns1 & 0xff;
                uch_asy3_tx_buf[6] = ush_irms_sns2 / 256;
                uch_asy3_tx_buf[7] = ush_irms_sns2 & 0xff;
                uch_asy3_tx_buf[8] = ush_irms_sns3 / 256;
                uch_asy3_tx_buf[9] = ush_irms_sns3 & 0xff;
//                uch_asy3_tx_buf[10] = ush_irms_ctr1 / 256;
//                uch_asy3_tx_buf[11] = ush_irms_ctr1 & 0xff;
//                uch_asy3_tx_buf[12] = ush_irms_ctr2 / 256;
//                uch_asy3_tx_buf[13] = ush_irms_ctr2 & 0xff;
//                uch_asy3_tx_buf[14] = ush_irms_ctr3 / 256;
//                uch_asy3_tx_buf[15] = ush_irms_ctr3 & 0xff;
                uch_asy3_tx_buf[10] = ush_irms_mod1 / 256;
                uch_asy3_tx_buf[11] = ush_irms_mod1 & 0xff;
                uch_asy3_tx_buf[12] = ush_irms_mod2 / 256;
                uch_asy3_tx_buf[13] = ush_irms_mod2 & 0xff;
                uch_asy3_tx_buf[14] = ush_irms_mod3 / 256;
                uch_asy3_tx_buf[15] = ush_irms_mod3 & 0xff;
                uch_asy3_tx_buf[16] =(unsigned)sh_t_sns1 / 256;
                uch_asy3_tx_buf[17] =(unsigned)sh_t_sns1 & 0xff;
                uch_asy3_tx_buf[18] =(unsigned)sh_t_sns2 / 256;
                uch_asy3_tx_buf[19] =(unsigned)sh_t_sns2 & 0xff;
                uch_asy3_tx_buf[20] =(unsigned)sh_t_sns3 / 256;
                uch_asy3_tx_buf[21] =(unsigned)sh_t_sns3 & 0xff;
                uch_asy3_tx_buf[22] =(unsigned)sh_t_mod1 / 256;
                uch_asy3_tx_buf[23] =(unsigned)sh_t_mod1 & 0xff;
                uch_asy3_tx_buf[24] =(unsigned)sh_t_mod2 / 256;
                uch_asy3_tx_buf[25] =(unsigned)sh_t_mod2 & 0xff;
                uch_asy3_tx_buf[26] =(unsigned)sh_t_mod3 / 256;
                uch_asy3_tx_buf[27] =(unsigned)sh_t_mod3 & 0xff;
/*                uch_asy3_tx_buf[22] =(unsigned)ts_state / 256;
                uch_asy3_tx_buf[23] =(unsigned)ts_state & 0xff;
                uch_asy3_tx_buf[24] =(unsigned)sh_prev_u_dcl / 256;
                uch_asy3_tx_buf[25] =(unsigned)sh_prev_u_dcl & 0xff;
                uch_asy3_tx_buf[26] =(unsigned)sh_u_dcl_compare / 256;
                uch_asy3_tx_buf[27] =(unsigned)sh_u_dcl_compare & 0xff;*/
                uch_asy3_tx_buf[28] =(unsigned)sh_u_dcl / 256;
                uch_asy3_tx_buf[29] =(unsigned)sh_u_dcl & 0xff;
                uch_asy3_tx_buf[30] = ush_urms_ul12 / 256;
                uch_asy3_tx_buf[31] = ush_urms_ul12 & 0xff;
                uch_asy3_tx_buf[32] = ush_urms_ul23 / 256;
                uch_asy3_tx_buf[33] = ush_urms_ul23 & 0xff;
                uch_asy3_tx_buf[34] = ush_urms_ul31 / 256;
                uch_asy3_tx_buf[35] = ush_urms_ul31 & 0xff;
                uch_asy3_tx_buf[36] = ush_ctr_status / 256;
                uch_asy3_tx_buf[37] = ush_ctr_status & 0xff;
                uch_asy3_tx_buf[38] = ush_fbk_status / 256;
                uch_asy3_tx_buf[39] = ush_fbk_status & 0xff;
                uch_asy3_tx_buf[40] = mes_mainerr_status.uch[1];
                uch_asy3_tx_buf[41] = mes_mainerr_status.uch[0];
                uch_asy3_tx_buf[42] = as_appl_status.uch[1];
                uch_asy3_tx_buf[43] = as_appl_status.uch[0];
                i = 40;
                break;

            case CMD_GETRUN:
                uch_asy3_tx_buf[4] = ush_swfreq / 256;
                uch_asy3_tx_buf[5] = ush_swfreq & 0xff;
                uch_asy3_tx_buf[6] = ush_fund / 256;
                uch_asy3_tx_buf[7] = ush_fund & 0xff;
                uch_asy3_tx_buf[8] = ush_irms_set / 256;
                uch_asy3_tx_buf[9] = ush_irms_set & 0xff;
                uch_asy3_tx_buf[10] = ush_u_dcl_set / 256;
                uch_asy3_tx_buf[11] = ush_u_dcl_set & 0xff;
                uch_asy3_tx_buf[12] = ush_pwm_perc_set / 256;
                uch_asy3_tx_buf[13] = ush_pwm_perc_set & 0xff;
                i = 10;
                break;
                
            case CMD_GETERR:
                break;
                
            case CMD_GETSYS:
                uch_asy3_tx_buf[4] = ush_swfreq / 256;
                uch_asy3_tx_buf[5] = ush_swfreq & 0xFF;
                uch_asy3_tx_buf[6] = ush_u_dcl_set / 256;
                uch_asy3_tx_buf[7] = ush_u_dcl_set & 0xFF;
                uch_asy3_tx_buf[8] = ush_oc_level / 256;
                uch_asy3_tx_buf[9] = ush_oc_level & 0xFF;
                uch_asy3_tx_buf[10] = ush_ov1_level / 256;
                uch_asy3_tx_buf[11] = ush_ov1_level & 0xFF;
                uch_asy3_tx_buf[12] = ush_ov2_level / 256;
                uch_asy3_tx_buf[13] = ush_ov2_level & 0xFF;
                uch_asy3_tx_buf[14] = ush_ot_level / 256;
                uch_asy3_tx_buf[15] = ush_ot_level & 0xFF;
                uch_asy3_tx_buf[16] = ush_max_lnvolt / 256;
                uch_asy3_tx_buf[17] = ush_max_lnvolt & 0xFF;
                uch_asy3_tx_buf[18] = ush_max_current / 256;
                uch_asy3_tx_buf[19] = ush_max_current & 0xFF;
                uch_asy3_tx_buf[20] = ush_max_dc / 256;
                uch_asy3_tx_buf[21] = ush_max_dc & 0xFF;
                uch_asy3_tx_buf[22] = ush_curr_diff / 256;
                uch_asy3_tx_buf[23] = ush_curr_diff & 0xFF;
                uch_asy3_tx_buf[24] = ush_cur_scale / 256;
                uch_asy3_tx_buf[25] = ush_cur_scale & 0xFF;
                uch_asy3_tx_buf[26] = ush_scur_scale / 256;
                uch_asy3_tx_buf[27] = ush_scur_scale & 0xFF;
                uch_asy3_tx_buf[28] = ush_volt_scale / 256;
                uch_asy3_tx_buf[29] = ush_volt_scale & 0xFF;
                uch_asy3_tx_buf[30] = (unsigned)sh_kP / 256;
                uch_asy3_tx_buf[31] = (unsigned)sh_kP & 0xFF;
                uch_asy3_tx_buf[32] = (unsigned)sh_kI / 256;
                uch_asy3_tx_buf[33] = (unsigned)sh_kI & 0xFF;
                
                i = 30;
                break;
                
            case CMD_GETDCOFF:
                for (i = 0;i < NUM1_CH; i ++)
                {
                    uch_asy3_tx_buf[4+i*2] = (unsigned)sh_adc1_cal[i] / 256;
                    uch_asy3_tx_buf[5+i*2] = (unsigned)sh_adc1_cal[i] & 0xff;
                }
                i *= 2;
                break;
                
            case CMD_GETTRLVL : 
                uch_asy3_tx_buf[4] = ush_u_in_oref[0] / 256;
                uch_asy3_tx_buf[5] = ush_u_in_oref[0] & 0xff;
                uch_asy3_tx_buf[6] = ush_s_in_oref[2] / 256;
                uch_asy3_tx_buf[7] = ush_s_in_oref[2] & 0xff;
                uch_asy3_tx_buf[8] = ush_s_in_oref[1] / 256;
                uch_asy3_tx_buf[9] = ush_s_in_oref[1] & 0xff;
                uch_asy3_tx_buf[10] = ush_s_in_oref[0] / 256;
                uch_asy3_tx_buf[11] = ush_s_in_oref[0] & 0xff;
                uch_asy3_tx_buf[12] = ush_u_in_oref[1] / 256;
                uch_asy3_tx_buf[13] = ush_u_in_oref[1] & 0xff;
                uch_asy3_tx_buf[14] = ush_cur_in_oref[2] / 256;
                uch_asy3_tx_buf[15] = ush_cur_in_oref[2] & 0xff;
                uch_asy3_tx_buf[16] = ush_cur_in_oref[1] / 256;
                uch_asy3_tx_buf[17] = ush_cur_in_oref[1] & 0xff;
                uch_asy3_tx_buf[18] = ush_cur_in_oref[0] / 256;
                uch_asy3_tx_buf[19] = ush_cur_in_oref[0] & 0xff;
                uch_asy3_tx_buf[20] = ush_u_ee_oref[0] / 256;
                uch_asy3_tx_buf[21] = ush_u_ee_oref[0] & 0xff;
                uch_asy3_tx_buf[22] = ush_s_ee_oref[2] / 256;
                uch_asy3_tx_buf[23] = ush_s_ee_oref[2] & 0xff;
                uch_asy3_tx_buf[24] = ush_s_ee_oref[1] / 256;
                uch_asy3_tx_buf[25] = ush_s_ee_oref[1] & 0xff;
                uch_asy3_tx_buf[26] = ush_s_ee_oref[0] / 256;
                uch_asy3_tx_buf[27] = ush_s_ee_oref[0] & 0xff;
                uch_asy3_tx_buf[28] = ush_u_ee_oref[1] / 256;
                uch_asy3_tx_buf[29] = ush_u_ee_oref[1] & 0xff;
                uch_asy3_tx_buf[30] = ush_cur_ee_oref[2] / 256;
                uch_asy3_tx_buf[31] = ush_cur_ee_oref[2] & 0xff;
                uch_asy3_tx_buf[32] = ush_cur_ee_oref[1] / 256;
                uch_asy3_tx_buf[33] = ush_cur_ee_oref[1] & 0xff;
                uch_asy3_tx_buf[34] = ush_cur_ee_oref[0] / 256;
                uch_asy3_tx_buf[35] = ush_cur_ee_oref[0] & 0xff;               
                uch_asy3_tx_buf[36] = ush_ov1_level_dac / 256;
                uch_asy3_tx_buf[37] = ush_ov1_level_dac & 0xff;
                uch_asy3_tx_buf[38] = ush_oc_s_level_dac / 256;
                uch_asy3_tx_buf[39] = ush_oc_s_level_dac & 0xff;
                uch_asy3_tx_buf[40] = ush_oc_s_level_dac / 256;
                uch_asy3_tx_buf[41] = ush_oc_s_level_dac & 0xff;
                uch_asy3_tx_buf[42] = ush_oc_s_level_dac / 256;
                uch_asy3_tx_buf[43] = ush_oc_s_level_dac & 0xff;
                uch_asy3_tx_buf[44] = ush_ov2_level_dac / 256;
                uch_asy3_tx_buf[45] = ush_ov2_level_dac & 0xff;
                uch_asy3_tx_buf[46] = ush_oc_cur_level_dac / 256;
                uch_asy3_tx_buf[47] = ush_oc_cur_level_dac & 0xff;
                uch_asy3_tx_buf[48] = ush_oc_cur_level_dac / 256;
                uch_asy3_tx_buf[49] = ush_oc_cur_level_dac & 0xff;
                uch_asy3_tx_buf[50] = ush_oc_cur_level_dac / 256;
                uch_asy3_tx_buf[51] = ush_oc_cur_level_dac & 0xff;
                i = 48;
                break;
                
            case CMD_GETTRACE:
                ush_read_trace_idx = uch_asy3_rx_buf[0]*256 + uch_asy3_rx_buf[1];
                uch_asy3_tx_buf[4] = uch_asy3_rx_buf[0];
                uch_asy3_tx_buf[5] = uch_asy3_rx_buf[1];
                ush_read_trace_idx = 0;
                uch_asy3_tx_buf[6] = (unsigned)sh_trace[ush_read_trace_idx][0] / 256;
                uch_asy3_tx_buf[7] = (unsigned)sh_trace[ush_read_trace_idx][0] & 0xff;
                uch_asy3_tx_buf[8] = (unsigned)sh_trace[ush_read_trace_idx][1] / 256;
                uch_asy3_tx_buf[9] = (unsigned)sh_trace[ush_read_trace_idx][1] & 0xff;
                uch_asy3_tx_buf[10] = (unsigned)sh_trace[ush_read_trace_idx][2] / 256;
                uch_asy3_tx_buf[11] = (unsigned)sh_trace[ush_read_trace_idx][2] & 0xff;
                uch_asy3_tx_buf[12] = (unsigned)sh_trace[ush_read_trace_idx][3] / 256;
                uch_asy3_tx_buf[13] = (unsigned)sh_trace[ush_read_trace_idx][3] & 0xff;
                uch_asy3_tx_buf[14] = (unsigned)sh_trace[ush_read_trace_idx][4] / 256;
                uch_asy3_tx_buf[15] = (unsigned)sh_trace[ush_read_trace_idx][4] & 0xff;
                uch_asy3_tx_buf[16] = (unsigned)sh_trace[ush_read_trace_idx][5] / 256;
                uch_asy3_tx_buf[17] = (unsigned)sh_trace[ush_read_trace_idx][5] & 0xff;
                uch_asy3_tx_buf[18] = (unsigned)sh_trace[ush_read_trace_idx][6] / 256;
                uch_asy3_tx_buf[19] = (unsigned)sh_trace[ush_read_trace_idx][6] & 0xff;
                uch_asy3_tx_buf[20] = (unsigned)sh_trace[ush_read_trace_idx][7] / 256;
                uch_asy3_tx_buf[21] = (unsigned)sh_trace[ush_read_trace_idx][7] & 0xff;
                uch_asy3_tx_buf[22] = (unsigned)sh_trace[ush_read_trace_idx][8] / 256;
                uch_asy3_tx_buf[23] = (unsigned)sh_trace[ush_read_trace_idx][8] & 0xff;
                uch_asy3_tx_buf[24] = (unsigned)sh_trace[ush_read_trace_idx][9] / 256;
                uch_asy3_tx_buf[25] = (unsigned)sh_trace[ush_read_trace_idx][9] & 0xff;
                i = 22;
                
                break;

            case CMD_SETSYS:
                if(as_appl_status.running)uch_asy3_tx_buf[2] |= 0x40;
                else {
                    ush_swfreq = uch_asy3_rx_buf[0]*256 + uch_asy3_rx_buf[1];
                    ush_u_dcl_set = uch_asy3_rx_buf[2]*256 + uch_asy3_rx_buf[3];
                    ush_oc_level = uch_asy3_rx_buf[4]*256 + uch_asy3_rx_buf[5];
                    ush_ov1_level = uch_asy3_rx_buf[6]*256 + uch_asy3_rx_buf[7];
                    ush_ov2_level = uch_asy3_rx_buf[8]*256 + uch_asy3_rx_buf[9];
                    ush_ot_level = uch_asy3_rx_buf[10]*256 + uch_asy3_rx_buf[11];
                    ush_max_lnvolt = uch_asy3_rx_buf[12]*256 + uch_asy3_rx_buf[13];
                    ush_max_current =  uch_asy3_rx_buf[14]*256 + uch_asy3_rx_buf[15];
                    ush_max_dc =  uch_asy3_rx_buf[16]*256 + uch_asy3_rx_buf[17];
                    ush_curr_diff =  uch_asy3_rx_buf[18]*256 + uch_asy3_rx_buf[19];
                    ush_cur_scale = uch_asy3_rx_buf[20]*256 + uch_asy3_rx_buf[21];
                    ush_scur_scale = uch_asy3_rx_buf[22]*256 + uch_asy3_rx_buf[23];
                    ush_volt_scale = uch_asy3_rx_buf[24]*256 + uch_asy3_rx_buf[25];
                    sh_kP = uch_asy3_rx_buf[26]*256 + uch_asy3_rx_buf[27]; 
                    sh_kI = uch_asy3_rx_buf[28]*256 + uch_asy3_rx_buf[29]; 
                    as_appl_status.check_params = TRUE;
                }
              
                uCommunicationState  = 1;   //set command
                break;

            case CMD_SETRUN:
                if(as_appl_status.running)uch_asy3_tx_buf[2] |= 0x40;
                else {
                    ush_swfreq = uch_asy3_rx_buf[0]*256 + uch_asy3_rx_buf[1];
                    ush_fund = uch_asy3_rx_buf[2]*256 + uch_asy3_rx_buf[3];
                    ush_irms_set  = uch_asy3_rx_buf[4]*256 + uch_asy3_rx_buf[5];
                    ush_u_dcl_set = uch_asy3_rx_buf[6]*256 + uch_asy3_rx_buf[7];
                    ush_pwm_perc_set = uch_asy3_rx_buf[8]*256 + uch_asy3_rx_buf[9];
                    //q16_irms_set = ush_irms_set*65536;
                }
                uCommunicationState  = 1;   //set command
                break;
                
            case CMD_SETDCV:
                if(as_appl_status.running)uch_asy3_tx_buf[2] |= 0x40;
                else {
                    ush_value = uch_asy3_rx_buf[0]*256 + uch_asy3_rx_buf[1];
                    if((ush_value >= UDCL_MIN) && (ush_value <= UDCL_MAX)) ush_u_dcl_set = ush_value;
                    else uch_asy3_tx_buf[2] |= 0xC0;
                }
                uCommunicationState  = 1;   //set command
                break;
                
            case CMD_SETCUR:
                ush_value = uch_asy3_rx_buf[0]*256 + uch_asy3_rx_buf[1];
                if(
#if CUR_MIN > 0
                        (ush_value >= CUR_MIN) &&
#endif
                        (ush_value <= CUR_MAX))
                {
                    if(as_appl_status.running)ush_irms_set_new = ush_value;
                    else ush_irms_set = ush_irms_set_new = ush_value;
                    as_appl_status.change_current = TRUE;
                } else uch_asy3_tx_buf[2] |= 0xC0;
                uCommunicationState  = 1;   //set command
                break;
                
            case CMD_SETPWM:
                ush_value = uch_asy3_rx_buf[0]*256 + uch_asy3_rx_buf[1];
                if(
#if PWM_MIN > 0
                        (ush_value >= PWM_MIN) &&
#endif
                        (ush_value <= PWM_MAX)){
                    ush_pwm_perc_set = ush_value;
                    as_appl_status.change_current = TRUE;
                } else uch_asy3_tx_buf[2] |= 0xC0;
                uCommunicationState  = 1;   //set command
                break;
                
            case CMD_SETSWFR:
                ush_value = uch_asy3_rx_buf[0]*256 + uch_asy3_rx_buf[1];
                if(as_appl_status.running)uch_asy3_tx_buf[2] |= 0x40;
                else {
                    if((ush_value >= SWFREQ_MIN) && (ush_value <= SWFREQ_MAX)){
                        ush_swfreq = ush_value;
                        pwm_init();
                    } else uch_asy3_tx_buf[2] |= 0xC0;
                }
                uCommunicationState  = 1;   //set command
                break;
                
            case CMD_SETFUND:
                ush_value = uch_asy3_rx_buf[0]*256 + uch_asy3_rx_buf[1];
                if((ush_value >= FUND_MIN) && (ush_value <= FUND_MAX)){
                    ush_fund_new = ush_value;
                    as_appl_status.change_freq = TRUE;
                } else uch_asy3_tx_buf[2] |= 0xC0;
                uCommunicationState  = 1;   //set command
                break;
                
            case CMD_SETOUT:
                ush_ctr =  uch_asy3_rx_buf[0]*256 + uch_asy3_rx_buf[1];
                uCommunicationState  = 1;   //set command
                break;
                
            case CMD_MKDCOFF:
                adc1_calc_offset();
                adc2_calc_offset();
                #if BOARDCFG_USE_TESTPROC
                bStartTestCalibrationOnlyNextStartTest = 1;
                bStartTest = 1;
                #endif
                break;
                
            case CMD_START:
                if(!faultDetected || as_appl_status.running)
                {
                    uch_asy3_tx_buf[2] |= 0x40;
                }
                else 
                {
                    #if BOARDCFG_USE_TESTPROC
                    bStartTest = 1;
                    bStartTestCalibrationIfNeeded = 1;
                    #endif
                }
                  
                break;
                
            case CMD_STOP:
                #if BOARDCFG_USE_TESTPROC
                bStopTest = 1;
                u8LastStopTestID = 4;
                #endif
                break;
                
            case CMD_CLRERR:
                #if BOARDCFG_USE_TESTPROC
                bStopTest = 1;
                u8LastStopTestID = 5;
                #endif
                //as_appl_status.clear_errors = TRUE;
                break;
                
            case CMD_SETTRACE:
                ush_trace_idx = 0;
                as_appl_status.trace_running = TRUE;
                break;
                
            default:
                uch_asy3_tx_buf[2] |= 0x80;

        }
        uch_asy3_tx_buf[3] = i;
        uch_asy3_tosend = 4 + i;
        as_appl_status.asy3_cmd_recvd = FALSE;
        CTREADER_nRxTodoEcho = uch_asy3_tosend + 1;

        CTREADER_nTxDelay = TX_DLAY;

    }
    
    /* write signal mapping //uch_asy3_rx_cmd */
    if(uCommunicationState)
    {
        
        
        
        /* CMD_SETDCV */
        //CTREADER_sh_u_dcl_set = ush_u_dcl_set; - check limits above here UDCL_MIN UDCL_MAX
        
        /* CMD_SETCUR */
        //unknown = ush_irms_set_new; - check limits above here CUR_MIN CUR_MAX
        if (as_appl_status.change_current == TRUE)
        {
            as_appl_status.change_current = FALSE;
            ush_irms_set = ush_irms_set_new;
        }
        
        /* CMD_SETPWM */
        //unknown = ush_pwm_perc_set; - check limits above here PWM_MIN PWM_MAX

        /* CMD_SETSWFR */
        //unknown = ush_swfreq; - check limits above here SWFREQ_MIN SWFREQ_MAX

        /* CMD_SETFUND */
        //unknown = ush_fund_new; - check limits above here FUND_MIN FUND_MAX
        if (as_appl_status.change_freq == TRUE)
        {
            as_appl_status.change_freq = FALSE;
            ush_fund = ush_fund_new;
        }

        /* CMD_SETOUT */
        #if BOARDCFG_USE_FPGA_DIO
        #if BOARDCFG_USE_TESTPROC
        if(TESTPROC_eGetState() == TEST_STA_IDLE)
        {
            u16DOExternal = ush_ctr;
        }
        #endif
        #endif

        
        /* CMD_SETTRACE */
        //unknown = as_appl_status.trace_running;
        
        
        
        
        /* CMD_SETSYS */
        #if !defined(BOARDCFG_USE_MCC_ADC) && (BOARDCFG_USE_OLD_NON_MCC_ADC)
        ADCPWM_vSetPWM1FreqHzReq(ush_swfreq);
        #endif
        CONTROL_s16SetTripTemperature_dDeg(ush_ot_level);
        CTREADER_sh_u_dcl_set = ush_u_dcl_set;
        #if BOARDCFG_USE_DAC_TRIP
        TripCurrentStack1[0] = ush_oc_level;
        TripCurrentStack1[1] = ush_oc_level;
        TripCurrentStack1[2] = ush_oc_level;
        //TripVoltageStack1 = ush_ov1_level;
        //TripVoltageStack2 = ush_ov2_level;
        TripVoltageStack2 = ush_max_lnvolt;
        TripCFBKStack1Fbk = ush_max_current;
        TripVoltageStack1 = ush_max_dc;
        #endif
        //unknown = ush_curr_diff;
        //unknown = ush_cur_scale;
        //unknown = ush_scur_scale;
        //unknown = ush_volt_scale;
        //unknown = sh_kI;
        
        
        /* CMD_SETRUN */
        //unknown = ush_swfreq;
        sControlData.sStack[STACK_1].s16Speed = MCLIB_s16Mul_s16u32(ush_fund,sControlConfig.u32DeciHzToSpeedMultiplier);
        sControlData.sStack[STACK_1].s16CurrentRMSReq1Ph = MCLIB_s16Mul_s16u32(ush_irms_set,sControlConfig.u32DeciAmperesToADCResultMultiplier);
        //unknown = ush_u_dcl_set;
        sControlData.sStack[STACK_1].s16VoltageAmpScaled = MCLIB_s16Mul_s16u32(ush_pwm_perc_set,MCLIB_u32FractionalQ_16_16(1.0 / ((10.0 * 100.0) / 32768)));
        //unknown = ush_pwm_perc_set;
        //unknown = q16_irms_set;???

        
    }
    /* write signal mapping END */
    
}

#endif /* USE_CTREADER */

