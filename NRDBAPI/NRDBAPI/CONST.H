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

#ifndef _CONST_H_
#define _CONST_H_

#ifdef NRDBMYSQL

#define AFX_RFX_SHORT_PSEUDO_NULL (0x7EE4)
#define AFX_RFX_INT_PSEUDO_NULL (0x7EE4)
#define AFX_RFX_LONG_PSEUDO_NULL (0x4a4d4120L)
#define AFX_RFX_BYTE_PSEUDO_NULL 255
#define AFX_RFX_SINGLE_PSEUDO_NULL (-9.123e19f)
#define AFX_RFX_DOUBLE_PSEUDO_NULL (-9.123e19)
#define AFX_RFX_BOOL_PSEUDO_NULL 2
#define AFX_RFX_DATE_PSEUDO_NULL CTime(0)
#define AFX_RFX_TIMESTAMP_PSEUDO_NULL 99

#endif

#define COMPARENULLDOUBLE 1e15

#endif
