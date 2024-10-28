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
#include "BDMainSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBDMainSet

IMPLEMENT_DYNAMIC(CBDMainSet, CRecordsetExt)

CBDMainSet::CBDMainSet(CNRDBase* pdb)
	: CRecordsetExt(pdb)
{
	//{{AFX_FIELD_INIT(CBDMainSet)
	m_Country = _T("");
	m_Province = _T("");
	m_City = _T("");
	m_Organization = _T("");
	m_Address = _T("");
	m_Telephone = _T("");
	m_Fax = _T("");
	m_Email = _T("");
   m_Logo = _T("");
   m_CoordAsLatLon = FALSE;
   m_Version = 0;
   m_UseCustomHeader = FALSE;
   m_UseCustomFooter = FALSE;
   m_CustomHeader = _T(""); 
   m_CustomFooter = _T("");
   m_StyleSheet = _T("");
	m_nFields = 16;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CBDMainSet::GetDefaultConnect()
{
	return _T("ODBC;DSN=BOHOL");
}

CString CBDMainSet::GetDefaultSQL()
{
	return _T("[BDMain]");
}

void CBDMainSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CBDMainSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Text(pFX, _T("[Country]"), m_Country);
	RFX_Text(pFX, _T("[Province]"), m_Province);
	RFX_Text(pFX, _T("[City]"), m_City);
	RFX_Text(pFX, _T("[Organization]"), m_Organization);
	RFX_Text(pFX, _T("[Address]"), m_Address);
	RFX_Text(pFX, _T("[Telephone]"), m_Telephone);
	RFX_Text(pFX, _T("[Fax]"), m_Fax);
	RFX_Text(pFX, _T("[Email]"), m_Email);
   RFX_Text(pFX, _T("[Logo]"), m_Logo);
   RFX_Bool(pFX, _T("[CoordAsLatLon]"), m_CoordAsLatLon);
   RFX_Bool(pFX, _T("[UseCustomHeader]"), m_UseCustomHeader);
   RFX_Bool(pFX, _T("[UseCustomFooter]"), m_UseCustomFooter);
   RFX_Text(pFX, _T("[CustomHeader]"), m_CustomHeader);
   RFX_Text(pFX, _T("[CustomFooter]"), m_CustomFooter);
   RFX_Text(pFX, _T("[StyleSheet]"), m_StyleSheet);   
   RFX_Int(pFX, _T("[Version]"), m_Version);
	//}}AFX_FIELD_MAP

   DoDataExchange(GetVar);   
}

/////////////////////////////////////////////////////////////////////////////
// CBDMainSet diagnostics

#ifdef _DEBUG
void CBDMainSet::AssertValid() const
{
	CRecordsetExt::AssertValid();
}

void CBDMainSet::Dump(CDumpContext& dc) const
{
	CRecordsetExt::Dump(dc);
}
#endif //_DEBUG

///////////////////////////////////////////////////////////////////////////////

void CBDMainSet::DoDataExchange(int iFlag)
{   
   DATA_EXCHANGE(m_Country, &m_pMain->m_Country, iFlag);
	DATA_EXCHANGE(m_Province, &m_pMain->m_Province, iFlag);
	DATA_EXCHANGE(m_City, &m_pMain->m_City, iFlag);
	DATA_EXCHANGE(m_Organization, &m_pMain->m_Organization, iFlag);
	DATA_EXCHANGE(m_Address, &m_pMain->m_Address, iFlag);
	DATA_EXCHANGE(m_Telephone, &m_pMain->m_Telephone, iFlag);
	DATA_EXCHANGE(m_Fax, &m_pMain->m_Fax, iFlag);
	DATA_EXCHANGE(m_Email, &m_pMain->m_Email, iFlag);
   DATA_EXCHANGE(m_Logo, &m_pMain->m_Logo, iFlag);
   DATA_EXCHANGE(m_CoordAsLatLon, &m_pMain->m_bCoordAsLatLon, iFlag);   
   DATA_EXCHANGE(m_UseCustomHeader, &m_pMain->m_bCustomHeader, iFlag);   
   DATA_EXCHANGE(m_UseCustomFooter, &m_pMain->m_bCustomFooter, iFlag);   
   DATA_EXCHANGE(m_CustomHeader, &m_pMain->m_sCustomHeader, iFlag);   
   DATA_EXCHANGE(m_CustomFooter, &m_pMain->m_sCustomFooter, iFlag);   
   DATA_EXCHANGE(m_StyleSheet, &m_pMain->m_sStyleSheet, iFlag);   
   DATA_EXCHANGE(m_Version, &m_pMain->m_nVersion, iFlag);   
}

///////////////////////////////////////////////////////////////////////////////

CString CBDMainSet::GetFilter()
{
   return "";
}

///////////////////////////////////////////////////////////////////////////////

BOOL BDMain(BDHANDLE hConnect, CBDMain* pMain, int iFlag)
{
   BOOL bOK = TRUE;
   CBDMainSet* pRecordSet = NULL;
 
   CNRDBase* pDBase = GetNRDBase(hConnect);
   if (pDBase == NULL)
   { 
      bOK = FALSE;
   };     

  // Create record set or use existing one
     
   if (bOK)
   {         
      IMPLEMENT_CREATERECORDSET(pDBase, pRecordSet, CBDMainSet)      
   };

     
  // Bind variables
  
   if (bOK)
   {  
	  pRecordSet->m_pMain = pMain;      
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
