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
#include "DlgSelDictionary.h"
#include "dlgaddsector.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSelDictionary dialog


CDlgSelDictionary::CDlgSelDictionary(CDictSelArray* paDictSel, BOOL *pbIncLinks, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelDictionary::IDD, pParent)
{
   m_paDictSel = paDictSel;
   m_pbIncLinks = pbIncLinks;
   m_lMaxId = 0;

	//{{AFX_DATA_INIT(CDlgSelDictionary)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgSelDictionary::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSelDictionary)
	DDX_Control(pDX, IDC_SELECTALL, m_pbSelectAll);
	DDX_Control(pDX, IDC_INCLINKS, m_ckIncLinks);
	DDX_Control(pDX, IDC_DICTIONARIES, m_lbDictionary);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSelDictionary, CDialog)
	//{{AFX_MSG_MAP(CDlgSelDictionary)
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	ON_LBN_SELCHANGE(IDC_DICTIONARIES, OnSelchangeDictionaries)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSelDictionary message handlers

BOOL CDlgSelDictionary::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   InitList();
   	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

void CDlgSelDictionary::InitList()
{
   m_lbDictionary.ResetContent();

   int i = 0; for (i = 0; i < m_paDictSel->GetSize(); i++)
   {
      int index = m_lbDictionary.AddString(m_paDictSel->GetAt(i).m_sDesc);
      m_lbDictionary.SetItemData(index, m_paDictSel->GetAt(i).m_lId);
      m_lbDictionary.SetSel(index, m_paDictSel->GetAt(i).m_bSel);

      m_lMaxId = max(m_paDictSel->GetAt(i).m_lId, m_lMaxId);
   }

   m_ckIncLinks.SetCheck(*m_pbIncLinks);

   OnSelchangeDictionaries();
}

/////////////////////////////////////////////////////////////////////////////

void CDlgSelDictionary::OnOK() 
{
   CDictSel dictsel;

   if (m_lbDictionary.GetSelCount() == 0)
   {
      AfxMessageBox(BDString(IDS_NOSECTOR));
      return;
   }

   // Retrieve dictionaries selected

   int i = 0; for (i = 0; i < m_lbDictionary.GetCount(); i++)
   {
      int j = 0; for (j = 0; j < m_paDictSel->GetSize(); j++)
      {
         if (m_lbDictionary.GetItemData(i) == (DWORD)m_paDictSel->GetAt(j).m_lId)
         {
            dictsel = m_paDictSel->GetAt(j);
            dictsel.m_bSel = m_lbDictionary.GetSel(i);
            m_paDictSel->SetAt(j, dictsel);
            break;
         }
      }
   }

   // Determine if links are to be included

   *m_pbIncLinks = m_ckIncLinks.GetCheck();	   
	CDialog::OnOK();
  
}

/////////////////////////////////////////////////////////////////////////////

void CDlgSelDictionary::OnSelectall() 
{
   m_lbDictionary.SelectAll();	
   m_pbSelectAll.Update(m_lbDictionary);
}

/////////////////////////////////////////////////////////////////////////////

void CDlgSelDictionary::OnSelchangeDictionaries() 
{
   m_pbSelectAll.Update(m_lbDictionary);	
}

/////////////////////////////////////////////////////////////////////////////

void CDlgSelDictionary::OnAdd() 
{
   CDictionary dictionary;

   CDlgAddSector dlg;
   if (dlg.DoModal() == IDOK)
   {
      dictionary.m_sDesc = dlg.GetName();      
      dictionary.m_lId = m_lMaxId+1;
      BDDictionary(BDHandle(), &dictionary, BDADD);
      BDEnd(BDHandle());
      CComboBoxFType::InitDictionary(TRUE);
      InitList();
   }	
}

/////////////////////////////////////////////////////////////////////////////

void CDlgSelDictionary::OnDelete() 
{
   CDictionary dictionary;

   int index = m_lbDictionary.GetCurSel();
   if (index != LB_ERR)
   {
      dictionary.m_lId = m_lbDictionary.GetItemData(index);
      BDDictionary(BDHandle(), &dictionary, BDDELETE);
      BDEnd(BDHandle());
      CComboBoxFType::InitDictionary(TRUE);
      InitList();
   }
}
