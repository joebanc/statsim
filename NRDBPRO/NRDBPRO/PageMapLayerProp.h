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
#if !defined(AFX_PAGEMAPLAYERPROP_H__757ED873_A0BA_11D4_A6C7_0080AD88D050__INCLUDED_)
#define AFX_PAGEMAPLAYERPROP_H__757ED873_A0BA_11D4_A6C7_0080AD88D050__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageMapLayerProp.h : header file
//

#include "maplayer.h"
#include "buttoncolor.h"
#include "listboxstyle.h"
#include "editplus.h"


/////////////////////////////////////////////////////////////////////////////
// CPageMapLayerProp dialog

class CPageMapLayerProp : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageMapLayerProp)

// Construction
public:
	CPageMapLayerProp(CMapLayer*, CMapProperties*);
   CPageMapLayerProp();
	~CPageMapLayerProp();

   static BOOL GetMinMax(CMapLayer*, double& dMin, double& dMax);

   CListBoxStyle& GetLBLayers() {return m_lbLayers;};	

protected:
   CMapLayer* m_pMapLayer;
   CMapProperties* m_pMapProperty;
   BOOL m_bCoord;
   BOOL m_bMapLines;   
   int m_nSepColour;
   CBitmap m_bitmapPopup;

   CStyleSchemes m_aStyleSchemes;
   CRangeSchemes m_aRangeSchemes;
     
   void OnAutocolor();
   void SepColour(BOOL bInit = FALSE);   
   void RangeColour(BOOL bInit = FALSE);

   CMapStyle GetStyle(int nValue, int index = -1);
   CColourRange GetColourRange(int nValue, int index = -1);

   void UpdateRangeColour();   

   void InitControls();
   void InitScheme(CStyleScheme&, COLORREF crMin, COLORREF crMax);
   void InitSchemes();
   void RestoreSchemes();
   void SaveSchemes();




   enum {none, autocolor, autosize, sepcolor, rangecolor, ranking, legendvalues};

// Dialog Data
	//{{AFX_DATA(CPageMapLayerProp)
	enum { IDD = IDD_MAPLAYERPROP };
	CButton	m_pbSchemeMenu;
	CComboBox	m_cbColourScheme;
	CComboBox	m_cbAutoStyle;
	CEdit	m_eFeatureName;	
	CListBoxStyle	m_lbLayers;	
	CEditPlus	m_eMin;
	CEditPlus	m_eMax;
   CButtonColour m_pbColour;
   CEdit	m_eLayerName;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageMapLayerProp)
	public:
	virtual void OnOK();
	virtual BOOL OnKillActive();   
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageMapLayerProp)
	virtual BOOL OnInitDialog();
	afx_msg void OnSepcolor();
	afx_msg void OnSelchangeLayers();
	afx_msg void OnChangeFeaturename();
	afx_msg void OnDeltaposUpdown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeAutostyle();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnSelchangeColourscheme();
	afx_msg void OnBrowse();
	afx_msg void OnSchememenu();
	afx_msg void OnSchemeSave();
	afx_msg void OnSchemeDelete();
	afx_msg void OnAdvanced();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEMAPLAYERPROP_H__757ED873_A0BA_11D4_A6C7_0080AD88D050__INCLUDED_)
