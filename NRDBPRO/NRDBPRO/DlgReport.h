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
#if !defined(AFX_DLGREPORT_H__DFBFFE82_38F3_11D3_A9BA_E7179BA68B70__INCLUDED_)
#define AFX_DLGREPORT_H__DFBFFE82_38F3_11D3_A9BA_E7179BA68B70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgReport.h : header file
//

#include "buttonselectall.h"
#include "comboboxftype.h"
#include "bdlistbox.h"
#include "longarray.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgReport dialog

class CDlgReport : public CDialog
{
// Construction
public:

	enum {Report=1, Histogram=2, Timeseries=4, PieChart=8};

	CDlgReport(int nType, int nIDD = IDD, CWnd* pParent = NULL);   // standard constructor
   ~CDlgReport();

   long GetFType() {return m_lFType;}
   CLongArray& GetFeatures() {return m_alFeatures;}
	CStringArray& GetFNames() {return m_asFeatures;}
	CLongArray& GetAttr() {return m_alAttr;}
   LPCSTR GetTitle() {return m_sTitle;}

protected:

   void DetermineTitle();
   void InitFeatures(long lFType);
   void RetrieveSel();

   virtual void RetrieveAttrs();
   virtual void SelChangeFType();

	long m_lFType;
   CLongArray m_alFeatures;  
	CStringArray m_asFeatures;
	CLongArray m_alAttr;  	
	int m_nType;
   CString m_sTitle;
   CAttrArray m_aAttr;

   CToolTipCtrl* m_pTooltip;   

// Dialog Data
	//{{AFX_DATA(CDlgReport)
	enum { IDD = IDD_REPORT };
	CButtonSelectAll	m_pbSelectAllAttr;
	CButtonSelectAll	m_pbSelectAll;
	CComboBoxFType	m_cbFTypes;
	CBDListBox	m_lbFeatures;
	CBDListBox	m_lbAttr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgReport)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgReport)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeFtype();
	virtual void OnOK();
	afx_msg void OnQuery();
	afx_msg void OnSelectall();
	afx_msg void OnDeltaposUpdown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelectallAttr();
	afx_msg void OnDictionary();
	afx_msg void OnSelchangeFeatures();
	afx_msg void OnSelchangeAttr();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);	
	afx_msg void OnSelect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGREPORT_H__DFBFFE82_38F3_11D3_A9BA_E7179BA68B70__INCLUDED_)
