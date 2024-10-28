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

#if !defined(AFX_BUTTONANIMATE_H__41DADDF1_7C5B_11D3_AA1E_98BCBB3EE770__INCLUDED_)
#define AFX_BUTTONANIMATE_H__41DADDF1_7C5B_11D3_AA1E_98BCBB3EE770__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ButtonAnimate.h : header file
//

#include "imagefile.h"

/////////////////////////////////////////////////////////////////////////////

#define NUMIMAGES 17

/////////////////////////////////////////////////////////////////////////////
// CButtonAnimate window

class CButtonAnimate : public CButton
{
// Construction
public:
	CButtonAnimate();

   void Init(int nBitmap);

   static void DrawBitmap(CDC* pDC, CBitmap& bitmap, CRect& rect, int nWidth,  int nHeight);
   
// Attributes
protected:

   CDC m_aDC[NUMIMAGES];
   int m_nTimer;
   int m_nImage;
   
   static CImageFile m_imagefile;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CButtonAnimate)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CButtonAnimate();

	// Generated message map functions
protected:
	//{{AFX_MSG(CButtonAnimate)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUTTONANIMATE_H__41DADDF1_7C5B_11D3_AA1E_98BCBB3EE770__INCLUDED_)
