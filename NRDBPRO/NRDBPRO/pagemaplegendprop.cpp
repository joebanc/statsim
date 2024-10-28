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
#include "PageMapLegendProp.h"
#include "sheetmapprop.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageMapLegendProp property page

IMPLEMENT_DYNCREATE(CPageMapLegendProp, CPropertyPage)

CPageMapLegendProp::CPageMapLegendProp(CMapLayerArray* pMapLayerArray, CMapLayer* pMapLayer, CMapProperties* pMapProp) : 
    CPropertyPage(CPageMapLegendProp::IDD)
{
   m_pMapLayer = pMapLayer;
   m_pMapLayerArray = pMapLayerArray;
   m_pMapProperty = pMapProp;

	//{{AFX_DATA_INIT(CPageMapLegendProp)
	//}}AFX_DATA_INIT
}

CPageMapLegendProp::CPageMapLegendProp()
{
}

CPageMapLegendProp::~CPageMapLegendProp()
{
}

void CPageMapLegendProp::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageMapLegendProp)
	DDX_Control(pDX, IDC_SHOWLAYERNAME, m_ckShowLayerName);
	DDX_Control(pDX, IDC_TITLE, m_eTitle);
	DDX_Control(pDX, IDC_COMMENT, m_eComment);
	//}}AFX_DATA_MAP

    DDX_Control(pDX, IDC_LAYERNAME, m_eLayerName);

}


BEGIN_MESSAGE_MAP(CPageMapLegendProp, CPropertyPage)
	//{{AFX_MSG_MAP(CPageMapLegendProp)
	ON_BN_CLICKED(IDC_FONT1, OnFont1)
	ON_BN_CLICKED(IDC_FONT2, OnFont2)
	ON_BN_CLICKED(IDC_SAMEASLABEL, OnSameaslabel)
	ON_BN_CLICKED(IDC_SINGLEFONT, OnSinglefont)
	ON_BN_CLICKED(IDC_FONTLEGEND, OnFontlegend)
	ON_BN_CLICKED(IDC_SHOWLAYERNAME, OnShowlayername)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageMapLegendProp message handlers

BOOL CPageMapLegendProp::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

   m_eTitle.SetWindowText(m_pMapLayerArray->m_sTitle);	
   m_eLayerName.SetWindowText(m_pMapLayer->GetName());   

   m_eComment.SetWindowText(m_pMapProperty->m_sComment);

   m_ckShowLayerName.SetCheck(m_pMapProperty->m_bShowLayerName);

   int nID = IDC_SINGLEFONT;
   if (!m_pMapLayerArray->m_bLegend1Font) nID = IDC_SAMEASLABEL;
   CheckRadioButton(IDC_SINGLEFONT, IDC_SAMEASLABEL, nID);

   OnSinglefont();   
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////

BOOL CPageMapLegendProp::OnSetActive() 
{
   // Set this as the default page

   CSheetMapProp *pSheet = (CSheetMapProp*)GetParent();
   ASSERT(pSheet->IsKindOf(RUNTIME_CLASS(CSheetMapProp)));
   pSheet->m_nDefaultPage = CSheetMapProp::legend;

   OnShowlayername();
	
	return CPropertyPage::OnSetActive();
}

/////////////////////////////////////////////////////////////////////////////

BOOL CPageMapLegendProp::OnKillActive() 
{
   m_eTitle.GetWindowText(m_pMapLayerArray->m_sTitle);

   m_pMapProperty->m_bShowLayerName = m_ckShowLayerName.GetCheck();
   
   m_eLayerName.GetWindowText(m_pMapProperty->m_sName);
   if (m_pMapLayer->GetSize() == 0 && m_pMapProperty->m_sName == "")
   {
      m_eLayerName.SetFocus();
      return FALSE;         
   }

   CString& sComment = m_pMapProperty->m_sComment;

	m_eComment.GetWindowText(sComment);   
   
   int nID = GetCheckedRadioButton(IDC_SINGLEFONT, IDC_SAMEASLABEL);
   m_pMapLayerArray->m_bLegend1Font = nID == IDC_SINGLEFONT;

	return CPropertyPage::OnKillActive();
}

/////////////////////////////////////////////////////////////////////////////

void CPageMapLegendProp::OnOK() 
{
   m_pMapLayer->GetMapProp() = *m_pMapProperty;
	
	CPropertyPage::OnOK();
}

/////////////////////////////////////////////////////////////////////////////

void CPageMapLegendProp::OnFont1() 
{
   LOGFONT lf = m_pMapLayerArray->m_logfontT;
	CFontDialog dlg(&lf, CF_SCREENFONTS);
   
   if (dlg.DoModal() == IDOK)
   {          
       m_pMapLayerArray->m_logfontT = lf;
   };   		
}

/////////////////////////////////////////////////////////////////////////////

void CPageMapLegendProp::OnFont2() 
{
	LOGFONT lf = m_pMapProperty->m_logfontC;
	CFontDialog dlg(&lf, CF_SCREENFONTS);
   
   if (dlg.DoModal() == IDOK)
   {          
       m_pMapProperty->m_logfontC = lf;
   };   		
}

///////////////////////////////////////////////////////////////////////////////
//
// Set the font for the legend
//

void CPageMapLegendProp::OnFontlegend() 
{
   LOGFONT lf = m_pMapLayerArray->m_logfontL;
   CFontDialog dlg(&lf, CF_SCREENFONTS);
   
   if (dlg.DoModal() == IDOK)
   {          
       m_pMapLayerArray->m_logfontL = lf;
   };   		

}

///////////////////////////////////////////////////////////////////////////////
//
// Disable font button if selected, legend font same a label
//

void CPageMapLegendProp::OnSameaslabel() 
{
   OnSinglefont();	
}

void CPageMapLegendProp::OnSinglefont() 
{
   int nID = GetCheckedRadioButton(IDC_SINGLEFONT, IDC_SAMEASLABEL);
	GetDlgItem(IDC_FONTLEGEND)->EnableWindow(nID == IDC_SINGLEFONT);	
}

///////////////////////////////////////////////////////////////////////////////

void CPageMapLegendProp::OnShowlayername() 
{
   // Layer name may be disabled for separate color or colour range, unless
   // overidden

   m_eLayerName.EnableWindow(!m_ckShowLayerName.IsWindowEnabled() || m_ckShowLayerName.GetCheck());		
}
