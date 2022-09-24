/* *****************************************************************************
 * File:   adcpwm.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 02 11 11:18
 * 
 * Description: ADC and PWM Driver
 * 
 **************************************************************************** */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdbool.h>
#include "boardcfg.h"
#include "adcpwm.h"
#include "adc_drv.h"
#include "hmlib.h"


/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */

uint16_t u16PWM1FreqHz = LOOPTIME1_HZ;
uint16_t u16PWM2FreqHz = LOOPTIME2_HZ;

uint16_t u16LoopTime1Tcy = (uint16_t)LOOPTIME1_TCY;
uint16_t u16LoopTime2Tcy = (uint16_t)LOOPTIME2_TCY;

int32_t s32RealSamplingFreqCoef;

uint16_t ADCStack1Buffered[ADC_STACK_COUNT];
uint16_t ADCStack2Buffered[ADC_STACK_COUNT];
uint16_t ADCStack3Buffered[ADC_STACK_COUNT];
uint16_t ADCDCLinkBuffered[ADC_STACK_COUNT];
uint16_t ADCLine_1Buffered[ADC_STACK_COUNT];
uint16_t ADCLine_2Buffered[ADC_STACK_COUNT];

uint16_t ADC2Stack1Buffered[ADC2_STACK_COUNT];
uint16_t ADC2Stack2Buffered[ADC2_STACK_COUNT];

uint16_t u16PWM1FreqHzReq = LOOPTIME1_HZ;
uint16_t u16PWM2FreqHzReq = LOOPTIME2_HZ;


uint32_t u32Fp = FCY_HZ;

bool bStack1FreqReset = 0;
bool bStack2FreqReset = 0;

uint16_t PWM_u16MinDutyCoef =   MCLIB_u16Fractional(PWM_MIN_DUTY_COEF   );
uint16_t PWM_u16MaxDutyCoef =   MCLIB_u16Fractional(PWM_MAX_DUTY_COEF   );
uint16_t PWM_u16StartDutyCoef = MCLIB_u16Fractional(PWM_START_DUTY_COEF );

uint16_t PWM_u16MinDutyTcy =    MCLIB_u16Mul_u16u16(MCLIB_u16Fractional(PWM_MIN_DUTY_COEF   ),(uint16_t)LOOPTIME1_TCY);
uint16_t PWM_u16MaxDutyTcy =    MCLIB_u16Mul_u16u16(MCLIB_u16Fractional(PWM_MAX_DUTY_COEF   ),(uint16_t)LOOPTIME1_TCY);
uint16_t PWM_u16StartDutyTcy =  MCLIB_u16Mul_u16u16(MCLIB_u16Fractional(PWM_START_DUTY_COEF ),(uint16_t)LOOPTIME1_TCY);

uint16_t u16ExecControlNewDataSkipBuffered;

/* *****************************************************************************
 * Function Definitions
 **************************************************************************** */

void PWM_vOutputEnable(void)
{
    /* !!! Update Compare Vectors Here */
#if 0
    PDC1 = PDC2 = PDC3 = PDC4 = PDC5 = PDC6 = PWM_u16StartDutyTcy;
#else
    EPWM_setCounterCompareValue(EPWM1_BASE,     EPWM_COUNTER_COMPARE_A, PWM_u16StartDutyTcy);
    EPWM_setCounterCompareValue(EPWM2_BASE,     EPWM_COUNTER_COMPARE_A, PWM_u16StartDutyTcy);
    EPWM_setCounterCompareValue(EPWM3_BASE,     EPWM_COUNTER_COMPARE_A, PWM_u16StartDutyTcy);
    EPWM_setCounterCompareValue(EPWM4_BASE,     EPWM_COUNTER_COMPARE_A, PWM_u16StartDutyTcy);
    EPWM_setCounterCompareValue(EPWM5_BASE,     EPWM_COUNTER_COMPARE_A, PWM_u16StartDutyTcy);
    EPWM_setCounterCompareValue(EPWM6_BASE,     EPWM_COUNTER_COMPARE_A, PWM_u16StartDutyTcy);
    EPWM_setCounterCompareValue(EPWM7_BASE,     EPWM_COUNTER_COMPARE_A, PWM_u16StartDutyTcy);
    EPWM_setCounterCompareValue(EPWM8_BASE,     EPWM_COUNTER_COMPARE_A, PWM_u16StartDutyTcy);
    EPWM_setCounterCompareValue(EPWM9_BASE,     EPWM_COUNTER_COMPARE_A, PWM_u16StartDutyTcy);
    EPWM_setCounterCompareValue(EPWM10_BASE,    EPWM_COUNTER_COMPARE_A, PWM_u16StartDutyTcy);
    EPWM_setCounterCompareValue(EPWM11_BASE,    EPWM_COUNTER_COMPARE_A, PWM_u16StartDutyTcy);
    EPWM_setCounterCompareValue(EPWM12_BASE,    EPWM_COUNTER_COMPARE_A, PWM_u16StartDutyTcy);
#endif
}

void PWM_vOutputDisable(void)
{
    /* !!! Update Compare Vectors Here */
#if 0
    /* zero duty cycle */
    PDC1 = 0;
    PDC2 = 0;
    PDC3 = 0;

    /* zero duty cycle */
    PDC4 = 0;
    PDC5 = 0;
    PDC6 = 0;
#else
    EPWM_setCounterCompareValue(EPWM1_BASE,     EPWM_COUNTER_COMPARE_A, 0);
    EPWM_setCounterCompareValue(EPWM2_BASE,     EPWM_COUNTER_COMPARE_A, 0);
    EPWM_setCounterCompareValue(EPWM3_BASE,     EPWM_COUNTER_COMPARE_A, 0);
    EPWM_setCounterCompareValue(EPWM4_BASE,     EPWM_COUNTER_COMPARE_A, 0);
    EPWM_setCounterCompareValue(EPWM5_BASE,     EPWM_COUNTER_COMPARE_A, 0);
    EPWM_setCounterCompareValue(EPWM6_BASE,     EPWM_COUNTER_COMPARE_A, 0);
    EPWM_setCounterCompareValue(EPWM7_BASE,     EPWM_COUNTER_COMPARE_A, 0);
    EPWM_setCounterCompareValue(EPWM8_BASE,     EPWM_COUNTER_COMPARE_A, 0);
    EPWM_setCounterCompareValue(EPWM9_BASE,     EPWM_COUNTER_COMPARE_A, 0);
    EPWM_setCounterCompareValue(EPWM10_BASE,    EPWM_COUNTER_COMPARE_A, 0);
    EPWM_setCounterCompareValue(EPWM11_BASE,    EPWM_COUNTER_COMPARE_A, 0);
    EPWM_setCounterCompareValue(EPWM12_BASE,    EPWM_COUNTER_COMPARE_A, 0);
#endif
}

void ADCPWM_vSetDutyCycleStack1 (MCLIB_s3Param_u16_t* psDutyCycles)
{
    /* !!! Update Compare Vectors Here */
#if 0
    PDC1 = psDutyCycles->u16Arg1;
    PDC2 = psDutyCycles->u16Arg2;
    PDC3 = psDutyCycles->u16Arg3;
#else
    EPWM_setCounterCompareValue(EPWM1_BASE,     EPWM_COUNTER_COMPARE_A, psDutyCycles->u16Arg1);
    EPWM_setCounterCompareValue(EPWM2_BASE,     EPWM_COUNTER_COMPARE_A, psDutyCycles->u16Arg2);
    EPWM_setCounterCompareValue(EPWM3_BASE,     EPWM_COUNTER_COMPARE_A, psDutyCycles->u16Arg3);
#endif
}

void ADCPWM_vSetDutyCycleStack2 (MCLIB_s3Param_u16_t* psDutyCycles)
{
    /* !!! Update Compare Vectors Here */
#if 0
    PDC4 = psDutyCycles->u16Arg1;
    PDC5 = psDutyCycles->u16Arg2;
    PDC6 = psDutyCycles->u16Arg3;
#else
    EPWM_setCounterCompareValue(EPWM4_BASE,     EPWM_COUNTER_COMPARE_A, psDutyCycles->u16Arg1);
    EPWM_setCounterCompareValue(EPWM5_BASE,     EPWM_COUNTER_COMPARE_A, psDutyCycles->u16Arg2);
    EPWM_setCounterCompareValue(EPWM6_BASE,     EPWM_COUNTER_COMPARE_A, psDutyCycles->u16Arg3);
#endif
}


void ADCPWM_vSetDutyCycleStack3 (MCLIB_s3Param_u16_t* psDutyCycles)
{
    /* !!! Update Compare Vectors Here */
#if 0
    PDC4 = psDutyCycles->u16Arg1;
    PDC5 = psDutyCycles->u16Arg2;
    PDC6 = psDutyCycles->u16Arg3;
#else
    EPWM_setCounterCompareValue(EPWM7_BASE,     EPWM_COUNTER_COMPARE_A, psDutyCycles->u16Arg1);
    EPWM_setCounterCompareValue(EPWM8_BASE,     EPWM_COUNTER_COMPARE_A, psDutyCycles->u16Arg2);
    EPWM_setCounterCompareValue(EPWM9_BASE,     EPWM_COUNTER_COMPARE_A, psDutyCycles->u16Arg3);
#endif
}

void ADCPWM_vSetDutyCycleStack4 (MCLIB_s3Param_u16_t* psDutyCycles)
{
    /* !!! Update Compare Vectors Here */
#if 0
    PDC4 = psDutyCycles->u16Arg1;
    PDC5 = psDutyCycles->u16Arg2;
    PDC6 = psDutyCycles->u16Arg3;
#else
    EPWM_setCounterCompareValue(EPWM10_BASE,     EPWM_COUNTER_COMPARE_A, psDutyCycles->u16Arg1);
    EPWM_setCounterCompareValue(EPWM11_BASE,     EPWM_COUNTER_COMPARE_A, psDutyCycles->u16Arg2);
    EPWM_setCounterCompareValue(EPWM12_BASE,     EPWM_COUNTER_COMPARE_A, psDutyCycles->u16Arg3);
#endif
}

void ADCPWM_vReadNewADCData(void)
{
#if BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC

    ADCStack1Buffered[ADC_STACK_CURRENT_L1] = u16Adc_Iph_FBK_DSP1 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack1Buffered[ADC_STACK_CURRENT_L2] = u16Adc_Iph_FBK_DSP2 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack1Buffered[ADC_STACK_CURRENT_L3] = u16Adc_Iph_FBK_DSP3 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack1Buffered[ADC_STACK_DC_VOLTAGE] = u16Adc_PV_Voltage_DSP << ADC_PWM_RESULT_TO_16BITS_SHIFT;

    ADCStack2Buffered[ADC_STACK_CURRENT_L1] = u16Adc_Iph_FBK_DSP4 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack2Buffered[ADC_STACK_CURRENT_L2] = u16Adc_Iph_FBK_DSP5 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack2Buffered[ADC_STACK_CURRENT_L3] = u16Adc_Iph_FBK_DSP6 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack2Buffered[ADC_STACK_DC_VOLTAGE] = u16Adc_Inlet_Liquid_Pressure_DSP << ADC_PWM_RESULT_TO_16BITS_SHIFT;

    ADCStack3Buffered[ADC_STACK_CURRENT_L1] = u16Adc_Iph_FBK_DSP7 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack3Buffered[ADC_STACK_CURRENT_L2] = u16Adc_Iph_FBK_DSP8 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack3Buffered[ADC_STACK_CURRENT_L3] = u16Adc_Iph_FBK_DSP9 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack3Buffered[ADC_STACK_DC_VOLTAGE] = u16Adc_Outlet_Liquid_Pressure_DSP << ADC_PWM_RESULT_TO_16BITS_SHIFT;


    ADCDCLinkBuffered[ADC_STACK_CURRENT_L1] = u16Adc_PV_Current_DSP << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCDCLinkBuffered[ADC_STACK_CURRENT_L2] = u16Adc_AN_IN_Aux1_DSP << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCDCLinkBuffered[ADC_STACK_CURRENT_L3] = u16Adc_AN_IN_Aux2_DSP << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCDCLinkBuffered[ADC_STACK_DC_VOLTAGE] = u16Adc_DC_Link_DSP << ADC_PWM_RESULT_TO_16BITS_SHIFT;

    ADCLine_1Buffered[ADC_STACK_CURRENT_L1] = u16Adc_AC_Voltage_L1_L2_DSP << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCLine_1Buffered[ADC_STACK_CURRENT_L2] = u16Adc_AC_Voltage_L2_L3_DSP << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCLine_1Buffered[ADC_STACK_CURRENT_L3] = u16Adc_Inlet_Liquid_Temperature_DSP << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCLine_1Buffered[ADC_STACK_DC_VOLTAGE] = u16Adc_DAC_outA << ADC_PWM_RESULT_TO_16BITS_SHIFT;

    ADCLine_2Buffered[ADC_STACK_CURRENT_L1] = u16Adc_Outlet_Liquid_Temperature_DSP << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCLine_2Buffered[ADC_STACK_CURRENT_L2] = u16Adc_Air_Temperature_Section_1_DSP << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCLine_2Buffered[ADC_STACK_CURRENT_L3] = u16Adc_Air_Temperature_Section_2_DSP << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCLine_2Buffered[ADC_STACK_DC_VOLTAGE] = u16Adc_DAC_outB << ADC_PWM_RESULT_TO_16BITS_SHIFT;

    ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L1] = 0;
    ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L2] = 0;
    ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L3] = 0;
    ADC2Stack1Buffered[ADC2_STACK_DCVOLTAGE2] = 0;
    ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L1] = 0;
    ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L2] = 0;
    ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L3] = 0;
    ADC2Stack2Buffered[ADC2_STACK_DCVOLTAGE2] = 0;

#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG

    #if BOARDCFG_IFBK_147_STACK_STYLE == 0
    ADCStack1Buffered[ADC_STACK_CURRENT_L1] = u16Adc_Iph_FBK_MCU1 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack1Buffered[ADC_STACK_CURRENT_L2] = u16Adc_Iph_FBK_MCU2 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack1Buffered[ADC_STACK_CURRENT_L3] = u16Adc_Iph_FBK_MCU3 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack1Buffered[ADC_STACK_DC_VOLTAGE] = u16Adc_DCLV_FBK_MCU1 << ADC_PWM_RESULT_TO_16BITS_SHIFT;

    ADCStack2Buffered[ADC_STACK_CURRENT_L1] = u16Adc_Iph_FBK_MCU4 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack2Buffered[ADC_STACK_CURRENT_L2] = u16Adc_Iph_FBK_MCU5 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack2Buffered[ADC_STACK_CURRENT_L3] = u16Adc_Iph_FBK_MCU6 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack2Buffered[ADC_STACK_DC_VOLTAGE] = u16Adc_DCLV_FBK_MCU2 << ADC_PWM_RESULT_TO_16BITS_SHIFT;

    ADCStack3Buffered[ADC_STACK_CURRENT_L1] = u16Adc_Iph_FBK_MCU7 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack3Buffered[ADC_STACK_CURRENT_L2] = u16Adc_Iph_FBK_MCU8 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack3Buffered[ADC_STACK_CURRENT_L3] = u16Adc_Iph_FBK_MCU9 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack3Buffered[ADC_STACK_DC_VOLTAGE] = u16Adc_DCLV_FBK_MCU3 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    #else
    ADCStack1Buffered[ADC_STACK_CURRENT_L1] = u16Adc_Iph_FBK_MCU1 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack1Buffered[ADC_STACK_CURRENT_L2] = u16Adc_Iph_FBK_MCU4 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack1Buffered[ADC_STACK_CURRENT_L3] = u16Adc_Iph_FBK_MCU7 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack1Buffered[ADC_STACK_DC_VOLTAGE] = u16Adc_DCLV_FBK_MCU1 << ADC_PWM_RESULT_TO_16BITS_SHIFT;

    ADCStack2Buffered[ADC_STACK_CURRENT_L1] = u16Adc_Iph_FBK_MCU2 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack2Buffered[ADC_STACK_CURRENT_L2] = u16Adc_Iph_FBK_MCU5 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack2Buffered[ADC_STACK_CURRENT_L3] = u16Adc_Iph_FBK_MCU8 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack2Buffered[ADC_STACK_DC_VOLTAGE] = u16Adc_DCLV_FBK_MCU2 << ADC_PWM_RESULT_TO_16BITS_SHIFT;

    ADCStack3Buffered[ADC_STACK_CURRENT_L1] = u16Adc_Iph_FBK_MCU3 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack3Buffered[ADC_STACK_CURRENT_L2] = u16Adc_Iph_FBK_MCU6 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack3Buffered[ADC_STACK_CURRENT_L3] = u16Adc_Iph_FBK_MCU9 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack3Buffered[ADC_STACK_DC_VOLTAGE] = u16Adc_DCLV_FBK_MCU3 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    #endif

    ADCDCLinkBuffered[ADC_STACK_CURRENT_L1] = u16Adc_DCLI_FBK_MCU1 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCDCLinkBuffered[ADC_STACK_CURRENT_L2] = u16Adc_DCLI_FBK_MCU2 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCDCLinkBuffered[ADC_STACK_CURRENT_L3] = u16Adc_DCLI_FBK_MCU3 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCDCLinkBuffered[ADC_STACK_DC_VOLTAGE] = u16Adc_DCLV_FBK_MCU4 << ADC_PWM_RESULT_TO_16BITS_SHIFT;

    ADCLine_1Buffered[ADC_STACK_CURRENT_L1] = u16Adc_LLV_FBK_MCU1 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCLine_1Buffered[ADC_STACK_CURRENT_L2] = u16Adc_LLV_FBK_MCU2 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCLine_1Buffered[ADC_STACK_CURRENT_L3] = u16Adc_LLV_FBK_MCU3 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCLine_1Buffered[ADC_STACK_DC_VOLTAGE] = u16Adc_DACOUTA << ADC_PWM_RESULT_TO_16BITS_SHIFT;

    ADCLine_2Buffered[ADC_STACK_CURRENT_L1] = u16Adc_LLV_FBK_MCU4 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCLine_2Buffered[ADC_STACK_CURRENT_L2] = u16Adc_LLV_FBK_MCU5 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCLine_2Buffered[ADC_STACK_CURRENT_L3] = u16Adc_LLV_FBK_MCU6 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCLine_2Buffered[ADC_STACK_DC_VOLTAGE] = u16Adc_DACOUTB << ADC_PWM_RESULT_TO_16BITS_SHIFT;

    ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L1] = 0;
    ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L2] = 0;
    ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L3] = 0;
    ADC2Stack1Buffered[ADC2_STACK_DCVOLTAGE2] = 0;
    ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L1] = 0;
    ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L2] = 0;
    ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L3] = 0;
    ADC2Stack2Buffered[ADC2_STACK_DCVOLTAGE2] = 0;

#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_SMALL_TEST

    ADCStack1Buffered[ADC_STACK_CURRENT_L1] = u16Value_I_U_OUT_ADC << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack1Buffered[ADC_STACK_CURRENT_L2] = u16Value_I_V_OUT_ADC << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack1Buffered[ADC_STACK_CURRENT_L3] = u16Value_I_W_OUT_ADC << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack1Buffered[ADC_STACK_DC_VOLTAGE] = u16Value_U_DC_FB_ADC << ADC_PWM_RESULT_TO_16BITS_SHIFT;

    ADCStack2Buffered[ADC_STACK_CURRENT_L1] = u16Value_I_U_ADC << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack2Buffered[ADC_STACK_CURRENT_L3] = u16Value_I_W_ADC << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack2Buffered[ADC_STACK_CURRENT_L2] = 0 - ADCStack2Buffered[ADC_STACK_CURRENT_L1] - ADCStack2Buffered[ADC_STACK_CURRENT_L3];
    ADCStack2Buffered[ADC_STACK_DC_VOLTAGE] = 0 << ADC_PWM_RESULT_TO_16BITS_SHIFT;

    ADCStack3Buffered[ADC_STACK_CURRENT_L1] = u16Value_I_U_TRIP << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack3Buffered[ADC_STACK_CURRENT_L3] = u16Value_I_W_TRIP << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCStack3Buffered[ADC_STACK_CURRENT_L2] = 0 - ADCStack3Buffered[ADC_STACK_CURRENT_L1] - ADCStack3Buffered[ADC_STACK_CURRENT_L3];
    ADCStack3Buffered[ADC_STACK_DC_VOLTAGE] = 0 << ADC_PWM_RESULT_TO_16BITS_SHIFT;

    ADCDCLinkBuffered[ADC_STACK_CURRENT_L1] = 0 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCDCLinkBuffered[ADC_STACK_CURRENT_L2] = 0 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCDCLinkBuffered[ADC_STACK_CURRENT_L3] = 0 << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCDCLinkBuffered[ADC_STACK_DC_VOLTAGE] = 0 << ADC_PWM_RESULT_TO_16BITS_SHIFT;

    ADCLine_1Buffered[ADC_STACK_CURRENT_L1] = u16Value_VAC_UV_ADC << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCLine_1Buffered[ADC_STACK_CURRENT_L2] = u16Value_VAC_VW_ADC << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCLine_1Buffered[ADC_STACK_CURRENT_L3] = 0 - ADCLine_1Buffered[ADC_STACK_CURRENT_L1] - ADCLine_1Buffered[ADC_STACK_CURRENT_L2];
    ADCLine_1Buffered[ADC_STACK_DC_VOLTAGE] = 0 << ADC_PWM_RESULT_TO_16BITS_SHIFT;

    ADCLine_2Buffered[ADC_STACK_CURRENT_L1] = u16Value_I_U_OUT_TRIP << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCLine_2Buffered[ADC_STACK_CURRENT_L3] = u16Value_I_W_OUT_TRIP << ADC_PWM_RESULT_TO_16BITS_SHIFT;
    ADCLine_2Buffered[ADC_STACK_CURRENT_L2] = 0 - ADCLine_2Buffered[ADC_STACK_CURRENT_L1] - ADCLine_2Buffered[ADC_STACK_CURRENT_L3];
    ADCLine_2Buffered[ADC_STACK_DC_VOLTAGE] = 0 << ADC_PWM_RESULT_TO_16BITS_SHIFT;

    ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L1] = 0;
    ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L2] = 0;
    ADC2Stack1Buffered[ADC2_STACK_CURRFBK_L3] = 0;
    ADC2Stack1Buffered[ADC2_STACK_DCVOLTAGE2] = 0;
    ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L1] = 0;
    ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L2] = 0;
    ADC2Stack1Buffered[ADC2_STACK_TEMPFBK_L3] = 0;
    ADC2Stack2Buffered[ADC2_STACK_DCVOLTAGE2] = 0;
#endif

}


/* *****************************************************************************
 * ADCPWM_vStackFreqSetup
 *
 * Description:
 **************************************************************************** */
uint16_t u16PWMForRecorderPeriod_usec = (1000000 / LOOPTIME1_HZ);

bool ADCPWM_vStackFreqSetupNeeded( void )
{
    bool bResult = false;

    if (u16PWM1FreqHzReq != u16PWM1FreqHz) {
        bResult = true;
    }
    if (u16PWM2FreqHzReq != u16PWM2FreqHz) {
        bResult = true;
    }
    return bResult;
}


uint16_t ADCPWM_u16GetPWM1FreqHzReq (void)
{
    return u16PWM1FreqHzReq;
}


uint16_t ADCPWM_u16GetPWM2FreqHzReq (void)
{
    return u16PWM2FreqHzReq;
}


uint16_t ADCPWM_u16GetPWM1FreqHz (void)
{
    return u16PWM1FreqHz;
}


uint16_t ADCPWM_u16GetPWM2FreqHz (void)
{
    return u16PWM2FreqHz;
}


uint16_t ADCPWM_u16GetLoopTime1Tcy (void)
{
    return u16LoopTime1Tcy;
}


uint16_t ADCPWM_u16GetLoopTime2Tcy (void)
{
    return u16LoopTime2Tcy;
}


void ADCPWM_vSetPWM1FreqHzReq (uint16_t u16Input)
{
    u16PWM1FreqHzReq = u16Input;
}


void ADCPWM_vSetPWM2FreqHzReq (uint16_t u16Input)
{
    u16PWM2FreqHzReq = u16Input;
}


bool ADCPWM_vStackFreqSetup( void )
{
    uint32_t u32FpDiv4;
    bool bResult = false;

    if (u16PWM1FreqHzReq != u16PWM1FreqHz)
    {
        u32FpDiv4 = u32Fp >> 2;
        u16PWM1FreqHz = u16PWM1FreqHzReq;
        u16LoopTime1Tcy = MCLIB_u16DivSat_u32u16(u32FpDiv4, u16PWM1FreqHz);
        u16PWMForRecorderPeriod_usec = MCLIB_u16DivSat_u32u16(1000000, u16PWM1FreqHz);
        HMLIB_vRecorderSetTimeBase(u16PWMForRecorderPeriod_usec);
        bResult = true;
    }

    if (u16PWM2FreqHzReq != u16PWM2FreqHz)
    {
        u32FpDiv4 = u32Fp >> 2;
        u16PWM2FreqHz = u16PWM2FreqHzReq;
        u16LoopTime2Tcy = MCLIB_u16DivSat_u32u16(u32FpDiv4, u16PWM2FreqHz);
        bResult = true;
    }
    return bResult;
}
