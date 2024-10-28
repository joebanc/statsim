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

#include "stdafx.h"
#include "GeoReference.h"
#include "float.h"
#include "math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

#define COUNT_MAX 100

//////////////////////////////////////////////////////////////////////

inline double square(double d) {return d*d;}

//////////////////////////////////////////////////////////////////////

double inline GetFactor(double dErrorMax, double dError, int nCount) 
{
   double d = pow(10,COUNT_MAX/2 - nCount) * (0.5 - rand()/(double)RAND_MAX) * dError / dErrorMax;
   
   return d;
}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoReference::CGeoReference()
{
   m_bInit = FALSE;
   m_nHeight = 0;
   m_nWidth = 0;
}

CGeoReference::~CGeoReference()
{

}

//////////////////////////////////////////////////////////////////////
//
// Save references to file .nrg
//

void CGeoReference::Save(LPCSTR sFile)
{
   CString s = sFile;
   s = s.Left(s.ReverseFind('.')) + ".nrg";   

   FILE* pFile = fopen(s,"w");
   if (pFile != NULL)
   {
      int i = 0; for (i = 0; i < m_aPoints.GetSize(); i++)
      {
         fprintf(pFile, "%lf,%lf,%i,%i\n",m_aPoints[i].m_dWX,m_aPoints[i].m_dWY, 
                 m_aPoints[i].m_nSX, m_aPoints[i].m_nSY);
      }
      fclose(pFile);
   }
}

//////////////////////////////////////////////////////////////////////

BOOL CGeoReference::Load(LPCSTR sFile)
{   
   BOOL bOK = TRUE;

   int nRet;
   CString s = sFile;
   s.SetAt(s.GetLength()-1, 'w');   

   s = s.Left(s.ReverseFind('.')) + ".nrg";

   m_aPoints.RemoveAll();
   CGeoPoint point;

   FILE* pFile = fopen(s,"r");
   if (pFile != NULL)
   {      
      while ((nRet = fscanf(pFile, "%lf,%lf,%d,%d",&point.m_dWX, &point.m_dWY, 
                    &point.m_nSX, &point.m_nSY)) == 4)
      {          
         m_aPoints.Add(point);           
      };
      if (nRet != EOF) m_aPoints.RemoveAll();

      fclose(pFile);
   } else
   {
      bOK = FALSE;
   }
   m_bInit = FALSE;

   return bOK;
}

//////////////////////////////////////////////////////////////////////
//
// Converts screen coordinates to world coordinates using the 
// georeferences points
//

BOOL CGeoReference::Convert(POINT point, double& dX, double& dY)
{
   if (!m_bInit)
   {
      return FALSE;
   };

  
   double dRX, dRY;
   Rotate(point.x, m_nHeight - point.y, dRX, dRY);

   dX = m_aX + m_bX * dRX + m_cX * dRY;
   dY = m_aY + m_bY * dRY + m_cY * dRX;

   return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
// Converts world coordinates to image coordinates using the 
// georeference points
//

BOOL CGeoReference::Convert(double dX, double dY, POINT& point)
{      
   if (!m_bInit)
   {
      return FALSE;
   }
  
   // First reverse x' = a + bx + cy

   double dRY = (m_cY * (dX - m_aX) - m_bX * (dY - m_aY)) / (m_cX * m_cY - m_bX * m_bY);
   double dRX = (dX - m_aX - m_cX * dRY) / m_bX;

   // Now reverse the rotation

   double x,y;
   Rotate((int)dRX, (int)dRY, x, y, TRUE);
   point.x = (int)x;
   point.y = (int)(m_nHeight-y);
        
   return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
// XW = ax + bx * XS
// YW = ay + by * YS 
//
// => a = XS0 * XW0 - XS0 * XW1
//        ----------------------
//             XS1 - XS0
//
// => b = W0 - W1
//        -------
//        S0 - S1
//
// Determine parameters
//

BOOL CGeoReference::Initialise(int nWidth, int nHeight)
{    
   double r;
   double rT = 0;
   int nR = 0;
   double rW, rS;   

   m_cX = 0;
   m_cY = 0;

   // Used in calculated rotation etc.

   m_nHeight = nHeight;
   m_nWidth = nWidth;

// Cause GP fault logo on Win98

   if (m_aPoints.GetSize() == 0) return FALSE;

// Normalize the x,y points wrt the map

   int i = 0; for (i = 0; i < m_aPoints.GetSize(); i++)
   {
      m_aPoints[i].m_nIX = m_aPoints[i].m_nSX;
      m_aPoints[i].m_nIY = m_nHeight - m_aPoints[i].m_nSY;
   }

// Determine the rotation

   for (i = 0; i < m_aPoints.GetSize(); i++)
   {
      for (int j = i+1; j < m_aPoints.GetSize(); j++)
      {         
         rW = -PI/2;
         rS = -PI/2;

         if (m_aPoints[i].m_dWY !=  m_aPoints[j].m_dWY)
         {
            rW = atan((m_aPoints[i].m_dWX - m_aPoints[j].m_dWX) / (m_aPoints[i].m_dWY - m_aPoints[j].m_dWY));
         }               
         if (m_aPoints[j].m_nIY != m_aPoints[i].m_nIY)
         {            
            rS = atan((m_aPoints[i].m_nIX - m_aPoints[j].m_nIX) / ((double)m_aPoints[i].m_nIY - m_aPoints[j].m_nIY));       
         } 

         r = rW - rS;
         if (r > PI/2) r -= PI;
         if (r < -PI/2) r += PI;

         rT += r; 
         nR++;
      }
   }
   m_r = rT/nR;

   CorrectRotation();

// Fit now
   
   if (LinearFit())
   {
   } else
   {
      return FALSE;
   }

   return TRUE;
}

//////////////////////////////////////////////////////////////////////

double CGeoReference::GetError()
{
     // Calculate error
   double dError = 0, dX, dY;
   int i = 0; for (i = 0; i < m_aPoints.GetSize(); i++)
   {
      Convert(CPoint(m_aPoints[i].m_nSX, m_aPoints[i].m_nSY), dX, dY);
      dError += square(m_aPoints[i].m_dWX - dX);
      dError += square(m_aPoints[i].m_dWY - dY);      
   }
   dError /= m_aPoints.GetSize()*2;

   return dError;
}

//////////////////////////////////////////////////////////////////////

BOOL CGeoReference::LinearFit()
{
   double aX, bX, aY, bY;
   double aXT, bXT, aYT, bYT;
   double dNX, dNY;   

   aXT = bXT = aYT = bYT = 0;
   dNX = dNY = 0;

   // Rotate the screen coordinates

   int i = 0; for (i = 0; i < m_aPoints.GetSize(); i++)
   {      
      Rotate(m_aPoints[i].m_nIX, m_aPoints[i].m_nIY, m_aPoints[i].m_dRX, m_aPoints[i].m_dRY);
   }  

   // Determine the parameters by using pairs of coordinates

   for (i = 0; i < m_aPoints.GetSize(); i++)
   {
      for (int j = i+1; j < m_aPoints.GetSize(); j++)
      {         

         // Determine linear transformation

         aX = (m_aPoints[j].m_dRX * m_aPoints[i].m_dWX - m_aPoints[i].m_dRX * m_aPoints[j].m_dWX) /
              (m_aPoints[j].m_dRX - m_aPoints[i].m_dRX);

         bX = (m_aPoints[j].m_dWX - m_aPoints[i].m_dWX) / (m_aPoints[j].m_dRX - m_aPoints[i].m_dRX);

         aY = (m_aPoints[j].m_dRY * m_aPoints[i].m_dWY - m_aPoints[i].m_dRY * m_aPoints[j].m_dWY) /
              (m_aPoints[j].m_dRY - m_aPoints[i].m_dRY);

         bY = (m_aPoints[j].m_dWY - m_aPoints[i].m_dWY) / (m_aPoints[j].m_dRY - m_aPoints[i].m_dRY);         

         // Determine totals
         // Bias mean towards larger displacements

         if (bX != 0)
         {           
            double d = fabs(m_aPoints[j].m_dRX - m_aPoints[i].m_dRX);

            dNX += d;
            aXT += aX*d;
            bXT += bX*d;
            
         }
         if (bY != 0)
         {
            double d = fabs(m_aPoints[j].m_dRY - m_aPoints[i].m_dRY);                       

            dNY += d;
            aYT += aY * d;
            bYT += bY * d;
         }
      }
   }

   // Determine average

   if (dNX > 0 && dNY > 0)
   {
      m_aX = aXT/dNX;
      m_bX = bXT/dNX;
      m_aY = aYT/dNY;
      m_bY = bYT/dNY;
      m_bInit = TRUE;

      return TRUE;
   } else
   {
      return FALSE;
   }
}

///////////////////////////////////////////////////////////////////////////////

void CGeoReference::CorrectRotation()
{   
   double r = m_r;

   // Correction rotation first

   double rT = 0;
   int nCount = 0;
   
   m_r = rT;
   LinearFit();
   double dError = GetError();
   m_r = r;    
   LinearFit();
   double dError1 = GetError();
   
   if (dError < dError1)
   {
      r = m_r;
      rT = (m_r - rT) / (dError - dError1);
   }           
   nCount++;   
   m_r = r;
}

//////////////////////////////////////////////////////////////////////
//
// Rotates nX, nY by m_r radians about the origin
// Returns dRX, dRY
//

void CGeoReference::Rotate(int nSX, int nSY, double& dRX, double& dRY, BOOL bReverse)
{
   double r = m_r;
   if (bReverse) 
   {
      r = -r;
   } 

   // Handle case where no rotation

   if (r == 0) {dRX = nSX; dRY = nSY; return;};

   double l = sqrt(square(nSX) + square(nSY));
   double degrees = r + PI/2;
   if (nSY != 0)
   {
      degrees = atan(nSX / (double)nSY) + r;
   }

   dRX = sin(degrees) * l;
   dRY = cos(degrees) * l;      


   
}

///////////////////////////////////////////////////////////////////////////////

void CGeoReference::LatLongAsString(double deg, CString& s, int iFlag)
{
   int hour,min;
   double dSec;

   char dir;
   hour = abs((int)deg);
   min = abs((int)((fabs(deg)-hour)*60));
   dSec = fabs((((fabs(deg)-hour)*60)-min)*60);

   if (fabs(dSec - 60) < 0.000001) 
   {
      min += 1;
      dSec = 0;
   }

   if (deg >= 0 && (iFlag & latitude) == latitude) dir = 'N';
   else if (deg < 0 && (iFlag & latitude) == latitude) dir = 'S';
   else if (deg >= 0 && (iFlag & longitude) == longitude) dir = 'E';
   else if (deg < 0 && (iFlag & longitude) == longitude) dir = 'W';
   else ASSERT(FALSE);

   s.Format("%d'%02d\'%.1lf\" %c",hour,min,dSec,dir);   
}
