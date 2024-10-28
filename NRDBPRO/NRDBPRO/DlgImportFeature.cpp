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
#include "DlgImportFeature.h"
#include "dlgeditfeature.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

long CDlgImportFeature::m_lFTypePrev;
long CDlgImportFeature::m_lFeaturePrev;
CString CDlgImportFeature::m_sDescPrev;
CString CDlgImportFeature::m_sFeaturePrev;

/////////////////////////////////////////////////////////////////////////////
//
// Intelligent version of compare, matches on individual words
//

inline int Compare(LPCSTR s1, LPCSTR s2)
{   
   int j = 0;
   BOOL bMatch = TRUE;
   while (*s1 != '\0' && *s2 != '\0')
   {      
      // Skip hypens and white space

     
      if (*s1 == *s2) 
      {
         j++;
      } 

      // If character does not match then continue through word - 
      // maybe just one character different

      else if (*s1 != *s2 && *s1 != ' ' && *s2 != ' ' && 
          *s1 != '-' && *s2 != '-')
      {
         // Do not increase match count
      }          
      
      // If match has failed then search to end of white space

      else
      {
         while (*s1 != '\0' && *s1 != ' ' && *s1 != '-') s1++;
         while (*s2 != '\0' && *s2 != ' ' && *s2 != '-') s2++;         

         while (*s1 == ' ' || *s1 == '-') s1++;      
         while (*s2 == ' ' || *s2 == '-') s2++;      
      }
      
      if (*s1 != '\0') s1++;
      if (*s2 != '\0') s2++;
   }
   return j;   
}

/////////////////////////////////////////////////////////////////////////////
// CDlgImportFeature dialog


CDlgImportFeature::CDlgImportFeature(long lFType, LPCSTR sDesc, LPCSTR sFeature, LPCSTR sFeatureParent, 
                                     long lParentFeature, BOOL bSelectAll, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImportFeature::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgImportFeature)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

   m_lFType = lFType;
   m_lParentFeature = lParentFeature;
   m_sDesc = sDesc;
   m_sFeature = sFeature;
   m_sFeatureParent = sFeatureParent;
   m_bSelectAll = bSelectAll;
}


void CDlgImportFeature::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImportFeature)
	DDX_Control(pDX, IDC_FEATURE, m_cbFeature);
	DDX_Control(pDX, IDC_DESCRIPTION, m_sDescription);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgImportFeature, CDialog)
	//{{AFX_MSG_MAP(CDlgImportFeature)
	ON_BN_CLICKED(IDC_CREATE, OnCreate)
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	ON_CBN_SELCHANGE(IDC_FEATURE, OnSelchangeFeature)
	ON_BN_CLICKED(IDC_CREATEALL, OnCreateAll)
	ON_BN_CLICKED(IDC_SKIP, OnSkip)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImportFeature message handlers

BOOL CDlgImportFeature::OnInitDialog() 
{
	CDialog::OnInitDialog();
   int nFit = 0;
   BOOL bFound = FALSE;
   CString s,s1;
   CFeature featureP;
   CFeatureType ftypeP, ftype;

   BDFTypeI(BDHandle(), m_lFType, &ftype);

   // Set the text

   m_sDescription.SetWindowText(ftype.m_sDesc + ": " + m_sDesc + "\r\n" + m_sFeature + "\r\n" + m_sFeatureParent);       
	
   // Load list of feature types

   CFeature feature;
   feature.m_lFeatureTypeId = m_lFType;

   BDFTypeParentI(BDHandle(), m_lFType, &ftypeP);

   // Retrieve all available features, if parent is specified then retrieve
   // only for the parent

   if (m_lParentFeature != 0)
   {
      feature.m_lParentFeature = m_lParentFeature;
      bFound = BDFeature(BDHandle(), &feature, BDSELECT4);
   } else
   {
      bFound = BDFeature(BDHandle(), &feature, BDSELECT2);
   };

   while (bFound)
   {
      // Recall previous selection if matches

      int index = m_cbFeature.AddString(feature.m_sName);
      m_cbFeature.SetItemData(index, feature.m_lId);
      
      if (m_lFTypePrev == feature.m_lFeatureTypeId && m_lFeaturePrev == feature.m_lId &&
          m_sDescPrev == feature.m_sName && 
          m_sFeaturePrev == m_sFeature + m_sFeatureParent)
      {
         m_cbFeature.SetCurSel(index);
      }      

      bFound = BDGetNext(BDHandle());
   }
   BDEnd(BDHandle());

   // If not selection then search for the closest

   if (m_cbFeature.GetCurSel() == CB_ERR)
   {
      int iBest = 0;

      int i = 0; for (i = 0; i < m_cbFeature.GetCount(); i++)
      {
         m_cbFeature.GetLBText(i, s);

         int j = Compare(m_sFeature, s);
         if (j > iBest)
         {
            m_cbFeature.SetCurSel(i);
            iBest = j;
         }

      }
      
   }   
   if (m_cbFeature.GetCurSel() == CB_ERR) m_cbFeature.SetCurSel(0);

   OnSelchangeFeature();

   CenterWindow();

   // Disable 'create all' if no feature name

   if (!m_bSelectAll) GetDlgItem(IDC_CREATEALL)->EnableWindow(FALSE);
   if (!m_bSelectAll) GetDlgItem(IDC_CREATE)->EnableWindow(FALSE);

   // Set focus on control

   m_cbFeature.SetFocus();
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////

void CDlgImportFeature::OnCreate() 
{	
   CDlgEditFeature dlg(m_lFType, m_sFeature, m_sFeatureParent);
   if (dlg.DoModal() == IDOK)
   {      
      m_lFeature = dlg.GetId();
                      
      EndDialog(IDC_SELECTALL);
   }
}

/////////////////////////////////////////////////////////////////////////////

void CDlgImportFeature::OnCancel() 
{
	m_lFeature = -1;
	
	CDialog::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////

void CDlgImportFeature::OnSelectall() 
{
   int index = m_cbFeature.GetCurSel();   
   if (index != CB_ERR)
   {
      m_lFeature = m_cbFeature.GetItemData(index);

      m_lFTypePrev = 0;
      m_lFeaturePrev = 0;

      EndDialog(IDC_SELECTALL);
   }	
}

/////////////////////////////////////////////////////////////////////////////

void CDlgImportFeature::OnSelchangeFeature() 
{
   CFeature feature;
   CFeatureType ftype;

   GetDlgItem(IDC_PARENT)->SetWindowText("");

   int index = m_cbFeature.GetCurSel();
   if (index != CB_ERR)
   {
      feature.m_lId = m_cbFeature.GetItemData(index);
      feature.m_lFeatureTypeId = m_lFType;
      
      if (BDFeature(BDHandle(), &feature, BDSELECT) && 
          feature.m_lParentFeature != 0)
      {
         if (BDFTypeParentI(BDHandle(), feature.m_lFeatureTypeId, &ftype))
         {
            feature.m_lId = feature.m_lParentFeature;
            feature.m_lFeatureTypeId = ftype.m_lId;
            BDFeature(BDHandle(), &feature, BDSELECT);
            GetDlgItem(IDC_PARENT)->SetWindowText(feature.m_sName);
         };
      }      
      BDEnd(BDHandle());
   }	

   // Disable select buttons if no features to select

   BOOL bEmpty = m_cbFeature.GetCount() == 0;   
   GetDlgItem(IDC_SELECTALL)->EnableWindow(!bEmpty);
}


/////////////////////////////////////////////////////////////////////////////

void CDlgImportFeature::OnCreateAll() 
{
   EndDialog(IDC_CREATEALL);	
}

/////////////////////////////////////////////////////////////////////////////

void CDlgImportFeature::OnSkip() 
{
   EndDialog(IDC_SKIP);
	
}
