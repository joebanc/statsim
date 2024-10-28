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

// MainFrm.h : interface of the CMainFrame class
//

#pragma once

//#include "SwitchDlg.h"
#include "DataView.h"
#include "ReportView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"

#include "CalendarBar.h"
#include "Resource.h"

class CSwitchDlg;

class COutlookBar : public CMFCOutlookBar
{
	virtual BOOL AllowShowOnPaneMenu() const { return TRUE; }
	virtual void GetPaneName(CString& strName) const { BOOL bNameValid = strName.LoadString(IDS_OUTLOOKBAR); ASSERT(bNameValid); if (!bNameValid) strName.Empty(); }
};

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
private:
	int m_IndImage, m_EltImage;
// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	
	CMFCRibbonStatusBar  m_wndStatusBar;  //now global
	CDataView         m_wndDataView;
	CReportView        m_wndReportView;
	COutputWnd        m_wndOutput;
	CPropertiesWnd    m_wndProperties;

	COutlookBar       m_wndNavigationBar;
	CTreeCtrl m_wndTree;
	CCalendarBar      m_wndCalendar;
	CMFCCaptionBar    m_wndCaptionBar;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnViewCaptionBar();
	afx_msg void OnUpdateViewCaptionBar(CCmdUI* pCmdUI);
	afx_msg void OnOptions();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnNMDblclkTreeRpt(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDataReset();
	afx_msg void OnFileNew();
	afx_msg void OnStatsExeSQL();
	afx_msg void OnStatsExeCMD();

	afx_msg void OnDataMatch();
	afx_msg void OnFileExportCoreCsv();
	afx_msg void OnFileExportData();
	afx_msg void OnFileExportCCICsv();
	afx_msg void OnFileExportMDGCsv();
	afx_msg void OnFileExportCCI();
	afx_msg void OnFileExportMDG();
	afx_msg void OnFileExportNrdb();
	afx_msg void OnStatsCSProXTabHH();
	afx_msg void OnStatsCSProXTabBrgy();
	afx_msg void OnDataEditThresh();
	afx_msg void OnCallEncode();
	afx_msg void OnCallNRDB();
	afx_msg void OnDataAttClean();

	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
	BOOL CreateOutlookBar(CMFCOutlookBar& bar, UINT uiID, CTreeCtrl& tree, CCalendarBar& calendar, int nInitialWidth);
	BOOL CreateCaptionBar();
	void PopulateTableTree(CTreeCtrl& tree);
	void PopulateIndTree(CTreeCtrl& tree);
	void PutSubElt(CTreeCtrl& pTree, DWORD id, HTREEITEM hParent);
	void DisplayInd();

	int FindFocusedOutlookWnd(CMFCOutlookBarTabCtrl** ppOutlookWnd);

	CMFCOutlookBarTabCtrl* FindOutlookParent(CWnd* pWnd);
	CMFCOutlookBarTabCtrl* m_pCurrOutlookWnd;
	CMFCOutlookBarPane*    m_pCurrOutlookPage;
public:
	void ExportCsv(CString table_ext, CString tablab,
		int eltID = 1007);
	afx_msg void OnFileImport();
	void SetAppName(LPCSTR Title);
	CSwitchDlg* pSwitchDlg;
	afx_msg void OnFileExport();
	afx_msg void OnDataClearhh();
	afx_msg void OnDataClearBPQ();
	afx_msg void OnDataClearSelHH();
	afx_msg void OnDataUpdate();
	afx_msg void OnDataProcMDG();
	afx_msg void OnDataProcBPQ();
	afx_msg void OnDataProcOKI();
	afx_msg void OnDataProcCCRI();

	afx_msg void OnCallNotepad();
};


