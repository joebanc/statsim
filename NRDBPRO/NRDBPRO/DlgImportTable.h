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
#if !defined(AFX_DLGIMPORTTABLE_H__345F72A1_458B_11D3_A9CC_81DBD7EB6870__INCLUDED_)
#define AFX_DLGIMPORTTABLE_H__345F72A1_458B_11D3_A9CC_81DBD7EB6870__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgImportTable.h : header file
//

#include "importodbc.h"
#include "comboboxftype.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgImportTable dialog

class CDlgImportTable : public CDialog
{
// Construction
public:
	
	CDlgImportTable(CDatabase* pDatabase, CImportTable importtable,                    
                   BOOL bImport, int nFileType, CWnd* pParent = NULL);   // standard constructor
   ~CDlgImportTable();

   CImportTable& GetImportTable() {return m_importTable;}
   
protected:

	CImportTable m_importTable;   
	CDatabase* m_pDatabase;
   BOOL m_bImport;	
	int m_nFileType;
   int m_nShape;
   CToolTipCtrl* m_pTooltip;   

	void InitFTypes();
	void InitColumns();
	void InitAttr();
   void InitImport();
   long GetColumnData(LPCSTR);   
   void DetermineShapefile();
	void MatchShape();
   BOOL Validate();
   void Restore();

    void ResizeWindow(int nControl);
    CString GetFilePath(LPCSTR sExt);

    void OnCreate();

// Dialog Data
   	
	//{{AFX_DATA(CDlgImportTable)
	enum { IDD = IDD_IMPORTTABLE };
	CListBox	m_lbAttr;
	CListBox	m_lbColumn;
   CListBox m_lbColumnAttr;
	CComboBoxFType	m_cbFType;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgImportTable)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgImportTable)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeFtype();
	afx_msg void OnSelchangeAttribute();
	afx_msg void OnSelchangeColumn();
	afx_msg void OnSelect();
   afx_msg void OnAdd();
   afx_msg void OnDelete();
   afx_msg void OnNext();
	virtual void OnOK();
	afx_msg void OnDictionary();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnAutomatch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGIMPORTTABLE_H__345F72A1_458B_11D3_A9CC_81DBD7EB6870__INCLUDED_)
