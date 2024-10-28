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

#ifndef _XMLFILE_H_
#define _XMLFILE_H_

#include "cdate.h"

///////////////////////////////////////////////////////////////////////////////

class CXMLAttr
{
public:
   CString m_sName;
   CString m_sValue;
};

///////////////////////////////////////////////////////////////////////////////

class CXMLObj
{
public:

   CXMLObj();
   ~CXMLObj();
   BOOL Write(FILE *pFile);
   BOOL Read(FILE *pFile, char ch = 0);

   BOOL ReadAttr(FILE* pFile);

   CString GetAttr(LPCSTR sName);
   void SetAttr(LPCSTR sName, LPCSTR sValue);
   void SetAttr(LPCSTR sName, int n);
   int GetInteger(LPCSTR sTag, LPCSTR sTagChild);
   double GetDouble(LPCSTR sTag, LPCSTR sTagChild);
   CString GetString(LPCSTR sTag, LPCSTR sTagChild);
   CDateTime GetDate(LPCSTR sTag, LPCSTR sTagChild);
   LOGFONT GetLogfont(LPCSTR sTag, LPCSTR sTagChild);

   void SetValue(LPCSTR sTag, LPCSTR sTagChild, LPCSTR s);
   void SetValue(LPCSTR sTag, LPCSTR sTagChild, int n);
   void SetValue(LPCSTR sTag, LPCSTR sTagChild, double d);
   void SetValue(LPCSTR sTag, LPCSTR sTagChild, CDateTime date);
   void SetValue(LPCSTR sTag, LPCSTR sTagChild, LOGFONT& logfont);

   BOOL GetFirst(LPCSTR sTag, LPCSTR sTagChild, CXMLObj*&);
   BOOL GetNext(LPCSTR sTag, LPCSTR sTagChild, CXMLObj*&);

   CXMLObj* GetXMLObj(LPCSTR sTag, LPCSTR sTagChild = NULL, BOOL bRequired = FALSE);

   BOOL GetChild(LPCSTR sName, CXMLObj*&);
   CXMLObj* GetChild() {return m_pChild;}
   CXMLObj* GetNext() {return m_pNext;}

   void SetName(LPCSTR s) {ASSERT(m_sName == ""); m_sName = s;}

   CXMLObj& operator=(CXMLObj& rSrc);

   void AddChild(CXMLObj*);
   CXMLObj* AddChild(LPCSTR sName);
   CXMLObj* AddNext(LPCSTR sName);

   CString m_sName;
   CString m_sValue;

protected:
   CXMLObj *m_pNext;
   CXMLObj *m_pChild;
   CArray <CXMLAttr, CXMLAttr> m_aAttr; 
};

///////////////////////////////////////////////////////////////////////////////

class CXMLFile : public CXMLObj
{
public:

   BOOL Write(LPCSTR sFile);
   BOOL Read(LPCSTR sFile);
};




#endif