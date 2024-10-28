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

#if !defined(AFX_GEOREFERENCE_H__D93C8A50_E754_11D4_A731_0080AD88D050__INCLUDED_)
#define AFX_GEOREFERENCE_H__D93C8A50_E754_11D4_A731_0080AD88D050__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////

#define PI 3.141592653

//////////////////////////////////////////////////////////////////////

class CGeoPoint
{
public:
   double m_dWX, m_dWY; // World
   double m_dRX, m_dRY; // Rotated
   int m_nSX, m_nSY;   // Screen
   int m_nIX, m_nIY;   // Screen inverted
};

//////////////////////////////////////////////////////////////////////

class CGeoReference  
{
public:
	CGeoReference();
	virtual ~CGeoReference();
   void Save(LPCSTR sFile);
   BOOL Load(LPCSTR sFile);

   BOOL Convert(POINT point, double& dX, double& dY);
   BOOL Convert(double dX, double dY, POINT& point);
   BOOL Initialise(int nWidth, int nHeight);

   double GetError();

   static void LatLongAsString(double deg, CString& s, int iFlag);

   enum {latitude=1, longitude};

   double GetRotation() {return m_r;}

   CArray <CGeoPoint, CGeoPoint> m_aPoints;   
   int m_nHeight;
   int m_nWidth;

protected:
   
   BOOL m_bInit;

   double m_aX, m_bX, m_aY, m_bY, m_cX, m_cY;
   double m_r;   

   void Rotate(int nX, int nY, double& dRX, double& dRY, BOOL bReverse = FALSE);
   BOOL LinearFit();
   BOOL Correction();
   void CorrectRotation();
   BOOL PolyFit();
   
};

#endif // !defined(AFX_GEOREFERENCE_H__D93C8A50_E754_11D4_A731_0080AD88D050__INCLUDED_)
