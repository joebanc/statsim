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
#include "sheetquery.h"
#include "PageQueryCalc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageQueryCalc property page

IMPLEMENT_DYNCREATE(CPageQueryCalc, CPropertyPage)

CPageQueryCalc::CPageQueryCalc(CQuery* pQuery, CQuery* pQueryPrev, int nType) : CPropertyPage(CPageQueryCalc::IDD)
{
   m_pQuery = pQuery;
   m_pQueryPrev = pQueryPrev;
   m_nType = nType;
   m_bChanged = FALSE; 

	//{{AFX_DATA_INIT(CPageQueryCalc)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPageQueryCalc::CPageQueryCalc()
{
   m_bChanged = FALSE;
}

CPageQueryCalc::~CPageQueryCalc()
{
}

void CPageQueryCalc::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageQueryCalc)
	DDX_Control(pDX, IDC_VALUE1, m_eValue1);
	DDX_Control(pDX, IDC_DECPLACES, m_eDecPlaces);
	DDX_Control(pDX, IDC_STATSNAME, m_eStatsName);
	DDX_Control(pDX, IDC_QUERYSTATS, m_lbQueryStats);
	DDX_Control(pDX, IDC_OPERATOR, m_cbOperator);
	DDX_Control(pDX, IDC_ATTR2, m_cbAttr2);
	DDX_Control(pDX, IDC_ATTR1, m_cbAttr1);
	DDX_Control(pDX, IDC_VALUE2, m_eValue2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageQueryCalc, CPropertyPage)
	//{{AFX_MSG_MAP(CPageQueryCalc)
	ON_LBN_SELCHANGE(IDC_QUERYSTATS, OnSelchangeQuerystats)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_CBN_SELCHANGE(IDC_ATTR1, OnSelchange)
	ON_CBN_SELCHANGE(IDC_ATTR2, OnSelchange)
	ON_CBN_SELCHANGE(IDC_OPERATOR, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


///////////////////////////////////////////////////////////////////////////////

BOOL CPageQueryCalc::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
   
   // Initialise statistics

   OnSelchangeQuerystats();

   OnSelchange();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////

void CPageQueryCalc::Update()
{
   m_cbAttr1.ResetContent();
   m_cbAttr2.ResetContent();

   m_eDecPlaces.SetValue(0);

      // Initialise controls according to the current feature type

   CString sFType;
   CQueryElement* pElement = m_pQuery;
   while (pElement != NULL)
   {
      if (pElement->GetDataType() == BDFTYPE)
      {
         sFType = CQuery::StripBrackets(pElement->GetDesc());
      }
      if (pElement->GetDataType() == BDNUMBER)
      {
         m_cbAttr1.AddStringX(sFType + " - " + CString(pElement->GetDesc()), 
                              (long)pElement);
         m_cbAttr2.AddStringX(sFType + " - " + CString(pElement->GetDesc()), 
                              (long)pElement);
      }

      // Add attributes for area and length of polygons (includes polylines as cannot 
      // distinguish at this stage)

      if (pElement->GetDataType() == BDMAPLINES)
      {
         CString s = CQuery::StripBrackets(CString(pElement->GetDesc()));
         CString sAreaUnits = BDGetApp()->GetUnits().GetAreaUnit().m_sAbbr;
         CString sLengthUnits = BDGetApp()->GetUnits().GetLengthUnit().m_sAbbr;

         m_cbAttr1.AddStringX(sFType + " - " + s + " [" + BDString(IDS_AREASQM) + ", " + sAreaUnits +"]",
                              (long)pElement);
         m_cbAttr1.AddStringX(sFType + " - " + s + " [" + BDString(IDS_LENGTHM) + ", " + sLengthUnits + "]",
                              (long)pElement);
         m_cbAttr1.AddStringX(sFType + " - " + s + " [" + BDString(IDS_CENTROID) + "]",
                              (long)pElement);
         m_cbAttr2.AddStringX(sFType + " - " + s + " [" + BDString(IDS_AREASQM) + ", " + sAreaUnits + "]",
                              (long)pElement);
         m_cbAttr2.AddStringX(sFType + " - " + s + "[" + BDString(IDS_LENGTHM) + ", " + sLengthUnits + "]",
                              (long)pElement);                                      
      }

      pElement = pElement->GetNextQuery();
   }   

   // Add 'value' to attribute lists

   m_cbAttr1.AddStringX(BDString(IDS_VALUE), NULL);
   m_cbAttr2.AddStringX(BDString(IDS_VALUE), NULL);

   // Set default values

   if (m_cbAttr1.GetCurSel() == CB_ERR) m_cbAttr1.SetCurSel(0);
   if (m_cbAttr2.GetCurSel() == CB_ERR) m_cbAttr2.SetCurSel(0);

   // Fill list of defined query statistics
   
   UpdateQueryStats();
}

/////////////////////////////////////////////////////////////////////////////

void CPageQueryCalc::UpdateQueryStats()
{
   m_lbQueryStats.ResetContent();

   // List already defined query stats

   CQueryElement* pElement = m_pQuery;
   while (pElement != NULL)
   {
      if (pElement->GetDataType() == BDQUERYSTATS)
      {         
         int index = m_lbQueryStats.AddString(pElement->GetDesc());
         m_lbQueryStats.SetItemDataPtr(index, pElement);
      }      
      pElement = pElement->GetNextQuery();
   }  
   
   m_lbQueryStats.SetCurSel(0);
   OnSelchangeQuerystats();  
}

/////////////////////////////////////////////////////////////////////////////

void CPageQueryCalc::OnSelchangeQuerystats() 
{
   int index = m_lbQueryStats.GetCurSel();
   if (index != CB_ERR)
   {
      CQueryCalc* pStats = (CQueryCalc*)m_lbQueryStats.GetItemDataPtr(index);

      // Recover the first attribute

      int i = 0; for (i = 0; i < m_cbAttr1.GetCount(); i++)
      {         
         CQueryElement* pElement = (CQueryElement*)m_cbAttr1.GetItemDataPtr(i);

         if (pElement != NULL)
         {
            if (pElement->GetAttrId() == pStats->GetAttrId() &&
                pElement->GetFTypeId() == pStats->GetFTypeId())
            {
               m_cbAttr1.SetCurSel(i);
               break;
            }             
         } else
         {
            if (pStats->GetAttrId() == 0)
            {
               m_cbAttr1.SetCurSel(i);
               m_eValue1.SetValue(pStats->GetValue1());
               break;
            }
         }
      }

      // Recover the operator

      for (i = 0; i < m_cbOperator.GetCount(); i++)
      {
         if ((int)m_cbOperator.GetItemData(i) == pStats->GetStatistic())
         {
            m_cbOperator.SetCurSel(i);
            break;
         }
      }

      // Recover the second attribute

      for (i = 0; i < m_cbAttr2.GetCount(); i++)
      {         
         CQueryElement* pElement = (CQueryElement*)m_cbAttr2.GetItemDataPtr(i);

         if (pElement != NULL)
         {
            if (pElement->GetAttrId() == pStats->GetAttrId2() &&
                pElement->GetFTypeId() == pStats->GetFTypeId2())
            {
               m_cbAttr2.SetCurSel(i);
               break;
            }             
         } 
         else
         {
            if (pStats->GetAttrId2() == 0)
            {
               m_cbAttr2.SetCurSel(i);
               m_eValue2.SetValue(pStats->GetValue2());
               break;
            }
         }

      }

      // Recover the name of the statistic

      m_eStatsName.SetWindowText(pStats->GetDesc());

      // Recover the number of decimal places

      m_eDecPlaces.SetValue(pStats->GetDecPlaces());

      // Reset window enabled status

      OnSelchange();
   }
	
}

/////////////////////////////////////////////////////////////////////////////
// CPageQueryCalc message handlers

BOOL CPageQueryCalc::OnSetActive() 
{
   Update();
   UpdateOperators();
   OnSelchange();
	
	return CPropertyPage::OnSetActive();
}

/////////////////////////////////////////////////////////////////////////////

void CPageQueryCalc::OnAdd() 
{
   CQueryCalc calc;
   CString sName;
   CString sAttr;
   CQueryElement* pElement = NULL;

   calc.SetDataType(BDQUERYSTATS);

  // Get First Attribute

   int index = m_cbAttr1.GetCurSel();
   if (index != CB_ERR)
   {
      if (m_cbAttr1.GetItemDataPtr(index) != NULL)
      {
         pElement = (CQueryElement*)m_cbAttr1.GetItemDataPtr(index);
         calc.SetFTypeId(pElement->GetFTypeId());
         calc.SetAttrId(pElement->GetAttrId());

         if (pElement->GetDataType() == BDMAPLINES)
         {
            m_cbAttr1.GetLBText(index, sAttr);
            if (sAttr.Find(BDString(IDS_AREASQM)) != -1) calc.SetQualifier(BDAREA);
            if (sAttr.Find(BDString(IDS_LENGTHM)) != -1) calc.SetQualifier(BDLENGTH);
            if (sAttr.Find(BDString(IDS_CENTROID)) != -1) calc.SetQualifier(BDCOORD);
         }
      } else
      {
         double d;
         if (m_eValue1.GetValue(d))
         {
            calc.SetValue1(d);
         } else
         {
            return;
         }
      }

     // Get Operator

      index = m_cbOperator.GetCurSel();
      if (index != CB_ERR)
      {
         int nCalc = m_cbOperator.GetItemData(index);
         calc.SetStatistic(nCalc);

         // Get Second Attribute

         index = m_cbAttr2.GetCurSel();
         if (index != CB_ERR)
         {
            if (nCalc != CQueryCalc::none)
            {
               CQueryElement* pElement = (CQueryElement*)m_cbAttr2.GetItemDataPtr(index);
               if (pElement != NULL)
               {               
                  calc.SetFTypeId2(pElement->GetFTypeId());
                  calc.SetAttrId2(pElement->GetAttrId());

                  if (pElement->GetDataType() == BDMAPLINES)
                  {
                     m_cbAttr2.GetLBText(index, sAttr);
                     if (sAttr.Find(BDString(IDS_AREASQM)) != -1) calc.SetQualifier2(BDAREA);
                     if (sAttr.Find(BDString(IDS_LENGTHM)) != -1) calc.SetQualifier2(BDLENGTH);
                  }
               } 
               // Value
               else
               {
                  double d;
                  if (m_eValue2.GetValue(d))
                  {
                     calc.SetValue2(d);
                  } else
                  {
                     return;
                  }
               }
            }

            // Get the number of decimal places

            int nDP;
            if (m_eDecPlaces.GetValue(nDP, 0, 10))
            {
                calc.SetDecPlaces(nDP);
            } else
            {
               return;
            }

            // Get Name

            m_eStatsName.GetWindowText(sName);
            sName.TrimLeft();
            if (sName.GetLength() != 0)
            {
               calc.SetDesc(sName);

              // Add to list

               CQueryCalc* pStats = new CQueryCalc(calc);               
               
               pElement = m_pQuery;
               while (pElement->GetNextQuery() != NULL)
               {
                  pElement = pElement->GetNextQuery();
               }
               ASSERT(pElement != NULL);
               pElement->SetNextQuery(pStats);

              // Update the list

               UpdateQueryStats();
               
            } else
            {
               AfxMessageBox(BDString(IDS_NOCALCNAME));
               m_eStatsName.SetFocus();
            }

         }         
      }
   }   	
   m_bChanged = FALSE;
}

/////////////////////////////////////////////////////////////////////////////

void CPageQueryCalc::OnDelete() 
{
   int index = m_lbQueryStats.GetCurSel();
   if (index != CB_ERR)
   {
      CQueryCalc* pStats = (CQueryCalc*)m_lbQueryStats.GetItemDataPtr(index);

      // Remove stats from list

      m_lbQueryStats.DeleteString(index);

      // Remove from query

      CQueryElement* pElement = m_pQuery;
      while (pElement != NULL)
      {
         if (pElement->GetNextQuery() == pStats)
         {
            CQueryElement* pNext = pStats->GetNextQuery();

            // Prevent other calculations being deleted
            pStats->SetNextQuery(NULL); 
            delete pStats;

            pElement->SetNextQuery(pNext);
            
            break;
         }         
         pElement = pElement->GetNextQuery();
      }
   }
	
}

/////////////////////////////////////////////////////////////////////////////
//
// Set a default name

void CPageQueryCalc::OnSelchange() 
{
   CQueryElement *pElement1 = NULL, *pElement2 = NULL;
   int nCalc = 0;
   CString sName1, sName2, sCalc;
   int i;

   // Update the operators based on the first attribute

   UpdateOperators();

   // Retrieve selections

   int index = m_cbAttr1.GetCurSel();
   if (index != CB_ERR)
   {
      pElement1 = (CQueryElement*)m_cbAttr1.GetItemDataPtr(index);
      m_cbAttr1.GetLBText(index, sName1);
      
     // Get Operator

      index = m_cbOperator.GetCurSel();
      if (index != CB_ERR)
      {
         nCalc = m_cbOperator.GetItemData(index);

         // If no operator then disable second attribute

         BOOL bEnable = (nCalc != CQueryCalc::none);
         m_cbAttr2.EnableWindow(bEnable); 
         
         // Get Second Attribute

         index = m_cbAttr2.GetCurSel();
         if (index != CB_ERR && m_cbAttr2.IsWindowEnabled() && m_cbAttr2.GetItemDataPtr(index) != NULL)
         {
            pElement2 = (CQueryElement*)m_cbAttr2.GetItemDataPtr(index);
            m_cbAttr2.GetLBText(index, sName2);
         }         
         
      }
   }   	

   // Create name
   
   if (pElement1 != NULL) 
   {
      i = sName1.Find('[');  
      if (i >= 0) sCalc = sName1.Mid(i+1,sName1.Find(']',i)-i-1);
      else sCalc = sName1;

      //if (nCalc == CQueryCalc::area) sCalc += " " + BDString(IDS_AREASQM);
   };

   if (pElement1 != NULL && pElement2 != NULL)
   {
      if (nCalc == CQueryCalc::add) sCalc += " + ";
      if (nCalc == CQueryCalc::subtract) sCalc += " - ";
      if (nCalc == CQueryCalc::multiply) sCalc += " x ";
      if (nCalc == CQueryCalc::divide) sCalc += " / ";
      if (nCalc == CQueryCalc::percent) sCalc += " % ";      
   };

   if (pElement2 != NULL) 
   {
      //sName = pElement2->GetDesc();
      i = sName2.Find('[');
      if (i >= 0) sCalc += sName2.Mid(i+1,sName2.Find(']',i)-i-1);
      else sCalc += sName2;
   };

   m_eStatsName.SetWindowText(sCalc);

   m_bChanged = TRUE;

   // Enable value control as required

   m_eValue1.EnableWindow(m_cbAttr1.GetItemData(m_cbAttr1.GetCurSel()) == NULL);
   m_eValue2.EnableWindow(m_cbAttr2.GetItemData(m_cbAttr2.GetCurSel()) == NULL);
   
}

///////////////////////////////////////////////////////////////////////////////
//
// Set available operators for the currently selected attribute
//

void CPageQueryCalc::UpdateOperators()
{
   CQueryElement *pElement1 = NULL;
   int nCalc = -1;

   int index = m_cbAttr1.GetCurSel();
   if (index != CB_ERR)
   {
      pElement1 = (CQueryElement*)m_cbAttr1.GetItemDataPtr(index);      
      index = m_cbOperator.GetCurSel();
      if (index != CB_ERR)
      {
         nCalc = m_cbOperator.GetItemData(index);
      };

      m_cbOperator.ResetContent();

      // Add numeric operators

      if (pElement1 == NULL || pElement1->GetDataType() == BDNUMBER ||
          pElement1->GetDataType() == BDMAPLINES)
      {
         m_cbOperator.AddStringX(BDString(IDS_NONE),CQueryCalc::none);
         m_cbOperator.AddStringX(BDString(IDS_ADD),CQueryCalc::add);
         m_cbOperator.AddStringX(BDString(IDS_SUBTRACT),CQueryCalc::subtract);
         m_cbOperator.AddStringX(BDString(IDS_DIVIDE),CQueryCalc::divide);
         m_cbOperator.AddStringX(BDString(IDS_MULTIPLY),CQueryCalc::multiply);
         m_cbOperator.AddStringX(BDString(IDS_PERCENT),CQueryCalc::percent);	
      }       

      // Restore previous selection

      int i = 0; for (i = 0; i < m_cbOperator.GetCount(); i++)
      {
         if ((int)m_cbOperator.GetItemData(i) == nCalc) m_cbOperator.SetCurSel(i);
      }
      if (m_cbOperator.GetCurSel() == CB_ERR) m_cbOperator.SetCurSel(0);

      index = m_cbOperator.GetCurSel();
      nCalc = m_cbOperator.GetItemData(index);

      m_cbAttr2.EnableWindow(nCalc != CQueryCalc::area);      
   }   

}

///////////////////////////////////////////////////////////////////////////////

BOOL CPageQueryCalc::OnKillActive() 
{
   CString sName;
   m_eStatsName.GetWindowText(sName);
   sName.TrimRight();

   /*if (m_bChanged)
   {
      int nRet = AfxMessageBox(BDString(IDS_ADDCALC) + sName + "?", MB_YESNOCANCEL);
      if (nRet == IDYES) OnAdd();
      else if (nRet == IDCANCEL) return FALSE;
   }
   m_bChanged = FALSE;*/

	return CPropertyPage::OnKillActive();
}


/////////////////////////////////////////////////////////////////////////////

void CPageQueryCalc::OnOK() 
{
   *m_pQueryPrev = *m_pQuery;      	  			
	CPropertyPage::OnOK();
}

/////////////////////////////////////////////////////////////////////////////

