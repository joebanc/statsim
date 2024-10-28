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
#if !defined(AFX_DOCGRAPH_H__DFBFFE81_38F3_11D3_A9BA_E7179BA68B70__INCLUDED_)
#define AFX_DOCGRAPH_H__DFBFFE81_38F3_11D3_A9BA_E7179BA68B70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DocGraph.h : header file
//

#include "longarray.h"
#include "query.h"

/////////////////////////////////////////////////////////////////////////////
// CDocGraph document

class CDocGraph : public CDocument
{
public: 

   BOOL GetFirstData(int iLine, double&, CString& sLabel);
   BOOL GetNextData(int iLine, double&, CString& sLabel);   
   LPCSTR GetTitle() {return m_sTitle;}

   virtual CString GetLegend(int iLine);
   virtual int GetNumLines();   


protected:
	CDocGraph();           // protected constructor used by dynamic creation   
	DECLARE_DYNCREATE(CDocGraph)

   long GetFType() {return m_lFType;}
   CLongArray& GetFeatures() {return m_alFeatures;}
   CStringArray& GetFNames() {return m_asFeatures;}
   CLongArray& GetAttr() {return m_alAttr;}

   virtual BOOL GetFirstDataQuery(int iLine, double&, CString& sLabel);
   virtual BOOL GetNextDataQuery(int iLine, double&, CString& sLabel);   

// Attributes

   long m_lFType;
   CLongArray m_alFeatures;  
   CStringArray m_asFeatures;
   CLongArray m_alAttr;     
   int m_iFeature;
   CDateTime m_datetime;
   long m_lAttrId;
   CAttrArray m_aAttr;   
   BOOL m_bFound;
   CString m_sTitle;
   int m_nType;
   BOOL m_bQuery;

   CQueryResult* m_pQueryResult;
   CArrayAttrSel m_aAttrSel;
   CQuery* m_pQuery;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocGraph)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDocGraph();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CDocGraph)
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOCGRAPH_H__DFBFFE81_38F3_11D3_A9BA_E7179BA68B70__INCLUDED_)
