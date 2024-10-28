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

#include "stdafx.h"
#include <afxpriv.h>

#include "nrdbpro.h"
#include "nrdb.h"
#include "docmap.h"
#include "viewmap.h"
#include "mainfrm.h"
#include "viewlegend.h"
#include "projctns.h"
#include "dlgeditattributes.h"
#include "dlgeditfeature.h"
#include "clip.h"
#include "definitions.h"
#include "comboboxpattern.h"
#include "comboboxsymbol.h"
#include "docsummary.h"
#include "dlgsearch.h"
#include "dlgimportdate.h"
#include "dlgdigitisetype.h"
#include "shapefile.h"
#include "dlgimageoptions.h"
#include "fileobj.h"
#include "spatial.h"
#include "viewlocator.h"
#include "dlgmapfeaturesel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////

#define DEFAULT_ZOOM 0.8
#define MAX_ZOOM 4000000 // metres (400km / UTM zone)
#define MAX_ZOOMLATLON 40000000// circumference of earth
#define MAX_PAN 2000000
#define MIN_ZOOM 10 // metres
#define SYMSIZE 10
#define SCROLLRANGE 1000
#define MMPERINCH 25.4
#define FITTHRESH 1000
#define WIN98POLYLINE 16383
#define WIDELINELEN 1360
#define NODESIZE 2
#define EPS 1e-10

/////////////////////////////////////////////////////////////////////////////

double inline square(double x) {return x*x;};
void inline swap(long& a, long &b) {long c=a;a=b;b=c;}
void inline swap(double& a, double &b) {double c=a;a=b;b=c;}

void inline DrawNode(CDC* pDC, CPoint point)
{
   pDC->MoveTo(point.x-NODESIZE, point.y-NODESIZE);
   pDC->LineTo(point.x+NODESIZE, point.y-NODESIZE);
   pDC->LineTo(point.x+NODESIZE, point.y+NODESIZE);
   pDC->LineTo(point.x-NODESIZE, point.y+NODESIZE);
   pDC->LineTo(point.x-NODESIZE, point.y-NODESIZE);
};


/////////////////////////////////////////////////////////////////////////////

CString CViewMap::m_sLogoText;
CString CViewMap::m_sLogoFile;

/////////////////////////////////////////////////////////////////////////////

struct CDblPoint
{
   double x;
   double y;
};

/////////////////////////////////////////////////////////////////////////////
// CViewMap

IMPLEMENT_DYNCREATE(CViewMap, CBDView)

BEGIN_MESSAGE_MAP(CViewMap, CBDView)
	//{{AFX_MSG_MAP(CViewMap)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_VIEW_ZOOMIN, OnViewZoomin)
	ON_COMMAND(ID_VIEW_ZOOMNORMAL, OnViewZoomnormal)
	ON_COMMAND(ID_VIEW_ZOOMOUT, OnViewZoomout)
	ON_WM_TIMER()	
    ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_KILLFOCUS() 	
    ON_WM_VSCROLL()   
	ON_WM_HSCROLL()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_WM_DESTROY()		   
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_MAP_EDITDATA, OnMapEditdata)
	ON_COMMAND(ID_MAP_EDITFEATURE, OnMapEditfeature)
	ON_COMMAND(ID_MAP_ADDFEATURE, OnMapAddFeature)
	ON_UPDATE_COMMAND_UI(ID_MAP_ADDFEATURE, OnUpdateMapAddFeature)
    ON_COMMAND(ID_MAP_SECTORALREPORT, OnMapSectoralReport)
   ON_UPDATE_COMMAND_UI(ID_MAP_SECTORALREPORT, OnUpdateMapSectoralReport)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_WM_PAINT()
	ON_COMMAND(ID_EDIT_SEARCH, OnEditSearch)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SEARCH, OnUpdateEditSearch)
	ON_UPDATE_COMMAND_UI(ID_MAP_EDITDATA, OnUpdateMapEditdata)
	ON_UPDATE_COMMAND_UI(ID_MAP_EDITFEATURE, OnUpdateMapEditfeature)	
	ON_WM_KEYDOWN()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_MAP_EDITLINES, OnMapEditlines)
	ON_UPDATE_COMMAND_UI(ID_MAP_EDITLINES, OnUpdateMapEditlines)
	ON_COMMAND(ID_MAP_EDITPOINTS, OnMapEditpoints)
	ON_UPDATE_COMMAND_UI(ID_MAP_EDITPOINTS, OnUpdateMapEditpoints)
	ON_COMMAND(ID_MAP_PAN, OnMapPan)
	ON_UPDATE_COMMAND_UI(ID_MAP_PAN, OnUpdateMapPan)
	ON_COMMAND(ID_MAP_MEASURE, OnMapMeasure)
	ON_UPDATE_COMMAND_UI(ID_MAP_MEASURE, OnUpdateMapMeasure)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_MAP_UPDATE, OnMapUpdate)
	ON_UPDATE_COMMAND_UI(ID_MAP_UPDATE, OnUpdateMapUpdate)
	ON_WM_MOUSEWHEEL()	
	ON_COMMAND(ID_MAP_VIEWFILE, OnMapViewfile)
	ON_UPDATE_COMMAND_UI(ID_MAP_VIEWFILE, OnUpdateMapViewfile)
	ON_COMMAND(ID_MAP_VIEWATTR, OnMapViewAttr)
   ON_WM_LBUTTONUP()   		
   ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)	
	ON_UPDATE_COMMAND_UI(ID_MAP_VIEWATTR, OnUpdateMapViewAttr)
	//}}AFX_MSG_MAP
   ON_COMMAND(ID_FILE_PRINT_DIRECT, CBDView::OnFilePrint)
	// Standard printing commands
   
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CViewMap construction/destruction

CViewMap::CViewMap()
{   
   m_dZoom = 1;
   m_dZoomP = 1;   

   m_bDefaultExtentNull = FALSE;
   
   // Must be called after m_bDefaultExtent is initialised
   InitialiseMapView();

   m_pRectTracker = NULL;
   m_pEditMapObj = NULL;
   m_pEditMapLayer = NULL;

   m_menuedit.LoadMenu(IDR_MAPEDIT);   

   m_nCurPage = 0;

   m_hDragCursor = LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CURDRAG));
   m_hDragCursor2 = LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CURDRAG2));
   m_hPanCursor = LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CURPAN));
   m_hCurSel = LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CURSEL));
   m_hCurIns = LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CURINS));

   m_bDragMode = FALSE;    
   m_bInsertMode = TRUE;
   m_nMapMode = none;
   m_ptPan = CPoint(-1,-1);

   m_pMapLayerObj = NULL;
   m_pMapLayer = NULL;   

   // Determine operating system

   DWORD dw = GetVersion();
   m_bWinNT = dw < 0x80000000;

   m_bWideLine = TRUE;

   m_pDoc = NULL;

   m_pMapLinesEdit = NULL;
   m_pMapCoordEdit = NULL;

   m_nMode = none;

   m_iEditPoint1 = 0;
   m_iEditPoint2 = 0;

   m_bAutoUpdate = TRUE;

   m_bViewFile = FALSE;
   m_bViewHotLink = FALSE;

   m_bLocator = FALSE;
   
}

///////////////////////////////////////////////////////////////////////////////

CViewMap::~CViewMap()
{  
   if (m_hDragCursor != NULL) DestroyCursor(m_hDragCursor);
   if (m_hDragCursor2 != NULL) DestroyCursor(m_hDragCursor2);
   if (m_hCurSel != NULL) DestroyCursor(m_hCurSel);
   if (m_hCurIns != NULL) DestroyCursor(m_hCurIns);
   if (m_hPanCursor != NULL) DestroyCursor(m_hPanCursor);
   RemoveTracker();
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::InitialiseMapView()
{
   // Determine the default projection

   CBDProjection projection;

   BOOL bFound = BDProjection(BDHandle(), &projection, BDGETINIT);
   while (bFound)
   {   
      if (projection.m_bDefault) 
      {         
         m_dMinX = projection.m_dMinEasting;
         m_dMaxX = projection.m_dMaxEasting;
         m_dMinY = projection.m_dMinNorthing;
         m_dMaxY = projection.m_dMaxNorthing;
         

         if (m_dMinX == NULL_DOUBLE || m_dMinY == NULL_DOUBLE || 
             m_dMaxX == NULL_DOUBLE || m_dMaxY == NULL_DOUBLE) 
         {
            m_bDefaultExtentNull = TRUE;
         };

         if (m_dMinX == m_dMaxX) {m_dMinX = 0; m_dMaxX = 1;}
         if (m_dMinY == m_dMaxY) {m_dMinY = 0; m_dMaxY = 1;}

		   m_dOffX = (m_dMinX + m_dMaxX)/2;
         m_dOffY = (m_dMinY + m_dMaxY)/2;

         m_dOffXP = m_dOffX;
         m_dOffYP = m_dOffY;

         break;
      };
      bFound = BDGetNext(BDHandle());
   }

   BDEnd(BDHandle());         

   // If not found then use latitude/longitude as extent
   if (!bFound)
   {
        m_dMinX = -180 * LATLONSCALE;
        m_dMaxX = 180 * LATLONSCALE;
        m_dMinY = -90 * LATLONSCALE;
        m_dMaxY = 90 * LATLONSCALE;

        m_dOffX = (m_dMinX + m_dMaxX)/2;
        m_dOffY = (m_dMinY + m_dMaxY)/2;

        m_dOffXP = m_dOffX;
        m_dOffYP = m_dOffY;    
   }

   // Reset logo text

   m_sLogoText = "";
   m_sLogoFile = "";
}

///////////////////////////////////////////////////////////////////////////////

BOOL CViewMap::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CBDView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////

void CViewMap::OnInitialUpdate() 
{
	CBDView::OnInitialUpdate();

   ShowScrollBar(SB_BOTH);
   SetScrollRange(SB_VERT,0,SCROLLRANGE);
   SetScrollRange(SB_HORZ,0,SCROLLRANGE);
   SetScrollPos();
	
	m_pDoc = GetDocument();	
}

/////////////////////////////////////////////////////////////////////////////
//
// Draw into a specified device context
//

void CViewMap::DrawRect(CDC* pDC, CRect rect, BOOL bLocator)
{
   double dZoom, dOffX, dOffY;      
   CRect rectD, rectL;
      
   // If full extent is set then the map is temporary set to the default extent

   if (bLocator)
   {
      // Determine coordinates of visible extent

      rectL.left = GetfXInv(m_rect.left);
      rectL.right = GetfXInv(m_rect.right);
      rectL.top = GetfYInv(m_rect.top);
      rectL.bottom = GetfYInv(m_rect.bottom);

      // Make a copy of the current zoom and pan level
      dZoom = m_dZoom;
      dOffX = m_dOffX;
      dOffY = m_dOffY;   
      m_bLocator = TRUE;
      OnViewZoomnormal();                        
   };

   m_rect = rect;
   DetermineAspect();  
   
   m_rectPaint.left = m_rectPaint.right = 0;
   OnDraw(pDC);

   // Tidy up

   if (bLocator)
   {
      // Draw a locator box
       
      rectD.left = GetfXPos(rectL.left);
      rectD.right = GetfXPos(rectL.right);
      rectD.top = GetfYPos(rectL.top);
      rectD.bottom = GetfYPos(rectL.bottom);

      // Clip to rectangle to the view

      CRect rectC = GetScreenRect(pDC);

      CRgn rgn;
      rgn.CreateRectRgn(rectC.left, rectC.top, rectC.right, rectC.bottom);
      pDC->SelectClipRgn(&rgn);

      DrawRect(pDC, &rectD, RGB(255,0,0));

      pDC->SelectClipRgn(NULL);

      // Reset zoom and pan levels

      m_dZoom = dZoom;
      m_dOffX = dOffX;
      m_dOffY = dOffY;
      DetermineAspect();
      m_pDoc->ResetScale();
      m_bLocator = FALSE;
   }

   // Tidy up

   GetClientRect(&m_rect);   
   DetermineAspect();           
}

/////////////////////////////////////////////////////////////////////////////

void CViewMap::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	OnPrepareDC(&dc);

	m_rectPaint = dc.m_ps.rcPaint;
   
   OnDraw(&dc);
   
   if (m_bAutoUpdate == -1) m_bAutoUpdate = 0;      
   
}


/////////////////////////////////////////////////////////////////////////////

void CViewMap::OnDraw(CDC* pDC)
{   
   CRect rect;         

   if (m_pDoc == NULL || !m_pDoc->IsKindOf(RUNTIME_CLASS(CDocMap))) return;
         
   // Check for wideline capabilities

   m_bWideLine = pDC->GetDeviceCaps(LINECAPS) & LC_WIDE;

   // If a scale has been set then set the zoom to reflect this

   if (m_pDoc->GetScale() != 0)
   {
      int nScale = GetScale(pDC);
      m_dZoom = (m_dZoom * nScale) / m_pDoc->GetScale();
   };

   BeginWaitCursor();   

   // If copying then change aspect ratio

   if (m_bCopy)
   {
      SetCopyRect(pDC);
   }
   
   // Fill the background color   

   if (!m_bLocator)
   {
      GetClientRect(&rect);
      pDC->FillSolidRect(&rect, pDC->GetBkColor());   
   };
   
   // Draw the layers

   if (m_pDoc->GetLayers()->GetSize() == 0)
   {
      if (!m_bLocator) DrawLogo(pDC);
   } else
   {  
      DrawLayers(pDC);      
   };   

   // Draw lines being edited

   if (m_pMapLinesEdit != NULL && !m_bLocator)
   {
     DrawMapEditLines(pDC, m_pMapLinesEdit, m_pEditMapLayer);
   };

   // Draw tape measure

   if (m_nMapMode == measure && !m_bLocator)
   {
      m_ptMeasure1 = CPoint(-1,-1);
      m_ptMeasure2 = CPoint(-1,-1);
      DrawTapeMeasure(pDC);
   }

   // Tidy Up

   if (m_bCopy)
   {
      GetClientRect(&m_rect);   
      DetermineAspect();
   };   

   EndWaitCursor();   

   // If requested to add a new feature then do so
   if (m_nMode & addnew)
   {      
      OnMapAddFeature();

      // Remove add new flag
      m_nMode = (m_nMode | addnew) ^ addnew;
   };

   // Reset

   m_rectPaint = CRect(0,0,0,0);
}

/////////////////////////////////////////////////////////////////////////////

void CViewMap::SetCopyRect(CDC* pDC)
{
   CSize sz = pDC->GetTextExtent("ABC");      
   m_rect.top = sz.cy*2;
   m_rect.bottom = min((m_rect.bottom * 9) / 10, m_rect.bottom - sz.cy *3);
   DetermineAspect(pDC);   
};

/////////////////////////////////////////////////////////////////////////////
// CViewMap printing

BOOL CViewMap::OnPreparePrinting(CPrintInfo* pInfo)
{
   pInfo->SetMaxPage(1);

	// default preparation
	return DoPreparePrinting(pInfo);   
}

void CViewMap::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* pInfo)
{
	pInfo->m_nNumPreviewPages = 1;
}

void CViewMap::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
   m_nCurPage = 0;
	GetClientRect(&m_rect);   

   DetermineAspect();
}

/////////////////////////////////////////////////////////////////////////////

void CViewMap::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{  
   m_rectP = pInfo->m_rectDraw;
   
   InitPrint(pDC);
 
   m_nCurPage = pInfo->m_nCurPage;   
	
	CBDView::OnPrint(pDC, pInfo);
}

/////////////////////////////////////////////////////////////////////////////

void CViewMap::InitPrint(CDC* pDC)
{
   LOGFONT lf;
   CFont font, fontT;
     
   // Retrieve the default layout

   m_pDoc->GetLayers()->SetMapLayout(BDGetApp()->GetLayout().GetDefault());   

   // Determine the offset so that the left and right hand margins are the
   // same size
   
   m_nWidth = pDC->GetDeviceCaps(PHYSICALWIDTH);
   m_nHeight = pDC->GetDeviceCaps(PHYSICALHEIGHT);

   // Layout mode
   if (m_nWidth == 0 && m_nHeight == 0)
   {
      m_nWidth = m_rectP.Width();
      m_nHeight = m_rectP.Height();
   }

   // Determine font height.  This is performed after the width and
   // height are calculated as these parameters are used for layout
   // and saving the image

   memset(&lf,0,sizeof(LOGFONT));
   lf.lfHeight = -8;
   lf.lfPitchAndFamily = 12;   
   lf.lfCharSet = NRDB_CHARSET;
   strcpy(lf.lfFaceName, BDString(IDS_DEFAULTFONT));      
   ScaleFont(pDC, &lf);         
   font.CreateFontIndirect(&lf);  
   CFont* pFontOld = pDC->SelectObject(&font);           
   CSize sz = pDC->GetTextExtent("ABC");
   pDC->SelectObject(pFontOld);

   int nOffSetXL = pDC->GetDeviceCaps(PHYSICALOFFSETX);
   int nOffSetXR = m_nWidth - m_rectP.Width() - nOffSetXL;
   int nOffSetX = max(nOffSetXL, nOffSetXR);
   m_rectP.left = m_rectP.left + nOffSetX - nOffSetXL;  
   
   // If the layout position is defined then use this
     
   // Determine the rectangle into which to draw the map

   if (BDGetApp()->GetLayout().IsAuto())
   {
      if (m_nHeight > m_nWidth) // Portrait
      {
         m_rect = m_rectP; 
         m_rect.left += sz.cy*2;
         m_rect.right = m_rect.right - sz.cy*2;
         m_rect.top += sz.cy*2; 
         m_rect.bottom = m_rect.top + (m_rectP.Height()*6)/10;
      }
      else
      {
	      m_rect = m_rectP; 
         m_rect.left += sz.cy*2;
         m_rect.right = (m_rect.right * 8)/10;   
         m_rect.top += sz.cy*4;   
         m_rect.bottom -= sz.cy*5;
      };

      // Adjust to fit scale on legend

      if (m_pDoc->GetLayers()->GetMapGrid().m_nType != CMapGrid::none && 
          m_pDoc->GetLayers()->GetMapGrid().m_nType != CMapGrid::defaultgrid)
      {
         m_rect.left += sz.cx;
         m_rect.right -= sz.cx;
      }
   }

   else
   {

      // Determine map position from active layout
   
      CMapLayoutObj layoutobj = m_pDoc->GetLayers()->GetMapLayout().GetLayoutObj(CMapLayout::map);
      CRect rectMap = layoutobj.m_rect;      
      if (rectMap.Width() > 0)
      {     
         m_rect = CMapLayout::RectFromPercent(m_rectP, rectMap);     
      }    
   };

   // Adjust the aspect ratio 

   DetermineAspect(pDC);      

}

/////////////////////////////////////////////////////////////////////////////
// CViewMap diagnostics

#ifdef _DEBUG
void CViewMap::AssertValid() const
{
	CBDView::AssertValid();
}

void CViewMap::Dump(CDumpContext& dc) const
{
	CBDView::Dump(dc);
}

CDocMap* CViewMap::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDocMap)));   
	return (CDocMap*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CViewMap message handlers

void CViewMap::DrawLogo(CDC* pDC)
{
    CRect rect;
    CDocMap* pDoc = GetDocument();
	 ASSERT_VALID(pDoc);    	 

    // Retrieve logo info

    if (m_sLogoText == "")
    {
      CBDMain main;
      BDMain(BDHandle(), &main, BDGETINIT);
      BDEnd(BDHandle());

      m_sLogoText = main.m_Organization;

      m_sLogoFile = main.m_Logo;
      if (m_sLogoFile.Find('\\') == -1 && m_sLogoFile != "") m_sLogoFile = BDGetAppPath() + m_sLogoFile;

      m_imagefile.Close();
    };

    // Load the logo file file

    if (!m_imagefile.IsOpen() && m_sLogoFile != "")
    {       
       //m_imagefile.m_nWaterColor = 4;
       if (!m_imagefile.Open(m_sLogoFile, 0))
       {
          m_sLogoFile = "";
       }              
    };
    
    // Display logo

    if (m_sLogoFile != "")
    {
       m_imagefile.OnDraw(pDC, m_rect); 
    }

    // If no logo then display default

    else
    {
       CBitmap bitmap, *pBitMapOld;
       bitmap.LoadBitmap(IDB_LOGO2);

       CDC dc;
       dc.CreateCompatibleDC(pDC);
       pBitMapOld = dc.SelectObject(&bitmap);    
     
       CRect rectS(0,0,299,284);              
	    
       CRect rectC;
       rectC = m_rect;              
       int nWidth = rectC.Width();
       int nHeight = rectC.Height();
	   
	   // Adjust aspect ratio

	   if (rectC.bottom == 0 || rectC.right == 0) return;

	   if ((double)rectS.bottom / nHeight > (double)rectS.right / nWidth)
	   {  
         nWidth = (nHeight * rectS.bottom) / rectS.right;             
	   } else
	   {		   
         nHeight = (nHeight * rectS.right) / rectS.bottom;                  
	   }

      //  Centre to page
      
      rectC.left = m_rect.left + (m_rect.Width() - nWidth) / 2;
      rectC.right = rectC.left + nWidth;
      rectC.top = m_rect.top + (m_rect.Height() - nHeight) /2;      
      rectC.bottom = rectC.top + nHeight;
    
	   // Copy, nb stretchblt handles changes of palette

	   pDC->StretchBlt(rectC.left,rectC.top, rectC.Width(), rectC.Height(), &dc, 0,0, rectS.Width(), 
		               rectS.Height(), SRCCOPY);
      dc.SelectObject(pBitMapOld);    

    };

   // Create Font	
           
   LOGFONT lf;
   memset(&lf,0,sizeof(lf));


   // Adjust size of font to window area

   lf.lfHeight = max(min(40, 40 * m_rect.Height() * m_rect.Width() / 75000), 6);

   lf.lfPitchAndFamily = 18;   
   lf.lfCharSet = NRDB_CHARSET;   

   strcpy(lf.lfFaceName, BDString(IDS_DEFAULTFONT));      
   CFont font;
   font.CreateFontIndirect(&lf);
     
   // Draw Text

   rect = m_rect;
   CFont* pFontOld = pDC->SelectObject(&font);
   pDC->SetBkMode(TRANSPARENT);          
   pDC->SetTextColor(RGB(192,192,192));
   pDC->DrawText(m_sLogoText, &rect, DT_RIGHT|DT_WORDBREAK); 
   rect.left-=1;
   rect.right-=1;
   pDC->SetTextColor(RGB(0,0,192));
   pDC->DrawText(m_sLogoText, &rect, DT_RIGHT|DT_WORDBREAK); 
   pDC->SelectObject(pFontOld);
   
}

/////////////////////////////////////////////////////////////////////////////

void CViewMap::DrawLayers(CDC* pDC)
{ 
   CFont *pFontOld = NULL;
      
   // Select clip region, allowing for different device context in
   // print preview mode
   
   CRect rectC = GetScreenRect(pDC);

   CRgn rgn;
   rgn.CreateRectRgn(rectC.left, rectC.top, rectC.right, rectC.bottom);
   pDC->SelectClipRgn(&rgn);

   // Draw each layer   
   int i = 0;

   for (i = m_pDoc->GetLayers()->GetSize()-1; i >= 0; i--)
   {
      CMapLayer* pMapLayer = m_pDoc->GetLayers()->GetAt(i);

      if (pMapLayer->IsVisible())
      {                  
         // Display objects within the layer

         int j = 0; for (j = 0; j < pMapLayer->GetSize(); j++)
         {                     
            CMapLayerObj* pMapLayerObj = pMapLayer->GetAt(j);
            ASSERT(pMapLayerObj->GetMapObject() != NULL);

            if (m_bAutoUpdate || pDC->IsPrinting() || m_bCopy)
            {
		    // If default layer then check for search match

               if (pMapLayerObj != m_pEditMapObj) // Don't draw obj being edited
               {                    
                  if (pMapLayerObj->GetDataType() == BDMAPLINES)
                  {                  
                     DrawMapLines(pDC, pMapLayerObj, pMapLayer);
                  }
	               else if (pMapLayerObj->GetDataType() == BDCOORD && !m_bLocator)
                  {
		               DrawCoords(pDC, pMapLayerObj, pMapLayer);
                  }
                  else if (pMapLayerObj->GetDataType() == BDIMAGE && !m_bLocator)
                  {                     
                     DrawImage(pDC, pMapLayerObj, pMapLayer);
                  }
               };
            } 
            
            // If autodraw if off then draw extent

            else
            {
               DrawWireFrame(pDC, pMapLayer, pMapLayerObj);               
            }
         };
      };      	  	  
   }
   m_aRectSym.RemoveAll();   

   // Draw the text in the reverse order so that the last layers have
   // precedence
   
   for (i = 0; i < m_pDoc->GetLayers()->GetSize() && !m_bLocator; i++)
   {
      CMapLayer* pMapLayer = m_pDoc->GetLayers()->GetAt(i);

      if (pMapLayer->IsVisible())
      {      
         // Create font
      
         CFont font;
         LOGFONT lf = pMapLayer->GetFont();
         ScaleFont(pDC, &lf);      
         if (pMapLayer->GetScaleFont())
         {
            lf.lfHeight = (int)(lf.lfHeight * m_dZoom);
            lf.lfWidth = (int)(lf.lfWidth * m_dZoom);
         }      
         font.CreateFontIndirect(&lf);            
         pFontOld = pDC->SelectObject(&font);        
      
         // Draw the text associated with map layers

         DrawLayerText(pDC, pMapLayer);

         // Tidy up

         pDC->SelectObject(pFontOld);              
      };      
   }
   m_aRectText.RemoveAll();
   
   // Clear clip region

   pDC->SelectClipRgn(NULL);

   // Draw bounding coordinates      

   if (!m_bLocator)
   {
      DrawCoords(pDC);                
   };

   // Draw bounding rectangle on print

   if (m_pDoc->GetLayers()->GetMapGrid().m_nType != CMapGrid::none)
   {
      CRect rect = m_rect;
      rect.right--;
      rect.bottom--;

      CMapLayoutObj layoutobj = m_pDoc->GetLayers()->GetMapLayout().GetLayoutObj(CMapLayout::map);
      DrawRect(pDC, &rect, layoutobj.m_style.m_crLine, layoutobj.m_style.m_nLineStyle, layoutobj.m_style.m_nLineWidth);
   };

   // Draw user defined borders

   if (pDC->IsPrinting() && !m_bLocator)
   {  
      DrawLayoutLocator(pDC);
	   DrawLayoutBorders(pDC);
      DrawLayoutImages(pDC);
      DrawLayoutText(pDC);      
      
   };

   // Draw print specific outputs

   if (pDC->IsPrinting() && !m_bLocator)

   {
      // Draw bounding rectangle
      
      if (BDGetApp()->GetLayout().IsAuto())
      {
         CRect rect = m_rectP;
         rect.right--;
         rect.bottom--;
         DrawRect(pDC, &rect, RGB(0,0,64), PS_SOLID, 1);           
      };      
      
      // Draw key
      // Set extent for calculating font size for images

      m_pDoc->GetViewLegend()->SetExtent(m_nWidth, m_nHeight);
      m_pDoc->GetViewLegend()->DrawLegend(pDC);      
   };

        

   // Draw the scale bar on copy

   if (m_bCopy)
   {  
      GetClientRect(&m_rectP);
      DrawCoords(pDC);                    
   }
}

/////////////////////////////////////////////////////////////////////////////

void CViewMap::DrawMapLines(CDC* pDC, CMapLayerObj* pMapLayerObj, CMapLayer* pMapLayer, BOOL bSearch)
{   
   CPointArray aPoints;   
   CPointArray aPolyPoints;
   CArray <INT,INT> aPolyCounts;
   POINT point;
   BOOL bNewLine = FALSE;
   BOOL bOut;
   int nClipped = 0;
   
   CMapStyle mapstyle;   
   
   GetStyle(pMapLayer, pMapLayerObj, 0, mapstyle);

   // Determine the extent

   GetExtent(pMapLayer, pMapLayerObj);
   
   // Optimization: If the extent is outside the current view then no need to draw

   if (!IsVisible(pDC, pMapLayerObj)) return;   

   // Create the pen
   
   if (bSearch)
   {
	   mapstyle.m_crFill = RGB(255,255,255);
	   mapstyle.m_crLine = RGB(255,0,0);
   }


   CPen pen(mapstyle.m_nLineStyle, GetLineWidth(pDC,mapstyle.m_nLineStyle, mapstyle.m_nLineWidth), mapstyle.m_crLine);
   CPen* pPenOld = pDC->SelectObject(&pen);

   // Create brush

   CBrush brush;
   CComboBoxPattern::CreateBrush(brush, mapstyle.m_nPattern,  mapstyle.m_nHatch, mapstyle.m_crFill);   
   CBrush* pBrushOld = pDC->SelectObject(&brush);

   // Set the background mode to transparent

   pDC->SetBkMode(TRANSPARENT);

   CLongLines* pMapLines = (CLongLines*)pMapLayerObj->GetMapObject();   
  
    // Plot the coordinates
   
   int i = 0; for (i = 0; i < pMapLines->GetSize(); i++)
   {                

      CLongCoord coord = pMapLines->GetAt(i);
      if (!coord.IsNull())
      {           
         point.x = (int)GetfXPos(coord.x);
         point.y = (int)GetfYPos(coord.y);
         
		 if (aPoints.GetSize() == 0 || CPoint(aPoints[aPoints.GetSize()-1]) != CPoint(point)) 
		 {
			 aPoints.Add(point);		  	       
		 
			 // Clip polylgons

			 if (!m_bWinNT && pMapLayer->IsPolygon())
			 {
				aPoints.RemoveAll();
				ClipPolygon(i, pMapLines, aPoints);
				bNewLine = TRUE;
			 }				 

			 // Clip polylines

			 if (!m_bWinNT && !pMapLayer->IsPolygon() && aPoints.GetSize() > 1)
			 {
				int n = aPoints.GetSize();
				ClipToRect(m_rect,aPoints[n-2].x, aPoints[n-1].x, aPoints[n-2].y, aPoints[n-1].y, 
					 nClipped, bOut);

				if (!bOut)
				{
				   aPoints.RemoveAll();			  
				   aPoints.Add(point);                 
				};
				if (nClipped & ClippedTo)
				{
				   bNewLine = TRUE;
				}
			 }         		 
		 }
      } 		  
   
      if (bNewLine || coord.IsNull() || i+1 == pMapLines->GetSize())
      {         		  
         // Draw polylines
               
         if (aPoints.GetSize() > 1) 
         {
			 // If polygon is too long for win98 then draw it as a polyline

            if (pMapLayer->IsPolygon())
            {    
				// Support for multi-part polygons				

	            aPolyPoints.Append(aPoints);
				   aPolyCounts.Add(aPoints.GetSize());

            } else
            {  
                DrawLines(pDC, aPoints.GetData(), aPoints.GetSize());							
            };
         };

         // Tidy up

         aPoints.RemoveAll();       
         if (!coord.IsNull()) aPoints.Add(point);  

         bNewLine = FALSE;		 
      }             
   };

   // Plot the multipart polygons

   if (pMapLayer->IsPolygon() && aPolyPoints.GetSize() > 0)
   {
	   BOOL bOK = TRUE;

	   if (aPolyCounts.GetSize() > 1)
	   {
		   bOK = pDC->PolyPolygon(aPolyPoints.GetData(), aPolyCounts.GetData(), aPolyCounts.GetSize());
	   } else
	   {
		   bOK = pDC->Polygon(aPolyPoints.GetData(), aPolyPoints.GetSize());
	   }

	   // If drawing a polygon fails draw as a polyline

	   if (!bOK)
	   {	   
		   POINT* pPoints = aPolyPoints.GetData();
		   int i = 0; for (i = 0; i < aPolyCounts.GetSize(); i++)
		   {
              DrawLines(pDC, pPoints, aPolyCounts[i]);
			     pPoints += aPolyCounts[i];
		   };
	   }
   }
     
   // Tidy up

   pDC->SetBkMode(OPAQUE);

   pDC->SelectObject(pPenOld);
   pDC->SelectObject(pBrushOld);

}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::DrawLines(CDC* pDC, POINT* pPoints, int nPoints)
{
// For Win95/98/ME must split into parts.  For device contexts not supporting wide lines, 
// the parts must be even smaller

   if (!m_bWinNT)
   {
	  int nSizeT = nPoints;
	  int nSize = min(nSizeT, m_bWideLine ? WIDELINELEN : WIN98POLYLINE);	  
      while (nSize > 0)
	  {
          pDC->Polyline(pPoints, nSize);
		  pPoints += nSize;
		  nSizeT -= nSize;
		  nSize = min(nSizeT, m_bWideLine ? WIDELINELEN : WIN98POLYLINE);
	  }
   } else
   {
      pDC->Polyline(pPoints, nPoints);                               
   };
}

///////////////////////////////////////////////////////////////////////////////
//
// Draw lines whilst editing
//

void CViewMap::DrawMapEditLines(CDC* pDC, CLongLines* pMapLines, CMapLayer* pMapLayer)
{
   CPoint point, point1;

   // First create the current pen  

   CMapStyle mapstyle;
   GetStyle(pMapLayer, m_pEditMapObj, 0, mapstyle);
      
   COLORREF crPen1 = mapstyle.m_crLine;
   COLORREF crPen2 = RGB(255-GetRValue(crPen1),255-GetGValue(crPen1),255-GetBValue(crPen1));
   if (crPen2 == RGB(255,255,255)) crPen2 = RGB(255,0,0);

   CPen pen1(pMapLayer->GetLineStyle(), GetLineWidth(pDC,pMapLayer), crPen1);
   CPen pen2(pMapLayer->GetLineStyle(), GetLineWidth(pDC,pMapLayer), crPen2);
   CPen* pPenOld = pDC->SelectObject(&pen1);

   // Now draw the lines (don't worry about clipping?)

   int i = 0; for (i = 0; i < pMapLines->GetSize(); i++)
   {
      if (!pMapLines->GetAt(i).IsNull())
      {
         CLongCoord coord = pMapLines->GetAt(i);

         point.x = (int)GetfXPos(coord.x);
         point.y = (int)GetfYPos(coord.y);
         
         // Draw a cross at each point

         DrawNode(pDC, point);

         // Draw a line from the last point

         if (i > 0 && !pMapLines->GetAt(i-1).IsNull())
         {
            pDC->MoveTo(point1.x, point1.y);
            pDC->LineTo(point.x, point.y);
         }
         point1 = point;

      };
   }

   // Reset pen

   pDC->SelectObject(pPenOld);
   pPenOld = pDC->SelectObject(&pen2);

   // Draw the selected points in different colour at end so not overpainted
   // for polygons

   if (pMapLines->GetSize() > 0)
   {
      if (!pMapLines->GetAt(m_iEditPoint1).IsNull())
      {
         CLongCoord coord = pMapLines->GetAt(m_iEditPoint1);
         point.x = (int)GetfXPos(coord.x);
         point.y = (int)GetfYPos(coord.y);
         DrawNode(pDC, point);
      
         if (pMapLines->GetSize() > m_iEditPoint2 && 
            !pMapLines->GetAt(m_iEditPoint2).IsNull())
         {
            CLongCoord coord = pMapLines->GetAt(m_iEditPoint2);
            point1.x = (int)GetfXPos(coord.x);
            point1.y = (int)GetfYPos(coord.y);            

            pDC->MoveTo(point1.x, point1.y);
            pDC->LineTo(point.x, point.y);
         };
      };
   };
         
   // Tidy up

   pDC->SelectObject(pPenOld);
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::DrawWireFrame(CDC *pDC, CMapLayer *pMapLayer, CMapLayerObj *pMapLayerObj)
{
   GetExtent(pMapLayer, pMapLayerObj);
   CRect rectE = pMapLayerObj->GetExtent();

   CRect rect;
   rect.left = GetfXPos(rectE.left);
   rect.top = GetfYPos(rectE.top);
   rect.right = GetfXPos(rectE.right);
   rect.bottom = GetfYPos(rectE.bottom);

   DrawRect(pDC, &rect, pMapLayer->GetColourLine(), PS_DOT, 1);   
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   BOOL bAdjust = TRUE;

	// Handle keys whilst editing

   if (m_pMapLinesEdit != NULL)
   {
      if (nChar == VK_LEFT)
      {
         if (m_iEditPoint1-1 >= 0 && !m_pMapLinesEdit->GetAt(m_iEditPoint1-1).IsNull())
         {
            m_iEditPoint1--;         
            if ((m_iEditPoint2-1 > m_iEditPoint1 || m_iEditPoint1 == 0) && 
              m_iEditPoint2-1 >= 0) m_iEditPoint2--;
         }
         // If at start of polygon then move to end

         else  if (m_nMode & polygon) 
         {
            while (m_iEditPoint1+2 < m_pMapLinesEdit->GetSize() && !m_pMapLinesEdit->GetAt(m_iEditPoint1+2).IsNull()) m_iEditPoint1++;
            if (!m_pMapLinesEdit->GetAt(m_iEditPoint1+1).IsNull()) m_iEditPoint2 = m_iEditPoint1+1;
            else m_iEditPoint2 = m_iEditPoint1;
         }         
      }
      if (nChar == VK_RIGHT)
      {         
         if (m_iEditPoint2+1 < m_pMapLinesEdit->GetSize() && !m_pMapLinesEdit->GetAt(m_iEditPoint2+1).IsNull()) 
         {   m_iEditPoint2++;         
             if (!(m_iEditPoint1 == 0 && m_iEditPoint2 == 1) &&
             m_iEditPoint1+1 < m_pMapLinesEdit->GetSize() && !m_pMapLinesEdit->GetAt(m_iEditPoint1+1).IsNull()) m_iEditPoint1++;         
         }
         // If reached end of polygon then move to start
         else  if (m_nMode & polygon) 
         {
            while (m_iEditPoint1-1 > 0 && !m_pMapLinesEdit->GetAt(m_iEditPoint1-1).IsNull()) m_iEditPoint1--;
            if (!m_pMapLinesEdit->GetAt(m_iEditPoint1+1).IsNull()) m_iEditPoint2 = m_iEditPoint1+1;
            else m_iEditPoint2 = m_iEditPoint1;
         }

      }     
      else if (nChar == VK_DELETE)
      {         
         EditPolylines(CLongLines(), FALSE);
      }
      else if (nChar == VK_INSERT)
      {  
         if (m_pMapLinesEdit->GetSize() > 0)
         {
            m_iEditPoint1 = m_pMapLinesEdit->GetSize()-1;

            if (!m_pMapLinesEdit->GetAt(m_iEditPoint1).IsNull())
            {
               CLongCoord coord;
               coord.SetNull();
               m_pMapLinesEdit->InsertAt(m_iEditPoint1, coord);
               m_iEditPoint1++;
            }
            m_iEditPoint2 = m_iEditPoint1;

            bAdjust = FALSE;
         };
      }

   // For polygons, ensure that m_iEditPoint1 <> m_iEditPoint2 i.e. cannot add
   // a point to the end of the polygon

      if (bAdjust) AdjustEditParams();

   // Update view

	  CDC *pDC = GetDC();
	  if (pDC != NULL)
	  {
         DrawMapEditLines(pDC, m_pMapLinesEdit, m_pEditMapLayer);
	     ReleaseDC(pDC);
	  }
   };
	
	CBDView::OnKeyDown(nChar, nRepCnt, nFlags);
}

///////////////////////////////////////////////////////////////////////////////
//
// Determines if the current maplines need to be drawn in the current view
//

BOOL CViewMap::IsVisible(CDC* pDC, CMapLayerObj *pMapLayerObj, BOOL bText)
{
   CRect rect, rectI, rectV;

   CRect& rectE = pMapLayerObj->GetExtent();

   if (rectE.left == 0 && rectE.right == 0) return FALSE;

   rect.left = (int)GetfXPos(rectE.left);
   rect.top = (int)GetfYPos(rectE.top);
   rect.right = (int)GetfXPos(rectE.right);
   rect.bottom = (int)GetfYPos(rectE.bottom);

   if (rect.top > rect.bottom) swap(rect.top, rect.bottom);

   if (rect.left == rect.right) rect.right++;
   if (rect.top == rect.bottom) rect.bottom++;

   rectV = m_rect;

   // Text not included in extent so isn't redrawn if use m_rectPaint

   if (!bText && m_rectPaint.left != m_rectPaint.right && !pDC->IsPrinting()) rectV = m_rectPaint;
   
   return rectI.IntersectRect(rect, rectV); 
}

/////////////////////////////////////////////////////////////////////////////
//
// Clip the polygon to the display rectangle
//
//

void CViewMap::ClipPolygon(int& iPos, CLongLines* pMapLines, CPointArray& aPoints)
{	
    CPoint point1, point2, point;
	BOOL bOut;
	int nClipped;	  
	CArray <POINT, POINT> aClipPoints;
          	
	// Copy data to aPoints

    while (iPos < pMapLines->GetSize() && !pMapLines->GetAt(iPos).IsNull())
    {
       CLongCoord coord = pMapLines->GetAt(iPos++);
       point.x = (int)GetfXPos(coord.x);
	   point.y = (int)GetfYPos(coord.y);

	   if (aPoints.GetSize() == 0 || CPoint(aPoints[aPoints.GetSize()-1]) != CPoint(point)) 
	   {
		   aPoints.Add(point);		  	   
	   };
    }

    // If line does not form a polygon then add first point to end
     
     if (CPoint(aPoints[0]) != CPoint(aPoints[aPoints.GetSize()-1]))
     {
        aPoints.Add(aPoints[0]);
     }
      
   // Optimization

   aClipPoints.SetSize(aPoints.GetSize());

   // Need to have two buffers as the clipped version may be larger than the
   // previous.  In order to be efficient, these are swapped at the end of 
   // each loop

   // Clip to each edge separately

   for (int j = 1; j <= 4; j++)   
   {	              
      int k = 0;      

	   int i = 0; for (i = 0; i < aPoints.GetSize(); i++)
	   {     
         point2 = aPoints[i];
         point = point2; // Unclipped

	      if (i > 0)
	      {
            // Clip to each vertex                        

            if (j == clipright) ClipToLine(point1.x, point1.y, point2.x, point2.y, m_rect.right, clipright, nClipped, bOut);
            else if (j == clipleft) ClipToLine(point1.x, point1.y, point2.x, point2.y, m_rect.left, clipleft, nClipped, bOut);
            else if (j == cliptop) ClipToLine(point1.y, point1.x, point2.y, point2.x, m_rect.top, cliptop, nClipped, bOut);
            else if (j == clipbottom) ClipToLine(point1.y, point1.x, point2.y, point2.x, m_rect.bottom, clipbottom, nClipped, bOut);
           
            // Add points to array

            if (bOut)
            {
               if (nClipped & ClippedFrom)
               {
                  aClipPoints.SetAtGrow(k++, point1);                                    
               }               
               aClipPoints.SetAtGrow(k++, point2);               
            }            
	      };	          
         point1 = point;
      };	  

	  if (k > 0)
	  {
         aClipPoints.SetAtGrow(k++, aClipPoints[0]);
	  };
                 
	   // Copy the array onto the next loop

	  aClipPoints.SetSize(k);
      aPoints.Copy(aClipPoints);
   }
}


/////////////////////////////////////////////////////////////////////////////
//
// Draws the text associated with map lines or coordinates
//

void CViewMap::DrawLayerText(CDC* pDC, CMapLayer* pMapLayer)
{
   BOOL bDrawText = TRUE;

   // Determine the color

   if (pMapLayer->GetTextPos() == CMapLayer::Null)
   {
      bDrawText = FALSE;
   }
   if (bDrawText)
   {
      int j = 0; for (j = 0; j < pMapLayer->GetSize(); j++)
      {
         CMapLayerObj* pMapLayerObj = pMapLayer->GetAt(j);

		 // Optimization, don't attempt to fit if no text

		 if (pMapLayerObj->GetText()[0] != '\0' && IsVisible(pDC, pMapLayerObj, TRUE))
		 {               
			  // Draw any associated text

          CLongLines* pMapLines = NULL;
          if (pMapLayerObj->GetDataType() == BDMAPLINES)
          {
             pMapLines = (CLongLines*)pMapLayerObj->GetMapObject();
          }
          
			 if (pMapLines == NULL || pMapLines->GetSize() > 0)
			 {               				
                 int x = GetfXPos(pMapLayerObj->GetCentreX());
                 int y = GetfYPos(pMapLayerObj->GetCentreY());
       
		  		 FitText(pDC, pMapLayerObj, pMapLayer, x, y);
				
			 };                  
		 };
      }
      DrawText(pDC, pMapLayer);
   };
}

/////////////////////////////////////////////////////////////////////////////

void CViewMap::GetExtent(CMapLayer* pMapLayer, CMapLayerObj* pMapLayerObj)
{  
   CRectEx rectE, rectPMax; 

   CRect& rRect = pMapLayerObj->GetExtent();

   // TODO need to optimise by determining centroid automatically as extent is loaded
   // from shapefiles for overlays and could be stored in CLongBinary

   if ((rRect.left == 0 && rRect.right == 0) ||
       (pMapLayerObj->GetCentreX() == 0 && pMapLayerObj->GetCentreY() == 0))
   {
      if (pMapLayerObj->GetDataType() == BDMAPLINES)
      {       
		  // Retrieve the total extent of the polygon

         CSpatialPoly* pPolygon = (CSpatialPoly*)pMapLayerObj->GetMapObject();
		 rectE = pPolygon->m_rect;

         // Retrieve the centroid
		 CLongCoord coord;
         pPolygon->GetCentroid(coord);  
		 rectPMax.left = rectPMax.right = coord.x;
		 rectPMax.top = rectPMax.bottom = coord.y;
         
      } 
      else if (pMapLayerObj->GetDataType() == BDCOORD)
      {
         CCoord* pCoord = (CCoord*)pMapLayerObj->GetMapObject();
                  
         double dX = GetfXInv((int)pMapLayer->GetSymSize()) - GetfXInv(0);

         rectE.left = (int)(pCoord->x - dX*2 + 0.5);
         rectE.right = (int)(pCoord->x + dX*2 + 0.5);
         rectE.top = (int)(pCoord->y + dX*2 + 0.5);
         rectE.bottom = (int)(pCoord->y - dX*2 + 0.5);

         rectPMax = rectE;
      }
      
      // Determine extent for images

      else if (pMapLayerObj->GetDataType() == BDIMAGE)
      {
         double dX, dY;
         CImageFile* pImageFile = (CImageFile*)pMapLayerObj->GetMapObject();

         pImageFile->m_georef.Convert(CPoint(0,0), dX, dY);
         rectE.left = (int)dX;
         rectE.top = (int)dY;

         pImageFile->m_georef.Convert(CPoint(pImageFile->m_buffer.width-1, pImageFile->m_buffer.height-1), dX, dY);
         rectE.right = (int)dX;
         rectE.bottom = (int)dY;         

         rectPMax = rectE;
      }

      // Store the result 

      if (!rectE.IsEmpty())
      {
         rRect = rectE; 

         // Store the centre of the largest polygon

         pMapLayerObj->GetCentreX() = (rectPMax.right + rectPMax.left)/2;
         pMapLayerObj->GetCentreY() = (rectPMax.bottom + rectPMax.top)/2;
      }   
   };

   
}

/////////////////////////////////////////////////////////////////////////////
//
// Determine if the map layer is visible within the current map extent.  If 
// not, offer to update the extent
//

void CViewMap::CheckExtent(CMapLayerArray* pMapLayerArray)
{
    CBDProjection projection;    

   // Determine the full extent of the view
   
    CRect rectNew = CRect(0,0,0,0);
    CRect rectAll = CRect(0,0,0,0);
       
    int j = 0; for (j = 0; j < pMapLayerArray->GetSize(); j++)
    {
       CMapLayer* pMapLayer = pMapLayerArray->GetAt(j);
             
       int i = 0; for (i = 0; i < pMapLayer->GetSize(); i++)
       {                       
          CMapLayerObj* pMapObj = pMapLayer->GetAt(i);

          // Determine extent of new layers

          if (!pMapLayer->IsExtentChecked())
          {
             if (rectNew.IsRectNull())
             {
                rectNew = pMapObj->GetExtent();
             } else
             {                    
                rectNew.top = min(rectNew.top, pMapObj->GetExtent().top);
                rectNew.bottom = max(rectNew.bottom, pMapObj->GetExtent().bottom); 
                rectNew.left = min(rectNew.left, pMapObj->GetExtent().left);
                rectNew.right = max(rectNew.right, pMapObj->GetExtent().right);
             }       
          } 

          // Determine total extent

           if (rectAll.IsRectNull())
           {
              rectAll = pMapObj->GetExtent();
           } else
           {                    
              rectAll.top = min(rectAll.top, pMapObj->GetExtent().top);
              rectAll.bottom = max(rectAll.bottom, pMapObj->GetExtent().bottom); 
              rectAll.left = min(rectAll.left, pMapObj->GetExtent().left);
              rectAll.right = max(rectAll.right, pMapObj->GetExtent().right);
           }       
       }
       pMapLayer->SetExtentChecked();
    };

   // If the view does not fit in the default view then offer to update it

    if (rectNew.left != rectNew.right)
    {
       if (m_bDefaultExtentNull || 
           ((rectNew.right < m_dMinX || rectNew.left > m_dMaxX) &&
           (rectNew.top < m_dMinY || rectNew.bottom > m_dMaxY)))
       {
          // Ask the user if the view should be updated

          if (AfxMessageBox(IDS_OUTSIDEEXTENT, MB_YESNO) == IDYES)
          {
             // Find the default projection

             BOOL bFound = BDProjection(BDHandle(), &projection, BDGETINIT);
             while (bFound)
             {   
                // Update it

               if (projection.m_bDefault) 
               {  
                  BDEnd(BDHandle());

                  projection.m_dMinEasting = rectAll.left;
                  projection.m_dMaxEasting = rectAll.right;
                  projection.m_dMinNorthing = rectAll.top;
                  projection.m_dMaxNorthing = rectAll.bottom;

                  BDProjection(BDHandle(), &projection, BDUPDATE);                                    

                  m_bDefaultExtentNull = FALSE;

               // Initialise and redraw the view

                  InitialiseMapView();                     
                  OnViewZoomnormal();

                  break;
               };
               bFound = BDGetNext(BDHandle());
            };
            BDEnd(BDHandle());                       

   // Initialise to latitude/longitude

           if (!bFound)
           {
               InitialiseMapView();                     
               OnViewZoomnormal();
           }
          };
       };
    };                   
}

/////////////////////////////////////////////////////////////////////////////

void CViewMap::DrawCoords(CDC* pDC, CMapLayerObj* pMapLayerObj, CMapLayer* pMapLayer, BOOL bSearch)
{   
   CBrush brush;   
   CRect rectWindow;         
   char* pName = NULL; 
   CRect rectI;                 
   
   // Optimization: If the extent is outside the current view then no need to draw, should
   // speed up drawing of custom symbols as well as prevent Win 98 from crashing when zoomed in 

   // Determine the extent

   GetExtent(pMapLayer, pMapLayerObj);

   if (!IsVisible(pDC, pMapLayerObj)) return;   
   
   CMapStyle mapstyle;
   GetStyle(pMapLayer, pMapLayerObj, 0, mapstyle);

   if (bSearch)
   {
	   mapstyle.m_crFill = RGB(255,255,255);
	   mapstyle.m_crLine = RGB(255,0,0);
   }
      
  // Convert the coordinate to screen coordinates     
 
   CCoord* pCoord = (CCoord*)pMapLayerObj->GetMapObject();

   if (pCoord != NULL && !pCoord->IsNull())
   {
	   double dX = GetfXPos((long)(pCoord->x + 0.5));
	   double dY = GetfYPos((long)(pCoord->y + 0.5));      

   // Determine the auto symbol size

      if (pMapLayer->GetAutoSize())
      {                
         ASSERT(pMapLayer->GetAutoMax() - pMapLayer->GetAutoMin() != 0);
         if (!IsNullDouble(pMapLayerObj->GetValue()))
         {            
            double d = (pMapLayerObj->GetValue() - pMapLayer->GetAutoMin()) / 
                       (pMapLayer->GetAutoMax() - pMapLayer->GetAutoMin());

            mapstyle.m_dSymSize *= AUTOSCALE*d;
         }       
      }

   // Scale the symbol

      if (pMapLayer->GetScaleFont())
      {
         mapstyle.m_dSymSize *= m_dZoom;
      }

   // Draw position of station      
      
      if (pMapLayer->GetBestFitSym())
      {
         CheckOverlapSym(pDC, mapstyle.m_dSymSize, dX, dY);
      }           
           
         CComboBoxSymbol::DrawSymbol(pDC, dX, dY, mapstyle);                 
   };
}

///////////////////////////////////////////////////////////////////////////////
//
// Display a georeferenced image associated with a map object
//

void CViewMap::DrawImage(CDC* pDC, CMapLayerObj* pMapLayerObj, CMapLayer* pMapLayer)
{
   CImageFile* pImage = (CImageFile*)pMapLayerObj->GetMapObject();

   // Determine if window is visible

   GetExtent(pMapLayer, pMapLayerObj);

   if (!IsVisible(pDC, pMapLayerObj)) return;   

   // Determine location of window

   CRect rect = m_rect;
   if (m_rectPaint.left != 0 && m_rectPaint.right != 0)
   {
      rect.IntersectRect(m_rect, m_rectPaint);
   };
   
   pImage->OnDraw(pDC, this, &rect);
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::FitText(CDC* pDC, CMapLayerObj* pMapLayerObj, CMapLayer* pMapLayer, 
                       int x, int y)
{
   CRect rect;
   BOOL bDrawText = FALSE;
   CRectText rectT;
   int iPos = CMapLayer::Start;
          
   // Search through all posible positions

   bDrawText = CheckOverlap(pDC, pMapLayer, pMapLayerObj->GetText(), x, y, rect, iPos);
   // If overlaps are allowed and the text cannot be fit
   // then draw it anyway

   if (pMapLayer->GetOverlap()) bDrawText = TRUE;

   // If the text label is outside the drawn rectangle then do not draw it

   if (rect.left < m_rect.left || rect.right > m_rect.right ||
       rect.top < m_rect.top || rect.bottom > m_rect.bottom)
   {
      bDrawText = FALSE;
   }
 
   if (bDrawText)
   {
      rectT.m_sText = pMapLayerObj->GetText();
      rectT.m_rect = rect;
      rectT.m_iPos = iPos;      
      rectT.m_nX = x;
      rectT.m_nY = y;      
      m_aRectTextL.Add(rectT);   
   };
   
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::DrawText(CDC* pDC, CMapLayer* pMapLayer)
{
   pDC->SetBkMode(TRANSPARENT);      
   pDC->SetTextColor(pMapLayer->GetTextColour());

   // Output the text
   int i = 0;

   for (i = 0; i < m_aRectTextL.GetSize(); i++)
   {
      pDC->TextOut(m_aRectTextL[i].m_rect.left, m_aRectTextL[i].m_rect.top, 
                   m_aRectTextL[i].m_sText);                      
   };   

   // Copy the text areas

   for (i = 0; i < m_aRectTextL.GetSize(); i++)
   {
      m_aRectText.Add(m_aRectTextL[i].m_rect);
   }
   m_aRectTextL.RemoveAll();

}

///////////////////////////////////////////////////////////////////////////////
//
// Returns true if the coordinate can be drawn and the rectangle into which
// it will be drawn
//


BOOL CViewMap::CheckOverlap(CDC* pDC, CMapLayer* pMapLayer, LPCSTR sText, 
                            int nX, int nY, CRect& rect, int& iPos)
{
   CSize sz(0,0);
   int nPosMin, nPosMax;
   BOOL bDrawText = FALSE;
   CRect rectI;
   int iOverlap = -1;

   if (pMapLayer->GetSize() == 0) return FALSE;
   BOOL bMapLines = pMapLayer->GetAt(0)->GetDataType() == BDMAPLINES;   
  
  // Determine the range of available positions

   if (pMapLayer->GetTextPos() == CMapLayer::BestFit)
   {
      if (!bMapLines)
      {
         nPosMin = CMapLayer::Right;
         nPosMax = CMapLayer::BottomLeft;
      } else
      {
         nPosMin = CMapLayer::Top;
         nPosMax = CMapLayer::Center;
      }
   } else
   {
      nPosMin = pMapLayer->GetTextPos();
      nPosMax = pMapLayer->GetTextPos();
   }

   // If refitting then start from last position

   if (iPos != CMapLayer::Start)
   {
      nPosMin = iPos;      
   };

   // Determine the rectangle
   
   sz = pDC->GetTextExtent(sText);
   
   for (iPos = nPosMin; iPos <= nPosMax && !bDrawText; iPos++)
   {
      bDrawText = TRUE;

      // For maplines, make centre the default option

      int nPos = iPos;
      if (pMapLayer->GetTextPos() == CMapLayer::BestFit && bMapLines)
      {
          nPos = iPos -1;
		  if (nPos < CMapLayer::Top) nPos = CMapLayer::Center;
      }

      switch (nPos)
      {
        case CMapLayer::Left :
           rect.left = nX - sz.cx - 2;
           rect.top = nY - sz.cy/2;
           break;
        case CMapLayer::Right :
           rect.left = nX + 2;
           rect.top = nY - sz.cy/2;
           break;
        case CMapLayer::Top :
            rect.left = nX - sz.cx/2;
            rect.top = nY - sz.cy -2;
          break;
       case CMapLayer::Bottom :
         rect.left = nX - sz.cx/2;
         rect.top = nY+2;
       break;            
         case CMapLayer::TopLeft : 
            rect.left = nX - sz.cx - 2; 
            rect.top = nY - sz.cy -2;
            break;
         case CMapLayer::BottomRight : 
            rect.left = nX +2; 
            rect.top = nY+2;
            break;
         case CMapLayer::TopRight :
            rect.left = nX +2; 
            rect.top = nY - sz.cy -2;
            break;
         case CMapLayer::BottomLeft:
            rect.left = nX - sz.cx - 2; 
            rect.top = nY+2;
            break;
         case CMapLayer::Center: 
            rect.left = nX - sz.cx/2;
            rect.top = nY - sz.cy/2;      
            break;
      }
      rect.right = rect.left + sz.cx;                  
      rect.bottom = rect.top + sz.cy;      

   // Check for overlap, do not display if string overlaps others
      int i = 0;

      for (i = 0; i < m_aRectText.GetSize(); i++)
      {
         if (rectI.IntersectRect(rect, m_aRectText[i]))
         {
            bDrawText = FALSE;
            break;
         }
      }

    // Check for overlap with existing text in this layer

      for (i = 0; i < m_aRectTextL.GetSize(); i++)
      {
         if (rectI.IntersectRect(rect, m_aRectTextL[i].m_rect))
         {
            bDrawText = FALSE;
            iOverlap = i;
            break;
         }
      }

    // Check for overlap with the edge of the page

      if (rect.left < m_rect.left || rect.right > m_rect.right ||
          rect.top < m_rect.top || rect.bottom > m_rect.bottom)
      {
         bDrawText = FALSE;
      }      
   };

   // If unable to fit the text then try moving existing text
   // Causing stack overflow!!

   /*if (iOverlap != -1 && !bDrawText)
   {
      // Move overlapping text

      CRectText rectT = m_aRectTextL[iOverlap];
      m_aRectTextL.RemoveAt(iOverlap);

      iPos =  rectT.m_iPos+1;      

      if (CheckOverlap(pDC, pMapLayer, rectT.m_sText, rectT.m_nX, rectT.m_nY, rect, 
                       iPos))
      {         
         // Re-add
         
         rectT.m_iPos = iPos;
         rectT.m_rect = rect;
         m_aRectTextL.Add(rectT);

         // If successful, try to re-fit the text
         iPos = CMapLayer::Start;
         bDrawText = CheckOverlap(pDC, pMapLayer, sText, nX, nY, rect, iPos);
      }
      // If not-restore the overlapping text
      else
      {
         m_aRectTextL.Add(rectT);
      }

   }*/

   return bDrawText;
}


///////////////////////////////////////////////////////////////////////////////
//
// If symbols have the option - 'best fit' then the symbol will be moved to 
// different locations until a position where it does not overlap is found
//

void CViewMap::CheckOverlapSym(CDC* pDC, double dSize, double& dX, double& dY)
{      
   BOOL bOverlap = TRUE;
   CRect rectI, rect;   
   CPoint pt;
   CPoint ptCoord((int) dX, (int) dY);       
   
   // Determine the rectangle
   
   CSize sz = CComboBoxSymbol::GetSymSize(pDC, dSize*1.5);
     
   for (int iPos = 1; iPos <= 9 && bOverlap; iPos++)
   {
      bOverlap = FALSE;
      switch (iPos)
      {      
      case 1: 
            pt.x = ptCoord.x - sz.cx; pt.y = ptCoord.y - sz.cy;          
            break;
      case 2: 
            pt.x = ptCoord.x - sz.cx; pt.y = ptCoord.y + sz.cy;          
            break;
      case 3:
            pt.x = ptCoord.x + sz.cx; pt.y = ptCoord.y + sz.cy;          
            break;
      case 4:
            pt.x = ptCoord.x + sz.cx; pt.y = ptCoord.y - sz.cy;          
            break;            
      case 5: 
            pt.x = ptCoord.x; pt.y = ptCoord.y - sz.cy;          
            break;
      case 6: 
            pt.x = ptCoord.x + sz.cx; pt.y = ptCoord.y;          
            break;
      case 7: 
            pt.x = ptCoord.x; pt.y = ptCoord.y + sz.cy;          
            break;
      case 8:
            pt.x = ptCoord.x - sz.cx; pt.y = ptCoord.y;          
            break;           
      case 9:
            pt.x = ptCoord.x; pt.y = ptCoord.y;                                  
            break;
      }
      rect.left = pt.x - sz.cx/2;
      rect.right = pt.x + sz.cx/2;
      rect.top = pt.y - sz.cx/2;                  
      rect.bottom = pt.y + sz.cy/2;      

   // Check for overlap, do not display if string overlaps others
      
      int i = 0; for (i = 0; i < m_aRectSym.GetSize(); i++)
      {
         if (rectI.IntersectRect(rect, m_aRectSym[i]))
         {
            bOverlap = TRUE;
            break;
         }
      }      
   };  
   m_aRectSym.Add(rect);

   dX = (rect.right + rect.left)/2;
   dY = (rect.top + rect.bottom)/2;
}

///////////////////////////////////////////////////////////////////////////////
//
// OLD retained for backwards compatibility
//

int CViewMap::GetLineWidth(CDC* pDC, CMapLayer* pMapLayer)
{   
   if (pMapLayer->GetLineStyle() == PS_SOLID)
   {
      return (int)(pMapLayer->GetLineWidth() * pDC->GetDeviceCaps(LOGPIXELSX) / MMPERINCH /3);
   } else
   {
      return 1;
   }
}

/////////////////////////////////////////////////////////////////////////////

int CViewMap::GetLineWidth(CDC* pDC, int nLineStyle, int nLineWidth)
{   
   if (nLineStyle == PS_SOLID)
   {
      return (int)(nLineWidth * pDC->GetDeviceCaps(LOGPIXELSX) / MMPERINCH /3);
   } else
   {
      return 1;
   }
}

/////////////////////////////////////////////////////////////////////////////
//
// World to screen
//

int CViewMap::GetfXPos(long a) 
{
   return (int)(((a - m_dOffX) * m_dZoom / (m_dMaxX - m_dMinX) + 0.5) * (m_rectA.right - m_rectA.left)+ m_rectA.left);   
           
}
int CViewMap::GetfYPos(long a) 
{ 
   return (int)(m_rectA.Height() - (((a - m_dOffY) * m_dZoom / (m_dMaxY - m_dMinY) + 0.5) * 
           (m_rectA.bottom - m_rectA.top)) + m_rectA.top);      
}

///////////////////////////////////////////////////////////////////////////////
//
// Screen to world

long CViewMap::GetfXInv(int a)
{
   return (long)(((((double)a - m_rectA.left) / (m_rectA.right - m_rectA.left)) - 0.5) * (m_dMaxX - m_dMinX) / m_dZoom + m_dOffX);
}

long CViewMap::GetfYInv(int a)
{
   return (long)((((m_rectA.Height() - ((double)a - m_rectA.top)) / (m_rectA.bottom - m_rectA.top)) - 0.5) * (m_dMaxY - m_dMinY) / m_dZoom + m_dOffY);
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnSize(UINT nType, int cx, int cy) 
{
	CBDView::OnSize(nType, cx, cy);
	
   GetClientRect(&m_rect);   	

   DetermineAspect();   
}

///////////////////////////////////////////////////////////////////////////////   
//
// Determine the aspect ratio for printing or for drawing
//

void CViewMap::DetermineAspect(CDC* pDC)
{
   // Allow for different shaped pixels

   if (pDC == NULL) pDC = GetDC();
   
   // Correct for aspect ratio

   m_rectA = m_rect;
   
   double dAspect = (m_dMaxX - m_dMinX) / (m_dMaxY - m_dMinY);
   dAspect *= (double)pDC->GetDeviceCaps(LOGPIXELSX) / pDC->GetDeviceCaps(LOGPIXELSY);

   if ((m_rect.right - m_rect.left) / (m_dMaxX - m_dMinX) >
       (m_rect.bottom - m_rect.top) / (m_dMaxY - m_dMinY))
   {
      double dWidth = m_rect.Height() * dAspect;
      m_rectA.left = m_rect.left + (m_rect.Width() - (int)dWidth)/2;
      m_rectA.right = m_rectA.left + (int)dWidth;      
   }
   else
   {
      double dHeight = m_rect.Width() / dAspect;
      m_rectA.top = m_rect.top + (m_rect.Height() - (int)dHeight)/2;
      m_rectA.bottom = m_rectA.top + (int)dHeight; 
   }  
}

///////////////////////////////////////////////////////////////////////////////   
//
// Returns the current scale, e.g. 1:50,000

int CViewMap::GetScale(CDC* pDC)
{
   CRect rect = m_rect;    
   
   if (pDC->IsPrinting())
   {
      rect = m_rectP;
   }

   if (m_bCopy)
   {
      SetCopyRect(pDC);
      rect = m_rect;
   }     

   // Determine the scale range in meters

   int nMin = (int)GetfXInv(rect.left);   
   int nMax = (int)GetfXInv(rect.right);   
   double dWidthW = nMax - nMin;   

   // Determine width of rectangle in metres on the screen or printer

   double dPixelsPerInch = pDC->GetDeviceCaps(LOGPIXELSX);
   double dWidthS = (rect.Width() / dPixelsPerInch) * MMPERINCH / 1000;

   // Tidy up

   if (m_bCopy)
   {
      GetClientRect(&m_rect);   
   };

   if (dWidthS == 0) return 0;
   else return (int)(dWidthW / dWidthS);
}

///////////////////////////////////////////////////////////////////////////////   

void CViewMap::OnLButtonDown(UINT nFlags, CPoint point) 
{  
   if (m_pDoc->GetLayers()->GetSize() == 0) return;

   // Add new point to polygon or polyline

   if (m_nMapMode == pan)
   {
	   m_ptPan = point;
	   return;
   }

   // Add points to array of lines

   if (m_nMapMode == measure)
   {
      CLongCoord coord;
      coord.x = GetfXInv(point.x);
      coord.y = GetfYInv(point.y);
      m_aMeasure.Add(coord);   
      
      // Remove last point

      CDC* pDC = GetDC(); // DC changes each time retrieved
      if (m_ptMeasure1.x != -1)
      {
         int nOldROP = pDC->SetROP2(R2_NOTXORPEN );
         pDC->MoveTo(m_ptMeasure2);      
         pDC->LineTo(m_ptMeasure1);
         pDC->SetROP2(nOldROP);

         m_ptMeasure1 = CPoint(-1,-1);
         m_ptMeasure2 = CPoint(-1,-1);

      }      

      // Update tape without redrawing entire map

      DrawTapeMeasure(pDC);      
      
      ReleaseDC(pDC);
      return;
   }

   if (m_pMapLinesEdit != NULL)
   {           
      CLongCoord coord;
      CLongLines longlines;
      coord.x = GetfXInv(point.x);
	   coord.y = GetfYInv(point.y);

      // Snap

      longlines.Add(coord);
      if (nFlags & MK_CONTROL)
      {
         SnapPoint(point, coord, longlines);
      }

      EditPolylines(longlines, TRUE);      
      return;
   }

   // Add new point

   if (m_pMapCoordEdit != NULL)
   {                
      m_pMapCoordEdit->x = GetfXInv(point.x);
	   m_pMapCoordEdit->y = GetfYInv(point.y);

      EditMode(FALSE);
      return;
   }



   // If drag mode is selected then allow controls to be dragged

   if (m_bDragMode)
   {
      CMapLayerObj* pMapLayerObj = HitTest(point, m_pEditMapLayer);
      if (pMapLayerObj != NULL && pMapLayerObj->GetDataType() == BDCOORD)
      {
         m_pMapLayer = m_pDoc->GetLayers()->GetAt(m_pDoc->GetLayers()->GetDefault());
         m_pEditMapObj = pMapLayerObj;                  
      }
   } 

   // Otherwise create the tracker

   else
   {
      CreateTracker(point);	
   };
	CBDView::OnLButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
//
// Snaps the point to the nearest node in the current layer, excluding edited
// object
//

void CViewMap::SnapPoint(CPoint point, CLongCoord coord, CLongLines& longlines)
{   
   double dDistMin = DBL_MAX;
   CPoint point1;
   CLongCoord coordMin;
   CLongLines *pLongLinesSnap = NULL;
   int iMin = -1;
   int iSnap = -1;
   int i1, i2;

   int i = 0, j = 0;

   ASSERT(m_pMapLayer != NULL);
   for (i = 0; i < m_pMapLayer->GetSize(); i++)
   {
      CMapLayerObj* pMapLayerObj = m_pMapLayer->GetAt(i); 
      
      ASSERT(pMapLayerObj->GetDataType() == BDMAPLINES);

      // Don't snap to lines being edited

      if (pMapLayerObj != m_pEditMapObj)
      {
         CLongLines* pMapLines = (CLongLines*)pMapLayerObj->GetMapObject();   

         for (j = 0; j < pMapLines->GetSize(); j++)
         {
             CLongCoord coordE = pMapLines->GetAt(j);

             if (!coordE.IsNull())
             {
                point1.x = GetfXPos(coordE.x);
                point1.y = GetfYPos(coordE.y);

                double dDist = square(point1.x - point.x)+square(point1.y-point.y);
                if (dDist < dDistMin)
                {
                    dDistMin = dDist;                    
                    coordMin = coordE;
                    pLongLinesSnap= pMapLines;
                    iMin = j;
                }
             }             
         }
      }      
   }

   if (dDistMin < square(4*NODESIZE))
   {
      longlines.RemoveAll();

      // Check to see if the object being edited is snapped to any other points on this object

      if (m_pMapLinesEdit->GetSize() > 0)
      {
         for (i2 = min(iMin+1,pLongLinesSnap->GetSize()-1); i2 < pLongLinesSnap->GetSize() && !pLongLinesSnap->GetAt(i2+1).IsNull(); i2++)
         {
            if (m_pMapLinesEdit->GetAt(m_iEditPoint1) == pLongLinesSnap->GetAt(i2)) 
            {
               iSnap = i2;
            }
         }
         for (i1 = max(0,iMin-1); i1 > 0 && !pLongLinesSnap->GetAt(i1-1).IsNull(); i1--)
         {
            if (m_pMapLinesEdit->GetAt(m_iEditPoint1) == pLongLinesSnap->GetAt(i1)) 
            {
               iSnap = i1;
            }
         }

         // If the object has been snapped then find the shortest length (nodes) and add these points

         if (iSnap != -1)
         {
            if (m_nMode & polyline)
            {
               if (iSnap < iMin)
               {
                  for (i = iSnap+1; i < iMin; i++) longlines.Add(pLongLinesSnap->GetAt(i));
               } else
               {
                  for (i = iSnap-1; i > iMin; i--) longlines.Add(pLongLinesSnap->GetAt(i));
               }
            }

            // For polygons need to loop round to start

            if (m_nMode & polygon)
            {
               if (abs(iSnap - iMin) < i2 - max(iSnap,iMin) + min(iSnap,iMin)-i1)
               {
                  if (iSnap < iMin)
                  {
                     for (i = iSnap+1; i < iMin; i++) longlines.Add(pLongLinesSnap->GetAt(i));
                  } else
                  {
                     for (i = iSnap-1; i > iMin; i--) longlines.Add(pLongLinesSnap->GetAt(i));
                  }
               } else
               {
                  if (iSnap < iMin)
                  {
                     for (i = iSnap-1; i >= i1; i--) longlines.Add(pLongLinesSnap->GetAt(i));
                     for (i = i2; i > iMin; i--) longlines.Add(pLongLinesSnap->GetAt(i));
                  } else
                  {
                     for (i = iSnap+1; i <= i2; i++) longlines.Add(pLongLinesSnap->GetAt(i));
                     for (i = i1; i < iMin; i++) longlines.Add(pLongLinesSnap->GetAt(i));
                  }
               }
            }
         }
      }

      longlines.Add(coordMin); 
   }
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::EditPolylines(const CLongLines& longlines, BOOL bAdd)
{
   CRect rect = CRect(0,0,0,0); // Refresh rect
   CPoint point, point1;
   int j;

   CArray <CLongCoord, CLongCoord> aUpdate;   

   if (bAdd)
   {
      // If clicking on a node then select it

      int iClosest = -1;
      double dDistMin = DBL_MAX;

      if (m_pMapLinesEdit->GetSize() > 0)
      {
         aUpdate.Add(m_pMapLinesEdit->GetAt(m_iEditPoint1));      
         aUpdate.Add(m_pMapLinesEdit->GetAt(m_iEditPoint2));      

         ASSERT(longlines.GetSize() > 0);
         CLongCoord coord = (CLongCoord&) longlines[0];
         point.x = GetfXPos(coord.x);
         point.y = GetfYPos(coord.y);
         
         int i = 0; for (i = 0; i < m_pMapLinesEdit->GetSize(); i++)
         {         
            CLongCoord coordE = m_pMapLinesEdit->GetAt(i);
            if (!coordE.IsNull())
            {
               point1.x = GetfXPos(coordE.x);
               point1.y = GetfYPos(coordE.y);            

               double dDist = square(point1.x - point.x)+square(point1.y-point.y);
               if (dDist < dDistMin)
               {
                  dDistMin = dDist;
                  iClosest = i;
               }
            };                       
         }      
      };

      // Select if not in insert mode (selecting initial active point), 
      // or when in insert mode (clicked on a node)

      if (iClosest != -1 && (!m_bInsertMode || 
          (m_bInsertMode && dDistMin < square(2*NODESIZE)))) 
      {
         m_iEditPoint1 = iClosest;
         m_iEditPoint2 = iClosest;

         AdjustEditParams();
         aUpdate.Add(m_pMapLinesEdit->GetAt(m_iEditPoint1));      
         aUpdate.Add(m_pMapLinesEdit->GetAt(m_iEditPoint2));      

      }

      // Insert      

      else if (m_bInsertMode)
      {
         int i = m_iEditPoint2;
         if (m_iEditPoint1 == m_iEditPoint2 && m_iEditPoint2 != 0) i++;
         for (j = 0; j < longlines.GetSize(); j++)
         {
            if (m_iEditPoint1 == 0 && m_iEditPoint2 == 0) m_pMapLinesEdit->InsertAt(i, (CLongCoord&) longlines[j]);
            else m_pMapLinesEdit->InsertAt(i+j, (CLongCoord&) longlines[j]);
         }
      
         // Store redraw area

         if (m_pMapLinesEdit->GetSize() > m_iEditPoint1 + 1)
         {
            aUpdate.Add(m_pMapLinesEdit->GetAt(m_iEditPoint1));
         }
         if (m_pMapLinesEdit->GetSize() > i + 2)
         {
            aUpdate.Add(m_pMapLinesEdit->GetAt(i + 1));
         };
         for (j = 0; j < longlines.GetSize(); j++)
         {
            aUpdate.Add((CLongCoord&) longlines[j]);      
         }

         // Update positions

         if (m_iEditPoint1 != 0 || m_iEditPoint2 != 0)
         {
            m_iEditPoint1+=j;
            m_iEditPoint2+=j;
         };
      } 
      // Select closest node

      else
      {
         
      }
   } 

   // Delete

   else
   {
      if (!m_pMapLinesEdit->GetAt(m_iEditPoint1).IsNull())
      {
         // Store redraw area

         int i = 0; for (i = 0; i < m_pMapLinesEdit->GetSize(); i++)
         {
            if (!m_pMapLinesEdit->GetAt(i).IsNull())
            {
               aUpdate.Add(m_pMapLinesEdit->GetAt(i));
            }
         }
         aUpdate.Add(m_pMapLinesEdit->GetAt(m_iEditPoint1));      

         
         if (m_nMode & polygon)
         {          

           // Remove point

           m_pMapLinesEdit->RemoveAt(m_iEditPoint1);         

            // If removing the first point then remove the end point

            if ((m_iEditPoint1 == 0 || m_pMapLinesEdit->GetAt(m_iEditPoint1-1).IsNull()) && m_pMapLinesEdit->GetAt(m_pMapLinesEdit->GetSize()-1).IsNull())
            {
				int i = 0;
               for (i = m_iEditPoint1; i < m_pMapLinesEdit->GetSize() && 
                    !m_pMapLinesEdit->GetAt(i).IsNull(); i++);
               if (i > 0)
               {
                  aUpdate.Add(m_pMapLinesEdit->GetAt(i-1));
                  m_pMapLinesEdit->RemoveAt(i-1);
                  
                  if (i-1 < m_iEditPoint2) m_iEditPoint2--;                  
               }                              
            }
            else if (m_iEditPoint1 > 0)
            {
               m_iEditPoint1--;
               m_iEditPoint2--;
            }
         }
         else 
         {
            m_pMapLinesEdit->RemoveAt(m_iEditPoint1);
            if (m_iEditPoint1 == m_iEditPoint2 && m_iEditPoint1 > 0 )
            {
               m_iEditPoint1--;
               m_iEditPoint2--;
            };
         }         
      };
   }

   // Ensure there is a terminating null

   if (m_pMapLinesEdit->GetSize() == 0 ||
      !m_pMapLinesEdit->GetAt(m_pMapLinesEdit->GetSize()-1).IsNull())
   {
      CLongCoord coord;
      coord.SetNull();
      m_pMapLinesEdit->Add(coord);
   };

   // If polygon then ensure close

   if (m_nMode & polygon)
   {
      int i1 = 0;
      int i = 0; for (i = 0; i < m_pMapLinesEdit->GetSize(); i++)
      {
         if (m_pMapLinesEdit->GetAt(i).IsNull())
         {
            if (i > 0 && m_pMapLinesEdit->GetAt(i1) != m_pMapLinesEdit->GetAt(i-1))
            {           
               m_pMapLinesEdit->InsertAt(i, m_pMapLinesEdit->GetAt(i1));               

               if (m_iEditPoint1 > i) 
               {
                  m_iEditPoint1++;
                  m_iEditPoint2++;
               }
            }            
            i1 = i+1;
         };
      }         
   }

   // For polygons, ensure that m_iEditPoint1 <> m_iEditPoint2 i.e. cannot add
   // a point to the end of the polygon

   AdjustEditParams();
   
   // Determine update rectangle
   int i = 0; for (i = 0; i < aUpdate.GetSize(); i++)
   {      
     CLongCoord coord = aUpdate.GetAt(i);

      if (!coord.IsNull())
      {
         point.x = GetfXPos(coord.x);
         point.y = GetfYPos(coord.y);

      
         if (rect.left == 0 && rect.right == 0)
         {
            rect.left = point.x-NODESIZE-1;
            rect.right = point.x+NODESIZE+1;
            rect.top = point.y-NODESIZE-1;
            rect.bottom = point.y+NODESIZE+1;
         } else
         {
            rect.left = min(rect.left, point.x-NODESIZE-1);
            rect.right = max(rect.right, point.x+NODESIZE+1);
            rect.top = min(rect.top, point.y-NODESIZE-1);
            rect.bottom = max(rect.bottom, point.y+NODESIZE+1);               
         };      
      };
   }

   // Include extremes



   // Redraw whole part of window unless adding point to end

   if (bAdd && m_iEditPoint1 == m_iEditPoint2)
   {
      CDC *pDC = GetDC();
      DrawMapEditLines(pDC, m_pMapLinesEdit, m_pEditMapLayer);
      ReleaseDC(pDC);
   }
   else
   {
      RedrawWindow(&rect);      
   } 
}


///////////////////////////////////////////////////////////////////////////////
//
// Ensures that parameters for polygon editing have valid values


void CViewMap::AdjustEditParams()
{
   if (m_nMode & polygon)
   {
      if (m_iEditPoint1 == m_iEditPoint2)
      {
         if (m_iEditPoint2 != 0)
         {
            m_iEditPoint1 = m_iEditPoint2-1;
         } else if (m_pMapLinesEdit->GetSize() > 0 && 
                   !m_pMapLinesEdit->GetAt(m_iEditPoint1).IsNull())
         {
           m_iEditPoint1 = 0;
           m_iEditPoint2 = 1;
         }
      };
   }
   // Sharing of a node is only available at the beginning or end of line

   else if (m_nMode & polyline)
   {
      if (m_iEditPoint1 == m_iEditPoint2)
      {
         if (m_iEditPoint1 > 0 && !m_pMapLinesEdit->GetAt(m_iEditPoint1-1).IsNull())
         {
            if (m_iEditPoint1+1 < m_pMapLinesEdit->GetSize() && 
                !m_pMapLinesEdit->GetAt(m_iEditPoint1+1).IsNull())
            {
               m_iEditPoint2++;
            }
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
//
// BOOL CViewMap::CreateTracker(CPoint point)
//


BOOL CViewMap::CreateTracker(CPoint point)
{ 
   CRect rect, rectTracker;   
  
   RemoveTracker();   
   
   m_pRectTracker = new CRectTracker(rect, CRectTracker::dottedLine);

   if (m_pRectTracker != NULL)
   {
      m_pRectTracker->TrackRubberBand(this, point);

      if (m_pRectTracker == NULL) return FALSE; // Track error

      if (m_pRectTracker->m_rect.Size() == m_pRectTracker->m_sizeMin || 
          m_pRectTracker->m_rect.Width() == 0)
      {
         return FALSE;
      }

      OnViewZoomRect();
   };
   return m_pRectTracker != NULL;
}; 

///////////////////////////////////////////////////////////////////////////////
//
// BOOL CViewMap::RemoveTracker()
//

BOOL CViewMap::RemoveTracker()
{
   if (m_pRectTracker != NULL)
   {
      delete m_pRectTracker;
      m_pRectTracker = NULL;
   }
   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
// void CViewMap::OnViewZoomRect()
//
// Zoom to selected area
//

void CViewMap::OnViewZoomRect()
{                              
   BOOL bOK = TRUE;
   CRect rectClient;

   m_dZoomP = m_dZoom;
   m_dOffXP = m_dOffX;
   m_dOffYP = m_dOffY;
     
   // Convert the tracker to world coordinates 

   m_pRectTracker->m_rect.NormalizeRect();

   double dTleft = GetfXInv(m_pRectTracker->m_rect.left);
   double dTright = GetfXInv(m_pRectTracker->m_rect.right);
   double dTtop = GetfYInv(m_pRectTracker->m_rect.top);
   double dTbottom = GetfYInv(m_pRectTracker->m_rect.bottom);
   
   // Determine pan
   
   double dOffX = (dTleft + dTright) / 2;
   double dOffY = (dTtop + dTbottom) / 2;
   double dZoom = m_dZoom;

   if (dTtop-dTbottom != 0 && dTright-dTleft != 0)
   {
      if ((dTright-dTleft)/(m_dMaxX-m_dMinX) > (dTtop-dTbottom)/(m_dMaxY-m_dMinY))
      {            
         dZoom = (m_dMaxX-m_dMinX)/(dTright-dTleft);          
      } else
      {
         dZoom = (m_dMaxY-m_dMinY)/(dTtop-dTbottom);
      };   
   };   

   // Prevent zooming beyong 10 metres across screen
   
     CLongCoord coord1, coord2;

     coord1.x = (int)m_dMinX;
     coord1.y = (int)m_dMinY;
     coord2.x = (int)m_dMaxX;
     coord2.y = (int)m_dMaxY;

     double dDist = BDProjection()->GetDistance(coord1, coord2, TRUE);
     double dZoomMin = (dDist)/MIN_ZOOM;   
                     
     if (dZoom / DEFAULT_ZOOM < dZoomMin)
     {           
         m_dZoom = dZoom;
         m_dOffX = dOffX;
         m_dOffY = dOffY;
     };

   // Remove tracker
   RemoveTracker();
   
  // Redraw the map
                 
   m_pDoc->ResetScale();
   Redraw();        

   m_nMapMode = none;
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnViewZoomin() 
{
  // Prevent zooming beyond 10 metres across screen     
      
   CLongCoord coord1, coord2;

   coord1.x = (int)m_dMinX;
   coord1.y = (int)m_dMinY;
   coord2.x = (int)m_dMaxX;
   coord2.y = (int)m_dMaxY;

   double dDist = BDProjection()->GetDistance(coord1, coord2, TRUE);
   double dZoomMin = (dDist)/MIN_ZOOM;   

// Calculate new zoom parameters

   m_dZoomP = m_dZoom;
   m_dOffXP = m_dOffX;
   m_dOffYP = m_dOffY;
    
   if (m_dZoom / DEFAULT_ZOOM < dZoomMin)
   {
      m_dZoom = m_dZoom / DEFAULT_ZOOM;
   };   
      
   RemoveTracker();   
   
// Redraw the map
   
   m_pDoc->ResetScale();
   Redraw();        	

   m_nMapMode = none;
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnViewZoomnormal() 
{
   BOOL bOK = TRUE;

   GetClientRect(m_rect);
  
   m_dZoom = 1;
   m_dOffX = (m_dMaxX + m_dMinX)/2;
   m_dOffY = (m_dMaxY + m_dMinY)/2;   
  
   if (!m_bLocator)
   {
      RemoveTracker();
                          
      // Redraw the map
      
      DetermineAspect();

      m_pDoc->ResetScale();
      Redraw();        		
   
      m_nMapMode = none;
   };
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnViewZoomout() 
{            
   // Prevent zooming beyong 10 metres across screen
      
   CLongCoord coord1, coord2;

   coord1.x = (int)m_dMinX;
   coord1.y = (int)m_dMinY;
   coord2.x = (int)m_dMaxX;
   coord2.y = (int)m_dMaxY;

   double dDist = BDProjection()->GetDistance(coord1, coord2, TRUE);
   double dZoomMax = (dDist)/MAX_ZOOM;

   if (BDProjection()->IsLatLon())
   {
      dZoomMax = dDist / MAX_ZOOMLATLON;
   }
   

  // For zoom out, if there has been a previous zoom in then return to this
  // if not return to the default zoom out

   if (m_dZoomP < m_dZoom)
   {
      m_dZoom = m_dZoomP;
      m_dOffX = m_dOffXP;
      m_dOffY = m_dOffYP;
   } else
   {  
      if (m_dZoom * DEFAULT_ZOOM > dZoomMax)
      {
         m_dZoom = m_dZoom * DEFAULT_ZOOM;      
      };
   };
        
   RemoveTracker();
      
  // Redraw the map

   m_pDoc->ResetScale();
   Redraw();        		
}



///////////////////////////////////////////////////////////////////////////////

BOOL CViewMap::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
		
	if (CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext))
	{          
	   return TRUE;
	};   
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnMouseMove(UINT nFlags, CPoint point) 
{  
	CRect rect;
   GetClientRect(rect);

   CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();

   // If measuring then display total distance to point

   if (m_nMapMode == measure && m_aMeasure.GetSize() > 0)
   {            

      // Measure distance so far

      double dDistance = 0;
      int i = 0; for (i = 0; i < m_aMeasure.GetSize()-1; i++)
      {         
         dDistance += BDProjection()->GetDistance(m_aMeasure[i], m_aMeasure[i+1]);
      }

      // Add distance to cursor

      CLongCoord coord;      
      coord.x = GetfXInv(point.x);
      coord.y = GetfYInv(point.y);

      dDistance += BDProjection()->GetDistance(m_aMeasure[m_aMeasure.GetSize()-1], coord);      

      // Display on status bar

      CString s;

      /*BOOL bKM = FALSE;
      if (dDistance > 1000)
      {
         dDistance /= 1000;
         bKM = TRUE;
      };

      if (bKM) s.Format("%s %.2lf ",(LPCSTR)BDString(IDS_TOTALLENGTH), dDistance);
      else s.Format("%s %.0lf ",(LPCSTR)BDString(IDS_TOTALLENGTH), dDistance);
      
      if (!bKM) s += BDString(IDS_METERS);
      else s+= BDString(IDS_KM);*/
      
      s.Format("%s %.1lf ",(LPCSTR)BDString(IDS_TOTALLENGTH), dDistance);
      s += (LPCSTR)BDGetApp()->GetUnits().GetLengthUnit().m_sAbbr;
      
      pWnd->SetStatusText(s);

      // Remove previous line

      if (m_ptMeasure1.x != -1)
      {
         CDC* pDC = GetDC(); // DC changes each time retrieved
         int nOldROP = pDC->SetROP2(R2_NOTXORPEN );
         pDC->MoveTo(m_ptMeasure2);      
         pDC->LineTo(m_ptMeasure1);
         pDC->SetROP2(nOldROP);
		 ReleaseDC(pDC);
      }

      // Rubber band the measure line

      m_ptMeasure1 = point;      
      m_ptMeasure2.x = GetfXPos(m_aMeasure[m_aMeasure.GetSize()-1].x);
      m_ptMeasure2.y = GetfYPos(m_aMeasure[m_aMeasure.GetSize()-1].y);

      CDC* pDC = GetDC(); // DC changes each time retrieved

      int nOldROP = pDC->SetROP2(R2_NOTXORPEN );
      pDC->MoveTo(m_ptMeasure2);      
      pDC->LineTo(m_ptMeasure1);
      pDC->SetROP2(nOldROP);
	  ReleaseDC(pDC);
           
      return;
   }

   // Display the mouse position on the status bar

   if (point.x >= 0 && point.y >= 0 && 
       point.x <= rect.right && point.y <= rect.bottom &&
       m_pDoc->GetLayers()->GetSize() > 0)
   {
      double dX = GetfXInv(point.x);
      double dY = GetfYInv(point.y);	

      
      CString s1,s2;      

      s1.Format(" %.0lf,%.0lf",dX,dY);      

      CCoord coord;
      coord.x = dX;
      coord.y = dY;
      BDProjection()->CoordAsString(coord, s2);

      if (BDProjection()->IsLatLon() || BDGetSettings().m_bCoordAsLatLon)
      {
          pWnd->SetStatusText(s2);
      } else
      {
         pWnd->SetStatusText(s1);
      };
      
   } else
   {
      pWnd->SetStatusText("");
   }	
   
   CBDView::OnMouseMove(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////

BOOL CViewMap::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
   if (m_nMapMode == pan)
   {
	   SetCursor(m_hPanCursor);
	   return TRUE;
   }
  
   if (m_bDragMode && m_pEditMapObj)
   {
      SetCursor(m_hDragCursor2);
      return TRUE;
   }
	else if (m_pEditMapObj != NULL || m_bDragMode)
   {
      if (m_bInsertMode)
      {
         SetCursor(m_hCurIns);
      } else
      {
         SetCursor(m_hCurSel);
      };
      return TRUE;
   } else
   {	
	   return CBDView::OnSetCursor(pWnd, nHitTest, message);
   };
}

///////////////////////////////////////////////////////////////////////////////
//
// Determine the selected feature
//

void CViewMap::OnRButtonDown(UINT nFlags, CPoint point) 
{
   if (m_pDoc->GetLayers()->GetSize() == 0) return;

   // End editing of map lines

   if (m_pMapLinesEdit != NULL)
   {      
      // End of polyline edit, and save changes
      EditMode(FALSE);                   
   }

   // If editing points then end this

   else if (m_bDragMode)
   {
      m_bDragMode = FALSE;
      m_pDoc->UpdateAllViews(this);
   }
	CBDView::OnRButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CAttrArray aAttr;    

   CView::OnRButtonUp(nFlags, point);

    if (m_pDoc->GetLayers()->GetSize() == 0) return;

   CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();

   m_pMapLayer = NULL;

   m_point = point;
   m_pMapLayerObj = HitTest(point, m_pMapLayer);

   // Display the File popup menu as a floating popup menu in the
   // client area of the main application window.

   ClientToScreen(&point);
   CMenu* pMapMenu = m_menuedit.GetSubMenu(0);

   // Allow editing of the feature
            
   if (m_pMapLayerObj != NULL)
   {   
      m_feature.m_lId = m_pMapLayerObj->GetFeature();
      m_feature.m_lFeatureTypeId = m_pMapLayer->GetFType();      

      if (BDFeature(BDHandle(), &m_feature, BDSELECT))
      {
         // Restore feature type, so inherited isn't used

         m_feature.m_lFeatureTypeId = m_pMapLayer->GetFType();

         BDEnd(BDHandle());

         // Set the status text to the selected feature

         pWnd->SetStatusText(m_feature.m_sName);

         // Determine if to enable view image
		   
		   m_bViewFile = FALSE;		 
         m_bViewHotLink = FALSE;

         BDFTypeAttrInit(BDHandle(), m_pMapLayer->GetFType(), &aAttr);
         int i = 0; for (i = 0; i < aAttr.GetSize(); i++)
         {
            if (aAttr.GetAt(i)->GetDataType() == BDHOTLINK) m_bViewFile = TRUE;
		      if (aAttr.GetAt(i)->GetDataType() == BDFILE) m_bViewFile = TRUE;		      
         }       
       		           
         pMapMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
                                 point.x, point.y, AfxGetMainWnd());    // owner is the main application window	                 
      };
   } 

   // Create popup menu for adding of features

   else
   {
           pMapMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
                                 point.x, point.y, AfxGetMainWnd());    // owner is the main application window	                 
   }

   BDEnd(BDHandle());
}


///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnMapEditdata() 
{   
   // Enable undo

   if (BDGetTransLevel(BDHandle()) > 0) BDCommit(BDHandle());
   BDBeginTrans(BDHandle());

   ASSERT(m_pMapLayerObj != NULL);

   CDlgEditAttributes dlg(m_feature.m_lId, m_feature.m_lFeatureTypeId, m_feature.m_sName);   
   if (dlg.DoModal() == IDOK)
   {
      CDlgProgress dlg;
      UpdateMapObject(m_pMapLayer, m_pMapLayerObj);
   };         
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnMapViewAttr() 
{
   ASSERT(m_pMapLayerObj != NULL);

   CMapLayer* pMapLayer = m_pDoc->GetLayers()->GetAt(m_pDoc->GetLayers()->GetDefault());
	
   if (!IsWindow(m_dlgViewAttr.GetSafeHwnd()))
   {
      m_dlgViewAttr.Create(CDlgViewAttr::IDD);
      m_dlgViewAttr.Update(m_feature.m_lId, m_feature.m_lFeatureTypeId);
      m_dlgViewAttr.ShowWindow(SW_SHOW);	
   };
}

void CViewMap::OnUpdateMapViewAttr(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(m_pMapLayerObj != NULL);	
}


///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnMapEditfeature() 
{  
   ASSERT(m_pMapLayerObj != NULL);

  // Enable undo

   if (BDGetTransLevel(BDHandle()) > 0)BDCommit(BDHandle());   
   BDBeginTrans(BDHandle());

  // Edit feature

	CDlgEditFeature dlg(m_feature.m_lFeatureTypeId, m_feature.m_lId);   
   if (dlg.DoModal() == IDOK)
   {
      UpdateMapObject(m_pMapLayer, m_pMapLayerObj);
   };       
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnMapAddFeature()
{
   long lFType = 0;   
   int nModeOld = m_nMode;

   // Enable undo

   if (BDGetTransLevel(BDHandle()) > 0) BDCommit(BDHandle());
   BDBeginTrans(BDHandle());

	// Determine feature type
	
    int i = m_pDoc->GetLayers()->GetDefault();       
	CMapLayer* pMapLayer = m_pDoc->GetLayers()->GetAt(i);
	lFType = pMapLayer->GetFType();
        	
   // Determine if polylines or points

   i = m_pDoc->GetLayers()->GetDefault();       

   if (m_nMode == 0)
   {
      BOOL bCoord = FALSE;
      CQueryElement* pElement = pMapLayer->GetQuery();
      while (pElement != NULL)
      {
         if (pElement->GetDataType() == BDCOORD && pElement->GetSelected())
         {
            m_nMode = points;
            break;
         } else if (pElement->GetDataType() == BDMAPLINES && pElement->GetSelected())
         {
            // If the editing a polygon (either new or existing)

            if ((m_nMode != 0 && m_nMode & polygon) || pMapLayer->IsPolygon())
            {
               m_nMode = polygon;
               break;
            } else
            {
               m_nMode = polyline;
               break;
            }
         }
         pElement = pElement->GetNextQuery();
      }      
   };

   // If set to polyline and maplayer is empty then confirm if polylines or polygons

   if (nModeOld == 0 && m_nMode == polygon && pMapLayer->GetSize() == 0)
   {
      CDlgDigitiseType dlg(TRUE);
      if (dlg.DoModal() == IDOK)
      {
         if (dlg.GetType() == SHPPolyLine) m_nMode = polyline;
      } else
      {
         return;
      }
   }

   // Create a new feature at the current coordinates

      if (lFType != 0)
      {            
		   m_pEditMapObj = new CMapLayerObj;
	      m_pEditMapObj->SetFeature(0);	
		   m_pEditMapLayer = pMapLayer;  
         m_pMapLayerObj = m_pEditMapObj;
      }   

   // Same as dragging

   if (m_pEditMapObj != NULL)
   {
      if (m_nMode & points)
      {
         CCoord* pCoord = new CCoord;
         m_pEditMapObj->SetMapObject(pCoord);
         m_pEditMapObj->SetDataType(BDCOORD);
         m_pMapCoordEdit = pCoord;

         EditMode(TRUE);             
      }
      else
      {               
         CLongLines* pMapLines = new CLongLines;
         m_pEditMapObj->SetMapObject(pMapLines);
         m_pEditMapObj->SetDataType(BDMAPLINES);
         m_pMapLinesEdit = pMapLines;

         EditMode(TRUE);             
      };
   };
}

///////////////////////////////////////////////////////////////////////////////
//
// Enables the editing of vertices for polylines or polygons or enters drag mode
// for points


void CViewMap::OnMapEditlines() 
{
   // Enable undo

   if (BDGetTransLevel(BDHandle()) > 0) BDCommit(BDHandle());
   BDBeginTrans(BDHandle());

   // Determine type of object
   
   if (m_pMapLayerObj->GetDataType() == BDMAPLINES)
   {      
      m_pEditMapLayer = m_pMapLayer; 
      m_pEditMapObj = m_pMapLayerObj;

      CLongLines* pMapLines = new CLongLines;
      pMapLines->Copy(*(CLongLines*)m_pMapLayerObj->GetMapObject());      

      m_pMapLinesEdit = pMapLines;

      // Determine if a polygon

      if (m_pMapLayer->IsPolygon()) m_nMode = polygon;
      else m_nMode = polyline;

      // Begin editing

      EditMode(TRUE); 

      // Select active point

      m_bInsertMode = FALSE;
      CLongCoord coord;
      CLongLines longlines;
      coord.x = GetfXInv(m_point.x);
      coord.y = GetfYInv(m_point.y);
      longlines.Add(coord);
      EditPolylines(longlines, TRUE);
      m_bInsertMode = TRUE;     
   } 

	
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnMapEditpoints() 
{
	m_bDragMode = !m_bDragMode;

   if (m_bDragMode) m_nMode = points;
   else m_nMode = 0;

   m_pDoc->UpdateAllViews(this);
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnUpdateMapEditpoints(CCmdUI* pCmdUI) 
{
   BOOL bPoints = FALSE;

   // Determine if layer contains points
   CMapLayer* pMapLayer = m_pDoc->GetLayers()->GetAt(m_pDoc->GetLayers()->GetDefault());
   if (pMapLayer != NULL && pMapLayer->GetSize() > 0)
   {
      if (pMapLayer->GetAt(0)->GetDataType() == BDCOORD) bPoints = TRUE;
   }

   pCmdUI->Enable(bPoints);
   pCmdUI->SetCheck(m_bDragMode);
	
}


///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnUpdateMapEditlines(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pMapLayerObj != NULL && 
                  m_pMapLayerObj->GetDataType() == BDMAPLINES);	
}

///////////////////////////////////////////////////////////////////////////////
//
// Pass polyline to be edited or to finish editing send NULL
//

BOOL CViewMap::EditMode(BOOL bStart, BOOL bAutoSave)
{           
   if (!bStart)
   {
      ASSERT(m_pEditMapObj != NULL);

      // If no feature defined then create a new one

      long lId = m_pEditMapObj->GetFeature();
      if (lId == 0)
      {
         CDlgMapFeatureSel dlg(m_pEditMapLayer->GetFType());   
         if (dlg.DoModal() == IDOK)
         {
            m_pEditMapObj->SetFeature(dlg.GetFeatureId());

         }
      };

      if (m_pEditMapObj->GetFeature() != 0)
      {  
         // Attempt to update existing attributes

         if (!UpdateMapObj(bAutoSave))         
         {
            // If a new feature then update through the editor,          

            CDlgEditAttributes dlg(m_pEditMapLayer->GetFType(), 
                                   m_pEditMapObj->GetFeature(), m_pEditMapObj);   
		      if (dlg.DoModal() == IDOK)
            {			  
               // Necessary as destructor not yet called for CDlgEditAttr
               BDSetProgressBar(NULL); 
               
               // Tidy up and prevent drawing
               m_pMapLinesEdit = NULL;
               m_pMapCoordEdit = NULL;

               // If polylines then ensure the map properties reflect this

               if (m_nMode & polyline)
               {
                  m_pEditMapLayer->GetMapProp().m_bPolygon = FALSE;
               }
               
               UpdateMapObject(m_pEditMapLayer, m_pEditMapObj);
            } 
            
            // Delete the new feature if not saving attributes

            else if (lId == 0)
            {
               CFeature feature;
               feature.m_lFeatureTypeId = m_pEditMapLayer->GetFType();
               feature.m_lId = m_pEditMapObj->GetFeature();
               BDFeature(BDHandle(), &feature, BDDELETE);
            }
         };
      };

     // Tidy up again in case of cancel
      
      if (lId == 0)
      {
         if (m_pEditMapObj != NULL) delete m_pEditMapObj;         
         m_pEditMapObj = NULL;
      } else
      {
         if (m_pMapLinesEdit != NULL)
         {
            delete m_pMapLinesEdit;
            m_pMapLinesEdit = NULL;
         };
         if (m_pMapCoordEdit != NULL)
         {
            delete m_pMapCoordEdit;
            m_pMapCoordEdit = NULL;
         }
      }
      
      m_pEditMapObj = NULL;   
      m_pEditMapLayer = NULL;
      m_pMapLayerObj = NULL;
      m_pMapLayer = NULL;
      m_pMapLinesEdit = NULL;
      m_pMapCoordEdit = NULL;            

      Redraw();

      if (!m_bDragMode) m_nMode = none;      
   } else
   {         
      // Initialise params

      if (m_nMode == polyline || m_nMode == polygon)
      {
         m_iEditPoint1 = 0;
         m_iEditPoint2 = 0;
         AdjustEditParams();
      };

   // Refresh the key view
      m_pDoc->UpdateAllViews(this);

   // Update this view for editing
      if (m_pEditMapObj->GetFeature() != 0)
      {
         Redraw();
      };
   };

   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CViewMap::UpdateMapObj(BOOL bAutoSave)
{
   CLongBinary longbin;

// Determine if the data already exists

   CAttrArray aAttr;
   aAttr.m_lFeature = m_pEditMapObj->GetFeature();
   aAttr.m_lFType = m_pEditMapLayer->GetFType();
   aAttr.m_lDate = m_pEditMapObj->GetDate();       

   if (BDAttribute(BDHandle(), &aAttr, BDSELECT))
   {
      if (bAutoSave || AfxMessageBox(IDS_SAVECHANGES, MB_YESNO) == IDYES)
      {
         long lAttrId = 0;

         // Determine which attribute contains the map lines
         CQueryElement* pElement = m_pEditMapLayer->GetQuery();
         while (pElement != NULL)
         {
            if (pElement->GetSelected())
            {
               if (pElement->GetDataType() == BDMAPLINES && 
                   (m_nMode == polyline || m_nMode == polygon))
               {
                   lAttrId = pElement->GetAttrId();
                   break;
               }
               if (pElement->GetDataType() == BDCOORD && m_nMode == points)
               {
                   lAttrId = pElement->GetAttrId();
                   break;
               }

            };
            pElement = pElement->GetNextQuery();
         }      

         // Update this attribute

         int i = 0; for (i = 0; i < aAttr.GetSize(); i++)
         {
            CAttribute* pAttr = aAttr.GetAt(i);
            if (pAttr->GetAttrId() == lAttrId)
            {    
               // Update polylines

               if (m_nMode == polyline || m_nMode == polygon)
               {
                  m_pMapLinesEdit->GetLongBinary(longbin);                  
                  pAttr->SetLongBinary(&longbin);

                  if (m_pMapLinesEdit != NULL)
                  {
                     delete m_pMapLinesEdit;
                     m_pMapLinesEdit = NULL;
                  };                  
                  break;
               } 
               // Update point

               else if (m_nMode == points)
               {
                  pAttr->SetCoord(*m_pMapCoordEdit);
                  ASSERT(m_pMapCoordEdit != NULL);
                  delete m_pMapCoordEdit;
                  m_pMapCoordEdit = NULL;

               }
            }
         }

         // Save back to database

         BDAttribute(BDHandle(), &aAttr, BDUPDATE);

         // Update
         UpdateMapObject(m_pEditMapLayer, m_pEditMapObj);
      };

      BDEnd(BDHandle());   

      return TRUE;
   }
   return FALSE;
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnMapSectoralReport()
{
   ASSERT(m_pMapLayerObj != NULL);

   CDocSummary::m_lFeatureDefault = m_feature.m_lId;
   CDocSummary::m_lFTypeDefault = m_feature.m_lFeatureTypeId;

   BDOnReportSummary();      
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnUpdateMapSectoralReport(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(m_pMapLayerObj != NULL);
}

///////////////////////////////////////////////////////////////////////////////

CMapLayerObj* CViewMap::HitTest(CPoint point, CMapLayer*& rpLayer)
{
   double dBestFit = 0;
   CMapLayerObj* pMapLayerObjBest = NULL;
   rpLayer = NULL;

   // For active layer

   int i = m_pDoc->GetLayers()->GetDefault();
   
   if (i < m_pDoc->GetLayers()->GetSize())
   {
      CMapLayer* pMapLayer = m_pDoc->GetLayers()->GetAt(i);

      int j = 0; for (j = 0; j < pMapLayer->GetSize() && pMapLayer->GetFType() != 0; j++)
      {
         CMapLayerObj* pMapLayerObj = pMapLayer->GetAt(j);

		 rpLayer = pMapLayer;
         
         // Check for a hit

         CRect& rect = pMapLayerObj->GetExtent();
         CRect rectS;
         rectS.left = (int)GetfXPos(rect.left);
         rectS.right = (int)GetfXPos(rect.right);
         rectS.top = (int)GetfYPos(rect.top);
         rectS.bottom = (int)GetfYPos(rect.bottom);

		 if (rectS.bottom > rectS.top) swap(rectS.bottom, rectS.top);

         // Check if the point lies within the extent
         
         if (rectS.left-1 <= point.x && point.x <= rectS.right+1 && 
             rectS.bottom-1 <= point.y && point.y <= rectS.top+1)
         {

            // For polygons, check if the point lies within the polygon

            if (pMapLayerObj->GetDataType() == BDMAPLINES)
            {
                 CCoord coord;
                 coord.x = GetfXInv(point.x);
                 coord.y = GetfYInv(point.y);
                 CLongLines* pLongLines = (CLongLines*)pMapLayerObj->GetMapObject();     
             
                 // Polygon test
                 if (pMapLayer->IsPolygon())
                 {
                    if (CSpatial::Inside(CLongCoord((long)coord.x,(long)coord.y), *pLongLines))
                    {
                        pMapLayerObjBest = pMapLayerObj;                         
                        break;
                    }
                 } 
                 // Test for click on polyline

                 else
                 {
                    if (OnPolyline(point, *pLongLines))
                    {
                        pMapLayerObjBest = pMapLayerObj;                         
                        break;
                    }
                 }
            } 

            // For points accept first object

            else
            {
                 pMapLayerObjBest = pMapLayerObj;                      
                 break;
            }
            
         }
      };      
   }
   return pMapLayerObjBest;
}

///////////////////////////////////////////////////////////////////////////////
//
// Returns true if point lies on polyline
//

BOOL CViewMap::OnPolyline(CPoint point, const CLongLines& longlines)
{    
    int i = 0; for (i = 0; i < longlines.GetSize()-1; i++)
    {
		CLongCoord& coord1 = (CLongCoord&) longlines.GetAt(i);
		CLongCoord& coord2 = (CLongCoord&) longlines.GetAt(i+1);

       if (!coord1.IsNull() && !coord2.IsNull())
       {          
          //CLongCoord& coord1 = longlines.GetAt(i);
          //CLongCoord& coord2 = longlines.GetAt(i+1);
 
   // Convert to screen coordinates

          CPoint point1 = CPoint(GetfXPos(coord1.x), GetfYPos(coord1.y));         
          CPoint point2 = CPoint(GetfXPos(coord2.x), GetfYPos(coord2.y));         

   // Prevent division by zero

          if (point1 != point2)
          {
  
   // Distance between point and line

           double d2 = square(point.x-point1.x)+square(point.y-point1.y);
           double e2 = square(point2.x-point.x) + square(point2.y-point.y);
           double g2 = square(point2.x-point1.x)+square(point2.y-point1.y);            
           double g = sqrt(g2);
           double c2 = e2 - square((d2 - e2 -g2) / (-2 * g));
           double a = sqrt(d2-c2);

           // Not on the line

           if (a/g >= 0 && a/g <= 1) 
           {   
              // Clicked on line if within one pixel

              if (c2 <= 4)
              {              
                 return TRUE;
              };
           };
          };
        }
    };
    return FALSE; //return the smallest distance between the hit point and the closest lines of the multyline.
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnKillFocus(CWnd* pNewWnd) 
{
	CBDView::OnKillFocus(pNewWnd);
	
   //ReleaseCapture();	
}

///////////////////////////////////////////////////////////////////////////////
//

void CViewMap::OnLButtonUp(UINT nFlags, CPoint point) 
{
   if (m_pDoc->GetLayers()->GetSize() == 0) return;	

   // If in pan mode then reposition the page

   if (m_nMapMode == pan)
   {
       int dX = GetfXInv(point.x) - GetfXInv(m_ptPan.x);
	   int dY = GetfYInv(point.y) - GetfYInv(m_ptPan.y);

      // Prevent panning too far

      if (!BDProjection()->IsLatLon())
      {
         if (fabs(m_dOffX - dX) > MAX_PAN || 
             fabs(m_dOffY - dY) > MAX_PAN) return;
      };

	   m_dOffX -= dX;                           
	   m_dOffY -= dY; 

	   Redraw();
	   m_ptPan = CPoint(-1,-1);
	   return;
   }

   if (m_pMapLinesEdit != NULL) return;

// Enable undo

   if (BDGetTransLevel(BDHandle()) > 0) BDCommit(BDHandle());
   BDBeginTrans(BDHandle());

   CCoord coord;
   CFeature feature;
   CArray <CAttrArray,CAttrArray> aAttrArray;   
   CAttrArray aAttr;

   if (m_pEditMapObj != NULL)
   {
      // Update the database value
      
      m_pMapCoordEdit = new CCoord;

      m_pMapCoordEdit->x = GetfXInv(point.x);
	   m_pMapCoordEdit->y = GetfYInv(point.y);

      EditMode(FALSE, TRUE);

      ASSERT(m_pMapCoordEdit == NULL);      
   }   

	CBDView::OnLButtonUp(nFlags, point);      
}

///////////////////////////////////////////////////////////////////////////////
//
// Updates the position of the map coordinate according to the value in the
// database
//

void CViewMap::UpdateMapObject(CMapLayer* pMapLayer, CMapLayerObj* pMapLayerObj)
{           
   ASSERT(pMapLayer != NULL && pMapLayerObj != NULL);

   BeginWaitCursor();

   m_pDoc->GetLayers()->Update(pMapLayer, pMapLayerObj);

   Redraw();

   EndWaitCursor();   
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	switch(nSBCode)
   {   
      case SB_LINEDOWN: 
            m_dOffY -= (m_dMaxY - m_dMinY) / m_dZoom/10;                           
      break;            

      case SB_LINEUP:
         m_dOffY += (m_dMaxY - m_dMinY) / m_dZoom/10;    
      break;
            
      case SB_PAGEDOWN:        
         m_dOffY -= (m_dMaxY - m_dMinY) / m_dZoom;                           
      break;            
      
      case SB_PAGEUP:                            
         m_dOffY += (m_dMaxY - m_dMinY) / m_dZoom;    
      break;         
     
      case SB_THUMBPOSITION:
         m_dOffY = m_dMinY + (m_dMaxY - m_dMinY) * (SCROLLRANGE-nPos) / SCROLLRANGE;
      break; 
  
      default:
         CBDView::OnVScroll(nSBCode, nPos, pScrollBar);
         return;
   }

   // Update the scroll position

   m_dOffY = max(m_dOffY, m_dMinY);
   m_dOffY = min(m_dOffY, m_dMaxY);
   
   // Redraw the map
    
    SetScrollPos();
    Invalidate();
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   switch(nSBCode)
   {     
      case SB_LINELEFT:        
         m_dOffX -= (m_dMaxX - m_dMinX) / m_dZoom/10;                  
      break;

      case SB_LINERIGHT:        
         m_dOffX += (m_dMaxX - m_dMinX) / m_dZoom/10;    
      break;

  // page left
         
      case SB_PAGELEFT:        
         m_dOffX -= (m_dMaxX - m_dMinX) / m_dZoom;                  
      break;            
  
  // page right
      
      case SB_PAGERIGHT:                   
         m_dOffX += (m_dMaxX - m_dMinX) / m_dZoom;    
      break;         
      
      case SB_THUMBPOSITION:         
         m_dOffX = m_dMinX + (m_dMaxX - m_dMinX) * nPos / SCROLLRANGE;         
      break;
     
      default:
         CBDView::OnHScroll(nSBCode, nPos, pScrollBar);
         return;
   }

   // Update the scroll position

   m_dOffX = max(m_dOffX, m_dMinX);
   m_dOffX = min(m_dOffX, m_dMaxX);
      
    // Redraw the map
    
    SetScrollPos();
    Invalidate();
}


///////////////////////////////////////////////////////////////////////////////
//
// void CViewMap::SetScrollPos()
//

void CViewMap::SetScrollPos()
{     
   if (m_dMaxX - m_dMinX != 0)
   {
	   CWnd::SetScrollPos(SB_HORZ,(int)((m_dOffX-m_dMinX) / (m_dMaxX - m_dMinX) * SCROLLRANGE));
	   CWnd::SetScrollPos(SB_VERT,(int)((1-(m_dOffY-m_dMinY) / (m_dMaxY - m_dMinY)) * SCROLLRANGE));
   };   
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::Redraw()
{                
   if (!m_bLocator)
   {
      SetScrollPos();   
      Invalidate(FALSE);
      m_pDoc->UpdateAllViews(this);         
   };
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnEditCopy() 
{
   CBDView::OnEditCopy();
}

///////////////////////////////////////////////////////////////////////////////
//
// Prevent from closing
//
void CViewMap::OnDestroy() 
{

}


///////////////////////////////////////////////////////////////////////////////
//
// Retrieves the appearance for the given map object
//

void CViewMap::GetStyle(CMapLayer* pMapLayer, CMapLayerObj* pMapLayerObj, double dValue, 
                        CMapStyle &mapstyle)
{
   // Set default values

   mapstyle.m_crFill = pMapLayer->GetColourFill();
   mapstyle.m_crLine = pMapLayer->GetColourLine();
   mapstyle.m_nPattern = pMapLayer->GetPattern();
   mapstyle.m_nHatch = pMapLayer->GetHatch();
   mapstyle.m_nLineStyle = pMapLayer->GetLineStyle();
   mapstyle.m_nLineWidth = pMapLayer->GetLineWidth();
   mapstyle.m_nSymbol = pMapLayer->GetSymbol();
   mapstyle.m_dSymSize = pMapLayer->GetSymSize();

   // Auto color

   if (pMapLayer->GetAutoColour())
   {
      if (pMapLayerObj != NULL) dValue = pMapLayerObj->GetValue();
      ASSERT(pMapLayer->GetAutoMax() - pMapLayer->GetAutoMin() != 0);
      if (!IsNullDouble(dValue))
      {  
         COLORREF cr = GetColour(dValue, pMapLayer->GetAutoMin(), pMapLayer->GetAutoMax(), 
                          pMapLayer->GetColour());      

         if (pMapLayer->IsPolygon()) mapstyle.m_crFill = cr;
         else mapstyle.m_crLine = cr;

      }
   }

   // Separate color

   else if (pMapLayer->GetSepColour() && pMapLayerObj != NULL)
   {
      int i = 0; for (i = 0; i < pMapLayer->GetMapProp().m_aColourFeature.GetSize(); i++)
      {
         CColourFeature &rColourFeature = pMapLayer->GetMapProp().m_aColourFeature.GetAt(i);

         if (pMapLayer->GetSepColour() == CMapLayer::SepColourFeature || 
             pMapLayer->GetSepColour() == CMapLayer::LegendValues)
         {
            if (rColourFeature.m_lFeatureId == pMapLayerObj->GetFeature())
            {
               mapstyle = (CMapStyle&)rColourFeature;               
            }
         } else
         {                             
            if (rColourFeature.m_sAttr == pMapLayerObj->GetText())

            {                 
               mapstyle = (CMapStyle&)rColourFeature;                            
            }
         }
      }
   }

   // Range color

   else if (pMapLayer->GetRangeColour())
   {

      if (pMapLayerObj != NULL) dValue = pMapLayerObj->GetValue();      
      if (!IsNullDouble(dValue))
      {
         CColourRangeArray& aRangeColour = pMapLayer->GetMapProp().m_aRangeColour;
         int i = 0; for (i = 0; i < aRangeColour.GetSize(); i++)
         {
            // Check for zero condition

            if (i == 0 && aRangeColour[i].m_dMin == aRangeColour[i].m_dMax &&
                fabs(dValue) < EPS)
            {
               mapstyle = aRangeColour[i];               
               break;
            }
         
            // Check against each colour rangee

            else if ((aRangeColour[i].m_dMin <= dValue && dValue < aRangeColour[i].m_dMax) ||
                aRangeColour[i].m_dMax == dValue && i == aRangeColour.GetSize()-1)
            {
               mapstyle = aRangeColour[i];               
               break;
            }
         }
      }
   }        
}

  
///////////////////////////////////////////////////////////////////////////////

COLORREF CViewMap::GetColour(double dValue, double dMin, double dMax, COLORREF cr)
{   
   COLORREF crNew = RGB(255,255,255);   
   double dLimit = dMax;
   
   double d = ((dValue - dMin) / (dMax - dMin));   

   if (dMin > dMax)
   {            
      d = ((dValue - dMin * 1.1) / (dMax - dMin * 1.1));
      dLimit = dMin*1.1;
   }
            
   if (dValue < dLimit)
   {        
      int r = GetRValue(cr);
      int g = GetGValue(cr);
      int b = GetBValue(cr);            
      crNew = RGB(255-(255-r)*d, 
                  255-(255-g)*d,
                  255-(255-b)*d);                     
   } else
   {
      if (dLimit == dMax) crNew = cr;             
   }            
   return crNew;
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::DrawRect(CDC* pDC, CRect* pRect, COLORREF cr, int nStyle, int nWidth)
{
   CPen pen(nStyle, nWidth, cr);
   CPen* pPenOld = pDC->SelectObject(&pen);
   pDC->MoveTo(pRect->left,pRect->top);
   pDC->LineTo(pRect->right, pRect->top);
   pDC->LineTo(pRect->right, pRect->bottom);
   pDC->LineTo(pRect->left, pRect->bottom);
   pDC->LineTo(pRect->left, pRect->top);
   pDC->SelectObject(pPenOld);
}

///////////////////////////////////////////////////////////////////////////////
//
// Draw the coordinates at the corners of the rectangle
//

void CViewMap::DrawCoords(CDC* pDC)
{
   CCoord coord;
   CString s;
   CRect rect;
   LOGFONT lf;
   CFont font;

   // Create font

   memset(&lf,0,sizeof(LOGFONT));
   lf.lfHeight = -8;
   lf.lfPitchAndFamily = 12;
   lf.lfCharSet = NRDB_CHARSET;
   strcpy(lf.lfFaceName, BDString(IDS_DEFAULTFONT));      
   ScaleFont(pDC, &lf);         
   font.CreateFontIndirect(&lf);  
   CFont* pFontOld = pDC->SelectObject(&font);           
   pDC->SetTextColor(RGB(0,0,64));

   // Determine the print preview draw area
  
   // Draw top left coordinate   

   if (m_pDoc->GetLayers()->GetMapGrid().m_nType == CMapGrid::defaultgrid)   
   {
      if (pDC->IsPrinting())
      {
         coord.x = (int)GetfXInv(m_rect.left);
         coord.y = (int)GetfYInv(m_rect.top);   
         rect.left = m_rect.left;
         rect.bottom = m_rect.top;
         rect.right = m_rect.right;
         rect.top = m_rectP.top;   
         BDProjection()->CoordAsString(coord, s);
         pDC->DrawText(s, &rect, DT_BOTTOM|DT_LEFT|DT_SINGLELINE|DT_NOPREFIX);
   
         // Draw top right

         coord.x = (int)GetfXInv(m_rect.right);
         coord.y = (int)GetfYInv(m_rect.top);   
         rect.left = m_rect.left;
         rect.bottom = m_rect.top;
         rect.right = m_rect.right;
         rect.top = m_rectP.top;   
         BDProjection()->CoordAsString(coord, s);
         pDC->DrawText(s, &rect, DT_BOTTOM|DT_RIGHT|DT_SINGLELINE);

         // Draw bottom left

         coord.x = (int)GetfXInv(m_rect.left);
         coord.y = (int)GetfYInv(m_rect.bottom);   
         rect.left = m_rect.left;
         rect.bottom = m_rectP.bottom;
         rect.right = m_rect.right;
         rect.top = m_rect.bottom;   
         BDProjection()->CoordAsString(coord, s);
         pDC->DrawText(s, &rect, DT_TOP|DT_LEFT);

         // Draw bottom right

         coord.x = (int)GetfXInv(m_rect.right);
         coord.y = (int)GetfYInv(m_rect.bottom);   
         rect.left = m_rect.left;
         rect.bottom = m_rectP.bottom;
         rect.right = m_rect.right;
         rect.top = m_rect.bottom;   
         BDProjection()->CoordAsString(coord, s);
         pDC->DrawText(s, &rect, DT_TOP|DT_RIGHT);
      };
   }

   // Draw coordinates along edge of map

   else if (m_pDoc->GetLayers()->GetMapGrid().m_nType == CMapGrid::labelsgrid ||
            m_pDoc->GetLayers()->GetMapGrid().m_nType == CMapGrid::labels)
   {
      DrawCoordsLat(pDC);
      DrawCoordsLng(pDC);      
   }   

   // Tidy up

   pDC->SelectObject(pFontOld);        

}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::DrawLayoutBorders(CDC *pDC)
{
   CMapLayout maplayout = m_pDoc->GetLayers()->GetMapLayout();

   int i = 0; for (i = 0; i < maplayout.m_aLayout.GetSize(); i++)
   {     
      CMapLayoutObj &layoutobj = maplayout.m_aLayout[i];
      if (layoutobj.m_nType == CMapLayout::box)
      {
		 // Create the bounding pen
         CPen pen(layoutobj.m_style.m_nLineStyle, layoutobj.m_style.m_nLineWidth, 
                  layoutobj.m_style.m_crLine);
         CPen *pPenOld = pDC->SelectObject(&pen);

		 // Fill the rectangle
		 CBrush brush;
	     CComboBoxPattern::CreateBrush(brush, layoutobj.m_style.m_nPattern,  layoutobj.m_style.m_nHatch, layoutobj.m_style.m_crFill);   
		 CBrush* pBrushOld = pDC->SelectObject(&brush);
		
         CRect rect = CMapLayout::RectFromPercent(m_rectP, layoutobj.m_rect);

		 pDC->FillRect(&rect, &brush);

         pDC->MoveTo(rect.left, rect.top);
         pDC->LineTo(rect.right, rect.top);
         pDC->LineTo(rect.right, rect.bottom);
         pDC->LineTo(rect.left, rect.bottom);
         pDC->LineTo(rect.left, rect.top);

		 pDC->SelectObject(pBrushOld);
         pDC->SelectObject(pPenOld);         
      };
   };
};

///////////////////////////////////////////////////////////////////////////////
// 
// Draws the image files
//

void CViewMap::DrawLayoutImages(CDC *pDC)
{
   CMapLayout maplayout = m_pDoc->GetLayers()->GetMapLayout();

   int i = 0; for (i = 0; i < maplayout.m_aLayout.GetSize(); i++)
   {     
      CMapLayoutObj &layoutobj = maplayout.m_aLayout[i];
      if (layoutobj.m_nType == CMapLayout::picture)
      {         
         CRect rect = CMapLayout::RectFromPercent(m_rectP, layoutobj.m_rect);

         // Hacky, reloading each time

         CImageFile image;

         // Add default directory if no path
         CString s = layoutobj.m_sImageFile;
         if (s.Find("\\") == -1) s = BDGetAppPath() + s;

         if (image.Open(s))
         {
            image.OnDraw(pDC, rect);
            image.Close();
         }

       
      };
   };
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::DrawLayoutText(CDC *pDC)
{
   CMapLayout maplayout = m_pDoc->GetLayers()->GetMapLayout();

   int i = 0; for (i = 0; i < maplayout.m_aLayout.GetSize(); i++)
   {     
      CMapLayoutObj &layoutobj = maplayout.m_aLayout[i];
      if (layoutobj.m_nType == CMapLayout::text)
      {         
         CRect rect = CMapLayout::RectFromPercent(m_rectP, layoutobj.m_rect);

         // Create font

         CFont font;
         LOGFONT lf = layoutobj.m_logfont;;   
         ScaleFont(pDC, &lf);         
         font.CreateFontIndirect(&lf);  
         CFont* pFontOld = pDC->SelectObject(&font);           

         pDC->SetBkMode(TRANSPARENT);      
         pDC->DrawText(layoutobj.m_sText, rect, DT_LEFT);

         pDC->SelectObject(pFontOld);
      };
   };
}

///////////////////////////////////////////////////////////////////////////////
//
// Draws the locator window onto the map view
//

void CViewMap::DrawLayoutLocator(CDC *pDC)
{
   if (!m_bLocator)
   {
      m_pDoc->GetViewLocator()->DrawLocator(pDC);      
   };
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::DrawCoordsLat(CDC* pDC)
{   
   double dMinLatY1, dMaxLatY1, dMinLatY2, dMaxLatY2;
   double dMinLngY1, dMaxLngY1, dMinLngY2, dMaxLngY2;
   double dLng;
   double dMin, dMax;
   double d;
   long x,y;
   CPoint pt1, pt2;
   CCoord coord;
   CString s;
   CRect rect;
   CSize sz;
   CPoint ptA,ptB;

   CMapGrid &mapgrid = m_pDoc->GetLayers()->GetMapGrid();


   // Set flags

   int nFlags = CProjection::latitude | CProjection::legend;
   if (mapgrid.m_nSecLat > 0) nFlags |= CProjection::seconds;

   // Create font

   CFont font;
   LOGFONT lf = mapgrid.m_logfont;
   
   ScaleFont(pDC, &lf);         
   font.CreateFontIndirect(&lf);  
   CFont* pFontOld = pDC->SelectObject(&font);           

   COLORREF crOld = pDC->SetTextColor(mapgrid.m_style.m_crLine);
   
   // Create pen

   CPen pen(mapgrid.m_style.m_nLineStyle, mapgrid.m_style.m_nLineWidth, mapgrid.m_style.m_crLine);
   CPen *pPenOld = pDC->SelectObject(&pen);

   // Determine extremes of box

   coord.x = (int)GetfXInv(m_rect.left);
   coord.y = (int)GetfYInv(m_rect.bottom); 
   BDProjection()->TransMercatorToLatLon(coord.x, coord.y, &dMinLatY1, &dMinLngY1);
   coord.x = (int)GetfXInv(m_rect.left);
   coord.y = (int)GetfYInv(m_rect.top); 
   BDProjection()->TransMercatorToLatLon(coord.x, coord.y, &dMaxLatY1, &dMaxLngY1);
   coord.x = (int)GetfXInv(m_rect.right);
   coord.y = (int)GetfYInv(m_rect.bottom); 
   BDProjection()->TransMercatorToLatLon(coord.x, coord.y, &dMinLatY2, &dMinLngY2);
   coord.x = (int)GetfXInv(m_rect.right);
   coord.y = (int)GetfYInv(m_rect.top); 
   BDProjection()->TransMercatorToLatLon(coord.x, coord.y, &dMaxLatY2, &dMaxLngY2);


   // Now determine the starting position

   double dInterval = mapgrid.m_nDegLat + mapgrid.m_nMinLat/60.0 + mapgrid.m_nSecLat/3600.0;

   BDProjection()->TransMercatorToLatLon(GetfXInv((m_rect.left+m_rect.right)/2), GetfYInv(m_rect.bottom), &dMin, &d);
   BDProjection()->TransMercatorToLatLon(GetfXInv((m_rect.left+m_rect.right)/2), GetfYInv(m_rect.top), &dMax, &d);

   dMin = floor(dMin / dInterval) * dInterval;

   for (double dLat = max(-90,dMin); dLat <= min(90,dMax) && dInterval > 0; dLat += dInterval)
   {               
       // Initialise

      pt1 = CPoint(-1,-1);
      pt2 = CPoint(-1,-1);

      // Clip

      CRect rectC = GetScreenRect(pDC);
      CRgn rgn;
      rgn.CreateRectRgn(rectC.left, rectC.top, rectC.right, rectC.bottom);
      pDC->SelectClipRgn(&rgn);
      
      int i = 0;
      for (dLng = max(-180,min(dMinLngY1, dMaxLngY1) - dInterval); dLng <= min(180,max(dMinLngY2, dMaxLngY2)) + dInterval; dLng += dInterval)
      {                       
         BDProjection()->LatLonToTransMercator(dLat, dLng, &x, &y);         
         ptB = CPoint(GetfXPos(x), GetfYPos(y));         

         if (i == 0)
         {
            pDC->MoveTo(ptB);
            i++;
         } else
         {     
            // Determine intersection with bounding box

            pt1.x = m_rect.left;
            if ((ptA.x <= pt1.x && pt1.x <= ptB.x))
            {
               if (ptB.x != ptA.x) pt1.y = ((pt1.x - ptA.x) * (ptB.y - ptA.y)) / (ptB.x - ptA.x) + ptA.y; 
               else pt1.y = ptA.x;
            }

            pt2.x = m_rect.right;
            if ((ptA.x <= pt2.x && pt1.x <= ptB.x))
            {
               if (ptB.x != ptA.x) pt2.y = ((pt2.x - ptA.x) * (ptB.y - ptA.y)) / (ptB.x - ptA.x) + ptA.y; 
               else pt2.y = ptA.x;
            }

            // Draw line

            if (mapgrid.m_nType == CMapGrid::labelsgrid)
            {
               pDC->LineTo(ptB);         
            };

            ptA = ptB;
         };
      };

      pDC->SelectClipRgn(NULL);
   

      if (pDC->IsPrinting())
      {
         
         // Draw text

         BDProjection()->LatLongAsString(dLat, s, nFlags);
         sz = pDC->GetTextExtent(s);

         // Draw ticks

         if (m_rect.top < pt1.y && pt1.y < m_rect.bottom)
         {
            pDC->MoveTo(pt1.x, pt1.y);
            pDC->LineTo(pt1.x - sz.cy/2, pt1.y);
         };

         if (m_rect.top < pt2.y && pt2.y < m_rect.bottom)
         {
            pDC->MoveTo(m_rect.right, pt2.y);
            pDC->LineTo(m_rect.right + sz.cy/2, pt2.y);
         };

         // Draw text
      
         // Left

         if (m_rect.top < pt1.y && pt1.y < m_rect.bottom)
         {
            rect.right = m_rect.left - sz.cy/2;
            rect.left = rect.right - sz.cx;
            rect.top = pt1.y - sz.cy/2;
            rect.bottom = pt1.y + sz.cy/2;
               
            pDC->DrawText(s, &rect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
         };

         // Right 
         if (m_rect.top < pt2.y && pt2.y < m_rect.bottom)
         {
            rect.left = m_rect.right + sz.cy/2;
            rect.top = pt2.y - sz.cy/2;
            rect.bottom = pt2.y + sz.cy/2;
            rect.right = rect.left + sz.cx;
      
            pDC->DrawText(s, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
         };
      };                        
   }

   // Tidy up

   pDC->SelectObject(pPenOld);
   pDC->SelectObject(pFontOld);           
   pDC->SetTextColor(crOld);
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::DrawCoordsLng(CDC* pDC)
{
   double dMinLatY1, dMaxLatY1, dMinLatY2, dMaxLatY2;
   double dMinLngY1, dMaxLngY1, dMinLngY2, dMaxLngY2;
   double dMinLat, dMaxLat;
   double dLat, dLng;
   double d;
   double dMin, dMax;
   long x,y;
   CPoint pt1, pt2;
   CCoord coord;
   CString s;
   CRect rect;
   CSize sz;
   CPoint ptA,ptB;

   CMapGrid &mapgrid = m_pDoc->GetLayers()->GetMapGrid();


   // Set flags

   int nFlags = CProjection::latitude | CProjection::legend;
   if (mapgrid.m_nSecLat > 0) nFlags |= CProjection::seconds;

   // Create font

   CFont font;
   LOGFONT lf = mapgrid.m_logfont;
   
   ScaleFont(pDC, &lf);         
   font.CreateFontIndirect(&lf);  
   CFont* pFontOld = pDC->SelectObject(&font);           

   COLORREF crOld = pDC->SetTextColor(mapgrid.m_style.m_crLine);
   
   // Create pen

   CPen pen(mapgrid.m_style.m_nLineStyle, mapgrid.m_style.m_nLineWidth, mapgrid.m_style.m_crLine);
   CPen *pPenOld = pDC->SelectObject(&pen);

   // Determine extremes of box

   coord.x = (int)GetfXInv(m_rect.left);
   coord.y = (int)GetfYInv(m_rect.bottom); 
   BDProjection()->TransMercatorToLatLon(coord.x, coord.y, &dMinLatY1, &dMinLngY1);
   coord.x = (int)GetfXInv(m_rect.left);
   coord.y = (int)GetfYInv(m_rect.top); 
   BDProjection()->TransMercatorToLatLon(coord.x, coord.y, &dMaxLatY1, &dMaxLngY1);
   coord.x = (int)GetfXInv(m_rect.right);
   coord.y = (int)GetfYInv(m_rect.bottom); 
   BDProjection()->TransMercatorToLatLon(coord.x, coord.y, &dMinLatY2, &dMinLngY2);
   coord.x = (int)GetfXInv(m_rect.right);
   coord.y = (int)GetfYInv(m_rect.top); 
   BDProjection()->TransMercatorToLatLon(coord.x, coord.y, &dMaxLatY2, &dMaxLngY2);

   // Now determine the starting position

   double dInterval = mapgrid.m_nDegLng + mapgrid.m_nMinLng/60.0 + mapgrid.m_nSecLng/3600.0;
   
   BDProjection()->TransMercatorToLatLon(GetfXInv((m_rect.left+m_rect.right)/2), GetfYInv(m_rect.bottom), &dMinLat, &d);
   BDProjection()->TransMercatorToLatLon(GetfXInv((m_rect.left+m_rect.right)/2), GetfYInv(m_rect.top), &dMaxLat, &d);
   dMin = min(min(min(dMinLngY1, dMinLngY2), dMaxLngY1),dMaxLngY2);
   dMax = max(max(max(dMinLngY1, dMinLngY2), dMaxLngY1),dMaxLngY2);

   dMin = floor(dMin / dInterval) * dInterval;

   for (dLng = max(-180,dMin); dLng < min(180,dMax) && dInterval > 0; dLng += dInterval)
   {         
      // Initialise

      pt1 = CPoint(-1,-1);
      pt2 = CPoint(-1,-1);

       // Draw line
            
      CRect rectC = GetScreenRect(pDC);

      CRgn rgn;
      rgn.CreateRectRgn(rectC.left, rectC.top, rectC.right, rectC.bottom);
      pDC->SelectClipRgn(&rgn);

      int i = 0;
      for (dLat = max(-90,dMinLat-dInterval*2); dLat <= min(90,dMaxLat+dInterval); dLat += dInterval)
      {                       
         BDProjection()->LatLonToTransMercator(dLat, dLng, &x, &y);         
         ptB = CPoint(GetfXPos(x), GetfYPos(y));

         if (i == 0)
         {
            pDC->MoveTo(ptB);
            i++;
         } else
         {     
            // Determine intersection with bounding box

            pt1.y = m_rect.bottom;
            if ((ptB.y <= pt1.y && pt1.y <= ptA.y))
            {
               if (ptA.y != ptB.y) pt1.x = ((pt1.y - ptA.y) * (ptB.x - ptA.x)) / (ptB.y - ptA.y) + ptA.x; 
               else pt1.x = ptA.x;
            }

            pt2.y = m_rect.top;
            if ((ptB.y <= pt2.y && pt2.y <= ptA.y))
            {
               if (ptA.y != ptB.y) pt2.x = ((pt2.y - ptA.y) * (ptB.x - ptA.x)) / (ptB.y - ptA.y) + ptA.x; 
               else pt2.x = ptA.x;
            }


            // Draw line
         
            if (mapgrid.m_nType == CMapGrid::labelsgrid)
            {
               pDC->LineTo(GetfXPos(x), GetfYPos(y));         
            };

            ptA = ptB;
         };
      };
      
      pDC->SelectClipRgn(NULL);
      

      if (pDC->IsPrinting())
      {
         // Draw text

         BDProjection()->LatLongAsString(dLng, s, nFlags);
         sz = pDC->GetTextExtent(s);

         // Draw ticks            

         if (m_rect.left < pt1.x && pt1.x < m_rect.right)
         {
            pDC->MoveTo(pt1.x, pt1.y);
            pDC->LineTo(pt1.x, pt1.y +  sz.cy/2);
         }

         if (m_rect.left < pt2.x && pt2.x < m_rect.right)
         {
            pDC->MoveTo(pt2.x, pt2.y);
            pDC->LineTo(pt2.x, pt2.y - sz.cy/2);
         };

         // Draw text
      
         // Bottom

         if (m_rect.left < pt1.x && pt1.x < m_rect.right)
         {
            rect.right = pt1.x + sz.cx/2;
            rect.left = pt1.x - sz.cx/2;
            rect.top = pt1.y + sz.cy/2;
            rect.bottom = rect.top + sz.cy;
               
            pDC->DrawText(s, &rect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
         };

         // Top

         if (m_rect.left < pt2.x && pt2.x < m_rect.right)
         {
            rect.right = pt2.x + sz.cx/2;
            rect.left = pt2.x - sz.cx/2;
            rect.bottom = pt2.y - sz.cy/2;
            rect.top = rect.bottom - sz.cy;
      
            pDC->DrawText(s, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);                  
         };
      }      
   }

   // Tidy up

   pDC->SelectObject(pPenOld);
   pDC->SelectObject(pFontOld);           
   pDC->SetTextColor(crOld);

}

///////////////////////////////////////////////////////////////////////////////

int CViewMap::DrawTitle(CDC *pDC, CRect* pRect)
{   
   CFont font;
   LOGFONT lf;
   int cy;

   // Create large font for title
   
   pDC->SetTextColor(RGB(0,0,0));  

   lf = m_pDoc->GetLayers()->GetFontT();
   ScaleFont(pDC, &lf);         
   font.CreateFontIndirect(&lf);  
   CFont* pFontOld = pDC->SelectObject(&font);           

   CRect rect;
   if (pRect == NULL)
   {
      rect.left = m_rect.left;
      rect.right = m_rect.right;
      rect.top = m_rectP.top+2;
      rect.bottom = m_rect.top;
   } else
   {
      rect = *pRect;
   }

   if (pRect == NULL)
   {
      cy = pDC->DrawText(m_pDoc->GetLayers()->GetTitle(), &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX); 
   } else
   {
      cy = pDC->DrawText(m_pDoc->GetLayers()->GetTitle(), &rect, DT_LEFT|DT_WORDBREAK|DT_TOP|DT_NOPREFIX); 
   };

   pDC->SelectObject(pFontOld);         

   return cy;
}


///////////////////////////////////////////////////////////////////////////////
//
// Draw the north arrow to the right of the scale bar
//

void CViewMap::DrawNorthArrow(CDC* pDC)
{
   CPen pen(PS_SOLID, 2, RGB(0,0,0));
   CPen* pPenOld = pDC->SelectObject(&pen);   
   int cx = m_rect.left + (m_rect.Width()*9)/10;

   CSize sz = pDC->GetTextExtent("W");      
   sz.cy /=2;
   
   int cy = min(m_rect.bottom + sz.cy*3, m_rectP.bottom-sz.cy/2);
   
   // Draw Arrow
   
   pDC->MoveTo(cx, cy);
   pDC->LineTo(cx-sz.cy/2, cy+sz.cy*2);
   pDC->LineTo(cx, cy+(sz.cy*3)/2);
   pDC->LineTo(cx+sz.cy/2, cy+sz.cy*2);
   pDC->LineTo(cx, cy);
   cy += sz.cy*2;

   // Draw N

   pDC->MoveTo(cx+sz.cy/4, cy);
   pDC->LineTo(cx+sz.cy/4, cy+sz.cy);
   pDC->LineTo(cx-sz.cy/4, cy);
   pDC->LineTo(cx-sz.cy/4, cy+sz.cy);
   cy += sz.cy*2;

   pDC->SelectObject(pPenOld);        
   
}




///////////////////////////////////////////////////////////////////////////////

CRect CViewMap::GetScreenRect(CDC* pDC)
{
   CPoint ptOrg;
   CRect rectC = m_rect;

   if (pDC->IsKindOf(RUNTIME_CLASS(CPreviewDC)))        
   {
      CPreviewDC *pPrevDC = (CPreviewDC *)pDC;
      pPrevDC->PrinterDPtoScreenDP(&rectC.TopLeft());
      pPrevDC->PrinterDPtoScreenDP(&rectC.BottomRight());        
      ::GetViewportOrgEx(pDC->m_hDC,&ptOrg); 

      rectC += ptOrg;
   }
   return rectC;
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
    pMainFrame->GetMessageBar()->SetWindowText("");   
	
	CBDView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pDoc->GetLayers()->GetSize() > 0);	
}


///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnFilePrintPreview() 
{
   CBDView::OnFilePrintPreview();
}

void CViewMap::OnFilePrint() 
{
   CBDView::OnFilePrint();
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnEditSearch() 
{
   m_aSearchObj.RemoveAll();

   CDlgSearch dlg;
   if (dlg.DoModal())
   {
	   CString sSearch = dlg.GetSearch();

		CMapLayer* pMapLayer = m_pDoc->GetLayers()->GetAt(m_pDoc->GetLayers()->GetDefault());
            
		  // Display objects within the layer

        sSearch.MakeLower();

		  int j = 0; for (j = 0; j < pMapLayer->GetSize(); j++)
		  {         
			 CMapLayerObj* pMapLayerObj = pMapLayer->GetAt(j);

		 // If default layer then check for search match
         
         CString sCompare = pMapLayerObj->GetText();
         sCompare.MakeLower();

			if (sSearch != "" && sCompare.Find(sSearch) != -1)
			{
				m_aSearchObj.Add(pMapLayerObj);
				SetTimer(0,1000,NULL);
			}
		}	
   } 
   Redraw();
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnTimer(UINT nIDEvent) 
{
	m_nSearch++;

	CMapLayer* pMapLayer = m_pDoc->GetLayers()->GetAt(m_pDoc->GetLayers()->GetDefault());

	// Draw selected objects

	if (m_aSearchObj.GetSize() > 0)
	{
      CDC *pDC = GetDC();
		int i = 0; for (i = 0; i < m_aSearchObj.GetSize(); i++)
		{
			CMapLayerObj* pMapLayerObj = m_aSearchObj[i];

			if (pMapLayerObj->GetDataType() == BDMAPLINES)
			{
   			   DrawMapLines(pDC, pMapLayerObj, pMapLayer, m_nSearch%2);
			}
			else if (pMapLayerObj->GetDataType() == BDCOORD)
			{
			   DrawCoords(pDC, pMapLayerObj, pMapLayer, m_nSearch%2);
			}
         ReleaseDC(pDC);
		}		
	} else
	{
		KillTimer(0);
	}
	CBDView::OnTimer(nIDEvent);
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnUpdateEditSearch(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(m_pDoc->GetLayers()->GetSize() > 0);  	
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnUpdateMapEditdata(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(m_pMapLayerObj != NULL);
	
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnUpdateMapEditfeature(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(m_pMapLayerObj != NULL);
	
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnUpdateMapAddFeature(CCmdUI* pCmdUI)
{
   BOOL bEnable = m_pDoc->GetLayers()->GetSize() > 0;

   // No adding for images or overlays

   if (m_pMapLayer != NULL && ((CString(m_pMapLayer->GetFileName()) != "") || 
       (m_pMapLayer->GetSize() != 0 && m_pMapLayer->GetAt(0)->GetDataType() == BDIMAGE)))
   {
      bEnable = FALSE;
   }
   pCmdUI->Enable(bEnable);
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnEditUndo() 
{
   // TODO transactions must be committed before some other part
   // of program accesses the database e.g. editing

   ASSERT(BDGetTransLevel(BDHandle()) > 0);

   if (m_pMapLayer != NULL)
   {      
      BDRollBack(BDHandle());      

      // Reload the data

	  CDlgProgress dlg(FALSE);

      m_pMapLayer->RemoveAll();
      m_pMapLayer->LoadData();

      Redraw();
   };	
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(BDGetTransLevel(BDHandle()) > 0 && m_pMapLayer != NULL);	
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnMapPan() 
{   
  // Switch off measure
   if (m_nMapMode == measure) OnMapMeasure();

   if (m_nMapMode == pan) m_nMapMode = none;
   else m_nMapMode = pan;	
}

void CViewMap::OnUpdateMapPan(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(m_pDoc->GetLayers()->GetSize() > 0); 
   pCmdUI->SetCheck(m_nMapMode == pan);	
	
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnMapMeasure() 
{
	if (m_nMapMode == measure) m_nMapMode = none;
   else m_nMapMode = measure;	

   // Clear initial points
   m_ptMeasure1 = CPoint(-1,-1);
   m_ptMeasure2 = CPoint(-1,-1);

   // Clear array of points
   m_aMeasure.RemoveAll();

   // Remove tape measure
   if (m_nMapMode == none)
   {
      Redraw();
   }	
}

void CViewMap::OnUpdateMapMeasure(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pDoc->GetLayers()->GetSize() > 0); 
   pCmdUI->SetCheck(m_nMapMode == measure);		
}

///////////////////////////////////////////////////////////////////////////////
//
// Saves the image, as printed, to a bitmap
//

void CViewMap::SaveImage(LPCSTR sFileName)
{    
    CDlgImageOptions dlg(CString(sFileName).Right(4).CompareNoCase(".jpg") == 0);
    if (dlg.DoModal() == IDOK)
    {
       int nWidth = dlg.GetWidth();
       int nHeight = dlg.GetHeight();

       CRect rect = CRect(0,0,nWidth, nHeight);    

       BeginWaitCursor();

       CDC dc;
	   CDC *pDC = GetDC();
       dc.CreateCompatibleDC(pDC); // Monochrome
       CBitmap bitmap;


       // Pass in Window DC to maintain colour depth
       bitmap.CreateCompatibleBitmap(pDC, nWidth, nHeight);
       CBitmap *pBitmapOld = dc.SelectObject(&bitmap);
       dc.FillSolidRect(&rect, RGB(255,255,255));

       // Initialise for printing

       m_rectP = rect;
       InitPrint(&dc);

       // Set the window extent

       dc.ScaleWindowExt(2,1,2,1);

       // Print to the device context

       dc.m_bPrinting = TRUE;
       DrawRect(&dc, m_rect);
       dc.m_bPrinting = FALSE;
 
       // Create an image file object from the device context       

       CImageFile image;
       if (image.CaptureDC(&dc, rect.Width(), rect.Height()))
       {
          // Save the image to file       
       
          if (!image.OnSave(sFileName,dlg.GetQuality(),dlg.IsColour()))
          {
             AfxMessageBox(IDS_ERRORWRITE);
          }
       
       }

       // Tidy up

       dc.SelectObject(&pBitmapOld);

       ReleaseDC(pDC);

       EndWaitCursor();
    };
    
    return;
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::DrawTapeMeasure(CDC* pDC)
{
   if (m_aMeasure.GetSize() > 0)
   {
      CPoint point1,point2;
      point1.x = GetfXPos(m_aMeasure[0].x);
      point1.y = GetfYPos(m_aMeasure[0].y);
      pDC->MoveTo(point1);

      for (int i = 1; i < m_aMeasure.GetSize(); i++)
      {       
          point2.x = GetfXPos(m_aMeasure[i].x);
          point2.y = GetfYPos(m_aMeasure[i].y);

          pDC->LineTo(point2);

          point1 = point2;
      }
   };
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   // Switch off measure
   
   if (m_nMapMode == measure) OnMapMeasure();
	
	CBDView::OnLButtonDblClk(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
//
// Manually update layers
//

void CViewMap::OnMapUpdate() 
{	
   // Redraw once
   m_bAutoUpdate = -1;
   Redraw();	   
}

void CViewMap::OnUpdateMapUpdate(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(!m_bAutoUpdate);	   
}

///////////////////////////////////////////////////////////////////////////////
//
// Zoom in / out on wheel
//

BOOL CViewMap::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
   if(zDelta > 0)
   {
   OnViewZoomout(); 
   }

   if(zDelta <0)
   {
   OnViewZoomin();
   }
  	
	return CBDView::OnMouseWheel(nFlags, zDelta, pt);
}

///////////////////////////////////////////////////////////////////////////////

void CViewMap::OnMapViewfile() 
{
	CAttrArray aAttr;
   BOOL bFound = FALSE;

   aAttr.m_lFType = m_feature.m_lFeatureTypeId;
   aAttr.m_lFeature = m_feature.m_lId;
   if (BDAttribute(BDHandle(), &aAttr, BDSELECT3))
   {
     // Cycle through all images associated with the feature as date is not stored with
     // map objects

     int i = 0; for (i = 0; i < aAttr.GetSize(); i++)
     {
        CAttribute* pAttr = aAttr.GetAt(i);
        if (pAttr->GetDataType() == BDHOTLINK)
        {
			  BDSetHtmlHelp(*pAttr->GetFile());			
           CMultiDocTemplate* pTemplate = BDGetDocTemplate("HtmlHelp");      
           pTemplate->OpenDocumentFile(NULL);   			   
			  bFound = TRUE;
        }        

        if (pAttr->GetDataType() == BDFILE)
        {
           // Write to a temporary file

           CFileObj fileobj;
           fileobj.Initialise(*pAttr->GetLongBinary());

           char sTempDir[MAX_PATH];
           GetTempPath(sizeof(sTempDir), sTempDir);
           CString sFile = CString(sTempDir) + fileobj.GetFileName();

           FILE* pFile = fopen(sFile, "wb");
           if (pFile != NULL)
           {
              fwrite(fileobj.GetData(), fileobj.GetSize(), 1, pFile);
              fclose(pFile);
           }

           // Open view to file and indicate that the file is temporary
           BDSetHtmlHelp(sFile, TRUE);			
           CMultiDocTemplate* pTemplate = BDGetDocTemplate("HtmlHelp");      
           pTemplate->OpenDocumentFile(NULL);   			   

           bFound = TRUE;


        }
     }
     if (!bFound)
     {
        AfxMessageBox(BDString(IDS_NOFILEFEATURE));
     }     
  }	
}

void CViewMap::OnUpdateMapViewfile(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(m_bViewFile || m_bViewHotLink);			
}

