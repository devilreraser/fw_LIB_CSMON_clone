/* *****************************************************************************
 * File:   sys_drv.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 11 23 13:21
 * 
 * Description: ...
 * 
 **************************************************************************** */
#ifdef _WIN32
#pragma once
#endif /* _WIN32 */

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "sys_drv.h"
#include "driverlib.h"
#include "device.h"
#include "hmcfg.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
UART_sModuleConfiguration_t sHexMonUartModuleConfiguration =
{
    HMDRV_HEXMON_BAUD,
    (UART_eParityAndData_t)UART_PARITY_HEXMON,
    (UART_eStopBits_t)UART_STOP_BITS,
};


/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */


/* EPWM Pinout Depends on _LAUNCHXL_F28379D global define */


void SYS_InitGPIO(void)
{

    //
    // UARTs
    //
#if (HMDRV_USE_UART_INIT == 0) || (HMDRV_USE_SCIA == 0)

    //
    // SCIRXDA_PIN is the SCI Rx pin.
    //
    GPIO_setPinConfig(GPIO_PIN_MODE(SCIRXDA_PIN, SCIRXDA));
    GPIO_setDirectionMode(SCIRXDA_PIN, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(SCIRXDA_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SCIRXDA_PIN, GPIO_QUAL_ASYNC);
    GPIO_setMasterCore(SCIRXDA_PIN, GPIO_CORE_CPUA);

    //
    // SCITXDA_PIN is the SCI Tx pin.
    //
    GPIO_setPinConfig(GPIO_PIN_MODE(SCITXDA_PIN, SCITXDA));
    GPIO_setDirectionMode(SCITXDA_PIN, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(SCITXDA_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SCITXDA_PIN, GPIO_QUAL_ASYNC);
    GPIO_setMasterCore(SCITXDA_PIN, GPIO_CORE_CPUA);

    #if BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG
    //
    // SCIREA_PIN is the SCI RE pin.
    //
    GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(SCIREA_PIN));
    GPIO_setDirectionMode(SCIREA_PIN, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(SCIREA_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SCIREA_PIN, GPIO_QUAL_ASYNC);
    if (UART_bUseSCIA_On_USB_Connector())
    {
        GPIO_writePin(SCIREA_PIN, SCIRE_DISABLE);
    }
    else
    {
        GPIO_writePin(SCIREA_PIN, SCIRE_ENABLE);
    }
    GPIO_setMasterCore(SCIREA_PIN, GPIO_CORE_CPUA);

    //
    // SCIDEA_PIN is the SCI DE pin.
    //
    GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(SCIDEA_PIN));
    GPIO_setDirectionMode(SCIDEA_PIN, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(SCIDEA_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SCIDEA_PIN, GPIO_QUAL_ASYNC);
    GPIO_writePin(SCIDEA_PIN, SCIDE_DISABLE);
    GPIO_setMasterCore(SCIDEA_PIN, GPIO_CORE_CPUA);
    #endif
#endif


#if (HMDRV_USE_UART_INIT == 0) || (HMDRV_USE_SCIA == 1)
    #if BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG
    //
    // SCIRXDB_PIN is the SCI Rx pin.
    //
    GPIO_setPinConfig(GPIO_PIN_MODE(SCIRXDB_PIN, SCIRXDB));
    GPIO_setDirectionMode(SCIRXDB_PIN, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(SCIRXDB_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SCIRXDB_PIN, GPIO_QUAL_ASYNC);
    GPIO_setMasterCore(SCIRXDB_PIN, GPIO_CORE_CPUA);

    //
    // SCITXDB_PIN is the SCI Tx pin.
    //
    GPIO_setPinConfig(GPIO_PIN_MODE(SCITXDB_PIN, SCITXDB));
    GPIO_setDirectionMode(SCITXDB_PIN, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(SCITXDB_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SCITXDB_PIN, GPIO_QUAL_ASYNC);
    GPIO_setMasterCore(SCITXDB_PIN, GPIO_CORE_CPUA);

    //
    // SCIREB_PIN is the SCI RE pin.
    //
    GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(SCIREB_PIN));
    GPIO_setDirectionMode(SCIREB_PIN, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(SCIREB_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SCIREB_PIN, GPIO_QUAL_ASYNC);
    GPIO_writePin(SCIREB_PIN, SCIRE_ENABLE);
    GPIO_setMasterCore(SCIREB_PIN, GPIO_CORE_CPU1);

    //
    // SCIDEB_PIN is the SCI DE pin.
    //
    GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(SCIDEB_PIN));
    GPIO_setDirectionMode(SCIDEB_PIN, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(SCIDEB_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SCIDEB_PIN, GPIO_QUAL_ASYNC);
    GPIO_writePin(SCIDEB_PIN, SCIDE_DISABLE);
    GPIO_setMasterCore(SCIDEB_PIN, GPIO_CORE_CPU1);
    #endif

#endif

    #if BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG
    #if BOARDCFG_USE_FPGA_SCI_C_RX
    //
    // SCIRXDC_PIN is the SCI Rx pin.
    //
    GPIO_setPinConfig(GPIO_PIN_MODE(SCIRXDC_PIN, SCIRXDC));
    GPIO_setDirectionMode(SCIRXDC_PIN, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(SCIRXDC_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SCIRXDC_PIN, GPIO_QUAL_ASYNC);
    GPIO_setMasterCore(SCIRXDC_PIN, GPIO_CORE_CPUA);
    #endif

    #if BOARDCFG_USE_FPGA_SCI_C_TX
    //
    // SCITXDC_PIN is the SCI Tx pin.
    //
    GPIO_setPinConfig(GPIO_PIN_MODE(SCITXDC_PIN, SCITXDC));
    GPIO_setDirectionMode(SCITXDC_PIN, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(SCITXDC_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SCITXDC_PIN, GPIO_QUAL_ASYNC);
    GPIO_setMasterCore(SCITXDC_PIN, GPIO_CORE_CPUA);
    #endif
    #endif






    //
    // EPWM
    //
    #if BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG
    InitEPwmGpio();
    #elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_SMALL_TEST
    InitEPwm6Gpio();
    InitEPwm7Gpio();
    InitEPwm8Gpio();
    #endif



    //
    // LEDs
    //

    #if BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG
    //
    // STAT_LED_G_PIN is the LED STATUS pin.
    //
    GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(STAT_LED_G_PIN));
    GPIO_setDirectionMode(STAT_LED_G_PIN, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(STAT_LED_G_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(STAT_LED_G_PIN, GPIO_QUAL_ASYNC);
    GPIO_writePin(STAT_LED_G_PIN, STAT_LED_ENABLE);
    GPIO_setMasterCore(STAT_LED_G_PIN, GPIO_CORE_CPUA);

    //
    // STAT_LED_A_B_PIN is the LED STATUS pin.
    //
    GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(STAT_LED_A_B_PIN));
    GPIO_setDirectionMode(STAT_LED_A_B_PIN, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(STAT_LED_A_B_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(STAT_LED_A_B_PIN, GPIO_QUAL_ASYNC);
    GPIO_writePin(STAT_LED_A_B_PIN, STAT_LED_ENABLE);
    GPIO_setMasterCore(STAT_LED_A_B_PIN, GPIO_CORE_CPUA);

    //
    // STAT_LED_R_PIN is the LED STATUS pin.
    //
    GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(STAT_LED_R_PIN));
    GPIO_setDirectionMode(STAT_LED_R_PIN, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(STAT_LED_R_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(STAT_LED_R_PIN, GPIO_QUAL_ASYNC);
    GPIO_writePin(STAT_LED_R_PIN, STAT_LED_ENABLE);
    GPIO_setMasterCore(STAT_LED_R_PIN, GPIO_CORE_CPU1);

    //
    // CLK_EN_FPGA_PIN is the FPGA Clock Enable pin.
    //
    GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(CLK_EN_FPGA_PIN));
    GPIO_setDirectionMode(CLK_EN_FPGA_PIN, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(CLK_EN_FPGA_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(CLK_EN_FPGA_PIN, GPIO_QUAL_ASYNC);
    GPIO_writePin(CLK_EN_FPGA_PIN, 1);
    GPIO_setMasterCore(CLK_EN_FPGA_PIN, GPIO_CORE_CPU1);



    //
    // FLT1_PIN is the Fault STATUS pin.
    //
    GPIO_setPinConfigInput(FLT1_PIN);

    //
    // FLT2_PIN is the Fault STATUS pin.
    //
    GPIO_setPinConfigInput(FLT2_PIN);

    //
    // FLT3_PIN is the Fault STATUS pin.
    //
    GPIO_setPinConfigInput(FLT3_PIN);

    #if !BOARDCFG_USE_FPGA_SCI_C_RX
    //
    // FLT4_PIN is the Fault STATUS pin.
    //
    GPIO_setPinConfigInput(FLT4_PIN);
    #endif

    #if !BOARDCFG_USE_FPGA_SCI_C_TX
    //
    // FLT5_PIN is the Fault STATUS pin.
    //
    GPIO_setPinConfigInput(FLT5_PIN);
    #endif

    //
    // FLT6_PIN is the Fault STATUS pin.
    //
    GPIO_setPinConfigInput(FLT6_PIN);


    //
    //  OE_PWM_BUF1..6
    //
    GPIO_setPinConfigOutput(OE_PWM_BUF1);
    GPIO_setPinConfigOutput(OE_PWM_BUF2);
    GPIO_setPinConfigOutput(OE_PWM_BUF3);
    GPIO_setPinConfigOutput(OE_PWM_BUF4);
    GPIO_setPinConfigOutput(OE_PWM_BUF5);
    GPIO_setPinConfigOutput(OE_PWM_BUF6);





    //
    // IOs
    //

    IO_vInit();


    //
    // SD Card
    //
    GPIO_setPadConfig(SDC_RSV1_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(SDC_RSV1_PIN));
    GPIO_setDirectionMode(SDC_RSV1_PIN, GPIO_DIR_MODE_OUT);
    GPIO_setQualificationMode(SDC_RSV1_PIN, GPIO_QUAL_ASYNC);
    GPIO_writePin(SDC_RSV1_PIN, 0); //??? inactive level set here

    GPIO_setPadConfig(SDC_RSV2_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(SDC_RSV2_PIN));
    GPIO_setDirectionMode(SDC_RSV2_PIN, GPIO_DIR_MODE_OUT);
    GPIO_setQualificationMode(SDC_RSV2_PIN, GPIO_QUAL_ASYNC);
    GPIO_writePin(SDC_RSV2_PIN, 0); //??? inactive level set here

    GPIO_setPadConfig(SDC_WP_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(SDC_WP_PIN));
    GPIO_setDirectionMode(SDC_WP_PIN, GPIO_DIR_MODE_OUT);
    GPIO_setQualificationMode(SDC_WP_PIN, GPIO_QUAL_ASYNC);
    GPIO_writePin(SDC_WP_PIN, 0); //??? inactive level set here

    GPIO_setPadConfig(SDC_DET_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(SDC_DET_PIN));
    GPIO_setDirectionMode(SDC_DET_PIN, GPIO_DIR_MODE_IN);
    GPIO_setQualificationMode(SDC_DET_PIN, GPIO_QUAL_ASYNC);

#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_SMALL_TEST

    //
    // CHANGE XBAR inputs from using GPIO0
    // if EPWM SYNCIN is enabled, EXTSYNCIN1 and EXTSYNCIN2 will use
    // GPIO0 (which is the output of EPWM1).
    // Pick any unused GPIO !!!
    //
    //XBAR_setInputPin(XBAR_INPUT5, XBAR_IN_PIN);
    //XBAR_setInputPin(XBAR_INPUT6, XBAR_IN_PIN);




    /* 28075 SCC IO configuration */
    // Outputs
    GPIO_setPadConfig(PRCH_REL_CTRL_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(PRCH_REL_CTRL_PIN));
    GPIO_setDirectionMode(PRCH_REL_CTRL_PIN, GPIO_DIR_MODE_OUT);
    GPIO_setQualificationMode(PRCH_REL_CTRL_PIN, GPIO_QUAL_ASYNC);
    GPIO_writePin(PRCH_REL_CTRL_PIN, 0);

    GPIO_setPadConfig(CONTACTOR_CTRL_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(CONTACTOR_CTRL_PIN));
    GPIO_setDirectionMode(CONTACTOR_CTRL_PIN, GPIO_DIR_MODE_OUT);
    GPIO_setQualificationMode(CONTACTOR_CTRL_PIN, GPIO_QUAL_ASYNC);
    GPIO_writePin(CONTACTOR_CTRL_PIN, 0);


    //Inputs
    GPIO_setPadConfig(CONTACTOR_FB_DSP_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(CONTACTOR_FB_DSP_PIN));
    GPIO_setDirectionMode(CONTACTOR_FB_DSP_PIN, GPIO_DIR_MODE_IN);
    GPIO_setQualificationMode(CONTACTOR_FB_DSP_PIN, GPIO_QUAL_ASYNC);



    /* 28075 SCC SPI CSs, WP,HOLD configuration */
    // SPI CSs, WP,HOLD
    GPIO_setPadConfig(CS_NEG_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(CS_NEG_PIN));
    GPIO_setDirectionMode(CS_NEG_PIN, GPIO_DIR_MODE_OUT);
    GPIO_setQualificationMode(CS_NEG_PIN, GPIO_QUAL_ASYNC);
    GPIO_writePin(CS_NEG_PIN, 1);

    GPIO_setPadConfig(CS_POS_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(CS_POS_PIN));
    GPIO_setDirectionMode(CS_POS_PIN, GPIO_DIR_MODE_OUT);
    GPIO_setQualificationMode(CS_POS_PIN, GPIO_QUAL_ASYNC);
    GPIO_writePin(CS_POS_PIN, 1);

    GPIO_setPadConfig(SPI_A_WP_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(SPI_A_WP_PIN));
    GPIO_setDirectionMode(SPI_A_WP_PIN, GPIO_DIR_MODE_OUT);
    GPIO_setQualificationMode(SPI_A_WP_PIN, GPIO_QUAL_ASYNC);
    GPIO_writePin(SPI_A_WP_PIN, 0); //??? inactive level set here

    GPIO_setPadConfig(SPI_A_HOLD_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(SPI_A_HOLD_PIN));
    GPIO_setDirectionMode(SPI_A_HOLD_PIN, GPIO_DIR_MODE_OUT);
    GPIO_setQualificationMode(SPI_A_HOLD_PIN, GPIO_QUAL_ASYNC);
    GPIO_writePin(SPI_A_HOLD_PIN, 0); //??? inactive level set here



    //
    // SPIA and SPIB.
    // External Connections:
    // -GPIO25 and GPIO17 - SPISOMI
    // -GPIO24 and GPIO16 - SPISIMO
    // -GPIO27 and GPIO19 - SPISTE
    // -GPIO26 and GPIO18 - SPICLK
    //

    //
    // GPIO17 is the SPISOMIA.
    //
    GPIO_setMasterCore(17, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_17_SPISOMIA);
    GPIO_setPadConfig(17, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(17, GPIO_QUAL_ASYNC);

    //
    // GPIO16 is the SPISIMOA clock pin.
    //
    GPIO_setMasterCore(16, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_16_SPISIMOA);
    GPIO_setPadConfig(16, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(16, GPIO_QUAL_ASYNC);

    //
    // GPIO19 is the SPISTEA.
    //
    GPIO_setMasterCore(19, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_19_SPISTEA);
    GPIO_setPadConfig(19, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(19, GPIO_QUAL_ASYNC);

    //
    // GPIO18 is the SPICLKA.
    //
    GPIO_setMasterCore(18, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_18_SPICLKA);
    GPIO_setPadConfig(18, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(18, GPIO_QUAL_ASYNC);


    /* 28075 SCC SPIB configuration */
    //
    // GPIO25 is the SPISOMIB.
    //
    GPIO_setMasterCore(25, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_25_SPISOMIB);
    GPIO_setPadConfig(25, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(25, GPIO_QUAL_ASYNC);

    //
    // GPIO24 is the SPISIMOB clock pin.
    //
    GPIO_setMasterCore(24, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_24_SPISIMOB);
    GPIO_setPadConfig(24, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(24, GPIO_QUAL_ASYNC);

    //
    // GPIO27 is the SPISTEB.
    //
    GPIO_setMasterCore(27, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_27_SPISTEB);
    GPIO_setPadConfig(27, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(27, GPIO_QUAL_ASYNC);

    //
    // GPIO26 is the SPICLKB.
    //
    GPIO_setMasterCore(26, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_26_SPICLKB);
    GPIO_setPadConfig(26, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(26, GPIO_QUAL_ASYNC);

#endif
}


