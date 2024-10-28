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

#if !defined(AFX_BUTTONCOLOR_H__6204B1A0_3510_11D3_ADF1_44C1F2C00000__INCLUDED_)
#define AFX_BUTTONCOLOR_H__6204B1A0_3510_11D3_ADF1_44C1F2C00000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ButtonColour.h : header file
//

#define BS_AUTO 1

// Message to notify parent that button's value has changed
#define BN_CHANGED WM_USER
#define ON_BN_CHANGED(id, memberFxn) ON_CONTROL(BN_CHANGED, id, memberFxn)

/////////////////////////////////////////////////////////////////////////////
// CButtonColour window

class CButtonColour : public CButton
{
// Construction
public:
	CButtonColour(COLORREF cr = RGB(0,0,0));

   COLORREF GetColour() {return m_bAuto ? -1 : m_color;}
   void SetColour(COLORREF);
   void SetStyle(int nFlag) {m_nStyle = nFlag;}   

// Attributes
protected:
   COLORREF m_color;
   BOOL m_bAuto;
   int m_nStyle;

// Operations
public:
   virtual void DrawItem(LPDRAWITEMSTRUCT);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CButtonColour)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CButtonColour();

	// Generated message map functions
protected:
	//{{AFX_MSG(CButtonColour)
	afx_msg void OnClicked();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUTTONCOLOR_H__6204B1A0_3510_11D3_ADF1_44C1F2C00000__INCLUDED_)
