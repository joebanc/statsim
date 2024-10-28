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
#include "DlgSearch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSearch dialog


CDlgSearch::CDlgSearch(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSearch::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSearch)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgSearch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSearch)
	DDX_Control(pDX, IDC_SEARCH, m_eSearch);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSearch, CDialog)
	//{{AFX_MSG_MAP(CDlgSearch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSearch message handlers

void CDlgSearch::OnOK() 
{
    m_eSearch.GetWindowText(m_sSearch);
	m_sSearch.TrimLeft();
	m_sSearch.TrimRight();

	if (!m_sSearch.IsEmpty())
	{	
	   CDialog::OnOK();
	};
}