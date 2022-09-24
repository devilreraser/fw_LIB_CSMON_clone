/* *****************************************************************************
 * File:   spidac.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 05 31 15:44
 * 
 * Description: SPI DAC
 * 
 **************************************************************************** */

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus



/* Guard condition file contents not included more than once */  
//#pragma once
#ifndef SPIDAC_H
#define	SPIDAC_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdint.h>
#include <stdbool.h>
    
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define DAC_CHANNELS_USAGE                          10
#define DAC_REVERSE_PIN_OPEN_HIGH                   1
#define TOGGLE_LD_ONLY_ON_FIRST_WRITE_COMPLETED     1
#define DAC_SPI_MODE_3_WRITE_SPI_MODE_2_READ        1

/* *****************************************************************************
 * Constants Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */
typedef enum
{
    SPIDAC_LD_DISABLE,      
    SPIDAC_LD_ENABLE,
            
    SPIDAC_LD_COUNT  
}SPIDAC_eLoadData_t;

typedef enum 
{
    SPIDAC_STA_IDLE,
    SPIDAC_STA_WRITE_DATA,
    SPIDAC_STA_WRITE_DATA_COMPLETED,
    SPIDAC_STA_SEND_DATA_WAIT,
    SPIDAC_STA_LOAD_DATA_START,
    SPIDAC_STA_LOAD_DATA_END,
            
    SPIDAC_STA_COUNT, 
            
    SPIDAC_STA_REQ_UPDATE_ALL = 0xFF
            
}SPIDAC_eState_t;


typedef enum 
{
    SPIDAC_ADDRESS_AO1 = 0b1000,
    SPIDAC_ADDRESS_AO2 = 0b0100,
    SPIDAC_ADDRESS_AO3 = 0b1100,
    SPIDAC_ADDRESS_AO4 = 0b0010,
    SPIDAC_ADDRESS_AO5 = 0b1010,
    SPIDAC_ADDRESS_AO6 = 0b0110,
    SPIDAC_ADDRESS_AO7 = 0b1110,
    SPIDAC_ADDRESS_AO8 = 0b0001,
    SPIDAC_ADDRESS_AO9 = 0b1001,
    SPIDAC_ADDRESS_AO10 = 0b0101,

}SPIDAC_eAddress_t;

typedef enum 
{
    SPIDAC_TRIP_CURR_STACK1_PHASEA  = SPIDAC_ADDRESS_AO1 ,
    SPIDAC_TRIP_CURR_STACK1_PHASEB  = SPIDAC_ADDRESS_AO2 ,
    SPIDAC_TRIP_CURR_STACK1_PHASEC  = SPIDAC_ADDRESS_AO3 ,
    SPIDAC_TRIP_CURR_STACK2_PHASEA  = SPIDAC_ADDRESS_AO4 ,
    SPIDAC_TRIP_CURR_STACK2_PHASEB  = SPIDAC_ADDRESS_AO5 ,
    SPIDAC_TRIP_CURR_STACK2_PHASEC  = SPIDAC_ADDRESS_AO6 ,
    SPIDAC_TRIP_CFBK_STACK1_CHANNEL = SPIDAC_ADDRESS_AO7 ,
    SPIDAC_TRIP_CFBK_STACK2_CHANNEL = SPIDAC_ADDRESS_AO8 ,
    SPIDAC_TRIP_VOLT_STACK1_CHANNEL = SPIDAC_ADDRESS_AO9 ,
    SPIDAC_TRIP_VOLT_STACK2_CHANNEL = SPIDAC_ADDRESS_AO10,

}SPIDAC_eSignalAddress_t;



/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 
extern uint16_t SPIDAC_u16AnalogAddress[DAC_CHANNELS_USAGE];
extern uint16_t SPIDAC_u16AnalogDataReq[DAC_CHANNELS_USAGE];
extern uint16_t SPIDAC_u16AnalogReadFbk[DAC_CHANNELS_USAGE];



/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void SPIDAC_vInit(uint32_t u32Fcy);
void SPIDAC_vProcess(void);

void SPIDAC_vSetChannel(SPIDAC_eSignalAddress_t eChannel, uint16_t u16Value);
bool SPIDAC_vGetChannel(SPIDAC_eSignalAddress_t eChannel, uint16_t* pu16Value);



#endif	/* SPIDAC_H */


#ifdef __cplusplus
}
#endif // __cplusplus
