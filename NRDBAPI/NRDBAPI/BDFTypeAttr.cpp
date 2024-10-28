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
#include "BDFTypeAttr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttributeTypeSet

IMPLEMENT_DYNAMIC(CAttributeTypeSet, CRecordsetExt)

CAttributeTypeSet::CAttributeTypeSet(CNRDBase* pdb)
	: CRecordsetExt(pdb)
{
	//{{AFX_FIELD_INIT(CAttributeTypeSet)
	m_ID = 0;
	m_Feature_Type_ID = 0;
   m_ColName = _T("");
	m_Description = _T("");
	m_Data_Type_ID = 0;	
   m_FType_Link = 0;
   m_PrimaryKey = FALSE;
	m_nFields = 7;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CAttributeTypeSet::GetDefaultConnect()
{
	return _T("ODBC;DSN=BOHOL");
}

CString CAttributeTypeSet::GetDefaultSQL()
{
	return _T("[BDFType_Attr]");
}

void CAttributeTypeSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CAttributeTypeSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("[ID]"), m_ID);
	RFX_Long(pFX, _T("[Feature_Type_ID]"), m_Feature_Type_ID);
   RFX_Text(pFX, _T("[Col_Name]"), m_ColName);	
	RFX_Text(pFX, _T("[Description]"), m_Description);	
	RFX_Long(pFX, _T("[Data_Type_ID]"), m_Data_Type_ID);	
   RFX_Long(pFX, _T("[FType_Link]"), m_FType_Link);	
   RFX_Bool(pFX, _T("[Primary_Key]"), m_PrimaryKey);	
	//}}AFX_FIELD_MAP

   DoDataExchange(GetVar);
}

///////////////////////////////////////////////////////////////////////////////

void CAttributeTypeSet::DoDataExchange(int iFlag)
{
   DATA_EXCHANGE(m_ID, &m_pFTypeAttr->m_lAttrId, iFlag);   
   DATA_EXCHANGE(m_Feature_Type_ID, &m_pFTypeAttr->m_lFType, iFlag);
   DATA_EXCHANGE(m_ColName, &m_pFTypeAttr->m_sColName, iFlag);   
   DATA_EXCHANGE(m_Description, &m_pFTypeAttr->m_sDesc, iFlag);   
   DATA_EXCHANGE(m_Data_Type_ID, &m_pFTypeAttr->m_lDataType, iFlag);   
   DATA_EXCHANGE(m_FType_Link, &m_pFTypeAttr->m_lFTypeLink, iFlag);
   DATA_EXCHANGE(m_PrimaryKey, &m_pFTypeAttr->m_bPrimaryKey, iFlag);
}

///////////////////////////////////////////////////////////////////////////////

CString CAttributeTypeSet::GetFilter()
{
   CString sFilter;

   if (m_iFlag == BDSELECT2)
   {
      sFilter.Format("[Feature_Type_ID] = %li", m_pFTypeAttr->m_lFType);      
   } else
   {
      sFilter.Format("[ID] = %li and [Feature_Type_ID] = %li", m_pFTypeAttr->m_lAttrId, 
                     m_pFTypeAttr->m_lFType);      
   };
   return sFilter;
}

/////////////////////////////////////////////////////////////////////////////

CString CAttributeTypeSet::GetSortOrder()
{
   return "[ID]";
}

/////////////////////////////////////////////////////////////////////////////
// CAttributeTypeSet diagnostics

#ifdef _DEBUG
void CAttributeTypeSet::AssertValid() const
{
	CRecordsetExt::AssertValid();
}

void CAttributeTypeSet::Dump(CDumpContext& dc) const
{
	CRecordsetExt::Dump(dc);
}
#endif //_DEBUG

///////////////////////////////////////////////////////////////////////////////

BOOL BDFTypeAttr(BDHANDLE hConnect, CFTypeAttr* pFTypeAttr, int iFlag)
{
   BOOL bOK = TRUE;
   CString sSQL;
   CAttributeTypeSet* pRecordSet = NULL;
 
   CNRDBase* pDBase = GetNRDBase(hConnect);
   if (pDBase == NULL)
   { 
      bOK = FALSE;
   };   
   
  // Create record set or use existing one
     
   if (bOK)
   {         
      IMPLEMENT_CREATERECORDSET(pDBase, pRecordSet, CAttributeTypeSet)      
   };
     
  // Bind variables
  
   if (bOK)
   {  
      pRecordSet->m_pFTypeAttr = pFTypeAttr;      
      pRecordSet->m_iFlag = iFlag;
   };
  
  // Call generic retrieval function

   if (bOK && iFlag == BDDELETEALL)
   {	   
      sSQL.Format("delete from `BDFTYPE_ATTR` where `Feature_Type_ID` = %li", 
		            pFTypeAttr->GetFTypeId());
      pDBase->ExecuteSQL(sSQL);
   }
   else if (bOK)
   {   
      bOK = pRecordSet->BEMGeneric(hConnect, iFlag);      
   }
               
   return bOK;
}
