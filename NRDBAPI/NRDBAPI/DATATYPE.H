// datatype.h: interface for the datatype class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATATYPE_H__F8245CA6_C9F4_4EAE_9E54_4AD59C8C2482__INCLUDED_)
#define AFX_DATATYPE_H__F8245CA6_C9F4_4EAE_9E54_4AD59C8C2482__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////////////////


#ifndef NRDBMYSQL
#define CNRDBString CString
#else

class DLLEXPORT CLongBinary : public CObject
{
	DECLARE_DYNAMIC(CLongBinary)

// Constructors
public:
	CLongBinary();
   CLongBinary(CLongBinary&);

// Attributes
	HGLOBAL m_hData;
	DWORD m_dwDataLength;

// Implementation
public:
	virtual ~CLongBinary();

};

///////////////////////////////////////////////////////////////////////////////

class DLLEXPORT CNRDBString
{
public:
   CNRDBString();
   ~CNRDBString();
   
   CNRDBString& operator=(CNRDBString);
   CNRDBString& operator=(LPCSTR);
   operator LPCSTR() const;
   void SetLength(long lLength);

protected:
   char *m_pData;
   long m_nSize;
};

#endif

#endif // !defined(AFX_DATATYPE_H__F8245CA6_C9F4_4EAE_9E54_4AD59C8C2482__INCLUDED_)
