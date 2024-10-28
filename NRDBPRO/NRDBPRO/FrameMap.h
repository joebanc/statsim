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
#if !defined(AFX_FRAMEMAP_H__5CB1BE60_1345_11D3_ADF1_44C1F2C00000__INCLUDED_)
#define AFX_FRAMEMAP_H__5CB1BE60_1345_11D3_ADF1_44C1F2C00000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrameMap.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFrameMap frame

class CFrameMap : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CFrameMap)
protected:
	CFrameMap();           // protected constructor used by dynamic creation

// Attributes
public:
	CSplitterWnd m_wndSplitter;
   CSplitterWnd m_wndSplitter2;
   CToolBar m_wndToolBar;
   BOOL m_bCreated;

// Operations
public:
	virtual BOOL OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* pContext );
   

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrameMap)
	public:
	virtual void ActivateFrame(int nCmdShow = -1);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CFrameMap();

	// Generated message map functions
	//{{AFX_MSG(CFrameMap)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAMEMAP_H__5CB1BE60_1345_11D3_ADF1_44C1F2C00000__INCLUDED_)
