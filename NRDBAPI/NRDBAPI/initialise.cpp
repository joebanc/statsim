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

#ifdef NRDBMYSQL
#define L
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Updates an existing database to the current version
//

BOOL BDUpdate(CNRDBase *pDBase)
{
   BOOL bOK = TRUE;

   CString sLongBinary = "longbinary";   
   if (!pDBase->IsJetDB()) sLongBinary = "longblob"; 
   
  // Add a new column to the table containing the version number, for
  // necessary for old versions of the database

   TRY
   {
     // Add columns for custom html (NRDB Pro 2.0 Database version 3)

      if (pDBase->GetDBVersion() < 3)
      {
       // Create table for symbols         

         pDBase->ExecuteSQL("create table `BDSymbol` (`ID` integer not null, "
                            "`Image` " + sLongBinary+ ")");

         pDBase->ExecuteSQL("ALTER TABLE `BDSymbol` ADD CONSTRAINT bdsymbol_1 PRIMARY KEY (`ID`)");

        // Update BDMain for custom headers

         pDBase->ExecuteSQL("alter table `BDMain` add `UseCustomHeader` bit");
	      pDBase->ExecuteSQL("alter table `BDMain` add `CustomHeader` text");
	      pDBase->ExecuteSQL("alter table `BDMain` add `UseCustomFooter` Bit");
	      pDBase->ExecuteSQL("alter table `BDMain` add `CustomFooter` text");
	      pDBase->ExecuteSQL("alter table `BDMain` add `StyleSheet` varchar(254)");
						    
	    // Add version number to database

         if (pDBase->GetDBVersion() < 2)
         {
            CString sSQL; 
            sSQL.Format("alter table `BDMain` add `version` smallint");
            pDBase->ExecuteSQL(sSQL);           
         };      

      };
   }
   CATCH(CDBException, pEx)
   {
      char sError[254];
      pEx->GetErrorMessage(sError, sizeof(sError));
      if (pDBase->m_nOnError == DisplayMessage)
      {         
         AfxMessageBox(sError);
      } else
      {
#ifdef _DEBUG
         AfxMessageBox("Debug: " + CString(sError));
#endif      
      }
      bOK = FALSE;
   }
   END_CATCH

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Creates the tables for a new data source
//

BOOL BDInitialise(LPCSTR sDSN, LPCSTR sUser, LPCSTR sPassword, LPCSTR sHost)
{
   BOOL bOK = FALSE;
   CNRDBase database;
        
   if (database.Open(sDSN, sUser, sPassword, sHost))
   {
         // MySQL longbinary objects are thought to be OLE objects under NT
      CString sLongBinary = "longbinary";   
      if (!database.IsJetDB()) sLongBinary = "longblob"; 

      TRY
      {
         // Create tables         

         // Dictionary
         database.ExecuteSQL("CREATE TABLE `BDDictionary` ( `ID` integer not null,"
                             "`Description` varchar(254))");
         database.ExecuteSQL("ALTER TABLE `BDDictionary` ADD CONSTRAINT bddictionary_1 PRIMARY KEY (`ID`)");
         database.ExecuteSQL("insert into `BDDictionary` ( `ID`, `Description` ) values (1, 'Default')");

         // BDFeature_Types

         database.ExecuteSQL("CREATE TABLE `BDFeature_Types` (`ID` integer not null, `Description` varchar(254),"
                             "`Internal` varchar(254), `Parent_FType` integer, `ManyToOne` bit," 
                             "`Dictionary` integer)");
         database.ExecuteSQL("ALTER TABLE `BDFeature_Types` ADD CONSTRAINT bdfeature_types_1 PRIMARY KEY (`ID`)");
         database.ExecuteSQL("ALTER TABLE `BDFeature_Types` ADD CONSTRAINT bdfeature_types_2 FOREIGN KEY (`Dictionary`)"
                             "REFERENCES `BDDictionary`( `ID`)");

         // BDFType_Attr

         database.ExecuteSQL("CREATE TABLE `BDFtype_Attr` (`ID` integer not null, `Feature_Type_ID` integer not null,"
                             "`Col_Name` varchar(254), `Description` varchar(254),"
                             "`Data_Type_ID` integer, `FType_Link` integer, `Primary_Key` bit)");

         database.ExecuteSQL("ALTER TABLE `BDFtype_Attr` ADD CONSTRAINT bdftype_attr_1 PRIMARY KEY (`ID`,`Feature_Type_ID`)");
         database.ExecuteSQL("ALTER TABLE `BDFtype_Attr` ADD CONSTRAINT bdftype_attr_2 FOREIGN KEY (`Feature_Type_ID`) REFERENCES `BDFeature_Types`(`ID`)");        


         // BDMain

         database.ExecuteSQL("CREATE TABLE `BDMain` ( `Version` integer, `Country` varchar(254),"
                             "`Province` varchar(254), `City` varchar(254), `Organization` varchar(254),"
                             "`Address`      varchar(254),  `Telephone` varchar(254), `Fax` varchar(254),"
                             "`Email` varchar(254), `Logo` varchar(254), `Coordaslatlon` bit, "
							 "`UseCustomHeader` bit, `CustomHeader` text, `UseCustomFooter` Bit, "
							 "`CustomFooter` text, `StyleSheet` varchar(254))");
         database.ExecuteSQL("insert into `BDMain` ( `Version`,`Organization`) values (3, 'Natural Resources Database')");

        // Projection

         database.ExecuteSQL("CREATE TABLE `BDProjection` (`ID` integer not null, `Name` varchar(254),"
                             "`Semi-major axis` double, `Semi-minor axis` double, "
                             "`Latitude origin` double, `Longitude` double, "
                             "`False easting` integer, `False northing` integer, "
                             "`Scale factor at origin` double, `Scale Coords` integer,"
                             "`Default` bit, `MinEasting` double, `MaxEasting` double,"
                             "`MinNorthing` double, `MaxNorthing` double)");                         

         database.ExecuteSQL("alter table bdprojection add constraint bdprojection_1 primary key (`ID`)");

         // Custom symbols

         database.ExecuteSQL("create table `BDSymbol` (`ID` integer not null, "
                            "`Image` " + sLongBinary + ")");

         database.ExecuteSQL("ALTER TABLE `BDSymbol` ADD CONSTRAINT bdsymbol_1 PRIMARY KEY (`ID`)");

         bOK = TRUE;
      };
      CATCH (CDBException, pEx)
      {          
         AfxMessageBox(pEx->m_strError);
      }
      END_CATCH
 
      database.Close();
   };


   return bOK;
}
