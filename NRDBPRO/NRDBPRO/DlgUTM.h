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
#if !defined(AFX_DLGUTM_H__DAC632A0_32FF_11D6_AD24_CF168B32AF0A__INCLUDED_)
#define AFX_DLGUTM_H__DAC632A0_32FF_11D6_AD24_CF168B32AF0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgUTM.h : header file
//

#include "buttonutm.h"

#define WM_SETZONE WM_USER+1

/////////////////////////////////////////////////////////////////////////////
// CDlgUTM dialog

class CDlgUTM : public CDialog
{
// Construction
public:
	CDlgUTM(CWnd* pParent = NULL);   // standard constructor

   int GetZone() {return m_nZone;}

protected:

   int m_nZone;


// Dialog Data
	//{{AFX_DATA(CDlgUTM)
	enum { IDD = IDD_UTM };
	CButtonUTM	m_pbUTM;
	CComboBox	m_cbZone;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgUTM)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgUTM)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeZone();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   LRESULT OnSetZone(WPARAM, LPARAM);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGUTM_H__DAC632A0_32FF_11D6_AD24_CF168B32AF0A__INCLUDED_)
