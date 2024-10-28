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
#include "PageQuery.h"
#include "SheetQuery.h"
#include "dlgselectfeatures.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageQuery property page

IMPLEMENT_DYNCREATE(CPageQuery, CPropertyPage)

CPageQuery::CPageQuery(CQuery* pQuery, CQuery* pQueryPrev, int nType) : CPropertyPage(CPageQuery::IDD)
{
	m_pQuery = pQuery;
   m_pQueryPrev = pQueryPrev;
   m_nType = nType;
   m_bUpdateList = FALSE;
   m_pTooltip = NULL;
   m_nNumber = 0;

	//{{AFX_DATA_INIT(CPageQuery)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPageQuery::CPageQuery()
{
   m_pTooltip = NULL;
}

CPageQuery::~CPageQuery()
{
   if (m_pTooltip != NULL) delete m_pTooltip;
}

void CPageQuery::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageQuery)	
	DDX_Control(pDX, IDC_FTYPE, m_cbFType);
	DDX_Control(pDX, IDC_ELEMENTS, m_ctrlTree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageQuery, CPropertyPage)
	//{{AFX_MSG_MAP(CPageQuery)
	ON_CBN_SELCHANGE(IDC_FTYPE, OnSelchangeFtype)
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)	
	ON_NOTIFY(TVN_SELCHANGED, IDC_ELEMENTS, OnSelchangedElements)
	ON_BN_CLICKED(IDC_DICTIONARY, OnDictionary)
	ON_BN_CLICKED(IDC_SELECT, OnSelect)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageQuery message handlers

BOOL CPageQuery::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

   // Set font for tree control

   LOGFONT lf;
   memset(&lf,0,sizeof(LOGFONT));
   lf.lfHeight = -11;
   lf.lfPitchAndFamily = 12;   
   lf.lfCharSet = NRDB_CHARSET;
   strcpy(lf.lfFaceName, BDString(IDS_DEFAULTFONT));         
   m_font.CreateFontIndirect(&lf); 
   m_ctrlTree.SetFont(&m_font);
   
	long lFType = 0;

   // Initialise the tree control
        
   m_imagelist.Create(IDB_TREECTRL, 16, 0, RGB(255,255,255));
   m_ctrlTree.SetImageList(&m_imagelist, TVSIL_NORMAL);

   // Initialise feature types

   m_cbFType.Init(BDFTypeSel());

   // Retrieved saved feature type

   m_cbFType.GetItemDataX(lFType);   
   
   if (m_pQueryPrev->GetFType() != 0 && 
       m_pQueryPrev->GetFType() == lFType)
   {
      UpdateList(m_pQueryPrev);            
      *m_pQuery = *m_pQueryPrev;      
   } else
   {
      OnSelchangeFtype(); 
   };

   // Enable tooltips

   BDHwndDialog() = m_hWnd;
   
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////

void CPageQuery::OnSelchangeFtype() 
{
   BeginWaitCursor();
   long lFType;

   m_bUpdateList = TRUE;

   m_ctrlTree.DeleteAllItems();

   // Retrieve the selected feature
     
   if (m_cbFType.GetItemDataX(lFType) != CB_ERR)
   {
      // Initialise the query
            
      m_pQuery->Initialise(lFType);

      // Display the results in the tree control

      UpdateList(m_pQuery);            
   }	

   m_bUpdateList = FALSE;

   EndWaitCursor();
}

/////////////////////////////////////////////////////////////////////////////

void CPageQuery::UpdateList(CQuery* pQuery, HTREEITEM hParent)
{
   CQueryElement* pElement = pQuery;

   m_bUpdateList = TRUE;

   if (hParent == TVI_ROOT) m_ctrlTree.DeleteAllItems();
   
   HTREEITEM hItem = m_ctrlTree.InsertItem(pElement->GetDesc(), hParent);   
   m_ctrlTree.SetCheck(hItem, pElement->GetSelected());
   HTREEITEM hLast = TVI_ROOT;   
   
   while (pElement != NULL)
   {
      pElement = pElement->GetNextQuery();
      if (pElement != NULL)
      {
         // If a feature type then move one level down and exit

         if (pElement->GetDataType() == BDFTYPE)
         {
            UpdateList((CQuery*)pElement, hItem);
            break;
         }          

         // Otherise add at this level

         else
         {
            if (pElement->GetDataType() != BDQUERYSTATS)
            {
               hLast = m_ctrlTree.InsertItem(pElement->GetDesc(), hItem);            
               m_ctrlTree.SetCheck(hLast, pElement->GetSelected() == TRUE);
            };
         };

         // If a link type then add links to tree

         if (pElement->GetDataType() == BDLINK || 
             pElement->GetDataType() == BDFEATURE ||
             pElement->GetDataType() == BDBOOLEAN)
         {            
            UpdateLink((CQueryLink*)pElement, hLast);
         }         
         
         // If a join then added as child

         if (pElement->GetQueryJoin() != NULL)
         {
            ASSERT(pElement->GetQueryJoin()->GetConditions().GetSize() > 0);

            if (!(pElement->GetQueryJoin()->GetConditions().GetAt(0).GetCond() & CQueryJoin::exclusive))
            {
               UpdateList((CQuery*)(pElement->GetQueryJoin()->GetNextQuery()), hLast);           
            };
         }
      };            
   };

   if (hParent == TVI_ROOT) m_ctrlTree.Expand(hItem, TVE_EXPAND);      

   m_bUpdateList = FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Update features for the feature type
//

void CPageQuery::UpdateLink(CQueryLink* pLink, HTREEITEM hParent)
{
   HTREEITEM hItem;

   int i = 0; for (i = 0; i < pLink->m_aFeatures.GetSize(); i++)
   {
      CQueryFeature* pFeature = &pLink->m_aFeatures.GetAt(i);
      hItem = m_ctrlTree.InsertItem(pFeature->m_sName, hParent);
      m_ctrlTree.SetCheck(hItem, pFeature->GetSelected());
   }
}

///////////////////////////////////////////////////////////////////////////////

void CPageQuery::OnSelectall() 
{
   // Determine which item has been selected

   HTREEITEM hItem = m_ctrlTree.GetSelectedItem();   
   if (hItem != NULL)
   {
      DWORD dw = m_ctrlTree.GetItemData(hItem);
      BOOL bCheck = !m_ctrlTree.GetCheck(hItem);
      m_ctrlTree.SetCheck(hItem, bCheck);

      // If a link then select all subitems

      HTREEITEM hChild = m_ctrlTree.GetChildItem(hItem);
      
      while (hChild != NULL)
      {
         m_ctrlTree.SetCheck(hChild, bCheck);
         hChild = m_ctrlTree.GetNextSiblingItem(hChild);
      };
   };	   	
}

///////////////////////////////////////////////////////////////////////////////
//
// Allows features to be selected by parent
//

void CPageQuery::OnSelect() 
{
   long lFType = 0; 

   // Determine the parent feature type of that selected

   int index = m_cbFType.GetCurSel();
   if (index != CB_ERR)
   {      
      lFType = m_cbFType.GetItemData(index);
     
   // Create a dialog allowing a sub-set of the features to be selected
   
      CDlgSelectFeatures dlg(lFType);      
      if (dlg.DoModal() == IDOK)
      {
         // Retrieve the data

         RetrieveSelections(m_pQuery);

         // Update the selected features

         CQueryElement* pElement = m_pQuery;
         while (pElement != NULL)
         {
            if (pElement->GetDataType() == BDFEATURE)
            {
               CQueryLink* pLink = (CQueryLink*)pElement;
               int i = 0; for (i = 0; i < pLink->m_aFeatures.GetSize(); i++)
               {
                  pLink->m_aFeatures[i].SetSelected(FALSE);
                  int j = 0; for (j = 0; j < dlg.GetFeatures().GetSize(); j++)
                  {
                     if ((long)dlg.GetFeatures().GetAt(j) == pLink->m_aFeatures[i].m_lId)
                     {
                        pLink->m_aFeatures[i].SetSelected(TRUE);
                        break;
                     }
                  }
               }

            }
            pElement = pElement->GetNextQuery();
         }

         // Restore the selections

         m_ctrlTree.DeleteAllItems();
         UpdateList(m_pQuery);            
      }
   };		
}


///////////////////////////////////////////////////////////////////////////////

void CPageQuery::RetrieveSelections(CQuery* pQuery, HTREEITEM hItem)
{
   CQueryElement* pElement = pQuery;
   HTREEITEM hLast = NULL;
   CString s;
   
   if (hItem == TVI_ROOT) hItem = m_ctrlTree.GetChildItem(hItem);      

   if (hItem != NULL)
   {
      BOOL bCheck = m_ctrlTree.GetCheck(hItem);
      pElement->SetSelected(bCheck);
      s = m_ctrlTree.GetItemText(hItem);
         
      pElement = pElement->GetNextQuery();
      while (pElement != NULL)
      {                     
         // Retrieve selections at this level
      
         if (hLast == NULL) 
         {
            hItem = m_ctrlTree.GetChildItem(hItem);
         } else
         {
            hItem = m_ctrlTree.GetNextSiblingItem(hItem);
         };            
         ASSERT(hItem != NULL);

         s = m_ctrlTree.GetItemText(hItem);
         bCheck = m_ctrlTree.GetCheck(hItem);
         pElement->SetSelected(bCheck);      

      // If a feature type then move one level down and exit

         if (pElement->GetDataType() == BDFTYPE)
         {
            RetrieveSelections((CQuery*)pElement, hItem);
            break;
         }          

         // If a link type then add links to tree

         if (pElement->GetDataType() == BDLINK ||
            pElement->GetDataType() == BDFEATURE ||
            pElement->GetDataType() == BDBOOLEAN)
         {         
            RetrieveLinkSel((CQueryLink*)pElement, hItem);
         }

         // Query joins

          if (pElement->GetQueryJoin() != NULL)
          {
             HTREEITEM hJoin = m_ctrlTree.GetChildItem(hItem);
             RetrieveSelections((CQuery*)(pElement->GetQueryJoin()->GetNextQuery()), hJoin);            
          }

          // Set hLast

          hLast = hItem;      

         // Get next query and skip statistics

         pElement = pElement->GetNextQuery();
         while (pElement != NULL && 
                pElement->GetDataType() == BDQUERYSTATS)
         { 
            pElement = pElement->GetNextQuery();
         }
      };
   };
}

///////////////////////////////////////////////////////////////////////////////

void CPageQuery::RetrieveLinkSel(CQueryLink* pLink, HTREEITEM hParent)
{
   HTREEITEM hItem = m_ctrlTree.GetChildItem(hParent);

   int i = 0; for (i = 0; i < pLink->m_aFeatures.GetSize(); i++)
   {
      ASSERT(hItem != NULL);
      BOOL bCheck = m_ctrlTree.GetCheck(hItem);
      pLink->m_aFeatures[i].SetSelected(bCheck);
      hItem = m_ctrlTree.GetNextSiblingItem(hItem);
   }
}

///////////////////////////////////////////////////////////////////////////////

void CPageQuery::OnSelchangedElements(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
   CQueryElement* pElement = NULL;
	   	
	*pResult = 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// Retrieves the element corresponding to the hItem
//

CQueryElement* CPageQuery::GetElement(CQuery* pQuery, HTREEITEM hItemSel, HTREEITEM hItem)
{
   CQueryElement* pElement = pQuery;
   HTREEITEM hLast = NULL;   
   
   if (hItem == TVI_ROOT) hItem = m_ctrlTree.GetChildItem(hItem);      
         
   pElement = pElement->GetNextQuery();
   while (pElement != NULL)
   {           
      // Retrieve selections at this level
      
      if (hLast == NULL) 
      {
         hItem = m_ctrlTree.GetChildItem(hItem);
      } else
      {
         hItem = m_ctrlTree.GetNextSiblingItem(hItem);
      };            
      ASSERT(hItem != NULL);      

    // Required item found

      if (hItem == hItemSel)       
      {
         return pElement;
      }

   // If a feature type then move one level down and exit

      if (pElement->GetDataType() == BDFTYPE)
      {
         return GetElement((CQuery*)pElement, hItemSel, hItem);         
      }                

      hLast = hItem;      

      pElement = pElement->GetNextQuery();

      while (pElement != NULL && 
             pElement->GetDataType() == BDQUERYSTATS)
      {
         pElement = pElement->GetNextQuery();
      }
   };

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void CPageQuery::OnDictionary() 
{
   m_cbFType.OnClickDictionary();	
}

///////////////////////////////////////////////////////////////////////////////

BOOL CPageQuery::OnSetActive() 
{
#ifdef BDQUERYWIZARD
   CWnd* pParent = GetParent();
	CPropertySheet* pSheet = DYNAMIC_DOWNCAST(CPropertySheet, pParent);
   pSheet->SetWizardButtons(PSWIZB_NEXT);   
#endif
	
	return CPropertyPage::OnSetActive();
}

///////////////////////////////////////////////////////////////////////////////

BOOL CPageQuery::OnKillActive() 
{   
   if (m_cbFType.GetCurSel() == CB_ERR) return TRUE;

// Retrieve selections
   
   RetrieveSelections(m_pQuery);
   m_cbFType.SaveFTypes();         
   
   // Validate that one map attribute is selected

   int nMapLines = 0, nCoord = 0;
   int nImages = 0;
   int nDate = 0;
   m_nNumber = 0;

   CQueryElement* pElement = m_pQuery;
   while (pElement != NULL)
   {
      if (pElement->GetSelected())
      {
         if (pElement->GetDataType() == BDMAPLINES) nMapLines++;
         if (pElement->GetDataType() == BDCOORD) nCoord++;         
         if (pElement->GetDataType() == BDIMAGE) nImages++;

         if (pElement->GetDataType() == BDNUMBER) m_nNumber++;         
      };

      if (pElement->GetDataType() == BDDATE && m_nType == CSheetQuery::TSGraph)
      {
         pElement->SetSelected(TRUE);
      }
      pElement = pElement->GetNextQuery();
   }

   if (m_nType == CSheetQuery::Map && nMapLines + nCoord + nImages != 1)
   {
      if (AfxMessageBox(BDString(IDS_SELMAPOBJ),MB_YESNO|MB_DEFBUTTON2) == IDNO) return 0;
   }   

   // Include the number of numeric attributes

   BOOL bStats = FALSE;
   pElement = m_pQuery;
   while (pElement != NULL)
   {
      if (pElement->GetDataType() == BDQUERYSTATS || pElement->GetGroupStat() != CQueryElement::none)
      {         
         bStats = TRUE;
         break;
      }      
      pElement = pElement->GetNextQuery();
   }  
   
   // Check that one numeric attribute is selected

   if ((m_nType == CSheetQuery::HistGraph || m_nType == CSheetQuery::TSGraph || m_nType == CSheetQuery::PieChart) && 
       m_nNumber == 0 && !bStats)
   {
      if (AfxMessageBox(BDString(IDS_NONUMSEL), MB_OKCANCEL) == IDOK)   
      {
          return FALSE;
      };
   }
	
	return CPropertyPage::OnKillActive();
}

///////////////////////////////////////////////////////////////////////////////

void CPageQuery::OnOK() 
{   
   // Okay, can exit

   *m_pQueryPrev = *m_pQuery;      	  	
	CPropertyPage::OnOK();
}

///////////////////////////////////////////////////////////////////////////////

void CPageQuery::OnMouseMove(UINT nFlags, CPoint point) 
{
   int nMapLines = 0, nCoord = 0;
   int nNum = 0, nText = 0, nDate = 0;

   //Set up the tooltip       
   if (!m_pTooltip)       
   {    
      int rt;
      m_pTooltip = new CToolTipCtrl;         
      rt = m_pTooltip->Create(this);
      ASSERT(rt!=0);

      BDToolTip() = m_pTooltip;
      
      rt = m_pTooltip->AddTool(&m_ctrlTree,BDString(IDS_ATTRIBUTESTIP));
      ASSERT(rt!=0);          

      m_pTooltip->Activate(TRUE);       
   }

   // Update the tooltip message according to what is selected

   if (m_pTooltip)       
   {
      RetrieveSelections(m_pQuery);      
      CQueryElement* pElement = m_pQuery;
      while (pElement != NULL)
      {
         if (pElement->GetSelected())
         {
            if (pElement->GetDataType() == BDMAPLINES) nMapLines++;
            else if (pElement->GetDataType() == BDCOORD) nCoord++;
            else if (pElement->GetDataType() == BDNUMBER) nNum++;
            else if (pElement->GetDataType() == BDDATE) nDate++;
            else nText++;
         };
         pElement = pElement->GetNextQuery();
      }               

      // Display tooltips for maps

      if (m_nType == CSheetQuery::Map)
      {
         if (nMapLines > 1 || nCoord > 1 || (nMapLines > 0 && nCoord > 0))
         {
            m_pTooltip->UpdateTipText(BDString(IDS_SELMAPOBJ),&m_ctrlTree);                  
         }
         else if (nCoord == 1 && nNum == 0 && nText == 0)
         {
            m_pTooltip->UpdateTipText(BDString(IDS_SELNUMTEXT), &m_ctrlTree);         
         } 
         else if (nMapLines == 1 && nNum == 0)
         {
            m_pTooltip->UpdateTipText(BDString(IDS_SELNUMTEXT), &m_ctrlTree);             
         }      
         else if (nMapLines == 0 && nCoord == 0)
         {
            m_pTooltip->UpdateTipText(BDString(IDS_SELMAPOBJ), &m_ctrlTree);                  
         } else
         {
            m_pTooltip->UpdateTipText("",&m_ctrlTree);                  
         }

         // Display tooltips for time-series graphs

      } else if (m_nType == CSheetQuery::TSGraph)
      {
         if (nNum == 0)
         {
            m_pTooltip->UpdateTipText(BDString(IDS_NONUMSEL), &m_ctrlTree);       
         } else
         {
            m_pTooltip->UpdateTipText("", &m_ctrlTree);       
         }
      }
      // Display tool tips for histogram graphs

      else if (m_nType == CSheetQuery::HistGraph)
      {
         if (nNum == 0)
         {
            m_pTooltip->UpdateTipText(BDString(IDS_NONUMSEL), &m_ctrlTree);       
         } else if (nText == 0)
         {
            m_pTooltip->UpdateTipText(BDString(IDS_SELTEXT), &m_ctrlTree);       
         } else
         {
            m_pTooltip->UpdateTipText("", &m_ctrlTree);       
         }
      }
   }
   

	
	CPropertyPage::OnMouseMove(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
//
// void CPageQuery::PostNcDestroy() 
//

void CPageQuery::PostNcDestroy() 
{
   BDHwndDialog() = NULL;
   BDToolTip() = NULL;
	
	CPropertyPage::PostNcDestroy();
}
