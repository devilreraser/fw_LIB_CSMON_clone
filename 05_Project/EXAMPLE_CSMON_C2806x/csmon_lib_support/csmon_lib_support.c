#include "csmon_lib_support.h"

#include "csmon_config.h"
#include "datetime.h"
#include "app_settings/manifest.h"

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

    // CSMON Initialization -> ~ 2.25ms
    csMonResponses.eInit = CSMON_eInit();

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

    CsMonExternalParametersInitialization();

    /* Recorder And Scope Initialization Made Once after parameter initialized */
    CsMonRecordersInitialization();

    // CSMON Internal Recorders Setup with Already Made Configuration
    CSMON_vAddSetupRecorderParameterMask(CSMON_MASK_RECORDER_0);
}
