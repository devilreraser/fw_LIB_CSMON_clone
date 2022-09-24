/* *****************************************************************************
 * File:   control.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 02 11 15:49
 * 
 * Description: Control of Two 3-Phase Inverters 
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef CONTROL_H
#define	CONTROL_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "stdint.h"
#include "stdbool.h"
#include "mclib.h"

#include "adcpwm.h"
#include "control_common.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

#define CONTROL_USE_NEW_RMS             1
#define CONTROL_FAULT_RESET_DELAY_TICKS 10      /* Resolution One PWM Period (CountPWM Periods in Fault Reset State in order to complete resets in Main Loop Process) */


/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */





/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */
typedef enum
{
    CTRL_STA_INIT,
    CTRL_STA_FAULT_RESET,
    CTRL_STA_FAULT,
    CTRL_STA_READY,
    CTRL_STA_CALIB,
    CTRL_STA_RUN,
    CTRL_STA_RUN_DELAY,     /* Delay Before Run mode start in order to fill DCVoltage filters */
    CTRL_STA_COUNT
}eControlState_t;

typedef enum
{
    CTRL_ERR_NO_ERROR,
    CTRL_ERR_NO_INIT,
    CTRL_ERR_TRIP_REQUEST,
    CTRL_ERR_CALIB,
    CTRL_ERR_RUN,
    CTRL_ERR_TRIP_FPGA,
    CTRL_ERR_PWM_FPGA,
    CTRL_ERR_TEMPERATURE,
    CTRL_ERR_COUNT
}eErrorState_t;

typedef enum
{
    STACK_1,
    #if BOARDCFG_STACK_COUNT >= 2
    STACK_2,
    #endif
    #if BOARDCFG_STACK_COUNT >= 3
    STACK_3,
    #endif
    STACK_COUNT
}CONTROL_eStack;

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */




typedef struct
{
   bool bStart;
   bool bClearFaults;
   bool bFault;
   bool bEmulateCurrentFbck;
   bool bEmulateVoltageFbck;
   bool bCalculateRMSABC;
   bool bCalculateCFBKRMSABC;
   bool bCalculateRMSDCBus;
   bool bUseControllerPI;
   
   //bool bUseCalibrationOnly;
   bool bStartNoCalib;
   bool bStartCalibOnly;
   bool bUseRunDelay;
   
   bool bErrorClearForced;
   
   bool bUseWrongVoltageOffsetDetection;


}sControlAccess_t;


typedef struct
{
    uint16_t bCurrentError : 1;
    uint16_t bCurrToVoltPICtrl : 1;
    uint16_t bVoltageReq : 1;
    uint16_t bFixWithDCLink : 1;
    uint16_t bVoltageDQReq : 1;
    uint16_t bParkInv : 1;
    uint16_t bClarkeInv : 1;

}sControlBypassFlags_t;

typedef struct
{
    uint16_t u16Register;
    sControlBypassFlags_t sFlags;
}uControlBypass_t;



typedef struct
{
    int16_t          s16Speed;               /* Speed of the Field */
    int16_t          s16Position;            /* Position of the Field */
    uint16_t         u16VoltageDCBus;        /* Voltage DC-Link */
    uint16_t         u16VoltageDCBusFilt;    /* Voltage DC-Link Filtered */

    int16_t          s16CurrentAmpFbck;      /* Measured Current Amplitude */
    int16_t          u16VoltageRMSReq;       /* Requested Voltage RMS */
    int16_t          u16VoltageAmpReq;       /* Requested Voltage Amplitude */
    int16_t          s16VoltageAmpScaled;    /* Scaled With DC-Link Value */

    int16_t          s16CurrentAmpFbckEmulation;
    int16_t          s16CurrentRMSFbckEmulation;
    int16_t          s16PositionFbckEmulation;   
    uint16_t          u16VoltageDCBusFbckEmulation;

    int16_t          s16SpeedFbckEmulation;
    uint16_t         u16ScaleModulationIndex;
    uint16_t         u16VoltageStartup;
    uint16_t         u16ShiftIncResolution;

    int16_t          s16CurrentAmpFbckIncResolution;
    int16_t          s16CurrentAmpFbckFilt;
    uint16_t         u16RoundTicksCounter;
    uint16_t         u16RoundTicksCount;

    int16_t          s16CurrentRMSFbck;      /* Measured Current RMS */
    int16_t          s16CurrentRMS1PhFbckEmulation;
    uint16_t         u16ShiftIncResolutionLast;
    int16_t          s16PositionOld;

    uint16_t         u16VoltageDCBusIncResolution;
    uint16_t         u16VoltageDCBusRMS;
    uint16_t         u16VoltageDCBusFiltPower2;
    uint16_t         u16VoltageDCBusRMSFilt;

    int16_t          s16CurrentRMSReq1Ph;
    int16_t          s16CurrentRMSReq;      /* Requested Current RMS 3Ph */
    int16_t          s16CurrentRMSError;
    uControlBypass_t uControlBypass;

    uint16_t         u16VoltageDCBusFbckEmulationAtStart;
    uint16_t         u16VoltageDCBusFbckEmulationRunning;
    uint16_t         u16VoltageDCBusFbckEmulationReq;
    uint16_t         u16ShiftIncResolutionLastDCBus;

    int16_t          u16ShiftIncResolutionRMSABC;
    int16_t          u16ShiftIncResolutionRMSCFBK;
    uint16_t         u16ControlPrescaler;
    uint16_t         u16ControlPrescalerCounter;

    uint16_t         u16VoltageDCBusDiffCalibration;
    uint16_t         u16VoltageDCBusStartCalibration;
    uint16_t         u16VoltageDCBusEndCalibration;
    uint16_t         u16VoltageDCBusDeltaCalibration;

    uint16_t         u16CurrentABCFbckTotalDecreaseResolution;



    MCLIB_s2Param_s16_t  sCurrentAlBeFbck;       /* AlBe After Clarke Transform */
    MCLIB_s2Param_s16_t  sVoltageDQScaled;       /* Fractional Scaled */
    MCLIB_s2Param_s16_t  sVoltageAlBeScaled;     /* Fractional Scaled */
    MCLIB_s2Param_s16_t  sCurrentDQFbckEmulation;       /*  */

    MCLIB_s2Param_s16_t  sCurrentAlBeFbckEmulation;     /*  */
    MCLIB_s2Param_s16_t  sPositionSinCos;
    MCLIB_s2Param_s16_t  sPositionSinCosFbckEmulation;
    MCLIB_s2Param_s16_t  sCurrentAlBeFbckIncResolution;       

    MCLIB_s3Param_u16_t  sDutyCycle;             
    MCLIB_s3Param_s16_t  sVoltageABCScaled;      /* Fractional Scaled */
    MCLIB_s3Param_s16_t  sCurrentABCFbck;        /* ABC Phase Currents Feedback */
    MCLIB_s3Param_s16_t  sCurrentABCFbckIncResolution;       

    MCLIB_s3Param_s16_t  sCurrentABCFbckReal;    /* ABC Phase Currents Feedback real */
    MCLIB_s3Param_u16_t  sDutyCycleSin;
    MCLIB_s3Param_u16_t  sDutyCycleSVM;
    MCLIB_s3Param_s16_t  sVoltageABCScaledSVM;      /* Fractional Scaled */


    MCLIB_s3Param_s16_t  sCurrentABCFbckFilt;    
    MCLIB_s3Param_s16_t  sCurrentABCFbckRMS;    
    MCLIB_s3Param_s16_t  sCurrentABCFbckRMSFilt;    
    MCLIB_s3Param_s16_t  sCFBKABCFbck;        /* ABC Phase CFBK from driver */

    MCLIB_s3Param_s16_t  sCFBKABCFbckIncResolution;       
    MCLIB_s3Param_s16_t  sCFBKABCFbckFilt;    
    MCLIB_s3Param_s16_t  sCFBKABCFbckRMS; 
    MCLIB_s3Param_s16_t  sCFBKABCFbckRMSFilt;    

    MCLIB_sFilterMAs16_t sCurrentAmpFbckFilt;
    MCLIB_sFilterMAs16_t sCurrentAFbckFilter;    /* A Phase Current Filter */
    MCLIB_sFilterMAs16_t sCurrentBFbckFilter;    /* B Phase Current Filter */
    MCLIB_sFilterMAs16_t sCurrentCFbckFilter;    /* C Phase Current Filter */

    MCLIB_sFilterMAu16_t sVoltageDCBusRMSFilter;
    MCLIB_sFilterMAs16_t sCurrentAFbckRMSFilter;    /* A Phase Current Filter */
    MCLIB_sFilterMAs16_t sCurrentBFbckRMSFilter;    /* B Phase Current Filter */
    MCLIB_sFilterMAs16_t sCurrentCFbckRMSFilter;    /* C Phase Current Filter */   

    MCLIB_sFilterMAu16_t sVoltageDCBusFilt;
    MCLIB_sFilterMAs16_t sCFBKAFbckFilter;    /* A Phase Current from driver Filter */
    MCLIB_sFilterMAs16_t sCFBKBFbckFilter;    /* B Phase Current from driver Filter */
    MCLIB_sFilterMAs16_t sCFBKCFbckFilter;    /* C Phase Current from driver Filter */

    MCLIB_sFilterMAu16_t sVoltageDCBusRootMeanFilter;
    MCLIB_sFilterMAs16_t sCFBKAFbckRMSFilter;    /* A Phase CFBK Filter */
    MCLIB_sFilterMAs16_t sCFBKBFbckRMSFilter;    /* B Phase CFBK Filter */
    MCLIB_sFilterMAs16_t sCFBKCFbckRMSFilter;    /* C Phase CFBK Filter */

    MCLIB_sFilterAVGs16_t sCFBKAFbckFilterAvg;    /* A Phase Current from driver Filter */
    MCLIB_sFilterAVGs16_t sCFBKBFbckFilterAvg;    /* B Phase Current from driver Filter */
    MCLIB_sFilterAVGs16_t sCFBKCFbckFilterAvg;    /* C Phase Current from driver Filter */

    MCLIB_sIntegrator_t sPositionIntegrator;
    MCLIB_sIntegrator_t sPositionFbckEmulationIntegrator;

    MCLIB_sRamp_t       sVoltageDCFbckEmulationRamp;

    MCLIB_sControllerPIpAWu16_t sVoltageReqPI;

    MCLIB_s3Param_s32_t sCurrentABCFbckTotal;

    MCLIB_sFilterAVGu32_t sVoltageDCBusRMSFiltNew;
    MCLIB_sFilterAVGs32_t sCurrentAFbckRMSFiltNew;
    MCLIB_sFilterAVGs32_t sCurrentBFbckRMSFiltNew;
    MCLIB_sFilterAVGs32_t sCurrentCFbckRMSFiltNew;

    MCLIB_sFilterAVGs32_t sCFBKAFbckRMSFiltNew;
    MCLIB_sFilterAVGs32_t sCFBKBFbckRMSFiltNew;
    MCLIB_sFilterAVGs32_t sCFBKCFbckRMSFiltNew;

    MCLIB_sFilterAVGu16_t sVoltageDCBusRMSFiltNewPass2;
    MCLIB_sFilterAVGs16_t sCurrentAFbckRMSFiltNewPass2;
    MCLIB_sFilterAVGs16_t sCurrentBFbckRMSFiltNewPass2;
    MCLIB_sFilterAVGs16_t sCurrentCFbckRMSFiltNewPass2;

    MCLIB_sFilterAVGs16_t sCFBKAFbckRMSFiltNewPass2;
    MCLIB_sFilterAVGs16_t sCFBKBFbckRMSFiltNewPass2;
    MCLIB_sFilterAVGs16_t sCFBKCFbckRMSFiltNewPass2;


     int16_t          s16SVMState;
    uint16_t          u16VoltageDCBusRMSFiltPass1;
    uint16_t          u16VoltageDCBusRMSFiltPass2;

    MCLIB_s3Param_s16_t  sCurrentABCFbckRMSFiltPass1;    
    MCLIB_s3Param_s16_t  sCFBKABCFbckRMSFiltPass1;    
    MCLIB_s3Param_s16_t  sCurrentABCFbckRMSFiltPass2;    
    MCLIB_s3Param_s16_t  sCFBKABCFbckRMSFiltPass2;    

    uint32_t u32VoltageDCBusPower2; 
    uint32_t u32VoltageDCBusPower2Avg; 

    MCLIB_s3Param_s32_t  sCurrentABCFbckPower2;    
    MCLIB_s3Param_s32_t  sCurrentABCFbckPower2Avg;    

    MCLIB_s3Param_s32_t  sCFBKABCFbckPower2;    
    MCLIB_s3Param_s32_t  sCFBKABCFbckPower2Avg;    


    uint32_t u32DummyEnd; 

}sStackData_t;


typedef struct
{
    uint16_t u16PeriodPWM;
    uint16_t u16DutyMin;
    uint16_t u16DutyMax;
    uint16_t u16SkippedPulses;

    int16_t s16DutyMinFrac;
    int16_t s16DutyMaxFrac;
    int16_t s16DutyLimitMaxFrac;
    int16_t s16DutyLimitMinFrac;

    uint16_t u16VoltageRunFilterBits;
    uint16_t u16CurrentRunFilterBits;
    uint16_t u16OffsetCalcFilterBits;
    uint16_t u16CFBKRunFilterBits;  

    uint16_t u16RunRmsFilterBits;

    uint32_t u32CalibCounter;   
    uint32_t u32SkippedPulsesProcessed;
    uint32_t u32RunDelayCounter;

    //uStackData_t uStack;
    union
    {
        sStackData_t sStack[STACK_COUNT];

        struct
        {
            sStackData_t sStack1;
            #if STACK_COUNT > 1
            sStackData_t sStack2;
            #endif
            #if STACK_COUNT > 2
            sStackData_t sStack3;
            #endif
        };
    };

   
}sControlData_t;

typedef struct
{
    int16_t s16CurrentRequestMax;
    int16_t s16FieldFrequencyRequestMax;
    int16_t s16FieldFrequencyRequestMin;
    int16_t s16PWMFrequencyRequestMax;
    int16_t s16PWMFrequencyRequestMin;

}sControlLimit_t;

typedef struct
{
    int16_t s16FieldSpeedLastStack1_dHz;
    uint16_t u16VoltageRequestDCLinkLast_dV;
}sControlParameterLast_t;


typedef struct
{
    int16_t s16FieldSpeed_dHz;
    int16_t s16FieldSpeedEmulation_dHz;

}sStackParameter_t;

typedef struct
{
    int16_t s16TripTemperature_dDeg;
    uint16_t u16VoltageRequestDCLink_dV;
    sStackParameter_t sStack[STACK_COUNT];

}sControlParameter_t;


typedef struct
{
    uint32_t u32ADCReferenceVoltage;
    uint32_t u32DACReferenceVoltage;

}sBoardParameter_t;




typedef union 
{
    uint16_t bErrTemp1 : 1;
    uint16_t bErrTemp2 : 1;
    uint16_t bErrTemp3 : 1;
 
}sTempError_t;

typedef union 
{
    uint16_t u16Register;
    sTempError_t sTempError;
    
}uTempError_t;




/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */



extern sControlConfig_t sControlConfig;

extern sControlAccess_t sControlAccess;
extern sControlData_t sControlData;

extern eControlState_t eControlState;
extern eErrorState_t eErrorState;

extern  int16_t CurrentStack1[3];
extern  int16_t CurrentStack2[3];
extern  uint16_t VoltageStack1;
extern  int16_t VoltageStack2;
extern  int16_t SpeedStack1;
extern  int16_t SpeedStack2;

extern  int16_t TempFbkStack1[3];


extern uint16_t DeadTimeStack1[3];
extern uint16_t DeadTimeStack2[3];

extern uint16_t ADCStack1Offset[ADC_STACK_COUNT];
extern uint16_t ADC2Stack1Offset[ADC2_STACK_COUNT];

extern uint16_t u16VoltageReqDCLink_dV;
extern int16_t s16CurrentRMSReq1Ph_dA;

extern  int16_t u16_irms_sns1;
extern  int16_t u16_irms_sns2;
extern  int16_t u16_irms_sns3;

extern  int16_t u16_irms_mod1;
extern  int16_t u16_irms_mod2;
extern  int16_t u16_irms_mod3;

extern  int16_t u16_urms_meas;

extern  int16_t CurrFbkStack1[3];

extern  int16_t s16CFKBNominalCurrentModule;




/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void CONTROL_vProcess (void);
void CONTROL_vProcessMainLoop(void);
uint16_t CONTROL_u16LimitPWMFrequency(uint16_t u16Input);
void CONTROL_vInit(void);
void CONTROL_vInitOnce (void);
void CONTROL_vRunRequest(bool bInput);
eControlState_t CONTROL_eGetState(void);
void CONTROL_vSetStartUpEmulationVoltage (void);
void CONTROL_vSetRunningEmulationVoltage (void);
void CONTROL_vCFBKCurrentMultiplierCalc(void);
void CONTROL_vInitMultipliers(void);

int16_t CONTROL_s16GetFieldFrequencyStack1_dHz (void);
void CONTROL_s16SetFieldFrequencyStack1_dHz(int16_t s16Input);
uint32_t CONTROL_u32GetAddrFieldFrequencyStack1_dHz (void);

uint32_t CONTROL_u32GetAddrVoltageRequestDCLink_dV (void);

int16_t CONTROL_s16GetTripTemperature_dDeg(void);
void CONTROL_s16SetTripTemperature_dDeg(int16_t s16Input);
void CONTROL_vSetStackMask(uint16_t u16StackMask);

#endif	/* CONTROL_H */
