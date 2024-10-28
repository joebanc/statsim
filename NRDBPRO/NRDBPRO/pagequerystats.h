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
#if !defined(AFX_PAGEQUERYORDER_H__FECA2820_3D26_11D4_A590_0080AD88D050__INCLUDED_)
#define AFX_PAGEQUERYORDER_H__FECA2820_3D26_11D4_A590_0080AD88D050__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageQueryOrder.h : header file
//

#include "bdcombobox.h"

class CQuery;

/////////////////////////////////////////////////////////////////////////////
// CPageQueryStats dialog

class CPageQueryStats : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageQueryStats)

// Construction
public:
	CPageQueryStats(CQuery* pQuery, CQuery* pQueryPrev, int nType);
   CPageQueryStats();
	~CPageQueryStats();

protected:
    
   CQuery* m_pQuery;
   CQuery* m_pQueryPrev;
   int m_nType;
   int m_nStatistic;

   //void Initialise(int nStat);

// Dialog Data
	//{{AFX_DATA(CPageQueryStats)
	enum { IDD = IDD_QUERYSTATS };
	CBDComboBox	m_cbStatistic;	
	CBDComboBox	m_cbGroupBy;
	CBDComboBox	m_cbAttr;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageQueryStats)
	public:
	virtual void OnOK();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageQueryStats)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeStatistic();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEQUERYORDER_H__FECA2820_3D26_11D4_A590_0080AD88D050__INCLUDED_)
