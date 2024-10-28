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
#if !defined(AFX_SHEETREPORTSETTINGS_H__BA270A43_D6F2_11E1_B3CF_000795C2378F__INCLUDED_)
#define AFX_SHEETREPORTSETTINGS_H__BA270A43_D6F2_11E1_B3CF_000795C2378F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SheetReportSettings.h : header file
//

#include "pagereportdetails.h"
#include "pagereporthtml.h"
#include "pagereportunits.h"

/////////////////////////////////////////////////////////////////////////////
// CSheetReportSettings

class CSheetReportSettings : public CPropertySheet
{
	DECLARE_DYNAMIC(CSheetReportSettings)

// Construction
public:
	CSheetReportSettings(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CSheetReportSettings(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	CBDMain GetSettings() {return m_main;}

// Attributes
protected:
	CPageReportDetails m_pagedetails;
    CPageReportHtml m_pagehtml;
    CPageReportUnits m_pageunits;

	CBDMain m_main;	


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSheetReportSettings)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSheetReportSettings();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSheetReportSettings)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHEETREPORTSETTINGS_H__BA270A43_D6F2_11E1_B3CF_000795C2378F__INCLUDED_)
