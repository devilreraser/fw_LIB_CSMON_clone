#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _01_SOURCE_CSMON_LIB_SUPPORT_PARAM_ID_ARRAY_2D_H_
#define _01_SOURCE_CSMON_LIB_SUPPORT_PARAM_ID_ARRAY_2D_H_

#include "param_id_array.h"


typedef struct sParam_id_array_2d_t_
{
    PtrParam_id_array_t pParamIdsArrays;
    uint16_t arraysCount;
} sParam_id_array_2d_t;

typedef sParam_id_array_2d_t* PtrParam_id_array_2d_t;


typedef struct sCParam_id_array_2d_t_
{
    PtrCParam_id_array_t pParamIdsArrays;
    uint16_t arraysCount;
} sCParam_id_array_2d_t;

typedef const sCParam_id_array_2d_t* PtrCParam_id_array_2d_t;


#endif /* _01_SOURCE_CSMON_LIB_SUPPORT_PARAM_ID_ARRAY_2D_H_ */

#ifdef __cplusplus
}
#endif /* __cplusplus */

