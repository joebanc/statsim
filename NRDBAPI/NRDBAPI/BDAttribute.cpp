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
#include "nrdbapi.h"
#include "featureattr.h"


///////////////////////////////////////////////////////////////////////////////

BOOL BDAttribute(BDHANDLE hConnect, CAttrArray* pArray, int iFlag)
{
   return BDAttributeCond(hConnect, pArray, 0, 0, 0,0, "", iFlag);
}

///////////////////////////////////////////////////////////////////////////////

/*BOOL BDAttributeRange(BDHANDLE hConnect, CAttrArray* pArray, 
                      long lFeatureMin, long lFeatureMax, int iFlag)
{
   return BDAttributeCond(hConnect, pArray, lFeatureMin, lFeatureMax, 0,0, "", iFlag);
}*/

///////////////////////////////////////////////////////////////////////////////

BOOL BDAttributeCond(BDHANDLE hConnect, CAttrArray* pArray, 
                     long lFeatureMin, long lFeatureMax, long lStartDate, long lEndDate, LPCSTR sFilter, int iFlag)
{
   BOOL bOK = TRUE;
   CFeatureAttrSet* pRecordSet = NULL;      
   CString sSQL;
   int nDateCond = 0; 
 
   // Get database handle
      
   CNRDBase* pDBase = GetNRDBase(hConnect); 
   if (pDBase == NULL)
   { 
      bOK = FALSE;
   };

   // Retrieve date condition

   if (iFlag == BDSELECTEARLIEST)
   {
      nDateCond = CFeatureAttrSet::earliest;
      iFlag = BDGETINIT;
   }
   else if (iFlag == BDSELECTLATEST)
   {
      nDateCond = CFeatureAttrSet::latest;
      iFlag = BDGETINIT;
   }

   // Retrieve in range

   if (iFlag == BDGETINIT && lFeatureMin != 0 && lFeatureMax != 0)
   { 
      iFlag = BDSELECT4;
   }
      
   // Initialise the array of attributes

   if (bOK && (iFlag != BDGETNEXT && iFlag != BDINSERTNEXT && iFlag != BDADD && 
       iFlag != BDUPDATE && iFlag != BDSELECT5) && pArray->m_lFType != 0) 
   {  
      if (bOK && !BDFTypeAttrInit(hConnect, pArray->m_lFType, pArray))
      {
         bOK = FALSE;
      }  	  
   };
   
   // Create the record set

   if (bOK)
   {         
       if (pDBase->GetRecordSetExt() != NULL &&                                         
           !pDBase->GetRecordSetExt()->IsKindOf(RUNTIME_CLASS(CFeatureAttrSet)))
      {                                                                                   
         pDBase->DeleteRecordSet();                                                       
      };                                                                                                                                                                            
      if (pDBase->GetRecordSetExt() == NULL)                                              
      {                                                                                   
         pRecordSet = new CFeatureAttrSet(pArray, pDBase);                      
         pDBase->SetRecordSet(pRecordSet);                                                
      };                                                                                  
      pRecordSet = (CFeatureAttrSet*)pDBase->GetRecordSetExt();                              
   };
   
       
   // Bind variables

   if (bOK)
   {
      pRecordSet->m_plDate = &pArray->m_lDate;
      pRecordSet->m_plOrder = &pArray->m_lOrder;
      pRecordSet->m_iFlag = iFlag;      
      pRecordSet->m_plFeature = &pArray->m_lFeature;
      pRecordSet->m_sFType = pArray->GetFTypeInternal(pDBase);
      pRecordSet->m_lFeatureMin = lFeatureMin;
      pRecordSet->m_lFeatureMax = lFeatureMax;
      pRecordSet->m_lStartDate = lStartDate;
      pRecordSet->m_lEndDate = lEndDate;
      pRecordSet->m_nDateCond = nDateCond;
      pRecordSet->m_sFilter = sFilter;
      pRecordSet->m_pDBase = pDBase;
   }

   // Perform retrieval etc.

   if (bOK && iFlag == BDDELETEALL)
   {
      sSQL.Format("delete from `%s` where `FEATURE_ID` = %li", pArray->GetFTypeInternal(pDBase), pArray->m_lFeature);
      pDBase->ExecuteSQL(sSQL);
   }
   else if (bOK)
   {
      bOK = pRecordSet->BEMGeneric(hConnect, iFlag);
   } 

   return bOK;
}

