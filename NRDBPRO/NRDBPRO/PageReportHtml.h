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
#if !defined(AFX_PAGEREPORTHTML_H__BA270A42_D6F2_11E1_B3CF_000795C2378F__INCLUDED_)
#define AFX_PAGEREPORTHTML_H__BA270A42_D6F2_11E1_B3CF_000795C2378F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageReportHtml.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPageReportHtml dialog

class CPageReportHtml : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageReportHtml)

// Construction
public:
	CPageReportHtml();
	~CPageReportHtml();

	void SetSettings(CBDMain* p) {m_pSettings = p;};

   enum {header,footer};


protected:

	CBDMain *m_pSettings;	
   int m_nLastEdit;

// Dialog Data
	//{{AFX_DATA(CPageReportHtml)
	enum { IDD = IDD_REPORTHTML };
	CComboBox	m_cbItem;
	CEdit	m_eHeader;
	CEdit	m_eFooter;
	CComboBox	m_cbStyleSheet;
	CButton	m_ckHeader;
	CButton	m_ckFooter;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageReportHtml)
	public:
	virtual BOOL OnKillActive();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageReportHtml)
	virtual BOOL OnInitDialog();
	afx_msg void OnUsefooter();
	afx_msg void OnUseheader();
	afx_msg void OnInsert();
	afx_msg void OnChangeHeader();
	afx_msg void OnChangeFooter();
	afx_msg void OnSetfocusFooter();
	afx_msg void OnSetfocusHeader();
	afx_msg void OnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEREPORTHTML_H__BA270A42_D6F2_11E1_B3CF_000795C2378F__INCLUDED_)
