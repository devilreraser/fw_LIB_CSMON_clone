/*!*******************************************************************************
  \file			lib_tsm.c

  \remarks	Funktionen zur Realisierung eines Transientenspeichers

*********************************************************************************/


/********************************************************************************
 *	system includes															    														*
 ********************************************************************************/

/********************************************************************************
 *	project includes																														*
 ********************************************************************************/
#include "basetype.h"
#include "hwglobal.h"
#include <F2837xD_device.h>     // DSP281x Headerfile Include File
#include "sys_param.h"
#include "partable.h"
#include "lib_tsm.h"
#include "version_info.h"

/********************************************************************************
 *	local definitions																														*
 ********************************************************************************/

/********************************************************************************
 *	public data															        														*
 ********************************************************************************/

extern UINT16 TiTimeMys;

//#pragma DATA_SECTION(TSdat,".extram"); 
//#pragma DATA_SECTION(PreDelayAnzahl,".extram"); 

TSPDAT_T TSdat;

#pragma DATA_SECTION(aiTransMemory,".tsm"); // bring this variable out of internal RAM

UINT32 aiTransMemory[TSP_MAX_WORDS];      // TSP area
INT16 PreDelayAnzahl;
/********************************************************************************
 *	private data															    															*
 ********************************************************************************/

/********************************************************************************
 *	private function prototypes - called intern only														*
 ********************************************************************************/
static BOOL bKanalInitTSP(WORD *ControlWord, UINT16 *ParVal, UINT16 FeldIndex);
static BOOL bParGetIndexTSP(UINT16 ParNr, UINT16 *TabIndex);
/********************************************************************************
 *	public functions																														*
 ********************************************************************************/

/********************************************************************************
* FUNKTIONSNAME:	void vTransMemoryTSP( void )								 									*
*********************************************************************************/
/*!\param		-
*
*  \remarks		Function for recording real-time data and later evaluation
*
*  \retval		-
********************************************************************************/
#pragma CODE_SECTION(vTransMemoryTSP,"ramfuncs"); // Store function in RAM

void vTransMemoryTSP( void )
{
	UINT32 *pMemory;	
	//register UINT16 i; 
			
	if(TSdat.Control & TSP_ENABLE)          	// if allowed
	{
		pMemory = TSdat.pMemPter; 				// Rearrangement of ring memory pointer -> Reg.
		TSdat.TUhr ++; 							// update internal timer
		
		if (TSdat.Status & TSP_READY) 			// keep current d. next sampling point
		{                              			// even when TSP is finished
			TSdat.TSample = TSdat.TUhr + 1;
			ParVal_TSStatus = TSdat.Status; 	// RS // updates the parameter for communication
			return;
		}
	
		if (TSdat.TUhr! = TSdat.TSample) 		// sampling time not yet reached
		{
			return;
		}
	    TSdat.TSample += TSdat.TAbtast;   		// Scanning point reached, -> redefine

#if 0
	  for(i = 0; i < TSdat.KanalAnzahl; i++) 	// record channels 1 - n
	  {
	  	switch(TSdat.aKanalFeldTyp[i]) 			// Differentiation of the type of the variable to be recorded
	  	{
	  		case _UINT16:	  		
	  		case _WORD:	  		
	  			*pMemory++ = (UINT16)*TSdat.apKanalFeld[i];
	  		break;
	  		
	  		case _INT16:
	  			*pMemory++ = (INT16)*TSdat.apKanalFeld[i];
	  		break;
	  		
	  		case _UINT32:
	  		case _INT32:
	  		case _DWORD:
	  			*pMemory++ = *TSdat.apKanalFeld[i];
	  		break;	  		
	  	}
	  }
#else
		// Optimization for 8 INT variables
	  	*pMemory++ = (INT16)*TSdat.apKanalFeld[0];
	  	*pMemory++ = (INT16)*TSdat.apKanalFeld[1];
	  	*pMemory++ = (INT16)*TSdat.apKanalFeld[2];
	  	*pMemory++ = (INT16)*TSdat.apKanalFeld[3];
	  	*pMemory++ = (INT16)*TSdat.apKanalFeld[4];
	  	*pMemory++ = (INT16)*TSdat.apKanalFeld[5];
	  	*pMemory++ = (INT16)*TSdat.apKanalFeld[6];
	  	*pMemory++ = (INT16)*TSdat.apKanalFeld[7];		
#endif
		
		if(PreDelayAnzahl > 0)
		{
			PreDelayAnzahl--;
			TSdat.Status &= ~TSP_CAPTURE;
		}
		else
		{
			switch(TSdat.TriggerType)							// Evaluate trigger type
			{
				// ******************************************************************
			  case UNSIGNED_LESS_THAN:
			  	switch(TSdat.aKanalFeldTyp[0]) // Differentiation of the type of the trigger variable
	  			{
			  		case _UINT32:			  			  		
			  		case _DWORD:
			  			if(*TSdat.apKanalFeld[0] <= TSdat.Pegel) 
				  		{
		      			TSdat.Status = TSP_CAPTURE; // if the level falls below: triggering 		
		      		}
			  		break;			  		
			  		
			  		case _UINT16:	  		
			  		case _WORD:			  			  			  		
			  			if(*(UINT16*)TSdat.apKanalFeld[0] <= TSdat.Pegel) 
				  		{
		      			TSdat.Status = TSP_CAPTURE; // if the level falls below: triggering 		  		
		      		}
		      	break;
		      	
		     		case _INT16:
			  		case _INT32:
			  		break;
	 			 	}
			  break;
				// ******************************************************************
			  case UNSIGNED_GREATER_THAN:
			  	switch(TSdat.aKanalFeldTyp[0]) // Differentiation of the type of the trigger variable
	  			{
			  		case _UINT32:			  			  		
			  		case _DWORD:
			  			if(*TSdat.apKanalFeld[0] >= TSdat.Pegel)
				  		{
		      			TSdat.Status = TSP_CAPTURE; // if level is exceeded: triggering		
		      		}
			  		break;			  		
			  		
			  		case _UINT16:	  		
			  		case _WORD:			  			  			  		
			  			if(*(UINT16*)TSdat.apKanalFeld[0] >= TSdat.Pegel) 
				  		{
		      			TSdat.Status = TSP_CAPTURE; // if level is exceeded: triggering 		
		      		}
		      	break;
		      	
			  		case _INT16:
			  		case _INT32:
			  		break;
			  		
	 			 	}
			 	break;
				// ******************************************************************
			  case SIGNED_LESS_THAN:
			  	switch(TSdat.aKanalFeldTyp[0]) // Differentiation of the type of the trigger variable
	  			{
			  		case _INT16:
			  			if(*(INT16*)TSdat.apKanalFeld[0] <= TSdat.Pegel) 
				  		{
		      			TSdat.Status = TSP_CAPTURE; // if the level falls below: triggering	  		
		      		}
			  		break;
			  		
			  		case _INT32:
			  			if((INT32)*TSdat.apKanalFeld[0] <= TSdat.Pegel) 
				  		{
		      			TSdat.Status = TSP_CAPTURE; // if the level falls below: triggering	
		      		}
			  		break;
			  		
			  		case _UINT32:			  			  		
			  		case _DWORD:
			  		case _UINT16:	  		
			  		case _WORD:			  			  			  		
		      	break;
	 			 	}
			  break;
				// ******************************************************************
			  case SIGNED_GREATER_THAN:
			  	switch(TSdat.aKanalFeldTyp[0]) // Differentiation of the type of the trigger variable
	  			{
			  		case _INT16:
			  			if(*(INT16*)TSdat.apKanalFeld[0] >= TSdat.Pegel) 
				  		{
		      			TSdat.Status = TSP_CAPTURE; // if level is exceeded: triggering 	 		
		      		}
			  		break;
			  		
			  		case _INT32:
			  			if((INT32)*TSdat.apKanalFeld[0] >= TSdat.Pegel) 
				  		{
		      			TSdat.Status = TSP_CAPTURE; // if level is exceeded: triggering 	
		      		}
			  		break;
			  		
			  		case _UINT32:			  			  		
			  		case _DWORD:
			  		case _UINT16:	  		
			  		case _WORD:			  			  			  		
		      	break;
	 			 	}
			 	break;
			}// ENDE switch(TSdat.TriggerType)
		}// ENDE else [if(PreDelayAnzahl > 0)]

	  if(pMemory == &aiTransMemory[TSP_MAX_WORDS])
	  {
	   	pMemory = aiTransMemory;           	// at the end of storage the pointer returns to the beginning
	  }

	  TSdat.pMemPter = pMemory;            		// pointer from register back into struct

	  if((TSdat.Status & TSP_CAPTURE) 		// wait for triggering, delay
     				&& TSdat.DelayAnzahl)		// Allows further recording and overwriting in the ring memory if READY is reset. (Delay number is set again when reinitializing)
	  {
			TSdat.DelayAnzahl -= 1;          		// expired until delay time

			if(TSdat.DelayAnzahl == 0)						// now the time has come
			{
				TSdat.Status = TSP_READY;        	// done - set flag
				ParVal_TSStatus = TSdat.Status;		// RS // updates the parameter for communication
				return;
			}											
    }
	}// ENDE TSdat.Control & TSP_ENABLE
	else 
	{
		TSdat.Status |= TSP_DISABLED;
	}
  
	ParVal_TSStatus = TSdat.Status;	// updates the parameter for communication
}



/********************************************************************************
* FUNKTIONSNAME:	void vInitTSP(void)										 *
*********************************************************************************/
/*!\param			-
*
*  \remarks 	Initializes transient storage.
* 							These settings are overwritten when communicating
* 							(with function WriteParVal_ControlTSP) initiates a new trigger process.
*
*  \retval		-
********************************************************************************/
void vInitTSP(void)
{
	UINT16 Div;
	float Val;

	TSdat.TAbtast = 1;                       
	TSdat.DelayAnzahl = 100;                          
	TSdat.Pegel = 1000;                                 
	TSdat.KanalAnzahl = 8;                    
	TSdat.pMemPter = (UINT32*)aiTransMemory; 	  				        
	TSdat.baseTime = BASE_TIME_REGELUNG;

	TSdat.apKanalFeld[0] = (UINT32*)&ParVal_i16_Test1; // "apKanalFeld" points to the value to be saved in the TSM
	TSdat.aKanalFeldTyp[0] = _INT16; // "apKanalFeldTyp" indicates the type of the value to be saved in the TSM

	TSdat.apKanalFeld[1] = (UINT32*)&ParVal_i16_Test2; 
	TSdat.aKanalFeldTyp[1] = _INT16; 
	
	TSdat.apKanalFeld[2] = (UINT32*)&ParVal_i16_Test3;
	TSdat.aKanalFeldTyp[2] = _INT16;
	
	TSdat.apKanalFeld[3] = (UINT32*)&ParVal_i16_Test4;
	TSdat.aKanalFeldTyp[3] = _INT16;
	
	TSdat.apKanalFeld[4] = (UINT32*)&ParVal_i16_Test5;	
	TSdat.aKanalFeldTyp[4] = _INT16;
	
	TSdat.apKanalFeld[5] = (UINT32*)&ParVal_i16_Test6;
	TSdat.aKanalFeldTyp[5] = _INT16;
			
	TSdat.apKanalFeld[6] = (UINT32*)&ParVal_i16_Test7;
	TSdat.aKanalFeldTyp[6] = _INT16;
		
	TSdat.apKanalFeld[7] = (UINT32*)&ParVal_i16_Test8;
	TSdat.aKanalFeldTyp[7] = _INT16;

	TSdat.TUhr = (UINT16)-1;                             
	TSdat.TSample = 0;                      
	TSdat.TriggerType = (TRIGGER_T)2;							
	TSdat.Status = 0;
	TSdat.Control |= TSP_ENABLE;

	ParVal_TSBaseTime = TSdat.baseTime /*= SigFlowCtrl.BaseTime_us*/;
	PreDelayAnzahl = (TSP_MAX_WORDS / TSdat.KanalAnzahl) - TSdat.DelayAnzahl;
	if(PreDelayAnzahl < 0) PreDelayAnzahl = 0;
	ParVal_TSControl = TSdat.Control;
	ParVal_TSTrigLevel = TSdat.Pegel;	
	ParVal_TSStatus = TSdat.Status;	
	ParVal_TSTrigCondition = (WORD)TSdat.TriggerType;
	ParVal_TSSamplStepWidthFactor = TSdat.TAbtast;

	ParVal_TSChannelNum = TSdat.KanalAnzahl;
	Div = ( ParVal_TSMemSize / TSdat.KanalAnzahl);
	Val = (Div - TSdat.DelayAnzahl) *100;
	ParVal_TSDelayNum = (UINT16)(0.5 + Val/ Div);
}


#if USE_CSMON

// Monitor change of TS Parameters and call respective handling functions
WORD ParVal_TSControl_shadow;
WORD ParVal_TSChannelNum_shadow;

void Monitor_CSMON_TS_Parameters()
{
   if (ParVal_TSControl_shadow != ParVal_TSControl)
   {
      WriteParVal_ControlTSP(ParVal_TSControl);
      ParVal_TSControl_shadow = ParVal_TSControl;
   }
   if (ParVal_TSChannelNum_shadow != ParVal_TSChannelNum)
   {
      WriteParVal_ChannelNumTSP(ParVal_TSChannelNum);
      ParVal_TSChannelNum_shadow = ParVal_TSChannelNum;
   }
}

#endif // USE_CSMON


/********************************************************************************
* FUNKTIONSNAME:		BOOL WriteParVal_ChannelNumTSP(UINT16 ChannelNum)			*
*********************************************************************************/
/*!\param			ChannelNum		-	Channel number
*
*  \remarks		Only sets the permissible values for this parameter
*
*  \retval		-
********************************************************************************/
RC_PAR_T WriteParVal_ChannelNumTSP(UINT16 ChannelNum)
{
	switch(ChannelNum) // Channel number restrictions
	{		
		case 1:	
			ParVal_TSChannelNum = 1;
		break;

		case 2:	case 3:
			ParVal_TSChannelNum = 2;
		break;

		case 4:	case 5: case 6: case 7:
			ParVal_TSChannelNum = 4;
		break;

		case 8:	
			ParVal_TSChannelNum = 8;
		break;

		default: 
			ParVal_TSChannelNum = 8;
		break;
	}

  if (ChannelNum < 1) return(RC_VALUE_LOW);
  if (ChannelNum > 8) return(RC_VALUE_HIGH);
  
  return (RC_PARA_OK);
}

/********************************************************************************
* FUNKTIONSNAME:		RC_PAR_T WriteParVal_ControlTSP(WORD ControlWord)										*
*********************************************************************************/
/*!\param			ControlWord -	control word
*
*  \remarks			describes parameters when this is no longer active
* 						as well as all other necessary parameters of TSPDAT_T TSPdat
*
*  \retval		RC_PAR_T
********************************************************************************/
RC_PAR_T WriteParVal_ControlTSP(WORD ControlWord)
{
	UINT16 Delay;

	if(ControlWord & TSP_START)			// TSP is not enabled?
	{
		TSdat.Control &= ~TSP_ENABLE;	// Definitely interrupt TSP operation
    TSdat.KanalAnzahl = (ParVal_TSChannelNum > 8 ? 1 :ParVal_TSChannelNum);	// Pass number of channels
    TSdat.KanalAnzahl = (ParVal_TSChannelNum < 1 ? 1 :ParVal_TSChannelNum);	// Pass number of channels
		TSdat.TriggerType = (TRIGGER_T)ParVal_TSTrigCondition;									// Trigger condition
		TSdat.Pegel = ParVal_TSTrigLevel;
		TSdat.TAbtast = ParVal_TSSamplStepWidthFactor;
		Delay =	(TSP_MAX_WORDS * (1.0 - (ParVal_TSDelayNum / 100.0)));
		TSdat.DelayAnzahl = Delay / TSdat.KanalAnzahl;
		PreDelayAnzahl = (TSP_MAX_WORDS / TSdat.KanalAnzahl) - TSdat.DelayAnzahl;
		TSdat.pMemPter = (UINT32*)aiTransMemory;	// Field pointer at the beginning !!
		TSdat.TUhr = (UINT16)-1;										// reset internal clock
		TSdat.TSample = 0;
		ParVal_TSStatus = TSdat.Status = 0;				// reset status

		// Alle Kanäle initialisieren
		bKanalInitTSP( &ControlWord, &ParVal_TSChannel0Par, 0); 
		bKanalInitTSP( &ControlWord, &ParVal_TSChannel1Par, 1); 
		bKanalInitTSP( &ControlWord, &ParVal_TSChannel2Par, 2); 
		bKanalInitTSP( &ControlWord, &ParVal_TSChannel3Par, 3); 
		bKanalInitTSP( &ControlWord, &ParVal_TSChannel4Par, 4); 
		bKanalInitTSP( &ControlWord, &ParVal_TSChannel5Par, 5);     
		bKanalInitTSP( &ControlWord, &ParVal_TSChannel6Par, 6); 
		bKanalInitTSP( &ControlWord, &ParVal_TSChannel7Par, 7); 

		TSdat.Control = TSP_ENABLE; // try enable TSP (TSP_START is reset)

    if (!(TSdat.Status  & TSP_WRONG_CONFIG))
    {
			TSdat.Status = TSP_IS_INIT;
    }
	}

	ParVal_TSControl = TSdat.Control;
  
  return(RC_PARA_OK);			 // must return FALSE
}

/********************************************************************************
 *	private functions																														*
 ********************************************************************************/

/********************************************************************************
* FUNKTIONSNAME:	static BOOL bKanalInitTSP(WORD *ControlWord, UINT16 *ParVal, UINT16 FeldIndex)*
*********************************************************************************/
/*!\param			Controlword		-	
*	\param			ParVal	- Pointer to parameter value
*	 \param			FeldIndex		- selected table index of the parameter table
*  \remarks		determines the pointer to the parameter value for the selected parameter
*
*  \retval		TRUE, FALSE
********************************************************************************/
static BOOL bKanalInitTSP(WORD *ControlWord, UINT16 *ParVal, UINT16 FeldIndex)
{	
	UINT16 TabIndex, Attr;

	if(bParGetIndexTSP(*ParVal, &TabIndex))	// ok
 	{
		Attr = *(ParTable[TabIndex].Attr);

    switch(Attr & PAR_TYPES) 
    {
    	case _UINT16:
    		TSdat.apKanalFeld[FeldIndex] = (UINT32*)((PAR_DES_U16_T*)ParTable[TabIndex].ParDes)->pVal;
				TSdat.aKanalFeldTyp[FeldIndex] = _UINT16;
			break;

	 		case _WORD: 
				TSdat.apKanalFeld[FeldIndex] = (UINT32*)((PAR_DES_WORD_T*)ParTable[TabIndex].ParDes)->pVal;
				TSdat.aKanalFeldTyp[FeldIndex] = _WORD;
			break;

    	case _INT16:
	   		TSdat.apKanalFeld[FeldIndex] = (UINT32*)((PAR_DES_I16_T*)ParTable[TabIndex].ParDes)->pVal;
	   		TSdat.aKanalFeldTyp[FeldIndex] = _INT16;
			break;
						
			case _UINT32: 
				TSdat.apKanalFeld[FeldIndex] = (UINT32*)((PAR_DES_U32_T*)ParTable[TabIndex].ParDes)->pVal;
				TSdat.aKanalFeldTyp[FeldIndex] = _UINT32;
			break;

			case _DWORD: 
				TSdat.apKanalFeld[FeldIndex] = (UINT32*)((PAR_DES_DWORD_T*)ParTable[TabIndex].ParDes)->pVal;
				TSdat.aKanalFeldTyp[FeldIndex] = _DWORD;
			break;
			
			case _INT32: 
				TSdat.apKanalFeld[FeldIndex] = (UINT32*)((PAR_DES_I32_T*)ParTable[TabIndex].ParDes)->pVal;
				TSdat.aKanalFeldTyp[FeldIndex] = _INT32;
			break;
			
	  	default:
	    	*ControlWord &= ~TSP_ENABLE;
	 	    TSdat.Status |= TSP_WRONG_CONFIG;		// Error!
		}
	}
  else
  {												// Error!
		bParGetIndexTSP(LEAST_PAR_INDEX, &TabIndex);		// smallest possible param is used
    Attr = *(ParTable[TabIndex].Attr);
    switch(Attr & PAR_TYPES) 
    {
    	case _UINT16:
    		TSdat.apKanalFeld[FeldIndex] = (UINT32*)((PAR_DES_U16_T*)ParTable[TabIndex].ParDes)->pVal;
				TSdat.aKanalFeldTyp[FeldIndex] = _UINT16;
			break;

	 		case _WORD: 
				TSdat.apKanalFeld[FeldIndex] = (UINT32*)((PAR_DES_WORD_T*)ParTable[TabIndex].ParDes)->pVal;
				TSdat.aKanalFeldTyp[FeldIndex] = _WORD;
			break;

    	case _INT16:
	   		TSdat.apKanalFeld[FeldIndex] = (UINT32*)((PAR_DES_I16_T*)ParTable[TabIndex].ParDes)->pVal;
	   		TSdat.aKanalFeldTyp[FeldIndex] = _INT16;
			break;
						
			case _UINT32: 
				TSdat.apKanalFeld[FeldIndex] = (UINT32*)((PAR_DES_U32_T*)ParTable[TabIndex].ParDes)->pVal;
				TSdat.aKanalFeldTyp[FeldIndex] = _UINT32;
			break;

			case _DWORD: 
				TSdat.apKanalFeld[FeldIndex] = (UINT32*)((PAR_DES_DWORD_T*)ParTable[TabIndex].ParDes)->pVal;
				TSdat.aKanalFeldTyp[FeldIndex] = _DWORD;
			break;
			
			case _INT32: 
				TSdat.apKanalFeld[FeldIndex] = (UINT32*)((PAR_DES_I32_T*)ParTable[TabIndex].ParDes)->pVal;
				TSdat.aKanalFeldTyp[FeldIndex] = _INT32;
			break;
			
	  	default: // Error!
	  		*ParVal = LEAST_PAR_INDEX;
	    	*ControlWord &= ~TSP_ENABLE;
	 	    TSdat.Status |= TSP_WRONG_CONFIG;		
	      return(FALSE);
		}// ENDE switch(Attr & PAR_TYPES)

		if (TSdat.KanalAnzahl >= (FeldIndex+1))	//Abort only if channel selected for recording
  	{													
			*ParVal = LEAST_PAR_INDEX;
			TSdat.Status |= TSP_WRONG_CONFIG;			// Error!
			return(FALSE);
		}
	}// ENDE else [if(bParGetIndexTSP( *ParVal, &TabIndex))]

	return(TRUE);
}



/********************************************************************************
* FUNKTIONSNAME:	static BOOL bParGetIndexTSP(UINT16 ParNr, UINT16* index)					*
*********************************************************************************/
/*!\param			ParNr		-	searched parameter number
*	 \param			index		- determined corresponding table index
*  \remarks						determines the table index of the
* 									Parameter table
*
*  \retval		TRUE, FALSE
********************************************************************************/

static BOOL bParGetIndexTSP(UINT16 ParNr, UINT16 *TabIndex)
{
	BOOL Found, Status = TRUE;
	UINT16 Start, End, Mid, Last;

	Found = FALSE;
	Start = 0;
	Last = End 	= /*MAX_PAR*/GetNumberOfParams();

	while(Start <= End)
	{
	  Mid = (Start + End) / 2;
	  if (Mid >= /*MAX_PAR*/Last)
	  {
	  	break;
	  }

	  if (ParNr == ParTable[Mid].index)
	  {
	    Found = TRUE;
	    *TabIndex = Mid;
	    break;
	  }

	  if (ParNr < ParTable[Mid].index)
	  {
	    End = Mid - 1;
		}
	  else
	  {
	    Start = Mid + 1;
		}
	}// ENDE while(Start <= End)

	if (!Found)
	{
		Status = FALSE;
	}

	return(Status);
}


