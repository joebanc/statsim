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
#include "nrdbpro.h"
#include "PageReportDetails.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageReportDetails property page

IMPLEMENT_DYNCREATE(CPageReportDetails, CPropertyPage)

CPageReportDetails::CPageReportDetails() : CPropertyPage(CPageReportDetails::IDD)
{
	//{{AFX_DATA_INIT(CPageReportDetails)
	//}}AFX_DATA_INIT
}

CPageReportDetails::~CPageReportDetails()
{
}

void CPageReportDetails::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageReportDetails)
	DDX_Control(pDX, IDC_ADDRESS, m_eAddress);
	DDX_Control(pDX, IDC_TELEPHONE, m_eTelephone);
	DDX_Control(pDX, IDC_PROVINCE, m_eProvince);
	DDX_Control(pDX, IDC_ORGANIZATION, m_eOrganization);
	DDX_Control(pDX, IDC_LOGO, m_eLogo);
	DDX_Control(pDX, IDC_FAX, m_eFax);
	DDX_Control(pDX, IDC_EMAIL, m_eEmail);
	DDX_Control(pDX, IDC_COUNTRY, m_eCountry);
	DDX_Control(pDX, IDC_CITY, m_eCity);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageReportDetails, CPropertyPage)
	//{{AFX_MSG_MAP(CPageReportDetails)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageReportDetails message handlers

BOOL CPageReportDetails::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CDialog::OnInitDialog();
	      
	m_eOrganization.SetWindowText(m_pSettings->m_Organization);
	m_eAddress.SetWindowText(m_pSettings->m_Address);
	m_eCity.SetWindowText(m_pSettings->m_City);
	m_eProvince.SetWindowText(m_pSettings->m_Province);
	m_eCountry.SetWindowText(m_pSettings->m_Country);
	m_eTelephone.SetWindowText(m_pSettings->m_Telephone);
	m_eFax.SetWindowText(m_pSettings->m_Fax);
	m_eEmail.SetWindowText(m_pSettings->m_Email);
	m_eLogo.SetWindowText(m_pSettings->m_Logo);	

    m_eTelephone.SetLimitText(254);
	m_eProvince.SetLimitText(254);
	m_eOrganization.SetLimitText(254);
	m_eLogo.SetLimitText(254);
	m_eFax.SetLimitText(254);
	m_eEmail.SetLimitText(254);
	m_eCountry.SetLimitText(254);
	m_eCity.SetLimitText(254);
	m_eAddress.SetLimitText(254);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////

void CPageReportDetails::OnBrowse() 
{
   CString sFilter;
   CImageFile::GetOpenFilterString(sFilter);
   
   CFileDialog dlg(TRUE,"",NULL,OFN_FILEMUSTEXIST, sFilter);

   if (dlg.DoModal() == IDOK)
   {
       m_eLogo.SetWindowText(dlg.GetPathName());
   }	
}

///////////////////////////////////////////////////////////////////////////////

BOOL CPageReportDetails::OnKillActive() 
{
   m_eOrganization.GetWindowText(m_pSettings->m_Organization);
   m_eAddress.GetWindowText(m_pSettings->m_Address);
   m_eCity.GetWindowText(m_pSettings->m_City);
   m_eProvince.GetWindowText(m_pSettings->m_Province);
   m_eCountry.GetWindowText(m_pSettings->m_Country);
   m_eTelephone.GetWindowText(m_pSettings->m_Telephone);
   m_eFax.GetWindowText(m_pSettings->m_Fax);
   m_eEmail.GetWindowText(m_pSettings->m_Email);
   m_eLogo.GetWindowText(m_pSettings->m_Logo);   
   	
	return CPropertyPage::OnKillActive();
}

///////////////////////////////////////////////////////////////////////////////

void CPageReportDetails::OnOK() 
{
   if (BDMain(BDHandle(), m_pSettings, BDUPDATE))	
   {     
      CPropertyPage::OnOK();
   }   			
}
