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
#include "nrdbpro.h"
#include "sheetmapstyle.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSheetMapStyle

IMPLEMENT_DYNAMIC(CSheetMapStyle, CPropertySheet)

CSheetMapStyle::CSheetMapStyle(CMapLayer *pMapLayer, CMapProperties* pMapProp, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
   m_pMapProp = pMapProp;
   m_pMapLayer = pMapLayer;

   // Create a style page

   m_pMapLayer->GetMapProp() = *m_pMapProp;
   m_pMapStyleProp = new CPageMapStyleProp(m_pMapLayer, m_pMapProp);	
   AddPage(m_pMapStyleProp);
}

CSheetMapStyle::~CSheetMapStyle()
{
   if (m_pMapStyleProp != NULL) delete m_pMapStyleProp;      
}


BEGIN_MESSAGE_MAP(CSheetMapStyle, CPropertySheet)
	//{{AFX_MSG_MAP(CSheetMapStyle)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSheetMapStyle message handlers
