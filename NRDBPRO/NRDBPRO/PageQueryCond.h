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
#if !defined(AFX_PAGEQUERY2_H__6200C843_329F_11D4_84E7_BF3009935903__INCLUDED_)
#define AFX_PAGEQUERY2_H__6200C843_329F_11D4_84E7_BF3009935903__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageQuery2.h : header file
//

#include "query.h"
#include "editplus.h"

/////////////////////////////////////////////////////////////////////////////
// CPageQueryCond dialog

class CPageQueryCond : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageQueryCond)

// Construction
public:
	CPageQueryCond(CQuery* pQuery, CQuery* pQueryPrev, int nType);
	CPageQueryCond();
	~CPageQueryCond();

protected:
   void UpdateList();

	CQuery* m_pQuery;
   CQuery* m_pQueryPrev;
   int m_nType;

// Dialog Data

    enum { IDD = IDD_QUERYCOND };

    CEditPlus	m_eValue;
	CListBox	m_lbCond;
	CBDComboBox	m_cbCond;
	CBDComboBox	m_cbAttr;

	//{{AFX_DATA(CPageQueryCond)	
	CEdit	m_eTo;
	CEdit	m_eFrom;
	CBDComboBox	m_cbDates;
   CButton	m_ckShowAllFeatures;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageQueryCond)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageQueryCond)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeDates();	
   afx_msg void OnAdd();
	afx_msg void OnDelete();
   afx_msg void OnSelChangeCond();
   afx_msg void OnSelChangeAttr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEQUERY2_H__6200C843_329F_11D4_84E7_BF3009935903__INCLUDED_)
