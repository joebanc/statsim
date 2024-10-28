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
#include "bdimportexport.h"
#include "query.h"
#include "longarray.h"
#include "maplayer.h"

static char g_lastchar;
static CString sHeaderOld;

///////////////////////////////////////////////////////////////////////////////

CString BDNextItem(FILE* pFile)
{
   CString s;
   if (sHeaderOld != "")
   {
      s = sHeaderOld;
      sHeaderOld = "";
      return s;
   } else
   {
    
      char ch;
      BOOL bDelimited = FALSE;
   
      ch = fgetc(pFile);

      if (ch == '"')
      {
         ch = fgetc(pFile);
         bDelimited = TRUE;
      };

      while ((ch != EOF && ch != '\n') &&
             ((ch != '=' && ch != ',') || bDelimited) &&
             (ch != '"' || !bDelimited))
      {
         s += ch;
         ch = fgetc(pFile);
      };

      if (bDelimited && ch == '"') ch = fgetc(pFile);

      g_lastchar = ch;

      return s;
   };
}

///////////////////////////////////////////////////////////////////////////////

int BDNextInt(FILE* pFile, BOOL bHeader)
{
   int i=0;
   CString s;
   if (bHeader) BDNextItem(pFile);
   s = BDNextItem(pFile);
   if (s.IsEmpty())
   {  
      AfxThrowFileException(CFileException::endOfFile,0,"");
   };

   if (!sscanf(s,"%d",&i)) 
   {
      AfxThrowFileException(CFileException::generic,0,"");
   };
   return i;

}

///////////////////////////////////////////////////////////////////////////////
// 
// Retrieves an integer corresponding to a specific header.  If this header is
// not found then the header is stored and the position in the file is maintained
// until this header is matched
//

int BDNextInt(FILE* pFile, LPCSTR sHeader, int nDefault)
{
   // Header matches or old header matches
   
   CString s;
   if (sHeaderOld == sHeader || (s = BDNextItem(pFile)) == sHeader) 
   {
      sHeaderOld = "";
      return BDNextInt(pFile, FALSE);
   }

   // Value not found, store header
   else 
   {
      sHeaderOld = s;

      // Search to end of line

      if (g_lastchar != '=')
      {
         char ch = fgetc(pFile);
         while (ch != EOF && ch != '\n')
         {
            s += ch;
            ch = fgetc(pFile);
         };
      }
      return nDefault;
   };
}

///////////////////////////////////////////////////////////////////////////////

double BDNextDouble(FILE* pFile, BOOL bHeader)
{
   double d;
   CString s;
   if (bHeader) BDNextItem(pFile);   
   s = BDNextItem(pFile);
   if (!sscanf(s,"%lf",&d)) 
      AfxThrowFileException(CFileException::generic,0,"");
   return d;
}

///////////////////////////////////////////////////////////////////////////////

double BDNextDouble(FILE* pFile, LPCSTR sHeader, double dDefault)
{
   // Header matches or old header matches
   
   CString s;
   if (sHeaderOld == sHeader || (s = BDNextItem(pFile)) == sHeader) 
   {
      sHeaderOld = "";
      return BDNextDouble(pFile, FALSE);
   }

   // Value not found, store header
   else 
   {
      sHeaderOld = s;
      return dDefault;
   };
};

///////////////////////////////////////////////////////////////////////////////

COLORREF BDNextRGB(FILE* pFile, LPCSTR sHeader)
{
   // Header matches or old header matches
   
   CString s;
   if (sHeaderOld == sHeader || (s = BDNextItem(pFile)) == sHeader) 
   {
      sHeaderOld = "";
      int r,g,b;         
      r = BDNextInt(pFile, FALSE);
      g = BDNextInt(pFile, FALSE);
      b = BDNextInt(pFile, FALSE);      
      return RGB(r,g,b);
   } else
   {
      sHeaderOld = s;
      return -1;
   }   
}

///////////////////////////////////////////////////////////////////////////////

CString BDNextStr(FILE* pFile, BOOL bHeader)
{
   if (bHeader)
   {
      BDNextItem(pFile);
   }
   return BDNextItem(pFile);
}

///////////////////////////////////////////////////////////////////////////////

CString BDNextStr(FILE* pFile, LPCSTR sHeader)
{
   // Header matches or old header matches

   CString s;
   if (sHeaderOld == sHeader || (s = BDNextItem(pFile)) == sHeader) 
   {
      sHeaderOld = "";
      return BDNextStr(pFile, FALSE);      
   }

   // Value not found, store header
   else 
   {
      sHeaderOld = s;
      return "";
   };
}

///////////////////////////////////////////////////////////////////////////////

void BDNext(FILE* pFile, CArrayAttrSel* pArray, long lFType)
{
   int i;

   BDNextItem(pFile);
   do
   {
      i = BDNextInt(pFile, FALSE);

      CQueryAttrSel attrsel;
      attrsel.m_lAttr = i;
      attrsel.m_lFType = lFType;
      pArray->Add(attrsel);
   } 
   while (g_lastchar != '\n' && g_lastchar != EOF);   
}

///////////////////////////////////////////////////////////////////////////////

void BDNext(FILE* pFile, CDWordArray& adw, BOOL bHeader)
{   
   adw.RemoveAll();

   if (bHeader) BDNextItem(pFile);

   int i;
   
   TRY
   {
      do
      {              
         i = BDNextInt(pFile, FALSE);
         adw.Add(i);       
      } 
      while (g_lastchar != '\n' && g_lastchar != EOF); 
   }

   // Handle empty list

     CATCH (CFileException, e)
     {
         if (e->m_cause != CFileException::endOfFile || 
             adw.GetSize() != 0)
         {                  
            AfxThrowFileException(CFileException::generic,0,"");           
         }
     }
     END_CATCH
}

///////////////////////////////////////////////////////////////////////////////

void BDNext(FILE* pFile, CDWordArray& adw, LPCSTR sHeader)
{
   // Header matches or old header matches
   
   CString s;
   if (sHeaderOld == sHeader || (s = BDNextItem(pFile)) == sHeader) 
   {
      sHeaderOld = "";
      BDNext(pFile, adw, FALSE);
   }

   // Value not found, store header
   else 
   {
      sHeaderOld = s;
      return;
   };   
}

///////////////////////////////////////////////////////////////////////////////

void BDNext(FILE* pFile, CMapStyle &mapstyle)
{   
   char sBuffer[256];
   if (fgets(sBuffer, sizeof(sBuffer), pFile) == NULL ||
       !mapstyle.StringAs(sBuffer))
   {
      AfxThrowFileException(CFileException::endOfFile,0,"");
   }
}

///////////////////////////////////////////////////////////////////////////////

void BDNext(FILE* pFile, CLongArray& al)
{
   if (BDNextItem(pFile) != "")
   {
      int i;

      do
      {
         i = BDNextInt(pFile, FALSE);
         al.Add(i);
      } 
      while (g_lastchar != '\n' && g_lastchar != EOF);  
   };
}

///////////////////////////////////////////////////////////////////////////////

BOOL StringAs(CString s, LOGFONT *p)
{
   return sscanf(s, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%32s", 
          &p->lfHeight, &p->lfWidth, &p->lfEscapement, 
          &p->lfOrientation, &p->lfWeight, &p->lfItalic,
          &p->lfUnderline, &p->lfStrikeOut, &p->lfCharSet, 
          &p->lfOutPrecision, &p->lfClipPrecision, &p->lfQuality,
          &p->lfPitchAndFamily, p->lfFaceName) >= 13;
} 

///////////////////////////////////////////////////////////////////////////////

void BDNext(FILE* pFile, LOGFONT* p, LPCSTR sHeader)
{
   CString s;
   if (sHeaderOld == sHeader || (s = BDNextItem(pFile)) == sHeader) 
   {
      sHeaderOld = "";
      p->lfHeight = BDNextInt(pFile, FALSE);
      p->lfWidth = BDNextInt(pFile, FALSE);
      p->lfEscapement = BDNextInt(pFile, FALSE);
      p->lfOrientation = BDNextInt(pFile, FALSE);
      p->lfWeight = BDNextInt(pFile, FALSE);
      p->lfItalic = BDNextInt(pFile, FALSE);
      p->lfUnderline = BDNextInt(pFile, FALSE);
      p->lfStrikeOut = BDNextInt(pFile, FALSE);
      p->lfCharSet = BDNextInt(pFile, FALSE);
      p->lfOutPrecision = BDNextInt(pFile, FALSE);
      p->lfClipPrecision = BDNextInt(pFile, FALSE);
      p->lfQuality = BDNextInt(pFile, FALSE);
      p->lfPitchAndFamily = BDNextInt(pFile, FALSE);
      strncpy(p->lfFaceName,BDNextItem(pFile), LF_FACESIZE);
   } else
   {
      sHeaderOld = s;      
   }
}

///////////////////////////////////////////////////////////////////////////////
//
// Extracts integers from a string and skips trailing non-alphanumeric 
// characters
//

BOOL BDNext(char*& ps, int& nValue)
{
   int i = 0;
   CString s;

   while (isdigit(*ps) || (*ps == '-' && i == 0))
   {
      s += *ps;
      ps++;
      i++;
   }

   // If period then number is not integer, return error

   if (*ps == '.') return FALSE;

   BDSkip(ps);

   return sscanf(s, "%d",&nValue) == 1;
}

///////////////////////////////////////////////////////////////////////////////
//
// Extracts floating point numbers exluding mantisa, from a string and skips
// trailing non-alphanumeric characters
//

BOOL BDNext(char*& ps, double& dValue)
{    
   int i = 0;
   CString s;
   while (isdigit(*ps) || *ps == '.' || (*ps == '-' && i == 0))
   {
      s += *ps;
      ps++;
      i++;
   }   

   BDSkip(ps);

   return sscanf(s, "%lf",&dValue) == 1;   
}

///////////////////////////////////////////////////////////////////////////////
//
// Extracts the next alpha numeric characters
//

BOOL BDNext(char*& ps, char& ch)
{
   ch = *ps;
   return ch != '\0';
}

///////////////////////////////////////////////////////////////////////////////
//
// Skips alpha numeric characters
//

void BDSkip(char*& ps)
{   
   while (*ps != '\0' && !isalpha(*ps) && !isdigit(*ps))
   {
      ps++;
   }
}

///////////////////////////////////////////////////////////////////////////////

CString AsString(LOGFONT* p)
{
   CString s;
   s.Format("%li,%li,%li,%li,%li,%1i,%1i,%1i,%1i,%1i,%1i,%1i,%1i,%s", 
           p->lfHeight, p->lfWidth, p->lfEscapement, p->lfOrientation, p->lfWeight, p->lfItalic,
           p->lfUnderline, p->lfStrikeOut, p->lfCharSet, p->lfOutPrecision, p->lfClipPrecision,
           p->lfQuality, p->lfPitchAndFamily, p->lfFaceName);  
   return s;
}

///////////////////////////////////////////////////////////////////////////////

void BDWrite(FILE* pFile, LOGFONT* pLogFont, LPCSTR sTitle)
{
   CString s = AsString(pLogFont);

   fprintf(pFile,"%s=%s\n", sTitle, (LPCSTR)s);  
}

///////////////////////////////////////////////////////////////////////////////

void BDWrite(FILE* pFile, CRect rect, LPCSTR sHeader)
{
   fprintf(pFile, "%s=%i,%i,%i,%i\n", sHeader, rect.left, rect.top, rect.right, rect.bottom);
}

///////////////////////////////////////////////////////////////////////////////

void BDNext(FILE* pFile, CRect& rect, LPCSTR sHeader)
{
   rect.left = BDNextInt(pFile, sHeader, -1);
   if (rect.left != -1)
   {
      rect.top = BDNextInt(pFile, FALSE);
      rect.right = BDNextInt(pFile, FALSE);
      rect.bottom = BDNextInt(pFile, FALSE);
   } else
   {
      rect = CRect();
   }
}
