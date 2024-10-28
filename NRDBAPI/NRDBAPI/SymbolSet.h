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
#if !defined(AFX_SYMBOLSET_H__79C73A21_EEBC_11E1_B3FE_000795C2378F__INCLUDED_)
#define AFX_SYMBOLSET_H__79C73A21_EEBC_11E1_B3FE_000795C2378F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SymbolSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSymbolSet recordset

class DLLEXPORT CSymbolSet : public CRecordsetExt
{
public:
	CSymbolSet(CNRDBase* pDatabase = NULL);
	DECLARE_DYNAMIC(CSymbolSet)

   virtual CString GetFilter();
   virtual void DoDataExchange(int iFlag);

   CBDSymbol *m_pSymbol;
   int m_iFlag;

// Field/Param Data
	//{{AFX_FIELD(CSymbolSet, CRecordsetExt)
	long	m_ID;
	CLongBinary	m_Image;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSymbolSet)
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

#endif // !defined(AFX_SYMBOLSET_H__79C73A21_EEBC_11E1_B3FE_000795C2378F__INCLUDED_)
