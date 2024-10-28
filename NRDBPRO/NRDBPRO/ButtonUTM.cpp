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
#include "nrdb.h"
#include "ButtonUTM.h"
#include "dlgutm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define EQUATOR 120
#define HEIGHT 196

/////////////////////////////////////////////////////////////////////////////
// CButtonUTM

CButtonUTM::CButtonUTM()
{
   m_nZone = 0;
   m_rect.left = m_rect.right = 0;
}

CButtonUTM::~CButtonUTM()
{
}


BEGIN_MESSAGE_MAP(CButtonUTM, CButton)
	//{{AFX_MSG_MAP(CButtonUTM)
	ON_WM_DRAWITEM()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CButtonUTM::SetZone(int nZone) 
{
   ASSERT((nZone >= 1 && nZone <= 60) || (nZone <= -1 && nZone >= -60));

   m_nZone = nZone;   

   CRect rect;
   GetClientRect(&rect);
   m_rect.left = max(0,m_rect.left - m_rect.Width());
   m_rect.right = min(rect.right, m_rect.right + m_rect.Width());

   RedrawWindow(m_rect, NULL, RDW_INVALIDATE|RDW_UPDATENOW);  // No erase

   
}

/////////////////////////////////////////////////////////////////////////////
// CButtonUTM message handlers

void CButtonUTM::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
   CBitmap bitmap;

   CDC* pDC = CDC::FromHandle(lpDIS->hDC);

   bitmap.LoadBitmap(IDB_UTM);
   CDC dc;
   dc.CreateCompatibleDC(pDC);
   CBitmap* pBitMapOld = dc.SelectObject(&bitmap);    

   // Only redraws part of window that has changed
    
    CRect rect = lpDIS->rcItem;
	
    CRect rectC;    	 
    GetClientRect(&rectC);
    	
	// Copy bitmap
   	
   pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dc, 
               rect.left, rect.top, SRCCOPY);

   dc.SelectObject(pBitMapOld);    

   // Draw the zone

   int longitude = -177+(abs(m_nZone)-1)*6;
   int x1 = rectC.left + (rectC.Width() * (longitude-3+180))/360;
   int x2 = rectC.left + (rectC.Width() * (longitude+3+180))/360;

   CPen pen(PS_SOLID, 1, RGB(255,128,0));
   CPen* pPenOld = pDC->SelectObject(&pen);   

   int y1 = 0;
   int y2 = HEIGHT-1;

   if (m_nZone < 0) y1 = EQUATOR;
   else y2 = EQUATOR;

   pDC->MoveTo(x1, y2);
   pDC->LineTo(x1, y1);
   pDC->MoveTo(x2, y2);
   pDC->LineTo(x2, y1);

   // Draw line through equator

   pDC->MoveTo(rect.left, EQUATOR);
   pDC->LineTo(rect.right, EQUATOR);
   
   pDC->SelectObject(pPenOld);   
   
   // Refresh area

   m_rect.left = max(0,x1);
   m_rect.right = min(x2+1,rectC.right);
   m_rect.top = rectC.top;
   m_rect.bottom = rectC.bottom;
}

///////////////////////////////////////////////////////////////////////////////

void CButtonUTM::OnLButtonDown(UINT nFlags, CPoint point) 
{  	
   // Determine the zone according to the where clicked and send a message to the
   // parent

   CRect rectC;
   GetClientRect(&rectC);   

   // Determine longitude corresponding to width

   int longitude = ((point.x - rectC.left)*360 / rectC.Width()) - 180;
   
   // Round to nearest zone

   longitude = (longitude+3)/6*6-3;

   // Convert to zone

   int nZone = (longitude+177)/6 + 1;

   // Determine if north or south of the equator   

   if (point.y > EQUATOR)
   {
      nZone = -nZone;
   }

   // Send message to parent

   GetParent()->PostMessage(WM_SETZONE, nZone, 0);
   
   // Default

	CButton::OnLButtonDown(nFlags, point);
}
