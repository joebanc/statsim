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
#if !defined(AFX_DLGADDLAYER_H__E7CB78C0_1CE0_11D3_ADF1_44D1F2C00000__INCLUDED_)
#define AFX_DLGADDLAYER_H__E7CB78C0_1CE0_11D3_ADF1_44D1F2C00000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAddLayer.h : header file
//

#include "buttoncolor.h"
#include "buttonselectall.h"
#include "comboboxftype.h"
#include "bdlistbox.h"
#include "longarray.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgAddLayer dialog

class CDlgAddLayer : public CDialog
{
// Construction
public:
	CDlgAddLayer(CWnd* pParent = NULL);   // standard constructor
   ~CDlgAddLayer();

   long GetFType() {return m_lFType;}
   CLongArray& GetFeatures() {return m_alFeatures;}
   CFTypeAttrArray& GetAttr() {return m_aAttr;}
   COLORREF GetColour() {return m_color;}
   BOOL IsMapLines() {return m_bMapLines;}

   void RemoveNonMapFTypes();

   static void Reset();
   static COLORREF GetDefaultColour() {return m_crSel;}

protected:

   void InitAttr();
   void RemoveAttr();
   void InitParentAttr();
   BOOL IsMapFType(long lFType);

   long m_lFType;
   CLongArray m_alFeatures;
   CFTypeAttrArray m_aAttr;
   CArray <CFTypeMap, CFTypeMap> m_aFTypeMap;
   COLORREF m_color;   
   BOOL m_bMapLines;
   BOOL m_bInit;
   static COLORREF m_crSel;

   CToolTipCtrl* m_pTooltip;   

// Dialog Data
	//{{AFX_DATA(CDlgAddLayer)
	enum { IDD = IDD_ADDLAYER };
	CComboBox	m_cbLabel;
	CComboBox	m_cbMapObj;
	CButtonSelectAll	m_pbSelectAll;
	CButtonColour	m_pbColour;		
	CComboBoxFType	m_cbFType;
	CBDListBox	   m_lbFeature;
	//}}AFX_DATA   

   virtual int OnToolHitTest( CPoint point, TOOLINFO* pTI ) const;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAddLayer)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAddLayer)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeFtype();	
	virtual void OnOK();
	afx_msg void OnSelectall();
	afx_msg void OnSelect();
	afx_msg void OnClose();
	afx_msg void OnDictionary();
	afx_msg void OnSelchangeFeature();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnOptions();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGADDLAYER_H__E7CB78C0_1CE0_11D3_ADF1_44D1F2C00000__INCLUDED_)
