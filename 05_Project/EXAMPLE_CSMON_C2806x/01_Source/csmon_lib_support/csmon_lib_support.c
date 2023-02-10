#include "csmon_lib_support.h"

#include "datetime.h"

#include "MotorParam/Generated/csmon_config.h"
#include "app_settings/manifest.h"
#include "app_settings/projectcfg.h"

#include "HexMonitor/01_Source/TI/hmdrv_init_dynamic_params.h"
#include "HexMonitor/01_Source/hmlib_init_dynamic_params.h"

#include "debug.h"


CSMON_LIB_SUPPORT_Responses_t csMonResponses;

bool CSMON_LIB_SUPPORT_bDateTimeSet = false;


CSMON_LIB_SUPPORT_sDateTime_t MAIN_sDateTimeGet =
{
    0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00 /* 2001-01-01-Mon-00:00:00 */
};


CSMON_LIB_SUPPORT_sDateTime_t MAIN_sDateTimeSet =
{
    BUILD_RTC_SEC,
    BUILD_RTC_MIN,
    BUILD_RTC_HOUR,
    0x00,
    BUILD_RTC_DAY,
    BUILD_RTC_MONTH,
    BUILD_RTC_YEAR,
    0x00,
    /* 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00 2001-01-01-Mon-00:00:00 */
};


/* *****************************************************************************
 * CSMON_vSetDateTime - Set RTC Value - called from CSMON library
 **************************************************************************** */
extern void CSMON_vSetDateTime (
        const uint_least8_t u8BCDSeconds,
        const uint_least8_t u8BCDMinutes,
        const uint_least8_t u8BCDHours,
        const uint_least8_t u8BCDWeekdays,
        const uint_least8_t u8BCDDay,
        const uint_least8_t u8BCDMonth,
        const uint_least8_t u8BCDYear)
{
    CSMON_LIB_SUPPORT_DateTime_set(&MAIN_sDateTimeSet, u8BCDSeconds, u8BCDMinutes, u8BCDHours, u8BCDWeekdays, u8BCDDay, u8BCDMonth, u8BCDYear);
    CSMON_LIB_SUPPORT_bDateTimeSet = true;
}


/* *****************************************************************************
 * CSMON_vGetDateTime - Get RTC Value - called from CSMON library
 **************************************************************************** */
extern void CSMON_vGetDateTime (
        uint_least8_t* const pu8BCDSeconds,
        uint_least8_t* const pu8BCDMinutes,
        uint_least8_t* const pu8BCDHours,
        uint_least8_t* const pu8BCDWeekdays,
        uint_least8_t* const pu8BCDDay,
        uint_least8_t* const pu8BCDMonth,
        uint_least8_t* const pu8BCDYear)
{
    *pu8BCDSeconds  = MAIN_sDateTimeGet.u8Seconds;
    *pu8BCDMinutes  = MAIN_sDateTimeGet.u8Minutes;
    *pu8BCDHours    = MAIN_sDateTimeGet.u8Hours;
    *pu8BCDWeekdays = MAIN_sDateTimeGet.u8Weekdays;
    *pu8BCDDay      = MAIN_sDateTimeGet.u8Day;
    *pu8BCDMonth    = MAIN_sDateTimeGet.u8Month;
    *pu8BCDYear     = MAIN_sDateTimeGet.u8Year;
}


/* *****************************************************************************
 * CsMon_init
 **************************************************************************** */
extern void CSMON_LIB_SUPPORT_init(void)
{
    CSMON_Responses_init(&csMonResponses);

    {
        const HMDRV_sInitDynamicParams_v1_t hmdrvInitDynamicParams =
        {
         .size = sizeof(HMDRV_sInitDynamicParams_v1_t),
         .SCI_baud = EXAMPLE_CSMON_C2806x_HMDRV_MODBUS_BAUD,
         .SCI_config = SCI_CONFIG_WLEN_8 | EXAMPLE_CSMON_C2806x_STOP_BITS | EXAMPLE_CSMON_C2806x_UART_PARITY_MODBUS
        };

        const HMLIB_sInitDynamicParams_v1_t hmlibInitDynamicParams =
        {
         .size = sizeof(HMLIB_sInitDynamicParams_v1_t),
         .pHMDRV_sInitDynamicParams = &hmdrvInitDynamicParams
        };

        const CSMON_sInitDynamicParams_v1_t csmonInitDynamicParams =
        {
         .size = sizeof(CSMON_sInitDynamicParams_v1_t),
         .pHMLIB_sInitDynamicParams = &hmlibInitDynamicParams
        };

        // CSMON Initialization -> ~ 2.25ms
        csMonResponses.eInit = CSMON_eInitDynamic(&csmonInitDynamicParams);
    }

    // Check CSMON Response Code if needed
    if (csMonResponses.eInit != CSMON_RESPONSE_CODE_OK)
    {
        /* If enters here - Fix Peripheral Frequency for Better Performance and Stability (DEVICE_LSPCLK_FREQ) */
        u32GetBaudError_PPM = CSMON_u32GetBaudError_PPM(CSMON_ID_PERIPHERAL_SCI_MODBUS);
        ASSERT(u32GetBaudError_PPM >= CSMON_u32PercentToPPM(3.0));
    }
    else
    {
        u32GetBaudError_PPM = CSMON_u32GetBaudError_PPM(CSMON_ID_PERIPHERAL_SCI_MODBUS);
    }

    {
        // Firmware Application Version Set to CSMON
        const uint16_t applicationVersion = MANIFEST_GetApplicationVersion();
        (void)CSMON_eSetFirmwareApplicationVersion(applicationVersion);
    }

    CSMON_LIB_SUPPORT_eSetParameterList(asParameterList, CSMON_CONFIG_PARAMETER_COUNT_MAX);

    /* Recorder And Scope Initialization Made Once after parameter initialized */
    CsMonRecordersInitialization();

    // CSMON Internal Recorders Setup with Already Made Configuration
    CSMON_vAddSetupRecorderParameterMask(CSMON_MASK_RECORDER_0);
}


extern void CSMON_LIB_SUPPORT_eSetParameterListCommon(
        const volatile MAIN_sParameterList_t* const pParameterList,
        const size_t maxParameterCount
        )
{
    const uint16_t u16Offset = sizeof(pParameterList[0]);
    const uint16_t u16MaxParameterCount = (uint16_t)maxParameterCount;

    /* First Put Real Address to calculate count parameters internally (last index is NULL) */
    CSMON_LIB_SUPPORT_eSetParameterListRealAddress(&pParameterList[0].u16RealAddress, u16Offset, u16MaxParameterCount);

    CSMON_eSetParameterListParameterID((uint16_t *)&pParameterList[0].u16ParameterIndexID, u16Offset);
    CSMON_eSetParameterListRegisterSize((uint16_t *)&pParameterList[0].uParameterSize.u16Register, u16Offset);
    CSMON_eSetParameterListRegisterFlags((uint16_t *)&pParameterList[0].uParameterFlags.u16Register, u16Offset);

    CSMON_eSetParameterListDataMaximum((uint32_t *)&pParameterList[0].u32Max.u32Register, u16Offset);
    CSMON_eSetParameterListDataMinimum((uint32_t *)&pParameterList[0].u32Min.u32Register, u16Offset);
    CSMON_eSetParameterListDataDefault((uint32_t *)&pParameterList[0].u32Def.u32Register, u16Offset);

    /* 0.0 - Default HEX Visualization; Any other -> Default Decimal Visualization */
    CSMON_eSetParameterListValueFormat((float *)&pParameterList[0].Norm, sizeof(asParameterList[0]));
}
