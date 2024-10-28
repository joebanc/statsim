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
#if !defined(AFX_DLGMAPLAYERS_H__D1FCB7C2_59BE_11D3_A9E0_C52B49F90470__INCLUDED_)
#define AFX_DLGMAPLAYERS_H__D1FCB7C2_59BE_11D3_A9E0_C52B49F90470__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMapLayers.h : header file
//

#include "maplayer.h"
#include "docmap.h"
#include "listboxtick.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgMapLayers dialog

class CDlgMapLayers : public CDialog
{
// Construction
public:
	CDlgMapLayers(CMapLayerArray* paMapObject, CDocMap* pDoc, CWnd* pParent = NULL);   // standard constructor
   ~CDlgMapLayers();

protected:

   CMapLayerArray* m_paMapObject;
   CDocMap* m_pDocMap;

   void InitLayers(int iSel = -1, BOOL bInit = FALSE);   

// Dialog Data
	//{{AFX_DATA(CDlgMapLayers)
	enum { IDD = IDD_MAPLAYERS };
	CButton	m_ckAutoUpdate;
	//}}AFX_DATA

   CListBoxTick m_lbMapLayer;
   
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMapLayers)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgMapLayers)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnDeltaposUpdown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnProperties();
	afx_msg void OnQuery();
	afx_msg void OnClose();
	virtual void OnOK();
	afx_msg void OnLegend();
	afx_msg void OnDblclkMaplayer();
	afx_msg void OnOverlays();
	afx_msg void OnNew();
	afx_msg void OnSelchangeMaplayer();
	afx_msg void OnAutoupdate();
	afx_msg void OnCbms();
	//}}AFX_MSG
   //LRESULT OnUpdateProgress(WPARAM, LPARAM);
   //LRESULT OnResetProgress(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMAPLAYERS_H__D1FCB7C2_59BE_11D3_A9E0_C52B49F90470__INCLUDED_)
