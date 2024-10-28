// LoginDlg.cpp : implementation file
//
#include "stdafx.h"
#include "LoginDlg.h"
#include "daeconv.h"

#ifndef STATSIMCONN
	#include "StatSimConn.h"
#endif
#ifndef STATSIMRS
	#include "StatSimRS.h"
#endif

extern CStatSimConn* pGlobalConn;
extern CStatSimRS* pelemRS;
extern CString userName, userPwd, sHost, sPort, sDB;
extern CStatSimRS ***pdictRS;
extern daedict ***pDCT;

extern bool IS_ODBC_INT;

extern CString dbPrefix;

extern CMFCRibbonStatusBarPane *g_wndStatusBarPane;
extern CProgressCtrl  m_ProgStatBar;

extern LPCSTR** qnrtables;
extern int *ntypes, nqnr;
extern int *rtlen;
extern LPCSTR** RTYPES;
extern long *qnrIDs;

extern float USDtoPHP;	//exchange rate
extern BOOL IsDirectProc;	//whether processing is by batch or direct
extern int nProcBatches;
extern vector<BOOL> arrayProcExist;
extern vector<LPCSTR> arrayBatchDB;

extern long hpq_id;
extern BOOL g_askBasicInd;
extern CStringArray sGeoLevels;
extern BOOL _APP_, _PAP_;

// CLoginDlg dialog

IMPLEMENT_DYNAMIC(CLoginDlg, CDialog)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoginDlg::IDD, pParent)
{

}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TEXTHOST, m_TextHost);
	DDX_Control(pDX, IDC_TEXTPORT, m_TextPort);
	DDX_Control(pDX, IDC_TEXTUSER, m_TextUser);
	DDX_Control(pDX, IDC_TEXTPWD, m_TextPwd);
	DDX_Control(pDX, IDC_COMBODB, m_ComboDB);
	DDX_Control(pDX, IDOK, m_OK);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CLoginDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CLoginDlg message handlers

BOOL CLoginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//CSphereProgress dlg(m_hWnd, ProgressFunction, L"Lengthy operation being performed...", true);
	//CSphereProgress dlg();

	this->SetIcon(AfxGetApp()->LoadIcon(IDI_KEYS), FALSE);

	VERIFY(m_Font.CreateFont(
		14,                        // nHeight
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
		_T( "Symbol")));                 // lpszFacename

	m_TextHost.SetWindowText( _T("localhost") );
	m_TextPort.EnableWindow(FALSE);
	//m_TextHost.EnableWindow(FALSE);
	m_TextPort.SetWindowText( _T("3306") );

	m_TextUser.SetWindowText( _T("admin") );
	//m_TextPwd.SetWindowText(_T("kontrabigbossdb"));
	m_TextPwd.SetFont(&m_Font);
	m_TextPwd.SetPasswordChar('*');
	
	PopulateDB();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CLoginDlg::PopulateDB()
{

	//connect just to populate the combo box
	delete pGlobalConn; pGlobalConn = 0;

	pGlobalConn = new CStatSimConn( _T("root"), _T(""), _T("localhost"), _T("3306"), _MBCS("mysql"), 
		FALSE);
	if (!pGlobalConn->IsConnected()) {
		AfxMessageBox(_T("Can't connect. Either wrong user name or password."));
		return;
	}

	CString sSQL;
	//sSQL.Format(L"SHOW DATABASES WHERE LEFT(`database`, %d) = '%s';", dbPrefix.GetLength(), dbPrefix);
	sSQL = L"SHOW DATABASES WHERE `database`<>'mysql' AND `database`<>'information_schema' AND \n\
			`database`<>'performance_schema' and `database`<>'test'";
	CStatSimRS *pRS = 0;

	pRS = new CStatSimRS(pGlobalConn, sSQL);
	int nRec = pRS->GetRecordCount();
	if (nRec>0) {
		pRS->MoveFirst();
	}
	else{
		m_nDB = 0;
	}

	for (int i=0; i<nRec; i++) {
		CString str = pRS->SQLFldValue((USHORT) 0);

		//AfxMessageBox(str);

		if (str.Find(_T("~"))==-1) {

			//increment number of dbs
			m_nDB++;

			//add string to database list
            m_ComboDB.AddString( str );

			//set data
			m_ComboDB.SetItemData(m_nDB-1, m_nDB);
			//m_sDB.push_back( ConstChar(str) );
			m_sDB.Add(str);
		}
		
		pRS->MoveNext();

	}

	//set the first database else statsimcbms
	if (nRec>0) {
		m_ComboDB.SetCurSel(0);
	}
	else {
		//update to set recent 
		m_ComboDB.SetWindowText( _T("StatSim-CBMS") );
	}

	delete pRS; pRS = 0;
	//AfxMessageBox(_T("Hello"));
}

void CLoginDlg::OnBnClickedOk()
{
	//SET LOCALE
	//setlocale(LC_ALL, "en_US.UTF-8");

	//AfxMessageBox(GetVersionInfo(NULL, _T("FILEVERSION")));
	//AfxMessageBox(GetVersionInfo(NULL, _T("FileDescription")));

	//CString msg;
	//msg.Format(L"Size of VARIANT: %d", sizeof(daevariant));

	//AfxMessageBox(msg);


	//varvec2d v2ds;
	//v2ds.resize(2);
	//v2ds.at(0).resize(2);
	//v2ds.at(1).resize(2);

	//v2ds.at(0).at(0) = L"00";
	//v2ds.at(0).at(1) = L"01";
	//v2ds.at(1).at(0) = L"10";
	//v2ds.at(1).at(1) = L"01";

	//LPCSTR **v2dst = v2ds;

	//AfxMessageBox(v2ds.at(0).at(0));
	//AfxMessageBox(v2ds.at(0).at(1));
	//AfxMessageBox(v2ds.at(1).at(0));
	//AfxMessageBox(v2ds.at(0).at(1));

	//AfxMessageBox(v2dst[0][0]);
	//AfxMessageBox(v2dst[0][1]);
	//AfxMessageBox(v2dst[0][0]);
	//AfxMessageBox(v2dst[1][1]);
	//CDataGridDlg dlg;
	//dlg.DoModal();

/*	char ch[16];
	VARIANT var[16];
	int idx;
	for (idx=0; idx<16; idx++)
	{
		ch[idx] = 'a';
		var[idx].intVal = 0;
	}

	int* ptr = 0;

	int sizeV = sizeof(BSTR);
	int sizeC = sizeof(ptr);
	CString msg;
	msg.Format(_T("Char: %d VARIANT: %d"), sizeC, sizeV);
	AfxMessageBox(msg);
*/		

	//vector< vector< genvec* > > entries;
	//entries.push_back(new statsimvec<int>);
	//entries.push_back(new statsimvec<LPCSTR>);

	//vector< vector<unsigned int> > pole;
	//int m=10, n=5;
	//for (int i=0; i<n; i++) {
	//	vector<unsigned int> po;
	//	for (int j=0; j<m; j++) {
    //        po.push_back(j+1);
	//	}
	//	pole.push_back(po);
	//	CString msg;
	//	msg.Format(_T("%d"), pole.at(i).at(i));
	//	AfxMessageBox(msg);
	//}

	int getLineRes, editLen;
	CString sSQL;
	CStringArray sSQLArray;

	//Host
	m_TextHost.GetWindowText(sHost);
	
	//Port
	m_TextPort.GetWindowText(sPort);

	//User
	editLen = m_TextUser.LineLength(1);
	getLineRes = m_TextUser.GetLine(0, userName.GetBuffer(editLen), editLen);
	
	userName.ReleaseBuffer(editLen); userName.MakeLower();

	/*
	if ( IsNull(userName) ) {
		AfxMessageBox( _T("Please enter user name.") );
		return;
	}
	*/

	if (userName == "admin" || userName == "administrator") {
		userName = "root";
	}
	//Password	
	editLen = m_TextPwd.LineLength(1);
	getLineRes = m_TextPwd.GetLine(0, userPwd.GetBuffer(editLen), editLen);
	userPwd.ReleaseBuffer(editLen);

	//DB
	//CString dbSuff;
	m_ComboDB.GetWindowText(sDB);
	sDB.Trim();
	//dbSuff.Trim();
	//sDB = dbPrefix + L"-" + dbSuff;

	if (sDB.Find(_T("~"))>=0) {
		CString msg(_T("Invalid character '~'."));
		AfxMessageBox(msg);
		return;
	}

	//test existence
	if ( !Exists(m_sDB, ConstChar(sDB)) ) {
		CString sMsg;
		sMsg.Format(_T("Database '%s' still does not exist.  Do you want to create and initialize it?"), 
			sDB);
		
		int msgResult = AfxMessageBox(sMsg, MB_YESNO|MB_ICONEXCLAMATION);
		if (msgResult==IDNO) {
			return;
		}
	}
	

	//global might have been connected, delete it
	delete pGlobalConn; pGlobalConn=0;
    	
	//connect - FALSE odbc
	pGlobalConn = new CStatSimConn( userName, userPwd, sHost, sPort, ConstChar(sDB), ODBC );
	if (!pGlobalConn->IsConnected()) {
		AfxMessageBox(_T("Can't connect. Either wrong user name or password."));
		return;
	}


	//Configure elements
	if (!ConfigDB(pGlobalConn)) {
		return;
	}

	//CTreeTestDlg dlg;
	//dlg.DoModal();
	//configure the record type!
	CStatSimRS *rtRS = 0, *pRS = 0;
	int i, j;

	// number of qnrs
	pRS = new CStatSimRS(pGlobalConn, (CString) _T("SELECT * FROM `~qnr`"));
	nqnr = pRS->GetRecordCount();
	if (nqnr>0)
		pRS->MoveFirst();

	// questionnaire IDs
	qnrIDs = new long[nqnr];
	//number of record types
	ntypes = new int[nqnr];
	// record types
	RTYPES = new LPCSTR*[nqnr];
	// record type lengths set at 0
	rtlen = new int[nqnr];
	for (j=0; j<nqnr; j++) {
		rtlen[j]=0;
	}
	// tables of questionnaires
	qnrtables = new LPCSTR*[nqnr];
	// dictionary records
	pdictRS = new CStatSimRS**[nqnr];
	// data dicitonaries
	pDCT = new daedict**[nqnr];

	_PAP_ = FALSE;
	_APP_ = FALSE;

	for (i=0; i<nqnr; i++) {

		qnrIDs[i] = _ttol( ( pRS->SQLFldValue(_MBCS("qnrID")) ) );

		if (_ttoi(( pRS->SQLFldValue(_MBCS("qnrID"))).Right(1))==1)
			_PAP_=TRUE;

		if (_ttoi(( pRS->SQLFldValue(_MBCS("qnrID"))).Right(1))==0)
			_APP_=TRUE;

		if ( qnrIDs[i]/100 == 0)
			hpq_id = qnrIDs[i];
	
		sSQL.Format(_T("SELECT * FROM `~rectype` WHERE `qnrID`=%d ORDER by `recID`;"), qnrIDs[i]);
		rtRS = new CStatSimRS( pGlobalConn, sSQL );		
		ntypes[i] = rtRS->GetRecordCount();
		qnrtables[i] = new LPCSTR[ntypes[i]];
		RTYPES[i] = new LPCSTR[ntypes[i]];

		pdictRS[i] = new CStatSimRS*[ntypes[i]];
		pDCT[i] = new daedict*[ntypes[i]];
		
		if (ntypes[i]>0)
			rtRS->MoveFirst();

		// assign 
		for (j=0; j<ntypes[i]; j++) {
			qnrtables[i][j] = ConstChar( rtRS->SQLFldValue(_MBCS("record")) );

			//AfxMessageBox((CString) qnrtables[i][j]);

			RTYPES[i][j] = ConstChar( rtRS->SQLFldValue(_MBCS("recID")) );
			//AfxMessageBox(RTYPES[i][j]);

			sSQL.Format(_T("SELECT * FROM `~hElementVar` WHERE (`rtype`='%s' AND `qnr`=%d) ORDER BY `seqno`;"), (CString) RTYPES[i][j], qnrIDs[i]);
			pdictRS[i][j] = new CStatSimRS( pGlobalConn, sSQL, TRUE);
			//AfxMessageBox(L"Ok");
		
			//pdictRS[i][j]->GetRSArray();
			pDCT[i][j] = new daedict( pdictRS[i][j]->GetRSArray() );

			//CString msg(pDCT[i][j]->GetVarName(0));
			//AfxMessageBox(msg);
			rtRS->MoveNext();
		}
		delete rtRS; rtRS = 0;
		pRS->MoveNext();
	}
	
	delete pRS; pRS = 0;

	//Length of record type variable
	sSQL = "SELECT * FROM `~hElementVar` WHERE `var`='rtype' GROUP BY `var`, qnr;";
	pRS = new CStatSimRS( pGlobalConn, sSQL );
	if ( pRS->GetRecordCount()>0 ) {		
		pRS->MoveFirst();
		
		//number of records might vary: for (i=0; i<nqnr; i++) {
		for (i=0; i<pRS->GetRecordCount(); i++) {
			int id = _ttoi( ( pRS->SQLFldValue(_MBCS("qnr")) ) );
			for (j=0; j<nqnr; j++) {
				if (id==qnrIDs[j]) { 
					LPCSTR sLen = ConstChar( pRS->SQLFldValue(_MBCS("len")) );
					rtlen[j]=atoi( sLen );
				}
			}	
			pRS->MoveNext();
		}
	}
	else {
		for (j=0; j<nqnr; j++) {//account for MDC
			rtlen[j]=-1;
		}	
	}

	delete pRS; pRS = NULL;

	pelemRS = new CStatSimRS( pGlobalConn );
	sSQL = "SELECT * FROM `~hElement` ORDER by `parent`;";
	pelemRS->RunSQL(sSQL);

	//the options table
	sSQL = _T("CREATE TABLE IF NOT EXISTS `~options` (\n\
		`optID` INTEGER UNSIGNED NOT NULL, \n\
		`optValue` FLOAT(4,2) UNSIGNED NOT NULL DEFAULT 0, \n\
		PRIMARY KEY(`optID`));");
	pGlobalConn->ExecuteSQL(sSQL);

	//test records
	sSQL = "SELECT * FROM `~options`;";
	pRS = new CStatSimRS( pGlobalConn, sSQL);
	if (pRS->GetRecordCount() == 0) {
		sSQL = "INSERT INTO `~options` (`optID`, `optValue`) VALUES (1,1), (2,48.00);";
		pGlobalConn->ExecuteSQL(sSQL);
	}
	delete pRS; pRS = NULL;

	//initial direct proc value
	IsDirectProc = 0;
	sSQL.Format(_T("SELECT * FROM `~options` WHERE `optID`=%d;"), 1 /*PROCMETHOD*/);
	pRS = new CStatSimRS( pGlobalConn, sSQL);
	
	if (pRS->GetRecordCount() > 0) {
		pRS->MoveFirst();
		IsDirectProc = _ttoi( ( pRS->SQLFldValue(_MBCS("optValue")) ) );
	}

	if (!IsDirectProc) {
		CString sql;
		sql = L"CREATE TABLE IF NOT EXISTS `~batchdb`(\n"
			L"`dbname` VARCHAR(255) NOT NULL, \n"
			L"`description` TEXT NULL, \n"
			L"`imported` TINYINT(1) UNSIGNED NOT NULL, \n"
			L"`processed` TINYINT(1) UNSIGNED NOT NULL, \n"
			L"PRIMARY KEY (`dbname`)) \n"
			L"ENGINE = MYISAM;";
		pGlobalConn->ExecuteSQL(sql);
	}

	//set batches to initial
	nProcBatches = 0;
	delete pRS; pRS = 0;

	//initial direct proc value
	USDtoPHP = 0;
	sSQL.Format(_T("SELECT * FROM `~options` WHERE `optID`=%d;"), 2 /*FOREX*/);
	pRS = new CStatSimRS( pGlobalConn, sSQL);
	
	if (pRS->GetRecordCount() > 0) {
		pRS->MoveFirst();
		USDtoPHP = tstof( ( pRS->SQLFldValue(_MBCS("optValue")) ) );
	}
	delete pRS; pRS = 0;
	
	if (!IsDirectProc) {
		BuildBatchDBs();
	}

	//CProgressExDlg *dlg = new CProgressExDlg();
	//dlg->m_ProgressList->InitProgressBars();
	//dlg->DoModal();
	//dlg->SetProgress(0, 100);

	//CSSProgressManager dlgmgr;
	//dlgmgr.Init(this->m_hWnd);
	//dlgmgr.BeginProgressDialog();
	//dlgmgr.Exit();;

	//set current hpq_id
	sSQL.Format(_T("SELECT `qnrID` FROM `~qnr` WHERE `questionnaire`='HPQ' LIMIT 1;"), 1 /*PROCMETHOD*/);
	pRS = new CStatSimRS( pGlobalConn, sSQL);
	if (pRS->GetRecordCount() > 0) {
		pRS->MoveFirst();
		CString sVal = pRS->SQLFldValue(_MBCS("qnrID"));
		hpq_id = _ttoi( ( sVal ) );
	}
	delete pRS; pRS = 0;
	
	//detect hh_ind
	if(TableExists(_T("hh_ind"), pGlobalConn) ) {
		g_askBasicInd = TRUE;
	}
	else {
		g_askBasicInd = FALSE;
	}
	
	sSQL = _T("SELECT `element` FROM `~helement` WHERE `etype`=1;");
	pRS = new CStatSimRS( pGlobalConn, sSQL);
	if (pRS->GetRecordCount() > 0) {
		pRS->MoveFirst();
		for (int i=0; i<pRS->GetRecordCount(); i++) {
			sGeoLevels.Add(pRS->SQLFldValue(_MBCS("element")));
		}
	}
	delete pRS; pRS = 0;


///////////////////////////	
//	MakeUnionQuery();
///////////////////////////
	OnOK();
}






