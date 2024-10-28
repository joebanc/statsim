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
#if !defined(AFX_DLGCOLOURRANGE_H__F6FEB1A1_EE1F_11E1_B3FA_000795C2378F__INCLUDED_)
#define AFX_DLGCOLOURRANGE_H__F6FEB1A1_EE1F_11E1_B3FA_000795C2378F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgColourRange.h : header file
//

#include "maplayer.h"
#include "buttoncolor.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgColourRange dialog

class CDlgColourRange : public CDialog
{
// Construction
public:
	CDlgColourRange(CMapLayer*, CMapProperties*, BOOL bDefault = FALSE, CWnd* pParent = NULL);   // standard constructor

protected:

   void EqualIntervals(CArray <double, double>&, int nIntervals);
   void NaturalBreaks(CArray <double, double>&, int nIntervals);
   void Percentiles(CArray <double, double>&, int nIntervals);
   void DefaultBreaks(CArray <double, double>&, int& nIntervals);

   double CalcSpread(CArray <double, double> &aRangeColor);

   CMapProperties *m_pMapProp;
   CMapLayer *m_pMapLayer;
   BOOL m_bDefault;

   enum {defaultbreaks=0, equalintervals=1, percentiles=2, naturalbreaks=3};

// Dialog Data
	//{{AFX_DATA(CDlgColourRange)
	enum { IDD = IDD_COLORRANGE };
	CButtonColour	m_pbColour;
	CComboBox	m_cbRound;
	CEditPlus	m_eIntervals;
	CComboBox	m_cbMethod;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgColourRange)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgColourRange)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeMethod();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCOLOURRANGE_H__F6FEB1A1_EE1F_11E1_B3FA_000795C2378F__INCLUDED_)
