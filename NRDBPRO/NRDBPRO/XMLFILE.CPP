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
#include "xmlfile.h"
#include "bdimportexport.h"

///////////////////////////////////////////////////////////////////////////////

CXMLObj::CXMLObj()
{
   m_pNext = NULL;
   m_pChild = NULL;
}

CXMLObj::~CXMLObj()
{
   // Delete any objects

   if (m_pChild != NULL)
   {
      delete m_pChild;
      m_pChild = NULL;
   }

   if (m_pNext != NULL)
   {
      delete m_pNext;
      m_pNext = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

BOOL CXMLFile::Write(LPCSTR sFile)
{
   BOOL bOK = FALSE;

   // Writes a XML data to the file specified

   FILE* pFile = fopen(sFile, "w");
   if (pFile != NULL)
   {
      bOK = CXMLObj::Write(pFile);

      fclose(pFile);
   }

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CXMLFile::Read(LPCSTR sFile)
{
   BOOL bOK = FALSE;

   FILE *pFile = fopen(sFile, "r");
   if (pFile != NULL)
   {
      bOK = CXMLObj::Read(pFile);

      fclose(pFile);
   }
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CXMLObj::Read(FILE *pFile, char ch)
{
   BOOL bOK = TRUE;
   CString sName;

   // Skip white space

   if (ch == 0) ch = fgetc(pFile);
   while (isspace(ch)) ch = fgetc(pFile);

   // Read tag

   if (ch == '<')
   {
      // Get name of tag

       ch = fgetc(pFile);
       while (isalnum(ch)) 
       {
          m_sName += ch;
          ch = fgetc(pFile);
       };

       // Retrieve attributes

       if (isspace(ch))
       {
          ReadAttr(pFile);
          ch = fgetc(pFile);
       };       

       // If inline tag then return 

       if (ch == '/')
       {
          ch = fgetc(pFile);
          bOK = ch == '>';
          return bOK;
       }

       // Skip tag closing

       
       ASSERT(ch == '>');
       if (ch != '>')
       { 
          bOK = FALSE;          
       }

       while (bOK)
       {          
          // Read value (if any)

          ch = fgetc(pFile);            
          while (isspace(ch)) ch = fgetc(pFile);

          while (ch != '<' && ch != EOF)
          {
             m_sValue += ch;
             ch = fgetc(pFile);
          }

          // Get next character

          ch = fgetc(pFile);

          // Closing tag
          
          if (ch == '/') 
          {             
             ch = fgetc(pFile);
             while (isalnum(ch))
             {
                sName += ch;
                ch = fgetc(pFile);
             }
             // Invalid tag

             if (sName != m_sName) bOK = FALSE;

             // Find closing tag

             while (ch != '>' && ch != EOF)
             {
                ch = fgetc(pFile);
             }
             return bOK;
          }

          // Child tag
                    
          ungetc(ch, pFile); 

          // Find next available child object

          CXMLObj **pChild = &m_pChild;
          while (*pChild != NULL)
          {
             pChild = &(*pChild)->m_pNext;
          }
          (*pChild) = new CXMLObj;

          bOK = (*pChild)->Read(pFile, '<');

       };
   } 

   // Invalid XML
   else
   {
      bOK = FALSE;
   }
   
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CXMLObj::ReadAttr(FILE *pFile)
{
   BOOL bOK = TRUE;
   char ch = fgetc(pFile);
 
   while (ch != '>' && ch != '/' && ch != EOF)
   {
      // Reset
      CXMLAttr attr;

      // Skip space
      
      while (isspace(ch)) ch = fgetc(pFile);

      if (isalpha(ch))
      {     
         // Get name

         while (ch != '=' && ch != EOF)
         {
            attr.m_sName += ch;  
            ch = fgetc(pFile);
         }

         // Get opening quote

         ch = fgetc(pFile);

         // Get value

         if (ch == '\"')
         {
            ch = fgetc(pFile);
            while (ch != '\"' && ch != EOF)
            {
               attr.m_sValue += ch;
               ch = fgetc(pFile);
            }; 

            // Add to list 

            m_aAttr.Add(attr);
         } else
         {
            bOK = FALSE;
         }
      };

      ch = fgetc(pFile);
   }

   // Unwind last character

   ungetc(ch, pFile);
   
   if (feof(pFile)) bOK = FALSE;
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CXMLObj::Write(FILE *pFile)
{   
   // Write tag

   putc('<', pFile);
   fputs(m_sName, pFile);

   // Write attributes

   int i = 0; for (i = 0; i < m_aAttr.GetSize(); i++)
   {
      putc(' ', pFile);
      fputs(m_aAttr[i].m_sName, pFile);
      fputs("=\"", pFile);
      fputs(m_aAttr[i].m_sValue, pFile);     
      putc('\"', pFile);
   }

   // Close tag

   putc('>', pFile);

   // Cannot have a value and a child

   ASSERT(m_sValue == "" || m_pChild == NULL);

   // if there is a value then write this   

   if (m_sValue != "")
   {
      fputs(m_sValue, pFile);
   }

   // Write the child

   else if (m_pChild != NULL)
   {      
      m_pChild->Write(pFile);    
   }

   // Close the tag

   fputs("</", pFile);
   fputs(m_sName, pFile);
   putc('>', pFile);
   putc('\n', pFile);

   // Write the next

   if (m_pNext != NULL)
   {
      //putc('\n', pFile);
      m_pNext->Write(pFile);
      //putc('\n', pFile);
   }

   return ferror(pFile) == 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// Search through the list of attributes and returns the matching value (if any)
//

CString CXMLObj::GetAttr(LPCSTR sName)
{
   int i = 0; for (i = 0; i < m_aAttr.GetSize(); i++)
   {
      if (m_aAttr[i].m_sName == sName) return m_aAttr[i].m_sValue;
   }
   return "";
}

///////////////////////////////////////////////////////////////////////////////
//
// Sets an attribute value, adding a new attribute if necessary.  If sValue
// is set to NULL then the attribute is deleted
//

void CXMLObj::SetAttr(LPCSTR sName, LPCSTR sValue)
{
   int i = 0; for (i = 0; i < m_aAttr.GetSize(); i++)
   {
      if (m_aAttr[i].m_sName == sName) 
      {
         if (sValue == NULL) m_aAttr.RemoveAt(i);
         else m_aAttr[i].m_sValue = sValue;
         return;
      }
   }

   // Add a new attribute

   CXMLAttr attr;
   attr.m_sName = sName;
   attr.m_sValue = sValue;
   m_aAttr.Add(attr);
}

///////////////////////////////////////////////////////////////////////////////

void CXMLObj::SetAttr(LPCSTR sName, int n)
{
   CString s;
   s.Format("%d", n);
   SetAttr(sName, s);
}

///////////////////////////////////////////////////////////////////////////////
//
// Searches through parent and child objects until finds matching names and
// then returns child value


CXMLObj* CXMLObj::GetXMLObj(LPCSTR sTag, LPCSTR sTagChild, BOOL bRequired)
{
   CXMLObj *pXMLObj = this;

   // If the tag matches then don't move down one level

   /*if (sTag == m_sName) pXMLObj = this;
   else pXMLObj = m_pChild;*/

   while (pXMLObj != NULL)
   {
      if (pXMLObj->m_sName == sTag || sTag[0] == '\0')
      {
         CXMLObj *pXMLChild = pXMLObj->m_pChild;
         while (pXMLChild != NULL)
         {
            if (pXMLChild->m_sName == sTagChild)
            {
               return pXMLChild;
            }
            pXMLChild = pXMLChild->m_pNext;
         }     
         break; // Not found
      }
      pXMLObj = pXMLObj->m_pNext;
   }

   // If not found, and required, then throw a file exception

   if (bRequired) AfxThrowFileException(0,0,"");

   return NULL;
}

//////////////////////////////////////////////////////////////////////////////

CString CXMLObj::GetString(LPCSTR sTag, LPCSTR sTagChild)
{
   CXMLObj *pXMLObj = this;
   while (pXMLObj != NULL)
   {
      if (pXMLObj->m_sName == sTag || sTag[0] == '\0')
      {
         pXMLObj = pXMLObj->m_pChild;
         while (pXMLObj != NULL)
         {
            if (pXMLObj->m_sName == sTagChild)
            {
               return pXMLObj->m_sValue;
            }
            pXMLObj = pXMLObj->m_pNext;
         }
         return "";
      }
      pXMLObj = pXMLObj->m_pNext;
   }

   return "";
}

///////////////////////////////////////////////////////////////////////////////
//
// Retrieves an integer value for a matching tag and child tag
//

int CXMLObj::GetInteger(LPCSTR sTag, LPCSTR sTagChild)
{
   int i = 0;
   CString s = GetString(sTag, sTagChild);
   sscanf(s, "%i", &i);
   return i;
}

double CXMLObj::GetDouble(LPCSTR sTag, LPCSTR sTagChild)
{
   double d = 0;
   CString s = GetString(sTag, sTagChild);
   sscanf(s, "%lf", &d);
   return d;
}

CDateTime CXMLObj::GetDate(LPCSTR sTag, LPCSTR sTagChild)
{
   CDateTime date;
   
   CString s = GetString(sTag, sTagChild);
   date.StringAsDate(s);
   return date;
}

LOGFONT CXMLObj::GetLogfont(LPCSTR sTag, LPCSTR sTagChild)
{
   LOGFONT logfont;
   memset(&logfont, 0, sizeof(logfont));

   CString s = GetString(sTag, sTagChild);
   StringAs(s, &logfont);
   return logfont;
}

///////////////////////////////////////////////////////////////////////////////
//
// Adds the object as the next child
//

void CXMLObj::AddChild(CXMLObj *pXMLObj)
{
   if (m_pChild == NULL)
   {
      m_pChild = pXMLObj;
   } else
   {
      CXMLObj *pChild = m_pChild;

      while (pChild->m_pNext != NULL)
      {
         pChild = pChild->m_pNext;
      }
      pChild->m_pNext = pXMLObj;
   }
}

///////////////////////////////////////////////////////////////////////////////

CXMLObj* CXMLObj::AddChild(LPCSTR sName)
{
   CXMLObj *pXMLObj = new CXMLObj;
   pXMLObj->m_sName = sName;
   AddChild(pXMLObj);
   return pXMLObj;
}

///////////////////////////////////////////////////////////////////////////////
//
// Creates a new xml object at the same level
//

CXMLObj* CXMLObj::AddNext(LPCSTR sName)
{
   CXMLObj *pXMLObj = this;
   while (pXMLObj->m_pNext != NULL)
   {
      pXMLObj = pXMLObj->m_pNext;
   }
   ASSERT(pXMLObj != NULL);

   pXMLObj->m_pNext = new CXMLObj();
   pXMLObj->m_pNext->m_sName = sName;
   return pXMLObj->m_pNext;
}

///////////////////////////////////////////////////////////////////////////////
//
// Finds the next XML object with the correponding name. pXMLObj must initially
// have value NULL
//

BOOL CXMLObj::GetChild(LPCSTR sName, CXMLObj *&pXMLObj)
{
   // Return first object

   if (pXMLObj == NULL) 
   {
      pXMLObj = m_pChild;
      return pXMLObj != NULL;
   }

   CXMLObj *pXMLChild = m_pChild;
  
   while (pXMLChild != NULL)
   {
      if (pXMLChild->m_sName == sName && pXMLChild == pXMLObj)
      {
         pXMLObj = pXMLChild->m_pNext;
         return pXMLObj != NULL;
      }
      pXMLChild = pXMLChild->m_pNext;

   };
   return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Sets the value of the child
//

void CXMLObj::SetValue(LPCSTR szTag, LPCSTR sTagChild, LPCSTR s)
{
   CXMLObj *pXMLObj = this;
   CXMLObj *pXMLChild = NULL;

   CString sTag = szTag;
   if (sTag == "") sTag = pXMLObj->m_sName;

   while (pXMLObj != NULL)
   {
      if (pXMLObj->m_sName == sTag || pXMLObj->m_sName == "")
      {
         pXMLObj->m_sName = sTag;
         pXMLChild = pXMLObj->m_pChild;
         while (pXMLChild != NULL)
         {
            if (pXMLChild->m_sName == sTagChild)
            {
               pXMLChild->m_sValue = s;
               return;
            }
            pXMLChild = pXMLChild->m_pNext;
         }
         pXMLChild = new CXMLObj;
         pXMLChild->m_sName = sTagChild;
         pXMLChild->m_sValue = s;
         pXMLObj->AddChild(pXMLChild);
         return;
      }
      if (pXMLObj->m_pNext == NULL)
      {
         pXMLObj->m_pNext = new CXMLObj;
         pXMLObj->m_pNext->m_sName = sTag;         
      }
      pXMLObj = pXMLObj->m_pNext;
   }
}

///////////////////////////////////////////////////////////////////////////////

void CXMLObj::SetValue(LPCSTR sTag, LPCSTR sTagChild, int n)
{
   CString s;
   s.Format("%d",n);
   SetValue(sTag, sTagChild, s);
}

///////////////////////////////////////////////////////////////////////////////

void CXMLObj::SetValue(LPCSTR sTag, LPCSTR sTagChild, double d)
{
   CString s;
   s.Format("%lf",d);
   SetValue(sTag, sTagChild, s);
}

///////////////////////////////////////////////////////////////////////////////

void CXMLObj::SetValue(LPCSTR sTag, LPCSTR sTagChild, CDateTime date)
{
   CString s;
   date.DateAsString(s);
   SetValue(sTag, sTagChild, s);
}

///////////////////////////////////////////////////////////////////////////////

void CXMLObj::SetValue(LPCSTR sTag, LPCSTR sTagChild, LOGFONT& logfont)
{
   CString s = AsString(&logfont);
   SetValue(sTag, sTagChild, s);
}

///////////////////////////////////////////////////////////////////////////////

CXMLObj& CXMLObj::operator=(CXMLObj& rSrc)
{
   if (m_pNext != NULL) delete m_pNext;
   if (m_pChild != NULL) delete m_pChild;
   m_pNext = NULL;
   m_pChild = NULL;

   m_sName = rSrc.m_sName;
   m_sValue = rSrc.m_sValue;
   m_aAttr.Copy(rSrc.m_aAttr);
   
   if (rSrc.m_pChild != NULL)
   {
      m_pChild = new CXMLObj;
      *m_pChild = *rSrc.m_pChild;
   }
   if (rSrc.m_pNext != NULL)
   {
      m_pNext = new CXMLObj;
      *m_pNext = *rSrc.m_pNext;
   }
   return *this;
}

///////////////////////////////////////////////////////////////////////////////
//
// Iterate through an XML object

BOOL CXMLObj::GetFirst(LPCSTR sTag, LPCSTR sTagChild, CXMLObj*& pXMLObj)
{
   pXMLObj = GetXMLObj(sTag, sTagChild);   
   if (pXMLObj != NULL)
   {
       return TRUE;
   } else
   {
      return FALSE;
   }
}

///////////////////////////////////////////////////////////////////////////////
//
// Iterate through an XML object

BOOL CXMLObj::GetNext(LPCSTR sTag, LPCSTR sTagChild, CXMLObj*& pXMLObj)
{
   pXMLObj = pXMLObj->m_pNext;
   while (pXMLObj != NULL)
   { 
      if (pXMLObj->m_sName == sTagChild) return TRUE;
        pXMLObj = pXMLObj->m_pNext;

   }
   return FALSE;
}
