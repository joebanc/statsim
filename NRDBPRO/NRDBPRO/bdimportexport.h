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

#ifndef _BDIMPORTEXPORT_H_
#define _BDIMPORTEXPORT_H_

class CArrayAttrSel;
class CLongArray;
class CMapStyle;

CString BDNextItem(FILE* pFile);
CString BDNextStr(FILE* pFile, BOOL bHeader = TRUE);
CString BDNextStr(FILE* pFile, LPCSTR sHeader);
int BDNextInt(FILE* pFile, BOOL bHeader = TRUE);
int BDNextInt(FILE* pFile, LPCSTR sHeader, int nDefault = 0);
double BDNextDouble(FILE* pFile, BOOL bHeader = TRUE);
double BDNextDouble(FILE* pFile, LPCSTR sHeader, double dDefault);
COLORREF BDNextRGB(FILE* pFile, LPCSTR sHeader);
void BDNext(FILE* pFile, CArrayAttrSel*, long lFType = 0);
void BDNext(FILE* pFile, LOGFONT*, LPCSTR sHeader);
void BDNext(FILE* pFile, CDWordArray&, BOOL bHeader = TRUE);
void BDNext(FILE* pFile, CDWordArray&, LPCSTR sHeader);
void BDNext(FILE* pFile, CLongArray&);
void BDNext(FILE* pFile, CMapStyle&);

void BDWrite(FILE* pFile, LOGFONT*, LPCSTR sHeader);
void BDWrite(FILE* pFile, CRect, LPCSTR sHeader);

CString AsString(LOGFONT*);
BOOL StringAs(CString, LOGFONT*);

void BDNext(FILE* pFile, CRect& rect, LPCSTR sHeader);

BOOL BDNext(char*&, int&);
BOOL BDNext(char*&, double&);
BOOL BDNext(char*&, char&);
void BDSkip(char*&);

#endif
