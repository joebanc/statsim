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
#if !defined(AFX_PAGEMAPTEXTPROP_H__757ED872_A0BA_11D4_A6C7_0080AD88D050__INCLUDED_)
#define AFX_PAGEMAPTEXTPROP_H__757ED872_A0BA_11D4_A6C7_0080AD88D050__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageMapTextProp.h : header file
//

#include "maplayer.h"
#include "bdcombobox.h"
#include "buttoncolor.h"

/////////////////////////////////////////////////////////////////////////////
// CPageMapTextProp dialog

class CPageMapTextProp : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageMapTextProp)

// Construction
public:
	CPageMapTextProp(CMapLayer*, CMapProperties*);
   CPageMapTextProp();
	~CPageMapTextProp();

protected:
   CMapLayer* m_pMapLayer;
   CMapProperties* m_pMapProperty;
   LOGFONT m_logfont;

// Dialog Data
	//{{AFX_DATA(CPageMapTextProp)
	enum { IDD = IDD_MAPTEXTPROP };
	CButton	m_ckScaleText;	
	CButtonColour	m_pbTextColour;	
	CBDComboBox	m_cbTextPos;
	CButton	m_pbFont;	
	CButton	m_ckOverlapText;	
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageMapTextProp)
	public:
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
   virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageMapTextProp)		
	virtual BOOL OnInitDialog();
	afx_msg void OnFont();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEMAPTEXTPROP_H__757ED872_A0BA_11D4_A6C7_0080AD88D050__INCLUDED_)
