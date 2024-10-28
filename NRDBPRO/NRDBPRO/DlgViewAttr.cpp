// DlgViewAttr.cpp : implementation file
//

#include "stdafx.h"
#include "nrdbpro.h"
#include "DlgViewAttr.h"
#include "bdattribute.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgViewAttr dialog


CDlgViewAttr::CDlgViewAttr(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgViewAttr::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgViewAttr)
	//}}AFX_DATA_INIT

}


void CDlgViewAttr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgViewAttr)
	DDX_Control(pDX, IDC_LIST1, m_lbAttr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgViewAttr, CDialog)
	//{{AFX_MSG_MAP(CDlgViewAttr)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgViewAttr message handlers

void CDlgViewAttr::Update(long lFeatureId, long lFType)
{
   m_lbAttr.ResetContent();
   m_lbAttr.SetTabStops(20);

   // Retrieve feature

   CFeature feature;
   feature.m_lId = lFeatureId;
   feature.m_lFeatureTypeId = lFType;
   BOOL bFound = BDFeature(BDHandle(), &feature, BDSELECT);
   BDEnd(BDHandle());

   // Retrieve a list of attributes for the feature

   CAttrArray aAttr;
   BDFTypeAttrInit(BDHandle(), lFType, &aAttr);

   CLongArray alAttr;
   CLongArray alFeature;

   int i = 0; for (i = 0; i < aAttr.GetSize(); i++)
   {
      alAttr.Add(aAttr.GetAt(i)->GetAttrId());
   }

   alFeature.Add(lFeatureId);

   // Retrieve the feature

   CQuery query(lFType, alAttr, alFeature);
   
   // Now requery the data

   CQueryResult result;
   if (result.Initialise(&query))
   {      
      int i = 0; for (i = 0; i < result.GetSize(); i++)
      {
         CQueryAttrArray *pAttrArray = result.GetAt(i);

            // Add feature name
         m_lbAttr.AddString(feature.m_sName);

         // Add date

         CString sDate;
         CDateTime date(pAttrArray->m_lDate, 0);
         date.DateAsString(sDate);
         m_lbAttr.AddString(sDate);
         

         int j = 0; for (j = 0; j < pAttrArray->GetSize(); j++)
         {
            CAttribute *pAttr = pAttrArray->GetAt(j);

            m_lbAttr.AddString(pAttr->GetDesc() + ":\t" + ((CBDAttribute*)pAttr)->AsString());
         }
      }
   }
}

void CDlgViewAttr::OnClose() 
{
	// TODO: Add your message handler code here and/or call default

   DestroyWindow();
   
   CDialog::OnClose();
}
