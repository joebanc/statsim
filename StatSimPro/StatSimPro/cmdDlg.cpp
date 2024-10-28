// CmdDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StatSimPro.h"
#include "CmdDlg.h"
#include "StatSimHTML.h"
#include "StatSimProView.h"

extern CStatSimConn* pGlobalConn;

extern CMFCRibbonStatusBarPane  *g_wndStatusBarPane;//I have transferred this from main to global to be accessible
extern CProgressCtrl  m_ProgStatBar;	//I have transferred this from main to global to be accessible

extern CStatSimProView *g_pSSHTMLView;
extern CString sDB;

// CCmdDlg dialog

IMPLEMENT_DYNAMIC(CCmdDlg, CDialog)
CCmdDlg::CCmdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCmdDlg::IDD, pParent)
{
	m_sTable = L"hpq_mem";
}

CCmdDlg::~CCmdDlg()
{
}

void CCmdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SQL, m_EditCmd);
}


BEGIN_MESSAGE_MAP(CCmdDlg, CDialog)
	ON_BN_CLICKED(IDC_RUN, OnBnClickedRun)
	ON_BN_CLICKED(IDC_CLOSE, OnBnClickedClose)
END_MESSAGE_MAP()


// CCmdDlg message handlers

void CCmdDlg::OnBnClickedRun()
{
	CString sCmd, sKW, sPredicate,
		msgText;
	CStatSimRS* pRS = 0;
	CStatSimHTML *pHTML = 0;
	sCmd = GetString(&m_EditCmd);

	sCmd.TrimLeft();
	sCmd.TrimRight();

	int sppos = sCmd.Find(_T(" "));

	sKW = sCmd.Left(sppos);
	sKW.TrimLeft();
	sKW.TrimRight();
	sKW.MakeLower();

	sPredicate = sCmd.Right(sCmd.GetLength() - sppos);
	sPredicate.TrimLeft();
	sPredicate.TrimRight();
	sPredicate.MakeLower();
	
	if (sKW==_T("use") ) {

		m_sTable.Format(_T("%s"), sPredicate);
		msgText.Format(_T("%s is active"), sPredicate);
		OnInitDialog();
		
		return;
	}

	else if (sKW==_T("tab") ) {

		CString sSQL;
		
		int sppos2 = sPredicate.Find(_T(" "));
		CString sVarX = sPredicate.Left(sppos2);

		sVarX.TrimLeft();
		sVarX.TrimRight();
		sVarX.MakeLower();

		CString sVarY = sPredicate.Right(sPredicate.GetLength() - sppos2);

		sVarY.TrimLeft();
		sVarY.TrimRight();
		sVarY.MakeLower();

		LPCTSTR sTitle, sSubTitle;
		sSubTitle = (sCmd);
		sTitle = _T( "CBMS StatSim Command");

		CreateDirectory(_T("C:\\CBMSDatabase\\System\\Reports"), NULL);
		CreateDirectory(_T("C:\\CBMSDatabase\\System\\Reports\\Temp"), NULL);
			
		m_sRptPath = _T("C:\\CBMSDatabase\\System\\Reports\\Temp\\StatSimRpt.htm");
			
		FILE* pFile = _tfopen(m_sRptPath, _T("w"));
		sSQL.Format(_T("SELECT `%s`, `%s` FROM `%s`;"), sVarX, sVarY, m_sTable);
		pRS = new CStatSimRS(pGlobalConn, sSQL);
		pHTML = new CStatSimHTML(pGlobalConn, pFile);	//generic or core
		pHTML->SetRS(pRS);

		pHTML->DrawXTab(TRUE, (CString) sTitle, (CString) sSubTitle, ConstChar(sVarX), ConstChar(sVarY));
		
		fclose(pFile);

		g_pSSHTMLView->Navigate2(m_sRptPath);
		OnOK();
		return;

	}
	
	else if(sKW==_T("fdt")) {

		CString sSQL;

		int sppos2 = sPredicate.Find(_T(" "));
		CString sVarX = sPredicate.Left(sppos2);

		sVarX.TrimLeft();
		sVarX.TrimRight();
		sVarX.MakeLower();

		CString sVarY = sPredicate.Right(sPredicate.GetLength() - sppos2);

		sVarY.TrimLeft();
		sVarY.TrimRight();
		sVarY.MakeLower();

		LPCTSTR sTitle, sSubTitle;
		sSubTitle = (sCmd);
		sTitle = _T( "CBMS StatSim Command");

		CreateDirectory(_T("C:\\CBMSDatabase\\System\\Reports"), NULL);
		CreateDirectory(_T("C:\\CBMSDatabase\\System\\Reports\\Temp"), NULL);
			
		m_sRptPath = _T("C:\\CBMSDatabase\\System\\Reports\\Temp\\StatSimRpt.htm");
			
		FILE* pFile = _tfopen(m_sRptPath, _T("w"));
		sSQL.Format(_T("SELECT `%s`, `%s` FROM `%s`;"), sVarX, sVarY, m_sTable);
		pRS = new CStatSimRS(pGlobalConn, sSQL);
		pHTML = new CStatSimHTML(pGlobalConn, pFile);	//generic or core
		pHTML->SetRS(pRS);
		pHTML->DrawFDxT(TRUE, (CString) sTitle, (CString) sSubTitle, ConstChar(sVarX), ConstChar(sVarY), 
			5, 10, 80 );	
			
		fclose(pFile);

		g_pSSHTMLView->Navigate2(m_sRptPath);
		OnOK();
		return;
	}


	OnOK();
}

void CCmdDlg::OnBnClickedClose()
{
	OnCancel();
}

BOOL CCmdDlg::OnInitDialog()
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
	
	m_EditCmd.SetFont(&m_Font);

	CString sWndTxt;
	sWndTxt = _T("StatSim: Execute Command on ") + m_sTable.MakeUpper();
	SetWindowText(sWndTxt);

	m_EditCmd.Clear();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
