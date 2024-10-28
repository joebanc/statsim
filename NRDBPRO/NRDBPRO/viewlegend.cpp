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
#include <strstream>

#include "nrdb.h"
#include "nrdbpro.h"
#include "ViewLegend.h"
#include "viewmap.h"
#include "projctns.h"
#include "sheetmapprop.h"
#include "comboboxpattern.h"
#include "comboboxsymbol.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#define RANGESIZE 5
#define COLUMNS 3
#define RANGECOLOR 10
#define SCROLLRANGE 1000

/////////////////////////////////////////////////////////////////////////////
// CViewLegend

IMPLEMENT_DYNCREATE(CViewLegend, CBDView)

CViewLegend::CViewLegend()
{   
   m_nDragLayer = null;
   m_hCursorKey = LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_DRAGLAYER));

   m_pDoc = NULL;
   
   m_nBottomY = 1;
}

CViewLegend::~CViewLegend()
{
   if (m_hCursorKey != NULL) DestroyCursor(m_hCursorKey);
}


BEGIN_MESSAGE_MAP(CViewLegend, CBDView)
	//{{AFX_MSG_MAP(CViewLegend)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_VIEW_ZOOMIN, OnViewZoomin)
	ON_COMMAND(ID_VIEW_ZOOMNORMAL, OnViewZoomnormal)
	ON_COMMAND(ID_VIEW_ZOOMOUT, OnViewZoomout)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_MAP_PAN, OnMapPan)
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CViewLegend diagnostics

#ifdef _DEBUG
void CViewLegend::AssertValid() const
{
	CBDView::AssertValid();
}

void CViewLegend::Dump(CDumpContext& dc) const
{
	CBDView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CViewLegend message handlers

void CViewLegend::OnInitialUpdate() 
{
	CBDView::OnInitialUpdate();
	
	m_pDoc = (CDocMap*)GetDocument();	

   // Allow scrolling 

   ShowScrollBar(SB_VERT);
   SetScrollRange(SB_VERT,0,SCROLLRANGE);
	
}

/////////////////////////////////////////////////////////////////////////////
// CViewLegend drawing

void CViewLegend::OnDraw(CDC* pDC)
{
	CFont *pFontOld;
   int cyold = 0;
   m_bCancel = FALSE;

   if (m_pDoc == NULL) return;   

   m_nColumn = 1;
   m_nCols = 1;

   m_anLayerPos.RemoveAll();   

   // Determine properties

   LOGFONT lf = m_pDoc->GetLayers()->GetFontL();      
   ScaleFont(pDC, &lf);
   CFont font;
   font.CreateFontIndirect(&lf);      
   pFontOld = pDC->SelectObject(&font);   
   CSize sz = pDC->GetTextExtent("W"); 
   pDC->SelectObject(pFontOld);
   
   m_nBorder = sz.cy;
   m_nTextPos = (m_nBorder*3);

   // If editing mode then display instructions

   if (GetDoc()->GetViewMap()->m_pMapLinesEdit != NULL ||
       GetDoc()->GetViewMap()->m_pMapCoordEdit != NULL ||
       GetDoc()->GetViewMap()->m_bDragMode )
   {
      CFont font;
	   LOGFONT lf = m_pDoc->GetLayers()->GetFontL();      
      font.CreateFontIndirect(&lf);      
      pFontOld = pDC->SelectObject(&font);
      
      int nString = IDS_EDITMAPINST;
      if (GetDoc()->GetViewMap()->m_bDragMode) nString = IDS_POINTSEDITINST;
      if (GetDoc()->GetViewMap()->m_pMapCoordEdit != NULL) nString = IDS_ADDPOINTINST;      
      pDC->DrawText(BDString(nString), m_rect, DT_LEFT|DT_WORDBREAK|DT_TOP|DT_NOPREFIX);
      
      pDC->SelectObject(pFontOld);
      return;
   }

   // Set text mode to transparent

   pDC->SetBkMode(TRANSPARENT);

   if (pDC->IsPrinting())
   {      
      if (BDGetApp()->GetLayout().IsAuto())
      {
         if (GetDoc()->GetViewMap()->m_nWidth < GetDoc()->GetViewMap()->m_nHeight)
         {
            m_rect = GetDoc()->GetViewMap()->GetRectPrint();         
            m_rect.left += m_nBorder;
            m_rect.right = m_rect.left + m_rect.Width() - m_nBorder/2;         
            m_rect.top = GetDoc()->GetViewMap()->GetRect().bottom + sz.cy*4;
         } else
         {
            m_rect = GetDoc()->GetViewMap()->GetRectPrint();
            m_rect.left = GetDoc()->GetViewMap()->GetRect().right;
            m_rect.right -= m_nBorder;
            m_rect.top = GetDoc()->GetViewMap()->GetRect().top;
         }; 
      
         // Make room for the grid labels

         if (m_pDoc->GetLayers()->GetMapGrid().m_nType != CMapGrid::none)
         {
            m_rect.left += m_nBorder * 2;
         };
      } else
      {
        // Determine the position for the legend from the layout

         CMapLayoutObj layoutobj = GetDoc()->GetLayers()->GetMapLayout().GetLayoutObj(CMapLayout::legend);
         CRect rectLegend = layoutobj.m_rect;   

         if (pDC->IsPrinting() && rectLegend.Width() != 0)
         {         
            m_rect = CMapLayout::RectFromPercent(GetDoc()->GetViewMap()->GetRectPrint(), rectLegend);
            //cy = m_rect.top;
         }
      }
   } else
   {
      GetClientRect(m_rect);      
   }         

   // Set top allow for scrolling

   int nPos = GetScrollPos(SB_VERT);
   int nViewY = (m_nBottomY * nPos)/SCROLLRANGE;

   int cy = m_rect.top + m_nBorder - nViewY;

   // Draw title 
      
      if (m_pDoc->GetLayers()->GetSize() > 0)
      {

         // Not portrait
         if (!pDC->IsPrinting() || GetDoc()->GetViewMap()->m_nWidth > GetDoc()->GetViewMap()->m_nHeight)
         {
           cy -= m_nBorder;
         }

         CRect rect;

         // Default title position

         if (BDGetApp()->GetLayout().IsAuto() || !pDC->IsPrinting())
         {
            rect.left = m_rect.left + m_nBorder;
            rect.right = m_rect.right - m_nBorder;
            rect.top = cy;
            rect.bottom = rect.top + sz.cy*8;
         }
         
         // Custom title position

         else
         {
            CMapLayoutObj layoutobj = GetDoc()->GetLayers()->GetMapLayout().GetLayoutObj(CMapLayout::title);
            CRect rectT = layoutobj.m_rect;
                  
            if (rectT.Width() > 0 && pDC->IsPrinting())
            {
               rect = CMapLayout::RectFromPercent(GetDoc()->GetViewMap()->GetRectPrint(), rectT);
            }
         };

         int ny = cy;
         ny += GetDoc()->GetViewMap()->DrawTitle(pDC, &rect);
         ny += m_nBorder;

         if (!pDC->IsPrinting() || BDGetApp()->GetLayout().IsAuto()) cy = ny;
      };
      
   // Store layer positions

   m_anLayerPos.Add(cy + nViewY);

// Determine the number of columns

   if (pDC->IsPrinting())
   {
	   CFont font;
	   LOGFONT lf = m_pDoc->GetLayers()->GetFontL();
      ScaleFont(pDC, &lf);            
      font.CreateFontIndirect(&lf);      
      pFontOld = pDC->SelectObject(&font);
	   CSize sz = pDC->GetTextExtent("abc");
	   pDC->SelectObject(pFontOld);

		m_nCols = max(1,m_rect.Width() / ((sz.cx * 30) / 3));
      m_rect.right = m_rect.left + m_rect.Width() / m_nCols;
   }

   // For each layer, draw its key

   int i = 0; for (i = 0; i < m_pDoc->GetLayers()->GetSize() && !m_bCancel; i++)
   {
      CMapLayer* pMapLayer = m_pDoc->GetLayers()->GetAt(i);      
      CMapStyle mapstyle;

      CViewMap::GetStyle(pMapLayer, NULL, 0, mapstyle);

      if (pMapLayer->IsVisible())
      {
         cyold = cy;
      
         // Create font
      
         CFont font;      
         LOGFONT lf = pMapLayer->GetFont();

         if (m_pDoc->GetLayers()->GetLegend1Font())
         {
            lf = m_pDoc->GetLayers()->GetFontL();
         }
         ScaleFont(pDC, &lf);            
         font.CreateFontIndirect(&lf);      
         pFontOld = pDC->SelectObject(&font);        

         // Determine height of text

            CSize sz;         
            sz = pDC->GetTextExtent(pMapLayer->GetName());                
         
            m_nHeight = sz.cy;

         // Display objects within the layer

         BOOL bCoord = FALSE;
         BOOL bMapLines = FALSE;
         BOOL bImage = FALSE;
      
         // Determine type of objects selected

         int j = 0; for (j = 0; j < pMapLayer->GetSize(); j++)
         {
            CMapLayerObj* pMapLayerObj = pMapLayer->GetAt(j);               
            if (pMapLayerObj->GetDataType() == BDMAPLINES) {bMapLines = TRUE; break;}
            else if (pMapLayerObj->GetDataType() == BDCOORD) {bCoord = TRUE; break;}
            else if (pMapLayerObj->GetDataType() == BDIMAGE) {bImage = TRUE; break;}
         };

         // Draw the legend even if this is overidden

         if ((pMapLayer->GetSepColour() || pMapLayer->GetRangeColour() ||
              pMapLayer->GetAutoColour() || pMapLayer->GetAutoSize()) && 
              pMapLayer->IsShowLayerName())
         {
             DrawText(pDC, m_rect.left + m_nBorder, cy, pMapLayer->GetName());   
         }

         // Draw appropriate legend

         if (pMapLayer->GetSepColour())
         {
            if (!m_pDoc->GetLayers()->GetLegend1Font())
            {
               pDC->SetTextColor(pMapLayer->GetTextColour());
            };

            for (int k = 0; k < pMapLayer->GetMapProp().m_aColourFeature.GetSize(); k++)
            {
               CColourFeature &rColourFeature = pMapLayer->GetMapProp().m_aColourFeature.GetAt(k);

               CMapStyle mapstyle = rColourFeature;
               CString s = rColourFeature.m_sFeature;
               CString s1 = rColourFeature.m_sAttr;              

               s.TrimRight();

               if (pMapLayer->GetSepColour() == CMapLayer::LegendValues)
               {
                  DrawLegendValues(pDC, pMapLayer, cy, s + "\n" + s1, mapstyle);
               } else
               {
			      if (s == "") s = " "; // Ensure space

               if (bMapLines) 
			      {
				      if (!DrawMapLineLegend(pDC, pMapLayer, cy, s,  mapstyle)) break;
			      }

               if (bCoord)
			      {
				      if (!DrawCoordLegend(pDC, pMapLayer, cy, s, mapstyle)) break;
			      }
               };
            }
         }

         // Range color

         else if (pMapLayer->GetRangeColour())
         {
            // Draw title

            if (!m_pDoc->GetLayers()->GetLegend1Font())
            {
               pDC->SetTextColor(pMapLayer->GetTextColour());
            };            

            CColourRangeArray& aRangeColour = pMapLayer->GetMapProp().m_aRangeColour;
            for (int k = 0; k < aRangeColour.GetSize(); k++)
            {
               CString s = aRangeColour.GetDesc(k);
               
               CMapStyle mapstyle = aRangeColour[k];

               if (bMapLines) DrawMapLineLegend(pDC, pMapLayer, cy, s, mapstyle);
               if (bCoord) DrawCoordLegend(pDC, pMapLayer, cy, s, mapstyle);
            }
         }         
       
         // Draw auto color

         else if (pMapLayer->GetAutoColour() || pMapLayer->GetAutoSize())
         {
            if (pMapLayer->GetAutoSize())
            {
               DrawAutoSize(pDC, pMapLayer, cy);            
            } else
            {
               DrawAutoColour(pDC, pMapLayer, cy, mapstyle);
            }
            DrawAutoScale(pDC, pMapLayer, cy);
         }
         else if (pMapLayer->IsShowLayerName())
         {
            // If no style then just draw layer name and line or symbol

            if (bMapLines) DrawMapLineLegend(pDC, pMapLayer, cy, NULL, mapstyle);
            if (bCoord) DrawCoordLegend(pDC, pMapLayer, cy, NULL, mapstyle);
         };

         // If no lines then display a symbol

         if (pMapLayer->GetSize() == 0 || bImage)
         {            
            DrawCoordLegend(pDC, pMapLayer, cy, NULL, mapstyle);
         }

         // Draw border around active layer      

         if (!pDC->IsPrinting() && !m_bCopy &&
            i == m_pDoc->GetLayers()->GetDefault())
         {         
            CRect rect = m_rect;
            rect.top = cyold;
            rect.bottom = cy;
            DrawBorder(pDC, rect);         
         }

         pDC->SelectObject(pFontOld);           

      };

     // Store layer positions (include invisible layers)

      m_anLayerPos.Add(cy + nViewY);       
   }

  
   // Draw the projection (unless already off edge of screen in automatic layout)

   if (m_pDoc->GetLayers()->GetSize() > 0 && (!m_bCancel || (!BDGetApp()->GetLayout().IsAuto() && pDC->IsPrinting())))
   {
      DrawProjection(pDC, cy);

      // Scale e.g. 1:50,000

      DrawScale(pDC, cy);

      // Scale bar

      DrawScaleBar(pDC, cy);

      DrawComments(pDC, cy);
   };        

   // Draw the north arrow

   if (m_pDoc->GetLayers()->GetSize() > 0)
   {
      DrawNorthArrow(pDC, cy);
   };   

   // If portrait, draw a box around the legend

   if (pDC->IsPrinting())     
   {
      CMapLayoutObj layoutobj = GetDoc()->GetLayers()->GetMapLayout().GetLayoutObj(CMapLayout::legend);
      CRect rectLegend = layoutobj.m_rect;

      if (rectLegend.Width() == 0)
      {
         if (GetDoc()->GetViewMap()->m_nWidth < GetDoc()->GetViewMap()->m_nHeight)   
         {
            CRect rect;
            rect.left = GetDoc()->GetViewMap()->GetRectPrint().left + m_nBorder;
            rect.right = GetDoc()->GetViewMap()->GetRectPrint().right - m_nBorder;
            rect.top = m_rect.top;
            rect.bottom = m_rect.bottom - m_nBorder;
            DrawBorder(pDC, rect, PS_SOLID, 1, RGB(0,0,0));
         }
      }
      
   }

   // Draw box around key if has focus

   if (GetFocus() == this && !m_bCopy && !pDC->IsPrinting())
   {
      CRect rect = m_rect;
      rect.right--;
      rect.bottom--;
      DrawBorder(pDC, rect);
   }   

   // Store new bottom position for scrollbar

   m_nBottomY = cy + nViewY;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CViewLegend::DrawMapLineLegend(CDC* pDC, CMapLayer* pMapLayer, int& cy, 
                                    LPCSTR s, CMapStyle mapstyle)
{

   CPen pen(mapstyle.m_nLineStyle, CViewMap::GetLineWidth(pDC,mapstyle.m_nLineStyle, mapstyle.m_nLineWidth), mapstyle.m_crLine);
   CPen* pPenOld = pDC->SelectObject(&pen);

   // Create brush

   CBrush brush;
   CComboBoxPattern::CreateBrush(brush, mapstyle.m_nPattern, mapstyle.m_nHatch, mapstyle.m_crFill);   
   CBrush* pBrushOld = pDC->SelectObject(&brush);
   
   // Output line or polygon

   int cx = m_rect.left + m_nBorder/2;

   if (!pMapLayer->GetAutoColour())
   {
      // Draw filled polygon
      if (pMapLayer->IsPolygon())
      {
         POINT aPoints[4];
         aPoints[0].x = cx + m_nBorder;
         aPoints[0].y = cy + m_nHeight/4;
         aPoints[1].x = cx + m_nBorder;
         aPoints[1].y = cy + (m_nHeight*3)/4;
         aPoints[2].x = cx + m_nTextPos-m_nBorder;
         aPoints[2].y = cy + (m_nHeight*3)/4;
         aPoints[3].x = cx + m_nTextPos-m_nBorder;
         aPoints[3].y = cy + m_nHeight/4;         
         pDC->Polygon(aPoints, sizeof(aPoints)/sizeof(POINT));
      } 
      // Draw sample line
      else
      {
         pDC->MoveTo(cx + m_nBorder, cy + m_nHeight/2);
         pDC->LineTo(cx + m_nTextPos-m_nBorder, cy + m_nHeight/2);
      }
   };

   // Set text color and draw the text

   if (!m_pDoc->GetLayers()->GetLegend1Font())
   {
      pDC->SetTextColor(pMapLayer->GetTextColour());
   };
   if (s == NULL) s = pMapLayer->GetName();   

   if (!pMapLayer->GetAutoColour()) cx += m_nTextPos;
   else cx += m_nBorder;

   BOOL bOK = DrawText(pDC, cx, cy, s);   

   pDC->SelectObject(pPenOld);
   pDC->SelectObject(pBrushOld);

   return bOK;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CViewLegend::DrawCoordLegend(CDC* pDC, CMapLayer* pMapLayer, int& cy, 
                                  LPCSTR s, CMapStyle mapstyle)
{            
   // Draw symbol

   int cx = m_rect.left + m_nBorder/2;

   CSize sz = CComboBoxSymbol::GetSymSize(pDC, pMapLayer->GetSymSize());   

   if (pMapLayer->GetSize() != 0)
   {
   

      if (!pMapLayer->GetAutoSize())
      {   
         CComboBoxSymbol::DrawSymbol(pDC, cx + m_nTextPos/2, cy + max(sz.cy, m_nHeight/2), mapstyle);         
      };
   };

   // Set text color

   if (!m_pDoc->GetLayers()->GetLegend1Font())
   {
      pDC->SetTextColor(pMapLayer->GetTextColour());
   };

   // Draw text

   if (s == NULL) s = pMapLayer->GetName();

   if (pMapLayer->GetSymbol() == CComboBoxSymbol::none) cx += m_nBorder;
   else  cx += m_nTextPos;   

   int cyOld = cy;
   BOOL bRet =  DrawText(pDC, cx, cy, s);   

   // Make room for custom symbols

   if (cyOld + sz.cy*2 > cy) cy = cyOld + sz.cy*2;

   return bRet;
}

/////////////////////////////////////////////////////////////////////////////

void CViewLegend::DrawLegendValues(CDC* pDC, CMapLayer* pMapLayer, int& cy, CString s, CMapStyle mapstyle)
{
   int cx = m_rect.left;

   // Draw value

   CString s1 = s.Left(s.Find('\n'));
   CString s2 = s.Mid(s.Find('\n')+1);
   
   int cyt = cy;   

   // Draw Legend Values
      
   
   DrawText(pDC, cx + m_nBorder, cyt, s1, MAP_UPDATEY);   

   CSize sz = pDC->GetTextExtent(s2);
   DrawText(pDC, m_rect.right - m_nBorder - sz.cx, cy, s2);   

   // Handles case where no legend values!
   cy = max(cy, cyt);
}

/////////////////////////////////////////////////////////////////////////////
//
// Draws a scale indicating the range of values in the auto color scale
//

void CViewLegend::DrawAutoColour(CDC* pDC, CMapLayer* pMapLayer, int& cy, CMapStyle mapstyle)
{
   CRect rectA;   
     
   int y = cy + m_nHeight/2;
   int cx = m_rect.left;

   // Draw the lines
   
   int nPenWidth = m_nBorder/2;

   int i = 0; for (i = 0; i < RANGECOLOR; i++)
   {
      // Select pen of the appropriate color

      double d = pMapLayer->GetAutoMin() + 
                 (pMapLayer->GetAutoMax()-pMapLayer->GetAutoMin())*i/RANGECOLOR;

      CViewMap::GetStyle(pMapLayer, NULL, d, mapstyle);            

     CPen pen(mapstyle.m_nLineStyle, CViewMap::GetLineWidth(pDC,mapstyle.m_nLineStyle, mapstyle.m_nLineWidth), mapstyle.m_crLine);
     CPen* pPenOld = pDC->SelectObject(&pen);

      CBrush brush;
      CComboBoxPattern::CreateBrush(brush, mapstyle.m_nPattern, mapstyle.m_nHatch, mapstyle.m_crFill);      

      int x1 = cx + m_nBorder + (m_rect.Width()-m_nBorder-m_nBorder)*i/RANGECOLOR;
      int x2 = cx + m_nBorder + (m_rect.Width()-m_nBorder-m_nBorder)*(i+1)/RANGECOLOR;

      CRect rect;
      rect.left = x1;
      rect.right = x2;
      rect.top = y-nPenWidth/2;
      rect.bottom = y+nPenWidth/2;

      pDC->FillRect(&rect, &brush);      

      // Draw bounding lines

    if (i == 0) 
    {
       pDC->MoveTo(x1, y-nPenWidth/2);
       pDC->LineTo(x1, y+nPenWidth/2);
    } else if (i+1 == RANGECOLOR)
    {
       pDC->MoveTo(x2, y-nPenWidth/2);
       pDC->LineTo(x2, y+nPenWidth/2);
    }

    pDC->MoveTo(x1, y-nPenWidth/2);
    pDC->LineTo(x2, y-nPenWidth/2);
    pDC->MoveTo(x1, y+nPenWidth/2);
    pDC->LineTo(x2, y+nPenWidth/2);

   pDC->SelectObject(pPenOld);


   }

   cy += m_nHeight;      
}

/////////////////////////////////////////////////////////////////////////////

void CViewLegend::DrawAutoSize(CDC* pDC, CMapLayer* pMapLayer, int& cy)
{      
   int nPenWidth = 1;
   int crPen = RGB(0,0,0);
   int nPenStyle = PS_SOLID;
   int nPattern = BS_SOLID;
   int nHatch = 0;

   // Determine the maximum height of the symbols

   CSize sz = CComboBoxSymbol::GetSymSize(pDC, pMapLayer->GetSymSize() * AUTOSCALE);         

   cy += sz.cy;   
   
   int i = 0; for (i = 0; i <= RANGESIZE; i++)
   {
      // Determine color of symbol

      double d = pMapLayer->GetAutoMin() + 
                 (pMapLayer->GetAutoMax()-pMapLayer->GetAutoMin())*i/RANGESIZE;

      CMapStyle mapstyle;
      CViewMap::GetStyle(pMapLayer, NULL, d, mapstyle);            

      // Determine position

      int x = m_rect.left + m_nBorder + (m_rect.Width()-m_nTextPos-m_nBorder)*i/RANGESIZE;

      mapstyle.m_dSymSize = (int)(((pMapLayer->GetSymSize() *AUTOSCALE)*i)/RANGESIZE);

      CComboBoxSymbol::DrawSymbol(pDC, x, cy, mapstyle);
   };

   cy += sz.cy;   
}

/////////////////////////////////////////////////////////////////////////////

void CViewLegend::DrawAutoScale(CDC* pDC, CMapLayer* pMapLayer, int& cy)
{
   CSize sz(0,20);       
   std::strstream str1, str2;   

   str1.precision(10);
   str2.precision(10);

   double dMin = pMapLayer->GetAutoMin();
   double dMax = pMapLayer->GetAutoMax();
   
   if (dMin < dMax)
   {
	   str1 << dMin << std::ends;   
	   str2 << dMax << std::ends;
   } else
   {           
	   if ((int)dMin % 10 == 1 && (int)dMin % 100 != 11) str1 << dMin << "st" << std::ends;
	   else if ((int)dMin % 10 == 2 && (int)dMin % 100 != 12) str1 << dMin << "nd" << std::ends;
	   else str1 << dMin << "th" << std::ends;

	  if ((int)dMax % 10 == 1 && (int)dMax % 100 != 11) str2 << dMax << "st" << std::ends;
	  else if ((int)dMax % 10 == 2 && (int)dMax % 100 != 12) str2 << dMax << "nd" << std::ends;
	  else str2 << dMax << "th" << std::ends;      
   }
   
   sz = pDC->GetTextExtent(str2.str());         
            	
   pDC->TextOut(m_rect.left + m_nBorder - sz.cy/2, cy, str1.str());                         	
   pDC->TextOut(m_rect.right-m_nBorder-sz.cx, cy, str2.str());                         	

	str1.rdbuf()->freeze(0);
   str2.rdbuf()->freeze(0);
   
   cy += sz.cy;   
}

/////////////////////////////////////////////////////////////////////////////

BOOL CViewLegend::DrawText(CDC* pDC, int cx, int& cy, LPCSTR sText,  int nFlags)
{
   CRect rect = m_rect;   
   rect.left = cx;
   rect.right = m_rect.right;
   rect.top = cy;      
   int y = pDC->DrawText(sText, &rect, DT_LEFT|DT_WORDBREAK|DT_TOP|DT_NOPREFIX|DT_NOCLIP);         

   if (nFlags & MAP_UPDATEY) cy += y;

  // If reached the bottom of the page and portrait then shift to the next column

   if (cy > m_rect.bottom - m_nBorder*2 && nFlags & MAP_UPDATEY)
   {
	  if (m_nColumn < m_nCols)
	  {
		  if (pDC->IsPrinting())   
		  {
			 m_nColumn++;			 

          int nWidth = m_rect.Width();
          m_rect.left = m_rect.right + m_nBorder;
          m_rect.right = m_rect.left + nWidth - m_nBorder;
          
          cy = m_rect.top;

          CMapLayoutObj layoutobj = GetDoc()->GetLayers()->GetMapLayout().GetLayoutObj(CMapLayout::map);
          if (BDGetApp()->GetLayout().IsAuto()) cy += m_nBorder*2;
		  } 
	  } else
	  {
         m_bCancel = TRUE;

         if (pDC->IsPrinting())
         {
            return FALSE;
         };
	  }
   };
   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

void CViewLegend::OnEditCopy() 
{
	CBDView::OnEditCopy();	
}

/////////////////////////////////////////////////////////////////////////////

void CViewLegend::DrawProjection(CDC* pDC, int& cy)
{   
   CFont font;

// Initialise layout

   CMapLayoutObj layoutobj = GetDoc()->GetLayers()->GetMapLayout().GetLayoutObj(CMapLayout::projection);
   CRect rectComments = layoutobj.m_rect;   
   if (!BDGetApp()->GetLayout().IsAuto() && pDC->IsPrinting())
   {      
      m_rect = CMapLayout::RectFromPercent(GetDoc()->GetViewMap()->GetRectPrint(), rectComments);
      cy = m_rect.top;
   }

   CString s = BDProjection()->GetProjectionName();
   CRect rect;   
   rect = m_rect;

   // Set the font

   if (rect.Width() > 0)
   {   
      CreateFont(pDC, font, -9);      
      CFont* pFontOld = pDC->SelectObject(&font);           

      pDC->SetTextColor(RGB(0,0,0));

      cy += m_nHeight;
 
      // If outputing to screen or rectangle, draw to a rectangle

      DrawText(pDC, m_rect.left + m_nBorder, cy, s);   

      // Tidy up

      pDC->SelectObject(pFontOld);        
   };
}

///////////////////////////////////////////////////////////////////////////////

void CViewLegend::DrawScale(CDC* pDC, int& cy)
{

   CFont font;  
   CRect rect;
   
// Initialise layout

   CMapLayoutObj layoutobj = GetDoc()->GetLayers()->GetMapLayout().GetLayoutObj(CMapLayout::scale);
   CRect rectComments = layoutobj.m_rect;   
   if (!BDGetApp()->GetLayout().IsAuto() && pDC->IsPrinting())
   {      
      m_rect = CMapLayout::RectFromPercent(GetDoc()->GetViewMap()->GetRectPrint(), rectComments);
      cy = m_rect.top;
   }

   rect = m_rect;

   if (rect.Width() > 0)
   {
      // Set the font

      CreateFont(pDC, font, -9);
      CFont* pFontOld = pDC->SelectObject(&font);           

      pDC->SetTextColor(RGB(0,0,0));

      cy += m_nHeight;
 
      // If outputing to screen or rectangle, draw to a rectangle
      // If the scale is almost the set value then use this

      int nScale = GetDoc()->GetViewMap()->GetScale(pDC);
      if (abs(nScale - GetDoc()->GetScale() < 10)) nScale = GetDoc()->GetScale();
      CString s;
      s.Format("%s 1:%i", (LPCSTR)BDString(IDS_SCALE), nScale);   
      DrawText(pDC, m_rect.left + m_nBorder, cy, s);   

      // Tidy up

      pDC->SelectObject(pFontOld);        
   };
}

///////////////////////////////////////////////////////////////////////////////
//
// Draw the scale bar
//

void CViewLegend::DrawScaleBar(CDC* pDC, int& cy)
{
   CRect rect;
   CString s;
   //BOOL bKM = FALSE;
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
   pDC->SetTextColor(RGB(0,0,0));
   CSize sz = pDC->GetTextExtent("ABC");

   // Draw the scale bar

    CBrush brush(RGB(0,0,0));

// Determine position of scale bar

    CRect rectS = m_rect;    
    rectS.left += m_nBorder;
    rectS.right = rectS.left + (rectS.Width() * 4)/5;
    rectS.top = cy;
    rectS.bottom = cy + (sz.cy*3)/2;
    
// Determine the layout position for the scalebar

   CMapLayoutObj layoutobj = GetDoc()->GetLayers()->GetMapLayout().GetLayoutObj(CMapLayout::scalebar);
   CRect rectScaleBar = layoutobj.m_rect;

   if (pDC->IsPrinting() && !BDGetApp()->GetLayout().IsAuto())
   {         
      rectS = CMapLayout::RectFromPercent(GetDoc()->GetViewMap()->GetRectPrint(), rectScaleBar);

      // Adjust to prevent fat scale bars
      rectS.bottom = rectS.top + (sz.cy*3)/2;
   }
 
   if (rectS.Width() > 0)
   {
     // Determine the scale range in meters
     // For lat/lon, based on bottom of map so assume linear
   
      CLongCoord coord1, coord2;

      coord1.x = GetDoc()->GetViewMap()->GetfXInv(rectS.left);   
      coord1.y = GetDoc()->GetViewMap()->GetfYInv(rectS.bottom);    

      coord2.x = GetDoc()->GetViewMap()->GetfXInv(rectS.right);   
      coord2.y = GetDoc()->GetViewMap()->GetfYInv(rectS.bottom);   

      int nDist = (int)BDProjection()->GetDistance(coord1, coord2);
      int nRange = nDist;

      int nScale = 1;

      // Round the scale down to a value between 5 and 10

      while (nRange > 50)
      {
         nRange /= 10;
         nScale *= 10;
      }
      while (nRange > 5)
      {
         nRange /= 2;
         nScale *= 2;
      }

      // Remove text height from scale bar height

      rectS.top += sz.cy;

      // Convert the range back up, but rounded

       int nScaleRange = nRange*nScale;
       if (nScaleRange == 0) return;

       int nScaleDiv = nScaleRange / nScale;
       //bKM = nScaleRange / nScaleDiv > 1000;
    
       int i = 0; for (i = 0; i <= nScaleDiv; i ++)
       {
          // Determine position of bar

          rect = rectS;
          rect.left = rectS.left + (int)((((rectS.right-rectS.left) * i) * (double)nScaleRange) / (nDist)) / nScaleDiv;
          rect.right = rectS.left + (int)((((rectS.right-rectS.left) * (i+1)) * (double)nScaleRange) / (nDist)) / nScaleDiv;
       
          if (i < nScaleDiv)
          {
             // Draw rectange

             if (i % 2)
             {
                pDC->FillRect(&rect, &brush);
             } else
             {
                GetDoc()->GetViewMap()->DrawRect(pDC, &rect, RGB(0,0,0));
             };

             // Draw text

             int n = (nScaleRange * i) / nScaleDiv;
             //if (bKM) n = n / 1000;
             s.Format("%i",n);
             rect.bottom = rect.top;
             rect.top = rect.bottom - sz.cy;
             pDC->DrawText(s, &rect, DT_BOTTOM|DT_LEFT|DT_SINGLELINE); 
          } else
          {
             // Display units         
    
             //s = BDString(IDS_METERS);
             //if (bKM) s = BDString(IDS_KM); 
             s = BDGetApp()->GetUnits().GetLengthUnit().m_sAbbr;
             rect.bottom = rect.top;
             rect.top = rect.bottom - sz.cy;
             pDC->DrawText(s, &rect, DT_BOTTOM|DT_LEFT|DT_SINGLELINE); 
          }
       }
   
       // Tidy up

      pDC->SelectObject(pFontOld);        

      // Update position

      cy += rectS.Height();
   };
}

///////////////////////////////////////////////////////////////////////////////

void CViewLegend::DrawComments(CDC* pDC, int& cy)
{
   LOGFONT lf;   
    
   // Set the font
     
   pDC->SetTextColor(RGB(0,0,0));  

   cy += m_nHeight;

   CMapLayoutObj layoutobj = GetDoc()->GetLayers()->GetMapLayout().GetLayoutObj(CMapLayout::source);
   CRect rectLegend = layoutobj.m_rect;   
   if (pDC->IsPrinting() && !BDGetApp()->GetLayout().IsAuto())
   {         
      m_rect = CMapLayout::RectFromPercent(GetDoc()->GetViewMap()->GetRectPrint(), rectLegend);
      cy = m_rect.top;
   }
 
   // If outputing to screen or rectangle, draw to a rectangle

   if (m_rect.Width() > 0)
   {
      CString s;

      int i = 0; for (i = 0; i < m_pDoc->GetLayers()->GetSize(); i++)
      {         
         CFont font;
         lf = m_pDoc->GetLayers()->GetAt(i)->GetFontC();

         ScaleFont(pDC, &lf);         
         font.CreateFontIndirect(&lf);  
         CFont* pFontOld = pDC->SelectObject(&font);           
         DrawText(pDC, m_rect.left + m_nBorder, cy, m_pDoc->GetLayers()->GetAt(i)->GetComment());   

         pDC->SelectObject(pFontOld);        
     
      }  
   };
        
}

///////////////////////////////////////////////////////////////////////////////

void CViewLegend::DrawNorthArrow(CDC* pDC, int& cy)
{
   CRect rect = m_rect;

   CPen pen(PS_SOLID, 2, RGB(0,0,0));
   CPen* pPenOld = pDC->SelectObject(&pen);   
   int cx = 0;

// Determine the default position for the title

   CMapLayoutObj layoutobj = GetDoc()->GetLayers()->GetMapLayout().GetLayoutObj(CMapLayout::arrow);
   CRect& rectNorthArrow = layoutobj.m_rect;

   if (pDC->IsPrinting() && !BDGetApp()->GetLayout().IsAuto())
   {         
      m_rect = CMapLayout::RectFromPercent(GetDoc()->GetViewMap()->GetRectPrint(), rectNorthArrow);
      rect = m_rect;
      cy = m_rect.top;
   }
      
   // Ensure space if portrait

   if (m_rect.Width() > 0)
   {
      int cy1 = cy;
      int nColumn = m_nColumn;
      int i = 0; for (i = 0; i < 4; i++) 
      {
         DrawText(pDC, cx, cy1, " ");
         if (m_nColumn != nColumn) 
         {
            cy = cy1;
            break;
         }
      };      
      cx = (rect.right + rect.left)/2;         
      if (cy + m_nBorder* 5 > GetDoc()->GetViewMap()->GetRectPrint().bottom) return;
 
      // Draw Arrow
   
      pDC->MoveTo(cx, cy);
      pDC->LineTo(cx-m_nBorder/2, cy+m_nBorder*2);
      pDC->LineTo(cx, cy+(m_nBorder*3)/2);
      pDC->LineTo(cx+m_nBorder/2, cy+m_nBorder*2);
      pDC->LineTo(cx, cy);
      cy += m_nBorder*2;

      // Draw N

      pDC->MoveTo(cx+m_nBorder/4, cy);
      pDC->LineTo(cx+m_nBorder/4, cy+m_nBorder);
      pDC->LineTo(cx-m_nBorder/4, cy);
      pDC->LineTo(cx-m_nBorder/4, cy+m_nBorder);
      cy += m_nBorder*2;

      pDC->SelectObject(pPenOld);        
   };   
}



///////////////////////////////////////////////////////////////////////////////

void CViewLegend::OnSize(UINT nType, int cx, int cy) 
{
	CBDView::OnSize(nType, cx, cy);
}

///////////////////////////////////////////////////////////////////////////////

void CViewLegend::DrawBorder(CDC* pDC, CRect rect, int nPen, int nWidth, int nColour)
{
   CPen pen(nPen, nWidth, nColour);
   CPen* pPenOld = pDC->SelectObject(&pen);
   pDC->MoveTo(rect.left, rect.bottom); 
   pDC->LineTo(rect.right, rect.bottom);
   pDC->LineTo(rect.right, rect.top);
   pDC->LineTo(rect.left, rect.top);
   pDC->LineTo(rect.left, rect.bottom);
   pDC->SelectObject(pPenOld);
}

///////////////////////////////////////////////////////////////////////////////

void CViewLegend::OnSetFocus(CWnd* pOldWnd) 
{
	CBDView::OnSetFocus(pOldWnd);
	
	Invalidate(TRUE);
   UpdateWindow();  
	
}

void CViewLegend::OnKillFocus(CWnd* pNewWnd) 
{
	CBDView::OnKillFocus(pNewWnd);
	
	Invalidate(TRUE);
   UpdateWindow();  
	
}

///////////////////////////////////////////////////////////////////////////////
//
// Override functions so that they can be accessed when the key window has
// input focus
//

void CViewLegend::OnFilePrintPreview() 
{
   m_pDoc->GetViewMap()->OnFilePrintPreview();
}

void CViewLegend::OnFilePrint() 
{
	m_pDoc->GetViewMap()->OnFilePrint();
}

///////////////////////////////////////////////////////////////////////////////

void CViewLegend::OnViewZoomin() 
{
	m_pDoc->GetViewMap()->OnViewZoomin();	
}

void CViewLegend::OnViewZoomnormal() 
{
	m_pDoc->GetViewMap()->OnViewZoomnormal();	
}

void CViewLegend::OnViewZoomout() 
{
   m_pDoc->GetViewMap()->OnViewZoomout();		
}

void CViewLegend::OnMapPan() 
{
	m_pDoc->GetViewMap()->OnMapPan();	
}

///////////////////////////////////////////////////////////////////////////////
//
// Update the current layer according to which point was clicked on
//

void CViewLegend::OnLButtonDown(UINT nFlags, CPoint point) 
{
   int nLayer = GetLayer(point.y);

   if (nLayer >= 0)
   {
      // Activate the drag layer mode

      m_nDragLayer = nLayer;

      m_hCursorOld = SetCursor(m_hCursorKey);

     // Set the active layer if changed and redraw

	   if (nLayer != m_pDoc->GetLayers()->GetDefault())
      {
         m_pDoc->GetLayers()->SetDefault(nLayer);      

         Invalidate(TRUE);
         UpdateWindow();  
      }
   };
	
	
	CBDView::OnLButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////

void CViewLegend::OnLButtonUp(UINT nFlags, CPoint point) 
{
   int nLayer = GetLayer(point.y);

   if (nLayer == start) nLayer = 0;
   if (nLayer == end) nLayer = m_pDoc->GetLayers()->GetSize()-1;

   // Swap layers

   if (nLayer != m_nDragLayer && m_nDragLayer >= 0)
   {            
      CMapLayer* pMapLayer = (CMapLayer*)m_pDoc->GetLayers()->GetAt(m_nDragLayer);
      
      // Shift layers up

	  int i = 0;      
      for (i = m_nDragLayer; i < nLayer; i++)
      {
         m_pDoc->GetLayers()->SetAt(i, m_pDoc->GetLayers()->GetAt(i+1));
      }            

      // Shift layers down

      for (i = m_nDragLayer; i > nLayer; i--)
      {
         m_pDoc->GetLayers()->SetAt(i, m_pDoc->GetLayers()->GetAt(i-1));
      }             

      // Set this layer the default

      m_pDoc->GetLayers()->SetAt(nLayer, pMapLayer);
      m_pDoc->GetLayers()->SetDefault(nLayer); 

      // Redraw the windows, key first

      Invalidate(TRUE);
      UpdateWindow();  
      m_pDoc->UpdateAllViews(this);   

      
   }
 
  // Reset cursor

   if (m_nDragLayer >= 0)
   {
      SetCursor(m_hCursorOld);
   }

   // Reset drag item

   m_nDragLayer = null;   

	CBDView::OnLButtonUp(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////

void CViewLegend::OnMouseMove(UINT nFlags, CPoint point) 
{   
	if (m_nDragLayer != null)
   {
      SetCursor(m_hCursorKey);
   } 
	
	CBDView::OnMouseMove(nFlags, point);
}


///////////////////////////////////////////////////////////////////////////////
//
// Display the properties dialog for the current layer
//

void CViewLegend::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	int nLayer = GetLayer(point.y);
	if (nLayer >= 0)
   {
      m_pDoc->GetLayers()->SetDefault(nLayer);
      CMapLayer* pMapLayer = m_pDoc->GetLayers()->GetAt(nLayer);

      CSheetMapProp dlg(m_pDoc->GetLayers(), pMapLayer, BDString(IDS_MAPPROP));
      if (dlg.DoModal())
      {
         m_pDoc->UpdateAllViews(NULL);   
      } else
      {
         Invalidate(TRUE);
         UpdateWindow();  
      }
   }
		
	CBDView::OnLButtonDblClk(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
//
// Determine which layer has been clicked on
//

int CViewLegend::GetLayer(int cy)
{
   int nPos = GetScrollPos(SB_VERT);
   int nViewY = (m_nBottomY * nPos)/SCROLLRANGE;

   int i = 0; for (i = 0; i < m_anLayerPos.GetSize()-1; i++)
   {
      if (cy + nViewY >= m_anLayerPos[i] && cy + nViewY <= m_anLayerPos[i+1])
      {
         return i;
      }
   }

   if (m_anLayerPos.GetSize() == 0) return null;
   else if (cy + nViewY < m_anLayerPos[0]) return start;

   ASSERT (cy + nViewY > m_anLayerPos[m_anLayerPos.GetSize()-1]);
   
   return end;   
}

///////////////////////////////////////////////////////////////////////////////
// 
// If right button is pressed, display map layer dialog
//

void CViewLegend::OnRButtonDown(UINT nFlags, CPoint point) 
{
   int nLayer = GetLayer(point.y);
	if (nLayer >= 0)
   {
      if (nLayer != m_pDoc->GetLayers()->GetDefault())
      {
         m_pDoc->GetLayers()->SetDefault(nLayer);

         Invalidate(TRUE);
         UpdateWindow();  
      };      
   }
   m_pDoc->OnViewLayers();
	      	
	CBDView::OnRButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////

void CViewLegend::CreateFont(CDC* pDC, CFont& font, int nHeight)
{
   LOGFONT lf;
   memset(&lf,0,sizeof(LOGFONT));
   lf.lfHeight = nHeight;
   lf.lfPitchAndFamily = 34;
   lf.lfCharSet = NRDB_CHARSET;
   strcpy(lf.lfFaceName, BDString(IDS_DEFAULTFONT));      
   ScaleFont(pDC, &lf);         
   font.CreateFontIndirect(&lf);  
}

///////////////////////////////////////////////////////////////////////////////
//
// Allow scrolling down
//

void CViewLegend::OnVScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar) 
{
   CRect rect;
   GetClientRect(&rect);

   int nPos = uPos;
   if (nSBCode != SB_THUMBPOSITION)
   {
      nPos = GetScrollPos(SB_VERT);
   };

   switch(nSBCode)
   {   
      case SB_LINEDOWN:      
         nPos += SCROLLRANGE/10;
      break;            

      case SB_LINEUP:
         nPos -= SCROLLRANGE/10;
      break;
            
      case SB_PAGEDOWN:        
         nPos += SCROLLRANGE/2;
      break;            
      
      case SB_PAGEUP:                            
         nPos -= SCROLLRANGE/10;
      break;         
     
      case SB_THUMBPOSITION:
         // Skip default
      break; 
  
      default:
         CBDView::OnVScroll(nSBCode, nPos, pScrollBar);
         return;
   }
   
   // Redraw the map
        
    nPos = min(SCROLLRANGE, nPos);
    nPos = max(0, nPos);
    uPos = nPos;

    SetScrollPos(SB_VERT, nPos);
    Invalidate();
    
	
	 CBDView::OnVScroll(nSBCode, uPos, pScrollBar);
}
