/*!*******************************************************************************
*	  \file			sys_param.c
*	
*	  \remarks	Parametertabellen, Parameterdefinition, Parameterbeschreibung
*							ACHTUNG! Im Gegensatz zum µC C167 ist eine Variable vom Typ char
*							im DSP 320F281x nicht 8, sondern 16 Bit lang!
*	
*	  \author		Frank Müller, Christian Irrgang M&P GmbH
*	  
*	  \date			13.12.04
*********************************************************************************/


//********************************************************************************
//*	system includes															    														*
//********************************************************************************/
#include <stdlib.h>							// EXTREM WICHTIG FÜR malloc()!!!!!


//********************************************************************************
//*	project includes																														*
//********************************************************************************/
#include "basetype.h"
#include "hwglobal.h"
#include <F2837xD_device.h>     // DSP281x Headerfile Include File
#include "partable.h"
#include "statemach.h"
#include "sys_eep.h"
#include "DSP_spi.h"
#include "errorstack.h"
//*******************************************************************************
//*	local definitions																														*
//********************************************************************************

//********************************************************************************
//*	public data															        														*
//********************************************************************************

//#pragma DATA_SECTION(Parameter,".extram"); 
//#pragma DATA_SECTION(StartCopyParsToPartable,".extram"); 
//#pragma DATA_SECTION(pParaStoreTable,".extram"); 
//#pragma DATA_SECTION(ParaStoreCount,".extram"); 
//#pragma DATA_SECTION(ParaStoreSize,".extram"); 
//#pragma DATA_SECTION(ulParCRC,".extram"); 
//#pragma DATA_SECTION(pParaStoreControlTable,".extram"); 
#pragma DATA_SECTION(ParaStoreControlTable,".extram"); 
#pragma DATA_SECTION(ParaStoreTable,".extram"); 

#ifdef SPIE2P_TEST
#pragma DATA_SECTION(TestStringSSC0RAM,".extram"); 
	UINT8 TestStringSSC0RAM[80];
	extern UINT8 TestStringSSC0[80];
#endif
PARAMETER_T Parameter;
UINT16	StartCopyParsToPartable;

//********************************************************************************
//*	private data															    															*
//********************************************************************************
UINT8* pParaStoreTable;

PAR_STORE_ENTRY_T* pParaStoreControlTable;
UINT16 ParaStoreCount;
UINT16 ParaStoreSize;
UINT32 ulParCRC = 0;
UINT16 ParaStoreControlTable[SIZE_OF_EEP_SHADOW];
UINT16 ParaStoreTable[SIZE_OF_EEP_SHADOW];

//********************************************************************************
//*	private function prototypes - called intern only														*
//********************************************************************************
static BOOL ParGetStoreIndex(UINT16 ParNr, UINT16 *TabIndex);


/********************************************************************************
* FUNKTIONSNAME:	BOOL InitParaStorage( void )										 							*
*********************************************************************************
*!\param			-
*
*  \remarks		Legt Speicherplatz für pParaStoreControlTable und pParaStoreTable an. 
*							We are using the LARGE MEMORY MODEL, so we have to call
*							far_malloc() instead of malloc(). If far_malloc() returns a
*							Null-pointer just increase the size of the -farheap option.
*							ACHTUNG: Ab k=size=33 werden Daten in der pParaStoreTable überschrieben
*							(d.h. die letzten P. überschreiben die P. am Anfang)
*
*  \retval		BOOL Erfolgsstatus
********************************************************************************/
BOOL InitParaStorage( void )
{
	UINT16 i, k, size = 0;
	UINT8* p;
	//UINT16* p;
//	UINT32 ulong_temp;
	BOOL Status = TRUE;
	
	// Speicherplatz für alle STORE-Parameter ermitteln
	for (i = 0, k = 0; i < /*MAX_PAR*/GetNumberOfParams() ;i++ )
	{
		if(*ParTable[i].Attr & PAR_STORE)
		{
			switch (*ParTable[i].Attr & PAR_TYPES)
			{
				case _UINT16: case _INT16: case _WORD:
					size += sizeof(INT16);
					k++;
					break;
				case _UINT32: case _INT32: case _DWORD:
					size += sizeof(INT32);
					// 32-Bit-Parameter können nur auf gerade Adressen im Speicher geschrieben werden!		
					// Damit beim Ablegen in ParaStoreTable ein UINT32 auf eine
					// "gerade" Adresse geschrieben werden kann, muß size um 1 erhöht werden
					// (und somit mehr Speicherplatz besorgen):
					//size++; 
					size+=1; 
					k++;
					break;
				default:
					Status = FALSE;
					#ifdef DEBUG
					ERROR_STOP();
					#endif
			}
		}
	}

	ParaStoreCount = k;									// Anzahl von Speicherparametern
	ParaStoreSize = size;								// Gesamtgröße der Speicherparameter
	
	//if( size > (EEP_PAGE_SIZE/2) )
	//{
	//	#ifdef DEBUG
	//	ERROR_STOP(); // Ein Schreibzugriff darf die Größe eine PAGE nicht übersteigen!
	//	#endif
	//  return(FALSE);
	//}

	//pParaStoreControlTable = (PAR_STORE_ENTRY_T*)far_malloc(k*sizeof(PAR_STORE_ENTRY_T));
	pParaStoreControlTable = (PAR_STORE_ENTRY_T*)ParaStoreControlTable;
	
	if (k*sizeof(PAR_STORE_ENTRY_T) >= SIZE_OF_EEP_SHADOW)
	{
		SET_ERROR(comm, _TABLE_OVERFLOW); 
		#ifdef DEBUG
		ERROR_STOP();
		#endif
	  return(FALSE);
	}

	if(pParaStoreControlTable == NULL)
	{
		#ifdef DEBUG
		ERROR_STOP();
		#endif
	  return(FALSE);
	}

	//pParaStoreTable = (UINT8*)far_malloc(size);
	pParaStoreTable = (UINT8*)ParaStoreTable;
	
	if (size >= SIZE_OF_EEP_SHADOW)
	{
		SET_ERROR(comm, _TABLE_OVERFLOW); 
		#ifdef DEBUG
		ERROR_STOP();
		#endif
	  return(FALSE);
	}
	
	if(pParaStoreTable == NULL)
	{
		#ifdef DEBUG
		ERROR_STOP();
		#endif
	  return(FALSE);
	}
	
	p = pParaStoreTable;

	for (i = 0, k = 0; i < /*MAX_PAR*/GetNumberOfParams() ;i++ )
	{
		if(*ParTable[i].Attr & PAR_STORE)
		{
			switch (*ParTable[i].Attr & PAR_TYPES)
			{
				case _UINT16: case _INT16: case _WORD:
					size = sizeof(INT16);
				break;
					
				case _UINT32: case _INT32: case _DWORD:
					size = sizeof(INT32);
					// 32-Bit-Parameter können nur auf gerade Adressen im Speicher geschrieben werden!		
					// Damit beim Ablegen in ParaStoreTable ein UINT32 auf eine
					//"gerade" Adresse geschrieben wird: p um 1 erhöhen (ungerade geht nicht!!):
					if( (UINT32)p & 0x0001L ) // p ist ungerade!
					{
						p++; //  p erhöhen (also pParaStoreTable erhöhen)
					}
					
				break;
				
				default:
					size = 0;
					Status = FALSE;
					#ifdef DEBUG
					ERROR_STOP();
					#endif
			}
			
		  pParaStoreControlTable[k].index = ParTable[i].index;
			pParaStoreControlTable[k].pValue = p;
			p += size;
			k++;
		}
	}

	
	if(Status == FALSE) // falls Status nicht beim Funktionsaufruf abgefragt wird
	{
		SET_ERROR(comm, _EEP_INIT01); //*AD* not yet defined // Initialisierungsfehler
		#ifdef DEBUG
		ERROR_STOP();
		#endif
	}
	
	return(Status);
}



/********************************************************************************
* FUNKTIONSNAME:	BOOL EvaluatePar( void )										 							*
*********************************************************************************
*!\param			-
*
*  \remarks		Liest beim Start aus einem Zwischenspeicher die Parameter 
*							und bringt sie in Ziel Parametertabelle (wird in statemachine aufgerufen)
*
*  \retval		Erfolgsstatus
********************************************************************************/
BOOL EvaluatePar( void )
{
	UINT16 i;
	RC_PAR_T RetVal;
	//static UINT16 Attr, Len = sizeof(UINT16);
	static UINT16 Len = SIZEOF_UINT16;
	static UINT16 Attr;
	BOOL Status = TRUE;
	INT32 i32_temp = 0;
	static UINT32 u32_temp = 0;
	static UINT16 j = 0;
	static INT16 feld[4]= {0,0,0,0};
	
	if (StartCopyParsToPartable)
	{
		u32_temp = ulGenParCRC(pParaStoreTable, ParaStoreSize);			// CRC-Berechnung
		if(u32_temp != ulParCRC)			// Test	auf Gleichheit
		{
		  	SET_ERROR(comm, _EEP_DATA_CRC);	  //*AD* not yet defined
			Status = FALSE;
			#ifdef DEBUG
			ERROR_STOP();
			#endif
			StartCopyParsToPartable = 0;
			return(Status);		
		}
	
		for (i = 0;i < ParaStoreCount ;i++)
		{
			Len = SIZEOF_UINT16;
			
			//RetVal = ReadParElement(pParaStoreControlTable[i].index, EL_ATTR, &Len, &Attr, 0);
			RetVal = ReadParElement(pParaStoreControlTable[i].index, EL_ATTR, &Len, &i32_temp, 0, 0);
			// Modifikation von Attr: Die Funktion ReadParElement()
			// gibt einen void-Zeiger zurück, der das Attribut des Parameters enthält.
			// "Attr" muß hier nach dem Aufruf von ReadParElement
			// ein 16-Bit-Wert sein 0xffff. Wegen der 3964-Kommunikation wird das Attribut
			// aber als 32-Bit-Wert so zurückgegeben: 0x00ff00ff, d.h. es ist
			// ein Byte zwischen geschoben, welches hier entfernt werden muß.
			Attr = (UINT16)(i32_temp & 0xff); // das erste Byte auf Attr schreiben
			Attr |= (UINT16)((i32_temp & 0xff0000) >> 8); // das dritte Byte auf Attr schreiben
			
			if(RetVal != RC_PARA_OK)
			{
		  		SET_ERROR(comm, _READ_PARA_FAILED);	  
				#ifdef DEBUG
				ERROR_STOP();
				#endif
				StartCopyParsToPartable = 0;
			 	Status = FALSE;
			}
	
			
			switch (Attr & PAR_TYPES)
			{
				case _UINT16: case _INT16: case _WORD:
					//Len = sizeof(UINT16);
					Len = SIZEOF_UINT16;
					// Modifikation von pValue: Weil die Funktion WriteParElement() auch von
					// der 3964-Kommunikation verwendet wird, müssen 16-Bit Parameter wie folgt an
					// WriteParElement() übergeben werden: 00xx00xx (je 8 Bit Daten, gefolgt von 8 Nullen)
					feld[0] = (*(INT16*)(pParaStoreControlTable[i].pValue)) & 0x00ff;
					feld[1] = ((*(INT16*)(pParaStoreControlTable[i].pValue)) >> 8) & 0x00ff;
	
				break;
				
				case _UINT32: case _INT32: case _DWORD:
					//Len = sizeof(UINT32);
					Len = SIZEOF_UINT32;		
					// Modifikation von pValue: Weil die Funktion WriteParElement() auch von
					// der 3964-Kommunikation verwendet wird, müssen 32-Bit Parameter wie folgt an
					// WriteParElement() übergeben werden:
					// 00xx00xx00xx00xx (je 8 Bit Daten, gefolgt von 8 Nullen)
					i32_temp = *(INT32*)(pParaStoreControlTable[i].pValue);				
					for(j = 0; j<4; j++)
					{
						feld[j] = i32_temp & 0x00ff;	// die unteren 16 Bit jeweils aufs Feld schreiben
						i32_temp = i32_temp >> 8; 	// die 16 nächsten Bits [int32_temp wird dabei geschreddert]
					}
				break;
										
				default:
		  			SET_ERROR(comm, _WRONG_STATE_EVAL);	  
					#ifdef DEBUG
					ERROR_STOP();
					#endif
					StartCopyParsToPartable = 0;
					return(FALSE);
			}
			
			RetVal = WriteParElement(pParaStoreControlTable[i].index, EL_VAL, &Len, (void*)feld, 1, 0, 0);
			
			if(RetVal != RC_PARA_OK)
			{
			  Status = FALSE;
				Parameter.index = pParaStoreControlTable[i].index;
				Parameter.value = *(UINT32*)pParaStoreControlTable[i].pValue;
			}
			
			pParaStoreControlTable[i].status = PAR_EQUAL;		// Kennung für nicht geändert
		}
		optime_init();
		//event_init();
		StartCopyParsToPartable = 0;
	}
	return(Status);
}



//********************************************************************************
//* FUNKTIONSNAME:	BOOL far GetParFromNVmem( void )										 					*
//*********************************************************************************
//*!\param			-
//*
//*  \remarks		liest Parameterbereich aus NV-Memory in ein Speicherabbild
//*							liest gespeicherte CRC aus NV-Memory
//*
//*  \retval		Erfolgsstatus
//********************************************************************************
BOOL GetParFromNVmem( void )
{
	BOOL Status = TRUE;
#if PARALLEL_NV_MEM
	Status = EEPReadReq(pParaStoreTable, (UINT8*)&ParSpaceEEP, ParaStoreSize);
#else
#if SERIELL_NV_MEM
  Status = EEPReadReq(pParaStoreTable, PAR_OFFSET_EEP, ParaStoreSize);// Parameter lesen
#else
//#error "no nv-memory type defined!"
#endif
#endif
	if(Status == FALSE)
	{
	  return(FALSE);
	}
#if PARALLEL_NV_MEM
  Status = EEPReadReq((UINT8*)&ulParCRC,(UINT8*)&ulParCRCEEP,sizeof(ulParCRC));

#else
#if SERIELL_NV_MEM
  Status = EEPReadReq((UINT8*)&ulParCRC,CRC_OFFSET_EEP,sizeof(ulParCRC));// Checksumme lesen
#else
//#error "no nv-memory type defined!"
#endif
#endif
	if(Status == FALSE)
	{
	  return(FALSE);
	}
	return(Status);
}


//********************************************************************************
//	FUNKTIONSNAME:	BOOL far CopyParamToNVmem( void )										 					*
//*********************************************************************************
//*!\param			-
//*
//*  \remarks		schreibt alle speicherbaren Parameter (Speicherabbild) ins NV-memory
//*
//*  \retval		Erfolgsstatus
//********************************************************************************
BOOL CopyParamToNVmem( void )
{
	BOOL Status = TRUE;
#if PARALLEL_NV_MEM
  Status = EEPWriteReq(pParaStoreTable, (UINT8*)&ParSpaceEEP, ParaStoreSize);
#else
#if SERIELL_NV_MEM
  Status = EEPWriteReq(pParaStoreTable, PAR_OFFSET_EEP, ParaStoreSize);
#else
//#error "no nv-memory type defined!"
#endif
#endif
  if(Status == FALSE)
  {
  	return(FALSE);
  }
  
  ulParCRC = ulGenParCRC(pParaStoreTable, ParaStoreSize);		// CRC berechnen und an EPROM senden
#if PARALLEL_NV_MEM
  Status = EEPWriteReq((UINT8*)&ulParCRC,(UINT8*)&ulParCRCEEP,sizeof(ulParCRC));
#else
#if SERIELL_NV_MEM
  Status = EEPWriteReq((UINT8*)&ulParCRC,CRC_OFFSET_EEP, sizeof(ulParCRC));

#else
//#error "no nv-memory type defined!"
#endif
#endif
  if(Status == FALSE)
  {
  	return(FALSE);
  }
	return(Status);
}



/********************************************************************************
* FUNKTIONSNAME:	BOOL WriteParaStoreValue( UINT16 parnum, void* Data, UINT16 Attr)*
*********************************************************************************
*	!\param			parnum	-	Parameternummer
*  						Data		-	Zeiger auf zu schreibende Daten
*	 						Attr		-	Attribut des Parameters
*
*  \remarks		schreibt den Parameterwert in den Spiegelbereich des EEPROM
*
*  \retval		Erfolgsstatus
********************************************************************************/
BOOL WriteParaStoreValue( UINT16 parnum, void* Data, UINT16 Attr)
{
	BOOL Status = TRUE;
	static UINT16 TabIndex;

	Status = ParGetStoreIndex(parnum, &TabIndex);
  if(Status == FALSE)
  {
  	#ifdef DEBUG
		ERROR_STOP();
		#endif
  	return(FALSE);
  }

	switch (Attr & PAR_TYPES)
	{		
		case _UINT16: case _INT16: case _WORD:
			*(UINT16*)pParaStoreControlTable[TabIndex].pValue = *(UINT16*)Data;
			break;
			
		case _UINT32: case _INT32: case _DWORD:
			// ACHTUNG: 32-Bit-Parameter können NICHT auf einer ungeraden Speicherstelle
			// in ParaStoreTable abgelegt werden! (Wenn man das versucht, werden Daten
			// VOR dem eigentlichen Speicherort überschrieben!)
			// --> Also sicherstellen, daß pParaStoreControlTable[TabIndex] hier auf eine gerade
			// Speicherstelle in ParaStoreTable zeigt.
			
			*(UINT32*)pParaStoreControlTable[TabIndex].pValue = *(UINT32*)Data;
			break;
			
		default:
			#ifdef DEBUG
			ERROR_STOP();
			#endif
			return(FALSE);
	}
	
	pParaStoreControlTable[TabIndex].status = PAR_UPDATE;	// Kennung für geändert und damit Speichern
	return(Status);
}



/********************************************************************************
* FUNKTIONSNAME:	void WriteParVal_NVmemControl( WORD ControlWord)		*
*********************************************************************************
*		!\param			ControlWord		-	Steuerwort
*
*		\remarks		beschreibt Parameter wenn dieser nicht mehr aktiv ist
*
*		\retval		-
********************************************************************************/
#if 0
RC_PAR_T WriteParVal_NVmemControl( WORD ControlWord)
{
	if(ParVal_NVmemControl == PAR_NOTHING)
	{
	  ParVal_NVmemControl = ControlWord;		// nur erlaubt wenn unbeschäftigt
	}
	
	return(RC_PARA_OK);			 								// dummy, hat nichts zu sagen
}
#endif


/********************************************************************************
* FUNKTIONSNAME:	void StateMachNVmem( void )										 						*
*********************************************************************************
*		!\param		-
*
*		\remarks	Zustandsmaschine für nonvolatile memory Parametermanagement
*
*		\retval		-
********************************************************************************/
void StateMachNVmem( void )
{
	static WORD State = PAR_READY;

//	#ifdef INTERPOLATIONSSCHALTKREIS
	if(StateAdminSPI == SPI_ADMIN_STATE_EEPROM) // Administrator hat Kommunikation mit Eeprom enabled
	{
//	#endif

		switch(ParVal_NVmemControl)
		{
			case PAR_SAVE:
				switch (State)
				{														// erst wenn Steuerwort im Ruhezustand
					case PAR_READY:
						CopyParamToNVmem();			// Parameter in nv-mem schreiben
						State = PAR_BUSY;				// nächster Zustand
					break;

					case PAR_BUSY:
						if(EEPIsEmpty())				// Warteschlange leer?
						{
						  State = PAR_READY;
							ParVal_NVmemControl = PAR_NOTHING;
						}
					break;

					default:
						State = PAR_READY;
				}
				break;

			case PAR_RECALL:							// Parameter aus nv-mem laden
				switch (State)
				{														// erst wenn Steuerwort im Ruhezustand
					case PAR_READY:
						GetParFromNVmem();			// Parameter aus nv-mem holen
						State = PAR_BUSY;				// nächster Zustand
					break;

					case PAR_BUSY:
						if(EEPIsEmpty())				// Warteschlange leer?
						{
						 	State = PAR_READY;
							ParVal_NVmemControl = PAR_NOTHING;
						}
					break;

					default:
						State = PAR_READY;
				}
			break;

			case PAR_DEFAULT:
				vLoadDefaultParams();					// defaultwerte laden
				ParVal_NVmemControl = PAR_NOTHING;
			break;

			#ifdef SPIE2P_TEST
			case SPIE2P_TEST_WRITE:
				//EEPWriteReq( TestStringSSC0, TST_OFFSET_EEP, 76);
				EEPWriteReq( TestStringSSC0, TST_OFFSET_EEP, 20);
				ParVal_NVmemControl = PAR_NOTHING;
			break;

			case SPIE2P_TEST_READ:
				//EEPReadReq( TestStringSSC0RAM, TST_OFFSET_EEP, 76);
				EEPReadReq( TestStringSSC0RAM, TST_OFFSET_EEP, 20);
				ParVal_NVmemControl = PAR_NOTHING;
			break;
			#endif
		}

//	#ifdef INTERPOLATIONSSCHALTKREIS
	}
//	#endif
}



/********************************************************************************
*	private functions																														*
********************************************************************************
********************************************************************************
* FUNKTIONSNAME:	static BOOL ParGetStoreIndex(UINT16 ParNr, UINT16* TabIndex)	*
*********************************************************************************
*		!\param		ParNr		-	gesuchte Parameternummer
*	 						TabIndex		- ermittelter entsprechender Tabellenindex
*
*		\remarks	ermittelt für den gesuchten Parameter den Tabellenindex der
*							Parameterspeichertabelle
*
*		\retval		TRUE, FALSE
********************************************************************************/
static BOOL ParGetStoreIndex(UINT16 ParNr, UINT16 *TabIndex)
{
	BOOL Found, Status = TRUE;
	UINT16 Start, End, Mid;

	Found = FALSE;
	Start = 0;
	End = ParaStoreCount;
	
	while (Start <= End)
	{
	  Mid = (Start + End) / 2;
	  
	  if (Mid >= ParaStoreCount)
	  {
	  	break;
	  }
	  
	  if (ParNr == pParaStoreControlTable[Mid].index)
	  {
	    Found = TRUE;
	    *TabIndex = Mid;
	    break;
	  }
	  
	  if (ParNr < pParaStoreControlTable[Mid].index)
	  {
	    End = Mid - 1;
		}
	  else
	  {
	    Start = Mid + 1;
		}
	}
	
	if(!Found)
	{
		#ifdef DEBUG
		ERROR_STOP();
		#endif
		Status = FALSE;
	}

	return(Status);
}


