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
#if !defined(AFX_DLGIMPORTFEATURE_H__51EFD4B1_4AE5_11D3_A9D4_D52F107E0670__INCLUDED_)
#define AFX_DLGIMPORTFEATURE_H__51EFD4B1_4AE5_11D3_A9D4_D52F107E0670__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgImportFeature.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgImportFeature dialog

class CDlgImportFeature : public CDialog
{
// Construction
public:
	CDlgImportFeature(long lFType, LPCSTR sDesc, LPCSTR sFeature, LPCSTR sFeatureParent,  long lParent, BOOL bSelectAll = TRUE, CWnd* pParent = NULL);   // standard constructor

   long GetFeature() {return m_lFeature;}   

protected:

   long m_lFType;
   long m_lParentFeature;
   CString m_sDesc;
   CString m_sFeature;
   CString m_sFeatureParent;
   long m_lFeature;      
   BOOL m_bSelectAll;

   static long m_lFTypePrev;
   static long m_lFeaturePrev;
   static CString m_sDescPrev;
   static CString m_sFeaturePrev;
    
// Dialog Data
	//{{AFX_DATA(CDlgImportFeature)
	enum { IDD = IDD_FEATURESEL };
	CComboBox	m_cbFeature;
	CEdit	m_sDescription;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgImportFeature)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgImportFeature)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelect();
	afx_msg void OnCreate();
	virtual void OnCancel();
	afx_msg void OnSelectall();
	afx_msg void OnSelchangeFeature();
	afx_msg void OnCreateAll();
	afx_msg void OnSkip();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGIMPORTFEATURE_H__51EFD4B1_4AE5_11D3_A9D4_D52F107E0670__INCLUDED_)
