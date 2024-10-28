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
#include "PageQuerystats.h"
#include "sheetquery.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageQueryStats property page

IMPLEMENT_DYNCREATE(CPageQueryStats, CPropertyPage)

CPageQueryStats::CPageQueryStats(CQuery* pQuery, CQuery* pQueryPrev, int nType) : CPropertyPage(CPageQueryStats::IDD)
{
   m_pQuery = pQuery;
   m_pQueryPrev = pQueryPrev;
   m_nType = nType;
   m_nStatistic = 0;

	//{{AFX_DATA_INIT(CPageQueryStats)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPageQueryStats::CPageQueryStats()
{
}

CPageQueryStats::~CPageQueryStats()
{
}

void CPageQueryStats::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageQueryStats)
	DDX_Control(pDX, IDC_STATISTIC2, m_cbStatistic);	
	DDX_Control(pDX, IDC_GROUPBY, m_cbGroupBy);
	DDX_Control(pDX, IDC_ATTR, m_cbAttr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageQueryStats, CPropertyPage)
	//{{AFX_MSG_MAP(CPageQueryStats)
	ON_CBN_SELCHANGE(IDC_STATISTIC2, OnSelchangeStatistic)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CPageQueryStats::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
  	
   m_cbStatistic.AddStringX(BDString(IDS_NONE), CQueryElement::none);
   m_cbStatistic.AddStringX(BDString(IDS_TOTAL), CQueryElement::total);
   m_cbStatistic.AddStringX(BDString(IDS_COUNTUNIQUE), CQueryElement::countunique);
   m_cbStatistic.AddStringX(BDString(IDS_MEAN), CQueryElement::mean);
   m_cbStatistic.AddStringX(BDString(IDS_COUNT), CQueryElement::count);
   m_cbStatistic.AddStringX(BDString(IDS_MAX), CQueryElement::max);
   m_cbStatistic.AddStringX(BDString(IDS_MIN), CQueryElement::min);

   m_cbStatistic.SetCurSel(0);
   OnSelchangeStatistic();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
//
// Fills combo boxes according to the selected statistic
//

void CPageQueryStats::OnSelchangeStatistic() 
{
   CString sFType;   

   int indexS = m_cbStatistic.GetCurSel();
   int indexA, indexG;
   ASSERT(indexS != CB_ERR);
   DWORD nStat = m_cbStatistic.GetItemData(indexS);

   BOOL bNone = nStat == CQueryElement::none;
   BOOL bCount = nStat == CQueryElement::count;
   BOOL bCountUnique = nStat == CQueryElement::countunique;
   BOOL bNumeric = !bNone && !bCount && !bCountUnique;

   // List all selected attributes

   m_cbGroupBy.ResetContent();
   m_cbAttr.ResetContent();

   if (m_nType != CSheetQuery::Map)
   {
      indexG = m_cbGroupBy.AddStringX(BDString(IDS_NONE), NULL);
      m_cbGroupBy.SetCurSel(indexG);
   };
   
   // Retrieve currently selected sort elements

   CQueryElement* pElement = m_pQuery;
   while (pElement != NULL)
   {
      // Determine the feature type name

      if (pElement->GetDataType() == BDFTYPE)
      {
         sFType = pElement->GetDesc();
      }
      CString s = CQuery::StripBrackets(sFType);
      if (pElement->GetDataType() == BDQUERYSTATS) s = m_pQuery->GetDesc();

      // Add Elements

      if ((pElement->GetSelected() || pElement->GetDataType() == BDQUERYSTATS))
      {                                    
         // List all attributes of type number

         if (bNumeric && (pElement->GetDataType() == BDNUMBER || pElement->GetDataType() == BDQUERYSTATS) || 
             nStat == CQueryElement::countunique) 
         {            
            indexA = m_cbAttr.AddStringX(s + " - " + CString(pElement->GetDesc()), 
                              (long)pElement);                        
         }          

         // Group By                 

         if ((m_nType != CSheetQuery::Map && pElement->GetDataType() != BDMAPLINES) || 
                  (m_nType == CSheetQuery::Map && (pElement->GetDataType() == BDCOORD 
                  || pElement->GetDataType() == BDMAPLINES)))
            
         {

            // For features check that the corresponding ftype has already been added
                           
               if (pElement->GetDataType() != BDFEATURE ||
                   m_cbGroupBy.FindString(-1, s + " - " + pElement->GetDesc()) == CB_ERR)
               {                  

                  indexG = m_cbGroupBy.AddStringX(s + " - " + CString(pElement->GetDesc()), 
                                    (long)pElement);
                  if (pElement->GetGroupBy()) m_cbGroupBy.SetCurSel(indexG);          

                  if (pElement->GetDataType() == BDDATE)
                  {
                      indexG = m_cbGroupBy.AddStringX(s + " - " + BDString(IDS_YEAR), 
                                                     (long)pElement);
                      if (pElement->GetGroupBy() == CQuery::year) m_cbGroupBy.SetCurSel(indexG);              

                      indexG = m_cbGroupBy.AddStringX(s + " - " + BDString(IDS_MONTH), 
                                                     (long)pElement);
                      if (pElement->GetGroupBy() == CQuery::month) m_cbGroupBy.SetCurSel(indexG);              
                  }                             
               };
         }         

         // Determine the statistic and set the active 

         if (pElement->GetGroupStat() != CQueryElement::none)
         {
            m_nStatistic = pElement->GetGroupStat();
            m_cbAttr.SetCurSel(indexA);
         };
      }        
      pElement = pElement->GetNextQuery();
   }   

   // Set default attribute

  if (m_cbAttr.GetCurSel() == CB_ERR) m_cbAttr.SetCurSel(0);

   // Disable controls according to selected statistic

   m_cbGroupBy.EnableWindow(!bNone);
   m_cbAttr.EnableWindow(!bNone && !bCount);	
}

/////////////////////////////////////////////////////////////////////////////
//
// Update the controls based on the currently active query
//

BOOL CPageQueryStats::OnSetActive() 
{
   // Fill values

   OnSelchangeStatistic();

   // Set the selected statistic

   int i = 0; for (i = 0; i < m_cbStatistic.GetCount(); i++)
   {
      if ((int)m_cbStatistic.GetItemData(i) == m_nStatistic) m_cbStatistic.SetCurSel(i);
   }

   // If no selections are made then set defaults   

   if (m_cbGroupBy.GetCurSel() == CB_ERR) m_cbGroupBy.SetCurSel(0);
   if (m_cbAttr.GetCurSel() == CB_ERR) m_cbAttr.SetCurSel(0);

   // Recall to enable/disable controls

   OnSelchangeStatistic();
     	
	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
//
// Save the selections
//

BOOL CPageQueryStats::OnKillActive() 
{
   CQueryElement* pGroupBy = NULL;
   CQueryElement* pAttr = NULL;
   CString s;
   int nGroupBy = CQuery::yes;

   // Retrieve selected statistic

   int index = m_cbStatistic.GetCurSel();
   ASSERT(index != CB_ERR);
   int nStatistic = m_cbStatistic.GetItemData(index);

   // Retrieve selected group by and attribute

   if (nStatistic != CQueryElement::none)
   {
      index = m_cbGroupBy.GetCurSel();
      if (index != CB_ERR)
      {
         pGroupBy = (CQueryElement*)m_cbGroupBy.GetItemDataPtr(index);
      
         m_cbGroupBy.GetWindowText(s);
         if (s.Find(BDString(IDS_YEAR)) > -1) nGroupBy = CQuery::year;
         else if (s.Find(BDString(IDS_MONTH)) > -1) nGroupBy = CQuery::month;

      } else 
      {
         AfxMessageBox(BDString(IDS_NOGROUPBY));
         return FALSE;
      }   

      // Must have a 'group by' for count

      if (nStatistic == CQuery::count && pGroupBy == NULL)
      {
         AfxMessageBox(BDString(IDS_NOGROUPBY));
         return FALSE;
      }
   
      if (nStatistic != CQueryElement::count)
      {
         index = m_cbAttr.GetCurSel();
         if (index != CB_ERR)
         {
            pAttr = (CQueryElement*)m_cbAttr.GetItemDataPtr(index);
         } else
         {
            if (nStatistic == CQueryElement::countunique)
            { 
                AfxMessageBox(BDString(IDS_SELATTR));
            } else
            {
               AfxMessageBox(BDString(IDS_NONUMSEL));
            };
            return FALSE;
         }

         // Check that the groupby and the attribute are not the same

         if (pAttr == pGroupBy)
         {
            AfxMessageBox(IDS_GROUPBYATTRSAME);
            return FALSE;
         }
      } else
      {
         pAttr = pGroupBy;
      }
   };

   // Search through query
   
   CQueryElement* pElement = m_pQuery;
   while (pElement != NULL)      
   {                  
      // Determine group by 

      if (pElement == pGroupBy) pElement->SetGroupBy(nGroupBy);
      else pElement->SetGroupBy(FALSE);

      // Determine statistic attribute

      if (pElement == pAttr) pElement->SetGroupStat(nStatistic);
      else pElement->SetGroupStat(CQueryElement::none);
      
      pElement = pElement->GetNextQuery();
   }   
	
	return CPropertyPage::OnKillActive();
}

/////////////////////////////////////////////////////////////////////////////

void CPageQueryStats::OnOK() 
{
	*m_pQueryPrev = *m_pQuery;      	  				
	CPropertyPage::OnOK();
}




