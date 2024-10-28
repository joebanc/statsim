// ProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StatSimPro.h"
#include "ProgressDlg.h"


// CProgressDlg dialog

IMPLEMENT_DYNAMIC(CProgressDlg, CDialog)

CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDlg::IDD, pParent)
{
	m_pView = NULL;
	
	AfxGetMainWnd()->RedrawWindow();
	Create(IDD, pParent);
	ShowWindow(SW_SHOW);
	CenterWindow();

	m_IsCancel = FALSE;

}

CProgressDlg::~CProgressDlg()
{
	if (IsWindow(GetSafeHwnd()))
	{
		EndDialog(!m_IsCancel);
	}

}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESSLABEL, m_ProgressLabel);
	DDX_Control(pDX, IDC_PROGRESSBAR, m_ProgressBar);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CProgressDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CProgressDlg message handlers

BOOL CProgressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetActiveWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CProgressDlg::OnBnClickedCancel()
{
	DestroyWindow();
	OnCancel();
}
