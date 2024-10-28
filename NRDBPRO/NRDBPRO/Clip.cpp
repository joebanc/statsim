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
#include "nrdb.h"
#include "clip.h"

///////////////////////////////////////////////////////////////////////////////
//
// Clips line to rectangle
//
// Returns IOUT=1 if the line should be drawn; 
// CLIPPED has will have value ClippedTo and or ClippedTo according to whether
// the line is clipped and x1, x2, y1, y2 will be adjusted
// accordingly
// 

void ClipToRect(CRect rect, long& x1, long& x2,long& y1, long& y2, int& rnClipped, BOOL& bOut)
{
   rnClipped = 0;
   int nClipped = 0;
   bOut = 0;
   

   // Clip to each edge separately

   for (int j = 1; j <= 4; j++)   
   {	              
      // Clip to each vertex                        

      if (j == clipright) ClipToLine(x1, y1, x2, y2, rect.right, clipright, nClipped, bOut);
      else if (j == clipleft) ClipToLine(x1, y1, x2, y2, rect.left, clipleft, nClipped, bOut);
      else if (j == cliptop) ClipToLine(y1, x1, y2, x2, rect.top, cliptop, nClipped, bOut);
      else if (j == clipbottom) ClipToLine(y1, x1, y2, x2, rect.bottom, clipbottom, nClipped, bOut);
     
      // Exit if not visible

      if (!bOut)
      {
         return;
      }

      // Store clipping results

      rnClipped |= nClipped;	
   };
}


///////////////////////////////////////////////////////////////////////////////
//
// Determines if line x1, y1 to x2, y2 crosses clip boundary defined by xB with
// nLine = left or right boundary as appropriate.  Returns nClipped = ClippedFrom
// ClippedTo as appropriate and bOut indicating if a line is to be drawn.
//
// To clip in the Y plane, swap the x and y parameters
//

void ClipToLine(long& x1, long& y1, long& x2, long& y2, long xB, int nLine, int& nClipped, BOOL& bOut)
{
   nClipped = 0;
   bOut = TRUE;   
   
   // Negate left parameters so comparison operators still work

   if (nLine == clipleft || nLine == cliptop)
   {
      xB = -xB;
      x1 = -x1;
      x2 = -x2;
   }   
           
   // Perform comparison
         
   if (x1 > xB && x2 > xB)
   {
      bOut = FALSE;         
   }
   else if (x1 < xB && x2 < xB)
   {
      bOut = TRUE;         
   }
   else if (x1 < xB && x2 > xB)
   {
      nClipped = ClippedTo;   
   }
   else if (x1 > xB && x2 < xB)
   {
      nClipped = ClippedFrom;      
   }   

   // Correct

   if (nLine == clipleft || nLine == cliptop)
   {      
      x1 = -x1;
      x2 = -x2;
      xB = -xB;
   }

   // Cut line

   if (nClipped & ClippedFrom)
   {
      // Use floating point arithmetic to stop overflows when zoomed in

      y1 = (long)(((double)y2-(double)y1) * ((double)xB - (double)x1) / ((double)x2 - (double)x1) + (double)y1);
      x1 = xB;      
   }
   else if (nClipped & ClippedTo)
   {
      y2 = (long)(((double)y2-(double)y1) * ((double)xB - (double)x1) / ((double)x2 - (double)x1) + (double)y1);
      x2 = xB;
   };
}

