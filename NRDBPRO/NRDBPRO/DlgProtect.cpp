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
#include "DlgProtect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgProtect dialog


CDlgProtect::CDlgProtect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProtect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgProtect)
	//}}AFX_DATA_INIT
}


void CDlgProtect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgProtect)
	DDX_Control(pDX, IDC_PROTECT, m_ckProtect);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgProtect, CDialog)
	//{{AFX_MSG_MAP(CDlgProtect)
	ON_BN_CLICKED(IDC_PROTECT, OnProtect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgProtect message handlers

BOOL CDlgProtect::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	OnProtect();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////

void CDlgProtect::OnProtect() 
{
	GetDlgItem(IDC_MESSAGE)->EnableWindow(m_ckProtect.GetCheck());
}

///////////////////////////////////////////////////////////////////////////////

void CDlgProtect::OnOK() 
{
   if (m_ckProtect.GetCheck())
   {
      GetDlgItem(IDC_MESSAGE)->GetWindowText(m_sMessage);
      m_sMessage.TrimLeft();

      if (m_sMessage == "")
      {
         AfxMessageBox(IDS_NOMESSAGE);
         return;
      }
   }
	
	CDialog::OnOK();
}
