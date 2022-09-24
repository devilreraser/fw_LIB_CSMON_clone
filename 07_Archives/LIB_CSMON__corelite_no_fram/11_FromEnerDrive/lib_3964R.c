/*!*******************************************************************************
  \file			lib_3964R.c

  \remarks		Interpretation der Protokolldaten

  \author		Frank Müller M&P GmbH
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

#include "sys_3964R.h"
#include "lib_3964R.h"
#include "lib_3964R_block.h"
#include "sys_param.h"
//#include "lib_tsm.h"	NICHT INCLUDIEREN; SONST GEHTS NICHT!//RS
#include "SerialComm.h"

/********************************************************************************
 *	local definitions																														*
 ********************************************************************************/

/********************************************************************************
 *	public data															        														*
 ********************************************************************************/

//#pragma DATA_SECTION(RecBuf3964,".extram"); 
//#pragma DATA_SECTION(TmtBuf3964,".extram"); 

BUFFER_3964 RecBuf3964;
BUFFER_3964 TmtBuf3964;

/********************************************************************************
 *	private data															    															*
 ********************************************************************************/

/********************************************************************************
 *	private function prototypes - called intern only														*
 ********************************************************************************/

/********************************************************************************
 *	public functions																														*
 ********************************************************************************/

/********************************************************************************
* FUNKTIONSNAME:	void EvaluateData3964( void )										 							*
*********************************************************************************/
/*!\param			-
*
*  \remarks		wertet den Datenblock vom 3964-Protokoll aus, interpretiert diese
*							Daten, führt Zugriffe auf das Parameterinterface aus und stellt
*							ggf. Sendedaten für das 3964-Protokoll zur Verfügung
*
*  \retval		-
********************************************************************************/
//#pragma CODE_SECTION(EvaluateData3964,"ramfuncs"); // Funktion im RAM ablegen

void EvaluateData3964( void )
{
	UINT16 index, elementNr, RecLen, Counter;
	static UINT16  Len;
	UINT8 elem;
	RC_PAR_T RetVal;
	RC_RTSP_T RetTsp;	//RS
	KDOS_3964 kdo;	

	if(RecLen = IsNewData())
	{
		// reset 3964 watchdog here 
		WatchdogCounter3964 = 0;
	   ClrNewData();									// Meldung rücksetzen 
		SetRecBufferStatus(FALSE);		// Sperren des Empfangspuffers der Dateninterpretation
		//index = RecBuf3964.header.index;
		index = *(UINT16 *)&RecBuf3964.header.index & 0x00ff | ((*((UINT16 *)&RecBuf3964.header.index + 1) & 0x00ff) << 8);
		elementNr = RecBuf3964.header.subindex;
		kdo = (KDOS_3964)RecBuf3964.header.Kdo;

		switch(kdo)
		{
		  case KDO_READ:														// ALLE PARAMETER AM ANFANG LESEN
		  /*
				if( (index == 17) && (elementNr == 8) )
				{		
					dummy++;	
				}
			*/	
					  
				if(RecLen != RecBuf3964.header.length)
				{
				  RetVal = RC_PAR_WRONG_FRAME;					// falsche Framelänge empfangen
				}
				else
				{
					Len = RecBuf3964.header.status;				// enthält bei der Leseanforderung die gewünschte Länge
					RetVal = ReadParElement(index, (PAR_EL_T)elementNr, &Len, &TmtBuf3964.Data, 0, 1);
					if(RetVal != RC_PARA_OK)
					{
					  Len = 0;														// keine Daten zurückschicken
					}
				}
				// header in jedem Fall übernehmen:
				memcpy(&TmtBuf3964.header, &RecBuf3964.header, sizeof(RecBuf3964.header));		
				TmtBuf3964.header.status = RetVal;			// returncode Parameterzugriff an Gegenstelle übermitteln
				TmtBuf3964.header.length = Len + SIZEOF_TMTBUF3964_HEADER;	// aktuelle Größe zurückschicken
				
				// Fix protocol engine
				CSerialComm_ProtocolEngineState = LOCKED;
				SetTmtRequest();												// Sendeanforderung an Protokollstatemachine übergeben
		 	break;
		  	
			case KDO_WRITE:														// Parameter schreiben
				if(RecLen != RecBuf3964.header.length)
				{
				  RetVal = RC_PAR_WRONG_FRAME;					// falsche Framelänge empfangen
				}
				else
				{
					Len = RecBuf3964.header.length - sizeof(RecBuf3964.header);					
					RetVal = WriteParElement(index, (PAR_EL_T)elementNr, &Len, &RecBuf3964.Data, 0, 0, 1);
				}
				// header Daten in jedem Fall übernehmen
				memcpy(&TmtBuf3964.header, &RecBuf3964.header, sizeof(RecBuf3964.header));
				TmtBuf3964.header.status = RetVal;			// returncode Parameterzugriff an Gegenstelle übermitteln				
				TmtBuf3964.header.length = sizeof(TmtBuf3964.header);	// aktuelle Größe zurückschicken
				// Fix protocol engine
				CSerialComm_ProtocolEngineState = LOCKED;
				SetTmtRequest();												// Sendeanforderung an Protokollstatemachine übergeben
			break;
			
			case KDO_WRITE_BACK:											// Testbetriebsart, sendet die Empfangsdaten zurück

				memcpy(&TmtBuf3964.header, &RecBuf3964.header, sizeof(RecBuf3964.header));
				memcpy(&TmtBuf3964.Data, &RecBuf3964.Data, Len);
				SetTmtRequest();												// Sendeanforderung an Protokollstatemachine übergeben
			break;
				
		  case KDO_POLLING_BATCH:														// Parameter lesen
				if(RecLen != RecBuf3964.header.length)
				{
				  RetVal = RC_PAR_WRONG_FRAME;					// falsche Framelänge empfangen
					// header in jedem Fall übernehmen					
					memcpy(&TmtBuf3964.header, &RecBuf3964.header, sizeof(RecBuf3964.header));
					TmtBuf3964.header.status = RetVal;			// returncode Parameterzugriff an Gegenstelle übermitteln
					TmtBuf3964.header.length = sizeof(TmtBuf3964.header);	// aktuelle Größe zurückschicken
				}
				else
				{
					memcpy(&TmtBuf3964, &RecBuf3964, sizeof(RecBuf3964));	// Kopiere alles
					if(PreTranslateMessage(&TmtBuf3964) == POLLING)
					{
						Counter = (UINT16)PollingBatch(&TmtBuf3964.Data, &Len);
						TmtBuf3964.header.status = Counter;			// enthält Anzahl der fehlerhaften Zugriffe auf Parameter
						TmtBuf3964.header.length = Len + SIZEOF_TMTBUF3964_HEADER;	// aktuelle Größe zurückschicken
						//TmtBuf3964.header.length = Len + sizeof(TmtBuf3964.header);	// aktuelle Größe zurückschicken
					}					
				}
				// Fix protocol engine
				CSerialComm_ProtocolEngineState = LOCKED;
				SetTmtRequest();												// Sendeanforderung an Protokollstatemachine übergeben
		 	break;
		  	
		  case KDO_READ_TRANSIENT:
				if(RecLen != RecBuf3964.header.length) // 6 != 6
				{
					RetTsp = RC_RTSP_WRONG_FRAME;					// falsche Framelänge empfangen
					
					memcpy(&TmtBuf3964.header, &RecBuf3964.header, sizeof(RecBuf3964.header));
					memcpy(&TmtBuf3964.Data, &RecBuf3964.Data, Len);
					TmtBuf3964.header.status = RetTsp;
					SetTmtRequest();												// Sendeanforderung an Protokollstatemachine übergeben
				}
				else
				{
					Len = RecBuf3964.header.length - sizeof(RecBuf3964.header);//6-6 = 0
					if(Len > 0) 	// enthält bei der Leseanforderung die Länge gesendeter Bytes
					{	
						// Übergibt Parameter für fast-Parametrierung von TSP, wenn vorhanden
						memcpy(&TmtBuf3964.Data, &RecBuf3964.Data, Len); 
					}
					// index should be 0 at first or could be interpreted as command
					// elementNr should be 0
					vStartRTSMachine();			// Start wird ausgeführt
		       }
			break;
			
			default:
				// header Daten in jedem Fall übernehmen			
				memcpy(&TmtBuf3964.header, &RecBuf3964.header, sizeof(RecBuf3964.header));			
				TmtBuf3964.header.status = RC_PAR_WRONG_CMD;	// ungültiger Kommandocode
				SetTmtRequest();												// Sendeanforderung an Protokollstatemachine übergeben
		}
		SetRecBufferStatus(TRUE);										// Sperren des Empfangspuffers der Dateninterpretation
	}

	if( (RTSState != RTS_OFF) )//&& !IsTmtRequest()					// Zustandsmaschine Read Transient weiterführen
	{
//		SetRecBufferStatus(FALSE);		// Sperren des Empfangspuffers der Dateninterpretation
		RetTsp =  ReadTransient(&index, &elem, &Len, &TmtBuf3964.Data);
//		if(RetTsp != RC_RTSP_OK) 
//		{
//			Len = 0;												// keine Daten zurückschicken
//		}
		if (RetTsp != RC_NO_TMT_REQUEST)
		{
			TmtBuf3964.header.Kdo = KDO_READ_TRANSIENT;
			TmtBuf3964.header.index = index;		//total num of Blocks
			TmtBuf3964.header.subindex = elem;	// current num of block
			TmtBuf3964.header.status = RetTsp;	// returncode Zustandsmaschine an Gegenstelle übermitteln		
			// Aktuelle Größe zurückschicken:			
			TmtBuf3964.header.length = Len + SIZEOF_TMTBUF3964_HEADER; // sizeof(TmtBuf3964.header) = SIZEOF_TMTBUF3964_HEADER = 6;
			SetTmtRequest(); // Sendeanforderung an Protokollstatemachine übergeben
		}
//		SetRecBufferStatus(TRUE);										// Sperren des Empfangspuffers der Dateninterpretation
	}

}

/********************************************************************************
* FUNKTIONSNAME:	BOOL WriteRecDataBuf(UINT16 c, UINT16 count)	 								*
*********************************************************************************/
/*!\param			c			zu schreibendes Zeichen
*  \param			count	Feldindex
*
*  \remarks		Funktion wird von Protokollbearbeitung aufgerufen,
*							Datenbereich des Protokolls wird hier zeichenweise hineingeschrieben
*
*  \retval		BOOL
********************************************************************************/
#pragma CODE_SECTION(WriteRecDataBuf,"ramfuncs"); // Funktion im RAM ablegen
BOOL WriteRecDataBuf(UINT16 c, UINT16 count)
{
	BOOL Status = TRUE;
	
	if(count > MAX_DATA_3964)
	{
	  Status = FALSE;
	}
	else
	{
		*(((UINT8*)&RecBuf3964) + count) = (UINT8)c;								
	}

	return(Status);
}

/********************************************************************************
* FUNKTIONSNAME:	BOOL ReadTmtDataBuf(UINT16* c, UINT16 count)									*
*********************************************************************************/
/*!\param			c			Zeiger auf gelesenes Zeichen
*	 \param			count	Feldindex
*
*  \remarks		Funktion wird von Protokollbearbeitung aufgerufen,
*							Datenbereich des Protokolls wird zeichenweise gefüllt mit den hier
*							gelesenen Daten
*
*  \retval		BOOL
********************************************************************************/
BOOL ReadTmtDataBuf(UINT16* c, UINT16 count)
{
	BOOL Status = TRUE;

	if(count > MAX_DATA_3964)
	{
	  Status = FALSE;
	}
	else
	{
	  *c = *(((UINT8*)&TmtBuf3964) + count); 
	}
	return(Status);
}
/********************************************************************************
* FUNKTIONSNAME:	void GetTmtDataBuf(UINT8** tmt)										 						*
*********************************************************************************/
/*!\param			tmt		Zeiger auf Transmitbuffer, als UINT8-Feld
*
*  \remarks		Funktion kann von Protokollverarbeitung aufgerufen um die Start-
*							adresse des Sendepuffers zu ermitteln, anschließend kann ein schneller
*							Transfer der kompletten Daten in den Sendepuffer 
*							des Interrupts gemacht werden (memcpy()), z.B. f. PEC-Transfer
*							also alternativ zum zeichenweisen Transfer mit ReadTmtDataBuf()
*
*  \retval		-
********************************************************************************/
void GetTmtDataBuf(UINT8** tmt)
{
	*tmt = (UINT8*)&TmtBuf3964;
}
/********************************************************************************
 *	private functions																														*
 ********************************************************************************/
