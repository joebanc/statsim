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
#include "include\freeimage.h"

#include "nrdbpro.h"
#include "FileObj.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
//
// compress function is exported as _compress@16, however the zlib.h defines 
// it as _compress therefore need to define elsewhere!
//

/*extern int _stdcall compressZ(BYTE *dest, ULONG *destLen, const BYTE *source, ULONG sourceLen);
extern int _stdcall uncompressZ (BYTE *dest, ULONG *destLen, const BYTE *source, ULONG sourceLen);*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileObj::CFileObj()
{

}

CFileObj::~CFileObj()
{

}

//////////////////////////////////////////////////////////////////////
//
// Opens the specified file and stores it in the CFileObj
//

BOOL CFileObj::Open(LPCSTR sFileName)
{
   BOOL bOK = FALSE;
   BYTE aBuffer[1024];
   long lSize;

   FILE* pFile = fopen(sFileName, "rb");
   if (pFile != NULL)
   {
      while ((lSize = fread(aBuffer, sizeof(BYTE), sizeof(aBuffer), pFile)) > 0)
      {
         m_aData.SetSize(m_aData.GetSize() + lSize);
         memcpy(m_aData.GetData() + m_aData.GetSize() - lSize, aBuffer, lSize);

         bOK = TRUE;
      }

      m_sFileName = sFileName;
      fclose(pFile);
   }
   
   return bOK;
}

//////////////////////////////////////////////////////////////////////
//
// Converts a longbinary back into a file object
//

BOOL CFileObj::Initialise(CLongBinary& longbin)
{
   BOOL bOK = FALSE;

   // Initialise

   m_sFileName = "";
   m_aData.RemoveAll();


   // Retrieve data

   BYTE* pData = (BYTE*)GlobalLock(longbin.m_hData);
   if (pData != NULL)
   {
      // Retrieve the file name

      while (*pData != '\0')
      {
         m_sFileName += *pData;
         pData++;
      };

      // Skip null

      pData++;

      // Retrieve file length

      ULONG lSize;
      memcpy(&lSize, pData, sizeof(long));
      pData += sizeof(long);

      m_aData.SetSize(lSize);
      
      if (FreeImage_ZLibUncompress(m_aData.GetData(), lSize, pData, longbin.m_dwDataLength-sizeof(long)-m_sFileName.GetLength() -1) > 0)
      {
         bOK = TRUE;
      }      

      GlobalUnlock(longbin.m_hData);
   } 

   return bOK;

}

//////////////////////////////////////////////////////////////////////
//
// Takes a file object and converts it to a long binary
//
// Format: filename \0 length (4 bytes) : compressed file
//

BOOL CFileObj::GetLongBinary(CLongBinary& longbin)
{
   BOOL bOK = TRUE;

  // Compress the file data

   ULONG nDestLen = (size_t)(m_aData.GetSize() * 1.01 + 12);
   BYTE* pBuffer = new BYTE[nDestLen];
   if (pBuffer == NULL || (nDestLen=FreeImage_ZLibCompress(pBuffer, nDestLen, m_aData.GetData(), m_aData.GetSize())) ==0)
   {
      bOK = FALSE;
   }

   // Extract the file name

   int index = m_sFileName.ReverseFind('\\');
   CString sFileName = m_sFileName.Mid(index+1);

   // Now write the data to the long file object

   if (bOK)
   {
      int nAllocLen = nDestLen + sFileName.GetLength() + 1 + sizeof(long);
      if (longbin.m_hData != NULL) GlobalFree(longbin.m_hData);
      longbin.m_hData = GlobalAlloc(GMEM_MOVEABLE|GMEM_DISCARDABLE, nAllocLen); 

      if (longbin.m_hData != NULL)
      {
         longbin.m_dwDataLength = nAllocLen;
         BYTE* pData = (BYTE*)GlobalLock(longbin.m_hData);
         if (pData != NULL)
         {
            // Store the file name

            memcpy(pData, sFileName.GetBuffer(0), sFileName.GetLength());
            pData[sFileName.GetLength()] = '\0';

            // Store the uncompressed size

            long cbSize = m_aData.GetSize();
            memcpy(pData + sFileName.GetLength()+1, &cbSize, sizeof(long));

            // Store the data

            memcpy(pData + sFileName.GetLength()+1 + sizeof(long), pBuffer, nDestLen);

            GlobalUnlock(longbin.m_hData);
         } else
         {
            bOK = FALSE;
         }
      } else
      {
         bOK = FALSE;
      }
   };

   // Tidy up

   if (pBuffer != NULL)
   {
      delete [] pBuffer;
   }

   return bOK;
}