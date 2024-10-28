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
#if !defined(AFX_VIEWGRAPH_H__DFBFFE80_38F3_11D3_A9BA_E7179BA68B70__INCLUDED_)
#define AFX_VIEWGRAPH_H__DFBFFE80_38F3_11D3_A9BA_E7179BA68B70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewGraph.h : header file
//

#include "docgraph.h"
#include "bdview.h"

/////////////////////////////////////////////////////////////////////////////
// CViewGraph view

typedef CArray <double, double> CDblArray;
typedef CArray <CDblArray, CDblArray> C2dDblArray;
typedef CArray <COLORREF, COLORREF> CColourArray;
typedef CArray <int, int> CIntArray;

/////////////////////////////////////////////////////////////////////////////

class CGraphProperties
{
public:
   CString m_sTitle;
   CStringArray m_aLegendText;
   CColourArray m_aColour;
   CIntArray m_aLineWidth;
   CIntArray m_aLineStyle;
   CIntArray m_aGraphStyle;
   int m_nType;
   LOGFONT m_logfont;
   double m_dXMin, m_dXMax;
   double m_dYMin, m_dYMax;
   int m_nYPoints;

   enum {lines=1, sticks, symbols, columns};

   CGraphProperties();
   CGraphProperties& operator=(CGraphProperties& rSrc);
};



/////////////////////////////////////////////////////////////////////////////

class CViewGraph : public CBDView
{
protected:
	CViewGraph();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CViewGraph)

// Attributes
public:

// Operations
public:
   CDocGraph* m_pDoc;

protected:

   C2dDblArray m_aGraphData;
   CStringArray m_aLabelText;
   
   CRect m_rectAxis, m_rect;
   BOOL m_bData;
   CGraphProperties m_prop;

   void DetermineScale();
   void DetermineScale(double dMin, double dMax, double& dSMin, double& dSMax, int& nPoints);
   void DrawYAxis(CDC* pDC);
   void DrawTitle(CDC* pDC, int nAdjustV = 100);
   void DrawLegend(CDC* pDC, int nAdjustV = 100, int nLegendW = 5);
   void CreateFont(CDC* pDC, CFont&, int nSize, BOOL bVert = FALSE);
   void ScaleFont(CDC* pDC, int& nFont, LPCSTR s, int nWidth, int nHeight=0);
   void DrawAxis(CDC* pDC);

   int GetXPos(int nWidth, int i, int j);

   virtual void DrawXAxis(CDC* pDC);
   virtual void DrawGraph(CDC* pDC);
   virtual int GetNumLegend() {return m_pDoc->GetNumLines();}
   virtual CString GetLegend(int iLine) {return m_pDoc->GetLegend(iLine);}

   COLORREF GetColour(int i);
   int GetWidth(CDC* pDC, int nStyle, int nWidth);




// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewGraph)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CViewGraph();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CViewGraph)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileSaveAs();
	afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);	
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnGraphProp();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWGRAPH_H__DFBFFE80_38F3_11D3_A9BA_E7179BA68B70__INCLUDED_)
