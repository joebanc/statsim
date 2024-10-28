// RemoveAutoSelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StatSimPro.h"
#include "RemoveAutoSelDlg.h"
//#include "ProgressExDlg.h"
//#include "StatSimTree.h"

#include "StatSimWnd.h"

#include <comdef.h>
//#include "ProgressDlgBar.h"

#ifndef STATSIMTREE
	#include "StatSimTree.h"
#endif

#ifndef STATSIMELT
	#include "StatSimElt.h"
#endif

extern CStatSimConn* pGlobalConn;
extern CStatSimRS* pelemRS;
extern CString userName, userPwd, sHost, sPort, sDB;

extern CStatSimRS ***pdictRS;
extern daedict ***pDCT;
extern LPCTSTR** qnrtables;

extern int *ntypes, nqnr;
extern int *rtlen;
extern LPCTSTR** RTYPES;
extern long* qnrIDs;

extern CMFCRibbonStatusBarPane g_wndStatusBarPane;
extern CProgressCtrl  m_ProgStatBar;

extern DWORD hlimit = MUN, hLstLimit = BRGY;

extern BOOL IsDirectProc;	//whether processing is by batch or direct
extern int nProcBatches;
extern vector<BOOL> arrayProcExist;
extern vector<LPCTSTR> arrayBatchDB;
extern int hpq_id;
// CRemoveAutoSelDlg dialog


IMPLEMENT_DYNAMIC(CRemoveAutoSelDlg, CDialog)

CRemoveAutoSelDlg::CRemoveAutoSelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRemoveAutoSelDlg::IDD, pParent)
{

	m_pDlg = NULL;

	m_pElt = 0;

	m_nListI = 0, m_nListJ = 3; // name, ID
	m_sListCols = 0;


}

CRemoveAutoSelDlg::~CRemoveAutoSelDlg()
{	
	delete m_pElt; m_pElt = 0;
	delete [] m_sListCols; m_sListCols = 0;

}

void CRemoveAutoSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FRAMESELECT, m_FrameSelect);
	DDX_Control(pDX, IDC_TREELEVEL, (CWnd&) m_TreeLevel);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDC_DESELECT, m_ButtonDeselect);
	DDX_Control(pDX, IDC_DESELECTALL, m_ButtonDeselectAll);
	DDX_Control(pDX, IDC_SELECT, m_ButtonSelect);
	DDX_Control(pDX, IDC_SELECTALL, (CWnd&) m_ButtonSelectAll);
	DDX_Control(pDX, IDC_LISTUNSEL, (CWnd&) m_ListUnselect);
	DDX_Control(pDX, IDC_LISTSEL, (CWnd&) m_ListSelect);
	DDX_Control(pDX, IDC_SPECIFYPATH, m_ButtonSpecPath);
	DDX_Control(pDX, IDC_VIEW, m_View);
	DDX_Control(pDX, IDC_CHANGE, m_Change);
}


BEGIN_MESSAGE_MAP(CRemoveAutoSelDlg, CDialog)
	ON_BN_CLICKED(IDC_SPECIFYPATH, &CRemoveAutoSelDlg::OnBnClickedSpecifypath)
//	ON_NOTIFY(TVN_SELCHANGED, IDC_TREELEVEL, &CRemoveAutoSelDlg::OnTvnSelchangedTreelevel)
	ON_BN_CLICKED(IDC_SELECT, &CRemoveAutoSelDlg::OnBnClickedSelect)
	ON_BN_CLICKED(IDC_DESELECT, &CRemoveAutoSelDlg::OnBnClickedDeselect)
	ON_BN_CLICKED(IDC_SELECTALL, &CRemoveAutoSelDlg::OnBnClickedSelectall)
	ON_BN_CLICKED(IDC_DESELECTALL, &CRemoveAutoSelDlg::OnBnClickedDeselectall)
	ON_BN_CLICKED(IDOK, &CRemoveAutoSelDlg::OnBnClickedOk)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTUNSEL, &CRemoveAutoSelDlg::OnNMDblclkListunsel)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTSEL, &CRemoveAutoSelDlg::OnNMDblclkListsel)
	ON_BN_CLICKED(IDC_VIEW, &CRemoveAutoSelDlg::OnBnClickedView)
	ON_BN_CLICKED(IDC_CHANGE, &CRemoveAutoSelDlg::OnBnClickedChange)
	ON_NOTIFY(NM_DBLCLK, IDC_TREELEVEL, &CRemoveAutoSelDlg::OnNMDblclkTreelevel)
END_MESSAGE_MAP()


// CRemoveAutoSelDlg message handlers

void CRemoveAutoSelDlg::OnBnClickedSpecifypath()
{
	OnCancel();
}

void CRemoveAutoSelDlg::FillUnSel() 
{

	CStatSimRS* plistRS; 
	plistRS = new CStatSimRS( pGlobalConn );

	CString lstTable, lstcritFld, lstcritValue, 
		lstlabelFld, lstIDFld, lstattrFld, lstDigit;

	CString lSQL, strID, strItem;

	int nCols = 2;
	LPCTSTR* sCols = (LPCTSTR*) malloc(nCols * sizeof(LPCTSTR));

	DWORD hclimit;
	HTREEITEM hselItem = m_TreeLevel.GetSelectedItem();

	if (hselItem!=NULL) {
		hclimit = m_TreeLevel.GetItemData(hselItem);
		strItem = m_TreeLevel.GetItemText(hselItem);
		strID = strItem.Right(9);

		//for characteristics of current
		lSQL.Format(_T("SELECT * FROM `~hElement` WHERE parent=%d AND elementID<=%d"), 
			hclimit, hLstLimit );
		plistRS->RunSQL(lSQL);

		if(hclimit == hlimit) {
			UINT i = m_TreeLevel.hIndex(hclimit);

			m_FrameSelect.SetWindowText(strItem + ":");
			
			if (plistRS->GetRecordCount() > 0) {
				plistRS->MoveFirst();
				lstTable = plistRS->SQLFldValue(_T("element"));
				lstlabelFld = plistRS->SQLFldValue(_T("name"));
				lstIDFld = plistRS->SQLFldValue(_T("IDField"));
			}
			
			if (plistRS->IsOpen() && plistRS!=NULL) {
				plistRS->Close(); delete plistRS; plistRS = 0;
				plistRS = new CStatSimRS( pGlobalConn );
			}

			//for digits of parent
			lSQL.Format(_T("SELECT * FROM `~hElement` WHERE `elementID`=%d"), hclimit);		
			plistRS->RunSQL(lSQL);
			if (plistRS->GetRecordCount() > 0) {
				plistRS->MoveFirst();
				lstDigit = plistRS->SQLFldValue(_T("digit"));
			}
			
			if (_ttoi(lstDigit) > 0) {
				lstcritFld.Format(_T("Left(%s, %s)"), lstIDFld, lstDigit);
				lstcritValue.Format(_T("%d"), _ttol(strID.Left(_ttoi(ConstChar(lstDigit)))));
			}
		
			if (plistRS->IsOpen() && plistRS!=NULL) {
				plistRS->Close(); delete plistRS; plistRS = 0;
				plistRS = new CStatSimRS( pGlobalConn );
			}

			sCols[0] = ConstChar(lstlabelFld);
			sCols[1] = _T("Path");

			m_ListUnselect.CleanUp(sCols, nCols, TRUE);
			m_ButtonDeselectAll.EnableWindow();
			m_ButtonDeselect.EnableWindow();
			m_ButtonSelectAll.EnableWindow();
			m_ButtonSelect.EnableWindow();
			m_ListSelect.CleanUp(sCols, nCols, TRUE);

			plistRS->GetHierRS(lstTable, lstcritFld, lstcritValue,
				lstlabelFld, lstIDFld, lstTable + _T("Path"), 
				&m_ListUnselect);
		}

		else {
			m_ListUnselect.CleanUp(sCols, nCols, FALSE, FRESH);
			m_ButtonDeselectAll.EnableWindow(FALSE);
			m_ButtonDeselect.EnableWindow(FALSE);
			m_ButtonSelectAll.EnableWindow(FALSE);
			m_ButtonSelect.EnableWindow(FALSE);
			m_ListSelect.CleanUp(sCols, nCols, FALSE, FRESH);
			m_FrameSelect.SetWindowText(_T("Selection:"));
		}

	}

	if (plistRS->IsOpen() && plistRS!=NULL) {
		plistRS->Close(); delete plistRS; plistRS = 0;
	}

	delete sCols; sCols = 0;
	

}
int CRemoveAutoSelDlg::AutoImport(CStatSimList* pSrcList)
{
	try
	{
		CString currPath, currPath1, currPath2, currPath3, //path forms
			currLevel, statusText, msgText;
		int msgResult;

		int k = pSrcList->GetItemCount();
		
		if (!IsDirectProc) {
			nProcBatches = nProcBatches + k;	//increment number of batches
		}

		arrayProcExist.resize(k);
		arrayBatchDB.resize(k);

		for (int i=0;i < k;i++)
		{
			currPath1 = _T("C:") + pSrcList->GetItemText(i, 2);
			//currPath2 = currPath1.Left(35) + currPath1.Right(13);
			currPath2 = currPath1;
			currPath2.Replace(_T(".txt"), _T(".hpq"));
			currPath3 = currPath1.Left(35) + currPath1.Right(7);
			//priorities: .txt(9), .hpq, .txt(3)

			currLevel = pSrcList->GetItemText(i, 0);			
			statusText.Format(_T("Importing %s: %s"), currLevel, currPath);
			LPCTSTR sBatchDB;
			currLevel.Replace(_T(" "), _T(""));
			
			CFileFind FileFinder;
			currPath1.Trim();
			currPath2.Trim();
			currPath3.Trim();

			BOOL ffound = 1;

check:		if ( FileFinder.FindFile((LPCTSTR) currPath1) )
			{
				ffound = 1;
				currPath = currPath1;
			}
			else if ( FileFinder.FindFile((LPCTSTR) currPath2) )
			{
				ffound = 1;
				currPath = currPath2;
			}
			else if ( FileFinder.FindFile((LPCTSTR) currPath3) )
			{
				ffound = 1;
				currPath = currPath3;
			}
			else
			{
				ffound = 0;
				currPath = currPath1 + " or " + currPath2 + " or " + currPath3;
			}      
			
			if ( !ffound ) {
							
				if (!IsDirectProc) {
					arrayProcExist[i] = FALSE;	//add array of boolean
					sBatchDB = NULL;		
					arrayBatchDB[i] = (LPCTSTR) sBatchDB;
				}

				CString fstatusText;
				fstatusText = statusText + _T(" - (Not Found)");
				g_wndStatusBarPane.SetText( fstatusText);
				msgText.Format(_T("The file %s of %s cannot be found.  What would you like to do?"), currPath, currLevel);
				int msgResult = AfxMessageBox (msgText, MB_ABORTRETRYIGNORE|MB_ICONEXCLAMATION);
				
				if (msgResult==IDABORT) {
					return 1;
				}
				else if (msgResult==IDRETRY) {
					goto check;
				}
				else if (msgResult==IDIGNORE) {
					continue;
				}


			}
			else {	
				g_wndStatusBarPane.SetText( statusText);
				if (!IsDirectProc) {
					delete pGlobalConn; pGlobalConn=0;	//redirect global database
					arrayProcExist[i] = TRUE;	//add array of boolean
					currLevel.Replace(_T("."),_T(""));		//replace dots
					currLevel.Replace(_T(","),_T(""));		//replace commas
					sBatchDB = ConstChar(_T("StatSimConn_") + currLevel);
					arrayBatchDB[i] = (LPCTSTR) sBatchDB;
					// odbc - FALSE
					pGlobalConn = new CStatSimConn( userName, userPwd, sHost, sPort, arrayBatchDB[i], FALSE );
					ConfigDB(pGlobalConn);
				}
				//ImportData(currPath, m_qnrID);

			}
		}
		
		delete pGlobalConn; pGlobalConn=0;	//redirect global database to the global
		// odbc - FALSE
		pGlobalConn = new CStatSimConn( userName, userPwd, sHost, sPort, ConstChar(sDB), ODBC );

		msgText.Format(_T("Do you want to import another set of files?"));
		msgResult = AfxMessageBox (msgText, MB_YESNO);

		if (msgResult==IDYES)
			return 0;
		else
			return 1;

	}

	catch(_com_error &e) 
	{
		_bstr_t bstrSource (e.Source());
		_bstr_t bstrDescription (e.Description());
		TRACE ( "Exception thrown for classes generated by #import" );
		TRACE ( "\tCode = %08lx\n", e.Error ());
		TRACE ( "\tCode meaning = %s\n", e.ErrorMessage ());
		TRACE ( "\tSource = %s\n", (LPCTSTR) bstrSource);
		TRACE ( "\tDescription = %s\n", (LPCTSTR) bstrDescription);
		
		AfxMessageBox ((LPCTSTR) bstrDescription);
	}

}
/*
int CRemoveAutoSelDlg::AutoImport(CStatSimList* pSrcList)
{

	CString currPath, currLevel, statusText, msgText;
	vector<LPCTSTR> sPath, sDesc;
	int importcount = 0;
	
	int k = pSrcList->GetItemCount();
	
	if (!IsDirectProc) {
		nProcBatches = nProcBatches + k;	//increment number of batches
	}

	for (int i=0;i < k;i++)
	{
		currPath = _T("C:") + pSrcList->GetItemText(i, 2);
		sPath.push_back( ConstChar(currPath) );

		currLevel = pSrcList->GetItemText(i, 0);
		sDesc.push_back( ConstChar(currLevel) );


	}

	_hprogress *phprogress = new _hprogress;
	phprogress->m_sObj = sPath;
	phprogress->m_sDesc = sDesc;

	m_pDlg = new CProgressExDlg(phprogress);
	m_pDlg->Invalidate();
	m_pDlg->UpdateWindow();

	m_pDlg->m_FrameList.SetWindowText(_T("Importing..."));
	CString sCaption;
	m_pDlg->m_ProgressBar.SetRange32(0, 100);

	m_pDlg->m_Ok.ShowWindow(SW_HIDE);

	for (int i=0; i<k; i++)
	{
		sCaption.Format(_T("Importing %d of %d: %s"), i+1, k, 
			m_pDlg->m_ProgressList->GetItemText(i, 2));
		m_pDlg->SetWindowText(sCaption);
		float pct = ((float) (i+1)/(float) k) *100;
		m_pDlg->m_ProgressBar.SetPos( (int) pct );

		//CRect iRect;
		//pDlg->m_ProgressList->GetItemRect(i, &iRect, LVIR_LABEL);
		//pDlg->m_ProgressList->Scroll( CSize(0, iRect.bottom) );

		daedata* pDta = NULL;
		daestream *pSSS = NULL;
		
		CStatSimElt lElt(pGlobalConn, HH);
		CString strIDX = lElt.IdxClause();
		CString text,
			strPath = phprogress->m_sObj[i];

		CStatSimWnd *pLabel = new CStatSimWnd(m_pDlg->m_ProgressList->m_ProgressLabels[i]),
			*pListDesc = new CStatSimWnd(m_pDlg->m_ProgressList, i, 1);
		CStatSimBar* pProg = new CStatSimBar(m_pDlg->m_ProgressList->m_ProgressBars[i]);
		
		CFileFind FileFinder;
		if ( !FileFinder.FindFile(strPath) ) {
			pListDesc->SetText(_T("Not found"));
			continue;
		}

		importcount++;

		int idx;
		for (int i=0; i<nqnr; i++) {
			if (qnrIDs[i]==m_qnrID)
				idx=i;
		}

		pSSS = new CStatSimStream(ConstChar(strPath), RTYPES[idx], ntypes[idx], rtlen[idx],
			NULL, NULL, FALSE);

		LPCTSTR*** pDctArray = new LPCTSTR**[ntypes[idx]];
		UINT* nDctVars = new UINT[ntypes[idx]];

		for (int j=0; j<ntypes[idx]; j++) {
			pDctArray[j] = pDCT[idx][j]->GetArray();
			nDctVars[j] = pDCT[idx][j]->nVars();
		}

		pSSS->MakeArray(pDctArray, nDctVars, NULL, _T(""), 
			pLabel, pProg);
		
		pDta = new daedata(pSSS->GetArray(), pSSS->TypeCountArr(), pSSS->TotLines(),
			pDCT[idx], RTYPES[idx], ntypes[idx], 
			pLabel, pProg);

		pGlobalConn->InsertRecords(pDta, qnrtables[idx], RTYPES[idx], ntypes[idx], 
			pLabel, pProg, 1, strIDX);

		pListDesc->SetText(_T("Completed"));
		m_pDlg->m_ProgressList->m_ProgressLabels[i]->DestroyWindow();

		delete pSSS; pSSS = NULL;
		delete pDta; pDta = NULL;

	}
	m_pDlg->m_FrameList.SetWindowText(_T("Result:"));
	m_pDlg->m_Ok.ShowWindow(SW_SHOW);

	m_pDlg->m_ProgressList->DestroyProgressBars();
	m_pDlg->m_ProgressList->DeleteColumn(0);

	m_pDlg->SetWindowText(_T("Statsim process result"));

	return importcount;

}

*/

void CRemoveAutoSelDlg::OnBnClickedSelect()
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

void CRemoveAutoSelDlg::OnBnClickedDeselect()
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

void CRemoveAutoSelDlg::OnBnClickedSelectall()
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

void CRemoveAutoSelDlg::OnBnClickedDeselectall()
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

void CRemoveAutoSelDlg::OnBnClickedOk()
{
	CString msgText;
	int msgResult;
	CDialog::OnOK();

	BeginWaitCursor();
	int imported = AutoImport(&m_ListSelect);
	if ( imported ) {
		
		msgText.Format(_T("Do you want to start the processing?"));
		msgResult = AfxMessageBox (msgText, MB_YESNO);
		
		if (msgResult==IDYES) {
			ConUpElt(); //Configure and update
		}
		delete m_pDlg; m_pDlg = 0;

	}

	EndWaitCursor();
}

void CRemoveAutoSelDlg::OnNMDblclkListunsel(NMHDR *pNMHDR, LRESULT *pResult)
{

	OnBnClickedSelect();	
	*pResult = 0;
}

void CRemoveAutoSelDlg::OnNMDblclkListsel(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnBnClickedDeselect();
	*pResult = 0;
}

BOOL CRemoveAutoSelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//temporary since no handler for autoselection in BPQ

	//set the tree imagelist
	HICON *pIcons = new HICON[4];
	pIcons[0] = AfxGetApp()->LoadIcon(IDI_NOCHECK);
	pIcons[1] = AfxGetApp()->LoadIcon(IDI_UNCHECKED);
	pIcons[2] = AfxGetApp()->LoadIcon(IDI_CHECKED);
	pIcons[3] = AfxGetApp()->LoadIcon(IDI_PARTIALLY);
	m_TreeLevel.SetImageListEx(pIcons, 4, TVSIL_STATE);
	delete [] pIcons; pIcons = 0;

	//list unselected
	pIcons = new HICON[1];
	pIcons[0] = AfxGetApp()->LoadIcon(IDI_GLOBE);
	m_ListUnselect.SetImageListEx(pIcons, 1);
	delete [] pIcons; pIcons = 0;

	//list selected
	pIcons = new HICON[1];
	pIcons[0] = AfxGetApp()->LoadIcon(IDI_GLOBEOK);
	m_ListSelect.SetImageListEx(pIcons, 1);
	delete [] pIcons; pIcons = 0;
	//

	//set icons for buttons
	m_ButtonSelect.SetButtonStyle(BS_ICON);
	m_ButtonSelect.SetIcon(AfxGetApp()->LoadIcon(IDI_FWD));
	m_ButtonSelectAll.SetButtonStyle(BS_ICON);
	m_ButtonSelectAll.SetIcon(AfxGetApp()->LoadIcon(IDI_FFWD));
	m_ButtonDeselect.SetButtonStyle(BS_ICON);
	m_ButtonDeselect.SetIcon(AfxGetApp()->LoadIcon(IDI_REW));
	m_ButtonDeselectAll.SetButtonStyle(BS_ICON);
	m_ButtonDeselectAll.SetIcon(AfxGetApp()->LoadIcon(IDI_FREW));


	//Generating tree of levels
	m_TreeLevel.DeleteAllItems();
	m_ListSelect.DeleteAllItems();
	m_ListUnselect.DeleteAllItems();

	//initialize the element
	if (!m_pElt)
		delete m_pElt; 
	m_pElt = new CStatSimElt(pGlobalConn, (ELEMENT) hlimit, TRUE);

	 //assign column names
	if (!m_sListCols)		
		delete [] m_sListCols; m_sListCols = 0;

	m_sListCols = new LPCTSTR[m_nListJ];
	m_sListCols[0] = m_pElt->ChiAttr(name);
	m_sListCols[1] = m_pElt->ChiAttr(IDField);
	CString sElt = m_pElt->ChiAttr(element);	//temp for element name
	m_sListCols[2] = ConstChar(sElt + _T("path"));

	//assign to lists
	m_ListUnselect.CleanUp(m_sListCols, m_nListJ, FALSE, EMPTY);
	m_ListSelect.CleanUp(m_sListCols, m_nListJ, FALSE, EMPTY);

	m_TreeLevel.EnableWindow(TRUE);	
	m_OK.EnableWindow(FALSE);

	//initialize and populate
	m_ProgStatBar.ShowWindow(SW_SHOW);
	
	//CStatSimWnd* hLabel = new CStatSimWnd(&g_wndStatusBarPane);
	//CStatSimBar* pProg = new CStatSimBar(&m_ProgStatBar);
	//m_TreeLevel.Initialize(pGlobalConn, _T("CBMS Database (C:\\CBMSDatabase\\) - 608"), hlimit,
	//	hLabel, pProg);

	m_TreeLevel.Initialize(pGlobalConn, _T("CBMS Database (C:\\CBMSDatabase\\) - 608"), hlimit);
	m_ProgStatBar.ShowWindow(SW_HIDE);
	
	//g_wndStatusBarPane.SetWindowText(_T("Ready"));
	m_TreeLevel.Expand(m_TreeLevel.GetRootItem(), TVE_EXPAND);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CRemoveAutoSelDlg::OnBnClickedView()
{

	ULONG nItems = 0;
	int k = hlimit-(1001), // prov=1003; minus 1001; index is 2 //hay, this is bad (1000)
		d = _ttol(m_pElt->Attr(digit));
	vector<LPCTSTR> *sItems = new vector<LPCTSTR>[m_nListJ]; //Name, ID, path
	ULONG *hSizeArray = m_TreeLevel.GetSizeArray();
	HTREEITEM **phItemArray = m_TreeLevel.GetItemArray();

	for (int i=0; i<hSizeArray[k]; i++) { 
		
		CStatSimRS* pRS = 0;
		CString sSQL, sCrit;
		UINT nStatus = m_TreeLevel.GetItemState( phItemArray[k][i], TVIS_STATEIMAGEMASK ) >> 12;
		sCrit = m_TreeLevel.GetItemText(phItemArray[k][i]);
		sCrit = sCrit.Right(9); //still default at 9 digits
		sCrit = sCrit.Left( d );

		if (nStatus == CStatSimTree::CHECKED) {
			sSQL.Format( _T("SELECT * FROM `%s` WHERE LEFT(`%s`, %d)='%s';"), 
				m_pElt->ChiAttr(element), m_pElt->ChiAttr(IDField), d, sCrit );
			
			pRS = new CStatSimRS(pGlobalConn, sSQL);
			if (pRS->GetRecordCount()>0) {
				pRS->MoveFirst();
				for (int j=0; j<pRS->GetRecordCount(); j++) {
					sItems[0].push_back( ConstChar(pRS->SQLFldValue(m_pElt->ChiAttr(name))) );
					sItems[1].push_back( ConstChar(pRS->SQLFldValue(m_pElt->ChiAttr(IDField))) );
					sItems[2].push_back( ConstChar(pRS->SQLFldValue(m_sListCols[2])) );
					

					nItems++;	//increment number of list items
					pRS->MoveNext();
				}
			}
			delete pRS; pRS = 0;
		}
	}

	if (nItems > 0) {
		m_nListI = sItems[0].size();		
		
		LPCTSTR** sArray = vector_to_array(sItems, m_nListJ);
		//m_ListUnselect.CleanUp(m_sListCols, m_nListJ, FALSE, EMPTY);
		m_ListUnselect.InsertBulk(m_sListCols, m_nListJ, sArray, m_nListI, TRUE, EMPTY);
		m_ListSelect.CleanUp(m_sListCols, m_nListJ, TRUE, EMPTY);
		
		m_Change.EnableWindow();
		m_View.EnableWindow(FALSE);
		m_TreeLevel.EnableWindow(FALSE);

	}
	else {

		m_nListI = 0;
		m_ListUnselect.CleanUp(m_sListCols, m_nListJ, FALSE, EMPTY);
		m_ListSelect.CleanUp(m_sListCols, m_nListJ, FALSE, EMPTY);

		m_Change.EnableWindow(FALSE);
		m_View.EnableWindow();
		m_TreeLevel.EnableWindow();

		AfxMessageBox(_T("Nothing is selected."));

	}

	delete [] sItems; sItems = 0;


}


//void CRemoveAutoSelDlg::OnTvnSelchangedTreelevel(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
//		
//	FillUnSel();	
//	
//	*pResult = 0;
//}

void CRemoveAutoSelDlg::OnBnClickedChange()
{
	m_TreeLevel.EnableWindow();
	m_ListSelect.CleanUp(m_sListCols, m_nListJ, FALSE, EMPTY);
	m_ListUnselect.CleanUp(m_sListCols, m_nListJ, FALSE, EMPTY);

	m_View.EnableWindow();
	m_Change.EnableWindow(FALSE);
}

/*
int CRemoveAutoSelDlg::AutoImport(CStatSimList* pSrcList)
{
	try
	{
		CString currPath, currLevel, statusText, msgText;
		int msgResult;

		int k = pSrcList->GetItemCount();
		
		if (!IsDirectProc) {
			nProcBatches = nProcBatches + k;	//increment number of batches
		}

		for (int i=0;i < k;i++)
		{
			currPath = _T("C:") + pSrcList->GetItemText(i, 2);
			currLevel = pSrcList->GetItemText(i, 0);			
			statusText.Format(_T("Importing %s: %s"), currLevel, currPath);
			LPCTSTR sBatchDB;
			currLevel.Replace(_T(" "), _T(""));
			
			CFileFind FileFinder;


check:		if ( !FileFinder.FindFile((LPCTSTR) currPath) ) {
				
				
				if (!IsDirectProc) {
					arrayProcExist.InsertAt(i, FALSE);	//add array of boolean
					sBatchDB = NULL;		
					arrayBatchDB.InsertAt(i, (LPCTSTR) sBatchDB);
				}

				CString fstatusText;
				fstatusText = statusText + _T(" - (Not Found)");
				g_wndStatusBarPane.SetText( fstatusText);
				msgText.Format(_T("The file %s of %s cannot be found.  What would you like to do?"), currPath, currLevel);
				int msgResult = AfxMessageBox (msgText, MB_ABORTRETRYIGNORE|MB_ICONEXCLAMATION);
				
				if (msgResult==IDABORT) {
					return 1;
				}
				else if (msgResult==IDRETRY) {
					goto check;
				}
				else if (msgResult==IDIGNORE) {
					continue;
				}


			}
			else {	
				g_wndStatusBarPane.SetText( statusText);
				if (!IsDirectProc) {
					delete pGlobalConn; pGlobalConn=0;	//redirect global database
					arrayProcExist.InsertAt(i, TRUE);	//add array of boolean
					currLevel.Replace(_T("."),_T(""));		//replace dots
					currLevel.Replace(_T(","),_T(""));		//replace commas
					sBatchDB = ConstChar(_T("StatSimConn_") + currLevel);
					arrayBatchDB.InsertAt(i, (LPCTSTR) sBatchDB);
					// odbc - FALSE
					pGlobalConn = new CStatSimConn( userName, userPwd, sHost, sPort, arrayBatchDB[i], FALSE );
					ConfigDB(pGlobalConn);
				}
				ImportData(currPath, m_qnrID);

			}
		}
		
		delete pGlobalConn; pGlobalConn=0;	//redirect global database to the global
		// odbc - FALSE
		pGlobalConn = new CStatSimConn( userName, userPwd, sHost, sPort, ConstChar(sDB), ODBC );


		
		//pPurok = new CPurok();
		//pPurok->UpdateTable();
		//delete pPurok;
		//pPurok = NULL;
		
		//CHousehold * pHousehold = new CHousehold();	
		//pHousehold->ProcInd();
		//delete pHousehold;
		//pHousehold = NULL;
		
		//CPerson * pPerson = new CPerson();	
		//pPerson->ProcInd();
		//delete pPerson;
		//pPerson = NULL;

		msgText.Format(_T("Do you want to import another set of files?"));
		msgResult = AfxMessageBox (msgText, MB_YESNO);

		if (msgResult==IDYES)
			return 1;
		else
			return 0;

	}

	catch(_com_error &e) 
	{
		_bstr_t bstrSource (e.Source());
		_bstr_t bstrDescription (e.Description());
		TRACE ( "Exception thrown for classes generated by #import" );
		TRACE ( "\tCode = %08lx\n", e.Error ());
		TRACE ( "\tCode meaning = %s\n", e.ErrorMessage ());
		TRACE ( "\tSource = %s\n", (LPCTSTR) bstrSource);
		TRACE ( "\tDescription = %s\n", (LPCTSTR) bstrDescription);
		
		AfxMessageBox ((LPCTSTR) bstrDescription);
	}


}



*/
void CRemoveAutoSelDlg::OnNMDblclkTreelevel(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
