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
#include "FeatureSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFeatureSet

IMPLEMENT_DYNAMIC(CFeatureSet, CRecordsetExt)

CFeatureSet::CFeatureSet(CNRDBase* pdb)
	: CRecordsetExt(pdb)
{
	//{{AFX_FIELD_INIT(CFeatureSet)
	m_ID = 0;	
	m_Description = _T("");
	m_Parent_Feature = 0;
	m_nFields = 3;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CFeatureSet::GetDefaultConnect()
{
	return _T("ODBC;DSN=BOHOL");
}

CString CFeatureSet::GetDefaultSQL()
{   
   return "[" + m_sTable + "]";
}

void CFeatureSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CFeatureSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("[ID]"), m_ID);
	RFX_Text(pFX, _T("[Description]"), m_Description);
    RFX_Long(pFX, _T("[Parent_Feature]"), m_Parent_Feature);
	//}}AFX_FIELD_MAP

   DoDataExchange(GetVar);
}

/////////////////////////////////////////////////////////////////////////////
// CFeatureSet diagnostics

#ifdef _DEBUG
void CFeatureSet::AssertValid() const
{
	CRecordsetExt::AssertValid();
}

void CFeatureSet::Dump(CDumpContext& dc) const
{
	CRecordsetExt::Dump(dc);
}
#endif //_DEBUG

///////////////////////////////////////////////////////////////////////////////

void CFeatureSet::DoDataExchange(int iFlag)
{   
   DATA_EXCHANGE(m_ID, &m_pFeature->m_lId, iFlag);
   DATA_EXCHANGE(m_Description, &m_pFeature->m_sName, iFlag);
   DATA_EXCHANGE(m_Parent_Feature, &m_pFeature->m_lParentFeature, iFlag);
}

///////////////////////////////////////////////////////////////////////////////

CString CFeatureSet::GetFilter()
{
   CString sFilter;

   if (m_iFlag == BDSELECT2)
   {      
   } 
   else if (m_iFlag == BDSELECT3)
   {      
	  CString s = m_pFeature->m_sName;
#ifdef NRDBMYSQL
	  s.Replace("'","\\'");
#else
	  s.Replace("'","''");
#endif
      sFilter.Format("[Description] = '%s'", (LPCSTR)s);

   }      
   else if (m_iFlag == BDSELECT4)
   {
      sFilter.Format("[Parent_Feature] = %li", m_pFeature->m_lParentFeature);
   }
   else
   {
      sFilter.Format("[ID] = %li", m_pFeature->m_lId);
   }
   return sFilter;
}

/////////////////////////////////////////////////////////////////////////////

CString CFeatureSet::GetSortOrder()
{
   return "[Description]";
}

/////////////////////////////////////////////////////////////////////////////

BOOL BDFeature(BDHANDLE hConnect, CFeature* pFeature, int iFlag)
{
   BOOL bOK = TRUE;
   CFeatureSet* pRecordSet = NULL;
   CFeatureType ftype;
   CAttrArray aAttr;
 
   CNRDBase* pDBase = GetNRDBase(hConnect);
   if (pDBase == NULL)
   { 
      bOK = FALSE;
   };   
       
   // Nb. If selecting features by feature type, if the feature type has
   // a one-to many relationship then the features are selected from the
   // parent feature type

   if (bOK && (iFlag == BDSELECT || iFlag == BDSELECT2 || iFlag == BDSELECT3 || 
	   iFlag == BDDELETE || iFlag == BDGETINIT))
   {	               
      BDFTypeI(hConnect, pFeature->m_lFeatureTypeId, &ftype);	   
      pFeature->m_lFeatureTypeId = ftype.m_lId;            
   }

   // Ensure there is a feature type id set

   ASSERT(iFlag != BDSELECT || pFeature->m_lFeatureTypeId != 0);

   if (bOK && (iFlag != BDGETNEXT && iFlag != BDINSERTNEXT))
   {
      bOK = BDFTypeAttrInit(hConnect, pFeature->m_lFeatureTypeId, &aAttr);
      
   }

   // If deleting then delete the corresponding data

   if (bOK && iFlag == BDDELETE)
   {
	   CString sSQL;
	   sSQL.Format("delete from `%s` where `FEATURE_ID` = %li", aAttr.GetFTypeInternal(pDBase), pFeature->m_lId);
	   pDBase->ExecuteSQL(sSQL);
   }   

  // Create record set or use existing one
     
   if (bOK)
   {         
      IMPLEMENT_CREATERECORDSET(pDBase, pRecordSet, CFeatureSet)           
   };
     
  // Bind variables
  
   if (bOK)
   {              
	   pRecordSet->m_pFeature = pFeature;            
      pRecordSet->m_iFlag = iFlag;      
      pRecordSet->m_sTable = aAttr.GetFTypeTable(pDBase);       
   };

  // If updating features then reset globally held info

   if (iFlag == BDADD || iFlag == BDUPDATE || iFlag == BDDELETE)
   {
	   internalinfo.m_links.Reset();
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

