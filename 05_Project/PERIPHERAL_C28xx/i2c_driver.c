/* *****************************************************************************
 * File:   i2c_driver.c
 * Author: Dimitar Lilov
 *
 * Created on 2021 02 23 14:55
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
#include "device.h"
#include "i2c_driver.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define I2CA                        0
#define I2CB                        1
#define I2C_COUNT                   2       //I2CA, I2CB

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */
//
// Defines
//
#define SLAVE_ADDRESS               0x50
#define REGISTER_HIGH_ADDR          0x00
#define REGISTER_LOW_ADDR           0x00
#define REGISTER_ADDRESS_SIZE       1
#define NUM_BYTES                   8
#define MAX_ADDR_SIZE               2       //
#define MAX_BUFFER_SIZE             14      // Max is currently 14 because if
                                            // 2 address bytes and the 16-byte
                                            // FIFO

//
// I2C message states for I2CMsg struct
//
#define MSG_STATUS_INACTIVE         0x0000 // Message not in use, do not send
#define MSG_STATUS_SEND_WITHSTOP    0x0010 // Send message with stop bit
#define MSG_STATUS_WRITE_BUSY       0x0011 // Message sent, wait for stop
#define MSG_STATUS_SEND_NOSTOP      0x0020 // Send message without stop bit
#define MSG_STATUS_SEND_NOSTOP_BUSY 0x0021 // Message sent, wait for ARDY
#define MSG_STATUS_RESTART          0x0022 // Ready to become master-receiver
#define MSG_STATUS_READ_BUSY        0x0023 // Wait for stop before reading data

//
// Error messages for read and write functions
//
#define ERROR_BUS_BUSY              0x1000
#define ERROR_STOP_NOT_READY        0x5555
#define SUCCESS                     0x0000


/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
//
// Typedefs
//
struct I2CMsg
{
    uint_least8_t* pData;
    uint16_t* pStatus;
    uint16_t msgStatus;                  // Word stating what state msg is in.
                                         // See MSG_STATUS_* defines above.
    uint16_t registerAddrBytes;          // memory/register address size in bytes.
    uint16_t slaveAddr;                  // Slave address tied to the message.
    uint16_t numBytes;                   // Number of valid bytes in message.
    uint_least8_t registerAddr[MAX_ADDR_SIZE];// memory/register address of data associated
                                         // with message (lowest byte - lowest index).
    uint_least8_t msgBuffer[MAX_BUFFER_SIZE]; // Array holding message data.
};

/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
//
// Globals
//
struct I2CMsg i2cMsgOut[I2C_COUNT] =
{
    {
        NULL,
        NULL,
        MSG_STATUS_INACTIVE,
        REGISTER_ADDRESS_SIZE,
        SLAVE_ADDRESS,
        NUM_BYTES,
        {
            REGISTER_LOW_ADDR,
            REGISTER_HIGH_ADDR
        },
        {
            0x00,                // Message bytes
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00
        }
    },

    {
        NULL,
        NULL,
        MSG_STATUS_INACTIVE,
        REGISTER_ADDRESS_SIZE,
        SLAVE_ADDRESS,
        NUM_BYTES,
        {
            REGISTER_LOW_ADDR,
            REGISTER_HIGH_ADDR
        },
        {
            0x00,                // Message bytes
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00
        }
    },
};

struct I2CMsg i2cMsgIn[I2C_COUNT]  =
{
    {
        NULL,
        NULL,
        MSG_STATUS_INACTIVE,
        REGISTER_ADDRESS_SIZE,
        SLAVE_ADDRESS,
        NUM_BYTES,
        {
            REGISTER_LOW_ADDR,
            REGISTER_HIGH_ADDR
        },
    },

    {
        NULL,
        NULL,
        MSG_STATUS_INACTIVE,
        REGISTER_ADDRESS_SIZE,
        SLAVE_ADDRESS,
        NUM_BYTES,
        {
            REGISTER_LOW_ADDR,
            REGISTER_HIGH_ADDR
        },
    },
};

struct I2CMsg *currentMsgPtr[I2C_COUNT];                   // Used in interrupt

uint16_t passCount[I2C_COUNT] = {0};
uint16_t failCount[I2C_COUNT] = {0};


/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */
void initI2C(uint32_t base);
uint16_t readData(uint32_t base, struct I2CMsg *msg);
uint16_t writeData(uint32_t base, struct I2CMsg *msg);

void fail(void);
void pass(void);

__interrupt void i2cAISR(void);
__interrupt void i2cBISR(void);


/* *****************************************************************************
 * Functions Local
 **************************************************************************** */
void initI2C(uint32_t base)
{
    //
    // Must put I2C into reset before configuring it
    //
    I2C_disableModule(base);

    //
    // I2C configuration. Use a 400kHz I2CCLK with a 33% duty cycle.
    //
    I2C_initMaster(base, DEVICE_SYSCLK_FREQ, 400000, I2C_DUTYCYCLE_33);
    I2C_setBitCount(base, I2C_BITCOUNT_8);
    I2C_setSlaveAddress(base, SLAVE_ADDRESS);
    I2C_setEmulationMode(base, I2C_EMULATION_FREE_RUN);

    //
    // Enable stop condition and register-access-ready interrupts
    //
    I2C_enableInterrupt(base, I2C_INT_STOP_CONDITION |
                        I2C_INT_REG_ACCESS_RDY);

    //
    // FIFO configuration
    //
    I2C_enableFIFO(base);
    I2C_clearInterruptStatus(base, I2C_INT_RXFF | I2C_INT_TXFF);

    //
    // Configuration complete. Enable the module.
    //
    I2C_enableModule(base);
}

//
// writeData - Function to send the data that is to be written to the I2C
//
uint16_t writeData(uint32_t base, struct I2CMsg *msg)
{
    uint16_t i;

    //
    // Wait until the STP bit is cleared from any previous master
    // communication. Clearing of this bit by the module is delayed until after
    // the SCD bit is set. If this bit is not checked prior to initiating a new
    // message, the I2C could get confused.
    //
    if(I2C_getStopConditionStatus(base))
    {
        return(ERROR_STOP_NOT_READY);
    }

    //
    // Setup slave address
    //
    I2C_setSlaveAddress(base, msg->slaveAddr);

    //
    // Check if bus busy
    //
    if(I2C_isBusBusy(base))
    {
        return(ERROR_BUS_BUSY);
    }

    //
    // Setup number of bytes to send msgBuffer and address
    //

    //
    // Setup data to send
    //
    i = msg->registerAddrBytes;
    if (i > MAX_ADDR_SIZE)
    {
        i = MAX_ADDR_SIZE;
    }
    I2C_setDataCount(base, (msg->numBytes + i));
    while(i > 0)
    {
        I2C_putData(base, msg->registerAddr[--i]);
    }

    for (i = 0; i < msg->numBytes; i++)
    {
        I2C_putData(base, msg->msgBuffer[i]);
    }

    //
    // Send start as master transmitter
    //
    I2C_setConfig(base, I2C_MASTER_SEND_MODE);
    I2C_sendStartCondition(base);
    I2C_sendStopCondition(base);

    return(SUCCESS);
}

//
// readData - Function to prepare for the data that is to be read from the I2C
//
uint16_t readData(uint32_t base, struct I2CMsg *msg)
{
    uint16_t i;

    //
    // Wait until the STP bit is cleared from any previous master
    // communication. Clearing of this bit by the module is delayed until after
    // the SCD bit is set. If this bit is not checked prior to initiating a new
    // message, the I2C could get confused.
    //
    if(I2C_getStopConditionStatus(base))
    {
        return(ERROR_STOP_NOT_READY);
    }

    //
    // Setup slave address
    //
    I2C_setSlaveAddress(base, msg->slaveAddr);

    //
    // If we are in the address setup phase, send the address without a
    // stop condition.
    //
    if(msg->msgStatus == MSG_STATUS_SEND_NOSTOP)
    {
        //
        // Check if bus busy
        //
        if(I2C_isBusBusy(base))
        {
            return(ERROR_BUS_BUSY);
        }

        //
        // Send data to setup EEPROM address
        //
        i = msg->registerAddrBytes;
        if (i > MAX_ADDR_SIZE)
        {
            i = MAX_ADDR_SIZE;
        }
        I2C_setDataCount(base, i);
        while(i > 0)
        {
            I2C_putData(base, msg->registerAddr[--i]);
        }
        I2C_setConfig(base, I2C_MASTER_SEND_MODE);
        I2C_sendStartCondition(base);
    }
    else if(msg->msgStatus == MSG_STATUS_RESTART)
    {
        //
        // Address setup phase has completed. Now setup how many bytes expected
        // and send restart as master-receiver.
        //
        I2C_setDataCount(base, (msg->numBytes));
        I2C_setConfig(base, I2C_MASTER_RECEIVE_MODE);
        I2C_sendStartCondition(base);
        I2C_sendStopCondition(base);
    }

    return(SUCCESS);
}

//
// i2cAISR - I2C A ISR (non-FIFO)
//
__interrupt void i2cAISR(void)
{
    I2C_InterruptSource intSource;
    uint16_t i;

    //
    // Read interrupt source
    //
    intSource = I2C_getInterruptSource(I2CA_BASE);

    //
    // Interrupt source = stop condition detected
    //
    if(intSource == I2C_INTSRC_STOP_CONDITION)
    {
        //
        // If completed message was writing data, reset msg to inactive state
        //
        if(currentMsgPtr[I2CA]->msgStatus == MSG_STATUS_WRITE_BUSY)
        {
            currentMsgPtr[I2CA]->msgStatus = MSG_STATUS_INACTIVE;
            if (currentMsgPtr[I2CA]->pStatus != NULL)
            {
                *currentMsgPtr[I2CA]->pStatus = MSG_STATUS_INACTIVE;
            }
        }
        else
        {
            //
            // If a message receives a NACK during the address setup portion of
            // the I2C Slave read, the code further below included in the register
            // access ready interrupt source code will generate a stop
            // condition. After the stop condition is received (here), set the
            // message status to try again. User may want to limit the number
            // of retries before generating an error.
            //
            if(currentMsgPtr[I2CA]->msgStatus == MSG_STATUS_SEND_NOSTOP_BUSY)
            {
                currentMsgPtr[I2CA]->msgStatus = MSG_STATUS_SEND_NOSTOP;
            }
            //
            // If completed message was reading I2C Slave data, reset message to
            // inactive state and read data from FIFO.
            //
            else if(currentMsgPtr[I2CA]->msgStatus == MSG_STATUS_READ_BUSY)
            {
                currentMsgPtr[I2CA]->msgStatus = MSG_STATUS_INACTIVE;
                if (currentMsgPtr[I2CA]->pStatus != NULL)
                {
                    *currentMsgPtr[I2CA]->pStatus = MSG_STATUS_INACTIVE;
                }

                for(i=0; (i < currentMsgPtr[I2CA]->numBytes) && (i < NUM_BYTES); i++)
                {
                    currentMsgPtr[I2CA]->msgBuffer[i] = I2C_getData(I2CA_BASE);
                    if (currentMsgPtr[I2CA]->pData != NULL)
                    {
                        currentMsgPtr[I2CA]->pData[i] = currentMsgPtr[I2CA]->msgBuffer[i];
                    }
                }


                //
                // Check received data
                //


#if 0
                for(i=0; i < NUM_BYTES; i++)
                {
                    if(i2cMsgIn[I2CA].msgBuffer[i] == i2cMsgOut[I2CA].msgBuffer[i])
                    {
                        passCount[I2CA]++;
                    }
                    else
                    {
                        failCount[I2CA]++;
                    }
                }

                if(passCount[I2CA] == NUM_BYTES)
                {
                    pass();
                }
                else
                {
                    fail();
                }
#endif
            }
        }
    }
    //
    // Interrupt source = Register Access Ready
    //
    // This interrupt is used to determine when the Register/Memory address setup
    // portion of the read data communication is complete. Since no stop bit
    // is commanded, this flag tells us when the message has been sent
    // instead of the SCD flag.
    //
    else if(intSource == I2C_INTSRC_REG_ACCESS_RDY)
    {
        //
        // If a NACK is received, clear the NACK bit and command a stop.
        // Otherwise, move on to the read data portion of the communication.
        //
        if((I2C_getStatus(I2CA_BASE) & I2C_STS_NO_ACK) != 0)
        {
            I2C_sendStopCondition(I2CA_BASE);
            I2C_clearStatus(I2CA_BASE, I2C_STS_NO_ACK);
        }
        else if(currentMsgPtr[I2CA]->msgStatus == MSG_STATUS_SEND_NOSTOP_BUSY)
        {
            currentMsgPtr[I2CA]->msgStatus = MSG_STATUS_RESTART;
        }
    }
    else
    {
        //
        // Generate some error from invalid interrupt source
        //
        asm("   ESTOP0");
    }

    //
    // Issue ACK to enable future group 8 interrupts
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP8);
}

//
// i2cBISR - I2C B ISR (non-FIFO)
//
__interrupt void i2cBISR(void)
{
    I2C_InterruptSource intSource;
    uint16_t i;

    //
    // Read interrupt source
    //
    intSource = I2C_getInterruptSource(I2CB_BASE);

    //
    // Interrupt source = stop condition detected
    //
    if(intSource == I2C_INTSRC_STOP_CONDITION)
    {
        //
        // If completed message was writing data, reset msg to inactive state
        //
        if(currentMsgPtr[I2CB]->msgStatus == MSG_STATUS_WRITE_BUSY)
        {
            currentMsgPtr[I2CB]->msgStatus = MSG_STATUS_INACTIVE;

            if (currentMsgPtr[I2CB]->pStatus != NULL)
            {
                *currentMsgPtr[I2CB]->pStatus = MSG_STATUS_INACTIVE;
            }
        }
        else
        {
            //
            // If a message receives a NACK during the address setup portion of
            // the I2C Slave read, the code further below included in the register
            // access ready interrupt source code will generate a stop
            // condition. After the stop condition is received (here), set the
            // message status to try again. User may want to limit the number
            // of retries before generating an error.
            //
            if(currentMsgPtr[I2CB]->msgStatus == MSG_STATUS_SEND_NOSTOP_BUSY)
            {
                currentMsgPtr[I2CB]->msgStatus = MSG_STATUS_SEND_NOSTOP;
            }
            //
            // If completed message was reading I2C Slave data, reset message to
            // inactive state and read data from FIFO.
            //
            else if(currentMsgPtr[I2CB]->msgStatus == MSG_STATUS_READ_BUSY)
            {
                currentMsgPtr[I2CB]->msgStatus = MSG_STATUS_INACTIVE;
                if (currentMsgPtr[I2CB]->pStatus != NULL)
                {
                    *currentMsgPtr[I2CB]->pStatus = MSG_STATUS_INACTIVE;
                }

                for(i=0; (i < currentMsgPtr[I2CB]->numBytes) && (i < NUM_BYTES); i++)
                {
                    currentMsgPtr[I2CB]->msgBuffer[i] = I2C_getData(I2CB_BASE);
                    if (currentMsgPtr[I2CB]->pData != NULL)
                    {
                        currentMsgPtr[I2CB]->pData[i] = currentMsgPtr[I2CB]->msgBuffer[i];
                    }
                }

                //
                // Check received data
                //
#if 0
                for(i=0; i < NUM_BYTES; i++)
                {
                    if(i2cMsgIn[I2CB].msgBuffer[i] == i2cMsgOut[I2CB].msgBuffer[i])
                    {
                        passCount[I2CB]++;
                    }
                    else
                    {
                        failCount[I2CB]++;
                    }
                }

                if(passCount[I2CB] == NUM_BYTES)
                {
                    pass();
                }
                else
                {
                    fail();
                }
#endif
            }
        }
    }
    //
    // Interrupt source = Register Access Ready
    //
    // This interrupt is used to determine when the I2C Register/Memory address setup
    // portion of the read data communication is complete. Since no stop bit
    // is commanded, this flag tells us when the message has been sent
    // instead of the SCD flag.
    //
    else if(intSource == I2C_INTSRC_REG_ACCESS_RDY)
    {
        //
        // If a NACK is received, clear the NACK bit and command a stop.
        // Otherwise, move on to the read data portion of the communication.
        //
        if((I2C_getStatus(I2CB_BASE) & I2C_STS_NO_ACK) != 0)
        {
            I2C_sendStopCondition(I2CB_BASE);
            I2C_clearStatus(I2CB_BASE, I2C_STS_NO_ACK);
        }
        else if(currentMsgPtr[I2CB]->msgStatus == MSG_STATUS_SEND_NOSTOP_BUSY)
        {
            currentMsgPtr[I2CB]->msgStatus = MSG_STATUS_RESTART;
        }
    }
    else
    {
        //
        // Generate some error from invalid interrupt source
        //
        asm("   ESTOP0");
    }

    //
    // Issue ACK to enable future group 8 interrupts
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP8);
}

//
// pass - Function to be called if data written matches data read
//
void
pass(void)
{
    asm("   ESTOP0");
    for(;;);
}

//
// fail - Function to be called if data written does NOT match data read
//
void fail(void)
{
    asm("   ESTOP0");
    for(;;);
}

void i2cRWProcess(uint16_t indexI2C)
{
    uint16_t error;

    //
    // **** Write data to I2C ****
    //
    // Check the outgoing message to see if it should be sent. In this
    // example it is initialized to send with a stop bit.
    //
    if(i2cMsgOut[indexI2C].msgStatus == MSG_STATUS_SEND_WITHSTOP)
    {
        //
        // Send the data to the I2C
        //
        error = writeData(indexI2C, &i2cMsgOut[indexI2C]);

        //
        // If communication is correctly initiated, set msg status to busy
        // and update currentMsgPtr for the interrupt service routine.
        // Otherwise, do nothing and try again next loop. Once message is
        // initiated, the I2C interrupts will handle the rest. See the
        // function i2cXISR().
        //
        if(error == SUCCESS)
        {
            currentMsgPtr[indexI2C] = &i2cMsgOut[indexI2C];
            i2cMsgOut[indexI2C].msgStatus = MSG_STATUS_WRITE_BUSY;
        }
    }

    //
    // **** Read data from I2C ****
    //
    // Check outgoing message status. Bypass read section if status is
    // not inactive.
    //
    if (i2cMsgOut[indexI2C].msgStatus == MSG_STATUS_INACTIVE)
    {
        //
        // Check incoming message status
        //
        if(i2cMsgIn[indexI2C].msgStatus == MSG_STATUS_SEND_NOSTOP)
        {
            //
            // Send EEPROM address setup
            //
            while(readData(indexI2C, &i2cMsgIn[indexI2C]) != SUCCESS)
            {
                //
                // Maybe setup an attempt counter to break an infinite
                // while loop. The I2C Slave will send back a NACK while it is
                // performing a write operation. Even though the write
                // is complete at this point, the I2C Slave could still be
                // busy (programming the data). Therefore, multiple
                // attempts are necessary.
                //
            }

            //
            // Update current message pointer and message status
            //
            currentMsgPtr[indexI2C] = &i2cMsgIn[indexI2C];
            i2cMsgIn[indexI2C].msgStatus = MSG_STATUS_SEND_NOSTOP_BUSY;
        }

        //
        // Once message has progressed past setting up the internal address
        // of the I2C Slave, send a restart to read the data bytes from the
        // I2C Slave. Complete the communication with a stop bit. msgStatus is
        // updated in the interrupt service routine.
        //
        else if(i2cMsgIn[indexI2C].msgStatus == MSG_STATUS_RESTART)
        {
            //
            // Read data portion
            //
            while(readData(indexI2C, &i2cMsgIn[indexI2C]) != SUCCESS)
            {
                //
                // Maybe setup an attempt counter to break an infinite
                // while loop.
                //
            }

            //
            // Update current message pointer and message status
            //
            currentMsgPtr[indexI2C] = &i2cMsgIn[indexI2C];
            i2cMsgIn[indexI2C].msgStatus = MSG_STATUS_READ_BUSY;
        }
    }

}



/* *****************************************************************************
 * Functions Global
 **************************************************************************** */
void I2C_DRIVER_vInit(void)
{
    uint16_t i;

    //
    // Initialize GPIOs 2 and 3 for use as SDA B and SCL B respectively
    //
    GPIO_setPinConfig(GPIO_2_SDAB);
    GPIO_setPadConfig(2, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(2, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(GPIO_3_SCLB);
    GPIO_setPadConfig(3, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(3, GPIO_QUAL_ASYNC);

    // The part below must be called after Interrupt_initModule() and Interrupt_initVectorTable()

    //
    // Interrupts that are used in this example are re-mapped to ISR functions
    // found within this file.
    //
    //Interrupt_register(I2CA_BASE, &i2cAISR);
    Interrupt_register(I2CB_BASE, &i2cBISR);

    //
    // Set I2C use, initializing it for FIFO mode
    //
    //initI2C(I2CA_BASE);
    initI2C(I2CB_BASE);

    //
    // Clear incoming message buffer
    //
    for (i = 0; i < MAX_BUFFER_SIZE; i++)
    {
        //i2cMsgIn[I2CA].msgBuffer[i] = 0x0000;
        i2cMsgIn[I2CB].msgBuffer[i] = 0x0000;
    }

    //
    // Set message pointer used in interrupt to point to outgoing message
    //
    //currentMsgPtr[I2CA] = &i2cMsgOut[I2CA];
    currentMsgPtr[I2CB] = &i2cMsgOut[I2CB];

    //
    // Enable interrupts required for this example
    //
    //Interrupt_enable(INT_I2CA);
    Interrupt_enable(INT_I2CB);

    // After This part must be enabled Global Interrupts
}

void I2C_DRIVER_vProcess(void)
{
    //i2cRWProcess(I2CA);
    i2cRWProcess(I2CB);
}

void I2C_DRIVER_vSetupMessageSlave (uint16_t indexI2C, uint16_t slaveAddress, uint16_t registerAddressBytes, bool bRead)
{
    struct I2CMsg *setupMsgPtr;
    if (bRead)
    {
        setupMsgPtr = &i2cMsgIn[indexI2C];
    }
    else
    {
        setupMsgPtr = &i2cMsgOut[indexI2C];
    }
    setupMsgPtr->slaveAddr = slaveAddress;
    setupMsgPtr->registerAddrBytes = registerAddressBytes;
}

bool I2C_DRIVER_vReadData (uint16_t indexI2C, uint16_t registerStartAddress, uint16_t countBytes, uint_least8_t* pData, uint16_t* pStatus)
{
    bool bResult = false;

    struct I2CMsg *setupMsgPtr;

    setupMsgPtr = &i2cMsgIn[indexI2C];

    if (setupMsgPtr->msgStatus == MSG_STATUS_INACTIVE)
    {
        bResult = true;
        setupMsgPtr->pData = pData;
        setupMsgPtr->numBytes = countBytes;
        setupMsgPtr->pStatus = pStatus;
        if (setupMsgPtr->pStatus != NULL)
        {
            *setupMsgPtr->pStatus = MSG_STATUS_READ_BUSY;
        }

        setupMsgPtr->registerAddr[0] = (registerStartAddress >> 0) & 0x00FF;
        setupMsgPtr->registerAddr[1] = (registerStartAddress >> 8) & 0x00FF;

        setupMsgPtr->msgStatus = MSG_STATUS_SEND_NOSTOP;
    }


    return bResult;
}

bool I2C_DRIVER_vWriteData (uint16_t indexI2C, uint16_t registerStartAddress, uint16_t countBytes, uint_least8_t* pData, uint16_t* pStatus)
{
    bool bResult = false;
    uint16_t i;

    struct I2CMsg *setupMsgPtr;

    setupMsgPtr = &i2cMsgOut[indexI2C];

    if (setupMsgPtr->msgStatus == MSG_STATUS_INACTIVE)
    {
        setupMsgPtr->pData = pData;
        if ((setupMsgPtr->pData != NULL) && (countBytes <= MAX_BUFFER_SIZE))
        {
            bResult = true;
            setupMsgPtr->numBytes = countBytes;

            setupMsgPtr->pStatus = pStatus;
            if (setupMsgPtr->pStatus != NULL)
            {
                *setupMsgPtr->pStatus = MSG_STATUS_WRITE_BUSY;
            }

            setupMsgPtr->registerAddr[0] = (registerStartAddress >> 0) & 0x00FF;
            setupMsgPtr->registerAddr[1] = (registerStartAddress >> 8) & 0x00FF;

            for (i = 0; i < countBytes; i++)
            {
                setupMsgPtr->msgBuffer[i] = pData[i];
            }
            setupMsgPtr->msgStatus = MSG_STATUS_SEND_WITHSTOP;
        }
    }


    return bResult;
}
