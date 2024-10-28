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

// nrdb.h : main header file for the NRDB application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifndef _NRDBPRO_H_
#define _NRDBPRO_H_

#include "nrdb.h"

#include "nrdb_i.h"
#include "bdcombobox.h"
#include "bdlistbox.h"

#include "editplus.h"
#include "comboboxftype.h"

#include "longarray.h"
#include "maplayer.h"

#include "projctns.h"
#include "dlgprogress.h"

/////////////////////////////////////////////////////////////////////////////

class CDocMap;

/////////////////////////////////////////////////////////////////////////////
// CNRDBApp:
// See nrdb.cpp for the implementation of this class
//

class CNRDBApp : public CWinApp
{
public:
	CNRDBApp();
   ~CNRDBApp();

   void SetProgressBar(CBDProgressBar* p) {m_pProgressBar = p;}   
   CBDProgressBar* GetProgressBar() {return m_pProgressBar;}   
   void SetProgressText(CStatic* pEdit) {m_pProgessText = pEdit;}
   CStatic* GetProgressText() {return m_pProgessText;}
   LPCSTR GetDataSource() {return m_sDataSource;}   
   CDocMap* GetDocMap();
   void OnClose() {OnAppExit();}
   int GetPreviousReport() {return m_bPreviousReport ? m_nPreviousReport : 0;}
   void SetPreviousReport(int nType) {m_nPreviousReport = nType;}
   CString GetAppPath();
   LPCSTR GetHtmlHelp() {return m_sHtmlHelp;}
   BOOL IsHtmlHelpTemp() {return m_bHtmlHelpTemp;}
   void SetHtmlHelp(LPCSTR s, BOOL bTemp) {m_sHtmlHelp = s; m_bHtmlHelpTemp=bTemp;}
   CBDMain& GetSettings() {return m_BDMain;}   
   CProjection* GetProjection() {return &m_projection;}
   CMapLayoutArray& GetLayout() {return m_aMapLayout;}
   BDHANDLE& GetDBHandle() {return m_hConnect;}
   CMultiDocTemplate* GetDocTemplate(LPCSTR pDocName);  
   void GetPrinterInfo(DEVMODE* pDevMode);
   CUnits& GetUnits() {return m_units;}
   
   static CString& LoadString(int nId) {m_sLoadString.LoadString(nId);return m_sLoadString;};

   // Default selections

   long GetFTypeSel() {return m_lFTypeSel;}
   CFeatureArray& GetFeatureSel() {return m_aFeatureSel;}
   CFTypeAttrArray& GetAttrSel() {return m_aAttrSel;}
   void SetFTypeSel(long lFType) {m_lFTypeSel = lFType;}   

   // Report types

public:

   HWND  m_hwndDialog;      
   CToolTipCtrl* m_gpToolTip;   
   CString m_sUser;
   CString m_sPassword;
   CString m_sHost;

protected:
            
   CBDProgressBar* m_pProgressBar;
   CStatic* m_pProgessText;
   CString m_sDataSource;
   int m_nPreviousReport;
   BOOL m_bPreviousReport;
   long m_lFTypeSel;
   CFeatureArray m_aFeatureSel;
   CFTypeAttrArray m_aAttrSel;   
   CString m_sHtmlHelp;
   BOOL m_bHtmlHelpTemp;
   static CString m_sLoadString;
   CBDMain m_BDMain;   
   CProjection m_projection;
   CMapLayoutArray m_aMapLayout;
   BDHANDLE m_hConnect;   
   BOOL m_bFType;
   CString m_sDefaultMap;
   CUnits m_units;
   

protected:   
   void SetLandscape();   
   CString DefaultDataSource();
   void UpdateMenu();
   BOOL Login();
   void Logout();   
   void LoadLayout();
   void SaveLayout();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNRDBApp)
	public:
	virtual BOOL InitInstance();
   virtual int ExitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);	   	
	//}}AFX_VIRTUAL

   BOOL ProcessMessageFilter(int code, LPMSG lpMsg);

// Implementation

	//{{AFX_MSG(CNRDBApp)
	afx_msg void OnAppAbout();	
	afx_msg void OnAppExit();
	afx_msg void OnReportGraph();
	afx_msg void OnReportTable();
	afx_msg void OnImportODBC();		
	afx_msg void OnDbaseCompress();
	afx_msg void OnReportSummary();
	afx_msg void OnReportTsgraph();
   afx_msg void OnReportPieChart();
   afx_msg void OnReportTSReport();
	afx_msg void OnToolsSettings();
   afx_msg void OnReportWizard();   
   afx_msg void OnReportWizardSetup();   
   afx_msg void OnViewLayers();   
	afx_msg void OnImportFile();
   afx_msg void OnImportShapefile();
	afx_msg void OnConvertShapefile();
	afx_msg void OnExportShapefile();
	afx_msg void OnFileLogout();
	afx_msg void OnReportPrevious();
	afx_msg void OnUpdateExportShapefile(CCmdUI* pCmdUI);
	afx_msg void OnHelpNrdbhomepage();
	afx_msg void OnHelpSupport();
	afx_msg void OnHelpTutorial();	
	afx_msg void OnFileNew();
	afx_msg void OnFileSetupFeatureTypes();
	afx_msg void OnFileSetupFeatures();
	afx_msg void OnUpdateFTypesExist(CCmdUI* pCmdUI);
	afx_msg void OnExplorer();
	afx_msg void OnMapLayout();
	afx_msg void OnUpdateReportPrevious(CCmdUI* pCmdUI);
	afx_msg void OnHelpRegister();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CNRDBApp* BDGetApp();

/////////////////////////////////////////////////////////////////////////////

#endif 
