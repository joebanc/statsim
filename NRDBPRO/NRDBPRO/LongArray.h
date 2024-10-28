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

#if !defined(AFX_LONGARRAY_H__C8F92BE0_04F1_11D5_B7A8_EDA55A1FBF43__INCLUDED_)
#define AFX_LONGARRAY_H__C8F92BE0_04F1_11D5_B7A8_EDA55A1FBF43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLongArray : public CArray <long, long>  
{
public:
	CLongArray();
	virtual ~CLongArray();

   int Find(long);
   void Sort();
   int FindSorted(long);

protected:
   
};

#endif // !defined(AFX_LONGARRAY_H__C8F92BE0_04F1_11D5_B7A8_EDA55A1FBF43__INCLUDED_)
