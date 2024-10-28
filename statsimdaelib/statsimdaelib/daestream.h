#pragma once

#include <stdio.h>
#include <fstream>
#include <functional>

#ifndef DAEVECTOR
	#include "daevector.h"
#endif

//Define StatSimStream
#ifndef DAESTREAM
	#define DAESTREAM

#define SSS_BEGIN	0
#define SSS_END		(-1)
#define rt_pos		1
#define pos_idx			0
#define len_idx			1
#define MAX_RTLEN		255

template<typename T> 
	struct del_obj: 
		public std::unary_function<const T*, void> 
{ 
	void operator()(const T* ptr) const
	{
		delete ptr;
	}
};

class __declspec(dllexport) daestream
{
public:
	//construction and destruction
	daestream(LPCSTR strPath, LPCSTR* pType = NULL, USHORT nTypes = 1, USHORT rtLen = 1, 
		BOOL initAttr = TRUE, BOOL split = FALSE);  //There has to be at least one type

	daestream(FILE *currStream, LPCSTR* pType = NULL, USHORT nTypes = 1, USHORT rtLen = 1, 
		BOOL initAttr = TRUE, BOOL split = FALSE);  //There has to be at least one type

	virtual ~daestream();

	
	//methods
	void MakeArray(LPCSTR*** pDctArr, UINT* nDictVars, UINT* nDictAttr = NULL, 
		LPCSTR sFilePath = NULL);

	varvec3d* GetArray();

	int GoTo(UINT pos);	//set file pointer to a specific location
	char* GetVal(ULONG row, ULONG col, UINT length, BOOL dynamic = FALSE);
	char* GetLine(ULONG row);
	LPCSTR GetType(ULONG row, USHORT nTypes);
	ULONG TotLines();
	ULONG Tochars();
	ULONG nRTYPE(USHORT RTi);
	ULONG* TypeCountArr();	//array of line type counts

	int GetFD();	//return file descriptor

	void SetRTLen(int len);	//set length of recordtype;
	short GetRTLen();

	float FileSize(long denom = 1);	//get file size, denom determines unit
	LPCSTR sFileSize();	//get file size in string
	LPCSTR sFileTimeMod();	//get time modified

	void SplitTypes(LPCSTR sFilePath = NULL, LPCSTR sExt = (LPCSTR) L".txt");	//file path

private:
	//attributes
	//CSSArrayBuilder** valArrBldr;	//value matrix builder 
	varvec3d value_array;	//we just fix / static array for known dimensions
	//LPCSTR ***pValueMatrix;	//[rtype k][column j][row i]
	LPCSTR ***pDctMatrix;

	ULONG* pCountType;	//array of line type counts
	FILE *pStream;
	ULONG nchars, nlines;

	LPCSTR m_sFileSize;
	LPCSTR m_sFileTimeMod;

	LPCSTR* m_pTYPE;
	USHORT m_nTYPES;
	USHORT m_rtLen;

	LPCSTR m_sPath;
	float m_nPercent;
	
	struct stat FDbuff;

	int sssfd;	//stream file descriptor (int)

	intvec2d pole;

	tstrvec1d type_array;	//type array builder	
										//char *typeArray;	//[row] = Array type

	//methods
	void Construct(FILE *currStream, LPCSTR* pType, USHORT nTypes, USHORT rtLen, 
		BOOL initAttr, BOOL split);

	ULONG RowPos(ULONG row);
	int RowLen(ULONG row);
	char* GetChar(ULONG offset, UINT length, BOOL dynamic);
	int ExtractChar(std::string srcBuff, std::string& destbuff, UINT srcLen, UINT start, UINT length);	//extract character from a buffer:zero-based start
	void GetAttr(BOOL split = FALSE); //get total number of lines and chars

	void InitmPtrs();
};

//	Notes:
//	fgets - gets set of characters but when it reaches '\n' it stops.  The pointer will resume in the following line when the command is called again
//	getc -  per character retrieval
//	fread - reads characters as specified in size
//
	//unused
//	CString GetLine();
//	__int64 GetTochar();
//	long GetTotLine();
//	long GetTotLine(CString critVal, __int64 ipos);	
//	fpos_t GoToRow(__int64 row);

#endif
//////////////////
