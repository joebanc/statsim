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

#if !defined(AFX_IMPORTFTYPE_H__3EE6DDA0_0D3D_11D5_A76B_0080AD88D050__INCLUDED_)
#define AFX_IMPORTFTYPE_H__3EE6DDA0_0D3D_11D5_A76B_0080AD88D050__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "importodbc.h"
#include "dlgprogress.h"
#include "projctns.h"

class CImportFType  
{
public:
	CImportFType(CImportDB* pImportDB, LPCSTR sDataSource);
	virtual ~CImportFType();

   int Import();

protected:

   BOOL ImportFTypes(long lFType);
   BOOL ImportFeatures(long lFType);
   BOOL ImportAttr(long lFType);

   int ImportMySQL();
   long ImportFTypeMySQL(CFeatureType ftype);
   void ImportFeatureMySQL(CFeatureType ftype, long lFType2);
   void ImportAttrMySQL(CFeatureType ftype, long lFType2);
   long DetermineFtype(HDBC hdbc, long lId, CString sFType = "");

   long GetFTypeLink(long lFTypeI, long lFType = 0);
   long GetFType(long lFType);

   BOOL ConvertProjection(CAttrArray&);
   void ConvertProjection(CLongCoord&);
   /*void ConvertProjection(CCoord&);*/
   CProjection m_projectionI;

   CImportDB* m_pImportDB;
   CString m_sDataSource;
   BDHANDLE m_hConnect;  
   CDlgProgress m_dlgProgress;
   long m_lFType;
   long m_lParentFType;
   CString m_sFType;
   CDatabase m_database;
   


};

#endif // !defined(AFX_IMPORTFTYPE_H__3EE6DDA0_0D3D_11D5_A76B_0080AD88D050__INCLUDED_)
