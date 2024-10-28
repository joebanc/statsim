// EditThreshDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StatSimPro.h"
#include "EditThreshDlg.h"
#include "StatSimProView.h"

extern CStatSimConn* pGlobalConn;
extern CMFCRibbonStatusBarPane  *g_wndStatusBarPane;//I have transferred this from main to global to be accessible
extern CProgressCtrl  m_ProgStatBar;	//I have transferred this from main to global to be accessible

extern CStatSimProView *g_pSSHTMLView;

extern int* eArray;	//the selected processed elements or geolevel
extern int nE;			//number of elements
extern BOOL wHH;

IMPLEMENT_DYNAMIC(CEditThreshDlg, CDialog)
/////////////////////////////////////////////////////////////////////////////
// CEditThreshDlg dialog


CEditThreshDlg::CEditThreshDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditThreshDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditThreshDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEditThreshDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditThreshDlg)
	DDX_Control(pDX, IDC_COMBO_MUNICIPALITY, m_ComboMunicipality);
	DDX_Control(pDX, IDC_COMBO_URBANITY, m_ComboUrbanity);
	DDX_Control(pDX, IDC_EDIT_SUBTHRESH, m_EditSubThresh);
	DDX_Control(pDX, IDC_EDIT_POVTHRESH, m_EditPovThresh);
	DDX_Control(pDX, IDC_COMBO_YEAR, m_ComboYear);
	DDX_Control(pDX, IDC_COMBO_PROVINCE, m_ComboProvince);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditThreshDlg, CDialog)
	//{{AFX_MSG_MAP(CEditThreshDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_PROVINCE, OnSelchangeComboProvince)
	ON_CBN_SELCHANGE(IDC_COMBO_YEAR, OnSelchangeComboYear)
	ON_CBN_SELCHANGE(IDC_COMBO_URBANITY, OnSelchangeComboUrbanity)
	ON_CBN_SELCHANGE(IDC_COMBO_MUNICIPALITY, OnSelchangeComboMunicipality)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditThreshDlg message handlers

BOOL CEditThreshDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();


	CStatSimRS * pRS;	
	CString sSQL;
	
	sSQL = "SELECT * FROM `prov`;";
	pRS = new CStatSimRS(pGlobalConn, sSQL);
	pRS->PutValuesTo(&m_ComboProvince, _MBCS("Province"), _MBCS("prov"), _MBCS("Province"));

	sSQL = "SELECT `year` AS `yearID`, `year` as `year` FROM `~thresh` GROUP BY `year`;";
	pRS = new CStatSimRS(pGlobalConn, sSQL);
	pRS->PutValuesTo(&m_ComboYear, _MBCS("year"), _MBCS("yearID"), _MBCS("year"));

	int i;
	i = m_ComboUrbanity.AddString(_T("Rural"));
	m_ComboUrbanity.SetItemData(i, 1);
	i = m_ComboUrbanity.AddString(_T("Urban"));
	m_ComboUrbanity.SetItemData(i, 2);

	delete pRS; pRS = 0;


	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditThreshDlg::OnSelchangeComboProvince() 
{

	CStatSimRS * pRS;	
	CString sSQL;

	int index = m_ComboProvince.GetCurSel();
	long id = m_ComboProvince.GetItemData(index);
	

	sSQL.Format(_T("SELECT * FROM `mun` WHERE `prov`=%d;"), id);

	//AfxMessageBox(sSQL);


	pRS = new CStatSimRS(pGlobalConn, sSQL);
	pRS->PutValuesTo(&m_ComboMunicipality, _MBCS("Municipality"), _MBCS("mun"), _MBCS("Municipality"));

	sSQL = "SELECT `year` AS `yearID`, `year` as `year` FROM `~thresh` GROUP BY `year`;";
	pRS = new CStatSimRS(pGlobalConn, sSQL);
	pRS->PutValuesTo(&m_ComboYear, _MBCS("year"), _MBCS("yearID"), _MBCS("year"));

	//update display
	int u, y, p, m;
	
	u = m_ComboUrbanity.GetCurSel();
	y = m_ComboYear.GetCurSel();
	p = m_ComboProvince.GetCurSel();
	m = m_ComboMunicipality.GetCurSel();

	if (u!=CB_ERR && y!=CB_ERR && p!=CB_ERR && m!=CB_ERR)
	{

		UpdateDisplay();

	}
	else
	{
			m_EditPovThresh.SetWindowText(_T(""));
			m_EditSubThresh.SetWindowText(_T(""));;
	}
	
	delete pRS; pRS = 0;	
}

void CEditThreshDlg::OnSelchangeComboYear() 
{

	m_ComboUrbanity.EnableWindow();
	
	//update display
	int u, y, p, m;
	
	u = m_ComboUrbanity.GetCurSel();
	y = m_ComboYear.GetCurSel();
	p = m_ComboProvince.GetCurSel();
	m = m_ComboMunicipality.GetCurSel();

	if (u!=CB_ERR && y!=CB_ERR && p!=CB_ERR && m!=CB_ERR)
	{

		UpdateDisplay();

	}
	else
	{
			m_EditPovThresh.SetWindowText(_T(""));
			m_EditSubThresh.SetWindowText(_T(""));;
	}

	
}
void CEditThreshDlg::UpdateDisplay()
{
	CStatSimRS * pRS;	
	CString sSQL;
	LPCSTR povthresh, subthresh;
	int i;
	
	i = m_ComboUrbanity.GetCurSel();
	m_idUrb = m_ComboUrbanity.GetItemData(i);

	i = m_ComboYear.GetCurSel();
	m_idYear = m_ComboYear.GetItemData(i);

	i = m_ComboProvince.GetCurSel();
	m_idProv = m_ComboProvince.GetItemData(i);

	i = m_ComboMunicipality.GetCurSel();
	m_idMun = m_ComboMunicipality.GetItemData(i);

	if (m_idUrb==1) {
		sSQL.Format(_T("SELECT `povthreshr`, `subthreshr` FROM `~thresh` WHERE `year`=%d AND `threshprov`=%d AND `threshmun`=%d;"), m_idYear, m_idProv, m_idMun);
		m_spovthrfld = "povthreshr";
		m_ssubthrfld = "subthreshr";
	}
	else {
		sSQL.Format(_T("SELECT `povthreshu`, `subthreshu` FROM `~thresh` WHERE `year`=%d AND `threshprov`=%d AND `threshmun`=%d;"), m_idYear, m_idProv, m_idMun);
		m_spovthrfld = "povthreshu";
		m_ssubthrfld = "subthreshu";
	}
	
	pRS = new CStatSimRS(pGlobalConn, sSQL);

	pRS->MoveFirst();

	m_EditSubThresh.EnableWindow();
	m_EditPovThresh.EnableWindow();

	povthresh = ConstChar( pRS->SQLFldValue( ConstChar(m_spovthrfld) ) );
	subthresh = ConstChar( pRS->SQLFldValue( ConstChar(m_ssubthrfld) ) );

	m_PovThresh0 = atof(povthresh);
	m_SubThresh0 = atof(subthresh);

	m_EditPovThresh.SetWindowText( (CString) povthresh );
	m_EditSubThresh.SetWindowText( (CString) subthresh );

}

void CEditThreshDlg::OnSelchangeComboUrbanity() 
{
	//update display
	int u, y, p, m;
	
	u = m_ComboUrbanity.GetCurSel();
	y = m_ComboYear.GetCurSel();
	p = m_ComboProvince.GetCurSel();
	m = m_ComboMunicipality.GetCurSel();

	if (u!=CB_ERR && y!=CB_ERR && p!=CB_ERR && m!=CB_ERR)
	{

		UpdateDisplay();

	}
	else
	{
			m_EditPovThresh.SetWindowText(_T(""));
			m_EditSubThresh.SetWindowText(_T(""));;
	}
}

void CEditThreshDlg::OnOK() 
{
	CString povthresh, subthresh, msgText, sSQL;
	int msgResult;
	BOOL prompt;

	m_EditPovThresh.GetWindowText( povthresh );
	m_EditSubThresh.GetWindowText( subthresh );

	m_PovThresh1 = _ttof( ( povthresh ) );
	m_SubThresh1 = _ttof( ( subthresh ) );


	if (m_PovThresh0!=m_PovThresh1) {
		if (m_SubThresh0!=m_SubThresh1) {
			msgText.Format(_T("Are you sure you want change poverty threshold from %.0f to %.0f and the subsistence threshold from %.0f to %.0f?"), 
				m_PovThresh0, m_PovThresh1, m_SubThresh0, m_SubThresh1);

			prompt = TRUE;
		}
		else {
			msgText.Format(_T("Are you sure you want change poverty threshold from %.0f to %.0f?"), 
				m_PovThresh0, m_PovThresh1);
			prompt = TRUE;
		}
	}
	else {
		if (m_SubThresh0!=m_SubThresh1) {
			msgText.Format(_T("Are you sure you want change subsistence threshold from %.0f to %.0f?"), 
				m_SubThresh0, m_SubThresh1);
			prompt = TRUE;
		}
		else {
			prompt = FALSE;
			msgText.Format(_T("No change detected."));
		}
	}


	if (prompt) {
		msgResult = AfxMessageBox (msgText, MB_YESNO);
		
		sSQL.Format(_T("UPDATE `~thresh` SET `%s`=%.0f, `%s`=%.0f WHERE `year`=%d AND `threshprov`=%d AND `threshmun`=%d;"), 
				m_spovthrfld, m_PovThresh1, m_ssubthrfld, m_SubThresh1, m_idYear, m_idProv, m_idMun); 

		if (msgResult==IDYES) {
			pGlobalConn->ExecuteSQL(sSQL);
		
			CDialog::OnOK();

			AfxMessageBox(_T("You must update the indicators due to change in thresholds either by importing data or clicking 'Process and update' in the 'Data' menu."));

			/*
			CStatSimElt* pElt;
			
			m_wndStatusBar.SetPaneText(0, "Generating basic indicators...");
			pElt = new CStatSimElt(pGlobalConn, HH, TRUE);
			pElt->CreateTable();		//table of households
			pElt->UpdateTable();		//update table of households
			if ( !pElt->ProcInd() ) {
				return;			//indicator of households
			}

			CString statusText;
			
			ELEMENT elt; 	
			for (int i=0; i<nE; i++) {
				elt = eArray[i];
				
				pElt = new CStatSimElt(pGlobalConn, elt, TRUE);
				
				if (elt!=HH)
					pElt->CrDemog();
				

				statusText.Format("Updating '%s' CBMS Core Indicators...", (CString) pElt->Attr(label));
				m_wndStatusBar.SetPaneText(0, statusText);
				pElt->CrCoreInd(TRUE, &m_wndStatusBar, &m_ProgStatBar);
				
				//composite
				if (wHH) {
					pElt->CrCCI();
				
				}
			}

			delete pElt; pElt = NULL;
			*/
		}
	}

	else {
		AfxMessageBox (msgText);
		CDialog::OnOK();
	}
	
}

void CEditThreshDlg::OnSelchangeComboMunicipality() 
{
	m_ComboYear.EnableWindow();	

	int u, y, p, m;
	
	u = m_ComboUrbanity.GetCurSel();
	y = m_ComboYear.GetCurSel();
	p = m_ComboProvince.GetCurSel();
	m = m_ComboMunicipality.GetCurSel();

	if (u!=CB_ERR && y!=CB_ERR && p!=CB_ERR && m!=CB_ERR)
	{

		UpdateDisplay();

	}
	else
	{
			m_EditPovThresh.SetWindowText(_T(""));
			m_EditSubThresh.SetWindowText(_T(""));;
	}

}
