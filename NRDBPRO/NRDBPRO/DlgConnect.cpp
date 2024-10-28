// DlgConnect.cpp : implementation file
//

#include "stdafx.h"
#include "nrdbpro.h"
#include "DlgConnect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgConnect dialog


CDlgConnect::CDlgConnect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgConnect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgConnect)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

   m_sUserName = AfxGetApp()->GetProfileString("Database","UserName");
   m_sPassword = AfxGetApp()->GetProfileString("Database","Password");
   m_sHost = AfxGetApp()->GetProfileString("Database","Host", "localhost");
}


void CDlgConnect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConnect)
	DDX_Control(pDX, IDC_USERNAME, m_eUserName);
	DDX_Control(pDX, IDC_PASSWORD, m_ePassword);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgConnect, CDialog)
	//{{AFX_MSG_MAP(CDlgConnect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgConnect message handlers

BOOL CDlgConnect::OnInitDialog() 
{
	CDialog::OnInitDialog();
   
   m_eUserName.SetWindowText(m_sUserName);
   m_ePassword.SetWindowText(m_sPassword);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgConnect::OnOK() 
{
   m_eUserName.GetWindowText(m_sUserName);
   m_ePassword.GetWindowText(m_sPassword);

   AfxGetApp()->WriteProfileString("Database","UserName",m_sUserName);
   AfxGetApp()->WriteProfileString("Database","Password",m_sPassword);
   AfxGetApp()->WriteProfileString("Database","Host",m_sHost);
	
	CDialog::OnOK();
}
