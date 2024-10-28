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
#if !defined(AFX_PAGEQUERY1_H__6200C842_329F_11D4_84E7_BF3009935903__INCLUDED_)
#define AFX_PAGEQUERY1_H__6200C842_329F_11D4_84E7_BF3009935903__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageQuery1.h : header file
//

#include "query.h"
#include "comboboxftype.h"

/////////////////////////////////////////////////////////////////////////////
// CPageQuery dialog

class CPageQuery : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageQuery)

// Construction
public:
	CPageQuery(CQuery* pQuery, CQuery* pQueryPrev, int nType);
	CPageQuery();
	~CPageQuery();

   void UpdateList(CQuery*, HTREEITEM hParent = TVI_ROOT);   

protected:
	CQuery* m_pQuery;
   CQuery* m_pQueryPrev;

   CImageList m_imagelist;
   int m_nNumber;
      
   void UpdateLink(CQueryLink*, HTREEITEM hParent);
   void RetrieveSelections(CQuery*, HTREEITEM hParent = TVI_ROOT);
   void RetrieveLinkSel(CQueryLink*, HTREEITEM hParent);
   CQueryElement* GetElement(CQuery*, HTREEITEM hItemSel, HTREEITEM hParent = TVI_ROOT);

   
   
   // Attributes

   BOOL m_bUpdateList;
   int m_nType;
   CToolTipCtrl* m_pTooltip;   
   CFont m_font;

// Dialog Data
	//{{AFX_DATA(CPageQuery)
	enum { IDD = IDD_QUERY };
	//CBDComboBox	m_cbSortBy;
	//CBDComboBox	m_cbStatistics;	
	CComboBoxFType	m_cbFType;
	CTreeCtrl	m_ctrlTree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageQuery)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageQuery)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeFtype();
	afx_msg void OnSelectall();		
	afx_msg void OnSelchangedElements(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDictionary();
	afx_msg void OnSelect();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEQUERY1_H__6200C842_329F_11D4_84E7_BF3009935903__INCLUDED_)
