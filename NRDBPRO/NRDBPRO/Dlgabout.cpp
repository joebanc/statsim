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
#include "dlgabout.h"
#include "viewmap.h"

/////////////////////////////////////////////////////////////////////////////

CDlgAbout::CDlgAbout(BOOL bSplash) : CDialog(CDlgAbout::IDD)
{
	//{{AFX_DATA_INIT(CDlgAbout)
	//}}AFX_DATA_INIT

	m_bSplash = bSplash;   
}

CDlgAbout::~CDlgAbout()
{   
};

void CDlgAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAbout)		
   DDX_Control(pDX, IDC_BUTTON1, m_pbAnimate);
   DDX_Control(pDX, IDC_BUTTON2, m_pbAnimate2);
   DDX_Control(pDX, IDC_BUTTON3, m_pbAnimate3);
   DDX_Control(pDX, IDC_CREDITS, m_pbCredits);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgAbout, CDialog)
	//{{AFX_MSG_MAP(CDlgAbout)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_HOMEPAGE, OnHomePage)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////

BOOL CDlgAbout::OnInitDialog() 
{
   CString s;
	CDialog::OnInitDialog();
   
   m_pbAnimate.Invalidate();
   m_pbAnimate.RedrawWindow();
   
   s = "NRDB Pro 2.3";
   if (BDIsMySQL(BDHandle())) s += " (MySQL version)";
   else s += " (ODBC version)";

   s += "\r\n" + BDString(IDS_BUILDDATE) + ": "+ CString(__DATE__);
   GetDlgItem(IDC_BUILDDATE)->SetWindowText(s);

	if (m_bSplash)
	{

      // Read delay from registry

       HKEY hKey;
       RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Natural Resources Database\\NRDB Pro",  &hKey);      

       int nDelay = 5;
       DWORD cb = sizeof(nDelay);
       DWORD nType;
       RegQueryValueEx(hKey, "Delay", 0, &nType, (BYTE*)&nDelay, &cb);                              
       RegCloseKey(hKey);

		 SetTimer(0,nDelay*1000,NULL);	    
       GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
	} 
   
  // Search through all child windows, if they are of static
  // text or push buttons then set their text
   	   	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

void CDlgAbout::OnTimer(UINT nIDEvent) 
{  
   if (nIDEvent == 0)
   {
	   OnOK();
   }
      
	CDialog::OnTimer(nIDEvent);
}

///////////////////////////////////////////////////////////////////////////////

void CDlgAbout::OnLButtonDown(UINT nFlags, CPoint point) 
{
    if (m_bSplash)
	{
		OnOK();
	}

   // Check if clicked on homepage

   CRect rect;
   GetDlgItem(IDC_HOMEPAGE)->GetClientRect(&rect);
   GetDlgItem(IDC_HOMEPAGE)->ClientToScreen(&rect);
   ScreenToClient(&rect);

   if (rect.left <= point.x && point.x <= rect.right && 
       rect.top <= point.y && point.y <= rect.bottom)
   {
      OnHomePage();
   }
	
	CDialog::OnLButtonDown(nFlags, point);
}



///////////////////////////////////////////////////////////////////////////////

void CDlgAbout::OnHomePage() 
{
   OnOK();
   if (!m_bSplash) BDOnHelpNrdbhomepage();   
}

///////////////////////////////////////////////////////////////////////////////

HBRUSH CDlgAbout::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
    return (HBRUSH)GetStockObject(WHITE_BRUSH);
}

///////////////////////////////////////////////////////////////////////////////

void CDlgAbout::OnLButtonDblClk(UINT nFlags, CPoint point) 
{   
	CDialog::OnLButtonDblClk(nFlags, point);
}


///////////////////////////////////////////////////////////////////////////////

void CDlgAbout::OnMouseMove(UINT nFlags, CPoint point) 
{
	CDialog::OnMouseMove(nFlags, point);
}


