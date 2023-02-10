#include "csmon_lib_support_parameter_list.h"

#include "csmon_lib_support/csmon_lib_support.h"


extern void CSMON_LIB_SUPPORT_eSetParameterListExternal(
        const volatile MAIN_sParameterList_t* const pParameterList,
        const size_t maxParameterCount
        )
{
    CSMON_LIB_SUPPORT_eSetParameterListCommon(pParameterList, maxParameterCount);

    const uint16_t u16Offset = sizeof(pParameterList[0]);

    CSMON_eSetParameterListFunctionCode((uint16_t *)&pParameterList[0].uFunctionCode, u16Offset);
    CSMON_eSetParameterListAccessLevel((uint16_t *)&pParameterList[0].uAccessLevel, u16Offset);
    CSMON_eSetParameterListBitsCount((uint16_t *)&pParameterList[0].uBitsCount, u16Offset);
    CSMON_eSetParameterListShortNaming((uint_least8_t *)&pParameterList[0].au8NameUnit, u16Offset);
    CSMON_eSetParameterListDataType((uint16_t *)&pParameterList[0].eDataTypeAttribute, u16Offset);
}
