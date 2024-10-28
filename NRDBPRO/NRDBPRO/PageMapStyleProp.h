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
#if !defined(AFX_PAGEMAPSTYLEPROP_H__757ED874_A0BA_11D4_A6C7_0080AD88D050__INCLUDED_)
#define AFX_PAGEMAPSTYLEPROP_H__757ED874_A0BA_11D4_A6C7_0080AD88D050__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageMapStyleProp.h : header file
//

#include "maplayer.h"
#include "comboboxlines.h"
#include "comboboxsymbol.h"
#include "comboboxpattern.h"
#include "buttoncolor.h"

/////////////////////////////////////////////////////////////////////////////
// CPageMapStyleProp dialog

class CPageMapStyleProp : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageMapStyleProp)

// Construction
public:
	CPageMapStyleProp(CMapLayer*, CMapProperties*);
   CPageMapStyleProp();
	~CPageMapStyleProp();

protected:
   CMapLayer* m_pMapLayer;
   CMapProperties* m_pMapProperty;
   BOOL m_bCoord, m_bMapLines;
   CBitmap m_bitmapPopup;

// Dialog Data
	//{{AFX_DATA(CPageMapStyleProp)
	enum { IDD = IDD_MAPSTYLESPROP };	
	CComboBox	m_cbSymSize;
	CButton	m_pbSymbolMenu;
	CComboBoxLines	m_cbLineStyle;	
	CComboBoxSymbol	m_cbSymbol;
   CComboBoxPattern	m_cbPattern;
   CButton	m_ckBestFitSym;	
	CButton	m_ckPolygon;		
	//}}AFX_DATA

	CButtonColour	m_pbColourLine;
	CButtonColour	m_pbColourFill;
	CButton	m_ckAutoFill;
   CButton	m_ckAutoLine;
	CButton	m_ckAutoSym;
	
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageMapStyleProp)
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
	//{{AFX_MSG(CPageMapStyleProp)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeLinestyle();
	afx_msg void OnSymbolMenu();
	afx_msg void OnSymbolsAdd();
	afx_msg void OnSymbolsDelete();
	afx_msg void OnSelchangeFillstyle();
	afx_msg void OnColorline();
	afx_msg void OnColorfill();
	afx_msg void OnSelchangeSymbol();
	afx_msg void OnSelchangeSymsize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   afx_msg void OnAutoColour();   
   afx_msg void OnCustomSymbol();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEMAPSTYLEPROP_H__757ED874_A0BA_11D4_A6C7_0080AD88D050__INCLUDED_)
