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
#if !defined(AFX_DLGADDOVERLAY_H__BC9461A0_1CA9_11D6_885C_0050BA4A2B07__INCLUDED_)
#define AFX_DLGADDOVERLAY_H__BC9461A0_1CA9_11D6_885C_0050BA4A2B07__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAddOverlay.h : header file
//

#include "maplayer.h"
#include "buttoncolor.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgAddOverlay dialog

class CDlgAddOverlay : public CDialog
{
// Construction
public:
	CDlgAddOverlay(CMapLayer* pMapLayer, LPCSTR sFileName = NULL, LPCSTR sColumn = NULL, CWnd* pParent = NULL);   // standard constructor

protected:

	CMapLayer* m_pMapLayer;
	CString m_sDataSource;
	CString m_sTable;
	CString m_sPath;
	CString m_sColumn;
   BOOL m_bDefault;

// Dialog Data
	//{{AFX_DATA(CDlgAddOverlay)
	enum { IDD = IDD_ADDLAYER };
	CComboBox	m_cbMapObj;
	CComboBox	m_cbFType;
	CButtonColour	m_pbColour;
	CComboBox	m_cbLabel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAddOverlay)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAddOverlay)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGADDOVERLAY_H__BC9461A0_1CA9_11D6_885C_0050BA4A2B07__INCLUDED_)
