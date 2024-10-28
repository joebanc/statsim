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
#include "nrdbase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Pointer to data structure holding global information
//

#pragma data_seg(".NRDB")
CInternalInfo internalinfo;
#pragma data_seg()

///////////////////////////////////////////////////////////////////////////////
//
// Allow type checking at runtime
//

#ifdef NRDBMYSQL
IMPLEMENT_DYNAMIC(CNRDBase, CMySQLDatabase);
IMPLEMENT_DYNAMIC(CMySQLDatabase, CObject); 
#else
IMPLEMENT_DYNAMIC(CNRDBase, CDatabase);
#endif

///////////////////////////////////////////////////////////////////////////////
//
// CNRDBase* GetNRDBase(BDHANDLE)
//
// Converts between a handle and a CNRDBase object
//

CNRDBase* GetNRDBase(BDHANDLE hConnect)
{    
   // Retrieve connection from index

   CNRDBase *pDBase = (CNRDBase*)hConnect;
      
   if (pDBase == NULL || pDBase == (BDHANDLE)-1 ||
       !pDBase->IsKindOf(RUNTIME_CLASS(CNRDBase)))
   {              
/*#ifndef NRDBMYSQL
      ASSERT(FALSE);      
#endif*/
      return NULL;
   };
   
   return pDBase;       
};

/////////////////////////////////////////////////////////////////////////////
// CNRDBase construction

CNRDBase::CNRDBase()
{
   m_pRecordSet = NULL;   
   m_pRecordSetInt = NULL;
   m_nOnError = DisplayMessage;
   m_nTransLevel = 0;
   m_bJetDB = FALSE;
   m_bInternal = FALSE; // using internal connection
}

///////////////////////////////////////////////////////////////////////////////

BOOL CNRDBase::Open(LPCSTR sDSN, LPCSTR sUser, LPCSTR sPassword, LPCSTR sHost)
{
   CString sDriver;

#ifdef NRDBMYSQL
   if (CMySQLDatabase::Open(sDSN, sUser, sPassword, sHost))
#else
   if (CDatabase::Open(sDSN))
#endif
   {
      GetDriver(sDriver);
      m_bJetDB = sDriver.CompareNoCase("ODBCJT32.DLL") == 0;
      return TRUE;
   }
   return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//
// CNRDBase::~CNRDBase()
//

CNRDBase::~CNRDBase()
{                                                 
   DeleteRecordSet();   
   
   if (IsOpen())
   {
      Close();
   };
}

///////////////////////////////////////////////////////////////////////////////
//
// BOOL CNRDBase::SetRecordSet(CRecordsetExt* pSet);
//
// Sets the database pointer to the current record set
//

BOOL CNRDBase::SetRecordSet(CRecordsetExt* pSet)
{                 
   BOOL bOK = TRUE;         
   
   if (!m_bInternal)
   {
      if (m_pRecordSet == NULL)
      {
         m_pRecordSet = pSet;
      } else
      { 
         ASSERT(FALSE);
         bOK = FALSE;      
      };              
   }
   
   // Internal

   else
   {
      if (m_pRecordSetInt == NULL)
      {
         m_pRecordSetInt = pSet;
      } else
      { 
         ASSERT(FALSE);
         bOK = FALSE;      
      };              
   }
   return bOK;  
};            

///////////////////////////////////////////////////////////////////////////////
//
// Set the recordset
//

CRecordsetExt* CNRDBase::GetRecordSetExt()
{
   if (!m_bInternal)
   {
      return (CRecordsetExt*)m_pRecordSet;
   } 
   else
   {
      return (CRecordsetExt*)m_pRecordSetInt;
   }
}

///////////////////////////////////////////////////////////////////////////////
//
// BOOL CNRDBase::DeleteRecordSet()
//
// Deletes the record set object
//

BOOL CNRDBase::DeleteRecordSet()
{
   BOOL bOK = TRUE;
  
  // Indicate the end of time series retrieval/insertion
  
  // Delete first record set

   if (!m_bInternal)
   {  
      if (m_pRecordSet != NULL)
      {  
         if (m_pRecordSet->IsOpen())
         {
            m_pRecordSet->Close();
         };
         delete m_pRecordSet;      
      }
      else
      {      
         bOK = FALSE;
      };    
      m_pRecordSet = NULL;
   }

   // Delete the internal recordset

   else 
   {   
      if (m_pRecordSetInt != NULL)
      {
         if (m_pRecordSetInt->IsOpen()) m_pRecordSetInt->Close();
         delete m_pRecordSetInt;
      }
      m_pRecordSetInt = NULL;
   };

   return bOK;
};

///////////////////////////////////////////////////////////////////////////////

void CFeatureLinks::Reset()
{  
  m_alFType.RemoveAll();
  m_aFeature.RemoveAll();
}

///////////////////////////////////////////////////////////////////////////////

void BDOnError(BDHANDLE hConnect, int iFlag)
{
   CNRDBase* pDBase = GetNRDBase(hConnect);
   if (pDBase != NULL)
   {
      pDBase->m_nOnError = iFlag;
   };
}

///////////////////////////////////////////////////////////////////////////////

void BDGetError(BDHANDLE hConnect, CString& sError)
{
   CNRDBase* pDBase = GetNRDBase(hConnect);
   if (pDBase != NULL)
   {
      sError = pDBase->m_sError;
   };
}

///////////////////////////////////////////////////////////////////////////////

void BDSetActiveWnd(HWND hWnd)
{
   internalinfo.m_hwnd = hWnd;
}

///////////////////////////////////////////////////////////////////////////////
//
// Determines the ODBC driver file
//


BOOL BDGetDriver(BDHANDLE hConnect, CString& sVersion)
{
   CNRDBase* pDBase = GetNRDBase(hConnect); 
   if (pDBase != NULL)
   {
      return pDBase->GetDriver(sVersion);   
   } else
   {
#ifdef NRDBMYSQL
      const char *sClient = mysql_get_client_info();
      sVersion = "MySQL " + (CString)sClient;
      return TRUE;
#endif
      return FALSE;
   }
}

///////////////////////////////////////////////////////////////////////////////

int BDGetVersion(BDHANDLE hConnect)
{
   CNRDBase* pDBase = GetNRDBase(hConnect); 
   return pDBase->GetDBVersion();
}

///////////////////////////////////////////////////////////////////////////////

#ifndef NRDBMYSQL
BOOL CNRDBase::GetDriver(CString& sVersion)
{

   char sResult[32] = "";
   short nLen;

   if (SQLGetInfo(m_hdbc, SQL_DRIVER_NAME, sResult, sizeof(sResult), &nLen) == SQL_SUCCESS)
   {
      sVersion = sResult;
      return TRUE;
   }   
   return FALSE;
}
#endif

///////////////////////////////////////////////////////////////////////////////

CString CFTypeLinks::GetFType(long lFType)
{
   if (m_aFType.GetSize() == 0)
   {
       LoadFTypes();
   }

   for (int i = 0; i < m_aFType.GetSize(); i++)
   {
      if (m_aFType[i].m_lId == lFType)
      {
         return m_aFType[i].m_sDesc;
      }
   }
   return "";
}

///////////////////////////////////////////////////////////////////////////////

void CFTypeLinks::LoadFTypes()
{
   CFeatureType ftype;
	BOOL bFound;

	// Reset existing features

	Reset();
	
	// Retrieve the features for this type
   	
   bFound = BDFeatureType(internalinfo.m_hConnectInt, &ftype, BDGETINIT);
	while (bFound)
	{
        m_aFType.Add(ftype);

		bFound = BDGetNext(internalinfo.m_hConnectInt);
	}	
   BDEnd(internalinfo.m_hConnectInt);

   // Now for features with 1:1 inheritance change the name to their parent

   for (int i = 0; i < m_aFType.GetSize(); i++)
   {
      if (!m_aFType[i].m_bManyToOne && m_aFType[i].m_lParentFType != 0)
      {         
         BDFTypeI(internalinfo.m_hConnectInt, m_aFType[i].m_lId, &ftype);
         m_aFType[i].m_sDesc = ftype.m_sDesc;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void CFTypeLinks::Reset()
{
  m_aFType.RemoveAll();
};

///////////////////////////////////////////////////////////////////////////////

CString CFeatureLinks::GetFeature(long lFeature, long lFType)
{
   // Determine if the features are already loaded

   for (int i = 0; i < m_alFType.GetSize(); i++)
   {
      if (m_alFType[i] == lFType) break;
   }
   if (i == m_alFType.GetSize()) LoadFeatures(lFType);

   // Retrieve the required feature

   for (i = 0; i < m_aFeature.GetSize(); i++)
   {
	   if (m_aFeature[i].m_lId == lFeature && 
          m_aFeature[i].m_lFeatureTypeId == lFType)
	   {
		   return m_aFeature[i].m_sName;
	   }
   }   
   return "";
}

///////////////////////////////////////////////////////////////////////////////

void CFeatureLinks::LoadFeatures(long lFType)
{
	CFeature feature;
	BOOL bFound;

	// Reset existing features

   m_alFType.Add(lFType);
	
	// Retrieve the features for this type

   feature.m_lFeatureTypeId = lFType;
	bFound = BDFeature(internalinfo.m_hConnectInt, &feature, BDSELECT2);
	while (bFound)
	{
        feature.m_sName.TrimRight();
        m_aFeature.Add(feature);

		bFound = BDGetNext(internalinfo.m_hConnectInt);
	}
	BDEnd(internalinfo.m_hConnectInt);
}

///////////////////////////////////////////////////////////////////////////////

CFeature::CFeature()
{
   m_lId = 0;
   m_lFeatureTypeId = 0;   
   m_lParentFeature = 0;
};

CFeature::CFeature(CFeature& rSrc)
{
   m_lId = rSrc.m_lId;
   m_lFeatureTypeId = rSrc.m_lFeatureTypeId;
   m_sName = rSrc.m_sName;
   m_lParentFeature = rSrc.m_lParentFeature;   
}

///////////////////////////////////////////////////////////////////////////////

CFeatureType::CFeatureType()
{
   m_lId = 0;   
   m_lParentFType = 0;
   m_bManyToOne = FALSE;
   m_lDictionary = 0;
}

///////////////////////////////////////////////////////////////////////////////

#ifndef NRDBMYSQL

// Retrieve data source names for ODBC

//BOOL BDGetDataSourceNames(HANDLE &henv, CString &sDSN)
BOOL BDGetDataSourceNames(HANDLE &henv, CString &sDSN, LPCSTR sUser, LPCSTR sPassword, LPCSTR sHost)

{   
   UCHAR szDSN[SQL_MAX_DSN_LENGTH+1];
   UCHAR sDescription[255];
   SWORD wDirection = SQL_FETCH_FIRST;         
   SWORD wLenDSN, wLenDesc;	

   if (henv == NULL)
   {
      SQLAllocEnv(&henv);
   } else
   {
      wDirection = SQL_FETCH_NEXT;         
   }
   
   if (SQLDataSources(henv, wDirection, szDSN, sizeof(szDSN), &wLenDSN, sDescription, sizeof(sDescription),
                  &wLenDesc) == SQL_SUCCESS)
   {
       sDSN = szDSN;
       return TRUE;
   } else
   {                   
      SQLFreeEnv(henv);
      return FALSE;
   };
}

///////////////////////////////////////////////////////////////////////////////

BOOL BDCreateDatasource(LPCSTR sDSN, LPCSTR sPath, LPCSTR sUser, LPCSTR sPassword, LPCSTR sHost)
{
  // TODO 
   ASSERT(FALSE);
   return FALSE;
}

///////////////////////////////////////////////////////////////////////////////

BOOL BDExecRetSQL(BDHANDLE hConnect, LPSTR sSQL, LPLONG plRet)
{
   BOOL bOK = TRUE;
   long lRet = 0;   
   SDWORD cbCount;   
   RETCODE nRetCode;
   
   CNRDBase* pBEMDBase = GetNRDBase(hConnect); 
   if (pBEMDBase == NULL)
   { 
      bOK = FALSE;
   };
  
   if (bOK)
   {              

      HSTMT hstmt;
      if (SQLAllocStmt(pBEMDBase->m_hdbc, &hstmt) == SQL_SUCCESS)
      {         
      
         if (::SQLExecDirect(hstmt, (UCHAR *)sSQL, strlen(sSQL)) != SQL_SUCCESS)
         { 
            bOK = FALSE;
         };
         
         if (bOK && SQLBindCol(hstmt, 1, SQL_C_LONG, &lRet, sizeof(long),  &cbCount) != SQL_SUCCESS)
         { 
            bOK = FALSE;
         };
             
         if (bOK)
         {
            bOK = SQLFetch(hstmt) == SQL_SUCCESS;        
         };
             
         AFX_SQL_SYNC(SQLFreeStmt(hstmt,SQL_DROP));         
      };      
   };   
   
  if (bOK)
  { 
     *plRet = lRet;
  };
  
  return bOK;
};

#endif


