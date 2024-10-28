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
#include "DocPieChart.h"
#include "dlgreport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocPieChart

IMPLEMENT_DYNCREATE(CDocPieChart, CDocGraph)

CDocPieChart::CDocPieChart()
{
   m_nType = CDlgReport::PieChart;
}

BOOL CDocPieChart::OnNewDocument()
{
	if (!CDocGraph::OnNewDocument())
		return FALSE;

   if (m_bQuery)
   {
      BDSetPreviousReport(CNRDB::piechartquery);
   } else
   {
      BDSetPreviousReport(CNRDB::piechart);
   }

	return TRUE;
}

CDocPieChart::~CDocPieChart()
{
}


BEGIN_MESSAGE_MAP(CDocPieChart, CDocGraph)
	//{{AFX_MSG_MAP(CDocPieChart)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocPieChart diagnostics

#ifdef _DEBUG
void CDocPieChart::AssertValid() const
{
	CDocGraph::AssertValid();
}

void CDocPieChart::Dump(CDumpContext& dc) const
{
	CDocGraph::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDocPieChart serialization

void CDocPieChart::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDocPieChart commands


