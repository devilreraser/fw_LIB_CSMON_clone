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
#include "io_drv.h"
#include "device.h"
#include "driverlib.h"
#include "boardcfg.h"

//
// FPU header file to access memcpy_fast_far().
//
#include "fpu_vector.h"
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */
typedef enum
{
    GREEN_LED_MODE_WHOLE_RAM_RW_TIME,
    GREEN_LED_MODE_WORD16_RAM_RD_TIME,
    GREEN_LED_MODE_WORD16_RAM_WR_TIME,
    GREEN_LED_MODE_WORD32_RAM_RD_TIME,
    GREEN_LED_MODE_WORD32_RAM_WR_TIME,
    GREEN_LED_MODE_PIN_TOGGLE_TIME,
} eGreenLedMode_t;


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


typedef struct
{
	uint16_t narrowMode;     //!< Read Setup Cycles
	uint16_t iBank;                //!< Banks available in SDRAM device
	uint16_t casLatency;     //!< CAS Latency for SDRAM device
	uint16_t pageSize;         //!< Pagesize of SDRAM device
} EMIF_SyncConfig;

typedef struct
{
    uint32_t tRfc;          //!< Auto refresh time
    uint32_t tRp;           //!< Row precharge time
    uint32_t tRcd;          //!< RAS to CAS delay
    uint32_t tWr;           //!< Write recovery time
    uint32_t tRas;          //!< Row active time
    uint32_t tRc;           //!< Read cycle time
    uint32_t tRrd;          //!< Row active to row active delay
} EMIF_SyncTimingParams;


#endif



/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
#if BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC

	EMIF_AsyncTimingParams sEMIF_1_Param =
	{
		0,      // uint32_t rSetup;     //!< Read Setup Cycles
		7,      // uint32_t rStrobe;    //!< Read Strobe Cycles (tested at 25 deg ambient works with 6 )
		0,      // uint32_t rHold;      //!< Read Hold Cycles
		0,      // uint32_t wSetup;     //!< Write Setup Cycles
		1,      // uint32_t wStrobe;    //!< Write Strobe Cycles
		0,      // uint32_t wHold;      //!< Write Hold Cycles
		0,      // uint32_t turnArnd;   //!< TurnAround Cycles
	};

	#ifndef EMIF_2_SDRAM
		EMIF_AsyncTimingParams sEMIF_2_Param =
		{
			15,     // uint32_t rSetup;		//!< Read Setup Cycles
			63,     // uint32_t rStrobe;	//!< Read Strobe Cycles (tested at 25 deg ambient works with 6 )
			7,      // uint32_t rHold;		//!< Read Hold Cycles
			15,     // uint32_t wSetup;		//!< Write Setup Cycles
			63,     // uint32_t wStrobe;	//!< Write Strobe Cycles
			7,      // uint32_t wHold;		//!< Write Hold Cycles
			0,      // uint32_t turnArnd;	//!< TurnAround Cycles  0 -> 8bit; 1 -> 16bit; 2 -> 32bit
		};
	#else
		EMIF_SyncTimingParams sdEMIF_2_tParam =
		{
			6,	// uint32_t tRfc;  0b  //!< Auto refresh time
			1,	// uint32_t tRp;     //!< Row precharge time
			1,	// uint32_t tRcd;    //!< RAS to CAS delay
			1,	// uint32_t tWr;     //!< Write recovery time
			4,	// uint32_t tRas;    //!< Row active time
			6,	// uint32_t tRc;     //!< Read cycle time
			1,	// uint32_t tRrd;    //!< Row active to row active delay
		};

		EMIF_SyncConfig sdEMIF_2_Config =
		{
			EMIF_SYNC_NARROW_MODE_FALSE,	// EMIF_SyncNarrowMode narrowMode;  //!< Read Setup Cycles
			EMIF_SYNC_BANK_1,			// EMIF_SyncBank iBank;             //!< Banks available in SDRAM device
			EMIF_SYNC_CAS_LAT_3 ,		// EMIF_SyncCASLatency casLatency;  //!< CAS Latency for SDRAM device
			EMIF_SYNC_COLUMN_WIDTH_11,	// EMIF_SyncPageSize pageSize;      //!< Pagesize of SDRAM device
		};
	#endif


#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG

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
	EMIF_AsyncTimingParams sEMIF_2_Param =
	{
		0,      //        uint32_t rSetup;            //!< Read Setup Cycles
		7,      //        uint32_t rStrobe;           //!< Read Strobe Cycles (tested at 25 deg ambient works with 6 )
		0,      //        uint32_t rHold;             //!< Read Hold Cycles
		0,      //        uint32_t wSetup;            //!< Write Setup Cycles
		1,      //        uint32_t wStrobe;           //!< Write Strobe Cycles
		0,      //        uint32_t wHold;             //!< Write Hold Cycles
		0,      //        uint32_t turnArnd;          //!< TurnAround Cycles
	};

#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_SMALL_TEST
	EMIF_AsyncTimingParams sEMIFParam = {
		0,      //        uint32_t rSetup;            //!< Read Setup Cycles
		4,      //        uint32_t rStrobe;           //!< Read Strobe Cycles (not tested at high temperature Strobe 3 works @ 25 deg put 4 for safety)
		0,      //        uint32_t rHold;             //!< Read Hold Cycles
		0,      //        uint32_t wSetup;            //!< Write Setup Cycles
		0,      //        uint32_t wStrobe;           //!< Write Strobe Cycles (assume not needed write strobes)
		0,      //        uint32_t wHold;             //!< Write Hold Cycles
		0,      //        uint32_t turnArnd;          //!< TurnAround Cycles
		};
	EMIF_AsyncTimingParams sEMIF_2_Param =
	{
		0,      //        uint32_t rSetup;            //!< Read Setup Cycles
		7,      //        uint32_t rStrobe;           //!< Read Strobe Cycles (tested at 25 deg ambient works with 6 )
		0,      //        uint32_t rHold;             //!< Read Hold Cycles
		0,      //        uint32_t wSetup;            //!< Write Setup Cycles
		1,      //        uint32_t wStrobe;           //!< Write Strobe Cycles
		0,      //        uint32_t wHold;             //!< Write Hold Cycles
		0,      //        uint32_t turnArnd;          //!< TurnAround Cycles
	};

#else
	EMIF_AsyncTimingParams sEMIFParam =
	{
		0b1111,		//        uint32_t rSetup;		//!< Read Setup Cycles
		0b111111,	//        uint32_t rStrobe;		//!< Read Strobe Cycles (tested at 25 deg ambient works with 6 )
		0b111,		//        uint32_t rHold;		//!< Read Hold Cycles
		0b1111,		//        uint32_t wSetup;		//!< Write Setup Cycles
		0b111111,	//        uint32_t wStrobe;		//!< Write Strobe Cycles
		0b111,		//        uint32_t wHold;		//!< Write Hold Cycles
		0b11,		//        uint32_t turnArnd;	//!< TurnAround Cyclescles
	};

	EMIF_AsyncTimingParams sEMIF_2_Param =
	{
		0b1111,		//        uint32_t rSetup;		//!< Read Setup Cycles
		0b111111,	//        uint32_t rStrobe;		//!< Read Strobe Cycles (tested at 25 deg ambient works with 6 )
		0b111,		//        uint32_t rHold;		//!< Read Hold Cycles
		0b1111,		//        uint32_t wSetup;		//!< Write Setup Cycles
		0b111111,	//        uint32_t wStrobe;		//!< Write Strobe Cycles
		0b111,		//        uint32_t wHold;		//!< Write Hold Cycles
		0b11,		//        uint32_t turnArnd;	//!< TurnAround Cycles
	};

#endif

// EMIF_1
EMIF_AsyncDataWidth eAsyncDataWidth16 = EMIF_ASYNC_DATA_WIDTH;
uint16_t 			u16UseA19AsBA1    = USE_A19_AS_BA1;

volatile uint16_t TestEmif = 0;
volatile uint16_t TestEmifWrRd = 0;
volatile uint32_t TestRslt = 0;

volatile uint32_t emifPtrWrAddress = BOARDCFG_EMIF_START_ADDRESS;
volatile uint32_t emifPtrRdAddress = BOARDCFG_EMIF_START_ADDRESS;

volatile uint32_t emifPtrWrAddress16 = BOARDCFG_EMIF_START_ADDRESS+2;
volatile uint32_t emifPtrRdAddress16 = BOARDCFG_EMIF_START_ADDRESS+1;

volatile uint32_t* emifPtrRd = 0;
volatile uint32_t* emifPtrWr = 0;
volatile uint32_t  emifDatRd = 0;
volatile uint32_t  emifDatWr = 0;

volatile uint16_t* emifPtrRd16 = 0;
volatile uint16_t* emifPtrWr16 = 0;
volatile uint16_t  emifDatRd16 = 0;
volatile uint16_t  emifDatWr16 = 0;

volatile uint16_t EmifWr = 0xFFFF;
volatile uint16_t EmifRd = 0xFFFF;

volatile uint16_t EmifWr16 = 0xFFFF;
volatile uint16_t EmifRd16 = 0xFFFF;

uint16_t EmifSetup = 0;
uint16_t Emif2_Setup = 0;


// EMIF_2
uint16_t u16UseEmif2_A12asBA1 = USE_EMIF2_A12_AS_BA1;

volatile uint16_t TestEmif2 = 0;
volatile uint16_t TestEmif2_WrRd = 0;
volatile uint32_t TestRsltEmif2 = 0;

volatile uint32_t emif2_PtrWrAddress = BOARDCFG_EMIF2_START_ADDRESS;
volatile uint32_t emif2_PtrRdAddress = BOARDCFG_EMIF2_START_ADDRESS;

volatile uint32_t emif2_PtrWrAddress16 = BOARDCFG_EMIF2_START_ADDRESS+2;
volatile uint32_t emif2_PtrRdAddress16 = BOARDCFG_EMIF2_START_ADDRESS+1;


volatile uint32_t* emif2_PtrRd = 0;
volatile uint32_t* emif2_PtrWr = 0;
volatile uint32_t  emif2_DatRd = 0;
volatile uint32_t  emif2_DatWr = 0;

volatile uint16_t* emif2_PtrRd16 = 0;
volatile uint16_t* emif2_PtrWr16 = 0;
volatile uint16_t  emif2_DatRd16 = 0;
volatile uint16_t  emif2_DatWr16 = 0;

//volatile uint16_t EmifWr = 0xFFFF;
//volatile uint16_t EmifRd = 0xFFFF;
//
//volatile uint16_t EmifWr16 = 0xFFFF;
//volatile uint16_t EmifRd16 = 0xFFFF;


volatile eGreenLedMode_t eGreenLedMode = GREEN_LED_MODE_PIN_TOGGLE_TIME;



/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */
extern uint32_t EMIF_vTest(uint16_t testMask);

#if (BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC)
	extern uint32_t EMIF_2_vTest(uint16_t testMask);
	extern void EMIF_1_vInit16_Module( EMIF_AsyncTimingParams tparam, EMIF_AsyncDataWidth eAsyncDataWidth, uint16_t u16UseA19AsBA1 );
	extern void EMIF_2_vInit16_Module( EMIF_AsyncTimingParams tparam, EMIF_AsyncDataWidth eAsyncDataWidth, uint16_t u16UseEmif2_A12asBA1 );
	extern void EMIF_2_vInit16_SDRam_Module( EMIF_SyncTimingParams tParam, EMIF_SyncConfig sdConfig );
#endif

extern void EMIF_vInitModule(EMIF_AsyncTimingParams tparam, EMIF_AsyncDataWidth eAsyncDataWidth, uint16_t u16UseA19AsBA1);  //EMIF_SyncConfig

/* *****************************************************************************
 * Functions
 **************************************************************************** */

void EMIF_vInit(void)
{

#if (BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC)
    EMIF_1_vInit16_Module( sEMIF_1_Param, eAsyncDataWidth16, u16UseA19AsBA1 );

	#ifndef EMIF_2_SDRAM
		EMIF_2_vInit16_Module( sEMIF_2_Param, eAsyncDataWidth16, u16UseEmif2_A12asBA1 );
	#else
		EMIF_2_vInit16_SDRam_Module( sdEMIF_2_tParam, sdEMIF_2_Config );
	#endif

#else
    EMIF_vInitModule( sEMIFParam, eAsyncDataWidth16, u16UseA19AsBA1 );
#endif
}


void EMIF_vProcess(void)
{
	if (EmifSetup) {
		EmifSetup = 0;
#if (BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC)
		EMIF_1_vInit16_Module(sEMIF_1_Param, eAsyncDataWidth16, u16UseA19AsBA1);
	#ifndef EMIF_2_SDRAM
		EMIF_2_vInit16_Module(sEMIF_2_Param, eAsyncDataWidth16, u16UseEmif2_A12asBA1);
	#else
		EMIF_2_vInit16_SDRam_Module( sdEMIF_2_tParam, sdEMIF_2_Config );
	#endif
#else
        EMIF_vInitModule( sEMIFParam, eAsyncDataWidth16, u16UseA19AsBA1 );
#endif
		//EMIF_vInitModule(sEMIFParam, eAsyncDataWidth16, u16UseA19AsBA1);
	}

	if (eGreenLedMode == GREEN_LED_MODE_PIN_TOGGLE_TIME) {
#if STAT_LED_G_PIN
        GPIO_writePin(STAT_LED_G_PIN, STAT_LED_ENABLE);
#endif
	}

	if (eGreenLedMode == GREEN_LED_MODE_PIN_TOGGLE_TIME) {
#if STAT_LED_G_PIN
        GPIO_writePin(STAT_LED_G_PIN, STAT_LED_DISABLE);
#endif
	}

	if (TestEmif != 0) {
		if (eGreenLedMode == GREEN_LED_MODE_WHOLE_RAM_RW_TIME) {
#if STAT_LED_G_PIN
            GPIO_writePin(STAT_LED_G_PIN, STAT_LED_ENABLE);
#endif
		}

		TestRslt = EMIF_vTest(TestEmif);

		if (eGreenLedMode == GREEN_LED_MODE_WHOLE_RAM_RW_TIME) {
#if STAT_LED_G_PIN
            GPIO_writePin(STAT_LED_G_PIN, STAT_LED_DISABLE);
#endif
		}

		TestEmif = 0;
	}

	if (TestEmifWrRd != 0) {
		emifPtrWr = (uint32_t*) emifPtrWrAddress;
		if (EmifWr != 0) {
			if (eGreenLedMode == GREEN_LED_MODE_WORD32_RAM_WR_TIME) {
#if STAT_LED_G_PIN
            GPIO_writePin(STAT_LED_G_PIN, STAT_LED_ENABLE);
#endif
			}

			*emifPtrWr = emifDatWr;
			if (eGreenLedMode == GREEN_LED_MODE_WORD32_RAM_WR_TIME) {
#if STAT_LED_G_PIN
            GPIO_writePin(STAT_LED_G_PIN, STAT_LED_DISABLE);
#endif
			}

			if (EmifWr < 0xFFFF) {
				EmifWr--;
			}
		}

		emifPtrRd = (uint32_t*) emifPtrRdAddress;
		if (EmifRd != 0) {
			if (eGreenLedMode == GREEN_LED_MODE_WORD32_RAM_RD_TIME) {
#if STAT_LED_G_PIN
            GPIO_writePin(STAT_LED_G_PIN, STAT_LED_ENABLE);
#endif
			}
			emifDatRd = *emifPtrRd;
			if (eGreenLedMode == GREEN_LED_MODE_WORD32_RAM_RD_TIME) {
#if STAT_LED_G_PIN
           GPIO_writePin(STAT_LED_G_PIN, STAT_LED_DISABLE);
#endif
			}
			if (EmifRd < 0xFFFF) {
				EmifRd--;
			}
		}

		emifPtrWr16 = (uint16_t*) emifPtrWrAddress16;
		if (EmifWr16 != 0) {
			if (eGreenLedMode == GREEN_LED_MODE_WORD16_RAM_WR_TIME) {
#if STAT_LED_G_PIN
            GPIO_writePin(STAT_LED_G_PIN, STAT_LED_ENABLE);
#endif
			}
			*emifPtrWr16 = emifDatWr16;
			if (eGreenLedMode == GREEN_LED_MODE_WORD16_RAM_WR_TIME) {
#if STAT_LED_G_PIN
            GPIO_writePin(STAT_LED_G_PIN, STAT_LED_DISABLE);
#endif
			}
			if (EmifWr16 < 0xFFFF) {
				EmifWr16--;
			}
		}

		emifPtrRd16 = (uint16_t*) emifPtrRdAddress16;
		if (EmifRd16 != 0) {
			if (eGreenLedMode == GREEN_LED_MODE_WORD16_RAM_RD_TIME) {
#if STAT_LED_G_PIN
            GPIO_writePin(STAT_LED_G_PIN, STAT_LED_ENABLE);
#endif
			}
			emifDatRd16 = *emifPtrRd16;
			if (eGreenLedMode == GREEN_LED_MODE_WORD16_RAM_RD_TIME) {
#if STAT_LED_G_PIN
            GPIO_writePin(STAT_LED_G_PIN, STAT_LED_DISABLE);
#endif
			}
			if (EmifRd16 < 0xFFFF) {
				EmifRd16--;
			}
		}
	}
}
#if (BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC)
bool bTestSingleRdEmif2 = false;
bool bTestSingleWrEmif2 = false;
uint16_t u16TestSingleRdEmif2 = 0;
uint16_t u16TestSingleWrEmif2 = 0;
uint16_t* pu16TestSingleRdEmif2 = (uint16_t*)BOARDCFG_EMIF2_CS2_START_ADDRESS;
uint16_t* pu16TestSingleWrEmif2 = (uint16_t*)BOARDCFG_EMIF2_CS2_START_ADDRESS;

uint16_t* pu16TestSingleRdEmif2_SDRAM = (uint16_t*)BOARDCFG_EMIF2_CS0_START_ADDRESS;
uint16_t* pu16TestSingleWrEmif2_SDRAM = (uint16_t*)BOARDCFG_EMIF2_CS0_START_ADDRESS;

uint16_t u16TestSingleRdEmif2_SDRAM = 0;
uint16_t u16TestSingleWrEmif2_SDRAM = 0;
bool Emif2_Read_SDRAM = 0;
bool Emif2_Write_SDRAM = 0;

void EMIF_2_vProcess(void)
{
	if (Emif2_Read_SDRAM)
	{
		memcpy_fast_far(&u16TestSingleRdEmif2_SDRAM, pu16TestSingleRdEmif2_SDRAM, 1);  // 1 = 16-bit
		Emif2_Read_SDRAM = 0;
	}

	if (Emif2_Write_SDRAM)
	{
		memcpy_fast_far(pu16TestSingleWrEmif2_SDRAM, &u16TestSingleWrEmif2_SDRAM, 1);  // 1 = 16-bit
		Emif2_Write_SDRAM = 0;
	}
/*
		// Fill far memory buffer with data.
    	memWdl = 0x0;
	    for(i=0; i < memSize; i++)
	    {
	        memcpy_fast_far((extSDRAMBuf + i), &memWdl, 2);
	        memWdl += 0x00050001;
	    }

	    // Read far memory buffer into local buffer and verify data.
	    memWdl = 0x0;
	    for(i=0; i < memSize; i++)
	    {
	        memcpy_fast_far((localRAMBuf + i), (extSDRAMBuf + i), 2);
	        // Return error if read data is incorrect.
	        if(localRAMBuf[i] != memWdl)
	        {
	            return(1);
	        }
	        memWdl += 0x00050001;
	    }
*/


	if (Emif2_Setup) {
		Emif2_Setup = 0;
	#ifndef EMIF_2_SDRAM
		EMIF_2_vInit16_Module( sEMIF_2_Param, eAsyncDataWidth16, u16UseEmif2_A12asBA1 );
	#else
		EMIF_2_vInit16_SDRam_Module( sdEMIF_2_tParam, sdEMIF_2_Config );
	#endif
	}

	if (TestEmif2 != 0) {
		TestRslt = EMIF_2_vTest(TestEmif2);
		TestEmif2 = 0;
	}

	if (TestEmif2_WrRd != 0)
	{
		emif2_PtrWr = (uint32_t*) emif2_PtrWrAddress;
		if (EmifWr != 0) {
			*emif2_PtrWr = emif2_DatWr;
			if (EmifWr < 0xFFFF) {
				EmifWr--;
			}
		}
		emif2_PtrRd = (uint32_t*) emif2_PtrRdAddress;
		if (EmifRd != 0) {
			emif2_DatRd = *emif2_PtrRd;
			if (EmifRd < 0xFFFF) {
				EmifRd--;
			}
		}

		emif2_PtrRd16 = (uint16_t*) emif2_PtrWrAddress16;
		if (EmifWr16 != 0)
		{
			*emif2_PtrRd16 = emif2_DatWr16;
			if (EmifWr16 < 0xFFFF) {
				EmifWr16--;
			}
		}

		emif2_PtrRd16 = (uint16_t*) emif2_PtrRdAddress16;
		if (EmifRd16 != 0)
		{
			emif2_DatRd16 = *emif2_PtrRd16;
			if (EmifRd16 < 0xFFFF) {
				EmifRd16--;
			}
		}
	}
	if(bTestSingleRdEmif2)
	{
		bTestSingleRdEmif2 = false;
		u16TestSingleRdEmif2 = *pu16TestSingleRdEmif2;
	}
	if(bTestSingleWrEmif2)
	{
		bTestSingleWrEmif2 = false;
		*pu16TestSingleWrEmif2 = u16TestSingleWrEmif2;
	}

}
#endif
