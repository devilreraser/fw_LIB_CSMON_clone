//#############################################################################
//
// FILE:   emif_ex3_16bit_sdram_far.c
//
// TITLE:  EMIF1 module accessing 16bit SDRAM using memcpy_fast_far().
//
//! \addtogroup driver_example_list
//! <h1> EMIF1 module accessing 16bit SDRAM using memcpy_fast_far(). </h1>
//!
//! This example configures EMIF1 in 16bit SYNC mode and uses CS0 as chip
//! enable.It will first write to an array in the SDRAM and then read it
//! back using the FPU function, memcpy_fast_far(), for both operations.
//!
//! The buffer in SDRAM will be placed in the .farbss memory on account
//! of the fact that its assigned the attribute "far" indicating it lies
//! beyond the 22-bit program address space. The compiler will take care
//! to avoid using instructions such as PREAD, which uses the Program
//! Read Bus, or addressing modes restricted to the lower 22-bit space
//! when accessing data with the attribute "far".
//!
//! \note The memory space beyond 22-bits must be treated as data space
//! for load/store operations only. The user is cautioned against using
//! this space for either instructions or working memory.
//!
//! \b External \b Connections \n
//!  - External SDR-SDRAM memory (MT48LC32M16A2 -75) daughter card
//!
//! \b Watch \b Variables \n
//! - \b testStatusGlobal - Equivalent to \b TEST_PASS if test finished
//!                         correctly, else the value is set to \b TEST_FAIL
//! - \b errCountGlobal - Error counter
//!
//
//#############################################################################
// $TI Release: F2837xD Support Library v3.09.00.00 $
// $Release Date: Thu Mar 19 07:35:24 IST 2020 $
// $Copyright:
// Copyright (C) 2013-2020 Texas Instruments Incorporated - http://www.ti.com/
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


//
// FPU header file to access memcpy_fast_far().
//
#include "fpu_vector.h"

//
// Defines
//
#define TEST_PASS 0xABCDABCD
#define TEST_FAIL 0xDEADDEAD
#define MEM_BUFFER_SIZE 0x500

//
// Globals
//
extern uint16_t errCountGlobal;
extern uint32_t testStatusGlobal;

//
// Buffer in local memory.
//
uint16_t localRAMBuf[MEM_BUFFER_SIZE];

//
// Buffer in far memory.
//
__attribute__((far)) volatile uint16_t extSDRAMBuf[MEM_BUFFER_SIZE];
#pragma DATA_SECTION(localRAMBuf, "ramgs0");

//
// Function Prototypes
//
extern void setupEMIF2PinmuxSync16Bit(void);
void clearDataBuffer(uint32_t memSize);
uint16_t readWriteSyncMemory(uint32_t memSize);



////////////////////////////////////////
//         Library mistake            //
////////////////////////////////////////
static inline void EMIF_2_setAccessProtection( uint32_t configBase, uint16_t access )
{
	uint16_t temp;

	// Check the arguments.
	ASSERT( EMIF_isEMIF2ConfigBaseValid(configBase) );
	ASSERT(access <= EMIF_ACCPROT0_MASK_EMIF2);
	temp = EMIF_ACCPROT0_MASK_EMIF2;

	// Sets the bits that enables access protection config.
	EALLOW;
	HWREGH(configBase + MEMCFG_O_EMIF2ACCPROT0) = (HWREGH(configBase + MEMCFG_O_EMIF2ACCPROT0) & ~(temp)) | access;
	EDIS;
}

static inline void EMIF_2_commitAccessConfig( uint32_t configBase )
{
	// Check the arguments.
	ASSERT( EMIF_isEMIF2ConfigBaseValid(configBase) );

	// Sets the bits that commits access protection config.
	EALLOW;
	HWREGH(configBase + MEMCFG_O_EMIF2COMMIT) |= MEMCFG_EMIF2COMMIT_COMMIT_EMIF2;
	EDIS;
}

static inline void EMIF_2_lockAccessConfig( uint32_t configBase )
{
	// Check the arguments.
	ASSERT( EMIF_isEMIF2ConfigBaseValid(configBase) );

	// Sets the bits that locks access protection config.
	EALLOW;
	HWREGH(configBase + MEMCFG_O_EMIF2LOCK) |= MEMCFG_EMIF2LOCK_LOCK_EMIF2;
	EDIS;
}

//
// EMIF_2 in SDRAM mode
//
void EMIF_2_vInit16_SDRam_Module( EMIF_SyncTimingParams tParam, EMIF_SyncConfig sdConfig )
{
    // Configure to run EMIF2 on half Rate. (EMIF2CLK = CPU1SYSCLK/2)
    SysCtl_setEMIF2ClockDivider( SYSCTL_EMIF2CLK_DIV_2 );

    // Disable Access Protection. (CPU_FETCH/CPU_WR/DMA_WR)
    EMIF_2_setAccessProtection( EMIF2CONFIG_BASE, 0x0 );

    // Commit the configuration related to protection. Till this bit remains set content of EMIF1ACCPROT0 register can't be changed.
    EMIF_2_commitAccessConfig( EMIF2CONFIG_BASE );

    // Lock the configuration so that EMIF2COMMIT register can't be changed any more.
    EMIF_2_lockAccessConfig( EMIF2CONFIG_BASE );

    // Configure GPIO pins for EMIF2.
    setupEMIF2PinmuxSync16Bit();

    EMIF_setSyncTimingParams( EMIF2_BASE, &tParam );

    // Configure Self Refresh exit timing. Txsr = 70ns = 0x7.
    EMIF_setSyncSelfRefreshExitTmng( EMIF2_BASE, 0x7U );

    // Configure Refresh Rate. Tref = 64ms for 8192 ROW, RR = 64000*100(Tfrq)/8192 = 781.25 (0x30E).
    //EMIF_setSyncRefreshRate( (uint32_t) EMIF2_BASE, (uint16_t) 781 );
    EMIF_setSyncRefreshRate( EMIF2_BASE, 781 );

    // Configure SDRAM parameters. PAGESIZE=2 (1024 elements per ROW), IBANK = 2 (4 BANK), CL = 3, NM = 1 (16bit).
    EMIF_setSyncMemoryConfig( EMIF2_BASE, &sdConfig );

    //
    EMIF_enableSyncSelfRefresh( EMIF2_BASE );
    EMIF_enableSyncPowerDown( EMIF2_BASE );

//    EMIF_enableSyncRefreshInPowerDown( EMIF2_BASE );
//    uint32_t totalAccesses = EMIF_getSyncTotalAccesses( EMIF2_BASE );
//    uint32_t totalActivateAccesses = EMIF_getSyncTotalActivateAccesses( EMIF2_BASE );
}


//
// Clear Data Buffer - This function clears the memory location of size memSize.
//
void clearDataBuffer(uint32_t memSize)
{
    uint32_t i;
    uint32_t memWdl = 0x0;

    // Clear far memory buffer.
    for(i=0; i < memSize; i++)
    {
        //memcpy_fast_far((extSDRAMBuf + i), &memWdl, 2);
    }

    // Clear local memory buffer.
    for(i = 0; i < memSize; i++)
    {
        localRAMBuf[i] = memWdl;
    }
}


//
// Read Write Sync Memory - This function writes data into memory & verifies the written data.
//
uint16_t readWriteSyncMemory(uint32_t memSize)
{
    uint32_t memWdl;
    uint32_t i;

    // Fill far memory buffer with data.
    memWdl = 0x0;
    for(i=0; i < memSize; i++)
    {
        //memcpy_fast_far((extSDRAMBuf + i), &memWdl, 2);
        memWdl += 0x00050001;
    }

    // Read far memory buffer into local buffer and verify data.
    memWdl = 0x0;
    for(i=0; i < memSize; i++)
    {
        //memcpy_fast_far((localRAMBuf + i), (extSDRAMBuf + i), 2);

        // Return error if read data is incorrect.
        if(localRAMBuf[i] != memWdl)
        {
            return(1);
        }
        memWdl += 0x00050001;
    }
    return(0);
}

//
// End of File
//
