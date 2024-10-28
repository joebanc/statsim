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
#include "DlgEditFeature.h"
#include "dlgselectfeatures.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgEditFeature dialog


CDlgEditFeature::CDlgEditFeature(long lFType, long lId, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditFeature::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgEditFeature)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

   m_lId = lId;
   m_lFType = lFType;
   m_lFTypeParent = 0;
   m_lFTypeGrandParent = 0;

}

/////////////////////////////////////////////////////////////////////////////

CDlgEditFeature::CDlgEditFeature(long lFType, LPCSTR sName, LPCSTR sParentName) : 
   CDialog(IDD, NULL)
{
   m_lId = 0;
   m_lFType = lFType;
   m_sName = sName;
   m_sParentName = sParentName;

   m_lFTypeParent = 0;
   m_lFTypeGrandParent = 0;
}

/////////////////////////////////////////////////////////////////////////////

void CDlgEditFeature::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgEditFeature)
	DDX_Control(pDX, IDC_PARENTFEATURE, m_cbParentFeature);
	DDX_Control(pDX, IDC_NAME, m_eDesc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgEditFeature, CDialog)
	//{{AFX_MSG_MAP(CDlgEditFeature)
	ON_CBN_SELCHANGE(IDC_PARENTFEATURE, OnSelchangeParentfeature)
	ON_BN_CLICKED(IDC_SELECT, OnSelect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgEditFeature message handlers

BOOL CDlgEditFeature::OnInitDialog() 
{
   long lParentFeatureSel = 0; 
   CFeatureType ftype, ftypeP, ftypeG;   
   CFeature feature;   
   BOOL bOK = TRUE;
   CString s;
  
	CDialog::OnInitDialog();   

  // Determine the parent feature types
   
   bOK = BDFTypeI(BDHandle(), m_lFType, &ftype);
   m_lFType = ftype.m_lId;

   if (BDFTypeParentI(BDHandle(), m_lFType, &ftypeP))
   {
      m_lFTypeParent = ftypeP.m_lId;

      BDFTypeParentI(BDHandle(), ftypeP.m_lId, &ftypeG);
      m_lFTypeGrandParent = ftypeG.m_lId;
   };

      
   // Add
   if (m_lId == 0)
   {
      m_iFlag = BDADD;
   
      BDNextId(BDHandle(), BDFEATURE, ftype.m_lId, &m_lId);

      m_eDesc.SetWindowText(m_sName);

      SetWindowText(BDString(IDS_ADDNAME));

      // Determine parent (if any)
      
      if (m_sParentName != "")
      {
         CFeature feature;
         feature.m_sName = m_sParentName;
         feature.m_lFeatureTypeId = ftype.m_lParentFType;
         if (BDFeature(BDHandle(), &feature, BDSELECT3))
         {
            lParentFeatureSel = feature.m_lId;
         }
      };      
   } else
   {
     m_iFlag = BDUPDATE;
	  feature.m_lId = m_lId;
     feature.m_lFeatureTypeId = m_lFType;
     BDFeature(BDHandle(), &feature, BDSELECT);
	  BDEnd(BDHandle());
     feature.m_sName.TrimRight();
     m_eDesc.SetWindowText(feature.m_sName);
	  lParentFeatureSel = feature.m_lParentFeature;

     if (m_lFType == 0) m_lFType = feature.m_lFeatureTypeId;

     ASSERT(m_lFType == feature.m_lFeatureTypeId);
   }

   GetDlgItem(IDS_NAME)->SetWindowText(ftype.m_sDesc);
   if (m_lFTypeParent != 0)
   {
      GetDlgItem(IDS_PARENTFTYPE)->SetWindowText(ftypeP.m_sDesc);
   } else
   {
      GetDlgItem(IDS_PARENTFTYPE)->SetWindowText("");
   }

   // Fill a list of available parent features
   
   feature.m_lFeatureTypeId = ftype.m_lParentFType;
   if (bOK && feature.m_lFeatureTypeId != 0)
   {    
	   BOOL bFound = BDFeature(BDHandle(), &feature, BDSELECT2);
	   while (bFound)
	   {
		   int index = m_cbParentFeature.AddString(feature.m_sName);
		   m_cbParentFeature.SetItemData(index, feature.m_lId);

		   if (lParentFeatureSel == feature.m_lId)
		   {
			   m_cbParentFeature.SetCurSel(index);
		   }
         
		   bFound = BDGetNext(BDHandle());
	   }
      BDEnd(BDHandle());
   } else
   {
	   m_cbParentFeature.EnableWindow(FALSE);
   }   

   // If not selection then search for the closest

   if (m_cbParentFeature.GetCurSel() == CB_ERR && m_sParentName != "")
   {
      int i = 0; for (i = 0; i < m_cbParentFeature.GetCount(); i++)
      {
         m_cbParentFeature.GetLBText(i, s);
         if (m_sParentName < s && i > 0) 
         {
            m_cbParentFeature.SetCurSel(i-1);
            break;
         }
         if (m_sParentName == s)
         {
            m_cbParentFeature.SetCurSel(i);
            break;
         }
      }
   }   

   if (m_cbParentFeature.GetCurSel() == CB_ERR) 
      m_cbParentFeature.SetCurSel(0);

   OnSelchangeParentfeature();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////
//
// Display the parent of the parent
//

void CDlgEditFeature::OnSelchangeParentfeature() 
{
   CFeature feature;
   GetDlgItem(IDS_PARENT)->SetWindowText("");

   int index = m_cbParentFeature.GetCurSel();
   if (index != CB_ERR)
   {
      feature.m_lId = m_cbParentFeature.GetItemData(index);
      feature.m_lFeatureTypeId = m_lFTypeParent;
      if (BDFeature(BDHandle(), &feature, BDSELECT) && feature.m_lParentFeature != 0)
      {
         feature.m_lId = feature.m_lParentFeature;
		 feature.m_lFeatureTypeId = m_lFTypeGrandParent;
         if (BDFeature(BDHandle(), &feature, BDSELECT))
         {
            GetDlgItem(IDS_PARENT)->SetWindowText(feature.m_sName);
         }
      }
      BDEnd(BDHandle());
   }	
}

///////////////////////////////////////////////////////////////////////////////

void CDlgEditFeature::OnOK() 
{
   CString sDesc;   
   BOOL bOK = TRUE;
   CFeature feature;

   // Retrieve the parent feature if exists

   if (m_cbParentFeature.IsWindowEnabled())
   {      
      int index = m_cbParentFeature.GetCurSel();
	  if (index != CB_ERR)
	  {
		  feature.m_lParentFeature = m_cbParentFeature.GetItemData(index);
	  } else
	  {
        AfxMessageBox(BDString(IDS_NOPARENT));
		  bOK = FALSE;
	  }
   };

   // Save values
  
   m_eDesc.GetWindowText(feature.m_sName);   
   if (bOK && !feature.m_sName.IsEmpty())
   {	  
	  feature.m_lId = m_lId;
	  feature.m_lFeatureTypeId = m_lFType;
      if (BDFeature(BDHandle(), &feature, m_iFlag))
      {
         CDialog::OnOK();
      };
   }	
}

///////////////////////////////////////////////////////////////////////////////

void CDlgEditFeature::OnSelect() 
{
   CDlgSelectFeatures dlg(m_lFTypeParent, TRUE);      
   if (dlg.DoModal() == IDOK)
   {
      dlg.Initialise(m_cbParentFeature);         
   }   
	
}
