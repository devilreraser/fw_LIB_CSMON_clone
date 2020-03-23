/*!*******************************************************************************
  \file			partable.h

  \remarks	spezieller header zu partable.c

  \author		Frank Müller (C) M&P GmbH 2004
	\date			19.03.04
*********************************************************************************/

#ifndef 	PARTABLE_H
#define 	PARTABLE_H
/********************************************************************************
 *	system includes															    														*
 ********************************************************************************/

/********************************************************************************
 *	project includes																														*
 ********************************************************************************/
#include "sys_param.h"

/********************************************************************************
 *	macros																																			*
 ********************************************************************************/
#define MAX_PAR				(478)		// obsolete!?
#define LAST_PAR_INDEX 		999			//RS für TSP Kanalauswahlbegrenzung
#define LEAST_PAR_INDEX 	1			//RS
#define P_(a)              (a)
#define NO_EVENTPARS       40

/********************************************************************************
 *	typedefs																																		*
 ********************************************************************************/

/********************************************************************************
 *	prototypes																																	*
 ********************************************************************************/
extern UINT16 GetNumberOfParams(void);	//  returns size of ParTable in byte


/********************************************************************************
 *	externs																																			*
 ********************************************************************************/
extern PAR_TAB_ENTRY_T ParTable[/*MAX_PAR*/];
#ifdef STRING_PARAMS
extern UINT8 ParVal_DeviceType[/*MAX_NAME_LEN*/];
extern UINT8 ParVal_HWVersion[/*MAX_NAME_LEN*/];
extern UINT8 ParVal_SWVersion[/*MAX_NAME_LEN*/];
extern UINT8 ParVal_SWBuildNum[/*MAX_NAME_LEN*/];
extern UINT8 ParVal_SWBuildDate[/*MAX_NAME_LEN*/];
extern UINT8 ParVal_SWToolVersion[/*MAX_NAME_LEN*/];
#endif

extern WORD ParVal_NVmemControl;
extern WORD ParVal_NVmemControl1;
extern WORD ParVal_commErrorWord;

extern INT32 ParVal_TSTrigLevel;	//RS
extern WORD ParVal_TSStatus;			//
extern WORD ParVal_TSControl;
extern WORD ParVal_TSTrigCondition;
extern UINT16 ParVal_TSSamplStepWidthFactor;
extern UINT16 ParVal_TSChannelNum;
extern INT16 ParVal_TSDelayNum;
extern UINT16 ParVal_TSBaseTime;
extern UINT16 ParVal_TSChannel0Par;
extern UINT16 ParVal_TSChannel1Par;
extern UINT16 ParVal_TSChannel2Par;
extern UINT16 ParVal_TSChannel3Par;
extern UINT16 ParVal_TSChannel4Par;
extern UINT16 ParVal_TSChannel5Par;
extern UINT16 ParVal_TSChannel6Par;
extern UINT16 ParVal_TSChannel7Par;
extern UINT16 ParVal_TSMemSize;

extern INT16 ParVal_i16_Test1;
extern INT16 ParVal_i16_Test2;
extern INT16 ParVal_i16_Test3;
extern INT16 ParVal_i16_Test4;
extern INT16 ParVal_i16_Test5;
extern INT16 ParVal_i16_Test6;
extern INT16 ParVal_i16_Test7;
extern INT16 ParVal_i16_Test8;
extern INT16 ParVal_i16_Test9;
extern INT16 ParVal_i16_Test10;
extern INT16 ParVal_i16_Test11;
extern INT16 ParVal_i16_Test12;
extern INT16 ParVal_i16_Test13;

extern INT32 ParVal_Vline;
extern INT32 ParVal_VLine1;
extern INT32 ParVal_VLine2;
extern INT32 ParVal_VLine3;
extern INT32 ParVal_Fline;
extern INT32 ParVal_Vdc;
extern INT32 ParVal_Iinv;
extern INT32 ParVal_Tigbt;
extern INT32 ParVal_Tigbt1;
extern INT32 ParVal_Tigbt2;
extern INT32 ParVal_Tigbt3;
extern INT32 ParVal_Tigbt4;
extern DWORD ParVal_DigIn;
extern WORD ParVal_DigOut;
extern INT32 ParVal_ILine;
extern INT32 ParVal_ILine1;
extern INT32 ParVal_ILine2;
extern INT32 ParVal_ILine3;
extern INT32 ParVal_CosPhiLine;
extern INT32 ParVal_Ih1;
extern INT32 ParVal_Ih2;
extern INT32 ParVal_Ih3;
extern INT32 ParVal_Ih4;
extern INT32 ParVal_Ih5;
extern INT32 ParVal_Ih6;
extern INT32 ParVal_Ih7;
extern INT32 ParVal_Ih8;
extern INT16 ParVal_RangeVL;
extern WORD ParVal_Island;
extern INT16 ParVal_RangeVdc;
extern INT32 ParVal_Pinv;
extern INT32 ParVal_Qinv;
extern INT32 ParVal_Vh1;
extern INT32 ParVal_Vh2;
extern INT32 ParVal_Vh3;
extern INT32 ParVal_Vh4;
extern INT32 ParVal_Vh5;
extern INT32 ParVal_Vh6;
extern INT32 ParVal_Vh7;
extern INT32 ParVal_Vh8;

extern INT16 ParVal_AnIn0;
extern INT16 ParVal_AnIn1;
extern INT16 ParVal_AnIn2;
extern INT16 ParVal_AnIn3;
extern INT16 ParVal_AnIn4;
extern INT16 ParVal_AnIn5;
extern INT16 ParVal_AnIn6;
extern INT16 ParVal_AnIn7;
extern INT16 ParVal_AnIn8;
extern INT16 ParVal_AnIn9;
extern INT16 ParVal_AnIn10;
extern INT16 ParVal_AnIn11;
extern INT16 ParVal_AnIn12;
extern INT16 ParVal_AnIn13;
extern INT16 ParVal_AnIn14;
extern INT16 ParVal_AnIn15;
extern INT16 ParVal_AnIn16;
extern INT16 ParVal_AnIn17;
extern INT16 ParVal_AnIn18;
extern INT16 ParVal_AnIn19;
extern INT16 ParVal_AnIn20;
extern INT16 ParVal_AnIn21;

extern INT16 ParVal_StateFaultCode;
extern INT16 ParVal_ConvState;
extern DWORD ParVal_HW_FaultCode;
extern DWORD ParVal_SW_FaultCode;
extern WORD ParVal_Comm_FaultCode;
extern WORD ParVal_Fieldbus_FaultCode;
extern INT16 ParVal_AutoresetTime;
extern DWORD ParVal_DigIn_FaultCode;
extern DWORD ParVal_AutoresetMask;

extern WORD ParVal_MainsCtrlIn;
extern WORD ParVal_FanCtrlIn;
extern WORD ParVal_PrechCtrlIn;
extern WORD ParVal_FaultResetIn;
extern WORD ParVal_StartStopIn;
extern WORD ParVal_PWMenableIn;
extern WORD ParVal_CtrlSrc;
extern INT16 ParVal_ForceDOs;
extern WORD ParVal_RefSrc;
extern WORD ParVal_StartDCIn;
extern WORD ParVal_HeaterIn;
extern WORD ParVal_ValveIn;

extern INT16 ParVal_TestRef1;
extern INT16 ParVal_TestRef2;
extern INT32 ParVal_IreRef;
extern INT32 ParVal_IreLim;
extern INT16 ParVal_Harmonic1_N;
extern INT16 ParVal_Harmonic1_D;
extern INT16 ParVal_Harmonic1_Q;
extern INT16 ParVal_Harmonic2_N;
extern INT16 ParVal_Harmonic2_D;
extern INT16 ParVal_Harmonic2_Q;
extern INT16 ParVal_Harmonic3_N;
extern INT16 ParVal_Harmonic3_D;
extern INT16 ParVal_Harmonic3_Q;
extern INT16 ParVal_Harmonic4_N;
extern INT16 ParVal_Harmonic4_D;
extern INT16 ParVal_Harmonic4_Q;
extern INT16 ParVal_Harmonic1_M;
extern INT16 ParVal_Harmonic2_M;
extern INT16 ParVal_Harmonic3_M;
extern INT16 ParVal_Harmonic4_M;
extern INT16 ParVal_Harmonic5_N;
extern INT16 ParVal_Harmonic5_D;
extern INT16 ParVal_Harmonic5_Q;
extern INT16 ParVal_Harmonic5_M;
extern INT16 ParVal_Harmonic6_N;
extern INT16 ParVal_Harmonic6_D;
extern INT16 ParVal_Harmonic6_Q;
extern INT16 ParVal_Harmonic6_M;
extern INT16 ParVal_Harmonic7_N;
extern INT16 ParVal_Harmonic7_D;
extern INT16 ParVal_Harmonic7_Q;
extern INT16 ParVal_Harmonic7_M;
extern INT16 ParVal_Harmonic8_N;
extern INT16 ParVal_Harmonic8_D;
extern INT16 ParVal_Harmonic8_Q;
extern INT16 ParVal_Harmonic8_M;
extern INT32 ParVal_Harmonic1_V1;
extern INT32 ParVal_Harmonic1_V2;
extern WORD ParVal_Harmonic1_Fb;
extern INT32 ParVal_Harmonic2_V1;
extern INT32 ParVal_Harmonic2_V2;
extern WORD ParVal_Harmonic2_Fb;
extern INT32 ParVal_Harmonic3_V1;
extern INT32 ParVal_Harmonic3_V2;
extern WORD ParVal_Harmonic3_Fb;
extern INT32 ParVal_Harmonic4_V1;
extern INT32 ParVal_Harmonic4_V2;
extern WORD ParVal_Harmonic4_Fb;
extern INT32 ParVal_Harmonic5_V1;
extern INT32 ParVal_Harmonic5_V2;
extern WORD ParVal_Harmonic5_Fb;
extern INT32 ParVal_Harmonic6_V1;
extern INT32 ParVal_Harmonic6_V2;
extern WORD ParVal_Harmonic6_Fb;
extern INT32 ParVal_Harmonic7_V1;
extern INT32 ParVal_Harmonic7_V2;
extern WORD ParVal_Harmonic7_Fb;
extern INT32 ParVal_Harmonic8_V1;
extern INT32 ParVal_Harmonic8_V2;
extern WORD ParVal_Harmonic8_Fb;
extern INT16 ParVal_Harmonic1_Vm;
extern INT16 ParVal_Harmonic2_Vm;
extern INT16 ParVal_Harmonic3_Vm;
extern INT16 ParVal_Harmonic4_Vm;
extern INT16 ParVal_Harmonic5_Vm;
extern INT16 ParVal_Harmonic6_Vm;
extern INT16 ParVal_Harmonic7_Vm;
extern INT16 ParVal_Harmonic8_Vm;

extern INT16 ParVal_EnableHarm;
extern INT16 ParVal_CosPhiRef;
extern WORD ParVal_QMode;
extern WORD ParVal_ReadyRunFault;

extern INT16 ParVal_AnaRef1;
extern INT16 ParVal_AnaRef2;
extern INT16 ParVal_AnaRef3;

extern INT16 ParVal_FNline;
extern INT16 ParVal_VNline;
extern INT32 ParVal_INinv;
extern INT16 ParVal_Ldr;
extern INT16 ParVal_Ldr_trans;
extern INT16 ParVal_Rdr;
extern INT32 ParVal_Cdc;
extern INT16 ParVal_VdcN;
extern INT16 ParVal_Vdcmax;
extern INT16 ParVal_Vdc_tripp;
extern INT16 ParVal_Vdc_tripm;
extern INT32 ParVal_RangeI;
extern INT32 ParVal_RangeIL;
extern INT32 ParVal_VI_phi;
extern INT32 ParVal_VP_phi;
extern INT16 ParVal_TrafoRatio;
extern INT16 ParVal_TrafoPhase;
extern INT16 ParVal_PhiVmes;
extern INT16 ParVal_Cfil;

extern INT16 ParVal_vLuv;
extern INT16 ParVal_vLwv;
extern INT16 ParVal_vLu;
extern INT16 ParVal_vLv;
extern INT16 ParVal_vinvu;
extern INT16 ParVal_vinvv;
extern INT16 ParVal_vLd;
extern INT16 ParVal_vLq;
extern INT16 ParVal_vLamp;
extern INT16 ParVal_vDC;
extern INT16 ParVal_iInvu;
extern INT16 ParVal_iInvv;
extern INT16 ParVal_iInvw;
extern INT16 ParVal_iInvd;
extern INT16 ParVal_iInvq;
extern INT16 ParVal_iInvamp;
extern INT16 ParVal_iLu;
extern INT16 ParVal_iLv;
extern INT16 ParVal_iLw;
extern INT16 ParVal_iLd;
extern INT16 ParVal_iLq;
extern INT16 ParVal_iLdav;
extern INT16 ParVal_iLqav;
extern INT16 ParVal_iLamp;
extern INT16 ParVal_fLine;
extern INT16 ParVal_thLine;
extern INT16 ParVal_tIGBT;
extern INT16 ParVal_Vdcref;
extern INT16 ParVal_iLqref;
extern INT16 ParVal_idref;
extern INT16 ParVal_iqref;
extern INT16 ParVal_udref;
extern INT16 ParVal_uqref;
extern INT16 ParVal_udIref;
extern INT16 ParVal_uqIref;
extern INT16 ParVal_ihdref1;
extern INT16 ParVal_ihqref1;
extern INT16 ParVal_ihLd1;
extern INT16 ParVal_ihLq1;
extern INT16 ParVal_ihinvd1;
extern INT16 ParVal_ihinvq1;
extern INT16 ParVal_ihinv_refd1;
extern INT16 ParVal_ihinv_refq1;
extern INT16 ParVal_vhrefd1;
extern INT16 ParVal_vhrefq1;
extern INT16 ParVal_ihdref2;
extern INT16 ParVal_ihqref2;
extern INT16 ParVal_ihLd2;
extern INT16 ParVal_ihLq2;
extern INT16 ParVal_ihinvd2;
extern INT16 ParVal_ihinvq2;
extern INT16 ParVal_ihinv_refd2;
extern INT16 ParVal_ihinv_refq2;
extern INT16 ParVal_vhrefd2;
extern INT16 ParVal_vhrefq2;
extern INT16 ParVal_ihdref3;
extern INT16 ParVal_ihqref3;
extern INT16 ParVal_ihLd3;
extern INT16 ParVal_ihLq3;
extern INT16 ParVal_ihinvd3;
extern INT16 ParVal_ihinvq3;
extern INT16 ParVal_ihinv_refd3;
extern INT16 ParVal_ihinv_refq3;
extern INT16 ParVal_vhrefd3;
extern INT16 ParVal_vhrefq3;
extern INT16 ParVal_ihdref4;
extern INT16 ParVal_ihqref4;
extern INT16 ParVal_ihLd4;
extern INT16 ParVal_ihLq4;
extern INT16 ParVal_ihinvd4;
extern INT16 ParVal_ihinvq4;
extern INT16 ParVal_ihinv_refd4;
extern INT16 ParVal_ihinv_refq4;
extern INT16 ParVal_vhrefd4;
extern INT16 ParVal_vhrefq4;
extern INT16 ParVal_ihdref5;
extern INT16 ParVal_ihqref5;
extern INT16 ParVal_ihLd5;
extern INT16 ParVal_ihLq5;
extern INT16 ParVal_ihinvd5;
extern INT16 ParVal_ihinvq5;
extern INT16 ParVal_ihinv_refd5;
extern INT16 ParVal_ihinv_refq5;
extern INT16 ParVal_vhrefd5;
extern INT16 ParVal_vhrefq5;
extern INT16 ParVal_ihdref6;
extern INT16 ParVal_ihqref6;
extern INT16 ParVal_ihLd6;
extern INT16 ParVal_ihLq6;
extern INT16 ParVal_ihinvd6;
extern INT16 ParVal_ihinvq6;
extern INT16 ParVal_ihinv_refd6;
extern INT16 ParVal_ihinv_refq6;
extern INT16 ParVal_vhrefd6;
extern INT16 ParVal_vhrefq6;
extern INT16 ParVal_ihdref7;
extern INT16 ParVal_ihqref7;
extern INT16 ParVal_ihLd7;
extern INT16 ParVal_ihLq7;
extern INT16 ParVal_ihinvd7;
extern INT16 ParVal_ihinvq7;
extern INT16 ParVal_ihinv_refd7;
extern INT16 ParVal_ihinv_refq7;
extern INT16 ParVal_vhrefd7;
extern INT16 ParVal_vhrefq7;
extern INT16 ParVal_ihdref8;
extern INT16 ParVal_ihqref8;
extern INT16 ParVal_ihLd8;
extern INT16 ParVal_ihLq8;
extern INT16 ParVal_ihinvd8;
extern INT16 ParVal_ihinvq8;
extern INT16 ParVal_ihinv_refd8;
extern INT16 ParVal_ihinv_refq8;
extern INT16 ParVal_vhrefd8;
extern INT16 ParVal_vhrefq8;

extern INT16 ParVal_Grid;
extern INT16 ParVal_Inverter;
extern INT16 ParVal_DC_link;
extern INT16 ParVal_Reference;
extern INT16 ParVal_Feedbacks;
extern INT16 ParVal_AnInputs;
extern INT16 ParVal_Faults;
extern INT16 ParVal_Controls;
extern INT16 ParVal_Testvars;
extern INT16 ParVal_Harmonics1;
extern INT16 ParVal_Harmonics2;
extern INT16 ParVal_Harmonics3;
extern INT16 ParVal_Trace;

extern UINT16 ParVal_OptimeHours;
extern UINT16 ParVal_OptimeSeconds;
extern UINT32 ParVal_OptimeWrite;

extern INT16 ParVal_ErrStack;
extern INT16 ParVal_ErrStackIndex;
extern UINT16 ParVal_ErrEvent1_Hours;
extern UINT16 ParVal_ErrEvent1_Secs;
extern INT16 ParVal_ErrEvent1_StateFault;
extern WORD ParVal_ErrEvent1_SW_Fault;
extern DWORD ParVal_ErrEvent1_HW_Fault;
extern WORD ParVal_ErrEvent1_Warnings;
extern UINT16 ParVal_ErrEvent2_Hours;
extern UINT16 ParVal_ErrEvent2_Secs;
extern INT16 ParVal_ErrEvent2_StateFault;
extern WORD ParVal_ErrEvent2_SW_Fault;
extern DWORD ParVal_ErrEvent2_HW_Fault;
extern WORD ParVal_ErrEvent2_Warnings;
extern UINT16 ParVal_ErrEvent3_Hours;
extern UINT16 ParVal_ErrEvent3_Secs;
extern INT16 ParVal_ErrEvent3_StateFault;
extern WORD ParVal_ErrEvent3_SW_Fault;
extern DWORD ParVal_ErrEvent3_HW_Fault;
extern WORD ParVal_ErrEvent3_Warnings;
extern UINT16 ParVal_ErrEvent4_Hours;
extern UINT16 ParVal_ErrEvent4_Secs;
extern INT16 ParVal_ErrEvent4_StateFault;
extern WORD ParVal_ErrEvent4_SW_Fault;
extern DWORD ParVal_ErrEvent4_HW_Fault;
extern WORD ParVal_ErrEvent4_Warnings;
extern UINT16 ParVal_ErrEvent5_Hours;
extern UINT16 ParVal_ErrEvent5_Secs;
extern INT16 ParVal_ErrEvent5_StateFault;
extern WORD ParVal_ErrEvent5_SW_Fault;
extern DWORD ParVal_ErrEvent5_HW_Fault;
extern WORD ParVal_ErrEvent5_Warnings;
extern UINT16 ParVal_ErrEvent6_Hours;
extern UINT16 ParVal_ErrEvent6_Secs;
extern INT16 ParVal_ErrEvent6_StateFault;
extern WORD ParVal_ErrEvent6_SW_Fault;
extern DWORD ParVal_ErrEvent6_HW_Fault;
extern WORD ParVal_ErrEvent6_Warnings;
extern UINT16 ParVal_ErrEvent7_Hours;
extern UINT16 ParVal_ErrEvent7_Secs;
extern INT16 ParVal_ErrEvent7_StateFault;
extern WORD ParVal_ErrEvent7_SW_Fault;
extern DWORD ParVal_ErrEvent7_HW_Fault;
extern WORD ParVal_ErrEvent7_Warnings;
extern UINT16 ParVal_ErrEvent8_Hours;
extern UINT16 ParVal_ErrEvent8_Secs;
extern INT16 ParVal_ErrEvent8_StateFault;
extern WORD ParVal_ErrEvent8_SW_Fault;
extern DWORD ParVal_ErrEvent8_HW_Fault;
extern WORD ParVal_ErrEvent8_Warnings;

extern INT16 ParVal_VoltageControl;
extern INT16 ParVal_OnePhase;
extern INT16 ParVal_VoltRef;
extern INT16 ParVal_FrequRef;
extern INT16 ParVal_RampTime;
extern INT16 ParVal_ActivateRef;
extern INT32 ParVal_VI_u;
extern INT32 ParVal_VP_u;
extern INT16 ParVal_Testmode;
extern INT16 ParVal_FieldbusLocalRemote;
extern INT32 ParVal_VI_i;
extern INT32 ParVal_VP_i;

extern INT32 ParVal_aM;
extern INT32 ParVal_VIi;
extern INT16 ParVal_IregMode;
extern INT16 ParVal_iInv_al;
extern INT16 ParVal_iInv_be;

extern INT16 ControlState;
extern INT16 LSI_IdentEn;
extern INT16 LSI_IdentFault;

extern INT16 ParVal_Harmonic1_phi;
extern INT16 ParVal_Harmonic2_phi;
extern INT16 ParVal_Harmonic3_phi;
extern INT16 ParVal_Harmonic4_phi;
extern INT16 ParVal_Harmonic5_phi;
extern INT16 ParVal_Harmonic6_phi;
extern INT16 ParVal_Harmonic7_phi;
extern INT16 ParVal_Harmonic8_phi;

extern INT16 ParVal_CurrentOffsets;
extern INT16 ParVal_OffsSelectIinv;
extern INT16 ParVal_OffsSelectIline;
extern INT16 ParVal_OffsInvActU;
extern INT16 ParVal_OffsInvActV;
extern INT16 ParVal_OffsInvActW;
extern INT16 ParVal_OffsLineActU;
extern INT16 ParVal_OffsLineActV;
extern INT16 ParVal_OffsLineActW;
extern INT16 ParVal_OffsInvSaveU;
extern INT16 ParVal_OffsInvSaveV;
extern INT16 ParVal_OffsInvSaveW;
extern INT16 ParVal_OffsLineSaveU;
extern INT16 ParVal_OffsLineSaveV;
extern INT16 ParVal_OffsLineSaveW;
extern INT16 ParVal_PhiImes;
extern INT16 ParVal_PWM_mode0;
extern INT16 ParVal_PWM_mode1;
extern INT16 ParVal_PWM_mode_i;
extern INT16 ParVal_PWM_Td;
extern INT32 ParVal_kVIDC;
extern INT16 ParVal_TempChar;
extern INT16 ParVal_PWM_frequ;
extern INT16 ParVal_PWM;
extern INT16 ParVal_TransMemory;
extern WORD ParVal_FaultRelIn;
extern WORD ParVal_Res1In;
extern WORD ParVal_Res2In;
extern INT16 ParVal_Overload;
extern INT16 ParVal_F_ov;
extern INT16 ParVal_Toverload;
extern INT16 ParVal_Tpause;
extern INT16 ParVal_Itrip;
extern INT16 ParVal_Imax;
extern INT16 ParVal_vinv_amp;
extern INT32 ParVal_Vinv;
extern INT32 ParVal_z1Vdc;
extern INT32 ParVal_kv_HarmLim;
extern INT32 ParVal_VI_boost;
extern INT32 ParVal_VP_boost;
extern WORD ParVal_HarmonicMode;

extern INT16 ParVal_iL0;
extern INT16 ParVal_iInv_0;
extern INT16 ParVal_u0ref;

extern INT16 ParVal_F_fil;
extern INT16 ParVal_D_fil;
extern INT16 ParVal_K_fil;

extern INT16 ParVal_CurrentControl;
extern INT32 ParVal_kc_fil;
extern INT32 ParVal_Harmonic_IGain;

extern WORD ParVal_PIBconfig;
extern WORD ParVal_PIBref;
extern INT32 ParVal_PIBtempdiff;
extern INT16 ParVal_I_StandbyOn;
extern INT16 ParVal_I_StandbyOff;

extern INT32 ParVal_IinvN;
extern INT32 ParVal_ILineN;
extern INT16 ParVal_VI_4Lbal;
extern INT16 ParVal_PWM_multiply;

extern WORD ParVal_DeviceIdentN;
extern UINT16 ParVal_HWVersionN;
extern UINT16 ParVal_SWVersionN;
extern UINT16 ParVal_SWBuildNumN;
extern INT16 ParVal_VoltRefRamp;
extern INT16 ParVal_RampTimeFrequ;
extern INT16 ParVal_VoltageMode;
extern INT16 ParVal_iLdref;

extern UINT16 ParVal_Passcode;
extern INT16 ParVal_Parameter_FaultCode;
extern UINT16 FieldbusStatus;
extern WORD ParVal_ILine_mon;

extern INT32 ParVal_kom_droop;
extern INT32 ParVal_kv_droop;
extern INT16 ParVal_Lv_droop;
extern INT16 ParVal_Rv_droop;
extern DWORD ParVal_HW_FaultMask;
extern INT32 ParVal_Harmonic_IGainInv;
extern INT32 ParVal_IreLim_PD;
extern INT16 ParVal_PWM_frequ2;
extern INT16 ParVal_PWM_frequ_select;
extern INT32 ParVal_VP_overload;
extern INT32 ParVal_VI_overload;

extern DWORD ParVal_DMA_ReadAddress;
extern WORD ParVal_DMA_ReadData;
extern WORD ParVal_DMA_WriteMode;
extern DWORD ParVal_DMA_WriteAddress;
extern WORD ParVal_DMA_WriteData;
extern DWORD ParVal_RAM_test_error;
extern WORD ParVal_PQcurrSrc;
extern INT32 ParVal_CosPhiInv;
extern WORD ParVal_CB_mode;
extern INT16 ParVal_Tigbt_max;

extern INT16 ParVal_kQ_FRT;
extern INT16 ParVal_ut_FRT;
extern INT16 ParVal_vFaultStart;
extern INT16 ParVal_vFaultEnd;
extern INT32 ParVal_iN_FRT;
extern INT16 ParVal_imax_LVRT;
extern INT16 ParVal_imax_HVRT;
extern INT16 ParVal_imax_asym;
extern INT16 ParVal_Vlim_asym;
extern INT16 ParVal_vFaultStartHV;
extern INT16 ParVal_vFaultEndHV;
extern INT16 ParVal_tLV1;
extern INT16 ParVal_tLV2;
extern INT16 ParVal_tLV3;
extern INT16 ParVal_tLV4;
extern INT16 ParVal_tLV5;
extern INT16 ParVal_tLV6;
extern INT16 ParVal_vLV1;
extern INT16 ParVal_vLV2;
extern INT16 ParVal_vLV3;
extern INT16 ParVal_vLV4;
extern INT16 ParVal_vLV5;
extern INT16 ParVal_vLV6;
extern INT16 ParVal_tHV1;
extern INT16 ParVal_tHV2;
extern INT16 ParVal_tHV3;
extern INT16 ParVal_tHV4;
extern INT16 ParVal_tHV5;
extern INT16 ParVal_tHV6;
extern INT16 ParVal_vHV1;
extern INT16 ParVal_vHV2;
extern INT16 ParVal_vHV3;
extern INT16 ParVal_vHV4;
extern INT16 ParVal_vHV5;
extern INT16 ParVal_vHV6;

extern INT16 ParVal_iq_FRT;
extern INT16 ParVal_id_limit;
extern INT16 ParVal_Plimit_F;
extern INT16 ParVal_Pref_ramp;
extern INT16 ParVal_Qref_ramp;
extern INT16 ParVal_vLpd;
extern INT16 ParVal_vLpq;
extern INT16 ParVal_vLp_amp;
extern INT16 ParVal_vLnd;
extern INT16 ParVal_vLnq;
extern INT16 ParVal_vLn_amp;
extern INT16 ParVal_vLcompd;
extern INT16 ParVal_vLcompq;
extern INT16 ParVal_VL_min;
extern INT16 ParVal_VL_max;

extern INT16 ParVal_vPV1;
extern INT16 ParVal_vPV2;
extern INT16 ParVal_vPV3;
extern INT16 ParVal_iPV1;
extern INT16 ParVal_iPV2;
extern INT16 ParVal_iPV3;
extern INT32 ParVal_V_PV1;
extern INT32 ParVal_V_PV2;
extern INT32 ParVal_V_PV3;
extern INT32 ParVal_I_PV1;
extern INT32 ParVal_I_PV2;
extern INT32 ParVal_I_PV3;
extern INT32 ParVal_P_DC;
extern INT16 ParVal_ia1;
extern INT16 ParVal_ia2;
extern INT16 ParVal_ia3;
extern INT16 ParVal_ib1;
extern INT16 ParVal_ib2;
extern INT16 ParVal_ib3;
extern INT16 ParVal_ic1;
extern INT16 ParVal_ic2;
extern INT16 ParVal_ic3;
extern INT16 ParVal_Vdc_min_on;
extern INT16 ParVal_ItripAsym;

extern UINT8 ParVal_Event[NO_EVENTPARS][MAX_EVENTSTRING_LEN];

extern INT16 ParVal_WaterTempMinWarn;
extern INT16 ParVal_WaterTempMaxWarn;
extern INT16 ParVal_WaterTempMaxFault;
extern INT16 ParVal_WaterTempHeatexOn;
extern INT16 ParVal_WaterTempHeatexOff;
extern INT16 ParVal_TempHyst;
extern INT16 ParVal_AirTempMaxWarn;
extern INT16 ParVal_AirTempMaxFault;
extern INT16 ParVal_FlowMinWarn;
extern INT16 ParVal_FlowMinFault;
extern INT16 ParVal_FlowMaxWarn;
extern INT16 ParVal_FlowMaxFault;
extern INT16 ParVal_FlowHyst;
extern INT16 ParVal_WaterInTemp;
extern INT16 ParVal_WaterOutTemp;
extern INT16 ParVal_Air1Temp;
extern INT16 ParVal_Air2Temp;
extern INT16 ParVal_PressureIn;
extern INT16 ParVal_PressureOut;
extern INT16 ParVal_CoolingFlow;
extern INT16 ParVal_Tf_Qref;

#endif	// PARTABLE_H
