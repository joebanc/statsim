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
// Users' Forum: http://nrdb.mypalawan.info/
// 
#if !defined(AFX_DLGFEATURES_H__12724320_1425_11D3_ADF1_44C1F2C00000__INCLUDED_)
#define AFX_DLGFEATURES_H__12724320_1425_11D3_ADF1_44C1F2C00000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFeatures.h : header file
//

#include "comboboxftype.h"

class CLongArray;

/////////////////////////////////////////////////////////////////////////////
// CDlgFeatures dialog

class CDlgFeatures : public CDialog
{
// Construction
public:
	CDlgFeatures(long lFType = 0, CWnd* pParent = NULL);   // standard constructor

protected:   

   void InitFeatures(long lIdSel);
   void EnableControls(long lIdSel);

   void GetChildren(long lFType, CLongArray& alFType121, CLongArray& alFType12M);

   long m_lFType;

// Dialog Data
	//{{AFX_DATA(CDlgFeatures)
	enum { IDD = IDD_FEATURES };
	CComboBoxFType	m_cbFTypes;
	CListBox	m_lbFeatures;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFeatures)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgFeatures)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeFtype();
	afx_msg void OnAdd();
	afx_msg void OnEdit();
	afx_msg void OnDelete();
	afx_msg void OnEditData();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelect();
	afx_msg void OnSelchangeFeatures();
	afx_msg void OnDictionary();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFEATURES_H__12724320_1425_11D3_ADF1_44C1F2C00000__INCLUDED_)
