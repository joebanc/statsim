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
#include <io.h>

#include "nrdb.h"
#include "ComboBoxSymbol.h"
#include "comboboxpattern.h"
#include "viewmap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#define MMPERINCH 25.4
#define MAXSIZECUSTOM 10
#define DEFAULTSIZECUSTOM 5
#define DEFAULTSIZESYSTEM 3

/////////////////////////////////////////////////////////////////////////////

CCustomSymArray CComboBoxSymbol::m_aCustomSym;

/////////////////////////////////////////////////////////////////////////////
// CComboBoxSymbol

CComboBoxSymbol::CComboBoxSymbol()
{
}

CComboBoxSymbol::~CComboBoxSymbol()
{   
}

CMapStyle CComboBoxSymbol::GetStyle(int index)
{
   CMapStyle style;
   style.m_nSymbol = GetItemData(index);
   return style;
}


/////////////////////////////////////////////////////////////////////////////


BEGIN_MESSAGE_MAP(CComboBoxSymbol, CComboBox)
	//{{AFX_MSG_MAP(CComboBoxSymbol)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CComboBoxSymbol message handlers

void CComboBoxSymbol::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
   CDC* pDC = CDC::FromHandle(lpDIS->hDC);              

   CMapStyle mapstyle;

  // Set viewing size

   mapstyle.m_dSymSize = DEFAULTSIZESYSTEM;
   
  // Output the symbol
    
    if ( lpDIS->itemAction & ODA_DRAWENTIRE ||
         lpDIS->itemAction & ODA_SELECT)
    {  
       // Determine style for item data

       if (GetCount())
       {    
          mapstyle.m_nSymbol = lpDIS->itemData;
       };

        // Determine colours

       mapstyle.m_crLine = GetSysColor(COLOR_WINDOWTEXT);      
       if (lpDIS->itemState & ODS_SELECTED) mapstyle.m_crLine = GetSysColor(COLOR_HIGHLIGHTTEXT);

       COLORREF crBack = GetSysColor(COLOR_WINDOW);   
       if (lpDIS->itemState & ODS_SELECTED) crBack = GetSysColor(COLOR_HIGHLIGHT);                                                  

       CBrush brBack(crBack);
       pDC->FillRect(&lpDIS->rcItem, &brBack);

       CPen pen(PS_SOLID, 1, mapstyle.m_crLine);
       CPen* pPenOld = pDC->SelectObject(&pen);

      // Fill with colour

	   long lX = (lpDIS->rcItem.right + lpDIS->rcItem.left)/2;
	   long lY = (lpDIS->rcItem.top + lpDIS->rcItem.bottom)/2; 
        
      if (mapstyle.m_nSymbol == none)
      {
         pDC->SetBkMode(TRANSPARENT);
         pDC->SetTextColor(mapstyle.m_crLine);
         pDC->DrawText(BDString(IDS_NONE), &lpDIS->rcItem, DT_CENTER);
      }       
      else
      {
         // Increase preview size for custom symbols

         if (LOWORD(mapstyle.m_nSymbol) == user)
         {
            mapstyle.m_dSymSize = DEFAULTSIZECUSTOM;
         }

	      DrawSymbol(pDC, lX, lY, mapstyle);       
      };

      pDC->SelectObject(pPenOld);
    }    
}

///////////////////////////////////////////////////////////////////////////////

void CComboBoxSymbol::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{      
   double dSize = (double)HIWORD(lpMIS->itemData)/10;      

   CSize sz = GetSymSize(GetDC(), dSize);   
   lpMIS ->itemHeight = sz.cy*2;
}


///////////////////////////////////////////////////////////////////////////////

void CComboBoxSymbol::Initialise(const CMapStyle& mapstylesel)
{
// Initialise

	ResetContent();

   // Load custom symbols for the registry

   if (!m_aCustomSym.m_bInit)
   {
       InitCustomSym();
   }

   // Add null symbol
   
   int index = AddString("");         
   SetItemData(index, none);
   SetCurSel(index);

   // Add system symbols

   for (int iType = triangle; iType <= times; iType++)
   {            
      // 'system' is zero so will match existing values

      int index = AddString("");                  
      SetItemData(index, MAKELONG(iType, 0));

      if (mapstylesel.m_nSymbol == iType) SetCurSel(index);      
   }            

   // Add custom symbols

   int i = 0; for (i = 0; i < m_aCustomSym.GetSize(); i++)
   {
      int index = AddString("");
      DWORD dw = MAKELONG(user, m_aCustomSym[i].m_nId);
      SetItemData(index, dw);

      if ((DWORD)mapstylesel.m_nSymbol == dw) SetCurSel(index);
   }
   
}

///////////////////////////////////////////////////////////////////////////////

void CComboBoxSymbol::InitCustomSym()
{
   m_aCustomSym.RemoveAll();

   // Load the custom symbols from the database

   CBDSymbol symbol;

#ifdef _DEBUG
//   symbol.m_lId=1;
//   BDSymbol(BDHandle(), &symbol, BDDELETE);
#endif

   BOOL bFound = BDSymbol(BDHandle(), &symbol, BDGETINIT);
   while (bFound)
   {
      CCustomSymbol customsym;

      customsym.m_nId = symbol.m_lId;
      int index = m_aCustomSym.Add(customsym);

      // Initialise when on array as to image is not copied in the constructor of
      // CCustomSymbol

      if (!m_aCustomSym[index].m_imagefile.Initialise(symbol.m_symbol, FALSE))
      {
          m_aCustomSym.RemoveAt(index);   
      }
      

      bFound = BDGetNext(BDHandle());
   }
   BDEnd(BDHandle());
}

///////////////////////////////////////////////////////////////////////////////

void CComboBoxSymbol::DrawSymbol(CDC* pDC, double lX, double lY, const CMapStyle& mapstyle)
{   
	int     nPts;
   POINT   pt[6];
   int  cx, cy, diam;

   // Select pen

   CPen pen(mapstyle.m_nLineStyle, mapstyle.m_nLineWidth, mapstyle.m_crLine);
   CPen* pPenOld = pDC->SelectObject(&pen);

   CBrush brush;
   CComboBoxPattern::CreateBrush(brush, mapstyle.m_nPattern, mapstyle.m_nHatch, mapstyle.m_crFill);   

	// Select brush
   
   CBrush* poldbrush = NULL;
	poldbrush = pDC->SelectObject( &brush );   
    
	// Determine size of symbol to be drawn

	CSize size = GetSymSize(pDC, mapstyle.m_dSymSize);
       
    cx = (long)lX;
    cy = (long)lY;
    diam = (int)(size.cy/2) +2;
      
	pDC->SetPolyFillMode( WINDING );
   switch (LOWORD(mapstyle.m_nSymbol)) 
	{
	   case triangle:                        
        nPts = 3;
		  pt[0].x = cx - diam;  pt[0].y = cy + diam;
		  pt[1].x = cx + diam;  pt[1].y = cy + diam;
		  pt[2].x = cx ;        pt[2].y = cy - diam;		  
		  pDC->Polygon( pt, nPts );
		  break;        

      case inversetriangle:
        nPts = 3;
		  pt[0].x = cx - diam;  pt[0].y = cy - diam;
		  pt[1].x = cx + diam;  pt[1].y = cy - diam;
		  pt[2].x = cx ;        pt[2].y = cy + diam;		  
		  pDC->Polygon( pt, nPts );
		  break;   
    
	   case square:
		  nPts = 4;
		  pt[0].x = cx - diam;  pt[0].y = cy - diam;
		  pt[1].x = cx + diam;  pt[1].y = cy - diam;
		  pt[2].x = cx + diam;  pt[2].y = cy + diam;
		  pt[3].x = cx - diam;  pt[3].y = cy + diam;		  
		  pDC->Polygon( pt, nPts );
		  break;

	   case circle:                    
		  pt[0].x = cx ;  pt[0].y = cy;
		  pDC->Ellipse(pt[0].x - diam, pt[0].y-diam, pt[0].x+diam, pt[0].y+diam);
		  break;

	   case diamond:
		  nPts = 4;
		  pt[0].x = cx;        pt[0].y = cy - diam;
		  pt[1].x = cx + diam;  pt[1].y = cy;
		  pt[2].x = cx;        pt[2].y = cy + diam;
		  pt[3].x = cx - diam;  pt[3].y = cy;		  
		  pDC->Polygon( pt, nPts );
		  break;

      case arrow:
         nPts = 5;
         pt[0].x = cx+diam*2; pt[0].y = cy+diam*2;
         pt[1].x = cx; pt[1].y = cy;
         pt[2].x = cx+diam;      pt[2].y = cy;
         pt[3].x = cx; pt[3].y = cy+diam;
         pt[4].x = cx; pt[4].y = cy;         
	      pDC->Polyline(pt, nPts );
         break;

      case cross:
         nPts = 6;
         pt[0].x = cx; pt[0].y = cy;
         pt[1].x = cx-diam; pt[1].y = cy;
         pt[2].x = cx+diam; pt[2].y = cy;
         pt[3].x = cx; pt[3].y = cy;
         pt[4].x = cx; pt[4].y = cy-diam;
         pt[5].x = cx; pt[5].y = cy+diam;
         pDC->Polyline(pt, nPts );
         break;    

      case times:
         nPts = 5;
         pt[0].x = cx-diam; pt[0].y = cy-diam;
         pt[1].x = cx+diam; pt[1].y = cy+diam;
         pt[2].x = cx; pt[2].y = cy;
         pt[3].x = cx-diam; pt[3].y = cy+diam;
         pt[4].x = cx+diam; pt[4].y = cy-diam;         
         pDC->Polyline(pt, nPts );
         break;    

      case user:         
         {
            long lId = HIWORD(mapstyle.m_nSymbol);
            CRect rect = CRect(cx-diam, cy-diam, cx+diam, cy+diam);

            int i = 0; for (i = 0; i < m_aCustomSym.GetSize(); i++)
            {
               if (m_aCustomSym[i].m_nId == lId)
               {
                  m_aCustomSym[i].m_imagefile.OnDraw(pDC, rect);
                  break;
               };
            };

            break;
         };
    };   
   
 // Tidy up

	if( poldbrush != NULL)
	  pDC->SelectObject( poldbrush );	

   if (pPenOld != NULL) pDC->SelectObject(pPenOld);

}

/////////////////////////////////////////////////////////////////////////////

CSize CComboBoxSymbol::GetSymSize(CDC* pDC, double dSize)
{
   CSize size;
   
   size.cx = (int)(dSize * pDC->GetDeviceCaps(LOGPIXELSX) / MMPERINCH /2); 
   size.cy = (int)(dSize * pDC->GetDeviceCaps(LOGPIXELSY) / MMPERINCH /2); 
  
   return size;
}

///////////////////////////////////////////////////////////////////////////////

void CComboBoxSymbol::OnDestroy() 
{
	CComboBox::OnDestroy();
	
}

///////////////////////////////////////////////////////////////////////////////

BOOL CComboBoxSymbol::AddSymbol()
{
   BOOL bOK = FALSE;
   CString sFilter;

   // Select the file

   CImageFile::GetOpenFilterString(sFilter);

   CFileDialog dlg(TRUE,"",NULL,OFN_FILEMUSTEXIST, sFilter);

   if (dlg.DoModal() == IDOK)
   {
      // Load the image file


      CImageFile imagefile;
      if (imagefile.Open(dlg.GetPathName()))
      {
         // Save it to the database

         CBDSymbol symbol;         

         BDNextId(BDHandle(), BDSYMBOL, 0, &symbol.m_lId);
         if (imagefile.AsLongBinary(symbol.m_symbol))
         {
            if (BDSymbol(BDHandle(), &symbol, BDADD))
            {
               bOK = TRUE;
            }
            BDEnd(BDHandle());

         }
      };

   }
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Deletes the currently selected symbol
//

BOOL CComboBoxSymbol::DeleteSymbol()
{
   int index = GetCurSel();
   if (index != CB_ERR)
   {
      DWORD dw = GetItemData(index);

      if (LOWORD(dw) == user)
      {
          CBDSymbol symbol;         
          symbol.m_lId = HIWORD(dw);
          if (BDSymbol(BDHandle(), &symbol, BDDELETE))
          {
             return TRUE;
          }
      }
   }
   return FALSE;
}
