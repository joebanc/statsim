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

#include "stdafx.h"
#include "MainFrm.h"
#include "ReportView.h"
#include "Resource.h"
#include "StatSimPro.h"
#include "daeconv.h"

#ifndef STATSIMCONN
	#include "StatSimConn.h"
#endif
#ifndef STATSIMRS
	#include "StatSimRS.h"
#endif

extern CStatSimConn* pGlobalConn;

class CReportViewMenuButton : public CMFCToolBarMenuButton
{
	friend class CReportView;

	DECLARE_SERIAL(CReportViewMenuButton)

public:
	CReportViewMenuButton(HMENU hMenu = NULL) : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
	{
	}

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE,
		BOOL bCustomizeMode = FALSE, BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE)
	{
		pImages = CMFCToolBar::GetImages();

		CAfxDrawState ds;
		pImages->PrepareDrawImage(ds);

		CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

		pImages->EndDrawImage(ds);
	}
};

IMPLEMENT_SERIAL(CReportViewMenuButton, CMFCToolBarMenuButton, 1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReportView::CReportView()
{
	m_nCurrSort = ID_SORTING_GROUPBYTYPE;
}

CReportView::~CReportView()
{
}

BEGIN_MESSAGE_MAP(CReportView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_REPORT_ADD_MEMBER_FUNCTION, OnReportAddMemberFunction)
	ON_COMMAND(ID_REPORT_ADD_MEMBER_VARIABLE, OnReportAddMemberVariable)
	ON_COMMAND(ID_REPORT_DEFINITION, OnReportDefinition)
	ON_COMMAND(ID_REPORT_PROPERTIES, OnReportProperties)
	ON_COMMAND(ID_NEW_FOLDER, OnNewFolder)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnSort)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnUpdateSort)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReportView message handlers

int CReportView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create views:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndReportView.Create(dwViewStyle, rectDummy, this, 2))
	{
		TRACE0("Failed to create report View\n");
		return -1;      // fail to create
	}

	// Load images:
	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_SORT);
	m_wndToolBar.LoadToolBar(IDR_SORT, 0, 0, TRUE /* Is locked */);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	CMenu menuSort;
	menuSort.LoadMenu(IDR_POPUP_SORT);

	m_wndToolBar.ReplaceButton(ID_SORT_MENU, CReportViewMenuButton(menuSort.GetSubMenu(0)->GetSafeHmenu()));

	CReportViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CReportViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != NULL)
	{
		pButton->m_bText = FALSE;
		pButton->m_bImage = TRUE;
		pButton->SetImage(GetCmdMgr()->GetCmdImage(m_nCurrSort));
		pButton->SetMessageWnd(this);
	}

	// Fill in some static tree view data (dummy code, nothing magic here)
	//FillReportView();

	return 0;
}

void CReportView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CReportView::FillReportView()
{

	CString sSQL;
	CStatSimRS *pCatRS = 0, *pSecRS = 0, *pFigRS = 0, *pEltRS = 0;
	sSQL = "SELECT * FROM `~Ind` WHERE `indtype`=1;";
	pCatRS = new CStatSimRS( pGlobalConn, sSQL);
	int nCat = pCatRS->GetRecordCount();
	if (nCat>0)
		pCatRS->MoveFirst();
	
	for ( int i=0; i<nCat; i++ ) {	
		
		_totchar sTxt( pCatRS->SQLFldValue( _MBCS("label") ) );
		HTREEITEM hCat = m_wndReportView.InsertItem(sTxt, 0, 0);
		m_wndReportView.SetItemState(hCat, TVIS_BOLD, TVIS_BOLD);


		pCatRS->MoveNext();

	}
	
	delete pCatRS; pCatRS = 0;

/*	HTREEITEM hRoot = m_wndReportView.InsertItem(_T("FakeApp Reports"), 0, 0);
	m_wndReportView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	HTREEITEM hReport = m_wndReportView.InsertItem(_T("CFakeAboutDlg"), 1, 1, hRoot);
	m_wndReportView.InsertItem(_T("CFakeAboutDlg()"), 3, 3, hReport);

	m_wndReportView.Expand(hRoot, TVE_EXPAND);

	hReport = m_wndReportView.InsertItem(_T("CFakeApp"), 1, 1, hRoot);
	m_wndReportView.InsertItem(_T("CFakeApp()"), 3, 3, hReport);
	m_wndReportView.InsertItem(_T("InitInstance()"), 3, 3, hReport);
	m_wndReportView.InsertItem(_T("OnAppAbout()"), 3, 3, hReport);

	hReport = m_wndReportView.InsertItem(_T("CFakeAppDoc"), 1, 1, hRoot);
	m_wndReportView.InsertItem(_T("CFakeAppDoc()"), 4, 4, hReport);
	m_wndReportView.InsertItem(_T("~CFakeAppDoc()"), 3, 3, hReport);
	m_wndReportView.InsertItem(_T("OnNewDocument()"), 3, 3, hReport);

	hReport = m_wndReportView.InsertItem(_T("CFakeAppView"), 1, 1, hRoot);
	m_wndReportView.InsertItem(_T("CFakeAppView()"), 4, 4, hReport);
	m_wndReportView.InsertItem(_T("~CFakeAppView()"), 3, 3, hReport);
	m_wndReportView.InsertItem(_T("GetDocument()"), 3, 3, hReport);
	m_wndReportView.Expand(hReport, TVE_EXPAND);

	hReport = m_wndReportView.InsertItem(_T("CFakeAppFrame"), 1, 1, hRoot);
	m_wndReportView.InsertItem(_T("CFakeAppFrame()"), 3, 3, hReport);
	m_wndReportView.InsertItem(_T("~CFakeAppFrame()"), 3, 3, hReport);
	m_wndReportView.InsertItem(_T("m_wndMenuBar"), 6, 6, hReport);
	m_wndReportView.InsertItem(_T("m_wndToolBar"), 6, 6, hReport);
	m_wndReportView.InsertItem(_T("g_wndStatusBarPane"), 6, 6, hReport);

	hReport = m_wndReportView.InsertItem(_T("Globals"), 2, 2, hRoot);
	m_wndReportView.InsertItem(_T("theFakeApp"), 5, 5, hReport);
	m_wndReportView.Expand(hReport, TVE_EXPAND);

	*/
}

void CReportView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndReportView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// Select clicked item:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	CMenu menu;
	menu.LoadMenu(IDR_POPUP_SORT);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}
}

void CReportView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndReportView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CReportView::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
}

void CReportView::OnSort(UINT id)
{
	if (m_nCurrSort == id)
	{
		return;
	}

	m_nCurrSort = id;

	CReportViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CReportViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != NULL)
	{
		pButton->SetImage(GetCmdMgr()->GetCmdImage(id));
		m_wndToolBar.Invalidate();
		m_wndToolBar.UpdateWindow();
	}
}

void CReportView::OnUpdateSort(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID == m_nCurrSort);
}

void CReportView::OnReportAddMemberFunction()
{
	AfxMessageBox(_T("Add member function..."));
}

void CReportView::OnReportAddMemberVariable()
{
	// TODO: Add your command handler code here
}

void CReportView::OnReportDefinition()
{
	// TODO: Add your command handler code here
}

void CReportView::OnReportProperties()
{
	// TODO: Add your command handler code here
}

void CReportView::OnNewFolder()
{
	AfxMessageBox(_T("New Folder..."));
}

void CReportView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndReportView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CReportView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndReportView.SetFocus();
}

void CReportView::OnChangeVisualStyle()
{
	m_ReportViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_REPORT_VIEW_24 : IDB_REPORT_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("Can't load bitmap: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_ReportViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ReportViewImages.Add(&bmp, RGB(255, 0, 0));

	m_wndReportView.SetImageList(&m_ReportViewImages, TVSIL_NORMAL);

	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_SORT_24 : IDR_SORT, 0, 0, TRUE /* Locked */);
}
