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
#if !defined(AFX_DLGSELECTFEATURES_H__CE3E5870_5F69_11D3_A9E8_936407643D70__INCLUDED_)
#define AFX_DLGSELECTFEATURES_H__CE3E5870_5F69_11D3_A9E8_936407643D70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSelectFeatures.h : header file
//

#include "buttonselectall.h"
#include "bdlistbox.h"
#include "longarray.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectFeatures dialog

class CDlgSelectFeatures : public CDialog
{
// Construction
public:
	CDlgSelectFeatures(long lFType, BOOL bSglSel = FALSE, CWnd* pParent = NULL);   // standard constructor

   void Initialise(CListBox&);
   void Initialise(CComboBox&);
   CLongArray& GetFeatures() {return m_alFeatures;}
   
protected:

   long m_lFType, m_lParentFType, m_lGrandParentFType;   
   CLongArray m_alFeatures;
   static CDWordArray m_alParentFeatures;
   CBDListBox* m_plbFeatures;
   BOOL m_bSglSel;      

// Dialog Data
	//{{AFX_DATA(CDlgSelectFeatures)
	enum { IDD = IDD_SELECTFEATURES };
	CButtonSelectAll	m_pbSelectAll2;
	CButtonSelectAll	m_pbSelectAll1;
	CBDListBox	m_lbFeaturesMult;
	CBDListBox	m_lbFeaturesSgl;
	CBDListBox	m_lbParentFeatures;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSelectFeatures)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSelectFeatures)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeParentfeatures();
	afx_msg void OnSelectall1();
	afx_msg void OnSelectall2();
	virtual void OnOK();
	afx_msg void OnSelchangeFeaturesmult();
	afx_msg void OnSelchangeFeaturessgl();
	afx_msg void OnSelect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSELECTFEATURES_H__CE3E5870_5F69_11D3_A9E8_936407643D70__INCLUDED_)
