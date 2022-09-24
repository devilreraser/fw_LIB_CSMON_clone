/* *****************************************************************************
 * File:   mclib.h
 * Author: Dimitar Lilov
 *
 * Created on 2018 11 27 14:12
 * 
 * Description: Math and Control Library 
 * 
 **************************************************************************** */




/* Guard condition file contents not included more than once */  
#ifndef MCLIB_H
#define	MCLIB_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "mclibdrv.h"



/* *****************************************************************************
 * Configuration Macro Definitions - Intended for external usage
 **************************************************************************** */
#define MCLIB_DEBUG_POLYNOM 1



/* *****************************************************************************
 * Header Includes
 **************************************************************************** */

#ifdef MCLIBCFG_USE_STDFIXLIB
#include <stdfix.h>
#endif


/* *****************************************************************************
 * Macro Definitions
 **************************************************************************** */
#ifndef MCLIB_DEBUG_POLYNOM
#define MCLIB_DEBUG_POLYNOM 0
#endif

#ifndef MCLIBCFG_USE_STDFIXLIB
#define FRACT16_MAX (0.999969482421875)                 /* (2^15-1)/2^15 */

#define FRACT16_MIN (-1.0)

#define FRACT32_MAX (0.9999999995343387126922607421875) /* (2^31-1)/2^31 */

#define FRACT32_MIN (-1.0)

#define UFRACT16_MAX (0.9999847412109375)               /* (2^16-1)/2^16 */

#define UFRACT16_MIN (0)

#define UFRACT32_MAX (0.99999999976716935634613037109375) /* (2^32-1)/2^32 */

#define UFRACTQ_16_16_MAX (65535.9999847412109375)  /* (2^32-1)/2^16 */

#define  FRACTQ_15_16_MAX (32767.9999847412109375)  /* (2^31-1)/2^16 */
#define  FRACTQ_15_16_MIN (-32768)  /* (2^31)/2^16 */

#define UFRACT32_MIN (0)
#endif


/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
typedef struct
{
    uint16_t u16Lo;
    uint16_t u16Hi;
}sWords_t;

#ifdef uint8_t
typedef struct
{
    uint8_t u8Lo;
    uint8_t u8Hi;   
}sBytes_t;
#endif

#ifdef uint8_t
typedef union
{
    uint16_t u16Value;
    uint16_t u16Reg;
    uint16_t au16Words[1];
     uint8_t au8Bytes[2];
    sBytes_t sBytes;
}uint16_uBase8;
#endif

typedef union
{
    uint16_t u16Value;
    uint16_t u16Reg;
    uint16_t au16Words[1];
}uint16_uBase16;

#ifdef uint8_t
#define uint16_u uint16_uBase8
#else
#define uint16_u uint16_uBase16
#endif

typedef struct
{
    uint16_t u16Mtply;
    uint16_t u16Shift;
}sCoeff_t;

#ifdef uint8_t
typedef union
{
    uint32_t u32Value;
    uint16_t au16Words[2];
     uint8_t au8Bytes[4];
    sWords_t sWords;
    sBytes_t asBytes[2];

    uint32_t u32Reg;
    sCoeff_t sCoeff;
}uint32_uBase8;
#endif

typedef union
{
    uint32_t u32Value;
    uint16_t au16Words[2];
    sWords_t sWords;

    uint32_t u32Reg;
    sCoeff_t sCoeff;
}uint32_uBase16;

#ifdef uint8_t
#define uint32_u uint32_uBase8
#else
#define uint32_u uint32_uBase16
#endif


typedef struct
{
    uint16_t u16InputMin;
    uint16_t u16InputMax;
    uint16_t u16OutputMin;
    uint16_t u16OutputMax;
    uint32_u uMultiplier;
}MCLIB_sLinearInterpolation_t;



typedef struct
{
    int16_t s16Lo;
    int16_t s16Hi;
}sWordsSigned_t;

typedef struct
{
    int16_t s16Mtply;
    int16_t s16Shift;
}sCoeffSigned_t;

typedef union
{
    int32_t s32Reg;
    sWordsSigned_t sWordsSigned;
    sCoeffSigned_t sCoeffSigned;
}int32_u;



typedef struct
{
    int16_t s16InputMin;
    int16_t s16InputMax;
    int16_t s16OutputMin;
    int16_t s16OutputMax;
    int32_u uMultiplier;
}MCLIB_sLinearInterpolationSigned_t;

typedef struct
{
    uint32_t u32State;
    uint32_t u32RampUp;
    uint32_t u32RampDown;
}MCLIB_sRamp_t;

typedef struct
{
    uint32_t u32Acc;
    uint16_t u16NSamples;
} MCLIB_sFilterMA_t;

typedef struct
{
    uint32_t u32Acc;
    uint16_t u16NSamples;
} MCLIB_sFilterMAu16_t;

typedef struct
{
    int32_t s32Acc;
    uint16_t u16NSamples;
} MCLIB_sFilterMAs16_t;

typedef struct
{
    uint32_t u32Acc;
    uint16_t u16NSamples;
    uint16_t u16NShift;
    uint32_t u32Mult;       /* Q16_16 Format of 1/u16NSamples */
} MCLIB_sFilterAVGu32_t;

typedef struct
{
    int32_t s32Acc;
    uint16_t u16NSamples;
    uint16_t u16NShift;
    uint32_t u32Mult;       /* Q16_16 Format of 1/u16NSamples */
} MCLIB_sFilterAVGs32_t;

typedef struct
{
    uint32_t u32Acc;
    uint16_t u16NSamples;
    uint16_t u16NShift;
    uint32_t u32Mult;       /* Q16_16 Format of 1/u16NSamples */
} MCLIB_sFilterAVGu16_t;

typedef struct
{
    int32_t s32Acc;
    uint16_t u16NSamples;
    uint16_t u16NShift;
    uint32_t u32Mult;       /* Q16_16 Format of 1/u16NSamples */
} MCLIB_sFilterAVGs16_t;


typedef struct
{
    int32_t s32State;/* Integrator state value */
    int16_t s16PrevInput;/* Input value in step k-1. */
    int16_t s16C1;/* Integrator coefficient = (E\f$_{MAX}\f$/T\f$_{s}\f$)(U\f$_{MAX}\f$*2)*(2\f$^{-u16NShift}\f$) */
    uint16_t u16NShift;/* Scaling factor for the integrator coefficient f16C1, integer format [0, 15] */
} MCLIB_sIntegrator_t;

typedef struct
{
    /* Proportional Gain, fractional format normalized to fit into (-2\f$^{15}\f$, 2\f$^{15}\f$-1) */
    int16_t s16PropGain;
    /* Integral Gain, fractional format normalized to fit into  (-2\f$^{15}\f$, 2\f$^{15}\f$-1) */
    int16_t s16IntegGain;
    /* Proportional Gain Shift, integer format [-15, 15] */
    int16_t s16PropGainShift;
    /* Integral Gain Shift, integer format [-15, 15] */
    int16_t s16IntegGainShift;
    /* Lower Limit of the controller, fractional format normalized to fit into (-2\f$^{15}\f$, 2\f$^{15}\f$-1) */
    int16_t s16Min;
    /* Upper Limit of the controller, fractional format normalized to fit into (-2\f$^{15}\f$, 2\f$^{15}\f$-1) */
    int16_t s16Max;
    /* State variable integral part at step k-1 */
    int32_t s32PrevIntegPart;
    /* State variable input error at step k-1 */
    int16_t s16PrevError;
    /* Limitation flag, if set to 1, the controller output has reached either the UpperLimit or LowerLimit */
    uint16_t u16LimitFlag;
} MCLIB_sControllerPIpAW_t;

typedef struct
{
    /* CC1 coefficient, fractional format normalized to fit into (-2\f$^{15}\f$, 2\f$^{15}\f$-1) */
    int16_t s16CC1;
    /* CC2 coefficient, fractional format normalized to fit into (-2\f$^{15}\f$, 2\f$^{15}\f$-1) */
    int16_t s16CC2;
    /* Internal controller accumulator */
    int32_t s32Acc;
    /* Controller input from the previous calculation step */
    int16_t s16PrevError;
    /* Upper Limit of the controller, fractional format normalized to fit into (-2\f$^{15}\f$, 2\f$^{15}\f$-1) */
    int16_t s16Max;
    /* Lower Limit of the controller, fractional format normalized to fit into (-2\f$^{15}\f$, 2\f$^{15}\f$-1) */
    int16_t s16Min;
    /* Scaling factor for the controller coefficients, integer format [0, 15] */
    uint16_t u16NShift;
} MCLIB_sControllerPIrAW_t;


typedef struct
{
    /* Proportional Gain, fractional format normalized to fit into (-2\f$^{15}\f$, 2\f$^{15}\f$-1) */
    int16_t s16PropGain;
    /* Integral Gain, fractional format normalized to fit into  (-2\f$^{15}\f$, 2\f$^{15}\f$-1) */
    int16_t s16IntegGain;
    /* Proportional Gain Shift, integer format [-15, 15] */
    int16_t s16PropGainShift;
    /* Integral Gain Shift, integer format [-15, 15] */
    int16_t s16IntegGainShift;
    /* Lower Limit of the controller, fractional format normalized to fit into (-2\f$^{16}\f$, 2\f$^{16}\f$-1) */
    uint16_t u16Min;
    /* Upper Limit of the controller, fractional format normalized to fit into (-2\f$^{16}\f$, 2\f$^{16}\f$-1) */
    uint16_t u16Max;
    /* State variable integral part at step k-1 */
    uint32_t u32PrevIntegPart;
    /* State variable input error at step k-1 */
    int16_t s16PrevError;
    /* Limitation flag, if set to 1, the controller output has reached either the UpperLimit or LowerLimit */
    uint16_t u16LimitFlag;
} MCLIB_sControllerPIpAWu16_t;

typedef struct
{
    /* CC1 coefficient, fractional format normalized to fit into (-2\f$^{15}\f$, 2\f$^{15}\f$-1) */
    int16_t s16CC1;
    /* CC2 coefficient, fractional format normalized to fit into (-2\f$^{15}\f$, 2\f$^{15}\f$-1) */
    int16_t s16CC2;
    /* Internal controller accumulator */
    uint32_t u32Acc;
    /* Controller input from the previous calculation step */
    int16_t s16PrevError;
    /* Upper Limit of the controller, fractional format normalized to fit into (-2\f$^{16}\f$, 2\f$^{16}\f$-1) */
    uint16_t u16Max;
    /* Lower Limit of the controller, fractional format normalized to fit into (-2\f$^{16}\f$, 2\f$^{16}\f$-1) */
    uint16_t u16Min;
    /* Scaling factor for the controller coefficients, integer format [0, 15] */
    uint16_t u16NShift;
} MCLIB_sControllerPIrAWu16_t;



typedef enum
{
    BOOL_FALSE,
    BOOL_TRUE
} MCLIB_eBool_t;

typedef void (*pfVoid_t)(void); 

typedef struct
{
    int16_t s16Arg1;
    int16_t s16Arg2;
} MCLIB_s2Param_t;

typedef struct
{
    int16_t s16Arg1;
    int16_t s16Arg2;
    int16_t s16Arg3;
} MCLIB_s3Param_t;


typedef struct
{
    int16_t s16Arg1;
    int16_t s16Arg2;
} MCLIB_s2Param_s16_t;

typedef struct
{
    int16_t s16Arg1;
    int16_t s16Arg2;
    int16_t s16Arg3;
} MCLIB_s3Param_s16_t;


typedef struct
{
    uint16_t u16Arg1;
    uint16_t u16Arg2;
} MCLIB_s2Param_u16_t;

typedef struct
{
    uint16_t u16Arg1;
    uint16_t u16Arg2;
    uint16_t u16Arg3;
} MCLIB_s3Param_u16_t;


typedef struct
{
    int32_t s32Arg1;
    int32_t s32Arg2;
    int32_t s32Arg3;
} MCLIB_s3Param_s32_t;



typedef struct
{
     int32_t s32C0;         /* Q15_16 */
     int16_t s16C1;         /* Q0_15 */
     int16_t s16C2;         /* Q0_15 */
     int16_t s16C1Shift;    /* s16C1Shift   increased resolution of coefficient C1 to fit best in 16 bits */
     int16_t s16C2Shift;    /* s16C2Shift   increased resolution of coefficient C2 to fit best in 16 bits */
} MCLIB_sPolynomSecondOrderFast_t;



/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * MCLIB_mValueLimitWithMaxMin - macro
 *
 * Description: Limits Value in Range LimitMin to LimitMax
 **************************************************************************** */
#define MCLIB_mValueLimitWithMaxMin(rValue,rLimitMax, rLimitMin) \
{ \
    if (rValue > rLimitMax) \
    { \
        rValue = rLimitMax; \
    } \
    if (rValue < rLimitMin) \
    { \
        rValue = rLimitMin; \
    } \
}

/* *****************************************************************************
 * MCLIB_s16d
 *
 * Description: signed 16 bit convert to deci-Degrees, Amperes, ... variable resolution
 **************************************************************************** */
#define MCLIB_s16d(x)   (int16_t)(10.0F * x)   /* dA(0.1A), dV(0.1V), dHz(0.1Hz) */
#define MCLIB_rToDeci(x)    (10.0F * x)   /* dA(0.1A), dV(0.1V), dHz(0.1Hz) */



/* *****************************************************************************
 * MCLIB_u16LOG2 
 * 
 * Description: unsigned 16 bit Ceil of Log with base 2
 **************************************************************************** */
/* log2(16)=4 log2(15)=4 */
#define LOG2_MATCH(nInput) ((nInput-1)*2)     
#define MCLIB_u16LOG2(nInput) (uint16_t)\
    ((0xff00 & LOG2_MATCH(nInput))\
        ? (0xf000 & LOG2_MATCH(nInput))\
            ? (0xc000 & LOG2_MATCH(nInput))\
                ? (0x8000 & LOG2_MATCH(nInput))\
                    ? 15U\
                    : 14U\
                : (0x2000 & LOG2_MATCH(nInput))\
                    ? 13U\
                    : 12U\
            : (0x0c00 & LOG2_MATCH(nInput))\
                ? (0x0800 & LOG2_MATCH(nInput))\
                    ? 11U\
                    : 10U\
                : (0x0200 & LOG2_MATCH(nInput))\
                    ? 9U\
                    : 8U\
        : (0x00f0 & LOG2_MATCH(nInput))\
            ? (0x00c0 & LOG2_MATCH(nInput))\
                ? (0x0080 & LOG2_MATCH(nInput))\
                    ? 7U\
                    : 6U\
                : (0x0020 & LOG2_MATCH(nInput))\
                    ? 5U\
                    : 4U\
            : (0x000c & LOG2_MATCH(nInput))\
                ? (0x0008 & LOG2_MATCH(nInput))\
                    ? 3U\
                    : 2U\
                : (0x0002 & LOG2_MATCH(nInput))\
                    ? 1U\
                    : 0U)



/* *****************************************************************************
 * MCLIB_s16Fractional 
 * 
 * Description: signed 16 bit fractional [-1..1] representation
 **************************************************************************** */
#define MCLIB_s16Fractional(fltValue) \
    ((int16_t) (((fltValue) < FRACT16_MAX) \
        ? (((fltValue) >= FRACT16_MIN) \
            ? ((int16_t) ((fltValue)*32768.0F + 0.5F)) \
            : (int16_t)INT16_MIN) \
        : (int16_t)INT16_MAX))

/* *****************************************************************************
 * MCLIB_s32Fractional 
 * 
 * Description: signed 32 bit fractional [-1..1] representation
 **************************************************************************** */
#define MCLIB_s32Fractional(fltValue)    \
    ((int32_t) (((fltValue) < FRACT32_MAX) \
        ? (((fltValue) >= FRACT32_MIN) \
            ? ((int32_t)((fltValue)*2147483648.0F + 0.5F)) \
            : (int32_t)INT32_MIN) \
        : (int32_t)INT32_MAX))

/* *****************************************************************************
 * MCLIB_s32FractionalQ_15_16 
 * 
 * Description: signed 32 bit fractional [-32768..32767.9999] representation
 **************************************************************************** */
#define MCLIB_s32FractionalQ_15_16(fltValue)    \
    ((int32_t) (((fltValue) < FRACTQ_15_16_MAX) \
        ? (((fltValue) >= FRACTQ_15_16_MIN) \
            ? ((int32_t)((fltValue)*65536.0F + 0.5F)) \
            : (int32_t)INT32_MIN) \
        : (int32_t)INT32_MAX))

/* *****************************************************************************
 * MCLIB_u16Fractional 
 * 
 * Description: unsigned 16 bit fractional [0..1] representation
 **************************************************************************** */
#define MCLIB_u16Fractional(fltValue)    \
    ((uint16_t) (((fltValue) < UFRACT16_MAX) \
        ? (((fltValue) >= 0) \
            ? ((uint16_t) ((fltValue)*65536.0F + 0.5F)) \
            : (uint16_t)0) \
        : (uint16_t)UINT16_MAX))

/* *****************************************************************************
 * MCLIB_u32Fractional 
 * 
 * Description: unsigned 32 bit fractional [0..1] representation
 **************************************************************************** */
#define MCLIB_u32Fractional(fltValue)    \
    ((uint32_t) (((fltValue) < UFRACT32_MAX) \
        ? (((fltValue) >= 0) \
            ? ((uint32_t)((fltValue)*4294967296.0F + 0.5F)) \
            : (uint32_t)0) \
        : (uint32_t)UINT32_MAX))

/* *****************************************************************************
 * MCLIB_u32FractionalQ_16_16 
 * 
 * Description: unsigned 32 bit fractional [0..65535.9999] representation
 **************************************************************************** */
#define MCLIB_u32FractionalQ_16_16(fltValue)    \
    ((uint32_t) (((fltValue) < UFRACTQ_16_16_MAX) \
        ? (((fltValue) >= 0) \
            ? ((uint32_t)((fltValue)*65536.0F + 0.5F)) \
            : (uint32_t)0) \
        : (uint32_t)UINT32_MAX))
          


/* roundup */
#define MCLIB_s16From_u32(u32Input) \
    (uint16_t)(((uint32_t)u32Input + 32768) >> 16)

/* roundup */
#define MCLIB_s16From_s32(s32Input) \
    (int16_t)(((int32_t)s32Input + 32768) >> 16)

#define MCLIB_u16From_u32(u32Input) \
    (uint16_t)(((uint32_t)u32Input + 32768) >> 16)

/* no roundup (trunked) */
#define MCLIB_s16From_u32T(u32Input) \
    (uint16_t)(((uint32_t)u32Input) >> 16)

#define MCLIB_s16FracFromFrac_u16(u16Input) \
    (int16_t)((uint16_t)u16Input >> 1)

/* negative input is saturated to 0 */
#define MCLIB_u16FracFromFrac_s16(s16Input) \
    ((int16_t)s16Input >= 0) \
        ?(uint16_t)((int16_t)s16Input << 1) \
        :(uint16_t)0

/* negative input is saturated to 0 */
#define MCLIB_u16From_s16(s16Input) \
    ((int16_t)s16Input >= 0) \
        ?(uint16_t)((int16_t)s16Input) \
        :(uint16_t)0

#define MCLIB_u16FracFromAbsFrac_s16(s16Input) \
    ((int16_t)s16Input >= 0) \
        ?(uint16_t)((uint16_t)(0+(int16_t)s16Input) << 1) \
        :(uint16_t)((uint16_t)(0-(int16_t)s16Input) << 1)


/* *****************************************************************************
 * MCLIB_s32AbsSat 
 * 
 * Description: signed 32 bit absolute value with saturation
 **************************************************************************** */
#define MCLIB_s32AbsSat(s32Input)    \
    (int32_t)(((int32_t)(s32Input) < 0) \
        ? MCLIB_s32NegSat(s32Input) \
        : (int32_t)(s32Input))

/* *****************************************************************************
 * MCLIB_s16AbsSat 
 * 
 * Description: signed 16 bit absolute value with saturation
 **************************************************************************** */
#define MCLIB_s16AbsSat(s16Input)    \
    (int16_t)(((int16_t)(s16Input) < 0) \
        ? MCLIB_s16NegSat(s16Input) \
        : (int16_t)(s16Input))

/* *****************************************************************************
 * MCLIB_s16NegSat 
 * 
 * Description: signed 16 bit negative value with saturation
 **************************************************************************** */
#define MCLIB_s16NegSat(s16Input)    \
    (int16_t)(((int16_t)(s16Input) == INT16_MIN) \
        ? INT16_MAX \
        : (-(int16_t)(s16Input)))



/* *****************************************************************************
 * MCLIB_s32NegSat 
 * 
 * Description: signed 32 bit negative value with saturation
 **************************************************************************** */
#define MCLIB_s32NegSat(s32Input)    \
    (int32_t)(((int32_t)(s32Input) == INT32_MIN) \
        ? INT32_MAX \
        : (-(int32_t)(s32Input)))





/* *****************************************************************************
 * MCLIB_u32AddSat_u32u32 
 * 
 * Description: unsigned 32 bit add with saturation
 **************************************************************************** */
#define MCLIB_u32AddSat_u32u32(u32Input1, u32Input2)    \
    ((uint32_t)(u32Input1 + u32Input2) < (uint32_t)u32Input1) \
        ? UINT32_MAX \
        : (uint32_t)(u32Input1 + u32Input2)

#define MCLIB_u16AddCarry_u32u32(u32Input1, u32Input2)    \
    ((uint32_t)(u32Input1 + u32Input2) < (uint32_t)u32Input1) \
        ? 1 \
        : 0

/* *****************************************************************************
 * MCLIB_u32SubSat_u32u32 
 * 
 * Description: unsigned 32 bit sub with saturation
 **************************************************************************** */
#define MCLIB_u32SubSat_u32u32(u32Input1, u32Input2)    \
    ((uint32_t)(u32Input1 - u32Input2) > (uint32_t)u32Input1) \
        ? 0 \
        : (uint32_t)(u32Input1 - u32Input2)



/* *****************************************************************************
 * MCLIB_s32AddSat_s32s32 
 * 
 * Description: signed 32 bit add with saturation
 **************************************************************************** */
#define MCLIB_s32AddSat_s32s32(s32Input1, s32Input2)    \
    (((int32_t)s32Input1 > 0) && ((int32_t)s32Input2 > 0) \
        && ((int32_t)(s32Input1 + s32Input2) < 0))\
            ? INT32_MAX \
            : (((int32_t)s32Input1 < 0) && ((int32_t)s32Input2 < 0) \
                && ((int32_t)(s32Input1 + s32Input2) > 0))\
                    ? INT32_MIN \
                    : (int32_t)(s32Input1 + s32Input2)

#define MCLIB_u16AddCarry_s32s32(s32Input1, s32Input2)    \
    (((int32_t)s32Input1 > 0) && ((int32_t)s32Input2 > 0) \
        && ((int32_t)(s32Input1 + s32Input2) < 0))\
            ? 1 \
            : (((int32_t)s32Input1 < 0) && ((int32_t)s32Input2 < 0) \
                && ((int32_t)(s32Input1 + s32Input2) > 0))\
                    ? 1 \
                    : 0



/* *****************************************************************************
 * MCLIB_u32AddSat_u32s32
 *
 * Description: unsigned and signed 32 bit add with saturation. Result unsigned
 **************************************************************************** */
#define MCLIB_u32AddSat_u32s32(u32Input1, s32Input2)    \
    ( ((int32_t)s32Input2 > 0) \
        && ((uint32_t)((uint32_t)u32Input1 + (uint32_t)s32Input2) < (uint32_t)u32Input1))\
            ? UINT32_MAX \
            : (((int32_t)s32Input2 < 0) \
                && ((uint32_t)(u32Input1 + s32Input2) > (uint32_t)u32Input1))\
                    ? 0 \
                    : (uint32_t)((uint32_t)u32Input1 + (uint32_t)s32Input2)


/* *****************************************************************************
 * MCLIB_s32SubSat_s32s32 
 * 
 * Description: signed 32 bit sub with saturation
 **************************************************************************** */
#define MCLIB_s32SubSat_s32s32(s32Input1, s32Input2)    \
    (((int32_t)s32Input1 > 0) && ((int32_t)s32Input2 < 0) \
        && ((int32_t)(s32Input1 - s32Input2) < 0))\
            ? INT32_MAX \
            : (((int32_t)s32Input1 < 0) && ((int32_t)s32Input2 > 0) \
                && ((int32_t)(s32Input1 - s32Input2) > 0))\
                    ? INT32_MIN \
                    : (int32_t)(s32Input1 - s32Input2)


/* *****************************************************************************
 * MCLIB_u16AddSat_u16u16 
 * 
 * Description: unsigned 16 bit add with saturation
 **************************************************************************** */
#define MCLIB_u16AddSat_u16u16(u16Input1, u16Input2)    \
    ((uint16_t)(u16Input1 + u16Input2) < (uint16_t)u16Input1) \
        ? UINT16_MAX \
        : (uint16_t)(u16Input1 + u16Input2)

/* *****************************************************************************
 * MCLIB_u16SubSat_u16u16 
 * 
 * Description: unsigned 16 bit sub with saturation
 **************************************************************************** */
#define MCLIB_u16SubSat_u16u16(u16Input1, u16Input2)    \
    ((uint16_t)(u16Input1 - u16Input2) > (uint16_t)u16Input1) \
        ? 0 \
        : (uint16_t)(u16Input1 - u16Input2)



/* *****************************************************************************
 * MCLIB_s16Sub 
 * 
 * Description: signed 16 bit add no saturation
 **************************************************************************** */
#define MCLIB_s16Sub(s16Input1, s16Input2)    \
    (int16_t)((s16Input1) - (s16Input2))


/* *****************************************************************************
 * MCLIB_u16Sub 
 * 
 * Description: unsigned 16 bit add no saturation
 **************************************************************************** */
#define MCLIB_u16Sub(u16Input1, u16Input2)    \
    (uint16_t)((u16Input1) - (u16Input2))


/* *****************************************************************************
 * MCLIB_s16Add_s16s16 
 * 
 * Description: signed 16 bit add no saturation
 **************************************************************************** */
#define MCLIB_s16Add_s16s16(s16Input1, s16Input2)    \
    (int16_t)((s16Input1) + (s16Input2))


/* *****************************************************************************
 * MCLIB_s16AddSat_s16s16 
 * 
 * Description: signed 16 bit add with saturation
 **************************************************************************** */
#define MCLIB_s16AddSat_s16s16(s16Input1, s16Input2)    \
    (((int16_t)(s16Input1) > 0) && ((int16_t)(s16Input2) > 0) \
        && ((int16_t)((s16Input1) + (s16Input2)) < 0))\
            ? INT16_MAX \
            : (((int16_t)(s16Input1) < 0) && ((int16_t)(s16Input2) < 0) \
                && ((int16_t)((s16Input1) + (s16Input2)) > 0))\
                    ? INT16_MIN \
                    : (int16_t)((s16Input1) + (s16Input2))

/* *****************************************************************************
 * MCLIB_s16SubSat_s16s16 
 * 
 * Description: signed 16 bit sub with saturation
 **************************************************************************** */
#define MCLIB_s16SubSat_s16s16(s16Input1, s16Input2)    \
    (((int16_t)(s16Input1) > 0) && ((int16_t)(s16Input2) < 0) \
        && ((int16_t)((s16Input1) - (s16Input2)) < 0))\
            ? INT16_MAX \
            : (((int16_t)(s16Input1) < 0) && ((int16_t)(s16Input2) > 0) \
                && ((int16_t)((s16Input1) - (s16Input2)) > 0))\
                    ? INT16_MIN \
                    : (int16_t)((s16Input1) - (s16Input2))



/* *****************************************************************************
 * MCLIB_s16Mul_s16s16 
 * 
 * Description: signed 16 bit mul with saturation
 **************************************************************************** */
#define MCLIB_s16Mul_s16s16(s16Input1, s16Input2)    \
    (((int16_t)(s16Input1) == INT16_MIN)&&((int16_t)(s16Input2) == INT16_MIN)) \
            ? INT16_MAX \
            : ((int16_t)((int32_t)((int32_t)(s16Input1) * (s16Input2)) >> 15))

/* *****************************************************************************
 * MCLIB_s16Mul_s16u16 
 * 
 * Description: signed and unsigned 16 bit mul with round
 **************************************************************************** */
#define MCLIB_s16Mul_s16u16(s16Input1, u16Input2)    \
    (int16_t)((int32_t)(((int32_t)(s16Input1) * (u16Input2))+ MCLIB_u16Fractional(0.5)) >> 16)


/* *****************************************************************************
 * MCLIB_s16MulSat_s16u16Int 
 * 
 * Description: signed and unsigned 16 bit Integer(not fractional) mul with round
 **************************************************************************** */
#define MCLIB_s16MulSat_s16u16Int(s16Input1, u16Input2)    \
(s16Input1 > 0) \
        ? (((int32_t)(s16Input1) * (u16Input2)) > 0) \
                ? ((int32_t)(s16Input1) * (u16Input2)) \
                : INT16_MAX \
        : (((int32_t)(s16Input1) * (u16Input2)) <= 0) \
                ? ((int32_t)(s16Input1) * (u16Input2)) \
                : INT16_MIN


/* *****************************************************************************
 * MCLIB_s32Mul_s16s16 
 * 
 * Description: signed 16 bit mul with 32 bit signed output 
 **************************************************************************** */
#ifdef __XC16
#if 0   /* Incorrect Math Calculation */
#define MCLIB_s32Mul_s16s16(s16Input1, s16Input2) \
    __builtin_mulss(s16Input1, s16Input2)
#endif
#define MCLIB_s32Mul_s16s16(s16Input1, s16Input2)    \
    (int32_t)(((int32_t)(s16Input1)*(s16Input2)) << 1U)
#else
#define MCLIB_s32Mul_s16s16(s16Input1, s16Input2)    \
    (int32_t)(((int32_t)(s16Input1)*(s16Input2)) << 1U)
#endif

/* *****************************************************************************
 * MCLIB_u32Mul_u16u16
 * 
 * Description: unsigned 16 bit mul with 32 bit unsigned output 
 **************************************************************************** */
/* Function below not checked used in interpolation */
#ifdef __XC16
#define MCLIB_u32Mul_u16u16(u16Input1, u16Input2) \
    __builtin_muluu(u16Input1, u16Input2)
#else
#define MCLIB_u32Mul_u16u16(u16Input1, u16Input2)    \
    (uint32_t)((uint32_t)(u16Input1)*(u16Input2))
#endif

/* *****************************************************************************
 * MCLIB_s16Div_s32u16
 * 
 * Description: signed 32 bit div with 16 bit signed output 
 **************************************************************************** */
#if 0
//#ifdef __XC16
#define MCLIB_s16Div_s32u16(s32Input1, u16Input2) \
    __builtin_divsud(s32Input1, u16Input2)
#else
#define MCLIB_s16Div_s32u16(s32Input1, u16Input2)    \
    (int16_t)((int32_t)(s32Input1)/(u16Input2))
#endif

/* *****************************************************************************
 * MCLIB_s16DivSat_s32u16
 * 
 * Description: signed 32 bit div with 16 bit signed output 
 **************************************************************************** */
#define MCLIB_s16DivSat_s32u16(s32Input1, u16Input2) \
    (u16Input2 == 0) \
        ? (INT16_MAX) \
        : (MCLIB_s32AbsSat(s32Input1) < (int32_t)((uint32_t)u16Input2 << 15)) \
            ? MCLIB_s16Div_s32u16(s32Input1, u16Input2) \
            : ((s32Input1) >= 0) \
                ? (INT16_MAX) \
                : (INT16_MIN)




/* *****************************************************************************
 * MCLIB_u16Div_u32u16
 * 
 * Description: unsigned 32 bit div with 16 bit unsigned output 
 **************************************************************************** */
#ifdef __XC16
#define MCLIB_u16Div_u32u16(u32Input1, u16Input2) \
    __builtin_divud(u32Input1, u16Input2)
#else
#define MCLIB_u16Div_u32u16(u32Input1, u16Input2)    \
    (uint16_t)((uint32_t)(u32Input1)/(u16Input2))
#endif

/* *****************************************************************************
 * MCLIB_u16DivSat_u32u16
 * 
 * Description: unsigned 32 bit div with 16 bit unsigned output 
 **************************************************************************** */
#define MCLIB_u16DivSat_u32u16(u32Input1, u16Input2) \
    (u16Input2 == 0) \
        ? (UINT16_MAX) \
        : ((u32Input1-1) > ((uint32_t)u16Input2 << 16)) \
            ? (UINT16_MAX) \
            : MCLIB_u16Div_u32u16(u32Input1, u16Input2) 



/* *****************************************************************************
 * MCLIB_s16Div_s32s16
 * 
 * Description: signed 32 bit div with 16 bit signed output 
 **************************************************************************** */
#define MCLIB_s32Div_s32s16(s32Input1, s16Input2)    \
    (int32_t)((int32_t)(s32Input1)/(s16Input2))



/* *****************************************************************************
 * MCLIB_u32Div_u32u16
 *
 * Description: unsigned 32 bit div with 16 bit unsigned output
 **************************************************************************** */
#define MCLIB_u32Div_u32u16(u32Input1, u16Input2)    \
    (uint32_t)((uint32_t)(u32Input1)/(u16Input2))

/* *****************************************************************************
 * MCLIB_s32Div_s32u16
 *
 * Description: unsigned 32 bit div with 16 bit unsigned output
 **************************************************************************** */
#define MCLIB_s32Div_s32u16(s32Input1, u16Input2)    \
    (int32_t)((int32_t)(s32Input1)/(u16Input2))


/* *****************************************************************************
 * MCLIB_s16Div_s32s16
 *
 * Description: signed 32 bit div with 16 bit signed output
 **************************************************************************** */
#ifdef __XC16
#define MCLIB_s16Div_s32s16(s32Input1, s16Input2) \
    __builtin_divsd(s32Input1, s16Input2)

#else
#define MCLIB_s16Div_s32s16(s32Input1, s16Input2)    \
    (int16_t)((int32_t)(s32Input1)/(s16Input2))
#endif

/* *****************************************************************************
 * MCLIB_s16DivSat_s32s16
 * 
 * Description: signed 32 bit div with 16 bit signed output 
 **************************************************************************** */
#define MCLIB_s16DivSat_s32s16(s32Input1, s16Input2) \
    (s16Input2 == 0) \
        ? (INT16_MAX) \
        : (MCLIB_s32AbsSat(s32Input1) < MCLIB_s32AbsSat((int32_t)((int32_t)s16Input2 << 15))) \
            ? MCLIB_s16Div_s32u16(s32Input1, u16Input2) \
            : ( (((s32Input1) >= 0) && ((s32Input1) >= 0)) || (((s32Input1) < 0) && ((s32Input1) < 0)) )\
                ? (INT16_MAX) \
                : (INT16_MIN)


/* *****************************************************************************
 * MCLIB_s16Mul_s16u32 - -use MCLIB_s16MulSat_s16u32 if possible overflow
 * 
 * Input1 - signed fractional[-1..1] or signed int[-32768..32768]
 * Input2 - unsigned 32 bit fractional [0..65535.9999] Q_16_16
 * 
 * Output - signed fractional[-1..1] or signed int[-32768..32768]
 * 
 * Description: signed 16 bit mul with 32 bit u32 Q_16_16 format. 
 *              Result is signed 16 bit output 
 **************************************************************************** */
#define MCLIB_s16Mul_s16u32(s16Input1, u32Input2_Q_16_16) \
        /* (u32Input2_Q_16_16 == MCLIB_u32FractionalQ_16_16(1.0)) ? s16Input1 : */\
        (int16_t)((((int16_t)(s16Input1) * (u32Input2_Q_16_16)) + \
        MCLIB_u16Fractional(0.5))>>16)

#define MCLIB_s16Mul_s16s32(s16Input1, s32Input2_Q_15_16) \
        (int16_t)((((int64_t)(s16Input1) * (s32Input2_Q_15_16)) + \
        MCLIB_u16Fractional(0.5))>>16)


/* *****************************************************************************
 * MCLIB_s16MulSat_s16u32 -
 *
 * Input1 - signed fractional[-1..1] or signed int[-32768..32768]
 * Input2 - unsigned 32 bit fractional [0..65535.9999] Q_16_16
 *
 * Output - signed fractional[-1..1] or signed int[-32768..32768]
 *
 * Description: signed 16 bit mul with 32 bit u32 Q_16_16 format.
 *              Result is Saturated signed 16 bit output
 **************************************************************************** */
#define MCLIB_s16MulSat_s16u32(s16Input1, u32Input2_Q_16_16) \
        ((int16_t)(s16Input1) > 0) && ((int64_t)((int16_t)(s16Input1) * (u32Input2_Q_16_16)) < 0)    \
            ? INT16_MAX \
            : ((int16_t)(s16Input1) < 0) && ((int64_t)((int16_t)(s16Input1) * (u32Input2_Q_16_16)) > 0)    \
                  ? INT16_MIN \
                  : (int16_t)((((int16_t)(s16Input1) * (u32Input2_Q_16_16)) + MCLIB_u16Fractional(0.5))>>16)


/* *****************************************************************************
 * MCLIB_u16Mul_u16u32
 *
 * Input1 - unsigned fractional[0..1] or unsigned int[0..65536]
 * Input2 - unsigned 32 bit fractional [0..65535.9999] Q_16_16
 *
 * Output - unsigned fractional[0..1] or unsigned int[0..65536]
 *
 * Description: unsigned 16 bit mul with 32 bit u32 Q_16_16 format.
 *              Result is unsigned 16 bit output
 **************************************************************************** */
#define MCLIB_u16Mul_u16u32(u16Input1, u32Input2_Q_16_16) \
        (uint16_t)((((uint16_t)(u16Input1) * (u32Input2_Q_16_16)) + \
        MCLIB_u16Fractional(0.5))>>16)

#if 0
/* *****************************************************************************
 * MCLIB_s16Mul_u16u32
 *
 * Input1 - unsigned fractional[0..1] or unsigned int[0..65536]
 * Input2 - unsigned 32 bit fractional [0..65535.9999] Q_16_16
 *
 * Output - signed fractional[-1..1] or signed int[-32768..32768]
 *
 * Description: unsigned 16 bit mul with 32 bit u32 Q_16_16 format.
 *              Result is signed 16 bit output
 **************************************************************************** */
#define MCLIB_s16Mul_u16u32(u16Input1, u32Input2_Q_16_16) \
        (int16_t)((((uint16_t)(u16Input1) * (u32Input2_Q_16_16)) + \
        MCLIB_u16Fractional(0.5))>>(16-1))
#endif

/* *****************************************************************************
 * MCLIB_s32Mul_s16u32 - -needs improvement to check overflow
 * 
 * Input1 - signed fractional[-1..1] or signed int[-32768..32768]
 * Input2 - unsigned 32 bit fractional [0..65535.9999] Q_16_16
 * 
 * Output - signed fractional[-1..1] or signed int[-32768..32768]
 * 
 * Description: signed 16 bit mul with 32 bit u32 Q_16_16 format. 
 *              Result is signed 16 bit output 
 **************************************************************************** */
#define MCLIB_s32Mul_s16u32(s16Input1, u32Input2_Q_16_16) \
        (int32_t)(((int32_t)(s16Input1) * (u32Input2_Q_16_16)) + \
        MCLIB_u16Fractional(0.5))


/* *****************************************************************************
 * MCLIB_s32Mul_s32u32 - -needs improvement to check overflow
 * 
 * Input1 - signed 32 bit fractional [-32768..32767.9999] Q_15_16
 * Input2 - unsigned 32 bit fractional [0..65535.9999] Q_16_16
 * 
 * Output - signed 32 bit fractional [-32768..32767.9999] Q_15_16
 * 
 * Description: signed 16 bit mul with 32 bit u32 Q_16_16 format. 
 *              Result is signed 16 bit output 
 **************************************************************************** */
#define MCLIB_s32Mul_s32u32(s32Input1_Q_15_16, u32Input2_Q_16_16) \
        (int32_t)((((int64_t)(s32Input1_Q_15_16) * (u32Input2_Q_16_16)) + \
        MCLIB_u16Fractional(0.5)) >> 16)

#define MCLIB_s16Mul_s32u32(s32Input1_Q_15_16, u32Input2_Q_16_16) \
        (int16_t)((((int64_t)(s32Input1_Q_15_16) * (u32Input2_Q_16_16)) + \
        MCLIB_u16Fractional(0.5)) >> 16)



/* *****************************************************************************
 * MCLIB_u32Mul_u32u32 - -needs improvement to check overflow
 * 
 * Input1 - unsigned 32 bit fractional [0..65535.9999] Q_16_16
 * Input2 - unsigned 32 bit fractional [0..65535.9999] Q_16_16
 * 
 * Output - unsigned 32 bit fractional [0..65535.9999] Q_16_16
 * 
 * Description: unsigned u32 Q_16_16 format mul with u32 Q_16_16 format. 
 *              Result is unsigned 32 bit u32 Q_16_16 format output 
 **************************************************************************** */
#define MCLIB_u32Mul_u32u32(u32Input1_Q_16_16, u32Input2_Q_16_16) \
        (uint32_t)((((uint64_t)(u32Input1_Q_16_16) * (u32Input2_Q_16_16)) + \
        MCLIB_u16Fractional(0.5)) >> 16)

#define MCLIB_u16Mul_u32u32(u32Input1_Q_16_16, u32Input2_Q_16_16) \
        (uint16_t)((((uint64_t)(u32Input1_Q_16_16) * (u32Input2_Q_16_16)) + \
        MCLIB_u16Fractional(0.5)) >> 16)



/* *****************************************************************************
 * MCLIB_u16Mul_u16u16
 * 
 * Input1,Input2 - unsigned fractional[0..1] or unsigned int[0..65535]
 * Output - unsigned fractional[0..1] or unsigned int[0..65535]
 * 
 * Description: signed 16 bit mul with 32 bit u32 Q_16_16 format. 
 *              Result is signed 16 bit output 
 **************************************************************************** */
#ifdef __XC16
#define MCLIB_u16Mul_u16u16(u16Input1, u16Input2) \
        (uint16_t)((__builtin_muluu(u16Input1, u16Input2) + \
        MCLIB_u16Fractional(0.5))>>16)
#else
#define MCLIB_u16Mul_u16u16(u16Input1, u16Input2) \
        (uint16_t)((((uint32_t)(u16Input1) * (u16Input2)) + \
        MCLIB_u16Fractional(0.5))>>16)
#endif
#define MCLIB_u16Mul_u16u16Std(u16Input1, u16Input2) \
        (uint16_t)((((uint32_t)(u16Input1) * (u16Input2)) + \
        MCLIB_u16Fractional(0.5))>>16)



/* *****************************************************************************
 * MCLIB_s16Power2 
 * 
 * Description: signed 16 bit raise on power of 2
 **************************************************************************** */
#define MCLIB_s16Power2(s16Input1) MCLIB_s16Mul_s16s16(s16Input1, s16Input1)

/* *****************************************************************************
 * MCLIB_s32Power2 - do not use with fractionals (use with > 1 numbers)
 * 
 * Description: signed 16 bit raise on power of 2
 **************************************************************************** */
#define MCLIB_s32Power2(s16Input1) (int32_t)(((int32_t)(s16Input1)*(s16Input1)))



/* *****************************************************************************
 * MCLIB_s16Power2
 *
 * Description: unsigned 16 bit raise on power of 2
 **************************************************************************** */
#define MCLIB_u16Power2(u16Input1) MCLIB_u16Mul_u16u16(u16Input1, u16Input1)
#define MCLIB_u32Power2(u16Input1) MCLIB_u32Mul_u16u16(u16Input1, u16Input1)
        

/* *****************************************************************************
 * MCLIB_s32ShL
 * 
 * Description: signed 32 bit shift left no saturation
 **************************************************************************** */
#define MCLIB_s32ShL(s32Input, u16Shift)    \
    (int32_t)(((int32_t)(s32Input) << (uint16_t)(u16Shift)))


/* *****************************************************************************
 * MCLIB_u32ShL
 *
 * Description: unsigned 32 bit shift left no saturation
 **************************************************************************** */
#define MCLIB_u32ShL(u32Input, u16Shift)    \
    (uint32_t)(((uint32_t)(u32Input) << (uint16_t)(u16Shift)))


/* *****************************************************************************
 * MCLIB_s32ShR
 *
 * Description: signed 32 bit shift right no saturation
 **************************************************************************** */
#define MCLIB_s32ShR(s32Input, u16Shift)    \
    (int32_t)(((int32_t)(s32Input) >> (uint16_t)(u16Shift)))


/* *****************************************************************************
 * MCLIB_s32ShLSat 
 * 
 * Description: signed 32 bit shift left with saturation
 **************************************************************************** */
#define MCLIB_s32ShLSat(s32Input, u16Shift)    \
    ((int32_t)(s32Input) >= 0) \
    	? ( (  ((int64_t)(s32Input) << (uint16_t)(u16Shift)) > INT32_MAX) \
                ? INT32_MAX \
                : (int32_t)((int32_t)(s32Input) << (uint16_t)(u16Shift)) ) \
    	: ( (  ((int64_t)(s32Input) << (uint16_t)(u16Shift)) < INT32_MIN) \
                ? INT32_MIN \
                : (int32_t)((int32_t)(s32Input) << (uint16_t)(u16Shift)) )   





/* *****************************************************************************
 * MCLIB_s16Sat 
 * 
 * Description: signed 16 bit saturation
 **************************************************************************** */
#define MCLIB_s16Sat(s16Input) \
    ((int32_t)(s16Input) > 0 ) \
    	? (((int32_t)(s16Input) > INT16_MAX ) \
            ? INT16_MAX \
            : (int16_t)((int32_t)(s16Input))) \
    	: (((int32_t)(s16Input) < INT16_MIN ) \
            ? INT16_MIN \
            : (int16_t)((int32_t)(s16Input)))

/* *****************************************************************************
 * MCLIB_s16ShL 
 * 
 * Description: signed 16 bit shift left no saturation
 **************************************************************************** */
#define MCLIB_s16ShL(s16Input, u16Shift)    \
    (int16_t)((s16Input) << (uint16_t)(u16Shift))

/* *****************************************************************************
 * MCLIB_s16ShLSat 
 * 
 * Description: signed 16 bit shift left with saturation
 **************************************************************************** */
#define MCLIB_s16ShLSat(s16Input, u16Shift)    \
    (((int32_t)(s16Input) << (uint16_t)(u16Shift)) > INT16_MAX) \
        ? INT16_MAX \
        : (((int32_t)(s16Input) << (uint16_t)(u16Shift)) < INT16_MIN) \
            ? INT16_MIN \
            : (int16_t)((int32_t)(s16Input) << (uint16_t)(u16Shift))


/* *****************************************************************************
 * MCLIB_s16ShR
 *
 * Description: signed 16 bit shift right no saturation
 **************************************************************************** */
#define MCLIB_s16ShR(s16Input, u16Shift)    \
    (int16_t)((s16Input) >> (uint16_t)(u16Shift))



/* *****************************************************************************
 * MCLIB_s16Max 
 * 
 * Description: signed 16 bit max find
 **************************************************************************** */
#define MCLIB_s16Max(s16Input1, s16Input2)    \
    (int16_t)(((s16Input1) > (s16Input2)) ? (s16Input1) : (s16Input2))


/* *****************************************************************************
 * MCLIB_u16Max - same as MCLIB_s16Max
 * 
 * Description: unsigned 16 bit max find
 **************************************************************************** */
#define MCLIB_u16Max(u16Input1, u16Input2)    \
    (uint16_t)(((u16Input1) > (u16Input2)) ? (u16Input1) : (u16Input2))


/* *****************************************************************************
 * MCLIB_s16Min 
 * 
 * Description: signed 16 bit min find
 **************************************************************************** */
#define MCLIB_s16Min(s16Input1, s16Input2)    \
    (int16_t)(((s16Input1) < (s16Input2)) ? (s16Input1) : (s16Input2))


/* *****************************************************************************
 * MCLIB_u16Min - same as MCLIB_s16Min
 * 
 * Description: unsigned 16 bit min find
 **************************************************************************** */
#define MCLIB_u16Min(u16Input1, u16Input2)    \
    (uint16_t)(((u16Input1) < (u16Input2)) ? (u16Input1) : (u16Input2))




/* *****************************************************************************
 * MCLIB_u32PolynomSecondOrder - still not working or tested
 * 
 * s16X         fractional Q0.16[0..1]
 * 
 * s32Return    fractional Q16.16[-65536..65535.9999]
 *              high 16 bits whole part int16_t, low 16 bits fractional part
 * 
 * u16C2Shift   increased resolution of coefficient C2 to fit best in 16 bits
 * u16C1Shift   increased resolution of coefficient C1 to fit best in 16 bits
 * 
 * Description: Calculate second order polynom: Y = X*X*C2 + X*C1 + C0
 **************************************************************************** */
#define MCLIB_u32PolynomSecondOrder(\
    u16X,s16C2,u16C2Shift,s16C1,u16C1Shift,s32C0) \
    ((((((int32_t)s16C2*u16X)>>16)*u16X)>>u16C2Shift) + \
    (((int32_t)s16C1*u16X)>>u16C1Shift) + s32C0)

/* *****************************************************************************
 * MCLIB_s32PolynomSecondOrder
 * 
 * s16X         fractional Q0.15[-1..1]
 * 
 * s32Return    fractional Q15.16[-32768..32767.9999]
 *              high 16 bits whole part int16_t, low 16 bits fractional part
 * 
 * u16C2Shift   increased resolution of coefficient C2 to fit best in 16 bits
 * u16C1Shift   increased resolution of coefficient C1 to fit best in 16 bits
 * 
 * Description: Calculate second order polynom: Y = X*X*C2 + X*C1 + C0
 **************************************************************************** */
#define MCLIB_s32PolynomSecondOrder(\
    s16X,s16C2,u16C2Shift,s16C1,u16C1Shift,s32C0) \
    (((((s16C2*(int32_t)s16X)>>15)*(int32_t)s16X)>>u16C2Shift) + \
    ((s16C1*(int32_t)s16X)>>u16C1Shift) + s32C0)

/* *****************************************************************************
 * MCLIB_s32PolynomThirdOrder - only snippet - to be made
 * 
 * s16X         fractional Q0.15[-1..1]
 * 
 * s32Return    fractional Q15.16[-32768..32767.9999]
 *              high 16 bits whole part int16_t, low 16 bits fractional part
 * 
 * u16C3Shift   increased resolution of coefficient C3 to fit best in 16 bits
 * u16C2Shift   increased resolution of coefficient C2 to fit best in 16 bits
 * u16C1Shift   increased resolution of coefficient C1 to fit best in 16 bits
 * 
 * Description: Calculate second order polynom: Y = X*X*X*C3 +X*X*C2 +X*C1 +C0
 **************************************************************************** */
#define MCLIB_s32PolynomThirdOrder(\
    s16X,s16C3,u16C3Shift,s16C2,u16C2Shift,s16C1,u16C1Shift,s32C0) \
    (((((((s16C3*(int32_t)s16X)>>15)*(int32_t)s16X)>>15)* \
        (int32_t)s16X)>>u16C3Shift) + \
    ((((s16C2*(int32_t)s16X)>>15)*(int32_t)s16X)>>u16C2Shift) + \
    ((s16C1*(int32_t)s16X)>>u16C1Shift) + s32C0)











#define MCLIB_FILTER_SAMPLES_256    8
#define MCLIB_FILTER_SAMPLES_512    9
#define MCLIB_FILTER_SAMPLES_1024   10
#define MCLIB_FILTER_SAMPLES_2048   11
#define MCLIB_FILTER_SAMPLES_4096   12






#define MCLIB_ONE_DIV_SQRT_3    (0.57735026918962576450914878050196F)
#define MCLIB_SQRT_3_DIV_TWO    (0.86602540378443864676372317075294F)

#define MCLIB_ONE_DIV_SQRT_2    (0.70710678118654752440084436210485F)
#define MCLIB_SQRT_2_DIV_TWO    (0.70710678118654752440084436210485F)

#define MCLIB_SQRT_2            (1.4142135623730950488016887242097F)
#define MCLIB_SQRT_3            (1.7320508075688772935274463415059F)
#define MCLIB_SQRT_6            (2.4494897427831780981972840747059F)
#define MCLIB_SQRT_3_DIV_SQRT_2 (1.2247448713915890490986420373529F)
#define MCLIB_SQRT_2_DIV_SQRT_3 (0.81649658092772603273242802490196F)


#define MCLIB_3PH_RMS_TO_AMP    MCLIB_SQRT_2_DIV_SQRT_3
#define MCLIB_1PH_RMS_TO_AMP    MCLIB_SQRT_2
#define MCLIB_1PH_TO_3PH_RMS    MCLIB_SQRT_3


#define MCLIB_NO_MODULATION_INDEX  (1.0F)   /* Do not use -1.0 because DC-Link Scale function Overflows */

/* SVM Modulation */
#define MCLIB_SVM_AMP_MODULATION_INDEX  MCLIB_SQRT_3_DIV_TWO


/* Sine-wave Modulation */
#define MCLIB_SIN_AMP_MODULATION_INDEX  MCLIB_NO_MODULATION_INDEX


/* *****************************************************************************
 * MCLIB_vClarke
 * 
 * psAlBe       Alpha-Beta Vectors
 * 
 * psABC        ABC Vectors 
 * 
 * Description: Clarke Transformation (ABC -> AlBe)
 **************************************************************************** */
#define MCLIB_vClarke(psAlBe, psABC)    \
    /* Alfa = A */\
    (psAlBe)->s16Arg1 = (psABC)->s16Arg1; \
    /* Beta = 1/SQRT(3) * (B-C)	*/ \
    (psAlBe)->s16Arg2 = MCLIB_s16SubSat_s16s16(\
        MCLIB_s16Mul_s16s16(\
            (psABC)->s16Arg2, MCLIB_s16Fractional(MCLIB_ONE_DIV_SQRT_3)), \
        MCLIB_s16Mul_s16s16(\
            (psABC)->s16Arg3, MCLIB_s16Fractional(MCLIB_ONE_DIV_SQRT_3)))



/* *****************************************************************************
 * MCLIB_vClarkeInv
 * 
 * psAlBe       Alpha-Beta Vectors
 * 
 * psABC        ABC Vectors 
 * 
 * Description: Inverse Clarke Transformation (AlBe -> ABC)
 **************************************************************************** */
#define MCLIB_vClarkeInv(psABC, psAlBe, s16Temp1, s16Temp2) \
    int16_t s16Temp1;\
    int16_t s16Temp2;\
    s16Temp1 = (psAlBe)->s16Arg1;\
    /* A = Alfa */\
    (psABC)->s16Arg1 = s16Temp1;\
    s16Temp1 = MCLIB_s16NegSat(s16Temp1 >> 1);\
    s16Temp2 = MCLIB_s16Mul_s16s16(\
        (psAlBe)->s16Arg2, MCLIB_s16Fractional(MCLIB_SQRT_3_DIV_TWO));\
    /* B = -Alfa/2 +SQRT(3)/2*Beta */\
    (psABC)->s16Arg2 = MCLIB_s16AddSat_s16s16(s16Temp1, s16Temp2);\
    /* C = -Alfa/2 -SQRT(3)/2*Beta */\
    (psABC)->s16Arg3 = MCLIB_s16SubSat_s16s16(s16Temp1, s16Temp2);



/* *****************************************************************************
 * MCLIB_vPark
 * 
 * psDQ         Direct-Quadrature Vectors 
 * 
 * psAlBe       Alpha-Beta Vectors
 * 
 * psSinCos     Sine & Cosine
 * 
 * Description: Park Transformation (AlBe -> DQ)
 **************************************************************************** */
#define MCLIB_vPark(psDQ, psSinCos, psAlBe)    \
    (psDQ)->s16Arg1 = (int16_t)(MCLIB_s32AddSat_s32s32(\
        MCLIB_s32Mul_s16s16((psSinCos)->s16Arg2, (psAlBe)->s16Arg1), \
        MCLIB_s32Mul_s16s16((psSinCos)->s16Arg1, (psAlBe)->s16Arg2)) >> 16);\
    (psDQ)->s16Arg2 = (int16_t)(MCLIB_s32SubSat_s32s32(\
        MCLIB_s32Mul_s16s16((psSinCos)->s16Arg2, (psAlBe)->s16Arg2), \
        MCLIB_s32Mul_s16s16((psSinCos)->s16Arg1, (psAlBe)->s16Arg1)) >> 16)


/* *****************************************************************************
 * MCLIB_vParkInv
 * 
 * psDQ         Direct-Quadrature Vectors 
 * 
 * psAlBe       Alpha-Beta Vectors
 * 
 * psSinCos     Sine & Cosine
 * 
 * Description: Inverse Park Transformation (DQ -> AlBe)
 **************************************************************************** */
#define MCLIB_vParkInv(psAlBe, psSinCos, psDQ)    \
    (psAlBe)->s16Arg1 = (int16_t)(MCLIB_s32SubSat_s32s32(\
        MCLIB_s32Mul_s16s16((psSinCos)->s16Arg2, (psDQ)->s16Arg1), \
        MCLIB_s32Mul_s16s16((psSinCos)->s16Arg1, (psDQ)->s16Arg2)) >> 16);\
    (psAlBe)->s16Arg2 = (int16_t)(MCLIB_s32AddSat_s32s32(\
        MCLIB_s32Mul_s16s16((psSinCos)->s16Arg2, (psDQ)->s16Arg2), \
        MCLIB_s32Mul_s16s16((psSinCos)->s16Arg1, (psDQ)->s16Arg1)) >> 16)


/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void MCLIB_vLinearInterpolationMultiplierInit(
        MCLIB_sLinearInterpolation_t* psInterpolation);

uint16_t MCLIB_u16LinearInterpolation(
        uint16_t u16Input,         
        MCLIB_sLinearInterpolation_t* psInterpolation);

void MCLIB_vLinearInterpolationSignedMultiplierInit(
        MCLIB_sLinearInterpolationSigned_t* psInterpolation);

int16_t MCLIB_s16LinearInterpolationSigned(       
        int16_t s16Input, 
        MCLIB_sLinearInterpolationSigned_t* psInterpolation);

int32_t MCLIB_s32PolynomSecondOrder_s32(
        int32_t s32X,
        int16_t s16C2,
        int16_t s16C2Shift,
        int16_t s16C1,
        int16_t s16C1Shift,
        int32_t s32C0);

int32_t MCLIB_s32PolynomSecondOrderFast_s32(
        int32_t s32X,
        MCLIB_sPolynomSecondOrderFast_t* psPolynom);

uint32_t MCLIB_u32RampLinear(
        uint32_t u32Input, 
        MCLIB_sRamp_t* psRamp);

void MCLIB_u16FilterAvgInit(
        uint16_t u16NSamples, 
        MCLIB_sFilterAVGu16_t* psFilterMA);

void MCLIB_s16FilterAvgInit(
        uint16_t u16NSamples, 
        MCLIB_sFilterAVGs16_t* psFilterMA);

uint16_t MCLIB_u16FilterAvg(
        uint16_t u16Input, 
        MCLIB_sFilterAVGu16_t* psFilterMA);

int16_t MCLIB_s16FilterAvg(
        int16_t s16Input, 
        MCLIB_sFilterAVGs16_t* psFilterMA);

void MCLIB_u32FilterAvgInit(
        uint16_t u16NSamples,
        MCLIB_sFilterAVGu32_t* psFilterMA);

void MCLIB_s32FilterAvgInit(
        uint16_t u16NSamples,
        MCLIB_sFilterAVGs32_t* psFilterMA);

uint32_t MCLIB_u32FilterAvg(
        uint32_t u32Input,
        MCLIB_sFilterAVGu32_t* psFilterMA);

int32_t MCLIB_s32FilterAvg(
        int32_t s32Input,
        MCLIB_sFilterAVGs32_t* psFilterMA);


void MCLIB_u16FilterMAInit(
        uint16_t u16NSamples, 
        MCLIB_sFilterMAu16_t* psFilterMA);

void MCLIB_s16FilterMAInit(
        uint16_t u16NSamples, 
        MCLIB_sFilterMAs16_t* psFilterMA);

uint16_t MCLIB_u16FilterMA(
        uint16_t u16Input, 
        MCLIB_sFilterMAu16_t* psFilterMA);

int16_t MCLIB_s16FilterMA(
        int16_t s16Input, 
        MCLIB_sFilterMAs16_t* psFilterMA);


int16_t MCLIB_s16SQRT(int16_t s16Input);

uint16_t MCLIB_u16SQRT(uint16_t u16Input);

int16_t MCLIB_s16SQRT_s32(int32_t s32Input);

uint16_t MCLIB_u16SQRT_u32(uint32_t u32Input);


int16_t MCLIB_s16ScaleVoltageWithDCLink(
        int16_t s16Input, 
        int16_t s16VoltageDCLink, 
        int16_t s16ScaleIndex);

uint16_t MCLIB_u16ScaleVoltageWithDCLink(
        uint16_t u16Input,
        uint16_t u16VoltageDCLink,
        uint16_t u16ScaleIndex);

int16_t MCLIB_s16Integrator(
        int16_t s16Input, 
        MCLIB_sIntegrator_t* psIntegrator);

void MCLIB_vSinCos(int16_t s16Input, MCLIB_s2Param_s16_t* psSinCos);

uint16_t MCLIB_u16CountLeadingBits_s16 (int16_t s16Input);

uint16_t MCLIB_u16CountLeadingBits_u16 (uint16_t u16Input);

uint16_t MCLIB_u16CountLeadingBits_s32 (int32_t s32Input);

uint16_t MCLIB_u16CountLeadingBits_u32 (uint32_t u32Input);

int16_t MCLIB_s16ControllerPIpAW(
        int16_t s16Error, 
        MCLIB_sControllerPIpAW_t* psControllerPI);

int16_t MCLIB_s16ControllerPIrAW(
        int16_t s16Error, 
        MCLIB_sControllerPIrAW_t* psControllerPI);
        
uint16_t MCLIB_u16ControllerPIpAW(
        int16_t s16Error,
        MCLIB_sControllerPIpAWu16_t* psControllerPI);

uint16_t MCLIB_u16ControllerPIrAW(
        int16_t s16Error,
        MCLIB_sControllerPIrAWu16_t* psControllerPI);

int16_t MCLIB_PIM_s16SvmStd(MCLIB_s3Param_s16_t* psDutyCycle, MCLIB_s2Param_s16_t* psVoltageAlBeFiltered);

        
#endif	/* MCLIB_H */
