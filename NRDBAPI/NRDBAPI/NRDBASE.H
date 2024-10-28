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


#ifndef _NRDBASE_H_
#define _NRDBASE_H_

#include "crecordx.h"

///////////////////////////////////////////////////////////////////////////////

BOOL BDExecRetSQL(BDHANDLE hConnect, LPSTR sSQL, LPLONG plRet);
BOOL BDNextId (BDHANDLE hConnect, LPSTR sTableName, LPLONG plNextId, BOOL bPositive);

///////////////////////////////////////////////////////////////////////////////

#ifdef NRDBMYSQL
class DLLEXPORT CNRDBase : public CMySQLDatabase
#else
class DLLEXPORT CNRDBase : public CDatabase
#endif
{

// Allows type checking at runtime

 DECLARE_DYNAMIC(CNRDBase); 

  CNRDBase();
  ~CNRDBase();

  BOOL SetRecordSet(CRecordsetExt* pSet);  
  BOOL DeleteRecordSet();   
  CRecordsetExt* GetRecordSetExt();    

#ifndef NRDBMYSQL
  BOOL GetDriver(CString& sVersion);  
#endif

  CString m_sError;
  int m_nOnError;
  int  m_nDBVersion;
  int m_nTransLevel;
  BOOL m_bJetDB;
  BOOL m_bInternal;

  int GetDBVersion() {if (m_nDBVersion == AFX_RFX_SHORT_PSEUDO_NULL) return 0;
                      else return m_nDBVersion;}
  BOOL IsJetDB() {return m_bJetDB;}

  BOOL Open(LPCSTR sDSN, LPCSTR sUser = NULL, LPCSTR sPassword = NULL, LPCSTR sHost = NULL);
   
// Database object
  
protected: 

  CRecordsetExt* m_pRecordSet;   
  CRecordsetExt* m_pRecordSetInt;
};

///////////////////////////////////////////////////////////////////////////////
//
// Class used for optimising retrieval of ftype names
//

class DLLEXPORT CFTypeLinks
{
public:
  void Reset();
  CString GetFType(long lFType);

  // Optimisation 

protected:

  void LoadFTypes();
  
  CArray <CFeatureType, CFeatureType> m_aFType;

};

///////////////////////////////////////////////////////////////////////////////
//
// Classes used for optimising retrieval of feature names
//


class DLLEXPORT CFeatureLinks
{
public:
  void Reset();
  CString GetFeature(long lFeature, long lFType);

  // Optimisation 

protected:

  void LoadFeatures(long lFType);

  CArray <long,long> m_alFType;
  CArray <CFeature, CFeature> m_aFeature;

};


///////////////////////////////////////////////////////////////////////////////
//
// Global data structure 
//


//#define MAXCONNECT 32

struct CInternalInfo
{
   
  //CNRDBase* m_pDBase[MAXCONNECT];      // Pointer to array of connect information 

  HWND m_hwnd;
  
  CFeatureLinks m_links;  
  CFTypeLinks m_aFType;
  long m_lFeatureSel; 
  long m_lFTypeSel; 

  int m_nConnections;

  BDHANDLE m_hConnectInt;  // Internal Connection    
      
};

///////////////////////////////////////////////////////////////////////////////

extern CInternalInfo internalinfo;

///////////////////////////////////////////////////////////////////////////////
//
// Functions to access members of CNRDBase class
//

CNRDBase* GetNRDBase(BDHANDLE);

/////////////////////////////////////////////////////////////////////////////

BOOL BDUpdate(CNRDBase*);

/////////////////////////////////////////////////////////////////////////////

#endif
