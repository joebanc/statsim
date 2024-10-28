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

#if !defined(AFX_IMAGEFILE_H__44260690_7FF6_11D5_A7F8_0080AD88D050__INCLUDED_)
#define AFX_IMAGEFILE_H__44260690_7FF6_11D5_A7F8_0080AD88D050__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////

class CViewMap;

#include "georeference.h"
#include "include\freeimage.h"

/////////////////////////////////////////////////////////////////////////////

#define IMG_GRAY 0x0001
#define IMG_JPG  0x0002
#define IMG_PNG  0x0004

/////////////////////////////////////////////////////////////////////////////

struct Buffer /* A viewer instance */
{                                
    int width, height;                          // Image size 
    BOOL flags;                                 // Use std. grayrey map?    
    int nChannels;                              // Number of channels
	 FIBITMAP *dib ; //nl FreeImage
    BOOL bTransparent;
	 COLORREF crTransparent; // color which is defined as transparent color

    Buffer();
};

/////////////////////////////////////////////////////////////////////////////

class CImageFile  
{
public:
	CImageFile();   
	~CImageFile();

   CImageFile(CImageFile&);   
   CImageFile& operator=(CImageFile&);

   enum {GeoRef=1, OnLoad=2, Transparent=4};

   BOOL Open(LPCTSTR sFile, int nFlags = Transparent, HWND hWndParent = NULL);
   void Close();
   BOOL IsOpen();

   void OnDraw(CDC* pDC, CRect rect = CRect(-1,-1,-1,-1));
   void OnDraw(CDC* pDC, CViewMap*, CRect* pRect = NULL);

   BOOL CaptureDC(CDC* pDC, int nWidth, int nHeight);

   BOOL OnSave(LPCSTR sFile, int nQuality = 100, BOOL bColor = TRUE);

   BOOL AsLongBinary(CLongBinary&);
   BOOL Initialise(CLongBinary&, BOOL bGeoRef = TRUE);   

   HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent);

   typedef enum {open=1, save=2, extonly=4} Filter;
   static int GetOpenFilterString(CString& sFilter, int flags=open) ;//nl FreeImage
   static BOOL IsImageFile(LPCSTR sFileName);

   // Operations
protected:
   void AspectRatio(CRect& rect);   

public:
   Buffer m_buffer;   
   FILE* m_pFile;
   BOOL m_bChanged;
   int m_nWaterColor;
   CGeoReference m_georef;   
   HBITMAP m_hBitmap;//Storing Device Dependent Bitmap (only if transparent)
   HBITMAP m_hbmMask;//Monochrome bitmap mask (only if transparent)
};

#endif // !defined(AFX_IMAGEFILE_H__44260690_7FF6_11D5_A7F8_0080AD88D050__INCLUDED_)
