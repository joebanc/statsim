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
#if !defined(AFX_SHEETMAPPROP_H__757ED871_A0BA_11D4_A6C7_0080AD88D050__INCLUDED_)
#define AFX_SHEETMAPPROP_H__757ED871_A0BA_11D4_A6C7_0080AD88D050__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SheetMapProp.h : header file
//

#include "pagemaptextprop.h"
#include "pagemapstyleprop.h"
#include "pagemaplayerprop.h"
#include "pagemaplegendprop.h"
#include "maplayer.h"

/////////////////////////////////////////////////////////////////////////////
// CSheetMapProp

class CSheetMapProp : public CPropertySheet
{
	DECLARE_DYNAMIC(CSheetMapProp)

// Construction
public:	
   CSheetMapProp();
	CSheetMapProp(CMapLayerArray* pMapLayerArray, CMapLayer* pMapLayer, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Protected
public:

   CMapLayer* m_pMapLayer;
   CMapLayerArray* m_pMapLayerArray;
   CMapProperties m_mapprop;

   CPageMapLayerProp* m_pMapLayerProp;
   CPageMapStyleProp* m_pMapStyleProp;
   CPageMapTextProp* m_pMapTextProp;
   CPageMapLegendProp* m_pMapLegendProp;

   static int m_nDefaultPage;
   enum {layer,style,text,legend};

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSheetMapProp)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSheetMapProp();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSheetMapProp)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHEETMAPPROP_H__757ED871_A0BA_11D4_A6C7_0080AD88D050__INCLUDED_)
