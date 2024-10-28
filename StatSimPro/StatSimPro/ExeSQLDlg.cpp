// ExeSQLDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StatSimPro.h"
#include "ExeSQLDlg.h"
#include "StatSimHTML.h"
#include "StatSimProView.h"

extern CStatSimConn* pGlobalConn;

extern CMFCRibbonStatusBarPane  *g_wndStatusBarPane;//I have transferred this from main to global to be accessible
extern CProgressCtrl  m_ProgStatBar;	//I have transferred this from main to global to be accessible

extern CStatSimProView *g_pSSHTMLView;
extern CString sDB;

// CExeSQLDlg dialog

IMPLEMENT_DYNAMIC(CExeSQLDlg, CDialog)
CExeSQLDlg::CExeSQLDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExeSQLDlg::IDD, pParent)
{
}

CExeSQLDlg::~CExeSQLDlg()
{
}

void CExeSQLDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SQL, m_EditSQL);
}


BEGIN_MESSAGE_MAP(CExeSQLDlg, CDialog)
	ON_BN_CLICKED(IDC_RUN, OnBnClickedRun)
	ON_BN_CLICKED(IDC_CLOSE, OnBnClickedClose)
END_MESSAGE_MAP()


// CExeSQLDlg message handlers

void CExeSQLDlg::OnBnClickedRun()
{
	CString sSQL, sKW,msgText;
	CStatSimRS* pRS = 0;
	CStatSimHTML *pHTML = 0;
	sSQL = GetString(&m_EditSQL);

	sSQL.TrimLeft();
	sSQL.TrimRight();

	int sppos = sSQL.Find(_T(" "));
	sKW = sSQL.Left(sppos);
	sKW.TrimLeft();
	sKW.TrimRight();
	sKW.MakeLower();

	if (sKW==_T("select") || sKW==_T("show") ) {

		LPCTSTR sTitle, sSubTitle;
		sSubTitle = (sSQL);
		sTitle = _T( "CBMS StatSim Query");

		if (sKW==_T("show")) {
			sSQL.Replace(_T(";"), _T(""));
			sSQL.Format(_T("%s WHERE LEFT(Tables_in_%s,1)<>'~';"), sSQL, sDB);
		}

		pRS = new CStatSimRS( pGlobalConn );

		if(pRS->RunSQL(sSQL)!=SQL_SUCCESS) {
			
			CreateDirectory(_T("C:\\CBMSDatabase\\System\\Reports"), NULL);
			CreateDirectory(_T("C:\\CBMSDatabase\\System\\Reports\\Temp"), NULL);
			
			LPCTSTR sTitle, sSubTitle;
			m_sRptPath = _T("C:\\CBMSDatabase\\System\\Reports\\Temp\\StatSimRpt.htm");
			sTitle = _T( "CBMS StatSim Query");
			sSubTitle = _T("You've got error in your SQL and your query returned no results");
			
			FILE* pFile = _tfopen(m_sRptPath, _T("w"));
			pHTML = new CStatSimHTML(pRS, pFile);	//generic or core
			pHTML->DrawGeneric(sTitle, sSubTitle);		
			
			fclose(pFile);

			g_pSSHTMLView->Navigate2(m_sRptPath);
			OnOK();
			return;

		}

		CreateDirectory(_T("C:\\CBMSDatabase\\System\\Reports"), NULL);
		CreateDirectory(_T("C:\\CBMSDatabase\\System\\Reports\\Temp"), NULL);
		
		m_sRptPath = _T("C:\\CBMSDatabase\\System\\Reports\\Temp\\StatSimRpt.htm");
		
		FILE* pFile = _tfopen(m_sRptPath, _T("w"));
		pHTML = new CStatSimHTML(pRS, pFile);	//generic or core
		pHTML->DrawGeneric(sTitle, sSubTitle);

		fclose(pFile);

		g_pSSHTMLView->Navigate2(m_sRptPath);
	
	}
	
	else {

		if ( pGlobalConn->ExecuteSQL(sSQL)==SQL_SUCCESS )  {
			
			msgText.Format(_T("Your SQL statement '%s' was executed."), sSQL);
			AfxMessageBox(msgText);

			CreateDirectory(_T("C:\\CBMSDatabase\\System\\Reports"), NULL);
			CreateDirectory(_T("C:\\CBMSDatabase\\System\\Reports\\Temp"), NULL);
			
			LPCTSTR sTitle, sSubTitle;
			m_sRptPath = _T("C:\\CBMSDatabase\\System\\Reports\\Temp\\StatSimRpt.htm");
			sTitle = _T( "CBMS StatSim Query");
			sSubTitle = _T("Your query returned no results");
			
			FILE* pFile = _tfopen(m_sRptPath, _T("w"));
			pHTML = new CStatSimHTML(pRS, pFile);	//generic or core
			pHTML->DrawGeneric(sTitle, sSubTitle);		
			
			fclose(pFile);

			g_pSSHTMLView->Navigate2(m_sRptPath);
			
		}

		else {

			CreateDirectory(_T("C:\\CBMSDatabase\\System\\Reports"), NULL);
			CreateDirectory(_T("C:\\CBMSDatabase\\System\\Reports\\Temp"), NULL);
			
			LPCTSTR sTitle, sSubTitle;
			m_sRptPath = _T("C:\\CBMSDatabase\\System\\Reports\\Temp\\StatSimRpt.htm");
			sTitle = _T( "CBMS StatSim Query");
			sSubTitle = _T("You've got error in your SQL and your query returned no results");
			
			FILE* pFile = _tfopen(m_sRptPath, _T("w"));
			pHTML = new CStatSimHTML(pRS, pFile);	//generic or core
			pHTML->DrawGeneric(sTitle, sSubTitle);		
			
			fclose(pFile);

			g_pSSHTMLView->Navigate2(m_sRptPath);
		}
	}


	OnOK();
}

void CExeSQLDlg::OnBnClickedClose()
{
	OnCancel();
}

BOOL CExeSQLDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	VERIFY(m_Font.CreateFont(
		10,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Courier")));                 // lpszFacename
	
	m_EditSQL.SetFont(&m_Font);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
