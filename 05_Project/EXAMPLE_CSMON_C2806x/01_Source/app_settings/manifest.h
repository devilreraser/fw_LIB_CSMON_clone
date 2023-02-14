#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Guard condition file contents not included more than once */
/* #pragma once */
#ifndef MANIFEST_H
#define MANIFEST_H

#include <stdint.h>


#define EXAMPLE_CSMON_C2806x_APPLICATION_VERSION_HI    6
#define EXAMPLE_CSMON_C2806x_APPLICATION_VERSION_LO    7


#define EXAMPLE_CSMON_C2806x_VENDOR_NAME            ("CONVESYS")
#define EXAMPLE_CSMON_C2806x_PRODUCT_CODE           ("CS-1291_R00_EXAMPLE_CSMON")
#define EXAMPLE_CSMON_C2806x_MAJOR_MINOR_REVISION   ("12.345")
#define EXAMPLE_CSMON_C2806x_VENDOR_URL             ("www.convesys.com")
#define EXAMPLE_CSMON_C2806x_PRODUCT_NAME           ("EXAMPLE_CSMON")
#define EXAMPLE_CSMON_C2806x_MODEL_NAME             ("ModelName")
#define EXAMPLE_CSMON_C2806x_USERAPPLICATION_NAME   ("ApplicationName")
#define EXAMPLE_CSMON_C2806x_USERRESERVE0_NAME      ("Reserve0")
#define EXAMPLE_CSMON_C2806x_USERRESERVE1_NAME      ("Reserve1")
#define EXAMPLE_CSMON_C2806x_USERRESERVE2_NAME      ("Reserve2")


static inline uint16_t MANIFEST_GetApplicationVersion(void)
{
    const uint16_t appVersion = (uint16_t)
            (
                    ((uint16_t)((uint_least8_t)((EXAMPLE_CSMON_C2806x_APPLICATION_VERSION_HI) & 0x00FF)) << 8) |
                    (           (uint_least8_t)((EXAMPLE_CSMON_C2806x_APPLICATION_VERSION_LO) & 0x00FF))
            );

    return appVersion;
}

#endif /* */

#ifdef __cplusplus
}
#endif /* __cplusplus */
