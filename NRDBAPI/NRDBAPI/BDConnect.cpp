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
#include "nrdbase.h"

///////////////////////////////////////////////////////////////////////////////
//
// BOOL BDConnect()
//
// Returns 0 if failed, 1 if successful and -1 is connected but not a valid
// NRDB Database
//

BOOL BDConnect(LPCSTR psDBase, BDHANDLE* pDBHandle, LPCSTR sUser, LPCSTR sPassword, LPCSTR sHost)
{   
   BOOL bOK = TRUE;   
   CString sError;
   BOOL bConnect = FALSE;
   
   CNRDBase *pDBase = NULL;   
        
 // Assign memory for this connection 
   
   if (bOK)
   {
      pDBase = new CNRDBase;
    
      if (pDBase == NULL)
      {         
         sError = "Unable to allocate memory";
         bOK = FALSE;
      };
   };
    
 // Use the global connect to identify where the database is stored 

   TRY
   {  

   if (bOK)
   {
      *pDBHandle = (BDHANDLE)(pDBase);          
               
     // Connect to the database
        
      CString sConnect = psDBase;
                                         
      bOK = pDBase->Open(sConnect, sUser, sPassword, sHost);     

      bConnect = bOK;
                      
    };


// Determine the version number of the database

   if (bOK)
   {
      pDBase->m_nOnError = ThrowException;

      CBDMain main;
      BDMain(*pDBHandle, &main, BDGETINIT);
      BDEnd(*pDBHandle);
      pDBase->m_nDBVersion = main.m_nVersion;

      // Update tables to current version

      BDUpdate(pDBase);

      pDBase->m_nOnError = DisplayMessage;

      internalinfo.m_nConnections++;
   }

// If okay then make an internal connection

   if (bOK && internalinfo.m_hConnectInt == NULL)
   {	   
      CNRDBase *pDBaseInt = new CNRDBase;        
      if (pDBaseInt->Open(psDBase, sUser, sPassword, sHost))
      {
         internalinfo.m_hConnectInt = (BDHANDLE)pDBaseInt;
         pDBaseInt->m_nDBVersion = pDBase->m_nDBVersion;
      } else
      {
         bOK = FALSE;
      }
   }
   
  }
  CATCH (CDBException, pEx)       
  {   
     bOK = FALSE;
     sError = pEx->m_strError;          
  }  
  END_CATCH

     // Display error messages and tidy up on error
  
  if (!bOK)
  {
     if (sError != "" && !bConnect)
     {
        AfxMessageBox(sError);
     };

     // Tidy up on error

     if (pDBase != NULL)
     {                                 
        pDBase->Close();
     }       
  };

  // Set return values

  if (!bOK)
  {
     *pDBHandle = (BDHANDLE)-1;
     if (bConnect) return -1;
  }


  return bOK;
}
