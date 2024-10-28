#pragma once

#ifndef DAEDICT
	#include "daedict.h"
#endif

#ifndef DAESTREAM
	#include "daestream.h"
#endif

#ifndef DAEVECTOR
	#include "daevector.h"
#endif

//Define StatSimDta
#ifndef DAEDATA
	#define DAEDATA

class __declspec(dllexport) daedata
{
public:
	daedata(varvec3d* pValueArray, ULONG* nPerType, ULONG totLines, 
		daedict **pSSDct, 
		LPCSTR* pType = NULL, USHORT nTypes = 1);		//generalized and default
	daedata(daestream *pSSS, daedict **pSSDct, 
		LPCSTR* pType = NULL, USHORT nTypes = 1);		//generalized and default
	daedata(LPCSTR **pArray);		//disabled temporarily
	virtual ~daedata();

	void Construct(varvec3d* pValueArray, daedict **pSSDct, LPCSTR* pType, USHORT nTypes); //construct matrix

	void Construct(daestream *pSSS, daedict **pSSDct, LPCSTR* pType, USHORT nTypes); //construct matrix
	
	//void Construct(CStatSimRS *pSSRS, daedict **pSSDct, LPCSTR* pType, USHORT nTypes); //construct matrix
	
	std::string* SQLItemValue(USHORT type, ULONG row, ULONG col, LPCSTR strEnclose = NULL, 
		BOOL bsReplace = TRUE, BOOL eReplace = TRUE);

	LPCSTR GetValue(USHORT type, ULONG row, ULONG col); //[type][row][column]
	
	const ULONG nRTYPE(USHORT RTi);
	
	daedict **pDict;  //this is needed to be accessed - do not destruct
	ULONG* rowSize;
	UINT* colSize;
	ULONG nLines;
	USHORT m_nTYPES;

	void SetArray(varvec3d* srcArray);

	ULONG maxSize(ULONG* nSize, USHORT n);

	//Get progress attributes
	float GetCurPercent()
	{
		return m_nPercent;
	}

private:

	//attributes
	varvec3d* pValueMatrix;	//[rtype k][column j][row i]

	ULONG* pCountType;	//array of record types

	//LPCSTR m_sProgress;
	float m_nPercent;

	//methods
	void PutValue(USHORT type, ULONG row, ULONG col, LPCSTR entry);

	void TransferValues(daestream *pSSS, daedict **pSSDct, LPCSTR* pType, USHORT nTypes); //transfer values to matrix
	
	//void TransferValues(CStatSimRS *pSSRS, daedict **pSSDct, LPCSTR* pType, USHORT nTypes); //transfer values to matrix

	void InitmPtrs();
	
};

#endif
//////////////////
