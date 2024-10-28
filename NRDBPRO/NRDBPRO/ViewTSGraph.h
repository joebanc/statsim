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

#if !defined(AFX_VIEWTSGRAPH_H__1B73A301_0B4E_11D4_AB57_0080AD88D050__INCLUDED_)
#define AFX_VIEWTSGRAPH_H__1B73A301_0B4E_11D4_AB57_0080AD88D050__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewTSGraph.h : header file
//

#include "viewgraph.h"
#include "doctsgraph.h"

/////////////////////////////////////////////////////////////////////////////
// CViewTSGraph view

class CViewTSGraph : public CViewGraph
{
protected:
	CViewTSGraph();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CViewTSGraph)

// Attributes
public:

// Operations
protected:

   CDocTSGraph* GetDoc() {return (CDocTSGraph*)GetDocument();}
   BOOL LabelXAxis(CDC* pDC);
   CString GetMonthName(int nMonth);
   void DrawTextC(CDC*, LPCSTR s, int x, int y);
   
   virtual void DrawXAxis(CDC* pDC);
   virtual void DrawGraph(CDC* pDC);

   CDateTime m_dtStart, m_dtEnd;
   //double m_dXMin, m_dXMax;

   BOOL m_bDays;
   BOOL m_bMonthsFull;
   BOOL m_bMonths;
   BOOL m_bDecades;
   double m_dTextHeight, m_dTextWidth;
   double m_dOriginX,  m_dOriginY;
   double m_dLenX;
   double m_dTickLength;
   double m_dDayY;
   double m_dMonthY;
   double m_dYearY;
   double m_dRange;
   
   C2dDblArray m_aXPosData;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewTSGraph)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CViewTSGraph();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CViewTSGraph)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWTSGRAPH_H__1B73A301_0B4E_11D4_AB57_0080AD88D050__INCLUDED_)
