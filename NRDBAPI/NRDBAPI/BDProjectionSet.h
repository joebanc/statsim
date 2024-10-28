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
#if !defined(AFX_BDPROJECTIONSET_H__FDDB3120_4140_11D4_A590_0080AD88D050__INCLUDED_)
#define AFX_BDPROJECTIONSET_H__FDDB3120_4140_11D4_A590_0080AD88D050__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BDProjectionSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBDProjectionSet recordset

class DLLEXPORT CBDProjectionSet : public CRecordsetExt
{
public:
	CBDProjectionSet(CNRDBase* pDatabase = NULL);
	DECLARE_DYNAMIC(CBDProjectionSet)

   virtual CString GetFilter();
   virtual void DoDataExchange(int iFlag);

   CBDProjection *m_pProjection;   
   CNRDBase *m_pDBase;
   int m_iFlag;

// Field/Param Data
	//{{AFX_FIELD(CBDProjectionSet, CRecordsetExt)
	long	m_ID;
	CNRDBString	m_Name;
	double	m_Semi_major_axis;
	double	m_Semi_minor_axis;
	double	m_Latitude_origin;
	double	m_Longitude;
	double	m_False_easting;
	double	m_False_northing;
	double	m_Scale_factor_at_origin;
   long     m_ScaleCoords;
   double   m_dMinEasting;
   double   m_dMaxEasting;
   double   m_dMinNorthing;
   double   m_dMaxNorthing;
	BOOL	m_Default;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBDProjectionSet)
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

#endif // !defined(AFX_BDPROJECTIONSET_H__FDDB3120_4140_11D4_A590_0080AD88D050__INCLUDED_)
