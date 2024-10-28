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
#include "dlgmapgrid.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgMapGrid dialog


CDlgMapGrid::CDlgMapGrid(CMapGrid mapgrid, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMapGrid::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMapGrid)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

   m_mapgrid = mapgrid;
}


void CDlgMapGrid::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMapGrid)
	DDX_Control(pDX, IDC_SECLNG, m_eSecLng);
	DDX_Control(pDX, IDC_SECLAT, m_eSecLat);
	DDX_Control(pDX, IDC_MINLNG, m_eMinLng);
	DDX_Control(pDX, IDC_MINLAT, m_eMinLat);
	DDX_Control(pDX, IDC_DEGLNG, m_eDegLng);
	DDX_Control(pDX, IDC_DEGLAT, m_eDegLat);
	DDX_Control(pDX, IDC_COLOR, m_pbColour);
	DDX_Control(pDX, IDC_APPEARANCE, m_cbAppearance);
	DDX_Control(pDX, IDC_LINESTYLE, m_cbLineStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMapGrid, CDialog)
	//{{AFX_MSG_MAP(CDlgMapGrid)
	ON_BN_CLICKED(IDC_FONT, OnFont)
	ON_CBN_SELCHANGE(IDC_APPEARANCE, OnSelchangeAppearance)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMapGrid message handlers

BOOL CDlgMapGrid::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Initialise...
   
   // line style
   
   m_cbLineStyle.Initialise(m_mapgrid.m_style.m_nLineStyle, m_mapgrid.m_style.m_nLineWidth);

   // Type
   
   m_cbAppearance.AddStringX(BDString(IDS_DEFAULT), CMapGrid::defaultgrid);
   m_cbAppearance.AddStringX(BDString(IDS_LABELSANDGRID), CMapGrid::labelsgrid);
   m_cbAppearance.AddStringX(BDString(IDS_LABELS), CMapGrid::labels);
   m_cbAppearance.AddStringX(BDString(IDS_NONE), CMapGrid::none);

   int i = 0; for (i = 0; i < m_cbAppearance.GetCount(); i++)
   {
      if ((int)m_cbAppearance.GetItemData(i) == m_mapgrid.m_nType) m_cbAppearance.SetCurSel(i);
   }   

   // Colour

   m_pbColour.SetColour(m_mapgrid.m_style.m_crLine);

   // Grid

   m_eDegLat.SetValue(m_mapgrid.m_nDegLat);
   m_eMinLat.SetValue(m_mapgrid.m_nMinLat);
   m_eSecLat.SetValue(m_mapgrid.m_nSecLat);
   m_eDegLng.SetValue(m_mapgrid.m_nDegLng);
   m_eMinLng.SetValue(m_mapgrid.m_nMinLng);
   m_eSecLng.SetValue(m_mapgrid.m_nSecLng);

   // Enable controls

   OnSelchangeAppearance();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////

void CDlgMapGrid::OnFont() 
{
   LOGFONT lf = m_mapgrid.m_logfont;
	CFontDialog dlg(&lf, CF_SCREENFONTS);
   
   if (dlg.DoModal() == IDOK)
   {          
       m_mapgrid.m_logfont = lf;
   };   	
	
}

///////////////////////////////////////////////////////////////////////////////

void CDlgMapGrid::OnSelchangeAppearance() 
{
   int index = m_cbAppearance.GetCurSel();
   DWORD dw = m_cbAppearance.GetItemData(index);

   BOOL bEnable = dw != CMapGrid::defaultgrid && dw != CMapGrid::none;

	m_eDegLat.EnableWindow(bEnable);
   m_eMinLat.EnableWindow(bEnable);
   m_eSecLat.EnableWindow(bEnable);
   m_eDegLng.EnableWindow(bEnable);
   m_eMinLng.EnableWindow(bEnable);
   m_eSecLng.EnableWindow(bEnable);
	
}

///////////////////////////////////////////////////////////////////////////////

void CDlgMapGrid::OnOK() 
{
   // Line Style

   int iLine = m_cbLineStyle.GetCurSel();
   m_mapgrid.m_style.m_nLineStyle = m_cbLineStyle.GetStyle(iLine);
   m_mapgrid.m_style.m_nLineWidth = m_cbLineStyle.GetWidth(iLine);

   // Type

   m_cbAppearance.GetItemDataX((long&)m_mapgrid.m_nType);

   // Colour
   
   m_mapgrid.m_style.m_crLine = m_pbColour.GetColour();

   // Get Values

   if (m_eDegLat.GetValue(m_mapgrid.m_nDegLat, 0, 89) &&
       m_eMinLat.GetValue(m_mapgrid.m_nMinLat, 0, 59) &&
       m_eSecLat.GetValue(m_mapgrid.m_nSecLat, 0, 59) && 
       m_eDegLng.GetValue(m_mapgrid.m_nDegLng, 0, 179) && 
       m_eMinLng.GetValue(m_mapgrid.m_nMinLng, 0, 59) && 
       m_eSecLng.GetValue(m_mapgrid.m_nSecLng, 0, 59))
   {

      CDialog::OnOK();
   }	
}

