// ClearSelHHDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StatSimPro.h"
#include "ClearSelHHDlg.h"

extern CStatSimConn* pGlobalConn;
extern CString userName, userPwd, sHost, sPort, sDB;

// CClearSelHHDlg dialog

IMPLEMENT_DYNAMIC(CClearSelHHDlg, CDialog)

CClearSelHHDlg::CClearSelHHDlg( CWnd* pParent /*=NULL*/)
	: CDialog(CClearSelHHDlg::IDD, pParent)
{

}

CClearSelHHDlg::~CClearSelHHDlg()
{
}

void CClearSelHHDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTDATADEFN, m_ListGrp);
}


BEGIN_MESSAGE_MAP(CClearSelHHDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CClearSelHHDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CClearSelHHDlg message handlers

BOOL CClearSelHHDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	RECT r;
	this->GetClientRect(&r);

	m_ListGrp.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT,
		CRect(r.left+10,r.top+10,r.right-100,r.bottom-10), this, IDC_LISTDATADEFN);

	//set the tree imagelist
	HICON *pIcons = new HICON[4];
	pIcons[0] = AfxGetApp()->LoadIcon(IDI_UNCHECKED);
	pIcons[1] = AfxGetApp()->LoadIcon(IDI_CHECKED);
	pIcons[2] = AfxGetApp()->LoadIcon(IDI_PARTIALLY);
	pIcons[3] = AfxGetApp()->LoadIcon(IDI_NOCHECK);
	m_ListGrp.SetImageListEx(pIcons, 4, LVSIL_STATE );
	delete [] pIcons; pIcons = 0;

	CString sSQL;
	sSQL = "SELECT `brgy`.`Barangay`, `mun`.`Municipality`, `prov`.`Province`, `regn`.`Region`, `brgy`.`brgyID` \n\
		   FROM `hpq_hh` INNER JOIN `brgy` USING (`regn`, `prov`, `mun`, `brgy`)\n\
		   INNER JOIN `mun` USING (`regn`, `prov`, `mun`) \n\
		   INNER JOIN `prov` USING (`regn`, `prov`) \n\
		   INNER JOIN `regn` USING (`regn`) \n\
		   GROUP BY `regn`, `prov`, `mun`, `brgy`;";

	CStatSimRS* pRS = new CStatSimRS(pGlobalConn, sSQL);

	m_ListGrp.InsertBulk(pRS);
	for (int i=0; i<m_ListGrp.GetItemCount(); i++) {
		m_ListGrp.SetCheckEx(i, CStatSimList::UNCHECKED);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CClearSelHHDlg::OnBnClickedOk()
{
	int nSelect = 0;

	for (int i=0; i<m_ListGrp.GetItemCount(); i++) {
		UINT nStatus = m_ListGrp.GetItemState( i, LVIS_STATEIMAGEMASK ) >> 12;
		if (nStatus == CStatSimList::CHECKED) {
			nSelect++;
		}
	}

	if (!nSelect) {
		AfxMessageBox(_T("Please select barangay(s) to remove."));
		return;
	}

	CString msgText;
	msgText.Format(_T("Are you sure you want to delete data of the selected barangays?"));
	int msgResult = AfxMessageBox (msgText, MB_YESNO);

	if(msgResult == IDNO) {
		return;
	}

	for (int i=0; i<m_ListGrp.GetItemCount(); i++) {
		
		UINT nStatus = m_ListGrp.GetItemState( i, LVIS_STATEIMAGEMASK ) >> 12;
		if (nStatus != CStatSimList::CHECKED) 
			continue;

		CStatSimRS* pRS = 0;
		CString sCrit = m_ListGrp.GetItemText(i, 4 /*brgyID*/),
			sSQL, sSQLExec, sFld;

		//sSQL = "SELECT `element` FROM `~helement` WHERE `elementID`>=2001 AND `elementID`<=2050;"; //up to before mem_ind
		sFld.Format(_T("Tables_in_%s"), sDB);
		sSQL.Format(_T("SHOW TABLES WHERE LEFT(`%s`, 4) = 'hpq_';"), sFld );
		pRS = new CStatSimRS(pGlobalConn, sSQL);

		pRS->MoveFirst();

		for (int j=0; j<pRS->GetRecordCount(); j++) {
			sSQLExec.Format(_T("DELETE IGNORE FROM `%s` WHERE CONCAT(`regn`, `prov`, `mun`, `brgy`)='%s';"), 
				pRS->SQLFldValue(ConstChar(sFld)), sCrit);

			pGlobalConn->ExecuteSQL(sSQLExec);
			pRS->MoveNext();
		}

		//what else!!!! - helement???
	}

	pGlobalConn->ExecuteSQL(_T("DROP TABLE IF EXISTS `hh_ind`,`mem_ind`;"));
	if(nSelect>1)
		msgText.Format(_T("Households from %d barangays were removed."), nSelect);
	else
		msgText.Format(_T("Households from %d barangay were removed."), nSelect);

	AfxMessageBox(msgText);

	OnOK();
}
