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
#include "BDProjectionSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//
// Initialise parameters, Nb. Access cannot handle BOOL with value other than
// 1 or 0

CBDProjection::CBDProjection()
{
   m_nID = 0;	
   m_dSemiMajorAxis = 0;
   m_dSemiMinorAxis = 0;
   m_dLatitudeOrigin = 0;
   m_dLongitudeOrigin = 0;
   m_dFalseEasting = 0;
   m_dFalseNorthing = 0;
   m_dScaleFactorAtOrigin = 0;
   m_lScaleCoords = 0;
   m_dMinEasting = 0;
   m_dMaxEasting = 0;
   m_dMinNorthing = 0;
   m_dMaxNorthing = 0;
   m_bDefault = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CBDProjectionSet

IMPLEMENT_DYNAMIC(CBDProjectionSet, CRecordsetExt)

CBDProjectionSet::CBDProjectionSet(CNRDBase* pdb)
	: CRecordsetExt(pdb)
{
	//{{AFX_FIELD_INIT(CBDProjectionSet)
	m_ID = 0;
	m_Name = _T("");
	m_Semi_major_axis = 0.0;
	m_Semi_minor_axis = 0.0;
	m_Latitude_origin = 0.0;
	m_Longitude = 0.0;
	m_False_easting = 0.0;
	m_False_northing = 0.0;
	m_Scale_factor_at_origin = 0.0;
   m_ScaleCoords = 0;
	m_Default = FALSE;
   m_dMinEasting = 0;
   m_dMaxEasting = 0;
   m_dMinNorthing = 0;
   m_dMaxNorthing = 0;
	m_nFields = 15;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CBDProjectionSet::GetDefaultConnect()
{
	return _T("ODBC;DSN=NRDB");
}

CString CBDProjectionSet::GetDefaultSQL()
{
	return _T("[BDProjection]");
}

void CBDProjectionSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CBDProjectionSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("[ID]"), m_ID);
	RFX_Text(pFX, _T("[Name]"), m_Name);
	RFX_Double(pFX, _T("[Semi-major axis]"), m_Semi_major_axis);
	RFX_Double(pFX, _T("[Semi-minor axis]"), m_Semi_minor_axis);
	RFX_Double(pFX, _T("[Latitude origin]"), m_Latitude_origin);
	RFX_Double(pFX, _T("[Longitude]"), m_Longitude);
	RFX_Double(pFX, _T("[False easting]"), m_False_easting);
	RFX_Double(pFX, _T("[False northing]"), m_False_northing);
	RFX_Double(pFX, _T("[Scale factor at origin]"), m_Scale_factor_at_origin);
   RFX_Long(pFX, _T("[Scale Coords]"), m_ScaleCoords);
   RFX_Double(pFX, _T("[MinEasting]"), m_dMinEasting);
   RFX_Double(pFX, _T("[MaxEasting]"), m_dMaxEasting);
   RFX_Double(pFX, _T("[MinNorthing]"), m_dMinNorthing);
   RFX_Double(pFX, _T("[MaxNorthing]"), m_dMaxNorthing);   
	RFX_Bool(pFX, _T("[Default]"), m_Default);
	//}}AFX_FIELD_MAP

   // Not recognising null on AddNew
   
   if (m_iFlag == BDADD && pFX->m_nOperation == CFieldExchange::MarkForAddNew)
   {              
		SetFieldDirty(&m_dMinEasting, TRUE);                  			   
      SetFieldNull(&m_dMinEasting, m_dMinEasting == AFX_RFX_DOUBLE_PSEUDO_NULL);
      SetFieldDirty(&m_dMaxEasting, TRUE);                  			   
      SetFieldNull(&m_dMaxEasting, m_dMinEasting == AFX_RFX_DOUBLE_PSEUDO_NULL);
      SetFieldDirty(&m_dMinNorthing, TRUE);                  			   
      SetFieldNull(&m_dMinNorthing, m_dMinEasting == AFX_RFX_DOUBLE_PSEUDO_NULL);
      SetFieldDirty(&m_dMaxNorthing, TRUE);                  			   
      SetFieldNull(&m_dMaxNorthing, m_dMinEasting == AFX_RFX_DOUBLE_PSEUDO_NULL);
   };
   

   DoDataExchange(GetVar);   
}

/////////////////////////////////////////////////////////////////////////////
// CBDProjectionSet diagnostics

#ifdef _DEBUG
void CBDProjectionSet::AssertValid() const
{
	CRecordsetExt::AssertValid();
}

void CBDProjectionSet::Dump(CDumpContext& dc) const
{
	CRecordsetExt::Dump(dc);
}
#endif


/////////////////////////////////////////////////////////////////////////////

void CBDProjectionSet::DoDataExchange(int iFlag)
{
   DATA_EXCHANGE(m_ID, &m_pProjection->m_nID, iFlag);
   DATA_EXCHANGE(m_Name, &m_pProjection->m_sName, iFlag);
   DATA_EXCHANGE(m_Semi_major_axis, &m_pProjection->m_dSemiMajorAxis, iFlag);
   DATA_EXCHANGE(m_Semi_minor_axis, &m_pProjection->m_dSemiMinorAxis, iFlag);
   DATA_EXCHANGE(m_Latitude_origin, &m_pProjection->m_dLatitudeOrigin, iFlag);
   DATA_EXCHANGE(m_Longitude, &m_pProjection->m_dLongitudeOrigin, iFlag); 
   DATA_EXCHANGE(m_False_easting, &m_pProjection->m_dFalseEasting, iFlag);   
   DATA_EXCHANGE(m_False_northing, &m_pProjection->m_dFalseNorthing, iFlag);   
	
	DATA_EXCHANGE(m_dMinEasting, &m_pProjection->m_dMinEasting, iFlag);   
	DATA_EXCHANGE(m_dMaxEasting, &m_pProjection->m_dMaxEasting, iFlag);   
	DATA_EXCHANGE(m_dMinNorthing, &m_pProjection->m_dMinNorthing, iFlag);   
	DATA_EXCHANGE(m_dMaxNorthing, &m_pProjection->m_dMaxNorthing, iFlag);   
	DATA_EXCHANGE(m_Scale_factor_at_origin, &m_pProjection->m_dScaleFactorAtOrigin, iFlag);   
	DATA_EXCHANGE(m_ScaleCoords, &m_pProjection->m_lScaleCoords, iFlag);	

	DATA_EXCHANGE(m_Default, &m_pProjection->m_bDefault, iFlag);      							

}

///////////////////////////////////////////////////////////////////////////////

CString CBDProjectionSet::GetFilter()
{
   CString s;

   if (m_iFlag == BDSELECT2)
   { 
      s.Format("[Default] = %i", m_pProjection->m_bDefault);
   } else
   {
      s.Format("[ID] = %li", m_pProjection->m_nID);
   };
   return s;
}

///////////////////////////////////////////////////////////////////////////////

BOOL BDProjection(BDHANDLE hConnect, CBDProjection* pProjection, int iFlag)
{
   BOOL bOK = TRUE;
   CBDProjectionSet* pRecordSet = NULL;
 
   CNRDBase* pDBase = GetNRDBase(hConnect);
   if (pDBase == NULL)
   { 
      bOK = FALSE;
   };   
   
  // Create record set or use existing one
     
   if (bOK)
   {         
      IMPLEMENT_CREATERECORDSET(pDBase, pRecordSet, CBDProjectionSet)      
   };
     
  // Bind variables
  
   if (bOK)
   {  
	  pRecordSet->m_pProjection = pProjection;      
     pRecordSet->m_iFlag = iFlag;
     pRecordSet->m_pDBase = pDBase;
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
