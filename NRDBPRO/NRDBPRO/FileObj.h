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

#if !defined(AFX_FILEOBJ_H__31D4CD50_CCC2_11E1_B3BD_000795C2378F__INCLUDED_)
#define AFX_FILEOBJ_H__31D4CD50_CCC2_11E1_B3BD_000795C2378F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFileObj  
{
public:
	CFileObj();
	virtual ~CFileObj();

   BOOL Open(LPCSTR sFileName);
   BOOL Initialise(CLongBinary&);
   BOOL GetLongBinary(CLongBinary&);

   LPCSTR GetFileName() {return m_sFileName;}
   BYTE* GetData() {return m_aData.GetData();}
   size_t GetSize() {return m_aData.GetSize();}

protected:

   CString m_sFileName;
   CByteArray m_aData;

};

#endif // !defined(AFX_FILEOBJ_H__31D4CD50_CCC2_11E1_B3BD_000795C2378F__INCLUDED_)
