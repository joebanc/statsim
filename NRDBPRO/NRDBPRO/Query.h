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

#ifndef _QUERY_H_
#define _QUERY_H_

#include "cdate.h"
#include "longarray.h"
#include "xmlfile.h"

class CQueryFeature;
class CArrayAttrSel;
class CQueryJoin;
class CQueryResultArray;

///////////////////////////////////////////////////////////////////////////////
//
// A condition applied to a query element (e.g. > 5)
//

class CQueryCond
{
public:
   CQueryCond() {m_nCond = 0; m_dCondValue = 0;}

   int GetCond() {return m_nCond;}
   void SetCond(int nCond) {m_nCond = nCond;}

   double GetCondValue() {return m_dCondValue;}
   void SetCondValue(double d) {m_dCondValue = d;}

   LPCSTR GetCondValueStr() {return m_sCondValue;}
   void SetCondValueStr(LPCSTR s) {m_sCondValue = s;}


protected:
   int m_nCond;
   double m_dCondValue;
   CString m_sCondValue;
};

///////////////////////////////////////////////////////////////////////////////

class CQueryElement 
{
public:
   CQueryElement();
   ~CQueryElement();
   CQueryElement(CQueryElement&);
   CQueryElement& operator=(CQueryElement&);

   long GetFTypeId() {return m_lFType;}
   void SetFTypeId(long l) {m_lFType = l;}

   int GetAttrId() {return m_nAttrId;}
   void SetAttrId(int n) {m_nAttrId = n;}

   void SetDataType(int n) {m_nDataType = n;}
   int GetDataType() {return m_nDataType;}
   
   void SetDesc(LPCSTR s) {m_sDesc = s;}
   LPCSTR GetDesc() {return m_sDesc;}

   void SetColName(LPCSTR s) {m_sColName = s;}
   LPCSTR GetColName() {return m_sColName;}
   
   void SetSelected(BOOL b = TRUE) {m_bSelected = b;}
   BOOL GetSelected() {return m_bSelected;}

   int GetGroupStat() {return m_nStatGroup;}
   void SetGroupStat(int n) {m_nStatGroup = n;}

   int GetGroupBy() {return m_nGroupBy;}
   void SetGroupBy(int n) {m_nGroupBy = n;}

   int GetSortBy() {return m_nSortBy;}
   void SetSortBy(int n) {m_nSortBy = n;}

   CQueryElement* GetNextQuery() {return m_pNext;}
   void SetNextQuery(CQueryElement* p) {m_pNext = p;}

   CQueryJoin* GetQueryJoin() {return m_pQueryJoin;}
   void SetQueryJoin(CQueryJoin* p) {m_pQueryJoin = p;}

   CArray <CQueryCond,CQueryCond>& GetConditions() {return m_aCondition;}
   
   static void Delete(CQueryElement*&);

   //virtual void Write(FILE* pFile);
   virtual BOOL Read(FILE* pFile);

   virtual void AsXML(CXMLObj*);
   virtual BOOL XMLAs(CXMLObj*);


   enum {none = 0, count, total, mean, min, max, countunique};      
   enum {greater = 1,  less, greaterequal, lessequal, equal, notequal, contains, notcontains, like};
   enum {no = 0, yes = 1, month = 2,  year = 3};   

protected:
     
   int m_nAttrId;
   int m_nDataType;
   long m_lFType; 
   int m_nStatGroup;
   int m_nGroupBy;   
   BOOL m_nSortBy;
   BOOL m_bSelected; // -1 to indicate required but not displayed
   CString m_sDesc;
   CString m_sColName;
   CArray <CQueryCond, CQueryCond> m_aCondition;
   
   CQueryElement* m_pNext;
   CQueryJoin *m_pQueryJoin;
};

///////////////////////////////////////////////////////////////////////////////

struct CQueryDate
{      
   int m_nID;
   char* m_psDescription;   
};

///////////////////////////////////////////////////////////////////////////////

class CQuery : public CQueryElement
{
public:
   CQuery();
   ~CQuery();
   CQuery(CQuery&);
   CQuery(long lFType, CLongArray& alAttr, CLongArray& alFeatures, int nSort = BDFEATURE);
   CQuery(long lFType, CArrayAttrSel& aAttr, CLongArray& alFeatures, int nSort = BDFEATURE);
   CQuery& operator=(CQuery&);
   
   enum {FirstDate, LastDate, FirstDateRange, LastDateRange, AllDatesRange, AllDatesAfter, 
         AllDatesBefore, AllDates};
   static CQueryDate m_aQueryDates[];   
 
   CString DateCondAsString();   

   long GetFType() {return m_lFType;}
   int GetDateCond() {return m_nDateCond;}
   CDateTime GetStartDate() {return m_dtStart;}
   CDateTime GetEndDate() {return m_dtEnd;}
   BOOL IsShowAllFeatures() {return m_bShowAllFeatures;}
   
   void SetStartDate(CDateTime dt) {m_dtStart = dt;}
   void SetEndDate(CDateTime dt) {m_dtEnd = dt;}
   void SetDateCond(int i) {m_nDateCond = i;}
   void SetShowAllFeatures(BOOL b) {m_bShowAllFeatures = b;}

   /*long GetFType() {return m_xmlobj.GetInteger("Query","FType");}
   int GetDateCond() {return m_xmlobj.GetInteger("Query","DateCond");}
   CDateTime GetStartDate() {return m_xmlobj.GetDate("Query","StartDate");}
   CDateTime GetEndDate() {return m_xmlobj.GetDate("Query","EndDate");}
   BOOL IsShowAllFeatures() {return m_xmlobj.GetInteger("Query","ShowAllFeatures");}
   
   void SetFType(long lFType) {m_xmlobj.SetValue("Query","FType",lFType);}
   void SetStartDate(CDateTime dt) {m_xmlobj.SetValue("Query","StartDate",dt);}
   void SetEndDate(CDateTime dt) {m_xmlobj.SetValue("Query","EndDate",dt);}
   void SetDateCond(int i) {m_xmlobj.SetValue("Query","DateCond",i);}
   void SetShowAllFeatures(BOOL b) {m_xmlobj.SetValue("Query","ShowAllFeatures",b);}*/
     
   void Initialise(long lFType);

   CString GetQueryName();
   
   //virtual void Write(FILE* pFile); 
   virtual BOOL Read(FILE* pFile);

   virtual void AsXML(CXMLObj*);
   virtual BOOL XMLAs(CXMLObj*);


   static CString StripBrackets(CString);

protected:

   void Initialise(long lFType, BOOL& bInitFeatures, BOOL& bInitDates);   

   long m_lFType;
   int m_nDateCond;   
   BOOL m_bShowAllFeatures;
   CDateTime m_dtStart, m_dtEnd;
};

///////////////////////////////////////////////////////////////////////////////

class CQueryLink : public CQueryElement
{
public:
   CQueryLink();
   CQueryLink(long lFType);
   CQueryLink(CQueryLink&);

   //virtual void Write(FILE* pFile); 
   virtual BOOL Read(FILE* pFile);

   virtual void AsXML(CXMLObj*);
   virtual BOOL XMLAs(CXMLObj*);

   void Sort(int left=-1, int right=-1);
   int Find(long lId);

   
   CArray <CQueryFeature, CQueryFeature> m_aFeatures;
};

///////////////////////////////////////////////////////////////////////////////

class CQueryNumber : public CQueryElement
{   
};

///////////////////////////////////////////////////////////////////////////////

class CQueryBoolean : public CQueryLink
{  
public:
   enum {no=0,yes=1};
   
   CQueryBoolean();
};

///////////////////////////////////////////////////////////////////////////////

class CQueryCalc : public CQueryElement
{
public:
   enum {none=0, subtract, divide, multiply, percent, area, length, add};

   CQueryCalc();
   CQueryCalc(CQueryCalc&);

   /*int GetAttrId2() {return m_xmlobj.GetInteger("QueryElement","AttrId2");}
   long GetFTypeId2() {return m_xmlobj.GetInteger("QueryElement","FTypeId2");}
   void SetAttrId2(int n) {m_xmlobj.SetValue("QueryElement","AttrId2",n);}
   void SetFTypeId2(int l) {m_xmlobj.SetValue("QueryElement","FTypeId2",l);}
   long GetQualifier() {return m_xmlobj.GetInteger("QueryElement","Qualifier");}
   void SetQualifier(long n) {m_xmlobj.SetValue("QueryElement","Qualifier",n);}
   long GetQualifier2() {return m_xmlobj.GetInteger("QueryElement","Qualifier2");}
   void SetQualifier2(long n) {m_xmlobj.SetValue("QueryElement","Qualifier2",n);}
   void SetValue1(double d) {m_xmlobj.SetValue("QueryElement","Value1",d);};
   double GetValue1() {return m_xmlobj.GetDouble("QueryElement","Value1");}
   void SetValue2(double d) {m_xmlobj.SetValue("QueryElement","Value2",d);};
   double GetValue2() {ASSERT(GetAttrId2() == 0); return m_xmlobj.GetDouble("QueryElement","Value2");}
   int GetStatistic() {return m_xmlobj.GetInteger("QueryElement","Statistic");}
   void SetStatistic(int n) {m_xmlobj.SetValue("QueryElement","Statistic",n);}
   void SetDecPlaces(int n) {m_xmlobj.SetValue("QueryElement","DecPlaces",n);}
   int GetDecPlaces() {return m_xmlobj.GetInteger("QueryElement","DecPlaces");}*/

   int GetAttrId2() {return m_nAttrId2;}
   long GetFTypeId2() {return m_lFType2;}
   void SetAttrId2(int n) {m_nAttrId2 = n;}
   void SetFTypeId2(int l) {m_lFType2 = l;}
   long GetQualifier() {return m_lQualifier;}
   void SetQualifier(long n) {m_lQualifier = n;}
   long GetQualifier2() {return m_lQualifier2;}
   void SetQualifier2(long n) {m_lQualifier2 = n;}
   void SetValue1(double d) {m_dValue1 = d;};
   double GetValue1() {ASSERT(GetAttrId() == 0); return m_dValue1;}
   void SetValue2(double d) {m_dValue2 = d;};
   double GetValue2() {ASSERT(m_nAttrId2 == 0); return m_dValue2;}
   int GetStatistic() {return m_nStatistic;}
   void SetStatistic(int n) {m_nStatistic = n;}
   void SetDecPlaces(int n) {m_nDecPlaces = n;}
   int GetDecPlaces() {return m_nDecPlaces;}

   //virtual void Write(FILE* pFile); 
   virtual BOOL Read(FILE* pFile);

   virtual void AsXML(CXMLObj*);
   virtual BOOL XMLAs(CXMLObj*);


protected:
   int m_nAttrId2; // Zero if using m_dValue
   long m_lFType2;          
   long m_lQualifier; // Used to indicate area or length
   long m_lQualifier2; // Used to indicate area or length
   int m_nStatistic;   
   int m_nDecPlaces;
   double m_dValue1;
   double m_dValue2;
};

///////////////////////////////////////////////////////////////////////////////
//
// Defines a spatial or non-spatial join between two attributes in different
// CQuery objects.  The first CQueryElement containing the joined attribute
// points to the CQueryJoin object.
//
// The CQueryJoin object defines the feature, attribute and condition that
// is joined to and points to the CQuery object for that feature
//

class CQueryJoin : public CQueryElement
{
public:
   enum {inside=1,intersects=2,within=4,centroidinside=8,equal=16,notequal=32};
   enum {inclusive=0,exclusive=256};

   static void Delete(CQueryJoin*&);

   CQueryJoin(CQueryJoin& rSrc) : CQueryElement((CQueryElement&)rSrc) {};
   CQueryJoin() {};

   virtual void AsXML(CXMLObj*);
   virtual BOOL XMLAs(CXMLObj*);

};

///////////////////////////////////////////////////////////////////////////////

class CQueryFeature : public CFeature
{
public:

   CQueryFeature();
   CQueryFeature(CFeature f) : CFeature(f) {};

   void SetSelected(BOOL b = TRUE) {m_bSelected = b;}
   BOOL GetSelected() {return m_bSelected;}

   BOOL operator>=(CQueryFeature &a) {return m_lId >= a.m_lId;}
   BOOL operator<=(CQueryFeature &a) {return m_lId <= a.m_lId;}

protected:

   BOOL m_bSelected;
};

///////////////////////////////////////////////////////////////////////////////

class CQueryAttrArray : public CAttrArray
{
friend class CQueryResult;

public:
   CQueryAttrArray(LPCSTR sFeature);
   CQueryAttrArray(CQueryAttrArray&);
   ~CQueryAttrArray();
   
   LPCSTR GetFName() {return m_sFeature;}
   void SetFName(LPCSTR s) {m_sFeature = s;}
   BOOL GetCondMet() {return m_bCondMet;}  
   void SetCondMet(BOOL b = TRUE) {m_bCondMet = b;}
   long GetActiveFeature() {return m_lFeatureActive;}   

protected:
   CString m_sFeature;
   CString m_sFeatureActive;
   BOOL m_bCondMet;
   long m_lFeatureActive;
   long m_lParentFeatureActive;
   CDateTime m_dateActive;
};

///////////////////////////////////////////////////////////////////////////////

class CQueryAttrSel
{
public:

   CQueryAttrSel();

   long m_lAttr;
   long m_lFType;
   long m_lDataType;
   int m_nStatGroup;
   int m_nGroupBy;
   BOOL m_nSortBy;

friend CQueryResult;

protected:
};

///////////////////////////////////////////////////////////////////////////////

class  CArrayAttrSel : public CArray <CQueryAttrSel,CQueryAttrSel>
{
public:
    CString GetAttrDesc(CAttrArray& aAttr);    
};



///////////////////////////////////////////////////////////////////////////////

class CQueryResult : public CArray <CQueryAttrArray*, CQueryAttrArray*>
{
public:
      
   CQueryResult();
   ~CQueryResult();

   BOOL Initialise(CQuery*); 
   CArrayAttrSel& GetAttrArray() {return m_aAttrSel;}
   void RemoveAllX();   
   CQueryResult* GetQueryResult() {return m_pQueryResult;}

   static int Compare(CAttribute*, CAttribute*);
   
protected:

   enum {Keep, Replace, New};

   CArrayAttrSel m_aAttrSel;
   CQueryResult* m_pQueryResult; // Result prior to stats
   CQuery* m_pQuery;
   BOOL m_bBaseFType;   
   int m_nGroupBy;
  
   BOOL InitFeatures(CQuery*);
   BOOL RetrieveData(CQuery*);
   BOOL RetrieveJoinData(CQuery*, CQueryResultArray&);
   BOOL JoinQueryResult(CQuery*, CQueryResultArray&);
   BOOL JoinQueryResult(CQueryElement* pQueryAttr, CQueryJoin* pQueryJoin, CQueryResult& queryresult);
   BOOL LoadDataAll(CQuery*, long lFType);
   
   BOOL CheckConditions(CQuery*, CAttrArray&);
   BOOL CheckValueCond(CQuery*, CAttrArray&);
   int UpdateDates(CQuery*, CAttrArray&, CQueryAttrArray* pAttrArray, int i);
   void RetrieveFeatures(CQuery*, CQueryAttrArray*);
   BOOL RetrieveAttr(CQuery*, CAttrArray&, CQueryAttrArray* pAttrArray);
   BOOL RetrieveMissing(CQuery*, CAttrArray&, CQueryAttrArray* pAttrArray);
   void CalcRowBasedStats();
   void CalcRowBasedStats(CQueryCalc* pStats, CQueryAttrArray& aAttr, int iAttr, int nDP);
   BOOL IsFTypeSelected(CQuery*, long lFType);
   int GetDP(CAttribute*);
   CString GetQueryFilter(CQuery*, CAttrArray&);

   double SetDP(double, int nDP);


   enum {no, yes, ftype};

  
   void SortQueryResults();
   void SortQueryResults(int left, int right);
   int Compare(CQueryAttrArray*, CQueryAttrArray*, int nSortId = 1);   

   void CalculateStatistics();
   void CalculateStatistics(CQueryResult& aGroupAttr, CQueryAttrSel& attrselgroup,
                            CQueryAttrSel& attrselstat);
   void DetermineGroup(CQueryResult&, CQueryAttrArray*& paAttrG, CAttribute* pAttrG, 
                       CAttribute* pAttrS, int nStat);
   void UpdateStatistics(CQueryAttrArray*& paAttrG, CAttribute* pAttr, int nStat);   

   void InitNullAttr(CQuery*, CAttrArray& aAttr, CQueryAttrArray* pAttrArray);

   void SortFeature(int start, int end);
   int FindSorted(long lFeature);

  
};

///////////////////////////////////////////////////////////////////////////////

class CQueryResultArray : public CArray <CQueryResult*, CQueryResult*>
{
public:
   ~CQueryResultArray() { RemoveAll();};
   void RemoveAll() {int i = 0; for (i = 0; i < GetSize(); i++) {delete GetAt(i);}; CArray<CQueryResult*, CQueryResult*>::RemoveAll();};
};

#endif
