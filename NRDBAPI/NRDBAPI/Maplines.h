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

#ifndef _MAPLINES_H_
#define _MAPLINES_H_

#include "mapobject.h"
#include "coord.h"

///////////////////////////////////////////////////////////////////////////////

class DLLEXPORT CLongLines : public CArray <CLongCoord, CLongCoord>
{
public:
   CLongLines();
   CLongLines(CLongLines&);   

   CLongLines(CLongBinary&);
   BOOL GetLongBinary(CLongBinary&);   
   BOOL Initialise(CLongBinary&);   

   BOOL IsProtected() {return m_sProtected != "";}
   LPCSTR GetProtected() {return m_sProtected;}
   void SetProtected(LPCSTR s) {m_sProtected = s;}

protected:
   BOOL DecodeMaplines(BYTE* pData, DWORD dwLength);

   // Message for protected shapefiles
   CString m_sProtected;
};

///////////////////////////////////////////////////////////////////////////////

class DLLEXPORT CMapLines : public CMapObject
{
   DECLARE_DYNCREATE(CMapLines)

   CArray <CCoord, CCoord> m_aCoords;

   CCoord GetAt(int i) {return m_aCoords.GetAt(i);}
   int GetSize() {return m_aCoords.GetSize();}
   void SetSize(int i) {m_aCoords.SetSize(i);}
   void RemoveAt(int i, int nCount) {m_aCoords.RemoveAt(i, nCount);}
  
   CMapLines();
   CMapLines(CMapLines&);
   CMapLines& operator=(CMapLines&);

public:

   CMapLines(CLongBinary&);

   void GetLongBinary(CLongBinary&);
   void Initialise(CLongBinary&);

   void Add(CCoord coord) {m_aCoords.Add(coord);}
   void RemoveAt(int i) {m_aCoords.RemoveAt(i);}
   void InsertAt(int i, CCoord coord) {m_aCoords.InsertAt(i, coord);}
   void InsertAt(int i, CMapLines& maplines) {m_aCoords.InsertAt(i, &maplines.m_aCoords);}
   void SetAt(int i, CCoord& coord) {m_aCoords.SetAt(i, coord);}
};

#endif
