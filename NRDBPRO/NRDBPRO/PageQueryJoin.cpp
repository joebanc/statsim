// PageQueryJoin.cpp : implementation file
//

#include "stdafx.h"
#include "nrdbpro.h"
#include "PageQueryJoin.h"
#include "sheetquery.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageQueryJoin property page

IMPLEMENT_DYNCREATE(CPageQueryJoin, CPropertyPage)

CPageQueryJoin::CPageQueryJoin(CQuery* pQuery, CQuery* pQueryPrev, int nType) : CPropertyPage(CPageQueryJoin::IDD)
{
   m_pQuery = pQuery;
   m_pQueryPrev = pQueryPrev;
   m_nType = nType;

	//{{AFX_DATA_INIT(CPageQueryJoin)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPageQueryJoin::CPageQueryJoin()
{
}


CPageQueryJoin::~CPageQueryJoin()
{
}

void CPageQueryJoin::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageQueryJoin)
	DDX_Control(pDX, IDC_JOINS, m_lbJoins);
	DDX_Control(pDX, IDC_FEATURETO, m_cbFTypeTo);
	DDX_Control(pDX, IDC_DISTANCE, m_eDistance);
	DDX_Control(pDX, IDC_CONDITION, m_cbCondition);
	DDX_Control(pDX, IDC_ATTRTO, m_cbAttrTo);
	DDX_Control(pDX, IDC_ATTRFROM, m_cbAttrFrom);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageQueryJoin, CPropertyPage)
	//{{AFX_MSG_MAP(CPageQueryJoin)
	ON_BN_CLICKED(IDC_DICTIONARY, OnSector)
	ON_CBN_SELCHANGE(IDC_FEATURETO, OnSelchangeFeatureto)
	ON_CBN_SELCHANGE(IDC_ATTRTO, OnSelchangeAttrto)
	ON_CBN_SELCHANGE(IDC_CONDITION, OnSelchangeCondition)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageQueryJoin message handlers

BOOL CPageQueryJoin::OnSetActive() 
{
	// Fill list of existing attributes

   CString sFType;
   m_cbAttrFrom.ResetContent();
   CQueryElement* pElement = m_pQuery;
   while (pElement != NULL)
   {
      if (pElement->GetDataType() == BDFTYPE)
      {
         sFType = CQuery::StripBrackets(pElement->GetDesc());
      }
      if (pElement->GetDataType() == BDCOORD || pElement->GetDataType() == BDMAPLINES)
      {
         m_cbAttrFrom.AddStringX(sFType + " - " + CString(pElement->GetDesc()),(long)pElement);
      };
            
      pElement = pElement->GetNextQuery();
   }
   m_cbAttrFrom.SetCurSel(0);

   // Fill list of feature types

   m_cbFTypeTo.Init(BDFTypeSel());
   OnSelchangeFeatureto();

   Update();
	
	return CPropertyPage::OnSetActive();
}

///////////////////////////////////////////////////////////////////////////////

void CPageQueryJoin::OnSelchangeFeatureto() 
{
   CAttrArray aAttr;

   m_cbAttrTo.ResetContent();

  // Retrieve list of attributes for the selected feature type

   long lFTypeId;    
   if (m_cbFTypeTo.GetItemDataX(lFTypeId) != CB_ERR)
   {
      if (BDFTypeAttrInit(BDHandle(), lFTypeId, &aAttr))
      {
         // TODO add the name as a link object         

         int i = 0; for (i = 0; i < aAttr.GetSize(); i++)
         {
            CAttribute* pAttr = aAttr.GetAt(i);

           // Retrieve the attribute name
            CString sDesc = pAttr->GetDesc();
            if (pAttr->GetDataType() == BDMAPLINES) sDesc += " [" + BDString(IDS_POLYLINESTYPE)+"]";
            if (pAttr->GetDataType() == BDCOORD) sDesc += " [" + BDString(IDS_COORDTYPE) + "]";

            // Only support for spatial joins at present

            if (pAttr->GetDataType() == BDMAPLINES || pAttr->GetDataType() == BDCOORD)
            {
               m_cbAttrTo.AddStringX(sDesc, aAttr.GetAt(i)->GetAttrId());
            };
         };
      }
   }
   m_cbAttrTo.SetCurSel(0);

   OnSelchangeAttrto();
	
}

/////////////////////////////////////////////////////////////////////////////

void CPageQueryJoin::OnSelchangeAttrto() 
{
   long lDataType1 = 0, lDataType2 = 0;
   CAttribute attr(0);

	// Display the conditions that are applicable to the selected attributes

   m_cbCondition.ResetContent();

   int index1 = m_cbAttrFrom.GetCurSel();
   int index2 = m_cbAttrTo.GetCurSel();
   if (index1 != CB_ERR && index2 != CB_ERR)
   {
      // Retrieve the data types of the attributes
      CQueryElement *pElement = (CQueryElement*)m_cbAttrFrom.GetItemDataPtr(index1);
      lDataType1 = pElement->GetDataType();
      
      attr.m_lAttrId = m_cbAttrTo.GetItemData(index2);
      m_cbFTypeTo.GetItemDataX(attr.m_lFType);
      
      if (BDFTypeAttr(BDHandle(), &attr, BDSELECT))
      {
         lDataType2 = attr.GetDataType();
      }

      // Now add available conditions

      if (lDataType1 != 0 && lDataType2 != 0)
      {
         if (lDataType1 == BDMAPLINES && lDataType2 == BDMAPLINES)
         {
            m_cbCondition.AddStringX("intersects", CQueryJoin::intersects);
            m_cbCondition.AddStringX("centroid inside", CQueryJoin::centroidinside);
         }         
         if ((lDataType1 == BDCOORD || lDataType1 == BDMAPLINES)
            && lDataType2 == BDMAPLINES) 
         {
            m_cbCondition.AddStringX("inside", CQueryJoin::inside);            
         }
                                    
         if ((lDataType1 == BDMAPLINES || lDataType1 == BDCOORD) && 
             (lDataType2 == BDMAPLINES || lDataType2 == BDCOORD))
         {            
            m_cbCondition.AddStringX("within", CQueryJoin::within);            
         } 

         CheckRadioButton(IDC_INCLUSIVE, IDC_EXCLUSIVE, IDC_INCLUSIVE);

         // TODO add support for other types
         /*else if (lDataType1 == BDNUMBER && lDataType2 == BDNUMBER)
         {
            m_cbCondition.AddStringX("Equal", CQueryJoin::equal);
            m_cbCondition.AddStringX("Not Equal", CQueryJoin::notequal);
         }*/
      };

   }
   m_cbCondition.SetCurSel(0);
   OnSelchangeCondition();
	
}

/////////////////////////////////////////////////////////////////////////////
void CPageQueryJoin::OnSelchangeCondition() 
{
   long lCondition;
	if (m_cbCondition.GetItemDataX(lCondition) != CB_ERR)
   {
      m_eDistance.EnableWindow(lCondition == CQueryJoin::within);
   }
	
}

/////////////////////////////////////////////////////////////////////////////

BOOL CPageQueryJoin::OnKillActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnKillActive();
}

///////////////////////////////////////////////////////////////////////////////

void CPageQueryJoin::OnSector() 
{
	m_cbFTypeTo.OnClickDictionary();		
}

///////////////////////////////////////////////////////////////////////////////

void CPageQueryJoin::OnAdd() 
{
	// Add the spatial join to the query object

   double dDistance = 0;
   CString sDesc;

   int index1 = m_cbAttrFrom.GetCurSel();
   int index2 = m_cbFTypeTo.GetCurSel();
   int index3 = m_cbAttrTo.GetCurSel();
   int index4 = m_cbCondition.GetCurSel();   

   if (index1 != CB_ERR && index2 != CB_ERR && index3 != CB_ERR && index4 != CB_ERR && 
       !m_eDistance.IsWindowEnabled() || m_eDistance.GetValue(dDistance))
   {
      CQueryElement *pQueryElement = (CQueryElement*)m_cbAttrFrom.GetItemDataPtr(index1);
      long lFTypeTo = m_cbFTypeTo.GetItemData(index2);
      long lAttrTo = m_cbAttrTo.GetItemData(index3);
      long lCond = m_cbCondition.GetItemData(index4);
      if (GetCheckedRadioButton(IDC_INCLUSIVE, IDC_EXCLUSIVE) == IDC_EXCLUSIVE)
      {
         lCond |= CQueryJoin::exclusive;
      }

      m_cbCondition.GetLBText(index4, sDesc);
      if (lCond & CQueryJoin::exclusive) sDesc = " not " + sDesc;
      
      CQueryJoin *pQueryJoin = new CQueryJoin;

      pQueryElement->SetQueryJoin(pQueryJoin);
      pQueryJoin->SetFTypeId(lFTypeTo);
      pQueryJoin->SetAttrId(lAttrTo);

      CQueryCond querycond;
      querycond.SetCond(lCond);
      querycond.SetCondValue(dDistance);
      pQueryJoin->GetConditions().Add(querycond);
      pQueryJoin->SetDesc(sDesc);
      pQueryJoin->SetDataType(BDJOIN);

      // Now append the query to the query condition

      CQuery *pQuery = new CQuery;
      pQuery->Initialise(lFTypeTo);
      pQueryJoin->SetNextQuery(pQuery);

      // Display list of joins

      Update();

      // Update the list

      CSheetQuery* pQuerySheet = (CSheetQuery*)GetParent();
      ASSERT(pQuerySheet->IsKindOf(RUNTIME_CLASS(CSheetQuery)));

      pQuerySheet->GetPageQuery()->UpdateList(m_pQuery);

   }  	
}

///////////////////////////////////////////////////////////////////////////////

void CPageQueryJoin::Update()
{
   // Display a list of the joins

   m_lbJoins.ResetContent();

   CQueryElement* pElement = m_pQuery;
   while (pElement != NULL)
   {
      if (pElement->GetQueryJoin() != NULL)
      {
         // First attribute

         CQueryJoin *pQueryJoin = pElement->GetQueryJoin();
         CString sDesc = pElement->GetDesc();
         sDesc = CQuery::StripBrackets(sDesc);

         ASSERT(pQueryJoin->GetConditions().GetSize() > 0);
         CQueryCond &querycond = pQueryJoin->GetConditions().GetAt(0);

         // Condition

         sDesc += " " + CString(pQueryJoin->GetDesc());

         if (querycond.GetCond() & CQueryJoin::within)
         {
            CString s;
            s.Format("%.0lfm of ", querycond.GetCondValue());
            sDesc += " " + s;
         }


         // Second attribute

         CQueryElement *pElement2 = pQueryJoin->GetNextQuery();
         while (pElement2 != NULL && pElement2->GetAttrId() != pQueryJoin->GetAttrId())
         {
            pElement2 = pElement2->GetNextQuery();
         }    
         ASSERT(pElement2 != NULL);
         sDesc += " " + CQuery::StripBrackets(CString(pElement2->GetDesc()));         
         

         m_lbJoins.AddString(sDesc);


      };
            
      pElement = pElement->GetNextQuery();
   }
   

}

///////////////////////////////////////////////////////////////////////////////

void CPageQueryJoin::OnOK() 
{
   *m_pQueryPrev = *m_pQuery;      	  				
	CPropertyPage::OnOK();	
	
	CPropertyPage::OnOK();
}

///////////////////////////////////////////////////////////////////////////////
//
// Deletes the selected join
//

void CPageQueryJoin::OnDelete() 
{
   int i = m_lbJoins.GetCurSel();
   int iCount = 0;

   // Count until reach ith join

   if (i != LB_ERR)
   {
      CQueryElement* pElement = m_pQuery;
      while (pElement != NULL)
      {
         if (pElement->GetQueryJoin() != NULL)
         {         
            iCount++;
         }; 

         // Delete the join

         if (iCount == i+1)
         {
            CQueryJoin* pQueryJoin = pElement->GetQueryJoin();
            delete pQueryJoin;
            pElement->SetQueryJoin(NULL);
            break;
         }
            
         pElement = pElement->GetNextQuery();   
      }
      Update();
   };	
}

///////////////////////////////////////////////////////////////////////////////


