/* *****************************************************************************
 * File:   hmrecparam_t.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 11 12 22:20
 * 
 * Description: Hex Monitor Recorder Types
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef HMRECPARAM_T_H
#define	HMRECPARAM_T_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "hmlib.h"



/*****************************************************************************
* Configuration Definitions Recorder support
******************************************************************************/

#ifndef HMRECPRM_MAX_VARS
#ifdef  HMDRV_MAX_REC_PARAM_VARS
#define HMRECPRM_MAX_VARS     HMDRV_MAX_REC_PARAM_VARS
#else
#define HMRECPRM_MAX_VARS     20  /* max. number of recorder variables */
#endif
#endif



/*****************************************************************************
* Recorder support
******************************************************************************/


#define HMRECPRM_FLOAT_TRIG_MASK      0x02

#ifndef HMRECPRM_EXECUTE_MICROSEC
#define HMRECPRM_EXECUTE_MICROSEC  100
#endif


#define HMRECPRM_EXECUTE_MICROSEC_BASE_0 HMRECPRM_EXECUTE_MICROSEC

#ifndef HMRECPRM_EXECUTE_MICROSEC_BASE_1
#define HMRECPRM_EXECUTE_MICROSEC_BASE_1    100
#endif

#ifndef HMRECPRM_EXECUTE_MICROSEC_BASE_2
#define HMRECPRM_EXECUTE_MICROSEC_BASE_2    100
#endif

#ifndef HMRECPRM_EXECUTE_MICROSEC_BASE_3
#define HMRECPRM_EXECUTE_MICROSEC_BASE_3    100
#endif



/* recorder time base declaration helpers */
#define HMRECPRM_BASE_SECONDS(x)  ((x) & 0x3fff)
#define HMRECPRM_BASE_MILLISEC(x) (((x) & 0x3fff) | 0x4000)
#define HMRECPRM_BASE_MICROSEC(x) (((x) & 0x3fff) | 0x8000)
#define HMRECPRM_BASE_NANOSEC(x)  (((x) & 0x3fff) | 0xc000)


/* recorder time base, specifies how often the recorder is called in the user app. */
#define HMRECPRM_TIMEBASE     HMRECPRM_BASE_MICROSEC((unsigned short)(HMRECPRM_EXECUTE_MICROSEC)) /* 0 = "unknown" */

#define HMRECPRM_FLOAT_TRIG   HMDRV_FLOAT_TRIG_ENABLE    /* enable/disable floating point triggering */

/* *****************************************************************************
 * Type Definitions and Enumerations
 **************************************************************************** */

/* trigger threshold level (1,2 or 4 bytes) */
typedef union 
{
    HMDRV_U8  u8; 
    HMDRV_S8  s8;
    HMDRV_U16 u16;
    HMDRV_S16 s16;
    HMDRV_U32 u32;
    HMDRV_S32 s32;
    HMDRV_FLOAT fp;
} HMRECPRM_uTrgThreshold_t;                        

/* compare functions prototype */
typedef HMDRV_BOOL(*HMRECPRM_PCOMPAREFUNC_Dummy)(void);



/* recorder runtime flags */
typedef struct
{
        unsigned bIsConfigured : 1;     /* recorder is configured */
        unsigned bIsRunning : 1;        /* recorder is running */
        unsigned bIsStopping : 1;       /* trigger activated, sample countdown */
        unsigned bInvirginCycle : 1;    /* virgin cycle of the circular buffer in-progress */
        unsigned bTrgCrossActive : 1;   /* trigger threshold was crossed */
        unsigned bUseOwnBuffer : 1;     /* use external specified from the user not dedicated for the module buffer */
        unsigned bStopDetected : 1;     /*  */
        unsigned bStopDetectEnable : 1; /*  */
    
}HMRECPRM_sFlags_t;

typedef union 
{
    HMDRV_FLAGS     nAll;
    HMRECPRM_sFlags_t    sFlg;   
} HMRECPRM_uFlags_t;


typedef union
{
    HMDRV_U32 u32Register;
    HMDRV_U16 au16Word[2];  
}HMRECPRM_u32Union_t;




/* Recorder Handle */
typedef struct
{
	HMRECPRM_uFlags_t  uFlags;                 /* recorder flags */
	HMDRV_ADDR      pBufferAddr;            /* recorder buffer address */
    HMDRV_SIZE_FLEX      nBufferSize;            /* recorder buffer size */

    /* Recorder buffer pointers */
    HMDRV_ADDR      pWritePtr;              /* write pointer in buffer */
    HMDRV_ADDR      pEndBuffPtr;            /* pointer to end of buffer */

    /* one sample are all records processed at one pass */
    HMDRV_U16       u16RecBuffStartIx;      /* first sample index */
    
    /* pointer to active compare function */
    HMRECPRM_PCOMPAREFUNC_Dummy    pfCompareFunc;

    /* configuration variables trigger */
    HMDRV_ADDR pu8TrgVarAddr;               /* trigger variable address */
    HMDRV_uVariableSizeOffset_t   uTrgVarSize;                /* trigger variable threshold size (if bit field the bit offset is here) */
    HMDRV_U8   u8TrgVarSigned;              /* trigger compare mode (0 = unsigned, 1 = signed) ??? consider boolean */
    HMRECPRM_uTrgThreshold_t uTrgThreshold;

    /* configuration variables */
    //HMRECPRM_u32Union_t  uRecTotalSmps;        /* number of samples to measure (consider used with HMDRV_SIZE_FLEX) */

    //HMRECPRM_u32Union_t  uRecPostTrigger;      /* number of post-trigger samples to keep */

    HMDRV_SIZE_FLEX  uRecTotalSmps;        /* number of samples to measure (consider used with HMDRV_SIZE_FLEX) */

    HMDRV_SIZE_FLEX  uRecPostTrigger;      /* number of post-trigger samples to keep */

    HMDRV_SIZE_FLEX  uRecSamplesAfterStopDetect;      /* number of post-trigger_main samples */

    HMDRV_U16  u16RecTimeDiv;          /* divisor of recorder "clock" */

    HMDRV_U8   u8RecTriggerMode;      /* trigger mode (0 = disabled, 1 = _/, 2 = \_) ??? consider both */

    HMDRV_U8    u8RecVarCount;        /* number of active recorder variables */
    HMDRV_U8    u8RecSampleSize;        /* Size of one sample of active recorder variables - not used and not needed -> but check Modbus uses it and malloc if queues used */
    HMDRV_ADDR  pu8RecVarAddr[HMRECPRM_MAX_VARS]; /* addresses of recorded variables */
    HMDRV_uVariableSizeOffset_t uRecVarSize[HMRECPRM_MAX_VARS]; /* sizes of recorded variables */

    /* runtime variables  */
    HMDRV_U16  u16RecTimeDivCtr;       /* recorder "clock" divisor counter */

    HMDRV_SIZE_FLEX  nStoprecCountDown;    /* post-trigger countdown counter */
    
    HMDRV_U16  u16RecTimeBase;         /* time base */


}HMRECPRM_sHandle_t;


/* compare functions prototype */
typedef HMDRV_BOOL (*HMRECPRM_PCOMPAREFUNC)(HMRECPRM_sHandle_t*);




typedef enum 
{
    HMRECPRM_CODE_OK,
    HMRECPRM_CODE_NOTINIT,
    HMRECPRM_CODE_RECRUN,
    HMRECPRM_CODE_RECDONE,
    HMRECPRM_CODE_SERVBUSY,
    HMRECPRM_CODE_INVBUFF,
    HMRECPRM_CODE_INVSIZE,
    HMRECPRM_CODE_FLOATDISABLED,
            
            
}HMRECPRM_eResponseCode_t;






/* time dimension */
typedef enum
{
    HMRECPRM_TIME_BASE_DIMENSION_SECONDS,
    HMRECPRM_TIME_BASE_DIMENSION_MILLISECONDS,
    HMRECPRM_TIME_BASE_DIMENSION_MICROSECONDS,
    HMRECPRM_TIME_BASE_DIMENSION_NANOSECONDS,
}HMRECPRM_eTimeBaseDimension_t;

typedef struct
{
    uint16_t nValue             : 14;
    uint16_t eDimension         : 2;  /* consider cast to HMRECPRM_eTriggerMode_t */
}HMRECPRM_sTimeBaseValue_t;

typedef union
{
    uint16_t u16Register;
    HMRECPRM_sTimeBaseValue_t sTimeBaseValue;
}HMRECPRM_uTimeBaseValue_t;


/* trigger mode (0 = disabled, 1 = _/, 2 = \_) ??? consider both */
typedef enum
{
    HMRECPRM_TRIGGER_NONE,
    HMRECPRM_TRIGGER_RISING_EDGE,
    HMRECPRM_TRIGGER_FALLING_EDGE,
    HMRECPRM_TRIGGER_ANY_EDGE,  /* not implemented for now */
}HMRECPRM_eTriggerMode_t;

/* trigger sign (0 = unsigned compare mode, 1 = signed compare mode */
typedef enum
{
    HMRECPRM_TRIGGER_UNSIGNED,
    HMRECPRM_TRIGGER_SIGNED,
}HMRECPRM_eTriggerSign_t;

/* trigger type (0 = unsigned/ signed integer char long compare mode, 1 = floating point compare mode */
typedef enum
{
    HMRECPRM_TRIGGER_INTEGER,
    HMRECPRM_TRIGGER_FLOAT,
}HMRECPRM_eTriggerType_t;

/* select time source from different places in the code */
typedef enum
{
    HMRECPRM_TIME_BASE_0,
    HMRECPRM_TIME_BASE_1,
    HMRECPRM_TIME_BASE_2,
    HMRECPRM_TIME_BASE_3,
}HMRECPRM_eTimeBase_t;











/* *****************************************************************************
 * Recorder Status
 **************************************************************************** */
typedef enum
{
    HMRECPRM_SYNC_SAMPLE_TRIG = 0,
    HMRECPRM_SYNC_SAMPLE_LAST = 1,
    HMRECPRM_SYNC_SAMPLE_FRST = 2,
}HMRECPRM_eTimeSyncSampleMode_t;


typedef enum
{
    HMRECPRM_REC_STA_NOT_COFIGURED = 0,
    HMRECPRM_REC_STA_NOT_STARTED = 1,
    HMRECPRM_REC_STA_RUNNING = 2,
    HMRECPRM_REC_STA_DONE = 3,
}HMRECPRM_eRecorderStatus_t;


typedef enum
{
    HMRECPRM_TIME_MICRO_SECONDS = 0,         /* u32MicroSec0BuffOvf1 in Micro-Seconds */
    HMRECPRM_TIME_BUFFER_OVERFLOW = 1,      /* u32MicroSec0BuffOvf1 is buffer overflow counter */
}HMRECPRM_eMicroSec0BuffOvf1Mode_t;



typedef struct
{
    uint32_t u8Day : 8;
    uint32_t u8Month : 8;
    uint32_t u8Year : 8;

    uint32_t b2TimeSyncSampleMode          : 2;    /* HMRECPRM_eTimeSyncSampleMode_t */
    uint32_t b2RecorderStatus              : 2;    /* HMRECPRM_eRecorderStatus_t */
    uint32_t b3Reserved                    : 3;
    uint32_t bMicroSec0BuffOvf1Mode        : 1;    /* HMRECPRM_eMicroSec0BuffOvf1Mode_t */
}HMRECPRM_sTimeSyncModeYYMMDD_t;

typedef struct
{
    uint32_t u8Second : 8;
    uint32_t u8Minute : 8;
    uint32_t u8Hour : 8;
    uint32_t u8WeekDays : 8;
}HMRECPRM_sTimeSyncWWHHMMSS_t;

typedef struct
{
    HMRECPRM_sTimeSyncModeYYMMDD_t  sTimeSyncModeYYMMDD;        /* 32 bits =   4 bytes */
    HMRECPRM_sTimeSyncWWHHMMSS_t    sTimeSyncWWHHMMSS_t;        /* 32 bits =   4 bytes */
    uint32_t                        u32MicroSec0BuffOvf1;       /* 32 bits =   4 bytes */
    uint32_t                        u32AvailableSampleCount;    /* 32 bits =   4 bytes */
}HMRECPRM_sStatus_t;                               /*128 bits =  16 bytes */
#define HMRECPRM_sStatus_t_ElementsCount (1 + 1 + 1 + 1)        /* HMRECPRM_sTimeSyncModeYYMMDD_t - 1 32-bit; HMRECPRM_sTimeSyncWWHHMMSS_t */






/* *****************************************************************************
 * Recorder Configuration
 **************************************************************************** */
typedef enum
{
    HMRECPRM_VARIANT_TYPE_BOOL_FLAG = 0x0200,
    HMRECPRM_VARIANT_TYPE_ENUM_FLAG = 0x0100,
    HMRECPRM_VARIANT_TYPE_SIGNED_FLAG = 0x0080,
    HMRECPRM_VARIANT_TYPE_FLOAT_FLAG = 0x0040,
    HMRECPRM_VARIANT_TYPE_SIZE_MASK = 0x0003,

    HMRECPRM_VARIANT_TYPE_U8  = 0x0000,
    HMRECPRM_VARIANT_TYPE_U16 = 0x0001,
    HMRECPRM_VARIANT_TYPE_U32 = 0x0002,
    HMRECPRM_VARIANT_TYPE_U64 = 0x0003, /* for HMRECPRM_eVariant64_t or larger */

    HMRECPRM_VARIANT_TYPE_S8  = 0x0080,
    HMRECPRM_VARIANT_TYPE_S16 = 0x0081,
    HMRECPRM_VARIANT_TYPE_S32 = 0x0082,
    HMRECPRM_VARIANT_TYPE_S64 = 0x0083, /* for HMRECPRM_eVariant64_t or larger */

    HMRECPRM_VARIANT_TYPE_F16 = 0x00C1,
    HMRECPRM_VARIANT_TYPE_F32 = 0x00C2,
    HMRECPRM_VARIANT_TYPE_F64 = 0x00C3, /* for HMRECPRM_eVariant64_t or larger */

}HMRECPRM_eVariantType_t; /* 16 bits - change also CSMON_eDataType_t */

typedef enum
{
    HMRECPRM_VISUAL_TYPE_MASK = 0x0007,

    HMRECPRM_VISUAL_TYPE_DEF = 0x0007,     /* type not specified (default hex if multiplier 0; if multiplier != 0 decimal) */

    HMRECPRM_VISUAL_TYPE_HEX = 0x0000,
    HMRECPRM_VISUAL_TYPE_DEC = 0x0001,
    HMRECPRM_VISUAL_TYPE_BIN = 0x0002,
    HMRECPRM_VISUAL_TYPE_SCI = 0x0003,     /* scientific */
    HMRECPRM_VISUAL_TYPE_ENG = 0x0004,     /* engineering */

}HMRECPRM_eVisualType_t; /* 16 bits - change also CSMON_eVisualType_t */




#define HMRECPRM_VariantType_IsSigned(_variantType_)    (0U != (HMRECPRM_VARIANT_TYPE_SIGNED_FLAG & (_variantType_)))
#define HMRECPRM_VariantType_IsUnsigned(_variantType_)  (0U == (HMRECPRM_VARIANT_TYPE_SIGNED_FLAG & (_variantType_)))

#define HMRECPRM_VariantType_IsFloatingPoint(_variantType_) (0U != (HMRECPRM_VARIANT_TYPE_FLOAT_FLAG & (_variantType_)))
#define HMRECPRM_VariantType_IsInteger(_variantType_)       (0U == (HMRECPRM_VARIANT_TYPE_FLOAT_FLAG & (_variantType_)))

#define HMRECPRM_VariantType_ByteSize(_variantType_)       (1U << (HMRECPRM_VARIANT_TYPE_SIZE_MASK & (_variantType_)))




typedef union
{
    HMDRV_U8  u8;
    HMDRV_S8  s8;
    HMDRV_U16 u16;
    HMDRV_S16 s16;
    HMDRV_U32 u32;
    HMDRV_S32 s32;
    HMDRV_FLOAT fp;
} HMRECPRM_uVariant32Value_t;  /* 32 bits */

typedef struct
{
    HMRECPRM_eVariantType_t eType;      /* 16 bits = 2 bytes */
    HMRECPRM_eVisualType_t eVisual;               /* 16 bits = 2 bytes */
    HMRECPRM_uVariant32Value_t eValue;  /* 32 bits = 4 bytes */
}HMRECPRM_sVariant32_t; /* 64 bits = 8 bytes*/




typedef struct
{
    HMRECPRM_eTimeBase_t        eTimeBase;                  /* 16 bits = 2 bytes */
    uint16_t                    nTimeDividerOrSamplesSkip;  /* 16 bits = 2 bytes */
} HMRECPRM_sSampleRateLookUp_t; /* 32 bits */


typedef struct
{
    HMDRV_FLOAT                     fSampleFrequency;   /* 32 bits = 4 bytes */ /* if isnan(fSampleFrequency) then use sSampleRateLookUp. */
    HMRECPRM_sSampleRateLookUp_t    sSampleRateLookUp;  /* 32 bits = 4 bytes */
}HMRECPRM_sSampleRateConfig_t; /* 64 bits = 2 * 32 bits = 8 bytes */


typedef struct
{
    HMRECPRM_eTriggerMode_t     eTriggerMode;       /* 16 bits = 2 bytes */
    uint16_t                    u16Reserved;        /* 16 bits = 2 bytes */
    uint32_t                    nVariableIDTrigger; /* 32 bits = 4 bytes */
    uint32_t                    nPreTriggerSamples; /* 32 bits = 4 bytes */
    HMRECPRM_sVariant32_t       sTriggerThreshold;  /* 64 bits = 8 bytes */
}HMRECPRM_sTrigerConfig_t; /* 160 bits = 5 * 32 bits = 20 bytes*/


typedef struct
{
    uint16_t u16ValueByteAlignment  : 3;
    /*  if (0 = u16ValueByteAlignment) then alignment = 1 byte.
        if (7 = u16ValueByteAlignment) then alignment = 8 bytes.
        For current task u16ValueByteAlignment = 1 i.e. alignment = 2 bytes.
    */

    uint16_t u16Reserved            : 5;

    uint16_t u16SampleByteAlignment : 8;
    /*  if (2^0 = u16SampleByteAlignment) then alignment = 1 byte.
        if (2^8 = u16SampleByteAlignment) then alignment = 256 bytes.
        u16SampleByteAlignment + 1 >= (u16ValueByteAlignment + 1) * HMRECPRM_MAX_VARS
    */


}HMRECPRM_sAlignments_t;

typedef struct
{
    float                           nSampleRateHz;                          /*  32 bits =  4 bytes */
    uint32_t                        nSampleCountMax;                        /*  32 bits =  4 bytes */
    uint32_t                        nPreTriggerSampleCount;                 /*  32 bits =  4 bytes */
    HMRECPRM_sAlignments_t          sAlignments;                            /*  16 bits =  2 bytes */
    uint16_t                        nCountVariables;                        /*  16 bits =  2 bytes */
    uint32_t                        nVariableIDRecord[HMRECPRM_MAX_VARS];   /* 640 bits = 80 bytes */
}HMRECPRM_sSampleFormat_t;                                   /* 768 bits = 24 * 32 bits = 96 bytes */

typedef struct
{
    HMRECPRM_sSampleFormat_t        sSampleFormat;      /* 768 bits = 24 * 32 bits =  96 bytes */
    uint32_t                        nTotalCountSamples; /*  32 bits =  1 * 32 bits =   4 bytes */
    HMRECPRM_sSampleRateConfig_t    sSampleRate;        /*  64 bits =  2 * 32 bits =   8 bytes */
    HMRECPRM_sTrigerConfig_t        sTrigerConfig;      /* 160 bits =  5 * 32 bits =  20 bytes */
}HMRECPRM_sConfig_t;                                   /* 1024 bits = 32 * 32 bits = 128 bytes */

#define HMRECPRM_sConfig_t_ElementsCount      45 /* 5 + HMRECPRM_MAX_VARS*2 */





/* *****************************************************************************
 * External Usage Variables
 **************************************************************************** */

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
HMDRV_BOOL HMRECPRM_CompareBOOL(HMRECPRM_sHandle_t* sHandle);
HMDRV_BOOL HMRECPRM_Compare8S(HMRECPRM_sHandle_t* sHandle);
HMDRV_BOOL HMRECPRM_Compare8U(HMRECPRM_sHandle_t* sHandle);
HMDRV_BOOL HMRECPRM_Compare16S(HMRECPRM_sHandle_t* sHandle);
HMDRV_BOOL HMRECPRM_Compare16U(HMRECPRM_sHandle_t* sHandle);
HMDRV_BOOL HMRECPRM_Compare32S(HMRECPRM_sHandle_t* sHandle);
HMDRV_BOOL HMRECPRM_Compare32U(HMRECPRM_sHandle_t* sHandle);
#if HMRECPRM_FLOAT_TRIG
HMDRV_BOOL HMRECPRM_Comparefloat(HMRECPRM_sHandle_t* sHandle);
#endif

void HMRECPRM_vInitParam(HMRECPRM_sHandle_t* sHandle, HMDRV_U16 u16ConfigModeIndex);
HMRECPRM_eResponseCode_t HMRECPRM_StartRecParam(HMDRV_U16 u16RecorderIndex);
HMRECPRM_eResponseCode_t HMRECPRM_StopRecParam(HMDRV_U16 u16RecorderIndex);
HMRECPRM_eResponseCode_t HMRECPRM_GetRecBuffCheckCodeParam(HMRECPRM_sHandle_t* sHandle);
HMDRV_BPTR HMRECPRM_GetRecBuffFreemasterParam(HMRECPRM_sHandle_t* sHandle, HMDRV_BPTR pMessageIO);  /* not needed or to be re-written */
HMRECPRM_sStatus_t* HMRECPRM_GetStatusRecParamAddress(HMDRV_U8 recorderIndex);

HMDRV_BOOL HMRECPRM_IsJustStopped(HMDRV_U16 u16RecorderIndex);
HMDRV_BOOL HMRECPRM_IsRunning(HMDRV_U16 u16RecorderIndex);
void HMRECPRM_SetUpRecParamStatusDateTime(HMDRV_U16 u16RecorderIndex, HMRECPRM_sTimeSyncWWHHMMSS_t* sTime, HMRECPRM_sTimeSyncModeYYMMDD_t* sDate, HMDRV_U32 u32MicroSec);
void HMRECPRM_ClrLastStopDetectionToRecorderStopSamplesCounter(HMDRV_U16 u16RecorderIndex);
void HMRECPRM_vDisableAllStopDetections(void);

HMRECPRM_eResponseCode_t HMRECPRM_GetRecStatusParam(HMRECPRM_sHandle_t* sHandle);
HMDRV_ADDR HMRECPRM_GetBufferRecParamAddress(HMDRV_U8 recorderIndex);
HMDRV_ADDR HMRECPRM_GetBufferRecParamPtrAddress(HMDRV_U8 recorderIndex);
void HMRECPRM_AbortRec(HMRECPRM_sHandle_t* sHandle);
#endif	/* HMRECPARAM_T_H */
