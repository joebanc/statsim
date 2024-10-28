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
#include "DlgImageOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgImageOptions dialog


CDlgImageOptions::CDlgImageOptions(BOOL bQuality, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImageOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgImageOptions)
	//}}AFX_DATA_INIT

   m_bQuality = bQuality;
}


void CDlgImageOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImageOptions)
	DDX_Control(pDX, IDC_QUALITY, m_sQuality);
	DDX_Control(pDX, IDC_SLIDER, m_ctrlSlider);
	DDX_Control(pDX, IDC_WIDTH, m_eWidth);
	DDX_Control(pDX, IDC_HEIGHT, m_eHeight);
	DDX_Control(pDX, IDC_COLOUR, m_ckColour);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgImageOptions, CDialog)
	//{{AFX_MSG_MAP(CDlgImageOptions)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER, OnCustomdrawSlider)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImageOptions message handlers

BOOL CDlgImageOptions::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Retrieve previous default values

   m_nWidth = AfxGetApp()->GetProfileInt("Images","Width",1024);
   m_nHeight = AfxGetApp()->GetProfileInt("Images","Height",768);
   m_nQuality = AfxGetApp()->GetProfileInt("Images","Quality",50);
   m_bColour = AfxGetApp()->GetProfileInt("Images","Colour",TRUE);

   // Set defaults

   m_eWidth.SetValue(m_nWidth);
   m_eHeight.SetValue(m_nHeight);   
   m_ckColour.SetCheck(m_bColour);

   // Set quality value

   m_ctrlSlider.SetRange(0,100);
   m_ctrlSlider.SetPos(m_nQuality);

   LRESULT lresult;   
   OnCustomdrawSlider(NULL,&lresult);

   // Disable slider for png images

   m_ctrlSlider.EnableWindow(m_bQuality);

   // Disable as need alter how png are stored in memory for b&w

   m_ckColour.EnableWindow(m_bQuality); 
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

void CDlgImageOptions::OnOK() 
{
	// Retrieve and validate values

   if (m_eWidth.GetValue(m_nWidth, 1) && 
       m_eHeight.GetValue(m_nHeight,1))
   {
       m_bColour = m_ckColour.GetCheck();

       m_nQuality = m_ctrlSlider.GetPos();

      // Save values to registry

       AfxGetApp()->WriteProfileInt("Images","Width",m_nWidth);
       AfxGetApp()->WriteProfileInt("Images","Height",m_nHeight);
       AfxGetApp()->WriteProfileInt("Images","Quality",m_nQuality);
       AfxGetApp()->WriteProfileInt("Images","Colour",m_bColour);

      // Okay

      CDialog::OnOK();
   }
	
	
}

///////////////////////////////////////////////////////////////////////////////

void CDlgImageOptions::OnCustomdrawSlider(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString s;
   s.Format("%i %%", m_ctrlSlider.GetPos());
   m_sQuality.SetWindowText(s);
   m_ctrlSlider.SetRange(0,100);
	
	*pResult = 0;
}
