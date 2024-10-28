#include "stdafx.h"
#include "StatSimAPI.h"
#include "StatSimInd.h"
#include "StatSimElt.h"

#ifndef STATSIMRS
	#include "StatSimRS.h"
#endif
#ifndef STATSIMCONN
	#include "statsimconn.h"
#endif

//Array of core indicator field suffixes
//DLLEXPORT LPCSTR **sHHCoreInd = (LPCSTR**) malloc( N_COREIND*sizeof(LPCSTR*) );
//DLLEXPORT LPCSTR ***sMEMCoreInd = (LPCSTR***) malloc( N_COREIND*sizeof(LPCSTR**) );
DLLEXPORT LPCSTR **sHHCoreInd = new LPCSTR*[N_COREIND];
DLLEXPORT LPCSTR ***sMEMCoreInd = new LPCSTR**[N_COREIND];
void InitIndTxt()
{
	int i, j;

	for (i=0; i<N_COREIND; i++) {

		//sHHCoreInd[i] = (LPCSTR*) malloc( 4*sizeof(LPCSTR) );
		sHHCoreInd[i] = new LPCSTR[4];
		//sMEMCoreInd[i] = (LPCSTR**) malloc( 4*sizeof(LPCSTR*) );
		sMEMCoreInd[i] = new LPCSTR*[4];
		
		for (j=0; j<2; j++) {
			
			//sMEMCoreInd[i][j] = (LPCSTR*) malloc( NSEXVARS*sizeof(LPCSTR) );
			sMEMCoreInd[i][j] = new LPCSTR[NSEXVARS];

		}

	}	

	//Array of core indicator field suffixes
	sHHCoreInd[DEATH05-1][VAR] = _MBCS("wDeath05");
	sHHCoreInd[DEATHPREG-1][VAR] = _MBCS("wDeathPreg");
	sHHCoreInd[MALN05-1][VAR] = _MBCS("wMaln05");
	sHHCoreInd[MSH-1][VAR] = _MBCS("MSH");
	sHHCoreInd[SQUAT-1][VAR] = _MBCS("Squat");
	sHHCoreInd[NTSWS-1][VAR] = _MBCS("ntSWS");
	sHHCoreInd[NTSTF-1][VAR] = _MBCS("ntSTF");
	sHHCoreInd[NTELEM612-1][VAR] = _MBCS("wntElem612");
	sHHCoreInd[NTHS1316-1][VAR] = _MBCS("wntHS1316");
	sHHCoreInd[POVP-1][VAR] = _MBCS("Povp");
	sHHCoreInd[SUBP-1][VAR] = _MBCS("Subp");
	sHHCoreInd[FSHORT-1][VAR] = _MBCS("FShort");
	sHHCoreInd[UNEMPL15AB-1][VAR] = _MBCS("wUnempl15ab");
	sHHCoreInd[VICTCR-1][VAR] = _MBCS("wVictCr");
	
	//labels
	sHHCoreInd[DEATH05-1][LABEL] = _MBCS("With child death");
	sHHCoreInd[DEATHPREG-1][LABEL] = _MBCS("With death due to pregnancy related causes");
	sHHCoreInd[MALN05-1][LABEL] = _MBCS("With malnourished children");
	sHHCoreInd[MSH-1][LABEL] = _MBCS("Living in makeshift house");
	sHHCoreInd[SQUAT-1][LABEL] = _MBCS("Informal settler");
	sHHCoreInd[NTSWS-1][LABEL] = _MBCS("Without access to safe water");
	sHHCoreInd[NTSTF-1][LABEL] = _MBCS("Without access to sanitary toilet facility");
	sHHCoreInd[NTELEM612-1][LABEL] = _MBCS("With members 6-12 not attending elementary");
	sHHCoreInd[NTHS1316-1][LABEL] = _MBCS("With members 13-16 not attending high school");
	sHHCoreInd[POVP-1][LABEL] = _MBCS("Income poor");
	sHHCoreInd[SUBP-1][LABEL] = _MBCS("Subsistence poor");
	sHHCoreInd[FSHORT-1][LABEL] = _MBCS("Experienced food shortage");
	sHHCoreInd[UNEMPL15AB-1][LABEL] = _MBCS("With unemployed members of the labor force");
	sHHCoreInd[VICTCR-1][LABEL] = _MBCS("With victims of crime");

	//numerators
	sHHCoreInd[DEATH05-1][NUM] = _MBCS("nDeath05");
	sHHCoreInd[DEATHPREG-1][NUM] = _MBCS("nDeathPreg");
	sHHCoreInd[MALN05-1][NUM] = _MBCS("nMaln05");
	sHHCoreInd[MSH-1][NUM] = _MBCS("MSH");
	sHHCoreInd[SQUAT-1][NUM] = _MBCS("Squat");
	sHHCoreInd[NTSWS-1][NUM] = _MBCS("ntSWS");
	sHHCoreInd[NTSTF-1][NUM] = _MBCS("ntSTF");
	sHHCoreInd[NTELEM612-1][NUM] = _MBCS("nntElem612");
	sHHCoreInd[NTHS1316-1][NUM] = _MBCS("nntHS1316");
	sHHCoreInd[POVP-1][NUM] = _MBCS("Povp");
	sHHCoreInd[SUBP-1][NUM] = _MBCS("Subp");
	sHHCoreInd[FSHORT-1][NUM] = _MBCS("FShort");
	sHHCoreInd[UNEMPL15AB-1][NUM] = _MBCS("nUnempl15ab");
	sHHCoreInd[VICTCR-1][NUM] = _MBCS("nVictCr");	
	
	//denominators
	sHHCoreInd[DEATH05-1][DENOM] = _MBCS("nmem05");
	sHHCoreInd[DEATHPREG-1][DENOM] = _MBCS("nmem01");
	sHHCoreInd[MALN05-1][DENOM] = _MBCS("nmem05");
	sHHCoreInd[MSH-1][DENOM] = _MBCS("tothh");
	sHHCoreInd[SQUAT-1][DENOM] = _MBCS("tothh");
	sHHCoreInd[NTSWS-1][DENOM] = _MBCS("tothh");
	sHHCoreInd[NTSTF-1][DENOM] = _MBCS("tothh");
	sHHCoreInd[NTELEM612-1][DENOM] = _MBCS("nmem612");
	sHHCoreInd[NTHS1316-1][DENOM] = _MBCS("nmem1316");
	sHHCoreInd[POVP-1][DENOM] = _MBCS("tothh");
	sHHCoreInd[SUBP-1][DENOM] = _MBCS("tothh");
	sHHCoreInd[FSHORT-1][DENOM] = _MBCS("tothh");
	sHHCoreInd[UNEMPL15AB-1][DENOM] = _MBCS("nlabfor");
	sHHCoreInd[VICTCR-1][DENOM] = _MBCS("totmem");
	
	//Array of core indicator field suffixes - mem
	sMEMCoreInd[DEATH05-1][VAR][TOT] = _MBCS("wDeath05");
	sMEMCoreInd[DEATHPREG-1][VAR][TOT] = _MBCS("wDeathPreg");
	sMEMCoreInd[MALN05-1][VAR][TOT] = _MBCS("wMaln05");
	sMEMCoreInd[MSH-1][VAR][TOT] = _MBCS("MSH");
	sMEMCoreInd[SQUAT-1][VAR][TOT] = _MBCS("Squat");
	sMEMCoreInd[NTSWS-1][VAR][TOT] = _MBCS("ntSWS");
	sMEMCoreInd[NTSTF-1][VAR][TOT] = _MBCS("ntSTF");
	sMEMCoreInd[NTELEM612-1][VAR][TOT] = _MBCS("wntElem612");
	sMEMCoreInd[NTHS1316-1][VAR][TOT] = _MBCS("wntHS1316");
	sMEMCoreInd[POVP-1][VAR][TOT] = _MBCS("Povp");
	sMEMCoreInd[SUBP-1][VAR][TOT] = _MBCS("Subp");
	sMEMCoreInd[FSHORT-1][VAR][TOT] = _MBCS("FShort");
	sMEMCoreInd[UNEMPL15AB-1][VAR][TOT] = _MBCS("wUnempl15ab");
	sMEMCoreInd[VICTCR-1][VAR][TOT] = _MBCS("wVictCr");
	
	//labels
	sMEMCoreInd[DEATH05-1][LABEL][TOT] = _MBCS("With child death");
	sMEMCoreInd[DEATHPREG-1][LABEL][TOT] = _MBCS("With death due to pregnancy related causes");
	sMEMCoreInd[MALN05-1][LABEL][TOT] = _MBCS("With malnourished children");
	sMEMCoreInd[MSH-1][LABEL][TOT] = _MBCS("Living in makeshift house");
	sMEMCoreInd[SQUAT-1][LABEL][TOT] = _MBCS("Informal settler");
	sMEMCoreInd[NTSWS-1][LABEL][TOT] = _MBCS("Without access to safe water");
	sMEMCoreInd[NTSTF-1][LABEL][TOT] = _MBCS("Without access to sanitary toilet facility");
	sMEMCoreInd[NTELEM612-1][LABEL][TOT] = _MBCS("With members not attending elementary");
	sMEMCoreInd[NTHS1316-1][LABEL][TOT] = _MBCS("With members not attending high school");
	sMEMCoreInd[POVP-1][LABEL][TOT] = _MBCS("Income poor");
	sMEMCoreInd[SUBP-1][LABEL][TOT] = _MBCS("Subsistence poor");
	sMEMCoreInd[FSHORT-1][LABEL][TOT] = _MBCS("Experienced food shortage");
	sMEMCoreInd[UNEMPL15AB-1][LABEL][TOT] = _MBCS("With unemployed member of the labor force");
	sMEMCoreInd[VICTCR-1][LABEL][TOT] = _MBCS("With Victim of crime");

	//Array of core indicator field suffixes - MEM
	for (i=0; i<N_COREIND; i++) {
		
		CString sVal;

		//male
		sVal.Format(_T("%s_Male"), (CString) sHHCoreInd[i][VAR]);
		sMEMCoreInd[i][VAR][MALE] = ConstChar(sVal);

		//female
		sVal.Format(_T("%s_Female"), (CString) sHHCoreInd[i][VAR]);
		sMEMCoreInd[i][VAR][FEMALE] = ConstChar(sVal);


		//male - label
		sVal.Format(_T("%s, Male"), (CString) sHHCoreInd[i][LABEL]);
		sMEMCoreInd[i][LABEL][MALE] = ConstChar(sVal);

		//female - label
		sVal.Format(_T("%s_, Female"), (CString) sHHCoreInd[i][LABEL]);
		sMEMCoreInd[i][LABEL][FEMALE] = ConstChar(sVal);	

	}	

}

void DestIndTxt()
{
	for (int i=0; i<N_COREIND; i++) {
		
		for (int j=0; j<2; j++) {
			
			if(sMEMCoreInd[i][j]) {
				delete [] sMEMCoreInd[i][j];
				sMEMCoreInd[i][j] = 0;
			}

		}

		if(sHHCoreInd[i]) {
			delete [] sHHCoreInd[i];
			sHHCoreInd[i] = 0;
		}
		if(sMEMCoreInd[i]) {
			delete [] sMEMCoreInd[i];
			sMEMCoreInd[i] = 0;
		}

	}	

}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStatSimInd::CStatSimInd(CStatSimConn * pDB, INDGRP indGrp, int cInd, int cE, BOOL initAttr)
{
	m_cIG = indGrp;
	m_pDB = pDB;
	m_cE = cE;
	m_cInd = cInd;

}

CStatSimInd::~CStatSimInd()
{

}
CString CStatSimInd::sIndTable()
{
	return GetSrcTable(m_cE);
}

CString CStatSimInd::sIndLabel()
{

	CString sSQL, sLabel;
	long nRS, i;
	CStatSimRS* pRS;

	sSQL.Format(_T("SELECT * FROM `~hElement` WHERE `elementID`=%d;"), 
		m_cE);
	
	pRS = new CStatSimRS( m_pDB, sSQL );
	nRS = pRS->GetRecordCount();
	if (nRS > 0) {
		pRS->MoveFirst();
		for ( i=0; i<nRS; i++ ) {
			sLabel = pRS->SQLFldValue(_MBCS("label"));
		}
		pRS->MoveNext();
	}

	else
		sLabel = _MBCS("");

	return sLabel;
}

CString CStatSimInd::GetDesc()
{

	CString sSQL, sVal;
	long nRS, i;
	CStatSimRS* pRS;

	sSQL.Format(_T("SELECT * FROM `~ind` WHERE `indID`=%d;"), 
		m_cInd);
	
	pRS = new CStatSimRS( m_pDB, sSQL );
	nRS = pRS->GetRecordCount();


	if (nRS > 0) {
		pRS->MoveFirst();
		for ( i=0; i<nRS; i++ ) {
			sVal = pRS->SQLFldValue(_MBCS("description"));

		}
		pRS->MoveNext();
	}

	else
		sVal = _MBCS("");

	return sVal;
}

CString CStatSimInd::GetLabel()
{

	CString sSQL, sVal;
	long nRS, i;
	CStatSimRS* pRS;

	sSQL.Format(_T("SELECT * FROM `~ind` WHERE `indID`=%d;"), 
		m_cInd);
	
	pRS = new CStatSimRS( m_pDB, sSQL );
	nRS = pRS->GetRecordCount();


	if (nRS > 0) {
		pRS->MoveFirst();
		for ( i=0; i<nRS; i++ ) {
			sVal = pRS->SQLFldValue(_MBCS("label"));

		}
		pRS->MoveNext();
	}

	else
		sVal = _MBCS("");

	return sVal;
}

CString CStatSimInd::sNumeratorLabel()
{

	CString sSQL, sVal;
	long nRS, i;
	CStatSimRS* pRS;

	sSQL.Format(_T("SELECT * FROM `~ind` WHERE `indID`=%d;"), 
		m_cInd);
	
	pRS = new CStatSimRS( m_pDB, sSQL );
	nRS = pRS->GetRecordCount();


	if (nRS > 0) {
		pRS->MoveFirst();
		for ( i=0; i<nRS; i++ ) {
			sVal = pRS->SQLFldValue(_MBCS("numeratorlabel"));

		}
		pRS->MoveNext();
	}

	else
		sVal = _MBCS("");

	return sVal;
}

CString CStatSimInd::sNumerator()
{

	CString sSQL, sVal;
	long nRS, i;
	CStatSimRS* pRS;

	sSQL.Format(_T("SELECT * FROM `~ind` WHERE `indID`=%d;"), 
		m_cInd);
	
	pRS = new CStatSimRS( m_pDB, sSQL );
	nRS = pRS->GetRecordCount();


	if (nRS > 0) {
		pRS->MoveFirst();
		for ( i=0; i<nRS; i++ ) {
			sVal = pRS->SQLFldValue(_MBCS("numerator"));

		}
		pRS->MoveNext();
	}

	else
		sVal = _MBCS("");

	return sVal;
}

CString CStatSimInd::sDenominator()
{

	CString sSQL, sVal;
	long nRS, i;
	CStatSimRS* pRS;

	sSQL.Format(_T("SELECT * FROM `~ind` WHERE `indID`=%d;"), 
		m_cInd);
	
	pRS = new CStatSimRS( m_pDB, sSQL );
	nRS = pRS->GetRecordCount();


	if (nRS > 0) {
		pRS->MoveFirst();
		for ( i=0; i<nRS; i++ ) {
			sVal = pRS->SQLFldValue(_MBCS("denominator"));

		}
		pRS->MoveNext();
	}

	else
		sVal = _MBCS("");

	return sVal;
}

CString CStatSimInd::sDenominatorLabel()
{

	CString sSQL, sVal;
	long nRS, i;
	CStatSimRS* pRS;

	sSQL.Format(_T("SELECT * FROM `~ind` WHERE `indID`=%d;"), 
		m_cInd);
	
	pRS = new CStatSimRS( m_pDB, sSQL );
	nRS = pRS->GetRecordCount();


	if (nRS > 0) {
		pRS->MoveFirst();
		for ( i=0; i<nRS; i++ ) {
			sVal = pRS->SQLFldValue(_MBCS("denominatorlabel"));

		}
		pRS->MoveNext();
	}

	else
		sVal = _MBCS("");

	return sVal;
}

CString CStatSimInd::sPropCompute()
{

	CString sSQL, sVal;
	long nRS, i;
	CStatSimRS* pRS;

	sSQL.Format(_T("SELECT * FROM `~ind` WHERE `indID`=%d;"), 
		m_cInd);
	
	pRS = new CStatSimRS( m_pDB, sSQL );
	nRS = pRS->GetRecordCount();


	if (nRS > 0) {
		pRS->MoveFirst();
		for ( i=0; i<nRS; i++ ) {
			sVal = pRS->SQLFldValue(_MBCS("propcompute"));

		}
		pRS->MoveNext();
	}

	else
		sVal = _MBCS("");

	return sVal;
}
BOOL CStatSimInd::WithCat()
{

	CString sSQL, sVal;
	BOOL cat = 0;
	long nRS, i;
	CStatSimRS* pRS;

	sSQL.Format(_T("SELECT * FROM `~ind` WHERE `indID`=%d;"), 
		m_cInd);
	
	pRS = new CStatSimRS( m_pDB, sSQL );
	nRS = pRS->GetRecordCount();


	if (nRS > 0) {
		pRS->MoveFirst();
		for ( i=0; i<nRS; i++ ) {
			sVal = pRS->SQLFldValue(_MBCS("withcat"));

			cat = _ttoi((sVal));
		}
		pRS->MoveNext();
	}

	else
		sVal = _MBCS("");

	return cat;
}
BOOL CStatSimInd::WithSex()
{

	CString sSQL, sVal;
	BOOL sx = 0;
	long nRS, i;
	CStatSimRS* pRS;

	sSQL.Format(_T("SELECT * FROM `~ind` WHERE `indID`=%d;"), 
		m_cInd);
	
	pRS = new CStatSimRS( m_pDB, sSQL );
	nRS = pRS->GetRecordCount();


	if (nRS > 0) {
		pRS->MoveFirst();
		for ( i=0; i<nRS; i++ ) {
			sVal = pRS->SQLFldValue(_MBCS("withsex"));
			sx = _ttoi((sVal));
		}
		pRS->MoveNext();
	}

	else
		sVal = _MBCS("");

	return sx;
}
CString CStatSimInd::GetSector()
{

	CString sSQL, sVal, sSector;
	int crit;
	long nRS, i;
	CStatSimRS* pRS;

	sSQL.Format(_T("SELECT * FROM `~ind` WHERE `indID`=%d;"), 
		m_cInd);
	
	pRS = new CStatSimRS( m_pDB, sSQL );
	nRS = pRS->GetRecordCount();


	if (nRS > 0) {
		pRS->MoveFirst();
		for ( i=0; i<nRS; i++ ) {
			sVal = pRS->SQLFldValue(_MBCS("parID"));

		}
		pRS->MoveNext();
	}

	else
		sVal = _MBCS("");

	crit = _ttoi((sVal));

	sSQL.Format(_T("SELECT * FROM `~ind` WHERE `indID`=%d;"), 
		crit);


	delete pRS; pRS = 0;
	pRS = new CStatSimRS( m_pDB, sSQL );
	nRS = pRS->GetRecordCount();
	
	if (nRS > 0) {
		pRS->MoveFirst();
		for ( i=0; i<nRS; i++ ) {
			sSector = pRS->SQLFldValue(_MBCS("label"));

		}
		pRS->MoveNext();
	}

	else
		sSector = _MBCS("");




	return sSector;
}


CString CStatSimInd::sIndDesc()
{

	CString sSQL, sDesc;
	long nRS, i;
	CStatSimRS* pRS;

	sSQL.Format(_T("SELECT * FROM `~hElement` WHERE `elementID`=%d;"), 
		m_cE);
	
	pRS = new CStatSimRS( m_pDB, sSQL );
	nRS = pRS->GetRecordCount();


	if (nRS > 0) {
		pRS->MoveFirst();
		for ( i=0; i<nRS; i++ ) {
			sDesc = pRS->SQLFldValue(_MBCS("description"));

		}
		pRS->MoveNext();
	}

	else
		sDesc = _MBCS("");

	return sDesc;
}

CString CStatSimInd::sTable()
{
	return _T("~Ind");
}
CString CStatSimInd::sILnkTable()
{
	return _T("~IndLnk");
}
CString CStatSimInd::sIVETable()
{
	return _T("~IndVE");
}
CString CStatSimInd::sISrcTable()
{
	return _T("~IndSrc");
}
CString CStatSimInd::sISrtTable()
{
	return _T("~IndSrt");
}

CString CStatSimInd::GetSrcTable(int id)
{
	CString sSQL, sSrcTable;
	long nRS, i;
	CStatSimRS* pRS;

	sSQL.Format(_T("SELECT * FROM `~hElement` WHERE `elementID`=%d;"), 
		id);
	
	pRS = new CStatSimRS( m_pDB, sSQL );
	nRS = pRS->GetRecordCount();
	if (nRS > 0) {
		pRS->MoveFirst();
		for ( i=0; i<nRS; i++ ) {
			sSrcTable = pRS->SQLFldValue(_MBCS("element"));
		}
		pRS->MoveNext();
	}

	else
		sSrcTable = _MBCS("");

	return sSrcTable;


}
CString CStatSimInd::GetEltID()
{
	CString sSQL, sEltID;
	long nRS, i;
	CStatSimRS* pRS;

	sSQL.Format(_T("SELECT * FROM `~hElement` WHERE `elementID`=%d;"), 
		m_cE);
	
	pRS = new CStatSimRS( m_pDB, sSQL );
	nRS = pRS->GetRecordCount();
	if (nRS > 0) {
		pRS->MoveFirst();
		for ( i=0; i<nRS; i++ ) {
			sEltID = pRS->SQLFldValue(_MBCS("elementID"));
		}
		pRS->MoveNext();
	}

	else
		sEltID = _MBCS("");

	return sEltID;


}
CString CStatSimInd::sSQLDestroy()
{

	CString sSQL;

	sSQL.Format(_T("DROP TABLE IF EXISTS `%s`;"), sIndTable());

	return sSQL;
	
}

CString CStatSimInd::sSQLExp()
{
	CString sSQL, sClause = _T("");
	long nRS, i;
	CStatSimRS* pRS;

	sSQL.Format(_T("SELECT * FROM `%s` WHERE `indID`=%d AND `elementID`=%d AND (`vetype`=1 OR `vetype`=2) ORDER BY `veID`;"), 
		sIVETable(), m_cInd, m_cE);

	pRS = new CStatSimRS( m_pDB, sSQL );
	nRS = pRS->GetRecordCount();

	if (nRS > 0) {
		pRS->MoveFirst();
		for ( i=0; i<nRS; i++ ) {
			if ( i==nRS-1 ) {
				
				sClause += pRS->SQLFldValue(_MBCS("sSQL")) + _T(" AS `") 
					+ pRS->SQLFldValue(_MBCS("ve")) + _T("` ");
			}
			
			else {

				sClause += pRS->SQLFldValue(_MBCS("sSQL")) + _T(" AS `")
					+ pRS->SQLFldValue(_MBCS("ve")) + _T("`, ");
			}
			pRS->MoveNext();
		}
	}

	else
		sClause = _T("");

	delete pRS; pRS = NULL;


	return sClause;

}

CString CStatSimInd::sSQLSrc()
{
	CString sSQL, sClause = _T("");
	long nRS, i;
	CStatSimRS* pRS;
	
	sSQL.Format(_T("SELECT * FROM `%s` WHERE `indID`=%d AND `elementID`=%d;"), 
		sISrcTable(), m_cInd, m_cE);

	pRS = new CStatSimRS( m_pDB, sSQL );
	nRS = pRS->GetRecordCount();

	if (nRS >= 1) {
		pRS->MoveFirst();
		for ( i=0; i<nRS; i++ ) {
			if (i==0)
				sClause = pRS->SQLFldValue(_MBCS("sSQL"));
			else 
				sClause += _T(", ") + pRS->SQLFldValue(_MBCS("sSQL"));
			
			pRS->MoveNext();
		}
	}
	else 
		sClause = _T("");

	
	delete pRS; pRS = NULL;
	return sClause;
}


CString CStatSimInd::sSQLLnk()
{
	CString sSQL, sClause = _T("");
	long nRS, i;
	CStatSimRS* pRS;
	
	sSQL.Format(_T("SELECT * FROM `%s` WHERE `indID`=%d AND `elementID`=%d;"), 
		sISrcTable(), m_cInd, m_cE);

	pRS = new CStatSimRS( m_pDB, sSQL );
	nRS = pRS->GetRecordCount();

	if (nRS == 1) {
		return _T("");
	}

	sSQL.Format(_T("SELECT * FROM `%s` WHERE `indID`=%d AND `elementID`=%d ORDER BY `LsrcID`;"), 
		sILnkTable(), m_cInd, m_cE);

	pRS = new CStatSimRS( m_pDB, sSQL );
	nRS = pRS->GetRecordCount();

	if (nRS > 0) {
		pRS->MoveFirst();
		for ( i=0; i<nRS; i++ ) {
			if ( i==0 ) {
				sClause = pRS->SQLFldValue(_MBCS("sSQL"));
			}
			else {
				sClause += _T(" AND ") + pRS->SQLFldValue(_MBCS("sSQL"));
			}

			pRS->MoveNext();
		}
	}
	else
		sClause = _T("");

	delete pRS; pRS = NULL;


	return sClause;

}

CString CStatSimInd::sSQLGroupBy()
{
	CString sSQL, sClause = _T("");
	long nRS, i;
	CStatSimRS* pRS;

	sSQL.Format(_T("SELECT * FROM `%s` WHERE `indID`=%d AND `elementID`=%d AND (`vetype`=4) ORDER BY `veID`;"), 
		sIVETable(), m_cInd, m_cE);

	pRS = new CStatSimRS( m_pDB, sSQL );
	nRS = pRS->GetRecordCount();

	if (nRS > 0) {
		pRS->MoveFirst();
		for ( i=0; i<nRS; i++ ) {
			if ( i==0 ) {
				sClause += pRS->SQLFldValue(_MBCS("sSQL"));

			}
			else {
				sClause += _T(", ") + pRS->SQLFldValue(_MBCS("sSQL"));
			}
			pRS->MoveNext();
		}
	}

	else
		sClause = _T("");

	delete pRS; pRS = NULL;


	return sClause;

}
CString CStatSimInd::sSQLPrKey()
{
	CString sSQL, sClause = _T("");
	long nRS, i;
	CStatSimRS* pRS;

	sSQL.Format(_T("SELECT * FROM `%s` WHERE `indID`=%d AND `elementID`=%d AND (`vetype`=5) ORDER BY `veID`;"), 
		sIVETable(), m_cInd, m_cE);

	pRS = new CStatSimRS( m_pDB, sSQL );
	nRS = pRS->GetRecordCount();

	if (nRS > 0) {
		pRS->MoveFirst();
		for ( i=0; i<nRS; i++ ) {
			if ( i==0 ) {
				sClause += pRS->SQLFldValue(_MBCS("sSQL"));

			}
			else {
				sClause += _T(", ") + pRS->SQLFldValue(_MBCS("sSQL"));
			}
			pRS->MoveNext();
		}
	}

	else
		sClause = _T("");

	delete pRS; pRS = NULL;


	return sClause;

}

CString CStatSimInd::sSQLIndex()
{
	CString sSQL, sClause = _T("");
	long nRS, i;
	CStatSimRS* pRS;

	sSQL.Format(_T("SELECT * FROM `%s` WHERE `indID`=%d AND `elementID`=%d AND (`vetype`=6) ORDER BY `veID`;"), 
		sIVETable(), m_cInd, m_cE);

	pRS = new CStatSimRS( m_pDB, sSQL );
	nRS = pRS->GetRecordCount();

	if (nRS > 0) {
		pRS->MoveFirst();
		for ( i=0; i<nRS; i++ ) {
			if ( i==0 ) {
				sClause += pRS->SQLFldValue(_MBCS("sSQL"));

			}
			else {
				sClause += _T(", ") + pRS->SQLFldValue(_MBCS("sSQL"));
			}
			pRS->MoveNext();
		}
	}

	else
		sClause = _T("");

	delete pRS; pRS = NULL;


	return sClause;

}

CString CStatSimInd::sSQLWhere()
{
	CString sSQL, sClause = _T("");
	long nRS, i;
	CStatSimRS* pRS;

	sSQL.Format(_T("SELECT * FROM `%s` WHERE `indID`=%d AND `elementID`=%d AND (`vetype`=3) ORDER BY `veID`;"), 
		sIVETable(), m_cInd, m_cE);

	pRS = new CStatSimRS( m_pDB, sSQL );
	nRS = pRS->GetRecordCount();

	if (nRS > 0) {
		pRS->MoveFirst();
		for ( i=0; i<nRS; i++ ) {
			if ( i==0 ) {
				sClause += pRS->SQLFldValue(_MBCS("sSQL"));
			}
			else {
				sClause += _T(" AND ") + pRS->SQLFldValue(_MBCS("sSQL"));
			}
			pRS->MoveNext();
		}
	}

	else
		sClause = _T("");

	delete pRS; pRS = NULL;


	return sClause;

}

CString CStatSimInd::BuildSQL(SQLTYPE SQLType)
{

	CString sSQL;

	if ( SQLType==SELECT ) {
		sSQL = _MBCS("SELECT ");
	}
	else {
		sSQL.Format(_T("CREATE TABLE IF NOT EXISTS `%s` \nSELECT "), sIndTable() );
	}

	sSQL.Format(_T("%s %s \nFROM %s \n"), sSQL, 
		sSQLExp(), sSQLSrc() );

	if ( !IsNull( sSQLLnk() ) ) {
		sSQL.Format(_T("%sWHERE %s \n"), sSQL, sSQLLnk() );
		if ( !IsNull( sSQLWhere() ) )
			sSQL.Format(_T("%sAND %s \n"), sSQL, sSQLWhere() );
	}

	else {
		if ( !IsNull( sSQLWhere() ) )
			sSQL.Format(_T("%sWHERE %s \n"), sSQL, sSQLWhere() );
	}

	if ( !IsNull( sSQLGroupBy() ) ) {
		sSQL.Format(_T("%sGROUP BY %s \n"), sSQL, sSQLGroupBy() );
	}

	sSQL += _T(";");

	return sSQL;


}


CString CStatSimInd::GetTable()
{

	return GetTable(m_cIG);

}

CString CStatSimInd::GetIDFld()
{

	return GetIDFld(m_cIG);

}

CString CStatSimInd::GetTable(INDGRP cIG)
{

	CString sTable;

	switch (cIG) {
	
	case FIG:

		sTable = _MBCS("~Ind");
		return sTable;
		break;

	case SECT:

		sTable = _MBCS("~Ind");
		return sTable;
		break;

	case CAT:

		sTable = _MBCS("~Ind");
		return sTable;
		break;


	}

	return _T("");

}

CString CStatSimInd::GetIDFld(INDGRP cIG)
{
	CString sField;

	switch (cIG) {
	
	case FIG:

		sField = _MBCS("indID");
		return sField;
		break;

	case SECT:

		sField = _MBCS("indID");
		return sField;
		break;

	case CAT:

		sField = _MBCS("indID");
		return sField;
		break;


	}

	return _T("");

}

DWORD CStatSimInd::DeleteItem(DWORD nID)
{
	return DeleteItem(m_cIG, nID);

}

DWORD CStatSimInd::DeleteItem(INDGRP cIG, DWORD nID)
{
	return DeleteItem(cIG, cIG, nID);

}
DWORD CStatSimInd::DeleteItem(INDGRP cIGTable, INDGRP cIGIDFld, DWORD nID)
{

	CString sSQL;
	DWORD iID;
	
	//delete from current
	sSQL.Format(_T("DELETE FROM `%s` WHERE `%s` = %d;"), 
		GetTable(cIGTable), GetIDFld(cIGIDFld), nID);

	iID = m_pDB->ExecuteSQL(sSQL);

	if ( iID <0 )
		return -1;

	return iID;

}

DWORD CStatSimInd::DeleteHItem(DWORD nID)
{
	CString msgText;
	DWORD iID, *childID, *grchildID;
	int nChild, nGrChild, i, j;
	
	switch (m_cIG) {
	
	case FIG:
		
		//delete from current
		iID = DeleteItem(nID);
		
		if ( iID <0 )
			return -1;
		
		return iID;
		break;

	case SECT:

		//delete from figures - child
		childID = GetChildID(nID);
		if (childID!=NULL) {
			nChild = m_nChild;			
			for (i=0; i<nChild; i++) {
				iID = DeleteItem(FIG, childID[i]);
				
				if ( iID <0 ) {
					msgText.Format(_T("Cannot delete item: %d."), iID);
					AfxMessageBox(msgText);
				}
			}
			free(childID);		
		}


		//delete from current
		iID = DeleteItem(nID);
		
		if ( iID <0 ) {
			msgText.Format(_T("Cannot delete item: %d."), iID);
			AfxMessageBox(msgText);
		}
		
		return iID;
		break;

	case CAT:

		childID = GetChildID(nID);
		if (childID!=NULL) {		
			nChild = m_nChild;			
			for (i=0; i<nChild; i++) {
				grchildID = GetChildID(SECT, childID[i]);
				if (grchildID!=NULL) {
					nGrChild = m_nChild;			
					for (j=0; j<nGrChild; j++) {
						iID = DeleteItem(FIG, grchildID[j]);
						
						if ( iID <0 ) {
							msgText.Format(_T("Cannot delete item: %d."), iID);
							AfxMessageBox(msgText);
						}
					}
					free(grchildID);
				}
				iID = DeleteItem(SECT, childID[i]);
				
				if ( iID <0 ) {
					msgText.Format(_T("Cannot delete item: %d."), iID);
					AfxMessageBox(msgText);
				}
			}
			free(childID);		
		}

		//delete from current
		iID = DeleteItem(nID);
		
		if ( iID <0 ) {
			msgText.Format(_T("Cannot delete item: %d."), iID);
			AfxMessageBox(msgText);
		}


		return iID;
		break;


	}

	return -1;

}

		
DWORD CStatSimInd::DeleteItem(CComboBox* pCB)
{
	
	int iSel;
	DWORD iID;

	iSel = pCB->GetCurSel();

	if (iSel!=CB_ERR)
		iID = pCB->GetItemData(iSel);

	CString sSQL;

	if ( DeleteHItem(iID)>=0 ) {
		pCB->DeleteString(iSel);
		return iID;
	}
	else {
		return -1;
	}
	


}

DWORD* CStatSimInd::GetChildID(DWORD nID)
{

	return GetChildID(m_cIG, nID);

}

DWORD* CStatSimInd::GetChildID(INDGRP cIG, DWORD nID)
{

	return GetChildID(cIG, cIG, nID);

}
DWORD* CStatSimInd::GetChildID(INDGRP cIG, INDGRP cIGIDFld, DWORD nID)
{
	CString sSQL;
	CStatSimRS* pRS;
	DWORD* iIDArray = NULL;

	sSQL.Format(_T("SELECT `%s` FROM `%s` WHERE `%s` = %d;"), 
		GetIDFld(cIG), GetTable(cIG), GetParIDFld(cIGIDFld), nID);

	pRS = new CStatSimRS( m_pDB, sSQL );

	m_nChild = pRS->GetRecordCount();

	if (m_nChild>0) {
		iIDArray = (DWORD*) malloc(m_nChild* sizeof(DWORD));
	}

	LPCSTR sID;

	if (m_nChild>0) {
		pRS->MoveFirst();
		for (long i=0; i<m_nChild; i++) {
			sID = ConstChar(pRS->SQLFldValue((USHORT) 0));
			iIDArray[i] = (DWORD) atol(sID);
			pRS->MoveNext();
		}
		
	}

	return iIDArray;

}

CString CStatSimInd::GetParIDFld(INDGRP cIG)
{
	CString sField;

	switch (cIG) {
	
	case FIG:

		sField = _MBCS("parID");
		break;

	case SECT:

		sField = _MBCS("parID");
		break;

	case CAT:

		sField = _MBCS("parID");
		break;

	default:
		sField = _MBCS("");


	}

	return sField;

}

void CStatSimInd::BuildInd(SQLTYPE SQLType)
{
	
	if ( m_cE <= IND_READY_END ) {	//all ready indicators
		return;
	}

	m_pDB->ExecuteSQL( sSQLDestroy() );
	
	m_pDB->ExecuteSQL( BuildSQL(SQLType) );
	AddPrimaryKey();
	AddIndex();
	
}
void CStatSimInd::AddPrimaryKey()
{
	CString sSQL;
	
	sSQL.Format(_T("ALTER TABLE `%s`\n ADD PRIMARY KEY (%s);"), sIndTable(), sSQLPrKey());

	m_pDB->ExecuteSQL(sSQL, FALSE);

}
void CStatSimInd::AddIndex()
{
	CString sSQL;
	
	sSQL.Format(_T("ALTER TABLE `%s`\n ADD INDEX %sIndex (%s);"), sIndTable(), sIndTable(), sSQLIndex());

	m_pDB->ExecuteSQL(sSQL, FALSE);

}
