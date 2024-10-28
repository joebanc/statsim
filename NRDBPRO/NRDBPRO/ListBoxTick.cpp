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
#include "ListBoxTick.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#define TICK_WIDTH 16
#define TICK_HEIGHT 16

/////////////////////////////////////////////////////////////////////////////
// CListBoxTick

CListBoxTick::CListBoxTick()
{
   m_bMultiSel = TRUE;
}

CListBoxTick::~CListBoxTick()
{
}


BEGIN_MESSAGE_MAP(CListBoxTick, CListBox)
	//{{AFX_MSG_MAP(CListBoxTick)
	ON_WM_LBUTTONDOWN()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListBoxTick message handlers

/////////////////////////////////////////////////////////////////////////////

int CListBoxTick::AddString(LPCSTR s)
{
   int index = CListBox::AddString(s);   
   SetItemDataPtr(index, new CListBoxItemData);
   return index;
}

int CListBoxTick::InsertString(int index, LPCSTR s)
{
   index = CListBox::InsertString(index, s);   
   SetItemDataPtr(index, new CListBoxItemData);
   return index;
}

///////////////////////////////////////////////////////////////////////////////

void CListBoxTick::SetItemData(int index, DWORD dw)
{
   CListBoxItemData* pData = (CListBoxItemData*)GetItemDataPtr(index);
   pData->m_dwItemData = dw;
}

DWORD CListBoxTick::GetItemData(int index)
{
   CListBoxItemData* pData = (CListBoxItemData*)GetItemDataPtr(index);
   return pData->m_dwItemData;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CListBoxTick::IsCheck(int index) 
{
   ASSERT(!m_bMultiSel);
   CListBoxItemData* pData = (CListBoxItemData*)GetItemDataPtr(index);
   return pData->m_bCheck;  
}

void CListBoxTick::SetCheck(int index, BOOL bSel) 
{
   ASSERT(!m_bMultiSel);
   CListBoxItemData* pData = (CListBoxItemData*)GetItemDataPtr(index);
   pData->m_bCheck = bSel;
}

/////////////////////////////////////////////////////////////////////////////

void CListBoxTick::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{
   CDC* pDC = CDC::FromHandle(lpDIS->hDC);    

   // Empty control

   if ((long)lpDIS->itemID < 0) return;

   // Get text

   CString string;
   GetText(lpDIS->itemID, string);

   // Determine the rectangles

   CRect rcText = lpDIS->rcItem;
   rcText.left += TICK_WIDTH + 1;
   rcText.top;

   // Paint the background

   COLORREF crBack = RGB(255,255,255);
   COLORREF crText = RGB(0,0,0);
   if (lpDIS->itemState & ODS_FOCUS)
   {
      crBack = RGB(0,0,128);
      crText = RGB(255,255,255);
   }

   CBrush br(crBack);
   CBrush* pbr = pDC->SelectObject(&br);
   pDC->FillRect(&rcText, &br);
   pDC->SelectObject(pbr);
                                                                    
    // Output the text

    pDC->SetBkMode(TRANSPARENT);       
    pDC->SetTextColor(crText);
    pDC->DrawText(string, &rcText, DT_VCENTER|DT_LEFT|DT_NOPREFIX);       

    // Draw selected
        
   CDC dcScreen;    
   CBitmap* pBitmap;
   CBitmap bitmap;

   dcScreen.CreateCompatibleDC(pDC);        
   
   if ((lpDIS->itemState & ODS_SELECTED && m_bMultiSel) || 
       (!m_bMultiSel && IsCheck(lpDIS->itemID)))        
   {
      bitmap.LoadBitmap(IDB_TICK);
   } else
   {
      bitmap.LoadBitmap(IDB_NOTICK);
   }

   pBitmap = dcScreen.SelectObject(&bitmap); 
   pDC->BitBlt(lpDIS->rcItem.left, lpDIS->rcItem.top,TICK_WIDTH,TICK_HEIGHT,
              &dcScreen,0,0,SRCCOPY);    
   dcScreen.SelectObject(pBitmap);
   bitmap.DeleteObject();
}

///////////////////////////////////////////////////////////////////////////////

void CListBoxTick::OnLButtonDown(UINT nFlags, CPoint point) 
{
   // Determine the cell clicked on

   if (GetCount() > 0) 
   {
      int iCell = GetTopIndex() + point.y / GetItemHeight(0);
      if (iCell < GetCount())
      {
	      if (point.x < TICK_WIDTH)
         {
            if (m_bMultiSel)
            {
               SetSel(iCell, !GetSel(iCell));      
            } else
            {  
               SetCheck(iCell, !IsCheck(iCell));
               SetCurSel(iCell);
            };            
            
         } else
         {            
            SetFocus();                      
         }
         if (m_bMultiSel)
         {
            SetCaretIndex(iCell);
         } else
         {
            SetCurSel(iCell);
         }
         GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(),LBN_SELCHANGE), (LPARAM)GetSafeHwnd());
      };
   };	
}

///////////////////////////////////////////////////////////////////////////////
//
// Tidy up memory
//

void CListBoxTick::OnDestroy() 
{
	CListBox::OnDestroy();
	
	int i = 0; for (i = 0; i < GetCount(); i++)
   {
      CListBoxItemData *pData = (CListBoxItemData*)GetItemDataPtr(i);
      if (pData != NULL) delete pData;      
   }

	
}
