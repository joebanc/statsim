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
#include "DocTSGraph.h"
#include "dlgreport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocTSGraph

IMPLEMENT_DYNCREATE(CDocTSGraph, CDocGraph)

CDocTSGraph::CDocTSGraph()
{
	m_nType = CDlgReport::Timeseries;
}

BOOL CDocTSGraph::OnNewDocument()
{
	if (!CDocGraph::OnNewDocument())
		return FALSE;

   if (m_bQuery)
   {
      BDSetPreviousReport(CNRDB::timeseriesquery);
   } else
   {
      BDSetPreviousReport(CNRDB::timeseries);
   }

	return TRUE;
}

CDocTSGraph::~CDocTSGraph()
{
}


BEGIN_MESSAGE_MAP(CDocTSGraph, CDocGraph)
	//{{AFX_MSG_MAP(CDocTSGraph)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocTSGraph diagnostics

#ifdef _DEBUG
void CDocTSGraph::AssertValid() const
{
	CDocGraph::AssertValid();
}

void CDocTSGraph::Dump(CDumpContext& dc) const
{
	CDocGraph::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDocTSGraph serialization

void CDocTSGraph::Serialize(CArchive& ar)
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
//
// For time-series graphs, every feature has its own line
//

int CDocTSGraph::GetNumLines()
{
   int iLines = 0;
   m_nAttr = 0;
   
   // Query

   if (m_pQueryResult != NULL) 
   {      
      int i = 0; for (i = 0; i < m_pQueryResult->GetAttrArray().GetSize(); i++)
      {         
         if (m_pQueryResult->GetAttrArray().GetAt(i).m_lDataType == BDNUMBER)
         {
            m_nAttr++;
         };
      };         
      iLines = m_nAttr * m_alFeatures.GetSize();
   }
   return iLines;
}


/////////////////////////////////////////////////////////////////////////////
// CDocTSGraph commands

BOOL CDocTSGraph::GetFirstData(int iLine, CDateTime& datetime, double& dValue)
{  
   m_iFeature = 0;
   m_iAttr = 0;

   int iNumber = -1;

   if (m_pQueryResult->GetSize() == 0) return FALSE;

   // Copy the attribute selections excluding numerical attributes
  
   CQueryAttrArray* pAttrArray = m_pQueryResult->GetAt(m_iFeature);   
   int i = 0; for (i = 0; i < pAttrArray->GetSize(); i++)
   {      
      if (pAttrArray->GetAt(i)->GetDataType() == BDNUMBER)
      {
         iNumber++;
      }

      if (iLine % m_nAttr == iNumber)
      {         
         m_iAttr = i;
         break;
      }
   };

   // Determine the feature

   m_lFeature = m_alFeatures[iLine/m_nAttr];

   return GetNextData(iLine, datetime, dValue);
}

/////////////////////////////////////////////////////////////////////////////

BOOL CDocTSGraph::GetNextData(int iLine, CDateTime& datetime, double& dValue)
{
   BOOL bOK = TRUE;

   // Find the next feature for this line

   while (m_iFeature < m_pQueryResult->GetSize() && 
          m_pQueryResult->GetAt(m_iFeature)->m_lFeature != m_lFeature)
   {
      m_iFeature++;
   }   
         
   if (m_iFeature < m_pQueryResult->GetSize())   
   {     
      
      // Retrieve data value

      CQueryAttrArray* pAttrArray = m_pQueryResult->GetAt(m_iFeature);
      int i = 0; for (i = 0; i < pAttrArray->GetSize(); i++)
      {           
         CAttribute* pAttr = pAttrArray->GetAt(i);
         if (i == m_iAttr)
         {
            if (pAttr->GetDataType() == BDNUMBER)
            {
               dValue = *pAttr->GetDouble();
            }            
         };
         if (pAttr->GetDataType() == BDDATE)
         {
            datetime = CDateTime(*pAttr->GetDate(),0);
         }
      };

     
      m_iFeature++;

   } else
   {
      bOK = FALSE;
   }
   return bOK;
};   
 
/////////////////////////////////////////////////////////////////////////////     

CString CDocTSGraph::GetLegend(int iLine)
{
   // Query

   if (m_pQueryResult != NULL) 
   {                 
      if (m_pQueryResult->GetSize() > 0)
      {
         CQueryAttrArray* pAttrArray = m_pQueryResult->GetAt(0);

         int i = 0; for (i = 0; i < pAttrArray->GetSize(); i++)
         {         
            CAttribute* pAttr = pAttrArray->GetAt(m_iAttr);
            return m_asFeatures[iLine/m_nAttr] + " " + pAttr->GetDesc();      
           
         };      
      } 
   }

   return "";      
}

