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
#if !defined(AFX_DOCBDREPORT_H__234AF772_3D6F_11D3_A9C4_9FA9021D9973__INCLUDED_)
#define AFX_DOCBDREPORT_H__234AF772_3D6F_11D3_A9C4_9FA9021D9973__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DocBDReport.h : header file
//

#include "docreport.h"
#include "query.h"

/////////////////////////////////////////////////////////////////////////////
// CDocBDReport document

class CDocBDReport : public CDocReport
{
protected:
	CDocBDReport();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDocBDReport)

   LPCSTR GetStatusText() {return m_sStatus;}
   CQueryResult* GetQueryResult() {return m_pQueryResult;}

// Attributes
protected:
   CQuery* m_pQuery;
   CQueryResult* m_pQueryResult;
   CString m_sStatus;
   CString m_sTitle;

// Operations
protected:

   CString FileAsHREF(CString);
   void ReportWizard();
   void WriteTextfile(FILE* pFile, CQueryResult*);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocBDReport)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual void OnCloseDocument();
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDocBDReport();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	virtual BOOL WriteReport();

protected:	

   BOOL WriteReportQuery(BOOL bTitle = TRUE);
   BOOL WriteHeaderQuery(CQueryResult*);
   BOOL WriteDataQuery(CQueryResult*);
   
	// Generated message map functions
protected:
	//{{AFX_MSG(CDocBDReport)		
	afx_msg void OnFileSaveAs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOCBDREPORT_H__234AF772_3D6F_11D3_A9C4_9FA9021D9973__INCLUDED_)
