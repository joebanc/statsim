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

CString ColumnAsString(BDHANDLE hConnect, CString sFtypeInternal, CAttribute* pAttr);
CString DataTypeAsString(CString sName, long lDataType);
CString GetFirstCSV(CString& s);

void AddPrimaryKey(CNRDBase* pDBase, CAttrArray& aAttr);

///////////////////////////////////////////////////////////////////////////////
//
// Create a table to contain the data for the specified feature type, based on
// the contents of the corresponding attribute type columns
//
// Table Name: FTYPE_#ID
//    Feature_ID  long integer } 
//    Date        long integer } primary key
//    Order       long integer }
//    [#Attr_ID  [float, text, long binary]+
//

BOOL BDFTypeCreate(BDHANDLE hConnect, long lFType)
{   
   BOOL bOK = TRUE;
       
   long lFeatureType = lFType;   
   CFeatureType ftype, ftypeP, ftypeI;
   CAttrArray aAttr, aAttrP;      
   CString sDataType;
   CString sSQL, sColumn;
   CAttrArray aAttrLink;

    // Get database handle
      
   CNRDBase* pDBase = GetNRDBase(hConnect); 
   if (pDBase == NULL)
   { 
      bOK = FALSE;
   };

   // Retrieve the attributes for the feature type

   BDFTypeAttrInit(hConnect, lFType, &aAttr);

   // Retrieve feature type details

   ftype.m_lId = lFType;
   BDFeatureType(hConnect, &ftype, BDSELECT);
   BDEnd(hConnect);

   // Now create the table to contain the features

   if (bOK)
   {   
      // Don't create table for one to one relationships

      if (ftype.m_lParentFType == 0 || ftype.m_bManyToOne || pDBase->GetDBVersion() < 3)
      { 
         sSQL.Format("create table `%s` (ID integer not null, Description varchar(%li), Parent_Feature integer, constraint `primarykey` primary key (ID)) ", aAttr.GetFTypeTable(pDBase), BD_NAMESTR);      
         pDBase->ExecuteSQL(sSQL);               		        

         // Create an index on the feature ids

         sSQL.Format("create index `index_1` on `%s` (`id`)", aAttr.GetFTypeTable(pDBase));
         pDBase->ExecuteSQL(sSQL);               		        
    
         sSQL.Format("create index `index_2` on `%s` (`Description`)", aAttr.GetFTypeTable(pDBase));
         pDBase->ExecuteSQL(sSQL);               		        

         sSQL.Format("create index `index_3` on `%s` (`Parent_Feature`)", aAttr.GetFTypeTable(pDBase));
         pDBase->ExecuteSQL(sSQL);               		        
      
         // Create reference to the parent feature

         if (BDFTypeParentI(hConnect, lFType, &ftypeP) && 
             BDFTypeAttrInit(hConnect, ftypeP.m_lId, &aAttrP))
         {
         
            sSQL.Format("alter table `%s` add constraint `parent` foreign key (Parent_Feature) references `%s`", 
                        aAttr.GetFTypeTable(pDBase), aAttrP.GetFTypeTable(pDBase));   
         } 
      };
   }  

   // Create SQL script for creating data table
   
    CString sOrder = "NRDB_ORDER";
    if (pDBase->GetDBVersion() < 3) sOrder = "ORDER";
    
    sSQL.Format("create table `%s` (`FEATURE_ID` integer not null, `LDATE` integer not null, `"+sOrder+"` integer not null", 
		             aAttr.GetFTypeInternal(pDBase));    

    for (int i = 0; bOK && i < aAttr.GetSize(); i++)
    {       
	    sColumn = ", " + ColumnAsString(hConnect, aAttr.GetFTypeInternal(pDBase), aAttr[i]);       
       sSQL += sColumn;	  	   
    }   

	// Add foreign key for features id
    
    BDFTypeI(hConnect, aAttr.m_lFType, &ftypeI);
#ifdef NRDBMYSQL
    sColumn.Format(",index (`FEATURE_ID`), constraint `%s_2` foreign key (`FEATURE_ID`) references `%s` (`ID`)", aAttr.GetFTypeInternal(pDBase), ftypeI.m_sInternal);    
#else
    sColumn.Format(",constraint `%s_2` foreign key (FEATURE_ID) references `%s`", aAttr.GetFTypeInternal(pDBase), ftypeI.m_sDesc);    
#endif
    sSQL += sColumn;

   // Add foreign keys for links

    for (i = 0; bOK && i < aAttr.GetSize(); i++)
    {
       CAttribute* pAttr = aAttr[i];
       if (pAttr->GetDataType() == BDLINK)
       {
          BDFTypeAttrInit(hConnect, pAttr->GetFTypeLink(), &aAttrLink);

#ifdef NRDBMYSQL
		    sColumn.Format(",index (`%s`), constraint `%d_%d_1` foreign key (`%s`) references `%s`(`ID`)", pAttr->m_sColName, pAttr->m_lFType, pAttr->m_lAttrId, pAttr->m_sColName, aAttrLink.GetFTypeTable(pDBase));		
#else
          sColumn.Format(",constraint foreign key `%s_%s_1` (`%s`) references `%s`", pAttr->m_sColName, pAttr->m_sColName, aAttr.GetFTypeInternal(pDBase), pAttr->m_sColName, aAttrLink.GetFTypeTable(pDBase));		
#endif 
       }
    };   

    sSQL += ");";

    // Execute SQL

    if (bOK)
    {
#ifdef _DEBUG
     FILE* pFile = fopen("d:\\temp\\debug.sql", "w");
       fputs(sSQL.GetBuffer(0), pFile);
       fclose(pFile);
#endif
       pDBase->ExecuteSQL(sSQL);  
       AddPrimaryKey(pDBase, aAttr);
    };

    // Add index

    sSQL.Format("create index `index_1` on `%s` (`FEATURE_ID`)", aAttr.GetFTypeInternal(pDBase));
    pDBase->ExecuteSQL(sSQL);               		        

    sSQL.Format("create index `index_2` on `%s` (`FEATURE_ID`, `LDATE`)", aAttr.GetFTypeInternal(pDBase));
    pDBase->ExecuteSQL(sSQL);               		        

    // In MySQL cannot rollback create/alter table so restart transactions

#ifdef NRDBMYSQL
   pDBase->BeginTrans();
#endif

    return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Updates an existing table, only adding or deleting columns that have changed
// thus retaining existing data
//

BOOL BDFTypeUpdate(BDHANDLE hConnect, long lFType, CAttrArray* pAttrOld)
{
	BOOL bOK = TRUE;
	CAttrArray aAttrNew;
	CString sSQL;
	CString s, sCol;

// Get database handle
      
   CNRDBase* pDBase = GetNRDBase(hConnect); 
   if (pDBase == NULL)
   { 
      bOK = FALSE;
   };   

  // Retrieve details of the existing attributes

   if (bOK)
   {
      bOK = BDFTypeAttrInit(hConnect, lFType, &aAttrNew);
   };

  // Remove existing primary key

   if (bOK)
   {      
      if (pDBase->IsJetDB())
      {
         sSQL.Format("alter table `%s` drop constraint `%s_1`", aAttrNew.GetFTypeInternal(pDBase), 
                    aAttrNew.GetFTypeInternal(pDBase));
      } else
      {
           sSQL.Format("alter table `%s` drop primary key", aAttrNew.GetFTypeInternal(pDBase));
      }
	   TRY
      {
		   pDBase->ExecuteSQL(sSQL);
      }
	   CATCH (CDBException, pEx)
      {        
         AfxMessageBox(pEx->m_strError);
      }
      END_CATCH
   };

  // For each attribute in the new list determine if it exists in the old

   for (int i = 0; bOK && i < aAttrNew.GetSize(); i++)
   {
      BOOL bFound = FALSE;
      for (int j = 0; j < pAttrOld->GetSize(); j++)
	  {
         if (aAttrNew[i]->m_sColName == pAttrOld->GetAt(j)->m_sColName)
		 {
			// Data type has changed so delete old column

			if (aAttrNew[i]->GetDataType() != pAttrOld->GetAt(j)->GetDataType() ||
             (aAttrNew[i]->GetDataType() == BDLINK &&
              aAttrNew[i]->GetFTypeLink() != pAttrOld->GetAt(j)->GetFTypeLink()))
			{
			  s = DataTypeAsString(pAttrOld->GetAt(i)->m_sColName, 
				                   pAttrOld->GetAt(i)->GetDataType());
			  do
			  {
               sCol = GetFirstCSV(s);
             
              if (pAttrOld->GetAt(i)->GetDataType() == BDLINK)
              { 
				  // New format, link constraints use featuretype and attribute name

#ifdef NRDBMYSQL
                  sSQL.Format("alter table `%s` drop constraint `%s_%s_1`", pAttrOld->GetFTypeInternal(pDBase), 
					          pAttrOld->GetAt(i)->GetFTypeId(), pAttrOld->GetAt(i)->GetAttrId());
#else
                  sSQL.Format("alter table `%s` drop constraint `%s_%s_1`", pAttrOld->GetFTypeInternal(pDBase), 
					          sCol.Mid(1,sCol.GetLength()-2), pAttrOld->GetFTypeInternal(pDBase));
#endif
				     TRY
				     {
				        pDBase->ExecuteSQL(sSQL);              
				     };
				     CATCH (CDBException, pEx)
				     {
					   bOK = FALSE;
					   //AfxMessageBox(pEx->m_strError);
				     }
				     END_CATCH       
              };			      
              
			     sSQL.Format("alter table `%s` drop column %s", pAttrOld->GetFTypeInternal(pDBase), sCol);				           
              pDBase->ExecuteSQL(sSQL);                 
              
			  } while (s != "");
			} else
			{
				bFound = TRUE;				
			}
			break;
		 }		 
	  }

	  // If not found (or deleted) then add column to table

	  if (!bFound)
	  {
		  // If definition contains more than one column then add one at
		  // a time

		  s = ColumnAsString(hConnect, aAttrNew.GetFTypeInternal(pDBase), aAttrNew[i]);                 

		  do
		  {			  
			  sCol = GetFirstCSV(s);
              
			  sSQL.Format("alter table `%s` add %s", aAttrNew.GetFTypeInternal(pDBase), sCol);
			  pDBase->ExecuteSQL(sSQL);                 			  
		  } while (s != "");
	  }    
   }

   // For attributes no longer used, delete them

   for (i = 0; bOK && i < pAttrOld->GetSize(); i++)
   {
	   BOOL bFound = FALSE;

	   for (int j = 0; j < aAttrNew.GetSize(); j++)
	   {
          if (aAttrNew[j]->m_sColName == pAttrOld->GetAt(i)->m_sColName)
		  {
			  bFound = TRUE;
			  break;
		  }
	   }

	   // Attribute has been deleted

	   if (!bFound)
	   {
		   s = DataTypeAsString(pAttrOld->GetAt(i)->m_sColName, 
			                    pAttrOld->GetAt(i)->GetDataType());
		   do 
		   {
			   sCol = GetFirstCSV(s);

               if (pAttrOld->GetAt(i)->GetDataType() == BDLINK)
			   {
                  TRY
                  {
                     sSQL.Format("alter table `%s` drop constraint `%s_%s_1`", pAttrOld->GetFTypeInternal(pDBase), 
					               sCol.Mid(1,sCol.GetLength()-2), pAttrOld->GetFTypeInternal(pDBase));
				      pDBase->ExecuteSQL(sSQL);              
                  } 
                  CATCH(CDBException, pEx)
                  {
                     // Try old style for backwards compatibility

                     sSQL.Format("alter table `%s` drop constraint `%s_%s_1`", pAttrOld->GetFTypeInternal(pDBase), 
					               pAttrOld->GetFTypeInternal(pDBase), sCol.Mid(1,sCol.GetLength()-2));
				         pDBase->ExecuteSQL(sSQL);              


                  }
                  END_CATCH
			   }
			   sSQL.Format("alter table `%s` drop column %s", pAttrOld->GetFTypeInternal(pDBase), sCol);
			   pDBase->ExecuteSQL(sSQL);              
		   } while (s != "");
	   }
   }

   // Update the primary key

   if (bOK)
   {      
      AddPrimaryKey(pDBase, aAttrNew);
   }
   
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

void AddPrimaryKey(CNRDBase* pDBase, CAttrArray& aAttr)
{
   CString sSQL;
   CString sKey = "FEATURE_ID, LDATE";

   for (int i = 0; i < aAttr.GetSize(); i++)
   {
       if (aAttr[i]->IsPrimaryKey()) sKey += ", " +
          DataTypeAsString(aAttr[i]->m_sColName, aAttr[i]->GetDataType());          
   };    
   sSQL.Format("alter table `%s` add constraint `%s_1` primary key (%s)", 
               aAttr.GetFTypeInternal(pDBase), aAttr.GetFTypeInternal(pDBase), sKey.GetBuffer(0));

   // TODO include attributes that are part of primary key
   pDBase->ExecuteSQL(sSQL);    
}

///////////////////////////////////////////////////////////////////////////////
//
// Deletes a table containing data for the corresponding data type
//

BOOL BDFTypeDelete(BDHANDLE hConnect, long lFType, int iFlag)
{
   BOOL bOK = TRUE;
   CString sSQL;
   CFeatureType ftype;
   CFeature feature;   
   CAttrArray aAttr;
   BOOL bTrans = FALSE;

   CNRDBase* pDBase = GetNRDBase(hConnect); 
   if (pDBase == NULL)
   { 
      bOK = FALSE;
   };   

   TRY
   {

      BDBeginTrans(hConnect);
      bTrans = TRUE;
  
   // Initialise attribute array

      BDFTypeAttrInit(hConnect, lFType, &aAttr);

   // Retrieve the feature type definition

      ftype.m_lId = lFType; 
      BDFeatureType(hConnect, &ftype, BDSELECT);
      BDEnd(hConnect);
 
   // Delete data

      if (bOK && iFlag & BDDELDATA && !(iFlag & BDDELFTYPE))
      {
         sSQL.Format("delete from `%s`;", aAttr.GetFTypeInternal(pDBase));         
         pDBase->ExecuteSQL(sSQL);     
      }

   // Delete features

      if (bOK && iFlag & BDDELFEATURE && !(iFlag & BDDELFTYPE))
      {
         sSQL.Format("delete from `%s`", aAttr.GetFTypeTable(pDBase));         
         pDBase->ExecuteSQL(sSQL);     
      }
     
   // Commit changes

     if (bOK)
     {
        BDCommit(hConnect);
        bTrans = FALSE;
     }

   // Drop the associated table

   if (bOK && iFlag & BDDELFTYPE)
   {                
      sSQL.Format("drop table `%s`;", aAttr.GetFTypeInternal(pDBase));         
      pDBase->ExecuteSQL(sSQL);           
   };  

   // Drop the feature table

   if (bOK && iFlag & BDDELFTYPE)
   {
      // Version 3 databases don't create a feature table for one to one relationships
      if (ftype.m_lParentFType == 0 || ftype.m_bManyToOne || pDBase->GetDBVersion() < 3)
      {
          sSQL.Format("drop table `%s`;", aAttr.GetFTypeTable(pDBase));         
          pDBase->ExecuteSQL(sSQL);     
      };
   }
      
         
   }
   CATCH (CDBException, pEx)
   {
      bOK = FALSE;
      AfxMessageBox(pEx->m_strError);
   }
   END_CATCH       

   // Delete the associated feature type

   if (bOK && iFlag & BDDELFTYPE)
   {
      ftype.m_lId = lFType;
      bOK = BDFeatureType(hConnect, &ftype, BDDELETE);
      BDEnd(hConnect);
   }

   // Handle transactions

   if (!bOK && bTrans) BDRollBack(hConnect);

   // In MySQL cannot rollback create/alter table so restart transactions

#ifdef NRDBMYSQL
   pDBase->BeginTrans();
#endif
       
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Note: Access ODBC supports 'longbinary' and MySQL ODBC supports 'long varbinary'

CString ColumnAsString(BDHANDLE hConnect, CString sFType, CAttribute* pAttr)
{	
   CString s;
   CAttrArray aAttrLink;
   
   CNRDBase* pDBase = GetNRDBase(hConnect); 
   ASSERT(pDBase != NULL);
   
   // MSAccess name for long binary objects is non-standard
   
   CString sLongBinary = "longbinary";

   // MySQL longbinary objects are thought to be OLE objects under NT
   if (!pDBase->IsJetDB()) sLongBinary = "longblob"; 
   
   long lDataType = pAttr->GetDataType();
   CString sName = pAttr->m_sColName;   

   // Number
	if (lDataType == BDNUMBER)
   {
      return "`"+ sName + "` float";
   }
   // Text
   else if (lDataType == BDTEXT) 
   {
      s.Format("%i", BD_SHORTSTR);
      return "`"+ sName + "` varchar ("+ s + ")";	
   }
   
   else if (lDataType == BDLONGTEXT)
   {
	   return "`"+ sName + "` longtext";	
   }
   else if (lDataType == BDMAPLINES || lDataType == BDIMAGE || lDataType == BDFILE) return "`"+ sName + "` " + sLongBinary;	
	else if (lDataType == BDCOORD) return "`"+ sName + "_x` float, " + "`"+ sName + "_y` float";
	else if (lDataType == BDBOOLEAN) return "`"+ sName + "` smallint";
   else if (lDataType == BDDATE) return "`"+ sName + "` integer";
   // Link
	else if (lDataType == BDLINK)
	{           
      return "`"+ sName + "` integer";
	} 
   // File
   else if (lDataType == BDHOTLINK)
   {
      s.Format("%i", BD_SHORTSTR);
      return "`"+ sName + "` varchar ("+ s + ")";	
   }
	
	
	
	ASSERT(FALSE);

	return "";
}

///////////////////////////////////////////////////////////////////////////////
//
// Returns the column names in the format [a],[b] This allows formats involving
// multiple columns to be handled
//

CString DataTypeAsString(CString sName, long lDataType)
{
	if (lDataType == BDCOORD)
	{
		return "`"+ sName + "_x`, " + "`"+ sName + "_y`";
	}
	else
	{
		return "`"+sName+"`";
	}

	return "";
}

///////////////////////////////////////////////////////////////////////////////

CString GetFirstCSV(CString& s)
{
   CString sRet;
   BOOL bInside = FALSE;

   for (int i = 0; i < s.GetLength()-1; i++)
   {
      if (s[i] == '`') bInside = !bInside;
      if (s[i] == ',' && !bInside) break;
   }

   if (i < s.GetLength()-1)
   {
     sRet = s.Left(i);
     s = s.Mid(i+1);
   }
   else
   {
      sRet = s;
      s = "";
   };

   return sRet;
}
