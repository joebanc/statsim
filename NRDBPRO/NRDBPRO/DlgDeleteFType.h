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
#if !defined(AFX_DLGDELETEFTYPE_H__86062C91_E8F9_11D3_AB15_0080AD88D050__INCLUDED_)
#define AFX_DLGDELETEFTYPE_H__86062C91_E8F9_11D3_AB15_0080AD88D050__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDeleteFType.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgDeleteFType dialog

class CDlgDeleteFType : public CDialog
{
// Construction
public:
	CDlgDeleteFType(CWnd* pParent = NULL);   // standard constructor

   BOOL IsDeleteData() {return m_bDeleteData;}
   BOOL IsDeleteFeatures() {return m_bDeleteFeatures;}
   BOOL IsDeleteFType() {return m_bDeleteFType;}

protected:

   BOOL m_bDeleteData;
   BOOL m_bDeleteFeatures;
   BOOL m_bDeleteFType;

// Dialog Data
	//{{AFX_DATA(CDlgDeleteFType)
	enum { IDD = IDD_DELETE };
	CButton	m_ckDeleteFType;
	CButton	m_ckDeleteFeature;
	CButton	m_ckDeleteData;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDeleteFType)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgDeleteFType)
	virtual void OnOK();
	afx_msg void OnClickDelete();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDELETEFTYPE_H__86062C91_E8F9_11D3_AB15_0080AD88D050__INCLUDED_)
