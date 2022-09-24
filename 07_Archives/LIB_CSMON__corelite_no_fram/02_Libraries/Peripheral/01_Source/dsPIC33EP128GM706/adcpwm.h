/* *****************************************************************************
 * File:   adcpwm.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 02 11 11:18
 * 
 * Description: ADC and PWM Driver
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef ADCPWM_H
#define	ADCPWM_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdbool.h>
#include <stdint.h>

#include "boardcfg.h" 
#include "control_cfg.h" 
#include "mclib.h"



/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
/* Basic loop period in units of Tcy */
#define LOOPTIME1_HZ     2500                   /* PWM Frequency 2.5Khz */
#define LOOPTIME2_HZ     2500                   /* PWM Frequency 2.5Khz */



#define ADC_RESOLUTION       12
#define ADC_REFERENCE_VOLTAGE_VOLTS         (3.3F)




/* *****************************************************************************
 * Configuration Definitions Calculation Functions
 **************************************************************************** */
/* *****************************************************************************
 * Function Like for Library Consider
 **************************************************************************** */


#ifndef BOARDCFG_USE_MCC_ADC
#define RESOLUTION_NANOSECONDS  1000000000.0
#define RESOLUTION_MICROSECONDS 1000000.0
#define RESOLUTION_MILLISECONDS 1000.0

#define TimeToTicks(flInputTime, flBaseFreq_HZ, flInputResolition) (uint16_t)(((flInputTime / flInputResolition) * flBaseFreq_HZ) + 0.5)

#define nSecToTicks(flInputTime, flBaseFreq_HZ) TimeToTicks (flInputTime, flBaseFreq_HZ, RESOLUTION_NANOSECONDS)
#define uSecToTicks(flInputTime, flBaseFreq_HZ) TimeToTicks (flInputTime, flBaseFreq_HZ, RESOLUTION_MICROSECONDS)
#define mSecToTicks(flInputTime, flBaseFreq_HZ) TimeToTicks (flInputTime, flBaseFreq_HZ, RESOLUTION_MILLISECONDS)

#define CoefToTicks(flCoef, u16PeriodTicks) (uint16_t)(flCoef * u16PeriodTicks)
#define PWM_MIN_DUTY_TCY    CoefToTicks(PWM_MIN_DUTY_COEF,LOOPTIME1_TCY)
#define PWM_MAX_DUTY_TCY    CoefToTicks(PWM_MAX_DUTY_COEF,LOOPTIME1_TCY)
#endif

/* *****************************************************************************
 * Configuration Definitions Calculations
 **************************************************************************** */
/* Basic loop period in units of Tcy */
#define LOOPTIME1_TCY    (unsigned int)(FCY_HZ / LOOPTIME1_HZ)
#define LOOPTIME2_TCY    (unsigned int)(FCY_HZ / LOOPTIME2_HZ)




#define PWM_START_DUTY_TCY    CoefToTicks(PWM_START_DUTY_COEF,LOOPTIME1_TCY)

/* *****************************************************************************
 * Constants Definitions
 **************************************************************************** */

/* *****************************************************************************
 * ADC Channels
 **************************************************************************** */

#ifndef BOARDCFG_USE_MCC_ADC
/* ADC Channels used on PWM Primary Trigger L1L2L3_S1S2 */
#define ADC1_PWM1_CHANNELS  (uint16_t)((1 << L1_CURRENT_1) | (1 << DC_VOLTAGE_1))
#define ADC1_PWM2_CHANNELS  (uint16_t)((1 << L2_CURRENT_1) | (1 << DC_VOLTAGE_1))
#define ADC1_PWM3_CHANNELS  (uint16_t)((1 << L3_CURRENT_1) | (1 << DC_VOLTAGE_1))
#define ADC1_PWM4_CHANNELS  (uint16_t)((1 << L1_CURRENT_2) | (1 << DC_VOLTAGE_2))
#define ADC1_PWM5_CHANNELS  (uint16_t)((1 << L2_CURRENT_2) | (1 << DC_VOLTAGE_2))
#define ADC1_PWM6_CHANNELS  (uint16_t)((1 << L3_CURRENT_2) | (1 << DC_VOLTAGE_2)) 

#define ADC2_PWM1_CHANNELS  (uint16_t)((1 << L1_CURRFBK_1) | (1 << DC_VOLTAGE_3))
#define ADC2_PWM2_CHANNELS  (uint16_t)((1 << L2_CURRFBK_1) | (1 << DC_VOLTAGE_3))
#define ADC2_PWM3_CHANNELS  (uint16_t)((1 << L3_CURRFBK_1) | (1 << DC_VOLTAGE_3))
#define ADC2_PWM4_CHANNELS  (uint16_t)((1 << L1_TEMPFBK_1) | (1 << DC_VOLTAGE_4))
#define ADC2_PWM5_CHANNELS  (uint16_t)((1 << L2_TEMPFBK_1) | (1 << DC_VOLTAGE_4))
#define ADC2_PWM6_CHANNELS  (uint16_t)((1 << L3_TEMPFBK_1) | (1 << DC_VOLTAGE_4))



/* ADC Channels used on PWM Primary Trigger L1L2L3 */
#define ADC1_PWM1_CHANNELS_L1L2L3  (uint16_t)((1 << L1_CURRENT_1) | (1 << DC_VOLTAGE_1) | (1 << L1_CURRENT_2) | (1 << DC_VOLTAGE_2))
#define ADC1_PWM2_CHANNELS_L1L2L3  (uint16_t)((1 << L2_CURRENT_1) | (1 << DC_VOLTAGE_1) | (1 << L2_CURRENT_2) | (1 << DC_VOLTAGE_2))
#define ADC1_PWM3_CHANNELS_L1L2L3  (uint16_t)((1 << L3_CURRENT_1) | (1 << DC_VOLTAGE_1) | (1 << L2_CURRENT_2) | (1 << DC_VOLTAGE_2))
#define ADC1_PWM4_CHANNELS_L1L2L3  (uint16_t)((1 << L1_CURRENT_1) | (1 << DC_VOLTAGE_1) | (1 << L1_CURRENT_2) | (1 << DC_VOLTAGE_2))
#define ADC1_PWM5_CHANNELS_L1L2L3  (uint16_t)((1 << L2_CURRENT_1) | (1 << DC_VOLTAGE_1) | (1 << L2_CURRENT_2) | (1 << DC_VOLTAGE_2))
#define ADC1_PWM6_CHANNELS_L1L2L3  (uint16_t)((1 << L3_CURRENT_1) | (1 << DC_VOLTAGE_1) | (1 << L2_CURRENT_2) | (1 << DC_VOLTAGE_2))



#define ADC2_PWM1_CHANNELS_L1L2L3  (uint16_t)((1 << L1_CURRFBK_1) | (1 << DC_VOLTAGE_3) | (1 << L1_TEMPFBK_1) | (1 << DC_VOLTAGE_4))
#define ADC2_PWM2_CHANNELS_L1L2L3  (uint16_t)((1 << L2_CURRFBK_1) | (1 << DC_VOLTAGE_3) | (1 << L2_TEMPFBK_1) | (1 << DC_VOLTAGE_4))
#define ADC2_PWM3_CHANNELS_L1L2L3  (uint16_t)((1 << L3_CURRFBK_1) | (1 << DC_VOLTAGE_3) | (1 << L3_TEMPFBK_1) | (1 << DC_VOLTAGE_4))
#define ADC2_PWM4_CHANNELS_L1L2L3  (uint16_t)((1 << L1_CURRFBK_1) | (1 << DC_VOLTAGE_3) | (1 << L1_TEMPFBK_1) | (1 << DC_VOLTAGE_4))
#define ADC2_PWM5_CHANNELS_L1L2L3  (uint16_t)((1 << L2_CURRFBK_1) | (1 << DC_VOLTAGE_3) | (1 << L2_TEMPFBK_1) | (1 << DC_VOLTAGE_4))
#define ADC2_PWM6_CHANNELS_L1L2L3  (uint16_t)((1 << L3_CURRFBK_1) | (1 << DC_VOLTAGE_3) | (1 << L3_TEMPFBK_1) | (1 << DC_VOLTAGE_4))
#endif

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */
typedef enum
{
    ADC_STACK_CURRENT_L1,
    ADC_STACK_CURRENT_L2,
    ADC_STACK_CURRENT_L3,
    ADC_STACK_DC_VOLTAGE,
    ADC_STACK_COUNT
}eADCStack_t;

typedef enum
{
    ADC2_STACK_CURRFBK_L1,
    ADC2_STACK_CURRFBK_L2,
    ADC2_STACK_CURRFBK_L3,
    ADC2_STACK_TEMPFBK_L1,
    ADC2_STACK_TEMPFBK_L2,
    ADC2_STACK_TEMPFBK_L3,
    ADC2_STACK_DCVOLTAGE2,
    ADC2_STACK_COUNT
}eADC2Stack_t;

#ifndef BOARDCFG_USE_MCC_ADC
typedef enum
{
    TRIGGER_PWM1,
    TRIGGER_PWM2,
    TRIGGER_PWM3,
    TRIGGER_PWM4,
    TRIGGER_PWM5,
    TRIGGER_PWM6,
    TRIGGER_PWM7,
    TRIGGER_PSEV,   /* Primary Special Event */
    TRIGGER_SSEV,   /* Secondary Special Event */
    TRIGGER_COUNT   
}eTrigger;


typedef enum
{
    STACK_MODE_NONE,
    STACK_MODE_NO_SHIFT,
    STACK_MODE_SHIFT_S1S2,
    STACK_MODE_SHIFT_L1L2L3,
    STACK_MODE_SHIFT_L1L2L3_S1S2,
    STACK_MODE_COUNT
}eStackMode_t;
#endif







/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */
extern uint16_t ADCStack1Buffered[ADC_STACK_COUNT];
extern uint16_t ADCStack2Buffered[ADC_STACK_COUNT];

extern uint16_t ADCStack3Buffered[ADC_STACK_COUNT];
extern uint16_t ADCDCLinkBuffered[ADC_STACK_COUNT];
extern uint16_t ADCLine_1Buffered[ADC_STACK_COUNT];
extern uint16_t ADCLine_2Buffered[ADC_STACK_COUNT];


extern uint16_t ADC2Stack1Buffered[ADC2_STACK_COUNT];
extern uint16_t ADC2Stack2Buffered[ADC2_STACK_COUNT];


#ifndef BOARDCFG_USE_MCC_ADC
extern uint16_t PWM_u16MinDutyTcy;
extern uint16_t PWM_u16MaxDutyTcy;

extern int32_t s32RealSamplingFreqCoef;
extern uint16_t u16ExecControlNewDataSkipBuffered;
extern uint16_t u16PWM1FreqHz;
extern uint16_t u16PWM2FreqHz;

extern uint16_t u16PWM1FreqHzReq;
extern uint16_t u16PWM2FreqHzReq;

extern uint16_t PWM_u16MinDutyCoef;
extern uint16_t PWM_u16MaxDutyCoef;
extern uint16_t PWM_u16StartDutyCoef;

extern uint16_t u16LoopTime1Tcy;
extern uint16_t u16LoopTime2Tcy;

extern bool bStack1FreqReset;
extern bool bStack2FreqReset;
#endif


/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
#ifndef BOARDCFG_USE_MCC_ADC
void ADC_vStop(void);
void ADC1_vReset(void);
void ADC2_vReset (void);
void ADC_vStart(void);
void ADC_vInit(void);
void ADC_vInterruptsSetup(void);
void ADCPWM_vStackModeChangeProcess( void );
void ADCPWM_vStackModeSetup( void );
void ADCPWM_vStackModeInit( uint32_t u32Fcy );

uint16_t ADCPWM_u16GetPWM1FreqHzReq (void);
uint16_t ADCPWM_u16GetPWM2FreqHzReq (void);
uint16_t ADCPWM_u16GetPWM1FreqHz (void);
uint16_t ADCPWM_u16GetPWM2FreqHz (void);
void ADCPWM_vSetPWM1FreqHzReq (uint16_t u16Input);
void ADCPWM_vSetPWM2FreqHzReq (uint16_t u16Input);

void PWM_vStop(void);       /* reset PWM Frequency trough ADC module */
void PWM_vStart(void);
#else
void ADC_RegisterFunctionsInISR(void);
#endif

void ADCPWM_vSetDutyCycleStack1 (MCLIB_s3Param_u16_t* psDutyCycles);
void ADCPWM_vSetDutyCycleStack2 (MCLIB_s3Param_u16_t* psDutyCycles);
void ADCPWM_vSetDutyCycleStack3 (MCLIB_s3Param_u16_t* psDutyCycles);

void PWM_vOutputEnable(void);
void PWM_vOutputDisable(void);

#ifndef BOARDCFG_USE_MCC_PWM
void PWM_vInit(void);
void PWM_vReset(void);
void PWM_vInterruptsSetup(void);
#else
void PWM_RegisterFunctionsInISR(void);
#endif

#endif	/* ADCPWM_H */
