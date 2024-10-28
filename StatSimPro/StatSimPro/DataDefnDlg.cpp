// DataDefnDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StatSimPro.h"
#include "DataDefnDlg.h"


// CDataDefnDlg dialog

IMPLEMENT_DYNAMIC(CDataDefnDlg, CDialog)

CDataDefnDlg::CDataDefnDlg(CStatSimConn *pSrcDB, CStatSimConn *pToDB, 
						   CWnd* pParent /*=NULL*/)
	: CDialog(CDataDefnDlg::IDD, pParent)
{
	m_pSrcDB = pSrcDB;
	m_pToDB = pToDB;

}

CDataDefnDlg::~CDataDefnDlg()
{
}

void CDataDefnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTDATADEFN, m_ListDataDefn);
}


BEGIN_MESSAGE_MAP(CDataDefnDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CDataDefnDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CDataDefnDlg message handlers

BOOL CDataDefnDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	RECT r;
	this->GetClientRect(&r);

	m_ListDataDefn.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT,
		CRect(r.left+10,r.top+10,r.right-100,r.bottom-10), this, IDC_LISTDATADEFN);

	//set the tree imagelist
	HICON *pIcons = new HICON[4];
	pIcons[0] = AfxGetApp()->LoadIcon(IDI_UNCHECKED);
	pIcons[1] = AfxGetApp()->LoadIcon(IDI_CHECKED);
	pIcons[2] = AfxGetApp()->LoadIcon(IDI_PARTIALLY);
	pIcons[3] = AfxGetApp()->LoadIcon(IDI_NOCHECK);
	m_ListDataDefn.SetImageListEx(pIcons, 4, LVSIL_STATE );
	delete [] pIcons; pIcons = 0;

	CString sSQL = _T("SELECT `label`, `qnrID` FROM `~qnr`;");
	CStatSimRS* pRS = new CStatSimRS(m_pSrcDB, sSQL);

	m_ListDataDefn.InsertBulk(pRS);
	for (int i=0; i<m_ListDataDefn.GetItemCount(); i++) {
		m_ListDataDefn.SetCheckEx(i, CStatSimList::UNCHECKED);
	}

	if (pRS->GetRecordCount()==1) {
		m_ListDataDefn.SetCheckEx(0, CStatSimList::CHECKED);
		OnBnClickedOk();
		return TRUE;
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDataDefnDlg::OnBnClickedOk()
{
	BOOL wSelect = FALSE;

	for (int i=0; i<m_ListDataDefn.GetItemCount(); i++) {
		UINT nStatus = m_ListDataDefn.GetItemState( i, LVIS_STATEIMAGEMASK ) >> 12;
		if (nStatus == CStatSimList::CHECKED) {
			wSelect = TRUE;
		}
	}

	if (!wSelect) {
		AfxMessageBox(_T("Please select questionnaires."));
		return;
	}

	for (int i=0; i<m_ListDataDefn.GetItemCount(); i++) {
		
		UINT nStatus = m_ListDataDefn.GetItemState( i, LVIS_STATEIMAGEMASK ) >> 12;
		if (nStatus != CStatSimList::CHECKED) 
			continue;

		CStatSimRS* pRS = 0;
		CString sCrit = m_ListDataDefn.GetItemText(i, 1),
			sSQL;

		sSQL.Format(_T("SELECT * FROM `~qnr` WHERE `qnrID`=%d;"), _ttoi((sCrit)));
		pRS = new CStatSimRS(m_pSrcDB, sSQL);
		m_pToDB->CreateTable(pRS, (CString) _T("~qnr"));
		m_pToDB->InsertRecords(pRS, (CString) _T("~qnr"), NULL, NULL, 
					FALSE, FALSE, TRUE);
		delete pRS; pRS = 0;
		sSQL.Format(_T("ALTER TABLE `~qnr` MODIFY COLUMN `qnrID` INTEGER NOT NULL DEFAULT 0, ADD PRIMARY KEY(`qnrID`);"));
		m_pToDB->ExecuteSQL(sSQL, FALSE);

		sSQL.Format(_T("SELECT * FROM `~rectype` WHERE `qnrID`=%d;"), _ttoi((sCrit)));
		pRS = new CStatSimRS(m_pSrcDB, sSQL);
		m_pToDB->CreateTable(pRS, (CString) _T("~rectype"));
		m_pToDB->InsertRecords(pRS, (CString) _T("~rectype"), NULL, NULL, 
					FALSE, FALSE, TRUE);
		delete pRS; pRS = 0;
		//sSQL.Format(_T("ALTER TABLE `~rectype` MODIFY COLUMN `recID` SMALLINT(6) NOT NULL, MODIFY COLUMN `qnrID` INTEGER NOT NULL DEFAULT 0, ADD PRIMARY KEY(`recID`, `qnrID`);"));
		//m_pToDB->ExecuteSQL(sSQL, FALSE);

		sSQL.Format(_T("SELECT * FROM `~helementvar` WHERE `qnr`=%d;"), _ttoi((sCrit)));
		pRS = new CStatSimRS(m_pSrcDB, sSQL);
		m_pToDB->CreateTable(pRS, (CString) _T("~helementvar"));
		m_pToDB->InsertRecords(pRS, (CString) _T("~helementvar"), NULL, NULL, 
					FALSE, FALSE, TRUE);
		delete pRS; pRS = 0;
		sSQL.Format(_T("ALTER TABLE `~helementvar` MODIFY COLUMN `var` VARCHAR(255) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '', \
					   MODIFY COLUMN `elementID` SMALLINT(6) NOT NULL DEFAULT 0, \
					   ADD PRIMARY KEY(`var`, `elementID`);"));
		m_pToDB->ExecuteSQL(sSQL, FALSE);

		//what else!!!! - helement???
	}

	OnOK();
}
