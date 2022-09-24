/* *****************************************************************************
 * File:   adc_drv.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 11 24 22:42
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
#include "adc_drv.h"
#include "boardcfg.h"
#include "system.h"

#if BOARDCFG_USE_DAC_AUTOMATION
	#include "dac_drv.h"
#endif



/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#if BOARDCFG_ADC_ISR_LOW_LEVEL == 0
	#define ADC_INTERRUPT_ACK_GROUP     INTERRUPT_ACK_GROUP1
	#define ADC_INT_NUMBER              ADC_INT_NUMBER1
#else
	#define ADC_INTERRUPT_ACK_GROUP     INTERRUPT_ACK_GROUP10
	#define ADC_INT_NUMBER              ADC_INT_NUMBER2
#endif


#define ADC_RESULTS_ARRAY_LENGTH    16

#if(ADC_RESOLUTION == 12)
	#define ADC_SAMPLE_WINDOW       75
#elif(ADC_RESOLUTION == 16)
	#define ADC_SAMPLE_WINDOW       64 //
#endif


/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */
#if BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC
	/* ADC_A */
	#define  DAC_OUT_A                  ADC_CH_ADCIN0   //						to_HIL_AO1    pin29_J7
	#define  DAC_OUT_B                  ADC_CH_ADCIN1   //						to_HIL_AO2    pin28_J7
	#define  I_FBK_DSP1                 ADC_CH_ADCIN2   // from_HIL_AIN3    pin7_J7
	#define  I_FBK_DSP2                 ADC_CH_ADCIN3   // from_HIL_AIN22           pin8_J6
	#define  I_FBK_DSP3                 ADC_CH_ADCIN4   // from_HIL_AIN5    pin9_J7
	#define  PV_CURRENT_DSP             ADC_CH_ADCIN5   // from_HIL_AIN2    pin4_J7
	/* ADC_B */
	#define  AC_VOLTAGE_L1_L2_DSP       ADC_CH_ADCIN0   // from_HIL_AIN18           pin4_J6
	#define  INLET_LIQUID_TEMP_DSP      ADC_CH_ADCIN1   // from_HIL_AIN14  pin16_J7
	#define  I_FBK_DSP4                 ADC_CH_ADCIN2   // from_HIL_AIN6    pin8_J7
	#define  I_FBK_DSP5                 ADC_CH_ADCIN3   // from_HIL_AIN7   pin11_J7
	#define  I_FBK_DSP6                 ADC_CH_ADCIN4   // from_HIL_AIN8   pin10_J7
	#define  AN_IN_AUX_1_DSP            ADC_CH_ADCIN5   // from_HIL_AIN20           pin6_J6
	/* ADC_C */
	#define  AIR_TEMP_SECTION_1_DSP     ADC_CH_ADCIN14  // from_HIL_AIN12  pin14_J7
	#define  AIR_TEMP_SECTION_2_DSP     ADC_CH_ADCIN15  // from_HIL_AIN13  pin17_J7
	#define  I_FBK_DSP7                 ADC_CH_ADCIN2   // from_HIL_AIN9   pin13_J7
	#define  I_FBK_DSP8                 ADC_CH_ADCIN3   // from_HIL_AIN10  pin12_J7
	#define  I_FBK_DSP9                 ADC_CH_ADCIN4   // from_HIL_AIN11  pin15_J7
	#define  AN_IN_AUX_2_DSP            ADC_CH_ADCIN5   // from_HIL_AIN21           pin9_J6
	/* ADC_D */
	#define  PV_VOLTAGE_DSP             ADC_CH_ADCIN0   // from_HIL_AIN1    pin5_J7
	#define  INLET_LIQUID_PRESS_DSP     ADC_CH_ADCIN1   // from_HIL_AIN16  pin18_J7
	#define  OUTLET_LIQUID_PRESS_DSP    ADC_CH_ADCIN2   // from_HIL_AIN17           pin5_J6
	#define  DC_LINK_DSP                ADC_CH_ADCIN3   // from_HIL_AIN4    pin6_J7
	#define  AC_VOLTAGE_L2_L3_DSP       ADC_CH_ADCIN4   // from_HIL_AIN19           pin7_J6
	#define  OUTLET_LIQUID_TEMP_DSP     ADC_CH_ADCIN5   // from_HIL_AIN15  pin19_J7

	/* SOC_ADC_A */
	#define  SOC_I_FBK_DSP1                 ADC_SOC_NUMBER0  //SOC_I_FBK_MCU1
	#define  SOC_I_FBK_DSP2                 ADC_SOC_NUMBER1  //SOC_I_FBK_MCU4
	#define  SOC_I_FBK_DSP3                 ADC_SOC_NUMBER2  //SOC_I_FBK_MCU7
	#define  SOC_DAC_OUT_A                  ADC_SOC_NUMBER3  //SOC_DACOUTA
	#define  SOC_DAC_OUT_B                  ADC_SOC_NUMBER4  //SOC_DACOUTB
	#define  SOC_PV_CURRENT_DSP             ADC_SOC_NUMBER5  //SOC_DCLI_FBK_MCU1
	/* SOC_ADC_B */
	#define  SOC_I_FBK_DSP4                 ADC_SOC_NUMBER0  //SOC_I_FBK_MCU2
	#define  SOC_I_FBK_DSP5                 ADC_SOC_NUMBER1  //SOC_I_FBK_MCU5
	#define  SOC_I_FBK_DSP6                 ADC_SOC_NUMBER2  //SOC_I_FBK_MCU8
	#define  SOC_AC_VOLTAGE_L1_L2_DSP       ADC_SOC_NUMBER3  //SOC_LLV_FBK_MCU1
	#define  SOC_INLET_LIQUID_TEMP_DSP      ADC_SOC_NUMBER4  //SOC_LLV_FBK_MCU3
	#define  SOC_AN_IN_AUX_1_DSP            ADC_SOC_NUMBER5  //SOC_DCLI_FBK_MCU2
	/* SOC_ADC_C */
	#define  SOC_I_FBK_DSP7                 ADC_SOC_NUMBER0  //SOC_I_FBK_MCU3
	#define  SOC_I_FBK_DSP8                 ADC_SOC_NUMBER1  //SOC_I_FBK_MCU6
	#define  SOC_I_FBK_DSP9                 ADC_SOC_NUMBER2  //SOC_I_FBK_MCU9
	#define  SOC_AIR_TEMP_SECTION_1_DSP     ADC_SOC_NUMBER3  //SOC_LLV_FBK_MCU5
	#define  SOC_AIR_TEMP_SECTION_2_DSP     ADC_SOC_NUMBER4  //SOC_LLV_FBK_MCU6
	#define  SOC_AN_IN_AUX_2_DSP            ADC_SOC_NUMBER5  //SOC_DCLI_FBK_MCU3
	/* SOC_ADC_D */
	#define  SOC_PV_VOLTAGE_DSP             ADC_SOC_NUMBER0  //SOC_DCLV_FBK_MCU1
	#define  SOC_INLET_LIQUID_PRESS_DSP     ADC_SOC_NUMBER1  //SOC_DCLV_FBK_MCU2
	#define  SOC_OUTLET_LIQUID_PRESS_DSP    ADC_SOC_NUMBER2  //SOC_DCLV_FBK_MCU3
	#define  SOC_DC_LINK_DSP                ADC_SOC_NUMBER3  //SOC_DCLV_FBK_MCU4
	#define  SOC_AC_VOLTAGE_L2_L3_DSP       ADC_SOC_NUMBER4  //SOC_LLV_FBK_MCU2
	#define  SOC_OUTLET_LIQUID_TEMP_DSP     ADC_SOC_NUMBER5  //SOC_LLV_FBK_MCU4

#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG

	/* ADCA */
	#define DACOUTA             ADC_CH_ADCIN0
	#define DACOUTB             ADC_CH_ADCIN1
	#define I_FBK_MCU1          ADC_CH_ADCIN2
	#define I_FBK_MCU4          ADC_CH_ADCIN3
	#define I_FBK_MCU7          ADC_CH_ADCIN4
	#define DCLI_FBK_MCU1       ADC_CH_ADCIN5

	/* ADCB */
	#define LLV_FBK_MCU1        ADC_CH_ADCIN0
	#define LLV_FBK_MCU3        ADC_CH_ADCIN1
	#define I_FBK_MCU2          ADC_CH_ADCIN2
	#define I_FBK_MCU5          ADC_CH_ADCIN3
	#define I_FBK_MCU8          ADC_CH_ADCIN4
	#define DCLI_FBK_MCU2       ADC_CH_ADCIN5

	/* ADCC */
	#define LLV_FBK_MCU5        ADC_CH_ADCIN14
	#define LLV_FBK_MCU6        ADC_CH_ADCIN15
	#define I_FBK_MCU3          ADC_CH_ADCIN2
	#define I_FBK_MCU6          ADC_CH_ADCIN3
	#define I_FBK_MCU9          ADC_CH_ADCIN4
	#define DCLI_FBK_MCU3       ADC_CH_ADCIN5

	/* ADCD */
	#define DCLV_FBK_MCU1       ADC_CH_ADCIN0
	#define DCLV_FBK_MCU2       ADC_CH_ADCIN1
	#define DCLV_FBK_MCU3       ADC_CH_ADCIN2
	#define DCLV_FBK_MCU4       ADC_CH_ADCIN3
	#define LLV_FBK_MCU2        ADC_CH_ADCIN4
	#define LLV_FBK_MCU4        ADC_CH_ADCIN5

	/* SOC */
	/* ADCA */
	#define SOC_I_FBK_MCU1      ADC_SOC_NUMBER0
	#define SOC_I_FBK_MCU4      ADC_SOC_NUMBER1
	#define SOC_I_FBK_MCU7      ADC_SOC_NUMBER2
	#define SOC_DACOUTA         ADC_SOC_NUMBER3
	#define SOC_DACOUTB         ADC_SOC_NUMBER4
	#define SOC_DCLI_FBK_MCU1   ADC_SOC_NUMBER5

	/* ADCB */
	#define SOC_I_FBK_MCU2      ADC_SOC_NUMBER0
	#define SOC_I_FBK_MCU5      ADC_SOC_NUMBER1
	#define SOC_I_FBK_MCU8      ADC_SOC_NUMBER2
	#define SOC_LLV_FBK_MCU1    ADC_SOC_NUMBER3
	#define SOC_LLV_FBK_MCU3    ADC_SOC_NUMBER4
	#define SOC_DCLI_FBK_MCU2   ADC_SOC_NUMBER5

	/* ADCC */
	#define SOC_I_FBK_MCU3      ADC_SOC_NUMBER0
	#define SOC_I_FBK_MCU6      ADC_SOC_NUMBER1
	#define SOC_I_FBK_MCU9      ADC_SOC_NUMBER2
	#define SOC_LLV_FBK_MCU5    ADC_SOC_NUMBER3
	#define SOC_LLV_FBK_MCU6    ADC_SOC_NUMBER4
	#define SOC_DCLI_FBK_MCU3   ADC_SOC_NUMBER5

	/* ADCD */
	#define SOC_DCLV_FBK_MCU1   ADC_SOC_NUMBER0
	#define SOC_DCLV_FBK_MCU2   ADC_SOC_NUMBER1
	#define SOC_DCLV_FBK_MCU3   ADC_SOC_NUMBER2
	#define SOC_DCLV_FBK_MCU4   ADC_SOC_NUMBER3
	#define SOC_LLV_FBK_MCU2    ADC_SOC_NUMBER4
	#define SOC_LLV_FBK_MCU4    ADC_SOC_NUMBER5

#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_SMALL_TEST
	/* ADCA */
	#define I_U_ADC         ADC_CH_ADCIN0
	#define VAC_UV_ADC      ADC_CH_ADCIN1
	#define U_DC_FB_ADC     ADC_CH_ADCIN2
	#define I_U_OUT_ADC     ADC_CH_ADCIN3
	#define I_W_OUT_TRIP    ADC_CH_ADCIN4

	/* ADCB */
	#define I_W_ADC         ADC_CH_ADCIN0
	#define VAC_VW_ADC      ADC_CH_ADCIN1
	#define I_U_TRIP        ADC_CH_ADCIN2
	#define I_V_OUT_ADC     ADC_CH_ADCIN3

	/* ADCD */
	#define I_W_TRIP        ADC_CH_ADCIN14
	#define I_W_OUT_ADC     ADC_CH_ADCIN0
	#define NOT_USED_D1     ADC_CH_ADCIN1
	#define I_U_OUT_TRIP    ADC_CH_ADCIN2
	#define NOT_USED_D3     ADC_CH_ADCIN3
	#define NOT_USED_D4     ADC_CH_ADCIN4

	/* SOC */
	#define I_U_ADC_SOC         ADC_SOC_NUMBER0
	#define VAC_UV_ADC_SOC      ADC_SOC_NUMBER1
	#define U_DC_FB_ADC_SOC     ADC_SOC_NUMBER2
	#define I_U_OUT_ADC_SOC     ADC_SOC_NUMBER3
	#define I_W_OUT_TRIP_SOC    ADC_SOC_NUMBER4

	#define I_W_ADC_SOC         ADC_SOC_NUMBER0
	#define VAC_VW_ADC_SOC      ADC_SOC_NUMBER1
	#define I_U_TRIP_SOC        ADC_SOC_NUMBER2
	#define I_V_OUT_ADC_SOC     ADC_SOC_NUMBER3

	#define I_W_TRIP_SOC        ADC_SOC_NUMBER0
	#define I_W_OUT_ADC_SOC     ADC_SOC_NUMBER1
	#define I_U_OUT_TRIP_SOC    ADC_SOC_NUMBER2

#else
	#error "Board Not Implemented"
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

pfVoid_t pfADCDataProcess = NULL;

uint16_t sampleWindow = ADC_SAMPLE_WINDOW;

uint16_t adcAResults[ADC_RESULTS_ARRAY_LENGTH];   // Buffer for results
uint16_t adcBResults[ADC_RESULTS_ARRAY_LENGTH];   // Buffer for results
#ifdef ADCC_BASE
uint16_t adcCResults[ADC_RESULTS_ARRAY_LENGTH];   // Buffer for results
#endif
uint16_t adcDResults[ADC_RESULTS_ARRAY_LENGTH];   // Buffer for results


#if BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC
	/* ADC_A */
	uint16_t u16Adc_DAC_outA       = 0;
	uint16_t u16Adc_DAC_outB       = 0;
	uint16_t u16Adc_Iph_FBK_DSP1   = 0;
	uint16_t u16Adc_Iph_FBK_DSP2   = 0;
	uint16_t u16Adc_Iph_FBK_DSP3   = 0;
	uint16_t u16Adc_PV_Current_DSP = 0;
	/* ADC_B */
	uint16_t u16Adc_AC_Voltage_L1_L2_DSP         = 0;
	uint16_t u16Adc_Inlet_Liquid_Temperature_DSP = 0;
	uint16_t u16Adc_Iph_FBK_DSP4                 = 0;
	uint16_t u16Adc_Iph_FBK_DSP5                 = 0;
	uint16_t u16Adc_Iph_FBK_DSP6                 = 0;
	uint16_t u16Adc_AN_IN_Aux1_DSP               = 0;
	/* ADC_C */
	uint16_t u16Adc_Air_Temperature_Section_1_DSP = 0;
	uint16_t u16Adc_Air_Temperature_Section_2_DSP = 0;
	uint16_t u16Adc_Iph_FBK_DSP7                  = 0;
	uint16_t u16Adc_Iph_FBK_DSP8                  = 0;
	uint16_t u16Adc_Iph_FBK_DSP9                  = 0;
	uint16_t u16Adc_AN_IN_Aux2_DSP                = 0;
	/* ADC_D */
	uint16_t u16Adc_PV_Voltage_DSP                = 0;
	uint16_t u16Adc_Inlet_Liquid_Pressure_DSP     = 0;
	uint16_t u16Adc_Outlet_Liquid_Pressure_DSP    = 0;
	uint16_t u16Adc_DC_Link_DSP                   = 0;
	uint16_t u16Adc_AC_Voltage_L2_L3_DSP          = 0;
	uint16_t u16Adc_Outlet_Liquid_Temperature_DSP = 0;

	/* ADC_A */
	int16_t Offset_DAC_outA       = 0;
	int16_t Offset_DAC_outB       = 0;
	int16_t Offset_Iph_FBK_DSP1   = 0;
	int16_t Offset_Iph_FBK_DSP2   = 0;
	int16_t Offset_Iph_FBK_DSP3   = 0;
	int16_t Offset_PV_Current_DSP = 0;
	/* ADC_B */
	int16_t Offset_AC_Voltage_L1_L2_DSP         = 0;
	int16_t Offset_Inlet_Liquid_Temperature_DSP = 0;
	int16_t Offset_Iph_FBK_DSP4                 = 0;
	int16_t Offset_Iph_FBK_DSP5                 = 0;
	int16_t Offset_Iph_FBK_DSP6                 = 0;
	int16_t Offset_AN_IN_Aux1_DSP               = 0;
	/* ADC_C */
	int16_t Offset_Air_Temperature_Section_1_DSP = 0;
	int16_t Offset_Air_Temperature_Section_2_DSP = 0;
	int16_t Offset_Iph_FBK_DSP7                  = 0;
	int16_t Offset_Iph_FBK_DSP8                  = 0;
	int16_t Offset_Iph_FBK_DSP9                  = 0;
	int16_t Offset_AN_IN_Aux2_DSP                = 0;
	/* ADC_D */
	int16_t Offset_PV_Voltage_DSP                = 0;
	int16_t Offset_Inlet_Liquid_Pressure_DSP     = 0;
	int16_t Offset_Outlet_Liquid_Pressure_DSP    = 0;
	int16_t Offset_DC_Link_DSP                   = 0;
	int16_t Offset_AC_Voltage_L2_L3_DSP          = 0;
	int16_t Offset_Outlet_Liquid_Temperature_DSP = 0;

	/* ADC_A */
	int16_t ReferenceOffset_DAC_outA       = 0;
	int16_t ReferenceOffset_DAC_outB       = 0;
	int16_t ReferenceOffset_Iph_FBK_DSP1   = 0;
	int16_t ReferenceOffset_Iph_FBK_DSP2   = 0;
	int16_t ReferenceOffset_Iph_FBK_DSP3   = 0;
	int16_t ReferenceOffset_PV_Current_DSP = 0;
	/* ADC_B */
	int16_t ReferenceOffset_AC_Voltage_L1_L2_DSP         = 0;
	int16_t ReferenceOffset_Inlet_Liquid_Temperature_DSP = 0;
	int16_t ReferenceOffset_Iph_FBK_DSP4                 = 0;
	int16_t ReferenceOffset_Iph_FBK_DSP5                 = 0;
	int16_t ReferenceOffset_Iph_FBK_DSP6                 = 0;
	int16_t ReferenceOffset_AN_IN_Aux1_DSP               = 0;
	/* ADC_C */
	int16_t ReferenceOffset_Air_Temperature_Section_1_DSP = 0;
	int16_t ReferenceOffset_Air_Temperature_Section_2_DSP = 0;
	int16_t ReferenceOffset_Iph_FBK_DSP7                  = 0;
	int16_t ReferenceOffset_Iph_FBK_DSP8                  = 0;
	int16_t ReferenceOffset_Iph_FBK_DSP9                  = 0;
	int16_t ReferenceOffset_AN_IN_Aux2_DSP                = 0;
	/* ADC_D */
	int16_t ReferenceOffset_PV_Voltage_DSP                = 0;
	int16_t ReferenceOffset_Inlet_Liquid_Pressure_DSP     = 0;
	int16_t ReferenceOffset_Outlet_Liquid_Pressure_DSP    = 0;
	int16_t ReferenceOffset_DC_Link_DSP                   = 0;
	int16_t ReferenceOffset_AC_Voltage_L2_L3_DSP          = 0;
	int16_t ReferenceOffset_Outlet_Liquid_Temperature_DSP = 0;

	/* ADC_A */
	int16_t TwoComplement_DAC_outA       = 0;
	int16_t TwoComplement_DAC_outB       = 0;
	int16_t TwoComplement_Iph_FBK_DSP1   = 0;
	int16_t TwoComplement_Iph_FBK_DSP2   = 0;
	int16_t TwoComplement_Iph_FBK_DSP3   = 0;
	int16_t TwoComplement_PV_Current_DSP = 0;
	/* ADC_B */
	int16_t TwoComplement_AC_Voltage_L1_L2_DSP         = 0;
	int16_t TwoComplement_Inlet_Liquid_Temperature_DSP = 0;
	int16_t TwoComplement_Iph_FBK_DSP4                 = 0;
	int16_t TwoComplement_Iph_FBK_DSP5                 = 0;
	int16_t TwoComplement_Iph_FBK_DSP6                 = 0;
	int16_t TwoComplement_AN_IN_Aux1_DSP               = 0;
	/* ADC_C */
	int16_t TwoComplement_Air_Temperature_Section_1_DSP = 0;
	int16_t TwoComplement_Air_Temperature_Section_2_DSP = 0;
	int16_t TwoComplement_Iph_FBK_DSP7                  = 0;
	int16_t TwoComplement_Iph_FBK_DSP8                  = 0;
	int16_t TwoComplement_Iph_FBK_DSP9                  = 0;
	int16_t TwoComplement_AN_IN_Aux2_DSP                = 0;
	/* ADC_D */
	int16_t TwoComplement_PV_Voltage_DSP                = 0;
	int16_t TwoComplement_Inlet_Liquid_Pressure_DSP     = 0;
	int16_t TwoComplement_Outlet_Liquid_Pressure_DSP    = 0;
	int16_t TwoComplement_DC_Link_DSP                   = 0;
	int16_t TwoComplement_AC_Voltage_L2_L3_DSP          = 0;
	int16_t TwoComplement_Outlet_Liquid_Temperature_DSP = 0;

	/* ADC_A */
	int16_t s16Adc_NoOffset_DAC_outA       = 0;
	int16_t s16Adc_NoOffset_DAC_outB       = 0;
	int16_t s16Adc_NoOffset_Iph_FBK_DSP1   = 0;
	int16_t s16Adc_NoOffset_Iph_FBK_DSP2   = 0;
	int16_t s16Adc_NoOffset_Iph_FBK_DSP3   = 0;
	int16_t s16Adc_NoOffset_PV_Current_DSP = 0;
	/* ADC_B */
	int16_t s16Adc_NoOffset_AC_Voltage_L1_L2_DSP         = 0;
	int16_t s16Adc_NoOffset_Inlet_Liquid_Temperature_DSP = 0;
	int16_t s16Adc_NoOffset_Iph_FBK_DSP4                 = 0;
	int16_t s16Adc_NoOffset_Iph_FBK_DSP5                 = 0;
	int16_t s16Adc_NoOffset_Iph_FBK_DSP6                 = 0;
	int16_t s16Adc_NoOffset_AN_IN_Aux1_DSP               = 0;
	/* ADC_C */
	int16_t s16Adc_NoOffset_Air_Temperature_Section_1_DSP = 0;
	int16_t s16Adc_NoOffset_Air_Temperature_Section_2_DSP = 0;
	int16_t s16Adc_NoOffset_Iph_FBK_DSP7                  = 0;
	int16_t s16Adc_NoOffset_Iph_FBK_DSP8                  = 0;
	int16_t s16Adc_NoOffset_Iph_FBK_DSP9                  = 0;
	int16_t s16Adc_NoOffset_AN_IN_Aux2_DSP                = 0;
	/* ADC_D */
	int16_t s16Adc_NoOffset_PV_Voltage_DSP                = 0;
	int16_t s16Adc_NoOffset_Inlet_Liquid_Pressure_DSP     = 0;
	int16_t s16Adc_NoOffset_Outlet_Liquid_Pressure_DSP    = 0;
	int16_t s16Adc_NoOffset_DC_Link_DSP                   = 0;
	int16_t s16Adc_NoOffset_AC_Voltage_L2_L3_DSP          = 0;
	int16_t s16Adc_NoOffset_Outlet_Liquid_Temperature_DSP = 0;

	/* ADC_A */
	uint16_t u16Adc_NoOffset_DAC_outA       = 0;
	uint16_t u16Adc_NoOffset_DAC_outB       = 0;
	uint16_t u16Adc_NoOffset_Iph_FBK_DSP1   = 0;
	uint16_t u16Adc_NoOffset_Iph_FBK_DSP2   = 0;
	uint16_t u16Adc_NoOffset_Iph_FBK_DSP3   = 0;
	uint16_t u16Adc_NoOffset_PV_Current_DSP = 0;
	/* ADC_B */
	uint16_t u16Adc_NoOffset_AC_Voltage_L1_L2_DSP         = 0;
	uint16_t u16Adc_NoOffset_Inlet_Liquid_Temperature_DSP = 0;
	uint16_t u16Adc_NoOffset_Iph_FBK_DSP4                 = 0;
	uint16_t u16Adc_NoOffset_Iph_FBK_DSP5                 = 0;
	uint16_t u16Adc_NoOffset_Iph_FBK_DSP6                 = 0;
	uint16_t u16Adc_NoOffset_AN_IN_Aux1_DSP               = 0;
	/* ADC_C */
	uint16_t u16Adc_NoOffset_Air_Temperature_Section_1_DSP = 0;
	uint16_t u16Adc_NoOffset_Air_Temperature_Section_2_DSP = 0;
	uint16_t u16Adc_NoOffset_Iph_FBK_DSP7                  = 0;
	uint16_t u16Adc_NoOffset_Iph_FBK_DSP8                  = 0;
	uint16_t u16Adc_NoOffset_Iph_FBK_DSP9                  = 0;
	uint16_t u16Adc_NoOffset_AN_IN_Aux2_DSP                = 0;
	/* ADC_D */
	uint16_t u16Adc_NoOffset_PV_Voltage_DSP                = 0;
	uint16_t u16Adc_NoOffset_Inlet_Liquid_Pressure_DSP     = 0;
	uint16_t u16Adc_NoOffset_Outlet_Liquid_Pressure_DSP    = 0;
	uint16_t u16Adc_NoOffset_DC_Link_DSP                   = 0;
	uint16_t u16Adc_NoOffset_AC_Voltage_L2_L3_DSP          = 0;
	uint16_t u16Adc_NoOffset_Outlet_Liquid_Temperature_DSP = 0;

	/* ADC_A */
	uint16_t u16Adc_NoOffsetSign_DAC_outA       = 0;
	uint16_t u16Adc_NoOffsetSign_DAC_outB       = 0;
	uint16_t u16Adc_NoOffsetSign_Iph_FBK_DSP1   = 0;
	uint16_t u16Adc_NoOffsetSign_Iph_FBK_DSP2   = 0;
	uint16_t u16Adc_NoOffsetSign_Iph_FBK_DSP3   = 0;
	uint16_t u16Adc_NoOffsetSign_PV_Current_DSP = 0;
	/* ADC_B */
	uint16_t u16Adc_NoOffsetSign_AC_Voltage_L1_L2_DSP         = 0;
	uint16_t u16Adc_NoOffsetSign_Inlet_Liquid_Temperature_DSP = 0;
	uint16_t u16Adc_NoOffsetSign_Iph_FBK_DSP4                 = 0;
	uint16_t u16Adc_NoOffsetSign_Iph_FBK_DSP5                 = 0;
	uint16_t u16Adc_NoOffsetSign_Iph_FBK_DSP6                 = 0;
	uint16_t u16Adc_NoOffsetSign_AN_IN_Aux1_DSP               = 0;
	/* ADC_C */
	uint16_t u16Adc_NoOffsetSign_Air_Temperature_Section_1_DSP = 0;
	uint16_t u16Adc_NoOffsetSign_Air_Temperature_Section_2_DSP = 0;
	uint16_t u16Adc_NoOffsetSign_Iph_FBK_DSP7                  = 0;
	uint16_t u16Adc_NoOffsetSign_Iph_FBK_DSP8                  = 0;
	uint16_t u16Adc_NoOffsetSign_Iph_FBK_DSP9                  = 0;
	uint16_t u16Adc_NoOffsetSign_AN_IN_Aux2_DSP                = 0;
	/* ADC_D */
	uint16_t u16Adc_NoOffsetSign_PV_Voltage_DSP                = 0;
	uint16_t u16Adc_NoOffsetSign_Inlet_Liquid_Pressure_DSP     = 0;
	uint16_t u16Adc_NoOffsetSign_Outlet_Liquid_Pressure_DSP    = 0;
	uint16_t u16Adc_NoOffsetSign_DC_Link_DSP                   = 0;
	uint16_t u16Adc_NoOffsetSign_AC_Voltage_L2_L3_DSP          = 0;
	uint16_t u16Adc_NoOffsetSign_Outlet_Liquid_Temperature_DSP = 0;


#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG

	uint16_t u16Adc_DACOUTA        = 0;
	uint16_t u16Adc_DACOUTB        = 0;
	uint16_t u16Adc_Iph_FBK_MCU1   = 0;
	uint16_t u16Adc_Iph_FBK_MCU4   = 0;
	uint16_t u16Adc_Iph_FBK_MCU7   = 0;
	uint16_t u16Adc_DCLI_FBK_MCU1  = 0;

	uint16_t u16Adc_LLV_FBK_MCU1    = 0;
	uint16_t u16Adc_LLV_FBK_MCU3    = 0;
	uint16_t u16Adc_Iph_FBK_MCU2    = 0;
	uint16_t u16Adc_Iph_FBK_MCU5    = 0;
	uint16_t u16Adc_Iph_FBK_MCU8    = 0;
	uint16_t u16Adc_DCLI_FBK_MCU2   = 0;

	uint16_t u16Adc_LLV_FBK_MCU5    = 0;
	uint16_t u16Adc_LLV_FBK_MCU6    = 0;
	uint16_t u16Adc_Iph_FBK_MCU3    = 0;
	uint16_t u16Adc_Iph_FBK_MCU6    = 0;
	uint16_t u16Adc_Iph_FBK_MCU9    = 0;
	uint16_t u16Adc_DCLI_FBK_MCU3   = 0;

	uint16_t u16Adc_DCLV_FBK_MCU1   = 0;
	uint16_t u16Adc_DCLV_FBK_MCU2   = 0;
	uint16_t u16Adc_DCLV_FBK_MCU3   = 0;
	uint16_t u16Adc_DCLV_FBK_MCU4   = 0;
	uint16_t u16Adc_LLV_FBK_MCU2    = 0;
	uint16_t u16Adc_LLV_FBK_MCU4    = 0;




	//int16_t Offset_DACOUTA          = 0;
	//int16_t Offset_DACOUTB          = 0;
	int16_t Offset_Iph_FBK_MCU1     = 0;
	int16_t Offset_Iph_FBK_MCU4     = 0;
	int16_t Offset_Iph_FBK_MCU7     = 0;
	int16_t Offset_DCLI_FBK_MCU1    = 0;

	//int16_t Offset_LLV_FBK_MCU1     = 0;
	//int16_t Offset_LLV_FBK_MCU3     = 0;
	int16_t Offset_Iph_FBK_MCU2     = 0;
	int16_t Offset_Iph_FBK_MCU5     = 0;
	int16_t Offset_Iph_FBK_MCU8     = 0;
	int16_t Offset_DCLI_FBK_MCU2    = 0;

	//int16_t Offset_LLV_FBK_MCU5     = 0;
	//int16_t Offset_LLV_FBK_MCU6     = 0;
	int16_t Offset_Iph_FBK_MCU3     = 0;
	int16_t Offset_Iph_FBK_MCU6     = 0;
	int16_t Offset_Iph_FBK_MCU9     = 0;
	int16_t Offset_DCLI_FBK_MCU3    = 0;

	int16_t Offset_DCLV_FBK_MCU1    = 0;
	int16_t Offset_DCLV_FBK_MCU2    = 0;
	int16_t Offset_DCLV_FBK_MCU3    = 0;
	int16_t Offset_DCLV_FBK_MCU4    = 0;
	//int16_t Offset_LLV_FBK_MCU2     = 0;
	//int16_t Offset_LLV_FBK_MCU4     = 0;



	//int16_t ReferenceOffset_DACOUTA         = 0;
	//int16_t ReferenceOffset_DACOUTB         = 0;
	int16_t ReferenceOffset_Iph_FBK_MCU1    = 0;
	int16_t ReferenceOffset_Iph_FBK_MCU4    = 0;
	int16_t ReferenceOffset_Iph_FBK_MCU7    = 0;
	int16_t ReferenceOffset_DCLI_FBK_MCU1   = 0;

	//int16_t ReferenceOffset_LLV_FBK_MCU1    = 0;
	//int16_t ReferenceOffset_LLV_FBK_MCU3    = 0;
	int16_t ReferenceOffset_Iph_FBK_MCU2    = 0;
	int16_t ReferenceOffset_Iph_FBK_MCU5    = 0;
	int16_t ReferenceOffset_Iph_FBK_MCU8    = 0;
	int16_t ReferenceOffset_DCLI_FBK_MCU2   = 0;

	//int16_t ReferenceOffset_LLV_FBK_MCU5    = 0;
	//int16_t ReferenceOffset_LLV_FBK_MCU6    = 0;
	int16_t ReferenceOffset_Iph_FBK_MCU3    = 0;
	int16_t ReferenceOffset_Iph_FBK_MCU6    = 0;
	int16_t ReferenceOffset_Iph_FBK_MCU9    = 0;
	int16_t ReferenceOffset_DCLI_FBK_MCU3   = 0;

	int16_t ReferenceOffset_DCLV_FBK_MCU1   = 0;
	int16_t ReferenceOffset_DCLV_FBK_MCU2   = 0;
	int16_t ReferenceOffset_DCLV_FBK_MCU3   = 0;
	int16_t ReferenceOffset_DCLV_FBK_MCU4   = 0;
	//int16_t ReferenceOffset_LLV_FBK_MCU2    = 0;
	//int16_t ReferenceOffset_LLV_FBK_MCU4    = 0;



	//int16_t TwoComplement_DACOUTA           = 0;
	//int16_t TwoComplement_DACOUTB           = 0;
	int16_t TwoComplement_Iph_FBK_MCU1      = 0;
	int16_t TwoComplement_Iph_FBK_MCU4      = 0;
	int16_t TwoComplement_Iph_FBK_MCU7      = 0;
	int16_t TwoComplement_DCLI_FBK_MCU1     = 0;

	//int16_t TwoComplement_LLV_FBK_MCU1      = 0;
	//int16_t TwoComplement_LLV_FBK_MCU3      = 0;
	int16_t TwoComplement_Iph_FBK_MCU2      = 0;
	int16_t TwoComplement_Iph_FBK_MCU5      = 0;
	int16_t TwoComplement_Iph_FBK_MCU8      = 0;
	int16_t TwoComplement_DCLI_FBK_MCU2     = 0;

	//int16_t TwoComplement_LLV_FBK_MCU5      = 0;
	//int16_t TwoComplement_LLV_FBK_MCU6      = 0;
	int16_t TwoComplement_Iph_FBK_MCU3      = 0;
	int16_t TwoComplement_Iph_FBK_MCU6      = 0;
	int16_t TwoComplement_Iph_FBK_MCU9      = 0;
	int16_t TwoComplement_DCLI_FBK_MCU3     = 0;

	int16_t TwoComplement_DCLV_FBK_MCU1     = 0;
	int16_t TwoComplement_DCLV_FBK_MCU2     = 0;
	int16_t TwoComplement_DCLV_FBK_MCU3     = 0;
	int16_t TwoComplement_DCLV_FBK_MCU4     = 0;
	//int16_t TwoComplement_LLV_FBK_MCU2      = 0;
	//int16_t TwoComplement_LLV_FBK_MCU4      = 0;



	int16_t s16Adc_NoOffset_DACOUTA        = 0;
	int16_t s16Adc_NoOffset_DACOUTB        = 0;
	int16_t s16Adc_NoOffset_Iph_FBK_MCU1   = 0;
	int16_t s16Adc_NoOffset_Iph_FBK_MCU4   = 0;
	int16_t s16Adc_NoOffset_Iph_FBK_MCU7   = 0;
	int16_t s16Adc_NoOffset_DCLI_FBK_MCU1  = 0;

	int16_t s16Adc_NoOffset_LLV_FBK_MCU1   = 0;
	int16_t s16Adc_NoOffset_LLV_FBK_MCU3   = 0;
	int16_t s16Adc_NoOffset_Iph_FBK_MCU2   = 0;
	int16_t s16Adc_NoOffset_Iph_FBK_MCU5   = 0;
	int16_t s16Adc_NoOffset_Iph_FBK_MCU8   = 0;
	int16_t s16Adc_NoOffset_DCLI_FBK_MCU2  = 0;

	int16_t s16Adc_NoOffset_LLV_FBK_MCU5   = 0;
	int16_t s16Adc_NoOffset_LLV_FBK_MCU6   = 0;
	int16_t s16Adc_NoOffset_Iph_FBK_MCU3   = 0;
	int16_t s16Adc_NoOffset_Iph_FBK_MCU6   = 0;
	int16_t s16Adc_NoOffset_Iph_FBK_MCU9   = 0;
	int16_t s16Adc_NoOffset_DCLI_FBK_MCU3  = 0;

	int16_t s16Adc_NoOffset_DCLV_FBK_MCU1  = 0;
	int16_t s16Adc_NoOffset_DCLV_FBK_MCU2  = 0;
	int16_t s16Adc_NoOffset_DCLV_FBK_MCU3  = 0;
	int16_t s16Adc_NoOffset_DCLV_FBK_MCU4  = 0;
	int16_t s16Adc_NoOffset_LLV_FBK_MCU2   = 0;
	int16_t s16Adc_NoOffset_LLV_FBK_MCU4   = 0;



	uint16_t u16Adc_NoOffset_DACOUTA        = 0;
	uint16_t u16Adc_NoOffset_DACOUTB        = 0;
	uint16_t u16Adc_NoOffset_Iph_FBK_MCU1   = 0;
	uint16_t u16Adc_NoOffset_Iph_FBK_MCU4   = 0;
	uint16_t u16Adc_NoOffset_Iph_FBK_MCU7   = 0;
	uint16_t u16Adc_NoOffset_DCLI_FBK_MCU1  = 0;

	uint16_t u16Adc_NoOffset_LLV_FBK_MCU1   = 0;
	uint16_t u16Adc_NoOffset_LLV_FBK_MCU3   = 0;
	uint16_t u16Adc_NoOffset_Iph_FBK_MCU2   = 0;
	uint16_t u16Adc_NoOffset_Iph_FBK_MCU5   = 0;
	uint16_t u16Adc_NoOffset_Iph_FBK_MCU8   = 0;
	uint16_t u16Adc_NoOffset_DCLI_FBK_MCU2  = 0;

	uint16_t u16Adc_NoOffset_LLV_FBK_MCU5   = 0;
	uint16_t u16Adc_NoOffset_LLV_FBK_MCU6   = 0;
	uint16_t u16Adc_NoOffset_Iph_FBK_MCU3   = 0;
	uint16_t u16Adc_NoOffset_Iph_FBK_MCU6   = 0;
	uint16_t u16Adc_NoOffset_Iph_FBK_MCU9   = 0;
	uint16_t u16Adc_NoOffset_DCLI_FBK_MCU3  = 0;

	uint16_t u16Adc_NoOffset_DCLV_FBK_MCU1  = 0;
	uint16_t u16Adc_NoOffset_DCLV_FBK_MCU2  = 0;
	uint16_t u16Adc_NoOffset_DCLV_FBK_MCU3  = 0;
	uint16_t u16Adc_NoOffset_DCLV_FBK_MCU4  = 0;
	uint16_t u16Adc_NoOffset_LLV_FBK_MCU2   = 0;
	uint16_t u16Adc_NoOffset_LLV_FBK_MCU4   = 0;



	uint16_t u16Adc_NoOffsetSign_DACOUTA        = 0;
	uint16_t u16Adc_NoOffsetSign_DACOUTB        = 0;
	uint16_t u16Adc_NoOffsetSign_Iph_FBK_MCU1   = 0;
	uint16_t u16Adc_NoOffsetSign_Iph_FBK_MCU4   = 0;
	uint16_t u16Adc_NoOffsetSign_Iph_FBK_MCU7   = 0;
	uint16_t u16Adc_NoOffsetSign_DCLI_FBK_MCU1  = 0;

	uint16_t u16Adc_NoOffsetSign_LLV_FBK_MCU1   = 0;
	uint16_t u16Adc_NoOffsetSign_LLV_FBK_MCU3   = 0;
	uint16_t u16Adc_NoOffsetSign_Iph_FBK_MCU2   = 0;
	uint16_t u16Adc_NoOffsetSign_Iph_FBK_MCU5   = 0;
	uint16_t u16Adc_NoOffsetSign_Iph_FBK_MCU8   = 0;
	uint16_t u16Adc_NoOffsetSign_DCLI_FBK_MCU2  = 0;

	uint16_t u16Adc_NoOffsetSign_LLV_FBK_MCU5   = 0;
	uint16_t u16Adc_NoOffsetSign_LLV_FBK_MCU6   = 0;
	uint16_t u16Adc_NoOffsetSign_Iph_FBK_MCU3   = 0;
	uint16_t u16Adc_NoOffsetSign_Iph_FBK_MCU6   = 0;
	uint16_t u16Adc_NoOffsetSign_Iph_FBK_MCU9   = 0;
	uint16_t u16Adc_NoOffsetSign_DCLI_FBK_MCU3  = 0;

	uint16_t u16Adc_NoOffsetSign_DCLV_FBK_MCU1  = 0;
	uint16_t u16Adc_NoOffsetSign_DCLV_FBK_MCU2  = 0;
	uint16_t u16Adc_NoOffsetSign_DCLV_FBK_MCU3  = 0;
	uint16_t u16Adc_NoOffsetSign_DCLV_FBK_MCU4  = 0;
	uint16_t u16Adc_NoOffsetSign_LLV_FBK_MCU2   = 0;
	uint16_t u16Adc_NoOffsetSign_LLV_FBK_MCU4   = 0;


#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_SMALL_TEST

	uint16_t u16Value_I_U_ADC = 0;
	uint16_t u16Value_VAC_UV_ADC = 0;
	uint16_t u16Value_U_DC_FB_ADC = 0;
	uint16_t u16Value_I_U_OUT_ADC = 0;
	uint16_t u16Value_I_W_OUT_TRIP = 0;

	uint16_t u16Value_I_W_ADC = 0;
	uint16_t u16Value_VAC_VW_ADC = 0;
	uint16_t u16Value_I_U_TRIP = 0;
	uint16_t u16Value_I_V_OUT_ADC = 0;

	uint16_t u16Value_I_W_TRIP = 0;
	uint16_t u16Value_I_W_OUT_ADC = 0;
	uint16_t u16Value_I_U_OUT_TRIP = 0;

#else
	#error "Board Not Implemented"
#endif


/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */
void ADC_vInitModule(uint32_t base);
void ADC_SOCInit(uint32_t base, ADC_Trigger adcTrigger, ADC_Channel ch0, ADC_Channel ch1, ADC_Channel ch2, ADC_Channel ch3, ADC_Channel ch4, ADC_Channel ch5, uint16_t chCount, ADC_SOCNumber socTriggerISR );



/* *****************************************************************************
 * Functions
 **************************************************************************** */
//
// Function to configure and power up ADC.  Input example: ADCA_BASE
//
void ADC_vInitModule(uint32_t base)
{
    // Set ADCCLK divider to /4
    ADC_setPrescaler(base, ADC_CLK_DIV_4_0);

    // Set pulse positions to late
    ADC_setInterruptPulseMode(base, ADC_PULSE_END_OF_CONV);

    // Power up the ADC and then delay for 1 ms
    ADC_enableConverter(base);

    DEVICE_DELAY_US(1000);
}


//
// Function to configure ePWM to generate the SOC.
//
void ADC_InitTriggerEPWM(uint32_t base)
{
    // Disable SOCA
    EPWM_disableADCTrigger(base, EPWM_SOC_A);

    // Configure the SOC to occur on the zero match event
    EPWM_setADCTriggerSource(base, EPWM_SOC_A, EPWM_SOC_TBCTR_ZERO);
    EPWM_setADCTriggerEventPrescale(base, EPWM_SOC_A, 1);

    EPWM_enableADCTrigger(base, EPWM_SOC_A);
}


//
// Function to configure ADC's SOC0 to be triggered by ePWM6.
//
void ADC_SOCInit(uint32_t base, ADC_Trigger adcTrigger, ADC_Channel ch0, ADC_Channel ch1, ADC_Channel ch2, ADC_Channel ch3, ADC_Channel ch4, ADC_Channel ch5, uint16_t chCount, ADC_SOCNumber socTriggerISR )
{
    // For 12-bit resolution, a sampling window of 15 ( 75 ns at a 200MHz SYSCLK rate) will be used.
	// For 16-bit resolution, a sampling window of 64 (320 ns at a 200MHz SYSCLK rate) will be used.
    if (chCount > 0) ADC_setupSOC(base, ADC_SOC_NUMBER0, adcTrigger, ch0, sampleWindow);
    if (chCount > 1) ADC_setupSOC(base, ADC_SOC_NUMBER1, adcTrigger, ch1, sampleWindow);
    if (chCount > 2) ADC_setupSOC(base, ADC_SOC_NUMBER2, adcTrigger, ch2, sampleWindow);
    if (chCount > 3) ADC_setupSOC(base, ADC_SOC_NUMBER3, adcTrigger, ch3, sampleWindow);
    if (chCount > 4) ADC_setupSOC(base, ADC_SOC_NUMBER4, adcTrigger, ch4, sampleWindow);
    if (chCount > 5) ADC_setupSOC(base, ADC_SOC_NUMBER5, adcTrigger, ch5, sampleWindow);

    // Set SOC0 to set the interrupt 1 flag. Enable the interrupt and make sure its flag is cleared.
    ADC_setInterruptSource(base, ADC_INT_NUMBER, socTriggerISR);
    ADC_enableInterrupt(base, ADC_INT_NUMBER);
    ADC_clearInterruptStatus(base, ADC_INT_NUMBER);
}


void ADC_vInit(void)
{
    uint16_t index;  // Index into result buffer

    ADC_vInitModule(ADCA_BASE);
    ADC_vInitModule(ADCB_BASE);
#ifdef ADCC_BASE
    ADC_vInitModule(ADCC_BASE);
#endif
    ADC_vInitModule(ADCD_BASE);


#if BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC
    ADC_InitTriggerEPWM(EPWM1_BASE);
    /* ADC_A */
	ADC_SOCInit(ADCA_BASE, ADC_TRIGGER_EPWM1_SOCA,
				(ADC_Channel) I_FBK_DSP1,             //I_FBK_MCU1,
				(ADC_Channel) I_FBK_DSP2,             //I_FBK_MCU4,
				(ADC_Channel) I_FBK_DSP3,             //I_FBK_MCU7,
				(ADC_Channel) DAC_OUT_A,              //DACOUTA,
				(ADC_Channel) DAC_OUT_B,              //DACOUTB,
				(ADC_Channel) PV_CURRENT_DSP,         //DCLI_FBK_MCU1,
				6,
				(ADC_SOCNumber) (SOC_PV_CURRENT_DSP));
	/* ADC_B */
	ADC_SOCInit(ADCB_BASE, ADC_TRIGGER_EPWM1_SOCA,
				(ADC_Channel) I_FBK_DSP4,             //I_FBK_MCU2,
				(ADC_Channel) I_FBK_DSP5,             //I_FBK_MCU5,
				(ADC_Channel) I_FBK_DSP6,             //I_FBK_MCU8,
				(ADC_Channel) AC_VOLTAGE_L1_L2_DSP,   //LLV_FBK_MCU1,
				(ADC_Channel) INLET_LIQUID_TEMP_DSP,  //LLV_FBK_MCU3,
				(ADC_Channel) AN_IN_AUX_1_DSP,        //DCLI_FBK_MCU2,
				6,
				(ADC_SOCNumber) (SOC_AN_IN_AUX_1_DSP));
	/* ADC_C */
	ADC_SOCInit(ADCC_BASE, ADC_TRIGGER_EPWM1_SOCA,
				(ADC_Channel) I_FBK_DSP7,             //I_FBK_MCU3,
				(ADC_Channel) I_FBK_DSP8,             //I_FBK_MCU6,
				(ADC_Channel) I_FBK_DSP9,             //I_FBK_MCU9,
				(ADC_Channel) AIR_TEMP_SECTION_1_DSP, //LLV_FBK_MCU5,
				(ADC_Channel) AIR_TEMP_SECTION_2_DSP, //LLV_FBK_MCU6,
				(ADC_Channel) AN_IN_AUX_2_DSP,        //DCLI_FBK_MCU3,
				6,
				(ADC_SOCNumber) (SOC_AN_IN_AUX_2_DSP));
	/* ADC_D */
	ADC_SOCInit(ADCD_BASE, ADC_TRIGGER_EPWM1_SOCA,
				(ADC_Channel) PV_VOLTAGE_DSP,         //DCLV_FBK_MCU1,
				(ADC_Channel) INLET_LIQUID_PRESS_DSP, //DCLV_FBK_MCU2,
				(ADC_Channel) OUTLET_LIQUID_PRESS_DSP,//DCLV_FBK_MCU3,
				(ADC_Channel) DC_LINK_DSP,            //DCLV_FBK_MCU4,
				(ADC_Channel) AC_VOLTAGE_L2_L3_DSP,   //LLV_FBK_MCU2,
				(ADC_Channel) OUTLET_LIQUID_TEMP_DSP, //LLV_FBK_MCU4,
				6,
				(ADC_SOCNumber) (SOC_OUTLET_LIQUID_TEMP_DSP));

#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG
    ADC_InitTriggerEPWM(EPWM1_BASE);
    ADC_SOCInit(ADCA_BASE, ADC_TRIGGER_EPWM1_SOCA,
                (ADC_Channel)I_FBK_MCU1,
                (ADC_Channel)I_FBK_MCU4,
                (ADC_Channel)I_FBK_MCU7,
                (ADC_Channel)DACOUTA,
                (ADC_Channel)DACOUTB,
                (ADC_Channel)DCLI_FBK_MCU1,
                6,
                (ADC_SOCNumber)(SOC_DCLI_FBK_MCU1));

    ADC_SOCInit(ADCB_BASE, ADC_TRIGGER_EPWM1_SOCA,
                (ADC_Channel)I_FBK_MCU2,
                (ADC_Channel)I_FBK_MCU5,
                (ADC_Channel)I_FBK_MCU8,
                (ADC_Channel)LLV_FBK_MCU1,
                (ADC_Channel)LLV_FBK_MCU3,
                (ADC_Channel)DCLI_FBK_MCU2,
                6,
                (ADC_SOCNumber)SOC_DCLI_FBK_MCU2);

#ifdef ADCC_BASE
    ADC_SOCInit(ADCC_BASE, ADC_TRIGGER_EPWM1_SOCA,
                (ADC_Channel)I_FBK_MCU3,
                (ADC_Channel)I_FBK_MCU6,
                (ADC_Channel)I_FBK_MCU9,
                (ADC_Channel)LLV_FBK_MCU5,
                (ADC_Channel)LLV_FBK_MCU6,
                (ADC_Channel)DCLI_FBK_MCU3,
                6,
                (ADC_SOCNumber)SOC_DCLI_FBK_MCU3);
#endif
    ADC_SOCInit(ADCD_BASE, ADC_TRIGGER_EPWM1_SOCA,
                (ADC_Channel)DCLV_FBK_MCU1,
                (ADC_Channel)DCLV_FBK_MCU2,
                (ADC_Channel)DCLV_FBK_MCU3,
                (ADC_Channel)DCLV_FBK_MCU4,
                (ADC_Channel)LLV_FBK_MCU2,
                (ADC_Channel)LLV_FBK_MCU4,
                6,
                (ADC_SOCNumber)SOC_LLV_FBK_MCU4);

#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_SMALL_TEST

    ADC_InitTriggerEPWM(EPWM6_BASE);

    ADC_SOCInit(ADCA_BASE, ADC_TRIGGER_EPWM6_SOCA,
                (ADC_Channel)I_U_ADC,
                (ADC_Channel)VAC_UV_ADC,
                (ADC_Channel)U_DC_FB_ADC,
                (ADC_Channel)I_U_OUT_ADC,
                (ADC_Channel)I_W_OUT_TRIP,
                (ADC_Channel)0xFFFF,
                5,
                (ADC_SOCNumber)(I_W_OUT_TRIP_SOC));
    ADC_SOCInit(ADCB_BASE, ADC_TRIGGER_EPWM6_SOCA,
                (ADC_Channel)I_W_ADC,
                (ADC_Channel)VAC_VW_ADC,
                (ADC_Channel)I_U_TRIP,
                (ADC_Channel)I_V_OUT_ADC,
                (ADC_Channel)0xFFFF,
                (ADC_Channel)0xFFFF,
                4,
                (ADC_SOCNumber)I_V_OUT_ADC_SOC);
    ADC_SOCInit(ADCD_BASE, ADC_TRIGGER_EPWM6_SOCA,
                (ADC_Channel)I_W_TRIP,
                (ADC_Channel)I_W_OUT_ADC,
                (ADC_Channel)I_U_OUT_TRIP,
                (ADC_Channel)0xFFFF,
                (ADC_Channel)0xFFFF,
                (ADC_Channel)0xFFFF,
                3,
                (ADC_SOCNumber)I_U_OUT_TRIP_SOC);

#endif
    ADC_vResetAquisitionTimeAllADC();

    // Initialize results buffer
    for(index = 0; index < ADC_RESULTS_ARRAY_LENGTH; index++) {
        adcAResults[index] = 0;
        adcBResults[index] = 0;
#ifdef ADCC_BASE
        adcCResults[index] = 0;
#endif
        adcDResults[index] = 0;
    }
}



uint16_t ADC_AquisitionTimeOld = 0;    //Resolution 5ns (200 MHz)
#if (BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC)
uint16_t ADC_AquisitionTime = 150;      //Resolution 5ns (200 MHz)
#else
uint16_t ADC_AquisitionTime = 75;      //Resolution 5ns (200 MHz)
#endif

void ADC_vResetAquisitionTime (uint32_t base)
{
    uint16_t acqps;
    acqps = ADC_AquisitionTime;

#if (BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG) || (BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC)
    volatile struct ADC_REGS * psHandle = (volatile struct ADC_REGS*)base;
    EALLOW;
    psHandle->ADCSOC0CTL.bit.ACQPS = acqps; //sample window cycles
    psHandle->ADCSOC1CTL.bit.ACQPS = acqps; //sample window cycles
    psHandle->ADCSOC2CTL.bit.ACQPS = acqps; //sample window cycles
    psHandle->ADCSOC3CTL.bit.ACQPS = acqps; //sample window cycles
    psHandle->ADCSOC4CTL.bit.ACQPS = acqps; //sample window cycles
    psHandle->ADCSOC5CTL.bit.ACQPS = acqps; //sample window cycles
    EDIS;

#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_SMALL_TEST
    uint32_t ctlRegAddr;
    uint16_t socNumber;

    for (socNumber = 0; socNumber < 6; socNumber++ )
    {
        //
        // Calculate address
        //
        ctlRegAddr = base + ADC_SOCxCTL_OFFSET_BASE + ((uint32_t)socNumber * 2U);

        //
        // set value
        //
        EALLOW;
        HWREGH(ctlRegAddr) = (HWREGH(ctlRegAddr) & (~(ADC_SOC0CTL_ACQPS_M))) | acqps;
        EDIS;
    }
#endif
}


void ADC_vResetAquisitionTimeAllADC (void)
{
    if (ADC_AquisitionTime != ADC_AquisitionTimeOld) {
        ADC_AquisitionTimeOld = ADC_AquisitionTime;

        ADC_vResetAquisitionTime(ADCA_BASE);
        ADC_vResetAquisitionTime(ADCB_BASE);
#ifdef ADCC_BASE
        ADC_vResetAquisitionTime(ADCC_BASE);
#endif
        ADC_vResetAquisitionTime(ADCD_BASE);
    }
}



void ADC_vOffsetSetup(void)
{
#if BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC
    ADC_setPPBCalibrationOffset(ADCA_BASE, ADC_PPB_NUMBER1, Offset_Iph_FBK_DSP1  ); //Iph_FBK_MCU1
    ADC_setPPBCalibrationOffset(ADCA_BASE, ADC_PPB_NUMBER2, Offset_Iph_FBK_DSP2  ); //Iph_FBK_MCU4
    ADC_setPPBCalibrationOffset(ADCA_BASE, ADC_PPB_NUMBER3, Offset_Iph_FBK_DSP3  ); //Iph_FBK_MCU7
    ADC_setPPBCalibrationOffset(ADCA_BASE, ADC_PPB_NUMBER4, Offset_PV_Current_DSP); //DCLI_FBK_MCU1

    ADC_setPPBCalibrationOffset(ADCB_BASE, ADC_PPB_NUMBER1, Offset_Iph_FBK_DSP4  ); //Iph_FBK_MCU2
    ADC_setPPBCalibrationOffset(ADCB_BASE, ADC_PPB_NUMBER2, Offset_Iph_FBK_DSP5  ); //Iph_FBK_MCU5
    ADC_setPPBCalibrationOffset(ADCB_BASE, ADC_PPB_NUMBER3, Offset_Iph_FBK_DSP6  ); //Iph_FBK_MCU8
    ADC_setPPBCalibrationOffset(ADCB_BASE, ADC_PPB_NUMBER4, Offset_AN_IN_Aux1_DSP); //DCLI_FBK_MCU2

#ifdef ADCC_BASE
    ADC_setPPBCalibrationOffset(ADCC_BASE, ADC_PPB_NUMBER1, Offset_Iph_FBK_DSP7  ); //Iph_FBK_MCU3
    ADC_setPPBCalibrationOffset(ADCC_BASE, ADC_PPB_NUMBER2, Offset_Iph_FBK_DSP8  ); //Iph_FBK_MCU6
    ADC_setPPBCalibrationOffset(ADCC_BASE, ADC_PPB_NUMBER3, Offset_Iph_FBK_DSP9  ); //Iph_FBK_MCU9
    ADC_setPPBCalibrationOffset(ADCC_BASE, ADC_PPB_NUMBER4, Offset_AN_IN_Aux2_DSP); //DCLI_FBK_MCU3
#endif

    ADC_setPPBCalibrationOffset(ADCD_BASE, ADC_PPB_NUMBER1, Offset_PV_Voltage_DSP            ); //DCLV_FBK_MCU1
    ADC_setPPBCalibrationOffset(ADCD_BASE, ADC_PPB_NUMBER2, Offset_Inlet_Liquid_Pressure_DSP ); //DCLV_FBK_MCU2
    ADC_setPPBCalibrationOffset(ADCD_BASE, ADC_PPB_NUMBER3, Offset_Outlet_Liquid_Pressure_DSP); //DCLV_FBK_MCU3
    ADC_setPPBCalibrationOffset(ADCD_BASE, ADC_PPB_NUMBER4, Offset_DC_Link_DSP               ); //DCLV_FBK_MCU4

#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG
    ADC_setPPBCalibrationOffset(ADCA_BASE, ADC_PPB_NUMBER1, Offset_Iph_FBK_MCU1 ); //Iph_FBK_MCU1
    ADC_setPPBCalibrationOffset(ADCA_BASE, ADC_PPB_NUMBER2, Offset_Iph_FBK_MCU4 ); //Iph_FBK_MCU4
    ADC_setPPBCalibrationOffset(ADCA_BASE, ADC_PPB_NUMBER3, Offset_Iph_FBK_MCU7 ); //Iph_FBK_MCU7
    ADC_setPPBCalibrationOffset(ADCA_BASE, ADC_PPB_NUMBER4, Offset_DCLI_FBK_MCU1); //DCLI_FBK_MCU1

    ADC_setPPBCalibrationOffset(ADCB_BASE, ADC_PPB_NUMBER1, Offset_Iph_FBK_MCU2 ); //Iph_FBK_MCU2
    ADC_setPPBCalibrationOffset(ADCB_BASE, ADC_PPB_NUMBER2, Offset_Iph_FBK_MCU5 ); //Iph_FBK_MCU5
    ADC_setPPBCalibrationOffset(ADCB_BASE, ADC_PPB_NUMBER3, Offset_Iph_FBK_MCU8 ); //Iph_FBK_MCU8
    ADC_setPPBCalibrationOffset(ADCB_BASE, ADC_PPB_NUMBER4, Offset_DCLI_FBK_MCU2); //DCLI_FBK_MCU2

#ifdef ADCC_BASE
    ADC_setPPBCalibrationOffset(ADCC_BASE, ADC_PPB_NUMBER1, Offset_Iph_FBK_MCU3 ); //Iph_FBK_MCU3
    ADC_setPPBCalibrationOffset(ADCC_BASE, ADC_PPB_NUMBER2, Offset_Iph_FBK_MCU6 ); //Iph_FBK_MCU6
    ADC_setPPBCalibrationOffset(ADCC_BASE, ADC_PPB_NUMBER3, Offset_Iph_FBK_MCU9 ); //Iph_FBK_MCU9
    ADC_setPPBCalibrationOffset(ADCC_BASE, ADC_PPB_NUMBER4, Offset_DCLI_FBK_MCU3); //DCLI_FBK_MCU3
#endif

    ADC_setPPBCalibrationOffset(ADCD_BASE, ADC_PPB_NUMBER1, Offset_DCLV_FBK_MCU1); //DCLV_FBK_MCU1
    ADC_setPPBCalibrationOffset(ADCD_BASE, ADC_PPB_NUMBER2, Offset_DCLV_FBK_MCU2); //DCLV_FBK_MCU2
    ADC_setPPBCalibrationOffset(ADCD_BASE, ADC_PPB_NUMBER3, Offset_DCLV_FBK_MCU3); //DCLV_FBK_MCU3
    ADC_setPPBCalibrationOffset(ADCD_BASE, ADC_PPB_NUMBER4, Offset_DCLV_FBK_MCU4); //DCLV_FBK_MCU4
#endif
}


void ADC_vReferenceOffsetSetup(void)
{
#if BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC
    ADC_setPPBReferenceOffset(ADCA_BASE, ADC_PPB_NUMBER1, ReferenceOffset_Iph_FBK_DSP1  ); //Iph_FBK_MCU1
    ADC_setPPBReferenceOffset(ADCA_BASE, ADC_PPB_NUMBER2, ReferenceOffset_Iph_FBK_DSP2  ); //Iph_FBK_MCU4
    ADC_setPPBReferenceOffset(ADCA_BASE, ADC_PPB_NUMBER3, ReferenceOffset_Iph_FBK_DSP3  ); //Iph_FBK_MCU7
    ADC_setPPBReferenceOffset(ADCA_BASE, ADC_PPB_NUMBER4, ReferenceOffset_PV_Current_DSP); //DCLI_FBK_MCU1

    ADC_setPPBReferenceOffset(ADCB_BASE, ADC_PPB_NUMBER1, ReferenceOffset_Iph_FBK_DSP4  ); //Iph_FBK_MCU2
    ADC_setPPBReferenceOffset(ADCB_BASE, ADC_PPB_NUMBER2, ReferenceOffset_Iph_FBK_DSP5  ); //Iph_FBK_MCU5
    ADC_setPPBReferenceOffset(ADCB_BASE, ADC_PPB_NUMBER3, ReferenceOffset_Iph_FBK_DSP6  ); //Iph_FBK_MCU8
    ADC_setPPBReferenceOffset(ADCB_BASE, ADC_PPB_NUMBER4, ReferenceOffset_AN_IN_Aux1_DSP); //DCLI_FBK_MCU2

#ifdef ADCC_BASE
    ADC_setPPBReferenceOffset(ADCC_BASE, ADC_PPB_NUMBER1, ReferenceOffset_Iph_FBK_DSP7  ); //Iph_FBK_MCU3
    ADC_setPPBReferenceOffset(ADCC_BASE, ADC_PPB_NUMBER2, ReferenceOffset_Iph_FBK_DSP8  ); //Iph_FBK_MCU6
    ADC_setPPBReferenceOffset(ADCC_BASE, ADC_PPB_NUMBER3, ReferenceOffset_Iph_FBK_DSP9  ); //Iph_FBK_MCU9
    ADC_setPPBReferenceOffset(ADCC_BASE, ADC_PPB_NUMBER4, ReferenceOffset_AN_IN_Aux2_DSP); //DCLI_FBK_MCU3
#endif

    ADC_setPPBReferenceOffset(ADCD_BASE, ADC_PPB_NUMBER1, ReferenceOffset_PV_Voltage_DSP            ); //DCLV_FBK_MCU1
    ADC_setPPBReferenceOffset(ADCD_BASE, ADC_PPB_NUMBER2, ReferenceOffset_Inlet_Liquid_Pressure_DSP ); //DCLV_FBK_MCU2
    ADC_setPPBReferenceOffset(ADCD_BASE, ADC_PPB_NUMBER3, ReferenceOffset_Outlet_Liquid_Pressure_DSP); //DCLV_FBK_MCU3
    ADC_setPPBReferenceOffset(ADCD_BASE, ADC_PPB_NUMBER4, ReferenceOffset_DC_Link_DSP               ); //DCLV_FBK_MCU4


#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG
    ADC_setPPBReferenceOffset(ADCA_BASE, ADC_PPB_NUMBER1, ReferenceOffset_Iph_FBK_MCU1 ); //Iph_FBK_MCU1
    ADC_setPPBReferenceOffset(ADCA_BASE, ADC_PPB_NUMBER2, ReferenceOffset_Iph_FBK_MCU4 ); //Iph_FBK_MCU4
    ADC_setPPBReferenceOffset(ADCA_BASE, ADC_PPB_NUMBER3, ReferenceOffset_Iph_FBK_MCU7 ); //Iph_FBK_MCU7
    ADC_setPPBReferenceOffset(ADCA_BASE, ADC_PPB_NUMBER4, ReferenceOffset_DCLI_FBK_MCU1); //DCLI_FBK_MCU1

    ADC_setPPBReferenceOffset(ADCB_BASE, ADC_PPB_NUMBER1, ReferenceOffset_Iph_FBK_MCU2 ); //Iph_FBK_MCU2
    ADC_setPPBReferenceOffset(ADCB_BASE, ADC_PPB_NUMBER2, ReferenceOffset_Iph_FBK_MCU5 ); //Iph_FBK_MCU5
    ADC_setPPBReferenceOffset(ADCB_BASE, ADC_PPB_NUMBER3, ReferenceOffset_Iph_FBK_MCU8 ); //Iph_FBK_MCU8
    ADC_setPPBReferenceOffset(ADCB_BASE, ADC_PPB_NUMBER4, ReferenceOffset_DCLI_FBK_MCU2); //DCLI_FBK_MCU2

#ifdef ADCC_BASE
    ADC_setPPBReferenceOffset(ADCC_BASE, ADC_PPB_NUMBER1, ReferenceOffset_Iph_FBK_MCU3 ); //Iph_FBK_MCU3
    ADC_setPPBReferenceOffset(ADCC_BASE, ADC_PPB_NUMBER2, ReferenceOffset_Iph_FBK_MCU6 ); //Iph_FBK_MCU6
    ADC_setPPBReferenceOffset(ADCC_BASE, ADC_PPB_NUMBER3, ReferenceOffset_Iph_FBK_MCU9 ); //Iph_FBK_MCU9
    ADC_setPPBReferenceOffset(ADCC_BASE, ADC_PPB_NUMBER4, ReferenceOffset_DCLI_FBK_MCU3); //DCLI_FBK_MCU3
#endif

    ADC_setPPBReferenceOffset(ADCD_BASE, ADC_PPB_NUMBER1, ReferenceOffset_DCLV_FBK_MCU1); //DCLV_FBK_MCU1
    ADC_setPPBReferenceOffset(ADCD_BASE, ADC_PPB_NUMBER2, ReferenceOffset_DCLV_FBK_MCU2); //DCLV_FBK_MCU2
    ADC_setPPBReferenceOffset(ADCD_BASE, ADC_PPB_NUMBER3, ReferenceOffset_DCLV_FBK_MCU3); //DCLV_FBK_MCU3
    ADC_setPPBReferenceOffset(ADCD_BASE, ADC_PPB_NUMBER4, ReferenceOffset_DCLV_FBK_MCU4); //DCLV_FBK_MCU4
#endif
}


void ADC_setPPBTwoComplement(uint32_t base, ADC_PPBNumber ppbNumber, int16_t enable)
{
    if (enable) {
        ADC_enablePPBTwosComplement(base, ppbNumber);
    } else {
        ADC_disablePPBTwosComplement(base, ppbNumber);
    }
}


void ADC_vTwoComplementSetup(void)
{
#if BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC
    ADC_setPPBTwoComplement(ADCA_BASE, ADC_PPB_NUMBER1, TwoComplement_Iph_FBK_DSP1  ); //Iph_FBK_MCU1
    ADC_setPPBTwoComplement(ADCA_BASE, ADC_PPB_NUMBER2, TwoComplement_Iph_FBK_DSP2  ); //Iph_FBK_MCU4
    ADC_setPPBTwoComplement(ADCA_BASE, ADC_PPB_NUMBER3, TwoComplement_Iph_FBK_DSP3  ); //Iph_FBK_MCU7
    ADC_setPPBTwoComplement(ADCA_BASE, ADC_PPB_NUMBER4, TwoComplement_PV_Current_DSP); //DCLI_FBK_MCU1

    ADC_setPPBTwoComplement(ADCB_BASE, ADC_PPB_NUMBER1, TwoComplement_Iph_FBK_DSP4  ); //Iph_FBK_MCU2
    ADC_setPPBTwoComplement(ADCB_BASE, ADC_PPB_NUMBER2, TwoComplement_Iph_FBK_DSP5  ); //Iph_FBK_MCU5
    ADC_setPPBTwoComplement(ADCB_BASE, ADC_PPB_NUMBER3, TwoComplement_Iph_FBK_DSP6  ); //Iph_FBK_MCU8
    ADC_setPPBTwoComplement(ADCB_BASE, ADC_PPB_NUMBER4, TwoComplement_AN_IN_Aux1_DSP); //DCLI_FBK_MCU2

#ifdef ADCC_BASE
    ADC_setPPBTwoComplement(ADCC_BASE, ADC_PPB_NUMBER1, TwoComplement_Iph_FBK_DSP7  ); //Iph_FBK_MCU3
    ADC_setPPBTwoComplement(ADCC_BASE, ADC_PPB_NUMBER2, TwoComplement_Iph_FBK_DSP8  ); //Iph_FBK_MCU6
    ADC_setPPBTwoComplement(ADCC_BASE, ADC_PPB_NUMBER3, TwoComplement_Iph_FBK_DSP9  ); //Iph_FBK_MCU9
    ADC_setPPBTwoComplement(ADCC_BASE, ADC_PPB_NUMBER4, TwoComplement_AN_IN_Aux2_DSP); //DCLI_FBK_MCU3
#endif

    ADC_setPPBTwoComplement(ADCD_BASE, ADC_PPB_NUMBER1, TwoComplement_PV_Voltage_DSP            ); //DCLV_FBK_MCU1
    ADC_setPPBTwoComplement(ADCD_BASE, ADC_PPB_NUMBER2, TwoComplement_Inlet_Liquid_Pressure_DSP ); //DCLV_FBK_MCU2
    ADC_setPPBTwoComplement(ADCD_BASE, ADC_PPB_NUMBER3, TwoComplement_Outlet_Liquid_Pressure_DSP); //DCLV_FBK_MCU3
    ADC_setPPBTwoComplement(ADCD_BASE, ADC_PPB_NUMBER4, TwoComplement_DC_Link_DSP               ); //DCLV_FBK_MCU4


#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG
    ADC_setPPBTwoComplement(ADCA_BASE, ADC_PPB_NUMBER1, TwoComplement_Iph_FBK_MCU1 ); //Iph_FBK_MCU1
    ADC_setPPBTwoComplement(ADCA_BASE, ADC_PPB_NUMBER2, TwoComplement_Iph_FBK_MCU4 ); //Iph_FBK_MCU4
    ADC_setPPBTwoComplement(ADCA_BASE, ADC_PPB_NUMBER3, TwoComplement_Iph_FBK_MCU7 ); //Iph_FBK_MCU7
    ADC_setPPBTwoComplement(ADCA_BASE, ADC_PPB_NUMBER4, TwoComplement_DCLI_FBK_MCU1); //DCLI_FBK_MCU1

    ADC_setPPBTwoComplement(ADCB_BASE, ADC_PPB_NUMBER1, TwoComplement_Iph_FBK_MCU2 ); //Iph_FBK_MCU2
    ADC_setPPBTwoComplement(ADCB_BASE, ADC_PPB_NUMBER2, TwoComplement_Iph_FBK_MCU5 ); //Iph_FBK_MCU5
    ADC_setPPBTwoComplement(ADCB_BASE, ADC_PPB_NUMBER3, TwoComplement_Iph_FBK_MCU8 ); //Iph_FBK_MCU8
    ADC_setPPBTwoComplement(ADCB_BASE, ADC_PPB_NUMBER4, TwoComplement_DCLI_FBK_MCU2); //DCLI_FBK_MCU2

#ifdef ADCC_BASE
    ADC_setPPBTwoComplement(ADCC_BASE, ADC_PPB_NUMBER1, TwoComplement_Iph_FBK_MCU3 ); //Iph_FBK_MCU3
    ADC_setPPBTwoComplement(ADCC_BASE, ADC_PPB_NUMBER2, TwoComplement_Iph_FBK_MCU6 ); //Iph_FBK_MCU6
    ADC_setPPBTwoComplement(ADCC_BASE, ADC_PPB_NUMBER3, TwoComplement_Iph_FBK_MCU9 ); //Iph_FBK_MCU9
    ADC_setPPBTwoComplement(ADCC_BASE, ADC_PPB_NUMBER4, TwoComplement_DCLI_FBK_MCU3); //DCLI_FBK_MCU3
#endif

    ADC_setPPBTwoComplement(ADCD_BASE, ADC_PPB_NUMBER1, TwoComplement_DCLV_FBK_MCU1); //DCLV_FBK_MCU1
    ADC_setPPBTwoComplement(ADCD_BASE, ADC_PPB_NUMBER2, TwoComplement_DCLV_FBK_MCU2); //DCLV_FBK_MCU2
    ADC_setPPBTwoComplement(ADCD_BASE, ADC_PPB_NUMBER3, TwoComplement_DCLV_FBK_MCU3); //DCLV_FBK_MCU3
    ADC_setPPBTwoComplement(ADCD_BASE, ADC_PPB_NUMBER4, TwoComplement_DCLV_FBK_MCU4); //DCLV_FBK_MCU4
#endif
}





uint16_t ADCANewData = 0;
uint16_t ADCBNewData = 0;
#ifdef ADCC_BASE
uint16_t ADCCNewData = 0;
#endif
uint16_t ADCDNewData = 0;

volatile uint16_t ADCANewDataResetCounter = 0;


void ADC_NewDataReset (void)
{
    if ((ADCANewData)
            || (ADCBNewData)
#ifdef ADCC_BASE
            || (ADCCNewData)
#endif
            || (ADCDNewData))
    {
        ADCANewDataResetCounter++;
    }
    ADCANewData = 0;
    ADCBNewData = 0;
#ifdef ADCC_BASE
    ADCCNewData = 0;
#endif
    ADCDNewData = 0;
}



bool bDACSetDACOUTMeasure = 1;

uint16_t ADC_NewData (void)
{
    uint16_t result = 0;
    if ((ADCANewData)
            && (ADCBNewData)
#ifdef ADCC_BASE
            && (ADCCNewData)
#endif
            && (ADCDNewData))
    {
    	/* ADC_A */
#if BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC
    	u16Adc_DAC_outA        = adcAResults[SOC_DAC_OUT_A];
        u16Adc_DAC_outB        = adcAResults[SOC_DAC_OUT_B];
        u16Adc_Iph_FBK_DSP1    = adcAResults[SOC_I_FBK_DSP1];
        u16Adc_Iph_FBK_DSP2    = adcAResults[SOC_I_FBK_DSP2];
        u16Adc_Iph_FBK_DSP3    = adcAResults[SOC_I_FBK_DSP3];
        u16Adc_PV_Current_DSP  = adcAResults[SOC_PV_CURRENT_DSP];
#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG
        u16Adc_DACOUTA         = adcAResults[SOC_DACOUTA];
        u16Adc_DACOUTB         = adcAResults[SOC_DACOUTB];
        u16Adc_Iph_FBK_MCU1    = adcAResults[SOC_I_FBK_MCU1];
        u16Adc_Iph_FBK_MCU4    = adcAResults[SOC_I_FBK_MCU4];
        u16Adc_Iph_FBK_MCU7    = adcAResults[SOC_I_FBK_MCU7];
        u16Adc_DCLI_FBK_MCU1   = adcAResults[SOC_DCLI_FBK_MCU1];
#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_SMALL_TEST
        u16Value_I_U_ADC       = adcAResults[I_U_ADC_SOC];
        u16Value_VAC_UV_ADC    = adcAResults[VAC_UV_ADC_SOC];
        u16Value_U_DC_FB_ADC   = adcAResults[U_DC_FB_ADC_SOC];
        u16Value_I_U_OUT_ADC   = adcAResults[I_U_OUT_ADC_SOC];
        u16Value_I_W_OUT_TRIP  = adcAResults[I_W_OUT_TRIP_SOC];
#endif

    	/* ADC_B */
#if BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC
        u16Adc_AC_Voltage_L1_L2_DSP         = adcBResults[SOC_AC_VOLTAGE_L1_L2_DSP];
        u16Adc_Inlet_Liquid_Temperature_DSP = adcBResults[SOC_INLET_LIQUID_TEMP_DSP];
        u16Adc_Iph_FBK_DSP4                 = adcBResults[SOC_I_FBK_DSP4];
        u16Adc_Iph_FBK_DSP5                 = adcBResults[SOC_I_FBK_DSP5];
        u16Adc_Iph_FBK_DSP6                 = adcBResults[SOC_I_FBK_DSP6];
        u16Adc_AN_IN_Aux1_DSP               = adcBResults[SOC_AN_IN_AUX_1_DSP];
#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG
        u16Adc_LLV_FBK_MCU1    = adcBResults[SOC_LLV_FBK_MCU1];
        u16Adc_LLV_FBK_MCU3    = adcBResults[SOC_LLV_FBK_MCU3];
        u16Adc_Iph_FBK_MCU2    = adcBResults[SOC_I_FBK_MCU2];
        u16Adc_Iph_FBK_MCU5    = adcBResults[SOC_I_FBK_MCU5];
        u16Adc_Iph_FBK_MCU8    = adcBResults[SOC_I_FBK_MCU8];
        u16Adc_DCLI_FBK_MCU2   = adcBResults[SOC_DCLI_FBK_MCU2];
#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_SMALL_TEST
        u16Value_I_W_ADC       = adcBResults[I_W_ADC_SOC];
        u16Value_VAC_VW_ADC    = adcBResults[VAC_VW_ADC_SOC];
        u16Value_I_U_TRIP      = adcBResults[I_U_TRIP_SOC];
        u16Value_I_V_OUT_ADC   = adcBResults[I_V_OUT_ADC_SOC];
#endif

    	/* ADC_C */
#ifdef ADCC_BASE
	#if BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC
			u16Adc_Air_Temperature_Section_1_DSP = adcCResults[SOC_AIR_TEMP_SECTION_1_DSP];
			u16Adc_Air_Temperature_Section_2_DSP = adcCResults[SOC_AIR_TEMP_SECTION_2_DSP];
			u16Adc_Iph_FBK_DSP7                  = adcCResults[SOC_I_FBK_DSP7];
			u16Adc_Iph_FBK_DSP8                  = adcCResults[SOC_I_FBK_DSP8];
			u16Adc_Iph_FBK_DSP9                  = adcCResults[SOC_I_FBK_DSP9];
			u16Adc_AN_IN_Aux2_DSP                = adcCResults[SOC_AN_IN_AUX_2_DSP];
	#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG
			u16Adc_LLV_FBK_MCU5    = adcCResults[SOC_LLV_FBK_MCU5];
			u16Adc_LLV_FBK_MCU6    = adcCResults[SOC_LLV_FBK_MCU6];
			u16Adc_Iph_FBK_MCU3    = adcCResults[SOC_I_FBK_MCU3];
			u16Adc_Iph_FBK_MCU6    = adcCResults[SOC_I_FBK_MCU6];
			u16Adc_Iph_FBK_MCU9    = adcCResults[SOC_I_FBK_MCU9];
			u16Adc_DCLI_FBK_MCU3   = adcCResults[SOC_DCLI_FBK_MCU3];
	#endif
#endif

		/* ADC_D */
#if BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC
		u16Adc_PV_Voltage_DSP                = adcDResults[SOC_PV_VOLTAGE_DSP];
		u16Adc_Inlet_Liquid_Pressure_DSP     = adcDResults[SOC_INLET_LIQUID_PRESS_DSP];
		u16Adc_Outlet_Liquid_Pressure_DSP    = adcDResults[SOC_OUTLET_LIQUID_PRESS_DSP];
		u16Adc_DC_Link_DSP                   = adcDResults[SOC_DC_LINK_DSP];
		u16Adc_AC_Voltage_L2_L3_DSP          = adcDResults[SOC_AC_VOLTAGE_L2_L3_DSP];
		u16Adc_Outlet_Liquid_Temperature_DSP = adcDResults[SOC_OUTLET_LIQUID_TEMP_DSP];
#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG
        u16Adc_DCLV_FBK_MCU1   = adcDResults[ADC_SOC_NUMBER0];
        u16Adc_DCLV_FBK_MCU2   = adcDResults[ADC_SOC_NUMBER1];
        u16Adc_DCLV_FBK_MCU3   = adcDResults[ADC_SOC_NUMBER2];
        u16Adc_DCLV_FBK_MCU4   = adcDResults[ADC_SOC_NUMBER3];
        u16Adc_LLV_FBK_MCU2    = adcDResults[ADC_SOC_NUMBER4];
        u16Adc_LLV_FBK_MCU4    = adcDResults[ADC_SOC_NUMBER5];
#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_SMALL_TEST
        u16Value_I_W_TRIP      = adcDResults[I_W_TRIP_SOC];
        u16Value_I_W_OUT_ADC   = adcDResults[I_W_OUT_ADC_SOC];
        u16Value_I_U_OUT_TRIP  = adcDResults[I_U_OUT_TRIP_SOC];
#endif

        ADCANewData = 0;
        ADCBNewData = 0;
#ifdef ADCC_BASE
        ADCCNewData = 0;
#endif
        ADCDNewData = 0;
        result = 1;

#if BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC

        if (ADC_AquisitionTime != ADC_AquisitionTimeOld) {
        	ADC_AquisitionTimeOld = ADC_AquisitionTime;

        #if BOARDCFG_USE_DAC_AUTOMATION
			if (DAC_bADCFeedbackMeasureNeeded())
			{
				bDACSetDACOUTMeasure = 1;
				ADC_SOCInit(ADCA_BASE, ADC_TRIGGER_EPWM1_SOCA,
							(ADC_Channel) I_FBK_DSP1,
							(ADC_Channel) I_FBK_DSP2,
							(ADC_Channel) I_FBK_DSP3,
							(ADC_Channel) DAC_OUT_A,
							(ADC_Channel) DAC_OUT_B,
							(ADC_Channel) PV_CURRENT_DSP,
							6,
							(ADC_SOCNumber) (SOC_PV_CURRENT_DSP) );
			} else {
				bDACSetDACOUTMeasure = 0;
				ADC_SOCInit(ADCA_BASE, ADC_TRIGGER_EPWM1_SOCA,
							(ADC_Channel) I_FBK_DSP1,
							(ADC_Channel) I_FBK_DSP2,
							(ADC_Channel) I_FBK_DSP3,
							(ADC_Channel) I_FBK_DSP1,
							(ADC_Channel) I_FBK_DSP1,
							(ADC_Channel) PV_CURRENT_DSP,
							6,
							(ADC_SOCNumber) (SOC_PV_CURRENT_DSP) );
			}
        #endif
        }
#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG

        #if BOARDCFG_USE_DAC_AUTOMATION
        if (DAC_bADCFeedbackMeasureNeeded())
        {
            bDACSetDACOUTMeasure = 1;
            ADC_SOCInit(ADCA_BASE, ADC_TRIGGER_EPWM1_SOCA,
                        (ADC_Channel)I_FBK_MCU1,
                        (ADC_Channel)I_FBK_MCU4,
                        (ADC_Channel)I_FBK_MCU7,
                        (ADC_Channel)DACOUTA,
                        (ADC_Channel)DACOUTB,
                        (ADC_Channel)DCLI_FBK_MCU1,
                        6,
                        (ADC_SOCNumber)(SOC_DCLI_FBK_MCU1));
        }
        else
        {
            bDACSetDACOUTMeasure = 0;
            ADC_SOCInit(ADCA_BASE, ADC_TRIGGER_EPWM1_SOCA,
                        (ADC_Channel)I_FBK_MCU1,
                        (ADC_Channel)I_FBK_MCU4,
                        (ADC_Channel)I_FBK_MCU7,
                        (ADC_Channel)I_FBK_MCU1,
                        (ADC_Channel)I_FBK_MCU1,
                        (ADC_Channel)DCLI_FBK_MCU1,
                        6,
                        (ADC_SOCNumber)(SOC_DCLI_FBK_MCU1));
        }
        #endif
#endif
    }
    return result;
}



void ADC_vSetFunctionADCDataProcess (pfVoid_t pFunc)
{
    pfADCDataProcess = pFunc;
}



//
// ADC A Interrupt 1 ISR
//
__interrupt void ADCA_ISR1(void)
{
    EINT;	// Clear INTM to enable interrupts
    ADCANewData = 1;

    adcAResults[0] = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);
    adcAResults[1] = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER1);
    adcAResults[2] = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER2);
    adcAResults[3] = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER3);
    adcAResults[4] = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER4);
    adcAResults[5] = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER5);

    adcAResults[0] = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);
    adcAResults[1] = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER1);
    adcAResults[2] = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER2);
    adcAResults[3] = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER3);
    adcAResults[4] = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER4);
    adcAResults[5] = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER5);

    // Clear the interrupt flag
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER);

    // Check if overflow has occurred
    if(true == ADC_getInterruptOverflowStatus(ADCA_BASE, ADC_INT_NUMBER)) {
        ADC_clearInterruptOverflowStatus(ADCA_BASE, ADC_INT_NUMBER);
        ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER);
    }

    if (pfADCDataProcess != NULL) {
        pfADCDataProcess();
    }

    DINT;	// Disable interrupts

    // Acknowledge the interrupt
    Interrupt_clearACKGroup(ADC_INTERRUPT_ACK_GROUP);
}


//
// ADC B Interrupt 1 ISR
//
__interrupt void ADCB_ISR1(void)
{
    EINT;	// Clear INTM to enable interrupts
    ADCBNewData = 1;

    adcBResults[0] = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER0);
    adcBResults[1] = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER1);
    adcBResults[2] = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER2);
    adcBResults[3] = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER3);
    adcBResults[4] = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER4);
    adcBResults[5] = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER5);

    adcBResults[0] = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER0);
    adcBResults[1] = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER1);
    adcBResults[2] = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER2);
    adcBResults[3] = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER3);
    adcBResults[4] = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER4);
    adcBResults[5] = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER5);

    // Clear the interrupt flag
    ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER);

    // Check if overflow has occurred
    if (true == ADC_getInterruptOverflowStatus(ADCB_BASE, ADC_INT_NUMBER)) {
        ADC_clearInterruptOverflowStatus(ADCB_BASE, ADC_INT_NUMBER);
        ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER);
    }

    if (pfADCDataProcess != NULL) {
        pfADCDataProcess();
    }

    DINT;	// Disable interrupts

    // Acknowledge the interrupt
    Interrupt_clearACKGroup(ADC_INTERRUPT_ACK_GROUP);
}


#ifdef ADCC_BASE
//
// ADC C Interrupt 1 ISR
//
__interrupt void ADCC_ISR1(void)
{
    EINT;	// Clear INTM to enable interrupts
    ADCCNewData = 1;

    adcCResults[0] = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER0);
    adcCResults[1] = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER1);
    adcCResults[2] = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER2);
    adcCResults[3] = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER3);
    adcCResults[4] = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER4);
    adcCResults[5] = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER5);

    adcCResults[0] = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER0);
    adcCResults[1] = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER1);
    adcCResults[2] = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER2);
    adcCResults[3] = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER3);
    adcCResults[4] = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER4);
    adcCResults[5] = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER5);

    // Clear the interrupt flag
    ADC_clearInterruptStatus(ADCC_BASE, ADC_INT_NUMBER);

    // Check if overflow has occurred
    if (true == ADC_getInterruptOverflowStatus(ADCC_BASE, ADC_INT_NUMBER)) {
        ADC_clearInterruptOverflowStatus(ADCC_BASE, ADC_INT_NUMBER);
        ADC_clearInterruptStatus(ADCC_BASE, ADC_INT_NUMBER);
    }

    if (pfADCDataProcess != NULL) {
        pfADCDataProcess();
    }

    DINT; 	// Disable interrupts

    // Acknowledge the interrupt
    Interrupt_clearACKGroup(ADC_INTERRUPT_ACK_GROUP);
}
#endif


//
// ADC D Interrupt 1 ISR
//
__interrupt void ADCD_ISR1(void)
{
    EINT;	// Clear INTM to enable interrupts
    ADCDNewData = 1;

    adcDResults[0] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER0);
    adcDResults[1] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER1);
    adcDResults[2] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER2);
    adcDResults[3] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER3);
    adcDResults[4] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER4);
    adcDResults[5] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER5);

    adcDResults[0] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER0);
    adcDResults[1] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER1);
    adcDResults[2] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER2);
    adcDResults[3] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER3);
    adcDResults[4] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER4);
    adcDResults[5] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER5);

    // Clear the interrupt flag
    ADC_clearInterruptStatus(ADCD_BASE, ADC_INT_NUMBER);

    // Check if overflow has occurred
    if(true == ADC_getInterruptOverflowStatus(ADCD_BASE, ADC_INT_NUMBER)) {
        ADC_clearInterruptOverflowStatus(ADCD_BASE, ADC_INT_NUMBER);
        ADC_clearInterruptStatus(ADCD_BASE, ADC_INT_NUMBER);
    }

    if (pfADCDataProcess != NULL) {
        pfADCDataProcess();
    }

    DINT;	// Disable interrupts

    // Acknowledge the interrupt
    Interrupt_clearACKGroup(ADC_INTERRUPT_ACK_GROUP);
}





