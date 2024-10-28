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
#include "PageMapTextProp.h"
#include "sheetmapprop.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageMapTextProp property page

IMPLEMENT_DYNCREATE(CPageMapTextProp, CPropertyPage)

CPageMapTextProp::CPageMapTextProp()
{
}

CPageMapTextProp::CPageMapTextProp(CMapLayer* pMapLayer, CMapProperties* pMapProperty) : CPropertyPage(CPageMapTextProp::IDD)
{
   m_pMapLayer = pMapLayer;
   m_pMapProperty = pMapProperty;
   m_logfont = m_pMapLayer->GetFont();
   
	//{{AFX_DATA_INIT(CPageMapTextProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPageMapTextProp::~CPageMapTextProp()
{
}

void CPageMapTextProp::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageMapTextProp)	
	DDX_Control(pDX, IDC_SCALETEXT, m_ckScaleText);	
	DDX_Control(pDX, IDC_TEXTCOLOR, m_pbTextColour);	
	DDX_Control(pDX, IDC_TEXTPOS, m_cbTextPos);
	DDX_Control(pDX, IDC_FONT, m_pbFont);		
	DDX_Control(pDX, IDC_OVERLAPTEXT, m_ckOverlapText);	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageMapTextProp, CPropertyPage)
	//{{AFX_MSG_MAP(CPageMapTextProp)
	ON_BN_CLICKED(IDC_FONT, OnFont)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageMapTextProp message handlers

BOOL CPageMapTextProp::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// Scan the map objects to determine which properties to display

   BOOL bCoord = FALSE;
   BOOL bMapLines = FALSE;
   int i = 0; for (i = 0; i < m_pMapLayer->GetSize(); i++)
   {
      CMapLayerObj* pMapLayerObj = (CMapLayerObj*)m_pMapLayer->GetAt(i);
      if (pMapLayerObj->GetDataType() == BDCOORD) bCoord = TRUE;
      if (pMapLayerObj->GetDataType() == BDMAPLINES) bMapLines = TRUE;
   }


   // Initialise controls
	
   m_ckOverlapText.SetCheck(m_pMapLayer->GetOverlap());
   m_ckScaleText.SetCheck(m_pMapLayer->GetScaleFont());

   m_pbTextColour.SetColour(m_pMapLayer->GetTextColour());

   // Initialise text position

   m_cbTextPos.AddStringX(BDString(IDS_NONE), CMapLayer::Null);

   m_cbTextPos.AddStringX(BDString(IDS_LEFT), CMapLayer::Left);         
   m_cbTextPos.AddStringX(BDString(IDS_RIGHT), CMapLayer::Right);         
   m_cbTextPos.AddStringX(BDString(IDS_TOP), CMapLayer::Top);         
   m_cbTextPos.AddStringX(BDString(IDS_BOTTOM), CMapLayer::Bottom);         
   m_cbTextPos.AddStringX(BDString(IDS_TOPLEFT), CMapLayer::TopLeft);         
   m_cbTextPos.AddStringX(BDString(IDS_TOPRIGHT), CMapLayer::TopRight);         
   m_cbTextPos.AddStringX(BDString(IDS_BOTTOMLEFT), CMapLayer::BottomLeft);         
   m_cbTextPos.AddStringX(BDString(IDS_BOTTOMRIGHT), CMapLayer::BottomRight);         
   m_cbTextPos.AddStringX(BDString(IDS_CENTER), CMapLayer::Center);
   m_cbTextPos.AddStringX(BDString(IDS_BESTFIT), CMapLayer::BestFit);
   
   for (i = 0; i < m_cbTextPos.GetCount(); i++)
   {
      if ((int)m_cbTextPos.GetItemData(i) == m_pMapLayer->GetTextPos()) 
         m_cbTextPos.SetCurSel(i);
   }   

   if (!bCoord && !bMapLines)
   {
      m_cbTextPos.EnableWindow(FALSE);
      m_ckScaleText.EnableWindow(FALSE);
      m_ckOverlapText.EnableWindow(FALSE);
   }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////

BOOL CPageMapTextProp::OnSetActive() 
{
   // Set this as the default page

   CSheetMapProp *pSheet = (CSheetMapProp*)GetParent();
   ASSERT(pSheet->IsKindOf(RUNTIME_CLASS(CSheetMapProp)));
   pSheet->m_nDefaultPage = CSheetMapProp::text;
	
	return CPropertyPage::OnSetActive();
}

///////////////////////////////////////////////////////////////////////////////

BOOL CPageMapTextProp::OnKillActive() 
{
   int iTextPos = m_cbTextPos.GetCurSel();

   if (iTextPos != CB_ERR)
   {      
      m_pMapProperty->m_bOverlap = m_ckOverlapText.GetCheck();
      m_pMapProperty->m_bScaleFont = m_ckScaleText.GetCheck();
      
      m_pMapProperty->m_nTextPos = m_cbTextPos.GetItemData(iTextPos);
      
      m_pMapProperty->m_colorText = m_pbTextColour.GetColour();

      m_pMapProperty->m_logfont = m_logfont;
      
	   return CPropertyPage::OnKillActive();
   };
	return FALSE;	
}

///////////////////////////////////////////////////////////////////////////////

void CPageMapTextProp::OnFont() 
{
   LOGFONT lf = m_logfont;
	CFontDialog dlg(&lf, CF_SCREENFONTS);
   
   if (dlg.DoModal() == IDOK)
   {          
       m_logfont = lf;
   };   	
	
}

/////////////////////////////////////////////////////////////////////////////////////

void CPageMapTextProp::OnOK() 
{
   m_pMapLayer->GetMapProp() = *m_pMapProperty;
	
	CPropertyPage::OnOK();
}
