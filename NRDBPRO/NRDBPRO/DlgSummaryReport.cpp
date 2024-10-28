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
#include "DlgSummaryReport.h"
#include "dlgselectfeatures.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSummaryReport dialog


CDlgSummaryReport::CDlgSummaryReport(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSummaryReport::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSummaryReport)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgSummaryReport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSummaryReport)
	DDX_Control(pDX, IDC_SELECTALL, m_pbSelectAll);
   DDX_Control(pDX, IDC_FTYPE, m_cbFTypes);
	DDX_Control(pDX, IDC_FEATURES, m_lbFeatures);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSummaryReport, CDialog)
	//{{AFX_MSG_MAP(CDlgSummaryReport)
	ON_CBN_SELCHANGE(IDC_FTYPE, OnSelchangeFtype)
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	ON_BN_CLICKED(IDC_DICTIONARY, OnDictionary)
	ON_BN_CLICKED(IDC_SELECT, OnSelect)
	ON_LBN_SELCHANGE(IDC_FEATURES, OnSelchangeFeatures)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSummaryReport message handlers

BOOL CDlgSummaryReport::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   m_cbFTypes.Init(BDFTypeSel(), TRUE);   

   m_bInit = TRUE;
   OnSelchangeFtype();   
   m_bInit = FALSE;
   
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

void CDlgSummaryReport::OnSelchangeFtype() 
{
   CAttrArray aAttr;
   CFeature feature;
   CFeatureType ftype;

   m_lbFeatures.ResetContent();   
   
   int index =m_cbFTypes.GetCurSel();
   if (index != CB_ERR)
   {
      long lFType = m_cbFTypes.GetItemData(index);      

	   // Initialise list of features corresponding to selected ftype

	   feature.m_lFeatureTypeId = lFType;
	   aAttr.m_lFType = feature.m_lFeatureTypeId;	         

      BOOL bFound = BDFeature(BDHandle(), &feature, BDSELECT2);
      while (bFound)
	   {
		   index = m_lbFeatures.AddString(feature.m_sName);
		   m_lbFeatures.SetItemData(index, feature.m_lId);

         if (m_bInit && BDIsFeatureSel(lFType, feature.m_lId))
         {
            m_lbFeatures.SetSel(index);
         }

		   bFound = BDGetNext(BDHandle());
	   }
	   BDEnd(BDHandle());      
   }
   OnSelchangeFeatures();
}

/////////////////////////////////////////////////////////////////////////////

void CDlgSummaryReport::OnOK() 
{
   CString s;
   CFeature feature;

	if (m_lbFeatures.GetSelCount() > 0)
	{
		// Retrieve feature type

		m_lFType = m_cbFTypes.GetItemData(m_cbFTypes.GetCurSel());

	  // Retrieve selected features
		
       int i = 0; for (i = 0; i < m_lbFeatures.GetCount(); i++)
	   {
          if (m_lbFeatures.GetSel(i))
		  {
			  m_alFeatures.Add(m_lbFeatures.GetItemData(i));
			  m_lbFeatures.GetText(i, s);			  
		  }
	   }

      m_cbFTypes.SaveFTypes();

      // Store default features

      BDFeatureSel().RemoveAll();
      for (i = 0; i < m_alFeatures.GetSize(); i++)
      {
         feature.m_lFeatureTypeId = m_lFType;
         feature.m_lId = m_alFeatures[i];
         BDFeatureSel().Add(feature);
      }         

	   CDialog::OnOK();
	};
}

/////////////////////////////////////////////////////////////////////////////

void CDlgSummaryReport::OnSelectall() 
{
	m_lbFeatures.SelectAll();		
	OnSelchangeFeatures();	
}

/////////////////////////////////////////////////////////////////////////////

void CDlgSummaryReport::OnSelchangeFeatures() 
{
	m_pbSelectAll.Update(m_lbFeatures);	
}

/////////////////////////////////////////////////////////////////////////////

void CDlgSummaryReport::OnDictionary() 
{
   m_cbFTypes.OnClickDictionary(TRUE);	
}

/////////////////////////////////////////////////////////////////////////////

void CDlgSummaryReport::OnSelect() 
{
   long lFType = 0; 

   // Determine the parent feature type of that selected

   int index = m_cbFTypes.GetCurSel();
   if (index != LB_ERR)
   {      
      lFType = m_cbFTypes.GetItemData(index);
     
   // Create a dialog allowing a sub-set of the features to be selected
   
      CDlgSelectFeatures dlg(lFType);      
      if (dlg.DoModal() == IDOK)
      {
         dlg.Initialise(m_lbFeatures);         
      }
   };		
}


