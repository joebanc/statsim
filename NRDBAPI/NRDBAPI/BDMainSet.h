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
#if !defined(AFX_BDMAINSET_H__A1B438A0_039D_11D4_AB44_0080AD88D050__INCLUDED_)
#define AFX_BDMAINSET_H__A1B438A0_039D_11D4_AB44_0080AD88D050__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BDMainSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBDMainSet recordset

class DLLEXPORT CBDMainSet : public CRecordsetExt
{
public:
	CBDMainSet(CNRDBase* pDatabase = NULL);
	DECLARE_DYNAMIC(CBDMainSet)

   virtual CString GetFilter();
   virtual void DoDataExchange(int iFlag);

   CBDMain* m_pMain;   
   int m_iFlag;
   int* m_pnDBVersion;

// Field/Param Data
	//{{AFX_FIELD(CBDMainSet, CRecordsetExt)
	CNRDBString	m_Country;
	CNRDBString	m_Province;
	CNRDBString	m_City;
	CNRDBString	m_Organization;
	CNRDBString	m_Address;
	CNRDBString	m_Telephone;
	CNRDBString	m_Fax;
	CNRDBString	m_Email;
   CNRDBString	m_Logo;
   BOOL     m_CoordAsLatLon;
   int      m_Version;
   BOOL     m_UseCustomHeader;
   BOOL     m_UseCustomFooter;
   CNRDBString  m_CustomHeader;
   CNRDBString  m_CustomFooter;
   CNRDBString  m_StyleSheet;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBDMainSet)
	public:
	virtual CString GetDefaultConnect();    // Default connection string
	virtual CString GetDefaultSQL();    // Default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);  // RFX support
	//}}AFX_VIRTUAL

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BDMAINSET_H__A1B438A0_039D_11D4_AB44_0080AD88D050__INCLUDED_)
