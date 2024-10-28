// ProcSelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StatSimPro.h"
#include "ProcSelDlg.h"

extern CStatSimConn* pGlobalConn;
extern CStatSimRS* pelemRS;

extern CMFCRibbonStatusBarPane *g_wndStatusBarPane;
extern CProgressCtrl  m_ProgStatBar;

// CProcSelDlg dialog

IMPLEMENT_DYNAMIC(CProcSelDlg, CDialog)

CProcSelDlg::CProcSelDlg(int elt_lim, CString sCaption,
	CWnd* pParent /*=NULL*/)
	: CDialog(CProcSelDlg::IDD, pParent)
{
	m_elt_lim = elt_lim;
	m_sCaption = sCaption;

}

CProcSelDlg::~CProcSelDlg()
{
}

void CProcSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_LISTUNSEL, m_ListUnselect);
	DDX_Control(pDX, IDC_LISTSEL, m_ListSelect);
	DDX_Control(pDX, IDC_SELECTALL, m_ButtonSelectAll);
	DDX_Control(pDX, IDC_SELECT, m_ButtonSelect);
	DDX_Control(pDX, IDC_DESELECTALL, m_ButtonDeselectAll);
	DDX_Control(pDX, IDC_DESELECT, m_ButtonDeselect);
}


BEGIN_MESSAGE_MAP(CProcSelDlg, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CProcSelDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CProcSelDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_SELECT, &CProcSelDlg::OnBnClickedSelect)
	ON_BN_CLICKED(IDC_DESELECT, &CProcSelDlg::OnBnClickedDeselect)
	ON_BN_CLICKED(IDC_SELECTALL, &CProcSelDlg::OnBnClickedSelectall)
	ON_BN_CLICKED(IDC_DESELECTALL, &CProcSelDlg::OnBnClickedDeselectall)
//	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTUNSEL, &CProcSelDlg::OnLvnItemchangedListunsel)
ON_NOTIFY(NM_DBLCLK, IDC_LISTUNSEL, &CProcSelDlg::OnNMDblclkListunsel)
ON_NOTIFY(NM_DBLCLK, IDC_LISTSEL, &CProcSelDlg::OnNMDblclkListsel)
END_MESSAGE_MAP()


// CProcSelDlg message handlers

BOOL CProcSelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	this->SetWindowText(m_sCaption);
	CString sSQL;
	CStatSimRS* pRS = NULL;

	m_ListUnselect.SetImage(IDB_HGLOBE);
	m_ListSelect.SetImage(IDB_GLOBE);

	//set icons for buttons
	m_ButtonSelect.SetButtonStyle(BS_ICON);
	m_ButtonSelect.SetIcon(AfxGetApp()->LoadIcon(IDI_FWD));
	m_ButtonSelectAll.SetButtonStyle(BS_ICON);
	m_ButtonSelectAll.SetIcon(AfxGetApp()->LoadIcon(IDI_FFWD));
	m_ButtonDeselect.SetButtonStyle(BS_ICON);
	m_ButtonDeselect.SetIcon(AfxGetApp()->LoadIcon(IDI_REW));
	m_ButtonDeselectAll.SetButtonStyle(BS_ICON);
	m_ButtonDeselectAll.SetIcon(AfxGetApp()->LoadIcon(IDI_FREW));

	int nColumnCount = m_ListUnselect.GetHeaderCtrl()->GetItemCount();
	int nSelColumnCount = m_ListSelect.GetHeaderCtrl()->GetItemCount();
	
	for (int i=0; i < nColumnCount; i++) {
		m_ListUnselect.DeleteColumn(0);
	}

	for (int j=0; j < nSelColumnCount; j++) {
		m_ListSelect.DeleteColumn(0);
	}

	m_OK.EnableWindow(FALSE);

	sSQL.Format(_T("SELECT `label`, `element`, `elementID` FROM `~hElement` WHERE `elementID`>=1002 AND `elementID`<=%d;"), m_elt_lim);	//select households up to region
	pRS = new CStatSimRS( pGlobalConn, sSQL);

	m_ListUnselect.InsertBulk(pRS);
	delete pRS; pRS = NULL;

	sSQL = "SELECT `label`, `element`, `elementID` FROM `~hElement` LIMIT 0;";	//select households up to region
	pRS = new CStatSimRS( pGlobalConn, sSQL);
	
	m_ListSelect.InsertBulk(pRS);
	delete pRS; pRS = NULL;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CProcSelDlg::OnBnClickedCancel()
{
	CString msgText;
	int msgResult;
	
	msgText.Format(_T("Are you sure you want to cancel processing?"));
	msgResult = AfxMessageBox (msgText, MB_YESNO);
	
	if (msgResult==IDYES) {
		CDialog::OnCancel();
	}
	else {
		return;
	}
	g_wndStatusBarPane->SetText(_T("Processing was cancelled"));
	OnCancel();
}

void CProcSelDlg::OnBnClickedOk()
{
	nElement = m_ListSelect.GetItemCount();
	elementIDArray = (int*) malloc( nElement * sizeof(int) );
	wHH = FALSE;
	
	for (int i=0; i<nElement; i++)
	{
		elementIDArray[i] = _ttoi( ( m_ListSelect.GetItemText(i, 2) ) );	//get elementID
		if ( elementIDArray[i]==HH )
			wHH = TRUE;
	}

	OnOK();
}

void CProcSelDlg::OnBnClickedSelect()
{
	m_ListUnselect.SwitchContents(&m_ListSelect);
	if (m_ListSelect.GetItemCount() > 0) {
		m_OK.EnableWindow(TRUE);
		m_OK.SetFocus();
	}
	else {
		m_OK.EnableWindow(FALSE);
	}
}

void CProcSelDlg::OnBnClickedDeselect()
{
	m_ListSelect.SwitchContents(&m_ListUnselect);
	if (m_ListSelect.GetItemCount() > 0) {
		m_OK.EnableWindow(TRUE);
		m_OK.SetFocus();
	}
	else {
		m_OK.EnableWindow(FALSE);
	}
}

void CProcSelDlg::OnBnClickedSelectall()
{
	m_ListUnselect.TransferContents(&m_ListSelect);
	
	if (m_ListSelect.GetItemCount() > 0) {
		m_OK.EnableWindow(TRUE);
		m_OK.SetFocus();
	}
	else {
		m_OK.EnableWindow(FALSE);
	}
}

void CProcSelDlg::OnBnClickedDeselectall()
{
	m_ListSelect.TransferContents(&m_ListUnselect);
	
	if (m_ListSelect.GetItemCount() > 0) {
		m_OK.EnableWindow(TRUE);
		m_OK.SetFocus();
	}
	else {
		m_OK.EnableWindow(FALSE);
	}
	
}

void CProcSelDlg::OnNMDblclkListunsel(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnBnClickedSelect();
	*pResult = 0;
}

void CProcSelDlg::OnNMDblclkListsel(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnBnClickedDeselect();
	*pResult = 0;
}
