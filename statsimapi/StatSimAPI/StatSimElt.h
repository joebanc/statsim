#pragma once

#ifndef STATSIMDAELIB
	#include "statsimdaelib.h"
#endif

//Define StatSimElt
#ifndef STATSIMELT
	#define STATSIMELT
#endif
//////////////////

class CStatSimConn;
class CSSMySQLconn;
class CStatSimRS;

class CStatSimWnd;
class CStatSimBar;
// StatSimElt.h: interface for the CStatSimElt class.
//
//////////////////////////////////////////////////////////////////////
//enum DLLEXPORT ELEMENT {MEM = 0, HH = 1, PUROK = 2, BRGY = 3, MUN = 4, PROV = 5, REGN = 6, COUNTRY = 7};

typedef int ELEMENT;
enum DLLEXPORT ELTATTR {elementID = 0, digit = 1, element = 2, label = 3, description = 4,
				IDField = 5, name = 6, parent = 7, etype = 8, EindID = 9, sSQL = 10, sSQLtype = 11};

void build_geo_attr(tstrvec2d& geo_arr, CStatSimConn *pDB);
BOOL field_exists(CString fldName, CString sTable, CStatSimConn* pDB);

class element_id
{
public:
	element_id(ELEMENT, LPCSTR);	//string id
	element_id(ELEMENT, unsigned long);		//long int id

	virtual ~element_id();

};
class __declspec(dllexport) CStatSimElt  
{
public:
	CStatSimElt(CStatSimConn * pDB, ELEMENT, BOOL initAttr = FALSE);
	virtual ~CStatSimElt();

//attributes
public:
	CStatSimRS* pRS;

private:
	/*ELEMENT*/ int currElt; int IDElt;
	LPCSTR eltTable, eltID;
	//CSSArrayBuilder* pArrBldr;
	varvec2d eltArray;
	LPCSTR *attrFldArr, *attrArr, *parattrArr, *chiattrArr;
	USHORT nAttr, nElt;

	CStatSimConn * pEltDB;
	CSSMySQLconn * pEltMySQLDB;
	void CrCoreInd(long qnrID, BOOL update = TRUE, CStatSimWnd *pLabel = NULL, CStatSimBar *pProgBar = NULL);

//Implementation
public:
	BOOL wAttr, wPar, wChi;

	LPCSTR Attr(ELTATTR);
	LPCSTR ParAttr(ELTATTR);
	LPCSTR ChiAttr(ELTATTR);
	LPCSTR* GetAttr();
	LPCSTR* GetParAttr();
	LPCSTR* GetChiAttr();
	
	ELEMENT GetParent();
	ELEMENT GetChild();
	UINT CrDigit();

	
	void CrPartialCore(int given);
	void CrIncDistCore(LPCSTR sIncDistVar, LPCSTR sSrc = _MBCS("hh_ind"));
	void DropPartialCore(int given);

	void CrCCI();
	void CrLogit(float cutoff = 0.5);
	void CrMDG(long qnrID, float perday = 0);
	void CrOKI(long qnrID);
	void CrCCRI(long qnrID);
	void CrCDP(long qnrID);
	void CrBPQTabs();

	void CrHHIndCatEx(CString sName, std::vector<CString> sVars, std::vector<CString> sLabels, CString sDenom = _T("tothh"));
	void CrBrgyIndCatEx(CString sSrc, CString sName, std::vector<CString> sVars, 
		std::vector<CString> sLabels, CString sDenom = _T("tothh"), BOOL sVarAsIs = TRUE);
	void CrMemInd(ELEMENT srcType, LPCSTR sInd, LPCSTR sDenom, LPCSTR sMExt = _MBCS("_Male"), LPCSTR sFExt = _MBCS("_Female"),
		LPCSTR sSrcInd = _MBCS("Mem_Ind"), LPCSTR sSrcDenom = _MBCS("Demog"), BOOL death = FALSE, LPCSTR IDF = _MBCS("memID"), 
		LPCSTR sAggrFcn = _MBCS("SUM"), int units = 100, 
		LPCSTR sIndOut = NULL, LPCSTR sDenomOut = NULL, LPCSTR sAugDenom = NULL);
	void CrHHInd(ELEMENT srcType, LPCSTR sInd, LPCSTR sDenom, 
		LPCSTR sSrcInd = _MBCS("HH_Ind"), LPCSTR sSrcDenom = _MBCS("TotPop"),
		LPCSTR IDF = _MBCS("hhID"), 
		LPCSTR sAggrFcn = _MBCS("SUM"), int units = 100,
		bool wHHSuffix = TRUE, LPCSTR sIndOut = NULL, LPCSTR sDenomOut = NULL);

	void CrTotPop(long qnrID, CStatSimWnd *pLabel = NULL, CStatSimBar *pProgBar = NULL);
	void CrDemog();
	int ProcInd(long qnrID);
	void ConfigTable();
	void AddPrimaryKey(CString Key);
	void AddPrimaryKey(CString sTabExt, CString Key);
	void AddPrimaryKey(LPCSTR sTable, LPCSTR KeySet);
	void AddKey(CString Key);
	void AddKey(CString sTabExt, CString Key);
	void UpdateTable();
	void CreateTable(BOOL dodrop=FALSE);
	CString IdxClause();


	//categorical xtab
	CString CrMemIndCat(ELEMENT srcType, LPCSTR sIndCat, LPCSTR sDenom, BOOL wLabel = TRUE, 
		LPCSTR sMExt = _MBCS("_Male"), LPCSTR sFExt = _MBCS("_Female"),
		LPCSTR sSrcIndCat = _MBCS("hpq_mem"), LPCSTR sSrcDenom = _MBCS("Demog"), 
		LPCSTR IDF = _MBCS("CONCAT(`hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`)"),
		LPCSTR sFilter = NULL, LPCSTR sSexSrc = _MBCS("hpq_mem"), LPCSTR sSexVar = _MBCS("sex"), LPCSTR sMemLine = _MBCS("memno"), LPCSTR* pSexVarExt = NULL,
		LPCSTR sAggrFcn = _MBCS("COUNT"));

	CString CrHHIndCat(ELEMENT srcType, LPCSTR sIndCat, LPCSTR sDenom, BOOL wLabel = TRUE,
		LPCSTR sSrcIndCat = _MBCS("hpq_hh"), LPCSTR sSrcDenom = _MBCS("TotPop"),
		LPCSTR IDF = _MBCS("CONCAT(`hpq_hh`.`regn`, `hpq_hh`.`prov`, `hpq_hh`.`mun`, `hpq_hh`.`brgy`, `hpq_hh`.`purok`, `hpq_hh`.`hcn`)"),
		LPCSTR sFilter = NULL, LPCSTR sWt = NULL);

private:
	USHORT fIDX(LPCSTR fldName);
	void DeleteInd();
	void InitAttr();
	CString sSQLDemog();

	CString sSQLTotHH();
	CString sSQLTotMem(LPCSTR sex = NULL, LPCSTR sSex = NULL, 
		BOOL exmem = FALSE, BOOL work = FALSE);
	CString sSQLTotPop();

	CString sSQLCoreInd();
	CString sSQLCCI();
	CString sSQLLogit(float cutoff);
	CString sSQLMDG();
	CString sSQLSelectMDG(float perday);

	CString sSQLMemInd(ELEMENT srcType, LPCSTR sInd, LPCSTR sDenom, LPCSTR sMExt, LPCSTR sFExt,
		LPCSTR sSrcInd, LPCSTR sSrcDenom, BOOL death, LPCSTR IDF, LPCSTR sAggrFcn, int units,
		LPCSTR sIndOut, LPCSTR sDenomOut, LPCSTR sAugDenom);
	CString sSQLHHInd(ELEMENT srcType, LPCSTR sInd, LPCSTR sDenom, LPCSTR sSrcInd, LPCSTR sSrcDenom, 
		LPCSTR IDF, LPCSTR sAggrFcn, int units,
		bool wHHSuffix, LPCSTR sIndOut, LPCSTR sDenomOut);

	//categorical
	CString sSQLMemIndCat(ELEMENT srcType, LPCSTR sIndCat, LPCSTR sDenom, LPCSTR sMExt, LPCSTR sFExt,
		LPCSTR sSrcIndCat, LPCSTR sSrcDenom, LPCSTR IDF,
		LPCSTR sFilter = NULL, LPCSTR sSexSrc = _MBCS("hpq_mem"), LPCSTR sSexVar = _MBCS("sex"), LPCSTR sMemLine = _MBCS("memno"), LPCSTR* pSexVarExt = NULL,
		LPCSTR sAggrFcn = _MBCS("COUNT"));
	CString sSQLHHIndCat(ELEMENT srcType, LPCSTR sIndCat, LPCSTR sDenom, LPCSTR sSrcIndCat, LPCSTR sSrcDenom, LPCSTR IDF,
		LPCSTR sFilter = NULL, LPCSTR sWt = NULL);

	CString sSQLGeo(LPCSTR sSRc);
	CString sGeo(LPCSTR sSR = NULL);
	CString sGeoJoin(LPCSTR sLSrc, LPCSTR sRSrc);
	CString sGeoJoin(LPCSTR eBase, ELEMENT srcType, LPCSTR sSrc);
	CString sGeoLabel(LPCSTR sSR);
	CString sSQLInd();
	CString sSQLConfig();
	CString sSQLCreate();
	CString sSQLUpdate();
	CString sSQLCrDecTable();
	CString sSQLCrQuiTable();

	//for partial tables
	CString sSQLCreatePartialTableHH(int given);
	CString sSQLInsertPartialTableHH(int given);
	CString sSQLCreatePartialTableMEM(int given);
	CString sSQLInsertPartialTableMEM(int given);

	CString sSQLCreateIncDistCoreIndTable(LPCSTR sIncDistVar);
	CString sSQLInsertIncDistCoreIndTable(LPCSTR sIncDistVar, LPCSTR sSrc);

	void SetQnrID(long qnrID);

	//get source ID
	int GetSrcID(LPCSTR sSrc);
	int nGKC();

	long m_qnrID;


	//temporary handler for geo data dictionary; id, 
	tstrvec2d m_geo_attr;	//0 - id; 1 - name; 2 - digits;

};


