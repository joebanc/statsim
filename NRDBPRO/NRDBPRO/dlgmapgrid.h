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
#if !defined(AFX_MAPGRID_H__BF2CCCA1_F6BB_11E1_B409_000795C2378F__INCLUDED_)
#define AFX_MAPGRID_H__BF2CCCA1_F6BB_11E1_B409_000795C2378F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMapGrid.h : header file
//

#include "comboboxlines.h"
#include "buttoncolor.h"
#include "bdcombobox.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgMapGrid dialog

class CDlgMapGrid : public CDialog
{
// Construction
public:
	CDlgMapGrid(CMapGrid mapgrid, CWnd* pParent = NULL);   // standard constructor

   CMapGrid GetMapGrid() {return m_mapgrid;}

protected:

   CMapGrid m_mapgrid;

// Dialog Data
	//{{AFX_DATA(CDlgMapGrid)
	enum { IDD = IDD_MAPGRID };
	CEditPlus	m_eSecLng;
	CEditPlus	m_eSecLat;
	CEditPlus	m_eMinLng;
	CEditPlus	m_eMinLat;
	CEditPlus	m_eDegLng;
	CEditPlus	m_eDegLat;
	CButtonColour	m_pbColour;
	CBDComboBox	m_cbAppearance;
	CComboBoxLines	m_cbLineStyle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMapGrid)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgMapGrid)
	virtual BOOL OnInitDialog();
	afx_msg void OnFont();
	virtual void OnOK();
	afx_msg void OnSelchangeAppearance();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPGRID_H__BF2CCCA1_F6BB_11E1_B409_000795C2378F__INCLUDED_)
