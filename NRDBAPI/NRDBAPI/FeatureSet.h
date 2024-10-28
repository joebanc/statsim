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

/////////////////////////////////////////////////////////////////////////////
// CFeatureSet recordset

class DLLEXPORT CFeatureSet : public CRecordsetExt
{
public:
	CFeatureSet(CNRDBase* pDatabase = NULL);
	DECLARE_DYNAMIC(CFeatureSet)

// Field/Param Data
	//{{AFX_FIELD(CFeatureSet, CRecordsetExt)
	long	m_ID;
	CNRDBString	m_Description;
	long m_Parent_Feature;
	//}}AFX_FIELD

	 CFeature* m_pFeature;    
    int    m_iFlag;	
    CString m_sTable;    

    void DoDataExchange(int iFlag);
    CString GetFilter();
    CString GetSortOrder();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFeatureSet)
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