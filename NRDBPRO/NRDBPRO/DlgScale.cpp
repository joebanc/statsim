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
#include "DlgScale.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgScale dialog


CDlgScale::CDlgScale(int nScale,  CWnd* pParent /*=NULL*/)
	: CDialog(CDlgScale::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgScale)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

   m_nScale = nScale;
}


void CDlgScale::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgScale)
	DDX_Control(pDX, IDC_SCALE, m_eScale);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgScale, CDialog)
	//{{AFX_MSG_MAP(CDlgScale)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgScale message handlers

BOOL CDlgScale::OnInitDialog() 
{
	CDialog::OnInitDialog();

   if (m_nScale != 0)
   {
	   m_eScale.SetValue(m_nScale);
   };
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

void CDlgScale::OnOK() 
{
	if (m_eScale.GetValue(m_nScale, 1))
   {
	   CDialog::OnOK();
   };
}
