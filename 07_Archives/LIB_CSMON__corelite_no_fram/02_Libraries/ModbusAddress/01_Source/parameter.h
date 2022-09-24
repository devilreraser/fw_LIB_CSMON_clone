/* *****************************************************************************
 * File:   parameter.h
 * Author: Dimitar Lilov
 *
 * Created on 2020 03 20 23:31
 * 
 * Description: ...
 * 
 **************************************************************************** */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Guard condition file contents not included more than once */  
/* #pragma once */
#ifndef PARAMETER_H
#define	PARAMETER_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
    
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */
#ifndef STRING_CONCAT
#define STRING_CONCAT_BASE(_a_, _b_) _a_##_b_
#define STRING_CONCAT(_a_, _b_) STRING_CONCAT_BASE(_a_, _b_)
#endif

#define PAR(eType,eAccess,ePass) (uint16_t)(STRING_CONCAT(PAR_TYPE,eType) | (STRING_CONCAT(PAR_ACCESS,eAccess) << PAR_ACCESS_SHIFT) | (STRING_CONCAT(PAR_PASSRW,ePass) << PAR_PASSRW_SHIFT))

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */
typedef enum
{
    _UINT16     = 0x0001,   /* UINT16            */
    _SINT16     = 0x0002,   /* INT16             */
    _UINT32     = 0x0003,   /* UINT32            */
    _SINT32     = 0x0004,   /* INT32             */
    _WORD16     = 0x0005,   /* WORD              */
    _WORD32     = 0x0006,   /* DWORD             */
    _A_UINT16   = 0x0007,   /* Array of UINT16   */
    _A_SINT16   = 0x0008,   /* Array of INT16    */
    _A_UINT32   = 0x0009,   /* Array of UINT32   */
    _A_SINT32   = 0x000a,   /* Array of INT32    */
    _A_WORD16   = 0x000b,   /* Array of WORD     */
    _A_WORD32   = 0x000c,   /* Array of DWORD    */
    _UINT08     = 0x000d,   /* byte              */
    _A_UINT08   = 0x000e,   /* string            */

}PARAMETER_eTypesShort_t;

typedef enum
{
    PAR_TYPE_UNKNOWN    = 0x0000,   /* Not Specified     */
    PAR_TYPE_UINT16     = 0x0001,   /* UINT16            */
    PAR_TYPE_SINT16     = 0x0002,   /* INT16             */
    PAR_TYPE_UINT32     = 0x0003,   /* UINT32            */
    PAR_TYPE_SINT32     = 0x0004,   /* INT32             */
    PAR_TYPE_WORD16     = 0x0005,   /* WORD              */
    PAR_TYPE_WORD32     = 0x0006,   /* DWORD             */
    PAR_TYPE_A_UINT16   = 0x0007,   /* Array of UINT16   */
    PAR_TYPE_A_SINT16   = 0x0008,   /* Array of INT16    */
    PAR_TYPE_A_UINT32   = 0x0009,   /* Array of UINT32   */
    PAR_TYPE_A_SINT32   = 0x000a,   /* Array of INT32    */
    PAR_TYPE_A_WORD16   = 0x000b,   /* Array of WORD     */
    PAR_TYPE_A_WORD32   = 0x000c,   /* Array of DWORD    */
    PAR_TYPE_UINT08     = 0x000d,   /* byte              */
    PAR_TYPE_A_UINT08   = 0x000e,   /* string            */
    PAR_TYPE_MASK       = 0x000f,
    PAR_TYPE_SHIFT      = 0x0000
}PARAMETER_eTypes_t;

#ifndef __XC16
typedef enum
{
    _NO       = 0x0000,   /* No Password */
    _RD       = 0x0001,   /* Password Access Only */
    _WR       = 0x0002,   /* Password Access Write */
}PARAMETER_ePassReadWriteShort_t;
#else
typedef enum
{
    _NO_PASS       = 0x0000,   /* No Password */
    _RD_PASS       = 0x0001,   /* Password Access Only */
    _WR_PASS       = 0x0002,   /* Password Access Write */
}PARAMETER_ePassReadWriteShort_t;
#endif

typedef enum
{
    PAR_PASSRW_NO       = 0x0000,   /* No Password - All can be accessed Free */
    PAR_PASSRW_RD       = 0x0001,   /* Password Access Only - Read is Free */
    PAR_PASSRW_WR       = 0x0002,   /* Password Access Write */
    PAR_PASSRW_ON       = 0x0003,   /* Password Access Only -> Same As PAR_PASSRW_RD */
    PAR_PASSRW_SHIFT    = 0x0007    /* bPassRead & bPassWrite Setup ShiftCount */
}PARAMETER_ePassReadWrite_t;

typedef enum
{
    _NA       = 0x0000,   /* No Access         */
    _RO       = 0x0001,   /* Read Only         */
    _WO       = 0x0002,   /* Write Only        */
    _RW       = 0x0003,   /* Read Write        */
}PARAMETER_eAccessShort_t;

typedef enum
{
    PAR_ACCESS_NA       = 0x0000,   /* No Access         */
    PAR_ACCESS_RO       = 0x0001,   /* Read Only         */
    PAR_ACCESS_WO       = 0x0002,   /* Write Only        */
    PAR_ACCESS_RW       = 0x0003,   /* Read Write        */
    PAR_ACCESS_MASK_RD  = 0x0001,   /* Read Mask         */
    PAR_ACCESS_MASK_WR  = 0x0002,   /* Write Mask        */
    PAR_ACCESS_MASK     = 0x0003,   /* Access Mask       */
    PAR_ACCESS_SHIFT    = 0x0004,
}PARAMETER_eAccess_t;

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

typedef struct
{
    uint16_t            :10;
    uint16_t bDirectWR  : 1;    /* Direct Write */
    uint16_t bFunctnWR  : 1;    /* Function Write */
    uint16_t            : 4;

}PARAMETER_sAttributesWriteMode_t;

typedef struct
{
    uint16_t eDataType  : 4;    /* PARAMETER_eTypes_t */

    uint16_t eAccess    : 2;    /* PARAMETER_eAccess_t */
    uint16_t bState     : 1;    /* Deny Access in Special Conditions */
    uint16_t bPassRead  : 1;    /* Password Access Only - Hidden Parameter */

    uint16_t bPassWrite : 1;    /* Password Access Write - Read Access Free */
    uint16_t bReserved1 : 1;    /* Not Used */
    uint16_t eWriteMode : 2;    /* Write Mode -> Direct Write|Function Write */

    uint16_t bInMenu    : 1;    /* Available In Parameter Group Menu */
    uint16_t bHidden    : 1;    /* Hidden from the Menu Group */
    uint16_t bReserved2 : 1;    /* Not Used */
    uint16_t bStore     : 1;    /* For EEPROM Storage */

}PARAMETER_sAttributes_t;

typedef union
{
    uint16_t u16Register;
    PARAMETER_sAttributes_t sAttributes;
    PARAMETER_sAttributesWriteMode_t sAttributesWriteMode;
}PARAMETER_uAttributes_t;

/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
 
#endif	/* PARAMETER_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
