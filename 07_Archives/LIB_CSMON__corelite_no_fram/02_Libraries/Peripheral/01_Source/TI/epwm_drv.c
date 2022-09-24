/* *****************************************************************************
 * File:   epwm_drv.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 11 24 02:34
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
#include "epwm_drv.h"
#include "driverlib.h"
#include "device.h"
#include "boardcfg.h"
#include "sys_drv.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#ifdef BOARDCFG_EPWM_USE_FOR_BOARD_HARDWARE_TEST
#define EPWM_USE_FOR_BOARD_HARDWARE_TEST    BOARDCFG_EPWM_USE_FOR_BOARD_HARDWARE_TEST
#else
#define EPWM_USE_FOR_BOARD_HARDWARE_TEST    0
#endif

#define EPWM_INVERT_LOW_SIDE    1U


#define EPWM_FREQ_HZ            2500

#define EPWM_TIMER_TBPRD  (((DEVICE_LSPCLK_FREQ / 2) / EPWM_FREQ_HZ) / 2)

#define EPWM_MAX_CMPA     EPWM_TIMER_TBPRD
#define EPWM_MIN_CMPA        0U
#define EPWM_MAX_CMPB     EPWM_TIMER_TBPRD
#define EPWM_MIN_CMPB        0U

#ifdef EPWM_ON_AT_STARTUP
#define EPWM_DEF_CMPA     EPWM_TIMER_TBPRD/2
#define EPWM_DEF_CMPB     EPWM_TIMER_TBPRD/2
#else
#define EPWM_DEF_CMPA     0
#define EPWM_DEF_CMPB     0
#endif

#define EPWM_CMP_UP         1U
#define EPWM_CMP_DOWN       0U

#if EPWM_USE_FOR_BOARD_HARDWARE_TEST
#define EPWM_ROLL_MODE      1U
#else
#define EPWM_ROLL_MODE      0U
#endif

#define EPWM_PHASE_SHIFT    0U

#define EPWM_ROLL_PRESCALER 10U

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
//
// Globals
//
typedef struct
{
    uint32_t epwmModule;
    uint16_t epwmCompADirection;
    uint16_t epwmCompBDirection;
    uint16_t epwmTimerIntCount;
    uint16_t epwmMaxCompA;
    uint16_t epwmMinCompA;
    uint16_t epwmDefCompA;
    uint16_t epwmMaxCompB;
    uint16_t epwmMinCompB;
    uint16_t epwmDefCompB;
    uint16_t epwmRoll;
    uint16_t epwmPhaseShift;
    uint16_t epwmPeriod;
}EPWM_sConfiguration;



/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
//
// Globals to hold the ePWM configuration used in this example
//
#if (BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG) || (BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC)
	#define EPWM_MODULES_USED       12
	#define EPWM_STACK_PHASES       3
	#define EPWM_START_MODULE_BASE  EPWM1_BASE
#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_SMALL_TEST
	#define EPWM_MODULES_USED       3
	#define EPWM_STACK_PHASES       1
	#define EPWM_START_MODULE_BASE  EPWM6_BASE
#endif

#define EPWM_FINAL_MODULE_BASE  (EPWM_START_MODULE_BASE + (EPWM_MODULES_USED * (EPWM2_BASE - EPWM1_BASE)))   /* this Base is not used */



EPWM_sConfiguration asEPWMConfig[EPWM_MODULES_USED]; /* does not work array with FreeMater utility */

uint16_t bUsePhaseOffset = 0;


uint16_t GlobalDisable = 0;

#if EPWM_USE_FOR_BOARD_HARDWARE_TEST
uint16_t bUseDutyOffset = 1;
uint16_t GlobalExternalUpdateCompare = 0;
#else
uint16_t bUseDutyOffset = 0;
uint16_t GlobalExternalUpdateCompare = 1;
#endif

uint16_t RollPrescaller = EPWM_ROLL_PRESCALER;

pfVoid_t pfEPWM1DataProcess = NULL;
pfVoid_t pfEPWM6DataProcess = NULL;

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */
void EPWM_vInitModule(uint32_t base, EPWM_sConfiguration* sConfig);
void EPWM_vInitConfig(EPWM_sConfiguration* sConfig);
void EPWM_vSetupEPWMActiveHighComplementary(uint32_t base);

/* *****************************************************************************
 * Functions
 **************************************************************************** */
uint32_t u32FpEPWM = FCY_HZ;


void EPWM_vSetPWMFrequencyGlobal(uint16_t u16PWMFreq_Hz)
{
    uint16_t index;
    uint32_t u32FpDiv4;
    uint16_t u16PeriodTimeTcy;

    u32FpDiv4 = u32FpEPWM >> 2; //must be fixed if not used default configuration

    /* Default EPWM clock is set to SYSCLK/2 -> Center aligned PWM-> totally divide by 4 */

    //ASSERT(SysCtl_getClock(DEVICE_OSCSRC_FREQ) == DEVICE_SYSCLK_FREQ);    //fix with read sys clock if needed

    u16PeriodTimeTcy = MCLIB_u16DivSat_u32u16(u32FpDiv4, u16PWMFreq_Hz);

    for (index = 0; index < EPWM_MODULES_USED; index++) {
        asEPWMConfig[index].epwmPeriod = u16PeriodTimeTcy;
        asEPWMConfig[index].epwmMaxCompA = u16PeriodTimeTcy;
        asEPWMConfig[index].epwmMaxCompB = u16PeriodTimeTcy;
    }
}


void EPWM_vInit (void)
{
    uint16_t index;
    uint32_t base;

    // Disable sync(Freeze clock to PWM as well)
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_GTBCLKSYNC);
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);


    base  = EPWM_START_MODULE_BASE;
    for (index = 0; index < EPWM_MODULES_USED; index++) {
        EPWM_vInitConfig(&asEPWMConfig[index]);
        EPWM_vInitModule(base, &asEPWMConfig[index]);
        base += EPWM2_BASE - EPWM1_BASE;
    }

    // Enable sync and clock to PWM
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_GTBCLKSYNC);
}


void EPWM_vInitConfig(EPWM_sConfiguration *sConfig)
{
	uint16_t phaseOffset = 0;
	uint16_t dutyRollOffset = 0;
	static uint16_t offsetPos = 0;

	if (bUsePhaseOffset) {
		phaseOffset = ((EPWM_TIMER_TBPRD * offsetPos) + EPWM_STACK_PHASES / 2) / EPWM_STACK_PHASES;
	}
	if (bUseDutyOffset) {
		dutyRollOffset = ((EPWM_TIMER_TBPRD * offsetPos) + EPWM_STACK_PHASES / 2) / EPWM_STACK_PHASES;
	}
	offsetPos++;
	if (offsetPos >= EPWM_TIMER_TBPRD) {
		offsetPos = 0;
	}

	sConfig->epwmModule = 0;
	sConfig->epwmCompADirection = EPWM_CMP_UP;
	sConfig->epwmCompBDirection = EPWM_CMP_DOWN;
	sConfig->epwmTimerIntCount = 0U;
	sConfig->epwmMinCompA = EPWM_MIN_CMPA;
	sConfig->epwmDefCompA = dutyRollOffset;
	//sConfig->epwmDefCompA = EPWM_DEF_CMPA;
	sConfig->epwmMinCompB = EPWM_MIN_CMPB;
	sConfig->epwmDefCompB = dutyRollOffset;
	//sConfig->epwmDefCompB = EPWM_DEF_CMPB;
	sConfig->epwmRoll = EPWM_ROLL_MODE;

	sConfig->epwmPhaseShift = phaseOffset;

	if (sConfig->epwmPeriod == 0) {
		sConfig->epwmPeriod = EPWM_TIMER_TBPRD;
	}
	if (sConfig->epwmMaxCompA == 0) {
		sConfig->epwmMaxCompA = EPWM_MAX_CMPA;
	}
	if (sConfig->epwmMaxCompB == 0) {
		sConfig->epwmMaxCompB = EPWM_MAX_CMPB;
	}
}


//
// EPWM_vInitModule - Configure ePWM
//
void EPWM_vInitModule(uint32_t base, EPWM_sConfiguration *sConfig)
{
	// Information this example uses to keep track of the direction the CMPA/CMPB values are moving,
	// the min and max allowed values and a pointer to the correct ePWM registers
	sConfig->epwmModule = base;

	// Set-up TBCLK
	EPWM_setTimeBasePeriod(base, sConfig->epwmPeriod);
	EPWM_setPhaseShift(base, sConfig->epwmPhaseShift);
	EPWM_setTimeBaseCounter(base, sConfig->epwmPhaseShift);

	// Set Compare values
	EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_A, sConfig->epwmDefCompA);
	EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_B, sConfig->epwmDefCompB);

	// Set up counter mode
	EPWM_setTimeBaseCounterMode(base, EPWM_COUNTER_MODE_UP_DOWN);
	EPWM_disablePhaseShiftLoad(base);
	EPWM_setClockPrescaler(base, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1);

	// Set up shadowing
	EPWM_setCounterCompareShadowLoadMode(base, EPWM_COUNTER_COMPARE_A, EPWM_COMP_LOAD_ON_CNTR_ZERO);
	EPWM_setCounterCompareShadowLoadMode(base, EPWM_COUNTER_COMPARE_B, EPWM_COMP_LOAD_ON_CNTR_ZERO);

	EPWM_setPeriodLoadMode(base, EPWM_PERIOD_SHADOW_LOAD);

	EPWM_selectPeriodLoadEvent(base, EPWM_SHADOW_LOAD_MODE_COUNTER_ZERO);

#if 0
    // Set actions
    EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
	EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
	EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);
	EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);

	EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
	EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);
	EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);
	EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB);

#else
	// Set actions
	EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
	EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);

#if EPWM_INVERT_LOW_SIDE
	EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
	EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);
#else
    EPWM_setActionQualifierAction(base,
                                  EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);
    EPWM_setActionQualifierAction(base,
                                  EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB);
#endif
#endif
	EPWM_vSetupEPWMActiveHighComplementary(base);

	// Interrupt where we will change the Compare Values. Select INT on Time base counter zero event, Enable INT, generate INT on 1st event
	EPWM_setInterruptSource(base, EPWM_INT_TBCTR_PERIOD);
	EPWM_enableInterrupt(base);
	EPWM_setInterruptEventCount(base, 1U);
}



uint16_t DeadFallingEdgeDelay = 280;    /* Resolution 10nsec */
uint16_t DeadRisingEdgeDelay = 280;    /* Resolution 10nsec */

uint16_t DeadFallingEdgeDelayMin = 240;    /* Resolution 10nsec */
uint16_t DeadRisingEdgeDelayMin = 240;    /* Resolution 10nsec */

void EPWM_vSetDeadTime(void)
{
    uint16_t index;
    uint32_t base;

    if (DeadFallingEdgeDelay < DeadFallingEdgeDelayMin)
    {
        DeadFallingEdgeDelay = DeadFallingEdgeDelayMin;
    }
    if (DeadRisingEdgeDelay < DeadRisingEdgeDelayMin)
    {
        DeadRisingEdgeDelay = DeadRisingEdgeDelayMin;
    }

    base  = EPWM_START_MODULE_BASE;
    for (index = 0; index < EPWM_MODULES_USED; index++)
    {

        EPWM_setFallingEdgeDelayCount(base, DeadFallingEdgeDelay);
        EPWM_setRisingEdgeDelayCount(base, DeadRisingEdgeDelay);
        base += EPWM2_BASE - EPWM1_BASE;
    }

}

void EPWM_vSetupEPWMActiveHighComplementary(uint32_t base)
{
	// Use EPWMA as the input for both RED and FED
	EPWM_setRisingEdgeDeadBandDelayInput(base, EPWM_DB_INPUT_EPWMA);
	EPWM_setFallingEdgeDeadBandDelayInput(base, EPWM_DB_INPUT_EPWMA);

	// Set the RED and FED values
	if (DeadFallingEdgeDelay < DeadFallingEdgeDelayMin) {
		DeadFallingEdgeDelay = DeadFallingEdgeDelayMin;
	}
	if (DeadRisingEdgeDelay < DeadRisingEdgeDelayMin) {
		DeadRisingEdgeDelay = DeadRisingEdgeDelayMin;
	}
	EPWM_setFallingEdgeDelayCount(base, DeadFallingEdgeDelay);
	EPWM_setRisingEdgeDelayCount(base, DeadRisingEdgeDelay);

	// Invert only the Falling Edge delayed output (AHC)
	EPWM_setDeadBandDelayPolarity(base, EPWM_DB_RED, EPWM_DB_POLARITY_ACTIVE_HIGH);
	EPWM_setDeadBandDelayPolarity(base, EPWM_DB_FED, EPWM_DB_POLARITY_ACTIVE_LOW);

	// Use the delayed signals instead of the original signals
	EPWM_setDeadBandDelayMode(base, EPWM_DB_RED, true);
	EPWM_setDeadBandDelayMode(base, EPWM_DB_FED, true);

	// DO NOT Switch Output A with Output B
	EPWM_setDeadBandOutputSwapMode(base, EPWM_DB_OUTPUT_A, false);
	EPWM_setDeadBandOutputSwapMode(base, EPWM_DB_OUTPUT_B, false);
}




//
// updateCompare - Function to update the frequency
//
void updateCompare(EPWM_sConfiguration *sConfig)
{
    if (GlobalExternalUpdateCompare)
    {
        return;
    }

    if (GlobalDisable)
    {
        EPWM_setCounterCompareValue(sConfig->epwmModule, EPWM_COUNTER_COMPARE_A, 0);
        EPWM_setCounterCompareValue(sConfig->epwmModule, EPWM_COUNTER_COMPARE_B, 0);

    }
    else
    {

        if (sConfig->epwmModule != 0)
        {



            if (sConfig->epwmRoll == 0)
            {

                //
                // Set-up TBCLK
                //
                EPWM_setTimeBasePeriod(sConfig->epwmModule,  sConfig->epwmPeriod);
                EPWM_setPhaseShift(sConfig->epwmModule, sConfig->epwmPhaseShift);


                //
                // Set Compare values
                //
                EPWM_setCounterCompareValue(sConfig->epwmModule, EPWM_COUNTER_COMPARE_A, sConfig->epwmDefCompA);
                EPWM_setCounterCompareValue(sConfig->epwmModule, EPWM_COUNTER_COMPARE_B, sConfig->epwmDefCompB);


            }
            else
            {
                //compAValue = EPWM_getCounterCompareValue(sConfig->epwmModule, EPWM_COUNTER_COMPARE_A);
                //compBValue = EPWM_getCounterCompareValue(sConfig->epwmModule, EPWM_COUNTER_COMPARE_B);


                //  Change the CMPA/CMPB values every 10th interrupt.
                if(sConfig->epwmTimerIntCount >= RollPrescaller)
                {
                    sConfig->epwmTimerIntCount = 0U;

                    // If we were increasing CMPA, check to see if we reached the max
                    // value. If not, increase CMPA else, change directions and decrease CMPA
                    if(sConfig->epwmCompADirection == EPWM_CMP_UP)
                    {
                        if(sConfig->epwmDefCompA < (sConfig->epwmMaxCompA))
                        {
                            EPWM_setCounterCompareValue(sConfig->epwmModule, EPWM_COUNTER_COMPARE_A, ++sConfig->epwmDefCompA);
                        }
                        else
                        {
                            sConfig->epwmCompADirection = EPWM_CMP_DOWN;
                            EPWM_setCounterCompareValue(sConfig->epwmModule, EPWM_COUNTER_COMPARE_A, --sConfig->epwmDefCompA);
                        }
                    }
                    // If we were decreasing CMPA, check to see if we reached the min
                    // value. If not, decrease CMPA else, change directions and increase CMPA
                    else
                    {
                        if( sConfig->epwmDefCompA == (sConfig->epwmMinCompA))
                        {
                            sConfig->epwmCompADirection = EPWM_CMP_UP;
                            EPWM_setCounterCompareValue(sConfig->epwmModule, EPWM_COUNTER_COMPARE_A, ++sConfig->epwmDefCompA);
                        }
                        else
                        {
                            EPWM_setCounterCompareValue(sConfig->epwmModule, EPWM_COUNTER_COMPARE_A, --sConfig->epwmDefCompA);
                        }
                    }
    #if 1
                    // If we were increasing CMPB, check to see if we reached the max
                    // value. If not, increase CMPB else, change directions and decrease CMPB
                    if(sConfig->epwmCompBDirection == EPWM_CMP_UP)
                    {
                        if(sConfig->epwmDefCompB < (sConfig->epwmMaxCompB))
                        {
                            EPWM_setCounterCompareValue(sConfig->epwmModule, EPWM_COUNTER_COMPARE_B, ++sConfig->epwmDefCompB);
                        }
                        else
                        {
                            sConfig->epwmCompBDirection = EPWM_CMP_DOWN;
                            EPWM_setCounterCompareValue(sConfig->epwmModule, EPWM_COUNTER_COMPARE_B, --sConfig->epwmDefCompB);
                        }
                    }
                    // If we were decreasing CMPB, check to see if we reached the min
                    // value. If not, decrease CMPB else, change directions and increase CMPB
                    else
                    {
                        if(sConfig->epwmDefCompB == (sConfig->epwmMinCompB))
                        {
                            sConfig->epwmCompBDirection = EPWM_CMP_UP;
                            EPWM_setCounterCompareValue(sConfig->epwmModule,EPWM_COUNTER_COMPARE_B, ++sConfig->epwmDefCompB);
                        }
                        else
                        {
                            EPWM_setCounterCompareValue(sConfig->epwmModule,EPWM_COUNTER_COMPARE_B,--sConfig->epwmDefCompB);
                        }
                    }
    #endif
                }
            }

        }
    }
    sConfig->epwmTimerIntCount++;
}


void EPWM_vSetFunctionEPWM1DataProcess (pfVoid_t pFunc)
{
    pfEPWM1DataProcess = pFunc;
}

void EPWM_vSetFunctionEPWM6DataProcess (pfVoid_t pFunc)
{
    pfEPWM6DataProcess = pFunc;
}

#define EPWM_CONFIG_INDEX(x)   (uint16_t)((x - EPWM_START_MODULE_BASE)/(EPWM2_BASE - EPWM1_BASE))

#if (EPWM1_BASE >= EPWM_START_MODULE_BASE) && (EPWM1_BASE < EPWM_FINAL_MODULE_BASE)
//
// epwm1ISR - ePWM 1 ISR
//
__interrupt void EPWM1_ISR(void)
{
    DEBUG_SET_PWM1();

    //
    // Update the CMPA and CMPB values
    //
    updateCompare(&asEPWMConfig[EPWM_CONFIG_INDEX(EPWM1_BASE)]);

    if (pfEPWM1DataProcess != NULL)
    {
        pfEPWM1DataProcess();
    }

    //
    // Clear INT flag for this timer
    //
    EPWM_clearEventTriggerInterruptFlag(EPWM1_BASE);

    //
    // Acknowledge interrupt group
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
    DEBUG_CLR_PWM1();
}
#endif
#if (EPWM2_BASE >= EPWM_START_MODULE_BASE) && (EPWM2_BASE < EPWM_FINAL_MODULE_BASE)

//
// epwm2ISR - ePWM 2 ISR
//
__interrupt void EPWM2_ISR(void)
{
    //
    // Update the CMPA and CMPB values
    //
    updateCompare(&asEPWMConfig[EPWM_CONFIG_INDEX(EPWM2_BASE)]);

    //
    // Clear INT flag for this timer
    //
    EPWM_clearEventTriggerInterruptFlag(EPWM2_BASE);

    //
    // Acknowledge interrupt group
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
}
#endif
#if (EPWM3_BASE >= EPWM_START_MODULE_BASE) && (EPWM3_BASE < EPWM_FINAL_MODULE_BASE)

//
// epwm3ISR - ePWM 3 ISR
//
__interrupt void EPWM3_ISR(void)
{
    //
    // Update the CMPA and CMPB values
    //
    updateCompare(&asEPWMConfig[EPWM_CONFIG_INDEX(EPWM3_BASE)]);

    //
    // Clear INT flag for this timer
    //
    EPWM_clearEventTriggerInterruptFlag(EPWM3_BASE);

    //
    // Acknowledge interrupt group
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
}


#endif
#if (EPWM4_BASE >= EPWM_START_MODULE_BASE) && (EPWM4_BASE < EPWM_FINAL_MODULE_BASE)

//
// epwm4ISR - ePWM 4 ISR
//
__interrupt void EPWM4_ISR(void)
{
    //
    // Update the CMPA and CMPB values
    //
    updateCompare(&asEPWMConfig[EPWM_CONFIG_INDEX(EPWM4_BASE)]);


    //
    // Clear INT flag for this timer
    //
    EPWM_clearEventTriggerInterruptFlag(EPWM4_BASE);

    //
    // Acknowledge interrupt group
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
}

#endif
#if (EPWM5_BASE >= EPWM_START_MODULE_BASE) && (EPWM5_BASE < EPWM_FINAL_MODULE_BASE)
//
// epwm5ISR - ePWM 5 ISR
//
__interrupt void EPWM5_ISR(void)
{
    //
    // Update the CMPA and CMPB values
    //
    updateCompare(&asEPWMConfig[EPWM_CONFIG_INDEX(EPWM5_BASE)]);

    //
    // Clear INT flag for this timer
    //
    EPWM_clearEventTriggerInterruptFlag(EPWM5_BASE);

    //
    // Acknowledge interrupt group
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
}

#endif
#if (EPWM6_BASE >= EPWM_START_MODULE_BASE) && (EPWM6_BASE < EPWM_FINAL_MODULE_BASE)
//
// epwm6ISR - ePWM 6 ISR
//
__interrupt void EPWM6_ISR(void)
{
    DEBUG_SET_PWM6();
    //
    // Update the CMPA and CMPB values
    //
    updateCompare(&asEPWMConfig[EPWM_CONFIG_INDEX(EPWM6_BASE)]);

    if (pfEPWM6DataProcess != NULL)
    {
        pfEPWM6DataProcess();
    }

    //
    // Clear INT flag for this timer
    //
    EPWM_clearEventTriggerInterruptFlag(EPWM6_BASE);

    //
    // Acknowledge interrupt group
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
    DEBUG_CLR_PWM6();
}


#endif
#if (EPWM7_BASE >= EPWM_START_MODULE_BASE) && (EPWM7_BASE < EPWM_FINAL_MODULE_BASE)

//
// epwm7ISR - ePWM 7 ISR
//
__interrupt void EPWM7_ISR(void)
{
    //
    // Update the CMPA and CMPB values
    //
    updateCompare(&asEPWMConfig[EPWM_CONFIG_INDEX(EPWM7_BASE)]);


    //
    // Clear INT flag for this timer
    //
    EPWM_clearEventTriggerInterruptFlag(EPWM7_BASE);

    //
    // Acknowledge interrupt group
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
}

#endif
#if (EPWM8_BASE >= EPWM_START_MODULE_BASE) && (EPWM8_BASE < EPWM_FINAL_MODULE_BASE)
//
// epwm8ISR - ePWM 8 ISR
//
__interrupt void EPWM8_ISR(void)
{
    //
    // Update the CMPA and CMPB values
    //
    updateCompare(&asEPWMConfig[EPWM_CONFIG_INDEX(EPWM8_BASE)]);

    //
    // Clear INT flag for this timer
    //
    EPWM_clearEventTriggerInterruptFlag(EPWM8_BASE);

    //
    // Acknowledge interrupt group
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
}
#endif
#if (EPWM9_BASE >= EPWM_START_MODULE_BASE) && (EPWM9_BASE < EPWM_FINAL_MODULE_BASE)

//
// epwm9ISR - ePWM 9 ISR
//
__interrupt void EPWM9_ISR(void)
{
    //
    // Update the CMPA and CMPB values
    //
    updateCompare(&asEPWMConfig[EPWM_CONFIG_INDEX(EPWM9_BASE)]);

    //
    // Clear INT flag for this timer
    //
    EPWM_clearEventTriggerInterruptFlag(EPWM9_BASE);

    //
    // Acknowledge interrupt group
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
}


#endif
#if (EPWM10_BASE >= EPWM_START_MODULE_BASE) && (EPWM10_BASE < EPWM_FINAL_MODULE_BASE)

//
// epwm10ISR - ePWM 10 ISR
//
__interrupt void EPWM10_ISR(void)
{
    //
    // Update the CMPA and CMPB values
    //
    updateCompare(&asEPWMConfig[EPWM_CONFIG_INDEX(EPWM10_BASE)]);


    //
    // Clear INT flag for this timer
    //
    EPWM_clearEventTriggerInterruptFlag(EPWM10_BASE);

    //
    // Acknowledge interrupt group
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
}
#endif
#if (EPWM11_BASE >= EPWM_START_MODULE_BASE) && (EPWM11_BASE < EPWM_FINAL_MODULE_BASE)

//
// epwm11ISR - ePWM 11 ISR
//
__interrupt void EPWM11_ISR(void)
{
    //
    // Update the CMPA and CMPB values
    //
    updateCompare(&asEPWMConfig[EPWM_CONFIG_INDEX(EPWM11_BASE)]);

    //
    // Clear INT flag for this timer
    //
    EPWM_clearEventTriggerInterruptFlag(EPWM11_BASE);

    //
    // Acknowledge interrupt group
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
}
#endif
#if (EPWM12_BASE >= EPWM_START_MODULE_BASE) && (EPWM12_BASE < EPWM_FINAL_MODULE_BASE)

//
// epwm12ISR - ePWM 12 ISR
//
__interrupt void EPWM12_ISR(void)
{
    //
    // Update the CMPA and CMPB values
    //
    updateCompare(&asEPWMConfig[EPWM_CONFIG_INDEX(EPWM12_BASE)]);

    //
    // Clear INT flag for this timer
    //
    EPWM_clearEventTriggerInterruptFlag(EPWM12_BASE);

    //
    // Acknowledge interrupt group
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
}

#endif


