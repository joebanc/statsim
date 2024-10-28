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
#if !defined(AFX_DLGPROGRESS_H__4F670C81_C19E_11D3_AACA_0080AD88D050__INCLUDED_)
#define AFX_DLGPROGRESS_H__4F670C81_C19E_11D3_AACA_0080AD88D050__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgProgress.h : header file
//

/////////////////////////////////////////////////////////////////////////////

class CBDProgressBar : public CProgressCtrl
{

public:

   CBDProgressBar();

   void SetRange32(int, int);
   void SetPos(int);
   void SetSet(int, int);
   void Reset();

protected:

   int m_nTotalSets;
   int m_nSet;
   int m_nRange;
};

/////////////////////////////////////////////////////////////////////////////
// CDlgProgress dialog

class CDlgProgress : public CDialog
{
   DECLARE_DYNCREATE(CDlgProgress);

// Construction
public:
	CDlgProgress(BOOL bCancel = TRUE, CWnd* pParent = NULL);   // standard constructor
   ~CDlgProgress();   

   BOOL SetPercent(int nPercent);
   BOOL SetProgress(int n);
   void SetRange(int, int);
   BOOL SetText(LPCSTR);   

   BOOL IsCancel() {return m_bCancel;}

   void Destroy();

protected:

   BOOL m_bCancel;   

// Dialog Data
	//{{AFX_DATA(CDlgProgress)
	enum { IDD = IDD_PROGRESS };
	CStatic	m_eText;
	CBDProgressBar	m_ctlProgress;
	//}}AFX_DATA
   LRESULT OnUpdateProgress(WPARAM, LPARAM);
   LRESULT OnResetProgress(WPARAM, LPARAM);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgProgress)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgProgress)
	afx_msg void OnClose();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPROGRESS_H__4F670C81_C19E_11D3_AACA_0080AD88D050__INCLUDED_)
