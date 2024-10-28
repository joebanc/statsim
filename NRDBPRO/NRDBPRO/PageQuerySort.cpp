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
#include "PageQuerySort.h"
#include "query.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

class CQuery;

/////////////////////////////////////////////////////////////////////////////
// CPageQuerySort property page

IMPLEMENT_DYNCREATE(CPageQuerySort, CPropertyPage)

CPageQuerySort::CPageQuerySort(CQuery* pQuery, CQuery* pQueryPrev, int nType) : CPropertyPage(CPageQuerySort::IDD)
{
   m_pQuery = pQuery;
   m_pQueryPrev = pQueryPrev;
   m_nType = nType;

	//{{AFX_DATA_INIT(CPageQuerySort)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPageQuerySort::CPageQuerySort()
{
}

CPageQuerySort::~CPageQuerySort()
{
}

void CPageQuerySort::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageQuerySort)
	DDX_Control(pDX, IDC_SORTATTR, m_lbSortAttr);
	DDX_Control(pDX, IDC_SORTBY, m_cbSortBy);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageQuerySort, CPropertyPage)
	//{{AFX_MSG_MAP(CPageQuerySort)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN, OnDeltaposUpdown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageQuerySort message handlers

BOOL CPageQuerySort::OnSetActive() 
{
   CString sFType;

   // Fill list of objects to sort by

   m_cbSortBy.ResetContent();
   m_lbSortAttr.ResetContent();

   CQueryElement* pElement = m_pQuery;
   while (pElement != NULL)
   {
      if (pElement->GetDataType() == BDFTYPE)
      {
         sFType = pElement->GetDesc();
      }
      CString s = CQuery::StripBrackets(sFType);
      if (pElement->GetDataType() == BDQUERYSTATS) s = m_pQuery->GetDesc();

      // Add to list of attributes that may be used to sort

      if ((pElement->GetSelected() || pElement->GetDataType() == BDQUERYSTATS))
      {
         if (pElement->GetDataType() != BDMAPLINES && pElement->GetDataType() != BDIMAGE && 
             pElement->GetDataType() != BDFILE)
         {

            if (m_cbSortBy.FindString(-1, s + " - " + CString(pElement->GetDesc())) == CB_ERR)
            {
               int index = m_cbSortBy.AddString(s + " - " + CString(pElement->GetDesc()));
               m_cbSortBy.SetItemDataPtr(index, pElement);
               if (pElement->GetSortBy()) m_cbSortBy.SetCurSel(index);                         
            };
         };
      }      

      pElement = pElement->GetNextQuery();
   }

   m_cbSortBy.SetCurSel(0);

   // Add to list those already selected for sorting

   int nId = 1;
   BOOL bFound;

   do
   {
      bFound = FALSE;
      CQueryElement* pElement = m_pQuery;

      while (pElement != NULL)
      {
         if (pElement->GetSortBy() == nId)
         {
            int index = m_lbSortAttr.AddString(pElement->GetDesc());
            m_lbSortAttr.SetItemDataPtr(index, pElement);
            bFound = TRUE;
            break;
         }
         pElement = pElement->GetNextQuery();
      }
      nId++;
   } while (bFound);



   
	
	return CPropertyPage::OnSetActive();
}

///////////////////////////////////////////////////////////////////////////////

void CPageQuerySort::OnAdd() 
{
   int index = m_cbSortBy.GetCurSel();
   if (index != CB_ERR)
   {
      CQueryElement* pElement = (CQueryElement*)m_cbSortBy.GetItemDataPtr(index);

      index = m_lbSortAttr.AddString(pElement->GetDesc());
      m_lbSortAttr.SetItemDataPtr(index, pElement);
   }
}

///////////////////////////////////////////////////////////////////////////////

void CPageQuerySort::OnDelete() 
{
   int index = m_lbSortAttr.GetCurSel();
   if (index != LB_ERR)
   {
	    m_lbSortAttr.DeleteString(index);
   };
	
}

///////////////////////////////////////////////////////////////////////////////
//
// Change ranking of sorting
//

void CPageQuerySort::OnDeltaposUpdown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

   CString s;
   int i = m_lbSortAttr.GetCurSel();
   int iSwap = -1;

   // Determine which items to swap

   if (pNMUpDown->iDelta == -1 && i > 0)
   {
      iSwap = i-1;
      
   }    
   else if (pNMUpDown->iDelta == 1 && i != LB_ERR && i+1 < m_lbSortAttr.GetCount())
   {
      iSwap = i+1;
   };

   // Swap the items
   if (iSwap != -1)
   {
      m_lbSortAttr.GetText(i, s);
      CQueryElement* pElement = (CQueryElement*)m_lbSortAttr.GetItemDataPtr(i);      

      m_lbSortAttr.DeleteString(i);
      m_lbSortAttr.InsertString(iSwap, s);
      m_lbSortAttr.SetItemDataPtr(iSwap, pElement);

      m_lbSortAttr.SetCurSel(iSwap);
   };
	
	
	*pResult = 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// Rank the sortby attribute according to the order in the list
//

BOOL CPageQuerySort::OnKillActive() 
{
   // Reset sortby attribute for all query elements

   CQueryElement* pElement = m_pQuery;
   while (pElement != NULL)      
   {      
      pElement->SetSortBy(FALSE);
      pElement = pElement->GetNextQuery();
   }   

   // Rank the sort by attributes

   int i = 0; for (i = 0; i < m_lbSortAttr.GetCount(); i++)
   {
      CQueryElement* pSort = (CQueryElement*)m_lbSortAttr.GetItemDataPtr(i);

      CQueryElement* pElement = m_pQuery;
      while (pElement != NULL)      
      {
         if (pSort == pElement)
         {
            pElement->SetSortBy(i+1);
            break;
         }
         pElement = pElement->GetNextQuery();
      }   
   }
	
	return CPropertyPage::OnKillActive();
}

///////////////////////////////////////////////////////////////////////////////

void CPageQuerySort::OnOK() 
{
	*m_pQueryPrev = *m_pQuery;      	  				
	CPropertyPage::OnOK();	
	
}

