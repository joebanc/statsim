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

#ifndef _IMPORTODBC_H_
#define _IMPORTODBC_H_

#include "maplayer.h"

#define IMPORT_SKIP BDString(IDS_DONTIMPORT)
#define IMPORT_DATE BDString(IDS_DATE)
#define IMPORT_NAME " [" + BDString(IDS_NAME) + "]"
#define IMPORT_CREATE BDString(IDS_IMPORTCREATE)
#define IMPORT_ID BDString(IDS_ID)

#define X_COORD 0x0000
#define Y_COORD 0x0001
#define LAT_COORD 0x0002
#define LON_COORD 0x0003
#define IMP_COORD 0x0004
#define IMP_MAPLINE 0x0020

#define X_COORDS " [" + BDString(IDS_X) + "]"
#define Y_COORDS " [" + BDString(IDS_Y) + "]"
#define LAT_COORDS " [" + BDString(IDS_LATITUDE) + "]"
#define LON_COORDS " [" + BDString(IDS_LONGITUDE) + "]"
#define IMP_COORDS " [" + BDString(IDS_COORDTYPE) + "]"
#define IMP_MAPLINES " [" + BDString(IDS_POLYLINESTYPE)+ "]"

#define SHAPEFILE_POINTS BDString(IDS_SHAPEFILE) + " [" + BDString(IDS_POINTS) + "]"
#define SHAPEFILE_POLYLINES BDString(IDS_SHAPEFILE) + " [" + BDString(IDS_POLYLINESTYPE) + "]"

///////////////////////////////////////////////////////////////////////////////

class CImportDB;
class CDlgProgress;

///////////////////////////////////////////////////////////////////////////////

struct CPolylineAttr
{
   CMapLayer m_maplayer;
   CAttrArray m_aAttr;
   
   CPolylineAttr& operator=(CPolylineAttr& rSrc)
   {
       ASSERT(rSrc.m_maplayer.GetSize() == 0);       
       m_aAttr = rSrc.m_aAttr;
       return *this;
   }   
};

///////////////////////////////////////////////////////////////////////////////

struct CImportColumn
{
   CString m_sNameImport;
   CString m_sNameAttr;   
   BOOL m_bAttr;
   char m_sText[BD_SHORTSTR];
   BOOL m_bShapeFile;
   long m_lItemData;
   
   CImportColumn();   
   CImportColumn(CImportColumn&);      

   void Write(FILE*);
   int Read(FILE*);
};

struct CImportFeature
{
   CImportFeature();

   CString m_sFeature;
   long m_lFeature;
   long m_lFType;
};

struct CImportFTypeI
{
   long m_lFType;
   long m_lFTypeI;
};

struct CImportFTypeParentI
{
   long m_lFType;
   long m_lFTypeParentI;
};

struct CImportPrimaryKey
{   
   CString m_sId;
   double m_dId;   
   long m_lFeature;
};

struct CImportTable
{
   CString m_sTableImport;   
   CString m_sFType;
   BOOL m_bImport; 
   CString m_sFeature;
   CString m_sParentFeature;
   CString m_sGrandParentFeature;
   CString m_sGreatGrandParentFeature;
   CString m_sShapeFile;
   CFeatureType m_ftype;
   BOOL m_bAutoCreate;
   //long m_lId; // Used for NRDB databases   
   CArray <long, long> m_alCreateAll;      
   CImportDB *m_pImportDB;
   int m_nRecord;   
   int m_nPrimaryKey;
   int m_nDataType;

   CArray <CImportPrimaryKey,CImportPrimaryKey> m_aPrimaryKey; 

   static void CleanString(LPSTR);
   static void CleanString(CString&);

   CArray <CImportColumn,CImportColumn> m_aColumn;

   CImportTable();
   CImportTable(CImportTable&);
   CImportTable& operator=(CImportTable&);
   void Write(FILE*);
   int Read(FILE*);

   BOOL Import(CDatabase& database, CImportDB& importDB, int nFileType);

   enum {DoNotImport=-999,Undefined=0,ImportCreate=-998

   };

protected:

   enum {Add=1, Update=2};
 
   int GetAttr(CAttrArray& aAttr, CString sAttr);
   long DetermineFeature(long lFType, CString, LPCSTR, LPCSTR, LPCSTR, BOOL bParent = FALSE);
   long DetermineFeature(long lFType, CString sId, CArray <CImportColumn,CImportColumn>& aColumns);
   BOOL DetermineNumber(CImportDB&, CAttrArray&);
   BOOL DetermineLink(CImportDB&, CAttrArray&);
   BOOL DetermineBool(CImportDB&, CAttrArray&);
   BOOL DetermineLatLon(CImportDB&, CAttrArray&);
   int  DetermineShapefile(CAttrArray&, CMapLayer*);   
   void RetrieveLongText(HSTMT, CAttrArray&);
   int  MergeShapefile(CAttrArray&, CLongLines*);
   BOOL WriteShapefile(CImportDB&);

   long CreateFeature(long lFType, LPCSTR, LPCSTR, LPCSTR, LPCSTR, long lParent);
   long GetFTypeI(long lFType);
   long GetFTypeParentI(long lFType);

   long GetCountRecords(HDBC hdbc, LPCSTR sTable);
   
   BOOL LoadShapefile(CImportDB&, CMapLayer &maplayer);

   CString GetShortFileTitle(CDatabase&);

   CArray <CImportFeature, CImportFeature> m_aImportFeature;   
   CArray <CImportFTypeI, CImportFTypeI> m_aImportFTypeI;
   CArray <CImportFTypeParentI, CImportFTypeParentI> m_aImportFTypeParentI;

   CArray <long, long> m_alAttrM; // Attribute indexes to be merged

   CArray <CPolylineAttr, CPolylineAttr> m_aAttrShapefile;
   long m_lAttrIdShapefile;   
};

///////////////////////////////////////////////////////////////////////////////

class CImportDB : public CArray <CImportTable, CImportTable>
{
public:

   CImportDB();
   
   void ImportODBC(LPCSTR sDataSource = NULL, LPCSTR sTable = NULL, BOOL bSysTables = FALSE, int nFileType = -1);	

   void Write(LPCSTR sFileName);
   BOOL Read(LPCSTR sFileName);

   CImportDB& operator=(CImportDB&);

   int Find(LPCSTR);
  
   static void CompactDatabase();

   enum {excel, access, dbase, shapefile, text};

   // Attributes

   FILE* m_pLogFile;
   int m_nImported;
   int m_nFetched;
   CDlgProgress* m_pDlgProgress;

protected:

   void Import(CImportTable& table);      

   CString m_sLogFile;


  
};

#endif
