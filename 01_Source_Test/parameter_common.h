/* *****************************************************************************
 * File:   parameter_common.h
 * Author: Dimitar Lilov
 *
 * Created on 2022 11 01 12:51
 * 
 * Description: ...
 * 
 **************************************************************************** */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Guard condition file contents not included more than once */  
/* #pragma once */
#ifndef PARAMETER_COMMON_H
#define	PARAMETER_COMMON_H

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
    
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define STRING_PARAMS

#define LANGUAGE_ENGLISH    1
//#define  LANGUAGE_GERMAN 1

#define NO_EVENTPARS       40


/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */
#define PAR_TYPES   0x000f          // Maske für Typencodierung im Parameterattribut

#define _UINT16         0x0001          // UINT16
#define _INT16          0x0002          // INT16
#define _UINT32         0x0003          // UINT32
#define _INT32          0x0004          // INT32
#define _WORD               0x0005          // WORD
#define _DWORD          0x0006          // DWORD
#define _A_UINT16       0x0007          // Array of UINT16
#define _A_INT16        0x0008          // Array of INT16
#define _A_UINT32       0x0009          // Array of UINT32
#define _A_INT32        0x000a          // Array of INT32
#define _A_WORD         0x000b          // Array of WORD
#define _A_DWORD        0x000c          // Array of DWORD
#define _UINT8          0x000d          //!< byte
#define _A_UINT8        0x000e          //!< string

#define PAR_ACCESS  0x0030          // Maske für Zugriffscodierung im Parameterattribut
#define PAR_NA          0x0000          // kein Zugriff möglich
#define PAR_RO          0x0010          // ReadOnly
#define PAR_WO          0x0020          // WriteOnly
#define PAR_RW          0x0030          // ReadWrite

#define PAR_STATE       0x0040          // verbietet Schreibzugriffe in bestimmten Zuständen
#define PAR_PASS_PR     0x0080          // Zugriff nur mit Passcode, Parameter hidden
#define PAR_PASS_PW     0x0100          // Zugriff nur mit Passcode, read erlaubt

#define PAR_A_BIT      0x0200           // bit array

#define PAR_WRITE       0x0c00          // Schreib modi
#define PAR_WR_DIR  0x0400          // direkter Schreibzugriff möglich
#define PAR_WR_FKT  0x0800          // Schreibzugriff nur über spezielle Behandlungsfunktion

#define PAR_STORE_EE    0x4000          // Parameter for storing in EEPROM
#define PAR_STORE       0x8000          // Parameter for storing in Parameterfile

#define PAR_MENUE       0x1000          // Parameter beschreibt Parametergruppe
#define PAR_HIDE        0x2000          // Parameter erscheint nicht im Menübaum

#define PAR_UPDATE  0x0001          // Kennung eines Parameters im RAM-Spiegel des EEPROM zum Speichern
#define PAR_EQUAL       0x0002          // Kennung .... Speichern nicht nötig

#define PAR_READY       0x0000          // keine Parameter nv-mem aktion aktiv
#define PAR_BUSY        0x0001          // eine Parameter nv-mem aktion ist aktiv




#define MAX_INT                0x7fff
#define MAX_UINT               0xffff
#define MAX_LONG               0x7fffffffL
#define MAX_ULONG              0xffffffffL


/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */
//! return codes des Parameterinterfaces
typedef enum
{
    RC_PARA_OK,                                     //!< 0, fehlerfreier Zugriff
    RC_PARA_NOT_EXIST,                      //!< 1, Parameter existiert nicht
    RC_ELEM_NOT_EXIST,                      //!< 2, Element existiert nicht
    RC_ATTR_NOT_EXIST,                      //!< 3, Attribut existiert nicht
    RC_NULL_POINTER,                            //!< 4, es wurde ein NULL-Pointer übergeben
    RC_WRONG_TYPE,                              //!< 5, falscher Typ
    RC_WRONG_SIZE,                              //!< 6, falsche Größe
    RC_VALUE_HIGH,                              //!< 7, Wert zu groß
    RC_VALUE_LOW,                                   //!< 8, Wert zu klein
    RC_READ_ONLY,                                   //!< 9, Schreibversuch auf Read-only
    RC_WRITE_ONLY,                              //!< 10, Leseversuch auf Write-only
    RC_PERM_DENIED,                             //!< 11, keine Zugriffsberechtigung
    RC_PAR_WRONG_CMD,                           //!< 12, falscher Kommandocode
    RC_PAR_WRONG_FRAME,                     //!< 13, Gesamtframe header + Daten hat falsche Länge
    RC_PAR_WRONG_STATE_WRITE,           //!< 14, Schreiben im aktuellen Zustand nicht erlaubt
    RC_PAR_UNVALID_DATA                     //!< 15, ungültige Daten
} RC_PAR_T;


//! Sprachen
typedef enum
{
    LANG_GERMAN,                                    //!< Deutsch
    LANG_ENGLISH                                    //!< Englisch
} LANGUAGE_T;

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */
#ifndef NULL
#define NULL  ((void *)0)
#endif

typedef unsigned int     UINT16;
typedef unsigned char    BOOL;
typedef signed int       INT16;
typedef signed long      INT32;
typedef signed long long INT64;
typedef char             INT8;
typedef char                 UINT8;
typedef unsigned long    UINT32;
typedef unsigned long long UINT64;
typedef unsigned int    WORD;
typedef unsigned long   DWORD;
typedef int int8_T;
typedef unsigned int uint8_T;
typedef int int16_T;
typedef unsigned int uint16_T;
typedef long int32_T;
typedef unsigned long uint32_T;
typedef float real32_T;
typedef double real64_T;

//*******************************************************************************
//  Beschreibungsstruktur für einen U16-Parameter
typedef const struct
{
  UINT16    Attr;                                       //!< El. 1, Attribute des Parameters
  //INT8*   Name;                                       //!< El. 2, Name
  UINT8*    Name;                                       //!< El. 2, Name
  UINT8*    Unit;                                       //!< El. 3, Einheit
  UINT16    Max;                                        //!< El. 4, Maximalwert
  UINT16    Min;                                        //!< El. 5, Minimalwert
  UINT16    Def;                                        //!< El. 6, Defaultwert
  float     Norm;                                       //!< El. 7, Normierungsfaktor nur für Darstellung auf Anzeigegerät
  UINT16*   pVal;                                       //!< El. 8, Zeiger auf Wert
  RC_PAR_T (*pfFct) (UINT16);           //!< Zeiger auf spezielle Behandlungsfunktion
} PAR_DES_U16_T;
//*******************************************************************************

//  Beschreibungsstruktur für einen I16-Parameter
typedef const struct
{
  UINT16    Attr;                                   //!< El. 1, Attribute des Parameters
  UINT8*    Name;                                   //!< El. 2, Name
  UINT8*    Unit;                                   //!< El. 3, Einheit
  INT16     Max;                                    //!< El. 4, Maximalwert
  INT16     Min;                                    //!< El. 5, Minimalwert
  INT16     Def;                                    //!< El. 6, Defaultwert
  float     Norm;                                   //!< El. 7, Normierungsfaktor nur für Darstellung auf Anzeigegerät
  INT16*    pVal;                                   //!< El. 8, Zeiger auf Wert
  RC_PAR_T (*pfFct) (INT16);        //!< Zeiger auf spezielle Behandlungsfunktion
} PAR_DES_I16_T;

//  Beschreibungsstruktur für einen U32-Parameter
typedef const struct
{
  UINT16    Attr;                                       //!< El. 1, Attribute des Parameters
  UINT8*    Name;                                       //!< El. 2, Name
  UINT8*    Unit;                                       //!< El. 3, Einheit
  UINT32    Max;                                        //!< El. 4, Maximalwert
  UINT32    Min;                                        //!< El. 5, Minimalwert
  UINT32    Def;                                        //!< El. 6, Defaultwert
  float     Norm;                                       //!< El. 7, Normierungsfaktor nur für Darstellung auf Anzeigegerät
  UINT32*   pVal;                                       //!< El. 8, Zeiger auf Wert
  RC_PAR_T (*pfFct) (UINT32);           //!< Zeiger auf spezielle Behandlungsfunktion
} PAR_DES_U32_T;

// Beschreibungsstruktur für einen I32-Parameter
typedef const struct
{
  UINT16    Attr;                                   //!< El. 1, Attribute des Parameters
  UINT8*    Name;                                   //!< El. 2, Name
  UINT8*    Unit;                                   //!< El. 3, Einheit
  INT32     Max;                                    //!< El. 4, Maximalwert
  INT32     Min;                                    //!< El. 5, Minimalwert
  INT32     Def;                                    //!< El. 6, Defaultwert
  float     Norm;                                   //!< El. 7, Normierungsfaktor nur für Darstellung auf Anzeigegerät
  INT32*    pVal;                                   //!< El. 8, Zeiger auf Wert
  RC_PAR_T (*pfFct) (INT32);        //!< Zeiger auf spezielle Behandlungsfunktion
} PAR_DES_I32_T;

// Beschreibungsstruktur für einen WORD-Parameter
typedef const struct
{
  UINT16    Attr;                                   //!< El. 1, Attribute des Parameters
  UINT8*    Name;                                   //!< El. 2, Name
  WORD      Max;                                    //!< El. 4, Maximalwert
  WORD      Min;                                    //!< El. 5, Minimalwert
  WORD      Def;                                    //!< El. 6, Defaultwert
  WORD* pVal;                                       //!< El. 8, Zeiger auf Wert
  RC_PAR_T (*pfFct) (WORD);         //!< Zeiger auf spezielle Behandlungsfunktion
} PAR_DES_WORD_T;

// Beschreibungsstruktur für einen DWORD-Parameter
typedef const struct
{
  UINT16    Attr;                                   //!< El. 1, Attribute des Parameters
  UINT8*    Name;                                   //!< El. 2, Name
  DWORD     Max;                                    //!< El. 4, Maximalwert
  DWORD     Min;                                    //!< El. 5, Minimalwert
  DWORD     Def;                                    //!< El. 6, Defaultwert
  DWORD*    pVal;                                   //!< El. 8, Zeiger auf Wert
  RC_PAR_T (*pfFct) (DWORD);        //!< Zeiger auf spezielle Behandlungsfunktion
} PAR_DES_DWORD_T;

//! Beschreibungsstruktur für einen UINT8-Feld-Parameter
typedef const struct
{
  UINT16    Attr;                                   //!< El. 1, Attribute des Parameters
  UINT8*    Name;                                   //!< El. 2, Name
  UINT16    size;                                   //!< El. 3, Größe des Felds
  UINT8*    pVal;                                   //!< El. 4, Zeiger auf Wert
  RC_PAR_T (/*far*/ *pfFct)(const UINT8*, UINT16);//!< Zeiger auf spezielle Behandlungsfunktion
} PAR_DES_A_UINT8_T;

// Eintrag in Parametertabelle für einen Parameter
//typedef const struct
//{
//    UINT16 index;                       //!< El. 0, ParameterNr.
//    //const UINT16* Attr;       //!< El. 1, Attribut des Parameters
//    void* ParDes;                       //!< Zeiger auf Beschreibungsstruktur
//} PAR_TAB_ENTRY_T;


/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
 
#endif	/* PARAMETER_COMMON_H */


#ifdef __cplusplus
}
#endif /* __cplusplus */
