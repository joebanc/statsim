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
#include "DocTSReport.h"
#include "dlgreportgraph.h"
#include "sheetquery.h"
#include "dlgprogress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocTSReport

IMPLEMENT_DYNCREATE(CDocTSReport, CDocReport)

CDocTSReport::CDocTSReport()
{
   m_pQuery = NULL;
   m_pQueryResult = NULL;
   m_pData = NULL;
}

CDocTSReport::~CDocTSReport()
{
   if (m_pQuery != NULL) delete m_pQuery;
   if (m_pQueryResult != NULL) delete m_pQueryResult;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CDocTSReport::OnNewDocument()
{	  
   if (BDGetPreviousReport() != CNRDB::tsreportquery)
   {
	   // Initialise report dialog

      CDlgReportGraph dlg(CDlgReport::Timeseries|CDlgReport::Report);

      int nRet = dlg.DoModal();
      if (nRet == IDOK)
      {	
		  m_pQuery = new CQuery(dlg.GetFType(), dlg.GetAttr(), dlg.GetFeatures());		  

        BDSetPreviousReport(CNRDB::tsreport);
 
      } else if (nRet == IDCANCEL)
      {
         return FALSE;
      }
   }

  // Create a query if none available

   if (m_pQuery == NULL)
   {
      m_pQuery = new CQuery;
      if (m_pQuery != NULL)
      {
         CSheetQuery dlg(m_pQuery, CSheetQuery::TSGraph, BDString(IDS_TIMESERIESREPORT)); 
         int nRet = dlg.DoModal();
         if (nRet != IDOK)
         {
            delete m_pQuery;
            m_pQuery = NULL;
         } else
         {
            BDSetPreviousReport(CNRDB::tsreportquery);
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
         return FALSE;
      }
      EndWaitCursor();      

   } else
   {
      return FALSE;
   }

   // Create the report

   if (!CDocReport::OnNewDocument())
		return FALSE;

   return TRUE;
}


///////////////////////////////////////////////////////////////////////////////


BEGIN_MESSAGE_MAP(CDocTSReport, CDocReport)
	//{{AFX_MSG_MAP(CDocTSReport)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocTSReport diagnostics

#ifdef _DEBUG
void CDocTSReport::AssertValid() const
{
	CDocReport::AssertValid();
}

void CDocTSReport::Dump(CDumpContext& dc) const
{
	CDocReport::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDocTSReport serialization

void CDocTSReport::Serialize(CArchive& ar)
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
// CDocTSReport commands

BOOL CDocTSReport::WriteReport()
{   
   CString s;

   if (m_pQueryResult->GetSize() == 0) return FALSE;

   // Search for the numeric attributes in the result

   int i = 0; for (i = 0; i < m_pQueryResult->GetAt(0)->GetSize(); i++)
   {
      if (m_pQueryResult->GetAt(0)->GetAt(i)->GetDataType() == BDNUMBER && 
          m_pQueryResult->GetAt(0)->GetAt(i)->GetFTypeId() == m_pQuery->GetFType())
      {
         RetrieveData(m_pQueryResult->GetAt(0)->GetAt(i)->GetAttrId());

         if (m_pData != NULL)
         {
            WriteData(m_pQueryResult->GetAt(0)->GetAt(i)->m_sDesc);
            delete [] m_pData;
         };         
      };

   }
           
   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

void CDocTSReport::WriteData(CString sAttr)
{
   CString s;
   
   NewLine();

   Bold();
       
  // Write the title

   Write(sAttr);

   NewLine();
   
   // Output the dates

   BeginTable(m_aDates.GetSize()+1);
   BeginTableRow();

   // Output the column headings
     
   AddTableCell(BDString(IDS_DATE),"colheader");

   int i = 0; for (i = 0; i < m_aDates.GetSize(); i++)
   {
       m_aDates[i].DateAsString(s);
       AddTableCell(s,"colheader");
   }
   EndTableRow();

   SetFont(FONT_SMALL);

   // Output the data

   for (i = 0; i < m_alFeatures.GetSize(); i++)
   {
      BeginTableRow();
      CString s;

      // Find the name of the features

      int j = 0; for (j = 0; j < m_pQueryResult->GetSize(); j++)
      {
         if (m_pQueryResult->GetAt(j)->m_lFeature == m_alFeatures[i])
         {
            s = m_pQueryResult->GetAt(j)->GetFName();
         }
      }
      AddTableCell(s, "data");

      // Output the data

      for (j = 0; j < m_aDates.GetSize(); j++)
      {
         double dValue = m_pData[i * m_aDates.GetSize() + j];
         if (!IsNullDouble(dValue))
         {
            std::strstream str;
            str.precision(10);
            str << dValue << std::ends;
            AddTableCell(str.str(), "data");
            str.rdbuf()->freeze(0);
         } else
         {
            AddTableCell("", "data");
         }
      }
      EndTableRow();
   }

   EndTable();



   
}

///////////////////////////////////////////////////////////////////////////////

void CDocTSReport::RetrieveData(long lAttrId)
{
   // Scan the data and produce a list of dates   

   int i = 0; for (i = 0; i < m_pQueryResult->GetSize(); i++)
   {
      BOOL bFound = FALSE;
      int j = 0; for (j = 0; j < m_aDates.GetSize(); j++)
      {
         if (CDateTime(m_pQueryResult->GetAt(i)->m_lDate,0) == m_aDates[j]) bFound = TRUE;
         if (CDateTime(m_pQueryResult->GetAt(i)->m_lDate,0) > m_aDates[j])
         {            
            break;
         }
      }
      if (!bFound) m_aDates.InsertAt(j,  CDateTime(m_pQueryResult->GetAt(i)->m_lDate,0)); 
   }

   // Count the number of features returned
   
   for (i = 0; i < m_pQueryResult->GetSize(); i++)
   {
      BOOL bFound = FALSE;
      int j = 0; for (j = 0; j < m_alFeatures.GetSize(); j++)
      {
         if (m_pQueryResult->GetAt(i)->m_lFeature == m_alFeatures[j]) break;
      }
      if (j == m_alFeatures.GetSize()) m_alFeatures.Add(m_pQueryResult->GetAt(i)->m_lFeature);
   }

  // Create an array to store the dates and values
   
   int nSize = m_aDates.GetSize() * m_alFeatures.GetSize();
   m_pData = new double[nSize];
   if (m_pData != NULL)
   {
      // Initialise the memory

      int i = 0; for (i = 0; i < nSize; i++) m_pData[i] = AFX_RFX_DOUBLE_PSEUDO_NULL;

      // Search through the results

      for (i = 0; i < m_pQueryResult->GetSize(); i++)
      {
         // Search where to place the value
         int j = 0; for (j = 0; CDateTime(m_pQueryResult->GetAt(i)->m_lDate,0) != m_aDates[j]; j++);
         for (int k = 0; m_pQueryResult->GetAt(i)->m_lFeature != m_alFeatures[k]; k++);           

         // Set the value

         for (int l = 0; l < m_pQueryResult->GetAt(k)->GetSize(); l++)
         {
            if (m_pQueryResult->GetAt(k)->GetAt(l)->GetAttrId() == lAttrId && 
               m_pQueryResult->GetAt(k)->GetAt(l)->GetFTypeId() == m_pQuery->GetFType())
            {
               m_pData[k * m_aDates.GetSize() + j ] = *m_pQueryResult->GetAt(i)->GetAt(l)->GetDouble();

               ASSERT(k * m_aDates.GetSize() + j  < nSize);
               break;
            };
         }
      
      }
   };
}
