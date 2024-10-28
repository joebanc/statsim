// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// StatSimPro.h : main header file for the StatSimPro application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

//#include "stdafx.h"
#include <vector>

//Define StatSimPro
#ifndef STATSIMPRO_H
	#define STATSIMPRO_H
#endif
/////////////////

#include "resource.h"       // main symbols
#include<windows.h>

#ifndef STATSIMAPI_H
	#include "StatSimAPI.h"
#endif
#ifndef STATSIMCONN
	#include "StatSimConn.h"
#endif
#ifndef STATSIMRS
	#include "StatSimRS.h"
#endif
#ifndef DAEDATA
	#include "daedata.h"
#endif
#ifndef STATSIMELT
	#include "statsimelt.h"
#endif

//#ifndef STATSIMDCT_H
//	#include "statsimdct.h"
//#endif

//define constants
//#define HPQ 1020040900
#define HPQ 720049
//#define BPQ 520070301
#define HH_COREIND 30017
#define HOUSEHOLD 5
#define IS_MYSQL_BE 1
#define STATSIMPRO_VER_6 6	//version
// temporary constant identifier if ODBC
#define ODBC FALSE
// temporary constant identifier for application path
#ifdef STATSIMSGE_VER_4
	#define SSPATH _T("C:\\CBMSDatabase\\System\\StatSimSGE 4.0")
#elif STATSIMPRO_VER_5
	#define SSPATH _T("C:\\CBMSDatabase\\System\\StatSimPro 5.0")
#elif STATSIMPRO_VER_6
	#define SSPATH GetExeFilePath() 
#else
	#define SSPATH _T("C:\\CBMSDatabase\\System\\StatSim 3.0")
#endif

// CStatSimProApp:
// See StatSimPro.cpp for the implementation of this class
//

//get file version
CString GetVersionInfo(HMODULE hLib, CString csEntry);

BOOL FieldExists(CString fldName, CString sTable, CStatSimConn* pDB);
BOOL TableExists(CString sTable, CStatSimConn* pDB);
CString CopyVal(CComboBox* pCB, CEdit* pEdit);
int GetNewIDVal(CString sTable, CString IDFld);

CString GetExeFilePath();
void ImportData(CString strPath, CStatSimConn *pToDB, long ID);
void ImportData(CString strPath, long ID);
int ImportBulkData(LPCSTR** sPathArray, UINT nPaths, long ID);

void ConUpElt(BOOL askbasic = TRUE, BOOL askmdg = FALSE);
void ProcMDG();
void ProcOKI();
void ProcCCRI();
void ProcBPQTabs();
int ConfigDB(CStatSimConn* pDB, CStatSimConn* pSrcDB = NULL, BOOL CopySrc = FALSE);

void TransferRecords(CStatSimRS* pFromRS, LPCSTR sTable, CStatSimConn* pToDB);
void TransferRecords(CStatSimConn* pFromDB, LPCSTR sTable, CStatSimConn* pToDB, LPCSTR sSQL = NULL);

void MakeUnionQuery();
void BuildBatchDBs();

void ExitMFCApp();

class CStatSimProApp : public CWinAppEx
{
public:
	CStatSimProApp();

private:
	POSITION m_DocTplPos, m_DocPos;
// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg virtual void OnFileNew();
	afx_msg void OnAppAbout();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnCallStatsim4();
	afx_msg void OnTabNewRpt();
	afx_msg void OnTabNewPortal();
};

extern CStatSimProApp theApp;
