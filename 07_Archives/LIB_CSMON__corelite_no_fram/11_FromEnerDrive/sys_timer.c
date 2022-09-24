/*!*******************************************************************************
*  \file			sys_timer.c
*
*  \remarks		Softwaretimer
*
*  \author		Christian Irrgang M&P GmbH
*********************************************************************************/


/********************************************************************************
 *	system includes															    														*
 ********************************************************************************/
/********************************************************************************
 *	project includes																														*
 ********************************************************************************/
#include "basetype.h"
#include "hwglobal.h"
#include <F2837xD_cputimer.h>     // DSP281x Headerfile Include File
#include <F2837xD_cputimervars.h>     // DSP281x Headerfile Include File
#include <F2837xD_device.h>     // DSP281x Headerfile Include File
#include "iqmathlib.h"
#include "sys_timer.h"
#include "libarit.h"
#include <F2837xD_gpio.h>
#include "partable.h"

/********************************************************************************
 *	local definitions																														*
 ********************************************************************************/

/********************************************************************************
 *	public data															        														*
 ********************************************************************************/

/********************************************************************************
 *	private data															    															*
 ********************************************************************************/
 
static SW_TIMER_T WaitTimer;

#ifdef SW_TIMER_TEST
static SW_TIMER_T TestTimer;
static UINT32 ExpiredTime;
#endif
/********************************************************************************
 *	private function prototypes - called intern only														*
 ********************************************************************************/


/********************************************************************************
* FUNKTIONSNAME:	void Init_SWTimer_BaseTimer (void)										 					*
*********************************************************************************/
/*!\param		-
*
*	 \remarks	-	Initialisierung der verwendeten globalen Countervariable!
*							Counter wird im Regelungsinterrupt hochgezählt!
*
*  \retval	-
********************************************************************************/
void Init_SWTimer_BaseTimer( void )
{
  //ReloadCpuTimer0();		// Vorteiler und Timer0 selbst rücksetzen
  //StartCpuTimer0();			// Start
  INIT_BASE_TIMER();
  START_BASE_TIMER();
}

/********************************************************************************
* FUNKTIONSNAME:	void SetTimeout (SW_TIMER_T* pTimer, UINT32 Time_us )					*
*********************************************************************************/
/*!\param			pTimer	-	Zeiger auf Timerstruktur
*	 \param			Time_us	- Zeit in µs
*
*	 \remarks		Setzt Timeout für einen Softwaretimer und aktiviert diesen.
*							Kleinster Wert 1 µs!
*							Größter Wert 3.600.000.000[µs] = 60 min
*
*  \retval		-
********************************************************************************/
#pragma CODE_SECTION(SetTimeout,"ramfuncs"); // Funktion im RAM ablegen

void SetTimeout (SW_TIMER_T* pTimer, UINT32 Time_us)
{	
  if(Time_us > 0)
  {
    pTimer->IsActive = TRUE;
    
    if(Time_us > MAX_TIMEOUT)
    {
    	Time_us = MAX_TIMEOUT; // Begrenzung nach oben
    }

    //pTimer->StartVal = MAX_TIMEOUT - ReadCpuTimer0Counter();
    pTimer->StartVal = MAX_TIMEOUT - READ_BASE_TIMER_COUNTER();

    pTimer->EndVal = pTimer->StartVal + Time_us;

    if(pTimer->EndVal < pTimer->StartVal)
    {																		// Overflow über MAX_ULONG, -> korrigieren
      pTimer->EndVal = MAX_ULONG - MAX_TIMEOUT + pTimer->EndVal; // umklappen
    }

    if(pTimer->EndVal > MAX_TIMEOUT)
    {																		// Endwert über dem Maximalwert, aber kein OV?
      pTimer->EndVal -= MAX_TIMEOUT;    // umklappen, das was zu viel ist beginnt bei 0
    }  
  }
  else
  {
    pTimer->IsActive = FALSE;							// nicht aktiv
  }
}

/********************************************************************************
* FUNKTIONSNAME:	BOOL IsTimeout( SW_TIMER_T* pTimer )								 					*
*********************************************************************************/
/*!\param			pTimer		-	Zeiger auf timerstruktur
*
*  \remarks		Überprüfung ob Endwert für Timer erreicht
*
*  \retval		BOOL
*							TRUE 	= timeout erreicht
*							FALSE = timeout nicht erreicht
********************************************************************************/
#pragma CODE_SECTION(IsTimeout,"ramfuncs"); // Funktion im RAM ablegen
BOOL IsTimeout (SW_TIMER_T* pTimer)
{
  UINT32 TimerVal;
  
  if (pTimer->IsActive == TRUE)
  {
    //TimerVal = MAX_TIMEOUT - ReadCpuTimer0Counter();
    TimerVal = MAX_TIMEOUT - READ_BASE_TIMER_COUNTER();
    if (TimerVal > pTimer->StartVal)
    {																          // timer noch vor dem reload des Basistimers
      if (pTimer->EndVal < pTimer->StartVal)   // endwert liegt aber nach dem reload
      {
        return FALSE;													// deshalb kann er auch noch nicht fertig sein
      }
      else
      {																				// endwert liegt vor dem reload
        if (TimerVal > pTimer->EndVal)					// endwert erreicht?
        {                                     // ja
          return TRUE;
        }
      }
    }
    else
    {		
#if 0
      if ((pTimer->EndVal > pTimer->StartVal) && 
          (pTimer->EndVal + (pTimer->EndVal - pTimer->StartVal) > MAX_TIMEOUT))         // nach dem reload des Basistimers
	   {
        return TRUE;
	   }
      else 
#endif
      if ((TimerVal > pTimer->EndVal) || (pTimer->EndVal > pTimer->StartVal))					  // endwert erreicht?
      {                                       // ja
        return TRUE;
      }
    }
  }		 
  return FALSE;
}

/********************************************************************************
* FUNKTIONSNAME:	UINT32 GetElapsedTime ( SW_TIMER_T* pTimer )									*
*********************************************************************************/
/*!\param			pTimer	-	Zeiger auf verwendeten Timer
*
*  \remarks		Liest Counter-Wert seit Timerstart.
*
*  \retval		UINT32	ExpiredTime
********************************************************************************/
UINT32 GetElapsedTime ( SW_TIMER_T* pTimer )
{
	UINT32 retVal = 0L;
	UINT32 TimerVal;
	
	if(pTimer->IsActive == TRUE)
	{
    //TimerVal = MAX_TIMEOUT - ReadCpuTimer0Counter();
    TimerVal = MAX_TIMEOUT - READ_BASE_TIMER_COUNTER();

    if(TimerVal > pTimer->StartVal)
    {																				// timer befindet sich vor reload
      retVal = TimerVal - pTimer->StartVal;
    }
    else
    {                                       // timer befindet sich nach reload
      retVal = MAX_TIMEOUT - pTimer->StartVal + TimerVal;
    }  
	}

	return retVal;  	
}


/********************************************************************************
* FUNKTIONSNAME:	void WaitTillTimeout (UINT32 Time_us)													*
*********************************************************************************/
/*!\param		Time_us	-	Wartezeit in us
*
*  \remarks	wartet bis Zeit vergangen ist
*
*  \retval		-
********************************************************************************/
void WaitTillTimeout (UINT32 Time_us)
{
   SetTimeout (&WaitTimer, Time_us);
   while (!IsTimeout (&WaitTimer));
}


#ifdef SW_TIMER_TEST
/********************************************************************************
* FUNKTIONSNAME:	void TestSoftwareTimer ( void )																*
*********************************************************************************/
/*!\param			-
*
*  \remarks		verschiedene Tests
*
*  \retval		-
********************************************************************************/

void TestSoftwareTimer( void )
{
StopCpuTimer0();
CpuTimer0Regs.TIM.all = 100;
StartCpuTimer0();
	
	SetTimeout (&TestTimer, 100000L);		// 100 ms, single shot
	//SET_GPIO_B(GPIOB_OUT_TEST_P4_11);		// EIN
	
	while (!IsTimeout (&TestTimer))
	{
		ExpiredTime = GetElapsedTime(&TestTimer);	// abgelaufene Zeit lesen
	}
	
	//CLEAR_GPIO_B(GPIOB_OUT_TEST_P4_11);		//  AUS
	
	//***********************************************************************	
StopCpuTimer0();
CpuTimer0Regs.TIM.all = 5000000;
StartCpuTimer0();

	SetTimeout (&TestTimer, 40000000L);		// 40 s, single shot
	//SET_GPIO_B(GPIOB_OUT_TEST_P4_11);			//  EIN
	
	while (!IsTimeout (&TestTimer))
	{
		ExpiredTime = GetElapsedTime(&TestTimer);		// abgelaufene Zeit lesen
	}
	
	//CLEAR_GPIO_B(GPIOB_OUT_TEST_P4_11);			//  AUS
	
	
	//***********************************************************************		
StopCpuTimer0();
CpuTimer0Regs.TIM.all = 200;
StartCpuTimer0();

	SetTimeout (&TestTimer, 2000L);				// cyclic

	while (1)
	{
		ExpiredTime = GetElapsedTime(&TestTimer);		// abgelaufene Zeit lesen
		if (IsTimeout(&TestTimer))
		{
			//TOGGLE_GPIO_B(GPIOB_OUT_TEST_P4_11);	// TOGGELN
			SetTimeout (&TestTimer, 2000L);				// cyclic
		}
	}

}
#endif

/********************************************************************************
 *	private functions															*
 ********************************************************************************/
