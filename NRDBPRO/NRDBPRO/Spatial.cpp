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
// TODO optimise using polygon extent
//

#include "stdafx.h"
#include "nrdbpro.h"
#include "Spatial.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSpatial::CSpatial()
{

}

CSpatial::~CSpatial()
{

}

///////////////////////////////////////////////////////////////////////////////

CSpatialPoly::CSpatialPoly(CLongBinary& longbinary) : CLongLines(longbinary)
{
	Initialise();
}

///////////////////////////////////////////////////////////////////////////////

CSpatialPoly::CSpatialPoly(CLongLines *pMapLines)
{
   Copy(*pMapLines);

   Initialise();
}

///////////////////////////////////////////////////////////////////////////////

void CSpatialPoly::Initialise()
{
   CLongCoord coord;

   // Determine if this is a polygon

   m_bPolygon = CSpatial::IsPolygon(this);

   // Determine the extent of each polygon
   // TODO check that top and bottom are not reversed for lat/long in southern hemisphere

   int i = 0, j = 0;

   int i = 0; for (i = 0; i < GetSize(); i++)
   {      
	  if (j+1 > m_aObjInfo.GetSize()) m_aObjInfo.SetSize(j+1);
	  CRectEx &rRect = m_aObjInfo[j].m_rect;

	  if (!GetAt(i).IsNull())
      {
         if (rRect.IsEmpty())
         {
            rRect.left = GetAt(i).x;
            rRect.right = GetAt(i).x;
            rRect.top = GetAt(i).y;
            rRect.bottom = GetAt(i).y;

			m_aObjInfo[j].m_nIndex = i;
         } else
         {
            rRect.left = min(GetAt(i).x, rRect.left);
            rRect.right = max(GetAt(i).x,rRect.right);
            rRect.top = max(GetAt(i).y, rRect.top);
            rRect.bottom = min(GetAt(i).y,rRect.bottom);
         }
      } else
	  {
		  j=j+1;
	  }

      // Determine overall extent

      if (i == 0)
      {
         m_rect.left = rRect.left;
         m_rect.right = rRect.right;
         m_rect.top = rRect.top;
         m_rect.bottom = rRect.bottom;
      } else
      {
         m_rect.left = min(rRect.left,m_rect.left);
         m_rect.right = max(rRect.right,m_rect.right);
         m_rect.top = max(rRect.top,m_rect.top);
         m_rect.bottom = min(rRect.bottom,m_rect.bottom);
      }
   }
     
   // Set initial value

   for (i = 0; i < m_aObjInfo.GetSize(); i++)
   {
      m_aObjInfo[i].m_bHole = FALSE;
   };

   // Determine hole punched polygons.  Assumes that holes will always follow
   // the polygon they are inside

   for (i = 0; m_bPolygon && i < m_aObjInfo.GetSize(); i++)
   {
	   for (j = i+1; j < m_aObjInfo.GetSize(); j++)
	   {
		  // Check that neither object is already considered a hole

		  if (!m_aObjInfo[i].m_bHole && !m_aObjInfo[j].m_bHole)
		  {
		  // Compare extents first

			 if (m_aObjInfo[i].m_rect.left <= m_aObjInfo[j].m_rect.left && 
				 m_aObjInfo[i].m_rect.right >= m_aObjInfo[j].m_rect.right && 
				 m_aObjInfo[i].m_rect.top >= m_aObjInfo[j].m_rect.top && 
				 m_aObjInfo[i].m_rect.bottom <= m_aObjInfo[j].m_rect.bottom)
			 {
				// If the centroid of the polygon is inside the larger polygon then assume this to be a hole

             GetCentroid(coord, j);
             m_aObjInfo[j].m_bHole = CSpatial::Inside(coord, *this, m_aObjInfo[i].m_nIndex);
             
					 /*!!!!!
             m_aObjInfo[j].m_bHole = CSpatial::Inside(*this, *this, m_aObjInfo[j].m_nIndex, m_aObjInfo[i].m_nIndex);
             */
				
			 }
		  }
	  }

   }
}

///////////////////////////////////////////////////////////////////////////////
//
// Returns true if the point occurs inside the polygon
//

BOOL CSpatial::Inside(CLongCoord coord, const CLongLines &longlines)
{   
    int i, j, c = 0;

  // Search through each polygon

    int i1 = 0;
 
    for (int i2 = 0; i2 < longlines.GetSize(); i2++)
    {
       CLongCoord& xy0 = (CLongCoord&) longlines.GetAt(i2);

	   if (xy0.IsNull() || i2+1 == longlines.GetSize())
       {
          for (i = i1, j = i2-1; i < i2; j = i++) 
          {
              CLongCoord& coord1 = (CLongCoord&) longlines.GetAt(i);
              CLongCoord& coord2 = (CLongCoord&) longlines.GetAt(j);
              if (
                   (
                     ((coord1.y <= coord.y)  && (coord.y < coord2.y )) || 
                     ((coord2.y <= coord.y) && (coord.y < coord1.y))
                   ) &&
                   (coord.x < (coord2.x - coord1.x) * (coord.y - coord1.y) / (coord2.y- coord1.y) + coord1.x))
	        
                c = !c;
          }
          // Return true if inside any polygon
          if (c) return c;

          i1 = i2+1;
       };
    };
    return c;
}

///////////////////////////////////////////////////////////////////////////////
// 
// Search for a point within a single polygon
//

BOOL CSpatial::Inside(CLongCoord coord, const CLongLines& longlines, int iStart)
{
   int c= 0;	

   // Find the end of the polygon

   int i = 0, i2 = 0,
	   j = 0;

   for (i2 = 0; i2 < longlines.GetSize(); i2++)
   {
       CLongCoord& xy0 = (CLongCoord&) longlines.GetAt(i2);
       if (xy0.IsNull()) break;
   };
     
   for (i = iStart, j = i2-1; i < i2	; j = i++) 
   {
      CLongCoord& coord1 = (CLongCoord&) longlines.GetAt(i);
      CLongCoord& coord2 = (CLongCoord&) longlines.GetAt(j);
      if (
           (
             ((coord1.y <= coord.y)  && (coord.y < coord2.y )) || 
             ((coord2.y <= coord.y) && (coord.y < coord1.y))
           ) &&
           (coord.x < (coord2.x - coord1.x) * (coord.y - coord1.y) / (coord2.y- coord1.y) + coord1.x))
	
        c = !c;
   }
  // Return true if inside any polygon
   return c;

}

///////////////////////////////////////////////////////////////////////////////

BOOL CSpatial::Inside(const CLongLines& longlines1, const CLongLines& longlines2, int iStart1, int iStart2)
{
   BOOL bInside = FALSE;
   CLongCoord coord;

   // Return true if all points from longlines1 are inside longlines2
   for (int i = iStart1; i < longlines1.GetSize() && !((CLongCoord&)longlines1[i]).IsNull(); i++)
   {
      if (!((CLongCoord&)longlines1[i]).IsNull())
      {
         bInside = TRUE;
         
         coord.x = longlines1[i].x;
         coord.y = longlines1[i].y;
         if (!Inside(coord, longlines2, iStart2)) return FALSE;
      }
   }
   
   return bInside;

}

///////////////////////////////////////////////////////////////////////////////

BOOL CSpatial::Intersects(const CLongLines& longlines1, const CLongLines& longlines2)
{
   // Returns true if a vertex from either polygon lies inside the other
   
   CLongCoord coord;
   int i = 0; for (i = 0; i < longlines1.GetSize(); i++)
   {
      if (!((CLongCoord&)longlines1[i]).IsNull())
      {                  
         coord.x = longlines1[i].x;
         coord.y = longlines1[i].y;
         if (Inside(coord, longlines2)) return TRUE;
      }
   }
   return FALSE;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CSpatial::Within(CLongCoord coord1, CLongCoord coord2, int nDist)
{
   CLongCoord longcoord1, longcoord2;

   longcoord1.x = (int)coord1.x;
   longcoord1.y = (int)coord1.y;
   longcoord2.x = (int)coord2.x;
   longcoord2.y = (int)coord2.y;
   return BDProjection()->GetDistance(longcoord1, longcoord2) < nDist;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CSpatial::Within(CLongCoord coord1, const CLongLines& longlines2, int nDist)
{
   CLongCoord coord2;

  // Return true if inside

  if (Inside(coord1, longlines2)) return TRUE;

  // Returns true if coord is within distance of any vertex of longlines 

  int i = 0; for (i = 0; i < longlines2.GetSize(); i++)
  {
      if (!((CLongCoord&)longlines2[i]).IsNull())
      {
         coord2.x = longlines2[i].x;
         coord2.y = longlines2[i].y;
         if (Within(coord1, coord2, nDist)) return TRUE; 
      }
  };
  return FALSE;
    
}

///////////////////////////////////////////////////////////////////////////////

BOOL CSpatial::Within(const CLongLines& longlines1, const CLongLines& longlines2, int nDist)
{
   CLongCoord coord;

   // Returns true if any vertex is within nDist of any vertex

   int i = 0; for (i = 0; i < longlines1.GetSize(); i++)
  {
      if (!((CLongCoord&)longlines1[i]).IsNull())
      {
         coord.x = longlines1[i].x;
         coord.y = longlines1[i].y;
         if (Within(coord, longlines2, nDist)) return TRUE; 
      }
  };
  return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Calculates the area of a polygon
//

BOOL CSpatialPoly::CalcArea(double& dTotalArea)
{
   double dArea = 0;
   
   dTotalArea = 0;

   if (!m_bPolygon) return FALSE;

   // Determine the area of each polygon

   int i = 0; for (i = 0; i < m_aObjInfo.GetSize(); i++)
   {
      dArea = CalcArea(m_aObjInfo[i].m_nIndex);      

      if (m_aObjInfo[i].m_bHole) 
      {
         dTotalArea -= fabs(dArea);
      }
      else 
      {
         dTotalArea += fabs(dArea);
      };
   }   

   dTotalArea *= BDGetApp()->GetUnits().GetAreaUnit().m_dScaleFactor;

   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

double CSpatialPoly::CalcArea(int index)
{
   double dArea = 0;
   for (int i = index; i < GetSize()-1 && !GetAt(i+1).IsNull(); i++)
   {
      dArea += (double)(GetAt(i).x) * (double)(GetAt(i+1).y) - 
               (double)(GetAt(i+1).x) * (double)(GetAt(i).y);

      ASSERT(!GetAt(i).IsNull() && !GetAt(i+1).IsNull());
   }
   dArea=dArea/2;

   return dArea; // Do not remove sign as used in calculating centroid
}

///////////////////////////////////////////////////////////////////////////////
//
// Determines the geometric centroid of the largest polygon (by extent)
// To determine the centroid of a specific polygon use the index argument
//

void CSpatialPoly::GetCentroid(CLongCoord& coord, int index)
{
   coord.SetNull();
   if (GetSize() == 0) return;

	// Find the largest polygon

	int iObj = -1;
   if (index == -1)
   {
	   double dMaxExtent=0, dExtent;

	   int i = 0; for (i = 0; i < m_aObjInfo.GetSize(); i++)
	   {
		   //dExtent = m_aObjInfo[i].m_rect.Area();
         dExtent = ((double)m_aObjInfo[i].m_rect.right - (double)m_aObjInfo[i].m_rect.left)*((double)m_aObjInfo[i].m_rect.top - (double)m_aObjInfo[i].m_rect.bottom);
		   if (dExtent > dMaxExtent)
		   {
			   iObj = i;
			   dMaxExtent = dExtent;
		   }
	   }

      ASSERT(iObj != -1);
      if (iObj == -1) return;
   } else
   {
      iObj = index;
   }

	// Now determine the area of the polygon

	double dArea = CalcArea(m_aObjInfo[iObj].m_nIndex);

	// Now determine the centroid

	double x = 0;
	double y = 0;
    for (int i = m_aObjInfo[iObj].m_nIndex; i < GetSize()-1 && !GetAt(i+1).IsNull();i++)
	{
		x += ((double)(GetAt(i).x) + (double)(GetAt(i+1).x))*((double)(GetAt(i).x)*(double)(GetAt(i+1).y) - (double)(GetAt(i+1).x) * (double)(GetAt(i).y));
		y += ((double)(GetAt(i).y) + (double)(GetAt(i+1).y))*((double)(GetAt(i).x)*(double)(GetAt(i+1).y) - (double)(GetAt(i+1).x) * (double)(GetAt(i).y));

		ASSERT(!GetAt(i).IsNull() && !GetAt(i+1).IsNull());
	}
	coord.x = (long)(x / (6.0*dArea));
	coord.y = (long)(y / (6.0*dArea));


}

///////////////////////////////////////////////////////////////////////////////
//
// Returns the length of the polygon
//

double CSpatial::CalcLength(CLongLines *pLongLines)
{
   double dLength = 0;
   int i = 0; for (i = 0; i < pLongLines->GetSize()-1; i++)
   {
      if (!pLongLines->GetAt(i).IsNull() && !pLongLines->GetAt(i+1).IsNull())
      {
         dLength += BDProjection()->GetDistance(pLongLines->GetAt(i), pLongLines->GetAt(i+1));
      }
   }
   return dLength;
}

/////////////////////////////////////////////////////////////////////////////
//
// Check if maplines are a polygon
//

BOOL CSpatial::IsPolygon(CLongLines* pMapLines)
{   
   int j = 0;
   int i = 0; for (i = 0; i < pMapLines->GetSize(); i++)
   {      
      CLongCoord& coord = pMapLines->GetAt(i);
      if (coord.IsNull() && i > 0)
      {
         if (pMapLines->GetAt(j).x != pMapLines->GetAt(i-1).x ||
             pMapLines->GetAt(j).y != pMapLines->GetAt(i-1).y) return FALSE;
         else j = i+1;         
      }
   }
   return TRUE;
}

