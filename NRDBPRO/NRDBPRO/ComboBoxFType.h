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

#if !defined(AFX_COMBOBOXFTYPE_H__0204DE10_B137_11D3_AAAD_0080AD88D050__INCLUDED_)
#define AFX_COMBOBOXFTYPE_H__0204DE10_B137_11D3_AAAD_0080AD88D050__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComboBoxFType.h : header file
//

#include "bdcombobox.h"

/////////////////////////////////////////////////////////////////////////////

struct CDictSel : public CDictionary
{   
   BOOL m_bSel;   
};

class CDictSelArray : public CArray <CDictSel, CDictSel>
{
};

/////////////////////////////////////////////////////////////////////////////
// CComboBoxFType window

class CComboBoxFType : public CBDComboBox
{
// Construction
public:
	CComboBoxFType();

   static CDictSelArray& GetSectorSel() {return m_aDictSel;}

// Attributes
protected:

   static CDictSelArray m_aDictSel;
   static BOOL m_bIncLinks;

// Operations
public:

   void Init(long lFTypeSel = 0, BOOL bManyToOne = FALSE, BOOL bRecurse = FALSE);
   static void ShowDictionary();      
   void OnClickDictionary(BOOL bManyToOne = FALSE);
   static void InitDictionary(BOOL bReset = FALSE);         

protected:
   
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboBoxFType)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CComboBoxFType();

	// Generated message map functions
protected:
	//{{AFX_MSG(CComboBoxFType)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMBOBOXFTYPE_H__0204DE10_B137_11D3_AAAD_0080AD88D050__INCLUDED_)
