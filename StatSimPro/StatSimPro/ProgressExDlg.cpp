// ProgressExDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StatSimPro.h"
#include "ProgressExDlg.h"
#include "StatSimWnd.h"

extern CStatSimConn* pGlobalConn;
extern CStatSimRS ***pdictRS;
extern daedict ***pDCT;
extern LPCSTR** qnrtables;

extern int *ntypes, nqnr;
extern int *rtlen;
extern LPCSTR** RTYPES;

_hprogress::_hprogress():m_proctype(IMPORT)
{
}

_hprogress::~_hprogress()
{
}

// CProgressExDlg dialog

IMPLEMENT_DYNAMIC(CProgressExDlg, CDialog)

CProgressExDlg::CProgressExDlg(_hprogress *phprogress, CWnd* pParent /*=NULL*/)
	: CDialog(CProgressExDlg::IDD, pParent), m_pDB(pGlobalConn)
{
	m_pDta = NULL;
	m_pSSS = NULL;

	m_bOK = FALSE;

	curr_i = -1;

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
		_T("Times New Roman")));                 // lpszFacename

	m_pView = NULL;
	m_ProgressList = 0;
	m_ProgressList = new CStatSimList(TRUE);

	m_phprogress = phprogress;
	
	AfxGetMainWnd()->RedrawWindow();

	Create(IDD, pParent);
	ShowWindow(SW_SHOW);
	CenterWindow();

	RECT r;
	this->GetClientRect(&r);

	m_ProgressList->Create(WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT,
		CRect(r.left+20,r.top+30,r.right-20,r.bottom-60), this, IDC_PROGRESSLIST);
	//COLORREF crBkColor = ::GetSysColor(COLOR_3DFACE);
	//m_ProgressList->SetBkColor(crBkColor);
	m_ProgressList->ShowWindow(SW_SHOW);

	SetActiveWindow();

	m_ProgressList->InsertColumn(0,_MBCS("Progress"),LVCFMT_LEFT,200);
	m_ProgressList->InsertColumn(1,_MBCS("Process"),LVCFMT_LEFT,250);
	m_ProgressList->InsertColumn(2,_MBCS("Description"),LVCFMT_LEFT,100);
	m_ProgressList->InsertColumn(3,_MBCS("Path"),LVCFMT_LEFT,300);
	m_ProgressList->GetHeaderCtrl()->EnableWindow(FALSE);

	int i,
		nObj = m_phprogress->m_sObj.size();

	for( i=0; i<nObj; i++ )
	{
		CString str;
		str.Format(_T("%d"),i+1);
		m_ProgressList->InsertItem(i, str);

		m_ProgressList->SetItemText(i, 1, _T("Not yet started"));

		str = m_phprogress->m_sDesc[i];
		m_ProgressList->SetItemText(i, 2, str);

		str = m_phprogress->m_sObj[i];
		m_ProgressList->SetItemText(i, 3, str);
		
	}

	m_ProgressList->InitProgressBars();

	// Repainting?
	//SetTimer(100,500,NULL);

}

CProgressExDlg::~CProgressExDlg()
{
	m_bOK = TRUE;
	m_ProgressList->DestroyWindow();
	delete m_ProgressList; m_ProgressList = 0;
}

void CProgressExDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDOK, m_Ok);
	DDX_Control(pDX, IDC_PROGRESSLIST, *m_ProgressList);
	DDX_Control(pDX, IDC_FRAMELIST, m_FrameList);
	DDX_Control(pDX, IDC_PROGRESSBAR, m_ProgressBar);
}


BEGIN_MESSAGE_MAP(CProgressExDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CProgressExDlg::OnBnClickedOk)
	//ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &CProgressExDlg::OnBnClickedCancel)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PROGRESSLIST, &CProgressExDlg::OnLvnItemchangedProgresslist)
END_MESSAGE_MAP()


// CProgressExDlg message handlers

void CProgressExDlg::OnBnClickedOk()
{
	m_bOK = TRUE;
	
	OnOK();
}
void CProgressExDlg::SetDB(CStatSimConn *pDB)
{
	m_pDB = pDB;
}

BOOL CProgressExDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CProgressExDlg::OnTimer(UINT nIDEvent)
{
	static int count=0;
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent==100) 
	{

		m_ProgressList->OnPaint();

	}

	CDialog::OnTimer(nIDEvent);
}

void CProgressExDlg::SetProgress(int bar, int pos)
{
	
	VERIFY(m_ProgressList->m_ProgressBars[bar]);

	//m_ProgressList->m_ProgressBars[bar]->SetPos(pos);
	//m_ProgressBar.SetPos(pos);

}

void CProgressExDlg::OnBnClickedCancel()
{
	DestroyWindow();
	OnCancel();
}

void CProgressExDlg::OnLvnItemchangedProgresslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
