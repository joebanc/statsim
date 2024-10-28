// EditLevelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StatSimPro.h"
#include "EditLevelDlg.h"


extern CStatSimConn* pGlobalConn;

extern CMFCRibbonStatusBarPane *g_wndStatusBarPane;
extern CProgressCtrl  m_ProgStatBar;

// CEditLevelDlg dialog

IMPLEMENT_DYNAMIC(CEditLevelDlg, CDialog)

CEditLevelDlg::CEditLevelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditLevelDlg::IDD, pParent)
{
	pComboLevel = NULL;
	plevelRS = NULL; peditRS = NULL;

	//pNavItmBldr = NULL;
	//pNavColBldr = NULL;
	//pEdtItmBldr = NULL;
	//pEdtColBldr = NULL;
	pnavElt = NULL;
	pedtElt = NULL;
	pEditCtrl = NULL;
	m_NavColArray = new LPCSTR[2];
	m_EdtColArray = new LPCSTR[2];

	//filler
	sFill = new LPCSTR*[1];;
	sFill[0] = new LPCSTR[2];
	sFill[0][0] = _MBCS("..."); sFill[0][1] = _MBCS("...");

}

CEditLevelDlg::~CEditLevelDlg()
{

	delete [] m_NavColArray;
	m_NavColArray = NULL;
	delete [] m_EdtColArray;
	m_EdtColArray = NULL;
	delete [] sFill[0];
	delete [] sFill[1];
	delete [] sFill; sFill = NULL;
}

void CEditLevelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTLEVEL, m_ListLevel);
	DDX_Control(pDX, IDC_LISTLEVELNAV, m_ListLevelNav);
	DDX_Control(pDX, IDC_TEXTSELECT, m_TextSelect);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDOK, m_Ok);
}


BEGIN_MESSAGE_MAP(CEditLevelDlg, CDialog)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTLEVELNAV, &CEditLevelDlg::OnNMDblclkListlevelnav)
//	ON_NOTIFY(NM_CLICK, IDC_LISTLEVELNAV, &CEditLevelDlg::OnNMClickListlevelnav)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTLEVEL, &CEditLevelDlg::OnNMDblclkListlevel)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LISTLEVEL, &CEditLevelDlg::OnLvnEndlabeleditListlevel)
	ON_BN_CLICKED(IDOK, &CEditLevelDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_EDIT, &CEditLevelDlg::OnBnClickedEdit)
	ON_NOTIFY(NM_RCLICK, IDC_LISTLEVEL, &CEditLevelDlg::OnNMRclickListlevel)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_LISTLEVEL, &CEditLevelDlg::OnLvnBeginlabeleditListlevel)
	ON_COMMAND(ID_LEVEL_EDIT, OnLevelEdit)
	ON_COMMAND(ID_LEVEL_DELETE, OnLevelDelete)
	ON_NOTIFY(NM_CLICK, IDC_LISTLEVELNAV, &CEditLevelDlg::OnNMClickListlevelnav)
END_MESSAGE_MAP()


// CEditLevelDlg message handlers
void CEditLevelDlg::PopulateNav(BOOL wroot, BOOL navend, 
								LPCSTR navFld, LPCSTR navVal) 
{
	CString critFld, critVal; //for nav

	//check if there is something to do
	if (navend)
		return;	

	//constructor for the attributes of nav element
	pnavElt = new CStatSimElt(pGlobalConn, navElt, TRUE);

	sNavElt = pnavElt->Attr(element);
	//m_NavColArray.resize(2);
	//m_NavColArray.at(0) = pnavElt->Attr(name);
	//m_NavColArray.at(1) = pnavElt->Attr(IDField);
	m_NavColArray[0] = pnavElt->Attr(name);
	m_NavColArray[1] = pnavElt->Attr(IDField);

	LPCSTR* sNavCols = m_NavColArray;

	if (sNavCols==NULL) {
		AfxMessageBox(_T("Empty."));
		return;
	}

	plevelRS = new CStatSimRS( pGlobalConn );

	//divert edit value if there is a digit
	if (crdigit>0) {
		critFld.Format(_T("LEFT(%s, %d)"), (CString) sNavCols[1], crdigit);
		navFld = ConstChar(critFld);
	}

	else {
		navFld = _MBCS("");
		navVal = _MBCS("");
	}

	/////////////////////////error/////////////////////////////////////////////////////////////
	LPCSTR** mNavItemArr = *plevelRS->GetRSArray(sNavElt, (CString) navFld, (CString) navVal, 
		(CString) sNavCols[0], (CString) sNavCols[1], (CString) sNavCols[1], wroot, TRUE, sFill);
	/////////////////////////error/////////////////////////////////////////////////////////////

	if (mNavItemArr==NULL) {
		AfxMessageBox(_T("Empty."));
		return;
	}

	nNavItem = plevelRS->GetRecordCount();

	m_ListLevelNav.InsertBulk(sNavCols, nNavAttr, mNavItemArr, nNavItem + wroot, TRUE, EMPTY);
	
	delete pnavElt; pnavElt = NULL;
	delete plevelRS; plevelRS = NULL;

}

void CEditLevelDlg::PopulateEdt(LPCSTR edtFld, LPCSTR edtVal) 
{
	CString critFld, critVal; //for edit - purok

	//constructor for the attributes of edit element
	pedtElt = new CStatSimElt(pGlobalConn, edtElt, TRUE);

	if ( pedtElt->wAttr!=TRUE ) {
		return;
	}

	CString buff; buff.Format(_T("%s"), (CString) pedtElt->Attr(name));
	sEdtElt = pedtElt->Attr(element);
	//m_EdtColArray.push_back(ConstChar(buff)); //0
	//m_EdtColArray.push_back(pedtElt->Attr(IDField)); //1
	m_EdtColArray[0] = ConstChar(buff);
	m_EdtColArray[1] = pedtElt->Attr(IDField);

	LPCSTR* sEditCols = m_EdtColArray;

	peditRS = new CStatSimRS( pGlobalConn);

	//divert edit value if there is a digit
	if (crdigit>0) {
		critFld.Format(_T("LEFT(%s, %d)"), (CString) sEditCols[1], crdigit);
		edtFld = ConstChar(critFld);
	}

	else {
		edtFld = _MBCS("");
		edtVal = _MBCS("");
	}

	LPCSTR** mEditItemArr = *peditRS->GetRSArray(sEdtElt, (CString) edtFld, (CString) edtVal, 
		(CString) sEditCols[0], (CString) sEditCols[1], (CString) sEditCols[1], 0, TRUE);

	//edit member variables global value
	sEdtEltLB = sEditCols[0];
	sEdtEltIDFld = sEditCols[1];
	///////////////////////////////////

	nEditItem = peditRS->GetRecordCount();

	m_ListLevel.InsertBulk(sEditCols, nEditAttr, mEditItemArr, nEditItem, TRUE, EMPTY);

	delete pedtElt; pedtElt = NULL;
	delete peditRS; peditRS = NULL;	


}
void CEditLevelDlg::Populate(LPCSTR* selArray, BOOL wNav, BOOL pass)
{

	
	CString critFld, critVal, sCrit;

	if (selArray==NULL) {
		AfxMessageBox(_T("Please click on the names."));
		return;
	}

	sCrit.Format(_T("%s"), (CString) selArray[0]);

	sFill[0][1] = selArray[1];	//update filler

	if (sCrit==_T("...")) {
	
		UpdateSelLables(selArray[0], UP);
		
		critVal.Format(_T("%s"), (CString) sFill[0][1]);		

		if (!pass) {
			LPCSTR sCrDigit = ConstChar( critVal.Left(crdigit) );
			critVal.Format(_T("%d"), atoi(sCrDigit));
			PopulateEdt( _MBCS(""), ConstChar(critVal) );
		}
		
		else {
			//pass depending on dir
			Pass(navElt, UP);
			LPCSTR sCrDigit = ConstChar( critVal.Left(crdigit) );
			critVal.Format(_T("%d"), atoi(sCrDigit));
			PopulateEdt( _MBCS(""), ConstChar(critVal) );
		}

		if (wNav)
			PopulateNav(WRoot(navElt), NavEnd(navElt), _MBCS(""), ConstChar(critVal) );
		
	}
	
	else {

		UpdateSelLables(selArray[0], DOWN);
		
		if (NavEnd(navElt)) 
			return;
		
		//pass depending on dir
		Pass(navElt, DOWN);

		critVal.Format(_T("%s"), (CString) selArray[1]);		
		LPCSTR sCrDigit = ConstChar( critVal.Left(crdigit) );
		critVal.Format(_T("%d"), atoi(sCrDigit));
		
		PopulateEdt( _MBCS(""), ConstChar(critVal) );

		if (wNav) {
			PopulateNav(WRoot(navElt), NavEnd(navElt), _MBCS(""), ConstChar(critVal));
		}
		
		if (!pass){
			Pass(navElt, UP);

		}
	}

}

void CEditLevelDlg::Pass(ELEMENT currElt, UD dir)
{
	switch (dir) {
	
	case UP:
		//pass elements up
		chiElt = navElt;	//current element as the child
		navElt = parElt;	//parent as the current element

		//get parent
		pnavElt = new CStatSimElt(pGlobalConn, navElt, TRUE);
		parElt = pnavElt->GetParent();
		
		//get critical digit
		crdigit = pnavElt->CrDigit();
		
		delete pnavElt; pnavElt = NULL;
		break;

	case DOWN:
		//pass elements down
		parElt = navElt;	//current element as the parent
		navElt = chiElt;	//child as the current element

		//get child
		pnavElt = new CStatSimElt(pGlobalConn, navElt, TRUE);
		chiElt = pnavElt->GetChild();
		
		//get critical digit
		crdigit = pnavElt->CrDigit();
		
		delete pnavElt; pnavElt = NULL;

		break;

	default:

		AfxMessageBox(_T("Unknown direction."));
		break;
	}

}
BOOL CEditLevelDlg::WRoot(ELEMENT currElt) 
{

	if (currElt==REGN)
		return FALSE;
	else
		return TRUE;

	
}
BOOL CEditLevelDlg::NavEnd(ELEMENT currElt) 
{

	if (currElt==PUROK)
		return TRUE;
	else
		return FALSE;

	
}
void CEditLevelDlg::UpdateSelLables(LPCSTR sLabel, UD dir)
{
	CString sText; 

	switch (dir) {
	
	case DOWN:
		
		switch (navElt) {
		
		case REGN:
			
			Region = sLabel;
			Province = _MBCS("ALL");
			Municipality = _MBCS("ALL");
			Barangay = _MBCS("ALL");
			
			sText.Format(_T("REGION:\t\t%s\r\nPROVINCE:\t%s\r\nMUNICIPALITY:\t%s\r\nBARANGAY:\t%s"), (CString) Region, (CString) Province, (CString) Municipality, (CString) Barangay);
			m_TextSelect.SetWindowText(sText);
			
			break;
		
		case PROV:
			
			//Region = _T("");
			Province = sLabel;
			Municipality = _MBCS("ALL");
			Barangay = _MBCS("ALL");
			
			sText.Format(_T("REGION:\t\t%s\r\nPROVINCE:\t%s\r\nMUNICIPALITY:\t%s\r\nBARANGAY:\t%s"), (CString) Region, (CString) Province, (CString) Municipality, (CString) Barangay);
			m_TextSelect.SetWindowText(sText);
			
			break;
		
		case MUN:
			
			//Region = _T("");
			//Province = _T("");
			Municipality = sLabel;
			Barangay = _MBCS("ALL");
			
			sText.Format(_T("REGION:\t\t%s\r\nPROVINCE:\t%s\r\nMUNICIPALITY:\t%s\r\nBARANGAY:\t%s"), (CString) Region, (CString) Province, (CString) Municipality, (CString) Barangay);
			m_TextSelect.SetWindowText(sText);
			
			break;
		
		case BRGY:
			
			//Region = _T("");
			//Province = _T("");
			//Municipality = _T("");
			Barangay = sLabel;
			
			sText.Format(_T("REGION:\t\t%s\r\nPROVINCE:\t%s\r\nMUNICIPALITY:\t%s\r\nBARANGAY:\t%s"), (CString) Region, (CString) Province, (CString) Municipality, (CString) Barangay);
			m_TextSelect.SetWindowText(sText);
			
			break;
			
		default:
			
			sText.Format(_T("REGION:\t\t%s\r\nPROVINCE:\t%s\r\nMUNICIPALITY:\t%s\r\nBARANGAY:\t%s"), (CString) Region, (CString) Province, (CString) Municipality, (CString) Barangay);
			m_TextSelect.SetWindowText(sText);
			break;
		}

		break;

	case UP:

		switch (navElt) {
		
		case REGN:
			
			Region = _MBCS("ALL");
			Province = _MBCS("ALL");
			Municipality = _MBCS("ALL");
			Barangay = _MBCS("ALL");
			
			sText.Format(_T("REGION:\t\t%s\r\nPROVINCE:\t%s\r\nMUNICIPALITY:\t%s\r\nBARANGAY:\t%s"), (CString) Region, (CString) Province, (CString) Municipality, (CString) Barangay);
			m_TextSelect.SetWindowText(sText);
			
			break;
		
		case PROV:
			
			//Region = _T("");
			Province = _MBCS("ALL");
			Municipality = _MBCS("ALL");
			Barangay = _MBCS("ALL");
			
			sText.Format(_T("REGION:\t\t%s\r\nPROVINCE:\t%s\r\nMUNICIPALITY:\t%s\r\nBARANGAY:\t%s"), (CString) Region, (CString) Province, (CString) Municipality, (CString) Barangay);
			m_TextSelect.SetWindowText(sText);
			
			break;
		
		case MUN:
			
			//Region = _T("");
			//Province = _T("");
			Municipality = _MBCS("ALL");
			Barangay = _MBCS("ALL");
			
			sText.Format(_T("REGION:\t\t%s\r\nPROVINCE:\t%s\r\nMUNICIPALITY:\t%s\r\nBARANGAY:\t%s"), (CString) Region, (CString) Province, (CString) Municipality, (CString) Barangay);
			m_TextSelect.SetWindowText(sText);
			
			break;
		
		case BRGY:
			
			//Region = _T("");
			//Province = _T("");
			//Municipality = _T("");
			Barangay = _MBCS("ALL");
			
			sText.Format(_T("REGION:\t\t%s\r\nPROVINCE:\t%s\r\nMUNICIPALITY:\t%s\r\nBARANGAY:\t%s"), (CString) Region, (CString) Province, (CString) Municipality, (CString) Barangay);
			m_TextSelect.SetWindowText(sText);
			
			break;
			
		default:
			
			sText.Format(_T("REGION:\t\t%s\r\nPROVINCE:\t%s\r\nMUNICIPALITY:\t%s\r\nBARANGAY:\t%s"), (CString) Region, (CString) Province, (CString) Municipality, (CString) Barangay);
			m_TextSelect.SetWindowText(sText);
			break;
		}
		break;

	default:

		sText.Format(_T("REGION:\t\t%s\r\nPROVINCE:\t%s\r\nMUNICIPALITY:\t%s\r\nBARANGAY:\t%s"), (CString) Region, (CString) Province, (CString) Municipality, (CString) Barangay);
		m_TextSelect.SetWindowText(sText);
		break;
	}

}

void CEditLevelDlg::EditLevelLabel() 
{
	int iSel = m_ListLevel.GetSelectedItem();
	
	if (iSel>=0) {

		// Make sure the focus is set to the list view control.
		m_ListLevel.SetFocus();
		pEditCtrl = m_ListLevel.EditLabel(iSel);
		//pEditCtrl = m_ListLevel.GetEditControl();
		selIndex = iSel;
		
		int getLineRes, lineCount, editLen;	
		CString editBuff;

		if (pEditCtrl!=NULL) {		
			editLen = pEditCtrl->LineLength(1);
			getLineRes = pEditCtrl->GetLine(selIndex, editBuff.GetBuffer(editLen), editLen);
			lineCount = pEditCtrl->GetLineCount();
			editBuff.ReleaseBuffer(editLen);
			m_sLName0 = ConstChar(editBuff);
		}
	}
	
}
void CEditLevelDlg::UpdateAndChange() 
{
	int nRowCount =  m_ListLevel.GetItemCount();

	CString newLevelName, levelID, strSQL;

	for (int i=0;i < nRowCount;i++) {	
		newLevelName = m_ListLevel.GetItemText(i, 0);
		levelID = m_ListLevel.GetItemText(i, 1);

		strSQL = _T("UPDATE `") + sEdtElt + _T("` SET `") + sEdtEltLB + _T("`='") + newLevelName + _T("' WHERE `") + sEdtEltIDFld + _T("`='") + levelID + _T("';");
		pGlobalConn->ExecuteSQL(strSQL);

	}

}

BOOL CEditLevelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_hasChanged = FALSE;

	//initialize menu
	menu.LoadMenu(IDR_EDITLEVEL);
	pMenu = menu.GetSubMenu(0);


	m_ListLevelNav.SetImage(IDB_FGLOBE);
	m_ListLevel.SetImage(IDB_FGLOBE);

	//initialize
	navElt = REGN;	//region
	edtElt = PUROK;	//purok

	//constructor for the attributes of nav element
	pnavElt = new CStatSimElt(pGlobalConn, navElt, TRUE);
	parElt = pnavElt->GetParent();
	chiElt = pnavElt->GetChild();
	crdigit = pnavElt->CrDigit();
	delete pnavElt; pnavElt = NULL;

	nNavAttr = 2;
	nEditAttr = nNavAttr;

	//pNavColBldr = new CSSArrayBuilder(STR_ARR, 1);
	//pEdtColBldr = new CSSArrayBuilder(STR_ARR, 1);

	//pNavColBldr->SetDim(DIM1, nNavAttr);
	//pEdtColBldr->SetDim(DIM1, nEditAttr);

	Region = Province = Municipality = Barangay = _MBCS("ALL"); 
	regnID = provID = munID = _MBCS(""); 

	PopulateNav(WRoot(navElt), NavEnd(navElt));
	PopulateEdt();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEditLevelDlg::OnNMDblclkListlevelnav(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPCSTR* selArray = m_ListLevelNav.GetSelectedArray();

	Populate(selArray);
	
	*pResult = 0;
}

void CEditLevelDlg::OnNMDblclkListlevel(NMHDR *pNMHDR, LRESULT *pResult)
{
	EditLevelLabel();
	*pResult = 0;
}

void CEditLevelDlg::OnLvnEndlabeleditListlevel(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	int getLineRes, lineCount, editLen;

	CString editBuff;

	editLen = pEditCtrl->LineLength(1);
	getLineRes = pEditCtrl->GetLine(selIndex, editBuff.GetBuffer(editLen), editLen);
	lineCount = pEditCtrl->GetLineCount();
	editBuff.ReleaseBuffer(editLen);

	m_sLName1 = ConstChar(editBuff);

	if (editBuff == "") {
		AfxMessageBox(_T("Please enter a valid value."));
	}
	else {
		m_ListLevel.SetItemText(selIndex, 0, editBuff);
	}

	if ( strcmp(m_sLName0, m_sLName1)!=STR_EQUAL || m_hasChanged==TRUE) {
		//set change indicator to true
		m_hasChanged = TRUE;
	}

	*pResult = 0;
}

void CEditLevelDlg::OnBnClickedOk()
{
	UpdateAndChange();
	OnOK();
}

void CEditLevelDlg::OnBnClickedEdit()
{
	EditLevelLabel();	
}

void CEditLevelDlg::OnNMRclickListlevel(NMHDR *pNMHDR, LRESULT *pResult)
{
	// Get menu

	int iSel = m_ListLevel.GetSelectedItem();
	if (iSel<0) {
		*pResult = 0;
		return;
	}
	
	// Get the current mouse location and convert it to client
	// coordinates.
	DWORD pos = GetMessagePos();
	CPoint pt(LOWORD(pos), HIWORD(pos));

	//convert to screen coord
	//ScreenToClient(&pt);
	
	CRect rect;
	
	if (!m_ListLevel.GetItemRect(iSel, &rect, LVIR_BOUNDS)) {
		*pResult = 0;
		return;
	}
	
	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
	  pt.x, pt.y, this);
	

	*pResult = 0;
}

void CEditLevelDlg::OnLvnBeginlabeleditListlevel(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
void CEditLevelDlg::OnLevelEdit() 
{
	EditLevelLabel();
	
}
void CEditLevelDlg::OnLevelDelete() 
{
	//USES_CONVERSION;

	int iSel = m_ListLevel.GetSelectedItem();
	CString sID, sLevel, sSQL, msg;
	
	if (iSel>=0) {

		//get the ids
		sLevel = m_ListLevel.GetItemText(iSel, 0);
		sID = m_ListLevel.GetItemText(iSel, 1);

		msg.Format(_T("Do you really want to delete %s - %s?"), sID, sLevel);
		if ( AfxMessageBox(msg, MB_YESNO)==IDYES ) {
			sSQL = _T("DELETE FROM `") + sEdtElt + _T("` WHERE `") + sEdtEltIDFld + _T("`='") + sID + _T("';");
			pGlobalConn->ExecuteSQL(sSQL);
			UpdateAndChange();
		
			
			//refresh the array
			LPCSTR* selArray = m_ListLevelNav.GetSelectedArray();
			if (selArray==NULL) {
				AfxMessageBox(_T("Please click on the names."));		
				return;
			}
			
			CString critVal;
			LPCSTR sCrDigit = ConstChar( critVal.Left(crdigit) );
			critVal.Format(_T("%d"), atoi(sCrDigit));
			Populate(selArray, FALSE, FALSE);

		}
	}
	
}

void CEditLevelDlg::OnNMClickListlevelnav(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (m_hasChanged) {
		CString msg;
		msg.Format(_T("Do you want to save the changes first?"));
		if ( AfxMessageBox(msg, MB_YESNO)==IDYES ) {
			UpdateAndChange();
		}
		m_hasChanged = FALSE;
	}


	LPCSTR* selArray = m_ListLevelNav.GetSelectedArray();

	if (selArray==NULL) {
		AfxMessageBox(_T("Please click on the names."));
		return;
	}
	
	CString critVal;
	critVal.Format(_T("%d"), _ttoi(critVal.Left(crdigit)));

	Populate(selArray, FALSE, FALSE);
	*pResult = 0;
}
