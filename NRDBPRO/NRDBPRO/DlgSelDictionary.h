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
#if !defined(AFX_DLGSELDICTIONARY_H__0204DE12_B137_11D3_AAAD_0080AD88D050__INCLUDED_)
#define AFX_DLGSELDICTIONARY_H__0204DE12_B137_11D3_AAAD_0080AD88D050__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSelDictionary.h : header file
//

#include "comboboxftype.h"
#include "bdlistbox.h"
#include "buttonselectall.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgSelDictionary dialog

class CDlgSelDictionary : public CDialog
{
// Construction
public:
	CDlgSelDictionary(CDictSelArray* paDictSel, BOOL *m_pbIncLinks, CWnd* pParent = NULL);   // standard constructor

protected:
   CDictSelArray* m_paDictSel;
   BOOL* m_pbIncLinks;

// Dialog Data
	//{{AFX_DATA(CDlgSelDictionary)
	enum { IDD = IDD_SELDICTIONARY };
	CButtonSelectAll	m_pbSelectAll;
	CButton	m_ckIncLinks;
	CBDListBox	m_lbDictionary;
	//}}AFX_DATA

   void InitList();

   long m_lMaxId;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSelDictionary)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSelDictionary)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelectall();
	afx_msg void OnSelchangeDictionaries();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSELDICTIONARY_H__0204DE12_B137_11D3_AAAD_0080AD88D050__INCLUDED_)
