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

#include "crecordx.h"
#include "nrdbase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRecordsetExt

#ifdef NRDBMYSQL
IMPLEMENT_DYNAMIC(CRecordsetExt, CMySQLRecordset)
IMPLEMENT_DYNAMIC(CMySQLRecordset, CObject);
#else
IMPLEMENT_DYNAMIC(CRecordsetExt, CRecordset)
#endif

///////////////////////////////////////////////////////////////////////////////

CRecordsetExt::CRecordsetExt(CNRDBase* pdb)
#ifdef NRDBMYSQL
   : CMySQLRecordset(pdb)
#else
   : CRecordset(pdb)
#endif
{                   
   ASSERT(pdb != NULL);
   m_bBound = FALSE;      
};

///////////////////////////////////////////////////////////////////////////////
//
// CRecordsetExt::~CRecordsetExt()
//
// Nb. According to Knowledge Base Article: Q113815, record sets are not
// closed because of a bug in MFC 2.0.  In the destructor, m_pDatabase is set
// to NULL when the database was passed into the destructor.  This prevents the
// recordset from being close.  The fix is to explicitly close the record set.
//

CRecordsetExt::~CRecordsetExt()
{
   if (IsOpen())
   {
      Close();
   }    
};

CString CRecordsetExt::GetDefaultConnect()
{
   return "";
}

CString CRecordsetExt::GetDefaultSQL()
{
   return "";
}

void CRecordsetExt::DoFieldExchange(CFieldExchange* pFX)
{
}


///////////////////////////////////////////////////////////////////////////////
//
// BOOL CRecordsetExt::BEMGeneric(hConnect, iFlag)
//
// Generic function for retrieving, adding and deleting from the database
//                               
// hConnect o database connection handle
//
// pRecordSet o pointer to record set with virtual functions allowing calls to 
//              subclasses
//
// iFlag o database operation
//       
//

BOOL CRecordsetExt::BEMGeneric(BDHANDLE hConnect, int iFlag)
{
   CString strFormat;
   BOOL bOK = TRUE;
   
   CNRDBase* pDBase = GetNRDBase(hConnect);
   if (pDBase == NULL)
   { 
      bOK = FALSE;
   };
  
   TRY
   {
                                                   
  // Bind variables for return values, reset record set
     
     if (iFlag != BDGETNEXT && iFlag != BDINSERTNEXT)         
     {        
        if (iFlag != BDUPDATE && iFlag != BDDELETE && iFlag != BDADD && 
            iFlag != BDINSERTINIT)
        {
           m_bBound = TRUE;
        } 
        else
        {
           m_bBound = FALSE;
        }
        
        if (IsOpen())
        { 
           Close();
        };
     };
  
  // Create record set and open it
      
      if (bOK && !IsOpen())
      {  
  
  
  // Set any filters for selecting a specific value
  
         if (bOK && (iFlag == BDSELECT || iFlag == BDUPDATE || iFlag == BDDELETE ||
                     iFlag == BDSELECT2 || iFlag == BDSELECT3 || iFlag == BDSELECT4 || iFlag == BDSELECT5))
         {            
            m_strFilter = GetFilter();
         };
  
  // Set sort order
  
         if (bOK && (iFlag == BDSELECT || iFlag == BDGETINIT || iFlag == BDSELECT2 || 
                     iFlag == BDSELECT3 || iFlag == BDSELECT4 || iFlag == BDSELECT5))
         {            
            m_strSort = GetSortOrder();
         }; 
             
  // Open record set
          
         Open();         
      };
  
  
  // Check record set is not empty
      
      if (bOK && (iFlag == BDGETNEXT || iFlag == BDSELECT || iFlag == BDUPDATE ||
                  iFlag == BDDELETE || iFlag == BDGETINIT || iFlag == BDSELECT2 || 
                  iFlag == BDSELECT3 || iFlag == BDSELECT4 || iFlag == BDSELECT5))
      {
         if (IsEOF())
         { 
            bOK = FALSE;
         };
      } 
      
  // Get next and check for end of record set
     
      if (bOK && iFlag == BDGETNEXT )
      {   
         
         MoveNext();  
  
         if (IsEOF())
         { 
            bOK = FALSE;
         };
      } 
  
  // Setup for adding
            
      else if (bOK && (iFlag == BDADD || iFlag == BDINSERTINIT || iFlag == BDINSERTNEXT))         
      {                                
         AddNew();            
      }
         
   // Setup for updating
         
      else if (bOK && iFlag == BDUPDATE)
      {                                 
         if (CanUpdate())
         {
            Edit(); // changes current record
         } else
         {              
            bOK = FALSE;
         };
      };
 
  // Bind variables to be inserted into database and update
  // Nb. Update returns FALSE if nothing is updated but this may occur e.g.
  //     if same value is retained in a dialog box
          
      if (bOK && (iFlag == BDADD || iFlag == BDUPDATE || iFlag == BDINSERTINIT || iFlag ==
          BDINSERTNEXT))
      {                                         
         SetVariables();         

         Update();                   
      }
         
  // Delete
   
      else if (bOK && (iFlag == BDDELETE))
      {     
         Delete();
      }; 
         
  // Close and delete CRecord, close if an error occurs including end found
  // Necessary to allow calls to BDGETINIT  
  
      if (!bOK || iFlag == BDUPDATE || iFlag == BDDELETE)
      {        
         Close();   
      };  
   } 
   
 // Handle excpeptions
   

   CATCH (CDBException, pEx)
   {
      if (pDBase->m_nOnError == DisplayMessage)
      {
         AfxMessageBox(pEx->m_strError);            
      }
      else if (pDBase->m_nOnError == ThrowException)
      {
#ifdef _DEBUG
          AfxMessageBox("Debug: " + pEx->m_strError);            
#endif
         pDBase->m_sError = pEx->m_strError;
         THROW_LAST();
      } 
      bOK = FALSE;
   }
   END_CATCH
            
   return bOK;
};

