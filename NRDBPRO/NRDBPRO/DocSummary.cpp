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
#include "DocSummary.h"
#include "dlgsummaryreport.h"
#include "dlgprogress.h"
#include "bdattribute.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

long CDocSummary::m_lFeatureDefault;
long CDocSummary::m_lFTypeDefault;

/////////////////////////////////////////////////////////////////////////////
// CDocSummary

IMPLEMENT_DYNCREATE(CDocSummary, CDocBDReport)

CDocSummary::CDocSummary()
{
}

BOOL CDocSummary::OnNewDocument()
{
   CFeatureType ftype;

   if (m_lFTypeDefault == 0)
   {

      CDlgSummaryReport dlg;

      if (dlg.DoModal() != IDOK) return FALSE;

      // Retrieve the selected values

	   m_lFType = dlg.GetFType();
	   int i = 0; for (i = 0; i < dlg.GetFeatures().GetSize(); i++) 
	   {
		   m_alFeatures.Add(dlg.GetFeatures().GetAt(i));		
	   };
   } 

// Automatic summary report

   else
   {      
      BDFTypeI(BDHandle(), m_lFTypeDefault, &ftype);
      m_lFType = ftype.m_lId;
      m_alFeatures.Add(m_lFeatureDefault);
      m_lFeatureDefault = 0;
      m_lFTypeDefault = 0;
   }

	if (!CDocReport::OnNewDocument())
		return FALSE;

   BDSetPreviousReport(CNRDB::summaryreport);

	return TRUE;
}

CDocSummary::~CDocSummary()
{
}


BEGIN_MESSAGE_MAP(CDocSummary, CDocBDReport)
	//{{AFX_MSG_MAP(CDocSummary)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocSummary diagnostics

#ifdef _DEBUG
void CDocSummary::AssertValid() const
{
	CDocBDReport::AssertValid();
}

void CDocSummary::Dump(CDumpContext& dc) const
{
	CDocBDReport::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDocSummary serialization

void CDocSummary::Serialize(CArchive& ar)
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
// CDocSummary commands

BOOL CDocSummary::WriteReport()
{
   BOOL bOK = TRUE;
   CFeatureType ftype;
   CAttrArray aAttr;

   CDlgProgress dlg;
   
   // Determine feature types which inherit from the feature type selected
   
   GetSubFTypes();

   // For these feature types, determine which features are derived from 
   // them

   GetSubFeatures();

   // Write the data   

   int i = 0; for (i = 0; i < m_aSubFTypes.GetSize() && bOK; i++)
   {   
      long lFType = GetParentFType(m_aSubFTypes[i]);
   
      // Determine if the feature type belongs to the selected sector

      CDictSelArray& aSectorSel = CComboBoxFType::GetSectorSel();
      for (int k = 0; k < aSectorSel.GetSize(); k++)
      {
         if (m_aSubFTypes[i].m_lDictionary == aSectorSel[k].m_lId && 
             aSectorSel[k].m_bSel)
         {                           
            break;
         }
      }
      if (k == aSectorSel.GetSize()) lFType = 0;
      
   // Update Progress


      if (lFType != 0)
      {

         CLongArray alFeatures;         

         int j = 0; for (j = 0; j < m_aFeatures.GetSize(); j++)
         {
            if (m_aFeatures[j].m_lFeatureTypeId == lFType)
            {
               alFeatures.Add(m_aFeatures[j].m_lId);               
            }
         }                 

         if (alFeatures.GetSize() > 0)
         {

            // Determine attributes to be displayed (all)

            // Initialise list of attributes corresponding to selected ftype

            CLongArray alAttr;            
            alAttr.Add(BDFEATURE);
            alAttr.Add(BDDATE);
         
	         BDFTypeAttrInit(BDHandle(), m_aSubFTypes[i].m_lId, &aAttr);
            BDEnd(BDHandle());

	         for (j = 0; j < aAttr.GetSize(); j++)
	         {		   
               if (aAttr[j]->GetDataType() != BDMAPLINES)
               {
                  alAttr.Add(aAttr[j]->GetAttrId());		     
               };
	         }
      
		      // Create the query

            if (m_pQuery != NULL) delete m_pQuery;
            
            m_pQuery = new CQuery(m_aSubFTypes[i].m_lId, alAttr, alFeatures,  BDFEATURE);

            // Include in report even if no data otherwise report will not display
            // if only these are selected
   
            m_pQuery->SetShowAllFeatures(TRUE);		    
            
		    // Retrieve the corresponding data
           
  		      if (m_pQueryResult != NULL) delete m_pQueryResult;
		      m_pQueryResult = new CQueryResult;

		    if (m_pQueryResult->Initialise(m_pQuery))
		    {
             // Write report

             if (m_pQueryResult->GetSize() > 0)
             {
                NewPara();
                Bold();
                Write(m_aSubFTypes[i].m_sDesc);
                NewPara();   
                NewPara();   
                SetFont(FONT_SMALL);

			       WriteReportQuery(FALSE);			 
             };
		    }
         };            
      };                    

   }  


   m_aSubFTypes.RemoveAll();
   m_aFeatures.RemoveAll();

   m_sStatus = "";

   return bOK;

}


/////////////////////////////////////////////////////////////////////////////
//
// Determines all feature types inheriting from the provided feature type
//

void CDocSummary::GetSubFTypes()
{
   long lFTypeP = 0;

   // First load all feature types from memory

   CFeatureType ftype;

   BOOL bFound = BDFeatureType(BDHandle(), &ftype, BDGETINIT);
   while (bFound)
   {
      m_aFTypes.Add(ftype);
      bFound = BDGetNext(BDHandle());
   }
   BDEnd(BDHandle());

   // Add lFType

   int i = 0; for (i = 0; i < m_aFTypes.GetSize(); i++)
   {
	   if (IsSubFType(m_aFTypes[i]))
	   {
 // Only include features for the currently seleted sector

          m_aSubFTypes.Add(m_aFTypes[i]); 
	   }
   }
}

/////////////////////////////////////////////////////////////////////////////
//
// Determines is the given ftype  is a subftype of the default m_lFType
// using the list of feature type m_aFTypes
//

BOOL CDocSummary::IsSubFType(CFeatureType& ftype)
{
   // Determine if found or not

   if (ftype.m_lId == m_lFType) return TRUE;   
   else if (ftype.m_lId == 0) return FALSE;

   if (ftype.m_lParentFType == 0) return FALSE;
   
   // Search parent

   int i = 0; for (i = 0; i < m_aFTypes.GetSize(); i++)
   {
	   if (m_aFTypes[i].m_lId == ftype.m_lParentFType)
	   {
		   return IsSubFType(m_aFTypes[i]);
	   }
   }
   
   ASSERT(FALSE);
   return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Determines the ftype from which the ftype inherits its features!
//

long CDocSummary::GetParentFType(CFeatureType& ftype)
{
   // Return if parent not one to one

   if (ftype.m_lParentFType == 0 || ftype.m_bManyToOne) return ftype.m_lId;

   // Find parent

   int i = 0; for (i = 0; i < m_aFTypes.GetSize(); i++)
   {
      if (m_aFTypes[i].m_lId == ftype.m_lParentFType)
         break;
   }
   ASSERT(i != m_aFTypes.GetSize());

   // Check if parent inherits from m_lFType

   return GetParentFType(m_aFTypes[i]);
}

/////////////////////////////////////////////////////////////////////////////
//
// Determines all feature inheriting from the provided features
//

void CDocSummary::GetSubFeatures()
{
   long lFeatureP = 0;
   long lFTypeP = 0;
   CFeature feature;   
   BOOL bFound;

  // Produce a list of 'parent feature types'

   int i = 0; for (i = 0; i < m_aSubFTypes.GetSize(); i++)
   {
     long lFType = GetParentFType(m_aSubFTypes[i]);
     int j = 0; for (j = 0; j < m_alFTypes.GetSize(); j++)
     {
        if (m_alFTypes[j] == lFType) break;
     }	 
	 if (j == m_alFTypes.GetSize()) m_alFTypes.Add(lFType);     	 
   }

   // Retrieve ALL features for subtypes of the feature types

   for (i = 0; i < m_alFTypes.GetSize(); i++)
   {
      feature.m_lFeatureTypeId = m_alFTypes[i];

      BOOL bFound = BDFeature(BDHandle(), &feature, BDGETINIT);
      while (bFound)
      {
          m_aFeatures.Add(feature);                    
         
         bFound = BDGetNext(BDHandle());
      }
      BDEnd(BDHandle());
   }

   // For each feature, determine if it is a sub-feature of the selected list

   for (i = 0; i < m_aFeatures.GetSize(); i++)
   {
      bFound = TRUE;

      // Find parent

      feature = GetParentFeature(m_aFeatures[i]);
      if (feature.m_lFeatureTypeId == m_lFType) 
      {
      
         // Search in selected list

         int j = 0; for (j = 0; j < m_alFeatures.GetSize(); j++)
         {
            if (feature.m_lId == m_alFeatures[j]) break;
         }

         // If not found then delete

         if (j == m_alFeatures.GetSize()) bFound = FALSE;
      } else
      {
         bFound = FALSE;
      }

      // Remove features not sub-features

      if (!bFound)
      {
         m_aFeatures.RemoveAt(i);
         i--;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
//
// Given the index of a feature in an array returns the index of the parent
// feature.
//

CFeature CDocSummary::GetParentFeature(CFeature& feature)
{
   CFeatureType ftype;

   if (feature.m_lFeatureTypeId != m_lFType) 
   {

      // Find the feature type

      int i = 0; for (i = 0; i < m_aSubFTypes.GetSize(); i++)
      {
         if (m_aSubFTypes[i].m_lId == feature.m_lFeatureTypeId) break;
      }
      ASSERT(i != m_aSubFTypes.GetSize());
      long lFTypeP = m_aSubFTypes[i].m_lParentFType;

      // Determine the parent feature
      
      for (i = 0; i < m_aSubFTypes.GetSize(); i++)
      {
         if (m_aSubFTypes[i].m_lId == lFTypeP) break;
      }
      ASSERT(i != m_aSubFTypes.GetSize());
      
      lFTypeP = GetParentFType(m_aSubFTypes[i]);

      // Search for the matching parent

      if (lFTypeP != 0)
      {
         for (i = 0; i < m_aFeatures.GetSize(); i++)
         {
            if (m_aFeatures[i].m_lId == feature.m_lParentFeature &&
                m_aFeatures[i].m_lFeatureTypeId == lFTypeP)
            {
               return GetParentFeature(m_aFeatures[i]);
            }
         }
      }
   }
   return feature;
}
