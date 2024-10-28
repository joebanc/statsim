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

#ifndef _PROJCTNS_H_
#define _PROJCTNS_H_

#include "xmlfile.h"

///////////////////////////////////////////////////////////////////////////////

#define UTM_SCALEFACTOR 0.9996
#define UTM_INDEX 0
#define LATLON_ID -2
#define PR_LATLON 1
#define PR_COORD 2
#define LATLONSCALE 1000000
#define UTM_ID_OLD -1

////////////////////////////////////////////////////////////////////////////////
//
// Function prototypes
//

class CProjection : public CBDProjection
{
public:

   CProjection();

   enum {latitude=1, longitude=2, legend=4, import=8, seconds=16};

   void TransMeractorInit(void);

   BOOL InitialiseProjection(BDHANDLE hConnect = NULL);   

   void LatLonToTransMercator(double latitude, double longitude, 
                              long *easting, long *northing);
   void LatLonToTransMercator(double latitude, double longitude, 
                              double *easting, double *northing);
   
   void TransMercatorToLatLon(double easting, double northing,
                              double *latitude, double *longitude);

   BOOL StringAsLatLong(LPCSTR s, double* d, int iFlag);
   void LatLongAsString(double d, CString& s, int iFlag);

   int StringAsCoord(CString s, CLongCoord*); // used for polylines
   int StringAsCoord(CString s, CCoord*); // used for coordinates
  
   void CoordAsString(CCoord coord, CString& s);

   BOOL StringAsBearing(CString sT, CLongCoord &coord);
  
   CString GetProjectionName();

   CBDProjection& GetProjection();

   BOOL IsUTMOld();
   BOOL IsLatLon();

   BOOL IsDefaultProjection() {return m_nID != 0;}
   double GetDistance(CLongCoord coord1, CLongCoord coord2, BOOL bDefaultUnits = FALSE);   

protected:

   void TransMercatorToLatLonInt(double easting, double northing,
                              double *latitude, double *longitude);
   void LatLonToTransMercatorInt(double latitude, double longitude, 
                              double *easting, double *northing);
   


   // Pre-calculated parameters

   double m_a, m_b, m_e2;
   double m_N, m_pow2N, m_pow3N;    
   
   BOOL StringAsBearing(CString sT, CCoord &coord);


};

///////////////////////////////////////////////////////////////////////////////

class CUnit
{
public:

   CString m_sName;
   CString m_sAbbr;
   double m_dScaleFactor;
   BOOL m_bDefault;
   int m_nType;

   CUnit() {m_dScaleFactor = 1; m_bDefault = FALSE; m_nType = 0;}
};

class CUnits : public CArray <CUnit, CUnit>
{

public:

   enum {length=1, area=2};

   void LoadUnits();
   void SaveUnits();
   void SetDefault(int nType, LPCSTR sName);

   CUnit GetAreaUnit();
   CUnit GetLengthUnit();

protected:

   void AsXML(CXMLObj*);
   void XMLAs(CXMLObj*);
};

///////////////////////////////////////////////////////////////////////////////
//
// Elipsoid Codes
//

struct ELLIPSOID
{
   char *m_psName;
   double m_dSemiMajorAxis;   
   double m_dRF; // Reciprocal of flattening   
};

class CEllipsoid : public ELLIPSOID
{
public:

   CEllipsoid(ELLIPSOID& rSrc) {(ELLIPSOID&)*this = rSrc;}

   static ELLIPSOID m_aEllipsoids[68];

   LPCSTR GetName() {return m_psName;}
   double GetSemiMajorAxis() {return m_dSemiMajorAxis;}
   double GetSemiMinorAxis();
};

///////////////////////////////////////////////////////////////////////////////

#endif // _PROJCTNS_H_
