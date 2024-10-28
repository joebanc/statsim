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
#include "shapefile.h"
#include "DlgDigitiseType.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDigitiseType dialog


CDlgDigitiseType::CDlgDigitiseType(BOOL bPolyOnly, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDigitiseType::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDigitiseType)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

   m_bPolyOnly = bPolyOnly;
}


void CDlgDigitiseType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDigitiseType)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDigitiseType, CDialog)
	//{{AFX_MSG_MAP(CDlgDigitiseType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDigitiseType message handlers

BOOL CDlgDigitiseType::OnInitDialog() 
{
	CDialog::OnInitDialog();

   GetDlgItem(IDC_POINTS)->EnableWindow(!m_bPolyOnly);
	
   CheckRadioButton(IDC_POLYLINES, IDC_POINTS, IDC_POLYLINES);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////
void CDlgDigitiseType::OnOK() 
{
    int nId = GetCheckedRadioButton(IDC_POLYLINES, IDC_POINTS);
	if (nId == IDC_POLYLINES) m_nType = SHPPolyLine;
   if (nId == IDC_POLYGONS) m_nType = SHPPolygon;
	if (nId == IDC_POINTS) m_nType = SHPPoint;
	
	CDialog::OnOK();
}
