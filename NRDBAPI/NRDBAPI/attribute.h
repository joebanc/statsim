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

#ifndef _ATTRIBUTE_H_
#define _ATTRIBUTE_H_

#include "coord.h"
#include "datatype.h"

// Maximum string lengths

#define BD_SHORTSTR 255
#define BD_NAMESTR 255 

// Data types

#define BDNUMBER 1
#define BDTEXT 2
#define BDCOORD 3
#define BDMAPLINES 4
#define BDLINK 7
#define BDBOOLEAN 8
#define BDHOTLINK 9
#define BDDICTIONARY 10
#define BDIMAGE 11
#define BDLONGTEXT 12
#define BDFILE 13

#define BDFTYPE -1
#define BDFEATURE -2
#define BDPARENTFEATURE -3
#define BDDATE -4
#define BDGRANDPARENTFEATURE -5
#define BDQUERYSTATS -6
#define BDGREATGRANDPARENTFEATURE -7
#define BDID -8
#define BDSYMBOL -9
#define BDAREA -10
#define BDLENGTH -11
#define BDJOIN -12





///////////////////////////////////////////////////////////////////////////////

class CNRDBase;

///////////////////////////////////////////////////////////////////////////////
//
// Class contains a single attribute associated with a feature.  E.g. the rainfall
// on a particular day or a map of a catchment
//

class DLLEXPORT CFTypeAttr
{
public:

   CFTypeAttr();
   CFTypeAttr(CFTypeAttr&);
   CFTypeAttr& operator=(CFTypeAttr&);

   CString m_sColName;
   CString m_sDesc;
   long m_lDataType;
   long m_lFTypeLink;
   long m_lAttrId;
   long m_lFType;
   BOOL m_bPrimaryKey;
   
   long GetDataType() {return m_lDataType;}
   long GetFTypeLink() {ASSERT(m_lDataType == BDLINK); return m_lFTypeLink;}
   long GetAttrId() {return m_lAttrId;}
   long GetFTypeId() {return m_lFType;}   
   BOOL IsPrimaryKey() {return m_bPrimaryKey;}

   CString GetDesc();

   void SetDesc(LPCSTR sDesc) {m_sDesc = sDesc;}   
   void SetAttrId(long lAttrId) {m_lAttrId = lAttrId;}
   void SetFTypeId(long lFType) {m_lFType = lFType;}
   void SetFTypeLink(long lFTypeLink) {ASSERT(m_lDataType == BDLINK); m_lFTypeLink = lFTypeLink;}
};

///////////////////////////////////////////////////////////////////////////////

class DLLEXPORT CAttribute : public CFTypeAttr
{
public:

   CAttribute(long lDataType);
   CAttribute(CFTypeAttr&);
   CAttribute(CAttribute&);
   ~CAttribute();

   void AllocInt();
   void FreeInt();

   void Init();   

   double* GetDouble() {ASSERT(m_lDataType == BDNUMBER); return (double*)m_pData;}
   char* GetString() {ASSERT(m_lDataType == BDTEXT); return (char*)m_pData;}
   CLongBinary* GetLongBinary() {ASSERT(m_lDataType == BDMAPLINES || m_lDataType == BDIMAGE || m_lDataType == BDFILE); return (CLongBinary*)m_pData;}   
   long* GetLink() {ASSERT(m_lDataType == BDLINK); return (long*)m_pData;}
   CCoord* GetCoord() {ASSERT(m_lDataType == BDCOORD); return (CCoord*)m_pData;}
   BOOL* GetBoolean() {ASSERT(m_lDataType == BDBOOLEAN); return (BOOL*)m_pData;}
   long* GetDate() {ASSERT(m_lDataType == BDDATE); return (long*)m_pData;}
   CString* GetFile() {ASSERT(m_lDataType == BDHOTLINK); return (CString*)m_pData;}
   CString* GetLongText() {ASSERT(m_lDataType == BDLONGTEXT); return (CString*)m_pData;}

   void SetDouble(double dValue) {ASSERT(m_lDataType == BDNUMBER); *(double*)m_pData = dValue;}
   void SetLink(long lValue) {ASSERT(m_lDataType == BDLINK);  *(long*)m_pData = lValue;}
   void SetString(LPCSTR s) {ASSERT(m_lDataType == BDTEXT); strncpy((char*)m_pData, s, BD_SHORTSTR);}
   void SetLongBinary(CLongBinary* p) {ASSERT(m_lDataType == BDMAPLINES || m_lDataType == BDIMAGE || m_lDataType == BDFILE); memcpy((CLongBinary*)m_pData, p, sizeof(CLongBinary));}
   void SetCoord(CCoord coord) {ASSERT(m_lDataType == BDCOORD); *(CCoord*)m_pData = coord;}
   void SetBoolean(BOOL bValue) {ASSERT(m_lDataType == BDBOOLEAN); *(long*)m_pData = bValue;}      
   void SetDate(long lDate) {ASSERT(m_lDataType == BDDATE); *(long*)m_pData = lDate;}      
   void SetFile(LPCSTR s) {ASSERT(m_lDataType == BDHOTLINK); *(CString*)m_pData = s;}
   void SetLongText(LPCSTR s) {ASSERT(m_lDataType == BDLONGTEXT); *(CString*)m_pData = s;}

   BOOL IsNull();
   
   CString AsString();
   BOOL AsAttr(CString sAttr);

   BOOL operator==(CAttribute&);
   BOOL operator!=(CAttribute&);   

   CAttribute& operator=(CAttribute&);

   int GetNumFields();   

// Implementation

 protected:

   double& GetDoubleInt() {ASSERT(m_lDataType == BDNUMBER); return *((double*)m_pDataInt);}
   long& GetLinkInt() {ASSERT(m_lDataType == BDLINK); return *((long*)m_pDataInt);}
   CNRDBString& GetStringInt() {ASSERT(m_lDataType == BDTEXT); return *((CNRDBString*)m_pDataInt);}
   CLongBinary& GetLongBinaryInt() {ASSERT(m_lDataType == BDMAPLINES || m_lDataType == BDIMAGE ||
	   m_lDataType == BDLONGTEXT || m_lDataType == BDFILE); return *((CLongBinary*)m_pDataInt);}   
   CCoord& GetCoordInt() {ASSERT(m_lDataType == BDCOORD); return *((CCoord*)m_pDataInt);}
   BOOL& GetBooleanInt() {ASSERT(m_lDataType == BDBOOLEAN); return *((BOOL*)m_pDataInt);}
   long& GetDateInt() {ASSERT(m_lDataType == BDDATE); return *((long*)m_pDataInt);}
   CNRDBString& GetFileInt() {ASSERT(m_lDataType == BDHOTLINK); return *((CNRDBString*)m_pDataInt);}

   void InitDoubleInt() {ASSERT(m_lDataType == BDNUMBER); *((double*)m_pDataInt) = 0;}
   void InitLinkInt() {ASSERT(m_lDataType == BDLINK); *((long*)m_pDataInt) = 0;}      
   void InitBooleanInt() {ASSERT(m_lDataType == BDBOOLEAN); *((BOOL*)m_pDataInt) = 0;}
   void InitDateInt() {ASSERT(m_lDataType == BDDATE); *((long*)m_pDataInt) = 0;}      

   void CopyLongBinary(CLongBinary& rSrc);   
 
   void* m_pData;  
   void* m_pDataInt;

friend class CFeatureAttrSet;
};

///////////////////////////////////////////////////////////////////////////////
//
// Class contains an array of attributes.  This would be a collection of
// attributes associated with a feature for a particular date
//

class DLLEXPORT CAttrArray : public CArray <CAttribute*, CAttribute*>
{
public:
   CAttrArray();   
   CAttrArray(CAttrArray&);   
   ~CAttrArray();   

   CAttrArray& operator=(CAttrArray&);

   void RemoveAllX();

   long m_lDate;
   long m_lOrder;
   long m_lFType;
   long m_lFeature;
   
   void SetFTypeName(LPCSTR sFType) {m_sName = sFType;}
   LPCSTR GetFTypeName() {return m_sName;}
         
   void SetFTypeInternal(LPCSTR sFType) {m_sInternal = sFType;}

   CString GetFTypeInternal(CNRDBase* pDBase);

   CString GetFTypeTable(CNRDBase* pDBase);

   CString GetFTypeI(CNRDBase* pDBase);



protected:   
   CString m_sInternal;
   CString m_sName;
};

#endif
