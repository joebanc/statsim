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
#if !defined(AFX_SHEETMAPSTYLE_H__C9ED6650_D7AE_11E1_B3D3_000795C2378F__INCLUDED_)
#define AFX_SHEETMAPSTYLE_H__C9ED6650_D7AE_11E1_B3D3_000795C2378F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SheetMapStyle.h : header file
//

#include "pagemapstyleprop.h"

/////////////////////////////////////////////////////////////////////////////
// CSheetMapStyle

class CSheetMapStyle : public CPropertySheet
{
	DECLARE_DYNAMIC(CSheetMapStyle)

// Construction
public:
	
	CSheetMapStyle(CMapLayer* pMapLayer, CMapProperties* pMapProp, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
protected:
   CMapLayer *m_pMapLayer;
   CMapProperties *m_pMapProp;
   CPageMapStyleProp *m_pMapStyleProp;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSheetMapStyle)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSheetMapStyle();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSheetMapStyle)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHEETMAPSTYLE_H__C9ED6650_D7AE_11E1_B3D3_000795C2378F__INCLUDED_)
