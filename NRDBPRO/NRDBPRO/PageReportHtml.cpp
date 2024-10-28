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
#include <io.h>

#include "nrdbpro.h"
#include "PageReportHtml.h"
#include "docreport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageReportHtml property page

IMPLEMENT_DYNCREATE(CPageReportHtml, CPropertyPage)

CPageReportHtml::CPageReportHtml() : CPropertyPage(CPageReportHtml::IDD)
{
	//{{AFX_DATA_INIT(CPageReportHtml)
	//}}AFX_DATA_INIT

   m_nLastEdit = header;
}

CPageReportHtml::~CPageReportHtml()
{
}

void CPageReportHtml::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageReportHtml)
	DDX_Control(pDX, IDC_ITEM, m_cbItem);
	DDX_Control(pDX, IDC_HEADER, m_eHeader);
	DDX_Control(pDX, IDC_FOOTER, m_eFooter);
	DDX_Control(pDX, IDC_STYLE, m_cbStyleSheet);
	DDX_Control(pDX, IDC_USEHEADER, m_ckHeader);
	DDX_Control(pDX, IDC_USEFOOTER, m_ckFooter);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageReportHtml, CPropertyPage)
	//{{AFX_MSG_MAP(CPageReportHtml)
	ON_BN_CLICKED(IDC_USEFOOTER, OnUsefooter)
	ON_BN_CLICKED(IDC_USEHEADER, OnUseheader)
	ON_BN_CLICKED(IDC_INSERT, OnInsert)
	ON_EN_CHANGE(IDC_HEADER, OnChangeHeader)
	ON_EN_CHANGE(IDC_FOOTER, OnChangeFooter)
	ON_EN_SETFOCUS(IDC_FOOTER, OnSetfocusFooter)
	ON_EN_SETFOCUS(IDC_HEADER, OnSetfocusHeader)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageReportHtml message handlers

BOOL CPageReportHtml::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// Initialise controls

   m_ckHeader.SetCheck(m_pSettings->m_bCustomHeader);
   m_ckFooter.SetCheck(m_pSettings->m_bCustomFooter);
   m_eHeader.SetWindowText(m_pSettings->m_sCustomHeader);
   m_eFooter.SetWindowText(m_pSettings->m_sCustomFooter);
   m_eHeader.SetSel(m_eHeader.GetWindowTextLength(), m_eHeader.GetWindowTextLength());
   m_eFooter.SetSel(m_eFooter.GetWindowTextLength(), m_eFooter.GetWindowTextLength());

   // Fill list of style sheets

   CDocReport::Initialise();
   int i = 0; for (i = 0; i < CDocReport::m_aReportItem.GetSize(); i++)
   {
      m_cbItem.AddString(CDocReport::m_aReportItem[i].m_sDesc);
   }
   m_cbItem.SetCurSel(0);

   // Add list of available stylesheets

   WIN32_FIND_DATA Buf = {0};         
   HANDLE handle = FindFirstFile (BDGetAppPath() + "*.css", &Buf);
   while (handle != INVALID_HANDLE_VALUE && handle != NULL)
   {
      if (Buf.cFileName[0] != '.')
      {
         int index = m_cbStyleSheet.AddString(Buf.cFileName);
         if (Buf.cFileName == m_pSettings->m_sStyleSheet)
         {
            m_cbStyleSheet.SetCurSel(index);
         }
      };
      
      if (!FindNextFile(handle, &Buf)) break;
   }
   if (m_cbStyleSheet.GetCurSel() == CB_ERR) m_cbStyleSheet.SetCurSel(0);

   // Enable or disable as required

   OnUseheader();
   OnUsefooter();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////

void CPageReportHtml::OnUsefooter() 
{
	m_eFooter.EnableWindow(m_ckFooter.GetCheck());	
}

void CPageReportHtml::OnUseheader() 
{
	m_eHeader.EnableWindow(m_ckHeader.GetCheck());	
}

///////////////////////////////////////////////////////////////////////////////

void CPageReportHtml::OnInsert() 
{
   CString s;
   m_cbItem.GetLBText(m_cbItem.GetCurSel(), s);	

   if (m_nLastEdit == header)
   {
      m_eHeader.ReplaceSel(s, TRUE);
   } else
   {
      m_eFooter.ReplaceSel(s, TRUE);
   }	
}

///////////////////////////////////////////////////////////////////////////////

void CPageReportHtml::OnChangeHeader() 
{
}

void CPageReportHtml::OnChangeFooter() 
{   
}

void CPageReportHtml::OnSetfocusFooter() 
{
	m_nLastEdit = footer;	
	
}

void CPageReportHtml::OnSetfocusHeader() 
{
   m_nLastEdit = header;	
}

///////////////////////////////////////////////////////////////////////////////

void CPageReportHtml::OnBrowse() 
{
   CFileDialog dlg(TRUE, "css", NULL,OFN_FILEMUSTEXIST, "*.css|*.css||");
   if (dlg.DoModal() == IDOK)
   {
      int index = m_cbStyleSheet.AddString(dlg.GetPathName());
      m_cbStyleSheet.SetCurSel(index);
   }
	
}

///////////////////////////////////////////////////////////////////////////////

BOOL CPageReportHtml::OnKillActive() 
{
	// Retrieve values

   m_pSettings->m_bCustomHeader = m_ckHeader.GetCheck();
   if (m_pSettings->m_bCustomHeader)
   {
      m_eHeader.GetWindowText(m_pSettings->m_sCustomHeader);
   }
   m_pSettings->m_bCustomFooter = m_ckFooter.GetCheck();
   if (m_pSettings->m_bCustomFooter)
   {
      m_eFooter.GetWindowText(m_pSettings->m_sCustomFooter);
   }

   // Retrieve the style sheet

   int index = m_cbStyleSheet.GetCurSel();
   if (index != CB_ERR)
   {
      m_cbStyleSheet.GetLBText(index, m_pSettings->m_sStyleSheet);
   }
	
	return CPropertyPage::OnKillActive();
}

///////////////////////////////////////////////////////////////////////////////

void CPageReportHtml::OnOK() 
{
   if (BDMain(BDHandle(), m_pSettings, BDUPDATE))	
   {        
      CPropertyPage::OnOK();
   }   					
}





