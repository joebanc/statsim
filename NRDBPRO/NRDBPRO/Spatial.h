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

#if !defined(AFX_SPATIAL_H__90BDD04B_E0D0_44AD_93C4_E1A3253DF5A3__INCLUDED_)
#define AFX_SPATIAL_H__90BDD04B_E0D0_44AD_93C4_E1A3253DF5A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////////////////
//
// Class for containing polygons or polylines for performing spatial operations
// Note, although CLongLines is used as base class, it is only as an array of
// coordinates and must not contain null coordinates
//

class CSpatialPolyObj
{
public:
   friend class CSpatialPoly;

   CSpatialPolyObj() {m_bHole = FALSE; m_nIndex = 0;}

protected:
   CRectEx m_rect; // bounding rect
   BOOL m_bHole; // hole punched
   int m_nIndex; // Index to start of line in CLongLines array
};

///////////////////////////////////////////////////////////////////////////////

class CSpatialPoly : public CLongLines
{
   friend class CSpatial;

public:
   CSpatialPoly() {m_bPolygon=-1;}
   CSpatialPoly(CLongLines *pLongLines);
   CSpatialPoly(CLongBinary&);
   operator CLongLines& () {return (CLongLines&)*this;}
   BOOL CalcArea(double& dArea);
   void GetCentroid(CLongCoord&, int index = -1);

   BOOL m_bPolygon;
   CRectEx m_rect; // bounding rect  

   CSpatialPolyObj GetInfo(int i) {return m_aObjInfo[i];}
   int GetNum() {return m_aObjInfo.GetSize();}
   void Initialise();
   BOOL IsPolygon() {return m_bPolygon;}

protected:
    
   CArray <CSpatialPolyObj, CSpatialPolyObj> m_aObjInfo;

   double CalcArea(int index);

};

///////////////////////////////////////////////////////////////////////////////
//
// Class for performing spatial operations

class CSpatial  
{
public:
	CSpatial();
	virtual ~CSpatial();

   static BOOL Inside(CLongCoord , const CLongLines&);
   static BOOL Inside(CLongCoord , const CLongLines&, int iStart);
   static BOOL Inside(const CLongLines& , const CLongLines&, int iStart1=0, int iStart2=0);
   static BOOL Intersects(const CLongLines& , const CLongLines&);
   static BOOL Within(CLongCoord , CLongCoord, int nDist);
   static BOOL Within(CLongCoord , const CLongLines&, int nDist);
   static BOOL Within(const CLongLines& , const CLongLines&, int nDist); 
   static BOOL GetCentroid(const CLongLines&, CCoord &rCoord); 

   // Internal functions

   static BOOL IsPolygon(CLongLines*);      
   static double CalcLength(CLongLines *pLongLines);

};

#endif // !defined(AFX_SPATIAL_H__90BDD04B_E0D0_44AD_93C4_E1A3253DF5A3__INCLUDED_)
