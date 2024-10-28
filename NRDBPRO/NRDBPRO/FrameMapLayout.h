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
#if !defined(AFX_FRAMEMAPLAYOUT_H__F6F2AF43_EE38_11E1_B3FA_000795C2378F__INCLUDED_)
#define AFX_FRAMEMAPLAYOUT_H__F6F2AF43_EE38_11E1_B3FA_000795C2378F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrameMapLayout.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFrameMapLayout frame

class CFrameMapLayout : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CFrameMapLayout)
protected:
	CFrameMapLayout();           // protected constructor used by dynamic creation

// Attributes
protected:
   CToolBar m_wndToolBar;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrameMapLayout)   
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CFrameMapLayout();

	// Generated message map functions
	//{{AFX_MSG(CFrameMapLayout)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAMEMAPLAYOUT_H__F6F2AF43_EE38_11E1_B3FA_000795C2378F__INCLUDED_)
