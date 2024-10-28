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

#if !defined(AFX_BUTTONUTM_H__DAC632A2_32FF_11D6_AD24_CF168B32AF0A__INCLUDED_)
#define AFX_BUTTONUTM_H__DAC632A2_32FF_11D6_AD24_CF168B32AF0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ButtonUTM.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CButtonUTM window

class CButtonUTM : public CButton
{
// Construction
public:
	CButtonUTM();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CButtonUTM)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CButtonUTM();
   void DrawItem(LPDRAWITEMSTRUCT);

   void SetZone(int nZone);

protected: 

   int m_nZone;
   CRect m_rect;

	// Generated message map functions
protected:
	//{{AFX_MSG(CButtonUTM)	
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUTTONUTM_H__DAC632A2_32FF_11D6_AD24_CF168B32AF0A__INCLUDED_)
