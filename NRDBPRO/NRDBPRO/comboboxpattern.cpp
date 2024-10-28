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
#include "comboboxpattern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComboBoxPattern

CComboBoxPattern::CComboBoxPattern()
{
}

CComboBoxPattern::~CComboBoxPattern()
{
}


BEGIN_MESSAGE_MAP(CComboBoxPattern, CComboBox)
	//{{AFX_MSG_MAP(CComboBoxPattern)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboBoxPattern message handlers

void CComboBoxPattern::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
   CDC* pDC = CDC::FromHandle(lpDIS->hDC);              
   
 // Determine colours

   COLORREF cr = GetSysColor(COLOR_WINDOWTEXT);      
   if (lpDIS->itemState & ODS_SELECTED) cr = GetSysColor(COLOR_HIGHLIGHTTEXT);

   COLORREF crBack = GetSysColor(COLOR_WINDOW);   
   if (lpDIS->itemState & ODS_SELECTED) crBack = GetSysColor(COLOR_HIGHLIGHT);                                                  

  // Output the symbol
    
    if ( lpDIS->itemAction & ODA_DRAWENTIRE ||
         lpDIS->itemAction & ODA_SELECT)
    { 
       int nType = LOWORD(lpDIS->itemData);
       int nHatch = HIWORD(lpDIS->itemData); 

       CBrush brBack(crBack);
       pDC->FillRect(&lpDIS->rcItem, &brBack);

       CBrush brush;
       
       // Draw the solid fill or the shape     

       if (nType != BS_NULL)
       {          
          CreateBrush(brush, nType, nHatch, GetSysColor(COLOR_WINDOWTEXT));      
          pDC->SetBkMode(TRANSPARENT);
          pDC->FillRect(&lpDIS->rcItem, &brush);          
          pDC->SetBkMode(OPAQUE);
       }
       	        
       // Display the <None> for BS_NULL

      else
      {        
         pDC->SetBkMode(TRANSPARENT);
         pDC->SetTextColor(cr);
         pDC->DrawText(BDString(IDS_NONE), &lpDIS->rcItem, DT_CENTER);
         pDC->SetBkMode(OPAQUE);
      }
      
      // Draw round the selected type

      if (lpDIS->itemState & ODS_SELECTED)
      {
         CPen pen(PS_SOLID, 2, GetSysColor(COLOR_HIGHLIGHT));
         CPen* pPenOld = pDC->SelectObject(&pen);

         pDC->MoveTo(lpDIS->rcItem.left, lpDIS->rcItem.top);
         pDC->LineTo(lpDIS->rcItem.right-1, lpDIS->rcItem.top);
         pDC->LineTo(lpDIS->rcItem.right-1, lpDIS->rcItem.bottom-1);
         pDC->LineTo(lpDIS->rcItem.left, lpDIS->rcItem.bottom-1);
         pDC->LineTo(lpDIS->rcItem.left, lpDIS->rcItem.top);

         pDC->SelectObject(pPenOld);
      };
      
    }    
}

///////////////////////////////////////////////////////////////////////////////

void CComboBoxPattern::CreateBrush(CBrush& brush, int nType, int nHatch, COLORREF cr)
{
   if (nType == BS_SOLID) brush.CreateSolidBrush(cr);
   else if (nType == BS_HATCHED) brush.CreateHatchBrush(nHatch, cr);
   else if (nType == BS_PATTERN) 
   {
      CBitmap bitmap;
      bitmap.LoadBitmap(IDB_PATTERN1);
      brush.CreatePatternBrush(&bitmap);
   }
   else if (nType == BS_NULL)
   {
      LOGBRUSH logbrush;
      logbrush.lbColor = 0;
      logbrush.lbStyle = BS_NULL;
      logbrush.lbHatch = 0;
      brush.CreateBrushIndirect(&logbrush);
   }
}

///////////////////////////////////////////////////////////////////////////////

void CComboBoxPattern::Initialise(int nType, int nHash)
{        
  
   // Add other symbols

   int iType = BS_NULL;
   do
   {   
      int iHash = HS_HORIZONTAL; 
      do          
      {               
         int index = AddString("");                  
         SetItemData(index,MAKELPARAM(iType, iHash));

         if (iType == nType && iHash == nHash) SetCurSel(index);

         if (GetCurSel() == CB_ERR && iType == BS_NULL) SetCurSel(index);

         // Re-iterate only for hatch style
         iHash++;
         if (iType != BS_HATCHED) break;
      } while (iHash < HS_DIAGCROSS);

      // Loop for three types
            
      if (iType == BS_HATCHED) break; 
      if (iType == BS_SOLID) iType = BS_HATCHED;
      if (iType == BS_NULL) iType = BS_SOLID;

   } while (TRUE);         
}

