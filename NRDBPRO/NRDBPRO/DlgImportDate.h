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
#if !defined(AFX_DLGIMPORTDATE_H__8C744F61_EE6F_11D3_AB1B_0080AD88D050__INCLUDED_)
#define AFX_DLGIMPORTDATE_H__8C744F61_EE6F_11D3_AB1B_0080AD88D050__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgImportDate.h : header file
//

#include "cdate.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgImportDate dialog

class CDlgImportDate : public CDialog
{
// Construction
public:
	CDlgImportDate(CWnd* pParent = NULL);   // standard constructor

   CDateTime GetDate() {return m_date;}
protected:

   CDateTime m_date;


// Dialog Data
	//{{AFX_DATA(CDlgImportDate)
	enum { IDD = IDD_IMPORTDATE };
	CEdit	m_eDate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgImportDate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgImportDate)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGIMPORTDATE_H__8C744F61_EE6F_11D3_AB1B_0080AD88D050__INCLUDED_)
