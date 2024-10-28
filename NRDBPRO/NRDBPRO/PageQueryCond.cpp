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
#include <strstream>

#include "nrdb.h"
#include "sheetquery.h"
#include "PageQueryCond.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageQueryCond property page

IMPLEMENT_DYNCREATE(CPageQueryCond, CPropertyPage)

CPageQueryCond::CPageQueryCond(CQuery* pQuery, CQuery* pQueryPrev, int nType) : 
   CPropertyPage(CPageQueryCond::IDD)
{
	m_pQuery = pQuery;
   m_pQueryPrev = pQueryPrev;
   m_nType = nType;

	//{{AFX_DATA_INIT(CPageQueryCond)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT	
}

CPageQueryCond::CPageQueryCond()
{
}

CPageQueryCond::~CPageQueryCond()
{
}

void CPageQueryCond::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageQueryCond)
	DDX_Control(pDX, IDC_TO, m_eTo);
	DDX_Control(pDX, IDC_FROM, m_eFrom);
	DDX_Control(pDX, IDC_DATES, m_cbDates);
   DDX_Control(pDX, IDC_VALUE, m_eValue);
	DDX_Control(pDX, IDC_LBCOND, m_lbCond);
	DDX_Control(pDX, IDC_COND, m_cbCond);
	DDX_Control(pDX, IDC_ATTR, m_cbAttr);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CPageQueryCond, CPropertyPage)
	//{{AFX_MSG_MAP(CPageQueryCond)
	ON_CBN_SELCHANGE(IDC_DATES, OnSelchangeDates)
   ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
   ON_LBN_SELCHANGE(IDC_LBCOND, OnSelChangeCond)
   ON_CBN_SELCHANGE(IDC_ATTR, OnSelChangeAttr)
	//}}AFX_MSG_MAP   

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageQueryCond message handlers

BOOL CPageQueryCond::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	   	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////

BOOL CPageQueryCond::OnSetActive() 
{
   CString sDate;

   // Intialise 

   m_cbDates.ResetContent();
   int i = 0; for (i = 0; m_pQuery->m_aQueryDates[i].m_nID != -1; i++)
   {
      CQueryDate& rQueryDate = m_pQuery->m_aQueryDates[i];
      int index = m_cbDates.AddStringX(rQueryDate.m_psDescription, 
                           rQueryDate.m_nID);
      if (rQueryDate.m_nID == m_pQuery->GetDateCond())
      {
         m_cbDates.SetCurSel(index);
      }    
   };   

   OnSelchangeDates();

   if (m_eFrom.IsWindowEnabled())
   {
      m_pQuery->GetStartDate().DateAsString(sDate);
      m_eFrom.SetWindowText(sDate);
      m_pQuery->GetEndDate().DateAsString(sDate);
      m_eTo.SetWindowText(sDate);
   }

  
   CString sFType;

   // Fill list of numeric attributes

   m_cbAttr.ResetContent();

   // Retrieve currently selected sort elements

   CQueryElement* pElement = m_pQuery;
   while (pElement != NULL)
   {
      // Determine the feature type name

      if (pElement->GetDataType() == BDFTYPE)
      {         
         sFType = CQuery::StripBrackets(pElement->GetDesc());
      }
      
      // Add Numeric Elements to list

      if (pElement->GetDataType() == BDNUMBER
          || pElement->GetDataType() == BDTEXT 
          || (pElement->GetDataType() == BDLONGTEXT && BDGetVersion(BDHandle()) >= 3)
          || pElement->GetDataType() == BDLINK
         )
      {
          int index = m_cbAttr.AddString(sFType + " - " + pElement->GetDesc());
          m_cbAttr.SetItemDataPtr(index, pElement);
      }      
      pElement = pElement->GetNextQuery();
   }   
   m_cbAttr.SetCurSel(0);

   // Add conditions to list

   OnSelChangeAttr();   

   // Restore list of existing conditions

   UpdateList();
	
	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
//
// Set the attributes corresponding to the currently selected datatype
//

void CPageQueryCond::OnSelChangeAttr()
{
   m_cbCond.ResetContent();

   int index = m_cbAttr.GetCurSel();
   if (index != CB_ERR)
   {
      CQueryElement *pElement = (CQueryElement*)m_cbAttr.GetItemDataPtr(index);
      
      // Numeric conditions

      if (pElement->GetDataType() == BDNUMBER)
      {            
         m_cbCond.AddStringX(BDString(IDS_EQUALTO),  CQueryElement::equal);
         m_cbCond.AddStringX(BDString(IDS_NOEQUALTO),  CQueryElement::notequal);
         m_cbCond.AddStringX(BDString(IDS_GREATERTHAN_Q),  CQueryElement::greater);
         m_cbCond.AddStringX(BDString(IDS_GREATEROREQUAL),  CQueryElement::greaterequal);
         m_cbCond.AddStringX(BDString(IDS_LESSTHAN_Q),  CQueryElement::less);
         m_cbCond.AddStringX(BDString(IDS_LESSTHANOREQUAL),  CQueryElement::lessequal);
      }

	  // memo fields are text only in database version 3

      else if ((pElement->GetDataType() == BDLONGTEXT && BDGetVersion(BDHandle()) >= 3)
		       || pElement->GetDataType() == BDTEXT || pElement->GetDataType() == BDLINK)
      {
         m_cbCond.AddStringX(BDString(IDS_EQUALTO),  CQueryElement::equal);
         m_cbCond.AddStringX(BDString(IDS_NOEQUALTO),  CQueryElement::notequal);
         
         m_cbCond.AddStringX(BDString(IDS_CONTAINS),  CQueryElement::contains);
         m_cbCond.AddStringX(BDString(IDS_NOTCONTAINS),  CQueryElement::notcontains);

         if (pElement->GetDataType() != BDLINK)
         {
            m_cbCond.AddStringX(BDString(IDS_LIKE),  CQueryElement::like);
         };         
      } 
      
   };

   m_cbCond.SetCurSel(0);
}

/////////////////////////////////////////////////////////////////////////////

void CPageQueryCond::OnSelchangeDates() 
{
   long lCond;
   m_cbDates.GetItemDataX(lCond);
  
   BOOL bRange = (lCond == CQuery::FirstDateRange || lCond == CQuery::LastDateRange || 
      lCond == CQuery::AllDatesRange);
   BOOL bFrom = (lCond == CQuery::AllDatesAfter || lCond == CQuery::AllDatesBefore);


   m_eFrom.EnableWindow(bRange || bFrom);
   m_eTo.EnableWindow(bRange);   
   GetDlgItem(IDS_TO)->EnableWindow(bRange);	

}

///////////////////////////////////////////////////////////////////////////////


void CPageQueryCond::UpdateList()
{

   CString s;

   // Produce list of existing conditions

   m_lbCond.ResetContent();

   CQueryElement* pElement = m_pQuery;
   while (pElement != NULL)
   {
      int i = 0; for (i = 0; i < pElement->GetConditions().GetSize(); i++)
      {       
         int nCond = pElement->GetConditions().GetAt(i).GetCond();
         double dCondValue = pElement->GetConditions().GetAt(i).GetCondValue();
         CString sCondValue = pElement->GetConditions().GetAt(i).GetCondValueStr();

         s = pElement->GetDesc();

         // Condition

         switch (nCond)
         {
            case CQueryElement::equal : s += " = "; break;
            case CQueryElement::notequal : s += " <> "; break;
            case CQueryElement::greater : s += " > "; break;
            case CQueryElement::greaterequal : s += " >= "; break;
            case CQueryElement::less : s += " < "; break;
            case CQueryElement::lessequal : s += " >= "; break;
            case CQueryElement::contains : s += " " + BDString(IDS_CONTAINS) + " "; break;
            case CQueryElement::notcontains : s += " " + BDString(IDS_NOTCONTAINS) + " "; break;
            case CQueryElement::like : s += " " + BDString(IDS_LIKE) + " "; break;
         }

         // Numeric value

         if (pElement->GetDataType() == BDNUMBER)
         {
			 std::strstream str;
            str.precision(10);
            str << dCondValue << std::ends;
            s += str.str();
            str.rdbuf()->freeze(0);
         }
         
         // String value

         else if (pElement->GetDataType() == BDTEXT || 
                  pElement->GetDataType() == BDLONGTEXT || 
                  pElement->GetDataType() == BDLINK)
         {
            s += "'" + sCondValue + "'";
         }
         

         int index = m_lbCond.AddString(s);
         m_lbCond.SetItemDataPtr(index, pElement);
      }

      pElement = pElement->GetNextQuery();
   };

   OnSelChangeCond();
}


///////////////////////////////////////////////////////////////////////////////

BOOL CPageQueryCond::OnKillActive() 
{
   long lCond;
   CString sFrom, sTo;
   CDateTime dtFrom, dtTo;
   BOOL bOK = TRUE;

   m_cbDates.GetItemDataX(lCond);

   // Retrieve dates

   if (m_eFrom.IsWindowEnabled())
   {
      m_eFrom.GetWindowText(sFrom);   
      bOK = dtFrom.StringAsDate(sFrom);
   };

   if (bOK && m_eTo.IsWindowEnabled())
   {
      m_eTo.GetWindowText(sTo);
      bOK = dtTo.StringAsDate(sTo);
   }
   
   if (bOK && dtFrom.IsValid() && dtFrom.IsValid() && m_eTo.IsWindowEnabled())
   {
      bOK = dtFrom <= dtTo;                 
   }

   if (!bOK)
   {
      AfxMessageBox(BDString(IDS_DATEFORMAT));
      return 0;
   };

   // Save values

   if (bOK)
   {
      m_pQuery->SetStartDate(dtFrom);
      m_pQuery->SetEndDate(dtTo);
	   m_pQuery->SetDateCond(lCond);
   };
   	
	// Save the query as the default for the next time the dialog is used  

   if (bOK)
   {
       return CPropertyPage::OnKillActive();
   } 
   return FALSE;
};

///////////////////////////////////////////////////////////////////////////////
//
// Retrieve condition and add it to the list
//


void CPageQueryCond::OnAdd() 
{
   double dValue;
   CString sValue;
   CQueryCond cond;

   // Retrieve attribute and condition

   int index = m_cbAttr.GetCurSel();
   int iCond = m_cbCond.GetCurSel();

   if (index != CB_ERR && iCond != CB_ERR)
   {
      CQueryElement* pElement = (CQueryElement*)m_cbAttr.GetItemDataPtr(index);
      int nCond = m_cbCond.GetItemData(iCond);

      // Numeric conditions

      if (pElement->GetDataType() == BDNUMBER)
      {         
         // Get numeric value

         if (m_eValue.GetValue(dValue, 0))
         {
            cond.SetCond(nCond);
            cond.SetCondValue(dValue);
            pElement->GetConditions().Add(cond);
            
            UpdateList();
         }
      } 
      // Text conditions

      else if (pElement->GetDataType() == BDTEXT || pElement->GetDataType() == BDLONGTEXT ||
               pElement->GetDataType() == BDLINK)
      {
         if (m_eValue.GetValue(sValue))
         {
            cond.SetCond(nCond);
            cond.SetCondValueStr(sValue);
            pElement->GetConditions().Add(cond);
            
            UpdateList();
         }
      }
   }	
}


///////////////////////////////////////////////////////////////////////////////


void CPageQueryCond::OnDelete() 
{
   int index = m_lbCond.GetCurSel();
   if (index != LB_ERR)
   {
      CQueryElement *pElement = (CQueryElement*)m_lbCond.GetItemDataPtr(index);
      pElement->GetConditions().RemoveAll();

      // TODO, remove only the selected condition
      
      

      UpdateList();
   }   
}


///////////////////////////////////////////////////////////////////////////////
//
// Update the controls to display the latest selection
//


void CPageQueryCond::OnSelChangeCond()
{
/* TODO!!!   

  int index = m_lbCond.GetCurSel();
   if (index != LB_ERR)
   {
      CQueryElement *pElement = (CQueryElement*)m_lbCond.GetItemDataPtr(index);

      // Search for the attribute

      int i = 0; for (i = 0; i < m_cbAttr.GetCount(); i++)
      {
         CQueryElement* pElementL = (CQueryElement*)m_cbAttr.GetItemDataPtr(i);
         if (pElementL == pElement) 
         {
            m_cbAttr.SetCurSel(i);
            break;
         }
      }

      // Find the condition

      for (i = 0; i < m_cbCond.GetCount(); i++)
      {
         if (pElement->GetCond() == (int)m_cbCond.GetItemData(i))            
         {
            m_cbCond.SetCurSel(i);
            break;
         };
      }

      // Set the numerical value

      if (pElement->GetDataType() == BDNUMBER)
      {
         strstream str;
         str.precision(10);
         str << pElement->GetCondValue() << std::ends;      
         m_eValue.SetWindowText(str.str());
         str.rdbuf()->freeze(0);
      } 
      else if (pElement->GetDataType() == BDTEXT || 
               pElement->GetDataType() == BDLONGTEXT || 
               pElement->GetDataType() == BDLINK)
      {
         m_eValue.SetWindowText(pElement->GetCondValueStr());
      }
   }   
   */
}

	
///////////////////////////////////////////////////////////////////////////////

void CPageQueryCond::OnOK() 
{
   *m_pQueryPrev = *m_pQuery;      	  		
	CPropertyPage::OnOK();
}
