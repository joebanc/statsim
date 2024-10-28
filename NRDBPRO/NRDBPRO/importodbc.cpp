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
#include <io.h>
#include <fcntl.h>
#include <odbcinst.h>

#include "nrdb.h"
#include "importodbc.h"
#include "dlgimportodbc.h"
#include "dlgimportfeature.h"
#include "bdimportexport.h"
#include "dlgimportstatus.h"
#include "utils.h"
#include "dlgprogress.h"
#include "dlgeditfeature.h"
#include "dlgimportdate.h"
#include "importmaplines.h"
#include "shapefile.h"
#include "importftype.h"
#include "mainfrm.h"
#include "dlgimportrename.h"
#include "dlgimportprimarykey.h"
#include "dlgprotect.h"

///////////////////////////////////////////////////////////////////////////////           

CImportColumn::CImportColumn()
{      
   m_bAttr = FALSE;
   m_bShapeFile = FALSE;
   m_sText[0] = '\0';
   m_lItemData = 0;
}

CImportColumn::CImportColumn(CImportColumn& rSrc)
{
   m_sNameImport = rSrc.m_sNameImport;
   m_sNameAttr = rSrc.m_sNameAttr;
   m_bAttr = rSrc.m_bAttr;
   m_bShapeFile = rSrc.m_bShapeFile;
   m_lItemData = rSrc.m_lItemData;
   strncpy(m_sText, rSrc.m_sText, BD_SHORTSTR);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

CImportFeature::CImportFeature()
{
   m_lFeature = 0;
   m_lFType = 0;
}

///////////////////////////////////////////////////////////////////////////////

CImportDB& CImportDB::operator=(CImportDB& rSrc)
{
   RemoveAll();
   int i = 0; for (i = 0; i < rSrc.GetSize(); i++) Add(rSrc[i]);   
   return *this;
}

///////////////////////////////////////////////////////////////////////////////

CImportDB::CImportDB()
{
   m_pDlgProgress = NULL;
}

///////////////////////////////////////////////////////////////////////////////
//
// Retrieve the data source and open it and the municipality that the ODBC
// data is to be imported for
//

void CImportDB::ImportODBC(LPCSTR sDataSource, LPCSTR sTable, BOOL bSysTables, int nFileType)
{
   CDatabase database;
   BOOL bCancel = FALSE;
   BOOL bNRDB = FALSE;
  
// Initialise variables

   m_nImported = 0;
   m_nFetched = 0;

// Open the log file

   m_sLogFile = GetTempFileName();
   m_pLogFile = fopen(m_sLogFile,"w");
   fprintf(m_pLogFile, BDString(IDS_IMPORTING) + "...\r\n");

// Start the transaction here so that OnCreate can also be cancelled

   if (BDGetTransLevel(BDHandle()) > 0) BDCommit(BDHandle());
   BDBeginTrans(BDHandle());


   // Open dialog to select data source and format

   CDlgImportODBC dlg(sDataSource, sTable, bSysTables, nFileType);
   int nRet = dlg.DoModal();

   // Begin the transaction

   BDOnError(BDHandle(), ThrowException);

   if (nRet == IDOK)
   {  
      *this = dlg.GetImportDB();      

     // Retrieve the format

      bNRDB = dlg.IsNRDB();
      if (bNRDB)
      {
          CImportFType importftype(this, sDataSource);
          m_nImported = importftype.Import();
          if (m_nImported < 0)
          {
             bCancel = TRUE;
          }
      }
      
      // Connect to the database

		else if (database.Open(dlg.GetDatabase()))
		{
         m_pDlgProgress = new CDlgProgress;

         int i = 0; for (i = 0; i < GetSize(); i++)
         {
            if (GetAt(i).m_bImport)
            {               
               GetAt(i).Import(database, *this, nFileType);
            };

            if (!m_pDlgProgress->SetPercent((100*i)/GetSize()))
            {
               bCancel = TRUE;
            }
         };
        
        // Close the database

			database.Close();

         delete m_pDlgProgress;
         m_pDlgProgress = NULL;
		};	      
   } else
   {
      bCancel = TRUE;
   }

   // Output the log file

   // Report number of records imported

   if (bCancel)
   {
      fprintf(m_pLogFile, BDString(IDS_IMPORTCANCEL) + "\r\n");                       

   } 
   else
   {      
      fprintf(m_pLogFile, BDString(IDS_NUMRECORDS) + ": %i\r\n", m_nImported);                       
      if (m_nImported == 0 && !bNRDB)
      {
         fprintf(m_pLogFile,"\r\n" + BDString(IDS_IMPORTTIP) + "\r\n");
      }
   };

   if (m_pLogFile != NULL) fclose(m_pLogFile);

   CDlgImportStatus dlgStatus(m_sLogFile, !bCancel && m_nImported != 0);

   if (dlgStatus.DoModal() == IDOK)
   {
      BDCommit(BDHandle());
   } else
   {
	  bCancel = TRUE;
      BDRollBack(BDHandle());
   }

   remove(m_sLogFile);
   BDOnError(BDHandle(), DisplayMessage);

   // Compact database

   if (m_nImported != 0 && !bCancel)
   {
      CompactDatabase();
   }

};


///////////////////////////////////////////////////////////////////////////////

CImportTable::CImportTable()
{   
   m_bImport = FALSE;   
   //m_lId = 0;
   m_pImportDB = NULL;
   m_lAttrIdShapefile = 0;
   m_bAutoCreate = FALSE;
};

CImportTable::CImportTable(CImportTable& rSrc)
{
   *this = rSrc;      
}

CImportTable& CImportTable::operator=(CImportTable& rSrc)
{	
   m_sTableImport = rSrc.m_sTableImport;
   m_sFType = rSrc.m_sFType;   
   m_bImport = rSrc.m_bImport;
   //m_lId = rSrc.m_lId;
   m_sFeature = rSrc.m_sFeature;
   m_sParentFeature = rSrc.m_sParentFeature;
   m_sGrandParentFeature = rSrc.m_sGrandParentFeature;
   m_sGreatGrandParentFeature = rSrc.m_sGreatGrandParentFeature;
   m_sShapeFile = rSrc.m_sShapeFile;
   m_bAutoCreate = rSrc.m_bAutoCreate;
   m_ftype = rSrc.m_ftype;

   m_aColumn.RemoveAll();
   int i = 0; for (i = 0; i < rSrc.m_aColumn.GetSize(); i++) m_aColumn.Add(rSrc.m_aColumn[i]);   
   return *this;
}
///////////////////////////////////////////////////////////////////////////////

BOOL CImportTable::Import(CDatabase& database, CImportDB& importDB, int nFileType)
{	
   SDWORD dwLength;   
   BOOL bOK = TRUE;
   CString sSQL;
   HSTMT hstmt;   
   int nRet;
   CFeatureType ftype;
   CAttrArray aAttr;      
   char sFeature[BD_SHORTSTR] = "";
   char sParentFeature[BD_SHORTSTR] = "";
   char sGrandParentFeature[BD_SHORTSTR] = "";
   char sGreatGrandParentFeature[BD_SHORTSTR] = "";
   char sDate[32] = "";
   char sID[32] = "";
   CDateTime date, dateDefault;
   CString sError;
   CString sProgress;
   m_nRecord = 1;   
   BOOL bCancel = FALSE;
   CMapLayer maplayer;   
   int nShape = 0;   
   m_nPrimaryKey = -1;

   m_pImportDB = &importDB;

// Retrieve attribute defintion for feature type from BEMO database

   ftype.m_sDesc = m_sFType;
   bOK = BDFeatureType(BDHandle(), &ftype, BDSELECT2);   
   if (bOK)
   {
      bOK = BDFTypeAttrInit(BDHandle(), ftype.m_lId, &aAttr);
   };   

// For DBase files construct a short file name

   CString sTable = m_sTableImport;
   if (nFileType == CImportDB::shapefile || nFileType == CImportDB::dbase)
   {
      sTable = GetShortFileTitle(database);
   }

// Count the number of records

   long lRecords = GetCountRecords(database.m_hdbc, sTable);

// Determine if shapefile data is to be loaded

   if (bOK) bOK = LoadShapefile(importDB, maplayer);
   
// Open statement
                          
   if (SQLAllocStmt(database.m_hdbc, &hstmt) == SQL_SUCCESS)
   {                 
      TRY
      {
		  // Construct SQL

       BOOL bFirst = TRUE;
       sSQL = "select ";
		 int i = 0; for (i = 0; i < m_aColumn.GetSize(); i++)
		 {			       
          if (!m_aColumn[i].m_bShapeFile)
          {
             if (!bFirst) sSQL += ", ";
			    sSQL += "`" + m_aColumn[i].m_sNameImport + "`";
             bFirst = FALSE;
          };
		 }
       if (sSQL == "select ") sSQL += "*";

       sSQL += " from `" + CString(sTable) + "`";

   // Execute statement
                        
       nRet = SQLExecDirect(hstmt, (UCHAR*)sSQL.GetBuffer(0), sSQL.GetLength()); 
      
   // Bind values to variables
      
       int iCol = 1;
		 for (i = 0; i < m_aColumn.GetSize() && nRet == SQL_SUCCESS; i++)
		 {
          // Handle date
       
          if (m_aColumn[i].m_sNameAttr == IMPORT_DATE)
          {
             nRet = SQLBindCol(hstmt, iCol, SQL_C_CHAR, sDate, sizeof(sDate), &dwLength);
          } 

          // Handle id

          else if (m_aColumn[i].m_sNameAttr == IMPORT_ID)
          {
             nRet = SQLBindCol(hstmt, iCol, SQL_C_CHAR, sID, sizeof(sID), &dwLength);
          } 
       
          // Retrieve feature name (if any)

          else if (m_aColumn[i].m_sNameAttr == m_sFeature + IMPORT_NAME)
          {
             nRet = SQLBindCol(hstmt, iCol, SQL_C_CHAR, sFeature, BD_SHORTSTR, &dwLength);                          
          }

          else if (m_aColumn[i].m_sNameAttr == m_sParentFeature  + IMPORT_NAME)
          {
             nRet = SQLBindCol(hstmt, iCol, SQL_C_CHAR, sParentFeature, BD_SHORTSTR, &dwLength);
          }

          else if (m_aColumn[i].m_sNameAttr == m_sGrandParentFeature + IMPORT_NAME)
          {
             nRet = SQLBindCol(hstmt, iCol, SQL_C_CHAR, sGrandParentFeature, BD_SHORTSTR, &dwLength);
          }
		  else if (m_aColumn[i].m_sNameAttr == m_sGreatGrandParentFeature + IMPORT_NAME)
          {
             nRet = SQLBindCol(hstmt, iCol, SQL_C_CHAR, sGreatGrandParentFeature, BD_SHORTSTR, &dwLength);
          }

          // Import the corresponding shapefile data

          else if ((m_aColumn[i].m_sNameImport == SHAPEFILE_POINTS || 
                   m_aColumn[i].m_sNameImport == SHAPEFILE_POLYLINES) && 
                   m_aColumn[i].m_sNameAttr != IMPORT_SKIP)
          {                           
             m_aColumn[i].m_bAttr = TRUE;
          }          

          // Skip columns not to be imported
       
          else if (m_aColumn[i].m_sNameAttr == IMPORT_SKIP)
          {            
          }
       
          // Bind columns

          else
          {
             m_aColumn[i].m_bAttr = TRUE;

             CAttribute* pAttr = NULL;
             int iAttr = GetAttr(aAttr, m_aColumn[i].m_sNameAttr);
          
             if (iAttr == Undefined)
             {
                ASSERT(FALSE);           
                AfxThrowDBException(0,&database, hstmt);
             }

             int j = 0; for (j = 0; j < aAttr.GetSize(); j++)
             {
                if (aAttr.GetAt(j)->GetAttrId() == LOWORD(iAttr))
                {
                   pAttr = aAttr.GetAt(j);
                }
             }
             ASSERT(pAttr != NULL);
             if (pAttr == NULL) AfxThrowDBException(0,&database, hstmt);

             WORD wFlag = HIWORD(iAttr);

             switch (pAttr->GetDataType())
             {
             case  BDNUMBER :                    
                   nRet = SQLBindCol(hstmt, iCol, SQL_C_CHAR, m_aColumn[i].m_sText, BD_SHORTSTR, &dwLength); 
                   break;
             case BDTEXT :        			
			       nRet = SQLBindCol(hstmt, iCol, SQL_C_CHAR, pAttr->GetString(), BD_SHORTSTR, &dwLength);                    
                   break;       

			// For long text, use SQLGetData to retrieve values

             case BDLONGTEXT :
                   nRet = SQLBindCol(hstmt, iCol, SQL_C_CHAR, NULL, 0, &dwLength);                    				  
				   break;
             case BDCOORD :             
                   if (wFlag == X_COORD)
                   {
                      nRet = SQLBindCol(hstmt, iCol, SQL_C_DOUBLE, &pAttr->GetCoord()->x, sizeof(double), &dwLength);
                   } else if (wFlag == Y_COORD)
                   {
                      nRet = SQLBindCol(hstmt, iCol, SQL_C_DOUBLE, &pAttr->GetCoord()->y, sizeof(double), &dwLength); 
                   } else if (wFlag == LAT_COORD)
                   {
                      nRet = SQLBindCol(hstmt, iCol, SQL_C_CHAR, m_aColumn[i].m_sText, BD_SHORTSTR, &dwLength);
                   } else if (wFlag == LON_COORD)
                   {
                      nRet = SQLBindCol(hstmt, iCol, SQL_C_CHAR, m_aColumn[i].m_sText, BD_SHORTSTR, &dwLength);
                   } else if (wFlag == IMP_COORD)
                   {
                      nRet = SQLBindCol(hstmt, iCol, SQL_C_CHAR, m_aColumn[i].m_sText, BD_SHORTSTR, &dwLength);
                   }

                   break;
             case BDLINK:                
                nRet = SQLBindCol(hstmt, iCol, SQL_C_CHAR, m_aColumn[i].m_sText, BD_SHORTSTR, &dwLength);
                   break;

             case BDBOOLEAN:
                nRet = SQLBindCol(hstmt, iCol, SQL_C_CHAR, m_aColumn[i].m_sText, BD_SHORTSTR, &dwLength);                
                break;             
             };             
          };
          if (!m_aColumn[i].m_bShapeFile) iCol++;
		 }
    } CATCH (CDBException, pEx)
    {
       bOK = FALSE;
    } END_CATCH

    // Set progress bar for shapefiles
    
      BDProgressRange(0,  lRecords);

	  // Retrieve the data and save it to the database

	   int nRet;
       while (!bCancel && bOK && SQL_SUCCEEDED(nRet = SQLFetch(hstmt)))
       {
           importDB.m_nFetched++;

		  // For longtext, retrieve data now
          
			 RetrieveLongText(hstmt, aAttr);			  
		  
          try
          {          

          // Determine feature

          long lFeature = 0;
          if (sID[0] == '\0')
          {
             lFeature = DetermineFeature(ftype.m_lId, sFeature, sParentFeature, 
                                           sGrandParentFeature, sGreatGrandParentFeature);
          } 
          else
          {
             lFeature = DetermineFeature(ftype.m_lId, sID, m_aColumn);
          }
          if (lFeature == -1) bOK = FALSE;
                    
          // Convert the date to a long

          if (sDate[0] != '\0') 
          {             
             if (!date.StringAsDate(sDate))
             {
                lFeature = 0;
                fprintf(importDB.m_pLogFile, "%i: " + BDString(IDS_INVALIDDATE)+ ": %s\r\n", m_nRecord, sDate);                     
             }
          } 
          // If no date supplied then request a date from the user
          else
          {
             CDlgImportDate dlg;
             if (!dateDefault.IsValid())
             {
                if (!m_bAutoCreate)
                {
                   if (dlg.DoModal() == IDOK)
                   {
                      dateDefault = dlg.GetDate();
                   } else
                   {
                      bOK = FALSE;
                   }                
                } else
                {
                   dateDefault.AsSystemDate();
                }
             }   
             date = dateDefault;
          }
          if (bOK && lFeature != 0)
          {
             aAttr.m_lDate = date.GetDateLong();
          };

          // For links, determine their ids

          if (!DetermineLink(importDB, aAttr))
          {
             bOK = FALSE;             
          }

          // For booleans, determine value

          if (!DetermineBool(importDB, aAttr))
          {
             lFeature = 0;
          }

          // Determine value of numbers

          if (!DetermineNumber(importDB, aAttr))
          {
             lFeature = 0;
          }

          // For latitude/lontitude determine value

          if (!DetermineLatLon(importDB, aAttr))
          {
             lFeature = 0;
          }

          // Update the feature attributes as these are used by DetermineShapefile

          aAttr.m_lFeature = lFeature;
          aAttr.m_lFType = ftype.m_lId;
          aAttr.m_lOrder = importDB.m_nImported;

          // For shapefiles, determine value

          int nFlag = BDADD;
          
          nShape = DetermineShapefile(aAttr, &maplayer);

          if (nShape == SHPPolygon)
          {
             // Store features at the end when the polylines have been
             // merged

             lFeature = 0;                     
          }
		    else if (nShape == -1)
          {
			   bOK = FALSE;
          }

          // Save values to database
                    
             if (lFeature != 0 && bOK)
             {                
                if (BDAttribute(BDHandle(), &aAttr, nFlag))
                {
                   importDB.m_nImported++;                                      
                };                
             };
             BDEnd(BDHandle());          
          } 
          catch (CDBException* pEx)
          {             
            // If duplicate value then create a new feature          
            try
            {
            // If unable to write to database then assume this is because of duplicate
            // attributes.  Create a new feature and write the data for this

               long lParent = 0;
               long lFeature = CreateFeature(ftype.m_lId, sFeature, sParentFeature, sGrandParentFeature, sGreatGrandParentFeature, lParent);
               if (lFeature)
               {
                  aAttr.m_lFeature = lFeature;

            
                  if (BDAttribute(BDHandle(), &aAttr, BDADD))
                  {
                     importDB.m_nImported++;                                      
                  };                                                         
               } 
          
            }
            catch (CDBException* pEx)
            {
               BDGetError(BDHandle(), sError);
               fprintf(importDB.m_pLogFile, "%i: %s\r\n", m_nRecord,(LPCSTR)sError);                           
               pEx->Delete();
            }          

            pEx->Delete();
          } 
          catch (CException* pEx)
          {
             char szError[128];
             pEx->GetErrorMessage(szError, sizeof(szError));
             fprintf(importDB.m_pLogFile, "%i: %s\r\n", m_nRecord, szError);                                 
             pEx->Delete();

             // Prevent from committing changes, 
             importDB.m_nImported = 0; 

             bOK = FALSE;
          }
          
          // Update progress

          if (nShape != SHPPolygon)
          {
             sProgress.Format(BDString(IDS_IMPORTED) + ": %i "+ BDString(IDS_ERRORS) +": %i",importDB.m_nImported, importDB.m_nFetched-importDB.m_nImported);
             BDProgressText(sProgress);
          } else
          {
             sProgress.Format(BDString(IDS_READING) + " %i",m_nRecord);
             BDProgressText(sProgress);             
          }
                   
          BDProgressPos(m_nRecord);
          if (importDB.m_pDlgProgress->IsCancel()) bCancel = TRUE;

          m_nRecord++;
	

          // Clean up ready for next string

          int i = 0; for (i = 0; i < m_aColumn.GetSize(); i++)
          {                           
             m_aColumn[i].m_sText[0] = '\0';
          }                 
          for (i = 0; i < aAttr.GetSize(); i++)
          {
             CAttribute* pAttr = aAttr.GetAt(i);
             if (pAttr->GetDataType() == BDTEXT) pAttr->SetString("");             
			    if (pAttr->GetDataType() == BDLONGTEXT) pAttr->SetLongText("");
          }

          // Reset all names otherwise if reach bottom of excel file still appears
          // to be (repeating) data

          sFeature[0] = '\0';
          sParentFeature[0] = '\0';
          sGrandParentFeature[0] = '\0';
          sGreatGrandParentFeature[0] = '\0';
       }   

   // Release statement on end
      
       SQLFreeStmt(hstmt,SQL_DROP);         
   }      

    // Once finished update the records with the shapefile data

   if (bOK && !bCancel)
   {
      bOK = WriteShapefile(importDB);      
   };
        
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// For BDLONGTEXT attributes, retrieves subsequent data
//

void CImportTable::RetrieveLongText(HSTMT hstmt, CAttrArray& aAttr)
{
   char sBuffer[BD_SHORTSTR];
   int iCol = 1;
   long nLength;
   int nRet;

   // Find any columns containing longtext attributes

   int i = 0; for (i = 0; i < m_aColumn.GetSize(); i++)
   {
      if (m_aColumn[i].m_bAttr)
      {
         CAttribute* pAttr = NULL;
         int iAttr = GetAttr(aAttr, m_aColumn[i].m_sNameAttr);         

         if (iAttr != Undefined)
         {
            int j = 0; for (j = 0; j < aAttr.GetSize(); j++)
            {
               if (aAttr.GetAt(j)->GetAttrId() == LOWORD(iAttr))
               {
                  pAttr = aAttr.GetAt(j);
               }
            }
		 }

		 // Retrieve data

		 if (pAttr->GetDataType() == BDLONGTEXT)
		 {			 
			 pAttr->SetLongText("");			 

			 do 
			 {
			    nRet = SQLGetData(hstmt, iCol, SQL_C_CHAR, sBuffer, BD_SHORTSTR, &nLength);

				// Append the retrieve values to the string

             if (SQL_SUCCEEDED(nRet) && nLength > 0)
             {
				    pAttr->SetLongText(*pAttr->GetLongText() + CString(sBuffer, nLength));
             };
             // Loop whilst data truncated
			 } while (nLength > 0 && nRet == SQL_SUCCESS_WITH_INFO);
		 }
	  };

	  if (!m_aColumn[i].m_bShapeFile) iCol++;
   }; 
}


///////////////////////////////////////////////////////////////////////////////

void CImportDB::CompactDatabase()
{
	// Determine the file name of the database

   CString sVersion;
   BDGetDriver(BDHandle(), sVersion);   
   if (sVersion == "odbcjt32.dll")
   {

      AfxGetApp()->BeginWaitCursor();

      HKEY hKey;
      UCHAR sRegKey[_MAX_PATH];
      CString sDBPath;
      char szDBPath[_MAX_PATH];
      long lFileLength = 0;
      long lFileLengthNew = 0;

      if (RegOpenKey(HKEY_CURRENT_USER, "SOFTWARE\\ODBC\\ODBC.INI\\" + CString(BDGetDataSource()), &hKey) == ERROR_SUCCESS)
      {
         DWORD dw = sizeof(sRegKey);           
	     DWORD dwDataType;

         if (RegQueryValueEx(hKey, "DBQ", NULL, &dwDataType, sRegKey, &dw) == ERROR_SUCCESS)
         {
            sDBPath = sRegKey;         
         }
      }   
      RegCloseKey(hKey);

      // Disconnect 

	   BDDisconnect(BDHandle());

       CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	   pWnd->SetStatusText("Compacting database...");

      GetShortPathName(sDBPath, szDBPath, sizeof(szDBPath));   

	   SQLConfigDataSource(NULL,ODBC_ADD_DSN, BDString(IDS_ACCESSDRIVER),
			     "COMPACT_DB=" + CString(szDBPath) + " " + CString(szDBPath) + " General\0"
			     "MaxScanRows=20\0\0");    		   

	   BDConnect(BDGetDataSource(), &BDHandleRef());    				 

	   AfxGetApp()->EndWaitCursor();
   };	      
}


///////////////////////////////////////////////////////////////////////////////

BOOL CImportTable::DetermineLink(CImportDB& importDB, CAttrArray &aAttr)
{
   int i = 0; for (i = 0; i < m_aColumn.GetSize(); i++)
   {
      // Determine attribute for each column

      if (m_aColumn[i].m_bAttr)
      {
         CAttribute* pAttr = NULL;
         int iAttr = GetAttr(aAttr, m_aColumn[i].m_sNameAttr);

         if (iAttr != Undefined)
         {
            int j = 0; for (j = 0; j < aAttr.GetSize(); j++)
            {
               if (aAttr.GetAt(j)->GetAttrId() == LOWORD(iAttr))
               {
                  pAttr = aAttr.GetAt(j);
               }
            }

            // For each link, search for the feature

            if (pAttr->GetDataType() == BDLINK)
            {				    
                // Set link to undefined

                pAttr->SetLink(0);

                if (m_aColumn[i].m_sText[0] != '\0') 
                {
                   long lAttr = DetermineFeature(pAttr->GetFTypeLink(), m_aColumn[i].m_sText, "", "", "");

                   // Reset link value so not re-used if next line in file is blank                 

                   if (lAttr == -1) return FALSE;
                   pAttr->SetLink(lAttr);
                }
            }     
         };
      };
   };

   // For any link attributes not being imported then set their value to
   // undefined

   for (i = 0; i < aAttr.GetSize(); i++)
   {
      CAttribute* pAttr = aAttr.GetAt(i);
      if (pAttr->GetDataType() == BDLINK &&
          *pAttr->GetLink() == 0)
      {
         long lAttr = DetermineFeature(pAttr->GetFTypeLink(), BDString(IDS_UNDEFINED), "", "", "");
         if (lAttr == -1) return FALSE;
         pAttr->SetLink(lAttr);                               
      }
   }

   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CImportTable::DetermineBool(CImportDB& importDB, CAttrArray &aAttr)
{
   int i = 0; for (i = 0; i < m_aColumn.GetSize(); i++)
   {
      // Determine attribute for each column

      if (m_aColumn[i].m_bAttr)
      {
         CAttribute* pAttr = NULL;
         int iAttr = GetAttr(aAttr, m_aColumn[i].m_sNameAttr);
         ASSERT(iAttr != Undefined);
         if (iAttr == Undefined) return FALSE;

         int j = 0; for (j = 0; j < aAttr.GetSize(); j++)
         {
            if (aAttr.GetAt(j)->GetAttrId() == LOWORD(iAttr))
            {
               pAttr = aAttr.GetAt(j);
            }
         }

         // For each link, search for the feature

         if (pAttr->GetDataType() == BDBOOLEAN)
         {             
             if (stricmp(m_aColumn[i].m_sText,BDString(IDS_YES)) == 0 ||
				 stricmp(m_aColumn[i].m_sText,BDString(IDS_Y)) == 0 ||
                 stricmp(m_aColumn[i].m_sText,BDString(IDS_TRUE)) == 0 ||
                 stricmp(m_aColumn[i].m_sText, "1") == 0)
             {
                pAttr->SetBoolean(TRUE);
             }
             else if (stricmp(m_aColumn[i].m_sText,BDString(IDS_NO)) == 0 ||
				 stricmp(m_aColumn[i].m_sText,BDString(IDS_N)) == 0 ||
                 stricmp(m_aColumn[i].m_sText,BDString(IDS_FALSE)) == 0 || 
                 stricmp(m_aColumn[i].m_sText, "0") == 0)
             {
                pAttr->SetBoolean(FALSE);
             }
			 else if (m_aColumn[i].m_sText[0] == '\0')
			 {
				 pAttr->SetBoolean(AFX_RFX_BOOL_PSEUDO_NULL);
			 }
             else
             {
                 fprintf(importDB.m_pLogFile, "%i: " + BDString(IDS_INVALIDVALUE) + ": %s (%s)\r\n", 
                         m_nRecord, m_aColumn[i].m_sNameAttr, m_aColumn[i].m_sText);                  
                 pAttr->SetBoolean(FALSE);             
             }                                            
         }     
      };
   };
   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CImportTable::DetermineNumber(CImportDB& importDB, CAttrArray& aAttr)
{
   int i = 0; for (i = 0; i < m_aColumn.GetSize(); i++)
   {
      // Determine attribute for each column

      if (m_aColumn[i].m_bAttr)
      {
         CAttribute* pAttr = NULL;
         int iAttr = GetAttr(aAttr, m_aColumn[i].m_sNameAttr);
         ASSERT(iAttr != Undefined);         
         if (iAttr == Undefined) return FALSE;

         int j = 0; for (j = 0; j < aAttr.GetSize(); j++)
         {
            if (aAttr.GetAt(j)->GetAttrId() == LOWORD(iAttr))
            {
               pAttr = aAttr.GetAt(j);
            }
         }

         // For each link, search for the feature

         if (pAttr->GetDataType() == BDNUMBER)
         {             
             if (!pAttr->AsAttr(m_aColumn[i].m_sText))
             {
                fprintf(importDB.m_pLogFile, "%i: " + BDString(IDS_INVALIDVALUE) + ": %s (%s)\r\n", 
                         m_nRecord, m_aColumn[i].m_sNameAttr, m_aColumn[i].m_sText);                       
                pAttr->SetDouble(AFX_RFX_DOUBLE_PSEUDO_NULL);
             }                                                      
         }     
      };
   };
   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CImportTable::DetermineLatLon(CImportDB& importDB, CAttrArray& aAttr)
{
   double dLat, dLon;
   double *pX = NULL, *pY = NULL;

   int i = 0; for (i = 0; i < m_aColumn.GetSize(); i++)
   {
      // Determine attribute for each column

      if (m_aColumn[i].m_bAttr)
      {
         CAttribute* pAttr = NULL;
         int iAttr = GetAttr(aAttr, m_aColumn[i].m_sNameAttr);
         ASSERT(iAttr != Undefined);
         if (iAttr == Undefined) return FALSE;

         int j = 0; for (j = 0; j < aAttr.GetSize(); j++)
         {
            if (aAttr.GetAt(j)->GetAttrId() == LOWORD(iAttr))
            {
               pAttr = aAttr.GetAt(j);
            }
         }

         // For each link, search for the feature

         WORD wFlag = HIWORD(iAttr);

         if (pAttr->GetDataType() == BDCOORD)            
         {  
            // Latitude

            if (wFlag == LAT_COORD)
            {  
               pX = &pAttr->GetCoord()->x;
               if (!BDProjection()->StringAsLatLong(m_aColumn[i].m_sText, &dLat, CProjection::latitude | CProjection::import))              
               {
                  fprintf(importDB.m_pLogFile, "%i: Invalid value for %s\r\n", 
                         m_nRecord, m_aColumn[i].m_sNameAttr);                                         
                  return FALSE;
               }
               
            }
            // Longitude

            else if (wFlag == LON_COORD)
            {
               pY = &pAttr->GetCoord()->y;
               if (!BDProjection()->StringAsLatLong(m_aColumn[i].m_sText, &dLon, CProjection::longitude | CProjection::import))               
               {
                  fprintf(importDB.m_pLogFile, "%i: Invalid value for %s\r\n", 
                         m_nRecord, m_aColumn[i].m_sNameAttr);                       
                  
                  return FALSE;
               }                         
            }            
            else if (wFlag == IMP_COORD)
            {
               // Allow empty coordinate pairs

               if (CString(m_aColumn[i].m_sText).IsEmpty())
               {                  
                  pAttr->GetCoord()->SetNull();
               }   
               else if (!BDProjection()->StringAsCoord(m_aColumn[i].m_sText, pAttr->GetCoord()) && 
                   !m_aColumn[i].m_bShapeFile)
               {
                  fprintf(importDB.m_pLogFile, "%i: Invalid value for %s\r\n", 
                          m_nRecord, m_aColumn[i].m_sNameAttr);
               };
            }
         }     
      };
   };

   // Assumes there is only one lat/long per column

   if (pX != NULL && pY != NULL)
   {
      BDProjection()->LatLonToTransMercator(dLat,dLon,pX, pY);         
   };

   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CImportTable::LoadShapefile(CImportDB &importDB, CMapLayer& maplayer)
{
   BOOL bOK = TRUE;

   int i = 0; for (i = 0; i < m_aColumn.GetSize(); i++)
   {
      if ((m_aColumn[i].m_sNameImport == SHAPEFILE_POINTS || 
           m_aColumn[i].m_sNameImport == SHAPEFILE_POLYLINES) && 
           m_aColumn[i].m_sNameAttr != IMPORT_SKIP)
      {          
          importDB.m_pDlgProgress->SetText(BDString(IDS_IMPORTSHAPEFILE) + "...");

          CShapeFile shapefile;

          if (!shapefile.ImportShapeFile(&maplayer, m_sShapeFile))
          {
             fprintf(importDB.m_pLogFile, BDString(IDS_ERRORSHAPEFILE) + ": %s\r\n", m_sShapeFile);                     
             bOK = FALSE;
          } 

          // Ask if shapefile is protected
 
          if (bOK && m_aColumn[i].m_sNameImport == SHAPEFILE_POLYLINES && !m_bAutoCreate)
          {
             CDlgProtect dlg;
             if (dlg.DoModal() == IDOK && dlg.IsProtected())
             {
                int i = 0; for (i = 0; i < maplayer.GetSize(); i++)
                {
                   CLongLines* pLongLines = (CLongLines*)maplayer.GetAt(i)->GetMapObject();
                   pLongLines->SetProtected(dlg.GetMessage());
                }
             } 
          }
      }
   }
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Update the map lines at the end as otherwise Access gets overwhelmed!
//

int CImportTable::DetermineShapefile(CAttrArray& aAttr, CMapLayer* pMapLayer)
{
   int nRet = 0;
   CCoord coordNull;
   coordNull.SetNull();

   // Search for the column containing the shapefile

   int i = 0; for (i = 0; i < m_aColumn.GetSize(); i++)
   {
     // Determine the attribute corresponding to the column

      if (m_aColumn[i].m_bShapeFile && m_aColumn[i].m_bAttr)
      {
        // Too many records in dbf file

         if (m_nRecord > pMapLayer->GetSize()) return -1;   

         CAttribute* pAttr = NULL;
         int iAttr = GetAttr(aAttr, m_aColumn[i].m_sNameAttr);
         ASSERT(iAttr != Undefined);
         if (iAttr == Undefined) return FALSE;

         int j = 0; for (j = 0; j < aAttr.GetSize(); j++)
         {
            if (aAttr.GetAt(j)->GetAttrId() == LOWORD(iAttr))
            {
               pAttr = aAttr.GetAt(j);
            }
         }

         // Copy the data

         if (pAttr != NULL)
         {
             if (m_aColumn[i].m_sNameImport == SHAPEFILE_POINTS)
             {                
		          CMapLayerObj* pMapLayerObj = pMapLayer->GetAt(m_nRecord-1);
				    CCoord* pCoord = (CCoord*)pMapLayerObj->GetMapObject();
                pAttr->SetCoord(*pCoord);          

                nRet = SHPPoint;
             }
             else if (m_aColumn[i].m_sNameImport == SHAPEFILE_POLYLINES)
             {                 
                nRet = SHPPolygon;

                CLongLines* pMapLines = (CLongLines*)pMapLayer->GetAt(m_nRecord-1)->GetMapObject();
                MergeShapefile(aAttr, pMapLines);

                m_lAttrIdShapefile = pAttr->GetAttrId();

                // Pointer transfered so set to null
                
                pMapLayer->GetAt(m_nRecord-1)->SetMapObject(NULL);

             }
             
         }   
      }
   }

   return nRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// As Access seems to run out of memory and disk space when continually updating
// longbinary objects, maplines will be stored in memory and written to database
// at end.
//

/*
int CImportTable::MergeShapefile(CAttrArray& aAttr, CLongLines* pMapLines)
{
   CAttribute* pAttrE = NULL;
   int nRet = Add;
   int iCompare = 1;
   int i = -1;
   BOOL bFound = FALSE;

   // Determine if the attributes are already stored
   // Optimisation, create an ordered list

   int iLow = 0;
   int iHigh = m_aAttrShapefile.GetSize()-1;   
   while (iLow <= iHigh)
   {
      int i = (iLow+iHigh)/2;
      CAttrArray& aAttrE = m_aAttrShapefile.GetAt(i).m_aAttr;
      int iCompare = 1;
      int j = 0; for (j = 0; j < aAttrE.GetSize() && iCompare == 0; j++)
      {
          if (aAttrE[j]->IsPrimaryKey()) 
          {
               iCompare = CQueryResult::Compare(aAttrE[j], aAttr[j]);               
          };               
      }
      if (iCompare == 0)
      {
         bFound = TRUE;
         break;
      } else if (iCompare > 0)
      {
         iLow = i+1;           
      } else
      {
         iHigh = i-1;
      }
   }   

   // If not found then add to the list

   if (!bFound)
   {
      // If greater than value then insert after it

      if (iCompare > 0) i++;

      CPolylineAttr polylineattr;
      polylineattr.m_aAttr = aAttr;
      m_aAttrShapefile.InsertAt(i,polylineattr);      
   } else
   {
      nRet = Update;
   }

   // Retrieve the existing stored maplines

   CPolylineAttr& aAttrE = m_aAttrShapefile[i];
   
   CMapLayerObj* pMapLayerObj = new CMapLayerObj;
	pMapLayerObj->SetMapObject(pMapLines);                        
   pMapLayerObj->SetDataType(BDMAPLINES);
   aAttrE.m_maplayer.Add(pMapLayerObj);

    // Tidy up

    return nRet;
}
*/

int CImportTable::MergeShapefile(CAttrArray& aAttr, CLongLines* pMapLines)
{
   CAttribute* pAttrE = NULL;
   int nRet = Add;

   // Determine if the attributes are already stored
   // TODO optimisation, create an ordered list

   BOOL bFound = FALSE;
   int i = 0; for (i = 0; i < m_aAttrShapefile.GetSize(); i++)
   {
      CAttrArray& aAttrE = m_aAttrShapefile.GetAt(i).m_aAttr;

      if (aAttrE.m_lFeature == aAttr.m_lFeature && 
          aAttrE.m_lDate == aAttr.m_lDate)
      {
         // Check primary key

         int j = 0; for (j = 0; j < aAttrE.GetSize(); j++)
         {
            if (aAttrE[j]->IsPrimaryKey() && 
                aAttrE[j] != aAttr[j]) break;                
         }
         if (j == aAttrE.GetSize())
         {
            bFound = TRUE;
            break;
         }
      }
   }

   // If not found then add to the list

   if (!bFound)
   {
      CPolylineAttr polylineattr;
      polylineattr.m_aAttr = aAttr;
      m_aAttrShapefile.Add(polylineattr);      
   } else
   {
      nRet = Update;
   }

   // Retrieve the existing stored maplines

   CPolylineAttr& aAttrE = m_aAttrShapefile[i];
   
   CMapLayerObj* pMapLayerObj = new CMapLayerObj;
	pMapLayerObj->SetMapObject(pMapLines);                        
   pMapLayerObj->SetDataType(BDMAPLINES);
   aAttrE.m_maplayer.Add(pMapLayerObj);

    // Tidy up

    return nRet;
}


///////////////////////////////////////////////////////////////////////////////
//
// Write the buffered shapefile and attributes back to the database
//

BOOL CImportTable::WriteShapefile(CImportDB& importDB)
{
    int nImported = 0;
    BOOL bOK = TRUE;
    CString sProgress;

    BDProgressRange(0, m_aAttrShapefile.GetSize());

    int i = 0; for (i = 0; i < m_aAttrShapefile.GetSize() && bOK; i++)
    {                  
       // Progress indicator
       sProgress.Format(BDString(IDS_WRITING) + " %i",i+1);
       BDProgressText(sProgress);
       BDProgressPos(i);

       CMapLayer& maplayer = m_aAttrShapefile[i].m_maplayer;          

       CLongLines maplines;

       // Determine total size of maplines

       int nSize = 0;
       int j = 0; for (j = 0; j < maplayer.GetSize(); j++)
       {
          ASSERT(maplayer.GetAt(j)->GetDataType() == BDMAPLINES);
          CLongLines* pMapLines = (CLongLines*)maplayer.GetAt(j)->GetMapObject();
          nSize += pMapLines->GetSize();
       }
       maplines.SetSize(nSize);

       // Copy the data

       nSize = 0;
       for (j = 0; j < maplayer.GetSize(); j++)
       {                        
          CLongLines* pMapLines = (CLongLines*)maplayer.GetAt(j)->GetMapObject();

          // Transfer protection status
          maplines.SetProtected(pMapLines->GetProtected());

          for (int k = 0; k < pMapLines->GetSize(); k++)
          {
             maplines.SetAt(k+nSize, pMapLines->GetAt(k));
          }             
          nSize += pMapLines->GetSize();
       }

       // Determine the attribute to store the maplines

       CAttrArray& aAttr = m_aAttrShapefile[i].m_aAttr;          
       CAttribute* pAttr = NULL;

       for (j = 0; j < aAttr.GetSize(); j++)
       {
          if (aAttr[j]->GetAttrId() == m_lAttrIdShapefile)
          {
             pAttr = aAttr.GetAt(j);
             break;
          };
       }
       
       // Convert to long binary

      // TODO write extent to CLongLines object

       CLongBinary longbinary;       
       maplines.GetLongBinary(longbinary);
       pAttr->SetLongBinary(&longbinary);
       longbinary.m_hData = NULL; // prevents deletion of memory                                     

       try
       {       
          if (BDAttribute(BDHandle(), &aAttr, BDADD))
          {
             importDB.m_nImported++;
          }

       }
       catch (CDBException* pEx)
       { 
          pEx->Delete();
          CString sError;
          BDGetError(BDHandle(), sError);
          fprintf(importDB.m_pLogFile, "%i: %s\r\n", i+1,(LPCSTR)sError);                           
       }    
       catch (CException* pEx)
       {          
          char szError[128];
          
          pEx->GetErrorMessage(szError, sizeof(szError));          
          fprintf(importDB.m_pLogFile, "%i: %s\r\n", i+1,(LPCSTR)szError);                           
          
          // Prevent from committing changes, 
          importDB.m_nImported = 0; 

          bOK = FALSE;          
          pEx->Delete();          
       }

       // Must call BDEnd as using a different address for aAttr
         
       BDEnd(BDHandle());

       // Release memory as one goes along

       maplayer.RemoveAll();
       aAttr.RemoveAllX();
    }

    return bOK;
}

///////////////////////////////////////////////////////////////////////////////

void CImportDB::Write(LPCSTR sFileName)
{
   FILE* pFile = fopen(sFileName, "wb");
   if (pFile != NULL)
   {
      int i = 0; for (i = 0; i < GetSize(); i++)
      {
         GetAt(i).Write(pFile);         
      }
      fprintf(pFile,"end\n");
      fclose(pFile);
   }
}

///////////////////////////////////////////////////////////////////////////////

void CImportTable::Write(FILE* pFile)
{  
   fprintf(pFile, "table=%s,%s,%i,%s,%s,%s,%s\nshapefile=%s\n",
          (LPCSTR)m_sTableImport,m_sFType, m_bImport,m_sFeature,m_sParentFeature, 
          m_sGrandParentFeature, m_sGreatGrandParentFeature,m_sShapeFile);

   int i = 0; for (i = 0; i < m_aColumn.GetSize(); i++)
   {
      m_aColumn[i].Write(pFile);      
   }   
   fprintf(pFile,"end\n");
}

///////////////////////////////////////////////////////////////////////////////

void CImportColumn::Write(FILE* pFile)
{   
   fprintf(pFile,"column=\"%s\",\"%s\",%i\n", (LPCSTR)m_sNameImport, (LPCSTR)m_sNameAttr, 
           m_bShapeFile);      
}

///////////////////////////////////////////////////////////////////////////////

BOOL CImportDB::Read(LPCSTR sFileName)
{
   int nRet = 0;

   RemoveAll();

   FILE* pFile = fopen(sFileName, "r");
   if (pFile != NULL)
   {
      CImportTable table;

      while ((nRet = table.Read(pFile)) == TRUE)
      {
         Add(table);
      }
      
      fclose(pFile);
   }

   return nRet != -1;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CImportTable::Read(FILE* pFile)
{
   int nRet = 0;

   *this = CImportTable(); // Reset

   CImportColumn column;   

   CString s = BDNextItem(pFile);

   if (s == "table")
   {     
      m_sTableImport = BDNextItem(pFile);
      m_sFType = BDNextItem(pFile);      
      s = BDNextItem(pFile);
      sscanf(s,"%d", &m_bImport);      
      m_sFeature = BDNextItem(pFile);
      m_sParentFeature = BDNextItem(pFile);
      m_sGrandParentFeature = BDNextItem(pFile);
	  m_sGreatGrandParentFeature = BDNextItem(pFile);
      m_sShapeFile = BDNextStr(pFile);

      while ((nRet = column.Read(pFile)) == TRUE)
      {
         m_aColumn.Add(column);
      }

      if (nRet == -1) return -1;      
   } else
   {      
      if (s != "end") return -1;
      return FALSE;
   }

   return TRUE;
};

///////////////////////////////////////////////////////////////////////////////

BOOL CImportColumn::Read(FILE* pFile)
{
   *this = CImportColumn(); // Reset   

   CString s = BDNextItem(pFile);

   if (s == "column")
   {           
      m_sNameImport = BDNextItem(pFile);
      m_sNameAttr = BDNextItem(pFile);        
      m_bShapeFile = BDNextInt(pFile,FALSE);
   } else
   {
      if (s != "end") return -1;
      return FALSE;
   }
   return TRUE;
}


///////////////////////////////////////////////////////////////////////////////

int CImportDB::Find(LPCSTR sTable)
{
   int j = 0; for (j = 0; j < GetSize(); j++)
   {
      if (GetAt(j).m_sTableImport == sTable) break;                    
   }

   // If not found then add

   if (j == GetSize()) return -1;
   else return j;

}

///////////////////////////////////////////////////////////////////////////////
//
// Searches through the attributes for one with the given name.  Returns the
// index or -1 on error
//

int CImportTable::GetAttr(CAttrArray& aAttr, CString sAttr)
{
   WORD wFlag = 0;

   // Test for x,y coordinate suffix

   if (sAttr.Right(strlen(X_COORDS)) == X_COORDS) 
   {
      sAttr = sAttr.Left(sAttr.GetLength()-strlen(X_COORDS));
      wFlag = X_COORD;
   };
   if (sAttr.Right(strlen(Y_COORDS)) == Y_COORDS)
   {      
      sAttr = sAttr.Left(sAttr.GetLength()-strlen(Y_COORDS));
      wFlag = Y_COORD;
   }
   if (sAttr.Right(strlen(LAT_COORDS)) == LAT_COORDS) 
   {
      sAttr = sAttr.Left(sAttr.GetLength()-strlen(LAT_COORDS));
      wFlag = LAT_COORD;
   };
   if (sAttr.Right(strlen(LON_COORDS)) == LON_COORDS)
   {
      sAttr = sAttr.Left(sAttr.GetLength()-strlen(LON_COORDS));
      wFlag = LON_COORD;
   }
   if (sAttr.Right(strlen(IMP_COORDS)) == IMP_COORDS)
   {
      sAttr = sAttr.Left(sAttr.GetLength()-strlen(IMP_COORDS));  
      wFlag = IMP_COORD;
   }   
   if (sAttr.Right(strlen(IMP_MAPLINES)) == IMP_MAPLINES)
   {
      sAttr = sAttr.Left(sAttr.GetLength() - strlen(IMP_MAPLINES));
      wFlag = IMP_MAPLINE;
   }   

   // Return matching attribute

   int i = 0; for (i = 0; i < aAttr.GetSize(); i++)
   {
      CAttribute* pAttr = aAttr.GetAt(i);
      if (pAttr->GetDesc() == sAttr) return MAKELONG(pAttr->GetAttrId(), wFlag);
   }
   return Undefined;
}

///////////////////////////////////////////////////////////////////////////////
//
// Returns the id of a feature, if it exists and is not an empty string.  If
// not the user is prompted with a dialog
//

long CImportTable::DetermineFeature(long lFType, CString sFeature, LPCSTR sFeatureParent, 
                                    LPCSTR sFeatureGrandParent, LPCSTR sFeatureGreatGrandParent, BOOL bParent)
{
   CFeature feature;   
   CFeatureType ftype;
   CArray <CFeature,CFeature> aFeatures;   
   feature.m_sName = sFeature;
   long lFeature = 0;
   long lParent = 0;
   BOOL bFound = FALSE;
   int nRet = 0;

   // Strip leading and trailing spaces

   sFeature.TrimLeft();
   sFeature.TrimRight();

   // Determine parent feature
  
   if (sFeatureParent[0] != '\0')
   {
	   long lPFType = GetFTypeParentI(lFType);
	   if (lPFType != 0)
	   {
		  lParent = DetermineFeature(lPFType, sFeatureParent, sFeatureGrandParent, sFeatureGreatGrandParent, "", TRUE);
	   }

      // Cancel
      if (lParent == -1) return -1;
   }

   if (sFeature.IsEmpty()) sFeature = BDString(IDS_UNDEFINED);

   // If one to one then determine the inherited feature type

   lFType = GetFTypeI(lFType);
   
   // Selected feature for all

   int i = 0; for (i = 0; i < m_aImportFeature.GetSize(); i++)
   {
      if (m_aImportFeature[i].m_lFType == lFType && 
          m_aImportFeature[i].m_sFeature == sFeature+sFeatureParent+sFeatureGrandParent+sFeatureGreatGrandParent)
      {
         return m_aImportFeature[i].m_lFeature;
      };
   }   

   // Search the database   
 
   if (!sFeature.IsEmpty())
   {           
      feature.m_lFeatureTypeId = lFType;
      feature.m_sName = sFeature;
      bFound = BDFeature(BDHandle(), &feature, BDSELECT3);
      while (bFound)
      {
         if (feature.m_lParentFeature == lParent || lParent == 0)
         {
            aFeatures.Add(feature);
         };
         bFound = BDGetNext(BDHandle());
      }
      BDEnd(BDHandle());
                     
      // If one unique feature then use it
      
      if (aFeatures.GetSize() == 1)
      {
         lFeature = aFeatures[0].m_lId;

         // Add for autoselection to speed up, especially for parent feature

         CImportFeature feature;
         feature.m_lFeature = lFeature;
         feature.m_lFType = lFType;
         feature.m_sFeature = sFeature + sFeatureParent + sFeatureGrandParent + sFeatureGreatGrandParent; 
         m_aImportFeature.Add(feature);         
      }

   };
   
   // If not found then display a dialog

   if (lFeature == 0)
   {    
      CDlgImportFeature dlg(lFType, "", sFeature, sFeatureParent, lParent); 

      int i = 0; for (i = 0; i < m_alCreateAll.GetSize(); i++)
      {
         if (m_alCreateAll[i] == lFType)
         {
            nRet = IDC_CREATEALL;
         }
      }

      // For auto import then create all

      if (m_bAutoCreate) nRet = IDC_CREATEALL;

      if (nRet != IDC_CREATEALL)
      {         
         // Sound alert incase import is occuring in the background      

         MessageBeep(MB_ICONQUESTION);

         nRet = dlg.DoModal();         
      }

      // If create all is selected then create automatically

      if (nRet == IDC_CREATEALL)
      {
         // Add new feature type

         int i = 0; for (i = 0; i < m_alCreateAll.GetSize(); i++)
         {
             if (m_alCreateAll[i] == lFType) break;
         }
         if (i == m_alCreateAll.GetSize()) m_alCreateAll.Add(lFType);         
         
         // Create feature

         if (!sFeature.IsEmpty())
         {
            lFeature = CreateFeature(lFType, sFeature, sFeatureParent, sFeatureGrandParent, sFeatureGreatGrandParent, lParent);
                              
            if (lFeature == 0)
            {
               // If create feature failed then display create dialog

               CDlgEditFeature dlg(lFType, sFeature, sFeatureParent);
               nRet = dlg.DoModal();

               if (nRet == IDOK)
               {      
                  lFeature = dlg.GetId();            
               };
            } else if (lFeature == -1)
            {
               return -1;
            }
         } else
         {  
            fprintf(m_pImportDB->m_pLogFile, "%i: No name supplied for feature, not imported\r\n", 
                    m_nRecord);                                 

         };
      }

      // For select then return feature

      else if (nRet == IDC_SELECT || nRet == IDC_SELECTALL)
      {
         lFeature = dlg.GetFeature();
      }

      // For select all, add to the list, per feature type

      if (nRet == IDC_SELECTALL)
      {         
         CImportFeature feature;
         feature.m_lFeature = lFeature;
         feature.m_lFType = lFType;
         feature.m_sFeature = sFeature + sFeatureParent; 
         m_aImportFeature.Add(feature);         
      }
      if (nRet == IDCANCEL)
      {
         return -1;
      }
   } else if (sFeature[0] == '\0')
   {
      fprintf(m_pImportDB->m_pLogFile, "%i: " + BDString(IDS_NONAME) + "\r\n", m_nRecord);                                 
   }

   return lFeature;
}

///////////////////////////////////////////////////////////////////////////////

long CImportTable::GetFTypeI(long lFType)
{
   CFeatureType ftype;

   int i = 0; for (i = 0; i < m_aImportFTypeI.GetSize(); i++)
   {
      if (m_aImportFTypeI[i].m_lFType == lFType) 
      {
         return m_aImportFTypeI[i].m_lFTypeI;
      };
   }
   if (BDFTypeI(BDHandle(), lFType, &ftype))
   {      
      CImportFTypeI importftype;
      importftype.m_lFType = lFType;
      importftype.m_lFTypeI = ftype.m_lId;
      m_aImportFTypeI.Add(importftype);
      return ftype.m_lId;
   }

   ASSERT(FALSE);
   return 0;   
}

///////////////////////////////////////////////////////////////////////////////

long CImportTable::GetFTypeParentI(long lFType)
{
   CFeatureType ftypeP;

   int i = 0; for (i = 0; i < m_aImportFTypeParentI.GetSize(); i++)
   {
      if (m_aImportFTypeParentI[i].m_lFType == lFType) 
      {
         return m_aImportFTypeParentI[i].m_lFTypeParentI;
      };
   }

   if (BDFTypeParentI(BDHandle(), lFType, &ftypeP))
   {      
      CImportFTypeParentI importftype;
      importftype.m_lFType = lFType;
      importftype.m_lFTypeParentI = ftypeP.m_lId;
      m_aImportFTypeParentI.Add(importftype);      

      return ftypeP.m_lId;
   }
   
   return 0;   
}

///////////////////////////////////////////////////////////////////////////////

long CImportTable::CreateFeature(long lFType, LPCSTR sFeature, LPCSTR sFeatureParent, 
								 LPCSTR sFeatureGrandParent, LPCSTR sFeatureGreatGrandParent, 
								 long lParent)
{
   CFeature feature;
   CFeatureType ftype;    
   BOOL bOK = TRUE;
   CString s;
   
   // Determine feature type

   ftype.m_lId = lFType;
   bOK = BDFeatureType(BDHandle(), &ftype, BDSELECT);
   BDEnd(BDHandle());

   // If no parent then ask for it

   if (lParent == 0)
   {
	   long lPFType = GetFTypeParentI(lFType);
	   if (lPFType != 0)
	   {
		  lParent = DetermineFeature(lPFType, sFeatureParent, sFeatureGrandParent, sFeatureGreatGrandParent, "", TRUE);
	   }
   }

   // Determine the next id

   if (bOK)
   {          
      BDNextId(BDHandle(), BDFEATURE, lFType, &feature.m_lId);
      
      feature.m_sName = sFeature;
      feature.m_lFeatureTypeId = lFType;

      // Set parent feature

      feature.m_lParentFeature = lParent;

      // Create feature 

      bOK = BDFeature(BDHandle(), &feature, BDADD);
	   BDEnd(BDHandle());
   };

   if (bOK)
   {
      return feature.m_lId;
   }
   return 0;
   
};

///////////////////////////////////////////////////////////////////////////////
//
// Determine the file being imported
//

CString CImportTable::GetShortFileTitle(CDatabase& database)
{
   CString sPath;
   CString sTable;
	char szDBase[_MAX_PATH];

   // Determine the directory

   int i = database.GetConnect().Find("DefaultDir=");      
   sPath = database.GetConnect().Mid(i+11);
   sPath = sPath.Left(sPath.Find(";"));

   // Create the full path

   if (sPath[sPath.GetLength()-1] != '\\') sPath += '\\';   
   sPath += m_sTableImport + ".dbf";

   // Convert to a short file name
    
   GetShortPathName(sPath, szDBase, sizeof(szDBase));

   // Retrieve the short file name

   sTable = szDBase;
   sTable = sTable.Mid(sTable.ReverseFind('\\')+1);
   sTable = sTable.Left(sTable.ReverseFind('.'));

   return sTable;

   return sPath;
}

///////////////////////////////////////////////////////////////////////////////

long CImportTable::GetCountRecords(HDBC hdbc, LPCSTR sTable)
{
   BOOL bOK = TRUE;
   long lRet = 0;   
   SDWORD cbCount;   
   RETCODE nRetCode;
      
   if (bOK)
   {              
      HSTMT hstmt;
      if (SQLAllocStmt(hdbc, &hstmt) == SQL_SUCCESS)
      {         
      
         CString sSQL = "select count (*) from [" + CString(sTable) + "]";
         if (::SQLExecDirect(hstmt, (UCHAR*)sSQL.GetBuffer(0), sSQL.GetLength()) != SQL_SUCCESS)
         { 
            bOK = FALSE;
         };
         
         if (bOK && SQLBindCol(hstmt, 1, SQL_C_LONG, &lRet, sizeof(long),  &cbCount) != SQL_SUCCESS)
         { 
            bOK = FALSE;
         };
             
         if (bOK)
         {
            bOK = SQLFetch(hstmt) == SQL_SUCCESS;        
         };
             
         AFX_SQL_SYNC(SQLFreeStmt(hstmt,SQL_DROP));         
      };      
   };   
   
  if (bOK)
  { 
     return lRet;
  };
  
  return 0;
};

///////////////////////////////////////////////////////////////////////////////
//
// Identify features based on a single attribute rather than feature and parent names
//

long CImportTable::DetermineFeature(long lFType, CString sId, CArray <CImportColumn,CImportColumn>& aColumns)
{
   CFeatureType ftype;
   long lFeature = 0;
   CAttrArray aAttr;
   int iAttr = 0;   
   double d;
   CImportPrimaryKey primarykey;

   // Determine the column to be used for identifying the feature

   if (m_nPrimaryKey == -1)
   {
      AfxGetApp()->BeginWaitCursor();

      if (BDFTypeI(BDHandle(), lFType, &ftype))
      {
         CDlgImportPrimaryKey dlg(ftype.m_lId);
         if (dlg.DoModal())
         {
            m_nPrimaryKey = dlg.GetAttrId();
         } else
         {
            return -1;
         }

         // Retrieve the data for the feature and store the id's and feature ids

         aAttr.m_lFType = ftype.m_lId;
         BOOL bFound = BDAttribute(BDHandle(), &aAttr, BDGETINIT);

         // Determine index of attribute

         if (bFound)
         {
            for (iAttr = 0; iAttr < aAttr.GetSize(); iAttr++)
            {
               if (aAttr[iAttr]->GetAttrId() == m_nPrimaryKey) 
               {
                  m_nDataType = aAttr[iAttr]->GetDataType();
                  break;
               }
            }
            ASSERT(iAttr < aAttr.GetSize());
         };

         while (bFound)
         {
            // Convert to integer
                     
            if (m_nDataType == BDNUMBER)
            {
               primarykey.m_dId = *aAttr[iAttr]->GetDouble();
            } else
            {
               primarykey.m_sId = aAttr[iAttr]->AsString();            
            }
            primarykey.m_sId.TrimLeft();
            primarykey.m_sId.TrimRight();

            primarykey.m_lFeature = aAttr.m_lFeature;
            m_aPrimaryKey.Add(primarykey);            

            bFound = BDGetNext(BDHandle());

         }
         BDEnd(BDHandle());
      };    

      AfxGetApp()->EndWaitCursor();
   };

   // Search for primary key 

   sId.TrimLeft();
   sId.TrimRight();

   if (sId != "")
   {
      if (m_nDataType == BDNUMBER)
      {
         sscanf(sId, "%lf", &d);
      }

      int i = 0; for (i = 0; i < m_aPrimaryKey.GetSize(); i++)
      {
         if ((m_nDataType == BDNUMBER && fabs(d-m_aPrimaryKey[i].m_dId) < 0.000001) ||                            
             (m_nDataType != BDNUMBER && sId == m_aPrimaryKey[i].m_sId))
         {
            lFeature = m_aPrimaryKey[i].m_lFeature;
         }
      }
   };

   return lFeature;
}
