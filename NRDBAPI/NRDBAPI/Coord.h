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
// NRDB Pro is part of the Natural Resources Database 
// 
// Homepage: http://www.nrdb.co.uk/
// 

#ifndef _COORD_H_
#define _COORD_H_

#include <float.h>
#include <math.h>
#include "const.h"
#include "mapobject.h"

#define NULL_READING AFX_RFX_DOUBLE_PSEUDO_NULL
#define NULL_READING_OLD DBL_MAX

//define export
#ifndef DLLEXPORT
	#define DLLEXPORT __declspec(dllexport)
#endif
#ifndef DLLIMPORT
	#define DLLIMPORT __declspec(dllimport)
#endif

inline BOOL IsNullDouble(double d) {return fabs(d - AFX_RFX_DOUBLE_PSEUDO_NULL) < fabs(AFX_RFX_DOUBLE_PSEUDO_NULL/1e5);};

///////////////////////////////////////////////////////////////////////////////

class DLLEXPORT CLongCoord
{
public:
   CLongCoord();
   CLongCoord(CLongCoord&);
   CLongCoord(long x, long y) {this->x = x; this->y = y;};
   CLongCoord& operator=(const CLongCoord& rSrc) {x = rSrc.x; y = rSrc.y; return *this;};

   BOOL operator==(CLongCoord&);
   BOOL operator!=(CLongCoord&);
   
   long x,y;

   BOOL IsNull() {return x == AFX_RFX_LONG_PSEUDO_NULL;}
   void SetNull() {x = AFX_RFX_LONG_PSEUDO_NULL; y = 0;}
   
   // For polylines, end of all parts
   void SetEOL() {x = AFX_RFX_LONG_PSEUDO_NULL; y = AFX_RFX_LONG_PSEUDO_NULL;} 
   BOOL IsEOL() {return y == AFX_RFX_LONG_PSEUDO_NULL;}
};

///////////////////////////////////////////////////////////////////////////////

class DLLEXPORT CCoord : public CMapObject
{
public:

   CCoord();
   CCoord(CCoord&);
   CCoord(double x1, double y1) {x=x1;y=y1;}
   CCoord& operator=(CCoord&);   

   BOOL operator==(CCoord&);
   BOOL operator!=(CCoord&);
   
   double x,y;

   BOOL IsNull() {return fabs(x-NULL_READING) < COMPARENULLDOUBLE || x == NULL_READING_OLD;}
   void SetNull() {x = NULL_READING; y = 0;}
   
   // For polylines, end of all parts
   void SetEOL() {x = NULL_READING; y = NULL_READING;} 
   BOOL IsEOL() {return y == NULL_READING || y == NULL_READING_OLD;}
};


///////////////////////////////////////////////////////////////////////////////

class CCoordArray : public CArray <CCoord, CCoord>
{
};

class CLongCoordArray : public CArray <CLongCoord, CLongCoord>
{
};

#endif
