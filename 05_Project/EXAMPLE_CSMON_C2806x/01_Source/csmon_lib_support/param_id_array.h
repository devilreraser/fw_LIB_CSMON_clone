#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _01_SOURCE_CSMON_LIB_SUPPORT_PARAM_ID_ARRAY_H_
#define _01_SOURCE_CSMON_LIB_SUPPORT_PARAM_ID_ARRAY_H_

#include <stdint.h>


typedef struct sParam_id_array_t_
{
    uint16_t* pParamIds;
    uint16_t paramsCount;
} sParam_id_array_t;

typedef sParam_id_array_t* PtrParam_id_array_t;


typedef struct sCParam_id_array_t_
{
    const uint16_t* pParamIds;
    uint16_t paramsCount;
} sCParam_id_array_t;

typedef const sCParam_id_array_t* PtrCParam_id_array_t;


#endif /* _01_SOURCE_CSMON_LIB_SUPPORT_PARAM_ID_ARRAY_H_ */

#ifdef __cplusplus
}
#endif /* __cplusplus */

