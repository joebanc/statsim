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
#include "DlgLayout.h"
#include "docmaplayout.h"
#include "dlgrename.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgLayout dialog


CDlgLayout::CDlgLayout(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLayout::IDD, pParent)
{


	//{{AFX_DATA_INIT(CDlgLayout)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgLayout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgLayout)
	DDX_Control(pDX, IDC_LAYOUT, m_lbLayout);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgLayout, CDialog)
	//{{AFX_MSG_MAP(CDlgLayout)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_RENAME, OnRename)   
	ON_LBN_SELCHANGE(IDC_LAYOUT, OnSelchangeLayout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CDlgLayout::OnInitDialog() 
{
	CDialog::OnInitDialog();

   InitLayout();   
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

void CDlgLayout::InitLayout()
{
   // Add names of layouts to list

   m_lbLayout.ResetContent();

   int i = 0; for (i = 0; i < BDGetApp()->GetLayout().GetSize(); i++)
   {
      int index = m_lbLayout.AddString(BDGetApp()->GetLayout().GetAt(i).m_sName);

      if (BDGetApp()->GetLayout().GetAt(i).m_bDefault)
      {
         m_lbLayout.SetCurSel(index);
      }
   }
   if (m_lbLayout.GetCurSel() == LB_ERR) m_lbLayout.SetCurSel(0);

   OnSelchangeLayout();
	
}

/////////////////////////////////////////////////////////////////////////////
// CDlgLayout message handlers

void CDlgLayout::OnAdd() 
{
   CMapLayout layout;      

   layout.m_sName.Format("%s%d", (LPCSTR)BDString(IDS_LAYOUT), m_lbLayout.GetCount()+1); 

   // Add default position of map and legend to layout

   CMapLayoutObj *pMapLayout = layout.GetLayoutObjPtr(CMapLayout::map);
   if (pMapLayout != NULL) pMapLayout->m_rect = CRect(50,50,600,950);
   pMapLayout = layout.GetLayoutObjPtr(CMapLayout::legend);
   if (pMapLayout != NULL) pMapLayout->m_rect = CRect(650,50,950,950); 

   BDGetApp()->GetLayout().Add(layout);
  

   SetDefault(BDGetApp()->GetLayout().GetSize()-1);

   //OnRename();
}

///////////////////////////////////////////////////////////////////////////////

void CDlgLayout::OnEdit() 
{
   // Get selection

   int index = m_lbLayout.GetCurSel();
   if (index > 0) // Not default
   {
      CMultiDocTemplate* pTemplate = BDGetApp()->GetDocTemplate("MapLayout");      
   
      CDocMapLayout* pDoc = (CDocMapLayout*)pTemplate->OpenDocumentFile(NULL);   			   

      ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CDocMapLayout)));

   
      if (pDoc != NULL)
      {              
         pDoc->SetMapLayout(BDGetApp()->GetLayout().GetData() + index);         

         SetDefault(index);
       
         OnOK();
      };		
   };
	
}

///////////////////////////////////////////////////////////////////////////////

void CDlgLayout::OnDelete() 
{
   int index = m_lbLayout.GetCurSel();
   if (index > 0)  // can't delete default
   {
       BDGetApp()->GetLayout().RemoveAt(index);
       InitLayout();
   };  	
}

///////////////////////////////////////////////////////////////////////////////

void CDlgLayout::OnOK() 
{
	int index = m_lbLayout.GetCurSel();
   if (index != LB_ERR) 
   {
      SetDefault(index);
   } 

   CDialog::OnOK();
}

///////////////////////////////////////////////////////////////////////////////

void CDlgLayout::SetDefault(int index)
{
   // First reset all default flags

   int i = 0; for (i = 0; i < BDGetApp()->GetLayout().GetSize(); i++)
   {
      CMapLayout layout = BDGetApp()->GetLayout().GetAt(i);
      layout.m_bDefault = i == index;
      BDGetApp()->GetLayout().SetAt(i, layout);
   }
   InitLayout();
}

///////////////////////////////////////////////////////////////////////////////
//
// Rename the current layer
//

void CDlgLayout::OnRename() 
{
   CRect rect;

   int index = m_lbLayout.GetCurSel();
   if (index > 0)  //Not default
   {
       CMapLayout* pMapLayout = BDGetApp()->GetLayout().GetData() + index;

       m_lbLayout.SetTopIndex(index);
       m_lbLayout.GetItemRect(index, &rect);

       m_lbLayout.ClientToScreen(&rect);

       CDlgRename dlg(pMapLayout->m_sName, &rect);
       if (dlg.DoModal() == IDOK)
       {
          pMapLayout->m_sName = dlg.GetText();          
       }
       InitLayout();
   }	
}

///////////////////////////////////////////////////////////////////////////////
//
// Cannot edit or delete default
//

void CDlgLayout::OnSelchangeLayout() 
{
   int index = m_lbLayout.GetCurSel();

   GetDlgItem(IDC_EDIT)->EnableWindow(index > 0);
   GetDlgItem(IDC_DELETE)->EnableWindow(index > 0);
   GetDlgItem(IDC_RENAME)->EnableWindow(index > 0);
	
}

