/* *****************************************************************************
 * File:   hmdrv.c
 * Author: Dimitar Lilov
 *
 * Created on 2019 01 02 19:56
 * 
 * Description: Hex Monitor Library MCU and Platform Dependent Driver
 * 
 **************************************************************************** */

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "hmdrv.h"

#if HMDRV_USE_EXT_MEM
#if HMDRV_USE_EXT_SPI
#include "spimem.h"
#endif
#if HMDRV_USE_EXT_PMP
#include "pmpmem.h"
#endif
#if HMDRV_USE_EXT_I2C
#include "i2cmem.h"
#endif
#endif


#if (!HMDRV_DISABLE)


/* *****************************************************************************
 * Variables Definitions - mainly for Debug
 **************************************************************************** */
//volatile HMDRV_U8*   HM_pCopyToSrc = 0;
//volatile HMDRV_U8*   HM_pCopyToDest = 0;
//volatile HMDRV_SIZE8 HM_pCopyToSize = 0;

//volatile HMDRV_U8*   HM_pCopyFromSrc  = 0;
//volatile HMDRV_U8*   HM_pCopyFromDest = 0;
//volatile HMDRV_SIZE8 HM_pCopyFromSize = 0;

uint32_t u32DeviceLSPCLKFreq = DEVICE_LSPCLK_FREQ;
uint32_t u32DeviceLSPCLKFreqRoundAndMinErrorBaud = DEVICE_LSPCLK_FREQ;

uint32_t HMDRV_u32SCIErrorPPM = 0;

uint32_t u32DeviceSYSCLKFreq = DEVICE_SYSCLK_FREQ;

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
#if HMDRV_USE_EXT_MEM
HMDRV_ADDR HMDRV_pGetInternalMemoryAddressWrite(HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize, HMDRV_U16* pu16ID);
HMDRV_ADDR HMDRV_pGetInternalMemoryAddressRead(HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize, HMDRV_ADDR_PNTR_MODIFIER HMDRV_SIZE8* pnSizeRead, HMDRV_U16* pu16ID);
void ProcessExternalMemory(HMDRV_ADDR nInternalRamBufferAddr, HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize, HMDRV_U16* pu16ID);
#endif

#if HMDRV_USE_UART_INIT
uint32_t HMDRV_u32GetSCIErrorPPM(void)
{
    return HMDRV_u32SCIErrorPPM;
}
#endif

void HMDRV_vDebugSend(void)
{
    if (HMDRV_EMPTY_TX(0))
    {
        HMDRV_SCI_PUTCHAR(0,0xAA);
    }
}



#if 1   /* was HMDRV_USE_UART_INIT */


#if ((HMDRV_USE_CONTROL_SUITE_LIB) || (HMDRV_USE_C2000_WARE_LIB && HMDRV_USE_C2000_WARE_LIB_DEVICE_SUPPORT))

#define CPU_FREQ    90000000.0        /* 90 MHz */
#define LSPCLK_FREQ (CPU_FREQ)
#define SCI_FREQ    HMDRV_SCI_BAUD
#define SCI_PRD     ((LSPCLK_FREQ/(SCI_FREQ*8))-1)

#if HMDRV_USE_SCIA
//
// scia_echoback_init - Test 1,SCIA  DLB, 8-bit word, baud rate 0x0103,
// default, 1 STOP bit, no parity
//
void
scia_echoback_init()
{
    //
    // Note: Clocks were turned on to the SCIA peripheral
    // in the InitSysCtrl() function
    //

    //
    // 1 stop bit,  No loopback, No parity,8 char bits, async mode,
    // idle-line protocol
    //
    SciaRegs.SCICCR.all =0x0007;

    //
    // enable TX, RX, internal SCICLK, Disable RX ERR, SLEEP, TXWAKE
    //
    SciaRegs.SCICTL1.all =0x0003;

    SciaRegs.SCICTL2.bit.TXINTENA = 0;
    SciaRegs.SCICTL2.bit.RXBKINTENA = 0;

    SciaRegs.SCIHBAUD = ((Uint16)SCI_PRD) >> 8;
    SciaRegs.SCILBAUD = SCI_PRD;

    SciaRegs.SCICTL1.all =0x0023;  // Relinquish SCI from Reset
}

//
// scia_fifo_init - Initalize the SCI FIFO
//
void
scia_fifo_init()
{
    SciaRegs.SCIFFTX.all=0xE040;
    SciaRegs.SCIFFRX.all=0x2044;
    SciaRegs.SCIFFCT.all=0x0;
}
#endif

#if HMDRV_USE_SCIB
//
// scib_echoback_init - Test 1,Scib  DLB, 8-bit word, baud rate 0x0103,
// default, 1 STOP bit, no parity
//
void
scib_echoback_init()
{
    //
    // Note: Clocks were turned on to the Scib peripheral
    // in the InitSysCtrl() function
    //

    //
    // 1 stop bit,  No loopback, No parity,8 char bits, async mode,
    // idle-line protocol
    //
    ScibRegs.SCICCR.all =0x0007;

    //
    // enable TX, RX, internal SCICLK, Disable RX ERR, SLEEP, TXWAKE
    //
    ScibRegs.SCICTL1.all =0x0003;

    ScibRegs.SCICTL2.bit.TXINTENA = 0;
    ScibRegs.SCICTL2.bit.RXBKINTENA = 0;

    ScibRegs.SCIHBAUD = ((Uint16)SCI_PRD) >> 8;
    ScibRegs.SCILBAUD = SCI_PRD;

    ScibRegs.SCICTL1.all =0x0023;  // Relinquish SCI from Reset
}

//
// scib_fifo_init - Initalize the SCI FIFO
//
void
scib_fifo_init()
{
    ScibRegs.SCIFFTX.all=0xE040;
    ScibRegs.SCIFFRX.all=0x2044;
    ScibRegs.SCIFFCT.all=0x0;
}
#endif

#endif  //#if ((HMDRV_USE_CONTROL_SUITE_LIB) || (HMDRV_USE_C2000_WARE_LIB && HMDRV_USE_C2000_WARE_LIB_DEVICE_SUPPORT))





/* *****************************************************************************
 * HMDRV_vInit
 *
 * Description: init function
 **************************************************************************** */
void HMDRV_vInit(void)
{


    //Interrupt_register(INT_TIMER0,  &TMR0_ISR);
#if BOARDCFG_FREE_RUN_TIMER_INTERNAL
    Interrupt_register(INT_TIMER1,  &TMR1_ISR);
#endif
    Interrupt_register(INT_TIMER2,  &TMR2_ISR);

#if HMDRV_USE_SCIA == 1
    Interrupt_register(INT_SCIA_RX, &SCIA_RXFIFOISR);
    Interrupt_register(INT_SCIA_TX, &SCIA_TXFIFOISR);
#endif
#if HMDRV_USE_SCIB == 1
    Interrupt_register(INT_SCIB_RX, &SCIB_RXFIFOISR);
    Interrupt_register(INT_SCIB_TX, &SCIB_TXFIFOISR);
#endif

    //
    // TMR
    //
    TMR_vInit();


    // Debug Pin Amber LED (middle Led)
#if HMDRV_USE_DEBUG_PIN
    GPIO_setPinConfigOutput(HMDRV_DEBUG_PIN);
#endif



#if ((HMDRV_USE_CONTROL_SUITE_LIB) || (HMDRV_USE_C2000_WARE_LIB && HMDRV_USE_C2000_WARE_LIB_DEVICE_SUPPORT))
#define STRING_CONCAT_BASE(_a_, _b_) _a_##_b_
#define STRING_CONCAT(_a_, _b_) STRING_CONCAT_BASE(_a_, _b_)
#define GPIO_PIN_MODE(_pin_, _mode_) STRING_CONCAT(GPIO_, STRING_CONCAT(_pin_, STRING_CONCAT(_, _mode_)))
#define GPIO_PIN_MODE_GPIO(_pin_) STRING_CONCAT(GPIO_, STRING_CONCAT(_pin_, STRING_CONCAT(_GPIO, _pin_)))
#endif



#ifdef HMDRV_USE_BOARDSCC
	#if BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC
		#if SCI_A == USB_UART
			#define SCIRXDA_PIN 43
    		#define SCITXDA_PIN 42
		#elif SCI_A == RS_485
			//#define SCITXDA_PIN 135
			//#define SCIRXDA_PIN 136
			//#define SCIREA_PIN  144
			//#define SCIDEA_PIN  143
		#endif
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

    #elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_SMALL_TEST
		#define SCIRXDA_PIN 28
		#define SCITXDA_PIN 29

    #endif

#elif defined(HMDRV_USE_BOARDLAUNCHPADXLTMS320F28069M)

    #if ((HMDRV_USE_CONTROL_SUITE_LIB) || (HMDRV_USE_C2000_WARE_LIB && HMDRV_USE_C2000_WARE_LIB_DEVICE_SUPPORT))
    #else
    /* not used by device support */
    #define SCITXDA_PIN 29
    #define SCIRXDA_PIN 28
    //#define SCIREA_PIN  ???
    //#define SCIDEA_PIN  ???


    #define SCITXDB_PIN 58
    #define SCIRXDB_PIN 15
    //#define SCIREB_PIN  ???
    //#define SCIDEB_PIN  ???
    #endif


#elif defined(HMDRV_USE_BOARDLAUNCHPADXLTMS320F28379D)

    #if ((HMDRV_USE_CONTROL_SUITE_LIB) || (HMDRV_USE_C2000_WARE_LIB && HMDRV_USE_C2000_WARE_LIB_DEVICE_SUPPORT))
    #else
    /* not used by device support */
    #define SCITXDA_PIN 42
    #define SCIRXDA_PIN 43
    //#define SCIREA_PIN  ???
    //#define SCIDEA_PIN  ???


    //#define SCITXDB_PIN ???
    //#define SCIRXDB_PIN ???
    //#define SCIREB_PIN  ???
    //#define SCIDEB_PIN  ???
    #endif


#endif  //#ifdef HMDRV_USE_BOARDSCC





#if ((HMDRV_USE_CONTROL_SUITE_LIB) || (HMDRV_USE_C2000_WARE_LIB && HMDRV_USE_C2000_WARE_LIB_DEVICE_SUPPORT))

    #if HMDRV_USE_SCIA


    //InitSciaGpio(); - same as code below

    EALLOW;

 /* Enable internal pull-up for the selected pins */
 // Pull-ups can be enabled or disabled disabled by the user.
 // This will enable the pullups for the specified pins.

     GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;    // Enable pull-up for GPIO28 (SCIRXDA)
 //  GpioCtrlRegs.GPAPUD.bit.GPIO7 = 0;     // Enable pull-up for GPIO7  (SCIRXDA)

     GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0;    // Enable pull-up for GPIO29 (SCITXDA)
 //  GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0;    // Enable pull-up for GPIO12 (SCITXDA)

 /* Set qualification for selected pins to asynch only */
 // Inputs are synchronized to SYSCLKOUT by default.
 // This will select asynch (no qualification) for the selected pins.

     GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 3;  // Asynch input GPIO28 (SCIRXDA)
 //  GpioCtrlRegs.GPAQSEL1.bit.GPIO7 = 3;   // Asynch input GPIO7 (SCIRXDA)

 /* Configure SCI-A pins using GPIO regs*/
 // This specifies which of the possible GPIO pins will be SCI functional pins.

     GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1;   // Configure GPIO28 for SCIRXDA operation
 //  GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 2;    // Configure GPIO7  for SCIRXDA operation

     GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1;   // Configure GPIO29 for SCITXDA operation
 //  GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 2;   // Configure GPIO12 for SCITXDA operation

     EDIS;

     #endif
     #if HMDRV_USE_SCIB

     //InitScibGpio(); - same as code below fixed

       EALLOW;

    /* Enable internal pull-up for the selected pins */
    // Pull-ups can be enabled or disabled disabled by the user.
    // This will enable the pullups for the specified pins.

    //  GpioCtrlRegs.GPAPUD.bit.GPIO11 = 0;    // Enable pull-up for GPIO11 (SCIRXDB)
        GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0;    // Enable pull-up for GPIO15 (SCIRXDB)
    //  GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;    // Enable pull-up for GPIO19 (SCIRXDB)
    //  GpioCtrlRegs.GPAPUD.bit.GPIO23 = 0;    // Enable pull-up for GPIO23 (SCIRXDB)
    //  GpioCtrlRegs.GPBPUD.bit.GPIO41 = 0;    // Enable pull-up for GPIO41 (SCIRXDB)
    //  GpioCtrlRegs.GPBPUD.bit.GPIO44 = 0;    // Enable pull-up for GPIO44 (SCIRXDB)

    //  GpioCtrlRegs.GPAPUD.bit.GPIO9 = 0;     // Enable pull-up for GPIO9 (SCITXDB)
    //  GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0;    // Enable pull-up for GPIO14 (SCITXDB)
    //  GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;    // Enable pull-up for GPIO18 (SCITXDB)
    //  GpioCtrlRegs.GPAPUD.bit.GPIO22 = 0;    // Enable pull-up for GPIO22 (SCITXDB)
    //  GpioCtrlRegs.GPBPUD.bit.GPIO40 = 0;    // Enable pull-up for GPIO40 (SCITXDB)
        GpioCtrlRegs.GPBPUD.bit.GPIO58 = 0;    // Enable pull-up for GPIO58 (SCITXDB)

    /* Set qualification for selected pins to asynch only */
    // Inputs are synchronized to SYSCLKOUT by default.
    // This will select asynch (no qualification) for the selected pins.

    //  GpioCtrlRegs.GPAQSEL1.bit.GPIO11 = 3;  // Asynch input GPIO11 (SCIRXDB)
        GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 3;  // Asynch input GPIO15 (SCIRXDB)
    //  GpioCtrlRegs.GPAQSEL2.bit.GPIO19 = 3;  // Asynch input GPIO19 (SCIRXDB)
    //  GpioCtrlRegs.GPAQSEL2.bit.GPIO23 = 3;  // Asynch input GPIO23 (SCIRXDB)
    //  GpioCtrlRegs.GPBQSEL1.bit.GPIO41 = 3;  // Asynch input GPIO41 (SCIRXDB)
    //  GpioCtrlRegs.GPBQSEL1.bit.GPIO44 = 3;  // Asynch input GPIO44 (SCIRXDB)


    /* Configure SCI-B pins using GPIO regs*/
    // This specifies which of the possible GPIO pins will be SCI functional pins.

    //  GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 2;   // Configure GPIO11 for SCIRXDB operation
        GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 2;   // Configure GPIO15 for SCIRXDB operation
    //  GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 2;   // Configure GPIO19 for SCIRXDB operation
    //  GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 3;   // Configure GPIO23 for SCIRXDB operation
    //  GpioCtrlRegs.GPBMUX1.bit.GPIO41 = 2;   // Configure GPIO41 for SCIRXDB operation
    //  GpioCtrlRegs.GPBMUX1.bit.GPIO44 = 2;   // Configure GPIO44 for SCIRXDB operation


    //  GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 2;   // Configure GPIO9 for SCITXDB operation
    //  GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 2;   // Configure GPIO14 for SCITXDB operation
    //  GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 2;   // Configure GPIO18 for SCITXDB operation
    //  GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 3;   // Configure GPIO22 for SCITXDB operation
    //  GpioCtrlRegs.GPBMUX1.bit.GPIO40 = 2;   // Configure GPIO40 for SCITXDB operation
        GpioCtrlRegs.GPBMUX2.bit.GPIO58 = 2;   // Configure GPIO58 for SCITXDB operation


        EDIS;


     #endif



#else   //#if ((HMDRV_USE_CONTROL_SUITE_LIB) || (HMDRV_USE_C2000_WARE_LIB && HMDRV_USE_C2000_WARE_LIB_DEVICE_SUPPORT))



  #if HMDRV_USE_SCIA

    //
    // SCIRXDA_PIN is the SCI Rx pin.
    //
    GPIO_setPinConfig(GPIO_PIN_MODE(SCIRXDA_PIN, SCIRXDA));
    GPIO_setDirectionMode(SCIRXDA_PIN, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(SCIRXDA_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SCIRXDA_PIN, GPIO_QUAL_ASYNC);
    GPIO_setMasterCore(SCIRXDA_PIN, GPIO_CORE_CPU1);

    //
    // SCITXDA_PIN is the SCI Tx pin.
    //
    GPIO_setPinConfig(GPIO_PIN_MODE(SCITXDA_PIN, SCITXDA));
    GPIO_setDirectionMode(SCITXDA_PIN, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(SCITXDA_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SCITXDA_PIN, GPIO_QUAL_ASYNC);
    GPIO_setMasterCore(SCITXDA_PIN, GPIO_CORE_CPU1);

    #ifdef HMDRV_USE_BOARDSCC
    #ifdef SCIREA_PIN
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

      GPIO_setMasterCore(SCIREA_PIN, GPIO_CORE_CPU1);
    #endif
    #ifdef SCIDEA_PIN
      //
      // SCIDEA_PIN is the SCI DE pin.
      //
      GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(SCIDEA_PIN));
      GPIO_setDirectionMode(SCIDEA_PIN, GPIO_DIR_MODE_OUT);
      GPIO_setPadConfig(SCIDEA_PIN, GPIO_PIN_TYPE_STD);
      GPIO_setQualificationMode(SCIDEA_PIN, GPIO_QUAL_ASYNC);
      GPIO_writePin(SCIDEA_PIN, SCIDE_DISABLE);
      GPIO_setMasterCore(SCIDEA_PIN, GPIO_CORE_CPU1);
    #endif
    #endif



  #endif
  #if HMDRV_USE_SCIB

      //
      // SCIRXDB_PIN is the SCI Rx pin.
      //
      GPIO_setPinConfig(GPIO_PIN_MODE(SCIRXDB_PIN, SCIRXDB));
      GPIO_setDirectionMode(SCIRXDB_PIN, GPIO_DIR_MODE_IN);
      GPIO_setPadConfig(SCIRXDB_PIN, GPIO_PIN_TYPE_STD);
      GPIO_setQualificationMode(SCIRXDB_PIN, GPIO_QUAL_ASYNC);
      GPIO_setMasterCore(SCIRXDB_PIN, GPIO_CORE_CPU1);

      //
      // SCITXDB_PIN is the SCI Tx pin.
      //
      GPIO_setPinConfig(GPIO_PIN_MODE(SCITXDB_PIN, SCITXDB));
      GPIO_setDirectionMode(SCITXDB_PIN, GPIO_DIR_MODE_OUT);
      GPIO_setPadConfig(SCITXDB_PIN, GPIO_PIN_TYPE_STD);
      GPIO_setQualificationMode(SCITXDB_PIN, GPIO_QUAL_ASYNC);
      GPIO_setMasterCore(SCITXDB_PIN, GPIO_CORE_CPU1);

    #ifdef HMDRV_USE_BOARDSCC
    #ifdef SCIREB_PIN
      //
      // SCIREB_PIN is the SCI RE pin.
      //
      GPIO_setPinConfig(GPIO_PIN_MODE_GPIO(SCIREB_PIN));
      GPIO_setDirectionMode(SCIREB_PIN, GPIO_DIR_MODE_OUT);
      GPIO_setPadConfig(SCIREB_PIN, GPIO_PIN_TYPE_STD);
      GPIO_setQualificationMode(SCIREB_PIN, GPIO_QUAL_ASYNC);
      GPIO_writePin(SCIREB_PIN, SCIRE_ENABLE);
      GPIO_setMasterCore(SCIREB_PIN, GPIO_CORE_CPUA);

    #endif
    #ifdef SCIDEB_PIN
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
  #endif


#endif  //#if ((HMDRV_USE_CONTROL_SUITE_LIB) || (HMDRV_USE_C2000_WARE_LIB && HMDRV_USE_C2000_WARE_LIB_DEVICE_SUPPORT))






#if ((HMDRV_USE_CONTROL_SUITE_LIB) || (HMDRV_USE_C2000_WARE_LIB && HMDRV_USE_C2000_WARE_LIB_DEVICE_SUPPORT))

#if HMDRV_USE_SCIA
  scia_fifo_init();      // Initialize the SCI FIFO
  scia_echoback_init();  // Initalize SCI for echoback
  //HMDRV_SCI_APPLICATION_INIT(UART_1);
#endif
#if HMDRV_USE_SCIB
  scib_fifo_init();      // Initialize the SCI FIFO
  scib_echoback_init();  // Initalize SCI for echoback
  //HMDRV_SCI_APPLICATION_INIT(UART_2);
#endif

#else

  //
  // Get DEVICE_SYSCLK_FREQ
  //
  u32DeviceSYSCLKFreq = SysCtl_getClock(DEVICE_OSCSRC_FREQ);

  //
  // Get DEVICE_LSPCLK_FREQ
  //
  u32DeviceLSPCLKFreq = SysCtl_getLowSpeedClock(DEVICE_OSCSRC_FREQ);




  uint32_t divider;
  uint32_t divider_mult_1000;
  uint32_t divider_mult_1000_reminder;
  uint32_t baud = HMDRV_SCI_BAUD;

  divider_mult_1000 = ((u32DeviceLSPCLKFreq*10)  / (baud * 8U/100));
  divider_mult_1000_reminder = divider_mult_1000 % 1000;
  divider = u32DeviceLSPCLKFreq / (baud * 8U);
  HMDRV_u32SCIErrorPPM = (divider_mult_1000_reminder * 1000) / divider;


  u32DeviceLSPCLKFreqRoundAndMinErrorBaud += (baud * (8U/2U));  /* Fix Rounding (LSPCLKFreq / Baud * 8) For Minimum Error */



  //
  // Initialize SCI and its FIFO.
  //



  HWREGH(HMDRV_SCI_BASE_HEXMON + SCI_O_CCR) = 0;   /* explicitly clear ADDRIDLE_MODE (in SCI_setConfig not cleared) */

  SCI_setConfig(HMDRV_SCI_BASE_HEXMON, u32DeviceLSPCLKFreqRoundAndMinErrorBaud, HMDRV_HEXMON_BAUD,
                (SCI_CONFIG_WLEN_8 | HMDRV_STOP_BITS | HMDRV_PARITY_HEXMON));    /* By Default Get HexMon Settings where not globals */
  SCI_enableModule(HMDRV_SCI_BASE_HEXMON);
  SCI_disableLoopback(HMDRV_SCI_BASE_HEXMON);
  SCI_resetChannels(HMDRV_SCI_BASE_HEXMON);
  SCI_enableFIFO(HMDRV_SCI_BASE_HEXMON);

#if  HMDRV_SCI_RX_ISR
  SCI_enableInterrupt(HMDRV_SCI_BASE_HEXMON, SCI_INT_RXFF);
#else
  SCI_disableInterrupt(HMDRV_SCI_BASE_HEXMON, SCI_INT_RXFF);
#endif

#if  HMDRV_SCI_TX_ISR
  SCI_enableInterrupt(HMDRV_SCI_BASE_HEXMON, SCI_INT_TXFF);
#else
  SCI_disableInterrupt(HMDRV_SCI_BASE_HEXMON, SCI_INT_TXFF);
#endif
  SCI_disableInterrupt(HMDRV_SCI_BASE_HEXMON, SCI_INT_RXERR);

  SCI_setFIFOInterruptLevel(HMDRV_SCI_BASE_HEXMON, SCI_FIFO_TX1, SCI_FIFO_RX1);
  SCI_performSoftwareReset(HMDRV_SCI_BASE_HEXMON);

  SCI_resetTxFIFO(HMDRV_SCI_BASE_HEXMON);
  SCI_resetRxFIFO(HMDRV_SCI_BASE_HEXMON);

#if (HMLIB_HEXMON_UART_MODULE != HMLIB_MODBUS_UART_MODULE)

  HWREGH(HMDRV_SCI_BASE_MODBUS + SCI_O_CCR) = 0;   /* explicitly clear ADDRIDLE_MODE (in SCI_setConfig not cleared) */

  SCI_setConfig(HMDRV_SCI_BASE_MODBUS, u32DeviceLSPCLKFreqRoundAndMinErrorBaud, HMDRV_MODBUS_BAUD,
                (SCI_CONFIG_WLEN_8 | HMDRV_STOP_BITS | HMDRV_PARITY_MODBUS));
  SCI_enableModule(HMDRV_SCI_BASE_MODBUS);
  SCI_disableLoopback(HMDRV_SCI_BASE_MODBUS);
  SCI_resetChannels(HMDRV_SCI_BASE_MODBUS);
  SCI_enableFIFO(HMDRV_SCI_BASE_MODBUS);

#if  HMDRV_SCI_RX_ISR
  SCI_enableInterrupt(HMDRV_SCI_BASE_MODBUS, SCI_INT_RXFF);
#else
  SCI_disableInterrupt(HMDRV_SCI_BASE_MODBUS, SCI_INT_RXFF);
#endif

#if  HMDRV_SCI_TX_ISR
  SCI_enableInterrupt(HMDRV_SCI_BASE_MODBUS, SCI_INT_TXFF);
#else
  SCI_disableInterrupt(HMDRV_SCI_BASE_MODBUS, SCI_INT_TXFF);
#endif
  SCI_disableInterrupt(HMDRV_SCI_BASE_MODBUS, SCI_INT_RXERR);

  SCI_setFIFOInterruptLevel(HMDRV_SCI_BASE_MODBUS, SCI_FIFO_TX1, SCI_FIFO_RX1);
  SCI_performSoftwareReset(HMDRV_SCI_BASE_MODBUS);

  SCI_resetTxFIFO(HMDRV_SCI_BASE_MODBUS);
  SCI_resetRxFIFO(HMDRV_SCI_BASE_MODBUS);


#endif




#endif  //#if ((HMDRV_USE_CONTROL_SUITE_LIB) || (HMDRV_USE_C2000_WARE_LIB && HMDRV_USE_C2000_WARE_LIB_DEVICE_SUPPORT))


  /* Timer ISR */
    //Interrupt_enable(INT_TIMER0);
#if BOARDCFG_FREE_RUN_TIMER_INTERNAL
    Interrupt_enable(INT_TIMER1);
#endif
    Interrupt_enable(INT_TIMER2);

#if HMDRV_USE_SCIA == 1
    Interrupt_enable(INT_SCIA_RX);
    Interrupt_enable(INT_SCIA_TX);
#endif
#if HMDRV_USE_SCIB == 1
    Interrupt_enable(INT_SCIB_RX);
    Interrupt_enable(INT_SCIB_TX);
#endif



#if (HMLIB_HEXMON_UART_MODULE == UART_A) || (HMLIB_MODBUS_UART_MODULE == UART_A)
        HMDRV_SCI_APPLICATION_INIT(UART_1);
#endif
#if (HMLIB_HEXMON_UART_MODULE == UART_B) || (HMLIB_MODBUS_UART_MODULE == UART_B)
        HMDRV_SCI_APPLICATION_INIT(UART_2);
#endif
#if (HMLIB_HEXMON_UART_MODULE == UART_C) || (HMLIB_MODBUS_UART_MODULE == UART_C)
        HMDRV_SCI_APPLICATION_INIT(UART_3);
#endif
#if (HMLIB_HEXMON_UART_MODULE == UART_D) || (HMLIB_MODBUS_UART_MODULE == UART_D)
        HMDRV_SCI_APPLICATION_INIT(UART_4);
#endif

}


void HMDRV_vProcess(void)
{
    #if ((HMDRV_USE_CONTROL_SUITE_LIB) || (HMDRV_USE_C2000_WARE_LIB && HMDRV_USE_C2000_WARE_LIB_DEVICE_SUPPORT))
        #if HMDRV_USE_SCIA
        /* To clear all errors a sw reset of the module is required */
        if (SciaRegs.SCIRXST.bit.RXERROR)
        {
            SciaRegs.SCICTL1.bit.SWRESET = 0;
            SciaRegs.SCICTL1.bit.SWRESET = 1;
        //HMDRV_SCI_APPLICATION_INIT(UART_1);
        }
        #endif
        #if HMDRV_USE_SCIB
        /* To clear all errors a sw reset of the module is required */
        if (ScibRegs.SCIRXST.bit.RXERROR)
        {
            ScibRegs.SCICTL1.bit.SWRESET = 0;
            ScibRegs.SCICTL1.bit.SWRESET = 1;
        //HMDRV_SCI_APPLICATION_INIT(UART_2);
        }
        #endif
        #error "to do rx fifo reset here"

    #else
        /*  */


        if (((HWREGH(HMDRV_SCI_BASE_HEXMON + SCI_O_RXST) & SCI_RXST_RXERROR) == SCI_RXST_RXERROR)
            || ((HWREGH(HMDRV_SCI_BASE_HEXMON + SCI_O_FFRX) & SCI_FFRX_RXFFOVF) == SCI_FFRX_RXFFOVF))
        {
            SCI_resetRxFIFO(HMDRV_SCI_BASE_HEXMON);
            SCI_performSoftwareReset(HMDRV_SCI_BASE_HEXMON);
            #if 0
            #if HMDRV_SCI_BASE_HEXMON == SCIA_BASE
            HMDRV_SCI_APPLICATION_INIT(UART_1);
            #elif HMDRV_SCI_BASE_HEXMON == SCIB_BASE
            HMDRV_SCI_APPLICATION_INIT(UART_2);
            #elif HMDRV_SCI_BASE_HEXMON == SCIC_BASE
            HMDRV_SCI_APPLICATION_INIT(UART_3);
            #elif HMDRV_SCI_BASE_HEXMON == SCID_BASE
            HMDRV_SCI_APPLICATION_INIT(UART_4);
            #else
            #error "SCI Not Supported"
            #endif
            #endif
        }

        #if (HMLIB_HEXMON_UART_MODULE != HMLIB_MODBUS_UART_MODULE)
        if (((HWREGH(HMDRV_SCI_BASE_MODBUS + SCI_O_RXST) & SCI_RXST_RXERROR) == SCI_RXST_RXERROR)
            || ((HWREGH(HMDRV_SCI_BASE_MODBUS + SCI_O_FFRX) & SCI_FFRX_RXFFOVF) == SCI_FFRX_RXFFOVF))
        {
            SCI_resetRxFIFO(HMDRV_SCI_BASE_MODBUS);
            SCI_performSoftwareReset(HMDRV_SCI_BASE_MODBUS);
            #if 0
            #if HMDRV_SCI_BASE_MODBUS == SCIA_BASE
            HMDRV_SCI_APPLICATION_INIT(UART_1);
            #elif HMDRV_SCI_BASE_MODBUS == SCIB_BASE
            HMDRV_SCI_APPLICATION_INIT(UART_2);
            #elif HMDRV_SCI_BASE_MODBUS == SCIC_BASE
            HMDRV_SCI_APPLICATION_INIT(UART_3);
            #elif HMDRV_SCI_BASE_MODBUS == SCID_BASE
            HMDRV_SCI_APPLICATION_INIT(UART_4);
            #else
            #error "SCI Not Supported"
            #endif
            #endif
        }
        #endif


    #endif  //#if ((HMDRV_USE_CONTROL_SUITE_LIB) || (HMDRV_USE_C2000_WARE_LIB && HMDRV_USE_C2000_WARE_LIB_DEVICE_SUPPORT))


}

#endif  //#if HMDRV_USE_UART_INIT


/* *****************************************************************************
 * HMDRV_CopyMemory
 * 
 * Description: memcpy function
 **************************************************************************** */
void HMDRV_CopyMemory(HMDRV_ADDR nDestAddr, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{
#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY


    HMDRV_U16* ps = (HMDRV_U16*)(((HMDRV_U32)nSrcAddr) >> 1);
    HMDRV_U16* pd = (HMDRV_U16*)(((HMDRV_U32)nDestAddr)>> 1);
    HMDRV_U16 data = 0;
    HMDRV_U8 oddSrc = (HMDRV_U8)((HMDRV_U32)nSrcAddr & 0x01);
    HMDRV_U8 oddDst = (HMDRV_U8)((HMDRV_U32)nDestAddr & 0x01);





    //pCopyToSrc  = ps;
    //pCopyToDest = pd;
    //pCopyToSize = nSize;
    /* FIX INVALID Address Infineon */
    //if (ps == 0) return 0;
    //if (pd == 0) return 0;




    if (oddSrc == 0)
    {
        if (oddDst == 0)
        {
            while(nSize--)
            {
                data = *ps++;

                if (nSize > 0)
                {
                    *pd++ = data;
                    nSize--;
                }
                else
                {
                    *pd &= 0xFF00;
                    *pd |= (data & 0xFF);
                }
            }
        }
        else
        {
            while(nSize--)
            {
                data = *ps++;

                *pd   &= 0x00FF;
                *pd++ |= (data << 8);

                if (nSize--)
                {
                   *pd &= 0xFF00;
                   *pd |= (data >> 8);
                }
                else
                {
                    break;
                }
            }
        }

    }
    else
    {
        if (oddDst == 1)
        {
            data = *ps;
            while(nSize--)
            {
                *pd   &= 0x00FF;
                *pd++ |= (data & 0xFF00);

                *pd++ = data >> 8;
                if (nSize--)
                {
                    data = *ps++;
                    *pd   &= 0xFF00;
                    *pd   |= (data & 0xFF);
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            data = *ps;
            while(nSize--)
            {
                *pd   &= 0xFF00;
                *pd   |= (data >> 8);

                *pd++ = data >> 8;
                if (nSize--)
                {
                    data = *ps++;
                    *pd   &= 0x00FF;
                    *pd++ |= (data << 8);
                }
                else
                {
                    break;
                }
            }

        }

    }
#elif HMDRV_CFG_BUS_WIDTH == 2

    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* ps = (HMDRV_U16*)(HMDRV_ADDR_SIZE)nSrcAddr;
    HMDRV_U16* pd = (HMDRV_U16*)(HMDRV_ADDR_SIZE)nDestAddr;
    HMDRV_U16 data;

    if(nSize == 1)
    {
        nSize = 2;
    }

    while(nSize)
    {
        data = *ps++;
        *pd++ = data;
        nSize-=2;
    }


#else
    HMDRV_U8* ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE) nSrcAddr;
    HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE) nDestAddr;

    while(nSize--)
        *pd++ = *ps++;
#endif  //#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY


}




/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/

HMDRV_BPTR HMDRV_CopyFromBufferInternal(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_U16* pd = (HMDRV_U16*)(((HMDRV_U32)nDestAddr) >> 1);
    HMDRV_U16 data = 0;
    HMDRV_U8 data8 = 0;
    HMDRV_U16 data16 = 0;
    HMDRV_U8 odd = (HMDRV_U8)((HMDRV_U32)nDestAddr & 0x01);

    //pCopyFromSrc  = ps;
    //pCopyFromDest = pd;
    //pCopyFromSize = nSize;

    data = *pd;

    if (odd == 0)
    {
        while(nSize--)
        {
            data = *pd;

            data &= 0xFF00;
            data8 = *ps++;
            data8 &= 0x00FF;
            data += (HMDRV_U16)data8;

            if (nSize--)
            {
                data &= 0x00FF;
                data8 = *ps++;
                data16 = (HMDRV_U16)data8 << 8;
                data += data16;
                *pd++ = data;
            }
            else
            {
                *pd = data;
                break;
            }

        }

    }
    else
    {

        while(nSize--)
        {
            data &= 0x00FF;
            data8 = *ps++;
            data16 = (HMDRV_U16)data8 << 8;
            data += data16;
            *pd++ = data;

            if (nSize--)
            {
                data = *pd;

                data &= 0xFF00;
                data8 = *ps++;
                data8 &= 0x00FF;
                data += (HMDRV_U16)data8;
                if (nSize == 0)
                {
                    *pd = data;
                }
            }
            else
            {
                *pd = data;
                break;
            }


        }

    }
#elif HMDRV_CFG_BUS_WIDTH == 2
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* pd = (HMDRV_U16*)(HMDRV_ADDR_SIZE)nDestAddr;
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_U16 data;
    while(nSize)
    {
        data = *ps++;
        if ((nSize & 1) == 0)
        {
            data += (HMDRV_U16)*ps++ << 8;
            nSize--;
        }
        *pd++ = data;
        nSize--;
    }
#else

    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff; 
    
    nDestAddr = HMRDV_FixEdsAddress(nDestAddr); /* fix U32 eds Address */
    
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;
    
    if (nDestAddr > 0xFFFF)
    {
        /* forbidden psv write - just advance read buffer pointer */
        ps += nSize;
        return (HMDRV_BPTR) ps;
    }
    
    
    HMDRV_SIZE8 nSizeBytes = nSize;      
    while(nSizeBytes > 0)
    {
        *pd++ = *ps++;
        nSizeBytes--;
    }

#endif  //#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY
    return (HMDRV_BPTR) ps;
}


/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/

HMDRV_BPTR HMDRV_CopyFromBufferInternalCoil(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY

    /* ??? Not checked */

    HMDRV_SIZE8 nBitOffset = 0;
    HMDRV_SIZE8 nBitOffsetDest = 0;

    HMDRV_U8 nextByte;
    HMDRV_U8 readBit;


    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    nDestAddr = HMRDV_FixEdsAddress(nDestAddr); /* fix U32 eds Address */
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* pd = (HMDRV_U16*)((HMDRV_ADDR_SIZE)nDestAddr >> 1);

    while(nSize--)
    {
        nextByte = *ps;

        if (nextByte & (1 << nBitOffset))
        {
            readBit = 1;
        }
        else
        {
            readBit = 0;
        }

        if (readBit)
        {
            *pd |= (1 << nBitOffsetDest);
        }
        else
        {
            *pd &= ~(1 << nBitOffsetDest);
        }

        nBitOffsetDest++;
        if (nBitOffsetDest == 0)
        {
            pd++;
        }

        nBitOffset++;
        if (nBitOffset == 0)
        {
            ps++;
        }

    }
    /* fix point to next byte in buffer */
    if (nBitOffset != 0)
    {
        ps++;
    }





#if 0
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_U16* pd = (HMDRV_U16*)(((HMDRV_U32)nDestAddr) >> 1);
    HMDRV_U16 data = 0;
    HMDRV_U8 data8 = 0;
    HMDRV_U16 data16 = 0;
    HMDRV_U8 odd = (HMDRV_U8)((HMDRV_U32)nDestAddr & 0x01);

    //pCopyFromSrc  = ps;
    //pCopyFromDest = pd;
    //pCopyFromSize = nSize;

    data = *pd;

    if (odd == 0)
    {
        while(nSize--)
        {
            data = *pd;

            data &= 0xFF00;
            data8 = *ps++;
            data8 &= 0x00FF;
            data += (HMDRV_U16)data8;

            if (nSize--)
            {
                data &= 0x00FF;
                data8 = *ps++;
                data16 = (HMDRV_U16)data8 << 8;
                data += data16;
                *pd++ = data;
            }
            else
            {
                *pd = data;
                break;
            }

        }

    }
    else
    {

        while(nSize--)
        {
            data &= 0x00FF;
            data8 = *ps++;
            data16 = (HMDRV_U16)data8 << 8;
            data += data16;
            *pd++ = data;

            if (nSize--)
            {
                data = *pd;

                data &= 0xFF00;
                data8 = *ps++;
                data8 &= 0x00FF;
                data += (HMDRV_U16)data8;
                if (nSize == 0)
                {
                    *pd = data;
                }
            }
            else
            {
                *pd = data;
                break;
            }


        }

    }
#endif


#elif HMDRV_CFG_BUS_WIDTH == 2
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* pd = (HMDRV_U16*)(HMDRV_ADDR_SIZE)nDestAddr;
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_U16 data;
    while(nSize)
    {
        data = *ps++;
        if ((nSize & 1) == 0)
        {
            data += (HMDRV_U16)*ps++ << 8;
            nSize--;
        }
        *pd++ = data;
        nSize--;
    }
#else

    


    #if 1

    HMDRV_SIZE8 nBitOffset = 0;
    HMDRV_SIZE8 nBitOffsetDest = 0;
    
    HMDRV_U8 nextByte;
    HMDRV_U8 readBit;
    
    
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff; 
    nDestAddr = HMRDV_FixEdsAddress(nDestAddr); /* fix U32 eds Address */
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;
    if (nDestAddr > 0xFFFF)
    {
        /* forbidden psv write - just advance read buffer pointer */
        ps += ( (nSize + 7) >> 3 );
        return (HMDRV_BPTR) ps;
    }

    while(nSize--)
    {
        nextByte = *ps;

        if (nextByte & (1 << nBitOffset))
        {
            readBit = 1;
        }
        else
        {
            readBit = 0;
        }

    #if 1
        if (readBit)
        {
            *pd |= (1 << nBitOffsetDest);
        }
        else
        {
            *pd &= ~(1 << nBitOffsetDest);
        }
    #else
        if (nBitOffsetDest == 0)
        {
            *pd = readBit;
        }
        else
        {
            *pd |= (readBit << nBitOffsetDest);
        }
    #endif

        nBitOffsetDest++;
        if (nBitOffsetDest == 0)
        {
            pd++;
        }

        nBitOffset++;
        if (nBitOffset == 0)
        {
            ps++;
        }

    }
    /* fix point to next byte in buffer */
    if (nBitOffset != 0)
    {
        ps++;
    }

    
    #else
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff; 
    nDestAddr = HMRDV_FixEdsAddress(nDestAddr); /* fix U32 eds Address */
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;
    if (nDestAddr > 0xFFFF)
    {
        /* forbidden psv write - just advance read buffer pointer */
        ps += nSize;
        return (HMDRV_BPTR) ps;
    }
    HMDRV_SIZE8 nSizeBytes = nSize;      
    while(nSizeBytes > 0)
    {
        *pd++ = *ps++;
        nSizeBytes--;
    }
    #endif

    
    
#endif  //#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY
    return (HMDRV_BPTR) ps;
}


/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/

HMDRV_BPTR HMDRV_CopyFromBufferInternalSingleCoil(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff)
{
#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY

    /* ??? Not checked */

    HMDRV_SIZE8 nBitOffset = 0;
    HMDRV_SIZE8 nBitOffsetDest = 0;

    HMDRV_U8 nextByte;
    HMDRV_U8 readBit;

    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;

    nDestAddr = HMRDV_FixEdsAddress(nDestAddr); /* fix U32 eds Address */

    //HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* pd = (HMDRV_U16*)(HMDRV_ADDR_SIZE)nDestAddr;
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* pd = (HMDRV_U16*)(((HMDRV_U32)nDestAddr) >> 1);

    nextByte = *ps;

    if (nextByte & (1 << nBitOffset))
    {
        readBit = 1;
    }
    else
    {
        readBit = 0;
    }

    if (readBit)
    {
        *pd |= (1 << nBitOffsetDest);
    }
    else
    {
        *pd &= ~(1 << nBitOffsetDest);
    }




//    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;

#if 0       /* needs implementation */
    HMDRV_U16* pd = (HMDRV_U16*)(((HMDRV_U32)nDestAddr) >> 1);
    HMDRV_U16 data = 0;
    HMDRV_U8 data8 = 0;
    HMDRV_U16 data16 = 0;
    HMDRV_U8 odd = (HMDRV_U8)((HMDRV_U32)nDestAddr & 0x01);

    //pCopyFromSrc  = ps;
    //pCopyFromDest = pd;
    //pCopyFromSize = nSize;

    data = *pd;

    if (odd == 0)
    {
        while(nSize--)
        {
            data = *pd;

            data &= 0xFF00;
            data8 = *ps++;
            data8 &= 0x00FF;
            data += (HMDRV_U16)data8;

            if (nSize--)
            {
                data &= 0x00FF;
                data8 = *ps++;
                data16 = (HMDRV_U16)data8 << 8;
                data += data16;
                *pd++ = data;
            }
            else
            {
                *pd = data;
                break;
            }

        }

    }
    else
    {

        while(nSize--)
        {
            data &= 0x00FF;
            data8 = *ps++;
            data16 = (HMDRV_U16)data8 << 8;
            data += data16;
            *pd++ = data;

            if (nSize--)
            {
                data = *pd;

                data &= 0xFF00;
                data8 = *ps++;
                data8 &= 0x00FF;
                data += (HMDRV_U16)data8;
                if (nSize == 0)
                {
                    *pd = data;
                }
            }
            else
            {
                *pd = data;
                break;
            }


        }

    }
#else
    ps++;
#endif  //#if 0


#elif HMDRV_CFG_BUS_WIDTH == 2

    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;

    #if 0       /* needs implementation */
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* pd = (HMDRV_U16*)(HMDRV_ADDR_SIZE)nDestAddr;
    HMDRV_U16 data;
    while(nSize)
    {
        data = *ps++;
        if ((nSize & 1) == 0)
        {
            data += (HMDRV_U16)*ps++ << 8;
            nSize--;
        }
        *pd++ = data;
        nSize--;
    }
    #else
    ps++;
    #endif

#else

    #if 1   //big
    HMDRV_SIZE8 nBitOffset = 0;
    HMDRV_SIZE8 nBitOffsetDest = 0;
    
    HMDRV_U8 nextByte;
    HMDRV_U8 readBit;

    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff; 
    
    nDestAddr = HMRDV_FixEdsAddress(nDestAddr); /* fix U32 eds Address */
    
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;
    
    if (nDestAddr > 0xFFFF)
    {
        /* forbidden psv write - just advance read buffer pointer */
        ps += 1;
        return (HMDRV_BPTR) ps;
    }
    nextByte = *ps;
    
    if (nextByte & (1 << nBitOffset))
    {
        readBit = 1;
    }
    else
    {
        readBit = 0;
    }
    
    #if 1
    if (readBit)
    {
        *pd |= (1 << nBitOffsetDest);
    }
    else
    {
        *pd &= ~(1 << nBitOffsetDest);
    }
    #else
    if (nBitOffsetDest == 0)
    {
        *pd = readBit;
    }
    else
    {
        *pd |= (readBit << nBitOffsetDest);
    }
    #endif
    
    nBitOffsetDest++;
    if (nBitOffsetDest == 0)
    {
        pd++;
    }
    

    #else   //#if 1   //big
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff; 
    
    nDestAddr = HMRDV_FixEdsAddress(nDestAddr); /* fix U32 eds Address */
    
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;
    
    if (nDestAddr > 0xFFFF)
    {
        /* forbidden psv write - just advance read buffer pointer */
        ps += nSize;
        return (HMDRV_BPTR) ps;
    }
    HMDRV_SIZE8 nSizeBytes = nSize;      
    while(nSizeBytes > 0)
    {
        *pd++ = *ps++;
        nSizeBytes--;
    }
    #endif  //#if 1   //big



#endif  //#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY
    return (HMDRV_BPTR) ps;
}



#if HMDRV_USE_EXT_MEM == 0
/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer, perform AND-masking
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - source memory in communication buffer, mask follows data
* @param  nSize     - buffer size (always in bytes)
*
******************************************************************************/

void HMDRV_CopyFromBufferInternalWithMask(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_U16* pd = (HMDRV_U16*)(((HMDRV_U32)nDestAddr) >> 1);
    HMDRV_U8* pm = ps + nSize;
    HMDRV_U16 mask, stmp, dtmp;

    while(nSize)
    {
        mask = *pm++;
        stmp = *ps++;
        dtmp = *pd;

        if ((nSize & 1) == 0)
        {
            mask += (HMDRV_U16)*pm++ << 8;
            stmp += (HMDRV_U16)*ps++ << 8;
            nSize--;
        }
        /* perform AND-masking */
        stmp = (HMDRV_U16) ((stmp & mask) | (dtmp & ~mask));

        /* put the result back */
        *pd++ = stmp;
        nSize--;
    }
#elif HMDRV_CFG_BUS_WIDTH == 2

    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* pd = (HMDRV_U16*)(HMDRV_ADDR_SIZE)nDestAddr;
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_U16 mask, stmp, dtmp;
    HMDRV_U8* pm = ps + nSize;

    while(nSize)
    {
        mask = *pm++;
        stmp = *ps++;
        dtmp = *pd;
        if ((nSize & 1) == 0)
        {
            mask += (HMDRV_U16)*pm++ << 8;
            stmp += (HMDRV_U16)*ps++ << 8;
            nSize--;
        }

        /* perform AND-masking */
        stmp = ((stmp & mask) | (dtmp & ~mask));


        *pd++ = stmp;

        nSize--;
    }

#else
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;
    HMDRV_U8* pm = ps + nSize;
    HMDRV_U8 mask, stmp, dtmp;

    while(nSize--)
    {
        mask = *pm++;
        stmp = *ps++;
        dtmp = *pd;

        /* perform AND-masking */
        stmp = (HMDRV_U8) ((stmp & mask) | (dtmp & ~mask));

        /* put the result back */
        *pd++ = stmp;
     }
#endif  //#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY
}

#endif //#if HMDRV_USE_EXT_MEM == 0




/**************************************************************************//*!
*
* @brief  Write-into the communication buffer memory
*
* @param  pDestBuff - pointer to destination memory in communication buffer
* @param  nSrcAddr  - source memory address
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/
HMDRV_BPTR HMDRV_CopyToBufferInternal(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{

#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY

    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* ps = (HMDRV_U16*)(((HMDRV_U32)nSrcAddr) >> 1);
    HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;
    HMDRV_U16 data = 0;
    HMDRV_U8 odd = (HMDRV_U8)((HMDRV_U32)nSrcAddr & 0x01);
    if (odd == 0)
    {
        while(nSize--)
        {
            data = *ps++;
            *pd++ = data & 0xFF;
            if (nSize--)
            {
                *pd++ = data >> 8;
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        data = *ps;
        while(nSize--)
        {
            *pd++ = data >> 8;
            if (nSize--)
            {
                data = *ps++;
                *pd++ = data & 0xFF;
            }
            else
            {
                break;
            }
        }
    }
#elif HMDRV_CFG_BUS_WIDTH == 2

    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* ps = (HMDRV_U16*)(HMDRV_ADDR_SIZE)nSrcAddr;
    HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;
    HMDRV_U16 data;
    while(nSize)
    {
        data = *ps++;
        *pd++ = data & 0x00FF;

        if ((nSize & 1) == 0)
        {
            *pd++ = data >> 8;
            nSize--;
        }
        nSize--;
    }


#else

    //__psv__ HMDRV_U8* ps_psv_conv = (__psv__ HMDRV_U8*)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    //__psv__ HMDRV_U8* ps_psv_noconv = nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    //__eds__ HMDRV_U8* psEds = (__eds__ HMDRV_U8*)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    __eds__ HMDRV_U8* ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    __psv__ HMDRV_U8* ps_psv = (__psv__ HMDRV_U8*)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    volatile HMDRV_BPTR pd = pDestBuff;
	volatile HMDRV_SIZE8 nSizeLoop = nSize;
    
    if (nSrcAddr > 0xFFFF)
    {   
        //AddressPsvNoConv = ps_psv_noconv;
        //AddressEds = psEds;
        //AddressEdsNoConv = ps;
        //AddressPsv = ps_psv_conv;
        //AddressIn = nSrcAddr;
        //AddressOut = ps_psv;
        
        ps_psv = (__psv__ HMDRV_U8*)(HMRDV_FixEdsAddress(ps_psv));
        
        while(nSizeLoop > 0)
        {
            *pd++ = *ps_psv++;
            nSizeLoop--;
        }
    }
    else
    {  
        while(nSizeLoop > 0)
        {
            *pd++ = *ps++;
            nSizeLoop--;
        }
    }
#endif  //#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY

    return pd;
}


/**************************************************************************//*!
*
* @brief  Write-into the communication buffer memory
*
* @param  pDestBuff - pointer to destination memory in communication buffer
* @param  nSrcAddr  - source memory address
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/
HMDRV_BPTR HMDRV_CopyToBufferInternalCoil(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{

#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY

    /* ??? Not Checked */


    HMDRV_SIZE8 nBitOffset = 0;
    HMDRV_SIZE8 nBitOffsetDest = 0;

    HMDRV_U8 nextByte;
    HMDRV_U8 readBit;
    volatile HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* ps;

    ps = (HMDRV_U16*)((HMDRV_ADDR_SIZE)nSrcAddr >> 1);

    while(nSize--)
    {
        nextByte = *ps;

        if (nextByte & (1 << nBitOffset))
        {
            readBit = 1;
        }
        else
        {
            readBit = 0;
        }

        if (readBit)
        {
            *pd |= (1 << nBitOffsetDest);
        }
        else
        {
            *pd &= ~(1 << nBitOffsetDest);
        }

        nBitOffsetDest++;
        if (nBitOffsetDest == 0)
        {
            pd++;
        }

        nBitOffset++;
        if (nBitOffset == 0)
        {
            ps++;
        }

    }

    /* fix point to next byte in buffer */
    if (nBitOffset != 0)
    {
        ps++;
    }

    #if 0       /* needs implementation */

    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* ps = (HMDRV_U16*)(((HMDRV_U32)nSrcAddr) >> 1);
    HMDRV_U16 data = 0;
    HMDRV_U8 odd = (HMDRV_U8)((HMDRV_U32)nSrcAddr & 0x01);
    if (odd == 0)
    {
        while(nSize--)
        {
            data = *ps++;
            *pd++ = data & 0xFF;
            if (nSize--)
            {
                *pd++ = data >> 8;
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        data = *ps;
        while(nSize--)
        {
            *pd++ = data >> 8;
            if (nSize--)
            {
                data = *ps++;
                *pd++ = data & 0xFF;
            }
            else
            {
                break;
            }
        }
    }
    #endif
#elif HMDRV_CFG_BUS_WIDTH == 2

    HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;

    #if 0       /* needs implementation */
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* ps = (HMDRV_U16*)(HMDRV_ADDR_SIZE)nSrcAddr;
    HMDRV_U16 data;
    while(nSize)
    {
        data = *ps++;
        *pd++ = data & 0x00FF;

        if ((nSize & 1) == 0)
        {
            *pd++ = data >> 8;
            nSize--;
        }
        nSize--;
    }
    #endif

#else

    #if 1

    HMDRV_SIZE8 nBitOffset = 0;
    HMDRV_SIZE8 nBitOffsetDest = 0;
    
    HMDRV_U8 nextByte;
    HMDRV_U8 readBit;
    volatile HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;
    __eds__ HMDRV_U8* ps;   //was HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* ps;
    __psv__ HMDRV_U8* ps_psv = (__psv__ HMDRV_U8*)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    
    ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    if (nSrcAddr > 0xFFFF)
    {
        ps_psv = (__psv__ HMDRV_U8*)(HMRDV_FixEdsAddress(ps_psv));
    }
	
    while(nSize--)
    {
    
        if (nSrcAddr > 0xFFFF)
        {
            nextByte = *ps_psv;
        }
        else
        {
            nextByte = *ps;
        }

        if (nextByte & (1 << nBitOffset))
        {
            readBit = 1;
        }
        else
        {
            readBit = 0;
        }

    #if 1
        if (readBit)
        {
            *pd |= (1 << nBitOffsetDest);
        }
        else
        {
            *pd &= ~(1 << nBitOffsetDest);
        }
    #else
        if (nBitOffsetDest == 0)
        {
            *pd = readBit;
        }
        else
        {
            *pd |= (readBit << nBitOffsetDest);
        }
    #endif

        nBitOffsetDest++;
        if (nBitOffsetDest == 0)
        {
            pd++;
        }

        nBitOffset++;
        if (nBitOffset == 0)
        {
            ps++;
            ps_psv++;
        }

    }
    
    /* fix point to next byte in buffer */
    if (nBitOffset != 0)
    {
        ps++;
    }
    
    
    #else
    //__psv__ HMDRV_U8* ps_psv_conv = (__psv__ HMDRV_U8*)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    //__psv__ HMDRV_U8* ps_psv_noconv = nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    //__eds__ HMDRV_U8* psEds = (__eds__ HMDRV_U8*)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    __eds__ HMDRV_U8* ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    __psv__ HMDRV_U8* ps_psv = (__psv__ HMDRV_U8*)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    volatile HMDRV_BPTR pd = pDestBuff;
	volatile HMDRV_SIZE8 nSizeLoop = nSize;
    
    if (nSrcAddr > 0xFFFF)
    {   
        //AddressPsvNoConv = ps_psv_noconv;
        //AddressEds = psEds;
        //AddressEdsNoConv = ps;
        //AddressPsv = ps_psv_conv;
        //AddressIn = nSrcAddr;
        //AddressOut = ps_psv;
        
        ps_psv = (__psv__ HMDRV_U8*)(HMRDV_FixEdsAddress(ps_psv));
        
        while(nSizeLoop > 0)
        {
            *pd++ = *ps_psv++;
            nSizeLoop--;
        }
    }
    else
    {  
        while(nSizeLoop > 0)
        {
        *pd++ = *ps++;
            nSizeLoop--;
    }
    }
    #endif
#endif  //#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY

    return (HMDRV_BPTR) pd;
}




/**************************************************************************//*!
*
* @brief  Write-into the communication buffer memory
*
* @param  pDestBuff - pointer to destination memory in communication buffer
* @param  nSrcAddr  - source memory address
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/
HMDRV_BPTR HMDRV_CopyToBufferInternalSingleCoil(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr)
{

#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY


    HMDRV_SIZE8 nBitOffset = 0; /* to be fixed !!! */

    HMDRV_U8 nextByte;
    //HMDRV_U8 readBit;
    volatile HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* ps;

    ps = (HMDRV_U16*)((HMDRV_ADDR_SIZE)nSrcAddr >> 1);

    nextByte = *ps;

    if (nextByte & (1 << nBitOffset))
    {
        //readBit = 1;
        *pd++ = 0xFF;
    }
    else
    {
        //readBit = 0;
        *pd++ = 0x00;
    }
    *pd++ = 0;



    #if 0       /* needs implementation */
    HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;

    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* ps = (HMDRV_U16*)(((HMDRV_U32)nSrcAddr) >> 1);
    HMDRV_U16 data = 0;
    HMDRV_U8 odd = (HMDRV_U8)((HMDRV_U32)nSrcAddr & 0x01);
    if (odd == 0)
    {
        while(nSize--)
        {
            data = *ps++;
            *pd++ = data & 0xFF;
            if (nSize--)
            {
                *pd++ = data >> 8;
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        data = *ps;
        while(nSize--)
        {
            *pd++ = data >> 8;
            if (nSize--)
            {
                data = *ps++;
                *pd++ = data & 0xFF;
            }
            else
            {
                break;
            }
        }
    }
    #endif
#elif HMDRV_CFG_BUS_WIDTH == 2

    HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;

    #if 0       /* needs implementation */
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* ps = (HMDRV_U16*)(HMDRV_ADDR_SIZE)nSrcAddr;
    HMDRV_U16 data;
    while(nSize)
    {
        data = *ps++;
        *pd++ = data & 0x00FF;

        if ((nSize & 1) == 0)
        {
            *pd++ = data >> 8;
            nSize--;
        }
        nSize--;
    }
    #endif

#else

    #if 1

    HMDRV_SIZE8 nBitOffset = 0; /* to be fixed !!! */
    
    HMDRV_U8 nextByte;
    HMDRV_U8 readBit;
    volatile HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;
    __eds__ HMDRV_U8* ps;   //was HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* ps;
    __psv__ HMDRV_U8* ps_psv = (__psv__ HMDRV_U8*)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    
    ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    if (nSrcAddr > 0xFFFF)
    {
        ps_psv = (__psv__ HMDRV_U8*)(HMRDV_FixEdsAddress(ps_psv));
    }

    if (nSrcAddr > 0xFFFF)
    {
        nextByte = *ps_psv;
    }
    else
    {
        nextByte = *ps;
    }
    if (nextByte & (1 << nBitOffset))
    {
        readBit = 1;
        *pd++ = 0xFF;
    }
    else
    {
        readBit = 0;
        *pd++ = 0x00;
    }
    *pd++ = 0;
    

    
    
    
    #else
    //__psv__ HMDRV_U8* ps_psv_conv = (__psv__ HMDRV_U8*)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    //__psv__ HMDRV_U8* ps_psv_noconv = nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    //__eds__ HMDRV_U8* psEds = (__eds__ HMDRV_U8*)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    __eds__ HMDRV_U8* ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    __psv__ HMDRV_U8* ps_psv = (__psv__ HMDRV_U8*)nSrcAddr; //was (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    volatile HMDRV_BPTR pd = pDestBuff;
	volatile HMDRV_SIZE8 nSizeLoop = nSize;
    
    if (nSrcAddr > 0xFFFF)
    {   
        //AddressPsvNoConv = ps_psv_noconv;
        //AddressEds = psEds;
        //AddressEdsNoConv = ps;
        //AddressPsv = ps_psv_conv;
        //AddressIn = nSrcAddr;
        //AddressOut = ps_psv;
        
        ps_psv = (__psv__ HMDRV_U8*)(HMRDV_FixEdsAddress(ps_psv));
        
        while(nSizeLoop > 0)
        {
            *pd++ = *ps_psv++;
            nSizeLoop--;
        }
    }
    else
    {  
        while(nSizeLoop > 0)
        {
            *pd++ = *ps++;
            nSizeLoop--;
        }
    }
    #endif
#endif  //#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY

    return (HMDRV_BPTR)pd;
}






#if HMDRV_USE_MODBUS

/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/
HMDRV_BPTR HMDRV_CopyFromBufferInternalModbus(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize, HMDRV_U8 u8ElementSize, HMDRV_U8* pu8ElementSizeArray, HMDRV_BOOL bStringNullTerminationPossible)
{
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    
    nDestAddr = HMRDV_FixEdsAddress(nDestAddr); /* fix U32 eds Address */
    
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;
    
    //HM_pCopyFromSrc  = ps;
    //HM_pCopyFromDest = pd;
    //HM_pCopyFromSize = nSize;

    HMDRV_SIZE8 nSizeBytes;

    volatile uint16_t* pd16;
    HMDRV_U16 u16TempData;

    HMDRV_U8 u8ElementSizeCounter = 0;
    HMDRV_U8 u8ElementSizeArrayIndex = 0;
    HMDRV_U8 u8ElementSizeNow = 0;
    //HMDRV_U8 u8ElementSizeNowAddressAdd = 0;


    nSizeBytes = nSize;
    while(nSizeBytes > 0)
    {
        /* Extract u8ElementSizeCounter */
        if (u8ElementSizeCounter == 0)
        {
            if (pu8ElementSizeArray == NULL)
            {
                u8ElementSizeNow = u8ElementSize;
            }
            else
            {
                if (u8ElementSizeArrayIndex >= u8ElementSize)
                {
                    u8ElementSizeArrayIndex = 0;
                }
                u8ElementSizeNow = pu8ElementSizeArray[u8ElementSizeArrayIndex++];
            }

            if (u8ElementSizeNow == HMDRV_ELEMENT_SIZE_STRING)
            {
                u8ElementSizeCounter = HMDRV_ELEMENT_SIZE_STRING;
            }
            else
            {
                u8ElementSizeCounter = u8ElementSizeNow;
            }

        }

        if (u8ElementSizeNow == HMDRV_ELEMENT_SIZE_STRING)
        {
            /* string expected */
            while(nSizeBytes > 0)
            {
                *pd++ = *ps;
                if (*ps++ == 0)
                {
                    nSizeBytes = 0;
                }
                else
                {
                    nSizeBytes--;
                    if (nSizeBytes == 0) /* force null termination */
                    {
                        *pd = 0;
                    }
                }
            }
        }
        else
        {


#if 0 //before 2020 01 14
            /* process copy fixed size data */

            if (((nSize&1) == 1) && (u8ElementSizeNow == nSize)) /* if odd bytes and single element  -> get padding zero first */
            {
                ps++;
            }

            while ((u8ElementSizeCounter > 0) && (nSizeBytes > 0))
            {
                u8ElementSizeCounter--;
                *(pd+u8ElementSizeCounter) = *ps++;
                nSizeBytes--;





                if (nSizeBytes == 0)
                {
                    if (((nSize&1) == 1) && (u8ElementSizeNow != nSize))/* if odd bytes and not single element  -> send padding zero at end */
                    {
                        ps++;
                    }
                }
            }
            pd += u8ElementSizeNow;
#endif
            /* process copy fixed size data */
            if (((nSize&1) == 1) && (u8ElementSizeNow == nSize)) /* if odd bytes and single element  -> get padding zero first */
            {
                ps++;
            }

            pd16 = (uint16_t*)pd + ((u8ElementSizeNow+1)>>1)-1;
            //u8ElementSizeNowAddressAdd = 0;

            while ((u8ElementSizeCounter > 0) && (nSizeBytes > 0))
            {
#if HMDRV_ADDR_WIDTH == 2

                //odd u8ElementSizeCounter
                if (u8ElementSizeCounter & 1 == 1)
                {
                    *pd16 = (*pd16 & 0xFF00) | ((*ps++) & 0x00FF);  /* preserve current high byte */
                    //*pd16 = (*ps++) & 0x00FF;
                    u8ElementSizeCounter--;
                    nSizeBytes--;
                }
                //even u8ElementSizeCounter
                else
                {
                    u16TempData = ((uint16_t)(*ps++) << 8);
                    u16TempData |= ((*ps++) & 0x00FF);
                    *pd16 = u16TempData;
                    u8ElementSizeCounter -= 2;
                    nSizeBytes -= 2;
                }
                pd16--;

                //u8ElementSizeNowAddressAdd++;

#else
                u8ElementSizeCounter--;
                *(pd+u8ElementSizeCounter) = *ps++;
                nSizeBytes--;
                //u8ElementSizeNowAddressAdd++;
#endif

                if (nSizeBytes == 0)
                {
                    if (((nSize&1) == 1) && (u8ElementSizeNow != nSize))/* if odd bytes and not single element  -> send padding zero at end */
                    {
                        ps++;
                    }
                }
            }

            // HMDRV_ADDR_WIDTH dependency made below
            //pd += u8ElementSizeNowAddressAdd;//2020 06 10 was pd += u8ElementSizeNow;
            //pd += u8ElementSizeNow;

#if HMDRV_ADDR_WIDTH == 2

            if (u8ElementSizeNow == 1)
            {
                u8ElementSizeNow = 2; /* uint8_t if 2 bytes long  */
            }
            pd += u8ElementSizeNow >> 1;


#else
            pd += u8ElementSizeNow;
#endif


        }
    }



    return (HMDRV_BPTR) ps;
}

/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/
HMDRV_BPTR HMDRV_CopyFromBufferInternalModbusBitField(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSizeBits, HMDRV_SIZE8 nStartBit, HMDRV_U8 u8ElementSize, HMDRV_U8* pu8ElementSizeArray, HMDRV_BOOL bStringNullTerminationPossible)
{
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;

    nDestAddr = HMRDV_FixEdsAddress(nDestAddr); /* fix U32 eds Address */

    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;


    HMDRV_U8* pm;

    //HM_pCopyFromSrc  = ps;
    //HM_pCopyFromDest = pd;
    //HM_pCopyFromSize = nSize;
    
    HMDRV_SIZE8 nSizeBytes; 
    HMDRV_SIZE8 nSize;
    
    volatile uint16_t* pd16;
    volatile uint16_t* pm16;
    HMDRV_U16 u16TempData;
    
    HMDRV_U64 u64Mask;

    HMDRV_U8 u8ElementSizeCounter = 0;
    HMDRV_U8 u8ElementSizeArrayIndex = 0;
    HMDRV_U8 u8ElementSizeNow = 0;
    //HMDRV_U8 u8ElementSizeNowAddressAdd = 0;
    
    nSize = (nSizeBits + nStartBit + 7) >> 3;

    u64Mask = ((1 << nSizeBits) - 1);
    u64Mask <<= nStartBit;

    pm = (HMDRV_U8*)&u64Mask;
    
    nSizeBytes = nSize; 
    while(nSizeBytes > 0)
    {
        /* Extract u8ElementSizeCounter */
        if (u8ElementSizeCounter == 0)
        {
            if (pu8ElementSizeArray == NULL)
            {
                u8ElementSizeNow = u8ElementSize;
            }
            else
            {
                if (u8ElementSizeArrayIndex >= u8ElementSize)
                {
                    u8ElementSizeArrayIndex = 0;
                }
                u8ElementSizeNow = pu8ElementSizeArray[u8ElementSizeArrayIndex++]; 
            }
            
            if (u8ElementSizeNow == HMDRV_ELEMENT_SIZE_STRING)
            {
                u8ElementSizeCounter = HMDRV_ELEMENT_SIZE_STRING;  
            }
            else
            {
                u8ElementSizeCounter = u8ElementSizeNow;
            }
            
        }
        
        if (u8ElementSizeNow == HMDRV_ELEMENT_SIZE_STRING) //not supported for bit field
        {
            /* string expected */
            while(nSizeBytes > 0)
            {
                *pd++ = *ps;
                if (*ps++ == 0)
                {
                    nSizeBytes = 0;
                }
                else
                {
                    nSizeBytes--;
                    if (nSizeBytes == 0) /* force null termination */
                    {
                        *pd = 0;
                    }
                }  
            }
        }        
        else
        {


#if 0 //before 2020 01 14
            /* process copy fixed size data */
            
            if (((nSize&1) == 1) && (u8ElementSizeNow == nSize)) /* if odd bytes and single element  -> get padding zero first */
            {
                ps++;  
            }
            
            while ((u8ElementSizeCounter > 0) && (nSizeBytes > 0))
            {
                u8ElementSizeCounter--;
                *(pd+u8ElementSizeCounter) = *ps++;
                nSizeBytes--;   





                if (nSizeBytes == 0)
                {
                    if (((nSize&1) == 1) && (u8ElementSizeNow != nSize))/* if odd bytes and not single element  -> send padding zero at end */
                    {
                        ps++;
                    }
                }
            }
            pd += u8ElementSizeNow;
#endif
            /* process copy fixed size data */
            if (((nSize&1) == 1) && (u8ElementSizeNow == nSize)) /* if odd bytes and single element  -> get padding zero first */
            {
                ps++;
            }

            pd16 = (uint16_t*)pd + ((u8ElementSizeNow+1)>>1)-1;
            //u8ElementSizeNowAddressAdd = 0;
            pm16 = (uint16_t*)pm + ((u8ElementSizeNow+1)>>1)-1;


            while ((u8ElementSizeCounter > 0) && (nSizeBytes > 0))
            {
#if HMDRV_ADDR_WIDTH == 2
                //odd u8ElementSizeCounter
                if (u8ElementSizeCounter & 1 == 1)
                {

                    *pd16 =   (*pd16 & (~(*pm16))) | ((*ps++) & (*pm16));  /* preserve other data */

                    //*pd16 = (*pd16 & 0xFF00) | ((*ps++) & 0x00FF);  /* preserve current high byte */

                    u8ElementSizeCounter--;
                    nSizeBytes--;
                }
                //even u8ElementSizeCounter
                else
                {
                    u16TempData = ((uint16_t)(*ps++) << 8);
                    u16TempData |= ((*ps++) & 0x00FF);
                    *pd16 =   (*pd16 & (~(*pm16))) | ((u16TempData) & (*pm16));  /* preserve other data */
                    u8ElementSizeCounter -= 2;
                    nSizeBytes -= 2;
                }
                pd16--;
                pm16--;

                //u8ElementSizeNowAddressAdd++;

#else
                u8ElementSizeCounter--;
                *(pd+u8ElementSizeCounter) =      (*(pd+u8ElementSizeCounter) & (~(*(pm+u8ElementSizeCounter)))) | ((*ps++) & (*(pm+u8ElementSizeCounter)));  /* preserve other data */
                //*(pd+u8ElementSizeCounter) = *ps++;
                nSizeBytes--;
                //u8ElementSizeNowAddressAdd++;
#endif
                
                if (nSizeBytes == 0)
                {
                    if (((nSize&1) == 1) && (u8ElementSizeNow != nSize))/* if odd bytes and not single element  -> send padding zero at end */
                    {
                        ps++;
                    }
                }
            }

            // HMDRV_ADDR_WIDTH dependency made below
            //pd += u8ElementSizeNowAddressAdd;//2020 06 10 was pd += u8ElementSizeNow;
            //pd += u8ElementSizeNow;

#if HMDRV_ADDR_WIDTH == 2

            if (u8ElementSizeNow == 1)
            {
                u8ElementSizeNow = 2; /* uint8_t if 2 bytes long  */
            }
            pd += u8ElementSizeNow >> 1;
            pm += u8ElementSizeNow >> 1;


#else
            pd += u8ElementSizeNow;
            pm += u8ElementSizeNow;
#endif


        }
    }
    


    return (HMDRV_BPTR) ps;
}

#if 0    //was before 2020 01 05
/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/
/* to do big to little endian */
HMDRV_BPTR HMDRV_CopyFromBufferInternalModbus(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;

	//HM_pCopyFromSrc  = ps;
	//HM_pCopyFromDest = pd;
	//HM_pCopyFromSize = nSize;
    
    HMDRV_SIZE8 nSizeBytes = nSize;      
    while(nSizeBytes > 0)
    {

        if (nSizeBytes >= 2)
        {
            *pd++ = *(ps+1);
            *pd++ = *(ps+0);
            nSizeBytes -= 2;
            ps += 2;
        }
        else /* if (nSizeBytes == 1) */
        {   
            *pd++ = *(ps+1);    /* write only Low Byte to memory */
            nSizeBytes -= 1;
            ps += 2;
        }
    }
	
    return (HMDRV_BPTR) ps;
}
#endif  //#if 0    //was before 2020 01 05
/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/
HMDRV_BPTR HMDRV_CopyFromBufferInternalModbusCoil(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nBitOffset, HMDRV_SIZE8 nBitOffsetDest)
{
    HMDRV_U8 nextByte;
    HMDRV_U8 readBit;
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    
    nDestAddr = HMRDV_FixEdsAddress(nDestAddr); /* fix U32 eds Address */
    
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;
    
    nextByte = *ps;
    
    if (nextByte & (1 << nBitOffset))
    {
        readBit = 1;
    }
    else
    {
        readBit = 0;
    }
    
#if 1

    while (nBitOffsetDest >= (8 * HMDRV_ADDR_WIDTH))
    {
        nBitOffsetDest -= (8 * HMDRV_ADDR_WIDTH);
        pd++;
    }

    if (readBit)
    {
        *pd |= (1 << nBitOffsetDest);
    }
    else
    {
        *pd &= ~(1 << nBitOffsetDest);
    }
#else
    if (nBitOffsetDest == 0)
    {
        *pd = readBit;
    }
    else
    {
        *pd |= (readBit << nBitOffsetDest);
    }
#endif

    return (HMDRV_BPTR) ps;
}





/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/
HMDRV_BPTR HMDRV_CopyFromBufferInternalModbusSingleCoil(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nBitOffset)
{
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_U8 inByte;
    HMDRV_U8 inByteL;
    HMDRV_U8 readBit;
    
    nDestAddr = HMRDV_FixEdsAddress(nDestAddr); /* fix U32 eds Address */
    
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;
    
    inByte = *ps++;
    inByteL = *ps++;
    if (inByteL != 0x00)
    {
        ps = NULL;
    }
    else
    {
        //inByte = *ps++;
        if (inByte == 0xFF)
        {
            readBit = 1;
        }
        else if (inByte == 0x00)
        {
            readBit = 0;
        }
        else
        {
            ps = NULL;
            return (HMDRV_BPTR) ps;
        }
        
        while (nBitOffset >= (8 * HMDRV_ADDR_WIDTH))
        {
            nBitOffset -= (8 * HMDRV_ADDR_WIDTH);
            pd++;
        }
        /* do the write */
        *pd &= ~(1 << nBitOffset);  /* clear first */
        *pd |= (readBit << nBitOffset);
    }
	
    return (HMDRV_BPTR) ps;
}



/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer, perform AND-masking
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - source memory in communication buffer, mask follows data
* @param  nSize     - buffer size (always in bytes)
*
******************************************************************************/


void HMDRV_CopyFromBufferInternalWithMaskModbus(HMDRV_ADDR nDestAddr, HMDRV_ADDR nSrcAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* po = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;   /* old data */
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;  /* new data */
    HMDRV_U8* pm = pSrcBuff;
    HMDRV_U8 maskAnd, maskOr, stmp;

    HMDRV_SIZE8 nSizeBytes = nSize;      
    while(nSizeBytes > 0)
    { 
        if (nSizeBytes >= 2)
        {
            /* Low Byte */
            maskAnd = *(pm+1);
            maskOr = *(pm+3);
            stmp = *po++;
            /* perform masking */
            stmp = (HMDRV_U8) ((stmp & maskAnd) | (maskOr & ~maskAnd));
            /* put the result back */
            *pd++ = stmp;            
            
            /* High Byte */
            maskAnd = *(pm+0);
            maskOr = *(pm+2);
            stmp = *po++;
            /* perform masking */
            stmp = (HMDRV_U8) ((stmp & maskAnd) | (maskOr & ~maskAnd));
            /* put the result back */
            *pd++ = stmp;            
            

            nSizeBytes -= 2;
            pm += 4;
        }
        else /* if (nSizeBytes == 1) */
        {  
            
            /* Low Byte */
            maskAnd = *(pm+1);
            maskOr = *(pm+3);
            stmp = *po++;
            /* perform masking */
            stmp = (HMDRV_U8) ((stmp & maskAnd) | (maskOr & ~maskAnd));
            /* put the result back */
            *pd++ = stmp;            
            
            nSizeBytes -= 1;
            pm += 4;
        }
    }
}

#if HMDRV_USE_EXT_MEM == 0
/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer, perform AND-masking
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - source memory in communication buffer, mask follows data
* @param  nSize     - buffer size (always in bytes)
*
******************************************************************************/

HMDRV_BOOL HMDRV_CopyFromBufferWithMaskModbus(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
    HMDRV_BOOL result = HMDRV_FALSE;
    HMDRV_ADDR pDestAddr = nDestAddr;
    HMDRV_ADDR pSrcAddr = pDestAddr;

    HMDRV_CopyFromBufferInternalWithMaskModbus(pDestAddr, pSrcAddr, pSrcBuff, nSize);
    result = HMDRV_TRUE;
    return result;
}

#endif  //#if HMDRV_USE_EXT_MEM == 0


/**************************************************************************//*!
*
* @brief  Write-into the communication buffer memory
*
* @param  pDestBuff - pointer to destination memory in communication buffer
* @param  nSrcAddr  - source memory address
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*         Data in buffer is written in 16-bit big-endian mode Modbus compatible
*
******************************************************************************/
HMDRV_BPTR HMDRV_CopyToBufferInternalModbus(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize, HMDRV_U8 u8ElementSize, HMDRV_U8* pu8ElementSizeArray)
{
    HMDRV_U8 nextByte;
    HMDRV_U16 nextWord;
    volatile HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* ps;   //was HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* ps;    2020 01 05 was __eds__ HMDRV_U8* ps;

    volatile uint16_t* ps16;
    
    ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;

    //psTest2 = (HMDRV_U8*)nSrcAddr;
    
    volatile HMDRV_SIZE8 nSizeBytes;
    
    volatile HMDRV_U8 u8ElementSizeCounter = 0;
    volatile HMDRV_U8 u8ElementSizeArrayIndex = 0;
    volatile HMDRV_U8 u8ElementSizeNow = 0;
       
    nSizeBytes = nSize; 
  
    while(nSizeBytes > 0)
    {
        /* Extract u8ElementSizeCounter */
        if (u8ElementSizeCounter == 0)
        {
            if (pu8ElementSizeArray == NULL)
            {
                u8ElementSizeNow = u8ElementSize;
            }
            else
            {
                if (u8ElementSizeArrayIndex >= u8ElementSize)
                {
                    u8ElementSizeArrayIndex = 0;
                }
                u8ElementSizeNow = pu8ElementSizeArray[u8ElementSizeArrayIndex++]; 
            }
            

            if (u8ElementSizeNow == HMDRV_ELEMENT_SIZE_STRING)
            {
                u8ElementSizeCounter = HMDRV_ELEMENT_SIZE_STRING;  
            }
            else
            {
                u8ElementSizeCounter = u8ElementSizeNow;
            }
            
        }
        
        if ((u8ElementSizeNow & HMDRV_ELEMENT_MASK_STRING) > 0) /* to do implementation for write */
        {
            /* string expected TI 16bit coded also in high byte */
            ps16 = (uint16_t*)ps;

            if (nSizeBytes & 0x0001)
            {
                nSizeBytes++;
            }

            while(nSizeBytes > 0)
            {
                nextWord = *ps16++;

                if (nSizeBytes >= 2)
                {
                    nSizeBytes -= 2;
                }
                else
                {
                    nSizeBytes = 0;
                }

                nextByte = nextWord & 0x00FF;

                if (nextByte == 0)
                {
                    *pd++ = 0;
                }
                else
                {
                    *pd++ = nextByte;

                    nextByte = nextWord >> 8;

                    if (nextByte == 0)
                    {
                        *pd++ = 0;

                    }
                    else
                    {
                        *pd++ = nextByte;
                    }
                }
            }
        }
        else if (u8ElementSizeNow == HMDRV_ELEMENT_SIZE_STRING)
        {
            /* string expected */
            while(nSizeBytes > 0)
            {
                nextByte = *ps;
                
                
                
                if (nextByte == 0)
                {
                    if (u8ElementSizeCounter == 0)
                    {
                        *pd++ = 0;
                        u8ElementSizeCounter++;
                    }
                    /* force string null termination if needed */
                    if ((u8ElementSizeCounter & 1) > 0)
                    {
                        *pd++ = 0;
                        u8ElementSizeCounter++;
                    }
                    
                    nSizeBytes = 0;
                    
                }
                else
                {
                    *pd++ = *ps++;

                    u8ElementSizeCounter++;                
                    nSizeBytes--; 
                    
                    if (nSizeBytes == 0)
                    {
                        /* force string null termination if needed */
                        if ((u8ElementSizeCounter & 1) > 0)
                        {
                            *pd++ = 0;
                            u8ElementSizeCounter++;
                        } 
                    }
                    
                }
                
            }
        }
        else
        {



            /* process copy fixed size data */
            if (((nSize&1) == 1) && (u8ElementSizeNow == nSize)) /* if odd bytes and single element  -> send padding zero first */
            {
                *pd++ = 0;  
            }

            ps16 = (uint16_t*)ps + ((u8ElementSizeNow+1)>>1)-1;

            
            while ((u8ElementSizeCounter > 0) && (nSizeBytes > 0))
            {
#if HMDRV_ADDR_WIDTH == 2

                //odd u8ElementSizeCounter
                if (u8ElementSizeCounter & 1 == 1)
                {
                    *pd++ = *ps16 & 0x00FF;
                    u8ElementSizeCounter--;
                    nSizeBytes--;
                }
                //even u8ElementSizeCounter
                else
                {
                    *pd++ = *ps16 >> 8;
                    *pd++ = *ps16 & 0x00FF;
                    u8ElementSizeCounter -= 2;
                    nSizeBytes -= 2;
                }
                ps16--;


#else
                u8ElementSizeCounter--;
                *pd++ = *(ps+u8ElementSizeCounter);
                nSizeBytes--;   
#endif
                
                if (nSizeBytes == 0)
                {
                    if (((nSize&1) == 1) && (u8ElementSizeNow != nSize))/* if odd bytes and not single element  -> send padding zero at end */
                    {
                        *pd++ = 0;
                    }
                }
            }       
#if HMDRV_ADDR_WIDTH == 2

            if (u8ElementSizeNow == 1)
            {
                u8ElementSizeNow = 2; /* uint8_t if 2 bytes long  */
            }
            ps += u8ElementSizeNow >> 1;


#else
            ps += u8ElementSizeNow;
#endif
        }

    }

    return (HMDRV_BPTR) pd;
}


#if 0    //was before 2020 01 05
/**************************************************************************//*!
*
* @brief  Write-into the communication buffer memory
*
* @param  pDestBuff - pointer to destination memory in communication buffer
* @param  nSrcAddr  - source memory address
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*         Data in buffer is written in 16-bit big-endian mode Modbus compatible
*
******************************************************************************/
HMDRV_BPTR HMDRV_CopyToBufferInternalModbus(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* ps;
    HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;

    ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;

    //psTest2 = (HMDRV_U8*)nSrcAddr;

    HMDRV_SIZE8 nSizeBytes = nSize;
    while(nSizeBytes > 0)
    {

        if (nSizeBytes >= 2)
        {
            *pd++ = *(ps+1);
            *pd++ = *(ps+0);
            nSizeBytes -= 2;
        }
        else /* if (nSizeBytes == 1) */
        {
            *pd++ = 0;   /* Zero High Byte in Buffer */
            *pd++ = *(ps+0);
            nSizeBytes -= 1;
        }
        ps += 2;
    }

    return (HMDRV_BPTR) pd;
}

#endif  //#if 0    //was before 2020 01 05






/**************************************************************************//*!
*
* @brief  Write-into the communication buffer memory
*
* @param  pDestBuff - pointer to destination memory in communication buffer
* @param  nSrcAddr  - source memory address
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*         Data in buffer is written in 16-bit big-endian mode Modbus compatible
*
******************************************************************************/
HMDRV_BPTR HMDRV_CopyToBufferInternalModbusCoil(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nBitOffset, HMDRV_SIZE8 nBitOffsetDest)
{
    HMDRV_U8 nextByte;
    HMDRV_U8 readBit;
    volatile HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* ps;   //was HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* ps;
    
    ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;

    while (nBitOffset >= (8 * HMDRV_ADDR_WIDTH))
    {
        nBitOffset -= (8 * HMDRV_ADDR_WIDTH);
        ps++;
    }



    nextByte = *ps;

    if (nextByte & (1 << nBitOffset))
    {
        readBit = 1;
    }
    else
    {
        readBit = 0;
    }
    
    while (nBitOffsetDest >= (8 * HMDRV_ADDR_WIDTH))
    {
        nBitOffsetDest -= (8 * HMDRV_ADDR_WIDTH);
        pd++;
    }
    if (readBit)
    {
        *pd |= (1 << nBitOffsetDest);
    }
    else
    {
        *pd &= ~(1 << nBitOffsetDest);
    }
    

    return (HMDRV_BPTR) pd;
}


/**************************************************************************//*!
*
* @brief  Write-into the communication buffer memory
*
* @param  pDestBuff - pointer to destination memory in communication buffer
* @param  nSrcAddr  - source memory address
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*         Data in buffer is written in 16-bit big-endian mode Modbus compatible
*
******************************************************************************/
HMDRV_BPTR HMDRV_CopyToBufferInternalModbusSingleCoil(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nBitOffset)
{
    HMDRV_U8 nextByte;
    //HMDRV_U8 readBit;
    volatile HMDRV_U8* pd = (HMDRV_U8*) pDestBuff;
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* ps;   //was HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* ps;
    
    ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;

    while (nBitOffset >= (8 * HMDRV_ADDR_WIDTH))
    {
        nBitOffset -= (8 * HMDRV_ADDR_WIDTH);
        ps++;
    }

    nextByte = *ps;

    if (nextByte & (1 << nBitOffset))
    {
        //readBit = 1;
        *pd++ = 0xFF;
    }
    else
    {
        //readBit = 0;
        *pd++ = 0x00;
    }
    *pd++ = 0;
    
    
    
    
    return (HMDRV_BPTR) pd;
}


#endif  //#if HMDRV_USE_MODBUS



/**************************************************************************//*!
*
* @brief  strlen function
*
* @param  nSrcAddr  - source memory address
* @param  nSize     - buffer size (always in bytes)
*
* @return string length
*
******************************************************************************/
HMDRV_SIZE8 HMDRV_GetStringLengthInternal(HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* ps;
    ps = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;

    
    HMDRV_SIZE8 nSizeBytes = 0;


    while(nSizeBytes < nSize)
    {
        if (*ps++ == NULL)
        {
            break;
        }
        nSizeBytes++;
    }

    return nSizeBytes;
}



/**************************************************************************//*!
*
* @brief  strlen function
*
* @param  nSrcAddr  - source memory address
* @param  nSize     - buffer size (always in bytes)
*
* @return string length
*
******************************************************************************/
HMDRV_SIZE8 HMDRV_GetStringLengthFromWordInternal(HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U16* ps16;
    ps16 = (HMDRV_U16*)(HMDRV_ADDR_SIZE)nSrcAddr;
    HMDRV_U16 u16Data;

    HMDRV_SIZE8 nSizeBytes = 0;


    while(nSizeBytes < nSize)
    {
        u16Data = *ps16++;

        if ((u16Data & 0x00FF) == NULL)
        {
            break;
        }
        nSizeBytes++;

        if(nSizeBytes < nSize)
        {
            if ((u16Data >> 8) == NULL)
            {
                break;
            }
            nSizeBytes++;
        }
    }

    return nSizeBytes;
}















#if HMDRV_USE_EXT_MEM == 1

/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/

HMDRV_BPTR HMDRV_CopyFromBuffer(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
    HMDRV_U8* ps;
    HMDRV_ADDR pDestAddr;
    HMDRV_U16 u16ExtMemID;
    
    pDestAddr = HMDRV_pGetInternalMemoryAddressWrite(nDestAddr, nSize, &u16ExtMemID);
    
    if ((pDestAddr != NULL) || (nDestAddr == NULL))
    {
        ps = HMDRV_CopyFromBufferInternal(pDestAddr, pSrcBuff, nSize);
        
        if (pDestAddr != nDestAddr)
        {
            ProcessExternalMemory(pDestAddr, nDestAddr, nSize, &u16ExtMemID);
        }
    }
    else
    {
        ps = pSrcBuff;
        ps += nSize;
        
        //ps = NULL;
    }

    
    
    
    return (HMDRV_BPTR) ps;
}


/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer, perform AND-masking
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - source memory in communication buffer, mask follows data
* @param  nSize     - buffer size (always in bytes)
*
******************************************************************************/

volatile HMDRV_U8* psTest5;
volatile HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pdTest5;
volatile HMDRV_U8* pmTest5;
volatile HMDRV_U8  maskTest5;

void HMDRV_CopyFromBufferInternalWithMask(HMDRV_ADDR nDestAddr, HMDRV_ADDR nSrcAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* po = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nSrcAddr;
    HMDRV_ADDR_PNTR_MODIFIER HMDRV_U8* pd = (HMDRV_U8*)(HMDRV_ADDR_SIZE)nDestAddr;
    HMDRV_U8* ps = (HMDRV_U8*) pSrcBuff;
    HMDRV_U8* pm = pSrcBuff + nSize;
    HMDRV_U8 mask, stmp, dtmp;

    while(nSize--)
    {
        pmTest5 = pm;
        
        mask = *pm++;
        
        maskTest5 = mask;    
        pdTest5 = pd;   
        psTest5 = ps;
        
        stmp = *ps++;
        dtmp = *po++;

        /* perform AND-masking */
        stmp = (HMDRV_U8) ((stmp & mask) | (dtmp & ~mask));

        /* put the result back */
        *pd++ = stmp;
    }
}





/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer, perform AND-masking
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - source memory in communication buffer, mask follows data
* @param  nSize     - buffer size (always in bytes)
*
******************************************************************************/
volatile HMDRV_ADDR pDestTest6;
volatile HMDRV_ADDR nDestTest6;
volatile HMDRV_ADDR pSrcTest6;



void HMDRV_CopyFromBufferWithMask(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
    //HMDRV_U8* ps;
    //HMDRV_U8* pd;
    HMDRV_ADDR pDestAddr;
    HMDRV_ADDR pSrcAddr;
    HMDRV_ADDR nSrcAddr;
    HMDRV_BPTR pDestBuff;
    HMDRV_SIZE8 nSizeRead;
    HMDRV_U16 u16ExtMemID;
    HMDRV_U16 u16ExtMemIDRead = 0xFFFF;
   
    HMDRV_SIZE8 nSizeReq = nSize;
    while(nSizeReq)
    {    
        pDestAddr = HMDRV_pGetInternalMemoryAddressRead(nDestAddr, nSizeReq, &nSizeRead, &u16ExtMemIDRead);
        nSizeReq -= nSizeRead;
        nSize = nSizeRead;
    
    pDestBuff = pSrcBuff;
    pSrcAddr = pDestAddr;
    nSrcAddr = nDestAddr;
    
    if ((pSrcAddr != NULL) || (nSrcAddr == NULL))
    {
        //pd = HMDRV_CopyToBufferInternal(pDestBuff, pSrcAddr, nSize);  
        
            pDestAddr = HMDRV_pGetInternalMemoryAddressWrite(nDestAddr, nSize, &u16ExtMemID);
        
        pDestTest6 = pDestAddr;
        nDestTest6 = nDestAddr;
        pSrcTest6 = pSrcAddr;

        if ((pDestAddr != NULL) || (nDestAddr == NULL))
        {
            HMDRV_CopyFromBufferInternalWithMask(pDestAddr, pSrcAddr, pSrcBuff, nSize);

            
            if (pDestAddr != nDestAddr)
            {
                    ProcessExternalMemory(pDestAddr, nDestAddr, nSize, &u16ExtMemID);
            }
        }
        else
        {
            //ps = pSrcBuff;
            //ps += nSize;
        }
    
    }
    else
    {
        /* raise error implement */
        //pd = pDestBuff;
        //while(nSize--) *pd++ = 0;   /* copy 0 data to Buffer on error */
        
        //ps = pSrcBuff;
        //ps += nSize;
    }
        #if HMDRV_USE_EXT_MEM
    #if HMDRV_USE_EXT_PMP
    PMPMEM_vReleaseQueueBuffer((HMDRV_U8*)(HMDRV_ADDR_SIZE)pSrcAddr);
    #endif
        #if HMDRV_USE_EXT_SPI
        SPIMEM_vReleaseQueueBuffer(&u16ExtMemIDRead);
        #endif
        #endif

        nDestAddr+= nSizeRead;        
    }

    //return (HMDRV_BPTR) pd;

    
    //return (HMDRV_BPTR) ps;
}

#define DEBUG_BYTE_ARRAY_READ_LEN   10
volatile HMDRV_U8 u8DebugDataArrayRead[DEBUG_BYTE_ARRAY_READ_LEN];

volatile HMDRV_ADDR u32DebugAddressRead;
volatile HMDRV_ADDR u32DebugAddressReadMin;
volatile HMDRV_ADDR u32DebugAddressReadMax;
volatile HMDRV_U8 u8DebugDataLowRead;
volatile HMDRV_U8 u8DebugDataHighRead;

#if 0  //was before 2020 01 05
/* *****************************************************************************
 * HMDRV_CopyToBuffer
 * 
 * Description: memcpy function when used external memory external module
 **************************************************************************** */
HMDRV_BPTR HMDRV_CopyToBuffer(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_U8* pd;
    HMDRV_ADDR pSrcAddr;
    
    pSrcAddr = HMDRV_pGetInternalMemoryAddressRead(nSrcAddr, nSize);
    
    if ((pSrcAddr != NULL) || (nSrcAddr == NULL))
    {
        pd = HMDRV_CopyToBufferInternal(pDestBuff, (HMDRV_ADDR)pSrcAddr, nSize);
    }
    else
    {
        /* raise error implement */
        pd = pDestBuff;
        while(nSize--) *pd++ = 0;   /* copy 0 data to Buffer on error */
        
        //pd = NULL;
    }
    #if HMDRV_USE_EXT_PMP
    PMPMEM_vReleaseQueueBuffer((HMDRV_U8*)(HMDRV_ADDR_SIZE)pSrcAddr);
    #endif
    
    return (HMDRV_BPTR) pd;
}

#endif
/* *****************************************************************************
 * HMDRV_CopyToBuffer
 * 
 * Description: memcpy function when used external memory external module
 **************************************************************************** */
HMDRV_BPTR HMDRV_CopyToBuffer(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_U8* pd = NULL;
    volatile HMDRV_ADDR pSrcAddr;
    HMDRV_SIZE8 nSizeRead;
    
    HMDRV_SIZE8 nSizeReq = nSize;
    
    HMDRV_U16 u16ExtMemIDRead = 0xFFFF;
    HMDRV_U8* pDebug = NULL;
    
    volatile HMDRV_U16 u16IndexDebug = 0;
    
    while(nSizeReq)
    {    
        pSrcAddr = HMDRV_pGetInternalMemoryAddressRead(nSrcAddr, nSizeReq, &nSizeRead, &u16ExtMemIDRead);
        nSizeReq -= nSizeRead;
        nSize = nSizeRead;
        u16IndexDebug = 0;


        if (pSrcAddr != (HMDRV_ADDR)(HMDRV_ADDR_SIZE)&HMDRV_CopyToBuffer_pSrcAddr)
        {
            HMDRV_CopyToBuffer_pSrcAddr = pSrcAddr;
        }
        if ((pSrcAddr != NULL) || (nSrcAddr == NULL))
        {
            pDestBuff = HMDRV_CopyToBufferInternal(pDestBuff, pSrcAddr, nSize);
            
            for (u16IndexDebug = 0; u16IndexDebug < DEBUG_BYTE_ARRAY_READ_LEN; u16IndexDebug++)
            {
                pDebug = (HMDRV_U8*)(HMDRV_ADDR_SIZE_SHORT)pSrcAddr + u16IndexDebug;
                u8DebugDataArrayRead[u16IndexDebug] = *pDebug;
            }
            if (((u32DebugAddressRead == nSrcAddr) || ((u32DebugAddressReadMin <= nSrcAddr) && (u32DebugAddressReadMax >= nSrcAddr)))
                    && (u8DebugDataArrayRead[1] == u8DebugDataHighRead) && (u8DebugDataArrayRead[0] == u8DebugDataLowRead))
            {
                u8DebugDataArrayRead[9] = 5;
            }
            
        }
        else
        {
            /* raise error implement */
            pd = pDestBuff;
            while(nSize--) *pd++ = 'E';   /* copy 'E' data to Buffer on error */
            pDestBuff = pd;
            //pd = NULL;
        }
        #if HMDRV_USE_EXT_MEM
        #if HMDRV_USE_EXT_PMP
        PMPMEM_vReleaseQueueBuffer((HMDRV_U8*)(HMDRV_ADDR_SIZE)pSrcAddr);
        #endif
        #if HMDRV_USE_EXT_SPI
        SPIMEM_vReleaseQueueBuffer(&u16ExtMemIDRead);
        #endif
        #endif

        nSrcAddr+= nSizeRead;        
    }
    pd = pDestBuff;
    return (HMDRV_BPTR) pd;
}




#if HMDRV_USE_MODBUS

/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/
HMDRV_BPTR HMDRV_CopyFromBufferModbus(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize, HMDRV_U8 u8ElementSize, HMDRV_U8* pu8ElementSizeArray, HMDRV_BOOL bStringNullTerminationPossible)
{
    HMDRV_U8* ps;
    HMDRV_ADDR pDestAddr;
    HMDRV_U16 u16ExtMemID;
    
    pDestAddr = HMDRV_pGetInternalMemoryAddressWrite(nDestAddr, nSize, &u16ExtMemID);
    
    if ((pDestAddr != NULL) || (nDestAddr == NULL))
    {
        ps = HMDRV_CopyFromBufferInternalModbus(pDestAddr, pSrcBuff, nSize, u8ElementSize, pu8ElementSizeArray, bStringNullTerminationPossible);
        
        if (pDestAddr != nDestAddr)
        {
            ProcessExternalMemory(pDestAddr, nDestAddr, nSize, &u16ExtMemID);
        }
    }
    else
    {
        ps = pSrcBuff;
        ps += nSize;
        
        ps = NULL;  /* return exception */
    }

    
    
    
    return (HMDRV_BPTR) ps;
}

/* *****************************************************************************
 * HMDRV_CopyFromBufferModbusCoil
 * 
 * Description: memcpy function when used external memory external module
 **************************************************************************** */
HMDRV_BPTR HMDRV_CopyFromBufferModbusCoil(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nBitOffset, HMDRV_SIZE8 nBitOffsetDest)
{
    HMDRV_U8* ps;
    HMDRV_ADDR pDestAddr;
    HMDRV_U16 u16ExtMemID;
    
    pDestAddr = HMDRV_pGetInternalMemoryAddressWrite(nDestAddr, 1, &u16ExtMemID);
    
    if ((pDestAddr != NULL) || (nDestAddr == NULL))
    {
        ps = HMDRV_CopyFromBufferInternalModbusCoil(pDestAddr, pSrcBuff, nBitOffset, nBitOffsetDest);
            
        if (pDestAddr != nDestAddr)
        {
            ProcessExternalMemory(pDestAddr, nDestAddr, 1, &u16ExtMemID);
        }
    }
    else
    {
        ps = NULL;  /* return exception */
    }
    return (HMDRV_BPTR) ps;
}


/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/
HMDRV_BPTR HMDRV_CopyFromBufferModbusSingleCoil(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nBitOffset)
{
    HMDRV_U8* ps;
    HMDRV_ADDR pDestAddr;
    HMDRV_U16 u16ExtMemID;
    
    pDestAddr = HMDRV_pGetInternalMemoryAddressWrite(nDestAddr, 1, &u16ExtMemID);
    
    if ((pDestAddr != NULL) || (nDestAddr == NULL))
    {
        ps = HMDRV_CopyFromBufferInternalModbusSingleCoil(pDestAddr, pSrcBuff, nBitOffset);
            
        if (pDestAddr != nDestAddr)
        {
            ProcessExternalMemory(pDestAddr, nDestAddr, 1, &u16ExtMemID);
        }
    }
    else
    {
        ps = NULL;  /* return exception */
    }
    return (HMDRV_BPTR) ps;
}





/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer, perform AND-masking
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - source memory in communication buffer, mask follows data
* @param  nSize     - buffer size (always in bytes)
*
******************************************************************************/

HMDRV_BOOL HMDRV_CopyFromBufferWithMaskModbus(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
    HMDRV_BOOL result = HMDRV_FALSE;
    //HMDRV_U8* ps;
    //HMDRV_U8* pd;
    HMDRV_ADDR pDestAddr;
    HMDRV_ADDR pSrcAddr;
    HMDRV_ADDR nSrcAddr;
    HMDRV_BPTR pDestBuff;
    HMDRV_SIZE8 nSizeRead;
    HMDRV_U16 u16ExtMemID;
    HMDRV_U16 u16ExtMemIDRead = 0xFFFF;
   
    HMDRV_SIZE8 nSizeReq = nSize;
    while(nSizeReq)
    {    
        pDestAddr = HMDRV_pGetInternalMemoryAddressRead(nDestAddr, nSizeReq, &nSizeRead, &u16ExtMemIDRead);        
        nSizeReq -= nSizeRead;
        nSize = nSizeRead;
    
    pDestBuff = pSrcBuff;
    pSrcAddr = pDestAddr;
    nSrcAddr = nDestAddr;
    
    if ((pSrcAddr != NULL) || (nSrcAddr == NULL))
    {
        //pd = HMDRV_CopyToBufferInternal(pDestBuff, pSrcAddr, nSize);  
        
            pDestAddr = HMDRV_pGetInternalMemoryAddressWrite(nDestAddr, nSize, &u16ExtMemID);
        
        //pDestTest6 = pDestAddr;
       // nDestTest6 = nDestAddr;
        //pSrcTest6 = pSrcAddr;

        if ((pDestAddr != NULL) || (nDestAddr == NULL))
        {
            HMDRV_CopyFromBufferInternalWithMaskModbus(pDestAddr, pSrcAddr, pSrcBuff, nSize);   

            if (pDestAddr != nDestAddr)
            {
                    ProcessExternalMemory(pDestAddr, nDestAddr, nSize, &u16ExtMemID);
            }
            result = HMDRV_TRUE;
        }
        else
        {
            //ps = pSrcBuff;
            //ps += nSize;
        }
    
    }
    else
    {
        /* raise error implement */
        //pd = pDestBuff;
        //while(nSize--) *pd++ = 0;   /* copy 0 data to Buffer on error */
        
        //ps = pSrcBuff;
        //ps += nSize;
    }

        #if HMDRV_USE_EXT_MEM
    #if HMDRV_USE_EXT_PMP
    PMPMEM_vReleaseQueueBuffer((HMDRV_U8*)(HMDRV_ADDR_SIZE)pSrcAddr);
    #endif
        #if HMDRV_USE_EXT_SPI
        SPIMEM_vReleaseQueueBuffer(&u16ExtMemIDRead);
        #endif
        #endif

        nDestAddr+= nSizeRead;        

    }
    
    //return (HMDRV_BPTR) pd;

    
    //return (HMDRV_BPTR) ps;
    
    return result;
}




/* *****************************************************************************
 * HMDRV_CopyToBufferModbus
 * 
 * Description: memcpy function when used external memory external module
 **************************************************************************** */
HMDRV_BPTR HMDRV_CopyToBufferModbus(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize, HMDRV_U8 u8ElementSize, HMDRV_U8* pu8ElementSizeArray)
//HMDRV_BPTR HMDRV_CopyToBufferModbus(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize, HMDRV_BOOL bStringDataExpected)
{
    volatile HMDRV_U8* pd = NULL;
    HMDRV_ADDR pSrcAddr;
    HMDRV_SIZE8 nSizeRead;
    
    HMDRV_SIZE8 nSizeReq = nSize;
    HMDRV_U16 u16ExtMemIDRead = 0xFFFF;
    
    while(nSizeReq)
    {    
        pSrcAddr = HMDRV_pGetInternalMemoryAddressRead(nSrcAddr, nSizeReq, &nSizeRead, &u16ExtMemIDRead);
        nSizeReq -= nSizeRead;
        nSize = nSizeRead;
    
    if ((pSrcAddr != NULL) || (nSrcAddr == NULL))
    {
            pDestBuff = HMDRV_CopyToBufferInternalModbus(pDestBuff, (HMDRV_ADDR)pSrcAddr, nSize, u8ElementSize, pu8ElementSizeArray);
    }
    else
    {
        /* raise error implement */
        pd = pDestBuff;
        while(nSize--) *pd++ = 0;   /* copy 0 data to Buffer on error */
        
            pDestBuff = NULL;  /* return exception */
        }

        #if HMDRV_USE_EXT_MEM
        #if HMDRV_USE_EXT_PMP
        PMPMEM_vReleaseQueueBuffer((HMDRV_U8*)(HMDRV_ADDR_SIZE)pSrcAddr);
        #endif
        #if HMDRV_USE_EXT_SPI
        SPIMEM_vReleaseQueueBuffer(&u16ExtMemIDRead);
        #endif
        #endif

        nSrcAddr+= nSizeRead;        
    }
    
    pd = pDestBuff;
    
    return (HMDRV_BPTR) pd;
}


/* *****************************************************************************
 * HMDRV_CopyToBufferModbusCoil
 * 
 * Description: memcpy function when used external memory external module
 **************************************************************************** */
HMDRV_BPTR HMDRV_CopyToBufferModbusCoil(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nBitOffset, HMDRV_SIZE8 nBitOffsetDest)
{
    volatile HMDRV_U8* pd;
    HMDRV_ADDR pSrcAddr;
    HMDRV_SIZE8 nSizeRead;
    HMDRV_SIZE8 nSize = 1;
    
    HMDRV_SIZE8 nSizeReq = nSize;
    HMDRV_U16 u16ExtMemIDRead = 0xFFFF;
    
    while(nSizeReq)
    {    
        pSrcAddr = HMDRV_pGetInternalMemoryAddressRead(nSrcAddr, nSizeReq, &nSizeRead, &u16ExtMemIDRead);
        nSizeReq -= nSizeRead;
        nSize = nSizeRead;

        if ((pSrcAddr != NULL) || (nSrcAddr == NULL))
        {
            pDestBuff = HMDRV_CopyToBufferInternalModbusCoil(pDestBuff, (HMDRV_ADDR)pSrcAddr, nBitOffset, nBitOffsetDest);
        }
        else
        {
            /* raise error implement */
            pd = pDestBuff;
            *pd = 0;   /* copy 0 data to Buffer on error */

            pDestBuff = NULL;  /* return exception */
        }

        #if HMDRV_USE_EXT_MEM
        #if HMDRV_USE_EXT_PMP
        PMPMEM_vReleaseQueueBuffer((HMDRV_U8*)(HMDRV_ADDR_SIZE)pSrcAddr);
        #endif
        #if HMDRV_USE_EXT_SPI
        SPIMEM_vReleaseQueueBuffer(&u16ExtMemIDRead);
        #endif
        #endif

        nSrcAddr += nSizeRead;
    }
    pd = pDestBuff;
    return (HMDRV_BPTR) pd;
}


/* *****************************************************************************
 * HMDRV_CopyToBufferModbusCoil
 * 
 * Description: memcpy function when used external memory external module
 **************************************************************************** */
HMDRV_BPTR HMDRV_CopyToBufferModbusSingleCoil(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nBitOffset)
{
    volatile HMDRV_U8* pd;
    HMDRV_ADDR pSrcAddr;
    HMDRV_SIZE8 nSizeRead;
    HMDRV_SIZE8 nSize = 1;
    
    HMDRV_SIZE8 nSizeReq = nSize;
    HMDRV_U16 u16ExtMemIDRead = 0xFFFF;
    
    while(nSizeReq)
    {    
        pSrcAddr = HMDRV_pGetInternalMemoryAddressRead(nSrcAddr, nSizeReq, &nSizeRead, &u16ExtMemIDRead);
        nSizeReq -= nSizeRead;
        nSize = nSizeRead;

        if ((pSrcAddr != NULL) || (nSrcAddr == NULL))
        {
            pDestBuff = HMDRV_CopyToBufferInternalModbusSingleCoil(pDestBuff, (HMDRV_ADDR)pSrcAddr, nBitOffset);
        }
        else
        {
            /* raise error implement */
            pd = pDestBuff;
            *pd = 0;   /* copy 0 data to Buffer on error */

            pDestBuff = NULL;  /* return exception */
        }

        #if HMDRV_USE_EXT_MEM
        #if HMDRV_USE_EXT_PMP
        PMPMEM_vReleaseQueueBuffer((HMDRV_U8*)(HMDRV_ADDR_SIZE)pSrcAddr);
        #endif
        #if HMDRV_USE_EXT_SPI
        SPIMEM_vReleaseQueueBuffer(&u16ExtMemIDRead);
        #endif
        #endif

        nSrcAddr += nSizeRead;
    }
    pd = pDestBuff;
    return (HMDRV_BPTR) pd;
}


/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/

HMDRV_BPTR HMDRV_CopyFromBufferWithSuccess(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
    HMDRV_U8* ps;
    HMDRV_ADDR pDestAddr;
    HMDRV_U16 u16ExtMemID;
    
    pDestAddr = HMDRV_pGetInternalMemoryAddressWrite(nDestAddr, nSize, &u16ExtMemID);
    
    if ((pDestAddr != NULL) || (nDestAddr == NULL))
    {
        ps = HMDRV_CopyFromBufferInternal(pDestAddr, pSrcBuff, nSize);
        
        if (pDestAddr != nDestAddr)
        {
            ProcessExternalMemory(pDestAddr, nDestAddr, nSize, &u16ExtMemID);
        }
    }
    else
    {
        ps = pSrcBuff;
        ps += nSize;
        
        ps = NULL;
    }

    
    
    
    return (HMDRV_BPTR) ps;
}


/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/

HMDRV_BPTR HMDRV_CopyFromBufferWithSuccessCoil(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff, HMDRV_SIZE8 nSize)
{
    HMDRV_U8* ps;
    HMDRV_ADDR pDestAddr;
    HMDRV_U16 u16ExtMemID;
    
    pDestAddr = HMDRV_pGetInternalMemoryAddressWrite(nDestAddr, nSize, &u16ExtMemID);
    
    if ((pDestAddr != NULL) || (nDestAddr == NULL))
    {
        ps = HMDRV_CopyFromBufferInternalCoil(pDestAddr, pSrcBuff, nSize);
        
        if (pDestAddr != nDestAddr)
        {
            ProcessExternalMemory(pDestAddr, nDestAddr, nSize, &u16ExtMemID);
        }
    }
    else
    {
        ps = pSrcBuff;
        ps += nSize;
        
        ps = NULL;
    }

    
    
    
    return (HMDRV_BPTR) ps;
}



/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/

HMDRV_BPTR HMDRV_CopyFromBufferWithSuccessSingleCoil(HMDRV_ADDR nDestAddr, HMDRV_BPTR pSrcBuff)
{
    HMDRV_U8* ps;
    HMDRV_ADDR pDestAddr;
    HMDRV_U16 u16ExtMemID;
    
    pDestAddr = HMDRV_pGetInternalMemoryAddressWrite(nDestAddr, 1, &u16ExtMemID);
    
    if ((pDestAddr != NULL) || (nDestAddr == NULL))
    {
        ps = HMDRV_CopyFromBufferInternalSingleCoil(pDestAddr, pSrcBuff);
        
        if (pDestAddr != nDestAddr)
        {
            ProcessExternalMemory(pDestAddr, nDestAddr, 1, &u16ExtMemID);
        }
    }
    else
    {
        ps = NULL;
    }

    return (HMDRV_BPTR) ps;
}





/* *****************************************************************************
 * HMDRV_CopyToBuffer
 * 
 * Description: memcpy function when used external memory external module
 **************************************************************************** */
HMDRV_BPTR HMDRV_CopyToBufferWithSuccess(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_U8* pd = NULL;
    HMDRV_ADDR pSrcAddr;
    HMDRV_SIZE8 nSizeRead;
    
    HMDRV_SIZE8 nSizeReq = nSize;
    HMDRV_U16 u16ExtMemIDRead = 0xFFFF;
    
    while(nSizeReq)
    {    
        pSrcAddr = HMDRV_pGetInternalMemoryAddressRead(nSrcAddr, nSizeReq, &nSizeRead, &u16ExtMemIDRead);
        nSizeReq -= nSizeRead;
        nSize = nSizeRead;

        if ((pSrcAddr != NULL) || (nSrcAddr == NULL))
        {
            pDestBuff = HMDRV_CopyToBufferInternal(pDestBuff, pSrcAddr, nSize);
        }
        else
        {
            /* raise error implement */
            pd = pDestBuff;
            while(nSize--) *pd++ = 0;   /* copy 0 data to Buffer on error */

            pDestBuff = NULL;
        }
        #if HMDRV_USE_EXT_MEM
        #if HMDRV_USE_EXT_PMP
        PMPMEM_vReleaseQueueBuffer((HMDRV_U8*)(HMDRV_ADDR_SIZE)pSrcAddr);
        #endif
        #if HMDRV_USE_EXT_SPI
        SPIMEM_vReleaseQueueBuffer(&u16ExtMemIDRead);
        #endif
        #endif

        nSrcAddr += nSizeRead;
    }
    pd = pDestBuff;
    return (HMDRV_BPTR) pd;
}


/* *****************************************************************************
 * HMDRV_CopyToBuffer
 * 
 * Description: memcpy function when used external memory external module
 **************************************************************************** */
HMDRV_BPTR HMDRV_CopyToBufferWithSuccessCoil(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_U8* pd = NULL;
    HMDRV_ADDR pSrcAddr;
    HMDRV_SIZE8 nSizeRead;
    
    HMDRV_SIZE8 nSizeReq = nSize;
    HMDRV_U16 u16ExtMemIDRead = 0xFFFF;
    
    while(nSizeReq)
    {    
        pSrcAddr = HMDRV_pGetInternalMemoryAddressRead(nSrcAddr, nSizeReq, &nSizeRead, &u16ExtMemIDRead);
        nSizeReq -= nSizeRead;
        nSize = nSizeRead;

        if ((pSrcAddr != NULL) || (nSrcAddr == NULL))
        {
            pDestBuff = HMDRV_CopyToBufferInternalCoil(pDestBuff, pSrcAddr, nSize);
        }
        else
        {
            /* raise error implement */
            pd = pDestBuff;
            while(nSize--) *pd++ = 0;   /* copy 0 data to Buffer on error */

            pDestBuff = NULL;
        }

        #if HMDRV_USE_EXT_MEM
        #if HMDRV_USE_EXT_PMP
        PMPMEM_vReleaseQueueBuffer((HMDRV_U8*)(HMDRV_ADDR_SIZE)pSrcAddr);
        #endif
        #if HMDRV_USE_EXT_SPI
        SPIMEM_vReleaseQueueBuffer(&u16ExtMemIDRead);
        #endif
        #endif

        nSrcAddr += nSizeRead;
    }
    pd = pDestBuff;
    return (HMDRV_BPTR) pd;
}



/* *****************************************************************************
 * HMDRV_CopyToBuffer
 * 
 * Description: memcpy function when used external memory external module
 **************************************************************************** */
HMDRV_BPTR HMDRV_CopyToBufferWithSuccessSingleCoil(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr)
{
    HMDRV_U8* pd;
    HMDRV_ADDR pSrcAddr;
    HMDRV_SIZE8 nSizeRead;
    HMDRV_SIZE8 nSize = 1;
    
    HMDRV_SIZE8 nSizeReq = nSize;
    HMDRV_U16 u16ExtMemIDRead = 0xFFFF;
    
    while(nSizeReq)
    {    
        pSrcAddr = HMDRV_pGetInternalMemoryAddressRead(nSrcAddr, nSizeReq, &nSizeRead, &u16ExtMemIDRead);
        nSizeReq -= nSizeRead;
        nSize = nSizeRead;

        if ((pSrcAddr != NULL) || (nSrcAddr == NULL))
        {
            pDestBuff = HMDRV_CopyToBufferInternalSingleCoil(pDestBuff, pSrcAddr);
        }
        else
        {
            /* raise error implement */
            pd = pDestBuff;
            *pd++ = 0;   /* copy 0 data to Buffer on error */
            *pDestBuff   = 0;   /* copy 0 data to Buffer on error */

            pd = NULL;
        }
        
        #if HMDRV_USE_EXT_MEM
        #if HMDRV_USE_EXT_PMP
        PMPMEM_vReleaseQueueBuffer((HMDRV_U8*)(HMDRV_ADDR_SIZE)pSrcAddr);
        #endif
        #if HMDRV_USE_EXT_SPI
        SPIMEM_vReleaseQueueBuffer(&u16ExtMemIDRead);
        #endif
        #endif

        nSrcAddr += nSizeRead;
    }
    pd = pDestBuff;
    return (HMDRV_BPTR) pd;
}






#endif           /* #if HMDRV_USE_MODBUS */




/* *****************************************************************************
 * HMDRV_GetStringLength
 * 
 * Description: strlen function when used external memory external module
 **************************************************************************** */
HMDRV_SIZE8 HMDRV_GetStringLength(HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_ADDR pSrcAddr;
    
    //HMDRV_SIZE8 nSizeFbck = nSize;
    HMDRV_SIZE8 nSizeFbckLoop = 0;
    HMDRV_SIZE8 nSizeRead;
    
    HMDRV_SIZE8 nSizeReq = nSize;
    HMDRV_U16 u16ExtMemIDRead = 0xFFFF;
    
    while(nSizeReq)
    {    
        pSrcAddr = HMDRV_pGetInternalMemoryAddressRead(nSrcAddr, nSizeReq, &nSizeRead, &u16ExtMemIDRead);
        nSizeReq -= nSizeRead;
        nSize = nSizeRead;

        if ((pSrcAddr != NULL) || (nSrcAddr == NULL))
        {
            nSize = HMDRV_GetStringLengthInternal((HMDRV_ADDR)pSrcAddr, nSize);
            nSizeFbckLoop += nSize;
        }

        #if HMDRV_USE_EXT_MEM
        #if HMDRV_USE_EXT_PMP
        PMPMEM_vReleaseQueueBuffer((HMDRV_U8*)(HMDRV_ADDR_SIZE)pSrcAddr);
        #endif
        #if HMDRV_USE_EXT_SPI
        SPIMEM_vReleaseQueueBuffer(&u16ExtMemIDRead);
        #endif
        #endif

        nSrcAddr += nSizeRead;
    }
    return nSizeFbckLoop;
}


#if 0 //was before 2020 01 05
/* *****************************************************************************
 * HMDRV_CopyToBuffer
 * 
 * Description: memcpy function when used external memory external module
 **************************************************************************** */
HMDRV_BPTR HMDRV_CopyToBufferWithSuccess(HMDRV_BPTR pDestBuff, HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize)
{
    HMDRV_U8* pd;
    HMDRV_ADDR pSrcAddr;
    
    pSrcAddr = HMDRV_pGetInternalMemoryAddressRead(nSrcAddr, nSize);
    
    if ((pSrcAddr != NULL) || (nSrcAddr == NULL))
    {
        pd = HMDRV_CopyToBufferInternal(pDestBuff, pSrcAddr, nSize);
    }
    else
    {
        /* raise error implement */
        pd = pDestBuff;
        while(nSize--) *pd++ = 0;   /* copy 0 data to Buffer on error */
        
        pd = NULL;
    }
    #if HMDRV_USE_EXT_PMP
    PMPMEM_vReleaseQueueBuffer((HMDRV_U8*)(HMDRV_ADDR_SIZE)pSrcAddr);
    #endif

 
    return (HMDRV_BPTR) pd;
}
#endif

HMDRV_BOOL HMDRV_bIsExternalAddress(HMDRV_ADDR nDestAddr)
{
    HMDRV_BOOL bResult = HMDRV_FALSE;
    if (nDestAddr >= EXT_MEM_MIN_VIRTUAL_ADDRESS)
    {
        bResult = HMDRV_TRUE;
    }
    return bResult;
}

HMDRV_U8*  HMDRV_pSnapshotAddressGet(HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize)
{
    if (nDestAddr >= EXT_MEM_SPI_MIN_VIRTUAL_ADDRESS)
    {
        #if HMDRV_USE_EXT_SPI
        return SPIMEM_pBufferAddressGet(nDestAddr);
        #else
        return NULL;
        #endif
    }
    else
    {
        #if HMDRV_USE_EXT_PMP
        return PMPMEM_pBufferAddressGet(nDestAddr, nSize);
        #else
        return NULL;
        #endif
    }
    
}

void HMDRV_CopySnapshotToMemory(HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize)
{
    if (nDestAddr >= EXT_MEM_SPI_MIN_VIRTUAL_ADDRESS)
    {
        #if HMDRV_USE_EXT_SPI
        SPIMEM_vSendBufferToSPI(nSize);
        #endif

    }
    else
    {
        #if HMDRV_USE_EXT_PMP
        PMPMEM_vSendBufferToPMP(nSize);
        #endif
    }
}





/* *****************************************************************************
 * HMDRV_pGetInternalMemoryAddressRead
 *
 * Description: convert virtual to real address
 **************************************************************************** */
HMDRV_ADDR HMDRV_pGetInternalMemoryAddressRead(HMDRV_ADDR nSrcAddr, HMDRV_SIZE8 nSize, HMDRV_ADDR_PNTR_MODIFIER HMDRV_SIZE8* pnSizeRead, HMDRV_U16* pu16ID)
{
    HMDRV_ADDR pSrcAddr;
    
    *pnSizeRead = nSize;
    if (nSrcAddr >= EXT_MEM_MIN_VIRTUAL_ADDRESS)
    {
        if (nSrcAddr >= EXT_MEM_SPI_MIN_VIRTUAL_ADDRESS)
        {
            #if HMDRV_USE_EXT_SPI
            pSrcAddr = (HMDRV_ADDR)(HMDRV_ADDR_SIZE_SHORT)SPIMEM_pExtMemRamToIntRam(nSrcAddr, nSize, pnSizeRead, pu16ID);
            #else
            pSrcAddr = NULL;
            #endif
        }
        else
        {
            #if USE_SPI_FLIR
            if (nSrcAddr >= EXT_MEM_FLIR_CCI_TWI_VIRTUAL_ADDRESS)
            {
                #if HMDRV_USE_EXT_I2C
                pSrcAddr = (HMDRV_ADDR)(HMDRV_ADDR_SIZE_SHORT)I2CMEM_pFlirRegToIntRam(nSrcAddr, nSize);
                #else
                pSrcAddr = NULL;
                #endif
            }
            else
            #endif
            if (nSrcAddr >= EXT_MEM_I2C_VIRTUAL_ADDRESS)
            {
                #if HMDRV_USE_EXT_I2C
                //was pSrcAddr = (HMDRV_ADDR)(HMDRV_ADDR_SIZE_SHORT)I2CMEM_pExtMemRamToIntRam(nSrcAddr, nSize);
                pSrcAddr = (HMDRV_ADDR)I2CMEM_pExtMemRamToIntRam(nSrcAddr, nSize);
                #else
                pSrcAddr = NULL;
                #endif
            }
            else
            {
                #if HMDRV_USE_EXT_PMP
                pSrcAddr = (HMDRV_ADDR)(HMDRV_ADDR_SIZE_SHORT)PMPMEM_pExtMemRamToIntRam(nSrcAddr, nSize);
                #else
                pSrcAddr = NULL;
                #endif
            }
        }
    }
    else
    {
        pSrcAddr = nSrcAddr;
    }
    return pSrcAddr;
}




/* *****************************************************************************
 * HMDRV_pGetInternalMemoryAddressWrite
 *
 * Description: convert virtual to real address
 **************************************************************************** */
HMDRV_ADDR HMDRV_pGetInternalMemoryAddressWrite(HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize, HMDRV_U16* pu16ID)
{
    HMDRV_ADDR pDestAddr;
    
    if (nDestAddr >= EXT_MEM_MIN_VIRTUAL_ADDRESS)
    {
        if (nDestAddr >= EXT_MEM_SPI_MIN_VIRTUAL_ADDRESS)
        {
            #if HMDRV_USE_EXT_SPI
            pDestAddr = (HMDRV_ADDR)(HMDRV_ADDR_SIZE_SHORT)SPIMEM_pExtMemRamFromIntRam(nDestAddr, nSize, pu16ID);
            #else
            pDestAddr = NULL;   /* Error Address */
            #endif

        }
        else
        {
            if (nDestAddr >= EXT_MEM_I2C_VIRTUAL_ADDRESS)
            {
                #if HMDRV_USE_EXT_I2C
                #if USE_SPI_FLIR
                if (nDestAddr >= EXT_MEM_FLIR_CCI_TWI_VIRTUAL_ADDRESS)
                {
                    pDestAddr = (HMDRV_ADDR)(HMDRV_ADDR_SIZE_SHORT)I2CMEM_pFlirRegFromIntRam();
                }
                else
                #endif
                {
                    pDestAddr = I2CMEM_pExtMemRamFromIntRam();
                }
                #else
                pDestAddr = NULL;   /* Error Address */
                #endif
            }
            else
            {
                #if HMDRV_USE_EXT_PMP
                pDestAddr = (HMDRV_ADDR)(HMDRV_ADDR_SIZE_SHORT)PMPMEM_pExtMemRamFromIntRam(nSize);
                #else
                pDestAddr = NULL;   /* Error Address */
                #endif

            }
        }
    }
    else
    {
        pDestAddr = nDestAddr;
    }
    return pDestAddr;
}


/* *****************************************************************************
 * ProcessExternalMemory
 *
 * Description: Update data in Extended memory area
 *
 **************************************************************************** */
void ProcessExternalMemory(HMDRV_ADDR nInternalRamBufferAddr, HMDRV_ADDR nDestAddr, HMDRV_SIZE8 nSize, HMDRV_U16* pu16ID)
{
    if (nDestAddr >= EXT_MEM_MIN_VIRTUAL_ADDRESS)
    {
        if (nDestAddr >= EXT_MEM_SPI_MIN_VIRTUAL_ADDRESS)
        {
            #if HMDRV_USE_EXT_SPI
            SPIMEM_vExtMemRamFromIntRamCopyData(pu16ID);
            #endif
        }
        else
        {
            
            #if USE_SPI_FLIR
            if (nDestAddr >= EXT_MEM_FLIR_CCI_TWI_VIRTUAL_ADDRESS)
            {
                #if HMDRV_USE_EXT_I2C
                I2CMEM_vFlirRegFromIntRamCopyData(nDestAddr, nSize);
                #endif
            }
            else
            #endif
            if (nDestAddr >= EXT_MEM_I2C_VIRTUAL_ADDRESS)
            {
                #if HMDRV_USE_EXT_I2C
                I2CMEM_vExtMemRamFromIntRamCopyData(nDestAddr, nSize);
                #endif
            }
            else
            {
                #if HMDRV_USE_EXT_PMP
                PMPMEM_vExtMemRamFromIntRamCopyData(nInternalRamBufferAddr, nDestAddr, nSize);
                #endif
            }
        }
    }
}






#endif      /* #if HMDRV_USE_EXT_MEM == 1 */




/* *****************************************************************************
 * EX Commands Handling (Extended Address Commands)
 **************************************************************************** */

/* Used EX Commands? */
#if (HMDRV_USE_EX_CMDS)

/* mixed EX and no-EX commands? */
#if (HMDRV_USE_NOEX_CMDS)

/**************************************************************************//*!
*
* @brief  When mixed EX and no-EX command may occur, this variable is
*         here to remember what command is just being handled.
*
******************************************************************************/

static HMDRV_BOOL pcm_bNextAddrIsEx;

void HMDRV_SetExAddr(HMDRV_BOOL bNextAddrIsEx)
{
    pcm_bNextAddrIsEx = bNextAddrIsEx;
}

/* only no-EX commands (we are in LARGE model) */
#else

/**************************************************************************//*!
*
* @brief  When mixed EX and no-EX commands are disabled, we are sure
*         only the EX are used (we are in LARGE mode here).
*
******************************************************************************/

static const HMDRV_BOOL pcm_bNextAddrIsEx = 1;

#endif /* mixed EX and no-EX commands */



/**************************************************************************//*!
*
* @brief  Store (global) address to communication buffer
*
******************************************************************************/

HMDRV_BPTR HMDRV_AddressToBufferBigEndian(HMDRV_BPTR pDest, HMDRV_ADDR nAddr)
{
    if(pcm_bNextAddrIsEx)
    {
        //*(HMDRV_U32*) pDest = ((HMDRV_U32)nAddr);
        *pDest++ = (HMDRV_U8)((HMDRV_U8)((HMDRV_U32)nAddr>>24) & 0x00FF); 
        *pDest++ = (HMDRV_U8)((HMDRV_U8)((HMDRV_U32)nAddr>>16) & 0x00FF); 
        *pDest++ = (HMDRV_U8)((HMDRV_U8)((HMDRV_U32)nAddr>> 8) & 0x00FF); 
        *pDest++ = (HMDRV_U8)((HMDRV_U8)((HMDRV_U32)nAddr>> 0) & 0x00FF); 
    }
    else
    {
        //*(HMDRV_U16*) pDest = ((HMDRV_U16)nAddr);
        *pDest++ = (HMDRV_U8)((HMDRV_U8)((HMDRV_U16)(HMDRV_ADDR_SIZE_LONG)nAddr>> 8) & 0x00FF);
        *pDest++ = (HMDRV_U8)((HMDRV_U8)((HMDRV_U16)(HMDRV_ADDR_SIZE_LONG)nAddr>> 0) & 0x00FF);
    }
    
    return pDest;
}

/**************************************************************************//*!
*
* @brief  Store (global) address to communication buffer
*
******************************************************************************/

HMDRV_BPTR HMDRV_AddressToBuffer(HMDRV_BPTR pDest, HMDRV_ADDR nAddr)
{
#if 0       //HMDRV_BIG_TO_LITTLE_ENDIAN
    //first work with TI ??? same as below???
    HMDRV_U32 addr = (HMDRV_U32)nAddr;
    //addr <<= 1;
    *pDest++ = ((HMDRV_U8)(addr >>  0)) & 0xFF;
    *pDest++ = ((HMDRV_U8)(addr >>  8)) & 0xFF;
    *pDest++ = ((HMDRV_U8)(addr >> 16)) & 0xFF;
    *pDest++ = ((HMDRV_U8)(addr >> 24)) & 0xFF;
#else
    if(pcm_bNextAddrIsEx)
    {
        //*(HMDRV_U32*) pDest = ((HMDRV_U32)nAddr);
        *pDest++ = (HMDRV_U8)((HMDRV_U8)((HMDRV_U32)nAddr>> 0) & 0x00FF); 
        *pDest++ = (HMDRV_U8)((HMDRV_U8)((HMDRV_U32)nAddr>> 8) & 0x00FF); 
        *pDest++ = (HMDRV_U8)((HMDRV_U8)((HMDRV_U32)nAddr>>16) & 0x00FF); 
        *pDest++ = (HMDRV_U8)((HMDRV_U8)((HMDRV_U32)nAddr>>24) & 0x00FF); 
    }
    else
    {
        //*(HMDRV_U16*) pDest = ((HMDRV_U16)nAddr);
        *pDest++ = (HMDRV_U8)((HMDRV_U8)((HMDRV_U16)(HMDRV_ADDR_SIZE_LONG)nAddr>> 0) & 0x00FF);
        *pDest++ = (HMDRV_U8)((HMDRV_U8)((HMDRV_U16)(HMDRV_ADDR_SIZE_LONG)nAddr>> 8) & 0x00FF);
    }
#endif

    return pDest;
}

/**************************************************************************//*!
*
* @brief  Fetch address from communication buffer
*
******************************************************************************/

HMDRV_BPTR HMDRV_AddressFromBufferBigEndian(HMDRV_ADDR_PNTR_MODIFIER HMDRV_ADDR* pAddr, HMDRV_BPTR pSrc)
{
    /* treat address as long word to determine the kind of the address */
    HMDRV_U32 addr = 0;

    if(pcm_bNextAddrIsEx)
    {
        addr = (HMDRV_U32)(
                ((HMDRV_U32)(*(HMDRV_U8*)(pSrc+0))<<24) + 
                ((HMDRV_U32)(*(HMDRV_U8*)(pSrc+1))<<16) + 
                ((HMDRV_U32)(*(HMDRV_U8*)(pSrc+2))<< 8) + 
                ((HMDRV_U32)(*(HMDRV_U8*)(pSrc+3))<< 0));
        pSrc += 4;
    }
    else
    {
        
        addr = (HMDRV_U32)(
                ((HMDRV_U16)(*(HMDRV_U8*)(pSrc+0))<< 8) + 
                ((HMDRV_U16)(*(HMDRV_U8*)(pSrc+1))<< 0));

        pSrc += 2;
    }
    
    *pAddr = (HMDRV_ADDR)addr;
    
    return pSrc;
}

/**************************************************************************//*!
*
* @brief  Fetch address from communication buffer
*
******************************************************************************/

HMDRV_BPTR HMDRV_AddressFromBuffer(HMDRV_ADDR_PNTR_MODIFIER HMDRV_ADDR* pAddr, HMDRV_BPTR pSrc)
{
    HMDRV_U32 addr = 0;
#if HMDRV_USE_BYTE_ADDRESSING_IN_16BIT_MEMORY
     HMDRV_U32 src = 0;

     src = *pSrc++;
     src &= 0x000000FF;
     addr =  src;
     src = *pSrc++;
     src &= 0x000000FF;
     src <<= 8;
     addr += src;
     src = *pSrc++;
     src &= 0x000000FF;
     src <<= 16;
     addr += src;
     src = *pSrc++;
     src &= 0x000000FF;
     src <<= 24;
     addr += src;
     //addr >>=1;
     *pAddr = (HMDRV_ADDR)addr;
     return pSrc;
#elif HMDRV_CFG_BUS_WIDTH == 2
     HMDRV_U16 src = 0;

     src = *pSrc++;
     src += *pSrc++ << 8;
     addr = src;

     if(pcm_bNextAddrIsEx)
     {
         src = *pSrc++;
         src += *pSrc++ << 8;
         addr += (HMDRV_U32)src << 16;
     }

     *pAddr = (HMDRV_ADDR)addr;
     return pSrc;
#else
     
     /* treat address as long word to determine the kind of the address */

    if(pcm_bNextAddrIsEx)
    {
        addr = (HMDRV_U32)(
                ((HMDRV_U32)(*(HMDRV_U8*)(pSrc+0))<< 0) + 
                ((HMDRV_U32)(*(HMDRV_U8*)(pSrc+1))<< 8) + 
                ((HMDRV_U32)(*(HMDRV_U8*)(pSrc+2))<<16) + 
                ((HMDRV_U32)(*(HMDRV_U8*)(pSrc+3))<<24));
        pSrc += 4;
    }
    else
    {
        
        addr = (HMDRV_U32)(
                ((HMDRV_U16)(*(HMDRV_U8*)(pSrc+0))<< 0) + 
                ((HMDRV_U16)(*(HMDRV_U8*)(pSrc+1))<< 8));

        pSrc += 2;
    }
    
    *pAddr = (HMDRV_ADDR)addr;
    
    return pSrc;
#endif

}




#endif  /* Used EX Commands */


HMDRV_U32 HMDRV_u32GetDeviceSYSCLKFreq(void)
{
    return u32DeviceSYSCLKFreq;
}

#endif  /* #if (!HMDRV_DISABLE) */

