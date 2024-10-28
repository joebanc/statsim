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
#if !defined(AFX_SHEETQUERY_H__6200C844_329F_11D4_84E7_BF3009935903__INCLUDED_)
#define AFX_SHEETQUERY_H__6200C844_329F_11D4_84E7_BF3009935903__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SheetQuery.h : header file
//

#include "pagequery.h"
#include "pagequerycond.h"
#include "pagequerystats.h"
#include "pagequerycalc.h"
#include "pagequerysort.h"
#include "pagequeryjoin.h"


/////////////////////////////////////////////////////////////////////////////
// CSheetQuery

class CSheetQuery : public CPropertySheet
{
	DECLARE_DYNAMIC(CSheetQuery)

// Construction
public:
	CSheetQuery(CQuery* pQuery, int nType, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);	

	enum {Text, HistGraph, TSGraph, Map, PieChart};

   CPageQuery* GetPageQuery() {return m_pPageQuery;}

// Attributes
protected:
	CPageQuery* m_pPageQuery;
	CPageQueryCond* m_pPageQueryCond;		
	CPageQueryStats* m_pPageQueryStats;		
   CPageQueryCalc* m_pPageQueryCalc;
   CPageQuerySort* m_pPageQuerySort;
   CPageQueryJoin* m_pPageQueryJoin;


   static CQuery m_QueryPrev;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSheetQuery)
	public:
	virtual BOOL OnInitDialog();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSheetQuery();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSheetQuery)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHEETQUERY_H__6200C844_329F_11D4_84E7_BF3009935903__INCLUDED_)
