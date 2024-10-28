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
#include "DocGraph.h"
#include "dlgreportgraph.h"
#include "sheetquery.h"
#include "dlgprogress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocGraph

IMPLEMENT_DYNCREATE(CDocGraph, CDocument)

CDocGraph::CDocGraph()
{
   m_pQueryResult = NULL;
   m_pQuery = NULL;
   m_nType = CDlgReport::Histogram;
}

CDocGraph::~CDocGraph()
{
   if (m_pQueryResult != NULL)
   {
      delete m_pQueryResult;
   };
   if (m_pQuery != NULL) delete m_pQuery;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CDocGraph::OnNewDocument()
{
   int nQueryType = 0;  
   m_bQuery = FALSE;

   // Report Wizard

      if (m_nType == CDlgReport::Histogram) nQueryType = CSheetQuery::HistGraph;
      else if (m_nType == CDlgReport::Timeseries) nQueryType = CSheetQuery::TSGraph;
	  else if (m_nType == CDlgReport::PieChart) nQueryType = CSheetQuery::PieChart;

      if (BDGetPreviousReport() != CNRDB::histogramquery &&
          BDGetPreviousReport() != CNRDB::timeseriesquery &&
          BDGetPreviousReport() != CNRDB::piechartquery)
      {
	      // Initialise report dialog

	      CDlgReportGraph dlg(m_nType);         

         int nRet = dlg.DoModal();
         if (nRet == IDOK)
         {	
	         // Retrieve the selected values

	         m_lFType = dlg.GetFType();
	         int i = 0; for (i = 0; i < dlg.GetFeatures().GetSize(); i++) 
	         {
		         m_alFeatures.Add(dlg.GetFeatures().GetAt(i));
		         m_asFeatures.Add(dlg.GetFNames().GetAt(i));
	         };

             for (i = 0; i < dlg.GetAttr().GetSize(); i++) 
             {
		         m_alAttr.Add(dlg.GetAttr().GetAt(i));
             };      

             int nSort = BDFEATURE;
             if (m_nType == CDlgReport::Timeseries) nSort = BDDATE;
		       m_pQuery = new CQuery(m_lFType, m_alAttr, m_alFeatures,  nSort);
		    
           BDSetPreviousReport(CNRDB::histogram);

         } else if (nRet == IDC_QUERY)
         {
            m_bQuery = TRUE;
         }
         else
         {
            return FALSE;
         }
      } else
      {
         m_bQuery = TRUE;
      }

      // Query

      if (m_bQuery)
      {
         m_pQuery = new CQuery;
         if (m_pQuery != NULL)
         {
            CSheetQuery dlg(m_pQuery, nQueryType, BDString(IDS_GRAPH)); 
            int nRet = dlg.DoModal();
            if (nRet != IDOK)
            {
               delete m_pQuery;
               m_pQuery = NULL;
            } else
            {
                 BDSetPreviousReport(CNRDB::histogramquery);

            }
         };
      }

   // Query or report wizard

   if (m_pQuery != NULL)
   {      
      CDlgProgress dlg;

      // Retrieve data

      BeginWaitCursor();
      m_pQueryResult = new CQueryResult;
      if (!m_pQueryResult->Initialise(m_pQuery))
      {
         if (AfxMessageBox(BDString(IDS_QUERYERROR), MB_YESNO) == IDYES)
         {
            return OnNewDocument();
         }
         return FALSE;
      }
      EndWaitCursor();      

      m_lFType = m_pQuery->GetFType();

     // Determine the features selected

      int i = 0; for (i = 0; i < m_pQueryResult->GetSize(); i++)
      {
         int j = 0; for (j = 0; j < m_alFeatures.GetSize(); j++)
         {
            if (m_pQueryResult->GetAt(i)->m_lFeature == (long)m_alFeatures[j]) break;
         }
         if (j == m_alFeatures.GetSize()) 
         {
            m_alFeatures.Add(m_pQueryResult->GetAt(i)->m_lFeature);
            m_asFeatures.Add(m_pQueryResult->GetAt(i)->GetFName());
         };
      }

      m_sTitle = m_pQuery->GetQueryName();

      // Exit if no data

   } else
   {
      return FALSE;
   }

   // Set title

   SetTitle(BDString(IDS_GRAPH) + " - " + m_sTitle);

  // Default creation

   if (!CDocument::OnNewDocument())
		return FALSE;
    
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDocGraph, CDocument)
	//{{AFX_MSG_MAP(CDocGraph)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocGraph diagnostics

#ifdef _DEBUG
void CDocGraph::AssertValid() const
{
	CDocument::AssertValid();
}

void CDocGraph::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDocGraph serialization

void CDocGraph::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////

int CDocGraph::GetNumLines()
{
   int iLines = 0;

   // Query

   if (m_pQueryResult != NULL) 
   {      
      int i = 0; for (i = 0; i < m_pQueryResult->GetAttrArray().GetSize(); i++)
      {         
         if (m_pQueryResult->GetAttrArray().GetAt(i).m_lDataType == BDNUMBER)
         {
            iLines++;
         };
      };       
   }
   
   return iLines;
}

/////////////////////////////////////////////////////////////////////////////
// CDocGraph commands

BOOL CDocGraph::GetFirstData(int iLine, double& rValue, CString& sLabel)
{   
   // Queries

   if (m_pQueryResult != NULL) 
   {
      return GetFirstDataQuery(iLine, rValue, sLabel);
   };
   
   return FALSE;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CDocGraph::GetNextData(int iLine, double& rdValue, CString& sLabel)
{  
 
   // Queries

   if (m_pQueryResult != NULL) 
   {
      return GetNextDataQuery(iLine, rdValue, sLabel);
   };
   return FALSE;
};   
 
/////////////////////////////////////////////////////////////////////////////     

CString CDocGraph::GetLegend(int iLine)
{
   // Query

   if (m_pQueryResult != NULL) 
   {                 
      if (m_pQueryResult->GetSize() > 0)
      {
         CQueryAttrArray* pAttrArray = m_pQueryResult->GetAt(0);

         int iNumber = 0;
         int i = 0; for (i = 0; i < pAttrArray->GetSize(); i++)
         {  
            CAttribute* pAttr = pAttrArray->GetAt(i);

            if (pAttr->GetDataType() == BDNUMBER)
            {
               iNumber++;
            }
            
            if (iNumber == iLine+1)
            {
               return pAttr->GetDesc();      
            };
         };      
      } else
      {
         //ASSERT(FALSE);
      }
   }

   return "";
}

/////////////////////////////////////////////////////////////////////////////     

BOOL CDocGraph::GetFirstDataQuery(int iLine, double& dValue, CString& sLabel)
{
   m_iFeature = 0;

   int iNumbers = 0;

   // Copy the attribute selections excluding numerical attributes

   m_aAttrSel.RemoveAll();
   int i = 0; for (i = 0; i < m_pQueryResult->GetAttrArray().GetSize(); i++)
   {
      if (m_pQueryResult->GetAttrArray().GetAt(i).m_lDataType != BDNUMBER)
      {
         m_aAttrSel.Add(m_pQueryResult->GetAttrArray().GetAt(i));
      } else
      {
         iNumbers++;

         if (iNumbers == iLine+1) 
         {         
            m_lAttrId = m_pQueryResult->GetAttrArray().GetAt(i).m_lAttr;
         }
      }
   };
   
   return GetNextDataQuery(iLine, dValue, sLabel);
}

/////////////////////////////////////////////////////////////////////////////     

BOOL CDocGraph::GetNextDataQuery(int iLine, double& dValue, CString& sLabel)
{
   BOOL bOK = TRUE;

   dValue = AFX_RFX_DOUBLE_PSEUDO_NULL;

   if (m_iFeature < m_pQueryResult->GetSize())
   {
      // Retrieve data value

      CQueryAttrArray* pAttrArray = m_pQueryResult->GetAt(m_iFeature);
      int i = 0; for (i = 0; i < pAttrArray->GetSize(); i++)
      {           
         CAttribute* pAttr = pAttrArray->GetAt(i);
         if (pAttr->GetAttrId() == m_lAttrId)
         {
            if (pAttr->GetDataType() == BDNUMBER)
            {
               dValue = *pAttr->GetDouble();
            }            
         };
         if (pAttr->GetDataType() == BDDATE)
         {
            m_datetime = CDateTime(*pAttr->GetDate(),0);
         }
      };

      // Retrieve label (without numbers)
     
      sLabel = m_aAttrSel.GetAttrDesc(*pAttrArray);

      m_iFeature++;

   } else
   {
      bOK = FALSE;
   }
   return bOK;
}



///////////////////////////////////////////////////////////////////////////////

void CDocGraph::OnUpdateFilePrintPreview(CCmdUI* pCmdUI) 
{	
}
