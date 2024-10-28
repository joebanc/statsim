//////////////////////////////////////////////////////
//
// NRDB Pro - Spatial database and mapping application
//
// Copyright (c) 1989-2004 Richard D. Alexander
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// NRDB Pro is part of the Natural Resources Database Project 
// 
// Homepage: http://www.nrdb.co.uk/
// 

#include "stdafx.h" 
#include <initguid.h>
#include <odbcinst.h>
#include <direct.h>
#include <io.h>

#include "nrdbpro.h"
#include "MainFrm.h"
#include "docmap.h"
#include "viewmap.h"
#include "framemap.h"
#include "docgraph.h"
#include "viewgraph.h"
#include "docbdreport.h"
#include "importodbc.h"
#include "importmaplines.h"
#include "bdhtmlview.h"
#include "framereport.h"
#include "dlgseldictionary.h"
#include "ComboBoxFType.h"
#include "dlglogin.h"
#include "docsummary.h"
#include "doctsgraph.h"
#include "viewtsgraph.h"
#include "dlgprojection.h"
#include "viewmap.h"
#include "sheetreportsettings.h"
#include "bdimportexport.h"
#include "dlgabout.h"
#include "importtextfile.h"
#include "shapefile.h"
#include "dlgaddlayer.h"
#include "dochtmlhelp.h"
#include "dlgnew.h"
#include "docpiechart.h"
#include "viewpiechart.h"
#include "doctsreport.h"
#include "comboboxsymbol.h"
#include "dlgftype.h"
#include "dlgfeatures.h"
#include "viewmaplayout.h"
#include "docmaplayout.h"
#include "framemaplayout.h"
#include "dlgconnect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////

#define HELP_DBSETUP 0x10000

#define LAYOUTFILE "layout.xml"
#define DEFAULT_NRM "default.nrm"

///////////////////////////////////////////////////////////////////////////////

CNRDBApp* BDGetApp();

BDHANDLE BDHandle() {return BDGetApp()->GetDBHandle();}
BDHANDLE& BDHandleRef() {return BDGetApp()->GetDBHandle();}
CNRDBApp* BDGetApp() {return (CNRDBApp*)AfxGetApp();}
CString BDString(int nId) {return BDGetApp()->LoadString(nId);};
CProjection* BDProjection() {return BDGetApp()->GetProjection();};
CDocMap* BDGetDocMap() {return BDGetApp()->GetDocMap();}
void BDSetProgressBar(CBDProgressBar* p) {BDGetApp()->SetProgressBar(p);}   
CBDProgressBar* BDGetProgressBar() {return BDGetApp()->GetProgressBar();}   
void BDSetProgressText(CStatic* pEdit) {BDGetApp()->SetProgressText(pEdit);}
CStatic* BDGetProgressText() {return BDGetApp()->GetProgressText();}
CBDMain& BDGetSettings() {return BDGetApp()->GetSettings();};
LPCSTR BDGetDataSource() {return BDGetApp()->GetDataSource();}   
void BDGetPrinterInfo(DEVMODE* pDevMode) {BDGetApp()->GetPrinterInfo(pDevMode);}
CMultiDocTemplate* BDGetDocTemplate(LPCSTR pDocName) {return BDGetApp()->GetDocTemplate(pDocName);}
BDOnHelpNrdbhomepage() {BDGetApp()->OnHelpNrdbhomepage(); return 0;}
long BDFTypeSel() {return BDGetApp()->GetFTypeSel();}
void BDFTypeSel(long lFType) {BDGetApp()->SetFTypeSel(lFType);}
LPCSTR BDGetHtmlHelp() {return BDGetApp()->GetHtmlHelp();}
void BDSetHtmlHelp(LPCSTR s, BOOL bTemp) {BDGetApp()->SetHtmlHelp(s, bTemp);}
int BDGetPreviousReport() {return BDGetApp()->GetPreviousReport();}
void BDSetPreviousReport(int nType) {BDGetApp()->SetPreviousReport(nType);}
void BDOnReportSummary() {BDGetApp()->OnReportSummary();}
HWND& BDHwndDialog() {return BDGetApp()->m_hwndDialog;}
CToolTipCtrl*& BDToolTip() {return BDGetApp()->m_gpToolTip;}
CString BDGetAppPath() {return BDGetApp()->GetAppPath();}
LPCSTR BDGetUser() {return BDGetApp()->m_sUser;};
LPCSTR BDGetPassword() {return BDGetApp()->m_sPassword;};
LPCSTR BDGetHost() {return BDGetApp()->m_sHost;};
BOOL BDIsMySQL(BDHANDLE handle) {CString sVersion; BDGetDriver(BDHandle(), sVersion); return sVersion.Left(5) == "MySQL";};


CFeatureArray& BDFeatureSel() {return BDGetApp()->GetFeatureSel();}
CFTypeAttrArray& BDAttrSel() {return BDGetApp()->GetAttrSel();}
BOOL BDIsAttrSel(long lFType, long lAttr) 
{
   int i = 0; for (i = 0; i < BDAttrSel().GetSize(); i++)
   {
      if (BDAttrSel().GetAt(i).GetFTypeId() == lFType && 
          BDAttrSel().GetAt(i).GetAttrId() == lAttr) return TRUE;
   }
   return FALSE;
}
BOOL BDIsFeatureSel(long lFType, long lFeature) 
{
   return BDFeatureSel().IsFeatureSel(lFType, lFeature);
}

/////////////////////////////////////////////////////////////////////////////

#define MENU_REPORT 4
#define MENU_MAP 3
#define MENU_FILE 0
#define MENU_ITEMS 7

/////////////////////////////////////////////////////////////////////////////

typedef CArray <CFTypeAttr, CFTypeAttr> CFTypeAttrArray;

/////////////////////////////////////////////////////////////////////////////

CString CNRDBApp::m_sLoadString;

/////////////////////////////////////////////////////////////////////////////
// CNRDBApp

BEGIN_MESSAGE_MAP(CNRDBApp, CWinApp)
	//{{AFX_MSG_MAP(CNRDBApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_REPORT_GRAPH, OnReportGraph)
	ON_COMMAND(ID_REPORT_TABLE, OnReportTable)
	ON_COMMAND(ID_IMPORT_ODBC, OnImportODBC)	
	ON_COMMAND(ID_REPORT_SUMMARY, OnReportSummary)
	ON_COMMAND(ID_REPORT_TSGRAPH, OnReportTsgraph)	
   ON_COMMAND(ID_REPORT_PIECHART, OnReportPieChart)	
   ON_COMMAND(ID_REPORT_TIMESERIES, OnReportTSReport)	
	ON_COMMAND(ID_TOOLS_SETTINGS, OnToolsSettings)
   ON_COMMAND(ID_REPORT_WIZARD, OnReportWizard)
   ON_COMMAND(ID_REPORT_WIZARD_SETUP, OnReportWizardSetup)
   ON_COMMAND(ID_VIEW_LAYERS, OnViewLayers)
	ON_COMMAND(ID_IMPORT_FILE, OnImportFile)
   ON_COMMAND(ID_IMPORT_SHAPEFILE, OnImportShapefile)
	ON_COMMAND(ID_CONVERT_SHAPEFILE, OnConvertShapefile)
	ON_COMMAND(ID_EXPORT_SHAPEFILE, OnExportShapefile)
	ON_COMMAND(ID_FILE_LOGOUT, OnFileLogout)
	ON_COMMAND(ID_REPORT_PREVIOUS, OnReportPrevious)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_SHAPEFILE, OnUpdateExportShapefile)
	ON_COMMAND(ID_HELP_NRDBHOMEPAGE, OnHelpNrdbhomepage)
	ON_COMMAND(ID_HELP_SUPPORT, OnHelpSupport)
	ON_COMMAND(ID_HELP_NRDBTUTORIAL, OnHelpTutorial)	
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_SETUP_FEATURETYPES, OnFileSetupFeatureTypes)
	ON_COMMAND(ID_FILE_SETUP_FEATURES, OnFileSetupFeatures)
   ON_UPDATE_COMMAND_UI(ID_IMPORT_ODBC, OnUpdateFTypesExist)	
	ON_UPDATE_COMMAND_UI(ID_REPORT_PREVIOUS, OnUpdateReportPrevious)
   ON_UPDATE_COMMAND_UI(ID_REPORT_WIZARD, OnUpdateFTypesExist)
   ON_UPDATE_COMMAND_UI(ID_REPORT_WIZARD_SETUP, OnUpdateFTypesExist)
   ON_UPDATE_COMMAND_UI(ID_IMPORT_SHAPEFILE, OnUpdateFTypesExist)
	ON_UPDATE_COMMAND_UI(ID_FILE_SETUP_FEATURES, OnUpdateFTypesExist)
	ON_UPDATE_COMMAND_UI(ID_REPORT_GRAPH, OnUpdateFTypesExist)
	ON_UPDATE_COMMAND_UI(ID_REPORT_SUMMARY, OnUpdateFTypesExist)
	ON_UPDATE_COMMAND_UI(ID_REPORT_TABLE, OnUpdateFTypesExist)
	ON_UPDATE_COMMAND_UI(ID_REPORT_TIMESERIES, OnUpdateFTypesExist)
	ON_UPDATE_COMMAND_UI(ID_REPORT_TSGRAPH, OnUpdateFTypesExist)
   ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFTypesExist)   
	ON_COMMAND(ID_HELP_REGISTER, OnHelpRegister)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CNRDBApp construction

CNRDBApp::CNRDBApp()
{
   m_pProgressBar = NULL;
   m_pProgessText = NULL;

   m_hwndDialog = NULL;      
   m_gpToolTip = NULL;

   m_hConnect = (BDHANDLE)-1;

   m_bFType = TRUE;   

   m_nPreviousReport = -1;   
}

/////////////////////////////////////////////////////////////////////////////

CNRDBApp::~CNRDBApp()
{

}

/////////////////////////////////////////////////////////////////////////////
// The one and only CNRDBApp object

CNRDBApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CNRDBApp initialization

BOOL CNRDBApp::InitInstance()
{
   CFeatureType ftype;
  
   // Allow the user of active-x controls

   AfxEnableControlContainer();
   
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

   SetRegistryKey(_T("Natural Resources Database"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)      
     
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_MAPTYPE,
		RUNTIME_CLASS(CDocMap),
		RUNTIME_CLASS(CFrameMap), // custom MDI child frame
		RUNTIME_CLASS(CViewMap));
	AddDocTemplate(pDocTemplate);

   // Register the template

	pDocTemplate = new CMultiDocTemplate(
		IDR_GRAPHTYPE,
		RUNTIME_CLASS(CDocGraph), 
		RUNTIME_CLASS(CMDIChildWnd),
		RUNTIME_CLASS(CViewGraph));
	AddDocTemplate(pDocTemplate); 

   pDocTemplate = new CMultiDocTemplate(
		IDR_TSGRAPHTYPE,
		RUNTIME_CLASS(CDocTSGraph), 
		RUNTIME_CLASS(CMDIChildWnd),
		RUNTIME_CLASS(CViewTSGraph));
	AddDocTemplate(pDocTemplate); 

   pDocTemplate = new CMultiDocTemplate(
      IDR_PIECHART,
		RUNTIME_CLASS(CDocPieChart), 
		RUNTIME_CLASS(CMDIChildWnd),
		RUNTIME_CLASS(CViewPieChart));
	AddDocTemplate(pDocTemplate); 

   pDocTemplate = new CMultiDocTemplate(
      IDR_TSREPORT,  
      RUNTIME_CLASS(CDocTSReport), 
		RUNTIME_CLASS(CFrameReport),
		RUNTIME_CLASS(CBDHtmlView));
   AddDocTemplate(pDocTemplate); 

   pDocTemplate = new CMultiDocTemplate(
      IDR_HTMLREPORT,
      RUNTIME_CLASS(CDocBDReport), 
		RUNTIME_CLASS(CFrameReport),
		RUNTIME_CLASS(CBDHtmlView));
   AddDocTemplate(pDocTemplate);

   pDocTemplate = new CMultiDocTemplate(
      IDR_HTMLREPORT,
      RUNTIME_CLASS(CDocBDReport), 
		RUNTIME_CLASS(CFrameReport),
		RUNTIME_CLASS(CBDHtmlView));
   AddDocTemplate(pDocTemplate);

   pDocTemplate = new CMultiDocTemplate(
      IDR_SUMMARYREPORT,
      RUNTIME_CLASS(CDocSummary), 
		RUNTIME_CLASS(CFrameReport),
		RUNTIME_CLASS(CBDHtmlView));
   AddDocTemplate(pDocTemplate);

   pDocTemplate = new CMultiDocTemplate(
      IDR_HTMLHELP,
      RUNTIME_CLASS(CDocHtmlHelp), 
		RUNTIME_CLASS(CFrameReport),
		RUNTIME_CLASS(CBDHtmlView));
   AddDocTemplate(pDocTemplate);

   pDocTemplate = new CMultiDocTemplate(
      IDR_MAPLAYOUT,
      RUNTIME_CLASS(CDocMapLayout), 
		RUNTIME_CLASS(CFrameMapLayout),
		RUNTIME_CLASS(CViewMapLayout));
   AddDocTemplate(pDocTemplate);


	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;   
   	
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();      
   
	// Display about box

	CDlgAbout dlg(TRUE);
	dlg.DoModal();

   // Check for default map

   if (_access(BDGetAppPath() + DEFAULT_NRM,00) == 0)
   {
      m_sDefaultMap = BDGetAppPath() + DEFAULT_NRM;
   }
   
   // Login

   if (!Login())
   {
      return FALSE;
   }

   // Retrieve settings

   BDMain(BDHandle(), &m_BDMain, BDGETINIT);
   BDEnd(BDHandle());

   // Load custom symbols

   CComboBoxSymbol::InitCustomSym();   
   
   // Set the printer to landscape

   SetLandscape();            

   // Open the file specified on the command line

   CDocMap* pDocMap = GetDocMap();
   if ((m_lpCmdLine[0] != '\0' || m_sDefaultMap != "") && pDocMap != NULL)
   {    
      if (m_lpCmdLine[0] != '\0') GetDocMap()->Open(m_lpCmdLine);  
      else GetDocMap()->Open(m_sDefaultMap);

      // Reset command line

      m_lpCmdLine[0] = '\0';
      m_sDefaultMap = "";
   }    

   BDEnd(BDHandle());   
 
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CNRDBApp::Login()
{
   BOOL bOK = TRUE;
   BOOL bConnect = FALSE;

  // Retrieve the default data source name

    CString sDataSource = DefaultDataSource();	  

   // If using MySQL then login prior to connecting to database
   
   if (BDIsMySQL(BDHandle()))
   {
      CDlgConnect dlg;

      m_sUser = dlg.GetUserName();
      m_sPassword = dlg.GetPassword();
      m_sHost = dlg.GetHost();

      // Try connecting using previous username and password

       BDHANDLE handle;
       while (!BDConnect(NULL, &handle, m_sUser, m_sPassword, m_sHost) && bOK)
       {        
          AfxMessageBox("Please enter the user name and password to connect to MySQL");

          if (dlg.DoModal() == IDOK)
          {
             m_sUser = dlg.GetUserName();
             m_sPassword = dlg.GetPassword();
             m_sHost = dlg.GetHost();                                       
          } else
          {
             bOK = FALSE;                
          }           
       };
       if (bOK)
       {
          BDDisconnect(handle);       
       };
   }
	
   // Connect to the database   
      
   do 
   {
      TRY
      {

      bOK = TRUE;
      CDlgLogin dlg(sDataSource);
      if (dlg.DoModal() != IDOK) return FALSE;

      BeginWaitCursor();
      int nRet = BDConnect(dlg.GetDataSource(), &m_hConnect, m_sUser, m_sPassword, m_sHost);
      EndWaitCursor();

      if (nRet == -1)
      {
         /*if (AfxMessageBox(BDString(IDS_INVALIDNRDB) + "\r\nWould you like to create the necessary NRDB tables?", MB_YESNO) == IDYES)
         {
            // Create tables
            BDInitialise(dlg.GetDataSource());                        
         }
         bOK = FALSE;*/
      } else
      {
         bOK = nRet == TRUE;
      };
   
      m_sDataSource = dlg.GetDataSource();      

      // Trap database errors

      }
      CATCH (CDBException, pEx)
      {
         AfxMessageBox(pEx->m_strError);    
         bOK = FALSE;
      }
      END_CATCH

   } while (!bOK);

   BDEnd(BDHandle());

   // Determine the projection

   m_projection.InitialiseProjection();      

   // Retrieve layout and units

   LoadLayout();   
   m_units.LoadUnits();
   
   // Open the map
       
   BeginWaitCursor();

   CDocMap* pDoc;
   CMultiDocTemplate* pTemplate = GetDocTemplate("Map");      
   pDoc = (CDocMap*)pTemplate->OpenDocumentFile(NULL);   
   EndWaitCursor();    

   // Retrieve default feature type

   m_lFTypeSel = GetProfileInt(m_sDataSource, "FTypeId", 0);

   // Reset default colour 

   CDlgAddLayer::Reset();

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

void CNRDBApp::OnFileLogout() 
{
   CFeatureType ftype;

 // Logff 

   Logout();

 // Re-login

  if (Login())
  {     
      // Reset sectors

      CComboBoxFType::InitDictionary(TRUE);

  } else
  {
     OnAppExit();
  }
}

/////////////////////////////////////////////////////////////////////////////

void CNRDBApp::Logout()
{
   // Disconnect from the database

  if ((int)m_hConnect >= 0)
  {

   if (BDGetTransLevel(BDHandle()) > 0) BDCommit(BDHandle());
   ASSERT(BDGetTransLevel(BDHandle()) == 0);

      BeginWaitCursor();
      BDDisconnect(m_hConnect);
      EndWaitCursor();
      m_hConnect = (BDHANDLE)-1;

      // Write the default feature for the database

      WriteProfileInt(m_sDataSource, "FTypeId", m_lFTypeSel);

      // Write the default colour

      CString s;

      COLORREF cr = CDlgAddLayer::GetDefaultColour();
      s.Format("%i,%i,%i", GetRValue(cr), GetGValue(cr), GetBValue(cr));
      WriteProfileString(m_sDataSource, "DefaultColour", s);

   };	
  
 // Close all windows

  CloseAllDocuments(FALSE);

  // Save the map layouts and units

   SaveLayout();
   GetUnits().SaveUnits();

 // Reset selections

  m_lFTypeSel = 0;
  m_aFeatureSel.RemoveAll();
  m_aAttrSel.RemoveAll();  


}

/////////////////////////////////////////////////////////////////////////////

int CNRDBApp::ExitInstance()
{    
   // Logoff

   Logout();

	return CWinApp::ExitInstance();

}

///////////////////////////////////////////////////////////////////////////////
//
// void CNRDBApp::OnAppExit() 
//
// Disconnect from the database
//

void CNRDBApp::OnAppExit() 
{

   CWinApp::OnAppExit();
}

///////////////////////////////////////////////////////////////////////////////

BOOL CNRDBApp::PreTranslateMessage(MSG* pMsg) 
{
   return CWinApp::PreTranslateMessage(pMsg);   		
}

///////////////////////////////////////////////////////////////////////////////
//
// Returns the template corresponding to the document name provided
//

CMultiDocTemplate* CNRDBApp::GetDocTemplate(LPCSTR pDocName)
{
      CMultiDocTemplate* pTemplate = NULL;
      CString strDocName;
      BOOL bFound = FALSE;

   // Search through document templates for one specified.
   // Nb. 32 bit version of foundation classes has a separate 
   // document manager class

     POSITION pos = m_pDocManager->GetFirstDocTemplatePosition();
      
      while (pos != NULL && bFound == FALSE)      
      {            
         pTemplate = (CMultiDocTemplate*)GetNextDocTemplate(pos);
         pTemplate->GetDocString(strDocName,CDocTemplate::docName);          
          
         if (strDocName == pDocName)             
         { 
            bFound = TRUE;
         }         
     }         

     if (bFound)
     { 
        return pTemplate;
     } else
     {
        return NULL;   
     };
};

///////////////////////////////////////////////////////////////////////////////

CDocMap* CNRDBApp::GetDocMap()
{
   CMultiDocTemplate* pTemplate = GetDocTemplate("Map");      

   POSITION pos = pTemplate->GetFirstDocPosition();
   if (pos != NULL)
   {
      CDocMap* pDoc = (CDocMap*)pTemplate->GetNextDoc(pos);

      ASSERT(pDoc != NULL && pDoc->IsKindOf(RUNTIME_CLASS(CDocMap)));
      return pDoc;
   }
   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void CNRDBApp::OnReportGraph() 
{
	CMultiDocTemplate* pTemplate = GetDocTemplate("Graph");      
   pTemplate->OpenDocumentFile(NULL);   	
}

///////////////////////////////////////////////////////////////////////////////

void CNRDBApp::OnReportTsgraph()  
{
	CMultiDocTemplate* pTemplate = GetDocTemplate("TSGraph");      
   pTemplate->OpenDocumentFile(NULL);   		
}

void CNRDBApp::OnReportPieChart()
{
   CMultiDocTemplate* pTemplate = GetDocTemplate("PieChart");      
   pTemplate->OpenDocumentFile(NULL);   		
}

void CNRDBApp::OnReportTSReport()
{
   CMultiDocTemplate* pTemplate = GetDocTemplate("TSReport");      
   pTemplate->OpenDocumentFile(NULL);   		
}

///////////////////////////////////////////////////////////////////////////////

void CNRDBApp::OnReportTable() 
{
   CMultiDocTemplate* pTemplate = GetDocTemplate("HtmlReport");      
   pTemplate->OpenDocumentFile(NULL);   		
}

///////////////////////////////////////////////////////////////////////////////

void CNRDBApp::OnReportSummary() 
{
   CMultiDocTemplate* pTemplate = GetDocTemplate("SummaryReport");      
   pTemplate->OpenDocumentFile(NULL);   			
}

///////////////////////////////////////////////////////////////////////////////

void CNRDBApp::OnImportODBC() 
{
	CImportDB import;
	import.ImportODBC();		   
}

///////////////////////////////////////////////////////////////////////////////

void CNRDBApp::OnImportShapefile() 
{
	CImportTextFile import;
   import.ImportShapefile();	
}

///////////////////////////////////////////////////////////////////////////////

void CNRDBApp::OnImportFile() 
{
	CImportTextFile import;
   import.ImportFile();	

   // Determine if feature types exist

   CFeatureType ftype;
   m_bFType = BDFeatureType(BDHandle(), &ftype, BDGETINIT);
   BDEnd(BDHandle());
}

///////////////////////////////////////////////////////////////////////////////

void CNRDBApp::OnReportPrevious() 
{
   m_bPreviousReport = TRUE;
   switch (m_nPreviousReport)
   {

      case CNRDB::maplayer : OnViewLayers(); break;
      case CNRDB::mapquery : OnViewLayers(); break;
      case CNRDB::standardreport : OnReportTable(); break;
      case CNRDB::reportquery : OnReportTable(); break;
      case CNRDB::summaryreport : OnReportSummary(); break;
      case CNRDB::histogram : OnReportGraph(); break;
      case CNRDB::histogramquery : OnReportGraph(); break;
      case CNRDB::timeseries : OnReportTsgraph(); break;
      case CNRDB::timeseriesquery: OnReportTsgraph(); break;
      case CNRDB::piechart : OnReportPieChart(); break;
      case CNRDB::piechartquery : OnReportPieChart(); break;
      case CNRDB::tsreport : OnReportTSReport(); break;
      case CNRDB::tsreportquery : OnReportTSReport(); break;
   }
   m_bPreviousReport = FALSE;
}

///////////////////////////////////////////////////////////////////////////////

void CNRDBApp::OnReportWizard() 
{
}

///////////////////////////////////////////////////////////////////////////////

void CNRDBApp::OnReportWizardSetup() 
{
}

///////////////////////////////////////////////////////////////////////////////

void CNRDBApp::OnViewLayers()
{
   GetDocMap()->OnLayers();
}

///////////////////////////////////////////////////////////////////////////////

void CNRDBApp::OnExportShapefile() 
{
   GetDocMap()->OnExportShapefile();
}

///////////////////////////////////////////////////////////////////////////////

void CNRDBApp::OnUpdateExportShapefile(CCmdUI* pCmdUI) 
{
   CDocMap* pDocMap = GetDocMap();
   if (pDocMap != NULL)
   {
	   int i = pDocMap->GetLayers()->GetDefault();   
      pCmdUI->Enable(i < GetDocMap()->GetLayers()->GetSize());   		
   };
}

///////////////////////////////////////////////////////////////////////////////
//
// Changes the printer to landscape.  Code taken from KB article Q126897
//

void CNRDBApp::SetLandscape()
{
   LPDEVNAMES lpDevNames;
   LPTSTR lpszDriverName, lpszDeviceName, lpszPortName;
   HANDLE hPrinter;      
   PRINTDLG   pd;

 // Get default printer settings.    PRINTDLG   pd;
   pd.lStructSize = (DWORD) sizeof(PRINTDLG);
   if (GetPrinterDeviceDefaults(&pd))
   {        // Lock memory handle.
      DEVMODE FAR* pDevMode = (DEVMODE FAR*)::GlobalLock(m_hDevMode);                 
      if (pDevMode)            
      {
            // Change printer settings in here.
            pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
           // Unlock memory handle.
         lpDevNames = (LPDEVNAMES)GlobalLock(pd.hDevNames);
         lpszDriverName = (LPTSTR )lpDevNames + lpDevNames->wDriverOffset;
         lpszDeviceName = (LPTSTR )lpDevNames + lpDevNames->wDeviceOffset;
         lpszPortName   = (LPTSTR )lpDevNames + lpDevNames->wOutputOffset;

         ::OpenPrinter(lpszDeviceName, &hPrinter, NULL);
         ::DocumentProperties(NULL,hPrinter,lpszDeviceName,pDevMode,
                           pDevMode, DM_IN_BUFFER|DM_OUT_BUFFER);

         // Sync the pDevMode.
         // See SDK help for DocumentProperties for more info.
         ::ClosePrinter(hPrinter);
         ::GlobalUnlock(m_hDevNames);
         ::GlobalUnlock(m_hDevMode);       
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void CNRDBApp::GetPrinterInfo(DEVMODE* pDevMode)
{
   PRINTDLG   pd;

 // Get default printer settings.    PRINTDLG   pd;
   pd.lStructSize = (DWORD) sizeof(PRINTDLG);
   if (GetPrinterDeviceDefaults(&pd))
   {        // Lock memory handle.
      DEVMODE* pDevMode1 = (DEVMODE FAR*)::GlobalLock(m_hDevMode);                 
      if (pDevMode1)            
      {
         *pDevMode = *pDevMode1;
            
         ::GlobalUnlock(m_hDevMode);       
      }
   }

}

///////////////////////////////////////////////////////////////////////////////
// App command to run the dialog
//

void CNRDBApp::OnAppAbout()
{
	CDlgAbout aboutDlg;
	aboutDlg.DoModal();
}

///////////////////////////////////////////////////////////////////////////////

void CNRDBApp::OnToolsSettings() 
{
   CSheetReportSettings dlg(IDS_REPORTSETTINGS);
   if (dlg.DoModal() == IDOK)
   {
	  m_BDMain = dlg.GetSettings();
   }
}

///////////////////////////////////////////////////////////////////////////////

BOOL CNRDBApp::ProcessMessageFilter(int code, LPMSG lpMsg)   
{
   if (m_hwndDialog != NULL)    
   {
      if (lpMsg->hwnd == m_hwndDialog ||
           ::IsChild(m_hwndDialog, lpMsg->hwnd))            
      {
         if (NULL != m_gpToolTip)
         {
             m_gpToolTip->RelayEvent(lpMsg);            
         }
      }
   }
   return CWinApp::ProcessMessageFilter(code, lpMsg);   
} 

///////////////////////////////////////////////////////////////////////////////
//
// Retrieve the default data source name, if a nrm file is specified on the
// command line
//

CString CNRDBApp::DefaultDataSource()
{
	if (m_lpCmdLine[0] != '\0' || m_sDefaultMap != "")
	{
      // Strip quotes

      CString sPath = m_lpCmdLine;
      if (sPath == "") sPath = m_sDefaultMap;

      if (sPath[0] == '"')
      {
         int i = sPath.Find('"', 1);
         sPath = sPath.Mid(1, i-1);
      }
		FILE* pFile = fopen(sPath,"r");
		if (pFile != NULL)
		{
			return BDNextStr(pFile);
			fclose(pFile);
		}
	}
	return "";
}

///////////////////////////////////////////////////////////////////////////////

void CNRDBApp::OnConvertShapefile() 
{
   CShapeFile shapefile;
   shapefile.Convert();
}


///////////////////////////////////////////////////////////////////////////////

void CNRDBApp::OnHelpNrdbhomepage() 
{
   m_sHtmlHelp = "http://www.nrdb.co.uk/?refer=nrdbpro2.3";

   CMultiDocTemplate* pTemplate = GetDocTemplate("HtmlHelp");      
   pTemplate->OpenDocumentFile(NULL);   			   
}

///////////////////////////////////////////////////////////////////////////////

void CNRDBApp::OnHelpRegister() 
{
	m_sHtmlHelp = "http://www.nrdb.co.uk/forum?action=registernew";

   CMultiDocTemplate* pTemplate = GetDocTemplate("HtmlHelp");      
   pTemplate->OpenDocumentFile(NULL);   			   
	
}


void CNRDBApp::OnHelpSupport() 
{	
	
}

void CNRDBApp::OnHelpTutorial() 
{	
}


///////////////////////////////////////////////////////////////////////////////
//
// Let user choose which file type to open
//

void CNRDBApp::OnFileNew() 
{

   CDlgNew dlg;
   int nID = dlg.DoModal();
   if (nID == IDC_MAPNEW)
   {
      BeginWaitCursor();
      CDocMap* pDoc;
      CMultiDocTemplate* pTemplate = GetDocTemplate("Map");      
      pDoc = (CDocMap*)pTemplate->OpenDocumentFile(NULL);   
      EndWaitCursor();  
   } 
   else if (nID == IDC_REPORTNEW) OnReportTable();
   else if (nID == IDC_SECTORALREPORTNEW) OnReportSummary();
   else if (nID == IDC_TSREPORTNEW) OnReportTSReport();
   else if (nID == IDC_HISTOGRAMNEW) OnReportGraph();
   else if (nID == IDC_TIMESERIESGRAPHNEW) OnReportTsgraph();
   else if (nID == IDC_PIECHARTNEW) OnReportPieChart();

}

///////////////////////////////////////////////////////////////////////////////
//
// Note uses RegOpenKey rather than CRegKey::Open as bypasses NT security for
// user (need additional parameters for RegOpenKeyEx)
//

CString CNRDBApp::GetAppPath()
{
   char sRegKey[128];
   long dw;

   // Construct the file name where the report wizard file will be saved

   HKEY hKey;
   if (RegOpenKey(HKEY_CLASSES_ROOT, "nrm_auto_file\\shell\\open\\command", &hKey) == ERROR_SUCCESS)
   {
      dw = sizeof(sRegKey);
           
      if (RegQueryValue(hKey, NULL, sRegKey, &dw) == ERROR_SUCCESS)
      {
         CString s = sRegKey;
         
         s = s.Mid(1,s.ReverseFind('\\'));
         return s;
      }
   }
   RegCloseKey(hKey);
   return "";
}

///////////////////////////////////////////////////////////////////////////////

void CNRDBApp::OnFileSetupFeatureTypes() 
{
	CDlgFType dlg;
	dlg.DoModal();	

   // Determine if feature types exist

   CFeatureType ftype;
   m_bFType = BDFeatureType(BDHandle(), &ftype, BDGETINIT);
   BDEnd(BDHandle());
}

///////////////////////////////////////////////////////////////////////////////

void CNRDBApp::OnFileSetupFeatures() 
{
   CDlgFeatures dlg;
   dlg.DoModal();	
}

/////////////////////////////////////////////////////////////////////
//
// Disable menu item if the database is still empty
//

void CNRDBApp::OnUpdateFTypesExist(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bFType);	
}

///////////////////////////////////////////////////////////////////////////////

void CNRDBApp::OnUpdateReportPrevious(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_nPreviousReport != -1);	
}

///////////////////////////////////////////////////////////////////////////////
//
// Retrieve the map layouts
//

void CNRDBApp::LoadLayout()
{
   // Convert xml object to scheme

   CXMLFile xmlfile;
   if (!xmlfile.Read(BDGetAppPath() + LAYOUTFILE) ||
       !m_aMapLayout.XMLAs(&xmlfile))
   {
      if (access(BDGetAppPath() + LAYOUTFILE, 00) == 0)
      {
         AfxMessageBox("Error reading: " + BDGetAppPath() + LAYOUTFILE);
         m_aMapLayout.RemoveAll();
      };
   }

   // Add default if empty
   if (m_aMapLayout.GetSize() == 0)
   {
      CMapLayout layout;
      layout.m_sName = BDString(IDS_DEFAULT);

      m_aMapLayout.Add(layout);
   };
}

///////////////////////////////////////////////////////////////////////////////

void CNRDBApp::SaveLayout()
{
   CXMLFile xmlfile;

   m_aMapLayout.AsXML(&xmlfile);
   xmlfile.Write(BDGetAppPath() + LAYOUTFILE);   
}

