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
#include "nrdbapi.h"

///////////////////////////////////////////////////////////////////////////////

BOOL BDBeginTrans(BDHANDLE hConnect)
{
   CNRDBase* pDBase = GetNRDBase(hConnect); 
     
   if (pDBase != NULL)
   {               
     if (pDBase->CanTransact())
     {   
         if (pDBase->m_nTransLevel == 0)
         {
            pDBase->BeginTrans();                           
         };
         pDBase->m_nTransLevel++;
         return TRUE;
      }
      else
      { 
         return FALSE;
      };
   } else
   {      
      return FALSE;
   };   
}

///////////////////////////////////////////////////////////////////////////////

BOOL BDCommit(BDHANDLE hConnect)
{
  CNRDBase* pDBase = GetNRDBase(hConnect); 
  BOOL bOK = TRUE;
  
  if (pDBase != NULL)  
  {           
     pDBase->DeleteRecordSet();
   
     if (pDBase->CanTransact())        
     {         
        pDBase->m_nTransLevel--;
        if (pDBase->m_nTransLevel == 0)
        {
           pDBase->CommitTrans();                    
        };
        ASSERT(pDBase->m_nTransLevel >= 0);
     };   
  } else
  {
     return FALSE;
  }
    
  return bOK;
}

///////////////////////////////////////////////////////////////////////////////

BOOL BDRollBack(BDHANDLE hConnect)
{
   BOOL bOK = TRUE;
   CNRDBase* pDBase = GetNRDBase(hConnect); 
     
   if (pDBase != NULL)
   {                         
     if (pDBase->CanTransact())              
     {        
        pDBase->Rollback();       
        pDBase->m_nTransLevel = 0;
     };   
   } else
   {
      return FALSE;
   }
   
  return bOK;
}

///////////////////////////////////////////////////////////////////////////////

int BDGetTransLevel(BDHANDLE hConnect)
{
   CNRDBase* pDBase = GetNRDBase(hConnect); 
   return pDBase->m_nTransLevel;
}