// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// StatSimPro.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "daeconv.h"
#include "StatSimPro.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "StatSimProDoc.h"
#include "StatSimProView.h"
#include "LoginDlg.h"
#include "StatSimSvc.h"
#include "EditLevelDlg.h"

#include "ProgressDlg.h"
#include "ProcSelDlg.h"

#include "ConfigDlg.h"
#include "StatSimWnd.h"
#include "DataDefnDlg.h"

#include "Winver.h"
#include "Windows.h"

#ifndef STATSIMCONN
	#include "StatSimConn.h"
#endif

#ifndef DAEDATA
	#include "daedata.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//global variables
CStatSimConn* pGlobalConn = NULL;
CString userName, userPwd, sHost, sPort, sDB;
bool IS_ODBC_INT = false;
CStatSimRS* pelemRS;
CStatSimRS ***pdictRS;
daedict ***pDCT;

CString dbPrefix = L"StatSim";

LPCSTR** qnrtables;
int *ntypes, nqnr;
int *rtlen;
long *qnrIDs;
long *critqnrIDs;
LPCSTR** RTYPES;

long hpq_id;
BOOL _APP_, _PAP_;//if both 1, mixed

// temporary handler of portal vs conventional report
bool isNewReport = true;

BOOL g_askBasicInd, g_canproc;

// path to report
LPCTSTR g_sRptPath;


//views
CStatSimProView *g_pSSHTMLView;
//CStatSimProDoc g_SSPRoDoc;

//CMFCRibbonStatusBarPane *g_wndStatusBarPane;	//I have transferred this from main to global to be accessible
CProgressCtrl  m_ProgStatBar;	//I have transferred this from main to global to be accessible

//CMFCRibbonStatusBarPane *g_wndStatusBarPane;	//I have transferred this from main to global to be accessible
CMFCRibbonStatusBarPane  *g_wndStatusBarPane;//I have transferred this from main to global to be accessible

int* eArray;	//the selected processed elements or geolevel
int nE;			//number of elements
BOOL wHH;		//if there is hh in the elements

float USDtoPHP;	//exchange rate
BOOL IsDirectProc;	//whether processing is by batch or direct
BOOL IsProgress;	//whether the application is in a progress
int nProcBatches;	//number of processing batches
vector<BOOL> arrayProcExist;
vector<LPCSTR> arrayBatchDB;
CStringArray sGeoLevels;

// array of lists in SGE view
//CStatSimList* m_pListSGE;
//CStatSimTree* m_pTreeSGE;

// CStatSimProApp

BEGIN_MESSAGE_MAP(CStatSimProApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CStatSimProApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
	ON_COMMAND(ID_CALL_STATSIM4, &CStatSimProApp::OnCallStatsim4)
	ON_COMMAND(ID_TAB_NEW_RPT, &CStatSimProApp::OnTabNewRpt)
	ON_COMMAND(ID_TAB_NEW_PORTAL, &CStatSimProApp::OnTabNewPortal)
END_MESSAGE_MAP()


// CStatSimProApp construction

CStatSimProApp::CStatSimProApp()
{
	m_bHiColorIcons = TRUE;

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("StatSimPro.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CStatSimProApp object

CStatSimProApp theApp;


// CStatSimProApp initialization

void CStatSimProApp::OnFileNew()
{
	CWinApp::OnFileNew();

	//ayaw pa eh
	return;

	m_DocTplPos = theApp.GetFirstDocTemplatePosition();

	CDocTemplate* pDocTemp = theApp.GetNextDocTemplate(m_DocTplPos);

	m_DocPos = pDocTemp->GetFirstDocPosition();

	CStatSimProDoc* pDoc = (CStatSimProDoc*) pDocTemp->GetNextDoc(m_DocPos);

	//pDoc->OnNewDocument();
	//pDoc->SetTitle(_T("panget"));
	
}
BOOL CStatSimProApp::InitInstance()
{
//	#ifdef STATSIMSGE_VER_4
//	#define SSPATH _T("C:\\CBMSDatabase\\System\\StatSimSGE 4.0")
//#elif STATSIMPRO_VER_5
//	#define SSPATH _T("C:\\CBMSDatabase\\System\\StatSimPro 5.0")
//#else
//	#define SSPATH _T("C:\\CBMSDatabase\\System\\StatSim 3.0")
//#endif

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	EnableTaskbarInteraction();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options

	//kill running mysqls
	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, 
		FALSE, FindProcessId(_T("mysqld-nt.exe")));
	TerminateProcess(processHandle, 0);
	CloseHandle(processHandle);	
	//kill service
	//KillService(_T("StatSimPro-MySQL"));
	//uninstall service
	//UnInstall(_T("StatSimPro-MySQL"));
	
	//install service
	//TCHAR *pLogFile = L"C:\\CBMSDatabase\\System\\StatSimPro 5.0\\StatSimSvc.log";

	TCHAR *sSvcName = L"StatSimPro-MySQL";
	//TCHAR *sSvcName = L"MySQL56";
	CString sMySQLini = _T("\"") + (CString) SSPATH + (CString) _T("\\MySQL\\bin\\mysqld.exe\" --defaults-file=\"") + (CString) SSPATH + _T("\\MySQL\\statsimpro.ini\" StatSimPro-MySQL");
	
	//AfxMessageBox(sMySQLini);
	Install(sMySQLini.GetBuffer(), sSvcName, pLogFile);
	//Install(_T("\"C:\\CBMSDatabase\\System\\StatSimPro 5.0\\MySQL 5.5\\bin\\mysqld.exe\" --defaults-file=\"C:\\CBMSDatabase\\System\\StatSimPro 5.0\\MySQL 5.5\\statsimpro.ini\" MySQL56"), sSvcName, pLogFile);
	//run service
	RunService(sSvcName);

	CLoginDlg* pLoginDlg = new CLoginDlg();

	if (pLoginDlg->DoModal() != IDOK) {
		return FALSE;
	}

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_StatSimProTYPE,
		RUNTIME_CLASS(CStatSimProDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CStatSimProView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);


	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	delete pLoginDlg;

	return TRUE;
}


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CEdit m_EditAbt;
	virtual BOOL OnInitDialog();

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ABT, m_EditAbt);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	//LPCSTR sAbt = _T("asa");
	//encountered improper argument;
	//m_EditAbt.SetWindowText(sAbt);
	m_EditAbt.EnableWindow(FALSE);
	return TRUE;
}
// App command to run the dialog
void CStatSimProApp::OnAppAbout()
{	
	theApp.OnFileNew();

	CString sHtmlTag;
	sHtmlTag = _T("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-1\"> \n\
		<title>Welcome StatSimPro 5</title>\n\
		</head>\n\
		<body>\n\
		<hr />\n\
		<p><font face=\"Calibri\"><b><font size=\"5\" color=\"#990000\">CBMS StatSimPro 6.0<br>\n\
		</font><font size=\"4\" color=\"#000080\">\n\
		<img border=\"0\" src=\".\\images\\logo.jpg\" align=\"left\" width=\"86\" height=\"110\">\n\
		</font></b></font></p>\n\
		<p><font face=\"Calibri\"><b><font size=\"4\" color=\"#000080\">The PEP-Asia CBMS Network <br>\n\
		</font></b><i>The CBMS StatSim software was developed by the CBMS Network with the aid of a grant from the International Development Research Centre (IDRC), Ottawa, Canada and Canadian International Development Agency (CIDA).<br>\n\
		<br>\n\
		The CBMS StatSim is owned by the CBMS Network. Its use and modification is subject to a contract and/or memorandum of agreement with the Network.\n\
		<br /> <br>\n\
		<hr />\n\
		Contact Information:<br>\n\
		</i><br><b>Dr. Celia M. Reyes</b><br>\n\
		<i>CBMS Network Leader</i><br>\n\
		CBMS Network Office<br>\n\
		10th Floor Angelo King International Center<br>\n\
		Estrada Corner Arellano Streets<br>\n\
		Malate, Manila<br>\n\
		Philippines<br>\n\
		Contact No: 5262067 or 5238888 loc. 274 <br>\n\
		<a href='mailto:celmreyes@yahoo.com'>celmreyes@yahoo.com</a><br>\n\
		<a href='mailto:pepcbmsphil@gmail.com'>pepcbmsphil@gmail.com</a><br>\n\
		<a href='mailto:cbms.network@gmail.com'>cbms.network@gmail.com</a><br>\n\
		<a href='http://www.pep-net.org'>Visit the PEP Website</a>\n\
		<br>&nbsp;</font></p>\n\
		<hr />\n\
		<i><font face='Calibri'>*The original design and program of the CBMS-StatsimPro 6.0 was created and developed for the CBMS Network by Mr. Joel Bancolita\n\
		</i></font><br /> <br>\n\
		</i><br>&nbsp;</font></p>\n\
		</body></html>");

	CreateDirectory(_T("C:\\CBMSDatabase\\System\\Reports"), NULL);
	CreateDirectory(_T("C:\\CBMSDatabase\\System\\Reports\\Temp"), NULL);

	g_sRptPath = _T("C:\\CBMSDatabase\\System\\Reports\\Temp\\StatSimRpt.htm");
	FILE * pFile = _tfopen(g_sRptPath, _T("w"));

	_ftprintf(pFile, (sHtmlTag) );
	fclose(pFile);

	g_pSSHTMLView->Navigate2((CString) g_sRptPath, NULL, NULL);


	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CStatSimProApp customization load/save methods

void CStatSimProApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CStatSimProApp::LoadCustomState()
{
}

void CStatSimProApp::SaveCustomState()
{
}

// CStatSimProApp message handlers

CString GetVersionInfo(HMODULE hLib, CString csEntry)
{

	CString csRet;

  if (hLib == NULL)
    hLib = AfxGetResourceHandle();
  
  HRSRC hVersion = FindResource( hLib, 
    MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION );
  if (hVersion != NULL)
  {
    HGLOBAL hGlobal = LoadResource( hLib, hVersion ); 
    if ( hGlobal != NULL)  
    {  
  
      LPVOID versionInfo  = LockResource(hGlobal);  
      if (versionInfo != NULL)
      {
        DWORD vLen,langD;
        BOOL retVal;    
    
        LPVOID retbuf=NULL;
    
        static TCHAR fileEntry[256];

        _stprintf(fileEntry, _T("\\VarFileInfo\\Translation"));
        retVal = VerQueryValue(versionInfo,fileEntry,&retbuf,(UINT *)&vLen);
        if (retVal && vLen==4) 
        {
          memcpy(&langD,retbuf,4);            
          _stprintf(fileEntry, _T("\\StringFileInfo\\%02X%02X%02X%02X\\%s"),
                  (langD & 0xff00)>>8,langD & 0xff,(langD & 0xff000000)>>24, 
                  (langD & 0xff0000)>>16, csEntry);            
        }
        else 
          _stprintf(fileEntry, _T("\\StringFileInfo\\%04X04B0\\%s"), 
            GetUserDefaultLangID(), csEntry);

        if (VerQueryValue(versionInfo,fileEntry,&retbuf,(UINT *)&vLen)) 
          csRet = (char*)retbuf;
      }
    }

    UnlockResource( hGlobal );  
    FreeResource( hGlobal );  
  }

  return csRet;
}

void MakeUnionQuery()
{
	
	CStatSimRS *pRS = NULL, *pSubRS = NULL;
	FILE *pUnionFile, *pOutFile, *pDelFile;
	
	int i, j, k, nTables;

	nTables = 127;

	CString sSQL, 
		sProvince, sMunicipality, sBarangay, sProv, sMun, sBrgy, 
		sThisDB, sFile, sUnionSQLInit, sAllUnionSQLInit, sUnionSQL, sAllUnionSQL;
	CStringArray sTables;
	sTables.SetSize(nTables);


/*	sTables[0] = "brgy_brgy_migr";
	sTables[1] = "brgy_coreind";
	sTables[2] = "brgy_country_migr";
	sTables[3] = "brgy_crimetype";
	sTables[4] = "brgy_death05";
	sTables[5] = "brgy_deathpreg";
	sTables[6] = "brgy_demog";
	sTables[7] = "brgy_electvote";
	sTables[8] = "brgy_empl15ab";
	sTables[9] = "brgy_fshort";
	sTables[10] = "brgy_g_occ";
	sTables[11] = "brgy_hh_prog_pciquintile";
	sTables[12] = "brgy_hhcoreind!fshort";
	sTables[13] = "brgy_hhcoreind!msh";
	sTables[14] = "brgy_hhcoreind!ntstf";
	sTables[15] = "brgy_hhcoreind!ntsws";
	sTables[16] = "brgy_hhcoreind!pciquintile";
	sTables[17] = "brgy_hhcoreind!povp";
	sTables[18] = "brgy_hhcoreind!squat";
	sTables[19] = "brgy_hhcoreind!subp";
	sTables[20] = "brgy_hhcoreind!wdeath05";
	sTables[21] = "brgy_hhcoreind!wdeathpreg";
	sTables[22] = "brgy_hhcoreind!wmaln05";
	sTables[23] = "brgy_hhcoreind!wntelem612";
	sTables[24] = "brgy_hhcoreind!wnths1316";
	sTables[25] = "brgy_hhcoreind!wunempl15ab";
	sTables[26] = "brgy_hhcoreind!wvictcr";
	sTables[27] = "brgy_hhwdeath05";
	sTables[28] = "brgy_hhwdeathpreg";
	sTables[29] = "brgy_hhwlabfor";
	sTables[30] = "brgy_hhwmaln05";
	sTables[31] = "brgy_hhwmem01d";
	sTables[32] = "brgy_hhwmem05";
	sTables[33] = "brgy_hhwmem05d";
	sTables[34] = "brgy_hhwmem10ab";
	sTables[35] = "brgy_hhwmem1316";
	sTables[36] = "brgy_hhwmem612";
	sTables[37] = "brgy_hhwmem616";
	sTables[38] = "brgy_hhwntelem612";
	sTables[39] = "brgy_hhwnths1316";
	sTables[40] = "brgy_hhwntliter10ab";
	sTables[41] = "brgy_hhwntsch616";
	sTables[42] = "brgy_hhwunempl15ab";
	sTables[43] = "brgy_hhwvictcr";
	sTables[44] = "brgy_jstatus";
	sTables[45] = "brgy_maln05";
	sTables[46] = "brgy_mdeady";
	sTables[47] = "brgy_mem_prog_pciquintile";
	sTables[48] = "brgy_memcoreind!fshort";
	sTables[49] = "brgy_memcoreind!msh";
	sTables[50] = "brgy_memcoreind!ntstf";
	sTables[51] = "brgy_memcoreind!ntsws";
	sTables[52] = "brgy_memcoreind!povp";
	sTables[53] = "brgy_memcoreind!squat";
	sTables[54] = "brgy_memcoreind!subp";
	sTables[55] = "brgy_memcoreind!wdeath05";
	sTables[56] = "brgy_memcoreind!wdeathpreg";
	sTables[57] = "brgy_memcoreind!wmaln05";
	sTables[58] = "brgy_memcoreind!wntelem612";
	sTables[59] = "brgy_memcoreind!wnths1316";
	sTables[60] = "brgy_memcoreind!wunempl15ab";
	sTables[61] = "brgy_memcoreind!wvictcr";
	sTables[62] = "brgy_memfshort";
	sTables[63] = "brgy_memmsh";
	sTables[64] = "brgy_memntstf";
	sTables[65] = "brgy_memntsws";
	sTables[66] = "brgy_mempovp";
	sTables[67] = "brgy_memsquat";
	sTables[68] = "brgy_memsubp";
	sTables[69] = "brgy_migr";
	sTables[70] = "brgy_mnutind";
	sTables[71] = "brgy_msh";
	sTables[72] = "brgy_mun_migr";
	sTables[73] = "brgy_ntelem612";
	sTables[74] = "brgy_nths1316";
	sTables[75] = "brgy_ntliter10ab";
	sTables[76] = "brgy_ntsch616";
	sTables[77] = "brgy_ntstf";
	sTables[78] = "brgy_ntsws";
	sTables[79] = "brgy_org_type_1";
	sTables[80] = "brgy_pciquintile";
	sTables[81] = "brgy_povp";
	sTables[82] = "brgy_progtype";
	sTables[83] = "brgy_prov_migr";
	sTables[84] = "brgy_regvote";
	sTables[85] = "brgy_roof";
	sTables[86] = "brgy_cci";
	sTables[87] = "brgy_sector";
	sTables[88] = "brgy_squat";
	sTables[89] = "brgy_subp";
	sTables[90] = "brgy_tenur";
	sTables[91] = "brgy_toil";
	sTables[92] = "brgy_totpop";
	sTables[93] = "brgy_totwage";
	sTables[94] = "brgy_unempl15ab";
	sTables[95] = "brgy_victcr";
	sTables[96] = "brgy_wage";
	sTables[97] = "brgy_wall";
	sTables[98] = "brgy_water";
	sTables[99] = "brgy_wnojob";
	sTables[100] = "brgy_workcl";
	sTables[101] = "brgy_ynotlookjob";
	sTables[102] = "hh";
	sTables[103] = "hh_coreind";
	sTables[104] = "hh_ind";
	sTables[105] = "hh_pciile_brgy";
	sTables[106] = "hh_pciile_purok";
	sTables[107] = "hh_prog_pciquintile";
	sTables[108] = "hh_cci";
	sTables[109] = "hh_totinile_brgy";
	sTables[110] = "hh_totinile_purok";
	sTables[111] = "hh_totpop";
	sTables[112] = "hpq_agriequip";
	sTables[113] = "hpq_aquarescatch";
	sTables[114] = "hpq_aquarestend";
	sTables[115] = "hpq_aquarestenddist";
	sTables[116] = "hpq_asset";
	sTables[117] = "hpq_boardpass";
	sTables[118] = "hpq_business";
	sTables[119] = "hpq_couple";
	sTables[120] = "hpq_crime";
	sTables[121] = "hpq_crop";
	sTables[122] = "hpq_cropdist";
	sTables[123] = "hpq_death";
	sTables[124] = "hpq_expmem";
	sTables[125] = "hpq_handicap";
	sTables[126] = "hpq_hh";
	sTables[127] = "hpq_landparcel";
	sTables[128] = "hpq_lstock";
	sTables[129] = "hpq_mem";
	sTables[130] = "hpq_ofw";
	sTables[131] = "hpq_prog";
	sTables[132] = "hpq_seniorcit";
	sTables[133] = "hpq_unipar";
	sTables[134] = "hpq_wage";
	sTables[135] = "mem";
	sTables[136] = "mem_ind";
	sTables[137] = "mem_prog_pciquintile";
	sTables[138] = "purok";
	sTables[139] = "purok_brgy_migr";
	sTables[140] = "purok_coreind";
	sTables[141] = "purok_country_migr";
	sTables[142] = "purok_death05";
	sTables[143] = "purok_deathpreg";
	sTables[144] = "purok_demog";
	sTables[145] = "purok_electvote";
	sTables[146] = "purok_empl15ab";
	sTables[147] = "purok_fshort";
	sTables[148] = "purok_hhwdeath05";
	sTables[149] = "purok_hhwdeathpreg";
	sTables[150] = "purok_hhwlabfor";
	sTables[151] = "purok_hhwmaln05";
	sTables[152] = "purok_hhwmem01d";
	sTables[153] = "purok_hhwmem05";
	sTables[154] = "purok_hhwmem05d";
	sTables[155] = "purok_hhwmem10ab";
	sTables[156] = "purok_hhwmem1316";
	sTables[157] = "purok_hhwmem612";
	sTables[158] = "purok_hhwmem616";
	sTables[159] = "purok_hhwntelem612";
	sTables[160] = "purok_hhwnths1316";
	sTables[161] = "purok_hhwntliter10ab";
	sTables[162] = "purok_hhwntsch616";
	sTables[163] = "purok_hhwunempl15ab";
	sTables[164] = "purok_hhwvictcr";
	sTables[165] = "purok_maln05";
	sTables[166] = "purok_memfshort";
	sTables[167] = "purok_memmsh";
	sTables[168] = "purok_memntstf";
	sTables[169] = "purok_memntsws";
	sTables[170] = "purok_mempovp";
	sTables[171] = "purok_memsquat";
	sTables[172] = "purok_memsubp";
	sTables[173] = "purok_migr";
	sTables[174] = "purok_msh";
	sTables[175] = "purok_mun_migr";
	sTables[176] = "purok_ntelem612";
	sTables[177] = "purok_nths1316";
	sTables[178] = "purok_ntliter10ab";
	sTables[179] = "purok_ntsch616";
	sTables[180] = "purok_ntstf";
	sTables[181] = "purok_ntsws";
	sTables[182] = "purok_povp";
	sTables[183] = "purok_prov_migr";
	sTables[184] = "purok_regvote";
	sTables[185] = "purok_cci";
	sTables[186] = "purok_squat";
	sTables[187] = "purok_subp";
	sTables[188] = "purok_totpop";
	sTables[189] = "purok_totwage";
	sTables[190] = "purok_unempl15ab";
	sTables[191] = "purok_victcr";
	sTables[192] = "purok_wage";
	sTables[193] = "vote_ind";
	sTables[194] = "wage_ind";
*/
	sTables[0] = "brgy_brgy_migr";
	sTables[1] = "brgy_coreind";
	sTables[2] = "brgy_country_migr";
	sTables[3] = "brgy_death05";
	sTables[4] = "brgy_deathpreg";
	sTables[5] = "brgy_demog";
	sTables[6] = "brgy_empl15ab";
	sTables[7] = "brgy_fshort";
	sTables[8] = "brgy_g_occ";
	sTables[9] = "brgy_hhcoreind!pciquintile";
	sTables[10] = "brgy_hhwdeath05";
	sTables[11] = "brgy_hhwdeathpreg";
	sTables[12] = "brgy_hhwlabfor";
	sTables[13] = "brgy_hhwmaln05";
	sTables[14] = "brgy_hhwmem01d";
	sTables[15] = "brgy_hhwmem05";
	sTables[16] = "brgy_hhwmem05d";
	sTables[17] = "brgy_hhwmem10ab";
	sTables[18] = "brgy_hhwmem1316";
	sTables[19] = "brgy_hhwmem612";
	sTables[20] = "brgy_hhwmem616";
	sTables[21] = "brgy_hhwntelem612";
	sTables[22] = "brgy_hhwnths1316";
	sTables[23] = "brgy_hhwntliter10ab";
	sTables[24] = "brgy_hhwntsch616";
	sTables[25] = "brgy_hhwunempl15ab";
	sTables[26] = "brgy_hhwvictcr";
	sTables[27] = "brgy_jstatus";
	sTables[28] = "brgy_maln05";
	sTables[29] = "brgy_memfshort";
	sTables[30] = "brgy_memmsh";
	sTables[31] = "brgy_memntstf";
	sTables[32] = "brgy_memntsws";
	sTables[33] = "brgy_mempovp";
	sTables[34] = "brgy_memsquat";
	sTables[35] = "brgy_memsubp";
	sTables[36] = "brgy_migr";
	sTables[37] = "brgy_mnutind";
	sTables[38] = "brgy_msh";
	sTables[39] = "brgy_mun_migr";
	sTables[40] = "brgy_ntelem612";
	sTables[41] = "brgy_nths1316";
	sTables[42] = "brgy_ntliter10ab";
	sTables[43] = "brgy_ntsch616";
	sTables[44] = "brgy_ntstf";
	sTables[45] = "brgy_ntsws";
	sTables[46] = "brgy_pciquintile";
	sTables[47] = "brgy_povp";
	sTables[48] = "brgy_prov_migr";
	sTables[49] = "brgy_roof";
	sTables[50] = "brgy_cci";
	sTables[51] = "brgy_sector";
	sTables[52] = "brgy_squat";
	sTables[53] = "brgy_subp";
	sTables[54] = "brgy_tenur";
	sTables[55] = "brgy_toil";
	sTables[56] = "brgy_totpop";
	sTables[57] = "brgy_unempl15ab";
	sTables[58] = "brgy_victcr";
	sTables[59] = "brgy_wall";
	sTables[60] = "brgy_water";
	sTables[61] = "brgy_wnojob";
	sTables[62] = "brgy_ynojob";
	sTables[63] = "hh";
	sTables[64] = "hh_coreind";
	sTables[65] = "hh_ind";
	sTables[66] = "hh_pciile_brgy";
	sTables[67] = "hh_pciile_purok";
	sTables[68] = "hpq_hh";
	sTables[69] = "hh_cci";
	sTables[70] = "hh_totinile_brgy";
	sTables[71] = "hh_totinile_purok";
	sTables[72] = "hh_totpop";
	sTables[73] = "mem";
	sTables[74] = "mem_ind";
	sTables[75] = "hpq_mem";
	sTables[76] = "purok";
	sTables[77] = "purok_brgy_migr";
	sTables[78] = "purok_coreind";
	sTables[79] = "purok_country_migr";
	sTables[80] = "purok_death05";
	sTables[81] = "purok_deathpreg";
	sTables[82] = "purok_demog";
	sTables[83] = "purok_empl15ab";
	sTables[84] = "purok_fshort";
	sTables[85] = "purok_hhwdeath05";
	sTables[86] = "purok_hhwdeathpreg";
	sTables[87] = "purok_hhwlabfor";
	sTables[88] = "purok_hhwmaln05";
	sTables[89] = "purok_hhwmem01d";
	sTables[90] = "purok_hhwmem05";
	sTables[91] = "purok_hhwmem05d";
	sTables[92] = "purok_hhwmem10ab";
	sTables[93] = "purok_hhwmem1316";
	sTables[94] = "purok_hhwmem612";
	sTables[95] = "purok_hhwmem616";
	sTables[96] = "purok_hhwntelem612";
	sTables[97] = "purok_hhwnths1316";
	sTables[98] = "purok_hhwntliter10ab";
	sTables[99] = "purok_hhwntsch616";
	sTables[100] = "purok_hhwunempl15ab";
	sTables[101] = "purok_hhwvictcr";
	sTables[102] = "purok_maln05";
	sTables[103] = "purok_memfshort";
	sTables[104] = "purok_memmsh";
	sTables[105] = "purok_memntstf";
	sTables[106] = "purok_memntsws";
	sTables[107] = "purok_mempovp";
	sTables[108] = "purok_memsquat";
	sTables[109] = "purok_memsubp";
	sTables[110] = "purok_migr";
	sTables[111] = "purok_msh";
	sTables[112] = "purok_mun_migr";
	sTables[113] = "purok_ntelem612";
	sTables[114] = "purok_nths1316";
	sTables[115] = "purok_ntliter10ab";
	sTables[116] = "purok_ntsch616";
	sTables[117] = "purok_ntstf";
	sTables[118] = "purok_ntsws";
	sTables[119] = "purok_povp";
	sTables[120] = "purok_prov_migr";
	sTables[121] = "purok_cci";
	sTables[122] = "purok_squat";
	sTables[123] = "purok_subp";
	sTables[124] = "purok_totpop";
	sTables[125] = "purok_unempl15ab";
	sTables[126] = "purok_victcr";

	sSQL = "SELECT * FROM `prov`;";
	pRS = new CStatSimRS(pGlobalConn, sSQL);
	pRS->MoveFirst();

	//provincial statsim
	for (i=0; i<pRS->GetRecordCount(); i++) {
		sProvince = pRS->SQLFldValue(_MBCS("province"));
		sProv = pRS->SQLFldValue(_MBCS("prov"));
		
		sProvince.Replace(_T("."), _T(""));
		sProvince.Replace(_T(" "), _T(""));
		sProvince.Replace(_T(","), _T(""));
		sProvince.Replace(_T("ñ"), _T("n"));

		sFile = _T("c:\\unions_") + sProvince + _T(".sql");	
		pUnionFile = _tfopen((sFile), _T("w"));

		sFile = _T("c:\\outfile_") + sProvince + _T(".sql");	
		pOutFile = _tfopen((sFile), _T("w"));
		
		sFile = _T("c:\\delete_") + sProvince + _T(".sql");	
		pDelFile = _tfopen((sFile), _T("w"));

		sSQL.Format(_T("SELECT * from `mun` where prov=%s;"), sProv);
		pSubRS = new CStatSimRS(pGlobalConn, sSQL);

		for (k=0; k<nTables; k++) {
			sUnionSQLInit.Format(_T("INSERT IGNORE INTO `StatSimConn_%s.`%s` \n"), sProvince, sTables[k]);
			_ftprintf(pUnionFile, (sUnionSQLInit));

			pSubRS->MoveFirst();
		
			for (j=0; j<pSubRS->GetRecordCount(); j++) {
				sMunicipality = pSubRS->SQLFldValue(_MBCS("municipality"));
				sMun = pSubRS->SQLFldValue(_MBCS("mun"));
				sThisDB = _T("StatSimConn_") + sProvince + _T("_") + sMunicipality;
				//sThisDB = _T("StatSimConn_") + sMunicipality;
				
				sThisDB.Replace(_T("."), _T(""));
				sThisDB.Replace(_T(" "), _T(""));
				sThisDB.Replace(_T(","), _T(""));
				sThisDB.Replace(_T("ñ"), _T("n"));


				if (j==0) {
					sUnionSQL.Format(_T("\tSELECT * FROM `%s`.`%s`\n"), sThisDB, sTables[k]);
				}
				else {
					sUnionSQL.Format(_T("\tUNION\n\tSELECT * FROM `%s`.`%s` \n"), sThisDB, sTables[k]);
				}
				
				CFileFind FileFinder;
				CString currPath = (CString) SSPATH + (CString) _T("\\MySQL 5.5\\data\\") + sThisDB;
				
				if ( FileFinder.FindFile((currPath)) ) {
					_ftprintf(pUnionFile, (sUnionSQL));
				}


				sSQL.Format(_T("SELECT * INTO OUTFILE 'c:/%s.xls' FIELDS TERMINATED BY '\\t' FROM %s; \n"), sTables[k], sTables[k]);
				_ftprintf(pOutFile, (sSQL));
				
				pSubRS->MoveNext();

			}
			_ftprintf(pUnionFile, _T(";\n\n"));

		}


		fclose(pUnionFile);
		fclose(pOutFile);
		fclose(pDelFile);
		pRS->MoveNext();
	}
	
	delete pRS; pRS = NULL;
	sSQL = "SELECT `prov`.`prov`, `Province`, `mun`.`mun`, `Municipality` FROM `mun` INNER JOIN `prov` ON (`prov`.`prov`=`mun`.`prov`);";
	pRS = new CStatSimRS(pGlobalConn, sSQL);
	pRS->MoveFirst();

	//municipal statsim
	for (i=0; i<pRS->GetRecordCount(); i++) {
		sProvince = pRS->SQLFldValue(_MBCS("province"));
		sMunicipality = pRS->SQLFldValue(_MBCS("municipality"));
		sMun = pRS->SQLFldValue(_MBCS("mun"));
		
		sProvince.Replace(_T("."), _T(""));
		sProvince.Replace(_T(" "), _T(""));
		sProvince.Replace(_T(","), _T(""));
		sProvince.Replace(_T("ñ"), _T("n"));
		sMunicipality.Replace(_T("."), _T(""));
		sMunicipality.Replace(_T(" "), _T(""));
		sMunicipality.Replace(_T(","), _T(""));
		sMunicipality.Replace(_T("ñ"), _T("n"));

		sFile = _T("c:\\unions_") + sProvince + _T("_") + sMunicipality + _T(".sql");	
		pUnionFile = _tfopen((sFile), _T("w"));

		sFile = _T("c:\\outfile_") + sProvince + _T("_") + sMunicipality + _T(".sql");	
		pOutFile = _tfopen((sFile), _T("w"));
		
		sFile = _T("c:\\delete_") + sProvince + _T("_") + sMunicipality + _T(".sql");	
		pDelFile = _tfopen((sFile), _T("w"));

		sSQL.Format(_T("SELECT * from brgy where mun=%s;"), sMun);
		pSubRS = new CStatSimRS(pGlobalConn, sSQL);		


		for (k=0; k<nTables; k++) {
			sUnionSQLInit.Format(_T("INSERT IGNORE INTO `StatSimConn_%s_%s`.`%s` \n"), sProvince, sMunicipality, sTables[k]);
			_ftprintf(pUnionFile, (sUnionSQLInit));

			pSubRS->MoveFirst();
		
			for (j=0; j<pSubRS->GetRecordCount(); j++) {
				sBarangay = pSubRS->SQLFldValue(_MBCS("barangay"));
				sBrgy = pSubRS->SQLFldValue(_MBCS("brgy"));
				//sThisDB = _T("StatSimConn_") + sProvince + _T("_") + sMunicipality + _T("_") + sBarangay;
				sThisDB = _T("StatSimConn_") + sBarangay;
				
				sThisDB.Replace(_T("."), _T(""));
				sThisDB.Replace(_T(" "), _T(""));
				sThisDB.Replace(_T(","), _T(""));
				sThisDB.Replace(_T("ñ"), _T("n"));


				if (j==pSubRS->GetRecordCount()-1) {
					sUnionSQL.Format(_T("\tSELECT * FROM `%s`.`%s` ;\n\n"), sThisDB, sTables[k]);
				}
				else {
					sUnionSQL.Format(_T("\tSELECT * FROM `%s`.`%s` UNION \n"), sThisDB, sTables[k]);
				}
				
				CFileFind FileFinder;
				CString currPath = (CString) SSPATH + (CString) _T("\\MySQL 5.5\\data\\") + sThisDB;
				
				if ( FileFinder.FindFile((currPath)) ) {
					_ftprintf(pUnionFile, (sUnionSQL));
				}


				sSQL.Format(_T("SELECT * INTO OUTFILE 'c:/%s.xls' FIELDS TERMINATED BY '\\t' FROM `%s`; \n"), sTables[k], sTables[k]);
				_ftprintf(pOutFile, (sSQL));
				
				pSubRS->MoveNext();

			}
		}


		fclose(pUnionFile);
		fclose(pOutFile);
		fclose(pDelFile);
		pRS->MoveNext();
	}

	sFile = _T("c:\\outfile.sql");	
	pOutFile = _tfopen((sFile), _T("w"));

	for (k=0; k<nTables; k++) {
		
		sSQL.Format(_T("SELECT * INTO OUTFILE 'c:/%s.xls' FIELDS TERMINATED BY '\\t' FROM `%s`; \n"), sTables[k], sTables[k]);

		_ftprintf(pOutFile, (sSQL));

	}
	fclose(pOutFile);



	sFile = _T("c:\\delete.sql");	
	pDelFile = _tfopen((sFile), _T("w"));

	for (k=0; k<nTables; k++) {
		
		sSQL.Format(_T("delete FROM `%s`; \n"), sTables[k]);

		_ftprintf(pDelFile, (sSQL));

	}
	fclose(pDelFile);

}

CString CopyVal(CComboBox* pCB, CEdit* pEdit)
{
	CString sVal;
	int iSel = 0, editLen, getLR;
	
	pEdit->Clear();
	pEdit->SetWindowText(_T(""));
	
	if ( pCB->SetEditSel(0, -1) ) {
		
		if ((pCB->GetEditSel())!=NULL) {
			pCB->Copy();
			pEdit->Paste();
			editLen = pEdit->LineLength(1);
			getLR = pEdit->GetLine(0, sVal.GetBuffer(editLen), editLen);
			sVal.ReleaseBuffer(editLen);
			
			pEdit->Clear();
			pEdit->SetWindowText(_T(""));
		}
	}
	
	else {
		sVal = _T("");
	}

	return sVal;

}

int GetNewIDVal(CString sTable, CString IDFld)
{

	CStatSimRS* pRS;
	CString sSQL;
	int ID = 0;

	sSQL = _T("SELECT max(`") + IDFld + _T("`) AS `maxID` FROM `") + sTable + _T("`;");

	pRS = new CStatSimRS( pGlobalConn, sSQL);

	if (pRS->GetRecordCount() > 0) {
		pRS->MoveFirst();
		LPCSTR sID = ConstChar(pRS->SQLFldValue(_MBCS("maxID")));
		ID = atoi(sID);
		
		if (ID < 0)
			ID = 0;

		ID = ID + 1;	
	}

	delete pRS; pRS = 0;

	return ID;
}
void ImportData(CString strPath, long ID)
{

	ImportData(strPath, pGlobalConn, ID);

	delete pGlobalConn; pGlobalConn=0;	//redirect global database
	// odbc - FALSE
	pGlobalConn = new CStatSimConn( userName, userPwd, sHost, sPort, ConstChar(sDB), ODBC );


}

//return folder containing the exe file
CString GetExeFilePath()
{
	TCHAR result[ MAX_PATH ];
	CString sFilePath( result, GetModuleFileName( NULL, result, MAX_PATH ) );
	return sFilePath.Left(sFilePath.ReverseFind('\\'));
}

void ImportData(CString strPath, CStatSimConn* pToDB, long ID)
{

	try
	{
		//CString sStatusText;
		//sStatusText.Format(_T("Importing "), strPath);
		//g_wndStatusBarPane->SetText(sStatusText);

		int idx;
		for (int i=0; i<nqnr; i++) {
			if (qnrIDs[i]==ID)
				idx=i;
		}

		CProgressDlg *pDlg = 0;
		daedata* pDta = NULL;
		//daestream *pSSS = NULL;
		daestream *pSSS = NULL;
		
		// set element
		CString sSQL; sSQL.Format(_T("SELECT `base_element` FROM `~qnr` WHERE `qnrID`=%d"), ID);
		CStatSimRS *pRS = new CStatSimRS(pGlobalConn, sSQL);
		if (pRS->GetRecordCount()>0)
			pRS->MoveFirst();
		int nElt = _ttoi( ( pRS->SQLFldValue( (USHORT) 0 ) ) );

		CStatSimElt lElt(pToDB, nElt);
		CString strIDX = lElt.IdxClause();

		//pSSS = new CStatSimStream(ConstChar(strPath), RTYPES[idx], ntypes[idx], rtlen[idx], NULL, NULL, FALSE);
		
		//pSSS = new daestream(ConstChar(strPath), RTYPES[idx], ntypes[idx], rtlen[idx], FALSE);

		LPCSTR*** pDctArray = new LPCSTR**[ntypes[idx]];
		UINT* nDctVars = new UINT[ntypes[idx]];

		for (int j=0; j<ntypes[idx]; j++) {
			pDctArray[j] = pDCT[idx][j]->GetArray();
			nDctVars[j] = pDCT[idx][j]->nVars();
		}

		//pDlg = new CProgressDlg();
		//CStatSimWnd* hLabel = new CStatSimWnd(&pDlg->m_ProgressLabel);
		//CStatSimBar* pProg = new CStatSimBar(&pDlg->m_ProgressBar);

		//SYSTEMTIME systime0, systime1;
		//GetSystemTime(&systime0);
		//pSSS->MakeArray(pDctArray, nDctVars);
		//pSSS->MakeArray(pDctArray, nDctVars, NULL, _T(""));
		//GetSystemTime(&systime1);
		//float sec0 = (float)systime0.wSecond + (float)( (float)systime0.wMilliseconds / 1000 );
		//float sec1 = (float)systime1.wSecond + (float)( (float)systime1.wMilliseconds / 1000 );
		//float elapsedtime = sec1-sec0;
		//CString msg; msg.Format(L"%f", elapsedtime);
		//AfxMessageBox(_T("please"));


		//test////////////////////////////////
		_tochar sFile(strPath);
		pToDB->InsertRecords(sFile, pDCT[idx], qnrtables[idx], rtlen[idx], RTYPES[idx], ntypes[idx], 
			NULL, NULL, 1, strIDX);
		//////////////////////////////////////

		//pDta = new daedata((pSSS->GetArray()), pSSS->TypeCountArr(), pSSS->TotLines(),
		//	pDCT[idx], RTYPES[idx], ntypes[idx]);

		//varpvec3d myvec = pSSS->GetArray();
		
/*		for (int k=0; (*myvec).size(); k++){
			CString filename;
			filename.Format(_T("d:\\stream_%d.dat"), k);
			FILE *pFile = _wfopen(filename, L"w");
			for (int i=0; i<(*myvec)[k].size(); i++) {
				for (int j=0; j<(*myvec)[k][i].size(); j++) {
					fwprintf(pFile, (*myvec)[k][i][j]);
					fwprintf(pFile, L"\t");
				}
				fwprintf(pFile, L"\n");
			}
			fclose(pFile);
		}
*/		
		//pToDB->InsertRecords(pDta, qnrtables[idx], RTYPES[idx], ntypes[idx], 
		//	NULL, NULL, 1, strIDX);
		
		delete pDlg; pDlg = NULL;	

		//di ma delete!!!
		//delete pSSS; pSSS = NULL;	//delete pDta; pDta = NULL; //this should be taken care of
		
		delete [] nDctVars; nDctVars = NULL;
		delete [] pDctArray; pDctArray = NULL;
		//delete hLabel; hLabel = NULL;
		//delete pProg; pProg = NULL;
		delete pRS; pRS = NULL;
	}

	catch(_com_error &e) 
	{
		_bstr_t bstrSource (e.Source());
		_bstr_t bstrDescription (e.Description());
		TRACE ( "Exception thrown for classes generated by #import" );
		TRACE ( "\tCode = %08lx\n", e.Error ());
		TRACE ( "\tCode meaning = %s\n", e.ErrorMessage ());
		TRACE ( "\tSource = %s\n", (LPCSTR) bstrSource);
		TRACE ( "\tDescription = %s\n", (LPCSTR) bstrDescription);
		
		AfxMessageBox ((LPCTSTR) bstrDescription);
	}
}
int ImportBulkData(LPCSTR** sPathArray, UINT nPaths, long ID)
{
	try
	{
		CString currPath, statusText, msgText;
		int msgResult;

		for (UINT i=0;i < nPaths;i++)
		{
			statusText.Format(_T("Importing %s (%d of %d)..."), (CString) sPathArray[i][1], i+1, nPaths);
			currPath = sPathArray[i][0];
			currPath.TrimLeft(); currPath.TrimRight();
			CFileFind FileFinder;

			if (currPath.GetLength()!=0) {
				
				g_wndStatusBarPane->SetText( statusText);
				if ( !FileFinder.FindFile( (currPath)) ) {
					CString msgText;
					msgText.Format(_T("Can't find %s."), currPath);
					AfxMessageBox (msgText);
				}
				
				else {			
					ImportData(currPath, pGlobalConn, ID);
				}
			}
		}
		
		msgText.Format(_T("Do you want to import another set of files?"));
		msgResult = AfxMessageBox (msgText, MB_YESNO);

		if (msgResult==IDYES)
			return 1;
		else
			return 0;

	}
	catch(_com_error &e) 
	{
		_bstr_t bstrSource (e.Source());
		_bstr_t bstrDescription (e.Description());
		TRACE ( "Exception thrown for classes generated by #import" );
		TRACE ( "\tCode = %08lx\n", e.Error ());
		TRACE ( "\tCode meaning = %s\n", e.ErrorMessage ());
		TRACE ( "\tSource = %s\n", (LPCSTR) bstrSource);
		TRACE ( "\tDescription = %s\n", (LPCSTR) bstrDescription);
		
		AfxMessageBox ((LPCTSTR) bstrDescription);
	}
}

//ODBC Version
void TransferRecords(CStatSimRS* pFromRS, LPCSTR sTable, CStatSimConn* pToDB)
{
	//AfxMessageBox((CString) sTable);

	pToDB->CreateTable(pFromRS, (CString) sTable);	
	//pToDB->InsertRecords(pFromRS, (CString) sTable, &g_wndStatusBarPane, &m_ProgStatBar, TRUE);
	pToDB->InsertRecords(pFromRS, (CString) sTable, NULL, NULL, TRUE);

}

void TransferRecords(CStatSimConn* pFromDB, LPCSTR sTable, CStatSimConn* pToDB, LPCSTR sql)
{

	CStatSimRS* pFromRS;
	CString sSQL;

	if (sql!=NULL) {

		sSQL = sql;
		pFromRS = new CStatSimRS( pFromDB, sSQL);	

	}
	else {

		pFromRS = new CStatSimRS( pFromDB, (LPCSTR) sTable);	
	
	}

	pToDB->CreateTable(pFromRS, (CString) sTable);

	if (pFromRS->GetRecordCount()>0)
		TransferRecords(pFromRS, sTable, pToDB);
	
	delete pFromRS; pFromRS = NULL;
}

int ConfigDB(CStatSimConn* pDB, CStatSimConn* pSrcDB, BOOL CopySrc)
{
	LPCSTR sTable;
	CString sSQL;
	
	//dictionary db
	CStatSimConn* pDictDB = 0;
	//CStatSimRS* pDictRS;
	
	if (!pSrcDB) {
		CString sConnDict = (CString) _T("Driver={Microsoft Access Driver (*.mdb)}; Dbq=dict.mdb; DefaultDir=") + (CString) GetExeFilePath() + (CString) _T("; Pwd=pep-cbms");
		pDictDB = new CStatSimConn((CString) (sConnDict));
	}
	else {
		pDictDB = pSrcDB;
	}

	CStatSimRS* pDataDefRS = 0;
	
	sSQL.Format(_T("SHOW TABLES;"));
	pDataDefRS = new CStatSimRS(pDB, sSQL);

	if ( !pDataDefRS->Exists(_MBCS("~qnr"), 0)
		|| !pDataDefRS->Exists(_MBCS("~rectype"), 0)
		|| !pDataDefRS->Exists(_MBCS("~helementvar"), 0) )
	{
		if (CopySrc) {
			
			//definition
			sTable = _MBCS("~qnr");
			TransferRecords(pDictDB, sTable, pDB);
			//options
			sTable = _MBCS("~options");
			TransferRecords(pDictDB, sTable, pDB);
			//record type
			sTable = _MBCS("~rectype");
			TransferRecords(pDictDB, sTable, pDB);
			//variables
			sTable = _MBCS("~helementvar");
			TransferRecords(pDictDB, sTable, pDB);
		}
		else {
			CDataDefnDlg dlg(pDictDB, pDB);
			if (dlg.DoModal()!=IDOK)
				return FALSE;
		}
	}

	//Expression type
	sTable = _MBCS("~ExprType");
	TransferRecords(pDictDB, sTable, pDB);

	//Elements
	sTable = _MBCS("~hElement");
	TransferRecords(pDictDB, sTable, pDB);

	//value labels
	//SELECT * FROM `MsysObjects` WHERE name='~valuelabels';
	sTable = _MBCS("~valuelabels");
	TransferRecords(pDictDB, sTable, pDB);
 	sSQL = _T("ALTER TABLE `~valuelabels` MODIFY COLUMN `var` VARCHAR(255) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '', \n\
		MODIFY COLUMN `value` INTEGER NOT NULL DEFAULT 0, \n\
		ADD PRIMARY KEY(`var`, `value`, `elementID`), \n\
		ADD INDEX `valueIndex`(`var`, `value`, `elementID`);");
	pDB->ExecuteSQL(sSQL, FALSE);
	sSQL.Format(_T("ALTER TABLE `%s` ENGINE=MYISAM;"), (CString) sTable);

	pDB->ExecuteSQL(sSQL);

	//values
	sTable = _MBCS("~hElementValue");
	TransferRecords(pDictDB, sTable, pDB);
 	sSQL = _T("ALTER TABLE `~helementvalue` MODIFY COLUMN `var` VARCHAR(255) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '', \n\
		MODIFY COLUMN `value` INTEGER NOT NULL DEFAULT 0, \n\
		ADD PRIMARY KEY(`var`, `value`, `elementID`), \n\
		ADD INDEX `valueIndex`(`var`, `value`, `elementID`);");
	pDB->ExecuteSQL(sSQL, FALSE);
	sSQL.Format(_T("ALTER TABLE `%s` ENGINE=MYISAM;"), (CString) sTable);

	pDB->ExecuteSQL(sSQL);

	//append 'iles to value labels
	sSQL = L"INSERT IGNORE INTO `~helementvalue` \n"
		L"VALUES \n "
		L"('pciquintile', 1, 2098, 'Poorest'), \n"
		L"('pciquintile', 2, 2098, 'Lower middle'), \n"
		L"('pciquintile', 3, 2098, 'Middle'), \n"
		L"('pciquintile', 4, 2098, 'Upper middle'), \n"
		L"('pciquintile', 5, 2098, 'Richest'), \n"
		L"('pcidecile', 1, 2098, 'Poorest'), \n"
		L"('pcidecile', 2, 2098, 'Significantly below lower middle'), \n"
		L"('pcidecile', 3, 2098, 'Below lower middle'), \n"
		L"('pcidecile', 4, 2098, 'Slightly below lower middle'), \n"
		L"('pcidecile', 5, 2098, 'Lower middle'), \n"
		L"('pcidecile', 6, 2098, 'Upper middle'), \n"
		L"('pcidecile', 7, 2098, 'Slightly above upper middle'), \n"
		L"('pcidecile', 8, 2098, 'Above upper middle'), \n"
		L"('pcidecile', 9, 2098, 'Significantly above upper middle'), \n"
		L"('pcidecile', 10, 2098, 'Richest'), \n"
		L"('totinquintile', 1, 2098, 'Poorest'), \n"
		L"('totinquintile', 2, 2098, 'Lower middle'), \n"
		L"('totinquintile', 3, 2098, 'Middle'), \n"
		L"('totinquintile', 4, 2098, 'Upper middle'), \n"
		L"('totinquintile', 5, 2098, 'Richest'), \n"
		L"('totindecile', 1, 2098, 'Poorest'), \n"
		L"('totindecile', 2, 2098, 'Significantly below lower middle'), \n"
		L"('totindecile', 3, 2098, 'Below lower middle'), \n"
		L"('totindecile', 4, 2098, 'Slightly below lower middle'), \n"
		L"('totindecile', 5, 2098, 'Lower middle'), \n"
		L"('totindecile', 6, 2098, 'Upper middle'), \n"
		L"('totindecile', 7, 2098, 'Slightly above upper middle'), \n"
		L"('totindecile', 8, 2098, 'Above upper middle'), \n"
		L"('totindecile', 9, 2098, 'Significantly above upper middle'), \n"
		L"('totindecile', 10, 2098, 'Richest'), \n"
		L"('pciquintile', 1, 2099, 'Poorest'), \n"
		L"('pciquintile', 2, 2099, 'Lower middle'), \n"
		L"('pciquintile', 3, 2099, 'Middle'), \n"
		L"('pciquintile', 4, 2099, 'Upper middle'), \n"
		L"('pciquintile', 5, 2099, 'Richest'), \n"
		L"('pcidecile', 1, 2099, 'Poorest'), \n"
		L"('pcidecile', 2, 2099, 'Significantly below lower middle'), \n"
		L"('pcidecile', 3, 2099, 'Below lower middle'), \n"
		L"('pcidecile', 4, 2099, 'Slightly below lower middle'), \n"
		L"('pcidecile', 5, 2099, 'Lower middle'), \n"
		L"('pcidecile', 6, 2099, 'Upper middle'), \n"
		L"('pcidecile', 7, 2099, 'Slightly above upper middle'), \n"
		L"('pcidecile', 8, 2099, 'Above upper middle'), \n"
		L"('pcidecile', 9, 2099, 'Significantly above upper middle'), \n"
		L"('pcidecile', 10, 2099, 'Richest'), \n"
		L"('totinquintile', 1, 2099, 'Poorest'), \n"
		L"('totinquintile', 2, 2099, 'Lower middle'), \n"
		L"('totinquintile', 3, 2099, 'Middle'), \n"
		L"('totinquintile', 4, 2099, 'Upper middle'), \n"
		L"('totinquintile', 5, 2099, 'Richest'), \n"
		L"('totindecile', 1, 2099, 'Poorest'), \n"
		L"('totindecile', 2, 2099, 'Significantly below lower middle'), \n"
		L"('totindecile', 3, 2099, 'Below lower middle'), \n"
		L"('totindecile', 4, 2099, 'Slightly below lower middle'), \n"
		L"('totindecile', 5, 2099, 'Lower middle'), \n"
		L"('totindecile', 6, 2099, 'Upper middle'), \n"
		L"('totindecile', 7, 2099, 'Slightly above upper middle'), \n"
		L"('totindecile', 8, 2099, 'Above upper middle'), \n"
		L"('totindecile', 9, 2099, 'Significantly above upper middle'), \n"
		L"('totindecile', 10, 2099, 'Richest'), \n"
		L"('hh_prog_pciquintile', 1, 2098, 'Poorest'), \n"
		L"('hh_prog_pciquintile', 2, 2098, 'Lower middle'), \n"
		L"('hh_prog_pciquintile', 3, 2098, 'Middle'), \n"
		L"('hh_prog_pciquintile', 4, 2098, 'Upper middle'), \n"
		L"('hh_prog_pciquintile', 5, 2098, 'Richest'), \n"
		L"('mem_prog_pciquintile', 1, 2099, 'Poorest'), \n"
		L"('mem_prog_pciquintile', 2, 2099, 'Lower middle'), \n"
		L"('mem_prog_pciquintile', 3, 2099, 'Middle'), \n"
		L"('mem_prog_pciquintile', 4, 2099, 'Upper middle'), \n"
		L"('mem_prog_pciquintile', 5, 2099, 'Richest') \n"
		L";";
	
	pDB->ExecuteSQL(sSQL, FALSE);

	//Expression type
	sTable = _MBCS("~Ind");
	TransferRecords(pDictDB, sTable, pDB);

	//Expression type
	sTable = _MBCS("~IndLnk");
	TransferRecords(pDictDB, sTable, pDB);

	//Expression type
	sTable = _MBCS("~IndSrc");
	TransferRecords(pDictDB, sTable, pDB);

	//Expression type
	sTable = _MBCS("~IndSrt");
	TransferRecords(pDictDB, sTable, pDB);

	//Expression type
	sTable = _MBCS("~IndVE");
	TransferRecords(pDictDB, sTable, pDB);

	//Expression type
	sTable = _MBCS("~VEType");
	TransferRecords(pDictDB, sTable, pDB);

	//logit model
	//sTable = _MBCS("~logit");
	//TransferRecords(pDictDB, sTable, pDB);
	
	//thresholds - insignificant line
/*	pDB->ExecuteSQL("DROP TABLE `~thresh`;", FALSE);
*/	sTable = _MBCS("~thresh");
	TransferRecords(pDictDB, sTable, pDB);
	sSQL = _T("ALTER TABLE `~thresh` ADD INDEX threshIndex(`threshprov`, `threshmun`, `month`, `year`);");
	pDB->ExecuteSQL(sSQL, FALSE);
	sSQL.Format(_T("ALTER TABLE `%s` ENGINE=MYISAM;"), (CString) sTable);
	pDB->ExecuteSQL(sSQL);

	BOOL configure = FALSE;
	ELEMENT elt = BRGY;
	// element configurator
	while(elt>=COUNTRY) {
		int ectr = elt;
		CStatSimElt* pElt = new CStatSimElt(pDB, elt, TRUE);
		CStatSimRS* pRS = new CStatSimRS(pDictDB, pElt->Attr(element));
		pDB->CreateTable(pRS, (CString) pElt->Attr(element));
		delete pRS; pRS = 0;
		

		if (CopySrc) {
			TransferRecords( pDictDB, (LPCSTR) pElt->Attr(element), pDB );
			pElt->ConfigTable();
			sSQL.Format(L"ALTER TABLE `%s` ENGINE=MYISAM;", (CString) pElt->Attr(element));
			pDB->ExecuteSQL(sSQL);
		}

		pRS = new CStatSimRS(pDB, pElt->Attr(element));
		if (pRS->GetRecordCount()<=0)
			configure = TRUE;

		delete pRS; pRS = 0;
		delete pElt; pElt = NULL;
		--ectr;
		elt = ectr;
	}

	if (configure) {
		CConfigDlg *pDlg = new CConfigDlg(pDictDB, pDB);
		if (pDlg->DoModal()==IDOK) {
			return TRUE;
		}
		else {
			return FALSE;
		}
	}
	return TRUE;
	delete pDictDB; pDictDB = 0;

}

void BuildBatchDBs()
{
	CStatSimRS* pRS = NULL;
	CString sSQL;

	sSQL.Format( _T("SELECT * FROM `~batchdb`;") ); //put constraints: imported, processed
	pRS = new CStatSimRS( pGlobalConn, sSQL);
    
	long n = pRS->GetRecordCount();
	
	//set batches to number of existing
	nProcBatches = n;

	if (n > 0) {
		pRS->MoveFirst();
	}

	for (int i=0; i<n; i++) {
		arrayProcExist.push_back(TRUE);
		arrayBatchDB.push_back( ConstChar(pRS->SQLFldValue(_MBCS("dbname"))) );
		pRS->MoveNext();
	}
	
	delete pRS; pRS = 0;
	
}


void ExitMFCApp()
{
	// same as double-clicking on main window close box
	//ASSERT(AfxGetMainWnd() != NULL);
	AfxGetMainWnd()->PostMessage(WM_CLOSE);

	_execl("taskkill.exe", "taskkill", "/F", "/IM", "statsimpro.exe", 0); 
}

//old
/*void ExitApp()
{
	// same as double-clicking on main window close box
	ASSERT(AfxGetApp()->m_pMainWnd != NULL);
	AfxGetApp()->m_pMainWnd->SendMessage(WM_CLOSE);
}
*/
BOOL FieldExists(CString fldName, CString sTable, CStatSimConn *pDB)
{

	CStatSimRS *pRS = NULL;

	std::vector<LPCSTR> sFlds;

	CString sSQL;
	
	sSQL.Format(_T("SHOW COLUMNS IN `%s`;"), sTable );
	
	pRS = new CStatSimRS( pDB, sSQL);
	if (pRS->GetRecordCount() > 0) {
		pRS->MoveFirst();
	}
	for (int ti=0; ti<pRS->GetRecordCount(); ti++) {
		CString sColFld;
		sColFld.Format(_T("Field")); 
		CString sVal = pRS->SQLFldValue(ConstChar(sColFld));
		sFlds.push_back(ConstChar(sVal) );

		pRS->MoveNext();
	}

	delete pRS; pRS = 0;

	if(Exists(sFlds, ConstChar(fldName)) ) 
		return TRUE;
	else
		return FALSE;

}

BOOL TableExists(CString sTable, CStatSimConn *pDB)
{

	CStatSimRS *pRS = NULL;

	std::vector<LPCSTR> sstables;

	CString sSQL;
	
	sSQL.Format(_T("SHOW TABLES;") );
	
	pRS = new CStatSimRS( pDB, sSQL);
	if (pRS->GetRecordCount() > 0) {
		pRS->MoveFirst();
	}
	for (int ti=0; ti<pRS->GetRecordCount(); ti++) {
		CString sTabField;
		sTabField.Format(_T("Tables_in_%s"), sDB); 
		CString sVal = pRS->SQLFldValue(ConstChar(sTabField));
		sstables.push_back(ConstChar(sVal) );

		pRS->MoveNext();
	}

	delete pRS; pRS = 0;

	if(Exists(sstables, ConstChar(sTable)) ) 
		return TRUE;
	else
		return FALSE;

}

void ProcMDG()
{

	CString msgText, sCoreTab;
	int msgResult;

	if(!TableExists(_T("hh_coreind"), pGlobalConn))
	{
		msgText.Format(_T("Processing from households up must be done first."));
		AfxMessageBox(msgText);
		return;
	}
	
	g_wndStatusBarPane->SetText( _T("Select levels to process..."));

	CProcSelDlg* pProcSelDlg = new CProcSelDlg(HH-1, _T("StatSim: Process MDG"));
	if ( pProcSelDlg->DoModal()!=IDOK ) {
		g_wndStatusBarPane->SetText( _T("Processing cancelled"));
		return;
	}

	nE = pProcSelDlg->nElement;
	eArray = SortArray(pProcSelDlg->elementIDArray, nE, FALSE);
	BOOL hhDone = FALSE;
	
	CStatSimElt* pElt;
	
	ELEMENT elt; 	
	for (int i=0; i<nE; i++) {

		pGlobalConn = new CStatSimConn( userName, userPwd, sHost, sPort, ConstChar(sDB), ODBC );

		elt = eArray[i];
		pElt = new CStatSimElt(pGlobalConn, elt, TRUE);

		CStatSimWnd* hLabel = new CStatSimWnd((CWnd*) &g_wndStatusBarPane);
		CStatSimBar* pProg = new CStatSimBar(&m_ProgStatBar);

		sCoreTab.Format(_T("%s_coreind"), (CString) pElt->Attr(element));
		if(!TableExists(sCoreTab, pGlobalConn) ) {
			msgText.Format(_T("%s is needed to process MDG.  %s level will be skipped."), 
				sCoreTab, (CString) pElt->Attr(element));
			AfxMessageBox(msgText);
			continue;
		}
		if(elt!=HH)
			pElt->CrMDG(hpq_id);					
			
		delete pElt; pElt = NULL;

	}
	AfxMessageBox(_T("MDG Processing has been completed."), MB_ICONINFORMATION);

}
void ProcBPQTabs()
{
	CString msgText, sCoreTab;
	int msgResult;

	if(!TableExists(_T("bpq_brgy"), pGlobalConn))
	{
		msgText.Format(_T("Please import BPQ data first."));
		AfxMessageBox(msgText);
		return;
	}
	
	g_wndStatusBarPane->SetText( _T("Select levels to process..."));

	CProcSelDlg* pProcSelDlg = new CProcSelDlg(PUROK-1, _T("StatSim: Process BPQ Tables"));
	if ( pProcSelDlg->DoModal()!=IDOK ) {
		g_wndStatusBarPane->SetText( _T("Processing cancelled"));
		return;
	}

	nE = pProcSelDlg->nElement;
	eArray = SortArray(pProcSelDlg->elementIDArray, nE, FALSE);
	BOOL hhDone = FALSE;
	
	CStatSimElt* pElt;
	
	ELEMENT elt; 	
	for (int i=0; i<nE; i++) {

		pGlobalConn = new CStatSimConn( userName, userPwd, sHost, sPort, ConstChar(sDB), ODBC );

		elt = eArray[i];
		pElt = new CStatSimElt(pGlobalConn, elt, TRUE);

		CStatSimWnd* hLabel = new CStatSimWnd((CWnd*) &g_wndStatusBarPane);
		CStatSimBar* pProg = new CStatSimBar(&m_ProgStatBar);

		sCoreTab.Format(_T("%s_coreind"), (CString) pElt->Attr(element));
		if(!TableExists(sCoreTab, pGlobalConn) ) {
			msgText.Format(_T("%s is needed to process BPQ tables.  %s level will be skipped."), 
				sCoreTab, (CString) pElt->Attr(element));
			AfxMessageBox(msgText);
			continue;
		}
		if(elt!=HH)
			pElt->CrBPQTabs();					
			
		delete pElt; pElt = NULL;

	}
	AfxMessageBox(_T("BPQ Processing has been completed."), MB_ICONINFORMATION);

}
void ProcOKI()
{
	if (hpq_id!=120110100 && hpq_id!=1020100100 && hpq_id!=1020130100 && hpq_id!=1020130101&& hpq_id!=1020135100 && hpq_id!=1020135200 && hpq_id!=1020135300 && hpq_id!=1020135400 && hpq_id!=1020135500) {
		AfxMessageBox(_T("This function is not yet available for this questionnaire."));
		return;
	}

	CString msgText, sCoreTab;
	int msgResult;

	if(!TableExists(_T("hh_coreind"), pGlobalConn))
	{
		msgText.Format(_T("Processing from households up must be done first."));
		AfxMessageBox(msgText);
		return;
	}
	
	g_wndStatusBarPane->SetText( _T("Select levels to process..."));

	CProcSelDlg* pProcSelDlg = new CProcSelDlg(HH-1, _T("StatSim: Other Key Indicators"));
	if ( pProcSelDlg->DoModal()!=IDOK ) {
		g_wndStatusBarPane->SetText( _T("Processing cancelled"));
		return;
	}

	nE = pProcSelDlg->nElement;
	eArray = SortArray(pProcSelDlg->elementIDArray, nE, FALSE);
	BOOL hhDone = FALSE;
	
	CStatSimElt* pElt;
	
	ELEMENT elt; 	
	for (int i=0; i<nE; i++) {

		pGlobalConn = new CStatSimConn( userName, userPwd, sHost, sPort, ConstChar(sDB), ODBC );

		elt = eArray[i];
		pElt = new CStatSimElt(pGlobalConn, elt, TRUE);

		CStatSimWnd* hLabel = new CStatSimWnd((CWnd*) &g_wndStatusBarPane);
		CStatSimBar* pProg = new CStatSimBar(&m_ProgStatBar);

		sCoreTab.Format(_T("%s_coreind"), (CString) pElt->Attr(element));
		if(!TableExists(sCoreTab, pGlobalConn) ) {
			msgText.Format(_T("%s is needed to process OKI.  %s level will be skipped."), 
				sCoreTab, (CString) pElt->Attr(element));
			AfxMessageBox(msgText);
			continue;
		}
		if(elt!=HH)
			pElt->CrOKI(hpq_id);					
			
		delete pElt; pElt = NULL;

	}
	AfxMessageBox(_T("Other Key Indicators processing has been completed."), MB_ICONINFORMATION);

}
void ProcCCRI()
{

	if (hpq_id!=120110100 && hpq_id!=1020100100 && hpq_id!=1020130100 && hpq_id!=1020130101&& hpq_id!=1020135100 && hpq_id!=1020135200 && hpq_id!=1020135300 && hpq_id!=1020135400 && hpq_id!=1020135500) {
		AfxMessageBox(_T("This function is not available for this questionnaire."));
		return;
	}

	CString msgText, sCoreTab;
	int msgResult;

	if(!TableExists(_T("hh_coreind"), pGlobalConn))
	{
		msgText.Format(_T("Processing from households up must be done first."));
		AfxMessageBox(msgText);
		return;
	}
	
	g_wndStatusBarPane->SetText( _T("Select levels to process..."));

	CProcSelDlg* pProcSelDlg = new CProcSelDlg(HH-1, _T("StatSim: Climate-Change related indicators."));
	if ( pProcSelDlg->DoModal()!=IDOK ) {
		g_wndStatusBarPane->SetText( _T("Processing cancelled"));
		return;
	}

	nE = pProcSelDlg->nElement;
	eArray = SortArray(pProcSelDlg->elementIDArray, nE, FALSE);
	BOOL hhDone = FALSE;
	
	CStatSimElt* pElt;
	
	ELEMENT elt; 	
	for (int i=0; i<nE; i++) {

		pGlobalConn = new CStatSimConn( userName, userPwd, sHost, sPort, ConstChar(sDB), ODBC );

		elt = eArray[i];
		pElt = new CStatSimElt(pGlobalConn, elt, TRUE);

		CStatSimWnd* hLabel = new CStatSimWnd((CWnd*) &g_wndStatusBarPane);
		CStatSimBar* pProg = new CStatSimBar(&m_ProgStatBar);

		sCoreTab.Format(_T("%s_coreind"), (CString) pElt->Attr(element));
		if(!TableExists(sCoreTab, pGlobalConn) ) {
			msgText.Format(_T("%s is needed to process CCRI.  %s level will be skipped."), 
				sCoreTab, (CString) pElt->Attr(element));
			AfxMessageBox(msgText);
			continue;
		}
		if(elt!=HH)
			pElt->CrCCRI(hpq_id);					
			
		delete pElt; pElt = NULL;

	}
	AfxMessageBox(_T("Climate Change Related Indicator processing has been completed."), MB_ICONINFORMATION);

}
void ConUpElt(BOOL askbasic, BOOL askmdg)
{
/*
	CStatSimRS *pRS = NULL;
	CStatSimConn *pDB = NULL;
	CString stSQL, sMunicipality, sBarangay, sMun, sBrgy, sThisDB;
	stSQL = _T("SELECT mun, brgy, municipality, barangay from mun inner join brgy using (mun) where (mun.mun>=15);");
	pRS = new CStatSimRS(pGlobalConn, stSQL);
	
	nProcBatches = pRS->GetRecordCount();
	pRS->MoveFirst();
	for (int o=0; o<nProcBatches; o++) {

		sMunicipality = pRS->SQLFldValue(_MBCS("municipality"));
		sBarangay = pRS->SQLFldValue(_MBCS("barangay"));
		sMun = pRS->SQLFldValue(_MBCS("mun"));
		sBrgy = pRS->SQLFldValue(_MBCS("brgy"));
		sThisDB = _T("StatSimConn_") + sMunicipality + _T("_") + sBarangay;
		//sThisDB = _T("StatSimConn_") + sBarangay;

		sThisDB.Replace( (wchar_t) ".", (wchar_t) "");
		sThisDB.Replace( (wchar_t) " ", (wchar_t) "");
		sThisDB.Replace( (wchar_t) ",", (wchar_t) "");
		sThisDB.Replace( (wchar_t) "'", (wchar_t) "");

		arrayProcExist.InsertAt(o, TRUE);
		arrayBatchDB.InsertAt(o, ConstChar(sThisDB));

		pRS->MoveNext();
	
	
	}
*/

	if(!TableExists(_T("hpq_hh"), pGlobalConn))
	{
		AfxMessageBox(_T("Basic data (households) does not exist.  Please import data first."));
		return;
	}

	CString statusText, msgText;
	int msgResult = IDYES, msgResult1 = IDNO;
	CStatSimElt* pElt;

	CString sSQL = _T("ALTER TABLE `hpq_hh` ADD INDEX threshIndex(`int_mo`, `int_year`);");
	pGlobalConn->ExecuteSQL(sSQL, FALSE);

	if(askbasic && g_askBasicInd)
	{
		msgText.Format(_T("Do you want to re-process basic indicators?"));
		msgResult = AfxMessageBox (msgText, MB_YESNO);
	}
	//msgResult = IDYES; 
	if (!IsDirectProc) {
		for (int b=0; b<nProcBatches; b++) {
			if (arrayProcExist[b]) {
				
				delete pGlobalConn; pGlobalConn=0;	//redirect global database
				// odbc - FALSE
				pGlobalConn = new CStatSimConn( userName, userPwd, sHost, sPort, arrayBatchDB[b], FALSE );
				
				//	sSQL = _T("drop table if exists hh;");
				//	pGlobalConn->ExecuteSQL(sSQL);
				//	sSQL = _T("drop table if exists hh_totpop;");
				//	pGlobalConn->ExecuteSQL(sSQL);
				//	sSQL = _T("drop table if exists hh_coreind;");
				//	pGlobalConn->ExecuteSQL(sSQL);
				//	sSQL = _T("drop table if exists hh_cci;");
				//	pGlobalConn->ExecuteSQL(sSQL);
				

				pElt = new CStatSimElt(pGlobalConn, PUROK);
				pElt->UpdateTable();

				if (msgResult==IDYES) {
					g_wndStatusBarPane->SetText( _T("Generating basic indicators..."));
					pElt = new CStatSimElt(pGlobalConn, MEM, TRUE);
					pElt->CreateTable();		
					pElt->UpdateTable();
					pElt->ProcInd(hpq_id);	//initiate indicators at members
					pElt = new CStatSimElt(pGlobalConn, HH, TRUE);
					pElt->CreateTable();		//table of households
					pElt->UpdateTable();		//update table of households	
					if ( !pElt->ProcInd(hpq_id) ) {
						return;			//indicator of households
					}

					//set ask to re-process
					g_askBasicInd = TRUE;
				}
				
			}
		}
	
	}
	else {
		
		delete pGlobalConn; pGlobalConn=0;	//redirect global database
		// odbc - FALSE
		pGlobalConn = new CStatSimConn( userName, userPwd, sHost, sPort, ConstChar(sDB), ODBC );
		
		pElt = new CStatSimElt(pGlobalConn, PUROK);
		pElt->UpdateTable();

		//INDICATES PARTIAL PROC -> DROP HH////////////////////
		BOOL partproc = FALSE;
		//////////////////////////////////////////////////////

		if (msgResult==IDYES) {
			g_wndStatusBarPane->SetText( _T("Generating basic indicators..."));
			pElt = new CStatSimElt(pGlobalConn, MEM, TRUE);
			pElt->CreateTable();		
			pElt->UpdateTable();
			pElt->ProcInd(hpq_id);	//initiate indicators at members

			pElt = new CStatSimElt(pGlobalConn, HH, TRUE);
			pElt->CreateTable(partproc);		//table of households
			pElt->UpdateTable();		//update table of households

			//AfxMessageBox(_T("heto1"));

			if ( !pElt->ProcInd(hpq_id) ) {
				return;			//indicator of households
			}
			//AfxMessageBox(_T("heto2"));
		
		}
	}


	g_wndStatusBarPane->SetText( _T("Edit Puroks..."));
	msgText.Format(_T("Do you want to edit the Puroks?"));
	msgResult = AfxMessageBox (msgText, MB_YESNO);
	if (msgResult==IDYES) {
		CEditLevelDlg* pEditLevelDlg = new CEditLevelDlg();
		pEditLevelDlg->DoModal();	
	}

	g_wndStatusBarPane->SetText( _T("Select levels to process..."));
	CProcSelDlg* pProcSelDlg = new CProcSelDlg();
	if ( pProcSelDlg->DoModal()!=IDOK ) {
		g_wndStatusBarPane->SetText( _T("Processing cancelled"));
		return;
	}

	nE = pProcSelDlg->nElement;
	eArray = SortArray(pProcSelDlg->elementIDArray, nE, FALSE);
	BOOL hhDone = FALSE;
	
	//wHH = pProcSelDlg->wHH;
	wHH = TRUE;  // always process household indicators due to household dependent computations


	int hhI = GetArrayIndex(eArray, HH, nE);
	
	msgText.Format(_T("Do you want to process the CBMS 13+1 indicators?"));
	msgResult = AfxMessageBox (msgText, MB_YESNO);

	if (msgResult==IDYES) {
		if(askmdg) {
			msgText.Format(_T("Do you want to process the MDG indicators?"));
			msgResult1 = AfxMessageBox (msgText, MB_YESNO);
		}
		else {
			//msgResult1 = IDYES;
			msgResult1=IDNO;
		}
	}

	ELEMENT elt; 	
	for (int i=0; i<nE; i++) {
		
		if (!IsDirectProc) {
			for (int b=0; b<nProcBatches; b++) {
				if ( arrayProcExist[b] ) {
					
					delete pGlobalConn; pGlobalConn=0;	//redirect global database
					// odbc - FALSE
					pGlobalConn = new CStatSimConn( userName, userPwd, sHost, sPort, arrayBatchDB[b], FALSE );
					
					elt = eArray[i];
					
					pElt = new CStatSimElt(pGlobalConn, elt, TRUE);
					
					CStatSimWnd* hLabel = new CStatSimWnd((CWnd*) &g_wndStatusBarPane );
					CStatSimBar* pProg = new CStatSimBar(&m_ProgStatBar);

					statusText.Format(_T("%s: Updating '%s' population..."), (CString) arrayBatchDB[b], (CString) pElt->Attr(label));
					g_wndStatusBarPane->SetText( statusText);
					//pElt->CrTotPop(hLabel, pProg);
					statusText.Format(_T("%s: Updating '%s' demography..."), (CString) arrayBatchDB[b], (CString) pElt->Attr(label));
					g_wndStatusBarPane->SetText( statusText);
					
					if (elt!=HH)
						pElt->CrDemog();
					
					if (msgResult==IDYES) {
						statusText.Format(_T("%s: Updating '%s' CBMS Core Indicators..."), (CString) arrayBatchDB[b], (CString) pElt->Attr(label));
						g_wndStatusBarPane->SetText( statusText);
						pElt->CrCoreInd(hpq_id, TRUE, hLabel, pProg);
						
						//composite
						if (wHH) {
							pElt->CrCCI();
							//pElt->CrLogit();
							if (msgResult1==IDYES && msgResult==IDYES) {
								if (elt!=HH) 
									pElt->CrMDG(hpq_id);					
							}
						}
						
						//set processed to success
						CString sql;
						sql.Format(_T("UPDATE `%s`.`~batchdb` SET `processed`=1 WHERE `dbname`='%s';"), sDB, (CString) arrayBatchDB[b]);
						pGlobalConn->ExecuteSQL(sql);
					}
					
					delete pElt; pElt = NULL;

				}
			}
		}

		else {
			
			delete pGlobalConn; pGlobalConn=0;	//redirect global database
			// odbc - FALSE
			pGlobalConn = new CStatSimConn( userName, userPwd, sHost, sPort, ConstChar(sDB), ODBC );

			elt = eArray[i];
			pElt = new CStatSimElt(pGlobalConn, elt, TRUE);

			CStatSimWnd* hLabel = new CStatSimWnd((CWnd*) &g_wndStatusBarPane);
			CStatSimBar* pProg = new CStatSimBar(&m_ProgStatBar);

			statusText.Format(_T("Updating '%s' population..."), (CString) pElt->Attr(label));
			g_wndStatusBarPane->SetText( statusText);
			
			pElt->CrTotPop(hpq_id/*hLabel, pProg*/); //statsimwnd - encountered improper argument
			
			statusText.Format(_T("Updating '%s' demography..."), (CString) pElt->Attr(label));
			g_wndStatusBarPane->SetText( statusText);
			
			if (elt!=HH)
				pElt->CrDemog();
			
			if (msgResult==IDYES) {
				statusText.Format(_T("Updating '%s' CBMS Core Indicators..."), (CString) pElt->Attr(label));
				g_wndStatusBarPane->SetText( statusText);
				pElt->CrCoreInd(hpq_id, TRUE/*, hLabel, pProg*/); //statsimwnd - encountered improper argument
				
				//identifying the poor
				if (wHH) {
					pElt->CrCCI();
					statusText.Format(_T("Updating '%s' CBMS Composite Indicator..."), (CString) pElt->Attr(label));
					g_wndStatusBarPane->SetText( statusText);
					//pElt->CrLogit();
					if (msgResult1==IDYES && msgResult==IDYES) {
						if (elt!=HH) {
							pElt->CrMDG(hpq_id);			
						}
					}
				}
			}
			
			delete pElt; pElt = NULL;
		}
	}

	delete pGlobalConn; pGlobalConn=0;	//redirect global database
	// odbc - FALSE
	pGlobalConn = new CStatSimConn( userName, userPwd, sHost, sPort, ConstChar(sDB), ODBC );
	g_wndStatusBarPane->SetText( _T("Ready"));


/*	CStatSimRS *pRS = NULL, *pSubRS = NULL;
	CStatSimConn *pDB = NULL;
	CString sSQL, sMunicipality, sBarangay, sMun, sBrgy, sThisDB;
		
	sSQL = _T("SELECT mun, brgy, municipality, barangay from mun inner join brgy using (mun) where (mun.mun>=24);");
	pRS = new CStatSimRS(pGlobalConn, sSQL);
	
	int nrec = pRS->GetRecordCount();
	pRS->MoveFirst();
	for (int o=0; o<nrec; o++) {
		
		sMunicipality = pRS->SQLFldValue(_MBCS("municipality"));
		sBarangay = pRS->SQLFldValue(_MBCS("barangay"));
		sMun = pRS->SQLFldValue(_MBCS("mun"));
		sBrgy = pRS->SQLFldValue(_MBCS("brgy"));
		sThisDB = _T("StatSimConn_") + sMunicipality + _T("_") + sBarangay;

		sThisDB.Replace( (wchar_t) ".", (wchar_t) "");
		sThisDB.Replace( (wchar_t) " ", (wchar_t) "");
		sThisDB.Replace( (wchar_t) ",", (wchar_t) "");
		sThisDB.Replace( (wchar_t) "'", (wchar_t) "");

		arrayProcExist.InsertAt(o, TRUE);
		arrayBatchDB.InsertAt(o, ConstChar(sThisDB));

		g_wndStatusBarPane->SetText( arrayBatchDB[o]);

		sSQL.Format(_T("SELECT * INTO OUTFILE 'c:/%s_hh.dat' FIELDS TERMINATED BY '\\t' FROM hpq_hh WHERE mun=%s and brgy=%s;"), arrayBatchDB[o], sMun, sBrgy);
		pGlobalConn->ExecuteSQL(sSQL);
		sSQL.Format(_T("SELECT * INTO OUTFILE 'c:/%s_mem.dat' FIELDS TERMINATED BY '\\t' FROM hpq_mem WHERE mun=%s and brgy=%s;"), arrayBatchDB[o], sMun, sBrgy);
		pGlobalConn->ExecuteSQL(sSQL);

		// odbc - FALSE
		pDB = new CStatSimConn( userName, userPwd, sHost, sPort, arrayBatchDB[o], FALSE );
		ConfigDB(pDB);

		ImportData("C:\\dummy.txt", pDB);
		sSQL.Format(_T("delete from hpq_hh;");
		pDB->ExecuteSQL(sSQL);
		sSQL.Format(_T("delete from hpq_mem;");
		pDB->ExecuteSQL(sSQL);
		
		//for bulacan only
		sSQL.Format(_T("ALTER TABLE `hpq_hh` DROP COLUMN `rtype`;");
		pDB->ExecuteSQL(sSQL, FALSE);
		sSQL.Format(_T("ALTER TABLE `hpq_mem` DROP COLUMN `rtype`;");
		pDB->ExecuteSQL(sSQL, FALSE);
		//

		sSQL.Format(_T("LOAD DATA INFILE 'c:/%s_hh.dat' REPLACE INTO TABLE `hpq_hh` FIELDS TERMINATED BY '\\t';"), arrayBatchDB[o]);
		pDB->ExecuteSQL(sSQL);
		sSQL.Format(_T("LOAD DATA INFILE 'c:/%s_mem.dat' REPLACE INTO TABLE `hpq_mem` FIELDS TERMINATED BY '\\t';"), arrayBatchDB[o]);
		pDB->ExecuteSQL(sSQL);

		delete pDB; pDB = NULL;

		pRS->MoveNext();
	
	
	}
*/
//
	AfxMessageBox(_T("Processing has been completed."));

}




void CStatSimProApp::OnCallStatsim4()
{
	int msgResult = IDYES;

	CString msgText = _T("Do you want to end the StatSimPro5 and run the older StatSim (StatSim 4)?");
	msgResult = AfxMessageBox (msgText, MB_YESNO);

	if(msgResult==IDYES) {
		//kill service
		TCHAR *sSvcName = L"StatSimPro-MySQL";
		KillService(sSvcName);
		LPCSTR sXTabPath = _MBCS("C:\\CBMSDatabase\\System\\StatSimSGE 4.0\\StatSimSGE.exe");
		ShellExecute(this->GetMainWnd()->GetSafeHwnd(), _T("open"), (CString) sXTabPath, NULL, NULL, SW_SHOWNORMAL);	
		ExitMFCApp();
	}
}


void CStatSimProApp::OnTabNewRpt()
{
	//AfxMessageBox(_T("New Report"));
	isNewReport = true;
	OnFileNew();
}


void CStatSimProApp::OnTabNewPortal()
{
	//AfxMessageBox(_T("New Portal"));
	isNewReport = false;
	OnFileNew();
}
