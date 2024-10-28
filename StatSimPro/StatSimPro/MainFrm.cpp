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

// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "StatSimPro.h"
#include "SwitchDlg.h"
#include "MainFrm.h"

#include "StatSimProView.h"
#include "StatSimInd.h"
#include "StatSimHTML.h"

#include "ExeSQLDlg.h"
#include "cmdDlg.h"
#include "MergeDataDlg.h"
#include "SelDataDlg.h"
#include "ProcSelDlg.h"
#include "EditThreshDlg.h"
#include "ClearSelHHDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CMFCRibbonStatusBarPane  *g_wndStatusBarPane;//I have transferred this from main to global to be accessible
extern CStatSimConn* pGlobalConn;
extern CString userName, userPwd, sHost, sPort, sDB;
extern bool IS_ODBC_INT;
extern CStatSimRS* pelemRS;
extern CStatSimRS ***pdictRS;
extern daedict ***pDCT;
extern CStatSimProView *g_pSSHTMLView;
extern LPCTSTR g_sRptPath;

extern long hpq_id;

extern BOOL g_askBasicInd;

DLLIMPORT LPCSTR **sHHCoreInd;
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_VIEW_CAPTION_BAR, &CMainFrame::OnViewCaptionBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CAPTION_BAR, &CMainFrame::OnUpdateViewCaptionBar)
	ON_COMMAND(ID_TOOLS_OPTIONS, &CMainFrame::OnOptions)
	ON_COMMAND(ID_FILE_IMPORT, &CMainFrame::OnFileImport)
	ON_NOTIFY(NM_DBLCLK, 1200, OnNMDblclkTreeRpt)
	ON_COMMAND(ID_DATA_RESET, &CMainFrame::OnDataReset)
	ON_COMMAND(ID_FILE_EXPORT, &CMainFrame::OnFileExport)
	ON_COMMAND(ID_DATA_CLEARHH, &CMainFrame::OnDataClearhh)
	ON_COMMAND(ID_DATA_CLEAR_BPQ, &CMainFrame::OnDataClearBPQ)
	ON_COMMAND(ID_DATA_CLEARSELHH, &CMainFrame::OnDataClearSelHH)
	ON_COMMAND(ID_DATA_UPDATE, &CMainFrame::OnDataUpdate)
	ON_COMMAND(ID_DATA_PROCMDG, &CMainFrame::OnDataProcMDG)
	ON_COMMAND(ID_DATA_PROCOKI, &CMainFrame::OnDataProcOKI)
	ON_COMMAND(ID_DATA_PROCCCRI, &CMainFrame::OnDataProcCCRI)
	ON_COMMAND(ID_DATA_PROCBPQ, &CMainFrame::OnDataProcBPQ)
	ON_COMMAND(ID_STATS_EXESQL, OnStatsExeSQL)
	ON_COMMAND(ID_STATS_CMD, OnStatsExeCMD)
	ON_COMMAND(ID_DATA_MATCH, &CMainFrame::OnDataMatch)
	ON_COMMAND(ID_FILE_EXPORT_NRDB, OnFileExportNrdb)
	ON_COMMAND(ID_FILE_EXPORT_CORE_CSV, OnFileExportCoreCsv)
	ON_COMMAND(ID_FILE_EXPORT_CCI_CSV, OnFileExportCCICsv)
	ON_COMMAND(ID_FILE_EXPORT_MDG_CSV, OnFileExportMDGCsv)
	ON_COMMAND(ID_FILE_EXPORT_CCI, OnFileExportCCI)
	ON_COMMAND(ID_FILE_EXPORT_MDG, OnFileExportMDG)
	ON_COMMAND(ID_FILE_EXPORT_DATA, OnFileExportData)
	ON_COMMAND(ID_STATS_CSPROXTAB_HH, OnStatsCSProXTabHH)
	ON_COMMAND(ID_STATS_CSPROXTAB_BRGY, OnStatsCSProXTabBrgy)
	ON_COMMAND(ID_DATA_EDITTHRESH, OnDataEditThresh)
	ON_COMMAND(ID_DATA_ATTCLEAN, OnDataAttClean)
	ON_COMMAND(ID_CALL_ENCODE, OnCallEncode)
	ON_COMMAND(ID_CALL_NRDB, OnCallNRDB)
	ON_COMMAND(ID_CALL_NOTEPAD, &CMainFrame::OnCallNotepad)
END_MESSAGE_MAP()


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_WINDOWS_7);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;
	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // other styles available...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // set to FALSE to place close button at right of tab area
	mdiTabParams.m_bTabIcons = FALSE;    // set to TRUE to enable document icons on MDI taba
	mdiTabParams.m_bAutoColor = TRUE;    // set to FALSE to disable auto-coloring of MDI tabs
	mdiTabParams.m_bDocumentMenu = TRUE; // enable the document menu at the right edge of the tab area
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	g_wndStatusBarPane = new CMFCRibbonStatusBarPane(); //dito nanggaling dati ang error!

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	
	//m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	//m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);

	//START -  MY MOD
	strTitlePane1.LoadString(IDS_STATSIM_DESC);
	//g_wndStatusBarPane->SetText(strTitlePane1);
	g_wndStatusBarPane->SetText(_T("PEP-Asia CBMS Network"));
	m_wndStatusBar.AddElement(g_wndStatusBarPane, L"");

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// Navigation pane will be created at left, so temporary disable docking at the left side:
	EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM | CBRS_ALIGN_RIGHT);

	// Create and setup "Outlook" navigation bar:
	if (!CreateOutlookBar(m_wndNavigationBar, ID_VIEW_NAVIGATION, m_wndTree, m_wndCalendar, 250))
	{
		TRACE0("Failed to create navigation pane\n");
		return -1;      // fail to create
	}

	// Create a caption bar:
	/*if (!CreateCaptionBar())
	{
		TRACE0("Failed to create caption bar\n");
		return -1;      // fail to create
	}
	*/
	// Outlook bar is created and docking on the left side should be allowed.
	EnableDocking(CBRS_ALIGN_LEFT);
	EnableAutoHidePanes(CBRS_ALIGN_RIGHT);

	// Load menu item image (not placed on any standard toolbars):
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// create docking windows
/*	if (!CreateDockingWindows())
	{
		TRACE0("Failed to create docking windows\n");
		return -1;
	}
*/

	// Enable enhanced windows management dialog
	EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	// Switch the order of document name and application name on the window title bar. This
	// improves the usability of the taskbar because the document name is visible with the thumbnail.
	ModifyStyle(0, FWS_PREFIXTITLE);

//	m_wndDataView.EnableDocking(CBRS_ALIGN_ANY);
//	m_wndReportView.EnableDocking(CBRS_ALIGN_ANY);
//	DockPane(&m_wndDataView);
//	CDockablePane* pTabbedBar = NULL;
//	m_wndReportView.AttachToTabWnd(&m_wndDataView, DM_SHOW, TRUE, &pTabbedBar);
//	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
//	DockPane(&m_wndOutput);
//	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
//	DockPane(&m_wndProperties);



/*	if (!m_wndDlgBar.Create(this, IDD_PROGRESSDLGBAR,
		CBRS_BOTTOM|CBRS_TOOLTIPS|CBRS_FLYBY, IDD_PROGRESSDLGBAR))
	{
		TRACE0("Failed to create DlgBar\n");
		return -1;      // Fail to create.
	}

	CRect rect;
	int iInd = m_wndStatusBar.CommandToIndex(ID_SEPARATOR);
	m_wndStatusBar.GetItemRect(iInd, rect);

	int l = rect.left, 
		t = rect.top, 
		r = rect.right,
		b = rect.bottom;

	//CString msg; msg.Format(_T("%d, %d, %d, %d", l,t,r,b);
	//AfxMessageBox(msg);

	m_ProgStatBar.Create(WS_CHILD|PBS_SMOOTH, 
		CRect(1, 5, 200 ,15), 
		&m_wndDlgBar, IDC_PROGRESSBAR);
*/
	//set window text as the current statsim
	CString sWndTxt = _T("CBMS StatSimPro: ") + sDB.MakeUpper();
	//this->SetWindowText(sWndTxt);
	//CMainFrame *pFrame=(CMainFrame*)GetParent();
	//pFrame->SetAppName(sWndTxt);
	//pFrame->OnUpdateFrameTitle(TRUE);	
	::SetWindowText(this->GetSafeHwnd(), sWndTxt);
	//END - MY MOD

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
	{
		return FALSE;
	}
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	
	cs.style &= ~FWS_ADDTOTITLE;	//contain only the titlke


	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// Create class view
	CString strReportView;
	bNameValid = strReportView.LoadString(IDS_REPORT_VIEW);
	ASSERT(bNameValid);
	if (!m_wndReportView.Create(strReportView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_REPORTVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT ))//| CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Class View window\n");
		return FALSE; // failed to create
	}

	// Create file view
	CString strDataView;
	bNameValid = strDataView.LoadString(IDS_DATA_VIEW);
	ASSERT(bNameValid);
	if (!m_wndDataView.Create(strDataView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_DATAVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create File View window\n");
		return FALSE; // failed to create
	}

	// Create output window
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Output window\n");
		return FALSE; // failed to create
	}

	// Create properties window
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Properties window\n");
		return FALSE; // failed to create
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hDataViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_DATA_VIEW_HC : IDI_DATA_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndDataView.SetIcon(hDataViewIcon, FALSE);

	HICON hReportViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_REPORT_VIEW_HC : IDI_REPORT_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndReportView.SetIcon(hReportViewIcon, FALSE);

	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

	UpdateMDITabbedBarsIcons();
}

BOOL CMainFrame::CreateOutlookBar(CMFCOutlookBar& bar, UINT uiID, CTreeCtrl& tree, CCalendarBar& calendar, int nInitialWidth)
{
	bar.SetMode2003();

	BOOL bNameValid;
	CString strTemp;
	bNameValid = strTemp.LoadString(IDS_SHORTCUTS);
	ASSERT(bNameValid);
	if (!bar.Create(strTemp, this, CRect(0, 0, nInitialWidth, 32000), uiID, WS_CHILD | WS_VISIBLE | CBRS_LEFT))
	{
		return FALSE; // fail to create
	}

	CMFCOutlookBarTabCtrl* pOutlookBar = (CMFCOutlookBarTabCtrl*)bar.GetUnderlyingWindow();

	if (pOutlookBar == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	pOutlookBar->EnableInPlaceEdit(TRUE);

	static UINT uiPageID = 1;

	// can float, can autohide, can resize, CAN NOT CLOSE
	DWORD dwStyle = AFX_CBRS_FLOAT | AFX_CBRS_AUTOHIDE | AFX_CBRS_RESIZE;

	CRect rectDummy(0, 0, 0, 0);
	const DWORD dwTreeStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	tree.Create(dwTreeStyle, rectDummy, &bar, 1200);
	// image
	CImageList *pImage = new CImageList();
	pImage->Create(16, 16, ILC_COLOR32, 0, 1);
	m_IndImage = pImage->Add(AfxGetApp()->LoadIcon(IDI_GLOBE));
	m_EltImage = pImage->Add(AfxGetApp()->LoadIcon(IDI_TEXTFILE));
	m_wndTree.SetImageList( pImage, TVSIL_NORMAL );
	
	bNameValid = strTemp.LoadString(IDS_FOLDERS);
	ASSERT(bNameValid);
	pOutlookBar->AddControl(&tree, strTemp, 2, TRUE, dwStyle);

	tree.DeleteAllItems();
	PopulateIndTree(tree);
	//tree.InsertItem(L"Panget1");
	//tree.InsertItem(L"Panget2");


//	calendar.Create(rectDummy, &bar, 1201);
//	bNameValid = strTemp.LoadString(IDS_CALENDAR);
//	ASSERT(bNameValid);
//	pOutlookBar->AddControl(&calendar, strTemp, 3, TRUE, dwStyle);


	bar.SetPaneStyle(bar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	pOutlookBar->SetImageList(theApp.m_bHiColorIcons ? IDB_PAGES_HC : IDB_PAGES, 24);
	pOutlookBar->SetToolbarImageList(theApp.m_bHiColorIcons ? IDB_PAGES_SMALL_HC : IDB_PAGES_SMALL, 16);
	pOutlookBar->RecalcLayout();

	BOOL bAnimation = theApp.GetInt(_T("OutlookAnimation"), TRUE);
	CMFCOutlookBarTabCtrl::EnableAnimation(bAnimation);

	bar.SetButtonsFont(&afxGlobalData.fontBold);

	return TRUE;
}

BOOL CMainFrame::CreateCaptionBar()
{
	if (!m_wndCaptionBar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this, ID_VIEW_CAPTION_BAR, -1, TRUE))
	{
		TRACE0("Failed to create caption bar\n");
		return FALSE;
	}

	BOOL bNameValid;

	CString strTemp, strTemp2;
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButton(strTemp, ID_TOOLS_OPTIONS, CMFCCaptionBar::ALIGN_LEFT, FALSE);
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON_TIP);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButtonToolTip(strTemp);

	bNameValid = strTemp.LoadString(IDS_CAPTION_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetText(strTemp, CMFCCaptionBar::ALIGN_LEFT);

	m_wndCaptionBar.SetBitmap(IDB_INFO, RGB(255, 255, 255), FALSE, CMFCCaptionBar::ALIGN_LEFT);
	bNameValid = strTemp.LoadString(IDS_CAPTION_IMAGE_TIP);
	ASSERT(bNameValid);
	bNameValid = strTemp2.LoadString(IDS_CAPTION_IMAGE_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetImageToolTip(strTemp, strTemp2);

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::OnViewCaptionBar()
{
	m_wndCaptionBar.ShowWindow(m_wndCaptionBar.IsVisible() ? SW_HIDE : SW_SHOW);
	RecalcLayout(FALSE);
}

void CMainFrame::OnUpdateViewCaptionBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndCaptionBar.IsVisible());
}

void CMainFrame::OnOptions()
{
	CMFCRibbonCustomizeDialog *pOptionsDlg = new CMFCRibbonCustomizeDialog(this, &m_wndRibbonBar);
	ASSERT(pOptionsDlg != NULL);

	pOptionsDlg->DoModal();
	delete pOptionsDlg;
}

void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CMDIFrameWndEx::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
}


void CMainFrame::OnFileImport()
{
	pSwitchDlg = new CSwitchDlg();
	pSwitchDlg->DoModal();
}

void CMainFrame::OnNMDblclkTreeRpt(NMHDR *pNMHDR, LRESULT *pResult)
{
	BeginWaitCursor();
	//theApp.OnFileNew();

	//if (ActiveView==REPORT) {
		DisplayInd();
	//}

	//else if (ActiveView==DATA) {
	//	DisplayDta();
	//}

	EndWaitCursor();

}

void CMainFrame::OnDataReset()
{
	CString msgText, sSQL;

	msgText.Format(_T("Do you really want to reset the database?  After this, you will have to run the program again to initialize.  Do you wish to continue?"));
	int msgResult = AfxMessageBox (msgText, MB_YESNO);
	
	if (msgResult!=IDYES) {
		return;
	}

	sSQL.Format(_T("DROP DATABASE `%s`;"), sDB);
	
	pGlobalConn->ExecuteSQL(sSQL);

	ExitMFCApp();

}


void CMainFrame::SetAppName(LPCSTR Title) {
	m_strTitle = Title;
}


void CMainFrame::OnFileExportCoreCsv() 
{

	ExportCsv(_T("coreind"), _T("Core indicator (CoreInd)"));

}
void CMainFrame::ExportCsv(CString table_ext, CString tablab, int eltID) {
	//create directory	
	CreateDirectory(_T("C:\\CBMSDatabase\\System\\Output"), NULL);

	CString sTable, statusText, sSQL, sVal, sPath;
	CStatSimElt* pElt;
	CStatSimRS* pRS = NULL;
	int ectr = 0;

	g_wndStatusBarPane->SetText(_T("Select levels to process..."));
	CProcSelDlg* pProcSelDlg = new CProcSelDlg(eltID);
	if ( pProcSelDlg->DoModal()!=IDOK ) {
		g_wndStatusBarPane->SetText(_T("Processing cancelled"));
		return;
	}

	int* eArray = pProcSelDlg->elementIDArray;
	int nE = pProcSelDlg->nElement;
	BOOL hhDone = FALSE, wHH = pProcSelDlg->wHH;


	ELEMENT elt; 	
	int _nExport = nE;

	for (int i=0; i<nE; i++) {
		
		elt = eArray[i];
		pElt = new CStatSimElt(pGlobalConn, elt, TRUE);

		sTable.Format(_T("%s_%s"), (CString) pElt->Attr(element), (CString) table_ext);

		if (!TableExists(sTable, pGlobalConn)) {
			CString msg;
			msg = _T("The table '") + sTable + _T("' does not exist! This will be skipped.");
			AfxMessageBox(msg);
			_nExport--;
			continue;
		}

		pRS = new CStatSimRS( pGlobalConn, ConstChar(sTable));

		statusText.Format(_T("Exporting '%s'..."), sTable);
		g_wndStatusBarPane->SetText( (statusText));

		sPath.Format(_T("C:\\CBMSDatabase\\System\\Output\\%s.csv"), (CString) sTable);
		FILE* pFile = _tfopen( (sPath), _T("w"));

		if (!pFile) {
			CString msg;
			msg = _T("Cannot access '") + sPath + _T("'! This will be skipped.");
			AfxMessageBox(msg);
			_nExport--;
			continue;
		}

		if (pRS->GetRecordCount()>0) {
			pRS->MoveFirst();
		}
		
		//field names first
		for (int j=0; j<pRS->GetFieldCount(); j++) {
			fprintf( pFile, pRS->GetFieldName(j) );
			fprintf( pFile, _MBCS(",") );
		}
		_ftprintf( pFile, _T("\n") );
		
		//then the records
		for (int i=0; i<pRS->GetRecordCount(); i++) {			
			for (int j=0; j<pRS->GetFieldCount(); j++) {
				
				//handle commas
				sVal = pRS->SQLFldValue(j);
				sVal.Replace(_T(","), _T(";"));

				_ftprintf( pFile, ( sVal ) );
				_ftprintf( pFile, _T(",") );
			}
			_ftprintf( pFile, _T("\n") );
			pRS->MoveNext();
		}

		delete pRS; pRS = NULL;
		
		fclose(pFile);
	}

	CString msg;
	msg.Format(_T("Exported %d %s table(s) to 'C:\\CBMSDatabase\\System\\Output'"), _nExport, tablab);
	AfxMessageBox(msg);
	
	
	g_wndStatusBarPane->SetText(_T("Ready"));
}

void CMainFrame::OnFileExportCCICsv() 
{

	ExportCsv(_T("cci"), _T("Composite Indicator (CCI)"));


}

void CMainFrame::OnFileExportMDGCsv() 
{

	ExportCsv(_T("mdg"), _T("MDG"), HH-1);


}
void CMainFrame::OnFileExport()
{
	// TODO: Add your command handler code here
}
void CMainFrame::OnFileExportNrdb() 
{
	CString sTable, statusText, sSQL;
	CStatSimElt* pElt;
	CStatSimRS* pRS = NULL;
	int ectr = 0;

	CString sSrcPath = (CString) SSPATH + (CString) _T("\\Ind_NRDB.mdb");

	//copy first
	CreateDirectory(_T("C:\\CBMSDatabase\\System\\Output"), NULL);
	remove("C:\\CBMSDatabase\\System\\Output\\Ind_NRDB.mdb");
	bool fcnResult = FALSE;	
    CopyFile((sSrcPath), 
		_T("C:\\CBMSDatabase\\System\\Output\\Ind_NRDB.mdb"), fcnResult);

	g_wndStatusBarPane->SetText(_T("Select levels to process..."));
	CProcSelDlg* pProcSelDlg = new CProcSelDlg();
	if ( pProcSelDlg->DoModal()!=IDOK ) {
		g_wndStatusBarPane->SetText(_T("Processing cancelled"));
		return;
	}

	CStatSimConn* pOutDB;
	LPCTSTR sConn = _T ("Driver={Microsoft Access Driver (*.mdb)}; Dbq=Ind_NRDB.mdb; DefaultDir=C:\\CBMSDatabase\\System\\Output;");
	pOutDB = new CStatSimConn(sConn);

	int* eArray = pProcSelDlg->elementIDArray;
	int nE = pProcSelDlg->nElement;
	BOOL hhDone = FALSE, wHH = pProcSelDlg->wHH;

	//modify hh core ind
	sSQL = "ALTER TABLE `hh_CoreInd` MODIFY `hcn_NRDB` int(6);";
	pGlobalConn->ExecuteSQL(sSQL, FALSE);
	
	sSQL = _T("ALTER TABLE `hh_CoreInd` MODIFY `hcn_NRDB` varchar(6);");
	pGlobalConn->ExecuteSQL(sSQL, FALSE);

	ELEMENT elt; 	
	int _nExport = nE;

	for (int i=0; i<nE; i++) {
		
		elt = eArray[i];
		pElt = new CStatSimElt(pGlobalConn, elt, TRUE);

		sTable.Format(_T("%s_coreind"), (CString) pElt->Attr(element));

		if (!TableExists(sTable, pGlobalConn)) {
			CString msg;
			msg = _T("The table '") + sTable + _T("' does not exist! This will be skipped.");
			AfxMessageBox(msg);
			_nExport--;
			continue;
		}

		pRS = new CStatSimRS( pGlobalConn, ConstChar(sTable));

		statusText.Format(_T("Exporting '%s' to 'Ind_NRDB.mdb'..."), sTable);
		g_wndStatusBarPane->SetText( (statusText));

		CString sSQL; sSQL.Format(_T("DELETE * FROM %s;"), sTable);
		pOutDB->ExecuteSQL(sSQL, FALSE);

		pOutDB->CreateTable(pRS, sTable, 
			FALSE, FALSE);
		pOutDB->InsertRecords(pRS, sTable, NULL, NULL, FALSE, TRUE);
		
		delete pElt; pElt = NULL;
	}

	CString msg;
	msg.Format(_T("Exported %d Core Indicator table(s) to 'C:\\CBMSDatabase\\System\\Output\\Ind_NRDB.mdb'"), _nExport);
	AfxMessageBox(msg);
	
	
	g_wndStatusBarPane->SetText(_T("Ready"));


}
void CMainFrame::OnFileExportMDG() 
{
	CString sTable, statusText, sSQL;
	CStatSimElt* pElt;
	CStatSimRS* pRS = NULL;
	int ectr = 0;

	CString sSrcPath = (CString) SSPATH + (CString) _T("\\Ind_NRDB.mdb");

	//copy first
	CreateDirectory(_T("C:\\CBMSDatabase\\System\\Output"), NULL);
	remove("C:\\CBMSDatabase\\System\\Output\\Ind_NRDB.mdb");
	bool fcnResult = FALSE;	
    CopyFile((sSrcPath), 
		_T("C:\\CBMSDatabase\\System\\Output\\Ind_NRDB.mdb"), fcnResult);

	g_wndStatusBarPane->SetText(_T("Select levels to process..."));
	CProcSelDlg* pProcSelDlg = new CProcSelDlg(HH-1, _T("StatSim: MDG Export"));
	if ( pProcSelDlg->DoModal()!=IDOK ) {
		g_wndStatusBarPane->SetText(_T("Processing cancelled"));
		return;
	}

	CStatSimConn* pOutDB;
	LPCTSTR sConn = _T ("Driver={Microsoft Access Driver (*.mdb)}; Dbq=Ind_NRDB.mdb; DefaultDir=C:\\CBMSDatabase\\System\\Output;");
	pOutDB = new CStatSimConn(sConn);

	int* eArray = pProcSelDlg->elementIDArray;
	int nE = pProcSelDlg->nElement;
	BOOL hhDone = FALSE, wHH = pProcSelDlg->wHH;

	ELEMENT elt; 	
	int _nExport = nE;
	for (int i=0; i<nE; i++) {
		
		elt = eArray[i];
		pElt = new CStatSimElt(pGlobalConn, elt, TRUE);

		sTable.Format(_T("%s_mdg"), (CString) pElt->Attr(element));

		if (!TableExists(sTable, pGlobalConn)) {
			CString msg;
			msg = _T("The table '") + sTable + _T("' does not exist! This will be skipped.");
			AfxMessageBox(msg);
			_nExport--;
			continue;
		}
		

		pRS = new CStatSimRS( pGlobalConn, ConstChar(sTable));

		statusText.Format(_T("Exporting '%s' to 'Ind_NRDB.mdb'..."), sTable);
		g_wndStatusBarPane->SetText( (statusText));

		CString sSQL; sSQL.Format(_T("DELETE * FROM %s;"), sTable);
		pOutDB->ExecuteSQL(sSQL, FALSE);

		pOutDB->CreateTable(pRS, sTable, 
			FALSE, FALSE);
		pOutDB->InsertRecords(pRS, sTable, NULL, NULL, FALSE, TRUE);
		
		delete pElt; pElt = NULL;
	}

	CString msg;
	msg.Format(_T("Exported %d MDG table(s) to 'C:\\CBMSDatabase\\System\\Output\\Ind_NRDB.mdb'"), _nExport);
	AfxMessageBox(msg);
	
	
	g_wndStatusBarPane->SetText(_T("Ready"));

}
void CMainFrame::OnFileExportCCI() 
{
	CString sTable, statusText, sSQL;
	CStatSimElt* pElt;
	CStatSimRS* pRS = NULL;
	int ectr = 0;

	CString sSrcPath = (CString) SSPATH + (CString) _T("\\Ind_NRDB.mdb");

	//copy first
	CreateDirectory(_T("C:\\CBMSDatabase\\System\\Output"), NULL);
	remove("C:\\CBMSDatabase\\System\\Output\\Ind_NRDB.mdb");
	bool fcnResult = FALSE;	
    CopyFile((sSrcPath), 
		_T("C:\\CBMSDatabase\\System\\Output\\Ind_NRDB.mdb"), fcnResult);

	g_wndStatusBarPane->SetText(_T("Select levels to process..."));
	CProcSelDlg* pProcSelDlg = new CProcSelDlg(HH, _T("StatSim: Export CCI"));
	if ( pProcSelDlg->DoModal()!=IDOK ) {
		g_wndStatusBarPane->SetText(_T("Processing cancelled"));
		return;
	}

	CStatSimConn* pOutDB;
	LPCTSTR sConn = _T ("Driver={Microsoft Access Driver (*.mdb)}; Dbq=Ind_NRDB.mdb; DefaultDir=C:\\CBMSDatabase\\System\\Output;");
	pOutDB = new CStatSimConn(sConn);

	int* eArray = pProcSelDlg->elementIDArray;
	int nE = pProcSelDlg->nElement;
	BOOL hhDone = FALSE, wHH = pProcSelDlg->wHH;

	ELEMENT elt;
	int _nExport = nE;
	for (int i=0; i<nE; i++) {
		
		elt = eArray[i];
		pElt = new CStatSimElt(pGlobalConn, elt, TRUE);
		sTable.Format(_T("%s_cci"), (CString) pElt->Attr(element));

		if (!TableExists(sTable, pGlobalConn)) {
			CString msg;
			msg = _T("The table '") + sTable + _T("' does not exist! This will be skipped.");
			AfxMessageBox(msg);
			_nExport--;
			continue;
		}
		
		if(elt!=HH) {
			sSQL.Format(_T("ALTER TABLE `%s_cci` MODIFY `%s_cci` FLOAT, MODIFY `%s_maxcci` INT(9), MODIFY `%s_mincci` INT(9), MODIFY `%s_sdcci` FLOAT;"),
				(CString) pElt->Attr(element),  (CString) pElt->Attr(element),  (CString) pElt->Attr(element),  (CString) pElt->Attr(element),  (CString) pElt->Attr(element));
			pGlobalConn->ExecuteSQL(sSQL, FALSE);
		}
		else {
			//modify hh core ind
			sSQL = "ALTER TABLE `hh_cci` ADD COLUMN `hcn_NRDB` int(6);";
			pGlobalConn->ExecuteSQL(sSQL, FALSE);
	
			sSQL = "UPDATE `hh_cci` SET `hcn_NRDB`=`hcn`;";
			pGlobalConn->ExecuteSQL(sSQL, FALSE);
			sSQL = "ALTER TABLE `hh_cci` MODIFY `hcn_NRDB` VARCHAR(6), MODIFY `hh_cci` int(2);";
			pGlobalConn->ExecuteSQL(sSQL, FALSE);
		}

		sTable.Format(_T("%s_CCI"), (CString) pElt->Attr(element));
		pRS = new CStatSimRS( pGlobalConn, ConstChar(sTable));

		statusText.Format(_T("Exporting '%s' to 'Ind_NRDB.mdb'..."), sTable);
		g_wndStatusBarPane->SetText( (statusText));

		sSQL.Format(_T("DELETE * FROM %s;"), sTable);
		pOutDB->ExecuteSQL(sSQL, FALSE);

		//AfxMessageBox(sSQL);
		pOutDB->CreateTable(pRS, sTable, 
			FALSE, FALSE);
		//AfxMessageBox(sSQL);
		pOutDB->InsertRecords(pRS, sTable, NULL, NULL, FALSE, TRUE);
		
		delete pElt; pElt = NULL;
	}

	CString msg;
	msg.Format(_T("Exported %d CCI table(s) to 'C:\\CBMSDatabase\\System\\Output\\Ind_NRDB.mdb'"), _nExport);
	AfxMessageBox(msg);
	
	
	g_wndStatusBarPane->SetText(_T("Ready"));


}

void CMainFrame::OnDataClearSelHH()
{
	if (!TableExists(_T("hpq_hh"), pGlobalConn))
	{
		AfxMessageBox(_T("Database is still empty"));
		return;
	}
	
	CClearSelHHDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnDataClearhh()
{
	BeginWaitCursor();

	ELEMENT elt = HH;
	CStatSimRS *pRS;
	CString sSQL, sSELClause, sTable, sClause, sESQL, msgText;
	sESQL.Format(_T("SELECT `elementID`, `element` FROM `~hElement` WHERE `elementID`>=%d;"), elt);
	pRS = new CStatSimRS( pGlobalConn, sESQL);

	
	msgText.Format(_T("Do you really want to clear the stored data inside the database?  After this, you will have to import them again.  Do you wish to continue?"));
	int msgResult = AfxMessageBox (msgText, MB_YESNO);
	
	if (msgResult!=IDYES) {
		return;
	}

	sSELClause = _T("DROP TABLE IF EXISTS ");
	
	if (pRS->GetRecordCount()>0) {
		pRS->MoveFirst();
		for (int i=0; i<pRS->GetRecordCount(); i++) {
			
			sTable = _T("`") + pRS->SQLFldValue(_MBCS("element")) + _T("`");
			sSQL = sSELClause + sTable + _T(";");

			pGlobalConn->ExecuteSQL(sSQL);

			if (i==0)
				sClause = sTable; 
			else
				sClause += _T(", ") + sTable; 

			sTable = "";
			pRS->MoveNext();
		}
	}

	msgText.Format(_T("%d items were cleared.  \n\nPopulation will be updated on next import."), pRS->GetRecordCount());
	
	EndWaitCursor();	
	
	AfxMessageBox(msgText);
}



void CMainFrame::OnDataUpdate()
{
/*	CProcSelIndDlg *pDlg;
	pDlg = new CProcSelIndDlg();
	pDlg->DoModal();
*/

	ConUpElt(g_askBasicInd);	
}
void CMainFrame::OnDataProcOKI()
{
	ProcOKI();	
}
void CMainFrame::OnDataProcCCRI()
{
	ProcCCRI();	
}
void CMainFrame::OnDataProcBPQ()
{
	ProcBPQTabs();	
}
void CMainFrame::OnDataProcMDG()
{
/*	CProcSelIndDlg *pDlg;
	pDlg = new CProcSelIndDlg();
	pDlg->DoModal();
*/
	ProcMDG();	
}
void CMainFrame::PutSubElt(CTreeCtrl& pTree, DWORD id, HTREEITEM hParent)
{
	CString sSQL;
	CStatSimRS *pRS;
	
	sSQL.Format(_T("SELECT * FROM `~hElement` WHERE `indID`=%d AND `etype`=3 ORDER by `elementid`;"), id );
	pRS = new CStatSimRS( pGlobalConn, sSQL);
	int nElt = pRS->GetRecordCount();
	if (nElt>0)
		pRS->MoveFirst();
	
	for ( int j=0; j<nElt; j++ ) {
		TVINSERTSTRUCT tvElt;
		tvElt.hParent = hParent;
		tvElt.hInsertAfter = NULL;
		tvElt.item.mask = TVIF_TEXT|TVIF_PARAM|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
		CString sTxt( pRS->SQLFldValue( _MBCS("label") ) );
		tvElt.item.pszText = sTxt.GetBuffer();
		DWORD eltID = _ttol((pRS->SQLFldValue( _MBCS("elementID") )));
		tvElt.item.lParam = eltID;

		tvElt.item.iImage = m_EltImage;
		tvElt.item.iSelectedImage = m_EltImage;
		HTREEITEM elt = pTree.InsertItem(&tvElt);	
		pRS->MoveNext();

	}

}

void CMainFrame::PopulateIndTree(CTreeCtrl& pTree)
{

	CString sSQL;
	CStatSimRS *pCatRS = 0, *pSecRS = 0, *pFigRS = 0, *pEltRS = 0;

	sSQL = "SELECT * FROM `~Ind` WHERE `indtype`=1 ORDER by `indID`;";
	pCatRS = new CStatSimRS( pGlobalConn, sSQL);
	int nCat = pCatRS->GetRecordCount();
	if (nCat>0)
		pCatRS->MoveFirst();
	
	int i, j, k;

	for ( i=0; i<nCat; i++ ) {	

		TVINSERTSTRUCT tvCat;
		tvCat.hParent = NULL;
		tvCat.hInsertAfter = NULL;
		tvCat.item.mask = TVIF_TEXT|TVIF_PARAM|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
		CString sTxt( pCatRS->SQLFldValue( _MBCS("label") ) );
		tvCat.item.pszText = sTxt.GetBuffer();
		DWORD catID = _ttol((pCatRS->SQLFldValue( _MBCS("indID") )));
		tvCat.item.lParam = catID;

		tvCat.item.iImage = m_IndImage;
		tvCat.item.iSelectedImage = m_IndImage;
		HTREEITEM cat = pTree.InsertItem(&tvCat);
		
		// sub elements
		PutSubElt(pTree, catID, cat);

		sSQL.Format(_T("SELECT * FROM `~Ind` WHERE parID=%d AND (`indtype`=2);"), catID );
		pSecRS = new CStatSimRS( pGlobalConn, sSQL);

		int nSec = pSecRS->GetRecordCount();
		if (nSec>0)
			pSecRS->MoveFirst();

		for ( int j=0; j<nSec; j++ ) {
			TVINSERTSTRUCT tvSec;
			tvSec.hParent = cat;
			tvSec.hInsertAfter = NULL;
			tvSec.item.mask = TVIF_TEXT|TVIF_PARAM|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
			CString sTxt( pSecRS->SQLFldValue( _MBCS("label") ) );
			tvSec.item.pszText = sTxt.GetBuffer();
			DWORD secID = _ttol((pSecRS->SQLFldValue( _MBCS("indID") )));
			tvSec.item.lParam = secID;

			tvSec.item.iImage = m_IndImage;
			tvSec.item.iSelectedImage = m_IndImage;

			HTREEITEM sec = pTree.InsertItem(&tvSec);	
	
			// sub elements
			PutSubElt(pTree, secID, sec);

			sSQL.Format(_T("SELECT * FROM `~Ind` WHERE parID=%d AND `indtype`=3 ORDER by `indid`;"), secID);
			pFigRS = new CStatSimRS( pGlobalConn, sSQL);
			
			int nFig = pFigRS->GetRecordCount();
			if (nFig>0)
				pFigRS->MoveFirst();
			
			for ( int k=0; k<nFig; k++ ) {
				TVINSERTSTRUCT tvFig;
				tvFig.hParent = sec;
				tvFig.hInsertAfter = NULL;
				tvFig.item.mask = TVIF_TEXT|TVIF_PARAM|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
				CString sTxt( pFigRS->SQLFldValue( _MBCS("label") ) );
				tvFig.item.pszText = sTxt.GetBuffer();
				DWORD figID = _ttol((pFigRS->SQLFldValue( _MBCS("indID") )));
				tvFig.item.lParam = figID;

				tvFig.item.iImage = m_IndImage;
				tvFig.item.iSelectedImage = m_IndImage;
				HTREEITEM fig = pTree.InsertItem(&tvFig);
				
				// sub elements
				PutSubElt(pTree, figID, fig);

				pFigRS->MoveNext();

			}

			delete pFigRS; pFigRS = 0;
			pSecRS->MoveNext();

		}
		
		delete pSecRS; pSecRS = 0;
		pCatRS->MoveNext();
	
	}
	
	delete pCatRS; pCatRS = 0;


}

void CMainFrame::PopulateTableTree(CTreeCtrl& pTree)
{
	CString sSQL;
	CStatSimRS *pRS = 0, *pSubRS = 0;

	sSQL.Format(_T("SHOW TABLES WHERE LEFT(`tables_in_%s`, 1)<>'~';"), sDB);
	pRS = new CStatSimRS(pGlobalConn, sSQL);
	
	int nTables = pRS->GetRecordCount();
	if (nTables>0)
		pRS->MoveFirst();

	int i, j;

	for ( i=0; i<nTables; i++ ) {	

		CString sCol;
		sCol.Format(_T("tables_in_%s"), sDB);
		TVINSERTSTRUCT tvInsert;
		tvInsert.hParent = NULL;
		tvInsert.hInsertAfter = NULL;
		tvInsert.item.mask = TVIF_TEXT;
		CString sTable = pRS->SQLFldValue( ConstChar(sCol) );
		tvInsert.item.pszText = sTable.GetBuffer() ;
		 
		HTREEITEM table = pTree.InsertItem(&tvInsert);
		
		sSQL.Format(_T("SHOW COLUMNS IN `%s`;"), sTable);
		pSubRS = new CStatSimRS(pGlobalConn, sSQL);
		int nCols = pSubRS->GetRecordCount();
		if (nCols>0)
			pSubRS->MoveFirst();

		for ( j=0; j<nCols; j++ ) {
			TVINSERTSTRUCT tvSubInsert;
			tvSubInsert.hParent = table;
			tvSubInsert.hInsertAfter = NULL;
			tvSubInsert.item.mask = TVIF_TEXT;
			CString sCol = pSubRS->SQLFldValue( _MBCS("field") );
			tvSubInsert.item.pszText = sCol.GetBuffer();
			HTREEITEM table = pTree.InsertItem(&tvSubInsert);
			
			pSubRS->MoveNext();
		}

		delete pSubRS; pSubRS = 0;

		pRS->MoveNext();
	
	}

	delete pRS; pRS = 0;
}

void CMainFrame::DisplayInd()
{
	if(!TableExists(_T("hpq_hh"), pGlobalConn) )
	{
		AfxMessageBox(_T("Basic data (household) does not exist.  Please import first"));
		return;
	}

	CStatSimInd* pSSInd;
	CStatSimElt* pSSElt;
	CStatSimRS* pRS = 0;
	CStatSimHTML* pHTML;
	CString sSQL, sCrit, 
		sTitle, sSubTitle;

	int units, iSelImage, iStateImage;
	DWORD id, indID;

	HTREEITEM tv = m_wndTree.GetSelectedItem();
	id = m_wndTree.GetItemData(tv);
	m_wndTree.GetItemImage(tv, iSelImage, iStateImage);
	
	if (iSelImage==m_EltImage) {
		HTREEITEM ptv = m_wndTree.GetParentItem(tv);
		indID = m_wndTree.GetItemData(ptv);
		theApp.OnFileNew();

	}
	else {

		return; //muna pero dapat load selind dlg
	}

	pSSElt = new CStatSimElt(pGlobalConn, id, TRUE);
	CString sElt(pSSElt->ParAttr(element));
	//AfxMessageBox(pSSElt->ParAttr(element));
	//build indicators
	pSSInd = new CStatSimInd(pGlobalConn, FIG, indID, id);

	pSSInd->BuildInd();

	//build report
	CreateDirectory(_T("C:\\CBMSDatabase\\System\\Reports"), NULL);
	CreateDirectory(_T("C:\\CBMSDatabase\\System\\Reports\\Temp"), NULL);
	g_sRptPath = _T("C:\\CBMSDatabase\\System\\Reports\\Temp\\StatSimRpt.htm");

	FILE* pFile = _tfopen(g_sRptPath, _T("w"));
	sCrit = pSSInd->sIndTable().Right(7);

	/*
	///test
	sSQL = _T("SELECT * FROM `hpq_mem`;");
	pRS = new CStatSimRS(pGlobalConn, sSQL);
	pHTML = new CStatSimHTML(pRS, pFile);
	//pHTML->DrawFDxT(_T("StatSimPro 5"), _T("Crosstabs"), _T("age_yr"), _T("civstat"),
	//	5, 10, 150);
	pHTML->DrawXTab(_T("StatSimPro 5"), _T("Crosstabs"), _T("g_occ"), _T("civstat"));
	delete pRS; pRS = 0;
	fclose(pFile);
	g_pSSHTMLView->Navigate2(ConstChar(g_sRptPath), NULL, NULL);
	return;
	////////////////
	*/


	if (sCrit=="totwage") {
		units = 1;
	}
	else {
		units = 100;
	}

	//if (pHTML->GetError()==SQL_ERROR){	//cancel draw
	//	fclose(pFile);
	//	return;
	//}

	//partials
/*	if ( id>=PARTIAL_CORE_START && id<=PARTIAL_CORE_END ) {
		
		pHTML = new CStatSimHTML(pRS, pFile);

		//output all the 14
		for (int i=0; i<N_COREIND; i++) {
			sSQL.Format(_T("SELECT * FROM `%s%s`;"), pSSElt->Attr(element), sHHCoreInd[i][VAR]);
			pRS = new CStatSimRS(pGlobalConn, sSQL);	
			
			if (pRS->GetRecordCount()>0) {
				pHTML->SetRS(pRS);
				sTitle = _T("CBMS StatSim - ") + pSSInd->sIndLabel();
				sSubTitle.Format(_T("%s - %s (%d of %d)"), pSSInd->sIndDesc(), sHHCoreInd[i][LABEL], i+1, N_COREIND);
				pHTML->DrawGeneric(sTitle, sSubTitle);
			}

			delete pRS; pRS = 0;

		}

		fclose(pFile);
		g_pSSHTMLView->Navigate2(ConstChar(g_sRptPath), NULL, NULL);
		return;

	}

	else if (id>=PARTIAL2_CORE_START && id<=PARTIAL2_CORE_END) {
	
		pHTML = new CStatSimHTML(pGlobalConn, pFile, indID, id);

		if (id>=3309 && id<=3312)
			pHTML->DrawPartial2HealthEduc(DEATH05, pSSInd->sIndLabel(), pSSInd->sIndDesc());
		else if (id>=3313 && id<=3316) 
			pHTML->DrawPartial2HealthEduc(DEATHPREG, pSSInd->sIndLabel(), pSSInd->sIndDesc());
		else if (id>=3317 && id<=3320) 
			pHTML->DrawPartial2HealthEduc(MALN05, pSSInd->sIndLabel(), pSSInd->sIndDesc());
		else if (id>=3321 && id<=3324) 
			pHTML->DrawPartial2HouseFac(MSH, pSSInd->sIndLabel(), pSSInd->sIndDesc());
		else if (id>=3325 && id<=3328) 
			pHTML->DrawPartial2HouseFac(SQUAT, pSSInd->sIndLabel(), pSSInd->sIndDesc());
		else if (id>=3329 && id<=3332) 
			pHTML->DrawPartial2HouseFac(NTSWS, pSSInd->sIndLabel(), pSSInd->sIndDesc());
		else if (id>=3333 && id<=3336) 
			pHTML->DrawPartial2HouseFac(NTSTF, pSSInd->sIndLabel(), pSSInd->sIndDesc());
		else if (id>=3337 && id<=3340) 
			pHTML->DrawPartial2HealthEduc(NTELEM612, pSSInd->sIndLabel(), pSSInd->sIndDesc());
		else if (id>=3341 && id<=3344) 
			pHTML->DrawPartial2HealthEduc(NTHS1316, pSSInd->sIndLabel(), pSSInd->sIndDesc());
		else if (id>=3345 && id<=3348)
			pHTML->DrawPartialILES(pSSInd->sIndLabel(), pSSInd->sIndDesc());
		else if (id>=3349 && id<=3352)
			pHTML->DrawPartialEmpl(pSSInd->sIndLabel(), pSSInd->sIndDesc());
		else if (id>=3353 && id<=3356)
			pHTML->DrawPartialEmpl(pSSInd->sIndLabel(), pSSInd->sIndDesc(), FALSE);
		else if (id>=3357 && id<=3360)
			pHTML->DrawPartialProg(pSSInd->sIndLabel(), pSSInd->sIndDesc(), FALSE);
		else if (id>=3361 && id<=3364)
			pHTML->DrawPartialProg(pSSInd->sIndLabel(), pSSInd->sIndDesc());
		
		fclose(pFile);
		
		g_pSSHTMLView->Navigate2(ConstChar(g_sRptPath), NULL, NULL);

		return;
	
	}

	else if (id>=PCIQUINTILE_CORE_START && id<=PCIQUINTILE_CORE_END) {
		
		pHTML = new CStatSimHTML(pGlobalConn, pSSInd->sIndTable(), pFile, indID, id);

		sTitle = "CBMS StatSim Partial Core Indicators Report";
		sSubTitle.Format(_T("The 14 indicators given income distribution"));
		pHTML->DrawGeneric(sTitle, sSubTitle);

		fclose(pFile);
		g_pSSHTMLView->Navigate2(ConstChar(g_sRptPath), NULL, NULL);
		return;
	}

	else if ( id>=TARGETING_START && id<=TARGETING_END ) {
		pHTML = new CStatSimHTML(pGlobalConn, ConstChar( pSSInd->sIndTable() ), pFile, indID, id);
		pHTML->DrawGeneric(pSSInd->sIndLabel(), pSSInd->sIndDesc());
		fclose(pFile);
		g_pSSHTMLView->Navigate2(ConstChar(g_sRptPath), NULL, NULL);
		return;
	}
*/	
	//templates
	if (id>=IND_CDP21_START && id<=IND_CDP21_END) {
		if (!TableExists(_T("mem_ind"), pGlobalConn)) {
			AfxMessageBox(_T("Processed information is needed to do display this report.  Run processing first."));
			return;
		}
		pHTML = new CStatSimHTML(pGlobalConn, pFile);	//generic or core
		pHTML->SetQnrID(hpq_id);
		pHTML->DrawCDP21(id, pSSInd->sIndLabel(), pSSInd->sIndDesc());
		//pHTML->DrawXTab(pSSInd->sIndLabel(), _T("civstat"),_T("sex"));		

		//std::map<int, CString> sTables;
		//sTables[IND_CDP21_PROV]
		//pHTML->Construct(pGlobalConn, _T("brgy_totpop"), pFile, 2001, 30004);
		//pHTML->DrawTotPop(pSSInd->sIndLabel(), _T("Population"));
		fclose(pFile);
		g_pSSHTMLView->Navigate2((g_sRptPath), NULL, NULL);
		return;
		
	}
	if (id>=IND_CDP22_START && id<=IND_CDP22_END) {

		if (!TableExists(_T("mem_ind"), pGlobalConn)) {
			AfxMessageBox(_T("Processed information is needed to do display this report.  Run processing first."));
			return;
		}

		pHTML = new CStatSimHTML(pGlobalConn, pFile);	//generic or core
		pHTML->SetQnrID(hpq_id);	//set questionnaire ID
		pHTML->DrawCDP22(id, pSSInd->sIndLabel(), pSSInd->sIndDesc());

		fclose(pFile);
		g_pSSHTMLView->Navigate2((g_sRptPath), NULL, NULL);
		return;
		
	}

	if (id>=IND_CDP23_START && id<=IND_CDP23_END) {

		if (!TableExists(_T("mem_ind"), pGlobalConn)) {
			AfxMessageBox(_T("Processed information is needed to do display this report.  Run processing first."));
			return;
		}

		pHTML = new CStatSimHTML(pGlobalConn, pFile);	//generic or core
		pHTML->SetQnrID(hpq_id);
		pHTML->DrawCDP23(id, pSSInd->sIndLabel(), pSSInd->sIndDesc());

		fclose(pFile);
		g_pSSHTMLView->Navigate2((g_sRptPath), NULL, NULL);
		return;
		
	}

	if (id>=IND_CDP24_START && id<=IND_CDP24_END) {

		CString sTable, sMsg; 
		
		pHTML = new CStatSimHTML(pGlobalConn, pFile);
		pHTML->SetQnrID(hpq_id);

		sTable.Format(_T("%s_wf_type"), sElt);
		if (!TableExists((sTable), pGlobalConn)) {
			sMsg = sTable + _T(" is needed in this report.  Please process BPQ tables.");
			AfxMessageBox(sMsg);
			//return;
		}
		else {
			pRS = new CStatSimRS(pGlobalConn, ConstChar(sTable));
			pHTML->SetRS(pRS);
			pHTML->DrawGeneric(pSSInd->sIndLabel(), pSSInd->sIndDesc());
			delete pRS; pRS=0;
		}


		sTable.Format(_T("%s_es_type"), sElt);
		if (!TableExists((sTable), pGlobalConn)) {
			sMsg = sTable + _T(" is needed in this report.  Please process BPQ tables.");
			AfxMessageBox(sMsg);
			//return;
		}
		else {
			pRS = new CStatSimRS(pGlobalConn, ConstChar(sTable));
			pHTML->SetRS(pRS);
			pHTML->DrawGeneric(pSSInd->sIndLabel(), pSSInd->sIndDesc());
			delete pRS; pRS=0;
		}

		sTable.Format(_T("%s_elec_src"), sElt);
		if (!TableExists((sTable), pGlobalConn)) {
			sMsg = sTable + _T(" is needed in this report.  Please process other key indicators.");
			AfxMessageBox(sMsg);
			//return;
		}
		else {
			pRS = new CStatSimRS(pGlobalConn, ConstChar(sTable));
			pHTML->SetRS(pRS);
			pHTML->DrawGeneric(pSSInd->sIndLabel(), pSSInd->sIndDesc());
			delete pRS; pRS=0;
		}

		//pHTML = new CStatSimHTML(pGlobalConn, pFile, indID, id);	//generic or core
		//pRS = new CStatSimRS(pGlobalConn, ConstChar(sTable));
		//pHTML->SetRS(pRS);
		//pHTML->DrawByCat(pSSInd->sIndLabel(), pSSInd->sIndDesc(), _T(""), _T(""), _T(""));

		fclose(pFile);
		g_pSSHTMLView->Navigate2((g_sRptPath), NULL, NULL);
		delete pRS; pRS=0;
		return;
		
	}
	if (id>=IND_CDP25_START && id<=IND_CDP25_END) {

		CString sTable, sMsg; 
		
		pHTML = new CStatSimHTML(pGlobalConn, pFile);
		pHTML->SetQnrID(hpq_id);

		sTable.Format(_T("%s_gd_type"), sElt);
		if (!TableExists((sTable), pGlobalConn)) {
			sMsg = sTable + _T(" is needed in this report.  Please process BPQ tables.");
			AfxMessageBox(sMsg);
			//return;
		}
		else {
			pRS = new CStatSimRS(pGlobalConn, ConstChar(sTable));
			pHTML->SetRS(pRS);
			pHTML->DrawGeneric(pSSInd->sIndLabel(), pSSInd->sIndDesc());
			delete pRS; pRS=0;
		}
		
		fclose(pFile);
		g_pSSHTMLView->Navigate2((g_sRptPath), NULL, NULL);
		delete pRS; pRS=0;
		return;

	}

	pHTML = new CStatSimHTML(pGlobalConn, ConstChar( pSSInd->sIndTable() ), pFile, indID, id);

	if (id>=IND_CORE_START && id<=IND_CORE_END) {
		//CString msg; msg = pSSInd->sIndLabel() + " and " +pSSInd->sIndDesc();
		//AfxMessageBox(msg);
		pHTML->SetQnrID(hpq_id);
		pHTML->DrawCore(pSSInd->sIndLabel(), pSSInd->sIndDesc());
	}
	else if (id==IND_COREIND_HH) {
		pHTML->DrawGeneric(pSSInd->sIndLabel(), pSSInd->sIndDesc());
	}
	else if (id>=IND_MDG_START && id<=IND_MDG_END) {
		pHTML->SetQnrID(hpq_id);
		pHTML->DrawMDG(pSSInd->sIndLabel(), pSSInd->sIndDesc());
	}
	else if (id>=IND_CCI_START && id<=IND_CCI_END) {
		pHTML->DrawGeneric(pSSInd->sIndLabel(), pSSInd->sIndDesc());
	}
	else if (id>=IND_TOTPOP_START && id<=IND_TOTPOP_END) {
		//CString msg; msg = pSSInd->sIndLabel() + " and " +pSSInd->sIndDesc();
		//AfxMessageBox(msg);
		pHTML->DrawTotPop(pSSInd->sIndLabel(), _T("")); //pSSInd->sIndDesc());
	}
	else if (id>=IND_DEMOG_START && id<=IND_DEMOG_END) {
		pHTML->DrawGeneric(pSSInd->sIndLabel(), pSSInd->sIndDesc());
	}
	else if ( id>=IND_READY_START && id<=IND_READY_END ) {

		if (pSSInd->WithSex()) {
			if (pSSInd->WithCat()) {
				pHTML->DrawByCatSex(pSSInd->sIndLabel(), pSSInd->sIndDesc(), _T(""), _T(""), _T(""));
			}
			else {
				pHTML->DrawByGroupSex(pSSInd->sIndLabel(), pSSInd->sIndDesc(), _T(""), _T(""), _T(""), units);

				if (id>=30024 && id<=30028) {
					sSQL = _T("SELECT `mnutind`, `sex` FROM `hpq_mem` where `age_yr`<=5;");
					pRS = new CStatSimRS(pGlobalConn, sSQL);
					pHTML->SetRS(pRS);
					pHTML->DrawXTab(TRUE, (CString) sTitle, (CString) sSubTitle, "mnutind", "sex");
					delete pRS; pRS = 0;
				}
				else if (id>=30029 && id<=30033) {
					if (TableExists(_T("hpq_death"), pGlobalConn)) {	
						sSQL = _T("SELECT * FROM `hpq_death` where `mdeadage`<=4;");
						pRS = new CStatSimRS(pGlobalConn, sSQL);
						pHTML->SetRS(pRS);
						pHTML->DrawXTab(TRUE, (CString) sTitle, (CString) sSubTitle, "mdeady", "mdeadsx");
						delete pRS; pRS = 0;
					}
				}
				else if ((id>=30059 && id<=30073)||(id>=30365 && id<=30379)) {
					if (id>=30059 && id<=30063) {
						sSQL = _T("SELECT `sex`, `educind`, `age_yr` FROM `hpq_mem` where `age_yr`>=6 and `age_yr`<=11;");
						sSubTitle = "Tabulation of children 6-11 by sex and school attendance";
					}
					else if (id>=30064 && id<=30068) {
						sSQL = _T("SELECT `sex`, `educind`, `age_yr` FROM `hpq_mem` where `age_yr`>=12 and `age_yr`<=15;");
						sSubTitle = "Tabulation of children 12-15 by sex and school attendance";
					}
					else if (id>=30069 && id<=30073){
						sSQL = _T("SELECT `sex`, `educind`, `age_yr` FROM `hpq_mem` where `age_yr`>=6 and `age_yr`<=15;");
						sSubTitle = "Tabulation of children 6-15 by sex and school attendance";
					}
					else if (id>=30365 && id<=30369) {
						sSQL = _T("SELECT `sex`, `educind`, `age_yr` FROM `hpq_mem` where `age_yr`>=6 and `age_yr`<=12;");
						sSubTitle = "Tabulation of children 6-12 by sex and school attendance";
					}
					else if (id>=30370 && id<=30374) {
						sSQL = _T("SELECT `sex`, `educind`, `age_yr` FROM `hpq_mem` where `age_yr`>=13 and `age_yr`<=16;");
						sSubTitle = "Tabulation of children 13-16 by sex and school attendance";
					}
					else if (id>=30375 && id<=30379) {
						sSQL = _T("SELECT `sex`, `educind`, `age_yr` FROM `hpq_mem` where `age_yr`>=6 and `age_yr`<=16;");
						sSubTitle = "Tabulation of children 6-16 by sex and school attendance";
					}

					pRS = new CStatSimRS(pGlobalConn, sSQL);
					pHTML->SetRS(pRS);
					pHTML->DrawXTab(TRUE, (CString) sTitle, (CString) sSubTitle, "sex", "educind");
					delete pRS; pRS = 0;

					if (id>=30059 && id<=30063) {
						sSQL = _T("SELECT `sex`, `gradel`, `age_yr` FROM `hpq_mem` where `age_yr`>=6 and `age_yr`<=11 and `educind`=1;");
						sSubTitle = "Tabulation of children 6-11 attending school, by sex and grade level";
					}
					else if (id>=30064 && id<=30068) {
						sSQL = _T("SELECT `sex`, `gradel`, `age_yr` FROM `hpq_mem` where `age_yr`>=12 and `age_yr`<=15 and `educind`=1;");
						sSubTitle = "Tabulation of children 12-15 attending school, by sex and grade level";
					}
					else if (id>=30069 && id<=30073) {
						sSQL = _T("SELECT `sex`, `gradel`, `age_yr` FROM `hpq_mem` where `age_yr`>=6 and `age_yr`<=15 and `educind`=1;");
						sSubTitle = "Tabulation of children 6-15 attending school, by sex and grade level";
					}
					else if (id>=30365 && id<=30369) {
						sSQL = _T("SELECT `sex`, `gradel`, `age_yr` FROM `hpq_mem` where `age_yr`>=6 and `age_yr`<=12 and `educind`=1;");
						sSubTitle = "Tabulation of children 6-12 attending school, by sex and grade level";
					}
					else if (id>=30370 && id<=30374) {
						sSQL = _T("SELECT `sex`, `gradel`, `age_yr` FROM `hpq_mem` where `age_yr`>=13 and `age_yr`<=16 and `educind`=1;");
						sSubTitle = "Tabulation of children 13-16 attending school, by sex and grade level";
					}
					else if (id>=30375 && id<=30379) {
						sSQL = _T("SELECT `sex`, `gradel`, `age_yr` FROM `hpq_mem` where `age_yr`>=6 and `age_yr`<=16 and `educind`=1;");
						sSubTitle = "Tabulation of children 6-16 attending school, by sex and grade level";
					}

					pRS = new CStatSimRS(pGlobalConn, sSQL);
					pHTML->SetRS(pRS);
					pHTML->DrawXTab(TRUE, (CString) sTitle, (CString) sSubTitle, "gradel", "sex");
					delete pRS; pRS = 0;
				}
				else if (id>=30094 && id<=30103) {
					sSQL = _T("SELECT `sex`, `jobind`, `age_yr`  FROM `hpq_mem` where `age_yr`>=15;");
					pRS = new CStatSimRS(pGlobalConn, sSQL);
					pHTML->SetRS(pRS);
					sSubTitle = "Tabulation of persons 15 years old and above by sex and job indicator";
					pHTML->DrawXTab(TRUE, (CString) sTitle, (CString) sSubTitle, "sex", "jobind");
					delete pRS; pRS = 0;

					sSQL = _T("SELECT `sex`, `fjob`, `age_yr`  FROM `hpq_mem` where `age_yr`>=15 and `jobind`=2;");
					pRS = new CStatSimRS(pGlobalConn, sSQL);
					pHTML->SetRS(pRS);
					sSubTitle = "Tabulation of non-working persons 15 years old and above by sex and finding job";
					pHTML->DrawXTab(TRUE, (CString) sTitle, (CString) sSubTitle, "sex", "fjob");
					delete pRS; pRS = 0;

					if (hpq_id==1020070400 || hpq_id==120070300 || hpq_id==120110100 || hpq_id==1020100100) {
						sSQL = _T("SELECT `sex`, `ynotlookjob`, `lastlookjob` FROM `hpq_mem` where `age_yr`>=15 and `jobind`=2 and `fjob`=2;");
						pRS = new CStatSimRS(pGlobalConn, sSQL);
						pHTML->SetRS(pRS);
						sSubTitle = "Tabulation of non-working persons 15 years old and above who are not looking for work by reason for not looking for work and last time find a job";
						pHTML->DrawXTab(TRUE, (CString) sTitle, (CString) sSubTitle, "ynotlookjob", "lastlookjob");
						delete pRS; pRS = 0;
						
						sSQL = _T("SELECT `sex`, `joppind`, `wtwind` FROM `hpq_mem` where `age_yr`>=15 and `jobind`=2 and `fjob`=2 and `ynotlookjob`>=2 and `ynotlookjob`<=5;");
						pRS = new CStatSimRS(pGlobalConn, sSQL);
						pHTML->SetRS(pRS);
						sSubTitle = "Tabulation of non-working persons 15 years old and above who are not looking for work due to reasons (2,3,4,5) by opportunity and willingness";
						pHTML->DrawXTab(TRUE, (CString) sTitle, (CString) sSubTitle, "joppind", "wtwind");
						delete pRS; pRS = 0;
					}

				}
			}
		}
		else {
			if (pSSInd->WithCat()) {
				pHTML->DrawByCat(pSSInd->sIndLabel(), pSSInd->sIndDesc(), _T(""), _T(""), _T(""));
			}
			else {
				pHTML->DrawByGroup(pSSInd->sIndLabel(), pSSInd->sIndDesc(), _T(""), _T(""), _T(""));

				if (id>=30034 && id<=30038) {
					if (TableExists(_T("hpq_death"), pGlobalConn)) {	
						sSQL = _T("SELECT * FROM `hpq_death` where `mdeadage`>=15 and `mdeadage`<=49;");
						pRS = new CStatSimRS(pGlobalConn, sSQL);
						pHTML->SetRS(pRS);
						pHTML->DrawXTab(TRUE, (CString) sTitle, (CString) sSubTitle, "mdeady", "mdeadsx");
						delete pRS; pRS = 0;
					}
				}
				else if (id>=30039 && id<=30043) {
					sSQL = _T("SELECT `tenur`, `urb` FROM `hpq_hh`;");
					pRS = new CStatSimRS(pGlobalConn, sSQL);
					pHTML->SetRS(pRS);
					pHTML->DrawXTab(TRUE, (CString) sTitle, (CString) sSubTitle, "tenur", "urb");
					delete pRS; pRS = 0;
				}
				else if (id>=30044 && id<=30048) {
					sSQL = _T("SELECT `wall`, `roof` FROM `hpq_hh`;");
					pRS = new CStatSimRS(pGlobalConn, sSQL);
					pHTML->SetRS(pRS);
					pHTML->DrawXTab(TRUE, (CString) sTitle, (CString) sSubTitle, "wall", "roof");
					delete pRS; pRS = 0;
				}
				else if (id>=30049 && id<=30053) {
					sSQL = _T("SELECT `water`, `urb` FROM `hpq_hh`;");
					pRS = new CStatSimRS(pGlobalConn, sSQL);
					pHTML->SetRS(pRS);
					pHTML->DrawXTab(TRUE, (CString) sTitle, (CString) sSubTitle, "water", "urb");
					delete pRS; pRS = 0;
				}
				else if (id>=30054 && id<=30058) {
					sSQL = _T("SELECT `toil`, `urb` FROM `hpq_hh`;");
					pRS = new CStatSimRS(pGlobalConn, sSQL);
					pHTML->SetRS(pRS);
					pHTML->DrawXTab(TRUE, (CString) sTitle, (CString) sSubTitle, "toil", "urb");
					delete pRS; pRS = 0;
				}
				else if (id>=30079 && id<=30088) {
					sSQL = _T("SELECT `mun`, `brgy`, `purok`, `hcn` FROM `hpq_hh` where `totin` is null;");
					pRS = new CStatSimRS(pGlobalConn, sSQL);
					pHTML->SetRS(pRS);
					sSubTitle.Format(_T("Households with missing income: %d"), pRS->GetRecordCount());
					pHTML->DrawGeneric((CString) sTitle, (CString) sSubTitle);
					delete pRS; pRS = 0;
				}
				else if (id>=30089 && id<=30093) {
					sSQL = _T("SELECT `fshort`, `urb` FROM `hpq_hh`;");
					pRS = new CStatSimRS(pGlobalConn, sSQL);
					pHTML->SetRS(pRS);
					pHTML->DrawXTab(TRUE, (CString) sTitle, (CString) sSubTitle, "fshort", "urb");
					delete pRS; pRS = 0;
				}


			}
		}
	}

	else {
		pHTML->DrawGeneric(pSSInd->sIndLabel(), pSSInd->sIndDesc());
	}
	
	fclose(pFile);

	g_pSSHTMLView->Navigate2((g_sRptPath), NULL, NULL);
}

void CMainFrame::OnStatsExeSQL() 
{
	
	CExeSQLDlg *pDlg = new CExeSQLDlg();
	pDlg->DoModal();
	
}

void CMainFrame::OnStatsExeCMD() 
{
	//AfxMessageBox(L"hello");
	CCmdDlg *pDlg = new CCmdDlg();
	pDlg->DoModal();
	
}

void CMainFrame::OnDataMatch()
{

	if (!TableExists(_T("hh_coreind"), pGlobalConn)) {
		AfxMessageBox(_T("Household information is not yet processed."));
		return;
	}

	CFileDialog dlg (TRUE, _T("mdb"),NULL, OFN_FILEMUSTEXIST, 
		_T("CBMS-NRDB MS Access Files (*.mdb)|*.mdb|")); 
	if (dlg.DoModal() == IDOK) {
		
		CString sFile, sCaption;
		sFile = dlg.GetPathName();

		CMergeDataDlg* pMDDlg = new CMergeDataDlg( ConstChar(sFile), this );
		pMDDlg->DoModal();

	}
}



void CMainFrame::OnFileExportData() 
{

	CSelDataDlg* pDlg = new CSelDataDlg();
	CStatSimRS* pRS = NULL;
	CString sSQL, sPath, sVal;

	if ( pDlg->DoModal() == IDOK) {
		
		pRS = new CStatSimRS( pGlobalConn );
		sSQL.Format(_T("SELECT * FROM `%s`;"), (CString) pDlg->m_sTable);
		
		if (pRS->RunSQL(sSQL)==SQL_ERROR){
			delete pRS; pRS= NULL;
			return;
		}
		
		sPath.Format(_T("C:\\CBMSDatabase\\System\\Output\\%s.csv"), (CString) pDlg->m_sTable);
		FILE* pFile = _tfopen( (sPath), _T("w"));

		if (pRS->GetRecordCount()>0) {
			pRS->MoveFirst();
		}
		
		//field names first
		for (int j=0; j<pRS->GetFieldCount(); j++) {
			fprintf( pFile, pRS->GetFieldName(j) );
			fprintf( pFile, _MBCS(",") );
		}
		_ftprintf( pFile, _T("\n") );
		
		//then the records
		for (int i=0; i<pRS->GetRecordCount(); i++) {			
			for (int j=0; j<pRS->GetFieldCount(); j++) {
				
				//handle commas
				sVal = pRS->SQLFldValue(j);
				sVal.Replace(_T(","), _T(";"));

				_ftprintf( pFile, ( sVal ) );
				_ftprintf( pFile, _T(",") );
			}
			_ftprintf( pFile, _T("\n") );
			pRS->MoveNext();
		}

		delete pRS; pRS = NULL;
		
		fclose(pFile);

	}

	ShellExecute(*this, _T("open"), sPath, NULL, NULL, SW_SHOWNORMAL);	
	
}

void CMainFrame::OnStatsCSProXTabHH() 
{

	LPCTSTR sXTabPath = _T("C:\\CBMSDatabase\\System\\Crosstab\\Core_HPQ.xtb");
	ShellExecute(*this, _T("open"), sXTabPath, NULL, NULL, SW_SHOWNORMAL);	
	
}

void CMainFrame::OnStatsCSProXTabBrgy() 
{

	LPCTSTR sXTabPath = _T("C:\\CBMSDatabase\\System\\Crosstab\\Core_BPQ.xtb");
	ShellExecute(*this, _T("open"), sXTabPath, NULL, NULL, SW_SHOWNORMAL );	
	
}
void CMainFrame::OnDataAttClean() 
{

	AfxMessageBox(_T("This will attempt to clean the data!"));
	CString sSQL, sFld, sSQLExec;
	
	CStatSimRS* pRS = 0;

	sFld.Format(_T("Tables_in_%s"), sDB);
	sSQL.Format(_T("SHOW TABLES WHERE LEFT(`%s`, 4) = 'hpq_';"), sFld );
	
	pRS = new CStatSimRS(pGlobalConn, sSQL);
	
	pRS->MoveFirst();
	
	for (int j=0; j<pRS->GetRecordCount(); j++) {

		sSQLExec.Format(_T("DELETE `%s` from `%s` left join `hpq_clean_hh` using (`prov`, `mun`, `brgy`, `purok`, `hcn`) WHERE size IS NULL;"), 
				pRS->SQLFldValue(ConstChar(sFld)),pRS->SQLFldValue(ConstChar(sFld)));
		
		pGlobalConn->ExecuteSQL(sSQLExec);
		pRS->MoveNext();
	}

	AfxMessageBox(_T("Data cleaned!"));
	
}

void CMainFrame::OnDataEditThresh() 
{
	CEditThreshDlg EditThreshDlg;
	EditThreshDlg.DoModal();
	
}

void CMainFrame::OnCallNRDB() 
{

	CFileFind FileFinder;
	LPCTSTR sPath1 = _T("C:\\Program Files (x86)\\Natural Resources Database\\nrdbpro.exe"),
		sPath2 = _T("C:\\Program Files\\Natural Resources Database\\nrdbpro.exe");

	if (FileFinder.FindFile(sPath1,0)) {
		ShellExecute(*this, _T("open"), sPath1, NULL, NULL, SW_SHOWNORMAL );
	}
	else if (FileFinder.FindFile(sPath2,0)) {
		ShellExecute(*this, _T("open"), sPath1, NULL, NULL, SW_SHOWNORMAL );
	}
	else {
		AfxMessageBox(_T("Cannot find NRDB Pro file."));
	}

}
void CMainFrame::OnCallEncode() 
{
	CString sEncanPath = (CString) SSPATH + (CString) _T("\\CBMSEncode.exe");
	//AfxMessageBox(sEncanPath);
	ShellExecute(*this, _T("open"), sEncanPath, NULL, NULL, SW_SHOWNORMAL );	
	
}

void CMainFrame::OnDataClearBPQ()
{
	CString sFld, sSQL, sSQLExec, msgText;
	msgText.Format(_T("Do you really want to clear the stored BPQ inside the database?  After this, you will have to import them again.  Do you wish to continue?"));
	int msgResult = AfxMessageBox (msgText, MB_YESNO);
	
	if (msgResult!=IDYES) {
		return;
	}

	CStatSimRS *pRS = 0;

	sFld.Format(_T("Tables_in_%s"), sDB);
	sSQL.Format(_T("SHOW TABLES WHERE LEFT(`%s`, 4) = 'bpq_';"), sFld );
	
	pRS = new CStatSimRS(pGlobalConn, sSQL);
	pRS->MoveFirst();
	
	for (int j=0; j<pRS->GetRecordCount(); j++) {
		sSQLExec.Format(_T("DROP TABLE `%s`;"), 
			pRS->SQLFldValue(ConstChar(sFld)));
		
		pGlobalConn->ExecuteSQL(sSQLExec);

		pRS->MoveNext();
		
	}

	msgText.Format(_T("BPQ tables were removed."));
	AfxMessageBox(msgText);


}

void CMainFrame::OnCallNotepad()
{
	ShellExecute(*this, _T("open"), _T("Notepad.exe"), NULL, NULL, SW_SHOWNORMAL);	

}
