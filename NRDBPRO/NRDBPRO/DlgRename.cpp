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
#include "DlgRename.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRename dialog


CDlgRename::CDlgRename(LPCSTR sText, CRect rect, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRename::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRename)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

   m_sText = sText;
   m_rect = rect;
}


void CDlgRename::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRename)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRename, CDialog)
	//{{AFX_MSG_MAP(CDlgRename)
	ON_WM_KILLFOCUS()
	ON_WM_CLOSE()
	ON_EN_KILLFOCUS(IDC_EDIT1, OnKillfocusEdit1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRename message handlers

BOOL CDlgRename::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   GetDlgItem(IDC_EDIT1)->SetWindowText(m_sText);

   SetWindowPos(NULL, m_rect.left, m_rect.top, m_rect.Width()+1, m_rect.Height()+1, SWP_NOZORDER);
   GetDlgItem(IDC_EDIT1)->SetWindowPos(NULL,0,0, m_rect.Width(), m_rect.Height(), SWP_NOZORDER|SWP_NOMOVE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////

void CDlgRename::OnKillFocus(CWnd* pNewWnd) 
{
	CDialog::OnKillFocus(pNewWnd);
		
}

///////////////////////////////////////////////////////////////////////////////

void CDlgRename::OnClose() 
{

	
	CDialog::OnClose();
}

///////////////////////////////////////////////////////////////////////////////

void CDlgRename::OnKillfocusEdit1() 
{

}

///////////////////////////////////////////////////////////////////////////////

void CDlgRename::OnOK() 
{
   GetDlgItem(IDC_EDIT1)->GetWindowText(m_sText);
	
	CDialog::OnOK();
}

