#include "StdAfx.h"
#include "StatSimElt.h"
#include "StatSimAPI.h"
#include "StatSimInd.h"
#include "StatSimWnd.h"

#ifndef STATSIMRS
	#include "statsimrs.h"
#endif

#ifndef STATSIMCONN
	#include "statsimconn.h"
#endif

//Array of core indicator field suffixes
extern LPCSTR **sHHCoreInd;
extern LPCSTR ***sMEMCoreInd;

short country_dig = 0, 
	regn_dig = 2, 
	prov_dig = 4, 
	mun_dig = 6,
	brgy_dig = 9,
	purok_dig = 11,
	hh_dig = 17,
	mem_dig = 19,
	id_dig = 9;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
element_id::element_id(ELEMENT e, LPCSTR id)
{

}
element_id::element_id(ELEMENT e, unsigned long id)
{
}

element_id::~element_id()
{
}

void build_geo_attr(tstrvec2d& geo_arr, CStatSimConn *pDB)
{
	///////////////////////////
	CString sSQL;
	sSQL = "SELECT `elementID`, `element`, `digit` \
		FROM `~helement` WHERE `elementid`<2000 \
		ORDER BY `elementid`;"; 
	CStatSimRS geoRS(pDB, sSQL);

	geoRS.MoveFirst();
	short ngeo = geoRS.GetRecordCount();
	geo_arr.resize(ngeo);

	for (short i = 0; i<ngeo; i++)
	{
		for (short j = 0; j<geoRS.GetFieldCount(); j++)
		{		
			geo_arr[i].push_back( geoRS.SQLFldValue(j) );

		}
		geoRS.MoveNext();
	}
	
}
BOOL field_exists(CString fldName, CString sTable, CStatSimConn *pDB)
{

	CStatSimRS *pRS = NULL;

	std::vector<LPCSTR> sFlds;

	CString sSQL;
	
	sSQL.Format(_T("SHOW COLUMNS IN `%s`;"), sTable );
	
	pRS = new CStatSimRS( pDB, sSQL);
	if (pRS->GetRecordCount() > 0) {
		pRS->MoveFirst();
	}
	for (int ti=0; ti<pRS->GetRecordCount(); ti++) {
		CString sColFld;
		sColFld.Format(_T("Field")); 
		CString sVal = pRS->SQLFldValue(ConstChar(sColFld));
		sFlds.push_back(ConstChar(sVal) );

		pRS->MoveNext();
	}

	delete pRS; pRS = 0;

	if(Exists(sFlds, ConstChar(fldName)) ) 
		return TRUE;
	else
		return FALSE;

}
void CStatSimElt::SetQnrID(long qnrID)
{
	m_qnrID = qnrID;
}

CStatSimElt::CStatSimElt(CStatSimConn *pDB, ELEMENT elt, BOOL initAttr)
{

	//initialise geo attributes
	//build_geo_attr(m_geo_attr, pDB);
	//////////////////////////////////
	

	InitIndTxt();

	pEltDB = pDB;
	
	//if (elt<MEM || elt>COUNTRY) {
	//	AfxMessageBox(_T("Element is not included in the enumeration!"));
	//	return;
	//}
		
	nElt = 8;
	currElt = elt;
	IDElt = elt;
	eltTable = _MBCS("~hElement");
	eltID = _MBCS("elementID");
	nAttr = 0;

	pRS = NULL;
	//pArrBldr = NULL;
	attrFldArr = NULL;
	attrArr = NULL;
	parattrArr = NULL;
	chiattrArr = NULL;

	if (initAttr) {
		
		wAttr = TRUE;
	
		USHORT j;
		
		CString sSQL;
		sSQL.Format(_T("SELECT * FROM `%s`;"), (CString) eltTable);
		
		pRS = new CStatSimRS( pEltDB );

		if ( pRS->RunSQL(sSQL)!=SQL_SUCCESS ) {
			wAttr = FALSE;
			return;
		}
		
		if (pRS->GetRecordCount()>0) {
			pRS->MoveFirst();
		}
		
		nAttr = pRS->GetFieldCount();
		
		attrFldArr = new LPCSTR[nAttr];
		for (j=0; j<nAttr; j++) {
			attrFldArr[j] = pRS->GetFieldName(j);
		}
		
		delete pRS; pRS = NULL;

		InitAttr();

	}

}

CStatSimElt::~CStatSimElt()
{

	DestIndTxt();
	delete pRS; pRS = NULL;
	//delete pArrBldr; pArrBldr = NULL;
	delete [] attrFldArr; attrFldArr = 0;
	delete [] attrArr; attrArr = 0;
	delete [] parattrArr; parattrArr = 0;
	delete [] chiattrArr; chiattrArr = 0;

}

void CStatSimElt::InitAttr()
{
	USHORT j;
	ELTATTR sF = elementID, vF;
	CString sSQL;

	//proper
	sSQL.Format(_T("SELECT * FROM `%s` WHERE `%s`=%d;"), (CString) eltTable, (CString) attrFldArr[sF], IDElt);
	//AfxMessageBox(sSQL);
	pRS = new CStatSimRS( pEltDB );
	if (pRS->RunSQL(sSQL)!=SQL_SUCCESS) {
		AfxMessageBox(_T("Cannot initiate element attributes."));
		return;
	}

	if (pRS->GetRecordCount()>0) {
		pRS->MoveFirst();
		attrArr = new LPCSTR[nAttr];
		for (j=0; j<nAttr; j++) {
			attrArr[j] = ConstChar(pRS->SQLFldValue(j));
		}
	}

	delete pRS; pRS = NULL;

	//parent
	vF = parent;
	sSQL.Format(_T("SELECT * FROM `%s` WHERE `%s`=%d;"), (CString) eltTable, (CString) attrFldArr[sF], atoi(attrArr[vF]));

	pRS = new CStatSimRS( pEltDB );
	pRS->RunSQL(sSQL);
	if (pRS->GetRecordCount()>0) {
		pRS->MoveFirst();
		wPar = TRUE; 
		parattrArr = new LPCSTR[nAttr];
		for (j=0; j<nAttr; j++) {
			parattrArr[j] = ConstChar(pRS->SQLFldValue(j));

		}
	}

	else {
		wPar = FALSE;
	}

	delete pRS; pRS = NULL;

	//child
	sF = parent;
	sSQL.Format(_T("SELECT * FROM `%s` WHERE `%s`=%d ORDER BY `%s`;"), (CString) eltTable, (CString) attrFldArr[sF], IDElt, (CString) eltID);
	pRS = new CStatSimRS( pEltDB );
	pRS->RunSQL(sSQL);

	if (pRS->GetRecordCount()>0) {
		pRS->MoveFirst();
		wChi = TRUE;		
		chiattrArr = new LPCSTR[nAttr];
		for (j=0; j<nAttr; j++) {
			chiattrArr[j] = ConstChar(pRS->SQLFldValue(j));
		}	
	}
	else {
		wChi = FALSE;
	}

	delete pRS; pRS = NULL;

}
USHORT CStatSimElt::fIDX(LPCSTR fldName)
{
	USHORT idx = 0;
	CString critFld, sFld;
	critFld.Format(_T("%s"), (CString) fldName);

	for (idx=0; idx<nAttr; idx++) {
		sFld.Format(_T("%s"), (CString) attrFldArr[idx]);
		if (sFld==critFld) {
			return idx;
		}
	}

	return -1;

}

LPCSTR* CStatSimElt::GetAttr()
{

	return attrArr;
	
}

LPCSTR* CStatSimElt::GetParAttr()
{
	if (wPar) {
		return parattrArr;
	}
	else {
		AfxMessageBox(_T("Element has no parent!"));
		return NULL;
	}
	
}

LPCSTR* CStatSimElt::GetChiAttr()
{

	if (wChi) {
		return chiattrArr;
	}
	else {
		AfxMessageBox(_T("Element has no child!"));
		return NULL;
	}

	
}

LPCSTR CStatSimElt::Attr(ELTATTR i)
{

	return attrArr[i];
	
}

LPCSTR CStatSimElt::ParAttr(ELTATTR i)
{

	if (wPar) {
		return parattrArr[i];
	}
	else {
		AfxMessageBox(_T("Element has no parent!"));
		return NULL;
	}
	
}

LPCSTR CStatSimElt::ChiAttr(ELTATTR i)
{

	if (wChi) {
		return chiattrArr[i];
	}
	else {
		AfxMessageBox(_T("Element has no child!"));
		return NULL;
	}
	
}

ELEMENT CStatSimElt::GetParent()
{
	ELTATTR attr = elementID;
	if (wAttr && wPar) {
		short parElt = atoi(parattrArr[attr]);
		return (ELEMENT) parElt;
	}

	else {
		AfxMessageBox(_T("Element has no parent/attributes!"));
		return (ELEMENT) -1;
	}

	
}

ELEMENT CStatSimElt::GetChild()
{
	ELTATTR attr = elementID;

	if (wAttr && wChi) {
		short chiElt = atoi(chiattrArr[attr]);
		return (ELEMENT) chiElt;
	}

	else {
		AfxMessageBox(_T("Element has no child/attributes!"));
		return (ELEMENT) -1;
	}
	
}

UINT CStatSimElt::CrDigit()
{
	ELTATTR attr = digit;
	UINT digit = 0;


	if (wAttr && wPar) {
		digit = (UINT) atoi(parattrArr[attr]);
	}

	else {
		AfxMessageBox(_T("Element has no parent/attributes!"));
	}

	return digit;
	
}

CString CStatSimElt::sSQLConfig()
{
	CString sSQL;

	switch(currElt) {

	case MEM:
		
		//Define index (primary key cannot be established always in this table)
		sSQL.Format(_T( "ALTER TABLE mem_ind \n\
			MODIFY regn tinyint(%d) UNSIGNED ZEROFILL, \n\
			MODIFY prov tinyint(%d) UNSIGNED ZEROFILL, \n\
			MODIFY mun tinyint(%d) UNSIGNED ZEROFILL, \n\
			MODIFY brgy smallint(%d) UNSIGNED ZEROFILL, \n\
			MODIFY purok smallint(%d) UNSIGNED ZEROFILL, \n\
			MODIFY hcn int(%d) UNSIGNED ZEROFILL, \n\
			MODIFY memno int(%d) UNSIGNED ZEROFILL, \n\
			MODIFY memID VARCHAR(%d), \n\
			ADD INDEX memIndex (regn, prov, mun, brgy, purok, hcn, memno, memID);"),
			regn_dig-country_dig,
			prov_dig-regn_dig,
			mun_dig-prov_dig,
			brgy_dig-mun_dig,
			purok_dig-brgy_dig,
			hh_dig-purok_dig,
			mem_dig-hh_dig,
			mem_dig);

		break;

	case HH:
		
		sSQL.Format(_T("ALTER TABLE hh_ind \n\
			MODIFY regn tinyint(%d) UNSIGNED ZEROFILL, \n\
			MODIFY prov tinyint(%d) UNSIGNED ZEROFILL, \n\
			MODIFY mun tinyint(%d) UNSIGNED ZEROFILL, \n\
			MODIFY brgy smallint(%d) UNSIGNED ZEROFILL, \n\
			MODIFY purok smallint(%d) UNSIGNED ZEROFILL, \n\
			MODIFY hcn int(%d) UNSIGNED ZEROFILL, \n\
			MODIFY hhID VARCHAR(%d), \n\
			ADD INDEX hhIndex (regn, prov, mun, brgy, purok, hcn, hhID);"), 
			regn_dig-country_dig,
			prov_dig-regn_dig,
			mun_dig-prov_dig,
			brgy_dig-mun_dig,
			purok_dig-brgy_dig,
			hh_dig-purok_dig,
			hh_dig);
		
		break;

	case PUROK:
		
		sSQL.Format(_T("ALTER TABLE purok \n\
			MODIFY regn tinyint(%d) UNSIGNED ZEROFILL, \n\
			MODIFY prov tinyint(%d) UNSIGNED ZEROFILL, \n\
			MODIFY mun tinyint(%d) UNSIGNED ZEROFILL, \n\
			MODIFY brgy smallint(%d) UNSIGNED ZEROFILL, \n\
			MODIFY purok smallint(%d) UNSIGNED ZEROFILL, \n\
			MODIFY purokID VARCHAR(%d), \n\
			MODIFY PurokName VARCHAR(50), \n\
			ADD INDEX purokIndex (regn, prov, mun, brgy, purok, purokID, PurokName), \n\
			ADD PRIMARY KEY (purokID);"),
			regn_dig-country_dig,
			prov_dig-regn_dig,
			mun_dig-prov_dig,
			brgy_dig-mun_dig,
			purok_dig-brgy_dig,
			purok_dig);

		break;

	case BRGY:
		
		sSQL.Format(_T("ALTER TABLE `brgy` \n\
			MODIFY regn tinyint(%d) UNSIGNED ZEROFILL, \n\
			MODIFY prov tinyint(%d) UNSIGNED ZEROFILL, \n\
			MODIFY mun tinyint(%d) UNSIGNED ZEROFILL, \n\
			MODIFY brgy smallint(%d) UNSIGNED ZEROFILL, \n\
			MODIFY brgyID VARCHAR(9), \n\
			MODIFY Barangay VARCHAR(100), \n\
			MODIFY urb tinyint(1), \n\
			MODIFY brgyPath VARCHAR(40), \n\
			ADD INDEX brgyIndex (regn, prov, mun, brgy, brgyID, Barangay), \n\
			ADD PRIMARY KEY (brgyID);"),
			regn_dig-country_dig,
			prov_dig-regn_dig,
			mun_dig-prov_dig,
			brgy_dig-mun_dig,
			id_dig);

		break;

	case MUN:
		
		sSQL.Format(_T("ALTER TABLE `mun` \n\
			MODIFY regn tinyint(%d) UNSIGNED ZEROFILL, \n\
			MODIFY prov tinyint(%d) UNSIGNED ZEROFILL, \n\
			MODIFY mun tinyint(%d) UNSIGNED ZEROFILL, \n\
			MODIFY munID VARCHAR(%d), \n\
			MODIFY Municipality VARCHAR(50), \n\
			MODIFY urb tinyint(1), \n\
			MODIFY munPath VARCHAR(36), \n\
			ADD INDEX munIndex (regn, prov, mun, munID, Municipality), \n\
			ADD PRIMARY KEY (munID);"),	
			regn_dig-country_dig,
			prov_dig-regn_dig,
			mun_dig-prov_dig,
			id_dig);

		break;

	case PROV:
		
		sSQL.Format(_T("ALTER TABLE `prov` \n\
			MODIFY regn tinyint(%d) UNSIGNED ZEROFILL, \n\
			MODIFY prov tinyint(%d) UNSIGNED ZEROFILL, \n\
			MODIFY provID VARCHAR(%d), \n\
			MODIFY Province VARCHAR(50), \n\
			MODIFY provPath VARCHAR(33), \n\
			ADD INDEX provIndex (regn, prov, provID, Province), \n\
			ADD PRIMARY KEY (provID);"),
			regn_dig-country_dig,
			prov_dig-regn_dig,
			id_dig);

		break;

	case REGN:
		
		sSQL.Format(_T("ALTER TABLE `regn` \n\
			MODIFY country smallint(3) UNSIGNED ZEROFILL, \n\
			MODIFY regn tinyint(%d) UNSIGNED ZEROFILL, \n\
			MODIFY regnID VARCHAR(%d), \n\
			MODIFY Region VARCHAR(50), \n\
			MODIFY regnPath VARCHAR(30), \n\
			ADD INDEX regnIndex (country, regn, regnID, Region), \n\
			ADD PRIMARY KEY (regnID);"),
			regn_dig-country_dig,
			id_dig);

		break;

	case COUNTRY:
		
		sSQL = "ALTER TABLE `country` \n\
			MODIFY country smallint(3) UNSIGNED ZEROFILL, \n\
			MODIFY CountryName VARCHAR(50), \n\
			MODIFY AlphaCode VARCHAR(3), \n\
			ADD INDEX countryIndex (country, CountryName, AlphaCode), \n\
			ADD PRIMARY KEY (country);";	

		break;

	default:

		AfxMessageBox(_T("To be updated!"));
		return _T("");
		break;
	
	}

	return sSQL;

}

void CStatSimElt::DeleteInd()
{
	CString sSQL;

	switch(currElt) {

	case MEM:
		
		sSQL = _T("DROP TABLE IF EXISTS Mem_Ind;");

		break;

	case HH:
		
		sSQL = _T("DROP TABLE IF EXISTS HH_Ind;");
		
		break;

	case PUROK:
		
		sSQL = _T("DROP TABLE IF EXISTS Purok_Ind;");

		break;

	case BRGY:
		
		sSQL = _T("DROP TABLE IF EXISTS Brgy_Ind;");

		break;

	case MUN:
		
		sSQL = _T("DROP TABLE IF EXISTS Mun_Ind;");

		break;

	case PROV:
		
		sSQL = _T("DROP TABLE IF EXISTS Prov_Ind;");

		break;

	case REGN:
		
		sSQL = _T("DROP TABLE IF EXISTS Regn_Ind;");

		break;

	default:

		AfxMessageBox(_T("To be updated!"));

		break;
	
	}

	pEltDB->ExecuteSQL( sSQL );

}

void CStatSimElt::CrHHInd(ELEMENT srcType, LPCSTR sInd, LPCSTR sDenom, 
						  LPCSTR sSrcInd, LPCSTR sSrcDenom,
						  LPCSTR IDF, LPCSTR AggrFcn, int units,
						  bool wHHSuffix, LPCSTR sIndOut, LPCSTR sDenomOut)
{	
	CString sSQL, sE, sEIDFld, sEName, sEDigit,
		sTable;
	
	if (sIndOut!=NULL) {
		sTable.Format(_T("%s_%s"), (CString) Attr(element), (CString) sIndOut);
	}
	else {
		sTable.Format(_T("%s_%s"), (CString) Attr(element), (CString) sInd);
	}
	
	sSQL.Format(_T("DROP TABLE IF EXISTS `%s`;"), (CString) sTable );
	pEltDB->ExecuteSQL( sSQL );

	sE = Attr(element);
	sEIDFld = Attr(IDField);
	sEName = Attr(name);
	sEDigit = Attr(digit);

	sSQL = sSQLHHInd(srcType, sInd, sDenom, sSrcInd, sSrcDenom, IDF, AggrFcn, units, wHHSuffix, sIndOut, sDenomOut);

	//if (sInd=="wDeath05") {
	//	FILE *pFile = fopen(_T("C:\\mycmd.sql"), _T("w"));
	//	fprintf( pFile, ConstChar(sSQL) );
	//	fclose(pFile);
	//}

	pEltDB->ExecuteSQL( sSQL );

	CString sExt; sExt.Format(_T("_%s"), (CString) sInd);

	AddPrimaryKey(sExt, sEIDFld);

}


CString CStatSimElt::CrHHIndCat(ELEMENT srcType, LPCSTR sIndCat, LPCSTR sDenom, BOOL wLabel,
							 LPCSTR sSrcIndCat, LPCSTR sSrcDenom,
							 LPCSTR IDF, LPCSTR sFilter, LPCSTR sWt)
{	
	CString sSQL, sE, sEName, sEDigit,
		sKey, sTable;
	
	sSQL.Format(_T("DROP TABLE IF EXISTS `%s_%s`;"), (CString) Attr(element), (CString) sIndCat );
	pEltDB->ExecuteSQL( sSQL );

	//add index on the variable to join the values
	sSQL.Format(_T("ALTER IGNORE TABLE `%s` ADD INDEX `%sIndex`(`%s`);"), (CString) sSrcIndCat, (CString) sIndCat, (CString) sIndCat);
	pEltDB->ExecuteSQL( sSQL, FALSE );

	sE = Attr(element);
	sKey.Format(_T("`%s`, `%s`"), (CString) Attr(IDField), (CString) sIndCat);
	sTable.Format(_T("%s_%s"), (CString) sE, (CString) sIndCat);
	sEName = Attr(name);
	sEDigit = Attr(digit);

	sSQL = sSQLHHIndCat(srcType, sIndCat, sDenom, sSrcIndCat, sSrcDenom, IDF, sFilter, sWt);
	
	//FILE *pFile = _tfopen(_T("d:\\mysqlhh.sql"), _T("w"));
	//fprintf( pFile, ConstChar(sSQL) );
	//fclose(pFile);

	pEltDB->ExecuteSQL( sSQL );
	
	AddPrimaryKey( ConstChar(sTable), ConstChar(sKey) );

	sSQL.Format(_T("UPDATE `%s` SET `%s_%s`=0 WHERE `%s_%s` IS NULL;"), sTable, (CString) sE, (CString) sIndCat, (CString) sE, (CString) sIndCat);
	pEltDB->ExecuteSQL( sSQL );
	sSQL.Format(_T("UPDATE `%s` SET `%s_%s_prop`=0 WHERE `%s_%s_prop` IS NULL;"), sTable, (CString) sE, (CString) sIndCat, (CString) sE, (CString) sIndCat);
	pEltDB->ExecuteSQL( sSQL );

	return sSQL;

}

void CStatSimElt::CrMemInd(ELEMENT srcType, LPCSTR sInd, LPCSTR sDenom, LPCSTR sMExt, LPCSTR sFExt,
						   LPCSTR sSrcInd, LPCSTR sSrcDenom, BOOL death, 
						   LPCSTR IDF, LPCSTR AggrFcn, int units,
						   LPCSTR sIndOut, LPCSTR sDenomOut, LPCSTR sAugDenom)
{	
	CString sSQL, sE, sEIDFld, sEName, sEDigit,
		sTable;

	if (sIndOut!=NULL) {
		sTable.Format(_T("%s_%s"), (CString) Attr(element), (CString) sIndOut);
	}
	else {
		sTable.Format(_T("%s_%s"), (CString) Attr(element), (CString) sInd);
	}
	
	sSQL.Format(_T("DROP TABLE IF EXISTS `%s`;"), sTable );
	pEltDB->ExecuteSQL( sSQL );

	sE = Attr(element);
	sEIDFld = Attr(IDField);
	sEName = Attr(name);
	sEDigit = Attr(digit);

	sSQL = sSQLMemInd(srcType, sInd, sDenom, sMExt, sFExt,
		sSrcInd, sSrcDenom, death, IDF, AggrFcn, units, sIndOut, sDenomOut, sAugDenom);

	pEltDB->ExecuteSQL( sSQL );

	CString sExt; sExt.Format(_T("_%s"), (CString) sInd);

	AddPrimaryKey(sExt, sEIDFld);
	AddKey(sExt, sEIDFld + _T("(2)"));

}

void CStatSimElt::CrTotPop(long qnrID, CStatSimWnd *pLabel, CStatSimBar *pProgBar)
{

	SetQnrID(qnrID);
	CString sSQL,  sE, sEIDFld, sEName, sEDigit, sTable;
	CStatSimRS *pRS = NULL;
	
	sSQL.Format(_T("DROP TABLE IF EXISTS `%s_TotPop`;"), (CString) Attr(element) );
	pEltDB->ExecuteSQL( sSQL );

	sE = Attr(element);
	sEIDFld = Attr(IDField);
	sEName = Attr(name);
	sEDigit = Attr(digit);
	sTable.Format(_T("%s_TotPop"), sE);

	//TotPop
	sSQL = sSQLTotPop();
	pEltDB->ExecuteSQL( sSQL );
	
	//TotHH
	sSQL = sSQLTotHH();
	if (currElt!=HH) {
		pRS = new CStatSimRS( pEltDB, sSQL );
		pEltDB->InsertRecords(pRS, sTable, pLabel, pProgBar);	
		delete pRS; pRS = NULL;
	}

	if (currElt!=HH) {
		//TotMem
		sSQL = sSQLTotMem();
		pRS = new CStatSimRS( pEltDB, sSQL );
		pEltDB->UpdateRecords( pRS, sTable, Attr(IDField), 
			pLabel, pProgBar);
		delete pRS; pRS = NULL;	
		
		//TotMem Male
		//sSQL.Format(_T("INSERT INTO `%s` %s"), sTable, sSQLTotMem());
		sSQL = sSQLTotMem(_MBCS("1"), _MBCS("Male"));
		pRS = new CStatSimRS( pEltDB, sSQL );
		
		pEltDB->UpdateRecords( pRS, sTable, Attr(IDField), 
			pLabel, pProgBar);
		
		delete pRS; pRS = NULL;
		
		//TotMem Female
		sSQL = sSQLTotMem(_MBCS("2"), _MBCS("Female"));
		pRS = new CStatSimRS( pEltDB, sSQL );
		
		pEltDB->UpdateRecords( pRS, sTable, Attr(IDField), 
			pLabel, pProgBar);
		
		delete pRS; pRS = NULL;
		
		//CString msg;
		//msg.Format(_T("Pucha %d"), m_qnrID);
		//SetQnrID(qnrID);
		
	if (m_qnrID==120110100 || m_qnrID==1020100100 || m_qnrID==1020130100 || m_qnrID==1020130101 || m_qnrID==1020135100 || m_qnrID==1020135200 || m_qnrID==1020135300 || m_qnrID==1020135400 || m_qnrID==1020135500){

		//ofw
		//TotMem - Fow
		sSQL = sSQLTotMem(NULL, NULL, TRUE, TRUE);
		pRS = new CStatSimRS( pEltDB, sSQL );

		//See SQL
		//FILE *pFile = fopen("d:\\mysql.sql", "w");
		//fprintf( pFile, ConstChar(sSQL) );
		//fclose(pFile);

		pEltDB->UpdateRecords( pRS, sTable, Attr(IDField), 
			pLabel, pProgBar);
		
		delete pRS; pRS = NULL;
		
		//TotMem Male
		sSQL = sSQLTotMem(_MBCS("1"), _MBCS("Male"), TRUE, TRUE);
		pRS = new CStatSimRS( pEltDB, sSQL );
		
		pEltDB->UpdateRecords( pRS, sTable, Attr(IDField), 
			pLabel, pProgBar);
		
		delete pRS; pRS = NULL;
		
		//TotMem Female
		sSQL = sSQLTotMem(_MBCS("2"), _MBCS("Female"), TRUE, TRUE);
		pRS = new CStatSimRS( pEltDB, sSQL );
		
		pEltDB->UpdateRecords( pRS, sTable, Attr(IDField), 
			pLabel, pProgBar);
		
		delete pRS; pRS = NULL;
		
		
		//TotMem - Fo
		sSQL = sSQLTotMem(NULL, NULL, TRUE, FALSE);
		pRS = new CStatSimRS( pEltDB, sSQL );
		
		pEltDB->UpdateRecords( pRS, sTable, Attr(IDField), 
			pLabel, pProgBar);
		
		delete pRS; pRS = NULL;
		
		//TotMem Male
		sSQL = sSQLTotMem(_MBCS("1"), _MBCS("Male"), TRUE, FALSE);
		pRS = new CStatSimRS( pEltDB, sSQL );
		
		pEltDB->UpdateRecords( pRS, sTable, Attr(IDField), 
			pLabel, pProgBar);
		
		delete pRS; pRS = NULL;
		
		//TotMem Female
		sSQL = sSQLTotMem(_MBCS("2"), _MBCS("Female"), TRUE, FALSE);
		pRS = new CStatSimRS( pEltDB, sSQL );
		
		pEltDB->UpdateRecords( pRS, sTable, Attr(IDField), 
			pLabel, pProgBar);
		
		delete pRS; pRS = NULL;
	}	
	}
	else {
		//TotMem
		
		//FILE* pFile = _tfopen(_T("c:\\sqltotmem.sql"), _T("w"));
		//_ftprintf(pFile, sSQLTotMem());
		//fclose(pFile);

		pEltDB->InsertFromSQL(sSQLTotMem(), sTable);	

		//pRS = new CStatSimRS( pEltDB, sSQL );
		//pEltDB->InsertRecords(pRS, sTable, pLabel, pProgBar, FALSE, TRUE);	
		//pEltDB->InsertRecords(pRS, sTable, NULL, NULL, FALSE, TRUE);	
		//delete pRS; pRS = NULL;	

		//TotMem Male	
		sSQL.Format(_T("CREATE TABLE `%s_temp` %s"), sTable, sSQLTotMem(_MBCS("1"), _MBCS("Male")) );	
		pEltDB->ExecuteSQL(sSQL);
		sSQL.Format(_T("ALTER TABLE `%s_temp` MODIFY COLUMN `hhID` varchar(17) NOT NULL, ADD INDEX `hhindex`(`%s`);"), sTable, (CString) Attr(IDField));
		pEltDB->ExecuteSQL(sSQL);

		pEltDB->UpdateRecords(sTable + _T("_temp"), sTable, Attr(IDField) );
		sSQL.Format(_T("DROP TABLE IF EXISTS `%s_temp`;"), sTable);
		pEltDB->ExecuteSQL(sSQL);
		
		//TotMem Female	
		sSQL.Format(_T("CREATE TABLE `%s_temp` %s"), sTable, sSQLTotMem(_MBCS("2"), _MBCS("Female")) );	
		pEltDB->ExecuteSQL(sSQL);
		sSQL.Format(_T("ALTER TABLE `%s_temp` MODIFY COLUMN `hhID` varchar(17) NOT NULL, ADD INDEX `hhindex`(`%s`);"), sTable, (CString) Attr(IDField));
		pEltDB->ExecuteSQL(sSQL);

		pEltDB->UpdateRecords(sTable + _T("_temp"), sTable, Attr(IDField) );
		sSQL.Format(_T("DROP TABLE IF EXISTS `%s_temp`;"), sTable);
		pEltDB->ExecuteSQL(sSQL);


	if (m_qnrID==120110100 || m_qnrID==1020100100 || m_qnrID==1020130100 || m_qnrID==1020130101 || m_qnrID==1020135100 || m_qnrID==1020135200 || m_qnrID==1020135300 || m_qnrID==1020135400 || m_qnrID==1020135500) {

		//ofw
		//TotMem - Fow
		sSQL.Format(_T("CREATE TABLE `%s_temp` %s"), sTable, sSQLTotMem(NULL, NULL, TRUE, TRUE) );	
		pEltDB->ExecuteSQL(sSQL);
		sSQL.Format(_T("ALTER TABLE `%s_temp` MODIFY COLUMN `hhID` varchar(17) NOT NULL, ADD INDEX `hhindex`(`%s`);"), sTable, (CString) Attr(IDField));
		pEltDB->ExecuteSQL(sSQL);
		
		pEltDB->UpdateRecords(sTable + _T("_temp"), sTable, Attr(IDField) );
		sSQL.Format(_T("DROP TABLE IF EXISTS `%s_temp`;"), sTable);
		pEltDB->ExecuteSQL(sSQL);
		
		//TotMem Male
		sSQL.Format(_T("CREATE TABLE `%s_temp` %s"), sTable, sSQLTotMem(_MBCS("1"), _MBCS("Male"), TRUE, TRUE) );	
		pEltDB->ExecuteSQL(sSQL);
		sSQL.Format(_T("ALTER TABLE `%s_temp` MODIFY COLUMN `hhID` varchar(17) NOT NULL, ADD INDEX `hhindex`(`%s`);"), sTable, (CString) Attr(IDField));
		pEltDB->ExecuteSQL(sSQL);
		
		pEltDB->UpdateRecords(sTable + _T("_temp"), sTable, Attr(IDField) );
		sSQL.Format(_T("DROP TABLE IF EXISTS `%s_temp`;"), sTable);
		pEltDB->ExecuteSQL(sSQL);
		
		//TotMem Female
		sSQL = sSQLTotMem(_MBCS("2"), _MBCS("Female"), TRUE, TRUE);
		sSQL.Format(_T("CREATE TABLE `%s_temp` %s"), sTable, sSQLTotMem(_MBCS("2"), _MBCS("Female"), TRUE, TRUE) );	
		pEltDB->ExecuteSQL(sSQL);
		sSQL.Format(_T("ALTER TABLE `%s_temp` MODIFY COLUMN `hhID` varchar(17) NOT NULL, ADD INDEX `hhindex`(`%s`);"), sTable, (CString) Attr(IDField));
		pEltDB->ExecuteSQL(sSQL);
		
		pEltDB->UpdateRecords(sTable + _T("_temp"), sTable, Attr(IDField) );
		sSQL.Format(_T("DROP TABLE IF EXISTS `%s_temp`;"), sTable);
		pEltDB->ExecuteSQL(sSQL);
		
		
		//TotMem - Fo
		sSQL.Format(_T("CREATE TABLE `%s_temp` %s"), sTable, sSQLTotMem(NULL, NULL, TRUE, FALSE) );	
		pEltDB->ExecuteSQL(sSQL);
		sSQL.Format(_T("ALTER TABLE `%s_temp` MODIFY COLUMN `hhID` varchar(17) NOT NULL, ADD INDEX `hhindex`(`%s`);"), sTable, (CString) Attr(IDField));
		pEltDB->ExecuteSQL(sSQL);
		
		pEltDB->UpdateRecords(sTable + _T("_temp"), sTable, Attr(IDField) );
		sSQL.Format(_T("DROP TABLE IF EXISTS `%s_temp`;"), sTable);
		pEltDB->ExecuteSQL(sSQL);
		
		//TotMem Male
		sSQL.Format(_T("CREATE TABLE `%s_temp` %s"), sTable, sSQLTotMem(_MBCS("1"), _MBCS("Male"), TRUE, FALSE) );	
		pEltDB->ExecuteSQL(sSQL);
		sSQL.Format(_T("ALTER TABLE `%s_temp` MODIFY COLUMN `hhID` varchar(17) NOT NULL, ADD INDEX `hhindex`(`%s`);"), sTable, (CString) Attr(IDField));
		pEltDB->ExecuteSQL(sSQL);
		
		pEltDB->UpdateRecords(sTable + _T("_temp"), sTable, Attr(IDField) );
		sSQL.Format(_T("DROP TABLE IF EXISTS `%s_temp`;"), sTable);
		pEltDB->ExecuteSQL(sSQL);
		
		//TotMem Female
		sSQL = sSQLTotMem(_MBCS("2"), _MBCS("Female"), TRUE, TRUE);
		sSQL.Format(_T("CREATE TABLE `%s_temp` %s"), sTable, sSQLTotMem(_MBCS("2"), _MBCS("Female"), TRUE, FALSE) );	
		pEltDB->ExecuteSQL(sSQL);
		sSQL.Format(_T("ALTER TABLE `%s_temp` MODIFY COLUMN `hhID` varchar(17) NOT NULL, ADD INDEX `hhindex`(`%s`);"), sTable, (CString) Attr(IDField));
		pEltDB->ExecuteSQL(sSQL);
		
		pEltDB->UpdateRecords(sTable + _T("_temp"), sTable, Attr(IDField) );
		sSQL.Format(_T("DROP TABLE IF EXISTS `%s_temp`;"), sTable);
		pEltDB->ExecuteSQL(sSQL);
	}
	}

}

void CStatSimElt::CrDemog()
{
	CString sSQL,  sE, sEIDFld, sEName, sEDigit;
	
	sSQL.Format(_T("DROP TABLE IF EXISTS `%s_Demog`;"), (CString) Attr(element) );
	pEltDB->ExecuteSQL( sSQL );

	sE = Attr(element);
	sEIDFld = Attr(IDField);
	sEName = Attr(name);
	sEDigit = Attr(digit);

	sSQL = sSQLDemog();

	pEltDB->ExecuteSQL( sSQL );

	AddPrimaryKey( (CString) "_Demog", sEIDFld);

}

CString CStatSimElt::sSQLDemog()
{
	CString sSQL;

	switch(currElt) {

	case MEM:
				
		sSQL = _T("");		

		break;

	case HH:
		
		sSQL = _T("");		

		break;

	case PUROK:
		
		sSQL = "CREATE TABLE IF NOT EXISTS `Purok_Demog` \n\
			SELECT  `purok`.`purokID` AS `purokID`, `regn`.`Region` AS `Region`, `prov`.`Province` AS `Province`, `mun`.`Municipality` AS `Municipality`, `brgy`.`Barangay` AS `Barangay`, `purok`.`PurokName` AS `PurokName`, \n\
			SUM(`Mem_Ind`.`mem01`) AS `purok_mem01`, SUM(`Mem_Ind`.`mem01_female`) AS `purok_mem01_Female`, SUM(`Mem_Ind`.`mem01_male`) AS `purok_mem01_Male`, \n\
			SUM(`Mem_Ind`.`mem14`) AS `purok_mem14`, SUM(`Mem_Ind`.`mem14_female`) AS `purok_mem14_Female`, SUM(`Mem_Ind`.`mem14_male`) AS `purok_mem14_Male`, \n\
			SUM(`Mem_Ind`.`mem04`) AS `purok_mem04`, SUM(`Mem_Ind`.`mem04_female`) AS `purok_mem04_Female`, SUM(`Mem_Ind`.`mem04_male`) AS `purok_mem04_Male`, \n\
			SUM(`Mem_Ind`.`mem05`) AS `purok_mem05`, SUM(`Mem_Ind`.`mem05_female`) AS `purok_mem05_Female`, SUM(`Mem_Ind`.`mem05_male`) AS `purok_mem05_Male`, \n\
			SUM(`Mem_Ind`.`mem612`) AS `purok_mem612`, SUM(`Mem_Ind`.`mem612_female`) AS `purok_mem612_Female`, SUM(`Mem_Ind`.`mem612_male`) AS `purok_mem612_Male`, \n\
			SUM(`Mem_Ind`.`mem611`) AS `purok_mem611`, SUM(`Mem_Ind`.`mem611_female`) AS `purok_mem611_Female`, SUM(`Mem_Ind`.`mem611_male`) AS `purok_mem611_Male`, \n\
			SUM(`Mem_Ind`.`mem1316`) AS `purok_mem1316`, SUM(`Mem_Ind`.`mem1316_female`) AS `purok_mem1316_Female`, SUM(`Mem_Ind`.`mem1316_male`) AS `purok_mem1316_Male`, \n\
			SUM(`Mem_Ind`.`mem1215`) AS `purok_mem1215`, SUM(`Mem_Ind`.`mem1215_female`) AS `purok_mem1215_Female`, SUM(`Mem_Ind`.`mem1215_male`) AS `purok_mem1215_Male`, \n\
			SUM(`Mem_Ind`.`mem615`) AS `purok_mem615`, SUM(`Mem_Ind`.`mem615_female`) AS `purok_mem615_Female`, SUM(`Mem_Ind`.`mem615_male`) AS `purok_mem615_Male`, \n\
			SUM(`Mem_Ind`.`mem616`) AS `purok_mem616`, SUM(`Mem_Ind`.`mem616_female`) AS `purok_mem616_Female`, SUM(`Mem_Ind`.`mem616_male`) AS `purok_mem616_Male`, \n\
			SUM(`Mem_Ind`.`mem10ab`) AS `purok_mem10ab`, SUM(`Mem_Ind`.`mem10ab_female`) AS `purok_mem10ab_Female`, \n\
			SUM(`Mem_Ind`.`mem10ab_male`) AS `purok_mem10ab_Male`, SUM(`Mem_Ind`.`mem15ab`) AS `purok_mem15ab`, SUM(`Mem_Ind`.`mem15ab_female`) AS `purok_mem15ab_Female`, SUM(`Mem_Ind`.`mem15ab_male`) AS `purok_mem15ab_Male`,\n\
			SUM(`Mem_Ind`.`mem018`) AS `purok_mem018`, SUM(`Mem_Ind`.`mem018_female`) AS `purok_mem018_Female`, SUM(`Mem_Ind`.`mem018_male`) AS `purok_mem018_Male`, \n\
			SUM(`Mem_Ind`.`mem18ab`) AS `purok_mem18ab`, SUM(`Mem_Ind`.`mem18ab_female`) AS `purok_mem18ab_Female`, SUM(`Mem_Ind`.`mem18ab_male`) AS `purok_mem18ab_Male`, \n\
			SUM(`Mem_Ind`.`mem17ab`) AS `purok_mem17ab`, SUM(`Mem_Ind`.`mem17ab_female`) AS `purok_mem17ab_Female`, SUM(`Mem_Ind`.`mem17ab_male`) AS `purok_mem17ab_Male`, \n\
			SUM(`Mem_Ind`.`mem1721`) AS `purok_mem1721`, SUM(`Mem_Ind`.`mem1721_female`) AS `purok_mem1721_Female`, SUM(`Mem_Ind`.`mem1721_male`) AS `purok_mem1721_Male`, SUM(`Mem_Ind`.`mem60ab`) AS `purok_mem60ab`, SUM(`Mem_Ind`.`mem60ab_female`) AS `purok_mem60ab_Female`, SUM(`Mem_Ind`.`mem60ab_male`) AS `purok_mem60ab_Male`, SUM(`Mem_Ind`.`labfor`) AS `purok_labfor`, SUM(`Mem_Ind`.`labfor_female`) AS `purok_labfor_Female`, SUM(`Mem_Ind`.`labfor_male`) AS `purok_labfor_Male`  \n\
			FROM `regn`, `prov`, `mun`, `brgy`, `purok`, `mem_ind` \n\
			WHERE (LEFT(`purok`.`purokID`, 11) = LEFT(`mem_ind`.`memID`, 11)) AND (LEFT(`purok`.`purokID`, 2) = LEFT(`regn`.`regnID`, 2)) AND (LEFT(`purok`.`purokID`, 4) = LEFT(`prov`.`provID`, 4)) AND (LEFT(`purok`.`purokID`, 6) = LEFT(`mun`.`munID`, 6)) AND (LEFT(`purok`.`purokID`, 9) = LEFT(`brgy`.`brgyID`, 9)) \n\
			GROUP BY `purok`.`purokID` \n\
			;";	

		break;

	case BRGY:
		
		sSQL = "CREATE TABLE IF NOT EXISTS `Brgy_Demog` \n\
			SELECT  `brgy`.`brgyID` AS `brgyID`, `regn`.`Region` AS `Region`, `prov`.`Province` AS `Province`, `mun`.`Municipality` AS `Municipality`, `brgy`.`Barangay` AS `Barangay`, \n\
			SUM(`Mem_Ind`.`mem01`) AS `brgy_mem01`, SUM(`Mem_Ind`.`mem01_female`) AS `brgy_mem01_Female`, SUM(`Mem_Ind`.`mem01_male`) AS `brgy_mem01_Male`, \n\
			SUM(`Mem_Ind`.`mem14`) AS `brgy_mem14`, SUM(`Mem_Ind`.`mem14_female`) AS `brgy_mem14_Female`, SUM(`Mem_Ind`.`mem14_male`) AS `brgy_mem14_Male`, \n\
			SUM(`Mem_Ind`.`mem04`) AS `brgy_mem04`, SUM(`Mem_Ind`.`mem04_female`) AS `brgy_mem04_Female`, SUM(`Mem_Ind`.`mem04_male`) AS `brgy_mem04_Male`, \n\
			SUM(`Mem_Ind`.`mem05`) AS `brgy_mem05`, SUM(`Mem_Ind`.`mem05_female`) AS `brgy_mem05_Female`, SUM(`Mem_Ind`.`mem05_male`) AS `brgy_mem05_Male`, \n\
			SUM(`Mem_Ind`.`mem612`) AS `brgy_mem612`, SUM(`Mem_Ind`.`mem612_female`) AS `brgy_mem612_Female`, SUM(`Mem_Ind`.`mem612_male`) AS `brgy_mem612_Male`, \n\
			SUM(`Mem_Ind`.`mem611`) AS `brgy_mem611`, SUM(`Mem_Ind`.`mem611_female`) AS `brgy_mem611_Female`, SUM(`Mem_Ind`.`mem611_male`) AS `brgy_mem611_Male`, \n\
			SUM(`Mem_Ind`.`mem1316`) AS `brgy_mem1316`, SUM(`Mem_Ind`.`mem1316_female`) AS `brgy_mem1316_Female`, SUM(`Mem_Ind`.`mem1316_male`) AS `brgy_mem1316_Male`, \n\
			SUM(`Mem_Ind`.`mem1215`) AS `brgy_mem1215`, SUM(`Mem_Ind`.`mem1215_female`) AS `brgy_mem1215_Female`, SUM(`Mem_Ind`.`mem1215_male`) AS `brgy_mem1215_Male`, \n\
			SUM(`Mem_Ind`.`mem615`) AS `brgy_mem615`, SUM(`Mem_Ind`.`mem615_female`) AS `brgy_mem615_Female`, SUM(`Mem_Ind`.`mem615_male`) AS `brgy_mem615_Male`, \n\
			SUM(`Mem_Ind`.`mem616`) AS `brgy_mem616`, SUM(`Mem_Ind`.`mem616_female`) AS `brgy_mem616_Female`, SUM(`Mem_Ind`.`mem616_male`) AS `brgy_mem616_Male`, \n\
			SUM(`Mem_Ind`.`mem10ab`) AS `brgy_mem10ab`, SUM(`Mem_Ind`.`mem10ab_female`) AS `brgy_mem10ab_Female`, \n\
			SUM(`Mem_Ind`.`mem10ab_male`) AS `brgy_mem10ab_Male`, SUM(`Mem_Ind`.`mem15ab`) AS `brgy_mem15ab`, SUM(`Mem_Ind`.`mem15ab_female`) AS `brgy_mem15ab_Female`, SUM(`Mem_Ind`.`mem15ab_male`) AS `brgy_mem15ab_Male`,\n\
			SUM(`Mem_Ind`.`mem018`) AS `brgy_mem018`, SUM(`Mem_Ind`.`mem018_female`) AS `brgy_mem018_Female`, SUM(`Mem_Ind`.`mem018_male`) AS `brgy_mem018_Male`, \n\
			SUM(`Mem_Ind`.`mem18ab`) AS `brgy_mem18ab`, SUM(`Mem_Ind`.`mem18ab_female`) AS `brgy_mem18ab_Female`, SUM(`Mem_Ind`.`mem18ab_male`) AS `brgy_mem18ab_Male`, \n\
			SUM(`Mem_Ind`.`mem17ab`) AS `brgy_mem17ab`, SUM(`Mem_Ind`.`mem17ab_female`) AS `brgy_mem17ab_Female`, SUM(`Mem_Ind`.`mem17ab_male`) AS `brgy_mem17ab_Male`, \n\
			SUM(`Mem_Ind`.`mem1721`) AS `brgy_mem1721`, SUM(`Mem_Ind`.`mem1721_female`) AS `brgy_mem1721_Female`, SUM(`Mem_Ind`.`mem1721_male`) AS `brgy_mem1721_Male`, SUM(`Mem_Ind`.`mem60ab`) AS `brgy_mem60ab`, SUM(`Mem_Ind`.`mem60ab_female`) AS `brgy_mem60ab_Female`, SUM(`Mem_Ind`.`mem60ab_male`) AS `brgy_mem60ab_Male`, SUM(`Mem_Ind`.`labfor`) AS `brgy_labfor`, SUM(`Mem_Ind`.`labfor_female`) AS `brgy_labfor_Female`, SUM(`Mem_Ind`.`labfor_male`) AS `brgy_labfor_Male`  \n\
			FROM `regn`, `prov`, `mun`, `brgy`, `mem_ind` \n\
			WHERE (LEFT(`brgy`.`brgyID`, 9) = LEFT(`mem_ind`.`memID`, 9)) AND (LEFT(`brgy`.`brgyID`, 2) = LEFT(`regn`.`regnID`, 2)) AND (LEFT(`brgy`.`brgyID`, 4) = LEFT(`prov`.`provID`, 4)) AND (LEFT(`brgy`.`brgyID`, 6) = LEFT(`mun`.`munID`, 6)) \n\
			GROUP BY `brgy`.`brgyID` \n\
			;";	

		break;

	case MUN:
		
		sSQL = "CREATE TABLE IF NOT EXISTS `Mun_Demog` \n\
			SELECT  `mun`.`munID` AS `munID`, `regn`.`Region` AS `Region`, `prov`.`Province` AS `Province`, `mun`.`Municipality` AS `Municipality`, \n\
			SUM(`Mem_Ind`.`mem01`) AS `mun_mem01`, SUM(`Mem_Ind`.`mem01_female`) AS `mun_mem01_Female`, SUM(`Mem_Ind`.`mem01_male`) AS `mun_mem01_Male`, \n\
			SUM(`Mem_Ind`.`mem14`) AS `mun_mem14`, SUM(`Mem_Ind`.`mem14_female`) AS `mun_mem14_Female`, SUM(`Mem_Ind`.`mem14_male`) AS `mun_mem14_Male`, \n\
			SUM(`Mem_Ind`.`mem04`) AS `mun_mem04`, SUM(`Mem_Ind`.`mem04_female`) AS `mun_mem04_Female`, SUM(`Mem_Ind`.`mem04_male`) AS `mun_mem04_Male`, \n\
			SUM(`Mem_Ind`.`mem05`) AS `mun_mem05`, SUM(`Mem_Ind`.`mem05_female`) AS `mun_mem05_Female`, SUM(`Mem_Ind`.`mem05_male`) AS `mun_mem05_Male`, \n\
			SUM(`Mem_Ind`.`mem612`) AS `mun_mem612`, SUM(`Mem_Ind`.`mem612_female`) AS `mun_mem612_Female`, SUM(`Mem_Ind`.`mem612_male`) AS `mun_mem612_Male`, \n\
			SUM(`Mem_Ind`.`mem611`) AS `mun_mem611`, SUM(`Mem_Ind`.`mem611_female`) AS `mun_mem611_Female`, SUM(`Mem_Ind`.`mem611_male`) AS `mun_mem611_Male`, \n\
			SUM(`Mem_Ind`.`mem1316`) AS `mun_mem1316`, SUM(`Mem_Ind`.`mem1316_female`) AS `mun_mem1316_Female`, SUM(`Mem_Ind`.`mem1316_male`) AS `mun_mem1316_Male`, \n\
			SUM(`Mem_Ind`.`mem1215`) AS `mun_mem1215`, SUM(`Mem_Ind`.`mem1215_female`) AS `mun_mem1215_Female`, SUM(`Mem_Ind`.`mem1215_male`) AS `mun_mem1215_Male`, \n\
			SUM(`Mem_Ind`.`mem615`) AS `mun_mem615`, SUM(`Mem_Ind`.`mem615_female`) AS `mun_mem615_Female`, SUM(`Mem_Ind`.`mem615_male`) AS `mun_mem615_Male`, \n\
			SUM(`Mem_Ind`.`mem616`) AS `mun_mem616`, SUM(`Mem_Ind`.`mem616_female`) AS `mun_mem616_Female`, SUM(`Mem_Ind`.`mem616_male`) AS `mun_mem616_Male`, \n\
			SUM(`Mem_Ind`.`mem10ab`) AS `mun_mem10ab`, SUM(`Mem_Ind`.`mem10ab_female`) AS `mun_mem10ab_Female`, \n\
			SUM(`Mem_Ind`.`mem10ab_male`) AS `mun_mem10ab_Male`, SUM(`Mem_Ind`.`mem15ab`) AS `mun_mem15ab`, SUM(`Mem_Ind`.`mem15ab_female`) AS `mun_mem15ab_Female`, SUM(`Mem_Ind`.`mem15ab_male`) AS `mun_mem15ab_Male`,\n\
			SUM(`Mem_Ind`.`mem018`) AS `mun_mem018`, SUM(`Mem_Ind`.`mem018_female`) AS `mun_mem018_Female`, SUM(`Mem_Ind`.`mem018_male`) AS `mun_mem018_Male`, \n\
			SUM(`Mem_Ind`.`mem18ab`) AS `mun_mem18ab`, SUM(`Mem_Ind`.`mem18ab_female`) AS `mun_mem18ab_Female`, SUM(`Mem_Ind`.`mem18ab_male`) AS `mun_mem18ab_Male`, \n\
			SUM(`Mem_Ind`.`mem17ab`) AS `mun_mem17ab`, SUM(`Mem_Ind`.`mem17ab_female`) AS `mun_mem17ab_Female`, SUM(`Mem_Ind`.`mem17ab_male`) AS `mun_mem17ab_Male`, \n\
			SUM(`Mem_Ind`.`mem1721`) AS `mun_mem1721`, SUM(`Mem_Ind`.`mem1721_female`) AS `mun_mem1721_Female`, SUM(`Mem_Ind`.`mem1721_male`) AS `mun_mem1721_Male`, SUM(`Mem_Ind`.`mem60ab`) AS `mun_mem60ab`, SUM(`Mem_Ind`.`mem60ab_female`) AS `mun_mem60ab_Female`, SUM(`Mem_Ind`.`mem60ab_male`) AS `mun_mem60ab_Male`, SUM(`Mem_Ind`.`labfor`) AS `mun_labfor`, SUM(`Mem_Ind`.`labfor_female`) AS `mun_labfor_Female`, SUM(`Mem_Ind`.`labfor_male`) AS `mun_labfor_Male`  \n\
			FROM `regn`, `prov`, `mun`, `mem_ind` \n\
			WHERE (LEFT(`mun`.`munID`, 6) = LEFT(`mem_ind`.`memID`, 6)) AND (LEFT(`mun`.`munID`, 2) = LEFT(`regn`.`regnID`, 2)) AND (LEFT(`mun`.`munID`, 4) = LEFT(`prov`.`provID`, 4)) \n\
			GROUP BY `mun`.`munID` \n\
			;";	

		break;

	case PROV:
		
		sSQL = "CREATE TABLE IF NOT EXISTS `Prov_Demog` \n\
			SELECT  `prov`.`provID` AS `provID`, `regn`.`Region` AS `Region`, `prov`.`Province` AS `Province`, \n\
			SUM(`Mem_Ind`.`mem01`) AS `prov_mem01`, SUM(`Mem_Ind`.`mem01_female`) AS `prov_mem01_Female`, SUM(`Mem_Ind`.`mem01_male`) AS `prov_mem01_Male`, \n\
			SUM(`Mem_Ind`.`mem14`) AS `prov_mem14`, SUM(`Mem_Ind`.`mem14_female`) AS `prov_mem14_Female`, SUM(`Mem_Ind`.`mem14_male`) AS `prov_mem14_Male`, \n\
			SUM(`Mem_Ind`.`mem04`) AS `prov_mem04`, SUM(`Mem_Ind`.`mem04_female`) AS `prov_mem04_Female`, SUM(`Mem_Ind`.`mem04_male`) AS `prov_mem04_Male`, \n\
			SUM(`Mem_Ind`.`mem05`) AS `prov_mem05`, SUM(`Mem_Ind`.`mem05_female`) AS `prov_mem05_Female`, SUM(`Mem_Ind`.`mem05_male`) AS `prov_mem05_Male`, \n\
			SUM(`Mem_Ind`.`mem612`) AS `prov_mem612`, SUM(`Mem_Ind`.`mem612_female`) AS `prov_mem612_Female`, SUM(`Mem_Ind`.`mem612_male`) AS `prov_mem612_Male`, \n\
			SUM(`Mem_Ind`.`mem611`) AS `prov_mem611`, SUM(`Mem_Ind`.`mem611_female`) AS `prov_mem611_Female`, SUM(`Mem_Ind`.`mem611_male`) AS `prov_mem611_Male`, \n\
			SUM(`Mem_Ind`.`mem1316`) AS `prov_mem1316`, SUM(`Mem_Ind`.`mem1316_female`) AS `prov_mem1316_Female`, SUM(`Mem_Ind`.`mem1316_male`) AS `prov_mem1316_Male`, \n\
			SUM(`Mem_Ind`.`mem1215`) AS `prov_mem1215`, SUM(`Mem_Ind`.`mem1215_female`) AS `prov_mem1215_Female`, SUM(`Mem_Ind`.`mem1215_male`) AS `prov_mem1215_Male`, \n\
			SUM(`Mem_Ind`.`mem615`) AS `prov_mem615`, SUM(`Mem_Ind`.`mem615_female`) AS `prov_mem615_Female`, SUM(`Mem_Ind`.`mem615_male`) AS `prov_mem615_Male`, \n\
			SUM(`Mem_Ind`.`mem616`) AS `prov_mem616`, SUM(`Mem_Ind`.`mem616_female`) AS `prov_mem616_Female`, SUM(`Mem_Ind`.`mem616_male`) AS `prov_mem616_Male`, \n\
			SUM(`Mem_Ind`.`mem10ab`) AS `prov_mem10ab`, SUM(`Mem_Ind`.`mem10ab_female`) AS `prov_mem10ab_Female`, \n\
			SUM(`Mem_Ind`.`mem10ab_male`) AS `prov_mem10ab_Male`, SUM(`Mem_Ind`.`mem15ab`) AS `prov_mem15ab`, SUM(`Mem_Ind`.`mem15ab_female`) AS `prov_mem15ab_Female`, SUM(`Mem_Ind`.`mem15ab_male`) AS `prov_mem15ab_Male`,\n\
			SUM(`Mem_Ind`.`mem018`) AS `prov_mem018`, SUM(`Mem_Ind`.`mem018_female`) AS `prov_mem018_Female`, SUM(`Mem_Ind`.`mem018_male`) AS `prov_mem018_Male`, \n\
			SUM(`Mem_Ind`.`mem18ab`) AS `prov_mem18ab`, SUM(`Mem_Ind`.`mem18ab_female`) AS `prov_mem18ab_Female`, SUM(`Mem_Ind`.`mem18ab_male`) AS `prov_mem18ab_Male`, \n\
			SUM(`Mem_Ind`.`mem17ab`) AS `prov_mem17ab`, SUM(`Mem_Ind`.`mem17ab_female`) AS `prov_mem17ab_Female`, SUM(`Mem_Ind`.`mem17ab_male`) AS `prov_mem17ab_Male`, \n\
			SUM(`Mem_Ind`.`mem1721`) AS `prov_mem1721`, SUM(`Mem_Ind`.`mem1721_female`) AS `prov_mem1721_Female`, SUM(`Mem_Ind`.`mem1721_male`) AS `prov_mem1721_Male`, SUM(`Mem_Ind`.`mem60ab`) AS `prov_mem60ab`, SUM(`Mem_Ind`.`mem60ab_female`) AS `prov_mem60ab_Female`, SUM(`Mem_Ind`.`mem60ab_male`) AS `prov_mem60ab_Male`, SUM(`Mem_Ind`.`labfor`) AS `prov_labfor`, SUM(`Mem_Ind`.`labfor_female`) AS `prov_labfor_Female`, SUM(`Mem_Ind`.`labfor_male`) AS `prov_labfor_Male`  \n\
			FROM `regn`, `prov`, `mem_ind` \n\
			WHERE (LEFT(`prov`.`provID`, 4) = LEFT(`mem_ind`.`memID`, 4)) AND (LEFT(`prov`.`provID`, 2) = LEFT(`regn`.`regnID`, 2)) \n\
			GROUP BY `prov`.`provID` \n\
			;";	

		break;

	case REGN:
		
		sSQL = "CREATE TABLE IF NOT EXISTS `Regn_Demog` \n\
			SELECT  `regn`.`regnID` AS `regnID`, `regn`.`Region` AS `Region`, \n\
			SUM(`Mem_Ind`.`mem01`) AS `regn_mem01`, SUM(`Mem_Ind`.`mem01_female`) AS `regn_mem01_Female`, SUM(`Mem_Ind`.`mem01_male`) AS `regn_mem01_Male`, \n\
			SUM(`Mem_Ind`.`mem14`) AS `regn_mem14`, SUM(`Mem_Ind`.`mem14_female`) AS `regn_mem14_Female`, SUM(`Mem_Ind`.`mem14_male`) AS `regn_mem14_Male`, \n\
			SUM(`Mem_Ind`.`mem04`) AS `regn_mem04`, SUM(`Mem_Ind`.`mem04_female`) AS `regn_mem04_Female`, SUM(`Mem_Ind`.`mem04_male`) AS `regn_mem04_Male`, \n\
			SUM(`Mem_Ind`.`mem05`) AS `regn_mem05`, SUM(`Mem_Ind`.`mem05_female`) AS `regn_mem05_Female`, SUM(`Mem_Ind`.`mem05_male`) AS `regn_mem05_Male`, \n\
			SUM(`Mem_Ind`.`mem612`) AS `regn_mem612`, SUM(`Mem_Ind`.`mem612_female`) AS `regn_mem612_Female`, SUM(`Mem_Ind`.`mem612_male`) AS `regn_mem612_Male`, \n\
			SUM(`Mem_Ind`.`mem611`) AS `regn_mem611`, SUM(`Mem_Ind`.`mem611_female`) AS `regn_mem611_Female`, SUM(`Mem_Ind`.`mem611_male`) AS `regn_mem611_Male`, \n\
			SUM(`Mem_Ind`.`mem1316`) AS `regn_mem1316`, SUM(`Mem_Ind`.`mem1316_female`) AS `regn_mem1316_Female`, SUM(`Mem_Ind`.`mem1316_male`) AS `regn_mem1316_Male`, \n\
			SUM(`Mem_Ind`.`mem1215`) AS `regn_mem1215`, SUM(`Mem_Ind`.`mem1215_female`) AS `regn_mem1215_Female`, SUM(`Mem_Ind`.`mem1215_male`) AS `regn_mem1215_Male`, \n\
			SUM(`Mem_Ind`.`mem615`) AS `regn_mem615`, SUM(`Mem_Ind`.`mem615_female`) AS `regn_mem615_Female`, SUM(`Mem_Ind`.`mem615_male`) AS `regn_mem615_Male`, \n\
			SUM(`Mem_Ind`.`mem616`) AS `regn_mem616`, SUM(`Mem_Ind`.`mem616_female`) AS `regn_mem616_Female`, SUM(`Mem_Ind`.`mem616_male`) AS `regn_mem616_Male`, \n\
			SUM(`Mem_Ind`.`mem10ab`) AS `regn_mem10ab`, SUM(`Mem_Ind`.`mem10ab_female`) AS `regn_mem10ab_Female`, \n\
			SUM(`Mem_Ind`.`mem10ab_male`) AS `regn_mem10ab_Male`, SUM(`Mem_Ind`.`mem15ab`) AS `regn_mem15ab`, SUM(`Mem_Ind`.`mem15ab_female`) AS `regn_mem15ab_Female`, SUM(`Mem_Ind`.`mem15ab_male`) AS `regn_mem15ab_Male`,\n\
			SUM(`Mem_Ind`.`mem018`) AS `regn_mem018`, SUM(`Mem_Ind`.`mem018_female`) AS `regn_mem018_Female`, SUM(`Mem_Ind`.`mem018_male`) AS `regn_mem018_Male`, \n\
			SUM(`Mem_Ind`.`mem18ab`) AS `regn_mem18ab`, SUM(`Mem_Ind`.`mem18ab_female`) AS `regn_mem18ab_Female`, SUM(`Mem_Ind`.`mem18ab_male`) AS `regn_mem18ab_Male`, \n\
			SUM(`Mem_Ind`.`mem17ab`) AS `regn_mem17ab`, SUM(`Mem_Ind`.`mem17ab_female`) AS `regn_mem17ab_Female`, SUM(`Mem_Ind`.`mem17ab_male`) AS `regn_mem17ab_Male`, \n\
			SUM(`Mem_Ind`.`mem1721`) AS `regn_mem1721`, SUM(`Mem_Ind`.`mem1721_female`) AS `regn_mem1721_Female`, SUM(`Mem_Ind`.`mem1721_male`) AS `regn_mem1721_Male`, SUM(`Mem_Ind`.`mem60ab`) AS `regn_mem60ab`, SUM(`Mem_Ind`.`mem60ab_female`) AS `regn_mem60ab_Female`, SUM(`Mem_Ind`.`mem60ab_male`) AS `regn_mem60ab_Male`, SUM(`Mem_Ind`.`labfor`) AS `regn_labfor`, SUM(`Mem_Ind`.`labfor_female`) AS `regn_labfor_Female`, SUM(`Mem_Ind`.`labfor_male`) AS `regn_labfor_Male`  \n\
			FROM `regn`, `mem_ind` \n\
			WHERE (LEFT(`regn`.`regnID`, 2) = LEFT(`mem_ind`.`memID`, 2)) \n\
			GROUP BY `regn`.`regnID` \n\
			;";	

		break;
		
	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}

	return sSQL;

}
CString CStatSimElt::sSQLCrDecTable()

{
	CString sSQL;
	

	switch(currElt) {

	case MEM:
				
		sSQL = _T("");		

		break;

	case HH:
		
		sSQL = _T("");		

		break;

	case PUROK:
		
		sSQL = _T("");		
		
		break;

	case BRGY:
		
		sSQL = "CREATE TABLE IF NOT EXISTS `brgy_decile_temp` ( \n\
			`regn` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`prov` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`mun` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`brgy` INTEGER(3) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`decile` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`mintotin` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			`maxtotin` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			`minpci` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			`maxpci` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			PRIMARY KEY(`regn`, `prov`, `mun`, `brgy`, `decile`), \n\
			INDEX `mintotin`(`mintotin`), \n\
			INDEX `maxtotin`(`maxtotin`), \n\
			INDEX `minpci`(`minpci`), \n\
			INDEX `maxpci`(`maxpci`) \n\
			) \n\
			ENGINE = MYISAM; \n";

		break;

	case MUN:
		
		sSQL = "CREATE TABLE IF NOT EXISTS `mun_decile_temp` ( \n\
			`regn` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`prov` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`mun` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`decile` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`mintotin` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			`maxtotin` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			`minpci` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			`maxpci` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			PRIMARY KEY(`regn`, `prov`, `mun`, `decile`), \n\
			INDEX `mintotin`(`mintotin`), \n\
			INDEX `maxtotin`(`maxtotin`), \n\
			INDEX `minpci`(`minpci`), \n\
			INDEX `maxpci`(`maxpci`) \n\
			) \n\
			ENGINE = MYISAM; \n";

		break;

	case PROV:
		
		sSQL = "CREATE TABLE IF NOT EXISTS `prov_decile_temp` ( \n\
			`regn` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`prov` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`decile` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`mintotin` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			`maxtotin` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			`minpci` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			`maxpci` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			PRIMARY KEY(`regn`, `prov`, `decile`), \n\
			INDEX `mintotin`(`mintotin`), \n\
			INDEX `maxtotin`(`maxtotin`), \n\
			INDEX `minpci`(`minpci`), \n\
			INDEX `maxpci`(`maxpci`) \n\
			) \n\
			ENGINE = MYISAM; \n";

		break;

	case REGN:
		
		sSQL = "CREATE TABLE IF NOT EXISTS `regn_decile_temp` ( \n\
			`regn` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`decile` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`mintotin` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			`maxtotin` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			`minpci` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			`maxpci` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			PRIMARY KEY(`regn`, `decile`), \n\
			INDEX `mintotin`(`mintotin`), \n\
			INDEX `maxtotin`(`maxtotin`), \n\
			INDEX `minpci`(`minpci`), \n\
			INDEX `maxpci`(`maxpci`) \n\
			) \n\
			ENGINE = MYISAM; \n";

		break;
		
	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}

	return sSQL;


}

CString CStatSimElt::sSQLCreateIncDistCoreIndTable(LPCSTR sIncDistVar)
{

	CString sSQL, sInd(sIncDistVar),
		sSQLInit, sSQLFD = _T("");

	for (int i=0; i<N_COREIND; i++) {
		
		CString sFld(sHHCoreInd[i][LABEL]);
		//temporarily set to labels
		sSQLFD = sSQLFD + _T("`") + sFld + _T(" - magnitude` INTEGER UNSIGNED NOT NULL, \n")
			+ _T("`") + sFld + _T(" - proportion` FLOAT(4,1) UNSIGNED NOT NULL, \n");

	}


	switch(currElt) {

	case MEM:
				
		sSQL = _T("");		

		break;

	case HH:
		
		sSQL = _T("");		

		break;

	case PUROK:
		
		sSQL = _T("");		

		break;

	case BRGY:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `brgy_HHCoreInd!") + sInd + _T("` ( \n\
			`regn` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`prov` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`mun` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`brgy` SMALLINT(3) UNSIGNED ZEROFILL NOT NULL, \n\
			`Region` varchar(50), \n\
			`Province` varchar(50), \n\
			`Municipality` varchar(50), \n\
			`Barangay` varchar(100), \n\
			`brgyID` VARCHAR(9) NOT NULL DEFAULT '', \n\
			`HH_") + sInd + _T("` VARCHAR(255) NOT NULL, \n\
			`") + sInd + _T(" - magnitude` INTEGER UNSIGNED NOT NULL, \n\
			`") + sInd + _T(" - proportion` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			`Mean per capita income` FLOAT(12,2) UNSIGNED NOT NULL, \n\
			`Mean household size` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			") + sSQLFD + _T("\
			PRIMARY KEY(`brgyID`, `HH_") + sInd + _T("`), \n\
			INDEX `IndIndex`(`regn`, `prov`, `mun`, `brgy`, `Region`, `Province`, `Municipality`, `Barangay`) \n\
			) \n\
			ENGINE = MYISAM; \n");	

		break;

	case MUN:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `mun_HHCoreInd!") + sInd + _T("` ( \n\
			`regn` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`prov` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`mun` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`Region` varchar(50), \n\
			`Province` varchar(50), \n\
			`Municipality` varchar(50), \n\
			`munID` VARCHAR(9) NOT NULL DEFAULT '', \n\
			`HH_") + sInd + _T("` VARCHAR(255) NOT NULL, \n\
			`") + sInd + _T(" - magnitude` INTEGER UNSIGNED NOT NULL, \n\
			`") + sInd + _T(" - proportion` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			`Mean per capita income` FLOAT(12,2) UNSIGNED NOT NULL, \n\
			`Mean household size` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			") + sSQLFD + _T("\
			PRIMARY KEY(`munID`, `HH_") + sInd + _T("`), \n\
			INDEX `IndIndex`(`regn`, `prov`, `mun`, `Region`, `Province`, `Municipality`) \n\
			) \n\
			ENGINE = MYISAM; \n");	

		break;

	case PROV:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `prov_HHCoreInd!") + sInd + _T("` ( \n\
			`regn` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`prov` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`Region` varchar(50), \n\
			`Province` varchar(50), \n\
			`provID` VARCHAR(9) NOT NULL DEFAULT '', \n\
			`HH_") + sInd + _T("` VARCHAR(255) NOT NULL, \n\
			`") + sInd + _T(" - magnitude` INTEGER UNSIGNED NOT NULL, \n\
			`") + sInd + _T(" - proportion` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			`Mean per capita income` FLOAT(12,2) UNSIGNED NOT NULL, \n\
			`Mean household size` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			") + sSQLFD + _T("\
			PRIMARY KEY(`provID`, `HH_") + sInd + _T("`), \n\
			INDEX `IndIndex`(`regn`, `prov`, `Region`, `Province`) \n\
			) \n\
			ENGINE = MYISAM; \n");	

		break;

	case REGN:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `regn_HHCoreInd!") + sInd + _T("` ( \n\
			`regn` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`Region` varchar(50), \n\
			`regnID` VARCHAR(9) NOT NULL DEFAULT '', \n\
			`HH_") + sInd + _T("` VARCHAR(255) NOT NULL, \n\
			`") + sInd + _T(" - magnitude` INTEGER UNSIGNED NOT NULL, \n\
			`") + sInd + _T(" - proportion` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			`Mean per capita income` FLOAT(12,2) UNSIGNED NOT NULL, \n\
			`Mean household size` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			") + sSQLFD + _T("\
			PRIMARY KEY(`regnID`, `HH_") + sInd + _T("`), \n\
			INDEX `IndIndex`(`regn`, `Region`) \n\
			) \n\
			ENGINE = MYISAM; \n");	

		break;
		
	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}

	return sSQL;


}

CString CStatSimElt::sSQLCreatePartialTableHH(int given)
{

	CString sSQL, sInd, sIndLabel,
		sSQLInit, sSQLFD = _T("");

	//get the given indicator given the index - 1
	sInd = sHHCoreInd[given-1][VAR];
	sIndLabel = sHHCoreInd[given-1][LABEL];

	for (int i=0; i<N_COREIND; i++) {

		if ( i != (given-1) ) {

			CString sFld(sHHCoreInd[i][LABEL]);

			//temporary to lables			
			sSQLFD = sSQLFD + _T("`") + sFld + _T(" - magnitude` INTEGER UNSIGNED NOT NULL, \n")
				+ _T("`") + sFld + _T(" - proportion` FLOAT(4,1) UNSIGNED NOT NULL, \n");
		
		}
	}


	switch(currElt) {

	case MEM:
				
		sSQL = _T("");		

		break;

	case HH:
		
		sSQL = _T("");		

		break;

	case PUROK:
		
		sSQL = _T("");		

		break;

	case BRGY:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `brgy_HHCoreInd!") + sInd + _T("` ( \n\
			`regn` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`prov` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`mun` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`brgy` SMALLINT(3) UNSIGNED ZEROFILL NOT NULL, \n\
			`Region` varchar(50), \n\
			`Province` varchar(50), \n\
			`Municipality` varchar(50), \n\
			`Barangay` varchar(100), \n\
			`brgyID` VARCHAR(9) NOT NULL DEFAULT '', \n\
			`") + sIndLabel + _T("` VARCHAR(255) NOT NULL, \n\
			`") + sIndLabel + _T(" - magnitude` INTEGER UNSIGNED NOT NULL, \n\
			`") + sIndLabel + _T(" - proportion` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			`Mean per capita income` FLOAT(12,2) UNSIGNED NOT NULL, \n\
			`Mean household size` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			") + sSQLFD + _T("\
			PRIMARY KEY(`brgyID`, `") + sIndLabel + _T("`), \n\
			INDEX `IndIndex`(`regn`, `prov`, `mun`, `brgy`, `Region`, `Province`, `Municipality`, `Barangay`) \n\
			) \n\
			ENGINE = MYISAM; \n");	

		break;

	case MUN:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `mun_HHCoreInd!") + sInd + _T("` ( \n\
			`regn` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`prov` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`mun` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`Region` varchar(50), \n\
			`Province` varchar(50), \n\
			`Municipality` varchar(50), \n\
			`munID` VARCHAR(9) NOT NULL DEFAULT '', \n\
			`") + sIndLabel + _T("` VARCHAR(255) NOT NULL, \n\
			`") + sIndLabel + _T(" - magnitude` INTEGER UNSIGNED NOT NULL, \n\
			`") + sIndLabel + _T(" - proportion` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			`Mean per capita income` FLOAT(12,2) UNSIGNED NOT NULL, \n\
			`Mean household size` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			") + sSQLFD + _T("\
			PRIMARY KEY(`munID`, `") + sIndLabel + _T("`), \n\
			INDEX `IndIndex`(`regn`, `prov`, `mun`, `Region`, `Province`, `Municipality`) \n\
			) \n\
			ENGINE = MYISAM; \n");	

		break;

	case PROV:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `prov_HHCoreInd!") + sInd + _T("` ( \n\
			`regn` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`prov` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`Region` varchar(50), \n\
			`Province` varchar(50), \n\
			`provID` VARCHAR(9) NOT NULL DEFAULT '', \n\
			`") + sIndLabel + _T("` VARCHAR(255) NOT NULL, \n\
			`") + sIndLabel + _T(" - magnitude` INTEGER UNSIGNED NOT NULL, \n\
			`") + sIndLabel + _T(" - proportion` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			`Mean per capita income` FLOAT(12,2) UNSIGNED NOT NULL, \n\
			`Mean household size` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			") + sSQLFD + _T("\
			PRIMARY KEY(`provID`, `") + sIndLabel + _T("`), \n\
			INDEX `IndIndex`(`regn`, `prov`, `Region`, `Province`) \n\
			) \n\
			ENGINE = MYISAM; \n");	

		break;

	case REGN:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `regn_HHCoreInd!") + sInd + _T("` ( \n\
			`regn` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`Region` varchar(50), \n\
			`regnID` VARCHAR(9) NOT NULL DEFAULT '', \n\
			`") + sIndLabel + _T("` VARCHAR(255) NOT NULL, \n\
			`") + sIndLabel + _T(" - magnitude` INTEGER UNSIGNED NOT NULL, \n\
			`") + sIndLabel + _T(" - proportion` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			`Mean per capita income` FLOAT(12,2) UNSIGNED NOT NULL, \n\
			`Mean household size` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			") + sSQLFD + _T("\
			PRIMARY KEY(`regnID`, `") + sIndLabel + _T("`), \n\
			INDEX `IndIndex`(`regn`, `Region`) \n\
			) \n\
			ENGINE = MYISAM; \n");	

		break;
		
	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}

	return sSQL;


}

CString CStatSimElt::sSQLCreatePartialTableMEM(int given)
{

	CString sSQL, sInd, sIndLabel,
		sSQLInit, sSQLFD = _T("");

	//get the given indicator given the index - 1
	sInd = sMEMCoreInd[given-1][VAR][TOT];
	sIndLabel = sMEMCoreInd[given-1][LABEL][TOT];

	for (int i=0; i<N_COREIND; i++) {

		if ( i != (given-1) ) {

			CString sFld(sMEMCoreInd[i][LABEL][TOT]);

			//temporary to lables			
			sSQLFD = sSQLFD + _T("`") + sFld + _T(" - magnitude` INTEGER UNSIGNED NOT NULL, \n")
				+ _T("`") + sFld + _T(", Male - magnitude` INTEGER UNSIGNED NOT NULL, \n")
				+ _T("`") + sFld + _T(", Female - magnitude` INTEGER UNSIGNED NOT NULL, \n")
				+ _T("`") + sFld + _T(" - proportion` FLOAT(4,1) UNSIGNED NOT NULL, \n")
				+ _T("`") + sFld + _T(", Male - proportion` FLOAT(4,1) UNSIGNED NOT NULL, \n")
				+ _T("`") + sFld + _T(", Female - proportion` FLOAT(4,1) UNSIGNED NOT NULL, \n");
		
		}
	}


	switch(currElt) {

	case MEM:
				
		sSQL = _T("");		

		break;

	case HH:
		
		sSQL = _T("");		

		break;

	case PUROK:
		
		sSQL = _T("");		

		break;

	case BRGY:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `brgy_MEMCoreInd!") + sInd + _T("` ( \n\
			`regn` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`prov` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`mun` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`brgy` SMALLINT(3) UNSIGNED ZEROFILL NOT NULL, \n\
			`Region` varchar(50), \n\
			`Province` varchar(50), \n\
			`Municipality` varchar(50), \n\
			`Barangay` varchar(100), \n\
			`brgyID` VARCHAR(9) NOT NULL DEFAULT '', \n\
			`") + sIndLabel + _T("` VARCHAR(255) NOT NULL, \n\
			`") + sIndLabel + _T(" - magnitude` INTEGER UNSIGNED NOT NULL, \n\
			`") + sIndLabel + _T(", Male - magnitude` INTEGER UNSIGNED NOT NULL, \n\
			`") + sIndLabel + _T(", Female - magnitude` INTEGER UNSIGNED NOT NULL, \n\
			`") + sIndLabel + _T(" - proportion` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			`") + sIndLabel + _T(", Male - proportion` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			`") + sIndLabel + _T(", Female - proportion` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			`Mean per capita income` FLOAT(12,2) UNSIGNED NOT NULL, \n\
			`Mean household size` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			") + sSQLFD + _T("\
			PRIMARY KEY(`brgyID`, `") + sIndLabel + _T("`), \n\
			INDEX `IndIndex`(`regn`, `prov`, `mun`, `brgy`, `Region`, `Province`, `Municipality`, `Barangay`) \n\
			) \n\
			ENGINE = MYISAM; \n");	

		break;

	case MUN:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `mun_MEMCoreInd!") + sInd + _T("` ( \n\
			`regn` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`prov` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`mun` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`Region` varchar(50), \n\
			`Province` varchar(50), \n\
			`Municipality` varchar(50), \n\
			`munID` VARCHAR(9) NOT NULL DEFAULT '', \n\
			`") + sIndLabel + _T("` VARCHAR(255) NOT NULL, \n\
			`") + sIndLabel + _T(" - magnitude` INTEGER UNSIGNED NOT NULL, \n\
			`") + sIndLabel + _T(", Male - magnitude` INTEGER UNSIGNED NOT NULL, \n\
			`") + sIndLabel + _T(", Female - magnitude` INTEGER UNSIGNED NOT NULL, \n\
			`") + sIndLabel + _T(" - proportion` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			`") + sIndLabel + _T(", Male - proportion` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			`") + sIndLabel + _T(", Female - proportion` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			`Mean per capita income` FLOAT(12,2) UNSIGNED NOT NULL, \n\
			`Mean household size` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			") + sSQLFD + _T("\
			PRIMARY KEY(`munID`, `") + sIndLabel + _T("`), \n\
			INDEX `IndIndex`(`regn`, `prov`, `mun`, `Region`, `Province`, `Municipality`) \n\
			) \n\
			ENGINE = MYISAM; \n");	

		break;

	case PROV:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `prov_MEMCoreInd!") + sInd + _T("` ( \n\
			`regn` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`prov` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`Region` varchar(50), \n\
			`Province` varchar(50), \n\
			`provID` VARCHAR(9) NOT NULL DEFAULT '', \n\
			`") + sIndLabel + _T("` VARCHAR(255) NOT NULL, \n\
			`") + sIndLabel + _T(" - magnitude` INTEGER UNSIGNED NOT NULL, \n\
			`") + sIndLabel + _T(", Male - magnitude` INTEGER UNSIGNED NOT NULL, \n\
			`") + sIndLabel + _T(", Female - magnitude` INTEGER UNSIGNED NOT NULL, \n\
			`") + sIndLabel + _T(" - proportion` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			`") + sIndLabel + _T(", Male - proportion` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			`") + sIndLabel + _T(", Female - proportion` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			`Mean per capita income` FLOAT(12,2) UNSIGNED NOT NULL, \n\
			`Mean household size` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			") + sSQLFD + _T("\
			PRIMARY KEY(`provID`, `") + sIndLabel + _T("`), \n\
			INDEX `IndIndex`(`regn`, `prov`, `Region`, `Province`) \n\
			) \n\
			ENGINE = MYISAM; \n");	

		break;

	case REGN:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `regn_MEMCoreInd!") + sInd + _T("` ( \n\
			`regn` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
			`Region` varchar(50), \n\
			`regnID` VARCHAR(9) NOT NULL DEFAULT '', \n\
			`") + sIndLabel + _T("` VARCHAR(255) NOT NULL, \n\
			`") + sIndLabel + _T(" - magnitude` INTEGER UNSIGNED NOT NULL, \n\
			`") + sIndLabel + _T(", Male - magnitude` INTEGER UNSIGNED NOT NULL, \n\
			`") + sIndLabel + _T(", Female - magnitude` INTEGER UNSIGNED NOT NULL, \n\
			`") + sIndLabel + _T(" - proportion` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			`") + sIndLabel + _T(", Male - proportion` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			`") + sIndLabel + _T(", Female - proportion` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			`Mean per capita income` FLOAT(12,2) UNSIGNED NOT NULL, \n\
			`Mean household size` FLOAT(4,1) UNSIGNED NOT NULL, \n\
			") + sSQLFD + _T("\
			PRIMARY KEY(`regnID`, `") + sIndLabel + _T("`), \n\
			INDEX `IndIndex`(`regn`, `Region`) \n\
			) \n\
			ENGINE = MYISAM; \n");	

		break;
		
	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}

	return sSQL;


}

CString CStatSimElt::sSQLInsertIncDistCoreIndTable(LPCSTR sIncDistVar, LPCSTR sS)
{

	CString sSQL, sInd(sIncDistVar), sSrc(sS),
		sSQLInit, sSQLFD = _T("");

	for (int i=0; i<N_COREIND; i++) {
		
		CString sFld(sHHCoreInd[i][VAR]);
		
		sSQLFD = sSQLFD + _T("SUM(`hh_CoreInd`.`") + sFld + _T("`) AS `nHH") + sFld + _T("`, \n")
			+ _T("(SUM(`hh_CoreInd`.`") + sFld + _T("`)/COUNT(`") + sSrc + _T("`.`") + sInd + _T("`))*100 AS `pHH") + sFld + _T("`");
		
		int off = 1;
		
		if ( i < N_COREIND-off ) {
			sSQLFD = sSQLFD + _T(", \n");
		}
		else {				
			sSQLFD = sSQLFD + _T(" \n");
		}

	}


	switch(currElt) {

	case MEM:
				
		sSQL = _T("");		

		break;

	case HH:
		
		sSQL = _T("");		

		break;

	case PUROK:
		
		sSQL = _T("");		

		break;

	case BRGY:
		
		sSQL = _T("INSERT IGNORE INTO `brgy_HHCoreInd!") + sInd + _T("` \n\
			SELECT `") + sSrc + _T("`.`regn` AS `regn`, `") + sSrc + _T("`.`prov` AS `prov`, `") + sSrc + _T("`.`mun` AS `mun`, `") + sSrc + _T("`.`brgy` AS `brgy`, \n\
			`hh_CoreInd`.`Region`, `hh_CoreInd`.`Province`, `hh_CoreInd`.`Municipality`, `hh_CoreInd`.`Barangay`, \n\
			`brgy_totpop`.`brgyID` AS `brgyID`, \n\
			`") + sSrc + _T("`.`") + sInd + _T("` AS `HH_") + sInd + _T("`, \n\
			COUNT(`") + sSrc + _T("`.`") + sInd + _T("`) AS `nHH") + sInd + _T("`, \n\
			(COUNT(`") + sSrc + _T("`.`") + sInd + _T("`)/`brgy_totpop`.`brgy_tothh`)*100 AS `pHH") + sInd + _T("`, \n\
			AVG(`") + sSrc + _T("`.`hh_pci`) AS `meanhhPCI`, \n\
			AVG(`") + sSrc + _T("`.`hsize`) AS `meanhhSize`, \n\
			") + sSQLFD + _T(" \n\
			FROM ( (`hh_coreind` INNER JOIN `") + sSrc + _T("` USING (`hhID`)) \n\
			INNER JOIN `brgy_totpop` ON (LEFT(`brgy_totpop`.`brgyID`, 9) = LEFT(`hh_CoreInd`.`hhID`, 9)) ) \n\
			GROUP BY `regn`, `prov`, `mun`, `brgy`, `hh_") + sInd + _T("`;");


		break;

	case MUN:
		
		sSQL = _T("INSERT IGNORE INTO `mun_HHCoreInd!") + sInd + _T("` \n\
			SELECT `") + sSrc + _T("`.`regn` AS `regn`, `") + sSrc + _T("`.`prov` AS `prov`, `") + sSrc + _T("`.`mun` AS `mun`, \n\
			`hh_CoreInd`.`Region`, `hh_CoreInd`.`Province`, `hh_CoreInd`.`Municipality`, \n\
			`mun_totpop`.`munID` AS `munID`, \n\
			`") + sSrc + _T("`.`") + sInd + _T("` AS `HH_") + sInd + _T("`, \n\
			COUNT(`") + sSrc + _T("`.`") + sInd + _T("`) AS `nHH") + sInd + _T("`, \n\
			(COUNT(`") + sSrc + _T("`.`") + sInd + _T("`)/`mun_totpop`.`mun_tothh`)*100 AS `pHH") + sInd + _T("`, \n\
			AVG(`") + sSrc + _T("`.`hh_pci`) AS `meanhhPCI`, \n\
			AVG(`") + sSrc + _T("`.`hsize`) AS `meanhhSize`, \n\
			") + sSQLFD + _T(" \n\
			FROM ( (`hh_coreind` INNER JOIN `") + sSrc + _T("` USING (`hhID`)) \n\
			INNER JOIN `mun_totpop` ON (LEFT(`mun_totpop`.`munID`, 6) = LEFT(`hh_CoreInd`.`hhID`, 6)) ) \n\
			GROUP BY `regn`, `prov`, `mun`, `hh_") + sInd + _T("`;");

		break;

	case PROV:
		
		sSQL = _T("INSERT IGNORE INTO `prov_HHCoreInd!") + sInd + _T("` \n\
			SELECT `") + sSrc + _T("`.`regn` AS `regn`, `") + sSrc + _T("`.`prov` AS `prov`, \n\
			`hh_CoreInd`.`Region`, `hh_CoreInd`.`Province`, \n\
			`prov_totpop`.`provID` AS `provID`, \n\
			`") + sSrc + _T("`.`") + sInd + _T("` AS `HH_") + sInd + _T("`, \n\
			COUNT(`") + sSrc + _T("`.`") + sInd + _T("`) AS `nHH") + sInd + _T("`, \n\
			(COUNT(`") + sSrc + _T("`.`") + sInd + _T("`)/`prov_totpop`.`prov_tothh`)*100 AS `pHH") + sInd + _T("`, \n\
			AVG(`") + sSrc + _T("`.`hh_pci`) AS `meanhhPCI`, \n\
			AVG(`") + sSrc + _T("`.`hsize`) AS `meanhhSize`, \n\
			") + sSQLFD + _T(" \n\
			FROM ( (`hh_coreind` INNER JOIN `") + sSrc + _T("` USING (`hhID`)) \n\
			INNER JOIN `prov_totpop` ON (LEFT(`prov_totpop`.`provID`, 4) = LEFT(`hh_CoreInd`.`hhID`, 4)) ) \n\
			GROUP BY `regn`, `prov`, `hh_") + sInd + _T("`;");

		break;

	case REGN:
		
		sSQL = _T("INSERT IGNORE INTO `regn_HHCoreInd!") + sInd + _T("` \n\
			SELECT `") + sSrc + _T("`.`regn` AS `regn`, \n\
			`hh_CoreInd`.`Region`, \n\
			`regn_totpop`.`regnID` AS `regnID`, \n\
			`") + sSrc + _T("`.`") + sInd + _T("` AS `HH_") + sInd + _T("`, \n\
			COUNT(`") + sSrc + _T("`.`") + sInd + _T("`) AS `nHH") + sInd + _T("`, \n\
			(COUNT(`") + sSrc + _T("`.`") + sInd + _T("`)/`regn_totpop`.`regn_tothh`)*100 AS `pHH") + sInd + _T("`, \n\
			AVG(`") + sSrc + _T("`.`hh_pci`) AS `meanhhPCI`, \n\
			AVG(`") + sSrc + _T("`.`hsize`) AS `meanhhSize`, \n\
			") + sSQLFD + _T(" \n\
			FROM ( (`hh_coreind` INNER JOIN `") + sSrc + _T("` USING (`hhID`)) \n\
			INNER JOIN `regn_totpop` ON (LEFT(`regn_totpop`.`regnID`, 2) = LEFT(`hh_CoreInd`.`hhID`, 2)) ) \n\
			GROUP BY `regn`, `hh_") + sInd + _T("`;");

		break;
		
	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}

	return sSQL;


}

CString CStatSimElt::sSQLInsertPartialTableHH(int given)
{

	CString sSQL, sInd,
		sSQLInit, sSQLFD = _T("");

	//get the given indicator given the index - 1
	sInd = sHHCoreInd[given-1][VAR];

	for (int i=0; i<N_COREIND; i++) {

		if ( i != (given-1) ) {

			CString sFld(sHHCoreInd[i][VAR]);
			
			sSQLFD = sSQLFD + _T("SUM(`hh_CoreInd`.`") + sFld + _T("`) AS `nHH") + sFld + _T("`, \n")
				+ _T("(SUM(`hh_CoreInd`.`") + sFld + _T("`)/COUNT(`hh_coreind`.`hh_") + sInd + _T("`))*100 AS `pHH") + sFld + _T("`");


			int off;

			if ( given==N_COREIND ) {
				off = 2;
			}
			else {
				off = 1;
			}
			
			if ( i < N_COREIND-off ) {
				sSQLFD = sSQLFD + _T(", \n");
			}
			else {
				sSQLFD = sSQLFD + _T(" \n");
			}

		}

	}


	switch(currElt) {

	case MEM:
				
		sSQL = _T("");		

		break;

	case HH:
		
		sSQL = _T("");		

		break;

	case PUROK:
		
		sSQL = _T("");		

		break;

	case BRGY:
		
		sSQL = _T("INSERT IGNORE INTO `brgy_HHCoreInd!") + sInd + _T("` \n\
			SELECT `hh_ind`.`regn` AS `regn`, `hh_ind`.`prov` AS `prov`, `hh_ind`.`mun` AS `mun`, `hh_ind`.`brgy` AS `brgy`, \n\
			`hh_CoreInd`.`Region`, `hh_CoreInd`.`Province`, `hh_CoreInd`.`Municipality`, `hh_CoreInd`.`Barangay`, \n\
			`brgy_totpop`.`brgyID` AS `brgyID`, \n\
			`hh_coreind`.`hh_") + sInd + _T("` AS `HH_") + sInd + _T("`, \n\
			COUNT(`hh_coreind`.`hh_") + sInd + _T("`) AS `nHH") + sInd + _T("`, \n\
			(COUNT(`hh_coreind`.`hh_") + sInd + _T("`)/`brgy_totpop`.`brgy_tothh`)*100 AS `pHH") + sInd + _T("`, \n\
			AVG(`hh_ind`.`hh_pci`) AS `meanhhPCI`, \n\
			AVG(`hh_ind`.`hsize`) AS `meanhhSize`, \n\
			") + sSQLFD + _T(" \n\
			FROM ( (`hh_coreind` INNER JOIN `hh_ind` USING (`hhID`)) \n\
			INNER JOIN `brgy_totpop` ON (LEFT(`brgy_totpop`.`brgyID`, 9) = LEFT(`hh_CoreInd`.`hhID`, 9)) ) \n\
			GROUP BY `regn`, `prov`, `mun`, `brgy`, `hh_") + sInd + _T("`;");


		break;

	case MUN:
		
		sSQL = _T("INSERT IGNORE INTO `mun_HHCoreInd!") + sInd + _T("` \n\
			SELECT `hh_ind`.`regn` AS `regn`, `hh_ind`.`prov` AS `prov`, `hh_ind`.`mun` AS `mun`, \n\
			`hh_CoreInd`.`Region`, `hh_CoreInd`.`Province`, `hh_CoreInd`.`Municipality`, \n\
			`mun_totpop`.`munID` AS `munID`, \n\
			`hh_coreind`.`hh_") + sInd + _T("` AS `HH_") + sInd + _T("`, \n\
			COUNT(`hh_coreind`.`hh_") + sInd + _T("`) AS `nHH") + sInd + _T("`, \n\
			(COUNT(`hh_coreind`.`hh_") + sInd + _T("`)/`mun_totpop`.`mun_tothh`)*100 AS `pHH") + sInd + _T("`, \n\
			AVG(`hh_ind`.`hh_pci`) AS `meanhhPCI`, \n\
			AVG(`hh_ind`.`hsize`) AS `meanhhSize`, \n\
			") + sSQLFD + _T(" \n\
			FROM ( (`hh_coreind` INNER JOIN `hh_ind` USING (`hhID`)) \n\
			INNER JOIN `mun_totpop` ON (LEFT(`mun_totpop`.`munID`, 6) = LEFT(`hh_CoreInd`.`hhID`, 6)) ) \n\
			GROUP BY `regn`, `prov`, `mun`, `hh_") + sInd + _T("`;");

		break;

	case PROV:
		
		sSQL = _T("INSERT IGNORE INTO `prov_HHCoreInd!") + sInd + _T("` \n\
			SELECT `hh_ind`.`regn` AS `regn`, `hh_ind`.`prov` AS `prov`, \n\
			`hh_CoreInd`.`Region`, `hh_CoreInd`.`Province`, \n\
			`prov_totpop`.`provID` AS `provID`, \n\
			`hh_coreind`.`hh_") + sInd + _T("` AS `HH_") + sInd + _T("`, \n\
			COUNT(`hh_coreind`.`hh_") + sInd + _T("`) AS `nHH") + sInd + _T("`, \n\
			(COUNT(`hh_coreind`.`hh_") + sInd + _T("`)/`prov_totpop`.`prov_tothh`)*100 AS `pHH") + sInd + _T("`, \n\
			AVG(`hh_ind`.`hh_pci`) AS `meanhhPCI`, \n\
			AVG(`hh_ind`.`hsize`) AS `meanhhSize`, \n\
			") + sSQLFD + _T(" \n\
			FROM ( (`hh_coreind` INNER JOIN `hh_ind` USING (`hhID`)) \n\
			INNER JOIN `prov_totpop` ON (LEFT(`prov_totpop`.`provID`, 4) = LEFT(`hh_CoreInd`.`hhID`, 4)) ) \n\
			GROUP BY `regn`, `prov`, `hh_") + sInd + _T("`;");

		break;

	case REGN:
		
		sSQL = _T("INSERT IGNORE INTO `regn_HHCoreInd!") + sInd + _T("` \n\
			SELECT `hh_ind`.`regn` AS `regn`, \n\
			`hh_CoreInd`.`Region`, \n\
			`regn_totpop`.`regnID` AS `regnID`, \n\
			`hh_coreind`.`hh_") + sInd + _T("` AS `HH_") + sInd + _T("`, \n\
			COUNT(`hh_coreind`.`hh_") + sInd + _T("`) AS `nHH") + sInd + _T("`, \n\
			(COUNT(`hh_coreind`.`hh_") + sInd + _T("`)/`regn_totpop`.`regn_tothh`)*100 AS `pHH") + sInd + _T("`, \n\
			AVG(`hh_ind`.`hh_pci`) AS `meanhhPCI`, \n\
			AVG(`hh_ind`.`hsize`) AS `meanhhSize`, \n\
			") + sSQLFD + _T(" \n\
			FROM ( (`hh_coreind` INNER JOIN `hh_ind` USING (`hhID`)) \n\
			INNER JOIN `regn_totpop` ON (LEFT(`regn_totpop`.`regnID`, 2) = LEFT(`hh_CoreInd`.`hhID`, 2)) ) \n\
			GROUP BY `regn`, `hh_") + sInd + _T("`;");

		break;
		
	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}

	return sSQL;


}

CString CStatSimElt::sSQLInsertPartialTableMEM(int given)
{

	CString sSQL, sInd,
		sSQLInit, sSQLFD = _T("");

	//get the given indicator given the index - 1
	sInd = sMEMCoreInd[given-1][VAR][TOT];

	for (int i=0; i<N_COREIND; i++) {

		if ( i != (given-1) ) {

			CString sFld(sMEMCoreInd[i][VAR][TOT]);
			
			sSQLFD = sSQLFD + _T("SUM(`hh_CoreInd`.`") + sFld + _T("`*`hh_CoreInd`.`hh_totmem`) AS `nHH") + sFld + _T("`, \n")
				+ _T("SUM(`hh_CoreInd`.`") + sFld + _T("`*`hh_CoreInd`.`hh_totmem_male`) AS `nHH") + sFld + _T("_male`, \n")
				+ _T("SUM(`hh_CoreInd`.`") + sFld + _T("`*`hh_CoreInd`.`hh_totmem_female`) AS `nHH") + sFld + _T("_female`, \n")
				+ _T("(SUM(`hh_CoreInd`.`") + sFld + _T("`*`hh_CoreInd`.`hh_totmem`)/SUM(`hh_coreind`.`hh_totmem`))*100 AS `pHH") + sFld + _T("`, \n")
				+ _T("(SUM(`hh_CoreInd`.`") + sFld + _T("`*`hh_CoreInd`.`hh_totmem_male`)/SUM(`hh_coreind`.`hh_totmem_male`))*100 AS `pHH") + sFld + _T("_male`, \n")
				+ _T("(SUM(`hh_CoreInd`.`") + sFld + _T("`*`hh_CoreInd`.`hh_totmem_female`)/SUM(`hh_coreind`.`hh_totmem_female`))*100 AS `pHH") + sFld + _T("_female` ");


			int off;

			if ( given==N_COREIND ) {
				off = 2;
			}
			else {
				off = 1;
			}
			
			if ( i < N_COREIND-off ) {
				sSQLFD = sSQLFD + _T(", \n");
			}
			else {
				sSQLFD = sSQLFD + _T(" \n");
			}

		}

	}


	switch(currElt) {

	case MEM:
				
		sSQL = _T("");		

		break;

	case HH:
		
		sSQL = _T("");		

		break;

	case PUROK:
		
		sSQL = _T("");		

		break;

	case BRGY:
		
		sSQL = _T("INSERT IGNORE INTO `brgy_MEMCoreInd!") + sInd + _T("` \n\
			SELECT `hh_ind`.`regn` AS `regn`, `hh_ind`.`prov` AS `prov`, `hh_ind`.`mun` AS `mun`, `hh_ind`.`brgy` AS `brgy`, \n\
			`hh_CoreInd`.`Region`, `hh_CoreInd`.`Province`, `hh_CoreInd`.`Municipality`, `hh_CoreInd`.`Barangay`, \n\
			`brgy_totpop`.`brgyID` AS `brgyID`, \n\
			`hh_coreind`.`hh_") + sInd + _T("` AS `HH_") + sInd + _T("`, \n\
			SUM(`hh_coreind`.`hh_totmem`) AS `nHH") + sInd + _T("`, \n\
			SUM(`hh_coreind`.`hh_totmem_male`) AS `nHH") + sInd + _T("_male`, \n\
			SUM(`hh_coreind`.`hh_totmem_female`) AS `nHH") + sInd + _T("_female`, \n\
			(SUM(`hh_coreind`.`hh_totmem`)/`brgy_totpop`.`brgy_totmem`)*100 AS `pHH") + sInd + _T("`, \n\
			(SUM(`hh_coreind`.`hh_totmem_male`)/`brgy_totpop`.`brgy_totmem_male`)*100 AS `pHH") + sInd + _T("_male`, \n\
			(SUM(`hh_coreind`.`hh_totmem_female`)/`brgy_totpop`.`brgy_totmem_female`)*100 AS `pHH") + sInd + _T("_female`, \n\
			AVG(`hh_ind`.`hh_pci`) AS `meanhhPCI`, \n\
			AVG(`hh_ind`.`hsize`) AS `meanhhSize`, \n\
			") + sSQLFD + _T(" \n\
			FROM ( (`hh_coreind` INNER JOIN `hh_ind` USING (`hhID`)) \n\
			INNER JOIN `brgy_totpop` ON (LEFT(`brgy_totpop`.`brgyID`, 9) = LEFT(`hh_CoreInd`.`hhID`, 9)) ) \n\
			GROUP BY `regn`, `prov`, `mun`, `brgy`, `hh_") + sInd + _T("`;");


		break;

	case MUN:
		
		sSQL = _T("INSERT IGNORE INTO `mun_MEMCoreInd!") + sInd + _T("` \n\
			SELECT `hh_ind`.`regn` AS `regn`, `hh_ind`.`prov` AS `prov`, `hh_ind`.`mun` AS `mun`, \n\
			`hh_CoreInd`.`Region`, `hh_CoreInd`.`Province`, `hh_CoreInd`.`Municipality`, \n\
			`mun_totpop`.`munID` AS `munID`, \n\
			`hh_coreind`.`hh_") + sInd + _T("` AS `HH_") + sInd + _T("`, \n\
			SUM(`hh_coreind`.`hh_totmem`) AS `nHH") + sInd + _T("`, \n\
			SUM(`hh_coreind`.`hh_totmem_male`) AS `nHH") + sInd + _T("_male`, \n\
			SUM(`hh_coreind`.`hh_totmem_female`) AS `nHH") + sInd + _T("_female`, \n\
			(SUM(`hh_coreind`.`hh_totmem`)/`mun_totpop`.`mun_totmem`)*100 AS `pHH") + sInd + _T("`, \n\
			(SUM(`hh_coreind`.`hh_totmem_male`)/`mun_totpop`.`mun_totmem_male`)*100 AS `pHH") + sInd + _T("_male`, \n\
			(SUM(`hh_coreind`.`hh_totmem_female`)/`mun_totpop`.`mun_totmem_female`)*100 AS `pHH") + sInd + _T("_female`, \n\
			AVG(`hh_ind`.`hh_pci`) AS `meanhhPCI`, \n\
			AVG(`hh_ind`.`hsize`) AS `meanhhSize`, \n\
			") + sSQLFD + _T(" \n\
			FROM ( (`hh_coreind` INNER JOIN `hh_ind` USING (`hhID`)) \n\
			INNER JOIN `mun_totpop` ON (LEFT(`mun_totpop`.`munID`, 6) = LEFT(`hh_CoreInd`.`hhID`, 6)) ) \n\
			GROUP BY `regn`, `prov`, `mun`, `hh_") + sInd + _T("`;");

		break;

	case PROV:
		
		sSQL = _T("INSERT IGNORE INTO `prov_MEMCoreInd!") + sInd + _T("` \n\
			SELECT `hh_ind`.`regn` AS `regn`, `hh_ind`.`prov` AS `prov`, \n\
			`hh_CoreInd`.`Region`, `hh_CoreInd`.`Province`, \n\
			`prov_totpop`.`provID` AS `provID`, \n\
			`hh_coreind`.`hh_") + sInd + _T("` AS `HH_") + sInd + _T("`, \n\
			SUM(`hh_coreind`.`hh_totmem`) AS `nHH") + sInd + _T("`, \n\
			SUM(`hh_coreind`.`hh_totmem_male`) AS `nHH") + sInd + _T("_male`, \n\
			SUM(`hh_coreind`.`hh_totmem_female`) AS `nHH") + sInd + _T("_female`, \n\
			(SUM(`hh_coreind`.`hh_totmem`)/`prov_totpop`.`prov_totmem`)*100 AS `pHH") + sInd + _T("`, \n\
			(SUM(`hh_coreind`.`hh_totmem_male`)/`prov_totpop`.`prov_totmem_male`)*100 AS `pHH") + sInd + _T("_male`, \n\
			(SUM(`hh_coreind`.`hh_totmem_female`)/`prov_totpop`.`prov_totmem_female`)*100 AS `pHH") + sInd + _T("_female`, \n\
			AVG(`hh_ind`.`hh_pci`) AS `meanhhPCI`, \n\
			AVG(`hh_ind`.`hsize`) AS `meanhhSize`, \n\
			") + sSQLFD + _T(" \n\
			FROM ( (`hh_coreind` INNER JOIN `hh_ind` USING (`hhID`)) \n\
			INNER JOIN `prov_totpop` ON (LEFT(`prov_totpop`.`provID`, 4) = LEFT(`hh_CoreInd`.`hhID`, 4)) ) \n\
			GROUP BY `regn`, `prov`, `hh_") + sInd + _T("`;");

		break;

	case REGN:
		
		sSQL = _T("INSERT IGNORE INTO `regn_MEMCoreInd!") + sInd + _T("` \n\
			SELECT `hh_ind`.`regn` AS `regn`, \n\
			`hh_CoreInd`.`Region`, \n\
			`regn_totpop`.`regnID` AS `regnID`, \n\
			`hh_coreind`.`hh_") + sInd + _T("` AS `HH_") + sInd + _T("`, \n\
			SUM(`hh_coreind`.`hh_totmem`) AS `nHH") + sInd + _T("`, \n\
			SUM(`hh_coreind`.`hh_totmem_male`) AS `nHH") + sInd + _T("_male`, \n\
			SUM(`hh_coreind`.`hh_totmem_female`) AS `nHH") + sInd + _T("_female`, \n\
			(SUM(`hh_coreind`.`hh_totmem`)/`regn_totpop`.`regn_totmem`)*100 AS `pHH") + sInd + _T("`, \n\
			(SUM(`hh_coreind`.`hh_totmem_male`)/`regn_totpop`.`regn_totmem_male`)*100 AS `pHH") + sInd + _T("_male`, \n\
			(SUM(`hh_coreind`.`hh_totmem_female`)/`regn_totpop`.`regn_totmem_female`)*100 AS `pHH") + sInd + _T("_female`, \n\
			AVG(`hh_ind`.`hh_pci`) AS `meanhhPCI`, \n\
			AVG(`hh_ind`.`hsize`) AS `meanhhSize`, \n\
			") + sSQLFD + _T(" \n\
			FROM ( (`hh_coreind` INNER JOIN `hh_ind` USING (`hhID`)) \n\
			INNER JOIN `regn_totpop` ON (LEFT(`regn_totpop`.`regnID`, 2) = LEFT(`hh_CoreInd`.`hhID`, 2)) ) \n\
			GROUP BY `regn`, `hh_") + sInd + _T("`;");

		break;
		
	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}

	return sSQL;


}

CString CStatSimElt::sSQLCrQuiTable()

{
	CString sSQL;
	

	switch(currElt) {

	case MEM:
				
		sSQL = _T("");		

		break;

	case HH:
		
		sSQL = _T("");		

		break;

	case PUROK:
		
		sSQL = _T("");		
		
		break;

	case BRGY:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `brgy_quintile_temp` ( \n\
			`regn` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`prov` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`mun` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`brgy` INTEGER(3) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`quintile` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`mintotin` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			`maxtotin` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			`minpci` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			`maxpci` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			PRIMARY KEY(`regn`, `prov`, `mun`, `brgy`, `quintile`), \n\
			INDEX `mintotin`(`mintotin`), \n\
			INDEX `maxtotin`(`maxtotin`), \n\
			INDEX `minpci`(`minpci`), \n\
			INDEX `maxpci`(`maxpci`) \n\
			) \n\
			ENGINE = MYISAM; \n");

		break;

	case MUN:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `mun_quintile_temp` ( \n\
			`regn` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`prov` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`mun` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`quintile` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`mintotin` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			`maxtotin` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			`minpci` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			`maxpci` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			PRIMARY KEY(`regn`, `prov`, `mun`, `quintile`), \n\
			INDEX `mintotin`(`mintotin`), \n\
			INDEX `maxtotin`(`maxtotin`), \n\
			INDEX `minpci`(`minpci`), \n\
			INDEX `maxpci`(`maxpci`) \n\
			) \n\
			ENGINE = MYISAM; \n");

		break;

	case PROV:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `prov_quintile_temp` ( \n\
			`regn` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`prov` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`quintile` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`mintotin` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			`maxtotin` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			`minpci` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			`maxpci` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			PRIMARY KEY(`regn`, `prov`, `quintile`), \n\
			INDEX `mintotin`(`mintotin`), \n\
			INDEX `maxtotin`(`maxtotin`), \n\
			INDEX `minpci`(`minpci`), \n\
			INDEX `maxpci`(`maxpci`) \n\
			) \n\
			ENGINE = MYISAM; \n");

		break;

	case REGN:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `regn_quintile_temp` ( \n\
			`regn` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`quintile` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			`mintotin` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			`maxtotin` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			`minpci` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			`maxpci` FLOAT(12,2) NOT NULL DEFAULT 0, \n\
			PRIMARY KEY(`regn`, `quintile`), \n\
			INDEX `mintotin`(`mintotin`), \n\
			INDEX `maxtotin`(`maxtotin`), \n\
			INDEX `minpci`(`minpci`), \n\
			INDEX `maxpci`(`maxpci`) \n\
			) \n\
			ENGINE = MYISAM; \n");

		break;
		
	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}

	return sSQL;


}

CString CStatSimElt::sSQLTotHH()
{

	CString sSQL;
	

	switch(currElt) {

	case MEM:
				
		sSQL = _T("");		

		break;

	case HH:
		
		sSQL = _T("");		

		break;

	case PUROK:
		
		sSQL = _T("SELECT  `purok`.`purokID` AS `purokID`, `regn`.`Region` AS `Region`, `prov`.`Province` AS `Province`, `mun`.`Municipality` AS `Municipality`, `brgy`.`Barangay` AS `Barangay`, `purok`.`PurokName` AS `PurokName`, \n\
			COUNT(`HH_Ind`.`hhID`) AS `Purok_TotHH` \n\
			FROM `regn`, `prov`, `mun`, `brgy`, `purok`, `hh_ind` \n\
			WHERE (LEFT(`purok`.`purokID`, 11) = LEFT(`hh_ind`.`hhID`, 11)) AND (LEFT(`purok`.`purokID`, 2) = LEFT(`regn`.`regnID`, 2)) AND (LEFT(`purok`.`purokID`, 4) = LEFT(`prov`.`provID`, 4)) AND (LEFT(`purok`.`purokID`, 6) = LEFT(`mun`.`munID`, 6)) AND (LEFT(`purok`.`purokID`, 9) = LEFT(`brgy`.`brgyID`, 9)) \n\
			GROUP BY `purokID` \n\
			;");
		
		break;

	case BRGY:
		
		sSQL = _T("SELECT  `brgy`.`brgyID` AS `brgyID`, `regn`.`Region` AS `Region`, `prov`.`Province` AS `Province`, `mun`.`Municipality` AS `Municipality`, `brgy`.`Barangay` AS `Barangay`, \n\
			COUNT(`HH_Ind`.`hhID`) AS `Brgy_TotHH` \n\
			FROM `regn`, `prov`, `mun`, `brgy`, `hh_ind` \n\
			WHERE (LEFT(`brgy`.`brgyID`, 9) = LEFT(`hh_ind`.`hhID`, 9)) AND (LEFT(`brgy`.`brgyID`, 2) = LEFT(`regn`.`regnID`, 2)) AND (LEFT(`brgy`.`brgyID`, 4) = LEFT(`prov`.`provID`, 4)) AND (LEFT(`brgy`.`brgyID`, 6) = LEFT(`mun`.`munID`, 6)) \n\
			GROUP BY `brgyID` \n\
			;");

		break;

	case MUN:
		
		sSQL = _T("SELECT  `mun`.`munID` AS `munID`, `regn`.`Region` AS `Region`, `prov`.`Province` AS `Province`, `mun`.`Municipality` AS `Municipality`, \n\
			COUNT(`HH_Ind`.`hhID`) AS `Mun_TotHH` \n\
			FROM `regn`, `prov`, `mun`, `hh_ind` \n\
			WHERE (LEFT(`mun`.`munID`, 6) = LEFT(`hh_ind`.`hhID`, 6)) AND (LEFT(`mun`.`munID`, 2) = LEFT(`regn`.`regnID`, 2)) AND (LEFT(`mun`.`munID`, 4) = LEFT(`prov`.`provID`, 4)) \n\
			GROUP BY `munID` \n\
			;");

		break;

	case PROV:
		
		sSQL = _T("SELECT  `prov`.`provID` AS `provID`, `regn`.`Region` AS `Region`, `prov`.`Province` AS `Province`, \n\
			COUNT(`HH_Ind`.`hhID`) AS `Prov_TotHH` \n\
			FROM `regn`, `prov`, `hh_ind` \n\
			WHERE (LEFT(`prov`.`provID`, 4) = LEFT(`hh_ind`.`hhID`, 4)) AND (LEFT(`prov`.`provID`, 2) = LEFT(`regn`.`regnID`, 2)) \n\
			GROUP BY `provID` \n\
			;");

		break;

	case REGN:
		
		sSQL = _T("SELECT  `regn`.`regnID` AS `regnID`, `regn`.`Region` AS `Region`, \n\
			COUNT(`HH_Ind`.`hhID`) AS `Regn_TotHH` \n\
			FROM `regn`, `hh_ind` \n\
			WHERE (LEFT(`regn`.`regnID`, 2) = LEFT(`hh_ind`.`hhID`, 2)) \n\
			GROUP BY `regnID` \n\
			;");

		break;
		
	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}
	return sSQL;

}

CString CStatSimElt::sSQLTotMem(LPCSTR sex, LPCSTR sSex, BOOL exmem, BOOL work)
{

	CString sExMem, sExVar, sSQL, sSexClause = _T(""), sTabExt = _T("");

	if(exmem) {
		if (work) {
			if (m_qnrID==120110100 || m_qnrID==1020100100){
				sExMem = _T("`hpq_mem`.`ofw`=1 AND ISNULL(`hpq_mem`.`ofw_count`)=FALSE AND `hpq_mem`.`jobind`=1 AND `hpq_mem`.`age_yr`>=5");
			}
			else if (m_qnrID==1020130100 || m_qnrID==1020130101 || m_qnrID==1020135100 || m_qnrID==1020135200 || m_qnrID==1020135300 || m_qnrID==1020135400 || m_qnrID==1020135500) {
				sExMem = _T("`hpq_mem`.`ofw`=1 AND `hpq_mem`.`jobind`=1 AND `hpq_mem`.`age_yr`>=5");
			}
			sExVar = _T("fow");
		}
		else {
			if (m_qnrID==120110100 || m_qnrID==1020100100){
				sExMem = _T("`hpq_mem`.`ofw`=1 AND ISNULL(`hpq_mem`.`ofw_count`)=FALSE AND (`hpq_mem`.`jobind`=2 OR `hpq_mem`.`age_yr`<5)");
			}
			else if (m_qnrID==1020130100 || m_qnrID==1020130101 || m_qnrID==1020135100 || m_qnrID==1020135200 || m_qnrID==1020135300 || m_qnrID==1020135400 || m_qnrID==1020135500) {
				sExMem = _T("`hpq_mem`.`ofw`=1");
			}

			sExVar = _T("fo");
		}
	}
	else {
		sExVar = _T("");
	}

	if (exmem) {
		if (sex!=NULL) 
			sSexClause.Format(_T("WHERE ( (`hpq_mem`.`sex`)=%s AND (%s ) ) "), (CString) sex, sExMem);
		else
			sSexClause.Format(_T("WHERE ( (%s ) ) "), sExMem);
	}
	else {
		if (sex!=NULL) 
			sSexClause.Format(_T("WHERE ( (`mem_ind`.`sex`)=%s ) "), (CString) sex);
		else
			sSexClause.Format(_T(""));
	}
	if (sSex!=NULL) {
		sTabExt.Format(_T("_%s"), (CString) sSex);
	}

	switch(currElt) {

	case MEM:

		sSQL = "";
		break;

	case HH:
		
		if(exmem) {
			if (sex!=NULL) {
				sSQL = _T("SELECT CONCAT(regn, prov, mun, brgy, purok, hcn) AS `hhID`, \n\
						  IFNULL(COUNT(`mem`.`memID`), 0) AS `hh_") + sExVar + _T("TotMem") + sTabExt + _T("` \n\
						  FROM `mem` INNER JOIN `hpq_mem` USING (regn, prov, mun, brgy, purok, hcn, memno)  \n\
						  ") + sSexClause + _T("\n\
						  GROUP BY regn, prov, mun, brgy, purok, hcn \n\
						  ;");	
			}
			else {
			
				sSQL = _T("SELECT CONCAT(regn, prov, mun, brgy, purok, hcn) AS `hhID`, \n\
						  IFNULL(COUNT(`mem`.`memID`), 0) AS `hh_") + sExVar + _T("TotMem` \n\
						  FROM `mem` INNER JOIN `hpq_mem` USING (regn, prov, mun, brgy, purok, hcn, memno)  \n\
						  ") + sSexClause + _T("\n\
						  GROUP BY regn, prov, mun, brgy, purok, hcn \n\
						  ;");
			}
		}
		else {
			if (sex!=NULL) {
				sSQL = _T("SELECT CONCAT(regn, prov, mun, brgy, purok, hcn) AS `hhID`, \n\
						  IFNULL(COUNT(`Mem_Ind`.`memID`), 0) AS `hh_TotMem") + sTabExt + _T("` \n\
						  FROM `Mem_Ind` \n\
						  ") + sSexClause + _T("\n\
						  GROUP BY regn, prov, mun, brgy, purok, hcn \n\
						  ;");	
			}
			else {
			
				sSQL = _T("SELECT `hh`.`hhID` AS `hhID`, `regn`.`Region` AS `Region`, `prov`.`Province` AS `Province`, `mun`.`Municipality` AS `Municipality`, `brgy`.`Barangay` AS `Barangay`, `purok`.`PurokName` AS `PurokName`, `hh`.`hhHead` as `hhHead`, \n\
							IFNULL(COUNT(`Mem_Ind`.`memID`), 0) AS `hh_TotMem` \n\
							FROM `hh` INNER JOIN `mem_ind` USING (regn, prov, mun, brgy, purok, hcn) INNER JOIN `purok` USING (regn, prov, mun, brgy, purok) INNER JOIN `brgy` USING (regn, prov, mun, brgy) INNER JOIN `mun` USING (regn, prov, mun) INNER JOIN `prov` USING (regn, prov)  INNER JOIN `regn` USING (regn) \n\
							GROUP BY regn, prov, mun, brgy, purok, hcn \n\
							;");
			}
		}

		break;

	case PUROK:
		
		if(exmem) {
			sSQL = _T("SELECT CONCAT(mem.regn, mem.prov, mem.mun, mem.brgy, mem.purok) AS `purokID`, \n\
					  COUNT(`mem`.`memID`) AS `purok_") + sExVar + _T("TotMem") + sTabExt + _T("` \n\
					  FROM `mem` INNER JOIN `hpq_mem` USING (regn, prov, mun, brgy, purok, hcn, memno)  \n\
					  ") + sSexClause + _T("\n\
					  GROUP BY regn, prov, mun, brgy, purok \n\
					  ;");	
		}
		else {
			sSQL = _T("SELECT CONCAT(regn, prov, mun, brgy, purok) AS `purokID`, \n\
					  COUNT(`Mem_Ind`.`memID`) AS `purok_TotMem") + sTabExt + _T("` \n\
					  FROM `Mem_Ind` \n\
					  ") + sSexClause + _T("\n\
					  GROUP BY regn, prov, mun, brgy, purok \n\
					  ;");	
		}
		break;

	case BRGY:
		
		if(exmem) {
			sSQL = _T("SELECT CONCAT(mem.regn, mem.prov, mem.mun, mem.brgy) AS `brgyID`, \n\
					  COUNT(`mem`.`memID`) AS `brgy_") + sExVar + _T("TotMem") + sTabExt + _T("` \n\
					  FROM `mem` INNER JOIN `hpq_mem` USING (regn, prov, mun, brgy, purok, hcn, memno)  \n\
					  ") + sSexClause + _T("\n\
					  GROUP BY regn, prov, mun, brgy \n\
					  ;");	
		}
		else {
			sSQL = _T("SELECT CONCAT(regn, prov, mun, brgy) AS `brgyID`, \n\
					  COUNT(`Mem_Ind`.`memID`) AS `brgy_TotMem") + sTabExt + _T("` \n\
					  FROM `Mem_Ind` \n\
					  ") + sSexClause + _T("\n\
					  GROUP BY regn, prov, mun, brgy \n\
					  ;");	
		}

		break;

	case MUN:
		
		if(exmem) {
			sSQL = _T("SELECT CONCAT(mem.regn, mem.prov, mem.mun, '000') AS `munID`, \n\
					  COUNT(`mem`.`memID`) AS `mun_") + sExVar + _T("TotMem") + sTabExt + _T("` \n\
					  FROM `mem` INNER JOIN `hpq_mem` USING (regn, prov, mun, brgy, purok, hcn, memno)  \n\
					  ") + sSexClause + _T("\n\
					  GROUP BY regn, prov, mun \n\
					  ;");	
		}
		else {
			sSQL = _T("SELECT CONCAT(regn, prov, mun, '000') AS `munID`, \n\
					  COUNT(`Mem_Ind`.`memID`) AS `mun_TotMem") + sTabExt + _T("` \n\
					  FROM `Mem_Ind` \n\
					  ") + sSexClause + _T("\n\
					  GROUP BY regn, prov, mun \n\
					  ;");	
		}

		break;

	case PROV:
		
		if(exmem) {
			sSQL = _T("SELECT CONCAT(mem.regn, mem.prov, '00000') AS `provID`, \n\
					  COUNT(`mem`.`memID`) AS `prov_") + sExVar + _T("TotMem") + sTabExt + _T("` \n\
					  FROM `mem` INNER JOIN `hpq_mem` USING (regn, prov, mun, brgy, purok, hcn, memno)  \n\
					  ") + sSexClause + _T("\n\
					  GROUP BY regn, prov \n\
					  ;");	
		}
		else {
			sSQL = _T("SELECT CONCAT(regn, prov, '00000') AS `provID`, \n\
					  COUNT(`Mem_Ind`.`memID`) AS `prov_TotMem") + sTabExt + _T("` \n\
					  FROM `Mem_Ind` \n\
					  ") + sSexClause + _T("\n\
					  GROUP BY regn, prov \n\
					  ;");	
		}

		break;

	case REGN:
		
		if(exmem) {
			sSQL = _T("SELECT CONCAT(mem.regn, '0000000') AS `regnID`, \n\
					  COUNT(`mem`.`memID`) AS `regn_") + sExVar + _T("TotMem") + sTabExt + _T("` \n\
					  FROM `mem` INNER JOIN `hpq_mem` USING (regn, prov, mun, brgy, purok, hcn, memno)  \n\
					  ") + sSexClause + _T("\n\
					  GROUP BY regn \n\
					  ;");	
		}
		else {
			sSQL = _T("SELECT CONCAT(regn, '0000000') AS `regnID`, \n\
					  COUNT(`Mem_Ind`.`memID`) AS `regn_TotMem") + sTabExt + _T("` \n\
					  FROM `Mem_Ind` \n\
					  ") + sSexClause + _T("\n\
					  GROUP BY regn \n\
					  ;");	
		}

		break;
		
	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}
	return sSQL;

}

CString CStatSimElt::sSQLTotPop()
{

	CString sSQL;

	switch(currElt) {

	case MEM:
		
		sSQL = _T("");		
		break;

	case HH:
		
		sSQL.Format(_T("CREATE TABLE IF NOT EXISTS `hh_TotPop` (\n\
			`hhID` varchar(%d), \n\
			`Region` varchar(50), \n\
			`Province` varchar(50), \n\
			`Municipality` varchar(50), \n\
			`Barangay` varchar(100), \n\
			`PurokName` varchar(50), \n\
			`hhHead` varchar(100), \n\
			`hh_TotMem` int, \n\
			`hh_TotMem_Male` int, \n\
			`hh_TotMem_Female` int, \n\
			`hh_FoTotMem` int, \n\
			`hh_FoTotMem_Male` int, \n\
			`hh_FoTotMem_Female` int, \n\
			`hh_FowTotMem` int, \n\
			`hh_FowTotMem_Male` int, \n\
			`hh_FowTotMem_Female` int, \n\
			PRIMARY KEY (`hhID`), INDEX hhIndex (`hhID`, `Region`, `Province`, `Municipality`, `Barangay`, `PurokName`, `hhHead`) \n\
			);"),
			hh_dig);

		break;
	
	case PUROK:
		
		sSQL.Format(_T("CREATE TABLE IF NOT EXISTS `Purok_TotPop` (\n\
			`purokID` varchar(%d), \n\
			`Region` varchar(50), \n\
			`Province` varchar(50), \n\
			`Municipality` varchar(50), \n\
			`Barangay` varchar(100), \n\
			`PurokName` varchar(50), \n\
			`Purok_TotHH` int, \n\
			`Purok_TotMem` int, \n\
			`Purok_TotMem_Male` int, \n\
			`Purok_TotMem_Female` int, \n\
			`Purok_FoTotMem` int, \n\
			`Purok_FoTotMem_Male` int, \n\
			`Purok_FoTotMem_Female` int, \n\
			`Purok_FowTotMem` int, \n\
			`Purok_FowTotMem_Male` int, \n\
			`Purok_FowTotMem_Female` int, \n\
			PRIMARY KEY (`purokID`), INDEX purokIndex (`purokID`, `Region`, `Province`, `Municipality`, `Barangay`, `PurokName`) \n\
			);"),
			purok_dig);

		break;

	case BRGY:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `Brgy_TotPop` (\n\
			`brgyID` varchar(9), \n\
			`Region` varchar(50), \n\
			`Province` varchar(50), \n\
			`Municipality` varchar(50), \n\
			`Barangay` varchar(100), \n\
			`Brgy_TotHH` int, \n\
			`Brgy_TotMem` int, \n\
			`Brgy_TotMem_Male` int, \n\
			`Brgy_TotMem_Female` int, \n\
			`Brgy_FoTotMem` int, \n\
			`Brgy_FoTotMem_Male` int, \n\
			`Brgy_FoTotMem_Female` int, \n\
			`Brgy_FowTotMem` int, \n\
			`Brgy_FowTotMem_Male` int, \n\
			`Brgy_FowTotMem_Female` int, \n\
			PRIMARY KEY (`brgyID`), INDEX brgyIndex (`brgyID`, `Region`, `Province`, `Municipality`, `Barangay`) \n\
			);");

		break;

	case MUN:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `Mun_TotPop` (\n\
			`munID` varchar(9), \n\
			`Region` varchar(50), \n\
			`Province` varchar(50), \n\
			`Municipality` varchar(50), \n\
			`Mun_TotHH` int, \n\
			`Mun_TotMem` int, \n\
			`Mun_TotMem_Male` int, \n\
			`Mun_TotMem_Female` int, \n\
			`Mun_FoTotMem` int, \n\
			`Mun_FoTotMem_Male` int, \n\
			`Mun_FoTotMem_Female` int, \n\
			`Mun_FowTotMem` int, \n\
			`Mun_FowTotMem_Male` int, \n\
			`Mun_FowTotMem_Female` int, \n\
			PRIMARY KEY (`munID`), INDEX munIndex (`munID`, `Region`, `Province`, `Municipality`) \n\
			);");

		break;

	case PROV:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `Prov_TotPop` (\n\
			`provID` varchar(9), \n\
			`Region` varchar(50), \n\
			`Province` varchar(50), \n\
			`Prov_TotHH` int, \n\
			`Prov_TotMem` int, \n\
			`Prov_TotMem_Male` int, \n\
			`Prov_TotMem_Female` int, \n\
			`Prov_FoTotMem` int, \n\
			`Prov_FoTotMem_Male` int, \n\
			`Prov_FoTotMem_Female` int, \n\
			`Prov_FowTotMem` int, \n\
			`Prov_FowTotMem_Male` int, \n\
			`Prov_FowTotMem_Female` int, \n\
			PRIMARY KEY (`provID`), INDEX provIndex (`provID`, `Region`, `Province`) \n\
			);");

		break;

	case REGN:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `Regn_TotPop` (\n\
			`regnID` varchar(9), \n\
			`Region` varchar(50), \n\
			`Regn_TotHH` int, \n\
			`Regn_TotMem` int, \n\
			`Regn_TotMem_Male` int, \n\
			`Regn_TotMem_Female` int, \n\
			`Regn_FoTotMem` int, \n\
			`Regn_FoTotMem_Male` int, \n\
			`Regn_FoTotMem_Female` int, \n\
			`Regn_FowTotMem` int, \n\
			`Regn_FowTotMem_Male` int, \n\
			`Regn_FowTotMem_Female` int, \n\
			PRIMARY KEY (`regnID`), INDEX regnIndex (`regnID`, `Region`) \n\
			);");

		break;
		
	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}
	return sSQL;

}

CString CStatSimElt::sSQLCCI()
{

	CString sSQL;

	switch(currElt) {

	case MEM:
		
		sSQL = _T("");		
		break;

	case HH:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `hh_CCI` (\n\
			SELECT `hh_CoreInd`.`hhID` AS `hhID`, `hh_coreind`.`hcn`, `hh_CoreInd`.`Region` AS `Region`, `hh_CoreInd`.`Province` AS `Province`, `hh_CoreInd`.`Municipality` AS `Municipality`, `hh_CoreInd`.`Barangay` AS `Barangay`, `hh_CoreInd`.`PurokName` AS `PurokName`, `hh_CoreInd`.`hhHead` AS `hhHead`, \n\
			( `wMaln05` + `wDeath04` + `wDeathPreg` + `Squat` + `MSH` + `ntSWS` + `ntSTF` + `wntsch615` + `Povp` + `Subp` + `Fshort` + `wUnempl15ab` + `wVictcr` ) AS `hh_CCI` \n\
			FROM `hh_CoreInd` \n\
			ORDER BY `hh_CCI` \n\
			);");

		break;
	
	case PUROK:

		sSQL = _T("CREATE TABLE IF NOT EXISTS `purok_CCI` (\n\
				  SELECT LEFT(`hhid`, 11) as `purokID`, `hh_cci`.`Region` AS `Region`, `hh_cci`.`Province` AS `Province`, `hh_cci`.`Municipality` AS `Municipality`, `hh_cci`.`Barangay` AS `Barangay`, `hh_cci`.`PurokName` AS `PurokName`, \n\
				  AVG(`hh_CCI`.`hh_CCI`) AS `purok_CCI`, MAX(`hh_CCI`.`hh_CCI`) AS `purok_maxCCI`, MIN(`hh_CCI`.`hh_CCI`) AS `purok_minCCI`, STD(`hh_CCI`.`hh_CCI`) AS `purok_sdCCI`  \n\
				  FROM `hh` INNER JOIN `hh_CCI` USING (`hhid`) \n\
				  GROUP BY `regn`, `prov`, `brgy`, `purok` \n\
				  ORDER BY `purok_CCI` \n\
				  );");

		break;

	case BRGY:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `brgy_CCI` (\n\
				  SELECT LEFT(`hhid`, 9) as `brgyID`, `hh_cci`.`Region` AS `Region`, `hh_cci`.`Province` AS `Province`, `hh_cci`.`Municipality` AS `Municipality`, `hh_cci`.`Barangay` AS `Barangay`, \n\
				  AVG(`hh_CCI`.`hh_CCI`) AS `brgy_CCI`, MAX(`hh_CCI`.`hh_CCI`) AS `brgy_maxCCI`, MIN(`hh_CCI`.`hh_CCI`) AS `brgy_minCCI`, STD(`hh_CCI`.`hh_CCI`) AS `brgy_sdCCI`  \n\
				  FROM `hh` INNER JOIN `hh_CCI` USING (`hhid`) \n\
				  GROUP BY `regn`, `prov`, `mun`, `brgy` \n\
				  ORDER BY `brgy_CCI` \n\
				  );");



		break;

	case MUN:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `mun_CCI` (\n\
				  SELECT CONCAT(LEFT(`hhid`, 6), '000') as `munID`, `hh_cci`.`Region` AS `Region`, `hh_cci`.`Province` AS `Province`, `hh_cci`.`Municipality` AS `Municipality`, \n\
				  AVG(`hh_CCI`.`hh_CCI`) AS `mun_CCI`, MAX(`hh_CCI`.`hh_CCI`) AS `mun_maxCCI`, MIN(`hh_CCI`.`hh_CCI`) AS `mun_minCCI`, STD(`hh_CCI`.`hh_CCI`) AS `mun_sdCCI`  \n\
				  FROM `hh` INNER JOIN `hh_CCI` USING (`hhid`) \n\
				  GROUP BY `regn`, `prov`, `mun` \n\
				  ORDER BY `mun_CCI` \n\
				  );");

		break;

	case PROV:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `prov_CCI` (\n\
				  SELECT CONCAT(LEFT(`hhid`, 4), '00000') as `provID`, `hh_cci`.`Region` AS `Region`, `hh_cci`.`Province` AS `Province`, \n\
				  AVG(`hh_CCI`.`hh_CCI`) AS `prov_CCI`, MAX(`hh_CCI`.`hh_CCI`) AS `prov_maxCCI`, MIN(`hh_CCI`.`hh_CCI`) AS `prov_minCCI`, STD(`hh_CCI`.`hh_CCI`) AS `prov_sdCCI`  \n\
				  FROM `hh` INNER JOIN `hh_CCI` USING (`hhid`) \n\
				  GROUP BY `regn`, `prov` \n\
				  ORDER BY `prov_CCI` \n\
				  );");

		break;

	case REGN:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `regn_CCI` (\n\
				  SELECT CONCAT(LEFT(`hhid`, 2), '0000000') as `regnID`, `hh_cci`.`Region` AS `Region`, \n\
				  AVG(`hh_CCI`.`hh_CCI`) AS `regn_CCI`, MAX(`hh_CCI`.`hh_CCI`) AS `regn_maxCCI`, MIN(`hh_CCI`.`hh_CCI`) AS `regn_minCCI`, STD(`hh_CCI`.`hh_CCI`) AS `regn_sdCCI`  \n\
				  FROM `hh` INNER JOIN `hh_CCI` USING (`hhid`) \n\
				  GROUP BY `regn` \n\
				  ORDER BY `regn_CCI` \n\
				  );");

		break;
		
	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}
	return sSQL;

}

CString CStatSimElt::sSQLLogit(float cutoff)
{

	if (currElt!=HH) {
		//AfxMessageBox(_T("Not applicable."));
		//return _T("");
	}

	CStatSimRS *pRS = 0;

	CString sSQL, sTerm, sCombi, sCutOff;
	switch(currElt) {

	case MEM:
		
		sSQL = _T("");		
		break;

	case HH:
		
		sSQL = _T("SELECT * FROM `~logit` ORDER BY `constant`;");
		pRS = new CStatSimRS( pEltDB, sSQL );
		sCutOff.Format(_T("%.4f"), cutoff);
		if (pRS->GetRecordCount()>0)
			pRS->MoveFirst();
		
		sCombi = _T("");
		for (int i=0; i<pRS->GetRecordCount(); i++) {
            
			//constant?
			BOOL IsCons = _ttoi(pRS->SQLFldValue("constant"));
			//set variable ito blank if constant
			CString sVar;
			if (IsCons)
				sVar = _T("");
			else
				sVar = _T("*") + pRS->SQLFldValue("variable");

			if (i<pRS->GetRecordCount()-1) {
				sCombi = sCombi + pRS->SQLFldValue(_MBCS("coefficient")) + sVar + _T(" + ");
			}
			else {
				sCombi = sCombi + pRS->SQLFldValue(_MBCS("coefficient")) + sVar;
			}
			pRS->MoveNext();
		}

		sSQL = _T("CREATE TABLE IF NOT EXISTS `hh_logit` (\n\
			SELECT `hh_CoreInd`.`hhID` AS `hhID`, `hh_CoreInd`.`Region` AS `Region`, `hh_CoreInd`.`Province` AS `Province`, `hh_CoreInd`.`Municipality` AS `Municipality`, `hh_CoreInd`.`Barangay` AS `Barangay`, `hh_CoreInd`.`PurokName` AS `PurokName`, \n\
			`hh_CoreInd`.`hhHead` AS `hhHead`, `povp` as `hh_povp`, (") + sCombi + _T(") as `hh_logit`, \n\
			(EXP(") + sCombi + _T(")/(1+EXP(") + sCombi + _T("))) as `hh_phat`, \n\
			If((EXP(") + sCombi + _T(")/(1+EXP(") + sCombi + _T(")))<") + sCutOff + _T(", 1, 0) as `hh_povphat` \n\
			FROM `hh_CoreInd` \n\
			ORDER BY `hh_phat` \n\
			);");
		
		delete pRS; pRS = 0;
		break;

	case PUROK:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `purok_LOGIT` (\n\
			SELECT `purok_TotPop`.`purokID` AS `purokID`, `purok_TotPop`.`Region` AS `Region`, `purok_TotPop`.`Province` AS `Province`, `purok_TotPop`.`Municipality` AS `Municipality`, `purok_TotPop`.`Barangay` AS `Barangay`, `purok_TotPop`.`PurokName` AS `PurokName`, \n\
			`purok_tothh`, \n\
			SUM(`hh_LOGIT`.`hh_povp`) AS `purok_povp`, \n\
			SUM(`hh_LOGIT`.`hh_povphat`) AS `purok_povphat` \n\
			FROM `purok_TotPop` INNER JOIN `hh_LOGIT` ON (LEFT(`purok_TotPop`.`purokID`, 11) = LEFT(`hh_LOGIT`.`hhID`, 11)) \n\
			GROUP BY `purokID` \n\
			ORDER BY `purok_povphat` \n\
			);");

		break;

	case BRGY:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `brgy_LOGIT` (\n\
			SELECT `brgy_TotPop`.`brgyID` AS `brgyID`, `brgy_TotPop`.`Region` AS `Region`, `brgy_TotPop`.`Province` AS `Province`, `brgy_TotPop`.`Municipality` AS `Municipality`, `brgy_TotPop`.`Barangay` AS `Barangay`, \n\
			`brgy_tothh`, \n\
			SUM(`hh_LOGIT`.`hh_povp`) AS `brgy_povp`, \n\
			SUM(`hh_LOGIT`.`hh_povphat`) AS `brgy_povphat` \n\
			FROM `brgy_TotPop` INNER JOIN `hh_LOGIT` ON (LEFT(`brgy_TotPop`.`brgyID`, 9) = LEFT(`hh_LOGIT`.`hhID`, 9)) \n\
			GROUP BY `brgyID` \n\
			ORDER BY `brgy_povphat` \n\
			);");

		break;

	case MUN:
		

		sSQL = _T("CREATE TABLE IF NOT EXISTS `mun_LOGIT` (\n\
			SELECT `mun_TotPop`.`munID` AS `munID`, `mun_TotPop`.`Region` AS `Region`, `mun_TotPop`.`Province` AS `Province`, `mun_TotPop`.`Municipality` AS `Municipality`, \n\
			`mun_tothh`, \n\
			SUM(`hh_LOGIT`.`hh_povp`) AS `mun_povp`, \n\
			SUM(`hh_LOGIT`.`hh_povphat`) AS `mun_povphat` \n\
			FROM `mun_TotPop` INNER JOIN `hh_LOGIT` ON (LEFT(`mun_TotPop`.`munID`, 6) = LEFT(`hh_LOGIT`.`hhID`, 6)) \n\
			GROUP BY `munID` \n\
			ORDER BY `mun_povphat` \n\
			);");

		break;

	case PROV:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `prov_LOGIT` (\n\
			SELECT `prov_TotPop`.`provID` AS `provID`, `prov_TotPop`.`Region` AS `Region`, `prov_TotPop`.`Province` AS `Province`, \n\
			`prov_tothh`, \n\
			SUM(`hh_LOGIT`.`hh_povp`) AS `prov_povp`, \n\
			SUM(`hh_LOGIT`.`hh_povphat`) AS `prov_povphat` \n\
			FROM `prov_TotPop` INNER JOIN `hh_LOGIT` ON (LEFT(`prov_TotPop`.`provID`, 4) = LEFT(`hh_LOGIT`.`hhID`, 4)) \n\
			GROUP BY `provID` \n\
			ORDER BY `prov_povphat` \n\
			);");

		break;

	case REGN:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `regn_LOGIT` (\n\
			SELECT `regn_TotPop`.`regnID` AS `regnID`, `regn_TotPop`.`Region` AS `Region`, \n\
			`regn_tothh`, \n\
			SUM(`hh_LOGIT`.`hh_povp`) AS `regn_povp`, \n\
			SUM(`hh_LOGIT`.`hh_povphat`) AS `regn_povphat` \n\
			FROM `regn_TotPop` INNER JOIN `hh_LOGIT` ON (LEFT(`regn_TotPop`.`regnID`, 2) = LEFT(`hh_LOGIT`.`hhID`, 2)) \n\
			GROUP BY `regnID` \n\
			ORDER BY `regn_povphat` \n\
			);");

		break;
		
	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}

	return sSQL;

}


CString CStatSimElt::sSQLSelectMDG(float perday)
{

	if (currElt==HH) {
		AfxMessageBox(_T("Not applicable."));
		return _T("");
	}
	CString sSQL, sSQLInit = _T(""), sSQLBody = _T(""), sSQLIndex = _T(""), 
		sE, sPerDay;

	sE = Attr(element);
	sPerDay.Format(_T("%.2f"), perday);
	
	switch(currElt) {

	case MEM:
		
		sSQLInit = _T("");		
		break;

	case HH:		

		sSQLInit = _T("");
		
		break;

	case PUROK:
		
		sSQLInit = "SELECT `purok_coreind`.`purokID` AS `purokID`, `purok_coreind`.`Region` AS `Region`, `purok_coreind`.`Province` AS `Province`, `purok_coreind`.`Municipality` AS `Municipality`, `purok_coreind`.`Barangay` AS `Barangay`, `purok_coreind`.`PurokName` AS `PurokName`, \n";

		break;

	case BRGY:
		
		sSQLInit = "SELECT `brgy_coreind`.`brgyID` AS `brgyID`, `brgy_coreind`.`Region` AS `Region`, `brgy_coreind`.`Province` AS `Province`, `brgy_coreind`.`Municipality` AS `Municipality`, `brgy_coreind`.`Barangay` AS `Barangay`, \n";

		break;

	case MUN:
		

		sSQLInit = "SELECT `mun_coreind`.`munID` AS `munID`, `mun_coreind`.`Region` AS `Region`, `mun_coreind`.`Province` AS `Province`, `mun_coreind`.`Municipality` AS `Municipality`, \n ";

		break;

	case PROV:
		
		sSQLInit = "SELECT `prov_coreind`.`provID` AS `provID`, `prov_coreind`.`Region` AS `Region`, `prov_coreind`.`Province` AS `Province`, \n ";

		break;

	case REGN:
		
		sSQLInit = "SELECT `regn_coreind`.`regnID` AS `regnID`, `regn_coreind`.`Region` AS `Region`, \n ";

		break;
		
	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}

	sSQLBody = _T("`") + sE + _T("_TotHH`, \n\
			`") + sE + _T("_TotMem`, \n\
			`") + sE + _T("_TotMem_Male`, \n\
			`") + sE + _T("_TotMem_Female`, \n\
			`") + sE + _T("_Povp`, `") + sE + _T("_Povp_Prop`, \n\
			`") + sE + _T("_MemPovp`, \n\
			`") + sE + _T("_MemPovp_Male`, \n\
			`") + sE + _T("_MemPovp_Female`, \n\
			`") + sE + _T("_MemPovp_Prop`, \n\
			`") + sE + _T("_MemPovp_Male_Prop`, \n\
			`") + sE + _T("_MemPovp_Female_Prop`, \n\
			`") + sE + _T("_Subp`, `") + sE + _T("_Subp_Prop`, \n\
			`") + sE + _T("_MemSubp`, \n\
			`") + sE + _T("_MemSubp_Male`, \n\
			`") + sE + _T("_MemSubp_Female`, \n\
			`") + sE + _T("_MemSubp_Prop`, \n\
			`") + sE + _T("_MemSubp_Male_Prop`, \n\
			`") + sE + _T("_MemSubp_Female_Prop`, \n\
			`") + sE + _T("_Fshort`, `") + sE + _T("_Fshort_Prop`, \n\
			`") + sE + _T("_MemFshort`, \n\
			`") + sE + _T("_MemFshort_Male`, \n\
			`") + sE + _T("_MemFshort_Female`, \n\
			`") + sE + _T("_MemFshort_Prop`, \n\
			`") + sE + _T("_MemFshort_Male_Prop`, \n\
			`") + sE + _T("_MemFshort_Female_Prop`, \n\
			`") + sE + _T("_HHwMem05`, \n\
			`") + sE + _T("_HHwMem05_Prop`, \n\
			`") + sE + _T("_HHwMem04d`, \n\
			`") + sE + _T("_HHwMem04d_Prop`, \n\
			`") + sE + _T("_Mem01`, \n\
			`") + sE + _T("_Mem01_Male`, \n\
			`") + sE + _T("_Mem01_Female`, \n\
			`") + sE + _T("_Mem05`, \n\
			`") + sE + _T("_Mem05_Male`, \n\
			`") + sE + _T("_Mem05_Female`, \n\
			`") + sE + _T("_HHwMaln05`, \n\
			`") + sE + _T("_HHwMaln05_Prop`, \n\
			`") + sE + _T("_Maln05`, \n\
			`") + sE + _T("_Maln05_Male`, \n\
			`") + sE + _T("_Maln05_Female`, \n\
			`") + sE + _T("_Maln05_Prop`, \n\
			`") + sE + _T("_Maln05_Male_Prop`, \n\
			`") + sE + _T("_Maln05_Female_Prop`, \n\
			`") + sE + _T("_maln04`, \n\
			`") + sE + _T("_maln04_Male`, \n\
			`") + sE + _T("_maln04_Female`, \n\
			`") + sE + _T("_maln04_Prop`, \n\
			`") + sE + _T("_maln04_Male_Prop`, \n\
			`") + sE + _T("_maln04_Female_Prop`, \n\
			`") + sE + _T("_HHwMem611`, \n\
			`") + sE + _T("_HHwMem611_Prop`, \n\
			`") + sE + _T("_Mem611`, \n\
			`") + sE + _T("_Mem611_Male`, \n\
			`") + sE + _T("_Mem611_Female`, \n\
			`") + sE + _T("_HHwmem611` - `") + sE + _T("_HHwntelem611` AS `") + sE + _T("_HHwelem611`, \n\
			100 - `") + sE + _T("_HHwntelem611_Prop` AS `") + sE + _T("_HHwelem611_Prop`, \n\
			`") + sE + _T("_mem611` - `") + sE + _T("_ntelem611` AS `") + sE + _T("_elem611`, \n\
			`") + sE + _T("_mem611_male` - `") + sE + _T("_ntelem611_male` AS `") + sE + _T("_elem611_male`, \n\
			`") + sE + _T("_mem611_female` - `") + sE + _T("_ntelem611_female` AS `") + sE + _T("_elem611_female`, \n\
			100 - `") + sE + _T("_ntelem611_Prop` AS `") + sE + _T("_elem611_Prop`, \n\
			100 - `") + sE + _T("_ntelem611_male_Prop` AS `") + sE + _T("_elem611_male_Prop`, \n\
			100 - `") + sE + _T("_ntelem611_female_Prop` AS `") + sE + _T("_elem611_female_Prop`, \n\
			`") + sE + _T("_HHwMem1215`, \n\
			`") + sE + _T("_HHwMem1215_Prop`, \n\
			`") + sE + _T("_Mem1215`, \n\
			`") + sE + _T("_Mem1215_Male`, \n\
			`") + sE + _T("_Mem1215_Female`, \n\
			`") + sE + _T("_HHwmem1215` - `") + sE + _T("_HHwnths1215` AS `") + sE + _T("_HHwhs1215`, \n\
			100 - `") + sE + _T("_HHwnths1215_Prop` AS `") + sE + _T("_HHwhs1215_Prop`, \n\
			`") + sE + _T("_mem1215` - `") + sE + _T("_nths1215` AS `") + sE + _T("_hs1215`, \n\
			`") + sE + _T("_mem1215_male` - `") + sE + _T("_nths1215_male` AS `") + sE + _T("_hs1215_male`, \n\
			`") + sE + _T("_mem1215_female` - `") + sE + _T("_nths1215_female` AS `") + sE + _T("_hs1215_female`, \n\
			100 - `") + sE + _T("_nths1215_Prop` AS `") + sE + _T("_hs1215_Prop`, \n\
			100 - `") + sE + _T("_nths1215_male_Prop` AS `") + sE + _T("_hs1215_male_Prop`, \n\
			100 - `") + sE + _T("_nths1215_female_Prop` AS `") + sE + _T("_hs1215_female_Prop`, \n\
			`") + sE + _T("_HHwMem615`, \n\
			`") + sE + _T("_HHwMem615_Prop`, \n\
			`") + sE + _T("_Mem615`, \n\
			`") + sE + _T("_Mem615_Male`, \n\
			`") + sE + _T("_Mem615_Female`, \n\
			`") + sE + _T("_HHwmem615` - `") + sE + _T("_HHwntsch615` AS `") + sE + _T("_HHwsch615`, \n\
			100 - `") + sE + _T("_HHwntsch615_Prop` AS `") + sE + _T("_HHwsch615_Prop`, \n\
			`") + sE + _T("_mem615` - `") + sE + _T("_ntsch615` AS `") + sE + _T("_sch615`, \n\
			`") + sE + _T("_mem615_male` - `") + sE + _T("_ntsch615_male` AS `") + sE + _T("_sch615_male`, \n\
			`") + sE + _T("_mem615_female` - `") + sE + _T("_ntsch615_female` AS `") + sE + _T("_sch615_female`, \n\
			100 - `") + sE + _T("_ntsch615_Prop` AS `") + sE + _T("_sch615_Prop`, \n\
			100 - `") + sE + _T("_ntsch615_male_Prop` AS `") + sE + _T("_sch615_male_Prop`, \n\
			100 - `") + sE + _T("_ntsch615_female_Prop` AS `") + sE + _T("_sch615_female_Prop`, \n\
			`") + sE + _T("_HHwdeath04`, \n\
			`") + sE + _T("_HHwdeath04_Prop`, \n\
			`") + sE + _T("_death04`, \n\
			`") + sE + _T("_death04_Male`, \n\
			`") + sE + _T("_death04_Female`, \n\
			`") + sE + _T("_death04_Prop`, \n\
			`") + sE + _T("_death04_Male_Prop`, \n\
			`") + sE + _T("_death04_Female_Prop`, \n\
			`") + sE + _T("_HHwDeathPreg`, \n\
			`") + sE + _T("_HHwDeathPreg_Prop`, \n\
			`") + sE + _T("_deathpreg`, \n\
			`") + sE + _T("_deathpreg_Male`, \n\
			`") + sE + _T("_deathpreg_Female`, \n\
			`") + sE + _T("_deathpreg_Prop`, \n\
			`") + sE + _T("_deathpreg_Male_Prop`, \n\
			`") + sE + _T("_deathpreg_Female_Prop`, \n\
			`") + sE + _T("_squat`, \n\
			`") + sE + _T("_squat_Prop`, \n\
			`") + sE + _T("_Memsquat`, \n\
			`") + sE + _T("_Memsquat_Male`, \n\
			`") + sE + _T("_Memsquat_Female`, \n\
			`") + sE + _T("_Memsquat_Prop`, \n\
			`") + sE + _T("_Memsquat_Male_Prop`, \n\
			`") + sE + _T("_Memsquat_Female_Prop`, \n\
			`") + sE + _T("_msh`, \n\
			`") + sE + _T("_msh_Prop`, \n\
			`") + sE + _T("_Memmsh`, \n\
			`") + sE + _T("_Memmsh_Male`, \n\
			`") + sE + _T("_Memmsh_Female`, \n\
			`") + sE + _T("_Memmsh_Prop`, \n\
			`") + sE + _T("_Memmsh_Male_Prop`, \n\
			`") + sE + _T("_Memmsh_Female_Prop` \n FROM \
			`") + sE + _T("_coreind`;");

/*			`") + sE + _T("_HHwMem10ab`, \n\
			`") + sE + _T("_HHwMem10ab_Prop`, \n\
			`") + sE + _T("_Mem10ab`, \n\
			`") + sE + _T("_Mem10ab_Male`, \n\
			`") + sE + _T("_Mem10ab_Female`, \n\
*/
	sSQL = sSQLInit + sSQLBody;
	return sSQL;

}

CString CStatSimElt::sSQLCoreInd()
{

	CString sSQL, sSQLInit = _T(""), sSQLBody = _T(""), sSQLIndex = _T(""), sE;

	sE = Attr(element);
	

	switch(currElt) {

	case MEM:
				
		sSQL = _T("");		

		break;

	case HH:

		//Males and females
		sSQLInit = _T("CREATE TABLE IF NOT EXISTS `HH_CoreInd` (\n\
			`hhID` varchar(50), \n\
			`Region` varchar(50), \n\
			`Province` varchar(50), \n\
			`Municipality` varchar(50), \n\
			`Barangay` varchar(50), \n\
			`PurokName` varchar(50), \n\
			`hhHead` varchar(50), \n\
			`hcn` int(6), \n\
			`hcn_NRDB` varchar(6), \n\
			`geopoint_hh_long` double, \n\
			`geopoint_hh_lat` double, \n\
			`hh_totmem` int(2), \n\
			`hh_totmem_male` int(2), \n\
			`hh_totmem_female` int(2), \n\
			`hh_fototmem` int(2), \n\
			`hh_fototmem_male` int(2), \n\
			`hh_fototmem_female` int(2), \n\
			`hh_fowtotmem` int(2), \n\
			`hh_fowtotmem_male` int(2), \n\
			`hh_fowtotmem_female` int(2), \n\
			`nmem04` int(2), \n\
			`nmem04_male` int(2), \n\
			`nmem04_female` int(2), \n\
			`nmem05` int(2), \n\
			`nmem05_male` int(2), \n\
			`nmem05_female` int(2), \n\
			`nmaln05` int(2), \n\
			`nmaln05_male` int(2), \n\
			`nmaln05_female` int(2), \n\
			`ndeath04` int(2), \n\
			`ndeath04_male` int(2), \n\
			`ndeath04_female` int(2), \n\
			`ndeath05` int(2), \n\
			`ndeath05_male` int(2), \n\
			`ndeath05_female` int(2), \n\
			`nmem01` int(2), \n\
			`nmem01_male` int(2), \n\
			`nmem01_female` int(2), \n\
			`ndeath01` int(2), \n\
			`ndeath01_male` int(2), \n\
			`ndeath01_female` int(2), \n\
			`ndeathpreg` int(2), \n\
			`ndeathpreg_male` int(2), \n\
			`ndeathpreg_female` int(2), \n\
			`wMem04` int(1), \n\
			`wMem05` int(1), \n\
			`wMaln05` int(1), \n\
			`HH_wMaln05` varchar(50), \n\
			`wMem04d` int(1), \n\
			`wMem05d` int(1), \n\
			`wDeath04` int(1), \n\
			`HH_wDeath04` varchar(50), \n\
			`wDeath05` int(1), \n\
			`HH_wDeath05` varchar(50), \n\
			`wMem01` int(1), \n\
			`wMem01d` int(1), \n\
			`wDeathPreg` int(1), \n\
			`HH_wDeathPreg` varchar(50), \n\
			`Squat` int(1), \n\
			`nSquat` int(2), \n\
			`nSquat_male` int(2), \n\
			`nSquat_female` int(2), \n\
			`HH_Squat` varchar(50), \n\
			`MSH` int(1), \n\
			`nMSH` int(2), \n\
			`nMSH_male` int(2), \n\
			`nMSH_female` int(2), \n\
			`HH_MSH` varchar(50), \n\
			`ntSWS` int(1), \n\
			`nntSWS` int(2), \n\
			`nntSWS_male` int(2), \n\
			`nntSWS_female` int(2), \n\
			`HH_ntSWS` varchar(50), \n\
			`ntSTF` int(1), \n\
			`nntSTF` int(2), \n\
			`nntSTF_male` int(2), \n\
			`nntSTF_female` int(2), \n\
			`HH_ntSTF` varchar(50), \n\
			`nmem611` int(2), \n\
			`nmem611_male` int(2), \n\
			`nmem611_female` int(2), \n\
			`nntElem611` int(2), \n\
			`nntElem611_male` int(2), \n\
			`nntElem611_female` int(2), \n\
			`nmem1215` int(2), \n\
			`nmem1215_male` int(2), \n\
			`nmem1215_female` int(2), \n\
			`nntHS1215` int(2), \n\
			`nntHS1215_male` int(2), \n\
			`nntHS1215_female` int(2), \n\
			`nmem612` int(2), \n\
			`nmem612_male` int(2), \n\
			`nmem612_female` int(2), \n\
			`nntElem612` int(2), \n\
			`nntElem612_male` int(2), \n\
			`nntElem612_female` int(2), \n\
			`nmem1316` int(2), \n\
			`nmem1316_male` int(2), \n\
			`nmem1316_female` int(2), \n\
			`nntHS1316` int(2), \n\
			`nntHS1316_male` int(2), \n\
			`nntHS1316_female` int(2), \n\
			`nmem615` int(2), \n\
			`nmem615_male` int(2), \n\
			`nmem615_female` int(2), \n\
			`nntSch615` int(2), \n\
			`nntSch615_male` int(2), \n\
			`nntSch615_female` int(2), \n\
			`nmem616` int(2), \n\
			`nmem616_male` int(2), \n\
			`nmem616_female` int(2), \n\
			`nntSch616` int(2), \n\
			`nntSch616_male` int(2), \n\
			`nntSch616_female` int(2), \n\
			`nmem1721` int(2), \n\
			`nmem1721_male` int(2), \n\
			`nmem1721_female` int(2), \n\
			`ntert1721` int(2), \n\
			`ntert1721_male` int(2), \n\
			`ntert1721_female` int(2), \n\
			`nmem10ab` int(2), \n\
			`nmem10ab_male` int(2), \n\
			`nmem10ab_female` int(2), \n\
			`nntLiter10ab` int(2), \n\
			`nntLiter10ab_male` int(2), \n\
			`nntLiter10ab_female` int(2), \n\
			`nmem1524` int(2), \n\
			`nmem1524_male` int(2), \n\
			`nmem1524_female` int(2), \n\
			`nLiter1524` int(2), \n\
			`nLiter1524_male` int(2), \n\
			`nLiter1524_female` int(2), \n\
			`wMem611` int(1), \n\
			`wntElem611` int(1), \n\
			`HH_wntElem611` varchar(50), \n\
			`wMem1215` int(1), \n\
			`wntHS1215` int(1), \n\
			`HH_wntHS1215` varchar(50), \n\
			`wMem612` int(1), \n\
			`wntElem612` int(1), \n\
			`HH_wntElem612` varchar(50), \n\
			`wMem1316` int(1), \n\
			`wntHS1316` int(1), \n\
			`HH_wntHS1316` varchar(50), \n\
			`wMem615` int(1), \n\
			`wntSch615` int(1), \n\
			`HH_wntSch615` varchar(50), \n\
			`wMem616` int(1), \n\
			`wntSch616` int(1), \n\
			`HH_wntSch616` varchar(50), \n\
			`wMem10ab` int(1), \n\
			`wntLiter10ab` int(1), \n\
			`HH_wntLiter10ab` varchar(50), \n\
			`wmem1524` int(1), \n\
			`allliter1524` int(1), \n\
			`wmem1721` int(1), \n\
			`alltert1721` int(1), \n\
			`Povp` int(1), \n\
			`nPovp` int(2), \n\
			`nPovp_male` int(2), \n\
			`nPovp_female` int(2), \n\
			`HH_Povp` varchar(50), \n\
			`Subp` int(1), \n\
			`nSubp` int(2), \n\
			`nSubp_male` int(2), \n\
			`nSubp_female` int(2), \n\
			`HH_Subp` varchar(50), \n\
			`Fshort` int(1), \n\
			`nFShort` int(2), \n\
			`nFShort_male` int(2), \n\
			`nFShort_female` int(2), \n\
			`HH_FShort` varchar(50), \n\
			`nLabFor` int(2), \n\
			`nLabFor_male` int(2), \n\
			`nLabFor_female` int(2), \n\
			`nUnempl15ab` int(2), \n\
			`nUnempl15ab_male` int(2), \n\
			`nUnempl15ab_female` int(2), \n\
			`wlabfor` int(1), \n\
			`wUnempl15ab` int(1), \n\
			`HH_wUnempl15ab` varchar(50), \n\
			`nVictCr` int(2), \n\
			`nVictCr_male` int(2), \n\
			`nVictCr_female` int(2), \n\
			`wVictcr` int(1), \n\
			`HH_wVictCr` varchar(50), \n\
			`hh_totmem_sq` int(4), \n\
			`wtv` int(1), \n\
			`wvhs` int(1), \n\
			`wref` int(1), \n\
			`wwmach` int(1), \n\
			`wairc` int(1), \n\
			`wcar` int(1), \n\
			`wphone` int(1), \n\
			`wcomputer` int(1), \n\
			`wmicrow` int(1), \n\
			`welec` int(1), \n\
			`toilscore` int(1), \n\
			`nmem014` int(2), \n\
			`nmem1564` int(2), \n\
			`nmem65ab` int(2), \n\
			`depratio` float(4,4), \n\
			`hhage` int(3), \n\
			`hheduca2` int(1), \n\
			`hheduca3` int(1), \n\
			`hheduca4` int(1), \n\
			`hheduca5` int(1), \n\
			`hheduca67` int(1), \n\
			`hhntagri` int(1), \n\
			`urb_loc` int(1), \n");
		
		sSQLIndex = _T("PRIMARY KEY (`hhID`), INDEX hhIndex (`hhID`, `Region`, `Province`, `Municipality`, `Barangay`, `PurokName`, `hhHead`) \n");

/*		sSQLInit = _T("CREATE TABLE IF NOT EXISTS `HH_CoreInd` (\n\
			"`hhID` varchar(50), \n\
			"`Region` varchar(50), \n\
			"`Province` varchar(50), \n\
			"`Municipality` varchar(50), \n\
			"`Barangay` varchar(50), \n\
			"`PurokName` varchar(50), \n\
			"`hhHead` varchar(50), \n\
			"`hcn` int(6), \n\
			"`hcn_NRDB` varchar(6), \n\
			"`hh_totmem` int(2), \n\
			"`hh_totmem_male` int(2), \n\
			"`hh_totmem_female` int(2), \n\
			"`nmem05` int(2), \n\
			"`nmaln05` int(2), \n\
			"`ndeath05` int(2), \n\
			"`nmem01` int(2), \n\
			"`ndeathpreg` int(2), \n\
			"`wMaln05` int(1), \n\
			"`HH_wMaln05` varchar(50), \n\
			"`wDeath05` int(1), \n\
			"`HH_wDeath05` varchar(50), \n\
			"`wDeathPreg` int(1), \n\
			"`HH_wDeathPreg` varchar(50), \n\
			"`Squat` int(1), \n\
			"`HH_Squat` varchar(50), \n\
			"`MSH` int(1), \n\
			"`HH_MSH` varchar(50), \n\

			"`ntSWS` int(1), \n\
			"`HH_ntSWS` varchar(50), \n\
			"`ntSTF` int(1), \n\
			"`HH_ntSTF` varchar(50), \n\
			"`nmem612` int(2), \n\
			"`nntElem612` int(2), \n\
			"`nmem1316` int(2), \n\
			"`nntHs1316` int(2), \n\
			"`nmem616` int(2), \n\
			"`nntSch616` int(2), \n\
			"`nmem10ab` int(2), \n\
			"`nntLiter10ab` int(2), \n\
			"`wntElem612` int(1), \n\
			"`HH_wntElem612` varchar(50), \n\
			"`wntHS1316` int(1), \n\
			"`HH_wntHS1316` varchar(50), \n\
			"`wntSch616` int(1), \n\
			"`HH_wntSch616` varchar(50), \n\
			"`wntLiter10ab` int(1), \n\
			"`HH_wntLiter10ab` varchar(50), \n\
			"`Povp` int(1), \n\
			"`HH_Povp` varchar(50), \n\
			"`Subp` int(1), \n\
			"`HH_Subp` varchar(50), \n\
			"`Fshort` int(1), \n\
			"`HH_FShort` varchar(50), \n\
			"`nlabfor` int(2), \n\
			"`nUnempl15ab` int(2), \n\
			"`wUnempl15ab` int(1), \n\
			"`HH_wUnempl15ab` varchar(50), \n\
			"`nVictcr` int(2), \n\
			"`wVictcr` int(1), \n\
			"`HH_wVictCr` varchar(50), \n");
		
		sSQLIndex = _T("PRIMARY KEY (`hhID`), INDEX hhIndex (`hhID`, `Region`, `Province`, `Municipality`, `Barangay`, `PurokName`, `hhHead`) \n");
*/

		
		break;

	case PUROK:

		sSQLInit = _T("CREATE TABLE IF NOT EXISTS `Purok_CoreInd` (\n\
			`purokID` varchar(11), \n\
			`Region` varchar(50), \n\
			`Province` varchar(50), \n\
			`Municipality` varchar(50), \n\
			`Barangay` varchar(50), \n\
			`PurokName` varchar(50), \n");

		sSQLIndex = _T("PRIMARY KEY (`purokID`), INDEX purokIndex (`purokID`, `Region`, `Province`, `Municipality`, `Barangay`, `PurokName`) \n");

		break;

	case BRGY:
		
		sSQLInit = _T("CREATE TABLE IF NOT EXISTS `Brgy_CoreInd` (\n\
			`brgyID` varchar(9), \n\
			`Region` varchar(50), \n\
			`Province` varchar(50), \n\
			`Municipality` varchar(50), \n\
			`Barangay` varchar(50), \n");

		sSQLIndex = _T("PRIMARY KEY (`brgyID`), INDEX brgyIndex (`brgyID`, `Region`, `Province`, `Municipality`, `Barangay`) \n");

		break;

	case MUN:
		
		sSQLInit = _T("CREATE TABLE IF NOT EXISTS `Mun_CoreInd` (\n\
			`munID` varchar(9), \n\
			`Region` varchar(50), \n\
			`Province` varchar(50), \n\
			`Municipality` varchar(50), \n");
		
		sSQLIndex = _T("PRIMARY KEY (`munID`), INDEX munIndex (`munID`, `Region`, `Province`, `Municipality`) \n");

		break;

	case PROV:
		
		sSQLInit = _T("CREATE TABLE IF NOT EXISTS `Prov_CoreInd` (\n\
			`provID` varchar(9), \n\
			`Region` varchar(50), \n\
			`Province` varchar(50), \n");
		
		sSQLIndex = _T("PRIMARY KEY (`provID`), INDEX provIndex (`provID`, `Region`, `Province`) \n");

		break;

	case REGN:
		
		sSQLInit = _T("CREATE TABLE IF NOT EXISTS `Regn_CoreInd` (\n\
			`regnID` varchar(9), \n\
			`Region` varchar(50), \n");
		
		sSQLIndex = _T("PRIMARY KEY (`regnID`), INDEX regnIndex (`regnID`, `Region`) \n");

		break;
		
	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}

	sSQLBody = _T("`") + sE + _T("_TotHH` int, \n\
			`") + sE + _T("_TotMem` int, \n\
			`") + sE + _T("_TotMem_Male` int, \n\
			`") + sE + _T("_TotMem_Female` int, \n\
			`") + sE + _T("_FoTotMem` int, \n\
			`") + sE + _T("_FoTotMem_Male` int, \n\
			`") + sE + _T("_FoTotMem_Female` int, \n\
			`") + sE + _T("_FowTotMem` int, \n\
			`") + sE + _T("_FowTotMem_Male` int, \n\
			`") + sE + _T("_FowTotMem_Female` int, \n\
			`") + sE + _T("_HHwMem05` int, \n\
			`") + sE + _T("_HHwMem05_Prop` float(4,1), \n\
			`") + sE + _T("_HHwMem04` int, \n\
			`") + sE + _T("_HHwMem04_Prop` float(4,1), \n\
			`") + sE + _T("_HHwMem04d` int, \n\
			`") + sE + _T("_HHwMem04d_Prop` float(4,1), \n\
			`") + sE + _T("_HHwMem05d` int, \n\
			`") + sE + _T("_HHwMem05d_Prop` float(4,1), \n\
			`") + sE + _T("_Mem04` int, \n\
			`") + sE + _T("_Mem04_Male` int, \n\
			`") + sE + _T("_Mem04_Female` int, \n\
			`") + sE + _T("_Mem05` int, \n\
			`") + sE + _T("_Mem05_Male` int, \n\
			`") + sE + _T("_Mem05_Female` int, \n\
			`") + sE + _T("_HHwMaln05` int, \n\
			`") + sE + _T("_HHwMaln05_Prop` float(4,1), \n\
			`") + sE + _T("_Maln05` int, \n\
			`") + sE + _T("_Maln05_Male` int, \n\
			`") + sE + _T("_Maln05_Female` int, \n\
			`") + sE + _T("_Maln05_Prop` float(4,1), \n\
			`") + sE + _T("_Maln05_Male_Prop` float(4,1), \n\
			`") + sE + _T("_Maln05_Female_Prop` float(4,1), \n\
			`") + sE + _T("_maln04` int, \n\
			`") + sE + _T("_maln04_Male` int, \n\
			`") + sE + _T("_maln04_Female` int, \n\
			`") + sE + _T("_maln04_Prop` float(4,1), \n\
			`") + sE + _T("_maln04_Male_Prop` float(4,1), \n\
			`") + sE + _T("_maln04_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwDeath04` int, \n\
			`") + sE + _T("_HHwDeath04_Prop` float(4,1), \n\
			`") + sE + _T("_HHwDeath05` int, \n\
			`") + sE + _T("_HHwDeath05_Prop` float(4,1), \n\
			`") + sE + _T("_Death04` int, \n\
			`") + sE + _T("_Death04_Male` int, \n\
			`") + sE + _T("_Death04_Female` int, \n\
			`") + sE + _T("_Death04_Prop` float(4,1), \n\
			`") + sE + _T("_Death04_Male_Prop` float(4,1), \n\
			`") + sE + _T("_Death04_Female_Prop` float(4,1), \n\
			`") + sE + _T("_Death05` int, \n\
			`") + sE + _T("_Death05_Male` int, \n\
			`") + sE + _T("_Death05_Female` int, \n\
			`") + sE + _T("_Death05_Prop` float(4,1), \n\
			`") + sE + _T("_Death05_Male_Prop` float(4,1), \n\
			`") + sE + _T("_Death05_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwMem01` int, \n\
			`") + sE + _T("_HHwMem01_Prop` float(4,1), \n\
			`") + sE + _T("_HHwMem01d` int, \n\
			`") + sE + _T("_HHwMem01d_Prop` float(4,1), \n\
			`") + sE + _T("_Mem01` int, \n\
			`") + sE + _T("_Mem01_Male` int, \n\
			`") + sE + _T("_Mem01_Female` int, \n\
			`") + sE + _T("_HHwDeathPreg` int, \n\
			`") + sE + _T("_HHwDeathPreg_Prop` float(4,1), \n\
			`") + sE + _T("_DeathPreg` int, \n\
			`") + sE + _T("_DeathPreg_Male` int, \n\
			`") + sE + _T("_DeathPreg_Female` int, \n\
			`") + sE + _T("_DeathPreg_Prop` float(4,1), \n\
			`") + sE + _T("_DeathPreg_Male_Prop` float(4,1), \n\
			`") + sE + _T("_DeathPreg_Female_Prop` float(4,1), \n\
			`") + sE + _T("_MSH` int, \n\
			`") + sE + _T("_MSH_Prop` float(4,1), \n\
			`") + sE + _T("_MemMSH` int, \n\
			`") + sE + _T("_MemMSH_Male` int, \n\
			`") + sE + _T("_MemMSH_Female` int, \n\
			`") + sE + _T("_MemMSH_Prop` float(4,1), \n\
			`") + sE + _T("_MemMSH_Male_Prop` float(4,1), \n\
			`") + sE + _T("_MemMSH_Female_Prop` float(4,1), \n\
			`") + sE + _T("_Squat` int, \n\
			`") + sE + _T("_Squat_Prop` float(4,1), \n\
			`") + sE + _T("_MemSquat` int, \n\
			`") + sE + _T("_MemSquat_Male` int, \n\
			`") + sE + _T("_MemSquat_Female` int, \n\
			`") + sE + _T("_MemSquat_Prop` float(4,1), \n\
			`") + sE + _T("_MemSquat_Male_Prop` float(4,1), \n\
			`") + sE + _T("_MemSquat_Female_Prop` float(4,1), \n\
			`") + sE + _T("_ntSWS` int, \n\
			`") + sE + _T("_ntSWS_Prop` float(4,1), \n\
			`") + sE + _T("_MemntSWS` int, \n\
			`") + sE + _T("_MemntSWS_Male` int, \n\
			`") + sE + _T("_MemntSWS_Female` int, \n\
			`") + sE + _T("_MemntSWS_Prop` float(4,1), \n\
			`") + sE + _T("_MemntSWS_Male_Prop` float(4,1), \n\
			`") + sE + _T("_MemntSWS_Female_Prop` float(4,1), \n\
			`") + sE + _T("_ntSTF` int, \n\
			`") + sE + _T("_ntSTF_Prop` float(4,1), \n\
			`") + sE + _T("_MemntSTF` int, \n\
			`") + sE + _T("_MemntSTF_Male` int, \n\
			`") + sE + _T("_MemntSTF_Female` int, \n\
			`") + sE + _T("_MemntSTF_Prop` float(4,1), \n\
			`") + sE + _T("_MemntSTF_Male_Prop` float(4,1), \n\
			`") + sE + _T("_MemntSTF_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwMem611` int, \n\
			`") + sE + _T("_HHwMem611_Prop` float(4,1), \n\
			`") + sE + _T("_Mem611` int, \n\
			`") + sE + _T("_Mem611_Male` int, \n\
			`") + sE + _T("_Mem611_Female` int, \n\
			`") + sE + _T("_HHwntElem611` int, \n\
			`") + sE + _T("_HHwntElem611_Prop` float(4,1), \n\
			`") + sE + _T("_ntElem611` int, \n\
			`") + sE + _T("_ntElem611_Male` int, \n\
			`") + sE + _T("_ntElem611_Female` int, \n\
			`") + sE + _T("_ntElem611_Prop` float(4,1), \n\
			`") + sE + _T("_ntElem611_Male_Prop` float(4,1), \n\
			`") + sE + _T("_ntElem611_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwMem1215` int, \n\
			`") + sE + _T("_HHwMem1215_Prop` float(4,1), \n\
			`") + sE + _T("_Mem1215` int, \n\
			`") + sE + _T("_Mem1215_Male` int, \n\
			`") + sE + _T("_Mem1215_Female` int, \n\
			`") + sE + _T("_HHwntHS1215` int, \n\
			`") + sE + _T("_HHwntHS1215_Prop` float(4,1), \n\
			`") + sE + _T("_ntHS1215` int, \n\
			`") + sE + _T("_ntHS1215_Male` int, \n\
			`") + sE + _T("_ntHS1215_Female` int, \n\
			`") + sE + _T("_ntHS1215_Prop` float(4,1), \n\
			`") + sE + _T("_ntHS1215_Male_Prop` float(4,1), \n\
			`") + sE + _T("_ntHS1215_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwMem612` int, \n\
			`") + sE + _T("_HHwMem612_Prop` float(4,1), \n\
			`") + sE + _T("_Mem612` int, \n\
			`") + sE + _T("_Mem612_Male` int, \n\
			`") + sE + _T("_Mem612_Female` int, \n\
			`") + sE + _T("_HHwntElem612` int, \n\
			`") + sE + _T("_HHwntElem612_Prop` float(4,1), \n\
			`") + sE + _T("_ntElem612` int, \n\
			`") + sE + _T("_ntElem612_Male` int, \n\
			`") + sE + _T("_ntElem612_Female` int, \n\
			`") + sE + _T("_ntElem612_Prop` float(4,1), \n\
			`") + sE + _T("_ntElem612_Male_Prop` float(4,1), \n\
			`") + sE + _T("_ntElem612_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwMem1316` int, \n\
			`") + sE + _T("_HHwMem1316_Prop` float(4,1), \n\
			`") + sE + _T("_Mem1316` int, \n\
			`") + sE + _T("_Mem1316_Male` int, \n\
			`") + sE + _T("_Mem1316_Female` int, \n\
			`") + sE + _T("_HHwntHS1316` int, \n\
			`") + sE + _T("_HHwntHS1316_Prop` float(4,1), \n\
			`") + sE + _T("_ntHS1316` int, \n\
			`") + sE + _T("_ntHS1316_Male` int, \n\
			`") + sE + _T("_ntHS1316_Female` int, \n\
			`") + sE + _T("_ntHS1316_Prop` float(4,1), \n\
			`") + sE + _T("_ntHS1316_Male_Prop` float(4,1), \n\
			`") + sE + _T("_ntHS1316_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwMem615` int, \n\
			`") + sE + _T("_HHwMem615_Prop` float(4,1), \n\
			`") + sE + _T("_Mem615` int, \n\
			`") + sE + _T("_Mem615_Male` int, \n\
			`") + sE + _T("_Mem615_Female` int, \n\
			`") + sE + _T("_HHwntSch615` int, \n\
			`") + sE + _T("_HHwntSch615_Prop` float(4,1), \n\
			`") + sE + _T("_ntSch615` int, \n\
			`") + sE + _T("_ntSch615_Male` int, \n\
			`") + sE + _T("_ntSch615_Female` int, \n\
			`") + sE + _T("_ntSch615_Prop` float(4,1), \n\
			`") + sE + _T("_ntSch615_Male_Prop` float(4,1), \n\
			`") + sE + _T("_ntSch615_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwMem616` int, \n\
			`") + sE + _T("_HHwMem616_Prop` float(4,1), \n\
			`") + sE + _T("_Mem616` int, \n\
			`") + sE + _T("_Mem616_Male` int, \n\
			`") + sE + _T("_Mem616_Female` int, \n\
			`") + sE + _T("_HHwntSch616` int, \n\
			`") + sE + _T("_HHwntSch616_Prop` float(4,1), \n\
			`") + sE + _T("_ntSch616` int, \n\
			`") + sE + _T("_ntSch616_Male` int, \n\
			`") + sE + _T("_ntSch616_Female` int, \n\
			`") + sE + _T("_ntSch616_Prop` float(4,1), \n\
			`") + sE + _T("_ntSch616_Male_Prop` float(4,1), \n\
			`") + sE + _T("_ntSch616_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwMem10ab` int, \n\
			`") + sE + _T("_HHwMem10ab_Prop` float(4,1), \n\
			`") + sE + _T("_Mem10ab` int, \n\
			`") + sE + _T("_Mem10ab_Male` int, \n\
			`") + sE + _T("_Mem10ab_Female` int, \n\
			`") + sE + _T("_HHwntLiter10ab` int, \n\
			`") + sE + _T("_HHwntLiter10ab_Prop` float(4,1), \n\
			`") + sE + _T("_ntLiter10ab` int, \n\
			`") + sE + _T("_ntLiter10ab_Male` int, \n\
			`") + sE + _T("_ntLiter10ab_Female` int, \n\
			`") + sE + _T("_ntLiter10ab_Prop` float(4,1), \n\
			`") + sE + _T("_ntLiter10ab_Male_Prop` float(4,1), \n\
			`") + sE + _T("_ntLiter10ab_Female_Prop` float(4,1), \n\
			`") + sE + _T("_Povp` int, `") + sE + _T("_Povp_Prop` float(4,1), \n\
			`") + sE + _T("_MemPovp` int, \n\
			`") + sE + _T("_MemPovp_Male` int, \n\
			`") + sE + _T("_MemPovp_Female` int, \n\
			`") + sE + _T("_MemPovp_Prop` float(4,1), \n\
			`") + sE + _T("_MemPovp_Male_Prop` float(4,1), \n\
			`") + sE + _T("_MemPovp_Female_Prop` float(4,1), \n\
			`") + sE + _T("_Subp` int, `") + sE + _T("_Subp_Prop` float(4,1), \n\
			`") + sE + _T("_MemSubp` int, \n\
			`") + sE + _T("_MemSubp_Male` int, \n\
			`") + sE + _T("_MemSubp_Female` int, \n\
			`") + sE + _T("_MemSubp_Prop` float(4,1), \n\
			`") + sE + _T("_MemSubp_Male_Prop` float(4,1), \n\
			`") + sE + _T("_MemSubp_Female_Prop` float(4,1), \n\
			`") + sE + _T("_Fshort` int, `") + sE + _T("_Fshort_Prop` float(4,1), \n\
			`") + sE + _T("_MemFshort` int, \n\
			`") + sE + _T("_MemFshort_Male` int, \n\
			`") + sE + _T("_MemFshort_Female` int, \n\
			`") + sE + _T("_MemFshort_Prop` float(4,1), \n\
			`") + sE + _T("_MemFshort_Male_Prop` float(4,1), \n\
			`") + sE + _T("_MemFshort_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwLabfor` int, \n\
			`") + sE + _T("_HHwLabfor_Prop` float(4,1), \n\
			`") + sE + _T("_Labfor` int, \n\
			`") + sE + _T("_Labfor_Male` int, \n\
			`") + sE + _T("_Labfor_Female` int, \n\
			`") + sE + _T("_HHwUnempl15ab` int, \n\
			`") + sE + _T("_HHwUnempl15ab_Prop` float(4,1), \n\
			`") + sE + _T("_Unempl15ab` int, \n\
			`") + sE + _T("_Unempl15ab_Male` int, \n\
			`") + sE + _T("_Unempl15ab_Female` int, \n\
			`") + sE + _T("_Unempl15ab_Prop` float(4,1), \n\
			`") + sE + _T("_Unempl15ab_Male_Prop` float(4,1), \n\
			`") + sE + _T("_Unempl15ab_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwVictCr` int, \n\
			`") + sE + _T("_HHwVictCr_Prop` float(4,1), \n\
			`") + sE + _T("_VictCr` int, \n\
			`") + sE + _T("_VictCr_Male` int, \n\
			`") + sE + _T("_VictCr_Female` int, \n\
			`") + sE + _T("_VictCr_Prop` float(4,1), \n\
			`") + sE + _T("_VictCr_Male_Prop` float(4,1), \n\
			`") + sE + _T("_VictCr_Female_Prop` float(4,1), \n");


	if (currElt!=HH)
		return sSQL = sSQLInit + sSQLBody + sSQLIndex + _T(");");
	else
		return sSQL = sSQLInit + sSQLIndex + _T(");");

}
CString CStatSimElt::sSQLMDG()
{

	CString sSQL, sSQLInit = _T(""), sSQLBody = _T(""), sSQLIndex = _T(""), sE;

	sE = Attr(element);
	

	switch(currElt) {

	case MEM:
				
		sSQL = _T("");		

		break;

	case HH:

		//Males and females
		sSQLInit = _T("");	
		break;

	case PUROK:

		sSQLInit = _T("CREATE TABLE IF NOT EXISTS `Purok_mdg` (\n\
			`purokID` varchar(11), \n\
			`Region` varchar(50), \n\
			`Province` varchar(50), \n\
			`Municipality` varchar(50), \n\
			`Barangay` varchar(50), \n\
			`PurokName` varchar(50), \n");

		sSQLIndex = _T("PRIMARY KEY (`purokID`), INDEX purokIndex (`purokID`, `Region`, `Province`, `Municipality`, `Barangay`, `PurokName`) \n");

		break;

	case BRGY:
		
		sSQLInit = _T("CREATE TABLE IF NOT EXISTS `Brgy_mdg` (\n\
			`brgyID` varchar(9), \n\
			`Region` varchar(50), \n\
			`Province` varchar(50), \n\
			`Municipality` varchar(50), \n\
			`Barangay` varchar(50), \n");
		
		sSQLIndex = _T("PRIMARY KEY (`brgyID`), INDEX brgyIndex (`brgyID`, `Region`, `Province`, `Municipality`, `Barangay`) \n");

		break;

	case MUN:
		
		sSQLInit = _T("CREATE TABLE IF NOT EXISTS `Mun_mdg` (\n\
			`munID` varchar(9), \n\
			`Region` varchar(50), \n\
			`Province` varchar(50), \n\
			`Municipality` varchar(50), \n");
		
		sSQLIndex = _T("PRIMARY KEY (`munID`), INDEX munIndex (`munID`, `Region`, `Province`, `Municipality`) \n");

		break;

	case PROV:
		
		sSQLInit = _T("CREATE TABLE IF NOT EXISTS `Prov_mdg` (\n\
			`provID` varchar(9), \n\
			`Region` varchar(50), \n\
			`Province` varchar(50), \n");
		
		sSQLIndex = _T("PRIMARY KEY (`provID`), INDEX provIndex (`provID`, `Region`, `Province`) \n");

		break;

	case REGN:
		
		sSQLInit = _T("CREATE TABLE IF NOT EXISTS `Regn_mdg` (\n\
			`regnID` varchar(9), \n\
			`Region` varchar(50), \n");
		
		sSQLIndex = _T("PRIMARY KEY (`regnID`), INDEX regnIndex (`regnID`, `Region`) \n");

		break;
		
	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}

/*	sSQLBody = _T("`") + sE + _T("_TotHH` int, \n\
			`") + sE + _T("_TotMem` int, \n\
			`") + sE + _T("_TotMem_Male` int, \n\
			`") + sE + _T("_TotMem_Female` int, \n\
			`") + sE + _T("_Povp` int, `") + sE + _T("_Povp_Prop` float(4,2), \n\
			`") + sE + _T("_MemPovp` int, \n\
			`") + sE + _T("_MemPovp_Male` int, \n\
			`") + sE + _T("_MemPovp_Female` int, \n\
			`") + sE + _T("_MemPovp_Prop` float(4,2), \n\
			`") + sE + _T("_MemPovp_Male_Prop` float(4,2), \n\
			`") + sE + _T("_MemPovp_Female_Prop` float(4,2), \n\
			`") + sE + _T("_Subp` int, `") + sE + _T("_Subp_Prop` float(4,2), \n\
			`") + sE + _T("_MemSubp` int, \n\
			`") + sE + _T("_MemSubp_Male` int, \n\
			`") + sE + _T("_MemSubp_Female` int, \n\
			`") + sE + _T("_MemSubp_Prop` float(4,2), \n\
			`") + sE + _T("_MemSubp_Male_Prop` float(4,2), \n\
			`") + sE + _T("_MemSubp_Female_Prop` float(4,2), \n\
			`") + sE + _T("_empl15ab` int, \n\
			`") + sE + _T("_empl15ab_Male` int, \n\
			`") + sE + _T("_empl15ab_Female` int, \n\
			`") + sE + _T("_empl15ab_Prop` float(4,2), \n\
			`") + sE + _T("_empl15ab_Male_Prop` float(4,2), \n\
			`") + sE + _T("_empl15ab_Female_Prop` float(4,2), \n\
			`") + sE + _T("_PovGap` float(6,2), \n\
			`") + sE + _T("_PovGap_Prop` float(6,2), \n\
			`") + sE + _T("_MemPovGap` float(6,2), \n\
			`") + sE + _T("_MemPovGap_Male` float(6,2), \n\
			`") + sE + _T("_MemPovGap_Female` float(6,2), \n\
			`") + sE + _T("_MemPovGap_prop` float(6,2), \n\
			`") + sE + _T("_MemPovGap_Male_prop` float(6,2), \n\
			`") + sE + _T("_MemPovGap_Female_prop` float(6,2), \n\
			`") + sE + _T("_Fshort` int, `") + sE + _T("_Fshort_Prop` float(4,2), \n\
			`") + sE + _T("_MemFshort` int, \n\
			`") + sE + _T("_MemFshort_Male` int, \n\
			`") + sE + _T("_MemFshort_Female` int, \n\
			`") + sE + _T("_MemFshort_Prop` float(4,2), \n\
			`") + sE + _T("_MemFshort_Male_Prop` float(4,2), \n\
			`") + sE + _T("_MemFshort_Female_Prop` float(4,2), \n\
			`") + sE + _T("_HHwMem05` int, \n\
			`") + sE + _T("_HHwMem05_Prop` float(4,1), \n\
			`") + sE + _T("_HHwMem04d` int, \n\
			`") + sE + _T("_HHwMem04d_Prop` float(4,1), \n\
			`") + sE + _T("_Mem01` int, \n\
			`") + sE + _T("_Mem01_Male` int, \n\
			`") + sE + _T("_Mem01_Female` int, \n\
			`") + sE + _T("_Mem05` int, \n\
			`") + sE + _T("_Mem05_Male` int, \n\
			`") + sE + _T("_Mem05_Female` int, \n\
			`") + sE + _T("_HHwMaln05` int, \n\
			`") + sE + _T("_HHwMaln05_Prop` float(4,1), \n\
			`") + sE + _T("_Maln05` int, \n\
			`") + sE + _T("_Maln05_Male` int, \n\
			`") + sE + _T("_Maln05_Female` int, \n\
			`") + sE + _T("_Maln05_Prop` float(4,1), \n\
			`") + sE + _T("_Maln05_Male_Prop` float(4,1), \n\
			`") + sE + _T("_Maln05_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwMem611` int, \n\
			`") + sE + _T("_HHwMem611_Prop` float(4,1), \n\
			`") + sE + _T("_Mem611` int, \n\
			`") + sE + _T("_Mem611_Male` int, \n\
			`") + sE + _T("_Mem611_Female` int, \n\
			`") + sE + _T("_HHwelem611` int, \n\
			`") + sE + _T("_HHwelem611_Prop` float(4,1), \n\
			`") + sE + _T("_Elem611` int, \n\
			`") + sE + _T("_elem611_Male` int, \n\
			`") + sE + _T("_elem611_Female` int, \n\
			`") + sE + _T("_elem611_Prop` float(4,1), \n\
			`") + sE + _T("_elem611_Male_Prop` float(4,1), \n\
			`") + sE + _T("_elem611_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwMem1215` int, \n\
			`") + sE + _T("_HHwMem1215_Prop` float(4,1), \n\
			`") + sE + _T("_Mem1215` int, \n\
			`") + sE + _T("_Mem1215_Male` int, \n\
			`") + sE + _T("_Mem1215_Female` int, \n\
			`") + sE + _T("_HHwHS1215` int, \n\
			`") + sE + _T("_HHwHS1215_Prop` float(4,1), \n\
			`") + sE + _T("_HS1215` int, \n\
			`") + sE + _T("_HS1215_Male` int, \n\
			`") + sE + _T("_HS1215_Female` int, \n\
			`") + sE + _T("_HS1215_Prop` float(4,1), \n\
			`") + sE + _T("_HS1215_Male_Prop` float(4,1), \n\
			`") + sE + _T("_HS1215_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwMem615` int, \n\
			`") + sE + _T("_HHwMem615_Prop` float(4,1), \n\
			`") + sE + _T("_Mem615` int, \n\
			`") + sE + _T("_Mem615_Male` int, \n\
			`") + sE + _T("_Mem615_Female` int, \n\
			`") + sE + _T("_HHwSch615` int, \n\
			`") + sE + _T("_HHwSch615_Prop` float(4,1), \n\
			`") + sE + _T("_Sch615` int, \n\
			`") + sE + _T("_Sch615_Male` int, \n\
			`") + sE + _T("_Sch615_Female` int, \n\
			`") + sE + _T("_Sch615_Prop` float(4,1), \n\
			`") + sE + _T("_Sch615_Male_Prop` float(4,1), \n\
			`") + sE + _T("_Sch615_Female_Prop` float(4,1), \n\
			`") + sE + _T("_elem611_mfratio` float(6,2), \n\
			`") + sE + _T("_hs1215_mfratio` float(6,2), \n\
			`") + sE + _T("_sch615_mfratio` float(6,2), \n\
			`") + sE + _T("_liter1524_mfratio` float(6,2), \n\
			`") + sE + _T("_HHwMem1721` int, \n\
			`") + sE + _T("_HHwMem1721_Prop` float(4,1), \n\
			`") + sE + _T("_Mem1721` int, \n\
			`") + sE + _T("_Mem1721_Male` int, \n\
			`") + sE + _T("_Mem1721_Female` int, \n\
			`") + sE + _T("_HHwTert1721` int, \n\
			`") + sE + _T("_HHwTert1721_Prop` float(4,1), \n\
			`") + sE + _T("_Tert1721` int, \n\
			`") + sE + _T("_Tert1721_Male` int, \n\
			`") + sE + _T("_Tert1721_Female` int, \n\
			`") + sE + _T("_Tert1721_Prop` float(4,1), \n\
			`") + sE + _T("_Tert1721_Male_Prop` float(4,1), \n\
			`") + sE + _T("_Tert1721_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwMem10ab` int, \n\
			`") + sE + _T("_HHwMem10ab_Prop` float(4,1), \n\
			`") + sE + _T("_Mem10ab` int, \n\
			`") + sE + _T("_Mem10ab_Male` int, \n\
			`") + sE + _T("_Mem10ab_Female` int, \n\
			`") + sE + _T("_HHwLiter10ab` int, \n\
			`") + sE + _T("_HHwLiter10ab_Prop` float(4,1), \n\
			`") + sE + _T("_Liter10ab` int, \n\
			`") + sE + _T("_Liter10ab_Male` int, \n\
			`") + sE + _T("_Liter10ab_Female` int, \n\
			`") + sE + _T("_Liter10ab_Prop` float(4,1), \n\
			`") + sE + _T("_Liter10ab_Male_Prop` float(4,1), \n\
			`") + sE + _T("_Liter10ab_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwMem1524` int, \n\
			`") + sE + _T("_HHwMem1524_Prop` float(4,1), \n\
			`") + sE + _T("_Mem1524` int, \n\
			`") + sE + _T("_Mem1524_Male` int, \n\
			`") + sE + _T("_Mem1524_Female` int, \n\
			`") + sE + _T("_HHwLiter1524` int, \n\
			`") + sE + _T("_HHwLiter1524_Prop` float(4,1), \n\
			`") + sE + _T("_Liter1524` int, \n\
			`") + sE + _T("_Liter1524_Male` int, \n\
			`") + sE + _T("_Liter1524_Female` int, \n\
			`") + sE + _T("_Liter1524_Prop` float(4,1), \n\
			`") + sE + _T("_Liter1524_Male_Prop` float(4,1), \n\
			`") + sE + _T("_Liter1524_Female_Prop` float(4,1), \n\
			`") + sE + _T("_ntagri` int, \n\
			`") + sE + _T("_ntagri_Male` int, \n\
			`") + sE + _T("_ntagri_Female` int, \n\
			`") + sE + _T("_ntagri_Prop` float(4,1), \n\
			`") + sE + _T("_ntagri_Male_Prop` float(4,1), \n\
			`") + sE + _T("_ntagri_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwDeath01` int, \n\
			`") + sE + _T("_HHwDeath01_Prop` float(4,1), \n\
			`") + sE + _T("_death01` int, \n\
			`") + sE + _T("_death01_Male` int, \n\
			`") + sE + _T("_death01_Female` int, \n\
			`") + sE + _T("_death01_Prop` float(4,1), \n\
			`") + sE + _T("_death01_Male_Prop` float(4,1), \n\
			`") + sE + _T("_death01_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwdeath04` int, \n\
			`") + sE + _T("_HHwdeath04_Prop` float(4,1), \n\
			`") + sE + _T("_death04` int, \n\
			`") + sE + _T("_death04_Male` int, \n\
			`") + sE + _T("_death04_Female` int, \n\
			`") + sE + _T("_death04_Prop` float(4,1), \n\
			`") + sE + _T("_death04_Male_Prop` float(4,1), \n\
			`") + sE + _T("_death04_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwdeath14` int, \n\
			`") + sE + _T("_HHwdeath14_Prop` float(4,1), \n\
			`") + sE + _T("_death14` int, \n\
			`") + sE + _T("_death14_Male` int, \n\
			`") + sE + _T("_death14_Female` int, \n\
			`") + sE + _T("_death14_Prop` float(4,1), \n\
			`") + sE + _T("_death14_Male_Prop` float(4,1), \n\
			`") + sE + _T("_death14_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwDeathPreg` int, \n\
			`") + sE + _T("_HHwDeathPreg_Prop` float(4,1), \n\
			`") + sE + _T("_deathpreg` int, \n\
			`") + sE + _T("_deathpreg_Male` int, \n\
			`") + sE + _T("_deathpreg_Female` int, \n\
			`") + sE + _T("_deathpreg_Prop` float(4,1), \n\
			`") + sE + _T("_deathpreg_Male_Prop` float(4,1), \n\
			`") + sE + _T("_deathpreg_Female_Prop` float(4,1), \n\
			`") + sE + _T("_hhwcouple` int, \n\
			`") + sE + _T("_hhwusebc` int, \n\
			`") + sE + _T("_hhwusebc_Prop` float(4,1), \n\
			`") + sE + _T("_couple` int, \n\
			`") + sE + _T("_usebc` int, \n\
			`") + sE + _T("_usebc_Prop` float(4,1), \n\
			`") + sE + _T("_hhwusecondom` int, \n\
			`") + sE + _T("_hhwusecondom_Prop` float(4,1), \n\
			`") + sE + _T("_usecondom` int, \n\
			`") + sE + _T("_usecondom_Prop` float(4,1), \n\
			`") + sE + _T("_HHwDeath` int, \n\
			`") + sE + _T("_death` int, \n\
			`") + sE + _T("_death_Male` int, \n\
			`") + sE + _T("_death_Female` int, \n\
			`") + sE + _T("_HHwDeath_tb` int, \n\
			`") + sE + _T("_hhwdeath_tb_Prop` float(4,1), \n\
			`") + sE + _T("_death_tb` int, \n\
			`") + sE + _T("_death_tb_Male` int, \n\
			`") + sE + _T("_death_tb_Female` int, \n\
			`") + sE + _T("_death_tb_Prop` float(4,1), \n\
			`") + sE + _T("_death_tb_Male_Prop` float(4,1), \n\
			`") + sE + _T("_death_tb_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwDeath_malaria` int, \n\
			`") + sE + _T("_hhwdeath_malaria_Prop` float(4,1), \n\
			`") + sE + _T("_death_malaria` int, \n\
			`") + sE + _T("_death_malaria_Male` int, \n\
			`") + sE + _T("_death_malaria_Female` int, \n\
			`") + sE + _T("_death_malaria_Prop` float(4,1), \n\
			`") + sE + _T("_death_malaria_Male_Prop` float(4,1), \n\
			`") + sE + _T("_death_malaria_Female_Prop` float(4,1), \n\
			`") + sE + _T("_sWS` int, \n\
			`") + sE + _T("_sWS_Prop` float(4,1), \n\
			`") + sE + _T("_MemsWS` int, \n\
			`") + sE + _T("_MemsWS_Male` int, \n\
			`") + sE + _T("_MemsWS_Female` int, \n\
			`") + sE + _T("_MemsWS_Prop` float(4,1), \n\
			`") + sE + _T("_MemsWS_Male_Prop` float(4,1), \n\
			`") + sE + _T("_MemsWS_Female_Prop` float(4,1), \n\
			`") + sE + _T("_sTF` int, \n\
			`") + sE + _T("_sTF_Prop` float(4,1), \n\
			`") + sE + _T("_MemsTF` int, \n\
			`") + sE + _T("_MemsTF_Male` int, \n\
			`") + sE + _T("_MemsTF_Female` int, \n\
			`") + sE + _T("_MemsTF_Prop` float(4,1), \n\
			`") + sE + _T("_MemsTF_Male_Prop` float(4,1), \n\
			`") + sE + _T("_MemsTF_Female_Prop` float(4,1), \n\
			`") + sE + _T("_squat` int, \n\
			`") + sE + _T("_squat_Prop` float(4,1), \n\
			`") + sE + _T("_Memsquat` int, \n\
			`") + sE + _T("_Memsquat_Male` int, \n\
			`") + sE + _T("_Memsquat_Female` int, \n\
			`") + sE + _T("_Memsquat_Prop` float(4,1), \n\
			`") + sE + _T("_Memsquat_Male_Prop` float(4,1), \n\
			`") + sE + _T("_Memsquat_Female_Prop` float(4,1), \n\
			`") + sE + _T("_msh` int, \n\
			`") + sE + _T("_msh_Prop` float(4,1), \n\
			`") + sE + _T("_Memmsh` int, \n\
			`") + sE + _T("_Memmsh_Male` int, \n\
			`") + sE + _T("_Memmsh_Female` int, \n\
			`") + sE + _T("_Memmsh_Prop` float(4,1), \n\
			`") + sE + _T("_Memmsh_Male_Prop` float(4,1), \n\
			`") + sE + _T("_Memmsh_Female_Prop` float(4,1), \n\
			`") + sE + _T("_inadeqlc` int, \n\
			`") + sE + _T("_inadeqlc_Prop` float(4,1), \n\
			`") + sE + _T("_Meminadeqlc` int, \n\
			`") + sE + _T("_Meminadeqlc_Male` int, \n\
			`") + sE + _T("_Meminadeqlc_Female` int, \n\
			`") + sE + _T("_Meminadeqlc_Prop` float(4,1), \n\
			`") + sE + _T("_Meminadeqlc_Male_Prop` float(4,1), \n\
			`") + sE + _T("_Meminadeqlc_Female_Prop` float(4,1), \n\
			`") + sE + _T("_labfor1524` int, \n\
			`") + sE + _T("_labfor1524_Male` int, \n\
			`") + sE + _T("_labfor1524_Female` int, \n\
			`") + sE + _T("_unempl1524` int, \n\
			`") + sE + _T("_unempl1524_Male` int, \n\
			`") + sE + _T("_unempl1524_Female` int, \n\
			`") + sE + _T("_unempl1524_Prop` float(4,1), \n\
			`") + sE + _T("_unempl1524_Male_Prop` float(4,1), \n\
			`") + sE + _T("_unempl1524_Female_Prop` float(4,1), \n\
			`") + sE + _T("_wphone` int, \n\
			`") + sE + _T("_wphone_Prop` float(4,1), \n\
			`") + sE + _T("_memwphone` int, \n\
			`") + sE + _T("_memwphone_Male` int, \n\
			`") + sE + _T("_memwphone_Female` int, \n\
			`") + sE + _T("_memwphone_Prop` float(4,1), \n\
			`") + sE + _T("_memwphone_Male_Prop` float(4,1), \n\
			`") + sE + _T("_memwphone_Female_Prop` float(4,1), \n\
			`") + sE + _T("_wmphone` int, \n\
			`") + sE + _T("_wmphone_Prop` float(4,1), \n\
			`") + sE + _T("_memwmphone` int, \n\
			`") + sE + _T("_memwmphone_Male` int, \n\
			`") + sE + _T("_memwmphone_Female` int, \n\
			`") + sE + _T("_memwmphone_Prop` float(4,1), \n\
			`") + sE + _T("_memwmphone_Male_Prop` float(4,1), \n\
			`") + sE + _T("_memwmphone_Female_Prop` float(4,1), \n\
			`") + sE + _T("_wcomputer` int, \n\
			`") + sE + _T("_wcomputer_Prop` float(4,1), \n\
			`") + sE + _T("_memwcomputer` int, \n\
			`") + sE + _T("_memwcomputer_Male` int, \n\
			`") + sE + _T("_memwcomputer_Female` int, \n\
			`") + sE + _T("_memwcomputer_Prop` float(4,1), \n\
			`") + sE + _T("_memwcomputer_Male_Prop` float(4,1), \n\
			`") + sE + _T("_memwcomputer_Female_Prop` float(4,1), \n");
*/

	sSQLBody = _T("`") + sE + _T("_TotHH` int, \n\
			`") + sE + _T("_TotMem` int, \n\
			`") + sE + _T("_TotMem_Male` int, \n\
			`") + sE + _T("_TotMem_Female` int, \n\
			`") + sE + _T("_Povp` int, `") + sE + _T("_Povp_Prop` float(4,2), \n\
			`") + sE + _T("_MemPovp` int, \n\
			`") + sE + _T("_MemPovp_Male` int, \n\
			`") + sE + _T("_MemPovp_Female` int, \n\
			`") + sE + _T("_MemPovp_Prop` float(4,2), \n\
			`") + sE + _T("_MemPovp_Male_Prop` float(4,2), \n\
			`") + sE + _T("_MemPovp_Female_Prop` float(4,2), \n\
			`") + sE + _T("_Subp` int, `") + sE + _T("_Subp_Prop` float(4,2), \n\
			`") + sE + _T("_MemSubp` int, \n\
			`") + sE + _T("_MemSubp_Male` int, \n\
			`") + sE + _T("_MemSubp_Female` int, \n\
			`") + sE + _T("_MemSubp_Prop` float(4,2), \n\
			`") + sE + _T("_MemSubp_Male_Prop` float(4,2), \n\
			`") + sE + _T("_MemSubp_Female_Prop` float(4,2), \n\
			`") + sE + _T("_empl15ab` int, \n\
			`") + sE + _T("_empl15ab_Male` int, \n\
			`") + sE + _T("_empl15ab_Female` int, \n\
			`") + sE + _T("_empl15ab_Prop` float(4,2), \n\
			`") + sE + _T("_empl15ab_Male_Prop` float(4,2), \n\
			`") + sE + _T("_empl15ab_Female_Prop` float(4,2), \n\
			`") + sE + _T("_PovGap` float(6,2), \n\
			`") + sE + _T("_PovGap_Prop` float(6,2), \n\
			`") + sE + _T("_MemPovGap` float(6,2), \n\
			`") + sE + _T("_MemPovGap_Male` float(6,2), \n\
			`") + sE + _T("_MemPovGap_Female` float(6,2), \n\
			`") + sE + _T("_MemPovGap_prop` float(6,2), \n\
			`") + sE + _T("_MemPovGap_Male_prop` float(6,2), \n\
			`") + sE + _T("_MemPovGap_Female_prop` float(6,2), \n\
			`") + sE + _T("_Fshort` int, `") + sE + _T("_Fshort_Prop` float(4,2), \n\
			`") + sE + _T("_MemFshort` int, \n\
			`") + sE + _T("_MemFshort_Male` int, \n\
			`") + sE + _T("_MemFshort_Female` int, \n\
			`") + sE + _T("_MemFshort_Prop` float(4,2), \n\
			`") + sE + _T("_MemFshort_Male_Prop` float(4,2), \n\
			`") + sE + _T("_MemFshort_Female_Prop` float(4,2), \n\
			`") + sE + _T("_HHwMem05` int, \n\
			`") + sE + _T("_HHwMem05_Prop` float(4,1), \n\
			`") + sE + _T("_HHwMem04d` int, \n\
			`") + sE + _T("_HHwMem04d_Prop` float(4,1), \n\
			`") + sE + _T("_Mem01` int, \n\
			`") + sE + _T("_Mem01_Male` int, \n\
			`") + sE + _T("_Mem01_Female` int, \n\
			`") + sE + _T("_Mem05` int, \n\
			`") + sE + _T("_Mem05_Male` int, \n\
			`") + sE + _T("_Mem05_Female` int, \n\
			`") + sE + _T("_HHwMaln05` int, \n\
			`") + sE + _T("_HHwMaln05_Prop` float(4,1), \n\
			`") + sE + _T("_Maln05` int, \n\
			`") + sE + _T("_Maln05_Male` int, \n\
			`") + sE + _T("_Maln05_Female` int, \n\
			`") + sE + _T("_Maln05_Prop` float(4,1), \n\
			`") + sE + _T("_Maln05_Male_Prop` float(4,1), \n\
			`") + sE + _T("_Maln05_Female_Prop` float(4,1), \n\
			`") + sE + _T("_maln04` int, \n\
			`") + sE + _T("_maln04_Male` int, \n\
			`") + sE + _T("_maln04_Female` int, \n\
			`") + sE + _T("_maln04_Prop` float(4,1), \n\
			`") + sE + _T("_maln04_Male_Prop` float(4,1), \n\
			`") + sE + _T("_maln04_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwMem611` int, \n\
			`") + sE + _T("_HHwMem611_Prop` float(4,1), \n\
			`") + sE + _T("_Mem611` int, \n\
			`") + sE + _T("_Mem611_Male` int, \n\
			`") + sE + _T("_Mem611_Female` int, \n\
			`") + sE + _T("_HHwelem611` int, \n\
			`") + sE + _T("_HHwelem611_Prop` float(4,1), \n\
			`") + sE + _T("_Elem611` int, \n\
			`") + sE + _T("_elem611_Male` int, \n\
			`") + sE + _T("_elem611_Female` int, \n\
			`") + sE + _T("_elem611_Prop` float(4,1), \n\
			`") + sE + _T("_elem611_Male_Prop` float(4,1), \n\
			`") + sE + _T("_elem611_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwMem1215` int, \n\
			`") + sE + _T("_HHwMem1215_Prop` float(4,1), \n\
			`") + sE + _T("_Mem1215` int, \n\
			`") + sE + _T("_Mem1215_Male` int, \n\
			`") + sE + _T("_Mem1215_Female` int, \n\
			`") + sE + _T("_HHwHS1215` int, \n\
			`") + sE + _T("_HHwHS1215_Prop` float(4,1), \n\
			`") + sE + _T("_HS1215` int, \n\
			`") + sE + _T("_HS1215_Male` int, \n\
			`") + sE + _T("_HS1215_Female` int, \n\
			`") + sE + _T("_HS1215_Prop` float(4,1), \n\
			`") + sE + _T("_HS1215_Male_Prop` float(4,1), \n\
			`") + sE + _T("_HS1215_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwMem615` int, \n\
			`") + sE + _T("_HHwMem615_Prop` float(4,1), \n\
			`") + sE + _T("_Mem615` int, \n\
			`") + sE + _T("_Mem615_Male` int, \n\
			`") + sE + _T("_Mem615_Female` int, \n\
			`") + sE + _T("_HHwSch615` int, \n\
			`") + sE + _T("_HHwSch615_Prop` float(4,1), \n\
			`") + sE + _T("_Sch615` int, \n\
			`") + sE + _T("_Sch615_Male` int, \n\
			`") + sE + _T("_Sch615_Female` int, \n\
			`") + sE + _T("_Sch615_Prop` float(4,1), \n\
			`") + sE + _T("_Sch615_Male_Prop` float(4,1), \n\
			`") + sE + _T("_Sch615_Female_Prop` float(4,1), \n\
			`") + sE + _T("_Mem1620` int, \n\
			`") + sE + _T("_Mem1620_Male` int, \n\
			`") + sE + _T("_Mem1620_Female` int, \n\
			`") + sE + _T("_tert1620` int, \n\
			`") + sE + _T("_tert1620_Male` int, \n\
			`") + sE + _T("_tert1620_Female` int, \n\
			`") + sE + _T("_tert1620_Prop` float(4,1), \n\
			`") + sE + _T("_tert1620_Male_Prop` float(4,1), \n\
			`") + sE + _T("_tert1620_Female_Prop` float(4,1), \n\
			`") + sE + _T("_elem611_mfratio` float(6,2), \n\
			`") + sE + _T("_hs1215_mfratio` float(6,2), \n\
			`") + sE + _T("_sch615_mfratio` float(6,2), \n\
			`") + sE + _T("_tert1620_mfratio` float(6,2), \n\
			`") + sE + _T("_liter1524_mfratio` float(6,2), \n\
			`") + sE + _T("_HHwMem1524` int, \n\
			`") + sE + _T("_HHwMem1524_Prop` float(4,1), \n\
			`") + sE + _T("_Mem1524` int, \n\
			`") + sE + _T("_Mem1524_Male` int, \n\
			`") + sE + _T("_Mem1524_Female` int, \n\
			`") + sE + _T("_HHwLiter1524` int, \n\
			`") + sE + _T("_HHwLiter1524_Prop` float(4,1), \n\
			`") + sE + _T("_Liter1524` int, \n\
			`") + sE + _T("_Liter1524_Male` int, \n\
			`") + sE + _T("_Liter1524_Female` int, \n\
			`") + sE + _T("_Liter1524_Prop` float(4,1), \n\
			`") + sE + _T("_Liter1524_Male_Prop` float(4,1), \n\
			`") + sE + _T("_Liter1524_Female_Prop` float(4,1), \n\
			`") + sE + _T("_death01` int, \n\
			`") + sE + _T("_death01_Male` int, \n\
			`") + sE + _T("_death01_Female` int, \n\
			`") + sE + _T("_death01_Prop` float(4,1), \n\
			`") + sE + _T("_death01_Male_Prop` float(4,1), \n\
			`") + sE + _T("_death01_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwdeath04` int, \n\
			`") + sE + _T("_HHwdeath04_Prop` float(4,1), \n\
			`") + sE + _T("_death04` int, \n\
			`") + sE + _T("_death04_Male` int, \n\
			`") + sE + _T("_death04_Female` int, \n\
			`") + sE + _T("_death04_Prop` float(4,1), \n\
			`") + sE + _T("_death04_Male_Prop` float(4,1), \n\
			`") + sE + _T("_death04_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwdeath14` int, \n\
			`") + sE + _T("_HHwdeath14_Prop` float(4,1), \n\
			`") + sE + _T("_death14` int, \n\
			`") + sE + _T("_death14_Male` int, \n\
			`") + sE + _T("_death14_Female` int, \n\
			`") + sE + _T("_death14_Prop` float(4,1), \n\
			`") + sE + _T("_death14_Male_Prop` float(4,1), \n\
			`") + sE + _T("_death14_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwDeathPreg` int, \n\
			`") + sE + _T("_HHwDeathPreg_Prop` float(4,1), \n\
			`") + sE + _T("_deathpreg` int, \n\
			`") + sE + _T("_deathpreg_Male` int, \n\
			`") + sE + _T("_deathpreg_Female` int, \n\
			`") + sE + _T("_deathpreg_Prop` float(4,1), \n\
			`") + sE + _T("_deathpreg_Male_Prop` float(4,1), \n\
			`") + sE + _T("_deathpreg_Female_Prop` float(4,1), \n\
			`") + sE + _T("_hhwcouple` int, \n\
			`") + sE + _T("_hhwusebc` int, \n\
			`") + sE + _T("_hhwusebc_Prop` float(4,1), \n\
			`") + sE + _T("_couple` int, \n\
			`") + sE + _T("_usebc` int, \n\
			`") + sE + _T("_usebc_Prop` float(4,1), \n\
			`") + sE + _T("_hhwusecondom` int, \n\
			`") + sE + _T("_hhwusecondom_Prop` float(4,1), \n\
			`") + sE + _T("_usecondom` int, \n\
			`") + sE + _T("_usecondom_Prop` float(4,1), \n\
			`") + sE + _T("_HHwDeath` int, \n\
			`") + sE + _T("_death` int, \n\
			`") + sE + _T("_death_Male` int, \n\
			`") + sE + _T("_death_Female` int, \n\
			`") + sE + _T("_HHwDeath_tb` int, \n\
			`") + sE + _T("_hhwdeath_tb_Prop` float(4,1), \n\
			`") + sE + _T("_death_tb` int, \n\
			`") + sE + _T("_death_tb_Male` int, \n\
			`") + sE + _T("_death_tb_Female` int, \n\
			`") + sE + _T("_death_tb_Prop` float(4,1), \n\
			`") + sE + _T("_death_tb_Male_Prop` float(4,1), \n\
			`") + sE + _T("_death_tb_Female_Prop` float(4,1), \n\
			`") + sE + _T("_HHwDeath_malaria` int, \n\
			`") + sE + _T("_hhwdeath_malaria_Prop` float(4,1), \n\
			`") + sE + _T("_death_malaria` int, \n\
			`") + sE + _T("_death_malaria_Male` int, \n\
			`") + sE + _T("_death_malaria_Female` int, \n\
			`") + sE + _T("_death_malaria_Prop` float(4,1), \n\
			`") + sE + _T("_death_malaria_Male_Prop` float(4,1), \n\
			`") + sE + _T("_death_malaria_Female_Prop` float(4,1), \n\
			`") + sE + _T("_sWS` int, \n\
			`") + sE + _T("_sWS_Prop` float(4,1), \n\
			`") + sE + _T("_MemsWS` int, \n\
			`") + sE + _T("_MemsWS_Male` int, \n\
			`") + sE + _T("_MemsWS_Female` int, \n\
			`") + sE + _T("_MemsWS_Prop` float(4,1), \n\
			`") + sE + _T("_MemsWS_Male_Prop` float(4,1), \n\
			`") + sE + _T("_MemsWS_Female_Prop` float(4,1), \n\
			`") + sE + _T("_sTF` int, \n\
			`") + sE + _T("_sTF_Prop` float(4,1), \n\
			`") + sE + _T("_MemsTF` int, \n\
			`") + sE + _T("_MemsTF_Male` int, \n\
			`") + sE + _T("_MemsTF_Female` int, \n\
			`") + sE + _T("_MemsTF_Prop` float(4,1), \n\
			`") + sE + _T("_MemsTF_Male_Prop` float(4,1), \n\
			`") + sE + _T("_MemsTF_Female_Prop` float(4,1), \n\
			`") + sE + _T("_squat` int, \n\
			`") + sE + _T("_squat_Prop` float(4,1), \n\
			`") + sE + _T("_Memsquat` int, \n\
			`") + sE + _T("_Memsquat_Male` int, \n\
			`") + sE + _T("_Memsquat_Female` int, \n\
			`") + sE + _T("_Memsquat_Prop` float(4,1), \n\
			`") + sE + _T("_Memsquat_Male_Prop` float(4,1), \n\
			`") + sE + _T("_Memsquat_Female_Prop` float(4,1), \n\
			`") + sE + _T("_msh` int, \n\
			`") + sE + _T("_msh_Prop` float(4,1), \n\
			`") + sE + _T("_Memmsh` int, \n\
			`") + sE + _T("_Memmsh_Male` int, \n\
			`") + sE + _T("_Memmsh_Female` int, \n\
			`") + sE + _T("_Memmsh_Prop` float(4,1), \n\
			`") + sE + _T("_Memmsh_Male_Prop` float(4,1), \n\
			`") + sE + _T("_Memmsh_Female_Prop` float(4,1), \n\
			`") + sE + _T("_inadeqlc` int, \n\
			`") + sE + _T("_inadeqlc_Prop` float(4,1), \n\
			`") + sE + _T("_Meminadeqlc` int, \n\
			`") + sE + _T("_Meminadeqlc_Male` int, \n\
			`") + sE + _T("_Meminadeqlc_Female` int, \n\
			`") + sE + _T("_Meminadeqlc_Prop` float(4,1), \n\
			`") + sE + _T("_Meminadeqlc_Male_Prop` float(4,1), \n\
			`") + sE + _T("_Meminadeqlc_Female_Prop` float(4,1), \n\
			`") + sE + _T("_wphone` int, \n\
			`") + sE + _T("_wphone_Prop` float(4,1), \n\
			`") + sE + _T("_wmphone` int, \n\
			`") + sE + _T("_wmphone_Prop` float(4,1), \n\
			`") + sE + _T("_wphoneu` int, \n\
			`") + sE + _T("_wphoneu_Prop` float(4,1), \n\
			`") + sE + _T("_wcomputer` int, \n\
			`") + sE + _T("_wcomputer_Prop` float(4,1), \n\
			`") + sE + _T("_winet` int, \n\
			`") + sE + _T("_winet_Prop` float(4,1), \n\
			");

/*			`") + sE + _T("_HHwMem10ab` int, \n\
			`") + sE + _T("_HHwMem10ab_Prop` float(4,1), \n\
			`") + sE + _T("_Mem10ab` int, \n\
			`") + sE + _T("_Mem10ab_Male` int, \n\
			`") + sE + _T("_Mem10ab_Female` int, \n\
			`") + sE + _T("_HHwLiter10ab` int, \n\
			`") + sE + _T("_HHwLiter10ab_Prop` float(4,1), \n\
			`") + sE + _T("_Liter10ab` int, \n\
			`") + sE + _T("_Liter10ab_Male` int, \n\
			`") + sE + _T("_Liter10ab_Female` int, \n\
			`") + sE + _T("_Liter10ab_Prop` float(4,1), \n\
			`") + sE + _T("_Liter10ab_Male_Prop` float(4,1), \n\
			`") + sE + _T("_Liter10ab_Female_Prop` float(4,1), \n\
*/
	if (currElt!=HH)
		return sSQL = sSQLInit + sSQLBody + sSQLIndex + _T(");");
	else
		return sSQL = sSQLInit + sSQLIndex + _T(");");

}

CString CStatSimElt::sSQLHHInd(ELEMENT eT, LPCSTR sI, LPCSTR sD, LPCSTR sSI, LPCSTR sSD, 
							   LPCSTR sIDField, LPCSTR sAF, int units,
							   bool wHHSuffix, LPCSTR sIO, LPCSTR sDO)
{
	CString sSQL, sInd, sDenom,
		sSrcInd, sSrcDenom, sSQLProp,
		sE, sEIDFld, sEName, sEDigit, sIDF,
		sAggrFcn, sUnits, 
		sHHSuffix, sIndOut, sDenomOut;

	sInd.Format(_T("%s"), (CString) sI);
	sDenom.Format(_T("%s"), (CString) sD);

	sSrcInd.Format(_T("%s"), (CString) sSI);
	sSrcDenom.Format(_T("%s"), (CString) sSD);
	sIDF.Format(_T("%s"), (CString) sIDField);
	sAggrFcn.Format(_T("%s"), (CString) sAF);
	sUnits.Format(_T("%d"), units);

	sE = Attr(element);
	sEIDFld = Attr(IDField);
	sEName = Attr(name);
	sEDigit = Attr(digit);

	if (sIO!=NULL) {
		sIndOut.Format(_T("%s"), (CString) sIO);
	}
	else {
		sIndOut = sInd;
	}
	if (sDO!=NULL) {
		sDenomOut.Format(_T("%s"), (CString) sDO);
	}
	else {
		sDenomOut = sInd;
	}

	if (wHHSuffix) {
		sHHSuffix = _T("HH_");
	}
	else {
		sHHSuffix = _T("");
	}

	switch(currElt) {

	case MEM:
				
		sSQL = _T("");		

		break;

	case HH:
		
		sSQL = _T("");		

		break;

	case PUROK:

		sSQL = _T("CREATE TABLE IF NOT EXISTS `Purok_") + sIndOut + _T("` \n\
			SELECT  `purok`.`purokID` AS `purokID`, `regn`.`Region` AS `Region`, `prov`.`Province` AS `Province`, `mun`.`Municipality` AS `Municipality`, `brgy`.`Barangay` AS `Barangay`, `purok`.`PurokName` AS `PurokName`, \n\
			") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sHHSuffix + sInd + _T("`) AS `Purok_") + sIndOut + _T("`, `purok_") + sSrcDenom + _T("`.`purok_") + sDenom + _T("` AS `Purok_") + sDenom + _T("`, (") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sHHSuffix + sInd + _T("`)/`purok_") + sSrcDenom + _T("`.`purok_") + sDenom + _T("`)*") + sUnits +_T(" AS `Purok_") + sIndOut + _T("_Prop` \n")
			+ sGeoJoin(sSD, eT, sSI) + 
			_T("GROUP BY `purok`.`purokID`;");

		break;

	case BRGY:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `Brgy_") + sIndOut + _T("` \n\
			SELECT  `brgy`.`brgyID` AS `brgyID`, `regn`.`Region` AS `Region`, `prov`.`Province` AS `Province`, `mun`.`Municipality` AS `Municipality`, `brgy`.`Barangay` AS `Barangay`, \n\
			") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sHHSuffix + sInd + _T("`) AS `Brgy_") + sIndOut + _T("`, `brgy_") + sSrcDenom + _T("`.`brgy_") + sDenom + _T("` AS `Brgy_") + sDenom + _T("`, (") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sHHSuffix + sInd + _T("`)/`brgy_") + sSrcDenom + _T("`.`brgy_") + sDenom + _T("`)*") + sUnits +_T(" AS `Brgy_") + sIndOut + _T("_Prop` \n") 
			+ sGeoJoin(sSD, eT, sSI) + 
			_T("GROUP BY `brgy`.`brgyID`\n;");

		break;

	case MUN:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `Mun_") + sIndOut + _T("` \n\
			SELECT  `mun`.`munID` AS `munID`, `regn`.`Region` AS `Region`, `prov`.`Province` AS `Province`, `mun`.`Municipality` AS `Municipality`, \n\
			") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sHHSuffix + sInd + _T("`) AS `Mun_") + sIndOut + _T("`, `mun_") + sSrcDenom + _T("`.`mun_") + sDenom + _T("` AS `Mun_") + sDenom + _T("`, (") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sHHSuffix + sInd + _T("`)/`mun_") + sSrcDenom + _T("`.`mun_") + sDenom + _T("`)*") + sUnits +_T(" AS `Mun_") + sIndOut + _T("_Prop` \n") 
			+ sGeoJoin(sSD, eT, sSI) + 
			_T("GROUP BY `mun`.`munID`\n;");

		break;

	case PROV:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `Prov_") + sIndOut + _T("` \n\
			SELECT  `prov`.`provID` AS `provID`, `regn`.`Region` AS `Region`, `prov`.`Province` AS `Province`, \n\
			") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sHHSuffix + sInd + _T("`) AS `Prov_") + sIndOut + _T("`, `prov_") + sSrcDenom + _T("`.`prov_") + sDenom + _T("` AS `Prov_") + sDenom + _T("`, (") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sHHSuffix + sInd + _T("`)/`prov_") + sSrcDenom + _T("`.`prov_") + sDenom + _T("`)*") + sUnits +_T(" AS `Prov_") + sIndOut + _T("_Prop` \n") 
			+ sGeoJoin(sSD, eT, sSI) + 
			_T("GROUP BY `prov`.`provID` \n;");

		break;

	case REGN:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `Regn_") + sIndOut + _T("` \n\
			SELECT  `regn`.`regnID` AS `regnID`, `regn`.`Region` AS `Region`, \n\
			") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sHHSuffix + sInd + _T("`) AS `Regn_") + sIndOut + _T("`, `regn_") + sSrcDenom + _T("`.`regn_") + sDenom + _T("` AS `Regn_") + sDenom + _T("`, (") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sHHSuffix + sInd + _T("`)/`regn_") + sSrcDenom + _T("`.`regn_") + sDenom + _T("`)*") + sUnits +_T(" AS `Regn_") + sIndOut + _T("_Prop` \n") 
			+ sGeoJoin(sSD, eT, sSI) + 
			_T("GROUP BY `regn`.`regnID` \n;");

		break;
		
	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}
	return sSQL;
}

CString CStatSimElt::sSQLMemInd(ELEMENT eT, LPCSTR sI, LPCSTR sD, LPCSTR sME, LPCSTR sFE,
								LPCSTR sSI, LPCSTR sSD, BOOL death,
								LPCSTR sIDField, LPCSTR sAF, int units,
								LPCSTR sIO, LPCSTR sDO, LPCSTR sAD)
{
	//When sAD is not null, this will replace the augmentation of death by sAD instead of the numerator
	CString sSQL, sInd, sDenom, sMExt, sFExt,
		sSrcInd, sSrcDenom, sSQLProp,
		sE, sEIDFld, sEName, sEDigit, sIDF,
		sFromClause,
		sAggrFcn, sUnits,
		sIndOut, sDenomOut, sAugDenom;

	sInd.Format(_T("%s"), (CString) sI);
	sDenom.Format(_T("%s"), (CString) sD);

	sSrcInd.Format(_T("%s"), (CString) sSI);
	sSrcDenom.Format(_T("%s"), (CString) sSD);
	sIDF.Format(_T("%s"), (CString) sIDField);
	sAggrFcn.Format(_T("%s"), (CString) sAF);
	sUnits.Format(_T("%d"), units);

	sMExt.Format(_T("%s"), (CString) sME);
	sFExt.Format(_T("%s"), (CString) sFE);

	sE = Attr(element);
	sEIDFld = Attr(IDField);
	sEName = Attr(name);
	sEDigit = Attr(digit);

	sSrcDenom.MakeLower();


	if (sIO!=NULL) {
		sIndOut.Format(_T("%s"), (CString) sIO);
	}
	else {
		sIndOut = sInd;
	}
	if (sDO!=NULL) {
		sDenomOut.Format(_T("%s"), (CString) sDO);
	}
	else {
		sDenomOut = sInd;
	}
	if (sAD!=NULL) {
		sAugDenom.Format(_T("%s"), (CString) sAD);
	}
	else {
		sAugDenom = sInd;
	}

	if (sSrcDenom=="totpop")
		sFromClause = _T("FROM `") + sE + _T("_totpop` INNER JOIN `") + sSrcInd + _T("` ON (LEFT(`") + sE + _T("_totpop`.`") + sE + _T("ID`, 11) = LEFT(`") + sSrcInd + _T("`.`") + sIDF + _T("`, 11)) \n");
	else
		sFromClause = _T("FROM `") + sE + _T("_totpop` INNER JOIN `") + sE + _T("_") + sSrcDenom + _T("` ON `") + sE + _T("_totpop`.`") + sE + _T("ID` = `") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("ID` INNER JOIN `") + sSrcInd + _T("` ON (LEFT(`") + sE + _T("_totpop`.`") + sE + _T("ID`, 11) = LEFT(`") + sSrcInd + _T("`.`") + sIDF + _T("`, 11)) \n");

	if (!death) {
		sSQLProp = _T("(") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + _T("`)/`") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + _T("`)*") + sUnits +_T(" AS `") + sE + _T("_") + sIndOut + _T("_Prop`, (") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + sFExt + _T("`)/`") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + sFExt + _T("`)*") + sUnits +_T(" AS `") + sE + _T("_") + sIndOut + sFExt + _T("_Prop`, (") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + sMExt + _T("`)/`") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + sMExt + _T("`)*") + sUnits +_T(" AS `") + sE + _T("_") + sIndOut + sMExt + _T("_Prop` ");
	}
	else {
		if (!sAD)
			sSQLProp = _T("(") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + _T("`)/(`") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + _T("`+ ") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + _T("`)))*") + sUnits +_T(" AS `") + sE + _T("_") + sIndOut + _T("_Prop`, (") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + sFExt + _T("`)/(`") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + sFExt + _T("`+ ") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + sFExt + _T("`)))*") + sUnits +_T(" AS `") + sE + _T("_") + sIndOut + sFExt + _T("_Prop`, (") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + sMExt + _T("`)/(`") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + sMExt + _T("`+ ") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + sMExt + _T("`)))*") + sUnits +_T(" AS `") + sE + _T("_") + sIndOut + sMExt + _T("_Prop` ");
		else
			sSQLProp = _T("(") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + _T("`)/(`") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + _T("`+ ") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sAugDenom + _T("`)))*") + sUnits +_T(" AS `") + sE + _T("_") + sIndOut + _T("_Prop`, (") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + sFExt + _T("`)/(`") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + sFExt + _T("`+ ") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sAugDenom + sFExt + _T("`)))*") + sUnits +_T(" AS `") + sE + _T("_") + sIndOut + sFExt + _T("_Prop`, (") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + sMExt + _T("`)/(`") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + sMExt + _T("`+ ") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sAugDenom + sMExt + _T("`)))*") + sUnits +_T(" AS `") + sE + _T("_") + sIndOut + sMExt + _T("_Prop` ");
	}

	switch(currElt) {

	case MEM:
				
		sSQL = _T("");		

		break;

	case HH:
		
		sSQL = _T("");		

		break;

	case PUROK:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `Purok_") + sIndOut + _T("` \n\
			SELECT  `purok`.`purokID` AS `purokID`, `regn`.`Region` AS `Region`, `prov`.`Province` AS `Province`, `mun`.`Municipality` AS `Municipality`, `brgy`.`Barangay` AS `Barangay`, `purok`.`PurokName` AS `PurokName`, \n\
			") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + _T("`) AS `Purok_") + sIndOut + _T("`, ") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + sMExt + _T("`) AS `Purok_") + sIndOut + sMExt + _T("`, ") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + sFExt + _T("`) AS `Purok_") + sIndOut + sFExt + _T("`, `purok_") + sSrcDenom + _T("`.`purok_") + sDenom + _T("` AS `Purok_") + sDenom + _T("`, `purok_") + sSrcDenom + _T("`.`purok_") + sDenom + sMExt + _T("` AS `Purok_") + sDenom + sMExt + _T("`, `purok_") + sSrcDenom + _T("`.`purok_") + sDenom + sFExt + _T("` AS `Purok_") + sDenom + sFExt + _T("`, ") + sSQLProp + _T(" \n") 
			+ sGeoJoin(sSD, eT, sSI) + 
			_T("GROUP BY `purok`.`purokID`;");

		break;

	case BRGY:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `Brgy_") + sIndOut + _T("` \n\
			SELECT  `brgy`.`brgyID` AS `brgyID`, `regn`.`Region` AS `Region`, `prov`.`Province` AS `Province`, `mun`.`Municipality` AS `Municipality`, `brgy`.`Barangay` AS `Barangay`, \n\
			") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + _T("`) AS `Brgy_") + sIndOut + _T("`, ") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + sMExt + _T("`) AS `Brgy_") + sIndOut + sMExt + _T("`, ") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + sFExt + _T("`) AS `Brgy_") + sIndOut + sFExt + _T("`, `brgy_") + sSrcDenom + _T("`.`brgy_") + sDenom + _T("` AS `Brgy_") + sDenom + _T("`, `brgy_") + sSrcDenom + _T("`.`brgy_") + sDenom + sMExt + _T("` AS `Brgy_") + sDenom + sMExt + _T("`, `brgy_") + sSrcDenom + _T("`.`brgy_") + sDenom + sFExt + _T("` AS `Brgy_") + sDenom + sFExt + _T("`, ") + sSQLProp + _T(" \n") 
			+ sGeoJoin(sSD, eT, sSI) +
			_T("GROUP BY `brgy`.`brgyID` \n;");

		break;

	case MUN:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `Mun_") + sIndOut + _T("` \n\
			SELECT  `mun`.`munID` AS `munID`, `regn`.`Region` AS `Region`, `prov`.`Province` AS `Province`, `mun`.`Municipality` AS `Municipality`, \n\
			") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + _T("`) AS `Mun_") + sIndOut + _T("`, ") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + sMExt + _T("`) AS `Mun_") + sIndOut + sMExt + _T("`, ") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + sFExt + _T("`) AS `Mun_") + sIndOut + sFExt + _T("`, `mun_") + sSrcDenom + _T("`.`mun_") + sDenom + _T("` AS `Mun_") + sDenom + _T("`, `mun_") + sSrcDenom + _T("`.`mun_") + sDenom + sMExt + _T("` AS `Mun_") + sDenom + sMExt + _T("`, `mun_") + sSrcDenom + _T("`.`mun_") + sDenom + sFExt + _T("` AS `Mun_") + sDenom + sFExt + _T("`, ") + sSQLProp + _T(" \n") 
			+ sGeoJoin(sSD, eT, sSI) + 
			_T("GROUP BY `mun`.`munID` \n;");

		break;

	case PROV:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `Prov_") + sIndOut + _T("` \n\
			SELECT  `prov`.`provID` AS `provID`, `regn`.`Region` AS `Region`, `prov`.`Province` AS `Province`, \n\
			") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + _T("`) AS `Prov_") + sIndOut + _T("`, ") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + sMExt + _T("`) AS `Prov_") + sIndOut + sMExt + _T("`, ") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + sFExt + _T("`) AS `Prov_") + sIndOut + sFExt + _T("`, `prov_") + sSrcDenom + _T("`.`prov_") + sDenom + _T("` AS `Prov_") + sDenom + _T("`, `prov_") + sSrcDenom + _T("`.`prov_") + sDenom + sMExt + _T("` AS `Prov_") + sDenom + sMExt + _T("`, `prov_") + sSrcDenom + _T("`.`prov_") + sDenom + sFExt + _T("` AS `Prov_") + sDenom + sFExt + _T("`, ") + sSQLProp + _T(" \n") 
			+ sGeoJoin(sSD, eT, sSI) + 
			_T("GROUP BY `prov`.`provID` \n;");

		break;

	case REGN:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `Regn_") + sIndOut + _T("` \n\
			SELECT  `regn`.`regnID` AS `regnID`, `regn`.`Region` AS `Region`, ") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + _T("`) AS `Regn_") + sIndOut + _T("`, ") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + sMExt + _T("`) AS `Regn_") + sIndOut + sMExt + _T("`, ") + sAggrFcn + ("(`") + sSrcInd + _T("`.`") + sInd + sFExt + _T("`) AS `Regn_") + sIndOut + sFExt + _T("`, `regn_") + sSrcDenom + _T("`.`regn_") + sDenom + _T("` AS `Regn_") + sDenom + _T("`, `regn_") + sSrcDenom + _T("`.`regn_") + sDenom + sMExt + _T("` AS `Regn_") + sDenom + sMExt + _T("`, `regn_") + sSrcDenom + _T("`.`regn_") + sDenom + sFExt + _T("` AS `Regn_") + sDenom + sFExt + _T("`, ") + sSQLProp + _T(" \n")
			+ sGeoJoin(sSD, eT, sSI) + 
			_T("GROUP BY `regn`.`regnID` \n;");

		break;
		
	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}
	return sSQL;


}

CString CStatSimElt::CrMemIndCat(ELEMENT srcType, LPCSTR sIndCat, LPCSTR sDenom, BOOL wLabel, 
							  LPCSTR sMExt, LPCSTR sFExt,
							  LPCSTR sSrcIndCat, LPCSTR sSrcDenom, 
							  LPCSTR IDF,
							  LPCSTR sFilter, LPCSTR sSexSrc, LPCSTR sSexVar, LPCSTR sMemLine, LPCSTR* pSexVarExt,
							  LPCSTR sAggrFcn)

{
	CString sSQL,  sE, sEName, sEDigit,
		sKey, sTable;
	
	sSQL.Format(_T("DROP TABLE IF EXISTS `%s_%s`;"), (CString) Attr(element), (CString) sIndCat );
	pEltDB->ExecuteSQL( sSQL );

	//add index on the variable to join the values
	sSQL.Format(_T("ALTER IGNORE TABLE `%s` ADD INDEX `%sIndex`(`%s`);"), (CString) sSrcIndCat, (CString) sIndCat, (CString) sIndCat);
	pEltDB->ExecuteSQL( sSQL, FALSE );
	
		
	sE = Attr(element);
	sKey.Format(_T("`%s`, `%s`"), (CString) Attr(IDField), (CString) sIndCat);
	sTable.Format(_T("%s_%s"), (CString) sE, (CString) sIndCat);
	sEName = Attr(name);
	sEDigit = Attr(digit);

	sSQL = sSQLMemIndCat(srcType, sIndCat, sDenom, sMExt, sFExt,
								sSrcIndCat, sSrcDenom,
								IDF, sFilter, sSexSrc, sSexVar, sMemLine, pSexVarExt, sAggrFcn);

	//FILE *pFile = _tfopen(_T("C:\\mysql.sql"), _T("w"));
	//_ftprintf( pFile, ConstChar(sSQL) );
	//fclose(pFile);

	pEltDB->ExecuteSQL( sSQL );
	
	AddPrimaryKey( ConstChar(sTable), ConstChar(sKey) );

	return sSQL;

}
CString CStatSimElt::sGeoJoin(LPCSTR sLS, LPCSTR sRS)
{

	CString sClause, sLSrc, sRSrc;
	sLSrc.Format(_T("%s"), (CString) sLS);
	sRSrc.Format(_T("%s"), (CString) sRS);

	switch(currElt) {

	case MEM:
				
		sClause = _T("");		

		break;

	case HH:
		
		sClause = _T("");		

		break;

	case PUROK:

		sClause = _T("(`") + sLSrc + _T("`.`regn` = `") + sRSrc + _T("`.`regn`) AND (`") + sLSrc + _T("`.`prov` = `") + sRSrc + _T("`.`prov`) AND (`") + sLSrc + _T("`.`mun` = `") + sRSrc + _T("`.`mun`) AND (`") + sLSrc + _T("`.`brgy` = `") + sRSrc + _T("`.`brgy`) AND (`") + sLSrc + _T("`.`purok` = `") + sRSrc + _T("`.`purok`)");

		break;

	case BRGY:
		
		sClause = _T("(`") + sLSrc + _T("`.`regn` = `") + sRSrc + _T("`.`regn`) AND (`") + sLSrc + _T("`.`prov` = `") + sRSrc + _T("`.`prov`) AND (`") + sLSrc + _T("`.`mun` = `") + sRSrc + _T("`.`mun`) AND (`") + sLSrc + _T("`.`brgy` = `") + sRSrc + _T("`.`brgy`)");

		break;

	case MUN:
		
		sClause = _T("(`") + sLSrc + _T("`.`regn` = `") + sRSrc + _T("`.`regn`) AND (`") + sLSrc + _T("`.`prov` = `") + sRSrc + _T("`.`prov`) AND (`") + sLSrc + _T("`.`mun` = `") + sRSrc + _T("`.`mun`)");

		break;

	case PROV:
		
		sClause = _T("(`") + sLSrc + _T("`.`regn` = `") + sRSrc + _T("`.`regn`) AND (`") + sLSrc + _T("`.`prov` = `") + sRSrc + _T("`.`prov`)");

		break;

	case REGN:
		
		sClause = _T("(`") + sLSrc + _T("`.`regn` = `") + sRSrc + _T("`.`regn`)");

		break;
		
	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}

	return sClause;

}

CString CStatSimElt::sGeoJoin(LPCSTR eBase, ELEMENT srcType, LPCSTR sSrc)
{

	CString sClause, sFromInit;

	if (srcType==MEM) {
		sFromInit = _T("FROM `") + (CString) sSrc + _T("`\nINNER JOIN `mem` USING (`memID`)\n");
	}
	else if (srcType==HH) {
		sFromInit = _T("FROM `") + (CString) sSrc + _T("`\nINNER JOIN `hh` USING (`hhID`)\n");
	}
	else if (srcType==HPQ_RAW) {
		sFromInit = _T("FROM `") + (CString) sSrc + _T("`\n");
	}
	else {
		AfxMessageBox(_T("Element type is not included!"));
	}


	switch(currElt) {

	case MEM:
				
		sClause = _T("");		

		break;

	case HH:
		
		sClause = _T("");		

		break;

	case PUROK:

		sClause = sFromInit + 
			_T("INNER JOIN `purok` USING (`regn`, `prov`, `mun`, `brgy`, `purok`)\n\
			INNER JOIN `purok_") + (CString) eBase + _T("` USING (`purokID`)\n\
			INNER JOIN `brgy` USING (`regn`, `prov`, `mun`, `brgy`)\n\
			INNER JOIN `mun` USING (`regn`, `prov`, `mun`)\n\
			INNER JOIN `prov` USING (`regn`, `prov`)\n\
			INNER JOIN `regn` USING (`regn`)\n\
			");
		break;

	case BRGY:
		
		sClause = sFromInit + 
			_T("INNER JOIN `brgy` USING (`regn`, `prov`, `mun`, `brgy`)\n\
			INNER JOIN `brgy_") + (CString) eBase + _T("` USING (`brgyID`)\n\
			INNER JOIN `mun` USING (`regn`, `prov`, `mun`)\n\
			INNER JOIN `prov` USING (`regn`, `prov`)\n\
			INNER JOIN `regn` USING (`regn`)\n\
			");

		break;

	case MUN:
		
		sClause = sFromInit + 
			_T("INNER JOIN `mun` USING (`regn`, `prov`, `mun`)\n\
			INNER JOIN `mun_") + (CString) eBase + _T("` USING (`munID`)\n\
			INNER JOIN `prov` USING (`regn`, `prov`)\n\
			INNER JOIN `regn` USING (`regn`)\n\
			");

		break;

	case PROV:
		
		sClause = sFromInit + 
			_T("INNER JOIN `prov` USING (`regn`, `prov`)\n\
			INNER JOIN `prov_") + (CString) eBase + _T("` USING (`provID`)\n\
			INNER JOIN `regn` USING (`regn`)\n\
			");

		break;

	case REGN:
		
		sClause = sFromInit + 
			_T("INNER JOIN `regn` USING (`regn`)\n\
			INNER JOIN `regn_") + (CString) eBase + _T("` USING (`regnID`)\n\
			");

		break;
		
	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}

	return sClause;

}

CString CStatSimElt::sGeo(LPCSTR sS)
{

	CString sClause, sSrc;
	sSrc.Format(_T("%s"), (CString) sS);

	switch(currElt) {

	case MEM:
				
		sClause = _T("");		

		break;

	case HH:
		
		sClause = _T("");		

		break;

	case PUROK:

		if(!sS)
			sClause = "`regn`, `prov`, `mun`, `brgy`, `purok`";
		else
			sClause = _T("");		

		break;

	case BRGY:
		
		if(!sS)
			sClause = "`regn`, `prov`, `mun`, `brgy`";
		else
			sClause = _T("`") + sSrc + _T("`.`Regn`, `") + sSrc + _T("`.`Prov`, `") + sSrc + _T("`.`Mun`, `") + sSrc + _T("`.`Brgy`");

		break;

	case MUN:
		
		if(!sS)
			sClause = "`regn`, `prov`, `mun`";
		else
			sClause = _T("`") + sSrc + _T("`.`Regn`, `") + sSrc + _T("`.`Prov`, `") + sSrc + _T("`.`Mun`");

		break;

	case PROV:
		
		if(!sS)
			sClause = "`regn`, `prov`";
		else
			sClause = _T("`") + sSrc + _T("`.`Regn`, `") + sSrc + _T("`.`Prov`");

		break;

	case REGN:
		
		if(!sS)
			sClause = "`regn`";
		else
			sClause = _T("`") + sSrc + _T("`.`Regn`");

		break;
		
	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}

	return sClause;

}

CString CStatSimElt::sGeoLabel(LPCSTR sS)
{
	return _T("");

}
CString CStatSimElt::sSQLGeo(LPCSTR sS)
{

	CString sClause, sSrc, sE;
	sSrc.Format(_T("%s"), (CString) sS);

	sE = Attr(element);

	switch(currElt) {

	case MEM:
				
		sClause = _T("");		

		break;

	case HH:
		
		sClause = _T("");		

		break;

	case PUROK:

		sClause = _T("");		

		break;

	case BRGY:
		
		sClause = _T("`") + sE + _T("_") + sSrc + _T("`.`Region` AS `Region`, `") + sE + _T("_") + sSrc + _T("`.`Province` AS `Province`, `") + sE + _T("_") + sSrc + _T("`.`Municipality` AS `Municipality`, `") + sE + _T("_") + sSrc + _T("`.`Barangay` AS `Barangay`");

		break;

	case MUN:
		
		sClause = _T("`") + sE + _T("_") + sSrc + _T("`.`Region` AS `Region`, `") + sE + _T("_") + sSrc + _T("`.`Province` AS `Province`, `") + sE + _T("_") + sSrc + _T("`.`Municipality` AS `Municipality`");

		break;

	case PROV:
		
		sClause = _T("`") + sE + _T("_") + sSrc + _T("`.`Region` AS `Region`, `") + sE + _T("_") + sSrc + _T("`.`Province` AS `Province`");

		break;

	case REGN:
		
		sClause = _T("`") + sE + _T("_") + sSrc + _T("`.`Region` AS `Region`");

		break;
		
	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}

	return sClause;


}
CString CStatSimElt::sSQLMemIndCat(ELEMENT eT, LPCSTR sI, LPCSTR sD, LPCSTR sME, LPCSTR sFE,
								LPCSTR sSI, LPCSTR sSD,
								LPCSTR sIDField, LPCSTR sFilter, LPCSTR sSS, LPCSTR sSV, LPCSTR sML, 
								LPCSTR* pSexVarExt, LPCSTR sAF)
{
	CString sSQL, sIndCat, sDenom, sMExt, sFExt,
		sSrcIndCat, sSrcDenom, sSQLProp,
		sE, sEIDFld, sEName, sEDigit, sIDF,
		sSrcEID, sWhereClause, sSexSrc, sSexVar, sMemLine, sFromClause, sAggrFcn;

	CStringArray sSexVarArray;
	sSexVarArray.SetSize(NSEXVARS);

	if (pSexVarExt!=NULL) {
		for (int i=0; i<NSEXVARS; i++) {
			sSexVarArray[i] = pSexVarExt[i];
		}
	}

	sMExt.Format(_T("%s"), (CString) sME);
	sFExt.Format(_T("%s"), (CString) sFE);

	sIndCat.Format(_T("%s"), (CString) sI);
	sDenom.Format(_T("%s"), (CString) sD);

	sSrcIndCat.Format(_T("%s"), (CString) sSI);
	sSrcDenom.Format(_T("%s"), (CString) sSD);
	sIDF.Format(_T("%s"), (CString) sIDField);

	sSrcEID.Format(_T("%d"), GetSrcID(sSI));
	sSexSrc.Format(_T("%s"), (CString) sSS);
	sSexVar.Format(_T("%s"), (CString) sSV);
	sMemLine.Format(_T("%s"), (CString) sML);
	sAggrFcn.Format(_T("%s"), (CString) sAF);

	sE = Attr(element);
	sEIDFld = Attr(IDField);
	sEName = Attr(name);
	sEDigit = Attr(digit);

	if (sFilter!=NULL) {
		sWhereClause.Format(_T("%s"), (CString) sFilter );
	}
	else {
		sWhereClause = _T("");
	}

	sSrcIndCat.MakeLower();
	sSexSrc.MakeLower();
	sSrcIndCat.TrimLeft();
	sSexSrc.TrimLeft();
	sSrcIndCat.TrimRight();
	sSexSrc.TrimRight();

	if (sSrcIndCat==sSexSrc) {
		sFromClause.Format(_T("`%s`"), (CString) sSI);
	}
	else {
		sFromClause.Format(_T("`%s` INNER JOIN `%s` ON (`%s`.`regn`=`%s`.`regn` AND `%s`.`prov`=`%s`.`prov` AND `%s`.`mun`=`%s`.`mun` AND `%s`.`brgy`=`%s`.`brgy` AND `%s`.`purok`=`%s`.`purok` AND `%s`.`hcn`=`%s`.`hcn` AND `%s`.`memno`=`%s`.`line`)"), 
			(CString) sSI, (CString) sSS, (CString) sSI, (CString) sSS, (CString) sSI, (CString) sSS, (CString) sSI, (CString) sSS, (CString) sSI, (CString) sSS, (CString) sSI, (CString) sSS, (CString) sSI, (CString) sSS, (CString) sSI, (CString) sSS, (CString) sML);
	}


	if (currElt==MEM) {
				
		sSQL = _T("");		

	}

	else if (currElt==HH) {
		
		sSQL = _T("");		

	}

	else if (currElt==PUROK) {
		
		sSQL = _T("");		

	}

	else if (currElt==BRGY || currElt==MUN || currElt==PROV || currElt==REGN) {
		if (pSexVarExt!=NULL) {
			
			if (sFilter!=NULL) {	
				sSQL = _T("CREATE TABLE `") + sE + _T("_") + sIndCat + _T("` \n\
					SELECT `") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, ") + sSQLGeo(sSD) + _T(", `") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + _T("` AS `") + sE + _T("_") + sDenom + _T("`, `") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + sMExt + _T("` AS `") + sE + _T("_") + sDenom + sMExt + _T("`, `") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + sFExt + _T("` AS `") + sE + _T("_") + sDenom + sFExt + _T("`,  \n\
					`valueset`.`value` AS `") + sIndCat + _T("`, `valueset`.`label` AS `") + sIndCat + _T("_Label`, IFNULL(`") + sE + _T("_") + sIndCat + _T("`, 0) AS `") + sE + _T("_") + sIndCat + _T("`, IFNULL(`") + sE + _T("_") + sIndCat + sMExt + _T("`, 0) AS `") + sE + _T("_") + sIndCat + sMExt + _T("`, IFNULL(`") + sE + _T("_") + sIndCat + sFExt + _T("`, 0) AS `") + sE + _T("_") + sIndCat + sFExt + _T("`, \n\
					(IFNULL(`") + sE + _T("_") + sIndCat + _T("`, 0)/`") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + _T("`)*100 AS `") + sE + _T("_") + sIndCat + _T("_Prop`, (IFNULL(`") + sE + _T("_") + sIndCat + sMExt + _T("`, 0)/`") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + sMExt + _T("`)*100 AS `") + sE + _T("_") + sIndCat + sMExt + _T("_Prop`, (IFNULL(`") + sE + _T("_") + sIndCat + sFExt + _T("`, 0)/`") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + sFExt + _T("`)*100 AS `") + sE + _T("_") + sIndCat + sFExt + _T("_Prop`  \n\
					FROM (((SELECT * FROM `~helementvalue` WHERE `~helementvalue`.`var`='") + sIndCat + _T("' AND `~helementvalue`.`elementID`=") + sSrcEID + _T(") AS `valueset`), `") + sE + _T("_") + sSrcDenom + _T("`) \n\
					LEFT JOIN \n\
					((SELECT LEFT(") + sIDF + _T(", ") + sEDigit + _T(") AS `") + sE + _T("ID`, `") + sSrcIndCat + _T("`.`") + sIndCat + _T("` AS `") + sIndCat + _T("`, ") + sAggrFcn + _T("(`") + sSrcIndCat + _T("`.`") + sSexVarArray[TOT] + _T("`) AS `") + sE + _T("_") + sIndCat + _T("`, ") + sAggrFcn + _T("(`") + sSrcIndCat + _T("`.`") + sSexVarArray[MALE] + _T("`) AS `") + sE + _T("_") + sIndCat + sMExt + _T("`, ") + sAggrFcn + _T("(`") + sSrcIndCat + _T("`.`") + sSexVarArray[FEMALE] + _T("`) AS `") + sE + _T("_") + sIndCat + sFExt + _T("` \n\
					FROM ") + sFromClause + _T(" \n\
					WHERE (") + sWhereClause + _T(") \n\
					GROUP BY `") + sE + _T("ID`, `") + sIndCat + _T("`) AS `") + sE + _T("_") + sIndCat + _T("_All`) \n\
					ON LEFT(`") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("ID`, ") + sEDigit + _T(") = LEFT(`") + sE + _T("_") + sIndCat + _T("_All`.`") + sE + _T("ID`, ") + sEDigit + _T(") AND `valueset`.`value` = `") + sE + _T("_") + sIndCat + _T("_All`.`") + sIndCat + _T("`; \n");
			}
			else {
				sSQL = _T("CREATE TABLE `") + sE + _T("_") + sIndCat + _T("` \n\
					SELECT `") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, ") + sSQLGeo(sSD) + _T(", `") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + _T("` AS `") + sE + _T("_") + sDenom + _T("`, `") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + sMExt + _T("` AS `") + sE + _T("_") + sDenom + sMExt + _T("`, `") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + sFExt + _T("` AS `") + sE + _T("_") + sDenom + sFExt + _T("`,  \n\
					`valueset`.`value` AS `") + sIndCat + _T("`, `valueset`.`label` AS `") + sIndCat + _T("_Label`, IFNULL(`") + sE + _T("_") + sIndCat + _T("`, 0) AS `") + sE + _T("_") + sIndCat + _T("`, IFNULL(`") + sE + _T("_") + sIndCat + sMExt + _T("`, 0) AS `") + sE + _T("_") + sIndCat + sMExt + _T("`, IFNULL(`") + sE + _T("_") + sIndCat + sFExt + _T("`, 0) AS `") + sE + _T("_") + sIndCat + sFExt + _T("`, \n\
					(IFNULL(`") + sE + _T("_") + sIndCat + _T("`, 0)/`") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + _T("`)*100 AS `") + sE + _T("_") + sIndCat + _T("_Prop`, (IFNULL(`") + sE + _T("_") + sIndCat + sMExt + _T("`, 0)/`") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + sMExt + _T("`)*100 AS `") + sE + _T("_") + sIndCat + sMExt + _T("_Prop`, (IFNULL(`") + sE + _T("_") + sIndCat + sFExt + _T("`, 0)/`") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + sFExt + _T("`)*100 AS `") + sE + _T("_") + sIndCat + sFExt + _T("_Prop`  \n\
					FROM (((SELECT * FROM `~helementvalue` WHERE `~helementvalue`.`var`='") + sIndCat + _T("' AND `~helementvalue`.`elementID`=") + sSrcEID + _T(") AS `valueset`), `") + sE + _T("_") + sSrcDenom + _T("`) \n\
					LEFT JOIN \n\
					((SELECT LEFT(") + sIDF + _T(", ") + sEDigit + _T(") AS `") + sE + _T("ID`, `") + sSrcIndCat + _T("`.`") + sIndCat + _T("` AS `") + sIndCat + _T("`, ") + sAggrFcn + _T("(`") + sSrcIndCat + _T("`.`") + sSexVarArray[TOT] + _T("`) AS `") + sE + _T("_") + sIndCat + _T("`, ") + sAggrFcn + _T("(`") + sSrcIndCat + _T("`.`") + sSexVarArray[MALE] + _T("`) AS `") + sE + _T("_") + sIndCat + sMExt + _T("`, ") + sAggrFcn + _T("(`") + sSrcIndCat + _T("`.`") + sSexVarArray[FEMALE] + _T("`) AS `") + sE + _T("_") + sIndCat + sFExt + _T("` \n\
					FROM ") + sFromClause + _T(" \n\
					GROUP BY `") + sE + _T("ID`, `") + sIndCat + _T("`) AS `") + sE + _T("_") + sIndCat + _T("_All`) \n\
					ON LEFT(`") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("ID`, ") + sEDigit + _T(") = LEFT(`") + sE + _T("_") + sIndCat + _T("_All`.`") + sE + _T("ID`, ") + sEDigit + _T(") AND `valueset`.`value` = `") + sE + _T("_") + sIndCat + _T("_All`.`") + sIndCat + _T("`; \n");
			}
		}
		else {
			if (sFilter!=NULL) {	
				sSQL = _T("CREATE TABLE `") + sE + _T("_") + sIndCat + _T("` \n\
					SELECT `") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, ") + sSQLGeo(sSD) + _T(", `") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + _T("` AS `") + sE + _T("_") + sDenom + _T("`, `") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + sMExt + _T("` AS `") + sE + _T("_") + sDenom + sMExt + _T("`, `") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + sFExt + _T("` AS `") + sE + _T("_") + sDenom + sFExt + _T("`,  \n\
					`valueset`.`value` AS `") + sIndCat + _T("`, `valueset`.`label` AS `") + sIndCat + _T("_Label`, IFNULL(`") + sE + _T("_") + sIndCat + _T("`, 0) AS `") + sE + _T("_") + sIndCat + _T("`, IFNULL(`") + sE + _T("_") + sIndCat + sMExt + _T("`, 0) AS `") + sE + _T("_") + sIndCat + sMExt + _T("`, IFNULL(`") + sE + _T("_") + sIndCat + sFExt + _T("`, 0) AS `") + sE + _T("_") + sIndCat + sFExt + _T("`, \n\
					(IFNULL(`") + sE + _T("_") + sIndCat + _T("`, 0)/`") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + _T("`)*100 AS `") + sE + _T("_") + sIndCat + _T("_Prop`, (IFNULL(`") + sE + _T("_") + sIndCat + sMExt + _T("`, 0)/`") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + sMExt + _T("`)*100 AS `") + sE + _T("_") + sIndCat + sMExt + _T("_Prop`, (IFNULL(`") + sE + _T("_") + sIndCat + sFExt + _T("`, 0)/`") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + sFExt + _T("`)*100 AS `") + sE + _T("_") + sIndCat + sFExt + _T("_Prop`  \n\
					FROM (((SELECT * FROM `~helementvalue` WHERE `~helementvalue`.`var`='") + sIndCat + _T("' AND `~helementvalue`.`elementID`=") + sSrcEID + _T(") AS `valueset`), `") + sE + _T("_") + sSrcDenom + _T("`) \n\
					LEFT JOIN \n\
					((SELECT `") + sE + _T("_") + sIndCat + _T("_Temp`.`") + sE + _T("ID` AS `") + sE + _T("ID`, `") + sE + _T("_") + sIndCat + _T("_Temp`.`") + sIndCat + _T("` AS `") + sIndCat + _T("`, IFNULL(`") + sE + _T("_") + sIndCat + _T("`, 0) AS `") + sE + _T("_") + sIndCat + _T("`, IFNULL(`") + sE + _T("_") + sIndCat + sMExt + _T("`, 0) AS `") + sE + _T("_") + sIndCat + sMExt + _T("`, IFNULL(`") + sE + _T("_") + sIndCat + sFExt + _T("`, 0) AS `") + sE + _T("_") + sIndCat + sFExt + _T("` FROM  \n\
					((SELECT LEFT(") + sIDF + _T(", ") + sEDigit + _T(") AS `") + sE + _T("ID`, `") + sSrcIndCat + _T("`.`") + sIndCat + _T("` AS `") + sIndCat + _T("`, ") + sAggrFcn + _T("(`") + sSrcIndCat + _T("`.`") + sIndCat + _T("`) AS `") + sE + _T("_") + sIndCat + _T("` \n\
					FROM ") + sFromClause + _T(" \n\
					WHERE (") + sWhereClause + _T(") \n\
					GROUP BY `") + sE + _T("ID`, `") + sIndCat + _T("`) AS `") + sE + _T("_") + sIndCat + _T("_Temp`) \n\
					LEFT JOIN \n\
					((SELECT LEFT(") + sIDF + _T(", ") + sEDigit + _T(") AS `") + sE + _T("ID`, `") + sSrcIndCat + _T("`.`") + sIndCat + _T("` AS `") + sIndCat + _T("`, ") + sAggrFcn + _T("(`") + sSrcIndCat + _T("`.`") + sIndCat + _T("`) AS `") + sE + _T("_") + sIndCat + sMExt + _T("` \n\
					FROM ") + sFromClause + _T(" \n\
					WHERE (`") + sSrcIndCat + _T("`.`") + sSexVar + _T("`=1 AND ") + sWhereClause + _T(") \n\
					GROUP BY `") + sE + _T("ID`, `") + sIndCat + _T("`) AS `") + sE + _T("_") + sIndCat + sMExt + _T("`) \n\
					USING (`") + sE + _T("ID`, `") + sIndCat + _T("`) \n\
					LEFT JOIN \n\
					((SELECT LEFT(") + sIDF + _T(", ") + sEDigit + _T(") AS `") + sE + _T("ID`, `") + sSrcIndCat + _T("`.`") + sIndCat + _T("` AS `") + sIndCat + _T("`, ") + sAggrFcn + _T("(`") + sSrcIndCat + _T("`.`") + sIndCat + _T("`) AS `") + sE + _T("_") + sIndCat + sFExt + _T("` \n\
					FROM ") + sFromClause + _T(" \n\
					WHERE (`") + sSrcIndCat + _T("`.`") + sSexVar + _T("`=2 AND ") + sWhereClause + _T(") \n\
					GROUP BY `") + sE + _T("ID`, `") + sIndCat + _T("`) AS `") + sE + _T("_") + sIndCat + sFExt + _T("`) \n\
					USING (`") + sE + _T("ID`, `") + sIndCat + _T("`)) AS `") + sE + _T("_") + sIndCat + _T("_All`) \n\
					ON LEFT(`") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("ID`, ") + sEDigit + _T(") = LEFT(`") + sE + _T("_") + sIndCat + _T("_All`.`") + sE + _T("ID`, ") + sEDigit + _T(") AND `valueset`.`value` = `") + sE + _T("_") + sIndCat + _T("_All`.`") + sIndCat + _T("`; \n");
			}
			else {
				sSQL = _T("CREATE TABLE `") + sE + _T("_") + sIndCat + _T("` \n\
					SELECT `") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, ") + sSQLGeo(sSD) + _T(", `") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + _T("` AS `") + sE + _T("_") + sDenom + _T("`, `") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + sMExt + _T("` AS `") + sE + _T("_") + sDenom + sMExt + _T("`, `") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + sFExt + _T("` AS `") + sE + _T("_") + sDenom + sFExt + _T("`,  \n\
					`valueset`.`value` AS `") + sIndCat + _T("`, `valueset`.`label` AS `") + sIndCat + _T("_Label`, IFNULL(`") + sE + _T("_") + sIndCat + _T("`, 0) AS `") + sE + _T("_") + sIndCat + _T("`, IFNULL(`") + sE + _T("_") + sIndCat + sMExt + _T("`, 0) AS `") + sE + _T("_") + sIndCat + sMExt + _T("`, IFNULL(`") + sE + _T("_") + sIndCat + sFExt + _T("`, 0) AS `") + sE + _T("_") + sIndCat + sFExt + _T("`, \n\
					(IFNULL(`") + sE + _T("_") + sIndCat + _T("`, 0)/`") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + _T("`)*100 AS `") + sE + _T("_") + sIndCat + _T("_Prop`, (IFNULL(`") + sE + _T("_") + sIndCat + sMExt + _T("`, 0)/`") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + sMExt + _T("`)*100 AS `") + sE + _T("_") + sIndCat + sMExt + _T("_Prop`, (IFNULL(`") + sE + _T("_") + sIndCat + sFExt + _T("`, 0)/`") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("_") + sDenom + sFExt + _T("`)*100 AS `") + sE + _T("_") + sIndCat + sFExt + _T("_Prop`  \n\
					FROM (((SELECT * FROM `~helementvalue` WHERE `~helementvalue`.`var`='") + sIndCat + _T("' AND `~helementvalue`.`elementID`=") + sSrcEID + _T(") AS `valueset`), `") + sE + _T("_") + sSrcDenom + _T("`) \n\
					LEFT JOIN \n\
					((SELECT `") + sE + _T("_") + sIndCat + _T("_Temp`.`") + sE + _T("ID` AS `") + sE + _T("ID`, `") + sE + _T("_") + sIndCat + _T("_Temp`.`") + sIndCat + _T("` AS `") + sIndCat + _T("`, IFNULL(`") + sE + _T("_") + sIndCat + _T("`, 0) AS `") + sE + _T("_") + sIndCat + _T("`, IFNULL(`") + sE + _T("_") + sIndCat + sMExt + _T("`, 0) AS `") + sE + _T("_") + sIndCat + sMExt + _T("`, IFNULL(`") + sE + _T("_") + sIndCat + sFExt + _T("`, 0) AS `") + sE + _T("_") + sIndCat + sFExt + _T("` FROM  \n\
					((SELECT LEFT(") + sIDF + _T(", ") + sEDigit + _T(") AS `") + sE + _T("ID`, `") + sSrcIndCat + _T("`.`") + sIndCat + _T("` AS `") + sIndCat + _T("`, ") + sAggrFcn + _T("(`") + sSrcIndCat + _T("`.`") + sIndCat + _T("`) AS `") + sE + _T("_") + sIndCat + _T("` \n\
					FROM ") + sFromClause + _T(" \n\
					GROUP BY `") + sE + _T("ID`, `") + sIndCat + _T("`) AS `") + sE + _T("_") + sIndCat + _T("_Temp`) \n\
					LEFT JOIN \n\
					((SELECT LEFT(") + sIDF + _T(", ") + sEDigit + _T(") AS `") + sE + _T("ID`, `") + sSrcIndCat + _T("`.`") + sIndCat + _T("` AS `") + sIndCat + _T("`, ") + sAggrFcn + _T("(`") + sSrcIndCat + _T("`.`") + sIndCat + _T("`) AS `") + sE + _T("_") + sIndCat + sMExt + _T("` \n\
					FROM ") + sFromClause + _T(" \n\
					WHERE (`") + sSrcIndCat + _T("`.`") + sSexVar + _T("`=1) \n\
					GROUP BY `") + sE + _T("ID`, `") + sIndCat + _T("`) AS `") + sE + _T("_") + sIndCat + sMExt + _T("`) \n\
					USING (`") + sE + _T("ID`, `") + sIndCat + _T("`) \n\
					LEFT JOIN \n\
					((SELECT LEFT(") + sIDF + _T(", ") + sEDigit + _T(") AS `") + sE + _T("ID`, `") + sSrcIndCat + _T("`.`") + sIndCat + _T("` AS `") + sIndCat + _T("`, ") + sAggrFcn + _T("(`") + sSrcIndCat + _T("`.`") + sIndCat + _T("`) AS `") + sE + _T("_") + sIndCat + sFExt + _T("` \n\
					FROM ") + sFromClause + _T(" \n\
					WHERE (`") + sSrcIndCat + _T("`.`") + sSexVar + _T("`=2) \n\
					GROUP BY `") + sE + _T("ID`, `") + sIndCat + _T("`) AS `") + sE + _T("_") + sIndCat + sFExt + _T("`) \n\
					USING (`") + sE + _T("ID`, `") + sIndCat + _T("`)) AS `") + sE + _T("_") + sIndCat + _T("_All`) \n\
					ON LEFT(`") + sE + _T("_") + sSrcDenom + _T("`.`") + sE + _T("ID`, ") + sEDigit + _T(") = LEFT(`") + sE + _T("_") + sIndCat + _T("_All`.`") + sE + _T("ID`, ") + sEDigit + _T(") AND `valueset`.`value` = `") + sE + _T("_") + sIndCat + _T("_All`.`") + sIndCat + _T("`; \n");
			}
		}
	}
	
	else {
		AfxMessageBox(_T("To be updated!"));
	}

	return sSQL;

}

CString CStatSimElt::sSQLHHIndCat(ELEMENT eT, LPCSTR sI, LPCSTR sD, LPCSTR sSI, 
	LPCSTR sSD, LPCSTR sIDField, LPCSTR sFilter, LPCSTR sWt)
{
	CString sSQL, sIndCat, sDenom,
		sSrcIndCat, sSrcDenom, sSQLProp,
		sE, sEIDFld, sEName, sEDigit, sIDF,
		sSrcEID, sProxyTable, sValClause;

	sIndCat.Format(_T("%s"), (CString) sI);
	sDenom.Format(_T("%s"), (CString) sD);

	sSrcIndCat.Format(_T("%s"), (CString) sSI);
	sSrcDenom.Format(_T("%s"), (CString) sSD);
	sIDF.Format(_T("%s"), (CString) sIDField);

	if (sWt)
		sValClause.Format(_T("SUM(`%s`.`%s`)"), (CString) sSrcIndCat, (CString) sWt);
	else
		sValClause= _T("COUNT(`") + sSrcIndCat + _T("`.`") + sIndCat + _T("`)");

	sSrcEID.Format(_T("%d"), GetSrcID(sSI));

	sE = Attr(element);
	sEIDFld = Attr(IDField);
	sEName = Attr(name);
	sEDigit = Attr(digit);

	if (sFilter!=NULL) {
		sProxyTable.Format(_T("((SELECT * FROM `%s` WHERE %s) AS `%s`)"), (CString) sSI, (CString) sFilter, (CString) sSI );
	}
	else {
		sProxyTable.Format(_T("%s"), (CString) sSI);
	}

	switch(currElt) {

	case MEM:
				
		sSQL = _T("");		

		break;

	case HH:
		
		sSQL = _T("");		

		break;

	case PUROK:

		sSQL = _T("");		

		break;

	case BRGY:
		
		sSQL = _T("CREATE TABLE `Brgy_") + sIndCat + _T("` \n\
			SELECT `brgy_") + sSrcDenom + _T("`.`brgyID` AS `brgyID`, `brgy_") + sSrcDenom + _T("`.`Region` AS `Region`, `brgy_") + sSrcDenom + _T("`.`Province` AS `Province`, `brgy_") + sSrcDenom + _T("`.`Municipality` AS `Municipality`, `brgy_") + sSrcDenom + _T("`.`Barangay` AS `Barangay`, `brgy_") + sSrcDenom + _T("`.`brgy_") + sDenom + _T("` AS `Brgy_") + sDenom + _T("`, \n\
			`valueset`.`value` AS `") + sIndCat + _T("`, `valueset`.`label` AS `") + sIndCat + _T("_Label`, ") + sValClause + _T(" AS `Brgy_") + sIndCat + _T("`, (") + sValClause + _T("/`brgy_") + sSrcDenom + _T("`.`brgy_") + sDenom + _T("`)*100 AS `Brgy_") + sIndCat + _T("_Prop` \n\
			FROM (((SELECT * FROM `~helementvalue` WHERE `~helementvalue`.`var`='") + sIndCat + _T("' AND `~helementvalue`.`elementID`=") + sSrcEID + _T(") AS `valueset`), `brgy_") + sSrcDenom + _T("`) \n\
			LEFT JOIN ") + sProxyTable + _T(" ON ((LEFT(`brgy_") + sSrcDenom + _T("`.`brgyID`, 9) = LEFT(") + sIDF + _T(", 9)) AND `valueset`.`value`=`") + sSrcIndCat + _T("`.`") + sIndCat + _T("`) \n\
			GROUP BY `brgyID`, `valueset`.`value`;");

		break;

	case MUN:
		
		sSQL = _T("CREATE TABLE `Mun_") + sIndCat + _T("` \n\
			SELECT `mun_") + sSrcDenom + _T("`.`munID` AS `munID`, `mun_") + sSrcDenom + _T("`.`Region` AS `Region`, `mun_") + sSrcDenom + _T("`.`Province` AS `Province`, `mun_") + sSrcDenom + _T("`.`Municipality` AS `Municipality`, `mun_") + sSrcDenom + _T("`.`mun_") + sDenom + _T("` AS `Mun_") + sDenom + _T("`, \n\
			`valueset`.`value` AS `") + sIndCat + _T("`, `valueset`.`label` AS `") + sIndCat + _T("_Label`, ") + sValClause + _T(" AS `Mun_") + sIndCat + _T("`, (") + sValClause + _T("/`mun_") + sSrcDenom + _T("`.`mun_") + sDenom + _T("`)*100 AS `Mun_") + sIndCat + _T("_Prop` \n\
			FROM (((SELECT * FROM `~helementvalue` WHERE `~helementvalue`.`var`='") + sIndCat + _T("' AND `~helementvalue`.`elementID`=") + sSrcEID + _T(") AS `valueset`), `mun_") + sSrcDenom + _T("`) \n\
			LEFT JOIN ") + sProxyTable + _T(" ON ((LEFT(`mun_") + sSrcDenom + _T("`.`munID`, 6) = LEFT(") + sIDF + _T(", 6)) AND `valueset`.`value`=`") + sSrcIndCat + _T("`.`") + sIndCat + _T("`) \n\
			GROUP BY `munID`, `valueset`.`value`;");

		break;

	case PROV:
		
		sSQL = _T("CREATE TABLE `Prov_") + sIndCat + _T("` \n\
			SELECT `prov_") + sSrcDenom + _T("`.`provID` AS `provID`, `prov_") + sSrcDenom + _T("`.`Region` AS `Region`, `prov_") + sSrcDenom + _T("`.`Province` AS `Province`, `prov_") + sSrcDenom + _T("`.`prov_") + sDenom + _T("` AS `Prov_") + sDenom + _T("`, \n\
			`valueset`.`value` AS `") + sIndCat + _T("`, `valueset`.`label` AS `") + sIndCat + _T("_Label`, ") + sValClause + _T(" AS `Prov_") + sIndCat + _T("`, (") + sValClause + _T("/`prov_") + sSrcDenom + _T("`.`prov_") + sDenom + _T("`)*100 AS `Prov_") + sIndCat + _T("_Prop` \n\
			FROM (((SELECT * FROM `~helementvalue` WHERE `~helementvalue`.`var`='") + sIndCat + _T("' AND `~helementvalue`.`elementID`=") + sSrcEID + _T(") AS `valueset`), `prov_") + sSrcDenom + _T("`) \n\
			LEFT JOIN ") + sProxyTable + _T(" ON ((LEFT(`prov_") + sSrcDenom + _T("`.`provID`, 4) = LEFT(") + sIDF + _T(", 4)) AND `valueset`.`value`=`") + sSrcIndCat + _T("`.`") + sIndCat + _T("`) \n\
			GROUP BY `provID`, `valueset`.`value`;");

		break;

	case REGN:
		
		sSQL = _T("CREATE TABLE `Regn_") + sIndCat + _T("` \n\
			SELECT `regn_") + sSrcDenom + _T("`.`regnID` AS `regnID`, `regn_") + sSrcDenom + _T("`.`Region` AS `Region`, `regn_") + sSrcDenom + _T("`.`regn_") + sDenom + _T("` AS `Regn_") + sDenom + _T("`, \n\
			`valueset`.`value` AS `") + sIndCat + _T("`, `valueset`.`label` AS `") + sIndCat + _T("_Label`, ") + sValClause + _T(" AS `Regn_") + sIndCat + _T("`, (") + sValClause + _T("/`regn_") + sSrcDenom + _T("`.`regn_") + sDenom + _T("`)*100 AS `Regn_") + sIndCat + _T("_Prop` \n\
			FROM (((SELECT * FROM `~helementvalue` WHERE `~helementvalue`.`var`='") + sIndCat + _T("' AND `~helementvalue`.`elementID`=") + sSrcEID + _T(") AS `valueset`), `regn_") + sSrcDenom + _T("`) \n\
			LEFT JOIN ") + sProxyTable + _T(" ON ((LEFT(`regn_") + sSrcDenom + _T("`.`regnID`, 2) = LEFT(") + sIDF + _T(", 2)) AND `valueset`.`value`=`") + sSrcIndCat + _T("`.`") + sIndCat + _T("`) \n\
			GROUP BY `regnID`, `valueset`.`value`;");

		break;
		
	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}

	return sSQL;

}


CString CStatSimElt::sSQLCreate()
{
	CString sSQL;

	switch(currElt) {

	case MEM:

		//make table of members in construction since there could be inconsistencies
		//default is empty
		sSQL.Format( _T("CREATE TABLE IF NOT EXISTS `mem` \n\
			(`regn` tinyint(%d) UNSIGNED ZEROFILL, \
			`prov` tinyint(%d) UNSIGNED ZEROFILL, \
			`mun` tinyint(%d) UNSIGNED ZEROFILL, \
			`brgy` smallint(%d) UNSIGNED ZEROFILL, \
			`purok` tinyint(%d) UNSIGNED ZEROFILL, \
			`hcn` int(%d) UNSIGNED ZEROFILL, \
			`memno` tinyint(%d) UNSIGNED ZEROFILL, \
			`memID` VARCHAR(%d), `Member` VARCHAR(100), \n\
			PRIMARY KEY (`memID`), INDEX memIndex (`regn`, `prov`, `mun`, `brgy`, `purok`, `hcn`, `memno`, `memID`, `Member`) );"),
			regn_dig-country_dig,
			prov_dig-regn_dig,
			mun_dig-prov_dig,
			brgy_dig-mun_dig,
			purok_dig-brgy_dig,
			hh_dig-purok_dig,
			mem_dig-hh_dig,
			mem_dig);
		
		
		break;

	case HH:
	
		//make table of hh in construction since there could be inconsistencies
		//default is empty
		sSQL.Format(_T("CREATE TABLE IF NOT EXISTS `hh` \n\
			(`regn` tinyint(%d) UNSIGNED ZEROFILL, \
			`prov` tinyint(%d) UNSIGNED ZEROFILL, \
			`mun` tinyint(%d) UNSIGNED ZEROFILL, \
			`brgy` smallint(%d) UNSIGNED ZEROFILL, \
			`purok` tinyint(%d) UNSIGNED ZEROFILL, \
			`hcn` int(%d) UNSIGNED ZEROFILL, \
			`hhID` VARCHAR(%d), `hhHead` VARCHAR(100), \n\
			`geopoint_hh_long` double UNSIGNED ZEROFILL, \
			`geopoint_hh_lat` double UNSIGNED ZEROFILL, \
			PRIMARY KEY (`hhID`), INDEX hhIndex (`regn`, `prov`, `mun`, `brgy`, `purok`, `hcn`, `hhID`, `hhHead`) );"),
			regn_dig-country_dig,
			prov_dig-regn_dig,
			mun_dig-prov_dig,
			brgy_dig-mun_dig,
			purok_dig-brgy_dig,
			hh_dig-purok_dig,
			hh_dig);

		break;

	case PUROK:
		
		//make table of puroks in construction since there could be inconsistencies
		//default is empty

		sSQL = _T("CREATE TABLE IF NOT EXISTS `purok` \n\
			(`regn` tinyint(2) UNSIGNED ZEROFILL, `prov` tinyint(2) UNSIGNED ZEROFILL, `mun` tinyint(2) UNSIGNED ZEROFILL, `brgy` smallint(3) UNSIGNED ZEROFILL, `purok` tinyint(2) UNSIGNED ZEROFILL, `purokID` VARCHAR(11), `PurokName` VARCHAR(50), \n\
			PRIMARY KEY (`purokID`), INDEX purokIndex (`regn`, `prov`, `mun`, `brgy`, `purok`, `purokID`, `PurokName`) );");
		
		break;

	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}

	return sSQL;

}

CString CStatSimElt::sSQLUpdate()
{
	CString sSQL;

	switch(currElt) {

	case MEM:
				
		sSQL = _T("INSERT IGNORE INTO `mem` \n\
			SELECT `hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`, CONCAT(regn, prov, mun, brgy, purok, hcn, memno) AS `memID`, IFNULL( IFNULL( CONCAT(`hpq_mem`.`msname`, ', ', `hpq_mem`.`mfname`), `hpq_mem`.`msname` ), `hpq_mem`.`mfname` ) AS `Member` \n\
			FROM `hpq_mem` \n\
			GROUP BY `hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`;");		

		break;

	case HH:
		
		if (field_exists(_T("geopoint_hh_long"), _T("hpq_hh"), pEltDB ) && field_exists(_T("geopoint_hh_long"), _T("hpq_hh"), pEltDB )) {
			sSQL = _T("INSERT IGNORE INTO `hh` \n\
					  SELECT hpq_hh.regn, hpq_hh.prov, hpq_hh.mun, hpq_hh.brgy, hpq_hh.purok, hpq_hh.hcn, CONCAT(hpq_hh.regn, hpq_hh.prov, hpq_hh.mun, hpq_hh.brgy, hpq_hh.purok, hpq_hh.hcn) AS `hhID`, `hhHead`, `geopoint_hh_long`, `geopoint_hh_lat` \n\
					  FROM hpq_hh LEFT JOIN hpq_HEADS \n\
					  USING (regn, prov, mun, brgy, purok, hcn);");		
		}
		else {
			sSQL = _T("INSERT IGNORE INTO `hh` \n\
					  SELECT hpq_hh.regn, hpq_hh.prov, hpq_hh.mun, hpq_hh.brgy, hpq_hh.purok, hpq_hh.hcn, CONCAT(hpq_hh.regn, hpq_hh.prov, hpq_hh.mun, hpq_hh.brgy, hpq_hh.purok, hpq_hh.hcn) AS `hhID`, `hhHead`, NULL as `geopoint_hh_long`, NULL as `geopoint_hh_lat` \n\
					  FROM hpq_hh LEFT JOIN hpq_HEADS \n\
					  USING (regn, prov, mun, brgy, purok, hcn);");		
		}
/*		sSQL = _T("INSERT IGNORE INTO `hh` \n\
			SELECT hpq_hh.regn, hpq_hh.prov, hpq_hh.mun, hpq_hh.brgy, hpq_hh.purok, hpq_hh.hcn, CONCAT(hpq_hh.regn, hpq_hh.prov, hpq_hh.mun, hpq_hh.brgy, hpq_hh.purok, hpq_hh.hcn) AS `hhID`, IFNULL( IFNULL( IFNULL( CONCAT(`hpq_mem`.`msname`, ', ', `hpq_mem`.`mfname`), `hpq_mem`.`msname` ), `hpq_mem`.`mfname` ), 'NULL') AS `hhHead` \n\
			FROM hpq_hh LEFT JOIN hpq_mem USING (regn, prov, mun, brgy, purok, hcn) \n\
			WHERE `hpq_mem`.`reln` = 1 \n\
			GROUP BY hpq_hh.regn, hpq_hh.prov, hpq_hh.mun, hpq_hh.brgy, hpq_hh.purok, hpq_hh.hcn;");		
*/
		break;

	case PUROK:
			
		sSQL = _T("INSERT IGNORE INTO `purok` \n\
			SELECT `hpq_hh`.regn, `hpq_hh`.prov, `hpq_hh`.mun, `hpq_hh`.brgy, `hpq_hh`.purok, CONCAT(regn, prov, mun, brgy, purok) AS purokID, \n\
			CONCAT('Purok ', `hpq_hh`.`purok`) AS PurokName \n\
			FROM `hpq_hh` \n\
			GROUP BY `hpq_hh`.regn, `hpq_hh`.prov, `hpq_hh`.mun, `hpq_hh`.brgy, `hpq_hh`.purok;");		

		break;
		
	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}
	return sSQL;

}

void CStatSimElt::CreateTable(BOOL dodrop)
{
	if (dodrop) {
		CString sSQL;
		sSQL.Format(_T("DROP TABLE IF EXISTS `%s`;"), (CString) Attr(element));
		pEltDB->ExecuteSQL( sSQL );
	}

	//execute the create

	//FILE *pF = fopen("d:\\my.sql", "w");
	//fwprintf(pF, sSQLCreate());
	//fclose(pF);


	pEltDB->ExecuteSQL( sSQLCreate() );

}

void CStatSimElt::UpdateTable()
{
	
	//create first
	CreateTable();

	CString sSQL;

	//heads table
	if (currElt==HH) {
		sSQL = "drop table if exists hpq_heads;";
		pEltDB->ExecuteSQL( sSQL );

		sSQL = "CREATE TABLE hpq_heads \n\
			   SELECT regn, prov, mun, brgy, purok, hcn, IFNULL( IFNULL( IFNULL( CONCAT(`hpq_mem`.`msname`, ', ', `hpq_mem`.`mfname`), `hpq_mem`.`msname` ), `hpq_mem`.`mfname` ), 'NULL') AS `hhHead`  \n\
			   FROM hpq_mem WHERE `hpq_mem`.`reln` = 1 GROUP BY regn, prov, mun, brgy, purok, hcn;";
	
		pEltDB->ExecuteSQL( sSQL );

		sSQL = "alter table hpq_heads add index headidx (regn, prov, mun, brgy, purok, hcn);";
		pEltDB->ExecuteSQL( sSQL );

	}
	//execute the update
	pEltDB->ExecuteSQL( sSQLUpdate() );

	if (currElt==HH) {
		sSQL = "drop table hpq_heads;";
		pEltDB->ExecuteSQL( sSQL );

	}

}

CString CStatSimElt::IdxClause()
{
		
	CString strIndex;

	switch(currElt) {

	case MEM:
		strIndex = _T("INDEX hpqIndex (regn, prov, mun, brgy, purok, hcn)");
		break;
	case HH:
		strIndex = _T("INDEX hpqIndex (regn, prov, mun, brgy, purok, hcn)");
		break;

	case BRGY:
		strIndex = _T("INDEX bpqIndex (regn, prov, mun, brgy)");
		break;

	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	}

	return strIndex;

}

void CStatSimElt::AddPrimaryKey(CString sKey)
{
	CString sSQL;
	ELTATTR iAttr = element;
	
	sSQL.Format(_T("ALTER TABLE `%s`\n\
		ADD PRIMARY KEY (`%s`);"), (CString) attrArr[iAttr], sKey);

	pEltDB->ExecuteSQL(sSQL, FALSE);

}

void CStatSimElt::AddPrimaryKey(LPCSTR sTable, LPCSTR sKey)
{
	CString sSQL;
	
	sSQL.Format(_T("ALTER TABLE `%s` \n\
		ADD PRIMARY KEY (%s);"), (CString) sTable, sKey);

	pEltDB->ExecuteSQL(sSQL, FALSE);

}

void CStatSimElt::AddPrimaryKey(CString sTabExt, CString sKey)
{
	CString sSQL;
	ELTATTR iAttr = element;
	
	sSQL.Format(_T("ALTER TABLE `%s%s`\n\
		ADD PRIMARY KEY (`%s`);"), (CString) attrArr[iAttr], sTabExt, sKey);

	pEltDB->ExecuteSQL(sSQL, FALSE);

}

void CStatSimElt::AddKey(CString sKey)
{
	CString sSQL;
	ELTATTR iAttr = element;
	
	sSQL.Format(_T("ALTER TABLE `%s`\n\
		ADD INDEX (`%s`);"), (CString) attrArr[iAttr], sKey);

	pEltDB->ExecuteSQL(sSQL, FALSE);

}

void CStatSimElt::AddKey(CString sTabExt, CString sKey)
{
	CString sSQL;
	ELTATTR iAttr = element;
	
	sSQL.Format(_T("ALTER TABLE `%s%s`\n\
		ADD INDEX (`%s`);"), (CString) attrArr[iAttr], sTabExt, sKey);

	pEltDB->ExecuteSQL(sSQL, FALSE);

}

CString CStatSimElt::sSQLInd()
{
	
	CString sSQL;

	switch(currElt) {

	case MEM:
		
		if (m_qnrID==1120041100 || m_qnrID==1020040900) {
		//general - 11200411 with migr (modified for PMT)
		sSQL = "CREATE TABLE Mem_Ind \n\
			SELECT *, If((mem04=1 And (MNUTIND=3 OR MNUTIND=4)),1,0) AS maln04, If((mem04=1 And (MNUTIND=3 OR MNUTIND=4) and sex=1),1,0) AS maln04_male, If((mem04=1 And (MNUTIND=3 OR MNUTIND=4) and sex=2),1,0) AS maln04_female, If((maln05_mod=1 Or maln05_sev=1),1,0) AS maln05, If(((maln05_mod=1 Or maln05_sev=1) And Sex=1),1,0) AS maln05_male, If(((maln05_mod=1 Or maln05_sev=1) And Sex=2),1,0) AS maln05_female, labfor-empl15ab AS unempl15ab, labfor_male-empl15ab_male AS unempl15ab_male, labfor_female-empl15ab_female AS unempl15ab_female, If((empl15ab=1),g_occ,-1) AS TypOcc, If((empl15ab=1),sector,-1) AS TypSec, If((sector<>1 AND jobind=1),1,0) AS ntAgri, If((empl15ab=1),jstatus,-1) AS JobStat \n\
			FROM (SELECT *, If(mem05=1,mnutind,-1) AS NutrStat, If((mem05=1 And MNUTIND=3),1,0) AS maln05_mod, If((mem05=1 And MNUTIND=4),1,0) AS maln05_sev, If((mem35=1 And daycare=1),1,0) AS daycare35, If(educal=17,1,0) AS educa2, If(educal>=21 AND educal<=24,1,0) AS educa3, If(educal=25,1,0) AS educa4, If((educal>=26 AND educal<=28) OR (educal>=31 AND educal<=34),1,0) AS educa5, If((educal=29) OR (educal>=35 AND educal<=37),1,0) AS educa67, If((mem612=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem612, If((mem612_male=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem612_male, If((mem612_female=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem612_female, If((mem611=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem611, If((mem611_male=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem611_male, If((mem611_female=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem611_female, If((mem1316=1 And educind=1 And gradel>=21 And gradel<=24),1,0) AS hs1316, If((mem1316_male=1 And educind=1 And gradel>=21 And gradel<=24),1,0) AS hs1316_male, If((mem1316_female=1 And educind=1 And gradel>=21 And gradel<=24),1,0) AS hs1316_female, If((mem1215=1 And educind=1 And gradel>=21 And gradel<=24),1,0) AS hs1215, If((mem1215_male=1 And educind=1 And gradel>=21 And gradel<=24),1,0) AS hs1215_male, If((mem1215_female=1 And educind=1 And gradel>=21 And gradel<=24),1,0) AS hs1215_female, If((mem616=1 And educind=1),1,0) AS sch616, If((mem616_male=1 And educind=1),1,0) AS sch616_male, If((mem616_female=1 And educind=1),1,0) AS sch616_female, If((mem615=1 And educind=1),1,0) AS sch615, If((mem615_male=1 And educind=1),1,0) AS sch615_male, If((mem615_female=1 And educind=1),1,0) AS sch615_female, If((mem1620=1 And educind=1 And gradel>=31 And gradel<=34),1,0) AS tert1620, If((mem1620_male=1 And educind=1 And gradel>=31 And gradel<=34),1,0) AS tert1620_male, If((mem1620_female=1 And educind=1 And gradel>=31 And gradel<=34),1,0) AS tert1620_female, If((mem1721=1 And educind=1 And gradel>=31),1,0) AS tert1721, If((mem1721_male=1 And educind=1 And gradel>=31),1,0) AS tert1721_male, If((mem1721_female=1 And educind=1 And gradel>=31),1,0) AS tert1721_female, If((mem10ab=1 And literind=1),1,0) AS liter10ab, If((mem10ab_male=1 And literind=1),1,0) AS liter10ab_male, If((mem10ab_female=1 And literind=1),1,0) AS liter10ab_female, If((mem1524=1 And literind=1),1,0) AS liter1524, If((mem1524_male=1 And literind=1),1,0) AS liter1524_male, If((mem1524_female=1 And literind=1),1,0) AS liter1524_female, \n\
			if((mem612=1 and educind=2) or (mem612=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem612, if((mem612_male=1 and educind=2) or (mem612_male=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem612_male, if((mem612_female=1 and educind=2) or (mem612_female=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem612_female, if((mem611=1 and educind=2) or (mem611=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem611, if((mem611_male=1 and educind=2) or (mem611_male=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem611_male, if((mem611_female=1 and educind=2) or (mem611_female=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem611_female, if((mem1316=1 and educind=2) or (mem1316=1 and educind=1 and (gradel<21 or gradel>24)),1,0) as nths1316, if((mem1316_male=1 and educind=2) or (mem1316_male=1 and educind=1 and (gradel<21 or gradel>24)),1,0) as nths1316_male, if((mem1316_female=1 and educind=2) or (mem1316_female=1 and educind=1 and (gradel<21 or gradel>24)),1,0) as nths1316_female, if((mem1215=1 and educind=2) or (mem1215=1 and educind=1 and (gradel<21 or gradel>24)),1,0) as nths1215, if((mem1215_male=1 and educind=2) or (mem1215_male=1 and educind=1 and (gradel<21 or gradel>24)),1,0) as nths1215_male, if((mem1215_female=1 and educind=2) or (mem1215_female=1 and educind=1 and (gradel<21 or gradel>24)),1,0) as nths1215_female, if((mem616=1 and educind=2),1,0) as ntsch616, if((mem616_male=1 and educind=2),1,0) as ntsch616_male, if((mem616_female=1 and educind=2),1,0) as ntsch616_female, if((mem615=1 and educind=2),1,0) as ntsch615, if((mem615_male=1 and educind=2),1,0) as ntsch615_male, if((mem615_female=1 and educind=2),1,0) as ntsch615_female, if((mem10ab=1 and literind=2),1,0) as ntliter10ab, if((mem10ab_male=1 and literind=2),1,0) as ntliter10ab_male, if((mem10ab_female=1 and literind=2),1,0) as ntliter10ab_female, \n\
			If(((mem15ab=1 And jobind=1) Or (mem15ab=1 And jobind=2 And fjob=1) Or (mem15ab=1 And jobind=2 And fjob=2 And ynojob>=2 And ynojob<=5)),1,0) AS labfor, If(((mem15ab_male=1 And jobind=1) Or (mem15ab_male=1 And jobind=2 And fjob=1) Or (mem15ab_male=1 And jobind=2 And fjob=2 And ynojob>=2 And ynojob<=5)),1,0) AS labfor_male, If(((mem15ab_female=1 And jobind=1) Or (mem15ab_female=1 And jobind=2 And fjob=1) Or (mem15ab_female=1 And jobind=2 And fjob=2 And ynojob>=2 And ynojob<=5)),1,0) AS labfor_female, IF(jobind=2,1,0) AS wnojob, IF(jobind=2 And sex=1,1,0) AS wnojob_male, IF(jobind=2 And sex=2,1,0) AS wnojob_female, If((mem15ab=1 And jobind=1),1,0) AS empl15ab, If((mem15ab_male=1 And jobind=1),1,0) AS empl15ab_male, If((mem15ab_female=1 And jobind=1),1,0) AS empl15ab_female, If((mem15ab<>1 And jobind=1),1,0) AS empl014 \n\
			FROM (SELECT hpq_mem.regn, hpq_mem.prov, hpq_mem.mun, hpq_mem.brgy, hpq_mem.purok, hpq_mem.hcn, hpq_mem.memno, CONCAT(regn, prov, mun, brgy, purok, hcn, memno) AS `memID`, \n\
			reln, SEX, age_yr, educal, if(sex=1,1,0) as Male,  if(sex=2,1,0) as Female,  if(reln<>8,1,0) as nthhwrkr, If((ipind=1),1,0) AS IPIndicator, If((ipind=1),IPGRP,-1) AS IPGroup, If((age_yr>=60),1,0) AS mem60ab, If((age_yr>=60) AND sex=1,1,0) AS mem60ab_male, If((age_yr>=60) AND sex=2,1,0) AS mem60ab_female, If((age_yr>=65),1,0) AS mem65ab, If((age_yr>=65) AND sex=1,1,0) AS mem65ab_male, If((age_yr>=65) AND sex=2,1,0) AS mem65ab_female, If(age_yr=0,1,0) AS mem01, If((age_yr=0) and (sex=1),1,0) AS mem01_male, If((age_yr=0) and (sex=2),1,0) AS mem01_female, If((age_yr<=5),1,0) AS mem05, If((age_yr<=5) and (sex=1),1,0) AS mem05_male, If((age_yr<=5) and (sex=2),1,0) AS mem05_female, If((age_yr<=4),1,0) AS mem04, If((age_yr<=4) and (sex=1),1,0) AS mem04_male, If((age_yr<=4) and (sex=2),1,0) AS mem04_female, If((age_yr<=4 AND age_yr>=1),1,0) AS mem14, If((age_yr<=4 AND age_yr>=1) and (sex=1),1,0) AS mem14_male, If((age_yr<=4 AND age_yr>=1) and (sex=2),1,0) AS mem14_female, mnutind, If((age_yr>=3 And age_yr<=5),1,0) AS mem35, educind, gradel, If((educind=1 And gradel=1),1,0) AS daycare, If((educind=1 And gradel=2),1,0) AS kinprep, If((age_yr>=6 And age_yr<=12),1,0) AS mem612, If((age_yr>=6 And age_yr<=12) AND (sex=1),1,0) AS mem612_male, If((age_yr>=6 And age_yr<=12) AND (sex=2),1,0) AS mem612_female, If((age_yr>=6 And age_yr<=11),1,0) AS mem611, If((age_yr>=6 And age_yr<=11) AND (sex=1),1,0) AS mem611_male, If((age_yr>=6 And age_yr<=11) AND (sex=2),1,0) AS mem611_female, If((age_yr>=13 And age_yr<=16),1,0) AS mem1316, If((age_yr>=13 And age_yr<=16) AND (sex=1),1,0) AS mem1316_male, If((age_yr>=13 And age_yr<=16) AND (sex=2),1,0) AS mem1316_female, If((age_yr>=12 And age_yr<=15),1,0) AS mem1215, If((age_yr>=12 And age_yr<=15) AND (sex=1),1,0) AS mem1215_male, If((age_yr>=12 And age_yr<=15) AND (sex=2),1,0) AS mem1215_female, If((age_yr>=6 And age_yr<=16),1,0) AS mem616, If((age_yr>=6 And age_yr<=16) AND (sex=1),1,0) AS mem616_male, If((age_yr>=6 And age_yr<=16) AND (sex=2),1,0) AS mem616_female, If((age_yr>=6 And age_yr<=15),1,0) AS mem615, If((age_yr>=6 And age_yr<=15) AND (sex=1),1,0) AS mem615_male, If((age_yr>=6 And age_yr<=15) AND (sex=2),1,0) AS mem615_female, literind, If((age_yr>=10),1,0) AS mem10ab, If((age_yr>=10) AND sex=1,1,0) AS mem10ab_male, If((age_yr>=10) AND sex=2,1,0) AS mem10ab_female, If((age_yr>=15),1,0) AS mem15ab, If((age_yr>=15) AND sex=1,1,0) AS mem15ab_male, If((age_yr>=15) AND sex=2,1,0) AS mem15ab_female, \n\
			If((age_yr>=18),1,0) AS mem18ab, If((age_yr>=18) AND sex=1,1,0) AS mem18ab_male, If((age_yr>=18) AND sex=2,1,0) AS mem18ab_female, If((age_yr<=18),1,0) AS mem018, If((age_yr<=18) AND sex=1,1,0) AS mem018_male, If((age_yr<=18) AND sex=2,1,0) AS mem018_female, \n\
			If((age_yr>=17),1,0) AS mem17ab, If((age_yr>=17) AND sex=1,1,0) AS mem17ab_male, If((age_yr>=17) AND sex=2,1,0) AS mem17ab_female, If((age_yr<=17),1,0) AS mem017, If((age_yr<=17) AND sex=1,1,0) AS mem017_male, If((age_yr<=17) AND sex=2,1,0) AS mem017_female, \n\
			If((age_yr>=17 And age_yr<=21),1,0) AS mem1721, If((age_yr>=17 And age_yr<=21) AND (sex=1),1,0) AS mem1721_male, If((age_yr>=17 And age_yr<=21) AND (sex=2),1,0) AS mem1721_female, If((age_yr>=16 And age_yr<=20),1,0) AS mem1620, If((age_yr>=16 And age_yr<=20) AND (sex=1),1,0) AS mem1620_male, If((age_yr>=16 And age_yr<=20) AND (sex=2),1,0) AS mem1620_female, jobind, fjob, ynojob, g_occ, sector, jstatus, \n\
			If((age_yr>=0 And age_yr<=14),1,0) AS mem014, If((age_yr>=0 And age_yr<=14) AND (sex=1),1,0) AS mem014_male, If((age_yr>=0 And age_yr<=14) AND (sex=2),1,0) AS mem014_female, If((age_yr>=15 And age_yr<=64),1,0) AS mem1564, If((age_yr>=15 And age_yr<=64) AND (sex=1),1,0) AS mem1564_male, If((age_yr>=15 And age_yr<=64) AND (sex=2),1,0) AS mem1564_female, If((age_yr>=15 And age_yr<=24),1,0) AS mem1524, If((age_yr>=15 And age_yr<=24) AND (sex=1),1,0) AS mem1524_male, If((age_yr>=15 And age_yr<=24) AND (sex=2),1,0) AS mem1524_female, \n\
			If(ylen_resid<=5, 1, 0) AS migr, If((ylen_resid<=5 AND sex=1), 1, 0) AS migr_male, If((ylen_resid<=5 AND sex=2), 1, 0) AS migr_female, If((ylen_resid<=5 AND country_resid<>608), 1, 0) AS country_migr, If((ylen_resid<=5 AND country_resid<>608 AND sex=1), 1, 0) AS country_migr_male, If((ylen_resid<=5 AND country_resid<>608 AND sex=2), 1, 0) AS country_migr_female, If((ylen_resid<=5 AND prov<>prov_resid_code), 1, 0) AS prov_migr, If((ylen_resid<=5 AND prov<>prov_resid_code AND sex=1), 1, 0) AS prov_migr_male, If((ylen_resid<=5 AND prov<>prov_resid_code AND sex=2), 1, 0) AS prov_migr_female, If((ylen_resid<=5 AND (prov=prov_resid_code AND mun<>mun_resid_code)), 1, 0) AS mun_migr, If((ylen_resid<=5 AND (prov=prov_resid_code AND mun<>mun_resid_code) AND sex=1), 1, 0) AS mun_migr_male, If((ylen_resid<=5 AND (prov=prov_resid_code AND mun<>mun_resid_code) AND sex=2), 1, 0) AS mun_migr_female, If((ylen_resid<=5 AND (prov=prov_resid_code AND mun=mun_resid_code AND brgy<>brgy_resid_code)), 1, 0) AS brgy_migr, If((ylen_resid<=5 AND (prov=prov_resid_code AND mun=mun_resid_code AND brgy<>brgy_resid_code) AND sex=1), 1, 0) AS brgy_migr_male, If((ylen_resid<=5 AND (prov=prov_resid_code AND mun=mun_resid_code AND brgy<>brgy_resid_code) AND sex=2), 1, 0) AS brgy_migr_female \n\
			FROM `hpq_mem`) as hpq_mem_temp) as mem_ind_temp;";
		}

		else if (m_qnrID==1020070400 || m_qnrID==120070300) {

		//VN01200703 - rev gradel
		sSQL = "CREATE TABLE Mem_Ind \n\
			SELECT *, If((mem04=1 And (MNUTIND=3 OR MNUTIND=4)),1,0) AS maln04, If((mem04=1 And (MNUTIND=3 OR MNUTIND=4) and sex=1),1,0) AS maln04_male, If((mem04=1 And (MNUTIND=3 OR MNUTIND=4) and sex=2),1,0) AS maln04_female, If((maln05_mod=1 Or maln05_sev=1),1,0) AS maln05, If(((maln05_mod=1 Or maln05_sev=1) And Sex=1),1,0) AS maln05_male, If(((maln05_mod=1 Or maln05_sev=1) And Sex=2),1,0) AS maln05_female, If((maln05_mod=1 Or maln05_sev=1), 'malnourished', 'not malnourished') AS MEM_maln05, labfor-empl15ab AS unempl15ab, labfor_male-empl15ab_male AS unempl15ab_male, labfor_female-empl15ab_female AS unempl15ab_female, If(empl15ab=1, 'employed', 'unemployed') AS MEM_Unempl15ab, If((empl15ab=1),g_occ,-1) AS TypOcc, If((empl15ab=1),sector,-1) AS TypSec, If((sector<>1 AND jobind=1),1,0) AS ntAgri, If((empl15ab=1),jstatus,-1) AS JobStat, If(Elem611=1, 'attending elementary', 'not attending elementary') AS MEM_ntElem611, If(HS1215=1, 'attending high school', 'not attending high school') AS MEM_ntHS1215, If(Elem612=1, 'attending elementary', 'not attending elementary') AS MEM_ntElem612, If(HS1316=1, 'attending high school', 'not attending high school') AS MEM_ntHS1316, If(Sch615=1, 'attending school', 'not attending school') AS MEM_ntSch615, If(Sch616=1, 'attending school', 'not attending school') AS MEM_ntSch616, If(Liter10ab=1, 'literate', 'illiterate') AS MEM_ntLiter10ab \n\
			FROM (SELECT *, If(mem05=1,mnutind,-1) AS NutrStat, If((mem05=1 And MNUTIND=3),1,0) AS maln05_mod, If((mem05=1 And MNUTIND=4),1,0) AS maln05_sev, If((mem35=1 And daycare=1),1,0) AS daycare35, If(educal=17,1,0) AS educa2, If(educal>=21 AND educal<=24,1,0) AS educa3, If(educal=25,1,0) AS educa4, If((educal>=26 AND educal<=28) OR (educal>=31 AND educal<=34),1,0) AS educa5, If((educal=29) OR (educal>=35 AND educal<=37),1,0) AS educa67, IF(educind=1,1,0) AS Sch, IF(educind=1 AND sex=1,1,0) AS Sch_Male, IF(educind=1 AND sex=2,1,0) AS Sch_Female, If((mem612=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem612, If((mem612_male=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem612_male, If((mem612_female=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem612_female, \n\
			If((mem611=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem611, If((mem611_male=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem611_male, If((mem611_female=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem611_female, \n\
			If((mem1316=1 And educind=1 And gradel>=21 And gradel<=24),1,0) AS hs1316, If((mem1316_male=1 And educind=1 And gradel>=21 And gradel<=24),1,0) AS hs1316_male, If((mem1316_female=1 And educind=1 And gradel>=21 And gradel<=24),1,0) AS hs1316_female, \n\
			If((mem1215=1 And educind=1 And gradel>=21 And gradel<=24),1,0) AS hs1215, If((mem1215_male=1 And educind=1 And gradel>=21 And gradel<=24),1,0) AS hs1215_male, If((mem1215_female=1 And educind=1 And gradel>=21 And gradel<=24),1,0) AS hs1215_female, \n\
			If((mem616=1 And educind=1),1,0) AS sch616, If((mem616_male=1 And educind=1),1,0) AS sch616_male, If((mem616_female=1 And educind=1),1,0) AS sch616_female, If((mem615=1 And educind=1),1,0) AS sch615, If((mem615_male=1 And educind=1),1,0) AS sch615_male, If((mem615_female=1 And educind=1),1,0) AS sch615_female, If((mem1620=1 And educind=1 And gradel>=31 And gradel<=34),1,0) AS tert1620, If((mem1620_male=1 And educind=1 And gradel>=31 And gradel<=34),1,0) AS tert1620_male, If((mem1620_female=1 And educind=1 And gradel>=31 And gradel<=34),1,0) AS tert1620_female, If((mem1721=1 And educind=1 And gradel>=31),1,0) AS tert1721, If((mem1721_male=1 And educind=1 And gradel>=31),1,0) AS tert1721_male, If((mem1721_female=1 And educind=1 And gradel>=31),1,0) AS tert1721_female, If((mem10ab=1 And literind=1),1,0) AS liter10ab, If((mem10ab_male=1 And literind=1),1,0) AS liter10ab_male, If((mem10ab_female=1 And literind=1),1,0) AS liter10ab_female, If((mem1524=1 And literind=1),1,0) AS liter1524, If((mem1524_male=1 And literind=1),1,0) AS liter1524_male, If((mem1524_female=1 And literind=1),1,0) AS liter1524_female, \n\
			if((mem612=1 and educind=2) or (mem612=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem612, if((mem612_male=1 and educind=2) or (mem612_male=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem612_male, if((mem612_female=1 and educind=2) or (mem612_female=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem612_female, if((mem611=1 and educind=2) or (mem611=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem611, if((mem611_male=1 and educind=2) or (mem611_male=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem611_male, if((mem611_female=1 and educind=2) or (mem611_female=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem611_female, if((mem1316=1 and educind=2) or (mem1316=1 and educind=1 and (gradel<21 or gradel>24)),1,0) as nths1316, if((mem1316_male=1 and educind=2) or (mem1316_male=1 and educind=1 and (gradel<21 or gradel>24)),1,0) as nths1316_male, if((mem1316_female=1 and educind=2) or (mem1316_female=1 and educind=1 and (gradel<21 or gradel>24)),1,0) as nths1316_female, if((mem1215=1 and educind=2) or (mem1215=1 and educind=1 and (gradel<21 or gradel>24)),1,0) as nths1215, if((mem1215_male=1 and educind=2) or (mem1215_male=1 and educind=1 and (gradel<21 or gradel>24)),1,0) as nths1215_male, if((mem1215_female=1 and educind=2) or (mem1215_female=1 and educind=1 and (gradel<21 or gradel>24)),1,0) as nths1215_female, if((mem616=1 and educind=2),1,0) as ntsch616, if((mem616_male=1 and educind=2),1,0) as ntsch616_male, if((mem616_female=1 and educind=2),1,0) as ntsch616_female, if((mem615=1 and educind=2),1,0) as ntsch615, if((mem615_male=1 and educind=2),1,0) as ntsch615_male, if((mem615_female=1 and educind=2),1,0) as ntsch615_female, if((mem10ab=1 and literind=2),1,0) as ntliter10ab, if((mem10ab_male=1 and literind=2),1,0) as ntliter10ab_male, if((mem10ab_female=1 and literind=2),1,0) as ntliter10ab_female, \n\
			IF(jobind=2,1,0) AS wnojob, IF(jobind=2 And sex=1,1,0) AS wnojob_male, IF(jobind=2 And sex=2,1,0) AS wnojob_female, If(((mem15ab=1 And jobind=1) Or (mem15ab=1 And jobind=2 And fjob=1) Or (mem15ab=1 And jobind=2 And fjob=2 And ynotlookjob>=2 And ynotlookjob<=5 And joppind=1 And wtwind=1) Or (mem15ab=1 And jobind=2 And fjob=2 And ynotlookjob=1 And lastlookjob=1 And joppind=1 And wtwind=1)),1,0) AS labfor, \n\
			If(((mem15ab_male=1 And jobind=1) Or (mem15ab_male=1 And jobind=2 And fjob=1) Or (mem15ab_male=1 And jobind=2 And fjob=2 And ynotlookjob>=2 And ynotlookjob<=5 And joppind=1 And wtwind=1) Or (mem15ab_male=1 And jobind=2 And fjob=2 And ynotlookjob=1 And lastlookjob=1 And joppind=1 And wtwind=1)),1,0) AS labfor_male, If(((mem15ab_female=1 And jobind=1) Or (mem15ab_female=1 And jobind=2 And fjob=1) Or (mem15ab_female=1 And jobind=2 And fjob=2 And ynotlookjob>=2 And ynotlookjob<=5 And joppind=1 And wtwind=1) Or (mem15ab_female=1 And jobind=2 And fjob=2 And ynotlookjob=1 And lastlookjob=1 And joppind=1 And wtwind=1)),1,0) AS labfor_female, If((mem15ab=1 And jobind=1),1,0) AS empl15ab, If((mem15ab_male=1 And jobind=1),1,0) AS empl15ab_male, If((mem15ab_female=1 And jobind=1),1,0) AS empl15ab_female, If((mem15ab<>1 And jobind=1),1,0) AS empl014 \n\
			FROM (SELECT hpq_mem.urb, hpq_mem.regn, hpq_mem.prov, hpq_mem.mun, hpq_mem.brgy, hpq_mem.purok, hpq_mem.hcn, hpq_mem.memno, CONCAT(regn, prov, mun, brgy, purok, hcn, memno) AS `memID`, \n\
			reln, SEX, age_yr, civstat, relgn, educal, if(sex=1,1,0) as Male,  if(sex=2,1,0) as Female,  if(reln<>8,1,0) as nthhwrkr, IF(ipind=1,1,0) AS ipind, IF(ipind=1 And sex=1,1,0) AS ipind_male, IF(ipind=1 And sex=2,1,0) AS ipind_female, If((ipind=1),IPGRP,-1) AS IPgrp, If((age_yr>=60),1,0) AS mem60ab, If((age_yr>=60) AND sex=1,1,0) AS mem60ab_male, If((age_yr>=60) AND sex=2,1,0) AS mem60ab_female, If((age_yr>=65),1,0) AS mem65ab, If((age_yr>=65) AND sex=1,1,0) AS mem65ab_male, If((age_yr>=65) AND sex=2,1,0) AS mem65ab_female, If(age_yr=0,1,0) AS mem01, If((age_yr=0) and (sex=1),1,0) AS mem01_male, If((age_yr=0) and (sex=2),1,0) AS mem01_female, If((age_yr<=4),1,0) AS mem04, If((age_yr<=4) and (sex=1),1,0) AS mem04_male, If((age_yr<=4) and (sex=2),1,0) AS mem04_female, If((age_yr<=4 AND age_yr>=1),1,0) AS mem14, If((age_yr<=4 AND age_yr>=1) and (sex=1),1,0) AS mem14_male, If((age_yr<=4 AND age_yr>=1) and (sex=2),1,0) AS mem14_female, If((age_yr<=5),1,0) AS mem05, If((age_yr<=5) and (sex=1),1,0) AS mem05_male, If((age_yr<=5) and (sex=2),1,0) AS mem05_female, mnutind, If((age_yr>=3 And age_yr<=5),1,0) AS mem35, educind, gradel, If((educind=1 And gradel=1),1,0) AS daycare, If((educind=1 And gradel=2),1,0) AS kinprep, If((age_yr>=6 And age_yr<=12),1,0) AS mem612, If((age_yr>=6 And age_yr<=12) AND (sex=1),1,0) AS mem612_male, If((age_yr>=6 And age_yr<=12) AND (sex=2),1,0) AS mem612_female, If((age_yr>=6 And age_yr<=11),1,0) AS mem611, If((age_yr>=6 And age_yr<=11) AND (sex=1),1,0) AS mem611_male, If((age_yr>=6 And age_yr<=11) AND (sex=2),1,0) AS mem611_female, If((age_yr>=13 And age_yr<=16),1,0) AS mem1316, If((age_yr>=13 And age_yr<=16) AND (sex=1),1,0) AS mem1316_male, If((age_yr>=13 And age_yr<=16) AND (sex=2),1,0) AS mem1316_female, If((age_yr>=12 And age_yr<=15),1,0) AS mem1215, If((age_yr>=12 And age_yr<=15) AND (sex=1),1,0) AS mem1215_male, If((age_yr>=12 And age_yr<=15) AND (sex=2),1,0) AS mem1215_female, If((age_yr>=6 And age_yr<=16),1,0) AS mem616, If((age_yr>=6 And age_yr<=16) AND (sex=1),1,0) AS mem616_male, If((age_yr>=6 And age_yr<=16) AND (sex=2),1,0) AS mem616_female, If((age_yr>=6 And age_yr<=15),1,0) AS mem615, If((age_yr>=6 And age_yr<=15) AND (sex=1),1,0) AS mem615_male, If((age_yr>=6 And age_yr<=15) AND (sex=2),1,0) AS mem615_female, literind, If((age_yr>=10),1,0) AS mem10ab, If((age_yr>=10) AND sex=1,1,0) AS mem10ab_male, If((age_yr>=10) AND sex=2,1,0) AS mem10ab_female, If((age_yr>=15),1,0) AS mem15ab, If((age_yr>=15) AND sex=1,1,0) AS mem15ab_male, If((age_yr>=15) AND sex=2,1,0) AS mem15ab_female, \n\
			If((age_yr>=18),1,0) AS mem18ab, If((age_yr>=18) AND sex=1,1,0) AS mem18ab_male, If((age_yr>=18) AND sex=2,1,0) AS mem18ab_female, If((age_yr<=18),1,0) AS mem018, If((age_yr<=18) AND sex=1,1,0) AS mem018_male, If((age_yr<=18) AND sex=2,1,0) AS mem018_female, \n\
			If((age_yr>=17),1,0) AS mem17ab, If((age_yr>=17) AND sex=1,1,0) AS mem17ab_male, If((age_yr>=17) AND sex=2,1,0) AS mem17ab_female, If((age_yr<=17),1,0) AS mem017, If((age_yr<=17) AND sex=1,1,0) AS mem017_male, If((age_yr<=17) AND sex=2,1,0) AS mem017_female, \n\
			If((age_yr>=17 And age_yr<=21),1,0) AS mem1721, If((age_yr>=17 And age_yr<=21) AND (sex=1),1,0) AS mem1721_male, If((age_yr>=17 And age_yr<=21) AND (sex=2),1,0) AS mem1721_female, If((age_yr>=16 And age_yr<=20),1,0) AS mem1620, If((age_yr>=16 And age_yr<=20) AND (sex=1),1,0) AS mem1620_male, If((age_yr>=16 And age_yr<=20) AND (sex=2),1,0) AS mem1620_female, jobind, fjob, ynotlookjob, lastlookjob, joppind, wtwind, g_occ, sector, jstatus, \n\
			If((age_yr>=0 And age_yr<=14),1,0) AS mem014, If((age_yr>=0 And age_yr<=14) AND (sex=1),1,0) AS mem014_male, If((age_yr>=0 And age_yr<=14) AND (sex=2),1,0) AS mem014_female, If((age_yr>=15 And age_yr<=64),1,0) AS mem1564, If((age_yr>=15 And age_yr<=64) AND (sex=1),1,0) AS mem1564_male, If((age_yr>=15 And age_yr<=64) AND (sex=2),1,0) AS mem1564_female, If((age_yr>=15 And age_yr<=24),1,0) AS mem1524, If((age_yr>=15 And age_yr<=24) AND (sex=1),1,0) AS mem1524_male, If((age_yr>=15 And age_yr<=24) AND (sex=2),1,0) AS mem1524_female, \n\
			If(ylen_resid<=5, 1, 0) AS migr, If((ylen_resid<=5 AND sex=1), 1, 0) AS migr_male, If((ylen_resid<=5 AND sex=2), 1, 0) AS migr_female, If((ylen_resid<=5 AND country_resid<>608), 1, 0) AS country_migr, If((ylen_resid<=5 AND country_resid<>608 AND sex=1), 1, 0) AS country_migr_male, If((ylen_resid<=5 AND country_resid<>608 AND sex=2), 1, 0) AS country_migr_female, If((ylen_resid<=5 AND prov<>prov_resid_code), 1, 0) AS prov_migr, If((ylen_resid<=5 AND prov<>prov_resid_code AND sex=1), 1, 0) AS prov_migr_male, If((ylen_resid<=5 AND prov<>prov_resid_code AND sex=2), 1, 0) AS prov_migr_female, If((ylen_resid<=5 AND (prov=prov_resid_code AND mun<>mun_resid_code)), 1, 0) AS mun_migr, If((ylen_resid<=5 AND (prov=prov_resid_code AND mun<>mun_resid_code) AND sex=1), 1, 0) AS mun_migr_male, If((ylen_resid<=5 AND (prov=prov_resid_code AND mun<>mun_resid_code) AND sex=2), 1, 0) AS mun_migr_female, If((ylen_resid<=5 AND (prov=prov_resid_code AND mun=mun_resid_code AND brgy<>brgy_resid_code)), 1, 0) AS brgy_migr, If((ylen_resid<=5 AND (prov=prov_resid_code AND mun=mun_resid_code AND brgy<>brgy_resid_code) AND sex=1), 1, 0) AS brgy_migr_male, If((ylen_resid<=5 AND (prov=prov_resid_code AND mun=mun_resid_code AND brgy<>brgy_resid_code) AND sex=2), 1, 0) AS brgy_migr_female \n\
			FROM `hpq_mem`) as hpq_mem_temp) as mem_ind_temp;";
		}

		else if (m_qnrID==120110100 || m_qnrID==1020100100) {
		//VN01201101
		sSQL = "CREATE TABLE Mem_Ind \n\
			SELECT *, If((mem04=1 And (MNUTIND=3 OR MNUTIND=4)),1,0) AS maln04, If((mem04=1 And (MNUTIND=3 OR MNUTIND=4) and sex=1),1,0) AS maln04_male, If((mem04=1 And (MNUTIND=3 OR MNUTIND=4) and sex=2),1,0) AS maln04_female, If((maln05_mod=1 Or maln05_sev=1),1,0) AS maln05, If(((maln05_mod=1 Or maln05_sev=1) And Sex=1),1,0) AS maln05_male, If(((maln05_mod=1 Or maln05_sev=1) And Sex=2),1,0) AS maln05_female, If((maln05_mod=1 Or maln05_sev=1), 'malnourished', 'not malnourished') AS MEM_maln05, labfor-empl15ab AS unempl15ab, labfor_male-empl15ab_male AS unempl15ab_male, labfor_female-empl15ab_female AS unempl15ab_female, If(empl15ab=1, 'employed', 'unemployed') AS MEM_Unempl15ab, If((empl15ab=1),g_occ,-1) AS TypOcc, If((empl15ab=1),sector,-1) AS TypSec, If((sector<>1 AND jobind=1),1,0) AS ntAgri, If((empl15ab=1),jstatus,-1) AS JobStat, If(Elem611=1, 'attending elementary', 'not attending elementary') AS MEM_ntElem611, If(HS1215=1, 'attending high school', 'not attending high school') AS MEM_ntHS1215, If(Elem612=1, 'attending elementary', 'not attending elementary') AS MEM_ntElem612, If(HS1316=1, 'attending high school', 'not attending high school') AS MEM_ntHS1316, If(Sch615=1, 'attending school', 'not attending school') AS MEM_ntSch615, If(Sch616=1, 'attending school', 'not attending school') AS MEM_ntSch616, If(Liter10ab=1, 'literate', 'illiterate') AS MEM_ntLiter10ab \n\
			FROM (SELECT *, If(mem05=1,mnutind,-1) AS NutrStat, If((mem05=1 And MNUTIND=3),1,0) AS maln05_mod, If((mem05=1 And MNUTIND=4),1,0) AS maln05_sev, If((mem35=1 And daycare=1),1,0) AS daycare35, If(educal=17,1,0) AS educa2, If(educal>=21 AND educal<=24,1,0) AS educa3, If(educal=25,1,0) AS educa4, If((educal>=26 AND educal<=28) OR (educal>=31 AND educal<=34),1,0) AS educa5, If((educal=29) OR (educal>=35 AND educal<=37),1,0) AS educa67, IF(educind=1,1,0) AS Sch, IF(educind=1 AND sex=1,1,0) AS Sch_Male, IF(educind=1 AND sex=2,1,0) AS Sch_Female, If((mem612=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem612, If((mem612_male=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem612_male, If((mem612_female=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem612_female, \n\
			If((mem611=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem611, If((mem611_male=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem611_male, If((mem611_female=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem611_female, \n\
			If((mem1316=1 And educind=1 And gradel>=21 And gradel<=24),1,0) AS hs1316, If((mem1316_male=1 And educind=1 And gradel>=21 And gradel<=24),1,0) AS hs1316_male, If((mem1316_female=1 And educind=1 And gradel>=21 And gradel<=24),1,0) AS hs1316_female, \n\
			If((mem1215=1 And educind=1 And gradel>=21 And gradel<=24),1,0) AS hs1215, If((mem1215_male=1 And educind=1 And gradel>=21 And gradel<=24),1,0) AS hs1215_male, If((mem1215_female=1 And educind=1 And gradel>=21 And gradel<=24),1,0) AS hs1215_female, \n\
			If((mem616=1 And educind=1),1,0) AS sch616, If((mem616_male=1 And educind=1),1,0) AS sch616_male, If((mem616_female=1 And educind=1),1,0) AS sch616_female, If((mem615=1 And educind=1),1,0) AS sch615, If((mem615_male=1 And educind=1),1,0) AS sch615_male, If((mem615_female=1 And educind=1),1,0) AS sch615_female, If((mem1620=1 And educind=1 And gradel>=31 And gradel<=34),1,0) AS tert1620, If((mem1620_male=1 And educind=1 And gradel>=31 And gradel<=34),1,0) AS tert1620_male, If((mem1620_female=1 And educind=1 And gradel>=31 And gradel<=34),1,0) AS tert1620_female, If((mem1721=1 And educind=1 And gradel>=31),1,0) AS tert1721, If((mem1721_male=1 And educind=1 And gradel>=31),1,0) AS tert1721_male, If((mem1721_female=1 And educind=1 And gradel>=31),1,0) AS tert1721_female, If((mem10ab=1 And literind=1),1,0) AS liter10ab, If((mem10ab_male=1 And literind=1),1,0) AS liter10ab_male, If((mem10ab_female=1 And literind=1),1,0) AS liter10ab_female, If((mem1524=1 And literind=1),1,0) AS liter1524, If((mem1524_male=1 And literind=1),1,0) AS liter1524_male, If((mem1524_female=1 And literind=1),1,0) AS liter1524_female, \n\
			if((mem612=1 and educind=2) or (mem612=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem612, if((mem612_male=1 and educind=2) or (mem612_male=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem612_male, if((mem612_female=1 and educind=2) or (mem612_female=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem612_female, if((mem611=1 and educind=2) or (mem611=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem611, if((mem611_male=1 and educind=2) or (mem611_male=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem611_male, if((mem611_female=1 and educind=2) or (mem611_female=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem611_female, if((mem1316=1 and educind=2) or (mem1316=1 and educind=1 and (gradel<21 or gradel>24)),1,0) as nths1316, if((mem1316_male=1 and educind=2) or (mem1316_male=1 and educind=1 and (gradel<21 or gradel>24)),1,0) as nths1316_male, if((mem1316_female=1 and educind=2) or (mem1316_female=1 and educind=1 and (gradel<21 or gradel>24)),1,0) as nths1316_female, if((mem1215=1 and educind=2) or (mem1215=1 and educind=1 and (gradel<21 or gradel>24)),1,0) as nths1215, if((mem1215_male=1 and educind=2) or (mem1215_male=1 and educind=1 and (gradel<21 or gradel>24)),1,0) as nths1215_male, if((mem1215_female=1 and educind=2) or (mem1215_female=1 and educind=1 and (gradel<21 or gradel>24)),1,0) as nths1215_female, if((mem616=1 and educind=2),1,0) as ntsch616, if((mem616_male=1 and educind=2),1,0) as ntsch616_male, if((mem616_female=1 and educind=2),1,0) as ntsch616_female, if((mem615=1 and educind=2),1,0) as ntsch615, if((mem615_male=1 and educind=2),1,0) as ntsch615_male, if((mem615_female=1 and educind=2),1,0) as ntsch615_female, if((mem10ab=1 and literind=2),1,0) as ntliter10ab, if((mem10ab_male=1 and literind=2),1,0) as ntliter10ab_male, if((mem10ab_female=1 and literind=2),1,0) as ntliter10ab_female, \n\
			IF(jobind=2,1,0) AS wnojob, IF(jobind=2 And sex=1,1,0) AS wnojob_male, IF(jobind=2 And sex=2,1,0) AS wnojob_female, If(((mem15ab=1 And jobind=1) Or (mem15ab=1 And jobind=2 And fjob=1) Or (mem15ab=1 And jobind=2 And fjob=2 And ynotlookjob>=2 And ynotlookjob<=5 And joppind=1 And wtwind=1) Or (mem15ab=1 And jobind=2 And fjob=2 And ynotlookjob=1 And lastlookjob>=1 And lastlookjob<=2 And joppind=1 And wtwind=1)),1,0) AS labfor, \n\
			If(((mem15ab_male=1 And jobind=1) Or (mem15ab_male=1 And jobind=2 And fjob=1) Or (mem15ab_male=1 And jobind=2 And fjob=2 And ynotlookjob>=2 And ynotlookjob<=5 And joppind=1 And wtwind=1) Or (mem15ab_male=1 And jobind=2 And fjob=2 And ynotlookjob=1 And lastlookjob>=1 And lastlookjob<=2 And joppind=1 And wtwind=1)),1,0) AS labfor_male, If(((mem15ab_female=1 And jobind=1) Or (mem15ab_female=1 And jobind=2 And fjob=1) Or (mem15ab_female=1 And jobind=2 And fjob=2 And ynotlookjob>=2 And ynotlookjob<=5 And joppind=1 And wtwind=1) Or (mem15ab_female=1 And jobind=2 And fjob=2 And ynotlookjob=1 And lastlookjob>=1 And lastlookjob<=2 And joppind=1 And wtwind=1)),1,0) AS labfor_female, If((mem15ab=1 And jobind=1),1,0) AS empl15ab, If((mem15ab_male=1 And jobind=1),1,0) AS empl15ab_male, If((mem15ab_female=1 And jobind=1),1,0) AS empl15ab_female, If((mem15ab<>1 And jobind=1),1,0) AS empl014, If(mem15ab=1 And jobind=1 And fxtra_wrk=1, 1, 0) AS underempl15ab_wrk, If(mem15ab_female=1 And jobind=1 And fxtra_wrk=1, 1, 0) AS underempl15ab_wrk_female, If(mem15ab_male=1 And jobind=1 And fxtra_wrk=1, 1, 0) AS underempl15ab_wrk_male, If(mem15ab=1 And jobind=1 And fadd_work_hrs=1, 1, 0) AS underempl15ab_hrs, If(mem15ab_female=1 And jobind=1 And fadd_work_hrs=1, 1, 0) AS underempl15ab_hrs_female, If(mem15ab_male=1 And jobind=1 And fadd_work_hrs=1, 1, 0) AS underempl15ab_hrs_male, If(mem15ab=1 And jobind=1 And (fxtra_wrk=1 OR fadd_work_hrs=1), 1, 0) AS underempl15ab, If(mem15ab_female=1 And jobind=1 And (fxtra_wrk=1 OR fadd_work_hrs=1), 1, 0) AS underempl15ab_female, If(mem15ab_male=1 And jobind=1 And (fxtra_wrk=1 OR fadd_work_hrs=1), 1, 0) AS underempl15ab_male \n\
			FROM (SELECT hpq_mem.urb, hpq_mem.regn, hpq_mem.prov, hpq_mem.mun, hpq_mem.brgy, hpq_mem.purok, hpq_mem.hcn, hpq_mem.memno, CONCAT(regn, prov, mun, brgy, purok, hcn, memno) AS `memID`, \n\
			reln, SEX, age_yr, civstat, relgn, educal, pwd_ind, pwd_type, fadd_work_hrs, fxtra_wrk, if(sex=1,1,0) as Male,  if(sex=2,1,0) as Female,  if(reln<>8,1,0) as nthhwrkr, IF(ipind=1,1,0) AS ipind, IF(ipind=1 And sex=1,1,0) AS ipind_male, IF(ipind=1 And sex=2,1,0) AS ipind_female, If((ipind=1),IPGRP,-1) AS IPgrp, If((age_yr>=60),1,0) AS mem60ab, If((age_yr>=60) AND sex=1,1,0) AS mem60ab_male, If((age_yr>=60) AND sex=2,1,0) AS mem60ab_female, If((age_yr>=65),1,0) AS mem65ab, If((age_yr>=65) AND sex=1,1,0) AS mem65ab_male, If((age_yr>=65) AND sex=2,1,0) AS mem65ab_female, If(age_yr=0,1,0) AS mem01, If((age_yr=0) and (sex=1),1,0) AS mem01_male, If((age_yr=0) and (sex=2),1,0) AS mem01_female, If((age_yr<=4),1,0) AS mem04, If((age_yr<=4) and (sex=1),1,0) AS mem04_male, If((age_yr<=4) and (sex=2),1,0) AS mem04_female, If((age_yr<=4 AND age_yr>=1),1,0) AS mem14, If((age_yr<=4 AND age_yr>=1) and (sex=1),1,0) AS mem14_male, If((age_yr<=4 AND age_yr>=1) and (sex=2),1,0) AS mem14_female, If((age_yr<=5),1,0) AS mem05, If((age_yr<=5) and (sex=1),1,0) AS mem05_male, If((age_yr<=5) and (sex=2),1,0) AS mem05_female, mnutind, If((age_yr>=3 And age_yr<=5),1,0) AS mem35, educind, gradel, If((educind=1 And gradel=1),1,0) AS daycare, If((educind=1 And gradel=2),1,0) AS kinprep, If((age_yr>=6 And age_yr<=12),1,0) AS mem612, If((age_yr>=6 And age_yr<=12) AND (sex=1),1,0) AS mem612_male, If((age_yr>=6 And age_yr<=12) AND (sex=2),1,0) AS mem612_female, If((age_yr>=6 And age_yr<=11),1,0) AS mem611, If((age_yr>=6 And age_yr<=11) AND (sex=1),1,0) AS mem611_male, If((age_yr>=6 And age_yr<=11) AND (sex=2),1,0) AS mem611_female, If((age_yr>=13 And age_yr<=16),1,0) AS mem1316, If((age_yr>=13 And age_yr<=16) AND (sex=1),1,0) AS mem1316_male, If((age_yr>=13 And age_yr<=16) AND (sex=2),1,0) AS mem1316_female, If((age_yr>=12 And age_yr<=15),1,0) AS mem1215, If((age_yr>=12 And age_yr<=15) AND (sex=1),1,0) AS mem1215_male, If((age_yr>=12 And age_yr<=15) AND (sex=2),1,0) AS mem1215_female, If((age_yr>=6 And age_yr<=16),1,0) AS mem616, If((age_yr>=6 And age_yr<=16) AND (sex=1),1,0) AS mem616_male, If((age_yr>=6 And age_yr<=16) AND (sex=2),1,0) AS mem616_female, If((age_yr>=6 And age_yr<=15),1,0) AS mem615, If((age_yr>=6 And age_yr<=15) AND (sex=1),1,0) AS mem615_male, If((age_yr>=6 And age_yr<=15) AND (sex=2),1,0) AS mem615_female, literind, If((age_yr>=10),1,0) AS mem10ab, If((age_yr>=10) AND sex=1,1,0) AS mem10ab_male, If((age_yr>=10) AND sex=2,1,0) AS mem10ab_female, If((age_yr>=15),1,0) AS mem15ab, If((age_yr>=15) AND sex=1,1,0) AS mem15ab_male, If((age_yr>=15) AND sex=2,1,0) AS mem15ab_female, \n\
			If((age_yr>=18),1,0) AS mem18ab, If((age_yr>=18) AND sex=1,1,0) AS mem18ab_male, If((age_yr>=18) AND sex=2,1,0) AS mem18ab_female, If((age_yr<=18),1,0) AS mem018, If((age_yr<=18) AND sex=1,1,0) AS mem018_male, If((age_yr<=18) AND sex=2,1,0) AS mem018_female, \n\
			If((age_yr>=17),1,0) AS mem17ab, If((age_yr>=17) AND sex=1,1,0) AS mem17ab_male, If((age_yr>=17) AND sex=2,1,0) AS mem17ab_female, If((age_yr<=17),1,0) AS mem017, If((age_yr<=17) AND sex=1,1,0) AS mem017_male, If((age_yr<=17) AND sex=2,1,0) AS mem017_female, \n\
			If((age_yr>=17 And age_yr<=21),1,0) AS mem1721, If((age_yr>=17 And age_yr<=21) AND (sex=1),1,0) AS mem1721_male, If((age_yr>=17 And age_yr<=21) AND (sex=2),1,0) AS mem1721_female, If((age_yr>=16 And age_yr<=20),1,0) AS mem1620, If((age_yr>=16 And age_yr<=20) AND (sex=1),1,0) AS mem1620_male, If((age_yr>=16 And age_yr<=20) AND (sex=2),1,0) AS mem1620_female, jobind, fjob, ynotlookjob, lastlookjob, joppind, wtwind, g_occ, sector, jstatus, \n\
			If((age_yr>=0 And age_yr<=14),1,0) AS mem014, If((age_yr>=0 And age_yr<=14) AND (sex=1),1,0) AS mem014_male, If((age_yr>=0 And age_yr<=14) AND (sex=2),1,0) AS mem014_female, If((age_yr>=15 And age_yr<=64),1,0) AS mem1564, If((age_yr>=15 And age_yr<=64) AND (sex=1),1,0) AS mem1564_male, If((age_yr>=15 And age_yr<=64) AND (sex=2),1,0) AS mem1564_female, If((age_yr>=15 And age_yr<=24),1,0) AS mem1524, If((age_yr>=15 And age_yr<=24) AND (sex=1),1,0) AS mem1524_male, If((age_yr>=15 And age_yr<=24) AND (sex=2),1,0) AS mem1524_female, \n\
			If(ylen_resid<=5, 1, 0) AS migr, If((ylen_resid<=5 AND sex=1), 1, 0) AS migr_male, If((ylen_resid<=5 AND sex=2), 1, 0) AS migr_female, If((ylen_resid<=5 AND country_resid<>608), 1, 0) AS country_migr, If((ylen_resid<=5 AND country_resid<>608 AND sex=1), 1, 0) AS country_migr_male, If((ylen_resid<=5 AND country_resid<>608 AND sex=2), 1, 0) AS country_migr_female, If((ylen_resid<=5 AND prov<>prov_resid_code), 1, 0) AS prov_migr, If((ylen_resid<=5 AND prov<>prov_resid_code AND sex=1), 1, 0) AS prov_migr_male, If((ylen_resid<=5 AND prov<>prov_resid_code AND sex=2), 1, 0) AS prov_migr_female, If((ylen_resid<=5 AND (prov=prov_resid_code AND mun<>mun_resid_code)), 1, 0) AS mun_migr, If((ylen_resid<=5 AND (prov=prov_resid_code AND mun<>mun_resid_code) AND sex=1), 1, 0) AS mun_migr_male, If((ylen_resid<=5 AND (prov=prov_resid_code AND mun<>mun_resid_code) AND sex=2), 1, 0) AS mun_migr_female, If((ylen_resid<=5 AND (prov=prov_resid_code AND mun=mun_resid_code AND brgy<>brgy_resid_code)), 1, 0) AS brgy_migr, If((ylen_resid<=5 AND (prov=prov_resid_code AND mun=mun_resid_code AND brgy<>brgy_resid_code) AND sex=1), 1, 0) AS brgy_migr_male, If((ylen_resid<=5 AND (prov=prov_resid_code AND mun=mun_resid_code AND brgy<>brgy_resid_code) AND sex=2), 1, 0) AS brgy_migr_female \n\
			FROM `hpq_mem` WHERE (ofw=2 or isnull(ofw)=TRUE)) as hpq_mem_temp) as mem_ind_temp;";
		}
		else if (m_qnrID==720130100) {
		sSQL = "CREATE TABLE Mem_Ind \n\
			SELECT *, If((mem04=1 And (MNUTIND=3 OR MNUTIND=4)),1,0) AS maln04, If((mem04=1 And (MNUTIND=3 OR MNUTIND=4) and sex=1),1,0) AS maln04_male, If((mem04=1 And (MNUTIND=3 OR MNUTIND=4) and sex=2),1,0) AS maln04_female, If((maln05_mod=1 Or maln05_sev=1),1,0) AS maln05, If(((maln05_mod=1 Or maln05_sev=1) And Sex=1),1,0) AS maln05_male, If(((maln05_mod=1 Or maln05_sev=1) And Sex=2),1,0) AS maln05_female, If((maln05_mod=1 Or maln05_sev=1), 'malnourished', 'not malnourished') AS MEM_maln05, labfor-empl15ab AS unempl15ab, labfor_male-empl15ab_male AS unempl15ab_male, labfor_female-empl15ab_female AS unempl15ab_female, If(empl15ab=1, 'employed', 'unemployed') AS MEM_Unempl15ab, If((empl15ab=1),g_occ,-1) AS TypOcc, If((empl15ab=1),sector,-1) AS TypSec, If((sector<>1 AND jobind=1),1,0) AS ntAgri, If((empl15ab=1),jstatus,-1) AS JobStat, If(Elem611=1, 'attending elementary', 'not attending elementary') AS MEM_ntElem611, If(HS1215=1, 'attending high school', 'not attending high school') AS MEM_ntHS1215, If(Elem612=1, 'attending elementary', 'not attending elementary') AS MEM_ntElem612, If(HS1316=1, 'attending high school', 'not attending high school') AS MEM_ntHS1316, If(Sch615=1, 'attending school', 'not attending school') AS MEM_ntSch615, If(Sch616=1, 'attending school', 'not attending school') AS MEM_ntSch616, If(Liter10ab=1, 'literate', 'illiterate') AS MEM_ntLiter10ab \n\
			FROM (SELECT *, If(mem05=1,mnutind,-1) AS NutrStat, If((mem05=1 And MNUTIND=3),1,0) AS maln05_mod, If((mem05=1 And MNUTIND=4),1,0) AS maln05_sev, If((mem35=1 And daycare=1),1,0) AS daycare35, If(educal=17,1,0) AS educa2, If(educal>=21 AND educal<=24,1,0) AS educa3, If(educal=25,1,0) AS educa4, If((educal>=26 AND educal<=28) OR (educal>=31 AND educal<=34),1,0) AS educa5, If((educal=29) OR (educal>=35 AND educal<=37),1,0) AS educa67, IF(educind=1,1,0) AS Sch, IF(educind=1 AND sex=1,1,0) AS Sch_Male, IF(educind=1 AND sex=2,1,0) AS Sch_Female, If((mem612=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem612, If((mem612_male=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem612_male, If((mem612_female=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem612_female, \n\
			If((mem611=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem611, If((mem611_male=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem611_male, If((mem611_female=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem611_female, \n\
			If((mem1316=1 And educind=1 And gradel>=21 And gradel<=24),1,0) AS hs1316, If((mem1316_male=1 And educind=1 And gradel>=21 And gradel<=24),1,0) AS hs1316_male, If((mem1316_female=1 And educind=1 And gradel>=21 And gradel<=24),1,0) AS hs1316_female, \n\
			If((mem1215=1 And educind=1 And gradel>=21 And gradel<=24),1,0) AS hs1215, If((mem1215_male=1 And educind=1 And gradel>=21 And gradel<=24),1,0) AS hs1215_male, If((mem1215_female=1 And educind=1 And gradel>=21 And gradel<=24),1,0) AS hs1215_female, \n\
			If((mem616=1 And educind=1),1,0) AS sch616, If((mem616_male=1 And educind=1),1,0) AS sch616_male, If((mem616_female=1 And educind=1),1,0) AS sch616_female, If((mem615=1 And educind=1),1,0) AS sch615, If((mem615_male=1 And educind=1),1,0) AS sch615_male, If((mem615_female=1 And educind=1),1,0) AS sch615_female, If((mem1620=1 And educind=1 And gradel>=31 And gradel<=34),1,0) AS tert1620, If((mem1620_male=1 And educind=1 And gradel>=31 And gradel<=34),1,0) AS tert1620_male, If((mem1620_female=1 And educind=1 And gradel>=31 And gradel<=34),1,0) AS tert1620_female, If((mem1721=1 And educind=1 And gradel>=31),1,0) AS tert1721, If((mem1721_male=1 And educind=1 And gradel>=31),1,0) AS tert1721_male, If((mem1721_female=1 And educind=1 And gradel>=31),1,0) AS tert1721_female, If((mem10ab=1 And literind=1),1,0) AS liter10ab, If((mem10ab_male=1 And literind=1),1,0) AS liter10ab_male, If((mem10ab_female=1 And literind=1),1,0) AS liter10ab_female, If((mem1524=1 And literind=1),1,0) AS liter1524, If((mem1524_male=1 And literind=1),1,0) AS liter1524_male, If((mem1524_female=1 And literind=1),1,0) AS liter1524_female, \n\
			if((mem612=1 and educind=2) or (mem612=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem612, if((mem612_male=1 and educind=2) or (mem612_male=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem612_male, if((mem612_female=1 and educind=2) or (mem612_female=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem612_female, if((mem611=1 and educind=2) or (mem611=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem611, if((mem611_male=1 and educind=2) or (mem611_male=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem611_male, if((mem611_female=1 and educind=2) or (mem611_female=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem611_female, if((mem1316=1 and educind=2) or (mem1316=1 and educind=1 and (gradel<21 or gradel>24)),1,0) as nths1316, if((mem1316_male=1 and educind=2) or (mem1316_male=1 and educind=1 and (gradel<21 or gradel>24)),1,0) as nths1316_male, if((mem1316_female=1 and educind=2) or (mem1316_female=1 and educind=1 and (gradel<21 or gradel>24)),1,0) as nths1316_female, if((mem1215=1 and educind=2) or (mem1215=1 and educind=1 and (gradel<21 or gradel>24)),1,0) as nths1215, if((mem1215_male=1 and educind=2) or (mem1215_male=1 and educind=1 and (gradel<21 or gradel>24)),1,0) as nths1215_male, if((mem1215_female=1 and educind=2) or (mem1215_female=1 and educind=1 and (gradel<21 or gradel>24)),1,0) as nths1215_female, if((mem616=1 and educind=2),1,0) as ntsch616, if((mem616_male=1 and educind=2),1,0) as ntsch616_male, if((mem616_female=1 and educind=2),1,0) as ntsch616_female, if((mem615=1 and educind=2),1,0) as ntsch615, if((mem615_male=1 and educind=2),1,0) as ntsch615_male, if((mem615_female=1 and educind=2),1,0) as ntsch615_female, if((mem10ab=1 and literind=2),1,0) as ntliter10ab, if((mem10ab_male=1 and literind=2),1,0) as ntliter10ab_male, if((mem10ab_female=1 and literind=2),1,0) as ntliter10ab_female, \n\
			IF(jobind=2,1,0) AS wnojob, IF(jobind=2 And sex=1,1,0) AS wnojob_male, IF(jobind=2 And sex=2,1,0) AS wnojob_female, IF(mcrimeind=1,1,0) AS mvictcr, IF(mcrimeind=1 And sex=1,1,0) AS mvictcr_male, IF(mcrimeind=1 And sex=2,1,0) AS mvictcr_female, If(((mem15ab=1 And jobind=1) Or (mem15ab=1 And jobind=2 And fjob=1) Or (mem15ab=1 And jobind=2 And fjob=2 And ynotlookjob>=2 And ynotlookjob<=5 And joppind=1 And wtwind=1) Or (mem15ab=1 And jobind=2 And fjob=2 And ynotlookjob=1 And lastlookjob>=1 And lastlookjob<=2 And joppind=1 And wtwind=1)),1,0) AS labfor, \n\
			If(((mem15ab_male=1 And jobind=1) Or (mem15ab_male=1 And jobind=2 And fjob=1) Or (mem15ab_male=1 And jobind=2 And fjob=2 And ynotlookjob>=2 And ynotlookjob<=5 And joppind=1 And wtwind=1) Or (mem15ab_male=1 And jobind=2 And fjob=2 And ynotlookjob=1 And lastlookjob>=1 And lastlookjob<=2 And joppind=1 And wtwind=1)),1,0) AS labfor_male, If(((mem15ab_female=1 And jobind=1) Or (mem15ab_female=1 And jobind=2 And fjob=1) Or (mem15ab_female=1 And jobind=2 And fjob=2 And ynotlookjob>=2 And ynotlookjob<=5 And joppind=1 And wtwind=1) Or (mem15ab_female=1 And jobind=2 And fjob=2 And ynotlookjob=1 And lastlookjob>=1 And lastlookjob<=2 And joppind=1 And wtwind=1)),1,0) AS labfor_female, If((mem15ab=1 And jobind=1),1,0) AS empl15ab, If((mem15ab_male=1 And jobind=1),1,0) AS empl15ab_male, If((mem15ab_female=1 And jobind=1),1,0) AS empl15ab_female, If((mem15ab<>1 And jobind=1),1,0) AS empl014, If(mem15ab=1 And jobind=1 And fxtra_wrk=1, 1, 0) AS underempl15ab_wrk, If(mem15ab_female=1 And jobind=1 And fxtra_wrk=1, 1, 0) AS underempl15ab_wrk_female, If(mem15ab_male=1 And jobind=1 And fxtra_wrk=1, 1, 0) AS underempl15ab_wrk_male, If(mem15ab=1 And jobind=1 And fadd_work_hrs=1, 1, 0) AS underempl15ab_hrs, If(mem15ab_female=1 And jobind=1 And fadd_work_hrs=1, 1, 0) AS underempl15ab_hrs_female, If(mem15ab_male=1 And jobind=1 And fadd_work_hrs=1, 1, 0) AS underempl15ab_hrs_male, If(mem15ab=1 And jobind=1 And (fxtra_wrk=1 OR fadd_work_hrs=1), 1, 0) AS underempl15ab, If(mem15ab_female=1 And jobind=1 And (fxtra_wrk=1 OR fadd_work_hrs=1), 1, 0) AS underempl15ab_female, If(mem15ab_male=1 And jobind=1 And (fxtra_wrk=1 OR fadd_work_hrs=1), 1, 0) AS underempl15ab_male \n\
			FROM (SELECT hpq_mem.urb, hpq_mem.regn, hpq_mem.prov, hpq_mem.mun, hpq_mem.brgy, hpq_mem.purok, hpq_mem.hcn, hpq_mem.memno, CONCAT(regn, prov, mun, brgy, purok, hcn, memno) AS `memID`, \n\
			reln, SEX, age_yr, civstat, educal, pwd_ind, pwd_type, fadd_work_hrs, fxtra_wrk, mcrimeind, if(sex=1,1,0) as Male,  if(sex=2,1,0) as Female,  if(reln<>8,1,0) as nthhwrkr, If((age_yr>=60),1,0) AS mem60ab, If((age_yr>=60) AND sex=1,1,0) AS mem60ab_male, If((age_yr>=60) AND sex=2,1,0) AS mem60ab_female, If((age_yr>=65),1,0) AS mem65ab, If((age_yr>=65) AND sex=1,1,0) AS mem65ab_male, If((age_yr>=65) AND sex=2,1,0) AS mem65ab_female, If(age_yr=0,1,0) AS mem01, If((age_yr=0) and (sex=1),1,0) AS mem01_male, If((age_yr=0) and (sex=2),1,0) AS mem01_female, If((age_yr<=4),1,0) AS mem04, If((age_yr<=4) and (sex=1),1,0) AS mem04_male, If((age_yr<=4) and (sex=2),1,0) AS mem04_female, If((age_yr<=4 AND age_yr>=1),1,0) AS mem14, If((age_yr<=4 AND age_yr>=1) and (sex=1),1,0) AS mem14_male, If((age_yr<=4 AND age_yr>=1) and (sex=2),1,0) AS mem14_female, If((age_yr<=5),1,0) AS mem05, If((age_yr<=5) and (sex=1),1,0) AS mem05_male, If((age_yr<=5) and (sex=2),1,0) AS mem05_female, mnutind, If((age_yr>=3 And age_yr<=5),1,0) AS mem35, educind, gradel, If((educind=1 And gradel=1),1,0) AS daycare, If((educind=1 And gradel=2),1,0) AS kinprep, If((age_yr>=6 And age_yr<=12),1,0) AS mem612, If((age_yr>=6 And age_yr<=12) AND (sex=1),1,0) AS mem612_male, If((age_yr>=6 And age_yr<=12) AND (sex=2),1,0) AS mem612_female, If((age_yr>=6 And age_yr<=11),1,0) AS mem611, If((age_yr>=6 And age_yr<=11) AND (sex=1),1,0) AS mem611_male, If((age_yr>=6 And age_yr<=11) AND (sex=2),1,0) AS mem611_female, If((age_yr>=13 And age_yr<=16),1,0) AS mem1316, If((age_yr>=13 And age_yr<=16) AND (sex=1),1,0) AS mem1316_male, If((age_yr>=13 And age_yr<=16) AND (sex=2),1,0) AS mem1316_female, If((age_yr>=12 And age_yr<=15),1,0) AS mem1215, If((age_yr>=12 And age_yr<=15) AND (sex=1),1,0) AS mem1215_male, If((age_yr>=12 And age_yr<=15) AND (sex=2),1,0) AS mem1215_female, If((age_yr>=6 And age_yr<=16),1,0) AS mem616, If((age_yr>=6 And age_yr<=16) AND (sex=1),1,0) AS mem616_male, If((age_yr>=6 And age_yr<=16) AND (sex=2),1,0) AS mem616_female, If((age_yr>=6 And age_yr<=15),1,0) AS mem615, If((age_yr>=6 And age_yr<=15) AND (sex=1),1,0) AS mem615_male, If((age_yr>=6 And age_yr<=15) AND (sex=2),1,0) AS mem615_female, literind, If((age_yr>=10),1,0) AS mem10ab, If((age_yr>=10) AND sex=1,1,0) AS mem10ab_male, If((age_yr>=10) AND sex=2,1,0) AS mem10ab_female, If((age_yr>=15),1,0) AS mem15ab, If((age_yr>=15) AND sex=1,1,0) AS mem15ab_male, If((age_yr>=15) AND sex=2,1,0) AS mem15ab_female, \n\
			If((age_yr>=18),1,0) AS mem18ab, If((age_yr>=18) AND sex=1,1,0) AS mem18ab_male, If((age_yr>=18) AND sex=2,1,0) AS mem18ab_female, If((age_yr<=18),1,0) AS mem018, If((age_yr<=18) AND sex=1,1,0) AS mem018_male, If((age_yr<=18) AND sex=2,1,0) AS mem018_female, \n\
			If((age_yr>=17),1,0) AS mem17ab, If((age_yr>=17) AND sex=1,1,0) AS mem17ab_male, If((age_yr>=17) AND sex=2,1,0) AS mem17ab_female, If((age_yr<=17),1,0) AS mem017, If((age_yr<=17) AND sex=1,1,0) AS mem017_male, If((age_yr<=17) AND sex=2,1,0) AS mem017_female, \n\
			If((age_yr>=17 And age_yr<=21),1,0) AS mem1721, If((age_yr>=17 And age_yr<=21) AND (sex=1),1,0) AS mem1721_male, If((age_yr>=17 And age_yr<=21) AND (sex=2),1,0) AS mem1721_female, If((age_yr>=16 And age_yr<=20),1,0) AS mem1620, If((age_yr>=16 And age_yr<=20) AND (sex=1),1,0) AS mem1620_male, If((age_yr>=16 And age_yr<=20) AND (sex=2),1,0) AS mem1620_female, jobind, fjob, ynotlookjob, lastlookjob, joppind, wtwind, g_occ, sector, jstatus, \n\
			If((age_yr>=0 And age_yr<=14),1,0) AS mem014, If((age_yr>=0 And age_yr<=14) AND (sex=1),1,0) AS mem014_male, If((age_yr>=0 And age_yr<=14) AND (sex=2),1,0) AS mem014_female, If((age_yr>=15 And age_yr<=64),1,0) AS mem1564, If((age_yr>=15 And age_yr<=64) AND (sex=1),1,0) AS mem1564_male, If((age_yr>=15 And age_yr<=64) AND (sex=2),1,0) AS mem1564_female, If((age_yr>=15 And age_yr<=24),1,0) AS mem1524, If((age_yr>=15 And age_yr<=24) AND (sex=1),1,0) AS mem1524_male, If((age_yr>=15 And age_yr<=24) AND (sex=2),1,0) AS mem1524_female \n\
			FROM `hpq_mem` WHERE (ofw=2 or ofw=0 or isnull(ofw)=TRUE)) as hpq_mem_temp) as mem_ind_temp;";
		}
		else if (m_qnrID==1020130100 || m_qnrID==1020130101 || m_qnrID==1020135100 || m_qnrID==1020135200 || m_qnrID==1020135300 || m_qnrID==1020135400 || m_qnrID==1020135500) {
		sSQL = "CREATE TABLE Mem_Ind \n\
			SELECT *, If((mem04=1 And (MNUTIND=3 OR MNUTIND=4)),1,0) AS maln04, If((mem04=1 And (MNUTIND=3 OR MNUTIND=4) and sex=1),1,0) AS maln04_male, If((mem04=1 And (MNUTIND=3 OR MNUTIND=4) and sex=2),1,0) AS maln04_female, If((maln05_mod=1 Or maln05_sev=1),1,0) AS maln05, If(((maln05_mod=1 Or maln05_sev=1) And Sex=1),1,0) AS maln05_male, If(((maln05_mod=1 Or maln05_sev=1) And Sex=2),1,0) AS maln05_female, If((maln05_mod=1 Or maln05_sev=1), 'malnourished', 'not malnourished') AS MEM_maln05, labfor-empl15ab AS unempl15ab, labfor_male-empl15ab_male AS unempl15ab_male, labfor_female-empl15ab_female AS unempl15ab_female, If(empl15ab=1, 'employed', 'unemployed') AS MEM_Unempl15ab, If((empl15ab=1),g_occ,-1) AS TypOcc, If((empl15ab=1),sector,-1) AS TypSec, If((sector<>1 AND (jobind=1 or entrepind=1)),1,0) AS ntAgri, If((empl15ab=1),jstatus,-1) AS JobStat, If(Elem611=1, 'attending elementary', 'not attending elementary') AS MEM_ntElem611, If(HS1215=1, 'attending high school', 'not attending high school') AS MEM_ntHS1215, If(Elem612=1, 'attending elementary', 'not attending elementary') AS MEM_ntElem612, If(HS1316=1, 'attending high school', 'not attending high school') AS MEM_ntHS1316, If(Sch615=1, 'attending school', 'not attending school') AS MEM_ntSch615, If(Sch616=1, 'attending school', 'not attending school') AS MEM_ntSch616, If(Liter10ab=1, 'literate', 'illiterate') AS MEM_ntLiter10ab \n\
			FROM (SELECT *, If(mem05=1,mnutind,-1) AS NutrStat, If((mem05=1 And MNUTIND=3),1,0) AS maln05_mod, If((mem05=1 And MNUTIND=4),1,0) AS maln05_sev, If((mem35=1 And daycare=1),1,0) AS daycare35, If(educal=17,1,0) AS educa2, If(educal>=21 AND educal<=24,1,0) AS educa3, If(educal=25,1,0) AS educa4, If((educal>=26 AND educal<=28) OR (educal>=31 AND educal<=34),1,0) AS educa5, If((educal=29) OR (educal>=35 AND educal<=37),1,0) AS educa67, IF(educind=1,1,0) AS Sch, IF(educind=1 AND sex=1,1,0) AS Sch_Male, IF(educind=1 AND sex=2,1,0) AS Sch_Female, If((mem612=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem612, If((mem612_male=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem612_male, If((mem612_female=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem612_female, \n\
			If((mem611=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem611, If((mem611_male=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem611_male, If((mem611_female=1 And educind=1 And gradel>=11 And gradel<=16),1,0) AS elem611_female, \n\
			If((mem1316=1 And educind=1 And gradel>=17 And gradel<=22),1,0) AS hs1316, If((mem1316_male=1 And educind=1 And gradel>=17 And gradel<=22),1,0) AS hs1316_male, If((mem1316_female=1 And educind=1 And gradel>=17 And gradel<=22),1,0) AS hs1316_female, \n\
			If((mem1215=1 And educind=1 And gradel>=17 And gradel<=22),1,0) AS hs1215, If((mem1215_male=1 And educind=1 And gradel>=17 And gradel<=22),1,0) AS hs1215_male, If((mem1215_female=1 And educind=1 And gradel>=17 And gradel<=22),1,0) AS hs1215_female, \n\
			If((mem616=1 And educind=1),1,0) AS sch616, If((mem616_male=1 And educind=1),1,0) AS sch616_male, If((mem616_female=1 And educind=1),1,0) AS sch616_female, If((mem615=1 And educind=1),1,0) AS sch615, If((mem615_male=1 And educind=1),1,0) AS sch615_male, If((mem615_female=1 And educind=1),1,0) AS sch615_female, If((mem1620=1 And educind=1 And gradel>=31 And gradel<=34),1,0) AS tert1620, If((mem1620_male=1 And educind=1 And gradel>=31 And gradel<=34),1,0) AS tert1620_male, If((mem1620_female=1 And educind=1 And gradel>=31 And gradel<=34),1,0) AS tert1620_female, If((mem1721=1 And educind=1 And gradel>=31),1,0) AS tert1721, If((mem1721_male=1 And educind=1 And gradel>=31),1,0) AS tert1721_male, If((mem1721_female=1 And educind=1 And gradel>=31),1,0) AS tert1721_female, If((mem10ab=1 And literind=1),1,0) AS liter10ab, If((mem10ab_male=1 And literind=1),1,0) AS liter10ab_male, If((mem10ab_female=1 And literind=1),1,0) AS liter10ab_female, If((mem1524=1 And literind=1),1,0) AS liter1524, If((mem1524_male=1 And literind=1),1,0) AS liter1524_male, If((mem1524_female=1 And literind=1),1,0) AS liter1524_female, \n\
			if((mem612=1 and educind=2) or (mem612=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem612, if((mem612_male=1 and educind=2) or (mem612_male=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem612_male, if((mem612_female=1 and educind=2) or (mem612_female=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem612_female, if((mem611=1 and educind=2) or (mem611=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem611, if((mem611_male=1 and educind=2) or (mem611_male=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem611_male, if((mem611_female=1 and educind=2) or (mem611_female=1 and educind=1 and (gradel<11 or gradel>16)),1,0) as ntelem611_female, if((mem1316=1 and educind=2) or (mem1316=1 and educind=1 and (gradel<17 or gradel>22)),1,0) as nths1316, if((mem1316_male=1 and educind=2) or (mem1316_male=1 and educind=1 and (gradel<17 or gradel>22)),1,0) as nths1316_male, if((mem1316_female=1 and educind=2) or (mem1316_female=1 and educind=1 and (gradel<17 or gradel>22)),1,0) as nths1316_female, if((mem1215=1 and educind=2) or (mem1215=1 and educind=1 and (gradel<17 or gradel>22)),1,0) as nths1215, if((mem1215_male=1 and educind=2) or (mem1215_male=1 and educind=1 and (gradel<17 or gradel>22)),1,0) as nths1215_male, if((mem1215_female=1 and educind=2) or (mem1215_female=1 and educind=1 and (gradel<17 or gradel>22)),1,0) as nths1215_female, if((mem616=1 and educind=2),1,0) as ntsch616, if((mem616_male=1 and educind=2),1,0) as ntsch616_male, if((mem616_female=1 and educind=2),1,0) as ntsch616_female, if((mem615=1 and educind=2),1,0) as ntsch615, if((mem615_male=1 and educind=2),1,0) as ntsch615_male, if((mem615_female=1 and educind=2),1,0) as ntsch615_female, if((mem10ab=1 and literind=2),1,0) as ntliter10ab, if((mem10ab_male=1 and literind=2),1,0) as ntliter10ab_male, if((mem10ab_female=1 and literind=2),1,0) as ntliter10ab_female, \n\
			IF((jobind=2 and entrepind=2),1,0) AS wnojob, IF((jobind=2 and entrepind=2) And sex=1,1,0) AS wnojob_male, IF((jobind=2 and entrepind=2) And sex=2,1,0) AS wnojob_female, IF(mcrimeind=1,1,0) AS mvictcr, IF(mcrimeind=1 And sex=1,1,0) AS mvictcr_male, IF(mcrimeind=1 And sex=2,1,0) AS mvictcr_female, If(((mem15ab=1 And (jobind=1 or entrepind=1)) Or (mem15ab=1 And (jobind=2 and entrepind=2) And fjob=1 And joppind=1 And wtwind=1) Or (mem15ab=1 And (jobind=2 and entrepind=2) And fjob=2 And ynotlookjob>=2 And ynotlookjob<=5 And joppind=1 And wtwind=1) Or (mem15ab=1 And (jobind=2 and entrepind=2) And fjob=2 And ynotlookjob=1 And lastlookjob>=1 And lastlookjob<=2 And joppind=1 And wtwind=1)),1,0) AS labfor, \n\
			If(((mem15ab_male=1 And (jobind=1 or entrepind=1)) Or (mem15ab_male=1 And (jobind=2 and entrepind=2) And fjob=1 And joppind=1 And wtwind=1) Or (mem15ab_male=1 And (jobind=2 and entrepind=2) And fjob=2 And ynotlookjob>=2 And ynotlookjob<=5 And joppind=1 And wtwind=1) Or (mem15ab_male=1 And (jobind=2 and entrepind=2) And fjob=2 And ynotlookjob=1 And lastlookjob>=1 And lastlookjob<=2 And joppind=1 And wtwind=1)),1,0) AS labfor_male, If(((mem15ab_female=1 And (jobind=1 or entrepind=1)) Or (mem15ab_female=1 And (jobind=2 and entrepind=2) And fjob=1 And joppind=1 And wtwind=1) Or (mem15ab_female=1 And (jobind=2 and entrepind=2) And fjob=2 And ynotlookjob>=2 And ynotlookjob<=5 And joppind=1 And wtwind=1) Or (mem15ab_female=1 And (jobind=2 and entrepind=2) And fjob=2 And ynotlookjob=1 And lastlookjob>=1 And lastlookjob<=2 And joppind=1 And wtwind=1)),1,0) AS labfor_female, If((mem15ab=1 And (jobind=1 or entrepind=1)),1,0) AS empl15ab, If((mem15ab_male=1 And (jobind=1 or entrepind=1)),1,0) AS empl15ab_male, If((mem15ab_female=1 And (jobind=1 or entrepind=1)),1,0) AS empl15ab_female, If((mem15ab<>1 And (jobind=1 or entrepind=1)),1,0) AS empl014, If(mem15ab=1 And (jobind=1 or entrepind=1) And fxtra_wrk=1, 1, 0) AS underempl15ab_wrk, If(mem15ab_female=1 And (jobind=1 or entrepind=1) And fxtra_wrk=1, 1, 0) AS underempl15ab_wrk_female, If(mem15ab_male=1 And (jobind=1 or entrepind=1) And fxtra_wrk=1, 1, 0) AS underempl15ab_wrk_male, If(mem15ab=1 And (jobind=1 or entrepind=1) And fadd_work_hrs=1, 1, 0) AS underempl15ab_hrs, If(mem15ab_female=1 And (jobind=1 or entrepind=1) And fadd_work_hrs=1, 1, 0) AS underempl15ab_hrs_female, If(mem15ab_male=1 And (jobind=1 or entrepind=1) And fadd_work_hrs=1, 1, 0) AS underempl15ab_hrs_male, If(mem15ab=1 And (jobind=1 or entrepind=1) And (fxtra_wrk=1 OR fadd_work_hrs=1), 1, 0) AS underempl15ab, If(mem15ab_female=1 And (jobind=1 or entrepind=1) And (fxtra_wrk=1 OR fadd_work_hrs=1), 1, 0) AS underempl15ab_female, If(mem15ab_male=1 And (jobind=1 or entrepind=1) And (fxtra_wrk=1 OR fadd_work_hrs=1), 1, 0) AS underempl15ab_male \n\
			FROM (SELECT hpq_mem.urb, hpq_mem.regn, hpq_mem.prov, hpq_mem.mun, hpq_mem.brgy, hpq_mem.purok, hpq_mem.hcn, hpq_mem.memno, CONCAT(regn, prov, mun, brgy, purok, hcn, memno) AS `memID`, \n\
			reln, SEX, age_yr, civstat, educal, pwd_ind, pwd_type, fadd_work_hrs, fxtra_wrk, mcrimeind, if(sex=1,1,0) as Male,  if(sex=2,1,0) as Female,  if(reln<>8,1,0) as nthhwrkr, If((age_yr>=60),1,0) AS mem60ab, If((age_yr>=60) AND sex=1,1,0) AS mem60ab_male, If((age_yr>=60) AND sex=2,1,0) AS mem60ab_female, If((age_yr>=65),1,0) AS mem65ab, If((age_yr>=65) AND sex=1,1,0) AS mem65ab_male, If((age_yr>=65) AND sex=2,1,0) AS mem65ab_female, If(age_yr=0,1,0) AS mem01, If((age_yr=0) and (sex=1),1,0) AS mem01_male, If((age_yr=0) and (sex=2),1,0) AS mem01_female, If((age_yr<=4),1,0) AS mem04, If((age_yr<=4) and (sex=1),1,0) AS mem04_male, If((age_yr<=4) and (sex=2),1,0) AS mem04_female, If((age_yr<=4 AND age_yr>=1),1,0) AS mem14, If((age_yr<=4 AND age_yr>=1) and (sex=1),1,0) AS mem14_male, If((age_yr<=4 AND age_yr>=1) and (sex=2),1,0) AS mem14_female, If((age_yr<=5),1,0) AS mem05, If((age_yr<=5) and (sex=1),1,0) AS mem05_male, If((age_yr<=5) and (sex=2),1,0) AS mem05_female, mnutind, If((age_yr>=3 And age_yr<=5),1,0) AS mem35, educind, gradel, If((educind=1 And gradel=1),1,0) AS daycare, If((educind=1 And gradel=2),1,0) AS kinprep, If((age_yr>=6 And age_yr<=12),1,0) AS mem612, If((age_yr>=6 And age_yr<=12) AND (sex=1),1,0) AS mem612_male, If((age_yr>=6 And age_yr<=12) AND (sex=2),1,0) AS mem612_female, If((age_yr>=6 And age_yr<=11),1,0) AS mem611, If((age_yr>=6 And age_yr<=11) AND (sex=1),1,0) AS mem611_male, If((age_yr>=6 And age_yr<=11) AND (sex=2),1,0) AS mem611_female, If((age_yr>=13 And age_yr<=16),1,0) AS mem1316, If((age_yr>=13 And age_yr<=16) AND (sex=1),1,0) AS mem1316_male, If((age_yr>=13 And age_yr<=16) AND (sex=2),1,0) AS mem1316_female, If((age_yr>=12 And age_yr<=15),1,0) AS mem1215, If((age_yr>=12 And age_yr<=15) AND (sex=1),1,0) AS mem1215_male, If((age_yr>=12 And age_yr<=15) AND (sex=2),1,0) AS mem1215_female, If((age_yr>=6 And age_yr<=16),1,0) AS mem616, If((age_yr>=6 And age_yr<=16) AND (sex=1),1,0) AS mem616_male, If((age_yr>=6 And age_yr<=16) AND (sex=2),1,0) AS mem616_female, If((age_yr>=6 And age_yr<=15),1,0) AS mem615, If((age_yr>=6 And age_yr<=15) AND (sex=1),1,0) AS mem615_male, If((age_yr>=6 And age_yr<=15) AND (sex=2),1,0) AS mem615_female, literind, If((age_yr>=10),1,0) AS mem10ab, If((age_yr>=10) AND sex=1,1,0) AS mem10ab_male, If((age_yr>=10) AND sex=2,1,0) AS mem10ab_female, If((age_yr>=15),1,0) AS mem15ab, If((age_yr>=15) AND sex=1,1,0) AS mem15ab_male, If((age_yr>=15) AND sex=2,1,0) AS mem15ab_female, \n\
			If((age_yr>=18),1,0) AS mem18ab, If((age_yr>=18) AND sex=1,1,0) AS mem18ab_male, If((age_yr>=18) AND sex=2,1,0) AS mem18ab_female, If((age_yr<=18),1,0) AS mem018, If((age_yr<=18) AND sex=1,1,0) AS mem018_male, If((age_yr<=18) AND sex=2,1,0) AS mem018_female, \n\
			If((age_yr>=17),1,0) AS mem17ab, If((age_yr>=17) AND sex=1,1,0) AS mem17ab_male, If((age_yr>=17) AND sex=2,1,0) AS mem17ab_female, If((age_yr<=17),1,0) AS mem017, If((age_yr<=17) AND sex=1,1,0) AS mem017_male, If((age_yr<=17) AND sex=2,1,0) AS mem017_female, \n\
			If((age_yr>=17 And age_yr<=21),1,0) AS mem1721, If((age_yr>=17 And age_yr<=21) AND (sex=1),1,0) AS mem1721_male, If((age_yr>=17 And age_yr<=21) AND (sex=2),1,0) AS mem1721_female, If((age_yr>=16 And age_yr<=20),1,0) AS mem1620, If((age_yr>=16 And age_yr<=20) AND (sex=1),1,0) AS mem1620_male, If((age_yr>=16 And age_yr<=20) AND (sex=2),1,0) AS mem1620_female, jobind, entrepind, fjob, ynotlookjob, lastlookjob, joppind, wtwind, RIGHT(CONCAT('0000', CAST(psoc4 as char)), 4) as psoc4, LEFT(RIGHT(CONCAT('0000', CAST(psoc4 as char)), 4),1) as g_occ, RIGHT(CONCAT('0000', CAST(psic4 as char)), 4) as psic4, LEFT(RIGHT(CONCAT('0000', CAST(psic4 as char)), 4),1) as sector, jstatus, \n\
			If((age_yr>=0 And age_yr<=14),1,0) AS mem014, If((age_yr>=0 And age_yr<=14) AND (sex=1),1,0) AS mem014_male, If((age_yr>=0 And age_yr<=14) AND (sex=2),1,0) AS mem014_female, If((age_yr>=15 And age_yr<=64),1,0) AS mem1564, If((age_yr>=15 And age_yr<=64) AND (sex=1),1,0) AS mem1564_male, If((age_yr>=15 And age_yr<=64) AND (sex=2),1,0) AS mem1564_female, If((age_yr>=15 And age_yr<=24),1,0) AS mem1524, If((age_yr>=15 And age_yr<=24) AND (sex=1),1,0) AS mem1524_male, If((age_yr>=15 And age_yr<=24) AND (sex=2),1,0) AS mem1524_female \n\
			FROM `hpq_mem` WHERE (ofw=2 or ofw=0 or isnull(ofw)=TRUE)) as hpq_mem_temp) as mem_ind_temp;";
		}
		else {
			AfxMessageBox(_T("The version of the questionnaire is not in the catalog."));
			sSQL = "NULL";
		}
		break;

	case HH:

		if (m_qnrID==1120041100 || m_qnrID==1020040900) {
		//general (revised for pmt) - 11200411
		sSQL = "CREATE TABLE HH_Ind \n\
			SELECT *, If(((hh_ntsws=1) OR (hh_ntstf=1) OR (typewall=3 Or typewall=6 Or typeroof=3 Or typeroof=6) OR (tenurstat=5 Or tenurstat=7)), 1,0) AS HH_InadeqLC, If((HH_nAsset>=1),1,0) AS HH_w1ASSET, If((hh_nasset=12),1,0) AS HH_WellASSETl, If((tenurstat=5 Or tenurstat=7),1,0) AS HH_Squat, If(((typewall=3 Or typewall=6 Or typeroof=3 Or typeroof=6)),1,0) AS HH_MSH \n\
			FROM (SELECT regn, prov, mun, brgy, purok, hcn, HHID, urb, victdeath, victdeathm, victdeathf, victtheft, victtheftm, victtheftf, victrape, victrapem, victrapef, victinjur, victinjurm, victinjurf, victocrim, victocrimm, victocrimf, victdeath+victtheft+victrape+victinjur+victocrim AS victcr, victdeathm+victtheftm+victrapem+victinjurm+victocrimm AS victcr_male, victdeathf+victtheftf+victrapef+victinjurf+victocrimf AS victcr_female, dead05m_1+dead05m_2+dead05m_3+dead05m_4+dead05m_5 AS death05_male, dead05f_1+dead05f_2+dead05f_3+dead05f_4+dead05f_5 AS death05_female, dead05_1+dead05_2+dead05_3+dead05_4+dead05_5 AS death05, dead04m_1+dead04m_2+dead04m_3+dead04m_4+dead04m_5 AS death04_male, dead04f_1+dead04f_2+dead04f_3+dead04f_4+dead04f_5 AS death04_female, dead04_1+dead04_2+dead04_3+dead04_4+dead04_5 AS death04, dead14m_1+dead14m_2+dead14m_3+dead14m_4+dead14m_5 AS death14_male, dead14f_1+dead14f_2+dead14f_3+dead14f_4+dead14f_5 AS death14_female, dead14_1+dead14_2+dead14_3+dead14_4+dead14_5 AS death14, dead01m_1+dead01m_2+dead01m_3+dead01m_4+dead01m_5 AS death01_male, dead01f_1+dead01f_2+dead01f_3+dead01f_4+dead01f_5 AS death01_female, dead01_1+dead01_2+dead01_3+dead01_4+dead01_5 AS death01, deadpreg_1+deadpreg_2+deadpreg_3+deadpreg_4+deadpreg_5 AS deathpreg, 0 AS deathpreg_male, deadpreg_1+deadpreg_2+deadpreg_3+deadpreg_4+deadpreg_5 AS deathpreg_female, HH_SWS, HH_ntSWS, WaterFac, HH_stf, HH_ntstf, ToilFac, If((ToilFac=1 OR ToilFac=2), 4, If(ToilFac=3, 3, If(ToilFac=4, 2, If(ToilFac>=6, 1, 0)))) AS HH_toilscore, HH_ntmsw, TypeWall, HH_ntmsr, Typeroof, If((HH_ntmsr=1 And HH_ntmsw=1),1,0) AS HH_ntmsh, HH_ntSquat, TenurStat, HH_welec, HH_wTV, HH_wVHS, HH_wComputer, HH_wREF, HH_wIRON, HH_wSTOVE, HH_wWMACH, HH_wMICROW, HH_wAIRC, HH_wPhone, HH_wCAR, HH_wTV+HH_wVHS+HH_wComputer+HH_wREF+HH_wIRON+HH_wSTOVE+HH_wWMACH+HH_wMICROW+HH_wAIRC+HH_wPhone+HH_wCAR AS HH_nASSET, \n\
			HH_CropFarm, HH_CropInc, (HH_CropInc/HH_Totin)*100 AS HH_CropIncPer, HH_Poult, HH_PoultInc, (HH_PoultInc/HH_Totin)*100 AS HH_PoultIncPer, HH_FishAqua, HH_FishInc, (HH_FishInc/HH_Totin)*100 AS HH_FishIncPer, HH_Forest, HH_ForInc, (HH_ForInc/HH_Totin)*100 AS HH_ForIncPer, HH_SaleRetail, HH_SaleInc, (HH_SaleInc/HH_Totin)*100 AS HH_SaleIncPer, HH_Manufacture, HH_ManufInc, (HH_ManufInc/HH_Totin)*100 AS HH_ManufIncPer, HH_Services, HH_ServInc, (HH_ServInc/HH_Totin)*100 AS HH_ServIncPer, HH_TransComm, HH_TransInc, (HH_TransInc/HH_Totin)*100 AS HH_TransIncPer, HH_MineQuarry, HH_MineInc, (HH_MineInc/HH_Totin)*100 AS HH_MineIncPer, HH_Construct, HH_ConstrInc, (HH_ConstrInc/HH_Totin)*100 AS HH_ConstrIncPer, HH_NEC, HH_EntrNECInc, (HH_EntrNECInc/HH_Totin)*100 AS HH_EntrNECIncPer, HH_EntreprInc, (HH_EntreprInc/HH_Totin)*100 AS HH_EntreprIncPer, \n\
			HH_SalWagInc, (HH_SalWagInc/HH_Totin)*100 AS HH_SalWagIncPer, HH_NetShCrpInc, (HH_NetShCrpInc/HH_Totin)*100 AS HH_NetShCrpIncPer, HH_SUPFInc, (HH_SupfInc/HH_Totin)*100 AS HH_SupfIncPer, HH_SUPRInc, (HH_SuprInc/HH_Totin)*100 AS HH_SuprIncPer, HH_RENTVInc, (HH_RentvInc/HH_Totin)*100 AS HH_RentvIncPer, HH_InterestInc, (HH_InterestInc/HH_Totin)*100 AS HH_InterestIncPer, HH_PensionInc, (HH_PensionInc/HH_Totin)*100 AS HH_PensionIncPer, HH_DividendInc, (HH_DividendInc/HH_Totin)*100 AS HH_DividendIncPer, HH_OTHSNECInc, (HH_OthsNECInc/HH_Totin)*100 AS HH_OthsNECIncPer, HH_OthSrcInc, (HH_OthSrcInc/HH_Totin)*100 AS HH_OthSrcIncPer, \n\
			HH_TOTIN, HH_PCI, hsize, hsize_sq, povThresh, subThresh, If((HH_PCI<povThresh),1,0) AS HH_PovP, If((HH_PCI<subThresh),1,0) AS HH_SubP, If((HH_PCI<povThresh And isnull(hh_totin)=false),(povthresh-hh_pci)/povthresh,0) AS HH_PovGap, HH_fshort_imm, HH_fshort_3mos, If((HH_fshort_imm=1 Or HH_fshort_3mos=1),1,0) AS HH_fshort, \n\
			HH_wOFW, HH_nOFW, OFW1_Occup, OFW2_Occup, OFW3_Occup, OFW4_Occup, OFW5_Occup, OFW1_Country, OFW2_Country, OFW3_Country, OFW4_Country, OFW5_Country \n\
			FROM (SELECT hpq_hh.regn, hpq_hh.prov, hpq_hh.mun, hpq_hh.brgy, hpq_hh.purok, hpq_hh.hcn, CONCAT(regn, prov, mun, brgy, purok, hcn) AS hhID, urb, hsize, power(hsize,2) AS hsize_sq, \n\
			If(deathind=1,deathtot,0) AS victdeath, If(deathind=1,deathm,0) AS victdeathm, If(deathind=1,deathf,0) AS victdeathf, If(theftind=1,thefttot,0) AS victtheft, If(theftind=1,theftm,0) AS victtheftm, If(theftind=1,theftf,0) AS victtheftf, If(rapeind=1,rapetot,0) AS victrape, If(rapeind=1,rapem,0) AS victrapem, If(rapeind=1,rapef,0) AS victrapef, If(injurind=1,injurtot,0) AS victinjur, If(injurind=1,injurm,0) AS victinjurm, If(injurind=1,injurf,0) AS victinjurf, If(ocrimind=1,ocrimtot,0) AS victocrim, If(ocrimind=1,ocrimm,0) AS victocrimm, If(ocrimind=1,ocrimf,0) AS victocrimf, \n\
			If((prevmind=1 And mdeadage_1<=5 And mdeadsx_1=1),1,0) AS dead05m_1, If((prevmind=1 And mdeadage_2<=5 And mdeadsx_2=1),1,0) AS dead05m_2, If((prevmind=1 And mdeadage_3<=5 And mdeadsx_3=1),1,0) AS dead05m_3, If((prevmind=1 And mdeadage_4<=5 And mdeadsx_4=1),1,0) AS dead05m_4, If((prevmind=1 And mdeadage_5<=5 And mdeadsx_5=1),1,0) AS dead05m_5, If((prevmind=1 And mdeadage_1<=5 And mdeadsx_1=2),1,0) AS dead05f_1, If((prevmind=1 And mdeadage_2<=5 And mdeadsx_2=2),1,0) AS dead05f_2, If((prevmind=1 And mdeadage_3<=5 And mdeadsx_3=2),1,0) AS dead05f_3, If((prevmind=1 And mdeadage_4<=5 And mdeadsx_4=2),1,0) AS dead05f_4, If((prevmind=1 And mdeadage_5<=5 And mdeadsx_5=2),1,0) AS dead05f_5, \n\
			If((prevmind=1 And mdeadage_1<=5 And (mdeadsx_1=1 OR mdeadsx_1=2)),1,0) AS dead05_1, If((prevmind=1 And mdeadage_2<=5 And (mdeadsx_2=1 OR mdeadsx_2=2)),1,0) AS dead05_2, If((prevmind=1 And mdeadage_3<=5 And (mdeadsx_3=1 OR mdeadsx_3=2)),1,0) AS dead05_3, If((prevmind=1 And mdeadage_4<=5 And (mdeadsx_4=1 OR mdeadsx_4=2)),1,0) AS dead05_4, If((prevmind=1 And mdeadage_5<=5 And (mdeadsx_5=1 OR mdeadsx_5=2)),1,0) AS dead05_5, \n\
			If((prevmind=1 And mdeadage_1<=4 And mdeadsx_1=1),1,0) AS dead04m_1, If((prevmind=1 And mdeadage_2<=4 And mdeadsx_2=1),1,0) AS dead04m_2, If((prevmind=1 And mdeadage_3<=4 And mdeadsx_3=1),1,0) AS dead04m_3, If((prevmind=1 And mdeadage_4<=4 And mdeadsx_4=1),1,0) AS dead04m_4, If((prevmind=1 And mdeadage_5<=4 And mdeadsx_5=1),1,0) AS dead04m_5, If((prevmind=1 And mdeadage_1<=4 And mdeadsx_1=2),1,0) AS dead04f_1, If((prevmind=1 And mdeadage_2<=4 And mdeadsx_2=2),1,0) AS dead04f_2, If((prevmind=1 And mdeadage_3<=4 And mdeadsx_3=2),1,0) AS dead04f_3, If((prevmind=1 And mdeadage_4<=4 And mdeadsx_4=2),1,0) AS dead04f_4, If((prevmind=1 And mdeadage_5<=4 And mdeadsx_5=2),1,0) AS dead04f_5, \n\
			If((prevmind=1 And mdeadage_1<=4 And (mdeadsx_1=1 OR mdeadsx_1=2)),1,0) AS dead04_1, If((prevmind=1 And mdeadage_2<=4 And (mdeadsx_2=1 OR mdeadsx_2=2)),1,0) AS dead04_2, If((prevmind=1 And mdeadage_3<=4 And (mdeadsx_3=1 OR mdeadsx_3=2)),1,0) AS dead04_3, If((prevmind=1 And mdeadage_4<=4 And (mdeadsx_4=1 OR mdeadsx_4=2)),1,0) AS dead04_4, If((prevmind=1 And mdeadage_5<=4 And (mdeadsx_5=1 OR mdeadsx_5=2)),1,0) AS dead04_5, \n\
			If((prevmind=1 And mdeadage_1<=4 And mdeadage_1>=1 And mdeadsx_1=1),1,0) AS dead14m_1, If((prevmind=1 And mdeadage_2<=4 And mdeadage_2>=1 And mdeadsx_2=1),1,0) AS dead14m_2, If((prevmind=1 And mdeadage_3<=4 And mdeadage_3>=1 And mdeadsx_3=1),1,0) AS dead14m_3, If((prevmind=1 And mdeadage_4<=4 And mdeadage_4>=1 And mdeadsx_4=1),1,0) AS dead14m_4, If((prevmind=1 And mdeadage_5<=4 And mdeadage_5>=1 And mdeadsx_5=1),1,0) AS dead14m_5, If((prevmind=1 And mdeadage_1<=4 And mdeadage_1>=1 And mdeadsx_1=2),1,0) AS dead14f_1, If((prevmind=1 And mdeadage_2<=4 And mdeadage_2>=1 And mdeadsx_2=2),1,0) AS dead14f_2, If((prevmind=1 And mdeadage_3<=4 And mdeadage_3>=1 And mdeadsx_3=2),1,0) AS dead14f_3, If((prevmind=1 And mdeadage_4<=4 And mdeadage_4>=1 And mdeadsx_4=2),1,0) AS dead14f_4, If((prevmind=1 And mdeadage_5<=4 And mdeadage_5>=1 And mdeadsx_5=2),1,0) AS dead14f_5, \n\
			If((prevmind=1 And mdeadage_1<=4 And mdeadage_1>=1 And (mdeadsx_1=1 OR mdeadsx_1=2)),1,0) AS dead14_1, If((prevmind=1 And mdeadage_2<=4 And mdeadage_2>=1 And (mdeadsx_2=1 OR mdeadsx_2=2)),1,0) AS dead14_2, If((prevmind=1 And mdeadage_3<=4 And mdeadage_3>=1 And (mdeadsx_3=1 OR mdeadsx_3=2)),1,0) AS dead14_3, If((prevmind=1 And mdeadage_4<=4 And mdeadage_4>=1 And (mdeadsx_4=1 OR mdeadsx_4=2)),1,0) AS dead14_4, If((prevmind=1 And mdeadage_5<=4 And mdeadage_5>=1 And (mdeadsx_5=1 OR mdeadsx_5=2)),1,0) AS dead14_5, \n\
			If((prevmind=1 And mdeadage_1=0 And mdeadsx_1=1),1,0) AS dead01m_1, If((prevmind=1 And mdeadage_2=0 And mdeadsx_2=1),1,0) AS dead01m_2, If((prevmind=1 And mdeadage_3=0 And mdeadsx_3=1),1,0) AS dead01m_3, If((prevmind=1 And mdeadage_4=0 And mdeadsx_4=1),1,0) AS dead01m_4, If((prevmind=1 And mdeadage_5=0 And mdeadsx_5=1),1,0) AS dead01m_5, If((prevmind=1 And mdeadage_1=0 And mdeadsx_1=2),1,0) AS dead01f_1, If((prevmind=1 And mdeadage_2=0 And mdeadsx_2=2),1,0) AS dead01f_2, If((prevmind=1 And mdeadage_3=0 And mdeadsx_3=2),1,0) AS dead01f_3, If((prevmind=1 And mdeadage_4=0 And mdeadsx_4=2),1,0) AS dead01f_4, If((prevmind=1 And mdeadage_5=0 And mdeadsx_5=2),1,0) AS dead01f_5, \n\
			If((prevmind=1 And mdeadage_1=0 And (mdeadsx_1=1 OR mdeadsx_1=2)),1,0) AS dead01_1, If((prevmind=1 And mdeadage_2=0 And (mdeadsx_2=1 OR mdeadsx_2=2)),1,0) AS dead01_2, If((prevmind=1 And mdeadage_3=0 And (mdeadsx_3=1 OR mdeadsx_3=2)),1,0) AS dead01_3, If((prevmind=1 And mdeadage_4=0 And (mdeadsx_4=1 OR mdeadsx_4=2)),1,0) AS dead01_4, If((prevmind=1 And mdeadage_5=0 And (mdeadsx_5=1 OR mdeadsx_5=2)),1,0) AS dead01_5, \n\
			If((prevmind=1 And mdeadsx_1=2 And mdeady_1=8 And mdeadage_1>=15 And mdeadage_1<=49),1,0) AS deadpreg_1, If((prevmind=1 And mdeadsx_2=2 And mdeady_2=8 And mdeadage_2>=15 And mdeadage_2<=49),1,0) AS deadpreg_2, If((prevmind=1 And mdeadsx_3=2 And mdeady_3=8 And mdeadage_3>=15 And mdeadage_3<=49),1,0) AS deadpreg_3, If((prevmind=1 And mdeadsx_4=2 And mdeady_4=8 And mdeadage_4>=15 And mdeadage_4<=49),1,0) AS deadpreg_4, If((prevmind=1 And mdeadsx_5=2 And mdeady_5=8 And mdeadage_5>=15 And mdeadage_5<=49),1,0) AS deadpreg_5, \n\
			If((water=1 Or water=2 Or water=3 Or water=4 Or water=5 Or water=6 Or water=10),1,0) AS HH_sws, If((water=7 Or water=8 Or water=9),1,0) AS HH_ntsws, hpq_hh.WATER AS WaterFac, If((toil=1 Or toil=2 Or toil=3),1,0) AS HH_stf, If((toil=4 Or toil=5),1,0) AS HH_ntstf, hpq_hh.TOIL AS ToilFac, \n\
			If((tenur=1 Or tenur=2 Or tenur=3 Or tenur=4 Or tenur=6),1,0) AS HH_ntSquat, hpq_hh.TENUR AS TenurStat, If((wall=1 Or wall=2 Or wall=4 Or wall=5),1,0) AS HH_ntmsw, hpq_hh.WALL AS TypeWall, If((roof=1 Or roof=2 Or roof=4 Or roof=5),1,0) AS HH_ntmsr, hpq_hh.ROOF AS TypeRoof, If((welec=1),1,0) AS HH_welec, \n\
			If((TV=1),1,0) AS HH_wTV, If((VHS=1),1,0) AS HH_wVHS, If((COMPUTER=1),1,0) AS HH_wComputer, If((REF=1),1,0) AS HH_wREF, If((IRON=1),1,0) AS HH_wIRON, If((STOVE=1),1,0) AS HH_wSTOVE, If((WMACH=1),1,0) AS HH_wWMACH, If((MICROW=1),1,0) AS HH_wMICROW, If((AIRC=1),1,0) AS HH_wAIRC, If((PHONE=1),1,0) AS HH_wPhone, If((CAR=1),1,0) AS HH_wCAR, \n\
			If(CROPIND=1,1,0) AS HH_CropFarm, If(cropind=1,cropincsh+cropinknd,0) AS HH_CropInc, If(poultind=1,1,0) AS HH_Poult, If(poultind=1,pouincsh+pouinknd,0) AS HH_PoultInc, If(fishind=1,1,0) AS HH_FishAqua, If(fishind=1,fishincsh+fishinknd,0) AS HH_FishInc, If(forind=1,1,0) AS HH_Forest, If(forind=1,forincsh+forinknd,0) AS HH_ForInc, If(salind=1,1,0) AS HH_SaleRetail, If(salind=1,salincsh+salinknd,0) AS HH_SaleInc, If(manind=1,1,0) AS HH_Manufacture, If(manind=1,manincsh+maninknd,0) AS HH_ManufInc, If(servind=1,1,0) AS HH_Services, If(servind=1,servincsh+servinknd,0) AS HH_ServInc, If(trnind=1,1,0) AS HH_TransComm, If(trnind=1,trnincsh+trninknd,0) AS HH_TransInc, If(minind=1,1,0) AS HH_MineQuarry, If(minind=1,minincsh+mininknd,0) AS HH_MineInc, If(cnsind=1,1,0) AS HH_Construct, If(cnsind=1,cnsincsh+cnsinknd,0) AS HH_ConstrInc, If(eothind=1,1,0) AS HH_NEC, If(eothind=1,eoincsh+eoinknd,0) AS HH_EntrNECInc, etotcsh+etotknd AS HH_EntreprInc, \n\
			wagcsh+wagknd AS HH_SalWagInc, agscsh+agsknd AS HH_NetShCrpInc, supfcsh+supfknd AS HH_SUPFInc, suprcsh+suprknd AS HH_SUPRInc, rentvcsh+rentvknd AS HH_RENTVInc, intrcsh+intrknd AS HH_InterestInc, pencsh+penknd AS HH_PensionInc, divcsh+divknd AS HH_DividendInc, othscsh+othsknd AS HH_OTHSNECInc, OTHSTOTCSH+OTHSTOTKND AS HH_OthSrcInc, \n\
			TOTIN AS HH_TOTIN, totin/hsize AS HH_PCI, If(urb=1, `~thresh`.`povthreshr`, `~thresh`.`povthreshu`) as `povThresh`, If(urb=1, `~thresh`.`subthreshr`, `~thresh`.`subthreshu`) as `subThresh`, If((gmeal<>ymeal And gymeal=1),1,0) AS HH_fshort_imm, If((FSHORT=1),1,0) AS HH_fshort_3mos, \n\
			If((OFWIND=1),1,0) AS HH_wOFW, If((ofwind=1),nofw,0) AS HH_nOFW, If((ofwind=1 AND ofw1_reln<>0),ofw1_occode,-1) AS OFW1_Occup, If((ofwind=1 AND ofw2_reln<>0),ofw2_occode,-1) AS OFW2_Occup, If((ofwind=1 AND ofw3_reln<>0),ofw3_occode,-1) AS OFW3_Occup, If((ofwind=1 AND ofw4_reln<>0),ofw4_occode,-1) AS OFW4_Occup, If((ofwind=1 AND ofw5_reln<>0),ofw5_occode,-1) AS OFW5_Occup, If((ofwind=1 AND ofw1_reln<>0),ofw1_country,-1) AS OFW1_Country, If((ofwind=1 AND ofw2_reln<>0),ofw2_country,-1) AS OFW2_Country, If((ofwind=1 AND ofw3_reln<>0),ofw3_country,-1) AS OFW3_Country, If((ofwind=1 AND ofw4_reln<>0),ofw4_country,-1) AS OFW4_Country, If((ofwind=1 AND ofw5_reln<>0),ofw5_country,-1) AS OFW5_Country \n\
			FROM (hpq_hh INNER JOIN `~thresh` ON (`hpq_hh`.`prov`=`~thresh`.`threshprov` AND `hpq_hh`.`mun`=`~thresh`.`threshmun` AND `hpq_hh`.`int_yy`=`~thresh`.`year` AND `hpq_hh`.`int_mm`=`~thresh`.`month`))) as hpq_hh_temp) as HH_Ind_Temp;";
		}

		else if (m_qnrID==1020070400 || m_qnrID==120070300) {

		//VN01200703
		sSQL = "CREATE TABLE HH_Ind \n\
			SELECT *, If(((hh_ntsws=1) OR (hh_ntstf=1) OR (typewall=3 Or typewall=6 Or typeroof=3 Or typeroof=6) OR (tenurstat=5 Or tenurstat=7)), 1,0) AS HH_InadeqLC, If((tenurstat=5 Or tenurstat=7),1,0) AS HH_Squat, If(((typewall=3 Or typewall=6 Or typeroof=3 Or typeroof=6)),1,0) AS HH_MSH \n\
			FROM (SELECT regn, prov, mun, brgy, purok, hcn, HHID, urbcode as urb, ifnull(sum(ctvicttot),0) as victcr, ifnull(sum(ctvictmale),0) as victcr_male, ifnull(sum(ctvictfemale),0) as victcr_female, death04_male, death04_female, death04, death14_male, death14_female, death14, death05_male, death05_female, death05, death01_male, death01_female, death01, deathpreg, 0 AS deathpreg_male, deathpreg as deathpreg_female, HH_SWS, HH_ntSWS, WaterFac, HH_stf, HH_ntstf, ToilFac, If((ToilFac=1 OR ToilFac=2), 4, If(ToilFac=3, 3, If(ToilFac=4, 2, If(ToilFac>=6, 1, 0)))) AS HH_toilscore, HH_ntmsw, TypeWall, HH_ntmsr, Typeroof, If((HH_ntmsr=1 And HH_ntmsw=1),1,0) AS HH_ntmsh, HH_ntSquat, TenurStat, HH_welec, \n\
			HH_CropFarm, HH_CropInc, (HH_CropInc/HH_Totin)*100 AS HH_CropIncPer, HH_Poult, HH_PoultInc, (HH_PoultInc/HH_Totin)*100 AS HH_PoultIncPer, HH_FishAqua, HH_FishInc, (HH_FishInc/HH_Totin)*100 AS HH_FishIncPer, HH_Forest, HH_ForInc, (HH_ForInc/HH_Totin)*100 AS HH_ForIncPer, HH_SaleRetail, HH_SaleInc, (HH_SaleInc/HH_Totin)*100 AS HH_SaleIncPer, HH_Manufacture, HH_ManufInc, (HH_ManufInc/HH_Totin)*100 AS HH_ManufIncPer, HH_Services, HH_ServInc, (HH_ServInc/HH_Totin)*100 AS HH_ServIncPer, HH_TransComm, HH_TransInc, (HH_TransInc/HH_Totin)*100 AS HH_TransIncPer, HH_MineQuarry, HH_MineInc, (HH_MineInc/HH_Totin)*100 AS HH_MineIncPer, HH_Construct, HH_ConstrInc, (HH_ConstrInc/HH_Totin)*100 AS HH_ConstrIncPer, HH_NEC, HH_EntrNECInc, (HH_EntrNECInc/HH_Totin)*100 AS HH_EntrNECIncPer, HH_EntreprInc, (HH_EntreprInc/HH_Totin)*100 AS HH_EntreprIncPer, \n\
			HH_SalWagInc, (HH_SalWagInc/HH_Totin)*100 AS HH_SalWagIncPer, HH_NetShCrpInc, (HH_NetShCrpInc/HH_Totin)*100 AS HH_NetShCrpIncPer, HH_SUPFInc, (HH_SupfInc/HH_Totin)*100 AS HH_SupfIncPer, HH_SUPRInc, (HH_SuprInc/HH_Totin)*100 AS HH_SuprIncPer, HH_RENTVInc, (HH_RentvInc/HH_Totin)*100 AS HH_RentvIncPer, HH_InterestInc, (HH_InterestInc/HH_Totin)*100 AS HH_InterestIncPer, HH_PensionInc, (HH_PensionInc/HH_Totin)*100 AS HH_PensionIncPer, HH_DividendInc, (HH_DividendInc/HH_Totin)*100 AS HH_DividendIncPer, HH_OTHSNECInc, (HH_OthsNECInc/HH_Totin)*100 AS HH_OthsNECIncPer, HH_OthSrcInc, (HH_OthSrcInc/HH_Totin)*100 AS HH_OthSrcIncPer, \n\
			HH_TOTIN, HH_PCI, rhsize, rhsize_nthhwrkr, hsize, hsize_sq, povThresh, subThresh, If((HH_PCI<povThresh And isnull(hh_totin)=false),1,0) AS HH_PovP, If((HH_PCI<subThresh And isnull(hh_totin)=false),1,0) AS HH_SubP, If((HH_PCI<povThresh And isnull(hh_totin)=false),(povthresh-hh_pci)/povthresh,0) AS HH_PovGap, HH_fshort_3mos, If((HH_fshort_3mos=1),1,0) AS HH_fshort \n\
			FROM (SELECT hpq_hh.regn, hpq_hh.prov, hpq_hh.mun, hpq_hh.brgy, hpq_hh.purok, hpq_hh.hcn, CONCAT(regn, prov, mun, brgy, purok, hcn) AS hhID, hsize, hpq_hh.urb as urbcode, power(hsize,2) AS hsize_sq, \n\
			IfNull(SUM(If((mdeadage<=5 And (mdeadsx=1 OR mdeadsx=2)),1,0)), 0) AS death05, IfNull(SUM(If((mdeadage<=5 And mdeadsx=1),1,0)), 0) AS death05_male, IfNull(SUM(If((mdeadage<=5 And mdeadsx=2),1,0)), 0) AS death05_female, \n\
			IfNull(SUM(If((mdeadage<=4 And (mdeadsx=1 OR mdeadsx=2)),1,0)), 0) AS death04, IfNull(SUM(If((mdeadage<=4 And mdeadsx=1),1,0)), 0) AS death04_male, IfNull(SUM(If((mdeadage<=4 And mdeadsx=2),1,0)), 0) AS death04_female, \n\
			IfNull(SUM(If((mdeadage<=4 AND mdeadage>=1 And (mdeadsx=1 OR mdeadsx=2)),1,0)), 0) AS death14, IfNull(SUM(If((mdeadage<=4 AND mdeadage>=1 And mdeadsx=1),1,0)), 0) AS death14_male, IfNull(SUM(If((mdeadage<=4 AND mdeadage>=1 And mdeadsx=2),1,0)), 0) AS death14_female, \n\
			IfNull(SUM(If((mdeadage=0 And (mdeadsx=1 OR mdeadsx=2)),1,0)), 0) AS death01, IfNull(SUM(If((mdeadage=0 And mdeadsx=1),1,0)), 0) AS death01_male, IfNull(SUM(If((mdeadage=0 And mdeadsx=2),1,0)), 0) AS death01_female, \n\
			IfNull(SUM(If((mdeadsx=2 And mdeady=8 And mdeadage>=15 and mdeadage<=49),1,0)), 0) AS deathpreg, \n\
			If((water=1 Or water=2 Or water=3 Or water=4 Or water=5 Or water=6 Or water=10),1,0) AS HH_sws, If((water=7 Or water=8 Or water=9 Or water=11),1,0) AS HH_ntsws, hpq_hh.WATER AS WaterFac, If((toil=1 Or toil=2 Or toil=3),1,0) AS HH_stf, If((toil=4 Or toil=5),1,0) AS HH_ntstf, hpq_hh.TOIL AS ToilFac, \n\
			If((tenur=1 Or tenur=2 Or tenur=3 Or tenur=4 Or tenur=6),1,0) AS HH_ntSquat, hpq_hh.TENUR AS TenurStat, If((wall=1 Or wall=2 Or wall=4 Or wall=5),1,0) AS HH_ntmsw, hpq_hh.WALL AS TypeWall, If((roof=1 Or roof=2 Or roof=4 Or roof=5),1,0) AS HH_ntmsr, hpq_hh.ROOF AS TypeRoof, If((welec=1),1,0) AS HH_welec, \n\
			If(CROPIND=1,1,0) AS HH_CropFarm, If(cropind=1,cropincsh+cropinknd,0) AS HH_CropInc, If(poultind=1,1,0) AS HH_Poult, If(poultind=1,pouincsh+pouinknd,0) AS HH_PoultInc, If(fishind=1,1,0) AS HH_FishAqua, If(fishind=1,fishincsh+fishinknd,0) AS HH_FishInc, If(forind=1,1,0) AS HH_Forest, If(forind=1,forincsh+forinknd,0) AS HH_ForInc, If(salind=1,1,0) AS HH_SaleRetail, If(salind=1,salincsh+salinknd,0) AS HH_SaleInc, If(manind=1,1,0) AS HH_Manufacture, If(manind=1,manincsh+maninknd,0) AS HH_ManufInc, If(servind=1,1,0) AS HH_Services, If(servind=1,servincsh+servinknd,0) AS HH_ServInc, If(trnind=1,1,0) AS HH_TransComm, If(trnind=1,trnincsh+trninknd,0) AS HH_TransInc, If(minind=1,1,0) AS HH_MineQuarry, If(minind=1,minincsh+mininknd,0) AS HH_MineInc, If(cnsind=1,1,0) AS HH_Construct, If(cnsind=1,cnsincsh+cnsinknd,0) AS HH_ConstrInc, If(eothind=1,1,0) AS HH_NEC, If(eothind=1,eoincsh+eoinknd,0) AS HH_EntrNECInc, etotcsh+etotknd AS HH_EntreprInc, \n\
			wagcsh+wagknd AS HH_SalWagInc, agscsh+agsknd AS HH_NetShCrpInc, supfcsh+supfknd AS HH_SUPFInc, suprcsh+suprknd AS HH_SUPRInc, rentvcsh+rentvknd AS HH_RENTVInc, intrcsh+intrknd AS HH_InterestInc, pencsh+penknd AS HH_PensionInc, divcsh+divknd AS HH_DividendInc, othscsh+othsknd AS HH_OTHSNECInc, OTHSTOTCSH+OTHSTOTKND AS HH_OthSrcInc, \n\
			TOTIN AS HH_TOTIN, rhsize, rhsize_nthhwrkr, totin/rhsize_nthhwrkr AS HH_PCI, If(hpq_hh.urb=1, `~thresh`.`povthreshr`, `~thresh`.`povthreshu`) as `povThresh`, If(hpq_hh.urb=1, `~thresh`.`subthreshr`, `~thresh`.`subthreshu`) as `subThresh`, If((FSHORT=1),1,0) AS HH_fshort_3mos \n\
			FROM (SELECT regn, prov, mun, brgy, purok, hcn, count(mem_ind.hcn) as rhsize, sum(mem_ind.nthhwrkr) as rhsize_nthhwrkr FROM mem_ind GROUP BY regn, prov, mun, brgy, purok, hcn) AS mem_ind_temp \n\
			INNER JOIN hpq_hh USING(regn, prov, mun, brgy, purok, hcn) LEFT JOIN `~thresh` ON (`hpq_hh`.`prov`=`~thresh`.`threshprov` AND `hpq_hh`.`mun`=`~thresh`.`threshmun` AND `hpq_hh`.`int_yy`=`~thresh`.`year` AND `hpq_hh`.`int_mm`=`~thresh`.`month`) LEFT JOIN `hpq_death` USING (regn, prov, mun, brgy, purok, hcn) GROUP BY regn, prov, mun, brgy, purok, hcn) as hpq_hh_temp LEFT JOIN hpq_crime USING (regn, prov, mun, brgy, purok, hcn) GROUP BY regn, prov, mun, brgy, purok, hcn) as hh_ind_temp;";
		//made left join on thresh
		}

		else if (m_qnrID==120110100 || m_qnrID==1020100100) {
		//VN01201101
		sSQL = "CREATE TABLE HH_Ind \n\
			SELECT *, If(((hh_ntsws=1) OR (hh_ntstf=1) OR (hh_msh=1) OR (hh_squat=1)), 1,0) AS HH_InadeqLC \n\
			FROM (SELECT regn, prov, mun, brgy, purok, hcn, HHID, urbcode as urb, ifnull(sum(ctvicttot),0) as victcr, ifnull(sum(ctvictmale),0) as victcr_male, ifnull(sum(ctvictfemale),0) as victcr_female, death04_male, death04_female, death04, death14_male, death14_female, death14, death05_male, death05_female, death05, death01_male, death01_female, death01, deathpreg, 0 AS deathpreg_male, deathpreg as deathpreg_female, \n\
			If((tenurstat=5 Or tenurstat=7 Or tenurstat=8 Or tenurstat=9),1,0) AS HH_Squat, If(((typewall=3 Or typewall=6 Or typeroof=3 Or typeroof=6)),1,0) AS HH_MSH, If((hh_sws=0 And (IsNull(waterfac)=false and waterfac<>14)),1,0) AS HH_ntSWS, If(toilfac=4 or toilfac=5,1,0) AS HH_ntSTF, HH_SWS, WaterFac, HH_stf, ToilFac, If((ToilFac=1 OR ToilFac=2), 4, If(ToilFac=3, 3, If(ToilFac=4, 2, If(ToilFac>=6, 1, 0)))) AS HH_toilscore, HH_ntmsw, TypeWall, HH_ntmsr, Typeroof, If((HH_ntmsr=1 And HH_ntmsw=1),1,0) AS HH_ntmsh, HH_ntSquat, TenurStat, HH_welec, house_type, \n\
			HH_CropFarm, HH_CropInc, (HH_CropInc/HH_Totin)*100 AS HH_CropIncPer, HH_Poult, HH_PoultInc, (HH_PoultInc/HH_Totin)*100 AS HH_PoultIncPer, HH_FishAqua, HH_FishInc, (HH_FishInc/HH_Totin)*100 AS HH_FishIncPer, HH_Forest, HH_ForInc, (HH_ForInc/HH_Totin)*100 AS HH_ForIncPer, HH_SaleRetail, HH_SaleInc, (HH_SaleInc/HH_Totin)*100 AS HH_SaleIncPer, HH_Manufacture, HH_ManufInc, (HH_ManufInc/HH_Totin)*100 AS HH_ManufIncPer, HH_Services, HH_ServInc, (HH_ServInc/HH_Totin)*100 AS HH_ServIncPer, HH_TransComm, HH_TransInc, (HH_TransInc/HH_Totin)*100 AS HH_TransIncPer, HH_MineQuarry, HH_MineInc, (HH_MineInc/HH_Totin)*100 AS HH_MineIncPer, HH_Construct, HH_ConstrInc, (HH_ConstrInc/HH_Totin)*100 AS HH_ConstrIncPer, HH_NEC, HH_EntrNECInc, (HH_EntrNECInc/HH_Totin)*100 AS HH_EntrNECIncPer, HH_EntreprInc, (HH_EntreprInc/HH_Totin)*100 AS HH_EntreprIncPer, \n\
			HH_SalWagInc, (HH_SalWagInc/HH_Totin)*100 AS HH_SalWagIncPer, HH_NetShCrpInc, (HH_NetShCrpInc/HH_Totin)*100 AS HH_NetShCrpIncPer, HH_SUPFInc, (HH_SupfInc/HH_Totin)*100 AS HH_SupfIncPer, HH_SUPRInc, (HH_SuprInc/HH_Totin)*100 AS HH_SuprIncPer, HH_RENTVInc, (HH_RentvInc/HH_Totin)*100 AS HH_RentvIncPer, HH_InterestInc, (HH_InterestInc/HH_Totin)*100 AS HH_InterestIncPer, HH_PensionInc, (HH_PensionInc/HH_Totin)*100 AS HH_PensionIncPer, HH_DividendInc, (HH_DividendInc/HH_Totin)*100 AS HH_DividendIncPer, HH_OTHSNECInc, (HH_OthsNECInc/HH_Totin)*100 AS HH_OthsNECIncPer, HH_OthSrcInc, (HH_OthSrcInc/HH_Totin)*100 AS HH_OthSrcIncPer, \n\
			HH_TOTIN, HH_PCI, rhsize, rhsize_nthhwrkr, hsize, hsize_sq, povThresh, subThresh, If((HH_PCI<povThresh And isnull(hh_totin)=false),1,0) AS HH_PovP, If((HH_PCI<subThresh And isnull(hh_totin)=false),1,0) AS HH_SubP, If((HH_PCI<povThresh And isnull(hh_totin)=false),(povthresh-hh_pci)/povthresh,0) AS HH_PovGap, HH_fshort_3mos, If((HH_fshort_3mos=1),1,0) AS HH_fshort \n\
			FROM (SELECT hpq_hh.regn, hpq_hh.prov, hpq_hh.mun, hpq_hh.brgy, hpq_hh.purok, hpq_hh.hcn, CONCAT(regn, prov, mun, brgy, purok, hcn) AS hhID, hsize, hpq_hh.urb as urbcode, power(hsize,2) AS hsize_sq, \n\
			IfNull(SUM(If((mdeadage<=5 And (mdeadsx=1 OR mdeadsx=2)),1,0)), 0) AS death05, IfNull(SUM(If((mdeadage<=5 And mdeadsx=1),1,0)), 0) AS death05_male, IfNull(SUM(If((mdeadage<=5 And mdeadsx=2),1,0)), 0) AS death05_female, \n\
			IfNull(SUM(If((mdeadage<=4 And (mdeadsx=1 OR mdeadsx=2)),1,0)), 0) AS death04, IfNull(SUM(If((mdeadage<=4 And mdeadsx=1),1,0)), 0) AS death04_male, IfNull(SUM(If((mdeadage<=4 And mdeadsx=2),1,0)), 0) AS death04_female, \n\
			IfNull(SUM(If((mdeadage<=4 AND mdeadage>=1 And (mdeadsx=1 OR mdeadsx=2)),1,0)), 0) AS death14, IfNull(SUM(If((mdeadage<=4 AND mdeadage>=1 And mdeadsx=1),1,0)), 0) AS death14_male, IfNull(SUM(If((mdeadage<=4 AND mdeadage>=1 And mdeadsx=2),1,0)), 0) AS death14_female, \n\
			IfNull(SUM(If((mdeadage=0 And (mdeadsx=1 OR mdeadsx=2)),1,0)), 0) AS death01, IfNull(SUM(If((mdeadage=0 And mdeadsx=1),1,0)), 0) AS death01_male, IfNull(SUM(If((mdeadage=0 And mdeadsx=2),1,0)), 0) AS death01_female, \n\
			IfNull(SUM(If((mdeadsx=2 And mdeady=8 And mdeadage>=15 And mdeadage<=49),1,0)), 0) AS deathpreg, house_type, \n\
			If((water=1 Or water=2 Or water=3 Or water=4 Or water=5 Or water=6 Or water=8 Or water=10 Or (water=14 And (source_water=1 Or source_water=2 Or source_water=3 Or source_water=4 Or source_water=5 Or source_water=6 Or source_water=8 Or source_water=10))),1,0) AS HH_sws, hpq_hh.WATER AS WaterFac, If((toil=1 Or toil=2 Or toil=3),1,0) AS HH_stf, hpq_hh.TOIL AS ToilFac, \n\
			If((tenur=1 Or tenur=2 Or tenur=3 Or tenur=4 Or tenur=6),1,0) AS HH_ntSquat, hpq_hh.TENUR AS TenurStat, If((wall=1 Or wall=2 Or wall=4 Or wall=5),1,0) AS HH_ntmsw, hpq_hh.WALL AS TypeWall, If((roof=1 Or roof=2 Or roof=4 Or roof=5),1,0) AS HH_ntmsr, hpq_hh.ROOF AS TypeRoof, If((welec=1),1,0) AS HH_welec, \n\
			If(CROPIND=1,1,0) AS HH_CropFarm, If(cropind=1,cropincsh+cropinknd,0) AS HH_CropInc, If(poultind=1,1,0) AS HH_Poult, If(poultind=1,pouincsh+pouinknd,0) AS HH_PoultInc, If(fishind=1,1,0) AS HH_FishAqua, If(fishind=1,fishincsh+fishinknd,0) AS HH_FishInc, If(forind=1,1,0) AS HH_Forest, If(forind=1,forincsh+forinknd,0) AS HH_ForInc, If(salind=1,1,0) AS HH_SaleRetail, If(salind=1,salincsh+salinknd,0) AS HH_SaleInc, If(manind=1,1,0) AS HH_Manufacture, If(manind=1,manincsh+maninknd,0) AS HH_ManufInc, If(servind=1,1,0) AS HH_Services, If(servind=1,servincsh+servinknd,0) AS HH_ServInc, If(trnind=1,1,0) AS HH_TransComm, If(trnind=1,trnincsh+trninknd,0) AS HH_TransInc, If(minind=1,1,0) AS HH_MineQuarry, If(minind=1,minincsh+mininknd,0) AS HH_MineInc, If(cnsind=1,1,0) AS HH_Construct, If(cnsind=1,cnsincsh+cnsinknd,0) AS HH_ConstrInc, If(eothind=1,1,0) AS HH_NEC, If(eothind=1,eoincsh+eoinknd,0) AS HH_EntrNECInc, etotcsh+etotknd AS HH_EntreprInc, \n\
			wagcsh+wagknd AS HH_SalWagInc, agscsh+agsknd AS HH_NetShCrpInc, supfcsh+supfknd AS HH_SUPFInc, suprcsh+suprknd AS HH_SUPRInc, rentvcsh+rentvknd AS HH_RENTVInc, intrcsh+intrknd AS HH_InterestInc, pencsh+penknd AS HH_PensionInc, divcsh+divknd AS HH_DividendInc, othscsh+othsknd AS HH_OTHSNECInc, OTHSTOTCSH+OTHSTOTKND AS HH_OthSrcInc, \n\
			TOTIN AS HH_TOTIN, rhsize, rhsize_nthhwrkr, totin/rhsize_nthhwrkr AS HH_PCI, If(hpq_hh.urb=1, `~thresh`.`povthreshr`, `~thresh`.`povthreshu`) as `povThresh`, If(hpq_hh.urb=1, `~thresh`.`subthreshr`, `~thresh`.`subthreshu`) as `subThresh`, If((FSHORT=1),1,0) AS HH_fshort_3mos \n\
			FROM (SELECT regn, prov, mun, brgy, purok, hcn, count(mem_ind.hcn) as rhsize, sum(mem_ind.nthhwrkr) as rhsize_nthhwrkr FROM mem_ind GROUP BY regn, prov, mun, brgy, purok, hcn) AS mem_ind_temp \n\
			INNER JOIN hpq_hh USING(regn, prov, mun, brgy, purok, hcn) INNER JOIN `~thresh` ON (`hpq_hh`.`prov`=`~thresh`.`threshprov` AND `hpq_hh`.`mun`=`~thresh`.`threshmun` AND `hpq_hh`.`int_yy`=`~thresh`.`year` AND `hpq_hh`.`int_mm`=`~thresh`.`month`) LEFT JOIN `hpq_death` USING (regn, prov, mun, brgy, purok, hcn) GROUP BY regn, prov, mun, brgy, purok, hcn) as hpq_hh_temp LEFT JOIN hpq_crime USING (regn, prov, mun, brgy, purok, hcn) GROUP BY regn, prov, mun, brgy, purok, hcn) as hh_ind_temp;";
		}
		else if (m_qnrID==720130100) {
		sSQL = "CREATE TABLE HH_Ind \n\
			SELECT *, If(((hh_ntsws=1) OR (hh_ntstf=1) OR (hh_msh=1) OR (hh_squat=1)), 1,0) AS HH_InadeqLC \n\
			FROM (SELECT regn, prov, mun, brgy, purok, hcn, HHID, urbcode as urb, death04_male, death04_female, death04, death14_male, death14_female, death14, death05_male, death05_female, death05, death01_male, death01_female, death01, deathpreg, 0 AS deathpreg_male, deathpreg as deathpreg_female, \n\
			If((tenurstat=5 Or tenurstat=7 Or tenurstat=8 Or tenurstat=9),1,0) AS HH_Squat, If(((typewall=3 Or typewall=6 Or typeroof=3 Or typeroof=6)),1,0) AS HH_MSH, If((hh_sws=0 And (IsNull(waterfac)=false and waterfac<>14)),1,0) AS HH_ntSWS, If((toilfac=4 or toilfac=5),1,0) AS HH_ntSTF, HH_SWS, WaterFac, HH_stf, ToilFac, If((ToilFac=1 OR ToilFac=2), 4, If(ToilFac=3, 3, If(ToilFac=4, 2, If(ToilFac>=6, 1, 0)))) AS HH_toilscore, HH_ntmsw, TypeWall, HH_ntmsr, Typeroof, If((HH_ntmsr=1 And HH_ntmsw=1),1,0) AS HH_ntmsh, HH_ntSquat, TenurStat, HH_welec, house_type, \n\
			HH_CropFarm, HH_CropInc, (HH_CropInc/HH_Totin)*100 AS HH_CropIncPer, HH_Poult, HH_PoultInc, (HH_PoultInc/HH_Totin)*100 AS HH_PoultIncPer, HH_FishAqua, HH_FishInc, (HH_FishInc/HH_Totin)*100 AS HH_FishIncPer, HH_Forest, HH_ForInc, (HH_ForInc/HH_Totin)*100 AS HH_ForIncPer, HH_SaleRetail, HH_SaleInc, (HH_SaleInc/HH_Totin)*100 AS HH_SaleIncPer, HH_Manufacture, HH_ManufInc, (HH_ManufInc/HH_Totin)*100 AS HH_ManufIncPer, HH_Services, HH_ServInc, (HH_ServInc/HH_Totin)*100 AS HH_ServIncPer, HH_TransComm, HH_TransInc, (HH_TransInc/HH_Totin)*100 AS HH_TransIncPer, HH_MineQuarry, HH_MineInc, (HH_MineInc/HH_Totin)*100 AS HH_MineIncPer, HH_Construct, HH_ConstrInc, (HH_ConstrInc/HH_Totin)*100 AS HH_ConstrIncPer, HH_NEC, HH_EntrNECInc, (HH_EntrNECInc/HH_Totin)*100 AS HH_EntrNECIncPer, HH_EntreprInc, (HH_EntreprInc/HH_Totin)*100 AS HH_EntreprIncPer, \n\
			HH_SalWagInc, (HH_SalWagInc/HH_Totin)*100 AS HH_SalWagIncPer, HH_NetShCrpInc, (HH_NetShCrpInc/HH_Totin)*100 AS HH_NetShCrpIncPer, HH_SUPFInc, (HH_SupfInc/HH_Totin)*100 AS HH_SupfIncPer, HH_SUPRInc, (HH_SuprInc/HH_Totin)*100 AS HH_SuprIncPer, HH_RENTVInc, (HH_RentvInc/HH_Totin)*100 AS HH_RentvIncPer, HH_InterestInc, (HH_InterestInc/HH_Totin)*100 AS HH_InterestIncPer, HH_PensionInc, (HH_PensionInc/HH_Totin)*100 AS HH_PensionIncPer, HH_DividendInc, (HH_DividendInc/HH_Totin)*100 AS HH_DividendIncPer, HH_OTHSNECInc, (HH_OthsNECInc/HH_Totin)*100 AS HH_OthsNECIncPer, HH_OthSrcInc, (HH_OthSrcInc/HH_Totin)*100 AS HH_OthSrcIncPer, \n\
			HH_TOTIN, HH_PCI, rhsize, rhsize_nthhwrkr, victcr, victcr_male, victcr_female, hsize, hsize_sq, povThresh, subThresh, If((HH_PCI<povThresh And isnull(hh_totin)=false),1,0) AS HH_PovP, If((HH_PCI<subThresh And isnull(hh_totin)=false),1,0) AS HH_SubP, If((HH_PCI<povThresh And isnull(hh_totin)=false),(povthresh-hh_pci)/povthresh,0) AS HH_PovGap, HH_fshort_3mos, If((HH_fshort_3mos=1),1,0) AS HH_fshort \n\
			FROM (SELECT hpq_hh.regn, hpq_hh.prov, hpq_hh.mun, hpq_hh.brgy, hpq_hh.purok, hpq_hh.hcn, CONCAT(regn, prov, mun, brgy, purok, hcn) AS hhID, hsize, hpq_hh.urb as urbcode, power(hsize,2) AS hsize_sq, \n\
			IfNull(SUM(If((mdeadage<=5 And (mdeadsx=1 OR mdeadsx=2)),1,0)), 0) AS death05, IfNull(SUM(If((mdeadage<=5 And mdeadsx=1),1,0)), 0) AS death05_male, IfNull(SUM(If((mdeadage<=5 And mdeadsx=2),1,0)), 0) AS death05_female, \n\
			IfNull(SUM(If((mdeadage<=4 And (mdeadsx=1 OR mdeadsx=2)),1,0)), 0) AS death04, IfNull(SUM(If((mdeadage<=4 And mdeadsx=1),1,0)), 0) AS death04_male, IfNull(SUM(If((mdeadage<=4 And mdeadsx=2),1,0)), 0) AS death04_female, \n\
			IfNull(SUM(If((mdeadage<=4 AND mdeadage>=1 And (mdeadsx=1 OR mdeadsx=2)),1,0)), 0) AS death14, IfNull(SUM(If((mdeadage<=4 AND mdeadage>=1 And mdeadsx=1),1,0)), 0) AS death14_male, IfNull(SUM(If((mdeadage<=4 AND mdeadage>=1 And mdeadsx=2),1,0)), 0) AS death14_female, \n\
			IfNull(SUM(If((mdeadage=0 And (mdeadsx=1 OR mdeadsx=2)),1,0)), 0) AS death01, IfNull(SUM(If((mdeadage=0 And mdeadsx=1),1,0)), 0) AS death01_male, IfNull(SUM(If((mdeadage=0 And mdeadsx=2),1,0)), 0) AS death01_female, \n\
			IfNull(SUM(If((mdeadsx=2 And mdeady=8 And mdeadage>=15 And mdeadage<=49),1,0)), 0) AS deathpreg, house_type, \n\
			If((water=1 Or water=2 Or water=3 Or water=4 Or water=5 Or water=6 Or water=8 Or water=10),1,0) AS HH_sws, hpq_hh.WATER AS WaterFac, If((toil=1 Or toil=2 Or toil=3),1,0) AS HH_stf, hpq_hh.TOIL AS ToilFac, \n\
			If((tenur=1 Or tenur=2 Or tenur=3 Or tenur=4 Or tenur=6),1,0) AS HH_ntSquat, hpq_hh.TENUR AS TenurStat, If((wall=1 Or wall=2 Or wall=4 Or wall=5),1,0) AS HH_ntmsw, hpq_hh.WALL AS TypeWall, If((roof=1 Or roof=2 Or roof=4 Or roof=5),1,0) AS HH_ntmsr, hpq_hh.ROOF AS TypeRoof, If((welec=1),1,0) AS HH_welec, \n\
			If(CROPIND=1,1,0) AS HH_CropFarm, If(cropind=1,cropincsh+cropinknd,0) AS HH_CropInc, If(poultind=1,1,0) AS HH_Poult, If(poultind=1,pouincsh+pouinknd,0) AS HH_PoultInc, If(fishind=1,1,0) AS HH_FishAqua, If(fishind=1,fishincsh+fishinknd,0) AS HH_FishInc, If(forind=1,1,0) AS HH_Forest, If(forind=1,forincsh+forinknd,0) AS HH_ForInc, If(salind=1,1,0) AS HH_SaleRetail, If(salind=1,salincsh+salinknd,0) AS HH_SaleInc, If(manind=1,1,0) AS HH_Manufacture, If(manind=1,manincsh+maninknd,0) AS HH_ManufInc, If(servind=1,1,0) AS HH_Services, If(servind=1,servincsh+servinknd,0) AS HH_ServInc, If(trnind=1,1,0) AS HH_TransComm, If(trnind=1,trnincsh+trninknd,0) AS HH_TransInc, If(minind=1,1,0) AS HH_MineQuarry, If(minind=1,minincsh+mininknd,0) AS HH_MineInc, If(cnsind=1,1,0) AS HH_Construct, If(cnsind=1,cnsincsh+cnsinknd,0) AS HH_ConstrInc, If(eothind=1,1,0) AS HH_NEC, If(eothind=1,eoincsh+eoinknd,0) AS HH_EntrNECInc, etotcsh+etotknd AS HH_EntreprInc, \n\
			wagcsh+wagknd AS HH_SalWagInc, agscsh+agsknd AS HH_NetShCrpInc, supfcsh+supfknd AS HH_SUPFInc, suprcsh+suprknd AS HH_SUPRInc, rentvcsh+rentvknd AS HH_RENTVInc, intrcsh+intrknd AS HH_InterestInc, pencsh+penknd AS HH_PensionInc, divcsh+divknd AS HH_DividendInc, othscsh+othsknd AS HH_OTHSNECInc, OTHSTOTCSH+OTHSTOTKND AS HH_OthSrcInc, \n\
			TOTIN AS HH_TOTIN, rhsize, rhsize_nthhwrkr, victcr, victcr_male, victcr_female, totin/rhsize_nthhwrkr AS HH_PCI, If(hpq_hh.urb=1, `~thresh`.`povthreshr`, `~thresh`.`povthreshu`) as `povThresh`, If(hpq_hh.urb=1, `~thresh`.`subthreshr`, `~thresh`.`subthreshu`) as `subThresh`, If((FSHORT=1),1,0) AS HH_fshort_3mos \n\
			FROM (SELECT regn, prov, mun, brgy, purok, hcn, count(mem_ind.hcn) as rhsize, sum(mem_ind.nthhwrkr) as rhsize_nthhwrkr, sum(mvictcr) as victcr, sum(mvictcr_male) as victcr_male, sum(mvictcr_female) as victcr_female FROM mem_ind GROUP BY regn, prov, mun, brgy, purok, hcn) AS mem_ind_temp \n\
			INNER JOIN hpq_hh USING(regn, prov, mun, brgy, purok, hcn) INNER JOIN `~thresh` ON (`hpq_hh`.`prov`=`~thresh`.`threshprov` AND `hpq_hh`.`mun`=`~thresh`.`threshmun` AND `hpq_hh`.`int_yy`=`~thresh`.`year` AND `hpq_hh`.`int_mm`=`~thresh`.`month`) LEFT JOIN `hpq_death` USING (regn, prov, mun, brgy, purok, hcn) GROUP BY regn, prov, mun, brgy, purok, hcn) as hpq_hh_temp) as hh_ind_temp;";
		}
		else if (m_qnrID==1020130100 || m_qnrID==1020130101 || m_qnrID==1020135100 || m_qnrID==1020135200 || m_qnrID==1020135300 || m_qnrID==1020135400 || m_qnrID==1020135500) {
		sSQL = "CREATE TABLE HH_Ind \n\
			SELECT *, If(((hh_ntsws=1) OR (hh_ntstf=1) OR (hh_msh=1) OR (hh_squat=1)), 1,0) AS HH_InadeqLC \n\
			FROM (SELECT regn, prov, mun, brgy, purok, hcn, HHID, urbcode as urb, death04_male, death04_female, death04, death14_male, death14_female, death14, death05_male, death05_female, death05, death01_male, death01_female, death01, deathpreg, 0 AS deathpreg_male, deathpreg as deathpreg_female, \n\
			If((tenurstat=5 Or tenurstat=7 Or tenurstat=8 Or tenurstat=9),1,0) AS HH_Squat, If(((typewall=3 Or typewall=6 Or typeroof=3 Or typeroof=6)),1,0) AS HH_MSH, If((hh_sws=0 And (IsNull(waterfac)=false And waterfac<>12)),1,0) AS HH_ntSWS, If((toilfac=6 or toilfac=8),1,0) AS HH_ntSTF, HH_SWS, WaterFac, HH_stf, ToilFac, If((ToilFac=1 OR ToilFac=2), 4, If(ToilFac=3, 3, If(ToilFac=4, 2, If(ToilFac>=6, 1, 0)))) AS HH_toilscore, HH_ntmsw, TypeWall, HH_ntmsr, Typeroof, If((HH_ntmsr=1 And HH_ntmsw=1),1,0) AS HH_ntmsh, HH_ntSquat, TenurStat, HH_welec, house_type, \n\
			HH_CropFarm, HH_CropInc, (HH_CropInc/HH_Totin)*100 AS HH_CropIncPer, HH_Poult, HH_PoultInc, (HH_PoultInc/HH_Totin)*100 AS HH_PoultIncPer, HH_FishAqua, HH_FishInc, (HH_FishInc/HH_Totin)*100 AS HH_FishIncPer, HH_Forest, HH_ForInc, (HH_ForInc/HH_Totin)*100 AS HH_ForIncPer, HH_SaleRetail, HH_SaleInc, (HH_SaleInc/HH_Totin)*100 AS HH_SaleIncPer, HH_Manufacture, HH_ManufInc, (HH_ManufInc/HH_Totin)*100 AS HH_ManufIncPer, HH_Services, HH_ServInc, (HH_ServInc/HH_Totin)*100 AS HH_ServIncPer, HH_TransComm, HH_TransInc, (HH_TransInc/HH_Totin)*100 AS HH_TransIncPer, HH_MineQuarry, HH_MineInc, (HH_MineInc/HH_Totin)*100 AS HH_MineIncPer, HH_Construct, HH_ConstrInc, (HH_ConstrInc/HH_Totin)*100 AS HH_ConstrIncPer, HH_NEC, HH_EntrNECInc, (HH_EntrNECInc/HH_Totin)*100 AS HH_EntrNECIncPer, HH_EntreprInc, (HH_EntreprInc/HH_Totin)*100 AS HH_EntreprIncPer, \n\
			HH_SalWagInc, (HH_SalWagInc/HH_Totin)*100 AS HH_SalWagIncPer, HH_NetShCrpInc, (HH_NetShCrpInc/HH_Totin)*100 AS HH_NetShCrpIncPer, HH_SUPFInc, (HH_SupfInc/HH_Totin)*100 AS HH_SupfIncPer, HH_SUPRInc, (HH_SuprInc/HH_Totin)*100 AS HH_SuprIncPer, HH_RENTVInc, (HH_RentvInc/HH_Totin)*100 AS HH_RentvIncPer, HH_InterestInc, (HH_InterestInc/HH_Totin)*100 AS HH_InterestIncPer, HH_PensionInc, (HH_PensionInc/HH_Totin)*100 AS HH_PensionIncPer, HH_DividendInc, (HH_DividendInc/HH_Totin)*100 AS HH_DividendIncPer, HH_OTHSNECInc, (HH_OthsNECInc/HH_Totin)*100 AS HH_OthsNECIncPer, HH_OthSrcInc, (HH_OthSrcInc/HH_Totin)*100 AS HH_OthSrcIncPer, \n\
			HH_TOTIN, HH_PCI, rhsize, rhsize_nthhwrkr, victcr_tmp+victmurd as victcr, victcr_tmp_male+victmurd_male as victcr_male, victcr_tmp_female+victmurd_female as victcr_female, victcr_tmp, victcr_tmp_male, victcr_tmp_female, victmurd, victmurd_male, victmurd_female, hsize, hsize_sq, povThresh, subThresh, If((HH_PCI<povThresh And isnull(hh_totin)=false),1,0) AS HH_PovP, If((HH_PCI<subThresh And isnull(hh_totin)=false),1,0) AS HH_SubP, If((HH_PCI<povThresh And isnull(hh_totin)=false),(povthresh-hh_pci)/povthresh,0) AS HH_PovGap, HH_fshort_3mos, If((HH_fshort_3mos=1),1,0) AS HH_fshort \n\
			FROM (SELECT hpq_hh.regn, hpq_hh.prov, hpq_hh.mun, hpq_hh.brgy, hpq_hh.purok, hpq_hh.hcn, CONCAT(regn, prov, mun, brgy, purok, hcn) AS hhID, hsize, hpq_hh.urb as urbcode, power(hsize,2) AS hsize_sq, \n\
			IfNull(SUM(If((mdeadage<=5 And (mdeadsx=1 OR mdeadsx=2)),1,0)), 0) AS death05, IfNull(SUM(If((mdeadage<=5 And mdeadsx=1),1,0)), 0) AS death05_male, IfNull(SUM(If((mdeadage<=5 And mdeadsx=2),1,0)), 0) AS death05_female, \n\
			IfNull(SUM(If((mdeadage<=4 And (mdeadsx=1 OR mdeadsx=2)),1,0)), 0) AS death04, IfNull(SUM(If((mdeadage<=4 And mdeadsx=1),1,0)), 0) AS death04_male, IfNull(SUM(If((mdeadage<=4 And mdeadsx=2),1,0)), 0) AS death04_female, \n\
			IfNull(SUM(If((mdeadage<=4 AND mdeadage>=1 And (mdeadsx=1 OR mdeadsx=2)),1,0)), 0) AS death14, IfNull(SUM(If((mdeadage<=4 AND mdeadage>=1 And mdeadsx=1),1,0)), 0) AS death14_male, IfNull(SUM(If((mdeadage<=4 AND mdeadage>=1 And mdeadsx=2),1,0)), 0) AS death14_female, \n\
			IfNull(SUM(If((mdeadage=0 And (mdeadsx=1 OR mdeadsx=2)),1,0)), 0) AS death01, IfNull(SUM(If((mdeadage=0 And mdeadsx=1),1,0)), 0) AS death01_male, IfNull(SUM(If((mdeadage=0 And mdeadsx=2),1,0)), 0) AS death01_female, \n\
			IfNull(SUM(If((mdeadsx=2 And mdeady=8 And mdeadage>=15 And mdeadage<=49),1,0)), 0) AS deathpreg, IfNull(SUM(If((mdeady=16),1,0)), 0) AS victmurd, IfNull(SUM(If((mdeady=16 And mdeadsx=1),1,0)), 0) AS victmurd_male, IfNull(SUM(If((mdeady=16 And mdeadsx=2),1,0)), 0) AS victmurd_female, house_type, \n\
			If((water=1 Or water=2 Or water=3 Or water=4 Or water=5 or water=11),1,0) AS HH_sws, hpq_hh.WATER AS WaterFac, If((toil=1 Or toil=2 Or toil=3 Or toil=4 Or toil=5),1,0) AS HH_stf, hpq_hh.TOIL AS ToilFac, \n\
			If((tenur=1 Or tenur=2 Or tenur=3 Or tenur=4 Or tenur=6),1,0) AS HH_ntSquat, hpq_hh.TENUR AS TenurStat, If((wall=1 Or wall=2 Or wall=4 Or wall=5),1,0) AS HH_ntmsw, hpq_hh.WALL AS TypeWall, If((roof=1 Or roof=2 Or roof=4 Or roof=5),1,0) AS HH_ntmsr, hpq_hh.ROOF AS TypeRoof, If((welec=1),1,0) AS HH_welec, \n\
			If(CROPIND=1,1,0) AS HH_CropFarm, If(cropind=1,cropincsh+cropinknd,0) AS HH_CropInc, If(poultind=1,1,0) AS HH_Poult, If(poultind=1,pouincsh+pouinknd,0) AS HH_PoultInc, If(fishind=1,1,0) AS HH_FishAqua, If(fishind=1,fishincsh+fishinknd,0) AS HH_FishInc, If(forind=1,1,0) AS HH_Forest, If(forind=1,forincsh+forinknd,0) AS HH_ForInc, If(salind=1,1,0) AS HH_SaleRetail, If(salind=1,salincsh+salinknd,0) AS HH_SaleInc, If(manind=1,1,0) AS HH_Manufacture, If(manind=1,manincsh+maninknd,0) AS HH_ManufInc, If(servind=1,1,0) AS HH_Services, If(servind=1,servincsh+servinknd,0) AS HH_ServInc, If(trnind=1,1,0) AS HH_TransComm, If(trnind=1,trnincsh+trninknd,0) AS HH_TransInc, If(minind=1,1,0) AS HH_MineQuarry, If(minind=1,minincsh+mininknd,0) AS HH_MineInc, If(cnsind=1,1,0) AS HH_Construct, If(cnsind=1,cnsincsh+cnsinknd,0) AS HH_ConstrInc, If(eothind=1,1,0) AS HH_NEC, If(eothind=1,eoincsh+eoinknd,0) AS HH_EntrNECInc, etotcsh+etotknd AS HH_EntreprInc, \n\
			wagcsh+wagknd AS HH_SalWagInc, agscsh+agsknd AS HH_NetShCrpInc, supfcsh+supfknd AS HH_SUPFInc, suprcsh+suprknd AS HH_SUPRInc, rentvcsh+rentvknd AS HH_RENTVInc, intrcsh+intrknd AS HH_InterestInc, pencsh+penknd AS HH_PensionInc, divcsh+divknd AS HH_DividendInc, othscsh+othsknd AS HH_OTHSNECInc, OTHSTOTCSH+OTHSTOTKND AS HH_OthSrcInc, \n\
			TOTIN AS HH_TOTIN, rhsize, rhsize_nthhwrkr, victcr_tmp, victcr_tmp_male, victcr_tmp_female, totin/rhsize_nthhwrkr AS HH_PCI, If(hpq_hh.urb=1, `~thresh`.`povthreshr`, `~thresh`.`povthreshu`) as `povThresh`, If(hpq_hh.urb=1, `~thresh`.`subthreshr`, `~thresh`.`subthreshu`) as `subThresh`, If((FSHORT=1),1,0) AS HH_fshort_3mos \n\
			FROM (SELECT regn, prov, mun, brgy, purok, hcn, count(mem_ind.hcn) as rhsize, sum(mem_ind.nthhwrkr) as rhsize_nthhwrkr, sum(mvictcr) as victcr_tmp, sum(mvictcr_male) as victcr_tmp_male, sum(mvictcr_female) as victcr_tmp_female FROM mem_ind GROUP BY regn, prov, mun, brgy, purok, hcn) AS mem_ind_temp \n\
			INNER JOIN hpq_hh USING(regn, prov, mun, brgy, purok, hcn) INNER JOIN `~thresh` ON (`hpq_hh`.`prov`=`~thresh`.`threshprov` AND `hpq_hh`.`mun`=`~thresh`.`threshmun` AND `hpq_hh`.`int_yy`=`~thresh`.`year` AND `hpq_hh`.`int_mm`=`~thresh`.`month`) LEFT JOIN `hpq_death` USING (regn, prov, mun, brgy, purok, hcn) GROUP BY regn, prov, mun, brgy, purok, hcn) as hpq_hh_temp) as hh_ind_temp;";
		}
		else {
			AfxMessageBox(_T("The version of the questionnaire is not in the catalog."));
			sSQL = "NULL";
		}
		break;

	case BRGY:
		sSQL = _T("CREATE TABLE brgy_ind \n\
			SELECT *, If((HH_nAsset>=1),1,0) AS HH_w1ASSET, If((hh_nasset=12),1,0) AS HH_WellASSETl, 1-HH_ntmsh AS HH_MSH, 1-HH_ntSquat AS HH_Squat, 1-HH_sws AS HH_ntSWS, 1-HH_stf AS HH_ntSTF \n\
			FROM (SELECT regn, prov, mun, brgy, purok, hcn, HHID, ifnull(sum(ctvicttot),0) as victcr, ifnull(sum(ctvictmale),0) as victcr_male, ifnull(sum(ctvictfemale),0) as victcr_female, death05_male, death05_female, death05, deathpreg, 0 AS deathpreg_male, deathpreg as deathpreg_female, HH_SWS, WaterFac, HH_stf, ToilFac, HH_ntmsw, TypeWall, HH_ntmsr, Typeroof, If((HH_ntmsr=1 And HH_ntmsw=1),1,0) AS HH_ntmsh, HH_ntSquat, TenurStat, HH_welec, HH_wTV, HH_wREF, HH_wIRON, HH_wSTOVE, HH_wWMACH, HH_wMICROW, HH_wAIRC, HH_wCAR, HH_wTV+HH_wREF+HH_wIRON+HH_wSTOVE+HH_wWMACH+HH_wMICROW+HH_wAIRC+HH_wCAR AS HH_nASSET, \n\
			HH_CropFarm, HH_CropInc, (HH_CropInc/HH_Totin)*100 AS HH_CropIncPer, HH_Poult, HH_PoultInc, (HH_PoultInc/HH_Totin)*100 AS HH_PoultIncPer, HH_FishAqua, HH_FishInc, (HH_FishInc/HH_Totin)*100 AS HH_FishIncPer, HH_Forest, HH_ForInc, (HH_ForInc/HH_Totin)*100 AS HH_ForIncPer, HH_SaleRetail, HH_SaleInc, (HH_SaleInc/HH_Totin)*100 AS HH_SaleIncPer, HH_Manufacture, HH_ManufInc, (HH_ManufInc/HH_Totin)*100 AS HH_ManufIncPer, HH_Services, HH_ServInc, (HH_ServInc/HH_Totin)*100 AS HH_ServIncPer, HH_TransComm, HH_TransInc, (HH_TransInc/HH_Totin)*100 AS HH_TransIncPer, HH_MineQuarry, HH_MineInc, (HH_MineInc/HH_Totin)*100 AS HH_MineIncPer, HH_Construct, HH_ConstrInc, (HH_ConstrInc/HH_Totin)*100 AS HH_ConstrIncPer, HH_NEC, HH_EntrNECInc, (HH_EntrNECInc/HH_Totin)*100 AS HH_EntrNECIncPer, HH_EntreprInc, (HH_EntreprInc/HH_Totin)*100 AS HH_EntreprIncPer, \n\
			HH_SalWagInc, (HH_SalWagInc/HH_Totin)*100 AS HH_SalWagIncPer, HH_NetShCrpInc, (HH_NetShCrpInc/HH_Totin)*100 AS HH_NetShCrpIncPer, HH_SUPFInc, (HH_SupfInc/HH_Totin)*100 AS HH_SupfIncPer, HH_SUPRInc, (HH_SuprInc/HH_Totin)*100 AS HH_SuprIncPer, HH_RENTVInc, (HH_RentvInc/HH_Totin)*100 AS HH_RentvIncPer, HH_InterestInc, (HH_InterestInc/HH_Totin)*100 AS HH_InterestIncPer, HH_PensionInc, (HH_PensionInc/HH_Totin)*100 AS HH_PensionIncPer, HH_DividendInc, (HH_DividendInc/HH_Totin)*100 AS HH_DividendIncPer, HH_OTHSNECInc, (HH_OthsNECInc/HH_Totin)*100 AS HH_OthsNECIncPer, HH_OthSrcInc, (HH_OthSrcInc/HH_Totin)*100 AS HH_OthSrcIncPer, \n\
			HH_TOTIN, HH_PCI, hsize, povThresh, subThresh, If((HH_PCI<povThresh),1,0) AS HH_PovP, If((HH_PCI<subThresh),1,0) AS HH_SubP, HH_fshort_3mos, If((HH_fshort_3mos=1),1,0) AS HH_fshort \n\
			FROM (SELECT hpq_hh.regn, hpq_hh.prov, hpq_hh.mun, hpq_hh.brgy, hpq_hh.purok, hpq_hh.hcn, CONCAT(regn, prov, mun, brgy, purok, hcn) AS hhID, hsize, \n\
			IfNull(SUM(If((mdeadage<=5 And (mdeadsx=1 OR mdeadsx=2)),1,0)), 0) AS death05, IfNull(SUM(If((mdeadage<=5 And mdeadsx=1),1,0)), 0) AS death05_male, IfNull(SUM(If((mdeadage<=5 And mdeadsx=2),1,0)), 0) AS death05_female, \n\
			IfNull(SUM(If((mdeadsx=2 And mdeady=8 And (mdeadage>=15 AND mdeadage<=49)),1,0)), 0) AS deathpreg, \n\
			If((water=1 Or water=2 Or water=3 Or water=4 Or water=5 Or water=6 Or water=10),1,0) AS HH_sws, hpq_hh.WATER AS WaterFac, If((toil=1 Or toil=2 Or toil=3),1,0) AS HH_stf, hpq_hh.TOIL AS ToilFac, \n\
			If((tenur=1 Or tenur=2 Or tenur=3 Or tenur=4 Or tenur=6 or tenur=8),1,0) AS HH_ntSquat, hpq_hh.TENUR AS TenurStat, If((wall=1 Or wall=2 Or wall=4 Or wall=5),1,0) AS HH_ntmsw, hpq_hh.WALL AS TypeWall, If((roof=1 Or roof=2 Or roof=4 Or roof=5),1,0) AS HH_ntmsr, hpq_hh.ROOF AS TypeRoof, If((welec=1),1,0) AS HH_welec, \n\
			If((TV=1),1,0) AS HH_wTV, If((REF=1),1,0) AS HH_wREF, If((IRON=1),1,0) AS HH_wIRON, If((STOVE=1),1,0) AS HH_wSTOVE, If((WMACH=1),1,0) AS HH_wWMACH, If((MICROW=1),1,0) AS HH_wMICROW, If((AIRC=1),1,0) AS HH_wAIRC, If((CAR=1),1,0) AS HH_wCAR, \n\
			If(CROPIND=1,1,0) AS HH_CropFarm, If(cropind=1,cropincsh+cropinknd,0) AS HH_CropInc, If(poultind=1,1,0) AS HH_Poult, If(poultind=1,pouincsh+pouinknd,0) AS HH_PoultInc, If(fishind=1,1,0) AS HH_FishAqua, If(fishind=1,fishincsh+fishinknd,0) AS HH_FishInc, If(forind=1,1,0) AS HH_Forest, If(forind=1,forincsh+forinknd,0) AS HH_ForInc, If(salind=1,1,0) AS HH_SaleRetail, If(salind=1,salincsh+salinknd,0) AS HH_SaleInc, If(manind=1,1,0) AS HH_Manufacture, If(manind=1,manincsh+maninknd,0) AS HH_ManufInc, If(servind=1,1,0) AS HH_Services, If(servind=1,servincsh+servinknd,0) AS HH_ServInc, If(trnind=1,1,0) AS HH_TransComm, If(trnind=1,trnincsh+trninknd,0) AS HH_TransInc, If(minind=1,1,0) AS HH_MineQuarry, If(minind=1,minincsh+mininknd,0) AS HH_MineInc, If(cnsind=1,1,0) AS HH_Construct, If(cnsind=1,cnsincsh+cnsinknd,0) AS HH_ConstrInc, If(eothind=1,1,0) AS HH_NEC, If(eothind=1,eoincsh+eoinknd,0) AS HH_EntrNECInc, etotcsh+etotknd AS HH_EntreprInc, \n\
			wagcsh+wagknd AS HH_SalWagInc, agscsh+agsknd AS HH_NetShCrpInc, supfcsh+supfknd AS HH_SUPFInc, suprcsh+suprknd AS HH_SUPRInc, rentvcsh+rentvknd AS HH_RENTVInc, intrcsh+intrknd AS HH_InterestInc, pencsh+penknd AS HH_PensionInc, divcsh+divknd AS HH_DividendInc, othscsh+othsknd AS HH_OTHSNECInc, OTHSTOTCSH+OTHSTOTKND AS HH_OthSrcInc, \n\
			TOTIN AS HH_TOTIN, totin/hsize AS HH_PCI, If(hpq_hh.urb=1, `~thresh`.`povthreshr`, `~thresh`.`povthreshu`) as `povThresh`, If(hpq_hh.urb=1, `~thresh`.`subthreshr`, `~thresh`.`subthreshu`) as `subThresh`, If((FSHORT=1),1,0) AS HH_fshort_3mos \n\
			FROM hpq_hh INNER JOIN `~thresh` ON (`prov`=`threshprov`) LEFT JOIN `hpq_death` USING (regn, prov, mun, brgy, purok, hcn) GROUP BY regn, prov, mun, brgy, purok, hcn) as hpq_hh_temp LEFT JOIN hpq_crime USING (regn, prov, mun, brgy, purok, hcn) GROUP BY regn, prov, mun, brgy, purok, hcn) as HH_Ind_Temp;");
			//"FROM (hpq_hh INNER JOIN `~thresh` ON (`int_mo`=`month` AND `int_year`=`year`))) as hpq_hh_temp) as HH_Ind_Temp;");	

	default:

		AfxMessageBox(_T("To be updated!"));
		return _T("");
		break;
	
	}


	return sSQL;

}


int CStatSimElt::ProcInd(long qnrID)
{
	m_qnrID=qnrID;


	//drop the table
	DeleteInd();

	//if (currElt==HH) {
	//	FILE *pF = fopen("d:\\my.sql", "w");
	//	fwprintf(pF, sSQLInd());
	//	fclose(pF);
	//}

	//execute the ind sql
	pEltDB->ExecuteSQL(sSQLInd(), TRUE);

	ConfigTable();

	if (currElt==HH) {
		
		int nRS_qnr = 0, nRS_ind = 0;
		CStatSimRS *pRS = NULL;
		
		CString sSQL;
		
		sSQL = _T("SELECT `hcn` FROM `hpq_hh`;");
		pRS = new CStatSimRS( pEltDB, sSQL );
		//pRS = new CStatSimRS( pEltDB, (LPCSTR) _MBCS("hpq_hh") );
		nRS_qnr = pRS->GetRecordCount();
		delete pRS; pRS = NULL;

		sSQL = _T("SELECT `hcn` FROM `hh_ind`;");
		pRS = new CStatSimRS( pEltDB, sSQL );
		//pRS = new CStatSimRS( pEltDB, (LPCSTR) _MBCS("hh_ind") );
		nRS_ind = pRS->GetRecordCount();
		delete pRS; pRS = NULL;

		//Test if there is no matching record
		if (nRS_ind==0) {
			AfxMessageBox(_T("The reference period of the data imported is not included in the given reference period in the thresholds.  Processing cannot continue."));
			return 0;
		}

		else {
			//Test if there are records not included
			if (nRS_qnr > nRS_ind) {
				AfxMessageBox(_T("Warning: The number of observations in processed data differs with the number of observations in the raw data."));
				return 1;
			}
			else {
				return 1;
			}
		}

		return 1;
	}
}

void CStatSimElt::ConfigTable()
{

	pEltDB->ExecuteSQL(sSQLConfig(), FALSE);
	
	if (currElt==HH)
		AddPrimaryKey( (CString) _T("_ind"), (CString) Attr(IDField) );

}

void CStatSimElt::CrCCI()
{

	CString sSQL;
	
	sSQL.Format(_T("DROP TABLE IF EXISTS `%s_CCI`"), (CString) Attr(element) );

	pEltDB->ExecuteSQL( sSQL );

	pEltDB->ExecuteSQL( sSQLCCI() );

}

void CStatSimElt::CrLogit(float cutoff)
{

	if (currElt!=HH) {
		//AfxMessageBox(_T("Not applicable."));
		//return;
	}

	CString sSQL;
	
	sSQL.Format(_T("DROP TABLE IF EXISTS `%s_Logit`"), (CString) Attr(element) );

	pEltDB->ExecuteSQL( sSQL );

	pEltDB->ExecuteSQL( sSQLLogit(cutoff) );

}
void CStatSimElt::CrCDP(long qnrID)
{

	if (currElt==HH || currElt==MEM) {
		//AfxMessageBox(_T("Not applicable."));
		return;
	}

	m_qnrID= qnrID;

	if (m_qnrID!=120110100 && m_qnrID!=1020100100 && m_qnrID!=1020130100 && m_qnrID!=1020130101 && m_qnrID!=1020135100 && m_qnrID!=1020135200 && m_qnrID!=1020135300 && m_qnrID!=1020135400 && m_qnrID!=1020135500) {
		AfxMessageBox(_T("This function is not yet available for this questionnaire."));
		return;
	}

	CStatSimRS *pRS = 0;

	CString sSQL, sFrTable, sToTable, sE;
	sE = Attr(element);

}
void CStatSimElt::CrCCRI(long qnrID)
{
	m_qnrID = qnrID;

	if (currElt==HH || currElt==MEM) {
		//AfxMessageBox(_T("Not applicable."));
		return;
	}

	if (m_qnrID!=120110100 && m_qnrID!=1020100100 && m_qnrID!=1020130100 && m_qnrID!=1020130101 && m_qnrID!=1020135100 && m_qnrID!=1020135200 && m_qnrID!=1020135300 && m_qnrID!=1020135400 && m_qnrID!=1020135500) {
		AfxMessageBox(_T("This function is not available for this questionnaire."));
		return;
	}

	CStatSimRS *pRS = 0;

	CString sSQL, sFrTable, sToTable, sE;
	sE = Attr(element);

	//decrease in harvest
	sSQL = "DROP TABLE IF EXISTS `cropind`;";
	pEltDB->ExecuteSQL(sSQL);

	sSQL = "CREATE TABLE `cropind` \n\
		SELECT hpq_hh.urb, hpq_hh.regn, hpq_hh.prov, hpq_hh.mun, hpq_hh.brgy, hpq_hh.purok, hpq_hh.hcn, \n\
		CONCAT(`hpq_hh`.`regn`, `hpq_hh`.`prov`, `hpq_hh`.`mun`, `hpq_hh`.`brgy`, `hpq_hh`.`purok`, `hpq_hh`.`hcn`) as `hhID`, \n\
		if(`cropind`=1,1,0) AS cropind, if(`cropind`=1 AND `u_amt_harv`=1,1,0) AS u_amt_harv \n\
		FROM hpq_hh;";

	pEltDB->ExecuteSQL(sSQL);

	CrHHInd(HPQ_RAW, _MBCS("cropind"), _MBCS("tothh"), _MBCS("cropind"), _MBCS("totpop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE);

	//decrease in harvest
	CrHHInd(HPQ_RAW, _MBCS("u_amt_harv"), _MBCS("cropind"), _MBCS("cropind"), _MBCS("cropind"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE);
	sSQL = "DROP TABLE IF EXISTS `cropind`;";
	pEltDB->ExecuteSQL(sSQL);

	if (currElt<=BRGY) {	
		//cause of low harvest
		CrHHIndCat(HPQ_RAW, _MBCS("u_low_harv"), _MBCS("u_amt_harv"), TRUE, _MBCS("hpq_hh"), _MBCS("u_amt_harv"), 
			_MBCS("CONCAT(`hpq_hh`.`regn`, `hpq_hh`.`prov`, `hpq_hh`.`mun`, `hpq_hh`.`brgy`, `hpq_hh`.`purok`, `hpq_hh`.`hcn`)"),
			_MBCS("`hpq_hh`.`u_amt_harv`=1"));

	}

	//decrease in harvest
	sSQL = "DROP TABLE IF EXISTS `poultind`;";
	pEltDB->ExecuteSQL(sSQL);

	sSQL = "CREATE TABLE `poultind` \n\
		SELECT hpq_hh.urb, hpq_hh.regn, hpq_hh.prov, hpq_hh.mun, hpq_hh.brgy, hpq_hh.purok, hpq_hh.hcn, \n\
		CONCAT(`hpq_hh`.`regn`, `hpq_hh`.`prov`, `hpq_hh`.`mun`, `hpq_hh`.`brgy`, `hpq_hh`.`purok`, `hpq_hh`.`hcn`) as `hhID`, \n\
		if(`poultind`=1,1,0) AS poultind, if(`poultind`=1 AND `u_amt_lve`=1,1,0) AS u_amt_lve \n\
		FROM hpq_hh;";

	pEltDB->ExecuteSQL(sSQL);

	CrHHInd(HPQ_RAW, _MBCS("poultind"), _MBCS("tothh"), _MBCS("poultind"), _MBCS("totpop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE);

	//decrease in harvest (livestock)
	CrHHInd(HPQ_RAW, _MBCS("u_amt_lve"), _MBCS("poultind"), _MBCS("poultind"), _MBCS("poultind"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE);
	sSQL = "DROP TABLE IF EXISTS `poultind`;";
	pEltDB->ExecuteSQL(sSQL);

	if (currElt<=BRGY) {	
		//cause of low harvest
		CrHHIndCat(HPQ_RAW, _MBCS("u_low_lve"), _MBCS("u_amt_lve"), TRUE, _MBCS("hpq_hh"), _MBCS("u_amt_lve"), 
			_MBCS("CONCAT(`hpq_hh`.`regn`, `hpq_hh`.`prov`, `hpq_hh`.`mun`, `hpq_hh`.`brgy`, `hpq_hh`.`purok`, `hpq_hh`.`hcn`)"),
			_MBCS("`hpq_hh`.`u_amt_lve`=1"));

	}

	//decrease in harvest
	sSQL = "DROP TABLE IF EXISTS `fishind`;";
	pEltDB->ExecuteSQL(sSQL);

	sSQL = "CREATE TABLE `fishind` \n\
		SELECT hpq_hh.urb, hpq_hh.regn, hpq_hh.prov, hpq_hh.mun, hpq_hh.brgy, hpq_hh.purok, hpq_hh.hcn, \n\
		CONCAT(`hpq_hh`.`regn`, `hpq_hh`.`prov`, `hpq_hh`.`mun`, `hpq_hh`.`brgy`, `hpq_hh`.`purok`, `hpq_hh`.`hcn`) as `hhID`, \n\
		if(`fishind`=1,1,0) AS fishind, if(`fishind`=1 AND `u_amt_fish`=1,1,0) AS u_amt_fish \n\
		FROM hpq_hh;";

	pEltDB->ExecuteSQL(sSQL);

	CrHHInd(HPQ_RAW, _MBCS("fishind"), _MBCS("tothh"), _MBCS("fishind"), _MBCS("totpop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE);

	//decrease in harvest
	CrHHInd(HPQ_RAW, _MBCS("u_amt_fish"), _MBCS("fishind"), _MBCS("fishind"), _MBCS("fishind"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE);
	sSQL = "DROP TABLE IF EXISTS `fishind`;";
	pEltDB->ExecuteSQL(sSQL);

	if (currElt<=BRGY) {	
		//cause of low harvest
		CrHHIndCat(HPQ_RAW, _MBCS("u_low_fish"), _MBCS("u_amt_fish"), TRUE, _MBCS("hpq_hh"), _MBCS("u_amt_fish"), 
			_MBCS("CONCAT(`hpq_hh`.`regn`, `hpq_hh`.`prov`, `hpq_hh`.`mun`, `hpq_hh`.`brgy`, `hpq_hh`.`purok`, `hpq_hh`.`hcn`)"),
			_MBCS("`hpq_hh`.`u_amt_fish`=1"));

	}

	//calamity

	if (currElt<=BRGY) {	
		if (m_qnrID!=1020130100 && m_qnrID!=1020130101 && m_qnrID!=1020135100 && m_qnrID!=1020135200 && m_qnrID!=1020135300 && m_qnrID!=1020135400 && m_qnrID!=1020135500) {
		CrHHIndCat(HPQ_RAW, _MBCS("calam_type"), _MBCS("tothh"), TRUE, _MBCS("hpq_calam"), _MBCS("totpop"), 
			_MBCS("CONCAT(`hpq_calam`.`regn`, `hpq_calam`.`prov`, `hpq_calam`.`mun`, `hpq_calam`.`brgy`, `hpq_calam`.`purok`, `hpq_calam`.`hcn`)"),
			_MBCS("`hpq_calam`.`calam_type_ind`=1"));
		}
		else {
			std::vector<CString> sVars, sLabels;
			
			//asset
			sVars.push_back(_T("calam1"));
			sVars.push_back(_T("calam2"));
			sVars.push_back(_T("calam3"));
			sVars.push_back(_T("calam4"));
			sVars.push_back(_T("calam5"));
			sVars.push_back(_T("calam6"));
			sVars.push_back(_T("calam7"));
			sVars.push_back(_T("calam8"));
			sVars.push_back(_T("calam9"));
			sVars.push_back(_T("calam10"));
			sVars.push_back(_T("calam11"));
			
			sLabels.push_back(_T("Typhoon"));
			sLabels.push_back(_T("Flood"));
			sLabels.push_back(_T("Drought"));
			sLabels.push_back(_T("Earthquake"));
			sLabels.push_back(_T("Volcanic eruption"));
			sLabels.push_back(_T("Landslide"));
			sLabels.push_back(_T("Tsunami"));
			sLabels.push_back(_T("Fire"));
			sLabels.push_back(_T("Forest fire"));
			sLabels.push_back(_T("Arm Conflict"));
			sLabels.push_back(_T("Others"));
			
			CrHHIndCatEx(_T("calam_type"), sVars, sLabels);


	}
	}
	//climate
	sSQL = "DROP TABLE IF EXISTS `climate`;";
	pEltDB->ExecuteSQL(sSQL);

	sSQL = "CREATE TABLE `climate` \n\
		SELECT hpq_hh.urb, hpq_hh.regn, hpq_hh.prov, hpq_hh.mun, hpq_hh.brgy, hpq_hh.purok, hpq_hh.hcn, \n\
		CONCAT(`hpq_hh`.`regn`, `hpq_hh`.`prov`, `hpq_hh`.`mun`, `hpq_hh`.`brgy`, `hpq_hh`.`purok`, `hpq_hh`.`hcn`) as `hhID`, \n\
		if(`temp_humid`=1,1,0) AS temp_humid, if(`elec_loss`=1,1,0) AS elec_loss, if(`sea_level`=1,1,0) AS sea_lvl_rise, \n\
		if(`water_supply`=1,1,0) AS wtr_supp_dec, if(`flood_freq`=1,1,0) AS flood_freq, if(`drought`=1,1,0) AS drought, \n\
		if(`evac_mveout`=1,1,0) AS evac_mveout, if(`disas_prep`=1,1,0) AS disas_prep \n\
		FROM hpq_hh;";

	pEltDB->ExecuteSQL(sSQL);

	CrHHInd(HPQ_RAW, _MBCS("temp_humid"), _MBCS("tothh"), _MBCS("climate"), _MBCS("totpop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE);
	CrHHInd(HPQ_RAW, _MBCS("elec_loss"), _MBCS("tothh"), _MBCS("climate"), _MBCS("totpop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE);
	CrHHInd(HPQ_RAW, _MBCS("sea_lvl_rise"), _MBCS("tothh"), _MBCS("climate"), _MBCS("totpop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE);
	CrHHInd(HPQ_RAW, _MBCS("wtr_supp_dec"), _MBCS("tothh"), _MBCS("climate"), _MBCS("totpop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE);
	CrHHInd(HPQ_RAW, _MBCS("flood_freq"), _MBCS("tothh"), _MBCS("climate"), _MBCS("totpop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE);
	CrHHInd(HPQ_RAW, _MBCS("drought"), _MBCS("tothh"), _MBCS("climate"), _MBCS("totpop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE);
	CrHHInd(HPQ_RAW, _MBCS("evac_mveout"), _MBCS("tothh"), _MBCS("climate"), _MBCS("totpop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE);
	CrHHInd(HPQ_RAW, _MBCS("disas_prep"), _MBCS("tothh"), _MBCS("climate"), _MBCS("totpop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE);

	sSQL = "DROP TABLE IF EXISTS `climate`;";
	pEltDB->ExecuteSQL(sSQL);
	if (currElt<=BRGY) {	

		if (m_qnrID!=1020130100 && m_qnrID!=1020130101 && m_qnrID!=1020135100 && m_qnrID!=1020135200 && m_qnrID!=1020135300 && m_qnrID!=1020135400 && m_qnrID!=1020135500) {

		//preparedness kit
		CrHHIndCat(HPQ_RAW, _MBCS("kit_type"), _MBCS("tothh"), TRUE, _MBCS("hpq_disaster"), _MBCS("totpop"), 
			_MBCS("CONCAT(`hpq_disaster`.`regn`, `hpq_disaster`.`prov`, `hpq_disaster`.`mun`, `hpq_disaster`.`brgy`, `hpq_disaster`.`purok`, `hpq_disaster`.`hcn`)"),
			_MBCS("`hpq_disaster`.`kit_ind`=1"));

		//insurance
		if (m_qnrID==120110100){
		CrHHIndCat(HPQ_RAW, _MBCS("ins_for"), _MBCS("tothh"), TRUE, _MBCS("hpq_insurance"), _MBCS("totpop"), 
			_MBCS("CONCAT(`hpq_insurance`.`regn`, `hpq_insurance`.`prov`, `hpq_insurance`.`mun`, `hpq_insurance`.`brgy`, `hpq_insurance`.`purok`, `hpq_insurance`.`hcn`)"),
			_MBCS("`hpq_insurance`.`ins_ind`=1"));

		}
		}
		else {
			std::vector<CString> sVars, sLabels;
			
			//asset
			sVars.push_back(_T("dp_kit1"));
			sVars.push_back(_T("dp_kit2"));
			sVars.push_back(_T("dp_kit3"));
			sVars.push_back(_T("dp_kit4"));
			sVars.push_back(_T("dp_kit5"));
			sVars.push_back(_T("dp_kit6"));
			sVars.push_back(_T("dp_kit7"));
			sVars.push_back(_T("dp_kit8"));
			sVars.push_back(_T("dp_kit9"));
			sVars.push_back(_T("dp_kit10"));
			sVars.push_back(_T("dp_kit11"));
			sVars.push_back(_T("dp_kit12"));
			sVars.push_back(_T("dp_kit13"));
			
			sLabels.push_back(_T("Water"));
			sLabels.push_back(_T("Food (canned goods, biscuit, bread)"));
			sLabels.push_back(_T("Matches/Lighter"));
			sLabels.push_back(_T("Flashlight/Emergency Light"));
			sLabels.push_back(_T("Radio/Transistor"));
			sLabels.push_back(_T("Candle"));
			sLabels.push_back(_T("Medical Kit"));
			sLabels.push_back(_T("Whistle"));
			sLabels.push_back(_T("Clothes"));
			sLabels.push_back(_T("Blanket"));
			sLabels.push_back(_T("Battery"));
			sLabels.push_back(_T("Important Documents"));
			sLabels.push_back(_T("Other type"));
			
			CrHHIndCatEx(_T("kit_type"), sVars, sLabels);
		}

	
	}

}

void CStatSimElt::CrBPQTabs()
{
	if (currElt==HH || currElt==MEM || currElt==PUROK) {
		//AfxMessageBox(_T("Not applicable."));
		return;
	}
	
	CString sSQL;
	
/*	sSQL = "DROP TABLE IF EXISTS `brgy_area`;";
	pEltDB->ExecuteSQL(sSQL);

	sSQL = "CREATE TABLE `brgy_area` \n\
		SELECT bpq_brgy.urb, bpq_brgy.regn, bpq_brgy.prov, bpq_brgy.mun, bpq_brgy.brgy, \n\
		CONCAT(`bpq_brgy`.`regn`, `bpq_brgy`.`prov`, `bpq_brgy`.`mun`, `bpq_brgy`.`brgy`) as `brgyID`, \n\
		bpq_brgy.tot_res_area, bpq_brgy.tot_com_area \n\
		FROM bpq_brgy;";

	pEltDB->ExecuteSQL(sSQL);

	CrHHInd(HPQ_RAW, _T("tot_res_area"), _T("totmem"), _T("bpq_brgy"), _T("totpop"), _T("brgyID"), 
		_T("SUM"), 1,
		FALSE);

	sSQL = "DROP TABLE IF EXISTS `brgy_area`;";
	pEltDB->ExecuteSQL(sSQL);

*/
	std::vector<CString> sVars, sLabels;

	//entrepreneurial
	sVars.push_back(_T("tot_res_area"));
	sVars.push_back(_T("tot_com_area"));
	sVars.push_back(_T("tot_ind_area"));
	sVars.push_back(_T("tot_agr_area"));
	sVars.push_back(_T("tot_min_area"));
	sVars.push_back(_T("tot_tim_area"));
	sVars.push_back(_T("tot_spe_area"));
	sVars.push_back(_T("tot_land_area"));

	sLabels.push_back(_T("Total residential area"));
	sLabels.push_back(_T("Total commercial area"));
	sLabels.push_back(_T("Total industrial area"));
	sLabels.push_back(_T("Total agricultural area"));
	sLabels.push_back(_T("Total mineral area"));
	sLabels.push_back(_T("Total timberland area"));
	sLabels.push_back(_T("Total special classes area"));
	sLabels.push_back(_T("Total land area"));

	CrBrgyIndCatEx(_T("bpq_brgy"), _T("area"), sVars, sLabels);

	CrHHIndCat(HPQ_RAW, _MBCS("hf_type"), _MBCS("tothh"), TRUE, _MBCS("bpq_health"), _MBCS("Totpop"), 
		_MBCS("CONCAT(`bpq_health`.`regn`, `bpq_health`.`prov`, `bpq_health`.`mun`, `bpq_health`.`brgy`)"),
		_MBCS("`bpq_health`.`hf_ind`=1"), _MBCS("hf_no"));

/*	CrHHIndCat(HPQ_RAW, _MBCS("hf_type"), _MBCS("tothh"), TRUE, _MBCS("bpq_health"), _MBCS("Totpop"), 
		_MBCS("CONCAT(`bpq_health`.`regn`, `bpq_health`.`prov`, `bpq_health`.`mun`, `bpq_health`.`brgy`)"),
		_MBCS("`bpq_health`.`hf_sdw`=1 AND `bpq_health`.`hf_ind`=1"), _MBCS("hf_no"));

	CrHHIndCat(HPQ_RAW, _MBCS("hf_type"), _MBCS("tothh"), TRUE, _MBCS("bpq_health"), _MBCS("Totpop"), 
		_MBCS("CONCAT(`bpq_health`.`regn`, `bpq_health`.`prov`, `bpq_health`.`mun`, `bpq_health`.`brgy`)"),
		_MBCS("`bpq_health`.`hf_st`=1 AND `bpq_health`.`hf_ind`=1"), _MBCS("hf_no"));
*/
	CrHHIndCat(HPQ_RAW, _MBCS("sf_type"), _MBCS("tothh"), TRUE, _MBCS("bpq_service"), _MBCS("Totpop"), 
		_MBCS("CONCAT(`bpq_service`.`regn`, `bpq_service`.`prov`, `bpq_service`.`mun`, `bpq_service`.`brgy`)"),
		_MBCS("`bpq_service`.`sf_ind`=1"), _MBCS("sf_no"));

/*	CrHHIndCat(HPQ_RAW, _MBCS("sf_type"), _MBCS("tothh"), TRUE, _MBCS("bpq_service"), _MBCS("Totpop"), 
		_MBCS("CONCAT(`bpq_service`.`regn`, `bpq_service`.`prov`, `bpq_service`.`mun`, `bpq_service`.`brgy`)"),
		_MBCS("`bpq_service`.`sf_sdw`=1 AND `bpq_service`.`sf_ind`=1"), _MBCS("sf_no"));

	CrHHIndCat(HPQ_RAW, _MBCS("sf_type"), _MBCS("tothh"), TRUE, _MBCS("bpq_service"), _MBCS("Totpop"), 
		_MBCS("CONCAT(`bpq_service`.`regn`, `bpq_service`.`prov`, `bpq_service`.`mun`, `bpq_service`.`brgy`)"),
		_MBCS("`bpq_service`.`sf_st`=1 AND `bpq_service`.`sf_ind`=1"), _MBCS("sf_no"));
*/
	CrHHIndCat(HPQ_RAW, _MBCS("ef_type"), _MBCS("tothh"), TRUE, _MBCS("bpq_education"), _MBCS("Totpop"), 
		_MBCS("CONCAT(`bpq_education`.`regn`, `bpq_education`.`prov`, `bpq_education`.`mun`, `bpq_education`.`brgy`)"),
		_MBCS("`bpq_education`.`ef_ind`=1"), _MBCS("ef_no"));

/*	CrHHIndCat(HPQ_RAW, _MBCS("ef_type"), _MBCS("tothh"), TRUE, _MBCS("bpq_education"), _MBCS("Totpop"), 
		_MBCS("CONCAT(`bpq_education`.`regn`, `bpq_education`.`prov`, `bpq_education`.`mun`, `bpq_education`.`brgy`)"),
		_MBCS("`bpq_education`.`ef_sdw`=1 AND `bpq_education`.`ef_ind`=1"), _MBCS("ef_no"));

	CrHHIndCat(HPQ_RAW, _MBCS("ef_type"), _MBCS("tothh"), TRUE, _MBCS("bpq_education"), _MBCS("Totpop"), 
		_MBCS("CONCAT(`bpq_education`.`regn`, `bpq_education`.`prov`, `bpq_education`.`mun`, `bpq_education`.`brgy`)"),
		_MBCS("`bpq_education`.`ef_st`=1 AND `bpq_education`.`ef_ind`=1"), _MBCS("ef_no"));

*/	CrHHIndCat(HPQ_RAW, _MBCS("pt_type"), _MBCS("tothh"), TRUE, _MBCS("bpq_public_transport"), _MBCS("Totpop"), 
		_MBCS("CONCAT(`bpq_public_transport`.`regn`, `bpq_public_transport`.`prov`, `bpq_public_transport`.`mun`, `bpq_public_transport`.`brgy`)"),
		_MBCS("`bpq_public_transport`.`pt_ind`=1"));

	sVars.clear();
	sLabels.clear();
	sVars.push_back(_T("conc_length"));
	sVars.push_back(_T("asph_length"));
	sVars.push_back(_T("grav_length"));
	sVars.push_back(_T("natu_length"));

	sLabels.push_back(_T("Concrete"));
	sLabels.push_back(_T("Asphalt"));
	sLabels.push_back(_T("Gravel"));
	sLabels.push_back(_T("Natural / earth"));

	CrBrgyIndCatEx(_T("bpq_brgy"), _T("road_length"), sVars, sLabels);

	CrHHIndCat(HPQ_RAW, _MBCS("wf_type"), _MBCS("tothh"), TRUE, _MBCS("bpq_water"), _MBCS("Totpop"), 
		_MBCS("CONCAT(`bpq_water`.`regn`, `bpq_water`.`prov`, `bpq_water`.`mun`, `bpq_water`.`brgy`)"),
		_MBCS("`bpq_water`.`wf_ind`=1"), _MBCS("wf_total"));

	CrHHIndCat(HPQ_RAW, _MBCS("gd_type"), _MBCS("tothh"), TRUE, _MBCS("bpq_garbage"), _MBCS("Totpop"), 
		_MBCS("CONCAT(`bpq_garbage`.`regn`, `bpq_garbage`.`prov`, `bpq_garbage`.`mun`, `bpq_garbage`.`brgy`)"),
		_MBCS("`bpq_garbage`.`gd_ind`=1"));

	CrHHIndCat(HPQ_RAW, _MBCS("es_type"), _MBCS("tothh"), TRUE, _MBCS("bpq_electricity"), _MBCS("Totpop"), 
		_MBCS("CONCAT(`bpq_electricity`.`regn`, `bpq_electricity`.`prov`, `bpq_electricity`.`mun`, `bpq_electricity`.`brgy`)"),
		_MBCS("`bpq_electricity`.`es_ind`=1"));


	sVars.clear();
	sLabels.clear();
	sVars.push_back(_T("typhoon"));
	sVars.push_back(_T("flooding"));
	sVars.push_back(_T("drought"));
	sVars.push_back(_T("earthquake"));
	sVars.push_back(_T("volcanic_eruption"));
	sVars.push_back(_T("landslide"));
	sVars.push_back(_T("tsunami"));
	sVars.push_back(_T("fire_in_houses_properties"));
	sVars.push_back(_T("forest_fire"));
	sVars.push_back(_T("epidemic"));
	sVars.push_back(_T("pest_infestation"));
	sVars.push_back(_T("livestock_poultry_disease"));
	sVars.push_back(_T("armed_conflict"));
	sVars.push_back(_T("closure_of_large_firm"));
	sVars.push_back(_T("closure_of_many_small_firm"));
	sVars.push_back(_T("mass_lay_off"));
	sVars.push_back(_T("opening_of_large_firm"));
	sVars.push_back(_T("opening_of_many_small_firm"));

	sLabels.push_back(_T("Typhoon"));
	sLabels.push_back(_T("Flooding"));
	sLabels.push_back(_T("Drought"));
	sLabels.push_back(_T("Earthquake"));
	sLabels.push_back(_T("Volcanic Eruption"));
	sLabels.push_back(_T("Landslide"));
	sLabels.push_back(_T("Tsunami"));
	sLabels.push_back(_T("FIre in houses/properties"));
	sLabels.push_back(_T("Forest Fire"));
	sLabels.push_back(_T("Epidemic"));
	sLabels.push_back(_T("Pest infestation"));
	sLabels.push_back(_T("Livestock/Poultry Diseases"));
	sLabels.push_back(_T("Armed Conflict"));
	sLabels.push_back(_T("Closure of Large Firm"));
	sLabels.push_back(_T("Closure of Many Small Firms"));
	sLabels.push_back(_T("Mass Lay-off"));
	sLabels.push_back(_T("Opening of Large Firm"));
	sLabels.push_back(_T("Opening of Many Small Firms"));

	CrBrgyIndCatEx(_T("bpq_brgy"), _T("sig_events"), sVars, sLabels);

	CrHHIndCat(HPQ_RAW, _MBCS("de_type"), _MBCS("tothh"), TRUE, _MBCS("bpq_disaster"), _MBCS("Totpop"), 
		_MBCS("CONCAT(`bpq_disaster`.`regn`, `bpq_disaster`.`prov`, `bpq_disaster`.`mun`, `bpq_disaster`.`brgy`)"),
		_MBCS("`bpq_disaster`.`de_ind`=1"));

	CrHHIndCat(HPQ_RAW, _MBCS("crime_type"), _MBCS("tothh"), TRUE, _MBCS("bpq_crime"), _MBCS("Totpop"), 
		_MBCS("CONCAT(`bpq_crime`.`regn`, `bpq_crime`.`prov`, `bpq_crime`.`mun`, `bpq_crime`.`brgy`)"),
		_MBCS("`bpq_crime`.`crime_ind`=1"), _MBCS("crime_total"));

}
void CStatSimElt::CrOKI(long qnrID)
{
	m_qnrID= qnrID;

	if (currElt==HH || currElt==MEM) {
		//AfxMessageBox(_T("Not applicable."));
		return;
	}

	if (m_qnrID!=120110100 && m_qnrID!=1020100100 && m_qnrID!=1020130100 && m_qnrID!=1020130101 && m_qnrID!=1020135100 && m_qnrID!=1020135200 && m_qnrID!=1020135300 && m_qnrID!=1020135400 && m_qnrID!=1020135500) {
		AfxMessageBox(_T("This function is not yet available for this questionnaire."));
		return;
	}

	CStatSimRS *pRS = 0;

	CString sSQL, sFrTable, sToTable, sE;
	sE = Attr(element);

	//for Indigenous people
	if (m_qnrID!=1020130100 && m_qnrID!=1020130101 && m_qnrID!=1020135100 && m_qnrID!=1020135200 && m_qnrID!=1020135300 && m_qnrID!=1020135400 && m_qnrID!=1020135500) {
	CrMemInd(HPQ_RAW, _MBCS("ipind"), _MBCS("TotMem"), _MBCS("_Male"), _MBCS("_Female"),
		_MBCS("Mem_Ind"), _MBCS("Totpop") );
	}
	//CrMemInd(HPQ_RAW, _T("ipind"), _T("totmem"), _T("_Male"), _T("_Female"),
	//	_T("mem_ind"), _T("totpop"));
	
/*	//indigenous group
	if (currElt<=BRGY) {
		CrMemIndCat(HPQ_RAW, _T("ipgrp"), _T("ipind"), TRUE, _T("_Male"), _T("_Female"), _T("hpq_mem"), _T("ipind"),
			_T("CONCAT(`hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`)"),
			_T("`hpq_mem`.`ipgrp`=1"));
	}
*/
	//program
/*	if (pLabel!=NULL) {
		pLabel->SetText(_MBCS("Program beneficiaries..."));
	}
*/
	//for rostered programs / assets
	if (currElt<=BRGY) {
		//types of program
		//CrHHIndCat(HPQ_RAW, _MBCS("progtype"), _MBCS("tothh"), TRUE, _MBCS("hpq_prog"), _MBCS("Totpop"), 
		//	_MBCS("CONCAT(`hpq_prog`.`regn`, `hpq_prog`.`prov`, `hpq_prog`.`mun`, `hpq_prog`.`brgy`, `hpq_prog`.`purok`, `hpq_prog`.`hcn`)"),
		//	_MBCS("`hpq_prog`.`progind`=1"));
		if (m_qnrID!=1020130100 && m_qnrID!=1020130101 && m_qnrID!=1020135100 && m_qnrID!=1020135200 && m_qnrID!=1020135300 && m_qnrID!=1020135400 && m_qnrID!=1020135500) {
		CrHHIndCat(HPQ_RAW, _MBCS("prog_type_g"), _MBCS("tothh"), TRUE, _MBCS("hpq_prog_gov"), _MBCS("Totpop"), 
			_MBCS("CONCAT(`hpq_prog_gov`.`regn`, `hpq_prog_gov`.`prov`, `hpq_prog_gov`.`mun`, `hpq_prog_gov`.`brgy`, `hpq_prog_gov`.`purok`, `hpq_prog_gov`.`hcn`)"),
			_MBCS("`hpq_prog_gov`.`progind_g`=1"));
	
		//types of organization
		CrMemIndCat(HPQ_RAW, _MBCS("org_type_1"), _MBCS("mem10ab"), TRUE, _MBCS("_Male"), _MBCS("_Female"), _MBCS("hpq_mem"), _MBCS("demog"),
			_MBCS("CONCAT(`hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`)"),
			_MBCS("`hpq_mem`.`orgind`=1 AND `hpq_mem`.`age_yr`>=10"));

		}
		if (m_qnrID==120110100) {
			//types of assets
		CrHHIndCat(HPQ_RAW, _MBCS("oa_type"), _MBCS("tothh"), TRUE, _MBCS("hpq_owned_asset"), _MBCS("Totpop"), 
			_MBCS("CONCAT(`hpq_owned_asset`.`regn`, `hpq_owned_asset`.`prov`, `hpq_owned_asset`.`mun`, `hpq_owned_asset`.`brgy`, `hpq_owned_asset`.`purok`, `hpq_owned_asset`.`hcn`)"),
			_MBCS("`hpq_owned_asset`.`oa_ind`=1"));
		}
	}

	if (m_qnrID==1020100100 || m_qnrID==1020130100 || m_qnrID==1020130101 || m_qnrID==1020135100 || m_qnrID==1020135200 || m_qnrID==1020135300 || m_qnrID==1020135400 || m_qnrID==1020135500) {

	std::vector<CString> sVars, sLabels;
	//asset
	sVars.push_back(_T("radio"));
	sVars.push_back(_T("tv"));
	sVars.push_back(_T("vplayer"));
	sVars.push_back(_T("stereo"));
	sVars.push_back(_T("karaoke"));
	sVars.push_back(_T("ref"));
	sVars.push_back(_T("efan"));
	sVars.push_back(_T("iron"));
	sVars.push_back(_T("stove"));
	sVars.push_back(_T("wmach"));
	sVars.push_back(_T("microw"));
	sVars.push_back(_T("computer"));
	sVars.push_back(_T("internet"));
	sVars.push_back(_T("celfone"));
	sVars.push_back(_T("telefone"));
	sVars.push_back(_T("airc"));
	sVars.push_back(_T("sewmach"));
	sVars.push_back(_T("car"));
	sVars.push_back(_T("motor"));
	
	if (m_qnrID==1020130100 || m_qnrID==1020130101 || m_qnrID==1020135100 || m_qnrID==1020135200 || m_qnrID==1020135300 || m_qnrID==1020135400 || m_qnrID==1020135500) {
		sVars.push_back(_T("landagri"));
		sVars.push_back(_T("landres"));
		sVars.push_back(_T("landcomm"));
		sVars.push_back(_T("sofa"));
		sVars.push_back(_T("dineset"));
	}
	else {
		sVars.push_back(_T("land"));
	}
	sVars.push_back(_T("own_other"));

	sLabels.push_back(_T("Radio/cassette"));
	sLabels.push_back(_T("TV"));
	sLabels.push_back(_T("VHS/VCD/DVD player"));
	sLabels.push_back(_T("Stereo/component"));
	sLabels.push_back(_T("karaoke"));
	sLabels.push_back(_T("ref"));
	sLabels.push_back(_T("electric fan"));
	sLabels.push_back(_T("electric iron"));
	sLabels.push_back(_T("LPG/gas stove/range"));
	sLabels.push_back(_T("washing machine"));
	sLabels.push_back(_T("microwave oven"));
	sLabels.push_back(_T("computer"));
	sLabels.push_back(_T("Internet Connection"));
	sLabels.push_back(_T("cellphone"));
	sLabels.push_back(_T("Telephone"));
	sLabels.push_back(_T("aircon"));
	sLabels.push_back(_T("sewing machine"));
	sLabels.push_back(_T("vehicles"));
	sLabels.push_back(_T("tricycle/motorcycle"));

	if (m_qnrID==1020130100 || m_qnrID==1020130101 || m_qnrID==1020135100 || m_qnrID==1020135200 || m_qnrID==1020135300 || m_qnrID==1020135400 || m_qnrID==1020135500) {
		sLabels.push_back(_T("Agricultural Land"));
		sLabels.push_back(_T("Residential Land"));
		sLabels.push_back(_T("Commercial Land"));
		sLabels.push_back(_T("Sofa"));
		sLabels.push_back(_T("Dining set"));
	}
	else {
		sLabels.push_back(_T("Land"));
	}
	sLabels.push_back(_T("Other"));

	CrHHIndCatEx(_T("oa_type"), sVars, sLabels);
	}

/*		//assets/////////
		sSQL = "RENAME TABLE `hpq_owned_asset` TO `hpq_owned_asset_temp`;";
		pEltDB->ExecuteSQL(sSQL);

		sSQL = "CREATE TABLE `hpq_owned_asset` \n\
			SELECT hpq_mem.urb, hpq_mem.regn, hpq_mem.prov, hpq_mem.mun, hpq_mem.brgy, hpq_mem.purok, hpq_mem.hcn, hpq_mem.memno, hpq_mem.sex, hpq_owned_asset_temp.asset \n\
			FROM hpq_mem INNER JOIN hpq_owned_asset_temp ON (hpq_mem.regn = hpq_owned_asset_temp.regn) AND (hpq_mem.prov = hpq_owned_asset_temp.prov) AND (hpq_mem.mun = hpq_owned_asset_temp.mun) AND (hpq_mem.brgy = hpq_owned_asset_temp.brgy) AND (hpq_mem.purok = hpq_owned_asset_temp.purok) AND (hpq_mem.hcn = hpq_owned_asset_temp.hcn) AND (hpq_mem.memno = hpq_owned_asset_temp.owner_line);";

		//FILE *pFile = fopen("C:\\mysql.sql"), _T("w");
		//fprintf( pFile, ConstChar(sSQLExec) );
		//fclose(pFile);

		pEltDB->ExecuteSQL(sSQL);

		CrMemIndCat(HPQ_RAW, _T("asset"), _T("totmem"), TRUE, _T("_Male"), _T("_Female"), _T("hpq_owned_asset"), _T("Totpop"),
			_T("CONCAT(`hpq_owned_asset`.`regn`, `hpq_owned_asset`.`prov`, `hpq_owned_asset`.`mun`, `hpq_owned_asset`.`brgy`, `hpq_owned_asset`.`purok`, `hpq_owned_asset`.`hcn`, `hpq_owned_asset`.`memno`)"),
			NULL, _T("hpq_owned_asset"));

		sSQL = "DROP TABLE IF EXISTS `hpq_owned_asset`;";
		pEltDB->ExecuteSQL(sSQL);

		sSQL = "RENAME TABLE `hpq_owned_asset_temp` TO `hpq_owned_asset`;";
		pEltDB->ExecuteSQL(sSQL);
		/////////////assets////////////////////////


		//by pci quintile
		sSQL = "DROP TABLE IF EXISTS `hh_prog_pciquintile`;";
		pEltDB->ExecuteSQL(sSQL);
		sSQL.Format("RENAME TABLE `hh_ind` TO `hh_ind_temp`;");
		pEltDB->ExecuteSQL(sSQL);

		sSQL = "CREATE TABLE `Hh_ind` \n\
			"SELECT Hh_pciile_" + sElt + ".regn, Hh_pciile_" + sElt + ".prov, Hh_pciile_" + sElt + ".mun, Hh_pciile_" + sElt + ".brgy, Hh_pciile_" + sElt + ".purok, Hh_pciile_" + sElt + ".hcn, hh_ind_temp.hhID, \n\
			"Hh_pciile_" + sElt + ".pciquintile AS Hh_prog_pciquintile, `hpq_prog`.`progind` \n\
			"FROM Hh_pciile_" + sElt + " INNER JOIN hpq_prog USING (regn, prov, mun, brgy, purok, hcn) INNER JOIN hh_ind_temp USING (regn, prov, mun, brgy, purok, hcn);";
		
		pEltDB->ExecuteSQL(sSQL);

		//program beneficiaries by income group
		CrHHIndCat(HPQ_RAW, _T("hh_prog_pciquintile"), _T("tothh"), TRUE, _T("hh_ind"), _T("Totpop"), 
			_T("hhID"), _T("`progind`=1"));	
		
		sSQL.Format(_T("RENAME TABLE `hh_ind` TO `hh_prog_pciquintile`;"));
		pEltDB->ExecuteSQL(sSQL);
		sSQL.Format(_T("RENAME TABLE `hh_ind_temp` TO `hh_ind`;"));
		pEltDB->ExecuteSQL(sSQL);

		//by pci quintile - mem
		sSQL = "DROP TABLE IF EXISTS `Mem_prog_pciquintile`;";
		pEltDB->ExecuteSQL(sSQL);
		sSQL.Format("RENAME TABLE `Mem_ind` TO `Mem_ind_temp`;");
		pEltDB->ExecuteSQL(sSQL);

		sSQL = "CREATE TABLE `Mem_ind` \n\
			"SELECT Hh_pciile_" + sElt + ".regn, Hh_pciile_" + sElt + ".prov, Hh_pciile_" + sElt + ".mun, Hh_pciile_" + sElt + ".brgy, Hh_pciile_" + sElt + ".purok, Hh_pciile_" + sElt + ".hcn, mem_ind_temp.memno, mem_ind_temp.memID, mem_ind_temp.sex, \n\
			"Hh_pciile_" + sElt + ".pciquintile AS Mem_prog_pciquintile, `hpq_prog`.`progind` \n\
			"FROM Hh_pciile_" + sElt + " INNER JOIN hpq_prog USING (regn, prov, mun, brgy, purok, hcn) INNER JOIN mem_ind_temp USING (regn, prov, mun, brgy, purok, hcn);";
		
		pEltDB->ExecuteSQL(sSQL);

		//program beneficiaries by income group
		CrMemIndCat(HPQ_RAW, _T("Mem_prog_pciquintile"), _T("totmem"), TRUE, _T("_Male"), _T("_Female"), _T("mem_ind"), _T("Totpop"),
			_T("MemID"), _T("`progind`=1"), _T("mem_ind"));	
		
		sSQL.Format(_T("RENAME TABLE `Mem_ind` TO `Mem_prog_pciquintile`;"));
		pEltDB->ExecuteSQL(sSQL);
		sSQL.Format(_T("RENAME TABLE `Mem_ind_temp` TO `Mem_ind`;"));
		pEltDB->ExecuteSQL(sSQL);
		//////////////////////////////////////////////////////////

	}

*/
	//electoral
/*	if (pLabel!=NULL) {
		pLabel->SetText(_MBCS("Electoral participation..."));
	}
*/
	//voters
	sSQL = "DROP TABLE IF EXISTS `vote_ind`;";
	pEltDB->ExecuteSQL(sSQL);

	sSQL = "CREATE TABLE `vote_ind` \n\
		SELECT hpq_mem.urb, hpq_mem.regn, hpq_mem.prov, hpq_mem.mun, hpq_mem.brgy, hpq_mem.purok, hpq_mem.hcn, hpq_mem.memno, \n\
		CONCAT(`hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`) as `memID`, \n\
		hpq_mem.sex, if(`regvotind`=1 and age_yr>=17,1,0) AS regvote, if(`regvotind`=1 and `age_yr`>=17 And `sex`=1,1,0) AS regvote_male, if(`regvotind`=1 and `age_yr`>=17 And `sex`=2,1,0) AS regvote_female, if(`regvotind`=1 and `age_yr`>=17 And `voted_last_election`=1,1,0) AS electvote, if(`regvotind`=1 and `age_yr`>=17 And `voted_last_election`=1 and `sex`=1,1,0) AS electvote_male, if(`regvotind`=1 and `age_yr`>=17 And `voted_last_election`=1 and `sex`=2,1,0) AS electvote_female \n\
		FROM hpq_mem;";

	pEltDB->ExecuteSQL(sSQL);
	CrMemInd(HPQ_RAW, _MBCS("regvote"), _MBCS("mem17ab"), _MBCS("_Male"), _MBCS("_Female"),
		_MBCS("vote_ind"), _MBCS("Demog"), FALSE, _MBCS("memID"));
	CrMemInd(HPQ_RAW, _MBCS("electvote"), _MBCS("mem17ab"), _MBCS("_Male"), _MBCS("_Female"),
		_MBCS("vote_ind"), _MBCS("Demog"), FALSE, _MBCS("memID"));



	//partials
/*	if (pLabel!=NULL) {
		pLabel->SetText(_MBCS("Core indicator tabulation..."));
	}

	for (int i=0; i<N_COREIND; i++) {
		
		if (currElt<=BRGY) {

			DropPartialCore(i+1);
			CrPartialCore(i+1);

		}

	}

	//migration
	if (pLabel!=NULL) {
		pLabel->SetText(_MBCS("Migration indicators..."));
	}
*/
	if (m_qnrID!=1020130100 && m_qnrID!=1020130101 && m_qnrID!=1020135100 && m_qnrID!=1020135200 && m_qnrID!=1020135300 && m_qnrID!=1020135400 && m_qnrID!=1020135500) {
	CrMemInd(HPQ_RAW, _MBCS("migr"), _MBCS("totmem"), _MBCS("_Male"), _MBCS("_Female"),
		_MBCS("Mem_Ind"), _MBCS("totpop"));
	//migration - country
	CrMemInd(HPQ_RAW, _MBCS("country_migr"), _MBCS("totmem"), _MBCS("_Male"), _MBCS("_Female"),
		_MBCS("Mem_Ind"), _MBCS("totpop"));
	//migration - province
	CrMemInd(HPQ_RAW, _MBCS("prov_migr"), _MBCS("totmem"), _MBCS("_Male"), _MBCS("_Female"),
		_MBCS("Mem_Ind"), _MBCS("totpop"));
	//migration - municipality
	CrMemInd(HPQ_RAW, _MBCS("mun_migr"), _MBCS("totmem"), _MBCS("_Male"), _MBCS("_Female"),
		_MBCS("Mem_Ind"), _MBCS("totpop"));
	//migration - barangay
	CrMemInd(HPQ_RAW, _MBCS("brgy_migr"), _MBCS("totmem"), _MBCS("_Male"), _MBCS("_Female"),
		_MBCS("Mem_Ind"), _MBCS("totpop"));
	
	//social security
	sSQL = "DROP TABLE IF EXISTS `sss_ind`;";
	pEltDB->ExecuteSQL(sSQL);

	sSQL = "CREATE TABLE `sss_ind` \n\
		SELECT hpq_mem.urb, hpq_mem.regn, hpq_mem.prov, hpq_mem.mun, hpq_mem.brgy, hpq_mem.purok, hpq_mem.hcn, hpq_mem.memno, \n\
		CONCAT(`hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`) as `memID`, \n\
		hpq_mem.sex, if(`sss_ind`=1,1,0) AS sss_ind, if(`sss_ind`=1 And `sex`=1,1,0) AS sss_ind_male, if(`sss_ind`=1 And `sex`=2,1,0) AS sss_ind_female \n\
		FROM hpq_mem;";

	pEltDB->ExecuteSQL(sSQL);
	CrMemInd(HPQ_RAW, _MBCS("sss_ind"), _MBCS("mem18ab"), _MBCS("_Male"), _MBCS("_Female"),
		_MBCS("sss_ind"), _MBCS("Demog"), FALSE, _MBCS("memID"));

	sSQL = "DROP TABLE IF EXISTS `sss_ind`;";
	pEltDB->ExecuteSQL(sSQL);

	///////////////////////////////////////
	//board passer

	sSQL = "DROP TABLE IF EXISTS `coll_grad`;";
	pEltDB->ExecuteSQL(sSQL);
	sSQL = "CREATE TABLE `coll_grad` \n\
		SELECT hpq_mem.urb, hpq_mem.regn, hpq_mem.prov, hpq_mem.mun, hpq_mem.brgy, hpq_mem.purok, hpq_mem.hcn, hpq_mem.memno, \n\
		CONCAT(`hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`) as `memID`, \n\
		hpq_mem.sex, if(`educal`>=35,1,0) AS coll_grad, if(`educal`>=35 And `sex`=1,1,0) AS coll_grad_male, if(`educal`>=35  And `sex`=2,1,0) AS coll_grad_female \n\
		FROM hpq_mem;";

	pEltDB->ExecuteSQL(sSQL);
	CrMemInd(HPQ_RAW, _MBCS("coll_grad"), _MBCS("mem18ab"), _MBCS("_Male"), _MBCS("_Female"),
		_MBCS("coll_grad"), _MBCS("Demog"), FALSE, _MBCS("memID"));

	sSQL = "DROP TABLE IF EXISTS `board_passer`;";
	pEltDB->ExecuteSQL(sSQL);

	sSQL = "CREATE TABLE `board_passer` \n\
		SELECT hpq_mem.urb, hpq_mem.regn, hpq_mem.prov, hpq_mem.mun, hpq_mem.brgy, hpq_mem.purok, hpq_mem.hcn, hpq_mem.memno, \n\
		CONCAT(`hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`) as `memID`, \n\
		hpq_mem.sex, if(`board_passer`=1,1,0) AS board_passer, if(`board_passer`=1 And `sex`=1,1,0) AS board_passer_male, if(`board_passer`=1 And `sex`=2,1,0) AS board_passer_female \n\
		FROM hpq_mem;";

	pEltDB->ExecuteSQL(sSQL);
	CrMemInd(HPQ_RAW, _MBCS("board_passer"), _MBCS("coll_grad"), _MBCS("_Male"), _MBCS("_Female"),
		_MBCS("board_passer"), _MBCS("coll_grad"), FALSE, _MBCS("memID"));

	sSQL = "DROP TABLE IF EXISTS `coll_grad`;";
	pEltDB->ExecuteSQL(sSQL);
	sSQL = "DROP TABLE IF EXISTS `board_passer`;";
	pEltDB->ExecuteSQL(sSQL);
	///////////////////////////////
	}
	//solo parent
	sSQL = "DROP TABLE IF EXISTS `solo_parent`;";
	pEltDB->ExecuteSQL(sSQL);

	sSQL = "CREATE TABLE `solo_parent` \n\
		SELECT hpq_mem.urb, hpq_mem.regn, hpq_mem.prov, hpq_mem.mun, hpq_mem.brgy, hpq_mem.purok, hpq_mem.hcn, hpq_mem.memno, \n\
		CONCAT(`hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`) as `memID`, \n\
		hpq_mem.sex, if(`solo_parent`=1,1,0) AS solo_parent, if(`solo_parent`=1 And `sex`=1,1,0) AS solo_parent_male, if(`solo_parent`=1 And `sex`=2,1,0) AS solo_parent_female \n\
		FROM hpq_mem;";

	pEltDB->ExecuteSQL(sSQL);
	CrMemInd(HPQ_RAW, _MBCS("solo_parent"), _MBCS("totmem"), _MBCS("_Male"), _MBCS("_Female"),
		_MBCS("solo_parent"), _MBCS("totpop"), FALSE, _MBCS("memID"));

	sSQL = "DROP TABLE IF EXISTS `solo_parent`;";
	pEltDB->ExecuteSQL(sSQL);

	///////////////////////////////////////

	//persons with disability
	sSQL = "DROP TABLE IF EXISTS `pwd_ind`;";
	pEltDB->ExecuteSQL(sSQL);

	sSQL = "CREATE TABLE `pwd_ind` \n\
		SELECT hpq_mem.urb, hpq_mem.regn, hpq_mem.prov, hpq_mem.mun, hpq_mem.brgy, hpq_mem.purok, hpq_mem.hcn, hpq_mem.memno, \n\
		CONCAT(`hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`) as `memID`, \n\
		hpq_mem.sex, if(`pwd_ind`=1,1,0) AS pwd_ind, if(`pwd_ind`=1 And `sex`=1,1,0) AS pwd_ind_male, if(`pwd_ind`=1 And `sex`=2,1,0) AS pwd_ind_female, \n\
		if(`pwd_ind`=1 AND `pwd_id`=1,1,0) AS pwd_id, if(`pwd_ind`=1 AND `pwd_id`=1 And `sex`=1,1,0) AS pwd_id_male, if(`pwd_ind`=1 And `pwd_id`=1 And `sex`=2,1,0) AS pwd_id_female \n\
		FROM hpq_mem;";

	pEltDB->ExecuteSQL(sSQL);
	CrMemInd(HPQ_RAW, _MBCS("pwd_ind"), _MBCS("totmem"), _MBCS("_Male"), _MBCS("_Female"),
		_MBCS("pwd_ind"), _MBCS("totpop"), FALSE, _MBCS("memID"));
	CrMemInd(HPQ_RAW, _MBCS("pwd_id"), _MBCS("pwd_ind"), _MBCS("_Male"), _MBCS("_Female"),
		_MBCS("pwd_ind"), _MBCS("pwd_ind"), FALSE, _MBCS("memID"));

	sSQL = "DROP TABLE IF EXISTS `pwd_ind`;";
	pEltDB->ExecuteSQL(sSQL);

	if (currElt<=BRGY) {	
		//types of disability
		CrMemIndCat(HPQ_RAW, _MBCS("pwd_type"), _MBCS("pwd_ind"), TRUE, _MBCS("_Male"), _MBCS("_Female"), _MBCS("hpq_mem"), _MBCS("pwd_ind"),
			_MBCS("CONCAT(`hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`)"),
			_MBCS("`hpq_mem`.`pwd_ind`=1"));

		//cause of disability
		if (m_qnrID!=1020130100 && m_qnrID!=1020130101 && m_qnrID!=1020135100 && m_qnrID!=1020135200 && m_qnrID!=1020135300 && m_qnrID!=1020135400 && m_qnrID!=1020135500) {
		CrMemIndCat(HPQ_RAW, _MBCS("pwd_cause"), _MBCS("pwd_ind"), TRUE, _MBCS("_Male"), _MBCS("_Female"), _MBCS("hpq_mem"), _MBCS("pwd_ind"),
			_MBCS("CONCAT(`hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`)"),
			_MBCS("`hpq_mem`.`pwd_ind`=1"));

		//use of PWD ID
		CrMemIndCat(HPQ_RAW, _MBCS("pwd_id_use_1"), _MBCS("pwd_id"), TRUE, _MBCS("_Male"), _MBCS("_Female"), _MBCS("hpq_mem"), _MBCS("pwd_id"),
			_MBCS("CONCAT(`hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`)"),
			_MBCS("`hpq_mem`.`pwd_id`=1"));
		}
	}
	///////////////////////////////////////

	//senior citizens
	sSQL = "DROP TABLE IF EXISTS `scid_ind`;";
	pEltDB->ExecuteSQL(sSQL);

	sSQL = "CREATE TABLE `scid_ind` \n\
		SELECT hpq_mem.urb, hpq_mem.regn, hpq_mem.prov, hpq_mem.mun, hpq_mem.brgy, hpq_mem.purok, hpq_mem.hcn, hpq_mem.memno, \n\
		CONCAT(`hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`) as `memID`, \n\
		hpq_mem.sex, if(`scid_ind`=1,1,0) AS scid_ind, if(`scid_ind`=1 And `sex`=1,1,0) AS scid_ind_male, if(`scid_ind`=1 And `sex`=2,1,0) AS scid_ind_female \n\
		FROM hpq_mem;";

	pEltDB->ExecuteSQL(sSQL);
	CrMemInd(HPQ_RAW, _MBCS("scid_ind"), _MBCS("mem60ab"), _MBCS("_Male"), _MBCS("_Female"),
		_MBCS("scid_ind"), _MBCS("demog"), FALSE, _MBCS("memID"));

	sSQL = "DROP TABLE IF EXISTS `scid_ind`;";
	pEltDB->ExecuteSQL(sSQL);

	if (m_qnrID!=1020130100 && m_qnrID!=1020130101 && m_qnrID!=1020135100 && m_qnrID!=1020135200 && m_qnrID!=1020135300 && m_qnrID!=1020135400 && m_qnrID!=1020135500) {
	if (currElt<=BRGY) {	
		//use of SCID
		CrMemIndCat(HPQ_RAW, _MBCS("scid_use_1"), _MBCS("scid_ind"), TRUE, _MBCS("_Male"), _MBCS("_Female"), _MBCS("hpq_mem"), _MBCS("scid_ind"),
			_MBCS("CONCAT(`hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`)"),
			_MBCS("`hpq_mem`.`scid_ind`=1"));

	}
	}
	///////////////////////////////////////

	if (m_qnrID!=1020130100 && m_qnrID!=1020130101 && m_qnrID!=1020135100 && m_qnrID!=1020135200 && m_qnrID!=1020135300 && m_qnrID!=1020135400 && m_qnrID!=1020135500) {
	//sickness and assistance
	sSQL = "DROP TABLE IF EXISTS `gsick`;";
	pEltDB->ExecuteSQL(sSQL);

	sSQL = "CREATE TABLE `gsick` \n\
		SELECT hpq_hh.urb, hpq_hh.regn, hpq_hh.prov, hpq_hh.mun, hpq_hh.brgy, hpq_hh.purok, hpq_hh.hcn, \n\
		CONCAT(`hpq_hh`.`regn`, `hpq_hh`.`prov`, `hpq_hh`.`mun`, `hpq_hh`.`brgy`, `hpq_hh`.`purok`, `hpq_hh`.`hcn`) as `hhID`, \n\
		if(`gsick`=1,1,0) AS gsick, if(`gsick`=1 And `cure_sick`=1,1,0) AS cure_sick \n\
		FROM hpq_hh;";

	pEltDB->ExecuteSQL(sSQL);

	CrHHInd(HPQ_RAW, _MBCS("gsick"), _MBCS("tothh"), _MBCS("gsick"), _MBCS("totpop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE);
	CrHHInd(HPQ_RAW, _MBCS("cure_sick"), _MBCS("gsick"), _MBCS("gsick"), _MBCS("gsick"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE);

	sSQL = "DROP TABLE IF EXISTS `gsick`;";

	pEltDB->ExecuteSQL(sSQL);
	
	std::vector<CString> sVars;
	std::vector<CString> sLabels;

	//cause of morbidity
	sVars.push_back(_T("s_pneumonia"));
	sVars.push_back(_T("s_bronchitis"));
	sVars.push_back(_T("s_diarrhea"));
	sVars.push_back(_T("s_influenza"));
	sVars.push_back(_T("s_hypertension"));
	sVars.push_back(_T("s_tuberculosis"));
	sVars.push_back(_T("s_heart_ailment"));
	sVars.push_back(_T("s_malaria"));
	sVars.push_back(_T("s_chicken_pox"));
	sVars.push_back(_T("s_dengue_fever"));
	sVars.push_back(_T("s_cholera"));
	sVars.push_back(_T("s_typhoid_fever"));
	sVars.push_back(_T("s_heat_stroke"));
	sVars.push_back(_T("s_asthma"));
	sVars.push_back(_T("other_sick"));

	sLabels.push_back(_T("pneumonia"));
	sLabels.push_back(_T("bronchitis"));
	sLabels.push_back(_T("diarrhea"));
	sLabels.push_back(_T("influenza"));
	sLabels.push_back(_T("hypertension"));
	sLabels.push_back(_T("tuberculosis"));
	sLabels.push_back(_T("heart ailment"));
	sLabels.push_back(_T("malaria"));
	sLabels.push_back(_T("chicken pox"));
	sLabels.push_back(_T("dengue fever"));
	sLabels.push_back(_T("cholera"));
	sLabels.push_back(_T("typhoid fever"));
	sLabels.push_back(_T("heat stroke"));
	sLabels.push_back(_T("asthma"));
	sLabels.push_back(_T("other sickness"));
	CrHHIndCatEx(_T("morb"), sVars, sLabels, _T("gsick"));

	sVars.clear();
	sLabels.clear();
	
	//place received treatment
	sVars.push_back(_T("cure_national"));
	sVars.push_back(_T("cure_provincial"));
	sVars.push_back(_T("cure_district"));
	sVars.push_back(_T("cure_municipal"));
	sVars.push_back(_T("cure_private"));
	sVars.push_back(_T("cure_rural"));
	sVars.push_back(_T("cure_brgy"));
	sVars.push_back(_T("cure_hilot"));
	sVars.push_back(_T("where_cure"));

	sLabels.push_back(_T("national hospital"));
	sLabels.push_back(_T("provincial hospital"));
	sLabels.push_back(_T("district hospital"));
	sLabels.push_back(_T("municipal hospital"));
	sLabels.push_back(_T("private hospital/clinic"));
	sLabels.push_back(_T("rural health units"));
	sLabels.push_back(_T("barangay clinic"));
	sLabels.push_back(_T("hilot/non-medical trained"));
	sLabels.push_back(_T("Others"));
	CrHHIndCatEx(_T("wcure"), sVars, sLabels, _T("cure_sick"));
	sVars.clear();
	sLabels.clear();
	///////////////////////////////////////

	//family planning
	sSQL = "DROP TABLE IF EXISTS `fam_plan`;";
	pEltDB->ExecuteSQL(sSQL);

	sSQL = "CREATE TABLE `fam_plan` \n\
		SELECT hpq_couple.urb, hpq_couple.regn, hpq_couple.prov, hpq_couple.mun, hpq_couple.brgy, hpq_couple.purok, hpq_couple.hcn, \n\
		CONCAT(`hpq_couple`.`regn`, `hpq_couple`.`prov`, `hpq_couple`.`mun`, `hpq_couple`.`brgy`, `hpq_couple`.`purok`, `hpq_couple`.`hcn`) as `hhID`, \n\
		1 as couple, if(`fam_plan`=1,1,0) AS fam_plan \n\
		FROM hpq_couple;";

	pEltDB->ExecuteSQL(sSQL);

	CrHHInd(HPQ_RAW, _MBCS("couple"), _MBCS("totmem"), _MBCS("fam_plan"), _MBCS("totpop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE);
	CrHHInd(HPQ_RAW, _MBCS("fam_plan"), _MBCS("couple"), _MBCS("fam_plan"), _MBCS("couple"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE);

	sSQL = "DROP TABLE IF EXISTS `fam_plan`;";
	pEltDB->ExecuteSQL(sSQL);
	if (currElt<=BRGY) {	
		//type of family planning method
		CrHHIndCat(HPQ_RAW, _MBCS("fp_meth"), _MBCS("fam_plan"), TRUE, _MBCS("hpq_couple"), _MBCS("fam_plan"), 
			_MBCS("CONCAT(`hpq_couple`.`regn`, `hpq_couple`.`prov`, `hpq_couple`.`mun`, `hpq_couple`.`brgy`, `hpq_couple`.`purok`, `hpq_couple`.`hcn`)"),
			_MBCS("`hpq_couple`.`fam_plan`=1"));

	}
	///////////////////////////////////////
	}

	std::vector<CString> sVars, sLabels;
	//waste management
	sVars.push_back(_T("garb_collect"));
	sVars.push_back(_T("garb_burn"));
	sVars.push_back(_T("garb_comp"));
	sVars.push_back(_T("garb_recycl"));
	sVars.push_back(_T("garb_wsegr"));
	sVars.push_back(_T("garb_cpit"));
	sVars.push_back(_T("garb_opit"));
	if (m_qnrID==120110100) {
	sVars.push_back(_T("garb_river"));
	}
	sVars.push_back(_T("garb_mgt_o"));

	sLabels.push_back(_T("garbage collected"));
	sLabels.push_back(_T("garbage burned"));
	sLabels.push_back(_T("garbage composted"));
	sLabels.push_back(_T("garbage recycled"));
	sLabels.push_back(_T("garbage segregated"));
	sLabels.push_back(_T("garbage dumped to closed pit"));
	sLabels.push_back(_T("garbage dumped to open pit"));
	if (m_qnrID==120110100) {
	sLabels.push_back(_T("garbage throwing in river"));
	}
	sLabels.push_back(_T("other waste management"));

	CrHHIndCatEx(_T("waste_mgt"), sVars, sLabels);

	sVars.clear();
	sLabels.clear();

	//////////////////////////////////////////////

	if (currElt<=BRGY) {	
		//type of dwelling unit
		CrHHIndCat(HPQ_RAW, _MBCS("house_type"), _MBCS("tothh"));

	}

	//electricity
	sSQL = "DROP TABLE IF EXISTS `welec`;";
	pEltDB->ExecuteSQL(sSQL);

	sSQL = "CREATE TABLE `welec` \n\
		SELECT hpq_hh.urb, hpq_hh.regn, hpq_hh.prov, hpq_hh.mun, hpq_hh.brgy, hpq_hh.purok, hpq_hh.hcn, \n\
		CONCAT(`hpq_hh`.`regn`, `hpq_hh`.`prov`, `hpq_hh`.`mun`, `hpq_hh`.`brgy`, `hpq_hh`.`purok`, `hpq_hh`.`hcn`) as `hhID`, \n\
		if(`welec`=1,1,0) AS welec \n\
		FROM hpq_hh;";

	pEltDB->ExecuteSQL(sSQL);

	CrHHInd(HPQ_RAW, _MBCS("welec"), _MBCS("tothh"), _MBCS("welec"), _MBCS("totpop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE);
	sSQL = "DROP TABLE IF EXISTS `welec`;";
	pEltDB->ExecuteSQL(sSQL);

	sVars.push_back(_T("elec_company"));
	sVars.push_back(_T("elec_generator"));
	sVars.push_back(_T("elec_solar"));
	sVars.push_back(_T("elec_battery"));

	sLabels.push_back(_T("electric company"));
	sLabels.push_back(_T("generator"));
	sLabels.push_back(_T("solar"));
	sLabels.push_back(_T("battery"));

	CrHHIndCatEx(_T("elec_src"), sVars, sLabels, _T("welec"));

	sVars.clear();
	sLabels.clear();

	//////////////////////////////////////////////

	//entrepreneurial
	sVars.push_back(_T("cropind"));
	sVars.push_back(_T("poultind"));
	sVars.push_back(_T("fishind"));
	sVars.push_back(_T("forind"));
	sVars.push_back(_T("salind"));
	sVars.push_back(_T("manind"));
	sVars.push_back(_T("servind"));
	sVars.push_back(_T("trnind"));
	sVars.push_back(_T("minind"));
	sVars.push_back(_T("cnsind"));
	sVars.push_back(_T("eothind"));

	sLabels.push_back(_T("engaged in crop farming and gardening"));
	sLabels.push_back(_T("engaged in livestock/poultry"));
	sLabels.push_back(_T("engaged in fishing"));
	sLabels.push_back(_T("engaged in forestry"));
	sLabels.push_back(_T("engaged in wholesale/retail"));
	sLabels.push_back(_T("engaged in manufacturing"));
	sLabels.push_back(_T("engaged in community, social & personal service"));
	sLabels.push_back(_T("engaged in transportation, storage & communicat"));
	sLabels.push_back(_T("engaged in mining & quarrying"));
	sLabels.push_back(_T("engaged in construction"));
	sLabels.push_back(_T("other activities nec"));

	CrHHIndCatEx(_T("ent_act"), sVars, sLabels);

	sVars.clear();
	sLabels.clear();

	//////////////////////////////////////////////

	delete pRS; pRS = NULL;

}
void CStatSimElt::CrBrgyIndCatEx(CString sSrc, CString sName, std::vector<CString> sVars, 
	std::vector<CString> sLabels, CString sDenom, BOOL sVarAsIs)
{

	if (currElt>BRGY)
		return;

	if (sVars.size()!=sLabels.size()) {
		AfxMessageBox(_T("Variables and labels do not match!"));
		return;
	}

	CString sSQL, sE;
	sE = Attr(element);

	sSQL = _T("DROP TABLE IF EXISTS `") + sE + _T("_") + sName + _T("`;");
	pEltDB->ExecuteSQL(sSQL);

	switch(currElt) {

	case MEM:
		
		//Define index (primary key cannot be established always in this table)
		sSQL ="";

		break;

	case HH:
		
		sSQL = "";
		break;

	case PUROK:
		
		sSQL = "";
		break;

	case BRGY:
		sSQL = _T("CREATE TABLE IF NOT EXISTS `brgy_") + sName + _T("` ( \n\
			`brgyID` varchar(9), \n\
			`region` varchar(50), \n\
			`province` varchar(50), \n\
			`municipality` varchar(50), \n\
			`barangay` varchar(100), \n\
			`brgy_") + sName + _T("` FLOAT UNSIGNED NOT NULL DEFAULT 0, \n\
			`brgy_") + sDenom + _T("` FLOAT UNSIGNED NOT NULL DEFAULT 0, \n\
			`brgy_") + sName + _T("_prop` FLOAT(5,2) NOT NULL DEFAULT 0, \n\
			`") + sName + _T("` INTEGER(2) UNSIGNED NOT NULL DEFAULT 0, \n\
			`") + sName + _T("_label` TEXT, \n\
			PRIMARY KEY(`brgyID`, `") + sName + _T("`) \n\
			) \n\
			ENGINE = MYISAM; \n");
		
		break;

	case MUN:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `mun_") + sName + _T("` ( \n\
			`munID` varchar(9), \n\
			`region` varchar(50), \n\
			`province` varchar(50), \n\
			`municipality` varchar(50), \n\
			`mun_") + sName + _T("` FLOAT UNSIGNED NOT NULL DEFAULT 0, \n\
			`mun_") + sDenom + _T("` FLOAT UNSIGNED NOT NULL DEFAULT 0, \n\
			`mun_") + sName + _T("_prop` FLOAT(5,2) NOT NULL DEFAULT 0, \n\
			`") + sName + _T("` INTEGER(2) UNSIGNED NOT NULL DEFAULT 0, \n\
			`") + sName + _T("_label` TEXT, \n\
			PRIMARY KEY(`munID`, `") + sName + _T("`) \n\
			) \n\
			ENGINE = MYISAM; \n");

		break;

	case PROV:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `prov_") + sName + _T("` ( \n\
			`provID` varchar(9), \n\
			`region` varchar(50), \n\
			`province` varchar(50), \n\
			`prov_") + sName + _T("` FLOAT UNSIGNED NOT NULL DEFAULT 0, \n\
			`prov_") + sDenom + _T("` FLOAT UNSIGNED NOT NULL DEFAULT 0, \n\
			`prov_") + sName + _T("_prop` FLOAT(5,2) NOT NULL DEFAULT 0, \n\
			`") + sName + _T("` INTEGER(2) UNSIGNED NOT NULL DEFAULT 0, \n\
			`") + sName + _T("_label` TEXT, \n\
			PRIMARY KEY(`provID`, `") + sName + _T("`) \n\
			) \n\
			ENGINE = MYISAM; \n");

		break;

	case REGN:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `regn_") + sName + _T("` ( \n\
			`regnID` varchar(9), \n\
			`region` varchar(50), \n\
			`regn_") + sName + _T("` FLOAT UNSIGNED NOT NULL DEFAULT 0, \n\
			`regn_") + sDenom + _T("` FLOAT UNSIGNED NOT NULL DEFAULT 0, \n\
			`regn_") + sName + _T("_prop` FLOAT(5,2) NOT NULL DEFAULT 0, \n\
			`") + sName + _T("` INTEGER(2) UNSIGNED NOT NULL DEFAULT 0, \n\
			`") + sName + _T("_label` TEXT, \n\
			PRIMARY KEY(`regnID`, `") + sName + _T("`) \n\
			) \n\
			ENGINE = MYISAM; \n");

		break;

	case COUNTRY:
		
		sSQL = "";
		break;

	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}

	pEltDB->ExecuteSQL(sSQL);

	for (int i=0;i<sVars.size();i++) {

		sSQL = _T("DROP TABLE IF EXISTS `") + sVars[i] + _T("`;");
		pEltDB->ExecuteSQL(sSQL);

		CString sVarExprCl;
		if (sVarAsIs) {
			sVarExprCl = sVars[i];
		}
		else {
			sVarExprCl = _T("if(`") + sVars[i] + _T("`=1,1,0)");

		}

		sSQL = _T("CREATE TABLE `") + sVars[i] + _T("` \n\
			SELECT ") + sSrc + _T(".urb, ") + sSrc + _T(".regn, ") + sSrc + _T(".prov, ") + sSrc + _T(".mun, ") + sSrc + _T(".brgy, \n\
			CONCAT(`") + sSrc + _T("`.`regn`, `") + sSrc + _T("`.`prov`, `") + sSrc + _T("`.`mun`, `") + sSrc + _T("`.`brgy`) as `obsID`, \n\
			") + sVarExprCl + _T(" AS ") + sVars[i] + _T(" \n\
			FROM ") + sSrc + _T(";");

		pEltDB->ExecuteSQL(sSQL);

		CString sSrcDenom;
		if (sDenom==_T("tothh")) {
			sSrcDenom = _T("totpop");
		}
		else {
			sSrcDenom = sDenom;
		}
	
		CrHHInd(HPQ_RAW, ConstChar(sVars[i]), ConstChar(sDenom), ConstChar(sVars[i]), ConstChar(sSrcDenom), _MBCS("obsID"), 
			_MBCS("SUM"), 100,
			FALSE);

		sSQL = _T("DROP TABLE IF EXISTS `") + sVars[i] + _T("`;");
		pEltDB->ExecuteSQL(sSQL);

		sSQL = _T("ALTER TABLE `") + sE + _T("_") + sVars[i] + _T("` \n\
			ADD COLUMN `") + sName + _T("` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			ADD COLUMN `") + sName + _T("_label` TEXT;");

		pEltDB->ExecuteSQL(sSQL);

		//update to set values
		CString sNameCode;
		sNameCode.Format(_T("%d"), i+1);
		sSQL = _T("UPDATE `") + sE + _T("_") + sVars[i] + _T("` \n\
			SET `") + sName + _T("`=") + sNameCode + _T(", \n\
			`") + sName + _T("_label`='") + sLabels[i] + _T("';");
		pEltDB->ExecuteSQL(sSQL);

		sSQL = _T("INSERT IGNORE INTO `") + sE + _T("_") + sName + _T("` \n\
			SELECT * FROM `") + sE  + _T("_") + sVars[i] + _T("`;");
		pEltDB->ExecuteSQL(sSQL);
				
		sSQL = _T("DROP TABLE IF EXISTS `") + sE + _T("_") + sVars[i] + _T("`;");
		pEltDB->ExecuteSQL(sSQL);
	}

}

void CStatSimElt::CrHHIndCatEx(CString sName, std::vector<CString> sVars, 
	std::vector<CString> sLabels, CString sDenom)
{

	if (currElt>BRGY)
		return;

	if (sVars.size()!=sLabels.size()) {
		AfxMessageBox(_T("Variables and labels do not match!"));
		return;
	}

	CString sSQL, sE;
	sE = Attr(element);

	sSQL = _T("DROP TABLE IF EXISTS `") + sE + _T("_") + sName + _T("`;");
	pEltDB->ExecuteSQL(sSQL);

	switch(currElt) {

	case MEM:
		
		//Define index (primary key cannot be established always in this table)
		sSQL ="";

		break;

	case HH:
		
		sSQL = "";
		break;

	case PUROK:
		
		sSQL = "";
		break;

	case BRGY:
		sSQL = _T("CREATE TABLE IF NOT EXISTS `brgy_") + sName + _T("` ( \n\
			`brgyID` varchar(9), \n\
			`region` varchar(50), \n\
			`province` varchar(50), \n\
			`municipality` varchar(50), \n\
			`barangay` varchar(100), \n\
			`brgy_") + sName + _T("` INTEGER(11) UNSIGNED NOT NULL DEFAULT 0, \n\
			`brgy_") + sDenom + _T("` INTEGER(11) UNSIGNED NOT NULL DEFAULT 0, \n\
			`brgy_") + sName + _T("_prop` FLOAT(5,2) NOT NULL DEFAULT 0, \n\
			`") + sName + _T("` INTEGER(2) UNSIGNED NOT NULL DEFAULT 0, \n\
			`") + sName + _T("_label` TEXT, \n\
			PRIMARY KEY(`brgyID`, `") + sName + _T("`) \n\
			) \n\
			ENGINE = MYISAM; \n");
		
		break;

	case MUN:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `mun_") + sName + _T("` ( \n\
			`munID` varchar(9), \n\
			`region` varchar(50), \n\
			`province` varchar(50), \n\
			`municipality` varchar(50), \n\
			`mun_") + sName + _T("` INTEGER(11) UNSIGNED NOT NULL DEFAULT 0, \n\
			`mun_") + sDenom + _T("` INTEGER(11) UNSIGNED NOT NULL DEFAULT 0, \n\
			`mun_") + sName + _T("_prop` FLOAT(5,2) NOT NULL DEFAULT 0, \n\
			`") + sName + _T("` INTEGER(2) UNSIGNED NOT NULL DEFAULT 0, \n\
			`") + sName + _T("_label` TEXT, \n\
			PRIMARY KEY(`munID`, `") + sName + _T("`) \n\
			) \n\
			ENGINE = MYISAM; \n");

		break;

	case PROV:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `prov_") + sName + _T("` ( \n\
			`provID` varchar(9), \n\
			`region` varchar(50), \n\
			`province` varchar(50), \n\
			`prov_") + sName + _T("` INTEGER(11) UNSIGNED NOT NULL DEFAULT 0, \n\
			`prov_") + sDenom + _T("` INTEGER(11) UNSIGNED NOT NULL DEFAULT 0, \n\
			`prov_") + sName + _T("_prop` FLOAT(5,2) NOT NULL DEFAULT 0, \n\
			`") + sName + _T("` INTEGER(2) UNSIGNED NOT NULL DEFAULT 0, \n\
			`") + sName + _T("_label` TEXT, \n\
			PRIMARY KEY(`provID`, `") + sName + _T("`) \n\
			) \n\
			ENGINE = MYISAM; \n");

		break;

	case REGN:
		
		sSQL = _T("CREATE TABLE IF NOT EXISTS `regn_") + sName + _T("` ( \n\
			`regnID` varchar(9), \n\
			`region` varchar(50), \n\
			`regn_") + sName + _T("` INTEGER(11) UNSIGNED NOT NULL DEFAULT 0, \n\
			`regn_") + sDenom + _T("` INTEGER(11) UNSIGNED NOT NULL DEFAULT 0, \n\
			`regn_") + sName + _T("_prop` FLOAT(5,2) NOT NULL DEFAULT 0, \n\
			`") + sName + _T("` INTEGER(2) UNSIGNED NOT NULL DEFAULT 0, \n\
			`") + sName + _T("_label` TEXT, \n\
			PRIMARY KEY(`regnID`, `") + sName + _T("`) \n\
			) \n\
			ENGINE = MYISAM; \n");

		break;

	case COUNTRY:
		
		sSQL = "";
		break;

	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}

	pEltDB->ExecuteSQL(sSQL);

	for (int i=0;i<sVars.size();i++) {

		sSQL = _T("DROP TABLE IF EXISTS `") + sVars[i] + _T("`;");
		pEltDB->ExecuteSQL(sSQL);

		sSQL = _T("CREATE TABLE `") + sVars[i] + _T("` \n\
			SELECT hpq_hh.urb, hpq_hh.regn, hpq_hh.prov, hpq_hh.mun, hpq_hh.brgy, hpq_hh.purok, hpq_hh.hcn, \n\
			CONCAT(`hpq_hh`.`regn`, `hpq_hh`.`prov`, `hpq_hh`.`mun`, `hpq_hh`.`brgy`, `hpq_hh`.`purok`, `hpq_hh`.`hcn`) as `hhID`, \n\
			if(`") + sVars[i] + _T("`=1,1,0) AS ") + sVars[i] + _T(" \n\
			FROM hpq_hh;");

		pEltDB->ExecuteSQL(sSQL);

		CString sSrcDenom;
		if (sDenom==_T("tothh")) {
			sSrcDenom = _T("totpop");
		}
		else {
			sSrcDenom = sDenom;
		}
	
		CrHHInd(HPQ_RAW, ConstChar(sVars[i]), ConstChar(sDenom), ConstChar(sVars[i]), ConstChar(sSrcDenom), _MBCS("hhID"), 
			_MBCS("SUM"), 100,
			FALSE);

		sSQL = _T("DROP TABLE IF EXISTS `") + sVars[i] + _T("`;");
		pEltDB->ExecuteSQL(sSQL);

		sSQL = _T("ALTER TABLE `") + sE + _T("_") + sVars[i] + _T("` \n\
			ADD COLUMN `") + sName + _T("` INTEGER(2) UNSIGNED ZEROFILL NOT NULL DEFAULT 0, \n\
			ADD COLUMN `") + sName + _T("_label` TEXT;");

		pEltDB->ExecuteSQL(sSQL);

		//update to set values
		CString sNameCode;
		sNameCode.Format(_T("%d"), i+1);
		sSQL = _T("UPDATE `") + sE + _T("_") + sVars[i] + _T("` \n\
			SET `") + sName + _T("`=") + sNameCode + _T(", \n\
			`") + sName + _T("_label`='") + sLabels[i] + _T("';");
		pEltDB->ExecuteSQL(sSQL);

		sSQL = _T("INSERT IGNORE INTO `") + sE + _T("_") + sName + _T("` \n\
			SELECT * FROM `") + sE  + _T("_") + sVars[i] + _T("`;");
		pEltDB->ExecuteSQL(sSQL);
				
		sSQL = _T("DROP TABLE IF EXISTS `") + sE + _T("_") + sVars[i] + _T("`;");
		pEltDB->ExecuteSQL(sSQL);
	}

}

void CStatSimElt::CrMDG(long qnrID, float perday)
{

	m_qnrID = qnrID;

	if (currElt==HH || currElt==MEM) {
		AfxMessageBox(_T("Not applicable."));
		return;
	}

	CStatSimRS *pRS = 0;

	CString sSQL, sFrTable, sToTable, sE;
	sE = Attr(element);

	sToTable.Format(_T("%s_mdg"), (CString) Attr(element));
	sFrTable.Format(_T("%s_coreind"), (CString) Attr(element));
	
	sSQL.Format(_T("DROP TABLE IF EXISTS `%s_MDG`"), (CString) Attr(element) );

	pEltDB->ExecuteSQL( sSQL );

	//FILE *pFile = fopen("c:\\a.txt","w");
	//fprintf(pFile, ConstChar(sSQLMDG()));
	//fclose(pFile);
	pEltDB->ExecuteSQL( sSQLMDG() );
	pEltDB->InsertFromSQL(sSQLSelectMDG(perday), sToTable);	

	if (currElt==HH || currElt==MEM) {
		return;
	}

	//poverty gap
	sFrTable = sE + _T("_povgap_tmp");
	sSQL = _T("DROP TABLE IF EXISTS `") + sFrTable + _T("`;");
	pEltDB->ExecuteSQL( sSQL );
	sSQL = _T("CREATE TABLE IF NOT EXISTS `") + sFrTable + _T("` \n\
			SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			SUM(`hh_ind`.`hh_povgap`) AS `") + sE + _T("_povgap`,\n\
			SUM(`hh_ind`.`hh_povgap`)/`") + sE + _T("_tothh` AS `") + sE + _T("_povgap_prop`,\n\
			SUM(`hh_coreind`.`hh_totmem`*`hh_ind`.`hh_povgap`) AS `") + sE + _T("_mempovgap`,\n\
			SUM(`hh_coreind`.`hh_totmem`*`hh_ind`.`hh_povgap`)/`") + sE + _T("_totmem` AS `") + sE + _T("_mempovgap_prop`,\n\
			SUM(`hh_coreind`.`hh_totmem_male`*`hh_ind`.`hh_povgap`)/`") + sE + _T("_totmem_male` AS `") + sE + _T("_mempovgap_male_prop`,\n\
			SUM(`hh_coreind`.`hh_totmem_female`*`hh_ind`.`hh_povgap`)/`") + sE + _T("_totmem_female` AS `") + sE + _T("_mempovgap_female_prop`\n\
			FROM `") + sE + _T("` INNER JOIN `") + sE + _T("_totpop` USING (`") + sE + _T("ID`) \n\
			INNER JOIN `hh` USING (") + sGeo() + _T(") \n\
			INNER JOIN `hh_coreind` USING (`hhID`)\n\
			INNER JOIN `hh_ind` USING (`hhID`)\n\
			GROUP BY `") + sE + _T("ID`;");
	
	pEltDB->ExecuteSQL(sSQL);
	pEltDB->UpdateRecords(sFrTable, sToTable, Attr(IDField) );
	sSQL = _T("DROP TABLE IF EXISTS `") + sFrTable + _T("`;");
	pEltDB->ExecuteSQL(sSQL);
	
	//tertiary 16-20
	sFrTable = sE + _T("_tert1620_tmp");
	sSQL = _T("DROP TABLE IF EXISTS `") + sFrTable + _T("`;");
	pEltDB->ExecuteSQL( sSQL );
	sSQL = _T("CREATE TABLE IF NOT EXISTS `") + sFrTable + _T("` \n\
			SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			SUM(`mem_ind`.`mem1620`) AS `") + sE + _T("_mem1620`,\n\
			SUM(`mem_ind`.`mem1620_male`) AS `") + sE + _T("_mem1620_male`,\n\
			SUM(`mem_ind`.`mem1620_female`) AS `") + sE + _T("_mem1620_female`,\n\
			SUM(`mem_ind`.`tert1620`) AS `") + sE + _T("_tert1620`,\n\
			SUM(`mem_ind`.`tert1620_male`) AS `") + sE + _T("_tert1620_male`,\n\
			SUM(`mem_ind`.`tert1620_female`) AS `") + sE + _T("_tert1620_female`,\n\
			(SUM(`mem_ind`.`tert1620`)/SUM(`mem_ind`.`mem1620`))*100 AS `") + sE + _T("_tert1620_prop`,\n\
			(SUM(`mem_ind`.`tert1620_male`)/SUM(`mem_ind`.`mem1620_male`))*100 AS `") + sE + _T("_tert1620_male_prop`,\n\
			(SUM(`mem_ind`.`tert1620_female`)/SUM(`mem_ind`.`mem1620_female`))*100 AS `") + sE + _T("_tert1620_female_prop`\n\
			FROM `") + sE + _T("_totpop` INNER JOIN `") + sE + _T("` USING (`") + sE + _T("ID`) \n\
			INNER JOIN `mem_ind` USING (") + sGeo() + _T(") \n\
			GROUP BY `") + sE + _T("ID`;");
	
	pEltDB->ExecuteSQL(sSQL);
	pEltDB->UpdateRecords(sFrTable, sToTable, Attr(IDField) );
	sSQL = _T("DROP TABLE IF EXISTS `") + sFrTable + _T("`;");
	pEltDB->ExecuteSQL(sSQL);

	//literacy 15-24
	sFrTable = sE + _T("_liter1524_tmp");
	sSQL = _T("DROP TABLE IF EXISTS `") + sFrTable + _T("`;");
	pEltDB->ExecuteSQL( sSQL );
	sSQL = _T("CREATE TABLE IF NOT EXISTS `") + sFrTable + _T("` \n\
			SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			SUM(`mem_ind`.`mem1524`) AS `") + sE + _T("_mem1524`,\n\
			SUM(`mem_ind`.`mem1524_male`) AS `") + sE + _T("_mem1524_male`,\n\
			SUM(`mem_ind`.`mem1524_female`) AS `") + sE + _T("_mem1524_female`,\n\
			SUM(`mem_ind`.`liter1524`) AS `") + sE + _T("_liter1524`,\n\
			SUM(`mem_ind`.`liter1524_male`) AS `") + sE + _T("_liter1524_male`,\n\
			SUM(`mem_ind`.`liter1524_female`) AS `") + sE + _T("_liter1524_female`,\n\
			(SUM(`mem_ind`.`liter1524`)/SUM(`mem_ind`.`mem1524`))*100 AS `") + sE + _T("_liter1524_prop`,\n\
			(SUM(`mem_ind`.`liter1524_male`)/SUM(`mem_ind`.`mem1524_male`))*100 AS `") + sE + _T("_liter1524_male_prop`,\n\
			(SUM(`mem_ind`.`liter1524_female`)/SUM(`mem_ind`.`mem1524_female`))*100 AS `") + sE + _T("_liter1524_female_prop`\n\
			FROM `") + sE + _T("_totpop` INNER JOIN `") + sE + _T("` USING (`") + sE + _T("ID`) \n\
			INNER JOIN `mem_ind` USING (") + sGeo() + _T(") \n\
			GROUP BY `") + sE + _T("ID`;");
	
	pEltDB->ExecuteSQL(sSQL);
	pEltDB->UpdateRecords(sFrTable, sToTable, Attr(IDField) );
	sSQL = _T("DROP TABLE IF EXISTS `") + sFrTable + _T("`;");
	pEltDB->ExecuteSQL(sSQL);

	//ratio girls:boys
	sSQL = _T("UPDATE `") + sE + _T("_mdg` SET \n\
			`") + sE + _T("_liter1524_mfratio` = `") + sE + _T("_liter1524_female_prop`/`") + sE + _T("_liter1524_male_prop`, \n\
			`") + sE + _T("_elem611_mfratio` = `") + sE + _T("_elem611_female_prop`/`") + sE + _T("_elem611_male_prop`, \n\
			`") + sE + _T("_hs1215_mfratio` = `") + sE + _T("_hs1215_female_prop`/`") + sE + _T("_hs1215_male_prop`, \n\
			`") + sE + _T("_sch615_mfratio` = `") + sE + _T("_sch615_female_prop`/`") + sE + _T("_sch615_male_prop`, \n\
			`") + sE + _T("_tert1620_mfratio` = `") + sE + _T("_tert1620_female_prop`/`") + sE + _T("_tert1620_male_prop` \n\
			;");
	//sSQL = _T("UPDATE `") + sE + _T("_mdg` SET \n\
	//		`") + sE + _T("_liter1524_mfratio` = `") + sE + _T("_liter1524_female`/`") + sE + _T("_liter1524_male`, \n\
	//		`") + sE + _T("_elem611_mfratio` = `") + sE + _T("_elem611_female`/`") + sE + _T("_elem611_male`, \n\
	//		`") + sE + _T("_hs1215_mfratio` = `") + sE + _T("_hs1215_female`/`") + sE + _T("_hs1215_male`, \n\
	//		`") + sE + _T("_sch615_mfratio` = `") + sE + _T("_sch615_female`/`") + sE + _T("_sch615_male` \n\
	//		;");

	pEltDB->ExecuteSQL(sSQL);

	//death01
	CrMemInd(HPQ_RAW, _MBCS("Death01"), _MBCS("Mem01"), _MBCS("_Male"), _MBCS("_Female"), _MBCS("HH_Ind"), _MBCS("Demog"), TRUE, _MBCS("hhID"));
	sFrTable.Format(_T("%s_Death01"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	//death14
	CrMemInd(HPQ_RAW, _MBCS("Death14"), _MBCS("Mem14"), _MBCS("_Male"), _MBCS("_Female"), _MBCS("HH_Ind"), _MBCS("Demog"), TRUE, _MBCS("hhID"));
	sSQL = _T("SELECT `") + sE + _T("_death14`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			`") + sE + _T("_death14`, \n\
			`") + sE + _T("_death14_male`, \n\
			`") + sE + _T("_death14_female`, \n\
			`") + sE + _T("_death14_prop`, \n\
			`") + sE + _T("_death14_male_prop`, \n\
			`") + sE + _T("_death14_female_prop` \n\
			FROM `") + sE + _T("_death14`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

if (m_qnrID==1120041100||m_qnrID==1020040900) {
	//create roster of couples (another innov)
	sSQL = _T("DROP TABLE IF EXISTS `hpq_couple`;");
	pEltDB->ExecuteSQL( sSQL );
	sSQL = "CREATE TABLE `hpq_couple` ( \n\
		   `rtype` varchar(2) DEFAULT NULL,\n\
		   `urb` tinyint(1) unsigned zerofill DEFAULT NULL,\n\
		   `regn` tinyint(2) unsigned zerofill DEFAULT NULL,\n\
		   `prov` tinyint(2) unsigned zerofill DEFAULT NULL,\n\
		   `mun` tinyint(2) unsigned zerofill DEFAULT NULL,\n\
		   `brgy` smallint(3) unsigned zerofill DEFAULT NULL,\n\
		   `purok` tinyint(2) unsigned zerofill DEFAULT NULL,\n\
		   `hcn` smallint(6) unsigned zerofill DEFAULT NULL,\n\
		   `couple_line` tinyint(2) unsigned zerofill DEFAULT NULL,\n\
		   `husband` varchar(20) DEFAULT NULL,\n\
		   `wife` varchar(20) DEFAULT NULL,\n\
		   `fam_plan` tinyint(1) DEFAULT NULL,\n\
		   `fp_meth` tinyint(2) DEFAULT NULL,\n\
		   `fp_meth_o` varchar(20) DEFAULT NULL,\n\
		   KEY `hpqIndex` (`regn`,`prov`,`mun`,`brgy`,`purok`,`hcn`) \n\
		   ) ENGINE=MyISAM DEFAULT CHARSET=latin1; ";
	
	pEltDB->ExecuteSQL(sSQL);
	sSQL = "INSERT INTO `hpq_couple` \n\
		   SELECT 99 as `rtype`, `urb`, `regn`, `prov`, `mun`, `brgy`, `purok`, `hcn`, 1 as `couple_line`, \n\
		   `husband_1` as `husband`, `wife_1` as `wife`, `fam_plan_1` as `fam_plan`, `fp_meth_1` as `fp_meth`, `fp_meth_1_o` as `fp_meth_o` \n\
		   from hpq_hh where fam_plan_1 is not null;";
	pEltDB->ExecuteSQL(sSQL);
	sSQL = "INSERT INTO `hpq_couple` \n\
		   SELECT 99 as `rtype`, `urb`, `regn`, `prov`, `mun`, `brgy`, `purok`, `hcn`, 2 as `couple_line`, \n\
		   `husband_2` as `husband`, `wife_2` as `wife`, `fam_plan_2` as `fam_plan`, `fp_meth_2` as `fp_meth`, `fp_meth_2_o` as `fp_meth_o` \n\
		   from hpq_hh where fam_plan_2 is not null;";
	pEltDB->ExecuteSQL(sSQL);
	sSQL = "INSERT INTO `hpq_couple` \n\
		   SELECT 99 as `rtype`, `urb`, `regn`, `prov`, `mun`, `brgy`, `purok`, `hcn`, 3 as `couple_line`, \n\
		   `husband_3` as `husband`, `wife_3` as `wife`, `fam_plan_3` as `fam_plan`, `fp_meth_3` as `fp_meth`, `fp_meth_3_o` as `fp_meth_o` \n\
		   from hpq_hh where fam_plan_3 is not null;";
	pEltDB->ExecuteSQL(sSQL);
	sSQL = "INSERT INTO `hpq_couple` \n\
		   SELECT 99 as `rtype`, `urb`, `regn`, `prov`, `mun`, `brgy`, `purok`, `hcn`, 4 as `couple_line`, \n\
		   `husband_4` as `husband`, `wife_4` as `wife`, `fam_plan_4` as `fam_plan`, `fp_meth_4` as `fp_meth`, `fp_meth_4_o` as `fp_meth_o` \n\
		   from hpq_hh where fam_plan_4 is not null;";
	pEltDB->ExecuteSQL(sSQL);
	sSQL = "INSERT INTO `hpq_couple` \n\
		   SELECT 99 as `rtype`, `urb`, `regn`, `prov`, `mun`, `brgy`, `purok`, `hcn`, 5 as `couple_line`, \n\
		   `husband_5` as `husband`, `wife_5` as `wife`, `fam_plan_5` as `fam_plan`, `fp_meth_5` as `fp_meth`, `fp_meth_5_o` as `fp_meth_o` \n\
		   from hpq_hh where fam_plan_5 is not null;";
	pEltDB->ExecuteSQL(sSQL);
}

if (m_qnrID!=1020130100 && m_qnrID!=720130100 && m_qnrID!=1020130101 && m_qnrID!=1020135100 && m_qnrID!=1020135200 && m_qnrID!=1020135300 && m_qnrID!=1020135400 && m_qnrID!=1020135500) {
	//contraceptives
	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			SUM(`hpq_hh`.`ncouple`) AS `") + sE + _T("_couple`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_hh` USING (") + sGeo() + _T(") \n\
			GROUP BY `") + sE + _T("ID`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	//dont use fam_plan!!??
	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_couple`.`hcn`) AS `") + sE + _T("_usebc`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_couple` USING (") + sGeo() + _T(") \n\
			WHERE `hpq_couple`.`fam_plan`=1 \n\
			GROUP BY `") + sE + _T("ID`;");
	//`hpq_couple`.`fp_meth`>=1 and `hpq_couple`.`fp_meth`<=13
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;
}

//code 8 or 9
if (m_qnrID==1020130100 || m_qnrID==1020130101 || m_qnrID==720130100 || m_qnrID==1020135100 || m_qnrID==1020135200 || m_qnrID==1020135300 || m_qnrID==1020135400 || m_qnrID==1020135500) {
	//AfxMessageBox(_T("Indicators on family planning is not available."));
}
else if (m_qnrID==120110100 || m_qnrID==1020100100) {
	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_couple`.`hcn`) AS `") + sE + _T("_usecondom`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_couple` USING (") + sGeo() + _T(") \n\
			WHERE (`hpq_couple`.`fam_plan`=1 AND (`hpq_couple`.`fp_meth`=8 OR `hpq_couple`.`fp_meth`=9)) \n\
			GROUP BY `") + sE + _T("ID`;");

}
//code 5
else if (m_qnrID==1120041100 || m_qnrID==1020040900) {

	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_couple`.`hcn`) AS `") + sE + _T("_usecondom`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_couple` USING (") + sGeo() + _T(") \n\
			WHERE (`hpq_couple`.`fam_plan`=1 AND (`hpq_couple`.`fp_meth`=5)) \n\
			GROUP BY `") + sE + _T("ID`;");
}

//code 6
else if (m_qnrID==1020070400 || m_qnrID==120070300) {
	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_couple`.`hcn`) AS `") + sE + _T("_usecondom`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_couple` USING (") + sGeo() + _T(") \n\
			WHERE (`hpq_couple`.`fam_plan`=1 AND (`hpq_couple`.`fp_meth`=6)) \n\
			GROUP BY `") + sE + _T("ID`;");
}
else {
	AfxMessageBox(_T("Questionnaire not in catalog."));
}
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	sSQL = _T("UPDATE `") + sE + _T("_mdg` \n\
			SET `") + sE + _T("_couple` = IFNULL(`") + sE + _T("_couple`, 0), \n\
			`") + sE + _T("_usebc` = IFNULL(`") + sE + _T("_usebc`, 0), \n\
			`") + sE + _T("_usecondom` = IFNULL(`") + sE + _T("_usecondom`, 0), \n\
			`") + sE + _T("_usebc_prop` = (`") + sE + _T("_usebc`/`") + sE + _T("_couple`)*100, \n\
			`") + sE + _T("_usecondom_prop` = (`") + sE + _T("_usecondom`/`") + sE + _T("_usebc`)*100 \n\
			;");

	pEltDB->ExecuteSQL(sSQL);

	//prevalence of tuberculosis
	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_death`.`hcn`) AS `") + sE + _T("_death_tb`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_death` USING (") + sGeo() + _T(") \n\
			WHERE (`hpq_death`.`mdeady`=4) \n\
			GROUP BY `") + sE + _T("ID`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_death`.`hcn`) AS `") + sE + _T("_death_tb_male`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_death` USING (") + sGeo() + _T(") \n\
			WHERE (`hpq_death`.`mdeady`=4 AND `hpq_death`.`mdeadsx`=1) \n\
			GROUP BY `") + sE + _T("ID`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_death`.`hcn`) AS `") + sE + _T("_death_tb_female`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_death` USING (") + sGeo() + _T(") \n\
			WHERE (`hpq_death`.`mdeady`=4 AND `hpq_death`.`mdeadsx`=2) \n\
			GROUP BY `") + sE + _T("ID`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	sSQL = _T("UPDATE `") + sE + _T("_mdg` \n\
			SET `") + sE + _T("_death_tb` = IFNULL(`") + sE + _T("_death_tb`, 0), \n\
			`") + sE + _T("_death_tb_male` = IFNULL(`") + sE + _T("_death_tb_male`, 0), \n\
			`") + sE + _T("_death_tb_female` = IFNULL(`") + sE + _T("_death_tb_female`, 0), \n\
			`") + sE + _T("_death_tb_prop` = (`") + sE + _T("_death_tb`/(`") + sE + _T("_totmem` + `") + sE + _T("_death_tb`))*100000, \n\
			`") + sE + _T("_death_tb_male_prop` = (`") + sE + _T("_death_tb_male`/(`") + sE + _T("_totmem_male` + `") + sE + _T("_death_tb_male`))*100000, \n\
			`") + sE + _T("_death_tb_female_prop` = (`") + sE + _T("_death_tb_female`/(`") + sE + _T("_totmem_female` + `") + sE + _T("_death_tb_female`))*100000 \n\
			;");

	pEltDB->ExecuteSQL(sSQL);

if (m_qnrID==120070300) {
	//malaria
	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_death`.`hcn`) AS `") + sE + _T("_death_malaria`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_death` USING (") + sGeo() + _T(") \n\
			WHERE (`hpq_death`.`mdeady`=9) \n\
			GROUP BY `") + sE + _T("ID`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_death`.`hcn`) AS `") + sE + _T("_death_malaria_male`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_death` USING (") + sGeo() + _T(") \n\
			WHERE (`hpq_death`.`mdeady`=9 AND `hpq_death`.`mdeadsx`=1) \n\
			GROUP BY `") + sE + _T("ID`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_death`.`hcn`) AS `") + sE + _T("_death_malaria_female`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_death` USING (") + sGeo() + _T(") \n\
			WHERE (`hpq_death`.`mdeady`=9 AND `hpq_death`.`mdeadsx`=2) \n\
			GROUP BY `") + sE + _T("ID`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	sSQL = _T("UPDATE `") + sE + _T("_mdg` \n\
			SET `") + sE + _T("_death_malaria` = IFNULL(`") + sE + _T("_death_malaria`, 0), \n\
			`") + sE + _T("_death_malaria_male` = IFNULL(`") + sE + _T("_death_malaria_male`, 0), \n\
			`") + sE + _T("_death_malaria_female` = IFNULL(`") + sE + _T("_death_malaria_female`, 0), \n\
			`") + sE + _T("_death_malaria_prop` = (`") + sE + _T("_death_malaria`/(`") + sE + _T("_totmem` + `") + sE + _T("_death_malaria`))*100000, \n\
			`") + sE + _T("_death_malaria_male_prop` = (`") + sE + _T("_death_malaria_male`/(`") + sE + _T("_totmem_male` + `") + sE + _T("_death_malaria_male`))*100000, \n\
			`") + sE + _T("_death_malaria_female_prop` = (`") + sE + _T("_death_malaria_female`/(`") + sE + _T("_totmem_female` + `") + sE + _T("_death_malaria_female`))*100000 \n\
			;");

	pEltDB->ExecuteSQL(sSQL);
}
	//employment 15ab
	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID`, \n\
		SUM(`mem_ind`.`empl15ab`) AS `") + sE + _T("_empl15ab`, \n\
		SUM(`mem_ind`.`empl15ab_male`) AS `") + sE + _T("_empl15ab_male`, \n\
		SUM(`mem_ind`.`empl15ab_female`) AS `") + sE + _T("_empl15ab_female`, \n\
		(SUM(`mem_ind`.`empl15ab`)/`") + sE + _T("_labfor`)*100 AS `") + sE + _T("_empl15ab_prop`, \n\
		(SUM(`mem_ind`.`empl15ab_male`)/`") + sE + _T("_labfor_male`)*100 AS `") + sE + _T("_empl15ab_male_prop`, \n\
		(SUM(`mem_ind`.`empl15ab_female`)/`") + sE + _T("_labfor_female`)*100 AS `") + sE + _T("_empl15ab_female_prop` \n\
		FROM `mem_ind` INNER JOIN `") + sE + _T("` USING (") + sGeo() + _T(") \n\
		INNER JOIN `") + sE + _T("_coreind` USING (`") + sE + _T("ID`) \n\
		GROUP BY `") + sE + _T("ID`");

	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	//inadequate living conditions, sws, stf
	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID`, \n\
		SUM(`hh_ind`.`hh_sws`) AS `") + sE + _T("_sws`, \n\
		SUM(`hh_ind`.`hh_sws`*`hh_coreind`.`hh_totmem`) AS `") + sE + _T("_memsws`, \n\
		SUM(`hh_ind`.`hh_sws`*`hh_coreind`.`hh_totmem_male`) AS `") + sE + _T("_memsws_male`, \n\
		SUM(`hh_ind`.`hh_sws`*`hh_coreind`.`hh_totmem_female`) AS `") + sE + _T("_memsws_female`, \n\
		SUM(`hh_ind`.`hh_stf`) AS `") + sE + _T("_stf`, \n\
		SUM(`hh_ind`.`hh_stf`*`hh_coreind`.`hh_totmem`) AS `") + sE + _T("_memstf`, \n\
		SUM(`hh_ind`.`hh_stf`*`hh_coreind`.`hh_totmem_male`) AS `") + sE + _T("_memstf_male`, \n\
		SUM(`hh_ind`.`hh_stf`*`hh_coreind`.`hh_totmem_female`) AS `") + sE + _T("_memstf_female`, \n\
		SUM(`hh_ind`.`hh_inadeqlc`) AS `") + sE + _T("_inadeqlc`, \n\
		SUM(`hh_ind`.`hh_inadeqlc`*`hh_coreind`.`hh_totmem`) AS `") + sE + _T("_meminadeqlc`, \n\
		SUM(`hh_ind`.`hh_inadeqlc`*`hh_coreind`.`hh_totmem_male`) AS `") + sE + _T("_meminadeqlc_male`, \n\
		SUM(`hh_ind`.`hh_inadeqlc`*`hh_coreind`.`hh_totmem_female`) AS `") + sE + _T("_meminadeqlc_female` \n\
		FROM `hh_coreind` INNER JOIN `hh_ind` USING (`hhID`) INNER JOIN `") + sE + _T("` USING (") + sGeo() + _T(") \n\
		GROUP BY `") + sE + _T("ID`");

	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	sSQL = _T("UPDATE `") + sE + _T("_mdg` SET \n\
		`") + sE + _T("_sws_prop`=(`") + sE + _T("_sws`/`") + sE + _T("_tothh`)*100, \n\
		`") + sE + _T("_memsws_prop`=(`") + sE + _T("_memsws`/`") + sE + _T("_totmem`)*100, \n\
		`") + sE + _T("_memsws_male_prop`=(`") + sE + _T("_memsws_male`/`") + sE + _T("_totmem_male`)*100, \n\
		`") + sE + _T("_memsws_female_prop`=(`") + sE + _T("_memsws_female`/`") + sE + _T("_totmem_female`)*100, \n\
		`") + sE + _T("_stf_prop`=(`") + sE + _T("_stf`/`") + sE + _T("_tothh`)*100, \n\
		`") + sE + _T("_memstf_prop`=(`") + sE + _T("_memstf`/`") + sE + _T("_totmem`)*100, \n\
		`") + sE + _T("_memstf_male_prop`=(`") + sE + _T("_memstf_male`/`") + sE + _T("_totmem_male`)*100, \n\
		`") + sE + _T("_memstf_female_prop`=(`") + sE + _T("_memstf_female`/`") + sE + _T("_totmem_female`)*100, \n\
		`") + sE + _T("_inadeqlc_prop`=(`") + sE + _T("_inadeqlc`/`") + sE + _T("_tothh`)*100, \n\
		`") + sE + _T("_meminadeqlc_prop`=(`") + sE + _T("_meminadeqlc`/`") + sE + _T("_totmem`)*100, \n\
		`") + sE + _T("_meminadeqlc_male_prop`=(`") + sE + _T("_meminadeqlc_male`/`") + sE + _T("_totmem_male`)*100, \n\
		`") + sE + _T("_meminadeqlc_female_prop`=(`") + sE + _T("_meminadeqlc_female`/`") + sE + _T("_totmem_female`)*100;");

	pEltDB->ExecuteSQL(sSQL);

	//phone, mphone, computer

if (m_qnrID==120110100) {
	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_owned_asset`.`hcn`) AS `") + sE + _T("_wphone`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_owned_asset` USING (") + sGeo() + _T(") \n\
			WHERE (`hpq_owned_asset`.`oa_type`=15 AND `hpq_owned_asset`.`oa_ind`=1) \n\
			GROUP BY `") + sE + _T("ID`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_owned_asset`.`hcn`) AS `") + sE + _T("_wmphone`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_owned_asset` USING (") + sGeo() + _T(") \n\
			WHERE (`hpq_owned_asset`.`oa_type`=14 AND `hpq_owned_asset`.`oa_ind`=1) \n\
			GROUP BY `") + sE + _T("ID`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_owned_asset`.`hcn`) AS `") + sE + _T("_wcomputer`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_owned_asset` USING (") + sGeo() + _T(") \n\
			WHERE (`hpq_owned_asset`.`oa_type`=12 AND `hpq_owned_asset`.`oa_ind`=1) \n\
			GROUP BY `") + sE + _T("ID`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_owned_asset`.`hcn`) AS `") + sE + _T("_winet`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_owned_asset` USING (") + sGeo() + _T(") \n\
			WHERE (`hpq_owned_asset`.`oa_type`=13 AND `hpq_owned_asset`.`oa_ind`=1) \n\
			GROUP BY `") + sE + _T("ID`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	sSQL = _T("UPDATE `") + sE + _T("_mdg` SET \n\
		`") + sE + _T("_wphone` = IFNULL(`") + sE + _T("_wphone`, 0), \n\
		`") + sE + _T("_wmphone` = IFNULL(`") + sE + _T("_wmphone`, 0), \n\
		`") + sE + _T("_wphoneu` = IFNULL(`") + sE + _T("_wphoneu`, 0), \n\
		`") + sE + _T("_wcomputer` = IFNULL(`") + sE + _T("_wcomputer`, 0), \n\
		`") + sE + _T("_wphone_prop`=(`") + sE + _T("_wphone`/`") + sE + _T("_tothh`)*100, \n\
		`") + sE + _T("_wmphone_prop`=(`") + sE + _T("_wmphone`/`") + sE + _T("_tothh`)*100, \n\
		`") + sE + _T("_wphoneu_prop`=(`") + sE + _T("_wphoneu`/`") + sE + _T("_tothh`)*100, \n\
		`") + sE + _T("_wcomputer_prop`=(`") + sE + _T("_wcomputer`/`") + sE + _T("_tothh`)*100, \n\
		`") + sE + _T("_winet_prop`=(`") + sE + _T("_winet`/`") + sE + _T("_tothh`)*100 \n\
		;");

	pEltDB->ExecuteSQL(sSQL);
}
else if (m_qnrID==1020100100 || m_qnrID==1020130100 || m_qnrID==1020130101 || m_qnrID==1020135100 || m_qnrID==1020135200 || m_qnrID==1020135300 || m_qnrID==1020135400 || m_qnrID==1020135500) {
	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_hh`.`hcn`) AS `") + sE + _T("_wphone`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_hh` USING (") + sGeo() + _T(") \n\
			WHERE (`hpq_hh`.`telefone`=1) \n\
			GROUP BY `") + sE + _T("ID`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_hh`.`hcn`) AS `") + sE + _T("_wmphone`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_hh` USING (") + sGeo() + _T(") \n\
			WHERE (`hpq_hh`.`celfone`=1) \n\
			GROUP BY `") + sE + _T("ID`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_hh`.`hcn`) AS `") + sE + _T("_wphoneu`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_hh` USING (") + sGeo() + _T(") \n\
			WHERE (`hpq_hh`.`celfone`=1 OR `hpq_hh`.`telefone`=1) \n\
			GROUP BY `") + sE + _T("ID`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_hh`.`hcn`) AS `") + sE + _T("_wcomputer`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_hh` USING (") + sGeo() + _T(") \n\
			WHERE (`hpq_hh`.`computer`=1) \n\
			GROUP BY `") + sE + _T("ID`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_hh`.`hcn`) AS `") + sE + _T("_winet`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_hh` USING (") + sGeo() + _T(") \n\
			WHERE (`hpq_hh`.`internet`=1) \n\
			GROUP BY `") + sE + _T("ID`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;


	sSQL = _T("UPDATE `") + sE + _T("_mdg` SET \n\
		`") + sE + _T("_wphone` = IFNULL(`") + sE + _T("_wphone`, 0), \n\
		`") + sE + _T("_wmphone` = IFNULL(`") + sE + _T("_wmphone`, 0), \n\
		`") + sE + _T("_wphoneu` = IFNULL(`") + sE + _T("_wphoneu`, 0), \n\
		`") + sE + _T("_wcomputer` = IFNULL(`") + sE + _T("_wcomputer`, 0), \n\
		`") + sE + _T("_wphone_prop`=(`") + sE + _T("_wphone`/`") + sE + _T("_tothh`)*100, \n\
		`") + sE + _T("_wmphone_prop`=(`") + sE + _T("_wmphone`/`") + sE + _T("_tothh`)*100, \n\
		`") + sE + _T("_wphoneu_prop`=(`") + sE + _T("_wphoneu`/`") + sE + _T("_tothh`)*100, \n\
		`") + sE + _T("_wcomputer_prop`=(`") + sE + _T("_wcomputer`/`") + sE + _T("_tothh`)*100, \n\
		`") + sE + _T("_winet_prop`=(`") + sE + _T("_winet`/`") + sE + _T("_tothh`)*100 \n\
		;");

	pEltDB->ExecuteSQL(sSQL);
}

else if (m_qnrID==1020070400||m_qnrID==120070300) {
	
	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_hh`.`hcn`) AS `") + sE + _T("_wphone`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_hh` USING (") + sGeo() + _T(") \n\
			WHERE (`hpq_hh`.`telefone`=1) \n\
			GROUP BY `") + sE + _T("ID`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_hh`.`hcn`) AS `") + sE + _T("_wmphone`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_hh` USING (") + sGeo() + _T(") \n\
			WHERE (`hpq_hh`.`celfone`=1) \n\
			GROUP BY `") + sE + _T("ID`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_hh`.`hcn`) AS `") + sE + _T("_wphoneu`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_hh` USING (") + sGeo() + _T(") \n\
			WHERE (`hpq_hh`.`celfone`=1 OR `hpq_hh`.`telefone`=1) \n\
			GROUP BY `") + sE + _T("ID`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_hh`.`hcn`) AS `") + sE + _T("_wcomputer`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_hh` USING (") + sGeo() + _T(") \n\
			WHERE (`hpq_hh`.`computer`=1) \n\
			GROUP BY `") + sE + _T("ID`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	sSQL = _T("UPDATE `") + sE + _T("_mdg` SET \n\
		`") + sE + _T("_wphone` = IFNULL(`") + sE + _T("_wphone`, 0), \n\
		`") + sE + _T("_wmphone` = IFNULL(`") + sE + _T("_wmphone`, 0), \n\
		`") + sE + _T("_wphoneu` = IFNULL(`") + sE + _T("_wphoneu`, 0), \n\
		`") + sE + _T("_wcomputer` = IFNULL(`") + sE + _T("_wcomputer`, 0), \n\
		`") + sE + _T("_wphone_prop`=(`") + sE + _T("_wphone`/`") + sE + _T("_tothh`)*100, \n\
		`") + sE + _T("_wmphone_prop`=(`") + sE + _T("_wmphone`/`") + sE + _T("_tothh`)*100, \n\
		`") + sE + _T("_wphoneu_prop`=(`") + sE + _T("_wphoneu`/`") + sE + _T("_tothh`)*100, \n\
		`") + sE + _T("_wcomputer_prop`=(`") + sE + _T("_wcomputer`/`") + sE + _T("_tothh`)*100;");

	pEltDB->ExecuteSQL(sSQL);

}
else if (m_qnrID==1120041100||m_qnrID==1020040900) {
	//phone - old; telefone - new
	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_hh`.`hcn`) AS `") + sE + _T("_wphone`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_hh` USING (") + sGeo() + _T(") \n\
			WHERE (`hpq_hh`.`phone`=1) \n\
			GROUP BY `") + sE + _T("ID`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	//phone - old; celfone - new
	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_hh`.`hcn`) AS `") + sE + _T("_wmphone`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_hh` USING (") + sGeo() + _T(") \n\
			WHERE (`hpq_hh`.`phone`=1) \n\
			GROUP BY `") + sE + _T("ID`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	//phone or mphone 
	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_hh`.`hcn`) AS `") + sE + _T("_wphoneu`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_hh` USING (") + sGeo() + _T(") \n\
			WHERE (`hpq_hh`.`phone`=1) \n\
			GROUP BY `") + sE + _T("ID`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	sSQL = _T("SELECT `") + sE + _T("`.`") + sE + _T("ID` AS `") + sE + _T("ID`, \n\
			COUNT(`hpq_hh`.`hcn`) AS `") + sE + _T("_wcomputer`\n\
			FROM `") + sE + _T("` INNER JOIN `hpq_hh` USING (") + sGeo() + _T(") \n\
			WHERE (`hpq_hh`.`computer`=1) \n\
			GROUP BY `") + sE + _T("ID`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS=0;

	sSQL = _T("UPDATE `") + sE + _T("_mdg` SET \n\
		`") + sE + _T("_wphone` = IFNULL(`") + sE + _T("_wphone`, 0), \n\
		`") + sE + _T("_wmphone` = IFNULL(`") + sE + _T("_wmphone`, 0), \n\
		`") + sE + _T("_wphoneu` = IFNULL(`") + sE + _T("_wphoneu`, 0), \n\
		`") + sE + _T("_wcomputer` = IFNULL(`") + sE + _T("_wcomputer`, 0), \n\
		`") + sE + _T("_wphone_prop`=(`") + sE + _T("_wphone`/`") + sE + _T("_tothh`)*100, \n\
		`") + sE + _T("_wmphone_prop`=(`") + sE + _T("_wmphone`/`") + sE + _T("_tothh`)*100, \n\
		`") + sE + _T("_wphoneu_prop`=(`") + sE + _T("_wphoneu`/`") + sE + _T("_tothh`)*100, \n\
		`") + sE + _T("_wcomputer_prop`=(`") + sE + _T("_wcomputer`/`") + sE + _T("_tothh`)*100;");

	pEltDB->ExecuteSQL(sSQL);

}


	return;


	//non-agri wagers
	sSQL = _T("DROP TABLE IF EXISTS `mem_ntagri`");
	pEltDB->ExecuteSQL( sSQL );
	std::vector<LPCSTR> tables;
	if (pEltDB->GetTableList(tables)) {
		if (Exists(tables, "hpq_wage", (int) tables.size())) {
			sSQL = "CREATE TABLE IF NOT EXISTS `mem_ntagri` \n\
				SELECT hpq_mem.urb, hpq_mem.regn, hpq_mem.prov, hpq_mem.mun, hpq_mem.brgy, hpq_mem.purok, hpq_mem.hcn, hpq_mem.memno, CONCAT(`hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`) as `memID`, hpq_mem.sex, \n\
				hpq_wage.wagcshm, hpq_wage.wagkndm, hpq_mem.g_occ, hpq_mem.sector, IF((`wagcshm`+`wagkndm`)>0,1,0) AS waged, IF((`wagcshm`+`wagkndm`)>0 AND hpq_mem.sector>2,1,0) AS ntagri, IF((`wagcshm`+`wagkndm`)>0 AND hpq_mem.sector>2 and hpq_mem.sex=1,1,0) AS ntagri_male, IF((`wagcshm`+`wagkndm`)>0 AND hpq_mem.sector>2 and hpq_mem.sex=2,1,0) AS ntagri_female, \n\
				`wagcshm`+`wagkndm` AS totwage, if(hpq_mem.sex=1 And (`wagcshm`+`wagkndm`)>0,1,0) AS wage_Male, IF(hpq_mem.sex=1, `wagcshm`+`wagkndm`,0) AS totwage_Male, if(hpq_mem.sex=2 And (`wagcshm`+`wagkndm`)>0,1,0) AS wage_Female, IF(hpq_mem.sex=2, `wagcshm`+`wagkndm`,0) AS totwage_Female \n\
				FROM hpq_mem INNER JOIN hpq_wage ON (hpq_mem.regn = hpq_wage.regn) AND (hpq_mem.prov = hpq_wage.prov) AND (hpq_mem.mun = hpq_wage.mun) AND (hpq_mem.brgy = hpq_wage.brgy) AND (hpq_mem.purok = hpq_wage.purok) AND (hpq_mem.hcn = hpq_wage.hcn) AND (hpq_mem.memno = hpq_wage.wage_line);";
		}
		else {
			sSQL = "CREATE TABLE IF NOT EXISTS `mem_ntagri` \n\
				SELECT urb, regn, prov, mun, brgy, purok, hcn, memno, CONCAT(`hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`) as `memID`, sex, \n\
				g_occ, sector, IF(sector>2,1,0) AS ntagri, IF(sector>2 and sex=1,1,0) AS ntagri_male, IF(sector>2 and sex=2,1,0) AS ntagri_female \n\
				FROM hpq_mem;";
		}
	}
	else {
		sSQL = "CREATE TABLE IF NOT EXISTS `mem_ntagri` \n\
			SELECT urb, regn, prov, mun, brgy, purok, hcn, memno, CONCAT(`hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`) as `memID`, sex, \n\
			g_occ, sector, IF(sector>2,1,0) AS ntagri, IF(sector>2 and sex=1,1,0) AS ntagri_male, IF(sector>2 and sex=2,1,0) AS ntagri_female \n\
			FROM hpq_mem;";
	}

	pEltDB->ExecuteSQL( sSQL );

	//wages under non-agri
	CrMemInd(HPQ_RAW, _MBCS("ntagri"), _MBCS("TotMem"), _MBCS("_Male"), _MBCS("_Female"), _MBCS("mem_ntagri"), _MBCS("Totpop"), 
		FALSE, _MBCS("memID"), _MBCS("SUM"), 100,
		_MBCS("ntagri"));

	sFrTable.Format(_T("%s_ntagri"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS = NULL;

	//infant deaths
	//sSQL = "SELECT `" + (CString) Attr(element) + _T("ID`, `" + (CString) Attr(element) + _T("_hhwdeath01`, \n\
	//	"`" + (CString) Attr(element) + _T("_hhwdeath01_prop` \n\
	//	"FROM `" + (CString) Attr(element) + _T("_coreind`;";
	//pRS = new CStatSimRS( pEltDB, sSQL );
	//pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	//delete pRS; pRS = NULL;
	CrMemInd(HPQ_RAW, _MBCS("death01"), _MBCS("Mem01"), _MBCS("_Male"), _MBCS("_Female"), _MBCS("hh_ind"), _MBCS("demog"), 
		TRUE, _MBCS("hhID"));
	sFrTable.Format(_T("%s_death01"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS = NULL;

	//child 0-5 deaths
	sSQL = _T("SELECT `") + (CString) Attr(element) + _T("ID`, `") + (CString) Attr(element) + _T("_hhwdeath05`, \n\
		`") + (CString) Attr(element) + _T("_hhwdeath05_prop` \n\
		FROM `") + (CString) Attr(element) + _T("_coreind`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS = NULL;
	sFrTable.Format(_T("%s_death05"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS = NULL;

	//maternal deaths
	sSQL = _T("SELECT `") + (CString) Attr(element) + _T("ID`, `") + (CString) Attr(element) + _T("_hhwdeathpreg`, \n\
		`") + (CString) Attr(element) + _T("_hhwdeathpreg_prop` \n\
		FROM `") + (CString) Attr(element) + _T("_coreind`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS = NULL;
	sFrTable.Format(_T("%s_deathpreg"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS = NULL;

	//condom usage
	sSQL = _T("DROP TABLE IF EXISTS `hh_famplan`");
	pEltDB->ExecuteSQL( sSQL );
	sSQL = "CREATE TABLE IF NOT EXISTS `hh_famplan` (\n\
		SELECT hpq_hh.urb, hpq_hh.regn, hpq_hh.prov, hpq_hh.mun, hpq_hh.brgy, hpq_hh.purok, hpq_hh.hcn, hpq_couple.couple_line, CONCAT(`hpq_hh`.`regn`, `hpq_hh`.`prov`, `hpq_hh`.`mun`, `hpq_hh`.`brgy`, `hpq_hh`.`purok`, `hpq_hh`.`hcn`, `hpq_couple`.`couple_line`) as `coupleID`, \n\
		1 as `hh_couple`, IF(`hpq_couple`.`fp_meth`=6,1,0) AS `hh_barrier_method` \n\
		FROM hpq_hh INNER JOIN hpq_couple USING (regn, prov, mun, brgy, purok, hcn));";
	pEltDB->ExecuteSQL( sSQL );

	CrHHInd(HPQ_RAW, _MBCS("couple"), _MBCS("totmem"), _MBCS("hh_famplan"), _MBCS("totpop"), _MBCS("coupleID"));
	CrHHInd(HPQ_RAW, _MBCS("barrier_method"), _MBCS("couple"), _MBCS("hh_famplan"), _MBCS("couple"), _MBCS("coupleID"));
	sFrTable.Format(_T("%s_barrier_method"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS = NULL;

	//causes of death (rostered death)
	sSQL = _T("DROP TABLE IF EXISTS `hh_deathcause`");
	pEltDB->ExecuteSQL( sSQL );
	sSQL = "CREATE TABLE IF NOT EXISTS `hh_deathcause` (\n\
		SELECT hpq_hh.urb, hpq_hh.regn, hpq_hh.prov, hpq_hh.mun, hpq_hh.brgy, hpq_hh.purok, hpq_hh.hcn, hpq_death.death_line, CONCAT(`hpq_hh`.`regn`, `hpq_hh`.`prov`, `hpq_hh`.`mun`, `hpq_hh`.`brgy`, `hpq_hh`.`purok`, `hpq_hh`.`hcn`, `hpq_death`.`death_line`) as `deadID`, \n\
		1 as `death`, IF(`hpq_death`.`mdeadsx`=1,1,0) AS `death_male`, IF(`hpq_death`.`mdeadsx`=2,1,0) AS `death_female`, \n\
		IF(`hpq_death`.`mdeady`=4,1,0) AS `death_tb`, IF(`hpq_death`.`mdeady`=4 AND `hpq_death`.`mdeadsx`=1,1,0) AS `death_tb_male`, IF(`hpq_death`.`mdeady`=4 AND `hpq_death`.`mdeadsx`=2,1,0) AS `death_tb_female`, \n\
		IF(`hpq_death`.`mdeady`=15,1,0) AS `death_malaria`, IF(`hpq_death`.`mdeady`=15 AND `hpq_death`.`mdeadsx`=1,1,0) AS `death_malaria_male`, IF(`hpq_death`.`mdeady`=15 AND `hpq_death`.`mdeadsx`=2,1,0) AS `death_malaria_female` \n\
		FROM hpq_hh INNER JOIN hpq_death USING (regn, prov, mun, brgy, purok, hcn) \n\
		where death_line is not null);";
	pEltDB->ExecuteSQL( sSQL );

	//tuberculosis
	CrMemInd(HPQ_RAW, _MBCS("death"), _MBCS("TotMem"), _MBCS("_Male"), _MBCS("_Female"), _MBCS("hh_deathcause"), _MBCS("Totpop"), 
		FALSE, _MBCS("deadID"), _MBCS("SUM"), 100,
		_MBCS("death"));
	CrMemInd(HPQ_RAW, _MBCS("death_tb"), _MBCS("death"), _MBCS("_Male"), _MBCS("_Female"), _MBCS("hh_deathcause"), _MBCS("death"), 
		FALSE, _MBCS("deadID"), _MBCS("SUM"), 100,
		_MBCS("death_tb"));
	sFrTable.Format(_T("%s_death_tb"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS = NULL;

	//malaria
	CrMemInd(HPQ_RAW, _MBCS("death_malaria"), _MBCS("death"), _MBCS("_Male"), _MBCS("_Female"), _MBCS("hh_deathcause"), _MBCS("death"), 
		FALSE, _MBCS("deadID"), _MBCS("SUM"), 100,
		_MBCS("death_malaria"));
	sFrTable.Format(_T("%s_death_malaria"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS = NULL;

	//sws
	sSQL = _T("SELECT `") + (CString) Attr(element) + _T("ID`, `" )
		+ (CString) Attr(element) + _T("_tothh` - `") 
		+ (CString) Attr(element) + _T("_ntsws` AS `") 
		+ (CString) Attr(element) + _T("_sws`, 100 - `") 
		+ (CString) Attr(element) + _T("_ntsws_prop` AS `") 
		+ (CString) Attr(element) + _T("_sws_prop`, `") 
		+ (CString) Attr(element) + _T("_totmem` - `") 
		+ (CString) Attr(element) + _T("_memntsws` AS `") 
		+ (CString) Attr(element) + _T("_memsws`, `") 
		+ (CString) Attr(element) + _T("_totmem_male` - `") 
		+ (CString) Attr(element) + _T("_memntsws_male` AS `") 
		+ (CString) Attr(element) + _T("_memsws_male`, `") 
		+ (CString) Attr(element) + _T("_totmem_female` - `") 
		+ (CString) Attr(element) + _T("_memntsws_female` AS `") 
		+ (CString) Attr(element) + _T("_memsws_female`, 100 - `") 
		+ (CString) Attr(element) + _T("_memntsws_prop` AS `") 
		+ (CString) Attr(element) + _T("_memsws_prop`, 100 - `") 
		+ (CString) Attr(element) + _T("_memntsws_male_prop` AS `") 
		+ (CString) Attr(element) + _T("_memsws_male_prop`, 100 - `") 
		+ (CString) Attr(element) + _T("_memntsws_female_prop` AS `") 
		+ (CString) Attr(element) + _T("_memsws_female_prop` \n\
		FROM `") + (CString) Attr(element) + _T("_coreind`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS = NULL;

	//stf
	sSQL = _T("SELECT `") + (CString) Attr(element) + _T("ID`, `") 
		+ (CString) Attr(element) + _T("_tothh` - `") 
		+ (CString) Attr(element) + _T("_ntstf` AS `") 
		+ (CString) Attr(element) + _T("_stf`, 100 - `") 
		+ (CString) Attr(element) + _T("_ntstf_prop` AS `") 
		+ (CString) Attr(element) + _T("_stf_prop`, `") 
		+ (CString) Attr(element) + _T("_totmem` - `") 
		+ (CString) Attr(element) + _T("_memntstf` AS `") 
		+ (CString) Attr(element) + _T("_memstf`, `") 
		+ (CString) Attr(element) + _T("_totmem_male` - `") 
		+ (CString) Attr(element) + _T("_memntstf_male` AS `") 
		+ (CString) Attr(element) + _T("_memstf_male`, `") 
		+ (CString) Attr(element) + _T("_totmem_female` - `") 
		+ (CString) Attr(element) + _T("_memntstf_female` AS `") 
		+ (CString) Attr(element) + _T("_memstf_female`, 100 - `") 
		+ (CString) Attr(element) + _T("_memntstf_prop` AS `") 
		+ (CString) Attr(element) + _T("_memstf_prop`, 100 - `") 
		+ (CString) Attr(element) + _T("_memntstf_male_prop` AS `") 
		+ (CString) Attr(element) + _T("_memstf_male_prop`, 100 - `") 
		+ (CString) Attr(element) + _T("_memntstf_female_prop` AS `") 
		+ (CString) Attr(element) + _T("_memstf_female_prop` \n\
		FROM `") + (CString) Attr(element) + _T("_coreind`;");
	pRS = new CStatSimRS( pEltDB, sSQL );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS = NULL;

	//secured tenure
	sSQL = _T("DROP TABLE IF EXISTS `hh_sectenur`");
	pEltDB->ExecuteSQL( sSQL );
	sSQL = "CREATE TABLE IF NOT EXISTS `hh_sectenur` (\n\
		SELECT `hh_Coreind`.`hhid`, \n\
		IF(`hh_ind`.`tenurstat`>=1 AND `hh_ind`.`tenurstat`<=3, 1, 0) as `hh_sectenur`, \n\
		IF(`hh_ind`.`tenurstat`>=1 AND `hh_ind`.`tenurstat`<=3, `hh_coreind`.`hh_totmem`, 0) as `memsectenur`, \n\
		IF(`hh_ind`.`tenurstat`>=1 AND `hh_ind`.`tenurstat`<=3, `hh_coreind`.`hh_totmem_male`, 0) as `memsectenur_male`, \n\
		IF(`hh_ind`.`tenurstat`>=1 AND `hh_ind`.`tenurstat`<=3, `hh_coreind`.`hh_totmem_female`, 0) as `memsectenur_female` \n\
		FROM `hh_ind` INNER JOIN `hh_coreind` USING (`hhID`));";
	pEltDB->ExecuteSQL( sSQL );

	//hh sectenur
	CrHHInd(HPQ_RAW, _MBCS("sectenur"), _MBCS("tothh"), _MBCS("hh_sectenur"), _MBCS("totpop"), _MBCS("hhID"));
	sFrTable.Format(_T("%s_sectenur"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS = NULL;

	//mem sectenur
	CrMemInd(HPQ_RAW, _MBCS("memsectenur"), _MBCS("totmem"), _MBCS("_Male"), _MBCS("_Female"), _MBCS("hh_sectenur"), _MBCS("totpop"),
		FALSE, _MBCS("hhID"), _MBCS("SUM"), 100,
		_MBCS("memsectenur"));
	sFrTable.Format(_T("%s_memsectenur"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS = NULL;

	//unempl1524
	sSQL = _T("DROP TABLE IF EXISTS `mem_unempl1524`");
	pEltDB->ExecuteSQL( sSQL );
	sSQL = "CREATE TABLE IF NOT EXISTS `mem_unempl1524` (\n\
		SELECT `memid`, \n\
		IF(`mem1524`=1 AND `labfor`=1, 1, 0) as `labfor1524`, \n\
		IF(`mem1524`=1 AND `labfor`=1 AND `sex`=1, 1, 0) as `labfor1524_male`, \n\
		IF(`mem1524`=1 AND `labfor`=1 AND `sex`=2, 1, 0) as `labfor1524_female`, \n\
		IF(`mem1524`=1 AND `labfor`=1 AND `unempl15ab`=1, 1, 0) as `unempl1524`, \n\
		IF(`mem1524`=1 AND `labfor`=1 AND `unempl15ab`=1 AND `sex`=1, 1, 0) as `unempl1524_male`, \n\
		IF(`mem1524`=1 AND `labfor`=1 AND `unempl15ab`=1 AND `sex`=2, 1, 0) as `unempl1524_female` \n\
		FROM `mem_ind`);";
	pEltDB->ExecuteSQL( sSQL );

	CrMemInd(HPQ_RAW, _MBCS("labfor1524"), _MBCS("totmem"), _MBCS("_Male"), _MBCS("_Female"), _MBCS("mem_unempl1524"), _MBCS("totpop"),
		FALSE, _MBCS("memID"), _MBCS("SUM"), 100,
		_MBCS("labfor1524"));
	CrMemInd(HPQ_RAW, _MBCS("unempl1524"), _MBCS("labfor1524"), _MBCS("_Male"), _MBCS("_Female"), _MBCS("mem_unempl1524"), _MBCS("labfor1524"),
		FALSE, _MBCS("memID"), _MBCS("SUM"), 100,
		_MBCS("unempl1524"));
	sFrTable.Format(_T("%s_unempl1524"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS = NULL;

	//assets
	sSQL = _T("DROP TABLE IF EXISTS `hh_assets`");
	pEltDB->ExecuteSQL( sSQL );
	sSQL = "CREATE TABLE IF NOT EXISTS `hh_assets` (\n\
		SELECT `hh_Coreind`.`hhid`, \n\
		`hh_ind`.`hh_wphone`, \n\
		`hh_ind`.`hh_wcomputer`, \n\
		`hh_ind`.`hh_wphone`*`hh_coreind`.`hh_totmem` AS `memwphone`, \n\
		`hh_ind`.`hh_wphone`*`hh_coreind`.`hh_totmem_male` AS `memwphone_male`, \n\
		`hh_ind`.`hh_wphone`*`hh_coreind`.`hh_totmem_female` AS `memwphone_female`, \n\
		`hh_ind`.`hh_wcomputer`*`hh_coreind`.`hh_totmem` AS `memwcomputer`, \n\
		`hh_ind`.`hh_wcomputer`*`hh_coreind`.`hh_totmem_male` AS `memwcomputer_male`, \n\
		`hh_ind`.`hh_wcomputer`*`hh_coreind`.`hh_totmem_female` AS `memwcomputer_female` \n\
		FROM `hh_ind` INNER JOIN `hh_coreind` USING (`hhID`));";
	pEltDB->ExecuteSQL( sSQL );

	CrHHInd(HPQ_RAW, _MBCS("wphone"), _MBCS("tothh"), _MBCS("hh_assets"), _MBCS("totpop"), _MBCS("hhID"));
	sFrTable.Format(_T("%s_wphone"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS = NULL;

	CrMemInd(HPQ_RAW, _MBCS("memwphone"), _MBCS("totmem"), _MBCS("_Male"), _MBCS("_Female"), _MBCS("hh_assets"), _MBCS("totpop"),
		FALSE, _MBCS("hhID"), _MBCS("SUM"), 100,
		_MBCS("memwphone"));
	sFrTable.Format(_T("%s_memwphone"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS = NULL;

	CrHHInd(HPQ_RAW, _MBCS("wcomputer"), _MBCS("tothh"), _MBCS("hh_assets"), _MBCS("totpop"), _MBCS("hhID"));
	sFrTable.Format(_T("%s_wcomputer"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS = NULL;

	CrMemInd(HPQ_RAW, _MBCS("memwcomputer"), _MBCS("totmem"), _MBCS("_Male"), _MBCS("_Female"), _MBCS("hh_assets"), _MBCS("totpop"),
		FALSE, _MBCS("hhID"), _MBCS("SUM"), 100,
		_MBCS("memwcomputer"));
	sFrTable.Format(_T("%s_memwcomputer"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));
	delete pRS; pRS = NULL;

}

void CStatSimElt::DropPartialCore(int given) /*Given indicator - always minus 1*/
{
	CString sSQL;
	
	//hh
	sSQL.Format(_T("DROP TABLE IF EXISTS `%s_hhcoreind!%s`;"), (CString) Attr(element), sHHCoreInd[given-1][VAR]);
	pEltDB->ExecuteSQL( sSQL );

	//mem
	sSQL.Format(_T("DROP TABLE IF EXISTS `%s_memcoreind!%s`;"), (CString) Attr(element), sHHCoreInd[given-1][VAR]);
	pEltDB->ExecuteSQL( sSQL );

}

void CStatSimElt::CrIncDistCore(LPCSTR sIncDistVar, LPCSTR sSrc)
{

	CString sSQL;
	
	//create the table
	sSQL = sSQLCreateIncDistCoreIndTable(sIncDistVar);
	pEltDB->ExecuteSQL( sSQL );

	sSQL = sSQLInsertIncDistCoreIndTable(sIncDistVar, sSrc);
	pEltDB->ExecuteSQL( sSQL );

	//FILE *pFile;
	//pFile = fopen("c:\\mysql.sql"), _T("w");
	//fprintf(pFile, ConstChar(sSQL));
	//fclose(pFile);

}

void CStatSimElt::CrPartialCore(int given)
{
	CString sSQL;
	
	//create the table
	sSQL = sSQLCreatePartialTableHH(given);
	pEltDB->ExecuteSQL( sSQL );
	//create the table - mem
	sSQL = sSQLCreatePartialTableMEM(given);
	pEltDB->ExecuteSQL( sSQL );

	//fill the table
	sSQL = sSQLInsertPartialTableHH(given);
	pEltDB->ExecuteSQL( sSQL );
	//fill the table - mem
	sSQL = sSQLInsertPartialTableMEM(given);
	pEltDB->ExecuteSQL( sSQL );
	
	//FILE *pFile;
	//pFile = fopen("c:\\mysql.sql", _T("w"));
	//fprintf(pFile, ConstChar(sSQL));
	//fclose(pFile);

}
void CStatSimElt::CrCoreInd(long qnrID, BOOL update, CStatSimWnd *pLabel, CStatSimBar *pProgBar)
{
	int n_ind =14;
	//std::vector<BOOL> ind_ind;
	BOOL* ind_ind = new BOOL[n_ind];
	for (int _i=0; _i<n_ind; _i++) {
		//ind_ind.push_back(TRUE);
		//ind_ind[_i] = FALSE;
		ind_ind[_i] = TRUE;	//the true setting for all
	}

	//my indicator settings
	//ind_ind[POVP-1] = true;
	//ind_ind[SUBP-1] = true;
	

	SetQnrID(qnrID);
	//prCaption.Format(_T("StatSim: Updating %s", TargetTable);
	
	//if (pLabel!=NULL) {
	//	if (pLabel->GetSafeHwnd()) pLabel->SetText(prCaption);
	//}				
	
	//if (pProgBar!=NULL) {
	//	if (pProgBar->GetSafeHwnd()) pProgBar->SetPos((int) m_nPercent);
	//}

	CStatSimRS *pRS = NULL;
	CString sFrTable, sToTable, sSQL;
	sFrTable.Format(_T("%s_TotPop"), (CString) Attr(element));
	sToTable.Format(_T("%s_CoreInd"), (CString) Attr(element));

	//drop first the table; if selective, i.e. at least one is not to be processed, do not delete
	BOOL drop_core=TRUE;
	for (int _i=0; _i<n_ind; _i++) {
		if (ind_ind[_i]==FALSE && currElt!=HH) {
			drop_core=FALSE;
			break;
		}
	}
	//for (std::vector<BOOL>::iterator ind_it=ind_ind.begin(); 
	//	ind_it!=ind_ind.end(); ++ind_it) {
	//	if (*ind_it==FALSE) {
	//		drop_core=FALSE;
	//		break;
	//	}
	//}
	if (drop_core) {

		sSQL.Format(_T("DROP TABLE IF EXISTS %s_CoreInd"), (CString) Attr(element));
		pEltDB->ExecuteSQL( sSQL );
		
		//create table of core indicators
		pEltDB->ExecuteSQL(sSQLCoreInd());
	}
	
	//return if not to be filled
	if (!update)
		return;

	if(currElt==HH) {
		sSQL.Format(_T("SELECT * FROM `%s`;"), sFrTable);
		pEltDB->InsertFromSQL(sSQL, sToTable);	
	}
	else {
		if (drop_core) { //do insert from totpop only if core is dropped
			pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
			pEltDB->InsertRecords(pRS, sToTable, pLabel, pProgBar, FALSE, TRUE);	
			delete pRS; pRS = NULL;
		}
	}

	if (currElt==HH) {
		/*"`hhHead`, \n\*/
		//Males and females
		sSQL = "CREATE TABLE `hh_coreind_temp` SELECT \n\
			`hhID`, \n\
			`hcn`, \n\
			`hcn_NRDB`, \n\
			`geopoint_hh_long`, \n\
			`geopoint_hh_lat`, \n\
			`nmem04`, \n\
			`nmem04_male`, \n\
			`nmem04_female`, \n\
			`nmem05`, \n\
			`nmem05_male`, \n\
			`nmem05_female`, \n\
			`nmaln05`, \n\
			`nmaln05_male`, \n\
			`nmaln05_female`, \n\
			`ndeath05`, \n\
			`ndeath05_male`, \n\
			`ndeath05_female`, \n\
			`nmem01`, \n\
			`nmem01_male`, \n\
			`nmem01_female`, \n\
			`ndeath01`, \n\
			`ndeath01_male`, \n\
			`ndeath01_female`, \n\
			`ndeathpreg`, \n\
			`ndeathpreg_male`, \n\
			`ndeathpreg_female`, \n\
			IF(`nmem05`>0, 1, 0) AS `wmem05`, \n\
			IF(`nmem04`>0, 1, 0) AS `wmem04`, \n\
			IF( (`nmem05`>0 AND `nmaln05`>0),1, IF( (`nmem05`>0 AND `nmaln05`=0),0, 0)) AS `wMaln05`, \n\
			IF( (`nmem05`>0 AND `nmaln05`>0),'With malnourished children', IF( (`nmem05`>0 AND `nmaln05`=0),'Without malnourished children', 'Without member 0-5')) AS `HH_wMaln05`, \n\
			IF(`nmem04`>0 OR `ndeath04`>0, 1, 0) AS `wmem04d`, \n\
			IF( (`ndeath04`>0),1, IF( (`nmem04`>0 AND `ndeath04`=0),0, 0)) AS `wdeath04`, \n\
			IF( (`ndeath04`>0),'With child death', IF( (`nmem04`>0 AND `ndeath04`=0),'Without child death', 'Without member 0-5 and no child death')) AS `HH_wdeath04`, \n\
			IF(`nmem05`>0 OR `ndeath05`>0, 1, 0) AS `wmem05d`, \n\
			IF( (`ndeath05`>0),1, IF( (`nmem05`>0 AND `ndeath05`=0),0, 0)) AS `wDeath05`, \n\
			IF( (`ndeath05`>0),'With child death', IF( (`nmem05`>0 AND `ndeath05`=0),'Without child death', 'Without member 0-5 and no child death')) AS `HH_wDeath05`, \n\
			IF(`nmem01`>0, 1, 0) AS `wmem01`, \n\
			IF(`nmem01`>0 OR `ndeathpreg`>0, 1, 0) AS `wmem01d`, \n\
			IF( (`ndeathpreg`>0),1, IF( (`nmem01`>0 AND `ndeathpreg`=0),0, 0)) AS `wDeathpreg`, \n\
			IF( (`ndeathpreg`>0),'With deaths due to pregnancy related causes', IF( (`nmem01`>0 AND `ndeathpreg`=0),'Without death due to pregnancy related causes', 'Not applicable')) AS `HH_wDeathpreg`, \n\
			`Squat`, \n\
			`nSquat`, \n\
			`nSquat_male`, \n\
			`nSquat_female`, \n\
			`MSH`, \n\
			`nMSH`, \n\
			`nMSH_male`, \n\
			`nMSH_female`, \n\
			`ntSWS`, \n\
			`nntSWS`, \n\
			`nntSWS_male`, \n\
			`nntSWS_female`, \n\
			`ntSTF`, \n\
			`nntSTF`, \n\
			`nntSTF_male`, \n\
			`nntSTF_female`, \n\
			IF(`Squat`=1,'Informal settler', 'Formal settler') as HH_Squat, \n\
			IF(`MSH`=1,'Living in makeshift housing', 'Not living in makeshift housing') as HH_MSH, \n\
			IF(`ntSWS`=1,'Without access to safe water', 'With access to safe water') as HH_ntSWS, \n\
			IF(`ntSTF`=1,'Without access to sanitary toilet', 'With access to sanitary toilet') as HH_ntSTF, \n\
			`nmem611`, \n\
			`nmem611_male`, \n\
			`nmem611_female`, \n\
			`nntElem611`, \n\
			`nntElem611_male`, \n\
			`nntElem611_female`, \n\
			`nmem1215`, \n\
			`nmem1215_male`, \n\
			`nmem1215_female`, \n\
			`nntHS1215`, \n\
			`nntHS1215_male`, \n\
			`nntHS1215_female`, \n\
			`nmem612`, \n\
			`nmem612_male`, \n\
			`nmem612_female`, \n\
			`nntElem612`, \n\
			`nntElem612_male`, \n\
			`nntElem612_female`, \n\
			`nmem1316`, \n\
			`nmem1316_male`, \n\
			`nmem1316_female`, \n\
			`nntHS1316`, \n\
			`nntHS1316_male`, \n\
			`nntHS1316_female`, \n\
			`nmem615`, \n\
			`nmem615_male`, \n\
			`nmem615_female`, \n\
			`nntSch615`, \n\
			`nntSch615_male`, \n\
			`nntSch615_female`, \n\
			`nmem616`, \n\
			`nmem616_male`, \n\
			`nmem616_female`, \n\
			`nntSch616`, \n\
			`nntSch616_male`, \n\
			`nntSch616_female`, \n\
			`nmem1721`, \n\
			`nmem1721_male`, \n\
			`nmem1721_female`, \n\
			`ntert1721`, \n\
			`ntert1721_male`, \n\
			`ntert1721_female`, \n\
			`nmem10ab`, \n\
			`nmem10ab_male`, \n\
			`nmem10ab_female`, \n\
			`nntLiter10ab`, \n\
			`nntLiter10ab_male`, \n\
			`nntLiter10ab_female`, \n\
			`nmem1524`, \n\
			`nmem1524_male`, \n\
			`nmem1524_female`, \n\
			`nLiter1524`, \n\
			`nLiter1524_male`, \n\
			`nLiter1524_female`, \n\
			IF(`nmem611`>0, 1, 0) AS `wmem611`, \n\
			IF( (`nmem611`>0 AND `nntelem611`>0),1, IF( (`nmem611`>0 AND `nntelem611`=0),0, 0)) AS `wNtelem611`, \n\
			IF( (`nmem611`>0 AND `nntelem611`>0),'With members not in elementary', IF( (`nmem611`>0 AND `nntelem611`=0),'All members attending elementary', 'No member 6-11')) AS `HH_wNtelem611`, \n\
			IF(`nmem1215`>0, 1, 0) AS `wmem1215`, \n\
			IF( (`nmem1215`>0 AND `nnths1215`>0),1, IF( (`nmem1215`>0 AND `nnths1215`=0),0, 0)) AS `wNths1215`, \n\
			IF( (`nmem1215`>0 AND `nnths1215`>0),'With members not in high school', IF( (`nmem1215`>0 AND `nnths1215`=0),'All members attending high school', 'No member 12-15')) AS `HH_wNths1215`, \n\
			IF(`nmem612`>0, 1, 0) AS `wmem612`, \n\
			IF( (`nmem612`>0 AND `nntelem612`>0),1, IF( (`nmem612`>0 AND `nntelem612`=0),0, 0)) AS `wNtelem612`, \n\
			IF( (`nmem612`>0 AND `nntelem612`>0),'With members not in elementary', IF( (`nmem612`>0 AND `nntelem612`=0),'All members attending elementary', 'No member 6-12')) AS `HH_wNtelem612`, \n\
			IF(`nmem1316`>0, 1, 0) AS `wmem1316`, \n\
			IF( (`nmem1316`>0 AND `nnths1316`>0),1, IF( (`nmem1316`>0 AND `nnths1316`=0),0, 0)) AS `wNths1316`, \n\
			IF( (`nmem1316`>0 AND `nnths1316`>0),'With members not in high school', IF( (`nmem1316`>0 AND `nnths1316`=0),'All members attending high school', 'No member 13-16')) AS `HH_wNths1316`, \n\
			IF(`nmem615`>0, 1, 0) AS `wmem615`, \n\
			IF( (`nmem615`>0 AND `nntsch615`>0),1, IF( (`nmem615`>0 AND `nntsch615`=0),0, 0)) AS `wNtsch615`, \n\
			IF( (`nmem615`>0 AND `nntsch615`>0),'With member not in school', IF( (`nmem615`>0 AND `nntsch615`=0),'All members attending school', 'No member 6-15')) AS `HH_wNtsch615`, \n\
			IF(`nmem616`>0, 1, 0) AS `wmem616`, \n\
			IF( (`nmem616`>0 AND `nntsch616`>0),1, IF( (`nmem616`>0 AND `nntsch616`=0),0, 0)) AS `wNtsch616`, \n\
			IF( (`nmem616`>0 AND `nntsch616`>0),'With member not in school', IF( (`nmem616`>0 AND `nntsch616`=0),'All members attending school', 'No member 6-16')) AS `HH_wNtsch616`, \n\
			IF(`nmem10ab`>0, 1, 0) AS `wmem10ab`, \n\
			IF( (`nmem10ab`>0 AND `nntLiter10ab`>0),1, IF( (`nmem10ab`>0 AND `nntLiter10ab`=0),0, 0)) AS `wNtLiter10ab`, \n\
			IF( (`nmem10ab`>0 AND `nntLiter10ab`>0),'With illiterate member', IF( (`nmem10ab`>0 AND `nntLiter10ab`=0),'All members are literate', 'Not Applicable')) AS `HH_wNtLiter10ab`, \n\
			IF(`nmem1524`>0, 1, 0) AS `wmem1524`, \n\
			IF(`nmem1524`>0 AND (`nmem1524`=`nLiter1524`), 1, 0) AS `allliter1524`, \n\
			IF(`nmem1721`>0, 1, 0) AS `wmem1721`, \n\
			IF(`nmem1721`>0 AND (`nmem1721`=`ntert1721`), 1, 0) AS `alltert1721`, \n\
			`Povp`, \n\
			`nPovp`, \n\
			`nPovp_male`, \n\
			`nPovp_female`, \n\
			`Subp`, \n\
			`nSubp`, \n\
			`nSubp_male`, \n\
			`nSubp_female`, \n\
			`Fshort`, \n\
			`nFShort`, \n\
			`nFShort_male`, \n\
			`nFShort_female`, \n\
			IF(`Povp`=1,'Poor', 'Non-poor') as HH_Povp, \n\
			IF(`Subp`=1,'Subsistently poor', 'Subsistently non-poor') as HH_Subp, \n\
			IF(`Fshort`=1,'Experienced food shortage', 'Did not experience food shortage') as HH_Fshort, \n\
			`nLabFor`, \n\
			`nLabFor_male`, \n\
			`nLabFor_female`, \n\
			`nUnempl15ab`, \n\
			`nUnempl15ab_male`, \n\
			`nUnempl15ab_female`, \n\
			IF(`nlabfor`>0, 1, 0) AS `wlabfor`, \n\
			IF( (`nlabfor`>0 AND `nunempl15ab`>0),1, IF( (`nlabfor`>0 AND `nunempl15ab`=0),0, 0)) AS `wUnempl15ab`, \n\
			IF( (`nlabfor`>0 AND `nunempl15ab`>0),'With unemployed members of the labor force', IF( (`nlabfor`>0 AND `nunempl15ab`=0),'All members in the labor force are employed', 'No members of the labor force')) AS `HH_wUnempl15ab`, \n\
			`nVictCr`, \n\
			`nVictCr_male`, \n\
			`nVictCr_female`, \n\
			IF( `nVictcr`>0,1,0 ) AS `wVictcr`, \n\
			IF( `nVictcr`>0,'With victims of crime','No victims of crime' ) AS `HH_wVictcr`, \n\
			`hh_totmem_sq`, \n\
			`nmem014`, \n\
			`nmem1564`, \n\
			`nmem65ab`, \n\
			`depratio`, \n\
			`hhage`, \n\
			`hheduca2`, \n\
			`hheduca3`, \n\
			`hheduca4`, \n\
			`hheduca5`, \n\
			`hheduca67`, \n\
			`hhntagri`, \n\
			If(`urb`=2,1,0) as `urb_loc` \n\
			FROM ( ( SELECT \n\
			`hh_ind`.`hhID`, \n\
			`hh`.`hhHead`, \n\
			`hh`.`hcn`, \n\
			CAST(`hh`.`hcn` as char) as `hcn_NRDB`, \n\
			`geopoint_hh_long`,  \n\
			`geopoint_hh_lat`, \n\
			SUM(`mem_ind`.`mem04`) as `nmem04`, \n\
			SUM(`mem_ind`.`mem04_male`) as `nmem04_male`, \n\
			SUM(`mem_ind`.`mem04_female`) as `nmem04_female`, \n\
			SUM(`mem_ind`.`mem05`) as `nmem05`, \n\
			SUM(`mem_ind`.`mem05_male`) as `nmem05_male`, \n\
			SUM(`mem_ind`.`mem05_female`) as `nmem05_female`, \n\
			SUM(`mem_ind`.`maln05`) as `nmaln05`, \n\
			SUM(`mem_ind`.`maln05_male`) as `nmaln05_male`, \n\
			SUM(`mem_ind`.`maln05_female`) as `nmaln05_female`, \n\
			`hh_ind`.`death04` as `ndeath04`, \n\
			`hh_ind`.`death04_male` as `ndeath04_male`, \n\
			`hh_ind`.`death04_female` as `ndeath04_female`, \n\
			`hh_ind`.`death05` as `ndeath05`, \n\
			`hh_ind`.`death05_male` as `ndeath05_male`, \n\
			`hh_ind`.`death05_female` as `ndeath05_female`, \n\
			SUM(`mem_ind`.`mem01`) as `nmem01`, \n\
			SUM(`mem_ind`.`mem01_male`) as `nmem01_male`, \n\
			SUM(`mem_ind`.`mem01_female`) as `nmem01_female`, \n\
			`hh_ind`.`death01` as `ndeath01`, \n\
			`hh_ind`.`death01_male` as `ndeath01_male`, \n\
			`hh_ind`.`death01_female` as `ndeath01_female`, \n\
			`hh_ind`.`deathpreg` as `ndeathpreg`, \n\
			`hh_ind`.`deathpreg_male` as `ndeathpreg_male`, \n\
			`hh_ind`.`deathpreg_female` as `ndeathpreg_female`, \n\
			`hh_ind`.`HH_Squat` as `Squat`, \n\
			(`hh_ind`.`HH_Squat`*`hh_totpop`.`hh_totmem`) AS `nSquat`, \n\
			(`hh_ind`.`HH_Squat`*`hh_totpop`.`hh_totmem_male`) AS `nSquat_male`, \n\
			(`hh_ind`.`HH_Squat`*`hh_totpop`.`hh_totmem_female`) AS `nSquat_female`, \n\
			`hh_ind`.`HH_MSH` as `MSH`, \n\
			(`hh_ind`.`HH_MSH`*`hh_totpop`.`hh_totmem`) AS `nMSH`, \n\
			(`hh_ind`.`HH_MSH`*`hh_totpop`.`hh_totmem_male`) AS `nMSH_male`, \n\
			(`hh_ind`.`HH_MSH`*`hh_totpop`.`hh_totmem_female`) AS `nMSH_female`, \n\
			`hh_ind`.`HH_ntSWS` as `ntSWS`, \n\
			(`hh_ind`.`HH_ntSWS`*`hh_totpop`.`hh_totmem`) AS `nntSWS`, \n\
			(`hh_ind`.`HH_ntSWS`*`hh_totpop`.`hh_totmem_male`) AS `nntSWS_male`, \n\
			(`hh_ind`.`HH_ntSWS`*`hh_totpop`.`hh_totmem_female`) AS `nntSWS_female`, \n\
			`hh_ind`.`HH_ntSTF` as `ntSTF`, \n\
			(`hh_ind`.`HH_ntSTF`*`hh_totpop`.`hh_totmem`) AS `nntSTF`, \n\
			(`hh_ind`.`HH_ntSTF`*`hh_totpop`.`hh_totmem_male`) AS `nntSTF_male`, \n\
			(`hh_ind`.`HH_ntSTF`*`hh_totpop`.`hh_totmem_female`) AS `nntSTF_female`, \n\
			SUM(`mem_ind`.`mem611`) as `nmem611`, \n\
			SUM(`mem_ind`.`mem611_male`) as `nmem611_male`, \n\
			SUM(`mem_ind`.`mem611_female`) as `nmem611_female`, \n\
			SUM(`mem_ind`.`ntElem611`) as `nntElem611`, \n\
			SUM(`mem_ind`.`ntElem611_male`) as `nntElem611_male`, \n\
			SUM(`mem_ind`.`ntElem611_female`) as `nntElem611_female`, \n\
			SUM(`mem_ind`.`mem612`) as `nmem612`, \n\
			SUM(`mem_ind`.`mem612_male`) as `nmem612_male`, \n\
			SUM(`mem_ind`.`mem612_female`) as `nmem612_female`, \n\
			SUM(`mem_ind`.`ntElem612`) as `nntElem612`, \n\
			SUM(`mem_ind`.`ntElem612_male`) as `nntElem612_male`, \n\
			SUM(`mem_ind`.`ntElem612_female`) as `nntElem612_female`, \n\
			SUM(`mem_ind`.`mem1215`) as `nmem1215`, \n\
			SUM(`mem_ind`.`mem1215_male`) as `nmem1215_male`, \n\
			SUM(`mem_ind`.`mem1215_female`) as `nmem1215_female`, \n\
			SUM(`mem_ind`.`ntHS1215`) as `nntHS1215`, \n\
			SUM(`mem_ind`.`ntHS1215_male`) as `nntHS1215_male`, \n\
			SUM(`mem_ind`.`ntHS1215_female`) as `nntHS1215_female`, \n\
			SUM(`mem_ind`.`mem1316`) as `nmem1316`, \n\
			SUM(`mem_ind`.`mem1316_male`) as `nmem1316_male`, \n\
			SUM(`mem_ind`.`mem1316_female`) as `nmem1316_female`, \n\
			SUM(`mem_ind`.`ntHS1316`) as `nntHS1316`, \n\
			SUM(`mem_ind`.`ntHS1316_male`) as `nntHS1316_male`, \n\
			SUM(`mem_ind`.`ntHS1316_female`) as `nntHS1316_female`, \n\
			SUM(`mem_ind`.`mem615`) as `nmem615`, \n\
			SUM(`mem_ind`.`mem615_male`) as `nmem615_male`, \n\
			SUM(`mem_ind`.`mem615_female`) as `nmem615_female`, \n\
			SUM(`mem_ind`.`ntSch615`) as `nntSch615`, \n\
			SUM(`mem_ind`.`ntSch615_male`) as `nntSch615_male`, \n\
			SUM(`mem_ind`.`ntSch615_female`) as `nntSch615_female`, \n\
			SUM(`mem_ind`.`mem616`) as `nmem616`, \n\
			SUM(`mem_ind`.`mem616_male`) as `nmem616_male`, \n\
			SUM(`mem_ind`.`mem616_female`) as `nmem616_female`, \n\
			SUM(`mem_ind`.`ntSch616`) as `nntSch616`, \n\
			SUM(`mem_ind`.`ntSch616_male`) as `nntSch616_male`, \n\
			SUM(`mem_ind`.`ntSch616_female`) as `nntSch616_female`, \n\
			SUM(`mem_ind`.`mem1721`) as `nmem1721`, \n\
			SUM(`mem_ind`.`mem1721_male`) as `nmem1721_male`, \n\
			SUM(`mem_ind`.`mem1721_female`) as `nmem1721_female`, \n\
			SUM(`mem_ind`.`tert1721`) as `ntert1721`, \n\
			SUM(`mem_ind`.`tert1721_male`) as `ntert1721_male`, \n\
			SUM(`mem_ind`.`tert1721_female`) as `ntert1721_female`, \n\
			SUM(`mem_ind`.`mem10ab`) as `nmem10ab`, \n\
			SUM(`mem_ind`.`mem10ab_male`) as `nmem10ab_male`, \n\
			SUM(`mem_ind`.`mem10ab_female`) as `nmem10ab_female`, \n\
			SUM(`mem_ind`.`ntLiter10ab`) as `nntLiter10ab`, \n\
			SUM(`mem_ind`.`ntLiter10ab_male`) as `nntLiter10ab_male`, \n\
			SUM(`mem_ind`.`ntLiter10ab_female`) as `nntLiter10ab_female`, \n\
			SUM(`mem_ind`.`mem1524`) as `nmem1524`, \n\
			SUM(`mem_ind`.`mem1524_male`) as `nmem1524_male`, \n\
			SUM(`mem_ind`.`mem1524_female`) as `nmem1524_female`, \n\
			SUM(`mem_ind`.`liter1524`) as `nliter1524`, \n\
			SUM(`mem_ind`.`liter1524_male`) as `nliter1524_male`, \n\
			SUM(`mem_ind`.`liter1524_female`) as `nliter1524_female`, \n\
			`hh_ind`.`HH_Povp` as `Povp`, \n\
			(`hh_ind`.`HH_Povp`*`hh_totpop`.`hh_totmem`) AS `nPovp`, \n\
			(`hh_ind`.`HH_Povp`*`hh_totpop`.`hh_totmem_male`) AS `nPovp_male`, \n\
			(`hh_ind`.`HH_Povp`*`hh_totpop`.`hh_totmem_female`) AS `nPovp_female`, \n\
			`hh_ind`.`HH_Subp` as `Subp`, \n\
			(`hh_ind`.`HH_Subp`*`hh_totpop`.`hh_totmem`) AS `nSubp`, \n\
			(`hh_ind`.`HH_Subp`*`hh_totpop`.`hh_totmem_male`) AS `nSubp_male`, \n\
			(`hh_ind`.`HH_Subp`*`hh_totpop`.`hh_totmem_female`) AS `nSubp_female`, \n\
			`hh_ind`.`HH_Fshort` as `Fshort`, \n\
			(`hh_ind`.`HH_FShort`*`hh_totpop`.`hh_totmem`) AS `nFShort`, \n\
			(`hh_ind`.`HH_FShort`*`hh_totpop`.`hh_totmem_male`) AS `nFShort_male`, \n\
			(`hh_ind`.`HH_FShort`*`hh_totpop`.`hh_totmem_female`) AS `nFShort_female`, \n\
			SUM(`mem_ind`.`LabFor`) as `nLabFor`, \n\
			SUM(`mem_ind`.`LabFor_male`) as `nLabFor_male`, \n\
			SUM(`mem_ind`.`LabFor_female`) as `nLabFor_female`, \n\
			SUM(`mem_ind`.`Unempl15ab`) as `nUnempl15ab`, \n\
			SUM(`mem_ind`.`Unempl15ab_male`) as `nUnempl15ab_male`, \n\
			SUM(`mem_ind`.`Unempl15ab_female`) as `nUnempl15ab_female`, \n\
			`hh_ind`.`VictCr` as `nVictCr`, \n\
			`hh_ind`.`VictCr_male` as `nVictCr_male`, \n\
			`hh_ind`.`VictCr_female` as `nVictCr_female`, \n\
			`hh_ind`.`hsize_sq` as `hh_totmem_sq`, \n\
			SUM(`mem_ind`.`mem014`) as `nmem014`, \n\
			SUM(`mem_ind`.`mem1564`) as `nmem1564`, \n\
			SUM(`mem_ind`.`mem65ab`) as `nmem65ab`, \n\
			(SUM(`mem_ind`.`mem014`)+SUM(`mem_ind`.`mem65ab`))/SUM(`mem_ind`.`mem1564`) as `depratio`, \n\
			SUM(If(`mem_ind`.`reln`=1, `mem_ind`.`age_yr`,0)) as `hhage`, \n\
			SUM(If(`mem_ind`.`reln`=1, `mem_ind`.`educa2`,0)) as `hheduca2`, \n\
			SUM(If(`mem_ind`.`reln`=1, `mem_ind`.`educa3`,0)) as `hheduca3`, \n\
			SUM(If(`mem_ind`.`reln`=1, `mem_ind`.`educa4`,0)) as `hheduca4`, \n\
			SUM(If(`mem_ind`.`reln`=1, `mem_ind`.`educa5`,0)) as `hheduca5`, \n\
			SUM(If(`mem_ind`.`reln`=1, `mem_ind`.`educa67`,0)) as `hheduca67`, \n\
			SUM(If(`mem_ind`.`reln`=1, `mem_ind`.`ntagri`,0)) as `hhntagri`, \n\
			`hh_ind`.`urb` as `urb` \n\
			FROM `hh` INNER JOIN `hh_totpop` ON (`hh_totpop`.`hhID` = `hh`.`hhID`) INNER JOIN `hh_ind` ON (`hh_ind`.`hhID` = `hh`.`hhID`) INNER JOIN `mem_ind` ON (`hh`.`regn` = `mem_ind`.`regn` AND `hh`.`prov` = `mem_ind`.`prov` AND `hh`.`mun` = `mem_ind`.`mun` AND `hh`.`brgy` = `mem_ind`.`brgy` AND `hh`.`purok` = `mem_ind`.`purok` AND `hh`.`hcn` = `mem_ind`.`hcn`) \n\
			GROUP BY `hh`.`regn`, `hh`.`prov`, `hh`.`mun`, `hh`.`brgy`, `hh`.`purok`, `hh`.`hcn` ) \n\
			AS `HH_CoreInd_Temp` )\n\
			;";

/*			`hh_ind`.`hh_wtv` as `wtv`, \n\
			`hh_ind`.`hh_wvhs` as `wvhs`, \n\
			`hh_ind`.`hh_wref` as `wref`, \n\
			`hh_ind`.`hh_wwmach` as `wwmach`, \n\
			`hh_ind`.`hh_wairc` as `wairc`, \n\
			`hh_ind`.`hh_wcar` as `wcar`, \n\
			`hh_ind`.`hh_wphone` as `wphone`, \n\
			`hh_ind`.`hh_wcomputer` as `wcomputer`, \n\
			`hh_ind`.`hh_wmicrow` as `wmicrow`, \n\
			`hh_ind`.`hh_welec` as `welec`, \n\
			`hh_ind`.`hh_toilscore` as `toilscore`, \n\

/*			`wtv`, \n\
			`wvhs`, \n\
			`wref`, \n\
			`wwmach`, \n\
			`wairc`, \n\
			`wcar`, \n\
			`wphone`, \n\
			`wcomputer`, \n\
			`wmicrow`, \n\
			`welec`, \n\
			`toilscore`, \n\
*/

/*		sSQL = _T("SELECT \n\
			`hhID`, \n\
			`hcn`, \n\
			`hcn_NRDB`, \n\
			`nmem05`, \n\
			`nmaln05`, \n\
			`ndeath05`, \n\
			`nmem01`, \n\
			`ndeathpreg`, \n\
			IF( (`nmem05`>0 AND `nmaln05`>0),1, IF( (`nmem05`>0 AND `nmaln05`=0),0, 0)) AS `wMaln05`, \n\
			IF( (`nmem05`>0 AND `nmaln05`>0),'With malnourished children', IF( (`nmem05`>0 AND `nmaln05`=0),'Without malnourished children', 'Without member 0-5')) AS `HH_wMaln05`, \n\
			IF( (`ndeath05`>0),1, IF( (`nmem05`>0 AND `ndeath05`=0),0, 0)) AS `wDeath05`, \n\
			IF( (`ndeath05`>0),'With child death', IF( (`nmem05`>0 AND `ndeath05`=0),'Without child death', 'Without member 0-5 and no child death')) AS `HH_wDeath05`, \n\
			IF( (`ndeathpreg`>0),1, IF( (`nmem01`>0 AND `ndeathpreg`=0),0, 0)) AS `wDeathpreg`, \n\
			IF( (`ndeathpreg`>0),'With deaths due to pregnancy related causes', IF( (`nmem01`>0 AND `ndeathpreg`=0),'Without death due to pregnancy related causes', 'Not applicable')) AS `HH_wDeathpreg`, \n\
			`Squat`, \n\
			`MSH`, \n\
			`ntSWS`, \n\
			`ntSTF`, \n\
			IF(`Squat`=1,'Informal settler', 'Formal settler') as HH_Squat, \n\
			IF(`MSH`=1,'Living in makeshift housing', 'Not living in makeshift housing') as HH_MSH, \n\
			IF(`ntSWS`=1,'Without access to safe water', 'With access to safe water') as HH_ntSWS, \n\
			IF(`ntSTF`=1,'Without access to sanitary toilet', 'With access to sanitary toilet') as HH_ntSTF, \n\
			`nmem612`, \n\
			`nntElem612`, \n\
			`nmem1316`, \n\
			`nntHs1316`, \n\
			`nmem616`, \n\
			`nntSch616`, \n\
			`nmem10ab`, \n\
			`nntLiter10ab`, \n\
			IF( (`nmem612`>0 AND `nntelem612`>0),1, IF( (`nmem612`>0 AND `nntelem612`=0),0, 0)) AS `wNtelem612`, \n\
			IF( (`nmem612`>0 AND `nntelem612`>0),'With members not in elementary', IF( (`nmem612`>0 AND `nntelem612`=0),'All members attending elementary', 'No member 6-12')) AS `HH_wNtelem612`, \n\
			IF( (`nmem1316`>0 AND `nnths1316`>0),1, IF( (`nmem1316`>0 AND `nnths1316`=0),0, 0)) AS `wNths1316`, \n\
			IF( (`nmem1316`>0 AND `nnths1316`>0),'With members not in high school', IF( (`nmem1316`>0 AND `nnths1316`=0),'All members attending high school', 'No member 13-16')) AS `HH_wNths1316`, \n\
			IF( (`nmem616`>0 AND `nntsch616`>0),1, IF( (`nmem616`>0 AND `nntsch616`=0),0, 0)) AS `wNtsch616`, \n\
			IF( (`nmem616`>0 AND `nntsch616`>0),'With member not in school', IF( (`nmem616`>0 AND `nntsch616`=0),'All members attending school', 'No member 6-16')) AS `HH_wNtsch616`, \n\
			IF( (`nmem10ab`>0 AND `nntLiter10ab`>0),1, IF( (`nmem10ab`>0 AND `nntLiter10ab`=0),0, 0)) AS `wNtLiter10ab`, \n\
			IF( (`nmem10ab`>0 AND `nntLiter10ab`>0),'With illiterate member', IF( (`nmem10ab`>0 AND `nntLiter10ab`=0),'All members are literate', 'Not Applicable')) AS `HH_wNtLiter10ab`, \n\
			`Povp`, \n\
			`Subp`, \n\
			`Fshort`, \n\
			IF(`Povp`=1,'Poor', 'Non-poor') as HH_Povp, \n\
			IF(`Subp`=1,'Subsistently poor', 'Subsistently non-poor') as HH_Subp, \n\
			IF(`Fshort`=1,'Experienced food shortage', 'Did not experience food shortage') as HH_Fshort, \n\
			`nlabfor`, \n\
			`nUnempl15ab`, \n\
			IF( (`nlabfor`>0 AND `nunempl15ab`>0),1, IF( (`nlabfor`>0 AND `nunempl15ab`=0),0, 0)) AS `wUnempl15ab`, \n\
			IF( (`nlabfor`>0 AND `nunempl15ab`>0),'With unemployed members of the labor force', IF( (`nlabfor`>0 AND `nunempl15ab`=0),'All members in the labor force are employed', 'No members of the labor force')) AS `HH_wUnempl15ab`, \n\
			`nVictcr`, \n\
			IF( `nVictcr`>0,1,0 ) AS `wVictcr`, \n\
			IF( `nVictcr`>0,'With victims of crime','No victims of crime' ) AS `HH_wVictcr` \n\
			FROM ( ( SELECT \n\
			`hh_ind`.`hhID`, \n\
			`hh`.`hhHead`, \n\
			`hh`.`hcn`, \n\
			CAST(`hh`.`hcn` as char) as `hcn_NRDB`, \n\
			SUM(`mem_ind`.`mem05`) as `nmem05`, \n\
			SUM(`mem_ind`.`maln05`) as `nmaln05`, \n\
			`hh_ind`.`death05` as `ndeath05`, \n\
			SUM(`mem_ind`.`mem01`) as `nmem01`, \n\
			`hh_ind`.`deathpreg` as `ndeathpreg`, \n\
			`hh_ind`.`HH_Squat` as `Squat`, \n\
			`hh_ind`.`HH_MSH` as `MSH`, \n\
			`hh_ind`.`HH_ntSWS` as `ntSWS`, \n\
			`hh_ind`.`HH_ntSTF` as `ntSTF`, \n\
			SUM(`mem_ind`.`mem612`) as `nmem612`, \n\
			SUM(`mem_ind`.`ntElem612`) as `nntElem612`, \n\
			SUM(`mem_ind`.`mem1316`) as `nmem1316`, \n\
			SUM(`mem_ind`.`ntHs1316`) as `nntHs1316`, \n\
			SUM(`mem_ind`.`mem616`) as `nmem616`, \n\
			SUM(`mem_ind`.`ntSch616`) as `nntSch616`, \n\
			SUM(`mem_ind`.`mem10ab`) as `nmem10ab`, \n\
			SUM(`mem_ind`.`ntLiter10ab`) as `nntLiter10ab`, \n\
			`hh_ind`.`HH_Povp` as `Povp`, \n\
			`hh_ind`.`HH_Subp` as `Subp`, \n\
			`hh_ind`.`HH_Fshort` as `Fshort`, \n\
			SUM(`mem_ind`.`labfor`) as `nlabfor`, \n\
			SUM(`mem_ind`.`Unempl15ab`) as `nUnempl15ab`, \n\
			`hh_ind`.`Victcr` as `nVictcr` \n\
			FROM `hh` INNER JOIN `hh_ind` ON (`hh_ind`.`hhID` = `hh`.`hhID`) INNER JOIN `mem_ind` ON (LEFT(`hh_ind`.`hhID`, 15) = LEFT(`Mem_Ind`.`memID`, 15)) \n\
			"GROUP BY `hhID` ) \n\
			AS `HH_CoreInd_Temp` )\n\
			;");		
		
*/		//FILE *pFile = fopen("C:\\mysql_hhcore.sql", "w");
		//fprintf( pFile, ConstChar(sSQL) );
		//fclose(pFile);

		pEltDB->ExecuteSQL(sSQL);
		sSQL = "ALTER TABLE `hh_coreind_temp` MODIFY COLUMN `hhID` varchar(17) NOT NULL, ADD INDEX `hhindex`(`hhID`);";
		pEltDB->ExecuteSQL(sSQL);

		pEltDB->UpdateRecords((CString) "hh_coreind_temp", sToTable, Attr(IDField) );
		sSQL = "DROP TABLE IF EXISTS `hh_coreind_temp`";
		pEltDB->ExecuteSQL(sSQL);

		//pRS = new CStatSimRS( pEltDB, sSQL );
		//pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField)/*,
		//	pLabel, pProgBar*/);

		return;
	}

	//Health and nutrition
	if (pLabel!=NULL)
		pLabel->SetText(_MBCS("Updating Health and Nutrition..."));

	if (ind_ind[DEATH05-1] || ind_ind[DEATHPREG-1] ||ind_ind[MALN05-1]) {
	//with mem 04
	CrHHInd(HH, _MBCS("wMem04"), _MBCS("tothh"), _MBCS("hh_coreind"), _MBCS("TotPop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("hhwMem04"));
	sFrTable.Format(_T("%s_hhwMem04"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	//with mem 05
	CrHHInd(HH, _MBCS("wMem05"), _MBCS("tothh"), _MBCS("hh_coreind"), _MBCS("TotPop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("hhwMem05"));
	sFrTable.Format(_T("%s_hhwMem05"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	}
	if (ind_ind[MALN05-1]) {
	//with maln05
	CrHHInd(HH, _MBCS("wMaln05"), _MBCS("hhwMem05"), _MBCS("hh_coreind"), _MBCS("hhwMem05"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("HHwMaln05"));
	sFrTable.Format(_T("%s_HHwMaln05"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;
	
	CrMemInd(HPQ_RAW, _MBCS("Maln05"), _MBCS("Mem05"));
	sFrTable.Format(_T("%s_Maln05"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;
	
	//maln 0-4
	CrMemInd(HPQ_RAW, _MBCS("maln04"), _MBCS("mem04"));
	sFrTable.Format(_T("%s_maln04"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);

	if (currElt<=BRGY) {
		CrMemIndCat(HPQ_RAW, _MBCS("mnutind"), _MBCS("mem05"), TRUE, _MBCS("_Male"), _MBCS("_Female"), _MBCS("hpq_mem"), _MBCS("maln05"),
			_MBCS("CONCAT(`hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`)"), 
			_MBCS("`hpq_mem`.`age_yr`<=5") );
	}


	delete pRS; pRS = NULL;
	}

	if (pLabel!=NULL)
		pLabel->SetText(_MBCS("Updating Health and Nutrition..."));


	if (ind_ind[DEATH05-1]) {

	//with mem 05
	CrHHInd(HH, _MBCS("wMem05d"), _MBCS("tothh"), _MBCS("hh_coreind"), _MBCS("TotPop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("hhwMem05d"));
	sFrTable.Format(_T("%s_hhwMem05d"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	//with Death05
	CrHHInd(HH, _MBCS("wDeath05"), _MBCS("hhwMem05d"), _MBCS("hh_coreind"), _MBCS("hhwMem05d"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("HHwDeath05"));
	sFrTable.Format(_T("%s_HHwDeath05"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	CrMemInd(HPQ_RAW, _MBCS("Death05"), _MBCS("Mem05"), _MBCS("_Male"), _MBCS("_Female"), _MBCS("HH_Ind"), _MBCS("Demog"), TRUE, _MBCS("hhID"));
	sFrTable.Format(_T("%s_Death05"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;


	//with mem 04
	CrHHInd(HH, _MBCS("wMem04d"), _MBCS("tothh"), _MBCS("hh_coreind"), _MBCS("TotPop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("hhwMem04d"));
	sFrTable.Format(_T("%s_hhwMem04d"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;
	
	//with Death04
	CrHHInd(HH, _MBCS("wDeath04"), _MBCS("hhwMem04d"), _MBCS("hh_coreind"), _MBCS("hhwMem04d"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("HHwDeath04"));
	sFrTable.Format(_T("%s_HHwDeath04"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	CrMemInd(HPQ_RAW, _MBCS("Death04"), _MBCS("Mem04"), _MBCS("_Male"), _MBCS("_Female"), _MBCS("HH_Ind"), _MBCS("Demog"), TRUE, _MBCS("hhID"));
	sFrTable.Format(_T("%s_Death04"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;


	//rosterize
	if (m_qnrID==1120041100 || m_qnrID==1020040900) {
	sSQL.Format(_T("DROP TABLE IF EXISTS `hpq_death`;"), (CString) Attr(element) );
	pEltDB->ExecuteSQL( sSQL );
	sSQL = "CREATE TABLE IF NOT EXISTS `hpq_death` (\n\
		   `rtype` varchar(2) DEFAULT NULL, \n\
		   `urb` tinyint(1) unsigned zerofill DEFAULT NULL, \n\
		   `regn` tinyint(2) unsigned zerofill DEFAULT NULL, \n\
		   `prov` tinyint(2) unsigned zerofill DEFAULT NULL, \n\
		   `mun` tinyint(2) unsigned zerofill DEFAULT NULL, \n\
		   `brgy` smallint(3) unsigned zerofill DEFAULT NULL, \n\
		   `purok` tinyint(2) unsigned zerofill DEFAULT NULL, \n\
		   `hcn` smallint(6) unsigned zerofill DEFAULT NULL, \n\
		   `death_line` tinyint(2) unsigned zerofill DEFAULT NULL, \n\
		   `deadnm` varchar(50) DEFAULT NULL, \n\
		   `mdeadsx` tinyint(1) DEFAULT NULL, \n\
		   `mdeadage` smallint(3) DEFAULT NULL, \n\
		   `mdeady` tinyint(2) DEFAULT NULL, \n\
		   `mdeady_o` varchar(20) DEFAULT NULL, \n\
		   KEY `hpqIndex` (`regn`,`prov`,`mun`,`brgy`,`purok`,`hcn`) \n\
		   ) ENGINE=MyISAM DEFAULT CHARSET=latin1; ";
	
	pEltDB->ExecuteSQL(sSQL);
	sSQL = "INSERT INTO `hpq_death` \n\
		   SELECT 99 as `rtype`, `urb`, `regn`, `prov`, `mun`, `brgy`, `purok`, `hcn`, 1 as `death_line`, \n\
		   `nmdead_1` as `deadnm`, `mdeadsx_1` as `mdeadsx`, `mdeadage_1` as `mdeadage`, `mdeady_1` as `mdeady`, `mdeady_o_1` as `mdeady_o` \n\
		   from hpq_hh where mdeadage_1 is not null;";
	pEltDB->ExecuteSQL(sSQL);
	sSQL = "INSERT INTO `hpq_death` \n\
		   SELECT 99 as `rtype`, `urb`, `regn`, `prov`, `mun`, `brgy`, `purok`, `hcn`, 2 as `death_line`, \n\
		   `nmdead_2` as `deadnm`, `mdeadsx_2` as `mdeadsx`, `mdeadage_2` as `mdeadage`, `mdeady_2` as `mdeady`, `mdeady_o_2` as `mdeady_o` \n\
		   from hpq_hh where mdeadage_2 is not null;";
	pEltDB->ExecuteSQL(sSQL);
	sSQL = "INSERT INTO `hpq_death` \n\
		   SELECT 99 as `rtype`, `urb`, `regn`, `prov`, `mun`, `brgy`, `purok`, `hcn`, 3 as `death_line`, \n\
		   `nmdead_3` as `deadnm`, `mdeadsx_3` as `mdeadsx`, `mdeadage_3` as `mdeadage`, `mdeady_3` as `mdeady`, `mdeady_o_3` as `mdeady_o` \n\
		   from hpq_hh where mdeadage_3 is not null;";
	pEltDB->ExecuteSQL(sSQL);
	sSQL = "INSERT INTO `hpq_death` \n\
		   SELECT 99 as `rtype`, `urb`, `regn`, `prov`, `mun`, `brgy`, `purok`, `hcn`, 4 as `death_line`, \n\
		   `nmdead_4` as `deadnm`, `mdeadsx_4` as `mdeadsx`, `mdeadage_4` as `mdeadage`, `mdeady_4` as `mdeady`, `mdeady_o_4` as `mdeady_o` \n\
		   from hpq_hh where mdeadage_4 is not null;";
	pEltDB->ExecuteSQL(sSQL);
	sSQL = "INSERT INTO `hpq_death` \n\
		   SELECT 99 as `rtype`, `urb`, `regn`, `prov`, `mun`, `brgy`, `purok`, `hcn`, 5 as `death_line`, \n\
		   `nmdead_5` as `deadnm`, `mdeadsx_5` as `mdeadsx`, `mdeadage_5` as `mdeadage`, `mdeady_5` as `mdeady`, `mdeady_o_5` as `mdeady_o` \n\
		   from hpq_hh where mdeadage_5 is not null;";
	pEltDB->ExecuteSQL(sSQL);
	}
	
	//for qnr with roster of deaths
	if (currElt<=BRGY) {
		CrMemIndCat(HPQ_RAW, _MBCS("mdeady"), _MBCS("totmem"), TRUE, _MBCS("_Male"), _MBCS("_Female"), _MBCS("hpq_death"), _MBCS("Totpop"),
			_MBCS("CONCAT(`hpq_death`.`regn`, `hpq_death`.`prov`, `hpq_death`.`mun`, `hpq_death`.`brgy`, `hpq_death`.`purok`, `hpq_death`.`hcn`, `hpq_death`.`death_line`)"),
			NULL, _MBCS("hpq_death"), _MBCS("mdeadsx"), _MBCS("death_line"));
	}

	delete pRS; pRS = NULL;
	//////////////////////////////////////////////////////////////////////////////

	if (pLabel!=NULL)
		pLabel->SetText(_MBCS("Updating Health and Nutrition..."));

	if (ind_ind[DEATHPREG-1]) {

	//with mem 01 (combined with death)
	CrHHInd(HH, _MBCS("wMem01d"), _MBCS("tothh"), _MBCS("hh_coreind"), _MBCS("TotPop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("hhwMem01d"));
	sFrTable.Format(_T("%s_hhwMem01d"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;
	}

	//with mem 01
	CrHHInd(HH, _MBCS("wMem01"), _MBCS("tothh"), _MBCS("hh_coreind"), _MBCS("TotPop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("hhwMem01"));
	sFrTable.Format(_T("%s_hhwMem01"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);

	//with DeathPreg
	CrHHInd(HH, _MBCS("wDeathPreg"), _MBCS("hhwMem01d"), _MBCS("hh_coreind"), _MBCS("hhwMem01d"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("HHwDeathPreg"));
	sFrTable.Format(_T("%s_HHwDeathPreg"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	CrMemInd(HPQ_RAW, _MBCS("DeathPreg"), _MBCS("Mem01"), _MBCS("_Male"), _MBCS("_Female"), _MBCS("HH_Ind"), _MBCS("Demog"), TRUE, _MBCS("hhID"));
	sFrTable.Format(_T("%s_DeathPreg"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	}

	//House
	if (pLabel!=NULL)
		pLabel->SetText(_MBCS("Updating Housing..."));

	if (ind_ind[SQUAT-1]) {

	CrHHInd(HPQ_RAW, _MBCS("Squat"), _MBCS("TotHH"), _MBCS("HH_Ind"), _MBCS("TotPop"));
	sFrTable.Format(_T("%s_Squat"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	//number of members who are squatters
	CrMemInd(HH, _MBCS("nSquat"), _MBCS("TotMem"), _MBCS("_Male"), _MBCS("_Female"), _MBCS("HH_coreind"), _MBCS("Totpop"), 
		FALSE, _MBCS("hhID"), _MBCS("SUM"), 100,
		_MBCS("MemSquat"));
	sFrTable.Format(_T("%s_MemSquat"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);

	if (currElt<=BRGY) {
		CrHHIndCat(HPQ_RAW, _MBCS("tenur"), _MBCS("tothh"));
	}

	delete pRS; pRS = NULL;
	}

	if (pLabel!=NULL)
		pLabel->SetText(_MBCS("Updating Housing..."));

	if (ind_ind[MSH-1]) {

	CrHHInd(HPQ_RAW, _MBCS("MSH"), _MBCS("TotHH"), _MBCS("HH_Ind"), _MBCS("TotPop"));
	sFrTable.Format(_T("%s_MSH"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	//number of members who are in msh
	CrMemInd(HH, _MBCS("nMSH"), _MBCS("TotMem"), _MBCS("_Male"), _MBCS("_Female"), _MBCS("HH_coreind"), _MBCS("Totpop"), 
		FALSE, _MBCS("hhID"), _MBCS("SUM"), 100,
		_MBCS("MemMSH"));
	sFrTable.Format(_T("%s_MemMSH"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);

	if (currElt<=BRGY) {
		CrHHIndCat(HPQ_RAW, _MBCS("wall"), _MBCS("tothh"));
		CrHHIndCat(HPQ_RAW, _MBCS("roof"), _MBCS("tothh"));
	}
		
	delete pRS; pRS = NULL;
	}


	//Wat San
	if (pLabel!=NULL)
		pLabel->SetText(_MBCS("Updating Water and Sanitation..."));

	if (ind_ind[NTSWS-1]) {
	//number of members who are ntsws
	CrMemInd(HH, _MBCS("nNTSWS"), _MBCS("TotMem"), _MBCS("_Male"), _MBCS("_Female"), _MBCS("HH_coreind"), _MBCS("Totpop"), 
		FALSE, _MBCS("hhID"), _MBCS("SUM"), 100,
		_MBCS("MemNTSWS"));
	sFrTable.Format(_T("%s_MemntSWS"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	CrHHInd(HPQ_RAW, _MBCS("ntSWS"), _MBCS("TotHH"), _MBCS("HH_Ind"), _MBCS("TotPop"));
	sFrTable.Format(_T("%s_ntSWS"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField));

	if (currElt<=BRGY) {
		CrHHIndCat(HPQ_RAW, _MBCS("water"), _MBCS("tothh"));
	}

	delete pRS; pRS = NULL;
	}

	if (pLabel!=NULL)
		pLabel->SetText(_MBCS("Updating Water and Sanitation..."));

	if (ind_ind[NTSWS-1]) {

	//number of members who are ntstf
	CrMemInd(HH, _MBCS("nNTSTF"), _MBCS("TotMem"), _MBCS("_Male"), _MBCS("_Female"), _MBCS("HH_coreind"), _MBCS("Totpop"), 
		FALSE, _MBCS("hhID"), _MBCS("SUM"), 100,
		_MBCS("MemNTSTF"));
	sFrTable.Format(_T("%s_MemNTSTF"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	CrHHInd(HPQ_RAW, _MBCS("ntSTF"), _MBCS("TotHH"), _MBCS("HH_Ind"), _MBCS("TotPop"));
	sFrTable.Format(_T("%s_ntSTF"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);

	if (currElt<=BRGY) {
		CrHHIndCat(HPQ_RAW, _MBCS("toil"), _MBCS("tothh"));
	}

	delete pRS; pRS = NULL;
	}

	//Educ
	if (pLabel!=NULL)
		pLabel->SetText(_MBCS("Updating Education..."));

	if (ind_ind[NTELEM612-1]) {
	//with mem 612
	CrHHInd(HH, _MBCS("wMem612"), _MBCS("tothh"), _MBCS("hh_coreind"), _MBCS("TotPop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("hhwMem612"));
	sFrTable.Format(_T("%s_hhwMem612"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;
	
	//with NtElem612
	CrHHInd(HH, _MBCS("wNtElem612"), _MBCS("hhwMem612"), _MBCS("hh_coreind"), _MBCS("hhwMem612"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("HHwNtElem612"));
	sFrTable.Format(_T("%s_HHwNtElem612"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	CrMemInd(HPQ_RAW, _MBCS("ntElem612"), _MBCS("Mem612"));
	sFrTable.Format(_T("%s_ntElem612"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);

	delete pRS; pRS = NULL;

	if (pLabel!=NULL)
		pLabel->SetText(_MBCS("Updating Education..."));

	//with mem 611
	CrHHInd(HH, _MBCS("wMem611"), _MBCS("tothh"), _MBCS("hh_coreind"), _MBCS("TotPop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("hhwMem611"));
	sFrTable.Format(_T("%s_hhwMem611"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;
	
	//with NtElem611
	CrHHInd(HH, _MBCS("wNtElem611"), _MBCS("hhwMem611"), _MBCS("hh_coreind"), _MBCS("hhwMem611"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("HHwNtElem611"));
	sFrTable.Format(_T("%s_HHwNtElem611"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	CrMemInd(HPQ_RAW, _MBCS("ntElem611"), _MBCS("Mem611"));
	sFrTable.Format(_T("%s_ntElem611"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);

	delete pRS; pRS = NULL;

	}

	if (pLabel!=NULL)
		pLabel->SetText(_MBCS("Updating Education..."));

	if (ind_ind[NTHS1316-1]) {

	//with mem 13-16
	CrHHInd(HH, _MBCS("wMem1316"), _MBCS("tothh"), _MBCS("hh_coreind"), _MBCS("TotPop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("hhwMem1316"));
	sFrTable.Format(_T("%s_hhwMem1316"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;
	
	//with NtHS1316
	CrHHInd(HH, _MBCS("wNtHS1316"), _MBCS("hhwMem1316"), _MBCS("hh_coreind"), _MBCS("hhwMem1316"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("HHwNtHS1316"));
	sFrTable.Format(_T("%s_HHwNtHS1316"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	CrMemInd(HPQ_RAW, _MBCS("ntHS1316"), _MBCS("Mem1316"));
	sFrTable.Format(_T("%s_ntHS1316"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	if (pLabel!=NULL)
		pLabel->SetText(_MBCS("Updating Education..."));

	//with mem 12-15
	CrHHInd(HH, _MBCS("wMem1215"), _MBCS("tothh"), _MBCS("hh_coreind"), _MBCS("TotPop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("hhwMem1215"));
	sFrTable.Format(_T("%s_hhwMem1215"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;
	
	//with NtHS1215
	CrHHInd(HH, _MBCS("wNtHS1215"), _MBCS("hhwMem1215"), _MBCS("hh_coreind"), _MBCS("hhwMem1215"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("HHwNtHS1215"));
	sFrTable.Format(_T("%s_HHwNtHS1215"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	CrMemInd(HPQ_RAW, _MBCS("ntHS1215"), _MBCS("Mem1215"));
	sFrTable.Format(_T("%s_ntHS1215"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;


	}


	if (pLabel!=NULL)
		pLabel->SetText(_MBCS("Updating Education..."));

	if (ind_ind[NTHS1316-1] || ind_ind[NTELEM612-1]) {

	//with mem 6-15
	CrHHInd(HH, _MBCS("wMem615"), _MBCS("tothh"), _MBCS("hh_coreind"), _MBCS("TotPop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("hhwMem615"));
	sFrTable.Format(_T("%s_hhwMem615"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;
	
	//with NtSCH615
	CrHHInd(HH, _MBCS("wNtSCH615"), _MBCS("hhwMem615"), _MBCS("hh_coreind"), _MBCS("hhwMem615"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("HHwNtSCH615"));
	sFrTable.Format(_T("%s_HHwNtSCH615"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	CrMemInd(HPQ_RAW, _MBCS("ntSch615"), _MBCS("Mem615"));
	sFrTable.Format(_T("%s_ntSch615"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	if (pLabel!=NULL)
		pLabel->SetText(_MBCS("Updating Education..."));

	//with mem 6-16
	CrHHInd(HH, _MBCS("wMem616"), _MBCS("tothh"), _MBCS("hh_coreind"), _MBCS("TotPop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("hhwMem616"));
	sFrTable.Format(_T("%s_hhwMem616"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;
	
	//with NtSCH616
	CrHHInd(HH, _MBCS("wNtSCH616"), _MBCS("hhwMem616"), _MBCS("hh_coreind"), _MBCS("hhwMem616"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("HHwNtSCH616"));
	sFrTable.Format(_T("%s_HHwNtSCH616"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	CrMemInd(HPQ_RAW, _MBCS("ntSch616"), _MBCS("Mem616"));
	sFrTable.Format(_T("%s_ntSch616"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	if (pLabel!=NULL)
		pLabel->SetText(_MBCS("Updating Education..."));

	//with mem 05
	CrHHInd(HH, _MBCS("wMem10ab"), _MBCS("tothh"), _MBCS("hh_coreind"), _MBCS("TotPop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("hhwMem10ab"));
	sFrTable.Format(_T("%s_hhwMem10ab"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;
	
	//with NtLITER10AB
	CrHHInd(HH, _MBCS("wNtLITER10AB"), _MBCS("hhwMem10ab"), _MBCS("hh_coreind"), _MBCS("hhwMem10ab"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("HHwNtLITER10AB"));
	sFrTable.Format(_T("%s_HHwNtLITER10AB"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	CrMemInd(HPQ_RAW, _MBCS("ntLiter10ab"), _MBCS("Mem10ab"));
	sFrTable.Format(_T("%s_ntLiter10ab"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;
	}
	/////////////////////////////////////////////////////////////////////////////
	//Income
	if (pLabel!=NULL)
		pLabel->SetText(_MBCS("Updating Income..."));

	if (ind_ind[POVP-1]) {

	//number of members who are povp
	CrMemInd(HH, _MBCS("nPovp"), _MBCS("TotMem"), _MBCS("_Male"), _MBCS("_Female"), _MBCS("HH_coreind"), _MBCS("Totpop"), 
		FALSE, _MBCS("hhID"), _MBCS("SUM"), 100,
		_MBCS("MemPovp"));
	sFrTable.Format(_T("%s_MemPOVP"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	CrHHInd(HPQ_RAW, _MBCS("Povp"), _MBCS("TotHH"), _MBCS("HH_Ind"), _MBCS("TotPop"));
	sFrTable.Format(_T("%s_Povp"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;
	}

	if (pLabel!=NULL)
		pLabel->SetText(_MBCS("Updating Income..."));

	if (ind_ind[SUBP-1]) {

	//number of members who are subp
	CrMemInd(HH, _MBCS("nSubp"), _MBCS("TotMem"), _MBCS("_Male"), _MBCS("_Female"), _MBCS("HH_coreind"), _MBCS("Totpop"), 
		FALSE, _MBCS("hhID"), _MBCS("SUM"), 100,
		_MBCS("MemSubp"));
	sFrTable.Format(_T("%s_MemSUBP"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	CrHHInd(HPQ_RAW, _MBCS("Subp"), _MBCS("TotHH"), _MBCS("HH_Ind"), _MBCS("TotPop"));
	sFrTable.Format(_T("%s_Subp"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	}

	if (pLabel!=NULL)
		pLabel->SetText(_MBCS("Updating Income..."));

	if (ind_ind[FSHORT-1]) {
	//number of members who are fshort
	CrMemInd(HH, _MBCS("nFShort"), _MBCS("TotMem"), _MBCS("_Male"), _MBCS("_Female"), _MBCS("HH_coreind"), _MBCS("Totpop"), 
		FALSE, _MBCS("hhID"), _MBCS("SUM"), 100,
		_MBCS("MemFShort"));
	sFrTable.Format(_T("%s_MemFSHORT"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);

	CrHHInd(HPQ_RAW, _MBCS("Fshort"), _MBCS("TotHH"), _MBCS("HH_Ind"), _MBCS("TotPop"));
	sFrTable.Format(_T("%s_Fshort"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;
		
	}

	CString sElt(Attr(element));

/*	if (pLabel!=NULL)
		pLabel->SetText(_MBCS("Updating Income groups..."));

	//framework
	//create the table of totin 'ile
	sSQL.Format(_T("DROP TABLE IF EXISTS `hh_totinile_%s`;"), (CString) Attr(element) );
	pEltDB->ExecuteSQL( sSQL );
	sSQL.Format( _T("CREATE TABLE IF NOT EXISTS `hh_totinile_%s` ( \n\
		`hhID` VARCHAR(%d), \n\
		`regn` TINYINT(%d) UNSIGNED ZEROFILL NOT NULL, \n\
		`prov` TINYINT(%d) UNSIGNED ZEROFILL NOT NULL, \n\
		`mun` TINYINT(%d) UNSIGNED ZEROFILL NOT NULL, \n\
		`brgy` SMALLINT(%d) UNSIGNED ZEROFILL NOT NULL, \n\
		`purok` TINYINT(%d) UNSIGNED ZEROFILL NOT NULL, \n\
		`hcn` INT(%d) UNSIGNED ZEROFILL NOT NULL, \n\
		`hsize` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
		`hh_totin` FLOAT(14,2) NOT NULL, \n\
		`rnk_totin` INTEGER NOT NULL, \n\
		`totindecile` INTEGER NOT NULL, \n\
		`totinquintile` INTEGER NOT NULL, \n\
		PRIMARY KEY(`hhID`), \n\
		INDEX `IDIndex`(`hhID`, `regn`, `prov`, `mun`, `brgy`, `purok`), \n\
		INDEX `IncIndex`(`totindecile`, `totinquintile`, `rnk_totin`) \n\
		) \n\
		ENGINE = MYISAM; \n"),
		sElt,
		hh_dig,
		regn_dig-country_dig,
		prov_dig-regn_dig,
		mun_dig-prov_dig,
		brgy_dig-mun_dig,
		purok_dig-brgy_dig,
		hh_dig-purok_dig);

	pEltDB->ExecuteSQL( sSQL );

	//create the table of pci 'ile
	sSQL.Format(_T("DROP TABLE IF EXISTS `hh_pciile_%s`;"), (CString) Attr(element) );
	pEltDB->ExecuteSQL( sSQL );
	sSQL.Format( _T("CREATE TABLE IF NOT EXISTS `hh_pciile_%s` ( \n\
		`hhID` VARCHAR(%d), \n\
		`regn` TINYINT(%d) UNSIGNED ZEROFILL NOT NULL, \n\
		`prov` TINYINT(%d) UNSIGNED ZEROFILL NOT NULL, \n\
		`mun` TINYINT(%d) UNSIGNED ZEROFILL NOT NULL, \n\
		`brgy` SMALLINT(%d) UNSIGNED ZEROFILL NOT NULL, \n\
		`purok` TINYINT(%d) UNSIGNED ZEROFILL NOT NULL, \n\
		`hcn` INT(%d) UNSIGNED ZEROFILL NOT NULL, \n\
		`hsize` TINYINT(2) UNSIGNED ZEROFILL NOT NULL, \n\
		`hh_pci` FLOAT(14,2) NOT NULL, \n\
		`rnk_pci` INTEGER NOT NULL, \n\
		`pcidecile` INTEGER NOT NULL, \n\
		`pciquintile` INTEGER NOT NULL, \n\
		PRIMARY KEY(`hhID`), \n\
		INDEX `IDIndex`(`hhID`, `regn`, `prov`, `mun`, `brgy`, `purok`), \n\
		INDEX `IncIndex`(`pcidecile`, `pciquintile`, `rnk_pci`) \n\
		) \n\
		ENGINE = MYISAM; \n"),
		sElt,
		hh_dig,
		regn_dig-country_dig,
		prov_dig-regn_dig,
		mun_dig-prov_dig,
		brgy_dig-mun_dig,
		purok_dig-brgy_dig,
		hh_dig-purok_dig);

	pEltDB->ExecuteSQL( sSQL );

	//generate case values wrt totin
	if (currElt<=BRGY) {
		
		int nGeo, nRec, 
			i, j, k, d,
			decval, offdec, quival, offqui;

		CStatSimRS *pIncRS = NULL;

		//get count in each geo to limit the rank of inc
		sSQL.Format(_T("SELECT * FROM `%s_TotPop`;"), (CString) Attr(element));
		pRS = new CStatSimRS( pEltDB, sSQL);

		nGeo = pRS->GetRecordCount();
		pRS->MoveFirst();

		//this is for totin
		for (i=0; i<nGeo; i++) {

			CString sCrit = pRS->SQLFldValue( Attr(IDField) ),
				sCritFld = Attr(IDField),
				sDigit = Attr(digit),
				sSQLInc, sIncTable,
				sSubCrit;


			////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//totin

			sSQL = _T("SELECT `hh_ind`.`hhid`, `hh_ind`.`regn`, `hh_ind`.`prov`, `hh_ind`.`mun`, `hh_ind`.`brgy`, `hh_ind`.`purok`, `hh_ind`.`hcn`, `hsize`, `hh_totin`, 0 AS `rnk_totin`, 0 AS `totindecile`, 0 AS `totinquintile` from `hh_ind` WHERE LEFT(`hhID`, ")
				+ sDigit + _T(") = LEFT('") + sCrit + _T("', ") + sDigit + _T(") ORDER BY `hh_totin`;");
			pIncRS = new CStatSimRS( pEltDB, sSQL);

			nRec = pIncRS->GetRecordCount();
			offdec = nRec/10;
			offqui = nRec/5;
			decval = 1;
			quival = 1;
			
			//insert first the temporary records
			sIncTable.Format(_T("hh_totinile_%s"), (CString) Attr(element));
			pEltDB->InsertRecords( pIncRS, ConstChar(sIncTable) );

			pIncRS->MoveFirst();
			
			for (j=0; j<nRec; j++) {
				
				sSubCrit = pIncRS->SQLFldValue(_T("hhID"));

				//set the rank to the current index
				sSQLInc.Format(_T("UPDATE `hh_totinile_%s` SET `rnk_totin`=%d WHERE `hhID`='%s';"), (CString) Attr(element), j+1, sSubCrit );
				//execute
				pEltDB->ExecuteSQL( sSQLInc );
				
				//deciles
				if ( (j+1) <= (decval*offdec) ) {
					//set the value of decile
					sSQLInc.Format(_T("UPDATE `hh_totinile_%s` SET `totindecile`=%d WHERE `hhID`='%s';"), (CString) Attr(element), decval, sSubCrit );
				}
				else {
					//increment if the multiple of the decile code is needed
					decval = decval + 1;

					if (decval>=10) {
						decval = 10;
					}

					//set the value of decile
					sSQLInc.Format(_T("UPDATE `hh_totinile_%s` SET `totindecile`=%d WHERE `hhID`='%s';"), (CString) Attr(element), decval, sSubCrit );
				}
				
				//execute
				pEltDB->ExecuteSQL( sSQLInc );

				if ( (j+1) <= (quival*offqui) ) {
					//set the value of quintile
					sSQLInc.Format(_T("UPDATE `hh_totinile_%s` SET `totinquintile`=%d WHERE `hhID`='%s';"), (CString) Attr(element), quival, sSubCrit );
				}
				else {
					//increment if the multiple of the quintile code is needed
					quival = quival + 1;
					
					if (quival>=5) {
						quival = 5;
					}

					//set the value of quintile
					sSQLInc.Format(_T("UPDATE `hh_totinile_%s` SET `totinquintile`=%d WHERE `hhID`='%s';"), (CString) Attr(element), quival, sSubCrit );
				}
				
				//execute
				pEltDB->ExecuteSQL( sSQLInc );

				pIncRS->MoveNext();

			}
			
			
			delete pIncRS; pIncRS = NULL;

			////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//pci

			sSQL = _T("SELECT `hh_ind`.`hhid`, `hh_ind`.`regn`, `hh_ind`.`prov`, `hh_ind`.`mun`, `hh_ind`.`brgy`, `hh_ind`.`purok`, `hh_ind`.`hcn`, `hsize`, `hh_pci`, 0 AS `rnk_pci`, 0 AS `pcidecile`, 0 AS `pciquintile` from `hh_ind` WHERE LEFT(`hhID`, ")
				+ sDigit + _T(") = LEFT('") + sCrit + _T("', ") + sDigit + _T(") ORDER BY `hh_pci`;");

			pIncRS = new CStatSimRS( pEltDB, sSQL);
			nRec = pIncRS->GetRecordCount();
			offdec = nRec/10;
			offqui = nRec/5;
			decval = 1;
			quival = 1;
			
			//insert first the temporary records
			sIncTable.Format(_T("hh_pciile_%s"), (CString) Attr(element));
			pEltDB->InsertRecords( pIncRS, ConstChar(sIncTable) );

			pIncRS->MoveFirst();
			
			for (j=0; j<nRec; j++) {
				
				sSubCrit = pIncRS->SQLFldValue(_T("hhID"));

				//set the rank to the current index
				sSQLInc.Format(_T("UPDATE `hh_pciile_%s` SET `rnk_pci`=%d WHERE `hhID`='%s';"), (CString) Attr(element), j+1, sSubCrit );
				//execute
				pEltDB->ExecuteSQL( sSQLInc );
				
				//deciles
				if ( (j+1) <= (decval*offdec) ) {
					//set the value of decile
					sSQLInc.Format(_T("UPDATE `hh_pciile_%s` SET `pcidecile`=%d WHERE `hhID`='%s';"), (CString) Attr(element), decval, sSubCrit );
				}
				else {
					//increment if the multiple of the decile code is needed
					decval = decval + 1;
					
					if (decval>=10) {
						decval = 10;
					}

					//set the value of decile
					sSQLInc.Format(_T("UPDATE `hh_pciile_%s` SET `pcidecile`=%d WHERE `hhID`='%s';"), (CString) Attr(element), decval, sSubCrit );
				}
				
				//execute
				pEltDB->ExecuteSQL( sSQLInc );

				if ( (j+1) <= (quival*offqui) ) {
					//set the value of quintile
					sSQLInc.Format(_T("UPDATE `hh_pciile_%s` SET `pciquintile`=%d WHERE `hhID`='%s';"), (CString) Attr(element), quival, sSubCrit );
				}
				else {
					//increment if the multiple of the quintile code is needed
					quival = quival + 1;
					
					if (quival>=5) {
						quival = 5;
					}

					//set the value of quintile
					sSQLInc.Format(_T("UPDATE `hh_pciile_%s` SET `pciquintile`=%d WHERE `hhID`='%s';"), (CString) Attr(element), quival, sSubCrit );
				}
				
				//execute
				pEltDB->ExecuteSQL( sSQLInc );

				pIncRS->MoveNext();

			}
			
			
			delete pIncRS; pIncRS = NULL;

			//move geo
			pRS->MoveNext();
		
		}

		//pci quintile
		//rename hh_ind first
		sSQL.Format(_T("RENAME TABLE `hh_ind` TO `hh_ind_temp`;"));
		pEltDB->ExecuteSQL(sSQL);
		sSQL.Format(_T("RENAME TABLE `hh_pciile_%s` TO `hh_ind`;"), (CString) Attr(element) );
		pEltDB->ExecuteSQL(sSQL);
		CrHHIndCat(HPQ_RAW, _T("pciquintile"), _T("tothh"), TRUE, _T("hh_ind"), _T("totpop"), 
			_T("hhID"));

		//go back
		sSQL.Format(_T("RENAME TABLE `hh_ind` TO `hh_pciile_%s`;"), (CString) Attr(element) );
		pEltDB->ExecuteSQL(sSQL);
		sSQL.Format(_T("RENAME TABLE `hh_ind_temp` TO `hh_ind`;"));
		pEltDB->ExecuteSQL(sSQL);
		
		//create the table of quintiles vs cores
		sSQL.Format(_T("DROP TABLE IF EXISTS `%s_hhcoreind!pciquintile`;"), (CString) Attr(element));
		pEltDB->ExecuteSQL( sSQL );
		CString sTable; sTable.Format(_T("hh_pciile_%s"), (CString) Attr(element));
		CrIncDistCore( _T("pciquintile"), ConstChar(sTable) );

	}
	
*/	//Employ
	if (pLabel!=NULL) {
		pLabel->SetText(_MBCS("Updating Employment..."));
	}

	if (ind_ind[UNEMPL15AB-1]) {

	//with labfor
	CrHHInd(HH, _MBCS("wLabfor"), _MBCS("tothh"), _MBCS("hh_coreind"), _MBCS("TotPop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("hhwLabfor"));
	sFrTable.Format(_T("%s_hhwLabfor"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;
	
	//with NtUNEMPL15AB
	CrHHInd(HH, _MBCS("wUnempl15ab"), _MBCS("hhwLabfor"), _MBCS("hh_coreind"), _MBCS("hhwLabfor"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("HHwUnempl15ab"));
	sFrTable.Format(_T("%s_HHwUnempl15ab"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	CrMemInd(HPQ_RAW, _MBCS("Unempl15ab"), _MBCS("LabFor"));
	sFrTable.Format(_T("%s_Unempl15ab"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	CrMemInd(HPQ_RAW, _MBCS("Empl15ab"), _MBCS("LabFor"));
	sFrTable.Format(_T("%s_Unempl15ab"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	if (m_qnrID==120110100 || m_qnrID==1020100100 || m_qnrID==1020130100 || m_qnrID==1020130101  || m_qnrID==1020135100 || m_qnrID==1020135200 || m_qnrID==1020135300 || m_qnrID==1020135400) {
	//underemployment
	CrMemInd(HPQ_RAW, _MBCS("underempl15ab_hrs"), _MBCS("empl15ab"), _MBCS("_Male"), _MBCS("_Female"),
			_MBCS("mem_ind"), _MBCS("empl15ab"));

	CrMemInd(HPQ_RAW, _MBCS("underempl15ab_wrk"), _MBCS("empl15ab"), _MBCS("_Male"), _MBCS("_Female"),
			_MBCS("mem_ind"), _MBCS("empl15ab"));

	CrMemInd(HPQ_RAW, _MBCS("underempl15ab"), _MBCS("empl15ab"), _MBCS("_Male"), _MBCS("_Female"),
			_MBCS("mem_ind"), _MBCS("empl15ab"));
	}
	if (currElt<=BRGY) {
		if (m_qnrID!=1020130100 && m_qnrID!=1020130101 && m_qnrID!=1020135100 && m_qnrID!=1020135200 && m_qnrID!=1020135300 && m_qnrID!=1020135400 && m_qnrID!=1020135500) {
		//occupation
		CrMemIndCat(HPQ_RAW, _MBCS("g_occ"), _MBCS("empl15ab"), TRUE, _MBCS("_Male"), _MBCS("_Female"), _MBCS("hpq_mem"), _MBCS("empl15ab"),
			_MBCS("CONCAT(`hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`)"),
			_MBCS("`hpq_mem`.`jobind`=1 AND `hpq_mem`.`age_yr`>=15"));
		
		//sector

		CrMemIndCat(HPQ_RAW, _MBCS("sector"), _MBCS("empl15ab"), TRUE, _MBCS("_Male"), _MBCS("_Female"), _MBCS("hpq_mem"), _MBCS("empl15ab"),
			_MBCS("CONCAT(`hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`)"),
			_MBCS("`hpq_mem`.`jobind`=1 AND `hpq_mem`.`age_yr`>=15"));
		}
		
		//without job
		CrMemInd(HPQ_RAW, _MBCS("wnojob"), _MBCS("totmem"), _MBCS("_Male"), _MBCS("_Female"),
			_MBCS("mem_ind"), _MBCS("totpop"));

	if (m_qnrID==1020070400 || m_qnrID==120070300|| m_qnrID==120110100 || m_qnrID==1020100100) {
		//reason of unemployment - GRB
		CrMemIndCat(HPQ_RAW, _MBCS("ynotlookjob"), _MBCS("wnojob"), TRUE, _MBCS("_Male"), _MBCS("_Female"), _MBCS("hpq_mem"), _MBCS("wnojob"),
			_MBCS("CONCAT(`hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`)"),
			_MBCS("`hpq_mem`.`jobind`=2"));

		//class of worker - GRB
		CrMemIndCat(HPQ_RAW, _MBCS("workcl"), _MBCS("empl15ab"), TRUE, _MBCS("_Male"), _MBCS("_Female"), _MBCS("hpq_mem"), _MBCS("empl15ab"),
			_MBCS("CONCAT(`hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`)"),
			_MBCS("`hpq_mem`.`jobind`=1 AND `hpq_mem`.`age_yr`>=15"));
	}
	else if (m_qnrID==1120041100 || m_qnrID==1020040900) {

		//reason of unemployment - GENERAL
		CrMemIndCat(HPQ_RAW, _MBCS("ynojob"), _MBCS("wnojob"), TRUE, _MBCS("_Male"), _MBCS("_Female"), _MBCS("hpq_mem"), _MBCS("wnojob"),
			_MBCS("CONCAT(`hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`)"),
			_MBCS("`hpq_mem`.`jobind`=2"));
	}
		//nature of employment
		CrMemIndCat(HPQ_RAW, _MBCS("jstatus"), _MBCS("empl15ab"), TRUE, _MBCS("_Male"), _MBCS("_Female"), _MBCS("hpq_mem"), _MBCS("empl15ab"),
			_MBCS("CONCAT(`hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`)"),
			_MBCS("`hpq_mem`.`jobind`=1 AND `hpq_mem`.`age_yr`>=15"));

	}


	if (m_qnrID==1020070400 || m_qnrID==120070300|| m_qnrID==120110100 || m_qnrID==1020100100) {
	//for rostered wages
	if (pLabel!=NULL) {
		pLabel->SetText(_MBCS("Salaries and wages..."));
	}

	//salaries
	sSQL = "DROP TABLE IF EXISTS `wage_ind`;";
	pEltDB->ExecuteSQL(sSQL);

	sSQL = "CREATE TABLE `wage_ind` \n\
		SELECT hpq_mem.urb, hpq_mem.regn, hpq_mem.prov, hpq_mem.mun, hpq_mem.brgy, hpq_mem.purok, hpq_mem.hcn, hpq_mem.memno, CONCAT(`hpq_mem`.`regn`, `hpq_mem`.`prov`, `hpq_mem`.`mun`, `hpq_mem`.`brgy`, `hpq_mem`.`purok`, `hpq_mem`.`hcn`, `hpq_mem`.`memno`) as `memID`, hpq_mem.sex, hpq_wage.wagcshm, hpq_wage.wagkndm, \n\
		IF((`wagcshm`+`wagkndm`)>0,1,0) AS wage, `wagcshm`+`wagkndm` AS totwage, if(hpq_mem.sex=1 And (`wagcshm`+`wagkndm`)>0,1,0) AS wage_Male, IF(hpq_mem.sex=1, `wagcshm`+`wagkndm`,0) AS totwage_Male, if(hpq_mem.sex=2 And (`wagcshm`+`wagkndm`)>0,1,0) AS wage_Female, IF(hpq_mem.sex=2, `wagcshm`+`wagkndm`,0) AS totwage_Female \n\
		FROM hpq_mem INNER JOIN hpq_wage ON (hpq_mem.regn = hpq_wage.regn) AND (hpq_mem.prov = hpq_wage.prov) AND (hpq_mem.mun = hpq_wage.mun) AND (hpq_mem.brgy = hpq_wage.brgy) AND (hpq_mem.purok = hpq_wage.purok) AND (hpq_mem.hcn = hpq_wage.hcn) AND (hpq_mem.memno = hpq_wage.wage_line);";


	pEltDB->ExecuteSQL(sSQL);
	
	//	count
	CrMemInd(HPQ_RAW, _MBCS("wage"), _MBCS("empl15ab"), _MBCS("_Male"), _MBCS("_Female"),
		_MBCS("wage_ind"), _MBCS("empl15ab"), FALSE, _MBCS("memID"),
		_MBCS("SUM"));

	//average
	CrMemInd(HPQ_RAW, _MBCS("totwage"), _MBCS("wage"), _MBCS("_Male"), _MBCS("_Female"),
		_MBCS("wage_ind"), _MBCS("wage"), FALSE, _MBCS("memID"), 
		_MBCS("SUM"), 1);


	delete pRS; pRS = NULL;
	}
	///////////////////////////////////////////////////////////////////////////////////
	}

	if (ind_ind[VICTCR-1]) {
	//Victims of crime
	if (pLabel!=NULL)
		pLabel->SetText(_MBCS("Updating Peace and Order..."));

	//with VictCr
	CrHHInd(HH, _MBCS("wVictCr"), _MBCS("tothh"), _MBCS("hh_coreind"), _MBCS("totpop"), _MBCS("hhID"), 
		_MBCS("SUM"), 100,
		FALSE, _MBCS("HHwVictCr"));
	sFrTable.Format(_T("%s_HHwVictCr"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	delete pRS; pRS = NULL;

	CrMemInd(HPQ_RAW, _MBCS("VictCr"), _MBCS("TotMem"), _MBCS("_Male"), _MBCS("_Female"), _MBCS("HH_Ind"), _MBCS("Totpop"), 
		FALSE, _MBCS("hhID"),"SUM", 100, NULL, NULL, "victmurd" );
	sFrTable.Format(_T("%s_VictCr"), (CString) Attr(element));
	pRS = new CStatSimRS( pEltDB, ConstChar(sFrTable) );
	pEltDB->UpdateRecords(pRS, sToTable, Attr(IDField),
		pLabel, pProgBar);
	

	//for rostered crime
	if (m_qnrID==1020070400 || m_qnrID==120070300|| m_qnrID==120110100 || m_qnrID==1020100100) {
	if (currElt<=BRGY) {
		//types of crime

		LPCSTR pSexVarExt[3];
		pSexVarExt[TOT] = _MBCS("ctvicttot");
		pSexVarExt[MALE] = _MBCS("ctvictmale");
		pSexVarExt[FEMALE] = _MBCS("ctvictfemale");

		CrMemIndCat(HPQ_RAW, _MBCS("crimetype"), _MBCS("victcr"), TRUE, _MBCS("_Male"), _MBCS("_Female"), _MBCS("hpq_crime"), _MBCS("victcr"),
			_MBCS("IFNULL( CONCAT(`hpq_crime`.`regn`, `hpq_crime`.`prov`, `hpq_crime`.`mun`, `hpq_crime`.`brgy`, `hpq_crime`.`purok`, `hpq_crime`.`hcn`, `hpq_crime`.`crime_line`), CONCAT(`hpq_crime`.`regn`, `hpq_crime`.`prov`, `hpq_crime`.`mun`, `hpq_crime`.`brgy`, `hpq_crime`.`purok`, `hpq_crime`.`hcn`) )"),
			_MBCS("`hpq_crime`.`crimeind`=1"), _MBCS("hpq_crime"), _MBCS("sex"), _MBCS("crime_line"), pSexVarExt,
			_MBCS("SUM"));
	}
	}
	///////////////////////////////////////////////////////////
	delete pRS; pRS = NULL;
		
	}

	//clear ind_ind
	delete [] ind_ind;
}

int CStatSimElt::nGKC()
{
	int nCodes;

	switch(currElt) {

	case MEM:
				
		nCodes = 0;

		break;

	case HH:
		
		nCodes = 0;

		break;

	case PUROK:

		nCodes = 5;

		break;

	case BRGY:
		
		nCodes = 4;

		break;

	case MUN:
		
		nCodes = 3;

		break;

	case PROV:
		
		nCodes = 2;

		break;

	case REGN:
		
		nCodes = 1;

		break;
		
	default:

		AfxMessageBox(_T("To be updated!"));
		break;
	
	}

	return nCodes;


}

int CStatSimElt::GetSrcID(LPCSTR sSrc)
{

	CString sSQL, sVal = _T("");
	LPCSTR sID = NULL;

	int retID;

	sSQL.Format(_T("SELECT * FROM `~hElement` WHERE `~hElement`.`element`='%s';"), (CString) sSrc);

	pRS = new CStatSimRS( pEltDB, sSQL );

	if (pRS->GetRecordCount()>0) {
		pRS->MoveFirst();
	}

	for (long i=0; i<pRS->GetRecordCount(); i++) {
		sVal = pRS->SQLFldValue(_MBCS("elementID"));
		pRS->MoveNext();
	}

	sID = ConstChar(sVal);

	if (sID==NULL) {
		retID = 0;
	}
	else {
		retID = atoi(sID);
	}

	return retID;

}