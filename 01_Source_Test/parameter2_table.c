/* *****************************************************************************
 * File:   parameter2_table.c
 * Author: Dimitar Lilov
 *
 * Created on 2022 11 01 12:51
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
#include "parameter2_table.h"
#include "parameter_common.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
typedef struct
{
 INT16 ParVal_FNline;
 INT16 ParVal_VNline;
 INT32 ParVal_RangeIL;
 INT16 ParVal_PhiVmes;
 INT16 ParVal_RangeVL;
 INT32 ParVal_Islim;
 INT16 ParVal_Vr0;
 INT32 ParVal_VP_M;
 INT32 ParVal_VI_M;

 INT32 ParVal_INinv;
 INT16 ParVal_Ldr;
 INT16 ParVal_Ldr_trans;
 INT16 ParVal_Rdr;
 INT32 ParVal_RangeI;
 INT16 ParVal_PhiImes;
 INT16 ParVal_Vdc_min_on;
 INT16 ParVal_Rs_S;
 INT16 ParVal_Lssig_S;

 INT16 ParVal_polp;
 INT16 ParVal_p_IGR;
 INT32 ParVal_MN;

 INT32 ParVal_IreRef;
 INT32 ParVal_VI_phi;
 INT32 ParVal_VP_phi;
 WORD ParVal_QMode;
 INT16 ParVal_Tf_Mref;
 WORD ParVal_RefSrc;
 UINT16 ParVal_TimeoutSyncAmp;
 UINT16 ParVal_TimeoutSyncPhase;

 WORD ParVal_StartStopIn;
 WORD ParVal_PWMenableIn;
 WORD ParVal_CtrlSrc;
 WORD ParVal_FaultRelIn;
 INT16 ParVal_ForceDOs;

 INT16 ParVal_F_ov;
 INT16 ParVal_Toverload;
 INT16 ParVal_Tpause;
 INT16 ParVal_Itrip;
 INT32 ParVal_VP_overload;
 INT32 ParVal_VI_overload;
 INT16 ParVal_ItripAsym;

 INT16 ParVal_Testmode;
 INT16 ParVal_TestRef1;
 INT16 ParVal_TestRef2;
 INT16 ParVal_FrequRef;
 INT16 ParVal_SyncTest;

 INT32 ParVal_VI_u;
 INT32 ParVal_VP_u;
 INT16 ParVal_VoltageMode;
 INT32 ParVal_VI_i;
 INT32 ParVal_VP_i;
 INT32 ParVal_VI_ipi;
 INT32 ParVal_VP_ipi;
 INT32 ParVal_VI_ini;
 INT32 ParVal_VP_ini;
 INT32 ParVal_aM;
 INT32 ParVal_VIi;
 INT16 ParVal_Ifb_sel;
 INT32 ParVal_Ktrim_vffwp;
 INT32 ParVal_Ktrim_vffwn_Prot;
 INT32 ParVal_Ktrim_vffwn_comp;
 INT32 ParVal_Ktrim_vffw0_comp;
 INT32 ParVal_k_iref_pn;
 INT32 ParVal_k_iref_np;
 INT16 ParVal_Tf_inref;
 INT16 ParVal_Tf_isref;

 INT16 ParVal_PWM_mode0;
 INT16 ParVal_PWM_Td;

 INT16 ParVal_OffsSelectIinv;
 INT16 ParVal_OffsSelectIline;
 INT16 ParVal_OffsInvSaveU;
 INT16 ParVal_OffsInvSaveV;
 INT16 ParVal_OffsInvSaveW;
 INT16 ParVal_OffsLineSaveU;
 INT16 ParVal_OffsLineSaveV;
 INT16 ParVal_OffsLineSaveW;

 INT32 ParVal_D_notch;
 UINT16 ParVal_FRT_first_period;
 INT16 ParVal_FRT_Ii_limit_sym;
 INT16 ParVal_FRT_Ii_limit_asym;
 INT16 ParVal_FRT_IdLimAsym;
} dt_CPU1toCPU2_Pars;


typedef struct
{
 INT32 ParVal_Vline;
 INT32 ParVal_Fline;
 INT32 ParVal_Iinv;
 INT32 ParVal_ILine;
 INT32 ParVal_CosPhiInv;
 INT32 ParVal_Pinv;
 INT32 ParVal_Qinv;
 INT32 ParVal_Vinv;
 WORD ParVal_ReadyRunFault;
 INT16 ParVal_ConvState;
 INT16 ParVal_InvCtrlState;
 INT32 ParVal_Ps;
 INT32 ParVal_Qs;
 DWORD ParVal_DigIn;
 WORD ParVal_DigOut;

 INT16 ParVal_Parameter_FaultCode;
 INT16 ParVal_StateFaultCode;
 DWORD ParVal_HW_FaultCode;
 DWORD ParVal_SW_FaultCode;

 INT16 ParVal_i16_Test1;
 INT16 ParVal_i16_Test2;
 INT16 ParVal_i16_Test3;
 INT16 ParVal_i16_Test4;
 INT16 ParVal_i16_Test5;
 INT16 ParVal_i16_Test6;
 INT16 ParVal_i16_Test7;
 INT16 ParVal_i16_Test8;
 INT16 ParVal_i16_Test9;
 INT16 ParVal_i16_Test10;
 INT16 ParVal_i16_Test11;
 INT16 ParVal_i16_Test12;
 INT16 ParVal_i16_Test13;

 INT16 ParVal_vLuv;
 INT16 ParVal_vLwv;
 INT16 ParVal_vLu;
 INT16 ParVal_vLv;
 INT16 ParVal_vinvu;
 INT16 ParVal_vinvv;
 INT16 ParVal_vinv_d;
 INT16 ParVal_vinv_q;
 INT16 ParVal_vLd;
 INT16 ParVal_vLq;
 INT16 ParVal_vLamp;
 INT16 ParVal_iInvu;
 INT16 ParVal_iInvv;
 INT16 ParVal_iInvw;
 INT16 ParVal_iInvd;
 INT16 ParVal_iInvq;
 INT16 ParVal_iInvamp;
 INT16 ParVal_iInvp_amp;
 INT16 ParVal_iInvpd;
 INT16 ParVal_iInvpq;
 INT16 ParVal_iLu;
 INT16 ParVal_iLv;
 INT16 ParVal_iLw;
 INT16 ParVal_iL_al;
 INT16 ParVal_iL_be;
 INT16 ParVal_iLd;
 INT16 ParVal_iLq;
 INT16 ParVal_iLdav;
 INT16 ParVal_iLqav;
 INT16 ParVal_iLamp;
 INT16 ParVal_fLine;
 INT16 ParVal_thLine;
 INT16 ParVal_iLqref;
 INT16 ParVal_idref;
 INT16 ParVal_iqref;
 INT16 ParVal_udref;
 INT16 ParVal_uqref;
 INT16 ParVal_udIrefp;
 INT16 ParVal_uqIrefp;
 INT16 ParVal_udIrefn;
 INT16 ParVal_uqIrefn;
 INT16 ParVal_ia1;
 INT16 ParVal_ia2;
 INT16 ParVal_ia3;
 INT16 ParVal_ib1;
 INT16 ParVal_ib2;
 INT16 ParVal_ib3;
 INT16 ParVal_ic1;
 INT16 ParVal_ic2;
 INT16 ParVal_ic3;
 INT16 ParVal_vLpd;
 INT16 ParVal_vLpq;
 INT16 ParVal_vLp_amp;
 INT16 ParVal_vLnd;
 INT16 ParVal_vLnq;
 INT16 ParVal_vLn_amp;
 INT16 ParVal_vpcompd;
 INT16 ParVal_vpcompq;
 INT16 ParVal_vncompd;
 INT16 ParVal_vncompq;
 INT16 ParVal_v0compd;
 INT16 ParVal_v0compq;
 INT16 ParVal_vncompd_Prot;
 INT16 ParVal_vncompq_Prot;
 INT16 ParVal_Imax;
 INT16 ParVal_iInv_al;
 INT16 ParVal_iInv_be;
 INT16 ParVal_vinv_amp;
 INT16 ParVal_iLdref;
 INT16 ParVal_thIGR;
 INT16 ParVal_thm;
 INT16 ParVal_ommTi;
 INT16 ParVal_omm;
 INT16 ParVal_rpm;
 INT16 ParVal_speed_rpm_fil;
 INT16 ParVal_iInvn_amp;
 INT16 ParVal_iInvnd;
 INT16 ParVal_iInvnq;
 INT16 ParVal_iInv0d;
 INT16 ParVal_iInv0q;
 INT16 ParVal_isp_amp;
 INT16 ParVal_isn_amp;
 INT16 ParVal_isnd;
 INT16 ParVal_isnq;
 INT16 ParVal_psisn_amp;
 INT16 ParVal_psisnd;
 INT16 ParVal_psisnq;
 INT16 ParVal_psisp_amp;
 INT16 ParVal_psispd;
 INT16 ParVal_psispq;
 INT16 ParVal_psis_amp;
 INT16 ParVal_psisd;
 INT16 ParVal_psisq;
 INT16 ParVal_psis0d;
 INT16 ParVal_psis0q;
 INT16 ParVal_torque;
 INT32 ParVal_Torque_Nm;
 INT16 ParVal_iInv_fb_pd;
 INT16 ParVal_iInv_fb_pq;
 INT16 ParVal_iInv_fb_nd;
 INT16 ParVal_iInv_fb_nq;
 INT16 ParVal_idref_tqref1;
 INT16 ParVal_idref_tqref2;
 INT16 ParVal_idref_tqreg;
 INT16 ParVal_irq_min;
 INT16 ParVal_iLrefnd;
 INT16 ParVal_iLrefnq;
 INT16 ParVal_indref;
 INT16 ParVal_inqref;
 INT16 ParVal_prot;
 INT16 ParVal_qrot;

 INT16 ParVal_AnIn0;
 INT16 ParVal_AnIn1;
 INT16 ParVal_AnIn2;
 INT16 ParVal_AnIn3;
 INT16 ParVal_AnIn4;
 INT16 ParVal_AnIn5;
 INT16 ParVal_AnIn6;
 INT16 ParVal_AnIn7;
 INT16 ParVal_AnIn8;
 INT16 ParVal_AnIn9;
 INT16 ParVal_AnIn10;
 INT16 ParVal_AnIn11;
 INT16 ParVal_AnIn12;
 INT16 ParVal_AnIn13;
 INT16 ParVal_AnIn14;
 INT16 ParVal_AnIn15;
 INT16 ParVal_AnIn16;
 INT16 ParVal_AnIn17;
 INT16 ParVal_AnIn18;
 INT16 ParVal_AnIn19;
 INT16 ParVal_AnIn20;
 INT16 ParVal_AnIn21;

 INT16 ParVal_OffsInvActU;
 INT16 ParVal_OffsInvActV;
 INT16 ParVal_OffsInvActW;
 INT16 ParVal_OffsLineActU;
 INT16 ParVal_OffsLineActV;
 INT16 ParVal_OffsLineActW;

 INT16 ParVal_vLcompd;  // obsolete
 INT16 ParVal_vLcompq;  // obsolete
} dt_CPU2toCPU1_Pars;

typedef struct
{
    UINT16 group;
    UINT16 event;
    UINT16 info;
    UINT16 valid;
}
EV_LIST2_T;

typedef struct
{
   UINT16 ParamInitDone;
   UINT16 MasterFault;
   UINT16 PWM_period;
   UINT16 Fault_reset;
   UINT16 FRT_active;
   UINT16 State_LSI;
   UINT16 MasterCmdOn;
   INT32 V_DC;
   INT32 th_PLL;
   INT32 om_PLL;
   INT32 VLpd;
   INT32 VLpq;
   INT32 VLp_amp;
   INT32 VLnd;
   INT32 VLnq;
   INT32 VLn_amp;
   INT32 VL0d;
   INT32 VL0q;
   INT32 VL0_amp;
   INT32 Mref;
   INT32 ispq_ref;
   INT16 Vdc_max;
   INT32 ispd_max_n;
   INT32 isnd_ref;
   INT32 isnq_ref;
   INT16 IN_LSI;
   UINT32 FRT_cnt_dip;
   UINT16 AsymGrid;
   INT16 Vdc_tripp;
   INT16 Vdc_tripm;
   UINT16 FRT_InDip;
   INT32 dvLine;
} dt_CPU1toCPU2_Data;

typedef struct
{
   UINT16 BootFinished;
   UINT16 State_GSI;
   UINT32 SlaveFault;
   INT32 speed_rpm;
   INT32 Vref_unlim;
   UINT16 OST;
   INT32 p_rotor;
   INT32 q_rotor;
} dt_CPU2toCPU1_Data;



/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */

dt_CPU1toCPU2_Pars  CPU1toCPU2_Pars;
dt_CPU1toCPU2_Data  CPU1toCPU2_Data;
dt_CPU2toCPU1_Pars  CPU2toCPU1_Pars;
dt_CPU2toCPU1_Data  CPU2toCPU1_Data;

UINT16 eventlist2_index = 0;
EV_LIST2_T Eventlist2[NO_EVENTPARS];


PAR_DES_I16_T  ParDes2_Overload =
{
    PAR_PASS_PW | PAR_MENUE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "2: Überlast",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Overload",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_Overload*/,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T  ParDes2_F_ov =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "2: Überlastfaktor",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Overload ratio",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    10000,                                              // Maximum
    1000,                                                               // Minimum
    1000,                                                               // Default
    0.001,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_F_ov,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T  ParDes2_FRT_IdLimAsym =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
    "2: Id limit asym FRT",                             // Name
    NULL,                                                           // Einheit
    10000,                                              // Maximum
    0,                                                              // Minimum
    1000,                                                               // Default
    0.001,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_FRT_IdLimAsym,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T  ParDes2_Itrip =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "2: Überstromschwelle",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Current trip level",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    10000,                                              // Maximum
    1000,                                                               // Minimum
    1250,                                                               // Default
    0.001,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_Itrip,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};



PAR_DES_I16_T  ParDes2_Toverload =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "2: Überlastzeit",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Overload time",                             // Name
#endif
#endif
    "s",                                                            // Einheit
    30000,                                              // Maximum
    1,                                                              // Minimum
    30,                                                             // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_Toverload,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T  ParDes2_Tpause =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "2: Pausenzeit",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Resting time",                              // Name
#endif
#endif
    "s",                                                            // Einheit
    30000,                                              // Maximum
    1,                                                              // Minimum
    30,                                                             // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_Tpause,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Netz
// Parameter-Beschreibung Grid
PAR_DES_I16_T  ParDes2_Grid =
{
    PAR_MENUE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "2: Generator",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Generator",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_Grid*/,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable VNline

// Netz-Nennsspannung (L-L), in V
// Parameter-Beschreibung VNline
PAR_DES_I16_T  ParDes2_VNline =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "2: Nennspannung",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Rated voltage",                             // Name
#endif
#endif
    "V",                                                            // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    400,                                                                // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_VNline,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T  ParDes2_polp =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "2: Polpaarzahl",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Pole pairs",                                // Name
#endif
#endif
    "",                                                         // Einheit
    10,                                             // Maximum
    1,                                                              // Minimum
    2,                                                              // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_polp,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T  ParDes2_p_IGR =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "2: Encoderpulse",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Encoder pulse number",                              // Name
#endif
#endif
    "",                                                         // Einheit
    10000,                                              // Maximum
    1,                                                              // Minimum
    1024,                                                               // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_p_IGR,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T  ParDes2_Vr0 =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "2: Leerlaufspannung Rotor",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Rotor open-loop voltage",                               // Name
#endif
#endif
    "V",                                                            // Einheit
    10000,                                              // Maximum
    0,                                                              // Minimum
    400,                                                                // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_Vr0,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable PhiVmes

PAR_DES_I16_T  ParDes2_PhiVmes =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "2: Umes Phase",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Vmes phase shift",                              // Name
#endif
#endif
    "deg",                                                          // Einheit
    1800,                                               // Maximum
    -1800,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_PhiVmes,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable PhiImes

PAR_DES_I16_T  ParDes2_PhiImes =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "2: Imes Phase",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Imes phase shift",                              // Name
#endif
#endif
    "deg",                                                          // Einheit
    1800,                                               // Maximum
    -1800,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_PhiImes,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable RangeVL

PAR_DES_I16_T  ParDes2_RangeVL =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "2: Messbereich Statorspannung",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Stator voltage range",                              // Name
#endif
#endif
    "V",                                                            // Einheit
    4000,                                               // Maximum
    -4000,                                                              // Minimum
    1200,                                                               // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_RangeVL,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};



// Parameter-Variable FNline

// Netz-Nennfrequenz, in Hz
// Parameter-Beschreibung FNline
PAR_DES_I16_T  ParDes2_FNline =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Nennfrequenz",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Rated frequency",                               // Name
#endif
#endif
    "Hz",                                                           // Einheit
    100,                                                // Maximum
    0,                                                              // Minimum
    50,                                                             // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_FNline,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Umrichter
// Parameter-Beschreibung Inverter
PAR_DES_I16_T  ParDes2_Inverter =
{
    PAR_MENUE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "2: Umrichter",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Inverter",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_Inverter*/,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable INinv

// Umrichter-Nennstrom, in A
// Parameter-Beschreibung INinv
PAR_DES_I32_T  ParDes2_INinv =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,
#ifdef LANGUAGE_GERMAN
    "2: Umrichter-Nennstrom",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Rated inverter current",                                // Name
#endif
#endif
    "A",                                                            // Einheit
    300000,                                             // Maximum
    0,                                                              // Minimum
    2150,                                                               // Default
    0.1,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_INinv,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I32_T  ParDes2_Islim =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,
#ifdef LANGUAGE_GERMAN
    "2: Max. Statorstrom",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Max. stator current",                               // Name
#endif
#endif
    "A",                                                            // Einheit
    300000,                                             // Maximum
    0,                                                              // Minimum
    2150,                                                               // Default
    0.1,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_Islim,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable I_StandbyOn
//#pragma DATA_SECTION(ParVal_I_StandbyOn,".extram");
INT16 ParVal_I_StandbyOn;

PAR_DES_I16_T  ParDes2_I_StandbyOn =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "2: Standby-Ein Laststrom",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Standby ON load current",                               // Name
#endif
#endif
    "A",                                                            // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &ParVal_I_StandbyOn,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};



// Parameter-Variable RangeI

// Messbereich Umrichterstrom, in A
// Parameter-Beschreibung RangeI
PAR_DES_I32_T  ParDes2_RangeI =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,
#ifdef LANGUAGE_GERMAN
    "2: Messbereich Umrichterstrom",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Inverter curent range",                             // Name
#endif
#endif
    "A",                                                            // Einheit
    100000,                                             // Maximum
    -100000,                                                                // Minimum
    5000,                                                               // Default
    0.1,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_RangeI,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable RangeIL

// Messbereich Netzstrom, in A
// Parameter-Beschreibung RangeIL
PAR_DES_I32_T  ParDes2_RangeIL =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,
#ifdef LANGUAGE_GERMAN
    "2: Messbereich Statorstrom",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Stator curent range",                               // Name
#endif
#endif
    "A",                                                            // Einheit
    200000,                                             // Maximum
    -200000,                                                                // Minimum
    12200,                                                              // Default
    0.1,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_RangeIL,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_Ldr =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Lm (Stator)",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Lm (stator)",                               // Name
#endif
#endif
    "uH",                                                           // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    508,                                                                // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_Ldr,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_Lssig_S =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Lsigma (Stator)",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Lsigma (stator)",                               // Name
#endif
#endif
    "uH",                                                           // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    508,                                                                // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_Lssig_S,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I32_T  ParDes2_MN =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                 // ReadOnly
    "2: Rated torque",                              // Name
    "Nm",                                                           // Einheit
    100000,                                             // Maximum
    10,                                                             // Minimum
    10000,                                                              // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_MN,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Ldr_trans

// Line choke transient inductance, in myH
// Parameter-Beschreibung Ldr_trans
PAR_DES_I16_T  ParDes2_Ldr_trans =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Lsigma (rotor)",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Lsigma (rotor)",                                // Name
#endif
#endif
    "uH",                                                           // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    508,                                                                // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_Ldr_trans,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Rdr

// Line choke resistance, in mOhm
// Parameter-Beschreibung Rdr
PAR_DES_I16_T  ParDes2_Rdr =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Rs+Rr (rotor)",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Rs+Rr (rotor)",                             // Name
#endif
#endif
    "mOhm",                                                         // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    10,                                                             // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_Rdr,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T  ParDes2_Rs_S =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: R Stator",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Rs (stator)",                               // Name
#endif
#endif
    "mOhm",                                                         // Einheit
    30000,                                              // Maximum
    0,                                                              // Minimum
    10,                                                             // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_Rs_S,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};



// Parameter-Variable ParVal_Testmode
PAR_DES_I16_T  ParDes2_Testmode =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "2: Testmode",                          // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "2: Testmode",                          // Name
    #endif
    #endif
    NULL,                                           // Einheit
    6,                                  // Max
    0,                              // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &CPU1toCPU2_Pars.ParVal_Testmode,               // Parameter
    NULL                                            // Behandlungsfunktion
};


PAR_DES_I16_T  ParDes2_SyncTest =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                   // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "2: SyncTest",                          // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "2: SyncTest",                          // Name
    #endif
    #endif
    NULL,                                           // Einheit
    1,                                  // Max
    0,                              // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &CPU1toCPU2_Pars.ParVal_SyncTest,               // Parameter
    NULL                                            // Behandlungsfunktion
};


// Parameter-Variable ParVal_i16_Test1
// Parameter-Variable  ParDes2_i16_Test1
PAR_DES_I16_T  ParDes2_i16_Test1 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "2: i16_Test1",                         // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "2: i16_Test1",                         // Name
    #endif
    #endif
    NULL,                                           // Einheit
    MAX_INT,                                    // Max
    -MAX_INT,                               // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &CPU2toCPU1_Pars.ParVal_i16_Test1,              // Parameter
    NULL                                            // Behandlungsfunktion
};

// Parameter-Variable ParVal_i16_Test2
// Parameter-Variable  ParDes2_i16_Test2
PAR_DES_I16_T  ParDes2_i16_Test2 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "2: i16_Test2",                         // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "2: i16_Test2",                         // Name
    #endif
    #endif
    NULL,                                           // Einheit
    MAX_INT,                                    // Max
    -MAX_INT,                               // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &CPU2toCPU1_Pars.ParVal_i16_Test2,              // Parameter
    NULL                                            // Behandlungsfunktion
};

// Parameter-Variable ParVal_i16_Test3
// Parameter-Variable  ParDes2_i16_Test3
PAR_DES_I16_T  ParDes2_i16_Test3 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "2: i16_Test3",                         // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "2: i16_Test3",                         // Name
    #endif
    #endif
    NULL,                                           // Einheit
    MAX_INT,                                    // Max
    -MAX_INT,                               // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &CPU2toCPU1_Pars.ParVal_i16_Test3,              // Parameter
    NULL                                            // Behandlungsfunktion
};

// Parameter-Variable ParVal_i16_Test4
// Parameter-Variable  ParDes2_i16_Test4
PAR_DES_I16_T  ParDes2_i16_Test4 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "2: i16_Test4",                         // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "2: i16_Test4",                         // Name
    #endif
    #endif
    NULL,                                           // Einheit
    MAX_INT,                                    // Max
    -MAX_INT,                               // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &CPU2toCPU1_Pars.ParVal_i16_Test4,              // Parameter
    NULL                                            // Behandlungsfunktion
};

// Parameter-Variable ParVal_i16_Test5
// Parameter-Variable  ParDes2_i16_Test5
PAR_DES_I16_T  ParDes2_i16_Test5 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "2: i16_Test5",                         // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "2: i16_Test5",                         // Name
    #endif
    #endif
    NULL,                                           // Einheit
    MAX_INT,                                    // Max
    -MAX_INT,                               // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &CPU2toCPU1_Pars.ParVal_i16_Test5,              // Parameter
    NULL                                            // Behandlungsfunktion
};

// Parameter-Variable ParVal_i16_Test6
// Parameter-Variable  ParDes2_i16_Test6
PAR_DES_I16_T  ParDes2_i16_Test6 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "2: i16_Test6",                         // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "2: i16_Test6",                         // Name
    #endif
    #endif
    NULL,                                           // Einheit
    MAX_INT,                                    // Max
    -MAX_INT,                               // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &CPU2toCPU1_Pars.ParVal_i16_Test6,              // Parameter
    NULL                                            // Behandlungsfunktion
};

// Parameter-Variable ParVal_i16_Test7
// Parameter-Variable  ParDes2_i16_Test7
PAR_DES_I16_T  ParDes2_i16_Test7 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "2: i16_Test7",                         // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "2: i16_Test7",                         // Name
    #endif
    #endif
    NULL,                                           // Einheit
    MAX_INT,                                    // Max
    -MAX_INT,                               // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &CPU2toCPU1_Pars.ParVal_i16_Test7,              // Parameter
    NULL                                            // Behandlungsfunktion
};

// Parameter-Variable ParVal_i16_Test8
// Parameter-Variable  ParDes2_i16_Test8
PAR_DES_I16_T  ParDes2_i16_Test8 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "2: i16_Test8",                         // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "2: i16_Test8",                         // Name
    #endif
    #endif
    NULL,                                           // Einheit
    MAX_INT,                                    // Max
    -MAX_INT,                               // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &CPU2toCPU1_Pars.ParVal_i16_Test8,              // Parameter
    NULL                                            // Behandlungsfunktion
};

// Parameter-Variable ParVal_i32_Test9
// Parameter-Variable  ParDes2_i32_Test9
PAR_DES_I16_T  ParDes2_i16_Test9 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "2: i16_Test9",                         // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "2: i16_Test9",                         // Name
    #endif
    #endif
    NULL,                                           // Einheit
    MAX_INT,                                    // Max
    -MAX_INT,                               // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &CPU2toCPU1_Pars.ParVal_i16_Test9,              // Parameter
    NULL                                            // Behandlungsfunktion
};

// Parameter-Variable ParVal_i32_Test10
// Parameter-Variable  ParDes2_i32_Test10
PAR_DES_I16_T  ParDes2_i16_Test10 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "2: i16_Test10",                            // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "2: i16_Test10",                            // Name
    #endif
    #endif
    NULL,                                           // Einheit
    MAX_INT,                                    // Max
    -MAX_INT,                               // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &CPU2toCPU1_Pars.ParVal_i16_Test10,             // Parameter
    NULL                                            // Behandlungsfunktion
};

// Parameter-Variable ParVal_i32_Test11
// Parameter-Variable  ParDes2_i32_Test11
PAR_DES_I16_T  ParDes2_i16_Test11 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "2: i16_Test11",                            // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "2: i16_Test11",                            // Name
    #endif
    #endif
    NULL,                                           // Einheit
    MAX_INT,                                    // Max
    -MAX_INT,                               // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &CPU2toCPU1_Pars.ParVal_i16_Test11,             // Parameter
    NULL                                            // Behandlungsfunktion
};

// Parameter-Variable ParVal_i16_Test12
// Parameter-Variable  ParDes2_i16_Test12
PAR_DES_I16_T  ParDes2_i16_Test12 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "2: i16_Test12",                            // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "2: i16_Test12",                            // Name
    #endif
    #endif
    NULL,                                           // Einheit
    MAX_INT,                                    // Max
    -MAX_INT,                               // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &CPU2toCPU1_Pars.ParVal_i16_Test12,             // Parameter
    NULL                                            // Behandlungsfunktion
};


// Parameter-Variable Testvars
//#pragma DATA_SECTION(ParVal_Testvars,".extram");
//INT16 ParVal_Testvars;
// Parameter-Variable  ParDes2_Testvars
PAR_DES_I16_T  ParDes2_Testvars =
{
    PAR_MENUE | PAR_RW | _INT16,                    // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "2: Testvariablen",                         // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "2: Test variables",                            // Name
    #endif
    #endif
    NULL,                                           // Einheit
    1,                                  // Max
    0,                              // Min
    0,                                              // Default
    1.0,                                            // Faktor
    NULL/*&ParVal_Testvars*/,               // Parameter
    NULL                                            // Behandlungsfunktion
};


// Parameter-Variable ParVal_i16_Test13
// Parameter-Variable  ParDes2_i16_Test13
PAR_DES_I16_T  ParDes2_i16_Test13 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                  // ReadWrite, INT16-Parameter
    #ifdef LANGUAGE_GERMAN
    "2: i16_Test13",                            // Name
    #else
    #ifdef LANGUAGE_ENGLISH
    "2: i16_Test13",                            // Name
    #endif
    #endif
    NULL,                                           // Einheit
    MAX_INT,                                    // Max
    -MAX_INT,                               // Min
    0,                                              // Default
    1.0,                                            // Faktor
    &CPU2toCPU1_Pars.ParVal_i16_Test13,             // Parameter
    NULL                                            // Behandlungsfunktion
};


// Parameter-Variable Feedbacks
//#pragma DATA_SECTION(ParVal_Feedbacks,".extram");
//INT16 ParVal_Feedbacks;

// Istwerte
// Parameter-Beschreibung Feedbacks
PAR_DES_I16_T  ParDes2_Feedbacks =
{
    PAR_MENUE | PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Istwerte",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Feedbacks",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_Feedbacks*/,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Vline

// Istwert Netzsspannung (L-L), in V
// Parameter-Beschreibung Vline
PAR_DES_I32_T  ParDes2_Vline =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Statorspannung",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Stator voltage",                                // Name
#endif
#endif
    "V",                                                            // Einheit
    10000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &CPU2toCPU1_Pars.ParVal_Vline,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};



PAR_DES_I32_T  ParDes2_Vinv =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Umrichterspannung",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Inverter voltage",                              // Name
#endif
#endif
    "V",                                                            // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU2toCPU1_Pars.ParVal_Vinv,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};



// Parameter-Variable Fline

// Istwert Netzfrequenz, in 0.1 Hz
// Parameter-Beschreibung Fline
PAR_DES_I32_T  ParDes2_Fline =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Rotorfrequenz",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Rotor frequency",                               // Name
#endif
#endif
    "Hz",                                                           // Einheit
    10000000,                                               // Maximum
    -10000000,                                                              // Minimum
    0,                                                              // Default
    0.01,                     // Faktor
    &CPU2toCPU1_Pars.ParVal_Fline,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Iinv

// Istwert Umrichterstrom, in A
// Parameter-Beschreibung Iinv
PAR_DES_I32_T  ParDes2_Iinv =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "Ausgangsstrom",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Inverter current",                              // Name
#endif
#endif
    "A",                                                            // Einheit
    500000,                                             // Maximum
    -500,                                                               // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &CPU2toCPU1_Pars.ParVal_Iinv,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};



// Parameter-Variable Pinv

// Istwert Wirkleistung, in kW
// Parameter-Beschreibung Pinv
PAR_DES_I32_T  ParDes2_Pinv =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Wirkleistung",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Active power GSI",                              // Name
#endif
#endif
    "kW",                                                           // Einheit
    50000,                                              // Maximum
    -50000,                                                             // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &CPU2toCPU1_Pars.ParVal_Pinv,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Qinv

// Istwert Blindleistung, in kVAr
// Parameter-Beschreibung Qinv
PAR_DES_I32_T  ParDes2_Qinv =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Blindleistung GSI",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Reactive power GSI",                                // Name
#endif
#endif
    "kVAr",                                                         // Einheit
    50000,                                              // Maximum
    -50000,                                                             // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &CPU2toCPU1_Pars.ParVal_Qinv,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable ILine

// Istwert Netzstrom, in A
// Parameter-Beschreibung ILine
PAR_DES_I32_T  ParDes2_ILine =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Statorstrom",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Stator current",                                // Name
#endif
#endif
    "A",                                                            // Einheit
    500000,                                             // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &CPU2toCPU1_Pars.ParVal_ILine,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Line power factor
// Parameter-Beschreibung CosPhiInv
PAR_DES_I32_T  ParDes2_CosPhiInv =
{
    PAR_RO | _INT32,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Cos(phi) Umrichter",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Cos(phi) inverter",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    65536,                                              // Maximum
    -65536,                                                             // Minimum
    0,                                                              // Default
    (1.0/65536),                      // Faktor
    &CPU2toCPU1_Pars.ParVal_CosPhiInv,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


//#pragma DATA_SECTION(ParVal_PWM,".extram");
//INT16 ParVal_PWM;
// Parameter-Beschreibung AnInputs
PAR_DES_I16_T  ParDes2_PWM =
{
    PAR_MENUE | PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: PWM/Betriebssystem",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: PWM/Opsys",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_PWM*/,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Analog Inputs

// Parameter-Variable AnInputs
//#pragma DATA_SECTION(ParVal_AnInputs,".extram");
//INT16 ParVal_AnInputs;
// Parameter-Beschreibung AnInputs
PAR_DES_I16_T  ParDes2_AnInputs =
{
    PAR_MENUE | PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Analoge Eingänge",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Analog inputs",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_AnInputs*/,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Analog Inputs
PAR_DES_I16_T  ParDes2_AnIn0 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Analogeingang 0",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Analog Input 0",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU2toCPU1_Pars.ParVal_AnIn0,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_AnIn1 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Analogeingang 1",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Analog Input 1",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU2toCPU1_Pars.ParVal_AnIn1,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_AnIn2 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Analogeingang 2",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Analog Input 2",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU2toCPU1_Pars.ParVal_AnIn2,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_AnIn3 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Analogeingang 3",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Analog Input 3",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU2toCPU1_Pars.ParVal_AnIn3,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_AnIn4 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Analogeingang 4",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Analog Input 4",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU2toCPU1_Pars.ParVal_AnIn4,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_AnIn5 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Analogeingang 5",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Analog Input 5",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU2toCPU1_Pars.ParVal_AnIn5,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_AnIn6 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Analogeingang 6",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Analog Input 6",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU2toCPU1_Pars.ParVal_AnIn6,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_AnIn7 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Analogeingang 7",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Analog Input 7",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU2toCPU1_Pars.ParVal_AnIn7,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_AnIn8 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Analogeingang 8",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Analog Input 8",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU2toCPU1_Pars.ParVal_AnIn8,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_AnIn9 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Analogeingang 9",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Analog Input 9",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU2toCPU1_Pars.ParVal_AnIn9,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_AnIn10 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Analogeingang 10",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Analog Input 10",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU2toCPU1_Pars.ParVal_AnIn10,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_AnIn11 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Analogeingang 11",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Analog Input 11",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU2toCPU1_Pars.ParVal_AnIn11,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_AnIn12 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Analogeingang 12",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Analog Input 12",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU2toCPU1_Pars.ParVal_AnIn12,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_AnIn13 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Analogeingang 13",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Analog Input 13",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU2toCPU1_Pars.ParVal_AnIn13,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_AnIn14 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Analogeingang 14",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Analog Input 14",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU2toCPU1_Pars.ParVal_AnIn14,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_AnIn15 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Analogeingang 15",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Analog Input 15",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU2toCPU1_Pars.ParVal_AnIn15,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_AnIn16 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Analogeingang 16",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Analog Input 16",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU2toCPU1_Pars.ParVal_AnIn16,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_AnIn17 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Analogeingang 17",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Analog Input 17",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU2toCPU1_Pars.ParVal_AnIn17,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_AnIn18 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Analogeingang 18",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Analog Input 18",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU2toCPU1_Pars.ParVal_AnIn18,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_AnIn19 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Analogeingang 19",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Analog Input 19",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU2toCPU1_Pars.ParVal_AnIn19,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_AnIn20 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Analogeingang 20",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Analog Input 20",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU2toCPU1_Pars.ParVal_AnIn20,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_AnIn21 =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Analogeingang 21",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Analog Input 21",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    -32767,                                                             // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU2toCPU1_Pars.ParVal_AnIn21,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable SW_FaultCode

// Software fault code
// Parameter-Beschreibung SW_FaultCode
PAR_DES_DWORD_T  ParDes2_SW_FaultCode =
{
    PAR_RO | _DWORD,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Software Fehlercode",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Software fault code",                               // Name
#endif
#endif
    0xffffffff,                                             // Maximum
    0x00000000,                                                             // Minimum
    0,                                                              // Default
    &CPU2toCPU1_Pars.ParVal_SW_FaultCode,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable HW_FaultCode

// Hardware fault code
// Parameter-Beschreibung HW_FaultCode
PAR_DES_DWORD_T  ParDes2_HW_FaultCode =
{
    PAR_RO | _DWORD,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Hardware Fehlercode",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Hardware fault code",                               // Name
#endif
#endif
    0xffffffff,                                             // Maximum
    0x00000000,                                                             // Minimum
    0,                                                              // Default
    &CPU2toCPU1_Pars.ParVal_HW_FaultCode,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Parameter_FaultCode

// Parameter fault code
PAR_DES_I16_T  ParDes2_Parameter_FaultCode =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Parameter Fehlercode",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Parameter fault code",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    32767,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU2toCPU1_Pars.ParVal_Parameter_FaultCode,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};




// Parameter-Variable Controls
//#pragma DATA_SECTION(ParVal_Controls,".extram");
//INT16 ParVal_Controls;
// Parameter-Beschreibung Controls
PAR_DES_I16_T  ParDes2_Controls =
{
    PAR_MENUE | PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Steuerung",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Controls",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1,                    // Faktor
    NULL/*&ParVal_Controls*/,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable ConvState
// Parameter-Beschreibung Converter state
PAR_DES_I16_T  ParDes2_ConvState =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Zustand Umrichter",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Converter state",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU2toCPU1_Pars.ParVal_ConvState,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable Faults
//#pragma DATA_SECTION(ParVal_Faults,".extram");
//INT16 ParVal_Faults;
// Parameter-Beschreibung Faults
PAR_DES_I16_T  ParDes2_Faults =
{
    PAR_MENUE | PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Fehler",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Faults",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1,                    // Faktor
    NULL/*&ParVal_Faults*/,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable StateFaultCode
// Parameter-Beschreibung State fault code
PAR_DES_I16_T  ParDes2_StateFaultCode =
{
    PAR_RO | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Zustandsfehlercode",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: State fault code",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU2toCPU1_Pars.ParVal_StateFaultCode,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable CtrlSrc

// Parameter-Beschreibung CtrlSrc
PAR_DES_WORD_T  ParDes2_CtrlSrc =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "2: Steuerquelle",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Control source",                                // Name
#endif
#endif
    0x0002,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &CPU1toCPU2_Pars.ParVal_CtrlSrc,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};



// Parameter-Variable CtrlSrc

// Fault reset input
// Parameter-Beschreibung CtrlSrc
PAR_DES_WORD_T  ParDes2_RefSrc =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "2: Sollwertquelle",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Reference source",                              // Name
#endif
#endif
    0x0002,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &CPU1toCPU2_Pars.ParVal_RefSrc,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable StartStopIn

// Start/Stop input
// Parameter-Beschreibung StartStopIn
PAR_DES_WORD_T  ParDes2_StartStopIn =
{
    PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "2: Start AC",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Start AC",                              // Name
#endif
#endif
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &CPU1toCPU2_Pars.ParVal_StartStopIn,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable PWMenableIn

// PWM enable input
// Parameter-Beschreibung PWMenableIn
PAR_DES_WORD_T  ParDes2_PWMenableIn =
{
    PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "2: PWM enable",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: PWM enable",                                // Name
#endif
#endif
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &CPU1toCPU2_Pars.ParVal_PWMenableIn,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};



// Parameter-Variable ReadyRunFault

// Parameter-Beschreibung ReadyRunFault
PAR_DES_WORD_T  ParDes2_ReadyRunFault =
{
    PAR_RO | _WORD,
#ifdef LANGUAGE_GERMAN
    "2: Betriebszustand",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Operation state",                               // Name
#endif
#endif
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &CPU2toCPU1_Pars.ParVal_ReadyRunFault,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable TestRef1

// Testsollwert 1
// Parameter-Beschreibung TestRef1
PAR_DES_I16_T  ParDes2_TestRef1 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                                  // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Sollwert 1",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Reference 1",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    0x7fff,                                             // Maximum
    0x8001,                                                             // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_TestRef1,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable TestRef2
// Testsollwert 2
// Parameter-Beschreibung TestRef2
PAR_DES_I16_T  ParDes2_TestRef2 =
{
    PAR_PASS_PW | PAR_RW | _INT16,                                  // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Sollwert 2",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Reference 2",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    0x7fff,                                             // Maximum
    0x8001,                                                             // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_TestRef2,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Parameter-Variable QMode

// Q Mode
// Parameter-Beschreibung QMode
PAR_DES_WORD_T  ParDes2_QMode =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "2: Modus Q",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Q Mode",                                // Name
#endif
#endif
    0x0003,                                             // Maximum
    0x0000,                                                             // Minimum
    1,                                                              // Default
    &CPU1toCPU2_Pars.ParVal_QMode,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};



PAR_DES_I32_T  ParDes2_VP_phi =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: P-Verstärkung cos(phi) control",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: P-Gain Is control",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    10.0*65536,                                             // Maximum
    -10.0*65536,                                                                // Minimum
    1.0*65536,                                                              // Default
    1.0/65536,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_VP_phi,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};



PAR_DES_I32_T  ParDes2_VI_phi =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: I-Verstärkung Is control",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: I-Gain Is control",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    0,                                                              // Minimum
    0.05*65536,                                                             // Default
    1.0/65536,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_VI_phi,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};



PAR_DES_I32_T  ParDes2_VP_overload =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: P-Verstärkung overload control",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: P-Gain overload control",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    10.0*65536,                                             // Maximum
    -10.0*65536,                                                                // Minimum
    1.0*65536,                                                              // Default
    1.0/65536,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_VP_overload,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};



PAR_DES_I32_T  ParDes2_VI_overload =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: I-Verstärkung overload control",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: I-Gain overload control",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    0,                                                              // Minimum
    0.05*65536,                                                             // Default
    1.0/65536,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_VI_overload,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I32_T  ParDes2_VP_M =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: P-Verstärkung torque control",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: P-Gain torque control",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    10.0*65536,                                             // Maximum
    -10.0*65536,                                                                // Minimum
    1.0*65536,                                                              // Default
    1.0/65536,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_VP_M,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I32_T  ParDes2_VI_M =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: I-Verstärkung torque control",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: I-Gain torque control",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    0,                                                              // Minimum
    0.05*65536,                                                             // Default
    1.0/65536,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_VI_M,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I32_T  ParDes2_VI_u =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: I-Verstärkung Vs amplitude",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: I-gain Vs amplitude",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    0,                                                              // Minimum
    0.05*65536,                                                             // Default
    1.0/65536,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_VI_u,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I32_T  ParDes2_VP_u =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: P-Verstärkung Vs amplitude",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: P-gain Vs amplitude",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    100.0*65536,                                                // Maximum
    0,                                                              // Minimum
    1.0*65536,                                                              // Default
    1.0/65536,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_VP_u,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I32_T  ParDes2_VI_i =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: I-Verstärkung Vs phase",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: I-gain Vs phase control",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    0,                                                              // Minimum
    0.05*65536,                                                             // Default
    1.0/65536,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_VI_i,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I32_T  ParDes2_VP_i =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: P-Verstärkung Vs phase",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: P-gain Vs phase control",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    100.0*65536,                                                // Maximum
    0,                                                              // Minimum
    1.0*65536,                                                              // Default
    1.0/65536,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_VP_i,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I32_T  ParDes2_VI_ipi =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: I-Verstärkung current p",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: I-gain current p",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    0,                                                              // Minimum
    0.05*65536,                                                             // Default
    1.0/65536,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_VI_ipi,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I32_T  ParDes2_VP_ipi =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: P-Verstärkung current p",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: P-gain current p",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    100.0*65536,                                                // Maximum
    0,                                                              // Minimum
    1.0*65536,                                                              // Default
    1.0/65536,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_VP_ipi,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I32_T  ParDes2_VI_ini =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: I-Verstärkung current n",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: I-gain current n",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    0,                                                              // Minimum
    0.05*65536,                                                             // Default
    1.0/65536,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_VI_ini,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I32_T  ParDes2_VP_ini =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: P-Verstärkung current n",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: P-gain current n",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    100.0*65536,                                                // Maximum
    0,                                                              // Minimum
    1.0*65536,                                                              // Default
    1.0/65536,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_VP_ini,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};



PAR_DES_I32_T  ParDes2_VIi =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Current I-gain",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Current I-gain",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    0,                                                              // Minimum
    0.05*65536,                                                             // Default
    1.0/65536,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_VIi,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};



PAR_DES_I32_T  ParDes2_aM =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Current closed loop",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Current closed loop",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    0,                                                              // Minimum
    0*65536,                                                                // Default
    1.0/65536,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_aM,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I32_T  ParDes2_k_iref_pn =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
    "2: k_iref_pn",                             // Name
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    0,                                                              // Minimum
    65536,                                                              // Default
    1.0/65536,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_k_iref_pn,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I32_T  ParDes2_k_iref_np =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
    "2: k_iref_np",                             // Name
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    0,                                                              // Minimum
    65536,                                                              // Default
    1.0/65536,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_k_iref_np,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I32_T  ParDes2_Ktrim_vffwp =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
    "2: Trim factor vffwp",                             // Name
    NULL,                                                           // Einheit
    2.0*65536,                                              // Maximum
    0,                                                              // Minimum
    0.05*65536,                                                             // Default
    1.0/65536,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_Ktrim_vffwp,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I32_T  ParDes2_Ktrim_vffwn_Prot =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
    "2: Trim factor vffwn Prot",                                // Name
    NULL,                                                           // Einheit
    2.0*65536,                                              // Maximum
    0,                                                              // Minimum
    0.05*65536,                                                             // Default
    1.0/65536,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_Ktrim_vffwn_Prot,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I32_T  ParDes2_D_notch =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
    "2: Notch filter damping",                              // Name
    NULL,                                                           // Einheit
    1.0*65536,                                              // Maximum
    0,                                                              // Minimum
    0.99*65536,                                                             // Default
    1.0/65536,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_D_notch,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I32_T  ParDes2_Ktrim_vffwn_comp =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
    "2: Trim factor vffwn comp",                                // Name
    NULL,                                                           // Einheit
    2.0*65536,                                              // Maximum
    0,                                                              // Minimum
    0.05*65536,                                                             // Default
    1.0/65536,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_Ktrim_vffwn_comp,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I32_T  ParDes2_Ktrim_vffw0_comp =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT32,                                   // ReadOnly
    "2: Trim factor vffw0 comp",                                // Name
    NULL,                                                           // Einheit
    5.0*65536,                                              // Maximum
    0,                                                              // Minimum
    0.05*65536,                                                             // Default
    1.0/65536,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_Ktrim_vffw0_comp,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Tracevariablen

//INT16 ParVal_Trace;
PAR_DES_I16_T  ParDes2_Trace =
{
    PAR_MENUE | PAR_RW | _INT16,                    // ReadWrite, INT16-Parameter
    "2: Tracelist",                                             // Name
    NULL,                                                   // Einheit
    1,                                              // Max
    0,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    NULL/*&ParVal_Trace*/,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_vLuv =
{
    PAR_PASS_PR | PAR_RO | _INT16,                  // ReadWrite, INT16-Parameter
    "2: vLuv",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_vLuv,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_vLwv =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: vLwv",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_vLwv,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_vLu =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: vLu",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_vLu,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_vLv =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: vLv",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_vLv,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_vinvu =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: vinv_u",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_vinvu,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_vinvv =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: vinv_v",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_vinvv,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_vLd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: vLd",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_vLd,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_vLq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: vLq",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_vLq,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_vLamp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: vLamp",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_vLamp,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_vinv_amp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: vinv_amp",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_vinv_amp,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iInvu =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: iInvu",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iInvu,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iInvv =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: iInvv",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iInvv,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iInvw =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: iInvw",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iInvw,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iInvd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: iInvd",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iInvd,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iInvq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: iInvq",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iInvq,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iInv_al =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: iInv_al",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iInv_al,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iInv_be =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: iInv_be",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iInv_be,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iL_al =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: is_al",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iL_al,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iL_be =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: is_be",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iL_be,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iInvamp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: iInvamp",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iInvamp,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iLu =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: isu",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iLu,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iLv =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: isv",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iLv,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iLw =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: isw",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iLw,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iLd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: isd",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iLd,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iLq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: isq",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iLq,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iLdav =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: ispd",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iLdav,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iLqav =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: ispq",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iLqav,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iLamp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: is_amp",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iLamp,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_fLine =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: fLine",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_fLine,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_thLine =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: thLine",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT - 1,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_thLine,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_thIGR =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: thIGR",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT - 1,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_thIGR,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_thm =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: thm",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT - 1,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_thm,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_ommTi =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: ommTi",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT - 1,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_ommTi,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_omm =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: omm",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT - 1,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_omm,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_rpm =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: rpm",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT - 1,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_rpm,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iLqref =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: iLrefpq",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iLqref,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iLdref =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: iLrefpd",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iLdref,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iLrefnd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: iLrefnd",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iLrefnd,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iLrefnq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: iLrefnq",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iLrefnq,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_idref =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: idref",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_idref,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iqref =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: iqref",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iqref,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_indref =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: indref",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_indref,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_inqref =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: inqref",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_inqref,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_udref =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: udref",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_udref,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_vLpd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: vLpd",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_vLpd,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_vLpq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: vLpq",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_vLpq,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_vLp_amp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: vLp_amp",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_vLp_amp,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_vLnd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: vLnd",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_vLnd,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_vLnq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: vLnq",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_vLnq,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_vLn_amp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: vLn_amp",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_vLn_amp,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_vpcompd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: vpcompd",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_vpcompd,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_vpcompq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: vpcompq",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_vpcompq,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_vncompd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: vncompd",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_vncompd,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_vncompq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: vncompq",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_vncompq,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_vncompd_Prot =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: vncompd_Prot",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_vncompd_Prot,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_vncompq_Prot =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: vncompq_Prot",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_vncompq_Prot,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_v0compd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: v0compd",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_v0compd,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_v0compq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: v0compq",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_v0compq,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iInv_fb_pd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: iInv_fb_pd",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iInv_fb_pd,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iInv_fb_pq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: iInv_fb_pq",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iInv_fb_pq,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iInv_fb_nd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: iInv_fb_nd",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iInv_fb_nd,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_iInv_fb_nq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: iInv_fb_nq",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iInv_fb_nq,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_ia1 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: ia1",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_ia1,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_ia2 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: ia2",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_ia2,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_ia3 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: ia3",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_ia3,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_ib1 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: ib1",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_ib1,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_ib2 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: ib2",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_ib2,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_ib3 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: ib3",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_ib3,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_ic1 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: ic1",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_ic1,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_ic2 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: ic2",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_ic2,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_ic3 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: ic3",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_ic3,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_uqref =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: uqref",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_uqref,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_udIrefp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: udIrefp",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_udIrefp,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_uqIrefp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: uqIrefp",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_uqIrefp,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_udIrefn =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: udIrefn",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_udIrefn,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_uqIrefn =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: uqIrefn",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_uqIrefn,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T  ParDes2_Imax =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: Imax",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_Imax,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};



// Voltage control

//INT16 ParVal_VoltageControl;
PAR_DES_I16_T  ParDes2_VoltageControl =
{
    PAR_MENUE | PAR_RW | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Generatorregelung",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Generator control",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_VoltageControl*/,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


// Current control

//INT16 ParVal_CurrentControl;
PAR_DES_I16_T  ParDes2_CurrentControl =
{
    PAR_MENUE | PAR_RW | _INT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Stromregelung",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Current control",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    1,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_CurrentControl*/,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T  ParDes2_Ifb_sel =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
    "2: Current fbk. select",                               // Name
    NULL,                                                           // Einheit
    0x000f,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_Ifb_sel,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};



PAR_DES_I16_T  ParDes2_PWM_mode0 =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "2: PWM Modus",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: PWM mode",                              // Name
#endif
#endif
    NULL,                                                           // Einheit
    0x0005,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_PWM_mode0,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T  ParDes2_PWM_Td =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "2: Totzeit",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Blanking time",                             // Name
#endif
#endif
    "us",                                                           // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_PWM_Td,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};



PAR_DES_I16_T  ParDes2_VoltageMode =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "2: Drehmoment-Modus",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Torque control mode",                               // Name
#endif
#endif
    NULL,                                                           // Einheit
    0x0003,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_VoltageMode,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T  ParDes2_FrequRef =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Frequenzsollwert",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Frequency reference",                               // Name
#endif
#endif
    "Hz",                                                           // Einheit
    4000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    0.1,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_FrequRef,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_U16_T  ParDes2_TimeoutSyncAmp =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _UINT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Timeout Sync Amplitude",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Timeout Sync Amplitude",                                // Name
#endif
#endif
    "ms",                                                           // Einheit
    60000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_TimeoutSyncAmp,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_U16_T  ParDes2_FRT_first_period =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _UINT16,                                    // ReadOnly
    "2: FRT first period",                              // Name
    "ms",                                                           // Einheit
    100,                                                // Maximum
    1,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_FRT_first_period,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T  ParDes2_FRT_Ii_limit_sym =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "2: FRT_Ii_limit_sym",                              // Name
    "",                                                         // Einheit
    32767,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_FRT_Ii_limit_sym,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T  ParDes2_FRT_Ii_limit_asym =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "2: FRT_Ii_limit_asym",                             // Name
    "",                                                         // Einheit
    32767,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_FRT_Ii_limit_asym,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_U16_T  ParDes2_TimeoutSyncPhase =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _UINT16,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Timeout Sync Phase",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Timeout Sync Phase",                                // Name
#endif
#endif
    "ms",                                                           // Einheit
    60000,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_TimeoutSyncPhase,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};



//#pragma DATA_SECTION(ParVal_CurrentOffsets,".extram");

PAR_DES_I16_T  ParDes2_CurrentOffsets =
{
    PAR_MENUE | PAR_RW | _INT16,                                    // ReadOnly
    "2: Current offsets",                               // Name
    NULL,                                                           // Einheit
    0,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    NULL/*&ParVal_CurrentOffsets*/,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};



PAR_DES_I16_T  ParDes2_OffsSelectIinv =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "2: Offset select inverter",                                // Name
    NULL,                                                           // Einheit
    4,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_OffsSelectIinv,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};



PAR_DES_I16_T  ParDes2_OffsSelectIline =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "2: Offset select Is",                              // Name
    NULL,                                                           // Einheit
    3,                                              // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_OffsSelectIline,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};



PAR_DES_I16_T  ParDes2_OffsInvActU =
{
    PAR_RW | _INT16,                                    // ReadOnly
    "2: Offset Iinv u",                             // Name
    NULL,                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU2toCPU1_Pars.ParVal_OffsInvActU,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};



PAR_DES_I16_T  ParDes2_OffsInvActV =
{
    PAR_RW | _INT16,                                    // ReadOnly
    "2: Offset Iinv v",                             // Name
    NULL,                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU2toCPU1_Pars.ParVal_OffsInvActV,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};



PAR_DES_I16_T  ParDes2_OffsInvActW =
{
    PAR_RW | _INT16,                                    // ReadOnly
    "2: Offset Iinv w",                             // Name
    NULL,                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU2toCPU1_Pars.ParVal_OffsInvActW,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T  ParDes2_OffsLineActU =
{
    PAR_RW | _INT16,                                    // ReadOnly
    "2: Offset Is1",                                // Name
    NULL,                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU2toCPU1_Pars.ParVal_OffsLineActU,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T  ParDes2_OffsLineActV =
{
    PAR_RW | _INT16,                                    // ReadOnly
    "2: Offset Is2",                                // Name
    NULL,                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU2toCPU1_Pars.ParVal_OffsLineActV,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T  ParDes2_OffsLineActW =
{
    PAR_RW | _INT16,                                    // ReadOnly
    "2: Offset Is3",                                // Name
    NULL,                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU2toCPU1_Pars.ParVal_OffsLineActW,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};



PAR_DES_I16_T  ParDes2_OffsInvSaveU =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "2: Offset Iinv u saved",                               // Name
    NULL,                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_OffsInvSaveU,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T  ParDes2_OffsInvSaveV =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "2: Offset Iinv v saved",                               // Name
    NULL,                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_OffsInvSaveV,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T  ParDes2_OffsInvSaveW =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "2: Offset Iinv w saved",                               // Name
    NULL,                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_OffsInvSaveW,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T  ParDes2_OffsLineSaveU =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "2: Offset Is1 saved",                              // Name
    NULL,                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_OffsLineSaveU,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T  ParDes2_OffsLineSaveV =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "2: Offset Is2 saved",                              // Name
    NULL,                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_OffsLineSaveV,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T  ParDes2_OffsLineSaveW =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "2: Offset Is3 saved",                              // Name
    NULL,                                                           // Einheit
    2000,                                               // Maximum
    -2000,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_OffsLineSaveW,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T  ParDes2_Tf_Mref =
{
    PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                 // ReadOnly
    "2: Torque ref time constant",                              // Name
    "ms",                                                           // Einheit
    30000,                                              // Maximum
    1,                                                              // Minimum
    100,                                                                // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_Tf_Mref,                                        // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T ParDes2_ItripAsym =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "2: Max. Stromasymmetrie",                  // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Max. current asymmetry",                                // Name
#endif
#endif
    NULL,                                                           // Einheit
    10000,                                              // Maximum
    1000,                                                               // Minimum
    1100,                                                               // Default
    0.001,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_ItripAsym,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T ParDes2_Vdc_min_on =
{
    PAR_PASS_PW | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                   // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: min. V_DC start",                   // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Vdc min ON",                                // Name
#endif
#endif
    "V",                                                            // Einheit
    2000,                                               // Maximum
    0,                                                              // Minimum
    500,                                                                // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_Vdc_min_on,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_DWORD_T ParDes2_DigIn =
{
    PAR_RO | _DWORD,                                    // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Digitale Eingänge",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Digital Inputs",                                // Name
#endif
#endif
    0xffffffff,                                             // Maximum
    0x00000000,                                                             // Minimum
    0,                                                              // Default
    &CPU2toCPU1_Pars.ParVal_DigIn,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_WORD_T ParDes2_DigOut =
{
    PAR_RO | _WORD,                                 // ReadOnly
#ifdef LANGUAGE_GERMAN
    "2: Digitale Ausgänge",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Digital Outputs",                               // Name
#endif
#endif
    0xffff,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &CPU2toCPU1_Pars.ParVal_DigOut,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_WORD_T ParDes2_FaultRelIn =
{
    PAR_PASS_PW | PAR_RW | _WORD,
#ifdef LANGUAGE_GERMAN
    "2: Steuerung Statorschütz",                    // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Stator cont. control",                              // Name
#endif
#endif
    0x0001,                                             // Maximum
    0x0000,                                                             // Minimum
    0,                                                              // Default
    &CPU1toCPU2_Pars.ParVal_FaultRelIn,                                     // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T ParDes2_ForceDOs =
{
    PAR_PASS_PW | PAR_RW | _INT16,
#ifdef LANGUAGE_GERMAN
    "2: Force digital outputs",                 // Name
#else
#ifdef LANGUAGE_ENGLISH
    "2: Force digital outputs",                             // Name
#endif
#endif
    NULL,                                                           // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU1toCPU2_Pars.ParVal_ForceDOs,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};


PAR_DES_I16_T ParDes2_Speed =
{
    PAR_RO | _INT16,                                    // ReadOnly
    "2: Speed",                             // Name
    "2: rpm",                                                           // Einheit
    20000,                                              // Maximum
    -20000,                                                             // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU2toCPU1_Pars.ParVal_speed_rpm_fil,                                      // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_iInvpd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: iInvpd",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iInvpd,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_iInvpq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: iInvpq",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iInvpq,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_iInvp_amp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: iInvp_amp",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iInvp_amp,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_iInvnd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: iInvnd",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iInvnd,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_iInvnq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: iInvnq",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iInvnq,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_iInvn_amp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: iInvn_amp",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iInvn_amp,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_iInv0d =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: iInv0d",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iInv0d,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_iInv0q =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: iInv0q",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_iInv0q,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_isnd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: isnd",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_isnd,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_isnq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: isnq",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_isnq,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_isn_amp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: isn_amp",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_isn_amp,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_isp_amp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: isp_amp",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_isp_amp,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_psispd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: psispd",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_psispd,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_psispq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: psispq",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_psispq,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_psisp_amp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: psisp_amp",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_psisp_amp,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_psisnd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: psisnd",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_psisnd,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_psisnq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: psisnq",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_psisnq,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_psis0d =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: psis0d",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_psis0d,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_psis0q =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: psis0q",                                                // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_psis0q,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_psisn_amp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: psisn_amp",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_psisn_amp,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_psisd =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: psisd",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_psisd,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_psisq =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: psisq",                                             // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_psisq,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_psis_amp =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: psis_amp",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_psis_amp,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_idref_tqref1 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: idref_tqref1",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_idref_tqref1,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_idref_tqref2 =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: idref_tqref2",                                              // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_idref_tqref2,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_idref_tqreg =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: idref_tqreg",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_idref_tqreg,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_irq_min =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: irq_min",                                               // Name
    NULL,                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_irq_min,                                        // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_prot =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: prot",                                              // Name
    "kW",                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_prot,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_qrot =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: qrot",                                              // Name
    "kVAr",                                                 // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_qrot,                                       // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_torque =
{
     PAR_PASS_PR | PAR_RO | _INT16,                 // ReadWrite, INT16-Parameter
    "2: torque",                                                // Name
    "Nm",                                                   // Einheit
    MAX_INT,                                                // Max
    -MAX_INT,                                           // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_torque,                                     // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I32_T ParDes2_Torque_Nm =
{
     PAR_RO | _INT32,                   // ReadWrite, INT16-Parameter
    "2: Torque",                                                // Name
    "Nm",                                                   // Einheit
    MAX_LONG,                                               // Max
    -MAX_LONG,                                          // Min
    0,                                                      // Default
    1.0,                                                    // Faktor
    &CPU2toCPU1_Pars.ParVal_Torque_Nm,                                      // Parameter
    NULL                                                    // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_InvCtrlState =
{
    PAR_RO | _INT16,                                    // ReadOnly
    "2: Inverter control state",                                // Name
    NULL,                                                           // Einheit
    1000,                                               // Maximum
    0,                                                              // Minimum
    0,                                                              // Default
    1,                    // Faktor
    &CPU2toCPU1_Pars.ParVal_InvCtrlState,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_Tf_inref =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                   // ReadOnly
    "2: Tf inref",                              // Name
    "ms",                                                           // Einheit
    100,                                                // Maximum
    1,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_Tf_inref,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};

PAR_DES_I16_T ParDes2_Tf_isref =
{
    PAR_PASS_PR | PAR_STORE | PAR_STORE_EE | PAR_RW | _INT16,                                   // ReadOnly
    "2: Tf isref",                              // Name
    "ms",                                                           // Einheit
    100,                                                // Maximum
    1,                                                              // Minimum
    0,                                                              // Default
    1.0,                      // Faktor
    &CPU1toCPU2_Pars.ParVal_Tf_isref,                                       // Parameter
    NULL                                                            // Behandlungsfunktion
};






/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */
