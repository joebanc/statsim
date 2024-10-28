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
#if !defined(AFX_DOCMAPLAYOUT_H__F6F2AF41_EE38_11E1_B3FA_000795C2378F__INCLUDED_)
#define AFX_DOCMAPLAYOUT_H__F6F2AF41_EE38_11E1_B3FA_000795C2378F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DocMapLayout.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDocMapLayout document

class CDocMapLayout : public CDocument
{
protected:
	CDocMapLayout();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDocMapLayout)

   void SetMapLayout(CMapLayout *pMapLayout) {m_pMapLayout = pMapLayout;}
   CMapLayout* GetMapLayout() {ASSERT(m_pMapLayout != NULL); return m_pMapLayout;}
   
// Operations
protected:

   CMapLayout *m_pMapLayout;

// Attributes
protected:


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocMapLayout)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual void SetTitle(LPCTSTR lpszTitle);
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDocMapLayout();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CDocMapLayout)
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOCMAPLAYOUT_H__F6F2AF41_EE38_11E1_B3FA_000795C2378F__INCLUDED_)
