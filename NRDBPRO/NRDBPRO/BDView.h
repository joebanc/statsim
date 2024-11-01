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

#if !defined(AFX_BDVIEW_H__BDCE3521_7178_11D3_AA05_A8A609051770__INCLUDED_)
#define AFX_BDVIEW_H__BDCE3521_7178_11D3_AA05_A8A609051770__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BDView.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CBDView view

class CBDView : public CView
{
public:
	CBDView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CBDView)

   void SetExtent(int nWidth, int nHeight) {m_nWidth = nWidth; m_nHeight = nHeight;}

// Attributes
protected:
   static BOOL m_bCopy;
   int m_nWidth, m_nHeight;

// Operations

   void OnEditCopy();
   void ScaleFont(CDC* pDC, LOGFONT* pLogfont);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBDView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CBDView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CBDView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BDVIEW_H__BDCE3521_7178_11D3_AA05_A8A609051770__INCLUDED_)
