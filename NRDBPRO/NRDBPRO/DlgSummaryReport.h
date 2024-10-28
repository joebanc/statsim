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
#if !defined(AFX_DLGSUMMARYREPORT_H__53AFCEA0_069B_11D4_AB4B_0080AD88D050__INCLUDED_)
#define AFX_DLGSUMMARYREPORT_H__53AFCEA0_069B_11D4_AB4B_0080AD88D050__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSummaryReport.h : header file
//

#include "buttonselectall.h"
#include "comboboxftype.h"
#include "bdlistbox.h"
#include "longarray.h"


/////////////////////////////////////////////////////////////////////////////
// CDlgSummaryReport dialog

class CDlgSummaryReport : public CDialog
{
// Construction
public:
	CDlgSummaryReport(CWnd* pParent = NULL);   // standard constructor

   long GetFType() {return m_lFType;}
   CLongArray& GetFeatures() {return m_alFeatures;}

protected:

	long m_lFType;
   CLongArray m_alFeatures; 
   BOOL m_bInit;

// Dialog Data
	//{{AFX_DATA(CDlgSummaryReport)
	enum { IDD = IDD_SUMMARYREPORT };
	CButtonSelectAll	m_pbSelectAll;
   CComboBoxFType	m_cbFTypes;
	CBDListBox	m_lbFeatures;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSummaryReport)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSummaryReport)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeFtype();
	virtual void OnOK();
	afx_msg void OnSelectall();
	afx_msg void OnDictionary();
	afx_msg void OnSelect();
	afx_msg void OnSelchangeFeatures();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSUMMARYREPORT_H__53AFCEA0_069B_11D4_AB4B_0080AD88D050__INCLUDED_)
