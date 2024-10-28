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
#include "nrdbpro.h"
#include "FrameMapLayout.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrameMapLayout

IMPLEMENT_DYNCREATE(CFrameMapLayout, CMDIChildWnd)

CFrameMapLayout::CFrameMapLayout()
{
}

CFrameMapLayout::~CFrameMapLayout()
{
}


BEGIN_MESSAGE_MAP(CFrameMapLayout, CMDIChildWnd)
	//{{AFX_MSG_MAP(CFrameMapLayout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrameMapLayout message handlers

BOOL CFrameMapLayout::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
   m_wndToolBar.Create(this);
   m_wndToolBar.LoadToolBar(IDR_MAPLAYOUT);   
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | CBRS_FLOAT_MULTI );
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);		

	
	return CMDIChildWnd::OnCreateClient(lpcs, pContext);
}
