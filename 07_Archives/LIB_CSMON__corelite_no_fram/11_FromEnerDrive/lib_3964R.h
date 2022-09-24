/*!*******************************************************************************
  \file			lib_3964R.h

  \remarks	spezieller header zu lib_3964R.c

  \author		Frank M�ller M&P GmbH
*********************************************************************************/

#ifndef LIB_3964R_H
#define LIB_3964R_H
/********************************************************************************
 *	system includes															    														*
 ********************************************************************************/

/********************************************************************************
 *	project includes																														*
 ********************************************************************************/

/********************************************************************************
 *	macros																																			*
 ********************************************************************************/
#define MAX_DATABLOCK_LEN	(MAX_DATA_3964 - sizeof(DATA_HEADER_3964)) // 255-6=249
/********************************************************************************
 *	typedefs																																		*
 ********************************************************************************/


//! Aufz�hlung der unterst�tzten Kommandos KDOS_3964
typedef enum	
{
  KDO_WRITE,		   		//!< Schreibkommando
  KDO_READ,						//!< Lesekommando (alle Parameter am Anfang in einem Rutsch lesen)
  KDO_WRITE_BACK,	   	//!< R�ckschreiben, TESTbetriebsart
  KDO_POLLING_BATCH, 	//!< mehrere Parameterwerte mit einem Mal lesen (zyklisches Update)
  KDO_READ_TRANSIENT 	//!< Transientenspeicher auslesen (Blockbetrieb)
} KDOS_3964;						


//! Protokollheader im Nutzdatenbereich des 3964R
typedef struct					
{
	UINT8	length;	   	//!< (0-tes Byte) Gesamtl�nge der Nutzdaten innerhalb 3964R
	UINT8 Kdo;		   	//!< (1-tes Byte) Kommandoauswahl
	UINT32 index;	   	//!< (2-tes und 3-tes Byte) Index des Parameters
	UINT8 subindex;	 	//!< (4-tes Byte) Subindex des Parameters
	UINT8 status;	   	//!< (5-tes Byte) Statusinformation �ber das Ergebnis des Vorganges
} DATA_HEADER_3964;

//! Buffer f�r Senden und Empfangen
// Achtung: Der Sendepuffer muss mindestens 4*RT_MAX_DATABLOCK_LEN gro� sein!
// Grund: Die zu sendenden Daten aus dem TSM (UINT32) m�ssen vor dem Senden 
// expandiert werden.
// Aus einem UINT32 im TSM werden vier(!) UINT16 in TmtBuf3964.Data!
// ACHTUNG: Data sollte etwas gr��er sein als wirklich ben�tigt,
// sonst sinkt die Geschwindigkeit beim Laden der Daten zum PC!!!
typedef struct
{
	DATA_HEADER_3964 header;		//!< Header innerhalb der Nutzdaten
	UINT8 Data[1024];						//!< Daten
} BUFFER_3964;

/********************************************************************************
 *	prototypes																																	*
********************************************************************************/
BOOL WriteRecDataBuf(UINT16 c, UINT16 count);
BOOL ReadTmtDataBuf(UINT16* c, UINT16 count);
void GetTmtDataBuf(UINT8** tmt);
void EvaluateData3964( void );

/********************************************************************************
 *	externs																																			*
 ********************************************************************************/
extern BUFFER_3964 RecBuf3964;
extern BUFFER_3964 TmtBuf3964;

#endif  // LIB_3964R_H
