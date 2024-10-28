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
// BOOL BDDisconnect(BDHANDLE)
//

BOOL BDDisconnect(BDHANDLE hConnect)
{
    BOOL bOK = TRUE;
    
    CNRDBase* pDBase = GetNRDBase(hConnect);
    
    if (pDBase != NULL)
    {
       pDBase->DeleteRecordSet();
   
   // Close the database
         
       if (pDBase->IsOpen())
       {
          pDBase->Close();          
       } else
       {      
          bOK = FALSE;
       };
       delete pDBase;

       internalinfo.m_nConnections--;
    };

    // If this is the last connection then disconnect the internal
    // connection

    if (internalinfo.m_hConnectInt != NULL && internalinfo.m_nConnections == 0)
    {
       CNRDBase *pNRDBaseInt = GetNRDBase(internalinfo.m_hConnectInt);
       if (pNRDBaseInt != NULL)
       {
          pNRDBaseInt->Close();
          delete pNRDBaseInt;
          internalinfo.m_hConnectInt=NULL;
       }
    };
     
   return bOK;
};
