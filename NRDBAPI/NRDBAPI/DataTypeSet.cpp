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
#include "DataTypeSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDataTypeSet

IMPLEMENT_DYNAMIC(CDataTypeSet, CRecordsetExt)

CDataTypeSet::CDataTypeSet(CNRDBase* pdb)
	: CRecordsetExt(pdb)
{
	//{{AFX_FIELD_INIT(CDataTypeSet)
	m_ID = 0;
	m_Description = _T("");
	m_nFields = 2;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CDataTypeSet::GetDefaultConnect()
{
	return _T("ODBC;DSN=BOHOL");
}

CString CDataTypeSet::GetDefaultSQL()
{
	return _T("[BDData_Types]");
}

void CDataTypeSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CDataTypeSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("[ID]"), m_ID);
	RFX_Text(pFX, _T("[Description]"), m_Description);
	//}}AFX_FIELD_MAP

   DoDataExchange(GetVar);
}

void CDataTypeSet::DoDataExchange(int iFlag)
{
   DATA_EXCHANGE(m_ID, m_plId, iFlag);
   DATA_EXCHANGE_STR(m_Description, m_psDescription, BD_SHORTSTR, iFlag);
}

CString CDataTypeSet::GetFilter()
{
   CString sFilter;
   sFilter.Format("[ID] = %li", *m_plId);
   return sFilter;
}

/////////////////////////////////////////////////////////////////////////////
// CDataTypeSet diagnostics

#ifdef _DEBUG
void CDataTypeSet::AssertValid() const
{
	CRecordsetExt::AssertValid();
}

void CDataTypeSet::Dump(CDumpContext& dc) const
{
	CRecordsetExt::Dump(dc);
}
#endif //_DEBUG

///////////////////////////////////////////////////////////////////////////////

BOOL BDDataTypeSet(BDHANDLE hConnect, LPLONG plId, LPSTR psDescription, int iFlag)
{
   BOOL bOK = TRUE;
   CDataTypeSet* pRecordSet = NULL;
 
   CNRDBase* pDBase = GetNRDBase(hConnect);
   if (pDBase == NULL)
   { 
      bOK = FALSE;
   };   
   
  // Create record set or use existing one
     
   if (bOK)
   {         
      IMPLEMENT_CREATERECORDSET(pDBase, pRecordSet, CDataTypeSet)      
   };
     
  // Bind variables
  
   if (bOK)
   {  
      pRecordSet->m_plId = plId;
      pRecordSet->m_psDescription = psDescription;      
   };
  
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