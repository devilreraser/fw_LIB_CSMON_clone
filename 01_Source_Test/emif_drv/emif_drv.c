/* *****************************************************************************
 * File:   emif_drv.c
 * Author: Dimitar Lilov
 *
 * Created on 2020 01 02 20:51
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
#include "emif_drv.h"
#include "device.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */
#define STRING_CONCAT_BASE(_a_, _b_) _a_##_b_
#define STRING_CONCAT(_a_, _b_) STRING_CONCAT_BASE(_a_, _b_)
#define GPIO_PIN_MODE(_pin_, _mode_) STRING_CONCAT(GPIO_, STRING_CONCAT(_pin_, STRING_CONCAT(_, _mode_)))
#define GPIO_PIN_MODE_GPIO(_pin_) STRING_CONCAT(GPIO_, STRING_CONCAT(_pin_, STRING_CONCAT(_GPIO, _pin_)))

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
#if 0       /* !!! used from FreeMasterUtil to see this structure - do NOT DELETE !!! */
typedef struct
{
    uint32_t rSetup;            //!< Read Setup Cycles
    uint32_t rStrobe;           //!< Read Strobe Cycles
    uint32_t rHold;             //!< Read Hold Cycles
    uint32_t wSetup;            //!< Write Setup Cycles
    uint32_t wStrobe;           //!< Write Strobe Cycles
    uint32_t wHold;             //!< Write Hold Cycles
    uint32_t turnArnd;          //!< TurnAround Cycles
}EMIF_AsyncTimingParams;
#endif



/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */

EMIF_AsyncTimingParams sEMIFParam =
{
    0,      //        uint32_t rSetup;            //!< Read Setup Cycles
    7,      //        uint32_t rStrobe;           //!< Read Strobe Cycles (tested upto 85 deg ambient 82 degrees MRAM Package Cool point)
    0,      //        uint32_t rHold;             //!< Read Hold Cycles
    0,      //        uint32_t wSetup;            //!< Write Setup Cycles
    1,      //        uint32_t wStrobe;           //!< Write Strobe Cycles
    0,      //        uint32_t wHold;             //!< Write Hold Cycles
    0,      //        uint32_t turnArnd;          //!< TurnAround Cycles
};

EMIF_AsyncDataWidth eAsyncDataWidth = EMIF_ASYNC_DATA_WIDTH;
uint16_t u16UseA19AsBA1 = USE_A19_AS_BA1;


/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */
void setupEMIF1PinmuxAsync32BitBoardSCC(uint16_t u16UseA19AsBA1);


/* *****************************************************************************
 * Functions
 **************************************************************************** */



//
// EMIF_vInit
//
void EMIF_vInit(void)
{
    //EMIF_AsyncTimingParams tparam;
    //
    // Configure to run EMIF1 on full Rate. (EMIF1CLK = CPU1SYSCLK)
    //
    SysCtl_setEMIF1ClockDivider(SYSCTL_EMIF1CLK_DIV_1);

    //
    // Grab EMIF1 For CPU1.
    //
    EMIF_selectMaster(EMIF1CONFIG_BASE, EMIF_MASTER_CPU1_G);

    //
    // Disable Access Protection. (CPU_FETCH/CPU_WR/DMA_WR)
    //
    EMIF_setAccessProtection(EMIF1CONFIG_BASE, 0x0);

    //
    // Commit the configuration related to protection. Till this bit remains
    // set, contents of EMIF1ACCPROT0 register can't be changed.
    //
    EMIF_commitAccessConfig(EMIF1CONFIG_BASE);

    //
    // Lock the configuration so that EMIF1COMMIT register can't be changed
    // any more.
    //
    EMIF_lockAccessConfig(EMIF1CONFIG_BASE);

    //
    // Configure GPIO pins for EMIF1.
    //
    setupEMIF1PinmuxAsync32BitBoardSCC(u16UseA19AsBA1);

    //
    // Configures Normal Asynchronous Mode of Operation.
    //
    EMIF_setAsyncMode(EMIF1_BASE, EMIF_ASYNC_CS2_OFFSET,
                      EMIF_ASYNC_NORMAL_MODE);

    //
    // Disables Extended Wait Mode.
    //
    EMIF_disableAsyncExtendedWait(EMIF1_BASE, EMIF_ASYNC_CS2_OFFSET);

    //
    // Configure EMIF1 Data Bus Width.
    //
    EMIF_setAsyncDataBusWidth(EMIF1_BASE, EMIF_ASYNC_CS2_OFFSET, eAsyncDataWidth);
    EMIF_setAsyncTimingParams(EMIF1_BASE, EMIF_ASYNC_CS2_OFFSET, &sEMIFParam);
}


void setupEMIF1PinmuxAsync32BitBoardSCC(uint16_t u16UseA19AsBA1)
{
    uint16_t i;

    //GPIO_setPinConfig(GPIO_28_EM1CS4N);
    //GPIO_setPinConfig(GPIO_29_EM1SDCKE);
    //GPIO_setPinConfig(GPIO_30_EM1CLK);



    GPIO_setPinConfig(GPIO_31_EM1WEN);      //WE




    //GPIO_setPinConfig(GPIO_32_EM1CS0N);
    //GPIO_setPinConfig(GPIO_33_EM1RNW);


#if 1
    GPIO_setPinConfig(GPIO_34_EM1CS2N);     //CE
#else
    GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(34));
    GPIO_setDirectionMode(34, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(34, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(34, GPIO_QUAL_ASYNC);
    GPIO_writePin(34,0);
#endif


#if 0
    GPIO_setPinConfig(GPIO_35_EM1CS3N);     //LUB
#else
    GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(35));
    GPIO_setDirectionMode(35, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(35, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(35, GPIO_QUAL_ASYNC);
    GPIO_writePin(35,0);
#endif




    //GPIO_setPinConfig(GPIO_36_EM1WAIT1);



#if 0
    GPIO_setPinConfig(GPIO_37_EM1OEN);      //OE
#else
    GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(37));
    GPIO_setDirectionMode(37, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(37, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(37, GPIO_QUAL_ASYNC);
    GPIO_writePin(37,0);
#endif

    //
    // Selecting address lines.
    //
    GPIO_setPinConfig(GPIO_38_EM1A0);
    GPIO_setPinConfig(GPIO_39_EM1A1);
    GPIO_setPinConfig(GPIO_40_EM1A2);
    GPIO_setPinConfig(GPIO_41_EM1A3);
    GPIO_setPinConfig(GPIO_44_EM1A4);
    GPIO_setPinConfig(GPIO_45_EM1A5);
    GPIO_setPinConfig(GPIO_46_EM1A6);
    GPIO_setPinConfig(GPIO_47_EM1A7);
    GPIO_setPinConfig(GPIO_48_EM1A8);
    GPIO_setPinConfig(GPIO_49_EM1A9);
    GPIO_setPinConfig(GPIO_50_EM1A10);
    GPIO_setPinConfig(GPIO_51_EM1A11);
    GPIO_setPinConfig(GPIO_52_EM1A12);
    GPIO_setPinConfig(GPIO_86_EM1A13);
    GPIO_setPinConfig(GPIO_87_EM1A14);

    //
    // Selecting data lines.
    //
    GPIO_setPinConfig(GPIO_53_EM1D31);
    GPIO_setPinConfig(GPIO_54_EM1D30);
    GPIO_setPinConfig(GPIO_55_EM1D29);
    GPIO_setPinConfig(GPIO_56_EM1D28);
    GPIO_setPinConfig(GPIO_57_EM1D27);
    GPIO_setPinConfig(GPIO_58_EM1D26);
    GPIO_setPinConfig(GPIO_59_EM1D25);
    GPIO_setPinConfig(GPIO_60_EM1D24);
    GPIO_setPinConfig(GPIO_61_EM1D23);
    GPIO_setPinConfig(GPIO_62_EM1D22);
    GPIO_setPinConfig(GPIO_63_EM1D21);
    GPIO_setPinConfig(GPIO_64_EM1D20);
    GPIO_setPinConfig(GPIO_65_EM1D19);
    GPIO_setPinConfig(GPIO_66_EM1D18);
    GPIO_setPinConfig(GPIO_67_EM1D17);
    GPIO_setPinConfig(GPIO_68_EM1D16);
    GPIO_setPinConfig(GPIO_69_EM1D15);
    GPIO_setPinConfig(GPIO_70_EM1D14);
    GPIO_setPinConfig(GPIO_71_EM1D13);
    GPIO_setPinConfig(GPIO_72_EM1D12);
    GPIO_setPinConfig(GPIO_73_EM1D11);
    GPIO_setPinConfig(GPIO_74_EM1D10);
    GPIO_setPinConfig(GPIO_75_EM1D9);
    GPIO_setPinConfig(GPIO_76_EM1D8);
    GPIO_setPinConfig(GPIO_77_EM1D7);
    GPIO_setPinConfig(GPIO_78_EM1D6);
    GPIO_setPinConfig(GPIO_79_EM1D5);
    GPIO_setPinConfig(GPIO_80_EM1D4);
    GPIO_setPinConfig(GPIO_81_EM1D3);
    GPIO_setPinConfig(GPIO_82_EM1D2);
    GPIO_setPinConfig(GPIO_83_EM1D1);
    GPIO_setPinConfig(GPIO_85_EM1D0);

    //
    // Setting DQM and Bank Select lines.
    //
    //GPIO_setPinConfig(GPIO_88_EM1DQM0);
    //GPIO_setPinConfig(GPIO_89_EM1DQM1);
    //GPIO_setPinConfig(GPIO_90_EM1DQM2);
    //GPIO_setPinConfig(GPIO_91_EM1DQM3);
    if (u16UseA19AsBA1 > 0)
    {
    GPIO_setPinConfig(GPIO_92_EM1BA1);
    }
    //GPIO_setPinConfig(GPIO_93_EM1BA0);
    //GPIO_setPinConfig(GPIO_94_EM1A21);

    GPIO_setPinConfig(GPIO_88_EM1A15);
    GPIO_setPinConfig(GPIO_89_EM1A16);
    GPIO_setPinConfig(GPIO_90_EM1A17);
    GPIO_setPinConfig(GPIO_91_EM1A18);
    if (u16UseA19AsBA1 == 0)
    {
    GPIO_setPinConfig(GPIO_92_EM1A19);
    }
    //GPIO_setPinConfig(GPIO_93_EM1BA0);
    //GPIO_setPinConfig(GPIO_94_EM1A21);



    //
    // Setup Async Mode for Data pins.
    //
    for(i=53; i<=85;i++)
    {
        if(i != 84)
        {
            GPIO_setPadConfig(i, GPIO_PIN_TYPE_PULLUP);
            GPIO_setQualificationMode(i, GPIO_QUAL_ASYNC);
        }
    }
}

