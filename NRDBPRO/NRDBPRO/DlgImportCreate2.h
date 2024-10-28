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
#if !defined(AFX_DLGIMPORTCREATE2_H__9052C141_3F7F_11D6_AD4E_DFE4E2836109__INCLUDED_)
#define AFX_DLGIMPORTCREATE2_H__9052C141_3F7F_11D6_AD4E_DFE4E2836109__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgImportCreate2.h : header file
//

#include "importodbc.h"
#include "listboxtick.h"

/////////////////////////////////////////////////////////////////////////////

struct CColumnType
{
   CString m_sColName;
   long m_nDataType;   
};

typedef CArray <CColumnType, CColumnType> CColumnTypeArray;

/////////////////////////////////////////////////////////////////////////////
// CDlgImportCreate2 dialog

class CDlgImportCreate2 : public CDialog
{
// Construction
public:
	CDlgImportCreate2(CDatabase* pDatabase, CImportTable* pImportTable,  int nShape, int nFileType, CWnd* pParent = NULL);   // standard constructor
   ~CDlgImportCreate2();

protected:

   void ResizeWindow(int nControl);
   void InitSectors();
   CString GetFilePath(LPCSTR sExt);
   BOOL CreateFType();
   BOOL UpdateImportList();

   CImportTable* m_pImportTable;
   CDatabase* m_pDatabase;
   CColumnTypeArray m_aImportCol;
   int m_nShape;
   int m_nFileType;   
   CArray <CFTypeAttr, CFTypeAttr> m_aFTypeAttr;

// Dialog Data
	//{{AFX_DATA(CDlgImportCreate2)
	enum { IDD = IDD_IMPORTCREATE };
	CComboBox	m_cbDate;
	CComboBox	m_cbLabel;	
	CEdit	m_eFType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgImportCreate2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgImportCreate2)
	virtual BOOL OnInitDialog();
	afx_msg void OnSector();
	afx_msg void OnSelchangeParentfytpe();
	afx_msg void OnSelect();
	afx_msg void OnSelchangeColumn();
	afx_msg void OnSelchangeAttribute();
	virtual void OnOK();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGIMPORTCREATE2_H__9052C141_3F7F_11D6_AD4E_DFE4E2836109__INCLUDED_)
