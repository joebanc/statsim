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
#include "bdattribute.h"
#include "projctns.h"

///////////////////////////////////////////////////////////////////////////////

CString CBDAttribute::AsString()
{
  CString s,s1,s2;

   double dLat, dLong;

	// Boolean

   if (m_lDataType == BDBOOLEAN)
   {
      if (*GetBoolean() == 1) return BDString(IDS_YES);
      else if (*GetBoolean() == 0) return BDString(IDS_NO);
      return "";
   }

  // latitude / longitude

  if (m_lDataType == BDCOORD)
  {
     if (BDGetSettings().m_bCoordAsLatLon)
	  {
		  CCoord coord = *GetCoord();
        if (!coord.IsNull())
        {      
			  BDProjection()->TransMercatorToLatLon(coord.x, coord.y, &dLat, &dLong);
           BDProjection()->LatLongAsString(dLat, s1, CProjection::latitude);
           BDProjection()->LatLongAsString(dLong, s2, CProjection::longitude);      
			  s = s1 + "," + s2;
        };
		  return s;
	  }
	  else
	  {
		  return CAttribute::AsString();
	  }
  }
   else
   {
      return CAttribute::AsString();
   }
}
