// datatype.cpp: implementation of the datatype class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "datatype.h"

#ifdef NRDBMYSQL

IMPLEMENT_DYNAMIC(CLongBinary, CObject)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLongBinary::CLongBinary()
{
   m_hData = NULL;
   m_dwDataLength = 0;
}

CLongBinary::CLongBinary(CLongBinary& rSrc)
{
   m_hData = rSrc.m_hData;
   m_dwDataLength = rSrc.m_dwDataLength;
}

CLongBinary::~CLongBinary()
{
   if (m_hData != NULL)
   {
      GlobalFree(m_hData);
   }
}

//////////////////////////////////////////////////////////////////////

CNRDBString::CNRDBString()
{
   m_pData = new char[1];
   strcpy(m_pData, "\0");
   m_nSize = 1;
}

CNRDBString::~CNRDBString()
{
   if (m_pData != NULL) delete [] m_pData;
}

CNRDBString& CNRDBString::operator=(CNRDBString src)
{
   if (m_nSize != src.m_nSize)
   {
      if (m_pData != NULL)
      {
         delete [] m_pData;
         m_pData = NULL;
      };
      if (src.m_nSize > 0)
      {
         m_pData = new char[src.m_nSize];
      };
      m_nSize = src.m_nSize;
   }
   if (src.m_nSize > 0)
   {
      strncpy(m_pData, src.m_pData, src.m_nSize);
   }
   return *this;
}

CNRDBString& CNRDBString::operator=(LPCSTR sz)
{
   // Do not reallocate length as address must remain fixed

   int len = strlen(sz);
   if (m_pData != NULL)
   {
      strncpy(m_pData, sz, m_nSize);
   };
   return *this;
}

CNRDBString::operator LPCSTR() const
{
   return m_pData;
}

void CNRDBString::SetLength(long lLength)
{
   if (lLength != m_nSize)
   {
      if (m_pData != NULL) delete m_pData;
      m_pData = NULL;
   }
   if (lLength > 0)
   {
      m_pData = new char[lLength];
   };
   m_nSize = lLength;
}

#endif
