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
#if !defined(AFX_PAGEREPORTDETAILS_H__BA270A41_D6F2_11E1_B3CF_000795C2378F__INCLUDED_)
#define AFX_PAGEREPORTDETAILS_H__BA270A41_D6F2_11E1_B3CF_000795C2378F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageReportDetails.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPageReportDetails dialog

class CPageReportDetails : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageReportDetails)

// Construction
public:
	CPageReportDetails();
	~CPageReportDetails();

	void SetSettings(CBDMain* p) {m_pSettings = p;};

protected:

	CBDMain *m_pSettings;	

// Dialog Data
	//{{AFX_DATA(CPageReportDetails)
	enum { IDD = IDD_REPORTDETAILS };
	CEdit	m_eAddress;
	CEdit	m_eTelephone;
	CEdit	m_eProvince;
	CEdit	m_eOrganization;
	CEdit	m_eLogo;
	CEdit	m_eFax;
	CEdit	m_eEmail;
	CEdit	m_eCountry;
	CEdit	m_eCity;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageReportDetails)
	public:
	virtual BOOL OnKillActive();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageReportDetails)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEREPORTDETAILS_H__BA270A41_D6F2_11E1_B3CF_000795C2378F__INCLUDED_)
