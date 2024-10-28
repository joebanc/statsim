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
#include "ViewGraph.h"
#include "mainfrm.h"
#include "dlggraphprop.h"
#include "docmap.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#define MMPERINCH 25.4

/////////////////////////////////////////////////////////////////////////////

inline int square(int x) {return x*x;}

/////////////////////////////////////////////////////////////////////////////
// CViewGraph

IMPLEMENT_DYNCREATE(CViewGraph, CBDView)

CViewGraph::CViewGraph()
{
   m_bData = FALSE;      
}

CViewGraph::~CViewGraph()
{
}


BEGIN_MESSAGE_MAP(CViewGraph, CBDView)
	//{{AFX_MSG_MAP(CViewGraph)
	ON_WM_CREATE()
	ON_WM_SIZE()	
   ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)	
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_GRAPH_PROP, OnGraphProp)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
   ON_COMMAND(ID_FILE_PRINT, CBDView::OnFilePrint)
   ON_COMMAND(ID_FILE_PRINT_DIRECT, CBDView::OnFilePrint)
   ON_COMMAND(ID_FILE_PRINT_PREVIEW, CBDView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewGraph diagnostics

#ifdef _DEBUG
void CViewGraph::AssertValid() const
{
	CBDView::AssertValid();
}

void CViewGraph::Dump(CDumpContext& dc) const
{
	CBDView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

CGraphProperties::CGraphProperties()
{
   m_dXMax = m_dXMin = 0;
   m_dYMax = m_dYMin = 0;
   m_nYPoints = 0;
   memset(&m_logfont,0,sizeof(LOGFONT));
   m_logfont.lfPitchAndFamily = 18;   
   m_logfont.lfHeight = 12;
   m_logfont.lfCharSet = NRDB_CHARSET;
   strcpy(m_logfont.lfFaceName, BDString(IDS_DEFAULTFONT));      
}


CGraphProperties& CGraphProperties::operator=(CGraphProperties& rSrc)
{
   m_sTitle = rSrc.m_sTitle;
   m_aLegendText.Copy(rSrc.m_aLegendText);
   m_aColour.Copy(rSrc.m_aColour);
   m_aLineWidth.Copy(rSrc.m_aLineWidth);
   m_aLineStyle.Copy(rSrc.m_aLineStyle);
   m_nType = rSrc.m_nType;
   m_aGraphStyle.Copy(rSrc.m_aGraphStyle);
   m_logfont = rSrc.m_logfont;
   m_dXMin = rSrc.m_dXMin;
   m_dXMax = rSrc.m_dXMax;
   m_dYMin = rSrc.m_dYMin;
   m_dYMax = rSrc.m_dYMax;
   m_nYPoints = rSrc.m_nYPoints;

   return *this;
}

/////////////////////////////////////////////////////////////////////////////

int CViewGraph::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBDView::OnCreate(lpCreateStruct) == -1)
		return -1;

   // Retrieve the document pointer

   m_pDoc = (CDocGraph*)GetDocument();

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CViewGraph message handlers

/////////////////////////////////////////////////////////////////////////////

void CViewGraph::OnInitialUpdate() 
{  
	CBDView::OnInitialUpdate();  	

   m_prop.m_nType = m_pDoc->m_nType;
}


/////////////////////////////////////////////////////////////////////////////

void CViewGraph::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
   CString sLabel;
   double dValue;    
   m_bData = FALSE;

   BeginWaitCursor();
   
  // Retrieve data and store in arrays
      
   m_aGraphData.SetSize(m_pDoc->GetNumLines());

   int iLine = 0;
   for (iLine = 0; iLine < m_pDoc->GetNumLines(); iLine++)
   {     
      int iPoint = 0;
      BOOL bFound = m_pDoc->GetFirstData(iLine, dValue, sLabel);
      while (bFound)
      {
         m_bData = TRUE;

         if (!IsNullDouble(dValue))
         {
	         m_aGraphData[iLine].Add(dValue);		   
         } else
         {
            m_aGraphData[iLine].Add(0);
         }

         m_aLabelText.SetAtGrow(iPoint++, sLabel);                        
      
		   bFound = m_pDoc->GetNextData(iLine, dValue, sLabel);
	   }                    
   };

   for (iLine = 0; iLine < GetNumLegend(); iLine++)
   {
      m_prop.m_aLegendText.SetAtGrow(iLine, GetLegend(iLine));
      m_prop.m_aColour.SetAtGrow(iLine, GetColour(iLine));
      m_prop.m_aLineStyle.SetAtGrow(iLine, PS_SOLID);
      m_prop.m_aLineWidth.SetAtGrow(iLine, 2);
      m_prop.m_aGraphStyle.SetAtGrow(iLine, CGraphProperties::lines);
   }
   		  
	// Set titles         
   
   m_prop.m_sTitle = m_pDoc->GetTitle();   
        
   EndWaitCursor();
}

/////////////////////////////////////////////////////////////////////////////
// CViewGraph drawing

void CViewGraph::OnDraw(CDC* pDC)
{
   CFont font, *pFontOld;	

   if (!pDC->IsPrinting())
   {
      GetClientRect(&m_rect);
   };

   // Text mode

   pDC->SetBkMode(TRANSPARENT);

   // If no data do not draw

   if (!m_bData)
   {
      CreateFont(pDC, font, 20);
      pFontOld = pDC->SelectObject(&font);
      pDC->DrawText(m_prop.m_sTitle + "\r\n" + BDString(IDS_NODATA), &m_rect, DT_CENTER|DT_NOPREFIX);
   }  else
   {

      // Determine position of axis

      CreateFont(pDC, font, 12);
      pFontOld = pDC->SelectObject(&font);
      CSize sz = pDC->GetTextExtent("123");
   
      m_rectAxis.left = min(sz.cx/3*10, m_rect.Width()/6);
      m_rectAxis.right = (m_rect.right * 3) / 4;
      m_rectAxis.top = min(max(sz.cy*2, m_rect.Height()/8), m_rect.Height()/4);
      m_rectAxis.bottom = m_rect.bottom - min(max(sz.cy*2, m_rect.Height()/8), m_rect.Height()/4);

      // Determine scale

      if (m_prop.m_dYMax == m_prop.m_dYMin)
      {
         DetermineScale();
      }

      // Create pen

      CPen pen(PS_SOLID, GetWidth(pDC, PS_SOLID, 1), RGB(0,0,0));
      CPen *pPenOld = pDC->SelectObject(&pen);
      
      // Draw axis

      DrawAxis(pDC);
      DrawXAxis(pDC);
      DrawYAxis(pDC);

      // Draw the graph

      DrawGraph(pDC);

      // Draw the title

      DrawTitle(pDC);

      // Draw legend

      DrawLegend(pDC);

      // Tidy up
  
      pDC->SelectObject(pPenOld);
   }
   pDC->SelectObject(pFontOld);
}

/////////////////////////////////////////////////////////////////////////////

void CViewGraph::DetermineScale()
{
   double dYMin = DBL_MAX;
   double dYMax = -DBL_MAX;

   int i = 0; for (i = 0; i < m_aGraphData.GetSize(); i++)
   {
      int j = 0; for (j = 0; j < m_aGraphData[i].GetSize(); j++)
      {
         double dValue = m_aGraphData[i][j];
         if (!IsNullDouble(dValue))
         {
            dYMin = min(dYMin, dValue);
            dYMax = max(dYMax, dValue);
         }
      }
   }

   if (dYMin != DBL_MAX)
   {
      CDocMap::DetermineScale(dYMin, dYMax, &m_prop.m_dYMin, &m_prop.m_dYMax, &m_prop.m_nYPoints);
   } else
   {
      m_prop.m_dYMin = 0;
      m_prop.m_dYMax = 1;
      m_prop.m_nYPoints = 5;
   }
}

/////////////////////////////////////////////////////////////////////////////

void CViewGraph::DrawAxis(CDC* pDC)
{
   pDC->MoveTo(m_rectAxis.left, m_rectAxis.bottom);
   pDC->LineTo(m_rectAxis.right, m_rectAxis.bottom);
   pDC->LineTo(m_rectAxis.right, m_rectAxis.top);
   pDC->LineTo(m_rectAxis.left, m_rectAxis.top);
   pDC->LineTo(m_rectAxis.left, m_rectAxis.bottom);
}

/////////////////////////////////////////////////////////////////////////////

void CViewGraph::DrawXAxis(CDC* pDC)
{
   CFont font;   
   BOOL bVert = m_aLabelText.GetSize() > 8;
   
   // Draw axis

   pDC->MoveTo(m_rectAxis.left, m_rectAxis.bottom);
   pDC->LineTo(m_rectAxis.right, m_rectAxis.bottom);
   
   // Create font

   int nFont = max(6,min(12,((12 * 8) / (m_aLabelText.GetSize()+2))));

   // Scale font to fit   
         
   CSize sz;
   int nMax;
   do 
   {
      nMax = 0;
      nFont--;
      CFont font;
      CreateFont(pDC, font, nFont);   
      CFont* pFontOld = pDC->SelectObject(&font);           
   
      int i = 0; for (i = 0; i < m_aLabelText.GetSize(); i++)
      {
         sz = pDC->GetTextExtent(m_aLabelText[i]);
         if (bVert) nMax = max(sz.cy, nMax);
         else nMax = max(sz.cx, nMax);         
      }               
      
      pDC->SelectObject(pFontOld);      
   } while (nMax > m_rectAxis.Width()/m_aLabelText.GetSize() && nFont > 2);   

   // Create font
         
   CreateFont(pDC, font, nFont, bVert);   
   CFont* pFontOld = pDC->SelectObject(&font);        
   sz = pDC->GetTextExtent("123");

   // Determine if labels should be vertical
      
   int nWidth = m_rectAxis.Width() / (m_aLabelText.GetSize()+2);
      
   // Draw labels

   int i = 0; for (i = 0; i < m_aLabelText.GetSize(); i++)
   {
      int nX = m_rectAxis.left + ((i+1)*m_rectAxis.Width())/(m_aLabelText.GetSize()+1);

      
      CRect rect;
      rect.top = m_rectAxis.bottom + sz.cy;
      rect.bottom = m_rect.bottom;
      rect.left = nX - nWidth/2;
      rect.right = nX + nWidth/2;
 
      if (bVert)
      {
         pDC->TextOut(nX, m_rectAxis.bottom+1, m_aLabelText[i]);
      } else
      {
         pDC->DrawText(m_aLabelText[i], &rect, DT_CENTER | DT_NOPREFIX | DT_WORDBREAK | DT_TOP);
      }      
   }

   // Tidy up

   pDC->SelectObject(pFontOld);        
}

/////////////////////////////////////////////////////////////////////////////

void CViewGraph::DrawYAxis(CDC* pDC)
{
   CRect rect;
   CFont font;
   
   pDC->SetBkMode(TRANSPARENT);       

   // Determine font size

   int nSize = 11;
   ScaleFont(pDC, nSize, "123", m_rectAxis.left/3, m_rectAxis.Height());   

   // Create font
      
   CreateFont(pDC, font, nSize);   
   CFont* pFontOld = pDC->SelectObject(&font);        
   CSize sz = pDC->GetTextExtent("123");

   // Label axis

   double d = m_prop.m_dYMin;
   int i = 0; for (i = 0; i <= m_prop.m_nYPoints; i++)
   {
      // Determine text

	   std::strstream sValue;                                                          
      sValue.precision(10);
	  sValue << d << std::ends;

      // Determine position of label

      int nY = m_rectAxis.bottom + ((m_rectAxis.top - m_rectAxis.bottom) * 
                  i) / m_prop.m_nYPoints;
      rect.left = 0;
      rect.right = m_rectAxis.left - sz.cx/3;
      rect.top = nY - sz.cy;
      rect.bottom = nY + sz.cy;
      
      pDC->DrawText(sValue.str(),&rect, DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX);
      sValue.rdbuf()->freeze(0);      

      // Draw label tick

      pDC->MoveTo(m_rectAxis.left, nY);
      pDC->LineTo(m_rectAxis.left-sz.cx/6, nY);
      
      // Determine next value

      d += (m_prop.m_dYMax - m_prop.m_dYMin) / m_prop.m_nYPoints;
   }

   // Tidy up

   pDC->SelectObject(pFontOld);        
}

/////////////////////////////////////////////////////////////////////////////

void CViewGraph::DrawGraph(CDC* pDC)
{      
   double dY, dY0;

   // Draw the data on the graph

   int i = 0; for (i = 0; i < m_aGraphData.GetSize(); i++)
   {
      // Create pen

      int nWidth = GetWidth(pDC, m_prop.m_aLineStyle[i], m_prop.m_aLineWidth[i]);
      CPen pen(m_prop.m_aLineStyle[i], nWidth, m_prop.m_aColour[i]);
      CPen* pPenOld = pDC->SelectObject(&pen);

      // Draw lines

      int j = 0; for (j = 0; j < m_aGraphData[i].GetSize(); j++)
      {
         int nX = GetXPos(nWidth, i, j);
		 int nX1 = (nX + GetXPos(nWidth, i, j+1))/2;	 
		 int nX0 = nX - (nX1-nX) + nWidth;
		 nX1 -= nWidth;

         dY0 = m_rectAxis.bottom - (0 - m_prop.m_dYMin) * 
                     (m_rectAxis.Height()) / (m_prop.m_dYMax - m_prop.m_dYMin);
         dY0 = max(dY0, m_rectAxis.top);
         dY0 = min(dY0, m_rectAxis.bottom);         

         dY = m_rectAxis.bottom - (m_aGraphData[i][j] - m_prop.m_dYMin) * 
                     (m_rectAxis.Height()) / (m_prop.m_dYMax - m_prop.m_dYMin);

         // Vertical lines

		 if (m_prop.m_aGraphStyle[i] == CGraphProperties::lines)
		 {
			 pDC->MoveTo(nX, (int)dY0);
			 pDC->LineTo(nX, (int)dY);
		 } 
       // Vertical columns

       else if (m_prop.m_aGraphStyle[i] == CGraphProperties::columns)
		 {
			 CRect rect;
			 rect.left = nX0;
			 rect.right = nX1;
			 rect.top = (long)dY;
			 rect.bottom = (long)dY0;
			 pDC->FillSolidRect(&rect, m_prop.m_aColour[i]);
		 }
       // Symbols

       else if (m_prop.m_aGraphStyle[i] == CGraphProperties::symbols)
       {
          int nY = (int)dY;
          pDC->MoveTo(nX-nWidth, nY-nWidth);
          pDC->LineTo(nX+nWidth, nY+nWidth);
          pDC->MoveTo(nX-nWidth, nY+nWidth);
          pDC->LineTo(nX+nWidth, nY-nWidth);
       }
      }

      pDC->SelectObject(pPenOld);
   }; 
}

/////////////////////////////////////////////////////////////////////////////

int CViewGraph::GetXPos(int nWidth, int i, int j)
{
     return m_rectAxis.left + ((j+1)*m_rectAxis.Width())/(m_aGraphData[i].GetSize()+1);
}

/////////////////////////////////////////////////////////////////////////////

void CViewGraph::DrawTitle(CDC* pDC,  int nAdjustV)
{      
   int nSize = 16;
   CSize sz;

   // Determine rectangle

   CRect rect;
   rect.left = m_rectAxis.left;
   rect.right = m_rectAxis.right;
   rect.top = m_rect.top;
   rect.bottom = (m_rectAxis.top*nAdjustV)/100;

   // Determine the size of font that will fit
   
   ScaleFont(pDC, nSize, m_prop.m_sTitle, rect.Width());   

   // Create the font

   CFont font;
   CreateFont(pDC, font, nSize);
   CFont* pFontOld = pDC->SelectObject(&font);           

   // Output the text

   pDC->SetBkMode(TRANSPARENT);
   pDC->DrawText(m_prop.m_sTitle, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX); 

   pDC->SelectObject(pFontOld);        

}

/////////////////////////////////////////////////////////////////////////////

void CViewGraph::DrawLegend(CDC* pDC, int nAdjustV, int nLegendW)
{
   CFont font;   
   CRect rect;

   // Size font according to the number of labels

   int nFont = max(6,min(12,((12 * 8) / (m_prop.m_aLegendText.GetSize()+2))));

   // Create font
         
   ScaleFont(pDC, nFont, "123", (m_rect.right-m_rectAxis.right)/nLegendW);
   CreateFont(pDC, font, nFont);   
   CFont* pFontOld = pDC->SelectObject(&font);        
   CSize sz = pDC->GetTextExtent("123");

   int nY = (m_rectAxis.top * nAdjustV) / 100;

   int i = 0; for (i = 0; i < m_prop.m_aLegendText.GetSize(); i++)
   {
      // Determine height      

      int nWidth = GetWidth(pDC, m_prop.m_aLineStyle[i], m_prop.m_aLineWidth[i]);
      CPen pen(m_prop.m_aLineStyle[i], nWidth, m_prop.m_aColour[i]);

      CPen* pPenOld = pDC->SelectObject(&pen);

      // Draw line

      pDC->MoveTo(m_rectAxis.right + sz.cx/3, nY + sz.cy/2);
      pDC->LineTo(m_rectAxis.right + sz.cx/3*2, nY);

      // Draw the text
     
      rect.left = m_rectAxis.right + sz.cx/3*3;
      rect.right = m_rect.right - sz.cx/3;
      rect.top = nY;
      rect.bottom = m_rect.bottom;

      nY += pDC->DrawText(m_prop.m_aLegendText[i], &rect, DT_LEFT|DT_WORDBREAK|DT_TOP|DT_NOPREFIX);               

      // Tidy up

      pDC->SelectObject(pPenOld);
   }

   pDC->SelectObject(pFontOld);
}


///////////////////////////////////////////////////////////////////////////////

void CViewGraph::OnSize(UINT nType, int cx, int cy) 
{
   cy = max(50, cy);

	CBDView::OnSize(nType, cx, cy);	   
	
}

/////////////////////////////////////////////////////////////////////////////

BOOL CViewGraph::OnPreparePrinting(CPrintInfo* pInfo) 
{	
   return DoPreparePrinting(pInfo);
}

/////////////////////////////////////////////////////////////////////////////

void CViewGraph::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
    pMainFrame->GetMessageBar()->SetWindowText("");   
	
	CBDView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

/////////////////////////////////////////////////////////////////////////////

void CViewGraph::OnFileSaveAs() 
{   

}

/////////////////////////////////////////////////////////////////////////////

void CViewGraph::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(FALSE);	
}

/////////////////////////////////////////////////////////////////////////////

void CViewGraph::OnUpdateFileOpen(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(FALSE);	
}



/////////////////////////////////////////////////////////////////////////////

void CViewGraph::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
   m_rect = pInfo->m_rectDraw;

	CBDView::OnPrint(pDC, pInfo);
}

/////////////////////////////////////////////////////////////////////////////

COLORREF CViewGraph::GetColour(int i)
{
   const COLORREF aCR[] = {RGB(0,0,128),RGB(0,128,0),RGB(128,0,0), 
                           RGB(0,128,128),RGB(128,128,0),RGB(128,0,128),                           
                           RGB(0,0,255),RGB(0,255,0),RGB(255,0,0), 
                           RGB(0,255,255),RGB(255,255,0),RGB(255,0,255),
                           RGB(0,0,192),RGB(0,192,0),RGB(192,0,0), 
                           RGB(0,192,192),RGB(192,192,0),RGB(192,0,192),
                           RGB(0,0,96),RGB(0,96,0),RGB(96,0,0), 
                           RGB(0,96,96),RGB(96,96,0),RGB(96,0,96)};
   
   return aCR[i % (sizeof(aCR)/sizeof(COLORREF))];
}

/////////////////////////////////////////////////////////////////////////////

int CViewGraph::GetWidth(CDC* pDC, int nStyle, int nWidth)
{
   if (nStyle == PS_SOLID)
      return (int)(pDC->GetDeviceCaps(LOGPIXELSX) / MMPERINCH /3)*nWidth;
   else 
      return 1;
}

//////////////////////////////////////////////////////////////////////////////

void CViewGraph::ScaleFont(CDC* pDC, int& nFont, LPCSTR s, int nWidth, int nHeight)
{   
   CSize sz;
   do 
   {
      CFont font;
      CreateFont(pDC, font, nFont);   
      CFont* pFontOld = pDC->SelectObject(&font);           
      
      sz = pDC->GetTextExtent(s);
      pDC->SelectObject(pFontOld);      

      nFont--;

   } while ((sz.cx > nWidth || (sz.cy > nHeight && nHeight != 0)) &&  nFont > 2);
}

//////////////////////////////////////////////////////////////////////////////

void CViewGraph::CreateFont(CDC* pDC, CFont& font, int nSize, BOOL bVert)
{
   // Adjust size of font according to size of screen

   int n = (int)sqrt((double)square(m_rect.Width()) + square(m_rect.Height()));
   nSize = max(1,min((nSize*3)/2,(nSize * n) / 800));

   // Create font

   LOGFONT lf = m_prop.m_logfont;
   lf.lfHeight = -(nSize * m_prop.m_logfont.lfHeight) / 12;     

   if (bVert)
   {
      lf.lfOrientation = -450;
      lf.lfEscapement = -450;
   }
   CBDView::ScaleFont(pDC, &lf);         
   font.CreateFontIndirect(&lf);               

}

///////////////////////////////////////////////////////////////////////////////

void CViewGraph::OnEditCopy() 
{
   CBDView::OnEditCopy();	
}

///////////////////////////////////////////////////////////////////////////////

void CViewGraph::OnGraphProp() 
{
	CDlgGraphProp dlg(&m_prop);
   if (dlg.DoModal() == IDOK)
   {
      Invalidate();
      RedrawWindow();
   }
}

///////////////////////////////////////////////////////////////////////////////

void CViewGraph::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   OnGraphProp();
	
	CBDView::OnLButtonDblClk(nFlags, point);
}
