// SelDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StatSimPro.h"
#include "SelDataDlg.h"

extern CStatSimConn* pGlobalConn;
extern CString sDB;

IMPLEMENT_DYNAMIC(CSelDataDlg, CDialog)

/////////////////////////////////////////////////////////////////////////////
// CSelDataDlg dialog


CSelDataDlg::CSelDataDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelDataDlg::IDD, pParent)
{
}
CSelDataDlg::~CSelDataDlg()
{
}

void CSelDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelDataDlg)
	DDX_Control(pDX, IDC_COMBO_DATA, m_ComboData);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSelDataDlg, CDialog)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSelDataDlg message handlers

BOOL CSelDataDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CStatSimRS *pRS;
	
	CString sWndText, sSQL;
	sWndText.Format(_T("StatSim - Select data:"));
	this->SetWindowText(sWndText);

	CString labelFld;
	labelFld.Format(_T("Tables_in_%s"), sDB);

	sSQL.Format(_T("SHOW TABLES WHERE LEFT(`%s`,1)<>'~';"), labelFld);
	pRS = new CStatSimRS( pGlobalConn, sSQL);

	//pRS->PutValuesTo(&m_ComboData, _T("element"), _T("elementID"), _T("element"));

	pRS->MoveFirst();
	for (UINT i=0; i<pRS->GetRecordCount(); i++)
	{		
		CString itmLabel = pRS->SQLFldValue(ConstChar(labelFld));

		int iCB = m_ComboData.AddString(itmLabel),
			iDW = m_ComboData.SetItemData(iCB, i+1);
		

		pRS->MoveNext();
	}

	m_ComboData.SetCurSel(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelDataDlg::OnOK() 
{

	m_sTable = ConstChar( GetString(&m_ComboData) );
	CDialog::OnOK();
}
