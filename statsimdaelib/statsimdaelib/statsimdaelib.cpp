// statsimdaelib.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "statsimdaelib.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}


LPCSTR _daesotch(string buffer)
{

	int size = buffer.length() + 1;
	LPSTR tempstr = new char[size];
	//_tcscpy_s(tempstr, size, buffer);
	strcpy(tempstr, buffer.c_str());

	return tempstr;

}

int exists(std::vector<std::string>& sArray, std::string sCrit)
{
	int id_found = -1;

	for (int i=0; i<sArray.size(); i++) {
		if ( sArray[i].compare(sCrit)==0 ) {
			id_found = i;
			return id_found;
		}
	}
	
	return id_found;

}

int exists(std::vector<int>& intArray, int intCrit)
{
	int id_found = -1;

	for (int i=0; i<intArray.size(); i++) {
		if ( intArray[i]==intCrit ) {
			id_found = i;
			return id_found;
		}
	}
	
	return id_found;

}
USHORT GetIDX(LPCSTR* pArray, USHORT nSize, LPCSTR sCrit, USHORT lengths, bool alpha)
{
	//boost::ptr_vector<LPCSTR>;
	daestring sVal, csVal;
	USHORT idx = -1;
	//LPCSTR sVal, csVal;
	//char *csBuff = new char[lengths];
	//char *sBuff = new char[lengths];


	for (USHORT k=0; k<nSize; k++) {

		if (!alpha) {

			sVal.format("%d", atoi(pArray[k]));
			//sprintf(sBuff, "%d", atoi(pArray[k]));
			//sVal = sBuff;
			
			csVal.format("%d", atoi(sCrit));
			//sprintf(csBuff, "%d", atoi(sCrit));
			//csVal = csBuff;

		}
		else {
			sVal.assign(pArray[k]);
			csVal.assign(sCrit);

			sVal.makelower().trim();
			csVal.makelower().trim();
			
			//MessageBox(0,L"Tangina nyo", L"trim", MB_OK);
		}
		
		//int IsIdentical = strcmp( sVal, csVal );
		int IsIdentical = sVal.compare( (string) csVal );
		if ( IsIdentical==0 ) {		//identical
			idx = k;
			//delete [] sBuff;
			//delete [] csBuff;
			return idx;
		}
		if ((k==nSize-1) && (idx==-1)) {
			//catch this!!!
			//AfxMessageBox( _ANSI("Type not found in array") );
		}
	}

	//delete [] sBuff;
	//delete [] csBuff;
	return idx;

}
