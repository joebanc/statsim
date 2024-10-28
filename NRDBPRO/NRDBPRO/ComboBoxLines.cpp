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
#include "ComboBoxLines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

DWORD CComboBoxLines::m_anLines[] = 
{

    MAKELONG(PS_NULL ,1), // None
    MAKELONG(PS_SOLID ,1), MAKELONG(PS_DASH ,1),
    MAKELONG(PS_DOT ,1), MAKELONG(PS_DASHDOT ,1), MAKELONG(PS_DASHDOTDOT ,1), 
    MAKELONG(PS_SOLID ,2), MAKELONG(PS_SOLID, 3), MAKELONG(PS_SOLID ,4), 
    MAKELONG(PS_SOLID ,5)
};


/////////////////////////////////////////////////////////////////////////////
// CComboBoxLines

CComboBoxLines::CComboBoxLines()
{
}

CComboBoxLines::~CComboBoxLines()
{
}


BEGIN_MESSAGE_MAP(CComboBoxLines, CComboBox)
	//{{AFX_MSG_MAP(CComboBoxLines)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboBoxLines message handlers

void CComboBoxLines::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
   CDC* pDC = CDC::FromHandle(lpDIS->hDC);    

// Determine line properties

   int nPenStyle = LOWORD(lpDIS->itemData);   
   int nWidth = HIWORD(lpDIS->itemData);   

 // Determine colours

   COLORREF cr = GetSysColor(COLOR_WINDOWTEXT);      
   if (lpDIS->itemState & ODS_SELECTED) cr = GetSysColor(COLOR_HIGHLIGHTTEXT);

   COLORREF crBack = GetSysColor(COLOR_WINDOW);   
   if (lpDIS->itemState & ODS_SELECTED) crBack = GetSysColor(COLOR_HIGHLIGHT);                                                  
  // Output the colour
    
    if ( lpDIS->itemAction & ODA_DRAWENTIRE ||
         lpDIS->itemAction & ODA_SELECT)
    {  
       CBrush brBack(crBack);
       pDC->FillRect(&lpDIS->rcItem, &brBack);

      // Fill with colour

       pDC->SetBkMode(TRANSPARENT);
       
       if (nPenStyle == PS_NULL)
       {
          pDC->SetTextColor(cr);
          pDC->DrawText(BDString(IDS_NONE), &lpDIS->rcItem, DT_CENTER);          
       } else
       {
          CPen pen(nPenStyle, nWidth, cr);
          CPen* pPenOld = pDC->SelectObject(&pen);
          pDC->MoveTo(lpDIS->rcItem.left+1, (lpDIS->rcItem.top+lpDIS->rcItem.bottom)/2);
          pDC->LineTo(lpDIS->rcItem.right-1, (lpDIS->rcItem.top+lpDIS->rcItem.bottom)/2);
          pDC->SelectObject(pPenOld);            
       };
    }    
}

/////////////////////////////////////////////////////////////////////////////

void CComboBoxLines::Initialise(int nLineStyle, int nLineWidth)
{
   int i = 0; for (i = 0; i < sizeof(m_anLines)/sizeof(DWORD); i++)
   {
      int index = AddString("");      
      SetItemData(index,m_anLines[i]);

      if (nLineStyle == GetStyle(index) && nLineWidth == GetWidth(index))
      {
         SetCurSel(index);
      }
   }   
}


