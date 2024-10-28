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
#include <strstream>
#include <math.h>
#include "nrdbapi.h"
#include "attribute.h"

///////////////////////////////////////////////////////////////////////////////

inline CString BDString(int nId)
{
   CString s;
   s.LoadString(nId);
   return s;
}

///////////////////////////////////////////////////////////////////////////////

CFTypeAttr::CFTypeAttr()
{
   m_lFType = 0;
   m_lDataType = 0;
   m_lFTypeLink = 0;
   m_lAttrId = 0;   
   m_bPrimaryKey = FALSE;
}

CFTypeAttr::CFTypeAttr(CFTypeAttr& rSrc)
{
   *this = rSrc;
};

CFTypeAttr& CFTypeAttr::operator=(CFTypeAttr& rSrc)
{
   m_sDesc = rSrc.m_sDesc;
   m_sColName = rSrc.m_sColName;
   m_lDataType = rSrc.m_lDataType;
   m_lFTypeLink = rSrc.m_lFTypeLink;
   m_lAttrId = rSrc.m_lAttrId;
   m_lFType = rSrc.m_lFType;
   m_bPrimaryKey = rSrc.m_bPrimaryKey;   
   return *this;
}



///////////////////////////////////////////////////////////////////////////////

CAttribute::CAttribute(CFTypeAttr& ftypeattr) : CFTypeAttr(ftypeattr)
{
   Init();
};

CAttribute::CAttribute(long lDataType)
{
   m_lDataType = lDataType;
   Init();     
};

void CAttribute::Init()
{   
   m_pData = NULL; 
   m_pDataInt = NULL;    
   if (m_lDataType != 0) AllocInt();
}

CAttribute::~CAttribute() 
{
   FreeInt();
}

///////////////////////////////////////////////////////////////////////////////

CAttribute::CAttribute(CAttribute& rSrc)
{
   Init();
   *this = rSrc;
}

///////////////////////////////////////////////////////////////////////////////

CAttribute& CAttribute::operator=(CAttribute& rSrc)
{
   FreeInt();
   
   this->CFTypeAttr::operator=(rSrc);
      
   AllocInt();
   if (m_lDataType == BDNUMBER) SetDouble(*rSrc.GetDouble());
   else if (m_lDataType == BDTEXT) SetString(rSrc.GetString());
   else if (m_lDataType == BDLINK) SetLink(*rSrc.GetLink());
   else if (m_lDataType == BDMAPLINES || m_lDataType == BDIMAGE || m_lDataType == BDFILE) CopyLongBinary(*rSrc.GetLongBinary());      
   else if (m_lDataType == BDCOORD) SetCoord(*rSrc.GetCoord());
   else if (m_lDataType == BDBOOLEAN) SetBoolean(*rSrc.GetBoolean());
   else if (m_lDataType == BDDATE) SetDate(*rSrc.GetDate());
   else if (m_lDataType == BDHOTLINK) SetFile(*rSrc.GetFile());
   else if (m_lDataType == BDLONGTEXT) SetLongText(*rSrc.GetLongText());
   else ASSERT(m_lDataType < 0);
   
   return *this;
}

///////////////////////////////////////////////////////////////////////////////

void CAttribute::CopyLongBinary(CLongBinary& rSrc)
{
   CLongBinary* pDest = GetLongBinary();

   if (pDest->m_hData != NULL) 
   {      
      GlobalFree(pDest->m_hData);
   };
   pDest->m_hData = NULL;
   pDest->m_dwDataLength = 0;
   
   if (rSrc.m_hData != NULL && rSrc.m_dwDataLength != 0)
   {
      DWORD dwSize = rSrc.m_dwDataLength;
      pDest->m_hData = GlobalAlloc(GMEM_MOVEABLE|GMEM_DISCARDABLE, dwSize);         
      pDest->m_dwDataLength = dwSize;
      if (pDest != NULL)
      {
         BYTE* pcDest = (BYTE*)GlobalLock(pDest->m_hData);
         BYTE* pcSrc = (BYTE*)GlobalLock(rSrc.m_hData);

         if (pcDest != NULL && pcSrc != NULL)
         {
            memcpy(pcDest,pcSrc,dwSize);
         };
         GlobalUnlock(pDest->m_hData);
         GlobalUnlock(rSrc.m_hData);         
      }
   };
}

///////////////////////////////////////////////////////////////////////////////

BOOL CAttribute::operator==(CAttribute& rSrc)
{
   // Compare names

   if (this->GetDesc() != rSrc.GetDesc())
   {
	   return FALSE;
   }
	
  // Different data types

   if (GetDataType() != rSrc.GetDataType())
   {
	   return FALSE;
   }

   // If the data type is link, check that the linked ftype is the same

   if (GetDataType() == BDLINK && GetFTypeLink() != 
	   rSrc.GetFTypeLink())
   {
	   return FALSE;
   }

   // The same

   return TRUE;				

}

///////////////////////////////////////////////////////////////////////////////

BOOL CAttribute::operator!=(CAttribute& rSrc)
{
	return !(rSrc==*this);
}

///////////////////////////////////////////////////////////////////////////////
//
// Outputs an attribute as a string
//

CString CAttribute::AsString()
{
   CString s;

   ASSERT(m_lDataType < 0 || m_pDataInt != NULL);

   if (m_lDataType == BDNUMBER)
   {
      if (!IsNull())
      {             
		  std::strstream str;
         str.precision(10);
		 str << *GetDouble() << std::ends;
	      s = str.str();
         str.rdbuf()->freeze(0);

        // Insert commas into number

         if (s.FindOneOf("eE") == -1)
         {
            // Find decimal point or end of number
            int i = s.Find(".");
            if (i == -1) i = s.GetLength()-1;
            else i--;

            // Work backwards adding commas, if first character is a negative sign
            // don't add a comma before it!

            i -=3;
            int j = 0;
            if (s[0] == '-') j = 1;
            for (; i >= j; i-=3)
            {
               s = s.Left(i+1) + "," + s.Mid(i+1);                  
            }         
         };
      };
   }
   else if (m_lDataType == BDTEXT)
   {
	   s = GetString();
   }
   else if (m_lDataType == BDMAPLINES) 
   {
	   s = "[Polylines]";    
   }
   else if (m_lDataType == BDIMAGE)
   {
      s = "[Image]";
   }
   else if (m_lDataType == BDFILE)
   {
      s = "[File]";
   }
   else if (m_lDataType == BDLINK) 
   {
	   s = internalinfo.m_links.GetFeature(*GetLink(), m_lFTypeLink);
   }
   else if (m_lDataType == BDCOORD)
   {
	   CCoord coord = *GetCoord();
	   if (!coord.IsNull())
	   {	   	            
         s.Format("%.2lf, %.2lf",coord.x, coord.y);
	   };
   }
   else if (m_lDataType == BDBOOLEAN)
   {
      if (*GetBoolean() == 1) s = "Yes";  
      else if (*GetBoolean() == 0) s = "No"; 
	   else s = "";
   } 
   else if (m_lDataType == BDFEATURE)
   {
      s = m_sDesc;
   }
   else if (m_lDataType == BDDATE)
   {
      s = m_sDesc;
   }
   else if (m_lDataType == BDHOTLINK)
   {
      s = *GetFile();
   }
   else if (m_lDataType == BDLONGTEXT)
   {
	  s = *GetLongText();
   }
   else 
   {
      ASSERT(FALSE);
   }

   return s;
}

/////////////////////////////////////////////////////////////////////////////
//
// Converts the string to the current data type 
//

BOOL CAttribute::AsAttr(CString s)
{
   char ch;
   double d;
   BOOL bComma = FALSE;

   s.TrimRight();

   if (m_lDataType == BDNUMBER)
   {
      // Check for null value

      if (s == "")
      {
         SetDouble(AFX_RFX_DOUBLE_PSEUDO_NULL);
         return TRUE;
      }

      // Remove correctly positioned commas
      
      int i = s.Find(".");
      if (i == -1) i = s.GetLength()-1;
      else i--;
      i -=3;
      for (; i >= 0; i-=3)
      {        
         if (s[i] == ',') 
         {            
            s = s.Left(i) + s.Mid(i+1);      
            i--;
            bComma = TRUE;
         }
         else if (bComma)
         {
            return FALSE;
         }
      }
      
      // Convert to double
      
      if (sscanf(s, "%lf%c", &d,&ch) == 1)
      {
         SetDouble(d);
         return TRUE;
      }
   }
   
   return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Returns TRUE if contains a null value.  Note for some data types no null value
// exists
//

BOOL CAttribute::IsNull()
{
   ASSERT(m_lDataType < 0 || m_pDataInt != NULL);

   if (m_lDataType == BDNUMBER)
   {   
      return IsNullDouble(*GetDouble());
   }
   else if (m_lDataType == BDTEXT)
   {
	   return FALSE;
   }
   else if (m_lDataType == BDLONGTEXT)
   {
	   return FALSE;
   }
   else if (m_lDataType == BDMAPLINES || m_lDataType == BDIMAGE || m_lDataType == BDFILE) 
   {
	   return m_pData == NULL ||
             GetLongBinary()->m_hData == NULL ||
             GetLongBinary()->m_dwDataLength == 0;
   }
   else if (m_lDataType == BDLINK) 
   {
	   return *GetLink() == 0 || *GetLink() == AFX_RFX_LONG_PSEUDO_NULL;
   }
   else if (m_lDataType == BDCOORD)
   {
	   CCoord coord = *GetCoord();
	   return coord.IsNull();	   
   }
   else if (m_lDataType == BDBOOLEAN)
   {
      return FALSE;
   } 
   else if (m_lDataType == BDFEATURE)
   {
      return FALSE;
   }
   else if (m_lDataType == BDDATE)
   {
      return FALSE;
   }
   else if (m_lDataType == BDHOTLINK)
   {
      return FALSE;
   }
   else 
   {
      ASSERT(FALSE);
      return FALSE;
   }
}

/////////////////////////////////////////////////////////////////////////////

CString CFTypeAttr::GetDesc()
{
   if (m_lDataType == BDFEATURE)
   {     
      return internalinfo.m_aFType.GetFType(GetFTypeId());
   } 
   else if (m_lDataType == BDDATE)
   {
      return "Date";
   }
   else
   {
      return m_sDesc;
   };
}

/////////////////////////////////////////////////////////////////////////////

void CAttribute::AllocInt()
{
   ASSERT(m_lDataType != 0);

   if (m_lDataType == BDNUMBER) m_pDataInt = new double;         
   else if (m_lDataType == BDTEXT) m_pDataInt = new CNRDBString;
   else if (m_lDataType == BDLINK) m_pDataInt = new long;
   else if (m_lDataType == BDMAPLINES || m_lDataType == BDIMAGE || m_lDataType == BDFILE) m_pDataInt = new CLongBinary;
   else if (m_lDataType == BDCOORD) m_pDataInt = new CCoord;
   else if (m_lDataType == BDBOOLEAN) m_pDataInt = new BOOL;
   else if (m_lDataType == BDDATE) m_pDataInt = new long;
   else if (m_lDataType == BDHOTLINK) m_pDataInt = new CNRDBString;
   else if (m_lDataType == BDLONGTEXT) m_pDataInt = new CLongBinary;
   else ASSERT(m_lDataType < 0);

   if (m_lDataType == BDNUMBER) m_pData = new double;
   else if (m_lDataType == BDTEXT) m_pData = new char[BD_SHORTSTR];
   else if (m_lDataType == BDLINK) m_pData = new long; 
   else if (m_lDataType == BDMAPLINES || m_lDataType == BDIMAGE || m_lDataType == BDFILE) m_pData = new CLongBinary;
   else if (m_lDataType == BDCOORD) m_pData = new CCoord;
   else if (m_lDataType == BDBOOLEAN) m_pData = new BOOL;
   else if (m_lDataType == BDDATE) m_pData = new long;
   else if (m_lDataType == BDHOTLINK) m_pData = new CString;
   else if (m_lDataType == BDLONGTEXT) m_pData = new CString;
   else ASSERT(m_lDataType < 0);

   // Initialise

   if (m_lDataType == BDNUMBER) {SetDouble(AFX_RFX_DOUBLE_PSEUDO_NULL); InitDoubleInt();}
   else if (m_lDataType == BDLINK) {SetLink(0); InitLinkInt();}
   else if (m_lDataType == BDBOOLEAN) {SetBoolean(FALSE); InitBooleanInt();}
   else if (m_lDataType == BDDATE) {SetDate(0); InitDateInt();}
   else if (m_lDataType == BDTEXT) {SetString("");}      

};

/////////////////////////////////////////////////////////////////////////////

void CAttribute::FreeInt()
{
   if (m_pDataInt != NULL)
   {
      // Free memory for long binary objects for internal      

      if (m_lDataType == BDMAPLINES || m_lDataType == BDIMAGE || m_lDataType == BDLONGTEXT ||
          m_lDataType == BDFILE)
      {
         CLongBinary* pLongBin = ((CLongBinary*) m_pDataInt);
         if (pLongBin->m_hData != NULL) 
         {            
            GlobalFree(pLongBin->m_hData);
			pLongBin->m_hData = NULL;
         };
      };

      if (m_lDataType == BDNUMBER) delete ((double*)m_pDataInt);
      else if (m_lDataType == BDTEXT) delete ((CNRDBString*)m_pDataInt);
	  else if (m_lDataType == BDLINK) delete ((long*)m_pDataInt);      
	  else if (m_lDataType == BDCOORD) delete ((CCoord*) m_pDataInt);
	  else if (m_lDataType == BDBOOLEAN) delete ((BOOL*)m_pDataInt);
      else if (m_lDataType == BDDATE) delete ((long*)m_pDataInt);
      else if (m_lDataType == BDHOTLINK) delete ((CNRDBString*)m_pDataInt);	  
     else if (m_lDataType == BDMAPLINES || m_lDataType == BDIMAGE || 
              m_lDataType == BDLONGTEXT || m_lDataType == BDFILE)
     {
        delete ((CLongBinary*) m_pDataInt);
     }
	  else ASSERT(m_lDataType < 0);

      m_pDataInt = NULL;
   }

   if (m_pData != NULL)
   {
      // Free memory for long binary objects

      if (m_lDataType == BDMAPLINES || m_lDataType == BDIMAGE || m_lDataType == BDFILE)
      {
         CLongBinary* pLongBin = ((CLongBinary*) m_pData);
         if (pLongBin->m_hData != NULL) 
         {            
            GlobalFree(pLongBin->m_hData);
			pLongBin->m_hData = NULL;
         };
      };

      // Free other memory

     if (m_lDataType == BDNUMBER) delete ((double*)m_pData);
	 else if (m_lDataType == BDLINK) delete ((long*)m_pData);
     else if (m_lDataType == BDTEXT) delete [] ((char*)m_pData);
     else if (m_lDataType == BDMAPLINES || m_lDataType == BDIMAGE || m_lDataType == BDFILE) delete ((CLongBinary*) m_pData);
	 else if (m_lDataType == BDCOORD) delete ((CCoord*) m_pData);
     else if (m_lDataType == BDBOOLEAN) delete ((BOOL*)m_pData);
     else if (m_lDataType == BDDATE) delete ((long*)m_pData);
     else if (m_lDataType == BDHOTLINK) delete ((CString*)m_pData);
	 else if (m_lDataType == BDLONGTEXT) delete ((CString*)m_pData);
	 else ASSERT(m_lDataType < 0);

      m_pData = NULL;
   }
}

/////////////////////////////////////////////////////////////////////////////

CAttrArray::CAttrArray()
{
   m_lDate = 0;
   m_lOrder = 0;
   m_lFType = 0;
   m_lFeature = 0;
}

/////////////////////////////////////////////////////////////////////////////

CAttrArray::CAttrArray(CAttrArray& rSrc)
{
   *this = rSrc;
}

/////////////////////////////////////////////////////////////////////////////

CAttrArray& CAttrArray::operator=(CAttrArray& rSrc)
{
   m_lDate = rSrc.m_lDate;
   m_lOrder  = rSrc.m_lOrder;
   m_lFType = rSrc.m_lFType;
   m_lFeature = rSrc.m_lFeature;
   m_sInternal = rSrc.m_sInternal;
   m_sName = rSrc.m_sName;

   RemoveAllX();
   for (int i = 0; i < rSrc.GetSize(); i++)
   {
      CAttribute* pAttr = new CAttribute(*rSrc.GetAt(i));      
      Add(pAttr);
   }
   return *this;
}

/////////////////////////////////////////////////////////////////////////////

CAttrArray::~CAttrArray()
{   
   RemoveAllX();
}

/////////////////////////////////////////////////////////////////////////////

CAttrArray::RemoveAllX()
{
   for (int i = 0; i < GetSize(); i++)
   {
      if (GetAt(i) != NULL)
      {
         delete GetAt(i);
      };
   }
   RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////

int CAttribute::GetNumFields()
{
   switch (m_lDataType)
   {
      case BDNUMBER : case BDLINK : case BDTEXT: case BDMAPLINES : 
      case BDBOOLEAN : case BDDATE : case BDHOTLINK : case BDIMAGE : 
	   case BDLONGTEXT : case BDFILE :
		  return 1;						
	  case BDCOORD : return 2;
	  default : ASSERT(FALSE);
   }
	
   return 0;
}

/////////////////////////////////////////////////////////////////////////////

BOOL BDFTypeAttrInit(BDHANDLE hConnect, long lFType, CAttrArray* pArray)
{
   BOOL bOK = TRUE;   
   CFeatureType ftype;

   ftype.m_lId = lFType;
   pArray->m_lFType = lFType;

   pArray->RemoveAllX();

   // Retrieve the feature type name

   bOK = BDFeatureType(hConnect, &ftype, BDSELECT);
   BDEnd(hConnect);

   if (bOK)
   {
	   pArray->SetFTypeName(ftype.m_sDesc);      
      pArray->SetFTypeInternal(ftype.m_sInternal);      
        
	   // Retrieve the attributes corresponding to the feature type
   	   
      CFTypeAttr ftypeattr;
      ftypeattr.m_lFType = lFType;

	   BOOL bFound = BDFTypeAttr(hConnect, &ftypeattr, BDSELECT2);
	   while (bFound)
	   {
		  // For each attribute allocate an object to describe the type 
		  // and name of the attribute and for storing its value
              
		  CAttribute* pAttr = new CAttribute(ftypeattr);		 
        pArray->Add(pAttr);

		  bFound = BDGetNext(hConnect);
	   }
	   BDEnd(hConnect);
   };
      
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

CString CAttrArray::GetFTypeInternal(CNRDBase* pDBase) 
{      
   if (pDBase->GetDBVersion() > 1 && pDBase->GetDBVersion() != AFX_RFX_INT_PSEUDO_NULL)
   {
      return m_sInternal + "_DATA";
   } else
   {
      return m_sInternal;
   }
}

///////////////////////////////////////////////////////////////////////////////

CString CAttrArray::GetFTypeTable(CNRDBase* pDBase) 
{   
   if (pDBase->GetDBVersion() > 1)
   {
      return m_sInternal;
   } else
   {
      CString s;
      s.Format("FEATURE%li", m_lFType);
      return s;
   }
}

