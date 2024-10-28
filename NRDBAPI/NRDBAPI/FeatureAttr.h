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
#if !defined(AFX_FEATUREATTR_H__82B59B20_142D_11D3_ADF1_44C1F2C00000__INCLUDED_)
#define AFX_FEATUREATTR_H__82B59B20_142D_11D3_ADF1_44C1F2C00000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FeatureAttr.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFeatureAttrSet recordset

class DLLEXPORT CFeatureAttrSet : public CRecordsetExt
{
public:
	CFeatureAttrSet(CAttrArray* pArray, CNRDBase* pDatabase = NULL);
   ~CFeatureAttrSet();
	DECLARE_DYNAMIC(CFeatureAttrSet)   

   enum {none, earliest, latest};
   
   LPLONG m_plDate;
   LPLONG m_plOrder;
   LPLONG m_plFeature;
   int  m_iFlag;
   CString m_sFType;
   long m_lFeatureMin;
   long m_lFeatureMax;
   long m_lStartDate;
   long m_lEndDate;
   int m_nDateCond;
   CString m_sFilter;      
   CNRDBase *m_pDBase;

protected:

// Field/Param Data
	//{{AFX_FIELD(CFeatureAttrSet, CRecordsetExt)
	long	m_FEATURE_ID;
	long	m_LDATE;
	long	m_ORDER;	
	//}}AFX_FIELD

   virtual CString GetFilter();   
   virtual CString GetSortOrder();
   virtual void DoDataExchange(int iFlag);
 
   CAttrArray* m_pArray;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFeatureAttrSet)
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

#endif // !defined(AFX_FEATUREATTR_H__82B59B20_142D_11D3_ADF1_44C1F2C00000__INCLUDED_)
