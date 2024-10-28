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
#include "nrdbpro.h"
#include "ViewMapLayout.h"
#include "sheetmapstyle.h"
#include "dlglayouttext.h"
#include "docmap.h"
#include "viewmap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#define HANDLESIZE 4
#define SNAPSIZE 50

/////////////////////////////////////////////////////////////////////////////
// CViewMapLayout

IMPLEMENT_DYNCREATE(CViewMapLayout, CView)

CViewMapLayout::CViewMapLayout()
{  
   // Get the aspect ration of the printer

   BeginWaitCursor();
   DEVMODE devmode = {0};
   BDGetPrinterInfo(&devmode);
   EndWaitCursor();

   m_nHeight = m_nWidth = 0;

   // Create the temporary device context

   PRINTDLG   pd;
   pd.lStructSize = (DWORD) sizeof(PRINTDLG);
   if (AfxGetApp()->GetPrinterDeviceDefaults(&pd))
   {
      HDC hDC = AfxCreateDC(pd.hDevNames, pd.hDevMode);
      if (hDC != NULL) 
      {     
         CDC dc;
         dc.Attach(hDC);

         m_nWidth = dc.GetDeviceCaps(HORZRES);
         m_nHeight = dc.GetDeviceCaps(VERTRES);                          
      };
   };

   m_pRectTracker = NULL;

   m_rect = CRect(0,0,0,0);
   m_rect = CRect(0,0,0,0);

   // Currently selected objects

   m_pLayoutObj = NULL;

   m_bOverlayMap = FALSE;
}

///////////////////////////////////////////////////////////////////////////////

void CViewMapLayout::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

}


///////////////////////////////////////////////////////////////////////////////

CViewMapLayout::~CViewMapLayout()
{
      if (m_pRectTracker != NULL) delete m_pRectTracker;
}


BEGIN_MESSAGE_MAP(CViewMapLayout, CView)
	//{{AFX_MSG_MAP(CViewMapLayout)
	ON_WM_SIZE()
	ON_COMMAND(ID_LAYOUT_MAP, OnLayoutMap)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_LAYOUT_ARROW, OnLayoutArrow)
	ON_COMMAND(ID_LAYOUT_LEGEND, OnLayoutLegend)
	ON_COMMAND(ID_LAYOUT_PROJECTION, OnLayoutProjection)
	ON_COMMAND(ID_LAYOUT_SCALEBAR, OnLayoutScalebar)
	ON_COMMAND(ID_LAYOUT_TITLE, OnLayoutTitle)
	ON_COMMAND(ID_LAYOUT_SELECT, OnLayoutSelect)
	ON_COMMAND(ID_LAYOUT_BORDER, OnLayoutBorder)
	ON_COMMAND(ID_LAYOUT_SOURCE, OnLayoutSource)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_LAYOUT_PICTURE, OnLayoutPicture)
	ON_COMMAND(ID_LAYOUT_TEXT, OnLayoutText)
	ON_COMMAND(ID_LAYOUT_SCALE, OnLayoutScale)
	ON_COMMAND(ID_LAYOUT_PREVIEW, OnLayoutPreview)
	ON_UPDATE_COMMAND_UI(ID_LAYOUT_PREVIEW, OnUpdateLayoutPreview)
	ON_WM_SETCURSOR()
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)
	ON_COMMAND(ID_MAP_LOCATOR, OnMapLocator)
	//}}AFX_MSG_MAP
   ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewMapLayout drawing

void CViewMapLayout::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();

   // If print preview then display the default map

   if (pDC->IsPrinting())
   {
      CViewMap* pViewMap = BDGetDocMap()->GetViewMap();
      if (pViewMap != NULL) pViewMap->OnDraw(pDC);
      return;
   };
   
      // No default printer
      
   if (m_nWidth == 0 || m_nHeight == 0) 
   {
	   pDC->TextOut(0,0,BDString(IDS_NOPRINTER));
	   return;
   };
   
   // Draw visible area

   pDC->FillSolidRect(&m_rect, RGB(255,255,255));
   
   // Draw the overlay map

   if (m_bOverlayMap && BDGetDocMap() != NULL)
   {
      CViewMap* pViewMap = BDGetDocMap()->GetViewMap();

      pViewMap->SetRectPrint(m_rect);
      pViewMap->InitPrint(pDC);
      pDC->m_bPrinting = TRUE;
      pViewMap->DrawRect(pDC, pViewMap->GetRect());
      pDC->m_bPrinting = FALSE;
   };

   // Create font

   LOGFONT lf;
   CFont font;
   memset(&lf,0,sizeof(LOGFONT));
   lf.lfHeight = -10;
   lf.lfPitchAndFamily = 12;   
   lf.lfCharSet = NRDB_CHARSET;
   strcpy(lf.lfFaceName, BDString(IDS_DEFAULTFONT));         
   font.CreateFontIndirect(&lf);  
   CFont* pFontOld = pDC->SelectObject(&font); 

   pDC->SetBkMode(TRANSPARENT);          
   
   // Draw edges after drawing preview map

   CRect rect;
   GetClientRect(&rect);
   rect.right = m_rect.left;
   pDC->FillSolidRect(&rect, RGB(192,192,192));

   GetClientRect(&rect);
   rect.left = m_rect.right;
   pDC->FillSolidRect(&rect, RGB(192,192,192));

   // Draw snap points

   int ix = m_rect.Width() / (m_nWidth/SNAPSIZE);
   int iy = m_rect.Height() / (m_nHeight/SNAPSIZE);

   if (ix >= 2 && iy >= 2)
   {
	   for (int i = m_rect.left; i < m_rect.right; i += ix)
	   {
		  for (int j = m_rect.top; j < m_rect.bottom; j += iy)
		  {
			 pDC->SetPixel(i, j, RGB(192,192,192));
		  }
	   }
   };

   // Draw the position of the boxes

   int i = 0; for (i = 0; i < GetLayout()->m_aLayout.GetSize(); i++)
   {
      CMapLayoutObj& layoutobj = GetLayout()->m_aLayout[i];
      CRect rect = layoutobj.m_rect;
      if (rect.Width() != 0 && rect.Height() != 0)
      {
         COLORREF cr = RGB(0,0,128);
         if (layoutobj.m_nType == CMapLayout::box) cr = layoutobj.m_style.m_crLine;

         CPen pen(layoutobj.m_style.m_nLineStyle, layoutobj.m_style.m_nLineWidth, cr);
         CPen *pPenOld = pDC->SelectObject(&pen);

         CRect rectO = CMapLayout::RectFromPercent(m_rect, rect);
         pDC->MoveTo(rectO.left, rectO.top);
         pDC->LineTo(rectO.left, rectO.bottom);
         pDC->LineTo(rectO.right, rectO.bottom);
         pDC->LineTo(rectO.right, rectO.top);
         pDC->LineTo(rectO.left, rectO.top);

         // Now draw the type of the object

         if (!m_bOverlayMap)
         {
            CString s = layoutobj.GetTypeDesc();        

            CRect rectT = rectO;
            rectT.left += 5;
            rectT.top += 5;
            rectT.right -= 5;
            rectT.bottom -= 5;

            rectT.top += pDC->DrawText(s, rectT, DT_LEFT);

            // Draw the image name (if any)

            rectT.top += pDC->DrawText(layoutobj.m_sImageFile, rectT, DT_LEFT);

            // Draw text

            rectT.top += pDC->DrawText(layoutobj.m_sText, rectT, DT_LEFT);
         };
                 
         // Tidy up

         pDC->SelectObject(pPenOld);
        
      }
   }

   // Draw tracker

   if (m_pRectTracker != NULL) m_pRectTracker->Draw(pDC);

   // Tidy up

   pDC->SelectObject(pFontOld);           

}

/////////////////////////////////////////////////////////////////////////////
// CViewMapLayout diagnostics

#ifdef _DEBUG
void CViewMapLayout::AssertValid() const
{
	CView::AssertValid();
}

void CViewMapLayout::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CViewMapLayout message handlers

void CViewMapLayout::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	CRect rect;
   GetClientRect(rect);

   // Adjust the aspect ratio of the rectangle to the printer

   m_rect = rect;  
   int nWidth = m_rect.Width();
   int nHeight = m_rect.Height();      

   if (m_nHeight == 0 || m_nWidth == 0) return;
   

	if ((double)m_nHeight / nHeight > (double)m_nWidth / nWidth)
	{  
      nWidth = (nHeight * m_nWidth) / m_nHeight;             
	} else
	{		   
      nHeight = (nWidth * m_nHeight) / m_nWidth;                  
	}
   m_rect.left = rect.left + (rect.Width() - nWidth) / 2;
   m_rect.right = m_rect.left + nWidth;
   m_rect.top = rect.top + (rect.Height() - nHeight) /2;      
   m_rect.bottom = m_rect.top + nHeight;

   // Recreate the tracker

   CreateTracker();
}

///////////////////////////////////////////////////////////////////////////////

void CViewMapLayout::CreateTracker()
{
   if (m_pLayoutObj == NULL) return;
  
	// Create a tracker to allow the position of the item to be changed

   if (m_pRectTracker != NULL) delete m_pRectTracker;
   m_pRectTracker = NULL;

   // Initialise

   CRect rect = m_pLayoutObj->m_rect;
   if (rect.Width() == 0 && rect.Height() == 0)
   {
      rect.left = 400;
      rect.right = 600;
      rect.top = 400;
      rect.bottom = 600;

      m_pLayoutObj->m_rect = rect;

   }
         
   // Adjust rect
      
   CRect rectT = CMapLayout::RectFromPercent(m_rect, rect);
                  
   m_pRectTracker = new CRectTracker(rectT, CRectTracker::dottedLine | CRectTracker::resizeOutside);
   m_pRectTracker->m_sizeMin = CSize(1,1);
   m_pRectTracker->m_nHandleSize = HANDLESIZE;   
   
   Invalidate(FALSE);

}

///////////////////////////////////////////////////////////////////////////////

void CViewMapLayout::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_nWidth == 0 || m_nHeight == 0) return;

   // Determine which item is being moved

   if (m_pRectTracker != NULL && m_pLayoutObj != NULL)
   {            
      POINT pt = point;
	   ClientToScreen(&pt);
	   ScreenToClient(&pt);

      // Allow sizing at corners only to maintain aspect ratio

      int nHitTest = m_pRectTracker->HitTest(pt);
      if (nHitTest >= 0)
      {
         m_pRectTracker->Track(this, pt, FALSE, NULL);

         // Get the new tracker position and convert it back to a percentage
           
           CRect rectT;
      
           m_pRectTracker->GetTrueRect(&rectT);      
           rectT.top += HANDLESIZE;
           rectT.bottom -= HANDLESIZE;
           rectT.left += HANDLESIZE;
           rectT.right -= HANDLESIZE;

           // Snap to grid

           int ix = m_rect.Width() / (m_nWidth/SNAPSIZE);
           int iy = m_rect.Height() / (m_nHeight/SNAPSIZE);
           rectT.left = (rectT.left + ix/2)/ ix * ix;
           rectT.right = (rectT.right + ix/2) / ix * ix;
           rectT.top = (rectT.top + iy/2) / iy * iy;
           rectT.bottom = (rectT.bottom + iy/2) / iy * iy;

           // Convert to percent
           
           CRect rect = CMapLayout::RectAsPercent(m_rect, rectT);                      

           // Adjust to edges

           int nWidth = rect.Width();
           int nHeight = rect.Height();

           rect.left = max(rect.left, 0);
           rect.right = min(rect.right, 1000);           
           rect.top = max(rect.top, 0);
           rect.bottom = min(rect.bottom, 1000);                     

           rect.left = min(rect.left, rect.right);
           rect.right = max(rect.right, rect.left);
           rect.top = min(rect.top, rect.bottom);
           rect.bottom = max(rect.bottom, rect.top);           

           // Save the position back

           m_pLayoutObj->m_rect = rect;
                                              
           Invalidate(FALSE);

           // Check for changes
           
           CreateTracker();
           
           return;
      }
   }
      
   // If outside the track area then check if another component of the map has 
   // been clicked on

   CMapLayoutObj *pLayoutObj = HitTest(point);
            
   // Set the new component

   if (pLayoutObj != NULL)
   {
      m_pLayoutObj = pLayoutObj;
      CreateTracker();            
   } else
   {
      OnLayoutSelect();
   }
	
	CView::OnLButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
//
// Determines which rectangle the point lies within
//

CMapLayoutObj* CViewMapLayout::HitTest(CPoint pt)
{
   int iRect = -1;
   int iSize = 0;

   // Determine if user has clicked in any of the rectangles bounding
   // the components of the map

   int i = 0; for (i = 0; i < GetLayout()->m_aLayout.GetSize(); i++)
   {
      // Convert the rectangle to screen coordinates

      CRect rectP = GetLayout()->m_aLayout[i].m_rect;
      CRect rect = CMapLayout::RectFromPercent(m_rect, rectP);

      if (rect.left <= pt.x && pt.x <= rect.right && 
          rect.top <= pt.y && pt.y <= rect.bottom)
      {               
         // Smaller rectangles take priority

         if (iRect == -1 || rect.Height() * rect.Width() < iSize)
         {
            iRect = i;
            iSize = rect.Height() * rect.Width();
         }               
      }
   }

   if (iRect != -1)
   {
      return &GetLayout()->m_aLayout[iRect];
   };
   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void CViewMapLayout::OnLayoutMap() 
{   
   CreateLayoutObj(CMapLayout::map);	
}

void CViewMapLayout::OnLayoutArrow() 
{	
   CreateLayoutObj(CMapLayout::arrow);	
}

void CViewMapLayout::OnLayoutLegend() 
{	
	CreateLayoutObj(CMapLayout::legend);
}

void CViewMapLayout::OnLayoutProjection() 
{	
   CreateLayoutObj(CMapLayout::projection);
}

void CViewMapLayout::OnLayoutScalebar() 
{	
   CreateLayoutObj(CMapLayout::scalebar);
}

void CViewMapLayout::OnLayoutTitle() 
{   
   CreateLayoutObj(CMapLayout::title);
}

void CViewMapLayout::OnLayoutSource() 
{
   CreateLayoutObj(CMapLayout::source);
}

void CViewMapLayout::OnLayoutScale() 
{
	CreateLayoutObj(CMapLayout::scale);
}

void CViewMapLayout::OnMapLocator() 
{
   CreateLayoutObj(CMapLayout::locator);
	
}
///////////////////////////////////////////////////////////////////////////////
 
void CViewMapLayout::CreateLayoutObj(int nType)
{
   m_pLayoutObj = GetLayout()->GetLayoutObjPtr(nType);
   if (m_pLayoutObj == NULL)
   {
      CMapLayoutObj layoutobj;
      layoutobj.m_nType = nType;
      int index = GetLayout()->m_aLayout.Add(layoutobj);
      m_pLayoutObj = GetLayout()->m_aLayout.GetData() + index;
   }
   CreateTracker();			
}

///////////////////////////////////////////////////////////////////////////////

void CViewMapLayout::CreateLayoutObjMult(int nType)
{
   // Add a new object of type box

   CMapLayoutObj layoutobj;
   layoutobj.m_nType = nType;

   int index = GetLayout()->m_aLayout.Add(layoutobj);

   m_pLayoutObj = GetLayout()->m_aLayout.GetData() + index;
   CreateTracker();	
}

///////////////////////////////////////////////////////////////////////////////
//
// Reset selection
//

void CViewMapLayout::OnLayoutSelect() 
{
   m_pLayoutObj = NULL;   

   if (m_pRectTracker != NULL) delete m_pRectTracker;
   m_pRectTracker = NULL;

   Invalidate(FALSE);
	
}

///////////////////////////////////////////////////////////////////////////////

void CViewMapLayout::OnLayoutBorder() 
{  
   CreateLayoutObjMult(CMapLayout::box);

   // Set the default box style to transparent

   m_pLayoutObj->m_style.m_nPattern = BS_NULL;
   m_pLayoutObj->m_style.m_nHatch = 0;
}

///////////////////////////////////////////////////////////////////////////////

void CViewMapLayout::OnLayoutPicture() 
{
	CreateLayoutObjMult(CMapLayout::picture);
	
}

///////////////////////////////////////////////////////////////////////////////

void CViewMapLayout::OnLayoutText() 
{
	CreateLayoutObjMult(CMapLayout::text);	
}

///////////////////////////////////////////////////////////////////////////////
//
// Support deleting of objects
//

void CViewMapLayout::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_DELETE)
   {
      if (m_pLayoutObj != NULL)
      {
         int i = 0; for (i = 0; i < GetLayout()->m_aLayout.GetSize(); i++)
         {
            if (GetLayout()->m_aLayout.GetData() + i == m_pLayoutObj)
            {
               GetLayout()->m_aLayout.RemoveAt(i);
               break;
            }
         }
         m_pLayoutObj = NULL;
         Invalidate(FALSE);

         if (m_pRectTracker != NULL) delete m_pRectTracker;
         m_pRectTracker = NULL;
      }      
   }
	
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

///////////////////////////////////////////////////////////////////////////////
//
// Display properties of the layout object
//

void CViewMapLayout::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
  // Determine which layout object is selected
 
   CMapLayoutObj *pLayoutObj = HitTest(point);
   if (pLayoutObj != NULL)
   {
      // Edit the style for a box

      if (pLayoutObj->m_nType == CMapLayout::box)
      {
         // Create maplayer

         CMapLayer maplayer;
         CMapLayerObj* pMapObj = new CMapLayerObj;
         pMapObj->SetDataType(BDMAPLINES);
         maplayer.Add(pMapObj);

         // Create map property

         CMapProperties mapprop;
         (CMapStyle&)mapprop = pLayoutObj->m_style;
         mapprop.m_bPolygon = TRUE;         

         // Create dialog

         CSheetMapStyle dlg(&maplayer, &mapprop, BDString(IDS_STYLE));            
         if (dlg.DoModal() == IDOK)
         {  
            pLayoutObj->m_style = mapprop;            
            Invalidate(FALSE);
         }
      }
      // Image

      else if (pLayoutObj->m_nType == CMapLayout::picture)
      {
         CString sFilter;
         CImageFile::GetOpenFilterString(sFilter);
   
         CFileDialog dlg(TRUE,"",NULL,OFN_FILEMUSTEXIST, sFilter);
         if (dlg.DoModal() == IDOK)
         {
            pLayoutObj->m_sImageFile = dlg.GetPathName();
            Invalidate(FALSE);
         }
      }

      // Text

      else if (pLayoutObj->m_nType == CMapLayout::text)
      {
         CDlgLayoutText dlg(pLayoutObj->m_sText, pLayoutObj->m_logfont);
         if (dlg.DoModal() == IDOK)
         {
            pLayoutObj->m_sText = dlg.GetString();
            pLayoutObj->m_logfont = dlg.GetFont();

         }
      }
   }
	
	CView::OnLButtonDblClk(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
//
// Toggles preview map mode
//

void CViewMapLayout::OnLayoutPreview() 
{   
   m_bOverlayMap = !m_bOverlayMap;   

   Invalidate(FALSE);
}

///////////////////////////////////////////////////////////////////////////////

void CViewMapLayout::OnUpdateLayoutPreview(CCmdUI* pCmdUI) 
{
   pCmdUI->SetCheck(m_bOverlayMap);
	
}

///////////////////////////////////////////////////////////////////////////////

BOOL CViewMapLayout::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
   if (m_pRectTracker != NULL)
   {
	   if (m_pRectTracker->SetCursor(pWnd, nHitTest))
      {
         return TRUE;
      }
   };
	
	return CView::OnSetCursor(pWnd, nHitTest, message);
}

///////////////////////////////////////////////////////////////////////////////

BOOL CViewMapLayout::OnPreparePrinting(CPrintInfo* pInfo) 
{
	// default preparation
	return DoPreparePrinting(pInfo);   
}

///////////////////////////////////////////////////////////////////////////////

void CViewMapLayout::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
   // Initialise the printing of the first map view   

   CViewMap* pViewMap = BDGetDocMap()->GetViewMap();
   if (pViewMap != NULL)
   {
      pViewMap->OnPrint(pDC, pInfo);
   };
	
	//CView::OnPrint(pDC, pInfo);
}

///////////////////////////////////////////////////////////////////////////////

void CViewMapLayout::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
   // Reset the default map

   CViewMap* pViewMap = BDGetDocMap()->GetViewMap();
   if (pViewMap != NULL) pViewMap->OnEndPrinting(pDC, pInfo);
	
	CView::OnEndPrinting(pDC, pInfo);
}

///////////////////////////////////////////////////////////////////////////////

void CViewMapLayout::OnUpdateFilePrintPreview(CCmdUI* pCmdUI) 
{   
   CDocMap* pDocMap = BDGetDocMap();
	pCmdUI->Enable(pDocMap != NULL && pDocMap->GetViewMap() != NULL);	
}

