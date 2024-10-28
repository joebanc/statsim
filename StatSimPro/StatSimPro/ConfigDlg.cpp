// ConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StatSimPro.h"
#include "ConfigDlg.h"

extern DWORD hlimit;
extern CMFCRibbonStatusBarPane *g_wndStatusBarPane;
extern CProgressCtrl  m_ProgStatBar;

// CConfigDlg dialog

IMPLEMENT_DYNAMIC(CConfigDlg, CDialog)

CConfigDlg::CConfigDlg(CStatSimConn *pSrcDB, CStatSimConn *pToDB, CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDlg::IDD, pParent)
{

	m_TreeLevel = new CStatSimTree();
	m_pSrcDB = pSrcDB;
	m_pToDB = pToDB;
	m_sListCols = 0;
	m_nListI = 0, m_nListJ = 2; // name, ID

	m_pElt = 0;

}

CConfigDlg::~CConfigDlg()
{

	delete m_TreeLevel; m_TreeLevel = 0;
	delete m_pElt; m_pElt = 0;
	delete [] m_sListCols; m_sListCols = 0;
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDC_COMBOLEVEL, m_ComboLevel);
	DDX_Control(pDX, IDC_DESELECT, m_Deselect);
	DDX_Control(pDX, IDC_DESELECTALL, m_DeselectAll);
	DDX_Control(pDX, IDC_LISTSEL, m_ListSel);
	DDX_Control(pDX, IDC_LISTUNSEL, m_ListUnsel);
	DDX_Control(pDX, IDC_SELECT, m_Select);
	DDX_Control(pDX, IDC_SELECTALL, m_SelectAll);
	DDX_Control(pDX, IDOK, m_Ok);
	DDX_Control(pDX, IDC_TREELEVEL, *m_TreeLevel);
	DDX_Control(pDX, IDC_FRAMESELECT, m_FrameSelect);
	DDX_Control(pDX, IDC_VIEW, m_View);
	DDX_Control(pDX, IDC_CHANGE, m_Change);
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
	ON_BN_CLICKED(IDC_SELECT, &CConfigDlg::OnBnClickedSelect)
	ON_BN_CLICKED(IDOK, &CConfigDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBOLEVEL, &CConfigDlg::OnCbnSelchangeCombolevel)
	ON_BN_CLICKED(IDCANCEL, &CConfigDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_VIEW, &CConfigDlg::OnBnClickedView)
	ON_BN_CLICKED(IDC_DESELECT, &CConfigDlg::OnBnClickedDeselect)
	ON_BN_CLICKED(IDC_SELECTALL, &CConfigDlg::OnBnClickedSelectall)
	ON_BN_CLICKED(IDC_DESELECTALL, &CConfigDlg::OnBnClickedDeselectall)
	ON_BN_CLICKED(IDC_CHANGE, &CConfigDlg::OnBnClickedChange)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTUNSEL, &CConfigDlg::OnNMDblclkListunsel)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTSEL, &CConfigDlg::OnNMDblclkListsel)
END_MESSAGE_MAP()

// CConfigDlg message handlers

BOOL CConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//set the tree imagelist
	HICON *pIcons = new HICON[4];
	pIcons[0] = AfxGetApp()->LoadIcon(IDI_NOCHECK);
	pIcons[1] = AfxGetApp()->LoadIcon(IDI_UNCHECKED);
	pIcons[2] = AfxGetApp()->LoadIcon(IDI_CHECKED);
	pIcons[3] = AfxGetApp()->LoadIcon(IDI_PARTIALLY);
	m_TreeLevel->SetImageListEx(pIcons, 4);
	delete [] pIcons; pIcons = 0;

	//list unselected
	pIcons = new HICON[1];
	pIcons[0] = AfxGetApp()->LoadIcon(IDI_GLOBE);
	m_ListUnsel.SetImageListEx(pIcons, 1);
	delete [] pIcons; pIcons = 0;

	//list selected
	pIcons = new HICON[1];
	pIcons[0] = AfxGetApp()->LoadIcon(IDI_GLOBEOK);
	m_ListSel.SetImageListEx(pIcons, 1);
	delete [] pIcons; pIcons = 0;
	//

	//set icons for buttons
	m_Select.SetButtonStyle(BS_ICON);
	m_Select.SetIcon(AfxGetApp()->LoadIcon(IDI_FWD));
	m_SelectAll.SetButtonStyle(BS_ICON);
	m_SelectAll.SetIcon(AfxGetApp()->LoadIcon(IDI_FFWD));
	m_Deselect.SetButtonStyle(BS_ICON);
	m_Deselect.SetIcon(AfxGetApp()->LoadIcon(IDI_REW));
	m_DeselectAll.SetButtonStyle(BS_ICON);
	m_DeselectAll.SetIcon(AfxGetApp()->LoadIcon(IDI_FREW));

	CString sSQL;
	sSQL.Format( _T("SELECT * FROM `~helement` WHERE (`elementID`<=%d AND `elementID`>=%d) ORDER BY `elementID`;"), hlimit, REGN);
	CStatSimRS *pRS = new CStatSimRS(m_pSrcDB, sSQL);

	pRS->PutValuesTo(&m_ComboLevel, _MBCS("label"), _MBCS("elementID"), _MBCS("element"));

	m_ComboLevel.SetCurSel(0);
	OnCbnSelchangeCombolevel();

	delete pRS; pRS = 0;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CConfigDlg::OnBnClickedOk()
{
	ELEMENT elt = BRGY; 
	CStatSimElt *pBaseElt = NULL, *pElt = NULL;
	pBaseElt = new CStatSimElt(m_pSrcDB, m_hlimit, TRUE);
	CStatSimRS* pRS = NULL;

	CString sSQL;
	int n = m_ListSel.GetItemCount();
	LPCSTR* sCritVals = 0;
	LPCSTR** sListArray = m_ListSel.GetItemArray();

	sCritVals = new LPCSTR[n];

	for (int i=0; i<n; i++) {
		sCritVals[i] = sListArray[i][1];
	}

	//return;
	while(elt>=COUNTRY) {
		int ectr = elt;
		pElt = new CStatSimElt(m_pToDB, elt, TRUE);

		if ( elt > (m_hlimit+1) ) {
			for (int i=0; i<n; i++) {
				sSQL.Format(_T("SELECT * FROM `%s` WHERE LEFT(`%s`, %s)=LEFT('%s', %s);"), 
					(CString) pElt->Attr(element), (CString) pElt->Attr(IDField), (CString) pBaseElt->ChiAttr(digit), (CString) sCritVals[i], (CString) pBaseElt->ChiAttr(digit));
				pRS = new CStatSimRS(m_pSrcDB, sSQL);
				m_pToDB->CreateTable(pRS, (CString) pElt->Attr(element));
				m_pToDB->InsertRecords(pRS, (CString) pElt->Attr(element), NULL, NULL, 
					FALSE, FALSE, TRUE);
				delete pRS; pRS = 0;
			}
		}
		else if (elt==m_hlimit+1) {
			pRS = new CStatSimRS(m_pSrcDB, pElt->Attr(element));
			m_pToDB->CreateTable(pRS, (CString) pElt->Attr(element));
			m_pToDB->InsertRecords(pRS, (CString) pElt->Attr(element), NULL, NULL, 
				FALSE, FALSE, TRUE,
				pElt->Attr(IDField), sCritVals, n);
			delete pRS; pRS = 0;
		}
		else if (elt==COUNTRY) {
			TransferRecords(m_pSrcDB, _MBCS("country"), m_pToDB);
		}
		else {
			int i;
			for (i=0; i<n; i++) {
				CString str(sCritVals[i]);
				int d = atoi(pElt->Attr(digit));
				str = str.Left(d);
				sCritVals[i] = ConstChar(ZeroFill(str, 9, FALSE));
			}
			vector<LPCSTR> uv = unique_array(sCritVals, n);
			LPCSTR *sUV = new LPCSTR[uv.size()];
			for (int i=0; i<uv.size(); i++) {
				sUV[i] = uv[i];
			}

			pRS = new CStatSimRS(m_pSrcDB, pElt->Attr(element));
			m_pToDB->CreateTable(pRS, (CString) pElt->Attr(element));
			m_pToDB->InsertRecords(pRS, (CString) pElt->Attr(element), NULL, NULL, 
				FALSE, FALSE, TRUE,
				pElt->Attr(IDField), sUV, uv.size());

			delete pRS; pRS = 0;
			delete [] sUV; sUV = 0;
		}

		pElt->ConfigTable();
		sSQL.Format(_T("ALTER TABLE `%s` ENGINE=MYISAM;"), (CString) pElt->Attr(element));
		m_pToDB->ExecuteSQL(sSQL);

		delete pElt; pElt = NULL;
		--ectr;
		elt = ectr;
	}

	OnOK();
}

void CConfigDlg::OnCbnSelchangeCombolevel()
{

	m_Ok.EnableWindow(FALSE);

	int iSel = m_ComboLevel.GetCurSel();
	DWORD iDta = m_ComboLevel.GetItemData(iSel);
	m_hlimit = iDta - 1; // temporarily for the parent
	
	if (!m_pElt)
		delete m_pElt; m_pElt = 0;
	
	m_pElt = new CStatSimElt(m_pSrcDB, (ELEMENT) m_hlimit, TRUE);

	 //assign column names
	if (!m_sListCols)		
		delete [] m_sListCols; m_sListCols = 0;

	m_sListCols = new LPCSTR[m_nListJ];
	m_sListCols[0] = m_pElt->ChiAttr(name);
	m_sListCols[1] = m_pElt->ChiAttr(IDField);

	m_TreeLevel->DeleteAllItems();


	if (iDta<=REGN) {
		m_TreeLevel->EnableWindow(FALSE);
		m_View.EnableWindow(FALSE);
		m_Change.EnableWindow(FALSE);
		m_ListSel.CleanUp(m_sListCols, m_nListJ, TRUE, EMPTY);
		m_ListUnsel.CleanUp(m_sListCols, m_nListJ, FALSE, EMPTY);
		CString sSQL = _T("SELECT `Region`, `regnID` FROM `regn`;");
		CStatSimRS* pRS = new CStatSimRS(m_pSrcDB, sSQL);
		m_ListUnsel.InsertBulk(pRS);

		delete pRS; pRS = 0;

	}
	else {
		m_TreeLevel->EnableWindow();
		m_View.EnableWindow();
		m_Change.EnableWindow(FALSE);
		m_ListSel.CleanUp(m_sListCols, m_nListJ, FALSE, EMPTY);
		m_ListUnsel.CleanUp(m_sListCols, m_nListJ, FALSE, EMPTY);

		//Initialize the tree control
		m_TreeLevel->Initialize(m_pSrcDB, _T("CBMS Database (C:\\CBMSDatabase\\) - 608"), m_hlimit);
		//expands the tree control
		m_TreeLevel->Expand(m_TreeLevel->GetRootItem(), TVE_EXPAND);

	}

}

void CConfigDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CConfigDlg::OnBnClickedView()
{

	ULONG nItems = 0;
	int k = m_hlimit-(1001), // prov=3; minus 1; index is 2
		d = atol(m_pElt->Attr(digit));
	vector<LPCSTR> *sItems = new vector<LPCSTR>[2]; //Name, ID
	ULONG *hSizeArray = m_TreeLevel->GetSizeArray();
	HTREEITEM **phItemArray = m_TreeLevel->GetItemArray();

	for (int i=0; i<hSizeArray[k]; i++) {
		
		CStatSimRS* pRS = 0;
		CString sSQL, sCrit;
		UINT nStatus = m_TreeLevel->GetItemState( phItemArray[k][i], TVIS_STATEIMAGEMASK ) >> 12;
		sCrit = m_TreeLevel->GetItemText(phItemArray[k][i]);
		sCrit = sCrit.Right(9); //still default at 9 digits
		sCrit = sCrit.Left( d );

		if (nStatus == CStatSimTree::CHECKED) {
			sSQL.Format( _T("SELECT * FROM `%s` WHERE LEFT(`%s`, %d)='%s';"), (CString) m_pElt->ChiAttr(element), (CString) m_pElt->ChiAttr(IDField), d, sCrit );
			pRS = new CStatSimRS(m_pSrcDB, sSQL);
			if (pRS->GetRecordCount()>0) {
				pRS->MoveFirst();
				for (int j=0; j<pRS->GetRecordCount(); j++) {
					sItems[0].push_back( ConstChar(pRS->SQLFldValue(m_pElt->ChiAttr(name))) );
					sItems[1].push_back( ConstChar(pRS->SQLFldValue(m_pElt->ChiAttr(IDField))) );
					nItems++;	//increment number of list items
					pRS->MoveNext();
				}
			}
			delete pRS; pRS = 0;
		}
	}

	if (nItems > 0) {
		m_nListI = sItems[0].size();		
		
		LPCSTR** sArray = vector_to_array(sItems, m_nListJ);
		//m_ListUnsel.CleanUp(m_sListCols, m_nListJ, FALSE, EMPTY);
		m_ListUnsel.InsertBulk(m_sListCols, 2, sArray, m_nListI, TRUE, EMPTY);
		m_ListSel.CleanUp(m_sListCols, m_nListJ, TRUE, EMPTY);
		
		m_Change.EnableWindow();
		m_View.EnableWindow(FALSE);
		m_ComboLevel.EnableWindow(FALSE);
		m_TreeLevel->EnableWindow(FALSE);

	}
	else {

		m_nListI = 0;
		m_ListUnsel.CleanUp(m_sListCols, m_nListJ, FALSE, EMPTY);
		m_ListSel.CleanUp(m_sListCols, m_nListJ, FALSE, EMPTY);

		m_Change.EnableWindow(FALSE);
		m_View.EnableWindow();
		m_ComboLevel.EnableWindow();
		m_TreeLevel->EnableWindow();

		AfxMessageBox(_T("Nothing is selected."));

	}

	delete [] sItems; sItems = 0;
}

void CConfigDlg::OnBnClickedDeselect()
{
	m_ListSel.SwitchContents(&m_ListUnsel);
	if (m_ListSel.GetItemCount() > 0) {
		m_Ok.EnableWindow(TRUE);
		m_Ok.SetFocus();
	}
	else {
		m_Ok.EnableWindow(FALSE);
	}
}

void CConfigDlg::OnBnClickedSelect()
{
	m_ListUnsel.SwitchContents(&m_ListSel);
	if (m_ListSel.GetItemCount() > 0) {
		m_Ok.EnableWindow(TRUE);
		m_Ok.SetFocus();
	}
	else {
		m_Ok.EnableWindow(FALSE);
	}
}

void CConfigDlg::OnBnClickedSelectall()
{
	m_ListUnsel.TransferContents(&m_ListSel);
	
	if (m_ListSel.GetItemCount() > 0) {
		m_Ok.EnableWindow(TRUE);
		m_Ok.SetFocus();
	}
	else {
		m_Ok.EnableWindow(FALSE);
	}
}

void CConfigDlg::OnBnClickedDeselectall()
{
	m_ListSel.TransferContents(&m_ListUnsel);
	
	if (m_ListSel.GetItemCount() > 0) {
		m_Ok.EnableWindow(TRUE);
		m_Ok.SetFocus();
	}
	else {
		m_Ok.EnableWindow(FALSE);
	}
}
void CConfigDlg::OnBnClickedChange()
{

	m_TreeLevel->EnableWindow();
	m_ComboLevel.EnableWindow();
	m_ListSel.CleanUp(m_sListCols, m_nListJ, FALSE, EMPTY);
	m_ListUnsel.CleanUp(m_sListCols, m_nListJ, FALSE, EMPTY);

	m_View.EnableWindow();
	m_Change.EnableWindow(FALSE);

}

void CConfigDlg::OnNMDblclkListunsel(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnBnClickedSelect();
	
	*pResult = 0;
}

void CConfigDlg::OnNMDblclkListsel(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnBnClickedDeselect();

	*pResult = 0;
}
