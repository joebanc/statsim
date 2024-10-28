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

#include "stdafx.h"
#include "nrdb.h"
#include "SheetQuery.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CQuery CSheetQuery::m_QueryPrev;

/////////////////////////////////////////////////////////////////////////////
// CSheetQuery

IMPLEMENT_DYNAMIC(CSheetQuery, CPropertySheet)

CSheetQuery::CSheetQuery(CQuery* pQuery, int nType, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
   if (pQuery->GetFType() != 0) m_QueryPrev = *pQuery;

	m_pPageQuery = new CPageQuery(pQuery, &m_QueryPrev, nType);
	m_pPageQueryCond = new CPageQueryCond(pQuery, &m_QueryPrev, nType);	
   m_pPageQueryStats = new CPageQueryStats(pQuery, &m_QueryPrev, nType);
   m_pPageQueryCalc = new CPageQueryCalc(pQuery, &m_QueryPrev, nType);
   m_pPageQuerySort = new CPageQuerySort(pQuery, &m_QueryPrev, nType);
   m_pPageQueryJoin = new CPageQueryJoin(pQuery, &m_QueryPrev, nType);

	AddPage(m_pPageQuery);
	AddPage(m_pPageQueryCond);
   AddPage(m_pPageQueryCalc);
   AddPage(m_pPageQueryStats);      
   AddPage(m_pPageQuerySort);
   AddPage(m_pPageQueryJoin);
}

/////////////////////////////////////////////////////////////////////////////

CSheetQuery::~CSheetQuery()
{
	if (m_pPageQuery != NULL) delete m_pPageQuery;
	if (m_pPageQueryCond != NULL) delete m_pPageQueryCond;
   if (m_pPageQueryStats != NULL) delete m_pPageQueryStats;
   if (m_pPageQueryCalc != NULL) delete m_pPageQueryCalc;
   if (m_pPageQuerySort != NULL) delete m_pPageQuerySort;
   if (m_pPageQueryJoin != NULL) delete m_pPageQueryJoin;
}

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSheetQuery, CPropertySheet)
	//{{AFX_MSG_MAP(CSheetQuery)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSheetQuery message handlers

BOOL CSheetQuery::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CSheetQuery::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}
