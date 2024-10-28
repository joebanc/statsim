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
#include <odbcinst.h>

#include "nrdb.h"
#include "ImportTextFile.h"
#include "importodbc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

#define MAX_DSN_LEN 32

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImportTextFile::CImportTextFile()
{

}

CImportTextFile::~CImportTextFile()
{

}

//////////////////////////////////////////////////////////////////////

void CImportTextFile::ImportShapefile()
{      
}

///////////////////////////////////////////////////////////////////////////////

void CImportTextFile::ImportFile()
{
   CString sDriver;
   BOOL bSysTables = FALSE;
   int nFileType = -1;
   CString sTable;

   CFileDialog dlg(TRUE, "xls", NULL, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, 
	                BDString(IDS_ALLFILES) + "|*.xls;*.dbf;*.mdb;*.txt;*.csv;*.shp|" + 
                   BDString(IDS_EXCELFILE) + " (*.xls)|*.xls|" + BDString(IDS_ACCESSFILE) +" (*.mdb)|*.mdb|" +
                   BDString(IDS_DBASEFILE) + " (*.dbf)|*.dbf|" + BDString(IDS_TEXTFILE) + " (*.txt, *.csv)|*.txt;*.csv|" +
                   BDString(IDS_SHAPEFILE) + " (*.shp)|*.shp||");
   
   if (dlg.DoModal() == IDOK)
   {
       CString sPath = dlg.GetPathName();

       if (dlg.GetFileExt().CompareNoCase("xls") == 0)
       {
          sDriver = BDString(IDS_EXCELDRIVER);          
          nFileType = CImportDB::excel;
          bSysTables = TRUE;          
       }
       else if (dlg.GetFileExt().CompareNoCase("dbf") == 0)
       {
          sDriver = BDString(IDS_DBASEDRIVER);                              
          sTable = dlg.GetFileTitle();
		    nFileType = CImportDB::dbase;
       } 
       else if (dlg.GetFileExt().CompareNoCase("mdb") == 0)
       {
          sDriver = BDString(IDS_ACCESSDRIVER);                              
          nFileType = CImportDB::access;
       }
       else if (dlg.GetFileExt().CompareNoCase("shp") == 0)
       {
          sDriver = BDString(IDS_DBASEDRIVER);
          sTable = dlg.GetFileTitle();
          CString sPath = dlg.GetPathName();
          sPath = sPath.Mid(0, sPath.ReverseFind('.')+1) + "dbf";		 
          nFileType = CImportDB::shapefile;
       }
       else
       {
          sDriver = BDString(IDS_TEXTDRIVER);
          sTable = dlg.GetFileName();
          nFileType = CImportDB::text;
       }

       // Create the data source

       CString sDSN = dlg.GetFileTitle();
       if (InitDataSource(sDriver, sPath, sDSN, sTable))
       {
          // Display the import dialog

          CImportDB import;
          import.ImportODBC(sDSN, sTable, bSysTables, nFileType);

          // Tidy up

          RemoveDataSource(sDriver, sDSN);
       }              
   }
}

///////////////////////////////////////////////////////////////////////////////
//
// Install ODBC Data Source
//

BOOL CImportTextFile::InitDbaseDataSource(LPCSTR sPath, CString &sTitle)
{  
   	
  /* CString sDriver = BDString(IDS_DBASEDRIVER);
   
   return InitDataSource(sDriver, sPath, sTitle, "");*/

   
   int nRes;
   CString szArgs,szDBQ, sDir;
   
   // Determine the database file
   
    szDBQ = sPath;

    sDir = szDBQ.Left(szDBQ.ReverseFind('\\'));

           
    // Create the registration string

    szArgs = "DSN=" + CString(sTitle) + "|" +             
             "DefaultDir=" + sDir + "|" +
             CString("Driver=") + "odbcjt32.dll|"+             
             CString("FIL=dBase III;|") +              
             "SafeTransactions=0|"+
             "UID=|";
    
    // Substitute '|' for '\0', Nb. CString::operator+ strips trailing '\0's

    for (int i = szArgs.GetLength()-1; i >= 0; i--)
    {
       if (szArgs[i] == '|') szArgs.SetAt(i,'\0');
    }

    nRes = SQLConfigDataSource(NULL,ODBC_ADD_DSN,
                               BDString(IDS_DBASEDRIVER), szArgs);

    if (nRes == FALSE)
    {
        ISODBCDisplaySQLInstallerErrorMsg();
    };    

    return nRes;
	
};

///////////////////////////////////////////////////////////////////////////////

void CImportTextFile::RemoveDbaseDataSource(LPCSTR sTitle)
{
   CString sArgs = "DSN=" + CString(sTitle);
   SQLConfigDataSource(NULL,ODBC_REMOVE_DSN, BDString(IDS_DBASEDRIVER),sArgs);          
};

///////////////////////////////////////////////////////////////////////////////

BOOL CImportTextFile::InitDataSource(LPCSTR sDriver, LPCSTR szFile, CString& sTitle, LPCSTR sTable)
{   
   int nRes;
   CString szArgs,szDBQ, sDir;
   
   // Determine the database file
   
    szDBQ = szFile;

    sDir = szDBQ.Left(szDBQ.ReverseFind('\\'));
      
    // Check if the data source name already exists
    // Ensure data source name does not exceed 32 characters

    CString sDSN;
    int i = 0; for (i = 0; i < min(sTitle.GetLength(), MAX_DSN_LEN-2); i++)
    {
       if (isalnum(sTitle[i])) sDSN += sTitle[i];       
    };   

    i = 1;
    sTitle = sDSN;
    while (IsExistDSN(sDSN))
    {       
       sDSN.Format("%s%i", sTitle, i++);
    }
    // Return new DSN
    sTitle = sDSN;
           
    // Create the registration string

    szArgs = "DSN=" + sDSN + "|" +             
             "DefaultDir=" + sDir + "|";

    if (sTable[0] == '\0') szArgs += "DBQ=" + CString(szFile) + "|";
        
    // Substitute '|' for '\0', Nb. CString::operator+ strips trailing '\0's

    for (i = szArgs.GetLength()-1; i >= 0; i--)
    {
       if (szArgs[i] == '|') szArgs.SetAt(i,'\0');
    }

    nRes = SQLConfigDataSource(NULL,ODBC_ADD_DSN,
                               sDriver,szArgs);

    if (nRes == FALSE)
    {
        ISODBCDisplaySQLInstallerErrorMsg();
    };    

    return nRes;
};

///////////////////////////////////////////////////////////////////////////////

BOOL CImportTextFile::IsExistDSN(LPCSTR sDSN2)
{
   HENV henv;
   UCHAR sDSN[SQL_MAX_DSN_LENGTH+1];
   UCHAR sDescription[255];
   SWORD wLenDSN, wLenDesc;	
   BOOL bFound = FALSE;

   if (SQLAllocEnv(&henv) == SQL_SUCCESS)
   {
      SWORD wDirection = SQL_FETCH_FIRST;         
      while (SQLDataSources(henv, wDirection, sDSN, sizeof(sDSN), &wLenDSN, sDescription, sizeof(sDescription),
                     &wLenDesc) == SQL_SUCCESS)
      {
         if (stricmp((char*)sDSN, sDSN2) == 0) return TRUE;
         
         wDirection = SQL_FETCH_NEXT;                  
      }
      SQLFreeEnv(henv);
   };
   return FALSE;
};

///////////////////////////////////////////////////////////////////////////////

void CImportTextFile::RemoveDataSource(LPCSTR sDriver, LPCSTR sTitle)
{
   CString sArgs = "DSN=" + CString(sTitle);
   SQLConfigDataSource(NULL,ODBC_REMOVE_DSN, sDriver, sArgs);          
};

///////////////////////////////////////////////////////////////////////////////

void CImportTextFile::ISODBCDisplaySQLInstallerErrorMsg()
{
   ULONG fErrorCode;
   int  nvResult;
   char szErrorMsg[SQL_MAX_MESSAGE_LENGTH];
   USHORT cbErrorMsg, cbErrorMsgMax, iError;
          
   iError = 1;
   cbErrorMsgMax = SQL_MAX_MESSAGE_LENGTH;

   nvResult = SQLInstallerError (iError, &fErrorCode, szErrorMsg,
                                 cbErrorMsgMax, &cbErrorMsg );   
   
   if ( nvResult != SQL_SUCCESS)
   {
      CString s;
      s.Format(BDString(IDS_ODBCERROR) + ": %i", fErrorCode);
      AfxMessageBox(s);
   }  else
   {
      CString s;
      s.Format(BDString(IDS_ODBCERROR) + ": %s", szErrorMsg);
      AfxMessageBox(s);
   }
};

///////////////////////////////////////////////////////////////////////////////
//
// DBase uses short file names for the names of its tables
//

CString CImportTextFile::GetShortFileTitle(LPCSTR sPath)
{
   CString sTable;
	char szDBase[_MAX_PATH];

   GetShortPathName(sPath, szDBase, sizeof(szDBase));
   sTable = szDBase;
   sTable = sTable.Mid(sTable.ReverseFind('\\')+1);
   sTable = sTable.Left(sTable.ReverseFind('.'));

   return sTable;
}