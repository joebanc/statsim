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
#if !defined(AFX_DLGLAYOUT_H__2197FA81_E5EF_11D7_B419_000795C2378F__INCLUDED_)
#define AFX_DLGLAYOUT_H__2197FA81_E5EF_11D7_B419_000795C2378F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgLayout.h : header file
//

#include "gridedit.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgLayout dialog

class CDlgLayout : public CDialog
{
// Construction
public:
	CDlgLayout(CWnd* pParent = NULL);   // standard constructor   

protected:

   void InitLayout();
   void SetDefault(int index);

// Dialog Data
	//{{AFX_DATA(CDlgLayout)
	enum { IDD = IDD_LAYOUT };
	CListBox	m_lbLayout;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgLayout)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgLayout)
	afx_msg void OnAdd();
	virtual BOOL OnInitDialog();
	afx_msg void OnEdit();
	virtual void OnOK();
	afx_msg void OnDelete();
	afx_msg void OnRename();	
	afx_msg void OnSelchangeLayout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLAYOUT_H__2197FA81_E5EF_11D7_B419_000795C2378F__INCLUDED_)
