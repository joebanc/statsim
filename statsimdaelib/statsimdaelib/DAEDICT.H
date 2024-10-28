#pragma once

#ifndef DAESTREAM
	#include "daestream.h"
#endif

//Define StatSimDct
#ifndef DAEDICT
	#define DAEDICT

class __declspec(dllexport) daedict
{
public:
	daedict(varvec2d* pArray);		//this is currently used
	daedict(daestream *pStream);		
	virtual ~daedict();

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
	void Construct(daestream *pSSS); //construct vars
};

#endif
///////////////////
