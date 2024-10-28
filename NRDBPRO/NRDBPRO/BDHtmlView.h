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

#if !defined(AFX_BDHTMLVIEW_H__E88E9CC0_7793_11D3_AA19_84940C36F970__INCLUDED_)
#define AFX_BDHTMLVIEW_H__E88E9CC0_7793_11D3_AA19_84940C36F970__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BDHtmlView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBDHtmlView html view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include <afxhtml.h>

class CBDHtmlView : public CHtmlView
{
public:
   BOOL PreCloseView();

protected:
	CBDHtmlView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CBDHtmlView)

// html Data
public:
	//{{AFX_DATA(CBDHtmlView)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
protected:

   class CDocBDReport* m_pDoc;   
   BOOL m_bForward;
   BOOL m_bBack;
   CStringArray m_asTempFile;

   virtual void OnCommandStateChange(long nCommand, BOOL bEnable);

// Operations

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBDHtmlView)
	public:
	virtual void OnInitialUpdate();
	virtual void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags, LPCTSTR lpszTargetFrameName, CByteArray& baPostedData, LPCTSTR lpszHeaders, BOOL* pbCancel);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CBDHtmlView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CBDHtmlView)
	afx_msg void OnGoback();
	afx_msg void OnGoforward();
	afx_msg void OnEditCopy();
	afx_msg void OnFilePrint();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnUpdateGoback(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGoforward(CCmdUI* pCmdUI);
	//}}AFX_MSG
	LRESULT OnSetMessage(WPARAM, LPARAM);   
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BDHTMLVIEW_H__E88E9CC0_7793_11D3_AA19_84940C36F970__INCLUDED_)
