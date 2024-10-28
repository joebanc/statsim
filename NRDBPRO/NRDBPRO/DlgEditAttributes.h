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

#if !defined(AFX_DLGEDITATTRIBUTES_H__1C669AC4_144D_11D3_ADF1_44C1F2C00000__INCLUDED_)
#define AFX_DLGEDITATTRIBUTES_H__1C669AC4_144D_11D3_ADF1_44C1F2C00000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgEditAttributes.h : header file
//

#include "nrdbgrid.h"
#include "maplayer.h"
#include "dlgprogress.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgEditAttributes dialog

class CDlgEditAttributes : public CDialog
{
// Construction
public:

   CDlgEditAttributes(CLongArray& alFeature, long lFType, CWnd* pParent = NULL);   // standard constructor
	CDlgEditAttributes(long lFeature, long lFType, LPCSTR sFeature, CWnd* pParent = NULL);   // standard constructor

   ~CDlgEditAttributes();

   CDlgEditAttributes(long lFType, long lFeature, CMapLayerObj* pMapLayerObj, CWnd* pParent = NULL);   

protected:

   BOOL GetRowDateTime(long lRow, CDateTime&);
   BOOL LoadData(long lFeature, int& iRow);
   BOOL CreateGrid();
   BOOL DeleteRow(int iRow, BOOL bDelete = TRUE);
   int RetrieveRow(int iRow, CAttrArray& attr);
   void DisplayError(int iCol, int iRow, LPCSTR sError = NULL);
   long GetFeature(int iRow);   
   BOOL InitialiseCols();
   void DeleteCell(int iCol, int iRow);

   void InitMapObject();

   long m_lFType;
   CLongArray m_alFeatures;         
   CString m_sFeature;
   CMapLayerObj* m_pMapLayerObj;

   CArray <CDWordArray, CDWordArray> m_aFTypeLink;

// Dialog Data
	//{{AFX_DATA(CDlgEditAttributes)
	enum { IDD = IDD_EDITATTRIBUTES };	
	CBDProgressBar	m_ctlProgress;
	//}}AFX_DATA

	CNRDBGrid	m_Grid;   

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgEditAttributes)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgEditAttributes)
	virtual BOOL OnInitDialog();
	afx_msg void OnImport();
	afx_msg void OnAdd();	
	afx_msg void OnDestroy();
	afx_msg void OnDelete();
	virtual void OnOK();   
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnExport();
	afx_msg void OnInsert();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   LRESULT OnUpdateProgress(WPARAM, LPARAM);
   LRESULT OnResetProgress(WPARAM, LPARAM);
   LRESULT OnLeaveCell(WPARAM,LPARAM);
   LRESULT OnChangeCell(WPARAM,LPARAM);
   LRESULT OnDataChange(WPARAM,LPARAM);
   LRESULT OnSSMKeyDown(WPARAM, LPARAM);   
   LRESULT OnComboSelChange(WPARAM, LPARAM);   

   void OnEnter(NMHDR* pNMHDR, LRESULT* pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEDITATTRIBUTES_H__1C669AC4_144D_11D3_ADF1_44C1F2C00000__INCLUDED_)
