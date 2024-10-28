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
#if !defined(AFX_DLGIMPORTODBC_H__8F79BE01_4556_11D3_A9C9_A9ED04AE2170__INCLUDED_)
#define AFX_DLGIMPORTODBC_H__8F79BE01_4556_11D3_A9C9_A9ED04AE2170__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgImportODBC.h : header file
//

#include "importODBC.h"
#include "listboxtick.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgImportODBC dialog

class CDlgImportODBC : public CDialog
{
// Construction
public:
	CDlgImportODBC(LPCSTR sDataSource, LPCSTR sTable, BOOL bSysTables, int nFileType, CWnd* pParent = NULL);   // standard constructor
	
   CImportDB& GetImportDB() {return m_importDB;}
   LPCSTR GetDatabase() {return m_sDatabase;}
   BOOL IsNRDB() {return m_bNRDB;}

protected:
   CDatabase m_database;
   CImportDB m_importDB;
   CString m_sDatabase;   
   CString m_sTable;   
   BOOL m_bSysTables;
   int m_nFileType;
   BOOL m_bNRDB;

   void ResizeWindow(int nControl);
   void InitDataSources();
   int Define();
   void InitFTypes();
   BOOL ValidColumns(LPCSTR);

   BOOL Validate();
	
// Dialog Data
	//{{AFX_DATA(CDlgImportODBC)
	enum { IDD = IDD_IMPORTODBC };
	CButton	m_pbSysTables;
	CListBoxTick	m_lbTables;
	CComboBox	m_cbDataSource;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgImportODBC)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgImportODBC)
	virtual BOOL OnInitDialog();
	afx_msg void OnConnect();
	afx_msg void OnClose();
	afx_msg void OnImport();
	afx_msg void OnDefine();
	afx_msg void OnDblclkTables();
	afx_msg void OnSave();
	afx_msg void OnLoad();
	virtual void OnOK();
	afx_msg void OnSelectall();
	afx_msg void OnSelchangeTables();
	afx_msg void OnAdministrator();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGIMPORTODBC_H__8F79BE01_4556_11D3_A9C9_A9ED04AE2170__INCLUDED_)
