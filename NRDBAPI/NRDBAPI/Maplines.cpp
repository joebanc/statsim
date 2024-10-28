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
#include "maplines.h"
#include "nrdbase.h"

///////////////////////////////////////////////////////////////////////////////

#define FLAG_SIGN 0x40
#define FLAG_BYTES 0x80
#define FLAG_MASK 0x3F

///////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CMapLines, CMapObject)

///////////////////////////////////////////////////////////////////////////////
//
// Class to create a dynamic array of type BYTE in a CLongBinary object.  This
// is pre-allocated and should be much more efficient than CArray
//

class CNRDBByteArray 
{
public:

   // Make a reference to the longbin.  The data is freed to prevent reallocating (and
   // copying) of old data

   CNRDBByteArray(CLongBinary &longbin) : m_longbin(longbin)
   {      
      m_nSize = 0;
      m_nMaxSize = 0;
      m_pData = 0;
      if (longbin.m_hData != NULL) 
      {
         GlobalFree(longbin.m_hData);
		 longbin.m_hData = NULL;
         m_longbin.m_dwDataLength = 0;
      }
   }

   // Pre-allocates the size

   void Initialise(int nSize)
   {  
      if (m_nSize < nSize) 
      {  
         if (m_pData != NULL)
         {
            GlobalUnlock(m_longbin.m_hData);
            m_pData = NULL;
         }
         if (m_longbin.m_hData == NULL)
         {
            m_longbin.m_hData = GlobalAlloc(GMEM_MOVEABLE|GMEM_DISCARDABLE, nSize);
         } else
         {            
            m_longbin.m_hData = GlobalReAlloc(m_longbin.m_hData,nSize,0);      
            m_longbin.m_dwDataLength = nSize;
         }

         if (m_longbin.m_hData != NULL) 
         {
            m_longbin.m_dwDataLength = nSize; 
            m_pData = (BYTE*)GlobalLock(m_longbin.m_hData);
            m_nMaxSize = nSize;

            ASSERT(m_pData != NULL);
         } else
         {
            m_nMaxSize = 0;
            m_nSize = 0;
            m_pData = NULL;
         }         
      };
   };   

   // Sets the final size and unlocks the memory

   void Finalise()
   {
      ASSERT(m_pData != NULL);
      ASSERT(m_longbin.m_hData != NULL);

      GlobalUnlock(m_longbin.m_hData);
      m_pData = NULL;

      m_longbin.m_hData = GlobalReAlloc(m_longbin.m_hData, m_nSize,0);
      m_longbin.m_dwDataLength = m_nSize;
      m_nMaxSize = m_nSize;
   }

   // Return allocated size

   int GetSize() {return m_nSize;};

   // Set size and grow if necessary

   void SetSize(int nSize)
   {
      m_nSize = nSize;
      if (m_nSize > m_nMaxSize)
      {
         Initialise((m_nMaxSize*150)/100);
      }
   }

   // Retrieve values
   
   BYTE& operator[](int index)
   {
      ASSERT(m_pData != NULL && index < m_nSize);
      return m_pData[index];

   };

   void Add(BYTE n)
   {
      ASSERT(m_pData != NULL);

      SetSize(m_nSize+1);
      m_pData[m_nSize-1] = n;
   }

protected:
    
   CLongBinary &m_longbin;
   BYTE *m_pData;
   int m_nSize;
   int m_nMaxSize;

};


///////////////////////////////////////////////////////////////////////////////

void inline write(CNRDBByteArray &aData, long l)
{
   int nLength= aData.GetSize();
   aData.SetSize(nLength + sizeof(long));      
   memcpy(&aData[nLength], &l, sizeof(long));
}

///////////////////////////////////////////////////////////////////////////////
//
// Write displacement, first byte contains sign flag and flag indicating whether
// to use two or three bytes
//

void inline writed(CNRDBByteArray &aData, long l)
{  
   BYTE b1 = l < 0 ? FLAG_SIGN : 0;
   l = abs(l);

   BOOL b3Bytes = l >= 0x4000;
   b1 |= b3Bytes ? FLAG_BYTES : 0;

   BOOL b4Bytes = l >= 0x200000;

   b1 |= l & FLAG_MASK;
   aData.Add(b1);

   l /= 0x40;
   BYTE b = (BYTE)l & 0xFF;
   ASSERT(b3Bytes || l < 0x100);
   aData.Add(b);

   if (b3Bytes)
   {
      l /= 0x100;
      BYTE b = l & 0x7F;
      if (b4Bytes) b |= FLAG_BYTES;
      aData.Add(b);

      if (b4Bytes)
      {
          l /= 0x80;
          ASSERT(l < 256);
          aData.Add((BYTE)l);
      }
   }
}


///////////////////////////////////////////////////////////////////////////////
//
// Retrieves a long integer from a 2/3/4 byte encoded word

long inline read(BYTE*& pData, int nVersion = 0)
{
   long l = 0;

   BYTE b1 = *pData;
   pData++;

   // Determine flags

   BOOL bSign = b1 & FLAG_SIGN;
   BOOL b3Bytes = b1 & FLAG_BYTES;

   // Retrieve values from lower two bytes

   l = b1 & FLAG_MASK;
   l += *pData*0x40;
   pData++;

   // Retrieve values from upper two bytes

   if (b3Bytes) 
   {
      if (nVersion >= 23)
      {
         BOOL b4Bytes = *pData & FLAG_BYTES;

         l += (*pData & 0x7F) * 0x4000;
         pData++;

         if (b4Bytes)
         {
            l += *pData * 0x200000;
            pData++;
         }
      } else
      {
         l += *pData * 0x4000;
         pData++;
      }
   };

   // Retrieve sign

   if (bSign) l = -l;

   return l;
}


///////////////////////////////////////////////////////////////////////////////
//
// Constructor creates a maplines object from a long binary object
//

CMapLines::CMapLines()
{  
};

CMapLines::CMapLines(CMapLines& rSrc)
{
   m_aCoords.RemoveAll();
   m_aCoords.SetSize(rSrc.m_aCoords.GetSize());
   for (int i = 0; i < rSrc.m_aCoords.GetSize(); i++)
   {
      m_aCoords.SetAt(i, rSrc.GetAt(i));
   }; 
}

CMapLines& CMapLines::operator=(CMapLines& rSrc)
{
   m_aCoords.RemoveAll();
   m_aCoords.SetSize(rSrc.m_aCoords.GetSize());
   for (int i = 0; i < rSrc.m_aCoords.GetSize(); i++)
   {
      m_aCoords.SetAt(i, rSrc.GetAt(i));
   }; 
   return *this;
}

///////////////////////////////////////////////////////////////////////////////

CMapLines::CMapLines(CLongBinary& longbin)
{                
   Initialise(longbin);
}

///////////////////////////////////////////////////////////////////////////////
//
// Maintained for backwards compatability, not efficient (I tried using virtual
// functions but still need to recompile old versions of NRDB). 
//

void CMapLines::Initialise(CLongBinary& longbin)
{
   m_aCoords.RemoveAll();

   // Initialise longlines object

   CLongLines longlines;
   longlines.Initialise(longbin);

   // Now convert it to maplines object
   
   SetSize(longlines.GetSize());

   for (int i = 0; i < longlines.GetSize(); i++)
   {
      CCoord coord;
      CLongCoord coordL;

      coordL = longlines.GetAt(i);
      if (coordL.IsNull()) coord.SetNull(); 
      else {coord.x = coordL.x; coord.y = coordL.y;}

      SetAt(i, coord);
   }
}

///////////////////////////////////////////////////////////////////////////////

CLongLines::CLongLines()
{   
}


///////////////////////////////////////////////////////////////////////////////

CLongLines::CLongLines(CLongLines& rSrc)
{
   m_sProtected = rSrc.m_sProtected;
   Copy(rSrc);
}

///////////////////////////////////////////////////////////////////////////////

CLongLines::CLongLines(CLongBinary& longbin)
{
   Initialise(longbin);
}

///////////////////////////////////////////////////////////////////////////////

BOOL CLongLines::Initialise(CLongBinary& longbin)
{
   BOOL bOK = TRUE;

   RemoveAll();

   CCoord coord;   
   CLongCoord coordL;

   double* pData = (double*)GlobalLock(longbin.m_hData);

   if (pData != NULL)
   {      
      UINT i = 0;
      UINT iPos = 0;
      
      // If the coordinates are in compressed format then decode

      BYTE a[5] = "NRDB";
      if (longbin.m_dwDataLength >= 8 && memcmp(&a[0], pData, sizeof(char)*4) == 0)
      {
         bOK = DecodeMaplines((BYTE*)pData, longbin.m_dwDataLength);
      } else
      {
         UINT nSize = longbin.m_dwDataLength / sizeof(double) /2;
         SetSize(nSize);

         while (i < nSize*2)
         {
            coord.x = pData[i++];
            coord.y = pData[i++];

            if (coord.x ==  NULL_READING || coord.y == NULL_READING ||
                coord.x == NULL_READING_OLD)
            {            
               coordL.SetNull();
            } else
            {
               coordL.x = (long)(coord.x + 0.5);
               coordL.y = (long)(coord.y + 0.5);
            }
            SetAt(iPos++, coordL);
         }
      };

      GlobalUnlock(longbin.m_hData);     
   };   

   return bOK;
}


/////////////////////////////////////////////////////////////////////
//
// Converts longbinary to maplines in run-length encoded format (see 
// below)

BOOL CLongLines::DecodeMaplines(BYTE* pData, DWORD dwDataLength)
{
   BOOL bOK = TRUE;
   m_sProtected = "";

   BYTE* pStart = pData;

   // Determine from header if protected

   BOOL bProtected = pData[7];

   // Retrieve version

   int nVersionHigh = ((BYTE*)pData)[4];
   int nVersionLow = ((BYTE*)pData)[5];
   int nVersion = nVersionHigh*10+nVersionLow;

   // Skip header

   pData += sizeof(long)*2; 
   int index=0;

   // Determine number of lines

   long nLines;
   memcpy(&nLines, pData, sizeof(nLines));
   pData += sizeof(long);

   // Determine the total length of lines

   long nLengthTotal = 0;
   if (nVersion >= 22)
   {
      memcpy(&nLengthTotal, pData, sizeof(long));
      pData += sizeof(long);

      // Allocate total array length

      SetSize(nLengthTotal+nLines);
   }
   
   // Retrieve protection string

   if (bProtected)
   {
      while (*pData != '\0') m_sProtected += *pData++;      
      pData++;
   }

   // For each line

   for (int i = 0; i < nLines && bOK; i++)
   {
      // Get the number of points

      long nPoints;
      memcpy(&nPoints, pData, sizeof(nPoints));
      pData += sizeof(long);

      // Resize the lines (not required if total size is pre-allocated)

      if (nLengthTotal == 0)
      {
         SetSize(GetSize() + nPoints+1);
      };

      // Get the first point

      CLongCoord coord;
      memcpy(&coord.x, pData, sizeof(long));
      pData += sizeof(long);
      memcpy(&coord.y, pData, sizeof(long));
      pData += sizeof(long);
      SetAt(index, coord);      
      index++;
      
      for (int j = 1; j < nPoints; j++)
      {                      
         coord.x = GetAt(index-1).x + read(pData, nVersion);
         coord.y = GetAt(index-1).y + read(pData, nVersion);
         SetAt(index, coord);
         index++;

         if ((DWORD)(pData-pStart) > dwDataLength)
         {
            ASSERT(FALSE);
            bOK = FALSE;
            break; // Faster than checking bOK on each iteration
         }
      }

      // Set null coord to indicate end
      
      coord.SetNull();
      SetAt(index, coord);
      index++;
   }

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Converts a maplines object to a CLongBinary object.  This is to be used
// by the standard version of NRDB, NRDB Pro used compressed data
//

void CMapLines::GetLongBinary(CLongBinary& longbin)
{
   // Allocate/Realloc the size

   DWORD dwSize = m_aCoords.GetSize() * sizeof(double) * 2;
   if (longbin.m_hData == NULL)
   {
      longbin.m_hData = GlobalAlloc(GMEM_MOVEABLE|GMEM_DISCARDABLE, dwSize);
   } else
   {
      longbin.m_hData = GlobalReAlloc(longbin.m_hData,dwSize,0);      
   }
   longbin.m_dwDataLength = dwSize;

   // Copy the data across
  
   double* pData = (double*)GlobalLock(longbin.m_hData);
   if (pData != NULL)
   {
      int i = 0;
      int j = 0;
      for (i = 0; i < m_aCoords.GetSize(); i++)
      {
         pData[j++] = m_aCoords.GetAt(i).x;
         pData[j++] = m_aCoords.GetAt(i).y;
      }

      GlobalUnlock(longbin.m_hData);
   }
}

///////////////////////////////////////////////////////////////////////////////
//
// Uses run-length encoding to store data as a binary object.  Coordinates are stored
// as integer displacements which may be 2 bytes (16km) or 3 bytes (4000km) 
// according to flag
//
// Format = header + no. lines + polylines*
// header = 'NRDB' + 1 + 0 + 0 + 0/1 (8 bytes) 1=protected
// polylines = no. lines (4 bytes)
//             total length (4 bytes)
//             extent 4x4 bytes
//             centroid 2x4 bytes
// [protected message string 0] 
//             x coordinate (4 bytes)
//             y coordinate (4 bytes)
//             x displacement (dcoord)
//             y displacement (dcoord)
// dcoord = length bit 0=2 bytes, 1=3 bytes (1 byte)
//          sign bit 1=-ve (1 byte)
//          0-6 bits (6 bits)
//          7-14 bits (1 byte)
//          [15-21] bits (1 byte)
//
// polylines may be locked.  A message is included which is displayed when
// attempt is made to export the data.
//
// when m_sProtected is non-null, this string is stored indicating
// that the polyline may not be exported
//

BOOL CLongLines::GetLongBinary(CLongBinary& longbin)
{
   CNRDBByteArray aData(longbin);

   BOOL bOK = TRUE;

   // Optimisation: Estimate size of resulting array to reduce re-allocation of memory

   aData.Initialise((int)(GetSize()*5));
     
   // Add the header 

   aData.Add('N');
   aData.Add('R');
   aData.Add('D');
   aData.Add('B');
   aData.Add(2);
   aData.Add(3);
   aData.Add(0);
   aData.Add(0);

   // Indicate polyline is protected

   if (m_sProtected != "")
   {
      aData[aData.GetSize()-1]=1;      
   }

   // Add dummy no of lines

   write(aData,0);

   // Add dummy total length of lines
   
   write(aData,0);

    // Add protected string

   if (m_sProtected != "")
   {
      for (int i = 0; i < m_sProtected.GetLength(); i++) aData.Add(m_sProtected[i]);
      aData.Add(0);
   };
  
   // Encode all the lines
   
   long n=0;
   long nLengthTotal = 0;

   for (int i = 0; i < GetSize(); i++)
   {
      // for each set of polylines, determine its length

      for (int j = i; j < GetSize() && !GetAt(j).IsNull(); j++)
      {          
      }
      long nLength = j-i;

      // Write the length to file

      if (nLength > 0)
      {
         n++;
         nLengthTotal += nLength;

         write(aData, nLength);

         // Write start coordinates

         write(aData, GetAt(i).x);
         write(aData, GetAt(i).y);

#ifdef _DEBUG
         /*long x = GetAt(i).x;
         long y = GetAt(i).y;
         BYTE *pData = aData.m_pData+aData.GetSize();*/
#endif

         for (i = i+1; j < GetSize() && !GetAt(i).IsNull(); i++)
         {
            /*ASSERT(i != 1524);*/
            writed(aData, GetAt(i).x - GetAt(i-1).x);
            writed(aData, GetAt(i).y - GetAt(i-1).y);

#ifdef _DEBUG
          /*x += read(pData);
          y += read(pData);
          ASSERT(x == GetAt(i).x && y == GetAt(i).y);*/
#endif
         }
      };      
   };

   // Fill in number of lines

   memcpy(&aData[8], &n, sizeof(n));

   // Fill in total length of lines

   memcpy(&aData[12], &nLengthTotal, sizeof(long));

   // Reallocate to the correct size

   aData.Finalise();  
  
   return bOK;
}
