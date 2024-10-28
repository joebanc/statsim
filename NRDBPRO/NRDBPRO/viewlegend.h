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
#if !defined(AFX_VIEWLEGEND_H__5CB1BE62_1345_11D3_ADF1_44C1F2C00000__INCLUDED_)
#define AFX_VIEWLEGEND_H__5CB1BE62_1345_11D3_ADF1_44C1F2C00000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewLegend.h : header file
//

#include "bdview.h"
#include "docmap.h"

/////////////////////////////////////////////////////////////////////////////

#define MAP_UPDATEY 1

/////////////////////////////////////////////////////////////////////////////
// CViewLegend view

class CViewLegend : public CBDView
{
protected:
	CViewLegend();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CViewLegend)

   void DrawLegend(CDC* pDC) {OnDraw(pDC);}   

// Attributes
protected:
   class CDocMap* m_pDoc;
   CRect m_rect;
   int m_nBorder;
   int m_nTextPos;
   int m_nHeight;
   CWordArray m_anLayerPos;
   int m_nDragLayer;
   int m_nColumn;
   int m_nCols;
   HCURSOR m_hCursorKey, m_hCursorOld; 
   
   int m_nBottomY;


   BOOL DrawMapLineLegend(CDC* pDC, CMapLayer* pMapLayerObj, int& cy, LPCSTR s, 
                          CMapStyle mapstyle);
   BOOL DrawCoordLegend(CDC*  pDC, CMapLayer* pMapLayerObj, int& cy, LPCSTR s, CMapStyle mapstyle);
   void DrawProjection(CDC* pDC, int& cy);
   void DrawScale(CDC* pDC, int& cy);
   void DrawScaleBar(CDC* pDC, int& cy);
   void DrawAutoColour(CDC* pDC, CMapLayer*, int& cy, CMapStyle mapstyle);   
   void DrawAutoSize(CDC* pDC, CMapLayer*, int& cy);   
   void DrawAutoScale(CDC* pDC, CMapLayer*, int& cy);   
   void DrawNorthArrow(CDC* pDC, int& cy);
   BOOL DrawText(CDC* pDC, int cx, int& cy, LPCSTR sText, int nFlags = MAP_UPDATEY);
   void DrawComments(CDC* pDC, int& cy);
   void DrawLegendValues(CDC* pDC, CMapLayer* pMapLayer, int& cy, CString s, CMapStyle mapstyle);
   void DrawBorder(CDC* pDC, CRect, int nPen = PS_DOT, int nWidth = 1, int nColour=RGB(128,128,128));
   int GetLayer(int cy);

   void CreateFont(CDC* pDC, CFont&, int nHeight);

   BOOL m_bCancel;

   CDocMap* GetDoc() {return (CDocMap*)GetDocument();}

   enum {null = -3, start = -2, end = -1};

// Operations

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewLegend)
	public:
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL
   virtual void OnDraw(CDC* pDC);      // overridden to draw this view

// Implementation
protected:
	virtual ~CViewLegend();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CViewLegend)
	afx_msg void OnEditCopy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnFilePrintPreview();
	afx_msg void OnFilePrint();
	afx_msg void OnViewZoomin();
	afx_msg void OnViewZoomnormal();
	afx_msg void OnViewZoomout();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMapPan();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWLEGEND_H__5CB1BE62_1345_11D3_ADF1_44C1F2C00000__INCLUDED_)
