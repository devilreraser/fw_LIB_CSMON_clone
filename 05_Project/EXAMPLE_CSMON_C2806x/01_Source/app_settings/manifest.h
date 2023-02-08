#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Guard condition file contents not included more than once */
/* #pragma once */
#ifndef MANIFEST_H
#define MANIFEST_H

#include <stdint.h>

#define TEST_CSMON_APPLICATION_VERSION_HI    1
#define TEST_CSMON_APPLICATION_VERSION_LO    0

#define TEST_CSMON_APPLICATION_VERSION       (uint16_t)(((uint16_t)((uint_least8_t)(TEST_CSMON_APPLICATION_VERSION_HI & 0x00FF)) << 8) | ((uint_least8_t)(TEST_CSMON_APPLICATION_VERSION_LO & 0x00FF)))


static inline uint16_t MANIFEST_GetApplicationVersion(void)
{
    return TEST_CSMON_APPLICATION_VERSION;
}

#endif /* */

#ifdef __cplusplus
}
#endif /* __cplusplus */
