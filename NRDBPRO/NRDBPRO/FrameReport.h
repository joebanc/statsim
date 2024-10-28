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
#if !defined(AFX_FRAMEREPORT_H__A1B438A2_039D_11D4_AB44_0080AD88D050__INCLUDED_)
#define AFX_FRAMEREPORT_H__A1B438A2_039D_11D4_AB44_0080AD88D050__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrameReport.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFrameReport frame

class CFrameReport : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CFrameReport)
protected:
	CFrameReport();           // protected constructor used by dynamic creation

// Attributes
public:
   CToolBar m_wndToolBar;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrameReport)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CFrameReport();

	// Generated message map functions
	//{{AFX_MSG(CFrameReport)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAMEREPORT_H__A1B438A2_039D_11D4_AB44_0080AD88D050__INCLUDED_)
