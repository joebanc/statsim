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
#if !defined(AFX_DOCPIECHART_H__E250BF90_DC03_11D5_AC49_B9A6803FFA08__INCLUDED_)
#define AFX_DOCPIECHART_H__E250BF90_DC03_11D5_AC49_B9A6803FFA08__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DocPieChart.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDocPieChart document

#include "docgraph.h"

class CDocPieChart : public CDocGraph
{
protected:
	CDocPieChart();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDocPieChart)

// Attributes
public:

// Operations
protected:


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocPieChart)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDocPieChart();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CDocPieChart)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOCPIECHART_H__E250BF90_DC03_11D5_AC49_B9A6803FFA08__INCLUDED_)
