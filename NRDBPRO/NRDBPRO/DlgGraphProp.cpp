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
#include "DlgGraphProp.h"
#include "dlgreport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgGraphProp dialog


CDlgGraphProp::CDlgGraphProp(CGraphProperties* pProp, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgGraphProp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgGraphProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

   m_pProp = pProp;
   m_prop = *pProp;
}


void CDlgGraphProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgGraphProp)
	DDX_Control(pDX, IDC_TICKS, m_eTicks);
	DDX_Control(pDX, IDC_LEGEND, m_eLegend);
	DDX_Control(pDX, IDC_MIN, m_eMin);
	DDX_Control(pDX, IDC_MAX, m_eMax);
	DDX_Control(pDX, IDC_TYPE, m_cbType);
	DDX_Control(pDX, IDC_STYLE, m_cbStyle);
	DDX_Control(pDX, IDC_COLOR, m_pbColour);
	DDX_Control(pDX, IDC_LINE, m_cbLine);
	DDX_Control(pDX, IDC_TITLE, m_sTitle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgGraphProp, CDialog)
	//{{AFX_MSG_MAP(CDlgGraphProp)
	ON_CBN_SELCHANGE(IDC_LINE, OnSelchangeLine)
	ON_BN_CLICKED(IDC_FONT, OnFont)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgGraphProp message handlers

BOOL CDlgGraphProp::OnInitDialog() 
{
	CDialog::OnInitDialog();

   m_sTitle.SetWindowText(m_prop.m_sTitle);

   // Fill the list of layers

   int i = 0; for (i = 0; i < m_prop.m_aLegendText.GetSize(); i++)
   {
      int index = m_cbLine.AddString(m_prop.m_aLegendText[i]);
      m_cbLine.SetItemData(index, i);
   }

   // Initialise sticks/lines

   if (m_prop.m_nType == CDlgReport::Histogram) 
   {
      int index = m_cbType.AddString(BDString(IDS_LINES));
      m_cbType.SetItemData(index, CGraphProperties::lines);
	   index = m_cbType.AddString(BDString(IDS_COLUMNS));
      m_cbType.SetItemData(index, CGraphProperties::columns);
      index = m_cbType.AddString(BDString(IDS_SYMBOLS));
      m_cbType.SetItemData(index, CGraphProperties::symbols);
   } 
   else if (m_prop.m_nType == CDlgReport::Timeseries)
   {
      int index = m_cbType.AddString(BDString(IDS_STICKS));
      m_cbType.SetItemData(index, CGraphProperties::sticks);
      index = m_cbType.AddString(BDString(IDS_LINES));
      m_cbType.SetItemData(index, CGraphProperties::lines);
      index = m_cbType.AddString(BDString(IDS_SYMBOLS));
      m_cbType.SetItemData(index, CGraphProperties::symbols);
   } else
   {
	  m_cbType.EnableWindow(FALSE);
   }

   // Set max/min

   if (m_prop.m_dYMin != m_prop.m_dYMax)
   {
      m_eMin.SetValue(m_prop.m_dYMin);
      m_eMax.SetValue(m_prop.m_dYMax);
      m_eTicks.SetValue(m_prop.m_nYPoints);
   } else
   {
      m_eMin.EnableWindow(FALSE);
      m_eMax.EnableWindow(FALSE);
      m_eTicks.EnableWindow(FALSE);
   }

   // Initialise line style

   m_cbStyle.Initialise(PS_SOLID, 1);

   m_cbLine.SetCurSel(0);
   m_iPrev = CB_ERR;
	OnSelchangeLine();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

void CDlgGraphProp::OnSelchangeLine() 
{
   // Save Previous selection

  
   if (m_iPrev != CB_ERR)
   {
      m_prop.m_aColour[m_iPrev] = m_pbColour.GetColour();
      int iStyle = m_cbStyle.GetCurSel();
      m_prop.m_aLineStyle[m_iPrev] = m_cbStyle.GetStyle(iStyle);
      m_prop.m_aLineWidth[m_iPrev] = m_cbStyle.GetWidth(iStyle);
	  m_prop.m_aGraphStyle[m_iPrev] = m_cbType.GetItemData(m_cbType.GetCurSel());
      m_eLegend.GetWindowText(m_prop.m_aLegendText[m_iPrev]);      
   };
  
   // Select new selection

   int index = m_cbLine.GetCurSel();

   if (index != CB_ERR)
   {
      int i  = m_cbLine.GetItemData(index);

      // Set the legend text

      m_eLegend.SetWindowText(m_prop.m_aLegendText[i]);      

      // Set line style

      int j = 0; for (j = 0; j < m_cbStyle.GetCount(); j++)
      {
         if (m_cbStyle.GetStyle(j) == m_prop.m_aLineStyle[i] && 
             m_cbStyle.GetWidth(j) == m_prop.m_aLineWidth[i])
         {
            m_cbStyle.SetCurSel(j);
            break;
         }
      }

	  // Set type of graph for line

      for (j = 0; j < m_cbType.GetCount(); j++)
		{
	      if ((int)m_cbType.GetItemData(j) == m_prop.m_aGraphStyle[i])
         {
            m_cbType.SetCurSel(j);
            break;
         }
      }      

      // Set color

      m_pbColour.SetColour(m_prop.m_aColour[i]);


      // Save previous selection

      m_iPrev = i;
   };

}

/////////////////////////////////////////////////////////////////////////////

void CDlgGraphProp::OnFont() 
{
   LOGFONT lf = m_prop.m_logfont;
	CFontDialog dlg(&lf, CF_SCREENFONTS);
   
   if (dlg.DoModal() == IDOK)
   {          
       m_prop.m_logfont = lf;
   };   		
}

/////////////////////////////////////////////////////////////////////////////

void CDlgGraphProp::OnOK() 
{
   // Retrieve selections

	m_sTitle.GetWindowText(m_prop.m_sTitle);
   
   // Save max and minimum scale

   if (!m_eMin.GetValue(m_prop.m_dYMin)) return;
   if (!m_eMax.GetValue(m_prop.m_dYMax, m_prop.m_dYMin)) return;
   if (!m_eTicks.GetValue(m_prop.m_nYPoints, 0)) return;

   // Save line settings

   OnSelchangeLine();

   *m_pProp = m_prop;	
	CDialog::OnOK();
}
