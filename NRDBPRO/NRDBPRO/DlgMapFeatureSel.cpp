// DlgMapFeatureSel.cpp : implementation file
//

#include "stdafx.h"
#include "nrdbpro.h"
#include "DlgMapFeatureSel.h"
#include "dlgselectfeatures.h"
#include "dlgeditfeature.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgMapFeatureSel dialog


CDlgMapFeatureSel::CDlgMapFeatureSel(long lFType, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMapFeatureSel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMapFeatureSel)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

   m_lFType = lFType;
   m_lFeature = 0;
}


void CDlgMapFeatureSel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMapFeatureSel)
	DDX_Control(pDX, IDC_FEATURE, m_lbFeatures);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMapFeatureSel, CDialog)
	//{{AFX_MSG_MAP(CDlgMapFeatureSel)
	ON_BN_CLICKED(IDC_SELECT, OnSelect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMapFeatureSel message handlers

BOOL CDlgMapFeatureSel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Initialise the feature names

   int index = m_lbFeatures.AddString(BDString(IDS_ADDNEW));
   m_lbFeatures.SetItemData(index, 0);

   CFeature feature;
   feature.m_lFeatureTypeId = m_lFType;
   BOOL bFound = BDFeature(BDHandle(), &feature, BDSELECT2);
   while (bFound)
   {
      index = m_lbFeatures.AddString(feature.m_sName);
      m_lbFeatures.SetItemData(index, feature.m_lId);
      bFound = BDGetNext(BDHandle());
   }
   BDEnd(BDHandle());
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////
//
// Select by parent feature type
//

void CDlgMapFeatureSel::OnSelect() 
{
	CDlgSelectFeatures dlg(m_lFType, TRUE);      
   if (dlg.DoModal() == IDOK)
   {
      dlg.Initialise(m_lbFeatures);         
   }
	
}

///////////////////////////////////////////////////////////////////////////////

void CDlgMapFeatureSel::OnOK() 
{
	int index = m_lbFeatures.GetCurSel();
   if (index != LB_ERR)
   {
      // Allow user to create a new feature

      if (m_lbFeatures.GetItemData(index) == 0)
      {
         CDlgEditFeature dlg(m_lFType);  
         if (dlg.DoModal() == IDOK)
         {
            m_lFeature = dlg.GetId();
         } else
         {
            return;
         }
      }
      // Otherwise return selected feature
      else
      {
         m_lFeature = m_lbFeatures.GetItemData(index);
      }

	   CDialog::OnOK();
   };
}

