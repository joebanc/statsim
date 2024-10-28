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
#include "BDComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBDComboBox

CBDComboBox::CBDComboBox()
{
}

CBDComboBox::~CBDComboBox()
{
}


BEGIN_MESSAGE_MAP(CBDComboBox, CComboBox)
	//{{AFX_MSG_MAP(CBDComboBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CBDComboBox::SaveFTypes()
{
   long lFTypeSel;
   
   if (GetItemDataX(lFTypeSel) != CB_ERR)   
   {      
      BDFTypeSel(lFTypeSel);
   }
   
}

/////////////////////////////////////////////////////////////////////////////

int CBDComboBox::AddStringX(LPCSTR lpstr, DWORD dwData)
{
   int index = CComboBox::AddString(lpstr);
   SetItemData(index, dwData);
   return index;
}

/////////////////////////////////////////////////////////////////////////////

int CBDComboBox::GetItemDataX(long& lValue)
{
   int index = GetCurSel();
   if (index != CB_ERR)
   {
      lValue = (long)GetItemData(index);
   }
   return index;
}

