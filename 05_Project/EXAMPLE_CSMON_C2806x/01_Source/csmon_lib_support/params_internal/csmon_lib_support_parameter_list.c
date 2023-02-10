#include "csmon_lib_support_parameter_list.h"

#include "csmon_lib_support/csmon_lib_support.h"


extern void CSMON_LIB_SUPPORT_eSetParameterListInternal(
        const volatile MAIN_sParameterList_t* const pParameterList,
        const size_t maxParameterCount
        )
{
    return CSMON_LIB_SUPPORT_eSetParameterListCommon(pParameterList, maxParameterCount);
}
