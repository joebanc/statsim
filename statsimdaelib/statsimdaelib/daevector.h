#pragma once
#ifndef _VECTOR_
	#include <vector>
#endif

//double *array = &(*vector.begin());

//DLLEXPORT has problems!!

//Define daevector
#ifndef DAEVECTOR
	#define DAEVECTOR

class __declspec(dllexport) daevariant 
{
public:
	daevariant(void);
	~daevariant(void);

	// various constructor flavours
	daevariant(const int& intVal);
	daevariant(const long& longVal);
	daevariant(const float& floatVal);
	daevariant(const std::string& strVal);
	daevariant(const LPCSTR strVal);
	daevariant(const daevariant& dVal);

	// various assignment operator flavours
	daevariant& operator=(const int& intVal);
	daevariant& operator=(const long& longVal);
	daevariant& operator=(const float& floatVal);
	daevariant& operator=(const std::string& strVal);
	daevariant& operator=(const LPCSTR strVal);
	daevariant& operator=(const daevariant& dVal);

	// various returns
	operator int();
	operator long();
	operator float();
	operator std::string();
	operator LPCSTR();

private:
	 enum Type{
            UNKOWN=0,
            INT,
            LONG,
            FLOAT,
            STRING
        };
        struct{
            Type type;
            union 
            {
                int iVal;
                long lVal;
                float fVal;
                std::string* sVal;
            };
        } data;
};

class daegenvec
{
public:
	virtual void resize(size_t _newsize);
	virtual void push_back(void);

};


template<class T> class daevector : public daegenvec, private std::vector<T>
{
public:
	virtual void resize(size_t _newsize);
	virtual void resize(size_t _newsize, T _newval);
	virtual void push_back(T& _newval);

};

class __declspec(dllexport) tstrvec1d : public std::vector<LPCSTR> 
{
private:
	LPCSTR* m_pArray;
public:
	
	tstrvec1d();
	~tstrvec1d();
	operator LPCSTR*();
};

class __declspec(dllexport) varvec2d : public std::vector< std::vector<daevariant> >
{
private:
	LPCSTR** m_pStrArray;
	int** m_pIntArray;
	long** m_pLongArray;
	float** m_pFloatArray;
public:
	
	varvec2d(void);
	~varvec2d(void);

	// return operators
	operator LPCSTR**();
};

typedef std::vector<int> intvec1d;
typedef std::vector< intvec1d > intvec2d;
//typedef std::vector<const char*> tstrvec1d;
typedef std::vector< std::vector<const TCHAR*> > tstrvec2d;
typedef std::vector< std::vector< std::vector<const TCHAR*> > > tstrvec3d;
typedef std::vector< daegenvec* > pdgv1d;
typedef std::vector< daegenvec > dgv1d;
typedef std::vector< std::vector<daegenvec*> > pdgv2d;
typedef std::vector< std::vector<daegenvec> > dgv2d;
typedef std::vector< std::vector<void*> > pvoidvec2d;
typedef std::vector< std::vector<void> > voidvec2d;

typedef std::vector<std::wstring> stlwstrvec1d;
typedef std::vector<stlwstrvec1d> stlwstrvec2d;

typedef std::vector<std::string> stlstrvec1d;
typedef std::vector<stlstrvec1d> stlstrvec2d;

typedef std::vector< daevariant> varvec1d;
//typedef std::vector< std::vector<daevariant> > varvec2d;
typedef std::vector< std::vector< std::vector<daevariant> > > varvec3d;

typedef std::vector< daevariant*> varpvec1d;
typedef std::vector< std::vector<daevariant*> > varpvec2d;
typedef std::vector< std::vector< std::vector<daevariant*> > > varpvec3d;

int __declspec(dllexport) destroyvec( varvec3d values, bool wptr = false );
int __declspec(dllexport) destroyvec( varvec2d values, bool wptr = false );
int __declspec(dllexport) destroyvec( varvec1d values, bool wptr = false );

int __declspec(dllexport) destroyvec( varpvec3d values, bool wptr = false );

//int DLLEXPORT destroyvec( varvec2d );
//int DLLEXPORT destroyvec( varvec1d );

#endif
///////////////////
