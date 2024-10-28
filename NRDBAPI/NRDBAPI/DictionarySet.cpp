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
#include "DictionarySet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CDictionary::CDictionary()
{
   m_lId = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CDictionarySet

IMPLEMENT_DYNAMIC(CDictionarySet, CRecordsetExt)

CDictionarySet::CDictionarySet(CNRDBase* pdb)
	: CRecordsetExt(pdb)
{
	//{{AFX_FIELD_INIT(CDictionarySet)
	m_ID = 0;
	m_Description = _T("");
	m_nFields = 2;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CDictionarySet::GetDefaultConnect()
{
	return _T("ODBC;DSN=BOHOL");
}

CString CDictionarySet::GetDefaultSQL()
{
	return _T("[BDDictionary]");
}

void CDictionarySet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CDictionarySet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("[ID]"), m_ID);
	RFX_Text(pFX, _T("[Description]"), m_Description);
	//}}AFX_FIELD_MAP

   DoDataExchange(GetVar);   
}

/////////////////////////////////////////////////////////////////////////////
// CDictionarySet diagnostics

#ifdef _DEBUG
void CDictionarySet::AssertValid() const
{
	CRecordsetExt::AssertValid();
}

void CDictionarySet::Dump(CDumpContext& dc) const
{
	CRecordsetExt::Dump(dc);
}
#endif //_DEBUG

///////////////////////////////////////////////////////////////////////////////

void CDictionarySet::DoDataExchange(int iFlag)
{
   DATA_EXCHANGE(m_ID, &m_pDictionary->m_lId, iFlag);
   DATA_EXCHANGE(m_Description, &m_pDictionary->m_sDesc, iFlag);   
}

///////////////////////////////////////////////////////////////////////////////

CString CDictionarySet::GetFilter()
{
   CString sFilter;   
   if (m_iFlag == BDSELECT2)
   {
      sFilter.Format("[Description] = '%s'",m_pDictionary->m_sDesc);
   } else
   {
      sFilter.Format("[ID] = %li", m_pDictionary->m_lId);      
   }
   return sFilter;
}

///////////////////////////////////////////////////////////////////////////////

BOOL BDDictionary(BDHANDLE hConnect, CDictionary* pDictionary, int iFlag)
{
   BOOL bOK = TRUE;
   CDictionarySet* pRecordSet = NULL;
 
   CNRDBase* pDBase = GetNRDBase(hConnect);
   if (pDBase == NULL)
   { 
      bOK = FALSE;
   };   
   
  // Create record set or use existing one
     
   if (bOK)
   {         
      IMPLEMENT_CREATERECORDSET(pDBase, pRecordSet, CDictionarySet)      
   };
     
  // Bind variables
  
   if (bOK)
   {  
	  pRecordSet->m_pDictionary = pDictionary;      
     pRecordSet->m_iFlag = iFlag;
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
