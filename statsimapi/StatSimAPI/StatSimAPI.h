// StatSimAPI.h : main header file for the STATSIMAPI DLL
//
//This is an interface to stream and database as well as data dictionary.
//CStatSimStream handles access to a atream (currently reading only)
//CStatSimConn and CStatSimRS handles access to database
//daedata handles generalized data from a stream or database
//daedict handles data dictionary generalized data from a stream or database

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifndef STATSIMAPI_H
	#define STATSIMAPI_H
#endif

#include "resource.h"		// main symbols

#ifndef _MBCS
	#define _MBCS(x) x
#endif

//define export
#ifndef DLLEXPORT
	#define DLLEXPORT __declspec(dllexport)
#endif
#ifndef DLLIMPORT
	#define DLLIMPORT __declspec(dllimport)
#endif

// CStatSimAPIApp
// See StatSimAPI.cpp for the implementation of this class
//
#define TOT 0
#define MALE 1
#define FEMALE 2
#define NSEXVARS 3

#define COUNTRY 1001
#define REGN 1002
#define PROV 1003
#define MUN 1004
#define BRGY 1005
#define PUROK 1006
#define HH 1007
#define MEM 1008
#define HPQ_RAW 2000
#define hpq_hh 2001
#define hpq_mem 2002
#define hpq_prog 2007

#define hh_ind 2098
#define mem_ind 2099

#define IDENTICAL 0

// indicators
#define IND_POVP 3012
#define IND_SUBP 3013
#define IND_DEATH05 3002
#define IND_DEATHPREG 3003

class daedict;

DLLEXPORT std::string sql_itm_val(LPCSTR cVal, daedict **pDict, USHORT type, int col, LPCSTR strEnclose = NULL, 
		BOOL bsReplace = TRUE, BOOL eReplace = TRUE);
DLLEXPORT int ExtCh(std::wstring srcBuff, std::string& destbuff, UINT start, UINT length);
DLLEXPORT LPCSTR ConstChar(CString buffer);
//DLLEXPORT LPCSTR ConsChar(CString buffer);
//sItems array of vectors, ndim number of vectors in the array
//sItems is currently transposed
DLLEXPORT LPCSTR** vector_to_array(std::vector<LPCSTR> *sItems, int ndim, BOOL sort = FALSE, LPCSTR sortby = NULL);
DLLEXPORT std::vector<LPCSTR>* array_to_vector(LPCSTR **sItems, int nCols, long nRows, BOOL sort = FALSE, LPCSTR sortby = NULL);

DLLEXPORT CString MFCStr(LPCSTR buffer);
DLLEXPORT CString GetString(int nId);			//Get String from String Table

DLLEXPORT int GetItemIndex(CComboBox *pCB, DWORD k);
DLLEXPORT int GetArrayIndex(int* intArray, int k, int nSize);
DLLEXPORT int GetArrayIndex(long* intArray, long k, int nSize);
DLLEXPORT int GetArrayIndex(LPCSTR* intArray, LPCSTR k, int nSize);

DLLEXPORT int* SortArray(int* intArray, int nSize, BOOL asc = TRUE);

DLLEXPORT BOOL IsNull(CString sTest);
DLLEXPORT BOOL NoSpace(CString sTest);
DLLEXPORT CRect ResizeWindow(int nControl, CDialog *pDlg, float m = 2);
DLLEXPORT CString GetString(CEdit* pEdit);
DLLEXPORT CString GetString(CComboBox* pCB);
DLLEXPORT CString NormStr(LPCSTR sStr);
DLLEXPORT HINSTANCE Execute(LPCSTR sExePath, HWND hWnd, LPCSTR sOperation = _MBCS("open"), 
							LPCSTR sParam = NULL, LPCSTR sDir = NULL, int nShowCmd = SW_SHOWNORMAL);
DLLEXPORT CString ZeroFill(CString sNum, int nLen, BOOL left = TRUE);
DLLEXPORT CString Mulchar(LPCSTR ch, short x);
DLLEXPORT CString WithChar(LPCSTR ch, short x);

DLLEXPORT BOOL Exists(LPCSTR* sArray, LPCSTR sCrit, int nItems);
DLLEXPORT BOOL Exists(std::vector<LPCSTR>& sArray, LPCSTR sCrit, int nItems = NULL);
DLLEXPORT BOOL Exists(CStringArray& sArray, LPCSTR sCrit, int nItems = NULL);
DLLEXPORT BOOL Exists(long* iArray, long iCrit, int nItems);
DLLEXPORT std::vector<LPCSTR> unique_array(LPCSTR* sArray, int size);


class CStatSimAPIApp : public CWinApp
{
public:
	CStatSimAPIApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
