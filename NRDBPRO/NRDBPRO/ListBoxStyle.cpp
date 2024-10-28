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
#include "ListBoxStyle.h"
#include "sheetmapstyle.h"
#include "viewmap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#define COLOR_WIDTH 16
#define COLOR_HEIGHT 16

/////////////////////////////////////////////////////////////////////////////

CListBoxStyleItem::CListBoxStyleItem()
{   
   m_dwItemData = 0;
   m_nHatch = 0;   
}

/////////////////////////////////////////////////////////////////////////////
// CListBoxStyle

CListBoxStyle::CListBoxStyle()
{
   m_nMode = CViewMap::polygon;
}

CListBoxStyle::~CListBoxStyle()
{
}


BEGIN_MESSAGE_MAP(CListBoxStyle, CListBox)
	//{{AFX_MSG_MAP(CListBoxStyle)
	ON_WM_LBUTTONDBLCLK()
   ON_WM_RBUTTONDOWN()   
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListBoxStyle message handlers

void CListBoxStyle::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{	
   CDC* pDC = CDC::FromHandle(lpDIS->hDC);    

   // Empty control

   if ((long)lpDIS->itemID < 0) return;
  
   // Get Item data
   
    CListBoxStyleItem* pItem = &m_aListBoxStyle[lpDIS->itemData];
    CString string = pItem->m_sText.Left(pItem->m_sText.Find('\n'));

   // Determine the rectangles

   CRect rcText = lpDIS->rcItem;
   rcText.left += COLOR_WIDTH + 3;
   rcText.top;

   // Paint the background

   COLORREF crBack = RGB(255,255,255);
   COLORREF crText = RGB(0,0,0);
   if (lpDIS->itemState & ODS_SELECTED)
   {
      crBack = RGB(0,0,128);
      crText = RGB(255,255,255);
   }   

   CBrush br(crBack);
   CBrush* pbr = pDC->SelectObject(&br);
   pDC->FillRect(&rcText, &br);
   
   // If the item has focus then draw a box around it

   if (lpDIS->itemState & ODS_FOCUS)
   {
      CPen pen(PS_DOT, 1, RGB(255,255,255));
      CPen *pPenOld = pDC->SelectObject(&pen);
      pDC->Rectangle(rcText);
      pDC->SelectObject(pPenOld);
   }

   pDC->SelectObject(pbr);
                                                                    
    // Output the text

    pDC->SetBkMode(TRANSPARENT);       
    pDC->SetTextColor(crText);
    pDC->DrawText(string, &rcText, DT_VCENTER|DT_LEFT|DT_NOPREFIX);       

    // Select brush

    CBrush brush;
    CComboBoxPattern::CreateBrush(brush, pItem->m_nPattern,  pItem->m_nHatch, pItem->m_crFill);   
    CBrush* pBrushOld = pDC->SelectObject(&brush);

    CPen pen(pItem->m_nLineStyle, pItem->m_nLineWidth, pItem->m_crLine);
    CPen* pPenOld = pDC->SelectObject(&pen);

    // Draw symbol

    if (m_nMode == CViewMap::points)
    { 
       CComboBoxSymbol::DrawSymbol(pDC, lpDIS->rcItem.left + COLOR_HEIGHT / 2, 
                                   lpDIS->rcItem.top + COLOR_HEIGHT / 2,(CMapStyle&)*pItem);
    } 

    // Draw line

    else if (m_nMode == CViewMap::polyline)
    {
        pDC->MoveTo(lpDIS->rcItem.left+1, (lpDIS->rcItem.top + lpDIS->rcItem.bottom)/2);
        pDC->LineTo(lpDIS->rcItem.left+COLOR_WIDTH-1, (lpDIS->rcItem.top + lpDIS->rcItem.bottom)/2);
    }

    // Draw polygon
    else
    {
      

      POINT aPoints[4];
      aPoints[0].x = lpDIS->rcItem.left +1;
      aPoints[0].y = lpDIS->rcItem.top +1;
      aPoints[1].x = lpDIS->rcItem.left +1;
      aPoints[1].y = lpDIS->rcItem.top + COLOR_HEIGHT -1;
      aPoints[2].x = lpDIS->rcItem.left + COLOR_WIDTH -1;
      aPoints[2].y = lpDIS->rcItem.top + COLOR_HEIGHT -1;
      aPoints[3].x = lpDIS->rcItem.left + COLOR_WIDTH -1;
      aPoints[3].y = lpDIS->rcItem.top +1;         
      pDC->Polygon(aPoints, sizeof(aPoints)/sizeof(POINT));
    } 
    

   // Tidy up

   pDC->SelectObject(pPenOld);
   pDC->SelectObject(pBrushOld);

}

/////////////////////////////////////////////////////////////////////////////

int CListBoxStyle::AddString(LPCSTR str, DWORD dwItemData, CMapStyle mapstyle)
{
   int index = CListBox::AddString("");
  
   CListBoxStyleItem coloritem;
   (CMapStyle&)coloritem = mapstyle;   
   coloritem.m_dwItemData = dwItemData;
   coloritem.m_sText = str;

   int i = m_aListBoxStyle.Add(coloritem);
   
   CListBox::SetItemData(index, i);

   return index;
}

/////////////////////////////////////////////////////////////////////////////

int CListBoxStyle::InsertString(int index, LPCSTR str, DWORD dwItemData, CMapStyle mapstyle)
{
   index = CListBox::InsertString(index, "");
  
   CListBoxStyleItem coloritem;
   (CMapStyle&)coloritem = mapstyle;   
   coloritem.m_dwItemData = dwItemData;
   coloritem.m_sText = str;

   int i = m_aListBoxStyle.Add(coloritem);
   
   CListBox::SetItemData(index, i);

   return index;
}

/////////////////////////////////////////////////////////////////////////////

void CListBoxStyle::ResetContent()
{
   CListBox::ResetContent();
   m_aListBoxStyle.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////

DWORD CListBoxStyle::GetItemData(int index)
{
   CListBoxStyleItem* pItem = &m_aListBoxStyle[CListBox::GetItemData(index)];
   return pItem->m_dwItemData;
}

/////////////////////////////////////////////////////////////////////////////

CMapStyle CListBoxStyle::GetStyle(int index)
{   
   CListBoxStyleItem* pItem = &m_aListBoxStyle[CListBox::GetItemData(index)];
   return *pItem;   
}

 
 
/////////////////////////////////////////////////////////////////////////////

void CListBoxStyle::SetStyle(int index, CMapStyle mapstyle)
{
   CListBoxStyleItem* pItem = &m_aListBoxStyle[CListBox::GetItemData(index)];
   (CMapStyle&)*pItem = mapstyle;
}


/////////////////////////////////////////////////////////////////////////////

void CListBoxStyle::SetText(int index, LPCSTR sText)
{
   CListBoxStyleItem* pItem = &m_aListBoxStyle[CListBox::GetItemData(index)];
   pItem->m_sText = sText;
   Invalidate();
   RedrawWindow();
}

/////////////////////////////////////////////////////////////////////////////

LPCSTR CListBoxStyle::GetText(int index)
{
   CListBoxStyleItem* pItem = &m_aListBoxStyle[CListBox::GetItemData(index)];
   return pItem->m_sText;
}

/////////////////////////////////////////////////////////////////////////////

void CListBoxStyle::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   // Determine the cell clicked on

   if (GetCount() > 0) 
   {
      int iCell = GetTopIndex() + point.y / GetItemHeight(0);
      if (iCell < GetCount())
      {
         // If clicking on the cells color square then produce the color selection
         // dialog

	      if (point.x < COLOR_WIDTH)
         {     
             // Create a dummy object so that the appropriate 
             // properties are enabled

            CMapLayer maplayer;
            CMapLayerObj* pMapObj = new CMapLayerObj;
            pMapObj->SetDataType(m_nMode == CViewMap::points ? BDCOORD : BDMAPLINES);
            maplayer.Add(pMapObj);

            CMapProperties mapprop;

            (CMapStyle&)mapprop = GetStyle(iCell);                        

            mapprop.m_bPolygon = (m_nMode == CViewMap::polygon);
            

            CSheetMapStyle dlg(&maplayer, &mapprop, BDString(IDS_STYLE));            

            if (dlg.DoModal() == IDOK)
            {  
               SetStyle(iCell, mapprop);               
            }
         } else
         {            
            SetFocus();                      
         }
                  
         SetCurSel(iCell);
         Invalidate();
         RedrawWindow();
         GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(),LBN_SELCHANGE), (LPARAM)GetSafeHwnd());
      };
   };		
	
	CListBox::OnLButtonDblClk(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////

void CListBoxStyle::UpDown(int iDir)
{
   CString s;

   int iSwap = -1;
	int i = GetCurSel();
   
   // Determine which items to swap

   if (iDir == -1 && i > 0)
   {
      iSwap = i-1;
      
   }    
   else if (iDir == 1 && i != LB_ERR && i+1 < GetCount())
   {
      iSwap = i+1;
   };

   // Swap the items
   if (iSwap != -1)
   {
      CString s = GetText(i);
      CMapStyle mapstyle = GetStyle(i);      
      DWORD dw = GetItemData(i);

      DeleteString(i);
      InsertString(iSwap, s, dw, mapstyle);      

      SetCurSel(iSwap); // Single selection
      SetCaretIndex(iSwap); // Multiple selection
   };
}

///////////////////////////////////////////////////////////////////////////////
//
// Create properties menu when right click on list
//

void CListBoxStyle::OnRButtonDown(UINT nFlags, CPoint point) 
{
}
