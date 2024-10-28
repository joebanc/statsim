#pragma once

#ifndef STATSIMDAELIB
#define STATSIMDAELIB

#ifndef DAE_NEXIST
#define DAE_NEXIST -1
#endif
//dae library items
#ifndef DAESTREAM
	#include "daestream.h"
#endif

#ifndef DAESTRING
	#include "daestring.h"
#endif
//#define SAFEDELETE(pointer) if((pointer)) \ 
//                           { delete (pointer); pointer = NULL; } 

__declspec(dllexport) LPCSTR _daesotch(string buffer);

__declspec(dllexport) int exists(std::vector<std::string>& sArray, std::string sCrit);	//returns an index to the existing item
__declspec(dllexport) int exists(std::vector<int>& intArray, int intCrit);	//returns an index to the existing item
__declspec(dllexport) USHORT GetIDX(LPCSTR* pArray, USHORT nSize, LPCSTR sCrit, USHORT lengths, bool alpha = FALSE);

#endif

/*
vector<char> vs;
//fill
ofstream outfile("nanocube.txt", ios::out | ios::binary);
ostream_iterator<char> oi(outfile, '\0');
copy(vs.begin(), vs.end(), oi);
*/