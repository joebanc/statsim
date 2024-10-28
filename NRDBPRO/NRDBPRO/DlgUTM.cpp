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
#include "DlgUTM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgUTM dialog


CDlgUTM::CDlgUTM(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgUTM::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgUTM)		   
	//}}AFX_DATA_INIT   
}


void CDlgUTM::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgUTM)
	DDX_Control(pDX, IDC_BUTTON1, m_pbUTM);
	DDX_Control(pDX, IDC_ZONE, m_cbZone);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgUTM, CDialog)
	//{{AFX_MSG_MAP(CDlgUTM)
	ON_CBN_SELCHANGE(IDC_ZONE, OnSelchangeZone)
   ON_MESSAGE(WM_SETZONE, OnSetZone)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgUTM message handlers

BOOL CDlgUTM::OnInitDialog() 
{
	CDialog::OnInitDialog();

   CString s;
	
   for (int i = 1; i <= 60; i++)
   {
      s.Format("%s %0i ", BDString(IDS_ZONE), i);

      int index = m_cbZone.AddString(s + " " + BDString(IDS_NORTH));
      m_cbZone.SetItemData(index, i);

      index = m_cbZone.AddString(s + " " + BDString(IDS_SOUTH));
      m_cbZone.SetItemData(index, -i);

      if (i == 30) m_cbZone.SetCurSel(index);
   }

   OnSelchangeZone();

   CenterWindow();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////

void CDlgUTM::OnSelchangeZone() 
{
   int index = m_cbZone.GetCurSel();
   if (index != CB_ERR)
   {
      int i = m_cbZone.GetItemData(index);

      CString s; 
      s.Format("%s=%i", BDString(IDS_CENTRALMERIDIAN), -177+(abs(i)-1)*6);   
      GetDlgItem(IDS_MERIDIAN)->SetWindowText(s);

      m_pbUTM.SetZone(i);
   };
}

///////////////////////////////////////////////////////////////////////////////

LRESULT CDlgUTM::OnSetZone(WPARAM wParam, LPARAM lParam)
{
   // Call from utm control to indicate that zone has changed

   int i = 0; for (i = 0; i < m_cbZone.GetCount(); i++)
   {
      if (m_cbZone.GetItemData(i) == wParam)
      {
         m_cbZone.SetCurSel(i);
         OnSelchangeZone();
         break;
      }
   }
   return 0;
}

///////////////////////////////////////////////////////////////////////////////

void CDlgUTM::OnOK() 
{
	int index = m_cbZone.GetCurSel();
   if (index != CB_ERR)
   {
      m_nZone = m_cbZone.GetItemData(index);
	
	   CDialog::OnOK();
   };
}

