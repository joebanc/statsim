// ImportSelPathDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StatSimPro.h"
#include "ImportSelPathDlg.h"
#include "ImportAutoSelDlg.h"

extern CStatSimConn* pGlobalConn;
extern CStatSimRS* pelemRS;
extern CString userName, userPwd;
extern CStatSimRS ***pdictRS;
extern daedict ***pDCT;

extern LPCSTR** qnrtables;

extern int *ntypes, nqnr;
extern int *rtlen;
extern LPCSTR** RTYPES;
extern long* qnrIDs;

extern CMFCRibbonStatusBarPane *g_wndStatusBarPane;	//I have transferred this from main to global to be accessible
extern CProgressCtrl  m_ProgStatBar;	//I have transferred this from main to global to be accessible
extern long hpq_id;

// CImportSelPathDlg dialog

IMPLEMENT_DYNAMIC(CImportSelPathDlg, CDialog)

CImportSelPathDlg::CImportSelPathDlg(long curr_qnrID, CWnd* pParent /*=NULL*/)
	: CDialog(CImportSelPathDlg::IDD, pParent)
{

	m_qnrID = curr_qnrID;
	//m_itemArray = new tstrvec2d;

	sCols = 0;
	width = 0;

}

CImportSelPathDlg::~CImportSelPathDlg()
{
/*	delete [] sCols;		//array of column labels;
	delete [] width;			//array of widths

	(*m_itemArray).clear();
	delete m_itemArray;
	m_itemArray = 0;
*/
}

void CImportSelPathDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLEAR, m_ButtonClear);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_REMOVE, m_ButtonRemove);
	DDX_Control(pDX, IDC_LISTPATH, m_ListPath);
	DDX_Control(pDX, IDC_AUTOSEL, m_ButtonAutoSelect);
	DDX_Control(pDX, IDC_ADD, m_ButtonAdd);
}


BEGIN_MESSAGE_MAP(CImportSelPathDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CImportSelPathDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CImportSelPathDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_AUTOSEL, &CImportSelPathDlg::OnBnClickedAutosel)
	ON_BN_CLICKED(IDC_ADD, &CImportSelPathDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_REMOVE, &CImportSelPathDlg::OnBnClickedRemove)
	ON_BN_CLICKED(IDC_CLEAR, &CImportSelPathDlg::OnBnClickedClear)
		//}}AFX_MSG_MAP
	//ON_WM_TIMER()

END_MESSAGE_MAP()


// CImportSelPathDlg message handlers

void CImportSelPathDlg::OnBnClickedOk()
{
	
	CString msgText;
	int msgResult;
	
	//m_ListPath.OnPaint();
	
	BeginWaitCursor();

	if ( !ImportBulkData(m_ListPath.GetItemArray(), m_ListPath.GetnRows(), m_qnrID) ) {
		
		OnOK();

		if (m_qnrID!=hpq_id)
			return;
		msgText.Format(_T("Do you want to start the processing?"));
		msgResult = AfxMessageBox (msgText, MB_YESNO);
		if (msgResult==IDYES) {
			ConUpElt(FALSE); //Configure and update
		}
	}

	EndWaitCursor();
}

void CImportSelPathDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CImportSelPathDlg::OnBnClickedAutosel()
{
	CImportAutoSelDlg* pImportAutoSelDlg;
	pImportAutoSelDlg = new CImportAutoSelDlg(m_qnrID);

	OnCancel();
	pImportAutoSelDlg->DoModal();
}

void CImportSelPathDlg::OnBnClickedAdd()
{
	CFileDialog dlg (TRUE,_T("txt"),NULL, OFN_FILEMUSTEXIST | 
		OFN_ALLOWMULTISELECT, _T("CBMS Text files (*.hpq, *.bpq)|*.hpq;*.bpq|")\
		_T("Other CBMS ASCII files (*.cbm, *.txt)|*.cbm;*.txt||")); 

	if (dlg.DoModal() == IDOK) 
	{    
		GetSelPaths(&dlg);

		//LPCSTR **mItemArray = m_itemArray->GetStrArr2D(TRANSPOSED);
		LPCSTR **pItemArray = new LPCSTR*[nPaths];
		for (int i=0; i<nPaths; i++) {
			pItemArray[i] = new LPCSTR[nAttr];
			for (int j=0; j<nAttr; j++) {
				pItemArray[i][j] = ConstChar(m_itemArray[j][i]);
			}
						
		}

		m_ListPath.InsertBulk(sCols, nAttr, pItemArray, nPaths, TRUE, NOACTION, width);
		
		if (m_ListPath.GetItemCount() > 0) {
			m_ButtonClear.EnableWindow();
			m_ButtonRemove.EnableWindow();
			m_OK.EnableWindow();
		}

		//cleanup
		for (int i=0; i<nPaths; i++) {
			if(pItemArray[i]) {
				delete pItemArray[i];
				pItemArray[i] = 0;
			}
		}
		if (pItemArray) {
			delete pItemArray;
			pItemArray = 0;
		}


	}
}
void CImportSelPathDlg::GetSelPaths(CFileDialog* pFileDlg)
{
	nPaths = 0;
	daestream* pSSS = NULL;

	//m_itemArray = new CSSArrayBuilder(STR_ARR, 2);
	//m_itemArray->SetDim(DIM1, nAttr);	//2D array with nAttr columns
	//m_itemArray->SetDim(DIM2);		//initialize at size 1
	//(*m_itemArray).resize(nAttr);
	m_itemArray.clear();
	m_itemArray.resize(nAttr);

	POSITION FileDlgPos = pFileDlg->GetStartPosition();

	while (FileDlgPos!=NULL) {

		nPaths++;

		int idx;
		for (int i=0; i<nqnr; i++) {
			if (qnrIDs[i]==m_qnrID)
				idx=i;
		}

		//Assign current values:
		CString strPath = pFileDlg->GetNextPathName(FileDlgPos);
		int rightBS = strPath.GetLength() - strPath.ReverseFind('\\') - 1;	//length minus position of \ minus one
		
		//file name
		//m_itemArray->PutVal(0, (nPaths-1), ConstChar(strPath));
		//(*m_itemArray)[0].push_back((strPath));
		m_itemArray[0].push_back(strPath);

		//file path
		//m_itemArray->PutVal(1, (nPaths-1), ConstChar(strPath.Right(rightBS)));
		//(*m_itemArray)[1].push_back((strPath.Right(rightBS)));
		m_itemArray[1].push_back((strPath.Right(rightBS)));

		pSSS = new daestream(ConstChar(strPath), RTYPES[idx], ntypes[idx], rtlen[idx], FALSE);

		//file size
		//m_itemArray->PutVal(2, (nPaths-1), pSSS->sFileSize());
		CString sSize(pSSS->sFileSize());
		//(*m_itemArray)[2].push_back(sSize);
		m_itemArray[2].push_back(sSize);

		//date modified
		//m_itemArray->PutVal(3, (nPaths-1), pSSS->sFileTimeMod());
		CString sTM(pSSS->sFileTimeMod());
		//(*m_itemArray)[3].push_back(sTM);
		m_itemArray[3].push_back(sTM);

		//Adjust by 1
		//m_itemArray->AdjDim(DIM2);		//adjust size by 1

		
		//cant destroy?
		//if (pSSS!=NULL) {
		//	delete pSSS; pSSS = 0;
		//}

	}

	//if (pSSS!=NULL) {
	//	delete pSSS; pSSS = 0;
	//}
	

}


void CImportSelPathDlg::OnBnClickedRemove()
{
	m_ListPath.SwitchContents();

	if (m_ListPath.GetItemCount()==0) {
		OnInitDialog();
	}
}

void CImportSelPathDlg::OnBnClickedClear()
{
	OnInitDialog();
}

BOOL CImportSelPathDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// temporary - no handler for autosel BPQ
	if (m_qnrID!=hpq_id) {
		m_ButtonAutoSelect.ShowWindow(SW_HIDE);
	}

	//set the tree imagelist
	HICON *pIcons = new HICON[1];
	pIcons[0] = AfxGetApp()->LoadIcon(IDI_TEXTFILE);
	m_ListPath.SetImageListEx(pIcons, 1);
	delete [] pIcons; pIcons = 0;
	//

	nPaths = 0;
	nAttr = 4;	//fixed number of columns;

	sCols = (LPCSTR*) malloc(nAttr * sizeof(LPCSTR));
	width = (int*) malloc(nAttr * sizeof(int));

	//The fixed array of attributes	
	sCols[0] = _MBCS("Path");			width[0] = 180;
	sCols[1] = _MBCS("File Name");		width[1] = 150;
	sCols[2] = _MBCS("Size");			width[2] = 80;
	sCols[3] = _MBCS("Date Modified");	width[3] = 120;

	m_ListPath.CleanUp(sCols, nAttr, FALSE, EMPTY, width);

	m_OK.EnableWindow(FALSE);
	m_ButtonRemove.EnableWindow(FALSE);
	m_ButtonClear.EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CImportSelPathDlg::OnTimer(UINT nIDEvent)
{
	static int count=0;
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent==100)
	{
		count++;
		for(int i=0;i<10;i++)
		{
			CString text;
			text.Format(_T("%d"),((count+i)%10)*10);
			m_ListPath.SetItemText(i,0,text);
		}
	}
	CDialog::OnTimer(nIDEvent);
}
