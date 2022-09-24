//#############################################################################
//
// FILE:   emif_ex1_16bit_asram.c
//
// TITLE:  EMIF1 ASYNC module accessing 16bit ASRAM.
//
//! \addtogroup driver_example_list
//! <h1> EMIF1 ASYNC module accessing 16bit ASRAM. </h1>
//!
//! This example configures EMIF1 in 16bit ASYNC mode and uses CS2 as chip
//! enable.
//!
//! \b External \b Connections \n
//!  - External ASRAM memory (CY7C1041CV33 -10ZSXA) daughter card
//!
//! \b Watch \b Variables \n
//! - \b testStatusGlobal - Equivalent to \b TEST_PASS if test finished
//!                         correctly, else the value is set to \b TEST_FAIL
//! - \b errCountGlobal - Error counter
//!
//
//#############################################################################
// $TI Release: F2837xD Support Library v3.06.00.00 $
// $Release Date: Mon May 27 06:48:24 CDT 2019 $
// $Copyright:
// Copyright (C) 2013-2019 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//#############################################################################

//
// Included Files
//
#include "device.h"
#include "driverlib.h"

#include "boardcfg.h"

#if (BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_BIG) || (BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC)

	#define EMIF_ASYNC_CS_OFFSET    EMIF_ASYNC_CS2_OFFSET  //only EMIF_ASYNC_CS2_OFFSET is valid for EMIF2
	#define EMIF_ASYNC_CS    2
	#ifndef EMIF_2_SDRAM
		#define EMIF2_ASYNC_CS   2
	#else
		#define EMIF_SYNC_CS_OFFSET    EMIF_O_SDRAM_CR
		#define EMIF2_SYNC_CS   0
	#endif
#elif BOARDCFG_BOARD == BOARDCFG_BOARD_SCC_SMALL_TEST
	//#define EMIF_ASYNC_CS_OFFSET    EMIF_ASYNC_CS3_OFFSET
	//#define EMIF_ASYNC_CS   3
	#define EMIF_ASYNC_CS_OFFSET    EMIF_ASYNC_CS2_OFFSET
	#define EMIF_ASYNC_CS   2
#endif

//
// Defines
//
#define TEST_PASS               0xABCDABCD
#define TEST_FAIL               0xDEADDEAD



#define ASRAM_START_ADDR        BOARDCFG_EMIF_START_ADDRESS
#define ASRAM_SIZE              BOARDCFG_EMIF_LENGTH

#define FPGA_SRAM_START_ADDR    BOARDCFG_EMIF2_START_ADDRESS
#define FPGA_SRAM_SIZE          BOARDCFG_EMIF2_LENGTH



//
// Globals
//
uint32_t errCountGlobal = 0;
uint32_t testStatusGlobal;
uint32_t i;

uint16_t u16CS234 = EMIF_ASYNC_CS;
uint16_t u16CS02  = EMIF2_SYNC_CS;

volatile uint32_t* memPtrStart;

//
// Function Prototypes
//
#if (BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC)
	extern void setupEMIF_1_PinmuxAsync16BitBoardSCC(uint16_t u16CS234, uint16_t u16UseA19AsBA1);
	extern void setupEMIF_2_PinmuxAsync16BitBoardSCC(uint16_t u16CS02, uint16_t u16UseEmif2_A12asBA1);
#endif
extern void setupEMIF1PinmuxAsync32BitBoardSCC(uint16_t u16CS234, uint16_t u16UseA19AsBA1);

uint32_t readWriteMem(uint32_t startAddr, uint32_t memSize);
uint32_t walkMemData(uint32_t startAddr, uint32_t memSize);
uint32_t walkMemAddr(uint32_t startAddr, uint32_t addrSize);
uint32_t readWriteMem16(uint32_t startAddr, uint32_t memSize);
uint32_t walkMemData16(uint32_t startAddr, uint32_t memSize);
uint32_t walkMemAddr16(uint32_t startAddr, uint32_t addrSize);
uint32_t accessMemData(uint32_t startAddr, uint32_t sizeToCheck);
uint32_t accessMemData16(uint32_t startAddr, uint32_t sizeToCheck);
void writeMemData(uint32_t startAddr, uint32_t memSize);
void readMemData(uint32_t startAddr, uint32_t memSize);
void writeMemData16(uint32_t startAddr, uint32_t memSize);
void readMemData16(uint32_t startAddr, uint32_t memSize);


EMIF_AsyncDataWidth eAsyncDataWidthLast;

#if (BOARDCFG_BOARD == BOARDCFG_BOARD_CS_1107_SCC)

void EMIF_1_vInit16_Module( EMIF_AsyncTimingParams tparam, EMIF_AsyncDataWidth eAsyncDataWidth, uint16_t u16UseA19AsBA1 )
{
    // Configure to run EMIF1 on full Rate. (EMIF1CLK = CPU1SYSCLK)
    SysCtl_setEMIF1ClockDivider( SYSCTL_EMIF1CLK_DIV_1 );

#if defined(CPU1)
    // Grab EMIF1 For CPU1.
    EMIF_selectMaster(EMIF1CONFIG_BASE, EMIF_MASTER_CPU1_G);
#endif

    // Disable Access Protection. (CPU_FETCH/CPU_WR/DMA_WR)
    EMIF_setAccessProtection(EMIF1CONFIG_BASE, 0x0);

    // Commit the configuration related to protection. Till this bit remains set, contents of EMIF1ACCPROT0 register can't be changed.
    EMIF_commitAccessConfig(EMIF1CONFIG_BASE);

    // Lock the configuration so that EMIF1COMMIT register can't be changed any more.
    EMIF_lockAccessConfig(EMIF1CONFIG_BASE);


    // Configure GPIO pins for EMIF1.
    setupEMIF_1_PinmuxAsync16BitBoardSCC( u16CS234, u16UseA19AsBA1 );

    // Configures Normal Asynchronous Mode of Operation.
	EMIF_setAsyncMode(EMIF1_BASE, EMIF_ASYNC_CS_OFFSET, EMIF_ASYNC_NORMAL_MODE);

    // Disables Extended Wait Mode.
    EMIF_disableAsyncExtendedWait(EMIF1_BASE, EMIF_ASYNC_CS_OFFSET);

    // Configure EMIF1 Data Bus Width.
    EMIF_setAsyncDataBusWidth(EMIF1_BASE, EMIF_ASYNC_CS_OFFSET, eAsyncDataWidth);
    eAsyncDataWidthLast = eAsyncDataWidth;

    // Configure the access timing for CS2 space.
//    typedef struct
//    {
//        uint32_t rSetup;            //!< Read Setup Cycles
//        uint32_t rStrobe;           //!< Read Strobe Cycles
//        uint32_t rHold;             //!< Read Hold Cycles
//        uint32_t wSetup;            //!< Write Setup Cycles
//        uint32_t wStrobe;           //!< Write Strobe Cycles
//        uint32_t wHold;             //!< Write Hold Cycles
//        uint32_t turnArnd;          //!< TurnAround Cycles
//    } EMIF_AsyncTimingParams;
//    tparam.rSetup = 0;
//    tparam.rStrobe = 6;
//    tparam.rHold = 0;
//    tparam.turnArnd = 0;
//    tparam.wSetup = 0;
//    tparam.wStrobe = 1;
//    tparam.wHold = 0;
    EMIF_setAsyncTimingParams(EMIF1_BASE, EMIF_ASYNC_CS_OFFSET, &tparam);
}



////////////////////////////////////
//    Texas library mistake ??    //
////////////////////////////////////

static inline void EMIF_2_setAccessProtection(uint32_t configBase, uint16_t access)
{
	uint16_t temp;

	// Check the arguments.
	ASSERT( EMIF_isEMIF1ConfigBaseValid(configBase ) || EMIF_isEMIF2ConfigBaseValid(configBase));
	if (configBase == EMIF1CONFIG_BASE) {
		ASSERT( access <= EMIF_ACCPROT0_MASK_EMIF1 );
		temp = EMIF_ACCPROT0_MASK_EMIF1;
	} else {
		ASSERT( access <= EMIF_ACCPROT0_MASK_EMIF2 );
		temp = EMIF_ACCPROT0_MASK_EMIF2;
	}

	// Sets the bits that enables access protection config.
	EALLOW;
	HWREGH( configBase + MEMCFG_O_EMIF2ACCPROT0 ) = (HWREGH( configBase + MEMCFG_O_EMIF2ACCPROT0 ) & ~(temp)) | access;
	EDIS;
}

static inline void EMIF_2_commitAccessConfig(uint32_t configBase)
{
    // Check the arguments.
	ASSERT( EMIF_isEMIF1ConfigBaseValid(configBase ) || EMIF_isEMIF2ConfigBaseValid( configBase ) );

    // Sets the bits that commits access protection config.
	EALLOW;
	HWREGH(configBase + MEMCFG_O_EMIF2COMMIT) |= MEMCFG_EMIF2COMMIT_COMMIT_EMIF2;
	EDIS;
}

static inline void EMIF_2_lockAccessConfig(uint32_t configBase)
{
	// Check the arguments.
	ASSERT( EMIF_isEMIF2ConfigBaseValid(configBase) );

	// Sets the bits that locks access protection config.
	EALLOW;
	HWREGH( configBase + MEMCFG_O_EMIF2LOCK ) |= MEMCFG_EMIF2LOCK_LOCK_EMIF2;
	EDIS;
}


void EMIF_2_vInit16_Module( EMIF_AsyncTimingParams tparam, EMIF_AsyncDataWidth eAsyncDataWidth, uint16_t u16UseEmif2_A12asBA1 )
{
    //EMIF_AsyncTimingParams tparam;   Configure to run EMIF2 on full Rate. (EMIF2CLK = CPU1SYSCLK)
    SysCtl_setEMIF2ClockDivider( SYSCTL_EMIF2CLK_DIV_1 );

    // EMIF2 has access only to CPU1.
    //EMIF_2_selectMaster( EMIF2CONFIG_BASE, EMIF_MASTER_CPU1_G );

    // Disable Access Protection. (CPU_FETCH/CPU_WR/DMA_WR)
    EMIF_2_setAccessProtection( EMIF2CONFIG_BASE, 0x0 );

    // Commit the configuration related to protection. Till this bit remains set, contents of EMIF2ACCPROT0 register can't be changed.
    EMIF_2_commitAccessConfig( EMIF2CONFIG_BASE );

    // Lock the configuration so that EMIF2COMMIT register can't be changed any more.
    EMIF_2_lockAccessConfig( EMIF2CONFIG_BASE );

    // Configure GPIO pins for EMIF2.
    setupEMIF_2_PinmuxAsync16BitBoardSCC( u16CS02, u16UseEmif2_A12asBA1 );

    // Configures Normal Asynchronous Mode of Operation.
	EMIF_setAsyncMode( EMIF2_BASE, EMIF_ASYNC_CS_OFFSET, EMIF_ASYNC_NORMAL_MODE );

    // Disables Extended Wait Mode.
    EMIF_disableAsyncExtendedWait( EMIF2_BASE, EMIF_ASYNC_CS_OFFSET );

    // Configure EMIF2 Data Bus Width.
    EMIF_setAsyncDataBusWidth( EMIF2_BASE, EMIF_ASYNC_CS_OFFSET, eAsyncDataWidth );
    eAsyncDataWidthLast = eAsyncDataWidth;

    // Configure the access timing for CS2 space.
//    typedef struct
//    {
//        uint32_t rSetup;            //!< Read Setup Cycles
//        uint32_t rStrobe;           //!< Read Strobe Cycles
//        uint32_t rHold;             //!< Read Hold Cycles
//        uint32_t wSetup;            //!< Write Setup Cycles
//        uint32_t wStrobe;           //!< Write Strobe Cycles
//        uint32_t wHold;             //!< Write Hold Cycles
//        uint32_t turnArnd;          //!< TurnAround Cycles
//    } EMIF_AsyncTimingParams;
//    tparam.rSetup = 0;
//    tparam.rStrobe = 6;
//    tparam.rHold = 0;
//    tparam.turnArnd = 0;
//    tparam.wSetup = 0;
//    tparam.wStrobe = 1;
//    tparam.wHold = 0;
    EMIF_setAsyncTimingParams( EMIF2_BASE, EMIF_ASYNC_CS_OFFSET, &tparam );
}

#endif
//
// EMIF_vInit
//
void EMIF_vInitModule( EMIF_AsyncTimingParams tparam, EMIF_AsyncDataWidth eAsyncDataWidth, uint16_t u16UseA19AsBA1 )
{
    //EMIF_AsyncTimingParams tparam;
    // Configure to run EMIF1 on full Rate. (EMIF1CLK = CPU1SYSCLK)
    SysCtl_setEMIF1ClockDivider( SYSCTL_EMIF1CLK_DIV_1 );

#if defined(CPU1)
    // Grab EMIF1 For CPU1.
    EMIF_selectMaster(EMIF1CONFIG_BASE, EMIF_MASTER_CPU1_G);
#endif

    // Disable Access Protection. (CPU_FETCH/CPU_WR/DMA_WR)
    EMIF_setAccessProtection(EMIF1CONFIG_BASE, 0x0);

    // Commit the configuration related to protection. Till this bit remains
    // set, contents of EMIF1ACCPROT0 register can't be changed.
    EMIF_commitAccessConfig(EMIF1CONFIG_BASE);

    // Lock the configuration so that EMIF1COMMIT register can't be changed any more.
    EMIF_lockAccessConfig(EMIF1CONFIG_BASE);


    // Configure GPIO pins for EMIF1.
    setupEMIF1PinmuxAsync32BitBoardSCC( u16CS234, u16UseA19AsBA1 );

    //
    // Configures Normal Asynchronous Mode of Operation.
    //
    EMIF_setAsyncMode(EMIF1_BASE, EMIF_ASYNC_CS_OFFSET,
                      EMIF_ASYNC_NORMAL_MODE);

    //
    // Disables Extended Wait Mode.
    //
    EMIF_disableAsyncExtendedWait(EMIF1_BASE, EMIF_ASYNC_CS_OFFSET);

    //
    // Configure EMIF1 Data Bus Width.
    //
	// EMIF_setAsyncDataBusWidth(EMIF1_BASE, EMIF_ASYNC_CS_OFFSET, EMIF_ASYNC_DATA_WIDTH_32);
    EMIF_setAsyncDataBusWidth(EMIF1_BASE, EMIF_ASYNC_CS_OFFSET, eAsyncDataWidth);
    eAsyncDataWidthLast = eAsyncDataWidth;

    //
    // Configure the access timing for CS2 space.
    //
//    typedef struct
//    {
//        uint32_t rSetup;            //!< Read Setup Cycles
//        uint32_t rStrobe;           //!< Read Strobe Cycles
//        uint32_t rHold;             //!< Read Hold Cycles
//        uint32_t wSetup;            //!< Write Setup Cycles
//        uint32_t wStrobe;           //!< Write Strobe Cycles
//        uint32_t wHold;             //!< Write Hold Cycles
//        uint32_t turnArnd;          //!< TurnAround Cycles
//    } EMIF_AsyncTimingParams;
//    tparam.rSetup = 0;
//    tparam.rStrobe = 3;
//    tparam.rHold = 0;
//    tparam.turnArnd = 0;
//    tparam.wSetup = 0;
//    tparam.wStrobe = 1;
//    tparam.wHold = 0;
    EMIF_setAsyncTimingParams(EMIF1_BASE, EMIF_ASYNC_CS_OFFSET, &tparam);
}



// EMIF_1_vTest
uint32_t EMIF_vTest(uint16_t testMask)
{
	uint32_t errCountResult = 0;
	uint16_t errCountLocal;
	testStatusGlobal = TEST_FAIL;

	if (testMask & 0x01) {
		// Checks basic RD/WR access to CS2 space.
		if (eAsyncDataWidthLast == EMIF_ASYNC_DATA_WIDTH_32) {
			errCountLocal = readWriteMem(ASRAM_START_ADDR, ASRAM_SIZE);
		} else if (eAsyncDataWidthLast == EMIF_ASYNC_DATA_WIDTH_16) {          // CS_1107_SCC
			errCountLocal = readWriteMem16(ASRAM_START_ADDR, ASRAM_SIZE / 2);  //0x100000 -> 1Mx16
		} else {
			errCountLocal = 1;
		}
		errCountGlobal = errCountGlobal + errCountLocal;
		errCountResult = errCountResult + errCountLocal;
	}

	if (testMask & 0x02) {
		// Address walk checks. (Tested for Memory with address width of 20bit)
		if (eAsyncDataWidthLast == EMIF_ASYNC_DATA_WIDTH_32) {
			errCountLocal = walkMemAddr(ASRAM_START_ADDR, 20);
		} else if (eAsyncDataWidthLast == EMIF_ASYNC_DATA_WIDTH_16) {
			errCountLocal = walkMemAddr16(ASRAM_START_ADDR, 20);
		} else {
			errCountLocal = 1;
		}
		errCountGlobal = errCountGlobal + errCountLocal;
		errCountResult = errCountResult + errCountLocal;
	}

	if (testMask & 0x04) {
		// Data walk checks.
		if (eAsyncDataWidthLast == EMIF_ASYNC_DATA_WIDTH_32) {
			errCountLocal = walkMemData(ASRAM_START_ADDR, ASRAM_SIZE);
		} else if (eAsyncDataWidthLast == EMIF_ASYNC_DATA_WIDTH_16) {
			errCountLocal = walkMemData16(ASRAM_START_ADDR, ASRAM_SIZE / 2);
		} else {
			errCountLocal = 1;
		}
		errCountGlobal = errCountGlobal + errCountLocal;
		errCountResult = errCountResult + errCountLocal;
	}

	if (testMask & 0x08) {
		// Data size checks.
		if (eAsyncDataWidthLast == EMIF_ASYNC_DATA_WIDTH_32) {
			errCountLocal = accessMemData(ASRAM_START_ADDR, ASRAM_SIZE);
		} else if (eAsyncDataWidthLast == EMIF_ASYNC_DATA_WIDTH_16) {
			errCountLocal = accessMemData16(ASRAM_START_ADDR, ASRAM_SIZE / 2);
		} else {
			errCountLocal = 1;
		}
		errCountGlobal = errCountGlobal + errCountLocal;
		errCountResult = errCountResult + errCountLocal;
	}

	if (testMask & 0x10) {
		// Write Data Test Time
		if (eAsyncDataWidthLast == EMIF_ASYNC_DATA_WIDTH_32) {
			writeMemData(ASRAM_START_ADDR, ASRAM_SIZE);
		} else if (eAsyncDataWidthLast == EMIF_ASYNC_DATA_WIDTH_16) {
			writeMemData16(ASRAM_START_ADDR, ASRAM_SIZE / 2);
		}
	}

	if (testMask & 0x20) {
		// Read Data Test Time
		if (eAsyncDataWidthLast == EMIF_ASYNC_DATA_WIDTH_32) {
			readMemData(ASRAM_START_ADDR, ASRAM_SIZE);
		} else if (eAsyncDataWidthLast == EMIF_ASYNC_DATA_WIDTH_16) {
			readMemData16(ASRAM_START_ADDR, ASRAM_SIZE / 2);
		}
	}

	if (errCountGlobal == 0x0) {
		testStatusGlobal = TEST_PASS;
	}

	return errCountResult;
}


// EMIF_2_vTest
uint32_t EMIF_2_vTest(uint16_t testMask)
{
	uint32_t errCountResult = 0;
	uint16_t errCountLocal;
	testStatusGlobal = TEST_FAIL;

	if (testMask & 0x01) {
		// Checks basic RD/WR access to CS2 space.
		if (eAsyncDataWidthLast == EMIF_ASYNC_DATA_WIDTH_32) {
			errCountLocal = readWriteMem(FPGA_SRAM_START_ADDR, FPGA_SRAM_SIZE);
		} else if (eAsyncDataWidthLast == EMIF_ASYNC_DATA_WIDTH_16) {                   // CS_1107_SCC
			errCountLocal = readWriteMem16( FPGA_SRAM_START_ADDR, FPGA_SRAM_SIZE / 2);  //0x00001000 -> 4Kx16
		} else {
			errCountLocal = 1;
		}
		errCountGlobal = errCountGlobal + errCountLocal;
		errCountResult = errCountResult + errCountLocal;
	}

	if (testMask & 0x02) {
		// Address walk checks. (Tested for Memory with address width of 20bit)
		if (eAsyncDataWidthLast == EMIF_ASYNC_DATA_WIDTH_32) {
			errCountLocal = walkMemAddr(FPGA_SRAM_START_ADDR, 20);
		} else if (eAsyncDataWidthLast == EMIF_ASYNC_DATA_WIDTH_16) {
			errCountLocal = walkMemAddr16(FPGA_SRAM_START_ADDR, 20);
		} else {
			errCountLocal = 1;
		}
		errCountGlobal = errCountGlobal + errCountLocal;
		errCountResult = errCountResult + errCountLocal;
	}

	if (testMask & 0x04) {
		// Data walk checks.
		if (eAsyncDataWidthLast == EMIF_ASYNC_DATA_WIDTH_32) {
			errCountLocal = walkMemData(FPGA_SRAM_START_ADDR, FPGA_SRAM_SIZE);
		} else if (eAsyncDataWidthLast == EMIF_ASYNC_DATA_WIDTH_16) {
			errCountLocal = walkMemData16(FPGA_SRAM_START_ADDR, FPGA_SRAM_SIZE / 2);
		} else {
			errCountLocal = 1;
		}
		errCountGlobal = errCountGlobal + errCountLocal;
		errCountResult = errCountResult + errCountLocal;
	}

	if (testMask & 0x08) {
		// Data size checks.
		if (eAsyncDataWidthLast == EMIF_ASYNC_DATA_WIDTH_32) {
			errCountLocal = accessMemData(FPGA_SRAM_START_ADDR, FPGA_SRAM_SIZE);
		} else if (eAsyncDataWidthLast == EMIF_ASYNC_DATA_WIDTH_16) {
			errCountLocal = accessMemData16(FPGA_SRAM_START_ADDR, FPGA_SRAM_SIZE / 2);
		} else {
			errCountLocal = 1;
		}
		errCountGlobal = errCountGlobal + errCountLocal;
		errCountResult = errCountResult + errCountLocal;
	}

	if (testMask & 0x10) {
		// Write Data Test Time
		if (eAsyncDataWidthLast == EMIF_ASYNC_DATA_WIDTH_32) {
			writeMemData(FPGA_SRAM_START_ADDR, FPGA_SRAM_SIZE);
		} else if (eAsyncDataWidthLast == EMIF_ASYNC_DATA_WIDTH_16) {
			writeMemData16(FPGA_SRAM_START_ADDR, FPGA_SRAM_SIZE / 2);
		}
	}

	if (testMask & 0x20) {
		// Read Data Test Time
		if (eAsyncDataWidthLast == EMIF_ASYNC_DATA_WIDTH_32) {
			readMemData(FPGA_SRAM_START_ADDR, FPGA_SRAM_SIZE);
		} else if (eAsyncDataWidthLast == EMIF_ASYNC_DATA_WIDTH_16) {
			readMemData16(FPGA_SRAM_START_ADDR, FPGA_SRAM_SIZE / 2);
		}
	}

	if (errCountGlobal == 0x0) {
		testStatusGlobal = TEST_PASS;
	}

	return errCountResult;
}


//
// Write Memory - This function performs simple write word accesses
// to memory for time test. 131.26ms / 0x100000 1Mb addresses -> 125.179 nSec/32Bit Word
//
void writeMemData(uint32_t startAddr, uint32_t memSize)
{
    uint32_t memWriteData;
    uint32_t *memPtr;
    uint32_t *memPtrEnd;

    memPtr = (uint32_t *)startAddr;
    memPtrEnd = memPtr;
    memPtrEnd += memSize/sizeof(uint32_t);
    memWriteData = 0x01234567;
    //
    // Write data to memory.
    //
    while (memPtr < memPtrEnd)
    {
        *memPtr++ = memWriteData;
    }
}
void writeMemData16(uint32_t startAddr, uint32_t memSize)
{
    uint16_t memWriteData;
    uint16_t *memPtr;
    uint16_t *memPtrEnd;

    memPtr = (uint16_t *)startAddr;
    memPtrEnd = memPtr;
    memPtrEnd += memSize/sizeof(uint16_t);
    memWriteData = 0x3456;
    //
    // Write data to memory.
    //
    while (memPtr < memPtrEnd)
    {
        *memPtr++ = memWriteData;
    }
}

//
// Read Memory - This function performs simple read word accesses
// from memory for time test. 162.68ms / 0x100000 1Mb addresses -> 155.144 nSec/32Bit Word
//
volatile uint32_t memReadDataTimeTest;
void readMemData(uint32_t startAddr, uint32_t memSize)
{
    //volatile uint32_t memReadData;
    uint32_t *memPtr;
    uint32_t *memPtrEnd;

    memPtr = (uint32_t *)startAddr;
    memPtrEnd = memPtr;
    memPtrEnd += memSize/sizeof(uint32_t);
    //
    // Read data from memory.
    //
    while (memPtr < memPtrEnd)
    {
        memReadDataTimeTest = *memPtr++;
    }
}

void readMemData16(uint32_t startAddr, uint32_t memSize)
{
    //volatile uint32_t memReadData;
    uint16_t *memPtr;
    uint16_t *memPtrEnd;

    memPtr = (uint16_t *)startAddr;
    memPtrEnd = memPtr;
    memPtrEnd += memSize/sizeof(uint16_t);
    //
    // Read data from memory.
    //
    while (memPtr < memPtrEnd)
    {
        memReadDataTimeTest = *memPtr++;
    }
}



//
// Read Write Memory - This function performs simple read/write word accesses
// to memory.
//
uint32_t readWriteMem(uint32_t startAddr, uint32_t memSize)
{
    uint32_t errors = 0;
    uint32_t memReadData;
    uint32_t memWriteData;
    uint32_t *memPtr;
    uint32_t i;

    memPtr = (uint32_t *)startAddr;
    memPtrStart = (volatile uint32_t*)memPtr;
    //
    // Write data to memory.
    //
    memWriteData = 0x01234567;
    for(i = 0; i < memSize; i++)
    {
        *memPtr++ = memWriteData;
        memWriteData += 0x11111111;
    }

    //
    // Verify data written to memory.
    //
    memWriteData = 0x01234567;
    memPtr = (uint32_t *)startAddr;
    for(i = 0; i < memSize; i++)
    {
        memReadData = *memPtr;
        if(memReadData != memWriteData)
        {
            errors++;
            //return(1);
        }
        memPtr++;
        memWriteData += 0x11111111;
    }
    return(errors);
}

uint32_t readWriteMem16(uint32_t startAddr, uint32_t memSize)
{
	uint32_t errors = 0;
	uint16_t memReadData;
	uint16_t memWriteData;
	uint16_t *memPtr;
	uint32_t i;

	memPtr = (uint16_t*) startAddr;
	memPtrStart = (volatile uint32_t*) memPtr;

	// Write data to memory.
	memWriteData = (uint16_t) 0x01234567;
	//memWriteData = (uint16_t) 0x02468ACE;
	for (i = 0; i < memSize; i++) {
		*memPtr++ = memWriteData;
		memWriteData += 0x11111111;
	}

	// Verify data written to memory.
	memWriteData = (uint16_t) 0x01234567;
	//memWriteData = (uint16_t) 0x02468ACE;
	memPtr = (uint16_t*) startAddr;
	for (i = 0; i < memSize; i++) {
		memReadData = *memPtr;
		if (memReadData != memWriteData) {
			errors++;
			//return(1);
		}
		memPtr++;
		memWriteData += 0x11111111;
	}
	return (errors);
}

//
// Walk Memory Data - This function performs a walking 0 & 1 on data lines
// for SRAM RD & WR.
//
uint32_t walkMemData(uint32_t startAddr, uint32_t memSize)
{
    uint32_t errors = 0;
    uint32_t sramReadData;
    uint32_t sramWriteData;
    uint32_t i;
    uint32_t k;
    uint32_t m;
    uint32_t *memPtr;
    uint32_t *memPtrIter;

    memPtr = (uint32_t *)startAddr;

    for(i = 0; i < memSize; i = i + 64)
    {
        for(m = 0; m < 2; m++)
        {
            //
            // Write loop.
            //
            memPtrIter = memPtr;
            sramWriteData = 0x01;
            for(k = 0; k < 32; k++)
            {
                if(m == 0)
                {
                    *memPtrIter++ = sramWriteData;
                }
                else
                {
                    *memPtrIter++ = ~sramWriteData;
                }
                sramWriteData = sramWriteData << 1;
            }

            //
            // Read loop.
            //
            memPtrIter = memPtr;
            sramWriteData = 0x01;
            for(k = 0; k < 32; k++)
            {
                sramReadData = *memPtrIter;
                if(m == 1)
                {
                    sramReadData = ~sramReadData;
                }
                if(sramReadData != sramWriteData)
                {
                    errors++;
                    //return(1);
                }
                memPtrIter++;
                sramWriteData = sramWriteData << 1;
            }
        }
        memPtr = memPtrIter;
    }
    return(errors);
}

uint32_t walkMemData16(uint32_t startAddr, uint32_t memSize)
{
    uint32_t errors = 0;
    uint16_t sramReadData;
    uint16_t sramWriteData;
    uint32_t i;
    uint32_t k;
    uint32_t m;
    uint16_t *memPtr;
    uint16_t *memPtrIter;

    memPtr = (uint16_t *)startAddr;

    for(i = 0; i < memSize; i = i + 32)
    {
        for(m = 0; m < 2; m++)
        {
            //
            // Write loop.
            //
            memPtrIter = memPtr;
            sramWriteData = 0x01;
            for(k = 0; k < 16; k++)
            {
                if(m == 0)
                {
                    *memPtrIter++ = sramWriteData;
                }
                else
                {
                    *memPtrIter++ = ~sramWriteData;
                }
                sramWriteData = sramWriteData << 1;
            }

            //
            // Read loop.
            //
            memPtrIter = memPtr;
            sramWriteData = 0x01;
            for(k = 0; k < 16; k++)
            {
                sramReadData = *memPtrIter;
                if(m == 1)
                {
                    sramReadData = ~sramReadData;
                }
                if(sramReadData != sramWriteData)
                {
                    errors++;
                    //return(1);
                }
                memPtrIter++;
                sramWriteData = sramWriteData << 1;
            }
        }
        memPtr = memPtrIter;
    }
    return(errors);
}

//
// Walk Memory Addresses - This function performs a toggle on each address bit.
//
uint32_t walkMemAddr(uint32_t startAddr, uint32_t addrSize)
{
    uint32_t errors = 0;
    uint32_t sramReadData;
    uint32_t sramWriteData;
    uint32_t k;
    uint32_t xshift;
    uint32_t *memPtr;
    uint32_t *memPtrIter;

    memPtr = (uint32_t *)startAddr;

    //
    // Write loop.
    //
    xshift = 0x00000001;
    sramWriteData = 0x00;
    for(k = 0; k < addrSize; k++)
    {
        memPtrIter = (uint32_t *)(startAddr + xshift);
        *memPtrIter = sramWriteData++;
        xshift = xshift << 1;
    }

    //
    // Read loop.
    //
    memPtrIter = memPtr;
    xshift = 0x00000001;
    sramWriteData = 0x00;
    for(k = 0; k < addrSize; k++)
    {
        memPtrIter = (uint32_t *)(startAddr + xshift);
        sramReadData = *memPtrIter;
        if(sramReadData != sramWriteData)
        {
            errors++;
            //return(1);
        }
        xshift = xshift << 1;
        sramWriteData++;
    }
    return(errors);
}

uint32_t walkMemAddr16(uint32_t startAddr, uint32_t addrSize)
{
    uint32_t errors = 0;
    uint16_t sramReadData;
    uint16_t sramWriteData;
    uint32_t k;
    uint32_t xshift;
    uint16_t *memPtr;
    uint16_t *memPtrIter;

    memPtr = (uint16_t *)startAddr;

    //
    // Write loop.
    //
    xshift = 0x00000001;
    sramWriteData = 0x00;
    for(k = 0; k < addrSize; k++)
    {
        memPtrIter = (uint16_t *)(startAddr + xshift);
        *memPtrIter = sramWriteData++;
        xshift = xshift << 1;
    }

    //
    // Read loop.
    //
    memPtrIter = memPtr;
    xshift = 0x00000001;
    sramWriteData = 0x00;
    for(k = 0; k < addrSize; k++)
    {
        memPtrIter = (uint16_t *)(startAddr + xshift);
        sramReadData = *memPtrIter;
        if(sramReadData != sramWriteData)
        {
            errors++;
            //return(1);
        }
        xshift = xshift << 1;
        sramWriteData++;
    }
    return(errors);
}

//
// Access Memory Data - This function performs different data type
// (HALFWORD/WORD) access.
//



uint32_t accessMemData(uint32_t startAddr, uint32_t sizeToCheck)
{

    uint32_t errors = 0;
    uint16_t memRdShort;
    uint32_t memRdLong;
    uint16_t memWrShort;
    uint32_t memWrLong;
    uint32_t i;
    uint16_t *memPtrShort;
    uint32_t *memPtrLong;

    //
    // Write short data.
    //
    memPtrShort = (uint16_t *)startAddr;
    memWrShort = 0x0605;

    for(i = 0; i < sizeToCheck/2; i++)
    {
        *memPtrShort++ = memWrShort;
        memWrShort += 0x0202;
    }

    //
    // Write long data.
    //
    memPtrLong = (uint32_t *)memPtrShort;
    memWrLong = 0x0C0B0A09;
    for(i = 0; i < sizeToCheck/4; i++)
    {
        *memPtrLong++ = memWrLong;
        memWrLong += 0x04040404;
    }

    //
    // Read short data.
    //
    memPtrShort = (uint16_t *)startAddr;
    memWrShort = 0x0605;
    for(i = 0; i < sizeToCheck; i++)
    {
        memRdShort = *memPtrShort;
        if(memRdShort != memWrShort)
        {
            errors++;
            errors |= 0x10000000;
            //return(1);
        }
        memPtrShort++;
        memWrShort += 0x0202;
    }

    //
    // Read long data.
    //
    memPtrLong = (uint32_t *)startAddr;
    memWrLong = 0x08070605;
    for(i = 0; i < sizeToCheck/2; i++)
    {
        memRdLong = *memPtrLong;
        if(memRdLong != memWrLong)
        {
            errors++;
            //return(1);
        }
        memPtrLong++;
        memWrLong += 0x04040404;
    }
    return(errors);
}

uint32_t accessMemData16(uint32_t startAddr, uint32_t sizeToCheck)
{

    uint32_t errors = 0;
    uint16_t memRdShort;
    uint32_t memRdLong;
    uint16_t memWrShort;
    uint32_t memWrLong;
    uint32_t i;
    uint16_t *memPtrShort;
    uint32_t *memPtrLong;

    //
    // Write short data.
    //
    memPtrShort = (uint16_t *)startAddr;
    memWrShort = 0x0605;

    for(i = 0; i < sizeToCheck/2; i++)
    {
        *memPtrShort++ = memWrShort;
        memWrShort += 0x0202;
    }
    for(i = 0; i < sizeToCheck/2; i++)
    {
        *memPtrShort++ = memWrShort;
        memWrShort += 0x0202;
    }


#if 0

    //
    // Write long data.
    //
    memPtrLong = (uint32_t *)memPtrShort;

    memWrLong = memWrShort;
    memWrShort += 0x0202;
    memWrLong += ((uint32_t)memWrShort << 16);
    memWrShort += 0x0202;
    //memWrLong = 0x0C0B0A09;
    for(i = 0; i < sizeToCheck/4; i++)
    {
        *memPtrLong++ = memWrLong;
        //memWrLong += 0x04040404;
        memWrLong = memWrShort;
        memWrShort += 0x0202;
        memWrLong += ((uint32_t)memWrShort << 16);
        memWrShort += 0x0202;
    }

#endif

    //
    // Read short data.
    //
    memPtrShort = (uint16_t *)startAddr;
    memWrShort = 0x0605;
    for(i = 0; i < sizeToCheck; i++)
    {
        memRdShort = *memPtrShort;
        if(memRdShort != memWrShort)
        {
            errors++;
            errors |= 0x10000000;
            //return(1);
        }
        memPtrShort++;
        memWrShort += 0x0202;
    }

    //
    // Read long data.
    //
    memPtrLong = (uint32_t *)startAddr;
    //memWrLong = 0x08070605;
    memWrShort = 0x0605;
    memWrLong = memWrShort;
    memWrShort += 0x0202;
    memWrLong += ((uint32_t)memWrShort << 16);
    memWrShort += 0x0202;
    for(i = 0; i < sizeToCheck/2; i++)
    {
        memRdLong = *memPtrLong;
        if(memRdLong != memWrLong)
        {
            errors++;
            //return(1);
        }
        memPtrLong++;
        //memWrLong += 0x04040404;

        memWrLong = memWrShort;
        memWrShort += 0x0202;
        memWrLong += ((uint32_t)memWrShort << 16);
        memWrShort += 0x0202;
    }
    return(errors);
}

//
// End of File
//
