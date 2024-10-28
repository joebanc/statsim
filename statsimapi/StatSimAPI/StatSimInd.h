#pragma once

//Define StatSimInd
#ifndef STATSIMIND
	#define STATSIMIND
#endif
//////////////////

class CStatSimConn;

enum DLLEXPORT INDGRP {FIG = 3, SECT = 2, CAT = 1};	//figure, sector, category
enum DLLEXPORT ELTGRP {GEN = 3, RAW = 2, GEO = 1};	//figure, sector, category
enum DLLEXPORT INDATTR {indID = 0, indPar = 1, indName = 2, indLabel = 3, indDesc = 4}; //id, parent id, name, description, definition
enum DLLEXPORT SQLTYPE {SELECT = 1, CREATE = 2};

//this will define the readily available indicators
#define IND_READY_START 30001
#define IND_READY_END 39999
//#define IND_READY_END 389

//this will define the readily available indicators
#define IND_CCI_START 30018
#define IND_CCI_END 30023
#define IND_COREIND_HH 30017

//define millennium development goals
#define IND_MDG_START 31001
#define IND_MDG_END 31005
//define CDP
#define IND_CDP21_START 32001
#define IND_CDP21_REGN 32001
#define IND_CDP21_PROV 32002
#define IND_CDP21_MUN 32003
#define IND_CDP21_BRGY 32004
#define IND_CDP21_PUROK 32005
#define IND_CDP21_END 32005
#define IND_CDP22_START 32006
#define IND_CDP22_REGN 32006
#define IND_CDP22_PROV 32007
#define IND_CDP22_MUN 32008
#define IND_CDP22_BRGY 32009
#define IND_CDP22_PUROK 32010
#define IND_CDP22_END 32010
#define IND_CDP23_START 32011
#define IND_CDP23_REGN 32011
#define IND_CDP23_PROV 32012
#define IND_CDP23_MUN 32013
#define IND_CDP23_BRGY 32014
#define IND_CDP23_PUROK 32015
#define IND_CDP23_END 32015

#define IND_CDP24_START 32016
#define IND_CDP24_REGN 32016
#define IND_CDP24_PROV 32017
#define IND_CDP24_MUN 32018
#define IND_CDP24_BRGY 32019
#define IND_CDP24_PUROK 32020
#define IND_CDP24_END 32020

#define IND_CDP25_START 32021
#define IND_CDP25_REGN 32021
#define IND_CDP25_PROV 32022
#define IND_CDP25_MUN 32023
#define IND_CDP25_BRGY 32024
#define IND_CDP25_PUROK 32025
#define IND_CDP25_END 32025

//define methods of identifying the poor
#define TARGETING_START 3365
#define TARGETING_END 3376

//this will define the readily available summary indicators
#define IND_TOTPOP_START 30001
#define IND_TOTPOP_END 30005
#define IND_DEMOG_START 30006
#define IND_DEMOG_END 30010

#define IND_CORE_START 30012
#define IND_CORE_END 30016

//id of the partial core indicators
#define PARTIAL_CORE_START 3264
#define PARTIAL_CORE_END 3271
#define PCIQUINTILE_CORE_START 3297
#define PCIQUINTILE_CORE_END 3300
#define PARTIAL2_CORE_START 3309
#define PARTIAL2_CORE_END 3364

//this will define indexes of the core
#define N_COREIND 14
#define VAR 0
#define LABEL 1
#define NUM 2
#define DENOM 3

//health
#define DEATH05 1
#define DEATHPREG 2
#define MALN05 3
//house
#define MSH 4
#define SQUAT 5
//watsan
#define NTSWS 6
#define NTSTF 7
//educ
#define NTELEM612 8
#define NTHS1316 9
//income
#define POVP 10
#define SUBP 11
#define FSHORT 12
//employ
#define UNEMPL15AB 13
//peace
#define VICTCR 14

//Indicator labels
DLLEXPORT void InitIndTxt();
DLLEXPORT void DestIndTxt();

class __declspec(dllexport) CStatSimInd  
{
public:
	CStatSimInd(CStatSimConn * pDB, INDGRP, int cInd = -1, int cE = -1, BOOL initAttr = FALSE);
	virtual ~CStatSimInd();

	//elemental (e.g. by purok)
	CString sIndTable();
	CString sIndLabel();
	CString sIndDesc();

	//single
	CString GetSector();
	CString GetLabel();
	CString GetDesc();
	CString sNumerator();
	CString sDenominator();
	CString sNumeratorLabel();
	CString sDenominatorLabel();
	CString sPropCompute();
	BOOL WithSex();
	BOOL WithCat();

	CString GetTable(INDGRP cIG);
	CString GetIDFld(INDGRP cIG);
	CString GetParIDFld(INDGRP cIG);
	CString GetTable();
	CString GetIDFld();
	CString GetParIDFld();
	CString GetEltID();

	void BuildInd(SQLTYPE SQLType = CREATE);
	void AddPrimaryKey();
	void AddIndex();

	CString BuildSQL(SQLTYPE SQLType = CREATE);
	CString sSQLExp();
	CString sSQLLnk();
	CString sSQLSrc();
	CString sSQLWhere();
	CString sSQLGroupBy();
	CString sSQLPrKey();
	CString sSQLIndex();
	
	CString sSQLDestroy();
	
	CString GetSrcTable(int id);

	DWORD DeleteHItem(DWORD nID);
	DWORD DeleteItem(CComboBox* pCB);

//data members
private:

	CStatSimConn* m_pDB;
	INDGRP m_cIG;
	int m_cInd;
	int m_cE;
	long m_nChild;

	CString sTable();
	CString sILnkTable();
	CString sIVETable();
	CString sISrcTable();
	CString sISrtTable();

	
//implementation
private:
	
	DWORD* GetChildID(INDGRP cIG, INDGRP cIGFld, DWORD nID);
	DWORD* GetChildID(INDGRP, DWORD);
	DWORD* GetChildID(DWORD);

	DWORD DeleteItem(INDGRP cIGTable, INDGRP cIGIDFld, DWORD nID);
	DWORD DeleteItem(INDGRP, DWORD);
	DWORD DeleteItem(DWORD);

};
