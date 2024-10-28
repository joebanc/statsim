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
#include "nrdbapi.h"
#include "nrdb.h"

///////////////////////////////////////////////////////////////////////////////
// 
// Returns the next id given the following types
//
// BDFTYPE 
// BDFEATURE FTypeId
// BDDICTIONARY 
//

BOOL BDNextId(BDHANDLE hConnect, int nType, long lId, LPLONG plNextId)
{
   CString s;
   CAttrArray aAttr;

   CNRDBase* pDBase = GetNRDBase(hConnect); 
   ASSERT(pDBase != NULL);

   if (nType == BDFTYPE)
   {
      return BDNextId(hConnect, "`BDFeature_Types`", plNextId, TRUE);
   } 
   else if (nType == BDDICTIONARY)
   {
      return BDNextId(hConnect, "`BDDictionary`", plNextId, TRUE);
   }
   else if (nType == BDFEATURE)
   {      
      BDFTypeAttrInit(hConnect, lId, &aAttr);
      s.Format("`%s`", (LPCSTR)aAttr.GetFTypeTable(pDBase));
      return BDNextId(hConnect, s.GetBuffer(0), plNextId, TRUE);   	  
   }
   else if (nType == BDSYMBOL)
   {
      return BDNextId(hConnect, "`BDSymbol`", plNextId, TRUE);
   }
   return FALSE;
   
}

///////////////////////////////////////////////////////////////////////////////

BOOL BDNextId (BDHANDLE hConnect, LPSTR sTableName,
               LPLONG plNextId, BOOL bPositive)
{  
   BOOL bOK = TRUE;
   long lNext = 0;   
   CString sSQL;
 
 // Get database handle
      
   CNRDBase* pBEMDBase = GetNRDBase(hConnect); 
   if (pBEMDBase == NULL)
   { 
      bOK = FALSE;
   };

  // Find next id 
  
   if (bOK)
   {
   
  // Create SQL string
    
      if ( bPositive )    
      {
         sSQL.Format("select max(ID) from %s", sTableName);    
      } else
      {
         sSQL.Format("select min(ID) from %s", sTableName);
      };
       
      bOK =  BDExecRetSQL(hConnect, sSQL.GetBuffer(0), &lNext);     
   };      
  
  // Determine next id
  
   if (bOK)
   {  
      *plNextId = lNext;       
      if ( bPositive )
      {  
         if (*plNextId <= 0L  )
         {
            *plNextId = 1L;
         } else
         {
            (*plNextId)++;
         }
      } else
      {
         if ( *plNextId >= 0L )
         {       
            *plNextId = -1L;
         } else
         {
            (*plNextId)--;
         };
      }
   };
   
   return bOK;
};

