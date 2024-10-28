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

#if !defined(AFX_BDCOMBOBOX_H__AA38D3D1_6984_11D3_A9F2_CC5C9EDC0770__INCLUDED_)
#define AFX_BDCOMBOBOX_H__AA38D3D1_6984_11D3_A9F2_CC5C9EDC0770__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BDComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBDComboBox window

class CBDComboBox : public CComboBox
{
// Construction
public:
	CBDComboBox();

// Attributes
public:

// Operations
public:
   void SaveFTypes();
   int AddStringX(LPCSTR, DWORD dwData);
   int GetItemDataX(long&);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBDComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBDComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBDComboBox)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BDCOMBOBOX_H__AA38D3D1_6984_11D3_A9F2_CC5C9EDC0770__INCLUDED_)
