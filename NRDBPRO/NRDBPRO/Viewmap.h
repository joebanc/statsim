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

#ifndef _MAPVIEW_H_
#define _MAPVIEW_H_

#include "bdview.h"
#include "maplayer.h"
#include "imagefile.h"
#include "array.h"
#include "dlgviewattr.h"

///////////////////////////////////////////////////////////////////////////////

#define NUMIMAGESMAP 24
#define AUTOSCALE 5

///////////////////////////////////////////////////////////////////////////////

struct CRectText
{
   CString m_sText;
   CRect m_rect;
   int m_iPos;   
   int m_nX, m_nY;   
};

///////////////////////////////////////////////////////////////////////////////

class CViewMap : public CBDView
{
friend class CViewLegend;
friend class CDlgMapLayers;
friend class CMapLayerArray;

protected: // create from serialization only
	CViewMap();
	DECLARE_DYNCREATE(CViewMap)

// Attributes
public:
	CDocMap* GetDocument();
   
   static int GetLineWidth(CDC* pDC, int nLineStyle, int nLineWidth);
   static int GetLineWidth(CDC* pDC, CMapLayer* pMapLayer);

   void DrawRect(CDC* pDC, CRect, BOOL bLocator = FALSE);

   void InitialiseMapView();

   void CheckExtent(CMapLayerArray*);   

   void Redraw();   
      
   static COLORREF GetColour(double dValue, double dMin, double dMax, COLORREF cr);

   static void GetStyle(CMapLayer*, CMapLayerObj*, double d, CMapStyle &mapstyle);

   CRect GetRect() {return m_rect;}
   CRect GetRectPrint() {return m_rectP;}
   void SetRectPrint(CRect rect) {m_rectP = rect;}
   void InitPrint(CDC* pDC);
   int DrawTitle(CDC* pDC, CRect* pRect = NULL);
   int GetScale(CDC* pDC);
   void SetCopyRect(CDC* pDC);   
   void SetMode(int nMode) {m_nMode = nMode;}
   void SaveImage(LPCSTR sFileName);

   enum {none, addnew=1, polyline=2, polygon=4, points=8};
    
   int GetfXPos(long a);
   int GetfYPos(long a);
   long GetfXInv(int a);
   long GetfYInv(int a);
  
protected:
   CDocMap* m_pDoc;

   CArray <CRect, CRect> m_aRectText;
   CArray <CRect, CRect> m_aRectSym;
   CArray <CRectText, CRectText> m_aRectTextL;   
   CArray <CMapLayerObj*,CMapLayerObj*> m_aSearchObj;

   void DrawLayers(CDC* pDC);
   void DrawMapLines(CDC* pDC, CMapLayerObj*, CMapLayer*, BOOL bSearch = FALSE);
   void DrawLines(CDC* pDC, POINT* pPoints, int nPoints);
   void DrawCoords(CDC* pDC, CMapLayerObj* pMapLayer, CMapLayer*, BOOL bSearch = FALSE);
   void DrawLogo(CDC* pDC);      
   void DrawTapeMeasure(CDC* pDC);
   void FitText(CDC* pDC, CMapLayerObj* pMapLayerObj, CMapLayer* pMapLayer, int x, int y);
   void DrawText(CDC* pDC, CMapLayer*);
   void DrawLayerText(CDC* pDC, CMapLayer* pMapLayer);
   void DrawRect(CDC* pDC, CRect* rect, COLORREF cr, int nStyle = PS_SOLID, int nWidth=1);
   void DrawCoords(CDC* pDC);
   void DrawCoordsLat(CDC* pDC);
   void DrawCoordsLng(CDC* pDC);
   void DrawScale(CDC* pDC);      
   void DrawNorthArrow(CDC* pDC);
   void DrawMapEditLines(CDC* pDC, CLongLines* pMapLines, CMapLayer* pMapLayer);
   void EditPolylines(const CLongLines& longlines, BOOL bAdd);
   void DrawImage(CDC* pDC, CMapLayerObj* pMapLayerObj, CMapLayer* pMapLayer);
   void ClipPolygon(int& i, CLongLines*, CPointArray& aPoints);
   CRect GetScreenRect(CDC* pDC);
   void GetExtent(CMapLayer*, CMapLayerObj*);      
   BOOL CreateTracker(CPoint);
   BOOL RemoveTracker();
   void OnViewZoomRect();
   CMapLayerObj* HitTest(CPoint, CMapLayer*&);
   void UpdateMapObject(CMapLayer* pMapLayer, CMapLayerObj* pMapLayerObj);
   void SetScrollPos();   
   BOOL CheckOverlap(CDC*, CMapLayer* pMapLayer, LPCSTR sText, int x, int y,  CRect&, int& iPos);   
   void CheckOverlapSym(CDC*, double dSize, double& dX, double& dY);   
   void DetermineAspect(CDC* pDC = NULL);   
   BOOL IsVisible(CDC* pDC, CMapLayerObj*, BOOL bText = FALSE);   
   BOOL EditMode(BOOL bStart, BOOL bAutoSave = FALSE);
   void AdjustEditParams();
   BOOL UpdateMapObj(BOOL bAutoSave = FALSE);   
   void DrawLayoutBorders(CDC *pDC);
   void DrawLayoutImages(CDC *pDC);
   void DrawLayoutText(CDC *pDC);
   void DrawLayoutLocator(CDC *pDC);
   void DrawWireFrame(CDC *pDC, CMapLayer *pMapLayer, CMapLayerObj *pMapLayerObj);
   void SnapPoint(CPoint point, CLongCoord, CLongLines&);
   BOOL IsAutoUpdate() {return m_bAutoUpdate;}
   void SetAutoUpdate(BOOL b) {m_bAutoUpdate = b;}
   BOOL OnPolyline(CPoint point, const CLongLines& longlines);

   enum {gridleft, gridright, gridtop, gridbottom};
   enum {pan=1, measure};

   
// Attributes
   
   double m_dZoom, m_dZoomP;
   double m_dOffXP, m_dOffYP;
   double m_dMinX, m_dMaxX;
   double m_dMinY, m_dMaxY;
   double m_dOffX, m_dOffY;   
   BOOL m_bDefaultExtentNull;
   int m_nMode;   
   int m_iEditPoint1, m_iEditPoint2;
   BOOL m_bAutoUpdate;

   static CString m_sLogoText;
   static CString m_sLogoFile;

   CRect m_rect;
   CRect m_rectP;      
   CRect m_rectA; // Aspect rect
   CRect m_rectPaint;
   CRectTracker* m_pRectTracker;
   CMapLayerObj* m_pEditMapObj;
   CMapLayer* m_pEditMapLayer;
   HCURSOR m_hDragCursor;
   HCURSOR m_hDragCursor2;
   HCURSOR m_hCursorOld;
   HCURSOR m_hPanCursor;
   HCURSOR m_hCurSel;
   HCURSOR m_hCurIns;
   BOOL m_bDragMode;
   BOOL m_nMapMode;
   BOOL m_bInsertMode;
   BOOL m_bWinNT;
   BOOL m_bWideLine;   
   CImageFile m_imagefile;   
   CPoint m_ptPan;
   CArray <CLongCoord, CLongCoord> m_aMeasure;
   CPoint m_ptMeasure1, m_ptMeasure2;
   BOOL m_bViewFile;
   BOOL m_bViewHotLink;

   int m_nCurPage;   

   // Editing

   CMenu m_menuedit;   
   CFeature m_feature;   
   CMapLayerObj* m_pMapLayerObj;
   CMapLayer* m_pMapLayer;
   POINT m_point;      
   CLongLines* m_pMapLinesEdit;
   CCoord* m_pMapCoordEdit;
   CDlgViewAttr m_dlgViewAttr;

   int m_nSearch;   
   BOOL m_bLocator;
     
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewMap)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);		
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL	

public:
   virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
   virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CViewMap();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
public:
	//{{AFX_MSG(CViewMap)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnViewZoomin();
	afx_msg void OnViewZoomnormal();
	afx_msg void OnViewZoomout();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEditCopy();
	afx_msg void OnDestroy();		
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMapEditdata();
	afx_msg void OnMapEditfeature();
	afx_msg void OnMapAddFeature();
	afx_msg void OnUpdateMapAddFeature(CCmdUI* pCmdUI);
   afx_msg void OnMapSectoralReport();
   afx_msg void OnUpdateMapSectoralReport(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnFilePrintPreview();
	afx_msg void OnFilePrint();
	afx_msg void OnPaint();
	afx_msg void OnEditSearch();
	afx_msg void OnUpdateEditSearch(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMapEditdata(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMapEditfeature(CCmdUI* pCmdUI);	
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMapEditlines();
	afx_msg void OnUpdateMapEditlines(CCmdUI* pCmdUI);
	afx_msg void OnMapEditpoints();
	afx_msg void OnUpdateMapEditpoints(CCmdUI* pCmdUI);
	afx_msg void OnMapPan();
	afx_msg void OnUpdateMapPan(CCmdUI* pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMapMeasure();
	afx_msg void OnUpdateMapMeasure(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMapAutoupdate();
	afx_msg void OnUpdateMapAutoupdate(CCmdUI* pCmdUI);
	afx_msg void OnMapUpdate();
	afx_msg void OnUpdateMapUpdate(CCmdUI* pCmdUI);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMapInsert();
	afx_msg void OnUpdateMapInsert(CCmdUI* pCmdUI);
	afx_msg void OnMapNewline();
	afx_msg void OnUpdateMapNewline(CCmdUI* pCmdUI);
	afx_msg void OnMapViewfile();
	afx_msg void OnUpdateMapViewfile(CCmdUI* pCmdUI);
	afx_msg void OnMapViewAttr();
	afx_msg void OnUpdateMapViewAttr(CCmdUI* pCmdUI);
	//}}AFX_MSG
   void OnLButtonUp(UINT nFlags, CPoint point); // Disabled
   void OnUpdateDragmode(CCmdUI* pCmdUI);
   //void OnDragmode();
   afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in MapView.cpp
inline CDocMap* CViewMap::GetDocument()
   { return (CDocMap*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

#endif
