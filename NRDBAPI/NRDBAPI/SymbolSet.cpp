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
#include "SymbolSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSymbolSet

IMPLEMENT_DYNAMIC(CSymbolSet, CRecordsetExt)

CSymbolSet::CSymbolSet(CNRDBase* pdb)
	: CRecordsetExt(pdb)
{
	//{{AFX_FIELD_INIT(CSymbolSet)
	m_ID = 0;
	m_nFields = 2;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CSymbolSet::GetDefaultConnect()
{
	return _T("ODBC;DSN=nrdbdemo");
}

CString CSymbolSet::GetDefaultSQL()
{
	return _T("[BDSymbol]");
}

void CSymbolSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CSymbolSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("[ID]"), m_ID);
	RFX_LongBinary(pFX, _T("[Image]"), m_Image);
	//}}AFX_FIELD_MAP

   // Indicate that binary object has changed

   if ((m_iFlag == BDUPDATE && pFX->m_nOperation == CFieldExchange::MarkForUpdate) ||
	   (m_iFlag == BDADD && pFX->m_nOperation == CFieldExchange::MarkForAddNew))
   {              
		SetFieldDirty(&m_Image, TRUE);                  			   
      SetFieldNull(&m_Image, !m_Image.m_hData);
   };

   DoDataExchange(GetVar);   
}

/////////////////////////////////////////////////////////////////////////////
// CSymbolSet diagnostics

#ifdef _DEBUG
void CSymbolSet::AssertValid() const
{
	CRecordsetExt::AssertValid();
}

void CSymbolSet::Dump(CDumpContext& dc) const
{
	CRecordsetExt::Dump(dc);
}
#endif //_DEBUG


///////////////////////////////////////////////////////////////////////////////

void CSymbolSet::DoDataExchange(int iFlag)
{
   DATA_EXCHANGE(m_ID, &m_pSymbol->m_lId, iFlag);
   DATA_EXCHANGE_LONGBIN(m_Image, &m_pSymbol->m_symbol, iFlag);               
}

///////////////////////////////////////////////////////////////////////////////

CString CSymbolSet::GetFilter()
{
   return "[ID]";   
}

///////////////////////////////////////////////////////////////////////////////

BOOL BDSymbol(BDHANDLE hConnect, CBDSymbol *pSymbol, int iFlag)
{
   BOOL bOK = TRUE;
   CSymbolSet* pRecordSet = NULL;
 
   CNRDBase* pDBase = GetNRDBase(hConnect);
   if (pDBase == NULL)
   { 
      bOK = FALSE;
   };   
   
  // Create record set or use existing one
     
   if (bOK)
   {         
      IMPLEMENT_CREATERECORDSET(pDBase, pRecordSet, CSymbolSet)      
   };
     
  // Bind variables
  
   if (bOK)
   {  
	  pRecordSet->m_pSymbol = pSymbol;      
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