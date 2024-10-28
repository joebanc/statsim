//////////////////////////////////////////////////////
//
// NRDB Pro - Spatial database and mapping application
//
// Copyright (c) 1989-2004 Richard D. Alexander
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// NRDB Pro is part of the Natural Resources Database Project 
// 
// Homepage: http://www.nrdb.co.uk/
// 

#include "stdafx.h"
#include "nrdb.h"
#undef CDialog
#include "DlgProgress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDlgProgress,  CDialog)

void BDProgressRange(int a, int b)
{   
	if (BDGetProgressBar() != NULL)
	{
		   BDGetProgressBar()->SetRange32(a, b);
	};     
}

BOOL BDProgressPos(int a)
{
   CBDProgressBar* pProgressBar = BDGetProgressBar();
	if (BDGetProgressBar() != NULL)
	{     
      if (pProgressBar->GetParent()->IsKindOf(RUNTIME_CLASS(CDlgProgress)))
      {           
          CDlgProgress* pProgress = (CDlgProgress*)pProgressBar->GetParent();
          return pProgress->SetProgress(a);                             
      } else
      {
         BDGetProgressBar()->SetPos(a);
      };
	};
   return TRUE;
}

void BDProgressText(LPCSTR s)
{
   if (BDGetProgressText() != NULL)
   {
      BDGetProgressText()->SetWindowText(s);
   }
}

/////////////////////////////////////////////////////////////////////////////
//
// class CBDProgressBar
//
// Supports multiple sets of progress i.e. a progress bar may consist of 5 parts
// with the progress on each part being displayed within the 1/5th
//

CBDProgressBar::CBDProgressBar()
{
   m_nTotalSets = 1;
   m_nSet = 0;
}

void CBDProgressBar::SetRange32(int nMin, int nMax)
{
   // Prevent resetting when using multiple sets

   if (nMax - nMin == 0)
   {
      if (m_nTotalSets == 1) Reset();
      return;
   }


   CProgressCtrl::SetRange32(nMin, nMax);

}

void CBDProgressBar::SetPos(int nPos)
{   
   CProgressCtrl::SetPos(nPos);

}

void CBDProgressBar::SetSet(int nSet, int nTotalSets)
{
   
   SetRange(0, nTotalSets);
   SetPos(nSet);
}

void CBDProgressBar::Reset()
{
   m_nTotalSets = 1;
   m_nSet = 0;
   CProgressCtrl::SetPos(0);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgProgress dialog


CDlgProgress::CDlgProgress(BOOL bCancel, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProgress::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgProgress)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

   AfxGetMainWnd()->RedrawWindow();
  
   Create(IDD, pParent);
   ShowWindow(SW_SHOW);
   CenterWindow();

   m_bCancel = m_bCancel;   
}

///////////////////////////////////////////////////////////////////////////////

CDlgProgress::~CDlgProgress()
{
   if (IsWindow(GetSafeHwnd()))
   {
      EndDialog(!m_bCancel);
   };
   BDSetProgressBar(NULL);
   BDSetProgressText(NULL);   

   // Tidy up
   //KillTimer(1);
}

void CDlgProgress::Destroy() 
{
   //EndDialog(!m_bCancel);
};

void CDlgProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgProgress)
	DDX_Control(pDX, IDC_TEXT, m_eText);
	DDX_Control(pDX, IDC_PROGRESS1, m_ctlProgress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgProgress, CDialog)
	//{{AFX_MSG_MAP(CDlgProgress)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CDlgProgress::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   BDSetActiveWnd(GetSafeHwnd());
   BDSetProgressBar(&m_ctlProgress);
   BDSetProgressText(&m_eText);

   GetDlgItem(IDCANCEL)->ShowWindow(m_bCancel);
   m_bCancel = FALSE;

   // Timer for updating display during long processes
   //SetTimer(1,10000, NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CDlgProgress message handlers

BOOL CDlgProgress::SetPercent(int nPercent)
{
   m_ctlProgress.SetRange(0,100);
   m_ctlProgress.SetPos(nPercent);   

   // Check for cancel button
   
   // Note, always pass handle of dialog box otherwise other components
   // e.g. html reports will not function correctly

   MSG msg;
   while (PeekMessage(&msg, GetSafeHwnd(), NULL, NULL, PM_REMOVE))
   {              
      TranslateMessage(&msg);
      DispatchMessage(&msg);         
   }

   return !m_bCancel;
}

/////////////////////////////////////////////////////////////////////////////

void CDlgProgress::SetRange(int nStart, int nEnd)
{
   m_ctlProgress.SetRange32(nStart, nEnd);   
}

/////////////////////////////////////////////////////////////////////////////

BOOL CDlgProgress::SetProgress(int n)
{
   m_ctlProgress.SetPos(n);   

   // Check for cancel button   
   
   MSG msg;
   if (PeekMessage(&msg, GetSafeHwnd(), NULL, NULL, PM_REMOVE) )
   {       
      TranslateMessage(&msg);
      DispatchMessage(&msg);               
   }

   return !m_bCancel;
}

/////////////////////////////////////////////////////////////////////////////

void CDlgProgress::OnClose() 
{
   m_bCancel = TRUE;	
	CDialog::OnClose();
}

/////////////////////////////////////////////////////////////////////////////

BOOL CDlgProgress::SetText(LPCSTR sText)
{
   GetDlgItem(IDC_TEXT)->SetWindowText(sText);

   // Check for cancel button

   MSG msg;
   while (PeekMessage(&msg, GetSafeHwnd(), NULL, NULL, PM_REMOVE))
   {              
      TranslateMessage(&msg);
      DispatchMessage(&msg);         
   }

   return !m_bCancel;
}

/////////////////////////////////////////////////////////////////////////////

void CDlgProgress::OnCancel() 
{
	m_bCancel = TRUE;
	
	//CDialog::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////


LRESULT CDlgProgress::OnResetProgress(WPARAM wParam, LPARAM lParam)
{
   BDGetProgressBar()->SetRange32(wParam, lParam);
   return 0;
}

/////////////////////////////////////////////////////////////////////////////

LRESULT CDlgProgress::OnUpdateProgress(WPARAM wParam, LPARAM)
{
   BDGetProgressBar()->SetPos(wParam);
   return 0;
}

void CDlgProgress::OnTimer(UINT nIDEvent) 
{
   // Refresh window periodically   
	/*RedrawWindow();*/      
	
	CDialog::OnTimer(nIDEvent);


}
