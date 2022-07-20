//###########################################################################
//
//On each timing sample the CPU1 writes a square and 3 sinewaves to the beginning
//of C1toC2 MSGRAM. CPU2 reads it and copy the same data at the beginning of
//C2toC1 MSGRAM.
//###########################################################################

#include "F2837xD_device.h"
#include "F2837xD_Gpio_defines.h"
#include "F2837xD_GlobalPrototypes.h"

#include "crc32.h"
#include "MB85RS4MT.h"
#include "MB85RS4MT_old.h"



#define LED1_PIN    18
#define LED2_PIN    17
#define LED3_PIN    16
#define LED4_PIN    19

#define LED_OFF     1
#define LED_ON      0


//#define STATREG_TEST
#define READ_WRITE_TEST1


uint16_t test_write_read = 0;

void add_crc32(uint16_t *data, unsigned int len);

//Definition of DELAY_US ---------
#define CPU_RATE   5.00L   // for a 200MHz CPU clock speed (SYSCLKOUT)
extern void F28x_usDelay(long LoopCount);
#define DELAY_US(A)  F28x_usDelay(((((long double) A * 1000.0L) / (long double)CPU_RATE) - 9.0L) / 5.0L)

uint16_t val;
uint16_t buf[10];
uint16_t dat[10] = {0x0001, 0x0203, 0x0405, 0x0607, 0x0809, 0x0A0B, 0x0C0D, 0x0E0F}; //Two extra words for CRC32
int res1, res2, res3;
uint32_t crc;

void main(void)
{
    int i=0, j=0;

    InitSysCtrl();                                      // Step 1. Initialize System Control:
                                                        // PLL, WatchDog, enable Peripheral Clocks
                                                        // This example function is found in the F2837xD_SysCtrl.c file.

    InitGpio();                                         // Step 2. Initialize GPIO:

    GPIO_SetupPinMux(LED1_PIN, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(LED1_PIN, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_WritePin(LED1_PIN, LED_OFF);

    GPIO_SetupPinMux(LED2_PIN, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(LED2_PIN, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_WritePin(LED2_PIN, LED_OFF);

    GPIO_SetupPinMux(LED3_PIN, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(LED3_PIN, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_WritePin(LED3_PIN, LED_OFF);

    GPIO_SetupPinMux(LED4_PIN, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(LED4_PIN, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_WritePin(LED4_PIN, LED_OFF);



    DINT;                                               // Step 3. Clear all interrupts and initialize PIE vector table:
                                                        // Disable CPU interrupts

    InitPieCtrl();                                      // Initialize the PIE control registers to their default state.
                                                        // The default state is all PIE interrupts disabled and flags
                                                        // are cleared.
                                                        // This function is found in the F2837xD_PieCtrl.c file.


    InitPieVectTable();                                 // Initialize the PIE vector table with pointers to the shell Interrupt
                                                        // Service Routines (ISR).
                                                        // This will populate the entire table, even if the interrupt
                                                        // is not used in this project.  This is useful for debug purposes.
                                                        // The shell ISR routines are found in F2837xD_DefaultIsr.c.
                                                        // This function is found in F2837xD_PieVect.c.

    EALLOW;
    PieVectTable.SPIC_RX_INT = &mb85rs4mt_RXFIFO_ISR;   // SPIC_RX FIFO ISR
    EDIS;

#ifndef _USE_LIBRARY
    InitSPIC_mb85rs4mt();                               // Initialize the SPIC for use with MB85RS4MT
#else
    MB85RS4MT_Init();
#endif

    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;                  // Enable the PIE block
    PieCtrlRegs.PIEIER6.bit.INTx9 = 1;                  // Enable PIE Group 6, INT 9 => SPIC_RX
    IER=M_INT6;                                         // Enable CPU INT6
    IFR = 0x0000;                                       // Clear all flags
    EINT;                                               // Enable Global Interrupts
    ERTM;                                               // We can debug ISRs

//#ifndef _USE_LIBRARY
    res1 = mb85rs4mt_write_enable();
//#else
//    res1 = MB85RS4MT_WriteEnable();
//#endif

    while(1)
    {
        GPIO_WritePin(LED1_PIN, !GPIO_ReadPin(LED1_PIN));

        if (test_write_read)
        {
            test_write_read = 0;

            GPIO_WritePin(LED2_PIN, LED_OFF);
            GPIO_WritePin(LED3_PIN, LED_ON);

            for(i=0;i<8;i++)
                dat[i] = dat[i]+ j;                          // Update dat

            add_crc32(dat, 8);                                // Add CRC32 at the end of the dat
            res1 = mb85rs4mt_write_data(256, dat, 10);        // Write the dat and the CRC32


            EALLOW; EDIS;                                  // Check here

            for(i=0;i<10;i++)
                buf[i] = 0;                                // Clear read buffer

            res2 = mb85rs4mt_read_data(256, buf, 10);      // Read from memory


            #ifndef _USE_LIBRARY
            //DELAY_US(1000*100);  //0.1 sec
            while(mb85rs4mt_busy()) {}
            #else
            while(MB85RS4MT_Busy()) {}
            #endif


            crc = crc32_halfbyte(buf, 8, 0);               //CRC32 of the dat

            if(((uint16_t)crc != buf[8]) || ((uint16_t)(crc>>16) != buf[9]) ){

                EALLOW; EDIS;                              // Check here
                GPIO_WritePin(LED4_PIN, LED_ON);
            }
            else
            {
                GPIO_WritePin(LED4_PIN, LED_OFF);
            }

        }



        DELAY_US(1000*1000);                           // 1 sec

    }

    for(j=0; j < 100; j++)
    {




#ifdef READ_WRITE_TEST1
    for(i=0;i<8;i++)
        dat[i] = dat[i]+ j;                          // Update dat

    add_crc32(dat, 8);                                // Add CRC32 at the end of the dat
    res1 = mb85rs4mt_write_data(256, dat, 10);        // Write the dat and the CRC32
#endif

#ifdef STATREG_TEST
        res = mb85rs4mt_read_statreg(&val);
        res = mb85rs4mt_write_statreg(0x3+((i++%8)<<4));
#endif

#ifdef READ_WRITE_TEST1

        DELAY_US(1000*100);                            // 0.1 sec

        EALLOW; EDIS;                                  // Check here

        for(i=0;i<10;i++)
            buf[i] = 0;                                // Clear read buffer

        res2 = mb85rs4mt_read_data(256, buf, 10);      // Read from memory


        DELAY_US(1000*1000);                           // 1 sec

        crc = crc32_halfbyte(buf, 8, 0);               //CRC32 of the dat

        if(((uint16_t)crc != buf[8]) || ((uint16_t)(crc>>16) != buf[9]) ){

            EALLOW; EDIS;                              // Check here
        }

#endif

    }
}
/*
 * Append the CRC32 of the dat to the end of the array.
 * The array should preallocate memory for the two extra 16 bit words at the end
 */
void add_crc32(uint16_t *data, unsigned int len){
    uint32_t crc;

    crc = crc32_halfbyte(data, len, 0);
    data[len] = (uint16_t)crc;
    data[len+1] = (uint16_t)(crc>>16);
}

