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
#if !defined(AFX_PAGEQUERYSTATS_H__924C12A0_3BB7_11D4_A590_0080AD88D050__INCLUDED_)
#define AFX_PAGEQUERYSTATS_H__924C12A0_3BB7_11D4_A590_0080AD88D050__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageQueryStats.h : header file
//

#include "query.h"

/////////////////////////////////////////////////////////////////////////////
// CPageQueryCalc dialog

class CPageQueryCalc : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageQueryCalc)

// Construction
public:
	CPageQueryCalc(CQuery* pQuery, CQuery* pQueryPrev, int nType);
   CPageQueryCalc();
	~CPageQueryCalc();

protected:
	CQuery* m_pQuery;
   CQuery* m_pQueryPrev;
   int m_nType;
   BOOL m_bChanged;

   void Update();
   void UpdateQueryStats();
   void UpdateOperators();

// Dialog Data
	//{{AFX_DATA(CPageQueryCalc)
	enum { IDD = IDD_QUERYCALC };
	CEditPlus	m_eValue1;
	CEditPlus	m_eValue2;
	CEditPlus	m_eDecPlaces;
	CEdit	m_eStatsName;
	CListBox	m_lbQueryStats;
	CBDComboBox	m_cbOperator;
	CBDComboBox	m_cbAttr2;
	CBDComboBox	m_cbAttr1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageQueryCalc)
	public:
	virtual BOOL OnSetActive();
	virtual void OnOK();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageQueryCalc)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeQuerystats();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnSelchange();
	afx_msg void OnChangeStatsname();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEQUERYSTATS_H__924C12A0_3BB7_11D4_A590_0080AD88D050__INCLUDED_)
