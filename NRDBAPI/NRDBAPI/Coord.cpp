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
#include <strstream>
#include "coord.h"

CCoord::CCoord()
{
   SetNull();
}

CCoord::CCoord(CCoord& rSrc)
{
   x = rSrc.x;
   y = rSrc.y;
}

CCoord& CCoord::operator=(CCoord& rSrc)
{
   x = rSrc.x;
   y = rSrc.y;
   return *this;
};

BOOL CCoord::operator==(CCoord& rSrc)
{
   return rSrc.x == x && rSrc.y == y;
}

BOOL CCoord::operator!=(CCoord& rSrc)
{
   return !(rSrc.x == x && rSrc.y == y);
}

CLongCoord::CLongCoord()
{
   SetNull();
}

CLongCoord::CLongCoord(CLongCoord& rSrc)
{
   x = rSrc.x;
   y = rSrc.y;
}

BOOL CLongCoord::operator==(CLongCoord& rSrc)
{
   return rSrc.x == x && rSrc.y == y;
}

BOOL CLongCoord::operator!=(CLongCoord& rSrc)
{
   return !(rSrc.x == x && rSrc.y == y);
}
