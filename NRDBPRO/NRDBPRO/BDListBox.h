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

#if !defined(AFX_BDLISTBOX_H__06706211_6B03_11D3_A9F5_BB3677F63570__INCLUDED_)
#define AFX_BDLISTBOX_H__06706211_6B03_11D3_A9F5_BB3677F63570__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BDListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBDListBox window

class CBDListBox : public CListBox
{
// Construction
public:
	CBDListBox();

// Attributes
public:

// Operations
public:
   void SelectAll();
   void UpDown(int iDir);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBDListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBDListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBDListBox)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BDLISTBOX_H__06706211_6B03_11D3_A9F5_BB3677F63570__INCLUDED_)
