// GetVersion.cpp : Legt das Klassenverhalten für die Anwendung fest.
//

#include "stdafx.h"
#include "GetVersion.h"
#include "GlobalFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetVersionApp

BEGIN_MESSAGE_MAP(CGetVersionApp, CWinApp)
	//{{AFX_MSG_MAP(CGetVersionApp)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetVersionApp Konstruktion

CGetVersionApp::CGetVersionApp()
{
	// ZU ERLEDIGEN: Hier Code zur Konstruktion einfügen
	// Alle wichtigen Initialisierungen in InitInstance platzieren
}

/////////////////////////////////////////////////////////////////////////////
// Das einzige CGetVersionApp-Objekt

CGetVersionApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CGetVersionApp Initialisierung

BOOL CGetVersionApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standardinitialisierung
	// Wenn Sie diese Funktionen nicht nutzen und die Größe Ihrer fertigen 
	//  ausführbaren Datei reduzieren wollen, sollten Sie die nachfolgenden
	//  spezifischen Initialisierungsroutinen, die Sie nicht benötigen, entfernen.

#ifdef _AFXDLL
	Enable3dControls();			// Diese Funktion bei Verwendung von MFC in gemeinsam genutzten DLLs aufrufen
#else
	Enable3dControlsStatic();	// Diese Funktion bei statischen MFC-Anbindungen aufrufen
#endif

  CString cs;
  cs.Format("FileVersion: %s\nProductVersion: %s\nMyPrivateInfo: %s", 
             CGlobalFunctions::GetFileVersionX(), 
             CGlobalFunctions::GetProductVersionX(),
             CGlobalFunctions::GetVersionInfo(NULL, "MyPrivateInfo"));
  AfxMessageBox(cs);
  
	// Da das Dialogfeld geschlossen wurde, FALSE zurückliefern, so dass wir die
	//  Anwendung verlassen, anstatt das Nachrichtensystem der Anwendung zu starten.
	return FALSE;
}
