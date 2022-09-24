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



/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
//#include "control_cfg.h"
#include "mclib.h"
#include "boardcfg.h"
#include "adc_drv.h"



#define LOOPTIME1_HZ     BOARDCFG_PWM_FREQUENCY_HZ                   /* PWM Frequency */
#define LOOPTIME2_HZ     BOARDCFG_PWM_FREQUENCY_HZ                   /* PWM Frequency */




/* *****************************************************************************
 * Configuration Definitions Calculation Functions
 **************************************************************************** */
/* *****************************************************************************
 * Function Like for Library Consider
 **************************************************************************** */


#define CoefToTicks(flCoef, u16PeriodTicks) (uint16_t)(flCoef * u16PeriodTicks)

#define RESOLUTION_NANOSECONDS  1000000000.0
#define RESOLUTION_MICROSECONDS 1000000.0
#define RESOLUTION_MILLISECONDS 1000.0

#define TimeToTicks(flInputTime, flBaseFreq_HZ, flInputResolition) (uint16_t)(((flInputTime / flInputResolition) * flBaseFreq_HZ) + 0.5)

#define nSecToTicks(flInputTime, flBaseFreq_HZ) TimeToTicks (flInputTime, flBaseFreq_HZ, RESOLUTION_NANOSECONDS)
#define uSecToTicks(flInputTime, flBaseFreq_HZ) TimeToTicks (flInputTime, flBaseFreq_HZ, RESOLUTION_MICROSECONDS)
#define mSecToTicks(flInputTime, flBaseFreq_HZ) TimeToTicks (flInputTime, flBaseFreq_HZ, RESOLUTION_MILLISECONDS)


/* *****************************************************************************
 * Configuration Definitions Calculations
 **************************************************************************** */
/* Basic loop period in units of Tcy */
#define LOOPTIME1_TCY    (unsigned int)((FCY_HZ / LOOPTIME1_HZ) / 4)    //!!! PWM CLOCK FIX ?/4 or not
#define LOOPTIME2_TCY    (unsigned int)((FCY_HZ / LOOPTIME2_HZ) / 4)    //!!! PWM CLOCK FIX ?/4 or not

#define PWM_MIN_DUTY_TCY    CoefToTicks(PWM_MIN_DUTY_COEF,LOOPTIME1_TCY)
#define PWM_MAX_DUTY_TCY    CoefToTicks(PWM_MAX_DUTY_COEF,LOOPTIME1_TCY)
#define PWM_START_DUTY_TCY    CoefToTicks(PWM_START_DUTY_COEF,LOOPTIME1_TCY)


#define ADC_PWM_RESULT_TO_16BITS_SHIFT  (BOARDCFG_ADC_RESULT_BITS - ADC_RESOLUTION)

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


/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */
extern uint16_t u16LoopTime1Tcy;
extern uint16_t u16LoopTime2Tcy;

extern int32_t s32RealSamplingFreqCoef;

extern uint16_t ADCStack1Buffered[ADC_STACK_COUNT];
extern uint16_t ADCStack2Buffered[ADC_STACK_COUNT];
extern uint16_t ADCStack3Buffered[ADC_STACK_COUNT];
extern uint16_t ADCDCLinkBuffered[ADC_STACK_COUNT];
extern uint16_t ADCLine_1Buffered[ADC_STACK_COUNT];
extern uint16_t ADCLine_2Buffered[ADC_STACK_COUNT];

extern uint16_t ADC2Stack1Buffered[ADC2_STACK_COUNT];
extern uint16_t ADC2Stack2Buffered[ADC2_STACK_COUNT];

extern bool bStack1FreqReset;
extern bool bStack2FreqReset;

extern uint16_t PWM_u16MinDutyTcy;
extern uint16_t PWM_u16MaxDutyTcy;
extern uint16_t PWM_u16StartDutyTcy;
extern uint16_t PWM_u16MinDutyCoef;
extern uint16_t PWM_u16MaxDutyCoef;
extern uint16_t PWM_u16StartDutyCoef;

extern uint16_t u16ExecControlNewDataSkipBuffered;

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */

void PWM_vOutputEnable(void);
void PWM_vOutputDisable(void);
void ADCPWM_vSetDutyCycleStack1 (MCLIB_s3Param_u16_t* psDutyCycles);
void ADCPWM_vSetDutyCycleStack2 (MCLIB_s3Param_u16_t* psDutyCycles);
void ADCPWM_vSetDutyCycleStack3 (MCLIB_s3Param_u16_t* psDutyCycles);
void ADCPWM_vSetDutyCycleStack4 (MCLIB_s3Param_u16_t* psDutyCycles);
void ADCPWM_vReadNewADCData(void);

bool ADCPWM_vStackFreqSetupNeeded( void );
uint16_t ADCPWM_u16GetPWM1FreqHzReq (void);
uint16_t ADCPWM_u16GetPWM2FreqHzReq (void);
uint16_t ADCPWM_u16GetPWM1FreqHz (void);
uint16_t ADCPWM_u16GetPWM2FreqHz (void);
void ADCPWM_vSetPWM1FreqHzReq (uint16_t u16Input);
void ADCPWM_vSetPWM2FreqHzReq (uint16_t u16Input);
bool ADCPWM_vStackFreqSetup( void );
uint16_t ADCPWM_u16GetLoopTime1Tcy (void);
uint16_t ADCPWM_u16GetLoopTime2Tcy (void);

#endif	/* ADCPWM_H */
