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
#include "GridCombo.h"
#include "nrdbgrid.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGridCombo

CGridCombo::CGridCombo()
{
   m_iDefault = -1;
}

CGridCombo::~CGridCombo()
{
}


BEGIN_MESSAGE_MAP(CGridCombo, CComboBox)
	//{{AFX_MSG_MAP(CGridCombo)
	ON_WM_KEYDOWN()
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
	ON_WM_KEYUP()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CGridCombo::SetCurSel(int index)
{
   m_iDefault = index;
   CComboBox::SetCurSel(index);
}

/////////////////////////////////////////////////////////////////////////////
// CGridCombo message handlers

void CGridCombo::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{      
   CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

///////////////////////////////////////////////////////////////////////////////

void CGridCombo::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{	
   CComboBox::OnKeyUp(nChar, nRepCnt, nFlags);
}

///////////////////////////////////////////////////////////////////////////////

void CGridCombo::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	  if (nChar == VK_RETURN || nChar == VK_ESCAPE || nChar == VK_TAB)
   {
   } else
   {
	   CComboBox::OnChar(nChar, nRepCnt, nFlags);
   };
}

///////////////////////////////////////////////////////////////////////////////

void CGridCombo::OnCloseup() 
{
   // Send rather than post so has immediate effect 
   GetParent()->PostMessage(WM_SETEDITMODE, FALSE);		
}


///////////////////////////////////////////////////////////////////////////////

BOOL CGridCombo::PreTranslateMessage(MSG* pMsg) 
{
  // Necessary for keys such as tab      
   
   UINT& nChar = pMsg->wParam;

   if (pMsg->message == WM_KEYDOWN)      
   {  
      // When escaping, restore the previous selection

      if (nChar == VK_ESCAPE)
      {
         SetCurSel(m_iDefault);
      }

      if (nChar == VK_TAB || nChar == VK_RETURN || nChar == VK_ESCAPE || 
          nChar == VK_LEFT || nChar == VK_RIGHT || nChar == VK_SHIFT)
      {	               
         //GetParent()->SetFocus();         
         return TRUE;
      };
   } 
    
   else if (pMsg->message == WM_KEYUP)
   {  
      if (nChar == VK_SHIFT)
      {
         GetParent()->SetFocus();
         return TRUE;
      };                 
   }; 
   
	
	return CComboBox::PreTranslateMessage(pMsg);
}
