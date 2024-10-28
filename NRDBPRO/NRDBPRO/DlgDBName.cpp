// DlgDBName.cpp : implementation file
//

#include "stdafx.h"
#include "nrdbpro.h"
#include "DlgDBName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDBName dialog


CDlgDBName::CDlgDBName(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDBName::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDBName)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgDBName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDBName)
	DDX_Control(pDX, IDC_DATABASE, m_eDatabase);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDBName, CDialog)
	//{{AFX_MSG_MAP(CDlgDBName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDBName message handlers

void CDlgDBName::OnOK() 
{   
	m_eDatabase.GetWindowText(m_sDBName);
	
   if (!m_sDBName.IsEmpty())
   {      
	   CDialog::OnOK();
   };
}
