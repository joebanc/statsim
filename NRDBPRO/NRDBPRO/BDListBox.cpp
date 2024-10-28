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
#include "BDListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBDListBox

CBDListBox::CBDListBox()
{
}

CBDListBox::~CBDListBox()
{
}


BEGIN_MESSAGE_MAP(CBDListBox, CListBox)
	//{{AFX_MSG_MAP(CBDListBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBDListBox message handlers

void CBDListBox::SelectAll()
{
   SetRedraw(FALSE);
   BOOL bSelect = GetCount() != GetSelCount();
   int i = 0; for (i = 0; i < GetCount(); i++)
   {
      SetSel(i, bSelect);
   }
   SetRedraw(TRUE);
}

/////////////////////////////////////////////////////////////////////////////

void CBDListBox::UpDown(int iDir)
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
      GetText(i, s);
      DWORD dw = GetItemData(i);

      DeleteString(i);
      InsertString(iSwap, s);
      SetItemData(iSwap, dw);

      SetCurSel(iSwap); // Single selection
      SetCaretIndex(iSwap); // Multiple selection
   };
}
