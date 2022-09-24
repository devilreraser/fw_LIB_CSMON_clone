/* *****************************************************************************
 * File:   io_drv.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 11 25 01:11
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
#ifndef IO_DRV_H
#define	IO_DRV_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdbool.h>
#include <stdint.h>
//#include "stdextend.h"
#include "boardcfg.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */


/* *****************************************************************************
 * SCI
 **************************************************************************** */

#if _LAUNCHXL_F28379D

	#define SCITXDA_PIN 42
	#define SCIRXDA_PIN 43
	#define SCIREA_PIN  120
	#define SCIDEA_PIN  143

	#define SCITXDB_PIN 137
	#define SCIRXDB_PIN 138
	#define SCIREB_PIN  121
	#define SCIDEB_PIN  144

#elif defined (_LAUNCHXL_F28069M)

	#define SCITXDA_PIN 29
	#define SCIRXDA_PIN 28
	#define SCIREA_PIN  120
	#define SCIDEA_PIN  143

	#define SCITXDB_PIN 58
	#define SCIRXDB_PIN 15
	#define SCIREB_PIN  121
	#define SCIDEB_PIN  144

#else

	#if BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC

		#define SCITXDA_PIN 42
		#define SCIRXDA_PIN 43
		//#define SCIREA_PIN  144
		//#define SCIDEA_PIN  143

	#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG

/*
        #define SCITXDA_PIN 135
        #define SCIRXDA_PIN 136
        #define SCIREA_PIN  120
        #define SCIDEA_PIN  143
*/
        //Fixed for CoreLite
        #define SCITXDA_PIN 29
        #define SCIRXDA_PIN 28

		#define SCITXDB_PIN 137
		#define SCIRXDB_PIN 138
		#define SCIREB_PIN  121
		#define SCIDEB_PIN  144

	#elif  BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_SMALL_TEST

		#define SCIRXDA_PIN 28
		#define SCITXDA_PIN 29

	#endif

#endif




#define SCIRE_ENABLE   0
#define SCIDE_ENABLE   1

#define SCIRE_DISABLE   (!SCIRE_ENABLE)
#define SCIDE_DISABLE   (!SCIDE_ENABLE)



/* 28075 Specific Pinout */

#if _LAUNCHXL_F28379D

#elif defined (_LAUNCHXL_F28069M)

#else

#if  BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_SMALL_TEST

#define PRCH_REL_CTRL_PIN       99
#define CONTACTOR_CTRL_PIN      36

#define CONTACTOR_FB_DSP_PIN    34

#define CS_NEG_PIN              22
#define CS_POS_PIN              23

#define SPI_A_WP_PIN            20
#define SPI_A_HOLD_PIN          21

#endif

#endif



#if _LAUNCHXL_F28379D

#define STAT_LED_G_PIN      28      /* N/A */
#define STAT_LED_A_B_PIN    31      /* D10 Blue */
#define STAT_LED_R_PIN      34      /* D9 */

#else

#if (BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG)

#define STAT_LED_G_PIN      16
#define STAT_LED_A_B_PIN    17      /* Amber */
#define STAT_LED_R_PIN      18

#endif

#endif

#define STAT_LED_ENABLE 0
#define STAT_LED_DISABLE   (!STAT_LED_ENABLE)



#if BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG

#define DI1_PIN     96
#define DI2_PIN     97
#define DI3_PIN     98
#define DI4_PIN     99
#define DI5_PIN    100
#define DI6_PIN    101
#define DI7_PIN    102
#define DI8_PIN    103
#define DI9_PIN    104
#define DI10_PIN   105
#define DI11_PIN   106
#define DI12_PIN   107
#define DI13_PIN   108
#define DI14_PIN   109
#define DI15_PIN   110
#define DI16_PIN   111
#define DI17_PIN   112
#define DI18_PIN   113
#define DI19_PIN   114
#define DI20_PIN   115
#define DI21_PIN   116
#define DI22_PIN   117
#define DI23_PIN   118
#define DI24_PIN   119

#define DO1_PIN      8
#define DO2_PIN      9
#define DO3_PIN     10
#define DO4_PIN     11
#define DO5_PIN     12
#define DO6_PIN     13
#define DO7_PIN     14
#define DO8_PIN     15
#define DO9_PIN     20
#define DO10_PIN    21
#define DO11_PIN    22
#define DO12_PIN    23
#define DO13_PIN    24
#define DO14_PIN    25
#define DO15_PIN    26
#define DO16_PIN    27

#define CLK_EN_FPGA_PIN     33

#define FLT1_PIN            132
#define FLT2_PIN            133
#define FLT3_PIN            134
#define FLT4_PIN            139     /* MIX With SCIRXDC_PIN */
#define FLT5_PIN            140     /* MIX With SCITXDC_PIN */
#define FLT6_PIN            141

#define SCIRXDC_PIN         139     /* MIX With FLT4_PIN */
#define SCITXDC_PIN         140     /* MIX With FLT5_PIN */




#define SDC_DET_PIN            124 /* Card Detect */
#define SDC_WP_PIN             125 /* Write Protect */

#define SDC_RSV1_PIN            126
#define SDC_RSV2_PIN            127

#define OE_PWM_BUF1         93
#define OE_PWM_BUF2         94
#define OE_PWM_BUF3         95
#define OE_PWM_BUF4         122
#define OE_PWM_BUF5         123
#define OE_PWM_BUF6         142

#if BOARDCFG_USE_FPGA_SCI
#define ENA_PWM1_PIN        OE_PWM_BUF1
#define ENA_PWM2_PIN        OE_PWM_BUF2
#define ENA_PWM3_PIN        OE_PWM_BUF3
#else
#define ENA_PWM1_PIN        OE_PWM_BUF1
#define ENA_PWM2_PIN        OE_PWM_BUF1
#define ENA_PWM3_PIN        OE_PWM_BUF1
#endif

#if BOARDCFG_USE_FPGA_SCI
#define CLR_FLT_PIN         OE_PWM_BUF6
#else
#define CLR_FLT_PIN         OE_PWM_BUF5
#endif



#endif

#define USE_ENABLE_OUTPUT1  1   /* not controlled from uEnableOutput if 0 */
#define USE_ENABLE_OUTPUT2  1   /* not controlled from uEnableOutput if 0 */
#define USE_ENABLE_OUTPUT3  1   /* not controlled from uEnableOutput if 0 */
#define USE_ENABLE_OUTPUT4  1   /* not controlled from uEnableOutput if 0 */
#define USE_ENABLE_OUTPUT5  1   /* not controlled from uEnableOutput if 0 */
#define USE_ENABLE_OUTPUT6  1   /* not controlled from uEnableOutput if 0 */

#define MASK_DENY_FAULT_INPUT1  0
#define MASK_DENY_FAULT_INPUT2  0
#define MASK_DENY_FAULT_INPUT3  0
#define MASK_DENY_FAULT_INPUT4  0
#define MASK_DENY_FAULT_INPUT5  0
#define MASK_DENY_FAULT_INPUT6  0

#define ENA_PWM_RUN         1
#define ENA_PWM_OFF         !(ENA_PWM_RUN)

#define CLR_FLT_RST         1
#define CLR_FLT_IDL         !(CLR_FLT_RST)

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */
#define GPIO_PIN_MODE(_pin_, _mode_) STRING_CONCAT(GPIO_, STRING_CONCAT(_pin_, STRING_CONCAT(_, _mode_)))

#define GPIO_PIN_MODE_GPIO(_pin_) STRING_CONCAT(GPIO_, STRING_CONCAT(_pin_, STRING_CONCAT(_GPIO, _pin_)))




/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */
typedef enum
{
    DIO_PIN_EN_1,
    DIO_PIN_EN_2,
    DIO_PIN_EN_3,
    DIO_PIN_EN_4,
    DIO_PIN_EN_COUNT,
}DIO_eEnablePin;


/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
typedef struct
{
    uint16_t bFLT1 : 1;
    uint16_t bFLT2 : 1;
    uint16_t bFLT3 : 1;
    uint16_t bFLT4 : 1;
    uint16_t bFLT5 : 1;
    uint16_t bFLT6 : 1;
    uint16_t bFLT7 : 1;
    uint16_t bFLT8 : 1;
}sFaultInput_t;

typedef struct
{
    uint16_t bEDET : 1;
    uint16_t bERR : 1;
    uint16_t bOCPIph : 1;
    uint16_t bOVT : 1;
    uint16_t bOCNIph : 1;
    uint16_t bOVDCL : 1;
    uint16_t bFLT7 : 1;
    uint16_t bFLT8 : 1;
}sFaultInputSCC_t;


typedef struct
{
    uint16_t bEN1 : 1;
    uint16_t bEN2 : 1;
    uint16_t bEN3 : 1;
    uint16_t bEN4 : 1;
    uint16_t bEN5 : 1;
    uint16_t bEN6 : 1;
    uint16_t bEN7 : 1;
    uint16_t bEN8 : 1;
}sEnableOutput_t;

typedef struct
{
    uint16_t bEnablePWM : 1;
    uint16_t bEnablePWM2 : 1;
    uint16_t bEnablePWM3 : 1;
    uint16_t bEN4 : 1;
    uint16_t bClearFaults : 1;
    uint16_t bEN6 : 1;
    uint16_t bEN7 : 1;
    uint16_t bEN8 : 1;
}sEnableOutputSCC_t;

typedef struct
{
    uint16_t bF0 : 1;
    uint16_t bF1 : 1;
    uint16_t bF2 : 1;
    uint16_t bF3 : 1;
    uint16_t bF4 : 1;
    uint16_t bF5 : 1;
    uint16_t bF6 : 1;
    uint16_t bF7 : 1;

    uint16_t bF8 : 1;
    uint16_t bF9  : 1;
    uint16_t bF10 : 1;
    uint16_t bF11 : 1;
    uint16_t bF12 : 1;
    uint16_t bF13 : 1;
    uint16_t bF14 : 1;
    uint16_t bF15 : 1;


}sFunctionIO_t;

typedef struct
{
    uint16_t bD1 : 1;
    uint16_t bD2 : 1;
    uint16_t bD3 : 1;
    uint16_t bD4 : 1;
    uint16_t bD5 : 1;
    uint16_t bD6 : 1;
    uint16_t bD7 : 1;
    uint16_t bD8 : 1;

    uint16_t bD9  : 1;
    uint16_t bD10 : 1;
    uint16_t bD11 : 1;
    uint16_t bD12 : 1;
    uint16_t bD13 : 1;
    uint16_t bD14 : 1;
    uint16_t bD15 : 1;
    uint16_t bD16 : 1;

}sControlIO_t;

typedef union
{
    uint16_t u16Register;
    sControlIO_t sControlIO;
}uControlIO_t;

typedef union
{
    uint16_t u16Register;
    sFunctionIO_t sFunctionIO;
}uFunctionIO_t;

typedef union
{
    uint16_t u16Register;
    sFaultInput_t sFaultInput;
    sFaultInputSCC_t sFaultInputSCC;
}uFaultInput_t;

typedef union
{
    uint16_t u16Register;
    sEnableOutput_t sEnableOutput;
    sEnableOutputSCC_t  sEnableOutputSCC;
}uEnableOutput_t;

/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */
#define GPIO_setPinConfigInput(_pin_) \
        GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(_pin_));\
        GPIO_setDirectionMode(_pin_, GPIO_DIR_MODE_IN);\
        GPIO_setPadConfig(_pin_, GPIO_PIN_TYPE_STD);\
        GPIO_setQualificationMode(_pin_, GPIO_QUAL_6SAMPLE);\
        GPIO_setMasterCore(_pin_, GPIO_CORE_CPUA)

#define GPIO_setPinConfigOutput(_pin_) \
        GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(_pin_));\
        GPIO_setDirectionMode(_pin_, GPIO_DIR_MODE_OUT);\
        GPIO_setPadConfig(_pin_, GPIO_PIN_TYPE_STD);\
        GPIO_setQualificationMode(_pin_, GPIO_QUAL_ASYNC);\
        GPIO_writePin(_pin_, 0);\
        GPIO_setMasterCore(_pin_, GPIO_CORE_CPUA)


/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 
extern uControlIO_t uDIExternal;
extern uControlIO_t uDOExternalFbk;

extern uint16_t u16DigitalOutputs;
extern uint32_t u32DigitalInputs;

#define u16DOExternal u16DigitalOutputs

extern uFaultInput_t uFaultInput;
extern uFaultInput_t uFaultInputDeny;

extern uEnableOutput_t uEnableOutput;

extern bool IO_bUseIOFbkEmulation;



/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void IO_vInit(void);
void IO_vProcessEnable2346(uint16_t nStack);
void IO_vProcess(void);
void IO_vClearTrippedErrors (void);
bool IO_COMMON_bIsError(void);

#endif	/* IO_DRV_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
