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

#if !defined(AFX_BUTTONCREDITS_H__F04969C1_48F0_11D6_AD71_905C54FB278D__INCLUDED_)
#define AFX_BUTTONCREDITS_H__F04969C1_48F0_11D6_AD71_905C54FB278D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ButtonCredits.h : header file
//

#define ROWS 30
#define COLS 8
#define OBJS 3

/////////////////////////////////////////////////////////////////////////////
// CButtonCredits window

class CButtonCredits : public CButton
{
// Construction
public:
	CButtonCredits();

// Attributes
protected:

   CString m_sDescription;
   static CFont m_font;

// Operations
public:

   void Credits(CDC*);   
   virtual void DrawItem(LPDRAWITEMSTRUCT);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CButtonCredits)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CButtonCredits();

	// Generated message map functions
protected:
	//{{AFX_MSG(CButtonCredits)	
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUTTONCREDITS_H__F04969C1_48F0_11D6_AD71_905C54FB278D__INCLUDED_)
