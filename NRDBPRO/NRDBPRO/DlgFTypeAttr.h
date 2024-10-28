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
#if !defined(AFX_DLGFTYPEATTR_H__5CB1BE64_1345_11D3_ADF1_44C1F2C00000__INCLUDED_)
#define AFX_DLGFTYPEATTR_H__5CB1BE64_1345_11D3_ADF1_44C1F2C00000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFTypeAttr.h : header file
//

#include "comboboxftype.h"

/////////////////////////////////////////////////////////////////////////////

struct CAttrTypes
{
	int m_nId;
   int m_nStringId;
   
   static CString GetDesc(int nId);
   static CAttrTypes m_aAttrType[10];
};

/////////////////////////////////////////////////////////////////////////////
// CDlgFTypeAttr dialog

class CDlgFTypeAttr : public CDialog
{
// Construction
public:
	CDlgFTypeAttr(long lId = 0, long lParentId = 0, long lType = 0, CWnd* pParent = NULL);   // standard constructor   
   ~CDlgFTypeAttr();

   long GetId() {return m_lFTypeId;}

protected:
   int m_iFlag;
   long m_lFTypeId;
   long m_lType;
   BOOL m_bEdit;
   BOOL m_bUpdate;
   long m_lParentId;
   BOOL m_bShowAttr;

   void OnInitAdd();
   void OnInitEdit();
   /*BOOL IsNameValid(CString&);*/
   static void RemoveInvalidChar(CString&);
   void CheckNameUnique(CString&);
   void InitSectors();
   void UpdateAttr();
   void ResizeWindow(int nControl);

   CFeatureType m_ftypeEdit;

// Dialog Data
	//{{AFX_DATA(CDlgFTypeAttr)
	enum { IDD = IDD_FTYPEATTR };
	CComboBox	m_cbDictionary;
	CComboBox	m_cbManyToOne;
	CComboBox	m_cbParentFType;
	CComboBoxFType	m_cbFTypeLink;
	CComboBox	m_cbAttrType;
	CListBox	m_lbAttributes;
	CEdit	m_eFTypeName;
	CEdit	m_eAttrName;
	//}}AFX_DATA

   CComboBox m_cbPrimaryKey;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFTypeAttr)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgFTypeAttr)
	afx_msg void OnAdd();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeAttributes();
	afx_msg void OnDelete();
	virtual void OnOK();
	afx_msg void OnSelchangeAttrtype();
	afx_msg void OnSelchangeFtypelink();
	afx_msg void OnDeltaposUpdown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();
	afx_msg void OnPrimarykey();
	afx_msg void OnSelchangeParentfytpe();
	afx_msg void OnUpdate();
	afx_msg void OnChangeAttrname();
	afx_msg void OnSector();
	afx_msg void OnKillfocusFtypename();
	afx_msg void OnSelchangeDictionary();
	afx_msg void OnKillfocusDictionary();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFTYPEATTR_H__5CB1BE64_1345_11D3_ADF1_44C1F2C00000__INCLUDED_)
