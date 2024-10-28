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
#include "GridEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGridEdit

CGridEdit::CGridEdit()
{
}

CGridEdit::~CGridEdit()
{
}


BEGIN_MESSAGE_MAP(CGridEdit, CEdit)
	//{{AFX_MSG_MAP(CGridEdit)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//
// Store default so that it can be reset if escape is pressed

void CGridEdit::SetWindowText(LPCSTR lpszString)
{
   m_sDefault = lpszString;
   CEdit::SetWindowText(lpszString);
}

/////////////////////////////////////////////////////////////////////////////
// CGridEdit message handlers

void CGridEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{	
   // Allow parent to handle tab/return key etc.

   if (nChar == VK_TAB || nChar == VK_RETURN || nChar == VK_ESCAPE)
   {
      GetParent()->PostMessage(WM_KEYDOWN, nChar, MAKELPARAM(nRepCnt, nFlags));
   } else
   {
	   CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
   };
}

void CGridEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_SHIFT)
   {
      GetParent()->PostMessage(WM_KEYUP, nChar, MAKELPARAM(nRepCnt, nFlags));
   } else
   {	
	   CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
   };
}

void CGridEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// Remove to prevent annoying beep
	
   if (nChar == VK_RETURN || nChar == VK_ESCAPE || nChar == VK_TAB)
   {
   } else
   {
	   CEdit::OnChar(nChar, nRepCnt, nFlags);
   };
}

///////////////////////////////////////////////////////////////////////////////

void CGridEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	
   // Inform parent if losing focus whilst in edit mode

      GetParent()->PostMessage(WM_SETFOCUS,(WPARAM)this, 0);

}

///////////////////////////////////////////////////////////////////////////////

BOOL CGridEdit::PreTranslateMessage(MSG* pMsg) 
{	   
   // Necessary for keys such as tab      
   
   if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
   {
	   if (pMsg->message == WM_KEYDOWN)      
      {  
         if (pMsg->wParam == VK_ESCAPE)
         {
            CEdit::SetWindowText(m_sDefault);
         }
         OnKeyDown(pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));                                       
         GetParent()->SetFocus();
         return TRUE;
      } 
      else if (pMsg->message == WM_KEYUP)
      {                
         OnKeyUp(pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));                        
         GetParent()->SetFocus();
         return TRUE;
      };                 
   };       
	
	return CEdit::PreTranslateMessage(pMsg);
}
