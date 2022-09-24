/* *****************************************************************************
 * File:   mclib.c
 * Author: Dimitar Lilov
 *
 * Created on 2018 11 27 14:12
 * 
 * 
 * Description: Math and Control Library 
 * 
 **************************************************************************** */


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "mclib.h"


/* *****************************************************************************
 * Configuration Definition
 **************************************************************************** */
#define USE_DIV_IN_U16_FILTER_AVG 0
#define USE_DIV_IN_U32_FILTER_AVG 0


/* *****************************************************************************
 * MCLIB_vLinearInterpolationMultiplierInit
 * 
 * Description: Initialize values of the multiplier and resolution increase 
 *      shift, based on the input and the output interval ranges.  
 *      uMultiplier=(u16OutputMax - u16OutputMin)/(u16InputMax - u16InputMin)
 **************************************************************************** */
void MCLIB_vLinearInterpolationMultiplierInit(
        MCLIB_sLinearInterpolation_t* psInterpolation)
{    
    uint32_t u32TempMultiplier;
    uint16_t u16TempShift = 0;
    
    u32TempMultiplier = (uint32_t)(
            ((uint64_t)(psInterpolation->u16OutputMax - 
                        psInterpolation->u16OutputMin) << 16) / 
            (psInterpolation->u16InputMax - psInterpolation->u16InputMin));
    
    while (u32TempMultiplier > UINT16_MAX)
    {
        u32TempMultiplier >>= 1;
        u16TempShift++;
    }
    psInterpolation->uMultiplier.sCoeff.u16Mtply = (uint16_t)u32TempMultiplier;
    psInterpolation->uMultiplier.sCoeff.u16Shift = 16 - u16TempShift;
}


/* *****************************************************************************
 * MCLIB_u16LinearInterpolation
 * 
 * Description: Perform Linear Interpolation.  
 **************************************************************************** */
uint16_t MCLIB_u16LinearInterpolation(
        uint16_t u16Input, 
        MCLIB_sLinearInterpolation_t* psInterpolation)
{
    uint16_t u16Result;
    
    if (u16Input >= psInterpolation->u16InputMax)
    {
        u16Result = psInterpolation->u16OutputMax;
    }
    else
    if (u16Input <= psInterpolation->u16InputMin)
    {
        u16Result = psInterpolation->u16OutputMin;
    }
    else
    {
        u16Result = psInterpolation->u16OutputMin + (uint16_t)(
                MCLIB_u32Mul_u16u16((u16Input - psInterpolation->u16InputMin),
                            psInterpolation->uMultiplier.sCoeff.u16Mtply)>>
                (psInterpolation->uMultiplier.sCoeff.u16Shift));
    }
       
    return u16Result;
}
/* *****************************************************************************
 * MCLIB_vLinearInterpolationSignedMultiplierInit
 * 
 * Description: Initialize values of the multiplier and resolution increase 
 *      shift, based on the input and the output interval ranges.  
 *      uMultiplier=(s16OutputMax - s16OutputMin)/(s16InputMax - s16InputMin)
**************************************************************************** */
void MCLIB_vLinearInterpolationSignedMultiplierInit(
        MCLIB_sLinearInterpolationSigned_t* psInterpolation)
{    
    int32_t s32TempMultiplier;
    int16_t s16TempShift = 0;
    
    s32TempMultiplier = (int32_t)(
            ((int64_t)(psInterpolation->s16OutputMax - 
                    psInterpolation->s16OutputMin) << 16)/
            (psInterpolation->s16InputMax - psInterpolation->s16InputMin));
    
    while ((s32TempMultiplier > INT16_MAX) || (s32TempMultiplier < INT16_MIN))
    {
        s32TempMultiplier >>= 1;
        s16TempShift++;
    }
    psInterpolation->uMultiplier.sCoeffSigned.s16Mtply = 
            (int16_t)s32TempMultiplier;
    psInterpolation->uMultiplier.sCoeffSigned.s16Shift = 
            16 - s16TempShift;
}

/* *****************************************************************************
 * MCLIB_s16LinearInterpolationSigned
 * 
 * Description: Perform Linear Interpolation.  
 **************************************************************************** */
int16_t MCLIB_s16LinearInterpolationSigned(
        int16_t s16Input, 
        MCLIB_sLinearInterpolationSigned_t* psInterpolation)
{
    int16_t s16Result;
    
    if (s16Input >= psInterpolation->s16InputMax)
    {
        s16Result = psInterpolation->s16OutputMax;
    }
    else
    if (s16Input <= psInterpolation->s16InputMin)
    {
        s16Result = psInterpolation->s16OutputMin;
    }
    else
    {
        s16Result = psInterpolation->s16OutputMin + (int16_t)(
                MCLIB_s32Mul_s16s16((s16Input - psInterpolation->s16InputMin),
                        psInterpolation->uMultiplier.sCoeffSigned.s16Mtply)>>
                (psInterpolation->uMultiplier.sCoeffSigned.s16Shift));
    }

    return s16Result;
}



/* Polynom Debug */
#if MCLIB_DEBUG_POLYNOM == 1
int32_t a;
int32_t a0;
int32_t a1;
int32_t a2;
uint16_t s;
uint16_t sI1;
uint16_t sI2;
uint16_t sD1;
uint16_t sD2;

uint16_t sC1;
uint16_t sC2;
#endif
/* End Polynom Debug */


/* *****************************************************************************
 * MCLIB_s32PolynomSecondOrder_s32  - first implementation of polynom in FAN project
 * 
 * s32X         fractional Q15.16[-32768..32767.9999]
 * 
 * s32Return    fractional Q15.16[-32768..32767.9999]
 *              high 16 bits whole part int16_t, low 16 bits fractional part
 * 
 * u16C2Shift   increased resolution of coefficient C2 to fit best in 16 bits
 * u16C1Shift   increased resolution of coefficient C1 to fit best in 16 bits
 * 
 * Description: Calculate second order polynom: Y = X*X*C2 + X*C1 + C0
 **************************************************************************** */
int32_t MCLIB_s32PolynomSecondOrder_s32(
        int32_t s32X,
        int16_t s16C2,
        int16_t s16C2Shift,
        int16_t s16C1,
        int16_t s16C1Shift,
        int32_t s32C0)
{
    uint16_t shiftDecreaseResolution = 0;
    uint16_t shiftDecreaseResolution1 = 0;
    uint16_t shiftDecreaseResolution2 = 0;
     int32_t c;
     int32_t c0;
     int32_t c1;
     int32_t c2;
    
    uint16_t u16C1Shift = 0;
    uint16_t u16C2Shift = 0;
    
    if (s16C1Shift > 0) u16C1Shift = s16C1Shift;
    if (s16C2Shift > 0) u16C2Shift = s16C2Shift;
    
    s32X >>= 1; /* prepare switch from Q15.16 to Q0.15 */
    
    /* Increase Resolution - consider future optimization */
    if (s32X > 0)
    {
        while(s32X > INT16_MAX)
        {
            shiftDecreaseResolution++;
            s32X >>= 1;
        }
    }
    else
    if (s32X < 0)
    {
        while(s32X < INT16_MIN)
        {
            shiftDecreaseResolution++;
            s32X >>= 1;
        }
    }
#if 0 
    while ((u16C1Shift > 0) 
        && (u16C2Shift > 0) 
        && (shiftDecreaseResolution > 0))
    {
        shiftDecreaseResolution--;
        u16C1Shift--;
        u16C2Shift--;
    }
#endif 
    
#if MCLIB_DEBUG_POLYNOM == 1
    sC1 = u16C1Shift;
    sC2 = u16C2Shift;
#endif
     
    shiftDecreaseResolution1 = shiftDecreaseResolution; 
    if (s16C1Shift < 0) shiftDecreaseResolution1 += (0-s16C1Shift);
    
    if (shiftDecreaseResolution1 > u16C1Shift) 
    {
        shiftDecreaseResolution1 -= u16C1Shift;
        u16C1Shift = 0;
    }
    else
    if (shiftDecreaseResolution1 < u16C1Shift) 
    {
        u16C1Shift -= shiftDecreaseResolution1;
        shiftDecreaseResolution1 = 0;
    }
    
    
    shiftDecreaseResolution2 = shiftDecreaseResolution << 1; 
    if (s16C2Shift < 0) shiftDecreaseResolution2 += (0-s16C2Shift);
    if (shiftDecreaseResolution2 > u16C2Shift) 
    {
        shiftDecreaseResolution2 -= u16C2Shift;
        u16C2Shift = 0;
    }
    else
    if (shiftDecreaseResolution2 < u16C2Shift) 
    {
        u16C2Shift -= shiftDecreaseResolution2;
        shiftDecreaseResolution2 = 0;
    }
    
    
    /*(((((s16C2*(int32_t)s32X)>>15)*(int32_t)s32X)>>u16C2Shift) + 
         ((s16C1*(int32_t)s32X)>>u16C1Shift) + s32C0) */
    c0 = s32C0;
    c1 = (((s16C1*s32X)>>u16C1Shift)<<shiftDecreaseResolution1);
    c2 = (((((s16C2*s32X)>>15)*s32X)>>u16C2Shift)<<shiftDecreaseResolution2);
    
#if MCLIB_DEBUG_POLYNOM == 1
    a0 = c0;
    a1 = c1;
    a2 = c2;
#endif
    
    c = c0 + c1 + c2;
    
#if MCLIB_DEBUG_POLYNOM == 1
    s = shiftDecreaseResolution;
    a = c;
    
    sI1 = u16C1Shift;
    sI2 = u16C2Shift;
    sD1 = shiftDecreaseResolution1;
    sD2 = shiftDecreaseResolution2;
#endif
    /* c >>= shiftDecreaseResolution; */
    return c;
}


/* *****************************************************************************
 * MCLIB_s32PolynomSecondOrderFast_s32
 *
 * s32X         fractional Q15.16[-32768..32767.9999]
 *
 * s32Return    fractional Q15.16[-32768..32767.9999]
 *              high 16 bits whole part int16_t, low 16 bits fractional part
 *
 * Description: Calculate second order polynom: Y = X*X*C2 + X*C1 + C0
 **************************************************************************** */
int32_t MCLIB_s32PolynomSecondOrderFast_s32(
        int32_t s32X,
        MCLIB_sPolynomSecondOrderFast_t* psPolynom)
{
    uint16_t shiftDecreaseResolution = 0;
    uint16_t shiftDecreaseResolution1 = 0;
    uint16_t shiftDecreaseResolution2 = 0;
     int32_t c;
     int32_t c0;
     int32_t c1;
     int32_t c2;

    uint16_t u16C1Shift = 0;
    uint16_t u16C2Shift = 0;

    if (psPolynom->s16C1Shift > 0) u16C1Shift = psPolynom->s16C1Shift;
    if (psPolynom->s16C2Shift > 0) u16C2Shift = psPolynom->s16C2Shift;

    s32X >>= 1; /* prepare switch from Q15.16 to Q0.15 */

    /* Increase Resolution - consider future optimization */
    if (s32X > 0)
    {
        while(s32X > INT16_MAX)
        {
            shiftDecreaseResolution++;
            s32X >>= 1;
        }
    }
    else
    if (s32X < 0)
    {
        while(s32X < INT16_MIN)
        {
            shiftDecreaseResolution++;
            s32X >>= 1;
        }
    }
#if 0
    while ((u16C1Shift > 0)
        && (u16C2Shift > 0)
        && (shiftDecreaseResolution > 0))
    {
        shiftDecreaseResolution--;
        u16C1Shift--;
        u16C2Shift--;
    }
#endif

#if MCLIB_DEBUG_POLYNOM == 1
    sC1 = u16C1Shift;
    sC2 = u16C2Shift;
#endif

    shiftDecreaseResolution1 = shiftDecreaseResolution;
    if (psPolynom->s16C1Shift < 0) shiftDecreaseResolution1 += (0-psPolynom->s16C1Shift);

    if (shiftDecreaseResolution1 > u16C1Shift)
    {
        shiftDecreaseResolution1 -= u16C1Shift;
        u16C1Shift = 0;
    }
    else
    if (shiftDecreaseResolution1 < u16C1Shift)
    {
        u16C1Shift -= shiftDecreaseResolution1;
        shiftDecreaseResolution1 = 0;
    }


    shiftDecreaseResolution2 = shiftDecreaseResolution << 1;
    if (psPolynom->s16C2Shift < 0) shiftDecreaseResolution2 += (0-psPolynom->s16C2Shift);
    if (shiftDecreaseResolution2 > u16C2Shift)
    {
        shiftDecreaseResolution2 -= u16C2Shift;
        u16C2Shift = 0;
    }
    else
    if (shiftDecreaseResolution2 < u16C2Shift)
    {
        u16C2Shift -= shiftDecreaseResolution2;
        shiftDecreaseResolution2 = 0;
    }


    /*(((((s16C2*(int32_t)s32X)>>15)*(int32_t)s32X)>>u16C2Shift) +
         ((s16C1*(int32_t)s32X)>>u16C1Shift) + s32C0) */
    c0 = psPolynom->s32C0;
    c1 = (((psPolynom->s16C1*s32X)>>u16C1Shift)<<shiftDecreaseResolution1);
    c2 = (((((psPolynom->s16C2*s32X)>>15)*s32X)>>u16C2Shift)<<shiftDecreaseResolution2);

#if MCLIB_DEBUG_POLYNOM == 1
    a0 = c0;
    a1 = c1;
    a2 = c2;
#endif

    c = c0 + c1 + c2;

#if MCLIB_DEBUG_POLYNOM == 1
    s = shiftDecreaseResolution;
    a = c;

    sI1 = u16C1Shift;
    sI2 = u16C2Shift;
    sD1 = shiftDecreaseResolution1;
    sD2 = shiftDecreaseResolution2;
#endif
    /* c >>= shiftDecreaseResolution; */
    return c;
}


/* *****************************************************************************
 * MCLIB_u32RampLinear
 * 
 * Description: Linear Ramp Of unsigned 32 bit input to 32 bit output
 **************************************************************************** */
uint32_t MCLIB_u32RampLinear(uint32_t u32Input, MCLIB_sRamp_t* psRamp)
{
    uint32_t u32Output;
    
    u32Output = (psRamp)->u32State;
    if (u32Input != u32Output)
    {
        if (u32Input > u32Output)
        {
            u32Output = 
                    MCLIB_u32AddSat_u32u32(u32Output, 
                            (psRamp)->u32RampUp);
            if (u32Output > u32Input)
            {
                u32Output = u32Input;
            }	
        }
        else /* if (u32Input < u32Output) */ 
        {
            u32Output = 
                    MCLIB_u32SubSat_u32u32(u32Output, 
                            (psRamp)->u32RampDown);
            if (u32Output < u32Input)
            {
                u32Output = u32Input;
            }	
        }
        (psRamp)->u32State = u32Output;
    }
    return u32Output;
    
}



/* *****************************************************************************
 * MCLIB_u16FilterAvg
 * 
 * Description: Moving Average Filter of unsigned 16 bit input to 16 bit output
 *              u16NSamples means count samples to average
 **************************************************************************** */
void MCLIB_u16FilterAvgInit(
        uint16_t u16NSamples, 
        MCLIB_sFilterAVGu16_t* psFilterMA)
{
    uint32_t u32ChangeMultiplier;   /* Q16_16 */
    
    if ((psFilterMA)->u16NSamples != u16NSamples)
    {
        //(psFilterMA)->u16NSamples >>= (psFilterMA)->u16NShift;
        u32ChangeMultiplier = MCLIB_u32Div_u32u16( (uint32_t)(u16NSamples) << 16, (psFilterMA)->u16NSamples );
        (psFilterMA)->u32Acc = MCLIB_s32Mul_s32u32((psFilterMA)->u32Acc, u32ChangeMultiplier);
        (psFilterMA)->u16NSamples = u16NSamples;
        //(psFilterMA)->u16NShift = 0;
        (psFilterMA)->u32Mult = MCLIB_u32Div_u32u16( (uint32_t)UINT16_MAX+1, (psFilterMA)->u16NSamples );
    }
}

uint16_t MCLIB_u16FilterAvg(
        uint16_t u16Input, 
        MCLIB_sFilterAVGu16_t* psFilterMA)
{
    uint16_t u16Output;
    uint32_t u32Acc;

    u32Acc = (psFilterMA)->u32Acc;

    /* Decrease Resolution Not To Overflow */
    //if (MCLIB_u16AddCarry_u32u32((u16Input >> (psFilterMA)->u16NShift), u32Acc))
    //{
    //    (psFilterMA)->u16NShift = (psFilterMA)->u16NShift + 1;
    //    u32Acc >>= 1;
    //}
    //u16Input >>= (psFilterMA)->u16NShift;
    u32Acc = MCLIB_u32AddSat_u32u32(u16Input, u32Acc);

    #if USE_DIV_IN_U16_FILTER_AVG
    u16Output = MCLIB_u16Div_u32u16(u32Acc, (psFilterMA)->u16NSamples);
    #else
    u16Output = MCLIB_u16Mul_u32u32(u32Acc, (psFilterMA)->u32Mult);
    #endif


    u32Acc = MCLIB_u32SubSat_u32u32(u32Acc, (uint32_t)u16Output);
    //u16Output <<= (psFilterMA)->u16NShift;
    /* Increase Resolution if possible */
    //if ((psFilterMA)->u16NShift > 0)
    //{
    //    if (u32Acc < (UINT32_MAX / 2))
    //    {
    //        (psFilterMA)->u16NShift--;
    //        u32Acc <<= 1;
    //    }
    //}

    (psFilterMA)->u32Acc = u32Acc;

    return u16Output;
}

/* *****************************************************************************
 * MCLIB_s16FilterAvg
 * 
 * Description: Moving Average Filter of signed 16 bit input to 16 bit output
 *              u16NSamples means count samples to average
 **************************************************************************** */

void MCLIB_s16FilterAvgInit(
        uint16_t u16NSamples, 
        MCLIB_sFilterAVGs16_t* psFilterMA)
{
    uint32_t u32ChangeMultiplier;   /* Q16_16 */
    
    if ((psFilterMA)->u16NSamples != u16NSamples)
    {
        //(psFilterMA)->u16NSamples >>= (psFilterMA)->u16NShift;
        u32ChangeMultiplier = MCLIB_u32Div_u32u16( (uint32_t)(u16NSamples) << 16, (psFilterMA)->u16NSamples );
        (psFilterMA)->s32Acc = MCLIB_s32Mul_s32u32((psFilterMA)->s32Acc, u32ChangeMultiplier);
        (psFilterMA)->u16NSamples = u16NSamples;
        //(psFilterMA)->u16NShift = 0;
        (psFilterMA)->u32Mult = MCLIB_u32Div_u32u16( (uint32_t)UINT16_MAX+1, (psFilterMA)->u16NSamples );
    }
}

int16_t MCLIB_s16FilterAvg(
        int16_t s16Input, 
        MCLIB_sFilterAVGs16_t* psFilterMA)
{
    int16_t s16Output;
    int32_t s32Acc;
    
    s32Acc = (psFilterMA)->s32Acc;

    /* Decrease Resolution Not To Overflow */
    //if (MCLIB_u16AddCarry_s32s32((s16Input >> (psFilterMA)->u16NShift), s32Acc))
    //{
    //    (psFilterMA)->u16NShift++;
    //    s32Acc >>= 1;
    //}
    //s16Input >>= (psFilterMA)->u16NShift;
    s32Acc = MCLIB_s32AddSat_s32s32(s16Input, s32Acc);

    #if USE_DIV_IN_U16_FILTER_AVG
    s16Output = MCLIB_s16Div_s32u16(s32Acc, (psFilterMA)->u16NSamples);
    #else
    s16Output = MCLIB_s16Mul_s32u32(s32Acc, (psFilterMA)->u32Mult);
    #endif



    s32Acc = MCLIB_s32SubSat_s32s32(s32Acc, s16Output);

    //s16Output <<= (psFilterMA)->u16NShift;
    /* Increase Resolution if possible */
    //if ((psFilterMA)->u16NShift > 0)
    //{
    //    if ((s32Acc < (INT32_MAX / 2)) && (s32Acc > (INT32_MIN / 2)))
    //    {
    //        (psFilterMA)->u16NShift--;
    //        s32Acc <<= 1;
    //    }
    //}

    (psFilterMA)->s32Acc = s32Acc;


    return s16Output;
}



/* *****************************************************************************
 * MCLIB_s32FilterAvg
 *
 * Description: Moving Average Filter of signed 32 bit input to 32 bit output
 *              u16NSamples means count samples to average
 **************************************************************************** */

void MCLIB_s32FilterAvgInit(
        uint16_t u16NSamples,
        MCLIB_sFilterAVGs32_t* psFilterMA)
{
    uint32_t u32ChangeMultiplier;   /* Q16_16 */

    if ((psFilterMA)->u16NSamples != u16NSamples)
    {
        (psFilterMA)->u16NSamples >>= (psFilterMA)->u16NShift;
        u32ChangeMultiplier = MCLIB_u32Div_u32u16( (uint32_t)(u16NSamples) << 16, (psFilterMA)->u16NSamples );
        (psFilterMA)->s32Acc = MCLIB_s32Mul_s32u32((psFilterMA)->s32Acc, u32ChangeMultiplier);
        (psFilterMA)->u16NSamples = u16NSamples;
        (psFilterMA)->u16NShift = 0;
        (psFilterMA)->u32Mult = MCLIB_u32Div_u32u16( (uint32_t)UINT16_MAX+1, (psFilterMA)->u16NSamples );
    }
}

int32_t MCLIB_s32FilterAvg(
        int32_t s32Input,
        MCLIB_sFilterAVGs32_t* psFilterMA)
{
    int32_t s32Output;
    int32_t s32Acc;

    s32Acc = (psFilterMA)->s32Acc;

    /* Decrease Resolution Not To Overflow */
    if (MCLIB_u16AddCarry_s32s32((s32Input >> (psFilterMA)->u16NShift), s32Acc))
    {
        (psFilterMA)->u16NShift++;
        s32Acc >>= 1;
    }
    s32Input >>= (psFilterMA)->u16NShift;
    s32Acc = MCLIB_s32AddSat_s32s32(s32Input, s32Acc);

    #if USE_DIV_IN_U32_FILTER_AVG
    s32Output = MCLIB_s32Div_s32u16(s32Acc, (psFilterMA)->u16NSamples);
    #else
    s32Output = MCLIB_s32Mul_s32u32(s32Acc, (psFilterMA)->u32Mult);
    #endif

    s32Acc = MCLIB_s32SubSat_s32s32(s32Acc, s32Output);
    s32Output <<= (psFilterMA)->u16NShift;

    /* Increase Resolution if possible */
    if ((psFilterMA)->u16NShift > 0)
    {
        if ((s32Acc < (INT32_MAX / 2)) && (s32Acc > (INT32_MIN / 2)))
        {
            (psFilterMA)->u16NShift--;
            s32Acc <<= 1;
        }
    }

    (psFilterMA)->s32Acc = s32Acc;


    return s32Output;
}

/* *****************************************************************************
 * MCLIB_u32FilterAvg
 *
 * Description: Moving Average Filter of unsigned 32 bit input to 32 bit output
 *              u16NSamples means count samples to average
 **************************************************************************** */
void MCLIB_u32FilterAvgInit(
        uint16_t u16NSamples,
        MCLIB_sFilterAVGu32_t* psFilterMA)
{
    uint32_t u32ChangeMultiplier;   /* Q16_16 */

    if ((psFilterMA)->u16NSamples != u16NSamples)
    {
        (psFilterMA)->u16NSamples >>= (psFilterMA)->u16NShift;
        u32ChangeMultiplier = MCLIB_u32Div_u32u16( (uint32_t)(u16NSamples) << 16, (psFilterMA)->u16NSamples );
        (psFilterMA)->u32Acc = MCLIB_u32Mul_u32u32((psFilterMA)->u32Acc, u32ChangeMultiplier);
        (psFilterMA)->u16NSamples = u16NSamples;
        (psFilterMA)->u16NShift = 0;
        (psFilterMA)->u32Mult = MCLIB_u32Div_u32u16( (uint32_t)UINT16_MAX+1, (psFilterMA)->u16NSamples );
    }
}

uint32_t MCLIB_u32FilterAvg(
        uint32_t u32Input,
        MCLIB_sFilterAVGu32_t* psFilterMA)
{
    uint32_t u32Output;
    uint32_t u32Acc;

    u32Acc = (psFilterMA)->u32Acc;

    /* Decrease Resolution Not To Overflow */
    if (MCLIB_u16AddCarry_u32u32((u32Input >> (psFilterMA)->u16NShift), u32Acc))
    {
        (psFilterMA)->u16NShift = (psFilterMA)->u16NShift + 1;
        u32Acc >>= 1;
    }
    u32Input >>= (psFilterMA)->u16NShift;
    u32Acc = MCLIB_u32AddSat_u32u32(u32Input, u32Acc);

    #if USE_DIV_IN_U32_FILTER_AVG
    u32Output = MCLIB_u32Div_u32u16(u32Acc, (psFilterMA)->u16NSamples);
    #else
    u32Output = MCLIB_u32Mul_u32u32(u32Acc, (psFilterMA)->u32Mult);
    #endif

    u32Acc = MCLIB_u32SubSat_u32u32(u32Acc, u32Output);
    u32Output <<= (psFilterMA)->u16NShift;

    /* Increase Resolution if possible */
    if ((psFilterMA)->u16NShift > 0)
    {
        if (u32Acc < (UINT32_MAX / 2))
        {
            (psFilterMA)->u16NShift--;
            u32Acc <<= 1;
        }
    }

    (psFilterMA)->u32Acc = u32Acc;


    return u32Output;
}


/* *****************************************************************************
 * MCLIB_u16FilterMA
 * 
 * Description: Moving Average Filter of unsigned 16 bit input to 16 bit output
 **************************************************************************** */
void MCLIB_u16FilterMAInit(
        uint16_t u16NSamples, 
        MCLIB_sFilterMAu16_t* psFilterMA)
{
    uint16_t u16ShiftCount;
    
    if (u16NSamples >= (psFilterMA)->u16NSamples)
    {
        u16ShiftCount = u16NSamples - (psFilterMA)->u16NSamples;
        (psFilterMA)->u32Acc <<= u16ShiftCount;
    }
    else
    {
        u16ShiftCount = (psFilterMA)->u16NSamples - u16NSamples;
        (psFilterMA)->u32Acc >>= u16ShiftCount;
    }

    (psFilterMA)->u16NSamples = u16NSamples;
}

uint16_t MCLIB_u16FilterMA(
        uint16_t u16Input, 
        MCLIB_sFilterMAu16_t* psFilterMA)
{
    uint16_t u16Output;
    uint32_t u32Acc;
    
    u32Acc = MCLIB_u32AddSat_u32u32((uint32_t)u16Input,
                    (psFilterMA)->u32Acc); 
    
    u16Output = (uint16_t)(u32Acc >> (psFilterMA)->u16NSamples); 
    
    (psFilterMA)->u32Acc = 
            MCLIB_u32SubSat_u32u32(u32Acc, (uint32_t)u16Output);
    
    return u16Output;
}


/* *****************************************************************************
 * MCLIB_s16FilterMA
 * 
 * Description: Moving Average Filter of signed 16 bit input to 16 bit output
 **************************************************************************** */
void MCLIB_s16FilterMAInit(
        uint16_t u16NSamples, 
        MCLIB_sFilterMAs16_t* psFilterMA)
{
    uint16_t u16ShiftCount;
    
    if (u16NSamples >= (psFilterMA)->u16NSamples)
    {
        u16ShiftCount = u16NSamples - (psFilterMA)->u16NSamples;
        (psFilterMA)->s32Acc <<= u16ShiftCount;
    }
    else
    {
        u16ShiftCount = (psFilterMA)->u16NSamples - u16NSamples;
        (psFilterMA)->s32Acc >>= u16ShiftCount;
    }

    (psFilterMA)->u16NSamples = u16NSamples;
}

int16_t MCLIB_s16FilterMA(
        int16_t s16Input, 
        MCLIB_sFilterMAs16_t* psFilterMA)
{
    int16_t s16Output;
    int32_t s32Acc;
    
    s32Acc = MCLIB_s32AddSat_s32s32((int32_t)s16Input,
                    (psFilterMA)->s32Acc); 
    
    s16Output = (int16_t)(s32Acc >> (psFilterMA)->u16NSamples); 
    
    (psFilterMA)->s32Acc = 
            MCLIB_s32SubSat_s32s32(s32Acc, (int32_t)s16Output);
    
    return s16Output;
}


/* *****************************************************************************
 * MCLIB_s16SQRT
 * 
 * Description: signed ?fractional? 16 bit SQRT Calculation
 **************************************************************************** */
int16_t MCLIB_s16SQRT(int16_t s16Input)
{
    int16_t s16Output;
    uint32_t u32Output;
    uint32_t u32Input;
    
    s16Output = 0;
    if (s16Input > 0)
    {
        u32Input = (uint32_t)s16Input;
        u32Input <<= 15;
        if (u32Input > 268435456)
        {
            s16Output = 16384;
        }
        s16Output += 8192;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -8192;
        }
        s16Output += 4096;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -4096;
        }
        s16Output += 2048;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -2048;
        }
        s16Output += 1024;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -1024;
        }
        s16Output += 512;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -512;
        }
        s16Output += 256;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -256;
        }
        s16Output += 128;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -128;
        }
        s16Output += 64;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -64;
        }
        s16Output += 32;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -32;
        }
        s16Output += 16;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -16;
        }
        s16Output += 8;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -8;
        }
        s16Output += 4;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -4;
        }
        s16Output += 2;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -2;
        }
        s16Output += 1;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -1;
        }
    }
    return s16Output;
}


/* *****************************************************************************
 * MCLIB_s16SQRT_s32
 *
 * Description: signed ?fractional? 16 bit SQRT Calculation
 **************************************************************************** */
int16_t MCLIB_s16SQRT_s32(int32_t s32Input)
{
    int16_t s16Output;
    uint32_t u32Output;
    uint32_t u32Input;

    s16Output = 0;
    if (s32Input > 0)
    {
        
        /* check why works !!! */
//#ifdef __XC16
        u32Input = (uint32_t)(s32Input >> 0);
//#else
//        u32Input = (uint32_t)(s32Input >> 1);
//#endif
        if (u32Input > 268435456)
        {
            s16Output = 16384;
        }
        s16Output += 8192;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -8192;
        }
        s16Output += 4096;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -4096;
        }
        s16Output += 2048;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -2048;
        }
        s16Output += 1024;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -1024;
        }
        s16Output += 512;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -512;
        }
        s16Output += 256;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -256;
        }
        s16Output += 128;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -128;
        }
        s16Output += 64;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -64;
        }
        s16Output += 32;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -32;
        }
        s16Output += 16;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -16;
        }
        s16Output += 8;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -8;
        }
        s16Output += 4;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -4;
        }
        s16Output += 2;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -2;
        }
        s16Output += 1;

        u32Output = (uint32_t)s16Output;
        u32Output *= (uint32_t)s16Output;
        if (u32Output > u32Input)
        {
            s16Output += -1;
        }
    }
    return s16Output;
}


/* *****************************************************************************
 * MCLIB_u16SQRT
 *
 * Description: signed ?fractional? 16 bit SQRT Calculation
 **************************************************************************** */
uint16_t MCLIB_u16SQRT(uint16_t u16Input)
{
    uint16_t u16Output;
    uint32_t u32Output;
    uint32_t u32Input;

    u16Output = 0;
    if (u16Input > 0)
    {
        u32Input = (uint32_t)u16Input;
        u32Input <<= 15;
        if (u32Input > 268435456)
        {
            u16Output = 16384;
        }
        u16Output = (uint16_t)(u16Output+8192);

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output = (uint16_t)(u16Output-8192);
        }
        u16Output = (uint16_t)(u16Output+4096);

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output  = (uint16_t)(u16Output-4096);
        }
        u16Output = (uint16_t)(u16Output+2048);

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output = (uint16_t)(u16Output-2048);
        }
        u16Output = (uint16_t)(u16Output+1024);

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output = (uint16_t)(u16Output-1024);
        }
        u16Output += 512;

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output = (uint16_t)(u16Output-512);
        }
        u16Output += 256;

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output = (uint16_t)(u16Output-256);
        }
        u16Output += 128;

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output = (uint16_t)(u16Output-128);
        }
        u16Output += 64;

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output = (uint16_t)(u16Output-64);
        }
        u16Output += 32;

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output = (uint16_t)(u16Output-32);
        }
        u16Output += 16;

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output = (uint16_t)(u16Output-16);
        }
        u16Output += 8;

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output = (uint16_t)(u16Output-8);
        }
        u16Output += 4;

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output = (uint16_t)(u16Output-4);
        }
        u16Output += 2;

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output = (uint16_t)(u16Output-2);
        }
        u16Output += 1;

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output = (uint16_t)(u16Output-1);
        }
    }
    return u16Output;
}

/* *****************************************************************************
 * MCLIB_u16SQRT_u32
 *
 * Description: signed ?fractional? 16 bit SQRT Calculation
 **************************************************************************** */
uint16_t MCLIB_u16SQRT_u32(uint32_t u32Input)
{
    uint16_t u16Output;
    uint32_t u32Output;
    //uint32_t u32Input;

    u16Output = 0;
    if (u32Input > 0)
    {
        u32Input = (u32Input >> 0);
        if (u32Input > 268435456)
        {
            u16Output = 16384;
        }
        u16Output = (uint16_t)(u16Output+8192);

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output = (uint16_t)(u16Output-8192);
        }
        u16Output = (uint16_t)(u16Output+4096);

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output  = (uint16_t)(u16Output-4096);
        }
        u16Output = (uint16_t)(u16Output+2048);

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output = (uint16_t)(u16Output-2048);
        }
        u16Output = (uint16_t)(u16Output+1024);

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output = (uint16_t)(u16Output-1024);
        }
        u16Output += 512;

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output = (uint16_t)(u16Output-512);
        }
        u16Output += 256;

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output = (uint16_t)(u16Output-256);
        }
        u16Output += 128;

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output = (uint16_t)(u16Output-128);
        }
        u16Output += 64;

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output = (uint16_t)(u16Output-64);
        }
        u16Output += 32;

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output = (uint16_t)(u16Output-32);
        }
        u16Output += 16;

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output = (uint16_t)(u16Output-16);
        }
        u16Output += 8;

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output = (uint16_t)(u16Output-8);
        }
        u16Output += 4;

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output = (uint16_t)(u16Output-4);
        }
        u16Output += 2;

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output = (uint16_t)(u16Output-2);
        }
        u16Output += 1;

        u32Output = (uint32_t)u16Output;
        u32Output *= (uint32_t)u16Output;
        if (u32Output > u32Input)
        {
            u16Output = (uint16_t)(u16Output-1);
        }
    }
    return u16Output;
}


/* *****************************************************************************
 * MCLIB_s16ScaleVoltageWithDCLink
 * 
 * Description: signed 16 bit Scale Voltage with the DC-Link Voltage
 **************************************************************************** */
int16_t MCLIB_s16ScaleVoltageWithDCLink(
        int16_t s16Input, 
        int16_t s16VoltageDCLink, 
        int16_t s16ScaleIndex)
{
    int16_t s16Output;
    int16_t s16Voltage;
    int16_t s16Mul;
    int32_t s32Mul;
    
    s16Voltage = (int16_t)(s16VoltageDCLink >> 1);

    if (s16Voltage == 0)
    {
        s16Output = 0;
    }
    else
    {
        s16Mul = MCLIB_s16Mul_s16s16(s16ScaleIndex, s16Input);
        if (MCLIB_s16AbsSat(s16Mul) > s16Voltage)
        {
            if (s16Input < 0)
            {
                s16Output = INT16_MIN;
            }
            else
            {
                s16Output = INT16_MAX;
            }
        }
        else
        {
            if (s16Voltage == 0)
            {
                if (s16Mul > 0)
                {
                    s16Output = INT16_MAX;
                }
                else
                {
                    s16Output = INT16_MIN;
                }
            }
            else
            {
                if (s16Input >= MCLIB_s16AbsSat(s16Voltage))
                {
                    s16Mul ^= s16Voltage;
                    s16Mul &= INT16_MIN;
                    if (s16Mul < 0)
                    {
                        s16Output = INT16_MIN;
                    }
                    else
                    {
                        s16Output = INT16_MAX;
                    }
                }
                else
                {
                    if (s16Voltage == 0)
                    {
                        if (s16Mul > 0)
                        {
                            s16Output = INT16_MAX;
                        }
                        else
                        {
                            s16Output = INT16_MIN;
                        }
                    }
                    else
                    {
                        s32Mul = MCLIB_s32ShL((int32_t)s16Mul, 15);
                        s16Output = (int16_t)(s32Mul / s16Voltage);
                    }
                }
            }
        }
    }
    return s16Output;
}


/* *****************************************************************************
 * MCLIB_u16ScaleVoltageWithDCLink
 *
 * Description: unsigned 16 bit Scale Voltage with the DC-Link Voltage
 **************************************************************************** */
uint16_t MCLIB_u16ScaleVoltageWithDCLink(
        uint16_t u16Input,
        uint16_t u16VoltageDCLink,
        uint16_t u16ScaleIndex)
{
    uint16_t u16Output;
    uint16_t u16Voltage;
    uint16_t u16Mul;
    uint32_t u32Mul;

    u16Voltage = (u16VoltageDCLink >> 0);
    
    if (u16Voltage == 0)
    {
        u16Output = 0;
    }
    else
    {
        u16Mul = MCLIB_u16Mul_u16u16(u16ScaleIndex, u16Input);
        if (u16Mul > u16Voltage)
        {
            //if (u16Input < 0)
            {
            //    u16Output = UINT16_MIN;
            }
            //else
            {
                u16Output = UINT16_MAX;
            }
        }
        else
        {
            //if (u16Voltage == 0) - not needed - already checked above
            {
                //if (u16Mul > 0)
                {
                //    u16Output = UINT16_MAX;
                }
                //else
                {
                //    u16Output = UINT16_MIN;
                }
            }
            //else
            {
                if (u16Input >= u16Voltage)
                {
                    //u16Mul ^= u16Voltage;
                    //u16Mul &= INT16_MIN;
                    //if (u16Mul < 0)
                    {
                    //    u16Output = INT16_MIN;
                    }
                    //else
                    {
                        u16Output = INT16_MAX;
                    }
                }
                else
                {
                    //if (u16Voltage == 0)
                    //{
                    //    if (u16Mul > 0)
                    //    {
                    //        u16Output = INT16_MAX;
                    //    }
                    //    else
                    //    {
                    //        u16Output = INT16_MIN;
                    //    }
                    //}
                    //else
                    //{
                        u32Mul = MCLIB_u32ShL((uint32_t)u16Mul, 16);
                        u16Output = (uint16_t)(u32Mul / u16Voltage);
                    //}
                }
            }
        }
    }
    return u16Output;
}


/* *****************************************************************************
 * MCLIB_s16Integrator
 * 
 * Description: signed 16 bit Integrator
 **************************************************************************** */
int16_t MCLIB_s16Integrator(
        int16_t s16Input, 
        MCLIB_sIntegrator_t* psIntegrator)
{
    (psIntegrator)->s32State += 
            MCLIB_s32Mul_s16s16(s16Input,(psIntegrator)->s16C1)+ 
                MCLIB_s32Mul_s16s16(
                    (psIntegrator)->s16PrevInput,
                    (psIntegrator)->s16C1);  
    (psIntegrator)->s16PrevInput = s16Input;
    /* roundup (+32768 -> +0.5 ) and down for the negative (-32768 -> -0.5 ) */
    
// 2019 09 09 Try fix position integrator with shift - old code commented   
//    return MCLIB_s16ShLSat(
//            (int16_t)((int32_t)(MCLIB_s32AddSat_s32s32(
//                                (psIntegrator)->s32State, 
//                                ((psIntegrator)->s32State & 32768L))) >> 16), 
//            (psIntegrator)->u16NShift);
    return  
            (int16_t)((int32_t)(MCLIB_s32AddSat_s32s32(
                                (psIntegrator)->s32State, 
                                ((psIntegrator)->s32State & 32768L))) >> (16 - (psIntegrator)->u16NShift));
}








typedef struct
{
    int16_t as16SinTable[4];
} MCLIB_sSinCalcCoef_t;

const MCLIB_sSinCalcCoef_t MCLIB_sSinCalcCoef = 
{
	{
		0xFF72,
		0x0A2C,
		0xAD53,
		0x4910,
	} 
};



/* *****************************************************************************
 * MCLIB_vSinCos
 * 
 * Description: Sine & Cosine Calculation From Angle [-pi..+pi]
 **************************************************************************** */
void MCLIB_vSinCos(int16_t s16Input, MCLIB_s2Param_s16_t* psSinCos)
{
    const int16_t* psSinCalcCoef = (int16_t*)&MCLIB_sSinCalcCoef;
    
    int16_t s16Input1;
    int16_t s16Input2;
    int16_t s16Input3;
    
    s16Input3 = MCLIB_s16NegSat(MCLIB_s16AbsSat(MCLIB_s16ShL(s16Input, 1)));
    s16Input1 = MCLIB_s16Mul_s16s16(s16Input3, s16Input3);	
    s16Input2 = MCLIB_s16Mul_s16s16(*(psSinCalcCoef+0), s16Input1); 
    s16Input2 = MCLIB_s16Add_s16s16(*(psSinCalcCoef+1), s16Input2); 
    s16Input2 = MCLIB_s16Mul_s16s16(s16Input2, s16Input1);
    s16Input2 = MCLIB_s16Add_s16s16(*(psSinCalcCoef+2), s16Input2); 
    s16Input2 = MCLIB_s16Mul_s16s16(s16Input2, s16Input1); 
    s16Input2 = MCLIB_s16Add_s16s16(*(psSinCalcCoef+3), s16Input2); 
    s16Input2 = MCLIB_s16Mul_s16s16(s16Input2, s16Input3);
    s16Input2 = MCLIB_s16AddSat_s16s16(s16Input2, s16Input3); 
    if (s16Input > 0)
    {
    	s16Input2 = MCLIB_s16NegSat(s16Input2);
    }
    (psSinCos)->s16Arg1 = s16Input2;  
    
    
    s16Input += 0x4000;
    
    s16Input3 = MCLIB_s16NegSat(MCLIB_s16AbsSat(MCLIB_s16ShL(s16Input, 1)));
    s16Input1 = MCLIB_s16Mul_s16s16(s16Input3, s16Input3);	
    s16Input2 = MCLIB_s16Mul_s16s16(*(psSinCalcCoef+0), s16Input1); 
    s16Input2 = MCLIB_s16Add_s16s16(*(psSinCalcCoef+1), s16Input2); 
    s16Input2 = MCLIB_s16Mul_s16s16(s16Input2, s16Input1);
    s16Input2 = MCLIB_s16Add_s16s16(*(psSinCalcCoef+2), s16Input2); 
    s16Input2 = MCLIB_s16Mul_s16s16(s16Input2, s16Input1); 
    s16Input2 = MCLIB_s16Add_s16s16(*(psSinCalcCoef+3), s16Input2); 
    s16Input2 = MCLIB_s16Mul_s16s16(s16Input2, s16Input3);
    s16Input2 = MCLIB_s16AddSat_s16s16(s16Input2, s16Input3); 
    if (s16Input > 0)
    {
    	s16Input2 = MCLIB_s16NegSat(s16Input2);
    }
    (psSinCos)->s16Arg2 = s16Input2; 
}


/* *****************************************************************************
 * MCLIB_u16CountLeadingBits_s16
 * 
 * Description: calculate count leading (sign) bits. On input zero returns 16
 **************************************************************************** */
uint16_t MCLIB_u16CountLeadingBits_s16 (int16_t s16Input)
{
    uint16_t u16Return = 0;
    if(s16Input < 0)
    {
        while (s16Input < 0) 
        {
            u16Return++;                                   
            s16Input = (int16_t)(((uint16_t)s16Input) << 1);
        } 
        u16Return--;    
    }
    else if (s16Input > 0)
    {
        while (s16Input > 0) 
        {
            u16Return++;                                   
            s16Input = (int16_t)(((uint16_t)s16Input) << 1);
        } 
        u16Return--;
    }
    else
    {
        u16Return = 16;
    }
    return(u16Return);
}


/* *****************************************************************************
 * MCLIB_u16CountLeadingBits_u16
 *
 * Description: calculate count leading (sign) bits. On input zero returns 16
 **************************************************************************** */
uint16_t MCLIB_u16CountLeadingBits_u16 (uint16_t u16Input)
{
    uint16_t u16Return = 0;
    if (u16Input > 0)
    {
        while (u16Input < (uint16_t)INT16_MIN)    /* 32768 */
        {
            u16Return++;
            u16Input = u16Input << 1;
        }
    }
    else
    {
        u16Return = 16;
    }
    return(u16Return);
}


/* *****************************************************************************
 * MCLIB_u16CountLeadingBits_s32
 * 
 * Description: calculate count leading (sign) bits. On input zero returns 32
 **************************************************************************** */
uint16_t MCLIB_u16CountLeadingBits_s32 (int32_t s32Input)
{
    uint16_t u16Return = 0;
    if(s32Input < 0)
    {
        while (s32Input < 0) 
        {
            u16Return++;                                   
            s32Input = (int32_t)(((uint32_t)s32Input) << 1);
        } 
        u16Return--;    
    }
    else if (s32Input > 0)
    {
        while (s32Input > 0) 
        {
            u16Return++;                                   
            s32Input = (int32_t)(((uint32_t)s32Input) << 1);
        } 
        u16Return--;
    }
    else
    {
        u16Return = 32;
    }
    return(u16Return);
}


/* *****************************************************************************
 * MCLIB_u16CountLeadingBits_u32
 *
 * Description: calculate count leading (sign) bits. On input zero returns 32
 **************************************************************************** */
uint16_t MCLIB_u16CountLeadingBits_u32 (uint32_t u32Input)
{
    uint16_t u16Return = 0;
    if (u32Input > 0)
    {
        while (u32Input < (uint32_t)INT32_MIN)
        {
            u16Return++;
            u32Input = u32Input << 1;
        }
        u16Return--;
    }
    else
    {
        u16Return = 32;
    }
    return(u16Return);
}


/* *****************************************************************************
 * MCLIB_s16ControllerPIpAW
 * 
 * Description: parallel PI controller with integral anti windup
 **************************************************************************** */
int16_t MCLIB_s16ControllerPIpAW(int16_t s16Error, MCLIB_sControllerPIpAW_t* psControllerPI)
{
    int32_t s32State;
    int32_t s32StateProp;
    int32_t s32Limit;
    int16_t s16Output;
    int16_t s16Limit;
    
    s32State = MCLIB_s32AddSat_s32s32(
            MCLIB_s32Mul_s16s16(
                (psControllerPI)->s16IntegGain, 
                s16Error), 
            MCLIB_s32Mul_s16s16(
                (psControllerPI)->s16IntegGain, 
                (psControllerPI)->s16PrevError));
    
    (psControllerPI)->s16PrevError = s16Error;
    s16Limit = (psControllerPI)->s16IntegGainShift;
    if (s16Limit >= 0)
    {
    	s32State = MCLIB_s32ShLSat(s32State, s16Limit);
    }
    else
    {	
    	s32State >>= (-s16Limit);
    }
    s32State = MCLIB_s32AddSat_s32s32(
                    s32State, 
                    (psControllerPI)->s32PrevIntegPart); 
    
    s32Limit = (int32_t)(psControllerPI)->s16Max << 16;
    if (s32State > s32Limit)
    {
    	s32State = s32Limit;
    }
    s32Limit = (int32_t)(psControllerPI)->s16Min << 16;
    if (s32State < s32Limit)
    {
    	s32State = s32Limit;
    }
    (psControllerPI)->s32PrevIntegPart = s32State;
    
    s32StateProp = MCLIB_s32Mul_s16s16((psControllerPI)->s16PropGain, s16Error);
    
    s16Limit = (psControllerPI)->s16PropGainShift;
    if (s16Limit >= 0)
    {
    	s32StateProp = MCLIB_s32ShLSat(s32StateProp, s16Limit);
    }
    else
    {	
    	s32StateProp >>= (-s16Limit);
    }
    s32State = MCLIB_s32AddSat_s32s32(s32State, s32StateProp); 
    
    /* roundup (+32768 -> +0.5 ) & round down negative (-32768 -> -0.5) */
    s16Output = (int16_t)((int32_t)(MCLIB_s32AddSat_s32s32(s32State, (s32State & 32768))) >> 16);
    
    (psControllerPI)->u16LimitFlag = 0;
    
    s16Limit = (psControllerPI)->s16Max;
    if (s16Output > s16Limit)
    {
    	s16Output = s16Limit;
    	(psControllerPI)->u16LimitFlag = 1;
    }
    s16Limit = (psControllerPI)->s16Min;
    if (s16Output < s16Limit)
    {
    	s16Output = s16Limit;
    	(psControllerPI)->u16LimitFlag = 1;
    }
    return (s16Output);   
}



/* *****************************************************************************
 * MCLIB_s16ControllerPIrAW
 * 
 * Description: recurrent form PI controller with integral anti windup
 **************************************************************************** */

int16_t MCLIB_s16ControllerPIrAW(int16_t s16Error, MCLIB_sControllerPIrAW_t* psControllerPI)
{
    uint16_t u16Shift = (16 - (psControllerPI)->u16NShift); 
    int32_t s32Acc = MCLIB_s32AddSat_s32s32(
            MCLIB_s32AddSat_s32s32(
                (psControllerPI)->s32Acc, 
                MCLIB_s32Mul_s16s16((psControllerPI)->s16PrevError, (psControllerPI)->s16CC2)), 
            MCLIB_s32Mul_s16s16(s16Error, (psControllerPI)->s16CC1));
    int32_t s32Limit;
    s32Limit = ((uint32_t)(psControllerPI)->s16Max << u16Shift);
    if (s32Acc > s32Limit)
    {
    	s32Acc = s32Limit;
    }
    s32Limit = ((uint32_t)(psControllerPI)->s16Min << u16Shift);
    if (s32Acc < s32Limit)
    {
    	s32Acc = s32Limit;
    } 
    (psControllerPI)->s32Acc = s32Acc;
    (psControllerPI)->s16PrevError = s16Error;
    /* roundup (+32768 -> +0.5 ), but round down for the negative (-32768 -> -0.5 ) -> */
    return ((int16_t)((uint32_t)(MCLIB_s32AddSat_s32s32(s32Acc, (s32Acc & 32768))) >> u16Shift));
    
}

/* *****************************************************************************
 * MCLIB_u16ControllerPIpAW - unsigned accumulator
 *
 * Description: parallel PI controller with integral anti windup
 **************************************************************************** */
uint16_t MCLIB_u16ControllerPIpAW(int16_t s16Error, MCLIB_sControllerPIpAWu16_t* psControllerPI)
{
    int32_t s32State;
    uint32_t u32State;
    int32_t s32StateProp;
    int32_t u32Limit;
    uint16_t u16Output;
    int16_t s16Limit;
    uint16_t u16Limit;

    s32State = MCLIB_s32AddSat_s32s32(
            MCLIB_s32Mul_s16s16(
                (psControllerPI)->s16IntegGain,
                s16Error),
            MCLIB_s32Mul_s16s16(
                (psControllerPI)->s16IntegGain,
                (psControllerPI)->s16PrevError));

    (psControllerPI)->s16PrevError = s16Error;
    s16Limit = (psControllerPI)->s16IntegGainShift;
    if (s16Limit >= 0)
    {
        s32State = MCLIB_s32ShLSat(s32State, s16Limit);
    }
    else
    {
        s32State >>= (-s16Limit);
    }
    u32State = MCLIB_u32AddSat_u32s32(
                    (psControllerPI)->u32PrevIntegPart,
                    s32State);

    u32Limit = (uint32_t)(psControllerPI)->u16Max << 16;
    if (u32State > u32Limit)
    {
        u32State = u32Limit;
    }
    u32Limit = (uint32_t)(psControllerPI)->u16Min << 16;
    if (u32State < u32Limit)
    {
        u32State = u32Limit;
    }
    (psControllerPI)->u32PrevIntegPart = u32State;

    s32StateProp = MCLIB_s32Mul_s16s16((psControllerPI)->s16PropGain, s16Error);

    s16Limit = (psControllerPI)->s16PropGainShift;
    if (s16Limit >= 0)
    {
        s32StateProp = MCLIB_s32ShLSat(s32StateProp, s16Limit);
    }
    else
    {
        s32StateProp >>= (-s16Limit);
    }
    u32State = MCLIB_u32AddSat_u32s32(u32State, s32StateProp);

    /* roundup (+32768 -> +0.5 ) & round down negative (-32768 -> -0.5) */
    u16Output = (uint16_t)((uint32_t)(MCLIB_u32AddSat_u32s32(u32State, (u32State & 32768))) >> 16);

    (psControllerPI)->u16LimitFlag = 0;

    u16Limit = (psControllerPI)->u16Max;
    if (u16Output > u16Limit)
    {
        u16Output = u16Limit;
        (psControllerPI)->u16LimitFlag = 1;
    }
    u16Limit = (psControllerPI)->u16Min;
    if (u16Output < u16Limit)
    {
        u16Output = u16Limit;
        (psControllerPI)->u16LimitFlag = 1;
    }
    return (u16Output);
}



/* *****************************************************************************
 * MCLIB_u16ControllerPIrAW - unsigned accumulator
 *
 * Description: recurrent form PI controller with integral anti windup
 **************************************************************************** */

uint16_t MCLIB_u16ControllerPIrAW(int16_t s16Error, MCLIB_sControllerPIrAWu16_t* psControllerPI)
{
    uint16_t u16Shift = (16 - (psControllerPI)->u16NShift);
    uint32_t u32Acc = MCLIB_u32AddSat_u32s32(
            MCLIB_u32AddSat_u32s32(
                (psControllerPI)->u32Acc,
                MCLIB_s32Mul_s16s16((psControllerPI)->s16PrevError, (psControllerPI)->s16CC2)),
            MCLIB_s32Mul_s16s16(s16Error, (psControllerPI)->s16CC1));
    uint32_t u32Limit;
    u32Limit = ((uint32_t)(psControllerPI)->u16Max << u16Shift);
    if (u32Acc > u32Limit)
    {
        u32Acc = u32Limit;
    }
    u32Limit = ((uint32_t)(psControllerPI)->u16Min << u16Shift);
    if (u32Acc < u32Limit)
    {
        u32Acc = u32Limit;
    }
    (psControllerPI)->u32Acc = u32Acc;
    (psControllerPI)->s16PrevError = s16Error;
    /* roundup (+32768 -> +0.5 ), but round down for the negative (-32768 -> -0.5 ) -> */
    return ((uint16_t)((uint32_t)(MCLIB_u32AddSat_u32s32(u32Acc, (u32Acc & 32768))) >> u16Shift));

}


/* *****************************************************************************
 * MCLIB_PIM_s16SvmStd - Standard Space Vector Modulation
 *
 * Description:
 **************************************************************************** */

int16_t MCLIB_PIM_s16SvmStd(MCLIB_s3Param_s16_t* psDutyCycle, MCLIB_s2Param_s16_t* psVoltageAlBeFiltered)
{
    int32_t s32Input3;
    int32_t s32Input5;
    int32_t s32Input7;

    int16_t s16Input1;
    int16_t s16Input4;
    int16_t s16Input6;
    int16_t s16InputA;
    int16_t s16InputB;
    int16_t s16InputC;
    int16_t s16Output;


    s16Input1 = MCLIB_s16Mul_s16s16((psVoltageAlBeFiltered)->s16Arg1, MCLIB_s16Fractional(0.866027832031259)); /* SQRT(3)/2 */
    s16Input4 = (psVoltageAlBeFiltered)->s16Arg2;
    s32Input5 = MCLIB_s32ShL(s16Input4, 14);
    s32Input3 = MCLIB_s32ShL((int32_t)s16Input1, 15);
    s32Input7 = s32Input5 + s32Input3;
    s32Input5 = s32Input5 - s32Input3;

    s16Input6 = MCLIB_s16ShR(s16Input4, 1);

    s16InputA = (s32Input7 >= MCLIB_s32Fractional(0.5)-1) ? (MCLIB_s16Fractional(0.5)-1) : ( (s32Input7 > MCLIB_s32Fractional(-0.5)) ? (s16Input6 + s16Input1) : (MCLIB_s16Fractional(-0.5)) );


    s16InputB = (s32Input5 >= MCLIB_s32Fractional(0.5)-1) ? (MCLIB_s16Fractional(0.5)-1) : ( (s32Input5 > MCLIB_s32Fractional(-0.5)) ? (s16Input6 - s16Input1) : (MCLIB_s16Fractional(-0.5)) );


    if (s16InputA < 0)
    {
        if (s16InputB < 0)
        {
            s16InputC = MCLIB_s16AddSat_s16s16(s16Input6, MCLIB_s16Fractional(0.5));
            (psDutyCycle)->s16Arg2 = s16InputC;
            s16InputC = MCLIB_s16SubSat_s16s16(s16InputC, s16InputB);
            (psDutyCycle)->s16Arg1 = s16InputC;
            (psDutyCycle)->s16Arg3 = MCLIB_s16SubSat_s16s16(s16InputC, s16InputA);
            s16Output = 4;
        }
        else
        {
            if (s16Input4 <= 0)
            {
                s16InputC = MCLIB_s16AddSat_s16s16(MCLIB_s16SubSat_s16s16(s16Input6, (int16_t)MCLIB_s32ShR(s32Input5, 16)), MCLIB_s16Fractional(0.5));
                (psDutyCycle)->s16Arg1 = MCLIB_s16Max(0, s16InputC);
                s16InputC = MCLIB_s16AddSat_s16s16(s16InputC, s16InputB);
                (psDutyCycle)->s16Arg2 = s16InputC;
                (psDutyCycle)->s16Arg3 = MCLIB_s16SubSat_s16s16(s16InputC, (psVoltageAlBeFiltered)->s16Arg2);
                s16Output = 3;
            }
            else
            {
                s16InputC = MCLIB_s16AddSat_s16s16(MCLIB_s16SubSat_s16s16((int16_t)MCLIB_s32ShR(s32Input7, 16), s16Input6), MCLIB_s16Fractional(0.5));
                (psDutyCycle)->s16Arg1 = MCLIB_s16Max(0, s16InputC);
                s16InputC = MCLIB_s16SubSat_s16s16(s16InputC, s16InputA);
                (psDutyCycle)->s16Arg3 = s16InputC;
                (psDutyCycle)->s16Arg2 = MCLIB_s16AddSat_s16s16(s16InputC, (psVoltageAlBeFiltered)->s16Arg2);
                s16Output = 2;
            }
        }

    }
    else
    {
        if (s16InputB >= 0)
        {
            s16InputC = MCLIB_s16ShR(MCLIB_s16SubSat_s16s16(MCLIB_s16Fractional(1.0), MCLIB_s16AddSat_s16s16(s16InputA, s16InputB)), 1)+1;
            (psDutyCycle)->s16Arg3 = s16InputC;
            s16InputC = MCLIB_s16AddSat_s16s16(s16InputC, s16InputA);
            (psDutyCycle)->s16Arg1 = s16InputC;
            (psDutyCycle)->s16Arg2 = MCLIB_s16AddSat_s16s16(s16InputC, s16InputB);
            s16Output = 1;
        }
        else
        {
            if (s16Input4 <= 0)
            {
                s16InputC = MCLIB_s16AddSat_s16s16(MCLIB_s16SubSat_s16s16(s16Input6, (int16_t)MCLIB_s32ShR(s32Input7, 16)), MCLIB_s16Fractional(0.49999));
                (psDutyCycle)->s16Arg2 = MCLIB_s16Max(0, s16InputC);
                s16InputB = MCLIB_s16SubSat_s16s16(s16InputC, (psVoltageAlBeFiltered)->s16Arg2);
                (psDutyCycle)->s16Arg3 = s16InputB;
                (psDutyCycle)->s16Arg1 = MCLIB_s16AddSat_s16s16(s16InputB, s16InputA);
                s16Output = 5;
            }
            else
            {
                s16InputB = MCLIB_s16SubSat_s16s16(MCLIB_s16Fractional(0.5)-1, (int16_t)MCLIB_s32ShR(s32Input7, 16));
                (psDutyCycle)->s16Arg3 = MCLIB_s16Max(0, s16InputB);
                s16InputB = MCLIB_s16AddSat_s16s16(s16InputB, (psVoltageAlBeFiltered)->s16Arg2);
                (psDutyCycle)->s16Arg2 = s16InputB;
                (psDutyCycle)->s16Arg1 = MCLIB_s16SubSat_s16s16(s16InputB, MCLIB_s16ShL((int16_t)MCLIB_s32ShR(s32Input5, 16), 1));
                s16Output = 0;
            }
        }
    }
    return s16Output;
}



