// GetVersion.h : Haupt-Header-Datei f�r die Anwendung GETVERSION
//

#if !defined(AFX_GETVERSION_H__573E997C_D4DB_47D6_8A87_C06D131BDFD7__INCLUDED_)
#define AFX_GETVERSION_H__573E997C_D4DB_47D6_8A87_C06D131BDFD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// CGetVersionApp:
// Siehe GetVersion.cpp f�r die Implementierung dieser Klasse
//

class CGetVersionApp : public CWinApp
{
public:
	CGetVersionApp();

// �berladungen
	// Vom Klassenassistenten generierte �berladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CGetVersionApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CGetVersionApp)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingef�gt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VER�NDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // !defined(AFX_GETVERSION_H__573E997C_D4DB_47D6_8A87_C06D131BDFD7__INCLUDED_)
