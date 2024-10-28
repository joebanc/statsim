// PageReportUnits.cpp : implementation file
//

#include "stdafx.h"
#include "nrdbpro.h"
#include "PageReportUnits.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageReportUnits property page

IMPLEMENT_DYNCREATE(CPageReportUnits, CPropertyPage)

CPageReportUnits::CPageReportUnits() : CPropertyPage(CPageReportUnits::IDD)
{
	//{{AFX_DATA_INIT(CPageReportUnits)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPageReportUnits::~CPageReportUnits()
{
}

void CPageReportUnits::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageReportUnits)
	DDX_Control(pDX, IDC_AREA, m_cbArea);
	DDX_Control(pDX, IDC_LENGTH, m_cbLength);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageReportUnits, CPropertyPage)
	//{{AFX_MSG_MAP(CPageReportUnits)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageReportUnits message handlers

BOOL CPageReportUnits::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CUnits &units = BDGetApp()->GetUnits();
   
   CUnit unit;

   int i = 0; for (i = 0; i < units.GetSize(); i++)
   {
      if (units[i].m_nType == CUnits::length)
      {
         int index = m_cbLength.AddString(units[i].m_sName);
         if (units[i].m_bDefault) m_cbLength.SetCurSel(index);
      } 
      else if (units[i].m_nType == CUnits::area)
      {
         int index = m_cbArea.AddString(units[i].m_sName);
         if (units[i].m_bDefault) m_cbArea.SetCurSel(index);
      }
   }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

BOOL CPageReportUnits::OnKillActive() 
{
   CString sUnit;

	// Retrieve default unit selections

   int index = m_cbLength.GetCurSel();
   if (index != CB_ERR)
   {
       m_cbLength.GetLBText(index, sUnit);
       BDGetApp()->GetUnits().SetDefault(CUnits::length, sUnit);
   }
   index = m_cbArea.GetCurSel();
   if (index != CB_ERR)
   {
       m_cbArea.GetLBText(index, sUnit);
       BDGetApp()->GetUnits().SetDefault(CUnits::area, sUnit);
   }
	
	return CPropertyPage::OnKillActive();
}
