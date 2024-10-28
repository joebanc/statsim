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
#include "DlgImportDate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgImportDate dialog


CDlgImportDate::CDlgImportDate(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImportDate::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgImportDate)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgImportDate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImportDate)
	DDX_Control(pDX, IDC_DATE, m_eDate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgImportDate, CDialog)
	//{{AFX_MSG_MAP(CDlgImportDate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImportDate message handlers

BOOL CDlgImportDate::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CDateTime date;
   CString sDate;

   date.AsSystemDate();
   date.DateAsString(sDate);
   m_eDate.SetWindowText(sDate);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

void CDlgImportDate::OnOK() 
{
   CString sDate;   

   m_eDate.GetWindowText(sDate);
   if (m_date.StringAsDate(sDate))
   { 	
	   CDialog::OnOK();
   } else
   {
      AfxMessageBox(BDString(IDS_DATEFORMAT));
   }
}
