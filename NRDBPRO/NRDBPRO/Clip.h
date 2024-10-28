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
#ifndef _CLIP_H_
#define _CLIP_H_

class CMapLines;

enum {NotClipped, ClippedFrom=1, ClippedTo = 2};
enum {clipleft=1, clipright=2, cliptop=3, clipbottom=4};

/*void ClipPolygon(int& iPos, CMapLines* pMapLines, CPointArray& aPoints);*/
void ClipToRect(CRect rect, long& x1, long& x2,long& y1, long& y2, int& rnClipped, BOOL& bOut);
void ClipToLine(long& x1, long& y1, long& x2, long& y2, long xLimit, int nLine, int& nClipped, BOOL& bOut);


#endif