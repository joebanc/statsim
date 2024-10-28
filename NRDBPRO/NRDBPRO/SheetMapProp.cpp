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
#include "SheetMapProp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

int CSheetMapProp::m_nDefaultPage = CSheetMapProp::layer;

/////////////////////////////////////////////////////////////////////////////
// CSheetMapProp

IMPLEMENT_DYNAMIC(CSheetMapProp, CPropertySheet)

CSheetMapProp::CSheetMapProp(CMapLayerArray* pMapLayerArray, CMapLayer* pMapLayer, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
   m_pMapLayer = pMapLayer;
   m_pMapLayerArray = pMapLayerArray;
   m_mapprop = CMapProperties(pMapLayer->GetMapProp());

   // Determine data type

   m_pMapLayerProp = new CPageMapLayerProp(m_pMapLayer, &m_mapprop);	
   m_pMapStyleProp = new CPageMapStyleProp(m_pMapLayer, &m_mapprop);	
   m_pMapTextProp = new CPageMapTextProp(m_pMapLayer, &m_mapprop);	
   m_pMapLegendProp = new CPageMapLegendProp(m_pMapLayerArray, m_pMapLayer, &m_mapprop);

   // Determine if layer contains images

   BOOL bImage = FALSE;
   int i = 0; for (i = 0; i < pMapLayer->GetSize(); i++)
   {
      if (pMapLayer->GetAt(i)->GetDataType() == BDIMAGE)
      {
         bImage = TRUE;
         break;
      }
   };
 
   // For legends, do not include layer and style controls
   
   if (pMapLayer->GetSize() != 0 && !bImage)

   {
	   AddPage(m_pMapLayerProp);
	   AddPage(m_pMapStyleProp);
   };
   
   AddPage(m_pMapTextProp);   
   AddPage(m_pMapLegendProp);
   

   // Legend or no data

   if (pMapLayer->GetSize() == 0) 
   {      
      SetActivePage(m_pMapLegendProp);
   } else
   {
      switch (m_nDefaultPage)
      {
         case layer : SetActivePage(m_pMapLayerProp); break;
         case style : SetActivePage(m_pMapStyleProp); break;
         case text : SetActivePage(m_pMapTextProp); break;
         case legend : SetActivePage(m_pMapLegendProp); break;      
      };
    };     
}

///////////////////////////////////////////////////////////////////////////////

CSheetMapProp::~CSheetMapProp()
{
   if (m_pMapLayerProp != NULL) delete m_pMapLayerProp;      
   if (m_pMapStyleProp != NULL) delete m_pMapStyleProp;      
   if (m_pMapTextProp != NULL) delete m_pMapTextProp;      
   if (m_pMapLegendProp != NULL) delete m_pMapLegendProp;

}


BEGIN_MESSAGE_MAP(CSheetMapProp, CPropertySheet)
	//{{AFX_MSG_MAP(CSheetMapProp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSheetMapProp message handlers
