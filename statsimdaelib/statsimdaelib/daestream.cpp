#include "stdafx.h"
#include "statsimdaelib.h"
#include "daestream.h"
#include "daestring.h"
#include "daeconv.h"
#include <malloc.h>
#include <sys/stat.h> 
#include <sys/types.h>
#include <time.h>
#include <sstream>
using namespace std;

enum DCTATTR { VAR = 0, TYPE = 1, WIDTH = 2, DEC = 3 };


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

daestream::daestream(FILE *currStream, LPCSTR* pType, USHORT nTypes, USHORT rtLen, 
					 BOOL initAttr, BOOL split)
{
	InitmPtrs();

	m_sPath = NULL;	//set to c: if no path is given

	Construct(currStream, pType, nTypes, rtLen, initAttr, split);
}

daestream::daestream(LPCSTR strPath, LPCSTR* pType, USHORT nTypes, USHORT rtLen,
							   BOOL initAttr, BOOL split)
{
	InitmPtrs();

	FILE *currStream = fopen(strPath, "r");
	m_sPath = strPath;	//set path

	if (currStream==NULL) {
		//LPCSTR msg; 
		//msg.Format( _ANSI("Error accessing %s"), strPath );
		//AfxMessageBox(msg);
		//put catcher here!!!
	}
	
	else {
		Construct(currStream, pType, nTypes, rtLen, initAttr, split);
	}

}

void daestream::InitmPtrs()
{
	
	pCountType = NULL;
	m_pTYPE = NULL; 
	m_sPath = NULL; 
	//pole = NULL;	
	//value_array = NULL;
	//type_array = NULL;

}

daestream::~daestream()
{

	//temporary log
	//FILE *myfile = tfopen("C:\\statsim.log", "w");
	//fprintf(myfile, "value_array size: %d\n", value_array.at(0).at(0).size());
	//fprintf(myfile, "value_array mem: %d\n\n", sizeof(value_array));
	//let's see

	if (pStream!=NULL) {
		fclose(pStream);
		delete pStream;
	}
	//dont delete pCounttype since it is not initialized here.

	//free memory blocks
	if(pCountType){
		delete [] pCountType; pCountType = 0;
	}
	if(m_pTYPE){
		delete [] m_pTYPE; m_pTYPE = 0;
	}

	return;


	//the following are buggy

	//destruct array builders and initialized pointers
	int i, j;
	tstrvec1d::iterator str_k;
	for (i=0; i<pole.size(); i++) {
		pole[i].clear();
		intvec1d(pole[i]).swap(pole[i]);
	}
	pole.clear();
	intvec2d(pole).swap(pole);

	//trim memory allocation
	//destroyvec(*value_array, true);
	//destroyvec(value_array, true);
	
	for (str_k=type_array.begin(); 
		str_k!=type_array.end(); 
		++str_k) 
	{
		delete [] *str_k;
	}
	type_array.clear();
	tstrvec1d(type_array).swap(type_array);

	//fprintf(myfile, "value_array size: %d\n", value_array[0][0].size());
	//fprintf(myfile, "value_array mem: %d\n", sizeof(value_array));
	//fclose(myfile);

	//catch this!!!
	if(_heapmin()<0)
	{
	}
	//	 AfxMessageBox( _ANSI("Cannot give back memory to OS.") );

	//delete value_array; value_array=0;
}

void daestream::Construct(FILE *currStream, LPCSTR* pType, USHORT nTypes, USHORT rtLen,
							   BOOL initAttr, BOOL split)
{
	
	if (nTypes < 1) {
		//catch this!!!
		//AfxMessageBox( _ANSI("There has to be at least one type of record!") );
		return;
	}

	if (rtLen<=MAX_RTLEN) {
		m_rtLen = rtLen;	//automatically set record type length
	}
	else {
		//catch
	}
	
	//m_pTYPE = pType;

	m_nTYPES = nTypes;
	m_pTYPE = new LPCSTR[m_nTYPES]; 
	
	//transfer values of pType
	for (USHORT i=0; i<m_nTYPES; i++){
		m_pTYPE[i] = pType[i];
	}
	
	pStream = currStream;
	sssfd = _fileno(pStream);
	
	//Get data associated with sssfd:
	int FDres = fstat( sssfd, &FDbuff );

	//Check if statistics are valid:
	if( FDres != 0 )
		printf( "Bad file file descriptor\n" );

	
	//Construct attributes:
	if (initAttr) {
		GetAttr(split);
	}
	
}

char* daestream::GetVal(ULONG row, ULONG col, UINT length, 
								BOOL dynamic)
{
	ULONG offset = RowPos(row) + col;

	if ( (offset + 1) >= RowPos(row+1) ) {
		return 0;
	}
	else {
		char* buff = GetChar(offset, length, dynamic);
		return buff;
	
	}

}

char* daestream::GetLine(ULONG row)
{

	return GetVal(row, 1, RowLen(row));

}

int daestream::GoTo(UINT pos)
{

	switch (pos) {

	case SSS_BEGIN:
		
		return fseek(pStream, 0, SEEK_SET);
		break;

	case SSS_END:
		
		return fseek(pStream, 0, SEEK_END);
		break;

	default:

		return fseek(pStream, pos, SEEK_SET);
		break;
	}

}

//overloaded
char* daestream::GetChar(ULONG offset, UINT length, BOOL dynamic)
{
	char* buff = 0;		//character buffer
	//dynamically set array, length + 1 relating to _fgetts
	buff = new char[length+1];
	
	if(buff == NULL) {
		//catch this!!!
		//AfxMessageBox( _ANSI("Insufficient memory available") );
		return 0;
	}

	else {

		if (dynamic) {	//test for dynamic movement of cursor
			fseek(pStream, (offset - 1), SEEK_CUR);
		}

		else {
			fseek(pStream, (offset - 1), SEEK_SET);
		}	
		
		if(fgets(buff, (length + 1), pStream) == NULL) {
			//until the number of characters read is equal to n-1 (which is length)
			//AfxMessageBox( _ANSI("Null characters") );
			return 0;
		}

		else {
			return buff;
		}
	}
	
}

LPCSTR daestream::GetType(ULONG i, USHORT nTypes)
{
	if (nTypes>1) {
		//_tochar sRet(type_array[i-1]);
		return type_array[i-1];
	}
	else {
		return 0;
	}

}

varvec3d* daestream::GetArray()
{
	return &value_array;
}

ULONG* daestream::TypeCountArr()
{
	return pCountType;
}

ULONG daestream::Tochars()
{
	return nchars;
		
}

ULONG daestream::TotLines()
{
	return nlines;
		
}

ULONG daestream::nRTYPE(USHORT RTi)
{

	return pCountType[RTi];

}

ULONG daestream::RowPos(ULONG i)
{	
	
	if(i > 0) {
		return pole[pos_idx][i-1];	//temporarily set as [pos][i-1] - real numbering
	}
	else {
		//catch!!!
		//AfxMessageBox( _ANSI("Specify positive values only!") );
		return -1;
	}
	
}

int daestream::RowLen(ULONG i)
{

	if(i > 0) {
		return pole[len_idx][i-1];	//temporarily set as [len][i-1] - real numbering
	}
	else {
		//catch!!!
		//AfxMessageBox( _ANSI("Specify positive values only!") );
		return -1;
	}
	
}

void daestream::GetAttr(BOOL split)
{
	
	USHORT Ti;

	//initialize
	if (!pCountType)
		pCountType = new ULONG[m_nTYPES];

	for (Ti=0; Ti<m_nTYPES; Ti++) {
		pCountType[Ti] = 0;
	}

	char *ch, *rt;
	ch = new char[1];
	rt = new char[m_rtLen];
	//bool sEOF;	//tells whether a series has been an end of file

	nchars = 0; nlines = 0;
	int currchars = 0;
	
	//(nlines + 1) means the first position is the initial 0
	//initialize - there is always at least one row (row[0])
	//pole = new intvec1[2];
	pole.resize(2);
	pole[pos_idx].push_back(0);	//initionalize position at 0;

	//if (m_nTYPES>1) {
	//	type_array = new strvec1;
	//}

	while((ch[0] = getc(pStream))!=EOF) {	
		
		nchars++;		//increment total characters
		m_nPercent = ((float) nchars/FileSize())*100;

		//detect new line
		if (ch[0]=='\n') {
			
			//newline: things to be done when new line is detected
			nlines++;
			
			//assign latest value length at proper index (different from pos)
			pole[len_idx].push_back( currchars );			
		
			//assign latest value for position of the line
			pole[pos_idx].push_back( ftell(pStream) );

			if (currchars==0)	//checks if there is still no character that have been read, means null line
				type_array.push_back( NULL );

			currchars = 0;	//!reset current number of characters of the line!

		}

		else {
			currchars++;	//increment total characters for the current line

			if ( currchars>0						//detect if the no. of chars exceeds zero
				&& currchars>=rt_pos					//detect if the no. of chars reaches the record type position
				&& currchars<=(rt_pos+m_rtLen-1) )	//detect if the no. of chars is still inside the length of the record type
			{
				rt[currchars-1] = ch[0];
			}
			else if ( currchars>0					//detect if the no. of chars exceeds zero
				&& currchars==rt_pos+m_rtLen )	//detect if the no. of chars immediately outside the length of the record type
			{
				if (m_pTYPE!=NULL) {
					if (m_nTYPES>1) {
						USHORT idx = GetIDX( m_pTYPE, m_nTYPES, rt, m_rtLen );	//get the index of the current type	
						if (idx>=0)	{ //detect if the record type fetched really exists
							//_tochar t_rt(rt);
							type_array.push_back( rt );
							pCountType[idx]++;
						}
						else {
							type_array.push_back( NULL );
						}
					}
				}
				else {
					for(Ti=0; Ti<m_nTYPES; Ti++){
						pCountType[Ti]++;
					}
				}
			}
		}
	}

	//check whether the previous is a newline
	fseek(pStream, ftell(pStream)-1, SEEK_SET);
	ch[0] = getc(pStream);
	//ch[0]=_gettc(pStream);

	if (ch[0]!='\n') {
		nlines++;
	}

}


void daestream::SetRTLen(int len)
{
	m_rtLen = len;
}


int daestream::GetFD()
{
	return sssfd;
}

//this doesn't work!
float daestream::FileSize(long denom)
{
	float rVal = (float) FDbuff.st_size/denom;
	return rVal;
}

LPCSTR daestream::sFileSize()
{
	daestring sSize;
	
	if (FDbuff.st_size < 1024) {
		sSize.format( _ANSI("%d B"), FDbuff.st_size);
	}
	
	else if ((FDbuff.st_size >= 1024) && (FDbuff.st_size < 1048576)) {
		sSize.format( _ANSI("%.1f KB"), FileSize(1024));
	}

	else if ((FDbuff.st_size >= 1048576) && (FDbuff.st_size < 1073741824)) {
		sSize.format( _ANSI("%.1f MB"), FileSize(1048576));
	}

	else {
		sSize.format( _ANSI("%.1f GB"), FileSize(1073741824));
	}

	m_sFileSize = _daesotch(sSize);
	return m_sFileSize;

	
/*	char buff[15];
	int err;

	if (FDbuff.st_size < 1024) {
		err = tsprintf(buff, 16, L"%d B", FDbuff.st_size);
	}
	
	else if ((FDbuff.st_size >= 1024) && (FDbuff.st_size < 1048576)) {
		err = tsprintf(buff, 16, L"%.1f KB", (float) FDbuff.st_size/1024 );
	}

	else if ((FDbuff.st_size >= 1048576) && (FDbuff.st_size < 1073741824)) {
		err = tsprintf(buff, 16, L"%.1f MB", (float) FDbuff.st_size/1048576 );
	}

	else {
		err = tsprintf(buff, 16, L"%.1f GB", (float) FDbuff.st_size/1073741824 );
	}


	if (err<=0)
		MessageBox(0, L"Conversion error", L"statsim", MB_OK);

	//strcpy(sSize, buff);
	//tstring sSize(buff);
	//MessageBox(0, tscpy(sSize, buff), L"Statsim", MB_OK);
	//return sSize;
	m_sFileSize = _towchar(buff);
	return m_sFileSize;
*/
}
LPCSTR daestream::sFileTimeMod()
{
	//stringstream sFileTM;
	//sFileTM<<ctime(&FDbuff.st_mtime);
	//m_sFileTimeMod = _towchar(sFileTM.str().c_str());

	//string sFileTM;
	//sFileTM.format(_ANSI("%s"), _towchar(ctime(&FDbuff.st_mtime)) );
	//MessageBox(0, sFileTM.c_str(), L"Pucha", MB_OK);
	//m_sFileTimeMod = _daesotch(sFileTM);
	//MessageBox(0, m_sFileTimeMod, L"Pucha", MB_OK);
	//unicode - no mbc yet
	
	m_sFileTimeMod = ctime(&FDbuff.st_mtime);
	return m_sFileTimeMod;

}
void daestream::SplitTypes(LPCSTR sFilePath, LPCSTR sExt)
{
	nlines = 0;

	//_tochar tsfile(sFilePath);
	daestring sfile(sFilePath);
	sfile.trim();

	char *rt;
	rt = new char[m_rtLen];

	if (sfile.length() <=0 ) 
		sFilePath = m_sPath;

	sfile.trimleft(); sfile.trimright();
	int dot = sfile.rfind("."); //length minus position of . minus one

	std::vector<daestring> sTypePath;
	sTypePath.resize(m_nTYPES);

	std::vector<FILE*> pFile;	//converted to vector to safeguard memory leaks
	
	int k;
	//open the streams (from type[1])
	for (k=0; k<m_nTYPES; k++) {
		sTypePath[k].format( "%s_TYPE%d%s", sfile.left(dot), k+1, sExt);
		pFile[k] = fopen( sTypePath[k].c_str(), "w" );
	}

	//_tochar sfile_str(sfile.c_str());
	ifstream inf(sfile.c_str());
	string line;
	string::size_type len;

	int nCh = 0;

/*
#ifdef _UNICODE
	while(!std::getline(inf, line).eof()) {
#else
	while(!std::getline(inf, line, '\n').eof()) {
#endif
*/		
	while(!std::getline(inf, line, '\n').eof()) {
		nlines++;
		len = line.length();
		if (!len)	//skip the line if zero length
			continue;

		LPCSTR sLine = line.c_str();
		
		for (int r=(rt_pos-1); r<(rt_pos+m_rtLen-1); r++) {	//incorporating the position of the record type
			rt[r] = sLine[r]; //get the record type characters/text
		}

		if (!rt)	//skip if null record type
			continue;

		USHORT idx = GetIDX( m_pTYPE, m_nTYPES, rt, m_rtLen );	//get the index of the current type

		fprintf(pFile[idx], "%s\n", sLine);	//get line and print
	
		nCh += len;
		float linprop = (nCh/FileSize())*100;

		//prCaption.Format( _ANSI("StatSim Split: Processing: %d percent..."),  (int) linprop);
		//m_sProgress = ConstChar(prCaption);
		m_nPercent = linprop;

	}

	//close the streams
	for (k=0; k<m_nTYPES; k++) {
		fclose(pFile[k]);
	}	
	//delete [] pFile; pFile = 0;	
}


void daestream::MakeArray(LPCSTR*** pDctArr, UINT* nDictVars, UINT* nDictAttr,
							   LPCSTR sPath)

{
	//value_array = new varvec3d;

	//SYSTEMTIME systime0, systime1;
	//GetSystemTime(&systime0);
	
	if (pDctArr==NULL) {
		//catch this!!!
		//AfxMessageBox( _ANSI("Can't process with null dictionary!") );
		return;
	}

	if (nDictVars==NULL) {
		//catch this!!!
		//AfxMessageBox( _ANSI("Can't process with no variables!") );
		return;
	}

	if ( strlen(m_sPath)<=0 && strlen(sPath)<=0 ) {
		//catch this!!!
		//AfxMessageBox( _ANSI("Can't process with no specified file!") );
		return;
	}

	pDctMatrix = pDctArr;	//pass the array of dictionary !important

	LPCSTR sFile;

	if (strlen(m_sPath)>0)
		sFile = m_sPath;
	else
		sFile = sPath;

	(value_array).resize(m_nTYPES);
	//initialize each vector
	for (short i=0; i<m_nTYPES; i++) {
		(value_array)[i].resize(nDictVars[i]);
	}
	
	//initialize
	if (!pCountType)
		pCountType = new ULONG[m_nTYPES];

	short k;
	UINT j;

	for (k=0; k<m_nTYPES; k++) {
		pCountType[k] = 0;
	}

	ifstream inf(sFile);
	string line;
	string::size_type len;

	int nCh = 0, cpos = 0, clen = 0;
	char* rt = new char[m_rtLen];
	//char *currVal = 0;

	// hey, i haven't noticed the line number reinitialization
	nlines = 0;

//#ifdef _UNICODE
//	while(!std::getline(inf, line).eof()) {
//#else
//	while(!std::getline(inf, line, '\n').eof()) {
//#endif
		
	//temporary log
	//FILE *myfile = fopen("d:\\statsim.log", "w");
	while(!std::getline(inf, line, '\n').eof()) {
		nlines++;
		len = line.length();

		if (!len)	//skip the line if zero length
			continue;
		
		//LPCSTR sLine = line.c_str();

		for (int r=(rt_pos-1); r<(rt_pos+m_rtLen-1); r++) {	//incorporating the position of the record type
			rt[r] = line[r]; //get the record type characters/text
		}

		//rt[m_rtLen-1] = '\0';	//terminating with 0 does not work

		if (!rt)	//skip if null record type
			continue;

		USHORT idx = GetIDX( m_pTYPE, m_nTYPES, rt, m_rtLen );	//get the index of the current type
		
		//daestring msg;
		//msg.format(L"%s index: %d", rt, idx);
		//MessageBox(0, msg.c_str(), L"wala", MB_OK);

		pCountType[idx]++;

		nCh += len;
		float linprop = (nCh/FileSize())*100;

		for (j=0; j<nDictVars[idx]; j++) {
			DCTATTR i = WIDTH;
			clen = atoi(pDctArr[idx][j][i]);
			
			//character buffer
			//dynamically set array, length + 1 relating to _fgetts
			//currVal  = new char[clen+1];
			std::string currVal;

			if ( !ExtractChar(line, currVal, len, cpos, clen) ) {	//get value
				//currVal = _ANSI("");
				currVal = "";
			}

			//_tochar sVal(currVal);
			//daestring str;
			//str.format("idx:%d; j:%d; len:%d; value: %s", idx, j, clen, currVal);
			//fprintf(myfile, "idx:%d; j:%d; len:%d; value: %s \n", idx, j, clen, currVal);
			//fprintf(myfile, "value_array mem: %d\n\n", sizeof(value_array));
			//let's see

			//daevariant* dVal = new daevariant(currVal);
			//daevariant dVal(currVal);
			(value_array)[idx][j].push_back( currVal );	//transfer value


			cpos += clen;
			clen = 0;
			//if (currVal) {
			//	delete [] currVal; 
			//	currVal = 0;
			//}
			//in destructor;
			//MessageBox(0, _T(""), _T("value"), MB_OK);

		}

		//MessageBox(_ANSI("line finished"));

		cpos = 0; //reset

		line.clear();
		//string(line).swap(line);

		//I removed the progress bar and status, malaki ang difference sa speed

	}
	//fclose(myfile);

	delete [] rt;

}

int daestream::ExtractChar(std::string srcBuff, std::string& destbuff, UINT srcLen, UINT start, UINT length)
{

	if (!srcBuff.length())
		return 0;

	if(srcBuff.length()<start)
		return 0;

	if(srcBuff.length()-start<length)
		return 0;

	daestring sTemp = srcBuff.substr(start,length);
	sTemp.trim();
	destbuff = sTemp;


	//if (!destbuff)
	//	return 0;

	//catch char overflow!!!

	//for (int i=0; i<length; i++) {
	//	if ( (start+i)<srcLen )
	//		destbuff[i] = srcBuff[start+i];
	//	else
	//		return 0;
	//}

	//destbuff[length] = '\0';

	return 1;

}


short daestream::GetRTLen()
{
	return m_rtLen;
}

