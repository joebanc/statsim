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
#if !defined(AFX_LISTBOXTICK_H__41488780_4947_11D3_A9D1_9E8F718E0870__INCLUDED_)
#define AFX_LISTBOXTICK_H__41488780_4947_11D3_A9D1_9E8F718E0870__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListBoxTick.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListBoxTick window

struct CListBoxItemData
{
   DWORD m_dwItemData;
   BOOL m_bCheck;

   CListBoxItemData() 
   {
      m_dwItemData = 0;
      m_bCheck = 0;
   }
};

/////////////////////////////////////////////////////////////////////////////

class CListBoxTick : public CListBox
{
// Construction
public:
	CListBoxTick();   

// Operations
public:   
   void Init(BOOL bMultiSel = TRUE) {m_bMultiSel = bMultiSel;}
   BOOL IsCheck(int i);
   void SetCheck(int i, BOOL bSel);

   int AddString(LPCSTR sString);
   int InsertString(int, LPCSTR);

   void SetItemData(int index, DWORD dw);
   DWORD GetItemData(int index);

// Attributes
protected:
   BOOL m_bMultiSel;
   
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListBoxTick)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);	
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListBoxTick();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListBoxTick)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);	
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTBOXTICK_H__41488780_4947_11D3_A9D1_9E8F718E0870__INCLUDED_)
