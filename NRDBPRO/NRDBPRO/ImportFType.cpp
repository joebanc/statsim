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
#include "ImportFType.h"
#include "comboboxftype.h"
#include "dlgftypeattr.h"

#define BUFFER_SIZE 8096

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImportFType::CImportFType(CImportDB* pImportDB, LPCSTR sDataSource)
{
   m_pImportDB = pImportDB;   
   m_sDataSource = sDataSource;
   m_lFType = 0;
   m_lParentFType = 0;   
}

//////////////////////////////////////////////////////////////////////

CImportFType::~CImportFType()
{

}

//////////////////////////////////////////////////////////////////////

int CImportFType::Import()
{   
   BOOL bCancel = FALSE;
   int nTables = 0;   
   BOOL bOK = TRUE;
   CString sError;
   int nImported = 0;

   if (BDIsMySQL(BDHandle())) return ImportMySQL();

   // Count the number of selected tables

   int i = 0; for (i = 0;i < m_pImportDB->GetSize(); i++)
   {
      if (m_pImportDB->GetAt(i).m_bImport) nTables++;
   }
   nTables *= 3;

   // Connect 

   if (BDConnect(m_sDataSource, &m_hConnect))
   {      
     // Copy ftypes

   // First retrieve the projection

   if (m_projectionI.InitialiseProjection(m_hConnect))
   {
      AfxMessageBox("Converting from " + m_projectionI.GetProjectionName() +  
                 " to " + BDProjection()->GetProjectionName() + ".");
   } else
   {
      AfxMessageBox("No valid projection defined for database being imported");
      bOK = FALSE;
   }

      int i = 0; for (i = 0; bOK && !bCancel && i < m_pImportDB->GetSize(); i++)
      {
         if (m_pImportDB->GetAt(i).m_bImport)
         {            
            TRY
            {
               // Import feature type definition

               bOK = ImportFTypes(m_pImportDB->GetAt(i).m_ftype.m_lId);
               
               if (!m_dlgProgress.SetPercent(((++nImported) * 100) / nTables)) bCancel = TRUE;                           
               m_dlgProgress.SetText("Importing: " + m_pImportDB->GetAt(i).m_sFType);

               if (bOK) 
               {                   
                   fprintf(m_pImportDB->m_pLogFile, BDString(IDS_IMPORTEDDEF) + ": %s\r\n", m_pImportDB->GetAt(i).m_sTableImport);

                   bOK = ImportFeatures(m_pImportDB->GetAt(i).m_ftype.m_lId);

                   if (!m_dlgProgress.SetPercent(((++nImported) * 100) / nTables)) bCancel = TRUE;                           

                   // Import attributes

                   if (bOK)
                   {
                      fprintf(m_pImportDB->m_pLogFile, BDString(IDS_IMPORTEDFEATURES) +  ": %s\r\n", m_pImportDB->GetAt(i).m_sTableImport);

                      bOK = ImportAttr(m_pImportDB->GetAt(i).m_ftype.m_lId);

                      if (bOK)
                      {
                         fprintf(m_pImportDB->m_pLogFile, BDString(IDS_IMPORTEDDATA) + 
                            ":%s\r\n", m_pImportDB->GetAt(i).m_sTableImport);

                         if (!m_dlgProgress.SetPercent(((++nImported) * 100) / nTables)) bCancel = TRUE;                           
                      }
                   }
               }                                                                                          
            }
            CATCH (CDBException, pEx)
            { 
               BDGetError(BDHandle(), sError);
               fprintf(m_pImportDB->m_pLogFile, "%s: %s\r\n",m_pImportDB->GetAt(i).m_sTableImport, (LPCSTR)sError);
            }    
            END_CATCH          
         };
      };

      BDDisconnect(m_hConnect);
   }
   
   // Reset sectors

   CComboBoxFType::InitDictionary(TRUE);

   // Return

   if (bCancel) return -1;
   return nImported/3;
}

///////////////////////////////////////////////////////////////////////////////
//
// Imports features from an NRDB Access database into a MySQL database.  
// Because the NRDBAPI support MySQL API and not ODBC then conversion is
// done through SQL calls
//

int CImportFType::ImportMySQL()
{   
   BOOL bCancel = FALSE;
   int nTables = 0;   
   BOOL bOK = TRUE;
   CString sError;
   int nImported = 0;   

   // Count the number of selected tables

   int i = 0; for (i = 0;i < m_pImportDB->GetSize(); i++)
   {
      if (m_pImportDB->GetAt(i).m_bImport) nTables++;
   }   
     
   // TODO!!! convert projection   

   // Open database and create statement

   if (m_database.Open(m_sDataSource))
   {             
      // Import the selected features
  
      for (i = 0; bOK && !bCancel && i < m_pImportDB->GetSize(); i++)
      {
         if (m_pImportDB->GetAt(i).m_bImport)
         {            
            TRY
            {
               // Import feature type definition

               m_dlgProgress.SetText(m_pImportDB->GetAt(i).m_ftype.m_sDesc);

               long lFType2 = ImportFTypeMySQL(m_pImportDB->GetAt(i).m_ftype);            

               ImportFeatureMySQL(m_pImportDB->GetAt(i).m_ftype, lFType2);

               ImportAttrMySQL(m_pImportDB->GetAt(i).m_ftype, lFType2);

               if (!m_dlgProgress.SetPercent(((++nImported) * 100) / nTables)) bCancel = TRUE;
               
            }
            CATCH (CDBException, pEx)
            {                
               fprintf(m_pImportDB->m_pLogFile, "%s: %s\r\n",m_pImportDB->GetAt(i).m_sTableImport, (LPCSTR)pEx->m_strError);
            }    
            END_CATCH          
         };
      };

      m_database.Close();
   }; 

   
   
   // Reset sectors

   CComboBoxFType::InitDictionary(TRUE);

   // Return

   if (bCancel) return -1;
   return nImported;
}

//////////////////////////////////////////////////////////////////////
//
// Imports an ODBC table to a MySQL database
//

long CImportFType::ImportFTypeMySQL(CFeatureType ftype)
{
   HSTMT hstmt;
   CString sSQL;
   SDWORD dwLength;   
   CFTypeAttr attr;      
   char sColName[BD_SHORTSTR];
   char sDesc[BD_SHORTSTR];   
   CDictionary dictionary;
   int nRet;

   // Make a copy of the ftype to be saved

   CFeatureType ftype2 = ftype;

   // Determine dictionary entry

    SQLAllocStmt(m_database.m_hdbc, &hstmt);
    sSQL.Format("select `Description` from `BDDictionary` where `ID` = %d",ftype.m_lDictionary);
    SQLExecDirect(hstmt, (UCHAR*)sSQL.GetBuffer(0), sSQL.GetLength()); 
    SQLBindCol(hstmt, 1, SQL_C_CHAR, sDesc, BD_SHORTSTR, &dwLength); 
    if (SQLFetch(hstmt) == SQL_SUCCESS)
    {
       dictionary.m_sDesc = sDesc;
       if (!BDDictionary(BDHandle(), &dictionary, BDSELECT2))
       {                
          // Create a corresponding entry in the data dictionary
       
          dictionary.m_sDesc = sDesc;
          BDNextId(BDHandle(), BDDICTIONARY, 0, &dictionary.m_lId);             
          BDDictionary(BDHandle(), &dictionary, BDADD);
       }; 
       ftype2.m_lDictionary = dictionary.m_lId;
    }
    SQLFreeStmt(hstmt,SQL_DROP);                                        

   // Determine feature type

    ftype2.m_lId = DetermineFtype(m_database.m_hdbc, ftype.m_lId, ftype.m_sDesc);                              

    if (ftype2.m_lId == 0)
    {                    
      // Determine parent

      ftype2.m_lParentFType = DetermineFtype(m_database.m_hdbc, ftype.m_lParentFType); 

      // Create the feature type         

      BDNextId(BDHandle(), BDFTYPE, 0, &ftype2.m_lId);   
      BDFeatureType(BDHandle(), &ftype2, BDADD);

         // Now retrieve the attributes  

      SQLAllocStmt(m_database.m_hdbc, &hstmt);
      sSQL.Format("select `ID`, `Col_Name`, `Description`, `Data_Type_ID`, `FType_Link`, `Primary_Key`"
             "from `BDFtype_Attr` where `Feature_Type_ID` = %d order by `ID`", ftype.m_lId);
      nRet = SQLExecDirect(hstmt, (UCHAR*)sSQL.GetBuffer(0), sSQL.GetLength()); 
      SQLBindCol(hstmt, 1, SQL_C_LONG, &attr.m_lAttrId, sizeof(long), &dwLength); 
      SQLBindCol(hstmt, 2, SQL_C_CHAR, sColName, BD_SHORTSTR, &dwLength); 
      SQLBindCol(hstmt, 3, SQL_C_CHAR, sDesc, BD_SHORTSTR, &dwLength); 
      SQLBindCol(hstmt, 4, SQL_C_LONG, &attr.m_lDataType, sizeof(long), &dwLength); 
      SQLBindCol(hstmt, 5, SQL_C_LONG, &attr.m_lFTypeLink, sizeof(long), &dwLength); 
      SQLBindCol(hstmt, 6, SQL_C_LONG, &attr.m_bPrimaryKey, sizeof(BOOL), &dwLength); 

      while (SQLFetch(hstmt) == SQL_SUCCESS)
      {
         attr.m_sColName = sColName; // Update column names
         attr.m_sDesc = sDesc;

         attr.m_lFType = ftype2.m_lId;

         // For each link attribute, determine the ftype id in the MySQL database            

         if (attr.m_lDataType == BDLINK)
         {
            attr.m_lFTypeLink = DetermineFtype(m_database.m_hdbc, attr.m_lFTypeLink);
         }
   
         BDFTypeAttr(BDHandle(), &attr, BDADD);               
      }              
      SQLFreeStmt(hstmt,SQL_DROP);             
      
   // Create the tables corresponding to the attributes

      BDFTypeCreate(BDHandle(), ftype2.m_lId);
    };
      
   return ftype2.m_lId;
}

//////////////////////////////////////////////////////////////////////
//
// Imports features from OBDC to MySQL database
//

void CImportFType::ImportFeatureMySQL(CFeatureType ftype, long lFType2)
{
   CFeature feature;      
   BOOL bOK = TRUE;
   char sDesc[BD_SHORTSTR];
   HSTMT hstmt;
   CString sSQL;
   SDWORD dwLength;   
   int nRet;
  
   // Retrieve features to be imported

   SQLAllocStmt(m_database.m_hdbc, &hstmt);
   sSQL.Format("select `ID`, `Description`, `Parent_Feature` from `%s`", (LPCSTR)ftype.m_sInternal);
   nRet = SQLExecDirect(hstmt, (UCHAR*)sSQL.GetBuffer(0), sSQL.GetLength()); 
   SQLBindCol(hstmt, 1, SQL_C_LONG, &feature.m_lId, sizeof(long), &dwLength); 
   SQLBindCol(hstmt, 2, SQL_C_CHAR, sDesc, BD_SHORTSTR, &dwLength); 
   SQLBindCol(hstmt, 3, SQL_C_LONG, &feature.m_lParentFeature, sizeof(long), &dwLength); 
   
   while (SQLFetch(hstmt) == SQL_SUCCESS)
   {
      feature.m_sName = sDesc;
      feature.m_lFeatureTypeId = lFType2;
      
      // Save the feature to the MySQL database

      BDFeature(BDHandle(), &feature, BDADD);
      BDEnd(BDHandle());
   };
   SQLFreeStmt(hstmt,SQL_DROP);                
}

//////////////////////////////////////////////////////////////////////
//
// Import the data from an ODBC database and save it to a MySQL database
//

void CImportFType::ImportAttrMySQL(CFeatureType ftype, long lFType2)
{
   CAttrArray aAttr;
   CFTypeAttr ftypeattr;
   BOOL bOK = TRUE;      
   HSTMT hstmt;
   CString sSQL;
   SDWORD dwLength;   
   int nDummy;
   int nRet;
   int iRow =0;
   CString sProgress;

   SDWORD dw;
  
   // Retrieve the structure of the table
   
   bOK = BDFTypeAttrInit(BDHandle(), lFType2, &aAttr);   
   aAttr.m_lFType = lFType2;

   // Retrieve attributes to be imported

   SQLAllocStmt(m_database.m_hdbc, &hstmt);
 
   // TODO (not included 'ORDER' or NRDBORDER as depends on version)

   sSQL = "select `FEATURE_ID`, `LDATE`";
   int i = 0; for (i = 0; i < aAttr.GetSize(); i++)
   {   
      if (aAttr[i]->m_lDataType == BDCOORD)
      {
         sSQL += ", `" + aAttr[i]->m_sColName + "_x`";
         sSQL += ", `" + aAttr[i]->m_sColName + "_y`";
      } else
      {
         sSQL += ", `" + aAttr[i]->m_sColName + "`";
      };
   }
   sSQL += " from `" + ftype.m_sInternal + "_DATA`";   
   nRet = SQLExecDirect(hstmt, (UCHAR*)sSQL.GetBuffer(0), sSQL.GetLength()); 

   // Bind the columns to the attribute array

   int iCol=1;
   SQLBindCol(hstmt, iCol++, SQL_C_LONG, &aAttr.m_lFeature, sizeof(long), &dwLength); 
   SQLBindCol(hstmt, iCol++, SQL_C_LONG, &aAttr.m_lDate, sizeof(long), &dwLength); 

   for (i = 0; i < aAttr.GetSize(); i++)
   {
      switch (aAttr[i]->m_lDataType)
      {
         case BDNUMBER : 
             SQLBindCol(hstmt, iCol++, SQL_C_DOUBLE, aAttr[i]->GetDouble(), sizeof(double), &dwLength); 
         break;
         case BDTEXT : 
             SQLBindCol(hstmt, iCol++, SQL_C_CHAR, aAttr[i]->GetString(), BD_SHORTSTR, &dwLength); 
         break;
         case BDCOORD : 
             SQLBindCol(hstmt, iCol++, SQL_C_DOUBLE, &aAttr[i]->GetCoord()->x, sizeof(double), &dwLength); 
             SQLBindCol(hstmt, iCol++, SQL_C_DOUBLE, &aAttr[i]->GetCoord()->y, sizeof(double), &dwLength); 
         break;
         case BDMAPLINES : case BDIMAGE : case BDFILE : 
            SQLBindCol(hstmt, iCol++, SQL_C_BINARY, NULL, 0, &dw); 
         break;
         case BDLINK : 
            SQLBindCol(hstmt, iCol++, SQL_C_LONG, aAttr[i]->GetLink(), sizeof(long), &dwLength); 
         break;
         case BDHOTLINK: case BDLONGTEXT : 
            SQLBindCol(hstmt, iCol++, SQL_C_CHAR, NULL, 0, &dwLength); 
         break;
         case BDBOOLEAN : 
            SQLBindCol(hstmt, iCol++, SQL_C_LONG, aAttr[i]->GetBoolean(), sizeof(BOOL), &dwLength); 
         break;
         default:             
            SQLBindCol(hstmt, iCol++, SQL_C_CHAR, NULL, 0, &dwLength); 
            ASSERT(FALSE); // Undefinded type
         break;
      }            
   };   

   // Now retrieve the data

   i=0;
   while (SQL_SUCCEEDED(SQLFetch(hstmt)))
   {
      aAttr.m_lOrder = i++;         

      // TODO!! convert projection
     
      // Retrieve the data for the memo and binary fields

      int iCol = 3;
      for (i = 0; i < aAttr.GetSize(); i++)
      {
          switch (aAttr[i]->m_lDataType)
          {
             case BDMAPLINES : case BDIMAGE : case BDFILE : 
                {

					CLongBinary *pLongBin = aAttr[i]->GetLongBinary();

                   // Determine the length of the object to be imported and allocate memory

                   if (SQL_SUCCEEDED(::SQLGetData(hstmt,(UWORD)iCol, SQL_C_BINARY, &nDummy, 0, &dwLength)))
				   {
					   
					   if (dwLength > 0)
					   {
						   if (pLongBin->m_hData != NULL)
						   {
							  pLongBin->m_hData = GlobalReAlloc(pLongBin->m_hData,dwLength,0);     
						   } else
						   {
							  pLongBin->m_hData = GlobalAlloc(GMEM_MOVEABLE|GMEM_DISCARDABLE, dwLength);
						   };
					   } else
					   {
						   if (pLongBin->m_hData != NULL) GlobalFree(pLongBin->m_hData);
						   pLongBin->m_hData = NULL;
					   };
					   pLongBin->m_dwDataLength = dwLength;
				   } else
				   {
					   ASSERT(FALSE);
				   }

                   // Now retrieve the data in chunks
                   BYTE* pData = (BYTE*)GlobalLock(pLongBin->m_hData);
                   if (pData != NULL)
                   {
                       long lOffset = 0;
                       long lTotalLength = dwLength;
                       while (lOffset < lTotalLength)
                       {
                          DWORD lBufferLen = min(lTotalLength-lOffset, BUFFER_SIZE);
                          if (!SQL_SUCCEEDED(SQLGetData(hstmt, iCol, SQL_C_BINARY, pData+lOffset, lBufferLen, &dwLength)))
                          {
                             break;
                          }
                          
                          lOffset += lBufferLen;
                       };
                       GlobalUnlock(pLongBin->m_hData);
                   };
                   iCol++;

#ifdef _DEBUG
     // Check if intact
                   //CLongLines *pMapLines = new CLongLines(*aAttr[i]->GetLongBinary());            

#endif
                }
                break;

             case BDHOTLINK: case BDLONGTEXT : 

              // Determine the length of the object to be imported and allocate memory

               if (::SQLGetData(hstmt,(UWORD)iCol, SQL_C_DEFAULT, &nDummy, 0, &dwLength) == SQL_SUCCESS)
               {
                  char *pData = new char[dwLength+1];
                  if (pData != NULL)
                  {
                       long lOffset = 0;
                       long lTotalLength = dwLength;
                       while (lOffset < lTotalLength)
                       {
                          DWORD lBufferLen = min(lTotalLength-lOffset, BUFFER_SIZE);
                          if (!SQL_SUCCEEDED(SQLGetData(hstmt, iCol, SQL_C_DEFAULT, pData+lOffset, lBufferLen, &dwLength)))
                          {
                             break;
                          }
                          
                          lOffset += lBufferLen;
                       }
                       *aAttr[i]->GetLongText() = pData;   
					   delete [] pData;
                  };                
               };
			   iCol++;
               break;

             // Change links with zero to null to maintain referencial intergrity
             case BDLINK:
                {
                   if (*aAttr[i]->GetLink() == 0) aAttr[i]->SetLink(AFX_RFX_LONG_PSEUDO_NULL);
                   iCol++;
                }
                break;

                
            // Skip two columns for coordinates
             case BDCOORD : 
                iCol+=2;
                break;
             default:
                iCol++;
          }
      }

	  sProgress.Format("%s %d", (LPCSTR)aAttr.GetFTypeName(), ++iRow);
	  m_dlgProgress.SetText(sProgress);

      // Save the data
      bOK = BDAttribute(BDHandle(), &aAttr, BDADD);  
	  BDEnd(BDHandle());
   }

#ifdef _DEBUG
		SWORD nOutlen;
		UCHAR lpszMsg[SQL_MAX_MESSAGE_LENGTH];
		UCHAR lpszState[SQL_SQLSTATE_SIZE];
		CString strMsg;
		CString strState;
		SDWORD lNative;
		
		::SQLError(NULL, m_database.m_hdbc,
			hstmt, lpszState, &lNative,
			lpszMsg, SQL_MAX_MESSAGE_LENGTH-1, &nOutlen);

#endif

   SQLFreeStmt(hstmt,SQL_DROP);                

}

//////////////////////////////////////////////////////////////////////
//
// Determines the ftype in the MySQL database corresponding to the
// id or name given
//

long CImportFType::DetermineFtype(HDBC hdbc, long lId, CString sFType)
{
   HSTMT hstmt;
   CString sSQL;
   SDWORD dwLength;   
   char sDesc[BD_SHORTSTR];

   // If no description then query from Access database

   if (sFType == "")
   {
      SQLAllocStmt(hdbc, &hstmt);
      sSQL.Format("select `Description` from `BDFeature_Types` where `ID` = %d", lId);
      SQLExecDirect(hstmt, (UCHAR*)sSQL.GetBuffer(0), sSQL.GetLength()); 
      SQLBindCol(hstmt, 1, SQL_C_CHAR, sDesc, BD_SHORTSTR, &dwLength); 
      if (SQLFetch(hstmt) == SQL_SUCCESS)      
      {
         sFType = sDesc;
      }
      SQLFreeStmt(hstmt,SQL_DROP);                                        
   }
   
   CFeatureType ftype;
   ftype.m_sDesc = sFType;
   if (BDFeatureType(BDHandle(), &ftype, BDSELECT2))
   {
      lId =  ftype.m_lId;
   } else
   {
      lId = 0;
   }
   BDEnd(BDHandle());

   return lId;
}

//////////////////////////////////////////////////////////////////////

BOOL CImportFType::ImportFTypes(long lFTypeI)
{
   CAttrArray aAttr;
   BOOL bOK = TRUE;
   CFeatureType ftypeI, ftype, ftypeP;
   CFTypeAttr ftypeattr;
   CDictionary dictionaryI, dictionary;


   // Determine next id
      
   BDNextId(BDHandle(), BDFTYPE, 0, &m_lFType);

   // Retrieve feature definition

   ftypeI.m_lId = lFTypeI;
   
   // Retrieve attribute definition
   
   if (BDFeatureType(m_hConnect, &ftypeI, BDSELECT) && 
       BDFTypeAttrInit(m_hConnect, ftypeI.m_lId, &aAttr))
   {
      // Check the sector exists, if it does not then add it

      dictionaryI.m_lId = ftypeI.m_lDictionary;
      BDDictionary(m_hConnect, &dictionaryI, BDSELECT);
      dictionary.m_sDesc = dictionaryI.m_sDesc;
      if (!BDDictionary(BDHandle(), &dictionary, BDSELECT2))
      {
         BDNextId(BDHandle(), BDDICTIONARY, 0, &dictionaryI.m_lId);

         BDDictionary(BDHandle(), &dictionaryI, BDADD);
         BDEnd(BDHandle());         
         ftypeI.m_lDictionary = dictionaryI.m_lId;
      } else
      {
         ftypeI.m_lDictionary = dictionary.m_lId;
      }

      // Determine the ftype parent

      if (ftypeI.m_lParentFType > 0)
      {
         m_lParentFType = GetFType(ftypeI.m_lParentFType);
         if (m_lParentFType < 0) bOK = FALSE;
      } else
      {
         m_lParentFType = 0;
      }
      
      // If the feature type exists then update the name

      int i = 1;
      m_sFType = ftypeI.m_sDesc;
      ftype.m_sDesc = m_sFType;
      if (BDFeatureType(BDHandle(), &ftype, BDSELECT2))
      {
         bOK = FALSE;
         fprintf(m_pImportDB->m_pLogFile, BDString(IDS_FEATUREEXISTS) + ": %s\r\n", ftypeI.m_sDesc);         
      }
      BDEnd(BDHandle());

      // Create feature type
      
      ftype = ftypeI;
      ftype.m_lId = m_lFType;
      ftype.m_sDesc = m_sFType;
      ftype.m_sInternal = m_sFType;
      ftype.m_lParentFType = m_lParentFType;
      if (bOK && BDFeatureType(BDHandle(), &ftype, BDADD))
      {
         // Create attribute definitions

         int j = 0; for (j = 0; bOK && j < aAttr.GetSize(); j++)
         {
            ftypeattr = *(CFTypeAttr*)aAttr[j];  
            ftypeattr.m_lFType = m_lFType;

            // Match the column name to the attribute name

            ftypeattr.m_sColName = ftypeattr.m_sDesc;

            if (ftypeattr.m_lDataType == BDLINK)
            {
               long lLinkFType = GetFTypeLink(ftypeattr.GetFTypeLink());
               if (lLinkFType > 0)
               {               
                  ftypeattr.SetFTypeLink(lLinkFType);
               } else
               {
                  bOK = FALSE;
               }
            }
            if (bOK) bOK = BDFTypeAttr(BDHandle(), &ftypeattr, BDADD);
         }

         // Create corresponding table for attributes

         if (bOK)
         {
            bOK = BDFTypeCreate(BDHandle(), m_lFType);
         };            

      } else
      {
         bOK = FALSE;
      }
   } else
   {
      bOK = FALSE;
   }
   
   BDEnd(BDHandle());
   BDEnd(m_hConnect);
      
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Returns the ftype from the ftype in the database being imported
//

long CImportFType::GetFType(long lFTypeI)
{
   BOOL bOK = TRUE;
   CFeatureType ftype, ftypeI;

   // Get the feature type

   ftypeI.m_lId = lFTypeI;
   BDFeatureType(m_hConnect, &ftypeI, BDSELECT);

   ftype.m_sDesc = ftypeI.m_sDesc;
   if (BDFeatureType(BDHandle(), &ftype, BDSELECT2))
   {         
      return ftype.m_lId;
   } else
   {
      fprintf(m_pImportDB->m_pLogFile, BDString(IDS_NOFEATURE) + ": %s\r\n", ftypeI.m_sDesc);
      bOK = FALSE;
   }
   return -1;
}

///////////////////////////////////////////////////////////////////////////////
//
// Imports features

BOOL CImportFType::ImportFeatures(long lFTypeI)
{
   CFeature feature;
   CFeatureType ftype;
   CArray <CFeature, CFeature> aFeatures;
   BOOL bOK = TRUE;
  
   // Retrieve features to be imported

   feature.m_lFeatureTypeId = lFTypeI;

   BOOL bFound = BDFeature(m_hConnect, &feature, BDGETINIT);
   while (bFound)
   {           
      aFeatures.Add(feature);

      bFound = BDGetNext(m_hConnect);
   }
   BDEnd(m_hConnect);

   // Determine if the parent features match

   ftype.m_lId = lFTypeI;   
   if (!BDFeatureType(m_hConnect, &ftype, BDSELECT) ||
       (ftype.m_lParentFType != 0 &&
        GetFTypeLink(ftype.m_lParentFType, m_lParentFType) <= 0))
   {
      bOK = FALSE;
   }
   BDEnd(m_hConnect);

   
   // Check if many to one

   ftype.m_lId = lFTypeI;
   if (BDFeatureType(m_hConnect, &ftype, BDSELECT) && 
       (ftype.m_bManyToOne || ftype.m_lParentFType == 0))
   {
      // Save the features   

      int i = 0; for (i = 0; bOK && i < aFeatures.GetSize(); i++)
      {
         feature = aFeatures[i];
         feature.m_lFeatureTypeId = m_lFType;      
         bOK = BDFeature(BDHandle(), &feature, BDADD);
         BDEnd(BDHandle());
      }
   } 
   BDEnd(m_hConnect);

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Determines which feature type the link belongs to 
// Returns the ftype or -1 if not found
//

long CImportFType::GetFTypeLink(long lFTypeI, long lFType)
{
   CFeature feature;
   CFeatureType ftype;
   CArray <CFeature, CFeature> aFeatures;
   BOOL bOK = TRUE;

   // If the feature type is undefined then determine its value

   if (lFType == 0)
   {
      lFType = GetFType(lFTypeI);
      if (lFType < 0) return -1;
   }

   // Retrieve features of the link type

   feature.m_lFeatureTypeId = lFTypeI;

   BOOL bFound = BDFeature(m_hConnect, &feature, BDGETINIT);
   while (bFound)
   {
      feature.m_sName.TrimRight();
      aFeatures.Add(feature);

      bFound = BDGetNext(m_hConnect);
   }
   BDEnd(m_hConnect);

   // Determine if the features match

   int i = 0; for (i = 0; bOK && i < aFeatures.GetSize(); i++)
   {      
      feature.m_lFeatureTypeId = lFType;
      feature.m_lId = aFeatures[i].m_lId;
      BDFeature(BDHandle(), &feature, BDSELECT);
      feature.m_sName.TrimRight();

      if (feature.m_sName != aFeatures[i].m_sName)
      {
         ftype.m_lId = lFTypeI;
         BDFeatureType(m_hConnect, &ftype, BDSELECT);
         aFeatures[i].m_sName.TrimRight();
         fprintf(m_pImportDB->m_pLogFile, BDString(IDS_NOMATCH) + ":%s, %s\r\n",aFeatures[i].m_sName, ftype.m_sDesc);
         bOK = FALSE;
      }
   }

   if (!bOK) return -1;
   else return lFType;
}

///////////////////////////////////////////////////////////////////////////////
//
// Imports the data for the feature type
//

BOOL CImportFType::ImportAttr(long lFTypeI)
{
   CAttrArray aAttrI, aAttrL, aAttr;
   CFTypeAttr ftypeattr;
   BOOL bOK = TRUE;   
   int iRow = 0;
   CString sProgress;
  
   // Copy data
   
   bOK = BDFTypeAttrInit(BDHandle(), m_lFType, &aAttrL);   

   // Save the data

   aAttrI.m_lFType = lFTypeI;
   BOOL bFound = BDAttribute(m_hConnect, &aAttrI, BDGETINIT);
   while (bFound && bOK)
   {
      // Update the link ftypes

      aAttr = aAttrI;
      aAttr.m_lFType = m_lFType;
      int i = 0; for (i = 0; i < aAttrI.GetSize(); i++)
      {                  
         aAttr[i]->SetFTypeId(m_lFType);
         aAttr[i]->m_sColName = aAttr[i]->m_sDesc;
         if (aAttr[i]->GetDataType() == BDLINK)
         {
            aAttr[i]->SetFTypeLink(aAttrL[i]->GetFTypeLink());
         };
      };

      // Make the internal name the same as the external

      aAttr.SetFTypeInternal(aAttr.GetFTypeName());

      // Convert to the current projection

      ConvertProjection(aAttr);

	  // Update progress
	  sProgress.Format("%s %d", (LPCSTR)aAttr.GetFTypeName(), ++iRow);
	  m_dlgProgress.SetText(sProgress);

      // Save the value

      bOK = BDAttribute(BDHandle(), &aAttr, BDADD);
      BDEnd(BDHandle());

      // Retrieve next

      bFound = BDGetNext(m_hConnect);
   }
   BDEnd(BDHandle());
   BDEnd(m_hConnect);

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Convert the projection from that of the database being imported from to
// that being imported to
//

BOOL CImportFType::ConvertProjection(CAttrArray& aAttr)
{
   BOOL bOK = TRUE;
   int i = 0; for (i = 0; i < aAttr.GetSize(); i++)
   {
      if (aAttr.GetAt(i)->GetDataType() == BDCOORD)
      {
         // Convert coord
         
         if (!aAttr.GetAt(i)->GetCoord()->IsNull())
         {
            CLongCoord coord = CLongCoord((long)aAttr[i]->GetCoord()->x,(long)aAttr[i]->GetCoord()->y);
            ConvertProjection(coord);
            aAttr[i]->SetCoord(CCoord(coord.x,coord.y));
         }          
         else
         {
            // Convert to psuedo null from dbl_max for compatability
            // with other databases

            aAttr.GetAt(i)->GetCoord()->SetNull();
         }
      } else if (aAttr.GetAt(i)->GetDataType() == BDMAPLINES)
      {         
         CLongBinary* pLongBin = aAttr.GetAt(i)->GetLongBinary();
		 if (pLongBin->m_hData != NULL)
		 {
			 CLongLines maplines(*pLongBin);

			 int j = 0; for (j = 0; j < maplines.GetSize(); j++)
			 {
				if (!maplines.GetAt(i).IsNull())
				{
				   ConvertProjection(maplines.GetAt(j));
				} else
				{
				   maplines.GetAt(j).SetNull();
				}
			 }

			 // Convert back

			 maplines.GetLongBinary(*aAttr.GetAt(i)->GetLongBinary());
		 }
      }
   }
   return bOK;
};

///////////////////////////////////////////////////////////////////////////////

void CImportFType::ConvertProjection(CLongCoord& coord)
{
  // Convert to latlon

   double dLat, dLon;

   m_projectionI.TransMercatorToLatLon(coord.x, coord.y, &dLat, &dLon);
   BDProjection()->LatLonToTransMercator(dLat, dLon, &coord.x,  &coord.y);
}

///////////////////////////////////////////////////////////////////////////////

/*void CImportFType::ConvertProjection(CCoord& coord)
{
   // Convert to latlon

   double dLat, dLon;

   m_projectionI.TransMercatorToLatLon(coord.x, coord.y, &dLat, &dLon);
   BDProjection()->LatLonToTransMercator(dLat, dLon, &coord.x,  &coord.y);
}*/
