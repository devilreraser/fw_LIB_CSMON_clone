/*!*******************************************************************************
  \file			lib_param.c

  \remarks	Funktionenschnittstelle für Parameterzugriff

  \author		Frank Müller M&P GmbH
*********************************************************************************/


/********************************************************************************
 *	system includes															    														*
 ********************************************************************************/
#include <stdio.h>
#include <string.h>     // DSP281x Headerfile Include File

/********************************************************************************
 *	project includes																														*
 ********************************************************************************/
#include "basetype.h"
#include "hwglobal.h"
#include <F2837xD_device.h>     // DSP281x Headerfile Include File
#include "sys_param.h"
#include "partable.h"
#include "statemach.h"
#include "libarit.h"

/********************************************************************************
 *	local definitions																														*
 ********************************************************************************/

/********************************************************************************
 *	public data															        														*
 ********************************************************************************/

//#pragma DATA_SECTION(MaxParNum,".extram"); 
//#pragma DATA_SECTION(temp_uint16,".extram"); 
//#pragma DATA_SECTION(temp_int16,".extram"); 
//#pragma DATA_SECTION(temp_uint32,".extram"); 
//#pragma DATA_SECTION(temp_int32,".extram"); 

UINT16 MaxParNum;										// größte unterstützte Parameternummer

/********************************************************************************
 *	private data															    															*
 ********************************************************************************/

/********************************************************************************
 *	private function prototypes - called intern only														*
 ********************************************************************************/
static BOOL ParGetIndex(UINT16 ParNr, UINT16 *TabIndex);
inline void Copy16_to_charbuf(UINT8* dest, UINT16 src);
inline void Copy32_to_charbuf(UINT8* dest, UINT32 src);
inline void Copyfloat_to_charbuf(UINT8* dest, float src);
inline void Copy16_to_pointer(UINT16* dest, void *src);
inline void CopyINT32_to_pointer(INT32* dest, void* src);
inline void CopyUINT32_to_pointer(UINT32* dest, void* src);

UINT16	temp_uint16	= 0;	
INT16		temp_int16	= 0;
UINT32	temp_uint32	= 0;
INT32		temp_int32	= 0;

/********************************************************************************
 *	public functions																														*
 ********************************************************************************/

/********************************************************************************
* FUNKTIONSNAME:	RC_PAR_T ReadParElement(UINT16 index, PAR_EL_T element, UINT16* len, void* Data, UINT16 returnmode, UINT16 password)*
*********************************************************************************/
/*!\param			index		-	Parameternummer
*	 \param			element	-	Element des Parameters
*	 \param			pLen		-	Länge der Daten
*	 \param			Data		-	Datenpuffer
*	 \param			returnmode - 0: return value as character (default M&P), 1: return value as number
*	 \param			password - 0: no password evaluation, 1: password evaluation is performed
*  \remarks		liest die Daten des Parameters und schreibt sie in den übergebenen
*							Puffer
*
*  \retval		RC_PAR_T
********************************************************************************/
RC_PAR_T ReadParElement(UINT16 index, PAR_EL_T element, UINT16* pLen, void* Data, UINT16 returnmode, UINT16 password)
{
	//static UINT16 TabIndex;
	UINT16 TabIndex,TabIndex1;
//	RC_PAR_T RetVal = RC_PARA_OK;
	UINT16 Attr, StringLen;
	UINT16* pAttr;
	void* pParDes,*pParDes1;
	UINT8* pUnit;
	UINT8* pName;
	//UINT16 uint16_temp;
/*
	static UINT16 dummy;

	if( (index == 17) && (element == 8) )
	{
		dummy++;;	
	}
*/

	// Tabellenindex aus Parameternummer ermitteln
	if(!ParGetIndex(index, &TabIndex))		
	{
		#ifdef DEBUG
//		ERROR_STOP();
		#endif
	  return(RC_PARA_NOT_EXIST);					// gibts leider nicht
	}

	// Zeiger überprüfen
	if(Data == 0)													// Überprüfung Puffer
	{
		#ifdef DEBUG
		ERROR_STOP();
		#endif
	  return(RC_NULL_POINTER);						// Nullpointer
	}

	// Attribut lesen

	pAttr = ParTable[TabIndex].Attr;			// Attribut lesen
	/*
	if ((UINT16)pAttr & 1) 
	{
	   return(RC_ATTR_NOT_EXIST);		   // Test ob Pointer ungerade
	}
	*/

	if(pAttr == NULL)											// initialisiert?
	{
		#ifdef DEBUG
		ERROR_STOP();
		#endif
	  return(RC_ATTR_NOT_EXIST);					// kein Attribut
	}

	Attr = *pAttr;

	// Beschreibungsstruktur ermitteln
	pParDes = ParTable[TabIndex].ParDes;

	if(pParDes == NULL)		 								// initialisiert?
	{
		#ifdef DEBUG
		ERROR_STOP();
		#endif
	  return(RC_NULL_POINTER);						// keine Beschreibungsstruktur
	}


	// zentrale Typprüfung/Größenüberprüfung für Max, Min, Def und Val
	if(element == EL_MAX || 
		 element == EL_MIN || 
		 element == EL_DEF || 
		 element == EL_VAL)
	{
	  switch (Attr & PAR_TYPES)
	  {
			case _UINT16: case _INT16: case _WORD:		// 16-Bit Typen		
				if(*pLen != SIZEOF_UINT16)
				{
					#ifdef DEBUG
					ERROR_STOP();
					#endif
				  return(RC_WRONG_SIZE);								// Größe fehlerhaft
				}
			break;
				
			case _UINT32: case _INT32: case _DWORD:		// 32-Bit Typen			
				if(*pLen != SIZEOF_UINT32)
				{
					#ifdef DEBUG
					ERROR_STOP();
					#endif
				  return(RC_WRONG_SIZE);								// Größe fehlerhaft
				}
			break;
			
			case _A_UINT8:
			break;
				
			default:																	// nicht (noch nicht) unterstützte Typen
				#ifdef DEBUG
				ERROR_STOP();
				#endif
				return(RC_WRONG_TYPE);									// Typ fehlerhaft
	  }
	}

	// elementspezifischer Zugriff auf die Daten
	switch (element)
	{
		// Zugriff auf den Index, Element 0, mandatory
		case EL_INDEX:
				Copy16_to_charbuf((UINT8*)Data, (UINT16)index);	
					
			if(*pLen != SIZEOF_UINT16)
			{
				#ifdef DEBUG
				ERROR_STOP();
				#endif
			  return(RC_WRONG_SIZE);									// Größe fehlerhaft
			}
			break;

		// Zugriff auf das Attribut, Element 1, mandatory
		case EL_ATTR:	
				Copy16_to_charbuf((UINT8*)Data, *pAttr);		
					
			
			if(*pLen != SIZEOF_UINT16)
			{
				#ifdef DEBUG
				ERROR_STOP();
				#endif
			  return(RC_WRONG_SIZE);									// Größe fehlerhaft
			}
		break;

		// Zugriff auf den Namen, Element 2, mandatory
		case EL_NAME:
			Attr = *pAttr;
			switch (Attr & PAR_TYPES)									// typspez. Zugriff auf die Beschreibungsstruktur
			{
			  case _UINT16: case _INT16: case _WORD:
					pName = ((PAR_DES_U16_T*)pParDes)->Name;		// 16-Bit Beschreibungsstrukturen
			 	break;
			 	
			  case _UINT32: case _INT32: case _DWORD:				
					pName = ((PAR_DES_U32_T*)pParDes)->Name;		// 32-Bit Beschreibungsstrukturen
			 	break;
			 	
			  case _A_UINT8:
					pName = ((PAR_DES_A_UINT8_T*)pParDes)->Name;
				break;
				
			  default:
				return(RC_WRONG_TYPE);
			}
			
			StringLen = strlen(pName);								// Größe ermitteln
			
			if(StringLen > MAX_NAME_LEN || StringLen > *pLen)
			{
			  return(RC_WRONG_SIZE);									// Größe fehlerhaft
			}
			
			*pLen = StringLen;												// bei Strings (Name und Einheit) Größe zurück
			strncpy((UINT8*)Data, pName, StringLen);	// String schreiben
			
		break;

		// Zugriff auf die Einheit, Element 3, optional
		case EL_UNIT:
			Attr = *pAttr;
			switch (Attr & PAR_TYPES)									// typspez. Zugriff auf die Beschreibungsstruktur
			{
			  case _UINT16: case _INT16:
					pUnit = ((PAR_DES_U16_T*)pParDes)->Unit;		// 16-Bit Beschreibungsstruktur
			 	break;
			  	
			  case _UINT32: case _INT32:
					pUnit = ((PAR_DES_U32_T*)pParDes)->Unit;		// 32-Bit Beschreibungsstrukturen
			 	break;
			  	
				default:																
					pUnit = NULL;													// für die anderen Typen gibts kein Attribut
			}
			if(pUnit == NULL)
			{
			  return(RC_ELEM_NOT_EXIST);							// Element existiert nicht
			}
			StringLen = strlen(pUnit);								// Größe ermitteln
			if(StringLen > MAX_UNIT_LEN || StringLen > *pLen)
			{
			  return(RC_WRONG_SIZE);									// Größe fehlerhaft
			}
			*pLen = StringLen;												// bei Strings (Name und Einheit) Größe zurück
			strncpy((UINT8*)Data, pUnit, StringLen);	// string schreiben
		break;

		// Zugriff auf Maximalwert, Element 4, optional
		case EL_MAX:
			switch (Attr & PAR_TYPES)									// typspez. Zugriff auf die Beschreibungsstruktur
			{
			  case _UINT16: 
					Copy16_to_charbuf((UINT8*)Data ,((UINT16) (((PAR_DES_U16_T*)pParDes)->Max)));			
			  break;
			  
			  case _INT16:
					Copy16_to_charbuf((UINT8*)Data ,((INT16) (((PAR_DES_I16_T*)pParDes)->Max)));			
			  break;
			  
			  case _UINT32: 
					Copy32_to_charbuf((UINT8*)Data ,((UINT32) (((PAR_DES_U32_T*)pParDes)->Max)));			
			  break;
			  
			  case _INT32:
					Copy32_to_charbuf((UINT8*)Data ,((INT32) (((PAR_DES_I32_T*)pParDes)->Max)));			
			  break;
			  
				case _WORD:
					Copy16_to_charbuf((UINT8*)Data ,((UINT16) (((PAR_DES_WORD_T*)pParDes)->Max)));			
				break;
				
				case _DWORD:
					Copy32_to_charbuf((UINT8*)Data ,((UINT32) (((PAR_DES_DWORD_T*)pParDes)->Max)));			
				break;
				
				case _A_UINT8:
					*(UINT16*)Data = ((PAR_DES_A_UINT8_T*)pParDes)->size;
				break;
				
				default:																// alle sonstigen Typen
					return(RC_ELEM_NOT_EXIST);						// element existiert nur für die obigen Typen
			}
			break;

		// Zugriff auf Minimalwert, Element 5, optional
		case EL_MIN:
			switch (Attr & PAR_TYPES)									// typspez. Zugriff auf die Beschreibungsstruktur
			{
			  case _UINT16: 
					Copy16_to_charbuf((UINT8*)Data ,((UINT16) (((PAR_DES_U16_T*)pParDes)->Min)));			
			 	break;
			 	
			  case _INT16:
					Copy16_to_charbuf((UINT8*)Data ,((INT16) (((PAR_DES_I16_T*)pParDes)->Min)));			
			  break;
			  
			  case _UINT32: 
					Copy32_to_charbuf((UINT8*)Data ,((UINT32) (((PAR_DES_U32_T*)pParDes)->Min)));			
			  break;
			  
			  case _INT32:
					Copy32_to_charbuf((UINT8*)Data ,((INT32) (((PAR_DES_I32_T*)pParDes)->Min)));			
			  break;
			  
				case _WORD:
					Copy16_to_charbuf((UINT8*)Data ,((UINT16) (((PAR_DES_WORD_T*)pParDes)->Min)));			
				break;
				
				case _DWORD:
					Copy32_to_charbuf((UINT8*)Data ,((UINT32) (((PAR_DES_DWORD_T*)pParDes)->Min)));			
				break;
				
				default:																// alle sonstigen Typen
					return(RC_ELEM_NOT_EXIST);						// element existiert nur für die obigen Typen
			}
			break;

		// Zugriff auf Defaultwert, Element 6, mandatory
		case EL_DEF:
			switch (Attr & PAR_TYPES)									// typspez. Zugriff auf die Beschreibungsstruktur
			{
			  case _UINT16:
					Copy16_to_charbuf((UINT8*)Data ,((UINT16) (((PAR_DES_U16_T*)pParDes)->Def)));			 
			 	break;
			 	
			  case _INT16:
					Copy16_to_charbuf((UINT8*)Data ,((INT16) (((PAR_DES_I16_T*)pParDes)->Def)));			 
			 	break;
			 	
			  case _UINT32: 
					Copy32_to_charbuf((UINT8*)Data ,((UINT32) (((PAR_DES_U32_T*)pParDes)->Def)));			 
			 	break;
			 	
			  case _INT32:
					Copy32_to_charbuf((UINT8*)Data ,((INT32) (((PAR_DES_I32_T*)pParDes)->Def)));			 
			 	break;
			 	
				case _WORD:
					Copy16_to_charbuf((UINT8*)Data ,((UINT16) (((PAR_DES_WORD_T*)pParDes)->Def)));			 
				break;
				
				case _DWORD:
					Copy32_to_charbuf((UINT8*)Data ,((UINT32) (((PAR_DES_DWORD_T*)pParDes)->Def)));			 
				break;
				
				default:																// alle sonstigen Typen
					return(RC_ELEM_NOT_EXIST);						// element existiert nur für die obigen Typen
			}
			break;

		// Zugriff auf Normierung, Element 7, mandatory f. basic types
		case EL_NORM:																// Normierungsfaktor, Typ ist immer float
			switch (Attr & PAR_TYPES)									// typspez. Zugriff auf die Beschreibungsstruktur
			{
			  case _UINT16: case _INT16:							// 16-Bit typen
					if (!returnmode)
						Copyfloat_to_charbuf((UINT8*)Data, ((float) (((PAR_DES_U16_T*)pParDes)->Norm)));		
					else
						*(float *)Data = (float) (((PAR_DES_U16_T*)pParDes)->Norm);		
			  break;
			  
			  case _UINT32: case _INT32:							// 32-Bit typen
					if (!returnmode)
						Copyfloat_to_charbuf((UINT8*)Data, ((float) (((PAR_DES_U32_T*)pParDes)->Norm)));				
					else
						*(float *)Data = (float) (((PAR_DES_U32_T*)pParDes)->Norm);		
			  break;
			  
				default:																// alle sonstigen Typen
					return(RC_ELEM_NOT_EXIST);						// element existiert nur für die obigen Typen
			}
			
			if(*pLen != SIZEOF_FLOAT)
			{
			  return(RC_WRONG_SIZE);									// fehlerhafte Größe
			}
			break;

		// Zugriff auf Wert, Element 8, mandatory
		case EL_VAL:
		{
			if ((PAR_DES_U16_T*)pParDes == NULL)
				return (RC_ELEM_NOT_EXIST);
				
			if (password && (Attr & PAR_PASS_PR))
			{
				ParGetIndex(9, &TabIndex1);
			  pParDes1 = ParTable[TabIndex1].ParDes;
			  // check passcode;
				if (*((PAR_DES_U16_T*)pParDes1)->pVal != PASSCODE)
			  		return(RC_PERM_DENIED);
			}
			
			if((Attr & PAR_ACCESS) != PAR_NA && (Attr & PAR_ACCESS) != PAR_WO)
			{																					// ja Parameter ist lesbar
				switch (Attr & PAR_TYPES)								// typspez. Zugriff auf die Beschreibungsstruktur
				{
				  case _UINT16: 												// UINT16-Parameter
						temp_uint16 = (UINT16) *(((PAR_DES_U16_T*)pParDes)->pVal);// Wert des Parameters 
						if (!returnmode)
							Copy16_to_charbuf((UINT8*)Data, temp_uint16 );
						else
							*(UINT16 *)Data = temp_uint16;
						
						if(temp_uint16 > ((PAR_DES_U16_T*)pParDes)->Max)			// Check Maximalwert
						{
							ParVal_Parameter_FaultCode = index;
						  //return(RC_VALUE_HIGH);																// Wert zu groß
						}
						
						if(temp_uint16 < ((PAR_DES_U16_T*)pParDes)->Min)			// Check Minimalwert
						{
							ParVal_Parameter_FaultCode = index;
						  //return(RC_VALUE_LOW);																	// Wert zu klein
						}
					break;
					
				  case _INT16:													// INT16-Parameter	
						temp_int16 = (INT16) *(((PAR_DES_I16_T*)pParDes)->pVal);
						if (!returnmode)
							Copy16_to_charbuf((UINT8*)Data, temp_int16);
						else
							*(INT16 *)Data = temp_int16;						
						
						if(temp_int16 > ((PAR_DES_I16_T*)pParDes)->Max)			// Check Maximalwert
						{
							ParVal_Parameter_FaultCode = index;
						  //return(RC_VALUE_HIGH);																// Wert zu groß
						}
						
						if(temp_int16 < ((PAR_DES_I16_T*)pParDes)->Min)			// Check Minimalwert
						{
							ParVal_Parameter_FaultCode = index;
						  //return(RC_VALUE_LOW);																	// Wert zu klein
						}
					break;
					
				  case _UINT32: 												// UINT32-Parameter
						temp_uint32 = (UINT32) *(((PAR_DES_U32_T*)pParDes)->pVal);
						if (!returnmode)
							Copy32_to_charbuf((UINT8*)Data, temp_uint32 );
						else
							*(UINT32 *)Data = temp_uint32;						
						
						if(temp_uint32 > ((PAR_DES_U32_T*)pParDes)->Max)			// Check Maximalwert
						{
							ParVal_Parameter_FaultCode = index;
						  //return(RC_VALUE_HIGH);																// Wert zu groß
						}

						if(temp_uint32 < ((PAR_DES_U32_T*)pParDes)->Min)			// Check Minimalwert
						{
							ParVal_Parameter_FaultCode = index;
						  //return(RC_VALUE_LOW);																	// Wert zu klein
						}
					break; 
					
				  case _INT32:													// INT32-Parameter
						temp_int32 = (INT32) *(((PAR_DES_I32_T*)pParDes)->pVal);
						if (!returnmode)
							Copy32_to_charbuf((UINT8*)Data, temp_int32 );
						else
							*(INT32 *)Data = temp_int32;													
						
						if(temp_int32 > ((PAR_DES_I32_T*)pParDes)->Max)			// Check Maximalwert
						{
							ParVal_Parameter_FaultCode = index;
						  //return(RC_VALUE_HIGH);																// Wert zu groß
						}
						
						if(temp_int32 < ((PAR_DES_I32_T*)pParDes)->Min)			// Check Minimalwert
						{
							ParVal_Parameter_FaultCode = index;
						  //return(RC_VALUE_LOW);																	// Wert zu klein
						}
					break;
					
					case _WORD:														// WORD-Parameter		
						temp_uint16 = (UINT16) *(((PAR_DES_WORD_T*)pParDes)->pVal);
						if (!returnmode)
							Copy16_to_charbuf((UINT8*)Data, temp_uint16);
						else
							*(UINT16 *)Data = temp_uint16;																			
						
						if(temp_uint16 > ((PAR_DES_WORD_T*)pParDes)->Max)			// Check Maximalwert
						{
							ParVal_Parameter_FaultCode = index;
						  //return(RC_VALUE_HIGH);																// Wert zu groß
						}
						
						if(temp_uint16 < ((PAR_DES_WORD_T*)pParDes)->Min)			// Check Minimalwert
						{
							ParVal_Parameter_FaultCode = index;
						  //return(RC_VALUE_LOW);																	// Wert zu klein
						}
					break;
					
					case _DWORD:													// DWORD-Parameter	
						temp_uint32 = (UINT32) *(((PAR_DES_DWORD_T*)pParDes)->pVal);
						if (!returnmode)
							Copy32_to_charbuf((UINT8*)Data, temp_uint32 );
						else
							*(UINT32 *)Data = temp_uint32;						

						if(temp_uint32 > ((PAR_DES_DWORD_T*)pParDes)->Max)		// Check Maximalwert
						{
							ParVal_Parameter_FaultCode = index;
						  //return(RC_VALUE_HIGH);																// Wert zu groß
						}
						
						if(temp_uint32 < ((PAR_DES_DWORD_T*)pParDes)->Min)		// Check Minimalwert
						{
							ParVal_Parameter_FaultCode = index;
						  //return(RC_VALUE_LOW);																	// Wert zu klein
						}
					break;
					
					case _A_UINT8:                      // ARRAY-UINT8-Parameter
					// Größe ermitteln (ohne '\0'): Textparameter müssen aber mit End of string abgeschlossen sein!
						StringLen = MIN(strlen((UINT8*)((PAR_DES_A_UINT8_T*)pParDes)->pVal), ((PAR_DES_A_UINT8_T*)pParDes)->size);
						*pLen = StringLen;                // bei Strings Größe zurück
						strncpy(Data, (UINT8*)((PAR_DES_A_UINT8_T*)pParDes)->pVal, StringLen); // String ohne '\0' schreiben
						break;
					
					default:														// alle sonstigen Typen
																							// momentan auch für arrays
																							// werden also noch nicht unterstützt
						return(RC_WRONG_TYPE);
				}																			// switch(Attr & PAR_TYPES)
			}
			else																		// if(Attr & PAR_ACCESS)
			{
			  return(RC_WRITE_ONLY);								// kein Lesezugriff möglich
			}																				// else if (Attr & PAR_ACCESS)
			break;																	// case EL_VAL:
		}																					// case EL_VAL:
		default:
			return(RC_ELEM_NOT_EXIST);
	}																						// switch element

	return(RC_PARA_OK);													// alles o.k.
}


/********************************************************************************
* FUNKTIONSNAME:	RC_PAR_T WriteParElement(UINT16 index, PAR_EL_T element, UINT16* len, void* Data, UINT16 nostore, UINT16 datamode, UINT16 password)*
*********************************************************************************/
/*!\param			index		-	Parameternummer
*	 \param			element	-	Element des Parameters
*	 \param			pLen		-	Länge der Daten
*	 \param			Data		-	Datenpuffer
*	 \param			nostore  -  = 1 forces data not to be stored in EEPROM		
*	 \param			datamode -  0: resort by character (default M&P), 1: write directly
*	\param			password -  0: no password evaluation, 1: password is evaluated
*  \remarks		liest die Daten aus dem Puffer und schreibt sie in den Parameter
*
*  \retval		RC_PAR_T
********************************************************************************/
RC_PAR_T WriteParElement(UINT16 index, PAR_EL_T element, UINT16* pLen, void* Data, UINT16 nostore, UINT16 datamode, UINT16 password)
{
	static UINT16 TabIndex,TabIndex1;
	RC_PAR_T RetVal = RC_PARA_OK;
	UINT16 Attr;
	UINT16* pAttr;
	void* pParDes,*pParDes1;
	static INT32 Temp = 0;
	
	// Tabellenindex aus Parameternummer ermitteln
	if(!ParGetIndex(index, &TabIndex))		
	{
  	#ifdef DEBUG
		ERROR_STOP();
		#endif
	  return(RC_PARA_NOT_EXIST);					// gibts leider nicht
	}

	// Zeiger überprüfen
	if(Data == 0)													// Überprüfung Puffer
	{
  	#ifdef DEBUG
		ERROR_STOP();
		#endif
	  return(RC_NULL_POINTER);						// Nullpointer
	}

	// Attribut lesen
	pAttr = ParTable[TabIndex].Attr;			// Attribut lesen

	if(pAttr == NULL)											// initialisiert?
	{
  	#ifdef DEBUG
		ERROR_STOP();
		#endif
	  return(RC_ATTR_NOT_EXIST);					// kein Attribut
	}

	Attr = *pAttr;

	// Beschreibungsstruktur ermitteln
	pParDes = ParTable[TabIndex].ParDes;

	if(pParDes == NULL)		 								// initialisiert?
	{
  	#ifdef DEBUG
		ERROR_STOP();
		#endif
	  return(RC_NULL_POINTER);						// keine Beschreibungsstruktur
	}


	// Schreibzugriff nur auf das Element Wert (EL_VAL)
	if(element != EL_VAL)
	{
		return(RC_READ_ONLY);								// Typ fehlerhaft
	}

	if((Attr & PAR_STATE) && IsSwitchedOn())
	{
	  return(RC_PAR_WRONG_STATE_WRITE);
	}

	if (password && (Attr & (PAR_PASS_PR | PAR_PASS_PW)))
	{
		ParGetIndex(9, &TabIndex1);
	  	pParDes1 = ParTable[TabIndex1].ParDes;
	  	// check passcode;
		if (*((PAR_DES_U16_T*)pParDes1)->pVal != PASSCODE)
	  		return(RC_PERM_DENIED);
	}

	if((Attr & PAR_ACCESS) == PAR_WO || (Attr & PAR_ACCESS) == PAR_RW || (nostore == 1))
	// RO parameters are written at param init (reload from eeprom)
	{																					// ja Parameter ist lesbar
		switch (Attr & PAR_TYPES)								// typspez. Zugriff auf die Beschreibungsstruktur
		{
		  case _UINT16: 												// UINT16-Parameter
		  		if (!datamode)
					Copy16_to_pointer( (UINT16*)&Temp, Data);
				else
					Temp = *(UINT16 *)Data;
				
				if(*pLen != SIZEOF_UINT16)									// Größencheck
				{
			  	#ifdef DEBUG
					ERROR_STOP();
					#endif
				  return(RC_WRONG_SIZE);										// falsche Größe
				}
				
				if((UINT16)Temp > ((PAR_DES_U16_T*)pParDes)->Max)			// Check Maximalwert
				{
			  	#ifdef DEBUG
					ERROR_STOP();
					#endif
				  return(RC_VALUE_HIGH);																// Wert zu groß
				}
																												
				if( (UINT16)Temp < ((PAR_DES_U16_T*)pParDes)->Min)			// Check Minimalwert
				{
					#ifdef DEBUG
					ERROR_STOP();
					#endif
				  return(RC_VALUE_LOW);																	// Wert zu klein
				}
				
				
				if(((((PAR_DES_U16_T*)pParDes)->pVal)) != NULL)
				{
					if((Attr & PAR_WRITE) == PAR_WR_DIR || ((PAR_DES_U16_T*)pParDes)->pfFct == NULL)
					{																											// Fkt. existiert nicht o.
						*(((PAR_DES_U16_T*)pParDes)->pVal) = (UINT16)Temp;// Wert direkt schreiben
					}
					else
					{
						RetVal = ((PAR_DES_U16_T*)pParDes)->pfFct((UINT16)Temp);// spezielle Funktion verwenden
						if(RetVal != RC_PARA_OK)
						{
						  return(RetVal);
						}
					}
				}
				else
				{
					#ifdef DEBUG
					ERROR_STOP();
					#endif
				  return(RC_NULL_POINTER);															// Zeigerziel existiert nicht
				}
				
			break;
				
		  case _INT16:													// INT16-Parameter		  
		  		if (!datamode)
					Copy16_to_pointer( (UINT16*)&Temp, Data);	
				else
					Temp = *(UINT16 *)Data;
	
				if(*pLen != SIZEOF_INT16)															// Größencheck
				{
					#ifdef DEBUG
					ERROR_STOP();
					#endif
				  return(RC_WRONG_SIZE);																// falsche Größe
				}
				
				if((INT16)Temp > ((PAR_DES_I16_T*)pParDes)->Max)			// Check Maximalwert
				{
					#ifdef DEBUG
					ERROR_STOP();
					#endif
				  return(RC_VALUE_HIGH);																// Wert zu groß
				}
				
				if((INT16)Temp < ((PAR_DES_I16_T*)pParDes)->Min)			// Check Minimalwert
				{
					#ifdef DEBUG
					ERROR_STOP();
					#endif
				  return(RC_VALUE_LOW);																	// Wert zu klein
				}
				if(((((PAR_DES_I16_T*)pParDes)->pVal)) != NULL)
				{
					if((Attr & PAR_WRITE) == PAR_WR_DIR || ((PAR_DES_I16_T*)pParDes)->pfFct == NULL)
					{
						*(((PAR_DES_I16_T*)pParDes)->pVal) = (INT16)Temp;	// Wert direkt schreiben
					}
					else
					{
						RetVal = ((PAR_DES_I16_T*)pParDes)->pfFct((INT16)Temp);		// spezielle Funktion verwenden
						if(RetVal != RC_PARA_OK)
						{
						  return(RetVal);
						}
					}
				}
				else
				{
				  return(RC_NULL_POINTER);															// Zeigerziel existiert nicht
				}

			break;
				
		  case _UINT32: 																						// UINT32-Parameter
		  		if (!datamode)
					CopyUINT32_to_pointer( (UINT32*)&Temp, Data);
				else
					Temp = *(UINT32 *)Data;
											
				if(*pLen != SIZEOF_UINT32)															// Größencheck
				{
					#ifdef DEBUG
					ERROR_STOP();
					#endif
				  return(RC_WRONG_SIZE);																// falsche Größe
				}
				
				if((UINT32)Temp > ((PAR_DES_U32_T*)pParDes)->Max)			// Check Maximalwert
				{
					#ifdef DEBUG
					ERROR_STOP();
					#endif
				  return(RC_VALUE_HIGH);																// Wert zu groß
				}
				
				if((UINT32)Temp < ((PAR_DES_U32_T*)pParDes)->Min)			// Check Minimalwert
				{
					#ifdef DEBUG
					ERROR_STOP();
					#endif
				  return(RC_VALUE_LOW);																	// Wert zu klein
				}
				
				if(((((PAR_DES_U32_T*)pParDes)->pVal)) != NULL)
				{
					if((Attr & PAR_WRITE) == PAR_WR_DIR || ((PAR_DES_U32_T*)pParDes)->pfFct == NULL)
					{
						*(((PAR_DES_U32_T*)pParDes)->pVal) = (UINT32)Temp;// Wert direkt schreiben
					}
					else
					{
					  RetVal = ((PAR_DES_U32_T*)pParDes)->pfFct((UINT32)Temp);		// spezielle Funktion verwenden
						if(RetVal != RC_PARA_OK)
						{
						  return(RetVal);
						}
					}
				}
				else
				{
				  return(RC_NULL_POINTER);															// Zeigerziel existiert nicht
				}
				
			break;				

		  case _INT32:													// INT32-Parameter
		  		if (!datamode)
					CopyINT32_to_pointer( (INT32*)&Temp, Data);							
				else
					Temp = *(UINT32 *)Data;

				if(*pLen != SIZEOF_INT32)								 							// Größencheck
				{
					#ifdef DEBUG
					ERROR_STOP();
					#endif
				  return(RC_WRONG_SIZE);																// falsche Größe
				}

				if((INT32)Temp > ((PAR_DES_I32_T*)pParDes)->Max)			// Check Maximalwert
				{
					#ifdef DEBUG
					ERROR_STOP();
					#endif
				  return(RC_VALUE_HIGH);																// Wert zu groß
				}
				
				if((INT32)Temp < ((PAR_DES_I32_T*)pParDes)->Min)					// Check Minimalwert
				{
					#ifdef DEBUG
					ERROR_STOP();
					#endif
				  return(RC_VALUE_LOW);																	// Wert zu klein
				}
				if(((((PAR_DES_I32_T*)pParDes)->pVal)) != NULL)
				{
					if((Attr & PAR_WRITE) == PAR_WR_DIR || ((PAR_DES_I32_T*)pParDes)->pfFct == NULL)
					{
						*(((PAR_DES_I32_T*)pParDes)->pVal) = (INT32)Temp;	// Wert direkt schreiben
					}
					else
					{
					  RetVal = ((PAR_DES_I32_T*)pParDes)->pfFct((INT32)Temp);		// spezielle Funktion verwenden
						if(RetVal != RC_PARA_OK)
						{
						  return(RetVal);
						}
					}
				}
				else
				{
					#ifdef DEBUG
					ERROR_STOP();
					#endif
				  return(RC_NULL_POINTER);															// Zeigerziel existiert nicht
				}

			break;
			
			case _WORD:														// WORD-Parameter					
		  		if (!datamode)
					Copy16_to_pointer( (UINT16*)&Temp, Data);	
				else
					Temp = *(UINT16 *)Data;
										
				if(*pLen != SIZEOF_WORD)																// Größencheck
				{
				  return(RC_WRONG_SIZE);																// falsche Größe
				}
				
				if((UINT16)Temp > ((PAR_DES_WORD_T*)pParDes)->Max)			// Check Maximalwert
				{
					#ifdef DEBUG
					ERROR_STOP();
					#endif
				  return(RC_VALUE_HIGH);																// Wert zu groß
				}
				
				if((UINT16)Temp < ((PAR_DES_WORD_T*)pParDes)->Min)			// Check Minimalwert
				{
					#ifdef DEBUG
					ERROR_STOP();
					#endif
				  return(RC_VALUE_LOW);																	// Wert zu klein
				}
				if(((((PAR_DES_WORD_T*)pParDes)->pVal)) != NULL)
				{
					if((Attr & PAR_WRITE) == PAR_WR_DIR || ((PAR_DES_WORD_T*)pParDes)->pfFct == NULL)
					{
						*(((PAR_DES_WORD_T*)pParDes)->pVal) = (UINT16)Temp;	// Wert direkt schreiben
					}
					else
					{
					  RetVal = ((PAR_DES_WORD_T*)pParDes)->pfFct((UINT16)Temp);		// spezielle Funktion verwenden
						if(RetVal != RC_PARA_OK)
						{
							#ifdef DEBUG
							ERROR_STOP();
							#endif
						  return(RetVal);
						}
					}
				}
				else
				{
				  return(RC_NULL_POINTER);															// Zeigerziel existiert nicht
				}
				
			break;
			
			case _DWORD:													// DWORD-Parameter	
		  		if (!datamode)
					CopyUINT32_to_pointer( (UINT32*)&Temp, Data);							
				else
					Temp = *(UINT32 *)Data;
				
				if(*pLen != SIZEOF_DWORD) 															// Größencheck
				{
					#ifdef DEBUG
					ERROR_STOP();
					#endif
				  return(RC_WRONG_SIZE);																// falsche Größe
				}
				
				if((UINT32)Temp > ((PAR_DES_DWORD_T*)pParDes)->Max)		// Check Maximalwert
				{
					#ifdef DEBUG
					ERROR_STOP();
					#endif
				  return(RC_VALUE_HIGH);																// Wert zu groß
				}
				if((UINT32)Temp < ((PAR_DES_DWORD_T*)pParDes)->Min) 		// Check Minimalwert
				{
					#ifdef DEBUG
					ERROR_STOP();
					#endif
				  return(RC_VALUE_LOW);																	// Wert zu klein
				}
				if(((((PAR_DES_DWORD_T*)pParDes)->pVal)) != NULL)
				{
					if((Attr & PAR_WRITE) == PAR_WR_DIR || ((PAR_DES_DWORD_T*)pParDes)->pfFct == NULL)
					{
						*(((PAR_DES_DWORD_T*)pParDes)->pVal) = (UINT32)Temp;	// Wert direkt schreiben
					}
					else
					{
					  RetVal = ((PAR_DES_DWORD_T*)pParDes)->pfFct((UINT32)Temp);	// spezielle Funktion verwenden
						if(RetVal != RC_PARA_OK)
						{
						  return(RetVal);
						}
					}
				}
				else
				{
					#ifdef DEBUG
					ERROR_STOP();
					#endif
				  return(RC_NULL_POINTER);															// Zeigerziel existiert nicht
				}
				
			break;

			case _A_UINT8:                // ARRAY-UINT8-Parameter
				if(*pLen > ((PAR_DES_A_UINT8_T*)pParDes)->size) // Größencheck
				{
					return(RC_VALUE_HIGH);    // falsche Größe (string zu lang)
				}
				if(((((PAR_DES_A_UINT8_T*)pParDes)->pVal)) != NULL)
				{
					((UINT8*)Data)[*pLen-1] = '\0'; // String Abschluss '\0' erzwingen
					//*((UINT8*)Data+*pLen) = '\0'; geht auch
					if((Attr & PAR_WRITE) == PAR_WR_DIR || ((PAR_DES_A_UINT8_T*)pParDes)->pfFct == NULL)
					{
						strncpy((UINT8*)((PAR_DES_A_UINT8_T*)pParDes)->pVal, Data, *pLen); // Wert direkt schreiben
					}
					else
					{
						RetVal = ((PAR_DES_A_UINT8_T*)pParDes)->pfFct(Data, *pLen); // spez. Funktion verwenden
						if(RetVal != RC_PARA_OK)
						{
							return(RetVal);
						}
					}
				}
				else
				{
					return(RC_NULL_POINTER);  // Zeigerziel existiert nicht
				}
				break;
			
			default:														// alle sonstigen Typen
																					// momentan auch für arrays
																				// werden also noch nicht unterstützt
				#ifdef DEBUG
				ERROR_STOP();
				#endif
				return(RC_WRONG_TYPE);
		}
	}
	else
	{
	  return(RC_READ_ONLY);								// schreibgeschützt
	}

	if(Attr & PAR_STORE)										// Speicherparameter?
	{																				// ja, -> in RAM-Spiegel f. EEPROM schreiben
		WriteParaStoreValue( index , (void*)&Temp, Attr); 
		if ((nostore == 0) && (ParVal_NVmemControl1 == PAR_NOTHING) && (ParVal_NVmemControl == PAR_NOTHING))
			ParVal_NVmemControl1 = PAR_SAVE_ALL;				// Par. zum Kopieren in EEPROM vormerken
	}

	return(RC_PARA_OK);											// alles o.k.
}

/********************************************************************************
* FUNKTIONSNAME:	void vLoadDefaultParams( void )										 						*
*********************************************************************************/
/*!\param			-
*
*  \remarks		lädt die Defaultwerte aller Parameter
*
*  \retval		-
********************************************************************************/
void vLoadDefaultParams( void )
{
	UINT16 i, *pAttr;
	static UINT16 TabIndex;
	void* pParDes;
	void* pData;

	//MaxParNum = ParTable[MAX_PAR-1].index;		/* *AD* MAX_PAR */
	UINT16 MaxTabIndex = GetNumberOfParams() - 1;		/* *AD* MAX_PAR */
	
#ifdef STRING_PARAMS
	//Geraeteidentifikation aus "version_info.h" holen und auf Parameter schreiben
	strncpy(ParVal_DeviceType, DEVICE_TYPE, (unsigned long)MAX_NAME_LEN);
	ParVal_DeviceType[MAX_NAME_LEN - 1] = '\0'; // letztes Zeichen 0x00
	
	strncpy(ParVal_HWVersion, HW_VERSION, (unsigned long)MAX_NAME_LEN);
	ParVal_HWVersion[MAX_NAME_LEN - 1] = '\0'; // letztes Zeichen 0x00

	strncpy(ParVal_SWVersion, SW_VERSION, (unsigned long)MAX_NAME_LEN);
	ParVal_SWVersion[MAX_NAME_LEN - 1] = '\0'; // letztes Zeichen 0x00

	strncpy(ParVal_SWBuildNum, SW_BUILD_NUM, (unsigned long)MAX_NAME_LEN);
	ParVal_SWBuildNum[MAX_NAME_LEN - 1] = '\0'; // letztes Zeichen 0x00
	
	strncpy(ParVal_SWBuildDate, SW_BUILD_DATE, (unsigned long)MAX_NAME_LEN);
	ParVal_SWBuildDate[MAX_NAME_LEN - 1] = '\0'; // letztes Zeichen 0x00
	
	strncpy(ParVal_SWToolVersion, SW_TOOL_VERSION, (unsigned long)MAX_NAME_LEN);
	ParVal_SWToolVersion[MAX_NAME_LEN - 1] = '\0'; // letztes Zeichen 0x00
#endif

	for (i = 0;i <= /*MaxParNum*/MaxTabIndex ;i++ )		/* *AD* MAX_PAR */
	{
		TabIndex = i;		/* *AD* MAX_PAR */
		/*if(!ParGetIndex(i, &TabIndex))		 *AD* MAX_PAR 
		{
		  continue;
		}*/
		pAttr = ParTable[TabIndex].Attr;
		if(pAttr == NULL)
		{
		  continue;														// kein Attribut, -> keine Initialisierung
		}

		// Beschreibungsstruktur ermitteln
		pParDes = ParTable[TabIndex].ParDes;

		if(pParDes == NULL)		 								// initialisiert?
		{
	  	continue;														// keine Beschreibungsstruktur -> keine Initialisierung möglich
		}

		switch (*pAttr & PAR_TYPES)									// typspez. Zugriff auf die Beschreibungsstruktur
		{
		  case _UINT16: 
				*(((PAR_DES_U16_T*)pParDes)->pVal) = ((PAR_DES_U16_T*)pParDes)->Def;
				pData = ((PAR_DES_U16_T*)pParDes)->pVal;
		 	break;
		 	
		  case _INT16:
				*(((PAR_DES_I16_T*)pParDes)->pVal) = ((PAR_DES_I16_T*)pParDes)->Def;
				pData = ((PAR_DES_I16_T*)pParDes)->pVal;
		 	break;
		  	
		  case _UINT32: 
				*(((PAR_DES_U32_T*)pParDes)->pVal) = ((PAR_DES_U32_T*)pParDes)->Def;
				pData = ((PAR_DES_U32_T*)pParDes)->pVal;
		 	break;
		  	
		  case _INT32:
				*(((PAR_DES_I32_T*)pParDes)->pVal) = ((PAR_DES_I32_T*)pParDes)->Def;
				pData = ((PAR_DES_I32_T*)pParDes)->pVal;
		 	break;
		  	
			case _WORD:
				*(((PAR_DES_WORD_T*)pParDes)->pVal) = ((PAR_DES_WORD_T*)pParDes)->Def;
				pData = ((PAR_DES_WORD_T*)pParDes)->pVal;
			break;
				
			case _DWORD:
				*(((PAR_DES_DWORD_T*)pParDes)->pVal) = ((PAR_DES_DWORD_T*)pParDes)->Def;
				pData = ((PAR_DES_DWORD_T*)pParDes)->pVal;
			break;
			
			default:
				continue;	// Typen ohne Defaultwert
		}

		// load actual number of parameters in par. 0
		if (TabIndex == 0)
		{
			*(((PAR_DES_U16_T*)pParDes)->pVal) = MaxTabIndex + 1;		/* *AD* MAX_PAR */
		}

		if(*pAttr & PAR_STORE)		 							// Speicherparameter?
		{																				// ja, -> in RAM-Spiegel f. EEPROM schreiben
			WriteParaStoreValue( ParTable[TabIndex].index , pData, *pAttr); //vLoadDefaultParams
		}

	}

}

/********************************************************************************
* FUNKTIONSNAME:	RC_PAR_T ReadParAddress (UINT16 index, void* pAdr, UINT16* pWriteType*
*********************************************************************************/
/*!\param			index				-	Parameternummer
*  \param			pAdr				-	Zeiger auf einen Speicherplatz für einen void Zeiger
*  \param			pWriteType	- Zeiger auf WriteTyp
*  \remarks		ermittelt einen Zeiger auf den Parameter oder die Behandlungsfunktion
*
*  \retval		RC_PAR_T
********************************************************************************/
RC_PAR_T ReadParAddress (UINT16 index, void** pAdr, UINT16* pWriteType)
{
	static UINT16 TabIndex;
//	RC_PAR_T RetVal = RC_PARA_OK;
	UINT16 Attr;
	//const UINT16* pAttr = NULL;
	UINT16* pAttr = NULL;
	void* pParDes;


	// Tabellenindex aus Parameternummer ermitteln
	if(!ParGetIndex(index, &TabIndex))		
	{
	  return(RC_PARA_NOT_EXIST);					// gibts leider nicht
	}

	// Zeiger überprüfen
	if(pAdr == NULL) 											// Überprüfung Puffer
	{
	  return(RC_NULL_POINTER);						// Nullpointer
	}

	if(pWriteType == NULL) 								// Überprüfung Puffer
	{
	  return(RC_NULL_POINTER);						// Nullpointer
	}

	// Attribut lesen
	pAttr = ParTable[TabIndex].Attr;			// Attribut lesen
	
	/*
	if ((UINT16)pAttr & 1) 
	{
	   return(RC_ATTR_NOT_EXIST);		   // Test ob Pointer ungerade
	}
	*/

	if(pAttr == NULL)											// initialisiert?
	{
	  return(RC_ATTR_NOT_EXIST);					// kein Attribut
	}

	Attr = *pAttr;

	// Beschreibungsstruktur ermitteln
	pParDes = ParTable[TabIndex].ParDes;

	if(pParDes == NULL)		 								// initialisiert?
	{
	  return(RC_NULL_POINTER);						// keine Beschreibungsstruktur
	}


	switch (Attr & PAR_TYPES)												// typspez. Zugriff auf die Beschreibungsstruktur
	{
	  case _UINT16: 																// UINT16-Parameter
			if((Attr & PAR_WRITE) == PAR_WR_DIR || ((PAR_DES_U16_T*)pParDes)->pfFct == NULL)
			{
				*pWriteType = PAR_WR_DIR;									// Wert direkt schreiben
				*pAdr = ((PAR_DES_U16_T*)pParDes)->pVal;	// Zeiger ermitteln
			}
			else
			{
			  *pAdr = (void *)((PAR_DES_U16_T*)pParDes)->pfFct;	// spezielle Funktion verwenden
				*pWriteType = PAR_WR_FKT;									// Wert über Funktioin schreiben
			}
			break;
	  case _INT16:																	// INT16-Parameter	
			if((Attr & PAR_WRITE) == PAR_WR_DIR || ((PAR_DES_I16_T*)pParDes)->pfFct == NULL)
			{
				*pWriteType = PAR_WR_DIR;									// Wert direkt schreiben
				*pAdr = ((PAR_DES_I16_T*)pParDes)->pVal;	// Zeiger ermitteln
			}
			else
			{
			  *pAdr = (void *)((PAR_DES_I16_T*)pParDes)->pfFct;	// spezielle Funktion verwenden
				*pWriteType = PAR_WR_FKT;									// Wert über Funktioin schreiben
			}
			break;
	  case _UINT32: 																// UINT32-Parameter
			if((Attr & PAR_WRITE) == PAR_WR_DIR || ((PAR_DES_U32_T*)pParDes)->pfFct == NULL)
			{
				*pWriteType = PAR_WR_DIR;									// Wert direkt schreiben
				*pAdr = ((PAR_DES_U32_T*)pParDes)->pVal;	// Zeiger ermitteln
			}
			else
			{
			  *pAdr = (void *)((PAR_DES_U32_T*)pParDes)->pfFct;	// spezielle Funktion verwenden
				*pWriteType = PAR_WR_FKT;									// Wert über Funktioin schreiben
			}
			break; 
	  case _INT32:																	// INT32-Parameter
			if((Attr & PAR_WRITE) == PAR_WR_DIR || ((PAR_DES_I32_T*)pParDes)->pfFct == NULL)
			{
				*pWriteType = PAR_WR_DIR;									// Wert direkt schreiben
				*pAdr = ((PAR_DES_I32_T*)pParDes)->pVal;	// Zeiger ermitteln
			}
			else
			{
			  *pAdr = (void *)((PAR_DES_I32_T*)pParDes)->pfFct;	// spezielle Funktion verwenden
				*pWriteType = PAR_WR_FKT;									// Wert über Funktioin schreiben
			}
			break;
		case _WORD:																		// WORD-Parameter		
			if((Attr & PAR_WRITE) == PAR_WR_DIR || ((PAR_DES_WORD_T*)pParDes)->pfFct == NULL)
			{
				*pWriteType = PAR_WR_DIR;									// Wert direkt schreiben
				*pAdr = ((PAR_DES_WORD_T*)pParDes)->pVal;	// Zeiger ermitteln
			}
			else
			{
			  *pAdr = (void *)((PAR_DES_WORD_T*)pParDes)->pfFct;	// spezielle Funktion verwenden
				*pWriteType = PAR_WR_FKT;									// Wert über Funktioin schreiben
			}
			break;
		case _DWORD:																	// DWORD-Parameter	
			if((Attr & PAR_WRITE) == PAR_WR_DIR || ((PAR_DES_DWORD_T*)pParDes)->pfFct == NULL)
			{
				*pWriteType = PAR_WR_DIR;									// Wert direkt schreiben
				*pAdr = ((PAR_DES_DWORD_T*)pParDes)->pVal;// Zeiger ermitteln
			}
			else
			{
			  *pAdr = (void *)((PAR_DES_DWORD_T*)pParDes)->pfFct;	// spezielle Funktion verwenden
				*pWriteType = PAR_WR_FKT;									// Wert über Funktioin schreiben
			}
			break;
		default:																			// alle sonstigen Typen
			return(RC_WRONG_TYPE);
	}																								// switch(Attr & PAR_TYPES)
	return(RC_PARA_OK);
}


/********************************************************************************
* FUNKTIONSNAME:	static BOOL ParGetIndex(UINT16 ParNr, UINT16* indexx)					*
*********************************************************************************/
/*!\param			ParNr		-	gesuchte Parameternummer
*	 \param			index		- ermittelter entsprechender Tabellenindex
*  \remarks		ermittelt für den gesuchten Parameter den Tabellenindex der
*							Parametertabelle
*
*  \retval		TRUE, FALSE
********************************************************************************/
static BOOL ParGetIndex(UINT16 ParNr, UINT16 *TabIndexx)
{
	BOOL Found, Status = TRUE;
	UINT16 Start, End, Mid, Last;

	Found = FALSE;
	Start = 0;
	Last = End = /*MAX_PAR*/GetNumberOfParams();
	while (Start <= End)
	{
	  Mid = (Start + End) / 2;
	  if (Mid >= /*MAX_PAR*/Last)
	  {
	  	break;
	  }
	  if (ParNr == ParTable[Mid].index)
	  {
	    Found = TRUE;
	    *TabIndexx = Mid;
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
	}
	if (!Found)
	{
	 Status = FALSE;
	}

	return(Status);
}



/********************************************************************************
* FUNKTIONSNAME:	void Copy16_to_charbuf(UINT8* dest, UINT16 src)					*
*********************************************************************************/
/*!\param			UINT8* dest -	Ziel
*	 \param			UINT16 src	- Quelle
*
*  \remarks		Funktion wird für die 3964-Kommunikation benötigt, falls Hardware 16-Bit-System.
*							Kopiert von der Quelle 8-Bit-Weise ins Ziel.
*							GRUND: Es können immer nur 8 Bit auf einmal empfangen werden (siehe TmtBuf3964).
*
*  \retval		-
*********************************************************************************/
inline void Copy16_to_charbuf(UINT8* dest, UINT16 src)
{
	*dest			= (UINT8)	(src & 0xff);					// die unteren 8 Bits ins Ziel kopieren
	*(dest+1) = (UINT8)	((src & 0xff00)>>8);	// die oberen 8 Bits ins Ziel kopieren
}


/********************************************************************************
* FUNKTIONSNAME:	void Copy32_to_charbuf(UINT8* dest, UINT32 src)					*
*********************************************************************************/
/*!\param			UINT32* dest -	Ziel
*	 \param			UINT32 src	- Quelle
*
*  \remarks		Funktion wird für die 3964-Kommunikation benötigt, falls Hardware 16-Bit-System.
*							Kopiert von der Quelle 8-Bit-Weise ins Ziel.
*							GRUND: Es können immer nur 8 Bit auf einmal empfangen werden (siehe TmtBuf3964).
*
*  \retval		-
*********************************************************************************/
inline void Copy32_to_charbuf(UINT8* dest, UINT32 src)
{
	*dest			= (UINT8)	(src & 0xff);							// die unteren 8 Bits ins Ziel kopieren
	*(dest+1) = (UINT8)	((src & 0xff00)>>8);			// die zweiten 8 Bits ins Ziel kopieren
	*(dest+2) = (UINT8)	((src & 0xff0000)>>16);		// die dritten 8 Bits ins Ziel kopieren
	*(dest+3) = (UINT8)	((src & 0xff000000)>>24);	// die oberen 8 Bits ins Ziel kopieren
}


/********************************************************************************
* FUNKTIONSNAME:	void Copyfloat_to_charbuf(UINT8* dest, float src)					*
*********************************************************************************/
/*!\param			UINT8* dest -	Ziel
*	 \param			UINT32 src	- Quelle
*
*  \remarks		Funktion wird für die 3964-Kommunikation benötigt, falls Hardware 16-Bit-System.
*  						Kopiert von der Quelle 8-Bit-Weise ins Ziel.
*							GRUND: Es können immer nur 8 Bit auf einmal empfangen werden (siehe TmtBuf3964).
*
*  \retval		-
*********************************************************************************/
inline void Copyfloat_to_charbuf(UINT8* dest, float src)
{
	*dest			= (UINT8)((*(UINT32*)&src & 0xff)>>0);	// die unteren 8 Bits ins Ziel kopieren
	*(dest+1)	= (UINT8)((*(UINT32*)&src & 0xff00)>>8);	// die zweiten 8 Bits ins Ziel kopieren
	*(dest+2)	= (UINT8)((*(UINT32*)&src & 0xff0000)>>16);	// die dritten 8 Bits ins Ziel kopieren
	*(dest+3)	= (UINT8)((*(UINT32*)&src & 0xff000000)>>24);	// die oberen 8 Bits ins Ziel kopieren
}



/********************************************************************************
* FUNKTIONSNAME:	void Copy16_to_pointer(UINT16* dest, void* src)					*
*********************************************************************************/
/*!\param			UINT16* dest -	Ziel
*	 \param			void* src	- Quelle
*
*  \remarks		Funktion wird für die 3964-Kommunikation benötigt, falls Hardware 16-Bit-System.
*							Kopiert von der Quelle die ERSTEN und DRITTEN 8 Bits
*							und setzt sie zu einem 16 Bit-Pointer zusammen.
*							GRUND: Es können immer nur 8 Bit auf einmal empfangen werden (siehe RecBuf3964).
*
*  \retval		-
*********************************************************************************/
inline void Copy16_to_pointer(UINT16* dest, void* src)
{
	*(UINT16*)dest =	((*((UINT16*)src + 0)) << 0) |	// die ERSTEN 8 Bits der Quelle zum Ziel kopieren
										((*((UINT16*)src + 1)) << 8);		// die DRITTEN 8 Bits der Quelle zum Ziel kopieren
}


/********************************************************************************
* FUNKTIONSNAME:	void CopyUINT32_to_pointer(UINT32* dest, void* src)					*
*********************************************************************************/
/*!\param			UINT32* dest -	Ziel
*	 \param			void* 	- Quelle
*
*  \remarks		Funktion wird für die 3964-Kommunikation benötigt, falls Hardware 16-Bit-System.
*							Kopiert von der Quelle die ERSTEN/DRITTEN/FÜNFTEN/SIEBENTEN 8 Bits
*							und setzt sie zu einem 32 Bit-Pointer zusammen.
*							GRUND: Es können immer nur 8 Bit auf einmal empfangen werden (siehe RecBuf3964).
*
*  \retval		-
*********************************************************************************/
inline void CopyUINT32_to_pointer(UINT32* dest, void* src)
{
	static INT32 dest1, dest2, dest3, dest4;

	dest1 =		*((UINT16*)src);			// die ERSTEN (und zweiten) 8 Bits der Quelle kopieren
	dest2 =		*((UINT16*)src + 1);	// die DRITTEN (und vierten) 8 Bits der Quelle kopieren
	dest3 =		*((UINT16*)src + 2);	// die FÜNFTEN (und sechsten) 8 Bits der Quelle kopieren
	dest4 =		*((UINT16*)src + 3);	// die SIEBENTEN (und achten) 8 Bits der Quelle kopieren

	*(UINT32*)dest = dest1 | (dest2<<8) | (dest3<<16) | (dest4<<24);										
}

/********************************************************************************
* FUNKTIONSNAME:	void CopyINT32_to_pointer(UINT32* dest, void* src)					*
*********************************************************************************/
/*!\param			INT32* dest -	Ziel
*	 \param			void* 	- Quelle
*
*  \remarks		Funktion wird für die 3964-Kommunikation benötigt, falls Hardware 16-Bit-System.
*							Kopiert von der Quelle die ERSTEN/DRITTEN/FÜNFTEN/SIEBENTEN 8 Bits
*							und setzt sie zu einem 32 Bit-Pointer zusammen.
*							GRUND: Es können immer nur 8 Bit auf einmal empfangen werden (siehe RecBuf3964).
*
*  \retval		-
*********************************************************************************/
inline void CopyINT32_to_pointer(INT32* dest, void* src)
{
	static INT32 dest1, dest2, dest3, dest4;
	
	dest1 =		*((UINT16*)src);			// die ERSTEN (und zweiten) 8 Bits der Quelle kopieren
	dest2 =		*((UINT16*)src + 1);	// die DRITTEN (und vierten) 8 Bits der Quelle kopieren
	dest3 =		*((UINT16*)src + 2);	// die FÜNFTEN (und sechsten) 8 Bits der Quelle kopieren
	dest4 =		*((UINT16*)src + 3);	// die SIEBENTEN (und achten) 8 Bits der Quelle kopieren

	*(INT32*)dest = dest1 | (dest2<<8) | (dest3<<16) | (dest4<<24);
}


