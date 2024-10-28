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
#include "DlgSelectFeatures.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CDWordArray CDlgSelectFeatures::m_alParentFeatures;

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectFeatures dialog


CDlgSelectFeatures::CDlgSelectFeatures(long lFType, BOOL bSglSel, CWnd* pParent /*=NULL*/)
	: CDialog(IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSelectFeatures)
	//}}AFX_DATA_INIT

   CFeatureType ftype;

   m_lFType = lFType;
   if (BDFTypeI(BDHandle(), lFType, &ftype))
   {
	   m_lFType = ftype.m_lId;
   }
   
   m_lParentFType = 0;
   m_lGrandParentFType = 0;

   // Determine parent and grandparent feature type
   
   if (BDFTypeParentI(BDHandle(), m_lFType, &ftype))
   {
	   m_lParentFType = ftype.m_lId;

      if (BDFTypeParentI(BDHandle(), m_lParentFType, &ftype))
      {
         m_lGrandParentFType = ftype.m_lId;
      }
   }
   
   // if no parent then replace dialog resource to a single feature selection

   else
   {
      m_lpszTemplateName = MAKEINTRESOURCE(IDD_SELECTFEATURES1);
   }
     
   // Handle single and multiple selections

   m_bSglSel = bSglSel;
   if (bSglSel) m_plbFeatures = &m_lbFeaturesSgl;         
   else m_plbFeatures = &m_lbFeaturesMult;               

   
}

/////////////////////////////////////////////////////////////////////////////

void CDlgSelectFeatures::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSelectFeatures)
	DDX_Control(pDX, IDC_SELECTALL2, m_pbSelectAll2);
	DDX_Control(pDX, IDC_SELECTALL1, m_pbSelectAll1);
	DDX_Control(pDX, IDC_FEATURESMULT, m_lbFeaturesMult);
	DDX_Control(pDX, IDC_FEATURESSGL, m_lbFeaturesSgl);
	DDX_Control(pDX, IDC_PARENTFEATURES, m_lbParentFeatures);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSelectFeatures, CDialog)
	//{{AFX_MSG_MAP(CDlgSelectFeatures)
	ON_LBN_SELCHANGE(IDC_PARENTFEATURES, OnSelchangeParentfeatures)
	ON_BN_CLICKED(IDC_SELECTALL1, OnSelectall1)
	ON_BN_CLICKED(IDC_SELECTALL2, OnSelectall2)
	ON_LBN_SELCHANGE(IDC_FEATURESMULT, OnSelchangeFeaturesmult)
	ON_LBN_SELCHANGE(IDC_FEATURESSGL, OnSelchangeFeaturessgl)
	ON_BN_CLICKED(IDC_SELECT, OnSelect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectFeatures message handlers

BOOL CDlgSelectFeatures::OnInitDialog() 
{
   CFeature feature;

	CDialog::OnInitDialog();

	/*if (m_lParentFType == 0) 
	{
		OnCancel();
		return FALSE;
	}*/

   // Supports single or multiple selection

   if (m_bSglSel)
   {
      m_lbFeaturesMult.ShowWindow(FALSE);   
      GetDlgItem(IDC_SELECTALL2)->ShowWindow(SW_HIDE);
   }
   else 
   {
      m_lbFeaturesSgl.ShowWindow(FALSE);
   };

   // If no grandparent then disable the select button

   if (m_lGrandParentFType == 0)
   {
      GetDlgItem(IDC_SELECT)->EnableWindow(FALSE);
   }
	
	// Fill list box with features corresponding to the type selected

   if (m_lParentFType != 0)
   {
      feature.m_lFeatureTypeId = m_lParentFType;
      BOOL bFound = BDFeature(BDHandle(), &feature, BDSELECT2);
      while (bFound)
      {
         int index = m_lbParentFeatures.AddString(feature.m_sName);
         m_lbParentFeatures.SetItemData(index, feature.m_lId);

	     int i = 0; for (i = 0; i < m_alParentFeatures.GetSize(); i++)
	     {
		     if ((long)m_alParentFeatures.GetAt(i) == feature.m_lId)
		     {
			     m_lbParentFeatures.SetSel(index);
		     }
	     }
         bFound = BDGetNext(BDHandle());
      };
      BDEnd(BDHandle());
   }

   OnSelchangeParentfeatures();   

// Set the names of the controls

   CFeatureType ftype;
   if (m_lParentFType != 0)
   {
      ftype.m_lId = m_lParentFType;
      BDFeatureType(BDHandle(), &ftype, BDSELECT);
      GetDlgItem(IDS_PARENT)->SetWindowText(ftype.m_sDesc);
   }
   
   ftype.m_lId = m_lFType;
   BDFeatureType(BDHandle(), &ftype, BDSELECT);
   GetDlgItem(IDS_FEATURE)->SetWindowText(ftype.m_sDesc);
   BDEnd(BDHandle());
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
//
// Add features to the list box corresponding to the selected feature type
//

void CDlgSelectFeatures::OnSelchangeParentfeatures() 
{
   
   CFeature feature;
   
   m_plbFeatures->ResetContent(); 

   BeginWaitCursor();
   m_plbFeatures->SetRedraw(FALSE);

   // Optimization, only retrieve features needed from the database

   if (m_lParentFType != 0)
   {
      int i = 0; for (i = 0; i < m_lbParentFeatures.GetCount(); i++)
      {
          if (m_lbParentFeatures.GetSel(i) )
	      {          
		     feature.m_lParentFeature = m_lbParentFeatures.GetItemData(i);
		     feature.m_lFeatureTypeId = m_lFType;
		     BOOL bFound = BDFeature(BDHandle(), &feature, BDSELECT4);
		     while (bFound)
		     {
                 int index = m_plbFeatures->AddString(feature.m_sName);
                 m_plbFeatures->SetItemData(index, feature.m_lId);
              
                bFound = BDGetNext(BDHandle());
		     }
		     BDEnd(BDHandle());
	      }
      }
   } 

   // No parent so retrieve all features

   else
   {
      feature.m_lFeatureTypeId = m_lFType;
      BOOL bFound = BDFeature(BDHandle(), &feature, BDSELECT2);
      while (bFound)
		{
           int index = m_plbFeatures->AddString(feature.m_sName);
           m_plbFeatures->SetItemData(index, feature.m_lId);
        
          bFound = BDGetNext(BDHandle());
      }
	   BDEnd(BDHandle());
   }
       
   m_plbFeatures->SetRedraw(TRUE);
   EndWaitCursor();

   m_pbSelectAll1.Update(m_lbParentFeatures);

   if (m_bSglSel) OnSelchangeFeaturessgl();
   else OnSelchangeFeaturesmult();   
}

/////////////////////////////////////////////////////////////////////////////

void CDlgSelectFeatures::OnSelectall1() 
{	
	m_lbParentFeatures.SelectAll();
    OnSelchangeParentfeatures();
}

/////////////////////////////////////////////////////////////////////////////

void CDlgSelectFeatures::OnSelectall2() 
{
   m_plbFeatures->SelectAll();
   m_pbSelectAll2.Update(*m_plbFeatures);
   OnSelchangeFeaturesmult();
}

/////////////////////////////////////////////////////////////////////////////

void CDlgSelectFeatures::OnSelchangeFeaturesmult() 
{
   m_pbSelectAll2.Update(*m_plbFeatures);

   GetDlgItem(IDOK)->EnableWindow(m_plbFeatures->GetSelCount() != 0);
}

/////////////////////////////////////////////////////////////////////////////

void CDlgSelectFeatures::OnSelchangeFeaturessgl() 
{
	GetDlgItem(IDOK)->EnableWindow(m_plbFeatures->GetCurSel() != LB_ERR);	
}

/////////////////////////////////////////////////////////////////////////////

void CDlgSelectFeatures::OnOK() 
{
	// Store the parent features

	m_alParentFeatures.RemoveAll();
    int i = 0; for (i = 0; i < m_lbParentFeatures.GetCount(); i++)
	{
       if (m_lbParentFeatures.GetSel(i))
	   {
          m_alParentFeatures.Add(m_lbParentFeatures.GetItemData(i));
	   }
	}
	
	// Store the features

   for (i = 0; i < m_plbFeatures->GetCount(); i++)
   {
      if (m_plbFeatures->GetSel(i))
      {
         m_alFeatures.Add(m_plbFeatures->GetItemData(i));
      }       
   }		
	
	CDialog::OnOK();
}



/////////////////////////////////////////////////////////////////////////////

void CDlgSelectFeatures::Initialise(CListBox& rlb)
{
   if (m_bSglSel) rlb.SetCurSel(-1);

   BeginWaitCursor();

   // First sort the selected features to optimise searching

   m_alFeatures.Sort();

   rlb.ShowWindow(SW_HIDE);

   int i = 0; for (i = 0; i < rlb.GetCount(); i++)
   {
      if (!m_bSglSel) rlb.SetSel(i, FALSE);      

      /*int j = 0; for (j = 0; j < m_alFeatures.GetSize(); j++)*/
      {
         /*if (rlb.GetItemData(i) == m_alFeatures.GetAt(j))*/

		 if (m_alFeatures.FindSorted(rlb.GetItemData(i)) != -1)
         {
            if (m_bSglSel)
            {
               rlb.SetCurSel(i);
               rlb.SetTopIndex(i);
            }
            else 
            {
               rlb.SetSel(i, TRUE);
            }
            //break;
         }
		 
      }
   };

   rlb.ShowWindow(SW_SHOW);

   EndWaitCursor();
}

///////////////////////////////////////////////////////////////////////////////

void CDlgSelectFeatures::Initialise(CComboBox& rcb)
{
   rcb.SetCurSel(-1);

   BeginWaitCursor();   
   int i = 0; for (i = 0; i < rcb.GetCount(); i++)
   {   
      int j = 0; for (j = 0; j < m_alFeatures.GetSize(); j++)
      {
         if ((long)rcb.GetItemData(i) == m_alFeatures.GetAt(j))
         {
            rcb.SetCurSel(i);
            rcb.SetTopIndex(i);            
            break;
         }
      }
   };   
   EndWaitCursor();
}

///////////////////////////////////////////////////////////////////////////////
//
// Recursive selection
//

void CDlgSelectFeatures::OnSelect() 
{
  CDlgSelectFeatures dlg(m_lParentFType, FALSE);      

  if (dlg.DoModal() == IDOK)
  {
     dlg.Initialise(m_lbParentFeatures);         
	 OnSelchangeParentfeatures();
  }	
}
