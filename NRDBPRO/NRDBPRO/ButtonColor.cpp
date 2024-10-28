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
#include "ButtonColor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CButtonColour

CButtonColour::CButtonColour(COLORREF cr)
{
   m_color = cr;
   m_bAuto = FALSE;
}

CButtonColour::~CButtonColour()
{
}


BEGIN_MESSAGE_MAP(CButtonColour, CButton)
	//{{AFX_MSG_MAP(CButtonColour)
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CButtonColour::SetColour(COLORREF cr)
{
   if (cr != -1)
   {
      m_color = cr;
      Invalidate();
      RedrawWindow();
   };
}

/////////////////////////////////////////////////////////////////////////////
// CButtonColour message handlers

void CButtonColour::DrawItem(LPDRAWITEMSTRUCT lp)
{
   CRect rect;
   CBrush brush;
   

   CDC* pDC = CDC::FromHandle(lp->hDC);
   GetClientRect(&rect);

   COLORREF cr = m_color;
   COLORREF cr1 = RGB(255,255,255);
   COLORREF cr2 = RGB(64,64,64);

   // Fill the contents of the button in the selected color

   if (lp->itemState & ODS_DISABLED)
   {
      cr = GetSysColor(COLOR_MENU);      
      cr1 = RGB(200,200,200);
      cr2 = RGB(128,128,128);
   }
   
   brush.CreateSolidBrush(cr);
   CBrush* pBrushOld = pDC->SelectObject(&brush);
     
   pDC->FillRect(&rect, &brush);
   pDC->SelectObject(pBrushOld);

   pDC->Draw3dRect(0,0,rect.Width(), rect.Height(), cr1, cr2);

   if (m_bAuto)
   {
      pDC->SetBkMode(TRANSPARENT);
      pDC->DrawText("Auto", &rect, DT_CENTER|DT_VCENTER);
   }

}

///////////////////////////////////////////////////////////////////////////////

void CButtonColour::OnClicked() 
{
   CColorDialog dlg(m_color);
   if (dlg.DoModal() == IDOK)
   {
      m_bAuto = FALSE;

      m_color = dlg.GetColor();
      Invalidate();
      RedrawWindow();      

      // Inform parent that selection has changed

      GetParent()->PostMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CHANGED), (LPARAM)GetSafeHwnd());            
   }	   
}

///////////////////////////////////////////////////////////////////////////////

void CButtonColour::OnRButtonDown(UINT nFlags, CPoint point) 
{
   if (m_nStyle & BS_AUTO)
   {
      m_bAuto = !m_bAuto;
      Invalidate();
      RedrawWindow();
   };
	
	CButton::OnRButtonDown(nFlags, point);
}

