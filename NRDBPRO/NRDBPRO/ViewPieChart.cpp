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
#include "ViewPieChart.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#define PI 3.141592653

/////////////////////////////////////////////////////////////////////////////
// CViewPieChart

IMPLEMENT_DYNCREATE(CViewPieChart, CViewGraph)

CViewPieChart::CViewPieChart()
{
}

CViewPieChart::~CViewPieChart()
{
}


BEGIN_MESSAGE_MAP(CViewPieChart, CViewGraph)
	//{{AFX_MSG_MAP(CViewPieChart)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewPieChart drawing

void CViewPieChart::OnDraw(CDC* pDC)
{
   CFont font, *pFontOld;	

   if (!pDC->IsPrinting())
   {
      GetClientRect(&m_rect);
   };

   // Determine position of axis

   CreateFont(pDC, font, 12);
   pFontOld = pDC->SelectObject(&font);
   CSize sz = pDC->GetTextExtent("123");

   m_rectAxis.left = min(sz.cx/3*10, m_rect.Width()/6);
   m_rectAxis.right = (m_rect.right * 3) / 4;
   m_rectAxis.top = min(max(sz.cy*5, m_rect.Height()/8), m_rect.Height()/3);
   m_rectAxis.bottom = m_rect.bottom - min(max(sz.cy*2, m_rect.Height()/8), m_rect.Height()/4);

	
   DrawTitle(pDC, 75);   

   DrawGraph(pDC);

   // Allow room for axis

   m_rectAxis.right += sz.cx/3*5;

   DrawLegend(pDC, 75, 4);
   
}

/////////////////////////////////////////////////////////////////////////////
//
//

void CViewPieChart::DrawGraph(CDC* pDC)
{
   CArray <CPoint, CPoint> aPoints;
   CPoint pt;
   CString s;
   CRect rect;

   // Create the font

   CFont font;
   CreateFont(pDC, font, 8);   
   CFont* pFontOld = pDC->SelectObject(&font);           

   // Transparent text

   pDC->SetBkMode(TRANSPARENT);

   // Determine total

   double dTotal = 0;
   int i = 0;
   for (i = 0; i < m_aGraphData[0].GetSize(); i++)
   {
      dTotal = dTotal + m_aGraphData[0][i];
   }
   
   // Draw arc

   int x = (m_rectAxis.left + m_rectAxis.right)/2;
   int y = (m_rectAxis.top + m_rectAxis.bottom)/2;
   int nRadius = min(m_rectAxis.Width()/2, m_rectAxis.Height()/2);
   double dStartAngle = 0;   
   
   for (i = 0; i < m_aGraphData[0].GetSize() && dTotal != 0; i++)
   {
     // Create color
      
      CBrush brush(GetColour(i));
      CBrush* pBrushOld = pDC->SelectObject(&brush);

      double dSweepAngle = (m_aGraphData[0][i] / dTotal) * 360;      

      // Create polygon

      aPoints.Add(CPoint(x,y));
      for (double j = dStartAngle; j <= dSweepAngle+dStartAngle; j += 0.1)
      {
         pt.x = (int)(x + nRadius * sin(j / 180 * PI));
         pt.y = (int)(y + nRadius * cos(j / 180 * PI));
         aPoints.Add(pt);
      }
      pDC->Polygon(aPoints.GetData(), aPoints.GetSize());
      aPoints.RemoveAll();

      // Label the value
   
	  std::strstream str;
      str.precision(10);
	  str << m_aGraphData[0][i] << std::ends;
	   s = str.str();
	   str.rdbuf()->freeze(0);      

      // Alternate position of labels
      double dLen = 1.2;
      if (m_aGraphData[0].GetSize() > 20 && i %2) dLen = 1.1;

      pt.x = (int)(x + nRadius*dLen * sin((dSweepAngle/2+dStartAngle) / 180.0 * PI));
      pt.y = (int)(y + nRadius*dLen * cos((dSweepAngle/2+dStartAngle) / 180.0 * PI));
      CSize sz = pDC->GetTextExtent(s);
      rect.left = pt.x - sz.cx;
      rect.right = pt.x + sz.cx;
      rect.top = pt.y - sz.cy/2;
      rect.bottom = pt.y + sz.cy/2;
      pDC->DrawText(s, &rect, DT_CENTER|DT_VCENTER);

      // Draw a connecting line

      CPen pen(PS_DOT, 1, RGB(0,0,0));
      CPen* pPenOld = pDC->SelectObject(&pen);
      pDC->MoveTo(pt.x, pt.y);
      pt.x = (int)(x + nRadius * sin((dSweepAngle/2+dStartAngle) / 180.0 * PI));
      pt.y = (int)(y + nRadius * cos((dSweepAngle/2+dStartAngle) / 180.0 * PI));
      pDC->LineTo(pt.x, pt.y);
      pDC->SelectObject(pPenOld);
      
      // Next
     
      dStartAngle += dSweepAngle;

      pDC->SelectObject(pBrushOld);
   };

   pDC->SelectObject(pFontOld);
}

/////////////////////////////////////////////////////////////////////////////
// CViewPieChart diagnostics

#ifdef _DEBUG
void CViewPieChart::AssertValid() const
{
	CViewGraph::AssertValid();
}

void CViewPieChart::Dump(CDumpContext& dc) const
{
	CViewGraph::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CViewPieChart message handlers

int CViewPieChart::GetNumLegend()
{
   return m_aGraphData[0].GetSize();
}

/////////////////////////////////////////////////////////////////////////////

CString CViewPieChart::GetLegend(int iLine)
{
   return m_aLabelText[iLine];
}
