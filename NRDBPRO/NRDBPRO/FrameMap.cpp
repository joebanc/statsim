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
#include "FrameMap.h"
#include "viewmap.h"
#include "viewlegend.h"
#include "viewlocator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrameMap

IMPLEMENT_DYNCREATE(CFrameMap, CMDIChildWnd)

CFrameMap::CFrameMap()
{
   m_bCreated = FALSE;
}

CFrameMap::~CFrameMap()
{
}


BEGIN_MESSAGE_MAP(CFrameMap, CMDIChildWnd)
	//{{AFX_MSG_MAP(CFrameMap)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrameMap message handlers

BOOL CFrameMap::OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* pContext )
{  
	CRect rect;	
   AfxGetMainWnd()->GetClientRect(&rect);

  // Create splitter windows

   m_wndSplitter.CreateStatic(this, 1, 2);     
   m_wndSplitter.CreateView(0,0, RUNTIME_CLASS(CViewMap), CSize((int)(rect.right*0.7), rect.bottom), pContext);
   m_wndSplitter2.CreateStatic(&m_wndSplitter, 2, 1, WS_CHILD | WS_VISIBLE, m_wndSplitter.IdFromRowCol(0, 1)); 
   m_wndSplitter2.CreateView(0, 0, RUNTIME_CLASS(CViewLegend), CSize((int)(rect.right*0.3), (int)(rect.bottom*0.5)), pContext); 
   m_wndSplitter2.CreateView(1, 0, RUNTIME_CLASS(CViewLocator), CSize((int)(rect.right*0.3), (int)(rect.bottom*.5)), pContext); 
   
   m_bCreated = TRUE;

   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

void CFrameMap::ActivateFrame(int nCmdShow) 
{
	// Always show the map maximized
	
	CMDIChildWnd::ActivateFrame(SW_SHOWMAXIMIZED);
}

///////////////////////////////////////////////////////////////////////////////

int CFrameMap::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
   m_wndToolBar.Create(this);
   m_wndToolBar.LoadToolBar(IDR_MAPTYPE);   
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | CBRS_FLOAT_MULTI );
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);		
		
	return 0;
}

///////////////////////////////////////////////////////////////////////////////

void CFrameMap::OnSize(UINT nType, int cx, int cy) 
{	
   CRect rect;
   GetClientRect(&rect);     
   
   if (m_bCreated)
   {
      m_wndSplitter.SetColumnInfo(0, (int)(rect.right*0.7), 0);
      m_wndSplitter.SetColumnInfo(1, (int)(rect.right*0.3), 0);
      m_wndSplitter2.SetRowInfo(0, (int)(rect.bottom*0.5), 0);
      m_wndSplitter2.SetRowInfo(1, (int)(rect.bottom*0.5), 0);
   };

   CMDIChildWnd::OnSize(nType, cx, cy);
}
