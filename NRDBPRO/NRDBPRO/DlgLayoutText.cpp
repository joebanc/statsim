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
#include "DlgLayoutText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgLayoutText dialog


CDlgLayoutText::CDlgLayoutText(LPCSTR sText, LOGFONT logfont, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLayoutText::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgLayoutText)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

   m_sText = sText;
   m_logfont = logfont;
}


void CDlgLayoutText::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgLayoutText)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgLayoutText, CDialog)
	//{{AFX_MSG_MAP(CDlgLayoutText)
	ON_BN_CLICKED(IDC_FONT, OnFont)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgLayoutText message handlers

BOOL CDlgLayoutText::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetDlgItem(IDC_TEXT)->SetWindowText(m_sText);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////

void CDlgLayoutText::OnFont() 
{
   LOGFONT lf = m_logfont;
	CFontDialog dlg(&lf, CF_SCREENFONTS);
   
   if (dlg.DoModal() == IDOK)
   {          
       m_logfont = lf;
   };   		
	
}

///////////////////////////////////////////////////////////////////////////////

void CDlgLayoutText::OnOK() 
{
   GetDlgItem(IDC_TEXT)->GetWindowText(m_sText);
	
	CDialog::OnOK();
}
