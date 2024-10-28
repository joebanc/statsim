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
#if !defined(AFX_LISTBOXSTYLE_H__92BE2641_A3D2_11D4_A6C9_0080AD88D050__INCLUDED_)
#define AFX_LISTBOXSTYLE_H__92BE2641_A3D2_11D4_A6C9_0080AD88D050__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListBoxStyle.h : header file
//

#include "maplayer.h"

///////////////////////////////////////////////////////////////////////////////

class CListBoxStyleItem : public CMapStyle
{
friend class CListBoxStyle;

public:
   CListBoxStyleItem();
   
protected:   
   DWORD m_dwItemData;
   CString m_sText;   
};

/////////////////////////////////////////////////////////////////////////////
// CListBoxStyle window

class CListBoxStyle : public CListBox
{
// Construction
public:
	CListBoxStyle();

// Attributes
public:
   int AddString(LPCSTR, DWORD dwItemData, CMapStyle mapstyle);
   int InsertString(int index, LPCSTR, DWORD dwItemData, CMapStyle mapstyle);      
   void ResetContent();
   DWORD GetItemData(int index);
  
   CMapStyle GetStyle(int index);
   void SetStyle(int index, CMapStyle mapstyle);
   
   void SetText(int index, LPCSTR sText);
   LPCSTR GetText(int index);
   void UpDown(int iDir);   

   void SetMode(int nMode) {ASSERT(m_nMode == BDMAPLINES || m_nMode == BDCOORD); m_nMode = nMode;};

protected:

   CArray <CListBoxStyleItem, CListBoxStyleItem> m_aListBoxStyle;

   int m_nMode;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListBoxStyle)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListBoxStyle();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListBoxStyle)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
   afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnProperties();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTBOXSTYLE_H__92BE2641_A3D2_11D4_A6C9_0080AD88D050__INCLUDED_)
