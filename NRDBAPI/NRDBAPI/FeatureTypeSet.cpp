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
#include "nrdbapi.h"
#include "FeatureTypeSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFeatureTypeSet

IMPLEMENT_DYNAMIC(CFeatureTypeSet, CRecordsetExt)

CFeatureTypeSet::CFeatureTypeSet(CNRDBase* pdb)
	: CRecordsetExt(pdb)
{
	//{{AFX_FIELD_INIT(CFeatureTypeSet)
	m_ID = 0;
	m_Description = _T("");
   m_Internal = _T("");
	m_Parent_FType = 0;
   m_ManyToOne = 0;
   m_Dictionary = 0;
	m_nFields = 6;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
   
}

CString CFeatureTypeSet::GetDefaultConnect()
{
	return _T("ODBC;DSN=BOHOL");
}

CString CFeatureTypeSet::GetDefaultSQL()
{
	return _T("[BDFeature_Types]");
}

void CFeatureTypeSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CFeatureTypeSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("[ID]"), m_ID);
	RFX_Text(pFX, _T("[Description]"), m_Description);
   RFX_Text(pFX,_T("[Internal]"), m_Internal);
	RFX_Long(pFX, _T("[Parent_FType]"), m_Parent_FType);	
	RFX_Bool(pFX, _T("[ManyToOne]"), m_ManyToOne);      
   RFX_Long(pFX, _T("[Dictionary]"), m_Dictionary);
	//}}AFX_FIELD_MAP

   DoDataExchange(GetVar);   
}

///////////////////////////////////////////////////////////////////////////////

void CFeatureTypeSet::DoDataExchange(int iFlag)
{
   DATA_EXCHANGE(m_ID, &m_pFeatureType->m_lId, iFlag);
   DATA_EXCHANGE(m_Description, &m_pFeatureType->m_sDesc, iFlag);
   DATA_EXCHANGE(m_Internal, &m_pFeatureType->m_sInternal, iFlag);
   DATA_EXCHANGE(m_Parent_FType, &m_pFeatureType->m_lParentFType, iFlag);
   DATA_EXCHANGE(m_ManyToOne, &m_pFeatureType->m_bManyToOne, iFlag);   
   DATA_EXCHANGE(m_Dictionary, &m_pFeatureType->m_lDictionary, iFlag);
}

///////////////////////////////////////////////////////////////////////////////

CString CFeatureTypeSet::GetFilter()
{
   CString sFilter;

   if (m_iFlag == BDSELECT2)
   {
      sFilter.Format("[Description] = '%s'",(LPCSTR)m_pFeatureType->m_sDesc);
   } else
   {
      sFilter.Format("ID = %li", m_pFeatureType->m_lId);
   };
   
   return sFilter;
}

/////////////////////////////////////////////////////////////////////////////
// CFeatureTypeSet diagnostics

#ifdef _DEBUG
void CFeatureTypeSet::AssertValid() const
{
	CRecordsetExt::AssertValid();
}

void CFeatureTypeSet::Dump(CDumpContext& dc) const
{
	CRecordsetExt::Dump(dc);
}
#endif //_DEBUG

///////////////////////////////////////////////////////////////////////////////

BOOL BDFeatureType(BDHANDLE hConnect, CFeatureType* pFeatureType, int iFlag)
{
   BOOL bOK = TRUE;
   CFeatureTypeSet* pRecordSet = NULL;
 
   CNRDBase* pDBase = GetNRDBase(hConnect);
   if (pDBase == NULL)
   { 
      bOK = FALSE;
   };   
   
  // Create record set or use existing one
     
   if (bOK)
   {         
      IMPLEMENT_CREATERECORDSET(pDBase, pRecordSet, CFeatureTypeSet)      
   };
     
  // Bind variables
  
   if (bOK)
   {  
	  pRecordSet->m_pFeatureType = pFeatureType;      
     pRecordSet->m_iFlag = iFlag;
   };

  // If updating ftypes then reset globally held info

   if (iFlag == BDADD || iFlag == BDUPDATE || iFlag == BDDELETE)
   {
	   internalinfo.m_links.Reset();
   }


  // When deleting a feature type, delete the corresponding attribute
  // definitions

   if (bOK && (iFlag == BDDELETE || iFlag == BDUPDATE))
   {
      CString sSQL;
      sSQL.Format("delete from BDFType_Attr where Feature_Type_ID = %li", pFeatureType->m_lId);
      pDBase->ExecuteSQL(sSQL);
   }
  
  // Call generic retrieval function
    
   if (bOK && pRecordSet != NULL)
   {   
      bOK = pRecordSet->BEMGeneric(hConnect, iFlag);      
   } else
   { 
      bOK = FALSE;
   };
               
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Returns the feature type for the id.  If the feature type uses 1:1 inheritance
// then return the parent
//

BOOL BDFTypeI(BDHANDLE hConnect, long lFType, CFeatureType* pFType)
{     
 // Get the feature type

   pFType->m_lId = lFType;
   BOOL bFound = BDFeatureType(hConnect, pFType, BDSELECT);
   BDEnd(hConnect);

   // If the feature type uses one to one inheritance then get the parent

   while (bFound && pFType->m_lParentFType != 0 &&       
          !pFType->m_bManyToOne)
   {
      // Determine the parent feature type
      
         pFType->m_lId = pFType->m_lParentFType;
         bFound = BDFeatureType(hConnect, pFType, BDSELECT);
         BDEnd(hConnect);
         ASSERT(bFound);           
   }
   BDEnd(hConnect);
   
   return bFound;
}

///////////////////////////////////////////////////////////////////////////////
//
// Returns the parent of the feature type.  This includes skipping over any 
// one to one relationships.  Returns false if no parent exists
//

BOOL BDFTypeParentI(BDHANDLE hConnect, long lFType, CFeatureType* pFType)
{
   BOOL bFound = BDFTypeI(hConnect, lFType, pFType);
   if (bFound && pFType->m_lParentFType != 0)
   {
      ASSERT(pFType->m_bManyToOne);

      bFound = BDFTypeI(hConnect, pFType->m_lParentFType, pFType);
      BDEnd(hConnect);
   } else
   {
      bFound = FALSE;
   }

   return bFound;
}


