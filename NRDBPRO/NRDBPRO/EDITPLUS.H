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

#ifndef _EDITPLUS_H_
#define _EDITPLUS_H_

#include <limits.h>
#include <float.h>
#include "cdate.h"

class CEditPlus : public CEdit
{
public:
    
    void SetValue(double, int nDP = -1);
    void SetValue(long);
    void SetValue(CDateTime);
    void SetValue(int);

    BOOL GetValue(double&, double dMin = -DBL_MAX, double dMax = DBL_MAX, BOOL bNull = FALSE);
    BOOL GetValue(long&, long lMin = LONG_MIN, long lMax = LONG_MAX, BOOL bNull = FALSE);
    BOOL GetValue(int&, int nMin = INT_MIN, int nMax = INT_MAX, BOOL bNull = FALSE);
    BOOL GetValue(CDateTime&, CDateTime dMin = CDateTime(100,1,1), BOOL bNull = FALSE);
    BOOL GetPath(CString&);
    BOOL GetFile(CString&, BOOL bNull = FALSE);
    BOOL GetFullPath(CString&);
    BOOL GetValue(CString&, BOOL bNull = FALSE);
    BOOL IsEmpty();

protected:
    CString GetLabel();
};

#endif
