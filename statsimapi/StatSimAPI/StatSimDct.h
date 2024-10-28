#ifndef DAESTREAM
	#include "daestream.h"
#endif

#ifndef STATSIMRS_H
	#include "StatSimRS.h"
#endif

//Define StatSimDct
#ifndef STATSIMDCT_H
	#define STATSIMDCT_H
#endif
///////////////////

#pragma once

class __declspec(dllexport) CStatSimDct
{
public:
	CStatSimDct(CStatSimRS *pSSRS);		//this is currently used
	CStatSimDct(daestream *pSSS);		
	virtual ~CStatSimDct();

	//methods
	LPCSTR GetVarName(ULONG index);
	LPCSTR GetVarType(ULONG index);
	LPCSTR GetVarLen(ULONG index);
	LPCSTR GetVarPrec(ULONG index);

	LPCSTR GetVarUnsigned(ULONG index);
	LPCSTR GetVarZerofill(ULONG index);

	LPCSTR GetVarLabel(ULONG index);
	ULONG GetVarBytes(ULONG index);
	ULONG nVars();
	LPCSTR** GetArray();
	

private:

	//attributes

	ULONG matsize;	//number of variables
	USHORT nAttr;	//number of attributes
	//CStringArray var, type, width, dec, label;	//contains varname, type and length (& decimals)
	LPCSTR** dctArray;	//contains varname, type, length (& decimals), and [labels]
	
	//methods
	USHORT SQLTypeDct(LPCSTR strType);
	void Construct(CStatSimRS *pSSRS); //construct vars
	void Construct(daestream *pSSS); //construct vars
};
