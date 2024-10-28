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
#if !defined(AFX_DLGPROJECTION_H__FDDB3121_4140_11D4_A590_0080AD88D050__INCLUDED_)
#define AFX_DLGPROJECTION_H__FDDB3121_4140_11D4_A590_0080AD88D050__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgProjection.h : header file
//

#include "editplus.h"

#define WM_UTMTIP WM_USER+1

/////////////////////////////////////////////////////////////////////////////
// CDlgProjection dialog

class CDlgProjection : public CDialog
{
// Construction
public:
	CDlgProjection(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgProjection)
	enum { IDD = IDD_PROJECTION };
	CButton	m_ckCoordAsLatLon;
	CEditPlus 	m_eScaleCoords;
	CComboBox	m_cbEllipsoid;
	CEditPlus	m_eMinNorthing;
	CEditPlus	m_eMinEasting;
	CEditPlus	m_eMaxNorthing;
	CEditPlus	m_eMaxEasting;
	CEditPlus	m_eSemiMinorAxis;
	CEditPlus	m_eSemiMajorAxis;
	CEditPlus	m_eScaleFactor;
	CEditPlus	m_eLongitudeOrigin;
	CEditPlus	m_eLatitudeOrigin;
	CEditPlus	m_eFalseNorthing;
	CEditPlus	m_eFalseEasting;
	CEditPlus	m_eName;
	CComboBox	m_cbProjections;
	//}}AFX_DATA
   
   long m_lMaxId;
   long m_lDefaultId;
   BOOL m_bLatLon;
   BOOL m_bUpdate;

   void Update(BOOL bInit = TRUE);
   void UpdateDefault();
   BOOL AddEdit(BOOL bAdd, long lId);
   BOOL m_bUTMTip;
   

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgProjection)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgProjection)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeName();
	afx_msg void OnAdd();
	virtual void OnOK();
	afx_msg void OnEdit();
	afx_msg void OnDelete();
	afx_msg void OnSelchangeEllipsoid();
	afx_msg void OnChange();
   afx_msg void OnSetExtent();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   LRESULT OnUTMTip(WPARAM, LPARAM);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPROJECTION_H__FDDB3121_4140_11D4_A590_0080AD88D050__INCLUDED_)
