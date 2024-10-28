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
#if !defined(AFX_VIEWMAPLAYOUT_H__F6F2AF42_EE38_11E1_B3FA_000795C2378F__INCLUDED_)
#define AFX_VIEWMAPLAYOUT_H__F6F2AF42_EE38_11E1_B3FA_000795C2378F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewMapLayout.h : header file
//

#include "maplayer.h"
#include "docmaplayout.h"

/////////////////////////////////////////////////////////////////////////////
// CViewMapLayout view

class CViewMapLayout : public CView
{
protected:
	CViewMapLayout();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CViewMapLayout)

// Attributes
protected:

   CMapLayoutObj* m_pLayoutObj;
   CRectTracker* m_pRectTracker;
   int m_nWidth;
   int m_nHeight;
   CRect m_rect;
   BOOL m_bOverlayMap;

// Operations
protected:
   
   void GetPrinterInfo();
   void CreateTracker();
   void CreateLayoutObj(int nType);
   void CreateLayoutObjMult(int nType);
   CMapLayoutObj* HitTest(CPoint pt);

   CDocMapLayout* GetDoc() {return (CDocMapLayout*)GetDocument();}
   CMapLayout* GetLayout() {return GetDoc()->GetMapLayout();}

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewMapLayout)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CViewMapLayout();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CViewMapLayout)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLayoutMap();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLayoutArrow();
	afx_msg void OnLayoutLegend();
	afx_msg void OnLayoutProjection();
	afx_msg void OnLayoutScalebar();
	afx_msg void OnLayoutTitle();
	afx_msg void OnLayoutSelect();
	afx_msg void OnLayoutBorder();
	afx_msg void OnLayoutSource();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLayoutPicture();
	afx_msg void OnLayoutText();
	afx_msg void OnLayoutScale();
	afx_msg void OnLayoutPreview();
	afx_msg void OnUpdateLayoutPreview(CCmdUI* pCmdUI);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	afx_msg void OnMapLocator();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWMAPLAYOUT_H__F6F2AF42_EE38_11E1_B3FA_000795C2378F__INCLUDED_)
