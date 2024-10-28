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
// CAttributeTypeSet recordset

class DLLEXPORT CAttributeTypeSet : public CRecordsetExt
{
public:
	CAttributeTypeSet(CNRDBase* pDatabase = NULL);
	DECLARE_DYNAMIC(CAttributeTypeSet)

// Field/Param Data
	//{{AFX_FIELD(CAttributeTypeSet, CRecordsetExt)
   CNRDBString m_ColName;
	CNRDBString	m_Description;
	long	m_ID;
	long	m_Feature_Type_ID;
	long	m_Data_Type_ID;	
   long  m_FType_Link;
   BOOL  m_PrimaryKey;
	//}}AFX_FIELD

   CFTypeAttr* m_pFTypeAttr;   
   int m_iFlag;

   virtual CString GetFilter();   
   virtual CString GetSortOrder();
   virtual void DoDataExchange(int iFlag);
   
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAttributeTypeSet)
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
