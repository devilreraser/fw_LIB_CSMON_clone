/* *****************************************************************************
 * File:   adc_drv.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 11 24 22:42
 * 
 * Description: ...
 * 
 **************************************************************************** */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Guard condition file contents not included more than once */  
/* #pragma once */
#ifndef ADC_DRV_H
#define	ADC_DRV_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdint.h>

#include "boardcfg.h"
#include "mclib.h"

#include "driverlib.h"
#include "device.h"


/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define ADC_RESOLUTION       12

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 
#if BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC
	/* ADC_A */
	extern uint16_t u16Adc_DAC_outA      ;
	extern uint16_t u16Adc_DAC_outB      ;
	extern uint16_t u16Adc_Iph_FBK_DSP1  ;
	extern uint16_t u16Adc_Iph_FBK_DSP2  ;
	extern uint16_t u16Adc_Iph_FBK_DSP3  ;
	extern uint16_t u16Adc_PV_Current_DSP;
	/* ADC_B */
	extern uint16_t u16Adc_AC_Voltage_L1_L2_DSP        ;
	extern uint16_t u16Adc_Inlet_Liquid_Temperature_DSP;
	extern uint16_t u16Adc_Iph_FBK_DSP4                ;
	extern uint16_t u16Adc_Iph_FBK_DSP5                ;
	extern uint16_t u16Adc_Iph_FBK_DSP6                ;
	extern uint16_t u16Adc_AN_IN_Aux1_DSP              ;
	/* ADC_C */
	extern uint16_t u16Adc_Air_Temperature_Section_1_DSP;
	extern uint16_t u16Adc_Air_Temperature_Section_2_DSP;
	extern uint16_t u16Adc_Iph_FBK_DSP7                 ;
	extern uint16_t u16Adc_Iph_FBK_DSP8                 ;
	extern uint16_t u16Adc_Iph_FBK_DSP9                 ;
	extern uint16_t u16Adc_AN_IN_Aux2_DSP              ;
	/* ADC_D */
	extern uint16_t u16Adc_PV_Voltage_DSP               ;
	extern uint16_t u16Adc_Inlet_Liquid_Pressure_DSP    ;
	extern uint16_t u16Adc_Outlet_Liquid_Pressure_DSP   ;
	extern uint16_t u16Adc_DC_Link_DSP                  ;
	extern uint16_t u16Adc_AC_Voltage_L2_L3_DSP         ;
	extern uint16_t u16Adc_Outlet_Liquid_Temperature_DSP;

#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG

	extern uint16_t u16Adc_DACOUTA       ;
	extern uint16_t u16Adc_DACOUTB       ;
	extern uint16_t u16Adc_Iph_FBK_MCU1  ;
	extern uint16_t u16Adc_Iph_FBK_MCU4  ;
	extern uint16_t u16Adc_Iph_FBK_MCU7  ;
	extern uint16_t u16Adc_DCLI_FBK_MCU1 ;

	extern uint16_t u16Adc_LLV_FBK_MCU1  ;
	extern uint16_t u16Adc_LLV_FBK_MCU3  ;
	extern uint16_t u16Adc_Iph_FBK_MCU2  ;
	extern uint16_t u16Adc_Iph_FBK_MCU5  ;
	extern uint16_t u16Adc_Iph_FBK_MCU8  ;
	extern uint16_t u16Adc_DCLI_FBK_MCU2 ;

	extern uint16_t u16Adc_LLV_FBK_MCU5  ;
	extern uint16_t u16Adc_LLV_FBK_MCU6  ;
	extern uint16_t u16Adc_Iph_FBK_MCU3  ;
	extern uint16_t u16Adc_Iph_FBK_MCU6  ;
	extern uint16_t u16Adc_Iph_FBK_MCU9  ;
	extern uint16_t u16Adc_DCLI_FBK_MCU3 ;

	extern uint16_t u16Adc_DCLV_FBK_MCU1 ;
	extern uint16_t u16Adc_DCLV_FBK_MCU2 ;
	extern uint16_t u16Adc_DCLV_FBK_MCU3 ;
	extern uint16_t u16Adc_DCLV_FBK_MCU4 ;
	extern uint16_t u16Adc_LLV_FBK_MCU2  ;
	extern uint16_t u16Adc_LLV_FBK_MCU4  ;

#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_SMALL_TEST

	extern uint16_t u16Value_I_U_ADC     ;
	extern uint16_t u16Value_VAC_UV_ADC  ;
	extern uint16_t u16Value_U_DC_FB_ADC ;
	extern uint16_t u16Value_I_U_OUT_ADC ;
	extern uint16_t u16Value_I_W_OUT_TRIP;

	extern uint16_t u16Value_I_W_ADC     ;
	extern uint16_t u16Value_VAC_VW_ADC  ;
	extern uint16_t u16Value_I_U_TRIP    ;
	extern uint16_t u16Value_I_V_OUT_ADC ;

	extern uint16_t u16Value_I_W_TRIP    ;
	extern uint16_t u16Value_I_W_OUT_ADC ;
	extern uint16_t u16Value_I_U_OUT_TRIP;

#endif


/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void ADC_vInit(void);
void ADC_vResetAquisitionTimeAllADC (void);
void ADC_vOffsetSetup(void);
void ADC_vReferenceOffsetSetup(void);
void ADC_vTwoComplementSetup(void);

void ADC_NewDataReset (void);
void ADC_vSetFunctionADCDataProcess (pfVoid_t pFunc);

uint16_t ADC_NewData (void);
__interrupt void ADCA_ISR1(void);
__interrupt void ADCB_ISR1(void);
__interrupt void ADCC_ISR1(void);
__interrupt void ADCD_ISR1(void);

#endif	/* ADC_DRV_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
