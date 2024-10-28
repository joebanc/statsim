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
#include "mainfrm.h"
#include "DataView.h"
#include "Resource.h"
#include "StatSimPro.h"

#ifndef STATSIMCONN
	#include "StatSimConn.h"
#endif
#ifndef STATSIMRS
	#include "StatSimRS.h"
#endif

#ifdef _DEBUG
#undef THIS_DATA
static char THIS_DATA[]=__DATA__;
#define new DEBUG_NEW
#endif

extern CStatSimConn* pGlobalConn;

/////////////////////////////////////////////////////////////////////////////
// CDataView

CDataView::CDataView()
{
}

CDataView::~CDataView()
{
}

BEGIN_MESSAGE_MAP(CDataView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	ON_COMMAND(ID_OPEN, OnFileOpen)
	ON_COMMAND(ID_OPEN_WITH, OnFileOpenWith)
	ON_COMMAND(ID_DUMMY_COMPILE, OnDummyCompile)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar message handlers

int CDataView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create view:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	if (!m_wndDataView.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("Failed to create data view\n");
		return -1;      // fail to create
	}

	// Load view images:
	m_DataViewImages.Create(IDB_DATA_VIEW, 16, 0, RGB(255, 0, 255));
	m_wndDataView.SetImageList(&m_DataViewImages, TVSIL_NORMAL);

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
	m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* Is locked */);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	// Fill in some static tree view data (dummy code, nothing magic here)
	FillDataView();
	AdjustLayout();

	return 0;
}

void CDataView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CDataView::FillDataView()
{


	/*
	HTREEITEM hRoot = m_wndDataView.InsertItem(_T("FakeApp files"), 0, 0);
	m_wndDataView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	HTREEITEM hSrc = m_wndDataView.InsertItem(_T("FakeApp Source Files"), 0, 0, hRoot);

	m_wndDataView.InsertItem(_T("FakeApp.cpp"), 1, 1, hSrc);
	m_wndDataView.InsertItem(_T("FakeApp.rc"), 1, 1, hSrc);
	m_wndDataView.InsertItem(_T("FakeAppDoc.cpp"), 1, 1, hSrc);
	m_wndDataView.InsertItem(_T("FakeAppView.cpp"), 1, 1, hSrc);
	m_wndDataView.InsertItem(_T("MainFrm.cpp"), 1, 1, hSrc);
	m_wndDataView.InsertItem(_T("StdAfx.cpp"), 1, 1, hSrc);

	HTREEITEM hInc = m_wndDataView.InsertItem(_T("FakeApp Header Files"), 0, 0, hRoot);

	m_wndDataView.InsertItem(_T("FakeApp.h"), 2, 2, hInc);
	m_wndDataView.InsertItem(_T("FakeAppDoc.h"), 2, 2, hInc);
	m_wndDataView.InsertItem(_T("FakeAppView.h"), 2, 2, hInc);
	m_wndDataView.InsertItem(_T("Resource.h"), 2, 2, hInc);
	m_wndDataView.InsertItem(_T("MainFrm.h"), 2, 2, hInc);
	m_wndDataView.InsertItem(_T("StdAfx.h"), 2, 2, hInc);

	HTREEITEM hRes = m_wndDataView.InsertItem(_T("FakeApp Resource Files"), 0, 0, hRoot);

	m_wndDataView.InsertItem(_T("FakeApp.ico"), 2, 2, hRes);
	m_wndDataView.InsertItem(_T("FakeApp.rc2"), 2, 2, hRes);
	m_wndDataView.InsertItem(_T("FakeAppDoc.ico"), 2, 2, hRes);
	m_wndDataView.InsertItem(_T("FakeToolbar.bmp"), 2, 2, hRes);

	

	m_wndDataView.Expand(hRoot, TVE_EXPAND);
	m_wndDataView.Expand(hSrc, TVE_EXPAND);
	m_wndDataView.Expand(hInc, TVE_EXPAND);
	*/
}

void CDataView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndDataView;
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
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
}

void CDataView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndDataView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CDataView::OnProperties()
{
	AfxMessageBox(_T("Properties...."));

}

void CDataView::OnFileOpen()
{
	// TODO: Add your command handler code here
}

void CDataView::OnFileOpenWith()
{
	// TODO: Add your command handler code here
}

void CDataView::OnDummyCompile()
{
	// TODO: Add your command handler code here
}

void CDataView::OnEditCut()
{
	// TODO: Add your command handler code here
}

void CDataView::OnEditCopy()
{
	// TODO: Add your command handler code here
}

void CDataView::OnEditClear()
{
	// TODO: Add your command handler code here
}

void CDataView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndDataView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CDataView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndDataView.SetFocus();
}

void CDataView::OnChangeVisualStyle()
{
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_EXPLORER_24 : IDR_EXPLORER, 0, 0, TRUE /* Locked */);

	m_DataViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_DATA_VIEW_24 : IDB_DATA_VIEW;

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

	m_DataViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_DataViewImages.Add(&bmp, RGB(255, 0, 255));

	m_wndDataView.SetImageList(&m_DataViewImages, TVSIL_NORMAL);
}


