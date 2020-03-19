/*!*******************************************************************************
  \file			sys_timer.h

  \remarks	spezieller header zu sys_timer.c

* \author		Christian Irrgang M&P GmbH
*********************************************************************************/

#ifndef SYS_TIMER_H
#define SYS_TIMER_H
/********************************************************************************
 *	system includes															    														*
 ********************************************************************************/

/********************************************************************************
 *	project includes																														*
 ********************************************************************************/
 
/********************************************************************************
 *	macros																																			*
 ********************************************************************************/
//#define SW_TIMER_TEST

// Reloadvalue CPU-Timer 
#define RELOAD_VAL_CPU_TIMER0 	((3600000000L >> HIGHSPEED_CLOCK_PRESCALER) - 1) 

#define MAX_TIMEOUT		RELOAD_VAL_CPU_TIMER0

#define INIT_BASE_TIMER()	ReloadCpuTimer1()
#define START_BASE_TIMER()	StartCpuTimer1()
#define READ_BASE_TIMER_COUNTER()	ReadCpuTimer1Counter()

/********************************************************************************
 *	typedefs																																		*
 ********************************************************************************/
//! Member für Timerfunktion
typedef struct
{
  UINT32	StartVal;				      //!< Anfangswert Softwarecounter z.Z. des Aufrufs von SetTimeout()
  UINT32	EndVal;					      //!< berechneter Endwert für Ablauf des Timers
  BOOL 	IsActive;							  //!< Timer aktiv / nicht aktiv
} SW_TIMER_T;

/********************************************************************************
 *	prototypes																																	*
 ********************************************************************************/
void 		Init_SWTimer_BaseTimer( void );
void    SetTimeout      (SW_TIMER_T* pTimer, UINT32 Time_us);
BOOL    IsTimeout       (SW_TIMER_T* pTimer);
UINT32 	GetElapsedTime  (SW_TIMER_T* pTimer);
void    WaitTillTimeout (UINT32 Time_us);

#ifdef SW_TIMER_TEST
void TestSoftwareTimer( void );
#endif

/********************************************************************************
 *	externs																																			*
 ********************************************************************************/
extern UINT32 SoftwareCounter;
extern UINT32 OverflowCounter;

#endif // SYS_TIMER_H


