/* *****************************************************************************
 * File:   boardcfg.h
 * Author: Dimitar Lilov
 *
 * Created on 2019 01 23 14:49
 * 
 * Description: Board Configuration
 * 
 **************************************************************************** */

/* Guard condition file contents not included more than once */  
#ifndef BOARDCFG_H
//#define	BOARDCFG_H - defined in "boardcfg_512MU814.h" or other depending on selected MCU

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <xc.h>
#include "projectcfg.h"

#if defined __dsPIC33EP512MU814__
#include "boardcfg_512MU814.h"
#elif defined __dsPIC33EP512GM710__
#include "boardcfg_512GM710.h"
#elif defined __dsPIC33EP128GM706__
#include "boardcfg_128GM706.h"
#else
#error "Board Configuration Error - Board not Defined."
#endif

#endif	/* BOARDCFG_H */
