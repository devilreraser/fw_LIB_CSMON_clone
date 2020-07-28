/*!*******************************************************************************
  \file			lib_tsm.h

  \remarks	special header for lib_tsm.c

*********************************************************************************/

#ifndef 	LIB_TSM_H
#define 	LIB_TSM_H
/********************************************************************************
 *	system includes															    														*
 ********************************************************************************/

/********************************************************************************
 *	project includes																														*
 ********************************************************************************/

/********************************************************************************
 *	macros																																			*
 ********************************************************************************/
#define TSP_MAX_WORDS 4096 		// number of storable UINT32
#define TSP_CHANN_ANZAHL 8 		// maximum possible number of channels
#define BASE_TIME_REGELUNG 250 	// mys, dummy f. autom. generated initialization structure
// *************** bits in the status word ****************************** **********
#define TSP_CAPTURE 0x0001 		// set bit -> trigger condition detected
#define TSP_READY 0x0002 		// set bit -> trigger condition. captured and post trigger expired
#define TSP_WRONG_CONFIG 0x0004 // Error during configuration TSPDAT_T TSdat
#define TSP_DISABLED 0x0008 	// TSP not released
#define TSP_IS_INIT 0x0010
// *************** bits in the control word ****************************** **********
#define TSP_ENABLE 0x0001 		// set bit -> TSP is released
#define TSP_START 0x0002 		// set bit -> TSP starts

#define TSM_IS_MODE_NORMAL () (TSdat.Mode == TSM_MODE_NORMAL)
#define TSM_SWITCH_MODE_NORMAL () {TSdat.Mode = TSM_MODE_NORMAL; ParVal_TSBaseTime = SigFlowCtrl.BaseTime_us;}
#define TSM_SWITCH_MODE_RHEOLOG () (TSdat.Mode = TSM_MODE_RHEOLOG)/********************************************************************************
 *	typedefs																																		*
 ********************************************************************************/

//! possible trigger types for TSM
typedef enum
{
	SIGNED_GREATER_THAN, 	//! <0, greater than (signed) [default]
	SIGNED_LESS_THAN, 		//! <1, less than (with sign)
	UNSIGNED_GREATER_THAN, 	//! <2, greater than (unsigned)
	UNSIGNED_LESS_THAN 		//! <3, less than (unsigned)
}	TRIGGER_T;

typedef enum
{
	TSM_MODE_NORMAL, //! <0, normal operation
	TSM_MODE_RHEOLOG //! <1, use for. rheological measurements0
} TSM_MODE_T;

//!	Data for TSM
typedef struct
{                             						
	UINT16 TAbtast; 			//! <Sample regarding baseTime
	   INT16 Delay number; 		//! <Delay
	   INT32 level; 			//! <Trigger level
	   UINT16 channel number; 	//! <maximum ... channels
	   UINT32 * pMemPter; 		//! <Pointer to transspei.
	UINT16 baseTime; 			//! <Sampling time in µs
	   UINT32 * apKanalFeld [TSP_CHANN_ANZAHL]; //! <points to the value to be saved
	   UINT16 aKanalFeldTyp [TSP_CHANN_ANZAHL]; //! <contains the type of the value to be saved
	   UINT16 TUhr; 			//! <internal counter
	   UINT16 TSample; 			//! <next sampling point
	TRIGGER_T TriggerType; 		//! <Encoding trigger type
	WORD status; 				//! <Status of the transient memory
	WORD control; 				//! <Control of the TSP
	TSM_MODE_T mode; 			//! <Operating mode
}	TSPDAT_T;
 

 
/********************************************************************************
 *	prototypes																																	*
 ********************************************************************************/
void vTransMemoryTSP( void );
void vInitTSP( void );

RC_PAR_T WriteParVal_ControlTSP( WORD ControlWord );		//RS
RC_PAR_T WriteParVal_ChannelNumTSP( UINT16 ChannelNum );//RS
void Monitor_CSMON_TS_Parameters(void);

/********************************************************************************
 *	externs																																			*
 ********************************************************************************/
extern TSPDAT_T TSdat;
extern UINT32 aiTransMemory[TSP_MAX_WORDS];
extern INT16 PreDelayAnzahl;
#endif	// LIB_TSM_H
