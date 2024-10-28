#pragma once

#ifndef STATSIMELT
	#include "StatSimElt.h"
#endif

#ifndef STATSIMIND
	#include "StatSimInd.h"
#endif

#ifndef DAEXTAB
	#include "daextab.h"
#endif
//Define StatSimHTML
#ifndef STATSIMHTML
	#define STATSIMHTML
#endif
//////////////////

#include <map>

class CStatSimConn;

enum bound {LEFT=0, RIGHT, TOP, BOTTOM};

class TagCell {

public:
	TagCell(LPCSTR sValue,
		LPCSTR lborderstyle = _MBCS("none"), LPCSTR rborderstyle = _MBCS("none"), LPCSTR tborderstyle = _MBCS("none"), LPCSTR bborderstyle = _MBCS("none"),
		float lborderpt = 0, float rborderpt = 0, float tborderpt = 0, float bborderpt = 0,
		float tdwidth = 139, float stwidth = 104.0, float height = 14.25,
		LPCSTR font = _MBCS("Arial"), float fontsize = 10.0, LPCSTR fontcolor = _MBCS("black"), LPCSTR fontweight = _MBCS("n"),
		int colspan = 1, int rowspan = 1,
		LPCSTR valign = NULL, LPCSTR halign = NULL
		);
	~TagCell();

	void SetValue(LPCSTR sValue);
	void SetBorder(bound b, LPCSTR style, float pt);
	void SetFont(LPCSTR font, float fontsize, LPCSTR fontcolor, LPCSTR fontweight);
	void SetTDWidth(int nCols);
	void SetSTWidth(int nCols);
	void SetColSpan(int nCols);
	void SetRowSpan(int nRows);
	void SetVAlign(LPCSTR sAlign);
	void SetHAlign(LPCSTR sAlign);

	LPCSTR GetValue();
	LPCSTR GetBorderStyle(bound b);
	float GetBorderSize(bound b);
	float GetTDWidth();
	float GetSTWidth();
	float GetHeight();
	LPCSTR GetFont();
	float GetFontSize();
	LPCSTR GetFontColor();
	LPCSTR GetFontWeight();
	int GetColSpan();
	int GetRowSpan();
	LPCSTR GetVAlign();
	LPCSTR GetHAlign();

private:
	//Value
	LPCSTR m_sValue;

	//borders and height
	LPCSTR m_lborderstyle;
	LPCSTR m_rborderstyle;
	LPCSTR m_tborderstyle;
	LPCSTR m_bborderstyle;

	
	float m_lborderpt;
	float m_rborderpt;
	float m_tborderpt;
	float m_bborderpt;
	float m_tdwidth;
	float m_stwidth;
	float m_height;

	//font
	LPCSTR m_font;
	float m_fontsize;
	LPCSTR m_fontcolor;
	LPCSTR m_fontweight;

	//alignment
	LPCSTR m_valign;
	LPCSTR m_halign;

	//others
	int m_colspan;
	int m_rowspan;

};

class __declspec(dllexport) CStatSimHTML  
{
public:
	CStatSimHTML(CStatSimConn *pDB, LPCSTR sTable, FILE *pFile, 
		int cInd = -1, int cE = -1, LPCSTR sParentTable = NULL);	//grouped

	CStatSimHTML(CStatSimConn *pDB, LPCSTR sTable, CString sPath, 
		int cInd = -1, int cE = -1, LPCSTR sParentTable = NULL);	//grouped
	
	CStatSimHTML(CStatSimConn *pDB, FILE *pFile, 
		int cInd = -1, int cE = -1);						//generic inside processing

	CStatSimHTML(CStatSimRS *pRS, FILE *pFile,
		int cInd = -1, int cE = -1, CStatSimRS *pParentRS = NULL);	//generic or core
	
	CStatSimHTML(CStatSimRS *pRS, CString sPath,
		int cInd = -1, int cE = -1, CStatSimRS *pParentRS = NULL);	//generic or core
	
	CStatSimHTML(CString sPath, LPCSTR sMode);
	virtual ~CStatSimHTML();

	void Construct(CStatSimConn *pDB, LPCSTR sTable, FILE *pFile, 
		int cInd = -1, int cE = -1, LPCSTR sParentTable = NULL);
//implementation
public:

	//templates
	void SetQnrID(long qnrID);
	void DrawGeneric(CString sTitle, CString sSubTitle);
	void DrawCore(CString sTitle, CString sSubTitle);
	void DrawCDP21(int id, CString sTitle, CString sSubTitle);
	void DrawCDP22(int id, CString sTitle, CString sSubTitle);
	void DrawCDP23(int id, CString sTitle, CString sSubTitle);
	void DrawTotPop(CString sTitle, CString sSubTitle);
	void DrawMDG(CString sTitle, CString sSubTitle);
	void DrawByGroup(CString sTitle, CString sSubTitle,
		CString sNumerator, CString sDenominator, 
		CString sLevel, int units = 100);
	void DrawByGroupSex(CString sTitle, CString sSubTitle,
		CString sNumerator, CString sDenominator, 
		CString sLevel, int units = 100);
	void DrawByCat(CString sTitle, CString sSubTitle,
		CString sNumerator, CString sDenominator, 
		CString sLevel);
	void DrawByCatSex(CString sTitle, CString sSubTitle,
		CString sNumerator, CString sDenominator, 
		CString sLevel);

	void DrawFDxT(BOOL wLabels, CString sTitle, CString sSubTitle,
		LPCSTR sRow, LPCSTR sCol, int cl_size, int min, int upper, CString sZeroLab = _T("Zero"));
	void DrawXTab(BOOL wLabels, CString sTitle, CString sSubTitle,
		LPCSTR sRow, LPCSTR sCol, CString sZeroLab = _T("Zero"), 
		int numidx = -1, int denomidx = -1, CString sLabelTable = _T("~valuelabels"));

	void SetRS(CStatSimRS *pRS);
	//
	//framework specific
	void DrawPartial2HealthEduc(int givenInd, CString sTitle, CString sSubTitle);
	void DrawPartial2HouseFac(int givenInd, CString sTitle, CString sSubTitle);
	void DrawPartialILES(CString sTitle, CString sSubTitle);
	void DrawPartialEmpl(CString sTitle, CString sSubTitle, bool occup = TRUE);
	void DrawPartialProg(CString sTitle, CString sSubTitle, bool sex = TRUE);
	//

	float FileSize(long denom = 1);	//get file size, denom determines unit
	FILE* GetStream();	//Get stream

	int GetError();;

//data members
private:

	CString sSQLGeoTabs(int ID);
	int GetStrings(int ID, CStatSimRS* pRS, CString& sSQL, CString& sGeo, CString sSrcTab, CString sRow, CString sCol, CString sAddCrit);

	void IterCDP21(CString sSrcTab, int id, CStatSimRS *pUseRS, CString sCrit, CString sTitle, CString sAddlLab);

	void IterXTab(CString sSrcTab, int id, CStatSimRS *pUseRS, CString sCrit, CString sAddlLab,
		BOOL wLabels, CString sTitle, CString sSubTitle,
		LPCSTR sRow, LPCSTR sCol, CString sZeroLab = _T("Zero"),
		int numidx = -1, int denomidx = -1);
	void IterFDxT(CString sSrcTab, int id, CStatSimRS *pUseRS, CString sCrit, CString sAddlLab,
		BOOL wLabels, CString sTitle, CString sSubTitle,
		LPCSTR sRow, LPCSTR sCol, int cl_size, int min, int upper, CString sZeroLab = _T("Zero"));

	BOOL GetLabels(std::map<int, CString> &colLabels, CString var, CString sLabForZero = _T("Zero"),
		CString sFromTable = _T("~valuelabels"));
	
	void DrawSettings();	//HTML settings first
	void DrawHeaderToTitle(CString sTitle, CString sSubTitle);	//repeating header and title
	void DrawFooter();	//footer
	void DrawHeaderCols(CString sIndLabel);	//spacing
	void DrawNewLine(CString sLevelName);	//spacing
	void DrawCell(TagCell cell);	//cell

	CString sParSQLInit();
	CString sSQLInit();
	CString sParSQLFrom();
	CString sSQLFrom();

	BOOL IsDeath;
	int error;
	CStatSimElt *m_pElt, *m_pParElt;
	CStatSimInd *m_pInd;
	int m_cInd, m_cE, m_cParE;
	CStatSimConn *m_pDB;
	CStatSimRS *m_pRS, *m_pParentRS;
	LPCSTR m_sTable, m_sParentTable,
		m_IDFld, m_ParIDFld,
		m_sLabel, m_sDesc, m_sSector, m_sPropCompute,
		m_sNumerator, m_sDenominator,
		m_sNumeratorLabel, m_sDenominatorLabel,
		m_sParNumerator, m_sParDenominator,
		m_sLevel, m_sParLevel;
	CString m_sPath, m_sCrit, m_sRef;
	FILE *m_pFile;

	struct stat FDbuff;
	int htmFD;	//stream file descriptor (int)
	long m_qnrID;


};
