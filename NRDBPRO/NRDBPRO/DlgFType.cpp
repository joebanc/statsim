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
#include "DlgFType.h"
#include "dlgftypeattr.h"
#include "dlgdeleteftype.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgFType dialog


CDlgFType::CDlgFType(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFType::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFType)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgFType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFType)
	DDX_Control(pDX, IDC_FEATURETYPES, m_lbFTypes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFType, CDialog)
	//{{AFX_MSG_MAP(CDlgFType)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFType message handlers

/////////////////////////////////////////////////////////////////////////////

BOOL CDlgFType::OnInitDialog() 
{
	CDialog::OnInitDialog();

   if (BDGetTransLevel(BDHandle()) > 0) BDCommit(BDHandle());
   ASSERT(BDGetTransLevel(BDHandle()) == 0);
	//BDBeginTrans(BDHandle());   
   //GetDlgItem(IDCANCEL)->EnableWindow(FALSE);

	InitFType(BDFTypeSel());

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

void CDlgFType::InitFType(long lIdSel)
{
   CFeatureType ftype;

   m_lbFTypes.ResetContent();

   BOOL bFound = BDFeatureType(BDHandle(), &ftype, BDGETINIT);
   while (bFound)
   {
      int index = m_lbFTypes.AddString(ftype.m_sDesc);
	  m_lbFTypes.SetItemData(index, ftype.m_lId);

	  if (lIdSel <= ftype.m_lId && m_lbFTypes.GetCurSel() == LB_ERR) 
     {
        BDFTypeSel(lIdSel);
        m_lbFTypes.SetCurSel(index);
     };

	  bFound = BDGetNext(BDHandle());
   }
   BDEnd(BDHandle());
   
   if (m_lbFTypes.GetCurSel() == LB_ERR) m_lbFTypes.SetCurSel(0);
}

/////////////////////////////////////////////////////////////////////////////

void CDlgFType::OnAdd() 
{
   CDlgFTypeAttr dlg;
   if (dlg.DoModal() == IDOK)
   {
      InitFType(dlg.GetId());
   }  
}

/////////////////////////////////////////////////////////////////////////////

void CDlgFType::OnEdit() 
{
   int index = m_lbFTypes.GetCurSel();
   if (index != LB_ERR)
   {
      int lId = m_lbFTypes.GetItemData(index);
      CDlgFTypeAttr dlg(lId);
      if (dlg.DoModal() == IDOK)
      {
         InitFType(dlg.GetId());
      }	
   };

}

///////////////////////////////////////////////////////////////////////////////

void CDlgFType::OnDelete() 
{
   CFeatureType ftype;
   long lFType;
   CString s;
      
   int index = m_lbFTypes.GetCurSel();
   if (index != LB_ERR)
   {
      lFType = m_lbFTypes.GetItemData(index);

      CDlgDeleteFType dlg;
      if (dlg.DoModal() == IDOK && 
         AfxMessageBox(BDString(IDS_CONFIRMDELETE), MB_YESNO|MB_DEFBUTTON2) == IDYES)
      {
         // If deleting definition then check that the a child does not exist

         if (dlg.IsDeleteFType() || dlg.IsDeleteFeatures())
         {
            BOOL bFound = BDFeatureType(BDHandle(), &ftype, BDGETINIT);
            while (bFound)
            {
               if (ftype.m_lParentFType == lFType)
               {
                  s.Format("%s: %s", BDString(IDS_CANNOTDEL), ftype.m_sDesc);     
                  AfxMessageBox(s);
                  return;
               }
               bFound = BDGetNext(BDHandle());
            }
            BDEnd(BDHandle());
         }

         // Delete data

         int iFlag = 0;
         if (dlg.IsDeleteData()) iFlag |= BDDELDATA;         
         if (dlg.IsDeleteFeatures()) iFlag |= BDDELFEATURE;         
         if (dlg.IsDeleteFType()) iFlag |= BDDELFTYPE;
         
         if (BDFTypeDelete(BDHandle(), lFType, iFlag))
         {
            AfxMessageBox(BDString(IDS_DELETED));
            InitFType(lFType);
         };         
      };
   }	
}

///////////////////////////////////////////////////////////////////////////////

void CDlgFType::OnOK() 
{          
	CDialog::OnOK();
}

///////////////////////////////////////////////////////////////////////////////

void CDlgFType::OnCancel() 
{
}

///////////////////////////////////////////////////////////////////////////////

void CDlgFType::OnClose() 
{
   OnOK();
}
