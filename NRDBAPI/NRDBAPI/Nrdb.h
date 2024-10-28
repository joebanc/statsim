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

//define export
#ifndef DLLEXPORT
	#define DLLEXPORT __declspec(dllexport)
#endif
#ifndef DLLIMPORT
	#define DLLIMPORT __declspec(dllimport)
#endif

#ifndef _NRDBAPI_H_
#define _NRDBAPI_H_

#include "maplines.h"
#include "attribute.h"

// Data access constants 

#define BDGETINIT              1          // Prepare query and get 1st value in data set 
#define BDGETNEXT              2          // Get next value in data set 
#define BDADD                  4          // Add to data set 
#define BDUPDATE               3          // Update data set 
#define BDDELETE               5          // Delete from data set 
#define BDINSERTINIT           16         // Prepare insert and insert 1st row in data set 
#define BDINSERTNEXT           17         // Add next row in data set 
#define BDSELECT               19         // Do a query that will return a subset of the data
#define BDSELECT2              20         // Do a query that will return a subset of the data
#define BDSELECT3              25         // Do a query that will return a subset of the data
#define BDSELECT4              26         // Do a query that will return a subset of the data
#define BDSELECT5              28         // Do a query that will return a subset of the data 
#define BDSELECTLATEST         29         // Do a query that will return the latest data
#define BDSELECTEARLIEST       30         
#define BDDELETEALL            27         // Delete all records of a given type

#define BDINTERNAL           1024         // Use internal handle

#define BDDELDATA              0x0001     // Parameters for BDFTypeDelete
#define BDDELFEATURE           0x0002
#define BDDELFTYPE             0x0004

// Messages

#define BDRESETPROGRESS WM_USER+1
#define BDUPDATEPROGESS WM_USER+2

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Connection handle type

typedef HANDLE BDHANDLE;

///////////////////////////////////////////////////////////////////////////////

class DLLEXPORT CFeatureType
{
public:
   CFeatureType();

   long m_lId;  
   CString m_sDesc;
   CString m_sInternal;
   long m_lParentFType;
   BOOL m_bManyToOne;
   long m_lDictionary;
};

typedef CArray <CFTypeAttr, CFTypeAttr> CFTypeAttrArray;

struct CFTypeMap
{
   BOOL m_bMap;
   long m_lFType;
};

class DLLEXPORT CFeature
{
public:
   CFeature();
   CFeature(CFeature&);

   long m_lId;
   long m_lFeatureTypeId;
   CString m_sName;
   long m_lParentFeature;
};

class DLLEXPORT CDictionary
{
public:
   CDictionary();

   long m_lId;
   CString m_sDesc;
};

class DLLEXPORT CBDMain
{
public:
   CString	m_Country;
   CString	m_Province;
   CString	m_City;
	CString	m_Organization;
	CString	m_Address;
	CString	m_Telephone;
	CString	m_Fax;
	CString	m_Email;
   CString  m_Logo;
   int      m_nVersion;
   BOOL     m_bCoordAsLatLon;
   BOOL     m_bCustomHeader;
   BOOL     m_bCustomFooter;
   CString  m_sCustomHeader;
   CString  m_sCustomFooter;
   CString  m_sStyleSheet;
};

class DLLEXPORT CBDProjection
{
public:
   long	m_nID;
	CString	m_sName;
	double	m_dSemiMajorAxis;
	double	m_dSemiMinorAxis;
	double	m_dLatitudeOrigin;
	double	m_dLongitudeOrigin;
	double	m_dFalseEasting;
	double	m_dFalseNorthing;
	double	m_dScaleFactorAtOrigin;
   long     m_lScaleCoords;
   double   m_dMinEasting;
   double   m_dMaxEasting;
   double   m_dMinNorthing;
   double   m_dMaxNorthing;
	BOOL	   m_bDefault;

   CBDProjection();
};

class DLLEXPORT CBDSymbol
{
public:
   long m_lId;
   CLongBinary m_symbol;
};

enum DLLEXPORT OnError {DisplayMessage,ThrowException};

///////////////////////////////////////////////////////////////////////////////

extern "C"
{
BOOL DLLEXPORT BDConnect(LPCSTR psDBase, BDHANDLE* pDBHandle, LPCSTR sUser = NULL, LPCSTR sPassword = NULL, LPCSTR sHost = NULL);
BOOL DLLEXPORT BDDisconnect(BDHANDLE);
BOOL DLLEXPORT BDFeatureType(BDHANDLE hConnect, CFeatureType* pFeatureType, int iFlag);
BOOL DLLEXPORT BDFTypeAttr(BDHANDLE hConnect, CFTypeAttr* pFTypeAttr, int iFlag);
BOOL DLLEXPORT BDFeature(BDHANDLE hConnect, CFeature* pFeature, int iFlag);
BOOL DLLEXPORT BDDictionary(BDHANDLE, CDictionary* pDictionary, int iFlag);
BOOL DLLEXPORT BDMain(BDHANDLE hConnect, CBDMain* pMain, int iFlag);
BOOL DLLEXPORT BDProjection(BDHANDLE hConnect, CBDProjection* pProjection, int iFlag);
BOOL DLLEXPORT BDSymbol(BDHANDLE hConnect, CBDSymbol *pSymbol, int iFlag);
BOOL DLLEXPORT BDGetNext(BDHANDLE hConnect);
BOOL DLLEXPORT BDEnd(BDHANDLE hConnect);
BOOL DLLEXPORT BDNextId(BDHANDLE hConnect, int nType, long lId, LPLONG plNextId);

BOOL DLLEXPORT BDFTypeCreate(BDHANDLE hConnect, long lFType);
BOOL DLLEXPORT BDFTypeUpdate(BDHANDLE hConnect, long lFType, CAttrArray* pArray);
BOOL DLLEXPORT BDFTypeDelete(BDHANDLE hConnect, long lFType, int iFlag);

BOOL DLLEXPORT BDFTypeParentI(BDHANDLE hConnect, long lFType, CFeatureType*);
BOOL DLLEXPORT BDFTypeI(BDHANDLE hConnect, long lFType, CFeatureType*);

BOOL DLLEXPORT BDFTypeAttrInit(BDHANDLE hConnect, long lFType, CAttrArray* pArray);

BOOL DLLEXPORT BDAttribute(BDHANDLE hConnect,  CAttrArray* pArray, int iFlag);
/*BOOL BDAttributeRange(BDHANDLE hConnect, CAttrArray* pArray, long lFeatureMin, long lFeatureMax, int iFlag = BDGETINIT);*/
BOOL DLLEXPORT BDAttributeCond(BDHANDLE hConnect, CAttrArray* pArray, long lFeatureMin, long lFeatureMax, long lDateStart, long lDateEnd, LPCSTR sFilter, int iFlag);

BOOL DLLEXPORT BDBeginTrans(BDHANDLE hConnect);
BOOL DLLEXPORT BDCommit(BDHANDLE hConnect);
BOOL DLLEXPORT BDRollBack(BDHANDLE hConnect);

void DLLEXPORT BDOnError(BDHANDLE hConnect, int);

void DLLEXPORT BDSetActiveWnd(HWND);

void DLLEXPORT BDGetError(BDHANDLE hConnect, CString&);

int DLLEXPORT BDGetTransLevel(BDHANDLE);

BOOL DLLEXPORT BDInitialise(LPCSTR sDSN, LPCSTR sUser = NULL, LPCSTR sPassword = NULL, LPCSTR sHost=NULL);

BOOL DLLEXPORT BDGetDriver(BDHANDLE, CString&);

int DLLEXPORT BDGetVersion(BDHANDLE hConnect);

BOOL DLLEXPORT BDGetDataSourceNames(HANDLE &henv, CString &sDSN, LPCSTR sUser = NULL, LPCSTR sPassword = NULL, LPCSTR sHost=NULL);

BOOL DLLEXPORT BDCreateDatasource(LPCSTR sDSN, LPCSTR sPath, LPCSTR sUser = NULL, LPCSTR sPassword = NULL, LPCSTR sHost = NULL);

}


#endif
