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

BOOL BDGetNext(BDHANDLE hConnect)
{
   BOOL bOK = TRUE;
   CNRDBase* pDBase = GetNRDBase(hConnect); 
   if (pDBase == NULL)
   { 
      bOK = FALSE;
   };
    
  // Calling MoveNext copies the values into the variables used for dynamic 
  // data exchange but not into the variables passed in the call to BDGETINIT.
  // It is therefore necessary to overide the virtual function DoFieldExchange
  // so that the values are returned for each CRecordSet subclass
   
   if (bOK)
   {     
      TRY 
      { 
         if (pDBase->GetRecordSetExt()->IsEOF() == FALSE)
         {                                             
         
  // Calls overidden DoFieldExchange
         
            pDBase->GetRecordSetExt()->MoveNext();      
         } else
         { 
            ASSERT(FALSE);
            bOK = FALSE;            
         };
      }

      CATCH (CDBException, pEx)
      {
         bOK = FALSE;          
         AfxMessageBox(pEx->m_strError);         
      }
      END_CATCH;
      
   } else
   { 
      bOK = FALSE;      
   };             
 
 // Check if scrolled beyond end
   
   if (bOK && pDBase->GetRecordSetExt()->IsEOF())
   { 
      bOK = FALSE;
   };
   
   return bOK;
}