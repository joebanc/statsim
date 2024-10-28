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
#include "DlgDeleteFType.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDeleteFType dialog


CDlgDeleteFType::CDlgDeleteFType(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDeleteFType::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDeleteFType)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

   m_bDeleteData = FALSE;
   m_bDeleteFeatures = FALSE;
   m_bDeleteFType = FALSE;
}


void CDlgDeleteFType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDeleteFType)
	DDX_Control(pDX, IDC_DELETEFTYPE, m_ckDeleteFType);
	DDX_Control(pDX, IDC_DELETEFEATURES, m_ckDeleteFeature);
	DDX_Control(pDX, IDC_DELETEDATA, m_ckDeleteData);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDeleteFType, CDialog)
	//{{AFX_MSG_MAP(CDlgDeleteFType)
	ON_BN_CLICKED(IDC_DELETEDATA, OnClickDelete)
	ON_BN_CLICKED(IDC_DELETEFEATURES, OnClickDelete)
	ON_BN_CLICKED(IDC_DELETEFTYPE, OnClickDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDeleteFType message handlers

BOOL CDlgDeleteFType::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   OnClickDelete();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

void CDlgDeleteFType::OnClickDelete() 
{   
   if ((m_ckDeleteFType.GetCheck() && !m_ckDeleteFeature.GetCheck()) ||
        (m_ckDeleteFeature.GetCheck() && !m_ckDeleteData.GetCheck()) ||
        !m_ckDeleteData.GetCheck())
   {
      GetDlgItem(IDOK)->EnableWindow(FALSE);
   } else
   {
      GetDlgItem(IDOK)->EnableWindow(TRUE);
   }   
}

/////////////////////////////////////////////////////////////////////////////

void CDlgDeleteFType::OnOK() 
{
	m_bDeleteData = m_ckDeleteData.GetCheck();
   m_bDeleteFeatures = m_ckDeleteFeature.GetCheck();
   m_bDeleteFType = m_ckDeleteFType.GetCheck();
	
	CDialog::OnOK();
}

