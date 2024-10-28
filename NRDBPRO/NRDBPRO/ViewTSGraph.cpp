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
#include "ViewTSGraph.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

const int MAX_DAYS_MONTH = 31;
const int MAX_STEP = 16;
const double AVERAGE_MONTH=30.4375;
const int MAX_DAYS_YEAR = 366;

/////////////////////////////////////////////////////////////////////////////
// CViewTSGraph

IMPLEMENT_DYNCREATE(CViewTSGraph, CViewGraph)

CViewTSGraph::CViewTSGraph()
{
}

CViewTSGraph::~CViewTSGraph()
{
}


BEGIN_MESSAGE_MAP(CViewTSGraph, CViewGraph)
	//{{AFX_MSG_MAP(CViewTSGraph)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewTSGraph drawing

void CViewTSGraph::OnDraw(CDC* pDC)
{	
   CViewGraph::OnDraw(pDC);

}

/////////////////////////////////////////////////////////////////////////////
// CViewTSGraph diagnostics

#ifdef _DEBUG
void CViewTSGraph::AssertValid() const
{
	CViewGraph::AssertValid();
}

void CViewTSGraph::Dump(CDumpContext& dc) const
{
	CViewGraph::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

void CViewTSGraph::OnInitialUpdate() 
{
	CViewGraph::OnInitialUpdate();
	
	// Determine whether to display sticks or lines

   for (int iLine = 0; iLine < m_pDoc->GetNumLines(); iLine++)
   {
      m_prop.m_aGraphStyle[iLine] = CGraphProperties::lines;

      if (m_pDoc->GetLegend(iLine).Find(BDString(IDS_TOTAL)) >=0 ) m_prop.m_aGraphStyle[iLine] = CGraphProperties::sticks;
   };
	
}

/////////////////////////////////////////////////////////////////////////////
// CViewTSGraph message handlers

void CViewTSGraph::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
   double dValue;      
   CDateTime datetime;      

   BeginWaitCursor();

   // Retrieve document
  
   m_aXPosData.SetSize(m_pDoc->GetNumLines());
   m_aGraphData.SetSize(m_pDoc->GetNumLines());

   int iLine = 0;

   for (iLine = 0; iLine < m_pDoc->GetNumLines(); iLine++)
   {      
      BOOL bFound = GetDoc()->GetFirstData(iLine, datetime, dValue);   
      
      if (!m_dtStart.IsValid() && datetime.IsValid())
      {
         m_dtStart = datetime; 
         m_dtEnd = datetime;
      };
              
      while (bFound)
      {
        // Transfer the data

         if (datetime.IsValid())
         {
            if (datetime < m_dtStart) m_dtStart = datetime;
            if (datetime > m_dtEnd) m_dtEnd = datetime;

            if (!IsNullDouble(dValue))
            {
             // If monthly or annual data then set to middle

               if (datetime.GetMonth() == 0) datetime.Advance(182,0);
               else if (datetime.GetDay() == 0) datetime.Advance(15,0);

               m_bData = TRUE;
               m_aXPosData[iLine].Add(datetime.GetDateLong());
               m_aGraphData[iLine].Add(dValue);                               
            }
         };
                 
         // Get the next reading
         
         bFound = GetDoc()->GetNextData(iLine, datetime, dValue);
      }           
    
    
    // Set number of data sets
            
      m_prop.m_aLegendText.SetAtGrow(iLine,m_pDoc->GetLegend(iLine));      
      m_prop.m_aColour.SetAtGrow(iLine, GetColour(iLine));
      m_prop.m_aLineStyle.SetAtGrow(iLine, PS_SOLID);
      m_prop.m_aLineWidth.SetAtGrow(iLine, 2);
      m_prop.m_aGraphStyle.SetAtGrow(iLine, CGraphProperties::lines);

      // For 'total' data such as rainfall, display sticks      
   };
   
   // Update start and end dates

   if (m_dtStart == m_dtEnd)
   {
      m_dtStart = m_dtStart - 365*5;
      m_dtEnd = m_dtEnd + 365*5;
   }

   // If monthly data then set the last date to the end of the month

   if (m_dtEnd.GetMonth() == 0)
   {
      m_dtEnd.AdvanceYear();      
      m_dtEnd.Advance(0,-1);
   }
   else if (m_dtEnd.GetDay() == 0)
   {
      m_dtEnd.AdvanceMonth(); 
      m_dtEnd.Advance(0,-1);
   }


   // Update the displacement data

   for (iLine = 0; iLine < m_pDoc->GetNumLines(); iLine++)
   { 
      int i = 0; for (i = 0; i < m_aXPosData[iLine].GetSize(); i++)
      {
         double dDisp = CDateTime((long)m_aXPosData[iLine][i],0) - m_dtStart;          
         m_aXPosData[iLine][i] = dDisp;
      };
   };

   m_prop.m_dXMin = 0;
   m_prop.m_dXMax = m_dtEnd - m_dtStart;

   // Set titles         
   
   m_prop.m_sTitle = m_pDoc->GetTitle();   
  
   EndWaitCursor();
  
}

///////////////////////////////////////////////////////////////////////////////

void CViewTSGraph::DrawGraph(CDC* pDC)
{     
   double dXOld = 0;
   double dYOld = 0;
   double dX, dY, dY0;

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
         dX = m_rectAxis.left + 1 + (m_aXPosData[i][j] - m_prop.m_dXMin) *
                     (m_rectAxis.Width()-2) / (m_prop.m_dXMax - m_prop.m_dXMin);

         dY = m_rectAxis.bottom +1 - (m_aGraphData[i][j] - m_prop.m_dYMin) * 
                     (m_rectAxis.Height()-2) / (m_prop.m_dYMax - m_prop.m_dYMin);

         // Join lines

         if (m_prop.m_aGraphStyle[i] == CGraphProperties::lines)
         {
            if (j > 0)
            {
               pDC->MoveTo((int)dXOld, (int)dYOld);
               pDC->LineTo((int)dX,(int)dY);
            };

            // If only one point then draw a cross

            if (m_aGraphData[i].GetSize() == 1)
            {
               pDC->MoveTo((int)dX - nWidth, (int)dY - nWidth);
               pDC->LineTo((int)dX + nWidth, (int)dY + nWidth);
               pDC->MoveTo((int)dX - nWidth, (int)dY + nWidth);
               pDC->LineTo((int)dX + nWidth, (int)dY - nWidth);
            }
         } 
         // Draw sticks

         else if (m_prop.m_aGraphStyle[i] == CGraphProperties::sticks)
         {
            dY0 = m_rectAxis.bottom - (0 - m_prop.m_dYMin) * 
                        (m_rectAxis.Height()) / (m_prop.m_dYMax - m_prop.m_dYMin);
            dY0 = max(dY0, m_rectAxis.top);
            dY0 = min(dY0, m_rectAxis.bottom);
 
            pDC->MoveTo((int)dX, (int)dY);
            pDC->LineTo((int)dX, (int)dY0);
         }
         else if (m_prop.m_aGraphStyle[i] == CGraphProperties::symbols)
         {
            int nX = (int)dX;
            int nY = (int)dY;
            pDC->MoveTo(nX-nWidth, nY-nWidth);
            pDC->LineTo(nX+nWidth, nY+nWidth);
            pDC->MoveTo(nX-nWidth, nY+nWidth);
            pDC->LineTo(nX+nWidth, nY-nWidth);
         }
         dXOld = dX;
         dYOld = dY;
      }

      pDC->SelectObject(pPenOld);
   };

}

///////////////////////////////////////////////////////////////////////////////
//
// BOOL CViewTSGraph::DrawXAxis()
//

void CViewTSGraph::DrawXAxis(CDC* pDC)
{
   BOOL bOK = TRUE;
   BOOL m_bYearTicks;

   CFont font;
   CreateFont(pDC, font, 9);
   CFont* pFontOld = pDC->SelectObject(&font);

   CSize sz = pDC->GetTextExtent("ABC");
 
   m_bDays = FALSE;
   m_bMonthsFull = FALSE;  
   m_bMonths = FALSE;
   m_bDecades = FALSE;

 

  // Check that the data represents a valid range

   if (!m_dtStart.IsValid()) return;

 // Determine position of axis

   m_dTextHeight = sz.cy;      
   m_dTextWidth =  sz.cx/3;
   m_dOriginX = m_rectAxis.left;
   m_dOriginY = m_rectAxis.bottom;
   m_dLenX = m_rectAxis.Width();
   m_dTickLength = m_dTextHeight / 3;

  // Determine positioning of labels

   m_dDayY = m_dOriginY + m_dTextHeight*0.2;
   m_dMonthY = m_dOriginY + m_dTextHeight*1.0;
   m_dYearY = m_dOriginY + m_dTextHeight*1.8;

 // Retrieve details of the graph from the corresponding document      

   m_dRange = m_dtEnd-m_dtStart+1;
 
 // If x scale is negative, do not display as it is not possible to determine where
 // graphics server places y axis

   if (m_prop.m_dXMin < 0)
   {
      return;
   }
  
 // Determine whether days are shown

   if (m_dRange < 100)
   { 
      m_bDays = TRUE;
   };
  
  // Determine if there is enough space to draw months in full

   if ((m_dTextWidth * 4.0 * m_dRange) / MAX_DAYS_MONTH < m_dLenX)
   {
      m_bMonthsFull = TRUE;
   };
      
   if (m_dRange < 2000)
   {
      m_bMonths = TRUE;
   } 
   
   m_bYearTicks = m_dRange < 50000;

   if (m_dRange > 10000)
   {
      m_bDecades = TRUE;
   } 
      
 // Draw axis ticks for days
 
   if (bOK && m_bDays)             
   { 
       CDateTime datetime = m_dtStart;
       while (datetime <= m_dtEnd)
       {             
          double dPosX = 0;
          
          if (m_dRange != 0)
          {
             dPosX = (int)(m_dOriginX + ((datetime - m_dtStart)*m_dLenX)/m_dRange);
          };
          
          pDC->MoveTo((int)dPosX, (int)m_dOriginY);
          pDC->LineTo((int)dPosX, (int)(m_dOriginY+m_dTickLength));
                           
          datetime.Advance(1,0);
       };      
   };
 
 // Draw ticks for months
   
   if (bOK && m_bMonths)
   {       
       int nMonth = max(1,m_dtStart.GetMonth());
       int nYear = m_dtStart.GetYear();
       CDateTime datetime;       
       do
       {
         CDateTime date(nYear,nMonth,1,0,0,0);
         datetime = date;
         nMonth++;
         if (nMonth > 12)
         {
            nMonth = 1;
            nYear++;
         }
       } while (datetime < m_dtStart);
  
       while (datetime < m_dtEnd)
       {             
          double dPosX = 0;
          
          if (m_dRange != 0)
          {
              dPosX = m_dOriginX + (((datetime - m_dtStart)*m_dLenX)/m_dRange);
          };             
          
          pDC->MoveTo((int)dPosX, (int)m_dOriginY);
          pDC->LineTo((int)dPosX, (int)(m_dOriginY+m_dTickLength*2));
          
      // Increment the month
      
          int nMonth = datetime.GetMonth();
          int nYear = datetime.GetYear();
          nMonth += 1;
          if (nMonth > 12)
          { 
             nMonth = 1;
             nYear += 1;
          };
          datetime = CDateTime(nYear,nMonth,1,0,0,0);
       }; 
   }
 
 // Draw ticks for years
 
   if (bOK)
   {       
 
 // Determine first visible year
    
       int nYear = m_dtStart.GetYear();
       CDateTime datetime;       
       do
       {
         CDateTime date(nYear,1,1,0,0,0);
         datetime = date;
         nYear++;
       } while(datetime < m_dtStart);   
 
 // Display ticks for years
       
       while (datetime <= m_dtEnd)
       {         
          if (!m_bDecades || m_bYearTicks || datetime.GetYear() % 10 == 0)
          {
             double dPosX = 0;
          
             if (m_dRange != 0)
             {
                dPosX = m_dOriginX + (((datetime - m_dtStart)*m_dLenX)/m_dRange);
             };
          
             int nTick = datetime.GetYear() % 10 == 0 ? 2 : 1;

             pDC->MoveTo((int)dPosX, (int)m_dOriginY);             
             pDC->LineTo((int)dPosX, (int)(m_dOriginY+m_dTickLength*nTick));             
          };
       
       // Increment the year
          
          int nYear = datetime.GetYear();          
          nYear += 1;
          datetime = CDateTime(nYear,1,1,0,0,0);
       }; 
   }  
  
   // Label the axis
   LabelXAxis(pDC);

   // Tidy up

   pDC->SelectObject(pFontOld);
  
};

///////////////////////////////////////////////////////////////////////////////
//                      
// CTSPlotView::LabelXAxis()
//
// Label the X axis for the graph
//

BOOL CViewTSGraph::LabelXAxis(CDC* pDC)
{  
      BOOL bSuccess = TRUE;
   
 // Determine what will be displayed on the axis
                                          
  // Draw the day numbers onto the graph
  
   if (bSuccess && m_bDays)
   {   
   
  // Determine if there is sufficent room to display all the text and
  // have a space either side
          
       long lAdvance = 1;             
       while (m_dLenX < m_dTextWidth * m_dRange *2 / lAdvance && 
              lAdvance < MAX_STEP)
       {
         lAdvance *= 2;
       }
          
  // Output the days
           
       CDateTime datetime = m_dtStart + 0.5;
       while (datetime < m_dtEnd + 1)
       {  
                   
          double dPosX = 0;
          
          if (m_dRange != 0)
          {   
             double dDisp = datetime - m_dtStart;
             dPosX = m_dOriginX + (dDisp * m_dLenX)/m_dRange;
          }                             
                                                                               
		  std::strstream strDay;
          int nDay = datetime.GetDay();
          if (nDay > 0)
          {
			  strDay << nDay << std::ends;
             DrawTextC(pDC, strDay.str(), (int)dPosX,(int)m_dDayY);
             strDay.rdbuf()->freeze(0);
          }
          datetime.Advance(lAdvance,0);   
       }; 
   }
  
  // Draw months onto the graph.  If the starting month falls outside
  // the range of dates then set it to the middle of the date range   
   
   if (bSuccess && m_bMonths)
   {   
       long lAdvance = 1;       
       while (m_dLenX < m_dTextWidth * m_dRange / MAX_DAYS_MONTH / lAdvance && 
              lAdvance < MAX_STEP)
       {
         lAdvance *= 2;
       }


       double dMonthY = m_dMonthY;
       if (m_bDays == FALSE)
       {
          dMonthY = m_dDayY;
       }
  
  // Determine start month that is visible on the graph
   
       int nMonth = max(1,m_dtStart.GetMonth());
       int nYear = m_dtStart.GetYear();
       CDateTime datetime;
       
       do
       {
         CDateTime date(nYear,nMonth,15,0,0,0);
         datetime = date;
         nMonth+=1;
         if (nMonth > 12)
         {
            nMonth = 1;
            nYear++;
         }
       } while (datetime < m_dtStart); 
  
 // If the date selected falls outside the range then set it to the middle of 
 // the date range
 
      if (datetime > m_dtEnd + 1)
      { 
         datetime = m_dtStart+(m_dtEnd-m_dtStart+1)/2;
      };           
       
  // Draw visible labels on the graph
       
       while (datetime <= m_dtEnd+1)
       {             
          int dPosX = 0;
          
          if (m_dRange != 0)
          {
            dPosX = (int)(m_dOriginX + ((datetime - m_dtStart)*m_dLenX)/m_dRange);
          };
                                                                                         
          int nMonth = datetime.GetMonth();
          
          CString strMonthLabels;
          CString strMonth;          
          
          if (m_bMonthsFull)
          {                         
            // If displaying days then append the year to the months name

             if (m_bDays)
             {
                strMonth.Format("%s %i", GetMonthName(nMonth), datetime.GetYear());
             } else
             {
                strMonth.Format("%s", GetMonthName(nMonth));
             }
          } else
          {             
             strMonthLabels.Format("%s", GetMonthName(nMonth));
             strMonth = strMonthLabels[0];
          };          
          DrawTextC(pDC, strMonth.GetBuffer(0), (int)dPosX,(int)dMonthY);
          datetime = datetime + AVERAGE_MONTH * lAdvance;
       }; 
   }
  
  // Draw the years onto the graph, if the decades flag is set then only
  // draw these
  
   if (bSuccess && !m_bDays)
   {                            
  
  // Determine the advance step

       long lAdvance = 1;       
       if (!m_bDecades)
       {                 
          while (m_dLenX < m_dTextWidth * m_dRange *4 / MAX_DAYS_YEAR / lAdvance && 
              lAdvance < MAX_STEP)
          {
            lAdvance *= 2;
          }
       } else
       {
          while (m_dLenX < m_dTextWidth * m_dRange *4 / MAX_DAYS_YEAR / 9 / lAdvance && 
                 lAdvance < MAX_STEP)
          {
            lAdvance *= 2;
          }
       }

  // Determine y position of text
  
       double dYearY = m_dYearY;
       if (m_bMonths == FALSE && m_bMonthsFull == FALSE)
       {        
          dYearY = m_dMonthY;
       }
       else if (m_bDays == FALSE)       
       {
          dYearY = m_dMonthY;
       }
     
  // Determine first visible year label
  
       int nYear = m_dtStart.GetYear();    
       CDateTime datetime;               
       do
       {
         CDateTime date(nYear,7,1,0,0,0);         
         datetime = date;
         nYear++;         
       } while (datetime < m_dtStart);   
       
  // If the year label falls outside the date range then set it to be the
  // middle of the date range
 
      if (datetime > m_dtEnd + 1)
      { 
         datetime = m_dtStart + (m_dtEnd - m_dtStart + 1)/2;
      };         
  
  // Output years
       
       int nDecade = 0;
       while (datetime.GetYear() <= m_dtEnd.GetYear())
       {             
          int dPosX = 0;
          
          if (m_dRange != 0)
          {                     
             dPosX = (int)(m_dOriginX + ((datetime - m_dtStart)*m_dLenX)/m_dRange);           
          };

          if (dPosX > m_dOriginX + m_dLenX) break;
		                                                                                           
          int nYear = datetime.GetYear();
          
          CString strMonthLabels;
          CString strMonth;          
          
          if (m_bDecades == FALSE || nYear%10 == FALSE)
          { 
             if (!m_bDecades || (nDecade++)%lAdvance== 0)
             {
				 std::strstream strDay;
				 strDay << datetime.GetYear() << std::ends;
                DrawTextC(pDC,  strDay.str(), (int)dPosX,(int)dYearY);
                strDay.rdbuf()->freeze(0);
             };
          };          

          if (m_bDecades)
          {
             datetime = datetime + 365.25;
          } else
          {
             datetime = datetime + 365.25 * lAdvance;
          }
       }; 
   }  
   
   return bSuccess;
};

///////////////////////////////////////////////////////////////////////////////

CString CViewTSGraph::GetMonthName(int nMonth)
{
   return CString(CDateTime::GetMonthName(nMonth)).Left(3);
}

///////////////////////////////////////////////////////////////////////////////

void CViewTSGraph::DrawTextC(CDC* pDC, LPCSTR s, int x, int y)
{
    CSize sz = pDC->GetTextExtent(s);

    CRect rect;
    rect.left = x - sz.cx/2;
    rect.right = x + sz.cx/2;
    rect.top = y;
    rect.bottom = y + sz.cy;

    pDC->DrawText(s, &rect, DT_SINGLELINE|DT_TOP|DT_CENTER);
}

///////////////////////////////////////////////////////////////////////////////   

void CViewTSGraph::OnMouseMove(UINT nFlags, CPoint point) 
{	
	CViewGraph::OnMouseMove(nFlags, point);
}

