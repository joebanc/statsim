#include "stdafx.h"
#include "daedata.h"
#include "daestring.h"
#include "daeconv.h"
#include "statsimdaelib.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

daedata::daedata(varvec3d* pValueArray, ULONG* nPerType, ULONG totLines, 
						 daedict **pSSDct,  
						 LPCSTR* pType, USHORT nTypes)
{
	InitmPtrs();

	if (nTypes < 1) {
		//catch this
		//AfxMessageBox(_ANSI("There has to be at least one type of record!"));
		return;
	}

	//pass dictionary
	pDict = pSSDct;
	//number of lines from stream
	nLines = totLines;

	//make pointer
	//VERIFY( rowSize = (ULONG *)malloc(nTypes * sizeof(ULONG)) );
	//VERIFY( colSize = (UINT *)malloc(nTypes * sizeof(UINT)) );
	rowSize = new ULONG[nTypes];
	colSize = new UINT[nTypes];

	for (USHORT Ti=0; Ti<nTypes; Ti++) {

		colSize[Ti] = pSSDct[Ti]->nVars();	//get from dictionary
		rowSize[Ti] = nPerType[Ti];	

	}

	Construct(pValueArray, pSSDct, pType, nTypes);

}

daedata::daedata(daestream *pSSS, daedict **pSSDct, 
						 LPCSTR* pType, USHORT nTypes)
{
	InitmPtrs();

	if (nTypes < 1) {
		//catch this!
		//AfxMessageBox(_ANSI("There has to be at least one type of record!"));
		return;
	}

	//pass dictionary
	pDict = pSSDct;

	//number of lines from stream
	nLines = pSSS->TotLines();

	//make pointer
	//VERIFY( rowSize = (ULONG *)malloc(nTypes * sizeof(ULONG)) );
	//VERIFY( colSize = (UINT *)malloc(nTypes * sizeof(UINT)) );
	rowSize = new ULONG[nTypes];
	colSize = new UINT[nTypes];

	for (USHORT Ti=0; Ti<nTypes; Ti++) {

		colSize[Ti] = pSSDct[Ti]->nVars();	//get from dictionary
		rowSize[Ti] = pSSS->nRTYPE(Ti);	

	}
	Construct(pSSS, pSSDct, pType, nTypes);

}

daedata::daedata(LPCSTR** pArray)
{
	//not available
	InitmPtrs();
}

void daedata::InitmPtrs()
{
	
	//pValueMatrix = NULL;
	pCountType = NULL;
	rowSize = NULL;
	colSize = NULL;
}


daedata::~daedata()
{

	//free memory blocks
	//free(*pValueMatrix);
	delete [] pCountType;
	delete [] rowSize;
	delete [] colSize;

	if(_heapmin()<0) {
		//catch this!
		//AfxMessageBox(_ANSI("Cannot give back memory to OS."));
	}

	return;
	//the following is buggy, note that!

	int i, j;
	for (i=0; i<(*pValueMatrix).size(); i++) {
		for (j=0; j<(*pValueMatrix)[i].size(); j++) {
			(*pValueMatrix)[i][j].clear();
		}
		(*pValueMatrix)[i].clear();
	}
	(*pValueMatrix).clear();

}
void daedata::Construct(varvec3d* pValueArray, daedict **pSSDct, 
							LPCSTR* pType, USHORT nTypes)

{
	//initialize
	m_nTYPES = nTypes;
	//VERIFY( pCountType = (ULONG*)malloc(m_nTYPES * sizeof(ULONG)) );
	pCountType = new ULONG[m_nTYPES];
	
	for (USHORT Ti=0; Ti<nTypes; Ti++) {

		pCountType[Ti] = rowSize[Ti];

	}

	pValueMatrix = pValueArray;
}

void daedata::Construct(daestream *pSSS, daedict **pSSDct, 
							LPCSTR* pType, USHORT nTypes)
{
	//initialize
	m_nTYPES = nTypes;
	//VERIFY( pCountType = (ULONG*)malloc(m_nTYPES * sizeof(ULONG)) );
	pCountType = new ULONG[m_nTYPES];
	
	for (USHORT Ti=0; Ti<nTypes; Ti++) {

		pCountType[Ti] = pSSS->nRTYPE(Ti);

	}

	//pValueMatrix = new varvec3d;
	(*pValueMatrix).resize(m_nTYPES);

	for (USHORT k=0; k<nTypes; k++) {
		(*pValueMatrix)[k].resize(colSize[k]);		

		for (UINT j=0; j<colSize[k]; j++) {
			(*pValueMatrix)[k][j].resize(rowSize[k]);	
		}
	}
	
	
	TransferValues(pSSS, pSSDct, pType, nTypes);
}

void daedata::PutValue(USHORT type, ULONG row, ULONG col, LPCSTR entry)
{
	(*pValueMatrix)[type][col][row] = entry;

}

LPCSTR daedata::GetValue(USHORT type, ULONG row, ULONG col)
{
	//MessageBox(0, (*pValueMatrix)[type][col][row], L"putang ina", MB_OK);

	return (*pValueMatrix)[type][col][row];

}

const ULONG daedata::nRTYPE(USHORT RTi)
{

	return pCountType[RTi];

}
ULONG daedata::maxSize(ULONG* nSize, USHORT n)
{

	ULONG MAX = 0;

	for (USHORT i=0; i<n; i++) {
		MAX = __max(nSize[i], MAX);
	}
	
	return MAX;

}

void daedata::TransferValues(daestream *pSSS, daedict **pSSDct, 
								 LPCSTR* pType, USHORT nTypes)
{
	//Record type is initially set to HH and Mems only

	UINT width, dec, nchars, pos;	//indicators
	LPCSTR currVal, finVal;	//handler of current value and final transfer value
	daestring currValTemp;  //handler of temporary current value

	ULONG counter = 0, min = 0, max = nLines;

	ULONG* typCtr = new ULONG[nTypes];

	for (USHORT k=0; k<nTypes; k++) {
		typCtr[k]=0;
	}

	char* crtype = 0;
	USHORT crtIDX;

	for (USHORT i=0; i<nLines; i++) {

		//initial position
		pos = 1;
		
		m_nPercent = ((float) (i+1)/(float) nLines)*100;
		
	
		//fetch rtype (presently in the leftmost) - revise if necessary
		if (pType!=NULL) {

			if (pSSS->GetVal(i+1, rt_pos, 1)!=NULL) {
				crtype = pSSS->GetVal(i+1, rt_pos, pSSS->GetRTLen());
				crtIDX = GetIDX(pType, nTypes, crtype, pSSS->GetRTLen());

			}
			
			else {
				//catch this
				//AfxMessageBox(_ANSI("Cannot get type of record!"));					
				return;
			}

		}

		else {
			crtIDX = 0;
		}
		
		for (USHORT j=0; j<colSize[crtIDX]; j++) {
			
			//reinitialize character number
			nchars = 0;	
			
			//conversion
			width = atoi(pSSDct[crtIDX]->GetVarLen(j));
			dec = atoi(pSSDct[crtIDX]->GetVarPrec(j));
			nchars = width;
			
			//handler of values
			if (pSSS->GetVal(i+1, pos, nchars)!=NULL) {
				//_tochar sTemp(pSSS->GetVal(i+1, pos, nchars));
				currValTemp = pSSS->GetVal(i+1, pos, nchars);	//pass
				currValTemp.trim();		//remove trailing spaces
				
				if(currValTemp.length()==0) {
					finVal = _ANSI("");
				}
				else {
					finVal = currVal;
				}

			}
			
			else {
				finVal = _ANSI("");
			}

            //_tochar sTemp(finVal);		
			PutValue(crtIDX, typCtr[crtIDX], j, finVal);				

			pos = pos + nchars;	//increment position
			
		}
		
		typCtr[crtIDX]++;
	}

}
void daedata::SetArray(varvec3d* srcArray)
{

	pValueMatrix = srcArray;

}
string* daedata::SQLItemValue(USHORT type, ULONG row, ULONG col, 
								  LPCSTR strEnclose,
								  BOOL bsReplace, BOOL eReplace)
{
	//daestring msg;
	//msg.format(_ANSI("t:%d, r:%d, c:%d"), type, row, col);
	//MessageBox(0,msg.c_str(), L"wala", MB_OK);
	

	daestring *itemValue;
	daestring itemType, sFind, sReplace,
		sEnclose(strEnclose);
	UINT intValue;
	USHORT shortValue;
	float floatValue;

	if ( sEnclose.trim().length()>0 ) {
		sReplace= _ANSI("\\") + sEnclose.left(1);
		sFind = sEnclose.left(1);
	}

	//_tochar sTemp(pDict[type]->GetVarType(col));
	itemType = pDict[type]->GetVarType(col);

	LPCSTR currVal = GetValue(type, row, col);
	itemValue = new daestring((LPCSTR) currVal);
	//MessageBox(0, itemValue->c_str(), L"wala", MB_OK);
	//itemValue->erase(itemValue->find_last_not_of(_ANSI(" "))+1);
	//itemValue->erase(0, itemValue->find_first_not_of(_ANSI(" ")));
	itemValue->trim();
	//MessageBox(0, itemValue->c_str(), L"wala", MB_OK);

	//MessageBox(0, itemValue->c_str(), L"wala", MB_OK);
	//check for null values		
	if (itemValue->length()==0) {
		itemValue->assign(_ANSI("NULL"));
	}


	else {
		
		if(itemType.compare(_ANSI("tinyint"))==0) {
			shortValue = atoi(currVal);;
			itemValue->format(_ANSI("%d"), shortValue);
		}
		
		else if(itemType.compare(_ANSI("smallint"))==0) {
			shortValue = atoi(currVal);;
			itemValue->format(_ANSI("%d"), shortValue);
		}

		else if(itemType.compare(_ANSI("int"))==0) {
			intValue = atoi(currVal);;
			itemValue->format(_ANSI("%d"), intValue);
		}
		
		else if(itemType.compare(_ANSI("mediumint"))==0) {
			intValue = atoi(currVal);;
			itemValue->format(_ANSI("%d"), intValue);
		}

		else if(itemType.compare(_ANSI("float"))==0) {
			
			floatValue = (float) atof(currVal);;
			itemValue->format(_ANSI("%f"), floatValue);
		}
		
		else if(itemType.compare(_ANSI("varchar"))==0) {
			
			//itemValue->Replace("'", _ANSI("")); //supressed due to errors
			if (bsReplace)
				itemValue->replace( _ANSI("\\"), _ANSI("\\\\") );
			
			if (eReplace)
				itemValue->replace(sFind, sReplace);

			*itemValue = sEnclose.left(1) + *itemValue + sEnclose.right(1);
		}
		
		else {
			//itemValue->Replace("'", _ANSI("")); //supressed due to errors
			if (bsReplace)
				itemValue->replace( _ANSI("\\"), _ANSI("\\\\") );
			
			if (eReplace)
				itemValue->replace(sFind, sReplace);
			*itemValue = sEnclose.left(1) + *itemValue + sEnclose.right(1);
		}
	}
	//MessageBox(0,itemValue->c_str(), L"wala", MB_OK);
	return itemValue;					

}
