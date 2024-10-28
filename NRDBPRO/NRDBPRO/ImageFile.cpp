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
#include "dlgprogress.h"

#include "ImageFile.h"
#include "viewmap.h"
#include "projctns.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////

#define NCHANNELS 3

/////////////////////////////////////////////////////////////////////////////

inline void swap(long& l1, long& l2)
{
   long l = l1;
   l1 = l2;
   l2 = l;
}

/////////////////////////////////////////////////////////////////////////////
//
// compress function is exported as _compress@16, however the zlib.h defines 
// it as _compress therefore need to define elsewhere!
//

/*extern int _stdcall compressZ(BYTE *dest, ULONG *destLen, const BYTE *source, ULONG sourceLen);
extern int _stdcall uncompressZ (BYTE *dest, ULONG *destLen, const BYTE *source, ULONG sourceLen);*/

/////////////////////////////////////////////////////////////////////////////

void inline write(CByteArray& aData, int n)
{
   int nLength= aData.GetSize();
   aData.SetSize(nLength + sizeof(n));      
   memcpy(&aData[nLength], &n, sizeof(n));
}

void inline write(CByteArray& aData, double n)
{
   int nLength= aData.GetSize();
   aData.SetSize(nLength + sizeof(n));      
   memcpy(&aData[nLength], &n, sizeof(n));
}

/////////////////////////////////////////////////////////////////////////////

DWORD WaitProcess(HWND hWnd, HANDLE handle);


//////////////////////////////////////////////////////////////////////
//
// Construction/Destruction

CImageFile::CImageFile()
{
   m_pFile = NULL;  
   m_nWaterColor = 1;
   m_hBitmap = NULL;
	m_hbmMask = NULL;
}

///////////////////////////////////////////////////////////////////////////////
//
// Copy constructor necessary for custom symbols

CImageFile::CImageFile(CImageFile& rSrc)
{
   *this = rSrc;   
}

CImageFile& CImageFile::operator =(CImageFile& rSrc)
{
   m_buffer = rSrc.m_buffer;
   m_pFile = NULL;
   m_bChanged = FALSE;
   m_nWaterColor = 1;

   // Note, m_georef not copied

   return *this;
};

///////////////////////////////////////////////////////////////////////////////

CImageFile::~CImageFile()
{
   Close();   
}

/////////////////////////////////////////////////////////////////////////////
// CImageDoc construction/destruction

Buffer::Buffer()
{       
    width =  height = 0;    
    flags = 0;            
    nChannels = 0;
	 dib = NULL;
    bTransparent = FALSE;
	 crTransparent= NULL; // color which is defined as transparent color
}

///////////////////////////////////////////////////////////////////////////////
//
// Converts an image to a long binary for storing in the database
//

BOOL CImageFile::Open(LPCTSTR lpszPathName, int nFlag, HWND hWnd) 
{     
   BOOL bOK = TRUE;

   if (hWnd == NULL) hWnd = GetFocus();

   // Tidy up from previous

   Close();

   //m_bTransparent = nFlag & Transparent;

   // Load data

   AfxGetApp()->BeginWaitCursor();

   //open all supported file format

	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN; 
			// check the file signature and deduce its format 
			// (the second argument is currently not used by FreeImage)
	fif = FreeImage_GetFileType(lpszPathName); 
   if (fif == FIF_UNKNOWN) fif = FreeImage_GetFIFFromFilename(lpszPathName);
	
   if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) 
	{ 
		// ok, let's load the file 
		FIBITMAP *dib = FreeImage_Load(fif, lpszPathName); 
      if (dib != NULL)
      {						
		   m_buffer.dib = FreeImage_Clone(dib);
		   ASSERT(m_buffer.dib != NULL);
		   m_buffer.flags = fif;
		   m_buffer.height = FreeImage_GetHeight(dib);
		   m_buffer.width = FreeImage_GetWidth(dib);
         m_buffer.nChannels = FreeImage_GetBPP(dib)/8;

         // Determine if transparent

         BOOL bHasBackground = FreeImage_HasBackgroundColor(dib);			
		   BOOL bIsTransparent = FreeImage_IsTransparent(dib);
         int nTransparentColorCount = FreeImage_GetTransparencyCount(dib);

         // TODO Not handling transparent images for non-paletised images

		   if(bHasBackground == TRUE || (bIsTransparent == TRUE && nTransparentColorCount > 0))
		   {
			   m_buffer.bTransparent = TRUE;
		   }

		   FreeImage_Unload(dib);
   
         // Now retrieve corresponding georeference coordinates if any
         // If retrieving map from .nrm file then only display the georeference
         // utility if not previously georeferenced

         if (nFlag & GeoRef && (!m_georef.Load(lpszPathName) || 
             m_georef.m_aPoints.GetSize() < 2))
         {
            // If image has not been georeferenced then run Image2Shp to allow
            // user opportunity

            CString sPath = BDGetAppPath();
            if (sPath.Right(1) != "\\") sPath += "\\";
            sPath += "Image2Shp.exe \"" + CString(lpszPathName) + "\" ";   
         
            // Disable window whilst Image2Shp is open to prevent
            // user from closing dialog

            CWnd::FromHandle(hWnd)->EnableWindow(FALSE);         

            // Now spawn the application

            PROCESS_INFORMATION proc = {0};
            STARTUPINFO start = {0};
            int ret;
	      
           // Initialize the STARTUPINFO structure:

            start.cb = sizeof (start);      
            start.wShowWindow = SW_SHOWNORMAL;

            // Start the shelled application:

            ret = CreateProcess(0, (LPSTR)(LPCSTR)sPath, 0, 0, TRUE,
                                NORMAL_PRIORITY_CLASS, 0, 0, &start, &proc);     
            if (ret)
            {                                  
               DWORD dw = WaitProcess(AfxGetMainWnd()->GetSafeHwnd(), proc.hProcess); 
               // dw == 0 on error 
            }

            CWnd::FromHandle(hWnd)->EnableWindow(TRUE);                 
            SetFocus(hWnd);         
         
            // Now attempt to load the georeferences again

            m_georef.Load(lpszPathName);
         };
      
         // Need to convert parameters from lat/long if necessary before
         // georefercing

         // Determine  if points are in lat/long or x,y

         BOOL bLatLon = TRUE;

         CArray <CGeoPoint,CGeoPoint>& geopoints = m_georef.m_aPoints;
         int i = 0; for (i = 0; i < geopoints.GetSize(); i++)
         {
            if (geopoints[i].m_dWX < -180 || geopoints[i].m_dWX > 180 ||
                geopoints[i].m_dWY < -90 || geopoints[i].m_dWY > 90)
            {
               bLatLon = FALSE;
               break;
            }
         }
      
         // If latitude/longitude then convert to x,y

         if (bLatLon)
         {
            int i = 0; for (i = 0; i < geopoints.GetSize(); i++)
            {
               long x,y;
               BDProjection()->LatLonToTransMercator(geopoints[i].m_dWY, geopoints[i].m_dWX, &x,&y);
               geopoints[i].m_dWX = x;
               geopoints[i].m_dWY = y;
            }
         }

         // Calcualate georeference

         m_georef.Initialise(m_buffer.width, m_buffer.height);
     
	      bOK &= m_buffer.dib != NULL;
      };
   }   
   
   AfxGetApp()->EndWaitCursor();

   // Quit if no file selected   
   
	return bOK;
}


/////////////////////////////////////////////////////////////////////////////

void CImageFile::Close()
{
   if (m_buffer.dib != NULL)
   {
      FreeImage_Unload(m_buffer.dib);
   };

   m_buffer.dib = NULL;   
   m_buffer.width = 0;
   m_buffer.height = 0;
   m_buffer.nChannels = 0;
   m_buffer.bTransparent = FALSE;
   m_buffer.crTransparent = NULL;

   DeleteObject(m_hBitmap);
	DeleteObject(m_hbmMask);
   m_hBitmap = NULL;
	m_hbmMask = NULL;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CImageFile::IsOpen()
{
   return m_buffer.dib != NULL;
}


///////////////////////////////////////////////////////////////////////////////

void CImageFile::OnDraw(CDC* pDC, CRect rect)
{   
   CRect rectI;
        
   CPaintDC* pPaintDC = (CPaintDC*)pDC;
   CRect rcP = &pPaintDC->m_ps.rcPaint;

   if (m_buffer.dib == NULL) return;

   if (!pDC->IsKindOf(RUNTIME_CLASS(CPaintDC))) rcP = rect;

   if (pDC->IsPrinting() && pDC->GetDeviceCaps(PHYSICALWIDTH) != 0)
   {
      rcP = CRect(0, 0, pDC->GetDeviceCaps(PHYSICALWIDTH),  
                  pDC->GetDeviceCaps(PHYSICALHEIGHT)); 
   }
   
   // Draw the base map

   if (rect.left == -1 && rect.right == -1) rect = rcP;  
   
// Optimization: Check for intersect

   if (!rectI.IntersectRect(rect, &rcP))
   {
      return;
   }
   
// Adjust aspect ratio of display area

   AspectRatio(rect);   

   if(!m_buffer.bTransparent) //not transparent
   {

      //draw a FreeImage image to the screen in Windows
   
      FIBITMAP *dib = FreeImage_Clone(m_buffer.dib);
   
      if (!dib) AfxMessageBox("Error cloning dib");

      // TODO handle watercolor
      // TODO handle transparent

	   SetStretchBltMode(pDC->GetSafeHdc(), COLORONCOLOR);

	   StretchDIBits(
				     pDC->GetSafeHdc(), // handle to DC
				     rect.left, // x-coord of destination upper-left corner
				     rect.top, // y-coord of destination upper-left corner
				     rect.right-rect.left, // width of destination rectangle
				     rect.bottom-rect.top, // height of destination rectangle
				     0, // x-coord of source upper-left corner
				     0, // y-coord of source upper-left corner
				     m_buffer.width, // width of source rectangle //FreeImage_GetHeight(dib),
				     m_buffer.height, // height of source rectangle // FreeImage_GetWidth(dib),	  
				     FreeImage_GetBits(dib), // bitmap bits
				     FreeImage_GetInfo(dib), // bitmap data
				     DIB_RGB_COLORS, // usage options
				     SRCCOPY); // raster operation code

	   FreeImage_Unload(dib);
   }
   // Transparent
   else
   {
      

		if(m_hBitmap  == NULL || m_hbmMask == NULL)
		{	//First time running => create temp hBitmap
		
		//convert a FreeImage image to a HBITMAP
		FIBITMAP *dib = FreeImage_Clone(m_buffer.dib);
		m_hBitmap = CreateDIBitmap(pDC->GetSafeHdc(), FreeImage_GetInfoHeader(dib),
		CBM_INIT, FreeImage_GetBits(dib), FreeImage_GetInfo(dib), DIB_RGB_COLORS);

		
	   	BOOL bHasBackground = FreeImage_HasBackgroundColor(dib);
			
		BOOL bIsTransparent = FreeImage_IsTransparent(dib);

			if(bIsTransparent == TRUE)
			{
			
		
				if (bHasBackground == TRUE) //especially for gif images with alpha transparency
				{

				RGBQUAD bkcolor;

				//AfxMessageBox("Has Background Color");

				//Retrieves the file background color of an image. 
				//Returns TRUE if successful, FALSE otherwise. For 8-bit images, 
				//the color index in the palette is returned in the rgbReserved 
				//member of the bkcolor parameter.
				//Retrieves transparent color only if alpha transparency is set
				FreeImage_GetBackgroundColor(dib,&bkcolor);

				//RGBQUAD to RGB (COLORREF)
				m_buffer.crTransparent = RGB(bkcolor.rgbRed,bkcolor.rgbGreen,bkcolor.rgbBlue);//bkcolor.rgbReserved

				//Create Black Mask
				m_hbmMask = CreateBitmapMask(m_hBitmap,m_buffer.crTransparent);




				}//end if bHasBackground
				else
				{
				//AfxMessageBox("Transparent, but no background color");

					int Transparent_Color_Count = FreeImage_GetTransparencyCount(dib);

					if(Transparent_Color_Count > 0)//not hight color 
					{

						if(Transparent_Color_Count = 1 )//only one transparent color
						{
							//AfxMessageBox("Only one transparent color");

	
							RGBQUAD *pal = FreeImage_GetPalette(dib); 

							m_buffer.crTransparent = RGB(pal[0].rgbRed,
														 pal[0].rgbGreen,
														 pal[0].rgbBlue);

								
							//Create Black Mask
							m_hbmMask = CreateBitmapMask(m_hBitmap,m_buffer.crTransparent);

					


						}//end Transparent_Color_Count = 1

						if(Transparent_Color_Count  >1 )//more than one transparent color
						{
							//AfxMessageBox("More than one transparent color");
							

							//Create Temp Black Mask
							m_hbmMask = CreateBitmap(m_buffer.width, m_buffer.height, 1, 1, NULL);
							
							BYTE *table = FreeImage_GetTransparencyTable(dib);
							RGBQUAD *pal = FreeImage_GetPalette(dib); 

							HDC hdcMem1 = pDC->GetSafeHdc();

								for( int i = 0; i < Transparent_Color_Count; i++ ) 
								{
									if( table[i] == 0 ) 
									{
								
										m_buffer.crTransparent = 
											RGB(pal[i].rgbRed,
												pal[i].rgbGreen,
												pal[i].rgbBlue);

									//Create Black Mask for the specified transparent color
									HBITMAP hbmTempMask = CreateBitmapMask(m_hBitmap,m_buffer.crTransparent);
									
									HDC hdcMem2 = CreateCompatibleDC(hdcMem1);

									 SelectObject(hdcMem1, m_hbmMask);
									 SelectObject(hdcMem2, hbmTempMask);

									 //Merge the previous black mask with the actual
									BitBlt(hdcMem2, 0, 0, m_buffer.width, m_buffer.height, hdcMem1, 0, 0, SRCAND);

									DeleteObject(hbmTempMask);
									DeleteDC(hdcMem2);

									}
								}


						

							DeleteDC(hdcMem1);
							


						}//end Transparent_Color_Count > 1
			

					}//end Transparent_Color_Count > 0

				}//end  bHasBackground

				FreeImage_Unload(dib);

			}//end IsTransparent 

		}//end hbitmap doesn't exists


		//Drawing transparent bitmap

			HDC hdc = pDC->GetSafeHdc();

			HDC hdcMem = CreateCompatibleDC(hdc);
			HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem,m_hbmMask);

			SetStretchBltMode(hdc, COLORONCOLOR);
			
			//Draw the mask
			StretchBlt(
				hdc,      // handle to destination DC
				rect.left, // x-coord of destination upper-left corner
				rect.top, // y-coord of destination upper-left corner
				rect.right-rect.left, // width of destination rectangle
				rect.bottom-rect.top, // height of destination rectangle
				hdcMem,       // handle to source DC
				0,  // x-coord of source upper-left corner
				0,  // y-coord of source upper-left corner
				m_buffer.width,//bm.bmWidth,    // width of source rectangle
				m_buffer.height,//bm.bmHeight,   // height of source rectangle
				SRCAND);      // raster operation code
				

			SelectObject(hdcMem,m_hBitmap);

			//Draw the bitmap in the mask area
			StretchBlt(
				hdc,      // handle to destination DC
				rect.left, // x-coord of destination upper-left corner
				rect.top, // y-coord of destination upper-left corner
				rect.right-rect.left, // width of destination rectangle
				rect.bottom-rect.top, // height of destination rectangle
				hdcMem,       // handle to source DC
				0,  // x-coord of source upper-left corner
				0,  // y-coord of source upper-left corner
				m_buffer.width,//bm.bmWidth,    // width of source rectangle
				m_buffer.height,//bm.bmHeight,   // height of source rectangle
				SRCPAINT);      // raster operation code


			SelectObject(hdcMem, hbmOld);
			DeleteDC(hdcMem);
			DeleteObject(hbmOld);				
   }
}

///////////////////////////////////////////////////////////////////////////////
//
// Draws an image with corresponding georeferenced coordinates into a map view
//

void CImageFile::OnDraw(CDC* pDC, CViewMap* pViewMap, CRect* pRect)
{
   // Determine the extremes of the bitmap in world coordinates

   CRectDbl rect;
   CRect rectV, rectC;
   CPoint point;

   m_georef.Convert(CPoint(0,0), rect.left, rect.top);
   m_georef.Convert(CPoint(m_buffer.width, m_buffer.height), rect.right, rect.bottom);

   // Now convert this to screen coordinates

   rectV.left = pViewMap->GetfXPos((int)rect.left);
   rectV.right = pViewMap->GetfXPos((int)rect.right);
   rectV.top = pViewMap->GetfYPos((int)rect.top);
   rectV.bottom = pViewMap->GetfYPos((int)rect.bottom);

   if (rectV.bottom < rectV.top) swap(rectV.bottom, rectV.top);

   // Clip to map view

   CRect rect1 = pViewMap->GetRect();
   if (pRect != NULL) rect1 = *pRect;
   rectC.IntersectRect(rectV, rect1);
   
   // Now draw the map into this area

   for (int i = rectC.left; i <= rectC.right; i++)
   {
      for (int j = rectC.top; j <= rectC.bottom; j++)
      {
         // Now get the world coordinates

         double x = pViewMap->GetfXInv(i);
         double y = pViewMap->GetfYInv(j);

         // Determine this on the original image

         m_georef.Convert(x,y, point);

         if (point.x >= 0 && point.x < m_buffer.width && 
             point.y >= 0 && point.y < m_buffer.height)
         {

			RGBQUAD cr;  
            //FreeImage_GetPixelColor(m_buffer.dib, point.x,m_buffer.height-point.y, &cr);  
			FreeImage_GetPixelColor(m_buffer.dib, point.x,point.y, &cr);  
			pDC->SetPixel(i, j, RGB(cr.rgbRed, cr.rgbGreen, cr.rgbBlue));   
         }
      }
   }
}
///////////////////////////////////////////////////////////////////////////////
//
// Converts a device context back to an image file
//

BOOL CImageFile::CaptureDC(CDC* pDC, int nWidth, int nHeight)
{
   BOOL bOK = TRUE;     

   CDlgProgress dlgProgress;
   dlgProgress.SetText(BDString(IDS_SAVING));

   // Initialise progres bar

   dlgProgress.SetRange(0, nHeight);   

   // Close any existing image

   Close();

   // Allocate FIBITMAP
   
	if((m_buffer.dib = FreeImage_AllocateT(FIT_BITMAP, nWidth, nHeight, NCHANNELS*8)) == NULL)
		return FALSE;
	
   // Create a compatible dc

   HDC memDC = CreateCompatibleDC(pDC->GetSafeHdc());
   
   // Copy from the device context to the bitmap

   HBITMAP bm = CreateCompatibleBitmap(pDC->GetSafeHdc(), nWidth, nHeight);
	HBITMAP oldbm = (HBITMAP)SelectObject(memDC, bm);
	BitBlt(memDC, 0, 0, nWidth, nHeight, pDC->GetSafeHdc(), 0, 0, SRCCOPY);

	if (GetDIBits(pDC->GetSafeHdc(),								// handle to DC
						bm,						// handle to bitmap
						0,							// first scan line to set
						FreeImage_GetHeight(m_buffer.dib),	// number of scan lines to copy
						FreeImage_GetBits(m_buffer.dib),	// array for bitmap bits
						FreeImage_GetInfo(m_buffer.dib),	// bitmap data buffer
						DIB_RGB_COLORS) ) 
   {
      m_buffer.width = nWidth;
      m_buffer.height = nHeight;
   } else
   {
      bOK = FALSE;
      Close();
   }

	DeleteObject(SelectObject(memDC, oldbm));
	DeleteObject(memDC);

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Saves the current image to the file specified
//

BOOL CImageFile::OnSave(LPCSTR sFile, int nQuality, BOOL bColor)
{
   BOOL bOK = FALSE;

   if (m_buffer.dib != NULL)
   {
       FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(sFile); 
       if (fif == FIF_UNKNOWN) fif = FreeImage_GetFIFFromFilename(sFile);       
        
        if (fif == FIF_JPEG)
        {
           bOK = FreeImage_Save(fif, m_buffer.dib, sFile, max(3,nQuality));                     
        } else
        {
           bOK = FreeImage_Save(fif, m_buffer.dib, sFile);                     
        }
   }

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Adjust the aspect ratio of the display rectangle so that the image does not
// appear distorted
//
   
void CImageFile::AspectRatio(CRect& rect)
{  
   if (rect.Height() / (double)rect.Width() > m_buffer.height / (double)m_buffer.width)
   {
      int nHeight = (m_buffer.height * rect.Width()) / m_buffer.width;
      rect.top = rect.top + rect.Height()/2 - nHeight/2;
      rect.bottom = rect.top + nHeight;
   }
   else
   {
      int nWidth = (m_buffer.width * rect.Height()) / m_buffer.height;
      rect.left = rect.left + rect.Width()/2 - nWidth/2;
      rect.right = rect.left + nWidth;
   }
}

///////////////////////////////////////////////////////////////////////////////

DWORD WaitProcess(HWND hWnd, HANDLE handle)
{
   MSG msg;       
   while (TRUE)
   {
      if (GetMessage(&msg, NULL, 0, 0))
      {
         TranslateMessage(&msg);
         DispatchMessage(&msg);                  
      };

     // Wait for exit code
               
      DWORD dword;
      if (!GetExitCodeProcess(handle, &dword) || dword != STILL_ACTIVE)
      {
         return dword;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
//
// Converts an image file into a long binary
//

BOOL CImageFile::AsLongBinary(CLongBinary& longbin)
{
   CByteArray aData;
   BOOL bOK = TRUE;

   // Add header information

   write(aData, m_buffer.width);
   write(aData, m_buffer.height);
   write(aData, m_buffer.flags);
   write(aData, m_buffer.nChannels);   

   // Write image data

   int cbHeader = aData.GetSize();   
   int cbImage = m_buffer.width*m_buffer.height*m_buffer.nChannels;

   aData.SetSize(cbHeader+cbImage);   

   // Retrieve the image with no padding bits, image stored bottom to top to maintain compatibility with previous versions of NRDB
   FreeImage_ConvertToRawBits(aData.GetData()+cbHeader, m_buffer.dib, 
       m_buffer.width*m_buffer.nChannels, m_buffer.nChannels*8, FI_RGBA_RED_MASK,FI_RGBA_GREEN_MASK,FI_RGBA_BLUE_MASK,FALSE); 

   // Write georeference data

   write(aData, m_georef.m_aPoints.GetSize());
   int i = 0; for (i = 0; i < m_georef.m_aPoints.GetSize(); i++)
   {
      write(aData, m_georef.m_aPoints[i].m_nSX);
      write(aData, m_georef.m_aPoints[i].m_nSY); 
      write(aData, m_georef.m_aPoints[i].m_dWX);
      write(aData, m_georef.m_aPoints[i].m_dWY);      
   }

   // Now compress the image to save space, this will also help with validation

   ULONG nDestLen = (size_t)(aData.GetSize() * 1.01 + 12);
   BYTE* aBuffer = new BYTE[nDestLen];

   if (aBuffer == NULL)
   {
      bOK = FALSE;
   }

   if (bOK && (nDestLen = FreeImage_ZLibCompress(aBuffer, nDestLen, aData.GetData(), aData.GetSize()))==0)
   {
      bOK = FALSE;
   }


   // Now convert this to a longbinary

   if (longbin.m_hData != NULL) GlobalFree(longbin.m_hData);
   longbin.m_hData = GlobalAlloc(GMEM_MOVEABLE|GMEM_DISCARDABLE, nDestLen + sizeof(long)); 
   if (longbin.m_hData != NULL)
   {
      longbin.m_dwDataLength = nDestLen + sizeof(long);
      BYTE* pData = (BYTE*)GlobalLock(longbin.m_hData);
      if (pData != NULL)
      {
         // Store the uncompress size

         long cbSize = aData.GetSize();
         memcpy(pData, &cbSize, sizeof(long));  
         memcpy(pData+sizeof(long), aBuffer, nDestLen); 
         GlobalUnlock(longbin.m_hData);
      } else
      {
         bOK = FALSE;
      }
   } else
   {
      bOK = FALSE;
   }

   // Tidy up

   if (aBuffer != NULL) delete [] aBuffer;

   return bOK;

}

///////////////////////////////////////////////////////////////////////////////
//
// Converts a longbinary back into an image file
//

BOOL CImageFile::Initialise(CLongBinary& longbin, BOOL bGeoRef)
{
   BOOL bOK = TRUE;
   ULONG lSize = 0;
   BYTE* pBuffer = NULL;
   BYTE* aBuffer = NULL;

   // Firstly uncompress the image
   
   BYTE* pData = (BYTE*)GlobalLock(longbin.m_hData);
   if (pData != NULL)
   {
      memcpy(&lSize, pData, sizeof(long));

      pBuffer = aBuffer = new BYTE[lSize];

      if (pBuffer != NULL)
      {
        if (!FreeImage_ZLibUncompress(pBuffer, lSize, pData+sizeof(long), longbin.m_dwDataLength-sizeof(long)))
        {
           bOK = FALSE;
        }
      } else
      {
        bOK = FALSE;
      }
   } else
   {
      bOK = FALSE;
   }

   // Now retrieve the values from the buffer

   if (bOK)
   {
      memcpy(&m_buffer.width, pBuffer, sizeof(m_buffer.width));
      pBuffer += sizeof(m_buffer.width);
      memcpy(&m_buffer.height, pBuffer, sizeof(m_buffer.height));
      pBuffer += sizeof(m_buffer.height);
      memcpy(&m_buffer.flags, pBuffer, sizeof(m_buffer.flags));
      pBuffer += sizeof(m_buffer.flags);
      memcpy(&m_buffer.nChannels, pBuffer, sizeof(m_buffer.nChannels));
      pBuffer += sizeof(m_buffer.nChannels);      
   };
          
  // Retrieve image

   if (bOK)
   {
      int cbImage = m_buffer.width * m_buffer.height * m_buffer.nChannels;
      if (m_buffer.dib != NULL) FreeImage_Unload(m_buffer.dib);
      
      //m_buffer.dib = FreeImage_ConvertFromRawBits(pBuffer, m_buffer.width, m_buffer.height, m_buffer.width*NCHANNELS, NCHANNELS*8, FI_RGBA_RED_MASK,FI_RGBA_GREEN_MASK,FI_RGBA_BLUE_MASK, TRUE);
	  m_buffer.dib = FreeImage_ConvertFromRawBits(pBuffer, m_buffer.width, m_buffer.height, m_buffer.width*m_buffer.nChannels, m_buffer.nChannels*8, FI_RGBA_RED_MASK,FI_RGBA_GREEN_MASK,FI_RGBA_BLUE_MASK, TRUE);

      if (m_buffer.dib != NULL)
      {       
         pBuffer += cbImage;
      } else
      {
         bOK = FALSE;
      }
   }

  // Retrieve georefencing

   if (bOK)
   {
      // Read number of georef points

      int nPoints = 0;
      memcpy(&nPoints, pBuffer, sizeof(nPoints));
      pBuffer += sizeof(nPoints);

      // Read in points

      m_georef.m_aPoints.SetSize(nPoints);

      int i = 0; for (i = 0; i < nPoints; i++)
      {
         memcpy(&m_georef.m_aPoints[i].m_nSX, pBuffer, sizeof(m_georef.m_aPoints[i].m_nSX));
         pBuffer += sizeof(m_georef.m_aPoints[i].m_nSX);

         memcpy(&m_georef.m_aPoints[i].m_nSY, pBuffer, sizeof(m_georef.m_aPoints[i].m_nSY));
         pBuffer += sizeof(m_georef.m_aPoints[i].m_nSY);

         memcpy(&m_georef.m_aPoints[i].m_dWX, pBuffer, sizeof(m_georef.m_aPoints[i].m_dWX));
         pBuffer += sizeof(m_georef.m_aPoints[i].m_dWX);

         memcpy(&m_georef.m_aPoints[i].m_dWY, pBuffer, sizeof(m_georef.m_aPoints[i].m_dWY));
         pBuffer += sizeof(m_georef.m_aPoints[i].m_dWY);
      }
    
     // Calculate rotation etc. from pararmeters

      if (bGeoRef)
      {
         bOK = m_georef.Initialise(m_buffer.width, m_buffer.height);
      };
   }


  // Tidy up

   if (aBuffer != NULL) delete [] aBuffer;


   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Builds a series of string pairs that specify filters you can apply to load a file. 
// The filter string is to be used by a 'File Open' dialog box 
// (GetOpenFileName or CFileDialog). 
// @param sFilter Input and output parameter.  
// @return Returns the number of supported import formats 

int CImageFile::GetOpenFilterString(CString& sFilter, int flags) 
{ 
  int i, iCount=0; 
  CString sExt;

  // If loading then add support for 'All image files'

   if (!(flags & save))
   {
      if (!(flags & extonly)) sFilter+=BDString(IDS_ALLFILES)+"|";

      for(i = 0; i < FreeImage_GetFIFCount(); i++) 
      {
         if((FreeImage_FIFSupportsReading((FREE_IMAGE_FORMAT)i) && flags&open) ||
            (FreeImage_FIFSupportsWriting((FREE_IMAGE_FORMAT)i) && flags&save))
         {
            // Exclude GIF as not licensed

            if ((FREE_IMAGE_FORMAT)i != FIF_GIF)
            {
               if (i>0)sFilter+=";";
               sFilter+="*."+CString(FreeImage_GetFIFExtensionList((FREE_IMAGE_FORMAT)i));

               // Replace commas with semi-colons e.g. *.jpg,*.jpeg
               sFilter.Replace(',',';');
            };
         };
      }
      sFilter+="|";
   }

// Build a string for each image type supported

   if (!(flags & extonly))
   {
      for(i = 0; i < FreeImage_GetFIFCount(); i++) 
      { 
	      if((FreeImage_FIFSupportsReading((FREE_IMAGE_FORMAT)i) && flags&open) ||
            (FreeImage_FIFSupportsWriting((FREE_IMAGE_FORMAT)i) && flags&save))
	      {  
            sExt = CString(FreeImage_GetFIFExtensionList((FREE_IMAGE_FORMAT)i));
                       
            // Exclude GIF as not licensed

            if (sExt.CompareNoCase("GIF") != 0)
            {
               sFilter+=FreeImage_GetFIFDescription((FREE_IMAGE_FORMAT)i)+CString("|");

               if (flags & save)
               {
                  int n = sExt.Find(",");
                  if (n >= 0) sExt = sExt.Left(n);
               }         
               sFilter+="*."+sExt+"|";            			
               iCount++;
            };
	      } 
      } 

      sFilter+="|";
   };
   return iCount; 
}  

///////////////////////////////////////////////////////////////////////////////
//
// Given the name of a file (including optional path), returns true if this 
// represents a supported file type
//

BOOL CImageFile::IsImageFile(LPCSTR sFile)
{
   FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(sFile); 
   if (fif == FIF_UNKNOWN) fif = FreeImage_GetFIFFromFilename(sFile);

   // Exclude GIF as not licensed

   if (fif == FIF_GIF) fif = FIF_UNKNOWN;

   return fif != FIF_UNKNOWN;

}

///////////////////////////////////////////////////////////////////////////////
//
// Creates a bitmap mask for a transparent colour
//

HBITMAP CImageFile::CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent)
{
    HDC hdcMem, hdcMem2;
    HBITMAP hbmMask;
    BITMAP bm;

    // Create monochrome (1 bit) mask bitmap.  

    GetObject(hbmColour, sizeof(BITMAP), &bm);
    hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

    // Get some HDCs that are compatible with the display driver

    hdcMem = CreateCompatibleDC(0);
    hdcMem2 = CreateCompatibleDC(0);

    SelectObject(hdcMem, hbmColour);//SelectBitmap(hdcMem, hbmColour);
    SelectObject(hdcMem2, hbmMask);//SelectBitmap(hdcMem2, hbmMask);

    // Set the background colour of the colour image to the colour
    // you want to be transparent.
    SetBkColor(hdcMem, crTransparent);

    // Copy the bits from the colour image to the B+W mask... everything
    // with the background colour ends up white while everythig else ends up
    // black...Just what we wanted.

    BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

    // Take our new mask and use it to turn the transparent colour in our
    // original colour image to black so the transparency effect will
    // work right.
    BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);

    // Clean up.

    DeleteDC(hdcMem);
    DeleteDC(hdcMem2);

    return hbmMask;
}