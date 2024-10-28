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

class CRecordsetExt;
class CMySQLRecordset;
class CNRDBase;

#ifndef _CRECORDX_H_
#define _CRECORDX_H_

#include "nrdb.h"
#include "mysqldbase.h"

///////////////////////////////////////////////////////////////////////////////

#ifdef NRDBMYSQL
class DLLEXPORT CRecordsetExt : public CMySQLRecordset
#else
class DLLEXPORT CRecordsetExt : public CRecordset
#endif
{
public:

   CRecordsetExt(CNRDBase* pDatabase = NULL);
   ~CRecordsetExt();

   virtual BOOL BEMGeneric(BDHANDLE, int iFlag);

// Implementation
protected:
       
   // Member variables

   BOOL m_bBound;   
   BOOL m_bGetLength;   

   void SetVariables() {DoDataExchange(SetVar);}
   virtual CString GetFilter() = 0;
   virtual CString GetSortOrder() {return "";}        
   virtual void DoDataExchange(int iFlag) = 0;

   enum {SetVar, GetVar};
             
 // Indicates that returned values should be copied values pointed to in arguments                          

   virtual CString GetDefaultConnect();   // Default connection string
   virtual CString GetDefaultSQL();    // Default SQL for Recordset
   virtual void DoFieldExchange(CFieldExchange* pFX); // RFX support

   DECLARE_DYNAMIC(CRecordsetExt)
// Field/Param Data
	//{{AFX_FIELD(CRecordsetExt, CRecordset)	
	//}}AFX_FIELD

inline DATA_EXCHANGE_STR(CNRDBString& dbvar, char* psvar, int len, int iFlag)
{
   if (iFlag == SetVar)
   {
      dbvar = psvar;
   }
   else if (iFlag == GetVar && m_bBound)
   {
      strncpy(psvar, dbvar, len-1);
      psvar[len-1] = '\0';
   }
};

///////////////////////////////////////////////////////////////////////////////

inline DATA_EXCHANGE_LONGBIN(CLongBinary& dbvar, CLongBinary* pvar, int iFlag)
{
	if (iFlag == SetVar)
	{
      if (dbvar.m_hData != NULL)
      {
         GlobalFree(dbvar.m_hData);
         dbvar.m_hData = NULL;
         dbvar.m_dwDataLength = 0;
      }
      if (pvar->m_hData != NULL && pvar->m_dwDataLength > 0)
      {
         dbvar.m_hData = GlobalAlloc(GMEM_MOVEABLE|GMEM_DISCARDABLE, pvar->m_dwDataLength);
		 if (dbvar.m_hData != NULL)
		 {
			 dbvar.m_dwDataLength = pvar->m_dwDataLength;
			 BYTE *var = (BYTE*)GlobalLock(pvar->m_hData);
			 BYTE *db = (BYTE*)GlobalLock(dbvar.m_hData);

			 if (var != NULL && db != NULL)
			 {
			    memcpy(db, var, pvar->m_dwDataLength);
			 } else
			 {
				AfxThrowMemoryException();
			 }

			 GlobalUnlock(pvar->m_hData);
			 GlobalUnlock(dbvar.m_hData);
		 } else
		 {
			 AfxThrowMemoryException();
		 }
      }
	  //dbvar.m_hData = pvar->m_hData;
	  //dbvar.m_dwDataLength = pvar->m_dwDataLength;

	}
	else if (iFlag == GetVar && m_bBound)
	{   
     if (pvar->m_hData != NULL)
     {
        GlobalFree(pvar->m_hData);
        pvar->m_hData = NULL;
        pvar->m_dwDataLength = 0;
     };
     if (dbvar.m_hData != NULL && dbvar.m_dwDataLength > 0)
     {
         pvar->m_hData = GlobalAlloc(GMEM_MOVEABLE|GMEM_DISCARDABLE, dbvar.m_dwDataLength);
		 if (pvar->m_hData != NULL)
		 {
			 pvar->m_dwDataLength = dbvar.m_dwDataLength;

			 BYTE *var = (BYTE*)GlobalLock(pvar->m_hData);
			 BYTE *db = (BYTE*)GlobalLock(dbvar.m_hData);

			 if (var != NULL && db != NULL)
			 {
			     memcpy(var, db, dbvar.m_dwDataLength);
			 } else
			 {
				 AfxThrowMemoryException();
			 }
         
			 GlobalUnlock(pvar->m_hData);
			 GlobalUnlock(dbvar.m_hData);
		 } else
		 {
			 AfxThrowMemoryException();
		 }
     };     	  
	}
};

	// Exchanges data between a long binary and a CString

inline void DATA_EXCHANGE_LONGTEXT(CLongBinary& longbin, CString* pString, int iFlag)
{
  // Copy the string to a long binary

	if (iFlag == SetVar)
	{
	  DWORD dwSize = pString->GetLength()+1;
       if (longbin.m_hData == NULL) 
	  {
           longbin.m_hData = GlobalAlloc(GMEM_MOVEABLE|GMEM_DISCARDABLE, dwSize);
	  } else
	  {
		  longbin.m_hData = GlobalReAlloc(longbin.m_hData,dwSize,0);      
	  }		  
	  longbin.m_dwDataLength = dwSize;

	  char* pData = (char*)GlobalLock(longbin.m_hData);
	  if (pData != NULL)
	  {
		  strcpy(pData, *pString);
		  GlobalUnlock(longbin.m_hData);
	  } else
	  {
		  ASSERT(FALSE);
	  }
	}

	// Retrieve the string from a long binary

	else if (iFlag == GetVar && m_bBound)
	{
      if (longbin.m_hData != NULL)
	  {
		  char* pData = (char*)GlobalLock(longbin.m_hData);
		  if (pData != NULL)
		  {
			  *pString = pData;
			   GlobalUnlock(longbin.m_hData);
		  }	else
		  {
			 ASSERT(FALSE);
		  }
	  }
	}
}

};

///////////////////////////////////////////////////////////////////////////////
//
// Macro for creating new recordset derived objects
//


#define IMPLEMENT_CREATERECORDSET(pDBase, pRecordSet, CRecordSetExtClass)                 \
if (pDBase->GetRecordSetExt() != NULL &&                                                  \
          pDBase->GetRecordSetExt()->IsKindOf(RUNTIME_CLASS(CRecordSetExtClass)) == FALSE)\
      {                                                                                   \
         pDBase->DeleteRecordSet();                                                       \
      };                                                                                  \
                                                                                          \
      if (pDBase->GetRecordSetExt() == NULL)                                              \
      {                                                                                   \
         pRecordSet = new CRecordSetExtClass(GetNRDBase(hConnect));                       \
         pDBase->SetRecordSet(pRecordSet);                                                \
      };                                                                                  \
      pRecordSet = (CRecordSetExtClass*)pDBase->GetRecordSetExt();                        \
      ASSERT(pRecordSet->IsKindOf(RUNTIME_CLASS(CRecordSetExtClass)));                    

///////////////////////////////////////////////////////////////////////////////

#define DATA_EXCHANGE(dbvar, pvar, iFlag)\
   if (iFlag == SetVar)\
   {\
      dbvar = *pvar;\
   }\
   else if (iFlag == GetVar && m_bBound)\
   {\
      *pvar = dbvar;\
   }


#endif
