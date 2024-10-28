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
#include "DlgImportRename.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgImportRename dialog


CDlgImportRename::CDlgImportRename(LPCSTR sName, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImportRename::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgImportRename)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

   m_sName = sName;
}


void CDlgImportRename::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImportRename)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgImportRename, CDialog)
	//{{AFX_MSG_MAP(CDlgImportRename)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImportRename message handlers

BOOL CDlgImportRename::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetDlgItem(IDC_NAME)->SetWindowText(m_sName);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgImportRename::OnOK() 
{
   CString s;
   GetDlgItem(IDC_NAME)->GetWindowText(s);
   s.TrimRight();
   s.TrimLeft();

   if (s != m_sName)
   { 
      m_sName = s;
	   CDialog::OnOK();
   };
}

