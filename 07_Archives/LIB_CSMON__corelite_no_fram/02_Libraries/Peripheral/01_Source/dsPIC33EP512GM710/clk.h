/* *****************************************************************************
 * File:   clk.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 04 04 16:12
 * 
 * Description: System Clock Initialization
 * 
 **************************************************************************** */


/* Guard condition file contents not included more than once */  
#ifndef CLK_H
#define	CLK_H


/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <stdint.h>
#include <stdbool.h>

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
void CLK_vInit(void);
uint32_t CLK_u32GetFcy (void);
uint32_t CLK_u32GetFpb (void);
bool CLK_bClockChangeRequest(void);

#endif	/* CLK_H */
