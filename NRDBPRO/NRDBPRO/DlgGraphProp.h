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
#if !defined(AFX_DLGGRAPHPROP_H__19B48AD1_27FE_11D5_A791_0080AD88D050__INCLUDED_)
#define AFX_DLGGRAPHPROP_H__19B48AD1_27FE_11D5_A791_0080AD88D050__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgGraphProp.h : header file
//

#include "viewgraph.h"
#include "buttoncolor.h"
#include "comboboxlines.h"
#include "editplus.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgGraphProp dialog

class CDlgGraphProp : public CDialog
{
// Construction
public:
	CDlgGraphProp(CGraphProperties*, CWnd* pParent = NULL);   // standard constructor

   CGraphProperties m_prop;
   CGraphProperties* m_pProp;
   int m_iPrev;

// Dialog Data
	//{{AFX_DATA(CDlgGraphProp)
	enum { IDD = IDD_GRAPHPROP };
	CEditPlus	m_eTicks;
	CEdit	m_eLegend;
	CEditPlus	m_eMin;
	CEditPlus	m_eMax;
	CComboBox	m_cbType;
	CComboBoxLines	m_cbStyle;
	CButtonColour	m_pbColour;
	CComboBox	m_cbLine;
	CEdit	m_sTitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgGraphProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgGraphProp)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeLine();
	afx_msg void OnFont();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGGRAPHPROP_H__19B48AD1_27FE_11D5_A791_0080AD88D050__INCLUDED_)
